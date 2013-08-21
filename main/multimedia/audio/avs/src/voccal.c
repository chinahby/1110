/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audio Calibration Database")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           V O C O D E R   C A L I B R A T I O N   D A T A B A S E
              D A T A   S T O R A G E  A N D   R E T R I V A L

DESCRIPTION
  The actual vocoder calibration values and the functions to gain access
  to the data.  This is version is based on constant ROM data.

EXTERNALIZED FUNCTIONS
  voc_cal_get_pcm_cal
    Get a pointer to pcm calibration structure.
  voc_cal_get_codec_config
    Get a pointer the codec configuration data.
  voc_cal_get_dfm_config
    Get a pointer to DFM configuration.
  voc_cal_get_dfm_dev_limit
    Get the DFM deviation limit.
  voc_cal_get_mic_gain_adjust
    Get the mic gain adjustment value for the specified codec.
  voc_cal_set_mic_gain_adjust
    Set the mic gain adjustment value for the specified codec.
  voc_cal_read_nv_cal
    Reads all NV items related to voc calibration.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 1999 - 2009 by QUALCOMM, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/voccal.c#49 $ $DateTime: 2009/10/26 01:58:06 $ $Author: nrchella $

when       who     what, where, why
--------   ---     ---------------------------------------------
10/26/09   nr      Audio Calibration update for SURF GSM WB mode for all 6K targets.
10/23/09    sj     Added SCMM specific changes for WB codec config.
10/08/09   knm     Integrated new marimba codec driver for SCMM
10/07/09   rm      Updated IIR coefficients for Dual MIC case(6800)
10/06/09   sj      Added SCMM specific changes
09/22/09   nr      Translation of voc_codec_mode_type to voc_cal_codec_cfg_type
                   to avoid invalid memory access (CR 190810).
06/24/09   lp      CR 185765 fix,sidetone gain set to 0 for speakerphone mode
06/10/09   nr      The Tx and Rx PCM IIR filters are disabled for GSM WB SURF Handset mode;
		   The Tx and Rx HPF filters are enabled for GSM WB SURF case
06/08/09   rm      Removed handling of MSMAUD_QSC6270_AUDIO_CAL
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
05/29/09   pk      Added new ADRC calibration values for ULC and other single chip targets
                   Disable audfmt IIR Filter for headset/stereo headset
                   Added comments for RVE parameters
05/23/09   sud     Audio Calibration update for ULC and SC2X.
                   Updated Copyright.
05/23/09   sud     Audio Calibration update for LCU.
05/22/09   nr	   Added WB calibration tables and new SURF devices (CR#177834,#173500)
05/13/09   rm      EVRC-WB and IIR filter table related updates
05/05/09   knm     Defined CONSTANT macro to dynamically change adie config.
04/14/09   skr     Providing ADRC tables and IIR tables for QTUNES and SYNTH
                   for all sample rate and device ( stereo headset, stereo usb,
                   speaker, other devices ) combinations , < Added the comment>
04/06/09   av      Adding support for TxPCM filter before EC.
                   Enabled HPF and Slope filters for Fluence through externalSADC
                   Copy right updated.
03/10/09   sj      Added slowtalk feature support
02/18/09   sj      Reduced the rxVolume for Full mode and VCO TTY
                   codecs for LCU. Fixed CR 170574
02/05/09   kk      Fixed CR 165405 - Phone Crashes "Error Fatal SND : Pfault 0"
                   when L4 Read Faults are enabled in the build.
01/29/09   sj      Ported CR 137990 fix to mainline
01/19/09   az      Adding support for stereo AAC recoding for camcorder app.
12/30/08    rm     Adding fluence parameters
12/22/08   sj      value of codecTxGain for voc_pcm_tty_on_chip_1_gsm_cal and
                   voc_pcm_tty_hco_gsm_cal are changed to 0x800 to avoid
                   saturation of baudot tones in Tx Path.
12/08/08   rm      Modification of wnr and rve parameters
05/13/08   sj      Added New seperate ADIE config for TTY headset.
                   Fixed CR 144204
11/18/08   knm     Added Macro for HS-USB along with USB car kit
11/17/08   rm      Fix provided for  CR 163916 - dualmic: muted uplink and downlink  and
                   updation of calibration sections.
11/11/08   rm      SURF 6800 calibration values updated from depot code.
11/03/08   dp      Correcting featurization issue for FM over BT
10/22/08   dp      FM over BT AG changes
10/14/08   sk      Modifed the code to update the HS mode value to
                   0x14FF for WB spekerphone.CR 154396.
30/08/08   vsud    Check in of RVE changes on behalf of vocoder team.
28/08/08   rm	   Added support for RVE
08/07/08   av      Added support for CrystalSpeech using EXTERNAL_SADC
04/11/08   sj      Lowered the codec Tx gain for voc_pcm_tty_on_chip_1_cal
                   and voc_pcm_tty_hco from 0x4000 to 0x800 for 6270/6240.
                   This will prevent Tx baudot to be saturated. fixed CR 151381.
05/08/08   pl      CR 143215 - Updated qsynth adrc paramter table for all supported
                   sample rates.
04/10/08   sp      Added all pass conefficients for 5 stage IIR filters that did
                   not have any coefficients. Fixed no audio issue for AMR WB
                   call on speakerphone and BT headset.
03/17/08  ymc/anb  Modified codec gain ctrl api to control gain changes.
                   Fix for SC2X codec pop issue through MSMAUD_PCM_DC_OFFSET feature.
02/07/08   vs      Added cal data for LCU (SURF and FFA)
12/06/07   anb     Fixes CR 129526-Updated the cal values for Handset on SC2X
10/19/07   anb     Updated the cal values for Headset on SC2X.
10/03/07   anb     Modified Rx PCM filter for Speaker Phone on SC2X
08/27/07   ymc     Added Crystal Speech support.
08/27/07  sg/wfd   Define unique OFF_BOARD AuxPCM Coded settings for 6800A-
                   like targets.  Cleanup obsolete code for older targets.
                   Specifically, #else code for MSMAUD_MSM6050_CODEC_CONFIG.
08/23/07   ymc     Added wideband EC parameters.
           ymc     Updated 5 stages IIR filter parameters for handset and
                   headset.
08/07/07    sk     Updated M6800 65nm IIR filter parameters.
07/27/07   ymc     Updated 5 stages IIR filter parameters
                   for handset and headset.
06/21/07   anb     Updated the QSC60x5 calibration values from system's team.
06/15/07   anb     Updated QSC60X5 codectxgain to support Q factor change in firmware
05/22/07   anb     Updated QSC60x5 Audio Calibration
05/14/07   sud     EPIC Vreg_NCP support in ground referenced capless mode
                   for QSC60x5 chipsets.
05/10/07    sp     Added support for AUX PCM primary PCM mode.
                   Added support for external SOC based BT solution that needs
                   primary pcm mode.
04/25/07   anb     Fixed a compilation issue when FEATURE_MM_REC is defined
04/20/07  sud/anb  Added QSC60X5 FFA audio calibration.
04/20/07  sp/anb   Added AUX PCM definitions for the SC2X hardware.
02/27/07   sud     Added 6800 65nm audio calibration.
02/27/07   ymc     Removed 6260 dependencies on new EC calibrations.
           ymc     Updated Copyright.
02/26/07   kan     Added AMR-WB changes
02/23/07   suk     Updated 6260 Audio Calibration. CR#110642
01/23/07   ymc     Correct new EC parameters naming.
           ymc     Updated Copyright.
01/23/07    kk     Fixed the issues related to adding 44k sample rate.
12/04/06   ymc     Added VOC_EC_BT_HEADSET support.
           ymc     Used VOC_EC_BT_HEADSET for HFK device.
11/23/06   suk     Removed the definition QDSP_INTERNAL_PCM_FORMAT_1 for 6260.
11/21/06   suk     Updated 6260 FFA audio calibration and added comments.
11/16/06   suk     Added 6260 FFA audio calibration.
11/10/06    az     Fixed the CR: 105868 In-Call MP3: Noise heard on 6260 FFA during
                   In-Call MP3 while in MONO Mode
10/05/06   suk     Added 6260 FFA audio calibration values for new EC (headset
                   only).
                   Fixed DENS_gamma_e_high placement in the handset, headset,
                   handsfree, and speaker structures.
09/11/06    ay     Fixed 6280 and QSC60X0 to use the same PCM filter
                   coefficients.
09/08/06    pl     Added support for 44K clock.
08/31/06    ay     Added 6280 FFA audio calibration.
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
07/26/06  nm/ymc   Added FEATURE_AVS_NEXTGEN_EC.
          nm/ymc   Corrected speaker EC calibrations for new EC when
                   FEATURE_AVS_NEXTGEN_EC is defined .
07/20/06   anb     Added support for FEATURE_AVS_AUDIO_DECODER_SCO
07/18/06   act     Updated QSC60x0 audio calibration.
07/10/06   act     Added hssd parameter to codec calibration data.
                   Also added missing codecs to voc_cal_mic_gain_adjust[]
                   and voc_cal_codec_vr_types[].
04/11/06   act     Added QSC60x0 FFA audio calibration.
03/24/06    pl     Seperated FIR filter from FEATURE_QSYNTH_ADRC and
                   FEATURE_QTUNES_AdRC.
03/15/06   ymc     Added FEATURE_INTERNAL_SADC support.
03/11/06   ymc/ay  Fixed mic muted when recording with speaker enabled.
03/10/06   sm/sp   Fixed a featurization issue with SDAC
02/14/06   ymc     Added 6280 FFA audio calibration.
                   Updated QTunes PCM filter control table.
                   Fixed BT_AG featurization in MM_REC adie config table.
                   Updated Copyright.
01/30/06    sp     Added support for I2S SBC interface.
12/02/05   act     Re-featurizing stereo-related codecs and sound devices.
10/08/05    sp     Added the feature MSMAUD_6500_FFA_AUDIO_CAL.
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05    st     Added output_3d_mode entries to voc_pcm_path_cal_type
                   structures.
09/02/05    sm     Modified stereo USB cal to use mono spkr+mic for voice
08/31/05    sm     Use voc_data_get_codec_mode()
08/10/05    sm     Featurized stereo USB support.
                   Adjusted speakerphone gains for better EC performance.
07/28/05    sm     Fixed missed entry on stereo USB support.
07/26/05    sm     Added stereo usb support
07/05/05    aw     Modified qsynth pcm filter for backward compatiblity.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/25/05    sm     Added GSM TTY calibration tables.
05/24/05    hz     Added support for MultiMedia/AAC record functionality.
05/04/05    aw     Replace PCM filter with IIR filter support for Qsynth.
04/22/05    aw     Clean up for audio side chain filter.
04/21/05   act     Changed name of voc_codec_type to voc_codec_class_type.
                   Reverted name of voc_codec_id_type to voc_codec_type.
04/01/05   act     Added tables and access functions to associate a specific
                   codec with a generic codec type.
04/01/05    hz     Added support for Enhanced AAC Plus decoder.
03/02/05    aw     Added support for side chain tuning filters.
02/15/05   act     Added EC parameter calibration structures.
02/15/05    sm     Added FEATURE_IIR_FILTER
02/14/05    hz     Added support for IMAGE_WM.
02/10/05    aw     Added support for T_FFA, since FEATURE_FFA will no longer be
                   used on newer targets.
01/20/05    sg     Modified to support MIDI on BT_AG codec.
01/07/05  sm/act   Added support for FEATURE_VOC_MIC_GAIN_ADJUST and
                   FEATURE_VOC_NV_CAL
12/03/04    sm     Removed FEATURE_USB_ISOC_AUDIO
12/01/04    aw     Added support for MP4D_AAC_NO_VPP image.
11/19/04    sg     Added support for QTV and QTV_AAC images.
10/15/04    hz     Added support for QDSP_IMAGE_MP4D_AAC_PLUS.
09/22/04    aw     Added support for in call viewfinder.
08/19/04    sm     Added FEATURE_AUDIO_CAL_MED_HIGH_FFA.
06/01/04    aw     Added support for H264 image.
05/11/04    vp     Merged ADRC and PCM filter support MP3/AAC from
                   MSMSHARED_AVS.00.09.05.02.00. Added support for H263 images.
04/01/04    sm     Audio gains for 6250 under FEATURE_AUDIO_CAL_MED_HIGH.
12/03/03    aw     Added audio support for QGL image.
11/24/03    aw     Modified code to support qsynth ADRC and PCM filter at
                   different sampling rate. Fix featurization on stereo headset.
11/22/03    sg     Added support for RealAudio codec.
10/30/03    sm     Added multimode calibration support.
10/10/03    sg     Added support for MP4D_MP3 image.
09/23/03    sm     Increased all audio gains for 6300 under
                   FEATURE_AUDIO_CAL_HIGH
08/21/03    aw     Added adie codec config for BT intercom device, CR#29347.
            st     Added definitions for BT intercom device.
07/24/03    sg     Added support for stereo headset device.
05/13/03   ymc     Corrected PCM format definitions for UMTS FFA.
04/18/03    aw     Added audio support for Q3D image.
04/17/03    st     Added support for DFM_SAT_DC_OFFSET.
03/13/03    st     Added support for MP4D+AAC image.
03/12/03    st     Modified BT AG codec config to use off board setting.
01/25/03    st     Disabled echo canceller for all TTY devices as recommended.
01/05/03    sg     Modified voc_cal_get_codec_config() to check for invalid
                   image type.
12/03/02    st     Modified code to use vocoder codec config settings as
                   default.
11/06/02    sm     Include msmaud.h.
10/30/02    aw     Fixed featurization.
10/29/02  st/sg    Added calibration values for speaker phone. Reorganized
                   ADIE config data so that it is no longer part of
                   voc_pcm_path_cal_type. ADIE config can be obtained by clients
                   using voc_cal_get_adie_config().
10/17/02   ymc     Redo audio calibration for UMTS FFA and SURF.
09/18/02    sm     Added in MSMAUD_ALT_BT_AG_CONFIG and MSMAUD_PCM_FORMAT_DAI.
                   Look for QDSP_IMAGE_BT_DEFINED when referring to f/w image.
09/16/02    st     Removed reference to QDSP_IMAGE_GPS; modified featurization
                   for QDSP_CODEC_CTRL_WB_AMP_1 and QDSP_CODEC_CTRL_WB_AMP_2.
09/06/02    sg     Moved ADIE config to msmaud.h; fixed BT headset config
                   values.
08/22/02    sg     Modified code to support FEATURE_EXTERNAL_SDAC and
                   FEATURE_INTERNAL_SDAC; added support for
                   QDSP_IMAGE_VOC_DEFINED; removed reference to QDSP_IMAGE_BT.
08/19/02    st     Changed VOC_DFM_SAT_AGC_NOM to 0xe6 for FFA and SURF.
08/13/02   sg/st   Added support for MP4 and AAC.
07/09/02    sm     Synced to perforce tip.
            sm     Replaced MSMHW_5200 by T_MSM5200.
           ymc     Replaced T_MSM5200 by MSMHW_MSM5200.  Removed T_KOVU.
06/13/02    st     Updated ADRC values for 32kHz synth.
06/11/02    st     Modified code to support FEATURE_HWTC.
03/21/02    st     Updated cal values; updated ESEC and NS settings for TTY
                   devices.
03/12/02    st     Modified code to add MSM6050 support; corrected the codec
                   configuration values for MSM6050; updated ADIE codec config
                   sequences to remove pops; added codecRxGain values in
                   cal tables.
03/04/02   jkl     Added FEATURE_VR_PUFE.
01/15/02   ymc     Added GSM and WCDMA support.  Added FEATURE_UMTS_FFA.
           ymc     Updated copyright.
12/11/01    aw     Added MSMAUD_ADIE_CODEC_CONFIG. Updated codec config command.
            aw     Added defines for MSM6050 codec config command.
            ro     Use msmaud.h to eliminate targetization.
12/05/01    sg     Mic gain changed to P6 and CODEC sidetone gain set to 0 for
                   VCO mode. PCM slope filter disabled for HCO mode.
10/04/01    sg     Added VCO and HCO modes for TTY.
10/01/01    sm     Added FEATURE_BT_AG_LOCAL_AUDIO.
09/11/01    st     Updated default values for ADRC and PCM filter settings for
                   QSynth image.
09/04/01    aw     Added FEATURE_QSYNTH_ADRC. Added defines for QSynth PCM
                   filter and ADRC data structure in each PCM paths.
08/28/01    st     Modified value of VOC_DFM_AUDIO_DEF in order to mute Tx
                   and disable SAT transponding.
07/31/01  sm/ro    Change TX volume for AUX PCM HFK to pass DFM tests.
                   Changed voc_cal_codec_tty_off_board for external PCM.
06/18/01  jkl/ym   Added FEATURE_ACP.
05/21/01   jkl     Removed Enhanced VR feature.
05/02/01    sm     Removed unnecessary TTY codec.
04/26/01    sm     Added FEATURE_USB_ISOC_AUDIO support.
04/11/01    sm     Calibrated 5105 handset AAGC.
03/08/01    sm     Corrected another TTY featurization error.
03/04/01    sm     Added codec config for QSynth and QTunes for off board
                   codec (digital HFK); removed NULL entries in codec config
                   table.
02/28/01    st     Removed support for 16K codec types; added proper
                   featurization to voc_cal_codec_cfg; changed
                   FEATURE_AUDIO_FORMAT to use FEATURE_QSYNTH || FEATURE_QTUNES;
                   modified codec configuration to enable wideband compensation
                   filter for QTunes and QSynth internal codec configurations.
02/22/01    sm     Renamed FEATURE_GPSONE to FEATURE_GPS_MODE.
02/21/01    vl     Added MSM5100 support.
12/10/00    st     Fixed #elif statement; modified TX DTMF gain to 0x1000.
12/05/00   jkl     Added FEATURE_ENHANCED_VR.
12/04/00  st/sm    Added QTUNES support, updated calibration tables.
10/18/00    sm     voc_cal_get_codec_config() updated for QDSP_IMAGE_VRA.
10/13/00   ymc     Added MSM5105 support; T_MSM33 as T_MSM33 & !T_MSM5105;
                   !T_MSM33 as !T_MSM33 || T_MSM5105.
09/21/00    sm     Added voc_cal_get_codec_config().
09/18/00    sm     Added analog QSynth codec path, 22khz sdac flag.
09/08/00    sm     Added Bluetooth/PPG support.
08/15/00   spf     Removed QDSP_PCM_MODE_16K_V setting from the
                   QDSP_INT_16K_PCM_FORMAT definition for PPG target.
08/14/00   spf     Updated codec configuration command defintions for PPG.
08/03/00    st     Replaced "#if define" with "#if defined".
08/03/00    sm     Added SDAC calibration for PPG.
07/14/00   spf     Added support for TTY headset and HFK devices.
                   Removed voc_cal_get_tty_cal().
06/16/00    sm     Updated calibration for avcRlinkSensitivityOffset.
05/30/00    st     Added defines for codec configuration commands for new PCM
                   paths.
05/26/00   spf     Updated FFA and TTP Tx calibration to match 5GP.
05/16/00    st     Modified codec configuration command for stereo DAC.
05/12/00   spf     Add TTY support: voc_tty_cal and voc_cal_get_tty_cal().
05/02/00    ro     Recal SAT AGC level for FFA.
           spf     Set VOC_DFM_SAT_THRESH to 0x0320.
03/28/00    sm     AAGC mainline.
03/22/00   spf     TTP phone targets use same Tx Volume as FFA for internal
                   codec channel 0.
03/08/00   spf     Added DFM configuration support.
02/35/00    ro     Increased AUX PCM TX gain by 2.5 dB.
01/25/00    ro     Use voc_ec_type and voc_ns_type.
01/21/00   spf     Updated off-board, off-chip PCM calibration to use
                   AEC instead of ESEC echo cancellation.
12/17/99   jkl     Adjusted TX gains for AUX channel (Analog HFK).
12/01/99   spf     Merged with 1.12.1.0: Calibrated audio AGC.
11/29/99   jkl     Modified Echo Canceller mode for headset and Analog HFK.
11/22/99   jkl     Added new AGC /AVC parameters values.
11/16/99   jkl     Set AUX_CLK_VAL to be level HIGH.
11/11/99   jkl     Lowered the DTMF TX gain.
11/04/99   jkl     Included FFA phone specific calibration data.
10/28/99   jkl     Updated calibration values.
10/18/99    st     Removed initialization for agc parameters.
10/14/99   jkl     Disabled Codec reset strobe from calibration table.
10/04/99   jkl     Included CAL_MEMORY for calibration tests.
09/22/99   spf     Adjusted codec side tone gains to 0x800 for internal codec.
                   Modified codec interface definitions to use
                   QDSP_DETECT_ZEROES_V instead of QDSP_DETECT_ONES_V.
09/10/99   spf     Removed T_MSM31_TCX0_DIV2.
09/09/99   jkl     Included CODEC TX and ST gains
08/26/99   spf     Pink Panther Rev2 support.
08/02/99   spf     Zeroed the tx_filter and rx_filter fields of
                   voc_pcm_on_board_cal.
06/15/99    ro     First version.

===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"            /* Definitions for byte, word, etc.        */
#include "msmaud.h"            /* MSMAudio features                       */
#include "clkregim.h"          /* Clock regime                            */
#include "voccal.h"            /* Interface definition for voc cal data   */
#include "vocdata.h"

#ifdef FEATURE_VOC_NV_CAL
#include "nv.h"                /* NV read operations                      */
#include "voci.h"              /* For VOC_NV_SIG                          */
#endif /* FEATURE_VOC_NV_CAL */

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

/* <EJECT> */

/* ===========================================================================
**
**                M A I N  C O N F I G U R A T I O N   A N D
**                     C A L I B R A T I O N   D A T A
**
** =========================================================================*/


#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

/* Configuration features
*/

/* off-chip on-board codec - uLaw or linear (default) */
#undef VOC_USE_ULAW


#ifdef FEATURE_VOC_MIC_GAIN_ADJUST
/* Cached mic gain adjustment values (default=0) */
sint15 voc_cal_mic_gain_adjust[VOC_CODEC_MAX] =
{
  /*  VOC_CODEC_ON_CHIP_0         */ 0
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  /* VOC_CODEC_ON_CHIP_0_DUAL_MIC */,0
#endif
  /*  VOC_CODEC_ON_CHIP_1         */,0
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  /*  VOC_CODEC_STEREO_HEADSET    */,0
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
  /*  VOC_CODEC_ON_CHIP_AUX       */,0
  /*  VOC_CODEC_ON_BOARD          */,0
#ifdef FEATURE_SPEAKER_PHONE
  /*  VOC_CODEC_SPEAKER           */,0
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_BT
  /*  VOC_CODEC_BT_INTERCOM       */,0
#ifdef FEATURE_BT_AG
  /*  VOC_CODEC_BT_AG             */,0
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  /* VOC_CODEC_BT_A2DP            */,0
  /* VOC_CODEC_BT_A2DP_SCO        */,0
#endif /* FEATURE_SBC_CODEC */
#endif /* FEATURE_BT */
  /*  VOC_CODEC_OFF_BOARD         */,0
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  /*  VOC_CODEC_SDAC              */,0
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
#if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
 /* VOC_CODEC_IN_MONO_SADC_OUT_MONO_HANDSET    */
                                    ,0
 /* VOC_CODEC_IN_MONO_SADC_OUT_STEREO_HEADSET  */
                                    ,0
 /* VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET  */
                                    ,0
 /* VOC_CODEC_IN_STEREO_SADC_OUT_STEREO_HEADSET*/
                                    ,0
#endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */
#ifdef FEATURE_TTY
  /*  VOC_CODEC_TTY_ON_CHIP_1     */,0
  /*  VOC_CODEC_TTY_OFF_BOARD     */,0
  /*  VOC_CODEC_TTY_VCO           */,0
  /*  VOC_CODEC_TTY_HCO           */,0
#endif /* FEATURE_TTY */
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
  /*  VOC_CODEC_USB               */,0
#ifdef FEATURE_INTERNAL_SDAC
  /*  VOC_CODEC_STEREO_USB        */,0
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
/* FM over BT AG */
#ifdef FEATURE_BT_AG
                                    ,0
#endif
  /*  VOC_CODEC_ON_CHIP_0_SURF    */,0
  /*  VOC_CODEC_ON_CHIP_1_SURF    */,0
#ifdef FEATURE_SPEAKER_PHONE
  /*  VOC_CODEC_SPEAKER_SURF      */,0
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM
  /* VOC_CODEC_HEADSET_FM */		,0
  /* VOC_CODEC_SPEAKER_FM */		,0
#endif
};
#endif /* FEATURE_VOC_MIC_GAIN_ADJUST */


/* Codec types */
voc_codec_class_type voc_cal_codec_class[VOC_CODEC_MAX] =
{
  /*  VOC_CODEC_ON_CHIP_0         */ VOC_CODEC_CLASS_INTERNAL
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  /* VOC_CODEC_ON_CHIP_0_DUAL_MIC */,VOC_CODEC_CLASS_INTERNAL
#endif
  /*  VOC_CODEC_ON_CHIP_1         */,VOC_CODEC_CLASS_INTERNAL
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
#ifdef FEATURE_INTERNAL_SDAC
  /*  VOC_CODEC_STEREO_HEADSET    */,VOC_CODEC_CLASS_INTERNAL_STEREO
#else
  /*  VOC_CODEC_STEREO_HEADSET    */,VOC_CODEC_CLASS_INTERNAL
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
  /*  VOC_CODEC_ON_CHIP_AUX       */,VOC_CODEC_CLASS_INTERNAL
  /*  VOC_CODEC_ON_BOARD          */,VOC_CODEC_CLASS_EXTERNAL
#ifdef FEATURE_SPEAKER_PHONE
  /*  VOC_CODEC_SPEAKER           */,VOC_CODEC_CLASS_INTERNAL
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_BT
  /*  VOC_CODEC_BT_INTERCOM       */,VOC_CODEC_CLASS_INTERNAL
#ifdef FEATURE_BT_AG
  /*  VOC_CODEC_BT_AG             */,VOC_CODEC_CLASS_BLUETOOTH
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  /* VOC_CODEC_BT_A2DP            */,VOC_CODEC_CLASS_BLUETOOTH_A2DP
  /* VOC_CODEC_BT_A2DP_SCO        */,VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO
#endif /* FEATURE_SBC_CODEC */
#endif /* FEATURE_BT */
  /*  VOC_CODEC_OFF_BOARD         */,VOC_CODEC_CLASS_EXTERNAL
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
#ifdef FEATURE_INTERNAL_SDAC
  /*  VOC_CODEC_SDAC              */,VOC_CODEC_CLASS_INTERNAL_STEREO
#else
  /*  VOC_CODEC_SDAC              */,VOC_CODEC_CLASS_EXTERNAL_STEREO
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
#if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
#ifdef FEATURE_INTERNAL_SADC
 /* VOC_CODEC_IN_MONO_SADC_OUT_MONO_HANDSET    */
                                    ,VOC_CODEC_CLASS_INTERNAL_SADC_HANDSET
 /* VOC_CODEC_IN_MONO_SADC_OUT_STEREO_HEADSET  */
                                    ,VOC_CODEC_CLASS_INTERNAL_SADC_HEADSET
 /* VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET  */
                                    ,VOC_CODEC_CLASS_INTERNAL_SADC_HANDSET
 /* VOC_CODEC_IN_STEREO_SADC_OUT_STEREO_HEADSET*/
                                    ,VOC_CODEC_CLASS_INTERNAL_SADC_HEADSET
#else
 /* VOC_CODEC_IN_MONO_SADC_OUT_MONO_HANDSET    */
                                    ,VOC_CODEC_CLASS_EXTERNAL_SADC_HANDSET
 /* VOC_CODEC_IN_MONO_SADC_OUT_STEREO_HEADSET  */
                                    ,VOC_CODEC_CLASS_EXTERNAL_SADC_HEADSET
 /* VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET  */
                                    ,VOC_CODEC_CLASS_EXTERNAL_SADC_HANDSET
 /* VOC_CODEC_IN_STEREO_SADC_OUT_STEREO_HEADSET*/
                                    ,VOC_CODEC_CLASS_EXTERNAL_SADC_HEADSET
#endif /* FEATURE_INTERNAL_SADC */
#endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */
#ifdef FEATURE_TTY
  /*  VOC_CODEC_TTY_ON_CHIP_1     */,VOC_CODEC_CLASS_INTERNAL
  /*  VOC_CODEC_TTY_OFF_BOARD     */,VOC_CODEC_CLASS_EXTERNAL
  /*  VOC_CODEC_TTY_VCO           */,VOC_CODEC_CLASS_INTERNAL
  /*  VOC_CODEC_TTY_HCO           */,VOC_CODEC_CLASS_INTERNAL
#endif /* FEATURE_TTY */
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
  /*  VOC_CODEC_USB               */,VOC_CODEC_CLASS_INTERNAL
#ifdef FEATURE_INTERNAL_SDAC
  /*  VOC_CODEC_STEREO_USB        */,VOC_CODEC_CLASS_INTERNAL_STEREO
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
/* FM over BT AG */
#ifdef FEATURE_BT_AG
                                    ,VOC_CODEC_CLASS_BLUETOOTH
#endif
  /*  VOC_CODEC_ON_CHIP_0_SURF    */,VOC_CODEC_CLASS_INTERNAL
  /*  VOC_CODEC_ON_CHIP_1_SURF    */,VOC_CODEC_CLASS_INTERNAL
#ifdef FEATURE_SPEAKER_PHONE
  /*  VOC_CODEC_SPEAKER_SURF      */,VOC_CODEC_CLASS_INTERNAL
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM
/*  VOC_CODEC_HEADSET    */  ,VOC_CODEC_CLASS_INTERNAL
/*  VOC_CODEC_SPEAKER    */  ,VOC_CODEC_CLASS_INTERNAL

#endif
};


#ifdef FEATURE_PHONE_VR
/* Codec VR type */
voc_codec_vr_type voc_cal_codec_vr_types[VOC_CODEC_MAX] =
{
  /*  VOC_CODEC_ON_CHIP_0         */ VOC_CODEC_VR_TYPE_HANDSET
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  /* VOC_CODEC_ON_CHIP_0_DUAL_MIC */,VOC_CODEC_VR_TYPE_HANDSET
#endif
  /*  VOC_CODEC_ON_CHIP_1         */,VOC_CODEC_VR_TYPE_HEADSET
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  /*  VOC_CODEC_STEREO_HEADSET    */,VOC_CODEC_VR_TYPE_HEADSET
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
  /*  VOC_CODEC_ON_CHIP_AUX       */,VOC_CODEC_VR_TYPE_CARKIT
  /*  VOC_CODEC_ON_BOARD          */,VOC_CODEC_VR_TYPE_NONE
#ifdef FEATURE_SPEAKER_PHONE
  /*  VOC_CODEC_SPEAKER           */,VOC_CODEC_VR_TYPE_CARKIT
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_BT
  /*  VOC_CODEC_BT_INTERCOM       */,VOC_CODEC_VR_TYPE_NONE
#ifdef FEATURE_BT_AG
  /*  VOC_CODEC_BT_AG             */,VOC_CODEC_VR_TYPE_HEADSET
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  /* VOC_CODEC_BT_A2DP            */,VOC_CODEC_VR_TYPE_NONE
  /* VOC_CODEC_BT_A2DP_SCO        */,VOC_CODEC_VR_TYPE_HEADSET
#endif /* FEATURE_SBC_CODEC */
#endif /* FEATURE_BT */
  /*  VOC_CODEC_OFF_BOARD         */,VOC_CODEC_VR_TYPE_CARKIT
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  /*  VOC_CODEC_SDAC              */,VOC_CODEC_VR_TYPE_NONE
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
#if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
 /* VOC_CODEC_IN_MONO_SADC_OUT_MONO_HANDSET    */
                                    ,VOC_CODEC_VR_TYPE_NONE
 /* VOC_CODEC_IN_MONO_SADC_OUT_STEREO_HEADSET  */
                                    ,VOC_CODEC_VR_TYPE_NONE
 /* VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET  */
                                    ,VOC_CODEC_VR_TYPE_NONE
 /* VOC_CODEC_IN_STEREO_SADC_OUT_STEREO_HEADSET*/
                                    ,VOC_CODEC_VR_TYPE_NONE
#endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */
#ifdef FEATURE_TTY
  /*  VOC_CODEC_TTY_ON_CHIP_1     */,VOC_CODEC_VR_TYPE_NONE
  /*  VOC_CODEC_TTY_OFF_BOARD     */,VOC_CODEC_VR_TYPE_NONE
  /*  VOC_CODEC_TTY_VCO           */,VOC_CODEC_VR_TYPE_NONE
  /*  VOC_CODEC_TTY_HCO           */,VOC_CODEC_VR_TYPE_NONE
#endif /* FEATURE_TTY */
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
  /*  VOC_CODEC_USB               */,VOC_CODEC_VR_TYPE_CARKIT
#ifdef FEATURE_INTERNAL_SDAC
  /*  VOC_CODEC_STEREO_USB        */,VOC_CODEC_VR_TYPE_CARKIT
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
/* FM over BT AG */
#ifdef FEATURE_BT_AG
                                    ,VOC_CODEC_VR_TYPE_HEADSET
#endif
/*  VOC_CODEC_ON_CHIP_0_SURF    */  ,VOC_CODEC_VR_TYPE_HANDSET
/*  VOC_CODEC_ON_CHIP_1_SURF    */  ,VOC_CODEC_VR_TYPE_HEADSET
#ifdef FEATURE_SPEAKER_PHONE
  /*  VOC_CODEC_SPEAKER_SURF      */,VOC_CODEC_VR_TYPE_CARKIT
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM
/*  VOC_CODEC_HEADSET    */  ,VOC_CODEC_VR_TYPE_HEADSET
/*  VOC_CODEC_SPEAKER    */  ,VOC_CODEC_VR_TYPE_CARKIT

#endif
};
#endif /* FEATURE_PHONE_VR */


/* <EJECT> */
/* ===========================================================================
**
**               V O C O D E R   C O N F I G U R A T I O N   A N D
**                  C A L I B R A T I O N   C O N S T A N T S
**
** =========================================================================*/

/* Compatibility defines
*/
#ifdef MSMAUD_DIN_LOOP_BIT
#define VOC_CAL_DIN_LOOP_V QDSP_DIN_LOOP_DIS_V
#else
#define VOC_CAL_DIN_LOOP_V QDSP_RESERVED_V
#endif


/* SDAC setup
*/
#define VOC_CAL_SDAC_CTRL_M       QDSP_I2S_CTRL_M            | \
                                  QDSP_SDAC_EN_M

#define VOC_CAL_SDAC_ENA_V        QDSP_I2S_DIS_V             | \
                                  QDSP_SDAC_EN_ON_V

#define VOC_CAL_SIGN_SWAP_V       QDSP_SDAC_SIGN_SWAP_DIS_V


#ifdef VOC_USE_ULAW
#if defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || defined(MSMAUD_SCMM_SURF_AUDIO_CAL)
#error code not present
#else
/* PCM format: 8-bit u-law without padding
*/
#define QDSP_PHONE_PCM_FORMAT    ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_ENA_V          | \
                                   QDSP_TX_HPF_ENA_V          | \
                                   QDSP_TX_SLOPE_FILT_ENA_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_ULAW_V        | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_ULAW_V        )
#endif /* defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || defined(MSMAUD_SCMM_SURF_AUDIO_CAL) */

#else
/* PCM format: 13-bit linear with padding enabled
*/

#if defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || defined(MSMAUD_SCMM_SURF_AUDIO_CAL)
#error code not present
#else
/* Off-chip, on-board codec */
#define QDSP_PHONE_PCM_FORMAT    ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_ENA_V          | \
                                   QDSP_TX_HPF_ENA_V          | \
                                   QDSP_TX_SLOPE_FILT_ENA_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_ENA_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_ENA_V         | \
                                   QDSP_ENC_PCM_LIN_V         )
#endif /* defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || defined(MSMAUD_SCMM_SURF_AUDIO_CAL) */

/* Internal codec */
#if defined(FEATURE_AUDIO_CAL_MED_HIGH) || \
                                         defined(FEATURE_AUDIO_CAL_MED_HIGH_FFA)
#define QDSP_INTERNAL_PCM_FMT_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_ENA_V          | \
                                    QDSP_TX_HPF_ENA_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )
#elif defined(MSMAUD_6260_FFA_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FMT_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_ENA_V          | \
                                    QDSP_TX_HPF_ENA_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )
#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FMT_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_DIS_V          | \
                                    QDSP_TX_HPF_DIS_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )
#elif defined(MSMAUD_6800_GSM_FFA_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FMT_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_ENA_V          | \
                                    QDSP_TX_HPF_ENA_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )
#elif defined(MSMAUD_6280_FFA_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FMT_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_ENA_V          | \
                                    QDSP_TX_HPF_ENA_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )
#elif defined (MSMAUD_6500_FFA_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FMT_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_ENA_V          | \
                                    QDSP_TX_HPF_ENA_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )
#elif defined (MSMAUD_QSC6270_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FMT_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_ENA_V          | \
                                    QDSP_TX_HPF_ENA_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )
#elif defined (MSMAUD_SCMM_FFA_AUDIO_CAL)
#error code not present
#elif defined (MSMAUD_SCMM_SURF_AUDIO_CAL)
#error code not present
#else
#define QDSP_INTERNAL_PCM_FMT_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_DIS_V          | \
                                    QDSP_TX_HPF_ENA_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )
#endif /* (FEATURE_AUDIO_CAL_MED_HIGH) ||
                (FEATURE_AUDIO_CAL_MED_HIGH_FFA) */

#if defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || defined(MSMAUD_SCMM_SURF_AUDIO_CAL)
#error code not present
#else
/* Internal codec for wb case (GSM)*/
#define QDSP_INTERNAL_PCM_FMT_GSM_WB ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_ENA_V          | \
                                    QDSP_TX_HPF_ENA_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )

#define QDSP_INTERNAL_PCM_FMT_GSM_SURF_WB ( QDSP_PCM_MODE_INTERNAL_V   | \
                                    QDSP_RX_HPF_ENA_V          | \
                                    QDSP_TX_HPF_ENA_V          | \
                                    QDSP_TX_SLOPE_FILT_DIS_V   | \
                                    QDSP_CODEC_RES_DIS_V       | \
                                    QDSP_DEC_PAD_DIS_V         | \
                                    QDSP_DEC_PCM_LIN_V         | \
                                    QDSP_ENC_PAD_DIS_V         | \
                                    QDSP_ENC_PCM_LIN_V         )
#endif /* defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || 
          defined(MSMAUD_SCMM_SURF_AUDIO_CAL) */

#if defined(MSMAUD_QSC60X0_AUDIO_CAL)

#define QDSP_INTERNAL_PCM_FORMAT ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_ENA_V          | \
                                   QDSP_TX_HPF_ENA_V          | \
                                   QDSP_TX_SLOPE_FILT_ENA_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )

#elif defined(MSMAUD_6260_FFA_AUDIO_CAL)

#define QDSP_INTERNAL_PCM_FORMAT ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_ENA_V          | \
                                   QDSP_TX_HPF_ENA_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )


#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FORMAT ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_DIS_V          | \
                                   QDSP_TX_HPF_DIS_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )

#elif defined(MSMAUD_6800_GSM_FFA_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FORMAT ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_ENA_V          | \
                                   QDSP_TX_HPF_ENA_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )


#elif defined (MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined (MSMAUD_QSC1110_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FORMAT ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_ENA_V          | \
                                   QDSP_TX_HPF_ENA_V          | \
                                   QDSP_TX_SLOPE_FILT_ENA_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )

#elif defined (MSMAUD_SCMM_FFA_AUDIO_CAL)
#error code not present
#elif defined (MSMAUD_SCMM_SURF_AUDIO_CAL)
#error code not present
#else

#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
#define QDSP_INTERNAL_PCM_FORMAT ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_ENA_V          | \
                                   QDSP_TX_HPF_ENA_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )

#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */

#endif /* MSMAUD_QSC60X0_AUDIO_CAL */

#if defined(MSMAUD_QSC60X0_AUDIO_CAL)

#define QDSP_INTERNAL_PCM_FORMAT_1 ( QDSP_PCM_MODE_INTERNAL_V   | \
                                     QDSP_RX_HPF_ENA_V          | \
                                     QDSP_TX_HPF_ENA_V          | \
                                     QDSP_TX_SLOPE_FILT_ENA_V   | \
                                     QDSP_CODEC_RES_DIS_V       | \
                                     QDSP_DEC_PAD_DIS_V         | \
                                     QDSP_DEC_PCM_LIN_V         | \
                                     QDSP_ENC_PAD_DIS_V         | \
                                     QDSP_ENC_PCM_LIN_V         )

#define QDSP_INTERNAL_PCM_FORMAT_SPEAKER ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_ENA_V          | \
                                   QDSP_TX_HPF_ENA_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )

#elif defined(MSMAUD_6500_FFA_AUDIO_CAL)

#define QDSP_INTERNAL_PCM_FORMAT_1 ( QDSP_PCM_MODE_INTERNAL_V   | \
                                     QDSP_RX_HPF_ENA_V          | \
                                     QDSP_TX_HPF_ENA_V          | \
                                     QDSP_TX_SLOPE_FILT_ENA_V   | \
                                     QDSP_CODEC_RES_DIS_V       | \
                                     QDSP_DEC_PAD_DIS_V         | \
                                     QDSP_DEC_PCM_LIN_V         | \
                                     QDSP_ENC_PAD_DIS_V         | \
                                     QDSP_ENC_PCM_LIN_V         )


#define QDSP_INTERNAL_PCM_FORMAT_SPEAKER ( QDSP_PCM_MODE_INTERNAL_V   | \
                                           QDSP_RX_HPF_ENA_V          | \
                                           QDSP_TX_HPF_ENA_V          | \
                                           QDSP_TX_SLOPE_FILT_DIS_V   | \
                                           QDSP_CODEC_RES_DIS_V       | \
                                           QDSP_DEC_PAD_DIS_V         | \
                                           QDSP_DEC_PCM_LIN_V         | \
                                           QDSP_ENC_PAD_DIS_V         | \
                                           QDSP_ENC_PCM_LIN_V         )
#elif defined (MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FORMAT_1 ( QDSP_PCM_MODE_INTERNAL_V   | \
                                     QDSP_RX_HPF_ENA_V          | \
                                     QDSP_TX_HPF_ENA_V          | \
                                     QDSP_TX_SLOPE_FILT_ENA_V   | \
                                     QDSP_CODEC_RES_DIS_V       | \
                                     QDSP_DEC_PAD_DIS_V         | \
                                     QDSP_DEC_PCM_LIN_V         | \
                                     QDSP_ENC_PAD_DIS_V         | \
                                     QDSP_ENC_PCM_LIN_V         )

#define QDSP_INTERNAL_PCM_FORMAT_SPEAKER ( QDSP_PCM_MODE_INTERNAL_V   | \
                                           QDSP_RX_HPF_ENA_V          | \
                                           QDSP_TX_HPF_ENA_V          | \
                                           QDSP_TX_SLOPE_FILT_DIS_V   | \
                                           QDSP_CODEC_RES_DIS_V       | \
                                           QDSP_DEC_PAD_DIS_V         | \
                                           QDSP_DEC_PCM_LIN_V         | \
                                           QDSP_ENC_PAD_DIS_V         | \
                                           QDSP_ENC_PCM_LIN_V         )

#elif defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || defined(MSMAUD_SCMM_SURF_AUDIO_CAL)
#error code not present
#else

#define QDSP_INTERNAL_PCM_FORMAT_1 ( QDSP_PCM_MODE_INTERNAL_V   | \
                                     QDSP_RX_HPF_ENA_V          | \
                                     QDSP_TX_HPF_ENA_V          | \
                                     QDSP_TX_SLOPE_FILT_ENA_V   | \
                                     QDSP_CODEC_RES_DIS_V       | \
                                     QDSP_DEC_PAD_DIS_V         | \
                                     QDSP_DEC_PCM_LIN_V         | \
                                     QDSP_ENC_PAD_DIS_V         | \
                                     QDSP_ENC_PCM_LIN_V         )

#define QDSP_INTERNAL_PCM_FORMAT_SPEAKER ( QDSP_PCM_MODE_INTERNAL_V   | \
                                           QDSP_RX_HPF_ENA_V          | \
                                           QDSP_TX_HPF_ENA_V          | \
                                           QDSP_TX_SLOPE_FILT_DIS_V   | \
                                           QDSP_CODEC_RES_DIS_V       | \
                                           QDSP_DEC_PAD_DIS_V         | \
                                           QDSP_DEC_PCM_LIN_V         | \
                                           QDSP_ENC_PAD_DIS_V         | \
                                           QDSP_ENC_PCM_LIN_V         )
#endif  /* MSMAUD_QSC60X0_AUDIO_CAL */

#if defined(FEATURE_AUDIO_CAL_MED_HIGH_FFA)
#define QDSP_INTERNAL_PCM_FMT_1_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                      QDSP_RX_HPF_ENA_V          | \
                                      QDSP_TX_HPF_ENA_V          | \
                                      QDSP_TX_SLOPE_FILT_DIS_V   | \
                                      QDSP_CODEC_RES_DIS_V       | \
                                      QDSP_DEC_PAD_DIS_V         | \
                                      QDSP_DEC_PCM_LIN_V         | \
                                      QDSP_ENC_PAD_DIS_V         | \
                                      QDSP_ENC_PCM_LIN_V         )
#elif defined(MSMAUD_6260_FFA_AUDIO_CAL) || \
      defined(MSMAUD_6280_FFA_AUDIO_CAL) || \
      defined(MSMAUD_6500_FFA_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FMT_1_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                      QDSP_RX_HPF_ENA_V          | \
                                      QDSP_TX_HPF_ENA_V          | \
                                      QDSP_TX_SLOPE_FILT_DIS_V   | \
                                      QDSP_CODEC_RES_DIS_V       | \
                                      QDSP_DEC_PAD_DIS_V         | \
                                      QDSP_DEC_PCM_LIN_V         | \
                                      QDSP_ENC_PAD_DIS_V         | \
                                      QDSP_ENC_PCM_LIN_V         )
#elif defined(MSMAUD_QSC6270_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FMT_1_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                      QDSP_RX_HPF_ENA_V          | \
                                      QDSP_TX_HPF_ENA_V          | \
                                      QDSP_TX_SLOPE_FILT_ENA_V   | \
                                      QDSP_CODEC_RES_DIS_V       | \
                                      QDSP_DEC_PAD_DIS_V         | \
                                      QDSP_DEC_PCM_LIN_V         | \
                                      QDSP_ENC_PAD_DIS_V         | \
                                      QDSP_ENC_PCM_LIN_V         )
#elif defined(MSMAUD_SCMM_FFA_AUDIO_CAL)
#error code not present
#elif defined(MSMAUD_SCMM_SURF_AUDIO_CAL)
#error code not present
#else
#define QDSP_INTERNAL_PCM_FMT_1_GSM ( QDSP_PCM_MODE_INTERNAL_V   | \
                                      QDSP_RX_HPF_DIS_V          | \
                                      QDSP_TX_HPF_ENA_V          | \
                                      QDSP_TX_SLOPE_FILT_ENA_V   | \
                                      QDSP_CODEC_RES_DIS_V       | \
                                      QDSP_DEC_PAD_DIS_V         | \
                                      QDSP_DEC_PCM_LIN_V         | \
                                      QDSP_ENC_PAD_DIS_V         | \
                                      QDSP_ENC_PCM_LIN_V         )
#endif /* defined(FEATURE_AUDIO_CAL_MED_HIGH_FFA) */

#if defined(MSMAUD_QSC6270_AUDIO_CAL)
#define QDSP_INTERNAL_PCM_FMT_1_GSM_WB ( QDSP_PCM_MODE_INTERNAL_V   | \
                                      QDSP_RX_HPF_ENA_V          | \
                                      QDSP_TX_HPF_ENA_V          | \
                                      QDSP_TX_SLOPE_FILT_DIS_V   | \
                                      QDSP_CODEC_RES_DIS_V       | \
                                      QDSP_DEC_PAD_DIS_V         | \
                                      QDSP_DEC_PCM_LIN_V         | \
                                      QDSP_ENC_PAD_DIS_V         | \
                                      QDSP_ENC_PCM_LIN_V         )
#elif defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || defined(MSMAUD_SCMM_SURF_AUDIO_CAL)
#error code not present
#else
#define QDSP_INTERNAL_PCM_FMT_1_GSM_WB ( QDSP_PCM_MODE_INTERNAL_V   | \
                                      QDSP_RX_HPF_DIS_V          | \
                                      QDSP_TX_HPF_ENA_V          | \
                                      QDSP_TX_SLOPE_FILT_ENA_V   | \
                                      QDSP_CODEC_RES_DIS_V       | \
                                      QDSP_DEC_PAD_DIS_V         | \
                                      QDSP_DEC_PCM_LIN_V         | \
                                      QDSP_ENC_PAD_DIS_V         | \
                                      QDSP_ENC_PCM_LIN_V         )
#endif /* defined(MSMAUD_QSC6270_AUDIO_CAL) */
#endif  /* VOC_USE_ULAW */


/* PCM format: 8-bit u-law with padding enabled
*/
#define QDSP_AUX_PCM_FORMAT     ( QDSP_PCM_MODE_INTERNAL_V   | \
                                  QDSP_DEC_PAD_ENA_V         | \
                                  QDSP_DEC_PCM_ULAW_V        | \
                                  QDSP_ENC_PAD_ENA_V         | \
                                  QDSP_ENC_PCM_ULAW_V        )

#ifdef FEATURE_BT
#define QDSP_BT_INTERCOM_PCM_FORMAT  (QDSP_PCM_MODE_STANDALONE_V | \
                                      QDSP_RX_HPF_ENA_V          | \
                                      QDSP_TX_HPF_ENA_V          | \
                                      QDSP_TX_SLOPE_FILT_ENA_V   | \
                                      QDSP_CODEC_RES_DIS_V       | \
                                      QDSP_DEC_PAD_DIS_V         | \
                                      QDSP_DEC_PCM_LIN_V         | \
                                      QDSP_ENC_PAD_DIS_V         | \
                                      QDSP_ENC_PCM_LIN_V         )

#ifdef FEATURE_BT_AG
#ifdef MSMAUD_ALT_BT_AG_CONFIG
#error code not present
#else /* MSMAUD_ALT_BT_AG_CONFIG */

#define QDSP_BT_AG_PCM_FORMAT    ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_DIS_V          | \
                                   QDSP_TX_HPF_DIS_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )
/* FM over BT AG */
#define QDSP_FM_BT_AG_PCM_FORMAT ( QDSP_PCM_MODE_STANDALONE_V | \
                                   QDSP_RX_HPF_DIS_V          | \
                                   QDSP_TX_HPF_DIS_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )


#ifdef FEATURE_AVS_AUDIO_DECODER_SCO
#define QDSP_BT_AG_MM_PCM_FORMAT ( QDSP_PCM_MODE_32K_STEREO_V | \
                                   QDSP_RX_HPF_DIS_V          | \
                                   QDSP_TX_HPF_DIS_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )
#endif /*FEATURE_AVS_AUDIO_DECODER_SCO*/

#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* MSMAUD_ALT_BT_AG_CONFIG */
#endif /* FEATURE_BT_AG */
#endif /* FEATURE_BT */

#if defined(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)
#define QDSP_EXT_SADC_PCM_FORMAT  ( QDSP_PCM_MODE_ENCODER_V |\
                                QDSP_RX_HPF_DIS_V           |\
				QDSP_TX_HPF_ENA_V           |\
				QDSP_TX_SLOPE_FILT_ENA_V    |\
                                QDSP_DEC_PCM_LIN_V          |\
                                QDSP_ENC_PCM_LIN_V)

#endif  /* FEATURE_AVS_CRYSTAL_SPEECH && EATURE_EXTERNAL_SADC*/
/*
**  Constants for pcm_ctrl
*/

#ifdef VOC_USE_ULAW
/* 8-bit PCM Frames
*/
#define QDSP_PCM_CTRL_DEF       ( QDSP_PCM_LB_DIS_V          | \
                                  QDSP_PCM_CLK_SENSE_NORM_V  | \
                                  QDSP_DEC_PCM_WIDTH_8_V     | \
                                  QDSP_DEC_PCM_SLOT_V(0)     | \
                                  QDSP_ENC_PCM_WIDTH_8_V     | \
                                  QDSP_ENC_PCM_SLOT_V(0)     )
#else
/* 16-bit PCM frames
*/
#define QDSP_PCM_CTRL_DEF       ( QDSP_PCM_LB_DIS_V          | \
                                  QDSP_PCM_CLK_SENSE_NORM_V  | \
                                  QDSP_DEC_PCM_WIDTH_16_V    | \
                                  QDSP_DEC_PCM_SLOT_V(0)     | \
                                  QDSP_ENC_PCM_WIDTH_16_V    | \
                                  QDSP_ENC_PCM_SLOT_V(0)     )
#endif

#define QDSP_AUX_PCM_CTRL_DEF   ( QDSP_PCM_LB_DIS_V          | \
                                  QDSP_PCM_CLK_SENSE_NORM_V  | \
                                  QDSP_DEC_PCM_WIDTH_16_V    | \
                                  QDSP_DEC_PCM_SLOT_V(0)     | \
                                  QDSP_ENC_PCM_WIDTH_16_V    | \
                                  QDSP_ENC_PCM_SLOT_V(0)     )

/* Internal codec not used
*/
#define QDSP_CODEC_INT_UNUSED   0


#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
#define QDSP_INT_16K_PCM_FORMAT ( QDSP_PCM_MODE_32K_INT_V    | \
                                  QDSP_CODEC_RES_DIS_V       | \
                                  QDSP_WB_COMP_ENA_V         | \
                                  QDSP_DEC_PAD_DIS_V         | \
                                  QDSP_DEC_PCM_LIN_V         | \
                                  QDSP_SDAC_SIGN_SWAP_DIS_V  )
#endif /* FEATURE_QSYNTH_COMMON || FEATURE_QTUNES_COMMON */

#ifdef FEATURE_INTERNAL_SDAC
#define QDSP_INTERNAL_SDAC_PCM_FORMAT ( QDSP_PCM_MODE_32K_INT_STEREO_V | \
                                        QDSP_CODEC_RES_DIS_V           | \
                                        QDSP_WB_COMP_DIS_V             | \
                                        QDSP_DEC_PAD_DIS_V             | \
                                        QDSP_DEC_PCM_LIN_V             | \
                                        VOC_CAL_SIGN_SWAP_V            )
#endif /* FEATURE_INTERNAL_SDAC */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#define QDSP_SBC_PCM_FORMAT           ( QDSP_PCM_MODE_32K_INT_STEREO_V | \
                                        QDSP_CODEC_RES_DIS_V           | \
                                        QDSP_WB_COMP_DIS_V             | \
                                        QDSP_DEC_PAD_DIS_V             | \
                                        QDSP_DEC_PCM_LIN_V             | \
                                        VOC_CAL_SIGN_SWAP_V            )
#endif /* FEATURE_SBC_CODEC || FEATURE_SBC_DSP_CODEC */

#ifdef FEATURE_EXTERNAL_SDAC
#define QDSP_SDAC_PCM_FORMAT    ( QDSP_PCM_MODE_32K_STEREO_V | \
                                  QDSP_CODEC_RES_DIS_V       | \
                                  QDSP_WB_COMP_DIS_V         | \
                                  QDSP_DEC_PAD_DIS_V         | \
                                  QDSP_DEC_PCM_LIN_V         | \
                                  VOC_CAL_SIGN_SWAP_V        )
#endif /* FEATURE_EXTERNAL_SDAC */

#if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
#define QDSP_SADC_PCM_FORMAT    ( QDSP_PCM_MODE_32K_INT_STEREO_V | \
                                  QDSP_CODEC_RES_DIS_V           | \
                                  QDSP_WB_COMP_DIS_V             | \
                                  QDSP_DEC_PAD_DIS_V             | \
                                  QDSP_DEC_PCM_LIN_V             | \
                                  VOC_CAL_SIGN_SWAP_V        )
#endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */


/*
**  Constants for codec_func_ctrl
*/
#define QDSP_CODEC_CTRL_AMP_OFF   QDSP_VBAP_CODEC_PWR_OFF_V
#define QDSP_CODEC_CTRL_AMP_1     QDSP_VBAP_CODEC_PWR_ON_V
#define QDSP_CODEC_CTRL_AMP_2     QDSP_VBAP_CODEC_PWR_ON_V
#define QDSP_CODEC_CTRL_VCO       QDSP_VBAP_CODEC_PWR_ON_V
#define QDSP_CODEC_CTRL_HCO       QDSP_VBAP_CODEC_PWR_ON_V
#define QDSP_CODEC_CTRL_AMP_AUX   QDSP_VBAP_CODEC_PWR_ON_V
#define QDSP_CODEC_CTRL_WB_AMP_1  QDSP_VBAP_CODEC_PWR_ON_V
#define QDSP_CODEC_CTRL_WB_AMP_2  QDSP_VBAP_CODEC_PWR_ON_V
#if defined(FEATURE_MM_REC) || defined(FEATURE_AVS_I2SSBC)
#define QDSP_CODEC_CTRL_PCM_IN_1  ( QDSP_VBAP_CODEC_I2S_MONO        | \
                                    QDSP_VBAP_CODEC_MODE_SEL_MONO   | \
                                    QDSP_VBAP_CODEC_L_R_N           | \
                                    QDSP_VBAP_CODEC_PWR_ON_V )

#define QDSP_CODEC_CTRL_PCM_IN_2  ( QDSP_VBAP_CODEC_I2S_STEREO      | \
                                    QDSP_VBAP_CODEC_MODE_SEL_STEREO | \
                                    QDSP_VBAP_CODEC_L_R_N           | \
                                    QDSP_VBAP_CODEC_PWR_ON_V        )

#define QDSP_CODEC_CTRL_PCM_IN_3  ( QDSP_VBAP_CODEC_INTERNAL_MONO   | \
                                    QDSP_VBAP_CODEC_MODE_SEL_MONO   | \
                                    QDSP_VBAP_CODEC_L_R_N           | \
                                    QDSP_VBAP_CODEC_PWR_ON_V        )

#define QDSP_CODEC_CTRL_PCM_IN_4  ( QDSP_VBAP_CODEC_INTERNAL_STEREO | \
                                    QDSP_VBAP_CODEC_MODE_SEL_STEREO | \
                                    QDSP_VBAP_CODEC_L_R_N           | \
                                    QDSP_VBAP_CODEC_PWR_ON_V        )
#endif /* FEATURE_MM_REC || FEATURE_AVS_I2SSBC */

#if defined(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)

#define QDSP_EXT_CODEC_CTRL_DUAL_MIC  ( QDSP_VBAP_CODEC_MODE_SEL_STEREO | \
                                    QDSP_VBAP_CODEC_L_R_N           | \
                                    QDSP_VBAP_CODEC_PWR_OFF_V        )

#endif  /*(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)*/


#define QDSP_CODEC_CTRL_DUAL_MIC  ( QDSP_VBAP_CODEC_MODE_SEL_STEREO | \
                                    QDSP_VBAP_CODEC_L_R_N           | \
                                    QDSP_VBAP_CODEC_PWR_ON_V        )



/*
**  Constants for codec_interface
*/

/* Internal codec
*/
#define QDSP_CODEC_INT          ( QDSP_LOOP_SEL_NONE_V       | \
                                  QDSP_I2S_DIS_V             | \
                                  QDSP_SDAC_SIGN_SAME_V      | \
                                  QDSP_AUX_CLK_DIS_V         | \
                                  QDSP_AUX_CLK_VAL_LO_V      | \
                                  QDSP_AUX_SYNC_VAL_LO_V     | \
                                  QDSP_CLK_SYNC_LOOP_DIS_V   | \
                                  QDSP_SDAC_BCLK_OUTP_V      | \
                                  QDSP_AUX_CLK_SENSE_NORM_V  | \
                                  QDSP_SDAC_MCLK_OUTP_V      | \
                                  QDSP_AUX_CODEC_NORM_V      | \
                                  QDSP_AUX_POL_NORMAL_V      | \
                                  QDSP_PCM_CLK_DIR_EXT_V     | \
                                  QDSP_SDAC_EN_OFF_V         )

/* off-chip, on-board codec
*/
#define QDSP_CODEC_ON_BOARD     ( QDSP_LOOP_SEL_NONE_V       | \
                                  QDSP_I2S_DIS_V             | \
                                  QDSP_SDAC_SIGN_SAME_V      | \
                                  QDSP_AUX_CLK_DIS_V         | \
                                  QDSP_AUX_CLK_VAL_LO_V      | \
                                  QDSP_AUX_SYNC_VAL_LO_V     | \
                                  QDSP_CLK_SYNC_LOOP_DIS_V   | \
                                  QDSP_SDAC_BCLK_OUTP_V      | \
                                  QDSP_AUX_CLK_SENSE_NORM_V  | \
                                  QDSP_SDAC_MCLK_OUTP_V      | \
                                  QDSP_AUX_CODEC_NORM_V      | \
                                  QDSP_AUX_POL_NORMAL_V      | \
                                  QDSP_PCM_CLK_DIR_MOD_V     | \
                                  QDSP_SDAC_EN_OFF_V         )

/* off-chip, off-board codec
*/

/* Configure the AUX PCM interface into primary PCM mode */
#if defined(MSMAUD_QSC60X5_AUDIO_HW)
/* For Q60X5 platforms */
#define QDSP_CODEC_OFF_BOARD    ( QDSP_AUX_CODEC_MODE_AUX_PCM    | \
                                  QDSP_AUX_PCM_MODE_MASTER_PRIM  | \
                                  QDSP_PCM_SYNC_INVERT_DIS       | \
                                  QDSP_PCM_SYNC_SHORT_OFFSET_DIS | \
                                  QDSP_PCM_SYNC_LONG_OFFSET_DIS  | \
                                  QDSP_ADSP_CODEC_CTL_ENA        | \
                                  QDSP_AUX_PCM_ENABLE            )

#elif defined(MSMAUD_6800A_AUX_INTERFACE)
/* For platforms that have HW Codec parameter setting like the 6800A */
#define QDSP_CODEC_OFF_BOARD    ( QDSP_LOOP_SEL_NONE_V       | \
                                  QDSP_I2S_DIS_V             | \
                                  QDSP_AUX_PRI_PCM_ENA_V     )

#else
/* For 6050, 6100, 6250, 6500, 6550, ... */
#define QDSP_CODEC_OFF_BOARD    ( QDSP_LOOP_SEL_NONE_V       | \
                                  QDSP_I2S_DIS_V             | \
                                  QDSP_SDAC_SIGN_SAME_V      | \
                                  QDSP_AUX_CLK_ENA_V         | \
                                  QDSP_AUX_CLK_VAL_LO_V      | \
                                  QDSP_AUX_SYNC_VAL_LO_V     | \
                                  QDSP_CLK_SYNC_LOOP_DIS_V   | \
                                  QDSP_SDAC_BCLK_OUTP_V      | \
                                  QDSP_AUX_CLK_SENSE_NORM_V  | \
                                  QDSP_SDAC_MCLK_OUTP_V      | \
                                  QDSP_AUX_CODEC_AUX_V       | \
                                  QDSP_AUX_POL_INVERTED_V    | \
                                  QDSP_PCM_CLK_DIR_EXT_V     | \
                                  QDSP_SDAC_EN_OFF_V         )
#endif /* MSMAUD_QSC60X5_AUDIO_HW */

#ifdef FEATURE_EXTERNAL_SDAC
/* EXTERNAL STEREO DAC
*/

#ifdef MSMAUD_QSC60X5_AUDIO_HW
#define QDSP_CODEC_SDAC         (QDSP_AUX_CODEC_MODE_I2S     | \
                                 QDSP_I2S_CLK_MODE_MASTER    | \
                                 QDSP_I2S_SDAC_MODE          | \
                                 QDSP_I2S_MCLK_MODE_MASTER   | \
                                 QDSP_I2S_MCLK_SRC_SDAC_CLK  | \
                                 QDSP_ADSP_CODEC_CTL_ENA     | \
                                 QDSP_I2S_ENABLE)
#else /* 6050, 6100, 6250, 6500, 6550, ... */
/* Currently, 6800A not known to be different from other non-60X5 6K platforms*/
#define QDSP_CODEC_SDAC         ( QDSP_I2S_DIS_V             | \
                                  QDSP_SDAC_SIGN_SAME_V      | \
                                  QDSP_AUX_CLK_DIS_V         | \
                                  QDSP_AUX_CLK_VAL_LO_V      | \
                                  QDSP_AUX_SYNC_VAL_LO_V     | \
                                  QDSP_CLK_SYNC_LOOP_DIS_V   | \
                                  QDSP_SDAC_BCLK_OUTP_V      | \
                                  QDSP_AUX_CLK_SENSE_NORM_V  | \
                                  QDSP_SDAC_MCLK_OUTP_V      | \
                                  QDSP_AUX_CODEC_NORM_V      | \
                                  QDSP_AUX_POL_NORMAL_V      | \
                                  QDSP_PCM_CLK_DIR_MOD_V     | \
                                  VOC_CAL_SDAC_ENA_V         )
#endif
#endif /* FEATURE_EXTERNAL_SDAC */

#ifdef FEATURE_EXTERNAL_SADC
#define VOC_CAL_SADC_ENA_V        QDSP_I2S_DIS_V             | \
                                  QDSP_RESERVED_V

#define QDSP_CODEC_SADC         ( QDSP_I2S_ENA_V             | \
                                  QDSP_SDAC_SIGN_REV_V       | \
                                  QDSP_AUX_CLK_ENA_V         | \
                                  QDSP_AUX_CLK_VAL_LO_V      | \
                                  QDSP_AUX_SYNC_VAL_LO_V     | \
                                  QDSP_CLK_SYNC_LOOP_DIS_V   | \
                                  QDSP_SDAC_BCLK_INP_V       | \
                                  QDSP_AUX_CLK_SENSE_NORM_V  | \
                                  QDSP_SDAC_MCLK_OUTP_V      | \
                                  QDSP_AUX_CODEC_NORM_V      | \
                                  QDSP_AUX_POL_NORMAL_V      | \
                                  QDSP_PCM_CLK_DIR_MOD_V     | \
                                  VOC_CAL_SADC_ENA_V         )
#endif /* FEATURE_EXTERNAL_SADC */

#if defined(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)
#define QDSP_CODEC_EXT_SADC_DUALMIC  ( QDSP_I2S_ENABLE          |\
                               QDSP_ADSP_CODEC_CTL_ENA  |\
                               QDSP_I2S_CLK_MODE_SLAVE  |\
                               QDSP_I2S_MCLK_MODE_SLAVE |\
                               QDSP_AUX_CODEC_MODE_I2S )

#endif/*FEATURE_AVS_CRYSTAL_SPEECH && FEATURE_EXTERNAL_SADC*/


#ifndef MSMAUD_UP_DMA_PATH_CONTROL
/*
**  Constants for DMA Path Control
*/
#define QDSP_DMA_DEF            ( QDSP_DMA_CH_45_SEL_CODEC_V | \
                                  QDSP_DMA_CH_23_SEL_DFM_V   | \
                                  QDSP_DMA_CH_01_SEL_PCM_V   )

#endif /* MSMAUD_UP_DMA_PATH_CONTROL */

/*
 * This section is specifically for incall mixer configuration.
 */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
#define QDSP_INTERNAL_PCM_FORMAT_MIXER ( QDSP_PCM_MODE_INTERNAL_V   | \
                                   QDSP_RX_HPF_DIS_V          | \
                                   QDSP_TX_HPF_DIS_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )

/* This is the new field added for SATURN HW codec, DSP
 * always needs it to be set to ONE for internal codecs.
 */
#define QDSP_INTERNAL_CODEC_MIXER 0x0001

#define QDSP_INTERNAL_PCM_FORMAT_1_MIXER ( QDSP_PCM_MODE_DECODER_V   | \
                                     QDSP_RX_HPF_DIS_V          | \
                                     QDSP_TX_HPF_DIS_V          | \
                                     QDSP_TX_SLOPE_FILT_DIS_V   | \
                                     QDSP_CODEC_RES_DIS_V       | \
                                     QDSP_DEC_PAD_DIS_V         | \
                                     QDSP_DEC_PCM_LIN_V         | \
                                     QDSP_ENC_PAD_DIS_V         | \
                                     QDSP_ENC_PCM_LIN_V         )

#define QDSP_BT_AG_PCM_FORMAT_MIXER    ((QDSP_PCM_MODE_M      + \
                                   QDSP_PCM_MODE_ENCODER_V)   | \
                                   QDSP_RX_HPF_DIS_V          | \
                                   QDSP_TX_HPF_DIS_V          | \
                                   QDSP_TX_SLOPE_FILT_DIS_V   | \
                                   QDSP_CODEC_RES_DIS_V       | \
                                   QDSP_DEC_PAD_DIS_V         | \
                                   QDSP_DEC_PCM_LIN_V         | \
                                   QDSP_ENC_PAD_DIS_V         | \
                                   QDSP_ENC_PCM_LIN_V         )
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

/* FM over BT AG */
#if defined(MSMAUD_ADIE_CODEC_FM_BT_AG_CONFIG_SEQ)

#define VOCCAL_FM_OVER_BT_AG

#endif


/* <EJECT> */
/* ===========================================================================
**
**               V O C O D E R   C O N F I G U R A T I O N   A N D
**                    C A L I B R A T I O N   D A T A
**
** =========================================================================*/

/*
 * This section is specifically for incall mixer configuration.
 */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
/* Internal codec 0 - for mixer architecture */
const qdsp_cmd_codec_config_type voc_cal_codec_int0_mixer = {
  QDSP_INTERNAL_PCM_FORMAT_MIXER,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_INTERNAL_CODEC_MIXER,  /* Internal codec control: For Saturn HW */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/* Internal codec 0 dual mic - for mixer architecture */
const qdsp_cmd_codec_config_type voc_cal_codec_int0_dual_mic_mixer = {
  QDSP_INTERNAL_PCM_FORMAT_MIXER,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_DUAL_MIC,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_INTERNAL_CODEC_MIXER,  /* Internal codec control: For Saturn HW */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

/* Internal codec 1 - for mixer architecture */
const qdsp_cmd_codec_config_type voc_cal_codec_int1_mixer = {
  QDSP_INTERNAL_PCM_FORMAT_1_MIXER,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_2,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_INTERNAL_CODEC_MIXER,  /* Internal codec control: For Saturn HW */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Bluetooth AG codec */
const qdsp_cmd_codec_config_type voc_cal_codec_bt_ag_mixer = {
  QDSP_BT_AG_PCM_FORMAT_MIXER,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_INT_UNUSED,
  QDSP_CODEC_OFF_BOARD,
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_PCM_V
};
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

/* Internal codec 0 */
const qdsp_cmd_codec_config_type voc_cal_codec_int0 = {
  QDSP_INTERNAL_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec 0 for wb case */
const qdsp_cmd_codec_config_type voc_cal_codec_int0_wb = {
  QDSP_INTERNAL_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

};

/* Internal codec 0 (GSM) */
const qdsp_cmd_codec_config_type voc_cal_codec_gsm_int0 = {
  QDSP_INTERNAL_PCM_FMT_GSM,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec 0 for wb case (GSM) */
const qdsp_cmd_codec_config_type voc_cal_codec_gsm_int0_wb = {
  QDSP_INTERNAL_PCM_FMT_GSM_WB,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec 0 for wb case (GSM) for SURF device */
const qdsp_cmd_codec_config_type voc_cal_codec_gsm_int0_surf_wb = {
  QDSP_INTERNAL_PCM_FMT_GSM_SURF_WB,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/* Internal codec 0 dual mic */
const qdsp_cmd_codec_config_type voc_cal_codec_int0_dual_mic = {
  QDSP_INTERNAL_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_DUAL_MIC,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec 0 dual mic (GSM) */
const qdsp_cmd_codec_config_type voc_cal_codec_gsm_int0_dual_mic = {
  QDSP_INTERNAL_PCM_FMT_GSM,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_DUAL_MIC,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec 0 dual mic for wb case */
const qdsp_cmd_codec_config_type voc_cal_codec_int0_dual_mic_wb = {
  QDSP_INTERNAL_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_DUAL_MIC,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec 0 dual mic (GSM) for wb case */
const qdsp_cmd_codec_config_type voc_cal_codec_gsm_int0_dual_mic_wb = {
  QDSP_INTERNAL_PCM_FMT_GSM,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_DUAL_MIC,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

};
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

/* Internal codec 1 */
const qdsp_cmd_codec_config_type voc_cal_codec_int1 = {
  QDSP_INTERNAL_PCM_FORMAT_1,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_2,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec 1 for wb case */
const qdsp_cmd_codec_config_type voc_cal_codec_int1_wb = {
  QDSP_INTERNAL_PCM_FORMAT_1,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_2,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec 1 (GSM) */
const qdsp_cmd_codec_config_type voc_cal_codec_gsm_int1 = {
  QDSP_INTERNAL_PCM_FMT_1_GSM,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_2,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec 1 for wb case(GSM) */
const qdsp_cmd_codec_config_type voc_cal_codec_gsm_int1_wb = {
#if defined(MSMAUD_QSC6270_AUDIO_CAL)
#error code not present
#else
  QDSP_INTERNAL_PCM_FMT_1_GSM,
#endif
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_2,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec, aux channel */
const qdsp_cmd_codec_config_type voc_cal_codec_aux = {
  QDSP_INTERNAL_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_AUX,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF
#endif
};

#ifdef FEATURE_SPEAKER_PHONE
/* Internal codec, aux ear, mic1 */
const qdsp_cmd_codec_config_type voc_cal_codec_speaker = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL) || defined(MSMAUD_QSC60X0_AUDIO_CAL) || \
    defined(MSMAUD_QSC60X5_AUDIO_CAL)  || defined(MSMAUD_QSC1110_AUDIO_CAL)
  QDSP_INTERNAL_PCM_FORMAT_SPEAKER,
#else
  QDSP_INTERNAL_PCM_FORMAT,
#endif
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec, aux ear, mic1 for wb case */
const qdsp_cmd_codec_config_type voc_cal_codec_speaker_wb = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL) || defined(MSMAUD_QSC60X0_AUDIO_CAL) || \
    defined(MSMAUD_QSC60X5_AUDIO_CAL)  || defined(MSMAUD_QSC1110_AUDIO_CAL)
  QDSP_INTERNAL_PCM_FORMAT_SPEAKER,
#else
  QDSP_INTERNAL_PCM_FORMAT,
#endif
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec, aux ear, mic1 */
const qdsp_cmd_codec_config_type voc_cal_codec_gsm_speaker = {
  QDSP_INTERNAL_PCM_FMT_GSM,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* Internal codec, aux ear, mic1 for wb case */
const qdsp_cmd_codec_config_type voc_cal_codec_gsm_speaker_wb = {
#if defined(MSMAUD_QSC6270_AUDIO_CAL)
#error code not present
#else
  QDSP_INTERNAL_PCM_FMT_GSM,
#endif
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

#endif /* FEATURE_SPEAKER_PHONE */

/* Off-chip, on-board codec */
const qdsp_cmd_codec_config_type voc_cal_codec_on_board = {
  QDSP_PHONE_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_INT_UNUSED,
  QDSP_CODEC_ON_BOARD,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_PCM_V
#else
  QDSP_DMA_DEF
#endif
};

#ifdef FEATURE_BT
/* Bluetooth Intercom codec */
const qdsp_cmd_codec_config_type voc_cal_codec_bt_intercom = {
  QDSP_BT_INTERCOM_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_1,
  QDSP_CODEC_INT,
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
};

#ifdef FEATURE_BT_AG
#ifdef MSMAUD_ALT_BT_AG_CONFIG
#error code not present
#else /* MSMAUD_ALT_BT_AG_CONFIG */

/* Bluetooth AG codec */
const qdsp_cmd_codec_config_type voc_cal_codec_bt_ag = {
  QDSP_BT_AG_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_INT_UNUSED,
  QDSP_CODEC_OFF_BOARD,
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_PCM_V
};

/* FM over BT AG */
const qdsp_cmd_codec_config_type voc_cal_codec_fm_bt_ag = {
  QDSP_FM_BT_AG_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_INT_UNUSED,
  QDSP_CODEC_OFF_BOARD,
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
};

#ifdef FEATURE_AVS_AUDIO_DECODER_SCO
/* Bluetooth AG-MM codec */
const qdsp_cmd_codec_config_type voc_cal_mm_codec_bt_ag = {
  QDSP_BT_AG_MM_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_INT_UNUSED,
  QDSP_CODEC_OFF_BOARD,
  QDSP_DMA_BT_CODEC_INT,       /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_BT_V
};
#endif /* FEATURE_AVS_AUDIO_DECODER_SCO */

#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* MSMAUD_ALT_BT_AG_CONFIG */
#endif /* FEATURE_BT_AG */

#ifdef FEATURE_AVS_I2SSBC
const qdsp_cmd_codec_config_type voc_cal_codec_i2ssbc = {
  QDSP_SADC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_2,
  QDSP_CODEC_SADC,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_SADC,          /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
  /* QDSP_8KHZ_INT_MODE_PCM_V */
#else
  QDSP_DMA_DEF
#endif
};
#endif /* FEATURE_AVS_I2SSBC */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
const qdsp_cmd_codec_config_type voc_cal_codec_bt_sbc = {
  QDSP_SBC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_WB_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF
#endif
};
#endif /* FEATURE_SBC_CODEC */
#endif /* FEATURE_BT */

/* Off-chip, off-board codec */
const qdsp_cmd_codec_config_type voc_cal_codec_off_board = {
  QDSP_AUX_PCM_FORMAT,
  QDSP_AUX_PCM_CTRL_DEF,
  QDSP_CODEC_INT_UNUSED,
  QDSP_CODEC_OFF_BOARD,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_PCM_V
#else
  QDSP_DMA_DEF
#endif
};

#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
/* 16KHz internal codec 0 */
const qdsp_cmd_codec_config_type voc_cal_codec_16k_int0 = {
  QDSP_INT_16K_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_WB_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* 16KHz internal codec 1 */
const qdsp_cmd_codec_config_type voc_cal_codec_16k_int1 = {
  QDSP_INT_16K_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_WB_AMP_2,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* 16KHz internal codec, aux channel  */
const qdsp_cmd_codec_config_type voc_cal_codec_16k_aux = {
  QDSP_INT_16K_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_AUX,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF
#endif
};

#ifdef FEATURE_SPEAKER_PHONE
/* 16KHz internal codec, aux ear, mic1  */
const qdsp_cmd_codec_config_type voc_cal_codec_16k_speaker = {
  QDSP_INT_16K_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_WB_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};
#endif /* FEATURE_SPEAKER_PHONE */
#endif /* FEATURE_QSYNTH_COMMON || FEATURE_QTUNES_COMMON */

#ifdef FEATURE_MM_REC
/* Internal codec 1 */
const qdsp_cmd_codec_config_type voc_cal_codec_int1_mm_rec = {
  QDSP_INTERNAL_PCM_FORMAT_1,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};
/* Support for stereo AAC MM recording */
const qdsp_cmd_codec_config_type voc_cal_codec_int1_mm_stereo_rec = {
  QDSP_INTERNAL_PCM_FORMAT_1,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_4,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF
#endif
};
const qdsp_cmd_codec_config_type voc_cal_codec_16k_int0_mm_rec = {
  QDSP_INT_16K_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

const qdsp_cmd_codec_config_type voc_cal_codec_int0_mm_rec = {
  QDSP_INTERNAL_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* 16KHz internal codec 1 */
const qdsp_cmd_codec_config_type voc_cal_codec_16k_int1_mm_rec = {
  QDSP_INT_16K_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* 16KHz internal codec, aux channel  */
const qdsp_cmd_codec_config_type voc_cal_codec_16k_aux_mm_rec = {
  QDSP_INT_16K_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF
#endif
};

#ifdef FEATURE_SPEAKER_PHONE
/* 16KHz internal codec, aux ear, mic1  */
const qdsp_cmd_codec_config_type voc_cal_codec_16k_speaker_mm_rec = {
  QDSP_INT_16K_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};
#endif /* FEATURE_SPEAKER_PHONE */

#ifdef FEATURE_INTERNAL_SDAC
/* PCM output using Internal Stereo DAC */
const qdsp_cmd_codec_config_type voc_cal_codec_sdac_mm_rec = {
  QDSP_INTERNAL_SDAC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};
#elif defined(FEATURE_EXTERNAL_SDAC)
/* PCM output using External Stereo DAC */
const qdsp_cmd_codec_config_type voc_cal_codec_sdac_mm_rec = {
  QDSP_SDAC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_INT_UNUSED,
  QDSP_CODEC_SDAC,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_SDAC,          /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_PCM_V,
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};
#endif /* #ifdef FEATURE_INTERNAL_SDAC */

#ifdef FEATURE_INTERNAL_SADC
/* PCM output using External Stereo DAC */
const qdsp_cmd_codec_config_type voc_cal_codec_sadc_mono_mm_rec = {
  QDSP_SADC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,          /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

const qdsp_cmd_codec_config_type voc_cal_codec_sadc_stereo_mm_rec = {
  QDSP_SADC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_4,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,          /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

#elif defined(FEATURE_EXTERNAL_SADC)
/* PCM output using External Stereo DAC */
const qdsp_cmd_codec_config_type voc_cal_codec_sadc_mono_mm_rec = {
  QDSP_SADC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_1,
  QDSP_CODEC_SADC,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_SADC,          /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

const qdsp_cmd_codec_config_type voc_cal_codec_sadc_stereo_mm_rec = {
  QDSP_SADC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_2,
  QDSP_CODEC_SADC,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_SADC,          /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
  /* QDSP_8KHZ_INT_MODE_PCM_V */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};
#endif /* FEATURE_INTERNAL_SADC */

const qdsp_cmd_codec_config_type voc_cal_codec_off_board_mm_rec = {
  QDSP_AUX_PCM_FORMAT,
  QDSP_AUX_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_OFF_BOARD,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_PCM_V
#else
  QDSP_DMA_DEF
#endif
};

/* Off-chip, on-board codec */
const qdsp_cmd_codec_config_type voc_cal_codec_on_board_mm_rec = {
  QDSP_PHONE_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_PCM_IN_3,
  QDSP_CODEC_ON_BOARD,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_PCM_V
#else
  QDSP_DMA_DEF
#endif
};
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_INTERNAL_SDAC
/* PCM output using Internal Stereo DAC */
const qdsp_cmd_codec_config_type voc_cal_codec_sdac = {
  QDSP_INTERNAL_SDAC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_WB_AMP_1,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};
#elif defined(FEATURE_EXTERNAL_SDAC)
/* PCM output using External Stereo DAC */
const qdsp_cmd_codec_config_type voc_cal_codec_sdac = {
  QDSP_SDAC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_INT_UNUSED,
  QDSP_CODEC_SDAC,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_SDAC,          /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_PCM_V,
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};
#endif

#ifdef FEATURE_TTY
/* Channel 1 uses same as voc_cal_codec_int1 */

/* TTY in VCO mode on Internal codec 1 */
const qdsp_cmd_codec_config_type voc_cal_codec_tty_vco = {
  QDSP_INTERNAL_PCM_FORMAT_1,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_VCO,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

/* TTY in HCO mode on Internal codec 1 */
const qdsp_cmd_codec_config_type voc_cal_codec_tty_hco = {
  QDSP_INTERNAL_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_HCO,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF,
#endif
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
};

#endif /* FEATURE_TTY */

#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
/* USB analog carkit, on-chip codec   */
const qdsp_cmd_codec_config_type voc_cal_codec_usb = {
  QDSP_INTERNAL_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_AUX,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF
#endif
};

#ifdef FEATURE_INTERNAL_SDAC
/* Stereo USB analog carkit, on-chip codec   */
const qdsp_cmd_codec_config_type voc_cal_codec_stereo_usb = {
  QDSP_INTERNAL_SDAC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_CODEC_CTRL_AMP_AUX,
  QDSP_CODEC_INT,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_DEFAULT,       /* DMA Path control        */
  QDSP_8KHZ_INT_MODE_CODEC_V
#else
  QDSP_DMA_DEF
#endif
};
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */


#if defined(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)

const qdsp_cmd_codec_config_type voc_cal_codec_tx_external_sadc_rx_internal = {
  QDSP_EXT_SADC_PCM_FORMAT,
  QDSP_PCM_CTRL_DEF,
  QDSP_EXT_CODEC_CTRL_DUAL_MIC,
  QDSP_CODEC_EXT_SADC_DUALMIC,
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  QDSP_DMA_PATH_SADC,
#ifdef MSMAUD_SCMM
#error code not present
#else
  QDSP_8KHZ_INT_MODE_CODEC_V,
#endif /* MSMAUD_SCMM */
#else
  QDSP_DMA_DEF
#endif
};

#endif /*(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)*/

#ifdef MSMAUD_PMIC_CONFIG
const voc_pmic_config_type voc_cal_pmic_config_legacy_mic = {
  TRUE,
  FALSE
};
const voc_pmic_config_type voc_cal_pmic_config_legacy_nomic = {
  FALSE,
  FALSE
};
/* Capless mode is only for HPH_L/R (headset/headphone) */
#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
const voc_pmic_config_type voc_cal_pmic_config_capless_mic = {
  TRUE,
  TRUE
};
const voc_pmic_config_type voc_cal_pmic_config_capless_nomic = {
  FALSE,
  TRUE
};
#endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
#endif  /* MSMAUD_PMIC_CONFIG */


/* ===========================================================================
**   ADIE Configuration
** =========================================================================*/

#ifndef MSMAUD_SCMM
#ifdef MSMAUD_ADIE_CODEC

/* ADIE Register values */
CONSTANT adie_codec_reg_type voc_adie_handset_config[] =
  MSMAUD_ADIE_CODEC_HANDSET_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_handset_no_micamp2_config[] =
  MSMAUD_ADIE_CODEC_HANDSET_NO_MICAMP2_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_handset_no_mic_config[] =
  MSMAUD_ADIE_CODEC_HANDSET_NO_MIC_CONFIG_SEQ;

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
CONSTANT adie_codec_reg_type voc_adie_handset_dual_mic_config[] =
  MSMAUD_ADIE_CODEC_HANDSET_DUAL_MIC_CONFIG_SEQ;
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

CONSTANT adie_codec_reg_type voc_adie_mono_headset_config[] =
  MSMAUD_ADIE_CODEC_MONO_HEADSET_CONFIG_SEQ;

/* FM over BT AG */
#if defined (VOCCAL_FM_OVER_BT_AG)
CONSTANT adie_codec_reg_type voc_adie_fm_bt_ag_headset_config[] =
  MSMAUD_ADIE_CODEC_FM_BT_AG_CONFIG_SEQ;
#endif /* VOCCAL_FM_OVER_BT_AG */

#ifdef FEATURE_INTERNAL_SDAC
CONSTANT adie_codec_reg_type voc_adie_stereo_headset_config[] =
  MSMAUD_ADIE_CODEC_STEREO_HEADSET_CONFIG_SEQ;
#endif /* FEATURE_INTERNAL_SDAC */

CONSTANT adie_codec_reg_type voc_adie_mono_headphone_config[] =
  MSMAUD_ADIE_CODEC_MONO_HEADPHONE_CONFIG_SEQ;

#ifdef FEATURE_INTERNAL_SDAC
CONSTANT adie_codec_reg_type voc_adie_stereo_headphone_config[] =
  MSMAUD_ADIE_CODEC_STEREO_HEADPHONE_CONFIG_SEQ;
#endif

CONSTANT adie_codec_reg_type voc_adie_aux_config[] =
  MSMAUD_ADIE_CODEC_AUX_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_aux_no_mic_config[] =
  MSMAUD_ADIE_CODEC_AUX_NO_MIC_CONFIG_SEQ;

#ifdef FEATURE_SPEAKER_PHONE
CONSTANT adie_codec_reg_type voc_adie_speaker_config[] =
  MSMAUD_ADIE_CODEC_SPEAKER_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_speaker_no_mic_config[] =
  MSMAUD_ADIE_CODEC_SPEAKER_NO_MIC_CONFIG_SEQ;

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

#endif /* FEATURE_SPEAKER_PHONE */

CONSTANT adie_codec_reg_type voc_adie_mono_headset_tty_config[] =
  MSMAUD_ADIE_CODEC_MONO_HEADSET_TTY_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_tty_vco_config[] =
  MSMAUD_ADIE_CODEC_TTY_VCO_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_tty_hco_config[] =
  MSMAUD_ADIE_CODEC_TTY_HCO_CONFIG_SEQ;

#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
CONSTANT adie_codec_reg_type voc_adie_usb_config[] =
  MSMAUD_ADIE_CODEC_USB_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_usb_no_mic_config[] =
  MSMAUD_ADIE_CODEC_USB_NO_MIC_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_stereo_usb_config[] =
  MSMAUD_ADIE_CODEC_STEREO_USB_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_stereo_usb_no_mic_config[] =
  MSMAUD_ADIE_CODEC_STEREO_USB_NO_MIC_CONFIG_SEQ;
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
CONSTANT adie_codec_reg_type voc_adie_pwr_down_config[] =
  MSMAUD_ADIE_CODEC_PD_CONFIG_SEQ;
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_INTERNAL_SADC
CONSTANT adie_codec_reg_type voc_adie_mono_sadc_mono_handset_config[] =
  MSMAUD_ADIE_CODEC_MONO_SADC_MONO_HANDSET_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_mono_sadc_stereo_headset_config[] =
  MSMAUD_ADIE_CODEC_MONO_SADC_STEREO_HEADSET_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_stereo_sadc_mono_handset_config[] =
  MSMAUD_ADIE_CODEC_STEREO_SADC_MONO_HANDSET_CONFIG_SEQ;

CONSTANT adie_codec_reg_type voc_adie_stereo_sadc_stereo_headset_config[] =
  MSMAUD_ADIE_CODEC_STEREO_SADC_STEREO_HEADSET_CONFIG_SEQ;
#endif /* FEATURE_INTERNAL_SADC */

/* ADIE modes */
/* Handset: Earpiece and MIC1 */
CONSTANT voc_adie_codec_config_type voc_adie_codec_handset_config =
{
  sizeof(voc_adie_handset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_HANDSET_DELAY_CONFIG_SIZE,
  voc_adie_handset_config
};

/* Handset: Earpiece and MIC1 with low codec gain and bypass MICAMP2 */
CONSTANT voc_adie_codec_config_type voc_adie_codec_handset_no_micamp2_config =
{
  sizeof(voc_adie_handset_no_micamp2_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_HANDSET_DELAY_CONFIG_SIZE,
  voc_adie_handset_no_micamp2_config
};

/* Handset: Earpiece and NO mic inputs */
CONSTANT voc_adie_codec_config_type voc_adie_codec_handset_no_mic_config =
{
  sizeof(voc_adie_handset_no_mic_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_HANDSET_NO_MIC_DELAY_CONFIG_SIZE,
  voc_adie_handset_no_mic_config
};

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/* Handset: Earpiece and dual mic inputs */
CONSTANT voc_adie_codec_config_type voc_adie_codec_handset_dual_mic_config =
{
  sizeof(voc_adie_handset_dual_mic_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_HANDSET_DUAL_MIC_DELAY_CONFIG_SIZE,
  voc_adie_handset_dual_mic_config
};
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

/* Headset: MIC2 and left headphone channel */
CONSTANT voc_adie_codec_config_type voc_adie_codec_mono_headset_config =
{
  sizeof(voc_adie_mono_headset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_MONO_HEADSET_DELAY_CONFIG_SIZE,
  voc_adie_mono_headset_config
};

/* FM over BT AG */
#if defined (VOCCAL_FM_OVER_BT_AG)
CONSTANT voc_adie_codec_config_type voc_adie_codec_fm_bt_ag_config =
{
  sizeof(voc_adie_fm_bt_ag_headset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_FM_BT_AG_DELAY_CONFIG_SIZE,
  voc_adie_fm_bt_ag_headset_config
};
#else
/* This target do not support FM over BT AG Adie settings */
CONSTANT voc_adie_codec_config_type voc_adie_codec_fm_bt_ag_config =
{
  0,
  0,
  NULL
};
#endif /* VOCCAL_FM_OVER_BT_AG */

/* Simulated Stereo Headset: left headphone channel fed to
 * both left and right, plus MIC2 input.
 */
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
CONSTANT voc_adie_codec_config_type voc_adie_codec_simul_stereo_headset_config =
{
#ifdef FEATURE_INTERNAL_SDAC
  sizeof(voc_adie_stereo_headset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_STEREO_HEADSET_DELAY_CONFIG_SIZE,
  voc_adie_stereo_headset_config
#else
  0, 0, NULL
#endif /* FEATURE_INTERNAL_SDAC */
};
#endif /* FEATURE_STEREO_HEADSET && FEATURE_INTERNAL_SDAC */

CONSTANT voc_adie_codec_config_type voc_adie_codec_headset_config =
{
#if defined(FEATURE_STEREO_HEADSET) && defined(FEATURE_INTERNAL_SDAC)
#error code not present
#else
  /* Headset: MIC2 and left headphone channel */
  sizeof(voc_adie_mono_headset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_MONO_HEADSET_DELAY_CONFIG_SIZE,
  voc_adie_mono_headset_config
#endif /* FEATURE_STEREO_HEADSET && FEATURE_INTERNAL_SDAC */
};

/* Mono Headphone: left headphone channel (no mic input) */
CONSTANT voc_adie_codec_config_type voc_adie_codec_mono_headphone_config =
{
  sizeof(voc_adie_mono_headphone_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_MONO_HEADPHONE_DELAY_CONFIG_SIZE,
  voc_adie_mono_headphone_config
};

/* Stereo Headphone: left + right headphone channels (no mic) */
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
CONSTANT voc_adie_codec_config_type voc_adie_codec_stereo_headphone_config =
{
#ifdef FEATURE_INTERNAL_SDAC
  sizeof(voc_adie_stereo_headphone_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_STEREO_HEADPHONE_DELAY_CONFIG_SIZE,
  voc_adie_stereo_headphone_config
#else
  0, 0, NULL
#endif /* FEATURE_INTERNAL_SDAC */
};
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */


#ifdef FEATURE_SPEAKER_PHONE
/* Speaker phone: Auxillary outputs and MIC1 */
CONSTANT voc_adie_codec_config_type voc_adie_codec_speaker_config =
{
  sizeof(voc_adie_speaker_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_SPEAKER_DELAY_CONFIG_SIZE,
  voc_adie_speaker_config
};

/* Speaker phone: Auxillary outputs (no mic input */
CONSTANT voc_adie_codec_config_type voc_adie_codec_speaker_no_mic_config =
{
  sizeof(voc_adie_speaker_no_mic_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_SPEAKER_NO_MIC_DELAY_CONFIG_SIZE,
  voc_adie_speaker_no_mic_config
};

#ifdef FEATURE_FM_OEM
#define MSMAUD_ADIE_CODEC_HEADSET_FM_DELAY_CONFIG_SIZE 16

const adie_codec_reg_type voc_adie_headset_fm_config[] =
{                                                                  \
	/* Enable bandgap */													\
	ADIE_REFERENCE_CONTROL1_R,		   ADIE_REFERENCE_CONTROL1_INIT_M,			ADIE_REF_CNTL1_BANDGAP_OPAMP_M, 0, \
	ADIE_REFERENCE_CONTROL2_R,		   ADIE_REFERENCE_CONTROL2_INIT_M,			ADIE_REFERENCE_CONTROL2_INIT_V, 		  0, \
	/* Turn on bias current and reference buffer for hph L */																						\
	/*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,		 ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,	  (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V 		   \
															 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V								\
															 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V					\
															 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V					\
															 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),				 0, \
	/* Configure Tx control registers */																											\
	/*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R, 	 ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,	  (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V						\
															 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V						\
															 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V ),			 0, \
	/*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R, 	 ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,	  ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,					  0, \
	ADIE_CODEC_TX_ADC_REGISTER_R,	   ADIE_CODEC_TX_ADC_REGISTER_INIT_M,		ADIE_CODEC_TX_ADC_REF_DIS_V,									  0, \
	ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_05_UA_V, 			0, \
	ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_V,							  0, \
	ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,	  0, \
	/* Configure AUX_PGA control registers */																											\
	/*0x86*/ADIE_AUX_PGA_CONTROL1_R,			 ADIE_AUX_PGA_CONTROL1_INIT_M,			  (ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V							 \
															|ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_08P5_DB_V						 \
															| ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V), 							   0, \
	/* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/																						\
	/* Configure Rx filter control registers */ 																											\
	/*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V 											  \
															 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V											\
															 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V											\
															 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V),									   0, \
	/*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,										   0, \
	/*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V 							  \
															 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),								   0, \
	/*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V,										   0, \
	/* Configure Rx PA control registers */ 																																\
	/*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,		 ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,	  ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,											   0, \
	/*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R, 	 ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,	  ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,								0, \
    /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
    /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    ADIE_RX_PA_ENABLE_REGISTER1_INIT_V,                                               0, \
    /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                            0, \
    /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
    /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
    /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V | 0x1),                                0, \
    /* Turn on HPH L/R PA's */                                               \
    /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                                       \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                           \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V | 0x1),                                           0, \
    /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                   | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
    /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                            0, \
    /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                   | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V),                                  0, \
    /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
    /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
    /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
    /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
    /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
    /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
    /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
    /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
    /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
    /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
    /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
    /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
};

//add miaoxiaoming
#define MSMAUD_ADIE_CODEC_SPEAKER_FM_DELAY_CONFIG_SIZE 8
const adie_codec_reg_type voc_adie_speaker_fm_config[] = 
{                                                                   \
 /* Enable bandgap */													\
	/*0x83*/ADIE_REFERENCE_CONTROL1_R,		   ADIE_REFERENCE_CONTROL1_INIT_M,			ADIE_REFERENCE_CONTROL1_INIT_V , 0, \
	/*0x84*/ADIE_REFERENCE_CONTROL2_R,		   ADIE_REFERENCE_CONTROL2_INIT_M,			ADIE_REFERENCE_CONTROL2_INIT_V, 		  0, \
	/*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R, 	 ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,	  ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,	0,					\
	/* Turn on bias current and reference buffer for hph L */																						\
	 /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                              /*40ms*/0, \
	/* Configure Tx control registers */																											\
	/*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R, 	 ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,	  (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V						\
															 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V						\
															 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V					\
															 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V ),			 0, \
	/*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R, 	 ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,	  ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,					  0, \
	/*0x8b*/ADIE_CODEC_TX_ADC_REGISTER_R,	   ADIE_CODEC_TX_ADC_REGISTER_INIT_M,		ADIE_CODEC_TX_ADC_REF_DIS_V,									  0, \
	/*0x8c*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_V, 			0, \
	/*0x8d*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_V,							  0, \
	/*0x8e*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_V,	  0, \
	/* Configure AUX_PGA control registers */																											\
	/*0x86*/ADIE_AUX_PGA_CONTROL1_R,			 ADIE_AUX_PGA_CONTROL1_INIT_M,			  (ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V							 \
															|ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_08P5_DB_V						 \
															| ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V), 							   0, \
	/* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/																						\
	/* Configure Rx filter control registers */ 																											\
	/*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V,									   0, \
	/*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,										   0, \
	/*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V,								   0, \
	/*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V,										   0, \
	/* Configure Rx PA control registers */ 																																\
	/*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,		 ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,	  ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V \
                                                                                        	|ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V  \
                                                                                        	|ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V \
                                                                                        	|ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V,											   0, \
	/*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R, 	 ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,	  ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V,								0, \
	/*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R, 	 ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,	  ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,											   0, \
	/*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R, 	 ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,	  ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,    0, \
	/*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R, 	 ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,	  ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_10P0_UA_V,								   0, \
	/*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R, 	 ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,	  ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_ENA_V,											   0, \
  
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_STEREO_V          \
                                                                                                    | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),              0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
};
	
//wangliang add!
const voc_adie_codec_config_type voc_adie_codec_headset_fm_config =
{
  sizeof(voc_adie_headset_fm_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_HEADSET_FM_DELAY_CONFIG_SIZE,
  voc_adie_headset_fm_config
};

//const adie_codec_reg_type voc_adie_speaker_fm_config[] =
//	MSMAUD_ADIE_CODEC_MONO_SPEAKER_FM_CONFIG_SEQ;

//wangliang add!
const voc_adie_codec_config_type voc_adie_codec_speaker_fm_config =
{
  sizeof(voc_adie_speaker_fm_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_SPEAKER_FM_DELAY_CONFIG_SIZE,
  voc_adie_speaker_fm_config
};
#endif

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

#endif /* FEATURE_SPEAKER_PHONE */


/* Auxillary device: MIC and left + right aux channels */
CONSTANT voc_adie_codec_config_type voc_adie_codec_aux_config =
{
  sizeof(voc_adie_aux_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_DELAY_CONFIG_SIZE,
  voc_adie_aux_config
};

/* Auxillary device:  left + right aux channels and NO mic input */
CONSTANT voc_adie_codec_config_type voc_adie_codec_aux_no_mic_config =
{
  sizeof(voc_adie_aux_no_mic_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_NO_MIC_DELAY_CONFIG_SIZE,
  voc_adie_aux_no_mic_config
};

/* Headset: MIC2 and left headphone channel */
CONSTANT voc_adie_codec_config_type voc_adie_codec_mono_headset_tty_config =
{
  sizeof(voc_adie_mono_headset_tty_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_MONO_HEADSET_TTY_DELAY_CONFIG_SIZE,
  voc_adie_mono_headset_tty_config
};


/* TTY in VCO mode: MIC1 and left headphone channel */
CONSTANT voc_adie_codec_config_type voc_adie_codec_tty_vco_config =
{
  sizeof(voc_adie_tty_vco_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_TTY_VCO_DELAY_CONFIG_SIZE,
  voc_adie_tty_vco_config
};

/* TTY in HCO mode: MIC2 and handset earpiece */
CONSTANT voc_adie_codec_config_type voc_adie_codec_tty_hco_config =
{
  sizeof(voc_adie_tty_hco_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_TTY_HCO_DELAY_CONFIG_SIZE,
  voc_adie_tty_hco_config
};

#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
/* Auxillary device: USB Carkit: MIC and left aux channel */
CONSTANT voc_adie_codec_config_type voc_adie_codec_usb_config =
{
  sizeof(voc_adie_usb_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_USB_DELAY_CONFIG_SIZE,
  voc_adie_usb_config
};

/* Auxillary device: USB Carkit: No MIC and left aux channel */
CONSTANT voc_adie_codec_config_type voc_adie_codec_usb_no_mic_config =
{
  sizeof(voc_adie_usb_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_USB_NO_MIC_DELAY_CONFIG_SIZE,
  voc_adie_usb_no_mic_config
};


/* Simulated Stereo USB: left aux channel fed to
 * both left and right, plus MIC input.
 */
CONSTANT voc_adie_codec_config_type voc_adie_codec_simul_stereo_usb_config =
{
  sizeof(voc_adie_stereo_usb_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_STEREO_USB_DELAY_CONFIG_SIZE,
  voc_adie_stereo_usb_config
};

/* Auxillary device: USB Carkit: No MIC and left & right aux channel */
CONSTANT voc_adie_codec_config_type voc_adie_codec_stereo_usb_no_mic_config =
{
  sizeof(voc_adie_stereo_usb_no_mic_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_STEREO_USB_NO_MIC_DELAY_CONFIG_SIZE,
  voc_adie_stereo_usb_no_mic_config
};
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
CONSTANT voc_adie_codec_config_type voc_adie_codec_pwr_down_config = {
  sizeof(voc_adie_pwr_down_config)/sizeof(adie_codec_reg_type),
  0,
  voc_adie_pwr_down_config
};
#endif /*FEATURE_SBC_CODEC */

#ifdef FEATURE_INTERNAL_SADC
CONSTANT
voc_adie_codec_config_type voc_adie_codec_mono_sadc_mono_handset_config =
{
  sizeof(voc_adie_mono_sadc_mono_handset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_MONO_SADC_MONO_HANDSET_DELAY_CONFIG_SIZE,
  voc_adie_mono_sadc_mono_handset_config
};

CONSTANT
voc_adie_codec_config_type voc_adie_codec_mono_sadc_stereo_headset_config =
{
  sizeof(voc_adie_mono_sadc_stereo_headset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_MONO_SADC_STEREO_HEADSET_DELAY_CONFIG_SIZE,
  voc_adie_mono_sadc_stereo_headset_config
};

CONSTANT
voc_adie_codec_config_type voc_adie_codec_stereo_sadc_mono_handset_config =
{
  sizeof(voc_adie_stereo_sadc_mono_handset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_STEREO_SADC_MONO_HANDSET_DELAY_CONFIG_SIZE,
  voc_adie_stereo_sadc_mono_handset_config
};

CONSTANT
voc_adie_codec_config_type voc_adie_codec_stereo_sadc_stereo_headset_config =
{
 sizeof(voc_adie_stereo_sadc_stereo_headset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_STEREO_SADC_STEREO_HEADSET_DELAY_CONFIG_SIZE,
  voc_adie_stereo_sadc_stereo_headset_config
};
#endif /* FEATURE_INTERNAL_SADC */
#endif /* MSMAUD_ADIE_CODEC */

#endif /* MSMAUD_SCMM */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
** Codec and ADIE configuration data, by image and path.
*/
typedef enum {
  VOC_CAL_CODEC_CFG_VOC,
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  VOC_CAL_CODEC_CFG_VOC_ADEC_MIXER,
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
  VOC_CAL_CODEC_CFG_SYNTH,
#endif
#ifdef FEATURE_MM_REC
  VOC_CAL_CODEC_CFG_MMREC,
#endif /* FEATURE_MM_REC */
#ifdef FEATURE_AVS_I2SSBC
  VOC_CAL_CODEC_CFG_I2SSBC,
#endif /* FEATURE_AVS_I2SSBC */
  VOC_CAL_CODEC_CFG_WB_VOC,
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
  VOC_CAL_CODEC_CFG_MAX
} voc_cal_codec_cfg_type;

typedef enum {
  VOC_CAL_PCM_PATH_CAL_CDMA,
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
  VOC_CAL_PCM_PATH_CAL_CDMA_WB,
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
  VOC_CAL_PCM_PATH_CAL_MAX
} voc_cal_pcm_path_cfg_type;

voc_cal_codec_cfg_type voc_codec_mode_to_voc_cal_codec_cfg[((uint32) VOC_CODEC_MODE_MAX)+1] =
{
  VOC_CAL_CODEC_CFG_VOC,                    /* VOC_CODEC_MODE_CDMA_VOICE */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  VOC_CAL_CODEC_CFG_VOC_ADEC_MIXER,         /* VOC_CODEC_MODE_CDMA_VOC_ADEC_MIXER */
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
  VOC_CAL_CODEC_CFG_SYNTH,                  /* VOC_CODEC_MODE_MULTIMEDIA */
#endif
#ifdef FEATURE_MM_REC
  VOC_CAL_CODEC_CFG_MMREC,                  /* VOC_CODEC_MODE_MM_REC */
#endif /* FEATURE_MM_REC */
#ifdef FEATURE_AVS_I2SSBC
  VOC_CAL_CODEC_CFG_I2SSBC,                 /* VOC_CODEC_MODE_I2SSBC */
#endif /* FEATURE_AVS_I2SSBC */
  VOC_CAL_CODEC_CFG_WB_VOC,                 /* VOC_CODEC_MODE_CDMA_WB_VOICE */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
  VOC_CAL_CODEC_CFG_MAX                     /* VOC_CODEC_MODE_MAX */
};

/* Audio path configuration - CODEC + ADIE config */
typedef struct {
  const qdsp_cmd_codec_config_type   *codec_config;
  #ifndef MSMAUD_SCMM
  CONSTANT voc_adie_codec_config_type   *adie_config;
  #endif /* MSMAUD_SCMM */
#ifdef MSMAUD_PMIC_CONFIG
  const voc_pmic_config_type         *pmic_config;
#endif
} voc_cal_audio_path_config_type;

#ifndef MSMAUD_SCMM
#if defined (MSMAUD_ADIE_CODEC) && defined (MSMAUD_PMIC_CONFIG)
#define AUDIO_PATH_CONFIG(v,x,y) (v), (x), (y)
#elif defined (MSMAUD_ADIE_CODEC)
#define AUDIO_PATH_CONFIG(v,x,y) (v), (x)
#else
#define AUDIO_PATH_CONFIG(v,x,y) (v)
#endif
#else
#error code not present
#endif /* MSMAUD_SCMM */

/* Audio path based on DSP image type and codec type */
const voc_cal_audio_path_config_type
      voc_cal_audio_path_config[VOC_CAL_CODEC_CFG_MAX][VOC_CODEC_MAX] =
{
  /*********************** Vocoder image ***********************/
  {
    /* VOC_CODEC_ON_CHIP_0             */
    { AUDIO_PATH_CONFIG(&voc_cal_codec_int0,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    /* VOC_CODEC_ON_CHIP_0_DUAL_MIC    */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_dual_mic,
                        &voc_adie_codec_handset_dual_mic_config,
                        &voc_cal_pmic_config_legacy_mic) }
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

    /* VOC_CODEC_ON_CHIP_1             */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    /* VOC_CODEC_ON_CHIP_AUX           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_aux,
                        &voc_adie_codec_aux_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_ON_BOARD              */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_on_board, NULL, NULL) }

    #ifdef FEATURE_SPEAKER_PHONE
    /* VOC_CODEC_SPEAKER_PHONE         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_speaker,
                        &voc_adie_codec_speaker_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_SPEAKER_PHONE */

    #ifdef FEATURE_BT
    /* VOC_CODEC_BT_INTERCOM           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_intercom,
                        &voc_adie_codec_handset_no_micamp2_config,
                        &voc_cal_pmic_config_legacy_mic) }

    #ifdef FEATURE_BT_AG
    /* VOC_CODEC_BT_AG                 */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL) }

    #ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
    #endif /* FEATURE_BT_AG_LOCAL_AUDIO */
    #endif /* FEATURE_BT_AG */

    #if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* VOC_CODEC_BT_SBC                */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL) }

    /* VOC_CODEC_BT_DUAL               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL) }
    #endif /* FEATURE_SBC_CODEC */
    #endif /* FEATURE_BT */

    /* VOC_CODEC_OFF_BOARD             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL) }


    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_SDAC                  */
    #ifdef FEATURE_INTERNAL_SDAC
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    #if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
    /* VOC_CODEC_ON_CHIP_0             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #if defined(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_tx_external_sadc_rx_internal,
                        &voc_adie_codec_handset_no_mic_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #else
    /* VOC_CODEC_ON_CHIP_0             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /*(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)*/

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */

    #ifdef FEATURE_TTY
    /* VOC_CODEC_TTY_ON_CHIP_1         */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_mono_headset_tty_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_mono_headset_tty_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_TTY_OFF_BOARD         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL) }

    /* VOC_CODEC_TTY_VCO               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_tty_vco,
                        &voc_adie_codec_tty_vco_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_TTY_HCO               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_tty_hco,
                        &voc_adie_codec_tty_hco_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_TTY */

    #if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    /* VOC_CODEC_USB                   */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_usb,
                        &voc_adie_codec_usb_config,
                        &voc_cal_pmic_config_legacy_mic)}

    #ifdef FEATURE_INTERNAL_SDAC
    /* VOC_CODEC_STEREO_USB            */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_usb,
                        &voc_adie_codec_usb_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

    /* FM over BT AG */
    #ifdef FEATURE_BT_AG
    ,{ AUDIO_PATH_CONFIG( &voc_cal_codec_fm_bt_ag,
                          &voc_adie_codec_fm_bt_ag_config,
                          &voc_cal_pmic_config_legacy_mic) }
    #endif

	/* VOC_CODEC_ON_CHIP_0_SURF             */
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_ON_CHIP_1_SURF             */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

   /* VOC_CODEC_SPEAKER_PHONE_SURF         */
    #ifdef FEATURE_SPEAKER_PHONE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_speaker,
                        &voc_adie_codec_speaker_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM
		/* VOC_CODEC_STEREO_HEADSET 	   */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
	   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
							&voc_adie_codec_headset_fm_config,
							&voc_cal_pmic_config_capless_mic) }
    #else
	   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
							&voc_adie_codec_headset_fm_config,
							&voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

      ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_speaker_fm_config,
                        &voc_cal_pmic_config_legacy_mic) }
#endif
    }

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  ,{
    /* VOC_CODEC_ON_CHIP_0             */
    { AUDIO_PATH_CONFIG(&voc_cal_codec_int0_mixer,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    /* VOC_CODEC_ON_CHIP_0_DUAL_MIC    */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_dual_mic_mixer,
                        &voc_adie_codec_handset_dual_mic_config,
                        &voc_cal_pmic_config_legacy_mic) }
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

    /* VOC_CODEC_ON_CHIP_1             */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    /* VOC_CODEC_ON_CHIP_AUX           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_aux,
                        &voc_adie_codec_aux_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_ON_BOARD              */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_on_board, NULL, NULL) }

    #ifdef FEATURE_SPEAKER_PHONE
    /* VOC_CODEC_SPEAKER_PHONE         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_speaker,
                        &voc_adie_codec_speaker_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_SPEAKER_PHONE */


    #ifdef FEATURE_BT
    /* VOC_CODEC_BT_INTERCOM           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_intercom,
                        &voc_adie_codec_handset_no_micamp2_config,
                        &voc_cal_pmic_config_legacy_mic) }

    #ifdef FEATURE_BT_AG
    /* VOC_CODEC_BT_AG                 */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag_mixer, NULL, NULL) }

    #ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
    #endif /* FEATURE_BT_AG_LOCAL_AUDIO */
    #endif /* FEATURE_BT_AG */

    #if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* VOC_CODEC_BT_SBC */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL) }

    /* VOC_CODEC_BT_DUAL */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL) }
    #endif /* FEATURE_SBC_CODEC */

    #endif /* FEATURE_BT */


    /* VOC_CODEC_OFF_BOARD             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL) }

    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    #ifdef FEATURE_INTERNAL_SDAC
    /* VOC_CODEC_SDAC                  */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_mixer,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    #if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
    /* VOC_CODEC_ON_CHIP_0             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_mixer,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #if defined(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_tx_external_sadc_rx_internal,
                        &voc_adie_codec_handset_no_mic_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #else
    /* VOC_CODEC_ON_CHIP_0             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_mixer,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /*(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)*/

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */

    #ifdef FEATURE_TTY
    /* VOC_CODEC_TTY_ON_CHIP_1         */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_mono_headset_tty_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_mono_headset_tty_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_TTY_OFF_BOARD         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL) }

    /* VOC_CODEC_TTY_VCO               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_tty_vco,
                        &voc_adie_codec_tty_vco_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_TTY_HCO               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_tty_hco,
                        &voc_adie_codec_tty_hco_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_TTY */

    #if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    /* VOC_CODEC_USB                   */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_usb,
                        &voc_adie_codec_usb_config,
                        &voc_cal_pmic_config_legacy_mic) }

    #ifdef FEATURE_INTERNAL_SDAC
   /* VOC_CODEC_STEREO_USB             */
  ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_usb,
                        &voc_adie_codec_usb_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

   /* FM over BT AG */
   #ifdef FEATURE_BT_AG
  ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_fm_bt_ag,
                       &voc_adie_codec_fm_bt_ag_config,
                       &voc_cal_pmic_config_legacy_mic) }
   #endif

    /* VOC_CODEC_ON_CHIP_0_SURF             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_mixer,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_ON_CHIP_1_SURF             */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mixer,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_SPEAKER_PHONE_SURF         */
    #ifdef FEATURE_SPEAKER_PHONE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_speaker,
                        &voc_adie_codec_speaker_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM   
	#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_headset_fm_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_headset_fm_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_speaker_fm_config,
                        &voc_cal_pmic_config_legacy_mic) }
#endif
  }
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

  /*********************** SYNTH, TUNES, AAC ***********************/

#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
 ,{
    /* VOC_CODEC_ON_CHIP_0             */
    { AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL) }

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    /* VOC_CODEC_ON_CHIP_0_DUAL_MIC    */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL) }
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

    /* VOC_CODEC_ON_CHIP_1             */
    #if defined(FEATURE_STEREO_HEADSET) && defined(FEATURE_INTERNAL_SDAC)
#error code not present
    #else
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_STEREO_HEADSET && FEATURE_INTERNAL_SDAC */

    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    /* VOC_CODEC_ON_CHIP_AUX           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_aux,
                        &voc_adie_codec_aux_no_mic_config, NULL) }

    /* VOC_CODEC_ON_BOARD              */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_on_board, NULL, NULL)}


    #ifdef FEATURE_SPEAKER_PHONE
    /* VOC_CODEC_SPEAKER_PHONE         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_speaker,
                        &voc_adie_codec_speaker_no_mic_config, NULL) }
    #endif /* FEATURE_SPEAKER_PHONE */

    #ifdef FEATURE_BT
    /* VOC_CODEC_BT_INTERCOM           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL) }
    #ifdef FEATURE_BT_AG
    #ifdef FEATURE_AVS_AUDIO_DECODER_SCO
    /* VOC_CODEC_BT_MM_AG              */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_mm_codec_bt_ag,NULL, NULL) }
    #else
    /* VOC_CODEC_BT_AG                 */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL) }
    #endif /*FEATURE_AVS_AUDIO_DECODER_SCO*/
    #ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
    #endif /* FEATURE_BT_AG_LOCAL_AUDIO */
    #endif /* FEATURE_BT_AG */

    #if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_sbc,
                        &voc_adie_codec_pwr_down_config, NULL) }
                           /* VOC_CODEC_BT_A2DP_SCO */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_sbc,
                        &voc_adie_codec_pwr_down_config, NULL) }
    #endif /* FEATURE_SBC_CODEC */
    #endif /* FEATURE_BT */

    /* VOC_CODEC_OFF_BOARD             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL) }

    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_SDAC                  */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_capless_nomic) }
    #else
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    #if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
    /* VOC_CODEC_ON_CHIP_0             */
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL) }

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_capless_nomic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_ON_CHIP_0             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL) }

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_capless_nomic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif  /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC*/

    #ifdef FEATURE_TTY
    /* VOC_CODEC_TTY_ON_CHIP_1         */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic) }
    #else
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_TTY_OFF_BOARD         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL) }

    /* VOC_CODEC_TTY_VCO               */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_TTY_HCO               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL) }
    #endif /* FEATURE_TTY */

    #if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    /* VOC_CODEC_USB                   */
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_usb,
                        &voc_adie_codec_usb_no_mic_config, NULL) }
    #ifdef FEATURE_INTERNAL_SDAC
    /* VOC_CODEC_STEREO_USB            */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_stereo_usb,
                        &voc_adie_codec_stereo_usb_no_mic_config, NULL) }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

    /* FM over BT AG */
    #ifdef FEATURE_BT_AG
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_fm_bt_ag,
                         &voc_adie_codec_fm_bt_ag_config,
                         &voc_cal_pmic_config_legacy_mic) }
    #endif

    /* VOC_CODEC_ON_CHIP_0_SURF             */
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL) }

    /* VOC_CODEC_ON_CHIP_1_SURF             */
    #if defined(FEATURE_STEREO_HEADSET) && defined(FEATURE_INTERNAL_SDAC)
#error code not present
    #else
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_STEREO_HEADSET && FEATURE_INTERNAL_SDAC */

    /* VOC_CODEC_SPEAKER_PHONE_SURF         */
    #ifdef FEATURE_SPEAKER_PHONE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_speaker,
                        &voc_adie_codec_speaker_no_mic_config, NULL) }
    #endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
	   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
							&voc_adie_codec_headset_fm_config,
							&voc_cal_pmic_config_capless_mic) }
    #else
	   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
							&voc_adie_codec_headset_fm_config,
							&voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
       ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                            &voc_adie_codec_speaker_fm_config,
                            &voc_cal_pmic_config_legacy_mic) }
#endif

  }
#endif /* FEATURE_QSYNTH_COMMON || FEATURE_QTUNES_COMMON */

#ifdef FEATURE_MM_REC /* VOC_CAL_CODEC_CFG_MMREC */
 ,{
    /* VOC_CODEC_ON_CHIP_0             */
    { AUDIO_PATH_CONFIG(&voc_cal_codec_int0_mm_rec,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    /* VOC_CODEC_ON_CHIP_0_DUAL_MIC    */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_mm_rec,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

    /* VOC_CODEC_ON_CHIP_1             */
    #if defined(FEATURE_STEREO_HEADSET) && defined(FEATURE_INTERNAL_SDAC)
#error code not present
    #else
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mm_rec,
                        &voc_adie_codec_mono_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mm_rec,
                        &voc_adie_codec_mono_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_STEREO_HEADSET && FEATURE_INTERNAL_SDAC */

    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mm_stereo_rec,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mm_stereo_rec,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    /* VOC_CODEC_ON_CHIP_AUX           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_aux_mm_rec,
                        &voc_adie_codec_aux_no_mic_config, NULL) }

    /* VOC_CODEC_ON_BOARD              */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_on_board_mm_rec, NULL, NULL) }

    #ifdef FEATURE_SPEAKER_PHONE
    /* VOC_CODEC_SPEAKER_PHONE         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_speaker_mm_rec,
                        &voc_adie_codec_speaker_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_SPEAKER_PHONE */

    #ifdef FEATURE_BT
    /* VOC_CODEC_BT_INTERCOM           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0_mm_rec,
                        &voc_adie_codec_handset_no_mic_config, NULL) }

    #ifdef FEATURE_BT_AG
    /* VOC_CODEC_BT_AG                 */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0_mm_rec,
                        &voc_adie_codec_handset_no_mic_config, NULL) }

    #ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
    #endif /* FEATURE_BT_AG_LOCAL_AUDIO */
    #endif /* FEATURE_BT_AG */

    #if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_sbc,
                        &voc_adie_codec_pwr_down_config, NULL) }
                           /* VOC_CODEC_BT_A2DP_SCO */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_sbc,
                        &voc_adie_codec_pwr_down_config, NULL) }
    #endif /* FEATURE_SBC_CODEC */
    #endif /* FEATURE_BT */

    /* VOC_CODEC_OFF_BOARD             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board_mm_rec, NULL, NULL) }

    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_SDAC                  */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    #if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
    #ifdef FEATURE_INTERNAL_SADC
    /* VOC_CODEC_MONO_SADC_MONO_HANDSET */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_mono_mm_rec,
                        &voc_adie_codec_mono_sadc_mono_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_MONO_SADC_STEREO_HEADSET */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_mono_mm_rec,
                       &voc_adie_codec_mono_sadc_stereo_headset_config,
                       &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_mono_mm_rec,
                       &voc_adie_codec_mono_sadc_stereo_headset_config,
                       &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_STEREO_SADC_MONO_HANDSET */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_stereo_mm_rec,
                       &voc_adie_codec_stereo_sadc_mono_handset_config,
                       &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_STEREO_SADC_STEREO_HEADSET */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_stereo_mm_rec,
                       &voc_adie_codec_stereo_sadc_stereo_headset_config,
                       &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_stereo_mm_rec,
                       &voc_adie_codec_stereo_sadc_stereo_headset_config,
                       &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #else
    /* VOC_CODEC_MONO_SADC_MONO_HANDSET */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_mono_mm_rec,
                        &voc_adie_codec_handset_no_mic_config, NULL) }
                       /* &voc_adie_codec_handset_config */

    /* VOC_CODEC_MONO_SADC_STEREO_HEADSET */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_mono_mm_rec,
                       &voc_adie_codec_stereo_headphone_config,
                       &voc_cal_pmic_config_capless_nomic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_mono_mm_rec,
                       &voc_adie_codec_stereo_headphone_config,
                       &voc_cal_pmic_config_legacy_nomic) }
    #endif   /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_STEREO_SADC_MONO_HANDSET */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_stereo_mm_rec,
                       &voc_adie_codec_handset_no_mic_config, NULL) }
                       /* &voc_adie_codec_handset_config */

    /* VOC_CODEC_STEREO_SADC_STEREO_HEADSET */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_stereo_mm_rec,
                       &voc_adie_codec_stereo_headphone_config,
                       &voc_cal_pmic_config_capless_nomic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sadc_stereo_mm_rec,
                       &voc_adie_codec_stereo_headphone_config,
                       &voc_cal_pmic_config_legacy_nomic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #endif /* FEATURE_INTERNAL_SADC */
    #endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */

    #ifdef FEATURE_TTY
    /* VOC_CODEC_TTY_ON_CHIP_1         */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mm_rec,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mm_rec,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_TTY_OFF_BOARD         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL) }

    /* VOC_CODEC_TTY_VCO               */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1_mm_rec,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1_mm_rec,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_TTY_HCO               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0_mm_rec,
                        &voc_adie_codec_handset_no_mic_config, NULL) }
    #endif /* FEATURE_TTY */

    #if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    /* VOC_CODEC_USB                   */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_usb,
                        &voc_adie_codec_usb_no_mic_config, NULL) }
    #ifdef FEATURE_INTERNAL_SDAC
    /* VOC_CODEC_STEREO_USB            */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_stereo_usb,
                        &voc_adie_codec_stereo_usb_no_mic_config, NULL) }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

    /* FM over BT AG */
    #ifdef FEATURE_BT_AG
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_fm_bt_ag,
                         &voc_adie_codec_fm_bt_ag_config,
                         &voc_cal_pmic_config_legacy_mic) }
    #endif

    /* VOC_CODEC_ON_CHIP_0_SURF             */
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_mm_rec,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_ON_CHIP_1_SURF             */
    #if defined(FEATURE_STEREO_HEADSET) && defined(FEATURE_INTERNAL_SDAC)
#error code not present
    #else
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mm_rec,
                        &voc_adie_codec_mono_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_mm_rec,
                        &voc_adie_codec_mono_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_STEREO_HEADSET && FEATURE_INTERNAL_SDAC */
    /* VOC_CODEC_SPEAKER_PHONE_SURF         */
    #ifdef FEATURE_SPEAKER_PHONE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_speaker_mm_rec,
                        &voc_adie_codec_speaker_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
	   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
							&voc_adie_codec_headset_fm_config,
							&voc_cal_pmic_config_capless_mic) }
    #else
	   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
							&voc_adie_codec_headset_fm_config,
							&voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
       ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                            &voc_adie_codec_speaker_fm_config,
                            &voc_cal_pmic_config_legacy_mic) }
#endif

  }
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_AVS_I2SSBC
  /** Codec/Adie config values for I2SSBC **/
  ,{
    /* VOC_CODEC_ON_CHIP_0             */
    { AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL)}

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    /* VOC_CODEC_ON_CHIP_0_DUAL_MIC    */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL) }
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

    /* VOC_CODEC_ON_CHIP_1             */
    #if defined(FEATURE_STEREO_HEADSET) && defined(FEATURE_INTERNAL_SDAC)
#error code not present
    #else
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #endif /* FEATURE_STEREO_HEADSET && FEATURE_INTERNAL_SDAC */

    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    /* VOC_CODEC_ON_CHIP_AUX           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_aux,
                        &voc_adie_codec_aux_no_mic_config, NULL) }

    /* VOC_CODEC_ON_BOARD              */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_on_board, NULL, NULL)}


    #ifdef FEATURE_SPEAKER_PHONE
    /* VOC_CODEC_SPEAKER_PHONE         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_speaker,
                        &voc_adie_codec_speaker_no_mic_config, NULL) }
    #endif /* FEATURE_SPEAKER_PHONE */


    #ifdef FEATURE_BT
    /* VOC_CODEC_BT_INTERCOM           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL)}

    #ifdef FEATURE_BT_AG
    /* VOC_CODEC_BT_AG                 */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL)}

    #ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
    #endif /* FEATURE_BT_AG_LOCAL_AUDIO */
    #endif /* FEATURE_BT_AG */

    #if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_i2ssbc,
                        &voc_adie_codec_pwr_down_config, NULL)}
    /* VOC_CODEC_BT_A2DP_SCO */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_i2ssbc,
                        &voc_adie_codec_pwr_down_config, NULL)}
    #endif /* FEATURE_SBC_CODEC */

    #endif /* FEATURE_BT */

    /* VOC_CODEC_OFF_BOARD             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL)}


    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_SDAC                  */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    #if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
    /* VOC_CODEC_ON_CHIP_0             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL)}

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_capless_nomic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_ON_CHIP_0             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL)}

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_stereo_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */

    #ifdef FEATURE_TTY
    /* VOC_CODEC_TTY_ON_CHIP_1         */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_TTY_OFF_BOARD         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL)}

    /* VOC_CODEC_TTY_VCO               */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_TTY_HCO               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL)}
    #endif /* FEATURE_TTY */

    #if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    /* VOC_CODEC_USB                   */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_usb,
                        &voc_adie_codec_usb_no_mic_config, NULL)}
    #ifdef FEATURE_INTERNAL_SDAC
    /* VOC_CODEC_STEREO_USB            */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_stereo_usb,
                        &voc_adie_codec_stereo_usb_no_mic_config, NULL)}
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

    /* FM over BT AG */
    #ifdef FEATURE_BT_AG
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_fm_bt_ag,
                         &voc_adie_codec_fm_bt_ag_config,
                         &voc_cal_pmic_config_legacy_mic) }
    #endif

    /* VOC_CODEC_ON_CHIP_0_SURF             */
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int0,
                        &voc_adie_codec_handset_no_mic_config, NULL)}

    /* VOC_CODEC_ON_CHIP_1_SURF             */
    #if defined(FEATURE_STEREO_HEADSET) && defined(FEATURE_INTERNAL_SDAC)
#error code not present
    #else
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_capless_nomic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_int1,
                        &voc_adie_codec_mono_headphone_config,
                        &voc_cal_pmic_config_legacy_nomic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #endif /* FEATURE_STEREO_HEADSET && FEATURE_INTERNAL_SDAC */

    /* VOC_CODEC_SPEAKER_PHONE_SURF         */
    #ifdef FEATURE_SPEAKER_PHONE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_16k_speaker,
                        &voc_adie_codec_speaker_no_mic_config, NULL) }
    #endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM

    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_headset_fm_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
                        &voc_adie_codec_headset_fm_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_speaker_wb,
                        &voc_adie_codec_speaker_fm_config,
                        &voc_cal_pmic_config_legacy_mic) }
#endif
  }
#endif /* FEATURE_AVS_I2SSBC */

  /* CDMA WB vocoder image*/
 ,{
	 /* Retaining config values unchanged for CDMA WB case from CDMA NB case */
    /* VOC_CODEC_ON_CHIP_0             */
    { AUDIO_PATH_CONFIG(&voc_cal_codec_int0_wb,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    /* VOC_CODEC_ON_CHIP_0_DUAL_MIC    */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_dual_mic_wb,
                        &voc_adie_codec_handset_dual_mic_config,
                        &voc_cal_pmic_config_legacy_mic) }
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

    /* VOC_CODEC_ON_CHIP_1             */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_wb,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_wb,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    /* VOC_CODEC_ON_CHIP_AUX           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_aux,
                        &voc_adie_codec_aux_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_ON_BOARD              */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_on_board, NULL, NULL) }

    #ifdef FEATURE_SPEAKER_PHONE
    /* VOC_CODEC_SPEAKER_PHONE         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_speaker_wb,
                        &voc_adie_codec_speaker_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_SPEAKER_PHONE */

    #ifdef FEATURE_BT
    /* VOC_CODEC_BT_INTERCOM           */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_intercom,
                        &voc_adie_codec_handset_no_micamp2_config,
                        &voc_cal_pmic_config_legacy_mic) }

    #ifdef FEATURE_BT_AG
    /* VOC_CODEC_BT_AG                 */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL) }

    #ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
    #endif /* FEATURE_BT_AG_LOCAL_AUDIO */
    #endif /* FEATURE_BT_AG */

    #if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* VOC_CODEC_BT_SBC                */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL) }

    /* VOC_CODEC_BT_DUAL               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_bt_ag, NULL, NULL) }
    #endif /* FEATURE_SBC_CODEC */
    #endif /* FEATURE_BT */

    /* VOC_CODEC_OFF_BOARD             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL) }


    #if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_SDAC                  */
    #ifdef FEATURE_INTERNAL_SDAC
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

    #if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
    /* VOC_CODEC_ON_CHIP_0             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #if defined(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_tx_external_sadc_rx_internal,
                        &voc_adie_codec_handset_no_mic_config,
                        &voc_cal_pmic_config_legacy_nomic) }
    #else
    /* VOC_CODEC_ON_CHIP_0             */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /*(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)*/

    /* VOC_CODEC_STEREO_HEADSET        */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_simul_stereo_headset_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
    #endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */

    #ifdef FEATURE_TTY
    /* VOC_CODEC_TTY_ON_CHIP_1         */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_mono_headset_tty_config,
                        &voc_cal_pmic_config_capless_mic) }
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1,
                        &voc_adie_codec_mono_headset_tty_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    /* VOC_CODEC_TTY_OFF_BOARD         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_off_board, NULL, NULL) }

    /* VOC_CODEC_TTY_VCO               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_tty_vco,
                        &voc_adie_codec_tty_vco_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_TTY_HCO               */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_tty_hco,
                        &voc_adie_codec_tty_hco_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_TTY */

    #if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    /* VOC_CODEC_USB                   */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_usb,
                        &voc_adie_codec_usb_config,
                        &voc_cal_pmic_config_legacy_mic)}

    #ifdef FEATURE_INTERNAL_SDAC
    /* VOC_CODEC_STEREO_USB            */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_usb,
                        &voc_adie_codec_usb_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

    /* FM over BT AG */
    #ifdef FEATURE_BT_AG
    ,{ AUDIO_PATH_CONFIG( &voc_cal_codec_fm_bt_ag,
                          &voc_adie_codec_fm_bt_ag_config,
                          &voc_cal_pmic_config_legacy_mic) }
    #endif

    /* VOC_CODEC_ON_CHIP_0_SURF             */
    ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int0_wb,
                        &voc_adie_codec_handset_config,
                        &voc_cal_pmic_config_legacy_mic) }

    /* VOC_CODEC_ON_CHIP_1_SURF             */
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_wb,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_capless_mic)}
    #else
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_int1_wb,
                        &voc_adie_codec_headset_config,
                        &voc_cal_pmic_config_legacy_mic)}
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

    #ifdef FEATURE_SPEAKER_PHONE
    /* VOC_CODEC_SPEAKER_PHONE_SURF         */
   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_speaker_wb,
                        &voc_adie_codec_speaker_config,
                        &voc_cal_pmic_config_legacy_mic) }
    #endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM
	
	#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
	   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
							&voc_adie_codec_headset_fm_config,
							&voc_cal_pmic_config_capless_mic) }
    #else
	   ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_sdac,
							&voc_adie_codec_headset_fm_config,
							&voc_cal_pmic_config_legacy_mic) }
    #endif  /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
       ,{ AUDIO_PATH_CONFIG(&voc_cal_codec_speaker_wb,
                            &voc_adie_codec_speaker_fm_config,
                            &voc_cal_pmic_config_legacy_mic) }
#endif

  }
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
};


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
** PCM Path configuration data
*/

#if defined(FEATURE_AUDFMT_FIR_FILTER) && defined(FEATURE_QTUNES_COMMON)

/* Default configuration for Qtunes PCM Filter coefficients.
*/
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
#error code not present
#else
CAL_MEMORY uint16 voc_qtunes_pcm_filter_coeff_default[] = {
  0x4000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000
};
#endif /* MSMAUD_6500_FFA_AUDIO_CAL */

/* Default values for Qtunes PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qtunes_pcm_filter_default = {
  0x0000,                         /* PCM filter enable/disable flag  */
  (uint16 *) voc_qtunes_pcm_filter_coeff_default,
                                  /* FIR filter coefficients         */
  0x0080,                         /* Number of coefficients          */
  0x0002                          /* Shift factor                    */
};

/* Configuration for Qtunes PCM Filter coefficients.
** PCM Filter coefficients for different sampling rates
*/

#if defined(MSMAUD_QSC60X0_AUDIO_CAL)     || \
    defined(MSMAUD_6280_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_QSC6270_AUDIO_CAL)     || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 


CAL_MEMORY uint16 voc_qtunes_pcm_filter_coeff_11k[] = {
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0001,0x0002,0x0003,0x0003,0x0002, 0x0000,0xFFFF,0xFFFC,0xFFF8,0xFFF6,
  0xFFF5,0xFFF7,0xFFFB,0x0001,0x000A, 0x0013,0x001A,0x001D,0x001B,0x0013,
  0x0004,0xFFF1,0xFFDC,0xFFC9,0xFFBE, 0xFFBE,0xFFCC,0xFFE8,0x000F,0x003C,
  0x0066,0x0083,0x008C,0x007A,0x004B, 0x0002,0xFFAB,0xFF52,0xFF09,0xFEE4,
  0xFEF0,0xFF38,0xFFBA,0x006B,0x0136, 0x01F9,0x028E,0x02C9,0x027E,0x017E,
  0xFF88,0xFC12,0xF51F,0xD830,0x27D0, 0x0AE1,0x03EE,0x0078,0xFE82,0xFD82,
  0xFD37,0xFD72,0xFE07,0xFECA,0xFF95, 0x0046,0x00C8,0x0110,0x011C,0x00F7,
  0x00AE,0x0055,0xFFFE,0xFFB5,0xFF86, 0xFF74,0xFF7D,0xFF9A,0xFFC4,0xFFF1,
  0x0018,0x0034,0x0042,0x0042,0x0037, 0x0024,0x000F,0xFFFC,0xFFED,0xFFE5,
  0xFFE3,0xFFE6,0xFFED,0xFFF6,0xFFFF, 0x0005,0x0009,0x000B,0x000A,0x0008,
  0x0004,0x0001,0x0000,0xFFFE,0xFFFD, 0xFFFD,0xFFFE,0xFFFF,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000
};

/* Values for Qtunes PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qtunes_pcm_filter_11k = {
  0xFFFF,                         /* PCM filter enable/disable flag  */
  (uint16 *) voc_qtunes_pcm_filter_coeff_11k,
                                  /* FIR filter coefficients         */
  0x0080,                         /* Number of coefficients          */
  0x0002                          /* Shift factor                    */
};

CAL_MEMORY uint16 voc_qtunes_pcm_filter_coeff_12k[] = {
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0xFFFF,0xFFFE,0xFFFD,0xFFFB,0xFFFA,
  0xFFF9,0xFFF9,0xFFF9,0xFFFA,0xFFFD, 0x0000,0x0006,0x000C,0x0013,0x001A,
  0x0020,0x0024,0x0026,0x0023,0x001C, 0x0010,0x0000,0xFFEA,0xFFD2,0xFFB8,
  0xFFA0,0xFF8C,0xFF7F,0xFF7C,0xFF87, 0xFFA0,0xFFC9,0x0000,0x0045,0x0093,
  0x00E5,0x0135,0x0179,0x01A9,0x01BC, 0x01A6,0x015D,0x00D6,0x0000,0xFEC3,
  0xFCE8,0xF9E5,0xF3AF,0xD7B0,0x2850, 0x0C51,0x061B,0x0318,0x013D,0x0000,
  0xFF2A,0xFEA3,0xFE5A,0xFE44,0xFE57, 0xFE87,0xFECB,0xFF1B,0xFF6D,0xFFBB,
  0x0000,0x0037,0x0060,0x0079,0x0084, 0x0081,0x0074,0x0060,0x0048,0x002E,
  0x0016,0x0000,0xFFF0,0xFFE4,0xFFDD, 0xFFDA,0xFFDC,0xFFE0,0xFFE6,0xFFED,
  0xFFF4,0xFFFA,0x0000,0x0003,0x0006, 0x0007,0x0007,0x0007,0x0006,0x0005,
  0x0003,0x0002,0x0001,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000
};

/* Values for Qtunes PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qtunes_pcm_filter_12k = {
  0xFFFF,                         /* PCM filter enable/disable flag  */
  (uint16 *) voc_qtunes_pcm_filter_coeff_12k,
                                  /* FIR filter coefficients         */
  0x0080,                         /* Number of coefficients          */
  0x0002                          /* Shift factor                    */
};

CAL_MEMORY uint16 voc_qtunes_pcm_filter_coeff_16k[] = {
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0001,0x0001,0x0002, 0x0002,0x0003,0x0004,0x0004,0x0005,
  0x0005,0x0005,0x0005,0x0004,0x0003, 0x0000,0xFFFE,0xFFF9,0xFFF4,0xFFEE,
  0xFFE7,0xFFE0,0xFFD9,0xFFD3,0xFFCD, 0xFFCA,0xFFC9,0xFFCB,0xFFD2,0xFFDD,
  0xFFED,0x0001,0x001B,0x003A,0x005D, 0x0083,0x00A9,0x00CF,0x00F1,0x010D,
  0x011F,0x0125,0x011B,0x00FD,0x00C8, 0x0077,0x0003,0xFF67,0xFE92,0xFD6D,
  0xFBC2,0xF903,0xF321,0xD77F,0x2881, 0x0CDF,0x06FD,0x043E,0x0293,0x016E,
  0x0099,0xFFFD,0xFF89,0xFF38,0xFF03, 0xFEE5,0xFEDB,0xFEE1,0xFEF3,0xFF0F,
  0xFF31,0xFF57,0xFF7D,0xFFA3,0xFFC6, 0xFFE5,0xFFFF,0x0013,0x0023,0x002E,
  0x0035,0x0037,0x0036,0x0033,0x002D, 0x0027,0x0020,0x0019,0x0012,0x000C,
  0x0007,0x0002,0x0000,0xFFFD,0xFFFC, 0xFFFB,0xFFFB,0xFFFB,0xFFFB,0xFFFC,
  0xFFFC,0xFFFD,0xFFFE,0xFFFE,0xFFFF, 0xFFFF,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000
};

/* Values for Qtunes PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qtunes_pcm_filter_16k = {
  0xFFFF,                         /* PCM filter enable/disable flag  */
  (uint16 *) voc_qtunes_pcm_filter_coeff_16k,
                                  /* FIR filter coefficients         */
  0x0080,                         /* Number of coefficients          */
  0x0002                          /* Shift factor                    */
};

CAL_MEMORY uint16 voc_qtunes_pcm_filter_coeff_22k[] = {
  0xFFFE,0xFFFD,0xFFFB,0xFFFA,0xFFF9, 0xFFF8,0xFFF7,0xFFF7,0xFFF8,0xFFF9,
  0xFFFB,0xFFFE,0x0001,0x0006,0x000B, 0x0010,0x0016,0x001B,0x0020,0x0023,
  0x0025,0x0026,0x0024,0x0020,0x0019, 0x0010,0x0004,0xFFF8,0xFFE8,0xFFD8,
  0xFFC7,0xFFB7,0xFFA8,0xFF9C,0xFF94, 0xFF90,0xFF91,0xFF99,0xFFA7,0xFFBB,
  0xFFD7,0xFFF8,0x001F,0x004A,0x0078, 0x00A8,0x00D6,0x0100,0x0124,0x0140,
  0x014F,0x0150,0x0140,0x011B,0x00DD, 0x0084,0x000A,0xFF67,0xFE8E,0xFD67,
  0xFBBB,0xF8FD,0xF31D,0xD77E,0x2882, 0x0CE3,0x0703,0x0445,0x0299,0x0172,
  0x0099,0xFFF6,0xFF7C,0xFF23,0xFEE5, 0xFEC0,0xFEB0,0xFEB1,0xFEC0,0xFEDC,
  0xFF00,0xFF2A,0xFF58,0xFF88,0xFFB6, 0xFFE1,0x0008,0x0029,0x0045,0x0059,
  0x0067,0x006F,0x0070,0x006C,0x0064, 0x0058,0x0049,0x0039,0x0028,0x0018,
  0x0008,0xFFFC,0xFFF0,0xFFE7,0xFFE0, 0xFFDC,0xFFDA,0xFFDB,0xFFDD,0xFFE0,
  0xFFE5,0xFFEA,0xFFF0,0xFFF5,0xFFFA, 0xFFFF,0x0002,0x0005,0x0007,0x0008,
  0x0009,0x0009,0x0008,0x0007,0x0006, 0x0005,0x0003,0x0002
};

/* Values for Qtunes PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qtunes_pcm_filter_22k = {
  0xFFFF,                         /* PCM filter enable/disable flag  */
  (uint16 *) voc_qtunes_pcm_filter_coeff_22k,
                                  /* FIR filter coefficients         */
  0x0080,                         /* Number of coefficients          */
  0x0002                          /* Shift factor                    */
};

CAL_MEMORY uint16 voc_qtunes_pcm_filter_coeff_24k[] = {
  0x0000,0x0000,0x0000,0x0000,0xFFFF, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
  0xFFFF,0x0000,0x0000,0x0000,0x0001, 0x0003,0x0005,0x0007,0x0009,0x000C,
  0x000F,0x0013,0x0018,0x001C,0x0021, 0x0027,0x002D,0x0033,0x0039,0x003F,
  0x0046,0x004C,0x0052,0x0057,0x005D, 0x0061,0x0064,0x0067,0x0068,0x0067,
  0x0065,0x0061,0x005A,0x0051,0x0045, 0x0036,0x0023,0x000D,0xFFF3,0xFFD3,
  0xFFAE,0xFF83,0xFF50,0xFF14,0xFECE, 0xFE79,0xFE11,0xFD8F,0xFCE4,0xFBF8,
  0xFA92,0xF822,0xF297,0xD751,0x28AF, 0x0D69,0x07DE,0x056E,0x0408,0x031C,
  0x0271,0x01EF,0x0187,0x0132,0x00EC, 0x00B0,0x007D,0x0052,0x002D,0x000D,
  0xFFF3,0xFFDD,0xFFCA,0xFFBB,0xFFAF, 0xFFA6,0xFF9F,0xFF9B,0xFF99,0xFF98,
  0xFF99,0xFF9C,0xFF9F,0xFFA3,0xFFA9, 0xFFAE,0xFFB4,0xFFBA,0xFFC1,0xFFC7,
  0xFFCD,0xFFD3,0xFFD9,0xFFDF,0xFFE4, 0xFFE8,0xFFED,0xFFF1,0xFFF4,0xFFF7,
  0xFFF9,0xFFFB,0xFFFD,0xFFFF,0x0000, 0x0000,0x0000,0x0001,0x0001,0x0001,
  0x0001,0x0001,0x0001,0x0001,0x0000, 0x0000,0x0000,0x0000
};

/* Values for Qtunes PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qtunes_pcm_filter_24k = {
  0xFFFF,                         /* PCM filter enable/disable flag  */
  (uint16 *) voc_qtunes_pcm_filter_coeff_24k,
                                  /* FIR filter coefficients         */
  0x0080,                         /* Number of coefficients          */
  0x0002                          /* Shift factor                    */
};

CAL_MEMORY uint16 voc_qtunes_pcm_filter_coeff_32k[] = {
  0x0000,0x0000,0x0000,0x0000,0xFFFF, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
  0xFFFF,0x0000,0x0000,0x0000,0x0001, 0x0003,0x0005,0x0007,0x0009,0x000C,
  0x000F,0x0013,0x0018,0x001C,0x0021, 0x0027,0x002D,0x0033,0x0039,0x003F,
  0x0046,0x004C,0x0052,0x0057,0x005D, 0x0061,0x0064,0x0067,0x0068,0x0067,
  0x0065,0x0061,0x005A,0x0051,0x0045, 0x0036,0x0023,0x000D,0xFFF3,0xFFD3,
  0xFFAE,0xFF83,0xFF50,0xFF14,0xFECE, 0xFE79,0xFE11,0xFD8F,0xFCE4,0xFBF8,
  0xFA92,0xF822,0xF297,0xD751,0x28AF, 0x0D69,0x07DE,0x056E,0x0408,0x031C,
  0x0271,0x01EF,0x0187,0x0132,0x00EC, 0x00B0,0x007D,0x0052,0x002D,0x000D,
  0xFFF3,0xFFDD,0xFFCA,0xFFBB,0xFFAF, 0xFFA6,0xFF9F,0xFF9B,0xFF99,0xFF98,
  0xFF99,0xFF9C,0xFF9F,0xFFA3,0xFFA9, 0xFFAE,0xFFB4,0xFFBA,0xFFC1,0xFFC7,
  0xFFCD,0xFFD3,0xFFD9,0xFFDF,0xFFE4, 0xFFE8,0xFFED,0xFFF1,0xFFF4,0xFFF7,
  0xFFF9,0xFFFB,0xFFFD,0xFFFF,0x0000, 0x0000,0x0000,0x0001,0x0001,0x0001,
  0x0001,0x0001,0x0001,0x0001,0x0000, 0x0000,0x0000,0x0000
};

/* Values for Qtunes PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qtunes_pcm_filter_32k = {
  0xFFFF,                         /* PCM filter enable/disable flag  */
  (uint16 *) voc_qtunes_pcm_filter_coeff_32k,
                                  /* FIR filter coefficients         */
  0x0080,                         /* Number of coefficients          */
  0x0002                          /* Shift factor                    */
};

CAL_MEMORY uint16 voc_qtunes_pcm_filter_coeff_44k[] = {
  0x0004,0x0005,0x0006,0x0008,0x000A, 0x000C,0x000E,0x0010,0x0013,0x0016,
  0x0019,0x001C,0x001F,0x0022,0x0026, 0x0029,0x002D,0x0030,0x0034,0x0037,
  0x003B,0x003E,0x0041,0x0044,0x0046, 0x0048,0x004A,0x004B,0x004C,0x004C,
  0x004C,0x004B,0x0049,0x0046,0x0043, 0x003E,0x0038,0x0031,0x0029,0x001F,
  0x0014,0x0008,0xFFFB,0xFFEB,0xFFD9, 0xFFC4,0xFFAE,0xFF94,0xFF78,0xFF58,
  0xFF33,0xFF0A,0xFEDB,0xFEA4,0xFE63, 0xFE16,0xFDB7,0xFD3E,0xFC9E,0xFBBD,
  0xFA63,0xF800,0xF283,0xD74A,0x28B6, 0x0D7D,0x0800,0x059D,0x0443,0x0362,
  0x02C2,0x0249,0x01EA,0x019D,0x015C, 0x0125,0x00F6,0x00CD,0x00A8,0x0088,
  0x006C,0x0052,0x003C,0x0027,0x0015, 0x0005,0xFFF8,0xFFEC,0xFFE1,0xFFD7,
  0xFFCF,0xFFC8,0xFFC2,0xFFBD,0xFFBA, 0xFFB7,0xFFB5,0xFFB4,0xFFB4,0xFFB4,
  0xFFB5,0xFFB6,0xFFB8,0xFFBA,0xFFBC, 0xFFBF,0xFFC2,0xFFC5,0xFFC9,0xFFCC,
  0xFFD0,0xFFD3,0xFFD7,0xFFDA,0xFFDE, 0xFFE1,0xFFE4,0xFFE7,0xFFEA,0xFFED,
  0xFFF0,0xFFF2,0xFFF4,0xFFF6,0xFFF8, 0xFFFA,0xFFFB,0xFFFC
};

/* Values for Qtunes PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qtunes_pcm_filter_44k = {
  0xFFFF,                         /* PCM filter enable/disable flag  */
  (uint16 *) voc_qtunes_pcm_filter_coeff_44k,
                                  /* FIR filter coefficients         */
  0x0080,                         /* Number of coefficients          */
  0x0002                          /* Shift factor                    */
};

CAL_MEMORY uint16 voc_qtunes_pcm_filter_coeff_48k[] = {
  0x000B,0x000D,0x000F,0x0012,0x0014, 0x0017,0x001A,0x001D,0x001F,0x0022,
  0x0025,0x0028,0x002B,0x002E,0x0031, 0x0034,0x0036,0x0039,0x003B,0x003D,
  0x003F,0x0040,0x0042,0x0042,0x0043, 0x0043,0x0043,0x0042,0x0040,0x003E,
  0x003B,0x0038,0x0033,0x002E,0x0029, 0x0022,0x001A,0x0011,0x0008,0xFFFE,
  0xFFF1,0xFFE4,0xFFD5,0xFFC4,0xFFB2, 0xFF9D,0xFF87,0xFF6E,0xFF52,0xFF33,
  0xFF10,0xFEE8,0xFEBA,0xFE86,0xFE47, 0xFDFC,0xFDA0,0xFD2A,0xFC8D,0xFBAF,
  0xFA58,0xF7F8,0xF27E,0xD748,0x28B8, 0x0D82,0x0808,0x05A8,0x0451,0x0373,
  0x02D6,0x0260,0x0204,0x01B9,0x017A, 0x0146,0x0118,0x00F0,0x00CD,0x00AE,
  0x0092,0x0079,0x0063,0x004E,0x003C, 0x002B,0x001C,0x000F,0x0002,0xFFF8,
  0xFFEF,0xFFE6,0xFFDE,0xFFD7,0xFFD2, 0xFFCD,0xFFC8,0xFFC5,0xFFC2,0xFFC0,
  0xFFBE,0xFFBD,0xFFBD,0xFFBD,0xFFBE, 0xFFBE,0xFFC0,0xFFC1,0xFFC3,0xFFC5,
  0xFFC7,0xFFCA,0xFFCC,0xFFCF,0xFFD2, 0xFFD5,0xFFD8,0xFFDB,0xFFDE,0xFFE1,
  0xFFE3,0xFFE6,0xFFE9,0xFFEC,0xFFEE, 0xFFF1,0xFFF3,0xFFF5
};

/* Values for Qtunes PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qtunes_pcm_filter_48k = {
  0xFFFF,                         /* PCM filter enable/disable flag  */
  (uint16 *) voc_qtunes_pcm_filter_coeff_48k,
                                  /* FIR filter coefficients         */
  0x0080,                         /* Number of coefficients          */
  0x0002                          /* Shift factor                    */
};

/* Qtunes PCM filter controls for each sampling rate
*/
const qdsp_cmd_audfmt_pcm_filter_type*
              voc_cal_qtunes_pcm_filter[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qtunes_pcm_filter_11k, /* Dummy for ZERO k - DO NOT USE      */
  &voc_cal_qtunes_pcm_filter_11k, /* 8k   */
  &voc_cal_qtunes_pcm_filter_11k, /* 11k  */
  &voc_cal_qtunes_pcm_filter_12k, /* 12k  */
  &voc_cal_qtunes_pcm_filter_16k, /* 16k  */
  &voc_cal_qtunes_pcm_filter_22k, /* 22k  */
  &voc_cal_qtunes_pcm_filter_24k, /* 24k  */
  &voc_cal_qtunes_pcm_filter_32k, /* 32k  */
  &voc_cal_qtunes_pcm_filter_44k, /* 44p1k  */
  &voc_cal_qtunes_pcm_filter_48k  /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qtunes_pcm_filter_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

#else

/* Qtunes PCM filter controls for each sampling rate
*/
const qdsp_cmd_audfmt_pcm_filter_type*
              voc_cal_qtunes_pcm_filter[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qtunes_pcm_filter_default, /* Dummy for ZERO k - DO NOT USE      */
  &voc_cal_qtunes_pcm_filter_default, /* 8k   */
  &voc_cal_qtunes_pcm_filter_default, /* 11k  */
  &voc_cal_qtunes_pcm_filter_default, /* 12k  */
  &voc_cal_qtunes_pcm_filter_default, /* 16k  */
  &voc_cal_qtunes_pcm_filter_default, /* 22k  */
  &voc_cal_qtunes_pcm_filter_default, /* 24k  */
  &voc_cal_qtunes_pcm_filter_default, /* 32k  */
  &voc_cal_qtunes_pcm_filter_default, /* 44p1k  */
  &voc_cal_qtunes_pcm_filter_default  /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qtunes_pcm_filter_default  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

#endif /* MSMAUD_QSC60X0_AUDIO_CAL || MSMAUD_6280_FFA_AUDIO_CAL */
#endif /* FEATURE_AUDFMT_FIR_FILTER && FEATURE_QTUNES_COMMON */

#ifdef FEATURE_QTUNES_ADRC
/* Default values for Qtunes Audio Dynamic Range Compressor controls
*/
CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_default = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_8k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};


CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_11p025k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_12k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_16k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_22p05k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_24k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_32k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_44p1k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_48k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FC3, 0xFAF9 },             /* Attack constant                 */
  { 0x7FFB, 0xFEC9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_adrc_ctl_44k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

/* Qtunes ADRC controls for each sampling rate
*/
const qdsp_cmd_audfmt_adrc_ctl_type*
              voc_cal_qtunes_adrc_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qtunes_adrc_ctl_default, /* Dummy for ZERO k - DO NOT USE     */
  &voc_cal_qtunes_adrc_ctl_8k,       /* 8k   */
  &voc_cal_qtunes_adrc_ctl_11p025k,  /* 11k  */
  &voc_cal_qtunes_adrc_ctl_12k,      /* 12k  */
  &voc_cal_qtunes_adrc_ctl_16k,      /* 16k  */
  &voc_cal_qtunes_adrc_ctl_22p05k,   /* 22k  */
  &voc_cal_qtunes_adrc_ctl_24k,      /* 24k  */
  &voc_cal_qtunes_adrc_ctl_32k,      /* 32k  */
  &voc_cal_qtunes_adrc_ctl_44p1k,    /* 44p1k */
  &voc_cal_qtunes_adrc_ctl_48k       /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qtunes_adrc_ctl_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

/* Default values for Qtunes Audio Dynamic Range Compressor controls
 for stereo headset */
CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_headset_adrc_ctl_default = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_headset_adrc_ctl_8k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};


CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_stereo_headset_adrc_ctl_11p025k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_stereo_headset_adrc_ctl_12k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_headset_adrc_ctl_16k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_headset_adrc_ctl_22p05k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_headset_adrc_ctl_24k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_headset_adrc_ctl_32k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_headset_adrc_ctl_44p1k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_headset_adrc_ctl_48k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FC3, 0xFAF9 },             /* Attack constant                 */
  { 0x7FFB, 0xFEC9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_headset_adrc_ctl_44k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

/* Qtunes ADRC controls for each sampling rate
*/
const qdsp_cmd_audfmt_adrc_ctl_type*
      voc_cal_qtunes_stereo_headset_adrc_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qtunes_stereo_headset_adrc_ctl_default, /* Dummy for ZERO k - DO NOT USE     */
  &voc_cal_qtunes_stereo_headset_adrc_ctl_8k,       /* 8k   */
  &voc_cal_qtunes_stereo_headset_adrc_ctl_11p025k,  /* 11k  */
  &voc_cal_qtunes_stereo_headset_adrc_ctl_12k,      /* 12k  */
  &voc_cal_qtunes_stereo_headset_adrc_ctl_16k,      /* 16k  */
  &voc_cal_qtunes_stereo_headset_adrc_ctl_22p05k,   /* 22k  */
  &voc_cal_qtunes_stereo_headset_adrc_ctl_24k,      /* 24k  */
  &voc_cal_qtunes_stereo_headset_adrc_ctl_32k,      /* 32k  */
  &voc_cal_qtunes_stereo_headset_adrc_ctl_44p1k,    /* 44p1k */
  &voc_cal_qtunes_stereo_headset_adrc_ctl_48k       /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qtunes_stereo_headset_adrc_ctl_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};


/* Default values for Qtunes Audio Dynamic Range Compressor controls
 for  speakerphone */
#if defined(MSMAUD_QSC60X0_AUDIO_CAL) || defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
    defined(MSMAUD_QSC1110_AUDIO_CAL) || defined (MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_default = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x7C5F },             /* Attack constant                 */
  { 0x7FBE, 0xAD84 },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_8k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7D36, 0xF9DC },             /* Attack constant                 */
  { 0x7E99, 0x86F7 },             /* Release constant                */
  0x0008                          /* System delay                    */
};


CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_11p025k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7DF9, 0x0DD5 },             /* Attack constant                 */
  { 0x7EBF, 0x7DD1 },             /* Release constant                */
  0x000B                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_12k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7E22, 0xE8ED },             /* Attack constant                 */
  { 0x7F10, 0x948D },             /* Release constant                */
  0x000C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_16k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7E99, 0x86F7 },             /* Attack constant                 */
  { 0x7F4C, 0x454D },             /* Release constant                */
  0x0010                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_22p05k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7EFB, 0x7DD1 },             /* Attack constant                 */
  { 0x7F7D, 0x7C5F },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_24k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F10, 0x948D },             /* Attack constant                 */
  { 0x7F88, 0x1217 },             /* Release constant                */
  0x0018                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_32k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F4C, 0x454D },             /* Attack constant                 */
  { 0x7FA6, 0x0304 },             /* Release constant                */
  0x0020                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_44p1k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x7C5F },             /* Attack constant                 */
  { 0x7FBE, 0xAD84 },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_48k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F88, 0x1217 },             /* Attack constant                 */
  { 0x7FC3, 0xFAF9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qtunes_spk_phone_adrc_ctl_44k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x7C5F },             /* Attack constant                 */
  { 0x7FBE, 0xAD84 },             /* Release constant                */
  0x002C                          /* System delay                    */
};

#else

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_default = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_8k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};


CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_11p025k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_12k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_16k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_22p05k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_24k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_32k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_44p1k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_48k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FC3, 0xFAF9 },             /* Attack constant                 */
  { 0x7FFB, 0xFEC9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_spk_phone_adrc_ctl_44k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};
#endif
/* Qtunes ADRC controls for each sampling rate
*/
const qdsp_cmd_audfmt_adrc_ctl_type*
      voc_cal_qtunes_spk_phone_adrc_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qtunes_spk_phone_adrc_ctl_default, /* Dummy for ZERO k - DO NOT USE     */
  &voc_cal_qtunes_spk_phone_adrc_ctl_8k,       /* 8k   */
  &voc_cal_qtunes_spk_phone_adrc_ctl_11p025k,  /* 11k  */
  &voc_cal_qtunes_spk_phone_adrc_ctl_12k,      /* 12k  */
  &voc_cal_qtunes_spk_phone_adrc_ctl_16k,      /* 16k  */
  &voc_cal_qtunes_spk_phone_adrc_ctl_22p05k,   /* 22k  */
  &voc_cal_qtunes_spk_phone_adrc_ctl_24k,      /* 24k  */
  &voc_cal_qtunes_spk_phone_adrc_ctl_32k,      /* 32k  */
  &voc_cal_qtunes_spk_phone_adrc_ctl_44p1k,    /* 44p1k */
  &voc_cal_qtunes_spk_phone_adrc_ctl_48k       /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qtunes_spk_phone_adrc_ctl_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};


/* Default values for Qtunes Audio Dynamic Range Compressor controls
 for Stereo USB   */
CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_usb_adrc_ctl_default = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_usb_adrc_ctl_8k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};


CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_stereo_usb_adrc_ctl_11p025k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qtunes_stereo_usb_adrc_ctl_12k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_usb_adrc_ctl_16k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_usb_adrc_ctl_22p05k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_usb_adrc_ctl_24k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_usb_adrc_ctl_32k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_usb_adrc_ctl_44p1k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_usb_adrc_ctl_48k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FC3, 0xFAF9 },             /* Attack constant                 */
  { 0x7FFB, 0xFEC9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qtunes_stereo_usb_adrc_ctl_44k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FFE, 0x5C95 },             /* Attack constant                 */
  { 0x7FF2, 0xE53A },             /* Release constant                */
  0x002C                          /* System delay                    */
};

/* Qtunes ADRC controls for each sampling rate
*/
const qdsp_cmd_audfmt_adrc_ctl_type*
      voc_cal_qtunes_stereo_usb_adrc_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qtunes_stereo_usb_adrc_ctl_default, /* Dummy for ZERO k - DO NOT USE     */
  &voc_cal_qtunes_stereo_usb_adrc_ctl_8k,       /* 8k   */
  &voc_cal_qtunes_stereo_usb_adrc_ctl_11p025k,  /* 11k  */
  &voc_cal_qtunes_stereo_usb_adrc_ctl_12k,      /* 12k  */
  &voc_cal_qtunes_stereo_usb_adrc_ctl_16k,      /* 16k  */
  &voc_cal_qtunes_stereo_usb_adrc_ctl_22p05k,   /* 22k  */
  &voc_cal_qtunes_stereo_usb_adrc_ctl_24k,      /* 24k  */
  &voc_cal_qtunes_stereo_usb_adrc_ctl_32k,      /* 32k  */
  &voc_cal_qtunes_stereo_usb_adrc_ctl_44p1k,    /* 44p1k */
  &voc_cal_qtunes_stereo_usb_adrc_ctl_48k       /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qtunes_stereo_usb_adrc_ctl_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

#endif /* FEATURE_QTUNES_ADRC */

#ifdef FEATURE_AUDFMT_STF
/* Number of Side chain filter will be used.
*/
#define VOC_CAL_AUDFMT_STF_NUM 4

#if (VOC_CAL_AUDFMT_STF_NUM > QDSP_CMD_AUDFMT_STF_MAX)
#error "Number of side chain filter is more than DSP can support"
#endif

/* MACRO for assigning coefficient values, DSP take in LSW, MSB format
*/
#undef COEFF
#define COEFF(msb,lsb) (lsb),(msb)

/* Side chain filter pan mode
** -1: Left panning; Filter is applied only to the left channel.
**  0: No panning; Filter is applied for both left and right channel.
**  1: Right panning; Filter is applied to only right channel.
**  Note:
**    In case the PCM output is configured as mono;
**    It will be treated as left channel input and filters with
**    right-pan (stfPan = 1) will be ignored.
*/
#define VOC_CAL_AUDFMT_STF_PAN_LEFT   0xFFFF
#define VOC_CAL_AUDFMT_STF_PAN_BOTH   0x0000
#define VOC_CAL_AUDFMT_STF_PAN_RIGHT  0x0001

/* Example of Side chain filter control parameter,
** different frequence can have different gain and coefficients values.
*/
CAL_MEMORY uint16 voc_cal_sidechain_filter_coeff
            [VOC_CAL_AUDFMT_STF_NUM * QDSP_CMD_AUDFMT_STF_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_STF_PAN_LEFT,
  VOC_CAL_AUDFMT_STF_PAN_RIGHT,
  VOC_CAL_AUDFMT_STF_PAN_BOTH,
  VOC_CAL_AUDFMT_STF_PAN_BOTH
};

CAL_MEMORY qdsp_cmd_audfmt_sidechain_filter_type
                                       voc_cal_sidechain_filter_default = {
  FALSE,                                                /* Enable/disable   */
  0x1000,                                               /* Main Chain Gain  */
  0x1000,                                               /* Side Chain Gain  */
  VOC_CAL_AUDFMT_STF_NUM,                               /* Number of bands  */
  voc_cal_sidechain_filter_coeff                        /* Coefficients     */
};

const qdsp_cmd_audfmt_sidechain_filter_type*
        voc_cal_sidechain_filter_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_sidechain_filter_default, /* Dummy for ZERO k - DO NOT USE     */
  &voc_cal_sidechain_filter_default,  /* 8k   */
  &voc_cal_sidechain_filter_default,  /* 11k  */
  &voc_cal_sidechain_filter_default,  /* 12k  */
  &voc_cal_sidechain_filter_default,  /* 16k  */
  &voc_cal_sidechain_filter_default,  /* 22k  */
  &voc_cal_sidechain_filter_default,  /* 24k  */
  &voc_cal_sidechain_filter_default,  /* 32k  */
  &voc_cal_sidechain_filter_default, /* 44p1k */
  &voc_cal_sidechain_filter_default  /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_sidechain_filter_default   /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */

};
/* Undefine Coefficient MACRO
*/
#undef COEFF
#endif /* FEATURE_AUDFMT_STF */

#ifdef FEATURE_AUDFMT_IIR_FILTER
/* Audio format IIR turning filter
**
** The maximum filter is 4
*/
#define VOC_CAL_AUDFMT_IIR_FILTER_NUM 4

#if (VOC_CAL_AUDFMT_IIR_FILTER_NUM > QDSP_CMD_AUDFMT_IIR_FILTER_MAX)
#error " The number of IIR filter is greater than the DSP maximum number"
#endif

/* MACRO for assigning coefficient values, DSP take in LSW, MSB format
*/
#undef COEFF
#define COEFF(msb,lsb) (lsb),(msb)


/* IIR filter pan mode
** -1: Left panning; Filter is applied only to the left channel.
**  0: No panning; Filter is applied for both left and right channel.
**  1: Right panning; Filter is applied to only right channel.
**  Note:
**    In case the PCM output is configured as mono;
**    It will be treated as left channel input and filters with
**    right-pan (stfPan = 1) will be ignored.
*/
#define VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT   0xFFFF
#define VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH   0x0000
#define VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT  0x0001

/* Example of IIR filter control parameter,
** different frequence can have different gain and coefficients values.
*/

#if defined (MSMAUD_QSC60X0_AUDIO_CAL) || defined (MSMAUD_QSC60X5_AUDIO_CAL) || \
    defined (MSMAUD_QSC6270_AUDIO_CAL) || defined(MSMAUD_QSC1110_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_8k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0xA9AA, 0x8331), COEFF(0x20E5, 0x1451),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_11k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x9E97, 0x326D), COEFF(0x276C, 0x9082),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_12k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x9C18, 0x47A4), COEFF(0x290B, 0xE752),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_16k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x952B, 0xC695), COEFF(0x2DDD, 0x5452),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_22k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8F72, 0x3771), COEFF(0x323A, 0x114C),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_24k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8E2A, 0xE59F), COEFF(0x3340, 0x59BE),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_32k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8AA4, 0x1FC6), COEFF(0x362D, 0x9473),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_44k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x87BA, 0x88D0), COEFF(0x38B6, 0x4F74),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_48k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x871A, 0x1500), COEFF(0x3945, 0x9F0B),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_8k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_8k                   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_11k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_11k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_12k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_12k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_16k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_16k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_22k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_22k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_24k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_24k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_32k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_32k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_44k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_44k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_48k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_48k                  /* Coefficients     */
};

const qdsp_cmd_audfmt_iir_filter_type*
        voc_cal_audfmt_iir_filter_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_audfmt_iir_filter_8k,  /* DO NOT USE     */
  &voc_cal_audfmt_iir_filter_8k,  /* 8k   */
  &voc_cal_audfmt_iir_filter_11k,  /* 11k  */
  &voc_cal_audfmt_iir_filter_12k,  /* 12k  */
  &voc_cal_audfmt_iir_filter_16k,  /* 16k  */
  &voc_cal_audfmt_iir_filter_22k,  /* 22k  */
  &voc_cal_audfmt_iir_filter_24k,  /* 24k  */
  &voc_cal_audfmt_iir_filter_32k,  /* 32k  */
  &voc_cal_audfmt_iir_filter_44k, /* 44p1k */
  &voc_cal_audfmt_iir_filter_48k   /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_audfmt_iir_filter_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

/* IIR Filter for Stereo Headset  */
CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_8k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0xA9AA, 0x8331), COEFF(0x20E5, 0x1451),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_11k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x9E97, 0x326D), COEFF(0x276C, 0x9082),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_12k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x9C18, 0x47A4), COEFF(0x290B, 0xE752),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_16k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x952B, 0xC695), COEFF(0x2DDD, 0x5452),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_22k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8F72, 0x3771), COEFF(0x323A, 0x114C),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_24k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8E2A, 0xE59F), COEFF(0x3340, 0x59BE),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_32k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8AA4, 0x1FC6), COEFF(0x362D, 0x9473),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_44k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x87BA, 0x88D0), COEFF(0x38B6, 0x4F74),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_48k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x871A, 0x1500), COEFF(0x3945, 0x9F0B),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};
#if defined(MSMAUD_QSC60X0_AUDIO_CAL) || defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
    defined(MSMAUD_QSC1110_AUDIO_CAL) || defined (MSMAUD_QSC6270_AUDIO_CAL) || \
    defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_8k = {
  FALSE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_8k    /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_11k = {
  FALSE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_11k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_12k = {
  FALSE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_12k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_16k = {
  FALSE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_16k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_22k = {
  FALSE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_22k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_24k = {
  FALSE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_24k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_32k = {
  FALSE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_32k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_44k = {
  FALSE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_44k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_48k = {
  FALSE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_48k   /* Coefficients     */
};

#else

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_8k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_8k    /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_11k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_11k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_12k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_12k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_16k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_16k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_22k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_22k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_24k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_24k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_32k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_32k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_44k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_44k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_48k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_48k   /* Coefficients     */
};
#endif

const qdsp_cmd_audfmt_iir_filter_type*
        voc_cal_audfmt_stereo_headset_iir_filter_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_audfmt_stereo_headset_iir_filter_8k,  /* DO NOT USE     */
  &voc_cal_audfmt_stereo_headset_iir_filter_8k,  /* 8k   */
  &voc_cal_audfmt_stereo_headset_iir_filter_11k,  /* 11k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_12k,  /* 12k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_16k,  /* 16k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_22k,  /* 22k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_24k,  /* 24k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_32k,  /* 32k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_44k, /* 44p1k */
  &voc_cal_audfmt_stereo_headset_iir_filter_48k   /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_audfmt_stereo_headset_iir_filter_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

/* IIR filter for Speaker phone  */
CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_8k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0xA9AA, 0x8331), COEFF(0x20E5, 0x1451),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_11k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x9E97, 0x326D), COEFF(0x276C, 0x9082),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_12k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x9C18, 0x47A4), COEFF(0x290B, 0xE752),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_16k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x952B, 0xC695), COEFF(0x2DDD, 0x5452),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_22k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8F72, 0x3771), COEFF(0x323A, 0x114C),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_24k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8E2A, 0xE59F), COEFF(0x3340, 0x59BE),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_32k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8AA4, 0x1FC6), COEFF(0x362D, 0x9473),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_44k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x87BA, 0x88D0), COEFF(0x38B6, 0x4F74),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_48k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x871A, 0x1500), COEFF(0x3945, 0x9F0B),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_8k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_8k         /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_11k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_11k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_12k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_12k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_16k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_16k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_22k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_22k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_24k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_24k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_32k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_32k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_44k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_44k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_48k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_48k        /* Coefficients     */
};

const qdsp_cmd_audfmt_iir_filter_type*
        voc_cal_audfmt_spk_phone_iir_filter_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_audfmt_spk_phone_iir_filter_8k,  /* DO NOT USE     */
  &voc_cal_audfmt_spk_phone_iir_filter_8k,  /* 8k   */
  &voc_cal_audfmt_spk_phone_iir_filter_11k,  /* 11k  */
  &voc_cal_audfmt_spk_phone_iir_filter_12k,  /* 12k  */
  &voc_cal_audfmt_spk_phone_iir_filter_16k,  /* 16k  */
  &voc_cal_audfmt_spk_phone_iir_filter_22k,  /* 22k  */
  &voc_cal_audfmt_spk_phone_iir_filter_24k,  /* 24k  */
  &voc_cal_audfmt_spk_phone_iir_filter_32k,  /* 32k  */
  &voc_cal_audfmt_spk_phone_iir_filter_44k, /* 44p1k */
  &voc_cal_audfmt_spk_phone_iir_filter_48k   /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_audfmt_spk_phone_iir_filter_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

/* IIR filter for Stereo USB */
CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_8k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0xA9AA, 0x8331), COEFF(0x20E5, 0x1451),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_11k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x9E97, 0x326D), COEFF(0x276C, 0x9082),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_12k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x9C18, 0x47A4), COEFF(0x290B, 0xE752),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_16k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x952B, 0xC695), COEFF(0x2DDD, 0x5452),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_22k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8F72, 0x3771), COEFF(0x323A, 0x114C),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_24k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8E2A, 0xE59F), COEFF(0x3340, 0x59BE),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_32k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x8AA4, 0x1FC6), COEFF(0x362D, 0x9473),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_44k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x87BA, 0x88D0), COEFF(0x38B6, 0x4F74),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_48k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4000, 0x0000), COEFF(0x8000, 0x0000), COEFF(0x4000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x4000, 0x0000), COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  /* Band MCoeff */
  COEFF(0x871A, 0x1500), COEFF(0x3945, 0x9F0B),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),
  COEFF(0x0000, 0x0000), COEFF(0x0000, 0x0000),

  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_8k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_8k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_11k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_11k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_12k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_12k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_16k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_16k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_22k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_22k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_24k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_24k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_32k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_32k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_44k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_44k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_48k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_48k       /* Coefficients     */
};

const qdsp_cmd_audfmt_iir_filter_type*
        voc_cal_audfmt_stereo_usb_iir_filter_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_audfmt_stereo_usb_iir_filter_8k,  /* DO NOT USE     */
  &voc_cal_audfmt_stereo_usb_iir_filter_8k,  /* 8k   */
  &voc_cal_audfmt_stereo_usb_iir_filter_11k,  /* 11k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_12k,  /* 12k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_16k,  /* 16k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_22k,  /* 22k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_24k,  /* 24k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_32k,  /* 32k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_44k, /* 44p1k */
  &voc_cal_audfmt_stereo_usb_iir_filter_48k   /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_audfmt_stereo_usb_iir_filter_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};


#else

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_8k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_11k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_12k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_16k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_22k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_24k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_32k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_44k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff_48k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_iir_filter_coeff[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_8k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_8k                   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_11k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_11k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_12k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_12k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_16k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_16k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_22k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_22k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_24k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_24k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_32k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_32k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_44k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_44k                  /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_48k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff_48k                  /* Coefficients     */
};


CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_iir_filter_default = {
  FALSE,                                                 /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                         /* Number of bands  */
  voc_cal_audfmt_iir_filter_coeff                        /* Coefficients     */
};


const qdsp_cmd_audfmt_iir_filter_type*
        voc_cal_audfmt_iir_filter_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_audfmt_iir_filter_8k,  /* DO NOT USE     */
  &voc_cal_audfmt_iir_filter_8k,  /* 8k   */
  &voc_cal_audfmt_iir_filter_11k,  /* 11k  */
  &voc_cal_audfmt_iir_filter_12k,  /* 12k  */
  &voc_cal_audfmt_iir_filter_16k,  /* 16k  */
  &voc_cal_audfmt_iir_filter_22k,  /* 22k  */
  &voc_cal_audfmt_iir_filter_24k,  /* 24k  */
  &voc_cal_audfmt_iir_filter_32k,  /* 32k  */
  &voc_cal_audfmt_iir_filter_44k, /* 44p1k */
  &voc_cal_audfmt_iir_filter_48k   /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_audfmt_iir_filter_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

/* IIR Filter table for stereo Head set  */
CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_8k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_11k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_12k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_16k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_22k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_24k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_32k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_44k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff_48k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_headset_iir_filter_coeff[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_8k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_8k    /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_11k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_11k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_12k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_12k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_16k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_16k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_22k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_22k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_24k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_24k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_32k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_32k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_44k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_44k   /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_48k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff_48k   /* Coefficients     */
};


CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_headset_iir_filter_default = {
  FALSE,                                                 /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                         /* Number of bands  */
  voc_cal_audfmt_stereo_headset_iir_filter_coeff         /* Coefficients     */
};


const qdsp_cmd_audfmt_iir_filter_type*
        voc_cal_audfmt_stereo_headset_iir_filter_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_audfmt_stereo_headset_iir_filter_8k,  /* DO NOT USE     */
  &voc_cal_audfmt_stereo_headset_iir_filter_8k,  /* 8k   */
  &voc_cal_audfmt_stereo_headset_iir_filter_11k,  /* 11k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_12k,  /* 12k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_16k,  /* 16k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_22k,  /* 22k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_24k,  /* 24k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_32k,  /* 32k  */
  &voc_cal_audfmt_stereo_headset_iir_filter_44k, /* 44p1k */
  &voc_cal_audfmt_stereo_headset_iir_filter_48k   /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_audfmt_stereo_headset_iir_filter_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};
/* IIR Filter table for speaker Phone */

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_8k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_11k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_12k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_16k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_22k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_24k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_32k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_44k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff_48k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_spk_phone_iir_filter_coeff[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_8k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_8k         /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_11k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_11k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_12k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_12k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_16k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_16k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_22k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_22k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_24k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_24k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_32k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_32k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_44k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_44k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_48k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff_48k        /* Coefficients     */
};


CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_spk_phone_iir_filter_default = {
  FALSE,                                                 /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                         /* Number of bands  */
  voc_cal_audfmt_spk_phone_iir_filter_coeff              /* Coefficients     */
};


const qdsp_cmd_audfmt_iir_filter_type*
        voc_cal_audfmt_spk_phone_iir_filter_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_audfmt_spk_phone_iir_filter_8k,  /* DO NOT USE     */
  &voc_cal_audfmt_spk_phone_iir_filter_8k,  /* 8k   */
  &voc_cal_audfmt_spk_phone_iir_filter_11k,  /* 11k  */
  &voc_cal_audfmt_spk_phone_iir_filter_12k,  /* 12k  */
  &voc_cal_audfmt_spk_phone_iir_filter_16k,  /* 16k  */
  &voc_cal_audfmt_spk_phone_iir_filter_22k,  /* 22k  */
  &voc_cal_audfmt_spk_phone_iir_filter_24k,  /* 24k  */
  &voc_cal_audfmt_spk_phone_iir_filter_32k,  /* 32k  */
  &voc_cal_audfmt_spk_phone_iir_filter_44k, /* 44p1k */
  &voc_cal_audfmt_spk_phone_iir_filter_48k   /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_audfmt_spk_phone_iir_filter_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

/* IIR Filter table for Stereo USB */

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_8k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_11k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_12k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_16k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_22k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_24k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_32k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_44k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff_48k[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{
  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY uint16 voc_cal_audfmt_stereo_usb_iir_filter_coeff[
    VOC_CAL_AUDFMT_IIR_FILTER_NUM * QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE] =
{

  /* Band NCoeff */
  COEFF(0x4068, 0x1d6f), COEFF(0x8218, 0x5c25), COEFF(0x3d9f, 0x1f28),
  COEFF(0x4119, 0x47df), COEFF(0x8436, 0x5aef), COEFF(0x3b24, 0xe51b),
  COEFF(0x41ca, 0x6229), COEFF(0x875b, 0xea73), COEFF(0x3816, 0x3449),
  COEFF(0x44bc, 0x5515), COEFF(0x9a00, 0xbf27), COEFF(0x2b12, 0x2f1e),
  /* Band MCoeff  */
  COEFF(0x8218, 0x5c25), COEFF(0x3e07, 0x3cf0),
  COEFF(0x8436, 0x5aef), COEFF(0x3c3e, 0x2d37),
  COEFF(0x875b, 0xea73), COEFF(0x39e0, 0x9695),
  COEFF(0x9a00, 0xbf27), COEFF(0x2fce, 0x8435),
  /* Shift factor */
  0x0002,
  0x0002,
  0x0002,
  0x0002,
  /* channel selection */
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_LEFT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_RIGHT,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH,
  VOC_CAL_AUDFMT_IIR_FILTER_PAN_BOTH
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_8k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_8k        /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_11k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_11k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_12k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_12k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_16k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_16k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_22k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_22k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_24k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_24k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_32k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_32k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_44k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_44k       /* Coefficients     */
};

CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_48k = {
  TRUE,                                                /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                       /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff_48k       /* Coefficients     */
};


CAL_MEMORY qdsp_cmd_audfmt_iir_filter_type
                                  voc_cal_audfmt_stereo_usb_iir_filter_default = {
  FALSE,                                                 /* Enable/disable   */
  VOC_CAL_AUDFMT_IIR_FILTER_NUM,                         /* Number of bands  */
  voc_cal_audfmt_stereo_usb_iir_filter_coeff             /* Coefficients     */
};


const qdsp_cmd_audfmt_iir_filter_type*
        voc_cal_audfmt_stereo_usb_iir_filter_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_audfmt_stereo_usb_iir_filter_8k,  /* DO NOT USE     */
  &voc_cal_audfmt_stereo_usb_iir_filter_8k,  /* 8k   */
  &voc_cal_audfmt_stereo_usb_iir_filter_11k,  /* 11k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_12k,  /* 12k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_16k,  /* 16k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_22k,  /* 22k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_24k,  /* 24k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_32k,  /* 32k  */
  &voc_cal_audfmt_stereo_usb_iir_filter_44k, /* 44p1k */
  &voc_cal_audfmt_stereo_usb_iir_filter_48k   /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_audfmt_stereo_usb_iir_filter_44k  /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

#endif /* MSMAUD_QSC60X0_AUDIO_CAL  || MSMAUD_QSC60X5_AUDIO_CAL */



/* Undefine Coefficient MACRO
*/
#undef COEFF
#endif /* FEATURE_AUDFMT_IIR_FILTER */

#if defined(FEATURE_AUDFMT_FIR_FILTER) && defined(FEATURE_QSYNTH_COMMON)
/* Default configuration for QSynth PCM Filter coefficients.
*/
CAL_MEMORY uint16 voc_qsynth_pcm_filter_coeff[] = {
  0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x4000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000,0x0000,0x0000,0x0000,
  0x0000,0x0000
};


/* Default values for Qsynth PCM filter controls
*/
CAL_MEMORY qdsp_cmd_audfmt_pcm_filter_type voc_cal_qsynth_pcm_filter[] = {
  {
    0x0000,                         /* PCM filter enable/disable flag */
    (uint16 *) voc_qsynth_pcm_filter_coeff,
                                    /* FIR filter coefficients         */
    0x001B,                         /* Number of coefficients          */
    0x0002                          /* Shift factor                    */
  }
#ifdef MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT
  ,{
    0x0000,                         /* PCM filter endable/disable flag */
    (uint16 *) voc_qsynth_pcm_filter_coeff,
                                    /* FIR filter coefficients         */
    0x001B,                         /* Number of coefficients          */
    0x0002                          /* Shift factor                    */
  }
#endif /* MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT */
};
#endif /* FEATURE_AUDFMT_FIR_FILTER  && FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QSYNTH_ADRC

/* Default values for Qsynth Audio Dynamic Range Compressor controls
*/

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_default = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_8k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_11p025k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_12k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_16k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_22p05k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_24k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_32k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_44p1k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_48k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FC3, 0xFAF9 },             /* Attack constant                 */
  { 0x7FFB, 0xFEC9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};


CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_adrc_ctl_44k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

const qdsp_cmd_audfmt_adrc_ctl_type*
               voc_cal_qsynth_adrc_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qsynth_adrc_ctl_default, /* Dummy for ZERO k - DO NOT USE     */
  &voc_cal_qsynth_adrc_ctl_8k,       /* 8k   */
  &voc_cal_qsynth_adrc_ctl_11p025k,  /* 11k  */
  &voc_cal_qsynth_adrc_ctl_12k,      /* 12k  */
  &voc_cal_qsynth_adrc_ctl_16k,      /* 16k  */
  &voc_cal_qsynth_adrc_ctl_22p05k,   /* 22p05k  */
  &voc_cal_qsynth_adrc_ctl_24k,      /* 24k  */
  &voc_cal_qsynth_adrc_ctl_32k,      /* 32k  */
  &voc_cal_qsynth_adrc_ctl_44p1k,    /* 44p1k */
  &voc_cal_qsynth_adrc_ctl_48k       /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qsynth_adrc_ctl_44k      /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

/* Default values for Qsynth Audio Dynamic Range Compressor controls
  for Stereo Headset */
CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_default = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_8k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_11p025k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_12k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_16k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_22p05k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_24k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_32k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_44p1k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_48k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FC3, 0xFAF9 },             /* Attack constant                 */
  { 0x7FFB, 0xFEC9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_headset_adrc_ctl_44k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

const qdsp_cmd_audfmt_adrc_ctl_type*
               voc_cal_qsynth_stereo_headset_adrc_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qsynth_stereo_headset_adrc_ctl_default, /* Dummy for ZERO k - DO NOT USE     */
  &voc_cal_qsynth_stereo_headset_adrc_ctl_8k,       /* 8k   */
  &voc_cal_qsynth_stereo_headset_adrc_ctl_11p025k,  /* 11k  */
  &voc_cal_qsynth_stereo_headset_adrc_ctl_12k,      /* 12k  */
  &voc_cal_qsynth_stereo_headset_adrc_ctl_16k,      /* 16k  */
  &voc_cal_qsynth_stereo_headset_adrc_ctl_22p05k,   /* 22p05k  */
  &voc_cal_qsynth_stereo_headset_adrc_ctl_24k,      /* 24k  */
  &voc_cal_qsynth_stereo_headset_adrc_ctl_32k,      /* 32k  */
  &voc_cal_qsynth_stereo_headset_adrc_ctl_44p1k,    /* 44p1k */
  &voc_cal_qsynth_stereo_headset_adrc_ctl_48k       /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qsynth_stereo_headset_adrc_ctl_44k      /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};


/* Default values for Qsynth Audio Dynamic Range Compressor controls
  for  Speaker Phone */
#if defined(MSMAUD_QSC60X0_AUDIO_CAL) || defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
    defined(MSMAUD_QSC1110_AUDIO_CAL) || defined (MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_default = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x7C5F },             /* Attack constant                 */
  { 0x7FBE, 0xAD84 },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_8k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7D36, 0xF9DC },             /* Attack constant                 */
  { 0x7E99, 0x86F7 },             /* Release constant                */
  0x0008                          /* System delay                    */
};


CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_11p025k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7DF9, 0x0DD5 },             /* Attack constant                 */
  { 0x7EBF, 0x7DD1 },             /* Release constant                */
  0x000B                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_12k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7E22, 0xE8ED },             /* Attack constant                 */
  { 0x7F10, 0x948D },             /* Release constant                */
  0x000C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_16k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7E99, 0x86F7 },             /* Attack constant                 */
  { 0x7F4C, 0x454D },             /* Release constant                */
  0x0010                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_22p05k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7EFB, 0x7DD1 },             /* Attack constant                 */
  { 0x7F7D, 0x7C5F },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_24k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F10, 0x948D },             /* Attack constant                 */
  { 0x7F88, 0x1217 },             /* Release constant                */
  0x0018                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_32k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F4C, 0x454D },             /* Attack constant                 */
  { 0x7FA6, 0x0304 },             /* Release constant                */
  0x0020                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_44p1k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x7C5F },             /* Attack constant                 */
  { 0x7FBE, 0xAD84 },             /* Release constant                */
  0x002C                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_48k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F88, 0x1217 },             /* Attack constant                 */
  { 0x7FC3, 0xFAF9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type voc_cal_qsynth_spk_phone_adrc_ctl_44k = {
  0xFFFF,                         /* ADRC enable/disable flag        */
  0x2580,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x7C5F },             /* Attack constant                 */
  { 0x7FBE, 0xAD84 },             /* Release constant                */
  0x002C                          /* System delay                    */
};

#else

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_default = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_8k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_11p025k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_12k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_16k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_22p05k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_24k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_32k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_44p1k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_48k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FC3, 0xFAF9 },             /* Attack constant                 */
  { 0x7FFB, 0xFEC9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type
voc_cal_qsynth_spk_phone_adrc_ctl_44k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};
#endif

const qdsp_cmd_audfmt_adrc_ctl_type*
               voc_cal_qsynth_spk_phone_adrc_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qsynth_spk_phone_adrc_ctl_default, /* Dummy for ZERO k - DO NOT USE     */
  &voc_cal_qsynth_spk_phone_adrc_ctl_8k,       /* 8k   */
  &voc_cal_qsynth_spk_phone_adrc_ctl_11p025k,  /* 11k  */
  &voc_cal_qsynth_spk_phone_adrc_ctl_12k,      /* 12k  */
  &voc_cal_qsynth_spk_phone_adrc_ctl_16k,      /* 16k  */
  &voc_cal_qsynth_spk_phone_adrc_ctl_22p05k,   /* 22p05k  */
  &voc_cal_qsynth_spk_phone_adrc_ctl_24k,      /* 24k  */
  &voc_cal_qsynth_spk_phone_adrc_ctl_32k,      /* 32k  */
  &voc_cal_qsynth_spk_phone_adrc_ctl_44p1k,    /* 44p1k */
  &voc_cal_qsynth_spk_phone_adrc_ctl_48k       /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qsynth_spk_phone_adrc_ctl_44k      /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};


/* Default values for Qsynth Audio Dynamic Range Compressor controls
  for  Stereo USB */

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_default = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_8k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_11p025k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_12k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_16k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_22p05k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_24k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_32k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_44p1k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_48k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7FC3, 0xFAF9 },             /* Attack constant                 */
  { 0x7FFB, 0xFEC9 },             /* Release constant                */
  0x0030                          /* System delay                    */
};

CAL_MEMORY qdsp_cmd_audfmt_adrc_ctl_type 
voc_cal_qsynth_stereo_usb_adrc_ctl_44k = {
  0x0000,                         /* ADRC enable/disable flag        */
  0x1E0C,                         /* Compression threshold           */
  0xCCCD,                         /* Compression slope               */
  0x0100,                         /* RMS signal energy time-ave const*/
  { 0x7F7D, 0x30AD },             /* Attack constant                 */
  { 0x7FF9, 0x7247 },             /* Release constant                */
  0x0016                          /* System delay                    */
};

const qdsp_cmd_audfmt_adrc_ctl_type*
               voc_cal_qsynth_stereo_usb_adrc_ctl[VOC_SAMPLE_RATE_MAX] = {
  &voc_cal_qsynth_stereo_usb_adrc_ctl_default, /* Dummy for ZERO k - DO NOT USE     */
  &voc_cal_qsynth_stereo_usb_adrc_ctl_8k,       /* 8k   */
  &voc_cal_qsynth_stereo_usb_adrc_ctl_11p025k,  /* 11k  */
  &voc_cal_qsynth_stereo_usb_adrc_ctl_12k,      /* 12k  */
  &voc_cal_qsynth_stereo_usb_adrc_ctl_16k,      /* 16k  */
  &voc_cal_qsynth_stereo_usb_adrc_ctl_22p05k,   /* 22p05k  */
  &voc_cal_qsynth_stereo_usb_adrc_ctl_24k,      /* 24k  */
  &voc_cal_qsynth_stereo_usb_adrc_ctl_32k,      /* 32k  */
  &voc_cal_qsynth_stereo_usb_adrc_ctl_44p1k,    /* 44p1k */
  &voc_cal_qsynth_stereo_usb_adrc_ctl_48k       /* 48k  */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,&voc_cal_qsynth_stereo_usb_adrc_ctl_44k      /* 44k  */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

#endif /* FEATURE_QSYNTH_ADRC */


/* Default values for ESEC echo canceller
*/
CAL_MEMORY qdsp_cmd_ec_params_type voc_cal_ec_params_esec =
{
  0x0000, /* 0ms     */             /* Farend hangover threshold       */
  0x00A0, /* 20ms    */             /* Doubletalk hangover threshold   */
  0x0190, /* 50ms    */             /* Startup mute hangover threshold */
  0x0000, /* NOMINAL */             /* Startup mute mode               */
  0x007F  /* 12dB    */             /* Startup ERLE threshold          */
};

/* Default values for HEADSET echo canceller
*/
CAL_MEMORY qdsp_cmd_ec_params_type voc_cal_ec_params_headset =
{
  0x0000, /* 0ms     */             /* Farend hangover threshold       */
  0x00A0, /* 20ms    */             /* Doubletalk hangover threshold   */
  0x0190, /* 50ms    */             /* Startup mute hangover threshold */
  0x0000, /* NOMINAL */             /* Startup mute mode               */
  0x007F  /* 12dB    */             /* Startup ERLE threshold          */
};

/* Default values for AEC echo canceller
*/
CAL_MEMORY qdsp_cmd_ec_params_type voc_cal_ec_params_aec =
{
  0x0320, /* 100ms   */             /* Farend hangover threshold       */
  0x0320, /* 100ms   */             /* Doubletalk hangover threshold   */
  0x0190, /* 50ms    */             /* Startup mute hangover threshold */
  0x0000, /* NOMINAL */             /* Startup mute mode               */
  0x007F  /* 12dB    */             /* Startup ERLE threshold          */
};

/* Default values for SPEAKER echo canceller
*/
CAL_MEMORY qdsp_cmd_ec_params_type voc_cal_ec_params_speaker =
{
  0x0320, /* 100ms   */             /* Farend hangover threshold       */
  0x0320, /* 100ms   */             /* Doubletalk hangover threshold   */
  0x07D0, /* 250ms   */             /* Startup mute hangover threshold */
  0x0000, /* NOMINAL */             /* Startup mute mode               */
  0x7FFF  /* MAX     */             /* Startup ERLE threshold          */
};

/* Default values for BT_HEADSET echo canceller (copied from ESEC default)
*/
CAL_MEMORY qdsp_cmd_ec_params_type voc_cal_ec_params_bt_headset =
{
  0x0000, /* 0ms     */             /* Farend hangover threshold       */
  0x00A0, /* 20ms    */             /* Doubletalk hangover threshold   */
  0x0190, /* 50ms    */             /* Startup mute hangover threshold */
  0x0000, /* NOMINAL */             /* Startup mute mode               */
  0x007F  /* 12dB    */             /* Startup ERLE threshold          */
};

/* Set of default values for echo canceller
*/
const qdsp_cmd_ec_params_type * voc_cal_ec_default_params[VOC_EC_DEFAULT] =
{
  /* VOC_EC_OFF     */ NULL
  /* VOC_EC_ESEC    */,&voc_cal_ec_params_esec
  /* VOC_EC_HEADSET */,&voc_cal_ec_params_headset
  /* VOC_EC_AEC     */,&voc_cal_ec_params_aec
  /* VOC_EC_SPEAKER */,&voc_cal_ec_params_speaker
  /* VOC_EC_BT_HEADSET */,&voc_cal_ec_params_bt_headset
};


/* Default values for ESEC RVE Single Mic
*/
CAL_MEMORY qdsp_cmd_rve_param_type voc_cal_rve_params_esec =
{ /* RVE PARAMETERS */
    3276,
    3276,
    767,
    2767,
    168,
    2767,
    17767,
    4767,
    32,
    120,
    0,
    0,
    8192,
    8192,
    32767,
    32767,
    {0,0,0,0},
    {4915,4915,4915,18022},
    32767,
    1,
    200,
    4
};

/* Default values for HEADSET RVE Single Mic
*/
CAL_MEMORY qdsp_cmd_rve_param_type voc_cal_rve_params_headset =
{ /* RVE PARAMETERS */
    3276,
    3276,
    767,
    2767,
    168,
    2767,
    17767,
    4767,
    32,
    120,
    0,
    0,
    8192,
    8192,
    32767,
    32767,
    {0,0,0,0},
    {4915,4915,4915,18022},
    32767,
    1,
    200,
    4
};


/* Default values for AEC RVE Single Mic
*/
CAL_MEMORY qdsp_cmd_rve_param_type voc_cal_rve_params_aec =
{ /* RVE PARAMETERS */
    3276,
    3276,
    767,
    2767,
    168,
    2767,
    17767,
    4767,
    32,
    120,
    0,
    0,
    8192,
    8192,
    32767,
    32767,
    {0,0,0,0},
    {4915,4915,4915,18022},
    32767,
    1,
    200,
    4
};


/* Default values for SPEAKER RVE Single Mic
*/

CAL_MEMORY qdsp_cmd_rve_param_type voc_cal_rve_params_speaker =
{ /* RVE PARAMETERS */
    3276,
    3276,
    767,
    2767,
    168,
    2767,
    17767,
    4767,
    32,
    120,
    0,
    0,
    8192,
    8192,
    32767,
    32767,
    {0,0,0,0},
    {4915,4915,4915,18022},
    32767,
    1,
    200,
    4
};

/* Default values for BT_HEADSET RVE Single Mic
*/
CAL_MEMORY qdsp_cmd_rve_param_type voc_cal_rve_params_bt_headset =
{ /* RVE PARAMETERS */
    3276,
    3276,
    767,
    2767,
    168,
    2767,
    17767,
    4767,
    32,
    120,
    0,
    0,
    8192,
    8192,
    32767,
    32767,
    {0,0,0,0},
    {4915,4915,4915,18022},
    32767,
    1,
    200,
    4
};


/*
 * Set of default values for RVE Single Mic
*/
const qdsp_cmd_rve_param_type * voc_cal_rve_default_params[VOC_EC_DEFAULT] =
{
  /* VOC_EC_OFF     */ NULL
  /* VOC_EC_ESEC    */,&voc_cal_rve_params_esec
  /* VOC_EC_HEADSET */,&voc_cal_rve_params_headset
  /* VOC_EC_AEC     */,&voc_cal_rve_params_aec
  /* VOC_EC_SPEAKER */,&voc_cal_rve_params_speaker
  /* VOC_EC_BT_HEADSET */,&voc_cal_rve_params_bt_headset
};


#ifdef FEATURE_AVS_NEXTGEN_EC

CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_handset = {
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x7FFF,     /* AF_limit */
  0x109F,     /* HS mode */
  0x0,        /* Tuning mode */
  0x14,       /* echo_path_delay */
  0x800,      /* OutputGain */
  0x2000,     /* InputGain */
  0x2000,     /* AF_twoalpha */
  0xFA,       /* AF_erl */
  0x46,       /* AF_taps */
  0x1,        /* AF_preset_coefs */
  0x2FF,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x4650,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x41A0,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x4A38,     /* DENS_tail_alpha */
  0x1770,     /* DENS_tail_portion */
  0x0,        /* DENS_gamma_e_alpha */
  0x100,      /* DENS_gamma_e_dt */
  0x100,      /* DENS_gamma_e_low */
  0x400,      /* DENS_gamma_e_rescue */
  0x200,      /* DENS_gamma_e_high */
  0x400,      /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x1E6,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x2879,     /* DENS_limit_NS */
  0x1C2,      /* DENS_NL_atten */
  0x2EE0,     /* DENS_CNI_level */
  0xFA0       /* WB_echo_ratio */
};

CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_headset = {
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x7FFF,     /* AF_limit */
  0x109F,     /* HS mode */
  0x0,        /* Tuning mode */
  0x14,       /* echo_path_delay */
  0x800,      /* OutputGain */
  0x2000,     /* InputGain */
  0x2000,     /* AF_twoalpha */
  0x200,      /* AF_erl */
  0x3C,       /* AF_taps */
  0x1,        /* AF_preset_coefs */
  0x2FF,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x2710,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x41A0,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x4650,     /* DENS_tail_alpha */
  0xBB8,      /* DENS_tail_portion */
  0x0,        /* DENS_gamma_e_alpha */
  0x12C,      /* DENS_gamma_e_dt */
  0x12C,      /* DENS_gamma_e_low */
  0x400,      /* DENS_gamma_e_rescue */
  0x200,      /* DENS_gamma_e_high */
  0x200,      /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x1E6,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x2879,     /* DENS_limit_NS */
  0x384,      /* DENS_NL_atten */
  0x32C8,     /* DENS_CNI_level */
  0xFA0       /* WB_echo_ratio */
};

CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_handsfree = {
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x7FFF,     /* AF_limit */
  0x179F,     /* HS mode */
  0x0,        /* Tuning mode */
  0x14,       /* echo_path_delay */
  0x800,      /* OutputGain */
  0x2000,     /* InputGain */
  0x2000,     /* AF_twoalpha */
  0x80,       /* AF_erl */
  0x8C,       /* AF_taps */
  0x1,        /* AF_preset_coefs */
  0x190,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x36B0,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x41A0,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x4A38,     /* DENS_tail_alpha */
  0x1770,     /* DENS_tail_portion */
  0x0,        /* DENS_gamma_e_alpha */
  0x190,      /* DENS_gamma_e_dt */
  0x190,      /* DENS_gamma_e_low */
  0x1E00,     /* DENS_gamma_e_rescue */
  0x258,      /* DENS_gamma_e_high */
  0x400,      /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x1E6,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x286D,     /* DENS_limit_NS */
  0x200,      /* DENS_NL_atten */
  0x32C8,     /* DENS_CNI_level */
  0xFA0       /* WB_echo_ratio */
};

CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_speaker =
{
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x7FFF,     /* AF_limit */
  0x179F,     /* HS mode */
  0x0,        /* Tuning mode */
  0x14,       /* echo_path_delay */
  0x800,      /* OutputGain */
  0x2000,     /* InputGain */
  0x2000,     /* AF_twoalpha */
  0x80,       /* AF_erl */
  0x8C,       /* AF_taps */
  0x1,        /* AF_preset_coefs */
  0x190,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x36B0,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x41A0,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x4A38,     /* DENS_tail_alpha */
  0x1770,     /* DENS_tail_portion */
  0x0,        /* DENS_gamma_e_alpha */
  0x190,      /* DENS_gamma_e_dt */
  0x190,      /* DENS_gamma_e_low */
  0x1E00,     /* DENS_gamma_e_rescue */
  0x258,      /* DENS_gamma_e_high */
  0x400,      /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x1E6,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x286D,     /* DENS_limit_NS */
  0x200,      /* DENS_NL_atten */
  0x32C8,     /* DENS_CNI_level */
  0xFA0       /* WB_echo_ratio */
};

/* Alternative set of default parameters to use for Speakerphone
{
  0x7FFF,     // NLPP_limit
  0x800,      // NLPP_gain
  0x7FFF,     // AF_limit
  0x179F,     // HS mode
  0x0,        // Tuning mode
  0x14,       // echo_path_delay
  0x800,      // OutputGain
  0x2000,     // InputGain
  0x2000,     // AF_twoalpha
  0x80,       // AF_erl
  0x8C,       // AF_taps
  0x1,        // AF_preset_coefs
  0x190,      // AF_offset
  0x40,       // AF_erl_bg
  0x20,       // AF_taps_bg
  0x36B0,     // PCD_threshold
  0x40,       // minimum_erl
  0x41A0,     // erl_step
  0x800,      // max_noise_floor
  0x63,       // det_threshold
  0x4E20,     // SPDET_far
  0x4E20,     // SPDET_mic
  0x1,        // SPDET_xclip
  0x3E80,     // DENS_tail_alpha
  0xBB8,      // DENS_tail_portion
  0x0,        // DENS_gamma_e_alpha
  0x140,      // DENS_gamma_e_dt
  0x140,      // DENS_gamma_e_low
  0x1E00,     // DENS_gamma_e_rescue
  0x1C2,      // DENS_gamma_e_high
  0x400,      // DENS_spdet_near
  0x300,      // DENS_spdet_act
  0x1E6,      // DENS_gamma_n
  0x190,      // DENS_NFE_blocksize
  0x286D,     // DENS_limit_NS
  0x200,      // DENS_NL_atten
  0x32C8,     // DENS_CNI_level
  0xFA0       // WB_echo_ratio
};
*/


CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_bt_headset =
{
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x21DB,     /* AF_limit */
  0x109F,     /* HS mode */
  0x0,        /* Tuning mode */
  0x78,       /* echo_path_delay */
  0x800,      /* OutputGain */
  0x2000,     /* InputGain */
  0x2000,     /* AF_twoalpha */
  0x140,      /* AF_erl */
  0xC8,       /* AF_taps */
  0x1,        /* AF_preset_coefs */
  0x190,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x3977,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x408A,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x6D60,     /* DENS_tail_alpha */
  0x4650,     /* DENS_tail_portion */
  0x0,        /* DENS_gamma_e_alpha */
  0x106,      /* DENS_gamma_e_dt */
  0x161,      /* DENS_gamma_e_low */
  0x1E00,     /* DENS_gamma_e_rescue */
  0x384,      /* DENS_gamma_e_high */
  0x1912,     /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x1DD,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x286D,     /* DENS_limit_NS */
  0x300,      /* DENS_NL_atten */
  0x2D55,     /* DENS_CNI_level */
  0x100       /* WB_echo_ratio */
};

/* 1st alternative set of default parameters to use for Bluetooth headset
{
  0x7FFF,     // NLPP_limit
  0x800,      // NLPP_gain
  0x21DB,     // AF_limit
  0x109F,     // HS mode
  0x0,        // Tuning mode
  0x78,       // echo_path_delay
  0x800,      // OutputGain
  0x2000,     // InputGain
  0x2000,     // AF_twoalpha
  0x1A4,      // AF_erl
  0xC8,       // AF_taps
  0x1,        // AF_preset_coefs
  0x190,      // AF_offset
  0x40,       // AF_erl_bg
  0x20,       // AF_taps_bg
  0x3977,     // PCD_threshold
  0x40,       // minimum_erl
  0x408A,     // erl_step
  0x800,      // max_noise_floor
  0x63,       // det_threshold
  0x4E20,     // SPDET_far
  0x4E20,     // SPDET_mic
  0x1,        // SPDET_xclip
  0x5DC0,     // DENS_tail_alpha
  0x2EE0,     // DENS_tail_portion
  0x0,        // DENS_gamma_e_alpha
  0x106,      // DENS_gamma_e_dt
  0x161,      // DENS_gamma_e_low
  0x1E00,     // DENS_gamma_e_rescue
  0x2BC,      // DENS_gamma_e_high
  0x1912,     // DENS_spdet_near
  0x300,      // DENS_spdet_act
  0x1DD,      // DENS_gamma_n
  0x190,      // DENS_NFE_blocksize
  0x286D,     // DENS_limit_NS
  0x300,      // DENS_NL_atten
  0x2D55,     // DENS_CNI_level
  0x100       // WB_echo_ratio
};
*/

/* 2nd alternative set of default parameters to use for Bluetooth headset
{
  0x7FFF,     // NLPP_limit
  0x800,      // NLPP_gain
  0x21DB,     // AF_limit
  0x109F,     // HS mode
  0x0,        // Tuning mode
  0x78,       // echo_path_delay
  0x800,      // OutputGain
  0x2000,     // InputGain
  0x2000,     // AF_twoalpha
  0x1A4,      // AF_erl
  0xC8,       // AF_taps
  0x1,        // AF_preset_coefs
  0x190,      // AF_offset
  0x40,       // AF_erl_bg
  0x20,       // AF_taps_bg
  0x3977,     // PCD_threshold
  0x40,       // minimum_erl
  0x408A,     // erl_step
  0x800,      // max_noise_floor
  0x63,       // det_threshold
  0x4E20,     // SPDET_far
  0x4E20,     // SPDET_mic
  0x1,        // SPDET_xclip
  0x5208,     // DENS_tail_alpha
  0x1F40,     // DENS_tail_portion
  0x0,        // DENS_gamma_e_alpha
  0x106,      // DENS_gamma_e_dt
  0x161,      // DENS_gamma_e_low
  0x1E00,     // DENS_gamma_e_rescue
  0x200,      // DENS_gamma_e_high
  0x1912,     // DENS_spdet_near
  0x300,      // DENS_spdet_act
  0x1DD,      // DENS_gamma_n
  0x190,      // DENS_NFE_blocksize
  0x286D,     // DENS_limit_NS
  0xC8,       // DENS_NL_atten
  0x2D55,     // DENS_CNI_level
  0x100       // WB_echo_ratio
};
*/

const qdsp_cmd_nextgen_ec_parms_type*
voc_cal_nextgen_ec_default_parms[VOC_EC_DEFAULT] =
{
  /* VOC_EC_OFF              */ NULL
  /* VOC_EC_ESEC (handset)   */,&voc_cal_nextgen_ec_parms_handset
  /* VOC_EC_HEADSET          */,&voc_cal_nextgen_ec_parms_headset
  /* VOC_EC_AEC (handsfree)  */,&voc_cal_nextgen_ec_parms_handsfree
  /* VOC_EC_SPEAKER          */,&voc_cal_nextgen_ec_parms_speaker
  /* VOC_EC_BT_HEADSET       */,&voc_cal_nextgen_ec_parms_bt_headset
};

/* ====== wideband new EC configurations ========= */

CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_wb_handset =
{
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x7FFF,     /* AF_limit */
  0x14FF,     /* HS mode */
  0x0,        /* Tuning mode */
  0x14,       /* echo_path_delay */
  0x800,      /* OutputGain */
  0x2000,     /* InputGain */
  0x2000,     /* AF_twoalpha */
  0xFA,       /* AF_erl */
  0x46,       /* AF_taps */
  0x1,        /* AF_preset_coefs */
  0x2FF,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x4650,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x41A0,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x4A38,     /* DENS_tail_alpha */
  0x1770,     /* DENS_tail_portion */
  0x0,        /* DENS_gamma_e_alpha */
  0x100,      /* DENS_gamma_e_dt */
  0x100,      /* DENS_gamma_e_low */
  0x400,      /* DENS_gamma_e_rescue */
  0x200,      /* DENS_gamma_e_high */
  0x400,      /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x320,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x1214,     /* DENS_limit_NS */
  0x1C2,      /* DENS_NL_atten */
  0x2EE0,     /* DENS_CNI_level */
  0xFA0       /* WB_echo_ratio */
};

CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_wb_headset =
{
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x7FFF,     /* AF_limit */
  0x14FF,     /* HS mode */
  0x0,        /* Tuning mode */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x23,       /* echo_path_delay */
#else
  0x14,       /* echo_path_delay */
#endif
  0x800,      /* OutputGain */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x4000,     /* InputGain */
#else
  0x2000,     /* InputGain */
#endif
  0x2000,     /* AF_twoalpha */
  0x200,      /* AF_erl */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x64,       /* AF_taps */
#else
  0x3C,       /* AF_taps */
#endif
  0x1,        /* AF_preset_coefs */
  0x2FF,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x2710,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x41A0,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x4650,     /* DENS_tail_alpha */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x1770,      /* DENS_tail_portion */
#else
  0xBB8,      /* DENS_tail_portion */
#endif
  0x0,        /* DENS_gamma_e_alpha */
  0x12C,      /* DENS_gamma_e_dt */
  0x12C,      /* DENS_gamma_e_low */
  0x400,      /* DENS_gamma_e_rescue */
  0x200,      /* DENS_gamma_e_high */
  0x200,      /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x320,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x1214,     /* DENS_limit_NS */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x550,      /* DENS_NL_atten */
#else
  0x384,      /* DENS_NL_atten */
#endif
  0x32C8,     /* DENS_CNI_level */
  0xFA0       /* WB_echo_ratio */
};

CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_wb_handsfree
= {
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x7FFF,     /* AF_limit */
  0x17FF,     /* HS mode */
  0x0,        /* Tuning mode */
  0x14,       /* echo_path_delay */
  0x800,      /* OutputGain */
  0x2000,     /* InputGain */
  0x2000,     /* AF_twoalpha */
  0x80,       /* AF_erl */
  0x8C,       /* AF_taps */
  0x1,        /* AF_preset_coefs */
  0x190,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x36B0,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x41A0,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x4A38,     /* DENS_tail_alpha */
  0x1770,     /* DENS_tail_portion */
  0x0,        /* DENS_gamma_e_alpha */
  0x190,      /* DENS_gamma_e_dt */
  0x190,      /* DENS_gamma_e_low */
  0x1E00,     /* DENS_gamma_e_rescue */
  0x258,      /* DENS_gamma_e_high */
  0x400,      /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x320,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x1214,     /* DENS_limit_NS */
  0x200,      /* DENS_NL_atten */
  0x32C8,     /* DENS_CNI_level */
  0xFA0       /* WB_echo_ratio */
};

CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_wb_speaker =
{
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x7FFF,     /* AF_limit */
  0x14FF,     /* HS mode */
  0x0,        /* Tuning mode */
  0x14,       /* echo_path_delay */
  0x800,      /* OutputGain */
  0x2000,     /* InputGain */
  0x2000,     /* AF_twoalpha */
  0x80,       /* AF_erl */
  0x8C,       /* AF_taps */
  0x1,        /* AF_preset_coefs */
  0x190,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x36B0,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x41A0,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x4A38,     /* DENS_tail_alpha */
  0x1770,     /* DENS_tail_portion */
  0x0,        /* DENS_gamma_e_alpha */
  0x190,      /* DENS_gamma_e_dt */
  0x190,      /* DENS_gamma_e_low */
  0x1E00,     /* DENS_gamma_e_rescue */
  0x258,      /* DENS_gamma_e_high */
  0x400,      /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x320,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x1214,     /* DENS_limit_NS */
  0x200,      /* DENS_NL_atten */
  0x32C8,     /* DENS_CNI_level */
  0xFA0       /* WB_echo_ratio */
};

/* Alternative set of default parameters to use for Speakerphone
{
  0x7FFF,     // NLPP_limit
  0x800,      // NLPP_gain
  0x7FFF,     // AF_limit
  0x14FF,     // HS mode
  0x0,        // Tuning mode
  0x14,       // echo_path_delay
  0x800,      // OutputGain
  0x2000,     // InputGain
  0x2000,     // AF_twoalpha
  0x80,       // AF_erl
  0x8C,       // AF_taps
  0x1,        // AF_preset_coefs
  0x190,      // AF_offset
  0x40,       // AF_erl_bg
  0x20,       // AF_taps_bg
  0x36B0,     // PCD_threshold
  0x40,       // minimum_erl
  0x41A0,     // erl_step
  0x800,      // max_noise_floor
  0x63,       // det_threshold
  0x4E20,     // SPDET_far
  0x4E20,     // SPDET_mic
  0x1,        // SPDET_xclip
  0x3E80,     // DENS_tail_alpha
  0xBB8,      // DENS_tail_portion
  0x0,        // DENS_gamma_e_alpha
  0x140,      // DENS_gamma_e_dt
  0x140,      // DENS_gamma_e_low
  0x1E00,     // DENS_gamma_e_rescue
  0x1C2,      // DENS_gamma_e_high
  0x400,      // DENS_spdet_near
  0x300,      // DENS_spdet_act
  0x320,      // DENS_gamma_n
  0x190,      // DENS_NFE_blocksize
  0x1214,     // DENS_limit_NS
  0x200,      // DENS_NL_atten
  0x32C8,     // DENS_CNI_level
  0xFA0       // WB_echo_ratio
};
*/


CAL_MEMORY qdsp_cmd_nextgen_ec_parms_type voc_cal_nextgen_ec_parms_wb_bt_headset
= {
  0x7FFF,     /* NLPP_limit */
  0x800,      /* NLPP_gain */
  0x21DB,     /* AF_limit */
  0x14FF,     /* HS mode */
  0x0,        /* Tuning mode */
  0x78,       /* echo_path_delay */
  0x800,      /* OutputGain */
  0x2000,     /* InputGain */
  0x2000,     /* AF_twoalpha */
  0xC8,       /* AF_erl */
  0x190,      /* AF_taps */
  0x2,        /* AF_preset_coefs */
  0x190,      /* AF_offset */
  0x40,       /* AF_erl_bg */
  0x20,       /* AF_taps_bg */
  0x36B0,     /* PCD_threshold */
  0x40,       /* minimum_erl */
  0x408A,     /* erl_step */
  0x800,      /* max_noise_floor */
  0x63,       /* det_threshold */
  0x4E20,     /* SPDET_far */
  0x4E20,     /* SPDET_mic */
  0x1,        /* SPDET_xclip */
  0x6D60,     /* DENS_tail_alpha */
  0x4650,     /* DENS_tail_portion */
  0x0,        /* DENS_gamma_e_alpha */
  0x106,      /* DENS_gamma_e_dt */
  0x161,      /* DENS_gamma_e_low */
  0x1E00,     /* DENS_gamma_e_rescue */
  0x384,      /* DENS_gamma_e_high */
  0x1000,     /* DENS_spdet_near */
  0x300,      /* DENS_spdet_act */
  0x320,      /* DENS_gamma_n */
  0x190,      /* DENS_NFE_blocksize */
  0x1214,     /* DENS_limit_NS */
  0x300,      /* DENS_NL_atten */
  0x2D55,     /* DENS_CNI_level */
  0x100       /* WB_echo_ratio */
};

/* 1st alternative set of default parameters to use for Bluetooth headset
{
  0x7FFF,     // NLPP_limit
  0x800,      // NLPP_gain
  0x21DB,     // AF_limit
  0x14FF,     // HS mode
  0x0,        // Tuning mode
  0x78,       // echo_path_delay
  0x800,      // OutputGain
  0x2000,     // InputGain
  0x2000,     // AF_twoalpha
  0xFA,       // AF_erl
  0x190,      // AF_taps
  0x2,        // AF_preset_coefs
  0x190,      // AF_offset
  0x40,       // AF_erl_bg
  0x20,       // AF_taps_bg
  0x36B0,     // PCD_threshold
  0x40,       // minimum_erl
  0x408A,     // erl_step
  0x800,      // max_noise_floor
  0x63,       // det_threshold
  0x4E20,     // SPDET_far
  0x4E20,     // SPDET_mic
  0x1,        // SPDET_xclip
  0x5DC0,     // DENS_tail_alpha
  0x2EE0,     // DENS_tail_portion
  0x0,        // DENS_gamma_e_alpha
  0x106,      // DENS_gamma_e_dt
  0x161,      // DENS_gamma_e_low
  0x1E00,     // DENS_gamma_e_rescue
  0x2BC,      // DENS_gamma_e_high
  0x1000,     // DENS_spdet_near
  0x300,      // DENS_spdet_act
  0x320,      // DENS_gamma_n
  0x190,      // DENS_NFE_blocksize
  0x1214,     // DENS_limit_NS
  0x300,      // DENS_NL_atten
  0x2D55,     // DENS_CNI_level
  0x100       // WB_echo_ratio
};
*/

/* 2nd alternative set of default parameters to use for Bluetooth headset
{
  0x7FFF,     // NLPP_limit
  0x800,      // NLPP_gain
  0x21DB,     // AF_limit
  0x14FF,     // HS mode
  0x0,        // Tuning mode
  0x78,       // echo_path_delay
  0x800,      // OutputGain
  0x2000,     // InputGain
  0x2000,     // AF_twoalpha
  0xFA,       // AF_erl
  0x190,      // AF_taps
  0x2,        // AF_preset_coefs
  0x190,      // AF_offset
  0x40,       // AF_erl_bg
  0x20,       // AF_taps_bg
  0x36B0,     // PCD_threshold
  0x40,       // minimum_erl
  0x408A,     // erl_step
  0x800,      // max_noise_floor
  0x63,       // det_threshold
  0x4E20,     // SPDET_far
  0x4E20,     // SPDET_mic
  0x1,        // SPDET_xclip
  0x5208,     // DENS_tail_alpha
  0x1F40,     // DENS_tail_portion
  0x0,        // DENS_gamma_e_alpha
  0x106,      // DENS_gamma_e_dt
  0x161,      // DENS_gamma_e_low
  0x1E00,     // DENS_gamma_e_rescue
  0x200,      // DENS_gamma_e_high
  0x1000,     // DENS_spdet_near
  0x300,      // DENS_spdet_act
  0x320,      // DENS_gamma_n
  0x190,      // DENS_NFE_blocksize
  0x1214,     // DENS_limit_NS
  0xC8,       // DENS_NL_atten
  0x2D55,     // DENS_CNI_level
  0x100       // WB_echo_ratio
};
*/

const qdsp_cmd_nextgen_ec_parms_type*
voc_cal_nextgen_ec_wb_default_parms[VOC_EC_DEFAULT] =
{
  /* VOC_EC_OFF              */ NULL
  /* VOC_EC_ESEC (handset)   */,&voc_cal_nextgen_ec_parms_wb_handset
  /* VOC_EC_HEADSET          */,&voc_cal_nextgen_ec_parms_wb_headset
  /* VOC_EC_AEC (handsfree)  */,&voc_cal_nextgen_ec_parms_wb_handsfree
  /* VOC_EC_SPEAKER          */,&voc_cal_nextgen_ec_parms_wb_speaker
  /* VOC_EC_BT_HEADSET       */,&voc_cal_nextgen_ec_parms_wb_bt_headset
};
#endif /* FEATURE_AVS_NEXTGEN_EC */


#ifdef FEATURE_AVS_CRYSTAL_SPEECH

CAL_MEMORY
qdsp_cmd_crystal_speech_parms_type voc_cal_crystal_speech_parms_handset = {
  0xfdff, /* CSMode */
  0x0000, /* CS_TuningMode */
  0x0028, /* CS_echo_path_delay */
  0x2000, /* AF1_twoalpha */
  0x03e8, /* AF1_erl */
  0x0040, /* AF1_taps */
  0x0001, /* AF1_preset_coefs */
  0x0308, /* AF1_offset */
  0x2000, /* AF2_twoalpha */
  0x00fa, /* AF2_erl */
  0x0020, /* AF2_taps */
  0x0001, /* AF2_preset_coefs */
  0x0308, /* AF2_offset */
  0x028f, /* PCD_twoalpha */
  0x0308, /* PCD_offset */
  0x2400, /* CSPCD_threshold */
  0x4000, /* WgThreshold */
  0x199a, /* MpThreshold */
  {0x7fff, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000}, /* SF_init_table0[8] */
  {0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000}, /* SF_init_table1[8] */
  0x0010, /* SF_taps */
  0x028f, /* SF_twoalpha */
  0x6666, /* DNNS_EchoAlphaRev */
  0x747b, /* DNNS_EchoYcomp */
  0x2000, /* DNNS_WbThreshold */
  0x4000, /* DNNS_EchoGammaHi */
  0x1000, /* DNNS_EchoGammaLo */
  0x2800, /* DNNS_NoiseGammaS */
  0x2800, /* DNNS_NoiseGammaN */
  0x0600, /* DNNS_NoiseGainMinS */
  0x0400, /* DNNS_NoiseGainMinN */
  0x2666, /* DNNS_NoiseBiasComp */
  0x2fc4, /* DNNS_AcThreshold */
  0x0100, /* WB_echo_ratio_2mic */
  0x0800, /* WB_Gamma_E */
  0x0400, /* WB_Gamma_NN */
  0x2000, /* WB_Gamma_SN */
  0x0000, /* vcodec_delay0 */
  0x0003, /* vcodec_delay1 */
  0x000f, /* vcodec_len0 */
  0x000f, /* vcodec_len1 */
  0x6666, /* vcodec_thr0 */
  0x0500, /* vcodec_thr1 */
  0x2000, /* FixCalFactorLeft */
  0x2000, /* FixCalFactorRight */
  {0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000}, /*uint32 vcodec_coeff0[15]*/
  {0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000}, /* uint32 vcodec_coeff1[15] */
  0x2000, /*CS_OutputGain*/
  0x3FFF, /*Enhance_MeuFilter1*/
  0x4189, /*Enhance_MeuFilter2*/
  0x4C10, /*Fix_OverEstimation*/
  0x7FFF, /*dmNLPPlimit*/
  0x800,   /*dmNLPPgain*/
  0x60 ,  /*windNoiseCfgNoiseThresh*/
  0xB00,  /*windNoiseSensitivity*/
  0x3C,   /*windNoiseHangOver*/
  0x4000, /*windPowerSmAlpha*/
  0x52,   /*lpEnergySmAlpha*/
  0x12A,  /*windNoiseEnergyOffset*/
  0x148,  /*windNoiseRatioSmAlpha*/
  {0, 0x00DC, 0, 0x01B8, 0, 0x00DC}, /*numCoefIirTdf2Lpf*/
  {0, 0xD072, 0, 0x12FE },           /*denCoefIirTdf2Lpf*/
  {0, 0x17DF, 0, 0xD0E3, 0,0x17DF},  /*numCoefIirTdf2Hpf*/
  {0, 0xD328, 0, 0x1162}             /*denCoefIirTdf2Hpf*/

};

const qdsp_cmd_crystal_speech_parms_type*
voc_cal_crystal_speech_default_parms[VOC_EC_DEFAULT] =
{
  /* VOC_EC_OFF              */ NULL
  /* VOC_EC_ESEC (handset)   */,&voc_cal_crystal_speech_parms_handset
  /* VOC_EC_HEADSET          */,NULL
  /* VOC_EC_AEC (handsfree)  */,NULL
  /* VOC_EC_SPEAKER          */,NULL
  /* VOC_EC_BT_HEADSET       */,NULL
};


/* ===== Crystal Speech parameters for wideband ===== */

CAL_MEMORY
qdsp_cmd_crystal_speech_parms_type voc_cal_crystal_speech_parms_wb_handset = {
  0xfdff, /* CSMode */
  0x0000, /* CS_TuningMode */
  0x0028, /* CS_echo_path_delay */
  0x2000, /* AF1_twoalpha */
  0x03e8, /* AF1_erl */
  0x0040, /* AF1_taps */
  0x0001, /* AF1_preset_coefs */
  0x0308, /* AF1_offset */
  0x2000, /* AF2_twoalpha */
  0x00fa, /* AF2_erl */
  0x0020, /* AF2_taps */
  0x0001, /* AF2_preset_coefs */
  0x0308, /* AF2_offset */
  0x028f, /* PCD_twoalpha */
  0x0308, /* PCD_offset */
  0x2400, /* CSPCD_threshold */
  0x4000, /* WgThreshold */
  0x199a, /* MpThreshold */
  {0x7fff, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000}, /* SF_init_table0[8] */
  {0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000}, /* SF_init_table1[8] */
  0x0010, /* SF_taps */
  0x028f, /* SF_twoalpha */
  0x6666, /* DNNS_EchoAlphaRev */
  0x747b, /* DNNS_EchoYcomp */
  0x2000, /* DNNS_WbThreshold */
  0x4000, /* DNNS_EchoGammaHi */
  0x1000, /* DNNS_EchoGammaLo */
  0x2800, /* DNNS_NoiseGammaS */
  0x2800, /* DNNS_NoiseGammaN */
  0x0600, /* DNNS_NoiseGainMinS */
  0x0400, /* DNNS_NoiseGainMinN */
  0x2666, /* DNNS_NoiseBiasComp */
  0x2fc4, /* DNNS_AcThreshold */
  0x0100, /* WB_echo_ratio_2mic */
  0x0800, /* WB_Gamma_E */
  0x0400, /* WB_Gamma_NN */
  0x2000, /* WB_Gamma_SN */
  0x0000, /* vcodec_delay0 */
  0x0003, /* vcodec_delay1 */
  0x000f, /* vcodec_len0 */
  0x000f, /* vcodec_len1 */
  0x6666, /* vcodec_thr0 */
  0x0500, /* vcodec_thr1 */
  0x2000, /* FixCalFactorLeft */
  0x2000, /* FixCalFactorRight */
  {0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000}, /*uint32 vcodec_coeff0[15]*/
  {0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000, 0x00000000,
   0x00000000, 0x00000000, 0x00000000}, /* uint32 vcodec_coeff1[15] */
  0x2000, /*CS_OutputGain*/
  0x3FFF, /*Enhance_MeuFilter1*/
  0x4189, /*Enhance_MeuFilter2*/
  0x4C10, /*Fix_OverEstimation*/
  0x7FFF, /*dmNLPPlimit*/
  0x800   /*dmNLPPgain*/
};

const qdsp_cmd_crystal_speech_parms_type*
voc_cal_crystal_speech_wb_default_parms[VOC_EC_DEFAULT] =
{
  /* VOC_EC_OFF              */ NULL
  /* VOC_EC_ESEC (handset)   */,&voc_cal_crystal_speech_parms_wb_handset
  /* VOC_EC_HEADSET          */,NULL
  /* VOC_EC_AEC (handsfree)  */,NULL
  /* VOC_EC_SPEAKER          */,NULL
  /* VOC_EC_BT_HEADSET       */,NULL
};

#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

/* Default values for HANDSET RVE DUAL MIC
*/
CAL_MEMORY qdsp_cmd_dual_rve_param_type voc_cal_dual_rve_params_handset =
{ /* RVE PARAMETERS */
    3276,					/* rx_rve_sm_alpha_noise */
    3276,					/* rx_rve_sm_alpha_ref */
    767,					/* rx_rve_sm_alpha_rec */
    6767,					/* rx_rve_sm_alpha_gain_increase */
    168,					/* rx_rve_sm_alpha_gain_decrease */
    2767,					/* rx_rve_sm_alpha_amp */
    17767,					/* rx_rve_sm_alpha_plattack */
    4767,					/* rx_rve_sm_alpha_pldecay */
    32,						/* rx_rve_low_gain_limit */
    160,					/* rx_rve_headroom */
    0,						/* rx_rve_dshift */
    0,						/* rx_rve_hf_dshift */
    8192,					/* rx_rve_scale_fact1 */
    8192,					/* rx_rve_scale_fact2 */
    32767,					/* rx_rve_peak_lim */
    32767,					/* rx_rve_hard_peak_lim */
    {0,0,0,0},				        /* rx_rve_noise_sens_thresh[4] */
    {4915,4915,4915,18022},	                /* rx_rve_gain_mult_vector[4] */
    32767,					/* rx_rve_upper_out_lim */
    2,						/* rx_rve_op_mode */
    200,					/* rx_rve_aeqVadThreshold */
    4						/* rx_rve_refVadHangoverMax */
};

/* Set of default values for RVE DUAL MIC
*/
const qdsp_cmd_dual_rve_param_type * voc_cal_dual_rve_default_params[VOC_EC_DEFAULT] =
{
  /* VOC_EC_OFF     */ NULL
  /* VOC_EC_ESEC    */,&voc_cal_dual_rve_params_handset
  /* VOC_EC_HEADSET */,NULL
  /* VOC_EC_AEC     */,NULL
  /* VOC_EC_SPEAKER */,NULL
  /* VOC_EC_BT_HEADSET */,NULL
};

/* Default configuration for on-chip codec Channel 0
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_0_cal = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume        */
  0x1000,                        /* Tx DTMF gain            */
  0x5A67,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,/*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x656F,                        /* CODEC Tx gain           */
  0x71CF,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x013E, 0x0138, 0x0270, 0x0298, 0x04F7, 0xFCBA, 0x2EDD},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
#if defined(CUST_EDITION) && !defined(FEATURE_OEMOMH)
//#ifdef FEATURE_VERSION_1110W516
#if defined(FEATURE_VERSION_K202)
  0xF56F,                        /* Tx Voice Volume         */
  0x3000,                        /* Tx DTMF gain            */
  0x9B00,                        /* CODEC Tx gain           */   //ÌýÍ²
  0x8000,                        /* CODEC Rx gain           */   //MIC
#else
#ifdef FEATURE_VERSION_K212
  0x956F,                        /* Tx Voice Volume          */
  0x3000,                        /* Tx DTMF gain            */
  0xCB00,                        /* CODEC Tx gain           */
  0x6000,                        /* CODEC Rx gain           */
#else
  0x956F,                        /* Tx Voice Volume          */
  0x2000,                        /* Tx DTMF gain            */
  0xCB00,                        /* CODEC Tx gain           */
  0x6000,                        /* CODEC Rx gain           */
#endif
#endif
                      			 /* CODEC Rx gain           */
//  #endif
#elif defined(FEATURE_VERSION_W317A)
  0x956F, 					     /* Tx Voice Volume		  */
  0x2000, 					     /* Tx DTMF gain			  */
  0xCB00, 					     /* CODEC Tx gain 		  */
  0x6000, 					     /* CODEC Rx gain 		  */
#elif defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
  0xa56F, 					   /* Tx Voice Volume		 */
  0x3000, 					   /* Tx DTMF gain		 */
  0xCB00, 					   /* CODEC Tx gain 		 */
  0x8000, 					   /* CODEC Rx gain 		 */             
#else
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xCB00,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
#endif
  0x2013,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0, 0, 0, 0, 0, 0, 0},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x8F47,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x7FB2,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x080E,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,/*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xF995F405,  0x01678FF7,  0x40000000,  0xF41270D5,  0x021153B4},
    { 0x0957D2C9,  0x29E1973B,  0x439A188C,  0xF9543F9E,  0x2D32171A},
    { 0xDEB504C3,  0x2D12441B,  0x439A188C,  0xE035B91A,  0x2E643A28},
    { 0x4C6C144A,  0x2F9BE92E,  0x439A188C,  0x4FDB2D3E,  0x2EB6BFFB},
    { 0x653E871A,  0x32F94FC8,  0x439A188C,  0x6AFC66EE,  0x35C21BFF}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xF995F405,  0x01678FF7,  0x40000000,  0xF41270D5,  0x021153B4},
    { 0x0957D2C9,  0x29E1973B,  0x439A188C,  0xF9543F9E,  0x2D32171A},
    { 0xDEB504C3,  0x2D12441B,  0x439A188C,  0xE035B91A,  0x2E643A28},
    { 0x4C6C144A,  0x2F9BE92E,  0x439A188C,  0x4FDB2D3E,  0x2EB6BFFB},
    { 0x653E871A,  0x32F94FC8,  0x439A188C,  0x6AFC66EE,  0x35C21BFF}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
   { 0xC7BD982A,  0x20F6C2EA,  0x40000000,  0xB8F814B6,  0x219A8827 },
   { 0x1932BD72,  0x257CB928,  0x3EE09449,  0x1B3E6D5C,  0x2305883C },
   { 0x94191681,  0x2EC8C2C7,  0x3EE09449,  0x91E3F7C1,  0x33338FF4 },
   { 0x3F1ADDB2,  0x3139A3B6,  0x3EE09449,  0x3E6C5F84,  0x30716FF7 },
   { 0xC7626980,  0x367571CC,  0x3EE09449,  0xC87A0CE9,  0x354E88A9 }
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x47CF,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,/*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x0008, 0x0016, 0xFF51, 0x00D9, 0x01DF, 0xEBA5, 0x6612},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#endif  /* MSMAUD_6500_FFA_AUDIO_CAL */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || T_FFA */
  }
#endif /* FEATURE_AUDIO_AGC */
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for on-chip codec Channel 0 for SURF
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_0_surf_cal = {
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x47CF,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,/*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x0008, 0x0016, 0xFF51, 0x00D9, 0x01DF, 0xEBA5, 0x6612},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || T_FFA */
  }
#endif /* FEATURE_AUDIO_AGC */
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for on-chip codec Channel 0 for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_0_cal_wb = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume        */
  0x1000,                        /* Tx DTMF gain            */
  0x5A67,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,/*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x656F,                        /* CODEC Tx gain           */
  0x71CF,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x013E, 0x0138, 0x0270, 0x0298, 0x04F7, 0xFCBA, 0x2EDD},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
#if defined(CUST_EDITION) && !defined(FEATURE_OEMOMH)
#ifdef FEATURE_VERSION_K212
  0x5000,						 /* Tx Voice Volume 		*/
  0x1000,						 /* Tx DTMF gain			*/
  0x5000,						 /* CODEC Tx gain			*/
  0x5000,                        /* CODEC Rx gain           */
#else
  0x5000,						 /* Tx Voice Volume 		*/
  0x1000,						 /* Tx DTMF gain			*/
  0x5000,						 /* CODEC Tx gain			*/
  0x5000,						 /* CODEC Rx gain			*/
  #endif
#else
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xCB00,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
#endif
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0xFFF5, 0x01D5, 0x0334, 0x044F, 0x06BA, 0x07A7, 0x4735},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x8F47,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  { /* Tx PCM IIR filter */
	QDSP_CMD_IIR_FILTER_ENABLE,
	5,
	{ 0xef03a727, 0x077f9d65, 0x40000000, 0x01106dd7, 0x0518910f },
	{ 0x24bc051f, 0x1357a0c2, 0x40000000, 0x2a4cd5e1, 0x14a227fe },
	{ 0xdc2eb5e0, 0x2936399d, 0x40000000, 0xdc476645, 0x29240ea0 },
	{ 0xb836edf2, 0x2aca26a1, 0x40000000, 0xb8d3ca35, 0x2a469331 },
	{ 0x496ced9d, 0x2c2af2dc, 0x40000000, 0x4a62707c, 0x2d042f8f }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  { /* Rx PCM IIR filter */
 	QDSP_CMD_IIR_FILTER_ENABLE,
	5,
	#ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
	{ 0xCDC1DDC2, 0x18EB2158, 0x40000000, 0xBF3E9E65, 0xFC564A0},
	{ 0xF27D7427, 0x1A188F47, 0x40000000, 0xF597B731, 0x2E9C434E},
	{ 0x326D06DE, 0x2CF6A326, 0x40000000, 0x30B54E3F, 0x2BFE5E9F},
	{ 0x9611817E, 0x2D9C0846, 0x40000000, 0x96F11BCE, 0x2F8E3BBE},
	{ 0xDCD53EBE, 0x330E6234, 0x40000000, 0xDC289C07, 0x3438A28F}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x7FB2,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x080E,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,/*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  { /* Tx PCM IIR filter */
	QDSP_CMD_IIR_FILTER_ENABLE,
	5,
	{ 0xef03a727, 0x077f9d65, 0x40000000, 0x01106dd7, 0x0518910f },
	{ 0x24bc051f, 0x1357a0c2, 0x40000000, 0x2a4cd5e1, 0x14a227fe },
	{ 0xdc2eb5e0, 0x2936399d, 0x40000000, 0xdc476645, 0x29240ea0 },
	{ 0xb836edf2, 0x2aca26a1, 0x40000000, 0xb8d3ca35, 0x2a469331 },
	{ 0x496ced9d, 0x2c2af2dc, 0x40000000, 0x4a62707c, 0x2d042f8f }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xF995F405,  0x01678FF7,  0x40000000,  0xF41270D5,  0x021153B4},
    { 0x0957D2C9,  0x29E1973B,  0x439A188C,  0xF9543F9E,  0x2D32171A},
    { 0xDEB504C3,  0x2D12441B,  0x439A188C,  0xE035B91A,  0x2E643A28},
    { 0x4C6C144A,  0x2F9BE92E,  0x439A188C,  0x4FDB2D3E,  0x2EB6BFFB},
    { 0x653E871A,  0x32F94FC8,  0x439A188C,  0x6AFC66EE,  0x35C21BFF}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  { /* Rx PCM IIR filter */
 	QDSP_CMD_IIR_FILTER_ENABLE,
	5,
	#ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
	{ 0xCDC1DDC2, 0x18EB2158, 0x40000000, 0xBF3E9E65, 0xFC564A0},
	{ 0xF27D7427, 0x1A188F47, 0x40000000, 0xF597B731, 0x2E9C434E},
	{ 0x326D06DE, 0x2CF6A326, 0x40000000, 0x30B54E3F, 0x2BFE5E9F},
	{ 0x9611817E, 0x2D9C0846, 0x40000000, 0x96F11BCE, 0x2F8E3BBE},
	{ 0xDCD53EBE, 0x330E6234, 0x40000000, 0xDC289C07, 0x3438A28F}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x47CF,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,/*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x0008, 0x0016, 0xFF51, 0x00D9, 0x01DF, 0xEBA5, 0x6612},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#endif  /* MSMAUD_6500_FFA_AUDIO_CAL */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || T_FFA */
  }
#endif /* FEATURE_AUDIO_AGC */
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for on-chip codec Channel 0 for SURF for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_0_surf_cal_wb = {
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x47CF,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,/*Pre EC Tx PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x0008, 0x0016, 0xFF51, 0x00D9, 0x01DF, 0xEBA5, 0x6612},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || T_FFA */
  }
#endif /* FEATURE_AUDIO_AGC */
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};


/* Default configuration for off-chip codec Channel 0
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_off_chip_dual_mic_cal = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume        */
  0x1000,                        /* Tx DTMF gain            */
  0x5A67,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x656F,                        /* CODEC Tx gain           */
  0x71CF,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x013E, 0x0138, 0x0270, 0x0298, 0x04F7, 0xFCBA, 0x2EDD},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x1FD9,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,     /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0xFFF5, 0x01D5, 0x0334, 0x044F, 0x06BA, 0x07A7, 0x4735},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x8F47,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x2862,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x47CF,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x0008, 0x0016, 0xFF51, 0x00D9, 0x01DF, 0xEBA5, 0x6612},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#endif  /* MSMAUD_6500_FFA_AUDIO_CAL */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || T_FFA */
  }
#endif /* FEATURE_AUDIO_AGC */
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for off-chip codec Channel 0 for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_off_chip_dual_mic_cal_wb = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume        */
  0x1000,                        /* Tx DTMF gain            */
  0x5A67,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x656F,                        /* CODEC Tx gain           */
  0x71CF,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x013E, 0x0138, 0x0270, 0x0298, 0x04F7, 0xFCBA, 0x2EDD},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x1FD9,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,     /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0xFFF5, 0x01D5, 0x0334, 0x044F, 0x06BA, 0x07A7, 0x4735},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x8F47,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x2862,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x47CF,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x0008, 0x0016, 0xFF51, 0x00D9, 0x01DF, 0xEBA5, 0x6612},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#endif  /* MSMAUD_6500_FFA_AUDIO_CAL */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || T_FFA */
  }
#endif /* FEATURE_AUDIO_AGC */
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/* Default configuration for on-chip codec Channel 0 dual mic
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_0_dual_mic_cal = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume        */
  0x1000,                        /* Tx DTMF gain            */
  0x5A67,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x656F,                        /* CODEC Tx gain           */
  0x71CF,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x013E, 0x0138, 0x0270, 0x0298, 0x04F7, 0xFCBA, 0x2EDD},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x1FD9,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,     /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0xFFF5, 0x01D5, 0x0334, 0x044F, 0x06BA, 0x07A7, 0x4735},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x8F47,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x2862,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x47CF,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x0008, 0x0016, 0xFF51, 0x00D9, 0x01DF, 0xEBA5, 0x6612},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#endif  /* MSMAUD_6500_FFA_AUDIO_CAL */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || T_FFA */
  }
#endif /* FEATURE_AUDIO_AGC */
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for on-chip codec Channel 0 dual mic for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_0_dual_mic_cal_wb = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume        */
  0x1000,                        /* Tx DTMF gain            */
  0x5A67,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x656F,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x656F,                        /* CODEC Tx gain           */
  0x71CF,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x013E, 0x0138, 0x0270, 0x0298, 0x04F7, 0xFCBA, 0x2EDD},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x1FD9,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x000,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,     /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0xFFF5, 0x01D5, 0x0334, 0x044F, 0x06BA, 0x07A7, 0x4735},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x8F47,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
	{ 0xef03a727, 0x077f9d65, 0x40000000, 0x01106dd7, 0x0518910f },
	{ 0x24bc051f, 0x1357a0c2, 0x40000000, 0x2a4cd5e1, 0x14a227fe },
	{ 0xdc2eb5e0, 0x2936399d, 0x40000000, 0xdc476645, 0x29240ea0 },
	{ 0xb836edf2, 0x2aca26a1, 0x40000000, 0xb8d3ca35, 0x2a469331 },
	{ 0x496ced9d, 0x2c2af2dc, 0x40000000, 0x4a62707c, 0x2d042f8f }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
	{ 0xCDC1DDC2, 0x18EB2158, 0x40000000, 0xBF3E9E65, 0xFC564A0},
	{ 0xF27D7427, 0x1A188F47, 0x40000000, 0xF597B731, 0x2E9C434E},
	{ 0x326D06DE, 0x2CF6A326, 0x40000000, 0x30B54E3F, 0x2BFE5E9F},
	{ 0x9611817E, 0x2D9C0846, 0x40000000, 0x96F11BCE, 0x2F8E3BBE},
	{ 0xDCD53EBE, 0x330E6234, 0x40000000, 0xDC289C07, 0x3438A28F}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x2862,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
	{ 0xef03a727, 0x077f9d65, 0x40000000, 0x01106dd7, 0x0518910f },
	{ 0x24bc051f, 0x1357a0c2, 0x40000000, 0x2a4cd5e1, 0x14a227fe },
	{ 0xdc2eb5e0, 0x2936399d, 0x40000000, 0xdc476645, 0x29240ea0 },
	{ 0xb836edf2, 0x2aca26a1, 0x40000000, 0xb8d3ca35, 0x2a469331 },
	{ 0x496ced9d, 0x2c2af2dc, 0x40000000, 0x4a62707c, 0x2d042f8f }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
	{ 0xCDC1DDC2, 0x18EB2158, 0x40000000, 0xBF3E9E65, 0xFC564A0},
	{ 0xF27D7427, 0x1A188F47, 0x40000000, 0xF597B731, 0x2E9C434E},
	{ 0x326D06DE, 0x2CF6A326, 0x40000000, 0x30B54E3F, 0x2BFE5E9F},
	{ 0x9611817E, 0x2D9C0846, 0x40000000, 0x96F11BCE, 0x2F8E3BBE},
	{ 0xDCD53EBE, 0x330E6234, 0x40000000, 0xDC289C07, 0x3438A28F}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x47CF,                        /* CODEC Tx Gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_BEFORE_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                 /* Rx PCM filter           */
  {0x0008, 0x0016, 0xFF51, 0x00D9, 0x01DF, 0xEBA5, 0x6612},
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xA5FABD82, 0x1E6482E5, 0x40000000, 0x9DE72588, 0x26E5A63B},
    { 0xAE75EF20, 0x23A99743, 0x3FF38D63, 0xAEB16F37, 0x2069BB32},
    { 0xDC5EFFFC, 0x29095F56, 0x3FF38D63, 0xD9D51A98, 0x2AC08D95},
    { 0x0111688E, 0x2B858345, 0x3FF38D63, 0x02D17EAF, 0x24802F67},
    { 0x2510F6DA, 0x2FDA0E4F, 0x3FF38D63, 0x2666B000, 0x306D1363}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0xDC6077C4,  0xFE561A4A, 0x40000000,  0xF5287D7A,  0xD247655A},
    { 0x4694EAA5,  0x1465059A, 0x416AC95A,  0x0A9EDA33,  0x095B1589},
    { 0xAB7680F9,  0x251ECE97, 0x416AC95A,  0xA94A9866,  0x34100E74},
    { 0xD6CC0B77,  0x2F4D1380, 0x416AC95A,  0xF85DE5E3,  0x1E04F34C},
    { 0x4E749379,  0x2F946D53, 0x416AC95A,  0x4BE3D224,  0x2E129412}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#endif  /* MSMAUD_6500_FFA_AUDIO_CAL */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || T_FFA */
  }
#endif /* FEATURE_AUDIO_AGC */
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

/* Default configuration for on-chip codec Channel 1
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_1_cal  = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x71CF,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x7FB2,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0xA0C3,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xCA63,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0,0,0,0,0,0,0},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)	
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
#if defined(CUST_EDITION) && !defined(FEATURE_OEMOMH)
#ifdef FEATURE_VERSION_K212
  0x5000,						 /* Tx Voice Volume 		*/
  0x1000,						 /* Tx DTMF gain			*/
  0x5000,						 /* CODEC Tx gain			*/
  0x5000,                        /* CODEC Rx gain           */
#else
  0x5000,						 /* Tx Voice Volume 		*/
  0x1000,						 /* Tx DTMF gain			*/
  0x5000,						 /* CODEC Tx gain			*/
  0x5000,						 /* CODEC Rx gain			*/
  #endif
#else
  0x5A67,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x3F8B,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
#endif
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xA0C3,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xA0C3,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
    VOC_EC_HEADSET,                /* Echo Canceller mode     */
    VOC_NS_ON,                     /* Noise suppressor enable */
    0x4000,                        /* Tx Voice Volume         */
    0x1000,                        /* Tx DTMF gain            */
    0x47CF,                        /* CODEC Tx gain           */
    0x4000,                        /* CODEC Rx gain           */
    0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#endif  /* (MSMAUD_6500_FFA_AUDIO_CAL) */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
#ifdef FEATURE_SURF_5200
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_SURF_5200 */
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_stereo_headset_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_stereo_headset_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_stereo_headset_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_ENABLED
};

/* Default configuration for on-chip codec Channel 1 for SURF
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_1_surf_cal  = {
    VOC_EC_HEADSET,                /* Echo Canceller mode     */
    VOC_NS_ON,                     /* Noise suppressor enable */
    0x4000,                        /* Tx Voice Volume         */
    0x1000,                        /* Tx DTMF gain            */
    0x47CF,                        /* CODEC Tx gain           */
    0x4000,                        /* CODEC Rx gain           */
    0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_stereo_headset_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_stereo_headset_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_stereo_headset_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_ENABLED
};

/* Default configuration for on-chip codec Channel 1 for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_1_cal_wb  = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x71CF,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x7FB2,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,   /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0xA0C3,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xCA63,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0,0,0,0,0,0,0},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
#if defined(CUST_EDITION) && !defined(FEATURE_OEMOMH)
#ifdef FEATURE_VERSION_K212
  0x5000,						 /* Tx Voice Volume 		*/
  0x1000,						 /* Tx DTMF gain			*/
  0x5000,						 /* CODEC Tx gain			*/
  0x5000,                        /* CODEC Rx gain           */
#else
  0x5000,						 /* Tx Voice Volume 		*/
  0x1000,						 /* Tx DTMF gain			*/
  0x5000,						 /* CODEC Tx gain			*/
  0x5000,						 /* CODEC Rx gain			*/
  #endif
#else
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x2CFC,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
#endif
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0x00D7, 0x02E7, 0xFDEF, 0x074D, 0x0492, 0xeec0, 0x4569},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0x00D7, 0x02E7, 0xFDEF, 0x074D, 0x0492, 0xeec0, 0x4569},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xA0C3,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset (xx.xx)   */
#elif defined(MSMAUD_6800_SURF_AUDIO_CAL)
  VOC_EC_HEADSET,                /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xA0C3,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
    VOC_EC_HEADSET,                /* Echo Canceller mode     */
    VOC_NS_ON,                     /* Noise suppressor enable */
    0x4000,                        /* Tx Voice Volume         */
    0x1000,                        /* Tx DTMF gain            */
    0x47CF,                        /* CODEC Tx gain           */
    0x4000,                        /* CODEC Rx gain           */
    0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#endif  /* (MSMAUD_6500_FFA_AUDIO_CAL) */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
#ifdef FEATURE_SURF_5200
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_SURF_5200 */
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_stereo_headset_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_stereo_headset_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_stereo_headset_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_ENABLED
};

/* Default configuration for on-chip codec Channel 1 for SURF for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_1_surf_cal_wb  = {
    VOC_EC_HEADSET,                /* Echo Canceller mode     */
    VOC_NS_ON,                     /* Noise suppressor enable */
    0x4000,                        /* Tx Voice Volume         */
    0x1000,                        /* Tx DTMF gain            */
    0x47CF,                        /* CODEC Tx gain           */
    0x4000,                        /* CODEC Rx gain           */
    0x0400,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0xFB6011EE,  0xD3B3BCAE, 0x40000000,  0x205C0FE6,  0xF10ADFF7},
    { 0xBFCFD696,  0x144FAFC0, 0x3C8F3B76,  0xA2C8031B,  0x24E6C265},
    { 0x2947B99B,  0x29B46090, 0x3C8F3B76,  0x22E3502B,  0x240945B9},
    { 0x4B2A4C0A,  0x2A8DB621, 0x3C8F3B76,  0x3A64923C,  0x1E9394A1},
    { 0xF862BD95,  0x2B35D6E2, 0x3C8F3B76,  0xEA9D2D06,  0x2577CDDC}
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
     #ifdef FEATURE_K_AMPLIFIER
	{ 0x4a06ad89,0x1a14bed7,0x40000000,0x8c9f5423,0x33d6015e},
	{ 0xf203f5f1,0x257bb24c,0x409c3adb,0x47818c42,0x183a6240},
	{ 0xd10a6e41,0x26d9fea0,0x409c3adb,0xf9744966,0x22dcd584},
	{ 0x4cb424ad,0x31a699e3,0x409c3adb,0x504592eb,0x33daa6a3},
	{ 0x8f4e33b1,0x31ec2df7,0x409c3adb,0xcc81fd16,0x25e84026}
	#else
    { 0x4A06AD89,  0x1A14BED7, 0x40000000,  0x8C9F5423,  0x33D6015E},
    { 0xF203F5F1,  0x257BB24C, 0x409C3ADB,  0x47818C42,  0x183A6240},
    { 0xD10A6E41,  0x26D9FEA0, 0x409C3ADB,  0xF9744966,  0x22DCD584},
    { 0x4CB424AD,  0x31A699E3, 0x409C3ADB,  0x504592EB,  0x33DAA6A3},
    { 0x8F4E33B1,  0x31EC2DF7, 0x409C3ADB,  0xCC81FD16,  0x25E84026}
	#endif
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
#ifdef FEATURE_SURF_5200
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_SURF_5200 */
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_stereo_headset_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_stereo_headset_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_stereo_headset_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_ENABLED
};

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

/* Default configuration for on-chip codec Aux. Channel
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_aux_cal = {
  VOC_EC_AEC,                    /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,           /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
#if defined(FEATURE_FFA) || defined(T_FFA)
#error code not present
#else
  0x4000,                        /* CODEC Tx gain           */
#endif
  0x4000,                        /* CODEC Rx gain           */
  0,                             /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/

  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
#ifdef FEATURE_SURF_5200
#error code not present
#else
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_SURF_5200 */
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

#ifdef FEATURE_SPEAKER_PHONE
/* Default configuration for on-chip codec aux ear, mic1
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_speaker_cal = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
#ifdef FEATURE_AVS_NEXTGEN_EC
  VOC_EC_SPEAKER,                /* Echo Canceller mode     */
#else
  VOC_EC_AEC,                    /* Echo Canceller mode     */
#endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0xA0C3,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x71CF,                          /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  #ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
  #else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
  #endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                       /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,           /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xCA63,                          /* CODEC Tx gain           */
  0x5A67,                          /* CODEC Rx gain           */
  0x0000,                          /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                   /* Tx PCM filter           */
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                   /* Rx PCM filter           */
  {0x000B, 0xFF61, 0xFEFD, 0xFF68, 0x024D, 0x08F9, 0x2D33},
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
  #endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
  #ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
  #else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
  #endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                       /* Noise suppressor enable */
#if defined(CUST_EDITION) && !defined(FEATURE_OEMOMH)
#if defined (FEATURE_VERSION_W516) || defined (FEATURE_VERSION_W208S) || defined(FEATURE_VERSION_W027)
  0x9000,						   /* Tx Voice Volume 		*/
  0x1000,						   /* Tx DTMF gain			*/
  0x5000,						   /* CODEC Tx gain			*/
  0x5000,						   /* CODEC Rx gain			*/
#elif defined(FEATURE_VERSION_K202)
  0xF000,						   /* Tx Voice Volume 		*/
  0x1000,						   /* Tx DTMF gain			*/
  0x5000,						   /* CODEC Tx gain			*/
  0x5000,						   /* CODEC Rx gain			*/
#else
  0x5000,						   /* Tx Voice Volume 		*/
  0x1000,						   /* Tx DTMF gain			*/
  0x5000,						   /* CODEC Tx gain			*/
  0x5000,						   /* CODEC Rx gain			*/
#endif
#elif defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
  0x7000, 						   /* Tx Voice Volume 	  */
  0x1000, 						   /* Tx DTMF gain		  */
  0x5000, 						   /* CODEC Tx gain		  */
#ifdef  FEATURE_VERSION_C260_IC18
  0xFFFF, 						   /* CODEC Rx gain		  */  
#else
  0x5000, 						   /* CODEC Rx gain		  */
#endif  
#else
  0x4000,                          /* Tx Voice Volume         */
  0x1000,                          /* Tx DTMF gain            */
  0x3F8B,                          /* CODEC Tx gain           */
  0x4000,                          /* CODEC Rx gain           */
#endif
  0x0000,                          /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                   /* Tx PCM filter           */
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                   /* Rx PCM filter           */
  {0, 0, 0, 0, 0, 0, 0},
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
  #endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  #ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
  #else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
  #endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x7FB2,                        /* CODEC Tx gain           */
  0xA0C3,                        /* CODEC Rx gain           */
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
  #endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
  #ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
#else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
  #endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                       /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,             /* Tx Voice Volume         */
  0x1000,                          /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0,                              /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#endif  /* MSMAUD_6500_FFA_AUDIO_CAL */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
#ifdef T_MSM5200
#error code not present
#else
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* T_MSM5200 */
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_spk_phone_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_spk_phone_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_spk_phone_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for on-chip codec aux ear, mic1 for SURF
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_speaker_surf_cal = {
#ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
#else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
#endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                       /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,             /* Tx Voice Volume         */
  0x1000,                          /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0,                              /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_spk_phone_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_spk_phone_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_spk_phone_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for on-chip codec aux ear, mic1 for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_speaker_cal_wb = {
#if defined(MSMAUD_6500_FFA_AUDIO_CAL)
#ifdef FEATURE_AVS_NEXTGEN_EC
  VOC_EC_SPEAKER,                /* Echo Canceller mode     */
#else
  VOC_EC_AEC,                    /* Echo Canceller mode     */
#endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0xA0C3,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x71CF,                          /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#elif defined(MSMAUD_QSC60X0_AUDIO_CAL)
  #ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
  #else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
  #endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                       /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,           /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0xCA63,                          /* CODEC Tx gain           */
  0x5A67,                          /* CODEC Rx gain           */
  0x0000,                          /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                   /* Tx PCM filter           */
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                   /* Rx PCM filter           */
  {0x000B, 0xFF61, 0xFEFD, 0xFF68, 0x024D, 0x08F9, 0x2D33},
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
  #endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#elif defined(MSMAUD_QSC60X5_AUDIO_CAL)  || \
      defined(MSMAUD_QSC1110_AUDIO_CAL)
  #ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
  #else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
  #endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                       /* Noise suppressor enable */
#if defined(CUST_EDITION) && !defined(FEATURE_OEMOMH)
#ifdef FEATURE_VERSION_K212
  0x5000,						   /* Tx Voice Volume 		*/
  0x1000,						   /* Tx DTMF gain			*/
  0x5000,						   /* CODEC Tx gain			*/
  0x5000,                          /* CODEC Rx gain           */
#else
  0x5000,						   /* Tx Voice Volume 		*/
  0x1000,						   /* Tx DTMF gain			*/
  0x5000,						   /* CODEC Tx gain			*/
  0x5000,						   /* CODEC Rx gain			*/
  #endif
#else
  0x4000,                          /* Tx Voice Volume         */
  0x1000,                          /* Tx DTMF gain            */
  0x4000,                          /* CODEC Tx gain           */
  0x4000,                          /* CODEC Rx gain           */
#endif
  0x0000,                          /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
                                   /* Tx PCM filter           */
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
                                   /* Rx PCM filter           */
  {0x0569, 0xFC6C, 0xFCFC, 0xFBE1, 0xF856, 0xF3A5, 0x321C},
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
  #endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#elif defined(MSMAUD_6800_FFA_AUDIO_CAL)
  #ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
  #else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
  #endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x7FB2,                        /* CODEC Tx gain           */
  0xA0C3,                        /* CODEC Rx gain           */
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xFFF3, 0x001D, 0xFFB9, 0x016B, 0xFA71, 0x0C08, 0x309A},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
  #ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
  #endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset (xx.xx)   */
#else /* Default */
  #ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
#else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
  #endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                       /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,             /* Tx Voice Volume         */
  0x1000,                          /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0,                              /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#endif  /* MSMAUD_6500_FFA_AUDIO_CAL */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
#ifdef T_MSM5200
#error code not present
#else
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* T_MSM5200 */
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_spk_phone_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_spk_phone_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_spk_phone_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for on-chip codec aux ear, mic1 for SURF for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_chip_speaker_surf_cal_wb = {
#ifdef FEATURE_AVS_NEXTGEN_EC
    VOC_EC_SPEAKER,                /* Echo Canceller mode     */
#else
    VOC_EC_AEC,                    /* Echo Canceller mode     */
#endif /* FEATURE_AVS_NEXTGEN_EC */
  VOC_NS_ON,                       /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,             /* Tx Voice Volume         */
  0x1000,                          /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0,                              /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
    0                              /* Rx dBm offset (xx.xx)   */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_spk_phone_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_spk_phone_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_spk_phone_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

#endif /* FEATURE_SPEAKER_PHONE */

/* Default configuration for off-chip, on-board codec
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_on_board_cal = {
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0,                             /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3,0x001d,0xffb9,0x016b,0xfa71,0x0c08,0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

#ifdef FEATURE_BT
/* Default configuration for BT Local Audio and Intercom codec
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_bt_intercom_cal = {
  VOC_EC_BT_HEADSET,             /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,           /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x2000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0,                             /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

#ifdef FEATURE_BT_AG
/* Default configuration for BT codec
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_bt_ag_cal = {
  VOC_EC_BT_HEADSET,             /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,           /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0,                             /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for BT codec for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_bt_ag_cal_wb = {
  VOC_EC_BT_HEADSET,             /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,           /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0,                             /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};
/* FM over BT AG */
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_fm_bt_ag_cal = {
  VOC_EC_BT_HEADSET,             /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  VOC_CAL_TX_GAIN_NOM,           /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0,                             /* CODEC Rx gain           */
  0,                             /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

#endif /* FEATURE_BT_AG */
#endif /* FEATURE_BT */

/* Default configuration for off-chip, off-board codec
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_off_board_cal = {
  VOC_EC_BT_HEADSET,             /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4d00,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0,                             /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0x0600, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
/* Default configuration for external stereo codec.
** None of these values are used except the Codec configuration.
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_sdac_cal = {
  VOC_EC_ESEC,                   /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
  0x1800,                        /* Tx Voice Volume         */
#endif
  0x1000,                        /* Tx DTMF gain            */
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
  0x4000,                        /* CODEC Tx gain           */
#endif
  0x4000,                        /* CODEC Rx gain           */
  0x400,                         /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
#ifdef FEATURE_INTERNAL_SDAC
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#else
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* FEATURE_INTERNAL_SDAC */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
#endif
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{                             /* AGC/AVC parameters      */
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_DESK
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */

#ifdef FEATURE_TTY
/* Default configuration for TTY on-chip codec Channel 1
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_tty_on_chip_1_cal  = {
  VOC_EC_OFF,                    /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x2000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
    defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
    defined(MSMAUD_QSC1110_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)|| \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x800,                         /* CODEC Tx gain           */
#else
  0x4000,                        /* CODEC Tx gain           */
#endif /* #if defined(MSMAUD_QSC6270_AUDIO_CAL) || 
              defined(MSMAUD_QSC60X5_AUDIO_CAL)
              defined(MSMAUD_QSC1110_AUDIO_CAL) */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x1013,                        /* CODEC Rx gain           */
#else
  0x4000,                        /* CODEC Rx gain           */
#endif
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for TTY off-chip, off-board codec
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_tty_off_board_cal = {
  VOC_EC_OFF,                    /* Echo Canceller mode     */
  VOC_NS_OFF,                    /* Noise suppressor enable */
  0x2000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
  0x4000,                        /* CODEC Rx gain           */
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0x0600, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for VCO TTY on internal codec Channel 1
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_tty_vco  = {
  VOC_EC_OFF,                    /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
  0x1800,                        /* Tx Voice Volume         */
#endif
  0x1000,                        /* Tx DTMF gain            */
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
  0x4000,                        /* CODEC Tx gain           */
#endif
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x1013,                        /* CODEC Rx gain           */
#else
  0x4000,                        /* CODEC Rx gain           */
#endif
  0,                             /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
#endif
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};


/* Default configuration for HCO TTY on internal codec Channel 1
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_tty_hco  = {
  VOC_EC_OFF,                    /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x2000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
    defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
    defined(MSMAUD_QSC1110_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x800,                         /* CODEC Tx gain           */
#else
  0x4000,                        /* CODEC Tx gain           */
#endif /* #if defined(MSMAUD_QSC6270_AUDIO_CAL) ||
              defined(MSMAUD_QSC60X5_AUDIO_CAL) 
              defined(MSMAUD_QSC1110_AUDIO_CAL) */
  0x4000,                        /* CODEC Rx gain           */
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
 QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
#if defined(FEATURE_FFA) || defined(FEATURE_TTP) || defined(T_FFA)
#error code not present
#else
#if defined(T_MSM5105)
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xf800, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
#else
#error code not present
#endif /* defined(T_MSM5105) */
#endif /* FEATURE_FFA || FEATURE_TTP || T_FFA */
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for TTY on-chip codec Channel 1
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_tty_on_chip_1_gsm_cal  = {
  VOC_EC_OFF,                    /* Echo Canceller mode     */
  VOC_NS_OFF,                    /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
    defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
    defined(MSMAUD_QSC1110_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x800,                         /* CODEC Tx gain           */
#else
  0x2000,                        /* CODEC Tx gain           */
#endif /* MSMAUD_QSC6270_AUDIO_CAL || MSMAUD_QSC60X5_AUDIO_CAL ||
          MSMAUD_QSC1110_AUDIO_CAL */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x1013,                        /* CODEC Rx gain           */
#else
  0x4000,                        /* CODEC Rx gain           */
#endif
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0xF100, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for VCO TTY on internal codec Channel 1
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_tty_vco_gsm_cal  = {
  VOC_EC_OFF,                    /* Echo Canceller mode     */
  VOC_NS_OFF,                    /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x2000,                        /* CODEC Tx gain           */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x1013,                        /* CODEC Rx gain           */
#else
  0x4000,                        /* CODEC Rx gain           */
#endif
  0,                             /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC,  /*Pre EC PCM Filter*/
                                 /* Tx PCM filter           */
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0xfff3, 0x001d, 0xffb9, 0x016b, 0xfa71, 0x0c08, 0x309a},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};


/* Default configuration for HCO TTY on internal codec Channel 1
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_tty_hco_gsm_cal  = {
  VOC_EC_OFF,                    /* Echo Canceller mode     */
  VOC_NS_OFF,                    /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
    defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
    defined(MSMAUD_QSC1110_AUDIO_CAL) || \
	defined(MSMAUD_SCMM_FFA_AUDIO_CAL)    || \
	defined(MSMAUD_SCMM_SURF_AUDIO_CAL) 
  0x800,                         /* CODEC Tx gain           */
#else
  0x2000,                        /* CODEC Tx gain           */
#endif /* MSMAUD_QSC6270_AUDIO_CAL || MSMAUD_QSC60X5_AUDIO_CAL ||
          MSMAUD_QSC1110_AUDIO_CAL */
  0x4000,                        /* CODEC Rx gain           */
  0x0000,                        /* CODEC sidetone gain     */
#ifdef MSMAUD_PCM_DC_OFFSET
  0xFF18,                        /* CODEC Rx left chan offset */
  0xFEEA,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

#endif /* FEATURE_TTY */

#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
/* Default configuration for USB HFK
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_usb_hfk_cal = {
  VOC_EC_AEC,                    /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
#ifdef MSMAUD_6280_FFA_AUDIO_CAL
#error code not present
#else
  0x4000,                        /* CODEC Rx gain           */
#endif  /* MSMAUD_6280_FFA_AUDIO_CAL */
#ifdef MSMAUD_6280_FFA_AUDIO_CAL
#error code not present
#else
  0x0000,                        /* CODEC sidetone gain     */
#endif  /* MSMAUD_6280_FFA_AUDIO_CAL */
#ifdef MSMAUD_PCM_DC_OFFSET
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
#ifdef MSMAUD_6280_FFA_AUDIO_CAL
#error code not present
#else
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
#endif  /* MSMAUD_6280_FFA_AUDIO_CAL */
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0x0600, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_stereo_usb_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_stereo_usb_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_stereo_usb_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};

/* Default configuration for USB HFK for wb case
*/
CAL_MEMORY voc_pcm_path_cal_type voc_pcm_usb_hfk_cal_wb = {
  VOC_EC_AEC,                    /* Echo Canceller mode     */
  VOC_NS_ON,                     /* Noise suppressor enable */
  0x4000,                        /* Tx Voice Volume         */
  0x1000,                        /* Tx DTMF gain            */
  0x4000,                        /* CODEC Tx gain           */
#ifdef MSMAUD_6280_FFA_AUDIO_CAL
#error code not present
#else
  0x4000,                        /* CODEC Rx gain           */
#endif  /* MSMAUD_6280_FFA_AUDIO_CAL */
#ifdef MSMAUD_6280_FFA_AUDIO_CAL
#error code not present
#else
  0x0000,                        /* CODEC sidetone gain     */
#endif  /* MSMAUD_6280_FFA_AUDIO_CAL */
#ifdef MSMAUD_PCM_DC_OFFSET
  0x0000,                        /* CODEC Rx left chan offset */
  0x0000,                        /* CODEC Rx right chan offset */
#endif  /* MSMAUD_PCM_DC_OFFSET */
  QDSP_CMD_TX_PCM_FILTER_AFTER_EC, /*Pre EC PCM Filter*/
#ifdef MSMAUD_6280_FFA_AUDIO_CAL
#error code not present
#else
  {0,0,0,0,0,0,0},               /* Tx PCM filter           */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
                                 /* Tx SecondaryMic PCM IIR filter*/
  {0,0,0,0,0,0,0},
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
#endif  /* MSMAUD_6280_FFA_AUDIO_CAL */
  {0,0,0,0,0,0,0},               /* Rx PCM filter           */
#ifdef FEATURE_IIR_FILTER
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    3,
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 }
  },
#ifdef FEATURE_IIR_FILTER_5S
  {                              /* Tx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  {                              /* Tx SecondaryMic PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_DISABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif  /*FEATURE_AVS_CRYSTAL_SPEECH*/
  {                              /* Rx PCM IIR filter       */
    QDSP_CMD_IIR_FILTER_ENABLE,
    5,
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 },
    { 0,0,0x40000000,0,0 }
  },
#endif /* FEATURE_IIR_FILTER_5S */
#endif /* FEATURE_IIR_FILTER */
  0                              /* Rx dBm offset           */
#ifdef FEATURE_AUDIO_AGC
  ,{
    0, 0xffff, 0x0780, 0xff9a, 0x1b0c, 0xf333,
    0x0600, 0,
    0x7f65, 0, 0x0780, 0xff9a, 0x1b0c, 0xf333
  }
#endif
  ,voc_cal_rve_default_params
  ,voc_cal_dual_rve_default_params
  ,voc_cal_ec_default_params
#ifdef FEATURE_AVS_NEXTGEN_EC
  ,voc_cal_nextgen_ec_default_parms
  ,voc_cal_nextgen_ec_wb_default_parms
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  ,voc_cal_crystal_speech_default_parms
  ,voc_cal_crystal_speech_wb_default_parms
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  ,voc_cal_qsynth_stereo_usb_adrc_ctl
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  ,voc_cal_qtunes_stereo_usb_adrc_ctl
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  ,voc_cal_sidechain_filter_ctl
#endif
#ifdef FEATURE_AUDFMT_IIR_FILTER
  ,voc_cal_audfmt_stereo_usb_iir_filter_ctl
#endif
#ifdef FEATURE_QAUDIOFX
  ,QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
  ,voc_cal_qsynth_pcm_filter
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  ,voc_cal_qtunes_pcm_filter
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  ,VOC_HSSD_DISABLED
};
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

/*  Array of pointers to configuration values for each PCM Path
*/
const voc_pcm_path_cal_type
                  *voc_pcm_cal_aptr[VOC_CAL_PCM_PATH_CAL_MAX][VOC_CODEC_MAX] =
{
  {
    /* VOC_CODEC_ON_CHIP_0        */ &voc_pcm_on_chip_0_cal
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    /* VOC_CODEC_ON_CHIP_0_DUAL_MIC */,&voc_pcm_on_chip_0_dual_mic_cal
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */
    /* VOC_CODEC_ON_CHIP_1        */,&voc_pcm_on_chip_1_cal
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_STEREO_HEADSET   */,&voc_pcm_on_chip_1_cal
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
    /* VOC_CODEC_ON_CHIP_AUX      */,&voc_pcm_on_chip_aux_cal
    /* VOC_CODEC_ON_BOARD         */,&voc_pcm_on_board_cal
#ifdef FEATURE_SPEAKER_PHONE
    /* VOC_CODEC_SPEAKER          */,&voc_pcm_on_chip_speaker_cal
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_BT
    /* VOC_CODEC_BT_INTERCOM      */,&voc_pcm_bt_intercom_cal
#ifdef FEATURE_BT_AG
    /* VOC_CODEC_BT_AG            */,&voc_pcm_bt_ag_cal
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* VOC_CODEC_BT_A2DP          */,&voc_pcm_bt_ag_cal
    /* VOC_CODEC_BT_A2DP_SCO      */,&voc_pcm_bt_ag_cal
#endif /* FEATURE_SBC_CODEC */
#endif /* FEATURE_BT */
    /* VOC_CODEC_OFF_BOARD        */,&voc_pcm_off_board_cal
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_SDAC             */,&voc_pcm_sdac_cal
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
#if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
    /* VOC_CODEC_IN_MONO_SADC_OUT_MONO_HANDSET     */  ,&voc_pcm_on_chip_0_cal
    /* VOC_CODEC_IN_MONO_SADC_OUT_STEREO_HEADSET   */  ,&voc_pcm_on_chip_1_cal
#if defined(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)
    /* VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET   */ ,&voc_pcm_off_chip_dual_mic_cal
#else
                                                      ,&voc_pcm_on_chip_0_cal
#endif /*(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)*/
    /* VOC_CODEC_IN_STEREO_SADC_OUT_STEREO_HEADSET */  ,&voc_pcm_on_chip_1_cal
#endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */
#ifdef FEATURE_TTY
    /* VOC_CODEC_TTY_ON_CHIP_1    */,&voc_pcm_tty_on_chip_1_cal
    /* VOC_CODEC_TTY_OFF_BOARD    */,&voc_pcm_tty_off_board_cal
    /* VOC_CODEC_TTY_VCO          */,&voc_pcm_tty_vco
    /* VOC_CODEC_TTY_HCO          */,&voc_pcm_tty_hco
#endif /* FEATURE_TTY */
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    /* VOC_CODEC_USB              */,&voc_pcm_usb_hfk_cal
#ifdef FEATURE_INTERNAL_SDAC
    /* VOC_CODEC_STEREO_USB       */,&voc_pcm_usb_hfk_cal
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
  /* FM over BT AG */
#ifdef FEATURE_BT_AG
                                    ,&voc_pcm_fm_bt_ag_cal
#endif
    /* VOC_CODEC_ON_CHIP_0_SURF  */ ,&voc_pcm_on_chip_0_surf_cal
    /* VOC_CODEC_ON_CHIP_1_SURF  */ ,&voc_pcm_on_chip_1_surf_cal
#ifdef FEATURE_SPEAKER_PHONE
	/* VOC_CODEC_SPEAKER_SURF    */ ,&voc_pcm_on_chip_speaker_surf_cal
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM  
	  ,&voc_pcm_on_chip_1_cal
	  ,&voc_pcm_on_chip_speaker_cal
#endif

  }
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
  ,{
	 /* CDMA WB */
    /* VOC_CODEC_ON_CHIP_0        */ &voc_pcm_on_chip_0_cal_wb
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    /* VOC_CODEC_ON_CHIP_0_DUAL_MIC */,&voc_pcm_on_chip_0_dual_mic_cal_wb
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */
    /* VOC_CODEC_ON_CHIP_1        */,&voc_pcm_on_chip_1_cal_wb
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_STEREO_HEADSET   */,&voc_pcm_on_chip_1_cal_wb
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
    /* VOC_CODEC_ON_CHIP_AUX      */,&voc_pcm_on_chip_aux_cal
    /* VOC_CODEC_ON_BOARD         */,&voc_pcm_on_board_cal
#ifdef FEATURE_SPEAKER_PHONE
    /* VOC_CODEC_SPEAKER          */,&voc_pcm_on_chip_speaker_cal_wb
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_BT
    /* VOC_CODEC_BT_INTERCOM      */,&voc_pcm_bt_intercom_cal
#ifdef FEATURE_BT_AG
    /* VOC_CODEC_BT_AG            */,&voc_pcm_bt_ag_cal_wb
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* VOC_CODEC_BT_A2DP          */,&voc_pcm_bt_ag_cal_wb
    /* VOC_CODEC_BT_A2DP_SCO      */,&voc_pcm_bt_ag_cal_wb
#endif /* FEATURE_SBC_CODEC */
#endif /* FEATURE_BT */
    /* VOC_CODEC_OFF_BOARD        */,&voc_pcm_off_board_cal
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    /* VOC_CODEC_SDAC             */,&voc_pcm_sdac_cal
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
#if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
    /* VOC_CODEC_IN_MONO_SADC_OUT_MONO_HANDSET     */  ,&voc_pcm_on_chip_0_cal
    /* VOC_CODEC_IN_MONO_SADC_OUT_STEREO_HEADSET   */  ,&voc_pcm_on_chip_1_cal
    /* VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET   */  ,&voc_pcm_on_chip_0_cal
    /* VOC_CODEC_IN_STEREO_SADC_OUT_STEREO_HEADSET */  ,&voc_pcm_on_chip_1_cal
#endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */
#ifdef FEATURE_TTY
    /* VOC_CODEC_TTY_ON_CHIP_1    */,&voc_pcm_tty_on_chip_1_cal
    /* VOC_CODEC_TTY_OFF_BOARD    */,&voc_pcm_tty_off_board_cal
    /* VOC_CODEC_TTY_VCO          */,&voc_pcm_tty_vco
    /* VOC_CODEC_TTY_HCO          */,&voc_pcm_tty_hco
#endif /* FEATURE_TTY */
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    /* VOC_CODEC_USB              */,&voc_pcm_usb_hfk_cal_wb
#ifdef FEATURE_INTERNAL_SDAC
    /* VOC_CODEC_STEREO_USB       */,&voc_pcm_usb_hfk_cal_wb
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
  /* FM over BT AG */
#ifdef FEATURE_BT_AG
                                    ,&voc_pcm_fm_bt_ag_cal
#endif
    /* VOC_CODEC_ON_CHIP_0_SURF   */,&voc_pcm_on_chip_0_surf_cal_wb
    /* VOC_CODEC_ON_CHIP_1_SURF   */,&voc_pcm_on_chip_1_surf_cal_wb
#ifdef FEATURE_SPEAKER_PHONE
	/* VOC_CODEC_SPEAKER_SURF     */,&voc_pcm_on_chip_speaker_surf_cal_wb
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM  
	 /* VOC_CODEC_HEADSET_AND_SPEAKER */,&voc_pcm_on_chip_1_cal
	 /* VOC_CODEC_HEADSET_AND_SPEAKER */,&voc_pcm_on_chip_speaker_cal
#endif

  }
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
};

/* <EJECT> */
/*===========================================================================

             D F M   D E F A U L T  C O N F I G U R A T I O N

===========================================================================*/

/* Default FM configuration mask
*/
#define VOC_DFM_AUDIO_DEF       ( QDSP_FM_RX_AUTO_MUTE_ENA_V | \
                                  QDSP_FM_TX_MUTE_ENA_V      | \
                                  QDSP_FM_COMP_ENA_V         | \
                                  QDSP_FM_PREEMP_ENA_V       | \
                                  QDSP_FM_LOOPBACK_DIS_V     | \
                                  QDSP_FM_DEEMP_ENA_V        | \
                                  QDSP_FM_EXPAND_ENA_V       | \
                                  QDSP_FM_SAT_TRANS_DIS_V    )

/* Default FM SAT Control Mask
*/
#define VOC_DFM_SAT_DEF         ( QDSP_FM_SAT_INT_ENA_V      | \
                                  QDSP_FM_SAT_ECC_6000_V     )

#define VOC_DFM_SAT_AGC_NOM         0x00e6
#define VOC_DFM_SAT_THRESH          0x0320
#define VOC_DFM_DEV_LIMIT_NOM       0x3180
#define VOC_DFM_DEV_LIMIT_DATA      0x3180
#define VOC_DFM_SAT_DC_OFFSET_NOM   0x0050


/* The default DFM configuration
*/
CAL_MEMORY qdsp_cmd_dfm_config_type voc_cal_dfm_config = {
  VOC_DFM_AUDIO_DEF,
  VOC_DFM_SAT_DEF,
  VOC_DFM_SAT_AGC_NOM,
  VOC_DFM_SAT_THRESH,
  VOC_DFM_DEV_LIMIT_NOM,
  VOC_DFM_SAT_DC_OFFSET_NOM
};

#define VOC_DATA_SLOWTALK_MAX_EXPANSION_RATIO      6

/* saved slowtalk paramters */
qdsp_cmd_slowtalk_parms_type voc_cal_slowtalk_configs =
{
  100,              /* ezHearTargetExpRatioIF */ /*default = no expansion */
  1,                /* ezHearVocoderNBorWBIF */  /*default = NB vocoder */
  50,               /* ezHearMaxLocalExpCompDiffIF */
  6,                /* ezHearMinSilenceDurationIF */
  4,                /* ezHearProcessDelayIF */
  0,                /* ezHearMaxGenSamplesExpansionIF */
  29511,            /* ezHearAvadThreshIF */
  29511,            /* ezHearAvadThresh2IF */
  128,              /* ezHearAvadPwrScaleIF */
  6,                /* ezHearAvadHangoverMaxIF */
  3277,             /* ezHearAvadAlphaSNRIF */
  12042,            /* ezHearAvadAvarScaleIF */
  3072,             /* ezHearAvadSnrDiffMaxIF */
  2560,             /* ezHearAvadSnrDiffMinIF */
  200,              /* ezHearAvadInitLengthIF */
  458,              /* ezHearAvadMaxValIF */
  145,              /* ezHearAvadInitBoundIF */
  290,              /* ezHearAvadResetBoundIF */
  40000,            /* ezHearCirBufSizeIF */
  456340275,        /* ezHearMinimuSimilarityIF */
  483183820,        /* ezHearMinPastSimilarityIF */
  429496729         /* ezHearMinForceSimilarityIF */
};


/* ===========================================================================
**
**                    A C C E S S   F U N C T I O N S
**
** =========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
/*===========================================================================

FUNCTION voc_cal_get_codec_class

DESCRIPTION
  Returns the codec type given a codec id.

DEPENDENCIES
  None.

RETURN VALUE
  voc_codec_class_type   = The classification of codec referred to by the
                           function argument.

SIDE EFFECTS
  None.

===========================================================================*/
voc_codec_class_type voc_cal_get_codec_class (
  voc_codec_type     pcm_path
)
{
  if (pcm_path < VOC_CODEC_MAX)
  {
    return voc_cal_codec_class[pcm_path];
  }
  else
  {
    return VOC_CODEC_CLASS_NONE;
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#ifdef FEATURE_PHONE_VR
/* <EJECT> */
/*===========================================================================

FUNCTION voc_cal_get_codec_vr_type

DESCRIPTION
  Returns the codec vr type given a codec id.

DEPENDENCIES
  None.

RETURN VALUE
  voc_codec_vr_type   = The VR type of codec referred to by the function
                        argument.

SIDE EFFECTS
  None.

===========================================================================*/
voc_codec_vr_type voc_cal_get_codec_vr_type (
  voc_codec_type     pcm_path
)
{
  if (pcm_path < VOC_CODEC_MAX)
  {
    return voc_cal_codec_vr_types[pcm_path];
  }
  else
  {
    return VOC_CODEC_VR_TYPE_NONE;
  }
}
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_cal_get_pcm_cal

DESCRIPTION
  Returns a pointer the pcm calibration data.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_CAL_SUCCESS - data retrieved successfully
  VOC_CAL_FAILURE - could not get data

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_cal_return_type voc_cal_get_pcm_cal (
  voc_codec_type        pcm_path,
  qdsp_image_type       image,
  voc_pcm_path_cal_type **pcm_cal_ptr
)
{
  voc_codec_mode_type mode;

  voc_data_get_codec_mode(image, &mode);

  switch(mode)
  {
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
  case VOC_CODEC_MODE_CDMA_WB_VOICE:
    *pcm_cal_ptr = (voc_pcm_path_cal_type *)
                      voc_pcm_cal_aptr[VOC_CAL_PCM_PATH_CAL_CDMA_WB][pcm_path];
	break;
  case VOC_CODEC_MODE_CDMA_VOICE:
  default:
    *pcm_cal_ptr = (voc_pcm_path_cal_type *)
                      voc_pcm_cal_aptr[VOC_CAL_PCM_PATH_CAL_CDMA][pcm_path];
	break;
  }
  return (VOC_CAL_SUCCESS);
} /* voc_cal_get_pcm_cal */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_cal_get_codec_config

DESCRIPTION
  Returns a pointer the codec configuration data.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_CAL_SUCCESS - data retrieved successfully
  VOC_CAL_FAILURE - could not get data

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_cal_return_type voc_cal_get_codec_config (
 voc_codec_type        pcm_path,
 qdsp_image_type       image,
 qdsp_cmd_codec_config_type **codec_config_ptr
)
{
  voc_codec_mode_type mode;
  *codec_config_ptr = NULL;

  voc_data_get_codec_mode(image, &mode);
  *codec_config_ptr = (qdsp_cmd_codec_config_type *)
                         voc_cal_audio_path_config[voc_codec_mode_to_voc_cal_codec_cfg[mode]][pcm_path].codec_config;
  return(VOC_CAL_SUCCESS);
} /* voc_cal_get_codec_config */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#ifndef MSMAUD_SCMM
/* <EJECT> */
#ifdef MSMAUD_ADIE_CODEC
/*===========================================================================

FUNCTION voc_cal_get_adie_config

DESCRIPTION
  Gets the ADIE configuration data, based on the PCM path and the DSP image.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_CAL_SUCCESS - data retrieved successfully
  VOC_CAL_FAILURE - could not get data

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_cal_return_type voc_cal_get_adie_config (
 voc_codec_type             pcm_path,
 qdsp_image_type            image,
 voc_adie_codec_config_type **adie_config_ptr
)
{
  voc_codec_mode_type mode;
  *adie_config_ptr = NULL;

  voc_data_get_codec_mode(image, &mode);
  *adie_config_ptr = (voc_adie_codec_config_type *)
                          voc_cal_audio_path_config[voc_codec_mode_to_voc_cal_codec_cfg[mode]][pcm_path].adie_config;
  return(VOC_CAL_SUCCESS);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* MSMAUD_ADIE_CODEC */

#endif /* MSMAUD_SCMM */

#ifdef MSMAUD_PMIC_CONFIG
/*===========================================================================

FUNCTION voc_cal_get_pmic_config

DESCRIPTION
  Gets the PMIC configuration data, based on the PCM path and the DSP image.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_CAL_SUCCESS - data retrieved successfully
  VOC_CAL_FAILURE - could not get data

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_cal_return_type voc_cal_get_pmic_config (
 voc_codec_type             pcm_path,
 qdsp_image_type            image,
 voc_pmic_config_type       **pmic_config_ptr
)
{
  voc_codec_mode_type mode;
  *pmic_config_ptr = NULL;

  voc_data_get_codec_mode(image, &mode);
  *pmic_config_ptr = (voc_pmic_config_type *)
                          voc_cal_audio_path_config[voc_codec_mode_to_voc_cal_codec_cfg[mode]][pcm_path].pmic_config;
  return(VOC_CAL_SUCCESS);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* MSMAUD_PMIC_CONFIG */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#ifdef FEATURE_VOC_MIC_GAIN_ADJUST
/* <EJECT> */
/*===========================================================================

FUNCTION voc_cal_get_mic_gain_adjust

DESCRIPTION
  Returns the stored mic gain adjustment value for the specified codec.

DEPENDENCIES
  voc_cal_mic_gain_adjust

RETURN VALUE
  VOC_CAL_SUCCESS - data retrieved successfully
  VOC_CAL_FAILURE - could not get data

SIDE EFFECTS
  None.

===========================================================================*/
voc_cal_return_type voc_cal_get_mic_gain_adjust
(
  voc_codec_type        pcm_path,
  sint15              **gain_cal_ptr
)
{
  if (pcm_path < (uint16) VOC_CODEC_MAX) {
    *gain_cal_ptr = &voc_cal_mic_gain_adjust[pcm_path];
    return (VOC_CAL_SUCCESS);
  } else {
    return (VOC_CAL_FAILURE);
  }
} /* voc_cal_get_mic_gain_adjust */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_cal_set_mic_gain_adjust

DESCRIPTION
  Stores the specified mic gain adjustment dbm value for the specified codec.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_CAL_SUCCESS - data retrieved successfully
  VOC_CAL_FAILURE - could not get data

SIDE EFFECTS
  voc_cal_mic_gain_adjust

===========================================================================*/
voc_cal_return_type voc_cal_set_mic_gain_adjust
(
  voc_codec_type        pcm_path,
  sint15                gain_cal
)
{
  if (pcm_path < (uint16) VOC_CODEC_MAX) {
    voc_cal_mic_gain_adjust[pcm_path] = gain_cal;
    return (VOC_CAL_SUCCESS);
  } else {
    return (VOC_CAL_FAILURE);
  }
} /* voc_cal_set_mic_gain_adjust */
#endif /* FEATURE_VOC_MIC_GAIN_ADJUST */

#ifdef FEATURE_VOC_NV_CAL
/* <EJECT> */
/*===========================================================================

LOCAL FUNCTION voc_cal_get_nv

DESCRIPTION
  Retrieves data stored in specified NV item.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_CAL_SUCCESS - NV read operation was successful.
  VOC_CAL_FAILED  - NV read operation failed.

SIDE EFFECTS
  VOC task is blocked until NV operation completes.

===========================================================================*/
static voc_cal_return_type voc_cal_get_nv
(
  nv_items_enum_type item,        /* which item                */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{
  static nv_cmd_type nvi;            /* nv command buffer                  */

  nvi.tcb_ptr    = &voc_tcb;         /* notify this task when done         */
  nvi.sigs       = VOC_NV_SIG;
  nvi.done_q_ptr = NULL;             /* command goes on no queue when done */

  nvi.item       = item;             /* item to read                       */
  nvi.cmd        = NV_READ_F;

  /* Set up NV so that it will read the data into the correct location */
  nvi.data_ptr   = data_ptr;


  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( &voc_tcb, VOC_NV_SIG );
  nv_cmd( &nvi );
  voc_nv_wait();

  if( nvi.status != NV_DONE_S ) {
    return VOC_CAL_FAILURE;
  }

  return VOC_CAL_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_cal_read_nv_cal

DESCRIPTION
  Reads NV related to voc calibration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_mic_gain_adjust

===========================================================================*/
void voc_cal_read_nv_cal (void)
{

#ifdef FEATURE_VOC_MIC_GAIN_ADJUST
  static nv_item_type data;

  /* NV_VOC_HS_MIC_GAIN_ADJUST_I */
  if (voc_cal_get_nv(NV_VOC_HS_MIC_GAIN_ADJUST_I,&data) == VOC_CAL_SUCCESS) {
    voc_cal_mic_gain_adjust[VOC_CODEC_ON_CHIP_0] =
      data.voc_hs_mic_gain_adjust;

#ifdef FEATURE_SPEAKER_PHONE
    voc_cal_mic_gain_adjust[VOC_CODEC_SPEAKER] =
      data.voc_hs_mic_gain_adjust;
#endif /* FEATURE_SPEAKER_PHONE */
  }
#endif /* FEATURE_VOC_MIC_GAIN_ADJUST */

}
#endif /* FEATURE_VOC_NV_CAL */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_cal_set_slowtalk_params

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_cal_set_slowtalk_params(
  voc_slowtalk_params_type *params
)
{
  static uint16 exp_ratio_level_conversion
                   [VOC_DATA_SLOWTALK_MAX_EXPANSION_RATIO]=
                                    {100, 110, 120, 130, 140, 150};

  /* Check for valid configuration */
  if (params->expansion_ratio_level >= VOC_DATA_SLOWTALK_MAX_EXPANSION_RATIO)
  {
    return (VOC_CMD_FAILED_S);
  }

  voc_cal_slowtalk_configs.ezHearTargetExpRatioIF = \
      exp_ratio_level_conversion[params->expansion_ratio_level];

  return (VOC_DONE_S);
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_cal_get_slowtalk_params

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_slowtalk_parms_type* voc_cal_get_slowtalk_params(void)
{
  return (&voc_cal_slowtalk_configs);
}


/* End of voccal.c */

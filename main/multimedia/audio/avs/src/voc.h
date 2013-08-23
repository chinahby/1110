#ifndef VOC_H
#define VOC_H
/*===========================================================================

         V O C O D E R    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the Vocoder Services.

REFERENCES
  MSM3000 User's Manual: 93-24207-1
  MSM3100 Device Specification: 93-V0104-1

Copyright (c) 1999 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/voc.h#30 $ $DateTime: 2009/10/06 06:46:01 $ $Author: subhashj $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/06/09   sj      Added SCMM specific changes
07/13/09   rm      Adding support for 4GV_WB_VoIP.
07/07/09   rk      Notifying SND that PAs are turned on, wakes up sound task.
05/08/09    vm     Added Support for Device switch from external to internal.
04/20/09    rm     CR 168030, handling voc_state_get_high_module
04/06/09    av     Adding voc_service_cfg_type.
04/07/09   knm     Added voc_master_vol() API to set the master volume.
19/02/09   kd/knm  Fixed CR-167490,sending midi commands before setting 
                   the rx and tx gains.
01/19/09   anb     Added voc_set_qtv_cb_func so that QTV can be informed about
                   mod/image swap event.Fixes Cr 163807(SR-1133583)
03/10/09   sj      Added slowtalk feature support
01/19/09   dp      CR 168251 Removed feturization for FEATURE_BT_QSOC
12/26/08   rm      Added RVE API changes
12/23/08   vm     Added test code for AUXPCM testing support 
12/19/08   rm      Fix for CR 164515 - Voice(Qcelp) recording fails after calling(4GV)
11/14/08   sj      Don't enable ECNS when we don't require it. Fixed CR 160542
11/24/08   skr     Added code for Audio + camera Concurrency
10/17/08   vm      added idlemetring support for qtv image
10/22/08   dp      FM over BT AG changes
10/14/08   anb     Delay sending bitstream/midi commands to the DSP till BT 
                   AG connection is up when FEATURE_AVS_BT_SCO_REWORK enabled.
                   Changes complete fix for CR 140315.
10/14/08   skr     Fixed CR 156254 Reset the sample rate and configuration 
10/02/08    vsud   Fixed CR# 157201 for SMAF file play back issues.
09/15/08   knm     Added support for Aux Line In(FM) volume control
08/26/08   anb     Added support for DSP based WMAPRO
07/11/08   skr     Changes to release VOCODER & ADEC separately during Incall.
06/20/08   knm     Added support for AuxPGA Line In loopback path configuration
04/15/08    sj     Added new API voc_set_codec_override() to support 
                   overriding any codec type with other codec.
03/27/08    KK     Fixed compiler Errors when FEATURE_AUDIO_QENSEMBLE is 
                   enabled for LCU.
03/17/08   anb     Fixed compiler warning by adding voc_mp3_set_song_config()  
03/11/08    kk     Added support for sending mp3 configuration 
                   information for DSP. 
02/07/08    kk     Added support for Qensemble phase 2 interface changes.
01/15/08    hs     Added support ISDB-T dual mono output control
01/07/08   az      Included the in call midi and the new in call architecture.
11/14/07    hs     Added QEnsemble support
08/27/07   ymc     Added crystal speech support.
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
08/08/07  anb/wfd  Added aux_pcm_clk to be enabled/disabled from other tasks like BT (CR 122106).
04/07/07    pl     Added support for starting qsynth AMR-NB
                   decoder.
03/16/07   ymc     Fixed voc_capability_type enum definitions for
                   AMR-WB vocoder.
02/26/07   kan     Added AMR-WB changes 
01/23/07    kk     Fixed issues related to adding 44k sample rate. 
12/04/06   ymc     Added VOC_EC_BT_HEADSET.
12/02/06   suk     Added seperate vocoder capability for EVRC VoIP.
11/08/06    sp     Added support for OEMSBL Audio.
10/31/06  sp/anb   Serialized the commands sent to BT, Fixes 97271 
09/11/06   ak      Added extern declarations for voc_clients_requested_cap, 
                   voc_clients_requested_cap_dir and voc_check_rec_capability.
                   Added type: voc_client_cap_dir_type. 
08/04/06    hz     Added fixes to CR#85799 - during VFE mode, some of wav 
                   shutter sounds can not be played.
09/08/06    pl     Added 44K CLK enum to voc_sample_rate_type.
09/05/06    ay     Added AMR-WB+ support.
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
08/13/06   act     Added ACQ key for handset driver for use in HSSD algorithm
07/31/06   anb     modified the QDSP_playback_enum_type to support BT apps team requirement
                   of variating between a HW/DTMF ringer on FFA
07/27/06    pl     Added ping-pong buffer support and new game audio features.
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
07/17/06    hz     Moved voc_get_desired_config from voci.h to here, since 
                   mvs needs to use it to check incall.
07/12/06    hz     Added support for incall mp3 GSM/UMTS, BTSCO and PostProc
                   features.
07/10/06   act     Added voc_hssd_type to control HSSD circuit behavior.
06/14/06    sp     Delay sending bitstream/midi commands to the DSP till BT 
                   A2DP connection is up.
05/31/06    sg     Re-featureized synth MIDI functions so that they are not
                   under FEATURE_ADEC.
03/27/06    ak     Added support for sidetone control.
03/15/06   ymc     Added FEATURE_INTERNAL_SADC support.
03/13/06   act     Adding voc_rx_dtmf_override_cmd().
03/13/06    st     Added support for delayed start of audio decoder channel;
                   added support for 3D positional final source settings
                   query command.
02/21/06   act     Modified voc_rx_mute_cmd() to support new parameter
                   for DTMF mixing support.
02/03/06    sm     Added 4GV VOIP support
01/31/06    hz     Added support for in-call mp3 feature.
01/30/06    sp     Added support for I2S SBC interface.
01/16/06    sm     Added FEATURE_QDJ_TIMEWARP
12/07/05    sp     Made changes so that codec config is issued only in idle
                   state when enabling BT.
11/16/05    sp     Made changes to support BT CCAP requirements. BT APP can
                   now initialize SCO connection at any time it is required.
                   Audio driver will issue codec config and setup the hardware
                   link only when appropriate image gets loaded onto ADSP.
10/19/05    pl     Added PCM AVSync support.
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05   pl/st   Added support for QAudioFX: 3D positional commands and query
                   interface; added voc_qcp_adec_start() function prototype.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is 
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
09/12/05    hz     Added AV sync support for windows media.
08/31/05    sm     VOX control
            sm     Added support for mode in voc_amp_enable_func_ptr_type
08/29/05    sp     Made changes so that A2DP suspend happens in vocfunc.c only.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
07/28/05    sm     Fixes for G723.
07/22/05    sm     Added WAV A/Ulaw support.
07/20/05    sp     Added support for AAC LOAS.
07/19/05    pl     Added new status VOC_DSP_DATA_ERR_S in voc_status_type.
07/13/05    sp     Added code to honor bitpool values preferred by BT A2DP 
                   headset. 
07/06/05   act     Modified voc_amp_enable_func_ptr_type to allow codec ID
                   to be passed as a parameter.
07/05/05    sm     Added G.723.1 support.
06/23/05    sm     TTY updates for auto-bypass and VCO/HCO
06/03/05    sg     Changed VOC_DUMMY_DATA_UINT32_MAX to 0x7FFFFFFF since the
                   compiler optimizes 0xFFFFFFFF to -1.
            sp     Changed sbc_data_type to audiosbcenc_data_type consequent
                   to such changes in BT driver.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/20/05    hz     Added support for MultiMedia/AAC record functionality.
            sg     Added directives for L4 support; forced some enums to use
                   32bit words.
05/16/05    hz     Added support for Vocoder image+ADPCM+VFE.
04/28/05   ymc     Added 4GV NB support.
04/22/05    aw     Clean up code for preset equalizer function.
04/21/05    hz     Added VirtualPacketLength support for Windows Media 
                   playback.
04/21/05   act     Changed name of voc_codec_type to voc_codec_class_type.
                   Reverted name of voc_codec_id_type to voc_codec_type.
04/01/05   act     Created generic codec types to associate with specific codec
                   IDs. Exported codec IDs to voccodec.h.
03/25/05   ac/sg   Added support for A/V synchronization.
03/21/05    aw     Feature-rized rex.h under FEATURE_PMIC_SPEAKER_ON_OFF_DELAY
03/14/05    sg     Included rex.h for PMIC driver support.
03/02/05    aw     Added support for Qconcert; preset EQ bands.
02/22/05    sm     Added G.711 vocoder support.
02/14/05    hz     Added support for windows media audio decode;
                   New voc capability: VOC_CAP_QTUNES_WMA.
02/05/05    st     Modified voc_set_dls_config() to support HQ DLS
                   configuration.
12/03/04    sm     Added FEATURE_VOC_DTMF_DETECT, removed FEATURE_USB_ISOC_AUDIO
12/01/04    aw     Added voc_check_pb_capability to check supported voice
                   playback type.
11/19/04    sp     Added support for Arm Post Processing.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
08/19/04    sg     Added voc_config_standby()
08/11/04    st     Added voc_midi_cmd_type structure definition and updated
                   voc_midi_cmd() function prototype in order to handle
                   multiple MIDI command buffers.
06/17/04    aw     Added voc_equalizer_init_crit_sect prototype.
06/08/04    aw     Added support for Spectrum Analyzer and Equalizer.
05/11/04    vp     Merged ADRC and PCM filter support MP3/AAC from
                   MSMSHARED_AVS.00.09.05.02.00.
05/04/04    sm     Added FEATURE_PMIC_SPEAKER_ON_OFF_DELAY.
03/22/04    st     Updated voc_pause_midi_seq() and voc_set_vol_midi_seq()
                   function prototypes; added support for PCM adec mode;
                   added VOC_PCM_FRAME_LENGTH for PCM recording.
                   Removed voc_seq_vol() function prototype; updated
                   voc_adec_pause() and voc_adec_chan_pause() function
                   prototypes; added voc_pause_midi_seq() and
                   voc_set_vol_midi_seq() function prototypes.
01/26/04    lg     Modified code to support volume adjustment and audioPause as
                   an array for multi_sequencer
01/26/04    sg     Added return status for voc_sample_rate().
01/21/04    aw     Added support for qsynth volume mapping function.
12/08/03    sg     Modified to support changes in the vocoder PCM interface.
12/02/03    sg     Added support for linear PCM playback.
11/22/03    sg     Added support for RealAudio codec.
10/16/03    aw     Modified code to support SMAF and Yamaha ADPCM.
10/10/03    sg     Modified to support additional AAC params reqd by the DSP.
            aw     Added MSMAUD_VOC_AMR_COMMON featurization.
08/21/03    st     Added VOC_CODEC_BT_INTERCOM codec type.
07/24/03    sg     Added STEREO_HEADSET codec type.
05/13/03   ymc     Added GSM half rate support.  Added voc_d2e_loop_cmd.
           ymc     Removed WVS support.
           ymc     Featurized voc_gsm_[rt]x_packet under MSMAUD_VOC_*.
           ymc     Updated copyright.
05/06/03    sg     Updated voc_aac_set_song_config() interface.
04/18/03    aw     Added audio support for Q3D image.
04/17/03    sm     Removed unused parm in voc_amr_ul_fn_type.
            sg     Added support for vocoder PCM interfaces.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
03/10/03    st     Added voc_adec_pause() for FEATURE_ADPCM.
            sm     Merged HR support so to use new MVS baseline.
02/28/03    st     Added voc_ec_ns_mode_type and voc_set_ec_override_mode()
                   and voc_set_ns_override_mode() function prototypes.
02/19/03    st     Added VOC_CMD_BUSY_S status; added support for ADPCM audio
                   decoder; renamed voc_qsynth_adec_mode_type enums. 
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
12/03/02    st     Added VOC_INFORM_GRAPH_IDLE and VOC_INFORM_GRAPH_AUDIO.
11/19/02    st     Added voc_synth_mvol() and voc_adec_mvol() function
                   prototypes.
11/18/02    st     Featurized GSM/WCDMA specific code.
11/06/02    sm     Added QCP AMR support.
10/29/02    st     Added codec type for speaker phone.
09/09/02   ymc     Added FEATURE_MVS.
           ymc     Added voc_get_curr_config.
09/06/02    st     Added voc_complete_bt_ag() function prototype; refeaturized
                   voc_qsynth_adec_mode_type properly.
08/22/02    sg     Added support for TTY activity status indication.
                   (Merged from MSM_AVS.01.05.18.01.02).
08/13/02   aw/st   Modified code to add support for ADPCM audio decoder.
           sg/st   Added support for MP4 and AAC.
07/09/02    sm     Synced to perforce tip.           
           ymc     Renamed VOC_AMR_SPEECH_PRO_DEGRADED as
                   VOC_AMR_SPEECH_DEGRADED, VOC_AMR_SPARE as VOC_AMR_ONSET.
03/12/02    st     Unfeaturized voc_sample_rate_type; obsoleted
                   FEATURE_NEW_SYNTH; replaced voc_codec_gain() with
                   voc_codec_pad() prototype; removed HFK pins function
                   prototypes.
03/04/02   jkl     Added VOC_VR_PUFE.
01/15/02   ymc     Added GSM and WCDMA support.
           ymc     Deleted VOC_API_VERSION.  VOC_API_VERSION_1 is no longer
                   supported.
           ymc     Updated copyright.
12/12/01    aw     Merged with MSM_AVS.01.05.17.
10/29/01    sg     Added packet interface support.
10/04/01    sg     Added VCO and HCO modes for TTY.
10/01/01    st     Modified code to support multiple ADEC channels:
                   voc_adec_move_data(), voc_adec_flush(), voc_adec_reset(),
                   voc_adec_chan_pan(), and voc_adec_chan_vol().
10/01/01    sm     Added FEATURE_BT_AG_LOCAL_AUDIO.
09/03/01    st     Removed voc_get_qcp_to_mem_buf() function prototype.
07/24/01   ymc     Added VOC_ACQ_TEST and voc_micro_loop_cmd.
06/18/01  jkl/ym   Added FEATURE_ACP.
05/21/01   jkl     Removed Enhanced VR feature.
04/26/01    st     Added voc_adec_mode_type definition; updated 
                   voc_adec_move_data() function prototype.
04/26/00    sm     Added FEATURE_USB_ISOC_AUDIO support.
                   Removed speed_factor from voc_mp3_set_song_config().
04/04/01    st     Updated featurization to include support for new features;
                   added voc_adec_reset() prototype.
03/02/01    sm     Added voc_set_bt_app_id() function prototype.
02/28/01    st     Removed 16K codecs from voc_codec_type, 16K is now implied
                   for QSynth and QTunes images.
12/10/00    st     Added proper featurization to voc_sample_rate_type and
                   throughout file.
12/05/00   jkl     Merged VR4.0 functions. Added FEATURE_ENHANCED_VR,
12/04/00    sm     Added voc_adec_chan_vol(), voc_mp3_set_song_config(),
                   voc_adec_chan_pan(), voc_master_volume(), voc_master_pan().
                   Voc_adec_move_data() returns voc_status_type.
            sm     Added QTUNES, MIDI, sample rate support.
09/18/00    sm     Added analog QSynth codec path.
09/08/00    sm     Added Bluetooth/PPG support.
08/28/00    ro     Uses QDSP Services - removed voc_offline, voc_online, 
07/27/00    ro     Support for FEATURE_VM_MEM_TO_QCP.
07/14/00   spf     Added support for TTY headset and HFK devices.
06/07/00    st     Added voc_register_codec_func() function prototype
                   and voc_amp_enable_func_ptr_type.
05/16/00    st     Added stereo DAC support; added support for QSynth image;
                   added voc_midi_cmd() function prototype.
05/12/00   spf     Add TTY support: voc_tty_mode_type and voc_set_tty_mode().
           jwh     Add UART2/PCM sharing: voc_disable_hfk_pins() and
                   voc_enable_hfk_pins(). (FEATURE_SDEVMAP_SHARE_HFK)
05/02/00   spf     Added speakerphone Echo Canceller support. Added 
                   VOC_INFORM_NO_SAT.
            sm     Added voc_aud_process_override().
01/25/00    ro     Add VR support: voc_get_service_option(), voc_set_vr_mode(), 
                   voc_ec_type, voc_ns_type, voc_pcm_filter_type.
                   Remove voc_set_msm_mode.
12/07/99   spf     Added voc_valid_addr().  Renamed voc_dpeek_cmd() and
                   voc_dpoke_cmd() to voc_peek_cmd() and voc_poke_cmd().
10/27/99   spf     Added VOC_API_VERSION_1 and VOC_API_VERSION_2 definitions.
10/25/99   spf     Comment cleanup.
10/18/99    st     Added support for audio AGC/AVC.
10/18/99   tc      Added extern voc_reset_ones_detect.
10/14/99   jkl     Added Pkt internal, DFM loop command functions.
                   Created VOC_API_VERSION.
10/12/99   tc      Added voc_read_ones_detect() and voc_reset_ones_detect().
10/04/99   jkl     Modified for dbm purposes.
09/16/99   spf     Removed FEATURE_VOC_SAT_CONTROL
09/10/99   jkl     Codec gain update.
09/09/99   spf     Added voc_online() and voc_offline().
08/19/99   spf     ARM2.5 update.
08/02/99   spf     Removed voc_dfm_sat_dfm_ctrl().
07/15/99   spf     DFM support.
05/15/99   ro/sf   Vocoder task header file for MSM3100.  Taken from voc.h
                   for MSM3000.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "msmaud.h"             /* MSM specific Audio features            */
#include "voccodec.h"           /* CODEC IDs                              */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
#include "bt.h"                 /* Bluetooth interfaces                   */
#endif /* FEATURE_BT_AG || FEATURE_SBC_CODEC */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#include "audiosbc.h"        /* BT SBC Codec internal definitions       */
#include "btpf.h"
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_AUDIO_FORMAT
#include "qw.h"
#endif /* FEATURE_AUDIO_FORMAT */

#if defined(FEATURE_PMIC_SPEAKER_ON_OFF_DELAY)||defined(FEATURE_KAMP_ON_OFF_DELAY)
#include "rex.h"                /* Definition of REX data types           */
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */
#include "snd.h"
/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* This definition is used to force enums to use 32 bits - required for L4 */
#define VOC_DUMMY_DATA_UINT32_MAX 0x7FFFFFFF

/*---------------------------------------------------------------------------
**                        VOCODER DATA RATE CONSTANTS
**
** The following constants are used with the 'rate' fields of vocoder
** transmit and receive command and response packets.
**-------------------------------------------------------------------------*/

typedef enum  
{
  VOC_0_RATE = 0,        /* Indicates vocoder data was blanked.      */
  VOC_8_RATE,            /* Indicates rate 1/8 vocoder data.         */
  VOC_4_RATE,            /* Indicates rate 1/4 vocoder data.         */
  VOC_2_RATE,            /* Indicates rate 1/2 vocoder data.         */
  VOC_1_RATE,            /* Indicates rate 1   vocoder data.         */
  VOC_ERASURE,           /* Indicates frame erasure                  */
  VOC_ERR_RATE,          /* Indicates invalid vocoder data.          */
  VOC_AMR_WB_RATE_0660 = 0, /* 6.60 kbit /s                          */
  VOC_AMR_WB_RATE_0885 = 1, /* 8.85 kbit /s                          */
  VOC_AMR_WB_RATE_1265 = 2, /* 12.65 kbit /s                         */
  VOC_AMR_WB_RATE_1425 = 3, /* 14.25 kbit /s                         */
  VOC_AMR_WB_RATE_1585 = 4, /* 15.85 kbit /s                         */
  VOC_AMR_WB_RATE_1825 = 5, /* 18.25 kbit /s                         */
  VOC_AMR_WB_RATE_1985 = 6, /* 19.85 kbit /s                         */
  VOC_AMR_WB_RATE_2305 = 7, /* 23.05 kbit /s                         */
  VOC_AMR_WB_RATE_2385 = 8, /* 23.85 kbit /s                         */
  VOC_AMR_WB_RATE_MAX  = 9,
  VOC_AMR_RATE_475  = 0, /* 4.75 kbit /s                             */
  VOC_AMR_RATE_515  = 1, /* 5.15 kbit /s                             */
  VOC_AMR_RATE_590  = 2, /* 5.90 kbit /s                             */
  VOC_AMR_RATE_670  = 3, /* 6.70 kbit /s                             */
  VOC_AMR_RATE_740  = 4, /* 7.40 kbit /s                             */
  VOC_AMR_RATE_795  = 5, /* 7.95 kbit /s                             */
  VOC_AMR_RATE_1020 = 6, /* 10.20 kbit /s                            */
  VOC_AMR_RATE_1220 = 7, /* 12.20 kbit /s, also used for GSM EFR     */
  VOC_GSM_ENH_FULL_RATE = 7, /* EFR placeholder                      */
  VOC_GSM_FULL_RATE = 8, /* 13.00 kbit /s, GSM Full Rate             */
  VOC_GSM_HALF_RATE = 9, /* 5.60 kbit /s, GSM Half Rate              */
  VOC_GSM_RATE_MAX  = 10,

  /* DO NOT USE: Force this enum to be a 32bit type */
  VOC_GSM_RATE_32BIT_DUMMY = VOC_DUMMY_DATA_UINT32_MAX
} voc_rate_type;

#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_AMR_WB_COMMON)

typedef enum
{
  VOC_GSM_DTX_OFF,      /* disable encoder DTX mode */
  VOC_GSM_DTX_ON        /* enable encoder DTX mode */
} voc_gsm_dtx_mode_type;

#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR || MSMAUD_VOC_AMR_WB_COMMON */


#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR)

typedef enum
{
  VOC_GSM_SID,                  /* GSM HR, FR or EFR : silence descriptor   */
  VOC_GSM_SPEECH_GOOD,          /* GSM HR, FR or EFR : good speech frame    */
  VOC_GSM_BFI,                  /* GSM HR, FR or EFR : bad frame indicator  */
  VOC_GSM_INVALID_SID,          /* GSM HR            : invalid SID frame    */

  VOC_AMR_SPEECH_GOOD,          /* AMR : good speech frame              */
  VOC_AMR_SPEECH_DEGRADED,      /* AMR : degraded speech frame          */
  VOC_AMR_ONSET,                /* AMR : onset                          */
  VOC_AMR_SPEECH_BAD,           /* AMR : bad speech frame               */
  VOC_AMR_SID_FIRST,            /* AMR : first silence descriptor       */
  VOC_AMR_SID_UPDATE,           /* AMR : successive silence descriptor  */
  VOC_AMR_SID_BAD,              /* AMR : bad silence descriptor frame   */
  VOC_AMR_NO_DATA,              /* AMR : no data                        */

  VOC_GSM_FRAME_MAX
} voc_gsm_frame_type;

typedef voc_rate_type voc_gsm_frame_rate_type;

#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_AMR_WB_COMMON

typedef enum {
  VOC_AMR_WB_SPEECH_GOOD,          /* AMR_WB : good speech frame              */
  VOC_AMR_WB_SPEECH_DEGRADED,      /* AMR_WB : degraded speech frame          */
  VOC_AMR_WB_SPEECH_LOST,          /* AMR_WB : speech lost frame              */
  VOC_AMR_WB_SPEECH_BAD,           /* AMR_WB : bad speech frame               */
  VOC_AMR_WB_SID_FIRST,            /* AMR_WB : first silence descriptor       */
  VOC_AMR_WB_SID_UPDATE,           /* AMR_WB : successive silence descriptor  */
  VOC_AMR_WB_SID_BAD,              /* AMR_WB : bad silence descriptor frame   */
  VOC_AMR_WB_NO_DATA,              /* AMR_WB : no data                        */
  VOC_AMR_WB_ONSET,                /* AMR_WB : onset                          */

  VOC_AMR_WB_FRAME_MAX
} voc_amr_wb_frame_type;

typedef voc_rate_type voc_amr_wb_frame_rate_type;

#endif  /* MSMAUD_VOC_AMR_WB_COMMON */

#ifdef MSMAUD_VOC_G711

/* G.711 vocoder modes */
typedef enum {
  VOC_G711_MODE_MULAW,
  VOC_G711_MODE_ALAW,
  VOC_G711_MODE_MAX,

  /* DO NOT USE: Force this enum to be a 32bit type */
  VOC_G711_MODE_32BIT_DUMMY = VOC_DUMMY_DATA_UINT32_MAX
} voc_g711_mode_type;

#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723

/* G.723.1 vocoder modes */
typedef enum {
  VOC_G723_MODE_HIGH,
  VOC_G723_MODE_LOW,
  VOC_G723_MODE_SID,
  VOC_G723_MODE_BLANK,
  VOC_G723_MODE_ERROR,
  VOC_G723_MODE_MAX
} voc_g723_mode_type;

#endif /* MSMAUD_VOC_G723 */

/* Vocoder Volume constants */
#define  VOC_VOL_SILENCE  -5000     /* volume for silence in ddbm */

/*---------------------------------------------------------------------------
                        VOCODER SERVICES RETURN VALUES

The following values can be returned by the Vocoder Services.
---------------------------------------------------------------------------*/

typedef enum
{
  VOC_DONE_S = 0,
    /* Indicates that command/response exchange was performed successfully. */

  VOC_BADFMT_S,
    /* Indicates that a format error was detected during the command/response
       exchange with the vocoder. For example, the response packet was not
       of the correct length. */

  VOC_BADCHK_S,
    /* Indicates that a packet checksum error was detected during the 
       command/response exchange with the vocoder. */

  VOC_TIMEOUT_S,
    /* Indicates that a timeout occurred while waiting for the vocoder
       response. */

  VOC_WINDOW_ERR_S,
    /* Indicates that a command was attempted outside of its allowed timing
       window. */

  VOC_CMD_BUSY_S,
    /* Indicates that the Vocoder Services was not able to execute the 
       command but not because the command was invalid. */

  VOC_CMD_FAILED_S,
    /* Indicates that the Vocoder Services was not able to execute the 
       command. */

  VOC_DSP_DATA_ERR_S,
    /* Indicates that the DSP reports error in the data stream */

  VOC_MAX_S,
    /* For determining number of status codes. This item MUST appear
       last in enum list. */

  /* DO NOT USE: Force this enum to be a 32bit type */
  VOC_STATUS_32BIT_DUMMY = VOC_DUMMY_DATA_UINT32_MAX
} voc_status_type;

/*---------------------------------------------------------------------------
                        VOCODER CAPABILITY VALUES

The following are all of the service options or capabilities that a vocoder
might support. They can be checked with 'voc_capability() or activated
with voc_config().
---------------------------------------------------------------------------*/
typedef enum {
  VOC_CAP_NONE      = 0,          /* No service option   */
  VOC_CAP_IS96      = (1 << 0),   /* IS-96  - QCELP-8k   */
  VOC_CAP_IS96A     = (1 << 1),   /* IS-96A - QCELP-8k   */
  VOC_CAP_IS733     = (1 << 2),   /* IS-733 - QCELP-13k  */
  VOC_CAP_IS127     = (1 << 3),   /* IS-127 - EVRC-8K    */
  VOC_CAP_4GV_NB    = (1 << 4),   /* 4GV narrow band     */
  VOC_CAP_4GV_WB    = (1 << 5),   /* 4GV wide band       */
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  VOC_CAP_AMR       = (1 << 8),   /* AMR Codec           */
  VOC_CAP_GSM_EFR   = (1 << 9),   /* GSM EFR Codec       */
  VOC_CAP_GSM_FR    = (1 << 10),  /* GSM Full Rate Codec */
  VOC_CAP_GSM_HR    = (1 << 11),  /* GSM Half Rate Codec */
#ifdef FEATURE_QSYNTH_COMMON
  VOC_CAP_QSYNTH    = (1 << 12),  /* QSynth Image        */
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES
  VOC_CAP_QTUNES    = (1 << 13),  /* QTunes Image        */
#endif /* FEATURE_QTUNES */
#ifdef FEATURE_QTUNES_AAC
  VOC_CAP_QTUNES_AAC = (1 << 14), /* QTunes AAC Image    */
#endif /* FEATURE_QTUNES_AAC */
#ifdef FEATURE_REAL_AUDIO
  VOC_CAP_QTUNES_RA  = (1 << 15), /* RealAudio Image     */
#endif /* FEATURE_REAL_AUDIO */
#ifdef FEATURE_WMA
  VOC_CAP_QTUNES_WMA = (1 << 16), /* Window Media Audio decoder */
#endif /* FEATURE_WMA */
  VOC_CAP_G711       = (1 << 17), /* G.711 vocoder       */
#ifdef FEATURE_MM_REC
  VOC_CAP_AAC_REC    = (1 << 18), /* MultiMedia Record   */
#endif /* FEATURE_MM_REC */
  VOC_CAP_G723       = (1 << 19), /* G.723.1 vocoder     */
  VOC_CAP_4GV_NB_VOIP= (1 << 20), /* 4GV narrow band, voip-specific           */
#ifdef FEATURE_AMR_WB_AUDIO_DEC
  VOC_CAP_QTUNES_AMR_WB     = (1 << 21), /* AMR-WB Audio Decoder */
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  VOC_CAP_QTUNES_AMR_WBPLUS = (1 << 22), /* AMR-WB+ Audio Decoder */
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
  VOC_CAP_IS127_VOIP = (1 << 23),  /* IS-127 - EVRC-8K, voip-specific  */
  VOC_CAP_AMR_WB     = (1 << 24),  /* AMR-WB vocoder */
#ifdef FEATURE_WMA
  VOC_CAP_QTUNES_WMA_PRO = (1 << 25), /* Window Media Audio decoder */
#endif /* FEATURE_WMA */
  VOC_CAP_4GV_WB_VOIP   =  (1 << 26),   /* 4GV_WB_VOIP Codec */
  VOC_CAP_MAX,

  /* DO NOT USE: Force this enum to be a 32bit type */
  VOC_CAP_32BIT_DUMMY = VOC_DUMMY_DATA_UINT32_MAX
} voc_capability_type;

/*---------------------------------------------------------------------------
                        VOCODER ACQUISITION VALUES

The following are the users of the Vocoder.  To use the vocoder, it is
acquired, used, and then later released.
---------------------------------------------------------------------------*/

/* Note: configuration priority is determined from low to high */

typedef enum {
  VOC_ACQ_HS = 0,
  VOC_ACQ_VOCREC,
  VOC_ACQ_CDMA,
#ifdef FEATURE_MVS
  VOC_ACQ_MVS,
#endif
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif
#ifdef FEATURE_MM_REC
  VOC_ACQ_MMREC,
#endif
  VOC_ACQ_SND,
  VOC_ACQ_VS,
  VOC_ACQ_VOCVS,/* For voicememo playback only */
#ifdef FEATURE_AVS_I2SSBC
  VOC_ACQ_I2SSBC,
#endif /* FEATURE_AVS_I2SSBC */
  VOC_ACQ_ACP,
  VOC_ACQ_TEST,
  VOC_MAX_ACQUIRE_TYPE,

  /* DO NOT USE: Force this enum to be a 32bit type */
  VOC_ACQ_32BIT_DUMMY = VOC_DUMMY_DATA_UINT32_MAX
} voc_acquire_type;

/*---------------------------------------------------------------------------
                        VOCODER CHANGE VALUES

voc_acquire is passed a pointer to a function to call when the vocoder goes
through a change of state.  Below are the statuses and a type definition
for the function pointer.
---------------------------------------------------------------------------*/

typedef enum {
  VOC_INFORM_RESET,        /* Vocoder is in reset                  */
  VOC_INFORM_IDLE,         /* Vocoder is in IDLE                   */
  VOC_INFORM_VOCODER,      /* Vocoder is in VOCODER state          */
  VOC_INFORM_CHANGE,       /* Vocoder is about to change state     */
  VOC_INFORM_MODULE_FETCHED,	/* Vocoder module available  	   */
  VOC_INFORM_MODULE_UNFETCHED, /* Vocoder module unavailable  	   */
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#ifdef FEATURE_QSYNTH_COMMON
  VOC_INFORM_QSYNTH,       /* Vocoder is in QSynth state           */
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  VOC_INFORM_QTUNES,       /* Vocoder is in QTunes state           */
#endif /* FEATURE_QTUNES_COMMON */
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
  VOC_INFORM_CAM_AUDIO_ACTIVE, 
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY*/
#ifdef FEATURE_GRAPH_AUDIO
  VOC_INFORM_GRAPH_IDLE,   /* Vocoder is in Graphics idle state    */
  VOC_INFORM_GRAPH_AUDIO,  /* Vocoder is in Graphics audio state   */
#endif /* FEATURE_GRAPH_AUDIO */
#ifdef FEATURE_VOC_ADPCM
  VOC_INFORM_VOCODER_ADPCM, /* Vocoder is in voc adpcm state       */
#endif /* FEATURE_VOC_ADPCM */
#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  VOC_INFORM_VOCODER_ADEC,   /* Vocoder is in voc+adec state       */
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
  VOC_INFORM_MIXER_VOC_IDLE,
  VOC_INFORM_MIXER_IDLE,
  VOC_INFORM_MIXER_ACTIVE,
  VOC_INFORM_MIXER_VOCODER,
#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  VOC_INFORM_MIXER_ADEC,
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB ||
          FEATURE_AVS_INCALL_MIXER_CMX */
  VOC_INFORM_MAX,

  /* DO NOT USE: Force this enum to be a 32bit type */
  VOC_INFORM_32BIT_DUMMY = VOC_DUMMY_DATA_UINT32_MAX
} voc_inform_type;

typedef enum {
  VOC_SERVICE_DEFAULT = 0,
  VOC_SERVICE_CDMA = VOC_SERVICE_DEFAULT,
  VOC_SERVICE_CDMA_WB,  
  VOC_SERVICE_UMTS,
  VOC_SERVICE_UMTS_WB,
  VOC_SERVICE_MAX
}voc_service_cfg_type;

/*---------------------------------------------------------------------------
                 DIRECTIONS OF VOCODER CLIENT CAPABILITY.  

Directions of a given capability. 
---------------------------------------------------------------------------*/
typedef enum{
VOC_CLIENT_RX_ONLY,
VOC_CLIENT_TX_ONLY,
VOC_CLIENT_BOTH_DIR
}voc_client_cap_dir_type;

/* A pointer to a function returning void, called with 1 parameter of
** type voc_inform_type.
*/
typedef void
  (* voc_state_cb_type)( voc_inform_type new_status );

/* Voice/Data Analog initialization spec. */
typedef enum {
  VOC_NORMAL_DEV,  
  VOC_DATA_DEV
} voc_dfm_deviation_type; 

/* Select the path for the PCM I/O
*/
typedef enum {
  VOC_CODEC_CLASS_INTERNAL,
#ifdef FEATURE_INTERNAL_SDAC
  VOC_CODEC_CLASS_INTERNAL_STEREO,
#endif /* FEATURE_INTERNAL_SDAC */
#ifdef FEATURE_BT
#ifdef FEATURE_BT_AG
  VOC_CODEC_CLASS_BLUETOOTH,
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  VOC_CODEC_CLASS_BLUETOOTH_A2DP,
  VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO,
#endif /* FEATURE_SBC_CODEC */
#endif /* FEATURE_BT */
  VOC_CODEC_CLASS_EXTERNAL,
#ifdef FEATURE_EXTERNAL_SDAC
  VOC_CODEC_CLASS_EXTERNAL_STEREO,
#endif /* FEATURE_EXTERNAL_SDAC */
#ifdef FEATURE_INTERNAL_SADC
  VOC_CODEC_CLASS_INTERNAL_SADC_HANDSET,
  VOC_CODEC_CLASS_INTERNAL_SADC_HEADSET,
#endif /* FEATURE_INTERNAL_SADC */
#ifdef FEATURE_EXTERNAL_SADC
  VOC_CODEC_CLASS_EXTERNAL_SADC_HANDSET,
  VOC_CODEC_CLASS_EXTERNAL_SADC_HEADSET,
#endif /* FEATURE_EXTERNAL_SADC */
  VOC_CODEC_CLASS_MAX,
  VOC_CODEC_CLASS_NONE
} voc_codec_class_type;

#ifdef FEATURE_PHONE_VR
typedef enum {
  VOC_CODEC_VR_TYPE_HANDSET,
  VOC_CODEC_VR_TYPE_HEADSET,
  VOC_CODEC_VR_TYPE_CARKIT,
  VOC_CODEC_VR_TYPE_MAX,
  VOC_CODEC_VR_TYPE_NONE
} voc_codec_vr_type;
#endif /* FEATURE_PHONE_VR */

/* Function pointer type of codec amplifier enable/disable function that
** gets registered with vocoder driver.
*/
typedef void (* voc_amp_enable_func_ptr_type) (
  voc_codec_type codec, voc_codec_mode_type mode, boolean enable );
/* Function pointer which is the CB function, when PAs are turned on,
** gets registered with vocoder driver.*/
typedef void (* voc_pa_on_cb_func_ptr_type) (void);

/* The various codec sampling rates that are supported
*/
typedef enum {
  VOC_SAMPLE_RATE_NONE,          /* Zero sampling rate, turn clocks off */
  VOC_SAMPLE_RATE_8000,          /* 8k      */
  VOC_SAMPLE_RATE_11025,         /* 11.025k */
  VOC_SAMPLE_RATE_12000,         /* 12k     */
  VOC_SAMPLE_RATE_16000,         /* 16k     */
  VOC_SAMPLE_RATE_22050,         /* 22.050k */
  VOC_SAMPLE_RATE_24000,         /* 24k     */
  VOC_SAMPLE_RATE_32000,         /* 32k     */
  VOC_SAMPLE_RATE_44100,         /* 44.1k   */
  VOC_SAMPLE_RATE_48000,         /* 48k     */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  VOC_SAMPLE_RATE_44000,         /* 44k     */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
  VOC_SAMPLE_RATE_UNKNOWN,       /* Unknown rate */
  VOC_SAMPLE_RATE_MAX = VOC_SAMPLE_RATE_UNKNOWN
} voc_sample_rate_type;

#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
typedef void (*voc_oemsbl_cb_func_ptr_type)(void);
#endif

#ifdef FEATURE_MM_REC
/*
 * Mono/Stereo Indication to the MultiMedia recording session.
 */
typedef enum {
  VOC_MM_CHAN_MONO,                        /* Mono   recording               */
  VOC_MM_CHAN_STEREO,                      /* Stereo recording               */
  VOC_MM_CHAN_INVALID                      /* Invalid                        */
  , VOC_MM_CHAN_U32 = VOC_DUMMY_DATA_UINT32_MAX
} voc_mm_rec_chan_type;

/*
 * Structure for passing MultiMedia record parameters from sound layer 
 * to vocoder
 */
typedef struct {
  uint16               bit_per_spl;        /* bit per Sample/Channel         */
  uint16               sample_rate_index;  /* sample rate index,             */
  uint16               rec_chan_index;     /* 0 - mono;  1 - stereo          */
} voc_mm_rec_parm_type;
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_GASYNTH_INTERFACE
typedef enum {  
  VOC_ADEC_VOC_QCP,
  VOC_ADEC_VOC_AMR
} voc_adec_voc_enum_type;
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
#ifdef FEATURE_MVS

/*---------------------------------------------------------------------------
                         FRAME PROCESSING
---------------------------------------------------------------------------*/
#if defined(MSMAUD_VOC_AMR_WB)
/* AMR_WB Uplink frame processing function definition
*/
typedef void (* voc_amr_wb_ul_fn_type)(
  uint8                    *vocoder_packet,
  voc_amr_wb_frame_type       voc_frame_type,
  voc_amr_wb_frame_rate_type  voc_amr_wb_mode
);

/* AMR_WB Downlink frame processing function definition
*/
typedef void (* voc_amr_wb_dl_fn_type)(
  uint8                    *vocoder_packet,
  voc_amr_wb_frame_type       *voc_frame_type,
  voc_amr_wb_frame_rate_type  *voc_amr_wb_mode
);
#endif /* defined(MSMAUD_VOC_AMR_WB) */

#if defined(MSMAUD_VOC_AMR)
/* AMR Uplink frame processing function definition
*/
typedef void (* voc_amr_ul_fn_type)(
  uint8                    *vocoder_packet,
  voc_gsm_frame_type       voc_frame_type,
  voc_gsm_frame_rate_type  voc_amr_mode
);

/* AMR Downlink frame processing function definition
*/
typedef void (* voc_amr_dl_fn_type)(
  uint8                    *vocoder_packet,
  voc_gsm_frame_type       *voc_frame_type,
  voc_gsm_frame_rate_type  *voc_amr_mode
);
#endif /* defined(MSMAUD_VOC_AMR) */

#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) */

#ifdef MSMAUD_VOC_HR
/* HR Uplink frame processing function definition
*/
typedef void (* voc_hr_ul_fn_type)(
  uint8                    *vocoder_packet,
  voc_gsm_frame_type       voc_frame_type,
  boolean                  enc_err_flag
);

/* HR Downlink frame processing function definition
*/
typedef void (* voc_hr_dl_fn_type)(
  uint8                    *vocoder_packet,
  voc_gsm_frame_type       *voc_frame_type,
  boolean                  *taf,
  boolean                  *bfi,
  boolean                  *ufi
);
#endif  /* MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711
/* G.711 Uplink frame processing function definition
*/
typedef void (* voc_g711_ul_fn_type)(
  uint8                    *vocoder_packet,
  uint16                   frame_length,
  voc_g711_mode_type       g711_mode
);

/* G.711 Downlink frame processing function definition
*/
typedef void (* voc_g711_dl_fn_type)(
  uint8                    *vocoder_packet,
  uint16                   *frame_length,
  voc_g711_mode_type       *g711_mode
);
#endif  /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
/* G.711 Uplink frame processing function definition
*/
typedef void (* voc_g723_ul_fn_type)(
  uint8                    *vocoder_packet,
  uint16                   frame_length,
  voc_g723_mode_type       g723_mode
);

/* G.711 Downlink frame processing function definition
*/
typedef void (* voc_g723_dl_fn_type)(
  uint8                    *vocoder_packet,
  uint16                   *frame_length,
  voc_g723_mode_type       *g723_mode
);
#endif  /* MSMAUD_VOC_G723 */

#endif  /* FEATURE_MVS */


#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
typedef void (*voc_bt_link_enabled_cb_type)(void);

#if defined(FEATURE_AVS_BT_SCO_REWORK)
typedef enum { 
  VOC_PLAYBACK_STOP              = 0, 
  VOC_PLAYBACK_OTHER             = (1 << 0), 
  VOC_PLAYBACK_OTHER_STOP        = (1 << 1),
  VOC_PLAYBACK_SCO               = (1 << 2), 
  VOC_PLAYBACK_A2DP              = (1 << 3),
  VOC_PLAYBACK_DTMF_START        = (1 << 4),
  VOC_PLAYBACK_DTMF_STOP         = (1 << 5),
  VOC_PLAYBACK_DTMF_RINGER_START = (1 << 6),
  VOC_PLAYBACK_DTMF_RINGER_STOP  = (1 << 7)
} voc_playback_enum_type; 

typedef void (*voc_bt_cb_func_ptr_type)(voc_playback_enum_type);

#endif /*AVS_BT_SCO_REWORK*/

#endif

/*Callback to notify QTV abt mod_disable event*/
typedef void (*voc_notify_qtv_cb_func_ptr_type)(uint32);

/* <EJECT> */
/*---------------------------------------------------------------------------
                       VOCODER PACKET INTERFACE
---------------------------------------------------------------------------*/
#ifdef FEATURE_VOC_PACKET_INTERFACE

/* Callbacks for packet clients */
/* Reverse Packet Interface */
typedef void (*voc_packet_rl_fn_type) (
  uint8          *vocoder_packet,     /* Pointer to data                 */
  voc_rate_type  current_rate,        /* Rate for current vocoder frame  */
  uint16         packet_length        /* Length of current packet        */
);

/* Forward Packet Interface */
typedef void (*voc_packet_fl_fn_type) (
  uint8          **vocoder_packet,     /* Pointer to data     */
  voc_rate_type  *rate                 /* Rate for this frame */
);

#endif /* FEATURE_VOC_PACKET_INTERFACE */


/*---------------------------------------------------------------------------
                       VOCODER PCM DATA INTERFACE
---------------------------------------------------------------------------*/
#ifdef FEATURE_VOC_PCM_INTERFACE

/* PCM Interfaces */
typedef enum {
  VOC_PCM_INTERFACE_RX_OUTPUT,  /* RX output */
  VOC_PCM_INTERFACE_RX_INPUT,   /* RX input  */
  VOC_PCM_INTERFACE_TX_OUTPUT,  /* TX output */
  VOC_PCM_INTERFACE_TX_INPUT,   /* TX input  */
  VOC_PCM_INTERFACE_MAX
} voc_pcm_interface_type;

/* PCM channel attributes:
   all samples are assumed to be 16-bit signed little endian by default
*/
typedef struct {
  voc_pcm_interface_type chan_id;  /* channel identifier */
  uint32 sample_rate;
  uint16 frame_len;                /* size of channel frame in no. of bytes */
} voc_pcm_chan_attrib_type;

typedef struct {
  voc_pcm_interface_type interleave_id;  /* channel ID to interleave */
  uint16 interleave_len;                 /* no. of bytes to interleave */
} voc_pcm_interleave_attrib_type;

/* This table describes how each frame is contributed by each channel. */
typedef struct {
  uint8 num_interleave;
  voc_pcm_interleave_attrib_type *interleave_tbl;
} voc_pcm_interleave_tbl_type;

typedef struct {
  voc_pcm_chan_attrib_type intf_dsc;      /* descriptor of interface itself */
  voc_pcm_interleave_tbl_type *interleave_dsc;  /* interleave descriptor */
} voc_pcm_interface_attrib_type;

/* Data status used by PCM input callbacks to indicate availability */
typedef enum {
  VOC_PCM_DATA_STATUS_AVAILABLE,    /* Data available for PCM input */
  VOC_PCM_DATA_STATUS_UNAVAILABLE,  /* Data not available           */
  VOC_PCM_DATA_STATUS_MAX
} voc_pcm_data_status_type;

/* Length of a frame of data in words */
/* TO ADD: length to be updated */
#define VOC_PCM_FRAME_LENGTH 160

#ifdef FEATURE_QDJ_TIMEWARP
/* TO ADD: length to be updated */
#define EVRC_PCM_FRAME_LENGTH 160
#endif /* FEATURE_QDJ_TIMEWARP */

/* Client callback interface for PCM output */
typedef void (*voc_pcm_client_output_fn_type) (
  const uint16  *pcm_data /* Always assume QDSP_PCM_DATA_FRAME_LEN sized array*/
);

/* Client callback interface for PCM input */
typedef voc_pcm_data_status_type (*voc_pcm_client_input_fn_type) (
  uint16  *pcm_data /* Always assume QDSP_PCM_DATA_FRAME_LEN sized array*/
);

#endif /* FEATURE_VOC_PCM_INTERFACE */

#ifdef FEATURE_MM_REC
typedef enum {
  VOC_MM_FORMAT_AAC,
  VOC_MM_FORMAT_MAX
} voc_mm_record_format_type;

/* Data status used by MultiMedia record callbacks to indicate availability */
typedef enum {
  VOC_MM_DATA_STATUS_AVAILABLE,      /* Data available for MM input         */
  VOC_MM_DATA_STATUS_UNAVAILABLE,    /* Data not available                  */
  VOC_MM_DATA_STATUS_MAX
  , VOC_MM_DATA_STATUS_U32 = VOC_DUMMY_DATA_UINT32_MAX
} voc_mm_data_status_type;

#ifdef FEATURE_AAC_REC
/* Maximum length of a frame of data in words */
#define VOC_AAC_FRAME_DATA_LENGTH     768
#define VOC_AAC_ENC_MAX_FRAME         8

/* Structure for each recorded AAC frame information */
typedef struct {
  uint32 frame_cnt;                         /* Frame number encoded           */
  uint32 data_length;                       /* Data length in this frame      */
} voc_aac_record_frame_type;

/* Data holder to get data from DSP for AAC record */
typedef struct {
  voc_aac_record_frame_type buf1_frame_info[VOC_AAC_ENC_MAX_FRAME]; 
                                 /* Holder for each frame info in buffer1    */
  uint16 buf1_frame_num;         /* Number of frames recorded in buffer1     */
  uint16 lost_frame1;            /* lost frame number in buffer1 recorded    */
  boolean buf1_flush;            /* Wether need to send buffer to cmx        */
  boolean buf1_avsync;           /* A/V sync report required                 */
  uint32  samples_rpt1;          /* A/V sync report samples for buffer1      */
  uint32  sync_frame1;           /* Frame number need to report              */

  voc_aac_record_frame_type buf2_frame_info[VOC_AAC_ENC_MAX_FRAME]; 
                                 /* Holder for each frame info in buffer2    */
  uint16 buf2_frame_num;         /* Number of frames recorded in buffer2     */
  uint16 lost_frame2;            /* lost frame number in buffer2 recorded    */
  boolean buf2_flush;            /* Wether need to send buffer to cmx        */
  boolean buf2_avsync;           /* A/V sync report required                 */
  uint32  samples_rpt2;          /* A/V sync report samples for buffer2      */
  uint32  sync_frame2;           /* Frame number need to report              */
} voc_aac_record_data_type;

/* Available Buffer information for AAC data record */
typedef struct {
  uint8  *buf1;                             /* Buffer1 starting address       */
  uint32 buf1_space;                        /* Available space of buffer1     */

  uint8  *buf2;                             /* Buffer2 starting address       */
  uint32 buf2_space;                        /* Available space of buffer2     */

  uint16 hdr_offset;                        /* Format header offset           */
  uint32 sync_samples;                      /* A/V sync required sync samples */
  uint32 sync_frames;                       /* Reported A/V sync frame number */
} voc_aac_record_buf_type;
#endif /* FEATURE_AAC_REC */

/* Data holder to get data from DSP during MultiMedia recording */
typedef union {
  uint32 dummy_data;
#ifdef FEATURE_AAC_REC
  voc_aac_record_data_type aac_buf; /* Recorded AAC data              */
#endif /* FEATURE_AAC_REC */
} voc_mm_record_data_type;

/* Buffer  holder to get data from DSP during MultiMedia recording */
typedef union {
  uint8 *dummy_buf;
#ifdef FEATURE_AAC_REC
  voc_aac_record_buf_type aac_buf;  /* Recorded AAC data              */
#endif /* FEATURE_AAC_REC */
} voc_mm_record_buf_type;

/* Client callback interface for MultiMedia output from DSP */
typedef void (* voc_mm_rec_cb_fn_type) (void);
#endif /* FEATURE_MM_REC */

/* Aux Pcm modes that are supported....*/
#ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
#endif
/*---------------------------------------------------------------------------
                             VOCODER SAT VALUES
---------------------------------------------------------------------------*/

/* SAT control values */
typedef enum {
  VOC_SAT_ON,
  VOC_SAT_OFF
} voc_sat_ctrl_type;

/* The allowable SCC values */
typedef enum {
  VOC_SCC_5970,
  VOC_SCC_6000,
  VOC_SCC_6030,
  VOC_SCC_INVALID
} voc_scc_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
                         AUDIO  PROCESSING  MODES
---------------------------------------------------------------------------*/

/* Select the Echo Cancellation mode
*/
typedef enum {
  VOC_EC_OFF = 0,
  VOC_EC_ESEC,
  VOC_EC_HEADSET,
  VOC_EC_AEC,
  VOC_EC_SPEAKER,
  VOC_EC_BT_HEADSET,
  VOC_EC_DEFAULT
} voc_ec_type;

/* Select the Noise Suppressor mode
*/
typedef enum {
  VOC_NS_OFF = 0,
  VOC_NS_ON,
  VOC_NS_DEFAULT
} voc_ns_type;

/* EC/NS override mode
*/
typedef enum {
  VOC_EC_NS_FORCE_DISABLED,    /* Force EC/NS to be disabled   */
  VOC_EC_NS_NORMAL_OPERATION   /* Have EC/NS operate as normal */
} voc_ec_ns_mode_type;

/* VOX (half/full duplex voice) mode
*/
typedef enum {
  VOC_VOX_OFF = 0,
  VOC_VOX_ON,
  VOC_VOX_DEFAULT
} voc_vox_type;

/* VOX override mode
*/
typedef enum {
  VOC_VOX_FORCE_ENABLED,     /* Force VOX to be enabled   */
  VOC_VOX_NORMAL_OPERATION   /* Have VOX operate as normal */
} voc_vox_mode_type;

/* Sidetone mode
*/
typedef enum {
  VOC_SIDETONE_OFF = 0,
  VOC_SIDETONE_ON,
  VOC_SIDETONE_DEFAULT
} voc_sidetone_type;

/* sidetone override mode
*/
typedef enum {
  VOC_SIDETONE_FORCE_DISABLED,     /* Force sidetone to be disabled   */
  VOC_SIDETONE_NORMAL_OPERATION   /* Have sidetone operate as normal */
} voc_sidetone_mode_type;

/* HSSD circuit control
*/
typedef enum {
  VOC_HSSD_DISABLED,
  VOC_HSSD_ENABLED
} voc_hssd_type;

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*---------------------------------------------------------------------------
                      VOICE RECOGNITION MODES
---------------------------------------------------------------------------*/
typedef enum {
  VOC_VR_MODE_OFF = 0,   /* No VR functions enabled      */
  VOC_VR_MODE_BC,        /* Enable MODE B/C VR front end */
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
  VOC_VR_MODE_MAX
} voc_vr_mode_type;

/*===========================================================================

         V O I C E   R E C O G N I T I O N   C A L I B R A T I O N
               
===========================================================================*/

/* Coefficients for the Q14 signed 13 tap FIR filter
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
} voc_pcm_filter_type;

/* Bark Amp weights of inverse filter (in Q10) for mode B narrowband and 
** wideband VR. 
*/
typedef struct {
  const unsigned short      *narrowband;
  const unsigned short      *wideband;
} voc_barkamp_weight_type;


/*  Vocoder parameters that can be modified when doing VR Capture
*/
typedef struct {
  voc_pcm_filter_type  tx_filter;  /* Encoder PCM filter coefficients         */
  uint16               tx_gain;    /* Microphone gain for capture             */
  voc_ec_type          ec_mode;    /* Echo cancellation mode                  */
  voc_ns_type          ns_enable;  /* Noise suppressor enable                 */
  voc_barkamp_weight_type  barkamp_weight;
} voc_vr_cal_type;

#endif /* FEATURE_PHONE_VR */


/* <EJECT> */
/*-------------------------------------------------------------------------
** Keep track of the desired configuration for each client
**-----------------------------------------------------------------------*/
typedef struct {
  voc_capability_type  config;
  boolean              standby_flag;
} voc_client_config_type;


/* <EJECT> */
#ifdef FEATURE_AUDIO_AGC
/*---------------------------------------------------------------------------
                      VOCODER AUDIO AGC SETTINGS

voc_agc_cmd is passed an AGC mode enum containing settings to activate and
deactivate the AGC features.
---------------------------------------------------------------------------*/
typedef enum {
  VOC_AGC_NONE = 0,               /* No AGC features enabled   */
  VOC_AGC_ALL,                    /* All AGC features enabled  */
  VOC_AGC_NO_AVC,                 /* AGC enabled, AVC disabled */
  VOC_AGC_INVALID                 /* Undefined or invalid      */
} voc_agc_type;
#endif




typedef enum {
    VOC_SMARTSND_NONE,           /* No AGC feature enabled         */
    VOC_SMARTSND_AGC_AVC_AIG,    /* AGC, AVC enabled with AIG      */
    VOC_SMARTSND_AGC_AVC,        /* AGC, AVC enabled without AIG   */
    VOC_SMARTSND_AVC,            /* AVC enabled without AGC        */
    VOC_SMARTSND_AGC_AIG,        /* AGC with AIG enabled.          */
    VOC_SMARTSND_AGC,            /* AGC without AIG enabled        */
    VOC_SMARTSND_RVE_NONE,       /* RVE is disabled.               */
    VOC_SMARTSND_RVE_AVAIL,      /* RVE is enabled.                */
    VOC_SMARTSND_INVALID         /* Undefined or Invalid           */
} voc_smartsnd_type;


/* <EJECT> */
#ifdef FEATURE_TTY
/*---------------------------------------------------------------------------
                               TTY  MODES
---------------------------------------------------------------------------*/
typedef enum {
  VOC_TTY_MODE_OFF = 0,
  VOC_TTY_MODE_ON,
  VOC_TTY_MODE_FULL = VOC_TTY_MODE_ON,
  VOC_TTY_MODE_VCO,
  VOC_TTY_MODE_HCO,
  VOC_TTY_MODE_MAX
} voc_tty_mode_type;

#ifdef FEATURE_TTY_ACTIVITY_STATUS
typedef void (*voc_tty_status_fn_type) (
  boolean encoder_status,     /* TRUE if encoder is active, FALSE otherwise */
  boolean decoder_status      /* TRUE if decoder is active, FALSE otherwise */
);
#endif

#endif /* FEATURE_TTY */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* This type is used to indicate the type of audio decoder that should be used.
*/
typedef enum {
#ifdef FEATURE_QSYNTH_COMMON
  VOC_ADEC_MODE_QSYNTH,      /* QSynth audio decoder mode */
#endif
#ifdef FEATURE_QTUNES_COMMON
  VOC_ADEC_MODE_QTUNES,      /* QTunes audio decoder mode */
#endif
#ifdef FEATURE_ADPCM
  VOC_ADEC_MODE_ADPCM,       /* ADPCM audio decoder mode  */
#endif
#ifdef FEATURE_PCM
  VOC_ADEC_MODE_PCM,         /* PCM audio decoder mode    */
#endif
#ifdef FEATURE_VOC_G711
  VOC_ADEC_MODE_WAV_MULAW,   /* WAV audio decoder mode    */
  VOC_ADEC_MODE_WAV_ALAW,    /* WAV audio decoder mode    */
#endif /* FEATURE_VOC_G711 */
  VOC_ADEC_MODE_MAX
} voc_adec_mode_type;

/* This structure is used to transfer audio decoder data to the DSP.
*/
typedef struct {
  uint8  *buf1;      /* Data buffer 1                      */
  uint32 length1;    /* Length, in bytes, of Data buffer 1 */
  uint8  *buf2;      /* Data buffer 2                      */
  uint32 length2;    /* Length, in bytes, of Data buffer 2 */
#ifdef FEATURE_GASYNTH_INTERFACE
  uint8  *buf_usage_mask; /* Mask to indicate which buffer(s) 
                            are in used by DSP */
#endif /* FEATURE_GASYNTH_INTERFACE */
} voc_adec_cmd_type;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

#ifdef FEATURE_QSYNTH_COMMON
/* This type is used to indicate the type of QSynth audio decoder that should
** be used.
*/
typedef enum {
  VOC_QSYNTH_ADEC_MODE_ADPCM,              /* QSynth ADPCM audio decoder    */
  VOC_QSYNTH_ADEC_MODE_YADPCM,             /* QSynth Y-ADPCM audio decoder  */
  VOC_QSYNTH_ADEC_MODE_QCP                 /* QSynth QCELP audio decoder    */
#ifdef FEATURE_QSYNTH_PCM
  , VOC_QSYNTH_ADEC_MODE_PCM               /* QSynth PCM audio decoder      */
#endif /* FEATURE_QSYNTH_PCM */
} voc_qsynth_adec_mode_type;
#endif /* FEATURE_QSYNTH_COMMON */

#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/* Type used to indicate the source of the ADPCM file. The method used to
** start the ADPCM file will dictate the behaviour of the vocoder driver.
*/
typedef enum {
  VOC_ADPCM_PLAY_FILE,     /* ADPCM file is started with play command   */
  VOC_ADPCM_PLAY_AUDFADEC  /* ADPCM file is started via audfadec module */
} voc_adpcm_play_enum_type;

/* This type is used to indicate the format of the audio decoder data */
typedef enum {
  VOC_ADPCM_ADPCM_MODE,         /* ADPCM format         */
  VOC_ADPCM_YADPCM_MODE,        /* Y-ADPCM format       */
  VOC_ADPCM_SIGNED_PCM_MODE,    /* Signed PCM format    */
  VOC_ADPCM_UNSIGNED_PCM_MODE,  /* Unsigned PCM format  */
#ifdef FEATURE_VOC_G711
  VOC_ADPCM_WAV_ALAW_MODE,      /* Wav, A-Law format    */
  VOC_ADPCM_WAV_MULAW_MODE,     /* Wav, Mu-Law format   */
#endif /* FEATURE_VOC_G711 */
  VOC_ADPCM_INVALID_MODE
} voc_adpcm_mode_enum_type;

/* This structure is used to setup audio decoder with all the required
** information.
*/
typedef struct {
  uint16                   bits_per_sample;  /* Number of bits per sample */
  uint16                   block_length;     /* Block length for ADPCM    */
  uint16                   sampling_rate;    /* Sampling rate             */
  uint16                   num_channels;     /* Number of channels        */
  voc_adpcm_mode_enum_type type;             /* Format of audio decoder   */
} voc_adpcm_spec_type;

/* This type is used to indicate the start mode for ADEC playback.
*/
typedef enum {
  VOC_ADEC_START_MODE_NORMAL,  /* Normal start - start playback immediately  */
  VOC_ADEC_START_MODE_DELAYED, /* Delayed start - load file and wait for cmd */
  VOC_ADEC_START_MODE_INVALID
} voc_adec_start_mode_enum_type;

/* enum type is used do differeniate what type PCM/ADPCM supported on DSP 
 * images 
 *
 * Based on current implementation:
 *
 *
 * ---------------------------------------------------------------------------- 
 * image        |  VFE/Vocoder   | Camcorder/MP4E    | G711    | Synth
 *-----------------------------------------------------------------------------
 * channel      |  1             |  1                |  1      | 1 and 2
 *-----------------------------------------------------------------------------
 * Bits/Sample  |  4             |  4                |  8      | 4, 8 ,16
 *-----------------------------------------------------------------------------
 * SampleRate   |  32 KHz        |  8 KHz            |  8 KHz  | 8 - 48 KHz
 *-----------------------------------------------------------------------------
 * BlockSize    |  512           |  256              |  1      | 
 *-----------------------------------------------------------------------------
 * */
typedef enum {
  VOC_ADPCM_VALID_NONE,

  VOC_ADPCM_VALID_SYNTH,
  VOC_ADPCM_VALID_8K,
  VOC_ADPCM_VALID_32K,
  VOC_ADPCM_VALID_G711,

  VOC_ADPCM_VALID_MAX
} voc_adpcm_valid_type;

#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_AAC
typedef enum {
  VOC_AAC_DATA_FORMAT_ADIF,
  VOC_AAC_DATA_FORMAT_ADTS,
  VOC_AAC_DATA_FORMAT_RAW,
  VOC_AAC_DATA_FORMAT_PSUEDO_RAW,
  VOC_AAC_DATA_FORMAT_LOAS
} voc_aac_data_format_enum_type;

typedef struct {
  uint16                         buffer_factor;
  voc_aac_data_format_enum_type  aac_format;
  /* See ISO/IEC 14496-3:2001(E) for details on the parameters below */
  byte                           sample_freq_index;
  byte                           audio_object;
  byte                           channel_configuration;
  byte                           ep_config;
  byte                           aac_section_data_resilience_flag;
  byte                           aac_scalefactor_data_resilience_flag;
  byte                           aac_spectral_data_resilience_flag;
#ifdef FEATURE_AAC_REC
  uint16                         bit_per_spl;
#endif /* FEATURE_AAC_REC */
} voc_aac_config_type;
#endif /* FEATURE_AAC */

#ifdef FEATURE_MP3
/* Structure used to pass the decoder configuration to the DSP.
*/
typedef struct {
  uint32 sample_freq;       /* Valid sample frequency in Hz        */
} voc_mp3_config_type;
#endif /* FEATURE_mp3 */

#ifdef FEATURE_REAL_AUDIO
/* Song specific DSP configuration for RealAudio */
typedef struct {
  byte                decoder_mode;      /* Decoder (Gecko) mode     */
  byte                sample_rate;       /* Sampling rate            */
  byte                num_channels;      /* Number of channels       */
  uint16              samples_per_frame; /* Samples per frame     */
  uint16              num_regions;       /* Number of regions        */
  uint16              bits_per_frame;    /* Number of bits per frame */
  uint16              coupling_start;    /* Stereo coupling region   */
  uint16              coupling_quant;    /* Coupling quantization    */
  uint16              frame_erasure;     /* Frame erasure flag       */
} voc_ra_dsp_config_type;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
/* Structure used to pass the decoder configuration to the DSP.
*/
typedef struct {
  byte version;             /* Only version 2 is supported         */   
  uint8  bits_per_sample;      /* Valid bits per sample             */
  uint16 channels;              /* number of channels 1 or 2          */
  uint16 bytes_per_second;  /* bytes per second for playing back   */
  uint16 sample_rate;       /* Valid sample frequency in Hz        */
  uint16 encoder_options;   /* Encoder option                      */
  uint16 virtual_pkt_len;   /* VirtualPacketLength in ASF          */
  uint16 advanced_encoder_opt1; /* Support for LBR bitstream */
  uint16 channel_mask;         /* For stero or mono streams  */
  uint16 format_tag;           /* Codec ID: 0x162 or 0x166 for WMAPRO 10 */
  uint32 advanced_encoder_opt2; /* Support for LBR bitstream */

} voc_wma_config_type;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
/* AMR-WB+ decoder configuration */
typedef struct {
  uint16 sample_rate;   /* Sampling rate in Hz  */
  uint16 dec_chan_mode; /* Decoder channel mode */
} voc_amr_wbp_config_type;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

#ifdef FEATURE_MIDI
/* The following type is used for setting up the volume level for the qsynth
** image. when it is playing Webaudio or SMAF file.
*/
typedef enum {
  VOC_QSYNTH_DEFAULT_VOL_LVL = 0,  /* Default volume level for MIDI/WEBAUDIO */
  VOC_QSYNTH_SMAF_VOL_LVL_MA2=1,    /* Volume level for SMAF   */
  VOC_QSYNTH_SMAF_VOL_LVL_MA3=1,
  VOC_QSYNTH_SMAF_VOL_LVL_MA5=2  /*volume level for Ma5 Smaf*/
} voc_qsynth_vol_mode_enum_type;
#endif /* FEATURE_MIDI */

#if defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT)
typedef struct {
  uint16 num_words;          /* Number of total words in command buffer  */
  uint16 num_commands;       /* Number of commands in command buffer     */
  uint16 *command_buf;       /* Buffer of commands                       */
} voc_midi_cmd_type;
#endif /* FEATURE_MIDI || FEATURE_MIDI_OUT */

#if defined(FEATURE_AUDIO_FORMAT) && \
    (defined(FEATURE_AVS_INCALL_ADEC_PB) || \
     defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)) 
/* Audio playback decoder mixing selection */
typedef enum {
  VOC_ADEC_MIX_MODE_NONE,  /* No mixing - playback on local speaker only */
  VOC_ADEC_MIX_MODE_RX,    /* Mix Playback output + RX voice call output */
  VOC_ADEC_MIX_MODE_MAX
} voc_adec_mix_mode_type;
#endif /* FEATURE_AUDIO_FORMAT && 
          (FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB) */

#ifdef FEATURE_AUDFMT_EQUALIZER
/* This defined the total number of Equalizer IIR filter supported by the
** DSP.
*/
#define VOC_EQ_MAX_BANDS_NUM 12

/* This defined the total number of bands are used for preset coefficient
** The preset value will not be change by the user API. The preset coefficient
** can only be access through voc_set_preset_eq_coeff(). The remaining EQ bands
** will be access by the USER throught the public Equalizer API.
*/
#define VOC_PRESET_EQ_MAX_NUM 0

#if (VOC_PRESET_EQ_MAX_NUM > VOC_EQ_MAX_BANDS_NUM)
#error "Preset Equalizer bands is more than maximum number"
#endif

/* This defiend the total number of equalizer bands
*/
#define VOC_EQ_BANDS_NUM (VOC_EQ_MAX_BANDS_NUM - VOC_PRESET_EQ_MAX_NUM)

/* The type specifications for equalizer's filter boost type
** Need to be insyn with the snd_equalizer_filter_boost_enum_type
*/
typedef enum {
  VOC_EQUALIZER_BASE_BOOST   = 1,    /* Bass Boost/Cut                    */
  VOC_EQUALIZER_TREBLE_BOOST = 2,    /* Treble Boost/Cut                  */
  VOC_EQUALIZER_BAND_BOOST   = 3     /* Band Boost/Cut                    */
} voc_equalizer_filter_boost_enum_type;

/* This structure is same as snd_equalizer_filter_type.
** Used to store the filter paramater.
*/
typedef struct {
  int16                         gain; /* Filter band initial gain          */
  uint16                        freq; /* Filter band cut-off/center freq   */
  voc_equalizer_filter_boost_enum_type 
                                type; /* Filter band type Band/Treble/Base */
  uint16                        qf;   /* Band boost quality factor         */
} voc_equalizer_filter_type;

#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/* Callback function to indicate that new spectrum analyzer data is available
*/
typedef void (* voc_spectrum_analyzer_cb_type) (
  const uint16 *buffer
);
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#ifdef FEATURE_AUDIO_FORMAT
/* This type is used to specify the type of codec for AV sync function */
typedef enum {
  VOC_AV_SYNC_DEC_MP3,        /* MP3 decoder       */
  VOC_AV_SYNC_DEC_AAC,        /* AAC decoder       */
  VOC_AV_SYNC_DEC_RA,         /* RealAudio decoder */
  VOC_AV_SYNC_DEC_VOCODER,    /* Vocoder           */
  VOC_AV_SYNC_DEC_WMA,        /* Vocoder           */
  VOC_AV_SYNC_DEC_WAVE,       /* PCM/ADPCM decoder */
  VOC_AV_SYNC_DEC_AMR_WB,     /* AMR-WB decoder    */
  VOC_AV_SYNC_DEC_AMR_WBPLUS, /* AMR-WB+ decoder   */
  VOC_AV_SYNC_DEC_INVALID
} voc_av_sync_dec_enum_type;

/* The following is used to indicate the type of AV Sync interval */
typedef enum {
  VOC_AV_SYNC_SAMPLE,     /* av sync interval in samples             */
  VOC_AV_SYNC_MS,         /* av sync interval in milliseconds        */
  VOC_AV_SYNC_INVALID     /* Designates an invalid type              */
} voc_av_sync_interval_type;

/* A/V Sync callback function */
typedef void (*voc_av_sync_cb_type)(
  qword      timestamp,      /* Timestamp for the callback                  */    
  qword      num_of_samples, /* PCM sample counter. Use only lower 48 bits  */
  qword      num_of_bytes    /* Bytes      counter. Use only lower 48 bits  */
);
#endif /* FEATURE_AUDIO_FORMAT */

#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE) 
/* Callback function to indicate that new PP data from DSP is available 
*/
typedef void (* voc_pp_cb_type )(
  uint16 buf_size, uint16* buffer, uint16 buf_pool_index);

/* Indicates PP status
*/
typedef enum {
  VOC_PP_STATUS_DISABLED,
  VOC_PP_STATUS_ENABLED,
}voc_pp_status_type;

typedef enum {
#ifdef FEATURE_SBC_CODEC
  VOC_PP_TYPE_SBC,
#endif /* FEATURE_SBC_CODEC */
#ifdef FEATURE_SBC_DSP_CODEC
  VOC_PP_TYPE_DSP_SBC,
#endif
  VOC_PP_TYPE_GENERIC
}voc_pp_type_type;

#endif /* FEATURE_ARM_POST_PROCESSING  || FEATURE_AVS_WB_PCM_RX_INTERFACE */
#ifdef FEATURE_SBC_DSP_CODEC

/* Enums for SBC configuration */

typedef enum {
  VOC_SBC_SUBBANDS_8,    
  VOC_SBC_SUBBANDS_MAX
}voc_sbc_subbands_type;

typedef enum {
  VOC_SBC_BLOCKS_4,     
  VOC_SBC_BLOCKS_8,     
  VOC_SBC_BLOCKS_12,
  VOC_SBC_BLOCKS_16,
  VOC_SBC_BLOCKS_MAX
}voc_sbc_blocks_type;

typedef enum {
  VOC_SBC_MODE_INVALID,
  VOC_SBC_MODE_MONO,
  VOC_SBC_MODE_DUAL,
  VOC_SBC_MODE_STEREO,
  VOC_SBC_MODE_JT_STEREO,
  VOC_SBC_MODE_MAX
}voc_sbc_mode_type;

typedef enum {
  VOC_SBC_ALLOC_SNR,
  VOC_SBC_ALLOC_LOUDNESS,
  VOC_SBC_ALLOC_MAX
}voc_sbc_alloc_type;

typedef struct {
  voc_sbc_blocks_type blocks;
  voc_sbc_mode_type   mode;
  voc_sbc_alloc_type  alloc;
}voc_sbc_config_params_type;
#endif

/* The following is used for aux line in status */
typedef enum {
  VOC_AUX_LINE_IN_MODE_ON,     
  VOC_AUX_LINE_IN_MODE_OFF
} voc_aux_line_in_mode_type;

typedef struct {
  uint16   expansion_ratio_level;
} voc_slowtalk_params_type;

/* <EJECT> */
/*===========================================================================

      C O N F I G U R A T I O N   A N D   I N I T I A L I Z A T I O N
                            F U N C T I O N S
  
===========================================================================*/

#ifdef FEATURE_VOC_PACKET_INTERFACE
/*===========================================================================

FUNCTION VOC_REGISTER_PACKET_CLIENTS

DESCRIPTION
  Clients that use the packet interface to send/receive data must
  register callbacks using this function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_register_packet_clients (
  voc_packet_rl_fn_type  rl_func,     /* Reverse Link Client function */
  voc_packet_fl_fn_type  fl_func      /* Forward Link Client function */
);
/*~ FUNCTION voc_register_packet_clients */
#endif /* FEATURE_VOC_PACKET_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_clients_requested_cap(voc_capability_type)

DESCRIPTION
  Determines what all clients have requested a particular capability.
  
DEPENDENCIES
  None

RETURN VALUE
  client mask

SIDE EFFECTS
  None

===========================================================================*/
extern uint16 voc_clients_requested_cap(voc_capability_type cap);

/* <EJECT> */
/*=============================================================================

FUNCTION voc_clients_requested_cap_dir(voc_capability_type)

DESCRIPTION
  Determines the effective direction of all the clients for a given capability. 
  
DEPENDENCIES
  None

RETURN VALUE
  voc_client_cap_dir_type

SIDE EFFECTS
  None

=============================================================================*/
voc_client_cap_dir_type voc_clients_requested_cap_dir
(voc_capability_type cap);
/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_curr_config

DESCRIPTION
  Return the current vocoder configuration.  No implications about current
vocoder state which is able to be retrieved by function voc_get_status.

DEPENDENCIES
  None

RETURN VALUE
  voc_capability_type

SIDE EFFECTS
  None

===========================================================================*/
extern voc_capability_type voc_get_curr_config(void);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_ACQUIRE

DESCRIPTION
  Acquire the vocoder for use.  This takes it out of powerdown.
  This does not acquire exclusive access, just access.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_acquire (
  voc_acquire_type   key,
  voc_state_cb_type  cb_ptr
);
/*~ FUNCTION voc_acquire */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_RELEASE

DESCRIPTION
  Release the vocoder, no longer in use.  This puts it into powerdown,
  if it is no longer being used by any user.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void   voc_release (
  voc_acquire_type key   /* What entity wants to release the Vocoder */
);
/*~ FUNCTION voc_release */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CONFIG

DESCRIPTION
  This function sets the Vocoder to a specific configuration option.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_config ( 
  voc_acquire_type   key,
  voc_capability_type option
);
/*~ FUNCTION voc_config */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CONFIG_STANDBY

DESCRIPTION
  This function sets the Vocoder to a specific configuration option, but
  remains in IDLE state, with appropriate firmware loaded onto the DSP.
  voc_config() must be called subsequently to make the system transition
  to vocoder state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_config_standby ( 
  voc_acquire_type    key,
  voc_capability_type option
);
/*~ FUNCTION voc_config_standby */

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

FUNCTION VOC_SET_VR_MODE

DESCRIPTION
  This function enables or disables Voice Recognition capabilities

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_set_vr_mode ( 
  voc_acquire_type   key,
  voc_vr_mode_type   mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_shared_mem_ptr

DESCRIPTION
  This function returns a pointer to the shared RAM area between the vocoder
  driver and the VR engine.  The vocoder shared RAM area is limited to two
  buffers used in voice recording.  The purpose of sharing RAM is to eliminate
  RAM usage for the recording buffers.  By sharing RAM, voice recording (for
  voice memo only at this point) and voice recognition can not be running 
  concurrently.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  Pointer to shared RAM area.

SIDE EFFECTS
  None

===========================================================================*/

extern void *voc_get_shared_mem_ptr (void);

#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
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
  Note that this function is invoked only when TTY mode is enabled using
  voc_set_tty_mode().
    
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void voc_register_tty_status_client (
  voc_tty_status_fn_type tty_func  /* Callback for TTY activity status */
);
#endif /* FEATURE_TTY_ACTIVITY_STATUS */

/*===========================================================================

FUNCTION VOC_SET_TTY_MODE

DESCRIPTION
  This function enables or disables TTY capabilities.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S       - Command was successful.
  VOC_CMD_FAILED_S - Command could not be executed.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_set_tty_mode ( 
  voc_tty_mode_type tty_mode
);
#endif /* FEATURE_TTY */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_INIT

DESCRIPTION
  This function intializes the vocoder in various stages from releasing
  reset to intializing the time offsets, filter coefficients and background
  noise estimate.

DEPENDENCIES
  ?

RETURN VALUE
  TRUE when vocoder fully initialized, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean voc_init ( void );
/*~ FUNCTION voc_init */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_VOL_CMD

DESCRIPTION
  This function sets the internal vocoder gain.

DEPENDENCIES
  Must be done a few frames after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
The vocoder rx gain is adjusted.

===========================================================================*/
extern voc_status_type voc_vol_cmd ( sint15 volume );

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_TX_CMD

DESCRIPTION
  This function gets the current vocoder frame and sets the rate for the
  next frame.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  Data rate for current vocoder frame.

SIDE EFFECTS
  Rate of next frame is set.

===========================================================================*/
extern voc_rate_type voc_tx_cmd
(
  voc_rate_type max_rate,             /* Max rate for vocoder next frame */
  voc_rate_type min_rate,             /* Min rate for vocoder next frame */
  byte **data_ptr                     /* Pointer to pointer to data */
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_RX_CMD

DESCRIPTION
  This function gives the current vocoder frame to the vocoder.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_rx_cmd (
  byte rate,                          /* rate of vocoder frame */
  byte *data_ptr                      /* pointer to frame data */
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_FRAME_CMD

DESCRIPTION
  This function sets the rate of the next frame for the vocoder.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_frame_cmd (
  voc_rate_type max_rate,       /* max rate for vocoder next frame */
  voc_rate_type min_rate        /* min rate for vocoder next frame */
);

#ifdef FEATURE_VOC_PACKET_INTERFACE
/* <EJECT> */
/*===========================================================================

FUNCTION VOC_SET_FRAME_RATE_CMD

DESCRIPTION
  This function sets the rate of the next frame for the vocoder.
  Clients using the packet interface must use this function instead
  of voc_frame_cmd().

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_set_frame_rate_cmd (
  voc_rate_type max_rate,       /* max rate for vocoder next frame */
  voc_rate_type min_rate        /* min rate for vocoder next frame */
);
/*~ FUNCTION voc_set_frame_rate_cmd */
#endif /* FEATURE_VOC_PACKET_INTERFACE */

/* <EJECT> */
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
extern voc_status_type voc_gsm_tx_packet(
  uint16 *data_ptr,
  voc_gsm_frame_type *frame_type,
  voc_gsm_frame_rate_type *frame_rate,
  voc_gsm_frame_rate_type next_frame_rate           /* only for AMR */
);


/* <EJECT> */
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
extern voc_status_type voc_gsm_rx_packet(
  uint16 *data_ptr,
  voc_gsm_frame_type frame_type,
  voc_gsm_frame_rate_type frame_rate,       /* only for AMR */
  boolean taf             /* time alignment flag, only for GSM FR or EFR */
);
#endif   /* MSMAUD_VOC_AMR || MSMAUD_VOC_EFR || MSMAUD_VOC_FR */

/* <EJECT> */
#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_AMR_WB_COMMON)

/*===========================================================================

FUNCTION VOC_SET_GSM_ENC_DTX

DESCRIPTION
  This function will turn encoder DTX on or off.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_set_gsm_enc_dtx(
  voc_gsm_dtx_mode_type dtx_mode
);
#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR) || MSMAUD_VOC_HR || MSMAUD_VOC_AMR_WB_COMMON */

/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_WB_COMMON
/*===========================================================================

FUNCTION VOC_INIT_AMR_WB_TX_RATE

DESCRIPTION
  This function sets the initial encoder rate for the AMR_WB vocoder.
  Maybe used between frames to set the encoder rate.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_init_amr_wb_tx_rate(
  voc_amr_wb_frame_rate_type amr_wb_tx_rate
);
#endif /* MSMAUD_VOC_AMR_WB_COMMON */

/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_COMMON
/*===========================================================================

FUNCTION VOC_INIT_AMR_TX_RATE

DESCRIPTION
  This function sets the initial encoder rate for the AMR vocoder.
  Maybe used between frames to set the encoder rate.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_init_amr_tx_rate(
  voc_gsm_frame_rate_type amr_tx_rate
);
#endif /* MSMAUD_VOC_AMR_COMMON */

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
extern voc_status_type voc_set_g711_mode(
  voc_g711_mode_type g711_mode
/*~ FUNCTION voc_set_g711_mode */
);
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
extern voc_status_type voc_set_g723_mode(
  voc_g723_mode_type g723_mode
);
#endif /* MSMAUD_VOC_G723 */

/* <EJECT> */
#ifdef FEATURE_MVS
#if defined(MSMAUD_VOC_AMR_WB)
/*===========================================================================

FUNCTION voc_amr_wb_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing of AMR_WB packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_amr_wb_register(
  voc_amr_wb_ul_fn_type  uplink_func,
  voc_amr_wb_dl_fn_type  downlink_func
);
#endif /* defined(MSMAUD_VOC_AMR_WB) */

/* <EJECT> */
#if defined(MSMAUD_VOC_AMR)
/*===========================================================================

FUNCTION voc_amr_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing of AMR packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_amr_register(
  voc_amr_ul_fn_type  uplink_func,
  voc_amr_dl_fn_type  downlink_func
);
#endif /* defined(MSMAUD_VOC_AMR) */

/* <EJECT> */
#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) */

/* <EJECT> */
#ifdef MSMAUD_VOC_HR
/*===========================================================================

FUNCTION voc_hr_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing of HR packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_hr_register(
  voc_hr_ul_fn_type  uplink_func,
  voc_hr_dl_fn_type  downlink_func
);
#endif  /* MSMAUD_VOC_HR */

/* <EJECT> */
#ifdef MSMAUD_VOC_G711
/*===========================================================================

FUNCTION voc_g711_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing of G.711 packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_g711_register(
  voc_g711_ul_fn_type  uplink_func,
  voc_g711_dl_fn_type  downlink_func
);
/*~ FUNCTION voc_g711_register */
#endif  /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
/*===========================================================================

FUNCTION voc_g723_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing of G.723 packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_g723_register(
  voc_g723_ul_fn_type  uplink_func,
  voc_g723_dl_fn_type  downlink_func
);
#endif  /* MSMAUD_VOC_G723 */

#endif  /* FEATURE_MVS */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_read_ones_detect

DESCRIPTION
  This function reads ones detect.

DEPENDENCIES
  None.

RETURN VALUE
  ONES DETECT STATUS.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean voc_read_ones_detect
(
  void
);

/* <EJECT> */
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
extern void voc_reset_ones_detect
(
  void
);

/* <EJECT> */
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
extern voc_status_type voc_register_codec_func (
  voc_codec_type                pcm_path,
  voc_amp_enable_func_ptr_type  func_ptr
);

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
voc_status_type voc_register_pa_on(
  voc_pa_on_cb_func_ptr_type  func_ptr
);


/* <EJECT> */
/*===========================================================================

FUNCTION VOC_DTMF_CMD

DESCRIPTION
  This function gives the specified tones to the vocoder to generate for
  the specified duration at the specified volume.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_dtmf_cmd (
  uint16 tone1,                  /* first tone to generate */
  uint16 tone2,                  /* second tone to generate */
  sint15 vol,                    /* volume to use */
  uint16 duration                /* duration of tones */
);

/* <EJECT> */
#if defined(FEATURE_MPEG_AUDIO) || defined(FEATURE_MIDI) || \
    defined(FEATURE_MIDI_OUT) || defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION voc_master_vol

DESCRIPTION
  This function sets the previously set master volume level for audio format playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_master_vol(void);

/* <EJECT> */
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
extern voc_status_type voc_master_volume(
  sint15 volume
);
#endif /* FEATURE_MPEG_AUDIO || FEATURE_MIDI || FEATURE_MIDI_OUT || 
          FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#if defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT)
/*===========================================================================

FUNCTION voc_midi_cmd

DESCRIPTION
  This function plays or stops a midi sequence.

DEPENDENCIES
  QDSP2 must be in QSYNTH state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_midi_cmd
(
  voc_midi_cmd_type *cmd,
  uint32            num_cmd,
  sint15            volume
);
#endif /* defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT) */

/* <EJECT> */
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
extern voc_status_type voc_synth_mvol (
  uint16 vol
);
#endif /* FEATURE_MIDI */

/* <EJECT> */
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* Callback definition for QDSP originated audio decoder data interrupts
*/
typedef void (*voc_adec_cb_func_type) (
  void *client_data
);

typedef void (*voc_codec_update_cb_func_type) (
  uint32 dualmono,
  void *client_data
);

/* <EJECT> */
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
extern void voc_adec_isr ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_init

DESCRIPTION
  This function configures data functions and activates the audio decoder
  playback process.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_adec_init(
  voc_adec_cb_func_type cb_func,
  const void            *client_data,
  voc_codec_update_cb_func_type cb_func2
);

/* <EJECT> */
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
extern voc_status_type voc_adec_move_data( 
  voc_adec_mode_type adec_type,
  voc_adec_cmd_type  *adec_cmd
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_reset

DESCRIPTION
  This function causes the vocoder to reset audio decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_adec_reset( uint32 reset_mask );

/* <EJECT> */
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
extern voc_status_type voc_adec_pause (
  boolean pause     /* TRUE if output should be paused */
);

/* <EJECT> */
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
extern boolean voc_adec_int_pending(void);


/* <EJECT> */
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
extern voc_status_type voc_master_pan (
  uint8 pan
);
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

/* <EJECT> */
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
extern void voc_set_adpcm_mode (
  boolean  mode
);

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
extern void voc_set_wav_mode (
  voc_adpcm_mode_enum_type  mode
);
#endif /* FEATURE_VOC_G711 */
#endif /* FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_GASYNTH_INTERFACE
/*===========================================================================

FUNCTION voc_adec_voc_start

DESCRIPTION
  This function starts the audio decoder for vocoder playback
  
DEPENDENCIES
  NONE
  
RETURN VALUE
  A value of voc_status_type to indiciate whether DSP is properly
  configured to accept 13K audio data. 

SIDE EFFECTS
  NONE
===========================================================================*/
extern voc_status_type voc_adec_voc_start (
  uint32                        channel,
  voc_adec_voc_enum_type        voc_type,
  voc_adec_start_mode_enum_type start_mode
);

/*===========================================================================

FUNCTION voc_adec_data_consumed

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
extern boolean voc_adec_data_consumed(
  uint8 channel
);
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION voc_get_adpcm_valid

DESCRIPTION
  This function will initialize a QSynth adpcm channel.

DEPENDENCIES
  FEATURE_QSYNTH_ADPCM.

RETURN VALUE
  voc_adpcm_valid_type

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_adpcm_valid_type voc_get_adpcm_valid (
  void *state,
  voc_adpcm_spec_type *spec
);

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
extern voc_status_type voc_adec_adpcm_start (
  uint32                        channel,
  voc_adpcm_play_enum_type      play_type,
  voc_adpcm_spec_type           *spec,
  voc_adec_start_mode_enum_type start_mode
);
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
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
extern voc_status_type voc_adec_flush ( uint16 flush_mask );

/* <EJECT> */
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
extern void voc_adec_adpcm_stop ( uint32 channel );
#endif /* FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
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
);
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
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
extern voc_status_type voc_adec_chan_pan (
  uint32 channel,
  sint15 pan
);

/* <EJECT> */
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
extern voc_status_type voc_adec_chan_start (
  uint32  channel
);

/* <EJECT> */
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
extern voc_status_type voc_adec_chan_pause (
  uint32  channel,
  boolean pause_flag
);

/* <EJECT> */
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
extern voc_status_type voc_adec_chan_vol (
  uint32 channel,
  uint16 volume
);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION voc_set_adec_mode

DESCRIPTION
  This function setup the new QSynth audio decoder mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_set_adec_mode (voc_qsynth_adec_mode_type mode);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
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
extern boolean voc_adec_pcm_stopped(void);

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
void voc_set_audfmt_mix_mode(voc_adec_mix_mode_type mix_mode);

/*===========================================================================

FUNCTION voc_get_desired_config

DESCRIPTION
  Returns the configuration selected by the client specified by the acquire_key

DEPENDENCIES
  Only valid if both the acquire_key and config are set by the selected client.

RETURN VALUE
  capability

SIDE EFFECTS
  None.

===========================================================================*/
voc_capability_type voc_get_desired_config(voc_acquire_type acq_key);
#endif /* FEATURE_AUDIO_FORMAT && FEATURE_AVS_INCALL_ADEC_PB */


/* <EJECT> */
#ifdef FEATURE_MPEG_AUDIO
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
);
#endif /* FEATURE_MPEG_AUDIO */

/* <EJECT> */
#ifdef FEATURE_AAC
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
  const voc_aac_config_type *config
);

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
void  voc_aac_reset_song_config(void);

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
);
#endif /* FEATURE_MP3 */

/* <EJECT> */
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
  const voc_ra_dsp_config_type *ra_config
);

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
void voc_ra_reset_song_config(void);

#endif /* FEATURE_REAL_AUDIO */

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
extern boolean voc_amr_wbp_set_song_config (
  const voc_amr_wbp_config_type *new_cfg
);

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
extern void voc_amr_wbp_reset_song_config (void);

#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

#endif /* FEATURE_ADEC */

/* <EJECT> */
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
typedef struct {
  uint32 hp_inst_def;   /* HP instrument definition table */
  uint32 hp_drum_def;   /* HP drum definition table       */
  uint32 hp_wave_info;  /* HP waveform table              */
  uint32 hq_inst_def;   /* HQ instrument definition table */
  uint32 hq_drum_def;   /* HQ drum definition table       */
  uint32 hq_wave_info;  /* HQ waveform table              */
  uint32 hq_inst_num;   /* Number of HQ instruments       */
  uint32 hq_drum_num;   /* Number of HQ drums             */
} voc_dls_config_type;

extern void voc_set_dls_config (
  voc_dls_config_type *dls_config
);

/* <EJECT> */
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
extern void voc_set_qsynth_vol_mode(
  voc_qsynth_vol_mode_enum_type mode
);

/* <EJECT> */
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
extern void voc_set_hybrid_mode (
  uint8 hybrid_mode
);

/* <EJECT> */
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
extern voc_status_type voc_pause_midi_seq (
  uint32  sequence,
  boolean pause_flag
);

/* <EJECT> */
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
extern voc_status_type voc_set_vol_midi_seq (
  uint32 sequence,
  uint16 vol
);
#endif /* FEATURE_MIDI */

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
extern boolean voc_valid_addr
(
   void* start_addr,
   uint16 length
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_peek_cmd

DESCRIPTION
  Reads 16-bit values from vocoder address space.  If the QDSP is in RESET
  state, the peek command will fail.  If the QDSP is in SLEEP/INIT state,
  the codec PLL and associated clock regimes must be enabled before the 
  memory read can take place.  This can take up to 5ms.  Codec PLL and clock 
  regimes are disabled again once the memory read is completed. 

DEPENDENCIES
  The address and length parameters should be checked to make sure that
  the memory block is in vocoder address space.  This is done using 
  voc_valid_addr().

RETURN VALUE
  voc_status_type
  - VOC_DONE_S: command was successful.
  - VOC_CMD_FAILED_S: command failed. 

SIDE EFFECTS
  Interrupts are locked.

===========================================================================*/
extern voc_status_type voc_peek_cmd
(
   void* src_ptr,     /* source pointer */    
   void* dest_ptr,    /* destination pointer */
   uint16 length      /* size of data buffer */
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_poke_cmd

DESCRIPTION
  Writes 16-bit values to vocoder address space.  If the QDSP is in RESET
  state, the poke command cannot be performed.  If the QDSP is in SLEEP/INIT 
  state, the codec PLL and associated clock regimes must be enabled before 
  the memory write can take place.  This can take up to 5ms.  Codec PLL and 
  clock regimes are disabled again once the memory write is completed. 

DEPENDENCIES
  The address and length parameters should be checked to make sure that
  the memory block is in vocoder address space.  This is done using 
  voc_valid_addr().

RETURN VALUE
  voc_status_type
  - VOC_DONE_S: command was successful.
  - VOC_CMD_FAILED_S: command failed. 

SIDE EFFECTS
  Interrupts are locked.

===========================================================================*/
extern voc_status_type voc_poke_cmd
(
   void* dest_ptr,    /* destination pointer */    
   void* src_ptr,     /* source pointer */
   uint16 length      /* size of data buffer */
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_RX_MUTE_CMD

DESCRIPTION
  This function mutes the vocoder receive audio path.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_rx_mute_cmd (
  boolean mute_on                   /* TRUE = mute, FALSE = unmute */
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_RX_DTMF_OVERRIDE_CMD

DESCRIPTION
  This function mutes the vocoder receive audio path during DTMF.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_rx_dtmf_override_cmd (
  boolean mute_on                   /* TRUE = mute, FALSE = unmute */
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_TX_MUTE_CMD

DESCRIPTION
  This function mutes the vocoder transmit audio path.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_tx_mute_cmd (
  boolean mute_on                   /* TRUE = mute, FALSE = unmute */
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_dtmf_tx_mute_cmd

DESCRIPTION
  Control mute/unmute of the DTMF output to the FM transmit path.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - Command completed successfully

SIDE EFFECTS
  If there is a DTMF tone playing, it will be extended

===========================================================================*/
extern voc_status_type voc_dtmf_tx_mute_cmd (
  boolean mute_on                   /* TRUE = mute, FALSE = unmute */
);

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_TX_RATE_LIMIT

DESCRIPTION
  This function limits the vocoder transmit rate to ( 2s+1 ) / ( 2s+2 ).

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_tx_rate_limit (
  word rate_factor                   /* RDR limit from Sysparm msg */
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_pkt_int_loop_cmd

DESCRIPTION
  This function sets the Packet loopback state of the vocoder receive
  audio path.  This causes the Rx packets to loop-back as Tx packets
  internally.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  voc_loopback_setting

===========================================================================*/
extern void voc_pkt_int_loop_cmd ( boolean loop_on );

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
extern void voc_micro_loop_cmd ( boolean loop_on );

#endif  /* FEATURE_UP_LOOPBACK_TEST */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_dfm_loop_cmd

DESCRIPTION
  This function sets the DFM loopback state of the vocoder 
  receive audio path. This causes the DFM TX to loopback to RX.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, DFM or
  Vocoder state.

RETURN VALUE
  None

SIDE EFFECTS
  voc_loopback_setting 

===========================================================================*/
extern void voc_dfm_loop_cmd ( boolean loop_on );

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PCM_LOOP_CMD

DESCRIPTION
  This function sets the PCM loopback state of the vocoder receive audio path.
  This causes the encoder's input samples to be looped back as the decoder's
  output samples.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_pcm_loop_cmd ( boolean loop_on );

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_AUDIO_LOOP_CMD

DESCRIPTION
  This function sets the Audio loopback state of the vocoder receive audio path.
  This causes the Tx samples to be looped back as the Rx samples.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_audio_loop_cmd ( boolean loop_on );

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PKT_LOOP_CMD

DESCRIPTION
  This function sets the Packet loopback state of the vocoder receive
  audio path.  This causes the Rx packets to loop-back as Tx packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_pkt_loop_cmd ( boolean loop_on );

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
extern void voc_d2e_loop_cmd ( boolean loop_on );

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CHECK_PB_CAPABILITY

DESCRIPTION
  This function checks whether the vocoder supports the desired playback option.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if the service option is supported
  FALSE - if the service option is not supported

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean voc_check_pb_capability ( voc_capability_type so );

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CHECK_REC_CAPABILITY

DESCRIPTION
  This function checks whether the vocoder supports the desired recording option

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if the service option is supported
  FALSE - if the service option is not supported

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean voc_check_rec_capability ( voc_capability_type so );
/* <EJECT> */

/*===========================================================================

FUNCTION VOC_CAPABILITY_SUPPORTED

DESCRIPTION
  This function checks whether the vocoder supports the
  desired service option.

DEPENDENCIES
  The version information must be initialized by a call to
  voc_test_cmd() prior to calling this routine.

RETURN VALUE
  TRUE - if the service option is supported
  FALSE - if the service option is not supported

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean voc_capability_supported( voc_capability_type );

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_VERSION

DESCRIPTION
  This function acquires version information from the Vocoder

DEPENDENCIES
  None

RETURN VALUE
  The ROM version and the RAM version.

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_version (
   uint8 *major,
   uint8 *minor
);

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

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
extern void voc_check_codec_clk_change
(
  voc_codec_type prev_codec,
  voc_codec_type curr_codec
);

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
extern boolean voc_aux_pcm_clk_required(voc_codec_type codec);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CODEC_SELECT

DESCRIPTION
  This function sets the desired vocoder to use a specific codec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_codec_select ( voc_codec_type path );

/* <EJECT> */
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
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
extern boolean voc_sample_rate ( voc_sample_rate_type rate );
#endif /* defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT) */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/* This type is used to enable/disable 3D positional mode */
typedef enum {
  VOC_3D_POS_MODE_DISABLE,   /* Disable 3D positional mode */
  VOC_3D_POS_MODE_ENABLE,    /* Enable 3D positional mode  */
  VOC_3D_POS_MODE_INVALID
} voc_3d_pos_mode_enum_type;

/* This structure is used to represent a position in X,Y,Z coordinates */
typedef struct {
  sint31 x_pos;  /* X position */
  sint31 y_pos;  /* Y position */
  sint31 z_pos;  /* Z position */
} voc_3d_pos_coord_type;

typedef voc_3d_pos_coord_type voc_3d_pos_vector_type;
/* This structure is used to represent orientation */
typedef struct {
  voc_3d_pos_vector_type forward;   /* Forward vector */
  voc_3d_pos_vector_type up;        /* Up vector      */
} voc_3d_pos_orient_type;

/* This structure is used to represent the volume attenuation */
typedef struct {
  uint32 max_distance; /* Distance at which sounds are played at max volume */
  uint32 min_distance; /* Distance beyond which sounds are muted            */
  uint32 scale;        /* Scale (0...1000), 1000: 1.0 (normal rolloff)      */
                       /*                      0: 0.0 (no rolloff)          */
  boolean mute;        /* Mute after Max: TRUE-Mute, FALSE-Hold after Max   */
} voc_3d_pos_vol_atten_type;

/* This enum is used to represent the Reverb parameter being set */
typedef enum{
  VOC_3D_POS_REVERB_SET_PRESET,       /* Set Reverb Preset */
  VOC_3D_POS_REVERB_SET_LEVEL,        /* Set Reverb Gain in db */
  VOC_3D_POS_REVERB_SET_TIME,        /* Set Decay time in ms */
  VOC_3D_POS_REVERB_SET_RATIO         /* Set Decay ratio */
} voc_3d_reverb_enum_type;

/* This structure is used to represent reverb */
typedef struct{
  uint16 env_type;          /* Environment type                             */
  sint31 room_gain;         /* Room Gain                                    */
  uint32 decay_ms;          /* Decay Time in ms for low frequencies         */
  uint32 decay_ratio;       /* Decay time ratio btwn high and low frequencies */
  uint16 ref_delay;         /* Initial reflection delay time in ms          */
  sint31 reverb_gain;       /* Reverb gain relative to room effect          */
  uint16 reverb_delay;      /* Reverb delay relative to initial reflection in ms */
} voc_3d_reverb_type;

/* The following structure contains all 3D positional source parameter
** settings.
*/
typedef struct {
  boolean                   src_enable; /* Source enable flag        */
  voc_3d_pos_coord_type     position;  /* Source position           */
  voc_3d_pos_vol_atten_type vol_atten; /* Source volume attenuation */
  sint31                    volume;    /* Source volume             */
  voc_3d_pos_vector_type    vel_vector; /* Source velocity vector   */
  sint31                    reverb_vol; /* Source reverb volume     */
} voc_3d_pos_src_all_setting_type;

/* The following callback type is used to return all source parameter settings
** to the client.
*/
typedef void (* voc_3d_pos_src_final_cb_type) (
  uint32                          channel,   /* ADEC channel queried */
  voc_3d_pos_src_all_setting_type *src_all   /* All source settings  */
);

/* <EJECT> */
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
extern voc_status_type voc_3d_pos_enable (
  voc_3d_pos_mode_enum_type mode
);

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
extern voc_status_type voc_3d_pos_reset_env (void);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_set_output_mode

DESCRIPTION
  This function sets QAFX output mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_3d_pos_set_output_mode (void);
/* <EJECT> */
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
extern voc_status_type voc_3d_pos_lsnr_pos (
  voc_3d_pos_coord_type *coord,
  uint32                duration_ms
);

/* <EJECT> */
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
extern voc_status_type voc_3d_pos_lsnr_orient (
  voc_3d_pos_orient_type *orient,
  uint32                 duration_ms
);

/* <EJECT> */
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
);

/* <EJECT> */
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
  None

===========================================================================*/
void voc_3d_reverb_reset (void);

/*===========================================================================

FUNCTION voc_3d_pos_reverb

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
  voc_3d_reverb_type       *param
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_reverb_enable

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
  boolean      enable
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_src_enable

DESCRIPTION
  This function issues sound sourcd 3D enable/disable command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_3d_pos_src_enable (
  uint32                    channel,
  voc_3d_pos_mode_enum_type mode
);

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
extern voc_status_type voc_3d_pos_reset_src (
  uint32 channel
);

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
extern voc_status_type voc_3d_pos_src_vol (
  uint32 channel,
  sint31 volume,
  uint32 duration_ms
);

/* <EJECT> */
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
extern voc_status_type voc_3d_pos_src_vol_atten (
  uint32                    channel,
  voc_3d_pos_vol_atten_type *vol_atten
);

/* <EJECT> */
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
extern voc_status_type voc_3d_pos_src_pos (
  uint32                channel,
  voc_3d_pos_coord_type *coord,
  uint32                duration_ms
);

/* <EJECT> */
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
) ;

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
voc_status_type voc_3d_reverb_src_vol (
  uint32                channel,
  sint31                vol
) ;

/* <EJECT> */
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
extern voc_status_type voc_3d_pos_query_env_enable ( 
  boolean *flag 
);

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
extern voc_status_type voc_3d_pos_query_lsnr_pos (
  voc_3d_pos_coord_type *coord
);

/* <EJECT> */
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
extern voc_status_type voc_3d_pos_query_lsnr_orient (
  voc_3d_pos_orient_type *orient
);

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
extern voc_status_type voc_3d_pos_query_lsnr_vel (
  voc_3d_pos_vector_type *vector
);

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
extern voc_status_type voc_3d_pos_query_reverb_enabled (
  boolean *flag
);

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
extern voc_status_type voc_3d_pos_query_reverb_setting (
  voc_3d_reverb_type *reverb
);
/* <EJECT> */
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
extern voc_status_type voc_3d_pos_query_src_final (
  uint32                       channel,
  voc_3d_pos_src_final_cb_type cb_func
);

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
extern voc_status_type voc_3d_pos_query_src_enable ( 
  uint32  channel,
  boolean *flag 
);
/* <EJECT> */
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
extern voc_status_type voc_3d_pos_query_src_pos (
  uint32                channel,
  voc_3d_pos_coord_type *coord
);

/* <EJECT> */
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
extern voc_status_type voc_3d_pos_query_src_vol (
  uint32 channel,
  sint31 *volume
);

/* <EJECT> */
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
extern voc_status_type voc_3d_pos_query_src_vol_atten (
  uint32                    channel,
  voc_3d_pos_vol_atten_type *vol_atten
);

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
extern voc_status_type voc_3d_pos_query_src_vel (
  uint32                 channel,
  voc_3d_pos_vector_type *vector
);

/*===========================================================================

FUNCTION voc_3d_pos_query_src_vol

DESCRIPTION
  This function queries the source reverb volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_3d_pos_query_src_revol (
  uint32 channel,
  sint31 *volume
);

#endif /* FEATURE_QAUDIOFX */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_GET_SERVICE_OPTION

DESCRIPTION
  This function returns the current service option.

DEPENDENCIES
  None.

RETURN VALUE
  Curent service option.

SIDE EFFECTS
  None.

===========================================================================*/
voc_capability_type voc_get_service_option( void );

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_GET_STATUS

DESCRIPTION
  This function returns the current vocoder state.

DEPENDENCIES
  None.

RETURN VALUE
  Curent vocoder state.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_inform_type voc_get_status( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_pcmpad

DESCRIPTION
 Get the value of the incoming (TX) PCM pad byte.

DEPENDENCIES

RETURN VALUE
  PCM pad byte

SIDE EFFECTS

===========================================================================*/

extern uint8 voc_get_pcmpad( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_put_pcmpad

DESCRIPTION
  Set the value of the outgoing (RX) PCM pad byte.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void voc_put_pcmpad( uint8 pad_byte );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_codec_pad

DESCRIPTION
  External codec pad values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_codec_pad(
  uint8  padval,   /* Value for pad bits             */
  uint8  mask      /* Mask of pad bits to control    */
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_TASK 

DESCRIPTION

DEPENDENCIES
  This procedure should be called by rex_def_task.  Also, each task
  must set its report signal at least once every DOG_CHECK_TIME milliseconds.

RETURN VALUE
  None, this procedure does not return.

SIDE EFFECTS


===========================================================================*/
extern void voc_task
(
  dword param
    /* The least significant rex_sigs_type portion of this dword is
    ** a mask of task report signals to be monitored.
    */
);

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_AUD_PROCESS_OVERRIDE

DESCRIPTION
  This function sets the audio process override mode.  Will override
  ESEC/NS/AGC/HPF settings to OFF if enabled.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - Operation completed successfully.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_aud_process_override (
  boolean aud_process_override
);

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
extern void voc_set_ec_override_mode (
  voc_ec_ns_mode_type   ec_mode
);

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
extern void voc_set_ns_override_mode (
  voc_ec_ns_mode_type   ns_mode
);

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
extern void voc_smartsnd_set_rve_mode (
  voc_smartsnd_type rve_mode
);


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
extern void voc_set_vox_override_mode (
  voc_vox_mode_type   vox_mode
);

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
extern void voc_set_sidetone_override_mode (
  voc_sidetone_mode_type   sidetone_mode
);

/* <EJECT> */
#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

FUNCTION VOC_RX_AGC_CMD

DESCRIPTION
  This function sets the Rx AGC/AVC mode.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - 

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_rx_agc_cmd ( voc_agc_type agc_mode );


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

extern voc_status_type voc_smartsnd_rx_agc_cmd (
  voc_smartsnd_type agc_mode
);

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
extern voc_status_type voc_tx_agc_cmd ( boolean agc_mode );
#endif /* FEATURE_AUDIO_AGC */

/* <EJECT> */
#ifdef FEATURE_BT_AG
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
extern void voc_set_bt_app_id( bt_app_id_type bt_app_id );

/* <EJECT> */
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
extern bt_app_id_type voc_get_bt_app_id(void);

/* <EJECT> */
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
extern boolean voc_complete_bt_ag (void);

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
extern void voc_register_ag_enabled_cb(voc_bt_link_enabled_cb_type cb);
#endif /* FEATURE_BT_AG */

/* <EJECT> */
#ifdef FEATURE_VOC_PCM_INTERFACE
/*===========================================================================

FUNCTION VOC_REGISTER_PCM_INPUT_CLIENT

DESCRIPTION
  Use this function to register a callback to feed PCM data on the RX input
  path to the vocoder. Set the client_func to NULL to de-register a client.

DEPENDENCIES
  The client must acquire the Vocoder before using this interface.

RETURN VALUE
  VOC_DONE_S       if successful.
  VOC_CMD_FAILED_S if the client's input parameters were incorrect.
  VOC_CMD_BUSY_S   if the vocoder ran out of command buffers to process this
                   command.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_register_pcm_input_client(
  voc_pcm_interface_type       interface,
  voc_pcm_client_input_fn_type client_func
);

/*===========================================================================

FUNCTION VOC_REGISTER_PCM_OUTPUT_CLIENT

DESCRIPTION
  Use this function to register a callbacks to get PCM output from the vocoder.

DEPENDENCIES
  The client must acquire the Vocoder before using this interface.

RETURN VALUE
  VOC_DONE_S       if successful.
  VOC_CMD_FAILED_S if the client's input parameters were incorrect.
  VOC_CMD_BUSY_S   if the vocoder ran out of command buffers to process this

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_register_pcm_output_client(
  voc_pcm_interface_type        interface,
  voc_pcm_client_output_fn_type client_func
);
#endif /* FEATURE_VOC_PCM_INTERFACE */

#if defined(FEATURE_PMIC_SPEAKER_ON_OFF_DELAY)||defined(FEATURE_KAMP_ON_OFF_DELAY)
/* Callback function after PMIC has shutdown */
typedef void (* voc_pmic_delay_cb_func_type)(void) ;

/*===========================================================================

FUNCTION VOC_PMIC_SPEAKER_DELAY

DESCRIPTION
  Use this function to register a callbacks to restore PMIC speakerphone
  state after an elapsed time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_pmic_speaker_delay(
  rex_timer_cnt_type           time,
  voc_pmic_delay_cb_func_type  cb_func
);
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */

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
extern void voc_equalizer_init_crit_sect( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_enable_equalizer

DESCRIPTION
  This function enable/dsiable the equalizer.

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
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_equalizer

DESCRIPTION
  This function setup the equalizer.

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
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_SET_EQUALIZER_GAIN

DESCRIPTION
  This function setup the equalizer gian on a specified band.

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
);

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
);
#endif /* FEATURE_AUDFMT_EQUALIZER */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
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
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_spectrum_analyzer_interval

DESCRIPTION
  Setup the spectrum analyzer update period

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
extern void voc_set_spectrum_analyzer_interval( void );
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION voc_mm_parm

DESCRIPTION
  Select a new set of paramters for multimedia recording

DEPENDENCIES
  None 

RETURN VALUE
  None 

SIDE EFFECTS
  None 

===========================================================================*/
extern void voc_set_mm_param (
  voc_sample_rate_type  sample_rate,
  voc_mm_rec_chan_type  chan_index,
  uint16                bit_per_spl,
  voc_mm_rec_cb_fn_type isr_cb_func
);

/*===========================================================================

FUNCTION VOC_MM_REC_UPDATE_READ_COUNT

DESCRIPTION
  Reset the read counter to DSP.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_mm_rec_update_read_count (
  uint32    read_cnt 
);

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
extern boolean voc_mm_rec_check_config (void);

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
extern boolean voc_mm_rec_get_data(
  voc_mm_record_format_type format,
  voc_mm_record_buf_type    *buf,
  voc_mm_record_data_type   *frame_info
);

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
extern void voc_mm_rec_cb_isr(void);
#endif /* #ifdef FEATURE_MM_REC */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION voc_set_av_sync

DESCRIPTION
  Registers the av_aync callback with vocoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
==========================================================================*/
extern void voc_set_av_sync (
  voc_av_sync_dec_enum_type  decoder_type,
  uint32                     av_sync_interval,
  voc_av_sync_interval_type  av_sync_mode,  
  /* Callback to receive the data */
  voc_av_sync_cb_type        av_sync_cb
); 
#endif /* FEATURE_AUDIO_FORMAT */

/* Defalut value for TX rate factor */
#define VOC_1X_TX_RATE_FACTOR_DEFAULT 0

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

#ifdef FEATURE_AUDFMT_QCONCERT
/* The following structure is used to define QConcert parameters
*/
typedef struct {
  uint16  mode;               /* Qconcert output mode                     */
  uint16  gain;               /* Gain value                               */
  uint16  spread;             /* Spread value                             */
  int16   geometry;           /* Geometry value                           */
} voc_qconcert_params_type;

/* <EJECT> */
/*===========================================================================

FUNCTION voc_enable_qconcert

DESCRIPTION
  This function will enable/disable qconcert

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
extern voc_status_type voc_enable_qconcert(
  boolean enable
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_qconcert_params

DESCRIPTION
  Setup the qconcert parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
extern voc_status_type voc_set_qconcert_params
(
  voc_qconcert_params_type *params
);
#endif /* FEATURE_AUDFMT_QCONCERT */


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
);

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
extern voc_status_type voc_set_slowtalk_params
(
  voc_slowtalk_params_type *params
);

/* <EJECT> */
#ifdef FEATURE_ARM_POST_PROCESSING
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
extern voc_status_type voc_pp_client_data 
(
  uint16    buf_length, 
  uint16    *buffer, 
  uint16    buf_index
);
#endif /* FEATURE_ARM_POST_PROCESSING */
/* <EJECT> */
#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE) 
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
extern void voc_pp_ctl 
( 
  voc_pp_cb_type  pp_cb,
    /* Callback to receive the data */
  voc_pp_type_type pp_type
);

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
extern boolean voc_pp_stereo_codec
(
  void
); 
#endif /* FEATURE_ARM_POST_PROCESSING  || FEATURE_AVS_WB_PCM_RX_INTERFACE */

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
void voc_free_wb_pcm_rx_interface_buffer( uint16 buf_index );

#endif /* (FEATURE_SBC_CODEC)|| defined(FEATURE_AVS_WB_PCM_RX_INTERFACE) */ 

#if defined FEATURE_SBC_CODEC || defined FEATURE_SBC_DSP_CODEC
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
); 
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
void voc_complete_bt_a2dp (void);

/* <EJECT> */
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
boolean voc_is_bt_a2dp_pending(void);

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
extern void voc_register_a2dp_enabled_cb(voc_bt_link_enabled_cb_type cb);
/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_bt_a2dp_remote_params

DESCRIPTION
   
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_set_bt_a2dp_remote_params ( audiosbcenc_data_type sbc_data);

/* <EJECT>*/
/*===========================================================================

FUNCTION voc_bt_a2dp_suspend

DESCRIPTION
  Suspend the BT A2DP connection if not already suspended
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_bt_a2dp_suspend(void);

/*=========================================================================
FUNCTION voc_set_a2dp_disconnected

DESCRIPTION
  Updates the information about the BT A2DP connection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void voc_set_a2dp_disconnected(void);
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_SBC_DSP_CODEC
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
void voc_sbc_configure(voc_sbc_config_params_type *params);

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
void voc_sbc_set_bitrate(uint32 bitrate);

#endif /* FEATURE_SBC_DSP_CODEC */

/* <EJECT> */                                                                                                     
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
boolean voc_image_supports_i2ssbc(void);

#endif /* FEATURE_AVS_I2SSBC */
/* <EJECT> */                                                                                                     
#ifdef FEATURE_VOC_DTMF_DETECT
typedef enum {
  VOC_DTMF_DETECTED_0,
  VOC_DTMF_DETECTED_1,
  VOC_DTMF_DETECTED_2,
  VOC_DTMF_DETECTED_3,
  VOC_DTMF_DETECTED_4,
  VOC_DTMF_DETECTED_5,
  VOC_DTMF_DETECTED_6,
  VOC_DTMF_DETECTED_7,
  VOC_DTMF_DETECTED_8,
  VOC_DTMF_DETECTED_9,
  VOC_DTMF_DETECTED_POUND,
  VOC_DTMF_DETECTED_STAR,
  VOC_DTMF_DETECTED_A,
  VOC_DTMF_DETECTED_B,
  VOC_DTMF_DETECTED_C,
  VOC_DTMF_DETECTED_D,
  VOC_DTMF_DETECTED_NONE
} voc_dtmf_detected_status_type;

typedef void (* voc_dtmf_detect_cb_type) (voc_dtmf_detected_status_type);

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
extern void voc_dtmf_detect (
  voc_dtmf_detect_cb_type rx_detect,
  voc_dtmf_detect_cb_type tx_detect
);
#endif /* FEATURE_VOC_DTMF_DETECT */

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
);
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
void voc_wma_reset_song_config( void );

#endif /* FEATURE_WMA */
#ifdef FEATURE_BT_AG

#if defined (FEATURE_AVS_BT_SCO_REWORK)
/*===========================================================================
FUNCTION voc_is_bt_ag_pending

DESCRIPTION
  This function returns TRUE if we are waiting for Sco link to be setup. 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_is_bt_ag_pending(void);

/* <EJECT> */
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
void voc_set_sco_connected(void);
#endif /*AVS_BT_SCO_REWORK*/

/*=========================================================================
FUNCTION voc_set_sco_disconnected

DESCRIPTION
  Updates the information about the BT sco connection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void voc_set_sco_disconnected(void);

#endif /* Feature_BT_AG */


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
extern void voc_set_timewarp(
  boolean enable,
  uint16  factor
);

#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif /* MSMAUD_VOC_4GV_VOIP */
/* <EJECT> */


#ifdef MSMAUD_VOC_4GV_WB_VOIP
/*===========================================================================

FUNCTION voc_set_4gv_WB_timewarp

DESCRIPTION
  Sets phase factors for timewarp in the qdsp.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_set_4gv_wb_timewarp(
  boolean  enable,
  uint16   run_length,
  uint16   phase_offset
);
#endif /* MSMAUD_VOC_4GV_WB_VOIP*/
#endif /* FEATURE_QDJ_TIMEWARP */

/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/* Include VR definitions
*/
#include "vocvr.h"
#endif
/*===========================================================================

FUNCTION voc_set_qtv_cb_func 

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
void voc_set_qtv_cb_func(voc_notify_qtv_cb_func_ptr_type cb_func);

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
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
void voc_set_bt_cb_func(voc_bt_cb_func_ptr_type cb_func);
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
boolean voc_get_bt_playback_dtmf_start(void);
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
void voc_set_bt_playback_dtmf_start(boolean bt_dtmf_play_evt_status);
#endif /* FEATURE_BT_AG|FEATURE_SBC_CODEC|FEATURE_SBC_DSP_CODEC& AVS_BT_SCO_REWORK*/


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
void voc_set_oemsbl_cb(voc_oemsbl_cb_func_ptr_type func_ptr);
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
extern void voc_ecodec_clock_ctl (
  boolean switchon
  /* True = turn ecodec clock ON, False = turn ecodec clock off */
);

#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*===========================================================================

FUNCTION voc_audfmt_adjust_sample

DESCRIPTION
  This function add/drop (s) the sample

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern voc_status_type voc_audfmt_adjust_sample
(
  boolean                        add_sample
);
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

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
extern boolean voc_check_cap_ready
(
  voc_capability_type cap
);

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION voc_dual_mono_mode

DESCRIPTION
  Set Dual Mono mode.
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_dual_mono_mode (
  uint16 dual_mono_mode
);

/*===========================================================================

FUNCTION voc_sbr_mode

DESCRIPTION
  Set SBR mode.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_sbr_mode (
  uint16 sbr_mode
);

/*===========================================================================

FUNCTION voc_codec_update_isr

DESCRIPTION
  codec update ISR
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_codec_update_isr ( 
  uint16 dualmono
);
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

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
extern void voc_set_codec_override ( 
  voc_codec_type codec_to_be_override, 
  voc_codec_type override_codec, 
  uint8 overrideFlag
);
#endif // SND_TEST_CODEC_OVERRIDE

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
extern void voc_process_fm_over_bt(
  voc_aux_line_in_mode_type aux_mode
);

/*===========================================================================

FUNCTION voc_process_aux_line_in

DESCRIPTION
  
DEPENDENCIES
  None.

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_process_aux_line_in(
  voc_aux_line_in_mode_type aux_mode
);

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
extern boolean voc_check_aux_line_in(
  void
);

/*===========================================================================

FUNCTION voc_process_aux_line_in_vol

DESCRIPTION
  This function process the aux line in volume set request
  
DEPENDENCIES
  None.

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_process_aux_line_in_vol(uint8 volume);

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
extern void voc_set_idle_metering_enable (uint16 idlemetering);

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
extern boolean voc_check_in_call (uint32 acquire_mask);

#ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
#endif

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
extern boolean voc_check_pa_on_status (
  void
);

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
extern void voc_set_pa_state (
  boolean state
);

/*===========================================================================

FUNCTION voc_set_tx_pcm_mux_config

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern void voc_set_tx_pcm_mux_config(uint32 tx_audio_ch_ctrl, 
                                      uint32 tx_audio_ch_msk);

/*===========================================================================

FUNCTION voc_set_tx_pcm_mux_config

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern void voc_set_rx_pcm_mux_config(uint32 rx_audio_ch_ctrl, 
                                      uint32 rx_audio_ch_msk);

#endif /* VOC_H */



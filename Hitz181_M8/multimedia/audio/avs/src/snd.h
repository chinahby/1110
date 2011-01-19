#ifndef SND_H
#define SND_H
/*===========================================================================

                S O U N D   T A S K    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the sound task.

Copyright (c) 1992 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/snd.h#17 $ $DateTime: 2009/11/16 20:16:30 $ $Author: kkmajeti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/16/09   kkm     Added the function snd_clr_bt_cb_func to deregister the
                   callback function registered bt BT for informing the play type.
09/07/09   vsud    Changes for resampler image imeplementation.
07/07/09   rk      Notifying SND that PAs are turned on, wakes up sound task.
01/19/09   anb     Added snd_set_qtv_cb_func so that QTV can be informed about
                   mod/image swap event.Fixes Cr 163807(SR-1133583)
03/10/09   sj      Added slowtalk feature support
01/19/09    dp     CR 168251 Removed feturization for FEATURE_BT_QSOC
12/26/08   rm      Added RVE API changes
12/10/08   knm     Added snd_is_task_up() API to check the sound task init status.
11/10/08    sk     Modified the code to resolve the Featurization issue for
                   FEATURE_AMR_WBPLUS_AUDIO_DEC 
08/26/08   anb     Added support for DSP based WMAPRO
06/24/08   knm     Added snd_set_aux_line_in support to configure AuxPGA
                   Line In loopback path.
06/23/08    ry     Added SMAF support for LED/VIB event synchronization.
03/16/08    sj     Added Support for OEMSBL
12/19/07    hs     Added support ISDB-T dual mono output control
12/07/07    kk     Fixed compiler warnings due to QEnsemble.
11/05/07    sk     Updated  SND_CMX_AF_STARTED START command to send the 
                   status to CMX layer 
                   CR 120253,124336,124337,120853
11/14/07    hs     Added QEnsemble support
10/26/07    hs     Added EVW file format support
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
08/08/07  anb/wfd  Added aux_pcm_clk to be enabled/disabled from other tasks like BT (CR 122106).
08/06/07    kk     Merged the Fix CR 124244 - Full ID3V2.3 support, 
                   support for URL link (W)frames .
05/23/07    kk     Fixed  CR 113932 - CMX get spec problem due to APIC 
                   frame in MP3 ID3V2.3 tag.
04/20/07    at     Added EVB enum to snd_qcp_format_type for EVRC-B file
                   format recording support.
04/02/07   ayu     Fixed AMR Recording and Playback issue. 
03/021/07   at     Added SND_CMX_AF_FILE_EVB to snd_cmx_af_file_type for
                   EVRC-B file format playback support.
03/15/07    kan    Added 4GV-WB Code 
02/27/07    kk     Added support for AAC BSAC audio
01/23/06    kk     Fixed issues related to 44k Sample rate.
11/20/06    sp     Cleaned up AV Sync related APIs.
                   Added support for OEMSBL Audio.
11/21/06    at     Added new status SND_PROMPT_NONE to snd_status_type.
11/09/06    az     Merged the code for MM API changes.
09/08/06    pl     Added 44k to snd_sample_rate_type
09/05/06    ay     Added AMR-WB+ support.
07/28/06   hz      Added support for ID3V2 and AAC ADTS/VBR song length.
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
07/26/06    sp     Added support for A2DP AV Sync
06/27/06    pl     Added environment & source 3D enable/disable and reset support.
06/26/06    pl     Defined new data structures for doppler, reverb and 
                   new ping-pong buffer data writing scheme.
03/27/06    ak     Added support for sidetone control.
03/16/06   anb     Added support for sbc re-configuration given the encoded params
                   CR 78581,87334. 
03/13/06    lg     Added support for SMAF meta data in AUDIO_SPEC callback
03/13/06    st     Added support for delayed start for playback of audio
                   decoder files to MIDI device; added support for repeat
                   playback mode for audio decoder files to MIDI device;
                   added support for query of all 3D positional source
                   parameter settings at end of playback.
01/30/06    sp     Added support for I2S SBC interface.
01/30/06    pl     Added support to provide a way to select hybrid mode 
                   for multisequencer mode and MIDI device.
10/21/05    st     Added SND_CMX_MIDI_OUT_QCP_START.
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05    st     Added support for QAudioFX: 3D positional commands and query
                   interface; added SND_CMX_ADEC_READ_VAR.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is 
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
09/12/05    hz     Added AV sync support for windows media.
08/31/05    sm     VOX updates
08/29/05    sp     Made changes to fix a bug resulting in playback not 
                   resuming in the BT A2DP headset after a fwd/rewind of 
                   streaming playback.
08/24/05   ayu     Modified code to delete recorded QCP files containing only
                   header information, for CR62770.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
08/04/05    sp     Added support for mixing of qcp audio and voice call.
07/22/05    sm     Added Wav A/ULaw support.
07/20/05    sp     Added support for AAC LOAS.
07/19/05    pl     Added SND_CMX_AF_ABORT to snd_af_data_cmd_enum for midi 
                   callback and extended snd_status_type to include more 
                   error-specific abort status.
07/13/05    sp     Changed SND_SBC_BITRATE_EXTREME to SND_SBC_BITRATE_HIGHEST
07/08/05    pl     Define the snd_supported_ops_mask_type and append the 
                   new bit mask type to every audio format spec type.
06/15/05    st     Added support for SVG commands in CMX callback.
06/03/05    sg     Changed SND_DUMMY_DATA_UINT32_MAX to 0x7FFFFFFF since the
                   compiler optimizes 0xFFFFFFFF to -1.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/27/05    sg     Forced some enums to 32bit for L4 support.
05/24/05    hz     Added support for MultiMedia/AAC record functionality.
05/18/05    ac     Added definitions to support new AV sync functionality.
04/28/05   ymc     Added 4GV NB support.
04/22/05    aw     Added support to get number of equalizer bands.
04/21/05    hz     Added VirtualPacketLength support for Windows Media 
                   playback.
03/25/05   ac/sg   Added support for A/V synchronization.
03/10/05    st     Added support for MFi LED control.
03/09/05    sg     Added meta-comments for RPC support on L4 project.
03/02/05    aw     Added support for Qconcert, API change for configuring
                   EQ bands.
02/14/05    hz     Added support for Windows Media Audio decoder.
02/05/05    st     Added support for DLS processing; added byte_offset
                   parameter to snd_audfmt_sound_type.
01/11/05    st     Added SND_CMX_AF_FILE_MFI.
11/19/04    sp     Added support for Arm Post Processing.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
06/18/04    aw     Update max/min refresh period for spectrum analyzer.
06/17/04    aw     Modified the max/min allowable period for spectrum analyzer.
06/08/04    aw     Added support for Equalizer and Spectrum analyzer.
04/23/04    lg     Added suport for dynamic SAF Object buffers.
04/16/04    st     Added support for animation positioning and control; added
                   cue and jump point status types.
03/22/04    st     Added support for PCM playback and recording.
                   Added SND_CMX_AF_TIME_TICK status command; updated
                   snd_cmx_smaf_note_ctl_type; added
                   snd_midi_out_qcp_pause_chan() and
                   snd_midi_out_qcp_resume_chan() function prototypes.
                   Removed SND_CMX_AF_WA_PLAY_DONE and SND_CMX_AF_WA_FAILURE
                   status types.
01/26/04    lg     Modified code to support tune, tempo & pan adjustments and
                   volume, pause & stop of each sequence in multi_sequencer
01/26/04    aw     Added support to get file specs.
01/21/04    aw     Added support for SMAF led/vib/graph info callback.
12/02/03    sg     Added support for linear PCM playback.
11/22/03    sg     Added support for RealAudio codec.
10/22/03    aw     Work arround for incorrect pointer cast problem between 
                   snd_audfmt_sound_type and snd_format_sound_type.
10/16/03    aw     Modified code to support SMAF and multi-sequence.
10/10/03    sg     Modified to support additional AAC params reqd by the DSP.
                   Rearranged snd_af_data_cmd_enum so that client libraries
                   can link correctly if IMELODY is not defined.
04/30/03    st     Added new language support for WebAudio files.
04/17/03    sm     Added AMR file format support.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
02/28/03    st     Added snd_ec_ns_mode_type and snd_ec_ctl() and snd_ns_ctl()
                   function prototypes.
02/19/03    st     Added support for ADPCM file type; renamed 
                   snd_adec_mode_enum_type to snd_qsynth_adec_mode_enum_type.
12/08/02    st     Removed featurization from snd_cmx_af_file_type to match
                   structure in CMX.
11/06/02    sm     Added support for AMR vocoder.
11/05/02    sg     Added support for iMelody format.
09/19/02    st     Added support for pixel poistioning for WebAudio picture
                   commands; added support to return content elements
                   information to client.
08/13/02   aw/st   Modified code to support ADPCM audio decoder.
           sg/st   Added support for MP4 and AAC; added support for flush
                   command; added support for JPEG image in CMX files; added
                   support to play audio files where format is specified by
                   client.
07/11/02    yl     Added the new enum, SND_CMX_QCP_REC_AUTO_STOP to 
                   snd_af_data_cmd_enum and changed the function prototype
                   for snd_qcp_record().
06/11/02    st     Added fixed fullrate EVRC and fixed halfrate 13K options to
                   snd_qcp_format_type.
10/01/01    st     Modified snd_midi_out_qcp() and snd_midi_out_qcp_stop()
                   function prototypes to support multiple channels; added
                   snd_midi_out_qcp_pause(), snd_midi_out_qcp_resume(),
                   snd_midi_out_qcp_volume() and snd_midi_out_pan() function
                   prototypes; added SND_CMX_ADEC_READ to snd_af_data_cmd_enum.
09/16/01    st     Changed SND_QCP_FUNC_GET_CNFG to SND_QCP_FUNC_CNFG to reuse
                   for setting cnfg chunk.
09/03/01    st     Added support for QCP recording and support functions (set/
                   get information); modified code to support playback of QCP
                   files to reverse/both links. 
07/24/01   ymc     Always defined SND_HFK.
06/18/01   jkl     Added FEATURE_ACP.
04/04/01    st     Added support for direct MIDI device output interface; added
                   support for animation for WebAudio files; modified code to 
                   add length parameter to snd_af_read_cb_func_type; added
                   support for ISO8859 character code sets for WebAudio; added
                   ENABLED and DISABLED status to snd_status_type.
02/28/01    st     Added support for audio format preparse mechanism.
12/04/00    sm     Added proto for snd_sound_pan().
          st/sm    Added stereo DAC sampling rate in sound.  Moved many MIDI
                   calls into audfmt.  Added SND_CMX_GET_FILE_LEN support.
09/05/00    st     Added support for remote sounds.
08/07/00    st     Added support to inform clients of an end cue point; added
                   support for client to specify a certain number of 
                   cue point iterations.
07/25/00    st     Added snd_audfmt_start_enum_type; modified 
                   snd_audfmt_sound_type to include a starting indicator.
07/22/00    st     Added SND_CMX_AF_CS_HANGUL for korean text support.
07/15/00    st     Use 8bit value for volume levels; added support for tune 
                   and tempo functions; added LED and audio spec support for
                   audio format callback functions.
06/13/00    st     Added code to handle text positioning for WebAudio files.
06/13/00    st     Added snd_audfmt_new_data() prototype.
05/18/00    st     Added WebAudio specific support; added SND_PRIO_MED; added 
                   sound flow function prototypes; added support for audio 
                   format sounds.
03/06/00    st     Removed FEATURE_ENHANCED_RINGS from scaling.
02/03/00    st     Added SND_BAD_VP to snd_status_type.
09/09/99  jkl/st   Modifications after code review; added AGC/AVC control
                   functions.
08/17/99   jkl     Modified for the New API
07/19/99   st/vl   Added snd_pb_status_type to snd_vp_free_buffer_type.
                   Removed 'far '. Added snd_get_voice_path_out() prototype.
06/30/99  sm/RPW   Integrated AGC feature with VR/VM features; added Q2 support
                   for SND_HFK and AVBAP defines (st).
04/20/99    st     Added vibrator support for Digby (FEATURE_VIBRATOR) target.
03/22/99    st     Added SND_SND_MUTE (mask that indicates that sound task is 
                   requesting mute); modified parametric_group_type.
03/04/99    st     Made modifications to support playing tones specified by
                   their frequency values in hertz: modified snd_tone_type,
                   snd_tone_packet_type, snd_multi_packet_type.
                   Removed reference to ringer enable line (moved to ring.h);
                   added support for voice memo.
02/26/99    st     Added SND_VR_HFK_CALL_RECEIVED to snd_predef_type.
01/13/99    st     Added support for voice prompting.
12/17/98    st     Added SND_HFK_PROTOCOL_TGP_VR to snd_hfk_protocol_type.
11/04/98    sm     Merged ARM/MSM3000 support to mainline.
10/05/98    st     HFK-VR mainlining: Added SND_EXT_PROTOCOL command; added 
                   SND_IDS_SIG signal; added extproto to snd_packets_type; 
                   added snd_hfk_protocol_type and snd_ext_proto_packet_type.
10/01/98    st     Added SND_TWO_CODECS for TGP targets.
08/17/98    st     Added SND_MAX_SOUND_TONE. Also added SND_AVBAP_SUPPORTED
                   for TGP and Odie builds.
08/10/98    st     Replaced usage of the SinBaD interface with the BORSCHT
                   interface by request from the WLL group.
06/22/98    st     Added SND_ADSP_TONEGEN feature definition, added 
                   SND_CLASS_ADSP in snd_class_type for ADSP tone generation.
05/29/98    ha     Added service transition sounds and external power 
                   on/off predefined sounds to snd_predef_type.
04/02/98    ha     Added Support for T_O.
03/18/98    ha     Added SND_CALL_IN_PROGRESS_WLL and SND_PHONE_OFF_HOOK_WLL to
                   snd_predef_type for use with FEATURE_STD_WLL.
02/13/98    ha     Added SND_OLD_VOLUME_UNITS internal feature.
01/30/98    ha     Added support for SND_DTACO_ROAM_TONE. 
01/22/98    ha     Added SND_ROAM_RING to snd_predef_type.
01/21/98    ha     Added SND_VOL_SCALE control tone (for enhanced rings) 
                   under FEATURE_ENHANCED_RINGS. 
                   Updated piano notes (octaves 4 and 5) in snd_tone_type. 
                   Updated Copyright.
01/16/98    ro     Added support for external communications and device id.
                   Added SND_EXT_CONNECT and SND_EXT_ID commands.
12/23/97    ha     Added SND_CLASS_RING to snd_class_type.
                   Removed SND_APATH_PRIVACY from snd_apath_type.
12/08/97    ha     Added click_enable field to snd_tone_packets_type.
                   Removed SND_USE_RINGER_INT for T_G cond. compile.
08/15/97   dak     Added stutter dial tone to be used if messages are waiting.
07/23/97    ro     Add Lint comment for SND_APATH_NONE.
06/17/97    ro     Support for T_Q.
06/17/97   dak     Added SND_DT_TONE_BRAZIL, SND_DIAL_TONE_BRAZIL,
                   SND_BUSY_BRAZIL and SND_RING_BRAZIL under 
                   FEATURE_BRAZIL_WLL and SND_REORDER_WLL under
                   FEATURE_STD_WLL.
06/06/97    ro     Add new pre-defined sound - SND_ALTERNATE_REORDER.
                   Get rid of some hanging comma's.
05/23/97    ro     Remove SND_NV_CMD_SIG, and replace it with
                   SND_OVERRIDE_TIMER_SIG.
                   Use FEATURE_HFK_PLUS to define SND_HFK_PLUS.
04/29/97    ro     Add #define SND_HFK_PLUS for T_T.
04/28/97    ro     Add T_MD for SND_HFK.
04/11/97    ro     Remove T_MD in conditional compiles, module does not have
                   a ringer or a carkit option. Remove unused SND_SINGLE_TONE.
03/28/97    ro     Support for TGP. Removed support for T_P. Re-ordered
                   snd_tone_type and added SND_FIRST_CONTROL_TONE to provide
                   a structured way of detecting control tones.
                   Added additional loop-back control tones.
                   Added SND_RING_C for user selectable rings.
                   Removed prototypes for snd_st_mode, snd_lpf_mode,
                   snd_wbd_mode, snd_voc_on. Removed snd_lpf_type.
                   Added SND_RING_1 through 9 for user-selectable ring sounds.
03/12/97   jjn     Added (TG==T_MD) for Module in conditional compile statements
02/12/97    ro     Added support for aux generators - SND_CLASS_AUX.
                   Added sound features: SND_USE_RINGER_ENABLE,
                   and SND_USE_RINGER_INT.
01/20/97    ro     Added SND_RPT1 in support of key delay function.
01/07/97    ro     Support for Car-kit and Earpiece DTMF's using the vocoder.
                   In snd_apath_type, paths are masks.
                   Added SND_ALL_PATHS.
                   Added SND_HFK, SND_SINGLE_TONE.
                   Added snd_path_packet_type, and SND_SET_PATH command
                   Removed snd_aud_packet_type
                   Changed snd_vol_packet_type to use snd_vol_type.
                   Added SND_VOLUME command
                   Added SND_PATH_TIMER_SIG
                   Added Maximum volume settings.
                   Added snd_vol_type, replaces byte in command packets.
                   Added snd_mute_control_type. Added
                   SND_APATH_PRIVACY to snd_apath_type.
                   Added rpt_sigs to snd_multi_packet_type.
                   Added snd_class_type. Added sclass field to cmd packets.
                   Added SND_SIGNAL_SILENCE, and SND_RPT_NOSIGNAL
                   to snd_tone_type.
10/07/96   dak     Replaced T_INDIA_WLL with FEATURE_INDIA_WLL
07/22/96   fkm     Minor Documentation Tweeks
03/19/96   fkm     Updates Per Code Review
02/16/96   fkm     Tweeks to comments and documentation
02/12/96   fkm     Lint and Documentation Updates and some Pre-Apex Defines.
12/13/95   fkm     India Updates and Menu-7-4 (mute) fix
11/30/95   fkm     Added Stuff for PIP and India Tones
11/02/95   fkm     Minor Name Change to Vocoder Gain Adjust Stuff and
                   added snd_set_voc_gain_adjust() to Interface Definitions.
11/02/95   fkm     Added snd_get_packet() defintion, Vocoder Gain Support,
                   slight change to Stereo Mute Packet.
10/10/95   fkm     Added SND_PHONE_AWAKE_SIG.
08/22/95   fkm     Added SND_MUTE_CAR_STEREO and SND_UNMUTE_CAR_STEREO
08/15/95   fkm     SND_ABRV_ALERT moved to SND_BUSY_ALERT and SND_ABVR_ALERT
                   reused per IS-99.
07/21/95   fkm     Message Waiting Sound and Tone Added to enums
06/19/95   fkm     Minor Documentation Update
06/14/95   fkm     Fix Documentation to SND.C matches SND.H
06/08/95   fkm     Defined SND_RINGER_OFF_SIG for Gemini
05/16/95   fkm     Added SND_ABRV_REORDER and SND_ABRV_INTERCEPT.
11/08/93   jah     Added SND_NV_CMD_SIG for NV command exchanges.
07/30/93   jah     Added snd_fm_pwr_type and prototype for snd_fm_tx_pwr().
06/04/93   jah     Added SND_VOC_CHNG_SIG.
05/03/93   jah     Added prototype for snd_wbd_mode().
04/12/93   jah     Added mute mask SND_MUTE_SNDK for muting during audio path
                   changes and other neat things that go 'klack'.
03/02/93   jah     Added snd_voc_on() prototype for the Portable.
02/19/93   jah     Added vox change signal for resetting vocoder background
                   noise estimate after a vox and path change.
02/09/93   jah     Added no-qwack changes, added DTMF path change to no-qwack.
01/08/93   jah     #ifdef'd tone/path/vol improvement while it stablizes.
                   Added SND_MUTE_SNDV for, voice during tone.
10/23/92   jah     Updated comments.
10/21/92   jah     Added masks for Diag mute override.
10/16/92   jah     Moved snd_apath_mute_ctl() to sndim.h (removed external
                   interface).  Added comments for mobile/portable.  Fixed
                   confusing naming between ring-back sound and ring-back tone.
09/15/92   jah     Changed SND_MUTE_ACPRX to SND_MUTE_ACPRXTX
09/10/92   jah     Added some new tones for ringing.
09/03/92   jah     Changed SND_MAX_MULTI_TONE to 66 for 1+32+32+1 tones for UI.
09/02/92   jah     Added SND_HORN_ON (horn alert).  Added snd_comp_ctl().
08/27/92   jah     Added snd_rxfm_mute_ctl()
08/25/92   jah     Added 1 KHz test tone
08/24/92   jah     Changed SND_MAX_MULTI_TONE to 65 for 32+32+1 tones for UI.
08/20/92   jah     Added SND_LOOP_BACK2 and repeat signal, added mute mask
                   for CDMA.
08/14/92   jah     Added "answer tone", "horn alert", "fade", and "Svc change"
07/27/92   jah     Converted to new BIO macros, added code review changes.
02/27/92   jah     created file

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "target.h"     /* Target Specific Defintions           */
#include "snddev.h"     /* Devices and methods                  */
#include "sndid.h"      /* Sound id                             */
#include "tdbid.h"      /* Tones id                             */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
 #include "audiosbcenc.h" /* Defintions for audiosbcenc */
#endif

#ifdef FEATURE_AUDIO_FORMAT
#include "qw.h"         
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Sound task feature control
*/

/* Target has a HFK (car-kit) option.
*/
#define SND_HFK

/* The carkit for this target uses "NGP Plus" protocol
*/
#ifdef FEATURE_HFK_PLUS
#error code not present
#endif

/* Does this target support intelligent accessories */
#if (defined(FEATURE_IDS))
#error code not present
#endif

/* Index of first control tone. If this value is changed to be different 
   from Zero, please update the valid tone range comparison */
#define SND_FIRST_CONTROL_TONE 0
/* Index of first audio tone */
#define SND_FIRST_AUDIO_TONE   100

/* This definition is used to force enums to use 32 bits - required for L4 */
#define SND_DUMMY_DATA_UINT32_MAX 0x7FFFFFFF

typedef uint16 snd_sound_id_type;
typedef uint16 snd_tone_type;

/* Control tones */
enum snd_control_tones {
  SND_SILENCE = SND_FIRST_CONTROL_TONE,
  SND_CALLBACK_SILENCE,/* Callback repeat then Silence - no tone at all    */
  SND_STOP,            /* Stop playing list (last item in a multi-tone)    */
  SND_RPT,             /* Repeat list (last item in a multi-tone)          */
  SND_RPT1,            /* Repeat the previous tone                         */
  SND_RPT_NOCALLBACK,  /* Repeat list - do not call callback               */
  SND_LOOP_BACK2,      /* Loop back 2 items, use duration as a loop count  */
  SND_LABEL,           /* Label for looping                                */
  SND_BACK_TO_LABEL,   /* Back to label use duration as a loop count       */
  SND_VOL_SCALE,       /* Adjust volume level for a tone                   */
  SND_LAST_CONTROL_TONE
};

/* Audio tones */
enum snd_audio_tones {
  SND_FREQ = SND_FIRST_AUDIO_TONE,
                      /* Tone is defined by specified frequency values    */
#ifdef FEATURE_VOICE_PLAYBACK
  SND_VOICE_PROMPT,   /* Voice prompt type                                */
#endif
  SND_LAST_AUDIO_TONE
};

#if (defined (FEATURE_VOICE_PLAYBACK) || defined (FEATURE_VOICE_MEMO))
#ifndef VOC_PB_HEADER_TYPE
#define VOC_PB_HEADER_TYPE
/* Forward definition of voc_pb_header_type */
typedef struct voc_pb_header_struct voc_pb_header_type;
#endif
#endif

/*
** Mute control is "voted", so that multiple callers can ask for the
** audio to mute and un-mute.  A counting semaphore was not appropriate
** due to the problems with the caller counting mute/unmutes properly.
** Each caller has a specifed mask which identifies it.
*/
typedef enum {
  SND_MUTE_NONE    = 0,    /* NO MUTERS - MUST BE ZERO       */
  SND_MUTE_ACPFM   = 2,    /* Analog Call Processing (FM)    */
  SND_MUTE_ACPRXTX = 4,    /* Analog Call Processing (RX/TX) */
  SND_MUTE_SND     = 8,    /* Sound Task, general            */
  SND_MUTE_SNDV    = 16,   /* Sound Task, voice during tone  */
  SND_MUTE_DIAG    = 64,   /* Diagnostic override            */
  SND_MUTE_SNDPATH = 256   /* Sound Task, Path Mute (FM)     */
} snd_mute_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Priority of the sound
*/
typedef enum {
  SND_PRIO_LOW,                 /* All sounds except DTMF burst    */
  SND_PRIO_MED,                 /* All sounds except DTMF burst    */
  SND_PRIO_HIGH,                /* Used in DTMF burst transmission */
  SND_PRIO_ALL,                 /* Used to stop all sounds         */

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_PRIO_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_priority_type;

/* Any physical path may be muted or unmuted.
*/
typedef enum {
  SND_MUTE_UNMUTED = 0,         /* Audio path is not muted.     */
  SND_MUTE_MUTED,               /* Audio path is muted.         */
  SND_MUTE_MAX,                 /* Always last in the list      */

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_MUTE_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_mute_control_type;

/* DTMF audio output path
*/
typedef enum {
  SND_APATH_LOCAL,         /* DTMF's on local audio        */
  SND_APATH_TX,            /* Transmit DTMFs               */
  SND_APATH_BOTH,          /* Tx and sound DTMF's locally  */
  SND_APATH_MUTE,          /* Mute the DTMFs               */

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_APATH_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_apath_type;

/* Used to declare what kind of filtering Sound is to provide.  Mostly,
** this is used on the External PCM interface, but there is nothing to
** say this must be so.
*/

typedef enum {
  SND_FILTERS_VOICE,    /* Set Filters for Normal Voice Call            */
  SND_FILTERS_DATA      /* Set Filters for Data Call (no filtering)     */
} snd_filters_type;

/* Database tones use the duration in msec and control tones use the
** general-purpose data field.
*/
typedef union {
  uint16    duration_ms;  /* Duration in milliseconds */
  uint16    data;         /* general purpose data     */
} snd_tone_data_type;

/* Define a single tone/DTMF with a duration
*/
typedef struct {
  snd_tone_type      tone;          /* Tone/DTMF to generate        */
  snd_tone_data_type param;         /* Tone data                    */
} snd_compact_tone_type;

/* Frequency pair which defines a tone/DTMF 
*/
typedef struct {
  snd_tone_type      tone;          /* Tone/DTMF to generate        */
  snd_tone_data_type param;         /* Tone data                    */
  word               freq_hi;       /* High frequency value in hz   */
  word               freq_lo;       /* Low frequency value in hz    */
} snd_flexible_tone_type;

/* Type specifications for types of sounds
*/
typedef enum {
  SND_COMPACT_SOUND,     /* Designates a sound using tones in the data base */
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif
  SND_FLEXIBLE_SOUND,    /* Sound that use freq. pairs to define tones      */
#ifdef FEATURE_VOICE_PLAYBACK
  SND_TONE_AND_VOICE,    /* Sound that may include voice prompts and tones  */
#endif
#ifdef FEATURE_AUDIO_FORMAT
  SND_AUDIO_FORMAT_SOUND,/* A midi/WebAudio sound                           */
  SND_FORMAT_SOUND,      /* Audio format sound where format is specified    */
#endif
  SND_INVALID_SOUND      /* Used to indicate an invalid sound type          */
} snd_type;

/* Type specifications for compact sounds
*/
typedef struct {
  snd_type               type;        /* Type = SND_COMPACT_SOUND         */
  snd_compact_tone_type  *tone_array; /* Pointer to the array of tones    */
} snd_compact_sound_type;

#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif /* FEATURE_REMOTE_SOUND */

/* Type specifications for flexible sounds
*/
typedef struct {
  snd_type               type;        /* Type = SND_FLEXIBLE_SOUND        */
  snd_flexible_tone_type *tone_array; /* Pointer to the array of tones    */
} snd_flexible_sound_type;

#ifdef FEATURE_VOICE_PLAYBACK
/* Type specifications for voice prompts and tone sounds
*/
typedef struct {
  snd_type               type;          /* Type = SND_TONE_AND_VOICE         */
  snd_compact_tone_type  *tone_array;   /* Pointer to of tones               */
  voc_pb_header_type     **prompt_list; /* Pointer to array of voice prompts */
} snd_tone_and_voice_sound_type;
#endif /* FEATURE_VOICE_PLAYBACK */

#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
/* The various codec sampling rates that are supported
*/
typedef enum {
  SND_SAMPLE_RATE_NONE,     /* Zero sampling rate, turn clocks off */

  SND_SAMPLE_RATE_8000,     /* 8k      */
  SND_SAMPLE_RATE_11025,    /* 11.025k */
  SND_SAMPLE_RATE_12000,    /* 12k     */

  SND_SAMPLE_RATE_16000,    /* 16k     */
  SND_SAMPLE_RATE_22050,    /* 22.050k */
  SND_SAMPLE_RATE_24000,    /* 24k     */
  
  SND_SAMPLE_RATE_32000,    /* 32k     */
  SND_SAMPLE_RATE_44100,    /* 44.1k   */
  SND_SAMPLE_RATE_48000,    /* 48k     */

#ifdef FEATURE_AAC
  SND_SAMPLE_RATE_64000,    /* 64k     */
  SND_SAMPLE_RATE_88200,    /* 88.2k   */
  SND_SAMPLE_RATE_96000,    /* 96k     */
#endif /* FEATURE_AAC */

#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  SND_SAMPLE_RATE_44000,    /* 44k     */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */

  SND_SAMPLE_RATE_MAX,      /* MAX     */
  SND_SAMPLE_RATE_UNKNOWN   = SND_SAMPLE_RATE_MAX /* Unknown rate */
} snd_sample_rate_type;

/* Common channel info
*/
typedef enum {
  SND_CHANNEL_UNKNOWN = 0,
  SND_CHANNEL_MONO,        /* Single channel (mono) data                  */
  SND_CHANNEL_DUAL,        /* Stereo data                                 */
  SND_CHANNEL_TRIPLE,      /* 3 channels: 1+2 (UNSUPPORTED)               */
  SND_CHANNEL_QUAD,        /* 4 channels: 1+2+1 (UNSUPPORTED)             */
  SND_CHANNEL_QUINTUPLE,   /* 5 channels: 1+2+2 (UNSUPPORTED)             */
  SND_CHANNEL_SEXTUPLE,    /* 5+1 channels: 1+2+2+1 (UNSUPPORTED)         */
  SND_CHANNEL_OCTUPLE      /* 7+1 channels: 1+2+2+2+1 (UNSUPPORTED)       */
} snd_channel_enum_type;
#endif /* defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT) */

#ifdef FEATURE_AUDIO_FORMAT
/* Commands/status enumerated type. These values are used in the audio format
** playback callback function to inform the client of status or to get data.
*/
typedef enum {
  SND_CMX_AF_READ,             /* Get data buffer from client                 */
  SND_CMX_AF_READ_VAR,         /* Get variable length data buffer from client */
  SND_CMX_AF_READ_VAR_DLS,     /* Get variable length DLS data buffer         */
  SND_CMX_AF_GET_DLS,          /* Get DLS waveform from client                */
  SND_CMX_AF_FREE_DLS,         /* Free DLS waveform buffer                    */
  SND_CMX_AF_PREPARSE,         /* Preparse command                            */
  SND_CMX_AF_FREE_BUF,         /* Free expired data buffer                    */
  SND_CMX_AF_FREE_VAR_BUF,     /* Free expired variable length data buffer    */
  SND_CMX_AF_RESET,            /* Reset client's state                        */
  SND_CMX_AF_SET_TRACKS,       /* Set the number of tracks                    */
  SND_CMX_AF_GET_FILE_LEN,     /* Get the length of the file                  */
  SND_CMX_AF_AUDIO_SPEC,       /* Specifications of WebAudio file             */
  SND_CMX_AF_FLUSHED,          /* Flushed data buffers and restarted playing  */
  SND_CMX_AF_REWIND,           /* Playback is rewinding                       */
  SND_CMX_AF_FFORWARD,         /* Playback is fast-forwarding                 */
  SND_CMX_AF_PAUSE,            /* Playback is paused                          */
  SND_CMX_AF_RESUME,           /* Playback has resumed                        */
  SND_CMX_AF_FA_DELAY,         /* Playback being delayed by data read         */
  SND_CMX_AF_TEMPO,            /* Playback tempo change                       */
  SND_CMX_AF_TUNE,             /* Playback tune change                        */
  SND_CMX_AF_PAN,              /* Playback stereo pan change                  */
  SND_CMX_AF_CUE_POINT_END,    /* Playback end of cue point encountered       */
  SND_CMX_AF_JUMP_POINT_END,   /* Playback end of jump point encountered      */
  SND_CMX_AF_REPEAT,           /* Playback at repeat point                    */
  SND_CMX_AF_CALC_TIME_RESULT, /* Calc time result status callback            */
  SND_CMX_AF_CALC_TIME_ABORT,  /* Calc time abort status callback             */
  SND_CMX_AF_PLAY_DONE,        /* Play done status                            */
  SND_CMX_AF_FAILURE,          /* Failure status                              */
  SND_CMX_AF_ABORT,            /* Abort status                                */
  SND_CMX_AF_TIME_TICK,        /* Tick update (usually occurs every second)   */
#ifdef FEATURE_WEBAUDIO
  SND_CMX_AF_TITLE,            /* Title of the sound                          */
  SND_CMX_AF_TEXT_CLEAR,       /* Text clear command                          */
  SND_CMX_AF_TEXT_NEW,         /* New text command                            */
  SND_CMX_AF_TEXT_APPEND,      /* Append text command                         */
  SND_CMX_AF_TEXT_DISABLE,     /* Disable text display command                */
  SND_CMX_AF_TEXT_ENABLE,      /* Enable text display command                 */
  SND_CMX_AF_TEXT_BACK,        /* Move text cursor back                       */
  SND_CMX_AF_TEXT_ADVANCE,     /* Advance text cursor                         */
  SND_CMX_AF_PIC_NEW,          /* New picture information                     */
  SND_CMX_AF_PIC_CLEAR,        /* Picture clear command                       */
  SND_CMX_AF_PIC_ENABLE,       /* Enable picture display                      */
  SND_CMX_AF_PIC_DISABLE,      /* Disable picture display                     */
  SND_CMX_AF_ANIM_CLEAR,       /* Animation clear command                     */
  SND_CMX_AF_ANIM_ENABLE,      /* Enable animation display                    */
  SND_CMX_AF_ANIM_DISABLE,     /* Disable animation display                   */
  SND_CMX_AF_SAF,              /* SAF data                                    */
  SND_CMX_AF_SAF_FRAME_ID,     /* SAF frame ID command                        */
  SND_CMX_AF_SAF_FRAME,        /* SAF frame command                           */
  SND_CMX_AF_SVG,              /* SVG data                                    */
  SND_CMX_AF_SVG_FRAME_ID,     /* SVG frame ID command                        */
#ifdef FEATURE_SMAF
  SND_CMX_SMAF_GRAPH_INFO,     /* SMAF Graphics Track chunk information       */
  SND_CMX_SMAF_LED_CONTROL,    /* SMAF LED Control                            */
  SND_CMX_SMAF_VIB_CONTROL,    /* SMAF vibrator Control                       */
#endif /* FEATURE_SMAF */
  SND_CMX_MFI_LED_CONTROL,     /* MFi LED Control                             */
#endif /* FEATURE_WEBAUDIO */
#ifdef FEATURE_QCP
  SND_CMX_QCP_SUCCESS,         /* QCP command was successful                  */
  SND_CMX_QCP_REC_GET_BUF,     /* QCP record/get buffer command               */
  SND_CMX_QCP_REC_DATA,        /* QCP recorded data command                   */
  SND_CMX_QCP_INT_TIME,        /* QCP time interval callback                  */
  SND_CMX_QCP_REC_BUFF_ERR,    /* QCP record stopped due to buffer error cmd  */
  SND_CMX_QCP_REC_STOP,        /* QCP record stop command                     */
  SND_CMX_QCP_REC_AUTO_STOP,   /* QCP recording is stopped due to the silence */
  SND_CMX_QCP_REC_ERROR,       /* QCP record error command                    */
  SND_CMX_QCP_FAILURE,         /* QCP command failed                          */
#endif /* FEATURE_QCP */
#ifdef FEATURE_PCM_REC
  SND_CMX_PCM_REC_HEADER_DATA, /* PCM record header data command              */
  SND_CMX_PCM_REC_DATA,        /* PCM record data command                     */
  SND_CMX_PCM_REC_DONE,        /* PCM record finished command                 */
  SND_CMX_PCM_REC_PAUSED,      /* PCM record paused status                    */
  SND_CMX_PCM_REC_RESUMED,     /* PCM record resumed status                   */
  SND_CMX_PCM_REC_INTERRUPTED, /* PCM record interrupted status               */
  SND_CMX_PCM_REC_INT_RESUME,  /* PCM record resumed from interrupted status  */
  SND_CMX_PCM_REC_DROP_FRAMES, /* PCM record frames being dropped status      */
  SND_CMX_PCM_REC_DROP_RESUME, /* PCM record resumed from frames dropping     */
#endif /* FEATURE_PCM_REC */
#ifdef FEATURE_MM_REC
  SND_CMX_MM_REC_HEADER_DATA,
  SND_CMX_MM_REC_DATA,        /* MM record data command                     */
  SND_CMX_MM_REC_DONE,        /* MM record finished command                 */
  SND_CMX_MM_REC_PAUSED,      /* MM record paused status                    */
  SND_CMX_MM_REC_ENABLED,     /* MM record enable status                    */
  SND_CMX_MM_REC_RESUMED,     /* MM record resumed status                   */
  SND_CMX_MM_REC_INTERRUPTED, /* MM record interrupted status               */
  SND_CMX_MM_REC_INT_RESUME,  /* MM record resumed from interrupted status  */
  SND_CMX_MM_REC_FAILURE,     /* MM record resumed failue occured           */
#endif /* FEATURE_MM_REC */
#ifdef FEATURE_MIDI_OUT_QCP
  SND_CMX_MIDI_OUT_QCP_START,  /* QCP file playback to MIDI device started    */
  SND_CMX_MIDI_OUT_ADEC_START_CMD,/* ADEC playback start command executed     */
  SND_CMX_MIDI_OUT_QCP_RESET,  /* Reset QCP file playback to MIDI device      */
  SND_CMX_MIDI_OUT_QCP_ABORT,  /* Abort command from QSynth audio decoder     */
#endif /* FEATURE_MIDI_OUT_QCP */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
  SND_CMX_ADEC_READ,           /* Get audio decoder buffer from client        */
  SND_CMX_ADEC_READ_VAR,       /* Get variable adec buffer from client        */
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_IMELODY)
  SND_CMX_AF_LED_CONTROL,      /* LED Control                                 */
#endif
#ifdef FEATURE_IMELODY
  SND_CMX_AF_BACKLIGHT_CONTROL,/* Backlight Control                           */
#endif
#ifdef FEATURE_DLS
  SND_CMX_AF_DLS_DONE,         /* DLS load command done                       */
  SND_CMX_AF_DLS_FAILURE,      /* DLS load command failed                     */
#endif
  SND_CMX_AF_STARTED,          /*  Added the START command to send to CMX     */   
  SND_CMX_AF_INVALID,          /* Invalid command                             */
  SND_CMX_AF_CODEC_UPDATE,     /* Audio codec info feedback                   */
  SND_CMX_AF_SBR_MODE,         /* SBR command confirmation                    */
  SND_CMX_AF_DUAL_MONO_MODE,   /* Dual Mono Channel Mode cmd configmration    */
} snd_af_data_cmd_enum;

/* Data read callback function pointer type. This callback function will be
** registered with the client to call when the data request is done.
*/
typedef void (* snd_af_read_cb_func_type) ( 
  void      *client_data,  /* Sound server data                 */
  uint8     *buf,          /* Buffer of data returned           */
  uint32    length,        /* Length of requested data in bytes */
  uint32    position       /* Position of requested data        */
);

/* Server type for data read request
*/
typedef struct {
  void                      *client_data; /* Client data                      */
  snd_af_read_cb_func_type  cb_func;      /* Callback function pointer        */
  uint32                    buffer_size;  /* Buffer size requested            */
  uint32                    offset;       /* Offset into buffer to start data */
} snd_af_read_cb_type;

/* Get DLS waveform callback function pointer type. This callback will be
** registered with the client to call when the DLS request is done.
*/
typedef void (* snd_af_get_dls_cb_func_type) (
  void      *client_data,  /* Sound server data                     */
  uint8     *data,         /* DLS waveform data                     */
  uint8     *buf,          /* Buffer to use to decode waveform      */
  uint32    length         /* Length of requested waveform in bytes */
);

/* Server type for get DLS request
*/
typedef struct {
  void                         *client_data;  /* Client data               */
  uint32                       length;        /* Length of DLS waveform    */
  uint32                       position;      /* Position of DLS waveform  */
  snd_af_get_dls_cb_func_type  cb_func;       /* Callback function pointer */
} snd_af_get_dls_cb_type;

/* Preparse function pointer type. This function gets called by the client
** so that parsing of audio format files can be done in their task context.
*/
typedef void (* snd_af_preparse_func_type) (
  void *client_data
);

/* Server type for preparse request
*/
typedef struct {
  void                          *client_data;  /* Client data               */
  snd_af_preparse_func_type     preparse_func; /* function pointer          */
} snd_af_preparse_type;

/* This enumerated type indicates the format of the audio format file. */
typedef enum {
  SND_CMX_AF_FILE_MIDI,       /* MIDI file          */
  SND_CMX_AF_FILE_CMID,       /* WebAudio file      */
  SND_CMX_AF_FILE_MFI,        /* MFi file           */
  SND_CMX_AF_FILE_SMAF,       /* SMAF file          */
  SND_CMX_AF_FILE_MP3,        /* MP3 file           */
  SND_CMX_AF_FILE_AAC,        /* AAC file           */
  SND_CMX_AF_FILE_IMY,        /* iMelody file       */
  SND_CMX_AF_FILE_RA,         /* RealAudio file     */
  SND_CMX_AF_FILE_WMA,        /* Windows Media file */
  SND_CMX_AF_FILE_WMA_PRO,    /* Windows Media 10 Pro file */
  SND_CMX_AF_FILE_QCP_13K,    /* 13k QCP file       */
  SND_CMX_AF_FILE_QCP_EVRC,   /* EVRC QCP file      */
  SND_CMX_AF_FILE_QCP_4GV_NB, /* 4GV_NB QCP file    */
  SND_CMX_AF_FILE_QCP_4GV_WB, /* 4GV_NB QCP file   */
  SND_CMX_AF_FILE_QCP_AMR,    /* AMR QCP file       */
  SND_CMX_AF_FILE_QCP_EFR,    /* EFR QCP file       */
  SND_CMX_AF_FILE_QCP_FR,     /* FR QCP file        */
  SND_CMX_AF_FILE_QCP_HR,     /* HR QCP file        */
  SND_CMX_AF_FILE_ADPCM,      /* ADPCM file         */
  SND_CMX_AF_FILE_AMR,        /* .AMR file          */
  SND_CMX_AF_FILE_PCM,        /* PCM file           */
  SND_CMX_AF_FILE_WAV_ALAW,   /* WAV, A-Law file    */
  SND_CMX_AF_FILE_WAV_MULAW,  /* WAV, Mu-Law file   */
  SND_CMX_AF_FILE_AWB,        /* .AWB file          */
  SND_CMX_AF_FILE_AMR_WBPLUS, /* AMR-WB+ file       */
  SND_CMX_AF_FILE_EVB,        /* .EVB file          */
  SND_CMX_AF_FILE_EVW,        /* .EVW file          */
  SND_CMX_AF_FILE_UNKNOWN     /* Unknown file type  */
} snd_cmx_af_file_type;

/* This structure allows clients to indicate which format to use
** to decode the audio data.
*/
typedef struct {
  snd_cmx_af_file_type file_type;         /* Codec to use for playback */
} snd_cmx_af_codec_type;

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_IMELODY)
/* Enumerated type to indicate LED state */
typedef enum {
  SND_CMX_AF_LED_DARK_BLACK,
  SND_CMX_AF_LED_DARK_BLUE,
  SND_CMX_AF_LED_DARK_GREEN,
  SND_CMX_AF_LED_DARK_CYAN,
  SND_CMX_AF_LED_DARK_RED,
  SND_CMX_AF_LED_DARK_PURPLE,
  SND_CMX_AF_LED_DARK_YELLOW,
  SND_CMX_AF_LED_DARK_WHITE,
  SND_CMX_AF_LED_BRIGHT_BLACK,
  SND_CMX_AF_LED_BRIGHT_BLUE,
  SND_CMX_AF_LED_BRIGHT_GREEN,
  SND_CMX_AF_LED_BRIGHT_CYAN,
  SND_CMX_AF_LED_BRIGHT_RED,
  SND_CMX_AF_LED_BRIGHT_PURPLE,
  SND_CMX_AF_LED_BRIGHT_YELLOW,
  SND_CMX_AF_LED_BRIGHT_WHITE
} snd_cmx_af_led_enum_type;

/* Server data for LED control callback */
typedef struct {
  boolean                  led_enable;   /* LED enabled       */
  snd_cmx_af_led_enum_type led_ctl;      /* LED state control */
} snd_cmx_af_led_ctl_type;
#endif /* FEATURE_WEBAUDIO || FEATURE_IMELODY */

typedef enum{
  SND_SUPPORTED_OPS_MASK_NONE    = 0x00, /* No supported operations */
  SND_SUPPORTED_OPS_MASK_SEEK    = 0x01,
  SND_SUPPORTED_OPS_MASK_GETTIME = 0x02
} snd_supported_ops_mask_type;

typedef struct {
  snd_cmx_af_file_type           file_type; /* Format of audio file           */
  snd_supported_ops_mask_type    supported_ops; /* Supported operations       */
} snd_audfmt_spec_type;

#ifdef FEATURE_MIDI
typedef snd_audfmt_spec_type snd_midi_spec_type;
#endif /* FEATURE_MIDI */
#ifdef FEATURE_WEBAUDIO
/* Enumerated type indicating char code set to use with text commands */
typedef enum {
  SND_CMX_AF_CS_ANSI,         /* ANSI character code set                 */
  SND_CMX_AF_CS_ISO8859_1,    /* ISO8859-1 character code set            */
  SND_CMX_AF_CS_ISO8859_2,    /* ISO8859-2 character code set            */
  SND_CMX_AF_CS_ISO8859_3,    /* ISO8859-3 character code set            */
  SND_CMX_AF_CS_ISO8859_4,    /* ISO8859-4 character code set            */
  SND_CMX_AF_CS_ISO8859_5,    /* ISO8859-5 character code set            */
  SND_CMX_AF_CS_ISO8859_6,    /* ISO8859-6 character code set            */
  SND_CMX_AF_CS_ISO8859_7,    /* ISO8859-7 character code set            */
  SND_CMX_AF_CS_ISO8859_8,    /* ISO8859-8 character code set            */
  SND_CMX_AF_CS_ISO8859_9,    /* ISO8859-9 character code set            */
  SND_CMX_AF_CS_ISO8859_10,   /* ISO8859-10 character code set           */
  SND_CMX_AF_CS_SHIFTJIS,     /* SHIFTJIS (japanese) character code set  */
  SND_CMX_AF_CS_HANGUL,       /* HANGUL (korean) character code set      */
  SND_CMX_AF_CS_CHINESE_SIMP, /* Chinese Simplified character code set   */
  SND_CMX_AF_CS_CHINESE_TRAD, /* Chinese Traditional character code set  */
  SND_CMX_AF_CS_HINDI,        /* Hindi character code set                */
  SND_CMX_AF_CS_UNKNOWN       /* Unknown character code set              */
} snd_cmx_af_char_set_enum_type;

/* This structure is used for returning data (usually text/picture) to the
** client.
*/
typedef struct {
  uint8     *buf1;          /* Buffer of data                 */
  uint32    length1;        /* Size of first buffer           */
  uint8     *buf2;          /* Buffer of data                 */
  uint32    length2;        /* Size of second buffer          */
  uint32    remain_bytes;   /* Number of bytes not in buffers */
} snd_cmx_af_buf_rtn_type;

/* Picture format type */
typedef enum {
  SND_CMX_AF_PIC_TYPE_PNG,      /* PNG image            */
  SND_CMX_AF_PIC_TYPE_BMP,      /* BMP image            */
  SND_CMX_AF_PIC_TYPE_JPEG,     /* JPEG image           */
  SND_CMX_AF_PIC_TYPE_UNKNOWN   /* Unknown image format */
} snd_cmx_af_pic_enum_type;

/* Picture draw/clear speed */
typedef enum {
  SND_CMX_AF_PIC_SPEED_NORMAL,      /* Draw/clear at normal speed */
  SND_CMX_AF_PIC_SPEED_SLOW,        /* Draw/clear at slow speed   */
  SND_CMX_AF_PIC_SPEED_MIDDLE,      /* Draw/clear at medium speed */
  SND_CMX_AF_PIC_SPEED_FAST         /* Draw/clear at fast speed   */
} snd_cmx_af_pic_speed_enum_type;

/* Positioning of picture/text within display output */
typedef enum {
  SND_CMX_AF_POS_LT,         /* Position Left/Top          */
  SND_CMX_AF_POS_RB,         /* Position Right/Bottom      */
  SND_CMX_AF_POS_CENTER,     /* Position Center            */
  SND_CMX_AF_POS_PERCENT,    /* Use percentage to position */
  SND_CMX_AF_POS_PIXEL       /* Use #pixels to position    */
} snd_cmx_af_pos_enum_type;

/* Server data for new picture comamnd */
typedef struct {
  snd_cmx_af_pic_enum_type         pic_type;      /* Format of picture       */
  snd_cmx_af_buf_rtn_type          *buf_rtn;      /* Picture data info       */
  snd_cmx_af_pic_speed_enum_type   draw_speed;    /* Draw speed              */
  snd_cmx_af_pos_enum_type         xpos_type;     /* Position in X plane     */
  snd_cmx_af_pos_enum_type         ypos_type;     /* Position in Y plane     */
  uint8                            xpos_percent;  /* percent or pixel offset */
                                                  /* in X plane              */
  uint8                            ypos_percent;  /* percent or pixel offset */
                                                  /* in Y plane              */
} snd_cmx_af_pic_new_type;

/* Server data for text comamnd */
typedef struct {
  snd_cmx_af_buf_rtn_type          *buf_rtn;      /* Text data info      */
  snd_cmx_af_pos_enum_type         xpos_type;     /* Position in X plane */
  snd_cmx_af_pos_enum_type         ypos_type;     /* Position in Y plane */
} snd_cmx_af_text_type;

/* Server data for SAF comamnd */
typedef enum {
  SND_CMX_AF_SAF_NOT_CONT,     /* SAF data complete, no more to follow     */
  SND_CMX_AF_SAF_CONT,         /* SAF data not complete, more data to come */
  SND_CMX_AF_SAF_CONT_INVALID  /* Invalid status                           */
} snd_cmx_af_saf_cont_type;

typedef struct {
  snd_cmx_af_saf_cont_type  cont;          /* SAF data status         */
  snd_cmx_af_buf_rtn_type   *buf_rtn;      /* SAF data info           */
  snd_cmx_af_pos_enum_type  xpos_type;     /* Position in X plane     */
  snd_cmx_af_pos_enum_type  ypos_type;     /* Position in Y plane     */
  uint8                     xpos_num;      /* percent or pixel offset */
                                           /* in X plane              */
  uint8                     ypos_num;      /* percent or pixel offset */
                                           /* in Y plane              */
  uint32                    objbuf_size;   /* Size of object buffer   */
} snd_cmx_af_saf_type;

/* Server data for SVG comamnd */
typedef struct {
  uint32   size;                           /* SVG data size info      */
} snd_cmx_af_svg_data_type;

/* Server data for SVG frame comamnd */
typedef struct {
  snd_cmx_af_pos_enum_type  xpos_type;     /* Position in X plane     */
  snd_cmx_af_pos_enum_type  ypos_type;     /* Position in Y plane     */
  uint8                     xpos_num;      /* percent or pixel offset */
                                           /* in X plane              */
  uint8                     ypos_num;      /* percent or pixel offset */
                                           /* in Y plane              */
} snd_cmx_af_svg_frame_type;

/* Bitmask of elements contained in c-MIDI file */
typedef enum {
  SND_CMID_CONTENTS_SONG    = 0x00000001, /* Content contains MIDI commands  */
  SND_CMID_CONTENTS_WAVE    = 0x00000002, /* Content contains WAV data       */
  SND_CMID_CONTENTS_TEXT    = 0x00000004, /* Content contains text events    */
  SND_CMID_CONTENTS_PIC     = 0x00000008, /* Content contains picture events */
  SND_CMID_CONTENTS_ANIM    = 0x00000010, /* Content contains animation      */
  SND_CMID_CONTENTS_LED     = 0x00000020, /* Content contains LED events     */
  SND_CMID_CONTENTS_VIB     = 0x00000040, /* Content contains VIB events     */
  SND_CMID_CONTENTS_UNKNOWN = (int)0x80000000  /* Content contains unrecognized   */
                                          /* elements                        */
} snd_cmid_contents_mask_type;

/* The following structure contains WebAudio file specifications.  It is used
** with the SND_CMX_AF_AUDIO_SPEC playback callback and the
** cmx_audfmt_get_specs() function.
*/
typedef struct {
  snd_cmx_af_file_type           file_type; /* Format of audio file           */
  snd_supported_ops_mask_type    supported_ops; /* Supported operations       */
  snd_cmx_af_char_set_enum_type  char_code; /* Char set used by text commands */
  uint32                         contents;  /* Bitmask of cMidi file contents */
} snd_cmid_spec_type;

typedef snd_audfmt_spec_type snd_mfi_spec_type;

#ifdef FEATURE_SMAF
/* Server data for SMAF graphics track chunk information */
typedef struct {
  uint8   num;                          /* Graphics Track number              */
  uint32  pos;                          /* Graphics Track data start position */
  uint32  size;                         /* Graphics Track data size           */
} snd_cmx_smaf_graph_info_type;

/* Server data for SMAF led/vibrator control information */
typedef enum {
	SND_SMAF_LED_CTL_STATUS_OFF = 0,
	SND_SMAF_LED_CTL_STATUS_ON,
} snd_cmx_smaf_led_ctl_status_enum_type;

typedef enum {
	SND_SMAF_VIB_CTL_STATUS_OFF = 0,
	SND_SMAF_VIB_CTL_STATUS_ON,
} snd_cmx_smaf_vib_ctl_status_enum_type;

typedef struct {
  snd_cmx_smaf_led_ctl_status_enum_type smaf_led_ctl;
  snd_cmx_smaf_vib_ctl_status_enum_type smaf_vib_ctl;
} snd_cmx_smaf_led_vib_ctl_type;

/* The following structure contains SMAF file specifications.  It is used
** with the SND_CMX_AF_AUDIO_SPEC playback callback and the
** cmx_audfmt_get_specs() function.
*/
typedef struct {
  snd_cmx_af_file_type           file_type; /* Format of audio file           */
  snd_supported_ops_mask_type    supported_ops; /* Supported operations       */
  char                           title[30];    /* Song Title                 */
  char                           artist[30];   /* Artist Name                */
  char                           copyright[30]; /* Copyright                 */
  uint8                          code_type;     /* Character set             */
} snd_smaf_spec_type;

#endif /* FEATURE_SMAF */

/* The following type is used to control LED operation for MFi files.
*/
typedef enum {
  SND_CMX_AF_MFI_LED_MASK_TYPE1 = 0x01,   /* LED type 1 */
  SND_CMX_AF_MFI_LED_MASK_TYPE2 = 0x02,   /* LED type 2 */
  SND_CMX_AF_MFI_LED_MASK_TYPE3 = 0x04    /* LED type 3 */
} snd_cmx_af_mfi_led_mask_type;

/* This structure contains a mask of LED types that are enabled. This
** structure is used with the SND_CMX_MFI_LED_CONTROL callback status type.
*/
typedef struct {
  uint32 led_ctl_mask;   /* Mask of enabled LEDs */
} snd_cmx_mfi_led_ctl_type;
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_MPEG_AUDIO
/* MPEG layer info
*/
typedef enum {  
  SND_MP3_LAYER_RESERVED = 0,  /* Reserved                                    */
  SND_MPEG2_LAYER_AAC = SND_MP3_LAYER_RESERVED,  /* MPEG2 AAC compression     */
  SND_MP3_LAYER_3,             /* MPEG Layer 3 compression                    */
  SND_MP3_LAYER_2,             /* MPEG Layer 2 compression                    */
  SND_MP3_LAYER_1,             /* MPEG Layer 1 compression                    */
  SND_MP3_LAYER_UNKNOWN        /* Unable to determine layer information       */
} snd_mpeg_layer_enum_type;
#endif /* FEATURE_MPEG_AUDIO */

#ifdef FEATURE_MP3
/* MP3 Version info
*/
typedef enum {  
  SND_MP3_VER_25,            /* MPEG Version 2.5                              */
  SND_MP3_VER_RESERVED,      /* Reserved                                      */
  SND_MP3_VER_2,             /* MPEG Version 2.0                              */
  SND_MP3_VER_1,             /* MPEG Version 1.0                              */
  SND_MP3_VER_UNKNOWN        /* Unable to determine version information       */
} snd_mp3_ver_enum_type;

/* MP3 bitrate info
*/
typedef enum {  
  SND_MP3_BITRATE_FREE = 0,   /* Free bitrate (determined by software)        */
  SND_MP3_BITRATE_8K   = 8,   /* Fixed bitrates                               */
  SND_MP3_BITRATE_16K  = 16,  /*                                              */
  SND_MP3_BITRATE_24K  = 24,  /*                                              */
  SND_MP3_BITRATE_32K  = 32,  /*                                              */
  SND_MP3_BITRATE_40K  = 40,  /*                                              */
  SND_MP3_BITRATE_48K  = 48,  /*                                              */
  SND_MP3_BITRATE_56K  = 56,  /*                                              */
  SND_MP3_BITRATE_64K  = 64,  /*                                              */
  SND_MP3_BITRATE_80K  = 80,  /*                                              */
  SND_MP3_BITRATE_96K  = 96,  /*                                              */
  SND_MP3_BITRATE_112K = 112, /*                                              */
  SND_MP3_BITRATE_128K = 128, /*                                              */
  SND_MP3_BITRATE_144K = 144, /*                                              */
  SND_MP3_BITRATE_160K = 160, /*                                              */
  SND_MP3_BITRATE_176K = 176, /*                                              */
  SND_MP3_BITRATE_192K = 192, /*                                              */
  SND_MP3_BITRATE_224K = 224, /*                                              */
  SND_MP3_BITRATE_256K = 256, /*                                              */
  SND_MP3_BITRATE_288K = 288, /*                                              */
  SND_MP3_BITRATE_320K = 320, /*                                              */
  SND_MP3_BITRATE_352K = 352, /*                                              */
  SND_MP3_BITRATE_384K = 384, /*                                              */
  SND_MP3_BITRATE_416K = 416, /*                                              */
  SND_MP3_BITRATE_448K = 448, /*                                              */
  SND_MP3_BITRATE_VAR = 500,  /* Variable bitrate (Changes each frame)        */
  SND_MP3_BITRATE_UNK = 501   /* Unable to determine bitrate information      */
} snd_mp3_bitrate_enum_type;

/* MP3 Channel info
*/
typedef enum {  
  SND_MP3_CHANNEL_STEREO,       /* Stereo data                                */
  SND_MP3_CHANNEL_JOINT_STEREO, /* Joint stereo data                          */
  SND_MP3_CHANNEL_DUAL,         /* Dual channel (stereo) data                 */
  SND_MP3_CHANNEL_SINGLE        /* Single channel (mono) data                 */
} snd_mp3_channel_enum_type;

/* MP3 Extension info
** The extension bits are valid when SND_MP3_CHANNEL_JOINT_STEREO is set.
*/
typedef enum {  
  /* For Layer 1 & 2 files */ 
  SND_MP3_EXT_BAND_4_31 = 0,            /* Channel extension info, 4-31       */
  SND_MP3_EXT_BAND_8_31 = 1,            /*                         8-31       */
  SND_MP3_EXT_BAND_12_31 = 2,           /*                        12-31       */
  SND_MP3_EXT_BAND_16_31 = 3,           /*                        16-31       */
  /* For Layer 3 files */ 
  SND_MP3_EXT_INTENSITY_OFF_MS_OFF = 0, /* Intensity stereo off, MS off       */
  SND_MP3_EXT_INTENSITY_ON_MS_OFF = 1,  /*                  on      off       */
  SND_MP3_EXT_INTENSITY_OFF_MS_ON = 2,  /*                  off     on        */
  SND_MP3_EXT_INTENSITY_ON_MS_ON = 3    /*                  on      on        */
} snd_mp3_ext_enum_type;

/* MP3 Emphasis info
*/
typedef enum {
  SND_MP3_EMPHASIS_NONE,     /* Emphasis flag                                 */
  SND_MP3_EMPHASIS_50_15_MS, /*                                               */
  SND_MP3_EMPHASIS_RESERVED, /*                                               */
  SND_MP3_EMPHASIS_CCITT_J17 /*                                               */
} snd_mp3_emphasis_enum_type;

/* ID3V1 Tag
 */
typedef struct {
  char             title[30];   /* MP3 song title            */
  char             artist[30];  /* MP3 artist name           */
  char             album[30];   /* MP3 album name            */
  char             year[4];     /* MP3 recording year        */
  char             comment[30]; /* MP3 ID3 Tag comments      */
  byte             genre;       /* MP3 ID3 genre number      */
} snd_id3v1_tag_type;

/* ID3V2 tag inforamtion */
typedef enum {
  SND_ID3V2_TAG_INFO_BY_POSITION,
  SND_ID3V2_TAG_INFO_BY_CONTENT
} snd_id3v2_tag_info_type;

/* ID3V2 Tag Generic frame content for other frames */

typedef struct {
  snd_id3v2_tag_info_type info_type;
  char    id[5];             /* id of this frame              */
  int16   flags;             /* flags of this frame            */
  union {
    uint8   *frame_content;    /* content of this frame         */
    uint32  file_position;     /* Frame content position in file */
  } frame_data;
  uint32  frame_size;        /* content size                  */
} snd_id3v2_generic_frame_type; 

/* ID3V2 Tag frame info for other frames. 
 *
 */
typedef struct {
  boolean found;             /* whether this frame existing   */
  uint16   flags;            /* flags of this frame           */
  uint32  file_position;     /* Frame content position in file*/
  uint32 size;               /* Size of the content */
} snd_id3v2_frame_info_type;

/* ID3V2 Tag frame content
 *
 */
typedef struct {
  uint8  *content;
  uint16 flag;
  uint32 size;
} snd_id3v2_tag_frame_type;

/* ID3V2 Tag content,
 * frames we supported
 */
typedef struct {
  uint16 tag_ver;
  uint8  tag_flag;

  snd_id3v2_tag_frame_type  title;
  snd_id3v2_tag_frame_type  subtitle;
  snd_id3v2_tag_frame_type  copyright;
  snd_id3v2_tag_frame_type  produced;
  snd_id3v2_tag_frame_type  composer;
  snd_id3v2_tag_frame_type  artist;
  snd_id3v2_tag_frame_type  orchestra;
  snd_id3v2_tag_frame_type  conductor; 
  snd_id3v2_tag_frame_type  lyricist;
  snd_id3v2_tag_frame_type  album;
  snd_id3v2_tag_frame_type  track;
  snd_id3v2_tag_frame_type  year;
  snd_id3v2_tag_frame_type  publisher;
  snd_id3v2_tag_frame_type  genre;
  snd_id3v2_tag_frame_type  station;
  snd_id3v2_tag_frame_type  encoder;
  snd_id3v2_tag_frame_type  length;
  snd_id3v2_frame_info_type apic;     /* Attached picture Information */ 
  snd_id3v2_generic_frame_type *gframes; /* Generic frames */
  uint32   gframes_cnt; /* Generic frame count */   
} snd_id3v2_tag_type;

/* ID3 Tag
 */
typedef struct {
  boolean  v1_tag_found;
  snd_id3v1_tag_type   v1_tag;

  boolean  v2_tag_found;
  snd_id3v2_tag_type   v2_tag;
} snd_id3_tag_type;

/* The following structure contains MP3 file specifications.  It is used with
** the SND_CMX_AF_AUDIO_SPEC playback callback and the cmx_audfmt_get_specs()
** function.
*/
typedef struct {
  snd_cmx_af_file_type              file_type;   /* Format of audio file      */
  snd_supported_ops_mask_type       supported_ops; /* Supported operations    */
  snd_mp3_ver_enum_type             version;     /* MPEG version              */
  snd_mpeg_layer_enum_type          layer;       /* MPEG layer compression    */
  boolean                           crc_f;       /* true = MPEG CRC protection*/
  snd_mp3_bitrate_enum_type         bitrate;     /* MPEG audio bitrate        */
  snd_sample_rate_type              sample_rate; /* MPEG audio sample rate    */
  snd_mp3_channel_enum_type         channel;     /* MPEG audio channel        */
  snd_mp3_ext_enum_type             extension;   /* MPEG audio chan. extension*/
  boolean                           copyright_f; /* true = copyrighted        */
  boolean                           original_f;  /* true = original recording */
  snd_mp3_emphasis_enum_type        emphasis;    /* MPEG audio emphasis       */

  snd_id3_tag_type                  id3_tag;     /* id3_tag information       */
} snd_mp3_spec_type;
#endif /* FEATURE_MP3 */


#ifdef FEATURE_AAC

/* MPEG4 audioObjectType
*/
typedef enum {
  SND_MP4_AUDIO_OBJECT_NULL            = 0,
  SND_MP4_AUDIO_OBJECT_AAC_MAIN,
  SND_MP4_AUDIO_OBJECT_AAC_LC,
  SND_MP4_AUDIO_OBJECT_AAC_SSR,
  SND_MP4_AUDIO_OBJECT_AAC_LTP,
  SND_MP4_AUDIO_OBJECT_AAC_SCALABLE    = 6,
  SND_MP4_AUDIO_OBJECT_ER_AAC_LC       = 17,
  SND_MP4_AUDIO_OBJECT_ER_AAC_LTP      = 19,
  SND_MP4_AUDIO_OBJECT_ER_AAC_SCALABLE = 20,
#ifdef FEATURE_AVS_BSAC  
  SND_MP4_AUDIO_OBJECT_BSAC            = 22,
#endif /* FEATURE_AVS_BSAC */
  SND_MP4_AUDIO_OBJECT_ER_AAC_LD       = 23,

  /* MPEG2 AAC entries for backward compatibility */
  /*
  SND_AAC_PROFILE_MAIN = SND_MP4_AUDIO_OBJECT_NULL,
  SND_AAC_PROFILE_LC   = SND_MP4_AUDIO_OBJECT_AAC_MAIN,
  SND_AAC_PROFILE_SSR  = SND_MP4_AUDIO_OBJECT_AAC_LC,
  */
  SND_MP4_AUDIO_OBJECT_UNKNOWN
} snd_mp4_audio_object_enum_type;

/* AAC Channel info
*/
typedef enum {  
  SND_AAC_CHANNEL_UNKNOWN = 0,
  SND_AAC_CHANNEL_MONO,       /* Single channel (mono) data                  */
  SND_AAC_CHANNEL_DUAL,       /* Stereo data                                 */
  SND_AAC_CHANNEL_TRIPLE,     /* 3 channels: 1+2 (UNSUPPORTED)               */
  SND_AAC_CHANNEL_QUAD,       /* 4 channels: 1+2+1 (UNSUPPORTED)             */
  SND_AAC_CHANNEL_QUINTUPLE,  /* 5 channels: 1+2+2 (UNSUPPORTED)             */
  SND_AAC_CHANNEL_SEXTUPLE,   /* 5+1 channels: 1+2+2+1 (UNSUPPORTED)         */
  SND_AAC_CHANNEL_OCTUPLE,    /* 7+1 channels: 1+2+2+2+1 (UNSUPPORTED)       */
  SND_AAC_CHANNEL_DUAL_MONO,  /* Dual Mono: 1+1 (Two SCEs)                   */
  SND_AAC_CHANNEL_UNSUPPORTED /* Indicating CMX is currently playing unsupported
                                    Channel mode.                            */
} snd_aac_channel_enum_type;

typedef struct {

  snd_cmx_af_file_type        file_type;   /* Format of audio file      */
  snd_supported_ops_mask_type supported_ops; /* Supported operations    */
  snd_mpeg_layer_enum_type    layer;       /* MPEG layer compression    */
  boolean                     crc_f;       /* false = CRC present       */
  snd_mp4_audio_object_enum_type audio_object; /* Audio Object type     */
  snd_sample_rate_type        sample_rate; /* MPEG audio sample rate    */
  boolean                     private_f;
  snd_aac_channel_enum_type   channel;     /* MPEG audio channel        */
  boolean                     original_f;  /* true = original recording */
  boolean                     home_f;
  uint32                      bit_rate;    /* Computed bit rate         */
  char                        title[30];   /* AAC song title            */
  char                        artist[30];  /* AAC artist name           */
  char                        album[30];   /* AAC album name            */
  char                        year[4];     /* AAC recording year        */
  char                        comment[30]; /* AAC ID3 Tag comments      */
  byte                        genre;       /* AAC ID3 genre number      */
} snd_aac_spec_type;

typedef enum {
  SND_AAC_DATA_FORMAT_ADIF,       /* ADIF                   */
  SND_AAC_DATA_FORMAT_ADTS,       /* ADTS                   */
  SND_AAC_DATA_FORMAT_RAW,        /* Raw (PV)               */
  SND_AAC_DATA_FORMAT_PSUEDO_RAW, /* Raw with frame markers */
  SND_AAC_DATA_FORMAT_LOAS        /* LOAS format            */
} snd_aac_data_format_enum_type;

typedef enum {
  SND_AF_DUAL_MONO_MODE_FL_FR, /* 1st SCE to left & right */
  SND_AF_DUAL_MONO_MODE_SL_SR, /* 2nd SCE to left & right */
  SND_AF_DUAL_MONO_MODE_SL_FR, /* 2nd SCE to left, main to right */
  SND_AF_DUAL_MONO_MODE_FL_SR, /* 1st SCE to left, sub to right default */
  SND_AF_DUAL_MONO_MODE_DEFAULT = 
  SND_AF_DUAL_MONO_MODE_FL_SR
} snd_af_dual_mono_mode_enum_type;

typedef enum {
  AUDIODEF_AF_SBR_MODE_DISABLE,
  AUDIODEF_AF_SBR_MODE_ENABLE,
  AUDIODEF_AF_SBR_MODE_DEFAULT = 
  AUDIODEF_AF_SBR_MODE_ENABLE
} snd_af_sbr_mode_enum_type;

/* This structure is used to indicate the information needed to configure
** the AAC codec for playback of raw data.
*/
typedef struct {
  snd_cmx_af_codec_type            codec_type;  /* Codec type indicator      */
  snd_aac_data_format_enum_type    format;      /* AAC encoding format       */
  snd_mp4_audio_object_enum_type   audio_object;/* Audio Object type         */
  snd_sample_rate_type             sample_rate; /* MPEG audio sample rate    */
  snd_aac_channel_enum_type        channel;     /* MPEG audio channel        */
  uint32                           bit_rate;    /* Computed bit rate         */

  /* See ISO/IEC 14496-3:2001(E) for details on the parameters below */
  byte                             ep_config;
  byte                             aac_section_data_resilience_flag;
  byte                             aac_scalefactor_data_resilience_flag;
  byte                             aac_spectral_data_resilience_flag;
  snd_af_dual_mono_mode_enum_type  dual_mono_mode;
  snd_af_sbr_mode_enum_type        sbr_mode;
} snd_aac_format_spec_type;

/* Structure defined for AAC codec info feedback when
   channel or sample rate changed in the bitstream */
typedef struct {
  snd_cmx_af_file_type      codec_type;
  snd_aac_channel_enum_type channel_mode;
  uint32                    sample_rate;
} snd_af_aac_info_type;

typedef union {
  snd_cmx_af_file_type    codec_type;
  snd_af_aac_info_type    aac_info;
} snd_cmx_af_info_type;

#endif /* FEATURE_AAC */

#ifdef FEATURE_IMELODY
typedef struct {

  snd_cmx_af_file_type           file_type;     /* Format of audio file    */
  snd_supported_ops_mask_type    supported_ops; /* Supported operations    */
  byte                           format;
  char                           name[76];      /* song title              */
  char                           composer[76];  /* artist name             */
  uint16                         beat;
  byte                           style;
} snd_imy_spec_type;
#endif /* FEATURE_IMELODY */

#ifdef FEATURE_REAL_AUDIO
/* Number of audio channels
*/
typedef enum {  
  SND_RA_CHANNEL_MONO = 1,   /* Single channel (mono) data                */
  SND_RA_CHANNEL_DUAL        /* Stereo data                               */
} snd_ra_channel_type;

typedef enum {
  SND_RA_DECODER_GECKO1 = 1,
  SND_RA_DECODER_GECKO2
} snd_ra_decoder_mode_type;

typedef enum {
  SND_RA_SAMPLES_PER_FRAME_256  = 256,
  SND_RA_SAMPLES_PER_FRAME_512  = 512,
  SND_RA_SAMPLES_PER_FRAME_1024 = 1024
} snd_ra_samples_per_frame_type;

/* Structure used to indicate information about RealAudio file to be played
*/
typedef struct {
  snd_cmx_af_codec_type         codec_type;     /* Codec type indicator     */
  snd_ra_decoder_mode_type      decoder_mode;   /* Decoder (Gecko) mode     */
  snd_sample_rate_type          sample_rate;    /* Sampling rate            */
  snd_ra_channel_type           num_channels;   /* Number of channels       */
  snd_ra_samples_per_frame_type samples_per_frame; /* Samples per frame     */
  uint16                        num_regions;    /* Number of regions        */
  uint16                        bits_per_frame; /* Number of bits per frame */
  uint16                        coupling_start; /* Stereo coupling region   */
  uint16                        coupling_quant; /* Coupling quantization    */
  uint16                        frame_erasure;  /* Frame erasure flag       */
} snd_ra_format_spec_type;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WAVE_PB
typedef struct {
  snd_cmx_af_file_type          file_type;   /* Format of audio file         */
  snd_supported_ops_mask_type   supported_ops; /* Supported operations       */
  uint16                        channels;    /* Number of channels           */
  uint32                        sample_rate; /* Samples per second           */
  uint32                        bytes_rate;  /* Average bytes per second     */
  uint16                        frame_size;  /* Block Align, num of bytes    */
                                          /* for n-channels                  */
  uint16                        bits_per_sample; /* Number of bits per sample*/
} snd_wave_spec_type;

#ifdef FEATURE_PCM
/* This type is used to indicate the format of PCM samples
*/
typedef enum {
  SND_WAVE_SAMPLE_FORMAT_SIGNED,    /* PCM samples are signed   */
  SND_WAVE_SAMPLE_FORMAT_UNSIGNED,  /* PCM samples are unsigned */
  SND_WAVE_SAMPLE_FORMAT_INVALID
} snd_wave_sample_format_type;

/* This structure is used to start playback of RAW PCM data
*/
typedef struct {
  snd_cmx_af_file_type        file_type;       /* Format of audio file       */
  snd_wave_sample_format_type sample_fmt;      /* Format of PCM samples      */
  uint32                      sample_rate;     /* Sampling rate in Hz        */
  uint16                      bits_per_sample; /* Number of bits per sample  */
  uint16                      channels;        /* Number of channels         */
} snd_wave_codec_type;
#endif /* FEATURE_PCM */
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_WMA
/* This structure is used to start playback of Window Media Audio data
*/
typedef struct{
  snd_cmx_af_file_type file_type;          /* Format of audio file      */
  snd_sample_rate_type sample_rate;        /* Sampling rate in Hz       */
  uint8                bits_per_sample;    /* Valid bits per sample     */
  uint16               version;            /* Only Version 2 supported  */
  uint16               channels;           /* Number of channels 1 or 2 */
  uint16               bytes_per_second;   /* Number of bits per second */
  uint16               encoder_options;    /* Encoder options           */
  uint16               virtual_pkt_len;    /* VirtulaPacketLength in ASF*/
  uint16               advanced_encoder_opt1; /* Support for LBR bitstream */
  uint16               channel_mask;         /* For stero or mono streams  */
  uint16               format_tag;           /* Codec ID: 0x162 or 0x166 for WMAPRO 10 */
  uint32               advanced_encoder_opt2; /* Support for LBR bitstream */

} snd_wma_format_spec_type;
#endif /* FEATURE_WMA */

#if defined(FEATURE_AMR_WB_AUDIO_DEC) || defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)
typedef struct {
  snd_cmx_af_codec_type codec_type;   /* Codec type indicator */
  snd_sample_rate_type  sample_rate;  /* Sampling rate        */
  snd_channel_enum_type channel_mode; /* Number of channels   */
} snd_ext_speech_format_spec_type;
#endif /* FEATURE_AMR_WB_AUDIO_DEC || FEATURE_AMR_WBPLUS_AUDIO_DEC */

#ifdef FEATURE_QCP
typedef struct {
  snd_cmx_af_file_type           file_type; /* Format of the audio file     */
  snd_supported_ops_mask_type    supported_ops; /* Supported operations     */
} snd_qcp_spec_type;
#endif /* FEATURE_QCP */

/* Server type for SND_CMX_AF_AUDIO_SPEC callback. Currently only contains
** character code set used by text commands.
*/
typedef union {
  snd_cmx_af_file_type           file_type; /* Format of the audio file       */
#ifdef FEATURE_MIDI
  snd_midi_spec_type             midi_spec; /* Specs for MIDI files           */
#endif /* FEATURE_MIDI */
#ifdef FEATURE_WEBAUDIO
  snd_cmid_spec_type             cmid_spec; /* Specs for CMID files           */
  snd_mfi_spec_type              mfi_spec;  /* Specs for MFI files            */
#ifdef FEATURE_SMAF
  snd_smaf_spec_type             smaf_spec; /* Specs for SMAF files           */
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */
#ifdef FEATURE_MP3
  snd_mp3_spec_type              mp3_spec;  /* Specs for MP3 files            */
#endif /* FEATURE_MP3 */
#ifdef FEATURE_AAC
  snd_aac_spec_type              aac_spec;  /* Specs for AAC files            */
#endif /* FEATURE_AAC */
#ifdef FEATURE_IMELODY
  snd_imy_spec_type              imy_spec;  /* Specs for IMY files            */
#endif /* FEATURE_IMELODY */
#ifdef FEATURE_WAVE_PB
  snd_wave_spec_type             wave_spec; /* Specs for WAV files            */
#endif /* FEATURE_WAVE_PB */
#ifdef FEATURE_QCP
  snd_qcp_spec_type              qcp_spec;  /* Specs for QCP files            */
#endif /* FEATURE_QCP */
} snd_cmx_af_spec_type;

/* This is a union of supported audio codecs where format is specified by the
** client.
*/
typedef union {
  snd_cmx_af_codec_type          codec_type;  /* Codec type                   */
#ifdef FEATURE_AAC
  snd_aac_format_spec_type       aac_spec;    /* Specs for AAC files          */
#endif
#ifdef FEATURE_REAL_AUDIO
  snd_ra_format_spec_type        ra_spec;     /* RealAudio specs              */
#endif /* FEATURE_REAL_AUDIO */
#ifdef FEATURE_PCM
  snd_wave_codec_type            wave_codec;  /* WAV codec type               */
#endif /* FEATURE_PCM */
#ifdef FEATURE_WMA
  snd_wma_format_spec_type       wma_spec;    /* WMA codec type               */
#endif
#if defined(FEATURE_AMR_WB_AUDIO_DEC) || defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)
  snd_ext_speech_format_spec_type ext_speech_spec; /* Ext. speech codec type  */
#endif /* FEATURE_AMR_WB_AUDIO_DEC || FEATURE_AMR_WBPLUS_AUDIO_DEC */
} snd_cmx_format_spec_type;

/* Callback function type for sound server client interaction
*/
typedef void (* snd_af_data_cb_func_type) (
  snd_af_data_cmd_enum  command,        /* Command/Status of callback       */
  void                  *audfmt_file,   /* File to which command pertains   */
  uint32                num,            /* Generic number determined by cmd */
  void                  *data           /* Data from sound server to client */
                                        /* also determined by command       */
);

/* Descriptor of required information for sound server to callback client
** with status/commands.
*/
typedef struct {
  void                     *audfmt_ptr; /* Handle to be referenced by client */
  snd_af_data_cb_func_type data_func;   /* callback function pointer         */
} snd_af_access_type;

/* The following enumerated type is used to indicate whether the audio
** file should be started from the cue point or from the begining (normal).
*/
typedef enum {
  SND_AUDFMT_START_NORMAL,      /* Start from normal position (beginning)    */
  SND_AUDFMT_START_CUE_POINT,   /* Start from cue point position if possible */
                                /* and repeat indefinitely                   */
  SND_AUDFMT_START_CUE_POINT_1, /* Start from cue point position if possible */
                                /* and play one iteration                    */
  SND_AUDFMT_START_CUE_POINT_2, /* Start from cue point position if possible */
                                /* and play two iterations                   */
  SND_AUDFMT_START_CUE_POINT_3, /* Start from cue point position if possible */
                                /* and play three iterations                 */
  SND_AUDFMT_START_CUE_POINT_4, /* Start from cue point position if possible */
                                /* and play four iterations                  */
  SND_AUDFMT_START_CUE_POINT_5, /* Start from cue point position if possible */
                                /* and play five iterations                  */

  /* Adding mutually exclusive entries to this enumerated type. The cue point
  ** enums are only applicable to WebAudio files, whereas the path enums
  ** are only applicable to QCP files. This was done in an effort to reuse
  ** parameters in existing APIs instead of adding a new enumerated type
  ** and modifying the APIs.
  */
  SND_AUDFMT_START_PATH_FORWARD,  /* Play on the forward path            */
  SND_AUDFMT_START_PATH_REVERSE,  /* Play on the reverse path            */
  SND_AUDFMT_START_PATH_BOTH,     /* Play on both paths                  */
  SND_AUDFMT_START_PATH_FORWARD_INCALL, /* Forward path and mixed with voice */
  SND_AUDFMT_START_PATH_BOTH_INCALL,    /* Both paths and mixed with voice   */
} snd_audfmt_start_enum_type;

/* The following is used to indicate the type of AV Sync interval*/
typedef enum {
  SND_AV_SYNC_SAMPLES,    /* av sync interval in samples or frames   */
  SND_AV_SYNC_TIME_MS,    /* av sync interval in milliseconds        */
  SND_AV_SYNC_INVALID     /* Designates an invalid type              */
} snd_av_sync_interval_type;

/* AV Sync callback function
*/
typedef void (*snd_af_av_sync_cb_func_ptr_type) (
  qword timestamp,         /* Timestamp for the callback in ms            */    
  qword num_of_samples,    /* Sample counter. Use only lower 48 bits      */
  qword num_of_bytes,      /* Sample counter. Use only lower 48 bits      */
  const void  *client_data /* av sync client data                         */
);

typedef struct {
  qword timestamp;           /* timestamp for the callback in ms               */    
  qword num_of_samples;      /* sample/frame counter. Use only lower 48 bits   */
  qword num_of_bytes;        /* bytes/frame counter. Use only for window media */
  uint32 sample_rate;        /*  sample rate */
} snd_af_av_sync_info_type;

/* Audio Video Sync extendable callback function */
typedef void (*snd_af_av_sync_ex_cb_func_ptr_type) (
  snd_af_av_sync_info_type  avsync_info,
  const void                *client_data      /* av sync client data */
);

/* Descriptor of required information for AV sync callbacks 
*/
typedef struct {
  uint32                          av_sync_interval; /* Interval in ms  */
  snd_av_sync_interval_type       av_sync_mode;     /* ms or samples   */ 
  snd_af_av_sync_cb_func_ptr_type av_sync_cb_func;  /* AV Sync Host cb */
  const void                      *client_data; /* client correlation data  */ 
  /* Extended AV Sync Cb */
  snd_af_av_sync_ex_cb_func_ptr_type av_sync_ex_cb_func; 
} snd_af_av_sync_type;

/* Type specification for an audio format (midi/WebAudio) sound.
*/
typedef struct {
  snd_type               type;          /* sound type                        */
  snd_af_access_type     audfmt_handle; /* Info to interact with client (get */
                                        /* data, send status/info)           */
  snd_audfmt_start_enum_type            /* Sound will start from cue point   */
                         start_position;/* if it exists                      */
  uint16                 repeat_ms;     /* Repeat timer, sound will repeat   */
                                        /* after this amount of silence      */
                                        /* (zero = do not repeat)            */

  /* This structure contains playback parameters for this sound */
  struct {
    uint32   repeat_ms:16;      /* Repeat time in ms                         */
    uint32   repeat_cnt:8;      /* Repeat loop count (0 = loop forever)      */
    uint32   repeat_enable:1;   /* Repeat mode enable flag                   */
    uint32   cuepoint_enable:1; /* Cuepoint mode enable flag                 */
    uint32   multiseq_flag:1;   /* Multisequencer flag                       */
    uint32   reserved:5;        /* Reserved bits                             */
    uint32   byte_offset;       /* Offset from start of file where content   */
                                /* actually begins                           */
  } param;
} snd_audfmt_sound_type;

/* Type specification for a sound where audio codec is specified by client.
*/
typedef struct {
  snd_type                 type;          /* sound type                      */
  snd_af_access_type       audfmt_handle; /* Info to interact with client    */
                                          /* (get data, send status/info)    */
  snd_cmx_format_spec_type input_spec;    /* File format details             */
  snd_af_av_sync_type      av_sync;       /* AV Sync related information     */
} snd_format_sound_type;

#ifdef FEATURE_QCP
/* This enumerated type is used to indicate the type of function to
** perform on the QCP file.
*/
typedef enum {
  SND_QCP_FUNC_GET_DATA_SIZE,    /* Get size of data chunk     */
  SND_QCP_FUNC_GET_FRAMES,       /* Get number of frames       */
  SND_QCP_FUNC_CNFG,             /* Get/set cnfg chunkk value  */
  SND_QCP_FUNC_LABL,             /* Get/set labl chunk         */
  SND_QCP_FUNC_TEXT,             /* Get/set text chunk         */
  SND_QCP_FUNC_INVALID           /* Invalid type               */
} snd_qcp_func_enum_type;

/* This enumerated type is used to indicate the mode/quality 
** for QCP recording.
*/
typedef enum {
  SND_QCP_FORMAT_FIXED_FULL_13K,  /* Fixed-full rate, 13K vocoder       */
  SND_QCP_FORMAT_VAR_FULL_13K,    /* Variable rate, full rate max, 13K  */
  SND_QCP_FORMAT_FIXED_FULL_EVRC, /* Fixed-full rate, EVRC vocoder      */
  SND_QCP_FORMAT_VAR_FULL_EVRC,   /* Variable rate, full rate max, EVRC */

  SND_QCP_FORMAT_FIXED_HALF_13K,  /* Fixed half rate, 13K vocoder       */
  SND_QCP_FORMAT_VAR_HALF_13K,    /* Varibale rate, half rate max, 13K  */

  SND_QCP_FORMAT_VAR_FULL_4GV_NB,   /* Variable rate, full rate max, 4GV_NB */
  SND_QCP_FORMAT_FIXED_FULL_4GV_NB, /* Fixed rate, full rate max, 4GV_NB    */

  SND_QCP_FORMAT_VAR_FULL_4GV_WB,   /* Variable rate, full rate max, 4GV_WB */
  SND_QCP_FORMAT_FIXED_FULL_4GV_WB, /* Fixed rate, full rate max, 4GV_WB   */

  SND_QCP_FORMAT_FIXED_4_75_AMR,  /* Fixed- 4.75 kbit/s, AMR            */
  SND_QCP_FORMAT_FIXED_5_15_AMR,  /* Fixed- 5.15 kbit/s, AMR            */
  SND_QCP_FORMAT_FIXED_5_9_AMR,   /* Fixed- 5.9  kbit/s, AMR            */
  SND_QCP_FORMAT_FIXED_6_7_AMR,   /* Fixed- 6.7  kbit/s, AMR            */
  SND_QCP_FORMAT_FIXED_7_4_AMR,   /* Fixed- 7.4  kbit/s, AMR            */
  SND_QCP_FORMAT_FIXED_7_95_AMR,  /* Fixed- 7.95 kbit/s, AMR            */
  SND_QCP_FORMAT_FIXED_10_2_AMR,  /* Fixed-10.2  kbit/s, AMR            */
  SND_QCP_FORMAT_FIXED_12_2_AMR,  /* Fixed-12.2  kbit/s, AMR            */

  SND_QCP_FORMAT_FIXED_4_75_AMR_DTX, /* Fixed- 4.75 kbit/s, AMR w DTX enabled */
  SND_QCP_FORMAT_FIXED_5_15_AMR_DTX, /* Fixed- 5.15 kbit/s, AMR w DTX enabled */
  SND_QCP_FORMAT_FIXED_5_9_AMR_DTX,  /* Fixed- 5.9  kbit/s, AMR w DTX enabled */
  SND_QCP_FORMAT_FIXED_6_7_AMR_DTX,  /* Fixed- 6.7  kbit/s, AMR w DTX enabled */
  SND_QCP_FORMAT_FIXED_7_4_AMR_DTX,  /* Fixed- 7.4  kbit/s, AMR w DTX enabled */
  SND_QCP_FORMAT_FIXED_7_95_AMR_DTX, /* Fixed- 7.95 kbit/s, AMR w DTX enabled */
  SND_QCP_FORMAT_FIXED_10_2_AMR_DTX, /* Fixed-10.2  kbit/s, AMR w DTX enabled */
  SND_QCP_FORMAT_FIXED_12_2_AMR_DTX, /* Fixed-12.2  kbit/s, AMR w DTX enabled */

#ifdef FEATURE_AUDFMT_AMR
  SND_AMR_FORMAT_FIXED_4_75_AMR,  /* Fixed- 4.75 kbit/s, AMR            */
  SND_AMR_FORMAT_FIXED_5_15_AMR,  /* Fixed- 5.15 kbit/s, AMR            */
  SND_AMR_FORMAT_FIXED_5_9_AMR,   /* Fixed- 5.9  kbit/s, AMR            */
  SND_AMR_FORMAT_FIXED_6_7_AMR,   /* Fixed- 6.7  kbit/s, AMR            */
  SND_AMR_FORMAT_FIXED_7_4_AMR,   /* Fixed- 7.4  kbit/s, AMR            */
  SND_AMR_FORMAT_FIXED_7_95_AMR,  /* Fixed- 7.95 kbit/s, AMR            */
  SND_AMR_FORMAT_FIXED_10_2_AMR,  /* Fixed-10.2  kbit/s, AMR            */
  SND_AMR_FORMAT_FIXED_12_2_AMR,  /* Fixed-12.2  kbit/s, AMR            */

  SND_AMR_FORMAT_FIXED_4_75_AMR_DTX, /* Fixed- 4.75 kbit/s, AMR Codec, AMR */
  SND_AMR_FORMAT_FIXED_5_15_AMR_DTX, /* Fixed- 5.15 kbit/s, AMR Codec, AMR */
  SND_AMR_FORMAT_FIXED_5_9_AMR_DTX,  /* Fixed- 5.9  kbit/s, AMR Codec, AMR */
  SND_AMR_FORMAT_FIXED_6_7_AMR_DTX,  /* Fixed- 6.7  kbit/s, AMR Codec, AMR */
  SND_AMR_FORMAT_FIXED_7_4_AMR_DTX,  /* Fixed- 7.4  kbit/s, AMR Codec, AMR */
  SND_AMR_FORMAT_FIXED_7_95_AMR_DTX, /* Fixed- 7.95 kbit/s, AMR Codec, AMR */
  SND_AMR_FORMAT_FIXED_10_2_AMR_DTX, /* Fixed-10.2  kbit/s, AMR Codec, AMR */
  SND_AMR_FORMAT_FIXED_12_2_AMR_DTX, /* Fixed-12.2  kbit/s, AMR Codec, AMR */
#endif /* FEATURE_AUDFMT_AMR */

  SND_QCP_FORMAT_FIXED_FULL_EFR,  /* Fixed full rate EFR                */
  SND_QCP_FORMAT_FIXED_FULL_FR,   /* Fixed full rate FR                 */
  SND_QCP_FORMAT_FIXED_FULL_HR,   /* Fixed full rate HR                 */

#ifdef FEATURE_AUDFMT_EVB
  SND_EVB_FORMAT_VAR_FULL_4GV_NB, /* .EVB format, variable rate, full rate max, 4GV-NB */
#endif /* FEATURE_AUDFMT_EVB */
#ifdef FEATURE_AUDFMT_EVW
  SND_EVW_FORMAT_VAR_FULL_4GV_WB, /* .EVW format, variable rate, full rate max, 4GV-WB */
#endif /* FEATURE_AUDFMT_EVW */

  SND_QCP_FORMAT_INVALID
} snd_qcp_format_type;

/* This enumerated type is used to indicate the path for QCP operations.
*/
typedef enum {
  SND_QCP_DIR_REVERSE,  /* QCP on reverse link (Tx path)         */
  SND_QCP_DIR_FORWARD,  /* QCP on forward link (Rx path)         */
  SND_QCP_DIR_BOTH,     /* QCP on both reverse and forward links */
  SND_QCP_DIR_INVALID
} snd_qcp_dir_type;

/* This callback function type is used as the callback type when
** requesting a buffer for QCP recording. This type is used as the data 
** parameter for sound to client callbacks with status SND_CMX_QCP_REC_GET_BUF.
** The parameters are a pointer to the buffer and the size of the buffer
** in bytes.
*/
typedef void (* snd_qcp_buf_cb_func_type) ( uint8 *, uint32 );

/* This enum type is used to describe the type of QCP data, in the buffer,
** that is to be recorded.
*/
typedef enum {
  SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED = 0, /* uncategorized data type     */
  SND_QCP_REC_DATA_BUFFER_RECDATA,           /* recorded sound data         */
  SND_QCP_REC_DATA_BUFFER_MAX
} snd_qcp_rec_data_buffer_enum_type;

/* This structure is used as the data parameter for SND_CMX_QCP_REC_DATA
** callbacks. This is used to return QCP recorded data to the client.
*/
typedef struct {
  snd_qcp_dir_type link;      /* Path QCP recording data is from            */
  snd_qcp_rec_data_buffer_enum_type
                   content;   /* Type of data being recorded                */
  uint8            *buffer;   /* Pointer to buffer of data                  */
  uint32           num_bytes; /* Number of bytes of data                    */
  uint32           position;  /* Position that the data belongs in QCP file */
} snd_qcp_rec_data_struct_type;

/* The following is the callback type used for sound server to client
** status updates for QCP recording. The type of data is determined by the
** status of the callback.
*/
typedef void (* snd_qcp_rec_cb_func_ptr_type) (
  snd_af_data_cmd_enum  status,        /* Command/Status of callback       */
  void                  *data,         /* Data from sound server to client */
  const void            *client_data   /* Client data pointer              */
);
#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM_REC
/* This enum type is used to indicate the direction of PCM recording
*/
typedef enum {
  SND_PCM_REC_DIR_REVERSE,  /* Record the reverse link */
  SND_PCM_REC_DIR_FORWARD,  /* Record the forward link */
  SND_PCM_REC_DIR_INVALID
} snd_pcm_rec_dir_type;

/* This structure contains the required initial two ping-pong buffers for
** PCM recording.
*/
typedef struct {
  uint8    *buffer1;    /* Pointer to first recording buffer   */
  uint32   num_buf1;    /* Size of first recording buffer      */
  uint8    *buffer2;    /* Pointer to second recording buffer  */
  uint32   num_buf2;    /* Size of second recording buffer     */
} snd_pcm_rec_cmd_struct_type;

/* This function pointer type is used to return PCM recording buffers
** from the client to AVS.
*/
typedef void (* snd_pcm_buf_cb_func_type) (
  uint8         *buffer,
  uint32        num_bytes,
  const void    *client_data
);

/* This structure is used as the data parameter for SND_CMX_PCM_REC_DATA and
** SND_CMX_PCM_REC_HEADER_DATA callbacks. This is used to return PCM recorded
** data to the client.
*/
typedef struct {
  uint8                     *rec_data_buf;  /* Buffer of recorded PCM data    */
  uint32                    rec_data_num;   /* Size of buffer                 */
  uint32                    rec_data_pos;   /* Position of recorded data      */
  snd_pcm_buf_cb_func_type  buf_cb_func;    /* CB func used to return buffer  */
  const void                *client_data;   /* Client data ptr for buf return */
} snd_pcm_rec_data_struct_type;

/* The following is the callback type used for sound server to client
** status updates for PCM recording. The type of data is determined by the
** status of the callback.
*/
typedef void (* snd_pcm_rec_cb_func_ptr_type) (
  snd_af_data_cmd_enum  status,        /* Command/Status of callback       */
  void                  *data,         /* Data from sound server to client */
  const void            *client_data   /* Client data pointer              */
);
#endif /* FEATURE_PCM_REC */

#if defined(FEATURE_QCP) || defined(FEATURE_MM_REC)
/* 
 * This enumerated type is used to indicate the mode for MultiMedia recording.
 */
typedef enum {
  SND_MM_FORMAT_AAC,                        /* AAC data file format           */
  SND_MM_FORMAT_QCP,                        /* AMR data file format           */
  SND_MM_FORMAT_INVALID                     /* Invalid file format            */
} snd_mm_rec_format_type;


#ifdef FEATURE_MM_REC
/*
 * Data structure for A/V sync report information.
 */
typedef struct {
  uint32 num_samples;         /* AV sync report, total samples just encoded   */
  uint32 time_stamp[2];       /* Time stamp at this moment                    */
  uint32 lost_samples;        /* Lost samples if any                          */
  uint32 frame_index;         /* Frame count/index for current frame          */
} snd_av_report_type;

/* 
 * The following is the callback type used for sound server to client
 * A/V sync for MM recording.
 */
typedef void (* snd_mm_rec_av_sync_cb_ptr) (
  snd_av_report_type *av_report,
  const void         *client_data
);                       

/*
 * MultiMedia record buffer information
 */
typedef struct {
  uint8                        *buf1;       /* Buffer 1 for recorded data     */
  uint32                       num_buf1;    /* Size of buf1, in bytes         */
  uint8                        *buf2;       /* Buffer 2 for recorded data     */
  uint32                       num_buf2;    /* Size of buf2, in bytes         */
} snd_mm_rec_buf_info;

#ifdef FEATURE_AAC_REC
/*
 * Structure specific for AAC recording parameters
 */
typedef struct {
  snd_mm_rec_format_type      format;         /* MM record/encode type        */
  snd_aac_data_format_enum_type
                               aac_file_format;/* MM recording file format    */
  snd_sample_rate_type         sample_rate;  /* MM recording input sample rate*/
  snd_mm_rec_av_sync_cb_ptr    sync_cb_func; /* A/V sync callback function    */
  snd_aac_channel_enum_type    chan;         /* Mono or Stereo                */
  uint16                       bit_per_spl;  /* Bit per sample/channel        */
  uint32                       sync_ms;      /* Time(ms) to do A/V sync report*/
} snd_mm_rec_aac_para_type;
#endif /* FEATURE_AAC_REC */

/* 
 * This structure contains the required initial two ping-pong buffers for
 * MultiMedia recording.
 */
typedef struct {
  uint8    *buffer1;                   /* Pointer to first recording buffer   */
  uint32   num_buf1;                   /* Size of first recording buffer      */
  uint8    *buffer2;                   /* Pointer to second recording buffer  */
  uint32   num_buf2;                   /* Size of second recording buffer     */
} snd_mm_rec_cmd_struct_type;

/* 
 * This function pointer type is used to return MulitMedia recording buffers
 * from the client to AVS.
 */
typedef void (* snd_mm_buf_cb_func_type) (
  uint8                  *buffer,      /* buffer pointer of the data          */
  uint32                 num_bytes,    /* data length                         */
  const void             *client_data  /* client data returned                */
);

/* 
 * This structure is used as the data parameter for SND_CMX_MM_REC_DATA and
 * SND_CMX_MM_REC_HEADER_DATA callbacks. This is used to return MultiMedia 
 * recorded data to the client.
 */
typedef struct {
  uint8                     *rec_data_buf;/* Buffer of recorded MM data       */
  uint32                    rec_data_num; /* Size of buffer                   */
  uint32                    rec_data_pos; /* Position of recorded data        */
  snd_mm_buf_cb_func_type   buf_cb_func;  /* CB func used to return buffer    */
  const void                *client_data; /* Client data ptr for buf return   */
} snd_mm_rec_data_struct_type;

/* 
 * The following is the callback type used for sound server to client
 * status updates for MultiMedia recording. The type of data is determined 
 * by the status of the callback.
 */
typedef void (* snd_mm_rec_cb_func_ptr_type) (
  snd_af_data_cmd_enum  status,        /* Command/Status of callback          */
  void                  *data,         /* Data from sound server to client    */
  const void            *client_data   /* Client data pointer                 */
);

/*
 * Data structure for recording control block at sound layer.
 */
typedef struct {
  snd_mm_rec_format_type      format;         /* MM record/encode file format */
  snd_mm_rec_cb_func_ptr_type cb_func;        /* Callback function to CMX     */
  const void                  *client_data;   /* Client data pointer          */
} snd_mm_rec_control_type;
#endif /* FEATURE_MM_REC */


/* This enum type is used to indicate the direction of PCM recording
*/
typedef enum {
  SND_MM_REC_DIR_REVERSE,                   /* Record the reverse link        */
  SND_MM_REC_DIR_FORWARD,                   /* Record the forward link        */
  SND_MM_REC_DIR_INVALID
} snd_mm_rec_dir_type;

/*
 * Data structure for QCP record parameters
 */
typedef struct {
  snd_mm_rec_format_type    mm_format;    /* MM record/encode type          */
  snd_qcp_format_type       foramt;       /* QCP recorded type              */
  snd_mm_rec_dir_type       rec_path;     /* QCP recorded direction         */
  uint32                    report_int;   /* Time callback interval in ms   */ 
  uint32                    auto_stop_ms; /* Silence time to end recording  */
#ifdef FEATURE_MM_REC
  snd_mm_rec_av_sync_cb_ptr sync_cb_func; /* A/V sync callback function     */
#endif /* FEATURE_MM_REC */
  uint32                    data_req_ms;/* Data request time interval in ms */
} snd_mm_qcp_rec_para_type;

/*
 * Union definition for different MultiMedia recording parameters
 */
typedef union {
  snd_mm_rec_format_type    format;         /* MM record/encode type          */
#ifdef FEATURE_AAC_REC
  snd_mm_rec_aac_para_type  aac_rec_param;  /* AAC recording parameters       */
#endif /* FEATURE_AAC_REC */
#ifdef FEATURE_QCP
  snd_mm_qcp_rec_para_type  qcp_rec_param;  /* QCP recording parameters       */
#endif /* FEATURE_QCP */
} snd_mm_rec_aud_type;
#endif /* FEATURE_QCP || FEATURE_MM_REC */

#endif /* FEATURE_AUDIO_FORMAT */

/* Type specifications for header struct of types of sounds.
** This is a base type for sounds and should never be instantiated.
*/
typedef union {
  snd_type                      type;
  snd_compact_sound_type        compact_sound;
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif
  snd_flexible_sound_type       flexible_sound;
#ifdef FEATURE_VOICE_PLAYBACK
  snd_tone_and_voice_sound_type tone_and_voice;
#endif
#ifdef FEATURE_AUDIO_FORMAT
  snd_audfmt_sound_type         audfmt_sound;
  snd_format_sound_type         format_sound;
#endif
} snd_sound_type;

#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */

/* Type specification to control EC/NS operation
*/
typedef enum {
  SND_EC_NS_FORCE_DISABLED,     /* Force EC/NS to be disabled  */
  SND_EC_NS_NORMAL_OPERATION,    /* Have EC/NS operate as usual */

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_EC_NS_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_ec_ns_mode_type;

/* Type specification to control VOX operation
*/
typedef enum {
  SND_VOX_FORCE_ENABLED,      /* Force VOX to be enabled  */
  SND_VOX_NORMAL_OPERATION,   /* Have VOX operate as usual */

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_VOX_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_vox_mode_type;

/* Type specification to control sidetone operation
*/
typedef enum {
  SND_SIDETONE_FORCE_DISABLED,      /* Force sidetone to be disabled  */
  SND_SIDETONE_NORMAL_OPERATION,   /* Have sidetone operate as usual */

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_SIDETONE_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_sidetone_mode_type;


/* Type specification to control smart sound ( AGC/RVE/AVC ) options
 */

typedef enum {
   SND_SMARTSND_NONE,									/* enable none of RVE/AGC/AVC */
   SND_SMARTSND_AGC_ENABLED_AIG_ENABLED,       						/* enable AGC with AIG */
   SND_SMARTSND_AGC_ENABLED_AIG_DISABLED,							/* enable AGC without AGC */
   SND_SMARTSND_AVC_AGC_AIG_ENABLED,                    /* enable AVC, AGC with AIG */
   SND_SMARTSND_AVC_AGC_ENABLED_AIG_DISABLED,                   	/* enable AVC, AGC without AIG */
   SND_SMARTSND_AVC_ENABLED_AGC_AIG_DISABLED,                       /* enable AVC only */
   SND_SMARTSND_RVE_AGC_AIG_ENABLED,                    /* enable RVE, AGC with AIG */
   SND_SMARTSND_RVE_AGC_ENABLED_AIG_DISABLED,                   	/* enable RVE, AGC without AIG */
   SND_SMARTSND_RVE_ENABLED_AGC_AIG_DISABLED,                           				/* enable RVE only */
   SND_SMARTSND_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_smartsnd_mode_type;




/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* The sound call back function argument type. See below call back prototype.
*/
typedef enum {
  
  SND_SUCCESS,             /* Command was accepted                          */
  SND_REPEAT,              /* Current sound has reached a repeat            */
  SND_PLAY_DONE,           /* Play request done                             */
  SND_PROMPT_DONE,         /* Playback is done                              */
  SND_PROMPT_BUSY,         /* No queues available for voice prompting       */
  SND_VP_DIFF_SO,          /* Playback of VP to incompatible service option */
  SND_BAD_VP,              /* Something wrong with VP header/buffer         */
  SND_DEVICE_CONNECTED,    /* Device connection                             */
#ifdef FEATURE_AUDIO_FORMAT
  SND_AF_TIME_TICK,        /* Tick update (usually occurs every second)     */
#endif
  SND_ABORT,                /* generic abort without further explaination    */
  SND_ENABLED,             /* Enabled status                                */
  SND_DISABLED,            /* Disabled status                               */
  SND_FAILURE,             /* Request is rejected because it is erroneous   */
  SND_ABORT_DATA_ACCESS_ERR, /* Abort for data access error                  */
  SND_ABORT_DATA_ERR,        /* Abort for invalid data being found           */
  SND_ABORT_RESOURCE_ERR,    /* Abort because of resource error              */
  SND_PROMPT_NONE,         /* There are no sound prompts playing            */
  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_STATUS_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_status_type;

/* The sound call back function prototype for most functions except
** for snd_get_volume and snd_get_levels
*/
typedef void (* snd_cb_func_ptr_type) 
             ( const void      *client_data, /* pointer to Client data   */
               snd_status_type status );     /* Status returned by Sound */
 
/* The sound call back function prototype for snd_get_volume
*/
typedef void (* snd_vol_cb_func_ptr_type)
             ( const void      *client_data, /* pointer to Client data   */
               snd_status_type status,       /* Status returned by Sound */
               uint16          volume );     /* Volume Level             */
 
/* The sound call back function prototype for snd_get_levels
*/
typedef void (* snd_lev_cb_func_ptr_type)
             ( const void      *client_data, /* pointer to Client data   */
               snd_status_type status,       /* Status returned by Sound */
               uint16          levels );     /* Maximum Volume Level     */

/* The sound registered function that is used by the auxiliary generator
*/
typedef void (* snd_aux_func_ptr_type) (boolean on);

#if defined(FEATURE_MIDI_OUT) || defined(FEATURE_WEBAUDIO) || \
    defined(FEATURE_QSYNTH_ADPCM)
/* Type specifications for QSynth audio decoder
*/ 

boolean snd_use_resampler_wave(void);

typedef enum {
  SND_QSYNTH_ADEC_MODE_DEFAULT,        /* Default type of QSynth ADEC */
  SND_QSYNTH_ADEC_MODE_ADPCM,          /* QSynth ADPCM audio decoder  */
  SND_QSYNTH_ADEC_MODE_QCP,            /* QSynth QCELP audio decoder  */
  SND_QSYNTH_ADEC_MODE_PCM             /* QSynth PCM audio decoder    */
} snd_qsynth_adec_mode_enum_type;
#endif /* FEATURE_MIDI_OUT || FEATURE_WEBAUDIO || FEATURE_QSYNTH_ADPCM */

#if defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_WAVE_PB) || \
    defined(FEATURE_QSYNTH_ADPCM)
/* This type is used to indicate the start mode for ADEC playback.
*/
typedef enum {
  SND_ADEC_START_MODE_NORMAL,  /* Normal start - start playback immediately  */
  SND_ADEC_START_MODE_DELAYED, /* Delayed start - load file and wait for cmd */
  SND_ADEC_START_MODE_INVALID
} snd_adec_start_mode_enum_type;
#endif /* FEATURE_MIDI_OUT_QCP || FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_MIDI_OUT_QCP
/* This type is used to indicate the repeat mode for ADEC playback.
*/
typedef enum {
  SND_ADEC_REPEAT_MODE_DISABLED, /* Repeat mode disabled - play file once   */
  SND_ADEC_REPEAT_MODE_ENABLED,  /* Repeat mode enabled - repeat file the   */
                                 /* indicated number of times               */
  SND_ADEC_REPEAT_MODE_INVALID
} snd_adec_repeat_mode_enum_type;

/* This structure is used to indicate the playback parameters for ADEC
*/
typedef struct {
  uint16                         volume;      /* Volume of adec playback      */
  sint15                         pan;         /* Panning of adec playback     */
  snd_adec_start_mode_enum_type  start_mode;  /* Start mode for playback      */
  snd_adec_repeat_mode_enum_type repeat_mode; /* Repeat mode for playback     */
  uint32                         repeat_num;  /* Number of iterations for     */
                                              /* repeat: 0 = rpt indefinitely */
} snd_midi_out_adec_param_type;
#endif /* FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_QAUDIOFX
/* This enum is used to enable/disable 3D positional mode */
typedef enum {
  SND_3D_POS_MODE_DISABLE,   /* Disable 3D positional mode */
  SND_3D_POS_MODE_ENABLE,    /* Enable 3D positional mode  */
  SND_3D_POS_MODE_INVALID
} snd_3d_pos_mode_enum_type;

/* This structure is used to issue a listener position or a source position
** command.
*/
typedef struct {
  sint31 x_pos;         /* X position of listener                           */
  sint31 y_pos;         /* Y position of listener                           */
  sint31 z_pos;         /* Z position of listener                           */
  uint32 duration_ms;   /* Duration in ms to move to new location           */
} snd_3d_pos_position_cmd_type;

/* The following structure is used to issue a listener orientation command */
typedef struct {
  sint31 x_forward;     /* X coordinate of forward vector                   */
  sint31 y_forward;     /* Y coordinate of forward vector                   */
  sint31 z_forward;     /* Z coordinate of forward vector                   */
  sint31 x_up;          /* X coordinate of up vector                        */
  sint31 y_up;          /* Y coordinate of up vector                        */
  sint31 z_up;          /* Z coordinate of up vector                        */
  uint32 duration_ms;   /* Duration in ms to move to new location           */
} snd_3d_pos_lsnr_orient_cmd_type;

/* This structure is used to issue a listener velocity or a source velocity
** command.
*/
typedef struct {
  sint31 x_vel;         /* X velocity of listener                           */
  sint31 y_vel;         /* Y velocity of listener                           */
  sint31 z_vel;         /* Z velocity of listener                           */
  uint32 duration_ms;   /* Duration in ms to change to new velocity         */
} snd_3d_pos_velocity_cmd_type;

typedef enum{
  SND_3D_POS_REVERB_SET_PRESET,        /* Set Decay time in ms */
  SND_3D_POS_REVERB_SET_LEVEL,         /* Set Decay time in ms */
  SND_3D_POS_REVERB_SET_TIME,          /* Set Decay time in ms */
  SND_3D_POS_REVERB_SET_RATIO         /* Set Decay ratio */
} snd_3d_reverb_enum_type;

/* This structure is used to issue a Reverb command. */
typedef struct {
  uint16 env_type;          /* Environment type                             */
  sint31 room_gain;         /* Room Gain                                    */
  uint32 decay_ms;          /* Decay Time in ms for low frequencies         */
  uint32 decay_ratio;       /* Decay time ratio btwn high and low frequencies */
  uint16 ref_delay;         /* Initial reflection delay time in ms          */
  sint31 reverb_gain;       /* Reverb gain relative to room effect          */
  uint16 reverb_delay;      /* Reverb delay relative to initial reflection in ms */
} snd_3d_reverb_cmd_type;

/* The following structure is used to issue a source volume command */
typedef struct {
  sint31  volume;       /* Volume of source, q15 value (default == 0x7fff) */
  uint32  duration_ms;  /* Duration in ms to move to new volume            */
} snd_3d_pos_src_volume_cmd_type;

/* The following structure is used to issue a source volume attenuation
** command.
*/
typedef struct {
  uint32  min_distance; /* Distance at which sounds are played at max volume */
  uint32  max_distance; /* Distance beyond which sounds are muted            */
  uint32  scale;        /* Scale (0...1000), 1000: 1.0 (normal rolloff)      */
                        /*                      0: 0.0 (no rolloff)          */
  boolean mute;         /* Mute after max: TRUE-Mute, FALSE-Hold after Max   */
} snd_3d_pos_src_vol_atten_cmd_type;

/* The following structure is used to issue a source reverb volume command */
typedef struct {
  sint31  volume;       /* Volume of source, q15 value (default == 0x7fff) */
} snd_3d_reverb_src_volume_cmd_type;

/* The following enum is used in querying 3D positional parameters */
typedef enum {
  SND_3D_POS_PARAM_ENV_ENABLED,         /* 3D enviornment enabled flag     */ 
  SND_3D_POS_PARAM_LSNR_POS,           /* Listener position               */
  SND_3D_POS_PARAM_LSNR_ORIENT,        /* Listener orientation            */
  SND_3D_POS_PARAM_LSNR_VEL,           /* Listener velocity vector        */
  SND_3D_POS_PARAM_REVERB_ENABLED,      /* Reverb enabled or disabled      */
  SND_3D_POS_PARAM_REVERB,             /* Reverb settings                 */
  SND_3D_POS_PARAM_SRC_FINAL_SETTINGS, /* Final source parameter settings */
  SND_3D_POS_PARAM_SRC_ENABLED,        /* source 3D effect enabled flag   */
  SND_3D_POS_PARAM_SRC_POS,            /* Source position                 */
  SND_3D_POS_PARAM_SRC_VOLUME,         /* Source volume                   */
  SND_3D_POS_PARAM_SRC_VOL_ATTEN,      /* Source volume attenuation       */
  SND_3D_POS_PARAM_SRC_VEL,            /* Source velocity                 */
  SND_3D_POS_PARAM_SRC_REV_VOL,        /* Source Reverb Volume            */
  SND_3D_POS_PARAM_INVALID
} snd_3d_pos_param_enum_type;

/* This type is used to return source and listener position */
typedef struct {
  sint31 x_pos;   /* X position */
  sint31 y_pos;   /* Y position */
  sint31 z_pos;   /* Z position */
} snd_3d_pos_position_setting_type;

/* This type is used to return listener orientation */
typedef struct {
  sint31 x_forward;     /* X coordinate of forward vector                   */
  sint31 y_forward;     /* Y coordinate of forward vector                   */
  sint31 z_forward;     /* Z coordinate of forward vector                   */
  sint31 x_up;          /* X coordinate of up vector                        */
  sint31 y_up;          /* Y coordinate of up vector                        */
  sint31 z_up;          /* Z coordinate of up vector                        */
} snd_3d_pos_lsnr_orient_setting_type;

/* This structure is used to return a Listener Velocity command. */
typedef struct {
  sint31 x_vel;         /* X velocity of listener                           */
  sint31 y_vel;         /* Y velocity of listener                           */
  sint31 z_vel;         /* Z velocity of listener                           */
  uint32 duration_ms;   /* Duration in ms to change to new velocity         */
} snd_3d_pos_lsnr_vel_setting_type;

/* This structure is used to return a Reverb command. */
typedef struct {
  uint16 env_type;          /* Environment type                             */
  sint31 room_gain;         /* Room Gain                                    */
  uint32 decay_ms;          /* Decay Time in ms for low frequencies         */
  uint32 decay_ratio;       /* Decay time ratio btwn high and low frequencies */
  uint16 ref_delay;         /* Initial reflection delay time in ms          */
  sint31 reverb_gain;       /* Reverb gain relative to room effect          */
  uint16 reverb_delay;      /* Reverb delay relative to initial reflection in ms */
} snd_3d_reverb_setting_type;

/* This type is used to return source volume attenuation */
typedef struct {
  uint32  min_distance; /* Distance at which sounds are played at max volume */
  uint32  max_distance; /* Distance beyond which sounds are muted            */
  uint32  scale;        /* Scale (0...1000), 1000: 1.0 (normal rolloff)      */
                        /*                      0: 0.0 (no rolloff)          */
  boolean mute;         /* Mute after max: TRUE-Mute, FALSE-Hold after Max   */
} snd_3d_pos_src_vol_atten_setting_type;

/* This type is used to return source volume */
typedef struct {
  sint31  volume;       /* Volume of source, q15 value (default == 0x7fff) */
} snd_3d_pos_src_volume_setting_type;

/* This type is used to return source velocity */
typedef struct {
  sint31 x_vel;         /* X velocity of source                             */
  sint31 y_vel;         /* Y velocity of source                             */
  sint31 z_vel;         /* Z velocity of source                             */
} snd_3d_pos_src_vel_setting_type;

/* This type is used to return source volume */
typedef struct {
  sint31  volume;       /* Volume of source, q15 value (default == 0x7fff) */
} snd_3d_reverb_src_volume_setting_type;

/* This structure is used to return all 3D positional source parameter
** settings.
*/
typedef struct {
  boolean                               src_enable; /* Source 3D enable flag */
  snd_3d_pos_position_setting_type      position;  /* Source position        */
  snd_3d_pos_src_vol_atten_setting_type vol_atten; /* Source vol attenuation */
  snd_3d_pos_src_volume_setting_type    volume;    /* Source volume          */
  snd_3d_pos_src_vel_setting_type       vel_vector; /* Source velocity vector*/
  snd_3d_reverb_src_volume_setting_type rev_vol;   /* Source reverb volume   */
} snd_3d_pos_src_all_setting_type;

/* The following type is a union of all the structures used to return
** information for a 3D query command.
*/
typedef union {
  boolean                               env_enabled_flag; /* env enable flag  */
  snd_3d_pos_position_setting_type      position;     /* Current position     */
  snd_3d_pos_lsnr_orient_setting_type   lsnr_orient;  /* Listener orientation */
  snd_3d_pos_lsnr_vel_setting_type      lsnr_vel;     /* Listener velocity    */
  boolean                               reverb_flag;  /* Reverb enable flag   */
  snd_3d_reverb_setting_type            reverb;       /* Reverb               */
  boolean                               src_enable_flag;/* src 3d enable flag */
  snd_3d_pos_src_vol_atten_setting_type src_vol_atten;/* Source vol atten.    */
  snd_3d_pos_src_volume_setting_type    src_volume;   /* Source vol setting   */
  snd_3d_pos_src_vel_setting_type       src_vel;   /* Source velocity setting */
  snd_3d_reverb_src_volume_setting_type rev_vol;    /* Source Reverb vol   */
  snd_3d_pos_src_all_setting_type       src_all;      /* All source settings  */
} snd_3d_pos_param_setting_type;

/* The following function pointer type is used as the query callback */
typedef void (*snd_3d_pos_get_cb_ptr_type) (
  snd_status_type               status,       /* Status of callback           */
  snd_3d_pos_param_enum_type    param,        /* Parameter setting returned   */
  snd_3d_pos_param_setting_type *setting,     /* Value of parameter           */
  const void                    *client_data  /* Client data                  */
);
#endif /* FEATURE_QAUDIOFX */

/* The sound callback function prototype for snd_sound_get_specs
*/
typedef void (* snd_spec_cb_func_ptr_type) ( 
  const void           *client_data,
  snd_status_type      status
);

#ifdef FEATURE_AUDFMT_EQUALIZER
/* The type specifications for equalizer's filter boost type
*/
typedef enum {
  SND_EQUALIZER_BASE_BOOST   = 1,     /* Bass Boost/Cut                    */
  SND_EQUALIZER_TREBLE_BOOST = 2,     /* Treble Boost/Cut                  */
  SND_EQUALIZER_BAND_BOOST   = 3      /* Band Boost/Cut                    */
} snd_equalizer_filter_boost_enum_type;

/* This structure is used as the data parameter for equalizer 
** This is used to setup the equalizer table in the vocoder driver.
** After this user can change the gain base on the available filter band.
*/
typedef struct {
  int16                         gain; /* Filter band initial gain          */
  uint16                        freq; /* Filter band cut-off/center freq   */
  snd_equalizer_filter_boost_enum_type type;
                                      /* Filter band type Band/Treble/Base */
  uint16                        qf;   /* Filter band quality factor        */
} snd_equalizer_filter_type;

/* The maximum gain's value in (dB)
*/
#define SND_EQUALIZER_MAX_GAIN (12)

/* The minmimum gain's value in (dB)
*/
#define SND_EQUALIZER_MIN_GAIN (-12)

#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/* Smallest/largest allowable period between spectrum analyzer updates
*/
#define SND_SPECTRUM_ANALYZER_MIN_PERIOD 5   /* 5 * 10ms   = 50ms   */
#define SND_SPECTRUM_ANALYZER_MAX_PERIOD 200 /* 200 * 10ms = 2000ms */

/* Number of spectral energy coefficients that will be provide
*/
#define SND_SPECTRUM_ANALYZER_NUM_COEFFS 32 /* 16 bit words */

/* Callback function to indicate that new spectrum analyzer data is available
*/
typedef void (* snd_spectrum_analyzer_cb_type)(
  const uint16 *data, 
  void *client_data
);
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

#ifdef FEATURE_AUDFMT_QCONCERT
/* The enum type of supported audio 3D expander output mode
*/
typedef enum {
  SND_QCONCERT_HEADPHONE,        /* Head phone                   */
  SND_QCONCERT_SPEAKER_FRONT,    /* Speaker, front firing        */
  SND_QCONCERT_SPEAKER_SIDE,     /* Speaker, side firing         */  
  SND_QCONCERT_SPEAKER_DESKTOP,  /* Speaker, desktop             */
} snd_qconcert_mode_enum_type;

/* This structure contains the configuration parameters of the Audio 3D 
** expander (QConcert)
*/
typedef struct {
  snd_qconcert_mode_enum_type mode;     /* Output mode           */
  uint16                      gain;     /* Gain value            */
  uint16                      spread;   /* Spread value          */
  int16                       geometry; /* Geometry value        */
} snd_qconcert_params_type;
#endif /* FEATURE_AUDFMT_QCONCERT */

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)

typedef struct {
  /* This is used by internal PP modules and should not be changed by clients */
  uint16 pp_id; 
  /* Length of the PP buffer */
  uint16 pp_buf_length;
  /* Buffer containing the PP data */
  uint16* pp_data;
  /* Number of channels */
  uint16  pp_num_channels;
}snd_pp_buffer_type;

typedef void (*snd_pp_cb_type) (const snd_pp_buffer_type pp_buffer);
#endif

#ifdef FEATURE_AVS_I2SSBC

typedef enum {
  SND_I2SSBC_STATUS_DISABLED,/* I2S SBC interface disabled */
  SND_I2SSBC_STATUS_ENABLED, /* I2S SBC interface has been enabled */ 
  SND_I2SSBC_STATUS_PENDING  /* The command is being processed */
}snd_i2ssbc_status_type;

/* Callback to get I2SSBC status */
typedef void (*snd_i2ssbc_cb_func_ptr_type) (snd_i2ssbc_status_type);        

typedef struct {
  snd_sample_rate_type          sample_rate;
  snd_i2ssbc_cb_func_ptr_type   cb_func;
}snd_i2ssbc_config_params_type;

#endif /* FEATURE_AVS_I2SSBC */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
typedef enum {
  SND_SBC_BITRATE_LOWEST,
  SND_SBC_BITRATE_LOW,
  SND_SBC_BITRATE_MEDIUM,
  SND_SBC_BITRATE_HIGH,
  SND_SBC_BITRATE_HIGHEST,
  SND_SBC_BITRATE_CUSTOM
}snd_sbc_bitrate_type;

/* Type of the BT driver function to be called to pass the SBC frames to 
   BT driver */
typedef void (*snd_sbc_frame_cb_func_ptr_type) (	
  uint8*  data,                  /* Pointer a buffer containing one SBC frame */
  uint32  seq_num,               /* Sequence number of the SBC frame */
  uint32  timestamp,             /* Timestamp of the SBC frame */
  boolean last_frame,            /* TRUE if this is the last frame of this run*/
  uint16  sbc_frame_len          /* length of the frame */
);

#endif

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)

typedef enum {
  SND_SBC_SUBBANDS_4,
  SND_SBC_SUBBANDS_8,
  SND_SBC_SUBBANDS_MAX
}snd_sbc_subbands_type;

typedef enum {
  SND_SBC_BLOCKS_4,
  SND_SBC_BLOCKS_8,
  SND_SBC_BLOCKS_12,
  SND_SBC_BLOCKS_16,
  SND_SBC_BLOCKS_MAX
}snd_sbc_blocks_type;

typedef enum {
  SND_SBC_MODE_INVALID,
  SND_SBC_MODE_MONO,
  SND_SBC_MODE_DUAL,
  SND_SBC_MODE_STEREO,
  SND_SBC_MODE_JT_STEREO,
  SND_SBC_MODE_MAX
}snd_sbc_mode_type;

typedef enum {
  SND_SBC_ALLOC_SNR,
  SND_SBC_ALLOC_LOUDNESS,
  SND_SBC_ALLOC_MAX
}snd_sbc_alloc_type;

typedef struct {
  snd_sbc_blocks_type    blocks;
  snd_sbc_mode_type      mode;
  snd_sbc_alloc_type     alloc;
  snd_sbc_subbands_type  bands;
}snd_sbc_config_params_type;
#endif /* FEATURE_SBC_CODEC */

typedef enum {
  SND_AUDFMT_POLY_MODE_0,    /* All HP-mode, 72 HP voices  */
  SND_AUDFMT_POLY_MODE_1,    /* 56 HP voices, 4 HQ voices */
  SND_AUDFMT_POLY_MODE_2,    /* 46 HP voices, 8 HQ voices  */
  SND_AUDFMT_POLY_MODE_3,    /* 38 HP voices, 12 HQ voices */
  SND_AUDFMT_POLY_MODE_4,    /* 30 HP voices, 16 HQ voices */
  SND_AUDFMT_POLY_MODE_5,    /* 20 HP voices, 20 HQ voices */
  SND_AUDFMT_POLY_MODE_6,    /* 12 HP voices, 24 HQ voices */
  SND_AUDFMT_POLY_MODE_7     /* All HQ-mode, 32 HQ voices  */
} snd_audfmt_poly_mode_type ;

typedef enum {
  SND_AUX_LINE_IN_MODE_ON,    /* Aux Line In On Mode*/
  SND_AUX_LINE_IN_MODE_OFF    /* Aux Line In Off Mode*/
} snd_aux_line_in_mode_type ;

#ifdef FEATURE_AUDFMT_HYBRID_MODE
typedef void (* snd_poly_mode_cb_func_ptr_type)
             ( const void *client_data,
               snd_status_type status,
               snd_audfmt_poly_mode_type poly_mode);

#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)

typedef void (*snd_bt_cb_func_ptr_type)(uint32); 

#endif /*AVS_BT_SCO_REWORK*/

/*Callback prototype for QTV*/
typedef void (*snd_notify_qtv_cb_func_ptr_type)(uint32);

#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
typedef void (*snd_oemsbl_cb_func_ptr_type)(void);
#endif

/* This structure contains the configuration parameters for  
** slowtalk 
*/
typedef struct {
  /* expansion ratio range 0 - 5 */
  uint16   expansion_ratio_level;
} snd_slowtalk_params_type;

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
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
extern void snd_register_aux_ctl
(
  snd_aux_func_ptr_type aux_ctl_fn_ptr
    /* Pointer to the function to control an auxiliary generator.*/
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_aux_line_in

DESCRIPTION
  Set the aux line on/off

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.
  
===========================================================================*/
extern void snd_set_aux_line_in (

  snd_cb_func_ptr_type           callback_ptr,
  /* Pointer to cb func for receving status about this command */

  const void                     *client_data,
  /* Pointer to Client provided data */

  snd_aux_line_in_mode_type aux_mode
  /* Aux line In mode */
);

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
extern void snd_set_device
(
  snd_device_type       device,
    /* The device chosen for this command   */
  snd_mute_control_type ear_mute,
    /* Mute, unmute output                  */
  snd_mute_control_type mic_mute,
    /* Mute, unmute microphone              */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_set_device */

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
extern void snd_set_volume
(
  snd_device_type       device,
    /* The device chosen for this command   */
  snd_method_type       method,
    /* method                               */
  uint8                 volume,
    /* Volume                               */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_set_volume */

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
extern void snd_get_volume
(
  snd_device_type          device,
    /* The New device Type the caller wants */
  snd_method_type          method,
    /* method                               */
  snd_vol_cb_func_ptr_type callback_ptr,
    /* Call back function                   */
  const void               *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_get_volume */

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
extern void snd_get_levels
(
  snd_device_type          device,
    /* The device chosen for this command   */
  snd_method_type          method,
    /* method                               */
  snd_lev_cb_func_ptr_type callback_ptr,
    /* Call back function                   */
  const void               *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_get_levels */

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
extern void snd_tone_start
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
);
/*~ FUNCTION snd_tone_start */

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
extern void snd_freq_tone_start
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
);
/*~ FUNCTION snd_freq_tone_start */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sample_tone_start

DESCRIPTION
  Start playing a tone with the specified volume in the command
  using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_sample_tone_start
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
);
/*~ FUNCTION snd_sample_tone_start */

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
extern void snd_tone_stop
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_tone_stop */

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
extern void snd_sound_id_start
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
);
/*~ FUNCTION snd_sound_id_start */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sample_sound_id_start

DESCRIPTION
  Start playing a sound from the database with the specified volume 
  in the command using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_sample_sound_id_start
(
  snd_device_type       device,
   /* The device chosen for this command    */
  snd_method_type       method,
    /* method                               */
  snd_sound_id_type     sound_id,
    /* Sound id                             */
  uint8                 volume,
    /* Volume to play this sound            */
  uint16                repeat_count,
    /* Nbr of times to repeat before stop   */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_sample_sound_id_start */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_start

DESCRIPTION
  Start playing a sound using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_sound_start
(
  snd_device_type       device,
   /* The device chosen for this command    */
  snd_method_type       method,
    /* method                               */
  const snd_sound_type  *sound_ptr,
    /* Sound                                */
  snd_priority_type     priority,
    /* Priority of the tone                 */
  snd_apath_type        dtmf_path,
    /* Play this tone OTA when in DFM call  */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sample_sound_start

DESCRIPTION
  Start playing a sound with the specified volume in the command
  using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_sample_sound_start
(
  snd_device_type       device,
   /* The device chosen for this command    */
  snd_method_type       method,
    /* method                               */
  const snd_sound_type  *sound_ptr,
    /* Pointer to sound list                */
  uint8                 volume,
    /* Volume to play this sound            */
  uint16                repeat_count,
    /* Nbr of times to repeat before stop   */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);

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
extern void snd_sound_stop
(
  snd_priority_type     priority,
    /* Priority of the tone                 */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_sound_stop */

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
extern void snd_vp_start
(
  snd_device_type        device,
    /* The device chosen for this command   */
  voc_pb_header_type     *vp_ptr,
    /* Voice prompt to be played            */ 
  snd_cb_func_ptr_type   callback_ptr,
    /* Call back function                   */
  const void             *client_data
    /* Pointer to Client data               */
);

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
extern void snd_vp_stop
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
#endif /* FEATURE_VOICE_PLAYBACK */

/* <EJECT> */
#ifdef FEATURE_VOICE_MEMO
/*===========================================================================

FUNCTION snd_vp_pause

DESCRIPTION
  Pause the current voice prompt

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_vp_pause
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_vp_resume

DESCRIPTION
  Resume the paused voice prompt

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_vp_resume
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
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
typedef void 
(* snd_time_cb_func_ptr_type) ( const void        *client_data,
                                snd_status_type   status,
                                uint32            time_ms );

extern void snd_sound_get_time 
(
  snd_sound_type             *sound_ptr,
  snd_time_cb_func_ptr_type  callback_ptr,
  const void                 *client_data
);

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
extern void snd_sound_get_specs (
  snd_sound_type            *sound_ptr,
  snd_spec_cb_func_ptr_type callback_ptr,
  const void                *client_data
);

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
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_fforward

DESCRIPTION
  Fast forward sound playback, if possible.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_sound_fforward (
  snd_priority_type     priority,
  uint32                fforward_ms,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_rewind

DESCRIPTION
  Rewind sound playback, if possible.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_sound_rewind (
  snd_priority_type     priority,
  uint32                rewind_ms,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_pause

DESCRIPTION
  Pause/resume sound playback, if possible.

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
);

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
extern void snd_sound_seek_pause (
  snd_priority_type     priority,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
);

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
extern void snd_sound_tempo (
  snd_priority_type     priority,
  uint32                tempo_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
);

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
extern void snd_sound_tune (
  snd_priority_type     priority,
  uint8                 tune_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
);

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
extern void snd_sound_pan (
  snd_priority_type     priority,
  uint8                 pan_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
);

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
extern void snd_sequence_stop (
  void                 *sequence,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
);

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
extern void snd_sequence_pause (
  void                 *sequence,
  boolean              pause_flag,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
);

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
extern void snd_sequence_fforward (
  void                 *sequence,
  uint32               ms_forward,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
);

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
extern void snd_sequence_rewind (
  void                 *sequence,
  uint32               ms_rewind,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
);

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
extern void snd_sequence_set_tempo (
  void                 *sequence,
  uint32               tempo_factor,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
);

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
extern void snd_sequence_set_tune (
  void                 *sequence,
  uint32               tune_factor,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
);

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
extern void snd_sequence_set_pan (
  void                 *sequence,
  uint16               pan_factor,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
);

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
extern void snd_sequence_set_vol (
  void                 *sequence,
  uint16               vol_factor,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
);

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
extern void snd_audfmt_get_time_abort (
  snd_cb_func_ptr_type         callback_ptr,
  const void                   *client_data
);

/* <EJECT> */
#ifdef FEATURE_DLS
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
extern void snd_audfmt_load_dls (
  snd_af_access_type *handle,
  uint32             byte_offset
);

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
extern void snd_audfmt_unload_dls (
  snd_af_access_type *handle
);
#endif /* FEATURE_DLS */

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
extern void snd_qcp_get_func (
  snd_qcp_func_enum_type func,
  snd_af_access_type     *handle
);

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
extern void snd_qcp_set_func (
  snd_qcp_func_enum_type       func,
  uint8                        *buffer,
  uint32                       num_bytes,
  snd_af_access_type           *handle,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);

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
extern void snd_qcp_record (
  snd_qcp_format_type          format,
  snd_qcp_dir_type             link,
  snd_mm_qcp_rec_para_type     *snd_rec_param,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);

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
extern void snd_qcp_record_stop (
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
);

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
extern void snd_qcp_record_pause (
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
);

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
extern void snd_qcp_record_resume (
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
);
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
extern void snd_pcm_record (
  snd_pcm_rec_dir_type         link,
  snd_pcm_rec_cmd_struct_type  *spec,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);

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
extern void snd_pcm_record_stop (
  snd_pcm_rec_dir_type link,
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
);

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
extern void snd_pcm_record_pause (
  snd_pcm_rec_dir_type link,
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
);

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
extern void snd_pcm_record_resume (
  snd_pcm_rec_dir_type link,
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
);
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION snd_mm_record

DESCRIPTION
  Starts a MultiMedia recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_mm_record (
  snd_mm_rec_dir_type          dir,
  snd_mm_rec_aud_type          *rec_param,
  snd_mm_rec_cmd_struct_type   *mm_spec,
  snd_mm_rec_cb_func_ptr_type  cb_func,
  const void                   *client_data
);

/*===========================================================================

FUNCTION snd_mm_record_stop

DESCRIPTION
  Ends a MultiMedia recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_mm_record_stop (
  snd_mm_rec_dir_type    link, 
  snd_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/*===========================================================================

FUNCTION snd_mm_record_pause

DESCRIPTION
  Pauses a MultiMedia recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_mm_record_pause (
  snd_mm_rec_dir_type    link, 
  snd_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/*===========================================================================

FUNCTION snd_mm_record_resume

DESCRIPTION
  Resumes a MultiMedia recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.
===========================================================================*/
extern void snd_mm_record_resume (
  snd_mm_rec_dir_type    link, 
  snd_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/*===========================================================================

FUNCTION snd_mm_isr_cb_func

DESCRIPTION
  MultiMedia recording interrupt service,

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.
===========================================================================*/
extern void snd_mm_isr_cb_func (void);
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
extern void snd_aux_pcm_clk_enable
(
  snd_cb_func_ptr_type     callback_ptr
    /* Call back function                     */
);
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
);

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
  A task switch might occurr

===========================================================================*/
extern void snd_equalizer_ctl
(
  boolean                  enable,
    /* Enable/Disable equalizer               */
  snd_cb_func_ptr_type     callback_ptr,
    /* Call back function                     */
  const void               *client_data
    /* Pointer to Client data                 */
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_equalizer_ctl

DESCRIPTION
  This function will setup the equalizer filter The equalizer filter setting
  will get copied into local structure and then user can update gain value 
  on each filter band through the snd_equalizer_gain_ctl().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
extern void snd_set_equalizer
(
  const snd_equalizer_filter_type *equalizer,
    /* Equalizer structure                    */
  uint32                          bands,
    /* Number of bands                        */
  snd_cb_func_ptr_type            callback_ptr,
    /* Call back function                     */
  const void                      *client_data
    /* Pointer to Client data                 */
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_equalizer_gain_ctl

DESCRIPTION
  This function will setup the gain on the specified filter band.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
extern void snd_equalizer_gain_ctl (
  uint16                band,
    /* The band number                      */
  int16                 gain,
    /* The new gain value                   */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_equalizer_get_bands

DESCRIPTION
  This function will get the available equalizer bands.

DEPENDENCIES
  None

RETURN VALUE
  number of equalizer bands.

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
extern uint32 snd_equalizer_get_bands ( void );
#endif /* FEATURE_AUDFMT_EQUALIZER */

/* <EJECT> */
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

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
extern void snd_spectrum_analyzer_ctl (
  uint16                         update_period,
    /* How often should the data be analyzed (10ms multiples) */

  snd_spectrum_analyzer_cb_type  sp_cb,
    /* Callback to receive the data */

  snd_cb_func_ptr_type           status_cb_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
);
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_QCONCERT
/*===========================================================================

FUNCTION snd_qconcert_ctl

DESCRIPTION
  This function is used to enable/disable QConcert feature to provide a 3D
  audio output effect.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_qconcert_ctl (
  boolean               enable,
    /* Flag to enable/disable qconcert */
  snd_cb_func_ptr_type  cb_func,
    /* Callback to indicate whether this ctl command was accepted */
  const void            *client_data
    /* Pointer to Client provided data */
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_qconcert_params 

DESCRIPTION
  This function is used to configure the 3D audio output effect.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_set_qconcert_params (
  snd_qconcert_params_type *params,
    /* QConcert control paramters */
  snd_cb_func_ptr_type     cb_func,
    /* Callback to indicate whether this ctl command was accepted */
  const void               *client_data
    /* Pointer to Client provided data */
);
#endif /* FEATURE_AUDFMT_QCONCERT */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
#endif /* FEATURE_AUDIO_FORMAT */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/*===========================================================================

FUNCTION snd_sbccodec_set_cb

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
); 

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
void snd_sbc_start_timer(void);
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
  snd_sbc_bitrate_type              bitrate_hint, 
  /* Suggested bitrate */

  uint32                            bitrate,
  /* Actual bitrate if the suggested bitrate is SND_SBC_BITRATE_CUSTOM */

  snd_cb_func_ptr_type              callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                        *client_data
    /* Pointer to Client provided data */

);
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
  snd_sbc_frame_cb_func_ptr_type sbc_cb_ptr,
  /* Callback function to be called by SND to send SBC frames */

  snd_cb_func_ptr_type           callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
);

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
);
#endif /* FEATURE_SBC_CODEC */

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
extern void snd_set_bt_cb_func(snd_bt_cb_func_ptr_type cb_func);

/*===========================================================================

FUNCTION snd_clr_bt_cb_func

DESCRIPTION
  This function is used to unregister the BT callback function which is registered
  to audio drivers for informing the tye of sound.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void snd_clr_bt_cb_func(snd_bt_cb_func_ptr_type cb_func);

#endif/*AVS_BT_SCO_REWORK*/
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
extern void snd_set_qtv_cb_func(snd_notify_qtv_cb_func_ptr_type cb_func);

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)
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
                           qword time_stamp);
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
void snd_sbc_avsync (qword frame_num);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbccodec_set_cb

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
); 

#endif

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

  snd_cb_func_ptr_type           snd_cb_func_ptr,
  /* Pointer to cb func for receving status about this command */

  const void                     *client_data
  /* Pointer to Client provided data */

);
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

);
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
  snd_pp_cb_type                 pp_cb,
    /* Callback to receive the data */

  snd_cb_func_ptr_type           callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
); 
#endif /* (FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE) */ 
#ifdef FEATURE_ARM_POST_PROCESSING
/* <EJECT> */
/*===========================================================================

FUNCTION snd_pp_done

DESCRIPTION
  This is the function called by the client to pass the buffer to the
  DSP after post processing is done.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_pp_done ( 
  snd_pp_buffer_type             pp_buffer,
  /* The same data structure that was passed to the client when this buffer
  was sent from DSP to client. */

  snd_cb_func_ptr_type           callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
);
#endif /* FEATURE_ARM_POST_PROCESSING */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT
/*===========================================================================

FUNCTION snd_midi_out_open

DESCRIPTION
  Enable midi device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_midi_out_open (
  snd_cb_func_ptr_type           callback_ptr,
    /* Call back function                   */
  snd_qsynth_adec_mode_enum_type adec_mode,
    /* QSynth ADec type                     */
  const void                     *client_data
    /* Pointer to Client data               */
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_close

DESCRIPTION
  Disable midi device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_midi_out_close (
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_msg

DESCRIPTION
  Issue commands to MIDI output device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_midi_out_msg (
  uint8  *msg_buffer,
  uint32 length
);

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION snd_midi_out_qcp

DESCRIPTION
  Play QCP file to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_midi_out_qcp (
  uint32                       channel,
  snd_af_access_type           *audfmt_handle,
  snd_midi_out_adec_param_type *param
);

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
extern void snd_midi_out_adec_start_chan ( uint32 channel );

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
extern void snd_midi_out_qcp_stop ( uint32 channel );

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
extern void snd_midi_out_qcp_pause ( void );

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
extern void snd_midi_out_qcp_resume ( void );

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
extern void snd_midi_out_qcp_pause_chan ( uint32 channel );

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
extern void snd_midi_out_qcp_resume_chan ( uint32 channel );

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
extern void snd_midi_out_qcp_pan ( uint32 channel, sint15 pan );

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_volume

DESCRIPTION
  Modifies volume of QCP file to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_midi_out_qcp_volume ( uint32 channel, uint16 volume );

#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
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
);

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
);
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
);

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
);

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
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_set

DESCRIPTION
  This function sets one or all 3D positional Reverb parameters command

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
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_enable

DESCRIPTION
  This function enables/disables 3D positional Reverb parameters command

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
);

/* <EJECT> */
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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);
#endif /* FEATURE_QAUDIOFX */

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
extern void snd_ec_ctl
(
  snd_ec_ns_mode_type   ec_mode,
    /* EC control                           */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_ec_ctl */

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
extern void snd_ns_ctl
(
  snd_ec_ns_mode_type   ns_mode,
    /* NS control                           */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_ns_ctl */

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
extern void snd_vox_ctl
(
  snd_vox_mode_type     vox_mode,
    /* VOX control                           */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
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
extern void snd_sidetone_ctl
(
  snd_sidetone_mode_type     sidetone_mode,
    /* Sidetone control                           */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/* <EJECT> */
#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

FUNCTION snd_avc_ctl

DESCRIPTION
  Controls AVC (automatic volume control). Note: AVC is only available when AGC
  is enabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_avc_ctl
(
  boolean               avc_ctl,
    /* AVC state                            */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_avc_ctl */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_agc_ctl

DESCRIPTION
  Controls AGC (automatic gain control).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_agc_ctl
(
  boolean               agc_ctl,
    /* AGC state                            */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);
/*~ FUNCTION snd_agc_ctl */

#endif /* FEATURE_AUDIO_AGC */

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
extern void snd_stereo_mute_ctl
(
  boolean               mute_stereo,
    /* Mute Stereo                          */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);

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
extern void snd_filters
(
  snd_filters_type      filters,
    /* Action to take for voice memo        */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);

/* <EJECT> */
#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */

/* <EJECT> */
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
  snd_dtmf_tx_mute_override

RETURN VALUE
  None

SIDE EFFECTS
  snd_dtmf_tx_mute_override

===========================================================================*/
extern void snd_dtmf_tx_mute_ctl
(
  snd_mute_type mask,
    /* mask of audio mute request associated with caller */

  boolean mute_dtmf
    /* True = mute DTMF, False = un-mute DTMF. */
);

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
extern void snd_phone_awake ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION snd_task

DESCRIPTION
  This procedure is the entrance procedure for the Sound Task.  It
  contains the main processing loop for the Sound Task which controls
  queue processing and sound generation.

DEPENDENCIES
  snd_array
  snd_path_q

RETURN VALUE
  Does not return

SIDE EFFECTS
  snd_tcb

===========================================================================*/
extern void snd_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
);

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
extern void snd_multi_set_poly_mode(
  snd_audfmt_poly_mode_type mode,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
);

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
extern void snd_multi_get_poly_mode(
  snd_poly_mode_cb_func_ptr_type cb_func,
  const void                     *client_data
);
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

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
extern word snd_audfmt_do_time (void);

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
extern void snd_vbr_check_ffwd (
  uint32 adjusted_ms,
  uint32 elapsed_ms,
  uint32 ms_forward
);

#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*===========================================================================

FUNCTION snd_audfmt_adjust_sample

DESCRIPTION
  This function is used to send packet with 
  add/drop one audio sample information to
  snd task.    

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS
 Command Packet queued to Sound Task.
==========================================================================*/
void snd_audfmt_adjust_sample(
  snd_priority_type              priority,
  boolean                        add_sample,
  snd_cb_func_ptr_type           callback_ptr,
  const void                     *client_data
);
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION snd_audfmt_dual_mono_mode

DESCRIPTION
  This function sets Dual-Mono channel mode of decoder.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
void snd_audfmt_dual_mono_mode(
  snd_priority_type                   priority,
  snd_af_dual_mono_mode_enum_type     dual_mono_mode,
  snd_cb_func_ptr_type                callback_ptr,
  const void                          *client_data
);

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
  snd_af_sbr_mode_enum_type sbr_mode,
  snd_cb_func_ptr_type           callback_ptr,
  const void                     *client_data
);
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

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
extern void snd_pa_on_cb(void);

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
extern void snd_set_oemsbl_cb(snd_oemsbl_cb_func_ptr_type oemsbl_func_ptr);

#endif

/* <EJECT> */
/*===========================================================================
 FUNCTION snd_smartsnd_ctl

 DESCRIPTION Controls AGC/AVC/RVE

 DEPENDENCIES
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/

extern void snd_smartsnd_ctl
(
  snd_smartsnd_mode_type 		smartsnd, 		   /* smartsound control */
  snd_cb_func_ptr_type     		callback_ptr,      /* Call back function */
  const void               		*client_data       /* Pointer to Client Data */
);

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
boolean snd_is_task_up ( void );

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
);

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
  snd_cb_func_ptr_type     cb_func,
    /* Callback to indicate whether this ctl command was accepted */
  const void               *client_data
    /* Pointer to Client provided data */
);


#endif /* SND_H */


#ifndef VOCSTATE_H
#define VOCSTATE_H
/*===========================================================================

                   Q D S P 2   V O C O D E R   D R I V E R
           S T A T E   C O N T R O L L E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions for functions and data used by the
  QDSP2 Vocoder Driver State Machine.

REFERENCES
  None.

Copyright (c) 1999 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocstate.h#11 $ $DateTime: 2009/10/15 05:43:22 $ $Author: kmodak $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/09   knm     Integrated new marimba codec driver for SCMM
10/06/09   sj      Added SCMM specific changes
09/07/09   vsud    Changes for resampler image imeplementation.
07/19/09   rk      Notifying SND that PAs are turned on, wakes up sound task.
05/08/09    vm     Added Support for Device switch from external to internal.
04/06/09    av     Included state variable afe_configured 
                   This is used to avoid Vocoder statemachine going to SLEEP when handovers occured.
                   (only for not reuired cases like soft handovers)
                   Copy right updated.
11/24/08   skr     Added code for Audio + camera Concurrency
10/06/08   vsud    added header file for compilation error.
05/09/08   skr     changes to prevent codec config being sent during the mixer 
                   exit sequence     
03/18/08   sj      Removed comilation errors for FEATURE_OEMSBL_AUDIBLE_TONE
03/17/08  ymc/anb  Modified codec gain ctrl api to control gain changes.
                   Fix for SC2X codec pop issue through MSMAUD_PCM_DC_OFFSET feature.
03/16/08    sj     Added Support for OEMSBL
03/11/08    kk     Added support for sending mp3 configuration 
                   information for DSP. 
01/07/08    az     Included the in call midi and the new in call architecture.
10/15/07    az     Included the support for the crystal speech for in call audio.
08/27/07   ymc     Added codec_pending.
07/30/07  anb/wfd  Added aux_pcm_clk to be enabled/disabled from other tasks like BT (CT 122106).
04/13/07   sud     Added support for independent control of Tx and Rx codec
                   clock for Saturn codec.
01/10/07    sg     Added VOC_STATE_QTUNES_DISABLE_PENDING state to indicate
                   if QTunes fw is in the process of being disabled in
                   order to stop commands from being issued.
12/07/06   ymc     Added voc_state_aux_pcm_clk_query().
11/20/06    sp     Added support for OEMSBL Audio.
09/05/06    ay     Added AMR-WB+ support.
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
05/28/06    hz     Added fixed for CR#75189. Before disconnecting BT link,
                   use default codec to enable adie, function prototype 
                   voc_state_bt_adie_wakeup added.
04/02/06    sg     Unfeature-ized FEATURE_QDSP_USES_MODULES and removed support
                   for the old-style QDSP services.
01/31/06    hz     Added support for in-call mp3 feature.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is 
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
07/28/05    sg     Fixed featurization for WAV playback.
07/22/05    sm     Added Wav A/ULaw support.
05/24/05    hz     Added support for MultiMedia/AAC record functionality.
05/16/05    hz     Added support for Vocoder image+ADPCM+VFE.
04/21/05   act     Changed name of voc_codec_type to voc_codec_class_type.
                   Reverted name of voc_codec_id_type to voc_codec_type.
04/11/05    sg     Modified to use voc_data_set_codec_cal() instead of
                   voc_data_set_codec which would cause the codec set by SND to
                   be overwritten.
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable.
03/02/05    sg     Added prototypes for ARM PP functions.
02/14/05    hz     Added support for WMA decode, wma_config was introduced 
                   into voc_state_control_type to hold WMA specific info,
                   New state QTUNES_CONFIG was added.
11/19/04    sp     Added support for Arm Post Processing.
10/14/04    sg     Added voc_aac_config_type to voc_state_data.
08/19/04  sg/sm    Featurized a reduced VOC_STATE_TIME to 5 ms under
                   FEATURE_VOC_FAST_TIMELINE.
                   Added config_standby parameter to voc_state_data.
08/12/04    sg     Added param to store number of higher priority apps using the
                   DSP.
02/13/04    sg     Merged QDSP services changes from
                   MSMSHARED_AVS.00.05.17.01.18.01.00
01/21/04    aw     Added support for qsynth volume mapping function.
12/02/03    sg     Added support for linear PCM playback.
11/22/03    sg     Added support for RealAudio codec.
10/21/03  aw/st    Added support for qsynth hybrid mode.
08/21/03    sg     Modified to support high and low priority vocoder clients
                   (vis-a-vis QDSP Services).
            st     Modified code to support audio mode selection for graphics
                   images.
05/13/03   ymc     Added VOC_TEST_D2E_LOOP.
04/18/03    aw     Added audio support for Q3D image.
04/17/03    sg     Added support for vocoder PCM interfaces.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
02/19/03    st     Added VOC_STATE_QSYNTH_DISABLE_PENDING state to indicate
                   if QSynth state is in the process of being disabled in
                   order to stop commands from being issued; added ADPCM
                   parameters to voc_state_control_type to support playback
                   of ADPCM files.
12/03/02    st     Modified code to add graph audio states; added num_apps
                   parameter to voc_state_control_type.
08/23/02    sg     Modified code to use FEATURE_EXTERNAL_SDAC.
08/22/02    sg     Added support for TTY activity status indication.
                   (Merged from MSM_AVS.01.05.18.01.02).
08/13/02  aw/st    Added audio decoder mode parameters in
                   voc_state_control_type.
           sg/st   Added support for MP4 and AAC.
06/11/02    st     Added voc_state_set_icodec_clk() function prototype.
03/21/02    st     Added voc_state_disable_codec_clk() function prototype;
                   modified voc_state_adie_wakeup() prototype to return boolean.
03/12/02    st     Added config_flag to voc_state_control_type; added
                   voc_state_adie_sleep() and voc_state_adie_wakeup() 
                   prototypes.
12/12/01    aw     Use msmaud.h to eliminate targetization.
08/14/01   ymc     Removed config_flag support.
07/24/01   ymc     Added support for loopback tests.
06/18/01   jkl     Added FEATURE_ACP. Replaced FEATURE_DFM_ANSWER_MACHINE.
05/30/01    sm     Added prototype for voc_state_set_tty() in 5100.
12/04/00  st/sm    Added QTUNES, stereo DAC support.
08/28/00    ro     Uses QDSP Services.
08/24/00   spf     Initial Bluetooth support.
07/13/00   jkl     Merged DFM Answer Machine support. 
06/11/00   spf     Updated references.
05/16/00    st     Added QSynth states; removed vr_mode_pending, vr_mode_mask,
                   tty_mode_pending, mode_change parameters and added 
                   current_image in voc_state_control_type.
05/12/00   spf     Add TTY support: Add tty_mode_pending and tty_mode to
                   voc_state_control_type.
05/02/00    sm     Added voc_state_init_aud_process().
01/25/00   ro/spf  Added support for VR and VM. Added voc_state_set_codec.
12/07/99   spf     Added voc_state_enable_clocks() and voc_state_disable_ 
                   clocks().
10/07/99   spf     Added VOC_STATE_DFM_SAT_DCC. Removed VOC_STATE_RETRY_TIME.
                   Replaced retry_flag field of voc_state_control_type with
                   retry_counter.
09/23/99   spf     Added VOC_STATE_RETRY_TIME.
09/10/99   spf     voc_offline support.
07/15/99   spf     DFM support.
06/15/99    ro     First version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"           /* Customer feature specific              */
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "rex.h"                /* definition of REX data types           */
#include "voc.h"                /* Vocoder interface definition           */
#include "voci.h"               /* Internal Vocoder Task Definitions      */
#include "qdsp.h"               /* QDSP Services                          */
#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
#include "clkrgm_msm.h"
#endif // FEATURE_OEMSBL_AUDIBLE_TONE
#ifndef MSMAUD_SCMM
#ifdef MSMAUD_RX_FILTER_REG_DWA_SET_RESET
#ifndef FEATURE_CMI
#include "adie_sbi.h" /*added to avoid compiler warnings*/
#endif /*FEATURE_CMI */
#endif
#endif /* MSMAUD_SCMM */


/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S
               
===========================================================================*/

/* --------------------------------------------------------------------------
** Vocoder State Machine Definitions
** ------------------------------------------------------------------------*/

/* The states that the vocoder driver may be in
*/
typedef enum {
  VOC_STATE_RESET = 0,
  VOC_STATE_IMAGE,
  VOC_STATE_SLEEP,
  VOC_STATE_INIT_ENTRY,
  VOC_STATE_INIT,
  VOC_STATE_IDLE_ENTRY,
  VOC_STATE_IDLE,
  VOC_STATE_DFM_ENTRY,
  VOC_STATE_DFM_SAT_DCC,
  VOC_STATE_DFM,
  VOC_STATE_VOCODER_ENTRY,
  VOC_STATE_VOCODER,
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */
#ifdef FEATURE_VOC_ADPCM
  VOC_STATE_ADPCM_VOCODER_ENTRY,
  VOC_STATE_ADPCM_VOCODER,
#endif /* FEATURE_VOC_ADPCM */
#ifdef FEATURE_AVS_INCALL_ADEC_PB
  VOC_STATE_ADEC_VOCODER_ENTRY,
  VOC_STATE_ADEC_VOCODER,
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_VOC_G711
  VOC_STATE_WAV_VOCODER_ENTRY,
  VOC_STATE_WAV_VOCODER,
#endif /* FEATURE_VOC_G711 */
  VOC_STATE_VOC_TWO_DECODERS,
#ifdef FEATURE_QSYNTH_COMMON
  VOC_STATE_QSYNTH_INIT,
  VOC_STATE_QSYNTH_ENTRY,
  VOC_STATE_QSYNTH,
  VOC_STATE_QSYNTH_DISABLE_PENDING,
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
  VOC_STATE_QTUNES_INIT,
  VOC_STATE_QTUNES_CONFIG,
  VOC_STATE_QTUNES_ENTRY,
  VOC_STATE_QTUNES,
  VOC_STATE_QTUNES_DISABLE_PENDING,
#endif /* FEATURE_QTUNES_COMMON */
#ifdef FEATURE_GRAPH_AUDIO
  VOC_STATE_GRAPH_INIT,
  VOC_STATE_GRAPH_IDLE_ENTRY,
  VOC_STATE_GRAPH_IDLE,
  VOC_STATE_GRAPH_AUDIO_ENTRY,
  VOC_STATE_GRAPH_AUDIO,
#endif /* FEATURE_GRAPH_AUDIO */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  VOC_STATE_MIXER_INIT,
  VOC_STATE_MIXER_IDLE_ENTRY,
  VOC_STATE_MIXER_IDLE,
  VOC_STATE_MIXER_ACTIVE_ENTRY,
  VOC_STATE_MIXER_ACTIVE,
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
  VOC_STATE_CAM_AUDIO_INIT_ENTRY,
  VOC_STATE_CAM_AUDIO_INIT,
  VOC_STATE_CAM_AUDIO_CONFIG,
  VOC_STATE_CAM_AUDIO_ENTRY,
  VOC_STATE_CAM_AUDIO_ACTIVE,
  VOC_STATE_CAM_AUDIO_DISABLE_PENDING,
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
  VOC_STATE_ERROR_RECOVERY
} voc_state_type;

/* Return values for QDSP2 State Control Functions
*/
typedef enum {
  VOC_STATE_SUCCESS = 0,
  VOC_STATE_CONTINUE,
  VOC_STATE_CONTINUE_IMMEDIATE,
  VOC_STATE_FAILURE
} voc_state_return_type;

typedef enum {
  VOC_STATE_SLIP_MAN,
  VOC_STATE_SLIP_AUTO
} voc_state_slip_type;

#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */

typedef enum {
  VOC_TEST_NONE,                  /* loopback test deactivated               */
  VOC_TEST_AUDIO_LOOP,            /* enc-to-dec audio front end loopback     */
  VOC_TEST_INT_PKT_LOOP,          /* enc-to-dec DSP internal packet loopback */
#ifdef FEATURE_UP_LOOPBACK_TEST       
  VOC_TEST_UP_PKT_LOOP,           /* enc-to-dec packet loopback through ARM  */
#endif
  VOC_TEST_D2E_LOOP,              /* dec-to-enc DSP internal packet loopback */
  VOC_TEST_DFM_LOOP               /* DFM enc-to-dec packet loopback          */
} voc_test_mode_type;

#ifdef FEATURE_GRAPH_AUDIO
/* This enum type is used to indicate the state of the audio mode selection
** for graphics images.
*/
typedef enum {
  VOC_STATE_AUDIO_MODE_NONE,     /* No audio mode is selected        */
  VOC_STATE_AUDIO_MODE_13K,      /* 13K vocoder mode is selected     */
  VOC_STATE_AUDIO_MODE_EVRC,     /* EVRC vocoder mode is selected    */
  VOC_STATE_AUDIO_MODE_AMR,      /* AMR vocoder mode is selected     */
  VOC_STATE_AUDIO_MODE_AAC,      /* AAC LC vocoder mode is selected     */
  VOC_STATE_AUDIO_MODE_DISABLED  /* Audio mode selection is disabled */
} voc_state_audio_mode_type;
#endif /* FEATURE_GRAPH_AUDIO */

/* The order of these enum values is extremely important! 
*/
typedef enum {
  VOC_STATE_MOD_STATUS_DISABLED,
  VOC_STATE_MOD_STATUS_ENABLE_REQUESTED,
  VOC_STATE_MOD_STATUS_ENABLED,
  VOC_STATE_MOD_STATUS_DISABLE_PENDING
} voc_state_mod_status_type;

/* The variables that affect the driver state
*/
typedef struct {
  qdsp_app_type          current_qdsps_app; /* Current QDSPS client id        */
  qdsp_image_type        current_image;     /* Current DSP image loaded       */
  qdsp_module_type       module_reqd;       /* Required DSP module            */
  voc_state_mod_status_type module_status;  /* Current module_status          */
  boolean                image_changed;     /* Image changed flag             */
  uint32                 num_higher_priority_apps;
  uint16                 acquire_mask;      /* All apps that have acquired    */
  uint16                 config_mask;       /* All apps that have config'ed   */
  boolean                config_flag;       /* Flag to indicate new config    */
  boolean                afe_configured;    /*  Flag to indicate if afe is configured*/
  voc_capability_type    config_pending;    /* Pending configuration          */
  voc_capability_type    config;            /* Current configuration          */
  boolean                config_standby;    /* Configure the DSP; Stay in idle*/
  boolean                codec_pending;     /* new codec pending for config   */
  voc_state_slip_type    slip_mode_pending; /* Pending sample slip mode       */
  voc_state_slip_type    slip_mode;         /* Current sample slip mode       */
  boolean                reset_flag;        /* First time up - reset          */
  uint16                 retry_counter;
  voc_state_cb_type      cb_table[ VOC_MAX_ACQUIRE_TYPE ];
  voc_state_type         state;
#ifdef FEATURE_PHONE_VR
  voc_vr_mode_type      vr_mode;            /* Current VR mode                */
#endif
#ifdef FEATURE_TTY
  voc_tty_mode_type     tty_mode;           /* Current TTY mode               */
#endif
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */

  voc_test_mode_type    test_mode_pending;  /* Pending test mode              */
  voc_test_mode_type    test_mode;          /* Current test mode              */

#ifdef FEATURE_QSYNTH_COMMON
  uint8                 hybrid_mode_pending; /* QSynth pending hybrid mode    */
  uint8                 hybrid_mode;         /* QSynth hybrid mode            */
  voc_qsynth_vol_mode_enum_type qsynth_vol_mode_pending;
  voc_qsynth_vol_mode_enum_type qsynth_vol_mode;
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QSYNTH_ADPCM
  voc_qsynth_adec_mode_type adec_mode_pending; /* Pending QSynth ADEC         */
  voc_qsynth_adec_mode_type adec_mode;         /* Current QSynth ADEC         */
#endif /* FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_WAVE_PB
  boolean adpcm_mode_enabled;               /* ADPCM mode is enabled/disabled */
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
#ifdef FEATURE_VOC_ADPCM
  boolean adpcm_voc_config;                 /* ADPCM VOC configuration flag   */
#endif /* FEATURE_VOC_ADPCM */
  boolean adpcm_mode_flag;                  /* ADPCM mode changed flag        */
  boolean adpcm_new_config;                 /* ADPCM new configuration flag   */
  uint16  adpcm_block;                      /* ADPCM block size config        */
  uint16  adpcm_sampling_rate;              /* ADPCM sampling rate config     */
  uint16  adpcm_num_channels;               /* ADPCM number of channels       */
  voc_adpcm_play_enum_type play_type;
  voc_adec_start_mode_enum_type start_mode;
  voc_adpcm_mode_enum_type   adpcm_mode;
  uint16  bits_per_sample;
  boolean   adpcm_start_pending;
#ifdef FEATURE_VOC_G711
  voc_adpcm_mode_enum_type wav_config;      /* WAV mode                       */
#endif /* FEATURE_VOC_G711 */
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_GRAPH_AUDIO
  voc_state_audio_mode_type audio_mode;     /* State of audio mode selection  */
#endif /* FEATURE_GRAPH_AUDIO */

#ifdef FEATURE_VOC_PCM_INTERFACE
  uint32  pcm_int_mode_pending;    /* Bitmask of PCM interfaces to be enabled */
  uint32  pcm_int_mode;            /* Bitmask of PCM interfaces enabled       */
#endif /* FEATURE_VOC_PCM_INTERFACE */

#ifdef FEATURE_QTUNES_COMMON
#if defined(FEATURE_AAC) || defined(FEATURE_AAC_REC)
  voc_aac_config_type    aac_config;        /* AAC song configuration         */
#endif
#if defined FEATURE_MP3
  voc_mp3_config_type    mp3_config;        /* MP3 song configuration         */
#endif
#ifdef FEATURE_REAL_AUDIO
  voc_ra_dsp_config_type real_audio_config; /* RealAudio song configuration   */
#endif
#ifdef FEATURE_WMA
  voc_wma_config_type    wma_config;        /*  Hold WMA config information   */
#endif
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  voc_amr_wbp_config_type amr_wbp_config;   /* AMR-WB+ config information     */
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

  boolean               tunes_config_pending; /* TRUE=New adec config pending */
#endif /* FEATURE_QTUNES_COMMON */
#ifdef FEATURE_MM_REC
  boolean               mm_rec_config_pending;/* TRUE=New rec  config pending */
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_AUDIO_FORMAT
#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
  voc_adec_mix_mode_type audfmt_mix_mode;
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_CMX
          FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#if defined(FEATURE_AVS_INCALL_MIXER_CMX) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  voc_capability_type    snd_config;
  voc_capability_type    voc_config;
  boolean                exit_mixer_state;
#endif 
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
/* I don't think we need this anymore */
  /*boolean mixer_vocoder_active_flag;
  boolean mixer_audio_active_flag; */
#endif
  boolean device_changed;
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

} voc_state_control_type;

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S                
  
===========================================================================*/

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
extern boolean voc_state_voc_tx_wb_query(void);


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
extern boolean voc_state_aux_pcm_clk_query(void);


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
extern void voc_state_test_mode_config(
  voc_test_mode_type test        /* test mode to be configured */
);


/* <EJECT> */
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
void voc_state_set_codec_cal ( voc_codec_type pcm_path );

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
);
#endif /* FEATURE_EXTERNAL_SDAC */

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
voc_state_return_type voc_state_init_aud_process( void );

/* <EJECT> */
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
extern voc_state_return_type voc_state_init_afe( void );

/* <EJECT> */
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
extern voc_state_control_type *voc_state_get_state_data (void);

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
void voc_inform (voc_inform_type inform_val);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_get_status

DESCRIPTION
  Converts the current internal state to the value needed by vocoder clients.

DEPENDENCIES
  None.  

RETURN VALUE
  The inform value for vocoder clients.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_inform_type voc_state_get_inform_val (void);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_enable_clocks

DESCRIPTION
  Enables codec PLL and all vocoder and codec clock regimes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_state_enable_clocks( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_disable_clocks

DESCRIPTION
  Disables codec PLL and all vocoder and codec clock regimes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_state_disable_clocks( void );

/* <EJECT> */
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
extern void voc_state_disable_codec_clk (void);
#endif /* MSMAUD_CODEC_CLOCK_CTRL */

/* <EJECT> */
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
extern clkrgm_codec_clk_type voc_state_get_icodec_clk_freq (
  voc_sample_rate_type rate
);

/* <EJECT> */
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
extern void voc_state_set_icodec_clk (
  voc_sample_rate_type rate
);

/* <EJECT> */
#ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
#endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */

/* <EJECT> */
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
extern void voc_state_adie_stall(void);

/* <EJECT> */
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
extern void voc_state_adie_sleep(void);

/* <EJECT> */
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
extern void voc_state_bt_adie_wakeup(voc_codec_type codec);
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
extern boolean voc_state_adie_wakeup(void);
#endif /* MSMAUD_ADIE_CODEC */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_ctrl

DESCRIPTION
  Drives the QDSP2 into the desired state based on command inputs.

DEPENDENCIES
  Call voc_state_control_init once before any calls to this function.

RETURN VALUE
  None
  
SIDE EFFECTS
  Changes state of the QDSP2

===========================================================================*/
extern void voc_state_control (void);

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
extern void voc_state_control_init (void);

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
extern voc_state_return_type voc_state_set_tty (
  voc_tty_mode_type tty_mode
);
#endif /* FEATURE_TTY */

/* <EJECT> */
#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
/*===========================================================================

FUNCTION voc_state_pp_ctl

DESCRIPTION
  Enables/Disables Post processing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_state_pp_ctl 
( 
  voc_pp_cb_type  pp_cb
    /* Callback to receive the data */
);

/* <EJECT> */
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
void voc_state_set_pp_status 
( 
  voc_pp_status_type new_status 
);
#endif /* (FEATURE_ARM_POST_PROCESSING)||(FEATURE_AVS_WB_PCM_RX_INTERFACE) */

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
void voc_state_shutdown (void);

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
extern voc_status_type voc_state_get_high_module (
  qdsp_module_type *module_select
);

/*===========================================================================

FUNCTION voc_state_get_clocks_on

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean voc_state_get_clocks_on (void);

/*===========================================================================

FUNCTION voc_state_set_clocks_on

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_state_set_clocks_on (boolean set);

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
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
extern voc_pp_status_type voc_state_get_pp_status(void);
#endif /* FEATURE_ARM_POST_PROCESSING || FEATURE_AVS_WB_PCM_RX_INTERFACE */

/*===========================================================================

FUNCTION voc_state_clear_fw_err_cnt

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern void voc_state_clear_fw_err_cnt (void);

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
extern voc_status_type voc_state_write_codec_gain (void);

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
extern voc_state_return_type voc_state_voc_config (voc_capability_type so);

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
extern voc_state_return_type voc_state_go_to_idle (void);

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
extern void voc_state_set_pcm_modes(void);
#endif

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
void voc_state_set_oemsbl_cb(voc_oemsbl_cb_func_ptr_type func_ptr);
#endif

/* <EJECT> */

/*===========================================================================

FUNCTION voc_state_enable_eclock

DESCRIPTION
  Increments a count of the external clock Vocoder has requested
  Conditionally enables vocoder and external codec clock regimes, if not already enabled.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_state_enable_eclock(void);
/*===========================================================================

FUNCTION voc_state_disable_eclock

DESCRIPTION
  Decrements a count of the eclocks Vocoder has requested
  Conditionally disables all vocoder and external codec clock regimes -
  theoretically if not already disabled, and only if the vocoder is acquired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Interrupts are locked.

===========================================================================*/
extern void voc_state_disable_eclock(void);

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
/*===========================================================================

FUNCTION voc_state_check_ica_mixer_module

DESCRIPTION
  
  
DEPENDENCIES

  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean voc_state_check_ica_mixer_module
(
  qdsp_module_type module
);

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
extern boolean voc_state_incall_synth_ready(void);
#endif /* FEATURE_AVS_INCALL_MIXER_CMX */

#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB || 
          FEATURE_AVS_INCALL_MIXER_CMX */

/*===========================================================================

FUNCTION voc_state_module_commit

DESCRIPTION
  This function verifies that incall synth is in ready state
  
DEPENDENCIES

  
RETURN VALUE
  boolean value indicates synth is ready or not

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_state_module_commit
(
  qdsp_module_type module
);

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
void voc_state_set_oemsbl_cb(voc_oemsbl_cb_func_ptr_type func_ptr);
#endif

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
voc_state_return_type voc_state_cam_audio_init_entry(void);

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
voc_state_return_type voc_state_cam_audio_init(void);



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
voc_state_return_type voc_state_cam_audio_config(void);


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
voc_state_return_type voc_state_cam_audio_entry(void);


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
voc_state_return_type voc_state_cam_audio_active(void);

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
void voc_state_set_pa_on_cb(voc_pa_on_cb_func_ptr_type func_ptr);

#endif /* VOCSTATE_H */


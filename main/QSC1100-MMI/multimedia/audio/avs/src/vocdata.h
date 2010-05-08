#ifndef VOCDATA_H
#define VOCDATA_H
/*===========================================================================

                   Q D S P 2   V O C O D E R   D R I V E R
  C A L I B R A T I O N    A N D   S T A T E   D A T A   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions for Vocoder driver state data.  

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

Copyright (c) 1999 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocdata.h#21 $ $DateTime: 2009/05/07 22:40:10 $ $Author: mkiran $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/09    vm     Added Support for Device switch from external to internal.
05/05/09   knm     Added feature to dynamically configure the adie on ULC
04/06/09    av     Adding VP3 Feature.
                   Support for TxPCM filter before EC
                   Updated Copyright.  
01/19/09   anb     Added voc_data_get/set_qtv_cb_func so that
                   QTV can be informed about mod/image swap event.
                   Fixes Cr 163807(SR-1133583)
03/10/09   sj      Added slowtalk feature support
12/26/08   rm      Added RVE API changes
12/26/08   skr     added MACRO definition(VOC_GRAPH_ADPCM_VALID_CHANNELS etc) 
                   to fix compilation issues due to CL 807920
12/23/08    vm     Added test code for AUXPCM testing support 
11/26/08   skr     Modified the code to fix ThinUI compilation issues.
11/26/08   skr     Fixed compiler warning
11/24/08   skr     Added code for Audio + camera Concurrency
11/06/08    kk     Fixed compilation error due to adding idlemetring support.
10/17/08   vm      added idlemetring support for qtv image
28/08/08    rm     Added support for RVE.
05/14/08    sr     New API voc_data_get_default_frame_rate(image,codec) .
                   Fix for CR 134407 - 16khz PCM sampling. Updated copyright.
03/17/08  ymc/anb  Modified codec gain ctrl api to control gain changes.
                   Fix for SC2X codec pop issue through MSMAUD_PCM_DC_OFFSET feature.
02/07/08    kk     Added support for Qensemble phase 2 interface changes.
12/19/07    hs     Added support ISDB-T dual mono output control
12/07/07    kk     Fixed compiler warnings due to QEnsemble.
11/14/07    hs     Added QEnsemble support
08/28/07   ymc     Added Crystal Speech support.
06/12/07    sg     Added support for selection of the AAC module based on MIPs
                   requirements.
06/08/07    sg     Added support for selection of the MP3 module based on MIPs
                   requirements.
04/20/07  sud/anb  Added PMIC MIC-BIAS support for QSC60X5.
02/26/07   kan     Added AMR-WB changes 
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
07/26/06  nm/ymc   Added FEATURE_AVS_NEXTGEN_EC.
07/26/06  nm/ymc   Added voc_data_[gs]et_1x_tx_rate_factor.
06/26/06    pl     Added new functions to convert volume from unit 
                   of mb to absolute value and vice verse.
                   Added new functions to convert 16.16 fixed value
                   to absolute value and vice versa.
04/03/06    ak     Added support for sidetone control.
03/24/06    pl     Seperate FIR filter from FEATURE_QTUNES_ADRC and 
                   FEATURE_QSYNTH_ADRC.
03/12/06    pl     Modified voc_data_get_qconcert_config() function prototype
                   to return status on whether global qconcert setting has 
                   been applied.
02/21/06   act     Added new parameter to voc_data_set_rx_mute() for
                   DTMF mixing support.
12/21/05    pl     Modified voc_data_get_qconcert_config() function prototype.
10/04/05    pl     Added voc_data_set_vox_override_mode() function prototype. 
                   to reoslve the 
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05    st     Added voc_data_compute_dbm_from_voc() prototype; added
                   QAudioFX function prototypes:
                   voc_data_get_3d_pos_output_mode(),
                   voc_data_set_3d_pos_enable_mode(),
                   voc_data_get_3d_pos_enable_mode().
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is 
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
08/31/05    sm     VOX support
            sm     Added voc_codec_mode_type() prototype.
07/13/05    sp     Added code to honor bitpool values preferred by BT A2DP 
                   headset. 
07/05/05    sm     Added G.723.1 support.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added support for MultiMedia/AAC record functionality.
05/04/05    aw     Replace PCM filter with IIR filter support for Qsynth.
04/21/05   act     Changed name of voc_codec_type to voc_codec_class_type.
                   Reverted name of voc_codec_id_type to voc_codec_type.
04/12/05    sg     Added voc_data_set_codec_cal() to allow the calibration to
                   set independently of voc_data_codec_inuse. Added function to
                   check if an image supports BT.
04/07/05   act     Added voc_data_set_audcal_mode.
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable.
03/25/05  ac/sg    Added support for A/V synchronization.
03/01/05    aw     Added support for Qconcert; preset EQ bandsl STF.
02/22/05    sm     Added G.711 vocoder support.
02/15/05   act     Added voc_data_get_ec_params().
02/15/05    sm     Added FEATURE_IIR_FILTER
02/05/05    st     Modified voc_data_set_dls_config() function prototype.
01/07/05   act     Added support for FEATURE_VOC_MIC_GAIN_ADJUST.
11/19/04    sp     Added support for Arm Post Processing.
06/08/04    aw     Added support for equalizer and spectrum analyzer.
                   Added voc_data_get_dtmf_duration to get DTMF duration.
05/11/04    vp     Merged ADRC and PCM filter support MP3/AAC from
                   MSMSHARED_AVS.00.09.05.02.00. Added support for H263 images.
            sm     Re-featurized gsm afe settings.
05/05/04    sm     Added voc_data_get_gsm_afe().
11/24/03    aw     Modified qsynth ADRC and PCM filter functions prototypes.
11/22/03    sg     Added support for RealAudio codec.
11/07/03  aw/st    Added support for Qsynth DLS.
10/30/03    sm     Added multimode calibration support.
10/10/03    aw     Added MSMAUD_VOC_AMR_COMMON featurization.
05/13/03   ymc     Added GSM half rate support.
           ymc     Updated copyright.
02/28/03    st     Added voc_data_set_ec_override_mode() and
                   voc_data_set_ns_override_mode() function prototypes.
02/19/03    st     Exposed voc_data_get_mm_rx_gain() for FEATURE_QSYNTH_ADPCM.
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
11/06/02    sm     Include msmaud.h.
10/29/02    sg     Modified voc_data_get_adie_codec_config() to use the new
                   adie cal. interface.
08/13/02    sg     Added support for AAC.
03/12/02    st     Replaced voc_data_set_codec_gain() with
                   voc_data_get_codec_gain(); added voc_data_set_dtmf_tx_mute()
                   and voc_data_get_dtmf_path_setting().
01/15/02   ymc     Added GSM and WCDMA support.
           ymc     Updated copyright.
11/19/01    aw     Added MSMAUD_ADIE_CODEC_CONFIG. 
09/04/01    aw     Added FEATURE_QSYNTH_ADRC. Added prototypes for getting
                   QSynth PCM filter and ADRC data structure from PCM path.
09/03/01    st     Added proper featurization around voc_data_get_mm_rx_gain().
06/18/01  jkl/ym   Added FEATURE_ACP.
05/21/01   jkl     Removed Enhanced VR feature.
03/04/01    st     Modified voc_data_get_codec_config() function prototype to
                   accept codec type.
01/26/01    st     Added voc_data_get_mm_rx_gain() function prototype.
12/10/00    st     Modified code to add proper featurization.
12/05/00   jkl     Merged VR4.0 functions. Added FEATURE_ENHANCED_VR,
                   FEATURE_DFM_ANSWER_MACHINE.
12/04/00    sm     Added voc_data_get/set_sample_rate().
09/21/00    sm     voc_data_get_codec() returns the pcm path only.  Added
                   voc_data_get_codec_config() to get the codec configuration.
07/14/00   spf     Removed voc_data_set_tty_mode().
06/11/00   spf     Updated references.
06/07/00    st     Added voc_data_register_codec_func() and 
                   voc_data_get_codec_func() function prototypes.
05/12/00   spf     Add TTY support: voc_data_set_tty_mode().
05/02/00   spf     Removed voc_data_set_dfm_sat_dcc_notify() and
                   voc_data_get_dfm_sat_dcc_notify().
            sm     Added get/set_aud_process_override().
03/28/00    sm     AAGC mainline.
03/08/00   spf     Moved DFM configuration to voccal.
01/25/00    ro     Added support for VR. Use voc_ec_type and voc_ns_type.
10/18/99    st     Added support for audio AGC/AVC.
10/14/99   jkl     Added functions to get Vocoder Diagnostic data.
10/04/99   jkl     Added new functions and reviewed PCM pad functions.
09/10/99   jkl     Included functions to access internal and external CODEC.
08/02/99   spf     Removed voc_data_set_timing().
07/15/99   spf     DFM support.
06/17/99    ro     Initial Version

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"           /* Customer Specific Definitions           */
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "msmaud.h"             /* MSMAudio features                       */
#include "voc.h"                /* Vocoder interface definition            */
#include "voccal.h"             /* Vocoder calibration database            */
#include "qdspcmd.h"            /* QDSP2 Driver Command Layer definitions  */


#include "vocvp3.h"             /*VP3 feature support*/


#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#include "audiosbc.h"
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_ADIE_DYNAMIC_CALIBRATION
#error code not present
#else
#define CONSTANT const
#endif /* FEATURE_ADIE_DYNAMIC_CALIBRATION */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Return type for data functions
*/
typedef enum {
  VOC_DATA_SUCCESS,
  VOC_DATA_FAILURE
} voc_data_return_type;

/* Volume selection type
*/
typedef enum {
  VOC_DATA_MUTE_VOL,
  VOC_DATA_NORM_VOL
} voc_data_vol_select_type;

typedef enum {
    VOC_SLOWTALK_STATIC_PARAMS_UPDATED,
    VOC_SLOWTALK_STATIC_PARAMS_NOT_UPDATED,
    VOC_SLOWTALK_STATIC_PARAMS_MAX
} voc_slowtalk_static_params_status_type;


#ifdef FEATURE_QSYNTH_ADPCM
/* These macros are used to verify that the ADPCM file is able to be played
** by the QSynth image. QSynth currently only supports single channel,
** 4 bits per sample and 8kHz or 16kHz or 32kHz sampling mode.
*/
#ifdef FEATURE_GASYNTH_INTERFACE
#define VOC_QSYNTH_ADPCM_VALID_CHANNELS(x)        ((x == 1) || (x == 2))
#else
#define VOC_QSYNTH_ADPCM_VALID_CHANNELS(x)        (x == 1)
#endif /* FEATURE_GASYNTH_INTERFACE */
#ifdef FEATURE_GASYNTH_INTERFACE
#define VOC_QSYNTH_ADPCM_VALID_SAMPLE_RATE(x)     (x <= 44100)
#else
#define VOC_QSYNTH_ADPCM_VALID_SAMPLE_RATE(x)     ((x == 8000)  || \
                                                   (x == 16000) || \
                                                   (x == 32000))
#endif /* FEATURE_GASYNTH_INTERFACE */
#define VOC_QSYNTH_ADPCM_VALID_BITS_PER_SAMPLE(x) (x == 4)

#ifdef FEATURE_STEREO_YADPCM
#define VOC_QSYNTH_YADPCM_VALID_CHANNELS(x)       ((x == 1) || (x == 2))
#else
#define VOC_QSYNTH_YADPCM_VALID_CHANNELS(x)       (x == 1)
#endif /* FEATURE_STEREO_YADPCM */

#ifdef FEATURE_GASYNTH_INTERFACE
#define VOC_QSYNTH_YADPCM_VALID_SAMPLE_RATE(x)    (x <= 44100)
#else
#define VOC_QSYNTH_YADPCM_VALID_SAMPLE_RATE(x)    (x <= 32000)
#endif /* FEATURE_GASYNTH_INTERFACE */

#define VOC_QSYNTH_YADPCM_PCM_VALID_BITS_PER_SAMPLE(x)   ((x == 4) || \
                                                          (x == 8))

#define VOC_QSYNTH_PCM_VALID_CHANNELS(x)          ((x == 1) || (x == 2))
#define VOC_QSYNTH_PCM_VALID_BITS_PER_SAMPLE(x)   ((x == 8) || (x == 16))
#ifdef FEATURE_GASYNTH_INTERFACE
#define VOC_QSYNTH_PCM_VALID_SAMPLE_RATE(x)       (x <= 48000)
#else
#define VOC_QSYNTH_PCM_VALID_SAMPLE_RATE(x)       ((x == 4000)   || \
                                                   (x == 8000)   || \
                                                   (x == 11025)  || \
                                                   (x == 12000)  || \
                                                   (x == 16000)  || \
                                                   (x == 22050)  || \
                                                   (x == 24000)  || \
                                                   (x == 32000)  || \
                                                   (x == 44100)  || \
                                                   (x == 48000))
#endif /* FEATURE_GASYNTH_INTERFACE */
#endif /* FEATURE_QSYNTH_ADPCM */


#if defined(FEATURE_GRAPH_ADPCM) || defined(FEATURE_VOC_ADPCM)
#define VOC_GRAPH_ADPCM_VALID_CHANNELS(x)        (x == 1)
#define VOC_GRAPH_ADPCM_VALID_BITS_PER_SAMPLE(x) (x == 4)
#endif /* FEATURE_GRAPH_ADPCM || FEATURE_VOC_ADPCM */

#ifdef FEATURE_VOC_ADPCM
#define VOC_VOC_32K_ADPCM_VALID_SAMPLE_RATE(x)     (x == 32000)
#define VOC_VOC_32K_ADPCM_VALID_BLOCK_SIZE(x)      (x == 512)
#endif /* FEATURE_VOC_ADPCM */

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S                
  
===========================================================================*/
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
extern voc_data_return_type voc_data_register_codec_func
(
  voc_codec_type                pcm_path,
  voc_amp_enable_func_ptr_type  func_ptr
);
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
extern voc_data_return_type voc_data_get_codec_func
(
  voc_codec_type                 pcm_path,
  voc_amp_enable_func_ptr_type  *func_ptr
);

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
extern voc_data_return_type voc_data_get_codec_mode (
  qdsp_image_type               image,
  voc_codec_mode_type           *codec_mode
);

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
extern voc_data_return_type voc_data_set_codec
( 
  voc_codec_type            pcm_path
);

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
extern voc_data_return_type voc_data_set_codec_cal
( 
  voc_codec_type               pcm_path,
  qdsp_image_type              image
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_codec

DESCRIPTION
  Copy the current codec type to the caller's local data.

DEPENDENCIES
  voc_data_set_codec() must be called prior.
 
RETURN VALUE
  None.

SIDE EFFECTS
  pcm_path_ptr

===========================================================================*/
extern void voc_data_get_codec
( 
  voc_codec_type*         pcm_path_ptr
);

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
extern void voc_data_get_codec_class
( 
  voc_codec_class_type*   codec_type_ptr
);

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
extern void voc_data_get_codec_vr_type
( 
  voc_codec_vr_type*         codec_type_ptr
);
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_default_frame_rate

DESCRIPTION
  Gets the default PCM sampling rate to be used depending on image and current
  codec. - Used only when there is no active/pending client.

DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_pcm_sample_rate_type voc_data_get_default_frame_rate
( 
  qdsp_image_type        image,
  voc_codec_type         codec
);

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
);

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
extern void voc_data_get_codec_offset(
  qdsp_cmd_codec_offset_ctrl_type *codec_offset_ptr
);

#endif /* MSMAUD_PCM_DC_OFFSET */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_codec_gain

DESCRIPTION
  Get CODEC gain values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  voc_data_codec_gain.codec_rx_gain
  voc_data_codec_gain.codec_st_gain
  voc_data_codec_gain.codec_tx_gain

===========================================================================*/
extern qdsp_cmd_codec_gain_ctrl_type* voc_data_get_codec_gain ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_pcmpad_vol

DESCRIPTION
  Set External CODEC pcm pad gains 
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the CODEC pcm pad state

SIDE EFFECTS
  voc_data_codec_pad

===========================================================================*/
extern uint16 voc_data_set_pcmpad_vol
(
  uint8   padval,             /* Value for pad bits          */
  uint8   mask,               /* Mask of pad bits to control */
  boolean update              /* Only update value if TRUE   */ 
);

/* <EJECT> */
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
);
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
extern voc_gsm_dtx_mode_type voc_data_get_gsm_dtx_mode(void);


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
extern void voc_data_set_gsm_dtx_mode(voc_gsm_dtx_mode_type dtx_mode);

#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR) || MSMAUD_VOC_HR || MSMAUD_VOC_AMR_WB_COMMON */

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
extern voc_amr_wb_frame_rate_type voc_data_get_amr_wb_init_rate(void);


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_amr_wb_init_rate

DESCRIPTION
  Set initial AMR_WB Tx rate database value.
  Invalid argument will be ignored.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_data_set_amr_wb_init_rate(
voc_amr_wb_frame_rate_type amr_wb_tx_rate
);

#endif  /* MSMAUD_VOC_AMR_WB_COMMON */

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
extern voc_gsm_frame_rate_type voc_data_get_amr_init_rate(void);


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
extern void voc_data_set_amr_init_rate(voc_gsm_frame_rate_type amr_tx_rate);

#endif  /* MSMAUD_VOC_AMR_COMMON */

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
extern void voc_data_set_1x_tx_rate_factor(word rate_factor);

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
extern word voc_data_get_1x_tx_rate_factor(void);

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
extern voc_g711_mode_type voc_data_get_g711_mode(void);

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
extern void voc_data_set_g711_mode(voc_g711_mode_type g711_mode);

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
extern voc_g723_mode_type voc_data_get_g723_mode(void);

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
extern void voc_data_set_g723_mode(voc_g723_mode_type g723_mode);

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
extern qdsp_cmd_vol_ctrl_type* voc_data_get_volume
(
  voc_data_vol_select_type vol_sel
);

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
extern void voc_data_set_rx_volume
( 
  sint15 rx_vol 
);

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
  voc_data_vo.rx_mute

===========================================================================*/
extern void voc_data_set_rx_mute
( 
  boolean mute_flag,
  boolean conditional_on_mixing
);

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
  voc_data_vol.tx_comfort_noise_ena

===========================================================================*/
extern void voc_data_set_tx_mute 
( 
  boolean mute_flag 
);

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
extern uint16 voc_data_get_dtmf_path_setting ( void );

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
extern void voc_data_set_dtmf_tx_mute (
  boolean mute_flag
);

/* <EJECT> */
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
extern qdsp_cmd_voc_timing_type* voc_data_get_timing
(
  voc_capability_type so
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_ec_mode

DESCRIPTION
  Get the echo-cancellation mode.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  voc_ec_type - pointer to the current echo canceller mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_ec_type* voc_data_get_ec_mode
(
  void
);

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
);

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
extern const qdsp_cmd_nextgen_ec_parms_type* voc_data_get_nextgen_ec_parms
(
  voc_ec_type ec_mode
);

#endif  /* FEATURE_AVS_NEXTGEN_EC */

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
extern
const qdsp_cmd_crystal_speech_parms_type* voc_data_get_crystal_speech_parms
(
  voc_ec_type ec_mode
);

#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

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
  None.

===========================================================================*/
extern voc_ns_type* voc_data_get_ns_mode
(
  void
);

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
extern voc_vox_type* voc_data_get_vox_mode
(
  void
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sidetone_mode

DESCRIPTION
  Get the sidetone mode.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  voc_sidetone_type - pointer to the current sidetone mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_sidetone_type* voc_data_get_sidetone_mode
(
  void
);

/* <EJECT> */

/*===========================================================================
FUNCTION voc_data_get_dtmf_tx_gain

DESCRIPTION
  Get the Transmit Gain for FM DTMF.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  uint16 - current DFMT Tx gain.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 voc_data_get_dtmf_tx_gain
(
  void
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_dtmf_rx_gain

DESCRIPTION
  Get the Receive Gain for FM DTMF.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  uint16 - current DFMT Rx gain.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 voc_data_get_dtmf_rx_gain
(
  sint15 vol
);

/*===========================================================================

FUNCTION voc_data_get_dtmf_duration

DESCRIPTION
  Get the duration time for FM DTMF.

DEPENDENCIES
  None.

RETURN VALUE
  uint16 - current DFMT duration.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 voc_data_get_dtmf_duration
(
  uint16 duration_ms
);

/* <EJECT> */
#if defined(FEATURE_MIDI) || defined(FEATURE_QTUNES_COMMON) || \
    defined(FEATURE_MIDI_OUT) || defined(FEATURE_QSYNTH_ADPCM)
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
);
#endif /* FEATURE_MIDI || FEATURE_QTUNES_COMMON || FEATURE_MIDI_OUT ||
          FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_pre_ec_pcm_filter
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
extern uint16* voc_data_get_pre_ec_tx_pcm_filter( void );
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
extern qdsp_cmd_pcm_filter_type* voc_data_get_rx_filter
(
  void
);

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
extern qdsp_cmd_pcm_filter_type* voc_data_get_tx_filter
(
  void
);

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
extern qdsp_cmd_pcm_filter_type* voc_data_get_tx_second_mic_filter
(
  void
);
#endif /*FEATURE_AVS_CRYSTAL_SPEECH*/

#ifdef FEATURE_IIR_FILTER
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_rx_iir_filter

DESCRIPTION
  Gets the Rx PCM IIR filter coefficients.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_iir_filter_type* - pointer to the current Rx PCM IIR filter 
                                  coefficients.

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_pcm_iir_filter_type* voc_data_get_rx_iir_filter
(
  int stage_cnt
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_tx_iir_filter

DESCRIPTION
  Gets the Tx PCM IIR filter coefficients

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_iir_filter_type* - pointer to the current Tx PCM IIR filter 
                                  coefficients.

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_pcm_iir_filter_type* voc_data_get_tx_iir_filter
(
  int stage_cnt
);

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/*===========================================================================

FUNCTION voc_data_get_tx_second_mic_iir_filter

DESCRIPTION
  Gets the Tx PCM SecondaryMic IIR filter coefficients

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_iir_filter_type* - pointer to the current Tx PCM IIR filter 
                                  coefficients.

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_pcm_iir_filter_type* voc_data_get_tx_second_mic_iir_filter
(
  int stage_cnt
);
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */

#endif /* FEATURE_IIR_FILTER */

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
extern void voc_data_set_ec_override_mode (
  voc_ec_ns_mode_type   ec_mode
);

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
extern void voc_data_set_ns_override_mode (
  voc_ec_ns_mode_type   ns_mode
);

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
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_sidetone_override_mode

DESCRIPTION
  Controls Sidetone.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_data_set_sidetone_override_mode (
  voc_sidetone_mode_type   sidetone_mode
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

/* <EJECT> */
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

extern voc_data_return_type voc_data_smartsnd_set_rx_agc
(
  voc_smartsnd_type agc_config
);
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

extern voc_data_return_type voc_data_smartsnd_set_rve
(
 voc_smartsnd_type rve_config
);

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
extern voc_data_return_type voc_data_set_rx_agc
(
  voc_agc_type agc_config
);

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
extern voc_data_return_type voc_data_set_tx_agc
(
  boolean      agc_config
);
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
extern void voc_data_get_agc
(
  qdsp_cmd_agc_config_type *agc_config_ptr
);


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

extern void voc_data_smartsnd_get_agc(
qdsp_cmd_agc_config_type  *qdsp_cmd_agc_confg_ptr
);


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
extern void voc_data_get_agc_param
(
  qdsp_cmd_agc_param_type *agc_param_ptr
);



/* <EJECT> */
/*=============================================================================

FUNCTION voc_data_smartsnd_get_agc_param

DESCRIPTION
  This function returns the QDSP AGC format command based on the current
  PCM Path AGC/AVC configuration.

DEPENDENCIES
  voc_data_smartsnd_rx_agc_config
  voc_data_pcm_cal_ptr

RETURN VALUE
  qdsp_cmd_agc_param_type

SIDE EFFECTS
  None.
 
  
 =============================================================================*/
extern void voc_data_smartsnd_get_agc_param
(
  qdsp_cmd_agc_param_type *agc_param_ptr
);


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
extern void voc_data_get_rve
(
  qdsp_cmd_rve_config_type *rve_config_ptr
);

/* <EJECT> */

/*===========================================================================

FUNCTION voc_data_get_rve_param

DESCRIPTION
  This function returns the QDSP RVE format command based on the current
  PCM Path RVE configuration, for Single Mic

DEPENDENCIES
  voc_data_pcm_cal_ptr

RETURN VALUE
  qdsp_cmd_rve_param_type

SIDE EFFECTS
  None.

===========================================================================*/
extern const qdsp_cmd_rve_param_type * voc_data_get_rve_param
(
 voc_ec_type ec_mode
);
/*===========================================================================

FUNCTION voc_data_get_dual_rve_param

DESCRIPTION
  This function returns the QDSP RVE format command based on the current
  PCM Path RVE configuration, for Dual Mic.

DEPENDENCIES
  voc_data_pcm_cal_ptr

RETURN VALUE
  qdsp_cmd_dual_rve_param_type

SIDE EFFECTS
  None.

===========================================================================*/
extern const qdsp_cmd_dual_rve_param_type* voc_data_get_dual_rve_param
(
 voc_ec_type ec_mode
);


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_slip_mode

DESCRIPTION
  Get sample slip mode.

DEPENDENCIES
  None.

RETURN VALUE
  uint16 - sample slip control value.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 voc_data_get_slip_mode
(
  voc_capability_type so 
);

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
extern void voc_data_set_dfm_sat_dcc
( 
   voc_scc_type sat_dcc
   /* New SAT DCC value */  
);

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
extern voc_scc_type voc_data_get_dfm_sat_dcc
( 
  void 
);

/* <EJECT> */
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
extern sint15 voc_data_compute_dbm_from_voc
(
  uint16  nominal_val,
    /* Value of the hardware register for a 0 DBM       */
  uint16  voc_value
);

/* <EJECT> */
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
);
/* <EJECT> */
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
);
/* <EJECT> */
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
extern qdsp_cmd_diag_type* voc_data_get_diags
(
  void
);

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
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_aud_process_override

DESCRIPTION
  Get the current audio process override value.

DEPENDENCIES
  None.

RETURN VALUE
  voc_aud_process_override

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean voc_data_get_aud_process_override
( 
  void
);

/* <EJECT> */
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
extern void voc_data_set_vr_mode ( 
  voc_vr_mode_type  vr_mode
);

/* <EJECT> */
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
extern void voc_data_set_vr_cal ( 
  voc_vr_cal_type *vr_cal_ptr
);

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
extern qdsp_cmd_pcm_filter_type* voc_data_get_vr_filter (
  void
);

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
extern voc_ec_type* voc_data_get_vr_ec (
  void
);

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
extern voc_ns_type* voc_data_get_vr_ns (
  void
);
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
);

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
);

#ifdef FEATURE_PHONE_VR
/* <EJECT> */
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
extern const unsigned short* voc_data_get_vr_bark_weight (
  voc_wideband_vr_type band_selection
);
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#if defined(FEATURE_AUDFMT_FIR_FILTER) && defined(FEATURE_QTUNES_COMMON)
/*===========================================================================

FUNCTION voc_data_get_qtunes_pcm_filter

DESCRIPTION
  Get the Qtunes PCM filter config for the current path.

DEPENDENCIES
   FEATURE_QTUNES_ADRC

RETURN VALUE
  Pointer to the Qtunes PCM filter config structure.

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_audfmt_pcm_filter_type *voc_data_get_qtunes_pcm_filter
(
  void
);
#endif /* FEATURE_AUDFMT_FIR_FILTER && FEATURE_QTUNES_COMMON */

/* <EJECT> */
#if defined(FEATURE_QTUNES_ADRC) || defined(FEATURE_QSYNTH_ADRC)
/*===========================================================================

FUNCTION voc_data_get_audfmt_adrc_ctl

DESCRIPTION
  Get the Qtunes ADRC config for the current path.

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
);
#endif /* FEATURE_QTUNES_ADRC */

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
extern const qdsp_cmd_audfmt_pcm_filter_type *voc_data_get_qsynth_pcm_filter
(
  void
);
#endif /* FEATURE_AUDFMT_FIR_FILTER && FEATURE_QSYNTH_COMMON */

/* <EJECT> */
#ifdef MSMAUD_ADIE_CODEC
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
extern voc_adie_codec_config_type *voc_data_get_adie_codec_config
(
  qdsp_image_type              image,
  voc_codec_type               codec
);

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
extern voc_adie_codec_config_type *voc_data_get_adie_codec_pwr_down_config
(
  void
);
#endif /* MSMAUD_ADIE_CODEC */
/* <EJECT> */
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
extern voc_pmic_config_type *voc_data_get_pmic_config
(
  qdsp_image_type              image,
  voc_codec_type               codec
);

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
extern void voc_data_set_dls_config (
  voc_dls_config_type *dls_config
);

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
extern qdsp_cmd_dls_config_type *voc_data_get_dls_config (void);
#endif /* FEATURE_MIDI */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_EQUALIZER
/*===========================================================================

FUNCTION voc_data_enable_equalizer

DESCRIPTION
  This function will enable/dsiable the Equalizer control flag for
  QSYNTH/QTUNES.

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
);


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_equalizer

DESCRIPTION
  This function will setup the Equalizer control filters QSYNTH/QTUNES.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_data_set_equalizer
(
  voc_equalizer_filter_type *filter,
  uint16                    num
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_equalizer_gain

DESCRIPTION
  Set the new equalizer gain value on a specified band.

DEPENDENCIES
  FEATURE_QSYNTH_ADRC

RETURN VALUE
  Pointer to the QSYNTH ADRC control structure.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_data_set_equalizer_gain
(
  uint16 band,
  int16  gain
);

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
);

/*===========================================================================

FUNCTION voc_data_set_preset_eq_coeff

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_data_set_preset_eq_coeff
(
  const uint16** VOC_SAMPLE_RATE_MAX,
  uint16         bands,
  boolean        enable
);
#endif /* FEATURE_AUDFMT_EQUALIZER */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/*===========================================================================

FUNCTION voc_data_get_spectrum_analyzer_period

DESCRIPTION
  Set the update period for spectrum analyzer.

DEPENDENCIES
  FEATURE_QSYNTH_ADRC

RETURN VALUE
  Pointer to the QSYNTH ADRC control structure.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_set_spectrum_analyzer_period
(
  uint16 update_period
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_spectrum_analyzer_interval

DESCRIPTION
  Get the update interval for spectrum analyzer.

DEPENDENCIES
  FEATURE_QSYNTH_ADRC

RETURN VALUE
  Pointer to the QSYNTH ADRC control structure.

SIDE EFFECTS
  None.

===========================================================================*/
const uint16 voc_data_get_spectrum_analyzer_interval
(
  void
);
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
void voc_data_enable_qconcert( boolean enable);

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
); 

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
);
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
);
#endif /* FEATURE_AUDFMT_STF */
 
/* <EJECT> */
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
);
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
extern boolean voc_data_is_stereo_tx_codec(void);

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
extern boolean voc_data_stereo_codec(void);

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
extern void voc_data_register_pp_func (voc_pp_cb_type pp_cb);

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
voc_pp_cb_type voc_data_get_pp_func(void);
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
void voc_data_save_pp_func (voc_pp_cb_type pp_cb, voc_pp_type_type pp_type);

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
voc_pp_cb_type voc_data_get_saved_pp_func(voc_pp_type_type pp_type);

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
void voc_data_set_pp_type(voc_pp_type_type pp_type);

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
voc_pp_type_type voc_data_get_pp_type(void);
#endif /* ( FEATURE_ARM_POST_PROCESSING )|| (FEATURE_AVS_WB_PCM_RX_INTERFACE ) */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT> */
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
void voc_data_set_sbc_remote_params(audiosbcenc_data_type *remote_params);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sbc_channel_mode

DESCRIPTION
 Gets the SBC channel_mode type 
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
audiosbcenc_data_type *voc_data_get_sbc_remote_params(void);

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
audiosbc_sample_rate_type voc_data_get_sbc_sample_rate(void);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_image_supports_bt_a2dp

DESCRIPTION
  Checks if the specified firmware image supports the Bluetooth SBC codec.

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE is supported; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean voc_data_image_supports_bt_a2dp 
(
  qdsp_image_type image
);
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

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_data_return_type voc_data_set_mic_gain_adjust
(
  voc_codec_type    codec,
  sint15            dbm_val
);
#endif /* FEATURE_VOC_MIC_GAIN_ADJUST */

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
extern const uint16 voc_data_get_num_samples_per_sec ( void );

#ifdef FEATURE_BT_AG
/*===========================================================================

FUNCTION voc_data_image_supports_bt_ag

DESCRIPTION
  Checks if the specified firmware image supports the Bluetooth AG codec.

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE is supported; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean voc_data_image_supports_bt_ag
(
  qdsp_image_type image
);
#endif /* FEATURE_BT_AG */

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION voc_data_set_mm_parm

DESCRIPTION
  Set the current multimedia record parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_mm_rec_parm

===========================================================================*/
extern void voc_data_set_mm_parm (
  voc_sample_rate_type  sample_rate,
  voc_mm_rec_chan_type  chan_index,
  uint16                bit_per_spl
);

/*===========================================================================

FUNCTION voc_data_get_mm_parm

DESCRIPTION
  Get the current multimedia record parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_mm_rec_parm

===========================================================================*/
extern voc_mm_rec_parm_type* voc_data_get_mm_parm (void);
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
extern void voc_data_set_3d_pos_enable_mode
(
  voc_3d_pos_mode_enum_type mode
);

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
extern voc_3d_pos_mode_enum_type voc_data_get_3d_pos_enable_mode(void);

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
extern qdsp_cmd_3d_pos_output_mode_enum_type
                                  voc_data_get_3d_pos_output_mode(void);

/* The following functions are only used by FEATURE_QAUDIOFX for now.
   But, it can be used for general purpose */
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
);

/*===========================================================================

FUNCTION voc_data_compute_mb_from_abs

DESCRIPTION
  This function returns the mb value from hardware register value.

DEPENDENCIES
  Requires voc_data_mb_table1[],
           voc_data_mb_table2[]
           voc_data_mb_table3[]
RETURN VALUE
  Returns a 32-bit integer containing the mb value.

SIDE EFFECTS
  None.
===========================================================================*/
sint31 voc_data_compute_mb_from_abs(
  uint16 abs_value
);

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
);

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
);
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
void voc_data_get_sbc_config(qdsp_cmd_sbc_codec_config_type *config_ptr);

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
uint32 voc_data_get_sbc_bitrate(void);

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
void voc_data_set_sbc_bitrate(uint32 bitrate);
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_sbc_config

DESCRIPTION
  This function sets the sbc encoder params

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
voc_status_type voc_data_set_sbc_config(
  voc_sbc_config_params_type *config_ptr
);
#endif
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
void voc_data_set_qtv_cb_func(voc_notify_qtv_cb_func_ptr_type cb_func);
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
voc_notify_qtv_cb_func_ptr_type voc_data_get_qtv_cb_func(void);

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
void voc_data_set_bt_cb_func(voc_bt_cb_func_ptr_type cb_func);
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_bt_cb_func 

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
voc_bt_cb_func_ptr_type voc_data_get_bt_cb_func(void);
#endif /*AVS_BT_SCO_REWORK*/

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
void voc_data_set_dual_mono_mode(uint16 mode);

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
uint16 voc_data_get_dual_mono_mode(void);

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
void voc_data_set_sbr_mode(uint16 mode);

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
uint16 voc_data_get_sbr_mode(void);

#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

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
qdsp_module_type voc_data_get_qtunes_module(voc_capability_type voc_cap);

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
boolean voc_data_current_image_supports_dcs(void);
#endif /* FEATURE_QTUNES_COMMON */

/*=======================================================================                       
FUNCTION voc_data_get_current_codec

DESCRIPTION
  Returns current codec in use

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

=======================================================================*/
extern voc_codec_type voc_data_get_current_codec(void);

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
extern void voc_data_set_idle_metering(uint16);

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
extern uint16 voc_data_get_idle_metering(void);

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
);
#endif /*# defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM) */


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

extern void* voc_data_get_vp3_buf_id_and_offset
(
  vocvp3_buf_id_type* buf_id
);


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
extern unsigned int* voc_data_get_buf_id_to_buf_size_table
(
  void
);


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
extern uint16** voc_data_get_buf_id_to_default_data_table
(
  void
);



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
voc_service_cfg_type voc_data_get_service_option
(
  void
);


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
void voc_data_set_service_option
(
  voc_service_cfg_type service
);

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
void voc_data_set_slowtalk_enable_flag ( boolean enable);

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
boolean voc_data_get_slowtalk_enable_flag ( void );

/*===========================================================================

FUNCTION voc_data_set_slowtalk_static_params_status_flag

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_set_slowtalk_static_params_status(
  voc_slowtalk_static_params_status_type flag
);

/*===========================================================================

FUNCTION voc_data_get_slowtalk_static_params_status_flag

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
voc_slowtalk_static_params_status_type 
voc_data_get_slowtalk_static_params_status (void);

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
extern void voc_data_set_eclock_switched(boolean change);

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
extern boolean voc_data_get_eclock_switched(void);

#endif /* VOCDATA_H */


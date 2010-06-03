#ifndef VOCCAL_H
#define VOCCAL_H
/*===========================================================================

           V O C O D E R   C A L I B R A T I O N   D A T A B A S E
              D A T A   S T O R A G E  A N D   R E T R I V A L

DESCRIPTION
  This header file contains all the definitions necessary to access the
  data contained in the vocoder calibration database.
  
Copyright (c) 1999 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/voccal.h#8 $ $DateTime: 2009/05/06 00:16:35 $ $Author: kmodak $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/09   kmm     Added FEATURE_ADIE_DYNAMIC_CALIBRATION macro for dynamic
                   adie calibration.
04/06/09   av      Adding support for TxPCM filter before EC
                   Copy right updated.
03/10/09   sj      Added slowtalk feature support
28/08/08   rm	   Added support for RVE
05/08/08   pl      CR 143215 - Modify qsynth_adrc_ctl to double pointer.
03/17/08  ymc/anb  Modified codec gain ctrl api to control gain changes.
                   Fix for SC2X codec pop issue through MSMAUD_PCM_DC_OFFSET feature.
02/07/08   mas     Renamed AUDIO_CALIBRATION to FEATURE_AVS_DYNAMIC_CALIBRATION
                   Updated Copyright
08/28/07   ymc     Added Crystal Speech support.
08/23/07   ymc     Added wideband EC parameters.
05/14/07   sud     EPIC Vreg_NCP support in ground referenced capless mode
                   for QSC60x5 chipsets.
04/20/07  sud/anb  Fixed compiler warning.
04/20/07  sud/anb  Added PMIC MIC-BIAS support for QSC60X5.
02/26/07   kan     Added AMR-WB changes 
07/26/06  nm/ymc   Added FEATURE_AVS_NEXTGEN_EC.
07/10/06   act     Modified voc_pcm_path_cal_type to include HSSD parameter.
03/24/06    pl     Seperate FIR filter out of FEATURE_QSYNTH_ADRC and 
                   FEATURE_QTUNES_ARRC.
09/18/05    st     Added output_3d_mode to voc_pcm_path_cal_type.
07/05/05    aw     Modified qsynth pcm filter for backward compatiblity. 
05/04/05    aw     Replace PCM filter with IIR filter support for Qsynth.
04/21/05   act     Changed name of voc_codec_type to voc_codec_class_type.
                   Reverted name of voc_codec_id_type to voc_codec_type.
04/01/05   act     Added tables and access functions to associate a specific
                   codec with a generic codec type.
03/01/05    aw     Added support for side chaing tuning filters.
02/15/05   act     Added EC parameter structure to voc_pcm_path_cal_type.
02/15/05    sm     Added FEATURE_IIR_FILTER
01/07/05   act     Added support for FEATURE_VOC_MIC_GAIN_ADJUST and 
                   FEATURE_VOC_NV_CAL
05/11/04    vp     Merged ADRC and PCM filter support MP3/AAC from
                   MSMSHARED_AVS.00.09.05.02.00.
11/24/03    aw     Modified code to support qsynth ADRC and PCM filter at
                   different output sampling rate.
11/07/03    aw     Removed adie_config variable from voc_pcm_path_cal_type
                   data structure.
10/30/03    sm     Added multimode calibration support.
04/18/03    aw     Added audio support for Q3D image.
11/06/02    sm     Include msmaud.h
10/29/02    sg     Added voc_cal_get_adie_config().
09/06/02    sg     Added num_to_delay to voc_adie_codec_config_type.
08/13/02    sg     Added support for AAC.
03/12/02    st     Fixed MSM6050 specific featurization; made val parameter
                   in voc_adie_codec_config_type a const pointer; added
                   codec_rx_gain parameter to voc_pcm_path_cal_type.
11/19/01    aw     Added MSMAUD_ADIE_CODEC_CONFIG.
09/04/01    aw     Added FEATURE_QSYNTH_ADRC. Added QSynth PCM filter and ADRC
                   data structure in PCM path data sturcture.
06/18/01  jkl/ym   Added FEATURE_ACP.
01/26/01    st     Modified VOC_CAL_DB0_QSYNTH_GAIN to 0x100; added
                   VOC_CAL_DB0_QTUNES_GAIN.
12/04/00    st     Added QTUNES support.
09/21/00    sm     Added voc_cal_get_codec_config()
07/14/00   spf     Removed voc_tty_cal_type and voc_cal_get_tty_cal().
05/16/00    st     Added VOC_CAL_DB0_QSYNTH_GAIN define.
05/12/00   spf     Add TTY support: voc_tty_cal_type and 
                   voc_cal_get_tty_cal().
03/08/00   spf     Added DFM configuration support.
01/25/00    ro     Remove voc_cal_ec_type and voc_cal_ns_type.
                   Use voc_ec_type and voc_ns_type.
11/22/99   jkl     Added new AGC /AVC parameters values.
11/11/99   jkl     Removed VOC_CAL_DTMF_TX_GAIN_NOM.
10/18/99    st     Removed agc_config parameter from voc_pcm_path_cal_type.
10/04/99   jkl     Defined Unity gain values for RX, DTMF and CODEC gains.
09/10/99   jkl     Included CODEC TX and ST gains.
08/26/99   spf     Pink Panther Rev2 support.
06/15/99    ro     First version.

===========================================================================*/

#include "comdef.h"             /* Definitions for byte, word, etc.          */
#include "msmaud.h"             /* MSMAudio features                         */
#include "voc.h"                /* Vocoder interface definition              */
#include "qdspcmd.h"            /* QDSP2 Driver Command Layer definitions    */
#ifdef MSMAUD_ADIE_CODEC
#include "adie.h"               /* ADIE Codec config definitions             */
#endif /* MSMAUD_ADIE_CODEC */
/* <EJECT> */

/* ===========================================================================
**
**                           D E F I N I T I O N S
**  
** =========================================================================*/

/* Return type for calibration functions
*/
typedef enum {
  VOC_CAL_SUCCESS,
  VOC_CAL_FAILURE
} voc_cal_return_type;

/* Default values
*/
#define VOC_CAL_TX_GAIN_NOM        0x4000
#define VOC_CAL_DB0_RX_GAIN        0x4000
#define VOC_CAL_DB0_DTMF_GAIN      0x4000
#define VOC_CAL_DB0_RX_CODEC_GAIN  0x4000
#ifdef FEATURE_QSYNTH_COMMON
#define VOC_CAL_DB0_QSYNTH_GAIN    0x100
#endif
#ifdef FEATURE_QTUNES_COMMON
#define VOC_CAL_DB0_QTUNES_GAIN    0x2000
#endif

/* To build a version for use in setting volume levels, define this symbol.
** When defined, the volume tables will be placed in RAM where values can
** be modified using the DM.
*/
#ifdef FEATURE_AVS_DYNAMIC_CALIBRATION
#define CAL_MEMORY
#else
#define CAL_MEMORY   const     /* Normally, tables are in ROM  */
#endif

#ifdef FEATURE_ADIE_DYNAMIC_CALIBRATION
#error code not present
#else
#define CONSTANT const
#endif /* FEATURE_ADIE_DYNAMIC_CALIBRATION */

#ifdef MSMAUD_ADIE_CODEC
typedef struct {
  uint8                        size;          /* Size of adie codec config    */
  uint8                        num_to_delay;  /* No. of settings to be delayed*/
  CONSTANT adie_codec_reg_type *val;          /* Adie codec config value      */
} voc_adie_codec_config_type;
#endif /* MSMAUD_ADIE_CODEC */

#ifdef MSMAUD_PMIC_CONFIG
typedef struct {
  boolean                   mic_bias_on;   /* Should mic bias be turned on? */
  boolean                   vreg_ncp_on;   /* Should EPIC Vreg_NCP be turned on? */
}voc_pmic_config_type;
#endif  /* MSMAUD_PMIC_CONFIG */

/* <EJECT> */

/* ===========================================================================
**
**                      D A T A   S T R U C T U R E S
**  
** =========================================================================*/

/* Calibration values for a PCM path
** These values do not change with volume level
*/
typedef struct {
  voc_ec_type                   ec_mode;         /* Echo Cancellation mode    */
  voc_ns_type                   ns_enable;       /* Noise suppressor enable   */
  uint16                        tx_gain;         /* TX Voice Volume           */
  uint16                        dtmf_tx_gain;    /* TX DTMF gain              */
  uint16                        codec_tx_gain;   /* CODEC TX gain             */
  uint16                        codec_rx_gain;   /* CODEC RX gain             */
  uint16                        codec_st_gain;   /* CODEC ST gain             */
#ifdef MSMAUD_PCM_DC_OFFSET
  uint16                        codec_rxL_offset;/* CODEC RX left chan offset */
  uint16                        codec_rxR_offset;/* CODEC RX right chan offset*/
#endif  /* MSMAUD_PCM_DC_OFFSET */
  uint16                        pre_ec_tx_pcm_filter_flag; /*Flag to shift the PCM filter*/
  qdsp_cmd_pcm_filter_type      tx_filter;       /* TX PCM filter coefficients*/
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  qdsp_cmd_pcm_filter_type      tx_secondary_mic_filter;
#endif
  qdsp_cmd_pcm_filter_type      rx_filter;       /* RX PCM filter coefficients*/
#ifdef FEATURE_IIR_FILTER
  qdsp_cmd_pcm_iir_filter_type  tx_iir_filter;   /* TX PCM filter coefficients*/
  qdsp_cmd_pcm_iir_filter_type  rx_iir_filter;   /* RX PCM filter coefficients*/
#ifdef FEATURE_IIR_FILTER_5S
  qdsp_cmd_pcm_iir_filter_type  tx_iir_filter5;  /* TX PCM filter coefficients*/
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  qdsp_cmd_pcm_iir_filter_type  tx_secondary_mic_iir_filter5;
#endif
  qdsp_cmd_pcm_iir_filter_type  rx_iir_filter5;  /* RX PCM filter coefficients*/
#endif
#endif /* FEATURE_IIR_FILTER */
  sint15                        rx_dbm_offset;   /* RX offset in dBm          */
#ifdef FEATURE_AUDIO_AGC
  qdsp_cmd_agc_param_type       agc_param;       /* AGC/AVC parameters        */
#endif
    
  const qdsp_cmd_rve_param_type        **rve_param;     /* RVE Single Mode parameters   */
  const qdsp_cmd_dual_rve_param_type   **dual_rve_param;  /* Dual Mode RVE parameters    */

  const qdsp_cmd_ec_params_type **ec_params;
#ifdef FEATURE_AVS_NEXTGEN_EC
  const qdsp_cmd_nextgen_ec_parms_type   **nextgen_ec_parms;  /* New EC parms */
  const qdsp_cmd_nextgen_ec_parms_type   **nextgen_ec_wb_parms; /* wideband */
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  const qdsp_cmd_crystal_speech_parms_type **cs_parms; /* CrystalSpeech parms */
  const qdsp_cmd_crystal_speech_parms_type **cs_wb_parms; /* wideband */
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */
#ifdef FEATURE_QSYNTH_ADRC
  const qdsp_cmd_audfmt_adrc_ctl_type
                               **qsynth_adrc_ctl;  /* QSYNTH ADRC control      */
#endif /* FEATURE_QSYNTH_ADRC */
#ifdef FEATURE_QTUNES_ADRC
  const qdsp_cmd_audfmt_adrc_ctl_type
                               **qtunes_adrc_ctl; /* QSYNTH ADRC control      */
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDFMT_STF
  const qdsp_cmd_audfmt_sidechain_filter_type **audfmt_sidechain_filter;
                                     /* Audio format sidechain tunning filter */
#endif /* FEATURE_AUDFMT_STF */
#ifdef FEATURE_AUDFMT_IIR_FILTER
  const qdsp_cmd_audfmt_iir_filter_type       **audfmt_iir_filter;
                                     /* Audio format IIR filter               */
#endif /* FEATURE_AUDFMT_IIR_FILTER */
#ifdef FEATURE_QAUDIOFX
  qdsp_cmd_3d_pos_output_mode_enum_type  output_3d_mode;
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDFMT_FIR_FILTER
#ifdef FEATURE_QSYNTH_COMMON
const qdsp_cmd_audfmt_pcm_filter_type
                               *qsynth_filter;    /* QSYNTH PCM filter config */
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
const qdsp_cmd_audfmt_pcm_filter_type
                               **qtunes_filter;   /* Qtunes PCM filter        */
#endif /* FEATURE_QTUNES_COMMON */
#endif /* FEATURE_AUDFMT_FIR_FILTER */
  voc_hssd_type                 hssd;
} voc_pcm_path_cal_type;

/* ===========================================================================
**
**                 F U N C T I O N   D E F I N I T I O N S
**  
** =========================================================================*/
/*===========================================================================

FUNCTION voc_cal_get_codec_class

DESCRIPTION
  Returns the codec type given a codec id.
  
DEPENDENCIES
  None.

RETURN VALUE
  voc_codec_class_type = The classification of codec referred to by the
                         function argument.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_codec_class_type voc_cal_get_codec_class (
  voc_codec_type     pcm_path
);

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
extern voc_codec_vr_type voc_cal_get_codec_vr_type (
  voc_codec_type     pcm_path
);
#endif /* FEATURE_PHONE_VR */

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
extern voc_cal_return_type voc_cal_get_pcm_cal (
  voc_codec_type        pcm_path,
  qdsp_image_type       image,
  voc_pcm_path_cal_type **pcm_cal_ptr
);

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
voc_cal_return_type voc_cal_get_codec_config (
 voc_codec_type        pcm_path,
 qdsp_image_type       image,
 qdsp_cmd_codec_config_type **codec_config_ptr
);

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
extern voc_cal_return_type voc_cal_get_adie_config (
 voc_codec_type             pcm_path,
 qdsp_image_type            image,
 voc_adie_codec_config_type **adie_config_ptr
);
#endif /* MSMAUD_ADIE_CODEC */

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
extern voc_cal_return_type voc_cal_get_pmic_config (
 voc_codec_type             pcm_path,
 qdsp_image_type            image,
 voc_pmic_config_type       **pmic_config_ptr
);
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
extern voc_cal_return_type voc_cal_get_mic_gain_adjust 
(
  voc_codec_type      pcm_path,
  sint15              **gain_cal_ptr
);
  
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
extern voc_cal_return_type voc_cal_set_mic_gain_adjust 
(
  voc_codec_type        pcm_path,
  sint15                gain_cal
);
#endif /* FEATURE_VOC_MIC_GAIN_ADJUST */

#ifdef FEATURE_VOC_NV_CAL
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
  voc_data_mic_gain_adjust (indirectly)

===========================================================================*/
extern void voc_cal_read_nv_cal (void);
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
);

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
qdsp_cmd_slowtalk_parms_type* voc_cal_get_slowtalk_params(void);


#endif /* VOCCAL_H */


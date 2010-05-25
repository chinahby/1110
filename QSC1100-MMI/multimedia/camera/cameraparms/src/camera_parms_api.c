

/*==========================================================================
                 Camera Services Interface for QMobiCat 

   DESCRIPTION
     This file contains definitions for API to monitor and update 
     camera prameters.

   Design Note:

   Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameraparms/main/latest/src/camera_parms_api.c#7 $ $DateTime: 2008/10/13 22:47:53 $ $Author: shilpam $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 29/02/08   sm      merge of changes
 12/06/07  nshree  Fixed the compilation issue.
 11/21/07  cg      Added support for Chromatix header 0x201. 
 11/08/07  hap     Fixing RVCT2.2 compiler warnings
 05/24/07  klee    Added more APIs based on AEC outdoor enhancement
                   CAMERA_API_AEC_OUTDOOR_BRIGHT_ENABLE
                   CAMERA_API_AEC_OUTDOOR_BRIGHT_REDUCTION
                   CAMERA_API_AEC_OUTDOOR_BRIGHT_THRESHOLD_LO
                   CAMERA_API_AEC_OUTDOOR_BRIGHT_THRESHOLD_HI
                   CAMERA_API_AEC_OUTDOOR_BRIGHT_DISCARDED - Read only
 05/10/07  klee    Added moer APIs based on system team's request
                   CAMERA_API_AWB_SNAPSHOT_R_GAIN - Read only
                   CAMERA_API_AWB_SNAPSHOT_B_GAIN - Read only
 03/30/07  klee    Clean-up lint errors due to un-matched data type in 3a
 03/14/07  klee    Removed unnecessary conditional statement which is always TRUE
 03/12/07  klee    Modified by 3A-restructuring
 02/20/07  klee    Fixed all lint errors (critical, high, medium, low and lint)
 02/16/07  klee    Updated APIs based on system's request
 02/15/07  klee    Added more APIs
                   AWB_PREV_R_GAIN - Read only
                   AWB_PREV_B_GAIN - Read only
                   AFR_PREVIEW_FPS - Read only
                   AEC_HIGH_LUMA_REGION_THRESHOLD - Read/Write
 12/19/06  klee    Added fix on CR 108540 - compiling error under FEATURE_CAMERA_YCBCR_ONLY
 11/20/06  jv      Removed static saturated color conversion matrix.
 10/20/06  jv      Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 09/21/06  jn      Added AF related params.
 09/15/06  klee    Fixed crash problem with QMobiCat engine
 09/15/06  klee    Cleaned up the lint errors
 09/15/06  klee    Added implementation for AWB, AEC and ASF
 08/28/06  klee    Added implementation for the variables on 3a live screen
 08/26/05  klee    Initial Relase.

============================================================================*/
/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_CAMERA
#ifdef FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
#include "camera_parms_api.h"
#include "camctrl.h"
#include <stdio.h>
#include <string.h>

/*============================================================================
                        CONSTANTS
============================================================================*/

/*============================================================================
                        INTERNAL ABSTRACT DATA TYPES
============================================================================*/

/*============================================================================
                        INTERNAL API DECLARATIONS
============================================================================*/

/*============================================================================
                        EXTERNAL VARIABLES DECLARATIONS
============================================================================*/
extern int8                          camera_asi;
extern camsensor_static_params_type  camsensor_static_params [];
extern uint32                        vfe_gamma_table_size;
extern boolean camera_update_vfe;
extern camera_vfe_configuration_control_and_status_type camera_vfe_control_and_status;
extern VFE_UpdateType vfeUpdate;
/*============================================================================
                        INTERNAL VARIABLES DEFINITIONS
============================================================================*/
static uint8  ui8min;
static uint8  ui8max; 
static int8   int8min;
static int8   int8max;
static uint32 ui32min;
#ifndef FEATURE_CAMERA_YCBCR_ONLY
static uint32 ui32max; 
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
static int32  int32min;
static int32  int32max;
#ifndef FEATURE_CAMERA_YCBCR_ONLY
static boolean need_awb_reconfig;
static boolean need_aec_reconfig;
static boolean need_gamma_reconfig;
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
static boolean need_af_reconfig;
static boolean need_vfe_update;
static boolean need_vfe_reconfig;
static camsensor_static_params_type * camsensor_ptr;
static camsensor_chromatix_output_type * chromatix_ptr;
/*============================================================================
                        EXTERNAL API DEFINITIONS
============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_PARMS_INIT

DESCRIPTION
  to initialize API.

CALLER
  camera_tune_init().

RETURN VALUE

===========================================================================*/
camera_ret_code_type camera_parms_init (void)
{
  ui8min = 0;
  ui8max = 0; 
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  need_awb_reconfig = FALSE;
  need_aec_reconfig = FALSE;
  need_gamma_reconfig = FALSE;
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
  need_vfe_update = FALSE;
  need_vfe_reconfig = FALSE;
  camsensor_ptr = &camsensor_static_params[camera_asi];
  chromatix_ptr = camsensor_static_params[camera_asi].chromatix_parms;

  if(camsensor_ptr == NULL || chromatix_ptr == NULL)
    return CAMERA_FAILED;
 
  return CAMERA_SUCCESS;
}

/*===========================================================================

FUNCTION      CAMERA_PARMS_READ

DESCRIPTION
  This API reads a parameter value and format.
  phase [in]      : Phase when parameter is being read.
  parm  [in]      : Enumerated value specifying the parameter to be read.
  table_index [in]: If parameter is a table
                    if >= 0, only the corresponding element of the table is read.
                    if -1 the whole table is read.
  value [out]     : Pointer to (type)value. Camera services allocates and frees this, if necessary.
  format [out]    : Structure specifying parameter format.

RETURN VALUE

===========================================================================*/
camera_ret_code_type camera_parms_read (
  camera_tune_phase_type        phase, 
  camera_api_parms_type         parm,
  /* If table_index is >= 0, 
   * only the corresponding member of the table is affected.  
   * If table_index is -1, the API operates on the entire table.
   */
  int32                         table_index, 
  void                          **value,
  camera_tune_parm_format_type  *format
)
{
/*lint -save -e685 */
  if ((parm > CAMERA_API_LAST) ||
      (format == NULL) ||
      (value == NULL) ||
      (phase > CAMERA_TUNE_PHASE_RUN))
  {
    return CAMERA_INVALID_PARM;
  }
/*lint -restore */
  memset(format, 0, sizeof(camera_tune_parm_format_type));
  format->num_elements = 1;

  switch (parm)
  {
#ifndef FEATURE_CAMERA_YCBCR_ONLY
    /*************** MWB parameters *****************/
    case CAMERA_API_MWB_FLUORESCENT_R_GAIN:
      format->name = "MWB-Fluorescent-Rgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_tl84_white_balance.r_gain;
      break;
    case CAMERA_API_MWB_FLUORESCENT_G_GAIN:
      format->name = "MWB-Fluorescent-Ggain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_tl84_white_balance.g_gain;
      break;
    case CAMERA_API_MWB_FLUORESCENT_B_GAIN:
      format->name = "MWB-Fluorescent-Bgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_tl84_white_balance.b_gain;
      break;
    case CAMERA_API_MWB_SUNNY_R_GAIN:
      format->name = "MWB-Sunny-Rgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_d50_white_balance.r_gain;
      break;
    case CAMERA_API_MWB_SUNNY_G_GAIN:
      format->name = "MWB-Sunny-Ggain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_d50_white_balance.g_gain;
      break;
    case CAMERA_API_MWB_SUNNY_B_GAIN:
      format->name = "MWB-Sunny-Bgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_d50_white_balance.b_gain;
      break;
    case CAMERA_API_MWB_TUNGSTEN_R_GAIN:
      format->name = "MWB-Tungsten-Rgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_incandescent_white_balance.r_gain;
      break;
    case CAMERA_API_MWB_TUNGSTEN_G_GAIN:
      format->name = "MWB-Tungsten-Ggain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_incandescent_white_balance.g_gain;
      break;
    case CAMERA_API_MWB_TUNGSTEN_B_GAIN:
      format->name = "MWB-Tungsten-Bgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_incandescent_white_balance.b_gain;
      break;
    case CAMERA_API_MWB_CLOUDY_R_GAIN:
      format->name = "MWB-Cloudy-Rgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_d65_white_balance.r_gain;
      break;
    case CAMERA_API_MWB_CLOUDY_G_GAIN:
      format->name = "MWB-Cloudy-Ggain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_d65_white_balance.g_gain;
      break;
    case CAMERA_API_MWB_CLOUDY_B_GAIN:
      format->name = "MWB-Cloudy-Bgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_d65_white_balance.b_gain;
      break;
    case CAMERA_API_MWB_STROBE_R_GAIN:
      format->name = "MWB-Strobe-Rgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->strobe_flash_white_balance.r_gain;
      break;
    case CAMERA_API_MWB_STROBE_G_GAIN:
      format->name = "MWB-Strobe-Ggain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->strobe_flash_white_balance.g_gain;
      break;
    case CAMERA_API_MWB_STROBE_B_GAIN:
      format->name = "MWB-Strobe-Bgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->strobe_flash_white_balance.b_gain;
      break;
    case CAMERA_API_MWB_LED_R_GAIN:
      format->name = "MWB-LED-Rgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->led_flash_white_balance.r_gain;
      break;
    case CAMERA_API_MWB_LED_G_GAIN:
      format->name = "MWB-LED-Ggain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->led_flash_white_balance.g_gain;
      break;
    case CAMERA_API_MWB_LED_B_GAIN:
      format->name = "MWB-LED-Bgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->led_flash_white_balance.b_gain;
      break;

    /*************** AWB parameters *****************/
    case CAMERA_API_AWB_ENABLE:
      format->name = "AWB_Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camsensor_ptr->awb_enable;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;
#if 0 //TODO: Updated values to be reflected in services
    case CAMERA_API_AWB_CHANNEL_GAIN_GREEN_EVEN_ROW:
      format->name = "AWB-ChannelGain-GreenEvenRow";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->chromatix_channel_balance_gains.green_even;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_CHANNEL_GAIN_GREEN_ODD_ROW:
      format->name = "AWB-ChannelGain-GreenOddRow";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->chromatix_channel_balance_gains.green_odd;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_CHANNEL_GAIN_RED:
      format->name = "AWB-ChannelGain-Red";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->chromatix_channel_balance_gains.red;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_CHANNEL_GAIN_BLUE:
      format->name = "AWB-ChannelGain-Blue";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->chromatix_channel_balance_gains.blue;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_CHROMATIX);
      break;
#endif
    case CAMERA_API_AWB_REFERENCE_POINT_RED_D65:
      format->name = "AWB-RefPointsD65-RedG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[0].red_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_D65:   
      format->name = "AWB-RefPointsD65-BlueG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[0].blue_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_D65:   
      format->name = "AWB-RefPointsD65-RedAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[0].red_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                       (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_D65:   
      format->name = "AWB-RefPointsD65-BlueAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[0].blue_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_D75:
      format->name = "AWB-RefPointsD75-RedG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[1].red_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_D75:   
      format->name = "AWB-RefPointsD75-BlueG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[1].blue_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_D75:   
      format->name = "AWB-RefPointsD75-RedAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[1].red_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_D75:   
      format->name = "AWB-RefPointsD75-BlueAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[1].blue_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_A:
      format->name = "AWB-RefPointsA-RedG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[2].red_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_A:     
      format->name = "AWB-RefPointsA-BlueG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[2].blue_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_A:     
      format->name = "AWB-RefPointsA-RedAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[2].red_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_A:     
      format->name = "AWB-RefPointsA-BlueAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[2].blue_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_TL84:
      format->name = "AWB-RefPointsTL84-RedG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[3].red_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_TL84:  
      format->name = "AWB-RefPointsTL84-BlueG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[3].blue_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_TL84:  
      format->name = "AWB-RefPointsTL84-RedAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[3].red_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_TL84:  
      format->name = "AWB-RefPointsTL84-BlueAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[3].blue_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_CW:
      format->name = "AWB-RefPointsCW-RedG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[4].red_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_CW:    
      format->name = "AWB-RefPointsCW-BlueG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[4].blue_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_CW:    
      format->name = "AWB-RefPointsCW-RedAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[4].red_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_CW:    
      format->name = "AWB-RefPointsCW-BlueAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[4].blue_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_H:
      format->name = "AWB-RefPointsH-RedG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[5].red_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_H:     
      format->name = "AWB-RefPointsH-BlueG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[5].blue_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_H:     
      format->name = "AWB-RefPointsH-RedAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[5].red_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_H:     
      format->name = "AWB-RefPointsH-BlueAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[5].blue_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_D50:
      format->name = "AWB-RefPointsD50-RedG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[6].red_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_D50:   
      format->name = "AWB-RefPointsD50-BlueG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[6].blue_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_D50:   
      format->name = "AWB-RefPointsD50-RedAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[6].red_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_D50:   
      format->name = "AWB-RefPointsD50-BlueAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[6].blue_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_CUST_F:
      format->name = "AWB-RefPointsCustFluo-RedG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[7].red_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_CUST_F:
      format->name = "AWB-RefPointsCustFluo-BlueG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[7].blue_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_CUST_F:
      format->name = "AWB-RefPointsCustFluo-RedAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[7].red_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_CUST_F:
      format->name = "AWB-RefPointsCustFluo-BlueAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[7].blue_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_NOON:
      format->name = "AWB-RefPointsNoon-RedG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[8].red_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_NOON:
      format->name = "AWB-RefPointsNoon-BlueG";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[8].blue_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_NOON:
      format->name = "AWB-RefPointsNoon-RedAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[8].red_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                      CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_NOON:
      format->name = "AWB-RefPointsNoon-BlueAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_reference_hw_rolloff[8].blue_gain_adj;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY | 
                      CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_LOW_LIGHT_VFE_MIN_Y:
      format->name = "AWB-StatConfigLowLight-LumaMin";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].y_min;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_MAX_Y:
      format->name = "AWB-StatConfigLowLight-LumaMax";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].y_max;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_M1:
      format->name = "AWB-StatConfigLowLight-Slope1";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].m1;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_M2:
      format->name = "AWB-StatConfigLowLight-Slope2";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].m2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_M3:
      format->name = "AWB-StatConfigLowLight-Slope3";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].m3;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_M4:
      format->name = "AWB-StatConfigLowLight-Slope4";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].m4;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_C1:
      format->name = "AWB-StatConfigLowLight-CbOffset1";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].c1;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_C2:
      format->name = "AWB-StatConfigLowLight-CrOffset2";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].c2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_C3:
      format->name = "AWB-StatConfigLowLight-CbOffset3";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].c3;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_C4:
      format->name = "AWB-StatConfigLowLight-CrOffset4";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].c4;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_MIN_Y:
      format->name = "AWB-StatConfigNormalLight-LumaMin";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].y_min;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_MAX_Y:
      format->name = "AWB-StatConfigNormalLight-LumaMax";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].y_max;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_M1:
      format->name = "AWB-StatConfigNormalLight-Slope1";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].m1;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_M2:
      format->name = "AWB-StatConfigNormalLight-Slope2";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].m2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_M3:
      format->name = "AWB-StatConfigNormalLight-Slope3";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].m3;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_M4:
      format->name = "AWB-StatConfigNormalLight-Slope4";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].m4;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_C1:
      format->name = "AWB-StatConfigNormalLight-CbOffset1";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].c1;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_C2:
      format->name = "AWB-StatConfigNormalLight-CrOffset2";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].c2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_C3:
      format->name = "AWB-StatConfigNormalLight-CbOffset3";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].c3;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_C4:
      format->name = "AWB-StatConfigNormalLight-CrOffset4";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].c4;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_MIN_Y:
      format->name = "AWB-StatConfigOutdoor-LumaMin";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].y_min;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_MAX_Y:
      format->name = "AWB-StatConfigOutdoor-LumaMax";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].y_max;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_M1:
      format->name = "AWB-StatConfigOutdoor-Slope1";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].m1;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_M2:
      format->name = "AWB-StatConfigOutdoor-Slope2";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].m2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_M3:
      format->name = "AWB-StatConfigOutdoor-Slope3";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].m3;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_M4:
      format->name = "AWB-StatConfigOutdoor-Slope4";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].m4;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_C1:
      format->name = "AWB-StatConfigOutdoor-CbOffset1";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].c1;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_C2:
      format->name = "AWB-StatConfigOutdoor-CrOffset2";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].c2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_C3:
      format->name = "AWB-StatConfigOutdoor-CbOffset3";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].c3;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_C4:
      format->name = "AWB-StatConfigOutdoor-CrOffset4";
      format->parm_type = JPEGENC_MC_INT16;
      *value = &chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].c4;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_INDOOR_INDEX:
      format->name = "AWB-IndoorIndex";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_indoor_index;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_OUTDOOR_INDEX:
      format->name = "AWB-OutdoorIndex";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_outdoor_index;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_SNOW_BLUE_ADJ_RATIO:
      format->name = "AWB-SnowBlueGainAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->snow_blue_gain_adj_ratio;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_BEACH_BLUE_ADJ_RATIO:
      format->name = "AWB-BeachBlueGainAdj";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->beach_blue_gain_adj_ratio;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_OUTLIER_DISTANCE:
      format->name = "AWB-OutlierDist";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->outlier_distance;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_GREEN_OFFSET_RG:
      format->name = "AWB-GreenOffRG";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->green_offset_rg;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_GREEN_OFFSET_BG:
      format->name = "AWB-GreenOffBG";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->green_offset_bg;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_CURRENT_SAMPLE_DECISION:
      format->name = "AWB-SampleDecision";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &cam3a_awb_state.agw_sample_decision;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AWB_PREV_R_GAIN:
      format->name = "AWB-PrevRGain";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &cam3a_awb_state.prev_wb.r_gain;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AWB_PREV_B_GAIN:
      format->name = "AWB-PrevBGain";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &cam3a_awb_state.prev_wb.b_gain;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AWB_SNAPSHOT_R_GAIN:
      format->name = "AWB-SnapshotRGain";
      format->parm_type = JPEGENC_MC_INT32;
      int32max = (int)((cam3a_awb_state.prev_wb.r_gain*cam3a_awb_state.prev_r_gain_adj) >> 7);
      *value = &int32max;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AWB_SNAPSHOT_B_GAIN:
      format->name = "AWB-SnapshotBGain";
      format->parm_type = JPEGENC_MC_INT32;
      int32max = (int)((cam3a_awb_state.prev_wb.b_gain*cam3a_awb_state.prev_b_gain_adj) >> 7);
      *value = &int32max;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    /*************** AWB Extended parameters *****************/
    case CAMERA_API_AWB_EXT_COMPACT_CLUSTER_R2:
      format->name = "AWBExt-CClusterDistSq";
      format->parm_type = JPEGENC_MC_UINT16;
      *value = &chromatix_ptr->compact_cluster_r2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_C_CLUSTER_TO_REF_SQ:
      format->name = "AWBExt-CClusterToRefSq";
      format->parm_type = JPEGENC_MC_UINT16;
      *value = &chromatix_ptr->compact_cluster_to_ref_point_r2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_A_CLUSTER_THRESH:
      format->name = "AWBExt-AClusterThresh";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->a_cluster_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_F_CLUSTER_THRESH:
      format->name = "AWBExt-FClusterThresh";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->f_cluster_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_DAY_CLUSTER_THRESH:
      format->name = "AWBExt-DayClusterThresh";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->day_cluster_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_OUTDOOR_G_THRESH:
      format->name = "AWBExt-OutdoorGThresh";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->outdoor_green_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_OUTDOOR_G_THR_BRT_F:
      format->name = "AWBExt-OutdoorGThrBrtF";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->outdoor_green_threshold_bright_F;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_OUTDOOR_G_THR_DARK_F:
      format->name = "AWBExt-AWBExt-OutdoorGThrDarkF";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->outdoor_green_threshold_dark_F;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_DAY_CLUSTER_THR_F:
      format->name = "AWBExt-DayClusterThrF";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->day_cluster_threshold_for_F;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WB_ALOW_F_LINE:
      format->name = "AWBExt-WBAllowFLine";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->white_balance_allow_fline;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_OUT_VALID_SAMPLES_THR:
      format->name = "AWBExt-OutValidSamplesThr";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->outdoor_valid_sample_cnt_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_OUT_GREEN_UP_THRESH:
      format->name = "AWBExt-OutGreenUpThresh";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->outdoor_green_upper_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_RADIUS_SQUARED_THR:
      format->name = "AWBExt-RadiusSquaredThr";
      format->parm_type = JPEGENC_MC_UINT16;
      *value = &chromatix_ptr->r2_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_OUT_G_THRESH_BRIGHT_A:
      format->name = "AWBExt-OutGThreshBrightA";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->outdoor_green_threshold_bright_A;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_OUT_G_THRESH_DARK_A:
      format->name = "AWBExt-OutGThreshDarkA";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->outdoor_green_threshold_dark_A;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_DAY_CLUSTER_THR_A:
      format->name = "AWBExt-DayClusterThrA";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->day_cluster_threshold_for_A;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_EXT_CC_GLOBAL_GAIN:
      format->name = "AWBExt-CCGlobalGain";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->color_correction_global_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_EXT_MIN_R_GAIN:
      format->name = "AWBExt-MinGains-Rgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_min_gains.r_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_MIN_G_GAIN:
      format->name = "AWBExt-MinGains-Ggain";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_min_gains.g_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_MIN_B_GAIN:
      format->name = "AWBExt-MinGains-Bgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_min_gains.b_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_MAX_R_GAIN:
      format->name = "AWBExt-MaxGains-Rgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_max_gains.r_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_MAX_G_GAIN:
      format->name = "AWBExt-MaxGains-Ggain";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_max_gains.g_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_MAX_B_GAIN:
      format->name = "AWBExt-MaxGains-Bgain";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->awb_max_gains.b_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_EXT_OUTDOOR_SAMPLE_INFLUENCE:
      format->name = "AWBExt-SampleInfluence-Outdoor";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->sample_influence.outdoor_influence;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_INDOOR_SAMPLE_INFLUENCE:
      format->name = "AWBExt-SampleInfluence-Indoor";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->sample_influence.indoor_influence;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_D65:
      format->name = "AWBExt-WeightVectorD65-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_D65:
      format->name = "AWBExt-WeightVectorD65-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_D65:
      format->name = "AWBExt-WeightVectorD65-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_D75:
      format->name = "AWBExt-WeightVectorD75-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_CLOUDY].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_D75:
      format->name = "AWBExt-WeightVectorD75-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_CLOUDY].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_D75:
      format->name = "AWBExt-WeightVectorD75-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_CLOUDY].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_A:
      format->name = "AWBExt-WeightVectorA-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_INCANDESCENT].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_A:
      format->name = "AWBExt-WeightVectorA-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_INCANDESCENT].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_A:
      format->name = "AWBExt-WeightVectorA-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_INCANDESCENT].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_TL84:
      format->name = "AWBExt-WeightVectorTL84-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_WARM_FLO].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_TL84:
      format->name = "AWBExt-WeightVectorTL84-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_WARM_FLO].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_TL84:
      format->name = "AWBExt-WeightVectorTL84-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_WARM_FLO].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_CW:
      format->name = "AWBExt-WeightVectorCW-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_COLD_FLO].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_CW:
      format->name = "AWBExt-WeightVectorCW-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_COLD_FLO].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_CW:
      format->name = "AWBExt-WeightVectorCW-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_COLD_FLO].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_H:
      format->name = "AWBExt-WeightVectorH-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_HORIZON].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_H:
      format->name = "AWBExt-WeightVectorH-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_HORIZON].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_H:
      format->name = "AWBExt-WeightVectorH-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_HORIZON].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_D50:
      format->name = "AWBExt-WeightVectorD50-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT1].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_D50:
      format->name = "AWBExt-WeightVectorD50-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT1].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_D50:
      format->name = "AWBExt-WeightVectorD50-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT1].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_CUSTFLUO:
      format->name = "AWBExt-WeightVectorCustFluo-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_CUSTOM_FLO].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_CUSTFLUO:
      format->name = "AWBExt-WeightVectorCustFluo-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_CUSTOM_FLO].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_CUSTFLUO:
      format->name = "AWBExt-WeightVectorCustFluo-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_CUSTOM_FLO].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_NOON:
      format->name = "AWBExt-WeightVectorNoon-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_NOON].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_NOON:
      format->name = "AWBExt-WeightVectorNoon-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_NOON].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_NOON:
      format->name = "AWBExt-WeightVectorNoon-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_NOON].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_DAYLIGHT:
      format->name = "AWBExt-WeightVectorDaylight-Indoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_HYBRID].indoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_DAYLIGHT:
      format->name = "AWBExt-WeightVectorDaylight-Outdoor";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_HYBRID].outdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_DAYLIGHT:
      format->name = "AWBExt-WeightVectorDaylight-InOut";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_weight_vector[AGW_AWB_HYBRID].inoutdoor_weight;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AWB_EXT_WHITE_WORLD_Y_MIN_RATIO:
      format->name = "AWBExt-WhiteWorldYMinRatio";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->awb_white_world_y_min_ratio;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    /*************** AEC parameters *****************/
    case CAMERA_API_AEC_ENABLE:
      format->name = "AEC-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camsensor_ptr->aec_enable;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;

    case CAMERA_API_AEC_AGGRESSIVENESS:
      format->name = "AEC-Aggressiveness";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &camctrl_tbl.aec_aggressiveness;
      ui32min = 0;
      format->min = &ui32min;
      ui32max = 2;
      format->max = &ui32max;
      break;

    case CAMERA_API_AEC_HIGH_LUMA_REGION_THRESHOLD:
      format->name = "AEC-HighLumaRegionThresh";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &camctrl_tbl.high_luma_region_threshold;
      break;

    case CAMERA_API_AEC_CURRENT_SENSOR_LUMA:
      format->name = "AEC_SENSOR_LUMA";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &cam3a_aec_state.current_luma;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AEC_CURRENT_VFE_LUMA:
      format->name = "AEC-VFELuma";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &cam3a_aec_state.awb_compensated_luma;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AEC_CURRENT_EXP_INDEX:
      format->name = "AEC-ExpIndex";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &cam3a_aec_state.prev_exp_index;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AEC_OUTDOOR_BRIGHT_ENABLE:
      format->name = "AEC-Outdoor-BrightEnable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camctrl_tbl.aec_outdoor_bright_region_is_supported;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;

    case CAMERA_API_AEC_OUTDOOR_BRIGHT_REDUCTION:
      format->name = "AEC-Outdoor-BrightReduction";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &camctrl_tbl.aec_outdoor_bright_region_reduction;
      break;

    case CAMERA_API_AEC_OUTDOOR_BRIGHT_THRESHOLD_LO:
      format->name = "AEC-Outdoor-BrightThreshLo";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &camctrl_tbl.aec_outdoor_bright_region_threshold_LO;
      break;

    case CAMERA_API_AEC_OUTDOOR_BRIGHT_THRESHOLD_HI:
      format->name = "AEC-Outdoor-BrightThreshHi";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &camctrl_tbl.aec_outdoor_bright_region_threshold_HI;
      break;

    case CAMERA_API_AEC_OUTDOOR_BRIGHT_DISCARDED:
      format->name = "AEC-Outdoor-BrightDiscarded";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &cam3a_aec_state.outdoor_bright_region_discarded_cnt;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;
#if 0 //TODO: Updated values to be reflected in services
    case CAMERA_API_AEC_EXPOSURE_GAIN_TYPE:
      format->name = "AEC-GainType";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->exposure_gain_type;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_CHROMATIX);
      break;
#endif
    case CAMERA_API_AEC_NUM_EXPOSURE_VALUES:
      format->name = "AEC-NumValues";
      format->parm_type = JPEGENC_MC_UINT16;
      *value = &chromatix_ptr->num_exposure_values;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AEC_TABLE_EXPOSURE_ENTRIES_GAIN:
      // This is a member of a structure in a 500 item array
      // This API can handle structures, and it can handle large arrays,
      // but it cannot handle large arrays of structures.
      // Instead, convert from indexed structure: exposure_table[index].gain
      // to indexed element: gain[index]
      format->name = "AEC-ExposureEntries-Gain";
      format->parm_type = JPEGENC_MC_UINT16;
      format->num_elements = MAX_EXPOSURE_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      if ((table_index >= 0) &&    // We can only deal with a single element, not the whole table
          (table_index < MAX_EXPOSURE_TABLE_SIZE))
      {
        *value = &chromatix_ptr->exposure_table[table_index].gain;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;

    case CAMERA_API_AEC_TABLE_EXPOSURE_ENTRIES_LINE_CT:
      // This is a member of a structure in a 500 item array
      // This API can handle structures, and it can handle large arrays,
      // but it cannot handle large arrays of structures.
      // Instead, convert from indexed structure: exposure_table[index].line_count
      // to indexed element: line_count[index]
      format->name = "AEC-ExposureEntries-LineCount";
      format->parm_type = JPEGENC_MC_UINT32;
      format->num_elements = MAX_EXPOSURE_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      if ((table_index >= 0) &&    // We can only deal with a single element, not the whole table
          (table_index < MAX_EXPOSURE_TABLE_SIZE))
      {
        *value = &chromatix_ptr->exposure_table[table_index].line_count;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;

    case CAMERA_API_AEC_LUMA_TARGET_DEFAULT:
      format->name = "AEC-Default";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->default_luma_target;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AEC_LUMA_TARGET_OUTDOOR:
      format->name = "AEC-Outdoor";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->outdoor_luma_target;
      format->flags = CAMERA_PARMS_API_REQUIRES_APPLY;
      break;

    case CAMERA_API_AEC_LUMA_TARGET_LOW_LIGHT:
      format->name = "AEC-LowLight";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->lowlight_luma_target;
      format->flags = CAMERA_PARMS_API_REQUIRES_APPLY;
      break;

    case CAMERA_API_AEC_LUMA_TARGET_BACK_LIGHT:
      format->name = "AEC-BacklightLuma";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->backlight_luma_target;
      format->flags = CAMERA_PARMS_API_REQUIRES_APPLY;
      break;

    case CAMERA_API_AEC_TOLERANCE:
      format->name = "AEC-LumaTolerance";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &chromatix_ptr->luma_tolerance;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    case CAMERA_API_AEC_EXP_INDEX_ADJ_STEP:
      format->name = "AEC-ExpIndexAdjStep";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->exposure_index_adj_step;
      format->flags = CAMERA_PARMS_API_CHROMATIX;
      break;
    case CAMERA_API_AEC_ISO100_GAIN:
      format->name = "AEC-ISO100Gain";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &chromatix_ptr->iso100_gain;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AEC_INDOOR_INDEX:
      format->name = "AEC-IndoorIndex";
      format->parm_type = JPEGENC_MC_UINT16;
      *value = &chromatix_ptr->aec_indoor_index;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AEC_OUTDOOR_INDEX:
      format->name = "AEC-OutdoorIndex";
      format->parm_type = JPEGENC_MC_UINT16;
      *value = &chromatix_ptr->aec_outdoor_index;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_CHROMATIX);
      break;
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
    /*************** AF parameters ******************/
    case CAMERA_API_AF_ENABLE:
      format->name = "AF-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &cam3a_af_state.is_enabled;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;

    case CAMERA_API_AF_GROSS_STEP:
      format->name = "AF-GrossSteps";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &camsensor_ptr->num_gross_steps_between_stat_points;
      int32min = 1;
      format->min = &int32min;
      int32max = camsensor_ptr->position_far_end;
      format->max = &int32max;
      break;
      
    case CAMERA_API_AF_FINE_STEP:
      format->name = "AF-FineSteps";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &camsensor_ptr->num_fine_steps_between_stat_points;
      int32min = 1;
      format->min = &int32min;
      int32max = camsensor_ptr->position_far_end;
      format->max = &int32max;
      break;

    case CAMERA_API_AF_FINE_SEARCH_POINTS:
      format->name = "AF-FineSearchPoints";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &camsensor_ptr->num_fine_search_points;
      int32min = 1;
      format->min = &int32min;
      int32max = camsensor_ptr->position_far_end;
      format->max = &int32max;
      break;

    case CAMERA_API_AF_PROCESS:
      format->name = "AF-Process";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &camsensor_ptr->af_process_type;
      int8min = (int8)AF_PROCESS_DEFAULT;
      format->min = &int8min;
      int8max = (int8)AF_HILL_CLIMBING_AGGRESSIVE;
      format->max = &int8max;
      format->flags = CAMERA_PARMS_API_REQUIRES_APPLY;
      break;

    case CAMERA_API_AF_MODE:
      format->name = "AF-Mode";
      format->parm_type = JPEGENC_MC_INT8;
      *value = &cam3a_af_state.search_mode;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AF_NEAR_END:
      format->name = "AF-NearEnd";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &camsensor_ptr->position_near_end;
      int32min = 0;
      format->min = &int32min;
      int32max = camsensor_ptr->position_far_end;
      format->max = &int32max;
      format->flags = CAMERA_PARMS_API_REQUIRES_APPLY;
      break;

    case CAMERA_API_AF_BOUNDARY:
      format->name = "AF-Boundary";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &camsensor_ptr->position_boundary;
      int32min = 0;
      format->min = &int32min;
      int32max = camsensor_ptr->position_far_end;
      format->max = &int32max;
      format->flags = CAMERA_PARMS_API_REQUIRES_APPLY;
      break;

    case CAMERA_API_AF_FAR_END:
      format->name = "AF-FarEnd";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &camsensor_ptr->position_far_end;
      int32min = 0;
      format->min = &int32min;
      int32max = camsensor_ptr->num_steps_near_to_closest_infinity;
      format->max = &int32max;
      format->flags = CAMERA_PARMS_API_REQUIRES_APPLY;
      break;

    case CAMERA_API_AF_COLLECT_END_STAT:
      format->name = "AF-CollectEndStat";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &cam3a_af_state.collect_end_stat;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;

    case CAMERA_API_AF_TEST_MODE:
      format->name = "AF-TestMode";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &cam3a_af_state.af_test_mode;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;

    case CAMERA_API_AF_UNDERSHOOT_PROTECT:
      format->name = "AF-UndershootProtect";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camctrl_tbl.undershoot_protect;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;
      
    case CAMERA_API_AF_RESET_LENS_AFTER_SNAP:
      format->name = "AF-ResetLensAfterSnap";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camctrl_tbl.reset_lens_after_snap;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;

    case CAMERA_API_AF_CLIP_TO_AF_RATIO:
      format->name = "AF-ClipToAFRatio";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &camctrl_tbl.clip_to_af_ratio;
      ui32min = 1;
      format->min = &ui32min;
      break;
      
    case CAMERA_API_AF_WINDOW_MAX_Y:
      format->name = "AF-WindowMaxY";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = (uint8*)(&vfeAfStatDspInput) + 8;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AF_WINDOW_MIN_Y:
      format->name = "AF-WindowMinY";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = (uint8*)(&vfeAfStatDspInput) + 9;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AF_POSITION:
      format->name = "AF-Position";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &cam3a_af_state.position;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AF_TRACE_POSITIONS:
      format->name = "AF-TracePositions";
      format->parm_type = JPEGENC_MC_INT32;
      format->num_elements = CAM3A_NUM_AF_COLLECTION_POINTS;
      format->flags        = CAMERA_PARMS_API_READ_ONLY;

      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = cam3a_af_state.tracing_info_positions;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < CAM3A_NUM_AF_COLLECTION_POINTS)
      {
        *value = &cam3a_af_state.tracing_info_positions[table_index];
      }   
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }

      break;

    case CAMERA_API_AF_TRACE_STATS:
      format->name = "AF-TraceStats";
      format->parm_type = JPEGENC_MC_INT32;
      format->num_elements = CAM3A_NUM_AF_COLLECTION_POINTS;
      format->flags        = CAMERA_PARMS_API_READ_ONLY;

      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = cam3a_af_state.tracing_info_stats;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < CAM3A_NUM_AF_COLLECTION_POINTS)
      {
        *value = &cam3a_af_state.tracing_info_stats[table_index];
      }     
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;

    case CAMERA_API_AF_TRACE_FOCUS_TIME:
      format->name = "AF-FocusTime";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &cam3a_af_state.tracing_info_focusing_time;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AF_WINDOW_LAST_LINE:
      format->name = "AF-WindowLastLine";
      format->parm_type = JPEGENC_MC_UINT16;
/*lint -save -e740*/
      *value = (uint16*)(&vfeAfStatDspInput) + 2;
/*lint -restore*/
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AF_WINDOW_1ST_LINE:
      format->name = "AF-Window1stLine";
      format->parm_type = JPEGENC_MC_UINT16;
/*lint -save -e740*/
      *value = (uint16*)(&vfeAfStatDspInput) + 3;
/*lint -restore*/
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AF_WINDOW_LAST_PIXEL:
      format->name = "AF-WindowLastPix";
      format->parm_type = JPEGENC_MC_UINT16;
/*lint -save -e740*/
      *value = (uint16*)(&vfeAfStatDspInput);
/*lint -restore*/
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    case CAMERA_API_AF_WINDOW_1ST_PIXEL:
      format->name = "AF-Window1stPix";
      format->parm_type = JPEGENC_MC_UINT16;
/*lint -save -e740*/
      *value = (uint16*)(&vfeAfStatDspInput) + 1;
/*lint -restore*/
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;
#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */	  
    /*************** color matrix parameters ******************/
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A11:
      format->name = "ColorCorr-Default-RtoR";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.a11;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A12:
      format->name = "ColorCorr-Default-GtoR";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.a12;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A13:
      format->name = "ColorCorr-Default-BtoR";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.a13;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A21:
      format->name = "ColorCorr-Default-RtoG";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.a21;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A22:
      format->name = "ColorCorr-Default-GtoG";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.a22;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A23:
      format->name = "ColorCorr-Default-BtoG";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.a23;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A31:
      format->name = "ColorCorr-Default-RtoB";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.a31;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A32:
      format->name = "ColorCorr-Default-GtoB";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.a32;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A33:
      format->name = "ColorCorr-Default-BtoB";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.a33;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_BIAS_R:
      format->name = "ColorCorr-Default-ROffset";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.bias_r;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_BIAS_G:
      format->name = "ColorCorr-Default-GOffset";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.bias_g;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_BIAS_B:
      format->name = "ColorCorr-Default-BOffset";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->color_correction.bias_b;
      break;

    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_ENABLE:
      format->name = "ColorCorr-LowLight-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &cam3a_awb_state.low_light_CC_enable;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;

    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A11:
      format->name = "ColorCorr-LowLight-RtoR";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.a11;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A12:
      format->name = "ColorCorr-LowLight-GtoR";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.a12;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A13:
      format->name = "ColorCorr-LowLight-BtoR";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.a13;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A21:
      format->name = "ColorCorr-LowLight-RtoG";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.a21;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A22:
      format->name = "ColorCorr-LowLight-GtoG";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.a22;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A23:
      format->name = "ColorCorr-LowLight-BtoG";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.a23;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A31:
      format->name = "ColorCorr-LowLight-RtoB";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.a31;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A32:
      format->name = "ColorCorr-LowLight-GtoB";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.a32;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A33:
      format->name = "ColorCorr-LowLight-BtoB";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.a33;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_BIAS_R:
      format->name = "ColorCorr-LowLight-ROffset";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.bias_r;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_BIAS_G:
      format->name = "ColorCorr-LowLight-GOffset";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.bias_g;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_BIAS_B:
      format->name = "ColorCorr-LowLight-BOffset";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->low_light_color_correction.bias_b;
      break;

    case CAMERA_API_COLOR_CONVERSION_TL84_A11:    
      format->name = "ColorConv-TL84-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.a11;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A12:    
      format->name = "ColorConv-TL84-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.a12;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A13:    
      format->name = "ColorConv-TL84-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.a13;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A21:    
      format->name = "ColorConv-TL84-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.a21;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A22:    
      format->name = "ColorConv-TL84-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.a22;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A23:    
      format->name = "ColorConv-TL84-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.a23;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A31:    
      format->name = "ColorConv-TL84-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.a31;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A32:    
      format->name = "ColorConv-TL84-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.a32;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A33:   
      format->name = "ColorConv-TL84-BtoCrb_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.a33;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_BIAS_Y:   
      format->name = "ColorConv-TL84-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.bias_y;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_BIAS_CB:   
      format->name = "ColorConv-TL84-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.bias_Cb;
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_BIAS_CR:   
      format->name = "ColorConv-TL84-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->TL84_flourescent_color_conversion.bias_Cr;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A11:    
      format->name = "ColorConv-A-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.a11;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A12:    
      format->name = "ColorConv-A-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.a12;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A13:    
      format->name = "ColorConv-A-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.a13;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A21:    
      format->name = "ColorConv-A-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.a21;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A22:    
      format->name = "ColorConv-A-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.a22;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A23:    
      format->name = "ColorConv-A-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.a23;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A31:    
      format->name = "ColorConv-A-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.a31;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A32:    
      format->name = "ColorConv-A-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.a32;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A33:    
      format->name = "ColorConv-A-BtoCr_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.a33;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_BIAS_Y:   
      format->name = "ColorConv-A-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.bias_y;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_BIAS_CB:   
      format->name = "ColorConv-A-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.bias_Cb;
      break;
    case CAMERA_API_COLOR_CONVERSION_A_BIAS_CR:   
      format->name = "ColorConv-A-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->incandescent_color_conversion.bias_Cr;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A11:    
      format->name = "ColorConv-D65-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.a11;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A12:    
      format->name = "ColorConv-D65-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.a12;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A13:    
      format->name = "ColorConv-D65-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.a13;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A21:    
      format->name = "ColorConv-D65-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.a21;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A22:    
      format->name = "ColorConv-D65-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.a22;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A23:    
      format->name = "ColorConv-D65-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.a23;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A31:    
      format->name = "ColorConv-D65-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.a31;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A32:    
      format->name = "ColorConv-D65-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.a32;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A33:    
      format->name = "ColorConv-D65-BtoCr_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.a33;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_BIAS_Y:   
      format->name = "ColorConv-D65-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.bias_y;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_BIAS_CB:   
      format->name = "ColorConv-D65-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.bias_Cb;
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_BIAS_CR:   
      format->name = "ColorConv-D65-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->daylight_color_conversion.bias_Cr;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A11:    
      format->name = "ColorConv-Mono-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.a11;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A12:    
      format->name = "ColorConv-Mono-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.a12;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A13:    
      format->name = "ColorConv-Mono-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.a13;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A21:    
      format->name = "ColorConv-Mono-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.a21;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A22:    
      format->name = "ColorConv-Mono-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.a22;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A23:    
      format->name = "ColorConv-Mono-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.a23;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A31:    
      format->name = "ColorConv-Mono-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.a31;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A32:    
      format->name = "ColorConv-Mono-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.a32;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A33:    
      format->name = "ColorConv-Mono-BtoCr_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.a33;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_BIAS_Y:   
      format->name = "ColorConv-Mono-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.bias_y;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_BIAS_CB:   
      format->name = "ColorConv-Mono-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.bias_Cb;
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_BIAS_CR:   
      format->name = "ColorConv-Mono-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->monochrome_special_effect.bias_Cr;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A11:    
      format->name = "ColorConv-Sepia-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.a11;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A12:    
      format->name = "ColorConv-Sepia-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.a12;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A13:    
      format->name = "ColorConv-Sepia-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.a13;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A21:    
      format->name = "ColorConv-Sepia-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.a21;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A22:    
      format->name = "ColorConv-Sepia-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.a22;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A23:    
      format->name = "ColorConv-Sepia-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.a23;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A31:    
      format->name = "ColorConv-Sepia-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.a31;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A32:    
      format->name = "ColorConv-Sepia-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.a32;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A33:    
      format->name = "ColorConv-Sepia-BtoCr_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.a33;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_BIAS_Y:   
      format->name = "ColorConv-Sepia-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.bias_y;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_BIAS_CB:   
      format->name = "ColorConv-Sepia-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.bias_Cb;
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_BIAS_CR:   
      format->name = "ColorConv-Sepia-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sepia_special_effect.bias_Cr;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A11:    
      format->name = "ColorConv-Negative-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.a11;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A12:    
      format->name = "ColorConv-Negative-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.a12;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A13:    
      format->name = "ColorConv-Negative-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.a13;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A21:    
      format->name = "ColorConv-Negative-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.a21;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A22:    
      format->name = "ColorConv-Negative-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.a22;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A23:    
      format->name = "ColorConv-Negative-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.a23;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A31:    
      format->name = "ColorConv-Negative-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.a31;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A32:    
      format->name = "ColorConv-Negative-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.a32;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A33:    
      format->name = "ColorConv-Negative-BtoCr_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.a33;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_BIAS_Y:   
      format->name = "ColorConv-Negative-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.bias_y;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_BIAS_CB:   
      format->name = "ColorConv-Negative-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.bias_Cb;
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_BIAS_CR:   
      format->name = "ColorConv-Negative-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->negative_special_effect.bias_Cr;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SAT_FACTOR:    
      format->name = "BSMColorConv-SatFactor";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->saturated_color_conversion_factor;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A11:    
      format->name = "ColorConv-Sunset-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.a11;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A12:    
      format->name = "ColorConv-Sunset-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.a12;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A13:    
      format->name = "ColorConv-Sunset-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.a13;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A21:    
      format->name = "ColorConv-Sunset-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.a21;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A22:    
      format->name = "ColorConv-Sunset-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.a22;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A23:    
      format->name = "ColorConv-Sunset-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.a23;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A31:    
      format->name = "ColorConv-Sunset-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.a31;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A32:    
      format->name = "ColorConv-Sunset-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.a32;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A33:    
      format->name = "ColorConv-Sunset-BtoCr_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.a33;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_BIAS_Y:   
      format->name = "ColorConv-Sunset-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.bias_y;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_BIAS_CB:   
      format->name = "ColorConv-Sunset-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.bias_Cb;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_BIAS_CR:   
      format->name = "ColorConv-Sunset-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->sunset_color_conversion.bias_Cr;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A11:    
      format->name = "ColorConv-SkinTL84-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.a11;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A12:    
      format->name = "ColorConv-SkinTL84-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.a12;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A13:    
      format->name = "ColorConv-SkinTL84-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.a13;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A21:    
      format->name = "ColorConv-SkinTL84-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.a21;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A22:    
      format->name = "ColorConv-SkinTL84-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.a22;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A23:    
      format->name = "ColorConv-SkinTL84-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.a23;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A31:    
      format->name = "ColorConv-SkinTL84-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.a31;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A32:    
      format->name = "ColorConv-SkinTL84-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.a32;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A33:    
      format->name = "ColorConv-SkinTL84-BtoCr_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.a33;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_BIAS_Y:   
      format->name = "ColorConv-SkinTL84-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.bias_y;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_BIAS_CB:   
      format->name = "ColorConv-SkinTL84-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.bias_Cb;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_BIAS_CR:   
      format->name = "ColorConv-SkinTL84-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion.bias_Cr;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A11:    
      format->name = "ColorConv-SkinA-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.a11;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A12:    
      format->name = "ColorConv-SkinA-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.a12;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A13:    
      format->name = "ColorConv-SkinA-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.a13;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A21:    
      format->name = "ColorConv-SkinA-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.a21;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A22:    
      format->name = "ColorConv-SkinA-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.a22;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A23:    
      format->name = "ColorConv-SkinA-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.a23;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A31:    
      format->name = "ColorConv-SkinA-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.a31;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A32:    
      format->name = "ColorConv-SkinA-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.a32;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A33:    
      format->name = "ColorConv-SkinA-BtoCr_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.a33;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_BIAS_Y:   
      format->name = "ColorConv-SkinA-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.bias_y;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_BIAS_CB:   
      format->name = "ColorConv-SkinA-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.bias_Cb;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_BIAS_CR:   
      format->name = "ColorConv-SkinA-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_a.bias_Cr;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A11:    
      format->name = "ColorConv-SkinD65-RtoY_a11";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.a11;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A12:    
      format->name = "ColorConv-SkinD65-GtoY_a12";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.a12;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A13:    
      format->name = "ColorConv-SkinD65-BtoY_a13";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.a13;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A21:    
      format->name = "ColorConv-SkinD65-RtoCb_a21";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.a21;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A22:    
      format->name = "ColorConv-SkinD65-GtoCb_a22";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.a22;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A23:    
      format->name = "ColorConv-SkinD65-BtoCb_a23";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.a23;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A31:    
      format->name = "ColorConv-SkinD65-RtoCr_a31";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.a31;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A32:    
      format->name = "ColorConv-SkinD65-GtoCr_a32";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.a32;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A33:    
      format->name = "ColorConv-SkinD65-BtoCr_a33";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.a33;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_BIAS_Y:   
      format->name = "ColorConv-SkinD65-Bias_Y";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.bias_y;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_BIAS_CB:   
      format->name = "ColorConv-SkinD65-Bias_Cb";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.bias_Cb;
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_BIAS_CR:   
      format->name = "ColorConv-SkinD65-Bias_Cr";
      format->parm_type = JPEGENC_MC_INT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG | 
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->skintone_color_conversion_d65.bias_Cr;
      break;
    /*************** AFR parameters *****************/
    case CAMERA_API_AFR_ENABLE:
      format->name = "AFR-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &cam3a_aec_state.afr_enable;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;

    case CAMERA_API_AFR_POSSIBLE_FRAME_COUNT:
      format->name = "AFR-PossibleFrameCnt";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &camsensor_ptr->num_possible_frame_rates;
      break;

    case CAMERA_API_AFR_PREVIEW_FPS:
      format->name = "AFR-PreviewFps";
      format->parm_type = JPEGENC_MC_UINT16;
      *value = &camsensor_ptr->preview_fps;
      format->flags = CAMERA_PARMS_API_READ_ONLY;
      break;

    /*************** Anti-baidng parameters *********/
    case CAMERA_API_ANTI_BAND_ENABLE:
      format->name = "Antibanding-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &cam3a_aec_state.antibanding_enable;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;

    case CAMERA_API_ANTI_BAND_PIXEL_CLK:
      format->name = "Antibanding-PixelClk";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &camsensor_ptr->pixel_clock;
      format->flags = CAMERA_PARMS_API_REQUIRES_APPLY;
      break;

    case CAMERA_API_ANTI_BAND_PIXEL_CLK_PER_LINE:
      format->name = "Antibanding-PixelClkPerLine";
      format->parm_type = JPEGENC_MC_INT32;
      *value = &camsensor_ptr->pixel_clock_per_line;
      format->flags = CAMERA_PARMS_API_REQUIRES_APPLY;
      break;

    /*************** defect pixel correction ********/
    case CAMERA_API_DEFECT_PIXEL_COR_ENABLE:
      format->name = "BPC-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camsensor_ptr->defectPixelCorrection.enable;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;
    case CAMERA_API_DEFECT_PIXEL_COR_THRESHOLD_MIN:
      format->name = "BPC-Fmin";
      format->parm_type = JPEGENC_MC_UINT16;
      *value = &camsensor_ptr->defectPixelCorrection.minThreshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_DEFECT_PIXEL_COR_THRESHOLD_MAX:
      format->name = "BPC-Fmax";
      format->parm_type = JPEGENC_MC_UINT16;
      *value = &camsensor_ptr->defectPixelCorrection.maxThreshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
    /*************** 3x3 luma filter ***************/
    case CAMERA_API_3X3_LUMA_ENABLE:
      format->name = "3X3ASF-LumaEnable ";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camsensor_ptr->f3x3LumaFilter.enable;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      break;
    case CAMERA_API_3X3_LUMA_HW_OR_SW:
#ifndef  QDSP_MODULE_VFE25_DEFINED
      format->name = "3X3ASF-LumaHWOrSW";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &vfe_hw_features.filter_3x3_luma;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
#else
#error code not present
#endif /*! QDSP_MODULE_VFE25_DEFINED */
      break;
    /*************** 3x3 h/w ASF parameters ******************/
    case CAMERA_API_3X3_ASF_ENABLE:
#ifndef  QDSP_MODULE_VFE25_DEFINED
      format->name = "3X3ASF-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &vfe_hw_features.filter_3x3_asf;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
#else
#error code not present
#endif /* ! QDSP_MODULE_VFE25_DEFINED */
      break;
#ifndef QDSP_MODULE_VFE25_DEFINED
    case CAMERA_API_3X3_ASF_EDGE_FILTER:
      format->name = "3X3ASF-Regular-Edge";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->num_elements = ASF_FILTER_SIZE * ASF_FILTER_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);

      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->chromatix_adaptive_spatial_filter.edge_filter;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ASF_FILTER_SIZE * ASF_FILTER_SIZE)
      {
        *value = &chromatix_ptr->chromatix_adaptive_spatial_filter.edge_filter[table_index / ASF_FILTER_SIZE][table_index % ASF_FILTER_SIZE];
      }   
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_3X3_ASF_NOISE_FILTER:
      format->name = "3X3ASF-Regular-Noise";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->num_elements = ASF_FILTER_SIZE * ASF_FILTER_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);

      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->chromatix_adaptive_spatial_filter.noise_filter;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ASF_FILTER_SIZE * ASF_FILTER_SIZE)
      {
        *value = &chromatix_ptr->chromatix_adaptive_spatial_filter.noise_filter[table_index / ASF_FILTER_SIZE][table_index % ASF_FILTER_SIZE];
      }   
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_3X3_ASF_E_THRESHOLD:
      format->name = "3X3ASF-Regular-EdgeThresh";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_adaptive_spatial_filter.edge_threshold;
      break;
    case CAMERA_API_3X3_ASF_N_THRESHOLD:
      format->name = "3X3ASF-Regular-NoiseThresh";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_adaptive_spatial_filter.noise_threshold;
      break;
    case CAMERA_API_3X3_ASF_E_DETECTION_FLAG:
      format->name = "3X3ASF-Regular-EdgeFlag";
      format->parm_type = JPEGENC_MC_UINT8;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_adaptive_spatial_filter.edge_detection_flag;
      break;
#endif /* !QDSP_MODULE_VFE25_DEFINED */
#if 0 //TODO: Updated values to be reflected in services
    case CAMERA_API_3X3_ASF_LOW_LIGHT_EDGE_FILTER:
      format->name = "3X3ASF-LowLight-Edge";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->num_elements = ASF_FILTER_SIZE * ASF_FILTER_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);

      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->chromatix_low_light_asf.edge_filter;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ASF_FILTER_SIZE * ASF_FILTER_SIZE)
      {
        *value = &chromatix_ptr->chromatix_low_light_asf.edge_filter[table_index / ASF_FILTER_SIZE][table_index % ASF_FILTER_SIZE];
      }   
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_3X3_ASF_LOW_LIGHT_NOISE_FILTER:
      format->name = "3X3ASF-LowLight-Noise";
      format->parm_type = JPEGENC_MC_FLOAT;
      format->num_elements = ASF_FILTER_SIZE * ASF_FILTER_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);

      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->chromatix_low_light_asf.noise_filter;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ASF_FILTER_SIZE * ASF_FILTER_SIZE)
      {
        *value = &chromatix_ptr->chromatix_low_light_asf.noise_filter[table_index / ASF_FILTER_SIZE][table_index % ASF_FILTER_SIZE];
      }   
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_3X3_ASF_LOW_LIGHT_E_THRESHOLD:
      format->name = "3X3ASF-LowLight-EdgeThresh";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_low_light_asf.edge_threshold;
      break;
    case CAMERA_API_3X3_ASF_LOW_LIGHT_N_THRESHOLD:
      format->name = "3X3ASF-LowLight-NoiseThresh";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_low_light_asf.noise_threshold;
      break;
    case CAMERA_API_3X3_ASF_LOW_LIGHT_E_DETECTION_FLAG:
      format->name = "3X3ASF-LowLight-EdgeFlag";
      format->parm_type = JPEGENC_MC_UINT8;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->chromatix_low_light_asf.edge_detection_flag;
      break;
#endif
    /*************** chroma suppression ************/
    case CAMERA_API_CHRO_SUP_ENABLE:
      format->name = "ChroSup-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camctrl_tbl.cs_is_supported;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      format->flags = CAMERA_PARMS_API_EXCLUDE_TARGET_UI;
      break;
    case CAMERA_API_CHRO_SUP_LUMA_THRES_1:
      format->name = "ChromaSupp-LumaThresh1";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->cs_luma_threshold1;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_CHRO_SUP_LUMA_THRES_2: 
      format->name = "ChromaSupp-LumaThresh2";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->cs_luma_threshold2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_CHRO_SUP_LUMA_THRES_3: 
      format->name = "ChromaSupp-LumaThresh3";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->cs_luma_threshold3;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_CHRO_SUP_LUMA_THRES_4: 
      format->name = "ChromaSupp-LumaThresh4";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->cs_luma_threshold4;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_CHRO_SUP_CHRO_THRES_1: 
      format->name = "ChromaSupp-ChromaThresh1";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->cs_chroma_threshold1;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_CHRO_SUP_CHRO_THRES_2:
      format->name = "ChromaSupp-ChromaThresh2";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->cs_chroma_threshold2;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    /*************** luma adaptation ***************/
    case CAMERA_API_LUMA_ADAP_ENABLE: 
      format->name = "LumaAdapt-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camctrl_tbl.la_is_supported;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      format->flags = CAMERA_PARMS_API_EXCLUDE_TARGET_UI;
      break;
    case CAMERA_API_LUMA_ADAP_THRESHOLD:
      format->name = "LumaAdapt-Thresh";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->la_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;

    /*************** HJR ***************************/
    case CAMERA_API_HJR_ENABLE:
      format->name = "HJR-Enable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camctrl_tbl.hjr_bayer_filtering_enable;
      ui8min = 0;
      format->min = &ui8min;
      ui8max = 1;
      format->max = &ui8max;
      format->flags = CAMERA_PARMS_API_EXCLUDE_TARGET_UI;
      break;
    case CAMERA_API_HJR_MAX_NUM_FRAMES:
      format->name = "HJR-MaxFrames";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &camctrl_tbl.hjr_max_frames;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_HJR_ONE_TO_TWO_OFFSET: 
      format->name = "HJR-1to2FrameOffset";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->one_to_two_frame_hjr_offset;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_HJR_N_REDUCTION_FLAT: 
      format->name = "HJR-FlatAreaNoise";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->flat_area_noise_reduction_level;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_HJR_N_REDUCTION_TEXTURE:
      format->name = "HJR-TextureNoise";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->texture_noise_reduction_level;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_HJR_TEXTURE_THRESHOLD:
      format->name = "HJR-TextureThresh";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->texture_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_HJR_K_TABLE:
      format->name = "HJR-Ktable";
      format->parm_type = JPEGENC_MC_UINT8;
      format->num_elements = HJR_K_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                     (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                      CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->hjr_k_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < HJR_K_TABLE_SIZE)
      {
        *value = &chromatix_ptr->hjr_k_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_HJR_BAYER_FILTERING_ENABLE:
      format->name = "HJR-BayerFilterEnable";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->hjr_bayer_filtering_enable;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_CHROMATIX);
      break; 
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
    /**************Auto Flicker Detection************/
#if 0 //TODO: Updated values to be reflected in services
    case CAMERA_API_AFD_DETECTION_DATA_STD_THRESH:
      format->name = "AFD-DetectionDataStdThresh";
      format->parm_type = JPEGENC_MC_FLOAT;
      *value = &chromatix_ptr->auto_flicker_detection_data.afd_std_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AFD_DETECTION_DATA_PCT_THRESH:
      format->name = "AFD-DetectionDataPctThresh";
      format->parm_type = JPEGENC_MC_UINT8;
      *value = &chromatix_ptr->auto_flicker_detection_data.afd_percent_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AFD_DETECTION_DATA_DIFF_THRESH:
      format->name = "AFD-DetectionDataDiffThresh";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &chromatix_ptr->auto_flicker_detection_data.afd_diff_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
    case CAMERA_API_AFD_DETECTION_DATA_FRAME_CT_THRESH:
      format->name = "AFD-DetectionDataFrameCtThresh";
      format->parm_type = JPEGENC_MC_UINT32;
      *value = &chromatix_ptr->auto_flicker_detection_data.afd_frame_ct_threshold;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      break;
#endif
    /*************** Gamma *************************/
    case CAMERA_API_GAMMA_DEFAULT:
      format->name = "Gamma-DefaultGamma";
      format->parm_type = JPEGENC_MC_UINT8;
      format->num_elements = vfe_gamma_table_size;
      format->flags = (camera_parm_api_flag_type)(CAMERA_PARMS_API_REQUIRES_APPLY |
                      CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                      CAMERA_PARMS_API_EXCLUDE_INI |
                      CAMERA_PARMS_API_EXCLUDE_JPEG |
                      CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->default_gamma_table.arr;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < vfe_gamma_table_size)
      {
        *value = &chromatix_ptr->default_gamma_table.arr[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_GAMMA_LOW_LIGHT: 
      format->name = "Gamma-LowLightGamma";
      format->parm_type = JPEGENC_MC_UINT8;
      format->num_elements = vfe_gamma_table_size;
      format->flags = (camera_parm_api_flag_type)(CAMERA_PARMS_API_REQUIRES_APPLY |
                      CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                      CAMERA_PARMS_API_EXCLUDE_INI |
                      CAMERA_PARMS_API_EXCLUDE_JPEG |
                      CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->lowlight_gamma_table.arr;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < vfe_gamma_table_size)
      {
        *value = &chromatix_ptr->lowlight_gamma_table.arr[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_GAMMA_OUTDOOR: 
      format->name = "Gamma-OutdoorGamma";
      format->parm_type = JPEGENC_MC_UINT8;
      format->num_elements = vfe_gamma_table_size;
      format->flags = (camera_parm_api_flag_type)(CAMERA_PARMS_API_REQUIRES_APPLY |
                      CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                      CAMERA_PARMS_API_EXCLUDE_INI |
                      CAMERA_PARMS_API_EXCLUDE_JPEG |
                      CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->outdoor_gamma_table.arr;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < vfe_gamma_table_size)
      {
        *value = &chromatix_ptr->outdoor_gamma_table.arr[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_GAMMA_BACK_LIGHT: 
      format->name = "Gamma-BacklightGamma";
      format->parm_type = JPEGENC_MC_UINT8;
      format->num_elements = vfe_gamma_table_size;
      format->flags = (camera_parm_api_flag_type)(CAMERA_PARMS_API_REQUIRES_APPLY |
                      CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                      CAMERA_PARMS_API_EXCLUDE_INI |
                      CAMERA_PARMS_API_EXCLUDE_JPEG |
                      CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->backlight_gamma_table.arr;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < vfe_gamma_table_size)
      {
        *value = &chromatix_ptr->backlight_gamma_table.arr[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_GAMMA_SOLARIZED: 
      format->name = "Gamma-SolarizeReflection";
      format->parm_type = JPEGENC_MC_UINT8;
      format->flags = (camera_parm_api_flag_type)
                     (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                      CAMERA_PARMS_API_EXCLUDE_INI |
                      CAMERA_PARMS_API_REQUIRES_APPLY |
                      CAMERA_PARMS_API_EXCLUDE_JPEG |
                      CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->solarized_gamma_reflection_point;
      break;
    /*************** Black Level *************************/
    case CAMERA_API_BLACK_LEVEL_NORMAL_LIGHT_OFFSET: 
      format->name = "BlackLevel-NormalLightBlackLevelOffset";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->black_level_offset;
      break;
#if 0 //TODO: Updated values to be reflected in services	  
    case CAMERA_API_BLACK_LEVEL_LOW_LIGHT_OFFSET: 
      format->name = "BlackLevel-LowLightLevelOffset";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_REQUIRES_APPLY |  //Todo: Remove if doesn't require apply
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->black_level_offset_per_low_light;
      break;
#endif	  
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
    /*************** Lens Rolloff *************************/
#ifndef QDSP_MODULE_VFE25_DEFINED
    case CAMERA_API_ROLLOFF_TABLE_TL84_CX: 
      format->name = "Rolloff-TableTL84-CenterX";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].cx;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_CY: 
      format->name = "Rolloff-TableTL84-CenterY";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].cy;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_WIDTH: 
      format->name = "Rolloff-TableTL84-Width";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].width;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_HEIGHT: 
      format->name = "Rolloff-TableTL84-Height";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].height;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_NUM_INTERVALS: 
      format->name = "Rolloff-TableTL84-Intervals";
      format->parm_type = JPEGENC_MC_UINT8;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].N;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_RADIUS_SQUARE_TABLE:
      format->name = "Rolloff-TableTL84-RSqTable";
      format->parm_type = JPEGENC_MC_UINT32;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].radius_square_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].radius_square_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_RED_CORRECTION_TABLE:
      format->name = "Rolloff-TableTL84-RedTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].red_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].red_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_GREEN_CORRECTION_TABLE:
      format->name = "Rolloff-TableTL84-GreenTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].green_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].green_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_BLUE_CORRECTION_TABLE:
      format->name = "Rolloff-TableTL84-BlueTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].blue_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].blue_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_CX: 
      format->name = "Rolloff-TableA-CenterX";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].cx;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_CY: 
      format->name = "Rolloff-TableA-CenterY";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].cy;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_WIDTH: 
      format->name = "Rolloff-TableA-Width";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].width;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_HEIGHT: 
      format->name = "Rolloff-TableA-Height";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].height;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_NUM_INTERVALS: 
      format->name = "Rolloff-TableA-Intervals";
      format->parm_type = JPEGENC_MC_UINT8;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].N;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_RADIUS_SQUARE_TABLE:
      format->name = "Rolloff-TableA-RSqTable";
      format->parm_type = JPEGENC_MC_UINT32;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].radius_square_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].radius_square_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_RED_CORRECTION_TABLE:
      format->name = "Rolloff-TableA-RedTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].red_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].red_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_GREEN_CORRECTION_TABLE:
      format->name = "Rolloff-TableA-GreenTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].green_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].green_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_BLUE_CORRECTION_TABLE:
      format->name = "Rolloff-TableA-BlueTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].blue_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].blue_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_CX: 
      format->name = "Rolloff-TableD65-CenterX";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].cx;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_CY: 
      format->name = "Rolloff-TableD65-CenterY";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].cy;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_WIDTH: 
      format->name = "Rolloff-TableD65-Width";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].width;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_HEIGHT: 
      format->name = "Rolloff-TableD65-Height";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].height;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_NUM_INTERVALS: 
      format->name = "Rolloff-TableD65-Intervals";
      format->parm_type = JPEGENC_MC_UINT8;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].N;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_RADIUS_SQUARE_TABLE:
      format->name = "Rolloff-TableD65-RSqTable";
      format->parm_type = JPEGENC_MC_UINT32;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].radius_square_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].radius_square_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_RED_CORRECTION_TABLE:
      format->name = "Rolloff-TableD65-RedTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].red_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].red_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_GREEN_CORRECTION_TABLE:
      format->name = "Rolloff-TableD65-GreenTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].green_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].green_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_BLUE_CORRECTION_TABLE:
      format->name = "Rolloff-TableD65-BlueTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].blue_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].blue_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_CX: 
      format->name = "Rolloff-TableLowLight-CenterX";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].cx;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_CY: 
      format->name = "Rolloff-TableLowLight-CenterY";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].cy;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_WIDTH: 
      format->name = "Rolloff-TableLowLight-Width";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].width;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_HEIGHT: 
      format->name = "Rolloff-TableLowLight-Height";
      format->parm_type = JPEGENC_MC_UINT16;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].height;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_NUM_INTERVALS: 
      format->name = "Rolloff-TableLowLight-Intervals";
      format->parm_type = JPEGENC_MC_UINT8;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].N;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_RADIUS_SQUARE_TABLE:
      format->name = "Rolloff-TableLowLight-RSqTable";
      format->parm_type = JPEGENC_MC_UINT32;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].radius_square_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].radius_square_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_RED_CORRECTION_TABLE:
      format->name = "Rolloff-TableLowLight-RedTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].red_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].red_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_GREEN_CORRECTION_TABLE:
      format->name = "Rolloff-TableLowLight-GreenTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].green_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].green_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_BLUE_CORRECTION_TABLE:
      format->name = "Rolloff-TableLowLight-BlueTable";
      format->parm_type = JPEGENC_MC_DOUBLE;
      format->num_elements = ROLLOFF_TABLE_SIZE;
      format->flags = (camera_parm_api_flag_type)
                      (CAMERA_PARMS_API_EXCLUDE_TARGET_UI |
                       CAMERA_PARMS_API_EXCLUDE_INI |
                       CAMERA_PARMS_API_EXCLUDE_JPEG |
                       CAMERA_PARMS_API_REQUIRES_APPLY |
                       CAMERA_PARMS_API_CHROMATIX);
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          *value = chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].blue_cf_table;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        *value = &chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].blue_cf_table[table_index];
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
#endif /* !QDSP_MODULE_VFE25_DEFINED */
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */  
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */

    default:
      return CAMERA_NOT_SUPPORTED;
  }

  return CAMERA_SUCCESS;
} /* camera_parms_read */

/*===========================================================================

FUNCTION      CAMERA_PARMS_WRITE

DESCRIPTION
  This API writes a parameter value. Camera services applies the value written.
  phase [in]      : Phase when parameter is being written.
  parm  [in]      : Enumerated value specifying the parameter to be written.
  table_index [in]: If parameter is a table
                   if >= 0, only the corresponding element of the table is written.
                   if -1 the whole table is written.
 value [in]      : Pointer to (type)value. 

RETURN VALUE

===========================================================================*/
/*lint -save -e715*/
camera_ret_code_type camera_parms_write (
  camera_tune_phase_type     phase, 
  camera_api_parms_type      parm, 
  /* If table_index is >= 0, 
   * only the corresponding member of the table is affected.  
   * If table_index is -1, the API operates on the entire table.
   */
  int32                      table_index,
  const void                 *value
)
{
 #ifndef FEATURE_CAMERA_YCBCR_ONLY 
  uint32 i;
 #ifndef QDSP_MODULE_VFE25_DEFINED
  uint32 j;
 #endif
 #endif 
  if (parm > CAMERA_API_LAST)
  {
    return CAMERA_INVALID_PARM;
  }

  switch (parm)
  {
#ifndef FEATURE_CAMERA_YCBCR_ONLY
    /*************** MWB parameters *****************/
    case CAMERA_API_MWB_FLUORESCENT_R_GAIN:     
      chromatix_ptr->chromatix_tl84_white_balance.r_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_TL84)->r_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_FLUORESCENT_G_GAIN:     
      chromatix_ptr->chromatix_tl84_white_balance.g_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_TL84)->g_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_FLUORESCENT_B_GAIN:     
      chromatix_ptr->chromatix_tl84_white_balance.b_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_TL84)->b_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_SUNNY_R_GAIN:     
      chromatix_ptr->chromatix_d50_white_balance.r_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_D65)->r_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_SUNNY_G_GAIN:     
      chromatix_ptr->chromatix_d50_white_balance.g_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_D65)->g_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_SUNNY_B_GAIN:     
      chromatix_ptr->chromatix_d50_white_balance.b_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_D65)->b_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_TUNGSTEN_R_GAIN:     
      chromatix_ptr->chromatix_incandescent_white_balance.r_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_A)->r_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_TUNGSTEN_G_GAIN:     
      chromatix_ptr->chromatix_incandescent_white_balance.g_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_A)->g_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_TUNGSTEN_B_GAIN:     
      chromatix_ptr->chromatix_incandescent_white_balance.b_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_A)->b_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_CLOUDY_R_GAIN:     
      chromatix_ptr->chromatix_d65_white_balance.r_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_CLOUDY)->r_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_CLOUDY_G_GAIN:     
      chromatix_ptr->chromatix_d65_white_balance.g_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_CLOUDY)->g_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_CLOUDY_B_GAIN:     
      chromatix_ptr->chromatix_d65_white_balance.b_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_CLOUDY)->b_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_STROBE_R_GAIN:     
      chromatix_ptr->strobe_flash_white_balance.r_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_STROBE_FLASH)->r_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_STROBE_G_GAIN:     
      chromatix_ptr->strobe_flash_white_balance.g_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_STROBE_FLASH)->g_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_STROBE_B_GAIN:     
      chromatix_ptr->strobe_flash_white_balance.b_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_STROBE_FLASH)->b_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_LED_R_GAIN:     
      chromatix_ptr->led_flash_white_balance.r_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_LED_FLASH)->r_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_LED_G_GAIN:     
      chromatix_ptr->led_flash_white_balance.g_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_LED_FLASH)->g_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_MWB_LED_B_GAIN:     
      chromatix_ptr->led_flash_white_balance.b_gain = *(float*)value;
      vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_LED_FLASH)->b_gain
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;

    /*************** AWB parameters *****************/
    case CAMERA_API_AWB_ENABLE:
      camsensor_ptr->awb_enable = *(uint8*)value;
      break;
#if 0 //TODO: Updated values to be reflected in services	  
    case CAMERA_API_AWB_CHANNEL_GAIN_GREEN_EVEN_ROW:     
      chromatix_ptr->chromatix_channel_balance_gains.green_even = *(float*)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
    case CAMERA_API_AWB_CHANNEL_GAIN_GREEN_ODD_ROW:     
      chromatix_ptr->chromatix_channel_balance_gains.green_odd = *(float*)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
    case CAMERA_API_AWB_CHANNEL_GAIN_RED:     
      chromatix_ptr->chromatix_channel_balance_gains.red = *(float*)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
    case CAMERA_API_AWB_CHANNEL_GAIN_BLUE:     
      chromatix_ptr->chromatix_channel_balance_gains.blue = *(float*)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
#endif
    case CAMERA_API_AWB_REFERENCE_POINT_RED_D65:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[0].red_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_D65:   
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[0].blue_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_D65:   
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[0].red_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_D65: 
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[0].blue_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_D75:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[1].red_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_D75:   
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[1].blue_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_D75:   
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[1].red_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_D75:   
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[1].blue_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_A:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[2].red_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_A:     
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[2].blue_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_A:     
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[2].red_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_A:     
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[2].blue_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_TL84:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[3].red_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_TL84:  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[3].blue_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_TL84:  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[3].red_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_TL84:  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[3].blue_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_CW:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[4].red_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_CW:    
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[4].blue_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_CW:    
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[4].red_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_CW:    
       if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[4].blue_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_H:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[5].red_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_H:     
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[5].blue_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_H:     
       if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[5].red_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_H:     
       if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[5].blue_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_D50:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[6].red_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_D50:   
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[6].blue_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_D50:   
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[6].red_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_D50:   
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[6].blue_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_CUST_F:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[7].red_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_CUST_F:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[7].blue_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_CUST_F:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[7].red_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_CUST_F:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[7].blue_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_RED_NOON:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[8].red_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_REFERENCE_POINT_BLUE_NOON:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[8].blue_gain = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_RED_NOON:
	if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[8].red_gain_adj = *(float*)value;
      break;
    case CAMERA_API_AWB_ADJUST_GAIN_BLUE_NOON:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->awb_reference_hw_rolloff[8].blue_gain_adj = *(float*)value;
      break;

    case CAMERA_API_AWB_LOW_LIGHT_VFE_MIN_Y:     
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_awb_reconfig = TRUE;
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].y_min = *(uint8*)value;
      cam3a_awb_state.bounding_box_low_light.y_min = *(uint8*)value;
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_MAX_Y:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].y_max = *(uint8*)value;
      cam3a_awb_state.bounding_box_low_light.y_max = *(uint8*)value;
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_M1:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].m1 = *(int8*)value;
      cam3a_awb_state.bounding_box_low_light.m1 = *(int8*)value;
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_M2:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].m2 = *(int8*)value;
      cam3a_awb_state.bounding_box_low_light.m2 = *(int8*)value;
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_M3:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].m3 = *(int8*)value;
      cam3a_awb_state.bounding_box_low_light.m3 = *(int8*)value;
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_M4:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].m4 = *(int8*)value;
      cam3a_awb_state.bounding_box_low_light.m4 = *(int8*)value;
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_C1:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].c1 = *(int16*)value;
      cam3a_awb_state.bounding_box_low_light.c1 = *(int16*)value;
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_C2:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].c2 = *(int16*)value;
      cam3a_awb_state.bounding_box_low_light.c2 = *(int16*)value;
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_C3:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].c3 = *(int16*)value;
      cam3a_awb_state.bounding_box_low_light.c3 = *(int16*)value;
      break;
    case CAMERA_API_AWB_LOW_LIGHT_VFE_C4:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_LOW_LIGHT].c4 = *(int16*)value;
      cam3a_awb_state.bounding_box_low_light.c4 = *(int16*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_MIN_Y:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].y_min = *(uint8*)value;
      cam3a_awb_state.bounding_box_normal.y_min= *(uint8*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_MAX_Y:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].y_max = *(uint8*)value;
      cam3a_awb_state.bounding_box_normal.y_max= *(uint8*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_M1:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].m1 = *(int8*)value;
      cam3a_awb_state.bounding_box_normal.m1= *(int8*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_M2:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].m2 = *(int8*)value;
      cam3a_awb_state.bounding_box_normal.m2= *(int8*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_M3:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].m3 = *(int8*)value;
      cam3a_awb_state.bounding_box_normal.m3= *(int8*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_M4:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].m4 = *(int8*)value;
      cam3a_awb_state.bounding_box_normal.m4= *(int8*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_C1:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].c1 = *(int16*)value;
      cam3a_awb_state.bounding_box_normal.c1= *(int16*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_C2:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].c2 = *(int16*)value;
      cam3a_awb_state.bounding_box_normal.c2= *(int16*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_C3:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].c3 = *(int16*)value;
      cam3a_awb_state.bounding_box_normal.c3= *(int16*)value;
      break;
    case CAMERA_API_AWB_NORMAL_LIGHT_VFE_C4:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_NORMAL_LIGHT].c4 = *(int16*)value;
      cam3a_awb_state.bounding_box_normal.c4= *(int16*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_MIN_Y:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].y_min = *(uint8*)value;
      cam3a_awb_state.bounding_box_outdoor.y_min= *(uint8*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_MAX_Y:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].y_max = *(uint8*)value;
      cam3a_awb_state.bounding_box_outdoor.y_max= *(uint8*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_M1:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].m1 = *(int8*)value;
      cam3a_awb_state.bounding_box_outdoor.m1= *(int8*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_M2:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].m2 = *(int8*)value;
      cam3a_awb_state.bounding_box_outdoor.m2= *(int8*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_M3:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].m3 = *(int8*)value;
      cam3a_awb_state.bounding_box_outdoor.m3= *(int8*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_M4:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].m4 = *(int8*)value;
      cam3a_awb_state.bounding_box_outdoor.m4= *(int8*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_C1:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].c1 = *(int16*)value;
      cam3a_awb_state.bounding_box_outdoor.c1= *(int16*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_C2:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].c2 = *(int16*)value;
      cam3a_awb_state.bounding_box_outdoor.c2= *(int16*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_C3:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].c3 = *(int16*)value;
      cam3a_awb_state.bounding_box_outdoor.c3= *(int16*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_VFE_C4:     
      chromatix_ptr->wb_exp_stats_hw_rolloff [AWB_STATS_OUTDOOR].c4 = *(int16*)value;
      cam3a_awb_state.bounding_box_outdoor.c4= *(int16*)value;
      break;

    case CAMERA_API_AWB_INDOOR_INDEX:     
      chromatix_ptr->awb_indoor_index = *(int32*)value;
      cam3a_aec_state.indoor_index = *(int32*)value;
      cam3a_aec_state.indoor_index_init = *(int32*)value;
      break;
    case CAMERA_API_AWB_OUTDOOR_INDEX:     
      chromatix_ptr->awb_outdoor_index = *(int32*)value;
      cam3a_aec_state.outdoor_index = *(int32*)value;
      cam3a_aec_state.outdoor_index_init = *(int32*)value;
      break;

    case CAMERA_API_AWB_SNOW_BLUE_ADJ_RATIO:
      chromatix_ptr->snow_blue_gain_adj_ratio = *(float*)value;
      break;
    case CAMERA_API_AWB_BEACH_BLUE_ADJ_RATIO:
      chromatix_ptr->beach_blue_gain_adj_ratio = *(float*)value;
      break;

    case CAMERA_API_AWB_OUTLIER_DISTANCE:
      chromatix_ptr->outlier_distance = *(int32*)value;
      cam3a_agw_ptr->awb_agw_grid_dist_2_threshold = *(int32*)value;
      break;

    case CAMERA_API_AWB_GREEN_OFFSET_RG:
      chromatix_ptr->green_offset_rg = *(int*)value;
      cam3a_agw_ptr->green_offset_rg = *(int*)value;
      break;

    case CAMERA_API_AWB_GREEN_OFFSET_BG:
      chromatix_ptr->green_offset_bg = *(int*)value;
      cam3a_agw_ptr->green_offset_bg = *(int*)value;
      break;

/*************** AWB Extended parameters *****************/
    case CAMERA_API_AWB_EXT_COMPACT_CLUSTER_R2:
      chromatix_ptr->compact_cluster_r2 = *(uint16*)value;
      cam3a_agw_ptr->compact_cluster_r2 = *(uint16*)value;
      break;
    case CAMERA_API_AWB_EXT_C_CLUSTER_TO_REF_SQ:     
      chromatix_ptr->compact_cluster_to_ref_point_r2 = *(uint16*)value;
      cam3a_agw_ptr->compact_cluster_to_ref_point_r2 = *(uint16*)value;
      break;
    case CAMERA_API_AWB_EXT_A_CLUSTER_THRESH:     
      chromatix_ptr->a_cluster_threshold = *(uint8*)value;
      cam3a_agw_ptr->a_cluster_threshold = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_F_CLUSTER_THRESH:     
      chromatix_ptr->f_cluster_threshold = *(uint8*)value;
      cam3a_agw_ptr->f_cluster_threshold = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_DAY_CLUSTER_THRESH:     
      chromatix_ptr->day_cluster_threshold = *(uint8*)value;
      cam3a_agw_ptr->day_cluster_threshold = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_OUTDOOR_G_THRESH:     
      chromatix_ptr->outdoor_green_threshold = *(uint8*)value;
      cam3a_agw_ptr->outdoor_green_threshold = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_OUTDOOR_G_THR_BRT_F:
      chromatix_ptr->outdoor_green_threshold_bright_F = *(uint8*)value;
      cam3a_agw_ptr->outdoor_green_threshold_bright_F = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_OUTDOOR_G_THR_DARK_F:
      chromatix_ptr->outdoor_green_threshold_dark_F = *(uint8*)value;
      cam3a_agw_ptr->outdoor_green_threshold_dark_F = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_DAY_CLUSTER_THR_F:
      chromatix_ptr->day_cluster_threshold_for_F = *(uint8*)value;
      cam3a_agw_ptr->day_cluster_threshold_for_F = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_WB_ALOW_F_LINE:
      chromatix_ptr->white_balance_allow_fline = *(uint8*)value;
      cam3a_agw_ptr->white_balance_allow_fline = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_OUT_VALID_SAMPLES_THR:
      chromatix_ptr->outdoor_valid_sample_cnt_threshold = *(uint8*)value;
      cam3a_agw_ptr->outdoor_valid_sample_cnt_threshold = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_OUT_GREEN_UP_THRESH:
      chromatix_ptr->outdoor_green_upper_threshold = *(uint8*)value;
      cam3a_agw_ptr->outdoor_green_upper_threshold = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_RADIUS_SQUARED_THR:
      chromatix_ptr->r2_threshold = *(uint16*)value;
      cam3a_agw_ptr->r2_threshold = *(uint16*)value;
      break;
    case CAMERA_API_AWB_EXT_OUT_G_THRESH_BRIGHT_A:
      chromatix_ptr->outdoor_green_threshold_bright_A = *(uint8*)value;
      cam3a_agw_ptr->outdoor_green_threshold_bright_A = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_OUT_G_THRESH_DARK_A:
      chromatix_ptr->outdoor_green_threshold_dark_A = *(uint8*)value;
      cam3a_agw_ptr->outdoor_green_threshold_dark_A = *(uint8*)value;
      break;
    case CAMERA_API_AWB_EXT_DAY_CLUSTER_THR_A:
      chromatix_ptr->day_cluster_threshold_for_A = *(uint8*)value;
      cam3a_agw_ptr->day_cluster_threshold_for_A = *(uint8*)value;
      break;

    case CAMERA_API_AWB_EXT_CC_GLOBAL_GAIN:
      chromatix_ptr->color_correction_global_gain = *(float*)value;
      cam3a_awb_state.color_correction_bias = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;

    case CAMERA_API_AWB_EXT_MIN_R_GAIN:
      chromatix_ptr->awb_min_gains.r_gain = *(float*)value;
      cam3a_awb_state.awb_min_wb.r_gain = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_AWB_EXT_MIN_G_GAIN:
      chromatix_ptr->awb_min_gains.g_gain = *(float*)value;
      cam3a_awb_state.awb_min_wb.g_gain = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_AWB_EXT_MIN_B_GAIN:
      chromatix_ptr->awb_min_gains.b_gain = *(float*)value;
      cam3a_awb_state.awb_min_wb.b_gain = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_AWB_EXT_MAX_R_GAIN:
      chromatix_ptr->awb_max_gains.r_gain = *(float*)value;
      cam3a_awb_state.awb_max_wb.r_gain = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_AWB_EXT_MAX_G_GAIN:
      chromatix_ptr->awb_max_gains.g_gain = *(float*)value;
      cam3a_awb_state.awb_max_wb.g_gain = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_AWB_EXT_MAX_B_GAIN:
      chromatix_ptr->awb_max_gains.b_gain = *(float*)value;
      cam3a_awb_state.awb_max_wb.b_gain = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;

    case CAMERA_API_AWB_EXT_OUTDOOR_SAMPLE_INFLUENCE:
      chromatix_ptr->sample_influence.outdoor_influence = *(float*)value;
      cam3a_agw_ptr->outdoor_sample_influence = *(float*)value;
      break;
    case CAMERA_API_AWB_EXT_INDOOR_SAMPLE_INFLUENCE:
      chromatix_ptr->sample_influence.indoor_influence = *(float*)value;
      cam3a_agw_ptr->indoor_sample_influence = *(float*)value;
      break;

    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_D65:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_D65:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_D65:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT][2] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_D75:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_CLOUDY].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_CLOUDY][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_D75:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_CLOUDY].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_CLOUDY][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_D75:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_CLOUDY].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_CLOUDY][2] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_A:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_INCANDESCENT].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_INCANDESCENT][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_A:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_INCANDESCENT].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_INCANDESCENT][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_A:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_INCANDESCENT].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_INCANDESCENT][2] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_TL84:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_WARM_FLO].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_WARM_FLO][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_TL84:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_WARM_FLO].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_WARM_FLO][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_TL84:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_WARM_FLO].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_WARM_FLO][2] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_CW:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_COLD_FLO].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_COLD_FLO][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_CW:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_COLD_FLO].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_COLD_FLO][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_CW:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_COLD_FLO].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_COLD_FLO][2] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_H:
      chromatix_ptr->awb_weight_vector[AGW_AWB_HORIZON].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_HORIZON][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_H:
      chromatix_ptr->awb_weight_vector[AGW_AWB_HORIZON].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_HORIZON][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_H:
      chromatix_ptr->awb_weight_vector[AGW_AWB_HORIZON].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_HORIZON][2] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_D50:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT1].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT1][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_D50:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT1].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT1][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_D50:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT1].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_SUNLIGHT1][2] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_CUSTFLUO:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_CUSTOM_FLO].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_CUSTOM_FLO][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_CUSTFLUO:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_CUSTOM_FLO].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_CUSTOM_FLO][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_CUSTFLUO:
      chromatix_ptr->awb_weight_vector[AGW_AWB_INDOOR_CUSTOM_FLO].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_INDOOR_CUSTOM_FLO][2] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_NOON:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_NOON].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_NOON][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_NOON:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_NOON].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_NOON][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_NOON:
      chromatix_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_NOON].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_OUTDOOR_NOON][2] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INDOOR_DAYLIGHT:
      chromatix_ptr->awb_weight_vector[AGW_AWB_HYBRID].indoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_HYBRID][0] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_OUTDOOR_DAYLIGHT:
      chromatix_ptr->awb_weight_vector[AGW_AWB_HYBRID].outdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_HYBRID][1] = *(int32*)value;
      break;
    case CAMERA_API_AWB_EXT_WEIGHT_VECTOR_INOUTDOOR_DAYLIGHT:
      chromatix_ptr->awb_weight_vector[AGW_AWB_HYBRID].inoutdoor_weight = *(int32*)value;
      cam3a_agw_ptr->awb_weight_vector[AGW_AWB_HYBRID][2] = *(int32*)value;
      break;

    case CAMERA_API_AWB_EXT_WHITE_WORLD_Y_MIN_RATIO:
      chromatix_ptr->awb_white_world_y_min_ratio = *(int32*)value;
      cam3a_awb_state.white_y_min_percent = *(int32*)value;
      break;

    /*************** AEC parameters *****************/
    case CAMERA_API_AEC_ENABLE:
      camsensor_ptr->aec_enable = *(uint8*)value;
      break;

    case CAMERA_API_AEC_AGGRESSIVENESS:
/*lint -save -e64 */
      camctrl_tbl.aec_aggressiveness = *(cam3a_aggressiveness_type*)value;
      cam3a_aec_state.aec_aggressiveness = *(cam3a_aggressiveness_type*)value;
/*lint -restore */
      break;

    case CAMERA_API_AEC_HIGH_LUMA_REGION_THRESHOLD:
      camctrl_tbl.high_luma_region_threshold = *(uint32*)value;
      break;

    case CAMERA_API_AEC_OUTDOOR_BRIGHT_ENABLE:
      camctrl_tbl.aec_outdoor_bright_region_is_supported = *(uint8*)value;
      break;

    case CAMERA_API_AEC_OUTDOOR_BRIGHT_REDUCTION:
      camctrl_tbl.aec_outdoor_bright_region_reduction = *(float*)value;
      break;

    case CAMERA_API_AEC_OUTDOOR_BRIGHT_THRESHOLD_LO:
      camctrl_tbl.aec_outdoor_bright_region_threshold_LO = *(uint32*)value;
      break;

    case CAMERA_API_AEC_OUTDOOR_BRIGHT_THRESHOLD_HI:
      camctrl_tbl.aec_outdoor_bright_region_threshold_HI = *(uint32*)value;
      break;
#if 0 //TODO: Updated values to be reflected in services
    case CAMERA_API_AEC_EXPOSURE_GAIN_TYPE:
      chromatix_ptr->exposure_gain_type = *(int8*)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
#endif
    case CAMERA_API_AEC_NUM_EXPOSURE_VALUES:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      chromatix_ptr->num_exposure_values = *(int16*)value;
      break;

    case CAMERA_API_AEC_TABLE_EXPOSURE_ENTRIES_GAIN:
      // This is a member of a structure in a 500 item array
      // This API can handle structures, and it can handle large arrays,
      // but it cannot handle large arrays of structures.
      // Instead, convert from indexed structure: exposure_table[index].gain
      // to indexed element: gain[index]
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      if ((table_index >= 0) &&    // We can only deal with a single element, not the whole table
          (table_index < MAX_EXPOSURE_TABLE_SIZE))
      {
        chromatix_ptr->exposure_table[table_index].gain = *(uint16 *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;

    case CAMERA_API_AEC_TABLE_EXPOSURE_ENTRIES_LINE_CT:
      // This is a member of a structure in a 500 item array
      // This API can handle structures, and it can handle large arrays,
      // but it cannot handle large arrays of structures.
      // Instead, convert from indexed structure: exposure_table[index].line_count
      // to indexed element: line_count[index]
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      if ((table_index >= 0) &&    // We can only deal with a single element, not the whole table
          (table_index < MAX_EXPOSURE_TABLE_SIZE))
      {
        chromatix_ptr->exposure_table[table_index].line_count = *(uint32 *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;

    case CAMERA_API_AEC_LUMA_TARGET_DEFAULT:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      chromatix_ptr->default_luma_target = *(uint8*)value;
      break;

    case CAMERA_API_AEC_LUMA_TARGET_OUTDOOR:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      chromatix_ptr->outdoor_luma_target = *(uint8*)value;
      break;

    case CAMERA_API_AEC_LUMA_TARGET_LOW_LIGHT:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      chromatix_ptr->lowlight_luma_target = *(uint8*)value;
      break;

    case CAMERA_API_AEC_LUMA_TARGET_BACK_LIGHT:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      chromatix_ptr->backlight_luma_target = *(uint8*)value;
      break;

    case CAMERA_API_AEC_TOLERANCE:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      chromatix_ptr->luma_tolerance = *(int32*)value;
      break;

    case CAMERA_API_AEC_EXP_INDEX_ADJ_STEP:
      chromatix_ptr->exposure_index_adj_step = *(float*)value;
      cam3a_aec_state.exposure_index_adj_step = *(float*)value;
      break;
    case CAMERA_API_AEC_ISO100_GAIN:
      chromatix_ptr->iso100_gain = *(uint32*)value;
      camctrl_tbl.iso100_gain = chromatix_ptr->iso100_gain;
      break;
    case CAMERA_API_AEC_INDOOR_INDEX:
      chromatix_ptr->aec_indoor_index = *(uint16*)value;
      cam3a_aec_state.indoor_index = chromatix_ptr->aec_indoor_index;
      cam3a_aec_state.indoor_index_init = chromatix_ptr->aec_indoor_index;
      break;
    case CAMERA_API_AEC_OUTDOOR_INDEX:
      chromatix_ptr->aec_outdoor_index = *(uint16*)value;
      cam3a_aec_state.outdoor_index = chromatix_ptr->aec_outdoor_index;
      cam3a_aec_state.outdoor_index_init = chromatix_ptr->aec_outdoor_index;
      break;
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
    /*************** AF parameters ******************/
    case CAMERA_API_AF_ENABLE:
      cam3a_af_state.is_enabled = *(uint8*)value;
      break;

    case CAMERA_API_AF_GROSS_STEP:
      camsensor_ptr->num_gross_steps_between_stat_points = *(int32*)value;
      break;
      
    case CAMERA_API_AF_FINE_STEP:
      camsensor_ptr->num_fine_steps_between_stat_points = *(int32*)value;
      break;

    case CAMERA_API_AF_FINE_SEARCH_POINTS:
      camsensor_ptr->num_fine_search_points = *(int32*)value;
      break;
      
    case CAMERA_API_AF_PROCESS:
      camsensor_ptr->af_process_type = *(cam3a_af_process_enum_type*)value;
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_af_reconfig = TRUE;
      break;

    case CAMERA_API_AF_NEAR_END:
      camsensor_ptr->position_near_end = *(int32*)value;
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_af_reconfig = TRUE;
      break;

    case CAMERA_API_AF_BOUNDARY:
      camsensor_ptr->position_boundary = *(int32*)value;
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_af_reconfig = TRUE;
      break;

    case CAMERA_API_AF_FAR_END:
      camsensor_ptr->position_far_end = *(int32*)value;
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_af_reconfig = TRUE;
      break;

    case CAMERA_API_AF_COLLECT_END_STAT:
      cam3a_af_state.collect_end_stat = *(uint8*)value;
      break;

    case CAMERA_API_AF_TEST_MODE:
      cam3a_af_state.af_test_mode = *(uint8*)value;
      break;

    case CAMERA_API_AF_UNDERSHOOT_PROTECT:
      camctrl_tbl.undershoot_protect = *(uint8*)value;
      break;

    case CAMERA_API_AF_RESET_LENS_AFTER_SNAP:
      camctrl_tbl.reset_lens_after_snap = *(uint8*)value;
      break;
      
    case CAMERA_API_AF_CLIP_TO_AF_RATIO:
      camctrl_tbl.clip_to_af_ratio = *(uint32*)value;
      break;
#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
    /*************** color matrix parameters ******************/
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A11:
      chromatix_ptr->color_correction.a11 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A12:
      chromatix_ptr->color_correction.a12 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A13:
      chromatix_ptr->color_correction.a13 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A21:
      chromatix_ptr->color_correction.a21 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A22:
      chromatix_ptr->color_correction.a22 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A23:
      chromatix_ptr->color_correction.a23 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A31:
      chromatix_ptr->color_correction.a31 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A32:
      chromatix_ptr->color_correction.a32 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_A33:
      chromatix_ptr->color_correction.a33 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_BIAS_R:
      chromatix_ptr->color_correction.bias_r = *(int16*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->bias_r = *(int16*)value;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_BIAS_G:
      chromatix_ptr->color_correction.bias_g = *(int16*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->bias_g = *(int16*)value;
      break;
    case CAMERA_API_COLOR_CORRECTION_DEFAULT_BIAS_B:
      chromatix_ptr->color_correction.bias_b = *(int16*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)->bias_b = *(int16*)value;
      break;

    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_ENABLE:
      cam3a_awb_state.low_light_CC_enable = *(uint8*)value;
      break;

    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A11:
      chromatix_ptr->low_light_color_correction.a11 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A12:
      chromatix_ptr->low_light_color_correction.a12 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A13:
      chromatix_ptr->low_light_color_correction.a13 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->A13
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A21:
      chromatix_ptr->low_light_color_correction.a21 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A22:
      chromatix_ptr->low_light_color_correction.a22 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A23:
      chromatix_ptr->low_light_color_correction.a23 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A31:
      chromatix_ptr->low_light_color_correction.a31 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A32:
      chromatix_ptr->low_light_color_correction.a32 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_A33:
      chromatix_ptr->low_light_color_correction.a33 = *(float*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,7);
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_BIAS_R:
      chromatix_ptr->low_light_color_correction.bias_r = *(int16*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->bias_r = *(int16*)value;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_BIAS_G:
      chromatix_ptr->low_light_color_correction.bias_g = *(int16*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->bias_g = *(int16*)value;
      break;
    case CAMERA_API_COLOR_CORRECTION_LOW_LIGHT_BIAS_B:
      chromatix_ptr->low_light_color_correction.bias_b = *(int16*)value;
      vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT)->bias_b = *(int16*)value;
      break;

    case CAMERA_API_COLOR_CONVERSION_TL84_A11:    
      chromatix_ptr->TL84_flourescent_color_conversion.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A12:    
      chromatix_ptr->TL84_flourescent_color_conversion.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A13:    
      chromatix_ptr->TL84_flourescent_color_conversion.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A21:    
      chromatix_ptr->TL84_flourescent_color_conversion.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A22:    
      chromatix_ptr->TL84_flourescent_color_conversion.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A23:    
      chromatix_ptr->TL84_flourescent_color_conversion.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A31:    
      chromatix_ptr->TL84_flourescent_color_conversion.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A32:    
      chromatix_ptr->TL84_flourescent_color_conversion.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_A33:   
      chromatix_ptr->TL84_flourescent_color_conversion.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_BIAS_Y:   
      chromatix_ptr->TL84_flourescent_color_conversion.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_BIAS_CB:   
      chromatix_ptr->TL84_flourescent_color_conversion.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_TL84_BIAS_CR:   
      chromatix_ptr->TL84_flourescent_color_conversion.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A11:    
      chromatix_ptr->incandescent_color_conversion.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A12:    
      chromatix_ptr->incandescent_color_conversion.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A13:    
      chromatix_ptr->incandescent_color_conversion.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A21:    
      chromatix_ptr->incandescent_color_conversion.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A22:    
      chromatix_ptr->incandescent_color_conversion.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A23:    
      chromatix_ptr->incandescent_color_conversion.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A31:    
      chromatix_ptr->incandescent_color_conversion.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A32:    
      chromatix_ptr->incandescent_color_conversion.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_A_A33:    
      chromatix_ptr->incandescent_color_conversion.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_A_BIAS_Y:   
      chromatix_ptr->incandescent_color_conversion.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_A_BIAS_CB:   
      chromatix_ptr->incandescent_color_conversion.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_A_BIAS_CR:   
      chromatix_ptr->incandescent_color_conversion.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A11:    
      chromatix_ptr->daylight_color_conversion.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A12:    
      chromatix_ptr->daylight_color_conversion.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A13:    
      chromatix_ptr->daylight_color_conversion.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A21:    
      chromatix_ptr->daylight_color_conversion.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A22:    
      chromatix_ptr->daylight_color_conversion.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A23:    
      chromatix_ptr->daylight_color_conversion.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A31:    
      chromatix_ptr->daylight_color_conversion.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A32:    
      chromatix_ptr->daylight_color_conversion.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_A33:    
      chromatix_ptr->daylight_color_conversion.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_BIAS_Y:   
      chromatix_ptr->daylight_color_conversion.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_BIAS_CB:   
      chromatix_ptr->daylight_color_conversion.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_D65_BIAS_CR:   
      chromatix_ptr->daylight_color_conversion.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A11:    
      chromatix_ptr->monochrome_special_effect.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A12:    
      chromatix_ptr->monochrome_special_effect.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A13:    
      chromatix_ptr->monochrome_special_effect.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A21:    
      chromatix_ptr->monochrome_special_effect.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A22:    
      chromatix_ptr->monochrome_special_effect.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A23:    
      chromatix_ptr->monochrome_special_effect.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A31:    
      chromatix_ptr->monochrome_special_effect.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A32:    
      chromatix_ptr->monochrome_special_effect.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_A33:    
      chromatix_ptr->monochrome_special_effect.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_BIAS_Y:   
      chromatix_ptr->monochrome_special_effect.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_BIAS_CB:   
      chromatix_ptr->monochrome_special_effect.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_MONO_BIAS_CR:   
      chromatix_ptr->monochrome_special_effect.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A11:    
      chromatix_ptr->sepia_special_effect.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A12:    
      chromatix_ptr->sepia_special_effect.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A13:    
      chromatix_ptr->sepia_special_effect.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A21:    
      chromatix_ptr->sepia_special_effect.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A22:    
      chromatix_ptr->sepia_special_effect.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A23:    
      chromatix_ptr->sepia_special_effect.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A31:    
      chromatix_ptr->sepia_special_effect.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A32:    
      chromatix_ptr->sepia_special_effect.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_A33:    
      chromatix_ptr->sepia_special_effect.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_BIAS_Y:   
      chromatix_ptr->sepia_special_effect.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_BIAS_CB:   
      chromatix_ptr->sepia_special_effect.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_SEPIA_BIAS_CR:   
      chromatix_ptr->sepia_special_effect.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A11:    
      chromatix_ptr->negative_special_effect.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A12:    
      chromatix_ptr->negative_special_effect.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A13:    
      chromatix_ptr->negative_special_effect.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A21:    
      chromatix_ptr->negative_special_effect.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A22:    
      chromatix_ptr->negative_special_effect.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A23:    
      chromatix_ptr->negative_special_effect.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A31:    
      chromatix_ptr->negative_special_effect.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A32:    
      chromatix_ptr->negative_special_effect.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_A33:    
      chromatix_ptr->negative_special_effect.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_BIAS_Y:   
      chromatix_ptr->negative_special_effect.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_BIAS_CB:   
      chromatix_ptr->negative_special_effect.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_COLOR_CONVERSION_NEGATIVE_BIAS_CR:   
      chromatix_ptr->negative_special_effect.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SAT_FACTOR:   
      chromatix_ptr->saturated_color_conversion_factor = *(float*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A11:    
      chromatix_ptr->sunset_color_conversion.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A12:    
      chromatix_ptr->sunset_color_conversion.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A13:    
      chromatix_ptr->sunset_color_conversion.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A21:    
      chromatix_ptr->sunset_color_conversion.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A22:    
      chromatix_ptr->sunset_color_conversion.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A23:    
      chromatix_ptr->sunset_color_conversion.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A31:    
      chromatix_ptr->sunset_color_conversion.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A32:    
      chromatix_ptr->sunset_color_conversion.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_A33:    
      chromatix_ptr->sunset_color_conversion.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_BIAS_Y:   
      chromatix_ptr->sunset_color_conversion.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_BIAS_CB:   
      chromatix_ptr->sunset_color_conversion.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SUNSET_BIAS_CR:   
      chromatix_ptr->sunset_color_conversion.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A11:    
      chromatix_ptr->skintone_color_conversion.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A12:    
      chromatix_ptr->skintone_color_conversion.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A13:    
      chromatix_ptr->skintone_color_conversion.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A21:    
      chromatix_ptr->skintone_color_conversion.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A22:    
      chromatix_ptr->skintone_color_conversion.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A23:    
      chromatix_ptr->skintone_color_conversion.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A31:    
      chromatix_ptr->skintone_color_conversion.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A32:    
      chromatix_ptr->skintone_color_conversion.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_A33:    
      chromatix_ptr->skintone_color_conversion.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_BIAS_Y:   
      chromatix_ptr->skintone_color_conversion.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_BIAS_CB:   
      chromatix_ptr->skintone_color_conversion.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_TL84_BIAS_CR:   
      chromatix_ptr->skintone_color_conversion.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A11:    
      chromatix_ptr->skintone_color_conversion_a.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A12:    
      chromatix_ptr->skintone_color_conversion_a.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A13:    
      chromatix_ptr->skintone_color_conversion_a.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A21:    
      chromatix_ptr->skintone_color_conversion_a.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A22:    
      chromatix_ptr->skintone_color_conversion_a.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A23:    
      chromatix_ptr->skintone_color_conversion_a.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A31:    
      chromatix_ptr->skintone_color_conversion_a.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A32:    
      chromatix_ptr->skintone_color_conversion_a.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_A33:    
      chromatix_ptr->skintone_color_conversion_a.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_BIAS_Y:   
      chromatix_ptr->skintone_color_conversion_a.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_BIAS_CB:   
      chromatix_ptr->skintone_color_conversion_a.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_A_BIAS_CR:   
      chromatix_ptr->skintone_color_conversion_a.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A11:    
      chromatix_ptr->skintone_color_conversion_d65.a11 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A11 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A12:    
      chromatix_ptr->skintone_color_conversion_d65.a12 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A12 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A13:    
      chromatix_ptr->skintone_color_conversion_d65.a13 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A13 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A21:    
      chromatix_ptr->skintone_color_conversion_d65.a21 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A21 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A22:    
      chromatix_ptr->skintone_color_conversion_d65.a22 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A22 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A23:    
      chromatix_ptr->skintone_color_conversion_d65.a23 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A23 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A31:    
      chromatix_ptr->skintone_color_conversion_d65.a31 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A31 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A32:    
      chromatix_ptr->skintone_color_conversion_d65.a32 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A32 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_A33:    
      chromatix_ptr->skintone_color_conversion_d65.a33 = *(float*)value;
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT)->A33 
        = vfe_convert_float_to_Qfactor(*(float*)value,10);
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_BIAS_Y:   
      chromatix_ptr->skintone_color_conversion_d65.bias_y = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_BIAS_CB:   
      chromatix_ptr->skintone_color_conversion_d65.bias_Cb = *(int16*)value;
      //Todo: Get to VFE
      break;
    case CAMERA_API_BSM_COLOR_CONVERSION_SKIN_D65_BIAS_CR:   
      chromatix_ptr->skintone_color_conversion_d65.bias_Cr = *(int16*)value;
      //Todo: Get to VFE
      break;
    /*************** AFR parameters *****************/
    case CAMERA_API_AFR_ENABLE:
      cam3a_aec_state.afr_enable = *(uint8*)value;
      break;

    case CAMERA_API_AFR_POSSIBLE_FRAME_COUNT:
      camsensor_ptr->num_possible_frame_rates = *(int32*)value;
      break;

    /*************** Anti-baidng parameters *********/
    case CAMERA_API_ANTI_BAND_ENABLE:
      cam3a_aec_state.antibanding_enable = *(uint8*)value;
      break;
/*lint -save -e732*/
    case CAMERA_API_ANTI_BAND_PIXEL_CLK:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      camsensor_ptr->pixel_clock = *(int32*)value;
      break;

    case CAMERA_API_ANTI_BAND_PIXEL_CLK_PER_LINE:
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_aec_reconfig = TRUE;
      camsensor_ptr->pixel_clock_per_line = *(int32*)value;
      break;
/*lint -restore*/
    /*************** defect pixel correction ********/
    case CAMERA_API_DEFECT_PIXEL_COR_ENABLE:
      camsensor_ptr->defectPixelCorrection.enable = *(uint8*)value;
      break;
    case CAMERA_API_DEFECT_PIXEL_COR_THRESHOLD_MIN:
      camsensor_ptr->defectPixelCorrection.minThreshold = *(uint16*)value;
      break;
    case CAMERA_API_DEFECT_PIXEL_COR_THRESHOLD_MAX:
      camsensor_ptr->defectPixelCorrection.maxThreshold = *(uint16*)value;
      break;
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
    /*************** 3x3 luma filter ***************/
    case CAMERA_API_3X3_LUMA_ENABLE:
      camsensor_ptr->f3x3LumaFilter.enable = *(uint8*)value;
      break;
    case CAMERA_API_3X3_LUMA_HW_OR_SW:
#ifndef QDSP_MODULE_VFE25_DEFINED
      vfe_hw_features.filter_3x3_luma = *(uint8*)value;
#else
#error code not present
#endif /* ! QDSP_MODULE_VFE25_DEFINED*/
      break;
    /*************** 3x3 h/w ASF parameters ******************/
    case CAMERA_API_3X3_ASF_ENABLE:
#ifndef QDSP_MODULE_VFE25_DEFINED		
      vfe_hw_features.filter_3x3_asf = *(uint8*)value;
      vfe_sw_features.enable_hw_filter_3x3_asf = *(uint8*)value;
#else
#error code not present
#endif /* !QDSP_MODULE_VFE25_DEFINED */	 
      break;
#ifndef QDSP_MODULE_VFE25_DEFINED
    case CAMERA_API_3X3_ASF_EDGE_FILTER:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ASF_FILTER_SIZE; i++)
          {
	    for (j = 0; j < ASF_FILTER_SIZE; j++)
            {   
              chromatix_ptr->chromatix_adaptive_spatial_filter.edge_filter [ i ][ j ] = ((float *) value)[(i*ASF_FILTER_SIZE) + j];
	    }
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ASF_FILTER_SIZE * ASF_FILTER_SIZE)
      {
        chromatix_ptr->chromatix_adaptive_spatial_filter.edge_filter[table_index / ASF_FILTER_SIZE][table_index % ASF_FILTER_SIZE] = *(float *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_update_asf(camsensor_ptr,VFE_PREVIEW);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_update = TRUE;
      break;
    case CAMERA_API_3X3_ASF_NOISE_FILTER:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ASF_FILTER_SIZE; i++)
          {
	    for (j = 0; j < ASF_FILTER_SIZE; j++)
            {   
              chromatix_ptr->chromatix_adaptive_spatial_filter.noise_filter [ i ][ j ] = ((float *) value)[(i*ASF_FILTER_SIZE) + j];
	    } 
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ASF_FILTER_SIZE * ASF_FILTER_SIZE)
      {
        chromatix_ptr->chromatix_adaptive_spatial_filter.noise_filter[table_index / ASF_FILTER_SIZE][table_index % ASF_FILTER_SIZE] = *(float *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_update_asf(camsensor_ptr,VFE_PREVIEW);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_update = TRUE;
      break;
    case CAMERA_API_3X3_ASF_E_THRESHOLD:
      chromatix_ptr->chromatix_adaptive_spatial_filter.edge_threshold = *(uint16*)value;
      vfe_update_asf(camsensor_ptr,VFE_PREVIEW);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_update = TRUE;
      break;
    case CAMERA_API_3X3_ASF_N_THRESHOLD:
      chromatix_ptr->chromatix_adaptive_spatial_filter.noise_threshold = *(uint16*)value;
      vfe_update_asf(camsensor_ptr,VFE_PREVIEW);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_update = TRUE;
      break;
    case CAMERA_API_3X3_ASF_E_DETECTION_FLAG:
      chromatix_ptr->chromatix_adaptive_spatial_filter.edge_detection_flag = *(uint8*)value;
      vfe_update_asf(camsensor_ptr,VFE_PREVIEW);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_update = TRUE;
      break;
#endif /* !QDSP_MODULE_VFE25_DEFINED */
#if 0 //TODO: Updated values to be reflected in services
    case CAMERA_API_3X3_ASF_LOW_LIGHT_EDGE_FILTER:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ASF_FILTER_SIZE; i++)
          {
            for (j = 0; j < ASF_FILTER_SIZE; j++)
            {   
              chromatix_ptr->chromatix_low_light_asf.edge_filter [ i ][ j ] = ((float *) value)[(i*ASF_FILTER_SIZE) + j];
            }
          }
          //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ASF_FILTER_SIZE * ASF_FILTER_SIZE)
      {
        chromatix_ptr->chromatix_low_light_asf.edge_filter[table_index / ASF_FILTER_SIZE][table_index % ASF_FILTER_SIZE] = *(float *)value;
        //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_3X3_ASF_LOW_LIGHT_NOISE_FILTER:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ASF_FILTER_SIZE; i++)
          {
            for (j = 0; j < ASF_FILTER_SIZE; j++)
            {   
              chromatix_ptr->chromatix_low_light_asf.noise_filter [ i ][ j ] = ((float *) value)[(i*ASF_FILTER_SIZE) + j];
            }
          }
          //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ASF_FILTER_SIZE * ASF_FILTER_SIZE)
      {
        chromatix_ptr->chromatix_low_light_asf.noise_filter[table_index / ASF_FILTER_SIZE][table_index % ASF_FILTER_SIZE] = *(float *)value;
        //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_3X3_ASF_LOW_LIGHT_E_THRESHOLD:
      chromatix_ptr->chromatix_low_light_asf.edge_threshold = *(uint16*)value;
      break;
    case CAMERA_API_3X3_ASF_LOW_LIGHT_N_THRESHOLD:
      chromatix_ptr->chromatix_low_light_asf.noise_threshold = *(uint16*)value;
      break;
    case CAMERA_API_3X3_ASF_LOW_LIGHT_E_DETECTION_FLAG:
      chromatix_ptr->chromatix_low_light_asf.edge_detection_flag = *(uint8*)value;
      break;
#endif
    /*************** chroma suppression ************/
    case CAMERA_API_CHRO_SUP_ENABLE:
      camctrl_tbl.cs_is_supported = *(uint8*)value;
      break;
    case CAMERA_API_CHRO_SUP_LUMA_THRES_1:
      chromatix_ptr->cs_luma_threshold1 = *(uint8*)value;
      break;
    case CAMERA_API_CHRO_SUP_LUMA_THRES_2: 
      chromatix_ptr->cs_luma_threshold2 = *(uint8*)value;
      break;
    case CAMERA_API_CHRO_SUP_LUMA_THRES_3: 
      chromatix_ptr->cs_luma_threshold3 = *(uint8*)value;
      break;
    case CAMERA_API_CHRO_SUP_LUMA_THRES_4: 
      chromatix_ptr->cs_luma_threshold4 = *(uint8*)value;
      break;
    case CAMERA_API_CHRO_SUP_CHRO_THRES_1: 
      chromatix_ptr->cs_chroma_threshold1 = *(uint8*)value;
      break;
    case CAMERA_API_CHRO_SUP_CHRO_THRES_2:
      chromatix_ptr->cs_chroma_threshold2 = *(uint8*)value;
      break;

    /*************** luma adaptation ***************/
    case CAMERA_API_LUMA_ADAP_ENABLE: 
      camctrl_tbl.la_is_supported = *(uint8*)value;
      break;
    case CAMERA_API_LUMA_ADAP_THRESHOLD:
      chromatix_ptr->la_threshold = *(uint8*)value;
      break;

    /*************** HJR ***************************/
    case CAMERA_API_HJR_ENABLE:
      camctrl_tbl.hjr_bayer_filtering_enable = *(uint8*)value;
      break;
    case CAMERA_API_HJR_MAX_NUM_FRAMES:
      camctrl_tbl.hjr_max_frames = *(uint8*)value;
      chromatix_ptr->max_number_of_frames = *(uint8*)value;
      break;
    case CAMERA_API_HJR_ONE_TO_TWO_OFFSET: 
      chromatix_ptr->one_to_two_frame_hjr_offset = *(uint8*)value;
      break;
    case CAMERA_API_HJR_N_REDUCTION_FLAT: 
      chromatix_ptr->flat_area_noise_reduction_level = *(uint8*)value;
      break;
    case CAMERA_API_HJR_N_REDUCTION_TEXTURE:
      chromatix_ptr->texture_noise_reduction_level = *(uint8*)value;
      break;
    case CAMERA_API_HJR_TEXTURE_THRESHOLD:
      chromatix_ptr->texture_threshold = *(uint8*)value;
      break;
    case CAMERA_API_HJR_K_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < HJR_K_TABLE_SIZE; i++)
          {
            chromatix_ptr->hjr_k_table[i] = ((uint8 *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < HJR_K_TABLE_SIZE)
      {
        chromatix_ptr->hjr_k_table[table_index] = *(uint8 *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_HJR_BAYER_FILTERING_ENABLE:
      chromatix_ptr->hjr_bayer_filtering_enable = *(uint8*)value;
      camctrl_tbl.hjr_bayer_filtering_enable = *(uint8*)value;
      break;
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
    /**************Auto Flicker Detection************/
#if 0 //TODO: Updated values to be reflected in services
    case CAMERA_API_AFD_DETECTION_DATA_STD_THRESH:
      chromatix_ptr->auto_flicker_detection_data.afd_std_threshold = *(float*)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
    case CAMERA_API_AFD_DETECTION_DATA_PCT_THRESH:
      chromatix_ptr->auto_flicker_detection_data.afd_percent_threshold = *(uint8*)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
    case CAMERA_API_AFD_DETECTION_DATA_DIFF_THRESH:
      chromatix_ptr->auto_flicker_detection_data.afd_diff_threshold = *(uint32*)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
    case CAMERA_API_AFD_DETECTION_DATA_FRAME_CT_THRESH:
      chromatix_ptr->auto_flicker_detection_data.afd_frame_ct_threshold = *(uint32*)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
#endif
    /*************** Gamma *************************/
    case CAMERA_API_GAMMA_DEFAULT:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < vfe_gamma_table_size; i++)
          {
            chromatix_ptr->default_gamma_table.arr[i] = ((uint8 *)value)[i];
          }
          need_gamma_reconfig = TRUE;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < vfe_gamma_table_size)
      {
        chromatix_ptr->default_gamma_table.arr[table_index] = *(uint8 *)value;
        need_gamma_reconfig = TRUE;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_GAMMA_LOW_LIGHT: 
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < vfe_gamma_table_size; i++)
          {
            chromatix_ptr->lowlight_gamma_table.arr[i] = ((uint8 *)value)[i];
          }
          need_gamma_reconfig = TRUE;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < vfe_gamma_table_size)
      {
        chromatix_ptr->lowlight_gamma_table.arr[table_index] = *(uint8 *)value;
        need_gamma_reconfig = TRUE;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_GAMMA_OUTDOOR: 
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < vfe_gamma_table_size; i++)
          {
            chromatix_ptr->outdoor_gamma_table.arr[i] = ((uint8 *)value)[i];
          }
          need_gamma_reconfig = TRUE;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < vfe_gamma_table_size)
      {
        chromatix_ptr->outdoor_gamma_table.arr[table_index] = *(uint8 *)value;
        need_gamma_reconfig = TRUE;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break;
    case CAMERA_API_GAMMA_BACK_LIGHT: 
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < vfe_gamma_table_size; i++)
          {
            chromatix_ptr->backlight_gamma_table.arr[i] = ((uint8 *)value)[i];
          }
          need_gamma_reconfig = TRUE;
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < vfe_gamma_table_size)
      {
        chromatix_ptr->backlight_gamma_table.arr[table_index] = *(uint8 *)value;
        need_gamma_reconfig = TRUE;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      break; 
    case CAMERA_API_GAMMA_SOLARIZED: 
      chromatix_ptr->solarized_gamma_reflection_point = *(uint8 *)value;
      vfe_init_gamma_table (camsensor_ptr);
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig = TRUE;
      break;

    /*************** Black Level *************************/
    case CAMERA_API_BLACK_LEVEL_NORMAL_LIGHT_OFFSET: 
      chromatix_ptr->black_level_offset = *(uint16 *)value;
      cam3a_awb_state.black_level = *(uint16 *)value;
      break;
#if 0 //TODO: Updated values to be reflected in services	  
    case CAMERA_API_BLACK_LEVEL_LOW_LIGHT_OFFSET: 
      chromatix_ptr->black_level_offset_per_low_light = *(uint16 *)value;
      //Todo: copy to internal structures if necessary and set apply flag(s) if necessary
      break;
#endif	  
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
    /*************** Lens Rolloff *************************/
#ifndef QDSP_MODULE_VFE25_DEFINED
    case CAMERA_API_ROLLOFF_TABLE_TL84_CX: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].cx = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_CY: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].cy = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_WIDTH: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].width = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_HEIGHT: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].height = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_NUM_INTERVALS: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].N = *(uint8 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_RADIUS_SQUARE_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].radius_square_table[i] = ((uint32 *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].radius_square_table[table_index] = *(uint32 *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_RED_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].red_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].red_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_GREEN_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].green_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].green_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_TL84_BLUE_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].blue_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].blue_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_CX: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].cx = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_CY: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].cy = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_WIDTH: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].width = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_HEIGHT: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].height = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_NUM_INTERVALS: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].N = *(uint8 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_RADIUS_SQUARE_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].radius_square_table[i] = ((uint32 *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].radius_square_table[table_index] = *(uint32 *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_RED_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].red_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].red_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_GREEN_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].green_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].green_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_A_BLUE_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].blue_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].blue_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_CX: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].cx = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_CY: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].cy = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_WIDTH: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].width = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_HEIGHT: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].height = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_NUM_INTERVALS: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].N = *(uint8 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_RADIUS_SQUARE_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].radius_square_table[i] = ((uint32 *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].radius_square_table[table_index] = *(uint32 *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_RED_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].red_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].red_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_GREEN_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].green_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].green_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_D65_BLUE_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].blue_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].blue_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_CX: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].cx = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_CY: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].cy = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_WIDTH: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].width = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_HEIGHT: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].height = *(uint16 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_NUM_INTERVALS: 
      chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].N = *(uint8 *)value;
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_RADIUS_SQUARE_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].radius_square_table[i] = ((uint32 *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].radius_square_table[table_index] = *(uint32 *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_RED_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].red_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].red_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_GREEN_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].green_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].green_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
    case CAMERA_API_ROLLOFF_TABLE_LOW_LIGHT_BLUE_CORRECTION_TABLE:
      if (table_index < 0)
      {
        if(table_index == -1)
        {
          for (i = 0; i < ROLLOFF_TABLE_SIZE; i++)
          {
            chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].blue_cf_table[i] = ((double *)value)[i];
          }
        }
        else
        {
          MSG_ERROR("Invalid table index, less than -1. For whole table table_index should be -1", 0, 0, 0);
          return CAMERA_NOT_SUPPORTED;
        }
      }
      else if (table_index < ROLLOFF_TABLE_SIZE)
      {
        chromatix_ptr->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].blue_cf_table[table_index] = *(double *)value;
      }
      else
      {
        MSG_ERROR("Invalid table index", 0, 0, 0);
        return CAMERA_NOT_SUPPORTED;
      }
      vfe_init_rolloff_tables(camsensor_ptr);	  
      if(phase != CAMERA_TUNE_PHASE_INIT)
        need_vfe_reconfig= TRUE;
      break;
#endif /* !QDSP_MODULE_VFE25_DEFINED */
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */	  
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
    default:
      return CAMERA_NOT_SUPPORTED;
  }

  return CAMERA_SUCCESS;
} /* camera_parms_write */
/*lint -restore*/
/*===========================================================================

FUNCTION      CAMERA_PARMS_APPLY

DESCRIPTION
  This API applies changes that are better done after all changes are made
  instead of after each individual change.

RETURN VALUE

===========================================================================*/
camera_ret_code_type camera_parms_apply ( void )
{
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  if(need_awb_reconfig)
  {
    camera_awb_config (camsensor_ptr);
    need_awb_reconfig = FALSE;
  }

  if(need_aec_reconfig)
  {
    camera_aec_config (camsensor_ptr);
    need_aec_reconfig = FALSE;
  }

  if(need_gamma_reconfig)
  {
    vfe_init_gamma_table (camsensor_ptr);
    need_gamma_reconfig = FALSE;
  }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

  if(need_af_reconfig)
  {
    (void)camera_af_config(camsensor_ptr, CAMERA_AF_MODE_UNCHANGED, (int)AF_PROCESS_DEFAULT);
    need_af_reconfig = FALSE;
  }

 if(need_vfe_update)
 {
      camera_update_vfe = TRUE;
      need_vfe_update = FALSE;
 }
 if(need_vfe_reconfig)
 {
       camera_vfe_control_and_status.reconfigure_vfe = TRUE;
      need_vfe_reconfig = FALSE;
 }
  return CAMERA_SUCCESS;
} /* camera_parms_apply */

/* End of External API. 
 * New static functions should go into INTERNAL API section.
 */


/*============================================================================
                        INTERNAL API DEFINITIONS
============================================================================*/
/* End of Internal API. 
 * New global functions should go into EXTERNAL API section.
 */

#endif /* FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H */
#endif /* FEATURE_CAMERA */


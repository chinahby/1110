#ifndef CAMERA_AEC_CORE_H
#define CAMERA_AEC_CORE_H
/*=============================================================================
FILE:       camera_aec_core.h

SERVICES:   Core Auto Exposure Control Header File

DESCRIPTION: 

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/aec/main/latest/src/camera_aec_core.h#2 $ $DateTime: 2008/11/14 10:16:02 $ $Author: cgera $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 02/26/07   klee     Removed CAM3A_AFR_COMPENSATION_FOR_FPS_CHAHGE (23)
 01/05/07   klee     Fixed flickering issue with AEC digital gain
 11/03/06   klee     Beautified the code by indenting and removing special characters
 11/02/06   klee     Added AEC Digital gain implementation
 09/15/06   klee     Cleaned up the lint errors
 07/26/06   klee     Moved in AEC core algorithm from camera_aec
 07/19/06   klee     Initial code

==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include "camerai.h"
#include "camsensor.h"

/*==============================================================================
  FEATURES
==============================================================================*/

/*==============================================================================
  CONSTANTS and MACROS
==============================================================================*/
#define CAM3A_AFR_STABLILITY_SEC 1

/*==============================================================================
  DATA TYPES
==============================================================================*/
typedef struct
{
  uint16    gain;         /* Sensor register gain value */
  uint32    line_count;   /* Line count exposure time */
  float     digital_gain; /* VFE digital gain applied */
} aec_output_type;

typedef struct
{
  boolean apply_forced_input;
  uint32  forced_line_count;
  float   forced_sensor_gain;
} aec_forced_input_type;

typedef struct
{
  uint16  snapshot_gain;
  uint32  snapshot_linecount;
  boolean apply_shapshot_fps;
  uint16  snapshot_fps;
  float   digital_gain_for_snapshot;
} aec_output_type_per_iso;

typedef enum
{
  AFR_SKIP_FRAME = 0,
  AFR_FAILED,
  AFR_SUCCESS
} afr_status_type;

typedef struct
{
  afr_status_type afr_status;
  int32           fps_adjustment;
} afr_output_type;

/*==============================================================================
  EXTERNAL VARIABLES
==============================================================================*/

/*==============================================================================
  EXTERNAL API DECLARATIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_MEM_ALLOCATE

DESCRIPTION
  allocate memory for AEC

CALLERS:
  camera_aec_mem_allocate

RETURN VALUE
  None

===========================================================================*/
boolean camera_aec_core_mem_allocate (void);

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_MEM_DEALLOCATE

DESCRIPTION
  free the memory allocated for AEC

CALLERS:
  camera_aec_mem_deallocate

RETURN VALUE
  None

===========================================================================*/
void camera_aec_core_mem_deallocate (void);

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_INIT

DESCRIPTION
  initiate core AEC algorithm

CALLERS:
  camera_aec_config()

RETURN VALUE
  None

===========================================================================*/
void camera_aec_core_init(camsensor_static_params_type* camsensorParamsPtr);

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_CALCULATE_CURRENT_LUMA

DESCRIPTION
  calculate average luma some for current frame

CALLERS:
  camera_aec_adjust_exposure()

RETURN VALUE
  None

===========================================================================*/
uint8 camera_aec_core_calculate_current_luma(
  int32 numRegions,
/* Num regions in input sum of luma array  */
  uint32 * sumLumaArray,
/* Input sum of luma array            */
  int32 pixelsPerRegion 
/* num pixels used for exposure stats in each region */
  );

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_RUN_ADJUST_EXPOSURE_ALGORITHM

DESCRIPTION
  core AEC algorithm

CALLERS:
  camera_aec_adjust_exposure

RETURN VALUE
  aec_output_type

===========================================================================*/
aec_output_type camera_aec_core_run_adjust_exposure_algorithm(uint8 current_vfe_luma);

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_RUN_AUTO_FRAME_RATE_ALGORITHM

DESCRIPTION
  Implements AFR (Auto Frame Rate) generically from sensor params.

GLOBALS
  cam3a_aec_state - uses varibles in this struct

DEPENDENCIES
  on external variables:
  cam3a_aec_state - global structure holding the AEC state
  On: cam3a_aec_state.afr_max_fps and cam3a_aec_state.afr_min_fps
    1) Set in the CAMERA_PARM_PREVIEW_FPS parameter
    2) Must be no greater than the max sensor FPS (defined by sensor clock)
    3) Must be no less than the min sensor FPS (defined by sensor clock)
    4) Must be in Q8 format (i.e. 15 FPS = (15 * 256)
    5) If either are zero, this function does nothing
    6) If both are the same (non zero) then we adjust the current
      FPS down step by step to meet the new fixed rate.
    7) Other wise, we implmenent true "Auto" frame rate mode.
 "use_in_auto_frame_rate" - this is not supported yet from the driver

RETURN VALUE
  afr_output_type

SIDE EFFECTS
  None

===========================================================================*/
afr_output_type camera_aec_core_run_auto_frame_rate_algorithm (
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  );

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_RUN_ADJUST_EXPOSURE_ALGORITHM_PER_ISO

DESCRIPTION
  core AEC algorithm

CALLERS:
  camera_aec_adjust_exposure

RETURN VALUE
  aec_output_type

===========================================================================*/
aec_output_type_per_iso camera_aec_core_run_adjust_exposure_algorithm_per_iso(
  camsensor_snapshot_config_type * snapshot_config_ptr,
  aec_forced_input_type forced_input);

void camera_aec_core_update_indoor_outdoor_indices 
  (
  /* Index into the supported frame rates table */
  int16 index
  );

#endif /* CAMERA_AEC_CORE_H */

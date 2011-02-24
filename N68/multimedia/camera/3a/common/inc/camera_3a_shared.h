#ifndef CAMERA_3A_SHARED_H
#define CAMERA_3A_SHARED_H

/*=============================================================================
FILE:       camera_3a_shared.h

SERVICES: 

DESCRIPTION: 

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/common/main/latest/inc/camera_3a_shared.h#6 $ $DateTime: 2009/04/13 23:57:33 $ $Author: c_caluru $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
  04/07/09  cg       Re-Enabling Feature FEATURE_CAMERA_BURST_MODE 
 12/24/08   cg       Changes for AEC Multishot
 11/03/08   cg       Added support for Ez Tune1.0,3A 1.4 and 0x203 header
 06/17/08   ww       Added camera_3a_shared_get_illuminant() for 3 rolloff tables for Micron
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 12/19/06   klee     Added fix on CR 108540 - compiling error under FEATURE_CAMERA_YCBCR_ONLY
 11/03/06   klee     Beautified the code by indenting and removing special characters
 11/02/06   klee     Added camera_3a_shared_unpack/pack_ColorCorrection mathods
 10/20/06   jv      Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 09/15/06   klee     Cleaned up the lint errors
 09/08/06   klee     Updated min/max macro to be CAM3A_MIN/CAM3A_MAX for 3a usage
 09/08/06   klee     Added camera_3a_shared_set/get_aec_awb_delay_count()
 07/12/06   dle      Re-do profiling.
 06/14/06   jv       Added camera_3a_shared_set_parm() 
                     and camera_3a_shared_get_parm()
 05/22/06   klee     Added camera_3a_select_rolloff_light()
 05/02/06   klee     Moved the definition of VfeStatOutStruct to camerai.h
 04/12/06   klee     Added 2 more APIs to support adjustable bounding box for AWB
 03/20/06   klee     cam3a.c and cam3a.h don't exist in 3a layer anymore.
                     Moved in 
                       cam3a_debug_type structure 
                       cam3a_get_debug_params()
                       cam3a_change_current_option()
                       cam3a_get_current_option()
                       cam3a_show_timing()
                       from cam3a.h
                     Updated
                       camera_3a_shared_mem_allocate()
                       camera_3a_shared_mem_deallocate()
                       camera_3a_shared_config()
                       camera_3a_shared_parse_regions()
                       as internal static APIs. (moved to camera_3a_shared.c)
                     Added
                       camera_3a_init()
                       camera_3a_mem_allocate()
                       camera_3a_mem_deallocate()
                       camera_3a_config()
                       camera_3a_parse_and_implement_aec_awb()
                       as external APIs
 02/24/06   ven      Added support for profiling awb and aec
 12/01/05   klee     fix lint error for 4.3 release
 10/04/05   klee     Initial code transfered from cam3a 

==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include <comdef.h>
#include <math.h>
#include "msg.h"
#include "vfe.h"
#include "camera.h"
#include "camerai.h"
#include "camsensor.h"
#include "clk.h"
#include "camera_aec.h"
#include "camera_awb.h"
#include "camera_af.h"
#include "assert.h"

/*==============================================================================
  FEATURES
==============================================================================*/

/*==============================================================================
  CONSTANTS and MACROS
==============================================================================*/
/* List of flags to indicate which VFE Stat commands 
 * we are waiting for 
 */
#define VFE_STAT_TYPE_ONE_IN_PROGRESS   0x1
#define VFE_STAT_TYPE_TWO_IN_PROGRESS   0x2
#define VFE_STAT_TYPE_THREE_IN_PROGRESS 0x4

//no RAW
#define VFE_NUM_BYTES_PER_REGION    16
/* 256 */
#define MAX_VFE_REGIONS             (16*16)

#ifdef FEATURE_CAMERA_YCBCR_ONLY
#define camera_3a_parse_and_implement_aec_awb(num_pixels_per_region_aec, camsensorParamsPtr)
#define camera_3a_select_gamma_table(op_mode) GAMMA_TABLE_INVALID
#define camera_3a_select_rolloff_light() GAMMA_TABLE_INVALID
#define camera_3a_shared_set_aec_awb_delay_count(num_frames, mode) 
#define camera_3a_shared_get_aec_awb_delay_count() 0
#define camera_3a_shared_update_black_level() 0
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*==============================================================================
  DATA TYPES
==============================================================================*/
#ifdef FEATURE_CAMERA_3A_PROFILING
typedef struct
{
  uint32 total_frames;
  int    total_processing_time;
}cam3a_timing_type;
#endif /* FEATURE_CAMERA_3A_PROFILING */

#ifdef FEATURE_CAMERA_MOBICAT
typedef struct
{
  /* ----------- Auto Exposure Stats ------------ */
  uint8 luma_target;            /* Luma value to try and reach */
  uint8 current_luma;           /* current luma value */
  int16 exp_index;              /* Current index into sensor's exp table */
  int32 luma_settled_cnt;       /* Increments when adjust_luma is called
                                  but there is not adjustment made */
  /* Auto Exposure Control algorithm used */
  camera_auto_exposure_mode_type aec_algo;

  /* AEC aggressiveness selection (tolerance) */
  cam3a_aggressiveness_type aec_aggressiveness;

  /* ----------- Auto Focus Stats ------------ */
  int32 elapsed_frame_cnt; /* How many frames since AF operation was requested */
  int32 position;     /* Current sensor position */
  cam3a_af_state_enum_type state;
  cam3a_af_process_enum_type process_type;


  /* ----------- Auto White Balance Stats ------------ */
  int32 agw_sample_decision; /* Adv Grey World illuminant choice */

  /* For WB history to provide a smooth update transition */
  float prev_wb_rgain;
  float prev_wb_ggain;
  float prev_wb_bgain;

} cam3a_debug_type;
#endif /* FEATURE_CAMERA_MOBICAT */

/*==============================================================================
  EXTERNAL VARIABLES
==============================================================================*/
/* Allocate mem for VfeStatOutStruct for AEC/AWB data */
/* This struct is used by the AEC/AWB algorithms directly */
extern VfeStatOutStruct * vfeStatStruct;

/*==============================================================================
  EXTERNAL API DECLARATIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_3A_SHARED_GET_TIME_MS

DESCRIPTION
   
CALLERS:

DEPENDENCIES
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
uint32 camera_3a_shared_get_time_ms(void);

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      CAMERA_3A_SHARED_UPDATE_BLACK_SIZE

DESCRIPTION
  modify black_level bit size to be 8 from 10
   
CALLERS:

DEPENDENCIES
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
uint8 camera_3a_shared_update_black_level(int16);
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION      CAMERA_3A_INIT

DESCRIPTION
   Called on phone power up.

CALLERS:
  camera_svcs_init ()


DEPENDENCIES
  None

INPUT
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_3a_init(void);

/*===========================================================================

FUNCTION     CAMERA_3A_MEM_DEALLOCATE

DESCRIPTION:

CALLERS:
  camera_svcs_start () & camera_process_stop_cleanup()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

=========================================================================== */
void camera_3a_mem_deallocate(void);

/*===========================================================================

FUNCTION      CAMERA_3A_START

DESCRIPTION
   Initializes global and locally used structure.  Provides
   tuning parameters for specific implementations

CALLERS:
  camera_svcs_start ()

DEPENDENCIES
  None

GLOBALS

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_3a_start(
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer */
  );


#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      CAMERA_3A_PARSE_AND_IMPLEMENT_AEC_AWB

DESCRIPTION
   Do inital parse of AEC AWB stats comming back from VFE
   
CALLERS:
  camera_process_qdsp_ae_awb_stats_complete_msg ()

DEPENDENCIES
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_3a_parse_and_implement_aec_awb(
  uint32 num_pixels_per_region_aec,
/* num pixels used for exposure stats in each region */
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer           */
  );
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      CAMERA_3A_SELECT_GAMMA_TABLE

DESCRIPTION
  select gamma table based on special effect and operation type
   
CALLERS:

DEPENDENCIES
  None

RETURN VALUE
  selected gamma table type
LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
vfe_gamma_table_type camera_3a_select_gamma_table(camsensor_op_mode_type op_mode);
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      CAMERA_3A_SELECT_ROLLOFF_LIGHT

DESCRIPTION
  select rolloff table based
   
CALLERS:

DEPENDENCIES
  None

RETURN VALUE
  selected rolloff table type
LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
camsensor_chromatix_rolloff_light_type camera_3a_select_rolloff_light(void);
#endif/* CAMERA_USES_CHROMATIX_0x203 */

#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION      CAMERA_3A_SHARED_SET_PARM

DESCRIPTION
  set camera_parm_info in 3a

CALLERS:
  CAMERA_SVCS_SET_PARAM

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_3a_shared_set_parm
  (
  camera_parm_type  id,
  int32             parm
  );

/*===========================================================================

FUNCTION      CAMERA_3A_SHARED_GET_PARM

DESCRIPTION
  provide camera_parm_info in aec

CALLERS:
  CAMERA_SVCS_GET_PARAM

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_3a_shared_get_parm
  (
  camera_parm_type       id,
  camera_parm_info_type *parm
  );

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION: CAMERA_3A_SHARED_SET_AEC_AWB_DELAY_COUNT
          former - CAMERA_AEC_AWB_DELAY

DESCRIPTION: 
  Defers AEC/AWB processing by specified number of frames.

CALLERS:
  CAMERA_SVCS_SET_PARM
  CAMERA_PROCESS_QDSP_OUTPUT1_MSG
  CAMERA_PROCESS_QDSP_UPDATE_COMPLETE_MSG
  CAMERA_PROCESS_QDSP_AE_AWB_STATS_COMPLETE_MSG
  CAMERA_CONFIG_VFE

SIDE EFFECTS: 

RETURN VALUE: 
  None.

============================================================================*/
void camera_3a_shared_set_aec_awb_delay_count
  (
  uint8 num_frames,
  camera_aec_awb_delay_type mode
  );
/*==============================================================================
FUNCTION: CAMERA_3A_PARSE_AND_IMPLEMENT_AEC_AWB_FOR_MULTISHOT
          

DESCRIPTION: 
  Do inital parse of AEC AWB stats comming back from VFE in case of multishot

CALLERS:
  
  CAMERA_PROCESS_QDSP_UPDATE_COMPLETE_MSG
  

SIDE EFFECTS: 

RETURN VALUE: 
  None.

============================================================================*/
#if defined  FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* defined  FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT */

#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION: CAMERA_3A_SHARED_GET_AEC_AWB_DELAY_COUNT

DESCRIPTION: 
  provide current camera_aec_awb_delay_cnt.

CALLERS:
  CAMERA_PROCESS_QDSP_OUTPUT1_MSG
  CAMERA_PROCESS_QDSP_AE_AWB_STATS_COMPLETE_MSG
  CAMERA_AEC_AWB_READY

SIDE EFFECTS: 

RETURN VALUE: 
  None.

============================================================================*/
uint8 camera_3a_shared_get_aec_awb_delay_count(void);
/*===========================================================================

FUNCTION:  CAMERA_3A_SHARED_GET_ILLUMINANT

DESCRIPTION: 
  returns current white balance

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/

cam3a_auto_white_balance_illuminant_type camera_3a_shared_get_illuminant(void);

#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#endif /* CAMERA_3A_SHARED_H */


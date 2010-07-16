/*=============================================================================
FILE:       camera_3a_shared.c

SERVICES:   

DESCRIPTION: 

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/common/main/latest/src/camera_3a_shared.c#7 $ $DateTime: 2009/04/22 23:29:28 $ $Author: c_caluru $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 04/22/09   cg       Update ASF for Preview as well.
 04/07/09   cg       Re-Enabling Feature FEATURE_CAMERA_BURST_MODE
 12/24/08   cg       Changes for AEC Multishot    
 11/03/08   cg       Ez Tune 1.0 , 3A1.4 and 0x203 header support
 06/17/08   ww       Added camera_3a_shared_get_illuminant() for 3 rolloff tables for Micron
 03/30/07   klee     Clean-up lint errors due to un-matched data type in 3a
 03/28/07   klee     Enhancement on gamma table interpolation
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 02/21/07   klee     Fixed additional lint errors
 02/20/07   klee     Fixed all lint errors (critical, high, medium, low and lint)
 12/19/06   klee     Added fix on CR 108540 - compiling error under FEATURE_CAMERA_YCBCR_ONLY
 11/03/06   klee     Beautified the code by indenting and removing special characters
 11/02/06   klee     Added camera_3a_shared_unpack/pack_ColorCorrection mathods
 11/02/06   klee     Enhance BSM initializing
 11/02/06   klee     Update BSM initializing for WB
 10/20/06   jv      Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 09/27/06   klee     Applied FEATURE_CAMERA_3A_USE_SGW_ALGO on SGW algorithm
 09/15/06   klee     Cleaned up the lint errors
 09/13/06   klee     Use outdoor_gamma_index for outdoor_gamma_table
 09/08/06   klee     Added camera_3a_shared_set/get_aec_awb_delay_count()
 09/01/06   kadhir   Removed unwanted Cross Compilation Dependency 
                     When qcamcorder Feature Disabled 
 08/31/06   klee     Enhance outdoor BSM support by D65 wb initializing
 08/31/06   klee     Bug fix on previous and current wb maintaining
 08/23/06   klee     Fixed no stats problem for AF in case of YUV
 08/18/06   klee     Fixed critical and high lint errors
 08/15/06   jv       Changed name of camera_get_bestshot_config to 
                     camera_svcs_bestshot_get_config
 08/11/06   klee     Cleaned up the code regarding tuning variables
                     - AEC
 08/04/06   dl       Removed extern variable camctrl_tbl.
 07/19/06   dle      Parsing uses stats in DSP memory. This allows VFE FW from
                     DME'ing the stats to Arm memory.
 07/18/06   dle      Fixed AEC/AWB stats numRegion setting bug.
                     Make VFE_4X4_SUPPRT a default.
                     Featurize for VFE_16X16_SUPPORT.
 07/16/06   klee     Added bestshot support - gamma selection, set_parm
 07/12/06   dle      Redo profiling.
 06/22/06   klee     update bounding box adjusting functions by removing numberOfRegions update
 06/14/06   jv       Added camera_3a_shared_set_parm() 
                     and camera_3a_shared_get_parm()
 05/27/06   dle      Use half of outdoor index as index for outdoor gamma.
 05/22/06   klee     Added camera_3a_select_rolloff_light()
 04/12/06   klee     Added 2 more APIs to support adjustable bounding box for AWB
 03/20/06   klee     cam3a.c and cam3a.h don't exist in 3a layer anymore.
                     Moved in 
                       cam3a_processed_frames (variable)
                       cam3a_sensor_params_ptr (variable)
                       cam3a_get_debug_params()
                       cam3a_change_current_option()
                       cam3a_get_current_option()
                       cam3a_show_timing()
                       from cam3a.c
                     Updated
                       camera_3a_shared_mem_allocate()
                       camera_3a_shared_mem_deallocate()
                       camera_3a_shared_config()
                       camera_3a_shared_parse_regions()
                       to be internal static APIs.
                     Added
                       camera_3a_init()
                       camera_3a_mem_allocate()
                       camera_3a_mem_deallocate()
                       camera_3a_config()
                       camera_3a_parse_and_implement_aec_awb()
                       as external APIs
 02/24/06   ven      Added support for profiling awb and aec
 02/02/06   dle      Temporily disable 16x16 so make sure check 8x8 first.
 12/14/05   ven      Added support for VFE 4X4 Statistics collection.
 12/01/05   klee     fix lint error for 4.3 release
 10/17/05   klee     clean up the lint error except
                     e737 on camera_3a_shared_parse_regions()
 10/04/05   klee     Initial code transfered from cam3a 

==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "target.h"
#include "customer.h"
#ifndef CAMERA_USES_SOFTDSP
#include "camera_3a_shared.h"
#include "qdspext.h"
#include "camctrl.h"
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203*/

/*==============================================================================
  INTERNAL VARIABLES
==============================================================================*/
/* Allocate mem for VfeStatOutStruct for AEC/AWB data */
/* This struct is used by the AEC/AWB algorithms directly */
/* This is quite large, malloc this one */
VfeStatOutStruct * vfeStatStruct = NULL;

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/* Number of frames AEC/AWB processing will be deferred */
static uint8   camera_aec_awb_delay_cnt;
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifdef FEATURE_CAMERA_3A_PROFILING
static int cam3a_processed_frames = 0;
#endif /* FEATURE_CAMERA_3A_PROFILING */

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
extern camsensor_static_params_type  camsensor_static_params [];
extern uint8 camera_asi;

/*==============================================================================
  INTERNAL API DECLARATIONS
==============================================================================*/
/*===========================================================================

FUNCTION        CAMERA_3A_SHARED_MEM_ALLOCATE

DESCRIPTION:

CALLERS:
  camera_3a_mem_allocate ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

=========================================================================== */
static boolean camera_3a_shared_mem_allocate(void);

/*===========================================================================

FUNCTION     CAMERA_3A_SHARED_MEM_DEALLOCATE

DESCRIPTION:

CALLERS:
  camera_3a_mem_deallocate ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

=========================================================================== */
static void camera_3a_shared_mem_deallocate(void);

/*==============================================================================
  INTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION        CAMERA_3A_SHARED_MEM_ALLOCATE

DESCRIPTION:

CALLERS:
  camera_3a_mem_allocate ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

=========================================================================== */
static boolean camera_3a_shared_mem_allocate (void)
{
  CAM_Free( vfeStatStruct );
  vfeStatStruct = CAM_Malloc( sizeof(VfeStatOutStruct) );

  if(vfeStatStruct == NULL)
  {
    camera_no_memory();
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION     CAMERA_3A_SHARED_MEM_DEALLOCATE

DESCRIPTION:

CALLERS:
  camera_3a_mem_deallocate ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

=========================================================================== */
static void camera_3a_shared_mem_deallocate (void)
{
  CAM_Free( vfeStatStruct );
  vfeStatStruct = NULL;
}

/*==============================================================================
  EXTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_3A_SHARED_GET_TIME_MS

DESCRIPTION
  return system time in millisecond
   
CALLERS:

DEPENDENCIES
  None

RETURN VALUE
  system time in millisecond

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
uint32 camera_3a_shared_get_time_ms(void)
{
  uint32 time[2];
  clk_uptime_ms(time);
  return time[0];
}

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
uint8 camera_3a_shared_update_black_level(int16 black_level)
{
  uint8 tmp;
  tmp = (uint8)(black_level/4);
  return tmp;
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION    CAMERA_3A_INIT

DESCRIPTION
  Memclears global and locally used structure for 3A.
  Called on phone power up.

DEPENDENCIES
  None

INPUT
  None

GLOBALS
  vfeStatDspInput - global structure that is the input to the stat command
  cam3a_aec_state - Auto exposure varibles and settings
  cam3a_awb_state - Auto white balance varibles and settings
  cam3a_af_state - Auto focus varibles and settings

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_3a_init (void)
{
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  camera_aec_init();

  camera_awb_init();
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

  camera_af_init();
} /* camera_3a_init */

/*===========================================================================

FUNCTION    CAMERA_3A_MEM_DEALLOCATE

DESCRIPTION
  Deallocate global and locally used structure that are too
  large to statically allocate

DEPENDENCIES
  None

INPUT

GLOBALS
  vfeStatStruct - formatted output of VFE Statistics command

RETURN VALUE
  None.

LIMITATIONS

SIDE EFFECTS
  Allocates memory, changes pointer references

===========================================================================*/
void camera_3a_mem_deallocate(void)
{
  /* Allocate mem for VfeStatOutStruct for AEC/AWB data */
  /* This struct is used by the AEC/AWB algorithms directly */
  camera_3a_shared_mem_deallocate();
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  camera_awb_mem_deallocate();

  camera_aec_mem_deallocate();
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
} /* camera_3a_mem_deallocate */

/*===========================================================================

FUNCTION    CAMERA_3A_START

DESCRIPTION
  Initializes global and locally used structure for 3A.  Provides
  tuning parameters for specific implementations.  Called on
  Camera Services init (camera_start () )

DEPENDENCIES
  None

INPUT
  camsensor_static_params_type * camsensorParamsPtr -
   Camsensor driver layer config, parameters are sensor specific

GLOBALS
  vfeStatDspInput - global structure that is the input to the stat command
  vfeExpWbStatDspInput - Input struct to VFE DSP to get AE/WB stats out
  vfeAfStatDspInput - Input struct to VFE DSP to get AF stats out
  cam3a_aec_state - Auto exposure varibles and settings
  cam3a_awb_state - Auto white balance varibles and settings
  cam3a_af_state - Auto focus varibles and settings

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_3a_start(
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer */
  )
{
  /* Malloc memory for structure & scratch that are too big to
     fit on the stack or in static memory */
  /* Allocate mem for VfeStatOutStruct for AEC/AWB data */
  /* This struct is used by the AEC/AWB algorithms directly */
  if(!camera_3a_shared_mem_allocate())
    return;

#ifndef FEATURE_CAMERA_YCBCR_ONLY
  if( camsensorParamsPtr->output_format == CAMSENSOR_BAYER )
  {
    if(!camera_awb_mem_allocate())
      return;
    if(!camera_aec_mem_allocate())
      return;
  }

  /* Let the VFE to copy AEC and AWB config from driver, if bayer */
  if( camsensorParamsPtr->output_format == CAMSENSOR_BAYER )
  {
    (void)vfe_copy_aec_awb_config(&vfeExpWbStatDspInput, camsensorParamsPtr);
  }

  if( camsensorParamsPtr->output_format == CAMSENSOR_BAYER )
  {
    camera_aec_config(camsensorParamsPtr);  
    camera_awb_config(camsensorParamsPtr);
  }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifdef FEATURE_CAMERA_3A_PROFILING
  cam3a_processed_frames = 0;
#endif /* FEATURE_CAMERA_3A_PROFILING */
} /* camera_3a_config */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION    CAMERA_3A_PARSE_AND_IMPLEMENT_AEC_AWB

DESCRIPTION
  Do inital parse of AEC AWB stats comming back from VFE
  and call functions to implement AEC and AWB.

DEPENDENCIES
  None

GLOBALS
  vfeExpWbStatDspInput - global structure that is the input to the AEC/AWB
                 stat command
  vfeAfStatDspInput - global structure that is the input to the AF stat
  vfeStatStruct - global structure used to analize 3A VFE output.

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 -e818 vfeConfigPtr parameter is not used */
void camera_3a_parse_and_implement_aec_awb(
  uint32             num_pixels_per_region_aec,
/* num pixels used for exposure stats in each region */
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  )
{
  int32 numRegions;

  numRegions = vfe_parse_regions_per_awb_aec();

  if(numRegions == -1) return;

  if(camsensorParamsPtr->aec_enable)
  {
    /* Extra precaution - do not update exposure if we could be gathering
       AF stats */
    if(!camera_af_is_active())
    {
      camera_aec_implement(
        num_pixels_per_region_aec,
        camsensorParamsPtr,
        numRegions
        );
    }
  }

  camera_awb_implement(
    camsensorParamsPtr,
    numRegions
    );
#ifdef FEATURE_CAMERA_3A_PROFILING
  cam3a_processed_frames++;
  if(cam3a_processed_frames >= 100)
  {
    cam3a_processed_frames = 0;
    camera_aec_show_timing();
    camera_awb_show_timing();
  }
#endif /* FEATURE_CAMERA_3A_PROFILING */

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */

} /* camera_3a_parse_and_implement_aec_awb */

/*===========================================================================

FUNCTION    CAMERA_3A_PARSE_AND_IMPLEMENT_AEC_AWB_FOR_SNAPSHOT

DESCRIPTION
  Do inital parse of AEC AWB stats comming back from VFE
  and call functions to implement AEC and AWB.

DEPENDENCIES
  None

GLOBALS
  vfeExpWbStatDspInput - global structure that is the input to the AEC/AWB
                 stat command
  vfeAfStatDspInput - global structure that is the input to the AF stat
  vfeStatStruct - global structure used to analize 3A VFE output.

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
#if defined  FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE && FEATURE_CAMERA_AEC_DURING_MULTISHOT*/

#endif /* FEATURE_CAMERA_YCBCR_ONLY */
/*lint -restore */

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
vfe_gamma_table_type camera_3a_select_gamma_table(camsensor_op_mode_type op_mode)
{
  vfe_gamma_table_type gamma_table_type;

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
  {
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
 {
   
    if(op_mode != CAMSENSOR_OP_MODE_SNAPSHOT)
    {
      gamma_table_type = GAMMA_TABLE_DEFAULT;
    }
    else
    {
      gamma_table_type = GAMMA_TABLE_INTERPOLATED; 
    }
 }
  } /* end of no bestshot */

  return gamma_table_type;
}
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
camsensor_chromatix_rolloff_light_type camera_3a_select_rolloff_light(void)
#endif /* FEATURE_CAMERA_USES_CHROMATIX_0x201 */
{

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
  camsensor_chromatix_rolloff_light_type rolloff_light_type;
#endif
  

#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
  if(cam3a_awb_state.awb_algo == CAM3A_AWB_SIMPLE_GREY_WORLD_ALGO)
  {
    rolloff_light_type = ROLLOFF_TL84_LIGHT;
  }
  else /* in case of advanced awb algorithm */
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */
  {
    if(cam3a_aec_state.exp_index == (int32)(cam3a_aec_state.num_exposure_table_values-1))
    {
      rolloff_light_type = ROLLOFF_LOW_LIGHT;
    }
    else /* in case of normal lighting condition */
    {
      switch(cam3a_awb_state.agw_sample_decision)
      {
        case CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT:
        case CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1:
        case CAM3A_AGW_AWB_OUTDOOR_CLOUDY:
        case CAM3A_AGW_AWB_DAYLIGHT_HYBRID:
          rolloff_light_type = ROLLOFF_D65_LIGHT;
          break;

        case CAM3A_AGW_AWB_INDOOR_INCANDESCENT:
        case CAM3A_AGW_AWB_HORIZON:
        case CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO:
          rolloff_light_type = ROLLOFF_A_LIGHT;
          break;

        case CAM3A_AGW_AWB_INDOOR_WARM_FLO:
        case CAM3A_AGW_AWB_INDOOR_COLD_FLO:
          rolloff_light_type = ROLLOFF_TL84_LIGHT;
          break;

        default:
          rolloff_light_type = ROLLOFF_TL84_LIGHT;
          break;
      } /* end of switch */
    } /* end of in case of normal lighting condition */
  } /* end of in case of advanced awb algorithm */

  return rolloff_light_type;
}
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
/*lint -save -e715 parm not reference */
camera_ret_code_type camera_3a_shared_set_parm
  (
  camera_parm_type  id,
  int32             parm
  )
{
  camera_ret_code_type ret_val;

  switch(id)
  {
#ifdef FEATURE_CAMERA_BESTSHOT_MODE    
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
    default:
      ret_val = CAMERA_INVALID_PARM;
      break;
  }  

  return ret_val;
}
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_3A_SHARED_GET_PARM

DESCRIPTION
  provide camera_parm_info in 3a

CALLERS:
  CAMERA_SVCS_GET_PARAM

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 -e818 parm not reference */
camera_ret_code_type camera_3a_shared_get_parm
  (
  camera_parm_type       id,
  camera_parm_info_type *parm
  )
{
  camera_ret_code_type ret_val;

  switch(id)
  {
    default:
      ret_val = CAMERA_INVALID_PARM;
      break;
  }

  return ret_val;
}
/*lint -restore */

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
  ) 
{
  uint32        save;

  INTLOCK_SAV (save);

  switch(mode)
  {
    /* initialization */
    case AEC_AWB_DELAY_INIT:
      camera_aec_awb_delay_cnt = 0;

      if(num_frames > camera_aec_awb_delay_cnt)
      {
        camera_aec_awb_delay_cnt = num_frames;
      }
      break;

    case AEC_AWB_DELAY_UPDATE:

      if(num_frames > camera_aec_awb_delay_cnt)
      {
        camera_aec_awb_delay_cnt = num_frames;
      }
      break;

    case AEC_AWB_DELAY_DECR:

      if(camera_aec_awb_delay_cnt > num_frames)
      {
        camera_aec_awb_delay_cnt -= num_frames;
      }
      else
      {
        camera_aec_awb_delay_cnt = 0;
      }
      break;

    default:
      break;
  }

  INTFREE_SAV (save);

  return;
} /* camera_aec_awb_delay */
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
uint8 camera_3a_shared_get_aec_awb_delay_count(void)
{
  return camera_aec_awb_delay_cnt;
}
/*===========================================================================

FUNCTION:  CAMERA_3A_SHARED_GET_ILLUMINANT

DESCRIPTION: 
  returns current white balance

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/
cam3a_auto_white_balance_illuminant_type camera_3a_shared_get_illuminant(void)
{
  return cam3a_awb_state.agw_sample_decision;
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
#endif //#ifndef CAMERA_USES_SOFTDSP
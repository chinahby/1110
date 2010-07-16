/*=============================================================================
 *
FILE:       camera_af.c

SERVICES:   Auto Focus file

DESCRIPTION: 

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/af/main/latest/src/camera_af.c#1 $ $DateTime: 2008/05/09 16:27:59 $ $Author: karenp $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 11/08/07   hap     Fixing RVCT2.2 compiler warnings
 03/30/07   klee     Clean-up lint errors due to un-matched data type in 3a
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 02/28/07   ven      Created a common exit function for AF to streamline the
                     profiling.
 01/10/07   klee     Fixed CR 109551 - AF callback is not working properly
 11/15/06   jn       AF always on without DSP changes.
 11/03/06   ven      Added latency logging for various modules.
 11/03/06   jn       Moved AF window calculation into camera_af.
 10/20/06   jv       Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 10/03/06   jn       Added custcam3a.h to featurize 3A algorithms.
 08/21/06   jn       Lens reset is now controlled by AF not by sensor.
 08/18/06   jn       Split camera_af into camera_af and camera_af_core (algorithm).
 08/11/06   jn       Added sensor ISP based AF support.
 08/09/06   jn       Added missing callbacks and collect end stat support for
                     hill climbing.
 08/04/06   dl       Removed extern variable camctrl_tbl.
 07/26/06   jn       Added support to reset lens after snapshot.
 07/18/06   jn       AF modulization.
 07/03/06   jn       Added Bestshot support.
 06/15/06   jn       Added camctrl flag to determine whether to move lens in
                     camera_drv_task context.
 04/26/06   jn       Added low light AF enhancement.
 04/09/06   jn       Removed auto mode.
 04/10/06   jn       Lens_move_done should be set in graph context.
                     Moved AF stat 0 check into top level.
 03/16/06   jn       Added support for fast exhaustive.
 03/10/06   jn       Updated af_config(). Removed redundant stat updates.
 03/09/06   jn       Renamed some AF states.
 03/01/06   jn       Go to AF_INACTIVE after last lens move is done.
 02/28/06   jn       Added full sweep.
 02/21/06   jn       No need to break up lens movement into small steps.
 02/21/06   dle      Use CAMERA_AF_MODE_MAX as an indication of Do Not Change.
 02/17/06   jn       Lens move is now in camera_drv_task context.
 02/16/06   dle      Linted.
 2/02/05    babakf   Removed tab, made message MED and HIGH vs. FATAL, removed
                     possibility of divistion by zero, made lines less than
                     80 characters, did lint pass.
 1/03/05    babakf   Added code to make sure we do not move lens more steps
                     than frame rate allows.
 1/02/05    babakf   Added code to support normal and macro mode with hard
                     boundary in between.  
 12/03/05   klee     clean-up the code, fix lint errors and check-in for 4.3 release
                     lint error 818 has been simply surpressed for all over the file.
                     **************** the latest AF enhancement ***************
                     - AF exhaustive search has not been changed at all.
                     - AF hill-climbing search has been enhanced
                     - For enhanced AF hill-climbing search,
                       OEMs may need drivers included in 4.3 releases or later
                     **********************************************************
 12/01/05   dli      Add CAMERA_AF_DO_SET_FRAME_DELAY to protect SONY 2MP and 
                     3MP piezo sensors for AF performance
 10/17/05   klee     clean up the lint error except
                     e704 on camera_af_do_process_exhaustive_search()
 10/04/05   klee     Initial code transfered from cam3a 

==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include "custcam3a.h"
#include "camctrl.h"
#ifndef CAMERA_USES_SOFTDSP
#include "camera_3a_shared.h"
#include "camera_af.h"
#include "camera_drv.h"
#include "graph.h"
#include "camera_af_core.h"
#include "camera_svcs.h"

#ifndef FEATURE_CAMERA_DRV_TASK
#error "Need FEATURE_CAMERA_DRV_TASK"
#endif

/*==============================================================================
  INTERNAL VARIABLES
==============================================================================*/

cam3a_af_state_struct cam3a_af_state;

/*==============================================================================
  EXTERNAL VARIABLES
==============================================================================*/
extern camera_snapshot_timing_type camera_snapshot_timing;
extern camera_snapshot_information_type camera_snapshot_information;


/*lint -save -e704, these shifts are okay */
/*lint -save -e713, ok */
/*lint -save -e732, ok */
/*lint -save -e641  ok to have enum be cast as int */
/*lint -save -e64   ok to have enum be cast as int */


/*==============================================================================
  INTERNAL API DECLARATIONS
==============================================================================*/

void camera_af_done(camera_cb_type cb);

/*===========================================================================
 *
 * FUNCTION    CAMERA_AF_DO_MOVE_LENS
 *
 * DESCRIPTION
 *   wrapper to decide whether to move lens in camera_drv_task context
 *
 * DEPENDENCIES
 *
 * RETURN VALUE
 *   None
 *
 * LIMITATIONS
 *
 * SIDE EFFECTS
 *   None
 *
 *===========================================================================*/
void camera_af_do_move_lens
(
  camsensor_move_focus_enum_type direction,
  int32 steps
)
{
  if (camctrl_tbl.change_context)
  {
    camera_drv_queue_focus((void *)0, 0, direction, steps, 0, 0);
    cam3a_af_state.move_lens_done = FALSE;
  }
  else
  {
    camera_af_do_safe_move(direction, steps);
  }
}


/*===========================================================================

FUNCTION    CAMERA_AF_DO_SAFE_MOVE

DESCRIPTION
  This function runs in camera_drv_task context.
  make movement only between near_end and far_end
  num_steps may be adjusted, if exceeds.

DEPENDENCIES

RETURN VALUE
  None

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_af_do_safe_move
(
  camsensor_move_focus_enum_type direction, 
  int32                          num_steps
)
{
  uint32 time_stamp;
  time_stamp = camera_3a_shared_get_time_ms();

  MSG_AFDEBUG("We are at %d, want to move lens %d steps", cam3a_af_state.position,
              num_steps, 0);
  
  if (direction == CAMSENSOR_MOVE_FOCUS_NEAR && 
      ((cam3a_af_state.position - num_steps) < 0))
    num_steps = cam3a_af_state.position;

  if (direction == CAMSENSOR_MOVE_FOCUS_FAR && 
      ((cam3a_af_state.position + num_steps) > cam3a_af_state.infinity_position))
    num_steps = cam3a_af_state.infinity_position - cam3a_af_state.position;

  if (num_steps > 0)
  {
    if (camsensor_move_focus(direction, num_steps) != CAMERA_SUCCESS)
    {
      if (camctrl_tbl.change_context)
      {
        graph_queue_camera_func((uint32)CAMERA_FUNC_MOVE_LENS_DONE,
                                (void *)0, (void *)0, FALSE, 0, 0, 0);
      }
      else
      {
        camera_af_process_lens_move_done(FALSE);
      }
      return;
    }
    else
    {
      cam3a_af_state.stats_moved++;
    
      if (direction == CAMSENSOR_MOVE_FOCUS_NEAR)
        cam3a_af_state.position -= num_steps;
      else
        cam3a_af_state.position += num_steps;
      MSG_AFDEBUG("moving %d steps, takes %d ms, at %d", num_steps, 
                  camera_3a_shared_get_time_ms()-time_stamp, direction);  
#ifdef FEATURE_CAMERA_MOBICAT
      cam3a_af_state.tracing_info_total_movement_lens += num_steps;
#endif

      if (camctrl_tbl.change_context)
      {
        graph_queue_camera_func((uint32)CAMERA_FUNC_MOVE_LENS_DONE,
                                (void *)0, (void *)0, TRUE, 0, 0, 0);
      }
      else
      {
        camera_af_process_lens_move_done(TRUE);
      }
    }
  }
  else
  {
    if (camctrl_tbl.change_context)
    {
      graph_queue_camera_func((uint32)CAMERA_FUNC_MOVE_LENS_DONE,
                              (void *)0, (void *)0, TRUE, 0, 0, 0);
    }
    else
    {
      camera_af_process_lens_move_done(TRUE);
    }
  }
  return;
}

/*===========================================================================

FUNCTION    CAMERA_AF_PROCESS_LENS_MOVE_DONE

DESCRIPTION
  This function takes appropriate actions based on lens movement status.

DEPENDENCIES

RETURN VALUE
  None

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_af_process_lens_move_done
(
  uint32 status
)
{  
  if (status == TRUE)
  {
    if ( cam3a_af_state.skip_frame_delay)
    {
      cam3a_af_state.frame_delay = 0;
      if (cam3a_af_state.state_changed)
      {  
        cam3a_af_state.frame_delay = 1; 
        cam3a_af_state.state_changed = FALSE;
      }
    }
    else if (cam3a_af_state.in_low_light)   /* low light condition */
    {
      cam3a_af_state.frame_delay = CAM3A_AF_FRAME_DELAY - 1;
    }
    else
    {
      cam3a_af_state.frame_delay = CAM3A_AF_FRAME_DELAY;
    }
    cam3a_af_state.move_lens_done = TRUE;
#ifdef FEATURE_CAMERA_MOBICAT
    cam3a_af_state.tracing_info_total_movement_trial++;
#endif
  }
  else
  {
    MSG_AFDEBUG("Lens move failed.", 0, 0, 0);
    cam3a_af_state.step_sensor_failure_cnt++;
    /* bail out if failed to move lens */
    camera_af_do_reset_lens();
    camera_af_done(CAMERA_EXIT_CB_FAILED);
    return;
  }

  if ((cam3a_af_state.state == AF_FOCUSING) ||
      (cam3a_af_state.state == AF_UNABLE_TO_FOCUS) ||
      (cam3a_af_state.state == AF_COLLECT_END_STAT))
  {
    if ((cam3a_af_state.state == AF_FOCUSING) ||
        (cam3a_af_state.state == AF_UNABLE_TO_FOCUS))
    {
      if (cam3a_af_state.collect_end_stat)
      {
        cam3a_af_state.state = AF_COLLECT_END_STAT;
      }
      else
      {
        camera_af_done(CAMERA_EXIT_CB_DONE);
      }
    }
    MSG_AFDEBUG("AF ENDs here - took %d moves to focus lens", cam3a_af_state.stats_moved, 0, 0);
#ifdef FEATURE_CAMERA_MOBICAT
    cam3a_af_state.tracing_info_focusing_time = camera_3a_shared_get_time_ms() -
                                                cam3a_af_state.af_start_time;
    MSG_AFDEBUG("Total Lens movements and Distance and Time are %d, %d, %d", 
                cam3a_af_state.tracing_info_total_movement_trial,
                cam3a_af_state.tracing_info_total_movement_lens,
                cam3a_af_state.tracing_info_focusing_time);
    
#endif
  }
}


/*==============================================================================
  EXTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_AF_INIT

DESCRIPTION
   Memclears global and locally used structure for af.
   Called on phone power up.

CALLERS:
  cam3a_init ()


DEPENDENCIES
  None

INPUT
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_af_init (void)
{
  /* Init AF state struct */
  memset (&cam3a_af_state, 0, sizeof(cam3a_af_state_struct));

  if (camctrl_tbl.af_is_supported)
  {
    cam3a_af_state.is_enabled = TRUE;
  }
  cam3a_af_state.move_lens_done = TRUE;
}

/*===========================================================================

FUNCTION      CAMERA_AF_START

DESCRIPTION
   Called by camera_svcs_start to set up AF module. Not to be confused with
   camera_af_start_focus().

DEPENDENCIES
  None

INPUT
  AF mode - CAMERA_AF_MODE_NORMAL or CAMERA_AF_MODE_MACRO

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_af_start
(
  const camsensor_static_params_type * camsensorParamsPtr,
  camera_af_mode_type                  mode
)
{
  if (camctrl_tbl.af_is_supported)
  {
    /* copy AF config from sensor */
    (void)vfe_copy_af_config(&vfeAfStatDspInput, camsensorParamsPtr);
   
    (void)camera_af_config( camsensorParamsPtr, mode, (int)AF_PROCESS_DEFAULT );
    /* Sync up lens position with the driver */
    cam3a_af_state.position = cam3a_af_state.infinity_position;
    if (camsensor_set_default_focus() == CAMERA_SUCCESS)
    {
      camera_pending_stats_cmds |= PENDING_AF_STATS_CMD_TO_ISSUE;
      /* command will be issued after vfe_capture */
    }
    else
    {
      MSG_AFDEBUG("Reset lens failed - AF disabled", 0, 0, 0);
      cam3a_af_state.is_enabled = FALSE;
    }
  }
  else
  {
    MSG_AFDEBUG("AF is not supported", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION      CAMERA_AF_CONFIG

DESCRIPTION
   Initializes global and locally used structure.  Provides
   tuning parameters for specific implementations

CALLERS:
  cam3a_config ()

DEPENDENCIES
  None

GLOBALS

RETURN VALUE
  CAMERA_SUCCESS if set successfully
  CAMERA_NOT_SUPPORTED if not supported
  CAMERA_FAILED if failed

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_af_config
(
  const camsensor_static_params_type * camsensorParamsPtr,
  camera_af_mode_type mode,
  int af_process
)
{    
  if (!camctrl_tbl.af_is_supported)
  {
    MSG_AFDEBUG("af_config unsupported", 0, 0, 0);
    return CAMERA_NOT_SUPPORTED;
  }

  if (camsensorParamsPtr == NULL)
  {
    MSG_AFDEBUG("af_config failed", 0, 0, 0);
    return CAMERA_FAILED;
  }


  /* Init AF state struct */
  cam3a_af_state.state = AF_INACTIVE;
  cam3a_af_state.stats_moved = 0;
  cam3a_af_state.move_lens_done = TRUE;
  cam3a_af_state.in_low_light = FALSE;

  // see which search algorithm we are to use

  if (af_process == AF_PROCESS_DEFAULT)
  {
    // use the one defined by the driver
    cam3a_af_state.af_process_type = camsensorParamsPtr->af_process_type;
  }
  else if (af_process == AF_PROCESS_UNCHANGED)
  {
    // else, do not change anything
  }
  else if ((af_process >= AF_EXHAUSTIVE_SEARCH) && (af_process < AF_PROCESS_MAX))
    cam3a_af_state.af_process_type = (cam3a_af_process_enum_type)af_process;
  else
  {
    MSG_AFDEBUG("unknown af_process received - force it to exhaustive", 0, 0, 0);
    cam3a_af_state.af_process_type =  AF_EXHAUSTIVE_SEARCH;
  }

  switch (mode)
  {
  case CAMERA_AF_MODE_UNCHANGED:
    break;

  case CAMERA_AF_MODE_NORMAL:
    cam3a_af_state.search_mode = mode;

    cam3a_af_state.near_end = camsensorParamsPtr->position_boundary;
    cam3a_af_state.far_end  = camsensorParamsPtr->position_far_end;
    cam3a_af_state.default_focus = camsensorParamsPtr->position_default_in_normal;
    cam3a_af_state.infinity_position = camsensorParamsPtr->position_far_end;
    break;

  case CAMERA_AF_MODE_MACRO:
    cam3a_af_state.search_mode = mode;

    cam3a_af_state.near_end = camsensorParamsPtr->position_near_end;
    cam3a_af_state.far_end  = camsensorParamsPtr->position_far_end;
    cam3a_af_state.default_focus = camsensorParamsPtr->position_default_in_macro;
    cam3a_af_state.infinity_position = camsensorParamsPtr->position_far_end;
    break;

  default:
    MSG_AFDEBUG("Unknown AF mode",0,0,0);
    break;
  }

  switch (cam3a_af_state.af_process_type)
  {
#ifdef CAMERA_AF_USE_EXHAUSTIVE_SEARCH
  case AF_EXHAUSTIVE_SEARCH:
  case AF_EXHAUSTIVE_FAST:
    camera_af_init_process_exhaustive_search(camsensorParamsPtr);
#endif
    break;

#ifdef CAMERA_AF_USE_HILL_CLIMBING
#error code not present
#endif

  default:
    MSG_AFDEBUG("Undefined af_process_type %d", cam3a_af_state.af_process_type, 0, 0);
  }

  /* tunr off testing flags for release build */
  cam3a_af_state.af_test_mode = FALSE;
  cam3a_af_state.collect_end_stat = FALSE;

  MSG_AFDEBUG("af_process = %d mode = %d", cam3a_af_state.af_process_type,
              cam3a_af_state.search_mode, 0);
  MSG_AFDEBUG("n = %d f = %d", cam3a_af_state.near_end, cam3a_af_state.far_end, 0);

  return CAMERA_SUCCESS;
}

#ifndef CAMERA_USES_SOFTDSP
/*===========================================================================

FUNCTION      CAMERA_AF_PROCESS_STATS_COMPLETE_MSG

DESCRIPTION
              Processes Auto Focus stats comming from VFE.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_af_process_stats_complete_msg (const CAMQDSP_AFStatsMessageType * msg)
{
  CAMQDSP_ReturnCodeType qdspRetCode = CAMQDSP_SUCCESS;

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  /* Got an AF stats cmd, clear pending flag */
  camera_pending_stats_cmds &= ~PENDING_AF_STATS_CMD_TO_ISSUE;

  if (!camera_af_is_active())
  {
    /* AF stats discarded */
    return;
  }

  MSG_AFDEBUG("AF stats received", 0, 0, 0);
  
  /* Keep track of AF frame count here */
  cam3a_af_state.elapsed_frame_cnt++;

  if (vfeStatStruct == NULL)
  {
    MSG_AFDEBUG("vfeStatStruct not allocated", 0, 0, 0);
    return;
  }

  if (cam3a_af_state.state != AF_INACTIVE)
  {
    vfeStatStruct->Focus = msg->focusValue; /* 27 bit focus value */
    vfeStatStruct->NFocus = msg->numFocus;  /* 9 bit num rows focus value */

    if (camera_af_process_focus_sensor( camctrl_tbl.camsensor_params_ptr ) != CAMERA_SUCCESS)
    {
      camera_af_done(CAMERA_EXIT_CB_FAILED);
    }
  }

  /* Above call to process_focus_sensor may have transitioned the state to inactive */
  if(!camera_af_is_active())
  {
    if (camera_dsp_command != CAMQDSP_CMD_IDLE)
    {
      qdspRetCode = VFE_AFStatsStop();
      MSG_AFDEBUG("Send AF Stats Stop to VFE", 0, 0, 0);
    }
    if (qdspRetCode != CAMQDSP_SUCCESS)
    {
      MSG_AFDEBUG("VFE AF Stats Stop command failed, code %d", qdspRetCode, 0, 0);
    }
    cam3a_af_state.state = AF_INACTIVE;

    /* No need to call back here since it was done in the algorithm code. */
  }
} /* camera_af_process_stats_complete_msg */
#endif
/*===========================================================================

FUNCTION      CAMERA_AF_PROCESS_FOCUS_SENSOR

DESCRIPTION
  Focus the sensor by gathering stats over a range of positions and
  selecting the best position.  Is complete when state transitions to
  AF_INACTIVE
  
  Algorithm:

  AF_START state
  -Input position is set back to the edge of infinity first (position must 
  reflect the actual sensor's physical focus position)

  AF_GATHER_STATS_COARSE_DELAY state
  -Here we delay 1 AF frame (or two actual frames) because the motion from the
   start state takes time to settle & accurate stats may not be gathered 
   without this delay.  This was found empirically on the Casio AF sensor

  AF_GATHER_STATS_COARSE state
  -Here, starting at the edge of infinity, we save the focus stat into an array
   & move the sensor closer in (near).  We only save stats if X number of 
   horizontal lines participated in the stats.  We move by a coarse measure 
   each time.  When we've reached the nearest edge, we search back & fine 
   the best focal point of the group.  We then move the sensor back to that 
   point - coarse/2 lengths, so that the middle of the course measure is 
   searched over.

  AF_GATHER_STATS_FINE_DELAY state
  -Here we delay 1 AF frame (or two actual frames) because the motion from the
   last sensor movement may take time to settle.

  AF_GATHER_STATS_FINE state
  -We perform a similar search as in coarse, but we single step over a limited
   range here to find the best focal position

  AF_INACTIVE stat
  -We transition to this state after completing the last movement in fine state
   and settling on a focal postion.

CALLERS:
  cam3a_process_focus_sensor ()

DEPENDENCIES

RETURN VALUE
  camera_ret_code_type:
  CAMERA_SUCCESS
  CAMERA_FAILED

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type 
camera_af_process_focus_sensor(const camsensor_static_params_type * 
                               camsensorParamsPtr)
{
  camera_ret_code_type  ret_val = CAMERA_SUCCESS;
  
  /* We cannot focus forever, if we exceed some max count, then just fail
     and return to inactive state */
  if (cam3a_af_state.elapsed_frame_cnt >= CAM3A_MAX_FRAMES_TO_PERFORM_FOCUS)
  {
    MSG_AFDEBUG("AF is taking too long - exit now", 0, 0, 0);
    camera_af_stop_focus();
    return ret_val;
  }
  
  /* if af stat is not valid, skip a frame */
  if ((!vfeStatStruct->Focus) || (!vfeStatStruct->NFocus))
  {
    MSG_AFDEBUG("Invalid AF stat from VFE: Focus = %d, NFocus = %d",
                vfeStatStruct->Focus, vfeStatStruct->Focus, 0);
    return ret_val;
  }

  cam3a_af_state.focusvalue_cur = (int32)(vfeStatStruct->Focus / vfeStatStruct->NFocus);

  if (cam3a_af_state.af_process_type == AF_EXHAUSTIVE_FAST)
  {
    cam3a_af_state.skip_frame_delay = TRUE;
  }
  else
  {
    cam3a_af_state.skip_frame_delay = FALSE;
  }

  switch (cam3a_af_state.af_process_type)
  {
#ifdef CAMERA_AF_USE_EXHAUSTIVE_SEARCH
  case AF_EXHAUSTIVE_SEARCH:
  case AF_EXHAUSTIVE_FAST:
    ret_val = camera_af_do_process_exhaustive_search(camsensorParamsPtr);
#endif
    break;

#ifdef CAMERA_AF_USE_HILL_CLIMBING
#error code not present
#endif

  default:
    MSG_AFDEBUG("Undefined af_process_type %d", cam3a_af_state.af_process_type, 0, 0);
  }
  
#ifdef CAMERA_AF_USE_FULL_SWEEP
  if ( ((cam3a_af_state.af_test_mode == TRUE) && 
        ((cam3a_af_state.state == AF_INACTIVE) ||
         (cam3a_af_state.state == AF_FOCUSING) ||
         (cam3a_af_state.state == AF_UNABLE_TO_FOCUS))) ||
        (cam3a_af_state.af_process_type == AF_FULL_SWEEP))
     
  {
    if ((cam3a_af_state.state == AF_INACTIVE) ||
        (cam3a_af_state.state == AF_FOCUSING)||
        (cam3a_af_state.state == AF_UNABLE_TO_FOCUS))
    {
        cam3a_af_state.state = AF_START;
        cam3a_af_state.af_process_type_org = cam3a_af_state.af_process_type;
        cam3a_af_state.af_process_type = AF_FULL_SWEEP;
        return CAMERA_SUCCESS;
    }    
    ret_val = camera_af_do_process_full_sweep(camsensorParamsPtr);
  }
  if ((cam3a_af_state.state == AF_INACTIVE) &&
     (cam3a_af_state.af_process_type == AF_FULL_SWEEP))
  {
    //Change back to original AF setting
    (void)camera_af_config(camsensorParamsPtr, cam3a_af_state.search_mode, AF_PROCESS_UNCHANGED);
    cam3a_af_state.af_process_type = cam3a_af_state.af_process_type_org;
  }  
#endif

  return ret_val;

} /* cam3a_process_focus_sensor */

/*===========================================================================

FUNCTION      CAMERA_AF_GET_FOCUS_POSITION

DESCRIPTION
  Get focus position

CALLERS:
  cam3a_get_focus_position ()

DEPENDENCIES

RETURN VALUE
  Value with in the range defined by CAMSENSOR layer for paticular sensor

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
int32 camera_af_get_focus_position (void)
{
  if(camctrl_tbl.af_use_isp)
  {
    return (camctrl_tbl.isp_af_get_position());
  }
  else
  {
    return(cam3a_af_state.position);
  }
} /* cam3a_get_focus_position */

/*===========================================================================

FUNCTION      CAMERA_AF_SET_FOCUS_POSITION

DESCRIPTION
  Set position of sensor based on passed in value

CALLERS:
  cam3a_set_focus_position

DEPENDENCIES

RETURN VALUE
  TRUE if operation completes successfully
  FALSE if not

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_af_set_focus_position( int32 position )
{
  cam3a_af_state.position = position;
  return TRUE;
} /* cam3a_set_focus_position */


/*===========================================================================
 *
 * FUNCTION    CAMERA_AF_IS_ACTIVE
 *
 * DESCRIPTION
 *   This function returns whether AF is active. 
 *
 * DEPENDENCIES
 *
 * RETURN VALUE
 *   TRUE if af is active, FALSE otherwise
 *
 * LIMITATIONS
 *
 * SIDE EFFECTS
 *   None
 *
 *===========================================================================*/
boolean camera_af_is_active
(
  void
)
{
  boolean ret_val;

  switch (cam3a_af_state.state)
  {
  case AF_START:               
  case AF_DECISION:            
  case AF_EXHAUSTIVE:          
  case AF_GATHER_STATS_COARSE: 
  case AF_GATHER_STATS_FINE:   
  case AF_FOCUSING:            
  case AF_UNABLE_TO_FOCUS:     
  case AF_COLLECT_END_STAT:    
    ret_val = TRUE;
    break;
  default:
    ret_val = FALSE;
  }

  return ret_val;
}

/*===========================================================================
 *
 * FUNCTION    CAMERA_AF_MOVE_LENS_TO
 *
 * DESCRIPTION
 *   This function is called to manually move lens to a particular position
 *
 * DEPENDENCIES
 *
 * RETURN VALUE
 *   CAMERA_SUCCESS       if moved to specified position
 *   CAMERA_NOT_SUPPORTED if af not supported by the sensor
 *   CAMERA_INVALID_STATE if af is in active state and manual focus is not permitted
 *   CAMERA_FAILED        if unable to move the sensor lens
 *
 * LIMITATIONS
 *
 * SIDE EFFECTS
 *   Sensor lens moved to requested position
 *
 *===========================================================================*/
camera_ret_code_type camera_af_move_lens_to
(
  int32 position
)
{
  camera_ret_code_type  ret_val = CAMERA_SUCCESS;
  int32                 steps;

  if (!camctrl_tbl.af_is_supported)
  {
    ret_val = CAMERA_NOT_SUPPORTED;
    MSG_AFDEBUG("Cannot manually move lens - not supported", 0, 0, 0);
  }
  else
  {
    if (camera_af_is_active())
    {
      camera_af_stop_focus();
    }

    MSG_AFDEBUG("Manually move lens from %d to %d", cam3a_af_state.position, position, 0);
    steps = position - cam3a_af_state.position;
    if (steps > 0)
    {
      camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_FAR, steps);
    }
    else if (steps < 0)
    {
      camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_NEAR, -steps);
    }
    cam3a_af_state.state = AF_INACTIVE;
  }
  
  return ret_val;
}

/*===========================================================================

FUNCTION      CAMERA_AF_START_FOCUS

DESCRIPTION
  API entry point for the focus sensor function.  Will queue in graph task
  and call callback when sensor has found optimial focal postion.

DEPENDENCIES

INPUT
  camera_cb_f_type callback - optional input; Will be called when AF is done,
     failed, or aborted.
  void *client_data         - again optional input.  Will be passed with
     callback function if applicable.

RETURN VALUE
  camera_ret_code_type
  If successful in focusing, the callback will be called with:
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

  Return value from this function:
    CAMERA_SUCCESS
    CAMERA_NO_SENSOR - if CAMSENSOR layer is not configured
    CAMERA_INVALID_STATE - if camera state is not ready for focus
    CAMERA_NOT_SUPPORTED - if the sensor we are currently using does not
       have a focus capability

  Return value from callback (final return):
    CAMERA_EXIT_CB_DONE   - AF completed, sensor is in focus
    CAMERA_EXIT_CB_FAILED - AF failed (probably failed to talk to sensor)
    CAMERA_EXIT_CB_ABORT  - AF stopped by other pending request

SIDE EFFECTS
  Launches graph task process that moves the sensor's lens

===========================================================================*/

camera_ret_code_type camera_af_start_focus
(
  camera_focus_e_type focus,
  camera_cb_f_type callback,
  void *client_data
)
{
  camera_snapshot_timing.autofocus_start = (uint32)timetick_get_ms();
  camera_snapshot_information.autofocus_mode = (uint8) cam3a_af_state.search_mode;
  
#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if (camsensor_initialized == FALSE)
  {
    MSG_AFDEBUG("No sensor", 0, 0, 0);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state != CAMERA_STATE_PREVIEW)
  {
    MSG_AFDEBUG("Invalid camera state - camera_state = %d", camera_state, 0, 0);
    return CAMERA_INVALID_STATE;
  }

  /* Is the sensor capable of auto-focus?  If not then return error */
  if (!camctrl_tbl.af_is_supported)
  {
    MSG_AFDEBUG("AF not supported by sensor", 0, 0, 0);
    return CAMERA_NOT_SUPPORTED;
  }

  /* AF was requested & is already underway - fail this 2nd request  */
  if (camera_af_is_active())
  {
    MSG_AFDEBUG("Cannot start focus - AF state = %d", cam3a_af_state.state, 0, 0);
    return CAMERA_INVALID_STATE;
  }

  if (focus == CAMERA_AUTO_FOCUS)
  {
    if (!camera_af_is_enabled())
    {
      MSG_AFDEBUG("AF is disabled", 0, 0, 0);
      return CAMERA_INVALID_STATE;
    }
  }
  else if (focus != CAMERA_MANUAL_FOCUS)
  {
    MSG_AFDEBUG("Unsupported focus type %d", focus, 0, 0);
    return CAMERA_NOT_SUPPORTED;
  }

  /*lint -save -e611 cast to generic callback funtion pointers */
  graph_queue_camera_func ((uint32)CAMERA_FUNC_START_FOCUS, (void *)callback, client_data,
                           (uint32)focus, 0, 0, 0);
  /*lint -restore */

  return CAMERA_SUCCESS;
} /* camera_af_start_focus */

/*===========================================================================

FUNCTION      CAMERA_AF_PROCESS_START_FOCUS

DESCRIPTION
  Kicks off a focus sensor series, which requires multiple frames to implement.

  For auto mode, once the best focal is achieved, EXIT_CB_DONE
  callback is executed and FOCUS_STEP parameter is updated.

  For manual mode, once the specified value in FOCUS_STEP parameter
  is achieved, EXIT_CB_DONE callback is executed.

  For continuous mode, if supported, EXIT_CB_DONE callback is
  never executed but the FOCUS_STEP parameter is updated
  continuously until camera_stop_focus() is called.

DEPENDENCIES
  None

RETURN VALUE
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when operation is achieved or stopped.

  If not successful,
   1. CAMERA_EXIT_CB_FAILED - usually because the sensor stepping operation
                              failed
   2. CAMERA_EXIT_CB_ABORT  - premature request to terminate Auto Focus
                              before it was complete.

SIDE EFFECTS
  None

===========================================================================*/

void camera_af_process_start_focus
(
  camera_cb_f_type     callback,
  void                *client_data,
  camera_focus_e_type  focus
)
{
  camera_ret_code_type   ret_val = CAMERA_SUCCESS;

  /* Save these away for later */
  cam3a_af_state.callback  = callback;
  cam3a_af_state.client_data  = client_data;

  /* move to the stated location (must have done a set_parm
     with CAMERA_PARM_FOCUS_STEP)                    */
  if (focus == CAMERA_MANUAL_FOCUS)
  {
    ret_val = camera_af_move_lens_to(camera_parm_manual_focus.current_value);
    if (ret_val == CAMERA_SUCCESS)
    {
      /* Manual focus succeeded */
      camera_af_done(CAMERA_EXIT_CB_DONE);
    }
    else
    {
      /* Manual focus failed */
      camera_af_done(CAMERA_EXIT_CB_FAILED);
    }
  } /* end if (focus == CAMERA_MANUAL_FOCUS) */

  else if (focus == CAMERA_AUTO_FOCUS)
  {
    if (camctrl_tbl.af_use_isp)
    {
      camctrl_tbl.isp_af_start(cam3a_af_state.search_mode, NULL);
      /* call back right away since sensor has no feedback status */
      camera_af_done(CAMERA_EXIT_CB_DONE);
    }
    else
    {
#ifndef FEATURE_CAMERA_YCBCR_ONLY
      /* If exposure needs more time ... */
      if ((camctrl_tbl.camsensor_params_ptr->aec_enable == TRUE) &&
          (cam3a_aec_state.luma_settled_cnt < CAM3A_LUMA_SETTLED_BEFORE_AF_CNT))
      {
        cam3a_af_state.state = AF_WAIT_FOR_AEC_TO_SETTLE;
        MSG_AFDEBUG("AF state = WAIT_FOR_AEC_TO_SETTLE", 0, 0, 0);
      }
      else
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
      {
        /* Start AF process by transitioning state variable */
        cam3a_af_state.state = AF_START;
        MSG_AFDEBUG("AF state = START", 0, 0, 0);
        cam3a_af_state.elapsed_frame_cnt = 0;
     
        /* Set pending flag for AF */
        camera_pending_stats_cmds |= PENDING_AF_STATS_CMD_TO_ISSUE;
      }

      /* Indicates to caller that the start of a long operation
      has begun (so the queue to Graph was a success).
      Also, this function (including focus operations)
      must complete successfully */
      if (cam3a_af_state.callback)
      {
         /* AF Completed successfully */
         (cam3a_af_state.callback) (CAMERA_RSP_CB_SUCCESS, \
            cam3a_af_state.client_data, CAMERA_FUNC_START_FOCUS, 0);
      }
    }
  } /* End else (for AUTO focus ) */


} /* camera_af_process_start_focus */

/*===========================================================================

FUNCTION      CAMERA_AF_FOCUS_IF_WAIT_FOR_AEC

DESCRIPTION
  This function is called by camera_svcs to see if AF is waiting for AEC to
  settle. If conditions are met, then AF can start.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camera_af_focus_if_wait_for_aec
(
  void
)
{
  if (cam3a_af_state.state == AF_WAIT_FOR_AEC_TO_SETTLE)
  {
#ifndef FEATURE_CAMERA_YCBCR_ONLY
    /* Check if luma has settled, or if if we have waited too long */
    if ((cam3a_aec_state.luma_settled_cnt >= CAM3A_LUMA_SETTLED_BEFORE_AF_CNT) ||
        (cam3a_aec_state.exp_index == (int32)(cam3a_aec_state.num_exposure_table_values - 1)) ||
        (cam3a_af_state.elapsed_frame_cnt >= CAM3A_MAX_NUM_FRAMES_TO_WAIT_FOR_AEC_TO_SETTLE))
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
    {
      cam3a_af_state.state = AF_START;
      MSG_AFDEBUG("AF state = START", 0, 0, 0);
      cam3a_af_state.elapsed_frame_cnt = 0;

      /* Set pending flag for AF */
      camera_pending_stats_cmds |= PENDING_AF_STATS_CMD_TO_ISSUE;
    }
  }
}

/*===========================================================================

FUNCTION      CAMERA_AF_SEND_VFE_AF_START_COMMAND

DESCRIPTION
  This function is called to send AF stat start command to VFE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_af_send_vfe_af_start_command
(
  void
)
{
  CAMQDSP_ReturnCodeType qdspRetCode;

  /* Set pending flag for AF */
  camera_pending_stats_cmds |= PENDING_AF_STATS_CMD_TO_ISSUE;

  /* Request AF Stats from VFE */
  if (camera_dsp_command != CAMQDSP_CMD_IDLE)
  {
    qdspRetCode = VFE_AFStatsStart(vfeAfStatDspInput);
    MSG_AFDEBUG("sending...", 0, 0, 0);

    if ( qdspRetCode != CAMQDSP_SUCCESS)
    {
      MSG_AFDEBUG ("VFE AF Stats Start command failed, code: %d", qdspRetCode, 0, 0);
    }
  }
  /* retry is handled in camera_process_qdsp_output1_msg */

}

/*===========================================================================
 *
 * FUNCTION    CAMERA_AF_STOP_FOCUS
 *
 * DESCRIPTION
 *   Bestshot calls this function to abort AF.
 *
 * DEPENDENCIES
 *
 * RETURN VALUE
 *   None
 *
 * LIMITATIONS
 *
 * SIDE EFFECTS
 *   None
 *
 *===========================================================================*/
void camera_af_stop_focus
(
  void
)
{

  if (camera_af_is_active())
  {
    /* No need to tell VFE to stop AF stats if AF is always on */
    camera_af_done(CAMERA_EXIT_CB_ABORT);
  }
}
     
/*===========================================================================
 *
 * FUNCTION    CAMERA_AF_ENABLE
 *
 * DESCRIPTION
 *   Bestshot calls this function to re-enable AF.
 *
 * DEPENDENCIES
 *
 * RETURN VALUE
 *   None
 *
 * LIMITATIONS
 *
 * SIDE EFFECTS
 *   None
 *
 *===========================================================================*/
void camera_af_enable
(
  void
)
{
  if (camctrl_tbl.af_is_supported)
  {
    cam3a_af_state.is_enabled = TRUE;
  }
}

/*===========================================================================
 *
 * FUNCTION    CAMERA_AF_DISABLE
 *
 * DESCRIPTION
 *   Bestshot calls this function to disable AF.
 *
 * DEPENDENCIES
 *
 * RETURN VALUE
 *   None
 *
 * LIMITATIONS
 *
 * SIDE EFFECTS
 *   None
 *
 *===========================================================================*/
void camera_af_disable
(
  void
)
{
  cam3a_af_state.is_enabled = FALSE;
}

/*===========================================================================
 *
 * FUNCTION    CAMERA_AF_IS_ENABLED
 *
 * DESCRIPTION
 *   Camera svcs calls this function to query whether AF is enabled or not.
 *
 * DEPENDENCIES
 *
 * RETURN VALUE
 *   TRUE if enabled, FALSE otherwise
 *
 * LIMITATIONS
 *
 * SIDE EFFECTS
 *   None
 *
 *===========================================================================*/
boolean camera_af_is_enabled
(
  void
)
{
  return (cam3a_af_state.is_enabled ? TRUE : FALSE);
}

/*===========================================================================

FUNCTION    CAMERA_AF_DO_RESET_LENS

DESCRIPTION
  Move focus to start up position.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_af_do_reset_lens
(
  void
)
{
  if (camctrl_tbl.af_is_supported)
  {
    /* if new AF command comes in before lens movement is done, we need protect it */
    cam3a_af_state.move_lens_done = FALSE;
    if (camsensor_set_default_focus() != CAMERA_SUCCESS)
    {
      MSG_AFDEBUG("Reset lens failed", 0, 0, 0);
    }
    else
    {
      cam3a_af_state.position = cam3a_af_state.infinity_position;
      MSG_AFDEBUG("Reset lens succeeded", 0, 0, 0);
    }
    cam3a_af_state.move_lens_done = TRUE;
  }
}

/*===========================================================================

FUNCTION    CAMERA_AF_DONE

DESCRIPTION
  This is the exit function of auto focus. At the end of auto 
  focus, this function is called and it returns to the call back function
  supplied to auto focus module.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_af_done(camera_cb_type cb)
{
  if (cb !=CAMERA_EXIT_CB_DONE )
  {
     MSG_AFDEBUG("Autofocus is not successful",0,0,0);
  }
  camera_snapshot_timing.autofocus_end = (uint32)timetick_get_ms();
  cam3a_af_state.state = AF_INACTIVE;
  if (cam3a_af_state.callback)
  {
    (cam3a_af_state.callback) (cb, cam3a_af_state.client_data,
                               CAMERA_FUNC_START_FOCUS, 0);
    cam3a_af_state.callback = NULL;
  }
}
#endif //#ifndef CAMERA_USES_SOFTDSP
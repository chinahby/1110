/*=============================================================================
 *
FILE:       camera_af.c

SERVICES:   Auto Focus file

DESCRIPTION: 

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/af/main/latest/src/camera_af_core.c#2 $ $DateTime: 2008/11/11 14:21:59 $ $Author: cgera $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
--------   -----    ----------------------------------------------------------
 11/1108    cg       Fix for CR # 154084
 10/03/06   jn       Added custcam3a.h to featurize 3A algorithms.
 09/12/06   jn       Added undershoot protection.
 09/08/06   klee     Updated min/max macro to be CAM3A_MIN/CAM3A_MAX
 09/01/06   kadhir   Removed unwanted Cross Compilation Dependency 
                     When qcamcorder Feature Disabled  
 08/18/06   jn       Split camera_af into camera_af and camera_af_core (algorithm).
 
==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "custcam3a.h"
#include "camctrl.h"
#include "camera_af.h"


/*==============================================================================
  INTERNAL VARIABLES
==============================================================================*/
static int downhill_allowance;
static float fv_drop_allowance;

#ifdef CAMERA_AF_USE_EXHAUSTIVE_SEARCH
static int uphill_threshold;
#endif /* CAMERA_AF_USE_EXHAUSTIVE_SEARCH */

#ifdef CAMERA_AF_USE_HILL_CLIMBING
#error code not present
#endif /* CAMERA_AF_USE_HILL_CLIMBING */

/*lint -save -e704, these shifts are okay */
/*lint -save -e713, ok */
/*lint -save -e732, ok */
/*lint -save -e641  ok to have enum be cast as int */
/*lint -save -e64   ok to have enum be cast as int */


/*==============================================================================
  INTERNAL API DECLARATIONS
==============================================================================*/
#ifdef CAMERA_AF_USE_HILL_CLIMBING
#error code not present
#endif /* CAMERA_AF_USE_HILL_CLIMBING */

/*==============================================================================
  INTERNAL API DEFINITIONS
==============================================================================*/

#ifdef CAMERA_AF_USE_EXHAUSTIVE_SEARCH
/*===========================================================================

FUNCTION    CAMERA_DO_PROCESS_EXHAUSTIVE_SEARCH

DESCRIPTION
  Focus the sensor by gathering stats over a range of positions and
  selecting the best position.  Is complete when the state transitions to
  AF_INACTIVE
  Tuned for the Casio NX5300 CCD sensor right now (but should work for any CCD)
  CMOS sensors with focus may require some adjustment

AF STATES:
  AF_INACTIVE
   Default focused state, at the edge of infinity initially

  AF_PROCESS_QUEUED
   AF was requested & is queued in camera_drv_task context

  AF_PROCESS_MANUAL_QUEUED
   Manual sensor motor change position was requested & is queued camera_drv_task context

  AF_START
   Input position is set back to the edge of infinity first (position must 
   reflect the actual sensor's physical focus position)

  AF_GATHER_STATS_COARSE
   Here, starting at the edge of infinity, we save the focus stat into an array
   & move the sensor closer in (near).  We only save stats if X number of 
   horizontal lines participated in the stats.  We move by a coarse measure 
   each time.  When we've reached the nearest edge, we search back & fine 
   the best focal point of the group.  We then move the sensor back to that 
   point - coarse/2 lengths, so that the middle of the course measure is 
   searched over.

   We also delay the appropriate number of frames between movements of the
   sensor's stepping motor (this number is sensor dependent)

  AF_GATHER_STATS_FINE
   We perform a similar search as in coarse, but we single step over a limited
   range here to find the best focal position
   We also delay the appropriate number of frames between movements of the
   sensor's stepping motor (this number is sensor dependent)

  AF_INACTIVE
   We transition to this state after completing the last movement in fine state
   and settling on a focal postion.

Other states:
  AF_WAIT_FOR_AEC_TO_SETTLE
   Before AF_START, to wait for AEC to settle down
   before trying to move lens.  No action here.

DEPENDENCIES

GLOBAL INPUT/OUTPUT:
  cam3a_af_state - a global structure to keep and advance AF state.
  vfeStatStruct - global structure used to analize 3A VFE output.
  vfeExpWbStatDspInput - global structure that is the input to the AEC/AWB
                 stat command
  vfeAfStatDspInput - global structure that is the input to the AF stat

RETURN VALUE
  camera_ret_code_type:
  CAMERA_SUCCESS
  CAMERA_FAILED
   Usually because we could not move the sensor's stepping motor
   We fail here only after many repeated attempts

SIDE EFFECTS
  None

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type 
camera_af_do_process_exhaustive_search (const camsensor_static_params_type 
                                        * camsensorParamsPtr)
{
  camera_ret_code_type  ret_val = CAMERA_SUCCESS;
  int32 just_moved = 0;
  int32 num_fine_focal_positions;
  int32 num_fine_steps_between_stat_points;
  int32 num_gross_steps_between_stat_points;
  boolean ok_to_move, stop_condition;
  int32 steps_to_move;


#ifdef FEATURE_CAMERA_MOBICAT
  int cnt;
#endif /* FEATURE_CAMERA_MOBICAT */

  if (camsensorParamsPtr == NULL) return CAMERA_FAILED;

  if (cam3a_af_state.move_lens_done == FALSE)
  {
    cam3a_af_state.frame_delay--;
    if (cam3a_af_state.frame_delay < -LENS_DONE_MOVE_THRESH)
    {
      // something is wrong, we never got
      // acknoweledgment that lens is done
      // moving
      cam3a_af_state.move_lens_done = TRUE;
    }
    return CAMERA_SUCCESS;
  }

  /* Set in camsensor layer - this could be thought of as the AF
    aggressiveness */

  if (camsensorParamsPtr->preview_fps >> 8 <= 8)   /* low light condition */
  {
    num_gross_steps_between_stat_points =
      camsensorParamsPtr->num_gross_steps_between_stat_points * 3 / 2;
    num_fine_steps_between_stat_points = 
      camsensorParamsPtr->num_fine_steps_between_stat_points * 2;
    num_fine_focal_positions = camsensorParamsPtr->num_fine_search_points / 2;
    cam3a_af_state.in_low_light = TRUE;
  }
  else
  {
    num_gross_steps_between_stat_points =
      camsensorParamsPtr->num_gross_steps_between_stat_points;
    num_fine_steps_between_stat_points = 
    camsensorParamsPtr->num_fine_steps_between_stat_points;
    num_fine_focal_positions = camsensorParamsPtr->num_fine_search_points;
    cam3a_af_state.in_low_light = FALSE;
  }

  // REVISIT: should add error handling to cap num gross or fine points
  // gathered to 10

  if ((vfeStatStruct == NULL) ||
      (cam3a_af_state.step_sensor_failure_cnt > CAM3A_NUM_FOCUS_ATTEMPTS))
  {
    if (cam3a_af_state.step_sensor_failure_cnt > CAM3A_NUM_FOCUS_ATTEMPTS)
    {
      MSG_AFDEBUG ("Focus, failed after %d attempt to move sensor position",
                CAM3A_NUM_FOCUS_ATTEMPTS,0, 0);
      cam3a_af_state.state = AF_ERROR;
    }
    else
    {
      cam3a_af_state.state = AF_INACTIVE;
    }
    /* Exit failure */
    if (cam3a_af_state.callback)
    {
      (cam3a_af_state.callback) (CAMERA_EXIT_CB_FAILED, 
                                 cam3a_af_state.client_data,
                                 CAMERA_FUNC_START_FOCUS, 0);
      cam3a_af_state.callback = NULL;
    }
    return CAMERA_FAILED;
  }

  if (cam3a_af_state.state == AF_START)
  {
    cam3a_af_state.af_start_time = camera_3a_shared_get_time_ms();
    cam3a_af_state.frame_delay = 0;
    cam3a_af_state.stats_moved = 0;

    MSG_AFDEBUG("type %d mode %d", cam3a_af_state.af_process_type,
                cam3a_af_state.search_mode, 0);
    MSG_AFDEBUG("n %d f %d ", cam3a_af_state.near_end, cam3a_af_state.far_end, 0);

    /* Get the sensor to a known position */

    if (cam3a_af_state.position <= cam3a_af_state.far_end)
    {
      if ((camctrl_tbl.undershoot_protect) &&
          (cam3a_af_state.far_end == camsensorParamsPtr->position_far_end))
      {
        MSG_AFDEBUG("orig position is %d", cam3a_af_state.position, 0, 0);
        /* to compensate initial len movement undershoot multiply delta by 1.6 */
        steps_to_move = (cam3a_af_state.far_end - cam3a_af_state.position) * 8 / 5;
        steps_to_move = (steps_to_move >= 2)? steps_to_move : 2; /* at least 2 steps */
        if (steps_to_move > cam3a_af_state.far_end)
        {
          steps_to_move = cam3a_af_state.far_end; /* don't want negative position */
        }
        cam3a_af_state.position = cam3a_af_state.far_end - steps_to_move;
        MSG_AFDEBUG ("changed position to %d", cam3a_af_state.position, 0, 0);
      }
      else
      {
        steps_to_move = cam3a_af_state.far_end - cam3a_af_state.position;
      }

      cam3a_af_state.state_changed = TRUE; /* only need to set this flag if move lens */
      camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_FAR, steps_to_move);
      just_moved = 1;
    }
    else if (cam3a_af_state.position > cam3a_af_state.far_end)
    {
      MSG_AFDEBUG ("AF_START Warning, Lens Position %d is larger than default_focus %d", 
                   cam3a_af_state.position, cam3a_af_state.default_focus, 0);
      /* we consider the current position not trustworthy */
      cam3a_af_state.position = 0;
      cam3a_af_state.state_changed = TRUE; /* only need to set this flag if move lens */
      camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_FAR,
                             cam3a_af_state.far_end - cam3a_af_state.position);
      just_moved = 1;
    }

    //DLI Changed for the two modes
    if (cam3a_af_state.search_mode == CAMERA_AF_MODE_NORMAL)
      cam3a_af_state.state = AF_GATHER_STATS_FINE;
    else //if(cam3a_af_state.search_mode == CAMERA_AF_MODE_MACRO) 
      cam3a_af_state.state = AF_GATHER_STATS_COARSE;

    cam3a_af_state.direction = CAMSENSOR_MOVE_FOCUS_NEAR; 
    //DLI changes end here
    cam3a_af_state.index = 0;
    //DLI added
    cam3a_af_state.focusvalue_max = 0;
    cam3a_af_state.focusvalue_min = 0;
    cam3a_af_state.num_downhill = 0;
    cam3a_af_state.num_uphill   = 0;

#ifdef FEATURE_CAMERA_MOBICAT
    /* initializing */
    cam3a_af_state.tracing_info_index = 0;
    for (cnt = 0; cnt < CAM3A_NUM_AF_COLLECTION_POINTS; cnt++)
    {
      cam3a_af_state.tracing_info_stats[cnt] = 0;
      cam3a_af_state.tracing_info_n_focus[cnt] = 0;
      cam3a_af_state.tracing_info_positions[cnt] = 0;
    }
    cam3a_af_state.tracing_info_total_movement_trial = 0;
    cam3a_af_state.tracing_info_total_movement_lens = 0;
    cam3a_af_state.tracing_info_focusing_time = camera_3a_shared_get_time_ms();
#endif
    if (just_moved == 1)
    {  
       MSG_AFDEBUG("Frame Delays after initialization %d, %d, %d", 
                   cam3a_af_state.frame_delay, vfeStatStruct->Focus/ vfeStatStruct->NFocus, 
                   cam3a_af_state.position);
       return CAMERA_SUCCESS;
    }  
  }
  if (cam3a_af_state.state == AF_GATHER_STATS_COARSE)
  {
    /* Keep same state, take no action just decrment
      frame delay count.  Thought is that the stats
      for this frame are not trustworth - so wait
      until they are */
    if (cam3a_af_state.frame_delay > 0)
    {
      if (cam3a_af_state.index == 0)
        MSG_AFDEBUG("Frame Delays, COARSE MODE,  and Nlin %d, %d, %d", 
                    cam3a_af_state.frame_delay, vfeStatStruct->Focus/ vfeStatStruct->NFocus, 
                    cam3a_af_state.position);
      cam3a_af_state.frame_delay--;
      return CAMERA_SUCCESS;
    }

    if (cam3a_af_state.index < CAM3A_NUM_AF_COLLECTION_POINTS)
    {
      /* Store 27 bit focus value divided by num valid rows
        to get an average */
      cam3a_af_state.stats[cam3a_af_state.index++] = 
      (int32)(vfeStatStruct->Focus / vfeStatStruct->NFocus);
      MSG_AFDEBUG("Index, Focusvalue, and LensPosition %d, %d, %d", 
                 cam3a_af_state.index-1, 
                 vfeStatStruct->Focus/vfeStatStruct->NFocus, 
                 cam3a_af_state.position);  /* increse index */

      if (cam3a_af_state.focusvalue_max < 
          cam3a_af_state.stats[cam3a_af_state.index-1])
      {
        cam3a_af_state.focusvalue_max = 
          cam3a_af_state.stats[cam3a_af_state.index-1];
        cam3a_af_state.position_max = cam3a_af_state.position;
        cam3a_af_state.num_downhill = 0;
        if (cam3a_af_state.index == 1)
          cam3a_af_state.focusvalue_min = cam3a_af_state.focusvalue_max;
      }
      else
      {
        //added by DLI, to include minFV
        if (cam3a_af_state.focusvalue_min > 
            cam3a_af_state.stats[cam3a_af_state.index-1])
          cam3a_af_state.focusvalue_min = 
            cam3a_af_state.stats[cam3a_af_state.index-1];
        if (cam3a_af_state.stats[cam3a_af_state.index-1] <= 
            cam3a_af_state.stats[cam3a_af_state.index-2])
        {
          cam3a_af_state.num_downhill++;
        }
        else
        {
          cam3a_af_state.num_downhill = 0;
        }
      }       
    }
#ifdef FEATURE_CAMERA_MOBICAT
    /* update tracing info array */
    cam3a_af_state.tracing_info_stats[cam3a_af_state.tracing_info_index] = 
    (int32)(vfeStatStruct->Focus / vfeStatStruct->NFocus);
    cam3a_af_state.tracing_info_n_focus[cam3a_af_state.tracing_info_index] = 
    (int32)vfeStatStruct->NFocus;
    cam3a_af_state.tracing_info_positions[cam3a_af_state.tracing_info_index] = 
    cam3a_af_state.position;
    /* increse index */
    cam3a_af_state.tracing_info_index++;
#endif

    /* Is there room to move the sensor again? */
    if (((cam3a_af_state.position-num_gross_steps_between_stat_points) >= 
         cam3a_af_state.near_end) && //End of search
        !((cam3a_af_state.num_downhill >= downhill_allowance) && 
          (cam3a_af_state.stats[cam3a_af_state.index-1] < 
           cam3a_af_state.focusvalue_max * fv_drop_allowance))) //FV Drop and Downhill 
    {
      camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_NEAR, num_gross_steps_between_stat_points);
      just_moved = 1;
    }

    /* Have we traversed the whole focal range & gathered
      focal stats at each point?
      If so we are at the nearest focal position */
    if ((((cam3a_af_state.position-num_gross_steps_between_stat_points) <=
           cam3a_af_state.near_end) && (just_moved == 0)) || 
        ((cam3a_af_state.num_downhill >= downhill_allowance) && 
         (cam3a_af_state.stats[cam3a_af_state.index-1] < 
          cam3a_af_state.focusvalue_max * fv_drop_allowance)))
    {

      int32 maxindex, localmax, i, moveBackSteps;

      cam3a_af_state.index--;      // point to last true data value
      maxindex = localmax = 0;
      for (i = 0; i <= cam3a_af_state.index; i++)
      {

        if (cam3a_af_state.stats[i] > localmax)
        {
          localmax = cam3a_af_state.stats[i];
          maxindex = i;
        }
      }

      moveBackSteps = 
      (cam3a_af_state.index-maxindex)*num_gross_steps_between_stat_points;

      //REVISIT to check FV_min/FV_max                                                                                        
      if ((cam3a_af_state.focusvalue_max < MODE_CHANGE_THRESH+200) || 
          (cam3a_af_state.focusvalue_min > 
           ((float)cam3a_af_state.focusvalue_max*CONFIDENCE_LEVEL_EXH)))
      {
        //FV_min and FV_max are too close, stay at the maximum position, &quit
        //No need to do fine search, because the case is either noisy, or 
        //low contrast
        if (moveBackSteps> 0)
        {
          cam3a_af_state.state = AF_UNABLE_TO_FOCUS;
          camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_FAR, moveBackSteps);
        }
        else
        {
          if (cam3a_af_state.collect_end_stat)
          {
            cam3a_af_state.state = AF_COLLECT_END_STAT;
          }
          else
          {
            cam3a_af_state.state = AF_INACTIVE;
            if (cam3a_af_state.callback)
            {
              (cam3a_af_state.callback) (CAMERA_EXIT_CB_DONE, 
                                         cam3a_af_state.client_data, 
                                         CAMERA_FUNC_START_FOCUS, 0);
              cam3a_af_state.callback = NULL;
            }
          }
        }

        MSG_AFDEBUG("Warning: FVs not reliable, Move to Maximum location and quit %d, %d, %d", cam3a_af_state.focusvalue_max, cam3a_af_state.focusvalue_min, cam3a_af_state.position);
        return CAMERA_SUCCESS;
      }

        //DLI changed to accommodate #Fine search here
      if (moveBackSteps>= (num_fine_focal_positions>>1)) 
      {

        /* Position start of search in center between the max recorded point */
        /*lint -save -e704 shift right of sighed quantity */
        if (cam3a_af_state.skip_frame_delay && (maxindex > 0))
        {
          moveBackSteps -= (num_fine_focal_positions>>1) - num_gross_steps_between_stat_points;
        }
        else
        {
          moveBackSteps -= num_fine_focal_positions>>1;
        }
           
        /*lint -restore */
        /* Select bin for finer search */
        if (cam3a_af_state.skip_frame_delay)
        {
          cam3a_af_state.state_changed = TRUE;
        }
        camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_FAR, moveBackSteps);
      }
      /* Else take the sensor all the way in (near) to start fine search */
      else /* this case is finishing the entire range. */
      {
        /* Select bin for finer search */
        if (cam3a_af_state.skip_frame_delay)
        {
          cam3a_af_state.state_changed = TRUE;
        }
        camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_NEAR,
                               cam3a_af_state.position - cam3a_af_state.near_end);
        // REVISIT: depending on the num steps, may need to wait longer
        // between next stat process
        moveBackSteps = cam3a_af_state.position - cam3a_af_state.near_end;
      }      

      /* Change state  */
      MSG_AFDEBUG("FOR DEBUG PURPOSE HERE CHANGE to FINESEARCH %d, %d, %d",
               cam3a_af_state.focusvalue_max,
               cam3a_af_state.focusvalue_min,
               cam3a_af_state.position);
      cam3a_af_state.state = AF_GATHER_STATS_FINE;
      cam3a_af_state.direction = CAMSENSOR_MOVE_FOCUS_FAR; 

      cam3a_af_state.focusvalue_max = 0;
      cam3a_af_state.index = 0;
      cam3a_af_state.num_downhill = 0;      
    }
  }
  else if (cam3a_af_state.state == AF_GATHER_STATS_FINE)
  {
    //NOTE: DLI replaces all the move direction to cam3a_af_state.direction
    /* Keep same state, take no action just decrment
      frame delay count.  Thought is that the stats
      for this frame are not trustworth - so wait
      until they are */
    if (cam3a_af_state.frame_delay > 0)
    {
      if (cam3a_af_state.index == 0)
        MSG_AFDEBUG("Frame Delays, FINE MODE,  and Nlines %d, %d, %d", cam3a_af_state.frame_delay, vfeStatStruct->Focus/ vfeStatStruct->NFocus, cam3a_af_state.position);
      cam3a_af_state.frame_delay--;
      return CAMERA_SUCCESS;
    }


    if (cam3a_af_state.index < CAM3A_NUM_AF_COLLECTION_POINTS)
    {
      /* Store 27 bit focus value divided by num valid rows
        to get an average */
      cam3a_af_state.stats[cam3a_af_state.index++] = 
      (int32)(vfeStatStruct->Focus / vfeStatStruct->NFocus);
      MSG_AFDEBUG("Index, Focusvalue, and LensPosition %d, %d, %d", cam3a_af_state.index, vfeStatStruct->Focus/ vfeStatStruct->NFocus, cam3a_af_state.position);  
      /* increse index */
      if (cam3a_af_state.focusvalue_max < 
          cam3a_af_state.stats[cam3a_af_state.index-1])
      {
        cam3a_af_state.focusvalue_max = 
          cam3a_af_state.stats[cam3a_af_state.index-1];
        cam3a_af_state.position_max = cam3a_af_state.position;
        cam3a_af_state.num_downhill = 0;
        /* update uphill indicator */
        cam3a_af_state.num_uphill++;
        if (cam3a_af_state.index == 1)
          cam3a_af_state.focusvalue_min = cam3a_af_state.focusvalue_max;
      }
      else
      {
        //added by DLI, to include minFV
        if (cam3a_af_state.focusvalue_min >= 
            cam3a_af_state.stats[cam3a_af_state.index-1])
          cam3a_af_state.focusvalue_min = 
            cam3a_af_state.stats[cam3a_af_state.index-1];
        if (cam3a_af_state.stats[cam3a_af_state.index-1] <= 
            cam3a_af_state.stats[cam3a_af_state.index-2])
        {
          cam3a_af_state.num_downhill++;
          /* clear uphill indicator */
          cam3a_af_state.num_uphill = 0;
        }
        else
        {
          cam3a_af_state.num_downhill = 0;
          /* update uphill indicator */
          cam3a_af_state.num_uphill++;
        }
      }
    }

#ifdef FEATURE_CAMERA_MOBICAT
    /* update tracing info array */
    cam3a_af_state.tracing_info_stats[cam3a_af_state.tracing_info_index] = 
    (int32)(vfeStatStruct->Focus / vfeStatStruct->NFocus);
    cam3a_af_state.tracing_info_n_focus[cam3a_af_state.tracing_info_index] = 
    (int32)vfeStatStruct->NFocus;
    cam3a_af_state.tracing_info_positions[cam3a_af_state.tracing_info_index] = 
    cam3a_af_state.position;
    /* increse index */
    cam3a_af_state.tracing_info_index++;
#endif

    if (camctrl_tbl.undershoot_protect)
    {
      /* undershoot condition is met if
       * 1) at far end or near end in macro mode
       * 2) at max fv
       * 3) has strong increasing tendency
       */
      if ((((cam3a_af_state.position == cam3a_af_state.near_end) &&
            (cam3a_af_state.search_mode == CAMERA_AF_MODE_MACRO)) ||
           (cam3a_af_state.position == cam3a_af_state.far_end)) &&
          (cam3a_af_state.focusvalue_cur == cam3a_af_state.focusvalue_max) &&
          (cam3a_af_state.num_uphill > uphill_threshold)) 
      {
        if (cam3a_af_state.position == cam3a_af_state.near_end)
        {
          cam3a_af_state.position = cam3a_af_state.position + camctrl_tbl.undershoot_adjust;
          MSG_AFDEBUG("undershoot detected, change position to %d", cam3a_af_state.position, 0, 0); 
          /* update position_max as well */
          cam3a_af_state.position_max = cam3a_af_state.position_max + camctrl_tbl.undershoot_adjust;
        }
        else if (cam3a_af_state.position == cam3a_af_state.far_end)
        {
          cam3a_af_state.position = cam3a_af_state.position - camctrl_tbl.undershoot_adjust;
          MSG_AFDEBUG("undershoot detected, change position to %d", cam3a_af_state.position, 0, 0);
          /* update position_max as well */
          cam3a_af_state.position_max = cam3a_af_state.position_max - camctrl_tbl.undershoot_adjust;
        }
        /* clear uphill indicator */
        cam3a_af_state.num_uphill = 0;
      }
    }
    
    /* Is there need to move the sensor again, or have we moved
      enough? */
    //DLI changed this condition to:  
    if (cam3a_af_state.search_mode == CAMERA_AF_MODE_NORMAL)
    {
      ok_to_move = (((cam3a_af_state.direction == CAMSENSOR_MOVE_FOCUS_FAR) &&
                ((cam3a_af_state.position + num_fine_steps_between_stat_points) 
                 <= cam3a_af_state.far_end) ) ||
               ((cam3a_af_state.direction == CAMSENSOR_MOVE_FOCUS_NEAR) &&
                ((cam3a_af_state.position - num_fine_steps_between_stat_points) >=
                 cam3a_af_state.near_end) )) && 
              (cam3a_af_state.num_downhill < downhill_allowance);
    }
    else
    {
      ok_to_move = (((cam3a_af_state.direction == CAMSENSOR_MOVE_FOCUS_FAR) &&
                ((cam3a_af_state.position + num_fine_steps_between_stat_points) 
                 <= cam3a_af_state.far_end) ) ||
               ((cam3a_af_state.direction == CAMSENSOR_MOVE_FOCUS_NEAR) &&
                ((cam3a_af_state.position - num_fine_steps_between_stat_points) >=
                 cam3a_af_state.near_end) ))           
              && (cam3a_af_state.index < num_fine_focal_positions) && 
              (cam3a_af_state.num_downhill < downhill_allowance);
    }

    if (ok_to_move)
    {
      camera_af_do_move_lens(cam3a_af_state.direction, num_fine_steps_between_stat_points);
      just_moved = 1;
    }

    // REVISIT - gather some number for fine focus
    //DLI changed for corner case where the last step was just check, 
    //this will become true and stats won't be update
    if (cam3a_af_state.search_mode == CAMERA_AF_MODE_NORMAL)

    {
       if(cam3a_af_state.in_low_light)
        {

           stop_condition = (((cam3a_af_state.position+(num_fine_steps_between_stat_points-1) >= cam3a_af_state.far_end) ||
                            (cam3a_af_state.position-(num_fine_steps_between_stat_points-1) <= cam3a_af_state.near_end)) && 
                            (just_moved == 0)) ||
                            (cam3a_af_state.num_downhill >= downhill_allowance);
        }

      else

      {

          stop_condition = (((cam3a_af_state.position >= cam3a_af_state.far_end) ||
                           (cam3a_af_state.position <= cam3a_af_state.near_end)) && 
                          (just_moved == 0)) ||
                          (cam3a_af_state.num_downhill >= downhill_allowance);

      }

    }

    else

    {
      if(cam3a_af_state.in_low_light)

      {
          stop_condition = (((cam3a_af_state.position+(num_fine_steps_between_stat_points-1) >= cam3a_af_state.far_end) ||
                            (cam3a_af_state.position-(num_fine_steps_between_stat_points-1) <= cam3a_af_state.near_end)) && 
                            (just_moved == 0)) ||
                            ((cam3a_af_state.index >= num_fine_focal_positions) &&
                            (!camctrl_tbl.undershoot_protect)) ||
                            ((cam3a_af_state.index >= num_fine_focal_positions + camctrl_tbl.undershoot_adjust) &&
                            (camctrl_tbl.undershoot_protect)) ||
                            (cam3a_af_state.num_downhill >= downhill_allowance);

      }

      else

      {

        stop_condition = (((cam3a_af_state.position >= cam3a_af_state.far_end) ||
                           (cam3a_af_state.position <= cam3a_af_state.near_end)) && 
                           (just_moved == 0)) ||
                           ((cam3a_af_state.index >= num_fine_focal_positions) &&
                           (!camctrl_tbl.undershoot_protect)) ||
                          ((cam3a_af_state.index >= num_fine_focal_positions + camctrl_tbl.undershoot_adjust) &&
                           (camctrl_tbl.undershoot_protect)) ||
                          (cam3a_af_state.num_downhill >= downhill_allowance);

      }

    }

 


    if (stop_condition)
    {
      int32 moveBackSteps;
     
      /* Select bin for finer search */
      //DLI change here to accommodate two-modes
      //first to see if the min and max FV are meaningful in the NORMAL mode only
      if ((cam3a_af_state.search_mode == CAMERA_AF_MODE_NORMAL) && 
          ((cam3a_af_state.focusvalue_max < MODE_CHANGE_THRESH+200) || 
           (cam3a_af_state.focusvalue_min > 
            ((float)cam3a_af_state.focusvalue_max*CONFIDENCE_LEVEL_EXH)) ))
      {
        //FV_min and FV_max are too close, stay at the maximum position, & quit
        //Need to report to UI that AF is with low confident
        MSG_AFDEBUG("Warning: FVs not reliable, Move to Maximum location and quit %d, %d, %d", cam3a_af_state.focusvalue_max, cam3a_af_state.focusvalue_min, cam3a_af_state.position);
      }
      moveBackSteps = cam3a_af_state.position - cam3a_af_state.position_max;
      if ( cam3a_af_state.skip_frame_delay)
      {
        if ((moveBackSteps !=0) &&
            ((cam3a_af_state.index -1) * num_fine_steps_between_stat_points > abs(moveBackSteps)))
        {
          moveBackSteps += (moveBackSteps > 0)?
            num_fine_steps_between_stat_points:-num_fine_steps_between_stat_points;
        }
      }
      /* if position_max == FAR_END or NEAR_END, do_safe_move should be able to cover*/       
      if (moveBackSteps > 0)
      {
        cam3a_af_state.state = AF_FOCUSING;
        camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_NEAR, moveBackSteps);
      }
      else if (moveBackSteps < 0)
      {
        cam3a_af_state.state = AF_FOCUSING;
        camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_FAR, -moveBackSteps);
      }
      else
      {
        /* we are done */
        if (cam3a_af_state.collect_end_stat)
        {
          cam3a_af_state.state = AF_COLLECT_END_STAT;
        }
        else
        {
          cam3a_af_state.state = AF_INACTIVE;
          if (cam3a_af_state.callback)
          {
            (cam3a_af_state.callback) (CAMERA_EXIT_CB_DONE, 
                                       cam3a_af_state.client_data, 
                                       CAMERA_FUNC_START_FOCUS, 0);
            cam3a_af_state.callback = NULL;
          }
          MSG_AFDEBUG("Took AF %d moves to focus the lens", cam3a_af_state.stats_moved, 0, 0);
        }
      }

#ifdef FEATURE_CAMERA_MOBICAT
      cam3a_af_state.tracing_info_focusing_time = 
      camera_3a_shared_get_time_ms() - 
      cam3a_af_state.tracing_info_focusing_time;
#endif
    }
  }
  else if (cam3a_af_state.state == AF_COLLECT_END_STAT)
  {
    if (cam3a_af_state.frame_delay > -2)
    {
      MSG_AFDEBUG("Frame Delays, Focusvalue, and Nlines %d, %d, %d", cam3a_af_state.frame_delay, vfeStatStruct->Focus/ vfeStatStruct->NFocus, cam3a_af_state.position);
      cam3a_af_state.frame_delay--;
      return CAMERA_SUCCESS;
    }
    
    /* Store 27 bit focus value divided by num valid rows to get an average */
    cam3a_af_state.locations[cam3a_af_state.index]=cam3a_af_state.position;   
    cam3a_af_state.stats[cam3a_af_state.index++]=
      (vfeStatStruct->Focus/vfeStatStruct->NFocus);

    MSG_AFDEBUG("Index, Focusvalue, and LensPosition %d, %d, %d",
                 cam3a_af_state.index-1, vfeStatStruct->Focus/ vfeStatStruct->NFocus, cam3a_af_state.position);  

    /* Change state, we are done  */
    cam3a_af_state.state = AF_INACTIVE;

    MSG_AFDEBUG("Took AF %d moves to focus the lens", 
               cam3a_af_state.stats_moved, 0, 0);

    if (cam3a_af_state.callback)
    {
      /* AF Completed successfully */
      (cam3a_af_state.callback) (CAMERA_EXIT_CB_DONE, 
                                 cam3a_af_state.client_data, 
                                 CAMERA_FUNC_START_FOCUS, 0);
      cam3a_af_state.callback = NULL;
    }

  }
  return ret_val;
}




/*===========================================================================

FUNCTION    CAMERA_AF_INIT_PROCESS_EXHAUSTIVE_SEARCH

DESCRIPTION
  configure af values

DEPENDENCIES

RETURN VALUE

LIMITATIONS
  currently, all the values are hard-coded.
  these values need to be dependent on sensors.

SIDE EFFECTS
  None

===========================================================================*/
void 
camera_af_init_process_exhaustive_search(const camsensor_static_params_type 
                                         * camsensorParamsPtr)
{

  MSG_LOW("Inside af_init_process_exhaustive_search", 0,0,0);  

  if (camsensorParamsPtr == NULL) return;

  cam3a_af_state.state = AF_INACTIVE;
  downhill_allowance = 3;
  fv_drop_allowance = 0.5;
  uphill_threshold = 3;
}
#endif /* CAMERA_AF_USE_EXHAUSTIVE_SEARCH */


#ifdef CAMERA_AF_USE_HILL_CLIMBING
#error code not present
#endif /* CAMERA_AF_USE_HILL_CLIMBING */



#ifdef CAMERA_AF_USE_FULL_SWEEP
/*===========================================================================

FUNCTION    CAMERA_AF_DO_PROCESS_FULL_SWEEP

DESCRIPTION
  Search the entire range exhaustively with the smallest step size
  Not intend to use as a practical method, only for test reference

DEPENDENCIES

RETURN VALUE
   CAMERA_SUCCESS or CAMERA_FAIL
LIMITATIONS
  this search happens only in AF_TEST mode

SIDE EFFECTS
  None  
===========================================================================*/
/*lint -save -e818 camsensorParamsPtr could be declared as pointing to const */
camera_ret_code_type 
camera_af_do_process_full_sweep (const camsensor_static_params_type 
                                 * camsensorParamsPtr)
{
  camera_ret_code_type  ret_val = CAMERA_SUCCESS;
  int32 just_moved = 0;
  int32 num_fine_steps_between_stat_points;
  int32 steps_to_move;  

  if (cam3a_af_state.move_lens_done == FALSE)
  {
    cam3a_af_state.frame_delay--;


    if (cam3a_af_state.frame_delay < -LENS_DONE_MOVE_THRESH)
    {
      // something is wrong, we never got
      // acknoweledgment that lens is done
      // moving
      cam3a_af_state.move_lens_done = TRUE;
    }
    return CAMERA_SUCCESS; 
  }
  if (camsensorParamsPtr == NULL) return CAMERA_FAILED;

  /* Set in camsensor layer - this could be thought of as the AF
    aggressiveness */  
  num_fine_steps_between_stat_points = 
  camsensorParamsPtr->num_fine_steps_between_stat_points;
  cam3a_af_state.near_end = camsensorParamsPtr->position_near_end;
  cam3a_af_state.far_end = camsensorParamsPtr->position_far_end;

  // REVISIT: should add error handling to cap num gross or fine points
  // gathered to 10

  if ((vfeStatStruct == NULL) ||
      (cam3a_af_state.step_sensor_failure_cnt > CAM3A_NUM_FOCUS_ATTEMPTS))
  {
    if (cam3a_af_state.step_sensor_failure_cnt > CAM3A_NUM_FOCUS_ATTEMPTS)
    {
      MSG_AFDEBUG ("Focus, failed after %d attempt to move sensor position",
                CAM3A_NUM_FOCUS_ATTEMPTS,0, 0);
      cam3a_af_state.state = AF_ERROR;
    }
    else
    {
      cam3a_af_state.state = AF_INACTIVE;
    }
    /* Exit failure */
    if (cam3a_af_state.callback)
    {
      (cam3a_af_state.callback) (CAMERA_EXIT_CB_FAILED, 
                                 cam3a_af_state.client_data,
                                 CAMERA_FUNC_START_FOCUS, 0);
    }
    return CAMERA_FAILED;
  }

  if (cam3a_af_state.state == AF_START)
  {
    cam3a_af_state.stats_moved = 0;

    /* Keep same state, take no action just decrment
      frame delay count.  Thought is that the stats
      for this frame are not trustworth - so wait
      until they are */
    MSG_AFDEBUG ("AF Started here, takes %d ms to get here from service, %d, %d", 
              camera_3a_shared_get_time_ms()-cam3a_af_state.af_start_time, 0, 0); 
    cam3a_af_state.frame_delay = 0; //Frame_delay doesn't make any sense here
    MSG_AFDEBUG("FULL SWEEP STARTS, Near_end, Far_end, %d, %d, %d ", cam3a_af_state.near_end,
               cam3a_af_state.far_end, 0);

    /* Get the sensor to a known position */

    if (cam3a_af_state.position <= cam3a_af_state.far_end)
    {
      if ((camctrl_tbl.undershoot_protect) &&
          (cam3a_af_state.far_end == camsensorParamsPtr->position_far_end))
      {
        MSG_AFDEBUG("orig position is %d", cam3a_af_state.position, 0, 0);
        /* to compensate initial len movement undershoot multiply delta by 1.6 */
        steps_to_move = (cam3a_af_state.far_end - cam3a_af_state.position) * 8 / 5;
        steps_to_move = (steps_to_move >= 2)? steps_to_move : 2; /* at least 2 steps */
        if (steps_to_move > cam3a_af_state.far_end)
        {
          steps_to_move = cam3a_af_state.far_end; /* don't want negative position */
        }
        cam3a_af_state.position = cam3a_af_state.far_end - steps_to_move;
        MSG_AFDEBUG ("changed position to %d", cam3a_af_state.position, 0, 0);
      }
      else
      {
        steps_to_move = cam3a_af_state.far_end - cam3a_af_state.position;
      }
             
      camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_FAR, steps_to_move);
      just_moved = 1; //DLI added
    }
    else if (cam3a_af_state.position > cam3a_af_state.far_end)
    {
      MSG_AFDEBUG ("AF_START Warning, Lens Position is larger than default_focus, %d, %d", 
                   cam3a_af_state.position, cam3a_af_state.far_end, 0);
      MSG_AFDEBUG ("Lens_pos > Default, can not trust, change from %d to %d", 
                   cam3a_af_state.position, 0, 0);
      cam3a_af_state.position = 0;
      camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_FAR,
                             cam3a_af_state.far_end - cam3a_af_state.position);
      just_moved = 1; //DLI added
    }

    cam3a_af_state.state = AF_GATHER_STATS_FINE;
    cam3a_af_state.direction = CAMSENSOR_MOVE_FOCUS_NEAR; 
    //DLI changes end here
    cam3a_af_state.index = 0;
    //DLI added
    cam3a_af_state.focusvalue_max = 0;

    if (just_moved == 1)
    {  
       MSG_AFDEBUG ("Frame Delays after initialization %d, %d, %d", 
            cam3a_af_state.frame_delay, vfeStatStruct->Focus/ vfeStatStruct->NFocus, 
            cam3a_af_state.position);
       return CAMERA_SUCCESS; //DLI added above two lines, to save one frame if the lens didn't move
    }  
  }
  else if (cam3a_af_state.state == AF_GATHER_STATS_FINE)
  {
    //NOTE: DLI replaces all the move direction to cam3a_af_state.direction
    /* Keep same state, take no action just decrment
      frame delay count.  Thought is that the stats
      for this frame are not trustworth - so wait
      until they are */
    if (cam3a_af_state.frame_delay > -2)
    {
      if (cam3a_af_state.index == 0)
        MSG_AFDEBUG("Frame Delays, FINE MODE,  and Nlines %d, %d, %d", cam3a_af_state.frame_delay, vfeStatStruct->Focus/ vfeStatStruct->NFocus, cam3a_af_state.position);
      cam3a_af_state.frame_delay--;
      return CAMERA_SUCCESS;
    }

    if (cam3a_af_state.index < CAM3A_NUM_AF_COLLECTION_POINTS)
    {
      /* Store 27 bit focus value divided by num valid rows
        to get an average */
      cam3a_af_state.stats[cam3a_af_state.index++] = 
      (int32)(vfeStatStruct->Focus / vfeStatStruct->NFocus);
      MSG_AFDEBUG("Index, Focusvalue, and LensPosition %d, %d, %d", cam3a_af_state.index, 
         vfeStatStruct->Focus/ vfeStatStruct->NFocus, cam3a_af_state.position);  
      if (cam3a_af_state.stats[cam3a_af_state.index-1] > cam3a_af_state.focusvalue_max)
      {
        cam3a_af_state.focusvalue_max = cam3a_af_state.stats[cam3a_af_state.index-1];
        cam3a_af_state.position_max = cam3a_af_state.position;
      }   
    }

    /* Is there need to move the sensor again, or have we moved
      enough? */
    //DLI changed this condition to:  
    if ((cam3a_af_state.position - num_fine_steps_between_stat_points) >=
           cam3a_af_state.near_end)
    {
      camera_af_do_move_lens(cam3a_af_state.direction, num_fine_steps_between_stat_points);
      just_moved = 1;
    }    

    // REVISIT - gather some number for fine focus
    //DLI changed for corner case where the last step was just check, 
    //this will become true and stats won't be update
    if ( (cam3a_af_state.position <= cam3a_af_state.near_end) && 
         (just_moved == 0))
    {
      if (cam3a_af_state.position_max > cam3a_af_state.position)
      {
        camera_af_do_move_lens(CAMSENSOR_MOVE_FOCUS_FAR,
                               cam3a_af_state.position_max - cam3a_af_state.position);
      }
      /* Change state, we are done  */
      if (cam3a_af_state.collect_end_stat)
      {
        cam3a_af_state.state = AF_COLLECT_END_STAT;
      }
      else
      {
        cam3a_af_state.state = AF_INACTIVE;
        if (cam3a_af_state.callback)
        {
          (cam3a_af_state.callback) (CAMERA_EXIT_CB_DONE, 
                                     cam3a_af_state.client_data, 
                                     CAMERA_FUNC_START_FOCUS, 0);
          cam3a_af_state.callback = NULL;
        }
      }
    }
  }
  else if (cam3a_af_state.state == AF_COLLECT_END_STAT)
  {
    if (cam3a_af_state.frame_delay > -2)
    {
      MSG_AFDEBUG("Frame Delays, Focusvalue, and Nlines %d, %d, %d",
                  cam3a_af_state.frame_delay,
                  vfeStatStruct->Focus/ vfeStatStruct->NFocus,
                  cam3a_af_state.position);
      cam3a_af_state.frame_delay--;
      return CAMERA_SUCCESS;
    }
    
    /* Store 27 bit focus value divided by num valid rows to get an average */
    cam3a_af_state.locations[cam3a_af_state.index]=cam3a_af_state.position;   
    cam3a_af_state.stats[cam3a_af_state.index++]=
      (vfeStatStruct->Focus/vfeStatStruct->NFocus);

    MSG_AFDEBUG("Index, Focusvalue, and LensPosition %d, %d, %d",
                cam3a_af_state.index-1, vfeStatStruct->Focus/ vfeStatStruct->NFocus, cam3a_af_state.position);  
    MSG_AFDEBUG("The maxFocusvalue, and LensPosition %d, %d",  
                  cam3a_af_state.focusvalue_max, cam3a_af_state.position_max, 0);

    /* Change state, we are done  */
    cam3a_af_state.state = AF_INACTIVE;

    MSG_AFDEBUG("Took AF %d moves to focus the lens", 
               cam3a_af_state.stats_moved, 0, 0);

    if (cam3a_af_state.callback)
    {
      /* AF Completed successfully */
      (cam3a_af_state.callback) (CAMERA_EXIT_CB_DONE, 
                                 cam3a_af_state.client_data, 
                                 CAMERA_FUNC_START_FOCUS, 0);
      cam3a_af_state.callback = NULL;
    }

  }
  return ret_val;
}
#endif /* CAMERA_AF_USE_FULL_SWEEP */

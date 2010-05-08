#ifndef CAMERA_AF_H
#define CAMERA_AF_H
/*=============================================================================
FILE:       camera_af.c

SERVICES:   Auto Focus Header File

DESCRIPTION: 

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/af/main/latest/inc/camera_af.h#1 $ $DateTime: 2008/05/09 16:27:59 $ $Author: karenp $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 08/18/06   jn       Split camera_af into camera_af and camera_af_core (algorithm).
 07/18/06   jn       AF modulization.
 07/03/06   jn       Added Bestshot support.
 03/09/06   jn       Renamed some AF states.
 03/01/06   jn       Removed unused AF states.
 02/28/06   jn       Defined MSG_AFDEBUG.
 02/21/06   jn       Added camera_af_process_lens_move_done().
 02/16/06   dle      Linted.
 12/03/05   klee     clean-up the code, fix lint errors and check-in for 4.3 release
                     lint error 818 has been simply surpressed for all over the file.
 12/03/05   klee     variables are added for enhanced hill-climbing AF
 1/04/05    babakf   Added AF_MODE_MACRO_HARD and AF_MODE_NORMALHARD to 
                     distiguish between soft and hard normal/macro mode.
                     Made several global variables used by camera_af.c part
                     of state struct
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

/*==============================================================================
  FEATURES
==============================================================================*/

/*==============================================================================
  CONSTANTS and MACROS
==============================================================================*/
#define CAM3A_NUM_FOCUS_ATTEMPTS          100
#define CAM3A_MAX_NUM_FRAMES_TO_WAIT_FOR_AEC_TO_SETTLE 15
#define CAM3A_MAX_FRAMES_TO_PERFORM_FOCUS (300+CAM3A_NUM_FOCUS_ATTEMPTS+CAM3A_MAX_NUM_FRAMES_TO_WAIT_FOR_AEC_TO_SETTLE)
#define CAM3A_NUM_AF_COLLECTION_POINTS 30
#define CAM3A_AF_FRAME_DELAY           2

#define AF_RESTORE_E_CB 0x1

#define THRESHOLD_IN_NOISE      (0.05)
#define THRESHOLD_FV_RATIO      (0.1)
#define CONFIDENCE_LEVEL        (0.7) /* DLI added */
#define CONFIDENCE_LEVEL_EXH    (0.9) /* DLI added */
#define REFOCUSING_CHECK_INDEX  4  /* DLI changed for speed up */
#define MODE_CHANGE_THRESH      300 /*DLI added for switching two mode after frist two FVs*/
#define LENS_DONE_MOVE_THRESH   400  // how long do we wait for lens done msg

#define MSG_AFDEBUG(x,a,b,c)    MSG_HIGH(x,a,b,c)

/*==============================================================================
  DATA TYPES
==============================================================================*/
typedef enum
{
  AF_INACTIVE = 0,            /* Default focused state, at the edge of infinity */
  AF_START,                   /* start state */
  AF_DECISION,                /* decision making state */
  AF_EXHAUSTIVE,
  AF_GATHER_STATS_COARSE,     /* gather stats with big steps */
  AF_GATHER_STATS_FINE,       /* gather stats with small steps */
  AF_FOCUSING,                /* found lens focus position*/
  AF_UNABLE_TO_FOCUS,         /* unable to focus due to noise */
  AF_ERROR,                   /* error state */
  AF_COLLECT_END_STAT,        /* collect end stat */
  //
  AF_WAIT_FOR_AEC_TO_SETTLE,
  AF_NUM_STATES
} cam3a_af_state_enum_type;

typedef enum
{
  AF_FRAME_DELAY = 0,   
  AF_FRAME_NODELAY
} cam3a_af_frame_mode;

typedef struct
{
  boolean is_enabled;
  /* Array of focus numbers at particular focal points */
  int32 stats[CAM3A_NUM_AF_COLLECTION_POINTS];     
  int32 index;                       /* Index into stats array above */
  /* How many frames since AF operation was requested */
  int32 elapsed_frame_cnt;           

  int32 near_end;
  int32 far_end;
  int32 position;                    /* Current sensor position */
  int32 boundary;
  int32 default_focus;
  int32 infinity_position;
  boolean move_lens_done;
  int32 stats_moved;
  uint32 af_start_time;
  boolean af_test_mode;
  boolean collect_end_stat;
  int32 frame_delay;
  cam3a_af_state_enum_type state;
  camera_cb_f_type  callback;
  void *client_data;
  int32 step_sensor_failure_cnt;
  /* for hill-climbing */
  int32 focusvalue_max;
  int32 focusvalue_min; /* to monitor minimum FV */
  int32 focusvalue_cur;
  int32 position_max;
  camera_af_mode_type search_mode;
  camsensor_move_focus_enum_type direction;
  int32 fine_index;
  int32 frame_cnt;      /* TO count how many frames needed*/
  //int32 topeakdist; /*distance between current position and peak position*/
  //int32 downhill;     /*the fv decrease index*/
  //int32 uphill;
  boolean skip_frame_delay;  /* used in fast exhaustive algorithm */
  int32 locations[CAM3A_NUM_AF_COLLECTION_POINTS];
  int32 num_downhill;
  int32 num_uphill;
  cam3a_af_process_enum_type af_process_type;
  cam3a_af_process_enum_type af_process_type_org; //process type before full sweep
  boolean in_low_light;
  boolean state_changed;
#ifdef FEATURE_CAMERA_MOBICAT
  /* AF tracing info */
  int32 tracing_info_index;
  int32 tracing_info_stats[CAM3A_NUM_AF_COLLECTION_POINTS]; 
  int32 tracing_info_n_focus[CAM3A_NUM_AF_COLLECTION_POINTS]; 
  int32 tracing_info_positions[CAM3A_NUM_AF_COLLECTION_POINTS]; 
  int32 tracing_info_total_movement_trial;
  int32 tracing_info_total_movement_lens;
  uint32 tracing_info_focusing_time;
#endif
} cam3a_af_state_struct;

/*==============================================================================
  EXTERNAL VARIABLES
==============================================================================*/
extern cam3a_af_state_struct  cam3a_af_state;

/*==============================================================================
  EXTERNAL API DECLARATIONS
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
void camera_af_init (void);

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
);

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
);

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
void camera_af_process_stats_complete_msg
(
  const CAMQDSP_AFStatsMessageType * msg
);

/*===========================================================================

FUNCTION      CAMERA_AF_PROCESS_FOCUS_SENSOR

DESCRIPTION
  Focus the sensor by gathering stats over a range of positions and
  selecting the best position.  Is complete when state transitions to
  AF_INACTIVE
  Tuned for the Casio NX5300 CCD sensor right now (but should work for any CCD)
  Algorithm:
  AF_START state
  -Input position is set back to the edge of infinity first (position must reflect
   the actual sensor's physical focus position)
  AF_GATHER_STATS_COARSE_DELAY state
  -Here we delay 1 AF frame (or two actual frames) because the motion from the
   start state takes time to settle & accurate stats may not be gathered without
   this delay.  This was found empirically on the Casio AF sensor
  AF_GATHER_STATS_COARSE state
  -Here, starting at the edge of infinity, we save the focus stat into an array
   & move the sensor closer in (near).  We only save stats if X number of horizontal
   lines participated in the stats.  We move by a coarse measure each time.  
   When we've reached the nearest edge, we search back & fine the best focal point 
   of the group.  We then move the sensor back to that point - coarse/2 lengths, so
   that the middle of the course measure is searched over.
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
camera_ret_code_type camera_af_process_focus_sensor (
  const camsensor_static_params_type * camsensorParamsPtr
  );

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
int32 camera_af_get_focus_position ( void );

/*===========================================================================

FUNCTION      CAMERA_AF_SET_FOCUS_POSITION

DESCRIPTION
  Set position of sensor based on passed in value

CALLERS:
  cam3a_set_focus_position()

DEPENDENCIES

RETURN VALUE
  TRUE if operation completes successfully
  FALSE if not

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_af_set_focus_position( int32 position );

/*===========================================================================
 *
 * FUNCTION    CAMERA_AF_DO_MOVE_LENS
 *
 * DESCRIPTION
 *   wrapper to decide whether move lens in camera_drv_task context
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
  int32 num_steps
);

/*===========================================================================
 *
 * FUNCTION    CAMERA_AF_DO_SAFE_MOVE
 *
 * DESCRIPTION
 *   make movement only between near_end and far_end
 *   num_steps may be adjusted, if exceeds.
 *
 * DEPENDENCIES
 *
 * RETURN VALUE
 *   CAMERA_SUCCESS or CAMERA_FAILED
 *
 * LIMITATIONS
 *
 * SIDE EFFECTS
 *   None
 *
 *===========================================================================*/
void camera_af_do_safe_move
(
  camsensor_move_focus_enum_type direction,
  int32 num_steps
);

/*===========================================================================

FUNCTION    CAMERA_AF_PROCESS_LENS_MOVE_DONE

DESCRIPTION
  Continue with AF after lens move is done.

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
);

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
);

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
 *   None
 *
 * LIMITATIONS
 *
 * SIDE EFFECTS
 *   None
 *
 *===========================================================================*/
camera_ret_code_type camera_af_move_lens_to
(
  int32 position
);

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
);

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
);

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
);

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
);

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
);
     
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
);

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
);

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
);

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
);

#endif /* CAMERA_AF_H */

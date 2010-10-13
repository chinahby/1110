/*=============================================================================
 *
FILE:       camera_af.c

SERVICES:   Auto Focus file

DESCRIPTION: 

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/af/main/latest/src/camera_af_core.h#1 $ $DateTime: 2008/05/09 16:27:59 $ $Author: karenp $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 08/18/06   jn       Split camera_af into camera_af and camera_af_core (algorithm).
 
==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/


/*===========================================================================

FUNCTION    CAMERA_AF_DO_PROCESS_EXHAUSTIVE_SEARCH

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
   Manual sensor motor change position was requested & is queued in camera_drv_task 
   context

  AF_START
   Input position is set back to the edge of infinity first (position must 
   reflect the actual sensor's physical focus position)

  AF_GATHER_STATS_COARSE
   Here, starting at the edge of infinity, we save the focus stat into an array
   & move the sensor closer in (near).  We only save stats if X number of 
   horizontal lines participated in the stats.  We move by a coarse measure 
   each time.  When we've reached the nearest edge, we search back & fine the 
   best focal point of the group.  We then move the sensor back to that point 
   - coarse/2 lengths, so that the middle of the course measure is searched 
   over.  We also delay the appropriate number of frames between movements of 
   the sensor's stepping motor (this number is sensor dependent)

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
                                        * camsensorParamsPtr);

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
                                         * camsensorParamsPtr);

/*===========================================================================

FUNCTION    CAMERA_AF_DO_PROCESS_HILL_CLIMBING

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
   Manual sensor motor change position was requested & is queued in camera_drv_task 
   context

  AF_START
   Input position is set back to the edge of infinity first (position must 
   reflect the actual sensor's physical focus position)

  AF_GATHER_STATS_COARSE
   Here, starting at the edge of infinity, we save the focus stat into an array
   & move the sensor closer in (near).  We only save stats if X number of 
   horizontal lines participated in the stats.  We move by a coarse measure 
   each time.  When we've reached the nearest edge, we search back & fine 
   the best focal point of the group.  We then move the sensor back to that 
   point - coarse/2 lengths, so
   that the middle of the course measure is searched over.
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
camera_ret_code_type camera_af_do_process_hill_climbing (void);

/*===========================================================================

FUNCTION    CAMERA_AF_INIT_PROCESS_HILL_CLIMBING

DESCRIPTION
  configure af values

DEPENDENCIES

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void 
camera_af_init_process_hill_climbing(const camsensor_static_params_type 
                                     * camsensorParamsPtr);

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
camera_ret_code_type camera_af_do_process_full_sweep
(
const camsensor_static_params_type *camsensorParamsPtr
);


/*=============================================================================
FILE:       camera_aec.c

SERVICES:   Auto Exposure File

DESCRIPTION: 

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/aec/main/latest/src/camera_aec.c#13 $ $DateTime: 2009/04/23 21:25:23 $ $Author: c_caluru $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ---------------------------------------------------------- 
 04/22/09   cg       Fixed the issue with Multishot when linear AFR is enabled.
 04/14/09   cg       For 0x203 header nightshot_fps should be used from chromatix header
 04/07/09   cg       Re-Enabling Feature FEATURE_CAMERA_BURST_MODE 
 12/03/09   cg       Fixed the featurization issues
 02/12/08   cg       Fix the issue when antibanding is set before nightshot mode.
 12/24/08   cg       Changes For Ez Tune 1.0 and AEC Multishot 
 03/11/08   cg       Updates for Ez Tune 1.0,3A 1.4 and 0x203 header
 01/10/08   sm       added return INVALID_PARAM for invalid params in camera_aec_set_nightshot_mode
 11/08/07   hap      Fixing RVCT2.2 compiler warnings
 08/17/07   smn      Fixed initialization error
 07/25/07   smn      Changed default value to a define and removed tabs.
 07/20/07   smn      Changed camera_aec_prepare_for_hjr for cutting exposure time to
 05/09/07   klee     Enhanced AWB preview with color correction bias interpolation
 03/30/07   klee     Clean-up lint errors due to un-matched data type in 3a
 03/29/09   klee     Combined camera_aec_retrive_exposure_info 
                     and camera_aec_get_exposure_info_for_gamma_interpolation
 03/28/07   klee     Enhancement on gamma table interpolation
 03/28/07   klee     Modified the name awb_input_exposure_type to be camera_exposure_info_type
 03/15/07   klee     Updated preview re-configuration on re-starting
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 02/20/07   klee     Fixed all lint errors (critical, high, medium, low and lint)
 01/31/07   klee     Fixed CR 110947 Nightshot mode is ignored when AFR is ON 
 01/09/07   klee     Enhanced AEC with digital gains
 01/05/07   klee     Fixed flickering issue with AEC digital gain
 11/10/06   ven      Removed the temporary latency introduced for HJR.
 11/03/06   klee     Beautified the code by indenting and removing special characters
 11/02/06   klee     Added AEC Digital gain implementation
 10/27/06   dlee     In _aec_config(), _aec_adjust_exposure_for_snapshot() and
                     _aec_adjust_exposure_as_per_hjr(), set
                     _aec_report_vfe_stable to force one report after VFE is
                     stable.
                     In _aec_adjust_exposure(), if camera_aec_report_vfe_stable
                     and cam3a_aec_state.luma_settled_cnt is non-zero, report
                     once using MSG_HIGH.
 10/24/06   klee     Updated AEC aggressiveness to be tunable depending on sensors
 10/20/06   jv      Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 10/16/06   ven      Featurized HJR
 10/11/06   ven      Added support for HJR tuning parameter to delay the
                     selection of multi frame registration based HJR.
 09/22/06   klee     Fix the bug on float->Q7 converting
 09/15/06   klee     Cleaned up the lint errors
 09/08/06   klee     Added exposure_index_step for fast convercenge
 09/08/06   klee     Updated camera_aec_adjust_exposure() to provide previous exposure_index for AWB
 08/31/06   klee     Bug fix on previous and current wb maintaining
 08/29/06   klee     Consolidated antibanding calculation
 08/18/06   klee     Fixed critical and high lint errors
 08/15/06   klee     Commented out camera_aec_set_nightshot_mode_legacy()
                     It is not used, but we keep the code
 08/11/06   klee     Cleaned up the code regarding tuning variables
                     - AEC
 08/10/06   klee     Cleaned up the code regarding tuning variables relocation 
                     - AWB/AEC
                     - Color matrix
 08/04/06   dl       Removed extern variable camctrl_tbl.
 07/27/06   ven      Since HJR and ISO are mutually exclusive, they are merged 
                     under a single set of selectable parameters from UI.
 07/26/06   klee     Moved out AEC core algorithm from camera_aec_core
 07/25/06   ven      Removed HJR structure from vfe_sw_feature_type and 
                     added to cam_ctrl_tbl
 07/25/06   klee     Moved indoor/outdoor_index_org into cam3a_aec_state
 07/24/06   ven      Added support for Antibanding for HJR
 07/19/06   klee     Fixed AFR bug - stuck mimimum after changing AFR range
 07/18/06   dle      Fixed another bug, need to update bias_table when
                     changing numberOfRegions.
 07/18/06   dle      Make VFE_4X4_SUPPRT a default.
                     Featurize for VFE_16X16_SUPPORT.
 07/15/06   ven      Cleaned Lint errors related to HJR
 07/13/06   ven      Moved hjr setup function to camera_aec.c
 07/12/06   dle      Re-do proviling.
 07/11/06   ven      Moved HJR AEC related code from camera svcs to aec
 07/11/06   klee     Enhanced the condition for AFR
 05/27/06   ven      Added UI support for HJR.
 05/09/06   klee     Fixed the bug in config function
 05/02/06   klee     Added camctrl fast convergence algorithm
                     Imported implementation on iso and nightshot mode from camera_svcs
                     Imported parm variables on iso and nightshot mode
                     Added set_parm and get_parm functions
 04/19/06   klee     Updated build_band_table().
                     This allow camsensor not to have aec_min_gain_step variable.
 04/14/06   klee     Added WB gain luma adjustment to vfe frame luma
                     - updated camera_aec_config() and 
                               camera_aec_adjust_exposure()
 03/30/06   klee     To handle high luma regions in indoor condition
                     added camera_aec_configure_snapshot_luma_target()
                     updated camera_aec_adjust_exposure_as_per_iso() and
                     camera_aec_calculate_current_luma()                      
                     This is for snapshot only
 03/03/06   klee     Updated camera_aec_implement() and camera_aec_config()
                     - current_vfe_luma is not available anymore
                     - current luma value need to be stored in cam3a_aec_state.current_luma
                     - cam3a_aec_state.current_luma should be initialized to be 128 in config()
 02/24/06   ven      Added support for profiling awb and aec
 02/02/06   dle      Temporily disable 16x16 so make sure check 8x8 first.
 12/20/05   sch      Updated night shot mode handler arguments.
 12/19/05   sch      Modifications for nightshot mode to go through cam3a 
                     to adjust the frame rate, indoor / outdoor indices.
 12/23/05   ven      Added boundary check to AFR to prevent oscillation.
 12/14/05   ven      Added support for VFE 4X4 Statistics collection.
 12/01/05   klee     fix lint error for 4.3 release
                     lint error 818 has been simply surpressed for all over the file.
 10/17/05   klee     clean up the lint error
 10/04/05   klee     Initial code transfered from cam3a 

==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#ifndef CAMERA_USES_SOFTDSP
#ifndef FEATURE_CAMERA_YCBCR_ONLY
#include "camera_3a_shared.h"
#include "camera_aec.h"
#include "camctrl.h"
#include "math.h"
#ifdef FEATURE_CAMERA_HJR
#include "camera_hjr.h"
#endif /* FEATURE_CAMERA_HJR */
#include "camera_aec_core.h"

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/
#ifdef FEATURE_CAMERA_HJR
#define HJR_LINE_COUNT_MIN_LIMIT 10
#endif /* FEATURE_CAMERA_HJR */
/*============================================================================
                        MACROS
============================================================================*/
#ifdef FEATURE_CAMERA_HJR
#define HJR_LUMA_THRESHOLD(x) ((x) * 0.8)
#endif /* FEATURE_CAMERA_HJR */
/*==============================================================================
  INTERNAL VARIABLES
==============================================================================*/
cam3a_aec_state_struct cam3a_aec_state;
static boolean camera_aec_report_vfe_stable = FALSE;
#ifdef FEATURE_CAMERA_HJR
HJR_struct HJR_structure;
#endif /* FEATURE_CAMERA_HJR */

#ifdef FEATURE_CAMERA_3A_PROFILING
static cam3a_timing_type cam3a_aec_timing;
#endif /* FEATURE_CAMERA_3A_PROFILING */

static camera_parm_info_type camera_parm_nightshot_mode;
static camera_parm_info_type camera_parm_iso;
#ifdef FEATURE_CAMERA_HJR
/* To override ISO if HJR is enabled */
static boolean hjr = FALSE;
#endif /* FEATURE_CAMERA_HJR */

/* Demonstraites a center-weighted exposure bias table where the center
   25% of the grid (& viewable area) gets 60% of the weight &
   the outlying areas get the remaining 40%.
   The sum of all entries must add up to the number
   of entries (64*256) in this case to assure uniform luma comparison results,
   if this isn't done, the result will be either a brigher or darker image) */
/* How to calculate this table:
   1) Start with the reference Frame Average table above (all 1's in Q0 format or
      "256" in Q8 format)
   2) Determine the weighting of each area
      For the center-weighted table here we use an inner window
      that comprises the inner 25% of the frame & is weighted
      60% of the total.  The outer window comprises 75% of the
      frame and is weighted 40%.  60 and 40% add to 100.
      Formula to apply to each entry:
      ((total num entries 64 or 256) * (weight for region)) / num entries in region
      =  ((64*.4) / 48) = 0.53333 [ for outer region with 48 points and 40% of the weighting]
      =  ((64*.6) / 16) = 2.4     [ for inner region with 16 points and 60% of the weighting]
      Verify these nubers by (48 * 0.5333) + (16 * 2.4) = 64
      ** Note that one cannot put all weight on one pixel - this may
      overflow the 16 bit unsigned value in the table entry.
   3) Now, convert all entries to Q8 format by multiplying by 256 and
      casting to an int.  This saves many cycles on the ARM by avoiding
      floating point math.
   4) The table below is for an 8x8 VFE grid, but we can easily scale up
      these results for a 16x16 VFE grid setting.
*/
#ifdef FEATURE_CAMERA_VFE_16X16_SUPPORT
LOCAL uint16 aec_center_weighted_16x16[256] = /* Q8 format */
{
/* Row 1 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 2 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 3 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 4 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 5; Row-wise, start of the inner region */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 6 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 7 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 8 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 9 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 10 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 11 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 12 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(2.4*256), (int)(2.4*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row-wise, end of the inner region */
/* Row 13 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 14 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 15 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row 16 */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256)
};
#endif /* FEATURE_CAMERA_VFE_16X16_SUPPORT */

LOCAL uint16 aec_center_weighted_8x8[64] = /* Q8 format */
{
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row-wise, start of the inner region */
  (int)(0.533*256), (int)(0.533*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(2.4*256), (int)(2.4*256),
  (int)(2.4*256), (int)(2.4*256), (int)(0.533*256), (int)(0.533*256),
/* Row-wise, end of the inner region */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256)
};

LOCAL uint16 aec_center_weighted_4x4[16] = /* Q8 format */
{
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),
/* Row-wise, start of the inner region */
  (int)(0.533*256), (int)(2.4*256), (int)(2.4*256), (int)(0.533*256),
  (int)(0.533*256), (int)(2.4*256), (int)(2.4*256), (int)(0.533*256),
/* Row-wise, end of the inner region */
  (int)(0.533*256), (int)(0.533*256), (int)(0.533*256), (int)(0.533*256),

};

/*
   Spot-Metering tables
      Formula to apply to each entry:
      ((total num entries 64 or 256) * (weight for region)) / num entries in region
      =  ((64*0.0) / 60) = 0.0 [ for outer region with 48 points and 40% of the weighting]
      =  ((64*1.0) / 4)  = 16.0     [ for inner region with 16 points and 60% of the weighting]
      Verify these nubers by (60 * 0.0) + (16 * 4) = 64
*/
#ifdef FEATURE_CAMERA_VFE_16X16_SUPPORT
LOCAL uint16 aec_spot_metering_16x16[256] = /* Q8 format */
{

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 1 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 2 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 3 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 4 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 5 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 6 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 7 */
/* Row-wise, start of the inner region */
  0, 0, 0, 0, 0, 0, 0, (64*256), (64*256), 0, 0, 0, 0, 0, 0, 0, /* row 8 */
  0, 0, 0, 0, 0, 0, 0, (64*256), (64*256), 0, 0, 0, 0, 0, 0, 0, /* row 9 */
/* Row-wise, end of the inner region */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 10 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 11 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 12 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 13 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 14 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* row 15 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* row 16 */
};
#endif /* FEATURE_CAMERA_VFE_16X16_SUPPORT */

LOCAL uint16 aec_spot_metering_8x8[64] = /* Q8 format */
{

  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
/* Row-wise, start of the inner region */
  0, 0, 0, (16*256), (16*256), 0, 0, 0,
  0, 0, 0, (16*256), (16*256), 0, 0, 0,
/* Row-wise, end of the inner region */
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

LOCAL uint16 aec_spot_metering_4x4[16] = /* Q8 format */
{

  0, 0, 0, 0, 
/* Row-wise, start of the inner region */
  0, (4*256), (4*256), 0,
  0, (4*256), (4*256), 0,
/* Row-wise, end of the inner region */
  0, 0, 0, 0

};


/*==============================================================================
  INTERNAL API DECLARATIONS
==============================================================================*/
static camera_ret_code_type camera_aec_auto_frame_rate(
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  );

static uint8 camera_aec_fps_index 
  (
  /* Q8 number */
  uint16 fps 
  );

static camera_ret_code_type camera_aec_adjust_exposure (
  uint32             num_pixels_per_region_aec,
/* num pixels used for exposure stats in each region */
  camsensor_static_params_type * camsensorParamsPtr,
/* Parameters for camsensor layer        */
  int32               numRegions
  );

static camera_ret_code_type camera_aec_set_iso_mode(camera_iso_mode_type iso_mode);
static camera_iso_mode_type camera_aec_get_max_iso_mode (void);
static camera_ret_code_type camera_aec_adjust_exposure_as_per_iso (void);

static camera_ret_code_type camera_aec_set_nightshot_mode (camera_nightshot_mode_type parm);
#if 0
static camera_ret_code_type camera_aec_set_nightshot_mode_legacy (camera_nightshot_mode_type parm);
#endif
/*==============================================================================
  INTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION    CAMERA_AEC_AUTO_FRAME_RATE

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
  camera_ret_code_type:
  CAMERA_FAILED  - if we were unable to update sensor
  CAMERA_SUCCESS - if everything worked

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type camera_aec_auto_frame_rate (
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  )
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;
  afr_output_type output;
  uint16 new_fps;
  int16 temp_index;
  /* signed number, will == 0 initially (no adjustment)
    If the code logic sets to 1 we increase FPS by 1 increment
    (defined in sensor's own frame_rate_table)
    If the code logic sets to -1 we decrease FPS by 1 increment
    (defined in sensor's own frame_rate_table)
  */

  if(camsensorParamsPtr == NULL) return CAMERA_FAILED;

  output = camera_aec_core_run_auto_frame_rate_algorithm(camsensorParamsPtr);

  if(output.afr_status == AFR_SKIP_FRAME)
    return CAMERA_SUCCESS;
  else if(output.afr_status == AFR_FAILED)
    return CAMERA_FAILED;

  /* signed number, will == 0 initially (no adjustment)
    If the code logic sets to 1 we increase FPS by 1 increment
    (defined in sensor's own frame_rate_table)
    If the code logic sets to -1 we decrease FPS by 1 increment
    (defined in sensor's own frame_rate_table)
  */
  /*      *** ADJUST FPS TO SLOWER RATE ***       */
  if((output.fps_adjustment == -1)  &&
    ((cam3a_aec_state.afr_index+1) < camsensorParamsPtr->num_possible_frame_rates))
  {

    /* Go to next (lower) frame rate */
    /* new_fps = camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index+1].fps; */
    /*lint -save -e734 */
    temp_index = cam3a_aec_state.afr_index + 1;
    /*lint -restore */
    new_fps = camsensorParamsPtr->frame_rate_array[temp_index].fps;
    if(new_fps < cam3a_aec_state.afr_min_fps)
    {
      ret_val = CAMERA_SUCCESS;
      new_fps =  camsensorParamsPtr->preview_fps;

    }
    else
    {
      ret_val = camsensor_set_frame_rate (new_fps);

      if(ret_val == CAMERA_SUCCESS)
      {
        camsensorParamsPtr->preview_fps = new_fps;

        /* compensate indoor/outdoor index */
        camera_aec_core_update_indoor_outdoor_indices(temp_index);

        /* Reset count since we changed FPS */
        cam3a_aec_state.frame_in_current_fps = 0;
        /* Change exposure index to try and compensate for lower frame rate */
        /* This faster_fps is appropriately negative to adjust to a lower frame rate */
        cam3a_aec_state.exp_index += 
          (int32)camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index].slower_fps_exp_table_index_mod;
        /* Insure we haven't exceeded exp index */
        if(cam3a_aec_state.exp_index > (int32)(cam3a_aec_state.num_exposure_table_values-1))
        {
          cam3a_aec_state.exp_index = (int32)(cam3a_aec_state.num_exposure_table_values-1);
        }

        /* Advance to next slower frame rate as the current one */
        cam3a_aec_state.afr_index++;
      }
    }
  }
  /*      *** ADJUST FPS TO FASTER RATE ***       */
  else if((output.fps_adjustment == 1) &&
    (cam3a_aec_state.afr_index > 0)    )
  /* Zero being the highest rate, cannot go higher */
  {
    /* Go to next (higher) frame rate */
    /* new_fps = camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index-1].fps; */
    /*lint -save -e734 */
    temp_index = cam3a_aec_state.afr_index - 1;
    /*lint -restore */
    new_fps = camsensorParamsPtr->frame_rate_array[temp_index].fps;

    if(new_fps > cam3a_aec_state.afr_max_fps)
    {
      ret_val = CAMERA_SUCCESS;
      new_fps =  camsensorParamsPtr->preview_fps;

    }
    else
    {
      ret_val = camsensor_set_frame_rate (new_fps);

      if(ret_val == CAMERA_SUCCESS)
      {
        camsensorParamsPtr->preview_fps = new_fps;

        /* compensate indoor/outdoor index */
        camera_aec_core_update_indoor_outdoor_indices(temp_index);

        /* Reset count since we changed FPS */
        cam3a_aec_state.frame_in_current_fps = 0;  /* Reset count since we changed FPS */
        /* Change exposure index to try and compensate for higher frame rate */
        cam3a_aec_state.exp_index += 
          (int32)camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index].faster_fps_exp_table_index_mod;
        /* Insure we haven't exceeded exp index */
        if(cam3a_aec_state.exp_index > (int32)(cam3a_aec_state.num_exposure_table_values-1))
        {
          cam3a_aec_state.exp_index = (int32)(cam3a_aec_state.num_exposure_table_values-1);
        }

        /* Advance to previous faster frame rate as the current one */
        cam3a_aec_state.afr_index--;
      }
    }
  }

  if(ret_val == CAMERA_FAILED)
  {
    MSG_HIGH ("Failed to adjust frame rate ",0,0,0);
  }
  else if(ret_val == CAMERA_NOT_SUPPORTED)
  {
    MSG_HIGH ("Adjust frame rate not supported in driver ",0,0,0);
  }

  cam3a_aec_state.afr_frame_rate = camsensorParamsPtr->preview_fps;

  return ret_val;
} /* cam3a_auto_frame_rate */

/*===========================================================================

FUNCTION    CAMERA_AEC_FPS_INDEX

DESCRIPTION
  Searches frame rate array for a specific fps. 

GLOBALS
  indoor_index_org
  outdoor_index_org

DEPENDENCIES

RETURN VALUE
  Index of the required fps in frame rate array.

SIDE EFFECTS
  None

===========================================================================*/
static uint8 camera_aec_fps_index 
  (
  /* Q8 number */
  uint16 fps
  )
{
  uint8 index = 0;

  if(camctrl_tbl.camsensor_params_ptr == NULL)
  {
    return index;
  }

  while(index < camctrl_tbl.camsensor_params_ptr->num_possible_frame_rates)
  {
    if(camctrl_tbl.camsensor_params_ptr->frame_rate_array[index].fps == fps)
    {
      return index;
    }

    ++index;
  }

  index = 0;

  return index;
} /* camera_fps_index */


/*==============================================================================
  EXTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_AEC_INIT

DESCRIPTION
   Memclears global and locally used structure for aec.
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
void camera_aec_init (void)
{
  /* Init AEC state struct */
  memset (&cam3a_aec_state, 0, sizeof(cam3a_aec_state_struct));

  camera_parm_nightshot_mode.minimum_value = (int)CAMERA_NIGHTSHOT_MODE_OFF;
  camera_parm_nightshot_mode.maximum_value = (int)CAMERA_NIGHTSHOT_MODE_ON;
  camera_parm_nightshot_mode.default_value = (int)CAMERA_NIGHTSHOT_MODE_OFF;
  camera_parm_nightshot_mode.current_value = (int)CAMERA_NIGHTSHOT_MODE_OFF;
  camera_parm_nightshot_mode.step_value    = 1;
  //
  camera_parm_iso.minimum_value = (int)CAMERA_ISO_AUTO;
  camera_parm_iso.maximum_value = (int)CAMERA_ISO_AUTO;
  camera_parm_iso.default_value = (int)CAMERA_ISO_AUTO;
  camera_parm_iso.current_value = (int)CAMERA_ISO_AUTO;
  camera_parm_iso.step_value    = 1;
}

/*===========================================================================

FUNCTION        CAMERA_AEC_MEM_ALLOCATE

DESCRIPTION:

CALLERS:
  cam3a_mem_allocate ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

=========================================================================== */
boolean camera_aec_mem_allocate (void)
{
  return camera_aec_core_mem_allocate();
}

/*===========================================================================

FUNCTION     CAMERA_AEC_MEM_DEALLOCATE

DESCRIPTION:

CALLERS:
  cam3a_mem_deallocate ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

=========================================================================== */
void camera_aec_mem_deallocate (void)
{
  camera_aec_core_mem_deallocate();
}

/*===========================================================================

FUNCTION      CAMERA_AEC_CONFIG

DESCRIPTION
   Initializes global and locally used structure.  Provides
   tuning parameters for specific implementations

CALLERS:
  cam3a_config ()

DEPENDENCIES
  None

GLOBALS

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_config (
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer */
  )
{
  camera_iso_mode_type temp_iso_mode;

  if(camsensorParamsPtr == NULL) return;

  /* Force a single report when luma_settled_cnt goes non-zero,
   * implying VFE is now stable. */
  camera_aec_report_vfe_stable = TRUE;

  cam3a_aec_state.camsensorParamsPtr = camsensorParamsPtr;
  /* Init AEC state struct */
  cam3a_aec_state.full_luma_array = FALSE;

  camera_parm_nightshot_mode.current_value = (int)CAMERA_NIGHTSHOT_MODE_OFF;

#ifdef FEATURE_CAMERA_3A_PROFILING
  /* Init AWB Processing Time calculation structures */
  cam3a_aec_timing.total_frames          = 0;
  cam3a_aec_timing.total_processing_time = 0;
#endif /* FEATURE_CAMERA_3A_PROFILING */

  /* reset aec parms */
  camera_parm_nightshot_mode.current_value = camera_parm_nightshot_mode.default_value;
  camera_parm_iso.current_value = camera_parm_iso.default_value;

  if(camsensorParamsPtr->aec_enable)
  {
    vfe_config_aec_per_chromatix(camsensorParamsPtr);

    /* AEC aggressiveness selection based on the setting in camctrl */
    cam3a_aec_state.aec_aggressiveness = camctrl_tbl.aec_aggressiveness;

    if((camsensorParamsPtr->pixel_clock == 0) ||
      (camsensorParamsPtr->pixel_clock_per_line == 0))
    {
      cam3a_aec_state.antibanding_enable = FALSE;
      MSG_ERROR ("Disabling antibanding, params are invalid %d %d",
        camsensorParamsPtr->pixel_clock, camsensorParamsPtr->pixel_clock_per_line, 0);
    }
    else
    {
      cam3a_aec_state.antibanding_enable = TRUE;
    }

    if(camsensorParamsPtr->num_possible_frame_rates > 0)
      cam3a_aec_state.afr_enable = TRUE;
    else
      cam3a_aec_state.afr_enable = FALSE;

    if(cam3a_aec_state.afr_max_fps ==0)
    cam3a_aec_state.afr_max_fps = camsensorParamsPtr->max_preview_fps;

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
    /*   --------------- AEC INIT---------------- */
    /*lint -save -e539 */
    camera_aec_core_init(camsensorParamsPtr);
    /*lint -restore */

    (void)camera_aec_set_aec_mode( CAMERA_AEC_CENTER_WEIGHTED, camsensorParamsPtr );

    /* Default exposure value, +/- bias value per sensor */
    camera_aec_set_brightness (cam3a_aec_state.default_luma_target);
    /* Set default exposure table to: antibanding off initially */
    (void)camera_aec_set_antibanding( CAMERA_ANTIBANDING_OFF, camsensorParamsPtr );

    temp_iso_mode = camera_aec_get_max_iso_mode();
    /*lint -save -e641 */
    if( (temp_iso_mode < CAMERA_ISO_MAX)&&(temp_iso_mode > CAMERA_ISO_AUTO) )
      camera_parm_iso.maximum_value = temp_iso_mode;
    /*lint -restore */

#ifdef FEATURE_CAMERA_HJR
    HJR_structure.gain_factor = HJR_DEFAULT_GAIN_FACTOR; /* default value */
#endif

    cam3a_aec_state.digital_gain = 1.0;
    cam3a_aec_state.prev_digital_gain = 1.0;
    cam3a_aec_state.digital_gain_for_snapshot = 1.0;
    cam3a_aec_state.stored_digital_gain = 1.0;
  }
}

/*===========================================================================

FUNCTION      CAMERA_AEC_ADJUST_EXPOSURE

DESCRIPTION
              Adjust exposure on the camera sensor.  Implement a 
              generic algorithm that can work with any sensor

CALLERS:
  cam3a_adjust_exposure ()

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type:
  CAMERA_FAILED  - if we were unable to update sensor
  CAMERA_SUCCESS - if everything worked

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e818*/
static camera_ret_code_type camera_aec_adjust_exposure(
  uint32             num_pixels_per_region_aec,
/* num pixels used for exposure stats in each region */
  camsensor_static_params_type * camsensorParamsPtr,
/* Parameters for camsensor layer        */
  int32               numRegions
  )
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;
  uint8 tmp_luma, black_level_updated;
  aec_output_type output;

  if(camsensorParamsPtr == NULL) return CAMERA_FAILED;

  tmp_luma = camera_aec_core_calculate_current_luma(
    numRegions,
    vfeStatStruct->SY,
    (int32)num_pixels_per_region_aec
    );

  black_level_updated = camera_3a_shared_update_black_level(cam3a_awb_state.black_level);
  if(tmp_luma > black_level_updated)
  {
    cam3a_aec_state.current_luma = tmp_luma - black_level_updated;
  }
  else
  {
    cam3a_aec_state.current_luma = 0;
  }

  /* Count frames in Auto Frame Rate */
  cam3a_aec_state.frame_in_current_fps++;
  cam3a_aec_state.prev_exp_index = cam3a_aec_state.exp_index;

  output = camera_aec_core_run_adjust_exposure_algorithm(cam3a_aec_state.current_luma);
  if(camera_aec_report_vfe_stable && cam3a_aec_state.luma_settled_cnt)
  {
    camera_aec_report_vfe_stable = FALSE;
    MSG_HIGH("VFE stablized.", 0, 0, 0);
  }

  cam3a_aec_state.current_register_gain = output.gain;
  if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
  {
    /* update digital gain with previous setting */
    cam3a_aec_state.digital_gain = cam3a_aec_state.stored_digital_gain;
    cam3a_aec_state.stored_digital_gain = output.digital_gain;
    if(cam3a_aec_state.current_line_count == output.line_count)
    {
      /* Update digital gain in VFE since there is no exposure time change remove frame lag */
      cam3a_aec_state.digital_gain = output.digital_gain;
    }
  }
  cam3a_aec_state.current_line_count = output.line_count;

#if defined  FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE && FEATURE_CAMERA_AEC_DURING_MULTISHOT*/
  {
  
    ret_val = camsensor_write_exposure_gain(output.gain, output.line_count);
    if(ret_val != CAMERA_SUCCESS)
    {
     cam3a_aec_state.exp_index = cam3a_aec_state.prev_exp_index;
    }
  }
 
  return ret_val;
} /* cam3a_adjust_exposure */
/*lint -restore*/
/*===========================================================================

FUNCTION      CAMERA_AEC_FLASH_NEEDED

DESCRIPTION
              See if flash is needed. The sensor device driver that supports
              auto flash should set camsensor_params->support_auto_flash to
              true.

CALLERS:
  cam3a_flash_needed ()

DEPENDENCIES
  Checks global AEC state struct for luma values

RETURN VALUE
  TRUE if flash is needed
  FALSE if not

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_aec_flash_needed(void)
{
  /* DIV By Zero Check added */
 if(cam3a_aec_state.current_luma !=0)
  {
    if(((int32)((cam3a_aec_state.luma_target / cam3a_aec_state.current_luma)*256)) >
    cam3a_aec_state.auto_flash_target_to_current_ratio) /* In Q8 format */
    {
    return TRUE;
    }
    else
    {
    return FALSE;
    }
  }
 else
  {
    return FALSE;
  }    
} /* cam3a_flash_needed */

/*===========================================================================

FUNCTION      CAMERA_AEC_GET_AEC_MODE

DESCRIPTION
  Get mode of the Auto Exposure Control

CALLERS:
  cam3a_get_aec_mode()

DEPENDENCIES

RETURN VALUE
  AEC mode in camera_auto_exposure_mode_type

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
camera_auto_exposure_mode_type camera_aec_get_aec_mode (void)
{
  return( cam3a_aec_state.aec_algo );
} /* cam3a_get_aec_mode */

/*===========================================================================

FUNCTION      CAMERA_AEC_IMPLEMENT

DESCRIPTION
   
CALLERS:
  cam3a_parse_and_implement_aec_awb ()

DEPENDENCIES
  camera_3a_shared_parse_regions should be called first

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e818 camsensorParamsPtr could be declared as pointing to const */
void camera_aec_implement (
  uint32             num_pixels_per_region_aec,
/* num pixels used for exposure stats in each region */
  camsensor_static_params_type * camsensorParamsPtr,
/* Parameters for camsensor layer        */
  int32               numRegions
  )
{
  /* As done on the 6100 DSP implementation, create a center window that
  consumes 25% of the total window area and weight this exposure as 60%
  of the total and the outlaying regions as the remaining 40% */
#ifdef FEATURE_CAMERA_3A_PROFILING
  int start_time, end_time, processing_time;
#endif /* FEATURE_CAMERA_3A_PROFILING */

  if(camsensorParamsPtr == NULL) return;
#ifdef FEATURE_CAMERA_3A_PROFILING
  start_time = timetick_get_ms ();
#endif /* FEATURE_CAMERA_3A_PROFILING */

  (void)camera_aec_adjust_exposure (num_pixels_per_region_aec, camsensorParamsPtr, numRegions);

  /* ----------------- Auto Frame Rate (AFR) -------------------------- */
  /* If the exposure index has changed, but before writing new exposure,
    table value, check to see if our current frame rate is appropriate */
  if( (cam3a_aec_state.afr_enable) && (preview_mode == CAMERA_PREVIEW_MODE_SNAPSHOT)
       #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
       #endif /* CAMERA_USES_CHROMATIX_0x203 */
    )  /* if enabled */
  /* & AFR mode = CAMERA_FRAME_RATE_AUTO) */
  {
    (void)camera_aec_auto_frame_rate( camsensorParamsPtr );

  } /* END if (camsensorParamsPtr->afr_enable)  */

#ifdef FEATURE_CAMERA_3A_PROFILING
  /*Calculate the processing Time */
  end_time = timetick_get_ms ();

  processing_time = end_time - start_time;
  if(processing_time > 0)
  {
    cam3a_aec_timing.total_processing_time += processing_time;
    cam3a_aec_timing.total_frames++;
  }
#endif /* FEATURE_CAMERA_3A_PROFILING */


} /* cam3a_parse_and_implement_aec_awb */
/*lint -restore */
/*===========================================================================

FUNCTION      CAMERA_AEC_IMPLEMENT_FOR_MULTISHOT

DESCRIPTION
   
CALLERS:
  cam3a_parse_and_implement_aec_awb ()

DEPENDENCIES
  camera_3a_shared_parse_regions should be called first

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
#if defined FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE && FEATURE_CAMERA_AEC_DURING_MULTISHOT*/
/*===========================================================================

FUNCTION      CAMERA_AEC_SET_BIAS_TABLE

DESCRIPTION
  Set bias table based on the AEC mode.

DEPENDENCIES

RETURN VALUE
  TRUE if operation completes successfully
  FALSE if not

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_aec_set_bias_table(
  const camsensor_static_params_type *camsensorParamsPtr 
  )
{
  if(camsensorParamsPtr == NULL) return FALSE;
#ifndef CAMERA_USES_SOFTDSP
  /* Choose an exposure table based on algorith selection above */
  if(vfeExpWbStatDspInput.numberOfRegions == VFE_8x8)
  {
    if(cam3a_aec_state.aec_algo == CAMERA_AEC_CENTER_WEIGHTED)
    {
      /* Now set pointer to bias weight table based on algo above */
      cam3a_aec_state.bias_table = aec_center_weighted_8x8;
    }
    else if(cam3a_aec_state.aec_algo == CAMERA_AEC_SPOT_METERING)
    {
      cam3a_aec_state.bias_table = aec_spot_metering_8x8;
    }
    else
    {
      cam3a_aec_state.bias_table = NULL;
    }
  }
#endif
#ifdef FEATURE_CAMERA_VFE_16X16_SUPPORT
  else if((VFE_NumberOfRegionsType)vfeExpWbStatDspInput.numberOfRegions == VFE_16x16)
  {
    if(cam3a_aec_state.aec_algo == CAMERA_AEC_CENTER_WEIGHTED)
    {
      /* Now set pointer to bias weight table based on algo above */
      cam3a_aec_state.bias_table = aec_center_weighted_16x16;
    }
    else if(cam3a_aec_state.aec_algo == CAMERA_AEC_SPOT_METERING)
    {
      cam3a_aec_state.bias_table = aec_spot_metering_16x16;
    }
    else
    {
      cam3a_aec_state.bias_table = NULL;
    }
  }
#endif /* FEATURE_CAMERA_VFE_16X16_SUPPORT */
#ifndef CAMERA_USES_SOFTDSP
  else
#endif
  {
    if(cam3a_aec_state.aec_algo == CAMERA_AEC_CENTER_WEIGHTED)
    {
      /* Now set pointer to bias weight table based on algo above */
      cam3a_aec_state.bias_table = aec_center_weighted_4x4;
    }
    else if(cam3a_aec_state.aec_algo == CAMERA_AEC_SPOT_METERING)
    {
      cam3a_aec_state.bias_table = aec_spot_metering_4x4;
    }
    else
    {
      cam3a_aec_state.bias_table = NULL;
    }
  }
  return TRUE;
} /* cam3a_set_bias_table */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_AEC_MODE

DESCRIPTION
  Set mode of the Auto Exposure Control

CALLERS:
  cam3a_set_aec_mode ()

DEPENDENCIES

RETURN VALUE
  TRUE if operation completes successfully
  FALSE if not

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e818 camsensorParamsPtr could be declared as pointing to const */
boolean camera_aec_set_aec_mode(
  camera_auto_exposure_mode_type aec_mode,
  const camsensor_static_params_type *camsensorParamsPtr 
/* Parameters for camsensor layer        */
  )
{
  if(camsensorParamsPtr == NULL) return FALSE;

  /* Check the aec method is supported */
  if(aec_mode >= CAMERA_AEC_MAX_MODES)
  {
    MSG_HIGH ("aec_mode = %d not allowed", aec_mode, 0, 0);
    return FALSE;
  }

  cam3a_aec_state.aec_algo = aec_mode;

  /* Choose an exposure table based on algorith selection above */
  return camera_aec_set_bias_table(camsensorParamsPtr);
} /* cam3a_set_aec_mode */
/*lint -restore */
/*===========================================================================

FUNCTION      CAMERA_AEC_SET_ANTIBANDING

DESCRIPTION
   Turn antibanding on (50 or 60hz) or off in AEC.

CALLERS:
  cam3a_set_antibanding ()

DEPENDENCIES
  None

INPUT
  None

GLOBALS
  cam3a_aec_state - Auto exposure varibles and settings

RETURN VALUE
  camera_ret_code_type:
  CAMERA_FAILED  - if this is a CCD sensor or if antibanding is disabled.
  CAMERA_SUCCESS - if everything worked

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e818 camsensorParamsPtr could be declared as pointing to const */
camera_ret_code_type camera_aec_set_antibanding (
  camera_antibanding_type state,
  const camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  )
{
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  if(camsensorParamsPtr == NULL) return CAMERA_FAILED;

  /* Check for valid param */
  if(state < CAMERA_MAX_ANTIBANDING)
  {
    if((state == CAMERA_ANTIBANDING_50HZ) || (state == CAMERA_ANTIBANDING_60HZ))
    {
      /* Error checks */
      if((cam3a_aec_state.antibanding_enable == FALSE) ||
        (camsensorParamsPtr->sensor_type == CAMSENSOR_CCD))
      {
        /* Invalid state for these operations */
        ret_code = CAMERA_FAILED;
      }
      else
      {
        cam3a_aec_state.antibanding = state;
        /* This forces AEC to update on the next frame */
        cam3a_aec_state.prev_exp_index = 0;

        if(state == CAMERA_ANTIBANDING_50HZ)
        {
          /* Set default exposure table to: antibanding off */
          cam3a_aec_state.num_exposure_table_values =
            cam3a_aec_state.num_50hz_exposure_table_values;
          cam3a_aec_state.exposure_table_ptr =
            cam3a_aec_state.exposure_table_50hz_ptr;
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
        }
        else
        {
          /* Set default exposure table to: antibanding off */
          cam3a_aec_state.num_exposure_table_values =
            cam3a_aec_state.num_60hz_exposure_table_values;
          cam3a_aec_state.exposure_table_ptr =
            cam3a_aec_state.exposure_table_60hz_ptr;
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
        }
      }
    } /* END if ((state == CAMERA_ANTIBANDING_50HZ) OR 60 */
    else
    {
      cam3a_aec_state.antibanding = CAMERA_ANTIBANDING_OFF;
      /* This forces AEC to update on the next frame */
      cam3a_aec_state.prev_exp_index = 0;

      /* Set default exposure table to: antibanding off */
      cam3a_aec_state.num_exposure_table_values =
        cam3a_aec_state.num_exposure_values_init;
      cam3a_aec_state.exposure_table_ptr =
        cam3a_aec_state.exposure_table_init;
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
    }
  }
  else
  {
    ret_code = CAMERA_FAILED;
  }


  return( ret_code );
} /* cam3a_set_antibanding */
/*lint -restore */
/*===========================================================================

FUNCTION      CAMERA_AEC_SET_BRIGHTNESS

DESCRIPTION
  Set brightness in AEC state and associated variables

CALLERS:
  camera_set_brightness ()

DEPENDENCIES
  None

INPUT

GLOBALS
  cam3a_aec_state - Auto exposure varibles and settings

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_set_brightness (uint8 luma_value)
{
  /* Make sure results of luma_value and luma_tolerance
   * addition and subtraction are within range. */
  if((luma_value + 8 * cam3a_aec_state.luma_tolerance) > 255)
  {
    luma_value = (uint8)(255 - 8 * cam3a_aec_state.luma_tolerance);
  }
  else if(luma_value < 8 * cam3a_aec_state.luma_tolerance)
  {
    luma_value = (uint8)(8 * cam3a_aec_state.luma_tolerance);
  }

  /* Also cap new luma target, high end only.
    Low end has already been caped */
  if(luma_value > CAM3A_MAX_LUMA_TARGET)
  {
    luma_value = CAM3A_MAX_LUMA_TARGET;
  }

  cam3a_aec_state.luma_tolerance = (uint8) ((luma_value * ((float)pow(10.0, (float)(1.0/cam3a_aec_state.exposure_index_adj_step))-1)) + 1);
  cam3a_aec_state.luma_target = luma_value;
  cam3a_aec_state.inner_max_luma_target  = 
    luma_value + cam3a_aec_state.luma_tolerance;
  cam3a_aec_state.outer_max_luma_target  = 
    luma_value + (uint8)(2 * cam3a_aec_state.luma_tolerance);
  cam3a_aec_state.outmost_max_luma_target = 
    luma_value + (uint8)(8 * cam3a_aec_state.luma_tolerance);
  cam3a_aec_state.inner_min_luma_target  = 
    luma_value - cam3a_aec_state.luma_tolerance;

  /* Make sure that we cap the low end */
  if((luma_value - (8 * cam3a_aec_state.luma_tolerance)) < 0)
  {
    cam3a_aec_state.outmost_min_luma_target = 0;
  }
  else
  {
    cam3a_aec_state.outmost_min_luma_target = 
      luma_value - (uint8)(8 * cam3a_aec_state.luma_tolerance);
  }

  if((luma_value - (2 * cam3a_aec_state.luma_tolerance)) < 0)
  {
    cam3a_aec_state.outer_min_luma_target = 0;
  }
  else
  {
    cam3a_aec_state.outer_min_luma_target = 
      luma_value - (uint8)(2 * cam3a_aec_state.luma_tolerance);
  }

} /* cam3a_set_brightness */

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_EXPOSURE_COMPENSATION

DESCRIPTION
  Set EV in AEC state and associated variables

CALLERS:
  cam3a_set_exposure_compensation ()

DEPENDENCIES
  None

INPUT

GLOBALS
  cam3a_aec_state - Auto exposure varibles and settings

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_set_exposure_compensation (uint8 luma_value)
{
  /* Make sure results of luma_value and luma_tolerance
   * addition and subtraction are within range. */
  if((luma_value + 8 * cam3a_aec_state.luma_tolerance) > 255)
  {
    luma_value = (uint8)(255 - 8 * cam3a_aec_state.luma_tolerance);
  }
  else if(luma_value < 2 * cam3a_aec_state.luma_tolerance)
  {
    luma_value = (uint8)(2 * cam3a_aec_state.luma_tolerance);
  }

  /* Also cap new luma target, high end only.
    Low end has already been caped */
  if(luma_value > CAM3A_MAX_LUMA_TARGET)
  {
    luma_value = CAM3A_MAX_LUMA_TARGET;
  }

  cam3a_aec_state.luma_tolerance = (uint8) ((luma_value * ((float)pow(10.0, (float)(1.0/cam3a_aec_state.exposure_index_adj_step))-1)) + 1);
  cam3a_aec_state.luma_target = luma_value;
  cam3a_aec_state.inner_max_luma_target  = 
    luma_value + cam3a_aec_state.luma_tolerance;
  cam3a_aec_state.outer_max_luma_target  = 
    luma_value + (uint8)(2 * cam3a_aec_state.luma_tolerance);
  cam3a_aec_state.outmost_max_luma_target = 
    luma_value + (uint8)(8 * cam3a_aec_state.luma_tolerance);
  cam3a_aec_state.inner_min_luma_target  = 
    luma_value - cam3a_aec_state.luma_tolerance;

  /* Make sure that we cap the low end */
  if((luma_value - (8 * cam3a_aec_state.luma_tolerance)) < 0)
  {
    cam3a_aec_state.outmost_min_luma_target = 0;
  }
  else
  {
    cam3a_aec_state.outmost_min_luma_target = 
      luma_value - (uint8)(8 * cam3a_aec_state.luma_tolerance);
  }

  if((luma_value - (2 * cam3a_aec_state.luma_tolerance)) < 0)
  {
    cam3a_aec_state.outer_min_luma_target = 0;
  }
  else
  {
    cam3a_aec_state.outer_min_luma_target = 
      luma_value - (uint8)(2 * cam3a_aec_state.luma_tolerance);
  }

} /* cam3a_set_exposure_compensation */


/*===========================================================================

FUNCTION      CAMERA_AEC_SET_ISO_MODE

DESCRIPTION
  Set mode of the ISO

CALLERS:
  cam3a_set_iso_mode ()

DEPENDENCIES

RETURN VALUE
  camera_ret_code_type

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type camera_aec_set_iso_mode(
  camera_iso_mode_type iso_mode
  )
{
  /*lint -save -e641 */
  if( (iso_mode < camera_parm_iso.minimum_value)||
    (iso_mode > camera_parm_iso.maximum_value) )
    return CAMERA_INVALID_PARM;

  camera_parm_iso.current_value = iso_mode;
  cam3a_aec_state.iso_mode = iso_mode;

#ifdef FEATURE_CAMERA_HJR
  if( camera_parm_iso.current_value == CAMERA_ISO_DEBLUR )
  {
    /* Set the one frame to 2 frame switching offset */
    cam3a_aec_state.one_to_two_frame_hjr_offset = (float)cam3a_aec_state.one_to_two_frame_hjr_offset_init;
  }
  else
  {
    cam3a_aec_state.one_to_two_frame_hjr_offset = 0.0;
  }
#endif /* FEATURE_CAMERA_HJR */
  return CAMERA_SUCCESS;
  /*lint -restore */
}

/*===========================================================================

FUNCTION      CAMERA_AEC_GET_MAX_ISO_MODE

DESCRIPTION
  Get mode of the ISO

CALLERS:
  cam3a_get_max_iso_mode()

DEPENDENCIES

RETURN VALUE
  ISO mode in camera_iso_mode_type

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
static camera_iso_mode_type camera_aec_get_max_iso_mode (void)
{
  camera_iso_mode_type max_iso_mode = CAMERA_ISO_AUTO;
  /* Now determine the max mode the sensor can support */
  float  max_gain = 1.0;
  uint16  iso_mode;
  uint16  multiplier, multiplied_gain;
  int32   exp_index;
  boolean found_match = FALSE;
  float   iso100_gain; 
	  

 iso100_gain = ((float)camctrl_tbl.iso100_gain) / Q8;

  if(camctrl_tbl.iso_is_supported == FALSE) return max_iso_mode;

 if(camctrl_tbl.aec_digital_gain_is_supported)
  {
    max_gain  = ((float)max_gain)/Q8;
  }
  else
  {
    max_gain  = camsensor_register_to_real_gain((uint16)max_gain);
  }

  

  exp_index = (int32) (cam3a_aec_state.num_exposure_table_values-1);
  max_gain  = cam3a_aec_state.exposure_table_ptr[exp_index].gain;

  /* Since are max mode is ISO_800 start with it */
  multiplier = 8; 
  iso_mode = (uint16) CAMERA_ISO_800;

  while(found_match == FALSE)
  {
    multiplied_gain = ( multiplier * iso100_gain );
    if(multiplied_gain <= max_gain)
    {
      max_iso_mode = (camera_iso_mode_type) iso_mode;
      found_match = TRUE;
    }
    else if((camera_iso_mode_type) iso_mode != CAMERA_ISO_100)
    {
      multiplier = multiplier/2;
      iso_mode--;
    }
    else
    {
      break;
    }
  }

  return max_iso_mode;
}

/*===========================================================================

FUNCTION      CAMERA_AEC_ADJUST_EXPOSURE_AS_PER_ISO

DESCRIPTION
              Adjust exposure on the camera sensor.  Implement a 
              generic algorithm that can work with any sensor

CALLERS:
  cam3a_adjust_exposure_as_per_iso ()

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type:
  CAMERA_FAILED  - if we were unable to update sensor
  CAMERA_SUCCESS - if everything worked

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type camera_aec_adjust_exposure_as_per_iso (void)
{

  camera_ret_code_type ret_val = CAMERA_SUCCESS;

  if(camctrl_tbl.iso_is_supported == TRUE)
  {
    aec_output_type_per_iso output;
    aec_forced_input_type forced_input;
    camsensor_snapshot_config_type snapshot_config;

    forced_input.apply_forced_input = FALSE;

#ifdef FEATURE_CAMERA_HJR
    if(hjr)
    {
      forced_input.apply_forced_input = TRUE;
      forced_input.forced_sensor_gain = HJR_structure.new_sensor_gain;
      forced_input.forced_line_count = HJR_structure.new_line_count;
    }
#endif /* FEATURE_CAMERA_HJR */

    output = camera_aec_core_run_adjust_exposure_algorithm_per_iso(&snapshot_config,forced_input);

    if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
    {
      cam3a_aec_state.digital_gain_for_snapshot = output.digital_gain_for_snapshot;
    }

    if(output.apply_shapshot_fps == TRUE)
    {
      ret_val = camctrl_tbl.set_snapshot_frame_rate(output.snapshot_fps);
      if(ret_val != CAMERA_SUCCESS)
      {
        MSG_ERROR("Failed to change the snapshot fps to :%d", output.snapshot_fps, 0, 0);
      }
    }

    if(ret_val == CAMERA_SUCCESS)
    {
      snapshot_config.gain = output.snapshot_gain;
      snapshot_config.linecount = output.snapshot_linecount;
      snapshot_config.digital_gain = output.digital_gain_for_snapshot;
      ret_val = camctrl_tbl.set_snapshot_exposure_gain ((void*)&snapshot_config);
    }

  }

  return ret_val;
}

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_NIGHT_SHOT_MODE

DESCRIPTION
  - Reduces sensor frame rate
  - Updates indoor / outdoor indices in the exposure table

CALLERS:
  CAMERA_SVCS_SET_PARAM

DEPENDENCIES
  camctrl_tbl.camsensor_params_ptr should be properly initialized.

RETURN VALUE
  camera_ret_code_type:
  CAMERA_INVALID_PARM - if param is not a valid
  CAMERA_FAILED  - if we were unable to update sensor
  CAMERA_SUCCESS - if everything worked

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type camera_aec_set_nightshot_mode 
  (
  camera_nightshot_mode_type parm
  )
{
  camera_ret_code_type ret_code = CAMERA_FAILED;
  uint8                index = 0;
  uint16               new_fps = 0;

  if(camctrl_tbl.camsensor_params_ptr == NULL)
  {
    return ret_code;
  }

  switch(parm)
  {
    case CAMERA_NIGHTSHOT_MODE_ON:
      /* Set rate back to 1/2 of maximum frame rate 
       * The support_nightshot_mode flag insures us that this is 
       * supported 
       */
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
      new_fps = camctrl_tbl.camsensor_params_ptr->nightshot_fps;
#endif /* CAMERA_USES_CHROMATIX_0x203 */

      cam3a_aec_state.afr_enable = FALSE;

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */

      break;

    case CAMERA_NIGHTSHOT_MODE_OFF:
      /* Set rate back to maximum frame rate */
      new_fps = camctrl_tbl.camsensor_params_ptr->max_preview_fps;
      cam3a_aec_state.afr_enable = TRUE;

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */

      break;

    default:  
      return CAMERA_INVALID_PARM;
  } 

  ret_code = camsensor_set_frame_rate (new_fps);

  if(ret_code == CAMERA_SUCCESS)
  {
    camctrl_tbl.camsensor_params_ptr->preview_fps = new_fps;
    /*lint -save -e641 */
    camera_parm_nightshot_mode.current_value = parm;
    /*lint -restore */
    /* Update indoor, outdoor indices */
    index = camera_aec_fps_index (new_fps);

    camera_aec_core_update_indoor_outdoor_indices (index);
  }
  else
  {
    MSG_ERROR ("Failed to set to FPS %d, Nightshot mode = %d ", \
      new_fps, (int) parm, 0);
    ret_code = CAMERA_NOT_SUPPORTED;
  }

  return ret_code;
} /* camera_set_nightshot_mode */

#ifdef FEATURE_CAMERA_3A_PROFILING
/*===========================================================================

FUNCTION      CAMERA_AEC_SHOW_TIMING

DESCRIPTION
   Displays the time taken by the aec algorithm for the current frame
   and the average time taken for all the frame processed previously.
      
CALLERS:
  cam3a_show_timing

DEPENDENCIES
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_show_timing (void)
{
  uint32 avg_processing_time;

  if(cam3a_aec_timing.total_frames == 0)
    return;

  avg_processing_time = cam3a_aec_timing.total_processing_time * 1000 /
    cam3a_aec_timing.total_frames;
  MSG_HIGH("AEC Average processing time per frame = %dus",
    avg_processing_time, 0, 0);
  cam3a_aec_timing.total_processing_time = 0;
  cam3a_aec_timing.total_frames = 0;
}
#endif /* FEATURE_CAMERA_3A_PROFILING */

#if 0
/*===========================================================================

FUNCTION      CAMERA_AEC_SET_NIGHT_SHOT_MODE_LEGACY

DESCRIPTION
  Legacy implementation of nightshot mode which used to be placed in camera-svcs
  Once the sensor drivers migrate to set_nightshot_mode, this code will be removed

CALLERS:
  CAMERA_AEC_SET_NIGHT_SHOT_MODE_INFO

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type:
  CAMERA_FAILED  - if we were unable to update sensor
  CAMERA_SUCCESS - if everything worked

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type camera_aec_set_nightshot_mode_legacy 
  (
  camera_nightshot_mode_type parm
  )
{
  camera_ret_code_type ret_code = CAMERA_FAILED;
  uint16               new_fps = 0;

  if(camctrl_tbl.camsensor_params_ptr == NULL)
  {
    return ret_code;
  }

  switch(parm)
  {
    case CAMERA_NIGHTSHOT_MODE_ON:
      /* Set rate back to 1/2 of maximum frame rate 
       * The support_nightshot_mode flag insures us that this is 
       * supported 
       */
      new_fps = camctrl_tbl.camsensor_params_ptr->nightshot_fps;
      break;

    case CAMERA_NIGHTSHOT_MODE_OFF:
      /* Set rate back to maximum frame rate */
      new_fps = camctrl_tbl.camsensor_params_ptr->max_preview_fps;
      break;

    default:  
      return ret_code;
  } 

  ret_code = camsensor_set_frame_rate (new_fps);

  if(ret_code == CAMERA_SUCCESS)
  {
    camctrl_tbl.camsensor_params_ptr->preview_fps = new_fps;
    camera_parm_nightshot_mode.current_value = parm;
  }
  else
  {
    MSG_ERROR ("Failed to set to FPS %d, Nightshot mode = %d ", \
      new_fps, (int) parm, 0);
    ret_code = CAMERA_NOT_SUPPORTED;
  }

  return ret_code;
}
#endif

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_PARM

DESCRIPTION
  set camera_parm_info in aec

CALLERS:
  CAMERA_SVCS_SET_PARAM

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_aec_set_parm
  (
  camera_parm_type  id,
  int32             parm
  )

{
  camera_ret_code_type ret_val;

  switch(id)
  {
    case CAMERA_PARM_NIGHTSHOT_MODE:
      if(camctrl_tbl.nightshot_is_supported == FALSE)
        return CAMERA_NOT_SUPPORTED;

      ret_val = camera_aec_set_nightshot_mode((camera_nightshot_mode_type)parm);
      break;

    case CAMERA_PARM_ISO:
      if(camctrl_tbl.iso_is_supported == FALSE)
        return CAMERA_NOT_SUPPORTED;

      ret_val = camera_aec_set_iso_mode ( (camera_iso_mode_type) parm );
      break;


    case CAMERA_PARM_FPS:
      ret_val = camsensor_set_frame_rate(parm);
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
      break;


    default:
      ret_val = CAMERA_INVALID_PARM;
      break;
  }  

  return ret_val;
}

/*===========================================================================

FUNCTION      CAMERA_AEC_GET_PARM

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
camera_ret_code_type camera_aec_get_parm
  (
  camera_parm_type       id,
  camera_parm_info_type *parm
  )
{
  camera_ret_code_type ret_val;

  switch(id)
  {
    case CAMERA_PARM_NIGHTSHOT_MODE:
      if(camctrl_tbl.nightshot_is_supported == FALSE)
        return CAMERA_NOT_SUPPORTED;

      *parm = camera_parm_nightshot_mode;
      ret_val = CAMERA_SUCCESS;
      break;

    case CAMERA_PARM_ISO:
      if(camctrl_tbl.iso_is_supported == FALSE)
        return CAMERA_NOT_SUPPORTED;

      *parm = camera_parm_iso;
      ret_val = CAMERA_SUCCESS;
      break;

    default:
      ret_val = CAMERA_INVALID_PARM;
      break;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION      CAMERA_AEC_ADJUST_SNAPSHOT_EXPOSURE_FOR_SNAPSHOT

DESCRIPTION
  adjust exposure for snapshot using iso
  if iso is not supported, it just return CAMERA_SUCCESS without doing anything.
  Only Debug info will be notified.

CALLERS:
  camera_config_vfe()

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type:

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_aec_adjust_exposure_for_snapshot (void)
{
  camera_ret_code_type ret_val;

  /* Force a single report when luma_settled_cnt goes non-zero,
   * implying VFE is now stable. */
  camera_aec_report_vfe_stable = TRUE;

  if(camctrl_tbl.iso_is_supported == FALSE)
  {
    MSG_CAM3ADEBUG("Don't do anything. ISO is not supported", 0, 0, 0);
    return CAMERA_SUCCESS;
  }

  ret_val = camera_aec_adjust_exposure_as_per_iso();
  return ret_val;
}

#ifdef FEATURE_CAMERA_HJR
/*===========================================================================

FUNCTION: CAMERA_AEC_HJR_ADJUST_FOR_MAX_GAIN

DESCRIPTION: 
Makes sure we adjust down the line count if we are over the max gain

DEPENDENCIES:
called by CAMERA_AEC_PREPARE_FOR_HJR

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/
void camera_aec_hjr_adjust_for_max_gain(float gain)
{
  while( gain > 2 ) /* two is the max because any more creates too much quantization error */
  {
    gain /= 2;
    HJR_structure.new_line_count *=2;
  }
  HJR_structure.new_dig_gain = vfe_convert_float_to_Qfactor(gain , 7);
}

/*===========================================================================

FUNCTION: CAMERA_AEC_PREPARE_FOR_HJR

DESCRIPTION: 
  PREPARES THE INPUT DATA AND RUNS THE HAND JITTER CONTROL ALGO TO DECIDE
  THE EXPOSURE TIME, GAIN, FRAME RATE, NUMBER OF FRAMES TO CAPTURE.

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/

void camera_aec_prepare_for_hjr(const camsensor_static_params_type *camsensorParamsPtr)
{
  int32 max_exp_index;
  uint16 max_register_gain;
  float gain_temp;

  if(camsensorParamsPtr == NULL)
    return;

  HJR_structure.maxNumFrames = camctrl_tbl.hjr_max_frames;

  if(HJR_structure.maxNumFrames > HJR_MAX_FRAMES_SUPPORTED)
  {
    HJR_structure.maxNumFrames = HJR_MAX_FRAMES_SUPPORTED;
  }
  else if( HJR_structure.maxNumFrames == 0 )
  {
    HJR_structure.maxNumFrames = 1;
  }

  HJR_structure.current_luma =  cam3a_aec_state.awb_compensated_luma;
  HJR_structure.luma_target = cam3a_aec_state.luma_target;

  HJR_structure.current_line_count = cam3a_aec_state.current_line_count;
  HJR_structure.current_sensor_gain = cam3a_aec_state.current_real_gain;

  max_exp_index = (int32)cam3a_aec_state.num_exposure_table_values;
  max_register_gain = cam3a_aec_state.exposure_table_ptr[max_exp_index-1].gain;


  HJR_structure.max_sensor_gain = 
    camsensor_register_to_real_gain(max_register_gain);
  HJR_structure.max_line_count = 
    cam3a_aec_state.exposure_table_ptr[max_exp_index-1].line_count;
 
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  {
  HJR_structure.max_frame_rate = cam3a_aec_state.afr_max_fps;
  HJR_structure.current_frame_rate = cam3a_aec_state.afr_frame_rate;
  }

  /* HJR control algorithm to decide on the new set up gain/exposure values */

  /* Set the values to default values first */
  HJR_structure.new_line_count = HJR_structure.current_line_count;
  HJR_structure.new_sensor_gain = HJR_structure.current_sensor_gain;
  HJR_structure.new_dig_gain = 128; /* Q7 */
  HJR_structure.new_numberFrames = 1;
  HJR_structure.new_frame_rate = HJR_structure.current_frame_rate;

  if ( HJR_structure.current_line_count < HJR_LINE_COUNT_MIN_LIMIT * HJR_structure.gain_factor)
  {
    return;
  }

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  {
  /* Meeting Luma target with a tolerence or not */
  if(HJR_structure.current_frame_rate == HJR_structure.max_frame_rate)
  {
    /* Halve the line count */
    /* Double the analog gain
     * If not possible use max gain and compensate 
     * with digital gain */
    HJR_structure.new_line_count = HJR_structure.current_line_count/HJR_structure.gain_factor;
    /* check to see if the sensor gain is now below min */
    HJR_structure.new_sensor_gain = 
    HJR_MIN((HJR_structure.current_sensor_gain*HJR_structure.gain_factor), 
    HJR_structure.max_sensor_gain);
    if(HJR_structure.new_sensor_gain < 
        (HJR_structure.current_sensor_gain*HJR_structure.gain_factor) )
    {
      gain_temp = 
        (float) (HJR_structure.current_sensor_gain*HJR_structure.gain_factor)
      /(float) (HJR_structure.new_sensor_gain);
      camera_aec_hjr_adjust_for_max_gain(gain_temp);
    }

    if(cam3a_aec_state.antibanding != CAMERA_ANTIBANDING_OFF)
    {
      /* Adjust the exposure to support Anti banding */
      /* Calculate the band gap */
      uint32  line_count_gap, line_count_compensation = 0;
      float   gain_compensation;

      if((cam3a_aec_state.antibanding == CAMERA_ANTIBANDING_50HZ)
        && (HJR_structure.current_line_count >= cam3a_aec_state.band_50hz_gap) )
      {
        /* Need to adjust the exposure in order not to break anti banding */
        line_count_gap = (HJR_structure.new_line_count % (uint32) cam3a_aec_state.band_50hz_gap);
        if(line_count_gap != 0)
        {
          /* Adjust the exposure */
          line_count_compensation = (unsigned long)(cam3a_aec_state.band_50hz_gap - line_count_gap);
        }
      }
      else if((cam3a_aec_state.antibanding == CAMERA_ANTIBANDING_60HZ)
        && (HJR_structure.current_line_count >= cam3a_aec_state.band_60hz_gap) )
      {
        /* Need to adjust the exposure in order not to break anti banding */
        line_count_gap = (HJR_structure.new_line_count % (uint32) cam3a_aec_state.band_60hz_gap);
        if(line_count_gap != 0)
        {
          /* Adjust the exposure */
          line_count_compensation = (unsigned long)(cam3a_aec_state.band_60hz_gap - line_count_gap);
        }
      }
      gain_compensation = (float)((float)(HJR_structure.new_line_count)/(float)(HJR_structure.new_line_count+line_count_compensation));
      HJR_structure.new_line_count += line_count_compensation;
      HJR_structure.new_sensor_gain *= gain_compensation;
    }

  }
  else
  {
    /*Running at Lower frame rate */
    /* Double the frame rate */

    /* Meeting Luma target with a tolerence or not */
    if(HJR_structure.current_luma >= HJR_LUMA_THRESHOLD(HJR_structure.luma_target))
    {
      HJR_structure.new_frame_rate = HJR_structure.current_frame_rate * 2;
      HJR_structure.new_line_count = (HJR_structure.current_line_count*2)/HJR_structure.gain_factor;
      HJR_structure.new_sensor_gain = 
        HJR_MIN((HJR_structure.current_sensor_gain*HJR_structure.gain_factor), 
        HJR_structure.max_sensor_gain);
      if(HJR_structure.new_sensor_gain < 
         (HJR_structure.current_sensor_gain*HJR_structure.gain_factor) )
      {
        gain_temp = 
          (float) (HJR_structure.current_sensor_gain*HJR_structure.gain_factor)
        /
          (float) (HJR_structure.new_sensor_gain);
        camera_aec_hjr_adjust_for_max_gain(gain_temp);
      }
      HJR_structure.new_numberFrames = 2;
    }
    else
    {
      HJR_structure.new_numberFrames = 3;
      HJR_structure.new_frame_rate = HJR_structure.current_frame_rate * 2;
      HJR_structure.new_dig_gain = vfe_convert_float_to_Qfactor(2, 7);
    }
  }

  if(HJR_structure.current_frame_rate != HJR_structure.new_frame_rate)
  {
    /*Change the frame rate */
    (void)camsensor_set_frame_rate((uint16)HJR_structure.new_frame_rate);
  }

 }
  MSG_LOW("HJR: No of Frames:%d",HJR_structure.new_numberFrames, 0, 0);

}

/*===========================================================================

FUNCTION: CAMERA_AEC_ADJUST_EXPOSURE_AS_PER_HJR

DESCRIPTION: 
  Adjusts the exposure and frame rate prior to taking snapshot

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/
void camera_aec_adjust_exposure_as_per_hjr(void)
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;

  /* Force a single report when luma_settled_cnt goes non-zero,
   * implying VFE is now stable. */
  camera_aec_report_vfe_stable = TRUE;

  if(camctrl_tbl.iso_is_supported == FALSE)
  {
    MSG_CAM3ADEBUG("ISO is not supported. Exposure adjustment is failed", 0, 0, 0);
  }

  hjr = TRUE;    
  ret_val = camera_aec_adjust_exposure_as_per_iso();
  if(ret_val != CAMERA_SUCCESS)
  {
    MSG_CAM3ADEBUG("Exposure adjustment is failed", 0, 0, 0);
  }
  hjr = FALSE;
}
#endif /* FEATURE_CAMERA_HJR */
#ifndef CAMERA_USES_SOFTDSP
/*===========================================================================

FUNCTION: CAMERA_AEC_APPLY_DIGITAL_GAIN_FOR_PREVIEW

DESCRIPTION: 
  apply digital gain for preview

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/

void camera_aec_apply_digital_gain_for_preview(VFE_ColorCorrectionType *packed_cc_ptr)
{
  float temp_gain;

  if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
  {
    if(
      (cam3a_awb_state.curr_weighted_wb.r_gain != cam3a_awb_state.prev_weighted_wb.r_gain) ||
      (cam3a_awb_state.curr_weighted_wb.g_gain != cam3a_awb_state.prev_weighted_wb.g_gain) ||
      (cam3a_awb_state.curr_weighted_wb.b_gain != cam3a_awb_state.prev_weighted_wb.b_gain)
      )
    {
      temp_gain = cam3a_aec_state.digital_gain;
    }
    else if((cam3a_aec_state.digital_gain - cam3a_aec_state.prev_digital_gain) != 0)
    {
      temp_gain = (cam3a_aec_state.digital_gain / cam3a_aec_state.prev_digital_gain);
    }
    else
    {
      temp_gain = 1.0;
    }

    vfe_apply_digital_gain_per_aec(packed_cc_ptr, temp_gain);

    cam3a_aec_state.prev_digital_gain = cam3a_aec_state.digital_gain;
  }
}

/*===========================================================================

FUNCTION: CAMERA_AEC_APPLY_DIGITAL_GAIN_FOR_ISO_SNAPSHOT

DESCRIPTION: 
  apply digital gain for snapshot with ISO

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/

void camera_aec_apply_digital_gain_for_iso_snapshot(VFE_ColorCorrectionType *packed_cc_ptr)
{
  float temp_gain;

  if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
  {
    /* digital gain for snapshot is only available with ISO */
    if(camctrl_tbl.iso_is_supported == TRUE)
    {
      temp_gain = (cam3a_aec_state.digital_gain_for_snapshot/cam3a_aec_state.digital_gain);

      vfe_apply_digital_gain_per_aec(packed_cc_ptr, temp_gain);
    }
  }
}
#endif
/*===========================================================================

FUNCTION: CAMERA_AEC_UPDATE_COLOR_CORRECTION_MATRIX

DESCRIPTION: 
  update color correction matrix with normal and low-light color correction matrice

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/
/*lint -save -e818 -e734*/
void camera_aec_update_color_correction_matrix(
  camera_3x3_rgb_matrix_type * sourceCC, 
  camera_3x3_rgb_matrix_type * low_light_CC_ptr)
{
  float low_light_CCweight;
  float one_minus_low_light_CCweight;

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
  {
    /*lint -save -e653 */
    low_light_CCweight = 
      ((float)(cam3a_aec_state.exp_index - ((cam3a_aec_state.indoor_index + (cam3a_aec_state.num_exposure_table_values - 1))/2)) /
      (int32)((cam3a_aec_state.num_exposure_table_values - 1) - cam3a_aec_state.indoor_index));
    /*lint -restore */
  }

  if( (low_light_CCweight < 0) || 
    (!cam3a_awb_state.low_light_CC_enable) )
  {
    low_light_CCweight = 0;
  }

  one_minus_low_light_CCweight = 1.0 - low_light_CCweight;

  low_light_CC_ptr->A11 = 
    (int32)((one_minus_low_light_CCweight * sourceCC->A11) + 
    (low_light_CCweight * low_light_CC_ptr->A11));
  low_light_CC_ptr->A12 = 
    (int32)((one_minus_low_light_CCweight * sourceCC->A12) + 
    (low_light_CCweight * low_light_CC_ptr->A12));
  low_light_CC_ptr->A13 = 
    (int32)((one_minus_low_light_CCweight * sourceCC->A13) + 
    (low_light_CCweight * low_light_CC_ptr->A13));

  low_light_CC_ptr->A21 = 
    (int32)((one_minus_low_light_CCweight * sourceCC->A21) + 
    (low_light_CCweight * low_light_CC_ptr->A21));
  low_light_CC_ptr->A22 = 
    (int32)((one_minus_low_light_CCweight * sourceCC->A22) + 
    (low_light_CCweight * low_light_CC_ptr->A22));
  low_light_CC_ptr->A23 = 
    (int32)((one_minus_low_light_CCweight * sourceCC->A23) + 
    (low_light_CCweight * low_light_CC_ptr->A23));

  low_light_CC_ptr->A31 = 
    (int32)((one_minus_low_light_CCweight * sourceCC->A31) + 
    (low_light_CCweight * low_light_CC_ptr->A31));
  low_light_CC_ptr->A32 = 
    (int32)((one_minus_low_light_CCweight * sourceCC->A32) + 
    (low_light_CCweight * low_light_CC_ptr->A32));
  low_light_CC_ptr->A33 = 
    (int32)((one_minus_low_light_CCweight * sourceCC->A33) + 
    (low_light_CCweight * low_light_CC_ptr->A33));

  low_light_CC_ptr->bias_r = 
    (int32)((one_minus_low_light_CCweight * sourceCC->bias_r) + 
    (low_light_CCweight * low_light_CC_ptr->bias_r));
  low_light_CC_ptr->bias_g = 
    (int32)((one_minus_low_light_CCweight * sourceCC->bias_g) + 
    (low_light_CCweight * low_light_CC_ptr->bias_g));
  low_light_CC_ptr->bias_b = 
    (int32)((one_minus_low_light_CCweight * sourceCC->bias_b) + 
    (low_light_CCweight * low_light_CC_ptr->bias_b));
}
/*lint -restore */
/*===========================================================================

FUNCTION: CAMERA_AEC_RETRIVE_EXPOSURE_INFO

DESCRIPTION: 
  retrive aec exposure info

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/
void camera_aec_retrive_exposure_info(
  boolean aec_enable,
  camera_exposure_info_request_type request_type,
  camera_exposure_info_type * exposure_info_ptr)
{
  if(aec_enable)
  {
    switch(request_type)
    {
      case EXPOSURE_PER_AWB_ALGO:
        exposure_info_ptr->indoor_index = cam3a_aec_state.indoor_index;
        exposure_info_ptr->outdoor_index = cam3a_aec_state.outdoor_index;
        exposure_info_ptr->current_exposure_index = (uint16)cam3a_aec_state.prev_exp_index;
        exposure_info_ptr->max_exp_index = cam3a_aec_state.num_exposure_table_values-1;
        exposure_info_ptr->indoor_midpoint= (cam3a_aec_state.indoor_index + exposure_info_ptr->max_exp_index + 1) >> 1;
        exposure_info_ptr->inoutdoor_midpoint = (cam3a_aec_state.indoor_index + cam3a_aec_state.outdoor_index + 1) >> 1;
        exposure_info_ptr->outdoor_midpoint = (cam3a_aec_state.outdoor_index+1)>> 1;
        break;

      case EXPOSURE_PER_GAMMA:
        exposure_info_ptr->current_exposure_index = (uint16)cam3a_aec_state.prev_exp_index; 
        exposure_info_ptr->outdoor_midpoint 
          = cam3a_aec_state.outdoor_index - ((cam3a_aec_state.indoor_index - cam3a_aec_state.outdoor_index)/4);
        exposure_info_ptr->outdoor_index = cam3a_aec_state.outdoor_index; 
        exposure_info_ptr->inoutdoor_midpoint 
          = cam3a_aec_state.outdoor_index + ((cam3a_aec_state.indoor_index - cam3a_aec_state.outdoor_index)/4);
        exposure_info_ptr->indoor_index = cam3a_aec_state.indoor_index; 
        exposure_info_ptr->indoor_midpoint
          = (cam3a_aec_state.num_exposure_table_values - 1) - (((cam3a_aec_state.num_exposure_table_values - 1)-cam3a_aec_state.indoor_index)/4);
        exposure_info_ptr->max_exp_index = cam3a_aec_state.num_exposure_table_values-1;   
        break;

      default:
        MSG_ERROR ("Invalid request type",0,0,0);
        return;
    }
  }
  else /* in case aec_enable == FALSE */
  {
    exposure_info_ptr->indoor_index = 0;
    exposure_info_ptr->outdoor_index = 0;
    exposure_info_ptr->current_exposure_index = 0;
    exposure_info_ptr->max_exp_index = cam3a_aec_state.num_exposure_table_values-1;
    exposure_info_ptr->indoor_midpoint= (exposure_info_ptr->max_exp_index + 1) >> 1;
    exposure_info_ptr->inoutdoor_midpoint = 0;
    exposure_info_ptr->outdoor_midpoint = 0;
  }
}

/*===========================================================================

FUNCTION      CAMERA_AEC_RESTORE_CONFIG_FOR_PREVIEW

DESCRIPTION
  restore aec gain and line_count for preview restart
   
CALLERS:
  camera_restore_config_for_preview()

===========================================================================*/
void camera_aec_restore_config_for_preview(void)
{
  uint16 gain;
  float temp_gain = 1.0;
  uint32 line_count;

   if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
   {
     /* Q8 real gain is in exposure table */
     temp_gain = (float)cam3a_aec_state.exposure_table_ptr[cam3a_aec_state.exp_index].gain / 256.0;
     gain = camsensor_real_to_register_gain(temp_gain);
    
   }
   else
   {
     gain = cam3a_aec_state.exposure_table_ptr[cam3a_aec_state.exp_index].gain;
    
   }
  
    line_count = 
    cam3a_aec_state.exposure_table_ptr[cam3a_aec_state.exp_index].line_count;

  (void)camsensor_write_exposure_gain(gain, line_count);
}
/*===========================================================================

FUNCTION      CAMERA_AEC_SETTLED

DESCRIPTION
   Returns TRUE if aec has settled +/- 20% of luma target or if we have reached
   maximum exposure index.
   
CALLERS:

DEPENDENCIES
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_aec_settled(cam3a_aec_state_struct *aec_state)
{
  if(aec_state->current_luma >= (0.8f*aec_state->luma_target) && 
     aec_state->current_luma <= (1.2f*aec_state->luma_target))
  {
    return TRUE;
  }
  else if((aec_state->exp_index >= aec_state->num_exposure_table_values - 1)&& 
          aec_state->afr_frame_rate <= aec_state->afr_min_fps)
  {
    return TRUE;
  }
  else
    return FALSE;
} /* camera_aec_settled */



#endif /* FEATURE_CAMERA_YCBCR_ONLY */
#endif //#ifndef CAMERA_USES_SOFTDSP
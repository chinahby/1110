/*=============================================================================
FILE:       camera_awb_core.c

SERVICES:   Core Auto White Balance File

DESCRIPTION:

        Copyright © 2004 - 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary


$Header: //source/qcom/qct/multimedia/camera/6k/3a/awb/main/latest/src/camera_awb_core.c#5 $ $DateTime: 2008/11/14 10:16:02 $ $Author: cgera $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 11/03/08   cg       Updates for AWB 1.4
 9/9/08     rs       Added bounds to array accesses CR153464
 11/08/07   hap      Fixing RVCT2.2 compiler warnings
 11/10/07   kdiv     Added condition check to avoid Overrun Vulnerability in 
                     camera_awb_core_init_agw_algorithm()
 08/16/07   klee     Applied actual outdoor daylight line for green heuristics in all-outlier case
 06/06/07   klee     Enhanced AWB 1.3 by updating adjust_rg_bg_by_history
 04/30/07   zl       Added sanity check in camera_awb_core_search_nearest_reference_point().
 04/05/07   klee     Modified awb low-light detection logic - not using FPS
 03/30/07   klee     Clean-up lint errors due to un-matched data type in 3a
 03/28/07   klee     Modified the name awb_input_exposure_type to be camera_exposure_info_type
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 03/06/07   klee     Fixed the AWB bug under outdoor light condition
 02/20/07   klee     Fixed all lint errors on AWB
 02/09/07   klee     Fixed critical lint errors
 01/26/07   klee     Updated minor typo in AWB core function
 01/24/07   klee     Enahnced AWB algorithm with white_has_single_peak
 01/17/07   klee     Added AWB history computation for A and Daylight
 01/09/07   klee     Enhanced AWB to provide stable preview by history
 01/04/07   klee     Updated camera_awb_core_adjust_rg_bg_by_history() to remove redundant computation
 01/02/07   klee     Enhanced AWB to provide stable preview by history
 12/22/06   klee     Update AWB to provide stable preview by history
 12/21/06   klee     Updated AWB to apply gain_adj only for the snapshot, not for preview
 12/21/06   klee     Enhanced AWB algorithm under low-light condition
 12/21/06   klee     Removed the code under CAMERA_AWB_FLOAT_ON
 12/01/06   klee     Fix on CR 107323 - AWB gain adjustment under daylight condition is not proper
 11/12/06   klee     Enhance AWB core algorithm by using region_cnt
 11/03/06   klee     Beautified the code by indenting and removing special characters
 10/20/06   jv       Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 10/18/06   klee     Fixed stack overflow issue with AWB core algorithm
 09/15/06   klee     Cleaned up the lint errors
 09/11/06   klee     Enhanced algorithm with two more tuning variables for green line
 09/08/06   klee     Updated min/max macro to be CAM3A_MIN/CAM3A_MAX
 09/01/06   kadhir   Removed unwanted Cross Compilation Dependency
                     When qcamcorder Feature Disabled
 08/31/06   klee     Fixed the bug on fixed point AWB algorithm - possible_green_cnt
 08/23/06   klee     Updated core algorithm with possible_indoor_green_cnt
 08/18/06   klee     Fixed critical and high lint errors
 08/17/06   klee     Updated core algorithm with possible_green_cnt
 08/15/06   klee     Updated core algorithm with decision making
 08/10/06   klee     Cleaned up the code regarding tuning variables relocation
                     - AWB/AEC
                     - Color matrix
 08/01/06   klee     AWB with white world algorithm
 07/25/06   klee     Updated frame drop condition
 06/20/06   klee     Initial code

==============================================================================*/

/*==============================================================================
  INCLUDES
==============================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#ifndef FEATURE_CAMERA_YCBCR_ONLY
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "camera_awb_core.h"
#include "camera_aec.h"
#include "camera_awb.h"

/*==============================================================================
  INTERNAL VARIABLES
==============================================================================*/
cam3a_advanced_grey_world_type * cam3a_agw_ptr = NULL;
int outlier_rg_grid[256], outlier_bg_grid[256];

/*==============================================================================
  INTERNAL API DECLARATIONS
==============================================================================*/
/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_CONVERT_TO_GRID

DESCRIPTION
  using float number

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
static int camera_awb_core_convert_to_grid(int rg_ratio_x, int bg_ratio_x,
  int *rg_grid, int *bg_grid);

/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_SEARCH_NEAREST_REFERENCE_POINT

DESCRIPTION
  using float number

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
static int camera_awb_core_search_nearest_reference_point(int rg_grid, int bg_grid,
  cam3a_auto_white_balance_illuminant_type *sample_decision,
  int32 *min_dist, int *day_idx);

/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_UPDATE_HISTORY

DESCRIPTION
  update awb history for stable preview

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
static void camera_awb_core_update_history(float rg_ratio, float bg_ratio, cam3a_auto_white_balance_illuminant_type decision,
  uint16 exp_index, int8 is_white);

/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_ADJUST_RG_BG_BY_HISTORY

DESCRIPTION
  adjust awb by history for stable preview

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
static void camera_awb_core_adjust_rg_bg_by_history(float *rg_ratio, float *bg_ratio,
  cam3a_auto_white_balance_illuminant_type *decision,
  camera_exposure_info_type* input_exposure_ptr);



//===============================================================
static int camera_awb_core_is_custom_flo_close_to_daylight(void);
static int camera_awb_core_smallest_exp_index_in_history(void);
static int camera_awb_core_is_there_daylight_in_history(void);
static int camera_awb_core_get_daylight_from_history(float *rg_ratio, float *bg_ratio);

int camera_awb_single_color_tracking_all_outliers(camera_exposure_info_type* input_exposure_ptr,
						  int num_outliers,
						  int *outlier_rg_grid,
						  int *outlier_bg_grid,
						  int outlier_ave_rg,
						  int outlier_ave_bg,	
						  int max_dist);

//================================================================


/*==============================================================================
  INTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_CONVERT_TO_GRID

DESCRIPTION
  using float number

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
static int camera_awb_core_convert_to_grid(int rg_ratio_x, int bg_ratio_x,
  int *rg_grid, int *bg_grid)
{
  int index2;

  for(index2=0; index2 < CAM3A_AGW_AWB_NUMBER_GRID_POINT-1; index2++)
  {  // find the R/G grid index
    if(rg_ratio_x<cam3a_agw_ptr->rgbg_grid_x[0])
    {
      *rg_grid=0;
      break;
    }
    if(rg_ratio_x>=cam3a_agw_ptr->rgbg_grid_x[index2] &&
      rg_ratio_x<cam3a_agw_ptr->rgbg_grid_x[index2+1])
    {
      *rg_grid=index2;
      break;
    }
  }
  if(index2==CAM3A_AGW_AWB_NUMBER_GRID_POINT)
    *rg_grid=CAM3A_AGW_AWB_NUMBER_GRID_POINT-1;

  for(index2=0; index2<CAM3A_AGW_AWB_NUMBER_GRID_POINT-1; index2++)
  {  // find the B/G grid index
    if(bg_ratio_x<cam3a_agw_ptr->rgbg_grid_x[0])
    {
      *bg_grid=0;
      break;
    }
    if(bg_ratio_x>=cam3a_agw_ptr->rgbg_grid_x[index2] &&
      bg_ratio_x<cam3a_agw_ptr->rgbg_grid_x[index2+1])
    {
      *bg_grid=index2;
      break;
    }
  }
  if(index2==CAM3A_AGW_AWB_NUMBER_GRID_POINT)
    *bg_grid=CAM3A_AGW_AWB_NUMBER_GRID_POINT-1;

  return 0;
}

/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_SEARCH_NEAREST_REFERENCE_POINT

DESCRIPTION
  using float number

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
static int camera_awb_core_search_nearest_reference_point(int rg_grid, int bg_grid,
  cam3a_auto_white_balance_illuminant_type *sample_decision,
  int32 *min_dist, int *day_idx)
{
  int index;
  int32 d1,d2,dist;

  *min_dist=0x3fffffff;

/* Search all lighting conditions */
  for(index=0;index<CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT;index++)
  {  // fixed reference points
    d1=(rg_grid-cam3a_agw_ptr->rg_grid[index]);
    d2=(bg_grid-cam3a_agw_ptr->bg_grid[index]);
    dist=d1*d1+d2*d2;

    if(dist<*min_dist)
    {
      *sample_decision=(cam3a_auto_white_balance_illuminant_type)index;   // set lighting condition
      *min_dist=dist;      // update the new minimum distance
    }
    else if(dist==*min_dist)
    {   // give fluorescent more preference over A & H
      if((index==(int)CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
        index==(int)CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
        index==(int)CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO ) &&
        (*sample_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT || 
        *sample_decision==CAM3A_AGW_AWB_HORIZON))
      {
        *sample_decision=(cam3a_auto_white_balance_illuminant_type)index;
      }
    }
  }//end for

  for(index=1;index<cam3a_agw_ptr->n_day1-1 && index<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
  {   // examine the daylight line 1
    d1=(rg_grid-cam3a_agw_ptr->day_line_1[index][0]);
    d2=(bg_grid-cam3a_agw_ptr->day_line_1[index][1]);
    dist=d1*d1+d2*d2;

    if(dist<*min_dist)
    {
      *min_dist=dist;   // update the new minimum distance
      *sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
      /* no normalization, need to set rg_adj and bg_adj to be 1 */
      *day_idx=1;
    }
  }

  for(index=1;index<cam3a_agw_ptr->n_day2-1 && index<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
  {   // examine the daylight line 2
    d1=(rg_grid-cam3a_agw_ptr->day_line_2[index][0]);
    d2=(bg_grid-cam3a_agw_ptr->day_line_2[index][1]);
    dist=d1*d1+d2*d2;

    if(dist<*min_dist)
    {
      *min_dist=dist;   // update the new minimum distance
      *sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
      *day_idx=2;
    }//end if
  }//end for

  //*****************************************
  // experimental code, added for day light line 3
  for(index=0;index<cam3a_agw_ptr->n_day3 && index<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
  {   // examine the daylight line 3
    d1=(rg_grid-cam3a_agw_ptr->day3_rg[index]);
    d2=(bg_grid-cam3a_agw_ptr->day3_bg[index]);
    dist=d1*d1+d2*d2;

    if(dist<*min_dist)
    {
      *min_dist=dist;   // update the new minimum distance
      *sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
      *day_idx=2;   // does not assign it to line 3 because it's not accounted for in awb algorithm
    }//end if
  }//end for
  //************************************************

  for(index=1;index<cam3a_agw_ptr->n_fline-1 && index<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
  {   // examine the F line 
    d1=(rg_grid-cam3a_agw_ptr->Fline[index][0]);
    d2=(bg_grid-cam3a_agw_ptr->Fline[index][1]);
    dist=d1*d1+d2*d2;

    if(dist<*min_dist)
    {
      *min_dist=dist;   // update the new minimum distance
      *sample_decision=CAM3A_AGW_AWB_INDOOR_WARM_FLO;
    }//end if
    else if(dist==*min_dist)
    {   // give fluorescent more preference over A & H
      if(*sample_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT || 
        *sample_decision==CAM3A_AGW_AWB_HORIZON)
      {
        *sample_decision=CAM3A_AGW_AWB_INDOOR_WARM_FLO;
      }
    }
  }//end for

  for(index=1;index<cam3a_agw_ptr->n_day_f_line-1 && index<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
  {   // examine the Day_F line , 3A 1.4 changes
    d1=(rg_grid-cam3a_agw_ptr->Day_F_line[index][0]);
    d2=(bg_grid-cam3a_agw_ptr->Day_F_line[index][1]);
    dist=d1*d1+d2*d2;

    if(dist<*min_dist)
    {
      *min_dist=dist;   // update the new minimum distance
      *sample_decision=CAM3A_AGW_AWB_INDOOR_WARM_FLO;
    }//end if
    else if(dist==*min_dist)
    {   // give fluorescent more preference over D50
      if(*sample_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 ||
		 *sample_decision==CAM3A_AGW_AWB_OUTDOOR_NOON)
      {
        *sample_decision=CAM3A_AGW_AWB_INDOOR_WARM_FLO;
      }
    }
  }//end for


  for(index=0;index<cam3a_agw_ptr->n_aline1-1 && index<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
  {   // examine the A line 1
    d1=(rg_grid-cam3a_agw_ptr->Aline1[index][0]);
    d2=(bg_grid-cam3a_agw_ptr->Aline1[index][1]);
    dist=d1*d1+d2*d2;

    if(dist<*min_dist)
    {
      *min_dist=dist;   // update the new minimum distance
      *sample_decision=CAM3A_AGW_AWB_INDOOR_INCANDESCENT;
    }//end if

  }//end for

  for(index=1;index<cam3a_agw_ptr->n_aline2-1 && index<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
  {   // examine the A line 2
    d1=(rg_grid-cam3a_agw_ptr->Aline2[index][0]);
    d2=(bg_grid-cam3a_agw_ptr->Aline2[index][1]);
    dist=d1*d1+d2*d2;

    if(dist<*min_dist)
    {
      *min_dist=dist;   // update the new minimum distance
      *sample_decision=CAM3A_AGW_AWB_INDOOR_INCANDESCENT;
    }//end if
  }//end for

  return 0;
}

/*==============================================================================
  EXTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_AWB_CORE_MEM_ALLOCATE

DESCRIPTION
  allocate memory for AWB

CALLERS:
  camera_awb_mem_allocate

RETURN VALUE
  None

===========================================================================*/
boolean camera_awb_core_mem_allocate (void)
{
/* Pointer to initied and scratch memory for AGW White Balance */
  CAM_Free( cam3a_agw_ptr );
  cam3a_agw_ptr = CAM_Malloc( sizeof(cam3a_advanced_grey_world_type) );
  if(cam3a_agw_ptr == NULL)
  {
    camera_no_memory();
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION      CAMERA_AWB_CORE_MEM_DEALLOCATE

DESCRIPTION
  free the memory allocated for AWB

CALLERS:
  camera_awb_mem_deallocate

RETURN VALUE
  None

===========================================================================*/
void camera_awb_core_mem_deallocate (void)
{
  CAM_Free( cam3a_agw_ptr );
  cam3a_agw_ptr = NULL;
}

/*===========================================================================

FUNCTION      CAMERA_AWB_CORE_INIT_AGW_ALGORITHM

DESCRIPTION
  initiate core AWB algorithm using advanced grey world
  this function is using fixed point number

CALLERS:
  camera_awb_init_awb_advanced_grey_world_algo

RETURN VALUE
  None

===========================================================================*/
/*lint -save -e524 -e653 -e747 -e790 -e818*/
void camera_awb_core_init_agw_algorithm(void)
{
  int  index,index2,i;
  int  n;
  float  delta;
  int  rgx, bgx;
  float  r1,r2,b1,b2, m;
  float  awb_min_ratio=1.0, awb_max_ratio=1.0;
  int  awb_min_ratio_x=1, awb_max_ratio_x=1;
  /******************************************** experimental*/
  /*lint -save -e578*/
  float x1,y1,x2,y2,x3,y3,m1,m2,xp,yp,vx,vy;
  int kx=0,ky=0, dx=0,dy=0;
  /*lint -restore*/
  /******************************************** experimental*/

  if(cam3a_agw_ptr == NULL)
  {
    MSG_FATAL ("Camera: cam3a_agw_ptr is not allocated", 0, 0, 0);
    return;
  }

  cam3a_agw_ptr->indoor_F_WB_locked=0;  // rest the indoor flurescent lock

  /* clearing out past history */
  for(i=0;i<AWB_MAX_HISTORY;i++)
  {
    cam3a_agw_ptr->awb_history_rg_ratio[i]=0.0;
    cam3a_agw_ptr->awb_history_bg_ratio[i]=0.0;
    cam3a_agw_ptr->awb_history_decision[i]=CAM3A_AGW_AWB_INVALID;
    cam3a_agw_ptr->awb_history_exp[i]=-1;
    cam3a_agw_ptr->awb_history_replaced[i]=0;
    cam3a_agw_ptr->awb_history_is_white[i]=-1;
  }
  cam3a_agw_ptr->awb_history_num = 0;

  cam3a_agw_ptr->awb_history_complete_exp_cnt=0;
  for(i=0;i<AWB_AEC_MAX_HISTORY;i++)
  {
    cam3a_agw_ptr->awb_history_complete_exp[i]=-1;
  }

  n=(CAM3A_AGW_AWB_NUMBER_GRID_POINT-1)/2;

  for(i=0;i<CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT;i++)
  {
    if(cam3a_agw_ptr->red_gain_table[i]>awb_max_ratio)
      awb_max_ratio=cam3a_agw_ptr->red_gain_table[i];
    if(cam3a_agw_ptr->blue_gain_table[i]>awb_max_ratio)
      awb_max_ratio=cam3a_agw_ptr->blue_gain_table[i];
    if(cam3a_agw_ptr->red_gain_table[i]<awb_min_ratio)
      awb_min_ratio=cam3a_agw_ptr->red_gain_table[i];
    if(cam3a_agw_ptr->blue_gain_table[i]<awb_min_ratio)
      awb_min_ratio=cam3a_agw_ptr->blue_gain_table[i];

    /* prepare fixed point red/blue gain tables */
    cam3a_agw_ptr->red_gain_table_x[i]=(int32) (cam3a_agw_ptr->red_gain_table[i]*(1<<RGBG_GRID_Q_NUM));
    cam3a_agw_ptr->blue_gain_table_x[i]=(int32) (cam3a_agw_ptr->blue_gain_table[i]*(1<<RGBG_GRID_Q_NUM));
  }

  awb_max_ratio*=1.2;
  awb_min_ratio*=0.8;

/* the constraint on reciprocal symmetry is removed here to increase grid resolution */
  if(awb_max_ratio<(1.0/awb_min_ratio))
    awb_max_ratio=1.0/awb_min_ratio;
  else if(awb_min_ratio>(1.0/awb_max_ratio))
    awb_min_ratio=1.0/awb_max_ratio;

/* prepare fixed point min/max ratio */
  awb_min_ratio_x=(int32) (awb_min_ratio * (1<<RGBG_GRID_Q_NUM));
  awb_max_ratio_x=(int32) (awb_max_ratio * (1<<RGBG_GRID_Q_NUM));

// for region < 1
  delta=(1.0-awb_min_ratio)/((float) n+0.5);
  cam3a_agw_ptr->rgbg_grid[0]=awb_min_ratio;
  cam3a_agw_ptr->rgbg_grid_x[0]=( awb_min_ratio * (1<< RGBG_GRID_Q_NUM) );
  for(index=1; index<=n; index++)
  {
    cam3a_agw_ptr->rgbg_grid[index]=cam3a_agw_ptr->rgbg_grid[index-1]+delta;
    cam3a_agw_ptr->rgbg_grid_x[index]= (int32) (cam3a_agw_ptr->rgbg_grid[index]* (1<< RGBG_GRID_Q_NUM));
  }

// for region > 1
  delta=(awb_max_ratio-1.0)/((float)n+0.5);
//cam3a_agw_ptr->rgbg_grid[n+1]=1.0+delta;
  for(index=n+1; index<CAM3A_AGW_AWB_NUMBER_GRID_POINT; index++)
  {
    cam3a_agw_ptr->rgbg_grid[index]=cam3a_agw_ptr->rgbg_grid[index-1]+delta;
    cam3a_agw_ptr->rgbg_grid_x[index]= (int32) (cam3a_agw_ptr->rgbg_grid[index]* (1<< RGBG_GRID_Q_NUM));
  }
  cam3a_agw_ptr->rgbg_grid[CAM3A_AGW_AWB_NUMBER_GRID_POINT]=awb_max_ratio;
  cam3a_agw_ptr->rgbg_grid_x[CAM3A_AGW_AWB_NUMBER_GRID_POINT]=(int32) (awb_max_ratio* (1<< RGBG_GRID_Q_NUM));

// find the location of the 8 lighting conditions on the grid matrix
  for(index=0; index<CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT; index++)
  {
    // no normalization, need to set rg_adj and bg_adj to be 1
    rgx=cam3a_agw_ptr->red_gain_table_x[index];
    bgx=cam3a_agw_ptr->blue_gain_table_x[index];

    if(rgx<awb_min_ratio_x)
      cam3a_agw_ptr->rg_grid[index]=0;
    else if(rgx>=awb_max_ratio_x)
      cam3a_agw_ptr->rg_grid[index]=CAM3A_AGW_AWB_NUMBER_GRID_POINT-1;
    else
    {
      for(index2=0; index2<CAM3A_AGW_AWB_NUMBER_GRID_POINT-1; index2++)
      {
        if(((rgx-cam3a_agw_ptr->rgbg_grid_x[index2])*
          (rgx-cam3a_agw_ptr->rgbg_grid_x[index2+1]))<=0)
        {
          cam3a_agw_ptr->rg_grid[index]=index2;
          break;
        }
      }
    }

    // convert bg ratio from float to grid
    if(bgx<awb_min_ratio_x)
      cam3a_agw_ptr->bg_grid[index]=0;
    else if(bgx>=awb_max_ratio_x)
      cam3a_agw_ptr->bg_grid[index]=CAM3A_AGW_AWB_NUMBER_GRID_POINT-1;
    else
    {
      for(index2=0; index2<CAM3A_AGW_AWB_NUMBER_GRID_POINT-1; index2++)
      {
        if(((bgx-cam3a_agw_ptr->rgbg_grid_x[index2])*
          (bgx-cam3a_agw_ptr->rgbg_grid_x[index2+1]))<=0)
        {
          cam3a_agw_ptr->bg_grid[index]=index2;
          break;
        }
      }
    }
  }

// generate day light line 1 (between shade & D65)
  r1=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_CLOUDY];  //D75
  b1=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_CLOUDY];
  r2=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];  //D65
  b2=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];

  // make sure the line goes from D75 to D65
  if(fabs((double)(r1-r2))>=fabs((double)(b1-b2)))
  {
    if((r1-r2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(b1-b2)/(r1-r2);

    if(r2>r1)
    {  // from D75 to D65  (r1 < r2)
      cam3a_agw_ptr->n_day1=(int) (r2-r1)+1;
      for(index=(int) r1; (index<= (int) r2) && (r2-r1)<CAM3A_AGW_AWB_NUMBER_GRID_POINT; index++)
      {
        cam3a_agw_ptr->day_line_1[index-(int) r1][0]=index;
        cam3a_agw_ptr->day_line_1[index-(int) r1][1]=
          (int) (m*((float) index-r1) + b1+0.5);
      }
    }
    else
    { // if (r2<r1), calibration is wrong.
      cam3a_agw_ptr->n_day1=(int) (r1-r2)+1;
      for(index=(int) r2; (index<=(int) r1) && (r1-r2)<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
      {
        cam3a_agw_ptr->day_line_1[(cam3a_agw_ptr->n_day1-1)-(index-(int) r2)][0]=index;
        cam3a_agw_ptr->day_line_1[(cam3a_agw_ptr->n_day1-1)-(index-(int) r2)][1]=
          (int) (m*((float) index-r2) + b2+0.5);
      }
    }
  }
  else
  {
    if((b1-b2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(r1-r2)/(b1-b2);

    if(b2>b1)
    {  // if (b2>b1) calibration is wrong
      cam3a_agw_ptr->n_day1=(int) (b2-b1)+1;
      for(index=(int) b1; (index<=(int) b2)&& (b2-b1)<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
      {  // b2 is D65, b1 is D75
        cam3a_agw_ptr->day_line_1[index-(int) b1][0]=
          (int) (m*((float) index-b1) + r1+0.5);
        cam3a_agw_ptr->day_line_1[index-(int) b1][1]=index;
      }
    }
    else
    {   // b2<b1, make it go from D75 to D65.
      cam3a_agw_ptr->n_day1=(int) (b1-b2)+1;
      for(index=(int) b2; (index<=(int) b1) && (b1-b2)<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
      {  // b2 is D65, b1 is D75
        cam3a_agw_ptr->day_line_1[(cam3a_agw_ptr->n_day1-1)-(index-(int) b2)][0]=
          (int) (m*((float) index-b2) + r2+0.5);
        cam3a_agw_ptr->day_line_1[(cam3a_agw_ptr->n_day1-1)-(index-(int) b2)][1]=index;
      }
    }
  }
  if((cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]==
    cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_CLOUDY]) &&
    (cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]==
    cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_CLOUDY]))
  {
    cam3a_agw_ptr->n_day1=1;
    cam3a_agw_ptr->day_line_1[0][0]=
      cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];
    cam3a_agw_ptr->day_line_1[0][1]=
      cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];
  }

// generate day light line 2 (between D65 & D50)
  r1=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];   // D65
  b1=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];
  r2=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1];  // D50
  b2=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1];

  // from D65 to D50
  if(fabs((double)(r1-r2))>=fabs((double)(b1-b2)))
  {
    if((r1-r2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(b1-b2)/(r1-r2);

    if(r2>r1)
    {  // From D65 to D50
      cam3a_agw_ptr->n_day2=(int) (r2-r1)+1;
      for(index=(int) r1; (index<=(int) r2) && ((int)(r2-r1)<CAM3A_AGW_AWB_NUMBER_GRID_POINT);index++)
      { // r1 is D65, r2 is D50
        cam3a_agw_ptr->day_line_2[index-(int) r1][0]=index;
        cam3a_agw_ptr->day_line_2[index-(int) r1][1]=
          (int) (m*((float) index-r1) + b1+0.5);
      }
    }
    else
    {
    // if (r2< r1) calibration is wrong,
      cam3a_agw_ptr->n_day2=(int) (r1-r2)+1;
      for(index=(int) r2; (index<=(int) r1) && ((int)(r1-r2)<CAM3A_AGW_AWB_NUMBER_GRID_POINT);index++)
      {  // r1 is D65, r2 is D50
        cam3a_agw_ptr->day_line_2[(cam3a_agw_ptr->n_day2-1)-(index-(int) r2)][0]=index;
        cam3a_agw_ptr->day_line_2[(cam3a_agw_ptr->n_day2-1)-(index-(int) r2)][1]=
          (int) (m*((float) index-r2) + b2+0.5);
      }

    }
  }
  else
  {
    if((b1-b2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(r1-r2)/(b1-b2);

    if(b2>b1)
    {  // if (b2>b1) calibration is wrong
      cam3a_agw_ptr->n_day2=(int) (b2-b1)+1;
      for(index=(int) b1; (index<=(int) b2) && ((int)(b2-b1)<CAM3A_AGW_AWB_NUMBER_GRID_POINT);index++)
      {  // b2 is D50, b1 is D65
        cam3a_agw_ptr->day_line_2[index-(int) b1][0]=
          (int) (m*((float) index-b1) + r1+0.5);
        cam3a_agw_ptr->day_line_2[index-(int) b1][1]=index;
      }
    }
    else
    { // make it go from D65 to D50
      cam3a_agw_ptr->n_day2=(int) (b1-b2)+1;
      for(index=(int) b2; (index<=(int) b1) && ((int)(b1-b2)<CAM3A_AGW_AWB_NUMBER_GRID_POINT);index++)
      {  // b2 is D50, b1 is D65
        cam3a_agw_ptr->day_line_2[(cam3a_agw_ptr->n_day2-1)-(index-(int) b2)][0]=
          (int) (m*((float) index-b2) + r2+0.5);
        cam3a_agw_ptr->day_line_2[(cam3a_agw_ptr->n_day2-1)-(index-(int) b2)][1]=index;
      }
    }
  }
  if((cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]==
    cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1]) &&
    (cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]==
    cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1]))
  {
    cam3a_agw_ptr->n_day2=1;
    cam3a_agw_ptr->day_line_2[0][0]=
      cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];
    cam3a_agw_ptr->day_line_2[0][1]=
      cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];
  }

// add a fluorescent line from CW to TL
  r1=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_COLD_FLO];
  b1=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_COLD_FLO];
  r2=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_WARM_FLO];
  b2=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_WARM_FLO];

  // make sure the line goes from CW to TL84
  if(fabs(r1-r2)>=fabs(b1-b2))
  {
    if((r1-r2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(b1-b2)/(r1-r2);

    if(r2>r1)
    {
      cam3a_agw_ptr->n_fline=(int) (r2-r1)+1;
      for(index=(int) r1; index<=(int) r2;index++)
      {  //r1=CW, r2=TL
        cam3a_agw_ptr->Fline[index-(int) r1][0]=index;
        cam3a_agw_ptr->Fline[index-(int) r1][1]=(int) (m*((float) index-r1) + b1+0.5);
      }
    }
    else
    { // if (r2<r1) calibration is wrong
      cam3a_agw_ptr->n_fline=(int) (r1-r2)+1;
      for(index=(int) r2; index<=(int) r1;index++)
      { //r1=CW, r2=TL, make it go from CW to TL
        cam3a_agw_ptr->Fline[(cam3a_agw_ptr->n_fline-1)-(index-(int) r2)][0]=index;
        cam3a_agw_ptr->Fline[(cam3a_agw_ptr->n_fline-1)-(index-(int) r2)][1]=(int) (m*((float) index-r2) + b2+0.5);
      }
    }
  }
  else
  {
    if((b1-b2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(r1-r2)/(b1-b2);

    if(b2>b1)
    {
      cam3a_agw_ptr->n_fline=(int) (b2-b1)+1;
      for(index=(int) b1; index<=(int) b2;index++)
      {  // b1= CW, b2=TL
        cam3a_agw_ptr->Fline[index-(int) b1][0]=(int) (m*((float) index-b1) + r1+0.5);
        cam3a_agw_ptr->Fline[index-(int) b1][1]=index;
      }
    }
    else
    {
      cam3a_agw_ptr->n_fline=(int) (b1-b2)+1;
      for(index=(int) b2; index<=(int) b1;index++)
      {  // b1= CW, b2=TL
        cam3a_agw_ptr->Fline[(cam3a_agw_ptr->n_fline-1)-(index-(int) b2)][0]=(int) (m*((float) index-b2) + r2+0.5);
        cam3a_agw_ptr->Fline[(cam3a_agw_ptr->n_fline-1)-(index-(int) b2)][1]=index;
      }
    }
  }

  // add a fluorescent line from D50 to (CW+TL84)/2 , 3A 1.4 change
  r1=(float) 0.5*(cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_WARM_FLO]+cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_COLD_FLO]);
  b1=(float) 0.5*(cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_WARM_FLO]+cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_COLD_FLO]);
  r2=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1];
  b2=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1];
 
  // make sure the line goes from D50 to fluorescent
  if(fabs(r1-r2)>=fabs(b1-b2))
  {
    if((r1-r2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(b1-b2)/(r1-r2);

    if(r2>r1)
    {
      cam3a_agw_ptr->n_day_f_line=(int) (r2-r1)+1;
      for(index=(int) r1; index<=(int) r2;index++)
      { 
        cam3a_agw_ptr->Day_F_line[index-(int) r1][0]=index;
        cam3a_agw_ptr->Day_F_line[index-(int) r1][1]=(int) (m*((float) index-r1) + b1+0.5);
      }
    }
    else
    { 
      cam3a_agw_ptr->n_day_f_line=(int) (r1-r2)+1;
      for(index=(int) r2; index<=(int) r1;index++)
      { 
        cam3a_agw_ptr->Day_F_line[(cam3a_agw_ptr->n_day_f_line-1)-(index-(int) r2)][0]=index;
        cam3a_agw_ptr->Day_F_line[(cam3a_agw_ptr->n_day_f_line-1)-(index-(int) r2)][1]=(int) (m*((float) index-r2) + b2+0.5);
      }
    }
  }
  else
  {
    if((b1-b2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(r1-r2)/(b1-b2);

    if(b2>b1)
    {
      cam3a_agw_ptr->n_day_f_line=(int) (b2-b1)+1;
      for(index=(int) b1; index<=(int) b2;index++)
      {  
        cam3a_agw_ptr->Day_F_line[index-(int) b1][0]=(int) (m*((float) index-b1) + r1+0.5);
        cam3a_agw_ptr->Day_F_line[index-(int) b1][1]=index;
      }
    }
    else
    {
      cam3a_agw_ptr->n_day_f_line=(int) (b1-b2)+1;
      for(index=(int) b2; index<=(int) b1;index++)
      {  
        cam3a_agw_ptr->Day_F_line[(cam3a_agw_ptr->n_day_f_line-1)-(index-(int) b2)][0]=(int) (m*((float) index-b2) + r2+0.5);
        cam3a_agw_ptr->Day_F_line[(cam3a_agw_ptr->n_day_f_line-1)-(index-(int) b2)][1]=index;
      }
    }
  }

// A line 1: from 3200K to A
// this point (3200K) is estimated by TL and A
  i=((cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_WARM_FLO]+
    2*cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT])/3);
  r1=(float) i;
  i=(float) ((cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_WARM_FLO]+
    2*cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT])/3);      // 3200K
  b1=(float) i;

  r2=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT];
  b2=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT];   // A

  // make A line 1 from 3200 to A.
  if(fabs(r1-r2)>=fabs(b1-b2))
  {
    if((r1-r2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(b1-b2)/(r1-r2);

    if(r2>r1)
    {
      cam3a_agw_ptr->n_aline1=(int) (r2-r1)+1;
      for(index=(int) r1; (index<=(int) r2 && ((index-(int) r1) < CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS)); index++)
      { // r1 = 3200K, r2 = A
        cam3a_agw_ptr->Aline1[index-(int) r1][0]=index;
        cam3a_agw_ptr->Aline1[index-(int) r1][1]=(int) (m*((float) index-r1) + b1+0.5);
      }
    }
    else
    {
      cam3a_agw_ptr->n_aline1=(int) (r1-r2)+1;
      for(index=(int) r2; index<=(int) r1;index++)
      { // r1 = 3200K, r2 = A
        if((cam3a_agw_ptr->n_aline1-1)-(index-(int) r2) < CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS)
        {
        cam3a_agw_ptr->Aline1[(cam3a_agw_ptr->n_aline1-1)-(index-(int) r2)][0]=index;
        cam3a_agw_ptr->Aline1[(cam3a_agw_ptr->n_aline1-1)-(index-(int) r2)][1]=(int) (m*((float) index-r2) + b2+0.5);
      }
    }
  }
  }
  else
  {
    if((b1-b2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(r1-r2)/(b1-b2);

    if(b2>b1)
    {
      cam3a_agw_ptr->n_aline1=(int) (b2-b1)+1;
      for(index=(int) b1; (index<=(int) b2 && ((index-(int) b1) < CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS));index++)
      { // b1=3200K, b2= A
        cam3a_agw_ptr->Aline1[index-(int) b1][0]=(int) (m*((float) index-b1) + r1+0.5);
        cam3a_agw_ptr->Aline1[index-(int) b1][1]=index;
      }
    }
    else
    {
      cam3a_agw_ptr->n_aline1=(int) (b1-b2)+1;
      for(index=(int) b2; index<=(int) b1;index++)
      { // b1=3200K, b2= A
        if((cam3a_agw_ptr->n_aline1-1)-(index-(int) b2) < CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS)
        {
        cam3a_agw_ptr->Aline1[(cam3a_agw_ptr->n_aline1-1)-(index-(int) b2)][0]=(int) (m*((float) index-b2) + r2+0.5);
        cam3a_agw_ptr->Aline1[(cam3a_agw_ptr->n_aline1-1)-(index-(int) b2)][1]=index;
      }
    }
  }
  }

//  A line 2:  from A to H
  r1=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT];
  b1=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT];
  r2=(float) cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_HORIZON];
  b2=(float) cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON];
  // make it go from A to H
  if(fabs(r1-r2)>=fabs(b1-b2))
  {
    if((r1-r2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(b1-b2)/(r1-r2);

    if(r2>r1)
    {
      cam3a_agw_ptr->n_aline2=(int) (r2-r1)+1;
      for(index=(int) r1; index<=(int) r2;index++)
      { // r1=A, r2=H
        cam3a_agw_ptr->Aline2[index-(int) r1][0]=index;
        cam3a_agw_ptr->Aline2[index-(int) r1][1]=(int) (m*((float) index-r1) + b1+0.5);
      }
    }
    else
    {
      cam3a_agw_ptr->n_aline2=(int) (r1-r2)+1;
      for(index=(int) r2; index<=(int) r1;index++)
      { // r1=A, r2=H
        cam3a_agw_ptr->Aline2[(cam3a_agw_ptr->n_aline2-1)-(index-(int) r2)][0]=index;
        cam3a_agw_ptr->Aline2[(cam3a_agw_ptr->n_aline2-1)-(index-(int) r2)][1]=(int) (m*((float) index-r2) + b2+0.5);
      }
    }
  }
  else
  {
    if((b1-b2) == 0)  /* divide by 0 protection */
      m=0;
    else
      m=(r1-r2)/(b1-b2);

    if(b2>b1)
    {
      cam3a_agw_ptr->n_aline2=(int) (b2-b1)+1;
      for(index=(int) b1; index<=(int) b2;index++)
      { // b1=A, b2=H
        cam3a_agw_ptr->Aline2[index-(int) b1][0]=(int) (m*((float) index-b1) + r1+0.5);
        cam3a_agw_ptr->Aline2[index-(int) b1][1]=index;
      }
    }
    else
    {
      cam3a_agw_ptr->n_aline2=(int) (b1-b2)+1;
      for(index=(int) b2; index<=(int) b1;index++)
      { // b1=A, b2=H
        cam3a_agw_ptr->Aline2[(cam3a_agw_ptr->n_aline2-1)-(index-(int) b2)][0]=(int) (m*((float) index-b2) + r2+0.5);
        cam3a_agw_ptr->Aline2[(cam3a_agw_ptr->n_aline2-1)-(index-(int) b2)][1]=index;
      }

    }
  }

  /* estimate the green line (fixed point) */
  cam3a_agw_ptr->green_line_mx=(cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]-
    cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1])*(1<< GREEN_Q_NUM);
  n=(cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]-
    cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1]);
  if(n==0) n=1;  /* make sure no divide-by-zero */
  cam3a_agw_ptr->green_line_mx=(cam3a_agw_ptr->green_line_mx)/n;   /* fixed point slope with Q number = GREEN_Q_NUM */

  n=cam3a_agw_ptr->green_offset_rg+ CAM3A_MIN(cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1],
    cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_COLD_FLO]);
  cam3a_agw_ptr->green_line_bx= cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]+cam3a_agw_ptr->green_offset_bg-
    (cam3a_agw_ptr->green_line_mx*n)/(1<<GREEN_Q_NUM);

  //**************************************************************************
  // point1 (x1,y1) is D65
  x1=cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];  
  y1=cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]; 
  // point2 (x2,y2) is D50
  x2=cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1];
  y2=cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1];
  // point3 (x3,y3) is the high noon RG and BG ratio
  x3=cam3a_agw_ptr->red_gain_table[AGW_AWB_OUTDOOR_NOON];
  y3=cam3a_agw_ptr->blue_gain_table[AGW_AWB_OUTDOOR_NOON];

  m1=(y2-y1)/(x2-x1);
  b1=y2-m1*x2;
  m2=-1.0/m1;
  b2=y3 - m2*x3;
  // (xp,yp) is the intersection point of line 1 and line 2
  xp=(b2-b1)/(m1-m2);
  yp=m1*xp+b1;

  vx=(x3-xp);  // (vx,vy) is the vector pointing from (xp,yp) to high noon)
  vy=(y3-yp);

  cam3a_agw_ptr->shifted_d50_rg=x2+vx;
  cam3a_agw_ptr->shifted_d50_bg=y2+vy;
  MSG_CAM3ADEBUG("d65_rg=%d, d65_bg =%d", 1000*x1,1000*y1,0);
  MSG_CAM3ADEBUG("d50_rg=%d, d50_bg =%d", 1000*x2,1000*y2,0);
  MSG_CAM3ADEBUG("noon_rg=%d, noon_bg =%d", 1000*x3,1000*y3,0);
  MSG_CAM3ADEBUG("xp=%d, yp =%d", 1000*xp,1000*yp,0);
  MSG_CAM3ADEBUG("vx=%d, vy =%d", 1000*vx,1000*vy,0);

  MSG_CAM3ADEBUG("shifted d50_rg=%d, shifted d50_bg =%d", 
    1000*cam3a_agw_ptr->shifted_d50_rg,1000*cam3a_agw_ptr->shifted_d50_bg,0);

  // D65 shifted = (x1,y1) in floating point
  x1=cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]+vx;
  y1=cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]+vy;
  MSG_CAM3ADEBUG("shifted D65 (float): rg=%d, bg =%d", 1000*x1,1000*y1,0);
  // shifted D65 in fixed point
  rgx=(int32) (x1*(1<<RGBG_GRID_Q_NUM));
  bgx=(int32) (y1*(1<<RGBG_GRID_Q_NUM));

  for(index2=0; index2<CAM3A_AGW_AWB_NUMBER_GRID_POINT; index2++)
  {
    if(((rgx-cam3a_agw_ptr->rgbg_grid_x[index2])*
      (rgx-cam3a_agw_ptr->rgbg_grid_x[index2+1]))<=0)
    {
      kx=index2;
      break;
    }
  }
  for(index2=0; index2<CAM3A_AGW_AWB_NUMBER_GRID_POINT; index2++)
  {
    if(((bgx-cam3a_agw_ptr->rgbg_grid_x[index2])*
      (bgx-cam3a_agw_ptr->rgbg_grid_x[index2+1]))<=0)
    {
      ky=index2;
      break;
    }
  }

  MSG_CAM3ADEBUG("shifted D65 (grid): rg=%d, bg =%d", kx,ky,0);

  dx=kx-cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];
  dy=ky-cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];

  MSG_CAM3ADEBUG("Delta (grid) for D65 is: dx=%d, dy =%d", dx,dy,0);

  cam3a_agw_ptr->n_day3=cam3a_agw_ptr->n_day2;
  for(i=0;(i<cam3a_agw_ptr->n_day3) && i<CAM3A_AGW_AWB_NUMBER_GRID_POINT;i++)
  {
    cam3a_agw_ptr->day3_rg[i]=dx+cam3a_agw_ptr->day_line_2[i][0];
    cam3a_agw_ptr->day3_bg[i]=dy+cam3a_agw_ptr->day_line_2[i][1];
  }

#if 1
  for(i=0;i<cam3a_agw_ptr->n_day3; i++)
  {
    MSG_CAM3ADEBUG("Day line 3=(%d, %d)", cam3a_agw_ptr->day3_rg[i],cam3a_agw_ptr->day3_bg[i],0);
  }
#endif
  // ***************************************************************************

  // print the line parameters for debug purpose
#if 1
  MSG_CAM3ADEBUG("------------ AWB init debug -----------", 0,0,0);
  for(i=0;(i<cam3a_agw_ptr->n_day1) && i<CAM3A_AGW_AWB_NUMBER_GRID_POINT; i++)
  {
    MSG_CAM3ADEBUG("Day line 1=(%d, %d)", cam3a_agw_ptr->day_line_1[i][0],cam3a_agw_ptr->day_line_1[i][1],0);
  }
  for(i=0;i<cam3a_agw_ptr->n_day2 && i<CAM3A_AGW_AWB_NUMBER_GRID_POINT; i++)
  {
    MSG_CAM3ADEBUG("Day line 2=(%d, %d)", cam3a_agw_ptr->day_line_2[i][0],cam3a_agw_ptr->day_line_2[i][1],0);
  }
  for(i=0;i<cam3a_agw_ptr->n_fline; i++)
  {
    MSG_CAM3ADEBUG("F line =(%d, %d)", cam3a_agw_ptr->Fline[i][0],cam3a_agw_ptr->Fline[i][1],0);
  }
  for(i=0;i<cam3a_agw_ptr->n_aline1; i++)
  {
    MSG_CAM3ADEBUG("A line 1=(%d, %d)", cam3a_agw_ptr->Aline1[i][0],cam3a_agw_ptr->Aline1[i][1],0);
  }
  for(i=0;i<cam3a_agw_ptr->n_aline2; i++)
  {
    MSG_CAM3ADEBUG("A line 2=(%d, %d)", cam3a_agw_ptr->Aline2[i][0],cam3a_agw_ptr->Aline2[i][1],0);
  }
  MSG_CAM3ADEBUG("------------ END AWB init debug -----------", 0,0,0);
#endif
} /* awb_init_agw_algorithm */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_AWB_CORE_RUN_AGW_ALGORITHM

DESCRIPTION
  core AWB algorithm using advanced grey world
  this function is using fixed point number

CALLERS:
  camera_awb_do_awb_advanced_grey_world_algo

RETURN VALUE
  awb_output_type

===========================================================================*/
/*lint -save -e774 -e550 -e534 -e644 -e539 -e525 -e701 -e702 -e831 -e818 -e414 -e736*/
awb_output_type camera_awb_core_run_agw_algorithm(awb_input_stats_type* input_stats_ptr,
  camera_exposure_info_type* input_exposure_ptr)
{
/* Organize declarations & malloc larger chunks
  Or, since this function is called a lot, move scratch to make
  it global  */
  int  index=0;
  int  rx,gx,bx;
  int32  x_cnt=0;
  int32  min_dist,  d1,d2, dist;
  float  r_gainf = 1.0, g_gainf = 1.0, b_gainf = 1.0, gain_min;
  int  x_rg_grid = 0, x_bg_grid = 0;
  cam3a_auto_white_balance_illuminant_type  sample_decision=CAM3A_AGW_AWB_INVALID;
  float  rg_ratio = 1.0, bg_ratio = 1.0;
  int  rg_ratio_x = 1, bg_ratio_x = 1;
  int  n_outlier=0;
  float  ave_rg_ratio, ave_bg_ratio;
  int  ave_rg_ratio_x, ave_bg_ratio_x;
  int  ave_rg_grid = 0, ave_bg_grid = 0;
  float  simple_rg_ratio, simple_bg_ratio;
  int  simple_rg_ratio_x, simple_bg_ratio_x;
  int  flg_all_outliers = 0;      /* all outliers */
  int  sample_cnt = 0;
  int weight_per_sample;
  int a_cluster=0, f_cluster=0, day_cluster=0;
  int a_cluster_rg_x=0, a_cluster_bg_x=0;
  int f_cluster_rg_x=0, f_cluster_bg_x=0;
  int day_cluster_rg_x=0, day_cluster_bg_x=0;
  int max_dist=0;
  int is_a_compact_cluster=0;
  int dayline=0, dayidx=0;
  float w1,w2;
  int p1=0, p2=0, p3=0, p4=0;
  int possible_green_cnt=0, possible_indoor_green_cnt=0;
  int outlier_a_cnt=0, outlier_day_cnt=0, outlier_f_cnt=0;
  float ftmp;
  int green_rgx=0, green_bgx=0;
  boolean decision_changed;
  awb_output_type output;
  int f_idx=-1, a_idx=-1, aline=-1;
  float r_adj, b_adj;
  boolean under_low_light;
  float d55_rg=1.0, d55_bg=1.0;

  memset(outlier_rg_grid, 0, sizeof(int)*256);
  memset(outlier_bg_grid, 0, sizeof(int)*256);
  output.is_confident = FALSE; /* not confident as a default */
  x_cnt=0;
  n_outlier=0;
  ave_rg_ratio=0.0;
  ave_bg_ratio=0.0;
  simple_rg_ratio=0.0;
  simple_bg_ratio=0.0;
  sample_cnt=0;
  decision_changed = FALSE;

  ave_rg_ratio_x=0;
  ave_bg_ratio_x=0;
  simple_rg_ratio_x=0;
  simple_bg_ratio_x=0;
  ftmp=1.0/(float) (1<<RGBG_GRID_Q_NUM);
  under_low_light = FALSE;

  MSG_CAM3ADEBUG("indoor_index=%d, outdoor_index=%d, max_index=%d", 
    input_exposure_ptr->indoor_index, input_exposure_ptr->outdoor_index, input_exposure_ptr->max_exp_index);

  for(index=0; index < input_stats_ptr->bin_cnt; index++)
  {
    rx = input_stats_ptr->rx[index];
    gx = input_stats_ptr->gx[index];
    bx = input_stats_ptr->bx[index];

    /* R, G, B are Q10 numbers */
    if(gx==0)
      gx=1;  // avoid divid-by-zero
    rg_ratio_x= ((rx<<RGBG_GRID_Q_NUM)+ (gx>>1))/gx;
    bg_ratio_x= ((bx<<RGBG_GRID_Q_NUM)+ (gx>>1))/gx;
    simple_rg_ratio_x+=rg_ratio_x;
    simple_bg_ratio_x+=bg_ratio_x;

    // compute the grid coordinate on the 2D plot
    camera_awb_core_convert_to_grid(rg_ratio_x, bg_ratio_x, &x_rg_grid, &x_bg_grid);

    // if a sample falls into this zone, it is likely to be a green sample in outdoor lighting
    // but it's not 100% certainty
    p1=CAM3A_MIN(cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_COLD_FLO],
      cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1]);   // min of (CW, D50)  
    p2=cam3a_agw_ptr->green_offset_bg+(cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]+ 
      cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1])/2;   // (D50+D65)/2 bg_ratio  
    p3= cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON]+ cam3a_agw_ptr->green_offset_bg;
    p4=((cam3a_agw_ptr->green_line_mx*x_rg_grid)/(1<<GREEN_Q_NUM) + cam3a_agw_ptr->green_line_bx);

    if(x_bg_grid<=p4 && x_rg_grid>0 && x_rg_grid<=(p1+cam3a_agw_ptr->green_offset_rg) 
      && x_bg_grid<p2 && x_bg_grid>p3)
    {
      possible_green_cnt++;
      green_rgx+=x_rg_grid;
      green_bgx+=x_bg_grid;
    }

    // count the samples that may be indoor green pixels
    if(x_rg_grid<p1 &&  x_bg_grid<=p3)
      possible_indoor_green_cnt++;

    if(x_bg_grid<cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON] && x_rg_grid>=p1 &&
      x_rg_grid<cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT])
      possible_indoor_green_cnt++;

    // the outdoor green and indoor green have some overlap. they are not 100% clear cut
    // then, test whether this point is an outlier
    // first, comparing it with the all reference points

    camera_awb_core_search_nearest_reference_point(x_rg_grid, x_bg_grid, &sample_decision, &min_dist, &dayidx);

    // if the minimum distance from sample to the reference points are
    // greater than the threshold, it's an outlier
    if(min_dist>(cam3a_agw_ptr->awb_agw_grid_dist_2_threshold*cam3a_agw_ptr->awb_agw_grid_dist_2_threshold) ||
      x_bg_grid>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_CLOUDY] || 
      x_rg_grid>(cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_HORIZON]+3) ||
      x_bg_grid<(cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON]-3))
    {   // collect outliers
      outlier_rg_grid[n_outlier]=x_rg_grid;
      outlier_bg_grid[n_outlier]=x_bg_grid;
      n_outlier++;
      // keep track of outlier's distribution in terms of 3 clusters
      if(sample_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
        sample_decision==CAM3A_AGW_AWB_HORIZON)
      {
        outlier_a_cnt++;
      }
      else if(sample_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
        sample_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
        sample_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
      {
        outlier_f_cnt++;
      }
      else
      {
        outlier_day_cnt++;
      }
    }
    else
    {   // collect valid samples
      cam3a_agw_ptr->sample_rg_grid[sample_cnt]=x_rg_grid;
      cam3a_agw_ptr->sample_bg_grid[sample_cnt]=x_bg_grid;
      sample_cnt++;

      // continuous weight vector 
      if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->indoor_index)
      {   // dark situation, use indoor weighting
        if((int)sample_decision >= 0)
          weight_per_sample = cam3a_agw_ptr->awb_weight_vector[sample_decision][0];
        else
          weight_per_sample=0;
      }
      else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->inoutdoor_midpoint)
      {   // indoor < idx < inout mid
        p1=input_exposure_ptr->current_exposure_index- input_exposure_ptr->inoutdoor_midpoint;
        p2=input_exposure_ptr->indoor_index-input_exposure_ptr->current_exposure_index;
        if((int)sample_decision >= 0)
          weight_per_sample = (p1*cam3a_agw_ptr->awb_weight_vector[sample_decision][0]+  
            p2*cam3a_agw_ptr->awb_weight_vector[sample_decision][2])/(p1+p2);
        else
          weight_per_sample=0;
      }
      else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->outdoor_index)
      {   // inout mid < idx < outdoor
        p1=input_exposure_ptr->current_exposure_index- input_exposure_ptr->outdoor_index;
        p2=input_exposure_ptr->inoutdoor_midpoint-input_exposure_ptr->current_exposure_index;
        if((int)sample_decision >= 0)
          weight_per_sample = (p1*cam3a_agw_ptr->awb_weight_vector[sample_decision][2]+  
            p2*cam3a_agw_ptr->awb_weight_vector[sample_decision][1])/(p1+p2);
        else
          weight_per_sample=0;
      }
      else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->outdoor_midpoint)
      {   // outdoor < idx < out mid
        p1=input_exposure_ptr->current_exposure_index- input_exposure_ptr->outdoor_midpoint;
        p2=input_exposure_ptr->outdoor_index-input_exposure_ptr->current_exposure_index;
        if((int)sample_decision >= 0)
          weight_per_sample = (p1*cam3a_agw_ptr->awb_weight_vector[sample_decision][2]+  
            p2*cam3a_agw_ptr->awb_weight_vector[sample_decision][1])/(p1+p2);
        else
          weight_per_sample=0;
      }
      else
      {    // idx < outdoor_midpoint
        p1=input_exposure_ptr->current_exposure_index;
        p2=input_exposure_ptr->outdoor_midpoint-input_exposure_ptr->current_exposure_index;
        if(sample_decision==CAM3A_AGW_AWB_OUTDOOR_CLOUDY)
        {   // shade D75
          weight_per_sample=(p1*cam3a_agw_ptr->awb_weight_vector[sample_decision][1]+  
            p2*1)/(p1+p2);
        }
        else if(sample_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT)
        {   //  D65
          weight_per_sample=(p1*cam3a_agw_ptr->awb_weight_vector[sample_decision][1]+   
            p2*2)/(p1+p2);
        }
        else if(sample_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID)
        {   //  Daylight lines
          if(dayidx==1)
            weight_per_sample=(p1*cam3a_agw_ptr->awb_weight_vector[sample_decision][1]+   
              p2*2)/(p1+p2);
          else
            weight_per_sample=(p1*cam3a_agw_ptr->awb_weight_vector[sample_decision][1]+   
              p2*cam3a_agw_ptr->awb_weight_vector[sample_decision][1]/3)/(p1+p2);
        }
        else
          weight_per_sample = cam3a_agw_ptr->awb_weight_vector[sample_decision][1];
      }

      // compute weighted R/G and B/G for valid samples
      x_cnt += (weight_per_sample);
      ave_rg_ratio_x +=(rg_ratio_x * weight_per_sample);
      ave_bg_ratio_x +=(bg_ratio_x * weight_per_sample);

      // classify the valid samples into 3 different cluster
      if(sample_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
        sample_decision==CAM3A_AGW_AWB_HORIZON)
      {   // A light cluster
        a_cluster++;
        a_cluster_rg_x+=rg_ratio_x;
        a_cluster_bg_x+=bg_ratio_x;
      }
      else if(sample_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
        sample_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
        sample_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
      {   // Fluorescent light cluster
        f_cluster++;
        f_cluster_rg_x+=rg_ratio_x;
        f_cluster_bg_x+=bg_ratio_x;
      }
      else
      {   // outdoor daylight cluster
        day_cluster++;
        day_cluster_rg_x+=rg_ratio_x;
        day_cluster_bg_x+=bg_ratio_x;
      }
    }
  }  // end of loop index

// computer simple grey world average R/G and B/G ratios
  simple_rg_ratio_x=(simple_rg_ratio_x + (input_stats_ptr->bin_cnt>>1))/input_stats_ptr->bin_cnt;
  simple_bg_ratio_x=(simple_bg_ratio_x + (input_stats_ptr->bin_cnt>>1))/input_stats_ptr->bin_cnt;
  simple_rg_ratio=(float) simple_rg_ratio_x*ftmp;
  simple_bg_ratio=(float) simple_bg_ratio_x*ftmp;

  if(possible_green_cnt>0)
  {   // compute the average of the green samples
    green_rgx/=possible_green_cnt;
    green_bgx/=possible_green_cnt;
  }

  if(cam3a_awb_state.regular_stat_on == TRUE)
    cam3a_awb_state.reg_green_cnt = possible_green_cnt;

  MSG_CAM3ADEBUG("### sgw_rg_ratio = %d , sgw_bg_ratio = %d. valid sample cnt=%d", 
    simple_rg_ratio*1000, simple_bg_ratio*1000,sample_cnt);
  MSG_CAM3ADEBUG("### outdoor_green=%d, indoor green=%d, x_cnt=%d", 
    possible_green_cnt, possible_indoor_green_cnt,x_cnt);
  MSG_CAM3ADEBUG("### day cluster=%d, f cluster=%d, a_cluster=%d", day_cluster, f_cluster, a_cluster);
//  compute the average R/G and B/G of all the pixels closed to all reference points

  if(cam3a_awb_state.white_stat_on == TRUE && day_cluster==0 && a_cluster==0 && f_cluster==0)
  {
    if(input_exposure_ptr->current_exposure_index == input_exposure_ptr->max_exp_index)
    {
      if(cam3a_awb_state.white_y_min_percent >=(YMIN_LOW_LIMIT-20))
      {
        cam3a_awb_state.white_y_min_percent--;
        MSG_CAM3ADEBUG("### white stat: low light: reduced y_min_percentage= %d", cam3a_awb_state.white_y_min_percent,0,0);
      }
    }
    else
    {
      if(cam3a_awb_state.white_y_min_percent >=YMIN_LOW_LIMIT)
      {
        cam3a_awb_state.white_y_min_percent--;
        MSG_CAM3ADEBUG("### white stat: reduced y_min_percentage= %d", cam3a_awb_state.white_y_min_percent,0,0);
      }
    }
    MSG_CAM3ADEBUG("### white stat, all outliers, low confidence, return", 0,0,0);
    cam3a_awb_state.white_decision =CAM3A_AGW_AWB_INVALID;
    return output;
  }

  if(input_exposure_ptr->current_exposure_index == input_exposure_ptr->max_exp_index
    && day_cluster==0 && f_cluster==0 && a_cluster==0)
  {
    /* low light & all of them are outliers */
    camera_awb_core_convert_to_grid(simple_rg_ratio_x, simple_bg_ratio_x, &x_rg_grid, &x_bg_grid);
    camera_awb_core_search_nearest_reference_point(x_rg_grid, x_bg_grid, &sample_decision, &min_dist, &dayidx);

    if(min_dist>(cam3a_agw_ptr->awb_agw_grid_dist_2_threshold*cam3a_agw_ptr->awb_agw_grid_dist_2_threshold*2))
    { /* the collected sample average is too far from any reference point. Too dangerous to perform SGW.
         reject this frame */
      MSG_CAM3ADEBUG("### all outliers, SGW ave to nearest ref point=%d, low confidence, reject", min_dist,0,0);
      return output;
    }

    // if the past history has data, keep same decision
    if (cam3a_agw_ptr->awb_history_num>3)
    {   // 3A 1.4 changes (low lgith with history)
      MSG_CAM3ADEBUG("### low light, use past history, reject this frame", min_dist,0,0);		
      return output;   
    }

    rg_ratio = simple_rg_ratio;
    bg_ratio = simple_bg_ratio;
    g_gainf = 1.0;
    r_gainf=1.0/rg_ratio;
    b_gainf=1.0/bg_ratio;
    MSG_CAM3ADEBUG("lowlight: use simple grey world. set decision=%d\n", sample_decision,0,0);

    under_low_light = TRUE;
  }

  if(under_low_light == FALSE) /* logic only for normol light */
  {
  // here, a dominant cluster concept is introduced. A dominant cluster means the
  // number of samples fall into this cluster exceeds a user-defined threshold
    if(sample_cnt>0)
    { // normal samples
      // there are some valid samples
      // this heuristics is for stable performance for MCC in lightbox
      // make decision based on cluster statistics
      if(a_cluster>((cam3a_agw_ptr->a_cluster_threshold*sample_cnt)/100)
        && input_exposure_ptr->current_exposure_index>=input_exposure_ptr->indoor_index)
      { // A cluster is a dominant winner , not too bright
        // use only A cluster for white balance
        if(a_cluster == 0)
          a_cluster = 1;
        ave_rg_ratio_x=(a_cluster_rg_x + (a_cluster>>1))/a_cluster;
        ave_bg_ratio_x=(a_cluster_bg_x + (a_cluster>>1))/a_cluster;
        MSG_CAM3ADEBUG("### A cluster is dominant", 0,0,0);
      }
      else if(f_cluster>((cam3a_agw_ptr->f_cluster_threshold*sample_cnt)/100) 
        && input_exposure_ptr->current_exposure_index>=input_exposure_ptr->indoor_index)
      { // F cluster is a dominant winner, not too bright
        // use only F cluster for white balance
        if(f_cluster == 0)
          f_cluster = 1;
        ave_rg_ratio_x=(f_cluster_rg_x + (f_cluster>>1))/f_cluster;
        ave_bg_ratio_x=(f_cluster_bg_x + (f_cluster>>1))/f_cluster;
        MSG_CAM3ADEBUG("### F cluster is dominant", 0,0,0);
      }
      else if(day_cluster>((cam3a_agw_ptr->day_cluster_threshold*sample_cnt)/100))
      { // day cluster is the dominant winner
        // use only Day cluster for white balance
        if(day_cluster == 0)
          day_cluster = 1;
        ave_rg_ratio_x=(day_cluster_rg_x + (day_cluster>>1))/day_cluster;
        ave_bg_ratio_x=(day_cluster_bg_x + (day_cluster>>1))/day_cluster;
        MSG_CAM3ADEBUG("### Day cluster is dominant", 0,0,0);
      }
      else
      { // use weight vector
        if(x_cnt == 0)
          x_cnt = 1;
        /*lint -save -e704*/
        ave_rg_ratio_x=(ave_rg_ratio_x + (x_cnt>>1))/x_cnt;
        ave_bg_ratio_x=(ave_bg_ratio_x + (x_cnt>>1))/x_cnt;
        /*lint -restore*/
        MSG_CAM3ADEBUG("### No dominant cluster, use WV", 0,0,0);
      }

      ave_rg_ratio=(float) ave_rg_ratio_x*ftmp;
      ave_bg_ratio=(float) ave_bg_ratio_x*ftmp;

      if(cam3a_awb_state.regular_stat_on == TRUE)
      {
        cam3a_awb_state.regular_ave_rg_ratio = ave_rg_ratio;
        cam3a_awb_state.regular_ave_bg_ratio = ave_bg_ratio;
        MSG_CAM3ADEBUG("### regular stat : ave_rg_ratio = %d , ave_bg_ratio = %d", ave_rg_ratio*1000, ave_bg_ratio*1000, 0);
      }
      if(cam3a_awb_state.white_stat_on == TRUE)
      {
        cam3a_awb_state.white_ave_rg_ratio = ave_rg_ratio;
        cam3a_awb_state.white_ave_bg_ratio = ave_bg_ratio;
        MSG_CAM3ADEBUG("### white stat : ave_rg_ratio = %d , ave_bg_ratio = %d", ave_rg_ratio*1000, ave_bg_ratio*1000, 0);
      }
    }
    else
    { // all the samples are ouliers,
      if(cam3a_awb_state.white_stat_on == TRUE)
      {
        cam3a_awb_state.white_decision = CAM3A_AGW_AWB_INVALID;
        cam3a_awb_state.white_has_single_peak=FALSE;
        MSG_CAM3ADEBUG("### white stat : all outliers. return", 0, 0, 0);
        if(cam3a_awb_state.white_y_min_percent >= YMIN_LOW_LIMIT)
          cam3a_awb_state.white_y_min_percent--;
        MSG_CAM3ADEBUG("### y_min_percentage= %d", cam3a_awb_state.white_y_min_percent,0,0);
        return output;
      }

    // when max exposure index is used and it's all outliers situation,
    // the current frame is not trustworthy. Reject
      if(input_exposure_ptr->current_exposure_index==input_exposure_ptr->max_exp_index)
      {
        MSG_CAM3ADEBUG("### all outliers, too dark, reject", 0,0,0);
        return output;
      }

      // sample_cnt==0
      // this is the fallback solution
      ave_rg_ratio=simple_rg_ratio;
      ave_bg_ratio=simple_bg_ratio;

      camera_awb_core_convert_to_grid(simple_rg_ratio_x, simple_bg_ratio_x, &ave_rg_grid, &ave_bg_grid);

      // compute the max distance between an outlier and the center of the outliers
      max_dist=0;    
      for(index=0; index<n_outlier; index++)
      {
        d1=outlier_rg_grid[index]-ave_rg_grid;
        d2=outlier_bg_grid[index]-ave_bg_grid;
        dist=d1*d1+d2*d2;
        if(dist>max_dist)
          max_dist=dist;
      }

      if(max_dist<=cam3a_agw_ptr->compact_cluster_r2)
        is_a_compact_cluster=1;  // is a compact cluster
      else
        is_a_compact_cluster=0;  // is NOT a compact cluster

      camera_awb_core_search_nearest_reference_point(ave_rg_grid, ave_bg_grid, &sample_decision, &min_dist, &dayidx);

      // too blue, too red,--> low confidence. reject this frame
      if(ave_rg_grid>(cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_HORIZON]+3) ||
        ave_bg_grid>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_CLOUDY])
      {
        MSG_CAM3ADEBUG("### too red or too blue. return", 0, 0, 0);
        return output;
      }
      MSG_CAM3ADEBUG("### all outliers : is_a_compact_cluster=%d, max_dist=%d", is_a_compact_cluster, max_dist, 0);
      MSG_CAM3ADEBUG("### all outliers : outliers RG grid=%d, BG grid=%d", ave_rg_grid, ave_bg_grid,0);

      if (camera_awb_single_color_tracking_all_outliers(input_exposure_ptr,
          n_outlier, outlier_rg_grid, outlier_bg_grid, ave_rg_grid, ave_bg_grid, max_dist)<0)
      {  //3A 1.4 change (single color tracking)
        MSG_CAM3ADEBUG("### all outliers : frame rejected for being single color", 0, 0, 0);
        return output;
      }

      /* using actual daylight line for estimating D55 */
      d55_rg=0.7*cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_rg[0]]
        + 0.3*cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_rg[cam3a_agw_ptr->n_day3-1]];
      d55_bg=0.7*cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_bg[0]]
        + 0.3*cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_bg[cam3a_agw_ptr->n_day3-1]];

      MSG_CAM3ADEBUG("D55_rg=%d, D55_bg=%d",d55_rg*1000, d55_bg*1000,0);

      if(is_a_compact_cluster==1 && min_dist<cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
      {  // all outliers, and it's a compact cluster
        // although all ouliers, but it's a compact cluster
        // not too far from reference point
        // this one is design for the situation that the test chart
        // is grey, but the lighting is not reference point.
        // the scene can not be too red or too blue  
        if((possible_green_cnt>((cam3a_agw_ptr->outdoor_green_threshold*input_stats_ptr->region_cnt)/100) 
          && possible_indoor_green_cnt==0) || possible_green_cnt>cam3a_agw_ptr->outdoor_green_upper_threshold)
        {
          if(input_exposure_ptr->current_exposure_index<input_exposure_ptr->outdoor_index)
          {    // outdoor situation
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1;  //D50
              rg_ratio=cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_rg[cam3a_agw_ptr->n_day3-1]];
              bg_ratio=cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_bg[cam3a_agw_ptr->n_day3-1]];
              MSG_CAM3ADEBUG("### all outliers, compact cluster, near ref,  D50 (0, outdoor)", 0, 0, 0);
            }
          }
          else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->outdoor_index && 
            input_exposure_ptr->current_exposure_index<input_exposure_ptr->inoutdoor_midpoint)
          {
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;  //Daylight line2
              dayline=2;
              p1=input_exposure_ptr->current_exposure_index-input_exposure_ptr->outdoor_index;
              p2=input_exposure_ptr->inoutdoor_midpoint-input_exposure_ptr->current_exposure_index;

              rg_ratio=(cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_rg[cam3a_agw_ptr->n_day3-1]]*p2+
                p1*d55_rg)/(p1+p2); ;
              bg_ratio=(cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_bg[cam3a_agw_ptr->n_day3-1]]*p2+
                p1*d55_bg)/(p1+p2);
              MSG_CAM3ADEBUG("### all outliers, compact cluster, near ref, daylight line 2, (outdoor, inout mid)", 0, 0, 0);
            }
          }
          else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->inoutdoor_midpoint && 
            input_exposure_ptr->current_exposure_index<input_exposure_ptr->indoor_index
            && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
          {
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
              rg_ratio=d55_rg;
              bg_ratio=d55_bg;
              MSG_CAM3ADEBUG("### all outliers, compact cluster, near ref,  D55 (inout mid, indoor)", 0, 0, 0);
            }
          }
          else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->indoor_index
            && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
          {  // indoor situation
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
        // when it's quite dark, and all outliers are compact cluster, the average is > D50_rg.
        // it's suspecious that it's not the true outdoor grey
              if(ave_rg_grid>=cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1])
              {
                MSG_CAM3ADEBUG("### all outliers, compact cluster, near ref,  dark, sample location suspecious, return", 0, 0, 0);
                return output;
              }
              else
              {
                sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
                rg_ratio=d55_rg;
                bg_ratio=d55_bg;
                MSG_CAM3ADEBUG("### all outliers, compact cluster, near ref,  D565 (indoor, max)", 0, 0, 0);
              }
            }
          }
        }
        else
        {  // use simple grey world, not enough green information
          rg_ratio=simple_rg_ratio;
          bg_ratio=simple_bg_ratio;
          MSG_CAM3ADEBUG("### all outliers, compact cluster, use simple grey world", 0, 0, 0);         
        }
      }   // end of the case of all outliers and compact cluster and close to reference point

      if(is_a_compact_cluster==1 && min_dist>=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
      {  // all outliers, and it's a compact cluster	   
        // this case, it could be user points the camera to a single color
        // object which has high chromacity 
        if((possible_green_cnt>((cam3a_agw_ptr->outdoor_green_threshold*input_stats_ptr->region_cnt)/100) && possible_indoor_green_cnt==0)
          || possible_green_cnt>cam3a_agw_ptr->outdoor_green_upper_threshold)
        {
          if(input_exposure_ptr->current_exposure_index<input_exposure_ptr->outdoor_index)
          {    // outdoor situation
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1;  //D50
              rg_ratio=cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_rg[cam3a_agw_ptr->n_day3-1]];
              bg_ratio=cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_bg[cam3a_agw_ptr->n_day3-1]];
              MSG_CAM3ADEBUG("### all outliers, compact cluster, far ref, D50, (0, outdoor)", 0, 0, 0);
            }
          }
          else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->outdoor_index && 
            input_exposure_ptr->current_exposure_index<input_exposure_ptr->inoutdoor_midpoint)
          {
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;  //Daylight line2
              dayline=2;
              p1=input_exposure_ptr->current_exposure_index-input_exposure_ptr->outdoor_index;
              p2=input_exposure_ptr->inoutdoor_midpoint-input_exposure_ptr->current_exposure_index;

              rg_ratio=(cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_rg[cam3a_agw_ptr->n_day3-1]]*p2+
                p1*d55_rg)/(p1+p2); ;
              bg_ratio=(cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_bg[cam3a_agw_ptr->n_day3-1]]*p2+
                p1*d55_bg)/(p1+p2);
              MSG_CAM3ADEBUG("### all outliers, compact cluster, far ref, Daylight line 2, (outdoor, inout mid)", 0, 0, 0);
            }
          }
          else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->inoutdoor_midpoint && 
            input_exposure_ptr->current_exposure_index<input_exposure_ptr->indoor_index
            && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
          {
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
              rg_ratio=d55_rg;
              bg_ratio=d55_bg;
              MSG_CAM3ADEBUG("### all outliers, compact cluster, near ref, (inout_mid, indoor),  D55", 0, 0, 0);
            }
          }
          else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->indoor_index
            && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
          {  // indoor situation
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
              rg_ratio=d55_rg;
              bg_ratio=d55_bg;
              MSG_CAM3ADEBUG("###  all outliers,compact cluster, near ref, (indoor, max),  D55", 0, 0, 0);
            }
          }
        }
        else
        { /* sometimes the green samples do not fall exactly into green zone.
         need this for dark condition */
          if(input_exposure_ptr->current_exposure_index>=((input_exposure_ptr->inoutdoor_midpoint+input_exposure_ptr->indoor_index)>>1)
            && possible_green_cnt>0 && possible_indoor_green_cnt==0 && 
            green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON] && 
            ave_rg_grid<cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1])
          {
            sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
            rg_ratio=d55_rg;
            bg_ratio=d55_bg;
            MSG_CAM3ADEBUG("### all outliers, compact cluster, far ref,  D55 (inout mid, max)", 0, 0, 0);
          }
          else
          {
            /* do not make decision */
            MSG_CAM3ADEBUG("### all outliers, campact+high chroma. return", 0, 0, 0);
            return output;
          }
        }
      }   // end of the case of all outliers and compact cluster and far away from reference point

      if(is_a_compact_cluster==0)
      {  // all outliers, and also not a compact cluster

        if(input_exposure_ptr->current_exposure_index<input_exposure_ptr->outdoor_index)
        {    // outdoor situation
          // if it's very bright. guess D50 anyway
          if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
            return output;
          else
          {
            sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1;
            rg_ratio=cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_rg[cam3a_agw_ptr->n_day3-1]];
            bg_ratio=cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_bg[cam3a_agw_ptr->n_day3-1]];
            MSG_CAM3ADEBUG("### all outliers, not campact. D50 (0, outdoor)", 0, 0, 0);    
          }
        }
        else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->outdoor_index && 
          input_exposure_ptr->current_exposure_index<input_exposure_ptr->inoutdoor_midpoint)
        {
          if((possible_green_cnt>((cam3a_agw_ptr->outdoor_green_threshold*input_stats_ptr->region_cnt)/100) && 
            possible_indoor_green_cnt==0) ||
            possible_green_cnt>((cam3a_agw_ptr->outdoor_green_upper_threshold*input_stats_ptr->region_cnt)/100))
          {
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;  //Daylight line2
              dayline=2;
              p1=input_exposure_ptr->current_exposure_index-input_exposure_ptr->outdoor_midpoint;
              p2=input_exposure_ptr->inoutdoor_midpoint-input_exposure_ptr->current_exposure_index;

              rg_ratio=(cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_rg[cam3a_agw_ptr->n_day3-1]]*p2+
                p1*d55_rg)/(p1+p2); ;
              bg_ratio=(cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_bg[cam3a_agw_ptr->n_day3-1]]*p2+
                p1*d55_bg)/(p1+p2); 
              MSG_CAM3ADEBUG("### all outliers, not campact. Daylight line 2, (outdoor, inout mid)", 0, 0, 0);  
            }
          }
          else
          {
            MSG_CAM3ADEBUG("### all outliers, not campact. not enough green. return", 0, 0, 0);
            return output;
          }
        }
        else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->inoutdoor_midpoint && 
          input_exposure_ptr->current_exposure_index<input_exposure_ptr->indoor_index
          && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
        {
          if((possible_green_cnt>((cam3a_agw_ptr->outdoor_green_threshold*input_stats_ptr->region_cnt)/100) && 
            possible_indoor_green_cnt==0) ||
            possible_green_cnt>((cam3a_agw_ptr->outdoor_green_upper_threshold*input_stats_ptr->region_cnt)/100))
          {
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
              rg_ratio=d55_rg;
              bg_ratio=d55_bg;
              MSG_CAM3ADEBUG("### all outliers, compact cluster, near ref, (inout mid, indoor),  D55", 0, 0, 0);
            }
          }
          else
          {
            MSG_CAM3ADEBUG("### all outliers, not campact. not enough green. return", 0, 0, 0);
            return output;
          }
        }
        else if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->indoor_index
          && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
        {  // indoor situation
          if((possible_green_cnt>((cam3a_agw_ptr->outdoor_green_threshold*input_stats_ptr->region_cnt)/100) && 
            possible_indoor_green_cnt==0) ||
            possible_green_cnt>((cam3a_agw_ptr->outdoor_green_upper_threshold*input_stats_ptr->region_cnt)/100))
          {
            if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
              return output;
            else
            {
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
              rg_ratio=d55_rg;
              bg_ratio=d55_bg;
              MSG_CAM3ADEBUG("### all outliers, compact cluster, near ref, (indoor, max),  D55", 0, 0, 0);
            }
          }
          else
          {
            MSG_CAM3ADEBUG("### all outliers, not campact. not enough green. return", 0, 0, 0);
            return output;
          }
        }
        else
        {
          MSG_CAM3ADEBUG("### all outliers, not campact. should not reach here. return", 0, 0, 0);
          return output;
        }
      }   // end of the case of all outliers and not compact cluster

      MSG_CAM3ADEBUG("### AWB all outliers, estimated rg_ratio=%d, bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
    // check the history to see if there is any better (more reliable decision) 
    // with valid near grey samples
      if (sample_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
        sample_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 ||
        sample_decision==CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
        sample_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
		sample_decision==CAM3A_AGW_AWB_OUTDOOR_NOON  )
      {   // all outlier with history tracking, 3A 1.4 change
        /*lint -restore*/
        MSG_CAM3ADEBUG("### AWB all outliers: change rg_ratio=%d bg_ratio=%d by history (daylight cnt=%d", 
          rg_ratio*1000, bg_ratio*1000,p1);
        MSG_CAM3ADEBUG("--- all outliers: update history with rg=%d, bg=%d, decision=%d",1000*rg_ratio, 1000*bg_ratio, sample_decision);
        camera_awb_core_update_history(rg_ratio, bg_ratio, sample_decision, input_exposure_ptr->current_exposure_index, (int8)cam3a_awb_state.white_stat_on);
        camera_awb_core_adjust_rg_bg_by_history(&rg_ratio, &bg_ratio, &sample_decision, input_exposure_ptr);
        MSG_CAM3ADEBUG("--- all outliers: after history adjustment: rg=%d, bg=%d", rg_ratio*1000, bg_ratio*1000,0);

      }
      if(rg_ratio == 0.0)
        rg_ratio = 1.0;
      if(bg_ratio == 0.0)
        bg_ratio = 1.0;

      g_gainf=1.0;
      /*lint -save -e414*/
      r_gainf=1.0/rg_ratio;
      b_gainf=1.0/bg_ratio;
      /*lint -restore*/

      MSG_CAM3ADEBUG("### AWB all outliers : flg_all_outliers = 1 : r_gainf = %d , g_gainf = %d , b_gainf = %d ", 
        r_gainf*1000, g_gainf*1000, b_gainf*1000);

      flg_all_outliers = 1;  // to skip the following if case

      cam3a_awb_state.regular_decision = sample_decision;
      cam3a_awb_state.regular_ave_rg_ratio = rg_ratio;
      cam3a_awb_state.regular_ave_bg_ratio = bg_ratio;
    }   // end of handling the case where all samples are outliers

    if(flg_all_outliers!=1)
    {  // there are valid samples
      /*-------------------------------------------------------------------------
       compute the grid coordinate of the average R/G and B/G ratio
      -------------------------------------------------------------------------*/
      // find the R/G grid index
      camera_awb_core_convert_to_grid(ave_rg_ratio_x, ave_bg_ratio_x, &ave_rg_grid, &ave_bg_grid);

      MSG_CAM3ADEBUG("### Before adjustment. ave_rg_ratio=%d  ave_bg_ratio=%d", ave_rg_ratio*1000, ave_bg_ratio*1000, 0);

      // compute the max distance between sample center and samples
      max_dist=0;
      for(index=0; index<sample_cnt; index++)
      {
        d1=cam3a_agw_ptr->sample_rg_grid[index]-ave_rg_grid;
        d2=cam3a_agw_ptr->sample_bg_grid[index]-ave_bg_grid;
        dist=d1*d1+d2*d2;
        if(dist>max_dist)
          max_dist=dist;
      }
      for(index=0; index<n_outlier; index++)
      {
        d1=outlier_rg_grid[index]-ave_rg_grid;
        d2=outlier_bg_grid[index]-ave_bg_grid;
        dist=d1*d1+d2*d2;
        if(dist>max_dist)
          max_dist=dist;
      }

      if(max_dist<=cam3a_agw_ptr->compact_cluster_r2)
        is_a_compact_cluster=1;
      else
        is_a_compact_cluster=0;

      MSG_CAM3ADEBUG("sample distribution r2=%d",max_dist, 0,0);

      //------------------------------------------------------------------------
      // figure out which reference point is the average R/G, B/G closest to
      //-------------------------------------------------------------------------
      min_dist=0x3fffffff;

      /* Search all lighting conditions */
      for(index=0;index<CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT;index++)
      {  // fixed reference points
        d1=(ave_rg_grid-cam3a_agw_ptr->rg_grid[index]);
        d2=(ave_bg_grid-cam3a_agw_ptr->bg_grid[index]);
        dist=d1*d1+d2*d2;

        if(dist<min_dist)
        {
          sample_decision=(cam3a_auto_white_balance_illuminant_type)index;   // set lighting condition
          min_dist=dist;      // update the new minimum distance
          if(sample_decision== CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
            sample_decision== CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
            sample_decision== CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO &&
            (input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY ||
            input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH))
          {
            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            {
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {
              rg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)*
                cam3a_agw_ptr->red_gain_table[sample_decision] +
                (cam3a_agw_ptr->indoor_sample_influence)*ave_rg_ratio);
              bg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)*
                cam3a_agw_ptr->blue_gain_table[sample_decision] +
                (cam3a_agw_ptr->indoor_sample_influence)*ave_bg_ratio);
            }
            MSG_CAM3ADEBUG("### F light. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0); 
            if(sample_decision== CAM3A_AGW_AWB_INDOOR_COLD_FLO)
              f_idx=0;
            if(sample_decision== CAM3A_AGW_AWB_INDOOR_WARM_FLO)
              f_idx=cam3a_agw_ptr->n_fline-1;
          }
          else if(sample_decision== CAM3A_AGW_AWB_INDOOR_INCANDESCENT &&
            (input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY ||
            input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH))
          {   // pull to NW corner to have warm appearance
            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            {   // compact cluster, may be just grey chart
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {   // not a compact cluster, make it appear warm
              rg_ratio=CAM3A_MIN(ave_rg_ratio,cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]);
              bg_ratio=CAM3A_MAX(ave_bg_ratio,cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]);
            }
            MSG_CAM3ADEBUG("### A light. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000,0);
            a_idx=0;
            aline=2;
          }
          else if(sample_decision== CAM3A_AGW_AWB_HORIZON &&
            (input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY ||
            input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH))
          {
            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            {   // likely to be grey chart test. must show grey
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {   // likely not to be grey chart, pull to incandescent to show warm appearance
              rg_ratio=CAM3A_MIN(ave_rg_ratio,cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]);
              bg_ratio=CAM3A_MAX(ave_bg_ratio,cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]);
      //	rg_ratio=CAM3A_MIN(ave_rg_ratio,cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_HORIZON]);
            //    bg_ratio=CAM3A_MAX(ave_bg_ratio,cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_HORIZON]);
            }
            MSG_CAM3ADEBUG("### Horizon. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
            a_idx=cam3a_agw_ptr->n_aline2-1;
            aline=2;
          }
          else
          {   // outdoor 
            if(input_exposure_ptr->search_mode==AWB_AGW_OUTDOOR_ONLY ||
              input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH)
            {
              if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
              {   // likely to be grey chart test. must show grey
                rg_ratio=simple_rg_ratio;
                bg_ratio=simple_bg_ratio;
              }
              else
              {
         // try to move the wb point to the south east corner of the sample average and
          // nearest reference point. Hopefully the result is less red and more blue
          // for outdoor condition
                rg_ratio=CAM3A_MAX(ave_rg_ratio, cam3a_agw_ptr->red_gain_table[sample_decision]);
                bg_ratio=CAM3A_MIN(ave_bg_ratio, cam3a_agw_ptr->blue_gain_table[sample_decision]);
              }
              MSG_CAM3ADEBUG("### day light. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
            }
          }
        }
        else if(dist==min_dist)
        {
          if(index==(int)CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
            index==(int)CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
            index==(int)CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
          {
            if(index==(int)CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
            {
              if(sample_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
                 sample_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO )
              {   // custom fluorescent is preferred over CW and TL
                sample_decision=(cam3a_auto_white_balance_illuminant_type)index;
                if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
                {   // likely to be grey chart test. must show grey
                  rg_ratio=simple_rg_ratio;
                  bg_ratio=simple_bg_ratio;
                }
                else
                {
                  rg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)*
                    cam3a_agw_ptr->red_gain_table[sample_decision] +
                    (cam3a_agw_ptr->indoor_sample_influence)*ave_rg_ratio);
                  rg_ratio=CAM3A_MAX(ave_rg_ratio, cam3a_agw_ptr->red_gain_table[sample_decision]);
                  bg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)*
                    cam3a_agw_ptr->blue_gain_table[sample_decision] +
                    (cam3a_agw_ptr->indoor_sample_influence)*ave_bg_ratio);
               }
               MSG_CAM3ADEBUG("### Custom F light. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
              }
            }


            if(sample_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
              sample_decision== CAM3A_AGW_AWB_HORIZON)
            {   // fluorescent is preferred over A and H
              sample_decision=(cam3a_auto_white_balance_illuminant_type)index;
              if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
              {   // likely to be grey chart test. must show grey
                rg_ratio=simple_rg_ratio;
                bg_ratio=simple_bg_ratio;
              }
              else
              {
                rg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)*
                  cam3a_agw_ptr->red_gain_table[sample_decision] +
                  (cam3a_agw_ptr->indoor_sample_influence)*ave_rg_ratio);
                bg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)*
                  cam3a_agw_ptr->blue_gain_table[sample_decision] +
                  (cam3a_agw_ptr->indoor_sample_influence)*ave_bg_ratio);
              }
              MSG_CAM3ADEBUG("### F light. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
            }
          }

          if(index==(int)CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 && 
            input_exposure_ptr->current_exposure_index >= input_exposure_ptr->inoutdoor_midpoint)
          {
            if( sample_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
              sample_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
              sample_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
            {
              sample_decision=(cam3a_auto_white_balance_illuminant_type)index;
              if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
              {   // likely to be grey chart test. must show grey
                rg_ratio=simple_rg_ratio;
                bg_ratio=simple_bg_ratio;
              }
              else
              {
            // try to move the wb point to the south east corner of the sample average and
            // nearest reference point. Hopefully the result is less red and more blue
                rg_ratio=CAM3A_MAX(ave_rg_ratio, cam3a_agw_ptr->red_gain_table[sample_decision]);
                bg_ratio=CAM3A_MIN(ave_bg_ratio, cam3a_agw_ptr->blue_gain_table[sample_decision]);
              }
              MSG_CAM3ADEBUG("### D50. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
            }
          }
        }
      }//end for

      if(input_exposure_ptr->search_mode==AWB_AGW_OUTDOOR_ONLY ||
        input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH)
      {
        for(index=1;(index<cam3a_agw_ptr->n_day1-1) && index<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
        {   // examine the daylight line 1
          d1=(ave_rg_grid-cam3a_agw_ptr->day_line_1[index][0]);
          d2=(ave_bg_grid-cam3a_agw_ptr->day_line_1[index][1]);
          dist=d1*d1+d2*d2;

          if(dist<min_dist)
          {
            min_dist=dist;   // update the new minimum distance
            sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;

            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            {   // likely to be grey chart test. must show grey
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {
         // try to move the wb point to the south east corner of the sample average and
         // nearest reference point. Hopefully the result is less red and more blue
              rg_ratio=CAM3A_MAX(ave_rg_ratio, cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day_line_1[index][0]]);
              bg_ratio=CAM3A_MIN(ave_bg_ratio, cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day_line_1[index][1]]);
            }
            dayline=1;
            dayidx=index;
            MSG_CAM3ADEBUG("### day line 1. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
          }
        }
        for(index=1;(index<cam3a_agw_ptr->n_day2-1) && index<CAM3A_AGW_AWB_NUMBER_GRID_POINT;index++)
        {   // examine the daylight line 2
          d1=(ave_rg_grid-cam3a_agw_ptr->day_line_2[index][0]);
          d2=(ave_bg_grid-cam3a_agw_ptr->day_line_2[index][1]);
          dist=d1*d1+d2*d2;

          if(dist<min_dist)
          {
            min_dist=dist;   // update the new minimum distance
            sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            {   // likely to be grey chart test. must show grey
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {
        // try to move the wb point to the south east corner of the sample average and
         // nearest reference point. Hopefully the result is less red and more blue
              rg_ratio=CAM3A_MAX(ave_rg_ratio, cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day_line_2[index][0]]);
              bg_ratio=CAM3A_MIN(ave_bg_ratio, cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day_line_2[index][1]]);
            }
            dayline=2;
            dayidx=index;
            MSG_CAM3ADEBUG("### day line 2. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
          }//end if
        }//end for

        /*******************************************************************************/          
        for(index=0;index<cam3a_agw_ptr->n_day3;index++)
        {   // examine the daylight line 2
          d1=(ave_rg_grid-cam3a_agw_ptr->day3_rg[index]);
          d2=(ave_bg_grid-cam3a_agw_ptr->day3_bg[index]);
          dist=d1*d1+d2*d2;

          if(dist<min_dist)
          {
            min_dist=dist;   // update the new minimum distance
            sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            {   // likely to be grey chart test. must show grey
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {
        /*
              rg_ratio = (float)((1.0 - cam3a_agw_ptr->outdoor_sample_influence)*
                cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day_line_2[index][0]] +
                (cam3a_agw_ptr->outdoor_sample_influence)*ave_rg_ratio);
              bg_ratio = (float)((1.0 - cam3a_agw_ptr->outdoor_sample_influence)*
                cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day_line_2[index][1]]+
                (cam3a_agw_ptr->outdoor_sample_influence)*ave_bg_ratio);  */

        // try to move the wb point to the south east corner of the sample average and
         // nearest reference point. Hopefully the result is less red and more blue
              rg_ratio=CAM3A_MAX(ave_rg_ratio, cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_rg[index]]);
              bg_ratio=CAM3A_MIN(ave_bg_ratio, cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->day3_bg[index]]);
            }
            dayline=2;  // will be modified
            dayidx=index;
            MSG_CAM3ADEBUG("### day line 3. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
          }//end if
        }//end for
        /*******************************************************************************/          
      }  // end if

      if(cam3a_agw_ptr->white_balance_allow_fline && 
        (input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY ||
        input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH))
      {
        if(cam3a_agw_ptr->Fline[0][0]>cam3a_agw_ptr->Fline[cam3a_agw_ptr->n_fline-1][0])
          p1=1;   // from TL to CW
        else
          p1=-1;  // from CW to TL
        p2= cam3a_agw_ptr->n_fline >> 1;   // p2 is the mid point index

        for(index=1;index<cam3a_agw_ptr->n_fline-1;index++)
        {   // examine the flourescent line
          d1=(ave_rg_grid-cam3a_agw_ptr->Fline[index][0]);
          d2=(ave_bg_grid-cam3a_agw_ptr->Fline[index][1]);
          dist=d1*d1+d2*d2;

          if(dist<min_dist)
          {
            min_dist=dist;   // update the new minimum distance
            if(index<p2)
            {    // first half of the line
              if(p1>0)
              {   // TL to CW
                sample_decision=CAM3A_AGW_AWB_INDOOR_WARM_FLO; 
              }
              else
              {   // CW to TL
                sample_decision=CAM3A_AGW_AWB_INDOOR_COLD_FLO; 
              }
            }
            else
            {   // second half of the line
              if(p1>0)
              {   // TL to CW
                sample_decision=CAM3A_AGW_AWB_INDOOR_COLD_FLO; 
              }
              else
              {   // CW to TL
                sample_decision=CAM3A_AGW_AWB_INDOOR_WARM_FLO; 
              }
            }
            f_idx=index;
            // changed to use filtered average to compensate for sensor variation
            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            {   // likely to be grey chart test. must show grey
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {
              rg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)* 
                cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->Fline[index][0]] +
                (cam3a_agw_ptr->indoor_sample_influence)*ave_rg_ratio);
              bg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)*
                cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->Fline[index][1]]+
                (cam3a_agw_ptr->indoor_sample_influence)*ave_bg_ratio);
            }
            MSG_CAM3ADEBUG("### F line. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
          }//end if
        }//end for
        // 3A 1.4 change (added day_f_line)
        if(cam3a_agw_ptr->Day_F_line[0][1]>cam3a_agw_ptr->Day_F_line[cam3a_agw_ptr->n_day_f_line-1][1])
          p1=1;   // from D50 to flo
        else
          p1=-1;  // from flo to D50 
        p2= cam3a_agw_ptr->n_day_f_line >> 1;   // p2 is the mid point index

        for(index=1;index<cam3a_agw_ptr->n_day_f_line-1;index++)
        {   // examine the flourescent line
          d1=(ave_rg_grid-cam3a_agw_ptr->Day_F_line[index][0]);
          d2=(ave_bg_grid-cam3a_agw_ptr->Day_F_line[index][1]);
          dist=d1*d1+d2*d2;

          if(dist<min_dist)
          {
            min_dist=dist;   // update the new minimum distance
            if(index<p2)
            {    // first half of the line
              if(p1>0)
              {   // from D50 to flo
				if(input_exposure_ptr->current_exposure_index<=input_exposure_ptr->indoor_index)  
                  sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1; 
                else
				  sample_decision=CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO; 			
              }
              else
              {   // from flo to D50 
				  sample_decision=CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO; 
              }
            }
            else
            {   // second half of the line
              if(p1>0)
              {   // from D50 to flo
                sample_decision=CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO; 
              }
              else
              {   // from flo to D50       
				 if(input_exposure_ptr->current_exposure_index<=input_exposure_ptr->indoor_index) 
                  sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1;
                 else
                   sample_decision=CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO; 
              }
            }

            // changed to use filtered average to compensate for sensor variation
            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            {   // likely to be grey chart test. must show grey
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {
              rg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)* 
                cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->Day_F_line[index][0]] +
                (cam3a_agw_ptr->indoor_sample_influence)*ave_rg_ratio);
              bg_ratio = (float)((1.0 - cam3a_agw_ptr->indoor_sample_influence)*
                cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->Day_F_line[index][1]]+
                (cam3a_agw_ptr->indoor_sample_influence)*ave_bg_ratio);
            }
            MSG_CAM3ADEBUG("### Day_F line. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
          }//end if
        }//end for
      }

      // A line 1
      if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY ||
        input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH)
      {
        for(index=0;index<cam3a_agw_ptr->n_aline1-1;index++)
        {   // examine the flourescent line
          d1=(ave_rg_grid-cam3a_agw_ptr->Aline1[index][0]);
          d2=(ave_bg_grid-cam3a_agw_ptr->Aline1[index][1]);
          dist=d1*d1+d2*d2;

          if(dist<min_dist)
          {
            min_dist=dist;   // update the new minimum distance
            sample_decision=CAM3A_AGW_AWB_INDOOR_INCANDESCENT;   // need a new type
            // changed to use filtered average to compensate for sensor variation
            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            { // compact cluster, likely to be grey chart
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {   // not a compact cluster, make it appear warm
              rg_ratio=CAM3A_MIN(ave_rg_ratio,cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->Aline1[index][0]]);
              bg_ratio=CAM3A_MAX(ave_bg_ratio,cam3a_agw_ptr->rgbg_grid[cam3a_agw_ptr->Aline1[index][1]]);
            }
            MSG_CAM3ADEBUG("### A line1. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
          }
          aline=2;
          a_idx=0;  // set it to A
        }

        // Aline 2
        // compare the rg ratio of starting & ending point of A line 2
        if(cam3a_agw_ptr->Aline2[0][0]>cam3a_agw_ptr->Aline2[cam3a_agw_ptr->n_aline2-1][0])
          p1=1;   // from H to A
        else
          p1=-1;  // from A to H
        p2= cam3a_agw_ptr->n_aline2 >> 1;   // p2 is the mid point index

        for(index=1;index<cam3a_agw_ptr->n_aline2-1;index++)
        {   // examine the flourescent line
          d1=(ave_rg_grid-cam3a_agw_ptr->Aline2[index][0]);
          d2=(ave_bg_grid-cam3a_agw_ptr->Aline2[index][1]);
          dist=d1*d1+d2*d2;

          if(dist<min_dist)
          {
            aline=2;
            a_idx=index;  // set it to A
            min_dist=dist;   // update the new minimum distance
            if(index<p2)
            {  // first half
              if(p1>0)
              {    // H to A
                sample_decision=CAM3A_AGW_AWB_HORIZON;   
              }
              else
              {   // A to H
                sample_decision=CAM3A_AGW_AWB_INDOOR_INCANDESCENT;  
              }
            }
            else
            {   // second half of the line
              if(p1>0)
              {   // H to A
                sample_decision=CAM3A_AGW_AWB_INDOOR_INCANDESCENT;   
              }
              else
              {   // A to H
                sample_decision=CAM3A_AGW_AWB_HORIZON;   
              }
            }
            // changed to use filtered average to compensate for sensor variation
            if(is_a_compact_cluster && min_dist<=cam3a_agw_ptr->compact_cluster_to_ref_point_r2)
            { // compact cluster, likely to be grey chart
              rg_ratio=simple_rg_ratio;
              bg_ratio=simple_bg_ratio;
            }
            else
            {   // not a compact cluster, make it appear warm
              rg_ratio=CAM3A_MIN(ave_rg_ratio,cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]);
              bg_ratio=CAM3A_MAX(ave_bg_ratio,cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]);
            }
            MSG_CAM3ADEBUG("### A line2. rg_ratio=%d  bg_ratio=%d", rg_ratio*1000, bg_ratio*1000, 0);
          }
        }
      }

    //*******************************************************************************
    // modify outdoor average (rg,bg) ratio based on brightness
    // see if this can prevent the purple sky
      if(sample_decision== CAM3A_AGW_AWB_DAYLIGHT_HYBRID || sample_decision== CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT
        || sample_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 || sample_decision==CAM3A_AGW_AWB_OUTDOOR_CLOUDY
		|| sample_decision==CAM3A_AGW_AWB_OUTDOOR_NOON)
      {
        if(bg_ratio>cam3a_agw_ptr->shifted_d50_bg)
        {
          p1=input_exposure_ptr->outdoor_index;
          //p2=(input_exposure_ptr->outdoor_index+input_exposure_ptr->outdoor_midpoint+1)>>1;
          p2=input_exposure_ptr->outdoor_index>>1;
          if(input_exposure_ptr->current_exposure_index<p1 &&
            input_exposure_ptr->current_exposure_index>=p2)
          {
            w1=(float) (input_exposure_ptr->current_exposure_index-p2)/(float) (p1-p2);
            w2=1.0-w1;
            rg_ratio=CAM3A_MAX(ave_rg_ratio, w1*ave_rg_ratio + w2*cam3a_agw_ptr->shifted_d50_rg);
            bg_ratio=CAM3A_MIN(ave_bg_ratio, w1*ave_bg_ratio + w2*cam3a_agw_ptr->shifted_d50_bg);
            MSG_CAM3ADEBUG("w1=%d, w2=%d, outdoor_mid< exp_idx <outdoor_idx", w1*1000, w2*1000,0);
            MSG_CAM3ADEBUG("after adjustment: rg_ratio=%d, bg_ratio=%d", rg_ratio*1000, bg_ratio*1000,0);
          }
          else if(input_exposure_ptr->current_exposure_index<p2)
          {
            rg_ratio=CAM3A_MAX(ave_rg_ratio, cam3a_agw_ptr->shifted_d50_rg);
            bg_ratio=CAM3A_MIN(ave_bg_ratio, cam3a_agw_ptr->shifted_d50_bg);
            MSG_CAM3ADEBUG("exp_idx<outdoor_mid, after adjustment: rg_ratio=%d, bg_ratio=%d", rg_ratio*1000, bg_ratio*1000,0);
          }
        }
      }

    //*******************************************************************************

      if(cam3a_awb_state.white_stat_on == TRUE && sample_decision==CAM3A_AGW_AWB_HORIZON)
      {
        cam3a_awb_state.white_decision =CAM3A_AGW_AWB_INVALID;
        MSG_CAM3ADEBUG("### white stat, decision=Horizon, low confidence. return", 0, 0,0);
        return output;
      }
      if(cam3a_awb_state.white_stat_on == TRUE && sample_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT
        && aline==2 &&  a_idx>(cam3a_agw_ptr->n_aline2/5))
      {
        cam3a_awb_state.white_decision =CAM3A_AGW_AWB_INVALID;
        MSG_CAM3ADEBUG("### white stat, decision=A, beyond 1/5 of Aline2, low confidence. return", 0, 0,0);
        return output;
      }

      if(cam3a_awb_state.white_stat_on == TRUE)
      {
        cam3a_awb_state.white_decision = sample_decision;
        cam3a_awb_state.white_ave_rg_ratio = rg_ratio;
        cam3a_awb_state.white_ave_bg_ratio = bg_ratio;
        MSG_CAM3ADEBUG("### white stat : ave_rg_ratio = %d , ave_bg_ratio = %d, decision=%d", 
          rg_ratio*1000, bg_ratio*1000, cam3a_awb_state.white_decision);
      }
      if(cam3a_awb_state.regular_stat_on == TRUE)
      {
        cam3a_awb_state.regular_decision = sample_decision;
        cam3a_awb_state.regular_ave_rg_ratio = rg_ratio;
        cam3a_awb_state.regular_ave_bg_ratio = bg_ratio;
        MSG_CAM3ADEBUG("### reg stat : ave_rg_ratio = %d , ave_bg_ratio = %d, decision=%d", 
          rg_ratio*1000, bg_ratio*1000, cam3a_awb_state.regular_decision);
      }
      if(sample_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID)
      {
        MSG_CAM3ADEBUG("### day light line= %d, day idx=%d", dayline, dayidx ,0);
      }
      if((int)cam3a_awb_state.regular_decision<0)
      {
        MSG_CAM3ADEBUG("### regular decision =%d. return", cam3a_awb_state.regular_decision, 0,0);
        return output;
      }
      /* ------------------------------------------------------  */
      decision_changed = FALSE;

      // low confidence: not bright enough, only a tiny fraction is close to outdoor, a lot of outliers 
      // are in other lighting conditions
      // this is the case of indoor situation with small amount of outdoor objects. since
      // the major part of the scene is indoor (due to colored wall & objects, it may not have
      // valid indoor samples), it should not apply outdoor white balance gain
      if(cam3a_awb_state.regular_stat_on == TRUE && input_exposure_ptr->current_exposure_index>input_exposure_ptr->inoutdoor_midpoint 
        && (sample_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 || sample_decision == CAM3A_AGW_AWB_OUTDOOR_NOON ||
        sample_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID || sample_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT) && 
        simple_bg_ratio<cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_INDOOR_COLD_FLO]  &&
        possible_green_cnt==0 && day_cluster<((input_stats_ptr->region_cnt*cam3a_agw_ptr->outdoor_valid_sample_cnt_threshold)/100))
      {
        MSG_CAM3ADEBUG("### regular decision=%d, day_cluster=%d, may be indoor, return", cam3a_awb_state.regular_decision, day_cluster,0);
        return output;
      }

      // reject the case of compact cluster that is too red or too blue
      if(is_a_compact_cluster==1 && (ave_rg_grid>(cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_HORIZON]+3) ||
        ave_bg_grid<(cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON]-3) || 
        ave_bg_grid>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_CLOUDY]))
      {
        MSG_CAM3ADEBUG("### regular decision=%d, compact cluster, too red or blue too weak/strong. return", cam3a_awb_state.regular_decision, 0,0);
        return output;   // lock on to the previous decision.
      }

      // in some dark situation, a small number of near grey samples distort the white balance
      // although it should be near D65, but these near grey samples draw it close to D50.
      if(input_exposure_ptr->current_exposure_index>((input_exposure_ptr->inoutdoor_midpoint+input_exposure_ptr->indoor_index)>>1) 
        && sample_cnt>0)
      {   // there are some near grey samles, dark.
        if(sample_decision== CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 || sample_decision==CAM3A_AGW_AWB_OUTDOOR_NOON||
          (sample_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID && dayline==2) )
        {   // if it's daylight line 2 or D50, it's low confidence
          if(possible_green_cnt>((cam3a_agw_ptr->outdoor_green_upper_threshold*input_stats_ptr->region_cnt)/100) && is_a_compact_cluster==0 
            && day_cluster<((cam3a_agw_ptr->outdoor_valid_sample_cnt_threshold*input_stats_ptr->region_cnt)/100) )
          {   // if enough green is available, not compact cluster, valid samples in day cluster is small
            // then move it up to D65.
            MSG_CAM3ADEBUG("### dark, enough green, not campact. few samples in day cluster, decision=%d ->D65", 
              sample_decision, 0, 0);
            sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  // D65
            if(cam3a_awb_state.white_stat_on == TRUE)
            {
              cam3a_awb_state.white_decision=sample_decision;
              rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
              bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision];
              cam3a_awb_state.white_ave_rg_ratio=rg_ratio;
              cam3a_awb_state.white_ave_bg_ratio=bg_ratio;
            }
            if(cam3a_awb_state.regular_stat_on == TRUE)
            {
              cam3a_awb_state.regular_decision=sample_decision;
              rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
              bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision];
              cam3a_awb_state.regular_ave_rg_ratio=rg_ratio;
              cam3a_awb_state.regular_ave_bg_ratio=bg_ratio;    
            }
            decision_changed=TRUE;
          }
        }
      }

    // for outdoor situation, if it's very bright and a compact cluster,
    // turn them into D50
      if(sample_decision== CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT || 
        sample_decision== CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
        sample_decision== CAM3A_AGW_AWB_DAYLIGHT_HYBRID
		||sample_decision==CAM3A_AGW_AWB_OUTDOOR_NOON )
      {
        if(is_a_compact_cluster==TRUE && cam3a_awb_state.regular_stat_on == TRUE &&
          input_exposure_ptr->current_exposure_index<=input_exposure_ptr->outdoor_index)
        {
          MSG_CAM3ADEBUG("### decision changed from %d to D50, bright and compact", sample_decision, 0,0);
          sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1; // D50
          decision_changed=TRUE;
          cam3a_awb_state.regular_decision=sample_decision;
          rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
          bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision];
        }
      }

      // white statistics supposed to get a narrow distribution 
      // if the distribution is large, do not trust it.
      if(cam3a_awb_state.white_stat_on == TRUE)
      {
        p1=0;
        if(day_cluster>0)
          p1++;
        if(f_cluster>0)
          p1++;
        if(a_cluster>0)
          p1++;

        if(p1==1)
          cam3a_awb_state.white_has_single_peak=TRUE;
        else
          cam3a_awb_state.white_has_single_peak=FALSE;

        if(p1>=2)
        {
          MSG_CAM3ADEBUG("### valid, white stat has multiple clusters, (D,F,A)=(%d,%d,%d)",
            day_cluster, f_cluster, a_cluster);
          if(cam3a_awb_state.white_y_min_percent <= YMIN_HIGH_LIMIT)
            cam3a_awb_state.white_y_min_percent++;
          MSG_CAM3ADEBUG("### y_min_percentage= %d", cam3a_awb_state.white_y_min_percent,0,0);
        }
      }

      // for dark situation, if white decision is outdoor while regular decision is indoor, reject
      if(cam3a_awb_state.white_stat_on == TRUE)
      {
        if(input_exposure_ptr->current_exposure_index>=input_exposure_ptr->indoor_index)
        {
          if(cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 ||
            cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
            cam3a_awb_state.white_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
            cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
			cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_NOON )
          {
            if(cam3a_awb_state.regular_decision==CAM3A_AGW_AWB_HORIZON ||
              cam3a_awb_state.regular_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
              cam3a_awb_state.regular_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
              cam3a_awb_state.regular_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
              cam3a_awb_state.regular_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
            {
              MSG_CAM3ADEBUG("### valid, white decision=%d -->-1, reg decision=%d. low confidence return",
                cam3a_awb_state.white_decision, cam3a_awb_state.regular_decision, 0);
              cam3a_awb_state.white_decision=CAM3A_AGW_AWB_INVALID;
              return output;
            }
          }
        }
      }

      // reject the case where flourescent type with possible green for outdoor
      if(cam3a_awb_state.regular_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
        cam3a_awb_state.regular_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
        cam3a_awb_state.regular_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
      {   // this case occurs for green plants with wood chips
        // or red bush, green leaves mixed with pink/red flowers, etc.
        if(input_exposure_ptr->current_exposure_index<input_exposure_ptr->outdoor_index)
        {
          if(is_a_compact_cluster==0 && decision_changed==0)
          {   // make sure it's not MCC in strong light
            if(input_exposure_ptr->search_mode==AWB_AGW_OUTDOOR_ONLY ||
              input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH)
            {
              MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->D50 (0,outdoor)", cam3a_awb_state.regular_decision, 0, 0);
              decision_changed = TRUE;
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1;  //D50
              rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
              bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision];

              if(cam3a_awb_state.white_stat_on == TRUE)
                cam3a_awb_state.white_decision = sample_decision;
              if(cam3a_awb_state.regular_stat_on == TRUE)
                cam3a_awb_state.regular_decision = sample_decision;
            }
            else
            {
              MSG_CAM3ADEBUG("### exp<outdoor, is a compact cluster=%d, not changed ", max_dist, 0, 0);
            }
          }
          else
          {
            MSG_CAM3ADEBUG("### exp<outdoor, not compact cluster, low confidence, reject ", 0, 0, 0);
            return output;
          }
        }
        else if(input_exposure_ptr->current_exposure_index<input_exposure_ptr->inoutdoor_midpoint)
        {   // exp_index between (outdoor, inoutdoor_midpoint)
          // do interpolation to avoid sudden AWB jump across the boundary
          if(is_a_compact_cluster==0 && max_dist<cam3a_agw_ptr->r2_threshold &&
            possible_green_cnt>0 && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
          {   // make sure it's not MCC in strong light
            if(input_exposure_ptr->search_mode==AWB_AGW_OUTDOOR_ONLY ||
              input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH)
            {
              decision_changed = TRUE;
              if (camera_awb_core_is_there_daylight_in_history()==1)
              {	// use past daylight history average, 3A 1.4 change
                camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
                sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
                dayline=2;
                dayidx=0;
              }
              else
              {
                sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;  //Daylight line 2
                dayline=2;
                p1=input_exposure_ptr->current_exposure_index-input_exposure_ptr->outdoor_index;
                p2=input_exposure_ptr->inoutdoor_midpoint-input_exposure_ptr->current_exposure_index;

                rg_ratio=(cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]*p2+
                p1*cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1])/(p1+p2); 
                bg_ratio=(cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]*p2+
                p1*cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1])/(p1+p2);
                MSG_CAM3ADEBUG("### valid, not campact, decision=%d ->daylight line 2 (outdoor, inout mid), p1=%d, p2=%d", 
                cam3a_awb_state.regular_decision, p1, p2);
              }
              

              if(cam3a_awb_state.white_stat_on == TRUE)
                cam3a_awb_state.white_decision = sample_decision;
              if(cam3a_awb_state.regular_stat_on == TRUE)
                cam3a_awb_state.regular_decision = sample_decision;
            }
          }
          else
          {
            MSG_CAM3ADEBUG("### outdoor<exp<inout_mid, max_dist=%d, not changed ", max_dist, 0, 0);
          }
        }
        else if(input_exposure_ptr->current_exposure_index<=input_exposure_ptr->indoor_index)
        {   // some outdoor scene has only distribution in F cluster only.
          // it could be shady or directly illuminated by sun
          if(possible_green_cnt>((cam3a_agw_ptr->outdoor_green_threshold_bright_F*input_stats_ptr->region_cnt)/100) &&
            is_a_compact_cluster==0 && max_dist<cam3a_agw_ptr->r2_threshold && possible_indoor_green_cnt==0
            && green_bgx>((cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON]+cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT])>>1))
          {
            MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->outdoor (iout mid,indoor)", 
              cam3a_awb_state.regular_decision, 0, 0);
            decision_changed = TRUE;
            if (camera_awb_core_is_there_daylight_in_history()==1)
            {	// use past daylight history average 3A 1.4 change
              camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
              sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
              dayline=2;
              dayidx=0;
            }
            else
            {
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
              rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
              bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision]; 
            }

            if(cam3a_awb_state.white_stat_on == TRUE)
              cam3a_awb_state.white_decision = sample_decision;
            if(cam3a_awb_state.regular_stat_on == TRUE)
              cam3a_awb_state.regular_decision = sample_decision;
          }

          if(cam3a_awb_state.regular_stat_on == TRUE && !decision_changed)
          {
            if(    cam3a_awb_state.white_decision == CAM3A_AGW_AWB_INDOOR_INCANDESCENT 
                && cam3a_awb_state.white_has_single_peak==TRUE
                && cam3a_awb_state.white_ave_rg_ratio<=cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]
                && cam3a_awb_state.white_ave_bg_ratio>=cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]
                && ave_rg_ratio*1.3>cam3a_awb_state.white_ave_rg_ratio 
                && ave_bg_ratio<1.3*cam3a_awb_state.white_ave_bg_ratio)
              {   // 3A 1.4 changes (mixed lighting imrprovement)
                  // to accept white stat to replace regular stat
                  // white stat location must be warmer than A
                  // and grey stat is relative close to white stat
                sample_decision = cam3a_awb_state.white_decision;
                cam3a_awb_state.regular_decision = cam3a_awb_state.white_decision;
                rg_ratio = cam3a_awb_state.white_ave_rg_ratio;
                bg_ratio = cam3a_awb_state.white_ave_bg_ratio;
                decision_changed = TRUE;
                MSG_CAM3ADEBUG("### reglar decision=%d -->changed to white decision=%d", sample_decision, cam3a_awb_state.white_decision, 0);
                MSG_CAM3ADEBUG("### adjust rg=%d, bg=%d (intdoor, max).",rg_ratio*1000 , bg_ratio*1000,0);         
              
              }

            if(cam3a_awb_state.white_decision == CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
              cam3a_awb_state.white_decision == CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
              cam3a_awb_state.white_decision == CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
            {
              MSG_CAM3ADEBUG("### reglar decision=%d -->changed to white decision=%d (inout mid, indoor)", 
                sample_decision, cam3a_awb_state.white_decision, 0);
              MSG_CAM3ADEBUG("### white rg=%d, bg=%d", cam3a_awb_state.white_ave_rg_ratio*1000 , cam3a_awb_state.white_ave_bg_ratio*1000,0);          
              rg_ratio = cam3a_awb_state.white_ave_rg_ratio;
              bg_ratio = cam3a_awb_state.white_ave_bg_ratio;
              sample_decision = cam3a_awb_state.white_decision;
              cam3a_awb_state.regular_decision = cam3a_awb_state.white_decision;
              decision_changed = TRUE;
            }
          }

          if(possible_green_cnt>0 && is_a_compact_cluster==0 && !decision_changed
            &&camera_awb_core_is_there_daylight_in_history()==1
            && green_bgx>((cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON]+cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT])>>1))
            {  // 3A 1.4 change (daylight-type fluroscent handling
            if(cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 || 
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
			  cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_NOON )
            {
              MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->outdoor (inoutmid, indoor)", cam3a_awb_state.regular_decision, 0, 0);
              decision_changed = TRUE;
              sample_decision=cam3a_awb_state.white_decision;
              camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
              if(cam3a_awb_state.white_stat_on == TRUE)
                cam3a_awb_state.white_decision = sample_decision;
              if(cam3a_awb_state.regular_stat_on == TRUE)
                cam3a_awb_state.regular_decision = sample_decision;
            }
          }
        }
        else
        {  // exp_index in (indoor, max)
          // less bright, needs more green samples to reject this frame
          if(possible_green_cnt>((cam3a_agw_ptr->outdoor_green_threshold_dark_F*input_stats_ptr->region_cnt)/100) 
            && is_a_compact_cluster==0  && max_dist<cam3a_agw_ptr->r2_threshold && possible_indoor_green_cnt==0
            && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT])
          {  // 3A 1.4 change (daylight-type F handling)
             // in some country, the custom flo is very close to daylight, so the indoor green objects
             // are mistaken as outdoor green. Try to prevent this.
             if(camera_awb_core_is_custom_flo_close_to_daylight()==1 && 
               (camera_awb_core_smallest_exp_index_in_history()>=input_exposure_ptr->indoor_index ||
                camera_awb_core_is_there_daylight_in_history()==1))
             {   // maintain indoor decision, no change, 3A 1.4 (daylight-type F handling)
               MSG_CAM3ADEBUG("custom flo close to daylight (indoor,max), do not change decision",0,0,0);
             }
             else
             {
              MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->D65 (indoor,max), green bgx=%d, horizon bg=%d", 
              cam3a_awb_state.regular_decision, green_bgx, cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON]);
              decision_changed = TRUE;
              if (camera_awb_core_is_there_daylight_in_history()==1)
              {  // use past daylight history average, 3A 1.4 (daylight-type F hanlding)
                camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
                sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
                dayline=2;
                dayidx=0;
              }
              else
              {
                sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
                rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
                bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision]; 
              }
              if(cam3a_awb_state.white_stat_on == TRUE)
                cam3a_awb_state.white_decision = sample_decision;
              if(cam3a_awb_state.regular_stat_on == TRUE)
                cam3a_awb_state.regular_decision = sample_decision;
				
            }
          }

             if(possible_green_cnt>((cam3a_agw_ptr->outdoor_green_upper_threshold*input_stats_ptr->region_cnt)/100) 
               && !decision_changed && possible_indoor_green_cnt==0
               && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT])
             {
               if (camera_awb_core_is_custom_flo_close_to_daylight()==1 && 
                   (camera_awb_core_smallest_exp_index_in_history()>=input_exposure_ptr->indoor_index ||
                   camera_awb_core_is_there_daylight_in_history()==1))
               {   // 3A 1.4 (daylight-type F handling)
                 MSG_CAM3ADEBUG("custom flo close to daylight (indoor,max), do not change decision",0,0,0);
               }
               else
               {
                 MSG_CAM3ADEBUG("### green cnt > threshold,  decision=%d ->D65 (indoor,max)", cam3a_awb_state.regular_decision, 0, 0);
                 decision_changed = TRUE;
                 if (camera_awb_core_is_there_daylight_in_history()==1)
                 {	// use past daylight history average
                   camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
                   sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
                   dayline=2;
                   dayidx=0;
                 }
                 else
                 {
                   sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
                   rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
                   bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision]; 
				}
                   if(cam3a_awb_state.white_stat_on == TRUE)
                     cam3a_awb_state.white_decision = sample_decision;
                   if(cam3a_awb_state.regular_stat_on == TRUE)
                     cam3a_awb_state.regular_decision = sample_decision;
                 }
               }

             if(cam3a_awb_state.regular_stat_on == TRUE && !decision_changed)
             {
               if(cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT
                 && cam3a_awb_state.white_has_single_peak==TRUE 
                 && cam3a_awb_state.white_ave_rg_ratio<=cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]
                 && cam3a_awb_state.white_ave_bg_ratio>=cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]
                 && ave_rg_ratio*1.3>cam3a_awb_state.white_ave_rg_ratio 
                 && ave_bg_ratio<1.3*cam3a_awb_state.white_ave_bg_ratio )
               {  // 3A 1.4 changes, (daylight-type F handling)
                 sample_decision = cam3a_awb_state.white_decision;
                 cam3a_awb_state.regular_decision = cam3a_awb_state.white_decision;
                 rg_ratio = cam3a_awb_state.white_ave_rg_ratio;
                 bg_ratio = cam3a_awb_state.white_ave_bg_ratio;
                 decision_changed = TRUE;
                 MSG_CAM3ADEBUG("### reglar decision=%d -->changed to white decision=%d", sample_decision, cam3a_awb_state.white_decision, 0);
                 MSG_CAM3ADEBUG("### adjust rg=%d, bg=%d (intdoor, max).",rg_ratio*1000 , bg_ratio*1000,0);         

              }

            if(cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
            {
              MSG_CAM3ADEBUG("### reglar decision=%d -->changed to white decision=%d", sample_decision, cam3a_awb_state.white_decision, 0);
              MSG_CAM3ADEBUG("### white rg=%d, bg=%d (intdoor, max).",cam3a_awb_state.white_ave_rg_ratio*1000 , cam3a_awb_state.white_ave_bg_ratio*1000,0);         
              cam3a_awb_state.regular_decision = cam3a_awb_state.white_decision;
              rg_ratio = cam3a_awb_state.white_ave_rg_ratio;
              bg_ratio = cam3a_awb_state.white_ave_bg_ratio;
              sample_decision = cam3a_awb_state.white_decision;
              decision_changed = TRUE;
            }
          }

            if(possible_green_cnt>0 && is_a_compact_cluster==0 && decision_changed==0 
              && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_INCANDESCENT] 
              && camera_awb_core_is_there_daylight_in_history()==1 )
            {  // 3A 1.4 change (daylight-type F handling)
              if((cam3a_awb_state.white_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
                cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 ||
				cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_NOON) &&
                (cam3a_awb_state.regular_stat_on == TRUE) && 
                (cam3a_awb_state.regular_decision == CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO))
              {
                if ((day_cluster<((15*sample_cnt)/100) || f_cluster>((80*sample_cnt)/100)) &&
                  possible_green_cnt<((30*input_stats_ptr->region_cnt)/100))
                {
                  MSG_CAM3ADEBUG("### Keep F decision for  indoor office condition ", 0, 0, 0);
                } 
                else
                {
                  MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->%d (indoor,max)", 
                  cam3a_awb_state.regular_decision, cam3a_awb_state.white_decision, 0);
                  decision_changed = TRUE;
                  sample_decision=cam3a_awb_state.white_decision;
                  camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);

                  if(cam3a_awb_state.regular_stat_on == TRUE)
                    cam3a_awb_state.regular_decision = sample_decision;                        
                }
              }
              else
              {
                MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->%d (indoor,max)", 
                cam3a_awb_state.regular_decision, cam3a_awb_state.white_decision, 0);
                decision_changed = TRUE;
                sample_decision=cam3a_awb_state.white_decision;
                camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);

              if(cam3a_awb_state.white_stat_on == TRUE)
                cam3a_awb_state.white_decision = sample_decision;
              if(cam3a_awb_state.regular_stat_on == TRUE)
                cam3a_awb_state.regular_decision = sample_decision;
            }
          }
        }
      }
	  //-----------------------------------------------------------------
	  // for the case that current decision is A or H
	  //------------------------------------------------------------------
      else if(sample_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
        sample_decision==CAM3A_AGW_AWB_HORIZON)
      {
        // this case occurs for green plants with wood chips
        // or red bush, green leaves mixed with pink/red flowers, etc.
        if(input_exposure_ptr->current_exposure_index<input_exposure_ptr->outdoor_index)
        {
          if(input_exposure_ptr->search_mode==AWB_AGW_OUTDOOR_ONLY ||
            input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH)
          {
            if(is_a_compact_cluster==0 && decision_changed==0)
            {   // make sure it's not MCC in strong light
              decision_changed = TRUE;
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1;  //D50
              rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
              bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision];
              MSG_CAM3ADEBUG("### valid, not campact, decision=%d -> D50, (0, outdooor)", cam3a_awb_state.regular_decision, 0, 0);

              if(cam3a_awb_state.white_stat_on == TRUE)
                cam3a_awb_state.white_decision = sample_decision;
              if(cam3a_awb_state.regular_stat_on == TRUE)
                cam3a_awb_state.regular_decision = sample_decision;
            }
            else
            {
              MSG_CAM3ADEBUG("### not campact, decision=%d (0, outdooor), low confidence, reject", cam3a_awb_state.regular_decision, 0, 0);
              return output;
            }
          }
        }
        else if(input_exposure_ptr->current_exposure_index<input_exposure_ptr->inoutdoor_midpoint)
        {
          if(is_a_compact_cluster==0 && decision_changed==0 
            && max_dist<cam3a_agw_ptr->r2_threshold && possible_green_cnt>0)
          {   // make sure it's not MCC in strong light
            if(input_exposure_ptr->search_mode==AWB_AGW_OUTDOOR_ONLY ||
              input_exposure_ptr->search_mode==AWB_AGW_INOUTDOOR_BOTH)
            {
              if (camera_awb_core_is_there_daylight_in_history()==1)
              {	// use past daylight history average 
                // 3A 1.4 change (daylight-type F handling)
                MSG_CAM3ADEBUG("### valid, not campact, decision=%d -> hybrid with history(out, inout mid)", 
                sample_decision, 0, 0);
                camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
                sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
                dayline=2;
                dayidx=0;
              }
              else
              {
                sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;  //Daylight line 2
                dayline=2;
                p1=input_exposure_ptr->current_exposure_index-input_exposure_ptr->outdoor_index;
                p2=input_exposure_ptr->inoutdoor_midpoint-input_exposure_ptr->current_exposure_index;

                rg_ratio=(cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]*p2+
                p1*cam3a_agw_ptr->red_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1])/(p1+p2); 
                bg_ratio=(cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]*p2+
                p1*cam3a_agw_ptr->blue_gain_table[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1])/(p1+p2);
                MSG_CAM3ADEBUG("### valid, not campact, decision=%d -> daylight line 2 (out, inout mid), p1=%d, p2=%d", 
                cam3a_awb_state.regular_decision, p1, p2);
              }

              decision_changed = TRUE;
              if(cam3a_awb_state.white_stat_on == TRUE)
                cam3a_awb_state.white_decision = sample_decision;
              if(cam3a_awb_state.regular_stat_on == TRUE)
                cam3a_awb_state.regular_decision = sample_decision;
            }
          }
          else
          {
            MSG_CAM3ADEBUG("### valid, not campact, decision=%d, low confidence, reject (out, inout mid)", sample_decision, 0, 0);
            return output;
          }
        }
        else if(input_exposure_ptr->current_exposure_index<input_exposure_ptr->indoor_index)
        {   // some outdoor scene has only distribution in A cluster only.
          // it could be shady or directly illuminated by sun
          if(possible_green_cnt>((cam3a_agw_ptr->outdoor_green_threshold_bright_A*input_stats_ptr->region_cnt)/100) &&
            is_a_compact_cluster==0 && max_dist<cam3a_agw_ptr->r2_threshold && decision_changed==0 
            && possible_indoor_green_cnt==0 && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
          {
            if (camera_awb_core_is_there_daylight_in_history()==1)
            { // use past daylight history average 
              // 3A 1.4 (daylight-type F handling)
              MSG_CAM3ADEBUG("### valid, not campact, decision=%d -> hybrid with history(inout mid, in)", 
              sample_decision, 0, 0);
              camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
              sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
              dayline=2;
              dayidx=0;
            }
            else
            {
              MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->D65 (inout_mid,indoor)", cam3a_awb_state.regular_decision, 0, 0);
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
              rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
              bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision]; 
            }
            decision_changed = TRUE;
            if(cam3a_awb_state.white_stat_on == TRUE)
              cam3a_awb_state.white_decision = sample_decision;
            if(cam3a_awb_state.regular_stat_on == TRUE)
              cam3a_awb_state.regular_decision = sample_decision;
          }

          if(cam3a_awb_state.regular_stat_on == TRUE && !decision_changed)
          {
            if((cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_HORIZON)
              && cam3a_awb_state.white_has_single_peak==TRUE)
            {
              MSG_CAM3ADEBUG("### reglar decision=%d -->changed to white decision=%d", sample_decision, cam3a_awb_state.white_decision, 0);
              MSG_CAM3ADEBUG("### white rg=%d, bg=%d (inout mid, indoor)",cam3a_awb_state.white_ave_rg_ratio*1000 , cam3a_awb_state.white_ave_bg_ratio*1000,0);         
              rg_ratio = cam3a_awb_state.white_ave_rg_ratio;
              bg_ratio = cam3a_awb_state.white_ave_bg_ratio;
              sample_decision = cam3a_awb_state.white_decision;
              cam3a_awb_state.regular_decision = cam3a_awb_state.white_decision;
              decision_changed = TRUE;
            }

            if(cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
            {
              MSG_CAM3ADEBUG("### reglar decision=%d -->changed to white decision=%d", sample_decision, cam3a_awb_state.white_decision, 0);
              MSG_CAM3ADEBUG("### white rg=%d, bg=%d (inout mid, indoor)",cam3a_awb_state.white_ave_rg_ratio*1000 , cam3a_awb_state.white_ave_bg_ratio*1000,0);         
              rg_ratio = cam3a_awb_state.white_ave_rg_ratio;
              bg_ratio = cam3a_awb_state.white_ave_bg_ratio;
              sample_decision = cam3a_awb_state.white_decision;
              cam3a_awb_state.regular_decision = cam3a_awb_state.white_decision;
              decision_changed = TRUE;
            }

          if(possible_green_cnt>0 && is_a_compact_cluster==0 && decision_changed==0
            && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
          {
            if(cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 || 
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
			  cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_NOON )
              {   // 3A 1.4 change (daylight-type F handling)
                if (camera_awb_core_is_there_daylight_in_history()==1)
                {	// use past daylight history average
                  MSG_CAM3ADEBUG("### valid, not campact, decision=%d -> hybrid with history(intout mid, in)", 
                  sample_decision, 0, 0);
                  camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
                  sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
                  dayline=2;
                  dayidx=0;
                }
                else
                {
                  MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->D65 (indout mid,indoor)", cam3a_awb_state.regular_decision, 0, 0);
                  sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
                  rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
                  bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision]; 
                }
                decision_changed = TRUE;
                if(cam3a_awb_state.white_stat_on == TRUE)
                  cam3a_awb_state.white_decision = sample_decision;
                if(cam3a_awb_state.regular_stat_on == TRUE)
                  cam3a_awb_state.regular_decision = sample_decision;
            }
          }
          } //end of if(cam3a_awb_state.regular_stat_on == TRUE && !decision_changed)
        }
        else
        {
          // less bright, needs more green samples to reject this frame
          if(possible_green_cnt>((cam3a_agw_ptr->outdoor_green_threshold_dark_A*input_stats_ptr->region_cnt)/100) 
            && is_a_compact_cluster==0 && max_dist<cam3a_agw_ptr->r2_threshold && decision_changed==0 
            && possible_indoor_green_cnt==0 && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
          {   // 3A 1.4 change (daylight-type F handling)
            if (camera_awb_core_is_there_daylight_in_history()==1)
            {	// use past daylight history average
              MSG_CAM3ADEBUG("### valid, not campact, decision=%d -> hybrid with history(in, max)", 
              sample_decision, 0, 0);
              camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
              sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
              dayline=2;
              dayidx=0;
            }
            else
          {
            MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->D65 (indoor,max)", cam3a_awb_state.regular_decision, 0, 0);
            sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
            rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
            bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision]; 
             }
             decision_changed = TRUE;
            if(cam3a_awb_state.white_stat_on == TRUE)
              cam3a_awb_state.white_decision = sample_decision;
            if(cam3a_awb_state.regular_stat_on == TRUE)
              cam3a_awb_state.regular_decision = sample_decision;
          }

          if(possible_green_cnt>((cam3a_agw_ptr->outdoor_green_upper_threshold*input_stats_ptr->region_cnt)/100) 
            && !decision_changed && possible_indoor_green_cnt==0
            && green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON])
          {
            if (camera_awb_core_is_there_daylight_in_history()==1)
            { // use past daylight history average
              // 3A 1.4 change (daylight-type F handling)
              MSG_CAM3ADEBUG("### valid, not campact, decision=%d -> hybrid with history(in, max)", 
              sample_decision, 0, 0);
              camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
              sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
              dayline=2;
              dayidx=0;
            }
            else
            {
              MSG_CAM3ADEBUG("### valid, not campact. decision=%d ->D65 (indoor,max)", cam3a_awb_state.regular_decision, 0, 0);
              sample_decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT;  //D65
              rg_ratio=cam3a_agw_ptr->red_gain_table[sample_decision];
              bg_ratio=cam3a_agw_ptr->blue_gain_table[sample_decision]; 
            }
            decision_changed = TRUE;
            if(cam3a_awb_state.white_stat_on == TRUE)
              cam3a_awb_state.white_decision = sample_decision;
            if(cam3a_awb_state.regular_stat_on == TRUE)
              cam3a_awb_state.regular_decision = sample_decision;
          }

          if(cam3a_awb_state.regular_stat_on == TRUE && !decision_changed)
          {
            if((cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_HORIZON)
              && cam3a_awb_state.white_has_single_peak==TRUE)
            {
              MSG_CAM3ADEBUG("### reglar decision=%d -->changed to white decision=%d", sample_decision, cam3a_awb_state.white_decision, 0);
              MSG_CAM3ADEBUG("### white rg=%d, bg=%d (indoor, max) ",cam3a_awb_state.white_ave_rg_ratio*1000 , cam3a_awb_state.white_ave_bg_ratio*1000,0);          
              rg_ratio=cam3a_awb_state.white_ave_rg_ratio;
              bg_ratio=cam3a_awb_state.white_ave_bg_ratio;
              sample_decision=cam3a_awb_state.white_decision;
              cam3a_awb_state.regular_decision=cam3a_awb_state.white_decision;
              decision_changed = TRUE;
            }

            if(cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
            {
              MSG_CAM3ADEBUG("### reglar decision=%d -->changed to white decision=%d", sample_decision, cam3a_awb_state.white_decision, 0);
              MSG_CAM3ADEBUG("### white rg=%d, bg=%d (indoor, max) ",cam3a_awb_state.white_ave_rg_ratio*1000 , cam3a_awb_state.white_ave_bg_ratio*1000,0);          
              rg_ratio = cam3a_awb_state.white_ave_rg_ratio;
              bg_ratio = cam3a_awb_state.white_ave_bg_ratio;
              sample_decision = cam3a_awb_state.white_decision;
              cam3a_awb_state.regular_decision = cam3a_awb_state.white_decision;
              decision_changed = TRUE;
            }

            if(possible_green_cnt>0 && is_a_compact_cluster==0 && !decision_changed
				&& green_bgx>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON] &&
  				camera_awb_core_is_there_daylight_in_history()==1)
            {
              if(cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
                cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
                cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 || 
                cam3a_awb_state.white_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
				cam3a_awb_state.white_decision==CAM3A_AGW_AWB_OUTDOOR_NOON)
              {
                MSG_CAM3ADEBUG("### valid, not campact, decision=%d -> hybrid with history(in, max)", 
                sample_decision, 0, 0);
                camera_awb_core_get_daylight_from_history(&rg_ratio, &bg_ratio);
                sample_decision=CAM3A_AGW_AWB_DAYLIGHT_HYBRID;
                dayline=2;
                dayidx=0;
					
                decision_changed = TRUE;
                if(cam3a_awb_state.white_stat_on == TRUE)
                  cam3a_awb_state.white_decision = sample_decision;
                if(cam3a_awb_state.regular_stat_on == TRUE)
                  cam3a_awb_state.regular_decision = sample_decision;
              }
            }
          }  // end of if(cam3a_awb_state.regular_stat_on == TRUE && !decision_changed)
        }
      }
      else
      {   // decision is outdoor daylight
        if(input_exposure_ptr->current_exposure_index>=((input_exposure_ptr->inoutdoor_midpoint+input_exposure_ptr->indoor_index)>>1))
        {  // likely indoor brightness
          // this heuristics is added to solve the problem of indoor with large blue object
          // reg decision is outdoor, but blue is less likely to be picked up by white decision.
          // therefore, white decision is used to convert it back to indoor light.
          if(cam3a_awb_state.regular_stat_on == TRUE)
          {   // original decision is not changed, still daylight type
            // the current decision is made by grey-world decision
            if(possible_green_cnt==0 && is_a_compact_cluster==0 && possible_indoor_green_cnt==0 && decision_changed==0 &&
              (cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
              cam3a_awb_state.white_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO) 
              )
            {
              MSG_CAM3ADEBUG("### reglar decision=%d -->changed to white decision=%d", sample_decision, cam3a_awb_state.white_decision, 0);
              MSG_CAM3ADEBUG("### white rg=%d, bg=%d ((inout mid+in)/2, max) ",
                cam3a_awb_state.white_ave_rg_ratio*1000 , cam3a_awb_state.white_ave_bg_ratio*1000,0);          

              rg_ratio = cam3a_awb_state.white_ave_rg_ratio;
              bg_ratio = cam3a_awb_state.white_ave_bg_ratio;
              sample_decision = cam3a_awb_state.white_decision;
              cam3a_awb_state.regular_decision=cam3a_awb_state.white_decision;
              decision_changed = TRUE;
            }
          }
        }
      }

      if(is_a_compact_cluster==0)
      {
        // reject the case that non-compact distribution that is beyond horizon
        if(ave_rg_grid>(cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_HORIZON]+(cam3a_agw_ptr->awb_agw_grid_dist_2_threshold>>1)) || 
          ave_bg_grid<(cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_HORIZON]-cam3a_agw_ptr->awb_agw_grid_dist_2_threshold))
        {
          MSG_CAM3ADEBUG("### regular decision=%d, not compact, too red or weak blue, return", 
            cam3a_awb_state.regular_decision, 0,0);
          return output;  // do not make decision
        }
      }


     // ************  section added for history update & maintainance ********************
      MSG_CAM3ADEBUG(">>> update history with rg=%d, bg=%d, decision=%d",1000*rg_ratio, 1000*bg_ratio, sample_decision);
      camera_awb_core_update_history(rg_ratio, bg_ratio, sample_decision, input_exposure_ptr->current_exposure_index, (int8)cam3a_awb_state.white_stat_on);
      camera_awb_core_adjust_rg_bg_by_history(&rg_ratio, &bg_ratio, &sample_decision, input_exposure_ptr);
      MSG_CAM3ADEBUG("### after history adjustment: rg=%d, bg=%d", rg_ratio*1000, bg_ratio*1000,0);
      if(cam3a_awb_state.regular_stat_on == TRUE)
      {
        cam3a_awb_state.regular_ave_rg_ratio=rg_ratio;
        cam3a_awb_state.regular_ave_bg_ratio=bg_ratio;
        cam3a_awb_state.regular_decision=sample_decision;
      }
      else
      {
        cam3a_awb_state.white_ave_rg_ratio=rg_ratio;
        cam3a_awb_state.white_ave_bg_ratio=bg_ratio;
        cam3a_awb_state.white_decision=sample_decision;
      }
    //*************************************************************************************

      g_gainf=1.0;
      r_gainf=1.0/rg_ratio;
      b_gainf=1.0/bg_ratio;
    } // end of if(!flg_all_outliers) 

  /* reject the decision that is contrary to the search mode */
    if(input_exposure_ptr->search_mode==AWB_AGW_INDOOR_ONLY)
    {   // suupposed to be indoor only
      if(sample_decision==CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
        sample_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
        sample_decision==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 ||
        sample_decision==CAM3A_AGW_AWB_DAYLIGHT_HYBRID||
		sample_decision==CAM3A_AGW_AWB_OUTDOOR_NOON )
      {
        return output;
      }
    }
    if(input_exposure_ptr->search_mode==AWB_AGW_OUTDOOR_ONLY)
    {   // suupposed to be OUTdoor only
      if(sample_decision==CAM3A_AGW_AWB_HORIZON ||
        sample_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
        sample_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
        sample_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
        sample_decision==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
      {
        return output;
      }
    }



  /* red and blue gain adjustment for Lens rolloff Correction */
    if((int)sample_decision<CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT)
    {
      if(decision_changed==TRUE)
      {  // decision has been changed, just use the reference point gain adj
       // without any interpolation
        output.gain_adj_r = cam3a_agw_ptr->red_gain_adj[sample_decision];
        output.gain_adj_b = cam3a_agw_ptr->blue_gain_adj[sample_decision]; 

        MSG_CAM3ADEBUG("### gain adj: r_gain_adj = %d , b_gain_adj = %d ",  
          cam3a_agw_ptr->red_gain_adj[sample_decision]*1000, 
          cam3a_agw_ptr->blue_gain_adj[sample_decision]*1000, 0);
      }
      else
      {   // decision has not been changed
        if(sample_decision==CAM3A_AGW_AWB_HORIZON 
          || sample_decision==CAM3A_AGW_AWB_INDOOR_INCANDESCENT)
        {
          if(a_idx<0)
          {
            output.gain_adj_r = cam3a_agw_ptr->red_gain_adj[sample_decision];
            output.gain_adj_b = cam3a_agw_ptr->blue_gain_adj[sample_decision]; 

            MSG_CAM3ADEBUG("### gain adj: r_gain_adj = %d , b_gain_adj = %d ",  
              cam3a_agw_ptr->red_gain_adj[sample_decision]*1000, 
              cam3a_agw_ptr->blue_gain_adj[sample_decision]*1000, 0);
          }
          else
          {
            w1=((float) cam3a_agw_ptr->n_aline2 - a_idx)/(float) cam3a_agw_ptr->n_aline2;
            w2=1.0-w1;
            r_adj=(w2*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_HORIZON] +
              w1*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]);
            b_adj=(w2*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_HORIZON] +
              w1*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_INDOOR_INCANDESCENT]);

            output.gain_adj_r = r_adj;
            output.gain_adj_b = b_adj;  

            MSG_CAM3ADEBUG("### A line 2 adj: r_gain_adj = %d , b_gain_adj = %d ",  r_adj*1000, b_adj*1000, 0);
          }
        }
        else if(sample_decision==CAM3A_AGW_AWB_INDOOR_COLD_FLO 
          || sample_decision==CAM3A_AGW_AWB_INDOOR_WARM_FLO)
        {
          if(f_idx<0)
          {
            output.gain_adj_r = cam3a_agw_ptr->red_gain_adj[sample_decision];
            output.gain_adj_b = cam3a_agw_ptr->blue_gain_adj[sample_decision]; 

            MSG_CAM3ADEBUG("### gain adj: r_gain_adj = %d , b_gain_adj = %d ",  
              cam3a_agw_ptr->red_gain_adj[sample_decision]*1000, 
              cam3a_agw_ptr->blue_gain_adj[sample_decision]*1000, 0);
          }
          else
          {
            w1=((float) cam3a_agw_ptr->n_fline - f_idx)/(float) cam3a_agw_ptr->n_fline;
            w2=1.0-w1;
            r_adj=(w2*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_INDOOR_WARM_FLO] +
              w1*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_INDOOR_COLD_FLO]);
            b_adj=(w2*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_INDOOR_WARM_FLO] +
              w1*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_INDOOR_COLD_FLO]);

            output.gain_adj_r = r_adj;
            output.gain_adj_b = b_adj;  

            MSG_CAM3ADEBUG("### F line adj: r_gain_adj = %d , b_gain_adj = %d ",  r_adj*1000, b_adj*1000, 0);
          }
        }
        else
        {
          output.gain_adj_r = cam3a_agw_ptr->red_gain_adj[sample_decision];
          output.gain_adj_b = cam3a_agw_ptr->blue_gain_adj[sample_decision]; 

          MSG_CAM3ADEBUG("### gain adj: r_gain_adj = %d , b_gain_adj = %d ",  
            cam3a_agw_ptr->red_gain_adj[sample_decision]*1000, 
            cam3a_agw_ptr->blue_gain_adj[sample_decision]*1000, 0);
        }
      }
    }
    else
    {  // daylight line
      if(dayline==1)
      {
        w1=((float) cam3a_agw_ptr->n_day1-(float) dayidx)/(float) cam3a_agw_ptr->n_day1;
        w2=1.0-w1;

        output.gain_adj_r = (w2*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT] +
          w1*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_CLOUDY]);
        output.gain_adj_b = (w2*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT] +
          w1*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_CLOUDY]);  

        MSG_CAM3ADEBUG("### day1 adj: r_gain_adj = %d , b_gain_adj = %d ",  
          (w2*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT] +
          w1*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_CLOUDY])*1000, 
          (w2*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT] +
          w1*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_CLOUDY])*1000, 0);
      }
      else if(dayline==2)
      {
        w1=((float) cam3a_agw_ptr->n_day2-(float) dayidx)/(float) cam3a_agw_ptr->n_day2;
        w2=1.0-w1;
        output.gain_adj_r = (w2*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1] +
          w1*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]);
        output.gain_adj_b = (w2*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1] +
          w1*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]);  

        MSG_CAM3ADEBUG("### day2 adj: r_gain_adj = %d , b_gain_adj = %d ",  
          (w2*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1] +
          w1*cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT])*1000, 
          (w2*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1] +
          w1*cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT])*1000, 0);
      }
      else
      { // this is a erroneous situation, should not get here
        output.gain_adj_r = cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT];
        output.gain_adj_b = cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]; 

        MSG_CAM3ADEBUG("### ERROR: r_gain_adj = %d , b_gain_adj = %d ",  
          cam3a_agw_ptr->red_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]*1000, 
          cam3a_agw_ptr->blue_gain_adj[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT]*1000, 0);
      }
    }
  } /* end of if(under_low_light == FALSE) */
  else
  {
    /* under low-light only red/blue gain_adj need to be decided */
    output.gain_adj_r = cam3a_agw_ptr->red_gain_adj[sample_decision];
    output.gain_adj_b = cam3a_agw_ptr->blue_gain_adj[sample_decision]; 
  }

// find the minimal gain and pull every color channel gain >= 1.0
  gain_min=1.0;
  if(r_gainf<=gain_min)
    gain_min=r_gainf;
  if(b_gainf<=gain_min)
    gain_min=b_gainf;

// modify the global variables
  ftmp=1.0/gain_min;
  r_gainf=r_gainf*ftmp;
  g_gainf=g_gainf*ftmp;
  b_gainf=b_gainf*ftmp;
  MSG_CAM3ADEBUG("### >>>  r_gainf = %d , g_gainf = %d ,b_gainf = %d <<< ",  
    r_gainf*1000, g_gainf*1000, b_gainf*1000);

  output.is_confident = TRUE; /* confident to use the gains and decision */
  output.sample_decision = (cam3a_auto_white_balance_illuminant_type)sample_decision;
  output.wb_gain_r = r_gainf;
  output.wb_gain_g = g_gainf;
  output.wb_gain_b = b_gainf;

  return output;
}
/*lint -restore */

/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_UPDATE_HISTORY

DESCRIPTION
  update awb history for stable preview

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
static void camera_awb_core_update_history(float rg_ratio, float bg_ratio, cam3a_auto_white_balance_illuminant_type decision,
  uint16 exp_index, int8 is_white)
{
  int i;

  if(cam3a_agw_ptr->awb_history_num<AWB_MAX_HISTORY)
  {   // new item appended to the end
    cam3a_agw_ptr->awb_history_rg_ratio[cam3a_agw_ptr->awb_history_num]=(int16) (rg_ratio*1000.0+0.5);
    cam3a_agw_ptr->awb_history_bg_ratio[cam3a_agw_ptr->awb_history_num]=(int16) (bg_ratio*1000.0+0.5);
    cam3a_agw_ptr->awb_history_decision[cam3a_agw_ptr->awb_history_num]=decision;
    cam3a_agw_ptr->awb_history_exp[cam3a_agw_ptr->awb_history_num]=(int16)exp_index;
    cam3a_agw_ptr->awb_history_replaced[cam3a_agw_ptr->awb_history_num]=0;
    cam3a_agw_ptr->awb_history_is_white[cam3a_agw_ptr->awb_history_num]=is_white;
    cam3a_agw_ptr->awb_history_num++;
  }
  else
  { // need to move items forward and append new one to the last position.
    for(i=0;i<(AWB_MAX_HISTORY-1);i++)
    {
      cam3a_agw_ptr->awb_history_rg_ratio[i]=cam3a_agw_ptr->awb_history_rg_ratio[i+1];
      cam3a_agw_ptr->awb_history_bg_ratio[i]=cam3a_agw_ptr->awb_history_bg_ratio[i+1];
      cam3a_agw_ptr->awb_history_decision[i]=cam3a_agw_ptr->awb_history_decision[i+1];
      cam3a_agw_ptr->awb_history_replaced[i]=cam3a_agw_ptr->awb_history_replaced[i+1];
      cam3a_agw_ptr->awb_history_exp[i]=cam3a_agw_ptr->awb_history_exp[i+1];
      cam3a_agw_ptr->awb_history_is_white[i]=cam3a_agw_ptr->awb_history_is_white[i+1];
    }
    cam3a_agw_ptr->awb_history_rg_ratio[AWB_MAX_HISTORY-1]=(int16) (rg_ratio*1000.0+0.5);
    cam3a_agw_ptr->awb_history_bg_ratio[AWB_MAX_HISTORY-1]=(int16) (bg_ratio*1000.0+0.5);
    cam3a_agw_ptr->awb_history_decision[AWB_MAX_HISTORY-1]=decision;
    cam3a_agw_ptr->awb_history_replaced[AWB_MAX_HISTORY-1]=0;
    cam3a_agw_ptr->awb_history_exp[AWB_MAX_HISTORY-1]=(int16)exp_index;
    cam3a_agw_ptr->awb_history_is_white[AWB_MAX_HISTORY-1]=is_white;
  }

  /* list the history for debug purpose */
#if 0
  MSG_CAM3ADEBUG("-----AWB history num = %d ", cam3a_agw_ptr->awb_history_num,0,0);
  for(i=0;i<cam3a_agw_ptr->awb_history_num;i++)
  {
    MSG_CAM3ADEBUG("-----AWB history:  rg_ratio = %d , bg_raio = %d ,decision = %d ",
      cam3a_agw_ptr->awb_history_rg_ratio[i], cam3a_agw_ptr->awb_history_bg_ratio[i], 
      cam3a_agw_ptr->awb_history_decision[i]);
    MSG_CAM3ADEBUG("-----AWB history: exp=%d, is_white=%d", cam3a_agw_ptr->awb_history_exp[i],
      cam3a_agw_ptr->awb_history_is_white[i],0);
  }
#endif
}

/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_ADJUST_RG_BG_BY_HISTORY

DESCRIPTION
  adjust awb by history for stable preview

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
/*lint -save -e550 -e818 -e702 -e734 -e736*/
static void camera_awb_core_adjust_rg_bg_by_history(float *rg_ratio, float *bg_ratio,
  cam3a_auto_white_balance_illuminant_type *decision,
  camera_exposure_info_type* input_exposure_ptr)
{
  int a_cluster=0, f_cluster=0, day_cluster=0, index;
  int sum, exp_ave, tmp;
  int32 a_rg=0, a_bg=0, f_rg=0, f_bg=0, day_rg=0, day_bg=0;
  int32 a_rg_ave, a_bg_ave, f_rg_ave, f_bg_ave, day_rg_ave, day_bg_ave;
//  int32 rg_diff, bg_diff, diff_threshold=20;
  int f_replaced_cnt=0, a_replaced_cnt=0, day_replaced_cnt=0;
  int max_exp, min_exp, range;
  int stable_range_threshold=25;  // for 3% AEC table step size
  int max_exp_delta;
  int small_range;
  int is_white_and_f;
  int is_reg_and_day;

  // if the number of history entries is small, return
  if(cam3a_agw_ptr->awb_history_num<=3 || cam3a_agw_ptr->awb_history_complete_exp_cnt<=5)
    return;

  MSG_CAM3ADEBUG("------ AWB history cnt=%d, AEC history cnt=%d",  
	  cam3a_agw_ptr->awb_history_num,cam3a_agw_ptr->awb_history_complete_exp_cnt,0 );
  
  // first, we look at the history to see how 
  // this part of the history has the data for exp, decision, rg & bg
  sum=0;
  max_exp=0;
  min_exp=999;
  is_white_and_f=0;
  is_reg_and_day=0;
  for(index=0;index<cam3a_agw_ptr->awb_history_num; index++)
  {
    sum+=cam3a_agw_ptr->awb_history_exp[index];
    if(cam3a_agw_ptr->awb_history_exp[index]>max_exp)
      max_exp=cam3a_agw_ptr->awb_history_exp[index];
    if(cam3a_agw_ptr->awb_history_exp[index]<min_exp)
      min_exp=cam3a_agw_ptr->awb_history_exp[index];

    if(cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_HORIZON ||
      cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_INCANDESCENT)
    {
      a_cluster++;
      a_rg+=cam3a_agw_ptr->awb_history_rg_ratio[index];
      a_bg+=cam3a_agw_ptr->awb_history_bg_ratio[index];
      if(cam3a_agw_ptr->awb_history_replaced[index]!=0)
        a_replaced_cnt++;
    }
    else if(cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
      cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
      cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
    {
      f_cluster++;
      f_rg+=cam3a_agw_ptr->awb_history_rg_ratio[index];
      f_bg+=cam3a_agw_ptr->awb_history_bg_ratio[index];
      if(cam3a_agw_ptr->awb_history_replaced[index]!=0)
        f_replaced_cnt++;
      if(cam3a_agw_ptr->awb_history_is_white[index]==1)
        is_white_and_f++;
    }
    else
    {
      day_cluster++;
      day_rg+=cam3a_agw_ptr->awb_history_rg_ratio[index];
      day_bg+=cam3a_agw_ptr->awb_history_bg_ratio[index];
      if(cam3a_agw_ptr->awb_history_replaced[index]!=0)
        day_replaced_cnt++;
      if(cam3a_agw_ptr->awb_history_is_white[index]==0)
        is_reg_and_day++;
    }
  }
  exp_ave=(sum+(cam3a_agw_ptr->awb_history_num>>1))/cam3a_agw_ptr->awb_history_num;
  small_range=max_exp-min_exp;  // exp range of the AWB/AEC history

  // now check AEC history (which has more items than the AWB history due to frame skip)
  max_exp=0;
  min_exp=999;
  max_exp_delta=0;
  for(index=1;index<cam3a_agw_ptr->awb_history_complete_exp_cnt; index++)
  {
    if(cam3a_agw_ptr->awb_history_complete_exp[index]>max_exp)
    {
      max_exp=cam3a_agw_ptr->awb_history_complete_exp[index];
    }
    if(cam3a_agw_ptr->awb_history_complete_exp[index]<min_exp)
    {
      min_exp=cam3a_agw_ptr->awb_history_complete_exp[index];
    }
    tmp=abs(cam3a_agw_ptr->awb_history_complete_exp[index]-cam3a_agw_ptr->awb_history_complete_exp[index-1]);
    if(tmp>max_exp_delta)
      max_exp_delta=tmp;
  }
  range=max_exp-min_exp;  // exp range of the AEC history
  
  MSG_CAM3ADEBUG("------ AWB history: A cnt=%d, F cnt=%d, Day cnt=%d",  a_cluster, f_cluster, day_cluster);
  MSG_CAM3ADEBUG("------ AWB history: complete exp range=%d (max=%d, min=%d)", range, max_exp, min_exp);
  MSG_CAM3ADEBUG("------ AWB history: max exp delta=%d, exp ave=%d, small range=%d", max_exp_delta,exp_ave,small_range);

  // the analysis of the history is done. Now start to do lock and unlock

  // LOCK/UNLOCK HEURISTIC 1:
  // if the past history is mostly daylight and it's bright,
  // reject current indoor decisions
  index= cam3a_agw_ptr->awb_history_num-1;  // point to the last entry in history
  if(max_exp<=(input_exposure_ptr->indoor_index-10) && max_exp_delta<=stable_range_threshold && 
	exp_ave<=(input_exposure_ptr->indoor_index-23) &&
	day_cluster>f_cluster && day_cluster>a_cluster)
  {
    if(day_cluster> f_cluster && day_cluster>a_cluster &&
      cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT &&
      cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 &&
      cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_OUTDOOR_CLOUDY &&
      cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_DAYLIGHT_HYBRID &&
	  cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_OUTDOOR_NOON )
    {
      if(day_cluster == 0)
        day_cluster = 1;
      day_rg_ave=(day_rg+ (day_cluster>>1))/ day_cluster;
      day_bg_ave=(day_bg+ (day_cluster>>1))/ day_cluster;
      *rg_ratio=0.001*(float) day_rg_ave;
      *bg_ratio=0.001*(float) day_bg_ave;
      *decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1; //D50;
      cam3a_agw_ptr->awb_history_num--; // remove the last history entry
      MSG_CAM3ADEBUG("------ AWB history: (LH0) indoor decision is changed to D50 (day ave) by history (exp_idx=%d)", 
        cam3a_agw_ptr->awb_history_exp[index],0,0);
      return;
    }
  }
  index= cam3a_agw_ptr->awb_history_num-1;
  if(day_cluster>f_cluster && day_cluster>a_cluster && 
    cam3a_agw_ptr->awb_history_exp[index]<input_exposure_ptr->inoutdoor_midpoint &&
    max_exp_delta<(stable_range_threshold>>1) )
  {
    if(cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT &&
      cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 &&
      cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_OUTDOOR_CLOUDY &&
      cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_DAYLIGHT_HYBRID &&
	  cam3a_agw_ptr->awb_history_decision[index]!=CAM3A_AGW_AWB_OUTDOOR_NOON)
    {
      if(day_cluster == 0)
        day_cluster = 1;
      day_rg_ave=(day_rg+ (day_cluster>>1))/ day_cluster;
      day_bg_ave=(day_bg+ (day_cluster>>1))/ day_cluster;
      *rg_ratio=0.001*(float) day_rg_ave;
      *bg_ratio=0.001*(float) day_bg_ave;
      *decision=CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1; //D50;
      cam3a_agw_ptr->awb_history_num--; // remove the last history entry
      MSG_CAM3ADEBUG("------ AWB history: (LH1) indoor decision is changed to D50 (day ave) by history (exp_idx=%d)", 
        cam3a_agw_ptr->awb_history_exp[index],0,0);
      return;
	}
  }
  

  // LOCK/UNLOCK HEURISTIC 2A:
  // if the history is mostly fluorescent, the exposure range is 0 (no change in exposure level)
  // lock the WB gains
  index=cam3a_agw_ptr->awb_history_num-1;
  if(range<=1 && small_range<=1 && f_cluster>day_cluster && f_cluster>a_cluster &&
	  exp_ave<input_exposure_ptr->max_exp_index && exp_ave>=input_exposure_ptr->indoor_index)
  {   // 3A 1.4 changes (indoor fluorescent gains lock)
      if(cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
         cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
        cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO)
      {
        if (cam3a_agw_ptr->indoor_F_WB_locked==0)
        {	  // it was not locked yet
          cam3a_agw_ptr->indoor_F_WB_locked=1;  // lock it
          if(f_cluster ==0 )
              f_cluster = 1;
          f_rg_ave=(f_rg+ (f_cluster>>1))/ f_cluster;
          f_bg_ave=(f_bg+ (f_cluster>>1))/ f_cluster;
          *rg_ratio=0.001*(float) f_rg_ave;
          *bg_ratio=0.001*(float) f_bg_ave;
          cam3a_agw_ptr->awb_history_replaced[index]=1;  
          cam3a_agw_ptr->awb_history_rg_ratio[index]=f_rg_ave;  // replace last entry of history
          cam3a_agw_ptr->awb_history_bg_ratio[index]=f_bg_ave;
          MSG_CAM3ADEBUG("------ AWB history: (LH2A) F locked (exp_idx=%d, range=%d, exp_ave=%d)", 
          cam3a_agw_ptr->awb_history_exp[index],range,exp_ave);
          
          return;
        }
        else
        {	  // it's already locked, use previous last entry
        *rg_ratio=0.001*(float) cam3a_agw_ptr->awb_history_rg_ratio[index-1];
        *bg_ratio=0.001*(float) cam3a_agw_ptr->awb_history_bg_ratio[index-1];	
        // remove current history entry
        cam3a_agw_ptr->awb_history_num--;
        MSG_CAM3ADEBUG("------ AWB history: (LH2A) F locked (exp_idx=%d, range=%d), gain locked", cam3a_agw_ptr->awb_history_exp[index],range,0);
        return;
      }
    }
    else
    {  
      // this current frame is not fluorescent, but the exposure range is 0,
      // leave it later to use preview stabilization to take care of it.
      MSG_CAM3ADEBUG("------ AWB history: (LH2A) bypass current frame",0,0,0);
    }
  }
  else
  {   // break the lock
	  cam3a_agw_ptr->indoor_F_WB_locked=0;
	  MSG_CAM3ADEBUG("------ AWB history: (LH2A)indoor_F_WB_locked reset to 0",0,0,0);
	  MSG_CAM3ADEBUG("------ range=%d, small_range=%d, exp_ave=%d",range,small_range,exp_ave);
  }



  // LOCK/UNLOCK HEURISTIC 2B: 
  // in indoor situation, a minor change of exposure should not move from F to outdoor
  // the blue wall on the hallway can cause this to happen
  index=cam3a_agw_ptr->awb_history_num-1;
  if(small_range<(stable_range_threshold>>2) && max_exp_delta<(stable_range_threshold>>2) 
    && f_cluster>a_cluster && f_cluster>day_cluster
    && cam3a_agw_ptr->awb_history_exp[index]>=input_exposure_ptr->indoor_index)
  {
    if(cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
      cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 ||
      cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
	  cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_OUTDOOR_NOON)
    {
      if(f_cluster == 0)
        f_cluster = 1;
      f_rg_ave=(f_rg+ (f_cluster>>1))/ f_cluster;
      f_bg_ave=(f_bg+ (f_cluster>>1))/ f_cluster;
      *rg_ratio=0.001*(float) f_rg_ave;
      *bg_ratio=0.001*(float) f_bg_ave;
      *decision=CAM3A_AGW_AWB_INDOOR_WARM_FLO; //TL84;

      // change the history, but exp_index is not modified.
      cam3a_agw_ptr->awb_history_replaced[index]=1;
      cam3a_agw_ptr->awb_history_rg_ratio[index]=f_rg_ave;
      cam3a_agw_ptr->awb_history_bg_ratio[index]=f_bg_ave;
      cam3a_agw_ptr->awb_history_decision[index]=CAM3A_AGW_AWB_INDOOR_WARM_FLO;

      MSG_CAM3ADEBUG("------ AWB history: (LH2)outdoor decision is changed to TL84 by history (exp_idx=%d, range=%d)", cam3a_agw_ptr->awb_history_exp[index],range,0);
      return;
    }
  }

  // LOCK/UNLOCK HEURISTIC 3:
  // if the past history is mostly flurorescent, and the exposure is quite stable,
  // reject the decision of A and HG
  index=cam3a_agw_ptr->awb_history_complete_exp_cnt-1;
  if(range<stable_range_threshold && max_exp_delta<(stable_range_threshold>>1)
	  && f_cluster>day_cluster && f_cluster>a_cluster && 
    cam3a_agw_ptr->awb_history_complete_exp[index]>=input_exposure_ptr->inoutdoor_midpoint)
  {   // F cluster is dominant
    // exposure variation range is less than a threshold
      // max exposure delta is less than a threshold
    index=cam3a_agw_ptr->awb_history_num-1;
    if(cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
      cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_HORIZON)
    {
      if(f_cluster >0)
      {
        f_rg_ave=(f_rg+ (f_cluster>>1))/ f_cluster;
        f_bg_ave=(f_bg+ (f_cluster>>1))/ f_cluster;
        *rg_ratio=0.001*(float) f_rg_ave;
        *bg_ratio=0.001*(float) f_bg_ave;
        *decision=CAM3A_AGW_AWB_INDOOR_WARM_FLO; //TL84;
        // change the history, but exp_index is not modified.
        /*cam3a_agw_ptr->awb_history_replaced[index]=1;
        cam3a_agw_ptr->awb_history_rg_ratio[index]=f_rg_ave;
        cam3a_agw_ptr->awb_history_bg_ratio[index]=f_bg_ave;
        cam3a_agw_ptr->awb_history_decision[index]=CAM3A_AGW_AWB_INDOOR_WARM_FLO; */
        cam3a_agw_ptr->awb_history_num--;
        MSG_CAM3ADEBUG("------ AWB history: (LH3) A or H decision is changed to TL84 by history (exp_idx=%d, range=%d)", cam3a_agw_ptr->awb_history_exp[index],range,0);
        return;
      }
    }
  }

  

  // the following logic is to remove sudden decision jitter
  // to increase preview stability

  // the following code is for preview stability for the decision in the same category
  index= cam3a_agw_ptr->awb_history_num-1;  // point to the last entry in the history
  if(small_range <= CAM3A_MAX(4,(stable_range_threshold>>3)) )
  { // it's considered a stable exposure history, should lock

    if(a_cluster>f_cluster && a_cluster>day_cluster)
    {  // A cluster is dominant
      if(a_cluster == 0)
        a_cluster = 1;
      a_rg_ave=(a_rg+ (a_cluster>>1))/ a_cluster;
      a_bg_ave=(a_bg+ (a_cluster>>1))/ a_cluster;
      *rg_ratio=0.001*(float) a_rg_ave;
      *bg_ratio=0.001*(float) a_bg_ave;
      MSG_CAM3ADEBUG("------ AWB history: A cluster is dominant, rg = %d, bg= %d ",  a_rg_ave, a_bg_ave,0);

      if(cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_INCANDESCENT ||
        cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_HORIZON )
      {
        cam3a_agw_ptr->awb_history_rg_ratio[index]=a_rg_ave;
        cam3a_agw_ptr->awb_history_bg_ratio[index]=a_bg_ave;
        cam3a_agw_ptr->awb_history_replaced[index]=1;
        MSG_CAM3ADEBUG("------ AWB history: (PVS)last entry replaced>> rg = %d, bg= %d ",  a_rg_ave, a_bg_ave,0);
      }
    }
    else if(f_cluster>a_cluster && f_cluster>day_cluster)
    {   // F cluster is dominant
      if(f_cluster == 0)
        f_cluster = 1;
      f_rg_ave=(f_rg+ (f_cluster>>1))/ f_cluster;
      f_bg_ave=(f_bg+ (f_cluster>>1))/ f_cluster;
      *rg_ratio=0.001*(float) f_rg_ave;
      *bg_ratio=0.001*(float) f_bg_ave;
      MSG_CAM3ADEBUG("------ AWB history: F cluster is dominant, rg = %d, bg= %d ",  f_rg_ave, f_bg_ave,0);

      if(cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_WARM_FLO ||
        cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_COLD_FLO ||
        cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO )
      {  // current decision is F
        cam3a_agw_ptr->awb_history_rg_ratio[index]=f_rg_ave;
        cam3a_agw_ptr->awb_history_bg_ratio[index]=f_bg_ave;
        cam3a_agw_ptr->awb_history_replaced[index]=1;
        MSG_CAM3ADEBUG("------ AWB history: (PVS)last entry replaced>> rg = %d, bg= %d ",  f_rg_ave, f_bg_ave,0);
      }
      else
      { // current decision is not flureoscent
        tmp=input_exposure_ptr->indoor_index;
        if(max_exp_delta<=2 && exp_ave>=tmp &&
          cam3a_agw_ptr->awb_history_exp[cam3a_agw_ptr->awb_history_num-1]>=tmp)
        {
          cam3a_agw_ptr->awb_history_rg_ratio[index]=f_rg_ave;
          cam3a_agw_ptr->awb_history_bg_ratio[index]=f_bg_ave;
          cam3a_agw_ptr->awb_history_decision[index]=CAM3A_AGW_AWB_INDOOR_WARM_FLO;
          *decision=CAM3A_AGW_AWB_INDOOR_WARM_FLO;
          cam3a_agw_ptr->awb_history_replaced[index]=1;
          MSG_CAM3ADEBUG("------ AWB history: (PVS)last entry changed, decision=%d --> TL84 ", *decision,0,0 );
        }
      }
    }
    else if(day_cluster>a_cluster && day_cluster>f_cluster)
    {   // day cluster is dominant (light box condition)
      if(day_cluster == 0)
        day_cluster = 1;
      day_rg_ave=(day_rg+ (day_cluster>>1))/ day_cluster;
      day_bg_ave=(day_bg+ (day_cluster>>1))/ day_cluster;
      *rg_ratio=0.001*(float) day_rg_ave;
      *bg_ratio=0.001*(float) day_bg_ave;
      MSG_CAM3ADEBUG("------ AWB history: Day cluster, rg = %d, bg= %d ",  day_rg_ave, day_bg_ave,0);

      if(cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
        cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 ||
        cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
        cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
		cam3a_agw_ptr->awb_history_decision[index]==CAM3A_AGW_AWB_OUTDOOR_NOON )
      {
        cam3a_agw_ptr->awb_history_rg_ratio[index]=day_rg_ave;
        cam3a_agw_ptr->awb_history_bg_ratio[index]=day_bg_ave;
        cam3a_agw_ptr->awb_history_replaced[index]=1;
        MSG_CAM3ADEBUG("------ AWB history: (PVS)last entry replaced>> rg = %d, bg= %d ",  day_rg_ave,day_bg_ave,0);
      }
    }
  }

  
  
  
}
/*lint -restore*/

/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_UPDATE_COMPLETE_AEC_HISTORY

DESCRIPTION
  update awb history for stable preview

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
void camera_awb_core_update_complete_aec_history(uint16 current_exposure_index)
{
  int i;
  if(cam3a_agw_ptr->awb_history_complete_exp_cnt<AWB_AEC_MAX_HISTORY)
  {
    cam3a_agw_ptr->awb_history_complete_exp[cam3a_agw_ptr->awb_history_complete_exp_cnt]=(int16)current_exposure_index;
    cam3a_agw_ptr->awb_history_complete_exp_cnt++;
  }
  else
  {
    cam3a_agw_ptr->awb_history_complete_exp_cnt=AWB_AEC_MAX_HISTORY;
    for(i=0;i<AWB_AEC_MAX_HISTORY-1;i++)
      cam3a_agw_ptr->awb_history_complete_exp[i]=cam3a_agw_ptr->awb_history_complete_exp[i+1];
    cam3a_agw_ptr->awb_history_complete_exp[cam3a_agw_ptr->awb_history_complete_exp_cnt-1]=(int16)current_exposure_index;
  }
#if 0
  MSG_CAM3ADEBUG ("exposure history count=%d",cam3a_agw_ptr->awb_history_complete_exp_cnt,0,0);
  for(i=0;i<cam3a_agw_ptr->awb_history_complete_exp_cnt;i++)
    MSG_CAM3ADEBUG ("Complete Exp History[%d]=%d",i,cam3a_agw_ptr->awb_history_complete_exp[i],0);
#endif
}

//=====================================================================================

int camera_awb_single_color_tracking_all_outliers(camera_exposure_info_type* input_exposure_ptr,
												  int num_outliers,
												  int *outlier_rg_grid,
												  int *outlier_bg_grid,
												  int outlier_ave_rg,
												  int outlier_ave_bg,				
												  int max_dist)
{
	int dist_threshod=60; // 60% of the max_dist
	int i,sum_rg=0, sum_bg=0, cnt=0, d1,d2, dist;
	int threshold;
	uint8 selected[256];
	int max_dist2;

	// Currently this single color tracking only applies to not so bright
	if (input_exposure_ptr->current_exposure_index<input_exposure_ptr->indoor_index
		|| cam3a_agw_ptr->awb_history_num<5)
		return 0;   // bright condition, do not apply this test
					// limited to indoor condition only at this moment
					// if there is not enough history entires, do not continue either.

	threshold= (dist_threshod * max_dist+50)/100; 
	
	MSG_CAM3ADEBUG ("max_dist=%d, threshold=%d",max_dist, threshold,0);

	for (i=0;i<256;i++)
		selected[i]=0;

	cnt=0;
	for (i=0;i<num_outliers; i++)
	{
		d1=outlier_rg_grid[i]-outlier_ave_rg;
		d2=outlier_bg_grid[i]-outlier_ave_bg;
		dist=d1*d1+d2*d2;
		if (dist<threshold)
		{
			sum_rg+=outlier_rg_grid[i];
			sum_bg+=outlier_bg_grid[i];
			cnt++;
			selected[i]=1;
		}
	}
	if (cnt==0)
		return 0;  // this single color rule does not apply, return and continue the 
	               // the regular green heuristics 



    // recalculate the mean of the remaining outlier samples
	// and also compute the max dist
	sum_rg=(sum_rg+(cnt>>1))/cnt;
	sum_bg=(sum_bg+(cnt>>1))/cnt;
	MSG_CAM3ADEBUG ("After dist thresholding, outlier ave(rg=%d, bg=%d)",sum_rg,sum_bg,0);
	MSG_CAM3ADEBUG ("After dist thresholding, outlier cnt=%d, (before=%d)",cnt,num_outliers,0);
	max_dist2=0;
	for (i=0;i<num_outliers; i++)
	{
		if (selected[i]==1)
		{
			d1=outlier_rg_grid[i]-sum_rg;
			d2=outlier_bg_grid[i]-sum_bg;
			dist=d1*d1+d2*d2;
			if (dist>max_dist2)
				max_dist2=dist;
		}
	}

	MSG_CAM3ADEBUG ("outlier max_dist (before=%d, after=%d)",max_dist, max_dist2,0);
	if (max_dist2<(2*cam3a_agw_ptr->compact_cluster_r2))
		return -1;  // very likely to be single color, reject this frame
	else
		return 0;   // continue for further analysis

}

//========================================================================================

// is custom flo close to daylight?
// return 0 for no, 1 for yes.
static int camera_awb_core_is_custom_flo_close_to_daylight(void)
{
    int d1,d2,dist;

	// if the bg_ratio of custom flo is > D50_bg, it's close to daylight
	if (cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO]>=cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1]
		|| cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO]< cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_COLD_FLO])
	{
		MSG_CAM3ADEBUG ("custom flo close to daylight",0,0,0);
		return 1;
	}

	// if custom flo is pretty close to D50 and its bg ration are higher than CW & TL84
	d1=(cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO]- cam3a_agw_ptr->rg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1]);
	d2=(cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO]- cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1]);
    dist=d1*d1+d2*d2;
	if (dist<=10 && cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO]>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_COLD_FLO]
		&& cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO]>cam3a_agw_ptr->bg_grid[CAM3A_AGW_AWB_INDOOR_WARM_FLO])
	{
		MSG_CAM3ADEBUG ("custom flo close to daylight",0,0,0);
		return 1;
	}
	else 
		return 0;
}
//=========================================================================================

// search the smallest exposure index in AEC history
static int camera_awb_core_smallest_exp_index_in_history(void)
{
  int i, min_index=9999;
  if (cam3a_agw_ptr->awb_history_complete_exp_cnt==0)
	  return 9999;

  for (i=0;i<cam3a_agw_ptr->awb_history_complete_exp_cnt;i++)
  {
	  if (cam3a_agw_ptr->awb_history_complete_exp[i]<min_index)
		  min_index=cam3a_agw_ptr->awb_history_complete_exp[i];
  }
  return min_index;
}
//============================================================================================

// search if there is any outdoor decision in the history
static int camera_awb_core_is_there_daylight_in_history(void)
{
	int i, found=0;
	for (i=0;i<cam3a_agw_ptr->awb_history_num;i++)
	{
		if (cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
			cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 ||
			cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
			cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
			cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_OUTDOOR_NOON)
		{
			MSG_CAM3ADEBUG ("found daylight in history",0,0,0);
			found=1;
			break;
		}
	}
	return found;
}
//==============================================================================================

static int camera_awb_core_get_daylight_from_history(float *rg_ratio, float *bg_ratio)
{
	int i;
	int sum_rg=0, sum_bg=0, cnt=0;

	for (i=0;i<cam3a_agw_ptr->awb_history_num;i++)
	{
		if (cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT ||
			cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1 ||
			cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_OUTDOOR_CLOUDY ||
			cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_DAYLIGHT_HYBRID ||
			cam3a_agw_ptr->awb_history_decision[i]==CAM3A_AGW_AWB_OUTDOOR_NOON)
		{
			sum_rg+=cam3a_agw_ptr->awb_history_rg_ratio[i];
			sum_bg+=cam3a_agw_ptr->awb_history_bg_ratio[i];
			cnt++;
			break;
		}
	}
	if (cnt==0)
		return 0;
	else
	{
		sum_rg=(sum_rg+(cnt>>1))/cnt;
		sum_bg=(sum_bg+(cnt>>1))/cnt;
		*rg_ratio=0.001*(float) sum_rg;
		*bg_ratio=0.001*(float) sum_bg;
		MSG_CAM3ADEBUG ("daylight history: rg=%d, bg=%d, cnt=%d",sum_rg,sum_bg,cnt);
		return cnt;
	}
}
//==============================================================================================



#endif /* FEATURE_CAMERA_YCBCR_ONLY */

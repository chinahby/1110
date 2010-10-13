/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            RR Profiling Unit

GENERAL DESCRIPTION
  This is the RF profiling unit which provides services to time profile the
  RF driver code for such actions as RF warmup (WU) and tagging areas of 
  concern for current consumption.
  
EXTERNALIZED FUNCTIONS
  None.  All functions are used internally by the RF module.   Any external
  references will be through the RFM interface layer.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  1) rf_prof strobe functions require that rf_prof_toggle_strobe_init() be 
     called first to establish the GPIO configuration.
     
  2) rf_prof timing functions are dependent on the timer services to be 
     initialized.
  
Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
 

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_prof.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
12/19/05   dyc     Lint fix for ptr check.
09/26/05   dyc     Additional featurization for RF_TIME_PROFILING
08/20/05   dyc     Initial Release.

===========================================================================*/

#include "comdef.h"
#include "err.h"
#include "msm.h"
#include "msm_drv.h"
#include "timetick.h"

#include "rflib_share.h"
#include "rfi.h"
#include "rficap.h"
#include "rf_prof.h"


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                      RF PROFILE CONSTANTS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                      SHARED DATA

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*
*
* Two sets of timing strobes are used.  SURF supports use of SW_DEBUG_RESGISTER
* port and GPIO lines.  The FFA can only support GPIO strobes.
*
* Time tags can be enabled by setting the following memory locations.
* All will use bit 0 of the time port register.  Bit will be 1 at
* the start of function, 0 at the end of the function.  Only one function
* should be set at a time, however multiple settings can be done if there
* is no function overlay.  Instrument additional functions as required.
*
* Attaching scope line to the following header can be used for meassuring.
*
* The TIMEPORT SW DEBUG header for bit 0:
*    Jaguar - J8-19
*    Cougar/Leopard - J6-19
*    LEO - J8-18
*
* GPIO test header
*    Jaguar - GPIO 77 @ J27-6  (used by SRCH)
*             GPIO 89 @ J39-10
* 
*  For FFA and SURF - use GPIO for strobing 
*  On Jaguar, GPIOs 77 and 89 are currently unused.
*/

#ifdef RF_TIME_PROFILING
#error code not present
#endif /* RF_TIME_PROFILING */

/* RF WU statistics structure - to be referenced by rf_chain_status */
rf_prof_struct_type rf_wu_stats[RF_PATH_MAX] = 
{
    0,0,
    0,0
};

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                      LOCAL DATA

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/* Intermediate storage for calculated warmup value */
static rf_prof_calc_type rf_wu_calc[RF_PATH_MAX] =
{
  0,0,0,TRUE,
  0,0,0,TRUE
};

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                      LOCAL MACROS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#ifdef RF_TIME_PROFILING
#error code not present
#endif /* RF_TIME_PROFILING */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                      FUNCTIONS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*===========================================================================

FUNCTION RF_PROF_TIMER_START                                INTERNAL FUNCTION

DESCRIPTION
  This function obtains the value of the tick counter and stores the value
  as the start count reference in usec.
  
DEPENDENCIES
  rf_chain_status structure previously initialized.

RETURN VALUE
  tick counter value in usecs.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rf_prof_timer_start( rf_path_enum_type path )
{
  /* Store timestamp */
  rf_wu_calc[path].wu_start_value = timetick_get();

  return ( rf_wu_calc[path].wu_start_value );
}

/*===========================================================================

FUNCTION RF_PROF_TIMER_STOP                                INTERNAL FUNCTION

DESCRIPTION
  This function obtains the elasped time now and when the last 
  rf_prof_timer_start function was called.  The elasped time is provided 
  in usecs.  
    
DEPENDENCIES
  rf_chain_status structure previously initialized.
  rf_prof_timer_start() called prior to this function

RETURN VALUE
  elapsed time in usec

SIDE EFFECTS
  None

===========================================================================*/
uint32 rf_prof_timer_stop( rf_path_enum_type path )
{
  uint32 delta_time = 0;

  /* 
  *  The first set of readings skew results high due to initialization   
  *  Clear out the first set of readings. 
  */
  if (rf_wu_calc[path].wu_is_first_sleep_cycle)
  {
    rf_wu_stats[path].rf_profile_wu_average = 0;
    rf_wu_stats[path].rf_profile_wu_last = 0;
    rf_wu_stats[path].wu_count = 0;

    rf_wu_calc[path].wu_total = 0;
    rf_wu_calc[path].wu_sum = 0;
    rf_wu_calc[path].wu_is_first_sleep_cycle = FALSE;    
  }
  else
  {
    /* Get the elapsed time from the start value */
    delta_time=(uint32)timetick_get_elapsed (rf_wu_calc[path].wu_start_value, T_USEC);

    rf_wu_calc[path].wu_sum += delta_time;
  }

  return ( delta_time );
}

/*===========================================================================

FUNCTION RF_PROF_ADD_TO_WARMUP                              INTERNAL FUNCTION

DESCRIPTION
  This function accumulates the warmup values.
  
    
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
__inline void rf_prof_add_to_warmup 
(
  rf_path_enum_type path,      /* rf path */
  uint32 warmup_component_us   /* warmup value to add in usecs */
)
{
  rf_wu_calc[path].wu_sum += warmup_component_us;
}

/*===========================================================================

FUNCTION RF_PROF_WARMUP_DONE                                INTERNAL FUNCTION

DESCRIPTION
  This function processes the current set of accumulated RF WU times.  The
  running average and current WU times are calculated and stored in the
  rf_chain_status structure for reference.   
    
DEPENDENCIES
  rf_chain_status structure previously initialized.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rf_prof_warmup_done
(
  rf_path_enum_type path      /* rf path */
)
{
  /* Store this RF WU set's current value */
  rf_wu_stats[path].rf_profile_wu_last = rf_wu_calc[path].wu_sum;

  /* Update values for calculating averages */
  rf_wu_calc[path].wu_total += rf_wu_stats[path].rf_profile_wu_last;
  rf_wu_stats[path].wu_count++;

  /* Calculate the average value */
  if ( rf_wu_stats[path].wu_count > 0 )
  {
    rf_wu_stats[path].rf_profile_wu_average = rf_wu_calc[path].wu_total / 
                                              rf_wu_stats[path].wu_count;
  }

  /* Clear for next WU set */
  rf_wu_calc[path].wu_sum = 0;
}

/*===========================================================================

FUNCTION RF_PROF_GET_WARMUP                                 INTERNAL FUNCTION

DESCRIPTION
  This function returns the RF WU last and average entries.   
    
DEPENDENCIES
  rf_chain_status structure previously initialized.
  
RETURN VALUE
  Parameters for last WU and average WU in usecs..

SIDE EFFECTS
  None

===========================================================================*/
void rf_prof_get_warmup
( 
  rf_path_enum_type path,    /* rf path */ 
  uint32 *last_wu_ptr,       /* last rf wu */
  uint32 *aver_wu_ptr        /* last rf aver wu */
)
{
  /* Make sure pointers are not NULL */
  if((last_wu_ptr != NULL) && (aver_wu_ptr != NULL))
  {
    /* Assign the WU values */
    *last_wu_ptr = rf_wu_stats[path].rf_profile_wu_last;
    *aver_wu_ptr = rf_wu_stats[path].rf_profile_wu_average;
  }
}

#ifdef RF_TIME_PROFILING
#error code not present
#endif /* RF_TIME_PROFILING */

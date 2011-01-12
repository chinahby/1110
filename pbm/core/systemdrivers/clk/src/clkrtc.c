/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               M S M   C L O C K    R T C   S E R V I C E S


GENERAL DESCRIPTION
    This module has RTC (Real Time Clock) services

EXTERNALIZED FUNCTIONS
clk_rtc_init

clk_rtc_get_time

clk_rtc_set_time


INITIALIZATION AND SEQUENCING REQUIREMENTS
    clk_rtc_init() should be called before calling any other function in this
module

  Copyright (c) 2003 by QUALCOMM, Incorporated.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


$Header: //depot/asic/msm6550/drivers/clk/clkrtc.c#2 $    $DateTime: 2005/10/28 09:42:35 $ $Author: andrewp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/07/03   kar     Initial revision
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "clkrtc.h"
#include "pm.h"
#include "msg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

FUNCTION CLK_RTC_INIT

DESCRIPTION
    This function gets time from PMIC and update time stamp with the PMIC
    RTC time. If PMIC RTC time is not set then the following time will be
    set for PMIC RTC:
    Year = 1980               Hour = 0
    Month = 1                 Minute  = 0
    Day   = 6                 Second = 0
    Day of week = 6

    Currently only 24 hour mode is supported

DEPENDENCIES
    sbi_init() and pm_init() must be called before calling this function

RETURN VALUE
    None

===========================================================================*/
void clk_rtc_init( void )
{
#ifdef FEATURE_PMIC_RTC
  clk_julian_type rtc_time;

  pm_err_flag_type pmic_err_flag = PM_ERR_FLAG__SUCCESS;

  /* Force display mode to 24 hours */
  pm_set_rtc_display_mode(PM_RTC_24HR_MODE);

  #if defined(FEATURE_PMIC_MM)
  /* Request a RTC Read */
  pmic_err_flag = pm_request_rtc_read(NULL);
  if (pmic_err_flag == PM_ERR_FLAG__IMMEDIATE_NOT_READY) {
    /* ignore this error */
    pmic_err_flag = PM_ERR_FLAG__SUCCESS;
  }
  #endif

  /* Get time from RTC before initialization */
  pmic_err_flag |= pm_rtc_rw_cmd(PM_RTC_GET_CMD, (pm_rtc_julian_type*)(&rtc_time));

  /* If we were not successful reading the time then
   try one more time */
  if (pmic_err_flag != PM_ERR_FLAG__SUCCESS)
  {
  #if defined(FEATURE_PMIC_MM)
  /* Request a RTC Read */
  pmic_err_flag = pm_request_rtc_read(NULL);
  if (pmic_err_flag == PM_ERR_FLAG__IMMEDIATE_NOT_READY) {
    /* ignore this error */
    pmic_err_flag = PM_ERR_FLAG__SUCCESS;
  }
  #endif

  /* Get time from RTC before initialization */
  pmic_err_flag |= pm_rtc_rw_cmd(PM_RTC_GET_CMD, (pm_rtc_julian_type*)(&rtc_time));
  }

  /* Check if time is valid. If not set default time */
  if ((pmic_err_flag != PM_ERR_FLAG__SUCCESS) ||
   !(pm_is_rtc_valid(PM_RTC_24HR_MODE, (pm_rtc_julian_type*)(&rtc_time))))
  {
    rtc_time.year = 1980;
    rtc_time.month = 1;
    rtc_time.day = 6;
    rtc_time.hour = 0;
    rtc_time.minute = 0;
    rtc_time.second = 0;
    rtc_time.day_of_week = 6;

    pm_rtc_rw_cmd(PM_RTC_SET_CMD, (pm_rtc_julian_type*)(&rtc_time));
  }


#endif
}

/*===========================================================================

FUNCTION CLK_RTC_GET_TIME

DESCRIPTION
    Get time from time stamp variable. And convert it to clk_julian_type

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_rtc_get_time (clk_julian_type *clk)
{
#ifdef FEATURE_PMIC_RTC
  pm_err_flag_type pmic_err_flag = PM_ERR_FLAG__SUCCESS;
  clk_julian_type rtc_time;

  /* Force display mode to 24 hours */
  pm_set_rtc_display_mode(PM_RTC_24HR_MODE);

  #if defined(FEATURE_PMIC_MM)
  /* Request a RTC Read */
  pmic_err_flag = pm_request_rtc_read(NULL);
  if (pmic_err_flag == PM_ERR_FLAG__IMMEDIATE_NOT_READY) {
    /* ignore this error */
    pmic_err_flag = PM_ERR_FLAG__SUCCESS;
  }
  #endif

  /* Get time from RTC before initialization */
  pmic_err_flag |= pm_rtc_rw_cmd(PM_RTC_GET_CMD, (pm_rtc_julian_type*)(&rtc_time));

  /* Check if time is valid. If not set default time */
  if ((pmic_err_flag != PM_ERR_FLAG__SUCCESS) ||
   !(pm_is_rtc_valid(PM_RTC_24HR_MODE, (pm_rtc_julian_type*)(&rtc_time))))
  {
    clk->year = 1980;
    clk->month = 1;
    clk->day = 6;
    clk->hour = 0;
    clk->minute = 0;
    clk->second = 0;
    clk->day_of_week = 6;
  }
  else {
    /* valid time */
    memcpy (clk, &rtc_time, sizeof(clk_julian_type));
  }
#endif
}


/*===========================================================================

FUNCTION CLK_RTC_SET_TIME

DESCRIPTION
    Set time for PMIC RTC and update system timestamp

DEPENDENCIES
    clk_rtc_init() should be called before calling this function

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void clk_rtc_set_time (clk_julian_type *clk)
{
#ifdef FEATURE_PMIC_RTC
    clk_julian_type rtc_time;

    memcpy (&rtc_time, clk, sizeof(clk_julian_type));

    /* Passing address of local copy of rtc since PMIC driver modifies the
     contents of rtc_time */
    pm_rtc_rw_cmd(PM_RTC_SET_CMD, (pm_rtc_julian_type*)(&rtc_time));

#endif

}





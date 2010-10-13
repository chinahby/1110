#ifndef CLK_RTC_H
#define CLK_RTC_H
/*===========================================================================

           C L O C K   R T C   H E A D E R    F I L E

DESCRIPTION
    Header file for clock RTC (Real Time Clock) services

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  $Header: //depot/asic/msm6550/drivers/clk/clkrtc.h#1 $ $DateTime: 2005/07/19 10:14:45 $ $Author: andrewp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/02   abp     Added defines for max values for hour, minute, second and 
                   month
12/10/02   abp     Initial release
===========================================================================*/

#include "clk.h" 


/*===========================================================================

                        DEFINITIONS AND DATA DECLARATIONS

===========================================================================*/
/* Max values for Hour, Minute, second, and month */
#define RTC_HOUR_MAX        23
#define RTC_MINUTE_MAX      59
#define RTC_SECOND_MAX      59
#define RTC_MONTH_MAX       12

/* Interval to resync timestamp with value from RTC */
#define CLKRTC_SYNC_INTERVAL_MS 20000

/*===========================================================================

                      FUNCTION DECLARATIONS

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
extern void clk_rtc_init( void );


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
extern void clk_rtc_get_time (clk_julian_type *clk);


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
extern void clk_rtc_set_time (clk_julian_type *clk);

/*===========================================================================

FUNCTION CLK_RTC_SYNC_CALLBACK

DESCRIPTION
    Update system timestamp to current PMIC RTC value.
    
DEPENDENCIES
    This function is a callback function and should not be called directly other
	code.
    
RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void clk_rtc_sync_callback(int4 ms_interval_unused);

#endif /* CLK_RTC_H */


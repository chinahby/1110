

/*===========================================================================
           P M   R T C   A P P   I M P L E M E N T A T I O N   F I L E

DESCRIPTION
  This file contains the implementation of the real-time clock application
  developed for the Qualcomm Power Manager Chip.

Copyright (c) 2003, 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pmapp_rtc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/07/07   aab     Added pm_rtc_pwr_rst_isr() and pm_rtc_get_power_reset_status() API
09/08/06   cng     LINT warning fixes
05/31/06   Vish    Fixed LINT warnings.
05/04/05   Vish    Changed pm_rtc_rw_cmd(PM_RTC_GET_CMD, ...) to return the
                   time even for the case when RTC is halted.
01/19/05   Vish    Updated function header for pm_rtc_rw_cmd().
11/07/03   Vish    Added the task of clearing the alarm trigger condition
                   within pm_reset_rtc_alarm() so that the master
                   RTC alarm interrupt could be cleared afterward. Also
                   resetting all alarms within pm_rtc_init() to avoid
                   any false alarms.                   
11/03/03   Vish    Renamed pm_s/get_time_adjust() to pm_rtc_s/get_time_adjust()
                   in order to be consistent with earlier PM chip libraries.
                   Added documentation. Wrapped this file under
                   FEATURE_PMIC_RTC.
10/30/03   Vish    Removed the use of pm_app_rtc_is_halted flag while
                   setting and getting time since RTC HAL already takes
                   care of it.
10/02/03   Vish    Adding alarm support.
09/24/03   Vish    Added pm_rtc_init() and also some checks to see if RTC
                   is currently halted before attempting to read time.
09/22/03   Vish    Created.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pm.h"
/*lint -esym(766, pmapp_rtc.h, pm66XXrtc.h, clk.h) */
#include "pmapp_rtc.h"
#include "pm66XXrtc.h"
#include "clk.h"

#if defined(FEATURE_PMIC_RTC)

/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
              LOCAL MACROS / VARIABLE / CONSTANT DEFINITIONS
===========================================================================*/
#define PM_RTC_MIN_YEAR          1980  // Lowest valid year
#define PM_RTC_MAX_YEAR          2100  // Highest valid year
#define PM_RTC_MAX_MONTH         12    // Highest valid number of months
#define PM_RTC_MAX_DOW           6     // Highest valid number of day-of-week
#define PM_RTC_MAX_DAY           31    // Highest valid number of days
#define PM_RTC_MAX_24HR_HOUR     23    // Highest valid number of hours (24HR mode)
#define PM_RTC_MAX_12HR_HOUR     32    // Highest valid number of hours (12HR mode)
#define PM_RTC_MIN_12HR_HOUR     01    // Lowest valid number of hours  (12HR mode)
#define PM_RTC_MIDL_12HR_HOUR    12    // In 12HR mode, 13 to 20 are
#define PM_RTC_MIDH_12HR_HOUR    21    //    not valid values
#define PM_RTC_MAX_MINUTES       59    // Highest valid number of minutes
#define PM_RTC_MAX_SECONDS       59    // Highest valid number of seconds

static pm_rtc_display_type  pm_app_rtc_current_display_mode = PM_RTC_24HR_MODE;
static boolean pm_rtc_power_reset_status = FALSE;

/*===========================================================================
                     LOCAL FUNCTION PROTOTYPES
===========================================================================*/
static void pm_app_rtc_conv_12hr_to_24hr_format(pm_rtc_julian_type *time_ptr);
static void pm_app_rtc_conv_24hr_to_12hr_format(pm_rtc_julian_type *time_ptr);
static pm_hal_rtc_alarm pm_app_rtc_alarm_conv_app_to_hal(pm_rtc_alarm_type);


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION   pm_rtc_init                                     EXTERNAL FUNCTION

DESCRIPTION
   This function is used for initializing RTC after a "power reset".

PARAMETERS
   None.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__SBI_OPT_ERR       -->  Communication with RTC failed

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked in this function.
===========================================================================*/
pm_err_flag_type pm_rtc_init(void)
{
   pm_err_flag_type      errFlag    = PM_ERR_FLAG__SUCCESS;
   
   // Clear all RTC alarms to make sure that we start off with a clean slate
   errFlag |= pm_reset_rtc_alarm(PM_RTC_ALL_ALARMS);

   return errFlag;
}


/*===========================================================================
FUNCTION   pm_get_rtc_display_mode                         EXTERNAL FUNCTION

DESCRIPTION
   This function returns the current real time clock display mode
   (24 or 12 hour mode). RTC defaults to 24 hr mode on phone power up and
   remains so until it is set to 12 hr mode explicitly using
   pm_set_rtc_display_mode().

PARAMETERS
   1) Name: mode_ptr
      - Pointer that is used to return the real time clock display mode.
      Type: pm_rtc_display_type*.
      - Valid values pointed to by the pointer:
         PM_RTC_12HR_MODE
         PM_RTC_24HR_MODE

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE -->  NULL pointer provided for mode_ptr

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   None.
===========================================================================*/
pm_err_flag_type pm_get_rtc_display_mode(
   pm_rtc_display_type  *mode_ptr)
{
   *mode_ptr = pm_app_rtc_current_display_mode;
   return PM_ERR_FLAG__SUCCESS;
}


/*===========================================================================
FUNCTION   pm_set_rtc_display_mode                         EXTERNAL FUNCTION

DESCRIPTION
   This function configures the real time clock display mode
   (24 or 12 hour mode). RTC defaults to 24 hr mode on phone power up and
   remains so until it is set to 12 hr mode explicitly using
   pm_set_rtc_display_mode().

PARAMETERS
   1) Name: mode
      - New RTC time display mode to be used.
      Type: pm_rtc_display_type
      - Valid values:
         PM_RTC_12HR_MODE
         PM_RTC_24HR_MODE

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE -->  Invalid display mode indicated by 'mode'

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   None.
===========================================================================*/
pm_err_flag_type pm_set_rtc_display_mode(
   pm_rtc_display_type mode)
{
   // do some sanity checking on params ...
   if (mode >= PM_RTC_INVALID_MODE)
   {
      return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
   }

   pm_app_rtc_current_display_mode = mode;
   return PM_ERR_FLAG__SUCCESS;
}


/*===========================================================================
FUNCTION   pm_rtc_rw_cmd                                   EXTERNAL FUNCTION

DESCRIPTION
   This function reads/writes the time and date from/to the PMIC RTC.
   The time/date format must be in 24 or 12 Hr mode depending on
   what mode, RTC has been initialized with; please refer to the description of
   pm_set_rtc_display_mode() for details.

   24 Hr and 12 Hr mode displays are related as shown below:

   24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
   12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31


PARAMETERS
   1) Name: cmd
      - Indicates if we want to set/get the current time in RTC.
      Type: pm_rtc_cmd_type
      - Valid Values:
         PM_RTC_SET_CMD
         PM_RTC_GET_CMD

   2) Name: current_time_ptr
      - Depending on the command, this function will use the structure
        pointer to update or return the current time in the RTC.
      Type: pm_rtc_julian_type*
         word year         = Year [1980..2100]
         word month        = Month of year [1..12]
         word day          = Day of month [1..31]
         word hour         = Depending on the display mode. Refer to the
                             function description.
         word minute       = Minute of hour [0..59]
         word second       = Second of minute [0..59]
         word day_of_week  = Day of the week [0..6] Monday through Sunday

         Note: 'day_of_week' is not required for setting the current time but
               returns the correct information when retrieving time from the RTC.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE  -->  Invalid command 'cmd'
     - PM_ERR_FLAG__PAR2_OUT_OF_RANGE  -->  NULL pointer provided for current_time_ptr
     - PM_ERR_FLAG__SBI_OPT_ERR        -->  Communication with RTC failed     
     - PM_ERR_FLAG__RTC_HALTED         -->  RTC is not running
     - PM_ERR_FLAG__SUCCESS            -->  SUCCESS

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()
   3) pm_set_rtc_display_mode()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
pm_err_flag_type  pm_rtc_rw_cmd(
   pm_rtc_cmd_type       cmd,
   pm_rtc_julian_type   *current_time_ptr)
{
   pm_err_flag_type      errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_hal_rtc_time_type  hal_time;

   // do some sanity checking on params ...
   if (cmd >= PM_RTC_INVALID_CMD)
   {
      return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
   }
   if (!current_time_ptr)
   {
      return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }

   switch (cmd)
   {
      case PM_RTC_SET_CMD:
         // Is user's time valid ?
         if (!pm_is_rtc_valid(pm_app_rtc_current_display_mode, current_time_ptr))
         {
            return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
         }
         else if (PM_RTC_12HR_MODE == pm_app_rtc_current_display_mode)  // time is in AM/PM mode?
         {
            // convert to a 24 hr time format understood by clk API
            pm_app_rtc_conv_12hr_to_24hr_format(current_time_ptr);
         }

         // convert Julian time ---> seconds elapsed from a known time in the past
         hal_time.sec = clk_julian_to_secs((clk_julian_type*)current_time_ptr);
         // start RTC
         errFlag |= pm_hal_rtc_start(&hal_time);
         break;

      case PM_RTC_GET_CMD:
         // get current time from HAL
         errFlag |= pm_hal_rtc_get_time(&hal_time);

         // use 'hal_time' irrespective of whether errors occured or not
         //if (!errFlag)
         //{
            // convert seconds ---> Julian time
            clk_secs_to_julian(hal_time.sec, (clk_julian_type*)current_time_ptr);

            // Julian time is in 24hr format
            // does the user need it in AM/PM format?
            if (PM_RTC_12HR_MODE == pm_app_rtc_current_display_mode)
            {
               pm_app_rtc_conv_24hr_to_12hr_format(current_time_ptr);
            }
         //}
         break;

      case PM_RTC_INVALID_CMD:
         // this will never be reached here because sanity check for 'cmd'
         // has already been done above; just to satisfy LINT's curiousity!
         break;

      // we'll not have 'default' so as to leave open the possibility for LINT
      //   to warn us when we do miss any new enums added in the future.
   }

   return errFlag;
}


/*===========================================================================
FUNCTION   pm_is_rtc_valid                                 EXTERNAL FUNCTION

DESCRIPTION
   Use this function to check if the RTC time going in or out of the
   PMIC if valid (within range). This function is meant to be used
   in conjunction with "pm_rtc_rw_cmd()".

   Example:
      pm_rtc_rw_cmd(PM_RTC_GET_CMD, &time);

      if (pm_is_rtc_valid(rtcMode, &time))
      {
         // RTC time is valid
      }

PARAMETERS
   1) Name: rtcMode
      - Display mode (24 or 12 hr)
      Type: pm_rtc_display_type
      - Valid Values:
            PM_RTC_12HR_MODE
            PM_RTC_24HR_MODE

   2) Name: time
      - Pointer to the Julian time structure that is to be validated.
      Type: pm_rtc_julian_type* Structure pointer.
     - Structure members:

        word year         = Year [1980..2100]
        word month        = Month of year [1..12]
        word day          = Day of month [1..31]
        word hour         = Depending on the display mode
        word minute       = Minute of hour [0..59]
        word second       = Second of minute [0..59]
        word day_of_week  = Day of the week [0..6] Monday through Sunday

RETURN VALUE
   Type: boolean
   - Possible values:
     - TRUE   -->  Time information pointed to by 'time' is valid
     - FALSE  -->  Otherwise

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
boolean pm_is_rtc_valid(
   pm_rtc_display_type          rtcMode,
   const pm_rtc_julian_type    *time)
{
   boolean time_valid = TRUE;

   /* Is the time read within range? */
   if ( (time->year < PM_RTC_MIN_YEAR)
        || (time->year > PM_RTC_MAX_YEAR)
        || (time->month > PM_RTC_MAX_MONTH)
        || (time->day_of_week > PM_RTC_MAX_DOW)
        || (time->day > PM_RTC_MAX_DAY)
        || (time->minute > PM_RTC_MAX_MINUTES)
        || (time->second > PM_RTC_MAX_SECONDS) )
   {
      time_valid = FALSE;
   }
   else
   {
      // Is the number of hours read within range?
      // check for both 24 and 12 hour modes

      // Here is the list of valid values:
      // 24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
      // 12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31
      switch(rtcMode)
      {
         case PM_RTC_12HR_MODE:
            if ( (time->hour > PM_RTC_MAX_12HR_HOUR)
                 || (time->hour < PM_RTC_MIN_12HR_HOUR)
                 || ( (time->hour > PM_RTC_MIDL_12HR_HOUR)
                      && (time->hour < PM_RTC_MIDH_12HR_HOUR) ) )
            {
               time_valid = FALSE;
            }
            break;

         case PM_RTC_24HR_MODE:
            if (time->hour > PM_RTC_MAX_24HR_HOUR)
            {
               time_valid = FALSE;
            }
            break;

         default:
            /* Invalid display mode */
            time_valid = FALSE;
      }
   }

   return time_valid;
}


/*===========================================================================
FUNCTION   pm_rtc_set_time_adjust                          EXTERNAL FUNCTION

DESCRIPTION
   This function sets the value for the 32768 Hz frequency offset
   adjustment in parts-per-million (PPM). In other words, adjust this value to
   compensate for inaccuracies in the 32768 Hz external crystal, which is used
   by the PMIC to run the RTC.

PARAMETERS
   Name: time_adjust
   - Adjustment (in PPM) = time_adjust * (3.05)
   Type: uint8
   - bits 0:5 represent the adjust value.
   - bit 6 is the sign bit
      1 --> Increase frequency (to compensate for slower than 32768 Hz crystal)
      0 --> Decrease frequency (to compensate for faster than 32768 Hz crystal)

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE  -->  Invalid value specified for time_adjust
     - PM_ERR_FLAG__SBI_OPT_ERR        -->  Communication with RTC failed
     - PM_ERR_FLAG__SUCCESS            -->  SUCCESS

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
pm_err_flag_type pm_rtc_set_time_adjust(
   uint8 time_adjust)
{
   uint8 hal_time_adjust = 0;

   // convesion math
   if (time_adjust < 64)  // sign-bit (6th bit is 0) and so freq is slowed down
   {
      //   time_adjust  --->  hal_time_adjust
      //             0  --->  64        [No freq adjustment]
      //             1  --->  65
      //             2  --->  66
      //             ...
      //            63  --->  127
      hal_time_adjust = (time_adjust + 64);
   }
   else if (time_adjust < 128)  // sign-bit is 1 and so freq is increased
   {
      //   time_adjust  --->  hal_time_adjust
      //            64  --->  64        [No freq adjustment]
      //            65  --->  63
      //            66  --->  62
      //            ...
      //           127  --->  1
      hal_time_adjust = (128 - time_adjust);
   }
   else  // invalid value provided
   {
      return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
   }

   return pm_hal_rtc_set_time_adjust(hal_time_adjust);
}


/*===========================================================================
FUNCTION   pm_rtc_get_time_adjust                          EXTERNAL FUNCTION

DESCRIPTION
   This function returns the current value used for the 32768 Hz frequency offset
   adjustment in PPM. This adjustment compensates for inaccuracies in the 32768 Hz
   external crystal, which is used by the PMIC to run the RTC.

PARAMETERS
   Name: time_adjust_ptr
   - Pointer to an 'uint8' used for returning the adjustment; use the following
     equation to calculate the frequency adjustment in parts-per-million (PPM):
                adjustment (PPM) = "adjustment value" * (3.05)
   Type: uint8* and the de-referenced uint8 has to be interpreted as follows
   - bits 0:5 represent the "adjustment value".
   - bit 6 is the sign bit
      1 --> Increase frequency (to compensate for slower than 32768 Hz crystal)
      0 --> Decrease frequency (to compensate for faster than 32768 Hz crystal)

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE  -->  NULL provided for time_adjust_ptr
     - PM_ERR_FLAG__SBI_OPT_ERR        -->  Communication with RTC failed
     - PM_ERR_FLAG__SUCCESS            -->  SUCCESS

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
pm_err_flag_type pm_rtc_get_time_adjust(
   uint8* time_adjust_ptr)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   uint8               hal_time_adjust = 0;

   errFlag = pm_hal_rtc_get_time_adjust(&hal_time_adjust);
   if (PM_ERR_FLAG__SUCCESS == errFlag)
   {
      // hal_time_adjust will be in [0..127] but 0 (195 ppm) is not
      // allowed by pm_rtc_set_time_adjust() but anyway we do
      // checks to make sure that we have a valid value to work with...
      if (0 == hal_time_adjust)         // lower boundary check
      {
         hal_time_adjust = 1;
      }
      else if (hal_time_adjust > 127)   // upper boundary check
      {
         hal_time_adjust = 127;
      }

      // conversion math
      if (hal_time_adjust < 64)   // range for increasing frequency
      {
         //   hal_time_adjust  --->  *time_adjust_ptr
         //                 1  --->  127
         //                 2  --->  126
         //                 ...
         //                63  --->  65
         *time_adjust_ptr = 128 - hal_time_adjust;
      }
      else // got to be less than 128
           // range for decreasing frequency or no change
      {
         //   hal_time_adjust  --->  *time_adjust_ptr
         //                 64 --->  0       [No freq adjustment]
         //                 65 --->  1
         //                 ...
         //                127 --->  63
         *time_adjust_ptr = hal_time_adjust - 64;
      }
   }
   return errFlag;
}


/*===========================================================================
FUNCTION   pm_rtc_alarm_rw_cmd                             EXTERNAL FUNCTION

DESCRIPTION
   This function reads/writes the alarm time and date from/to the PMIC RTC.
   The time/date format must be in 24 or 12 Hr mode depending on
   what mode, RTC has been initialized with; please refer to the description of
   pm_set_rtc_display_mode() for details.

   24 Hr and 12 Hr mode displays are related as shown below:

   24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
   12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31


PARAMETERS
   1) Name: cmd
      - Indicates if we want to set/get the current alarm time in RTC.
      Type: pm_rtc_cmd_type
      - Valid Values:
         PM_RTC_SET_CMD
         PM_RTC_GET_CMD

   2) Name: what_alarm
      - Indicates the specific alarm to be set/queried in RTC.
      Type: pm_rtc_alarm_type
      - Valid Values:
         PM_RTC_ALARM_1

   3) Name: alarm_time_ptr
      - Depending on the command, this function will use the structure
        pointer to update or return the current alarm time in the RTC.
      Type: pm_rtc_julian_type*
         word year         = Year [1980..2100]
         word month        = Month of year [1..12]
         word day          = Day of month [1..31]
         word hour         = Depending on the display mode. Refer to the
                             function description.
         word minute       = Minute of hour [0..59]
         word second       = Second of minute [0..59]
         word day_of_week  = Day of the week [0..6] Monday through Sunday

         Note: 'day_of_week' is not required for setting the current alarm time but
               returns the correct information when retrieving the current
               programmed alarm time from the RTC.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE  -->  Invalid command 'cmd'
     - PM_ERR_FLAG__PAR2_OUT_OF_RANGE  -->  Unknown alarm info provided for 'what_alarm'
     - PM_ERR_FLAG__PAR3_OUT_OF_RANGE  -->  NULL pointer provided for alarm_time_ptr
     - PM_ERR_FLAG__SBI_OPT_ERR        -->  Communication with RTC failed
     - PM_ERR_FLAG__SUCCESS            -->  SUCCESS

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()
   3) pm_set_rtc_display_mode()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
pm_err_flag_type pm_rtc_alarm_rw_cmd(
   pm_rtc_cmd_type       cmd,
   pm_rtc_alarm_type     what_alarm,
   pm_rtc_julian_type   *alarm_time_ptr)
{
   pm_err_flag_type      errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_hal_rtc_time_type  hal_alarm_time;
   pm_hal_rtc_alarm      hal_alarm  = PM_HAL_RTC_ALARM_INVALID;

   // do some sanity checking on params ...
   if (cmd >= PM_RTC_INVALID_CMD)
   {
      return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
   }

   if ( (!what_alarm)                         // Is this any alarm?
        // Is this a valid one?  
        || (what_alarm > PM_RTC_ALL_ALARMS)   //lint !e685 suppress "Relational operator '>' always evaluates to 'false'"
                                              // Reason: PM_RTC_ALL_ALARMS is set up as an OR of all individual alarms
                                              //         which themselves are varying powers of 2. So any alarm passed
                                              //         to this func as something greater than PM_RTC_ALL_ALARMS will
                                              //         be a bogus one, and this check will then evaluate to 'true'.
        // Does this include more than 1 alarm at the same time?
        || (what_alarm & (what_alarm-1)) )    //lint !e641 suppress "Converting enum 'pm_rtc_alarm_type' to int"
   {
      return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   hal_alarm = pm_app_rtc_alarm_conv_app_to_hal(what_alarm);

   if (!alarm_time_ptr)
   {
      return PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
   }

   switch (cmd)
   {
      case PM_RTC_SET_CMD:
         // Is the alarm time valid ?
         if (!pm_is_rtc_valid(pm_app_rtc_current_display_mode, alarm_time_ptr))
         {
            return PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
         }
         else if (PM_RTC_12HR_MODE == pm_app_rtc_current_display_mode)  // time is in AM/PM mode?
         {
            // convert to a 24 hr time format understood by clk API
            pm_app_rtc_conv_12hr_to_24hr_format(alarm_time_ptr);
         }

         // convert Julian time ---> seconds elapsed from a known time in the past
         hal_alarm_time.sec = clk_julian_to_secs((clk_julian_type*)alarm_time_ptr);
         // enable alarm
         errFlag |= pm_hal_rtc_enable_alarm(hal_alarm, &hal_alarm_time);
         break;

      case PM_RTC_GET_CMD:
         // get alarm time programmed from HAL
         errFlag |= pm_hal_rtc_get_alarm_time(hal_alarm, &hal_alarm_time);

         if (!errFlag)
         {
            // convert seconds ---> Julian time
            clk_secs_to_julian(hal_alarm_time.sec, (clk_julian_type*)alarm_time_ptr);

            // Julian time is in 24hr format
            // does the user need it in AM/PM format?
            if (PM_RTC_12HR_MODE == pm_app_rtc_current_display_mode)
            {
               pm_app_rtc_conv_24hr_to_12hr_format(alarm_time_ptr);
            }
         }
         break;

      case PM_RTC_INVALID_CMD:
         // this will never be reached here because sanity check for 'cmd'
         // has already been done above; just to satisfy LINT's curiousity!
         break;

      // we'll not have 'default' so as to leave open the possibility for LINT
      //   to warn us when we do miss any new enums added in the future.
   }

   return errFlag;
}


/*===========================================================================
FUNCTION   pm_reset_rtc_alarm                              EXTERNAL FUNCTION

DESCRIPTION
   This function disables the specified alarm so that it doesn't go off in
   the future. If the alarm has already triggered by the time this function
   was called, it will also clear the alarm trigger condition so that the
   master RTC alarm interrupt could be cleared afterward.

PARAMETERS
   1) Name: what_alarm
      - Indicates what alarm to be turned OFF.
      Type: pm_rtc_alarm_type
      - Valid inputs:
           PM_RTC_ALARM_1
           PM_RTC_ALL_ALARMS
      Note: If multiple alarms are available in the future, a subset of that
      can be selectively chosen using the bit-wise OR (|) operator

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE -->  Unknown alarm(s) specified
     - PM_ERR_FLAG__SBI_OPT_ERR       -->  Communication with RTC failed

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
pm_err_flag_type pm_reset_rtc_alarm(pm_rtc_alarm_type what_alarm)
{
   pm_err_flag_type      errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_hal_rtc_alarm      hal_alarm  = PM_HAL_RTC_ALARM_INVALID;

   // do some sanity checking on params ...
   if (what_alarm > PM_RTC_ALL_ALARMS)   //lint !e685 suppress "Relational operator '>' always evaluates to 'false'"
                                         // Reason: PM_RTC_ALL_ALARMS is set up as an OR of all individual alarms
                                         //         which themselves are varying powers of 2. So any alarm passed
                                         //         to this func as something greater than PM_RTC_ALL_ALARMS will
                                         //         be a bogus one, and this check will then evaluate to 'true'.
   {
      return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
   }

   // reset ALARM_1 if required ...
   
   //lint -e{655} suppress "bit-wise operation uses (compatible) enum's"
   //lint -e{641} suppress "Converting enum 'pm_rtc_alarm_type' to int"
   if (PM_RTC_ALARM_1 == (what_alarm & PM_RTC_ALARM_1))
   {
      hal_alarm = pm_app_rtc_alarm_conv_app_to_hal(PM_RTC_ALARM_1);
      errFlag |= pm_hal_rtc_disable_alarm(hal_alarm);
      what_alarm &= ( (pm_rtc_alarm_type)(~PM_RTC_ALARM_1) );
   }
   // reset other alarms in the future here ...

   return errFlag;
}


/*===========================================================================
FUNCTION   pm_get_rtc_alarm_status                         EXTERNAL FUNCTION

DESCRIPTION
   This function returns the status of the various alarms supported in
   RTC. A value of 1 means that the alarm has triggered and 0 means
   otherwise.

PARAMETERS
   1) Name: alarm_status
      -  Pointer to a valid uint8 used for returning the status info
         for the various alarms (currently only one alarm, PM_RTC_ALARM_1).
      Type: uint8*
      - Valid inputs:
         Pointer to a valid uint8 variable.
              Bit7   Bit6   Bit5   Bit4   Bit3   Bit2   Bit1   Bit0
                                                                ^
                                                                |
                                                            Alarm1's status

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE -->  NULL pointer provided for 'alarm_status'
     - PM_ERR_FLAG__SBI_OPT_ERR       -->  Communication with RTC failed

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
pm_err_flag_type pm_get_rtc_alarm_status(uint8 *alarm_status)
{
   return pm_hal_rtc_get_alarm_status(alarm_status);
}


/*===========================================================================
FUNCTION   pm_app_rtc_conv_12hr_to_24hr_format                LOCAL FUNCTION

DESCRIPTION
   This function converts the time from the 12-hr (AM/PM) format to the 24-hr
   format.

   24-hr and 12-hr formatting of the "hour hand" are related as shown below:

   24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
   12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31

PARAMETERS
   1) Name: time_ptr
      -  Pointer to a valid Julian time structure having time in 12-hr format.
         This function modifies the time pointed to by 'time_ptr' to 24-hr
         format in place.
      Type: pm_rtc_julian_type*

RETURN VALUE
   None.

DEPENDENCIES
   None.

SIDE EFFECTS
   The input time is modified to produce the output of the time format
   conversion.
===========================================================================*/
static void pm_app_rtc_conv_12hr_to_24hr_format(pm_rtc_julian_type *time_ptr)
{
   word  hr = time_ptr->hour;

   if (12 == hr)
   {
      time_ptr->hour = 0;
   }
   else if (32 == hr)
   {
      time_ptr->hour = 12;
   }
   else if ( (hr >= 21) && (hr <= 31) )
   {
      time_ptr->hour -= 8;
   }
}


/*===========================================================================
FUNCTION   pm_app_rtc_conv_24hr_to_12hr_format                LOCAL FUNCTION

DESCRIPTION
   This function converts the time from the 24-hr format to the 12-hr (AM/PM)
   format.

   24-hr and 12-hr formatting of the "hour hand" are related as shown below:

   24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
   12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31

PARAMETERS
   1) Name: time_ptr
      -  Pointer to a valid Julian time structure having time in 24-hr format.
         This function modifies the time pointed to by 'time_ptr' to 12-hr
         format in place.
      Type: pm_rtc_julian_type*


RETURN VALUE
   None.

DEPENDENCIES
   None.

SIDE EFFECTS
   The input time is modified to produce the output of the time format
   conversion.
===========================================================================*/
static void pm_app_rtc_conv_24hr_to_12hr_format(pm_rtc_julian_type *time_ptr)
{
   word  hr = time_ptr->hour;

   if (0 == hr)
   {
      time_ptr->hour = 12;
   }
   else if (12 == hr)
   {
      time_ptr->hour = 32;
   }
   else if ( (hr >= 13) && (hr <= 23) )
   {
      time_ptr->hour += 8;
   }
}


/*===========================================================================
FUNCTION   pm_app_rtc_alarm_conv_app_to_hal                   LOCAL FUNCTION

DESCRIPTION
   This function maps the alarm identifier used in this "RTC application"
   layer to the one actually used in the Hardware Abstraction Layer (HAL).

PARAMETERS
   1) Name: app_alarm
      -  Alarm identifier used in this "RTC application" layer
      Type: pm_rtc_alarm_type
      - Valid values:
         PM_RTC_ALARM_1

RETURN VALUE
   Type: pm_hal_rtc_alarm
   - Possible values:
     - PM_HAL_RTC_ALARM_1    -->   Mapped to by PM_RTC_ALARM_1

DEPENDENCIES
   None.

SIDE EFFECTS
   None.
===========================================================================*/
static pm_hal_rtc_alarm pm_app_rtc_alarm_conv_app_to_hal(pm_rtc_alarm_type app_alarm)
{
   pm_hal_rtc_alarm  hal_alarm = PM_HAL_RTC_ALARM_INVALID;

   switch (app_alarm)
   {
      case PM_RTC_ALARM_1:
         hal_alarm = PM_HAL_RTC_ALARM_1;
         break;

      default:
         break;
   }

   return hal_alarm;
}


/*===========================================================================
FUNCTION  pm_rtc_pwr_rst_isr                                  LOCAL FUNCTION
DESCRIPTION
  RTC power reset IRQ interrupt service routine; it initializes RTC after
  a power reset.
PARAMETERS
  None.
RETURN VALUE
  None.
DEPENDENCIES
  The following functions need to be called before we call this function:
  1) rflib_init()
  2) pm_init()
  3) pm_init_delayed()
SIDE EFFECTS
  Clears the RTC power reset IRQ
===========================================================================*/
void pm_rtc_pwr_rst_isr(void)
{
   //mask out the interrupt.  DO NOT CLEAR THE INTERRUPT
   (void)pm_set_irq_handle(PM_PWR_RST_IRQ_HDL, NULL);

   // initialize the RTC driver after a RTC power reset condition
   (void)pm_rtc_init();
   // if anything else is needed as a result of RTC having been halted
   // (like setting new time, alarms, etc.), do those here ...

   pm_rtc_power_reset_status = TRUE;
}



/*===========================================================================
FUNCTION   pm_rtc_get_power_reset_status               EXTERNAL FUNCTION

DESCRIPTION
   This function returns the status of rtc power reset latched status
   A value of 1 means that power reset happened and 0 means otherwise.

PARAMETERS
   None

RETURN VALUE
   None

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()
   
SIDE EFFECTS
   None
===========================================================================*/
boolean pm_rtc_get_power_reset_status(void)
{
   return pm_rtc_power_reset_status;
}

#endif //FEATURE_PMIC_RTC


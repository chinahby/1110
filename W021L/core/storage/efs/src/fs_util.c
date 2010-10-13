/***********************************************************************
 * fs_util.c
 *
 * Utility functions for EFS2
 * Copyright (C) 2003-2009, Qualcomm, Inc.
 *
 ***********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_util.c#5 $ $DateTime: 2009/07/06 18:43:01 $ $Author: ebuller $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2009-07-06    ebb     Stubbed out clk_xxx calls when compiling for boot.
2009-06-12    ebb     Moved rex_xxx calls to fs_osal.h abstraction.
2009-03-25    wek     Move FAT time/date conversions functions to fs_util.c.
2008-11-06    sh      Added Babylon substitution for ZAP
2007-02-02    umr     Integrated Julian time conversions.
2007-01-31     rp     Moved fs_strlcpy() and fs_strlcat() outside FS_STANDALONE
2007-01-12    dlb     Provide implementations of safer string routines.
2006-11-14    sh      Lint fix for QW_CVT_Q2N
2006-09-20    dlb     Lint cleanup.
2006-06-13    dlb     Clean up revision/copyright.
2005-12-06    dlb     Change time simulator for unit test.
2005-01-27    dlb     Allow standalone builds.
2004-10-15    dlb     Update copyright header.
2004-10-07    dlb     Whitespace cleanup.
2003-11-17     gr     Modified fs_time to use clk_read_ms to get rid of
                      rounding problems.
2003-10-17     gr     Moved a couple more functions in from elsewhere in EFS2.
2003-10-17     gr     Created.

===========================================================================*/

#include "fs_util.h"
#include "fs_osal.h"
#include "task.h"
#include "fs_babylon.h"

/* If we are compiling for the boot image stub out clk_xxx calls */
#ifdef FEATURE_EFS_BOOT_ENV
  #define FS_CLK_READ_MS(now) (void)now
  #define FS_QW_CVT_Q2N(now)  0
#else
  #include "clk.h"
  #define FS_CLK_READ_MS(now) clk_read_ms(now)
  #define FS_QW_CVT_Q2N(now)  QW_CVT_Q2N(now)
#endif
/*
 * Time Translation Tables
 * The following tables are used for making conversions between Julian dates
 * and number of seconds.
 */

/*
 * This is the number of days in a leap year set. A leap year set includes 1
 * leap year, and 3 normal years.
 */
#define FS_TIME_JUL_QUAD_YEAR        (366+(3*365))

/*  Definitions for FAT time bit fields.  */
#define FAT_TIME_SEC_SHIFT       0
#define FAT_TIME_SEC_MASK        0x001f /* Seconds 0-29 in two sec intervals */
#define FAT_TIME_MIN_SHIFT       5
#define FAT_TIME_MIN_MASK        0x07e0 /* Minutes 0-59  */
#define FAT_TIME_HOUR_SHIFT      11
#define FAT_TIME_HOUR_MASK       0xf800 /* Hour 0-23 */

/*  Definitions for FAT date bit fields.  */
#define FAT_DATE_DAY_SHIFT       0
#define FAT_DATE_DAY_MASK        0x001f  /* Day in month 1-31 */
#define FAT_DATE_MONTH_SHIFT     5
#define FAT_DATE_MONTH_MASK      0x01e0  /* Month 1-12 */
#define FAT_DATE_YEAR_SHIFT      9
#define FAT_DATE_YEAR_MASK       0xfe00  /* Year 0-127 (1980+value) */

/* Minimum value FAT time can take when expressed as POSIX
 * Duration between Jan 1st 1970 and Jan 1 1980, expressed as seconds. */
#define FS_TIME_FAT_DELTA         315532800

/* Represents Jan 1st 1980 in FAT 16 bit field */
#define FAT_DEFAULT_DATE          0x0021
/* FAT starts it's year count from 1980 */
#define FAT_BASE_YEAR             1980

/*
 * The year_tab table is used for determining the number of days which
 * have elapsed since the start of each year of a leap year set. It has
 * 1 extra entry which is used when trying to find a 'bracketing' year.
 * The search is for a day >= year_tab[i] and day < year_tab[i+1].
 */
static const uint16 year_tab[] = {
  0,                              /* Year 0 (leap year) */
  366,                            /* Year 1             */
  366+365,                        /* Year 2             */
  366+365+365,                    /* Year 3             */
  366+365+365+365                 /* Bracket year       */
};

/*
 * The norm_month_tab table holds the number of cumulative days that have
 * elapsed as of the end of each month during a non-leap year.
 */
static const uint16 norm_month_tab[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+28,                                /* Feb */
  31+28+31,                             /* Mar */
  31+28+31+30,                          /* Apr */
  31+28+31+30+31,                       /* May */
  31+28+31+30+31+30,                    /* Jun */
  31+28+31+30+31+30+31,                 /* Jul */
  31+28+31+30+31+30+31+31,              /* Aug */
  31+28+31+30+31+30+31+31+30,           /* Sep */
  31+28+31+30+31+30+31+31+30+31,        /* Oct */
  31+28+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+28+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/*
 * The leap_month_tab table holds the number of cumulative days that have
 * elapsed as of the end of each month during a leap year.
 */
static const uint16 leap_month_tab[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+29,                                /* Feb */
  31+29+31,                             /* Mar */
  31+29+31+30,                          /* Apr */
  31+29+31+30+31,                       /* May */
  31+29+31+30+31+30,                    /* Jun */
  31+29+31+30+31+30+31,                 /* Jul */
  31+29+31+30+31+30+31+31,              /* Aug */
  31+29+31+30+31+30+31+31+30,           /* Sep */
  31+29+31+30+31+30+31+31+30+31,        /* Oct */
  31+29+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+29+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/*
 * The day_offset table holds the number of days to offset as of the end of
 * each year.
 */
static const uint16 day_offset[] = {
  1,                                    /* Year 0 (leap year) */
  1+2,                                  /* Year 1             */
  1+2+1,                                /* Year 2             */
  1+2+1+1                               /* Year 3             */
};

/* 5 days (duration between Jan 1 and Jan 6), expressed as seconds. */
#define FS_TIME_JUL_OFFSET_S         432000UL

/*
 * This is the year upon which all time values used by the Clock Services are
 * based.  NOTE:  The user base day (GPS) is Jan 6 1980, but the internal base
 * date is Jan 1 1980 to simplify calculations
 */
#define FS_TIME_JUL_BASE_YEAR        1980

/******************************************************************************
 * FUNCTION    FS_TIME_JUL_FROM_SECS
 *
 * DESCRIPTION This procedure converts a specified number of elapsed seconds
 *             since the base date to its equivalent Julian date and time.
 *             Number of seconds since base date and Pointer to Julian date
 *             record are passed in as parameters.
 *
 * RETURN VALUE
 *             The specified Julian date record is filled in with equivalent
 *             date/time and returned into the area pointed to by julian_ptr.
 *****************************************************************************/
void
fs_time_jul_from_secs (uint32 secs, fs_time_julian_type *julian)
{
  unsigned int  i;
  /* Days since beginning of year or quad-year */
  uint16        days;
  /* Quad years since base date */
  unsigned int  quad_years;
  /* Leap-year or non-leap year month table */
  const uint16  *month_table;

  /*
   * Add number of seconds from Jan 1 to Jan 6 from input seconds in order to
   * have number of seconds since Jan 1, 1980 for calculation*
   */
  secs += FS_TIME_JUL_OFFSET_S;

  /*
   * Divide elapsed seconds by 60: remainder is seconds of Julian date;
   * quotient is number of elapsed minutes.
   */
  julian->second = secs % 60;
  secs = secs / 60;

  /*
   * Divide elapsed minutes by 60: remainder is minutes of Julian date;
   * quotient is elapsed hours.
   */
  julian->minute = secs % 60;
  secs = secs / 60;

  /*
   * Divide elapsed hours by 24: remainder is hours of Julian date; quotient
   * is elapsed days.
   */
  julian->hour = secs % 24;
  secs = secs / 24;

  /*
   * Now things get messier. We have number of elapsed days. The 1st thing we
   * do is compute how many leap year sets have gone by. We multiply this value
   * by 4 (since there are 4 years in a leap year set) and add in the base
   * year. This gives us a partial year value.
   */
  days = secs % FS_TIME_JUL_QUAD_YEAR;
  quad_years = secs / FS_TIME_JUL_QUAD_YEAR;

  julian->year = FS_TIME_JUL_BASE_YEAR + (4 * quad_years);

  /*
   * Now we use the year_tab to figure out which year of the leap year set we
   * are in.
   */
  for (i = 0; days >= year_tab[i + 1]; i++)
  {
    /* No-op. Table seach. */
  }

  /* Subtract out days prior to current year. */
  days -= year_tab[i];

  /* Use search index to arrive at current year. */
  julian->year += i;

  /*
   * Take the day-of-week offset for the number of quad-years, add in the
   * day-of-week offset for the year in a quad-year, add in the number of
   * days into this year.
   */
  julian->day_of_week =
  (day_offset[3] * quad_years + day_offset[i] + days) % 7;

  /*
   * Now we know year, hour, minute and second. We also know how many days we
   * are into the current year. From this, we can compute day & month.
   */

  /* Use leap_month_tab in leap years, and norm_month_tab in other years */

  month_table = (i == 0) ? leap_month_tab : norm_month_tab;

  /* Search month-table to compute month */
  for (i = 0; days >= month_table[i + 1]; i++)
  {
    /* No-op. Table seach. */
  }

  /* Compute & store day of month. */
  julian->day = days - month_table[i] + 1;

  /* Store month. */
  julian->month = i + 1;

} /* fs_time_jul_from_secs */

/******************************************************************************
 * FUNCTION      FS_TIME_JUL_TO_SECS
 *
 * DESCRIPTION   This procedure converts a specified Julian date and time to an
 *            equivalent number of elapsed seconds since the base date.
 *
 * RETURN VALUE
 *             Number of elapsed seconds since base date.
 *****************************************************************************/
uint32
fs_time_jul_to_secs (const fs_time_julian_type *julian)
{
  /* Time in various units (days, hours, minutes, and finally seconds) */
  uint32  time;

  /* First, compute number of days contained in the last whole leap
     year set. */
  time = ((julian->year - FS_TIME_JUL_BASE_YEAR) / 4L ) *
         FS_TIME_JUL_QUAD_YEAR;

  /* Now, add in days since last leap year to start of this month. */
  if ((julian->year & 0x3) == 0)
  {
    /*
     * If this is a leap year, add in only days since the beginning of the
     * year to start of this month.
     */
    time += leap_month_tab[julian->month - 1];
  }
  else
  {
    /*
     * If this is not a leap year, add in days since last leap year plus days
     * since start of this year to start of this month.
     */
    time += year_tab[julian->year & 0x3];
    time += norm_month_tab[julian->month - 1];
  }

  /* Add in days in current month. */
  time += julian->day - 1;
  /* Add in elapsed hours, minutes, and seconds  */
  time = time * 24 + julian->hour;
  time = time * 60 + julian->minute;
  time = time * 60 + julian->second;

  /*
   * Subtract number of seconds from base (GPS) date of 6 Jan 1980 to
   * calculation base date of 1 Jan 1980
   */
  time -= FS_TIME_JUL_OFFSET_S;

  /* Return elapsed seconds. */
  return time;
} /* fs_time_jul_to_secs */

/***********************************************************************
 * FUNCTION      fs_time
 *
 * DESCRIPTION   Returns the time in seconds since Jan 1, 1970 which is the
 *               starting point for time according to the POSIX(TM) standard.
 **********************************************************************/
unsigned int
fs_time (void)
{
  uint32 mobile_time;
  uint64 cur_msecs;

  /* convert to secs */
  cur_msecs = fs_time_ms_u64() / 1000;
  mobile_time = (uint32) cur_msecs;

  if (mobile_time < CDMA_VALID_TIME)
    mobile_time = 0;

  mobile_time += CDMA_TO_FS_TIME_DELTA;

  return mobile_time;
}

/***********************************************************************
 * FUNCTION      fs_time_ms
 *
 * DESCRIPTION   Returns the time in milliseconds since Jan 6, 1980
 *               CDMA Time.
 **********************************************************************/
void
fs_time_ms (qword now)
{
  FS_CLK_READ_MS (now);
}

/***********************************************************************
 * FUNCTION      fs_time_ms_u64
 *
 * DESCRIPTION   Returns the time in milliseconds since Jan 6, 1980
 *               CDMA Time. Same as fs_time_ms, but result is a uint64.
 **********************************************************************/
uint64
fs_time_ms_u64 (void)
{
  qword  now;
  uint64 cur_msecs;
  FS_CLK_READ_MS (now);
  cur_msecs = FS_QW_CVT_Q2N (now);  /*lint !e740*/

  return cur_msecs;
}

/***********************************************************************
 * FUNCTION      fs_util_wait
 *
 * DESCRIPTION   Waits for a specified time in milliseconds.
 *               It is assumed that FS_OP_COMPLETE_SIG will not be set
 *               for the calling task while in this function waiting
 *               for timer expiration.
 **********************************************************************/
void
fs_util_wait (int msec)
{
  fs_os_timer_type  fs_dev_tout_timer;
  fs_os_tcb_type    *my_tcb;

  my_tcb = fs_os_self ();
  fs_os_def_timer (&fs_dev_tout_timer, my_tcb, FS_OP_COMPLETE_SIG);
  fs_os_timed_wait (FS_OP_COMPLETE_SIG, &fs_dev_tout_timer, msec);
}

/* Lint isn't able to tell that the 'src' buffer passed into strlcpy will
 * have a null termination within it's valid range (this is not enforced by
 * this code).  This is the only way I've found to get past this warning.
 */
struct large_buffer {
  char buffer[65536];
};
#define LARGE_CAST(x)  (((struct large_buffer *) (x))->buffer)

size_t
fs_strlcpy (char *dest, const char *src, size_t dest_size)
{
  size_t len = strlen (src);

  if (dest_size > 0) {
    size_t n;

    n = MIN (len, dest_size - 1);

    /* Lint likes to warn about the source buffer not being long enough. */
    memmove (dest, LARGE_CAST (src), n);

    dest[n] = 0;
  }

  return len;
}

size_t
fs_strlcat (char *dest, const char *src, size_t dest_size)
{
  size_t len = 0;

#if 0
  while (len < dest_size && dest[len] != 0)
    len++;
#else
  /* I can't get the above to please lint. */
  len = strlen (dest);
  if (len >= dest_size)
    return len + strlen (src);
#endif

  return len + fs_strlcpy (dest + len, src, dest_size - len);
}

/* Duration in seconds since jan 1st 1970 to Base Year of 1980. */
#define FS_POSIX_TIME_TO_BASE_YEAR         315532800UL

/* Year to consider as base when doing calendar calculations */
#define FS_POSIX_CALENDAR_BASE_YEAR        1980

/* Time from jan 1st 1970 to Feb 28th 2100 23:59:59 */
#define FS_POSIX_TIME_TO_NON_LEAP_YEAR   4107542399UL


/******************************************************************************
 * FUNCTION    FS_POSIX_TIME_TO_CALENDAR
 *
 * DESCRIPTION This procedure converts a specified number of elapsed seconds
 *             since Jan 1st 1970 (POSIX Time) and converts it to a calendar
 *             form. The minimum date it can take is Jan 1st 1980. It can be
 *             changed by modifying FS_POSIX_CALENDAR_BASE and
 *             FS_POSIX_TIME_TO_BASE_YEAR to any leap year. Good for our
 *             purpose of using it for FAT. The Day of Week is one day off
 *             for any dates beyond Feb 28th 2100 23:59:59. We are not using
 *             it on the upper layer.
 *
 *             This function is uses the same algorithm as
 *             fs_time_jul_from_secs
 *
 * RETURN VALUE  The parameter julian is populated with the calendar date.
 *****************************************************************************/
static void
fs_posix_time_to_calendar (uint32 secs, fs_time_julian_type *julian)
{
  unsigned int  i;
  /* Days since beginning of year or quad-year */
  uint16        days;
  /* Quad years since base date */
  unsigned int  quad_years;
  /* Leap-year or non-leap year month table */
  const uint16  *month_table;

  memset (julian, 0, sizeof (fs_time_julian_type));

  /* If seconds is below our base year, bail out. */
  if (secs < FS_POSIX_TIME_TO_BASE_YEAR)
    return;

  /* Add number of seconds since calendar base year. */
  secs -= FS_POSIX_TIME_TO_BASE_YEAR;

  /* This is a quick fix for some drawbacks of the original algorithm.
   * This algorithm considers all years divisible by 4 a leap years. It does
   * not take into account the non leap years that are divisible by 100
   * but not by 400, for example 1700, 1800, 1900, 2100 do not have a Feb 29th,
   * even though they are divisible by 4.
   *
   * Our representation of time has two limits, Jan 1st 1970 and Feb 7th 2106
   * 06:28:15 and between the limits there is only one case of the special non
   * leap year (2100). To account for this we can add an one day offset for
   * any day above Feb 28th 2100 23:59:59 */
  if (secs > (FS_POSIX_TIME_TO_NON_LEAP_YEAR - FS_POSIX_TIME_TO_BASE_YEAR))
    secs += (60 * 60 * 24);

  /*
   * Divide elapsed seconds by 60: remainder is seconds of Julian date;
   * quotient is number of elapsed minutes.
   */
  julian->second = secs % 60;
  secs = secs / 60;

  /*
   * Divide elapsed minutes by 60: remainder is minutes of Julian date;
   * quotient is elapsed hours.
   */
  julian->minute = secs % 60;
  secs = secs / 60;

  /*
   * Divide elapsed hours by 24: remainder is hours of Julian date; quotient
   * is elapsed days.
   */
  julian->hour = secs % 24;
  secs = secs / 24;

  /*
   * Now things get messier. We have number of elapsed days. The 1st thing we
   * do is compute how many leap year sets have gone by. We multiply this value
   * by 4 (since there are 4 years in a leap year set) and add in the base
   * year. This gives us a partial year value.
   */
  days = secs % FS_TIME_JUL_QUAD_YEAR;
  quad_years = secs / FS_TIME_JUL_QUAD_YEAR;

  julian->year = FS_POSIX_CALENDAR_BASE_YEAR + (4 * quad_years);

  /*
   * Now we use the year_tab to figure out which year of the leap year set we
   * are in.
   */
  for (i = 0; days >= year_tab[i + 1]; i++)
  {
    /* No-op. Table seach. */
  }

  /* Subtract out days prior to current year. */
  days -= year_tab[i];

  /* Use search index to arrive at current year. */
  julian->year += i;

  /*
   * Take the day-of-week offset for the number of quad-years, add in the
   * day-of-week offset for the year in a quad-year, add in the number of
   * days into this year.
   */
  julian->day_of_week =
  (day_offset[3] * quad_years + day_offset[i] + days) % 7;

  /*
   * Now we know year, hour, minute and second. We also know how many days we
   * are into the current year. From this, we can compute day & month.
   */

  /* Use leap_month_tab in leap years, and norm_month_tab in other years */

  month_table = (i == 0) ? leap_month_tab : norm_month_tab;

  /* Search month-table to compute month */
  for (i = 0; days >= month_table[i + 1]; i++)
  {
    /* No-op. Table seach. */
  }

  /* Compute & store day of month. */
  julian->day = days - month_table[i] + 1;

  /* Store month. */
  julian->month = i + 1;

} /* fs_posix_time_to_calendar */

/******************************************************************************
 * FUNCTION      FS_CALENDAR_TO_POSIX_TIME
 *
 * DESCRIPTION   This procedure converts a specified calendar date and time
 *               to an equivalent POSIX time.
 *
 * RETURN VALUE  Number of elapsed seconds since Jan 1st 1970.
 *****************************************************************************/
static uint32
fs_calendar_to_posix_time (const fs_time_julian_type *julian)
{
  /* Time in various units (days, hours, minutes, and finally seconds) */
  uint32  time;

  /* First, compute number of days contained in the last whole leap
     year set. */
  time = ((julian->year - FS_POSIX_CALENDAR_BASE_YEAR) / 4L ) *
         FS_TIME_JUL_QUAD_YEAR;

  /* Now, add in days since last leap year to start of this month. */
  if ((julian->year & 0x3) == 0)
  {
    /*
     * If this is a leap year, add in only days since the beginning of the
     * year to start of this month.
     */
    time += leap_month_tab[julian->month - 1];
  }
  else
  {
    /*
     * If this is not a leap year, add in days since last leap year plus days
     * since start of this year to start of this month.
     */
    time += year_tab[julian->year & 0x3];
    time += norm_month_tab[julian->month - 1];
  }

  /* Add in days in current month. */
  time += julian->day - 1;
  /* Add in elapsed hours, minutes, and seconds  */
  time = time * 24 + julian->hour;
  time = time * 60 + julian->minute;
  time = time * 60 + julian->second;

  /* This is a temporary fix for some drawbacks of the original algorithm.
   * For an explanation on why this is done read the comments on
   * fs_poxix_time_to_calendar () */
  if (time > (FS_POSIX_TIME_TO_NON_LEAP_YEAR - FS_POSIX_TIME_TO_BASE_YEAR))
    time -= (60 * 60 * 24);

  /* Check overflow. */
  if ((0xFFFFFFFF - FS_POSIX_TIME_TO_BASE_YEAR) > time)
    /* Add number of seconds from Base calendar year.  */
    time += FS_POSIX_TIME_TO_BASE_YEAR;
  else
    /* Overflow, use max value */
    time = 0xFFFFFFFF;

  /* Return elapsed seconds. */
  return time;
} /* fs_calendar_to_posix_time */

uint16
fs_posix_to_fat_date (uint32 ptime)
{
  fs_time_julian_type julian;
  uint16 fatdate;

  /* FAT can't represent any date/time below Jan 1st 1980, If it is below
   * just return a defult date, the lowest possible date. */
  if (ptime < FS_TIME_FAT_DELTA)
    return FAT_DEFAULT_DATE;

  fs_posix_time_to_calendar (ptime, &julian);

  fatdate = (unsigned short)
    ((((julian.year - FAT_BASE_YEAR) << FAT_DATE_YEAR_SHIFT) &
                                                      FAT_DATE_YEAR_MASK) |
            ((julian.month << FAT_DATE_MONTH_SHIFT) & FAT_DATE_MONTH_MASK) |
                ((julian.day << FAT_DATE_DAY_SHIFT) & FAT_DATE_DAY_MASK));

  return fatdate;
}


uint16
fs_posix_to_fat_time (uint32 ptime)
{
  unsigned short hour;
  unsigned short min;
  unsigned short sec;
  uint16 fattime;

  /* FAT cant represent any date/time below Jan 1st 1980, check for this
   * condition */
  if (ptime < FS_TIME_FAT_DELTA)
    return 0;

  /* We just need Hour, Min and Seconds here. Simply divide by 60 by 60 and
   * than 24. We dont have to go trough complications in
   * fs_time_jul_from_secs() to calculate week date leap year etc.
   */
  sec = ptime % 60;
  ptime = ptime/60;

  min = ptime % 60;
  ptime = ptime/60;

  hour = ptime % 24;

  fattime = (unsigned short)
         (((hour << FAT_TIME_HOUR_SHIFT) & FAT_TIME_HOUR_MASK) |
            ((min << FAT_TIME_MIN_SHIFT) & FAT_TIME_MIN_MASK) |
     (((sec >> 1) << FAT_TIME_SEC_SHIFT) & FAT_TIME_SEC_MASK));

  return fattime;
}

uint32
fs_fat_to_posix_time (uint16 fat_time, uint16 fat_date)
{
  fs_time_julian_type jul;
  uint32 posix_time;

  jul.year = fat_date >> FAT_DATE_YEAR_SHIFT;
  jul.year += FAT_BASE_YEAR;

  jul.month = (fat_date & FAT_DATE_MONTH_MASK) >> FAT_DATE_MONTH_SHIFT;

  jul.day = (fat_date & FAT_DATE_DAY_MASK) >> FAT_DATE_DAY_SHIFT;

  jul.hour = (fat_time & FAT_TIME_HOUR_MASK) >> FAT_TIME_HOUR_SHIFT;
  jul.minute = (fat_time & FAT_TIME_MIN_MASK) >> FAT_TIME_MIN_SHIFT;
  jul.second = ((fat_time & FAT_TIME_SEC_MASK) >> FAT_TIME_SEC_SHIFT)<<1;
  posix_time = fs_calendar_to_posix_time (&jul);

  return posix_time;
}

/* Long ago, we had a facility called ZAP that would send messages out
 * using the T32 JTAG interface and DCC.  This messaging system was
 * very intrusive and would slow the build.  It's no longer supported.
 * However, there are still some useful messages and markers in the
 * code that calls these old JZAP macros.  We handle those with
 * Babylon, when that feature is enabled.  To do it, we have to have a
 * printf-like mechanism to prepare the strings.
 *
 * This should only be enabled in debug/engineering builds!
 */
#ifdef FEATURE_EFS_BABYLON
#include <stdarg.h>             /* for va_arg */
#include <stdio.h>              /* for vsnprintf */

/* The string buffer has to be long enough for the longest possible message,
 * yet not so long as to overflow the caller's stack. */
#define MAX_JZAP_MSG_LEN 200

void
jzap_bab_printf (const char *format, ...)
{
  static char print_buf[MAX_JZAP_MSG_LEN];
  va_list arg_ptr;

/*lint -save -e746 -e10 -e64 -e718 -e529 -e530 */
  /* Prepare the string */
  va_start (arg_ptr, format);
  (void) vsnprintf (print_buf, MAX_JZAP_MSG_LEN, format, arg_ptr);
  va_end (arg_ptr);
/*lint -restore */

  /* Send the resulting string to Babylon */
  EFS_BAB_PRINT (EFS_BAB_ZAP, print_buf);
}
#endif

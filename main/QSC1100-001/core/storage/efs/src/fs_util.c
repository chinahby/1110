/***********************************************************************
 * fs_util.c
 *
 * Utility functions for EFS2
 * Copyright (C) 2003-2008, Qualcomm, Inc.
 *
 ***********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_util.c#2 $ $DateTime: 2008/12/12 09:45:53 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
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
#include "clk.h"
#include "rex.h"
#include "task.h"
#include "fs_babylon.h"

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
  clk_read_ms (now);
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
  clk_read_ms (now);
  cur_msecs = QW_CVT_Q2N (now);  /*lint !e740*/

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
  rex_timer_type  fs_dev_tout_timer;
  rex_tcb_type    *my_tcb;

  my_tcb = rex_self ();
  rex_def_timer (&fs_dev_tout_timer, my_tcb, FS_OP_COMPLETE_SIG);
  rex_timed_wait (FS_OP_COMPLETE_SIG, &fs_dev_tout_timer, msec);
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

/* =======================================================================
                               oscl_time.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/src/oscl_time.cpp#7 $
$DateTime: 2008/05/08 14:04:55 $
$Change: 656428 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"
#include "oscl_time.h"
#include "byte_order_utils.h"
#include "qtv_msg.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* const uint64 scaled_freq_conv = 28147498;*/
const uint32 origin_offset = 2208988800U;
const uint64 freq_conv = 281474977;

const char months[][4] = { "Jan", "Feb", "Mar", "Apr", "May",
  "Jun", "Jul", "Aug", "Sep", "Oct", "Nov","Dec",
  ""};
const char days[][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION 
  set_to_current_time

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
int NTPTime::set_to_current_time()
{
  zrex_time_offset_type tv;
  uint64 tmp;


  // get the current time
  // This gives back time since 0h Jan 1, 1970 (ZREX_EPOCH)
  zrex_get_current_time_offset(&tv);


  timevalue = origin_offset;
  // now convert to proper time origin
  timevalue += tv.seconds;

  timevalue <<= 32;



  // Do the same thing as unix but multiply by 1000 since the lower 32 bits
  // of the timeval for unix is in microseconds.

  // now convert the frequency of the fractional part
  // from usec to 2^32 ticks/sec.
  tmp = ((unsigned long)(tv.milliseconds * 1000) * freq_conv) >> 16;

  timevalue += tmp;

  return 0;
}

/* ======================================================================
FUNCTION 
  DayIndexFromDate

DESCRIPTION
 return index (0=Sunday, 1=Monday... etc)
 Uses the day-of-the-week algorithm from the book Elementary Number
 Theory by Uspensky and Heaslet.  To simplify the algorithm March is 
 used as the first month in the year and so year and month are changed
 accordingly

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
========================================================================== */
int DayIndexFromDate(int year, int month, int day)
{
  int y;
  int c;

  if ( month > 12 || month < 1 || year < 1 || day < 1 || day > 31 )
    return -1; // bad value(s)

  year = ( month < 3) ? year - 1 : year;
  month = ( month < 3) ? month + 10 : month - 2;

  y = year%100;
  c = year/100;

  int day_index = (day + int(2.6*double(month)-0.2) + y + y/4 + c/4 - 2*c ) % 7;
  
  // make sure it's not negative
  if (day_index < 0)
  {
    day_index += 7;
  }

  return day_index;
}


/* ======================================================================
FUNCTION 
  PV8601ToRFC822

DESCRIPTION
  // Convert from PV8601 Format (20010805T185430.000Z) 
  // to RFC822 Format (Sun Aug  5 18:54:30 2001)
  // No <time.h> functions will be used because this code must run on
  // all platforms.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
bool PV8601ToRFC822(PV8601timeStrBuf pv8601_buffer, CtimeStrBuf ctime_buffer)
{

  char buf[5];

  int year,mon,day,hour,min,sec;

  (void)std_strlcpy(buf,pv8601_buffer,5);
  year = atoi(buf);

  (void)std_strlcpy(buf,pv8601_buffer+4,3);
  mon = atoi(buf);
  if( !( (mon >=1)&&(mon<=12) ) )
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PV8601ToRFC822 ,Invalid Month field" );
      return false;
  }

  std_strlcpy(buf,pv8601_buffer+6,3);
  day = atoi(buf);
  if( !( (day >=1)&&(day<=31) ) )
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PV8601ToRFC822 ,Invalid Day field" );      
      return false;
  }


  std_strlcpy(buf,pv8601_buffer+9,3);
  hour = atoi(buf);
  if( !( (hour >=0)&&(hour<=23) ) )
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PV8601ToRFC822 ,Invalid Hour field" );
      return false;
  }

  std_strlcpy(buf,pv8601_buffer+11,3);
  min = atoi(buf);
  if( !( (min >=0)&&(min<=59) ) )
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PV8601ToRFC822 ,Invalid Minute field" );
      return false;
  }

  std_strlcpy(buf,pv8601_buffer+13,3);
  sec = atoi(buf);
  if( !( (sec >=0)&&(sec<=59) ) )
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PV8601ToRFC822 ,Invalid Second field" );
      return false;
  }

  if( (DayIndexFromDate(year,mon,day) == -1) )
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PV8601ToRFC822 ,Invalid Date used" );
      return false;
  }

  (void)std_strlprintf(ctime_buffer,CTIME_BUFFER_SIZE,
          "%s %s %2d %02d:%02d:%02d %04d",  
          days[DayIndexFromDate(year,mon,day)],
          months[mon-1],
          day, hour, min, sec, year);

  return true;
}

/* ======================================================================
FUNCTION 
  TimeValue::get_rfc822_gmtime_str

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
char *TimeValue::get_rfc822_gmtime_str(int, char *time_str)
{
  time_str[0] = NULL_TERM_CHAR;
  return time_str;
}

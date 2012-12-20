#ifndef __OSCL_TIME_H
#define __OSCL_TIME_H
/* =======================================================================
                               oscl_time.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/inc/oscl_time.h#8 $
$DateTime: 2009/01/29 06:14:06 $
$Change: 829425 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include <stdio.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"

#include <time.h>

#include "zrex_os.h"
#include <string.h>

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
const int CTIME_BUFFER_SIZE = 26;
const int PV8601TIME_BUFFER_SIZE = 21;

//! The following are used internally so not documented in the API
const long MapToSeconds[] = {1, 1000, 1000000};
const long MapToUSeconds[] = {1000000, 1000, 1};
const long MapTo100NanoSeconds[] = {10000000, 10000, 10};
// ms from 1900, Jan, 1, 0h to 1970, Jan, 1, 0h
const uint32 unix_ntp_offset=2208988800U;
// ms from 1970, Jan, 1, 0h to 1980, Jan, 6, 0h
const uint32 sys_ntp_offset = 2524953600U; 

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef char CtimeStrBuf[CTIME_BUFFER_SIZE];
typedef char PV8601timeStrBuf[PV8601TIME_BUFFER_SIZE];

typedef timeval BasicTimeStruct;
class TimeValue;  // Forward definition

//! The TimeUnits enum can be used when constructing a TimeValue class.
#ifndef FEATURE_WINCE
typedef enum { SECONDS=0, MILLISECONDS=1, MICROSECONDS=2 } TimeUnits;
#endif // #ifndef FEATURE_WINCE

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
FUNCTION 
  PV8601ToRFC822

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
bool PV8601ToRFC822(PV8601timeStrBuf pv8601_buffer, CtimeStrBuf ctime_buffer);

/* ======================================================================
FUNCTION 
  RFC822ToPV8601

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
void RFC822ToPV8601(CtimeStrBuf ctime_buffer, PV8601timeStrBuf);

/* ======================================================================
CLASS 
  NTPTime

DESCRIPTION
  The NTPTime class represents a time value as the number of seconds since 0h (UTC) Jan. 1, 1900

   Conversion to/from Unix (epoch at 0h Jan. 1, 1970) amount to addition/subtraction of 2208988800.
   A single 64 bit value is used to represent the time. This value represents the number of seconds
   since 0h (UTC) Jan. 1, 1900. There is an implied binary point betweeen the upper 32 bits and lower
   32 bits (this is referred to as a 32.32 fractional representation).
   For example a binary value of 00000000 00000000 00000000 00000011 10000000 00000000 00000000 00000000
   represents 3.5 seconds since Jan 1, 1900.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class NTPTime {
  
private:
  uint64 timevalue;
 
public:
  //! The default constructor creates an NTPTime instance representing the current system time.
  NTPTime() {set_to_current_time();}

  //! Copy constructor to create a new NTPTime from an existing one.
  NTPTime(const NTPTime& src) {timevalue = src.timevalue;};
  
  //! Construct an NTPTime from a uint32. 
  /*! \param seconds The uint32 input represents the number of seconds since Jan. 1, 1900. */

  NTPTime(const uint32 seconds) {timevalue = int64(seconds) << 32;};

  //! Construct an NTPTime from a int. 
  /*! \param seconds The int input represents the number of seconds since Jan. 1, 1900. */
  NTPTime(const int seconds) 
  {
    if (seconds < 0) return; 
    timevalue = int64(seconds) << 32;
  };
  

  //! Construct a NTPTime instance from a TimeValue instance.
  /*!
     This constructor creates an NTPTime value representing the same absolute time as the TimeValue parameter.
   \param t A reference to a TimeValue object.
  */
  NTPTime(const TimeValue& t);

  //! Construct a NTPTime instance from a uint64 value.
  /*! \param value A 64 bit integer argument which is used as the ntp 32.32 fractional representation.*/
  NTPTime(const uint64 value){timevalue = value;}; 

  
  //! The assignment operator for a 32 bit integer.
  /*! \param newval A 32 bit integer representing the upper 32 bits of the 32.32 NTP time (e.g. the number of whole seconds since Jan 1, 1900 UTC).*/

  NTPTime& operator=(uint32 newval) {timevalue = int64(newval) << 32;
  return *this;};


  //! The assignment operator for a 64 bit integer.
  /*! \param newval A 64 bit value which represents the 32.32 fractional representation of the ntp time. */
  NTPTime& operator=(uint64 newval) {timevalue = newval; return *this;};
  
  //! The += operator is used to add a 64 bit 32.32 value to an existing NTPTime value.
  /*! \param val The 64 bit 32.32 value to add to this object's value. */
  NTPTime& operator+=(uint64 val) {timevalue += val; return *this;};

  //! The - operator allows subtraction of one NTPTime value from another.  This is useful to measure an interval.
  /*! \param ntpt A reference to the NTPTime object to be subracted from this one. */

  NTPTime  operator-(const NTPTime &ntpt) const {NTPTime temp;temp.timevalue = timevalue - ntpt.timevalue;return temp;};

  //! This method coverts a 32-bit unix time to NTP time.
  /*!
      This method sets the value of the NTPTime instance to the absolute time represented by the 32 bit input
    argument.
    \param unixtime This 32-bit value is interpreted as the number of seconds since the unix epoch Jan. 1 1970.
  */

  void set_from_unix_time(const uint32 unixtime) {timevalue = int64(unixtime + unix_ntp_offset) << 32;};

  //! Grab the middle 32 bits of the 64 bit 32.32 representation.
  /* \return This method returns the middle 32 bits of the 32.32 representation. */

  uint32 get_middle32() const {return (uint32)((timevalue >> 16) & 0xFFFFFFFF);};


  //! This method returns the upper 32 bits of the 32.32 representation.

  uint32 get_upper32() const {return (uint32)((timevalue >> 32)  & 0xFFFFFFFF);};


  //! This method returns the lower 32 bits of the 32.32 representation.

  uint32 get_lower32() const {return (uint32)((timevalue)  & 0xFFFFFFFF);};

  //! This method converts the ntp time value to unix time.
  /*!
      This method returns a 32 bit value representing the same absolute time as the NTP time value,
    but expressed as whole seconds since the unix epoch. The fractional part of the ntp value is discarded.
  */

  int32 to_unix_time() const {return (uint32)((timevalue >> 32) - unix_ntp_offset);};

  //! This method returns the time value in millisec
  uint64 to_ms() const
  {
    uint64 temp = (uint64)get_lower32() * 1000;
    temp /= 0xFFFFFFFF;
    temp += (uint64)get_upper32() * 1000;
    return temp;
  }

  //! This method returns the 32.32 ntp representation.
  uint64 get_value()   const {return timevalue;};

  //! This method sets the 32.32 representation to the current system time value.
  int set_to_current_time(); 
  
};

/* ======================================================================
CLASS 
  TimeValue

DESCRIPTION
  The TimeValue class represents a time value in a format native to the system.
  This class provides common time functions independent of any OS. The actual representation used
  is native to the system that the class is compiled on to increase efficiency.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class TimeValue {
  public:
  //! Create a TimeValue representing the current time.
  TimeValue();

  static char *get_rfc822_gmtime_str(int max_time_strlen, 
                              char *time_str);
};

/* ======================================================================
FUNCTION 
  TimeValue::TimeValue

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
inline TimeValue::TimeValue()
{

}

#endif // __OSCL_TIME_H

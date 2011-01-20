/*===========================================================================

                  D A T A  S E R V I C E S  N T P  F I L E
                   
DESCRIPTION

  This module contains functions that for converting a GPS format time stamp
  (returned by ts_get()) to a NTP format time stamp.

EXTERNALIZED FUNCTIONS

  ds_get_ntp()
    This function converts the time stamp returned by ts_get (usually in GPS
    format) to NTP format.


Copyright (c) 2001-2002 by QUALCOMM, Incorporated.  All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsntp.c_v   1.9   07 Oct 2002 14:17:52   senthilv  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsntp.c#2 $ $DateTime: 2008/06/02 08:26:53 $ $Author: msankar $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/08    ms     Fixed Critical/High Lint errors
06/24/03   jd      Removed GSM/GPRS featurization since ts_get should always 
                   work.  Limitation for now: NTP services only work in 1X.
10/7/02    vsk     GPRS integration changes : feature wrap in FEATURE_GSM_GPRS
04/17/02   rc      Wrapped QW_CVT_Q2N macro in !FEATURE_DATA_WCDMA_PS
02/12/02   dwp     Wrap module in FT_DS || FT_DATA_PS for non DS builds.
12/21/01   dwp     Wrap QW_CVT_Q2N macro in !FEATURE_DATA_PS.
11/12/01   dwp     Add include of qw.h.
08/14/01   jd      Fixed EPOCH_OFFSET for ARM (was off by 2 days)
06/22/01   jd      Fixed calculation of fractional seconds
06/21/01   jd      Fixed EPOCH_OFFSET and EPOCH_DIVERGENCE for unix
06/20/01   jd      GPS/NTP epoch delta incorrectly counted 1900 as a leap 
                   year - fixed.
04/14/01   jd      Added support for off-target (unix) builds
02/14/01   js      Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_DS) || defined(FEATURE_DATA_PS)

#include "dsntp.h" 
#include "msg.h" 
#include "qw.h"

#ifdef __unix
#error code not present
#elif defined T_ARM

#include "ts.h"

#endif



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*--------------------------------------------------------------------------
  EPOCH_OFFSET = the offset between NTP and local epoch in s.

  For GPS,
  This following is the number of standard seconds between NTP's epoch
  (1/1/1900) and GPS's epoch (1/6/1980).

  This gap contains a total of 80 years, 6 days, and 17 leap days.
  
  For __unix,
  The unix (local epoch) is 01/01/1970, 00:00:00 UTC.  
  This gap contains a total of 70 years, 17 leap days.

  Note: century years are not leap years, except on multiples of 400.

---------------------------------------------------------------------------*/
#ifdef __unix
#error code not present
#elif defined T_ARM

#define EPOCH_OFFSET  2524953613uL

#else 

#define EPOCH_OFFSET  0ul   /* set rel. to appropriate epoch */

#endif

/*---------------------------------------------------------------------------
   EPOCH_DIVERGENCE = today's divergence between NTP and local epoch in s.

   A total of 10 leap seconds between 6/30/1972 and 1/6/1980 need to be
   taken into account. The former date is the date which the first leap 
   second was introduced into the UTC and subsequently into NTP; the latter 
   is the epoch for the GPS time scale.

   As of January 1999, GPS is ahead of UTC by 13 standard seconds.      
---------------------------------------------------------------------------*/
#if defined(__unix) || defined(T_ARM)

#define EPOCH_DIVERGENCE 13

#else /* some other target? */

#define EPOCH_DIVERGENCE 0

#endif



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                      EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION DS_GET_NTP

DESCRIPTION
  This function converts the time stamp returned by ts_get (usually in GPS
  format) to NTP format.

DEPENDENCIES 
  None. 

PARAMETERS
  This function takes two pointers to uint32. Any initial values can be can
  be passed in as they will be overwritten with the NTP value converted from
  the GPS time stamp.

  uint32 *secs   -  Ineteger part of the NTP format time stamp. 
  uint32 *msecs  -  Fraction part of the NTP format time stamp. 

RETURN VALUE
  NTP time stamp is returned via the intPart and fracPart pointers (with each
  pointing to a uint32).

SIDE EFFECTS
  None.
===========================================================================*/
void ds_get_ntp 
(
  uint32 *secs, 
  uint32 *msecs
)
{
  qword gps_ts;           /* temp. storage for time stamp in gps_format    */
  uint64 gps_ts64;        /* uint64 equivalent of the GPS time stamp.      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef __unix
#error code not present
#elif defined T_ARM

  /*-------------------------------------------------------------------------
    What time is it now? (in GPS)
  -------------------------------------------------------------------------*/
  ts_get(gps_ts);

  /*-------------------------------------------------------------------------
    The following macro (as defined in qword.h) converts gps_ts to 
    a native C 64 bit value (unsigned long long)
  -------------------------------------------------------------------------*/
  gps_ts64 = QW_CVT_Q2N(gps_ts);    

  MSG_MED("GPS TIME IS %x %x", 
          ((uint32 *) &gps_ts64)[1], 
          ((uint32 *) &gps_ts64)[0], 
          0);

  /*-------------------------------------------------------------------------
    convert chips to seconds and fractional seconds.
  
    25 x 2^21 chips = 1 second
  -------------------------------------------------------------------------*/
  *secs  = (uint32) ((gps_ts64 >> 21) / 25) + EPOCH_OFFSET - EPOCH_DIVERGENCE;
  *msecs = (uint32) (gps_ts64 % (25 * (2 << 21)));

#else

  /*-------------------------------------------------------------------------
    For other targets, code must be added:
    gps_ts64 is the number of ms since GPS's epoch.
  -------------------------------------------------------------------------*/

  #error Epoch not defined for this target! 

#endif

  MSG_MED ("NTP time is %x.%x", *secs, *msecs, 0);

} /* ds_get_ntp() */

#endif /* FEATURE_DS || FEATURE_DATA_PS */

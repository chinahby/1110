/* =======================================================================

                          qtv_utils.cpp

DESCRIPTION
  Definition of QTV utility classes.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/common/main/latest/src/qtv_utils.cpp#2 $
$DateTime: 2009/03/18 21:44:35 $
$Change: 866593 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_utils.h"
#include "qtvsystem.h" /* QTV_Malloc & QTV_Free */

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifdef PLATFORM_LTK
#error code not present
#endif
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */
/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION:
  WaitForFlag

DESCRIPTION:
  Waits (via polling) for a flag to be set.

PARAMETERS:
  bool &flag
    The flag we're waiting for.  We'll sleep and poll until the flag
    goes to true (or we hit timeout).

  unsigned long timeout_ms
    The timeout.  If we wait this long and the flag doesn't get set,
    we'll return false.  0 means 'wait forever'.

  unsigned long poll_interval_ms
    How often we'll check the flag, in ms.  Larger numbers will cause
    less CPU load, but will also introduce more jitter.

RETURN VALUE:
  bool
    true if the flag was set, false if we timed out.

SIDE EFFECTS:
  Can cause rapid task-switching if poll_interval_ms is too small.
===========================================================================*/
bool WaitForFlag( bool &flag, 
                  unsigned long timeout_ms, 
                  unsigned long poll_interval_ms )
{
  bool          result      = true;
  unsigned long time_waited = 0;

  while ( !flag )
  {
    if( timeout_ms > 0 && time_waited >= timeout_ms )
    {
      result = false;
      break;
    }

    rex_sleep( poll_interval_ms );
    time_waited += poll_interval_ms;
  }

  return result;
}

// ***************************************************************************
// Difference
//
// Calculate the difference between two unsigned values, with support
// for an arbitrary rollover point.
// ***************************************************************************
int64 Difference( const uint64& a, const uint64& b, const uint64& rolloverAfter )
{
   int64  result;
   int64  abs_result;
   uint64 r;

   r = rolloverAfter + 1;
   // The arithmetic must be done before the cast, or errors will occur
   // if the MSB becomes involved.
   result     = ( int64 )( b - a );
   abs_result = abs( result );
   // It is safe to cast abs_result to uint64 because an absolute value
   // can't be negative.
   if ( ( uint64 )abs_result > r / 2 )
   {
      // It is safe to cast( r - abs_result ) to int64 because we know
      // abs_result > r/2, which means the result of the subtraction can not
      // have the MSB set.  Therefore, no data can be lost in the cast.
      result = ( int64 )( r - ( uint64 )abs_result ) * ( -result / abs_result );
   }
   // no else: if abs_result is small, the original calculation is valid.

   return result;
}

// ***************************************************************************
// Sum
//
// Calculate the sum of two unsigned values, with support
// for an arbitrary rollover point.
// ***************************************************************************
uint64 Sum( const uint64& a, const uint64& b, const uint64& rolloverAfter )
{
   return ( a + b ) % ( rolloverAfter + 1 );
}


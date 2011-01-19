#ifndef RF_ERRRO_H
#define RF_ERROR_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      R F   E R R O R   H A N D L I N G

GENERAL DESCRIPTION
    This file contains declarations for error handling in the RF library.

    Simple error strings can be returned from error conditions with the
    RF_FATAL_ERROR macro.  This should be used sparingly, only at locations that
    are truely errors.  Use of this macro does not eliminate the need to
    gracefully recover from unexpected errors, but can provide a notification
    mechanism when error recovery handlers are invoked.
    
    This file defines RF_ASSERT, a graceful assertion checking module for
    use in the RF driver and RF library.  Assertions can be configured to
    cause fatal errors, or log the error and continue with a non-fatal message.

    The motivation for this module is the importance of catching failures
    in the RF library without fatally halting the software on an error
    condition.  The NV error logging still captures the file and line
    number of the error condition, facilitating debugging.

    The RF library requires finer control over the assertion level than
    the assert.h header file provides.  This assertion routine also
    maintains the timing of the software between fatal and non-fatal
    instances, to the resolution of the cache hit profile.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_error.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/07   et      added global rf_error_flag for Klocwork error handling
03/03/07   dyc     Support for QSC60x5
           bmg     Added comdef.h so NULL will always be defined
           bmg     C++ compatibility fixes
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
06/15/06   zg      Fixed the problem of not including rficap.h
10/11/05   bmg     Initial Revision. 
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rficap.h"
#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern boolean rf_error_flag;

/*===========================================================================

FUNCTION    RF_ERROR

DESCRIPTION
   Will display a fatal error on the mobile display or log non-fatally log
   the error to NV depending on the compile time option RF_HAS_FATAL_ERRORS.
   This function is not intended to be called directly, it should be used
   in the context of a macro that will automatically provide the filename
   and line number.

   RF_FATAL_ERROR() and RF_ASSERT() use this function.
   
PARAMETERS
  Message to be displayed, and the filename and line number on which the
  error occured.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  The phone may halt if RF_HAS_FATAL_ERRORS is defined.

===========================================================================*/
void rf_error( const unsigned int line, const char *file, const char *message );
#ifdef __cplusplus
}
#endif


/*===========================================================================

MACRO    RF_FATAL_ERROR

DESCRIPTION
   Will display a fatal error on the mobile display or log non-fatally log
   the error to NV depending on the compile time option RF_HAS_FATAL_ERRORS.
   
PARAMETERS
  Message to be displayed.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  The phone may halt if RF_HAS_FATAL_ERRORS is defined.

===========================================================================*/
#ifdef RF_HAS_LIBRARY_DEBUGGING

#define RF_FATAL_ERROR( xx_msg ) \
  rf_error( __LINE__, __FILE__, xx_msg )

#else /* RF_HAS_LIBRARY_DEBUGGING */

#define RF_FATAL_ERROR( xx_msg ) \
  rf_error( __LINE__, __FILE__, NULL )

#endif /* RF_HAS_LIBRARY_DEBUGGING */


/*===========================================================================

MACRO    RF_ASSERT

DESCRIPTION
   Test the boolean condition and produce an error message if the condition
   is false.
   
PARAMETERS
  Boolean test condition.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  The phone may halt if RF_HAS_FATAL_ERRORS is defined.

===========================================================================*/
#ifdef RF_HAS_LIBRARY_DEBUGGING

#define RF_ASSERT( xx_exp ) \
  if ( !(xx_exp) )          \
  {                         \
    rf_error( __LINE__, __FILE__, "Assertion " #xx_exp " failed" ); \
  }                         \
  else {}

#else /* RF_HAS_LIBRARY_DEBUGGING */

#define RF_ASSERT( xx_exp ) \
  if ( !(xx_exp) )          \
  {                         \
    rf_error( __LINE__, __FILE__, "RF Assertion failed" ); \
  }                         \
  else {}

#endif /* RF_HAS_LIBRARY_DEBUGGING */

#endif /* RF_ERROR_H */

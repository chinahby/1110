/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      R F   E R R O R   H A N D L I N G

GENERAL DESCRIPTION
    This file contains the external definition of the rf_error function,
    which is used in the RF_ASSERT and RF_FATAL_ERROR macros.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_error.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/14/05   sar     Eliminated lint error.
10/11/05   bmg     Initial Revision. 
===========================================================================*/

#include "err.h"
#include "msg.h"
#include "rf_error.h"

void rf_error( const unsigned int line, const char *file, const char *message )/*lint -esym(715, message) unused */
{
  #ifdef RF_HAS_FATAL_ERRORS
  
  if ( message != NULL)
  {
    err_fatal_core_dump( line, file, message );
  }
  else
  {
    err_fatal_core_dump( line, file, "RF library fatal error" );
  }

  #else /* RF_HAS_FATAL_ERRORS */

#ifndef USES_RELEASE_VERSION
  MSG_SPRINTF_2( MSG_SSID_RF, MSG_LVL_ERROR, "RF library error occurred in file %s at line %d", file, line );
  err_put_log ( line, file ); /*lint !e734*/
#endif
  #endif /* RF_HAS_FATAL_ERRORS */
}


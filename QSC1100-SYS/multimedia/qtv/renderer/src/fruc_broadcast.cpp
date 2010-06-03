/* =======================================================================

                          fruc_broadcast.cpp

DESCRIPTION
  Implementation of the QTV FRUC broadcasts.

Copyright 2005, 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/src/fruc_broadcast.cpp#5 $
$DateTime: 2008/05/08 13:21:55 $
$Change: 656367 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "fruc_broadcast.h"
#include <stdio.h>
#include "assert.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

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
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

  /* ======================================================================
FUNCTION:
  qtv_FRUC_bcast_class::qtv_FRUC_bcast_class

DESCRIPTION:
  Constructor.
  
PARAMETERS:
  FRUCResult - tells the listener if the FRUC was successful or not.

RETURN VALUE:
  None.
===========================================================================*/
qtv_FRUC_bcast_class::qtv_FRUC_bcast_class
( 
  const FRUCResult result
)
: QTV_Broadcast    ( VIDEO_FRUC_RESULT ),
  m_result(result)
{
}

/* ======================================================================
FUNCTION:
  qtv_FRUC_bcast_class::~qtv_FRUC_bcast_class

DESCRIPTION:
  Destructor.
  
PARAMETERS:
  None.
  
RETURN VALUE:
  None.
===========================================================================*/
qtv_FRUC_bcast_class::~qtv_FRUC_bcast_class( void )
{
}

#ifdef QTV_BROADCAST_VERBOSE_DIAG

/* ======================================================================
FUNCTION:
  qtv_FRUC_bcast_class::Describe
  
DESCRIPTION:
  Every broadcast needs to be able to generate a one-line summary
  of itself for diag output.  Returns the number of characters
  of the buffer used.
  
PARAMETERS:
  char* const pBuf
    The buffer into which to place our description
    
  const int bufSiz
    The size of the above buffer.
      
RETURN VALUE:
  int.
    The number of characters (not including the final '\0') which would
    be required to print everything we wanted.  If this number is < bufSiz,
    that means it all fit.

    Conveniently, this is the same as the return code from snprintf.

    Negative numbers mean something went wrong.
    
===========================================================================*/
int qtv_FRUC_bcast_class::Describe
( 
  char* const pBuf, 
  const int bufSiz 
)
{
  int rc       = -1;
  int rc_total = -1;
  static const char* str[] = { "Success", "Outlier Error", "Fatal Error" };

  if ( pBuf != 0 )
  {
    /* Let the base class describe base class stuff first.
    */
    rc       = QTV_Broadcast::Describe( pBuf, bufSiz );
    rc_total = rc;
    if ( rc >= 0 )
    {
#ifndef PLATFORM_LTK
      rc = snprintf( pBuf + rc, bufSiz - rc, " %s", str[m_result] );
      if ( rc >= 0 ) { rc_total += rc; }
      else           { rc_total = rc;  }
#endif /* !PLATFORM_LTK */
    }
  }

  return rc_total;
}

#endif

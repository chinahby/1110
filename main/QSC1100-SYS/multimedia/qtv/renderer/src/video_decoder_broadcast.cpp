/* =======================================================================

                          video_decoder_broadcast.cpp

DESCRIPTION
  Implementation of the QTV video decoder broadcasts.

Copyright 2005, 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/src/video_decoder_broadcast.cpp#5 $
$DateTime: 2008/05/08 13:21:55 $
$Change: 656367 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "video_decoder_broadcast.h"
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
  qtv_VIDEO_DECODE_TIMING_bcast_class::qtv_VIDEO_DECODE_TIMING_bcast_class

DESCRIPTION:
  Constructor.
  
PARAMETERS:
  const unsigned decode_duration
    decode duration = how long the call to vdec_decode took.  This usually
    tells us how long the ARM side of the decode took.

RETURN VALUE:
  None.
===========================================================================*/
qtv_VIDEO_DECODE_TIMING_bcast_class::qtv_VIDEO_DECODE_TIMING_bcast_class
( 
  const unsigned decode_duration,
  const unsigned timestamp,
  const VDEC_ERROR decode_status
)
: QTV_Broadcast    ( VIDEO_DECODE_TIMING ),
  m_decode_duration( decode_duration ),
  m_timestamp( timestamp ),
  m_decode_status(decode_status)
{
}

/* ======================================================================
FUNCTION:
  qtv_VIDEO_DECODE_TIMING_bcast_class::~qtv_VIDEO_DECODE_TIMING_bcast_class

DESCRIPTION:
  Destructor.
  
PARAMETERS:
  None.
  
RETURN VALUE:
  None.
===========================================================================*/
qtv_VIDEO_DECODE_TIMING_bcast_class::~qtv_VIDEO_DECODE_TIMING_bcast_class( void )
{
}

#ifdef QTV_BROADCAST_VERBOSE_DIAG

/* ======================================================================
FUNCTION:
  qtv_VIDEO_DECODE_TIMING_bcast_class::Describe
  
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
int qtv_VIDEO_DECODE_TIMING_bcast_class::Describe
( 
  char* const pBuf, 
  const int bufSiz 
)
{
  int rc       = -1;
  int rc_total = -1;

  if ( pBuf != 0 )
  {
    /* Let the base class describe base class stuff first.
    */
    rc       = QTV_Broadcast::Describe( pBuf, bufSiz );
    rc_total = rc;
    if ( rc >= 0 )
    {
#ifndef PLATFORM_LTK
      rc = snprintf( pBuf + rc, 
                     bufSiz - rc, 
                     " PTS: %d, %ldms", 
                     m_timestamp,
                     m_decode_duration );
      if ( rc >= 0 )
      {
        rc_total += rc;
      }
      else
      {
        rc_total = rc;
      }
#endif /* !PLATFORM_LTK */
    }
  }

  return rc_total;
}

#endif

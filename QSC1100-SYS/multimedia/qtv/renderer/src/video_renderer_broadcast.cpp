/* =======================================================================

                          video_renderer_broadcast.cpp

DESCRIPTION
  Implementation of the QTV video renderer broadcast.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/src/video_renderer_broadcast.cpp#5 $
$DateTime: 2008/05/08 13:21:55 $
$Change: 656367 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "video_renderer_broadcast.h"
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
  QTV_VideoRendererBroadcast::QTV_VideoRendererBroadcast

DESCRIPTION:
  Constructor.
  
PARAMETERS:
  const QTV_Broadcast::Type type
    The type of this broadcast.
  const Subtype             subtype
    The subtype of this broadcast.
  const VDEC_FRAME*         pFrame
    The frame whose final disposal we are announcing.
  const long                timeliness
    The timeliness of that disposal.

RETURN VALUE:
  None.
===========================================================================*/
QTV_VideoRendererBroadcast::QTV_VideoRendererBroadcast
(
  const QTV_Broadcast::Type type,
  const Subtype             subtype,
  const VDEC_FRAME         &frame,
  const long                timeliness
)
: QTV_Broadcast            ( type ),
  m_subtype                ( subtype ),
  m_timeliness             ( timeliness )
{
  memcpy( &m_frame, &frame, sizeof( VDEC_FRAME ) );
  m_frame.pBuf = NULL; // This isn't guaranteed to be good once broadcasts
                       // are received, so we're NULLing it to prevent
                       // misunderstanding and hijinks.
}

/* ======================================================================
FUNCTION:
  QTV_VideoRendererBroadcast::~QTV_VideoRendererBroadcast

DESCRIPTION:
  Destructor.
  
PARAMETERS:
  None.
  
RETURN VALUE:
  None.
===========================================================================*/
QTV_VideoRendererBroadcast::~QTV_VideoRendererBroadcast( void )
{
}

#ifdef QTV_BROADCAST_VERBOSE_DIAG

/* ======================================================================
FUNCTION:
  QTV_VideoRendererBroadcast::Describe
  
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
int QTV_VideoRendererBroadcast::Describe( char* const pBuf, const int bufSiz )
{
  static const char* const aSubtypesAsText[ NUM_SUBTYPES - FIRST_SUBTYPE ] /*lint!e656 */= 
  {
    "none",
    "avsync",
    "render fail",
  };

  int rc      ( -1 );
  int rc_total( -1 );
  
  if ( pBuf )
  {
    // Let the base class describe base class stuff first.
    
    rc = QTV_Broadcast::Describe( pBuf, bufSiz );

    // use the rest of the string to describe our specialized stuff
    if ( rc < 0 )
    {
      rc = 0; // If the base class had an error, the string is all ours.
    }

    rc_total = rc;
    
#ifndef PLATFORM_LTK
    rc = snprintf( pBuf + rc, 
                   bufSiz - rc, 
                   ":%s [L %u TS %ld] %ldms", 
                   aSubtypesAsText[ m_subtype - FIRST_SUBTYPE ]/*lint!e656 */,
                   m_frame.layer, m_frame.timestamp,
                   m_timeliness );
#endif /* !PLATFORM_LTK */

    rc_total += rc;
  }

  return rc_total;
}

#endif /* QTV_BROADCAST_VERBOSE_DIAG */

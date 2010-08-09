#ifndef QTV_VIDEO_DECODER_BROADCAST_H
#define QTV_VIDEO_DECODER_BROADCAST_H
/* =======================================================================

                          video_decoder_broadcast.h

DESCRIPTION
  Definition of the broadcast types generated by the video decoder.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/inc/video_decoder_broadcast.h#1 $
$DateTime: 2008/05/09 08:14:38 $
$Change: 657007 $

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
#include "broadcast.h"
#include "vdecoder_types.h"
#include "qtv_utils.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

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
**                        Class & Function Declarations
** ======================================================================= */
class qtv_VIDEO_DECODE_TIMING_bcast_class : public QTV_Broadcast
{
public:
  qtv_VIDEO_DECODE_TIMING_bcast_class( const unsigned decode_duration, 
                                       const unsigned timestamp,
                                       const VDEC_ERROR decode_status );
  virtual ~qtv_VIDEO_DECODE_TIMING_bcast_class( void );

  #ifdef QTV_BROADCAST_VERBOSE_DIAG

  virtual int Describe( char* const pBuf, const int bufSiz );

  #endif /* QTV_BROADCAST_VERBOSE_DIAG */

  /* decode duration = how long the call to vdec_decode took.  This usually
  ** tells us how long the ARM side of the decode took.
  */
  const unsigned m_decode_duration;

  /* timestamp = the presentation timestamp of the frame that this message
  ** refers to.  Used to uniquely associate messages with their frames.
  */
  const unsigned m_timestamp;

  /* decode Status = Tells whether the decode was successful or not.
  */
  const VDEC_ERROR m_decode_status;
};

#endif // QTV_VIDEO_DECODER_BROADCAST_H

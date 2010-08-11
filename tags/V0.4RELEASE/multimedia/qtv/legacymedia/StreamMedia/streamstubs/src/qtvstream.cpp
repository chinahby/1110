/* =======================================================================
         qtvstream.cpp
DESCRIPTION
   This module defines the qtvstream class.

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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/src/qtvstream.cpp#3 $
$DateTime: 2008/07/29 21:17:21 $
$Change: 712884 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */

#include "customer.h"
#include "qtvstreambase.h"




/* real-time millisecond clock function */
uint32 pvGetTickCount()
{
  return 0;
}
uint64 pvCvt2Systime(uint32 time)
{
  QTV_USE_ARG1(time);
  return 0;
}

bool get_next_line(const char *start_ptr, const char * end_ptr, 
                   const char *& line_start, 
                   const char *& line_end)
{
  QTV_USE_ARG2(start_ptr, end_ptr);
  QTV_USE_ARG2(line_start, line_end );
  return false;
}

QtvStream::QtvStream(QtvConfig *configObject, bool *status, 
                     void (*truncationCallback)(void), 
                     SDPInfo *sdpInfo , bool log ) 
{
   QTV_USE_ARG2(configObject,truncationCallback);
   QTV_USE_ARG2(sdpInfo, log);
   *status = false;
}

QtvStream::~QtvStream()
{
}

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif



/* =======================================================================
                              rtprtcp.h
DESCRIPTION
  This file defines constants for the RTP protocol

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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/inc/rtprtcp.h#2 $
$DateTime: 2008/07/11 08:12:20 $
$Change: 700132 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#ifndef RTPRTCP_H
#define RTPRTCP_H

/*
** Includes
*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"

#include "zrex_os.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

const int16 NUM_BYTES_IN_UINT_32 = 4;
const uint8 DEFAULT_RTPRTCP_VERSION = 2;
const int16 RTPRTCP_VERSION_BIT_POSITION = 6;
const int16 RTPRTCP_PAD_FLAG_BIT_POSITION = 5;
const uint16 MAX_STR_COUNT = 256;
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef uint32 RtpSsrc;
typedef uint32 RtpTimeStamp;
typedef uint16 RtpPayloadType;
typedef uint16 RtpSeqType;

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
**                        Class Declarations
** ======================================================================= */
#endif

#ifndef _RTSP_OEM_HEADERS_H_
#define _RTSP_OEM_HEADERS_H_
/* =======================================================================
                               rtsp_oem_headers.h
DESCRIPTION
  Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/inc/rtsp_oem_headers.h#2 $
$DateTime: 2008/07/11 08:12:20 $
$Change: 700132 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

#ifndef _QTVPLAYERAPI_H_
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "QtvPlayer.h"
#endif

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

// # of oem headers which can be defined.  Change max as necessary.
#define OEM_RTSP_HEADER_TABLE_SIZE 10

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

struct OemRtspHeaderEntry
{
   // OR of values in QtvPlayer::AffectedRTSPMethods. 0=unused
   int  affectedMethods;
   char *headerName;
   char *headerValue;
};


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
**                        Function Declarations
** ======================================================================= */
class RtspOemHeaders
{
public:
   // return singleton instance
   static RtspOemHeaders *GetInstance();

   QtvPlayer::OemRtspHeaderResult EditOemRtspHeaders(
          QtvPlayer::OemRtspHeaderCommand whatToDo,
          uint32 whichMethodsAffected, // QtvPlayer::AffectedRTSPMethod,
                                       // dc on delete
          const char *headerName,  // don't care for delete_all
          const char *headerValue  // don't care for delete_all, delete
   );

   
};

#endif /* _RTSP_OEM_HEADERS_H_ */


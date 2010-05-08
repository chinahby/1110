/* =======================================================================
                               rtsp_oem_headers.cpp
DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 2004 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/src/rtsp_oem_headers.cpp#3 $
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
#include "rtsp_oem_headers.h"


/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#

//#define RTSP_HEADER_OVERRIDE_TEST

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
**                          Macro Definitions
** ======================================================================= */

/* -----------------------------------------------------------------------
** Static/private  Data Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/* ======================================================================
FUNCTION
  RtspOemHeaders::GetInstance

DESCRIPTION
   This is a class static function.
   It returns the singleton instance of the class that is supported in
 Qtv.  The instance is created as necessary.
DEPENDENCIES
  None

RETURN VALUE
  The singleton instance, or null.

SIDE EFFECTS
  The singleton rtsp oem headers handler is created as needed.
========================================================================== */
RtspOemHeaders *RtspOemHeaders::GetInstance()
{ 
   return NULL;
}



/* ======================================================================
FUNCTION
  RtspOemHeaders::EditOemRtspHeaders

DESCRIPTION
   This is the main function of this class.  QtvPlayer uses it to
  add and remove entries from the oem rtsp headers list.

DEPENDENCIES
  None

RETURN VALUE
  Result of the requested operation

SIDE EFFECTS
  The oem headers array is updated
========================================================================== */

QtvPlayer::OemRtspHeaderResult RtspOemHeaders::EditOemRtspHeaders(
   QtvPlayer::OemRtspHeaderCommand whatToDo,
   uint32 whichMethodsAffected, // AffectedRTSPMethod, dc on delete
   const char *headerName,  // don't care for delete_all
   const char *headerValue  // don't care for delete_all, delete
)
{
   QTV_USE_ARG2(headerName, headerValue);
   QTV_USE_ARG2(whichMethodsAffected, whatToDo);
   return QtvPlayer::OEM_RTSP_RESULT_OK; 

   
}



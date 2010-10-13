/* =======================================================================
                              zrex_oscl_utils.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
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

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/src/zrex_oscl_utils.cpp#7 $
$DateTime: 2008/05/08 14:44:36 $
$Change: 656482 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer_oscl_utils.h"

//////////////////////////////////////////////////////////////////////
//
// ZUtils
//
////////////////////////////////////////////////////////////////////


/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** GLobal Data Declarations
** ----------------------------------------------------------------------- */
//system time zero.  
static zrex_time_offset_type systemTimeZero={0,0};

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
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION:
  ZUtils::Clock

DESCRIPTION:
  Returns the time

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
   Returns the time

SIDE EFFECTS:
  None.
========================================================================== */
unsigned long ZUtils::Clock()
{
	zrex_time_offset_type timeNow;
  unsigned long msec;

  INTLOCK();
	zrex_get_current_time_offset(&timeNow);

	msec = 1000*(timeNow.seconds - systemTimeZero.seconds) + (timeNow.milliseconds - systemTimeZero.milliseconds);

  INTFREE();
	return msec;
}

/* ======================================================================
FUNCTION:
  ZUtils::InitClock

DESCRIPTION:
  Initalizes the clock

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.
  
SIDE EFFECTS:
  None.
========================================================================== */
void ZUtils::InitClock()
{
	//set system time zero
	zrex_get_current_time_offset(&systemTimeZero);
}

/* ======================================================================
FUNCTION:
  ZUtils::Terminate

DESCRIPTION:
  None.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.
  
SIDE EFFECTS:
  None.
========================================================================== */
void ZUtils::Terminate()
{
	//nothing needed.
}



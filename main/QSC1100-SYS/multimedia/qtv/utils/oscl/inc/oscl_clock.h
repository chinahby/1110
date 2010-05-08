#ifndef __OSCL_CLOCK_H__
#define __OSCL_CLOCK_H__
/* =======================================================================
                               oscl_clock.h
DESCRIPTION
  Initializes and provides Clock objects
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/inc/oscl_clock.h#7 $
$DateTime: 2008/05/08 14:04:55 $
$Change: 656428 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include <stdio.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"
#include <time.h>
#include "qcplayer_oscl_utils.h"
#include <string.h>

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
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===================================================================
CLASS:       OSCL_Clock

DESCRIPTION: 

PUBLIC FUNCTIONS & CONSTRUCTORS:
             OSCL_Clock
      ~OSCL_Clock

PUBLIC DATA:

HOW TO USE:
=====================================================================*/

class OSCL_Clock
{

public:

  OSCL_Clock();
  ~OSCL_Clock();
  int InitializeClock();
  int SetClock(int ClockValue);
  int GetClock();
  uint64 CvtClock2Systime(uint32 time);

private:

  zrex_time_offset_type systemTimeZero;

  int   ClockOffset;    // OSCL_Clock reference time

  bool  IsInitialized;
};


#endif  // End #ifdef __OSCL_CLOCK_H__  

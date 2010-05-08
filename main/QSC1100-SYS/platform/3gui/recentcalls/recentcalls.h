#ifndef RECENTCALLS_H
#define RECENTCALLS_H

/*=============================================================================

FILE: recentcalls.h
 
SERVICES:  External interface to the Recent Calls applet
 
GENERAL DESCRIPTION:
 
PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2003 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/RecentCalls/recentcalls.h#3 $
$DateTime: 2004/05/14 14:02:01 $

                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
04/24/03   kar      Initial revision
 
=============================================================================*/
 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMCFGI.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

////
// If this string is passed to the applet with ISHELL_StartAppletArgs(),
// the applet will start in the Missed Call dialog:
#define RECENTCALLS_MISSED_CALL_SCREEN_STR   "missed call"

////
// If this string is passed to the applet with ISHELL_StartAppletArgs(),
// the applet will start in the Redial dialog:
#define RECENTCALLS_REDIAL_SCREEN_STR        "redial"

//
// Max number of digits in a call history entry 
#ifdef FEATURE_36_DIGIT_DIAL_STRING
#define CALLHISTORY_MAXDIGITS             37
#else
#define CALLHISTORY_MAXDIGITS             33
#endif

//
// Max number of letters of an call history entry 
// (including the NULL byte)
#define CALLHISTORY_MAXLETTERS            25
/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef struct {
  uint8              status;         // incoming/outgoing/missed     
  dword              time_stamp;     // years to seconds
  AECHAR             digits[CALLHISTORY_MAXDIGITS];  
  AECHAR             letters[CALLHISTORY_MAXLETTERS];
  dword              duration;                   // Duration of call in seconds
} CallHistoryEntry;

/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/
boolean RecentCalls_AddEntry (IShell *pShell, CallHistoryEntry *ent);

#endif /* RECENTCALLS_H */


#ifndef __AEECALLHISTORYEXT_H__
#define __AEECALLHISTORYEXT_H__

/*===========================================================================
FILE: AEECallHistoryExt.h

SERVICES: OEM specific extensions to the standard AEECallHistory interface 

GENERAL DESCRIPTION:
  Declaration for additional fields added in OEM Layer. 

PUBLIC CLASSES AND STATIC FUNCTIONS: 

INITIALIZATION AND SEQUENCING REQUIREMENTS


        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEECallHistoryExt.h#3 $
$DateTime: 2007/12/04 12:53:49 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
12/03/07     clm     Fix problems with building OEMPBM.c on targets using older
                     versions of AEECallHistory.h
11/15/07     clm     Add AEECALLHISTORY_FIELD_TECH_TYPE field data value
01/16/04     PA      Initial QCT version
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEECallHistory.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
            
/* Temoprary fix till Call History interface and OAT tests are redefined in
 * BREW 3.1 
 */

// Pre-defined Technology types
// New Tech Types should be coordinated with AEECallHistory.h
#ifndef AEECALLHISTORY_TECH_TYPE_VOICE
#define  AEECALLHISTORY_TECH_TYPE_VOICE             0
#endif // AEECALLHISTORY_TECH_TYPE_VOICE

#ifndef AEECALLHISTORY_TECH_TYPE_VIDEO
#define  AEECALLHISTORY_TECH_TYPE_VIDEO             1
#endif // AEECALLHISTORY_TECH_TYPE_VIDEO

#ifndef AEECALLHISTORY_TECH_TYPE_DATA
#define  AEECALLHISTORY_TECH_TYPE_DATA              2
#endif // AEECALLHISTORY_TECH_TYPE_DATA

#define AEECALLHISTORY_FIELD_USER_DEFINED   (AEECALLHISTORY_FIELD_NONE+0xFF00) 

#define AEECALLHISTORY_FIELD_NUMBER_EXT  (AEECALLHISTORY_FIELD_USER_DEFINED+1)

#ifndef AEECALLHISTORY_FIELD_TECH_TYPE
#define  AEECALLHISTORY_FIELD_TECH_TYPE  (AEECALLHISTORY_FIELD_USER_DEFINED+2)
#endif // AEECALLHISTORY_FIELD_TECH_TYPE
  /* 
   * Null terminated AECHAR string: { *, +, ?, 0-9, #, T, H }
   * In contrast, AEECALLHISTORY_FIELD_NUMBER has char 
   * string with an alphabet of { *, 0-9, #, ','}
   *
   */

#endif


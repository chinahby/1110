#ifndef OEMALARM_PRIV_H
#define OEMALARM_PRIV_H
/*=============================================================================

FILE: OEMAlarm_priv.h

SERVICES:  Interface to provide support for Real Time Clock services to BREW.

GENERAL DESCRIPTION:
 
PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/apps/StaticExtensions/oem/inc/OEMAlarm_priv.h#1 $
  $DateTime: 2005/01/28 14:30:33 $
  $Author: rsomani $
  $Change: 160574 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/21/04   rsb     Moving to OEMAlarm from OEMRTC.
11/02/04   rsb     Initial Revision.

===========================================================================*/

/*===========================================================================

                    INCLUDE FILES

===========================================================================*/

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/


/*===========================================================================

                    FUNCTION DECLARATIONS

===========================================================================*/

extern int OEMAlarm_RegisterISR(void);
extern int OEMAlarm_SetAlarm(uint32 expiration);

#endif /* OEMALARM_PRIV_H */


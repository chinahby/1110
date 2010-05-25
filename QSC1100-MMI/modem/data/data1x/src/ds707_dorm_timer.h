#ifndef DS707_DORM_TIMER_H
#define DS707_DORM_TIMER_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                  D O R M A N T  T I M E R  C O N T R O L

GENERAL DESCRIPTION
  This file contains functions used to handle the dormant timer.  This 
  functionality is also known as the holddown timer.  It prevents a pkt call
  from re-originiating out of dormancy for a time specified by the base
  station.

EXTERNALIZED FUNCTIONS
  DS707_DORM_TIMER_PARSE_SOCM
    Called when a service-option control message is received, requiring the
    dormant timer value to be changed.
    
  DS707_DORM_TIMER_RESET_VAL
    Called when a data session goes null in JCDMA or the SID has changed.
    The holddown timer is set back to it's default value.
    
  DS707_DORM_TIMER_GET_TIMER
    Returns the dormant timer value.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_dorm_timer.h_v   1.1   15 Nov 2002 21:36:16   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_dorm_timer.h#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/02   ak      Updated file header comments.

===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "snm.h"


/*===========================================================================
                                TYPEDEFS
===========================================================================*/
/*---------------------------------------------------------------------------
  Type for the Service Option Control Message for Packet Data Dormant Timer
  Control.
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  byte  cntl;          /* Dormant Timer Control & Field Type               */
  byte  dorm_time;     /* Packet Data Dormant Timer value (in 1/10 sec)    */
} ds707_dorm_timer_socm_msg_type;


/*===========================================================================
                                 EXTERNS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DORM_TIMER_RESET_VAL

DESCRIPTION   This function resets the dormant timer to it's default value.
              If a rex timer is running, it is not affected.

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timer_reset_val(void);

/*===========================================================================
FUNCTION      DS707_DORM_TIMER_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  It is assumed that the message is already known to be a SOCM
              which contains dormant timer info.

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_dorm_timer_parse_socm
(
  ds707_dorm_timer_socm_msg_type  *pzid_socm_msg   /* SOCM with PZID info  */
);

/*===========================================================================
FUNCTION      DS707_DORM_TIMER_GET_TIMER

DESCRIPTION   This function returns the dormant timer value.

DEPENDENCIES  None

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
word ds707_dorm_timer_get_timer(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_DORM_TIMER_H */


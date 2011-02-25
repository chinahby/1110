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

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_dorm_timer.c_v   1.3   15 Nov 2002 21:36:08   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_dorm_timer.c#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/03   ak      Fix for JCDMA dorm timer value.
11/15/02   ak      Updated file header comments.
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_1X

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "ds707.h"
#include "ds707_dorm_timer.h"
#include "msg.h"
#include "snm.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*---------------------------------------------------------------------------
  Mask to get the DORM_CNTL bits.
---------------------------------------------------------------------------*/
#define DORM_TIMERI_CNTL_MASK           0xE0

/*---------------------------------------------------------------------------
  Values of the DORM_CNTL field in the Packet Data Dormant Timer Control
  service option control message.
---------------------------------------------------------------------------*/
#define DORM_TIMERI_CNTL_DISABLE        0x0
#define DORM_TIMERI_CNTL_SET_TIMER      0x20

/*--------------------------------------------------------------------------
  Default Dormant Timer value (in milliseconds).
---------------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_1X
/*--------------------------------------------------------------------------
  DEFAULT_DORM_TIME:
  JCDMA requirement: Change default timer to 5secs. Note, this is in 
  violation of IS-707.
---------------------------------------------------------------------------*/
  #define DORM_TIMERI_TIMER_DEFAULT     5000
#else
  #define DORM_TIMERI_TIMER_DEFAULT     0
#endif /* FEATURE_JCDMA_1X */

/*---------------------------------------------------------------------------
  Packet Data Dormant Timer value (in milliseconds).
---------------------------------------------------------------------------*/
LOCAL word dorm_timeri_val = DORM_TIMERI_TIMER_DEFAULT;


/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DORM_TIMER_RESET_VAL

DESCRIPTION   This function resets the dormant timer to it's default value.
              If a rex timer is running, it is not affected.

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timer_reset_val(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dorm_timeri_val = DORM_TIMERI_TIMER_DEFAULT;
}

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
  ds707_dorm_timer_socm_msg_type  *dorm_socm_msg   /* SOCM with PZID info  */
)
{
  snm_socm_return_type  result = SNM_VALID;  /* return value               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Service option control message for Packet Data Dormant Timer Control.
  -------------------------------------------------------------------------*/
  MSG_MED("Rx'ed Dormant Timer SO Ctl Msg", 0, 0, 0);

  if( (dorm_socm_msg->cntl & DORM_TIMERI_CNTL_MASK) == 
                                                   DORM_TIMERI_CNTL_DISABLE )
  {
    /*-----------------------------------------------------------------------
      Disable BS/MSC control of the dormant timer i.e. reset the timer to
      its default value.
    -----------------------------------------------------------------------*/
    dorm_timeri_val = DORM_TIMERI_TIMER_DEFAULT;
  }
  else if( (dorm_socm_msg->cntl & DORM_TIMERI_CNTL_MASK) == 
                                                 DORM_TIMERI_CNTL_SET_TIMER )
  {
    if( dorm_socm_msg->dorm_time == 0 )
    {
      /*---------------------------------------------------------------------
       The dormant timer value is set to zero => disable BS/MSC control of
       the dormant timer i.e. reset the timer to its default value.
      ---------------------------------------------------------------------*/
      dorm_timeri_val = DORM_TIMERI_TIMER_DEFAULT;
    }
    else
    {
#ifdef FEATURE_JCDMA_1X
      dorm_timeri_val = (word)(dorm_socm_msg->dorm_time * 100);
#else
      /*---------------------------------------------------------------------
        Set the dormant timer value to the greater of the current value, or
        the value specified in the service option control message.
      ---------------------------------------------------------------------*/
      dorm_timeri_val = 
                      MAX(dorm_timeri_val, 
                                     (word)(dorm_socm_msg->dorm_time * 100));
#endif /* FEATURE_JCDMA_1X */
    }
  }
  else
  {
    MSG_MED("Unknown DORM_CNTL value: 0x%x", 
                   (dorm_socm_msg->cntl & DORM_TIMERI_CNTL_MASK) >> 5, 0, 0);
    result = SNM_INVALID_MSG_TYP;
  }

  return(result);
} /* ds707_dorm_timer_parse_socm() */


/*===========================================================================
FUNCTION      DS707_DORM_TIMER_GET_TIMER

DESCRIPTION   This function returns the dormant timer value.

DEPENDENCIES  None

RETURN VALUE  Returns the dormant timer value.

SIDE EFFECTS  None
===========================================================================*/
word ds707_dorm_timer_get_timer(void)
{
  return(dorm_timeri_val);
}
#endif /* FEATURE_DATA_IS707   */


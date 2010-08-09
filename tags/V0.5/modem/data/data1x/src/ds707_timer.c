/*===========================================================================

                        D S 7 0 7 _ T I M E R
GENERAL DESCRIPTION
  This file contains the functionality for the IS-707 specific timers.  These
  are:
    The holddown timer
    The inactivitiy timer (which lets phone go dormant in a packet data call).

EXTERNALIZED FUNCTIONS
  ds707_timer_cb()
    Registered as the callback for the holddown and idle timers.  Based on 
    which timer expired, sends msg to DS task, letting it know of timer 
    expiration.
    
 ds707_timer_init()
   Registers the holddown and idle timer with DS task.  This creates the
   timer in REX and associates the callback with it.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_timer_init() on startup.

 Copyright (c) 2002 - 2009 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_timer.c_v   1.1   19 Nov 2002 18:41:34   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_timer.c#2 $ $DateTime: 2009/05/30 06:04:02 $ $Author: msankar $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/09   ms      Added Support for EPZID Hysteresis.
05/12/04   sy      Added pzid hysteresis timer.
10/01/03   ak      Added pzid delay timer.
11/17/02   ak      Updated file header comments.
02/20/02   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "assert.h"
#include "ds3gtimer.h"
#include "dstask.h"
#include "ds707.h"
#include "ds707_timer.h"
#include "err.h"
#include "rex.h"



/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
   Clocks
---------------------------------------------------------------------------*/  

LOCAL rex_timer_type        ds707_cta_timer;         /* idle timeout timer */
LOCAL rex_timer_type        ds707_holddown_timer;    /* holddown timer     */

#if defined(FEATURE_DS_PZID_HYSTERESIS) || defined(FEATURE_DS_EPZID_HYSTERESIS)
/*-------------------------------------------------------------------------
  Timer used for hysteresis activation timer\ hysteresis timer .
  -------------------------------------------------------------------------*/
LOCAL rex_timer_type        ds707_pzid_hyst_timer;   /* pzid hysteresis    */
#endif /* FEATURE_DS_PZID_HYSTERESIS || FEATURE_DS_EPZID_HYSTERESIS */

LOCAL rex_timer_type        ds707_pzid_delay_timer;  /* pzid delay timer   */


/*===========================================================================
                           INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_TIMER_CB

DESCRIPTION   Callback for the AT+CTA , holddown timer expiration.
              PZID Hysteresis tiemr expiration.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_timer_cb
(
  unsigned long    timer_id
)
{
  ds_cmd_type           *cmd_ptr;                    /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( cmd_ptr = ds_get_cmd_buf() ) == NULL )
  {
    ERR_FATAL( "Can't get cmd buf from DS task", 0, 0, 0 );
  }

  switch ((ds3g_timer_enum_type)timer_id)
  {
    case DS3G_TIMER_CTA:
      cmd_ptr->hdr.cmd_id = DS_707_PKT_CTA_TIMER_EXPIRED_CMD;
      break;

    case DS3G_TIMER_HOLDDOWN:
      cmd_ptr->hdr.cmd_id = DS_707_PKT_HOLDDOWN_TIMER_EXPIRED_CMD;
      break;

    case DS3G_TIMER_PZID_HYSTERESIS:
#if defined(FEATURE_DS_PZID_HYSTERESIS) || defined(FEATURE_DS_EPZID_HYSTERESIS)
      cmd_ptr->hdr.cmd_id = DS_707_PKT_PZID_HYSTERESIS_TIMER_EXPIRED_CMD;
#endif /* FEATURE_DS_PZID_HYSTERESIS */
      break;
    case DS3G_TIMER_PZID_DELAY:
      cmd_ptr->hdr.cmd_id = DS_707_PKT_PZID_DELAY_TIMER_EXPIRED_CMD;
      break;

    default:
      ERR("Bad timer id on callback %d",timer_id,0,0);
      ASSERT(0);
      return;
  }
  
  ds_put_cmd( cmd_ptr );
}


/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_TIMER_INIT

DESCRIPTION   Register 707 timers with 3G DSMGR Timer services.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_timer_init(void)
{
  ds3gi_timer_register(&ds707_cta_timer,
                       &ds707_timer_cb,
                       DS3G_TIMER_CTA);

  ds3gi_timer_register(&ds707_holddown_timer,
                       &ds707_timer_cb,
                       DS3G_TIMER_HOLDDOWN);

#if defined(FEATURE_DS_PZID_HYSTERESIS) || defined(FEATURE_DS_EPZID_HYSTERESIS)
  ds3gi_timer_register(&ds707_pzid_hyst_timer,
                       &ds707_timer_cb,
                       DS3G_TIMER_PZID_HYSTERESIS);
#endif /* FEATURE_DS_PZID_HYSTERESIS ||  FEATURE_DS_EPZID_HYSTERESIS*/
  
  ds3gi_timer_register(&ds707_pzid_delay_timer,
                       &ds707_timer_cb,
                       DS3G_TIMER_PZID_DELAY);
} /* ds707_timer_init() */


#endif /* FEATURE_DATA_IS707 */


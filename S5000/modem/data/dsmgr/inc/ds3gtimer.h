#ifndef DS3GTIMER_H
#define DS3GTIMER_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                        T I M E R   F U N C T I O N S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains wrapper functions for REX timer services. These functions
  may be used by software components within the Data Services Task.

  In order to add a new timer, the timer id must be defined below, the timer
  must be initialized in ds3gi_timer_init(), and logic to handle the timer
  expiration must be added to ds3gi_process_timer_expired_cmd().


  Copyright (c) 2001 by QUALCOMM Incorporated. All Rights Reserved.
  Copyright (c) 2002 by QUALCOMM Incorporated. All Rights Reserved.
  Copyright (c) 2003 by QUALCOMM Incorporated. All Rights Reserved.
  Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gtimer.h_v   1.0   08 Aug 2002 11:19:50   akhare  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/inc/ds3gtimer.h#1 $ $DateTime: 2008/04/11 12:46:53 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/06   squ     fixed a lint warning
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
05/12/04   sy      Added pzid hysteresis timer.
10/01/03   ak      Added PZID delay timer.
07/13/02   atp     Added 707 async timers.
03/05/02   smp     Added definition for DCD wink timer.
07/10/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "dstask.h"
#include "rex.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  List of timer ids across Data.  No enum's as the actual timers are
  defined in the technology-specific modules, and then registered into an
  array that is kept centrally.
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_TIMER_MIN = -1,       /* minimum value - used for bounds checking   */

  /*-------------------------------------------------------------------------
     GENERIC TIMERS
  -------------------------------------------------------------------------*/
  DS3G_TIMER_RING_TE,        /* timer used to ring the laptop              */
  DS3G_TIMER_WINK_DCD,       /* timer used to wink DCD                     */

  /*-------------------------------------------------------------------------
     IS707 TIMERS
  -------------------------------------------------------------------------*/
  DS3G_TIMER_CTA,
  DS3G_TIMER_HOLDDOWN,
  DS3G_TIMER_PZID_HYSTERESIS,
  DS3G_TIMER_PZID_DELAY,

  /*-------------------------------------------------------------------------
     IS707 ASYNC TIMERS
  -------------------------------------------------------------------------*/
  DS3G_TIMER_ASYNC_PTCL_OPENING,

  DS3G_TIMER_MAX             /* maximum value - used for bounds checking   */

} ds3g_timer_enum_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS3GI_TIMER_INIT

DESCRIPTION
  This function initializes the timer services. This function must be called
  once, at Data Services Task startup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_timer_init( void );



/*===========================================================================

FUNCTION DS3GI_TIMER_REGISTER

DESCRIPTION
  Clients register their timer info with timer services.  The passed in
  ID must come from the ds3g_timer_enum_type.  Once registered, the 
  timers cannot be de-registered (though they can be stopped/started).
  
  Registers the timer with REX.  Timer is not started.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_timer_register
(
  rex_timer_type       *timer_ptr,
  rex_timer_cb_type     timer_cb_ptr,
  ds3g_timer_enum_type  timer_id    
);


/*===========================================================================

FUNCTION DS3GI_START_TIMER

DESCRIPTION
  This function starts the specified timer for the specified duration.

DEPENDENCIES
  A valid timer id must be specified, otherwise the timer is not started.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_start_timer
(
  ds3g_timer_enum_type  timer_id,        /* Timer to be started            */
  dword                 duration         /* Timer duration in milliseconds */
);


/*===========================================================================

FUNCTION DS3GI_STOP_TIMER

DESCRIPTION
  This function stops the specified timer.

DEPENDENCIES
  A valid timer id must be specified, otherwise no action is taken.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_stop_timer
(
  ds3g_timer_enum_type  timer_id                    /* Timer to be stopped */
);


/*===========================================================================

FUNCTION DS3GI_PROCESS_TIMER_EXPIRED_CMD

DESCRIPTION
  This function processes the DS_TIMER_EXPIRED_CMD. It determines which timer
  has expired, and calls the appropriate function to handle the timer expiry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_process_timer_expired_cmd
(
  ds_cmd_type  *cmd_ptr                              /* Pointer to command */
);



#endif /* FEATURE_DATA */

#endif /* DS3GTIMER_H */

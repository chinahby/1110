/*==========================================================================
Description
  Target-specific diagnostic system routines.  These were moved to a separate 
  file to avoid coupling with MC and other IS95 DMSS components.
    
Copyright (c) 2001-2008 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagmm.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/06   as      Fixed critical lint errors.
10/31/05   as      Fixed lint errors.
02/23/05   as      Included srchmc.h to prevent compiler warnings
01/07/03   djm     add mc set digital offline function for WPLT use, mode change
01/08/03   ma     featurization for GSM PLT
12/08/02   djm     corrected WPLT featurization issue
11/20/01   as      Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "diagtarget.h"
#include "diag.h"
#include "diagi.h"
#include "log.h"
#include "msg.h"
#ifndef FEATURE_WINCE
#include "tmc.h"
#endif
#include "cm.h"
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "diagnv.h"

#ifdef T_SLEEP
#include "sleep.h"
#endif

#ifdef PLATFORM_LTK
#error code not present
#endif

#if defined (FEATURE_IS2000) || defined (FEATURE_HWTC)
#include "db.h"
#endif

#if defined (T_SLEEP) && defined (FEATURE_IS2000)
#include "srchmc.h"
#endif


/*===========================================================================

FUNCTION DIAG_OFFLINE

DESCRIPTION
  This function performs processing required when the diag task receives
  a signal to go offline.  This processing consists of clearing the signal
  DIAG_OFFLINE_COMPLETE_SIG and acknowledging mc's offline signal.

===========================================================================*/
void diag_offline (void)
{
  MSG_LOW("Got offline state change", 0L, 0L, 0L );

  /* Notify the task manager that DIAG has gone offline.  DIAG doesn't 
     actually do anything differnt, but it must ACK the offline
     command nonetheless. */
#ifndef FEATURE_WINCE
  task_offline ();
#endif
} /* diag_offline */



/*===========================================================================

FUNCTION DIAG_READY_START_TASK

DESCRIPTION
  This procedure is called by diag when it is ready to start its task.
  Diag will wait on DIAG_TASK_START_SIG (0x8000) after this call.

===========================================================================*/
void diag_ready_start_task (void)
{
  MSG_LOW("DIAG task is started", 0, 0, 0);
#ifndef FEATURE_WINCE
  /* Instruct the task controller service to start this task */
  task_start (DIAG_RPT_TIMER_SIG, DOG_DIAG_RPT, &diag_rpt_timer);
#endif

  return;
}



/*===========================================================================

FUNCTION DIAG_SHUTDOWN

DESCRIPTION
  This procedure performs shutdown processing for the Diagnostic task.

  After this procedure is called the Diagnostic task will not be running.  
 
===========================================================================*/
void diag_shutdown (void) 
{
   MSG_LOW("Diag shutdown state change", 0L, 0L, 0L);

   (void) rex_clr_sigs (&diag_tcb, TASK_STOP_SIG);

#ifndef FEATURE_WINCE
   /* Instruct the task controller service to shut down the system. */
   task_stop ();
#endif
} /* diag_shutdown */



/*===========================================================================

FUNCTION DIAG_POWERDOWN

DESCRIPTION
  This procedure sends the command to MC to power down the phone.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The phone must powerdown as a result of this call.
 
===========================================================================*/
void diag_powerdown (void) 
{
#ifndef FEATURE_GSM_PLT 
#if defined (T_SLEEP) && defined (FEATURE_IS2000)
    /* If we could be sleeping, wake up search, which makes sure the
    ** rest of the phone, namely MC, wakes up and responds to our request
    ** in a timely fashion.
    */
    srch_rude_wakeup( FALSE );
#endif
#if !defined(FEATURE_HWTC) && !defined(FEATURE_WCDMA_PLT)
    (void) cm_ph_cmd_oprt_mode(
                        NULL,
                        NULL,
                        CM_CLIENT_ID_ANONYMOUS,         /* Dummy client ID */
                        SYS_OPRT_MODE_PWROFF
                       );
#endif /* FEATURE_HWTC */
#endif
} /* diag_powerdown */




#ifdef FEATURE_WCDMA_PLT
#error code not present
#endif /* FEATURE_WCDMA_PLT */


/*===========================================================================

FUNCTION DIAG_KICK_WATCHDOG

DESCRIPTION
  This procedure reports to the watchdog and resets the timer.

===========================================================================*/
void diag_kick_watchdog(void)
{
#ifndef FEATURE_WINCE
  dog_report(DOG_DIAG_RPT);
  (void) rex_set_timer( &diag_rpt_timer, DOG_DIAG_RPT_TIME );
#else
#error code not present
#endif /* FEATURE_WINCE */
}



/*=========================================================================*/
void diag_allow_sleep (void)
{
#ifdef T_SLEEP

  (void) rex_set_sigs (&sleep_tcb, SLEEP_DIAG_OKTS_SIG);

  /* Backward compatibile until SLEEP VUs sync up */
  #ifdef SLEEP_LOG_OKTS_SIG
    (void) rex_set_sigs (&sleep_tcb, SLEEP_LOG_OKTS_SIG);
  #endif
  
#endif
}



/*=========================================================================*/
void diag_forbid_sleep(void)
{
#ifdef T_SLEEP

  (void) rex_clr_sigs (&sleep_tcb, SLEEP_DIAG_OKTS_SIG);

  /* Backward compatibile until SLEEP VUs sync up */
  #ifdef SLEEP_LOG_OKTS_SIG
    (void) rex_clr_sigs (&sleep_tcb, SLEEP_LOG_OKTS_SIG);
  #endif

#endif
}

/*lint +esym(715,pkt_len,req_ptr) */



#if defined (FEATURE_OTASP) || defined (FEATURE_UIM_RUIM)
/*===========================================================================

FUNCTION DIAG_OTASP_UPDATE

DESCRIPTION
  This procedure tells DIAG to refresh its status cache from NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void diag_otasp_update( void )
{
  (void) rex_set_sigs( &diag_tcb, DIAG_REFRESH_CACHE_SIG );
} /* diag_otasp_update */
#endif /* defined (FEATURE_OTASP) || defined (FEATURE_UIM_RUIM) */

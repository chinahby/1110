/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   M A I N    R E C E I V E    T A S K

GENERAL DESCRIPTION
  This module contains the Main Receive task initialization and
  startup processing.  It also contains the main processing loop
  for the Main Receive task.

EXTERNALIZED FUNCTIONS
  rx_task
    This is the entrance procedure for the Main Receive task.
    It contains the main processing loop for the task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The rx_task function contains the Main Receive task.  It must be
  specified in a call to rex_def task and started by the Main Control
  task.

Copyright (c) 1992-2005 through 2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/rx.c_v   1.2   06 Sep 2002 09:25:06   varavamu  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/rx.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/07   pg      Update to new sleep voting mechanism - removal of 
                   SLEEP_RXC_OKTS_SIG.
07/11/05   fh      Cleaned up header file including from MUX:
                     changed rxc.h to rxcmc.h
04/08/05   vlc     Set SLEEP_RXC_OKTS_SIG before entering RX task for loop.
09/11/03   yll     Added the featurization for ACP include files.
06/02/03   bkm     Lint cleanup.
09/06/02   va      Cleaned up lint errors.
06/07/02   jqi     Updated with multimode task operation interface.
03/06/00   ry      Added IS2000 featurization
01/17/00   lcc     Corrected message level to take care of warning during
                   compilation.
05/10/99   kmp     Changed T_ACP to FEATURE_ACP
12/12/95   jca     When CMD_Q sigs set we're now making sure there's an item
                   on the cmd_q before passing control to ACP/CDMA subtasks.
08/02/94   jca     Added T_ACP conditional compiles to support ISS1 release.
01/06/94   jah     Changed watchdog iface to use dog_report().
09/01/92   jai     Uncommented calls to rxc_subtask and rxc_powerup_init.
07/09/92   jca     Added processing for TASK_STOP_SIG and TASK_OFFLINE_SIG.
07/07/92   arh     Updated to new MC.H. Checked into PVCS for DMSS use.
04/16/92   jca     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef T_MSM5000
/* This file uses only up to MSG_MED but compilation rule uses MSG_HIGH.  So let's
  redefine the MSG_LEVEL to reflect what's in this file to prevent the warning.
  This must be placed before msg.h */
#undef MSG_LEVEL
#define MSG_LEVEL MSG_LVL_MED /* Define this to the highest used in this file */
#endif /* T_MSM5000 */

#include "rex.h"
#include "task.h"
#include "dog.h"
#include "rx.h"
#include "rxcmc.h"
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#include "msg.h"
#include "mc.h"
#include "target.h"
#include "sleep.h"

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

LOCAL rex_timer_type rx_rpt_timer;  /* Watchdog timer */

#define RX_WATCHDOG_REPORT( ) \
  dog_report(DOG_RX_RPT); \
  (void) rex_set_timer( &rx_rpt_timer, DOG_RX_RPT_TIME )

#define RX_CLEAR_WATCHDOG_TIMER( ) \
  (void) rex_clr_timer( &rx_rpt_timer ); \
  (void) rex_clr_sigs ( &rx_tcb, RX_RPT_TIMER_SIG )

/* <EJECT> */
/*===========================================================================

FUNCTION RX_TASK

DESCRIPTION
  This procedure is the entrance procedure for the Main Receive task.
  It contains the main processing loop for the Main Receive task which
  controls the activation of the ACPRX and RXC subtasks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rx_task (
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
)
{

  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by rex */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------*/
  /* Perform initialization */
  /*------------------------*/
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  rxc_powerup_init();

  /*------------------*/
  /* Initialize timer */
  /*------------------*/
  rex_def_timer( &rx_rpt_timer, &rx_tcb, RX_RPT_TIMER_SIG );

  /*--------------------------------------------------*/
  /* Process task startup procedure from task         */
  /* controller.                                       */
  /*--------------------------------------------------*/
  (void) task_start(
    RX_RPT_TIMER_SIG,             /* report timer signal for task */
    DOG_RX_RPT,                   /* watchdog report */
    &rx_rpt_timer                 /* pointer to report timer */
  ); /*lint !e792*/

  /*--------------------------------------------------------------------*/
  /* Initially kick watchdog and set timer for watchdog report interval */
  /*--------------------------------------------------------------------*/
  RX_WATCHDOG_REPORT( );

  for (;;) { /* Never exit this loop... */

#ifdef FEATURE_ACP
#error code not present
#else
    rex_signals_mask =
      rex_wait( RX_RPT_TIMER_SIG | RX_CDMA_CMD_Q_SIG |
                TASK_OFFLINE_SIG | TASK_STOP_SIG );
#endif

    if ((rex_signals_mask & RX_RPT_TIMER_SIG) != 0) {
      /*-------------------------------*/
      /* Kick watchdog and reset timer */
      /*-------------------------------*/
      RX_WATCHDOG_REPORT( );
    }

    /*---------------------------------------------------------*/
    /* Check if powerdown command signal was set.  If set then */
    /* clear signal, process task stop procedure, and proceed. */
    /*---------------------------------------------------------*/
    if ((rex_signals_mask & TASK_STOP_SIG) != 0) {
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &rx_tcb, TASK_STOP_SIG );
      (void) task_stop(); /*lint !e792*/
    }

    /*-------------------------------------------------------*/
    /* Check if offline command signal was set.  If set then */
    /* clear signal, process task offline procedrue, and     */
    /*-------------------------------------------------------*/
    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0) {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &rx_tcb, TASK_OFFLINE_SIG );
      (void) task_offline(); /*lint !e792*/
    }

    /*------------------------------------------------------------------*/
    /* The (analog or CDMA) MC subtask indicates it wishes to acvtivate */
    /* the (analog or CDMA) subtask by setting the command queue signal */
    /* (via a call to acprx_cmd() or rxc_cmd()).                        */
    /*------------------------------------------------------------------*/
    if ((rex_signals_mask & RX_CDMA_CMD_Q_SIG) != 0)
    {
      if (q_cnt( &rxc_cmd_q) != 0)
      {
        /*--------------------------------------------------------*/
        /* Clear watchdog timer before passing control to subtask */
        /*--------------------------------------------------------*/
        RX_CLEAR_WATCHDOG_TIMER( );

        /*-----------------------------------*/
        /* Activate the CDMA Receive subtask */
        /*-----------------------------------*/
        MSG_LOW( "Entering rxc_subtask", 0,0,0 );

        rxc_subtask();

        MSG_LOW( "Exiting rxc_subtask", 0,0,0 );

        /*-------------------------------*/
        /* Kick watchdog and reset timer */
        /*-------------------------------*/
        RX_WATCHDOG_REPORT( );
      }
      else
      {
        /* ---------------------------------------------------------
        ** Signal is set but there's nothing on the queue - continue
        ** to spin in wait loop
        ** --------------------------------------------------------- */
        (void) rex_clr_sigs( &rx_tcb, RX_CDMA_CMD_Q_SIG );
      }
    } /* end if ((rex_signals_mask & RX_CDMA_CMD_Q_SIG) != 0) */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  } /* end for (;;) */
} /* end rx_task */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   M A I N    T R A N S M I T     T A S K

GENERAL DESCRIPTION
  This module contains the Main Transmit task initialization and
  startup processing.  It also contains the main processing loop
  for the Main Transmit task.

EXTERNALIZED FUNCTIONS
  tx_task
    This is the entrance procedure for the Main Transmit task.
    It contains the main processing loop for the task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The tx_task function contains the Main Transmit task.  It must be
  specified in a call to rex_def task and started by the Main Control
  task.

Copyright (c) 1992-2005,1994 by QUALCOMM Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/tx.c_v   1.2   06 Sep 2002 09:25:22   varavamu  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/tx.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/05   fh      Cleaned up header file including from MUX:
                     changed txc.h to txcmc.h
09/11/03   yll     Added the featurization for ACP include files.
06/02/03   bkm     Lint cleanup.
09/06/02   va      Cleaned up lint errors.
06/07/02   jqi     Updated with multimode task operation interface.
03/06/00   ry      Added IS2000 featurization
01/17/00   lcc     Corrected message level to take care of warning during
                   compilation.
05/10/99   kmp     Changed T_ACP to FEATURE_ACP
08/02/94   jca     Added T_ACP conditional compiles to support ISS1 release.
01/06/94   jah     Changed watchdog iface to use dog_report().
09/01/92   jai     Uncommmented calls to txc_subtask and txc_powerup_init.
07/27/92   jca     Added "break" in "for" loop to facilitate PC unit testing.
07/09/92   jca     Added processing for TASK_STOP_SIG and TASK_OFFLINE_SIG.
07/07/92   arh     Updated for new mc.h. Checked into PVCS for DMSS use.
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

#include "comdef.h"
#include "target.h"
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "tx.h"
#include "txcmc.h"
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#include "msg.h"
#include "mc.h"

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

LOCAL rex_timer_type tx_rpt_timer;  /* Watchdog timer */

#define TX_WATCHDOG_REPORT( ) \
  dog_report(DOG_TX_RPT); \
  (void) rex_set_timer( &tx_rpt_timer, DOG_TX_RPT_TIME )

#define TX_CLEAR_WATCHDOG_TIMER( ) \
  (void) rex_clr_timer( &tx_rpt_timer ); \
  (void) rex_clr_sigs ( &tx_tcb, TX_RPT_TIMER_SIG )

/* <EJECT> */
/*===========================================================================

FUNCTION TX_TASK

DESCRIPTION
  This procedure is the entrance procedure for the Main Transmit task.
  It contains the main processing loop for the Main Transmit task which
  controls the activation of the ACPTX and TXC subtasks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tx_task
(
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
  txc_powerup_init();

  /*------------------*/
  /* Initialize timer */
  /*------------------*/
  rex_def_timer( &tx_rpt_timer, &tx_tcb, TX_RPT_TIMER_SIG );

  /*-----------------------------------------------------*/
  /* Process task startup procedure from task controller */
  /*-----------------------------------------------------*/
   task_start(
    TX_RPT_TIMER_SIG,             /* report timer signal for task */
    DOG_TX_RPT,                   /* watchdog report signal */
    &tx_rpt_timer                 /* pointer to report timer */
  );

  /*--------------------------------------------------------------------*/
  /* Initially kick watchdog and set timer for watchdog report interval */
  /*--------------------------------------------------------------------*/
  TX_WATCHDOG_REPORT( );

  for (;;) { /* Never exit this loop... */

#ifdef FEATURE_ACP
#error code not present
#else
    rex_signals_mask =
      rex_wait( TX_RPT_TIMER_SIG | TX_CDMA_CMD_Q_SIG |
                TASK_OFFLINE_SIG | TASK_STOP_SIG );
#endif

    if ((rex_signals_mask & TX_RPT_TIMER_SIG) != 0) {
      /*-------------------------------*/
      /* Kick watchdog and reset timer */
      /*-------------------------------*/
      TX_WATCHDOG_REPORT( );
    }

    /*---------------------------------------------------------*/
    /* Check if powerdown command signal was set.  If set then */
    /* clear signal, process task stop procedure, and proceed. */
    /*---------------------------------------------------------*/
    if ((rex_signals_mask & TASK_STOP_SIG) != 0) {
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &tx_tcb, TASK_STOP_SIG );
      task_stop();

      #if (TG==T_PC)
#error code not present
      #endif
    }

    /*-------------------------------------------------------*/
    /* Check if offline command signal was set.  If set then */
    /* clear signal, process task offline procedure, and     */
    /* proceed.                                              */
    /*-------------------------------------------------------*/
    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0) {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &tx_tcb, TASK_OFFLINE_SIG );
      task_offline();
    }

    /*------------------------------------------------------------------*/
    /* The (analog or CDMA) MC subtask indicates it wishes to acvtivate */
    /* the (analog or CDMA) subtask by setting the command queue signal */
    /* (via a call to acptx_cmd() or txc_cmd()).                        */
    /*------------------------------------------------------------------*/
    if ((rex_signals_mask & TX_CDMA_CMD_Q_SIG) != 0)
    {
      /*--------------------------------------------------------*/
      /* Clear watchdog timer before passing control to subtask */
      /*--------------------------------------------------------*/
      TX_CLEAR_WATCHDOG_TIMER( );

      /*------------------------------------*/
      /* Activate the CDMA Transmit subtask */
      /*------------------------------------*/
      MSG_LOW( "Entering txc_subtask", 0,0,0 );

      txc_subtask();

      MSG_LOW( "Exiting txc_subtask", 0,0,0 );

      /*-------------------------------*/
      /* Kick watchdog and reset timer */
      /*-------------------------------*/
      TX_WATCHDOG_REPORT( );
    } /* end if ((rex_signals_mask & TX_CDMA_CMD_Q_SIG) != 0) */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  } /* end for (;;) */
} /* end tx_task */

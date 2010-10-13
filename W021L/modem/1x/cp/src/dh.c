/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          M A I N    D I F F I E - H E L M A N (D H)   T A S K

GENERAL DESCRIPTION
  This module handles the exponentiation function required for Diffie-Helman
  key exchange specified in IS-683A. It also has a random number generator
  that generates a 'true' random number. MD5 algorithm is used for hashing
  and the seed for MD5 is obtained from the searcher task that fills a buffer
  with random bits which is the random noise from the radio receiver. The
  time taken to fill the buffer while in the traffic channel is in the order
  of 50ms and the time to generate the random number is in the order of 50ms.

LIMITATIONS
  No other task can use the exponentiation function provided by this task
  when the key exchange is in progress

Copyright (c) 1998-2005 by QUALCOMM Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/dh.c_v   1.1   01 Aug 2002 11:15:14   sjaikuma  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/dh.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/07   pg      More Lint Cleanup.
12/10/04   sb      Fixed lint issues.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
09/23/04   fc      Fixed lint errors.
11/20/02   va      Fixed a compiler warning.
07/25/02   jqi     Updated with task operation interface.
01/20/99   ck      Ifdefed the include file and the call to function apwrx
                   based on the compiler
11/19/98   ck      Changed the dh_wait function's wait mask to include
                   OFFLINE_SIG and STOP_SIG.
10/09/98   ck      Added OFFLINE_SIG and STOP_SIG to the wait mask while
                   waiting for signal from Search to enter the main routine.
                   Changed the MD5 hash to hash on equal sizes of the buffer.
08/25/98   ck      Featurised the Exponentiation module and the interface to
                   this task under FEATURE_DH_EXP
06/25/98   ck      Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#ifdef FEATURE_DH

#include <string.h>
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "msg.h"
#include "err.h"
#include "mc.h"
#include "cai.h"
#include "dh.h"
#include "srchmc.h"
#include "md5.h"
#ifdef FEATURE_DH_EXP
#include "queue.h"
#ifdef T_ARM
#include "apwrx.h"
#else
#include "apwrx86.h"
#endif /* T_ARM */
#endif

/*===========================================================================
             DEFINITIONS AND DATA DECLARATIONS FOR MODULE

      This section contains local definitions for constants, macros, types,
      variables and other items needed by this module.
===========================================================================*/

/* Timer definitions */
rex_timer_type dh_rpt_timer;  /* Watchdog timer */

#ifdef FEATURE_DH_EXP
/* Buffers to be placed on the auth_free_q */
#define                    DH_NUM_CMD_BUFS 3

dh_cmd_type dh_cmd_pool[ DH_NUM_CMD_BUFS ];

/* Queue to hold commands for dh */
q_type          dh_cmd_q;

/* Queue to hold free buffers to be placed on the dh_cmd_q */
q_type          dh_cmd_free_q;

/* Buffer to hold report to be returned to task issuing command */
dh_rpt_type dh_rpt_buf;

#endif /* FEATURE_DH_EXP */

/* Buffer for command to Search task */
srch_cmd_type   dh_srch_buf;

/* stores the 160 byte true random number generated */
byte dh_rand_number[DH_RAND];

#ifdef FEATURE_DH_EXP
/*===========================================================================

FUNCTION DH_PROCESS_COMMAND

DESCRIPTION
  This procedure processes a command sent to the DH task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the command is DH_EXP_F, around 12 seconds of CPU time will be taken
  to do this big calculation.

===========================================================================*/

LOCAL void dh_process_command
(
  dh_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
   dword apwrx[24]; /* considering a maximum of 768 bit prime */
   int i;

   switch(cmd_ptr->hdr.command)
   {
      case DH_EXP_F:

           MSG_MED( "Starting the exponentiation", 0, 0, 0 );

#ifdef T_ARM
           (void) apwrxN(cmd_ptr->exp.prime_bits,
                  cmd_ptr->exp.dh_base,
                  cmd_ptr->exp.dh_exponent,
                  apwrx,
                  cmd_ptr->exp.dh_prime); 
#else
           (void) apwrxN(cmd_ptr->exp.prime_bits,
                  (unsigned int *)cmd_ptr->exp.dh_base,
                  (unsigned int *)cmd_ptr->exp.dh_exponent,
                  (unsigned int *)apwrx,
                  (unsigned int *)cmd_ptr->exp.dh_prime); 
#endif

           MSG_MED( "Finished the exponentiation", 0, 0, 0 );

           /* --------------------------------------------
           ** Report back the result of the exponentiation
           ** ----------------------------------------- */
           if (cmd_ptr->exp.rpt_function != NULL)
           {
               dh_rpt_buf.rpt_type = DH_EXP_R;

               /* Fill in necessary fields of command block */
               dh_rpt_buf.rpt_hdr.task_ptr = NULL;

               /* copy the result into an array of 96 dwords */

               memset(dh_rpt_buf.rpt.exp_result, 0, DH_RESULT);
               for (i = 0 ; i < (cmd_ptr->exp.prime_bits/(int)(sizeof(dword) * 8)) ; i++) /*lint !e573 !e574 !e737*/
                  dh_rpt_buf.rpt.exp_result[i] = apwrx[i];

               /* Queue report for requesting task */
               (*(cmd_ptr->exp.rpt_function))( &dh_rpt_buf );
               MSG_HIGH( "Reporting result of exponentiation", 0, 0, 0 );
           }

      break;

      default:
         ERR("Invalid command received by DH",0,0,0);
      break;
   }

}  /* dh_process_command */
#endif

/*===========================================================================

FUNCTION DH_INIT

DESCRIPTION
  This procedure initializes the queues and timers for DH Task.
  It should be called only once, at powerup time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dh_init( void )
{
#ifdef FEATURE_DH_EXP
  byte i; /* Index into free buffer pool */
#endif

/*- - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------
  ** Initialize timers
  ** ----------------- */
  rex_def_timer( &dh_rpt_timer, &dh_tcb, DH_RPT_TIMER_SIG );

  /* -----------------
  ** Initialize queues
  ** ----------------- */
#ifdef FEATURE_DH_EXP

  (void) q_init( &dh_cmd_q );
  (void) q_init( &dh_cmd_free_q );

  /* Fill dh_free_q */
  for (i = 0; i < DH_NUM_CMD_BUFS; i++)
  {
    dh_cmd_pool[i].hdr.cmd_hdr.done_q_ptr = &dh_cmd_free_q;
    q_put( &dh_cmd_free_q, q_link( &dh_cmd_pool[i],
           &dh_cmd_pool[i].hdr.cmd_hdr.link ) );
  }
#endif
} /* dh_init() */

/*===========================================================================

FUNCTION DH_TASK

DESCRIPTION
  This procedure is the entrance procedure for the DH task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/

void dh_task
(
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
)
{
  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by rex */
#ifdef FEATURE_DH_EXP
  dh_cmd_type *cmd_ptr;            /* Pointer to received command */
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------
  ** Perform initialization
  ** ---------------------- */
  dh_init();

  /* ------------------------------------------------
  ** Process task start procedure.
  ** ------------------------------------------------ */
  task_start(
    DH_RPT_TIMER_SIG,             /* report timer signal for task */
    DOG_DH_RPT,                   /* watchdog report */
    &dh_rpt_timer                 /* pointer to report timer */
  );

  /* ------------------------------------------------------------------
  ** Initially kick watchdog and set timer for watchdog report interval
  ** ------------------------------------------------------------------ */

  DH_DOG_RPT();

  /* Searcher guarantees implicitly that it will fill the buffer as soon
     as it acquires the paging channel. Hence we are guaranteed that
     DH will get the command signal long before we enter the traffic
     channel */

  /* Wait for the signal from searcher */

  (void) dh_wait( DH_SRCH_RAND_SIG );
  (void) rex_clr_sigs( &dh_tcb,  DH_SRCH_RAND_SIG );
  MSG_MED(" DH_SRCH_RAND_SIG received ",0,0,0);

  /* Generate the random number as random bits are available at this point
  ** from searcher and also command the searcher to refresh the bits*/
  generate_rand();

  for (;;)
  {
    /* Never exit this loop... */

#ifdef FEATURE_DH_EXP
    /* If there is already a command on our command queue,  don't wait. */
    if (q_cnt( &dh_cmd_q ) != 0)
    {
      rex_signals_mask = DH_CMD_Q_SIG;
    }
    else
    {
#endif /* FEATURE_DH_EXP */

      rex_signals_mask =
        rex_wait( DH_RPT_TIMER_SIG
#ifdef FEATURE_DH_EXP
                  | DH_CMD_Q_SIG | DH_ABORT_EXP_SIG
#endif /* FEATURE_DH_EXP */
                  | TASK_STOP_SIG | DH_RAND_USED_SIG | TASK_OFFLINE_SIG );

#ifdef FEATURE_DH_EXP
    }
#endif /* FEATURE_DH_EXP */

    if ((rex_signals_mask & DH_RPT_TIMER_SIG) != 0)
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dog_report(DOG_DH_RPT);
      (void) rex_set_timer( &dh_rpt_timer, DOG_DH_RPT_TIME );
    }

    /* -------------------------------------------------------
    ** Check if powerdown command signal was set.  If set then
    ** clear signal, and process task stop procedure.
    ** ------------------------------------------------------- */
    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &dh_tcb, TASK_STOP_SIG );
      /* Process task stop procedure. */
      task_stop();

#ifdef FEATURE_DH_EXP
      /* we want to abort the exponentiation in progress if any
      ** as we are going to powerdown  */
      (void) rex_set_sigs( &dh_tcb, DH_ABORT_EXP_SIG);
#endif /* FEATURE_DH_EXP */

    }

    /* -----------------------------------------------------
    ** Check if offline command signal was set.  If set then
    ** clear signal, and process task offline procedure.
    ** ----------------------------------------------------- */
    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &dh_tcb, TASK_OFFLINE_SIG );
      /* Process task offline procedure. */
      task_offline();
    }

    if ((rex_signals_mask & DH_RAND_USED_SIG) != 0)
    {
      MSG_MED(" RAND_USED_SIG received",0,0,0);
      (void) rex_clr_sigs( &dh_tcb, DH_RAND_USED_SIG );
      /* Waiting on DH_SRCH_RAND_SIG ensures that we do not use the
      buffer while searcher is filling the buffer and that the same
      seed is not used twice */
      (void) dh_wait( (word) DH_SRCH_RAND_SIG );
      (void) rex_clr_sigs( &dh_tcb,  DH_SRCH_RAND_SIG );
      MSG_MED( "DH_SRCH_RAND_SIG received", 0,0,0 );
      generate_rand();
    }

#ifdef FEATURE_DH_EXP

    /* we received an abort signal for exponentiation, but we are
    ** not doing any, so clear it */
    /* this signal must be processed before processing the CMD_Q_SIG */
    if ((rex_signals_mask & DH_ABORT_EXP_SIG) != 0)
    {
       (void) rex_clr_sigs( &dh_tcb,  DH_ABORT_EXP_SIG );
    }

    /*-----------------------------
    ** We have a command to process
    ** ---------------------------- */
    if ((rex_signals_mask & DH_CMD_Q_SIG) != 0)
    {
      (void) rex_clr_sigs( &dh_tcb, DH_CMD_Q_SIG );
      if ((cmd_ptr = (dh_cmd_type*) q_get( &dh_cmd_q )) != NULL)
      {
        dh_process_command( cmd_ptr );

        /* return buffer to free queue */
        if( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
        {
          /* place command on requested queue */
          q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr, &cmd_ptr->hdr.cmd_hdr.link );
        }
        else
        {
          ERR("done_q_ptr was NULL!", 0, 0, 0);
        }
      }
    } /* end if ((rex_signals_mask & DH_CMD_Q_SIG) != 0) */
#endif
  } /* end for (;;) */
} /* end dh_task */

#ifdef FEATURE_DH_EXP
/*===========================================================================

FUNCTION DH_CMD

DESCRIPTION
  The dh_cmd pointed to by the parameter passed in is queued up for DH.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void dh_cmd (
  dh_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
  /* -------------------------------------------------------
  ** All DH commands should return to the free queue.  Set
  ** this field again here to ensure no buffers get lost.
  ** ------------------------------------------------------- */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = &dh_cmd_free_q;

    /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );

  q_put( &dh_cmd_q, &cmd_ptr->hdr.cmd_hdr.link ); /* and queue it */

  (void)rex_set_sigs( &dh_tcb, DH_CMD_Q_SIG );  /* signal the Dh task */

} /*dh_cmd */
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION DH_WAIT

DESCRIPTION
      This function just performs a REX wait as specified by the
      caller, except that it automatically kicks the watchdog timer every
      so often while waiting. It also acknowledges the TASK_STOP_SIG and
      TASK_OFFLINE_SIG when they are set.

DEPENDENCIES
      Relies on DH_RPT_TIMER_SIG being raised often enough to
      avoid watchdog alarms, so should not be called until dh_init has
      run.

RETURN VALUE
      Returns the value returned by rex_wait.

SIDE EFFECTS
      The watchdog gets kicked periodically.

===========================================================================*/

rex_sigs_type  dh_wait
(
  /* Mask of signals to wait for */
  word  sigs
)
{
  /* Return value from REX: bitmap of signals found raised */
  rex_sigs_type  rex_signals_mask;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Keep waiting and kicking the watchdog as long as the return
     value has the kick timer signal set and none of the user's
     desired signals are set. Also process offline or stop procedure
     if TASK_OFFLINE_SIG or TASK_STOP_SIG is set*/

  do
  {
    /* Wait for the caller's conditions OR our kick timer */
    rex_signals_mask = rex_wait ( sigs | DH_RPT_TIMER_SIG | TASK_STOP_SIG |
                                TASK_OFFLINE_SIG );

    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &dh_tcb, TASK_STOP_SIG );
      /* Process task stop procedure. */
      task_stop();
    }
    else if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &dh_tcb, TASK_OFFLINE_SIG );
      /* Process task offline procedure. */
      task_offline();
    }
    else if ((rex_signals_mask & DH_RPT_TIMER_SIG) != 0)
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dog_report(DOG_DH_RPT);
      (void) rex_set_timer( &dh_rpt_timer, DOG_DH_RPT_TIME );
    }
  } while ( (rex_signals_mask & sigs ) == 0x0000 );
  /* Keep looping as long as user's conditions are un-met */

  /* Return the rex_wait return value with user condition(s) set */
  return ( rex_signals_mask );

} /* dh_wait */

/*===========================================================================

FUNCTION GET_RAND_FROM_DH

DESCRIPTION
      This function returns a random number that is generated and stored in
      DH. Successive Calls to this function should not be made in less than
      100ms. This is a limitation and can be reduced by reducing the number
      of bytes of input seed to the generator.

DEPENDENCIES
      None.

RETURN VALUE
      A pointer to the random number.

SIDE EFFECTS
      None.

===========================================================================*/

byte* get_rand_from_dh(void)
{
   static byte * random_number;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   random_number = dh_rand_number;

   /* The RAND_USED_SIG is set to indicate that the random number has been
   ** retrieved and a new number needs to be generated */
   (void) rex_set_sigs(&dh_tcb, DH_RAND_USED_SIG);
   return(random_number);
}

/*===========================================================================

FUNCTION GENERATE_RAND

DESCRIPTION
      This function generates a  random number of length 160 bits using the
      MD5 hash algorithm. MD5 hash outputs a message digest of length 128 bits.
      160 bit random number is obtained by 2 successive iterations of MD5.
      The random bits from the radio noise has the highest entropy and one
      byte is fed as input for every output bit that we need. New seed is
      input each time to MD5 and hence searcher is commanded to collect
      the random bits at the end of each random number generation.

DEPENDENCIES
      None.

RETURN VALUE
      None.

SIDE EFFECTS
      None.

===========================================================================*/

void generate_rand(void)
{

   /* The current context of the MD5 hash algorithm */
   MD5_CTX context;

   /* holds the 128 bit result of MD5 */
   unsigned char digest[MD5_RESULT];

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Use 128 bytes in the buffer to generate a 128 bit random number */

   MSG_MED("Starting the random number generation", 0, 0, 0);
   /* Initialize MD5 with a new context */
   MD5Init(&context);

   /* Update the context with the new message block */
   MD5Update(&context, srch_rand_buf, (SRCH_MAX_RAND_BUF_SIZE / 2));

   /* End the message digest operation */
   MD5Final(digest, &context);

   /* store the result in the upper 128 bits */
   memcpy(dh_rand_number, digest, MD5_RESULT );

   /* Use the next 32 bytes in the buffer to generate a 128 bit
      random number and we need only 32 bits of the result  */

   /* Initialize MD5 with a new context */
   MD5Init(&context);

   /* Update the context with the second message block */
   MD5Update(&context, (srch_rand_buf + (SRCH_MAX_RAND_BUF_SIZE / 2)),
                        (SRCH_MAX_RAND_BUF_SIZE / 2));

   /* End the message digest operation */
   MD5Final(digest, &context);

   MSG_MED("Finished the random number generation", 0, 0, 0);

   /* copy 32 bits of the result into the least significant 32 bits */
   memcpy((dh_rand_number + MD5_RESULT), digest, (DH_RAND - MD5_RESULT));

   MSG_HIGH("Sent SRCH to refresh random bits", 0, 0, 0);

   dh_srch_buf.rand.hdr.cmd = SRCH_RAND_BITS_F;
   dh_srch_buf.rand.hdr.cmd_hdr.task_ptr = NULL;
   dh_srch_buf.rand.hdr.cmd_hdr.done_q_ptr = NULL;
   /* Send Command to Search task */
   srch_cmd( &dh_srch_buf );
}
#endif /*FEATURE_DH */

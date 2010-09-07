#ifndef DH_H
#define DH_H
/*===========================================================================

               E X T E R N A L   D I F F I E - H E L M A N
                      T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

Copyright (c) 1998-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/dh.h_v   1.0.2.0   30 Nov 2001 16:53:36   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/dh.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $


when       who    what, where, why
--------   ---    ----------------------------------------------------------
12/10/04   sb      Fixed lint issue.
10/20/04   an     Replaced inclusion of srch.h with srchmc.h
09/23/04   fc     Fixed lint errors.
01/20/99   ck     Removed set_a_key_temp as it was not being used and also
                  removed extern from functions generate_rand and dh_wait
08/25/98   ck     Featurised the interface routines for the exponentiation 
                  module under FEATURE_DH_EXP
06/25/98   ck     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#ifdef FEATURE_DH
#include "comdef.h"
#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "srchmc.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define DH_BASE                   64
  /* Number of bytes of Diffie-Helman dh_base  */

#define DH_PRIME                  64
  /* Number of bytes of Diffie-Helman dh_prime  */

#define DH_RESULT                 64
  /* Number of bytes of Diffie-Helman result  */

#define DH_RAND                   20
  /* Number of bytes of Random number generated in DH by the random number generator */

#define DH_EXPONENT               64
  /* Number of bytes of Diffie-Helman dh_exponent  */


#define MD5_RESULT                16
  /* Number of bytes of result returned by the MD5 algorithm */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   External declarations for structures declared in mc_cdma.c            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

extern srch_cmd_type  dh_srch_buf;  /* Buffer for command to Search task */

/* Timer definitions */
extern rex_timer_type dh_rpt_timer;  /* Watchdog timer */


/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define  DH_RPT_TIMER_SIG      0x0001
  /* This signal is set when the dh_rpt_timer expires.
     This timer is used to enable DH to kick the watchdog on time */

#define  DH_CMD_Q_SIG          0x0002
  /* This signal is set when an item is put on the dh_cmd_q. */

#define  DH_RAND_USED_SIG      0x0004
  /* This signal is set when the random number has been retrieved from DH*/

#define  DH_SRCH_RAND_SIG      0x0008
  /* This signal is set when search has filled the a-key buffer with random bits */

#define  DH_ABORT_EXP_SIG      0x0010
  /* This signal is set when exponentiation needs to be aborted  */

/*--------------------------------------------------------------------------
                                 MACROS
--------------------------------------------------------------------------*/

/* Kick the watchdog */
#define  DH_DOG_RPT() \
  dog_report(DOG_DH_RPT); \
  (void) rex_set_timer ( &dh_rpt_timer, DOG_DH_RPT_TIME )

/*--------------------------------------------------------------------------
                                  DH REPORT

--------------------------------------------------------------------------*/
/* -------------------------------------------------------------------
** The DH Task optionally sends a report when it completes
** a command which produces a result which would be useful to the
** task which issued the command.
** ------------------------------------------------------------------- */
#ifdef FEATURE_DH_EXP
typedef enum
{
  DH_EXP_R                   /* Diffie Helman exponentiation report*/
} dh_report_code_type;

typedef struct
{
  cmd_hdr_type         rpt_hdr;
    /* Includes the queue links,  pointer to requesting task TCB,
       signal to set on completion,  and done queue. */
  dh_report_code_type rpt_type; /* What type of report this is */
  union
  {
    dword               exp_result[DH_RESULT/sizeof(dword)]; /* Exponentiation result */
  } rpt;
}
dh_rpt_type;
#endif
/*--------------------------------------------------------------------------
                                 DH COMMANDS

--------------------------------------------------------------------------*/
#ifdef FEATURE_DH_EXP

/* Command types */
typedef enum {
  DH_EXP_F           /* Begin exponentiating */
} dh_cmd_name_type;

/* The common Header for all the commands */
typedef struct {
  cmd_hdr_type          cmd_hdr;        /* command header */
  dh_cmd_name_type      command;        /* The command */
} dh_hdr_type;

/* Begin the exponentiation */
typedef struct {                  /* DH_EXP_F */
  dh_hdr_type     hdr;
  int             prime_bits;
  dword           dh_base[DH_BASE/sizeof(dword)];
  dword           dh_exponent[DH_EXPONENT/sizeof(dword)];
  dword           dh_prime[DH_PRIME/sizeof(dword)];
  void            (*rpt_function)( dh_rpt_type * );
} dh_exp_type;

typedef union {
  dh_hdr_type                hdr;     /* Generic header */
  dh_exp_type                exp;     /* DH exponentiation */
} dh_cmd_type;


/* Queue for other tasks to get dh command buffers from */
extern q_type dh_cmd_free_q;

#endif


/* <EJECT> */
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

extern void dh_task
(
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
);

#ifdef FEATURE_DH_EXP

/* <EJECT> */
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
extern void dh_cmd
(
  dh_cmd_type *cmd_ptr                    /* the command to queue up */
);

#endif

/*===========================================================================

FUNCTION DH_WAIT

DESCRIPTION    This function just performs a REX wait as specified by the
      caller, except that it automatically kicks the watchdog timer every
      so often while waiting.

DEPENDENCIES   Relies on DH_RPT_TIMER_SIG being raised often enough to
      avoid watchdog alarms, so should not be called until dh_init has
      run.

RETURN VALUE   Returns the value returned by rex_wait.

SIDE EFFECTS   The watchdog gets kicked periodically.

===========================================================================*/

rex_sigs_type dh_wait
(
  word  sigs
    /* Mask of signals to wait for */
);

/* <EJECT> */
/*===========================================================================

FUNCTION GET_RAND_FROM_DH

DESCRIPTION
      This function returns a random number that is generated and stored in DH

DEPENDENCIES
      None.

RETURN VALUE
      None.

SIDE EFFECTS
      None.

===========================================================================*/

extern byte* get_rand_from_dh(void);

/* <EJECT> */
/*===========================================================================

FUNCTION GENERATE_RAND

DESCRIPTION
      This function generates a  random number

DEPENDENCIES
      None.

RETURN VALUE
      None.

SIDE EFFECTS
      None.

===========================================================================*/

void generate_rand(void);


/* <EJECT> */
/*=========================================================================== */
#endif /* FEATURE_DH */
#endif /* DH_H */


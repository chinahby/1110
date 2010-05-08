/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        C D M A    M A I N    C O N T R O L    S U B T A S K 
            S E A R C H E R  T A S K  I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the queues used for communication
  from the Searcher task to the CDMA Main Control Subtask.
  
EXTERNALIZED FUNCTIONS
  None.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1992-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsrch.c_v   1.0.2.0   30 Nov 2001 19:13:54   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccsrch.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/04   an      Include mc.h and task.h because of srchmc.h inclusion in
                   mccsrch.h
08/02/04   yll     Added the function to allow Search to set signals for MC
                   when sending Search reports.
09/04/98   pms     Neared the variables to save ROM space.
10/31/96   rdh     Increased buffer size from 5 to 10 to prevent buffer
                   starvation leading to dropped calls and/or fatal errors.

07/16/92   ptw     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "queue.h"
#include "qw.h"
#include "mccsrch.h"
#include "task.h"
#include "mc.h"


/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define                        MCCSRCH_NUM_RPTS 10
mccsrch_rpt_type mccsrch_bufs[ MCCSRCH_NUM_RPTS ];
/* Buffers to be placed on the mcc_srch_free_q. Size of this
   buffer must consider the competing pilot situation at call
   origination. In dynamic pilot scenarios, up to 3-4 suppressed
   idle handoffs can generated in system access state, plus the
   number of PSMMs which can be generated (up to 4-5) immediately
   after Search enters Traffic channel state. */

/* Queue definitions */
q_type mcc_srch_q;
  /* Queue holding messages from the Searcher task for the Main control task 
     to process  */

q_type          mcc_srch_free_q;
  /* Queue for free buffers to be placed, when filled, on the 
     mcc_srch_q queue */


/*===========================================================================

FUNCTION MCCSRCH_INIT

DESCRIPTION
  This function initializes the queues between the Searcher task and the CDMA
  Main Control task.  It also places buffers on the free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsrch_init( void )
{
  word i;
    /* index through array of free buffers */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ( void )q_init( &mcc_srch_q );
  ( void )q_init( &mcc_srch_free_q);

  /* Fill mcc_srch_free_q */
  for( i=0; i< MCCSRCH_NUM_RPTS; i++ )
  {
    mccsrch_bufs[i].hdr.rpt_hdr.done_q_ptr = &mcc_srch_free_q;
    q_put( &mcc_srch_free_q, q_link( &mccsrch_bufs[i],
                                &mccsrch_bufs[i].hdr.rpt_hdr.link ));
  }

}/* mccsrch_init */



/*===========================================================================

FUNCTION MCCSRCH_SET_SRCH_RPT_SIGNAL

DESCRIPTION
  This function allows the Search to set a signal for MC when sending a
  Search report.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsrch_set_srch_rpt_signal ( void )
{
  /* Sets the MCC_Q_SIG for MC */
  ( void ) rex_set_sigs ( &mc_tcb, MCC_Q_SIG );
}

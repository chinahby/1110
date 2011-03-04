/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        C D M A    M A I N    C O N T R O L    S U B T A S K 
            R E C E I V E   T A S K  I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the queues used for communication
  from the Receive task to the CDMA Main Control Subtask.
  
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccrx.c_v   1.0.2.0   30 Nov 2001 17:13:56   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccrx.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/98   pms     Neared the variables to save ROM space.
07/20/92   jai     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "queue.h"
#include "mccrx.h"


/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Buffers to be placed on the mcc_rx_free_q */
#define                        MCCRX_NUM_RPT_BUFS 3
mccrx_rpt_type mccrx_rpt_pool[ MCCRX_NUM_RPT_BUFS ];

/* queue for free buffers */
q_type mcc_rx_free_q;

/* Queue for reports from RXC to MCC */
q_type          mcc_rx_rpt_q;




/*===========================================================================

FUNCTION MCCRX_INIT

DESCRIPTION
  This function initializes the queues between the Receive task and the CDMA
  Main Control task.  It also places buffers on the free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccrx_init( void )
{
  word i;
    /* index through array of free buffers */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ( void )q_init( &mcc_rx_rpt_q );
  ( void )q_init( &mcc_rx_free_q );

  /* Fill mcc_rx_free_q */
  for( i=0; i< MCCRX_NUM_RPT_BUFS ; i++ )
  {
    mccrx_rpt_pool[i].hdr.rpt_hdr.done_q_ptr = &mcc_rx_free_q;
    q_put( &mcc_rx_free_q, q_link( &mccrx_rpt_pool[i],
           &mccrx_rpt_pool[i].hdr.rpt_hdr.link ) );
  }

}/* mccrxtx_init */

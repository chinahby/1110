#ifndef DSS_PS_H
#define DSS_PS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    D S    S O C K E T S   P S   T A S K   C M D    H E A D E R   F I L E                                     

GENERAL DESCRIPTION
   This task has the cmd processing for commands sent by the sockets layer
   to PS to be processed.
   
   These commands include commands to WRITE/CONNECT/CLOSE.

  Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/14/02    na/aku   Created.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DS_SOCKETS

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
extern q_type   ps_socket_send_q;

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION DSSPS_NV_INIT()

DESCRIPTION
  Initialization of socket configuration via NV/RUIM in the PS task.

DEPENDENCIES
  Called either during powerup or when the SIM_INIT_COMPLETED event
  is received.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_nv_init
(
  void
);

/*===========================================================================
FUNCTION DSSPS_INIT()

DESCRIPTION
  Powerup initialization of the sockets stuff in the PS task.

DEPENDENCIES
  Called at powerup from the PS context

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_init
( 
  void
);
#endif /* FEATURE_DS_SOCKETS */
#endif /* DSS_PS_H */

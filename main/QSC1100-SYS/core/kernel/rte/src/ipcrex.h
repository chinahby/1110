#ifndef IPCREX_H
#define IPCREX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

   ipc_new()        - Allocate a new message

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/kernel/rte/main/latest/src/ipcrex.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/13   et      Created module.
04/06/05   et      Added rex.h

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rex.h"
#include "ipctypes.h"

#include "pq.h"
#include "ipcdefi.h"



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/
#define IPC_NODE_INFO(a) (&(((rex_tcb_type*)(a))->ipc_info))


/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/


/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/
void ipc_post_msg
(
   ipc_internal_msg_type* msg_ptr,
   struct rex_tcb_struct* tcb_ptr
);


#endif /* IPCREX_H */


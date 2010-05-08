#ifndef IPC_DISPATCH_H
#define IPC_DISPATCH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H E A D E R   F O R
               G E N E R I C  I P C  D I S P A T C H E R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  ipc_dispatch_msg

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/ipc_dispatch.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/06   vdr     Start using KxIPC
08/11/06    gr     Changed ipc_dispatch_func_type definition for backward
                   compatibility.
07/29/06    gr     Initial Revision.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "IxErrno.h"

#include "ixipc.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/

#define IPC_DISPATCH_MAX_FUNCTIONS 16

/*===========================================================================
              TYPE DECLARATIONS
===========================================================================*/

typedef void (*kxipc_dispatch_func_type) (ixipc_msg_type **ipc_msg_ptr);
  

/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/

/*==============================================================
* FUNCTION:  ipc_dispatch_init()
*
* DESCRIPTION
*   Initializes this module.
==============================================================*/
void ipc_dispatch_init (void);

/*==============================================================
* FUNCTION:  ipc_dispatch_register()
*
* DESCRIPTION
*   Registers a dispatch function for a specified service Id.
*   Returns E_SUCCESS on success.
==============================================================*/
IxErrnoType ipc_dispatch_register (kxipc_dispatch_func_type func,
  unsigned int service_id);

/*==============================================================
* FUNCTION:  ipc_dispatch_deregister()
*
* DESCRIPTION
*   Deregisters a dispatch function.
*   Returns E_SUCCESS on success.
==============================================================*/
IxErrnoType ipc_dispatch_deregister (kxipc_dispatch_func_type func);

/*==============================================================
* FUNCTION:  ipc_dispatch_msg()
*
* DESCRIPTION
*   Dispatches an IPC message. The appropriate handler function
*   is called depending on the service id of the IPC message
*   retrieved. Returns E_FAILURE if no handler is found; if a
*   handler is found, returns the return status of the handler
*   function.
==============================================================*/
void ipc_dispatch_msg ( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* IPC_DISPATCH_H */

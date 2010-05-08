#ifndef IPCUTIL_H
#define IPCUTIL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               I P C    E R R O R    S E R V I C E S
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/ipcutil.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/06    gr     Initial Revision.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __cplusplus
extern "C" { 
#endif

#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/


/*===========================================================================
              TYPE DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/


/*===========================================================================
FUNCTION ipc_node_register_self

DESCRIPTION
   Registers the calling thread as an ipc node. This function returns
   TRUE immediately if the node is already registered.

PARAMETERS

RETURN VALUE
  TRUE if the registration succeeded, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
unsigned int ipc_node_register_self( void );

/*===========================================================================
FUNCTION ipc_malloc

DESCRIPTION
    This function will allocate memory for IPC clients/servers from the
    IPC shared memory pool.

PARAMETERS
    size: Size of the memory to be allocated.

RETURN VALUE
    Pointer to the allocated memory.

SIDE EFFECTS
    None
===========================================================================*/
void* ipc_malloc
(
   unsigned int      size
);

/*===========================================================================
FUNCTION ipc_free

DESCRIPTION
    This function will free up memory allocated in the IPC shared memory
    pool.

PARAMETERS
    ptr: Pointer to the allocated memory.

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void ipc_free
(
   void       *ptr
);


/*===========================================================================
FUNCTION ipc_memcpy

DESCRIPTION
    This function copies memory allocated in the IPC shared memory
    pool.

PARAMETERS
    destination: destination pointer to the allocated memory.
	source: source pointer to the allocated memory.
	size: size of copy in bytes

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void* ipc_memcpy
(
   void       *destination,
   const void *source,
   uint32      size
);


#ifdef __cplusplus
}
#endif

#endif /* IPCUTIL_H */

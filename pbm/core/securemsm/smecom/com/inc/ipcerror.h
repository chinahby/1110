#ifndef IPCERROR_H
#define IPCERROR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               I P C    E R R O R    S E R V I C E S
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/ipcerror.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/05    ws     Initial Revision.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/
#define IPC_MSG_LENGTH_MAX       256

/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION IPC_ERROR

DESCRIPTION

PARAMETERS
  message:     Message describing the IPC related error

RETURN VALUE
  none

SIDE EFFECTS
  Notifies potential clients.
===========================================================================*/
#define IPC_ERROR( format, ipc_arg1, ipc_arg2, ipc_arg3 )                                      \
   do {                                                                                        \
      static char ipc_msg[IPC_MSG_LENGTH_MAX];                                                 \
      snprintf( ipc_msg, sizeof(ipc_msg), format, (uint32)(ipc_arg1), (uint32)(ipc_arg2), (uint32)(ipc_arg3));   \
      __ipc_error( __FILE__, __LINE__, ipc_msg );                                              \
   } while (0)                                                                                 

/*===========================================================================

FUNCTION __IPC_ERROR

DESCRIPTION
  Report an IPC related error.
  Do not call directly; use IPC_ERROR instead.

DEPENDENCIES

===========================================================================*/
void __ipc_error( char* filename, long line, char* message);


#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif

#endif /* IPCERROR_H */

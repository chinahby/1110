#ifndef KXIPCUTIL_H
#define KXIPCUTIL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               K X I P C    E R R O R    S E R V I C E S
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/kxipc_util.h#3 $

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "quartz/KxIpc.h"

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
  External function and macro declarations.
---------------------------------------------------------------------------*/

/*===========================================================================
MACRO kxipc_create

DESCRIPTION
  Creates a new IPC message.
===========================================================================*/
#define kxipc_create(mt, ptr, p) \
   __kxipc_create(sizeof(mt), mt##_id, (KxIpcMsgType**) ptr, p)

#define kxipc_create_sized(mt, size, ptr, p) \
   __kxipc_create(size, mt##_id, (KxIpcMsgType**) ptr, p)

/*===========================================================================
MACRO kxipc_send_and_recv

DESCRIPTION
  Blocking call to send an IPC message and wait for the response.
  The response pointer will be NULL if the call fails.
===========================================================================*/
#define kxipc_send_and_recv(s_ptr, node, msg_id, r_ptr) \
   __kxipc_send_and_recv((KxIpcMsgType *) s_ptr, node, msg_id,\
   (KxIpcMsgType **) r_ptr)

/*===========================================================================
FUNCTION __kxipc_create

DESCRIPTION
===========================================================================*/
void __kxipc_create
(
   uint32             size,
   KxIpcMsgIdType   id,
   KxIpcMsgType     **ptr,
   KxIpcMsgPoolType pool
);

/*===========================================================================
FUNCTION __kxipc_send_and_recv

DESCRIPTION
  Blocking call to send an IPC message and wait for the response.
  The response pointer will be NULL if the call fails.
===========================================================================*/
void __kxipc_send_and_recv
(
   KxIpcMsgType   *request,
   uint32            destination,
   KxIpcMsgIdType response_id,
   KxIpcMsgType   **response
);

#ifdef __cplusplus
}
#endif

#endif /* KXIPCUTIL_H */

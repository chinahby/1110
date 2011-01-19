#ifndef IXIPC_H
#define IXIPC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H E A D E R   F O R
                      I X I P C  M O D U L E

GENERAL DESCRIPTION
  This module provides a wrapper layer that routes calls to either KxIPC or
  RTE IPC depending on a feature definition. The function signatures
  closely match those of the functions in the KxIPC API.

  For more information on what the functions do, see kxipc.h or ipc.h.

EXTERNALIZED FUNCTIONS
  ixipc_new
  ixipc_delete
  ixipc_send
  ixipc_receive
  ixipc_message_waiting
  ixipc_sender
  ixipc_init
  ixipc_init_client
  ixipc_get_my_service_id
  ixipc_length

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/ixipc.h#3 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
06/26/07     sam     Featurize include paths for quartz 1.3.5.
12/16/06      rv     Fixed compiler warnings
10/10/06     vdr     Fixed compilation errors
10/25/06      gr     Converted ixipc_service_locate to a macro.
10/25/06      gr     Added ixipc_service_locate.
10/23/06      gr     Added missing functions and macros.
10/21/06      gr     Initial Revision.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "customer.h"
#include "comdef.h"
#include "assert.h"
#include "ipcsvc.h"
#include "ipcutil.h"
#ifdef FEATURE_NATIVE_KXIPC
#include "quartz/KxIpc.h"
#include "kxipc_util.h"
#else
#include "ipc.h"
#include "ipcns.h"
#endif
#include "rex.h"

#ifdef __IPC_DEBUG_MODE__
#include "tracker.h"
extern trackerlist g_ipclist;     /* track ixipc messages */
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
                         TYPE DEFINITIONS
===========================================================================*/

#ifdef FEATURE_NATIVE_KXIPC
typedef KxIpcMsgType    ixipc_msg_type;
typedef KxIpcNodeIdType ixipc_node_id_type;
typedef KxErrnoType     ixipc_errno_type;
#else
union ipc_msg_type {
  uint32 dummy;
};
typedef union ipc_msg_type ixipc_msg_type;
typedef ipc_node_id_type   ixipc_node_id_type;
typedef errno_enum_type    ixipc_errno_type;
#endif /* FEATURE_NATIVE_KXIPC */

typedef enum ixipc_sig_type_e
{
  IXIPC_REX_SIG_TYPE,
  IXIPC_KX_SIG_TYPE,
  IXIPC_REX_KX_SIG_TYPE
} ixipc_sig_type;


/*---------------------------------------------------------------------------
  External macro and function declarations.
---------------------------------------------------------------------------*/

/*===========================================================================
MACRO ixipc_create

DESCRIPTION
  Creates a new IPC message.
===========================================================================*/
#ifdef __IPC_DEBUG_MODE__
  #define ixipc_create(mt, ptr, p) \
    __ixipc_create_debug(sizeof(mt), (ipc_msg_id_type)mt##_id, ptr, p,\
      __FILE__, __LINE__)
#else
  #define ixipc_create(mt, ptr, p) \
    __ixipc_create(sizeof(mt), (ipc_msg_id_type)mt##_id, ptr, p)
#endif

/*===========================================================================
MACRO ixipc_create_var

DESCRIPTION
  Creates a new IPC message of variable length.
===========================================================================*/
#ifdef __IPC_DEBUG_MODE__
  #define ixipc_create_var(mt, len, ptr, p) \
    __ixipc_create_debug(len, (ipc_msg_id_type)mt##_id, ptr,\
      p, __FILE__, __LINE__)
#else
  #define ixipc_create_var(mt, len, ptr, p) \
    __ixipc_create(len, (ipc_msg_id_type)mt##_id, ptr, p)
#endif

/*===========================================================================
MACRO ixipc_send_and_recv

DESCRIPTION
  Blocking call to send an IPC message and wait for the response.
  The response pointer will be NULL if the call fails.
===========================================================================*/
#define ixipc_send_and_recv(s_ptr, node, msg_id, r_ptr) \
   __ixipc_send_and_recv((ixipc_msg_type *) s_ptr, node, msg_id, r_ptr)

/*===========================================================================
MACRO ixipc_send

DESCRIPTION
  Send an IPC message.
===========================================================================*/
#ifdef FEATURE_NATIVE_KXIPC
  #define ixipc_send(msg_ptr, node_id) \
    KxIpc_Send((KxIpcMsgType**)(msg_ptr), (node_id))
#else
  #define ixipc_send(msg_ptr, node_id) \
    ipc_send((union ipc_msg_type**)(msg_ptr), (node_id))
#endif

/*===========================================================================
MACRO ixipc_sender

DESCRIPTION
  Returns the sender of an IPC message.
===========================================================================*/
#ifdef FEATURE_NATIVE_KXIPC
  #define ixipc_sender(msg_ptr) KxIpc_GetSender((KxIpcMsgType*)(msg_ptr))
#else
  #define ixipc_sender(msg_ptr) ipc_sender((union ipc_msg_type*)((void*)msg_ptr))
#endif

#ifdef __IPC_DEBUG_MODE__
/*===========================================================================
MACRO ixipc_new_debug

DESCRIPTION
  Creates a new IPC message.
===========================================================================*/
#define ixipc_new(size, msg_id, pool_id) \
  ixipc_new_debug (size, msg_id, pool_id, __FILE__, __LINE__);
#endif

/*==============================================================
* MACRO ixipc_service_locate
*
* DESCRIPTION
*   Just returns the service id passed in if FEATURE_NATIVE_KXIPC is defined
*   and calls ipcns_service_locate if FEATURE_NATIVE_KXIPC is not defined.
===============================================================*/
#ifdef FEATURE_NATIVE_KXIPC
  #define ixipc_service_locate(service_id)\
    (service_id)
#else
  #define ixipc_service_locate(service_id)\
    ipcns_service_locate ((service_id), TRUE)
#endif

/*==============================================================
* FUNCTION:  ixipc_new()
*
* DESCRIPTION
*   Calls KxIpc_New if FEATURE_NATIVE_KXIPC is defined and
*   ipc_new if FEATURE_NATIVE_KXIPC is not defined.
==============================================================*/
#ifdef __IPC_DEBUG_MODE__
ixipc_msg_type* ixipc_new_debug (uint32 size, ipc_msg_id_type msg_id,
  ipc_msg_pool_type pool_id, const char *filename, unsigned int lineno);
#else
ixipc_msg_type* ixipc_new (uint32 size, ipc_msg_id_type msg_id,
  ipc_msg_pool_type pool_id);
#endif

/*==============================================================
* FUNCTION:  ixipc_receive()
*
* DESCRIPTION
*   Calls KxIpc_Receive if FEATURE_NATIVE_KXIPC is defined and
*   ipc_receive if FEATURE_NATIVE_KXIPC is not defined.
==============================================================*/
ixipc_msg_type* ixipc_receive (ipc_msg_id_type msg_id);

/*==============================================================
* FUNCTION:  ixipc_message_waiting()
*
* DESCRIPTION
*   Calls KxIpc_IsMsgWaiting if FEATURE_NATIVE_KXIPC is defined.
*   Returns FALSE otherwise.
==============================================================*/
boolean ixipc_message_waiting (rex_sigs_type ipc_signal);

/*==============================================================
* FUNCTION:  ixipc_init()
*
* DESCRIPTION
*   Calls KxIpc_Init if FEATURE_NATIVE_KXIPC is defined and
*   ipc_init if FEATURE_NATIVE_KXIPC is not defined.
==============================================================*/
void ixipc_init (ixipc_node_id_type node_id, uint32 receive_sig);

/*==============================================================
* FUNCTION:  ixipc_init_client()
*
* DESCRIPTION
*   Calls KxIpc_Init_Client if FEATURE_NATIVE_KXIPC is defined
*   and ipc_init_client if FEATURE_NATIVE_KXIPC is not defined.
==============================================================*/
void ixipc_init_client (const char* mem_name);

/*==============================================================
* FUNCTION:  ixipc_get_my_service_id()
*
* DESCRIPTION
*   Calls KxIpc_GetServiceId if FEATURE_NATIVE_KXIPC is defined.
*   Returns 0 otherwise.
==============================================================*/
uint32 ixipc_get_my_service_id (void);

/*==============================================================
* FUNCTION:  ixipc_length()
*
* DESCRIPTION
*   Calls KxIpc_GetMsgLength if FEATURE_NATIVE_KXIPC is defined.
*   Returns 0 otherwise.
==============================================================*/
ixipc_node_id_type ixipc_length (ixipc_msg_type* msg_ptr);

/*==============================================================
* FUNCTION:  ixipc_terminate()
*
* DESCRIPTION
*   Calls KxIpc_Terminate if FEATURE_NATIVE_KXIPC is defined and
*   ipcns_node_deregister otherwise.
==============================================================*/
void ixipc_terminate (void);

/*==============================================================
* FUNCTION:  __ixipc_create()
*
* DESCRIPTION
*   Calls __kxipc_create if FEATURE_NATIVE_KXIPC is defined
*   and __ipc_create if FEATURE_NATIVE_KXIPC is not defined.
==============================================================*/
#ifdef __IPC_DEBUG_MODE__
  void __ixipc_create_debug (uint32 size, ipc_msg_id_type id,
    ixipc_msg_type **ptr, ipc_msg_pool_type  pool,
    const char *file_name, unsigned int line_number);
#else
  void __ixipc_create (uint32 size, ipc_msg_id_type id,
    ixipc_msg_type **ptr, ipc_msg_pool_type  pool);
#endif

/*==============================================================
* FUNCTION __ixipc_send_and_recv
*
* DESCRIPTION
*   Calls __kxipc_send_and_recv if FEATURE_NATIVE_KXIPC is defined
*   and __ipc_send_and_recv if FEATURE_NATIVE_KXIPC is not defined.
===============================================================*/
void __ixipc_send_and_recv (ixipc_msg_type *request, uint32 destination,
  ipc_msg_id_type response_id, ixipc_msg_type **response);

/*==============================================================
* FUNCTION ixipc_delete
*
* DESCRIPTION
*   Calls KxIpc_Delete if FEATURE_NATIVE_KXIPC is defined
*   and ipc_delete if FEATURE_NATIVE_KXIPC is not defined.
===============================================================*/
void ixipc_delete (ixipc_msg_type **msg_ptr);

/*==============================================================
* FUNCTION:  ixinit_init_system()
*
* DESCRIPTION
*   Calls kxinit_init_system if FEATURE_NATIVE_KXIPC is defined.
==============================================================*/
void ixinit_init_system (const char *client_name);

/*===========================================================================
FUNCTION IXIPC_SIG_WAIT

DESCRIPTION
  Suspends a task until the one or more of the signals in the mask are set for
  the current task, or resumes if a kxipc signal is sent

DEPENDENCIES
  None.

RETURN VALUES
  The signals that were set for the task.
  OUTPUT_PARAM - 

SIDE EFFECTS
  Will cause a task swap if the signals for the task are not already set at
  the time of the call.
===========================================================================*/
ixipc_sig_type
ixipc_sig_wait
(
  rex_sigs_type  *p_sigs,
  uint32         *kxipc_signals
) ;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* IXIPC_H */

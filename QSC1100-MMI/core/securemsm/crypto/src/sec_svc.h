#ifndef SEC_SVC_H
#define SEC_SVC_H
/*===========================================================================

                    S E C U R I T Y    S E R V I C E S
					
                          I P C    S E R V E R

         E X T E R N A L   A P P L I C A T I O N   I N T E R F A C E

DESCRIPTION
  This file defines the SEC IPC server interface used to support
  IPC based security serives functionality

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cryptographic library is initialized by Security Services
  internally and no application specific intialization is required.

 Copyright (c) 2005-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/sec_svc.h#1 $ $DateTime: 2007/06/26 13:17:46 $Author: $					  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/05   sb      Changes to fix a possible race condition when issuing Async/Sync cmds
02/25/05   sb      Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_SEC

#include "rex.h"
#include "task.h"
#include "assert.h"
#ifdef FEATURE_SEC_IPC
#include "ixipc.h"
#endif
#include "msg.h"
#include "err.h"

#include "sectask.h"
#include "secerrno.h"
#include "secapi.h"
#include "seccrypt.h"
#include "seccrypti.h"

#ifdef	FEATURE_SEC_IPC_SVC


/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

extern uint8 ipc_num_rsps[MAX_PRI_ORDER_NUM];



/*===========================================================================

                       PUBLIC FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION SEC_SVC_FINISH_OP

DESCRIPTION
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void
sec_svc_finish_op
(
	seccrypt_instance_type*  instance_ptr
);

void
sec_svc_finish_op2
(
	seccrypt_cmd_type*  cmd_ptr	
);



/*==========================================================================
 FUNCTION SEC_SVC_PROCESS_IPC_SIG

 DESCRIPTION 
   This function handles IPC signal

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void
sec_svc_process_ipc_sig
( 
	void
);



/*===========================================================================

FUNCTION SEC_SVC_INIT_AFTER_TASK_START

DESCRIPTION

  Initialize security task objects that needs to be initialized after
  reporting back to MC that CM is ready to run by calling on 
  mc_task_start().

DEPENDENCIES
  This function must be called only once at powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void
sec_svc_init_after_task_start
(
	void
);


/*---------------------------------------------------------------------------
  General crypto library functions
---------------------------------------------------------------------------*/

secerrno_enum_type seccrypt_ipc_hsh_set_param
(
  seccrypt_handle_type     handle,
  secapi_algo_enum_type    algo,
  sechsh_param_enum_type   param,
  sechsh_param_data_type*  param_data_ptr
);

secerrno_enum_type seccrypt_ipc_register_wait_func
(
  rex_tcb_type*  tcb_ptr,
  rex_sigs_type  (*wait_fptr)( rex_sigs_type )
);




/*===========================================================================

                       INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


#endif /* FEATURE_SEC_IPC_SVC */
#endif /* FEATURE_SEC */

#endif /* SEC_SVC_H */

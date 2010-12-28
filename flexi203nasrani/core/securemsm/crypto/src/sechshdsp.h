#ifndef SECHSHDSP_H
#define SECHSHDSP_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                        H A S H I N G   R O U T I N E S


DESCRIPTION
  This is the header file for the Security Services hashing module.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2000-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: L:/src/asw/COMMON/vcs/sechshdsp.h_v   1.2   24 Sep 2003 15:05:04   omichael  $
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/sechshdsp.h#1 $ $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/03   om      Added dummy DSP function pointer.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_SEC

#ifdef FEATURE_SEC_DSP_CRYPTO

#include "secerrno.h"
#include "seccrypt.h"
#include "seccrypti.h"


/*===========================================================================

                        DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Type for a hash DSP instance, included by the generic seccrypt instance
---------------------------------------------------------------------------*/
struct sechshdsp_instance_s
{
  void* dummy;
};


/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECHSHDSP_POWERUP_INIT()

DESCRIPTION
  This function does all one-time initialization of DSP-specific hash data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void sechshdsp_powerup_init( void );

/*===========================================================================

FUNCTION SECHSHDSP_PLATFORM_INIT()

DESCRIPTION
  Re-initialization of DSP platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void sechshdsp_platform_init
(
  seccrypt_instance_type*      instance_ptr
);

/*===========================================================================

FUNCTION SECHSHDSP_CREATE_DIGEST()

DESCRIPTION
  This function will create a message digest using the 
  specified algorithm executing on the DSP.

DEPENDENCIES
  The hash mode should be properly set by calling sechsh_set_param.

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type sechshdsp_create_digest
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECHSHDSP_START_HASH_OP()

DESCRIPTION
  This function kicks off a hash operation on the DSP.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type sechshdsp_start_hash_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECHSHDSP_CONTINUE_HASH_OP()

DESCRIPTION
  This function kicks off a subsequent hash operation for continuous
  hashing.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type sechshdsp_continue_hash_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECHSHDSP_SETUP_DME()

DESCRIPTION
  This function will set up the DME tables for a hashing 
  operation.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type sechshdsp_setup_dme
(
  seccrypt_instance_type*             instance_ptr
);

/*===========================================================================

FUNCTION SECHSHDSP_CHECK_HASH_OP()

DESCRIPTION
  This function chacks the outcome of a DSP hash operation and sets
  the instance's errno accordingly.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type sechshdsp_check_hash_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECHSHDSP_FINISH_OP()

DESCRIPTION
  This function does command completion work for a DSP hash operation.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type sechshdsp_finish_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECHSHDSP_OP_COMPLETE_CB()

DESCRIPTION
  This function is called upon command completion (triggered by 
  DSP ISR).

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern void sechshdsp_op_complete_cb
(
  seccrypt_instance_type*  instance_ptr
);

#else /* FEATURE_SEC_DSP_CRYPTO */

/*---------------------------------------------------------------------------
  Dummy type for hash DSP function ptrs
---------------------------------------------------------------------------*/
#define sechshdsp_op_complete_cb  NULL

/*---------------------------------------------------------------------------
  Dummy type for a hash DSP instance when the DSP platform is not supported
---------------------------------------------------------------------------*/
struct sechshdsp_instance_s
{
  void* dummy;
};

#endif /* FEATURE_SEC_DSP_CRYPTO */

#endif /* FEATURE_SEC */

#endif

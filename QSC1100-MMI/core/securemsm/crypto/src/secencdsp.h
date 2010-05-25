#ifndef SECENCDSP_H
#define SECENCDSP_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                     C I P H E R I N G   R O U T I N E S


DESCRIPTION
  This is the header file for the Security Services ciphering module.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2000-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: L:/src/asw/COMMON/vcs/secencdsp.h_v   1.2   24 Sep 2003 15:05:04   omichael  $
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/secencdsp.h#1 $ $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/04   df      added new function secencdsp_invalidate_data_cache to
                   support DSP-ARM failover mechanism
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


/*===========================================================================

                        DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Type for a cipher DSP instance, included by the generic seccrypt instance
---------------------------------------------------------------------------*/
struct secencdsp_instance_s
{
  dsm_item_type*  out_msg_ptr;
};


/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECENCDSP_POWERUP_INIT()

DESCRIPTION
  This function does one-time initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
extern void secencdsp_powerup_init( void );

/*===========================================================================

FUNCTION SECENCDSP_PLATFORM_INIT()

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
void secencdsp_platform_init
(
  seccrypt_instance_type*      instance_ptr
);

/*===========================================================================

FUNCTION SECENCDSP_ENCRYPT()

DESCRIPTION
  This function will encrypt the passed message using the instance-specific
  algorithm.
  Based on the MAC mode, it will create the specified MAC if necessary.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_BAD_DATA       Data verification failed
    E_NOT_SUPPORTED  Option hasn't been coded yet
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secencdsp_encrypt
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECENCDSP_DECRYPT()

DESCRIPTION
  This function will decrypt the passed message using the instance-specific
  algorithm.
  Based on the MAC mode, it will verify the specified MAC if necessary.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_BAD_DATA       Data verification failed
    E_NOT_SUPPORTED  Option hasn't been coded yet
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secencdsp_decrypt
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECENCDSP_SETUP_DME()

DESCRIPTION
  This function will set up the and DME tables for a ciphering 
  operation.
  NOTE: En- and decryption use separate DSM item pointers as 
        parameters for the operation, the table gets filled
        accordingly.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secencdsp_setup_dme
(
  seccrypt_instance_type*             instance_ptr
);

/*===========================================================================

FUNCTION SECENCDSP_START_CIPHER_OP()

DESCRIPTION
  This function starts a cipher op on the DSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
extern secerrno_enum_type secencdsp_start_cipher_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECENCDSP_CHECK_CIPHER_OP()

DESCRIPTION
  This function checks the outcome of a cipher op on the DSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
extern secerrno_enum_type secencdsp_check_cipher_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECENCDSP_OP_COMPLETE_CB()

DESCRIPTION
  This function is called by the SECCRYPT_OP_COMPLETION_CMD
  processor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
extern void secencdsp_op_complete_cb
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECENCDSP_INVALIDATE_DATA_CACHE()

DESCRIPTION
  This function cleans the arm data cache for cipher specific areas

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
secerrno_enum_type secencdsp_invalidate_data_cache
(
  seccrypt_instance_type* instance_ptr
);

/*===========================================================================

FUNCTION SECENCDSP_FINISH_OP()

DESCRIPTION
  This function does cipher operation completion work.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
extern secerrno_enum_type secencdsp_finish_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state
);

#else /* FEATURE_SEC_DSP_CRYPTO */

/*---------------------------------------------------------------------------
  Dummy type for cipher DSP function ptrs
---------------------------------------------------------------------------*/
#define secencdsp_op_complete_cb  NULL

/*---------------------------------------------------------------------------
  Dummy type for a cipher DSP instance when the DSP platform is not supported
---------------------------------------------------------------------------*/
struct secencdsp_instance_s
{
  void* dummy;
};

#endif  /* FEATURE_SEC_DSP_CRYPTO */

#endif

#endif

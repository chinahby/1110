#ifndef SECPKXDSP_H
#define SECPKXDSP_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
            P U B L I C   K E Y   E X C H A N G E   R O U T I N E S


DESCRIPTION
  This is the header file for the Security Services PKX module.
  The PKX module provides functions to execute public key exchange
  operations and signing / authentication procedures using the 
  RSA algorithm.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2000-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: L:/src/asw/COMMON/vcs/secpkxdsp.h_v   1.3   24 Sep 2003 15:05:04   omichael  $
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/secpkxdsp.h#1 $ $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/05   sb      Added support for Rsa signing, secure modmath operations
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

#define SECCRYPT_DSP_SCRATCH_SIZE  128


/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Type for a PKX DSP instance, included by the generic seccrypt instance
---------------------------------------------------------------------------*/
struct secpkxdsp_instance_s
{
  uint8 opaque_buffer[SECCRYPT_DSP_SCRATCH_SIZE];
};


/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECPKXDSP_POWERUP_INIT()

DESCRIPTION
  This function does all one-time initialization of DSP-specific PKX data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void secpkxdsp_powerup_init( void );

/*===========================================================================

FUNCTION SECPKXDSP_PLATFORM_INIT()

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
extern void secpkxdsp_platform_init
(
  seccrypt_instance_type*      instance_ptr
);


/*===========================================================================

FUNCTION SECPKXDSP_MODEXP()

DESCRIPTION
  This function is the DSP-specific implementation of an Modular Exponentiation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkxdsp_modexp
(
  seccrypt_instance_type*  instance_ptr 

);

/*===========================================================================

FUNCTION SECPKXDSP_CREATE_KEY()

DESCRIPTION
  This function will create a key using the Diffie-Helman algorithm. The
  key created is dependent on the passed key type: Private, Public, or
  Secret. The Key will be written to the passed address.

  This function will use previously stored values for the base, modulus,
  and key information needed by the Diffie-Helman algorithm. 

  The implementation uses the aDSP.

DEPENDENCIES
  The passed Crypto-instance must be initialized to perform the Diffie-Helman
  algorithm

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any Diffie-Helman parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secpkxdsp_create_key
(
  seccrypt_instance_type*      instance_ptr
);

/*===========================================================================

FUNCTION SECPKXDSP_ENCRYPT()

DESCRIPTION
  This function is the DSP-specific implementation of an RSA public
  key encryption.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkxdsp_encrypt
(
  seccrypt_instance_type*  instance_ptr 
);

/*===========================================================================

FUNCTION SECPKXDSP_AUTHENTICATE()

DESCRIPTION
  This function is the DSP-specific implementation of an RSA public
  key authentication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkxdsp_authenticate
(
  seccrypt_instance_type*  instance_ptr 
);


/*===========================================================================

FUNCTION SECPKXDSP_SIGN()

DESCRIPTION
  This function is the DSP-specific implementation of an RSA private
  key sign operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkxdsp_sign
(
  seccrypt_instance_type*  instance_ptr 
);

/*===========================================================================

FUNCTION SECPKXDSP_SETUP_DME_TABLE()

DESCRIPTION
  This function sets up the DME table entries for base, exponent,
  and modulus.

  NOTE: The DSP context contains the lengths of each of these parameters,
        the DME table entries are read until all three parameters are
        complete. Since the parameters are stored 32-bit padded, each
        DME entry corresponds to one modmath parameter.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS      if the function executes successfully.
  E_IN_PROGRESS  if any parameters are missing

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secpkxdsp_setup_dme_table
(
  seccrypt_instance_type*  instance_ptr 
);

/*===========================================================================

FUNCTION SECPKXDSP_START_MODMATH_OP()

DESCRIPTION
  This function kicks off a modular math operation on the DSP.

DEPENDENCIES
  The parameter pointers in the DSP-specific instance are set up.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkxdsp_start_modmath_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECPKXDSP_CHECK_MODMATH_OP()

DESCRIPTION
  This function checks the outcome of a DSP modular math operation
  and sets the instance's errno accordingly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkxdsp_check_modmath_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECPKXDSP_OP_COMPLETE_CB()

DESCRIPTION
  This function is called upon command completion from the DSP ISR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void secpkxdsp_op_complete_cb
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECPKXDSP_FINISH_OP()

DESCRIPTION
  This function does DSP-specific completion of a PKX operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkxdsp_finish_op
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state  
);

#else /* FEATURE_SEC_DSP_CRYPTO */

/*---------------------------------------------------------------------------
  Dummy type for PKX DSP function ptrs
---------------------------------------------------------------------------*/
#define secpkxdsp_op_complete_cb  NULL

/*---------------------------------------------------------------------------
  Dummy type for a PKX DSP instance when the DSP platform is not supported
---------------------------------------------------------------------------*/
struct secpkxdsp_instance_s
{
  void* dummy;
};

#endif /* FEATURE_SEC_DSP_CRYPTO */

#endif /* FEATURE_SEC */

#endif /* SECPKXDSP_H */

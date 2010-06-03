
#ifndef SECAPIKS_H
#define SECAPIKS_H



/*===========================================================================

                    S E C U R I T Y    S E R V I C E S

                 C R Y P T O G R A P H I C   L I B R A R Y
				 
         E X T E R N A L   A P P L I C A T I O N   I N T E R F A C E				 


DESCRIPTION
  This file defines the external API for applications using
  keystore cryptographic functionality from Security Services.


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cryptographic library is initialized by Security Services
  internally and no application specific intialization is required.

 Copyright (c) 2005-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/secapiks.h#1 $ $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $					  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/05   sb      Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "rex.h"

#ifdef FEATURE_SEC
#ifdef FEATURE_SEC_KS

#include "secerrno.h"


/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/


/* Key store type definition. */
typedef enum secapiks_key_type_e
{
	SECAPIKS_KEY_TYPE_DRM_DEV_KEY = 0,
	SECAPIKS_KEY_TYPE_MAX = SECAPIKS_KEY_TYPE_DRM_DEV_KEY
} secapiks_key_type_t;


typedef void* secapiks_handle_type;


typedef struct secapiks_rsa_key_s
{
	uint8  modulus    [SECAPI_PKX_MAX_KEY_SIZE];
	uint8  pub_key    [SECAPI_PKX_MAX_KEY_SIZE];
	uint8  priv_key   [SECAPI_PKX_MAX_KEY_SIZE];
	uint8  priv_key_iv[SECAPI_ENC_AES128_BLOCK_BYTE_LEN];
} secapiks_rsa_key_t;


typedef union secapiks_keys_u
{
	secapiks_rsa_key_t  drm_dev_key;
} secapiks_keys_t;



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECAPIKS_OPEN()

DESCRIPTION
	Create key store handle for a given key type

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_ABORTED

SIDE EFFECTS
	None.
===========================================================================*/   
secerrno_enum_type
secapiks_open
(
	secapiks_key_type_t        key_type,
	secapiks_handle_type*      ks_handle
);


/*===========================================================================

FUNCTION SECAPIKS_CLOSE()

DESCRIPTION
	Create key store handle for a given key type

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_ABORTED

SIDE EFFECTS
	None.
===========================================================================*/   
secerrno_enum_type
secapiks_close
(
	secapiks_handle_type       ks_handle
);

#endif /* FEATURE_SEC_KS */
#endif /* FEATURE_SEC */
#endif /* SECAPIKS_H */

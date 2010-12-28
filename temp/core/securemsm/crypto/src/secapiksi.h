
#ifndef SECAPIKSI_H
#define SECAPIKSI_H



/*===========================================================================

                    S E C U R I T Y    S E R V I C E S

                 C R Y P T O G R A P H I C   L I B R A R Y
				 
         I N T E R N A L  A P P L I C A T I O N   I N T E R F A C E				 


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
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/secapiksi.h#1 $ $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $					  

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


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION SECAPIKSI_GET_KEY()

DESCRIPTION
	Returns a reference to key storage.

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_ABORTED

SIDE EFFECTS
	None.
===========================================================================*/   
secerrno_enum_type
secapiksi_get_key
(
	secapiks_handle_type       ks_handle,
	secapiks_keys_t            **key
);



/*===========================================================================

FUNCTION SECAPIKSI_CREATE()

DESCRIPTION
	Create key store

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_ABORTED

SIDE EFFECTS
	None.
===========================================================================*/   
secerrno_enum_type
secapiksi_create
(
	secapiks_key_type_t        key_type,
	secapiks_keys_t            *key
);

#endif /* FEATURE_SEC_KS */
#endif /* FEATURE_SEC */
#endif /* SECAPIKSI_H */

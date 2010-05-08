#ifndef SECPKXI_H
#define SECPKXI_H



/*===========================================================================

                    S E C U R I T Y    S E R V I C E S

                 C R Y P T O G R A P H I C   L I B R A R Y

         I N T E R N A L   A P P L I C A T I O N   I N T E R F A C E

DESCRIPTION
  This file defines the internal API for applications using
  cryptographic functionality from Security Services.
  The cryptographic API (CAPI) comprises

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cryptographic library is initialized by Security Services
  internally and no application specific intialization is required.

 Copyright (c) 2005-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/secpkxi.h#1 $ $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $					  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/06   jay     updated header files
03/02/05   sb      Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "rex.h"

#ifdef FEATURE_SEC

#include "secerrno.h"


/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION SECAPII_EMSA_PSS_ENCODE()

DESCRIPTION
	Signature encoding algorithm, refer to PKCS#1 V2.1

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type
secpkxi_emsa_pss_encode(
	uint8* msg_ptr,
	uint32 msg_ptr_len, 
	uint8* encoded_msg_ptr,
	uint32 encoded_msg_len,
    uint32 encoded_msg_bit_len
	);


/*===========================================================================

FUNCTION SECAPII_EMSA_PSS_DECODE()

DESCRIPTION
	Signature decoding algorithm, refer to PKCS#1 V2.1

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_FAILURE

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type
secpkxi_emsa_pss_decode(
	uint8* msg_ptr,
	uint32 msg_ptr_len, 
	uint8* encoded_msg_ptr,
	uint32 encoded_msg_len,
    uint32 encoded_msg_bit_len
	);

#endif
#endif


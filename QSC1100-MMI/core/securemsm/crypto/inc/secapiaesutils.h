#ifndef SECAPIAESUTILS_H
#define SECAPIAESUTILS_H


/*===========================================================================

                     A E S   U T I L I T Y   F U N C T I O N S
							 
FILE:  secapiaesutils.h

DESCRIPTION:
   

   References:

   RFC 3566; 
   Section B.A.9.2.4 in DVB Bluebook A100 ( IP Datacast over DVB-H:
   Service Purchase and Protection (SPP) )

   Design Notes:
   
EXTERNALIZED FUNCTIONS

   aes_xcbc_mac() 
	AES in CBC  with a set of extensions[XCBC-MAC-1];
	refer to RFC 3566

 Copyright (c) 2005-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*=========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secapiaesutils.h#2 $ 
  $DateTime: 2008/04/07 17:53:07 $ 
  $Author: jamese $					  					  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/05   rv      Created file.

============================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_SEC

#include "rex.h"
#include "task.h"
#include "assert.h"
#include "msg.h"
#include "err.h"

#include "secerrno.h"
#include "secapi.h"
#include "seccrypt.h"

/*==========================================================================
 FUNCTION SECAPI_AES_XCBX_MAC_PRF
  
 DESCRIPTION 
   This function implements the DVB Document A100; Section B.A.9.4
   AES-XCBC-MAC-PRF

 PARAMETERS
   auth_seed      -[I/P] AS as specified by DVB DocA100
   auth_seed_len  -[I/P] length of AS ( should be 16 bytes
   constant       -[I/P] CONSTANT as specified by DVB DocA100
   constant_len   -[I/P] length of CONSTANT
   output_ptr     -[O/P] memory for storing the expected out put
                         it should be allocated by the caller
   output_len     -[O/P] the size of memory allocated for output_ptr
                         SHOULD BE A MUTIPLE OF 16 bytes (128 bits)
   
 DEPENDENCIES 
   
 RETURN VALUE 
   Security services error code.
 
 SIDE EFFECTS
   None
===========================================================================*/
secerrno_enum_type secapi_aes_xcbc_mac_prf
(
   uint8*                        auth_seed,
   uint8                         auth_seed_len,
   uint8*                        constant,
   uint64                        constant_len,
   uint8*                        output_ptr,
   uint64*                       output_len
);

#endif /* FEATURE_SEC */
#endif /* SECAPIAESUTILS_H */



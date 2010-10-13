#ifndef SECAPI_UTIL_H
#define SECAPI_UTIL_H

#include "customer.h"

#ifdef FEATURE_SEC

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secapi_util.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/09   ejt     added routine secapi_util_read_hw_key_api_type 
95/21/07   avm     corrected RPC meta comments
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "IxErrno.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "secapi.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*
** Default value of IV for AES encryption.
*/
extern uint8 secapi_util_AESIV[SECAPI_ENC_AES128_BLOCK_BYTE_LEN];

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
*
* FUNCTION: secapi_util_encrypt_with_hw_key ()
*
* DESCRIPTION:
*
*   Encrypts the input data using the hardware primary key. On platforms
* where there is no hardware key support, a dummy key is used.
*
===========================================================================*/
IxErrnoType secapi_util_encrypt_with_hw_key (char *data,
  unsigned int data_len, char *encrypted_data,
  unsigned int *encrypted_data_len_ptr);

/*===========================================================================
*
* FUNCTION: secapi_util_decrypt_with_hw_key ()
*
* DESCRIPTION:
*
*   Decrypts the input data using the hardware primary key. On platforms
* where there is no hardware key support, a dummy key is used.
*
===========================================================================*/
IxErrnoType secapi_util_decrypt_with_hw_key (char *data,
  unsigned int data_len, char *decrypted_data,
  unsigned int *decrypted_data_len_ptr);

/*===========================================================================
* FUNCTION  secapi_util_read_hw_key_api_type
*
* DESCRIPTION
*    This function determines the API to use to read the HW DEV Key
*    on the Modem processor and returns true for API2 and false for API1.
*
===========================================================================*/
boolean secapi_util_read_hw_key_api_type(void);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_SEC */

#endif /* SECAPI_UTIL_H */

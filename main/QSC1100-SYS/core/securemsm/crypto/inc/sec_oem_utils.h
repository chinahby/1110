#ifndef SEC_OEM_UTILS_H
#define SEC_OEM_UTILS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      S E C U R I T Y    S E R V I C E S
 
                        SEC OEM Utils Header file

GENERAL DESCRIPTION
  Contains OEM utility for reading/writing private data

EXTERNAL FUNCTIONS
  sec_oem_data_set()
  sec_oem_data_get()

Copyright (c) 2008-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/sec_oem_utils.h#8 $ 
  $DateTime: 2009/03/24 18:32:55 $ $Author: shitalm $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/13/09   bm      Removed storing seed into SFS
11/17/09   avm     added API's to enable EM's to provision the seed for CSPRNG
                   OEM's can select the mechanism they need to use for storage 
				   and retrieval of CSPRNG seed.
02/08/08   avm     added API's to enable OEM provisioned Device Key for encrypt
                   Decrypt function.
09/10/07   gr      created
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "secerrno.h"


/* This enum identifies the key type to be used with System SFS */
typedef enum
{
  E_MSM_DEV_KEY       = 0,  /* Use MSM Device Key. Only useful (i.e device unique key) if Secure Boot to be enabled */
  E_OEM_DEV_KEY       = 1,  /* Use OEM Specified key. Protecting the key is OEM's responsibility */
  E_SEC_PART_DEV_KEY  = 2,  /* Use a key from secure partition. Secure partition is a flash partition that is write
                               protected. */
  E_NO_DEV_KEY        = 3
}secapi_util_selected_dev_key_type;

/* This enum identifies the seed type to be used with CSPRNG */
typedef enum
{
  E_CSPRNG_OEM_SEED       = 0,  /* Use OEM specified seed mechanism, currently
                                 * defaulted to NV */
  E_CSPRNG_NV_SEED        = 1,  /* Use NV to store seed data, presently routed
                                 * through oem layer. */
  E_CSPRNG_NO_SEED        = 2
}secapi_util_selected_csprng_seed_type;

/*===========================================================================

FUNCTION SEC_OEM_DATA_SET ()

DESCRIPTION
  Allocates memory, copies the data passed in into the allocated memory
  and zeroes out the input data. The data copy may involve obfuscation
  operations.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS if successful, another error code if not

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sec_oem_data_set (void *input_data, unsigned int input_len,
  void **output_handle);

/*===========================================================================

FUNCTION SEC_OEM_DATA_GET ()

DESCRIPTION
  Locates the data corresponding to the handle passed in, allocates memory
  for this data and copies the data into the allocated memory. The data copy
  may involve a de-obfuscation operation.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS if successful, another error code if not

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sec_oem_data_get (void *input_handle, void *output_data,
  unsigned int *output_len);

/*===========================================================================

FUNCTION SEC_OEM_DATA_CLEAR ()

DESCRIPTION
  Locates the data corresponding to the handle passed in, clears the data
  and frees the memory allocated for the data and the handle.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS if successful, another error code if not

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sec_oem_data_clear (void *input_handle);

/*===========================================================================

FUNCTION SEC_OEM_DATA_DUP ()

DESCRIPTION
  Duplicates data using the handle passed in and returns a handle to the
  duplicated data.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS if successful, another error code if not

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sec_oem_data_dup (void *input_handle, void **output_handle);

/*===========================================================================
*
* FUNCTION: secapi_oem_hw_key_get ()
*
* DESCRIPTION:
*
*   Provides an API for OEM implementation of unique device ID, Hardware Key.
*
===========================================================================*/
secerrno_enum_type secapi_oem_hw_key_get (void *key, int len);

/*===========================================================================
*
* FUNCTION: secapi_oem_dev_key_type_get ()
*
* DESCRIPTION:
*
*   Provides an API for OEM. to specify the type of device key to use.
*
===========================================================================*/

secapi_util_selected_dev_key_type secapi_oem_dev_key_type_get(void);

/*===========================================================================
*
* FUNCTION: secapi_oem_iv_get ()
*
* DESCRIPTION:
*
*   Allows an OEM to provide an IV for decryption.
*
===========================================================================*/
secerrno_enum_type secapi_oem_iv_get (void *iv, int len);

/*===========================================================================
*
* FUNCTION: secapi_oem_csprng_seed_get ()
*
* DESCRIPTION:
*
*   this API is called by CSPRNG to retrieve the seed. this API Allows an OEM
*   to customize the seed retrieval mechanism, currently defaulted to use the
*   seed value for CSPRNG to be stored in and retrived from the NV memory.
*
* RETURN VALUE:
*
*   returns E_SUCCESS once the seed to used as the initial entropy for CSPRNG is
*   read correctly from the storage mechanism, presently as NV item. if 
*   successful also returns the populated seed_ptr.
*
===========================================================================*/
secerrno_enum_type secapi_oem_csprng_seed_get ( uint8* seed_ptr, uint16 seed_len);

/*===========================================================================
*
* FUNCTION: secapi_oem_csprng_seed_put ()
*
* DESCRIPTION:
*
*   Allows an OEM defined mechanism to store the updated seed value for CSPRNG.
*
* RETURN VALUE:
*
*  Returns E_SUCCESS if seed is stored in its correct place. presently as an NV
*
===========================================================================*/
secerrno_enum_type secapi_oem_csprng_seed_put (uint8* seed_ptr, uint16 seed_len);

/*===========================================================================
*
* FUNCTION: secapi_oem_csprng_seed_type_get ()
*
* DESCRIPTION:
*
*   Allows OEM to specify the seed type to be used with CSPRNG.
*
* RETURN VALUE:
* 
* returns the selected seed mechanism as an enum.
*
===========================================================================*/
secapi_util_selected_csprng_seed_type secapi_oem_csprng_seed_type_get(void);


#ifdef __cplusplus
}
#endif


#endif /* SEC_OEM_UTILS_H */

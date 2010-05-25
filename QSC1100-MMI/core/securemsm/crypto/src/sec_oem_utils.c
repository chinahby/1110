/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      S E C U R I T Y    S E R V I C E S
 
                           SEC OEM Utilities

GENERAL DESCRIPTION
  Contains OEM utilities for obfuscating key data

  NOTE: The functions here do not perform any obfuscation and are present
  merel

EXTERNAL FUNCTIONS
   sec_oem_data_set()
   sec_oem_data_get()
   sec_oem_data_clear()

Copyright (c) 2008-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/sec_oem_utils.c#13 $ 
  $DateTime: 2009/04/06 08:33:07 $ $Author: etarolli $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/09   ejt     Removed #include efuse.h
03/02/09   ejt     Added PLATFORM_LTK feature flag to support SDK/LTK
02/13/09   bm      Removed storing seed into SFS
11/17/09   avm     added API's to enable EM's to provision the seed for CSPRNG
                   OEM's can select the mechanism they need to use for storage 
				   and retrieval of CSPRNG seed.
02/28/08   avm     Removed references to extra headers
02/08/08   avm     added API's to enable OEM provisioned Device Key for encrypt
                   Decrypt function.
09/10/07   gr      created
*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "sec_oem_utils.h"
#include "msg.h"
#include "secerrno.h"
#include "assert.h"
#include "UxCommon.h"
#include "secutil.h"
#include "nv.h"

#ifdef FEATURE_SEC_SFS
#include "sfs_private.h"
#endif /* FEATURE_SEC_SFS */

#if defined (FEATURE_SEC_KEY_SEC)
/*===========================================================================

                     FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

secerrno_enum_type sec_oem_data_set (void *input_data, unsigned int input_len,
  void **output_handle)
{
  unsigned int *stored_handle;

  if ((NULL == input_data) || (0 == input_len) || (NULL == output_handle))
    return E_INVALID_ARG;

  /* We need to store the data length, so allocate extra bytes. There will
  ** be no alignment problems as long as sizeof(int) is four or less, since
  ** UxMalloc allocates on four-byte boundaries.
  */
  ASSERT (sizeof(unsigned int) <= 4);

  stored_handle = UxMalloc (input_len + sizeof(unsigned int), UX_ALLOC_LOCAL);

  if (NULL == stored_handle)
    return E_NO_MEMORY;

  /* This function merely makes a copy of the input data and returns a
  ** pointer to the copy. An obfuscation operation can be performed instead,
  ** if desired. 
  */

  *stored_handle = input_len;

  *output_handle = (void *) (stored_handle + 1);

  memcpy (*output_handle, input_data, input_len);

  memset (input_data, input_len, 0x00);

  return E_SUCCESS;
}

secerrno_enum_type sec_oem_data_get (void *input_handle, void *output_data,
  unsigned int *output_len)
{
  unsigned int *stored_data;

  /* This function merely makes a copy of the input data. A de-obfuscation
  ** operation will be required if the data was obfuscated before being stored.
  */
  if ((NULL == input_handle) || (NULL == output_data) || (NULL == output_len))
    return E_INVALID_ARG;

  stored_data = ((unsigned int *) input_handle) - 1;

  if (*output_len < *stored_data)
    return E_NO_MEMORY;

  *output_len = *stored_data;

  memcpy (output_data, (void *) input_handle, *output_len);

  return E_SUCCESS;
}

secerrno_enum_type sec_oem_data_clear (void *input_handle)
{
  unsigned int *stored_data;

  if (NULL == input_handle)
    return E_INVALID_ARG;

  stored_data = ((unsigned int *) input_handle) - 1;

  memset (stored_data, 0x00, *stored_data + sizeof(unsigned int));

  UxFree (stored_data);

  return E_SUCCESS;
}

secerrno_enum_type sec_oem_data_dup (void *input_handle, void **output_handle)
{
  unsigned int *stored_data;
  unsigned int *new_handle;

  if ((NULL == input_handle) || (NULL == output_handle))
    return E_INVALID_ARG;

  stored_data = ((unsigned int *) input_handle) - 1;

  new_handle = UxMalloc (*stored_data + sizeof(unsigned int), UX_ALLOC_LOCAL);

  if (NULL == new_handle)
    return E_NO_MEMORY;

  memcpy (new_handle, stored_data, *stored_data + sizeof(unsigned int));

  *output_handle = (void *) (new_handle + 1);

  return E_SUCCESS;
}
#endif // (FEATURE_SEC_KEY_SEC)

/*===========================================================================
*
* FUNCTION: secapi_oem_hw_key_get ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
secerrno_enum_type secapi_oem_hw_key_get (void *key, int len)
{
  int i;
  uint8 *_key = (uint8*)key;

  if ((NULL == key) || (0 == len))
    return E_INVALID_ARG;

/* this is a dummy implementation of hardware key, and serves as a 
   placeholder for OEMs hardware key provisioning implementation.
*/

  for(i = 0; i < len; i++) _key[i] = i + 0x50;
  MSG_HIGH("using OEM def HW key!",0,0,0);
  return E_SUCCESS;
}

/*===========================================================================
*
* FUNCTION: secapi_oem_dev_key_type_get ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
secapi_util_selected_dev_key_type secapi_oem_dev_key_type_get(void)
{
  secapi_util_selected_dev_key_type dev_key_type_select;
#ifdef FEATURE_SEC_SFS_MSM_DEV_KEY
  dev_key_type_select = E_MSM_DEV_KEY;
#else
  dev_key_type_select = E_OEM_DEV_KEY;
#endif
/* OEM can change the above, if they like to use OEM_DEV_KEY for system SFS.
     It is recommended to use OEM_DEV_KEY, if Secure Boot is not enabled
     on the target. */
MSG_MED("Key type selected: %d ",dev_key_type_select,0,0);
return dev_key_type_select;

}

/*===========================================================================
*
* FUNCTION: secapi_oem_iv_get ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
secerrno_enum_type secapi_oem_iv_get (void *iv, int len)
{
  int i;
  uint8 *_iv = (uint8*)iv;

  if ((NULL == iv) || (0 == len))
    return E_INVALID_ARG;

  /* This is a dummy implementation of this function. It serves as a
     placeholder for the OEM's hardware key provisioning implementation.
  */

  for(i = 0; i < len; i++) _iv[i] = i + 0x50;
  MSG_HIGH("using OEM defined IV!",0,0,0);
  return E_SUCCESS;
}

/*===========================================================================
*
* FUNCTION: secapi_oem_csprng_seed_get ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
secerrno_enum_type secapi_oem_csprng_seed_get (uint8* seed_ptr, uint16 seed_len)
{
	
  secerrno_enum_type stat = E_NOT_AVAILABLE;
  uint8 *_seed_ptr = (uint8*)seed_ptr;
  if ((NULL == seed_ptr) || (0 == seed_len))
  {
    return E_INVALID_ARG;
  }
   

  stat = secutil_get_nv_item( NV_SEC_CSPRNG_INIT_SEED_I,
                              _seed_ptr,
                              seed_len );
  if( E_SUCCESS == stat )
  {
    MSG_MED("sec_oem_util: nv data for csprng! retrieved", 0, 0, 0);  
  }
  else if( stat == E_NO_DATA )
  {
    MSG_ERROR("sec_oem_util: no nv data for csprng!", 0, 0, 0);
  }
  else
  {
    MSG_ERROR("sec_oem_util: error reading nv data for csprng (%d)!", stat, 0, 0);
  }
  return stat;
}

/*===========================================================================
*
* FUNCTION: secapi_oem_csprng_seed_put ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
secerrno_enum_type secapi_oem_csprng_seed_put (uint8* seed_ptr, uint16 seed_len)
{
  secerrno_enum_type stat = E_NOT_AVAILABLE;
  uint8 *_seed_ptr = (uint8*)seed_ptr;
  if ((NULL == seed_ptr) || (0 == seed_len))
    return E_INVALID_ARG;

  stat = secutil_put_nv_item( NV_SEC_CSPRNG_INIT_SEED_I, _seed_ptr );
  if( E_SUCCESS != stat )
  {
    MSG_ERROR( "secrnd: NV item write seed error: %d", stat, 0, 0 );
  }
  return stat;
}

/*===========================================================================
*
* FUNCTION: secapi_oem_csprng_seed_type_get ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
secapi_util_selected_csprng_seed_type secapi_oem_csprng_seed_type_get(void)
{
  secapi_util_selected_csprng_seed_type csprng_seed_type_select;

  csprng_seed_type_select = E_CSPRNG_OEM_SEED;

  /* OEM can change the above */

  MSG_MED("seed type selected: %d ",csprng_seed_type_select,0,0);
  return csprng_seed_type_select;
}


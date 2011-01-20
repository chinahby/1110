/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION
  This file provides the minimal hardware setup required for the PBL to
  execute.
  
EXTERNALIZED FUNCTIONS
  dbl_target_interface_init
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2009 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/shared/boot_pbl_accessor.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/19/09   dk      Reset logging support for ULC target.
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_pbl_accessor.h"
#include "boot_error_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/* Pointer to data coming from PBL */
static boot_pbl_shared_data_type   *boot_pbl_shared_data_ptr = NULL;  
  
/*=============================================================================   
                              FUNCTION DEFINITIONS
=============================================================================*/

/*=========================================================================

FUNCTION  BOOT_PBL_GET_CLOCK_SPEED

DESCRIPTION
  This function returns the clock speed set by PBL.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 boot_pbl_get_clock_speed
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return boot_pbl_shared_data_ptr->clk_bus_speed;
} /* boot_pbl_get_clock_speed */

/*===========================================================================

FUNCTION  BOOT_PBL_SHA1_HASH

DESCRIPTION
  This function exposes the sha1_hash routine exposed by PBL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void boot_pbl_sha1_hash
(
  const uint8  *buff_ptr, 
  uint32  buff_size, 
  uint8  *digest_ptr
)
{
  BL_VERIFY( ( boot_pbl_shared_data_ptr != NULL &&
               boot_pbl_shared_data_ptr->fn_tbl != NULL ),
             BL_ERR_NULL_PTR_PASSED );

  boot_pbl_shared_data_ptr->fn_tbl->sha1_hash( buff_ptr,
                                               buff_size,
                                               digest_ptr
                                             );
} /* boot_pbl_sha1_hash */

/*===========================================================================
FUNCTION  BOOT_PBL_VERIFY_X509_CHAIN

DESCRIPTION
  This function exposes the verify_x509_chain routine exported by PBL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
pbl_secx509_cert_type* boot_pbl_verify_x509_chain
(
    const uint8*                   x509_chain_ptr,
    uint32                         x509_chain_len,
    const uint8*                   x509_root_cert_ptr,
    pbl_secx509_cert_type**        qc_ca_ptr,
    pbl_secx509_cert_type**        oem_ca_ptr,
    secboot_dbg_info_type*         dbg_info_ptr
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return( boot_pbl_shared_data_ptr->fn_tbl->secboot_verify_x509_chain( 
                                        x509_chain_ptr,
                                        x509_chain_len,
                                        x509_root_cert_ptr,
                                        qc_ca_ptr,
                                        oem_ca_ptr,
                                        dbg_info_ptr ));
}

/*===========================================================================

FUNCTION  BOOT_PBL_VERIFY_CODE_SIGNATURE

DESCRIPTION
  This function exposes the verify_code_signature routine exported by PBL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean boot_pbl_verify_code_signature
(
    const uint8*                  code_ptr,
    uint32                        code_len,
    uint64                        msm_hw_id,
    uint64                        sw_vsn_id,
    const uint8*                  signature_ptr,
    uint32                        signature_len,
    pbl_secx509_cert_type*        attestation_cert_ptr
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return ( boot_pbl_shared_data_ptr->fn_tbl->secboot_verify_code_signature(
                                             code_ptr,
                                             code_len,
                                             msm_hw_id,
                                             sw_vsn_id,
                                             signature_ptr,
                                             signature_len,
                                             attestation_cert_ptr) );
}

/*===========================================================================

FUNCTION  BOOT_PBL_IS_AUTH_ENABLED

DESCRIPTION
  This function returns whether PBL detected that auth was enabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean boot_pbl_is_auth_enabled
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return (boot_pbl_shared_data_ptr->auth_enabled);
}

/*===========================================================================

FUNCTION  BOOT_PBL_GET_MSM_HW_ID

DESCRIPTION
  This function returns msm hw ID read by PBL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
uint64 boot_pbl_get_msm_hw_id
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return (boot_pbl_shared_data_ptr->msm_hw_id);
}

/*===========================================================================

FUNCTION  BOOT_PBL_GET_FLASH_TYPE

DESCRIPTION
  This function returns the flash type detected by PBL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boot_flash_type boot_pbl_get_flash_type
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return (boot_pbl_shared_data_ptr->flash_shared_data->type);
} /* boot_pbl_get_flash_type */

/*===========================================================================

FUNCTION  BOOT_PBL_GET_FLASH_SHARED_DATA

DESCRIPTION
  This function returns the pointer to the flash related information found
  by PBL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boot_flash_shared_dev_info_type* boot_pbl_get_flash_shared_data
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return (boot_pbl_shared_data_ptr->flash_shared_data);
} /* boot_pbl_get_flash_shared_data */

/*===========================================================================

FUNCTION  BOOT_PBL_GET_ROOT_CERT_PTR

DESCRIPTION
  This function returns the root_cert_ptr determined by PBL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
uint8* boot_pbl_get_root_cert_ptr
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return (boot_pbl_shared_data_ptr->root_cert_ptr);
} /* boot_pbl_get_root_cert_ptr */

/*===========================================================================

FUNCTION  BOOT_PBL_GET_DBL_HDR_PTR

DESCRIPTION
  This function returns the dbl_hdr_ptr shared by PBL

DEPENDENCIES
  None

RETURN VALUE
  boot_sbl_header_type*

SIDE EFFECTS
  None.

===========================================================================*/
boot_sbl_header_type* boot_pbl_get_dbl_hdr_ptr
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return (boot_pbl_shared_data_ptr->dbl_hdr);
} /* boot_pbl_get_dbl_hdr_ptr */

/*===========================================================================

FUNCTION  BOOT_PBL_INIT_SHARED_ROUTINE

DESCRIPTION
  This function would initialize shared routine of PBL

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean boot_pbl_init_shared_routine
(
   pbl_error_handler_type err_handler_ptr,
   pbl_sha1_func_type sha1_func_ptr
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );
  
  boot_pbl_shared_data_ptr->fn_tbl->init_shared_routines(err_handler_ptr, sha1_func_ptr);

  return TRUE;
} /* boot_pbl_init_shared_routine */

/*===========================================================================

FUNCTION  BOOT_PBL_GET_STACK_BASE

DESCRIPTION
  This function returns stack base shared by PBL

DEPENDENCIES
  None

RETURN VALUE
  uint8*

SIDE EFFECTS
  None.

===========================================================================*/
uint8* boot_pbl_get_stack_base
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return (boot_pbl_shared_data_ptr->boot_stack_base);
}

/*===========================================================================

FUNCTION  BOOT_PBL_GET_DBL_DEST_ADDR

DESCRIPTION
  This function returns the destinatioon address of DBL shared by PBL

DEPENDENCIES
  None

RETURN VALUE
  uint8*

SIDE EFFECTS
  None.

===========================================================================*/
uint8* boot_pbl_get_dbl_dest_addr
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  return (boot_pbl_shared_data_ptr->dbl_hdr->dbl_dest_ptr);
}

/*===========================================================================

FUNCTION  BOOT_PBL_ENTER_DOWNLOADER

DESCRIPTION
  This function exposes the boot_downloader routine exported by PBL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_pbl_enter_downloader( void )
{
  return; /* ULC PBL doesn't have downloader */
}

/*===========================================================================

FUNCTION  BOOT_PBL_INTERFACE_INIT

DESCRIPTION
  Function that will initialize the target interface so that generic DBL
  code can use it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean boot_pbl_interface_init
(
  boot_pbl_shared_data_type   *pbl_shared_data  /* Pointer to data coming from PBL */
)
{
  if (pbl_shared_data == NULL)
  {
    return FALSE;
  }

  boot_pbl_shared_data_ptr = pbl_shared_data;
  return TRUE;
} /* boot_pbl_interface_init */

/*===========================================================================

FUNCTION  BOOT_PBL_GET_BOOT_STACK_BASE

DESCRIPTION
  This function returns the boot_stack_base shared by PBL

DEPENDENCIES
  None

RETURN VALUE
  uint32 - pbl_stack_base boot_stack_base

SIDE EFFECTS
  None

===========================================================================*/
uint32 boot_pbl_get_boot_stack_base
(
  void
)
{
    BL_VERIFY( boot_pbl_shared_data_ptr->boot_stack_base != NULL,
               BL_ERR_NULL_PTR_PASSED );

  return ((uint32)boot_pbl_shared_data_ptr->boot_stack_base);
} /* boot_pbl_get_boot_stack_base */


#ifndef BOOT_PBL_ACCESSOR_H
#define BOOT_PBL_ACCESSOR_H

/*=============================================================================

                            B o o t  S h a r e d
                       B o o t  P B L  A c c e s s o r
                            H e a d e r  F i l e
GENERAL DESCRIPTION
  This file provides an interface to device bootloader to set and get the
  root certificate.
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2009 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/shared/boot_pbl_accessor.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/19/09   dk      Reset logging support for ULC target.
02/07/08   MJS     Use PBL authentication routines.
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash.h"
#include "boot_headers.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_PBL_GET_CLOCK_SPEED

DESCRIPTION
  This function returns the clock speed set by PBL.

DEPENDENCIES
  None
  
RETURN VALUE
  The clock speed configured by the PBL

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 boot_pbl_get_clock_speed
(
  void
);

/*===========================================================================

FUNCTION  BOOT_PBL_SHA1_HASH

DESCRIPTION
  This function exposes the sha1_hash routine exposed by PBL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void boot_pbl_sha1_hash
(
  const uint8  *buff_ptr, 
  uint32  buff_size, 
  uint8  *digest_ptr
);

/*===========================================================================

FUNCTION  BOOT_PBL_VERIFY_X509_CHAIN

DESCRIPTION
  This function exposes the verify_x509_chain routine exported by PBL.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the cert chain was verified, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern struct pbl_secx509_cert_type* boot_pbl_verify_x509_chain
(
    const uint8*                   x509_chain_ptr,
    uint32                         x509_chain_len,
    const uint8*                   x509_root_cert_ptr,
    pbl_secx509_cert_type**        qc_ca_ptr,
    pbl_secx509_cert_type**        oem_ca_ptr,
    secboot_dbg_info_type*         dbg_info_ptr
);

/*===========================================================================

FUNCTION  BOOT_PBL_VERIFY_CODE_SIGNATURE

DESCRIPTION
  This function exposes the verify_code_signature routine exported by PBL.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the code signature was verified, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean boot_pbl_verify_code_signature
(
    const uint8*                  code_ptr,
    uint32                        code_len,
    uint64                        msm_hw_id,
    uint64                        sw_vsn_id,
    const uint8*                  signature_ptr,
    uint32                        signature_len,
    pbl_secx509_cert_type*        attestation_cert_ptr
);

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
extern void boot_pbl_enter_downloader( void );

/*===========================================================================

FUNCTION  BOOT_PBL_GET_ROOT_CERT_PTR

DESCRIPTION
  This function returns the root_cert_ptr determined by PBL

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the root certificate

SIDE EFFECTS
  None

===========================================================================*/
extern uint8* boot_pbl_get_root_cert_ptr
(
  void
);

/*===========================================================================

FUNCTION  BOOT_PBL_GET_FLASH_SHARED_DATA

DESCRIPTION
  This function returns the pointer to the flash related information found
  by PBL.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the PBL shared flash data structure

SIDE EFFECTS
  None

===========================================================================*/
extern boot_flash_shared_dev_info_type* boot_pbl_get_flash_shared_data
(
  void
);

/*===========================================================================

FUNCTION  BOOT_PBL_IS_AUTH_ENABLED

DESCRIPTION
  This function returns whether PBL detected that auth was enabled.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if auth is enabled, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean boot_pbl_is_auth_enabled
(
  void
);

/*===========================================================================

FUNCTION  BOOT_PBL_GET_MSM_HW_ID

DESCRIPTION
  This function returns msm hw ID read by PBL

DEPENDENCIES
  None

RETURN VALUE
  The MSM hardware ID

SIDE EFFECTS
  None

===========================================================================*/
extern uint64 boot_pbl_get_msm_hw_id
(
  void
);

/*===========================================================================

FUNCTION  BOOT_PBL_GET_FLASH_TYPE

DESCRIPTION
  This function returns the flash type detected by PBL

DEPENDENCIES
  None

RETURN VALUE
  The boot flash type detected by the PBL

SIDE EFFECTS
  None

===========================================================================*/
extern boot_flash_type boot_pbl_get_flash_type
(
  void
);

/*===========================================================================

FUNCTION  BOOT_PBL_GET_DBL_HDR_PTR

DESCRIPTION
  This function returns the dbl_hdr_ptr shared by PBL

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the DBL header

SIDE EFFECTS
  None

===========================================================================*/
extern boot_sbl_header_type* boot_pbl_get_dbl_hdr_ptr
(
  void
);

/*===========================================================================

FUNCTION  BOOT_PBL_INIT_SHARED_ROUTINE

DESCRIPTION
  This function would initialize shared routine of PBL

DEPENDENCIES
  None

RETURN VALUE
  TRUE if initialization was sucessful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean boot_pbl_init_shared_routine
(
   pbl_error_handler_type err_handler_ptr,
   pbl_sha1_func_type sha1_func_ptr
);

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
);

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
);

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
void boot_pbl_enter_downloader
( 
  void
);

/*===========================================================================

FUNCTION  BOOT_PBL_INTERFACE_INIT

DESCRIPTION
  Function that will initialize the target interface so that generic SBL
  code can use it.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if initialization was successful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean boot_pbl_interface_init
(
  boot_pbl_shared_data_type   *pbl_shared_data  /* Pointer to data coming from PBL */
);

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
extern uint32 boot_pbl_get_boot_stack_base
(
  void
);


#endif  /* BOOT_PBL_ACCESSOR_H */


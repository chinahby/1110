#ifndef SECBOOT_API_H
#define SECBOOT_API_H

/*===========================================================================

                      QUALCOMM Secondary  B o o t  L o a d e r
                    A P I ' s    F o r    S e c u r i t y   L i b

DESCRIPTION
    This header file contains declarations and definitions for the QCSBL
    interface to the software that authenticates the OEMSBL and AMSS.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/msmshared/secboot2/shared/1_0/sec/secboot_api.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/07   anb     API's for Security Library..

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "boot_comdef.h"
#include "secboot_types.h"
#include "miheader.h"
#include "secboot_errno.h"

/*=====================================================================================
FUNCTION PBL_SHA1HASH

DESCRIPTION
  This function calculates the hash.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the signature check passed.

SIDE EFFECTS
  None  
=======================================================================================*/
void pbl_SHA1Hash(const uint8 *buff_ptr, uint32 buff_size, uint8 *digest_ptr);

/*========================================================================
FUNCTION PBL_BIGINT_MODEXP

DESCRIPTION
   pbl_BIGINT_ModExp() uses Montgomery multiplication(MM).
   The preprocessing step (R^2 mod M and n0) of MM is cached to speed
   up the next ModExp with the same key. This step is slow as division is
   used to compute R^2 mod M.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the signature check passed.

SIDE EFFECTS
  None  
==========================================================================*/
errno_enum_type  pbl_BIGINT_ModExp  (BigInt *r, BigInt *a, BigInt *p, BigInt *m);

/*===========================================================================
FUNCTION SECBOOT_VERIFY_CODE_SIGNATURE_API

DESCRIPTION
  This function validates a signature over the given code.

DEPENDENCIES
  None

PARAMETERS
  code_ptr              - Address of the code image
  code_len              - Size of the code image in bytes
  msm_hw_id             - ID of the MSM (read from MSM register)
  sw_vsn_id             - Software version ID
  signature_ptr         - Address of the code signature to check
  signature_len         - Length of the code signature to check in bytes
  attestation_cert_ptr  - Pointer to parsed attestation certificate

RETURN VALUE
  TRUE if the signature check passed.

SIDE EFFECTS
  None
===========================================================================*/
boolean secboot_verify_code_signature_api
(
  const uint8*        code_ptr,
  uint32              code_len,
  uint64              msm_hw_id,
  uint64              sw_vsn_id,
  const uint8*        signature_ptr,
  uint32              signature_len,
  pbl_secx509_cert_type*  attestation_cert_ptr
);

/*===========================================================================
FUNCTION SECBOOT_VERIFY_X509_CHAIN_API

DESCRIPTION
  This function parses a list of certificates authenticating a code
  signature.

DEPENDENCIES
  None

PARAMETERS
  x509_chain_ptr     - Address of attestation certificate chain
  x509_chain_len     - Size of attestation certificate chain
  x509_root_cert_ptr - Address of the parsed root certificate in ROM
  qc_ca_ptr          - Address of Qualcomm CA storage, filled in by function
                       if the chain is using the Qualcomm CA
  oem_ca_ptr         - Address of OEM CA storage, filled in by function
                       if the chain is using the OEM CA
  dbg_info_ptr       - Address of debug info from attestation certificate,
                       filled in by function

RETURN VALUE
  Pointer to the attestation certificate, NULL if valiadtion failed.

SIDE EFFECTS
  None
===========================================================================*/
pbl_secx509_cert_type* secboot_verify_x509_chain_api
(
  const uint8*             x509_chain_ptr,
  uint32                   x509_chain_len,
  const uint8*             x509_root_cert_ptr,
  pbl_secx509_cert_type**  qc_ca_ptr,
  pbl_secx509_cert_type**  oem_ca_ptr,
  secboot_dbg_info_type*   dbg_info_ptr
);

/*===========================================================================

FUNCTION  SECBOOT_SHA1_API

DESCRIPTION

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
boolean secboot_SHA1_api
(
  const uint8 *hash_input_ptr,    /* starting ptr for hash   */
  uint32 input_size,              /* size to be hashed       */
  uint8 *hash_output_buffer       /* Output buffer to store the hash value */
);

/*===========================================================================

FUNCTION  SECBOOT_INIT_SHA1_API

DESCRIPTION

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void secboot_init_SHA1_api
(
  uint32        total_size,
  unsigned char *buf_ptr
);

/*===========================================================================

FUNCTION  SECBOOT_SHA1_HASH_AND_LOAD_API

DESCRIPTION

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
boolean secboot_SHA1_hash_and_load_api
(
  const unsigned char *buff_ptr,
  uint32        input_size,
  boolean       finish,
  unsigned char *digest_ptr
);

#endif /* SECBOOT_API_H */

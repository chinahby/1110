#ifndef BOOT_HEADERS_H
#define BOOT_HEADERS_H

/*===========================================================================

                          B o o t  H e a d e r s
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and type definitions for the
  configuration data stored in external flash and used by the boot ROM
  code.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/romboot/shared/boot_headers.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/07   ty      Code review changes. Remove 4C offset, update sbl header
04/23/07   ty      Initial LCU port.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_flash.h"
#include "secboot_types.h"
#include "pbl_error_handler.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* The function pointer type that performs SHA1 hashing */
typedef void (*pbl_sha1_func_type)( const uint8* buff_ptr,
                                    uint32 buff_size,
                                    uint8* digest_ptr 
                                  );
#endif
/*---------------------------------------------------------------------------
  Magic number definition for identifying NOR device
---------------------------------------------------------------------------*/
#define NOR_CODEWORD    0x00F4EBB5

/*---------------------------------------------------------------------------
  Magic number definition for identifying DBL Header.
---------------------------------------------------------------------------*/
#define MAGIC_NUM       0x73D71034     /* Used for NOR, NAND, etc. flash. */


#define ONENAND_16_BIT     ((uint32)NAND_8_BIT) /* 16 bit oneNAND */
#define MDOC_16_BIT        ((uint32)NAND_8_BIT) /* 16 bit Sandisk mDOC */

/*---------------------------------------------------------------------------
  Magic number definition for identifying PBL shared data.
---------------------------------------------------------------------------*/
#define BOOT_PBL_SHARED_DATA_MAGIC  0x78577020

#ifndef _ARM_ASM_

/*---------------------------------------------------------------------------
  Defines the codewords used to specify NAND data width, page size greater
  than 512 byte (future devices), and NAND device type of NAND.  These
  codewords are located in the DBL header. There are reserved codewords for
  future expansion

  Enumerations are integers according to C standard. 
---------------------------------------------------------------------------*/
typedef enum
{
  NO_NAND_DEVICE      = (int)0x00000000, /* Possibly no NAND device
                                            connected */
  NAND_8_BIT          = (int)0x844BDCD1, /* 8 bit NAND */
  NAND_8_BIT_16_BIT   = (int)0xD7344BD1, /* 8 bit and 16 bit NAND codeword */
  CW_RESERVED_0       = (int)0xBF50274A, /* Previously 16 bit oneNAND */
  CW_RESERVED_1       = (int)0x233CD127, /* Reserved */
  CW_RESERVED_2       = (int)0x78A68ABC, /* Previously 8 bit, 2K SUPERAND */
  UNPROGRAMMED_WORD   = (int)0xFFFFFFFF, /* 32 bit word not programmed */
} nand_codeword_enum_type;

typedef pbl_secx509_cert_type* (*secboot_verify_x509_chain_func_ptr_type)
  (
    const uint8*                   x509_chain_ptr,
    uint32                         x509_chain_len,
    const uint8*                   x509_root_cert_ptr,
    pbl_secx509_cert_type**        qc_ca_ptr,
    pbl_secx509_cert_type**        oem_ca_ptr,
    secboot_dbg_info_type*         dbg_info_ptr
  );

typedef boolean (*secboot_verify_code_signature_func_ptr_type)
  (
    const uint8*                  code_ptr,
    uint32                        code_len,
    uint64                        msm_hw_id,
    uint64                        sw_vsn_id,
    const uint8*                  signature_ptr,
    uint32                        signature_len,
    pbl_secx509_cert_type*        attestation_cert_ptr
  );

typedef boolean ( *compute_hash_and_load_func_ptr_type)
(
  unsigned char *buff_ptr,
  uint32        input_size,  
  boolean       finish,
  unsigned char *digest_ptr
);

typedef void (*init_hash_func_ptr_type)
(
  uint32        total_size,
  unsigned char *buf_ptr
);

  /*---------------------------------------------------------------------------
  Defines the SBL header type stored in external flash and read by the boot
  ROM code.

  This structure needs to be word aligned (4 bytes) because the size of
  the structure is used an argument to PBL_MEMCPY
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  codeword;            /* Codeword/magic number defining flash type
                                  information. */
  uint32  magic;               /* Magic number */
  uint32  RESERVED_0;          /* RESERVED */
  uint32  RESERVED_1;          /* RESERVED */
  uint32  RESERVED_2;          /* RESERVED */
  uint32  dbl_src;             /* Location of DBL in flash. This is given in
                                  byte offset from beginning of flash.  */
  uint8*  dbl_dest_ptr;        /* Pointer to location to store DBL in RAM.
                                  Also, entry point at which execution begins.
                                  */
  uint32  dbl_image_size;      /* Size of DBL image in bytes */
  uint32  dbl_code_size;       /* Size of code region in DBL image in bytes */
  uint8*  dbl_sig_ptr;         /* Pointer to images attestation signature */
  uint32  dbl_sig_size;        /* Size of the attestation signature in
                                  bytes */
  uint8*  dbl_cert_chain_ptr;  /* Pointer to the chain of attestation
                                  certificates associated with the image. */
  uint32  dbl_cert_chain_size; /* Size of the attestation chain in bytes */
  uint32  fsbl_image_size;     /* Size of FSBL image in bytes */
  uint32  fsbl_src;            /* Location of FSBL in flash. This is given in
                                  byte offset from beginning of flash.  */
  uint8*  fsbl_dest_ptr;       /* Pointer to location to store FSBL in RAM.
                                  */

  uint32  RESERVED_3;          /* RESERVED */
  uint32  RESERVED_4;          /* RESERVED */
  uint32  RESERVED_5;          /* RESERVED */
  uint32  RESERVED_6;          /* RESERVED */
} boot_sbl_header_type;

/*---------------------------------------------------------------------------
  Structure defining the functionality exported from the PBL to
  second stage bootloaders.
---------------------------------------------------------------------------*/
typedef struct 
{
  /*===========================================================================

  FUNCTION POINTER  INIT_SHARED_ROUTINES

  DESCRIPTION
    This function will initialize function pointers used by the PBL shared
    routines exported to later stage boot loaders.  The alternate error handler
    and the SHA1 hashing routine will be updated for the purposes of using
    PBL routines in a later stage boot loader.  The alternate error handler
    routines will only be updated if err_handler_ptr is not NULL; the SHA-1 
    hashing routine will only be updated if it is not NULL.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    shared_data of the PBL state is initialized.

  ===========================================================================*/
  void (*init_shared_routines)
  (
   pbl_error_handler_type err_handler_ptr,
   pbl_sha1_func_type sha1_func_ptr
  );
  
  /*===========================================================================

  FUNCTION POINTER  SHA1_HASH

  DESCRIPTION
    This function will compute the SHA-1 hash of a block of data and stores
    the message digest (hash) in the output buffer passed as input.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  pbl_sha1_func_type sha1_hash;

  /*===========================================================================

  FUNCTION POINTER  SECBOOT_VERIFY_X509_CHAIN

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
secboot_verify_x509_chain_func_ptr_type secboot_verify_x509_chain;

  /*===========================================================================

  FUNCTION POINTER  SECBOOT_VERIFY_CODE_SIGNATURE

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
secboot_verify_code_signature_func_ptr_type secboot_verify_code_signature;
}boot_pbl_fn_tbl_type;

/*---------------------------------------------------------------------------
  Defines the data structure that is filled with data that is shared with
  the DBL image. It may be necessary for the DBL to make a deep copy of
  this structure for its own use if it wipes out PBL memory.
---------------------------------------------------------------------------*/
typedef struct boot_pbl_shared_data_type
{
  /* Magic word indicating that the shared data structure is valid. */
  uint32                           magic;

  /* Fields below are filled in by initialization function */
  uint32 pbl_version;
  const boot_pbl_fn_tbl_type      *fn_tbl;
  uint8                           *boot_stack_base;

  /* The region of IMEM required for use by exported PBL routines. */
  uint8                           *shared_section_base;
  uint32                          shared_section_size;

  /* Bus speed set up in PBL */
  uint32                          clk_bus_speed;

  /* Fields below are filled in by flash module. This is a pointer
   * that points to a structure in IMEM. DBL needs to make its own
   * deep copy of this structure */
  boot_flash_shared_dev_info_type *flash_shared_data; 

  /* Fields below are filled in by auth module */
  boolean                         auth_enabled;
  uint8                           *root_cert_ptr;
  uint64                          msm_hw_id;

  /* Fields below are filled in by loader module. */
  boot_sbl_header_type            *dbl_hdr;

} boot_pbl_shared_data_type;

#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif  /* BOOT_HEADERS_H */


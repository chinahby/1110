#ifndef _MIIMAGEHEADER_H_
#define _MIIMAGEHEADER_H_

/*===========================================================================

               MULTI-IMAGE HEADER FILE

DESCRIPTION
  This header file contains declarations and type definitions for the
  Image Header Information.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2004-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/mibib/miimageheader.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/07   ps       Added support for SECBOOT 2.0
08/15/06   anb      Update NOR address.
07/20/06   rjain    Added support for CFG_DATA image type for NOR builds
06/02/05   tkuo     Change for progressive boot.
04/15/05   tkuo     Fix cfg_data address in IRAM for secured boot mode.
12/18/04   csh/tkuo Update NOR address map to 14M code and 2M EFS of 16M NOR.
09/14/04   tkuo     Created for MSM6275 non-secured boot mode.

============================================================================*/

#ifndef _ARM_ASM_

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifdef BUILD_BOOT_CHAIN
#include "boot_comdef.h"
#else
#include "comdef.h"
#endif
#include "mibib.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
===========================================================================*/
/*===========================================================================
 The following section is used by AMSS code
===========================================================================*/
/* This is the address of cfg_data located at IRAM; It goes with PBL code
** If the PBL code chagnes, please confirm this address again.
*/
#ifdef FEATURE_L4
#error code not present
#else
  #define NONSEC_CFG_DATA_ADDR    0xFFFF606C
  #define SEC_CFG_DATA_ADDR       0x78000904
#endif

/* This is used to see the size of each hash value;20 bytes per hash result */
#define SECBOOT_SIGNATURE_SIZE  20   /* SHA-1 digest size  */


typedef enum
{
  NONE_IMG = 0,
  OEM_SBL_IMG,
  AMSS_IMG,
  QCSBL_IMG,
  HASH_IMG,
  NANDPRG_IMG,
  CFG_DATA,
  NORPRG_IMG,
  HOSTDL_IMG,
  FSBL_IMG,
  DBL_IMG,
  OSBL_IMG,
  APPS_IMG,
  APPSBL_IMG,
  MAX_IMG = 0x7FFFFFFF
} image_type;

/* Image component in order, code, signature, certificate */
typedef enum
{
  NONE_SEG =0,
  CODE_SEG,      /* Code        */
  SIGNATURE_SEG, /* Signature   */
  CERT_SEG,      /* Certificate */
  MAX_ITEM = 0x7FFFFFFF
} image_component_type;

typedef struct
{
  image_type image_id;       /* Identifies the type of image this header
                                 represents (OEM SBL, AMSS, Apps boot loader,
                                 etc.). */
  uint32 header_vsn_num;     /* Header version number. */
  uint32 image_src;          /* Location of image in flash: Address of
                                 image in NOR or page/sector offset to image
                                 from page/sector 0 in NAND/SUPERAND. */
  uint8* image_dest_ptr;     /* Pointer to location to store image in RAM.
                                 Also, entry point at which image execution
                                 begins. */
  uint32 image_size;         /* Size of complete image in bytes */
  uint32 code_size;          /* Size of code region of image in bytes */
  uint8* signature_ptr;      /* Pointer to images attestation signature */
  uint32 signature_size;     /* Size of the attestation signature in
                                 bytes */
  uint8* cert_chain_ptr;     /* Pointer to the chain of attestation
                                 certificates associated with the image. */
  uint32 cert_chain_size;    /* Size of the attestation chain in bytes */

} mi_boot_image_header_type;

typedef struct
{
  image_type image_id;       /* Identifies the type of elf image this header
                                 represents (OEM SBL, AMSS, etc.). */
  uint32 image_src;          /* Location of elf image in flash: Address of
                                 image in NOR or page/sector offset to image
                                 from page/sector 0 in NAND/SUPERAND. */
} elf_image_info_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#endif  /* _ASM_ARM_ */
#endif  /* _MIIMAGEHEADER_H_ */


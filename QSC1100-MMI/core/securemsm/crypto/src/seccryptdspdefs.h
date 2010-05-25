#ifndef SECCRYPTDSPDEFS_H
#define SECCRYPTDSPDEFS_H
/*===========================================================================

                      S E C U R I T Y   S E R V I C E S

       A R M - Q D S P   C R Y P T O G R A P H I C   I N T E R F A C E
                   
            D E F I N I T I O N S   F R O M   Q D S P   I S O D

DESCRIPTION
  This is the header file defining shared values and data structures
  between ARM and QDSP.
  The definitions are from the Security Services aDSP ISOD document.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2004-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/seccryptdspdefs.h#1 $ 
  $DateTime: 2007/06/26 13:17:46 $ 
  $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/07   rv      added support for SHA256 on dsp
08/31/05   df      Changes to support SFS hipri/devkey/sha/aes codes
02/25/05   sb      Added MSM device key based command codes for AES128 and Secure modmath
03/04/04   om      Created module.

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_SEC
#ifdef FEATURE_SEC_DSP_CRYPTO

#include "seccryptdsperrno.h"


/*===========================================================================

                         CONSTANTS AND DEFINITIONS

===========================================================================*/

/* The shared command queue uses individual IDs for the different      */
/* applets. The following define is for crypto applet commands.        */
#define SECCRYPTDSP_SS_CRYPTO_CMD_CODE     0x000B

/* The following values from the crypto DSP interface spec combine via */
/* logical ORing to form a 16-bit command to the crypto DSP image:     */
/*  + combine cipher-algo / mode / direction                           */
/*  + combine hash-algo / hash-mode                                    */
/*  + use standalone value                                             */
#define SECCRYPTDSP_CACHE_DELETE_CMD_CODE  0x0100  /* Standalone value */
#define SECCRYPTDSP_CACHE_FLUSH_CMD_CODE   0x0101  /* Standalone value */

#define SECCRYPTDSP_DECRYPT_CMD_CODE       0x0000  /* Cipher direction */
#define SECCRYPTDSP_ENCRYPT_CMD_CODE       0x0001  /* Cipher direction */

#define SECCRYPTDSP_ECB_MODE_CMD_CODE      0x0000  /* Cipher mode      */
#define SECCRYPTDSP_CBC_MODE_CMD_CODE      0x0002  /* Cipher mode      */

#define SECCRYPTDSP_ARC4_CMD_CODE          0x0200  /* Cipher algo      */
#define SECCRYPTDSP_DES_CMD_CODE           0x0300
#define SECCRYPTDSP_3DES_CMD_CODE          0x0304
#define SECCRYPTDSP_AES128_CMD_CODE        0x0400
#define SECCRYPTDSP_AES192_CMD_CODE        0x0404
#define SECCRYPTDSP_AES256_CMD_CODE        0x0408
#define SECCRYPTDSP_AES128_MSM_DEVKEY_CMD_CODE 0x040C /* secure key aes */

#define SECCRYPTDSP_SHA_CMD_CODE           0x0500  /* Hash algo        */
#define SECCRYPTDSP_MD5_CMD_CODE           0x0600  
#define SECCRYPTDSP_SHA256_CMD_CODE        0x0900
#define SECCRYPTDSP_SHA_HMAC_DEVKEY_CMD_CODE 0x0508 /* secure hmac */

#define SECCRYPTDSP_CONTINUE_CMD_CODE      0x0000  /* Hash mode        */
#define SECCRYPTDSP_FINAL_CMD_CODE         0x0001  /* Hash mode        */

#define SECCRYPTDSP_MODEXP_CMD_CODE        0x0700  /* Standalone value */
#define SECCRYPTDSP_SEEK_PRIME_CMD_CODE    0x0701  /* Standalone value */
#define SECCRYPTDSP_MODEXP_MSM_DEVKEY_ENC_EXP_CMD_CODE 0x0702  /* Standalone value */

#define SECCRYPTDSP_DME_IO_TEST_CMD_CODE   0x0800  /* Standalone value */


/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/* General command structure definition */
typedef struct
{
  /* The pointers are treated as a structure of two ptrs */
  uint32   this_ptr; /* This pointer for the command structure */
  uint32   ctx_ptr;  /* Pointer to the context structure */

  /* General command information */
  uint32   cmd_code;

  /* DME I/O tables */
  uint32   dme_input_ptr;    /* Address of DME input table           */
  uint32   dme_output_ptr;   /* Address of DME output table          */
  uint32   dummy;            /* Address of odd-byte table (obsolete) */
  uint32   dme_input_len;    /* Size of DME input table              */
  uint32   dme_output_len;   /* Size of DME output table             */

  /* Outcome of the operation (written by DSP) */
  uint32   context_state;
  uint32   cmd_status_sem;
} seccryptdsp_cmd_type;

/* Definition of DME table entries */
typedef struct seccryptdsp_dme_table_entry_s
{
  uint32  data_ptr;         /* Pointer to payload (address in host order)  */
  uint32  data_len;         /* Length of payload (LS 24 bit in host order) */
} seccryptdsp_dme_table_entry_type;


#endif /* FEATURE_SEC_DSP_CRYPTO */

#endif /* FEATURE_SEC */

#endif /* SECCRYPTDSPDEFS_H */

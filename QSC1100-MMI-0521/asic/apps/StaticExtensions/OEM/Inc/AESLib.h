#ifndef AESLIB_H
#define AESLIB_H


/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
  $Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/AESLib.h#1 $
  $DateTime: 2004/06/17 18:08:45 $ $Author: jayanthm $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/04   jay     Created file
===========================================================================*/

#include "OEMFeatures.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"

#ifdef FEATURE_SEC_DRM

/*---------------------------------------------------------------------------
  Defines
---------------------------------------------------------------------------*/
#define AES_BLOCK_SIZE     16
#define AES128_KEY_SIZE    16
#define AES256_KEY_SIZE    32

/*---------------------------------------------------------------------------
  Enums
---------------------------------------------------------------------------*/
/* Mode  */
typedef enum
{
  MODE_CBC  = 0,
  MODE_ECB  = 1
} AESModeEnumType;

/* Padding types */
typedef enum
{
  NO_PADDING = 0,
  RFC2630_PADDING,
  MAX_PADDING_TYPE
} AESPaddingEnumType;

/*---------------------------------------------------------------------------
  Structs
---------------------------------------------------------------------------*/
/* Tho following mimics the context structure from aes.h */
typedef struct
{
  uint32  sub_key[4 * AES_BLOCK_SIZE]; /* the key schedule  */
  uint32  Nkey;               /* the number of words in the key input block */
  uint32  Nrnd;               /* the number of cipher rounds                */
} AESCTXType;


/* Struct that holds the AES Cipher Info */
typedef struct
{
  boolean  enc_subkeys_generated;
  boolean  dec_subkeys_generated;

  /* Indicates CBC or ECB mode (CBC is default) */
  AESModeEnumType  mode;
  
  /* Padding */
  struct
  {
    AESPaddingEnumType  type;
    uint8               value;
  } padding;

  /* AES info */
  struct
  {
    uint8       iv[ AES_BLOCK_SIZE ];
    uint8       key[ AES256_KEY_SIZE ];
    AESCTXType  ctx;
  } enc;

  struct
  {
    uint8       iv[ AES_BLOCK_SIZE ];
    uint8       key[ AES256_KEY_SIZE ];
    AESCTXType  ctx;
  } dec;

}AESInstanceType;

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION AESLib_Decrypt()

DESCRIPTION
  Using the AES cipher algorithm, decrypt the ciphertext at in_msg_ptr
  and write the plaintext output to out_msg_ptr.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    SUCCESS  on sucessful decryption
    EFAILED  Failed to decrypt
    EBADPARM

SIDE EFFECTS
  None.
===========================================================================*/
int AESLib_Decrypt
(
  AESInstanceType*  instance_ptr,
  uint8*            in_msg_ptr,
  uint8*            out_msg_ptr,
  uint32            in_msg_len
);

/*===========================================================================

FUNCTION AESLib_Encrypt()

DESCRIPTION
  Using the AES cipher algorithm, encrypt the plaintext at in_msg_ptr
  and write the ciphertext output to out_msg_ptr.
  
DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    SUCCESS  on sucessful encryption
    EFAILED  Failed to encrypt
    EBADPARM


SIDE EFFECTS
  None.
===========================================================================*/
int  AESLib_Encrypt
(
  AESInstanceType*  instance_ptr,
  uint8*            in_msg_ptr,
  uint8*            out_msg_ptr,
  uint32            in_msg_len
);

/*===========================================================================

FUNCTION AESLib_Unpad()

DESCRIPTION
  This function verifies and removes the specified padding from the
  deciphered data.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    SUCCESS  on sucessful undpadding
    EFAILED  Failed to unpad

SIDE EFFECTS
  None.
===========================================================================*/
int AESLib_Unpad
(
  AESInstanceType*  instance_ptr,
  uint8*            msg_ptr,
  uint32*           msg_len_ptr
);






















#endif /* FEATURE_SEC_DRM */
#endif /* AESLIB_H */

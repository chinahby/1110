/*!
 * @header mf_drm_media_cipher.h
 *
 * GENERAL DESCRIPTION:
 *    This library interface allows you to encrypt and decrypt media.
 *    
 *    The library is an interface to an AES crypto core which does performs the
 *    core functionality of the library.
 *
 *    ENCRYPTION / DECRYPTION NOTES:
 *    So in order to encrypt media the sequence of calls would be:
 *    MF_GenerateProgramKey to generate the program key unless you already have it
 *    and then repeat calls to MF_FormMediaCryptIVData and MF_EncryptMediaData till
 *    all of the data is encrypted.
 *    For each call to MF_EncryptMediaData, generate new IV Data.  
 *
 *    To decrypt media use:
 *    Call MF_FormMediaCryptIVData and MF_DecryptMediaData and repeat these calls
 *    till all media is decrypted.
 *   
 *    IV DATA NOTES:
 *    For Media Encryption/Decryption, use MF_FormMediaCryptIVData to form the IV 
 *    data. 
 *
 * IMPLEMENTATION-SPECIFIC COMMENTS:
 *    None
 *   
 * AUTHOR: Pooja Aggarwal
 *
 * (c) COPYRIGHT 2004 QUALCOMM Incorporated.
 *
 */  

/* =======================================================================
                           Perforce Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/mfdrm/main/latest/src/mf_drm_media_cipher.h#3 $
$DateTime: 2008/05/08 10:55:34 $
$Change: 656201 $

========================================================================== */

#ifndef HEADER_MF_DRM_MEDIA_CIPHER_H
#define HEADER_MF_DRM_MEDIA_CIPHER_H

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include <stdio.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "mf_drm_cryptotypes.h"


#ifdef  __cplusplus
extern "C" {
#endif
//----------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------
/*!
 * @defined MF_MEDIA_CIPHER_BLOCK_SIZE_BYTES
 * @abstract
 *    Block size for the media cipher API. It defines the key size and 
 *    IV data size for the AES Core. 
 */
#define MF_MEDIA_CIPHER_BLOCK_SIZE_BYTES MF_CRYPTOTYPES_CIPHER_BLOCK_SIZE

/*!
 *  @typedef MF_Media_Cipher_Status
 *  @abstract Error/Success status for functions in the Media Cipher API
 *  @constant MF_MEDIA_CIPHER_SUCCESS Operation successful
 *  @constant MF_MEDIA_CIPHER_FAIL Operation unsuccessful
 *  @constant MF_MEDIA_CIPHER_INVALID_INPUT_ERR Invalid input to the API
 *  @constant MF_MEDIA_CIPHER_BLOCK_SIZE_ERR Invalid block size used in data
 *  @constant MF_MEDIA_CIPHER_BUFFER_SMALL_ERR Buffer overflow error
 *  @constant MF_MEDIA_CIPHER_KEY_INIT_ERR Error while initializing AES key
 *  @constant MF_MEDIA_CIPHER_RNG_ERR Error while generating random numbers
 */
 
typedef enum
{
    MF_MEDIA_CIPHER_SUCCESS,
    MF_MEDIA_CIPHER_FAIL,
    MF_MEDIA_CIPHER_INVALID_INPUT_ERR,
    MF_MEDIA_CIPHER_BLOCK_SIZE_ERR,
    MF_MEDIA_CIPHER_BUFFER_SMALL_ERR,
    MF_MEDIA_CIPHER_KEY_INIT_ERR,
    MF_MEDIA_CIPHER_RNG_ERR
} MF_Media_Cipher_Status;


//----------------------------------------------------------------------------
// Global Data Declarations
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Static Data Declarations
//----------------------------------------------------------------------------
 

//----------------------------------------------------------------------------
// Function Prototypes
//----------------------------------------------------------------------------

/*!
 * @function MF_EncryptMediaData
 * @abstract Function to encrypt media data using AES CTR mode
 * @discussion
 *   This function takes as input a program key, a media data
 *   buffer, IV data ( initialization vector) and returns the media
 *   data encrypted with the program key
 *
 * @param inputProgramKey Key for encryption
 * @param inputMediaData Media data to be encrypted
 * @param inputMediaDataLength Input media data length in bytes
 * @param inputIvData IV data
 * @param outputMediaData Encrypted data 
 * @param outputMediaDataLength In-out parameter: In - Length of buffer passed in, 
 *                              Out - Length of buffer filled 
 * @result MF_Media_Cipher_Status
 */

MF_Media_Cipher_Status MF_EncryptMediaData ( const MFProgramKey * inputProgramKey,
                                             const CRYPTOTYPES_UINT8 * inputMediaData,
                                             const CRYPTOTYPES_UINT32 inputMediaDataLength,
                                             const MFIvData * inputIvData,
                                             CRYPTOTYPES_UINT8 * outputMediaData,
                                             CRYPTOTYPES_UINT32 * outputMediaDataLength );
/*!
 * @function MF_DecryptMediaData
 * @abstract Function to decrypt media data using AES CTR mode
 * @discussion
 *   This function takes as input a program key, a media data
 *   buffer, IV data ( initialization vector) and returns the media
 *   data decrypted with the program key
 *
 * @param inputProgramKey Key for decryption
 * @param inputMediaData Input media data to be decrypted
 * @param inputMediaDataLength Input media data length in bytes
 * @param inputIvData IV data
 * @param outputMediaData Decrypted data
 * @param outputMediaDataLength In-out parameter: In - Length of buffer passed in, 
 *                              Out - Length of buffer filled 
 * @result MF_Media_Cipher_Status
 */ 

MF_Media_Cipher_Status MF_DecryptMediaData ( const MFProgramKey * inputProgramKey,
                                             const CRYPTOTYPES_UINT8 * inputMediaData,
                                             const CRYPTOTYPES_UINT32 inputMediaDataLength,
                                             const MFIvData * inputIvData,
                                             CRYPTOTYPES_UINT8 * outputMediaData,
                                             CRYPTOTYPES_UINT32 * outputMediaDataLength );


/*!
 * @function MF_FormMediaCryptIVData
 * @abstract Function to generate IV data prior for media encryption/decryption
 * @discussion
 *   This function is called to generate IV data before Media encryption / decryption
 *   The function does bit crunching on the input block Number and pid info
 *   to generate this IV data. For more info, look for the algo in the implementation
 *   file
 *
 * @param blockNumber Sequential block number of the block being encrypted
 * @param pid PID of the program being processed
 * @param pidLen Length of the PID
 * @param outputIvData Iv Data generated
 *
 * @result MF_Media_Cipher_Status
 */
MF_Media_Cipher_Status MF_FormMediaCryptIVData ( const CRYPTOTYPES_UINT32 blockNumber,
                                                 const CRYPTOTYPES_UINT8 * pid,
                                                 const CRYPTOTYPES_UINT32 pidLen,
                                                 MFIvData * outputIvData );

#ifdef  __cplusplus
}
#endif

#endif // HEADER_MF_DRM_MEDIA_CIPHER_H 

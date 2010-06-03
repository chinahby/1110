// ============================================================================
// FILE: mf_drm_media_cipher.c
// 
// GENERAL DESCRIPTION:
//     See mf_drm_media_cipher.h for a general description.
// 
// IMPLEMENTATION-SPECIFIC COMMENTS:
//     None.
//               (c) COPYRIGHT 2004 QUALCOMM Incorporated.
//                          All Rights Reserved.
//                       QUALCOMM Proprietary/GTDR
// ============================================================================

/* =======================================================================
                           Perforce Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/mfdrm/main/latest/src/mf_drm_media_cipher.c#3 $
$DateTime: 2008/05/08 10:55:34 $
$Change: 656201 $

========================================================================== */

// ----------------------------------------------------------------------------
// Include Files
// ----------------------------------------------------------------------------
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "mf_drm_media_cipher.h"
#include "mf_drm_aes.h"

#include <string.h>

#ifndef MFDRM_NO_QTV_MSG
#include "qtv_msg.h"
#endif

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
typedef struct
{
    CRYPTOTYPES_UINT8 key[MF_MEDIA_CIPHER_BLOCK_SIZE_BYTES];
} MFAesKey;

// ----------------------------------------------------------------------------
// Global Data Declarations
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Static Data Declarations
// ----------------------------------------------------------------------------
 

// ----------------------------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------------------------
static MF_Media_Cipher_Status MF_AESCtrEncrypt ( const MFAesKey * inputKey,
                                                 const CRYPTOTYPES_UINT8 * inputData,
                                                 const CRYPTOTYPES_UINT32 inputDataLength,
                                                 const MFIvData * inputIvData,
                                                 CRYPTOTYPES_UINT8 * outputData,
                                                 CRYPTOTYPES_UINT32 * outputDataLength );


// ----------------------------------------------------------------------------
// Function Definitions
// ----------------------------------------------------------------------------


// ============================================================================
// FUNCTION: MF_EncryptMediaData
// 
// DESCRIPTION:
// 
// 
// RETURN VALUE:
//    None
// 
// SIDE EFFECTS:
//    None
// ============================================================================

MF_Media_Cipher_Status MF_EncryptMediaData ( const MFProgramKey * inputProgramKey,
                                             const CRYPTOTYPES_UINT8 * inputMediaData,
                                             const CRYPTOTYPES_UINT32 inputMediaDataLength,
                                             const MFIvData * inputIvData,
                                             CRYPTOTYPES_UINT8 * outputMediaData,
                                             CRYPTOTYPES_UINT32 * outputMediaDataLength )
{

    // Sanity Checks
    if ( (inputProgramKey == NULL) ||
         (inputMediaData == NULL) ||
         (inputIvData == NULL) ||
         (outputMediaData == NULL) ||
         (outputMediaDataLength == NULL))
    {
#ifndef MFDRM_NO_QTV_MSG
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_EncryptMediaData: Error: Invalid Input\n");
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_EncryptMediaData: inputProgramKey: %u\n", inputProgramKey);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_EncryptMediaData: inputMediaData: %u\n", inputMediaData);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_EncryptMediaData: inputIvData: %u\n", inputIvData);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_EncryptMediaData: outputMediaData: %u\n", outputMediaData);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_EncryptMediaData: outputMediaDataLength: %u\n", outputMediaDataLength);
#endif
        return MF_MEDIA_CIPHER_INVALID_INPUT_ERR;
    }
 
    if (*outputMediaDataLength < inputMediaDataLength)
    {
#ifndef MFDRM_NO_QTV_MSG
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_EncryptMediaData: Error: Output buffer small\n");
#endif
        return MF_MEDIA_CIPHER_BUFFER_SMALL_ERR;
    }
    
    // Encrypt the media data 
    return MF_AESCtrEncrypt ( (MFAesKey *)inputProgramKey, 
                              inputMediaData, 
                              inputMediaDataLength, 
                              inputIvData, 
                              outputMediaData, 
                              outputMediaDataLength );
}

// ============================================================================
// FUNCTION: MF_DecryptMediaData
// 
// DESCRIPTION:
// 
// 
// RETURN VALUE:
//    None
// 
// SIDE EFFECTS:
//    None
// ============================================================================

MF_Media_Cipher_Status MF_DecryptMediaData ( const MFProgramKey * inputProgramKey,
                                             const CRYPTOTYPES_UINT8 * inputMediaData,
                                             const CRYPTOTYPES_UINT32 inputMediaDataLength,
                                             const MFIvData * inputIvData,
                                             CRYPTOTYPES_UINT8 * outputMediaData,
                                             CRYPTOTYPES_UINT32 * outputMediaDataLength )

{

    // Sanity Checks 
    if ( (inputProgramKey == NULL) ||
         (inputMediaData == NULL) ||
         (inputIvData == NULL) ||
         (outputMediaData == NULL) ||
         (outputMediaDataLength == NULL))
    {
#ifndef MFDRM_NO_QTV_MSG
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_DecryptMediaData: Error: Invalid Input\n");
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_DecryptMediaData: inputProgramKey: %u\n", inputProgramKey);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_DecryptMediaData: inputMediaData: %u\n", inputMediaData);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_DecryptMediaData: inputIvData: %u\n", inputIvData);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_DecryptMediaData: outputMediaData: %u\n", outputMediaData);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_DecryptMediaData: outputMediaDataLength: %u\n", outputMediaDataLength);
#endif
        return MF_MEDIA_CIPHER_INVALID_INPUT_ERR;
    }
   
    if (*outputMediaDataLength < inputMediaDataLength)
    {
#ifndef MFDRM_NO_QTV_MSG
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_DecryptMediaData: Error: Output buffer small\n");
#endif
        return MF_MEDIA_CIPHER_BUFFER_SMALL_ERR;
    }
    
    // Decrypt the media data 
    return MF_AESCtrEncrypt ( (MFAesKey *)inputProgramKey, 
                              inputMediaData, 
                              inputMediaDataLength, 
                              inputIvData, 
                              outputMediaData, 
                              outputMediaDataLength );

}

// ============================================================================
// FUNCTION: MF_FormMediaCryptIVData
// 
// DESCRIPTION:
// 
// 
// RETURN VALUE:
//    None
// 
// SIDE EFFECTS:
//    None
// ============================================================================

MF_Media_Cipher_Status MF_FormMediaCryptIVData ( const CRYPTOTYPES_UINT32 blockNumber,
                                                 const CRYPTOTYPES_UINT8 * pid,
                                                 const CRYPTOTYPES_UINT32 pidLen,
                                                 MFIvData * outputIvData )
{
    
    unsigned int ivDataLen;
    unsigned int offset;
    unsigned int pidLenToCopy;
    
    // Sanity Checks 
    if ((pid == NULL) ||
        (pidLen == 0) ||
        (outputIvData == NULL) ||
        (blockNumber == 0))
    {
#ifndef MFDRM_NO_QTV_MSG
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_FormMediaCryptIVData: Error: Invalid Input\n");
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_FormMediaCryptIVData: pid: %u\n", pid);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_FormMediaCryptIVData: pidLength: %u\n", pidLen);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_FormMediaCryptIVData: outputIvData: %u\n", outputIvData);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_FormMediaCryptIVData: blockNumber: %u\n", blockNumber);
#endif
        return MF_MEDIA_CIPHER_INVALID_INPUT_ERR;
    }
    
    // Algorithm: First zeroize the Iv Data. 
    // Then copy the block number in big endian format to the first half 
    // of IV Data offsetting it so that it is right justified
    // Next copy the pid to second half of the IV offsetting it so that it
    // too is right justified in the second half of the IV field
    // Incase pid len is greater than half of IV length, then copy the
    // (IV Len)/2 MSBs of pid to the second half of IV
    
    ivDataLen = sizeof (outputIvData->data);
    memset ( outputIvData->data, 0, ivDataLen);
    
    // Copy block number to ivData 
    offset = ivDataLen/2 - sizeof (CRYPTOTYPES_UINT32);
    PUTU32((outputIvData->data + offset), blockNumber);
    
    // Copy ivDataLen/2 number of MSBs of pid to IV Data 
    if ( pidLen < ivDataLen/2 )
    {
        offset = ivDataLen - pidLen;
        pidLenToCopy = pidLen;
    }
    else
    {
        offset = ivDataLen/2;
        pidLenToCopy = ivDataLen/2;
    }
    
    memcpy ( (outputIvData->data + offset), pid, pidLenToCopy );
    
    return MF_MEDIA_CIPHER_SUCCESS;
}


// ============================================================================
// FUNCTION: MF_AESCtrEncrypt
// 
// DESCRIPTION:
// 
// 
// RETURN VALUE:
//    None
// 
// SIDE EFFECTS:
//    None
// ============================================================================

static MF_Media_Cipher_Status MF_AESCtrEncrypt ( const MFAesKey * inputKey,
                                                 const CRYPTOTYPES_UINT8 * inputData,
                                                 const CRYPTOTYPES_UINT32 inputDataLength,
                                                 const MFIvData * inputIvData,
                                                 CRYPTOTYPES_UINT8 * outputData,
                                                 CRYPTOTYPES_UINT32 * outputDataLength )
{
    CRYPTOTYPES_UINT8 counterBuffer[MF_MEDIA_CIPHER_BLOCK_SIZE_BYTES];
    unsigned int number;
    int status;
    AES_KEY encryptionKey;
    MFIvData tempIv;
    unsigned int index;


    // Initialize the state information variables for AES CTR mode 
    number = 0;
    memset ( counterBuffer, 0, sizeof (counterBuffer));
    memcpy ( tempIv.data, inputIvData->data, sizeof (inputIvData->data));

    // Initialize the key 
    status = AES_set_encrypt_key( (unsigned char *)inputKey->key, 
                                  sizeof (inputKey->key) * 8, &encryptionKey);    
    
    if (status < 0)
    {
#ifndef MFDRM_NO_QTV_MSG
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "MF_AESCtrEncrypt: Error: Key Initialization \n");
#endif
        return MF_MEDIA_CIPHER_KEY_INIT_ERR;
    }

    // Encrypt the  data 
    AES_ctr128_encrypt ( inputData, outputData, inputDataLength,
                         &encryptionKey, (unsigned char *)tempIv.data, counterBuffer, &number );

    *outputDataLength = inputDataLength;

    // NOTE: The parameter number is used internally by AES CTR to keep track
    // of how many bytes of IV have been XORed with input data in a block. It
    // can be used later along with counterBuffer to continue with the same 
    // initial counter - i.e. to call AES-CTR a second time in conitinuation 
    // to the last AES-CTR call. We however do not need this state as we are
    // re-initializing the IV (counter) for each block. - Pooja 06/02/2004
    
    // Zero out the state info 
    for (index = 0; index < 10; index++)
    {
        memset (&encryptionKey, 0, sizeof (encryptionKey));
        memset ( counterBuffer, 0, sizeof (counterBuffer));
    }
    
    return MF_MEDIA_CIPHER_SUCCESS;
}



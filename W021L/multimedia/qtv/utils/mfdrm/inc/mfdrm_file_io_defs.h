/*!
 * @header mfdrm_file_io_defs.h
 *
 * GENERAL DESCRIPTION:
 *    Key structure definitions for mfdrm_file_io
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

$Header: //source/qcom/qct/multimedia/qtv/utils/mfdrm/main/latest/inc/mfdrm_file_io_defs.h#3 $
$DateTime: 2008/05/08 10:55:34 $
$Change: 656201 $

========================================================================== */

#ifndef _MFDRM_FILE_IO_DEFS_H_
#define _MFDRM_FILE_IO_DEFS_H_

// ----------------------------------------------------------------------------
// Include files 
// ----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif
// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

/*!
 * @enum MFDRM_FILEIO_KEY_TYPE_ENUM
 * @abstract 
 *    Enum for key types used in IMEDIA SetMediaParm to set decryption key
 * @const MFDRM_FILEIO_KEY_TYPE_AES_CTR_16_PID_6 Key with 16 byte AES-CTR key and 6 byte PID
 */
enum MFDRM_FILEIO_KEY_TYPE_ENUM
{
    MFDRM_FILEIO_KEY_TYPE_AES_CTR_16_PID_6 = 1
};

/*!
 * @enum MFDRM_FILEIO_KEY_VER_ENUM
 * @abstract 
 *    Enum for key versions used in IMEDIA SetMediaParm to set decryption key
 * @const MFDRM_FILEIO_KEY_VER_AES_CTR_16_PID_6 Key with 16 byte AES-CTR key and 6 byte PID
 */
enum MFDRM_FILEIO_KEY_VER_ENUM
{
    MFDRM_FILEIO_KEY_VER_AES_CTR_16_PID_6 = 1
};

/*!
 * @defined MFDRM_FILEIO_KEY_LEN_AES_CTR_16_PID_6
 * @abstract 
 *    AES Key length for key type MFDRM_FILEIO_KEY_TYPE_AES_CTR_16_PID_6
 */
#define MFDRM_FILEIO_KEY_LEN_AES_CTR_16_PID_6 16

/*!
 * @defined MFDRM_FILEIO_KEY_LEN_AES_CTR_16_PID_6
 * @abstract
 *    Pid length for key type MFDRM_FILEIO_KEY_TYPE_AES_CTR_16_PID_6
 */
#define MFDRM_FILEIO_PID_LEN_AES_CTR_16_PID_6 6

// -----------------------------------------------------------------------
// Type Declarations
// ----------------------------------------------------------------------- 

/*!
 * @typedef MFDRM_FILEIO_KEY_BASE
 * @abstract 
 *    Base key type for all MFDRM decryption keys set using IMEDIA SetMediaParm
 * @field type Key type (from enum MFDRM_FILEIO_KEY_TYPE_ENUM)
 * @field version Key version from (from enum MFDRM_FILEIO_KEY_VER_ENUM)
 */
typedef struct _MFDRM_FILEIO_KEY_BASE
{
    unsigned int type;
    unsigned int version;
} MFDRM_FILEIO_KEY_BASE;

/*!
 * @typedef MFDRM_FILEIO_KEY_V1
 * @abstract 
 *    Version 1 of MFDRM_FILEIO_KEY 
 * @field type Key type (from enum MFDRM_FILEIO_KEY_TYPE_ENUM)
 * @field version Key version from (from enum MFDRM_FILEIO_KEY_VER_ENUM)
 * @field dwBlockSize Block size, in bytes, for decryption and IV
 * @field decryptKey AES 16 byte key for decryption
 * @field pid 6 byte PID for IV
 */
typedef struct _MFDRM_FILEIO_KEY_V1
{
    unsigned int type;
    unsigned int version;
    unsigned int dwBlockSize;
    unsigned char decryptKey[MFDRM_FILEIO_KEY_LEN_AES_CTR_16_PID_6];
    unsigned char pid[MFDRM_FILEIO_PID_LEN_AES_CTR_16_PID_6];
} MFDRM_FILEIO_KEY_V1;

// ----------------------------------------------------------------------------
// Global Constant Declarations
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Global Data Declarations
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // _MFDRM_FILE_IO_DEFS_H_

/*!
 * @header mfdrm_file_io.h
 *
 * GENERAL DESCRIPTION:
 *    Interface definition for mfdrm file io shim layer around AES CTR core
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

$Header: //source/qcom/qct/multimedia/qtv/utils/mfdrm/main/latest/inc/mfdrm_file_io.h#3 $
$DateTime: 2008/05/08 10:55:34 $
$Change: 656201 $

========================================================================== */

#ifndef _MFDRM_FILE_IO_H_
#define _MFDRM_FILE_IO_H_

// ----------------------------------------------------------------------------
// Include files 
// ----------------------------------------------------------------------------

#include <stdio.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

/*!
 * @typedef MFDRM_FILE
 * @abstract 
 *    MFDRM File type structure
 * @field filePtr File handle for opened file
 * @field type Type of decryption key used in file open 
 * @field version Version of decryption key used in file open 
 * @field key Key to be used to decrypt MFDRM file
 * @field keyLength Length of key field in bytes
 * @field blockSize Decryption block size to be used
 * @field pidLength Length of pid field in bytes
 * @field pid PID to be used for IV generation
 */
typedef struct _MFDRM_FILE
{
    FILE    * filePtr;
    unsigned int type;
    unsigned int version;
    unsigned char * key;
    unsigned int keyLength;
    unsigned int blockSize;
    unsigned char * pid;
    unsigned int pidLength;
} MFDRM_FILE;


// ----------------------------------------------------------------------------
// Global Data Declarations
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Static Data Declarations
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------------------------

/*!
 * @function MFDRM_FileOpen
 * @abstract File open function for MFDRM FILE IO
 * @discussion
 *   This function creates an instance of MFDRM_FILE and returns a pointer
 *   to the instance. It instantiates the instance with the input decryption
 *   key and the file handle for the encrypted file whose pathname is filename.
 *   In case of error, it returns NULL. The argument mode must be "r" or "rb"
 *   since no other modes of operation are supported
 *
 * @param filename Pathname for the file to be opened
 * @param mode Mode in which to open the file - "r" or "rb"
 * @param decryptionKey Key material to be used for decryption
 * @param size Size of decryptionKey in bytes
 *
 * @result MFDRM_FILE handle
 */

MFDRM_FILE * MFDRM_FileOpen (const char * filename, const char *mode, const void * decryptionKey, const unsigned int size);

/*!
 * @function MFDRM_IsFileOpen
 * @abstract
 *   This function tells whether the file pointed to, is open or not
 *
 * @param fp MFDRM_FILE handle of the file to be tested
 *
 * @result true/false
 */

bool MFDRM_IsFileOpen(MFDRM_FILE *fp);

/*!
 * @function MFDRM_FileRead
 * @abstract File read function for MFDRM FILE IO
 * @discussion
 *   This function reads upto numelements of length size from the 
 *   MFDRM_FILE handle into the array pointed to by buffer. It uses the
 *   key to decrypt the data read before writing to the buffer.
 *
 * @param buffer Array to store data read
 * @param size Size of each element to be read
 * @param numelements Number of elements to be read
 * @param fp Handle of MFDRM_FILE from which to read
 *
 * @result Number of elements read. In case of error, it returns 0 and 
 *         sets the error indicator for fp 
 */

uint32  MFDRM_FileRead (void *buffer, uint32 size, uint32 numelements,  MFDRM_FILE *fp);

/*!
 * @function MFDRM_FileSeek
 * @abstract File seek function for MFDRM FILE IO
 * @discussion
 *   This function mimics the OSCL_FileSeek functionality and performs the seek
 *   operation on the file. No MediaFLO DRM specific changes are needed
 *
 * @param fp MFDRM_FILE handle in which to seek
 * @param offset Number of bytes to skip
 * @param origin From where to start the seek operation
 *
 * @result -1 for error
 */

int32 MFDRM_FileSeek (MFDRM_FILE *fp,  int32 offset, int32 origin);

/*!
 * @function MFDRM_FileTell
 * @abstract Returns the current file pointer position
 * @discussion
 *   This function mimics the OSCL_FileTell functionality.
 *
 * @param fp MFDRM_FILE handle whose file pointer position is to be returned
 *
 * @result -1 on error
 */

int32 MFDRM_FileTell (MFDRM_FILE *fp);

/*!
 * @function MFDRM_FileClose
 * @abstract File close function for MFDRM FILE IO
 * @discussion
 *   This function closes the file and releases any associated memory in fp
 *   that was allocated in MFDRM_FileOpen
 *
 * @param fp MFDRM_FILE handle to be closed
 *
 * @result 0 on success else EOF
 */

int32 MFDRM_FileClose (MFDRM_FILE *fp);

/*!
 * @function MFDRM_FileFlush
 * @abstract Dummy function
 * @discussion
 *   This function does nothing. Mimics a similar function in OSCL
 *
 * @param fp MFDRM_FILE handle
 *
 * @result 0
 */

int32 MFDRM_FileFlush (MFDRM_FILE *fp);

/*!
 * @function MFDRM_CheckEndOfFile
 * @abstract This function checks if end of file has been reached.
 *
 * @param fp MFDRM_FILE handle
 *
 * @result Non-zero if end of file was reached in the last operation
 */

int32 MFDRM_CheckEndOfFile (MFDRM_FILE *fp);

/*!
 * @function MFDRM_GetFileError
 * @abstract Checks if an error has occurred in the last file io operation.
 *
 * @param fp MFDRM_FILE handle
 *
 * @result Non-zero in case of error
 */

int32 MFDRM_GetFileError (MFDRM_FILE *fp);

#endif // _MFDRM_FILE_IO_H_

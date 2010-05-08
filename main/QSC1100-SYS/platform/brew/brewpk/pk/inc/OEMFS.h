/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMFS.h

SERVICES:  OEM File System Interface

GENERAL DESCRIPTION:
        Definitions, typedefs, etc. for OEM File System

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef OEM_FS_H
#define OEM_FS_H

#include "AEEFile.h"
#include "AEE_OEMFile.h"

#if defined(__cplusplus)
extern "C" {
#endif

//
// EFS Enumeration Structure
//


typedef struct
{
   char       attrib;
   uint32     dwCreationDate;
   uint32     dwSize;
   char       szName[1];

} OEMFSEnum;

// file access restrictions
typedef uint32 OEMFSRmtAccessMode;
#define FRAM_READ   0x01
#define FRAM_WRITE  0x02
#define FRAM_LIST   0x04
#define FRAM_DELETE 0x08

typedef boolean (*PFNCHKRMTACCESS)( char *pName, OEMFSRmtAccessMode mode );

/**************************************************************************/
/*               GENERIC FILESYSTEM EXPORTED OPERATIONS                   */
/**************************************************************************/

extern const AEEConstFile *** OEMFS_GetConstFileList(void);

extern int OEMFS_Open
(
  const char            *szFilename,           /* Name of file to open */
  AEEOpenFileMode        nMode,                /* Open mode */
  OEMINSTANCE           *pFileHandle
);

extern int OEMFS_Close
(
  OEMINSTANCE            pFileHandle
);

extern int32 OEMFS_Read
(
  OEMINSTANCE            pFileHandle,          /* Handle of file to read from */
  void                  *buffer,               /* Buffer for read bytes */
  uint32                nBytes,                /* Number of bytes to read */
  int                   *pnErr                 /* output error */
);

extern int32 OEMFS_WriteEx
(
  OEMINSTANCE            pFileHandle,          /* Handle of file to write to */
  const void            *buffer,               /* Buffer with bytes to write */
  uint32                nBytes,                /* Number of bytes to write */
  const char            *szFilename,           /* Name of the file */ 
  int                   *pnErr                 /* output error */
);

extern int32 OEMFS_Write
(
 OEMINSTANCE            pFileHandle,          /* Handle of file to write to */
 const void            *buffer,               /* Buffer with bytes to write */
 uint32                nBytes,                /* Number of bytes to write */
 int                   *pnErr                 /* output error */
 );

extern int OEMFS_GetAttributes
(
  const char            *szFilename,           /* File name */
  char                  *pattrib,
  uint32                *pdwCreationDate,
  uint32                *pdwSize
);

extern int OEMFS_GetOpenFileAttributes
(
  OEMINSTANCE            pFileHandle,           /* File handle */
  char                  *pattrib,
  uint32                *pdwCreationDate,
  uint32                *pdwSize
);

extern int32 OEMFS_SeekEx
(
  OEMINSTANCE           pFileHandle,          /* File handle */
  AEEFileSeekType       sType,                 /* Offset starting point */
  int32                 nPos,                  /* Offset */
  const char            *szFilename,           /* Name of the file */
  int                   *pnErr                 /* output error */
);

extern int32 OEMFS_Seek
(
 OEMINSTANCE           pFileHandle,          /* File handle */
 AEEFileSeekType       sType,                 /* Offset starting point */
 int32                 nPos,                  /* Offset */
 int                   *pnErr                 /* output error */
 );

extern int32 OEMFS_Tell
(
  OEMINSTANCE            pFileHandle,          /* File handle */
  int                   *pnErr                 /* output error */
);

extern int OEMFS_TruncateEx
(
  OEMINSTANCE           pFileHandle,          /* File handle */
  const char            *szFilename,           /* Name of the file */
  uint32                nPos                   /* New length of file */
);

extern int OEMFS_Truncate
(
 OEMINSTANCE           pFileHandle,          /* File handle */
 uint32                nPos                   /* New length of file */
);


extern int OEMFS_Remove
(
  const char            *szFilename            /* Name of file to remove */
);

extern int OEMFS_Rename
(
  const char            *szSource,             /* Current name of file */
  const char            *szDest                /* New name of file */
);

extern int OEMFS_Mkdir
(
  const char            *szDir                 /* Name of new directory */
);

extern int OEMFS_Rmdir
(
  const char            *szDir                 /* Directory to remove */
);

extern int OEMFS_Test
(
  const char            *szFilename            /* Filename to test */
);

extern int OEMFS_EnumStart /* returns OS-defined pointer to iterator struct */
(
  const char            *szDir,                /* Root directory */
  char                  isDir,                 /* File or subdir list */
  OEMINSTANCE          *ppcxt                  /* output, enum handle */
);

extern int OEMFS_EnumNext
(
   OEMINSTANCE pcxt,                   /* pointer from EnumStart */
   OEMFSEnum * pFSEnum,
   int         *pnFSEnumSize
);

extern int OEMFS_EnumStop
(
 OEMINSTANCE        pcxt                   /* pointer from EnumStart */
);

extern uint32 OEMFS_SpaceAvail
(
  void
);


extern int OEMFS_GetFreeSpaceEx
(
  const char *szPath,
  uint32 *pdwTotal,
  uint32 *pdwFree
);

extern uint32 OEMFS_SpaceUsed
(
  void
);

extern void OEMFS_RegRmtAccessChk
(
 const char        **pszDirList,
 uint32            sizeDirList,
 PFNCHKRMTACCESS   pfn
);

extern uint16 OEMFS_GetMaxPathLength
(
 void
);

extern AEE_EXPORTS int OEMFS_GetNativePath
(
   const char *cpszIn,
   char *pszOut,
   int *pnOutLen
);

extern int OEMFS_StatVFS
(
 const char *cpszPath,
 uint64 *qwTotal,
 uint64 *qwFree
);

#if defined(__cplusplus)
}
#endif

/*========================================================================
  INTERFACES DOCUMENTATION
=======================================================================
Interface Name: OEMFS
=======================================================================

Function: OEMFS_Open()

Description:
   Open the file specified by "szFilename" for the type of access specified
   by "nMode."

   This function allocates and initializes a data structure that it
   will return.  The current file position should be set to be the
   beginning of the file for all "nMode".

Prototype:
   int OEMFS_Open
   (
      const char            *szFilename,
      AEEOpenFileMode       nMode,
      OEMINSTANCE           *ppFileHandle
   );

Parameters:
   szFilename: name of file to open
   nMode: open mode
   ppFileHandle: [out] pointer to place to store new file handle

Return Value:
   SUCCESS on SUCCESS
   If there is an error, this function should return an error
    from AEEError.h

Comments:

   Common error codes are:

   EFILEEXISTS:  if called with _OFM_CREATE and the file exists

   EFILENOEXISTS: if called without _OFM_CREATE and the file does not exist

   EFSFULL: A file cannot be created because the FS is full.

   EBADPARM: "nMode" is not a valid mode

   EBADFILENAME: szFilename is NULL, empty, or longer than AEE_MAX_FILE_NAME

   Other error codes: any valid brew error code.  For example, the last error
   may be set to ENOMEMORY if memory for the OEM defined data structure
   cannot be allocated.

   Modes _OFM_READWRITE, _OFM_APPEND and _OFM_CREATE should open the file for
   read write.

   A file may be opened more than once.

   This function will not create sub directories.  See OEMFS_Mkdir().

Side Effects:
   None.

See Also:
   OEMFS_Close()
   OEMFS_GetLastError()
   OEMFS_Mkdir()

========================================================================

Function: OEMFS_Close()

Description:

   Close the file identified by "pFileHandle".  This function takes the
   file handle returned by OEMFS_Open() and frees all resources associated
   with it.  Subsequent operations on the file handle fail.

Prototype:
   int OEMFS_Close ( void *pFileHandle );

Parameters:
   pFileHandle: The handle value returned from OEMFS_Open()

Return Value:
   SUCCESS:  if the operation was successful.
   Valid BREW-defined error code: Otherwise
   Common error values:
     EMEMPTR: "pFileHandle" does not point to a valid handle

Comments:
   This function should set an internal error value that can be retrieved
   with OEMFS_GetLastError().

Side Effects:
   None.

See Also:
   OEMFS_Open()

===========================================================================

Function: OEMFS_Read()

Description:
   Read "nBytes" bytes from the file identified by "pFileHandle" into
   "buffer" starting at the current file pointer position associated
   with "pFileHandle." The number of bytes actually read is returned.
   This may be fewer bytes than requested if, for example, fewer bytes
   remain in the file before the End Of File (EOF).
   If an error occurs with the operation, 0 is returned.

Prototype:
   uint32 OEMFS_Read
   (
      void                  *pFileHandle,
      void                  *buffer,
      uint32                nBytes,
      int                  *pnErr
   );

Parameters:
   pFileHandle: handle of file returned from OEMFS_Open
   buffer: buffer for read bytes
   nBytes: number of bytes to read
   pnErr: [out] pointer to error code

Return Value:
   The function returns the number of bytes read into "buffer" or
   the value -1 if there is an error (including EOF)

Comments:
   This function should set pnErr.  Common errors:
     EFILEEOF: End of file (see comment below)
     EMEMPTR: file pointer or buffer is NULL
     Other valid BREW error code.

   If there are fewer bytes between the current file pointer and EOF,
   OEMFS_Read() returns bytes remaining until the EOF.  Future calls to
   OEMFS_Read() will return -1, and set pnErr to EFILEEOF until the file
   pointer has been changed using OEMFS_Seek().

Side Effects:
   If no errors were encountered, the current file pointer position should be
   incremented so that it points immediately after the last byte returned.

See Also:
   OEMFS_Seek()

==========================================================================

Function: OEMFS_Write()

Description:
   Write "nBytes" bytes from "buffer" into the file identified by
   "pFileHandle" starting at the current file pointer position associated
   with "pFileHandle."  The number of bytes actually written is returned
   following operation completion.

Prototype:
   uint32 OEMFS_Write
   (
      void                  *pFileHandle,
      const void            *buffer,
      uint32                nBytes,
      int                   *pnErr
   );

Parameters:
   pFileHandle: handle of file to write to, returned by OEMFS_Open
   buffer: buffer with bytes to write
   nBytes: number of bytes to write
   pnErr: [out] pointer to error code

Return Value:
   The function returns the number of bytes written from "buffer" or
   the value -1 if there is an error.

Comments:
   This function should set pnErr.  Common errors:
     EMEMPTR: If the file pointer or buffer is NULL
     EFSFULL: The file system is full
     Other valid BREW defined error code.

Side Effects:
   If no error is encountered, the current file pointer position should be
   incremented so that it points immediately after the last byte returned.

   If the file pointer is not at the end of the file, up to "nBytes" bytes will
   be overwritten with the data in "buffer".  Ie, data is not inserted into the
   file.

See Also:
   OEMFS_Seek()

========================================================================
Function: OEMFS_WriteEx()

Description:
   Write "nBytes" bytes from "buffer" into the file identified by
   "pFileHandle" starting at the current file pointer position associated
   with "pFileHandle."  The number of bytes actually written is returned
   following operation completion.

Prototype:
   uint32 OEMFS_WriteEx
   (
      void                  *pFileHandle,
      const void            *buffer,
      uint32                nBytes,
      const char            *szFilename,
      int                   *pnErr
   );

Parameters:
   pFileHandle: handle of file to write to, returned by OEMFS_Open
   buffer: buffer with bytes to write
   nBytes: number of bytes to write
   szFileName: pointer to buffer of filename for file with open handle
               in pFileHandle
   pnErr: [out] pointer to error code

Return Value:
   The function returns the number of bytes written from "buffer" or
   the value -1 if there is an error.

Comments:
   This function should set pnErr.  Common errors:
     EMEMPTR: If the file pointer or buffer is NULL
     EFSFULL: The file system is full
     Other valid BREW defined error code.

Side Effects:
   If no error is encountered, the current file pointer position should be
   incremented so that it points immediately after the last byte returned.

   If the file pointer is not at the end of the file, up to "nBytes" bytes will
   be overwritten with the data in "buffer".  Ie, data is not inserted into the
   file.

See Also:
   OEMFS_SeekEx()
   OEMFS_Write()

========================================================================

Function: OEMFS_GetAttributes()

Description:
   Return the file attributes of the file specified by "szFilename."
   The attributes matched are exactly the same as those currently
   returned to the programmer via the upper level API.

   The attributes are:
      - Flags (None, Hidden, System, Directory)
      - Creation date in seconds from the beginning of the CDMA epoch
      - File size
      - File name

Prototype:
   int OEMFS_GetAttributes
   (
      const char            *szFilename,
      char     *pattrib,
      uint32   *pdwCreationDate,
      uint32   *pdwSize
   );

Parameters:
   szFilename: filename
   pattrib: [out] file attributes, may be NULL
   pdwCreationDate: [out] file creation date, seconds since
                      Jan 6, 1980 GMT, may be NULL
   pdwSize: [out] file size

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.
   Common error codes:
     EFILENOEXISTS: File does not exist

Comments:
   pattrib, pdwCreationDate and pdwSize may be NULL, indicating
   the caller is not interested in this info

Side Effects:
   None

See Also:
   OEMFS_GetOpenFileAttributes()

==========================================================================

Function: OEMFS_GetOpenFileAttributes()

Description:
   Return the file attributes of the currently open file specified by
   "pFileHandle"

   The attributes are:
      - Flags (Hidden, System, Directory)
      - Creation date in seconds from the beginning of the CDMA epoch
      - File size
      - File name

Prototype:
   int OEMFS_GetOpenFileAttributes
   (
      void*    pFileHandle,
      char     *pattrib,
      uint32   *pdwCreationDate,
      uint32   *pdwSize
   );

Parameters:
   pFileHandle: file handle returned by OEMFS_Open()
   pattrib: [out] file attributes, may be NULL
   pdwCreationDate: [out] file creation date, seconds since
                      Jan 6, 1980 GMT, may be NULL
   pdwSize: [out] file size

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.

Comments:
   pattrib, pdwCreationDate and pdwSize may be NULL, indicating
   the caller is not interested in this info

Side Effects:
   None

See Also:
   OEMFS_GetFileAttributes()

========================================================================

Function: OEMFS_Seek()

Description:
   Set the current file pointer position of the file associated with
   "pFileHandle" from a starting point "sType" offset by a number of
   bytes (positive or negative) specified by "nPos".

   sType can be one of three values: _SEEK_START (offset is based on the
   beginning of the file), _SEEK_END (offset is based on the end of the
   file, or _SEEK_CURRENT (offset from the current position).

Prototype:
   int32  OEMFS_Seek
   (
      void                  *pFileHandle,
      AEEFileSeekType       sType,
      int32                 nPos,
      int                   *pnErr
   );

Parameters:
   pFileHandle: file handle returned by OEMFS_Open()
   sType: type of seek starting point
   nPos: offset from starting point
   pnErr: [out] pointer to error code

Return Value:
   Returns the new fp position if the operation was successful.
   Otherwise returns -1 and sets pnErr.

Comments:
   This function must set pnErr.  Common error codes:
     EBADSEEKPOS: if the seek operation would place the file pointer before
                  the beginning of the file
     EFSFULL: If growing the file would consume more space than available
     EMEMPTR: If "pFileHandle" is a bad memory pointer
     EBADPARM: If sType is not a valid seek type

Side Effects:
   If the new seek pointer is beyond the end of the file, the file will
   be enlarged to accommodate the seek and need not be initialized to any
   particular value.

See Also:
   OEMFS_Tell()

========================================================================

Function: OEMFS_SeekEx()

Description:
   Set the current file pointer position of the file associated with
   "pFileHandle" from a starting point "sType" offset by a number of
   bytes (positive or negative) specified by "nPos".

   sType can be one of three values: _SEEK_START (offset is based on the
   beginning of the file), _SEEK_END (offset is based on the end of the
   file, or _SEEK_CURRENT (offset from the current position).

Prototype:
   int32  OEMFS_SeekEx
   (
      void                  *pFileHandle,
      AEEFileSeekType       sType,
      int32                 nPos,
      const char            *szFilename,
      int                   *pnErr
   );

Parameters:
   pFileHandle: file handle returned by OEMFS_Open()
   sType: type of seek starting point
   nPos: offset from starting point
   szFileName: pointer to buffer of filename for file with open handle
               in pFileHandle
   pnErr: [out] pointer to error code

Return Value:
   Returns the new fp position if the operation was successful.
   Otherwise returns -1 and sets pnErr.

Comments:
   This function must set pnErr.  Common error codes:
     EBADSEEKPOS: if the seek operation would place the file pointer before
                  the beginning of the file
     EFSFULL: If growing the file would consume more space than available
     EMEMPTR: If "pFileHandle" is a bad memory pointer
     EBADPARM: If sType is not a valid seek type

Side Effects:
   If the new seek pointer is beyond the end of the file, the file will
   be enlarged to accommodate the seek and need not be initialized to any
   particular value.

See Also:
   OEMFS_Tell()

========================================================================
Function: OEMFS_Tell()

Description:
   Return the current file pointer position of the file associated with
   "pFileHandle."

Prototype:
   int32 OEMFS_Tell
   (
      void  *pFileHandle
      int   *pnErr
   );

Parameters:
   pFileHandle: file handle returned from OEMFS_Open()
   pnErr: [out] pointer to error

Return Value:
   Returns the offset from the beginning of the file if successful.  If
   an error occurs with the operation, -1 is returned and pnErr is set

Comments:
   pnErr should be set to a BREW error code

Side Effects:
   None

See Also:
   OEMFS_Seek()

===========================================================================

Function: OEMFS_Truncate()

Description:
   Truncate the file identified by "pFileHandle" to the position specified by
   "nPos".  The file must not be open for read-only operations.  The
   offset must be less than the total length of the file.

Prototype:
   int OEMFS_Truncate
   (
      void                  *pFileHandle,
      uint32                nPos,
   );

Parameters:
   pFileHandle: file handle
   nPos: file truncate position (new file size)

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.

Comments:
   Common error codes:
     EBADPARM: if nPos is beyond the End Of File (EOF)

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMFS_TruncateEx()

Description:
   Truncate the file identified by "pFileHandle" to the position specified by
   "nPos".  The file must not be open for read-only operations.  The
   offset must be less than the total length of the file.

Prototype:
   int OEMFS_TruncateEx
   (
      void                  *pFileHandle,
      const char            *szFilename,
      uint32                nPos,
   );

Parameters:
   pFileHandle: file handle
   szFileName: pointer to buffer of filename for file with open handle
               in pFileHandle
   nPos: file truncate position (new file size)

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.

Comments:
   Common error codes:
     EBADPARM: if nPos is beyond the End Of File (EOF)

Side Effects:
   None

See Also:
   None

===========================================================================
Function: OEMFS_Remove()

Description:
   Remove the file identified by "szFilename".

Prototype:
   int OEMFS_Remove
   (
      const char            *szFilename
   );

Parameters:
   szFilename: name of file to remove

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.

Comments:
   Common error codes:
     EFILEOPEN: File is open
     EFILENOEXISTS: The file does not exist
     EBADFILENAME: if "szFilename" is NULL, empty or longer than AEE_MAX_FILE_NAME

   OEMFS_Remove shall ensure that the file "szFilename" is not currently open.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEMFS_Rename()

Description:
   Rename the file identified by "old_filename" to "new_filename".

Prototype:
   int OEMFS_Rename
   (
      const char            *szSource,
      const char            *szDest
   );

Parameters:
   szSource: current name of the file
   szDest: new name for the file

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.

Comments:
   Common error codes:
     EBADFILENAME: If either filename is NULL, empty or longer than AEE_MAX_FILE_NAME
     EFILEOPEN: File is open

   An opened file may not be renamed or moved.

Side Effects:
   If the FS supports renaming an open file, future operations on the file
   should effect the new filename.

See Also:
   None

==========================================================================

Function: OEMFS_Mkdir()

Description:
   Creates a new user file directory named "szDir".  This directory will
   be created as long as the new directory's parent directory exists.

Prototype:
   int OEMFS_Mkdir
   (
      const char            *szDir
   );

Parameters:
   szDir: name of new directory

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.

Comments:
   Common error codes:
     EFILENOEXISTS: If the parent directory does not exist
     EFILEEXISTS: If a file or directory of that name already exists
     EBADFILENAME: If "szDir" is NULL, empty or longer than AEE_MAX_FILE_NAME
     EFSFULL: If the FS is full

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEMFS_Rmdir()

Description:
   Removes the user file directory specified by "szDir".  "szDir" must be empty.

Prototype:
   int OEMFS_Rmdir
   (
      const char            *szDir
   );

Parameters:
   szDir: directory to remove

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.

Comments:
   Common error codes:
     EBADFILENAME: "szDir" is NULL, empty or longer than AEE_MAX_FILE_NAME
     EFILENOEXISTS: If "szDir" does not exist
     EDIRNOTEMPTY: If "szDir" is not empty

Side Effects:
   None

See Also:
   OEMFS_Remove()

==========================================================================

Function: OEMFS_Test()

Description:
   It tests for the existence of a file or a directory: first checks if there
   is a file with the specified name. If not, it then checks if there is a
   directory with the specified name.

Prototype:
   int OEMFS_Test
   (
      const char            *szFilename
   );

Parameters:
   szFilename: File or directory to check existence of

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.

Comments:
   Common error codes:
     EFILENOEXISTS: If the file or directory does not exist

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEMFS_EnumStart()

Description:
   Initialize a file or directory enumeration control object for use by
   subsequent calls to OEMFS_EnumNext.  Enumeration can be either of
   files or directories in the parent directory "szDir".  To enumerate
   files, the "isDir" parameter should be zero.  To enumerate directories,
   the value will be non-zero.

Prototype:
   int OEMFS_EnumStart
   (
      const char            *szDir,
      char                  isDir
      OEMINSTANCE          *pcxt
   );

Parameters:
   szDir: Directory to be enumerated
   isDir: Flag to indicate enumeration of files or directories
   pcxt: [out] pointer to new enumeration context

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.

Comments:
   Common error codes:
     EFILENOEXISTS: If an element of the directory path does not exist
     EBADFILENAME: If "szDir" is NULL, empty or longer than the OEM's
                    max file name

Side Effects:
   Allocates memory to hold structure

See Also:
   OEMFS_EnumNext()
   OEMFS_EnumEnd()

==========================================================================

Function: OEMFS_EnumNext()

Description:
   Using the file or directory enumeration control object referenced
   by "pIterator", return file information for the next file or
   directory within directory specified in OEMFS_EnumStart.

   Upon success, the OEMFSEnum object will contain the values associated
   with the enumerated file or directory.

Prototype:
   int OEMFS_EnumNext
   (
      OEMINSTANCE  pcxt,
      OEMFSEnum   *pFSEnum,
      int         *pnFSEnumSize
   );

Parameters:
   pcxt: enumeration handle, returned from OEMFS_EnumStart
   pFSEnum: [out] pointer to structure to receive enumeration result, may be NULL
   pnFSEnumSize: [in/out] on input, taken as size, in bytes of pFSEnum
                  on output: if pFSEnum is non-null, filled with number of
                              bytes written to pFSEnum
                             if pFSEnum is null, filled with number of
                              bytes needed to hold the next entry

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.  
   Common error codes:
     EFILEEOF: There are no items left to be enumerated.
     EMEMPTR: if "pIterator" or "pInfo" is invalid.

Comments:
   This function should set an internal error value that can be retrieved
   with OEMFS_GetLastError().

Side Effects:
   None

See Also:
   OEMFS_EnumStart()
   OEMFS_EnumStop()

==========================================================================

Function: OEMFS_EnumStop()

Description:
   Free up any resources associated with an enumeration.

Prototype:
   int OEMFS_EnumStop
   (
      OEMFSEnum   *pcxt
   );

Parameters:
   pcxt: enumeration control object returned from OEMFS_EnumStart

Return Value:
   Returns SUCCESS if the operation was successful.  Otherwise returns
   a valid BREW-defined error code.
   Common error codes:
     EMEMPTR: if "pIterator" is not valid

Comments:
   This function should set an internal error value that can be retrieved
   with OEMFS_GetLastError().

Side Effects:
   None

See Also:
   OEMFS_EnumStart()
   OEMFS_EnumNext()

==========================================================================

Function: OEMFS_SpaceAvail()

Description:
   Returns the amount of available file system space (units are bytes).

Prototype:
   uint32  OEMFS_SpaceAvail
   (
      void
   );

Parameters:
   None

Return Value:
   Number of bytes of file system space currently available.

Comments:
   This function does not set or return an error.

Side Effects:
   None

See Also:
   OEMFS_SpaceUsed()

==========================================================================

Function: OEMFS_SpaceUsed()

Description:
   Returns the amount of "in-use" file system space (units are bytes).

Prototype:
   dword  OEMFS_SpaceUsed
   (
      void
   );

Parameters:
   None

Return Value:
   Number of bytes of file system space currently in-use.

Comments:
   This function does not set or return an error.

Side Effects:
   None

See Also:
   OEMFS_SpaceAvail()

==========================================================================

Function: OEMFS_GetFreeSpaceEx()

Description:
   This function gets the number of free bytes currently available on
   file system. If the passed parameters pdwTotal and pdwFree are non-NULL, 
   this function initializes pdwTotal with the total room in the file system
   and pdwFree with the free space in the file system. Parameter szPath
   indicates the file system (main flash, peripheral cards, etc) for which
   the file space details are required. If the szPath points to a peripheral
   card (say MMC), (using fs:/card0/), the returned parameters are related
   to the file system on that card.


Prototype:
   int OEMFS_GetFreeSpaceEx
   (
      const char *szPath,
      uint32 *pdwTotal,
      uint32 *pdwFree
   );

Parameters:
    szPath : Pointer a path string that specifies the file system 
             whose space details are required. 
    pdwTotal : Placeholder for the total room in the file system.
    pdwFree : Placeholder for the available space in the file system.

Return Value:
    SUCCESS : if successful
    EBADFILENAME : if passed name does not point to a valid file system
    EUNSUPPORTED : if peripheral card (say MMC) is not present or on
                   the Simulator.

Comments:
   Free space is measured in bytes.

Side Effects:
   None

See Also:
   OEMFS_SpaceAvail()

==========================================================================

Function: OEMFS_RegRmtAccessChk()

Description:
   This function is called by the AEE layer during BREW initialization
   to setup the remote file access restrictions.

   This function has two responsibilities:
   - Registers the directory paths to monitor for remote file access
     requests.
   - Saves the AEE layer callback that should be called when a
     remote file access request is made for a file in one of the
     directories being monitored

Prototype:
   extern void OEMFS_RegRmtAccessChk
   (
   const char        **pszDirList,
   uint32            sizeDirList,
   PFNCHKRMTACCESS   pfn
   )

Parameters:
   pszDirList:    Pointer to a list of NULL terminated strings.  Each
                  string contains a complete path name to monitor
                  for remote file access requests
   nListElements: Number of elements in pszDirList (number of paths to
                  monitor.
   pfn:           AEE Callback that must be called by the
                  OEM layer when a remote file access request is made.
                  This callback will return either true or false,
                  indicating whether to allow or deny the remote file
                  access requests.

Return Value:
   None

Comments:
   None


Side Effects:
   None

See Also:
   None
==========================================================================

Function: OEMFS_GetMaxPathLength()

Description:
   This function is called by the AEE layer to determine the smallest
     buffer required to represent the longest BREW file namespace path,
     INCLUDING NULL TERMINATION.  This information is provided as a
     hint to applications allocating memory to receive enumeration
     results.

   For example, the BREW file namespace path "fs:/shared" may map to the
     native path "/pub".  In the case where the native filesystem's
     maximum path length is 256, OEMFS_GetMaxPathLength() should
     return:

        256 - strlen("/pub") + strlen("fs:/shared") = 262

  OEMFS implementors should arrive at the value above by examining all
    possible maps from the BREW file namespace to the native file
    namespace, and return the largest value.

Prototype:
   extern uint16 OEMFS_GetMaxPathLength
   (
   void
   )

Parameters:
   None

Return Value:
   the smallest buffer that can accept any BREW path in the system

Comments:
   This is a user-mode function.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEMFS_GetNativePath()

Description:
   This function is called by the AEE layer to map a BREW pathname to
    a "native" pathname, one that will be acceptable to be passed directly
    to the underlying OEM's filesystem's API.

   BREW will not generally call this function, rather, it is provided
     for those subsystems that accept filenames directly from BREW
     that access the OEM's filesystem APIs directly on behalf of
     a BREW application.

Prototype:
   extern int OEMFS_GetNativePath
   (
   const char        *cpszIn,
   char              *pszOut,
   int               *pnOutLen
   )

Parameters:
   cpszIn:  a pathname in the canonical BREW file namespace, it will begin
               with "fs:/"

   pszOut:  [out] output buffer for this function to write into
   pnOutLen: [in/out] if pszOut is non-null, on input this is the length of
                   pszOut, and on output, is set to the number of bytes
                   written to pszOut (including null termination)
               if pszOut is null, pnOutLen is ignored on input, and on
                   output is set to the number of bytes required to map the
                   path.

Return Value:
   SUCCESS if the name could be mapped and was mapped successfully
   EBADFILENAME if the name could not be mapped
   EBUFFERTOOSMALL if pszOut is non-null and *pnOutLen is not big enough
              to hold the path + null-termination

Comments:
   pszOut is always null terminated on output.

   This is a user-mode function.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEMFS_StatVFS()

Description:
   This function gets the number of free bytes currently available on
   file system. If the passed parameters pdwTotal and pdwFree are non-NULL, 
   this function initializes qdwTotal with the total room in the file system
   and qdwFree with the free space in the file system. Parameter cpszPath
   indicates the file system (main flash, peripheral cards, etc) for which
   the file space details are required. If the szPath points to a peripheral
   card (say MMC), (using fs:/card0/), the returned parameters are related
   to the file system on that card.

   This function works with filesystems greater than 4GB in size as it 
   uses uint64 function arguments instead of uint32.


Prototype:
   int OEMFS_StatVFS
   (
      const char *cpszPath,
      uint64 *qwTotal,
      uint64 *qwFree
   );

Parameters:
    cpszPath : Pointer a path string that specifies the file system 
               whose space details are required. 
    qdwTotal : Placeholder for the total room in the file system.
    qdwFree  : Placeholder for the available space in the file system.

Return Value:
    SUCCESS : if successful
    EBADFILENAME : if passed name does not point to a valid file system
    
Comments:
   Free space is measured in bytes.

Side Effects:
   None

See Also:
   OEMFS_GetFreeSpaceEx()
==============================================================

AEEFS

See the IFile Interface and the IFileMgr Interface in the BREW API Reference.


========================================================================*/

#endif /* OEM_FS_H */


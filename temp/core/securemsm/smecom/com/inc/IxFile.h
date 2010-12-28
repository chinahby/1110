#ifndef IXFILE_H
#define IXFILE_H
/*============================================================================
IxFile

ABSTRACT
   The IxFile interface definitions.

DESCRIPTION
   This service will provide basic file operations to EFS.


Copyright (c) 2005 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxFile.h#3 $

=========================================================================== */
#ifdef __cplusplus
  #include "IxBase.h"
#endif
#include "rtedef.h"
#include "comdef.h"
#include "IxErrno.h"
#include "wstring.h"

#include "IxFileSystem.h"

/*
** SFS drives
*/
#define SYS_SFS_DRIVE                  "sys_sfs:"
#define USR_SFS_DRIVE                  "usr_sfs:"

typedef struct
{
   int               nStructSize;
   char              attrib;
   uint32            dwCreationDate;
   uint32            dwSize;
   char  *           pszFile;
   int               nMaxFile;   
   wchar *           pszDescription;
   int               nDescriptionSize;
   uint32 *          pClasses;
   int               nClassesSize;
   int               fileType;
} IxFileInfoEx;

typedef struct
{
   char        attrib;
   uint32      dwCreationDate;
   uint32      dwSize;
   char        szName[IXFILE_NAME_MAX];
} IxFileInfo;

typedef struct
{
   uint16            wMaxFiles;     //Maximum number if Files in EFS this Module is allowed to create
   uint16            wFilesUsed;    //Number of Files currenty used by this Module
   uint32            dwMaxSpace;    //Maximum EFS Space this module is allowed to consume
   uint32            dwSpaceUsed;   //Total space currently used by this module so far
} IxFileUseInfo;

typedef enum {
   IxFile_SEEK_START,
   IxFile_SEEK_END,
   IxFile_SEEK_CURRENT
} IxFileSeekType;


/*
** Types used in directory enumeration.
*/
typedef unsigned int IxFileDirHandleT;

#ifdef __cplusplus

class IxFile : public IxBase {

    friend class IxFileMgr;

public:

   virtual unsigned long AddRef( void ) = 0;
   virtual unsigned long Release( void ) = 0;
   /* Gets the data representing the object */
   virtual void GetState( uint8** stateDataPtr, uint32* stateDataLenPtr ) = 0;

   /* Sets the data for the object */
   virtual void SetState( uint8* stateDataPtr, uint32 stateDataLen ) = 0;

//   virtual void        Readable( PFNNOTIFY pfn, void * pUser);

   /***********************************************************************
   * Read
   *
   * Attempt to read 'nWant' of data from the file associated with this
   * object.  Returns the number of bytes successfully read, or -1 for an error.
   * Zero is a valid result, and generally indicates that the end of file 
   * has been reached.
   *
   * It is permitted for Read to return fewer bytes than were requested,
   * even if the data is available in the file.
   **********************************************************************/
   virtual int32       Read( void * pDest, uint32 nWant) = 0;

//   virtual void        Cancel( PFNNOTIFY pfn, void * pUser);

  /***********************************************************************
   * Write
   *
   * Attempt to write 'dwCount' of data to the file associated with this
   * object. Returns the number of bytes successfully  written or -1 for 
   * an error.  Zero generally indicates that the filesystem is full.
   *
   * It is permitted for Write to write fewer bytes than were requested,
   * even if space is available.
   **********************************************************************/
   virtual uint32 Write( PACKED const void * pBuffer, uint32 dwCount ) = 0;


  /***********************************************************************
   * GetInfo
   *
   * Gets the file creation date, file size, filename, and file attributes
   * of the file pointed to by this object.
   *
   * This information is returned in an IxFileInfo struct.
   **********************************************************************/
   virtual IxErrnoType GetInfo( IxFileInfo * pInfo ) = 0;


  /***********************************************************************
   * Seek
   * 
   * Moves the file pointer associated with this object by a given distance
   * from a specified origin.
   *
   * The specified origin (seek) can take three values:
   *    IxFile_SEEK_START
   *    IxFile_SEEK_END,
   *    IxFile_SEEK_CURRENT
   *
   * The move distance can be specified in positive or negative directions (values).
   *  For example, negative direction is used when seeking from the end of the file
   * (_SEEK_END). If the seek type is _SEEK_CURRENT and move distance is 0 (zero),
   *  this acts as a tell operation and returns the current position of the file pointer.
   *  Otherwise, the file pointer is moved by the given distance from the specified
   *  origin. If the file was opened in the _OFM_READ mode, this operation succeeds
   *  if the move distance from the specified origin is within the boundaries of the
   *  file.If the file was opened in the _OFM_READWRITE mode and the move distance points
   *  to the location before the beginning of the file, this operation fails. If the move
   *  distance from the specified origin is past the end of the file, the file size
   *  is extended to the point of the move distance (from the seek origin).
   **********************************************************************/
   virtual IxErrnoType Seek( IxFileSeekType seek, int32 position ) = 0; 


  /***********************************************************************
   * Truncate 
   *
   * Truncates the file associated with this object to a position specified by
   * truncate_pos parameter. 
   *
   * To truncate a file, the file needs to be open for writing.
   **********************************************************************/
   virtual IxErrnoType Truncate( uint32 truncate_pos ) = 0;


  /***********************************************************************
   * GetInfoEx
   *
   * Gets the extended information of the file associated with this object.
   * The extended file information is contained in IxFileInfoEx.
   **********************************************************************/
   virtual IxErrnoType GetInfoEx( IxFileInfoEx * pi ) = 0;


  /***********************************************************************
   * SetCacheSize
   *
   * UnSupported
   **********************************************************************/
   virtual IxErrnoType SetCacheSize( int nSize ) = 0;


  /***********************************************************************
   * SetCacheSize
   *
   * UnSupported
   **********************************************************************/
   virtual void * Map( void * pStart, uint32 dwSize, int protections,
      int flags, uint32 dwOffset ) = 0;


  /***********************************************************************
   * Tell
   *
   * Retruns the current position of the file associated with this object.
   **********************************************************************/
   virtual int32 Tell( ) = 0;

   virtual ~IxFile() {}

};

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif

   void          ixfile_delete( void *pFile );

   unsigned long ixfile_add_ref( void *pFile );

   unsigned long ixfile_release( void *pFile );

   int32         ixfile_read( void * pFile, void * pDest, uint32 nWant );

   uint32        ixfile_write( void * pFile, PACKED const void * pBuffer,
      uint32 dwCount );

   IxErrnoType   ixfile_seek( void * pFile, IxFileSeekType seek,
      int32 position );

   IxErrnoType   ixfile_truncate( void * pFile, uint32 truncate_pos );

   IxErrnoType   ixfile_get_info_ex( void * pFile, IxFileInfoEx * pi );

#ifdef __cplusplus
}
#endif

#endif

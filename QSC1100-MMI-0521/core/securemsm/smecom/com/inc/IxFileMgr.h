#ifndef IXFILEMGR_H
#define IXFILEMGR_H
/*============================================================================
IxFileMgr

ABSTRACT
   The IxFileMgr interface definitions.

DESCRIPTION
   This service will provide basic file operations to EFS.


Copyright (c) 2006 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxFileMgr.h#3 $

=========================================================================== */
#include "customer.h"
#include "IxFileSystem.h"

#if defined(DUAL_PD) && defined(SEC_PD)

   #include "IxFileMgrClient.h"

#else

   #ifdef __cplusplus
      #include "IxBase.h"
   #endif

   #include "rtedef.h"
   #include "comdef.h"
   #include "IxErrno.h"
   #include "IxFileServer.h"

   #ifdef __cplusplus

   class IxFileMgr : public IxBase {
   public:

      IxFileMgr( );
      IxFileMgr( const char *pFSName );

      virtual ~IxFileMgr( );

      virtual unsigned long AddRef( void );
      virtual unsigned long Release( void );

      /* Gets the data representing the object */
      virtual void GetState( uint8** stateDataPtr, uint32* stateDataLenPtr )
      { (void) stateDataPtr; (void) stateDataLenPtr; };

      /* Sets the data for the object */
      virtual void SetState( uint8* stateDataPtr, uint32 stateDataLen )
      { (void) stateDataPtr; (void) stateDataLen; };

      /**********************************************************************
      * OpenFile
      *
      * Open a file in a specified mode.  A  pointer to an IxFile object
      * that is associated with the opened file is returned.  NULL otherwise.
      **********************************************************************/
      virtual IxFile *OpenFile( const char *pszFile,
         IxFileOpenFileMode mode );


      /**********************************************************************
      * GetInfo
      * 
      * Gets the file creation date, file size, filename, and file attributes
      * of the file.
      **********************************************************************/
      virtual IxErrnoType GetInfo( const char *pszName, IxFileInfo * pInfo );


      /**********************************************************************
      * Remove
      *
      * Removes the file identified by the given filename.
      **********************************************************************/
      virtual IxErrnoType Remove( const char *pszName );


      /**********************************************************************
      * MkDir
      *
      * Creates a directory, specified by a directory name.
      **********************************************************************/
      virtual IxErrnoType MkDir( const char *pszDir );

      /**********************************************************************
      * RmDir
      *
      * Removes a directory identified by a given directory name.
      **********************************************************************/
      virtual IxErrnoType RmDir( const char *pszDir );


      /**********************************************************************
      * Test
      * 
      * Tests if the specified file/directory exists.
      **********************************************************************/
      virtual IxErrnoType Test( const char *pszName );

      
      /**********************************************************************
      * GetLastError
      * 
      * To be called when a file operation has failed and the caller needs to
      * know about the reason for failure.
      **********************************************************************/
      virtual IxErrnoType GetLastError( void );


      /**********************************************************************
      * Rename
      *
      * Renames the file from the source name to the destination name.
      **********************************************************************/
      virtual IxErrnoType Rename( const char * pszSrc, const char * pszDest );


      /**********************************************************************
      * GetInfoEx
      *
      * This function gets the extended information of a file identified by
      * a file name.  The extended file information is contained in the
      * IxFileInfoEx structure.
      **********************************************************************/
      virtual IxErrnoType GetInfoEx( const char * pszName,
         IxFileInfoEx * pi );


      /**********************************************************************
      * OpenDir
      *
      * Opens a directory for iteration. Returns E_SUCCESS on success and
      * an error code on failure.
      **********************************************************************/
      virtual IxErrnoType OpenDir( const char * pszName,
         IxFileDirHandleT * pDirHandle );


      /**********************************************************************
      * ReadDir
      *
      * Reads the next entry from an open directory. Returns E_SUCCESS
      * on success and an error code on failure.
      **********************************************************************/
      virtual IxErrnoType ReadDir( IxFileDirHandleT dirHandle,
         IxFileDirEntT *pDirEnt );


      /**********************************************************************
      * CloseDir
      *
      * Closes an open directory. Returns E_SUCCESS on success and an
      * error code on failure.
      **********************************************************************/
      virtual IxErrnoType CloseDir( IxFileDirHandleT dirHandle );


      /**********************************************************************
      * GetFreeSpace
      *
      * Unsupported
      **********************************************************************/
      virtual uint32 GetFreeSpace( uint32 * pdwTotal );


      /**********************************************************************
      * SetDescription
      *
      * Unsupported
      **********************************************************************/
      virtual IxErrnoType SetDescription( const char * pszName,
         wchar * pszDesc );


      /**********************************************************************
      * Use
      *
      * Unsupported
      **********************************************************************/
      virtual IxErrnoType Use( const char * pszName,boolean bUse );


      /**********************************************************************
      * GetFileUseInfo
      *
      * Unsupported
      **********************************************************************/
      virtual IxErrnoType GetFileUseInfo( IxFileUseInfo *pfu );


      /**********************************************************************
      * ResolvePath
      *
      * Unsupported
      **********************************************************************/
      virtual IxErrnoType ResolvePath( const char *cpszIn, char *pszOut,
         int *pnOutLen );


      /**********************************************************************
      * CheckPathAccess
      *
      * Unsupported
      **********************************************************************/
      virtual IxErrnoType CheckPathAccess( const char *cpszIn,
         uint32 dwDesiredRights, uint32 *pdwActualRights );


      /**********************************************************************
      * GetFreeSpaceEx
      *
      * Unsupported
      **********************************************************************/
      virtual IxErrnoType GetFreeSpaceEx( const char *cpszPath,
         uint32 * pdwTotal, uint32 *pdwFree );

   private:
      uint32 m_nRefs;

      virtual IxFileServer *OpenSFSFile( const char *pszFile,
         IxFileOpenFileMode mode );

      IxFileSystem *m_fs;

   };

#endif /* DUAL_PD && SEC_PD */

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif

   void *      ixfilemgr_new(void);

   void        ixfilemgr_delete( void *pFileMgr );

   unsigned long ixfilemgr_add_ref( void *pFileMgr );

   unsigned long ixfilemgr_release( void *pFileMgr );

   void *      ixfilemgr_open_file( void *pFileMgr, const char *pszFile,
      IxFileOpenFileMode mode );

   IxErrnoType ixfilemgr_get_info( void *pFileMgr, const char *pszName,
      IxFileInfo * pInfo );

   IxErrnoType ixfilemgr_remove( void *pFileMgr, const char *pszName );

   IxErrnoType ixfilemgr_mk_dir( void *pFileMgr, const char *pszDir );

   IxErrnoType ixfilemgr_rm_dir( void *pFileMgr, const char *pszDir );

   IxErrnoType ixfilemgr_test( void *pFileMgr, const char *pszName );

   IxErrnoType ixfilemgr_get_last_error( void *pFileMgr );

   IxErrnoType ixfilemgr_get_info_ex( void *pFileMgr, const char * pszName,
      IxFileInfoEx * pi );

#ifdef __cplusplus
}
#endif

#endif /* IXFILEMGR_H */

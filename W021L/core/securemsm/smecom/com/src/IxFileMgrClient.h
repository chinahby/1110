#ifndef IXFILEMGRCLIENT_H
#define IXFILEMGRCLIENT_H
/*============================================================================
IxFileMgrClient

ABSTRACT
   The IxFileMgrClient interface definitions.

DESCRIPTION
   Provides basic file operations to EFS.


Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/IxFileMgrClient.h#3 $

=========================================================================== */
#include "comdef.h"
#include "IxErrno.h"
#include "IxFile.h"
#include "IxFileClient.h"
#ifdef FEATURE_SEC_SFS
   #include "IxSFSFile.h"
#endif

#define IxFileMgr IxFileMgrClient

#ifdef __cplusplus

class IxFileMgrClient : public IxBase {
public:

   IxFileMgrClient( );

   IxFileMgrClient( const char *pFSName );

   virtual ~IxFileMgrClient( );

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
   * Open a file in a specified mode.  A  pointer to an IxFile object that
   * is associated with the opened file is returned.  NULL otherwise.
   **********************************************************************/
   virtual IxFile *OpenFile( const char *pszFile, IxFileOpenFileMode mode );


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
   virtual IxErrnoType GetInfoEx( const char * pszName, IxFileInfoEx * pi );


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
   virtual IxErrnoType SetDescription( const char * pszName, wchar * pszDesc );


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
   virtual IxErrnoType GetFreeSpaceEx( const char *cpszPath, uint32 * pdwTotal,
      uint32 *pdwFree );

private:
   uint32 m_nRefs;
   uint32 serverNodeId;
   uint32 ipcMemPoolId;

   virtual IxFileClient * OpenSFSFile( const char *path,
      IxFileOpenFileMode mode );
   virtual void SetServerNodeId( uint32 serverNode );
   virtual void SetIPCMemPoolId( uint32 poolId );
};

#endif /* __cplusplus */

#endif /* IXFILEMGR_CLIENT_H */

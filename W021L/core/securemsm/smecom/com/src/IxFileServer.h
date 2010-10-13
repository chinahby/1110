#ifndef IXFILESERVER_H
#define IXFILESERVER_H
/*============================================================================
IxFileServer

ABSTRACT
   The IxFileServer interface definitions.

DESCRIPTION
   Provides basic file operations to EFS.


Copyright (c) 2005 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/IxFileServer.h#3 $

=========================================================================== */
#include "comdef.h"
#include "IxErrno.h"
#include "IxFile.h"

#ifdef __cplusplus

class IxFileServer : public IxFile {

    friend class IxFileMgr;

public:

   IxFileServer( );
   virtual ~IxFileServer( );

   virtual unsigned long AddRef( void );
   virtual unsigned long Release( void );

   /* Gets the data representing the object */
   virtual void GetState( uint8** stateDataPtr, uint32* stateDataLenPtr )
   { (void) stateDataPtr; (void) stateDataLenPtr; };

   /* Sets the data for the object */
   virtual void SetState( uint8* stateDataPtr, uint32 stateDataLen )
   { (void) stateDataPtr; (void) stateDataLen; };

   virtual int32       Read( void * pDest, uint32 nWant);

   virtual uint32      Write( PACKED const void * pBuffer, uint32 dwCount );

   virtual IxErrnoType GetInfo( IxFileInfo * pInfo );

   virtual IxErrnoType Seek( IxFileSeekType seek, int32 position );

   virtual IxErrnoType Truncate( uint32 truncate_pos );

   virtual IxErrnoType GetInfoEx( IxFileInfoEx * pi );

   virtual IxErrnoType SetCacheSize( int nSize );

   virtual void *      Map( void * pStart, uint32 dwSize, int protections,
      int flags, uint32 dwOffset );

   virtual int32       Tell( void );

private:
   virtual void SetFileHandle( int fileHandle );
   virtual void SetBuffering( bool buffering_enabled );
   int IxFileServer::FileSystemSeek( IxFileSeekType seek_type,
      int32 position );

   uint32        m_nRefs;
   int           file_handle;
   bool          is_buffered;
   int32         m_curPos;
   int32         m_fsCurPos;
   IxFileSystem *m_fs;
};

#endif /* __cplusplus */

#endif /* IXFILESERVER_H */

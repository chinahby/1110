#ifndef IXFILECLIENT_H
#define IXFILECLIENT_H
/*============================================================================
IxFile

ABSTRACT
   The IxFileClient interface definitions.

DESCRIPTION
   Provides basic file operations to EFS.


Copyright (c) 2005 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/IxFileClient.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
12/05/06    gr     Initial Revision

=========================================================================== */
#include "comdef.h"
#include "IxErrno.h"
#include "IxFile.h"

#ifdef __cplusplus

class IxFileClient : public IxFile {

    friend class IxFileMgrClient;

public:

   IxFileClient( );
   virtual ~IxFileClient( );

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
   virtual void SetServerObject( uint32 pServerObject );
   virtual void SetServerNodeId( uint32 serverNode );
   virtual void SetIPCMemPoolId( uint32 poolId );

   uint32  m_nRefs;
   uint32  serverNodeId;
   uint32  ipcMemPoolId;
   uint32  m_pServerObject;
};

#endif /* __cplusplus */

#endif /* IXFILECLIENT_H */

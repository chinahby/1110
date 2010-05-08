#ifndef IXHTTPXPORTSOCKET_H
#define IXHTTPXPORTSOCKET_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

             H T T P   C L I E N T   A P P L I C A T I O N

     I P   S O C K E T   T R A N S P O R T   I M P L E M E N T A T I O N

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/src/IxHTTPXportSocket.h#3 $ $DateTime: 2008/04/03 11:42:01 $ $Author: sladha $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/06   jay/ssm Support reading multiple TCP segs & DRM integration fixes    
12/29/05   om      Created Module

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "IxHTTP.h"
#include "IxHTTPXport.h"

class IxHTTPSvc;


/*===========================================================================

           DEFINITIONS AND CONSTANTS FOR HTTP TEST SERVER

===========================================================================*/

#define IXHTTP_BUFFER_SIZE (1024*16)

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*===========================================================================

                 API DEFINITION

===========================================================================*/
class IxHTTPXportSocket : public IxHTTPXport
{
public:
  IxHTTPXportSocket(IxHTTPSvc* svc);
  virtual ~IxHTTPXportSocket();

  virtual boolean IsAsync( void ){ return TRUE; };

  IxErrnoType SetServer( char* addr, uint32 len );

  virtual IxErrnoType Open( void );
  IxErrnoType Reopen( void );
  virtual IxErrnoType Close( void );
  virtual IxErrnoType Send( const uint8* dataPtr, uint32 dataLen );
  virtual IxErrnoType Send(  );
  virtual uint32      GetRsp( uint8** rspRef, IxErrnoType* myErrno );

  void MainEventHandler( void );

  void ProcessSocketCb( void );
  void SocketCallback( void );
  void NetworkCallback( void );

  void GetIPNodeByNameCallback(uint32 server);

  sint15 GetAppId( void ) { return __appId; };
  void SendEvent( void );
  void DeselectRead( void );
  void SelectRead( void );

private:
  typedef enum
  {
    INIT_STATE,
    NET_OPEN_STATE,
    DNS_LOOKUP_STATE,
    SOCK_OPEN_STATE,
    XPORT_READY_STATE,
    SOCK_CLOSE_STATE,
    NET_CLOSE_STATE,
    CLOSE_COMPLETE_STATE,
    COMPLETE_STATE,
    FAILED_STATE,
    RESERVED_STATE = 0xffffffffU
  } StateType;

  /* Private Methods */
  IxErrnoType ResolveServerName( void );
  IxErrnoType OpenNetwork( void );
  IxErrnoType CloseNetwork( void );
  IxErrnoType OpenSocket( void );
  IxErrnoType CloseSocket( void );
  IxErrnoType CloseSync( void );
  void ReadFromSocket(void);

  /* Private Data */
  StateType    __state;
  char*        __serverName;
  uint32       __serverAddr;
  uint16       __serverPort;
  sint15       __appId;
  uint32       __eventMask;
  void*        __eventUdataPtr;
  sint15       __serverSocket;
  IxHTTPSvc*   __svc;
  const char*  __sendData;
  uint32       __sendLen;
  uint32       __sendOffset;
  char*        __readData;
  uint32       __dataLen;
  boolean      __reopen;

public:
  static uint32  __uPDPProfileNum;

};

#endif /* IXHTTPXPORTSOCKET_H */

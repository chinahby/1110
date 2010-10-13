#ifndef IXHTTPXPORTWINSOCK_H
#define IXHTTPXPORTWINSOCK_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

             H T T P   C L I E N T   A P P L I C A T I O N

   W I N   S O C K E T   T R A N S P O R T   I M P L E M E N T A T I O N

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/src/IxHTTPXportWinSock.h#1 $ $DateTime: 2007/07/17 11:00:30 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/05   om      Created Module

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "IxHTTP.h"
#include "IxHTTPXport.h"
#include "Winsock2.h"



/*===========================================================================

           DEFINITIONS AND CONSTANTS FOR HTTP TEST SERVER

===========================================================================*/

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*===========================================================================

                 API DEFINITION

===========================================================================*/
class IxHTTPXportWinSock : public IxHTTPXport
{
public:
  IxHTTPXportWinSock();
  ~IxHTTPXportWinSock();

  boolean IsAsync( void ){ return FALSE; };

  IxErrnoType SetServer( char* addr, uint32 len );

  IxErrnoType Open( void );
  IxErrnoType Close( void );
  IxErrnoType Send( const uint8* dataPtr, uint32 dataLen );
  uint32      GetRsp( uint8** rspRef, IxErrnoType* myErrno);

private:
  IxErrnoType ResolveServerName( void );
  IxErrnoType OpenNetwork( void );
  IxErrnoType CloseNetwork( void );
  IxErrnoType OpenSocket( void );
  IxErrnoType CloseSocket( void );

private:
  char*     __serverName;
  uint32    __serverAddr;
  uint16    __serverPort;
  SOCKET    __serverSocket;
  uint8*    __rspPtr;
  uint32    __rspLen;
};

#endif /* IXHTTPXPORTWINSOCK_H */

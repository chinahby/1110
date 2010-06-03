#ifndef IXHTTPCLNT_H
#define IXHTTPCLNT_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

             H T T P   C L I E N T   A P P L I C A T I O N

           I P C   C L I E N T   I M P L E M E N T A T I O N

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/src/IxHTTPClnt.h#5 $ 
  $DateTime: 2008/03/24 15:40:18 $ 
  $Author: glennf $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/06  vdr      Start using KxIPC
12/29/05   om      Created Module

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ixipc.h"

#include "IxHTTP.h"
#include "IxHTTPPtcl.h"
#include "IxHTTPPtclV11.h"


/*===========================================================================

           DEFINITIONS AND CONSTANTS FOR HTTP TEST SERVER

===========================================================================*/

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
extern "C" void IxHTTPClnt_ProcessMsg( ixipc_msg_type**  msgPtr );

/*===========================================================================

                 API DEFINITION

===========================================================================*/
class IxHTTPClnt : public IxHTTP
{
public:
  IxHTTPClnt();
  virtual ~IxHTTPClnt();

  /* Method to specifiy header attributes */
  virtual IxErrnoType AddAttribute( const char* attr, const char* value );
  virtual IxErrnoType GetAttribute( const char* attr, char **value );
  virtual IxErrnoType SetAttribute( const char* attr, const char* value );
  virtual const char** GetSupportedAttributes( void );

  /* GET method and response */
  virtual IxErrnoType Get( char*  urlPtr,uint32 urlLen );
  virtual IxErrnoType Get( IxStream& urlStream );

  virtual void GetNotifier( IxErrnoType   myErrno,
                            IxStream*     rspStream,
                            uint32        rspStatusCode) = 0;

  /* POST method and response */
  virtual IxErrnoType Post( char*   urlPtr, 
                            uint32  urlLen,
                            char*   bodyPtr,
                            uint32  bodyLen );

  virtual IxErrnoType Post( IxStream&  urlStream,
                            IxStream&   bodyStream );

  virtual void PostNotifier( IxErrnoType   myErrno,
                             IxStream*     rspStream,
                             uint32        rspStatusCode) = 0;
  
  void SetTimeout ( uint32 miliSec ) { __timeOut = miliSec; };

  
  IxErrnoType Close( void );

  /* Response message handler */
  void ProcessMsg( ixipc_msg_type**  msgPtr );

  uint32 GetClientId(void) {return __clientId;};

protected:
  uint32                  __uniqueId;

private:
  char*                   __urlPtr;
  IxHTTPPtcl::AttrType    __attrList[IXHTTP_ATTR_LIST_SIZE];
  uint32                  __clientId;
  uint32                  __timeOut;
  boolean                 __haveSvcObj;
  static uint32 clntId;
};

#endif /* IXHTTP_H */

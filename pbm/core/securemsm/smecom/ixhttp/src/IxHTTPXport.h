#ifndef IXHTTPXPORT_H
#define IXHTTPXPORT_H
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

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/src/IxHTTPXport.h#1 $ $DateTime: 2007/07/17 11:00:30 $ $Author: jmiera $

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


/*===========================================================================

           DEFINITIONS AND CONSTANTS FOR HTTP TEST SERVER

===========================================================================*/

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*===========================================================================

                 API DEFINITION

===========================================================================*/
class IxHTTPXport
{
public:
  IxHTTPXport(){};
  virtual ~IxHTTPXport(){};
  
  virtual boolean IsAsync( void ) = 0;

  virtual IxErrnoType Open( void ) = 0;
  //virtual void              OpenNotifier( IxErrnoType errno ) = 0;

  virtual IxErrnoType Close( void ) = 0;
  //virtual void              CloseNotifier( IxErrnoType errno ) = 0; 

  virtual IxErrnoType Send( const uint8* dataPtr, uint32 dataLen ) = 0;

  virtual IxErrnoType Send( ) = 0;
  //virtual void              ReceiveNotifier( uint8 dataPtr, uint32 dataLen ) = 0;

  virtual uint32           GetRsp( uint8** rspRef,  IxErrnoType* myErrno) = 0;
};

#endif /* IXHTTP_H */

#ifndef IXHTTPPTCLV11_H
#define IXHTTPPTCLV11_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

             H T T P   C L I E N T   A P P L I C A T I O N

             H T T P   v. 1.1   I M P L E M E N T A T I O N

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/src/IxHTTPPtclV11.h#1 $
  $DateTime: 2007/07/17 11:00:30 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/05   om      Created Module

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "IxHTTPPtcl.h"


/*===========================================================================

           DEFINITIONS AND CONSTANTS FOR HTTP TEST SERVER

===========================================================================*/

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*===========================================================================

                 API DEFINITION

===========================================================================*/
class IxHTTPPtclV11 : public IxHTTPPtcl
{
public:
  IxHTTPPtclV11();
  IxHTTPPtclV11( AttrType* attrList );
  virtual ~IxHTTPPtclV11();

  virtual char*    CreateHeader( char* urlPtr, MethodType method, int bodyLength );
  virtual IxErrnoType   ParseHeader( char** rspRef, uint32* rspLen );

private:
  AttrType*   __attrList;
};

#endif /* IXHTTPPTCLV11_H */

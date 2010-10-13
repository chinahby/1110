#ifndef IXHTTPPTCL_H
#define IXHTTPPTCL_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

             H T T P   C L I E N T   A P P L I C A T I O N

                H T T P   I M P L E M E N T A T I O N

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/src/IxHTTPPtcl.h#1 $ $DateTime: 2007/07/17 11:00:30 $ $Author: jmiera $

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
class IxHTTPPtcl
{
public:
  typedef enum
  {
    GET,
    POST,
    RESERVED = 0xffffffffU
  } MethodType;

  typedef struct
  {
    char*    name;
    char*    value;
    boolean  rcvCheckRequired;
  } AttrType;

  IxHTTPPtcl(){};
  IxHTTPPtcl( AttrType* attrList ){(void) attrList; /* Dummy stmt to supress warning on unused variable */};
  virtual ~IxHTTPPtcl(){};

  virtual char*    CreateHeader( char* urlPtr, MethodType method, int bodyLength) = 0;
  virtual IxErrnoType   ParseHeader( char** rspRef, uint32* rspLen ) = 0;
};

#endif /* IXHTTPPTCL_H */

#ifndef IXHTTPMIMEDEFAULTHANDLER_H
#define IXHTTPMIMEDEFAULTHANDLER_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

    H T T P   M I M E   D E F A U L T   A N D L E R   I N T E R F A C E

GENERAL DESCRIPTION
  This interface defines the generic MIME handler used by IxHTTP for 
  content types other tan the requested one.

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/src/IxHTTPMimeDefaultHandler.h#1 $ $DateTime: 2007/07/17 11:00:30 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/05   om      Created Module

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "IxStream.h"
#include "IxErrno.h"
#include "IxHTTPMimeHandler.h"


/*===========================================================================

           DEFINITIONS AND CONSTANTS FOR HTTP TEST SERVER

===========================================================================*/

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/


/*===========================================================================

                 API DEFINITION

===========================================================================*/
class IxHTTPMimeDefaultHandler : public IxHTTPMimeHandler
{
public:
  /* Method definitions */
  IxHTTPMimeDefaultHandler(){};
  virtual ~IxHTTPMimeDefaultHandler();

  /* Method to handle content of a registered type */
  virtual void ProcessContent( char* mimeType, uint8* contentPtr, uint32 contentLen );
  virtual void ProcessContent( char* mimeType, IxStream* contentStreamPtr );
};

#endif /* IXHTTPMIMEDEFAULTHANDLER_H */

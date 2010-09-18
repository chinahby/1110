#ifndef IXHTTPMIMEREGISTRY_H
#define IXHTTPMIMEREGISTRY_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

          H T T P   M I M E   H A N D L E R   R E G I S T R Y

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/inc/IxHTTPMimeRegistry.h#1 $ $DateTime: 2007/07/17 11:00:30 $ $Author: jmiera $

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
#include "IxHTTPMimeDefaultHandler.h"


/*===========================================================================

           DEFINITIONS AND CONSTANTS FOR HTTP TEST SERVER

===========================================================================*/

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
#define IXHTTTPMIMEREGISTRY_MAX_MIME_LENGTH   256

/*===========================================================================

                 API DEFINITION

===========================================================================*/
class IxHTTPMimeRegistry
{
public:
  /* Storage for the handlers */
  typedef struct mimeHandlerS
  {
    char                      mimeType[IXHTTTPMIMEREGISTRY_MAX_MIME_LENGTH];
    IxHTTPMimeHandler*        handlerPtr;
    struct mimeHandlerS*      nextPtr;
  } HandlerStoreType;

  /* Method definitions */

  /* Methods to reguster and unregister handlers */
  static IxErrnoType RegisterHandler( IxHTTPMimeHandler* handlerPtr,
                                      const char* mimeTypePtr );
  static IxErrnoType DeregisterHandler( IxHTTPMimeHandler* handlerPtr );

  static IxHTTPMimeHandler* GetHandler( const char* mimeTypePtr );

private:

  /* Private data */
  static HandlerStoreType*         __registryPtr;
};
 

#endif /* IXHTTPMIMEREGISTRY_H */

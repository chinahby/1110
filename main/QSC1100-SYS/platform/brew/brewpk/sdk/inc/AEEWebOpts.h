#ifndef AEEWEBOPTS_H
#define AEEWEBOPTS_H
/*======================================================

FILE:    AEEWebOpts.h

SERVICES:
        IWebOpts

GENERAL DESCRIPTION:
        WebOpts stuff, an alias for xOpt

=====================================================

      Copyright © 2006 QUALCOMM Incorporated 
              All Rights Reserved.
           QUALCOMM Proprietary/GTDR

=====================================================*/
#include "AEE.h"
#include "AEEError.h"
#include "../../inc/AEEIxOpts.h"

/* ======================================================================= 
   Type Declarations
   ======================================================================= */

#define INHERIT_IWebOpts INHERIT_IxOpts
#define IWebOpts IxOpts
#define IWebOptsVtbl IxOptsVtbl

typedef struct xOpt WebOpt;

#define WEBOPT_RESERVED       XOPT_RESERVED
#define WEBOPT_RESERVED_LAST  XOPT_RESERVED_LAST
#define WEBOPT_END            XOPT_END                                     
#define WEBOPT_ANY            XOPT_ANY
#define WEBOPT_REMOVED        XOPT_REMOVED 
#define WEBOPT_COPYOPTS       XOPT_COPYOPTS

#define WEBOPT_SZ             XOPT_SZ
#define WEBOPT_SZ_LAST        XOPT_SZ_LAST

#define WEBOPT_32BIT          XOPT_32BIT
#define WEBOPT_32BIT_LAST     XOPT_32BIT_LAST

#define WEBOPT_INTERFACE      XOPT_INTERFACE
#define WEBOPT_INTERFACE_LAST XOPT_INTERFACE_LAST

#define WEBOPT_DEFAULTS       XOPT_DEFAULTS

#define WEBOPT_VARBUFFER      XOPT_VARBUFFER
#define WEBOPT_VARBUFFER_LAST XOPT_VARBUFFER_LAST

#define WEBOPT_ASN1           XOPT_ASN1
#define WEBOPT_ASN1_LAST      XOPT_ASN1_LAST

#define WEBOPT_WSZ            XOPT_WSZ
#define WEBOPT_WSZ_LAST       XOPT_WSZ_LAST

#define WEBOPT_VARIABLE       XOPT_VARIABLE
#define WEBOPT_VARIABLE_LAST  XOPT_VARIABLE_LAST
#define WEBOPT_VARIABLE_INC   XOPT_VARIABLE_INC

#define WEBOPT_VARIABLE_UNIQUE  XOPT_VARIABLE_UNIQUE

/*=====================================================================
  function declarations 
  ===================================================================== */

#define IWEBOPTS_AddRef             IxOpts_AddRef
#define IWEBOPTS_Release            IxOpts_Release
#define IWEBOPTS_QueryInterface     IxOpts_QueryInterface
#define IWEBOPTS_AddOpt             IxOpts_AddOpt
#define IWEBOPTS_RemoveOpt          IxOpts_RemoveOpt
#define IWEBOPTS_GetOpt             IxOpts_GetOpt

#define WEBOPT_GETVARBUFFERLENGTH   XOPT_GETVARBUFFERLENGTH
#define WEBOPT_GETVARBUFFERDATA     XOPT_GETVARBUFFERDATA
#define WEBOPT_SETVARBUFFER         XOPT_SETVARBUFFER

/*
=======================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

WebOpt

Description:
   WebOpt is an alias for xOpt.

See Also:
   xOpt

======================================================================= 
   INTERFACES DOCUMENTATION
======================================================================= 

IWebOpts Interface

Description:
   IWebOpts is an alias for the IxOpts interface.

See Also:
   IxOpts

=======================================================================

IWEBOPTS_AddRef()

Description:
   This function is inherited from IQI_AddRef().

Comments:
IWebOpts is the base class of IWeb and IWebResp, which use IWebOpts 

=======================================================================

IWEBOPTS_Release()

Description:
   This function is inherited from IQI_Release().

=======================================================================

IWEBOPTS_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

=======================================================================

IWEBOPTS_AddOpt()

Description:
   This function is inherited from IxOpts_AddOpt().

=======================================================================

IWEBOPTS_RemoveOpt()

Description:
   This function is inherited from IxOpts_RemoveOpt().

=======================================================================

IWEBOPTS_GetOpt()

Description:
   This function is inherited from IxOpts_GetOpt().

=======================================================================
*/

#endif /* AEEWEBOPTS_H */

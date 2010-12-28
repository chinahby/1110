#ifndef AEEBTSPPCLIENTGETOBJECT_H
#define AEEBTSPPCLIENTGETOBJECT_H
/*=============================================================================
FILE:         AEEBTSPPClientGetObject.h

SERVICES:     GetObject method for SPP client interface

DESCRIPTION:  This header file contains definition of GetObject method for 
              SPP client interface
===============================================================================
Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#include "AEEBT.h"
#include "AEEBTSPPClient.h"

static __inline int IBT_GetSPPClient(IBT* p, void** ppSPPCli)
{
   return AEEGETPVTBL((p),IBT)->GetObject((p),AEEIID_BT_SPP_CLIENT,
                                          NULL,0,(ppSPPCli));
}

/*=============================================================================
IBT_GetSPPClient()

Description:
   This function creates an object of type IBTSPPClient.

Prototype:
   int IBT_GetSPPClient(IBT* po, void** ppSPPCli);

Parameters:
   po [i]       : the IBT object
   ppSPPCli [o] : pointer to the created IBTSPPClient object if succesful,
                  NULL otherwise

Return Value:
   SUCCESS      : ppSPPClient points to an object of type IBTSPPClient
   EBADPARM     : invalid input pointer(s)
   ENOMEMORY    : no memory to create object
   EUNSUPPORTED : IBTSPPClient class is not supported
=============================================================================*/
#endif //AEEBTSPPCLIENTGETOBJECT_H

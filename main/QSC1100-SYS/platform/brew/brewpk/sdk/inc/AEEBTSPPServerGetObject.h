#ifndef AEEBTSPPSERVERGETOBJECT_H
#define AEEBTSPPSERVERGETOBJECT_H
/*=============================================================================
FILE:         AEEBTSPPServerGetObject.h

SERVICES:     GetObject method for SPP server interface

DESCRIPTION:  This header file contains definition of GetObject method for 
              SPP server interface
===============================================================================
Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#include "AEEBT.h"
#include "AEEBTSPPServer.h"

static __inline int IBT_GetSPPServer(IBT* po, BTServiceClassUUID uuid,
                                     const uint8* pszServiceName, 
                                     uint16 version,
                                     void** ppSPPSrv)
{
   struct
   {
      BTServiceClassUUID uuid;
      const uint8* pszServiceName;
      uint16 version;
   } local;
   local.uuid = uuid;
   local.version = version;
   local.pszServiceName = pszServiceName;
   return AEEGETPVTBL((po),IBT)->GetObject(po,AEEIID_BT_SPP_SERVER,
                                           &local,sizeof(local),ppSPPSrv);

}
/*=============================================================================
INTERFACE DOCUMENTATION 
=============================================================================
IBT Interface Reference
===========================================================================
IBT_GetSPPServer()

Description:
   This function creates an object of type IBTSPPServer.

Prototype:
   int IBT_GetSPPServer(IBT*                po, 
                        BTServiceClassUUID  uuid,
                        const uint8*        pszServiceName, 
                         uint16             version,
                        void**              ppSPPSrv);

Parameters:
   po [i]           : the IBT object
   uuid [i]         : specific UUID advertising the kind of SPP service 
                      the SPP server supports
   pServiceName [i] : name of the service to be advertised
   version          : Version of the server
   ppSPPSrv [o]     : pointer to the created IBTSPPServer object if succesful,
                      NULL otherwise

Return Value:
   SUCCESS      : ppSPPSrv points to an object of type IBTSPPServer
   EBADPARM     : invalid input pointer(s)
   ENOMEMORY    : no memory to create object
   EUNSUPPORTED : IBTSPPServer class is not supported

===============================================================================*/
#endif //AEEBTSPPSERVERGETOBJECT_H

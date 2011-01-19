#ifndef _AEESOCKPORT_H_
#define _AEESOCKPORT_H_

/*======================================================
FILE:      AEESockPort.h

SERVICES:  ISockPort (IPort based Socket).

GENERAL DESCRIPTION:

  ISOCKPORT provides a socket interface that operates on generic
  addresses, such that internet (AEE_AF_INET, AEE_AF_INET6) and
  local domain (AEE_AF_LOCAL) sockets can be accessed through the same interface.

  Currently local domain (AEE_AF_LOCAL) sockets are not supported.

PUBLIC CLASSES AND STATIC FUNCTIONS:

    ISOCKPORT

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/* older includes must stay */
#include "AEEPort.h"
#include "AEENetworkTypes.h"
#include "AEEStdLib.h"

/* new definition */
#include "../../inc/AEEISockPort.h"
#include "../../inc/AEESockPort.bid"

#define AEECLSID_SOCKPORT  AEECLSID_SockPort

#define ISOCKPORT_AddRef            ISockPort_AddRef
#define ISOCKPORT_Release           ISockPort_Release
#define ISOCKPORT_QueryInterface    ISockPort_QueryInterface
#define ISOCKPORT_Read              ISockPort_Read
#define ISOCKPORT_Readable          ISockPort_Readable
#define ISOCKPORT_GetLastError      ISockPort_GetLastError
#define ISOCKPORT_Write             ISockPort_Write
#define ISOCKPORT_Writeable         ISockPort_Writeable
#define ISOCKPORT_IOCtl             ISockPort_IOCtl
#define ISOCKPORT_Open              ISockPort_Open
#define ISOCKPORT_Close             ISockPort_Close
#define ISOCKPORT_OpenEx            ISockPort_OpenEx
#define ISOCKPORT_SelectNetwork     ISockPort_SelectNetwork
#define ISOCKPORT_Bind              ISockPort_Bind
#define ISOCKPORT_Listen            ISockPort_Listen
#define ISOCKPORT_Accept            ISockPort_Accept
#define ISOCKPORT_Connect           ISockPort_Connect
#define ISOCKPORT_WriteV            ISockPort_WriteV
#define ISOCKPORT_ReadV             ISockPort_ReadV
#define ISOCKPORT_SendTo            ISockPort_SendTo
#define ISOCKPORT_RecvFrom          ISockPort_RecvFrom
#define ISOCKPORT_GetSockName       ISockPort_GetSockName
#define ISOCKPORT_GetPeerName       ISockPort_GetPeerName
#define ISOCKPORT_GetOpt            ISockPort_GetOpt
#define ISOCKPORT_SetOpt            ISockPort_SetOpt
#define ISOCKPORT_Shutdown          ISockPort_Shutdown
#define ISOCKPORT_GetSDBAckInfo     ISockPort_GetSDBAckInfo

#define ISOCKPORT_ReadableEx        ISockPort_ReadableEx
#define ISOCKPORT_WriteableEx       ISockPort_WriteableEx
#define ISOCKPORT_DbgMark           ISockPort_DbgMark

#define ISOCKPORT_SelectNetworkEx   ISockPort_SelectNetworkEx
#define ISOCKPORT_IsRealized        ISockPort_IsRealized
#define ISOCKPORT_Realize           ISockPort_Realize

#endif /* _AEESOCKPORT_H_ */

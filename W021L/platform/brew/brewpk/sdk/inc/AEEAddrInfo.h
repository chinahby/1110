#ifndef AEEADDRINFO_H
#define AEEADDRINFO_H

/*=============================================================================

FILE:         AEEAddrInfo.h

SERVICES:     IAddrInfo interface

DESCRIPTION:  

===============================================================================
        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/* older includes must stay */
#include "AEE.h"
#include "AEENetworkTypes.h"

#include "../../inc/AEEIAddrInfo.h"

#define AEEIID_ADDRINFO AEEIID_IAddrInfo

#define IADDRINFO_AddRef         IAddrInfo_AddRef
#define IADDRINFO_Release        IAddrInfo_Release
#define IADDRINFO_QueryInterface IAddrInfo_QueryInterface
#define IADDRINFO_SelectNetwork  IAddrInfo_SelectNetwork
#define IADDRINFO_StartQuery     IAddrInfo_StartQuery
#define IADDRINFO_GetResult      IAddrInfo_GetResult
#define IADDRINFO_GetResultAddr  IAddrInfo_GetResultAddr

#endif /* #ifndef AEEADDRINFO_H */

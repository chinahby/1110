#ifndef AEEADDRINFOCACHECACHE_H
#define AEEADDRINFOCACHECACHE_H

/*=============================================================================

FILE:         AEEAddrInfoCache.h

SERVICES:     IAddrInfoCache interface

DESCRIPTION:  

===============================================================================
        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEE.h"

#include "../../inc/AEEIAddrInfoCache.h"

#define AEEIID_ADDRINFOCACHE AEEIID_IAddrInfoCache

#define IADDRINFOCACHE_AddRef           IAddrInfoCache_AddRef
#define IADDRINFOCACHE_Release          IAddrInfoCache_Release
#define IADDRINFOCACHE_QueryInterface   IAddrInfoCache_QueryInterface
#define IADDRINFOCACHE_SelectNetwork    IAddrInfoCache_SelectNetwork
#define IADDRINFOCACHE_ClearCache       IAddrInfoCache_ClearCache
#define IADDRINFOCACHE_ClearCacheEntry  IAddrInfoCache_ClearCacheEntry

#endif /* #ifndef AEEADDRINFOCACHECACHE_H */

#ifndef AEEIADDRINFOCACHECACHE_H
#define AEEIADDRINFOCACHECACHE_H

/*=============================================================================

FILE:         AEEIAddrInfoCache.h

SERVICES:     IAddrInfoCache interface

DESCRIPTION:  

===============================================================================
        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEIQI.h"

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

//*********************************************************************************************************
//
// IAddrInfoCache Interface
//
//*********************************************************************************************************

#define AEEIID_IAddrInfoCache    0x01037ae6

#define INHERIT_IAddrInfoCache(iname) \
   INHERIT_IQI(iname); \
   int        (*SelectNetwork)(iname* po, int nNetwork); \
   int        (*ClearCache)(iname* po); \
   int        (*ClearCacheEntry)(iname* po, const char *szNodeName); \
   int        (*ExtFunc)(iname* po, AEECLSID id, void *pBuf, int nBufSize) \
              /* Extension Function to allow future enhancement to the interface without breaking its binary compatibility */ \

AEEINTERFACE_DEFINE(IAddrInfoCache);

/////////////////////////
// IAddrInfoCache methods
/////////////////////////

static __inline uint32 IAddrInfoCache_AddRef(IAddrInfoCache *me)
{
   return AEEGETPVTBL(me,IAddrInfoCache)->AddRef(me);
}

static __inline uint32 IAddrInfoCache_Release(IAddrInfoCache *me)
{
   return AEEGETPVTBL(me,IAddrInfoCache)->Release(me);
}

static __inline int IAddrInfoCache_QueryInterface(IAddrInfoCache *me, 
                                                  AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IAddrInfoCache)->QueryInterface(me, cls, ppo);
}

static __inline int IAddrInfoCache_SelectNetwork(IAddrInfoCache *me, int nNetwork)
{
   return AEEGETPVTBL(me,IAddrInfoCache)->SelectNetwork(me, nNetwork);
}

static __inline int IAddrInfoCache_ClearCache(IAddrInfoCache *me)
{
   return AEEGETPVTBL(me,IAddrInfoCache)->ClearCache(me);
}

static __inline int IAddrInfoCache_ClearCacheEntry(IAddrInfoCache *me, const char *szNodeName)
{
   return AEEGETPVTBL(me,IAddrInfoCache)->ClearCacheEntry(me, szNodeName);
}

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IAddrInfoCache Interface

Description:
   This interface provides access to the address info results cache.
   This interface allows to clear a specific host name or the whole cache.

   IAddrInfoCache_ClearCache() and IAddrInfoCache_ClearCacheEntry() are used for
   clearing cache entries.

The following header file is required:~
   BREW 4.0 - AEEIAddrInfoCache.h
   BREW 3.1 - AEEAddrInfoCache.h

=============================================================================

IAddrInfoCache_AddRef()

Description:
   This function is inherited from IBase_AddRef(). 

See Also:
   IAddrInfoCache_Release()

=============================================================================

IAddrInfoCache_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface(). 

See Also:
   IQI_QueryInterface

=============================================================================

IAddrInfoCache_Release()

Description:
   This function is inherited from IBase_Release(). 

See Also:
   IAddrInfoCache_AddRef()

=============================================================================

IAddrInfoCache_SelectNetwork()

Description:
   This function selects a specific data network.

Prototype:
   int IAddrInfoCache_SelectNetwork(IAddrInfoCache *me, int nNetwork)

Parameters:
   me: the interface pointer
   nNetwork: [in] : data network type (AEE_NETWORK_*)
 
Return Value:
   AEE_NET_SUCCESS: the data network was selected
   AEE_EBADSTATE: the network is not valid
   AEE_NET_EOPNOTSUPP: the network has already been selected.
   AEE_EFAILED: other error.

   Other error codes are also possible.

Comments:
   When an IAddrInfoCache instance is created, no network is selected.  
   If an IAddrInfoCache method that requires a network (i.e. ClearCache) is called before
   IAddrInfoCache_SelectNetwork(), AEE_NETWORK_DEFAULT will be implicitly selected,
   and the decision of which network to actually use is deferred to lower layers.

   Once a network has been selected, either explicitly via
   IAddrInfoCache_SelectNetwork(), or implicitly as described above, the network 
   may not be changed. To use a different network, a new IAddrInfoCache instance is 
   required.

Version:
   Introduced BREW Client 3.1.5

See Also:
   Data Networks

=============================================================================

IAddrInfoCache_ClearCache()

Description:
   This function clears the cache.
  
Prototype:
   int IAddrInfoCache_ClearCache(IAddrInfoCache *me);

Parameters:
   me: the interface pointer

Return Value:

   AEE_SUCCESS:
      The cache was cleared.

   All other values indicate failure; the caller should verify that
   ClearCache() succeeded.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

=============================================================================
IAddrInfoCache_ClearCacheEntry()

Description:
   This function clears a single cache entry.

Prototype:
   int IAddrInfoCache_ClearCacheEntry(IAddrInfoCache *me, const char *szNodeName);

Parameters:
   me: the interface pointer
   szNodeName: [in] : host name

Return Value:

   AEE_NET_SUCCESS:
      The cache entry was cleared or does not exist.

   All other values indicate failure; the caller should verify that
   ClearCacheEntry() succeeded.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

=============================================================================
*/

#endif /* #ifndef AEEIADDRINFOCACHECACHE_H */

#ifndef  IMEMCACHE_H
#define  IMEMCACHE_H
/*=================================================================================
FILE:          IMemCache.h

SERVICES:      IMemCache interface

DESCRIPTION:   

PUBLIC CLASSES:

INITIALIAZTION AND SEQUENCEING REQUIREMENTS:

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEEQueryInterface.h"


//*********************************************************************************
//
// IMemCache Interface
//
//*********************************************************************************

#define     AEEIID_MEMCACHE          0x010303a1

#define     MEMCACHE_WRITEBACK_CACHE  1
#define     MEMCACHE_FLUSH_DATACACHE  2
#define     MEMCACHE_FLUSH_INSTCACHE  4



typedef struct IMemCache IMemCache;

#define INHERIT_IMemCache(iname) \
   INHERIT_IQueryInterface(iname); \
   int  (*Clean)(iname *po, uint32 dwAddr, uint32 dwSize, int nFlags)


AEEINTERFACE(IMemCache)
{
   INHERIT_IMemCache(IMemCache);
};


static __inline uint32 IMEMCACHE_AddRef(IMemCache *me)
{
   return AEEGETPVTBL(me,IMemCache)->AddRef(me);
}

static __inline uint32 IMEMCACHE_Release(IMemCache *me)
{
   return AEEGETPVTBL(me,IMemCache)->Release(me);
}

static __inline int IMEMCACHE_QueryInterface(IMemCache *me, AEECLSID cls,
                                             void ** ppo)
{
   return AEEGETPVTBL(me,IMemCache)->QueryInterface(me, cls, ppo);
}

static __inline int IMEMCACHE_FlushDataCache(IMemCache *me, 
                                             uint32 dwAddr, uint32 dwSize)
{
   return AEEGETPVTBL(me,IMemCache)->Clean(me, dwAddr, dwSize, 
                                           MEMCACHE_FLUSH_DATACACHE);
}

static __inline int IMEMCACHE_SyncDataCache(IMemCache *me, 
                                             uint32 dwAddr, uint32 dwSize)
{
   return AEEGETPVTBL(me,IMemCache)->Clean(me, dwAddr, dwSize, 
                                           MEMCACHE_WRITEBACK_CACHE |
                                           MEMCACHE_FLUSH_DATACACHE );
}

static __inline int IMEMCACHE_FlushInstCache(IMemCache *me, 
                                             uint32 dwAddr, uint32 dwSize)
{
   return AEEGETPVTBL(me,IMemCache)->Clean(me, dwAddr, dwSize, 
                                           MEMCACHE_FLUSH_INSTCACHE);
}

/*============================================================================
   INTERFACE DOCUMENTATION
==============================================================================

IMemCache Interface

Description:
   This interface is used to flush and/or synchronize the data and instruction
   caches on a Brew-enabled device. 

==============================================================================
Function: IMEMCACHE_AddRef()

This function is inherited from IQI_AddRef().

==============================================================================
Function: IMEMCACHE_Release()

This function is inherited from IQI_Release().

==============================================================================
Function: IMEMCACHE_QueryInterface()

This function is inherited from IQI_QueryInterface().

==============================================================================
Function: IMEMCACHE_FlushDataCache()

Description:
   This method is used to flush the data cache.

Prototype:
   int IMEMCACHE_FlushDataCache(IMemCache *me, uint32 dwAddr, uint32 dwSize)

Parameters:
   po          [in]: Pointer to IMemCache interface.
   dwAddr          : Start address of the buffer that needs to be flushed
                     from the cache.
   dwSize          : Size of the buffer that needs to be flushed from the
                     cache.

Return Value:
   SUCCESS: if function executed correctly.~
   EFAILED: if cache could not be flushed.

Comments:
   This method cleans the data cache without writing back to memory. Please
   use IMEMCACHE_SyncDataCache to synchronize the data cache with memory.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4.

See Also:

==============================================================================
Function: IMEMCACHE_SyncDataCache()

Description:
   This method is used to flush the data cache.

Prototype:
   int IMEMCACHE_SyncDataCache(IMemCache *me, uint32 dwAddr, uint32 dwSize)

Parameters:
   po          [in]: Pointer to IMemCache interface.
   dwAddr          : Start address of the buffer that needs to be synchronized
                     from the data cache to memory.
   dwSize          : Size of the buffer that needs to be synchronized
                     from the data cache to memory.

Return Value:
   SUCCESS: if function executed correctly.~
   EFAILED: if cache could not be synchronized.

Comments:
   This method writes back data from cache to memory before flushing the data
   cache. Please use IMEMCACHE_FlushDataCache to flush data from the data cache
   without writing it back to memory.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4.

See Also:

==============================================================================
Function: IMEMCACHE_FlushInstCache()

Description:
   This method is used to flush the data cache.

Prototype:
   int IMEMCACHE_FlushInstCache(IMemCache *me, uint32 dwAddr, uint32 dwSize)

Parameters:
   po          [in]: Pointer to IMemCache interface.
   dwAddr          : Start address of the instuction buffer that needs to be 
                     flushed from the cache.
   dwSize          : Size of the instruction buffer that needs to be flushed from
                     the cache.

Return Value:
   SUCCESS: if function executed correctly.~
   EFAILED: if cache could not be flushed.

Comments:

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4.

See Also:   
============================================================================*/


#endif /* IMEMCACHE_H */

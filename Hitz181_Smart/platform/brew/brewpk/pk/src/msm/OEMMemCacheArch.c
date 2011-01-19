/*======================================================
FILE:  OEMMemCacheArch.c

SERVICES:


GENERAL DESCRIPTION:
   Functions for helping implement the MemCache interface
   and flushing the instruction and/or data caches.

        Copyright 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEMemCache.h"
#include "AEEStdLib.h"
#include "cache_mmu.h"

extern void mmu_clean_and_invalidate_data_cache(void);

//====================================================================
//
//====================================================================
extern int OEMMemCache_ArchClean(uint32 dwAddr, uint32 dwSize, int nFlags)
{
   if (nFlags & MEMCACHE_WRITEBACK_CACHE )
   {
      mmu_clean_and_invalidate_data_cache();
   }
   else if (nFlags & MEMCACHE_FLUSH_DATACACHE )
   {
      mmu_invalidate_data_cache_lines((uint32*)dwAddr, dwSize);
   }

   if (nFlags & MEMCACHE_FLUSH_INSTCACHE )
   {
      mmu_invalidate_instruction_cache();
   }

   return SUCCESS;
}



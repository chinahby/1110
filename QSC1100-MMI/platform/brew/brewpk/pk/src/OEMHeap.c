/*===========================================================================
                              OEMHeap.c

  OEM REFERENCE HEAP ROUTINES
  
  This file contains a reference implementation of a heap.
    
        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "OEMFeatures.h"

#ifdef OEMHEAP

#include "OEMHeap.h"

//Supervisor mode heap. All MALLOCs in SVC mode allocate
//from this buffer. OEMs need to fine tune this size.
#if !defined(OEM_STATIC_SVC_HEAP_SIZE)
#define  OEM_STATIC_SVC_HEAP_SIZE      65536
#endif //OEM_STATIC_SVC_HEAP_SIZE

// 
// Static Heap - Must Modify this size to fit the size on the phone!
//
#ifndef FEATURE_DYNAMIC_OEM_HEAP
static char guHeapBytes[OEM_STATIC_HEAP_SIZE];
#endif // !FEATURE_DYNAMIC_OEM_HEAP

void * gpuHeapBytes   = NULL;
uint32 guHeapBytesLen = 0;

int OEM_GetHeapInitBytes(void **ppHeapBytes, uint32 *puHeapBytesLen)
{
#ifdef FEATURE_DYNAMIC_OEM_HEAP
   *ppHeapBytes    = gpuHeapBytes;
/* NOTE: HeapBytesLen is reduced by 4 to allow a pad that would allow
   ADS PACKED struct accessing code to work without attempting to read
   beyond the heap. */
   *puHeapBytesLen = guHeapBytesLen - 4;
#else
   *ppHeapBytes    = guHeapBytes;
   *puHeapBytesLen = OEM_STATIC_HEAP_SIZE;
#endif

   return SUCCESS;
}

#endif /* #ifdef OEMHEAP */


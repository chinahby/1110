/*======================================================
FILE:  OEMHeap.h

SERVICES:  BREW OEM Heap interface

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE for heap and contained
in OEMHeap.c.

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(OEMHEAP_H)
#define OEMHEAP_H

#include "AEEStdLib.h"

#if defined(__cplusplus)
extern "C" {
#endif
#ifdef CUST_EDITION	
#include "AEE_OEMHeap.h"
#endif /*CUST_EDITION*/
extern int OEM_GetHeapInitBytes(void **ppHeapBytes, uint32 *puHeapBytesLen);

#if defined(__cplusplus)
}
#endif

#ifdef USES_RELEASE_VERSION
#if (AEE_DBG_HEAP == 1)
#define sys_realloc(p, n)      AEEHeap_DbgMark(AEEHeap_Realloc(p,n),__FILE__,__LINE__)
#define sys_strdup(p)          (char *)AEEHeap_DbgMark(AEEHeap_StrDup(p),__FILE__,__LINE__)
#define sys_malloc(n)          AEEHeap_DbgMark(AEEHeap_Malloc(n),__FILE__,__LINE__)
#else
#define sys_realloc            AEEHeap_Realloc
#define sys_strdup             AEEHeap_StrDup
#define sys_malloc             AEEHeap_Malloc
#endif

#define sys_free               AEEHeap_Free
#else
#define sys_realloc(p, n)      OEMOS_DbgReallocMark(AEEHeap_Realloc(p,n),p,__FILE__,__LINE__)
#define sys_strdup(p)          (char *)OEMOS_DbgMark(AEEHeap_StrDup(p),__FILE__,__LINE__)
#define sys_malloc(n)          OEMOS_DbgMark(AEEHeap_Malloc(n),__FILE__,__LINE__)
#define sys_free(p)            if(p != NULL){OEMOS_DbgUnMark(AEEHeap_Free,p,__FILE__,__LINE__); p = NULL;}
#endif
#define sys_lockmem            AEEHeap_Lock
#define sys_unlockmem          AEEHeap_Unlock

#define OEM_Realloc            sys_realloc
#define OEM_Malloc             sys_malloc
#define OEM_Free               sys_free


/*=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================
OEMHeap Interface
=======================================================================


Function: OEM_GetInitHeapBytes

Description: Provide's memory for BREW's user-mode heap, used for implicit
    initialization of BREW's user-mode heap.

Prototype: 
    int OEM_GetHeapInitBytes(void **ppHeapBytes, uint32 *puHeapBytesLen);

Parameters:
   ppHeapBytes: pointer to pointer to bytes BREW should use for the heap
   puHeapBytesLen: number of bytes in *ppHeapBytes that can be used for 
                    the heap

Return Value:
   EUNSUPPORTED if the OEM prefers to initialize BREW's heap explicitly
   SUCCESS if ppHeapBytes and puHeapBytesLen are correctly filled

Comments:
  This function is optional for an OEM to implement.  If this function is
     left unimplemented, however, AEEHeap_Init() must be called prior to 
     AEE_Init().

  An OEM may either explicitly initialize BREW's user-mode heap by 
     calling AEEHeap_Init() or by implementing this function, which 
     will be called on the first access of BREW's user-mode heap.

See Also:
   AEEHeap_Init()

==================================================================*/

#endif

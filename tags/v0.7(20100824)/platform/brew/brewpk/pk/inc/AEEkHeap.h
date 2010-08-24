#ifndef AEEKHEAP_H
#define AEEKHEAP_H
/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  AEEkHeap.h

SERVICES:  low-level, instantiable, sbrk-based heap functionality

GENERAL DESCRIPTION:
	Implementation for BREW's lowest heap

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "AEE.h"

typedef struct AEEkHeap AEEkHeap;
typedef struct AEEkHeapNodeInfo
{
   void        *p;
   uint32       uSize;
   uint32       uOverhead; /* includes node overhead and 
                              rounding overhead */
   uint32       uFlags;
#define KHNF_FREE 0x01

   uint32       uLockCount;
   void       **ppSentinel;

} AEEkHeapNodeInfo;

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**************************************************************************/
/*               AEE kHeap EXPORTED METHODS                               */
/**************************************************************************/

extern int AEEkHeap_Init(AEEkHeap *pHeap, 
                         void *(*pfnSbrk)(void *pSbrkCx, int32 nIncrement), 
                         void *pSbrkCx);

extern void *AEEkHeap_Realloc(AEEkHeap *pHeap, void *p, uint32 uSize);
   
extern uint32 AEEkHeap_Lock(AEEkHeap *pHeap, void *p);
extern uint32 AEEkHeap_Unlock(AEEkHeap *pHeap, void *p);
extern void **AEEkHeap_Sentinel(AEEkHeap *pHeap, void *p, void **ppSentinel);

extern int AEEkHeap_NodeInfo(AEEkHeap *pHeap, 
                             AEEkHeapNodeInfo *pNodeInfoBuf, 
                             unsigned uNodeInfoBufSize);

extern void AEEkHeap_Compress(AEEkHeap *pHeap);

extern int AEEkHeap_Walk(AEEkHeap *pHeap, AEEkHeapNodeInfo *pWalkBuf, 
                         unsigned uWalkBufSize);

extern boolean AEEkHeap_CheckMemAvail(AEEkHeap *me, uint32 uSize);

extern uint32 AEEkHeap_GetMemStats(AEEkHeap *me, uint32 * pdwTotal, uint32 * pdwMax);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEEkHeap

Description:
   An instance of a BREW heap.  This type is opaque to indicate
   that heap data structures are not be manipulated outside the kHeap 
   code.

   To create an AEEkHeap, call AEEkHeap_Init() with a pointer to a 
   potentially growable buffer (via the sbrk callback function)

Definition:
 typedef struct AEEkHeap AEEkHeap;

Members:
 None

=======================================================================

AEEkHeapNodeInfo

Description:
  AEEkHeapNodeInfo holds context for an in-progress heap "walk."  To start 
  a heap walk, set p to NULL.

Definition:
   typedef struct AEEkHeapNodeInfo 
   {
      void        *p;
      uint32       uSize;
      uint32       uOverhead;

      uint32       uFlags;
      
      uint32       uLockCount;
      void       **ppSentinel;
   
   } AEEkHeapNodeInfo;

Members:
  p: current node data pointer (user data)
  uSize: size of p
  uOverhead: amount of extra memory consumed by this node in the 
              heap, not including uSize, 
  uFlags: other information about the node, set to a logical-OR of 
          the following values:
            KHNF_FREE: if set in uFlags, indicates a free node
  uLockCount: node's lock count
  ppSentinel: the node's sentinel


=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
AEEkHeap Interface
=======================================================================  

Function: AEEkHeap_Init()

Description:
   Creates and initializes a new AEEkHeap.

Prototype:
   int AEEkHeap_Init(AEEkHeap *pHeap, 
                    void *(*pfnSbrk)(void *pSbrkCx, int32 nIncrement), 
                    void *pSbrkCx);

Parameters:
   pHeap: pointer to a buffer that the heap will instantiate itself in
   pfnSbrk: function to grow the heap's buffer, if pfnSbrk is NULL, 
             pSbrkCx is interpreted as the end of the heap buffer.
~
         Description:
            Grows, shrinks, and describes the accompanying pHeap buffer.
         
         Prototype:
            void *(*pfnSbrk)(void *pSbrkCx, int32 nIncrement)
         
         Parameters:
            pSbrkCx: callback context for growing pHeap
            nIncrement: description of how much to change the size of pHeap
            
         Return Value:
            The new *end* of pHeap.
         
         Comments:
            Calling pfnSbrk with 0 nIncrement should return the current 
                end of pHeap.
            Calling pfnSbrk with a negative value of nIncrement should 
                shrink pHeap, if possible.
*
   pSbrkCx: pHeap owner-supplied context, passed as first parameter to
             pfnSbrk

Return Value:
   SUCCESS on success
   EBADPARM pHeap is not sufficiently aligned (currently pHeap must be on 
             a 4-byte boundary)
   ENOMEMORY if pHeap is not big enough and can't be grown to accomodate
             the heap's internal data structure

Comments:
   pHeap (the base of the heap, the heap's buffer) must not change
   by default. The heap is set up to zero-initialize newly allocated 
    bytes and to mark newly freed bytes with 0xfe in DEBUG mode

Side Effects:
   None

See Also:
   None

========================================================================

Function: AEEkHeap_Realloc()

Description:
   Frees, mallocs, or reallocs a pointer within a heap.

Prototype:
   void *AEEkHeap_Realloc(AEEkHeap *pHeap, void *p, uint32 uSize)

Parameters:
   pHeap: pointer to the heap to use for the operation
   p: pointer to manipulate
   uSize: new desired size

Return Value:
   The new adjusted heap node

Comments:
   realloc(p,0) frees p.
   realloc(0,1) allocates 1 byte.
   The memory returned is zero-initialized, unless the high bit of 
     uSize is set to 1.

Side Effects:
   None

See Also:
   AEEkHeap_Lock()
   AEEkHeap_Unlock()
   AEEkHeap_Sentinel()

========================================================================
AEEkHeap_Unlock()
 See documentation for AEEkHeap_Lock()


========================================================================

Function: AEEkHeap_Lock()

Description:
   Locks and unlocks (make movable) a heap node.

Prototype:
   uint32 AEEkHeap_Lock(AEEkHeap *pHeap, void *p);

   uint32 AEEkHeap_Unlock(AEEkHeap *pHeap, void *p);

Parameters:
   pHeap: pointer to the heap to use for the operation
   p: pointer to lock/unlock
 
Return Value:
   New lock count

Comments:
   Unlock() has no effect unless the node's sentinel is non-null.
   Newly allocated nodes have a lock count of 1.
   The lock count cannot drop below 0, so this code:
===pre>
      for (uint32 i = 0; i < MAX_UINT32; i++) Unlock(p);
      Lock(p);
===/pre>

    results in a locked pointer.

Side Effects:
   If the lock count is equal to 0, the node is movable.
   The heap may move the node during a call to Unlock().

See Also:
   AEEkHeap_Sentinel()

========================================================================

Function: AEEkHeap_Sentinel()

Description:
   Sets a pointer's "sentinel," the memory used to hold the pointer
      value if the node becomes movable.

Prototype:
   void **AEEkHeap_Sentinel(AEEkHeap *pHeap, void *p, void **ppSentinel);

Parameters:
   pHeap: pointer to the heap to use for the operation
   p: pointer to set the sentinel for
   ppSentinel: new sentinel

Return Value:
   Previous sentinel value

Comments:
   ppSentinel, if non-null, *must* be initialized to the value of 
     the node, as the *application* sees it.  For example:
===pre>
        char *p = malloc(10);
        char **pp = &p+1
        sentinel(p,pp);
===/pre>

      is legal.  pp will be updated with a 1-offset value of p when
      p is moved.  This is useful when wrapping AEEkHeap in another
      heap that exports sentinel-based movable nodes.

Side Effects:
   If the lock count is equal to 0, the node is moveable.
   The heap may move the node during a call to Sentinel().

See Also:
   AEEkHeap_Lock()
   AEEkHeap_Unlock()

========================================================================

Function: AEEkHeap_Compress()

Description:
   Requests a heap compaction.

Prototype:
   void AEEkHeap_Compress(AEEkHeap *pHeap)

Parameters:
   pHeap: pointer to the heap to compress

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   AEEkHeap_Lock()
   AEEkHeap_UnLock()
   AEEkHeap_Init()
   AEEkHeap_Sentinel()

========================================================================

Function: AEEkHeap_NodeInfo()

Description:
   Gets information about this block in the heap.

Prototype:
   int AEEkHeap_NodeInfo(AEEkHeap *pHeap, AEEkHeapNodeInfo *pNodeInfoBuf, 
                         unsigned uNodeInfoBufSize);

Parameters:
   pHeap: pointer to the heap to get node info from
   pNodeInfoBuf: buffer to fill with information about this node
   uNodeInfoBufSize: size of pNodeInfoBuf

Return Value:
   SUCCESS   if pNodeInfoBuf is filled correctly
   EMEMPTR   if pNodeInfoBuf->p is not a valid heap pointer
   EHEAP     if the heap appears corrupt
   EBADPARM  if uNodeInfoBufSize is incorrect

Comments:
   pNodeInfoBuf->p should be set to the node for which the caller 
    wants info

Side Effects:
   None

See Also:
   AEEkHeap_Lock()
   AEEkHeap_UnLock()
   AEEkHeap_Sentinel()
   AEEkHeap_Walk()

========================================================================

Function: AEEkHeap_Walk()

Description:
   Enumerates the blocks in the heap.

Prototype:
   int AEEkHeap_Walk(AEEkHeap *pHeap, AEEkHeapNodeInfo *pWalkBuf, 
                    uint32 uWalkBufSize);

Parameters:
   pHeap: pointer to the heap to walk
   pWalkBuf: buffer to fill with information about the next node
   uWalkBufSize: size of pWalkBuf

Return Value:
   SUCCESS   if pWalkBuf is filled correctly
   EMEMPTR   if pWalkBuf->p is not a valid heap pointer
   EHEAP     if the heap appears corrupt
   EBADPARM  if uWalkBufSize is incorrect
   ENOMEMORY if there are no more nodes

Comments:
   To restart enumeration, set pWalkBuf->p to NULL.

Side Effects:
   None

See Also:
   AEEkHeap_Lock()
   AEEkHeap_UnLock()
   AEEkHeap_Sentinel()
   AEEkHeap_NodeInfo()

========================================================================

Function: AEEkHeap_CheckMemAvail()

Description:
   Checks whether a node of a given size can be allocated.

Prototype:
   boolean AEEkHeap_CheckMemAvail(AEEkHeap *me, uint32 uSize);

Parameters:
   me: pointer to the heap to use for the operation.
   uSize: size of the allocation request.

Return Value:
   TRUE   Sufficient space to allocate this request
   FALSE  Insufficient space to allocate this request

Comments:
   ALLOC_NO_ZMEM may be set. This flag should be cleared to find the correct 
   size of the allocation request.

Side Effects:
   None

See Also:
   None

========================================================================

Function: AEEkHeap_GetMemStats()

Description:
   Gets memory stats of this heap.

Prototype:
   uint32 AEEkHeap_GetMemStats(AEEkHeap *me, uint32 * pdwTotal, uint32 * pdwMax);

Parameters:
   me: pointer to the heap for this operation
   pdwTotal [OUT]: If non-NULL, Filled by this routine containing the total size of the heap
   pdwMax [OUT]: If non-NULL, Filled by this routine containing the largest block available
   

Return Value:
   Total free buffer space

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================
*/

#endif /* #ifndef AEEKHEAP_H */

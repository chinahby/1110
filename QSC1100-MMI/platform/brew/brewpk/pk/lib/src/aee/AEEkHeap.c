/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  AEEkHeap.c

SERVICES:  low-level, instantiable, sbrk-based heap functionality

GENERAL DESCRIPTION:
        Definitions, typedefs, etc. for BREW's lowest heap

        Copyright © 2003-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*
||  Structure of the heap...
||
||  Each heap node looks like this, laid out in contiguous memory:
||        HNode structure (fixed length)
||        Left padding (variable length)
||        Usable memory (variable length)
||        Right padding (variable length)
||        HNodeTrailer structure (fixed length)
||
||  All heap nodes are packed next to one another without gaps, and
||   every heap node is aligned to an address which is a multiple
||   of BASIC_ALIGN -- 4 in the current implementation.
||
||  The beginning of each "section" of a heap node is also aligned to an
||   address which is a multiple of BASIC_ALIGN, except for the Right
||   padding, which may start at any byte address.
||
||  Current sizes (or ranges of sizes) for an allocated heap node:
||
||    HNode           == 20 bytes
||    Left padding    == 0-28 bytes, always a multiple of 4
||    Usable memory   == at least 1 byte, upper limit determined by
||                       available memory
||    Right padding   == 0-31 bytes, usable memory plus right padding
||                       is always a multiple of 4
||    HNodeTrailer    == 8 bytes
||
||  Current sizes (or ranges of sizes) for a free heap node:
||
||    HNode           == 20 bytes
||    Left padding    == 0 bytes
||    Usable memory   == at least 4 bytes, upper limit determined by
||                       available memory, always a multiple of 4
||    Right padding   == 0 bytes
||    HNodeTrailer    == 8 bytes
||
||  Note that the smallest legal heap node, either free or allocated,
||   is 32 bytes, and this provides our upper limits on the size of
||   the Left padding and/or the Right padding.  We will never create
||   a node with a padding section of 32 bytes or greater; in such a
||   case, there is enough memory to split the node and create a new
||   node rather than "wasting" the memory in a padding section.
||
||  The HNode structure of an allocated node:
||
||   void *pLink1     -- the "handle" of the node used when memory
||                       is designated as moveable.
||   void *pLink2     -- TBD, currently always 0.
||   uint32 dwCheck   -- a checkword, chosen such that the XOR of
||                       all of the words of the HNode is equal to
||                       the starting address of the HNode.
||   uint32 dwSize    -- the sum of the sizes of the Left padding,
||                       Usable memory, and Right padding in this node.
||   uint32 dwFlags   -- various flags and bitfields as described
||                       below.
||
||  The HNode structure of a free node:
||
||   void *pLink1     -- the "previous" pointer for managing the
||                       doubly-linked linked list of free nodes.
||   void *pLink2     -- the "next" pointer for managing the
||                       doubly-linked linked list of free nodes.
||   uint32 dwCheck   -- a checkword, chosen such that the XOR of
||                       all of the words of the HNode is equal to
||                       the starting address of the HNode.
||   uint32 dwSize    -- the size of the Usable memory in this node.
||   uint32 dwFlags   -- various flags and bitfields as described
||                       below.
||
||  The HNodeTrailer structure of a node:
||
||   uint32 dwMagic   -- the XOR of the starting address of the
||                       HNodeTrailer structure with HNODE_TRAILER_MAGIC.
||   uint32 dwSize    -- the identical value to the dwSize field in the
||                       HNode structure at the beginning of this node.
||
||  The Left padding of a node is always filled with a single byte,
||   which is the upper byte of the dwFlags field in the HNode
||   structure at the beginning of the node.  Because the dwFlags
||   field is the last word in the HNode structure, and because the
||   size of the Left padding is encoded in the upper byte of the
||   dwFlags field, this means that given a pointer to the Usable
||   memory in a node, we can find the size of the Left padding
||   by examining the single word immediately before the Usable
||   memory.  We will either be fetching the actual dwFlags, or
||   we will be fetching a word whose upper byte matches the
||   upper byte of the actual dwFlags -- in either case, the
||   size of the Left padding is immediately available.
||
||  The Usable memory of a node is not initialized or examined in
||   any way in a free node.  In an allocated node, this memory
||   is owned by the caller who allocated the memory and the heap
||   code will not examine or modify the memory in any way while
||   the node remains allocated.
||
||  The Right padding of a node is not initialized or examined in
||   any way.
*/

#include "AEEComdef.h"
#include "AEEPriv.h"
#include "AEEStdLib.h"
#include "AEEkHeap.h"

/*
||  Some compile-time defaults.  Note that the code is probably
||   not going to work if you simply change BASIC_ALIGN.  The choice
||   of 4-byte alignment for nodes is designed into the heap in some
||   non-obvious ways -- mainly, the idea that 32 bytes of padding will
||   never be necessary, because anyplace that you might consider
||   putting 32 bytes of padding, you could insert an extra node
||   instead.  And by eliminating the need for padding of 32 bytes or
||   more, we can restrict the number of bits needed to represent
||   padding sizes.
||
||  DEFAULT_USER_ALIGN and DEFAULT_DELAY_MAX may be set freely
||   within reasonable ranges.
*/

#define BASIC_ALIGN                 4    /* If you change this, there may be
                                            significant code changes needed.
                                            You probably want to change the
                                            one immediately below. */

#define DEFAULT_USER_ALIGN          8    /* Change this one if you want to
                                            change the "visible" behavior of
                                            the heap -- the default alignment
                                            of allocated blocks.  This can
                                            be overridden on a "per-heap"
                                            basis as well. */

#define DEFAULT_DELAY_MAX           0    /* Number of freed blocks which
                                            should be kept around as
                                            protection against dangling
                                            accesses or double frees.
                                            May be changed at runtime. */

/*
||  Verify at compile time that both BASIC_ALIGN and DEFAULT_USER_ALIGN
||   are positive powers of two >= 4, and that DEFAULT_USER_ALIGN is not
||   less than BASIC_ALIGN.
||
||  Note that the bit expression (N & (N-1)), where N is a positive
||   integer, is only zero when N is a power of two.
*/

#if BASIC_ALIGN < 4
#error Check value of BASIC_ALIGN
#endif

#if (BASIC_ALIGN & (BASIC_ALIGN-1)) != 0
#error Check value of BASIC_ALIGN
#endif

#if DEFAULT_USER_ALIGN < BASIC_ALIGN
#error Check value of DEFAULT_USER_ALIGN
#endif

#if (DEFAULT_USER_ALIGN & (DEFAULT_USER_ALIGN-1)) != 0
#error Check value of DEFAULT_USER_ALIGN
#endif

/*
||  Define some constants related to the heap.
||
||  All of these are guaranteed to be of type "uint32"
||   to guarantee reliable comparisons.
*/

#define BASIC_UALIGN                ((uint32)BASIC_ALIGN)
#define BASIC_UMASK                 (BASIC_UALIGN-1)

#define HNODE_SIZE                  ((uint32)sizeof(HNode))
#define HNODETRAILER_SIZE           ((uint32)sizeof(HNodeTrailer))
#define MEMHEAP_SIZE                ((uint32)sizeof(MemHeap))
#define NODE_OVERHEAD               (HNODE_SIZE+HNODETRAILER_SIZE)
#define MINIMUM_NODE_SIZE           (NODE_OVERHEAD+BASIC_UALIGN)
#define MINIMUM_HEAP_SIZE           (MEMHEAP_SIZE+MINIMUM_NODE_SIZE)
#define MAX_LEGAL_ADDRESS           (~BASIC_UALIGN+1)
#define MAX_ADDRESS_OF_NODE_START   (MAX_LEGAL_ADDRESS-MINIMUM_NODE_SIZE)
#define MAX_ADDRESS_OF_HEAP_START   (MAX_LEGAL_ADDRESS-MINIMUM_HEAP_SIZE)

/*
||  "Forward" definitions of structure typedefs.
*/

typedef struct _HNode HNode;
typedef struct _HNodeTrailer HNodeTrailer;
typedef struct _MemHeap MemHeap;

/*
||  Define the HNode structure.
*/

struct _HNode {
   void *pLink1;
   void *pLink2;
   uint32 dwCheck;
   uint32 dwSize;
#define HNODE_FLAG_FREE        0x00000001u
#define HNODE_FLAG_BUSY        0x00000002u
#define HNODE_FLAG_LEFT        0x00000004u
#define HNODE_FLAG_RIGHT       0x00000008u
#define HNODE_FLAG_DELAY       0x00000010u
#define HNODE_FLAG_RESERVED    0x00000020u
#define HNODE_FLAG_UASHIFT     0x000007C0u
#define HNODE_FLAG_LOCKS       0x0007F800u
#define HNODE_FLAG_EXTRA_RIGHT 0x00F80000u
#define HNODE_FLAG_EXTRA_LEFT  0x07000000u
#define HNODE_FLAG_SIG         0xF0000000u
#define HNODE_FLAG_SIG_MASK    0xF8000000u
   uint32 dwFlags;
};

/*
||  Functions for manipulating HNode flags -- inserting and
||   extracting bitfields.
||
||  Examples of use:
||    dwLocks = HEXT(pNode->dwFlags, HNODE_FLAG_LOCKS)
||
||    pNode->dwFlags = HINS(pNode->dwFlags, HNODE_FLAG_LOCKS, dwLocks)
*/

#define HSCALE(x) ((x)&(~(x)+1))
#define HEXT(f,x) (((f)&(x))/HSCALE(x))
#define HINS(f,x,n) (((f)&~(x))|(((n)*HSCALE(x))&(x)))

/*
||  Maximum number of locks allowed on a node.
*/

#define HNODE_MAX_LOCKS        HEXT(HNODE_FLAG_LOCKS, HNODE_FLAG_LOCKS)

/*
||  Define the HNodeTrailer structure.  The MAGIC number is
||   arbitrary.
*/

struct _HNodeTrailer {
#define HNODE_TRAILER_MAGIC 0x56FC2A2Cu
   uint32 dwMagic;
   uint32 dwSize;
};

/*
||  Define the MemHeap structure.  The MAGIC number is
||   arbitrary.
||
||  The number of buckets here (BUCKET_COUNT) should match
||   the set of possible return values from the h1_bucket()
||   function below.
*/

struct _MemHeap {
#define BUCKET_COUNT 32
   HNode aBuckets[BUCKET_COUNT];
   HNode pDelay[1];
   HNode *pFirstNode;
   HNode *pNodeHighLimit;
   HNode *pNodeLowLimit;
   void (*pCorruptFunc)(MemHeap *, HNode *, uint32);
   void *pWalkLast;
   void *pWalkFixed;
   uint32 uAlign;
   uint32 dwDelayMax;
   uint32 dwDelayCount;
   uint32 dwTotalBytes;
   uint32 dwFreeBytes;
   uint32 dwFreeBlocks;
#define MEMHEAP_MAGIC 0x6487ED51u
   uint32 dwCheck;
};

/*
||  h1_bucket() segregates different memory buffer sizes into buckets
||  in such a way that ordering is preserved.  Basically, if
||                size(BufferA)  >            size(BufferB)
||  then
||      h1_bucket(size(BufferA)) >= h1_bucket(size(BufferB))
||
||  These bucket sizes are chosen somewhat arbitrarily, and the
||   performance of the heap may be tweaked by modifying the
||   bucket mapping function.
*/

static __inline uint32 h1_bucket(uint32 value)
{
#define BINSIZE0  (0x00000004u)
#define BINSIZE1  (0x00000008u)
#define BINSIZE2  (0x0000000Cu)
#define BINSIZE3  (0x00000010u)
#define BINSIZE4  (0x00000018u)
#define BINSIZE5  (0x00000020u)
#define BINSIZE6  (0x00000030u)
#define BINSIZE7  (0x00000040u)
#define BINSIZE8  (0x00000060u)
#define BINSIZE9  (0x00000080u)
#define BINSIZE10 (0x000000C0u)
#define BINSIZE11 (0x00000100u)
#define BINSIZE12 (0x00000180u)
#define BINSIZE13 (0x00000200u)
#define BINSIZE14 (0x00000300u)
#define BINSIZE15 (0x00000400u)
#define BINSIZE16 (0x00000600u)
#define BINSIZE17 (0x00000800u)
#define BINSIZE18 (0x00000C00u)
#define BINSIZE19 (0x00001000u)
#define BINSIZE20 (0x00001800u)
#define BINSIZE21 (0x00002000u)
#define BINSIZE22 (0x00003000u)
#define BINSIZE23 (0x00004000u)
#define BINSIZE24 (0x00006000u)
#define BINSIZE25 (0x00008000u)
#define BINSIZE26 (0x0000C000u)
#define BINSIZE27 (0x00010000u)
#define BINSIZE28 (0x00018000u)
#define BINSIZE29 (0x00020000u)
#define BINSIZE30 (0x00030000u)
#define BINSIZE31 (0xFFFFFFFFu)
   unsigned long bin;
   if (value <= BINSIZE15)
   {
      if (value <= BINSIZE7)
      {
         if (value <= BINSIZE3)
         {
            if (value <= BINSIZE1)
            {
               if (value <= BINSIZE0)
               {
                  bin = 0;
               }
               else
               {
                  bin = 1;
               }
            }
            else // > BINSIZE1
            {
               if (value <= BINSIZE2)
               {
                  bin = 2;
               }
               else
               {
                  bin = 3;
               }
            }
         }
         else // > BINSIZE3
         {
            if (value <= BINSIZE5)
            {
               if (value <= BINSIZE4)
               {
                  bin = 4;
               }
               else
               {
                  bin = 5;
               }
            }
            else // > BINSIZE5
            {
               if (value <= BINSIZE6)
               {
                  bin = 6;
               }
               else
               {
                  bin = 7;
               }
            }
         }
      }
      else // value > BINSIZE7
      {
         if (value <= BINSIZE11)
         {
            if (value <= BINSIZE9)
            {
               if (value <= BINSIZE8)
               {
                  bin = 8;
               }
               else
               {
                  bin = 9;
               }
            }
            else // > BINSIZE9
            {
               if (value <= BINSIZE10)
               {
                  bin = 10;
               }
               else
               {
                  bin = 11;
               }
            }
         }
         else // > BINSIZE11
         {
            if (value <= BINSIZE13)
            {
               if (value <= BINSIZE12)
               {
                  bin = 12;
               }
               else
               {
                  bin = 13;
               }
            }
            else // > BINSIZE13
            {
               if (value <= BINSIZE14)
               {
                  bin = 14;
               }
               else
               {
                  bin = 15;
               }
            }
         }
      }
   }
   else // > BINSIZE15
   {
      if (value <= BINSIZE23)
      {
         if (value <= BINSIZE19)
         {
            if (value <= BINSIZE17)
            {
               if (value <= BINSIZE16)
               {
                  bin = 16;
               }
               else
               {
                  bin = 17;
               }
            }
            else  // > BINSIZE17
            {
               if (value <= BINSIZE18)
               {
                  bin = 18;
               }
               else
               {
                  bin = 19;
               }
            }
         }
         else // > BINSIZE19
         {
            if (value <= BINSIZE21)
            {
               if (value <= BINSIZE20)
               {
                  bin = 20;
               }
               else
               {
                  bin = 21;
               }
            }
            else // > BINSIZE21
            {
               if (value <= BINSIZE22)
               {
                  bin = 22;
               }
               else
               {
                  bin = 23;
               }
            }
         }
      }
      else // > BINSIZE23
      {
         if (value <= BINSIZE27)
         {
            if (value <= BINSIZE25)
            {
               if (value <= BINSIZE24)
               {
                  bin = 24;
               }
               else
               {
                  bin = 25;
               }
            }
            else // > BINSIZE25
            {
               if (value <= BINSIZE26)
               {
                  bin = 26;
               }
               else
               {
                  bin = 27;
               }
            }
         }
         else // > BINSIZE27
         {
            if (value <= BINSIZE29)
            {
               if (value <= BINSIZE28)
               {
                  bin = 28;
               }
               else
               {
                  bin = 29;
               }
            }
            else // > BINSIZE29
            {
               if (value <= BINSIZE30)
               {
                  bin = 30;
               }
               else
               {
                  bin = 31;
               }
            }
         }
      }
   }
   return bin;
}

/*
||  h1_create_node() creates a heap node.  No assumption is made
||   about what the memory contained prior to the call.  In particular,
||   if a node existed there before, it will be overwritten.
||
||  The HNode and HNodeTrailer structures are completely filled in,
||   and the left padding is filled in if needed.
||
||  It is up to the caller to make sure that the values passed in
||   for pLink1, dwFlags, dwExtraLeft, and dwExtraRight are
||   consistent, and that the entire range of memory designated
||   by (pMemory,dwSize) is valid, reserved, R/W memory.
||
||  This function also does double duty to initialize the bucket
||   and delayed-free linked lists.  If the dwSize parameter is
||   passed in as zero (0), this function initializes an HNode
||   structure only, with pLink1 and pLink2 both pointing at
||   itself.  (An "empty" circular doubly-linked list.)
*/

static __inline void h1_create_node(MemHeap *pHeap,
                                    void *pMemory,
                                    void *pLink1,
                                    uint32 dwSize,
                                    uint32 dwFlags,
                                    uint32 dwExtraLeft,
                                    uint32 dwExtraRight)
{
   uint32 dwCheck;
   HNode *pNode;

   pNode = (HNode *)pMemory;

   dwCheck = (uint32)pNode;

   if (dwSize) {
      HNodeTrailer *pNodeTrailer;

      pNodeTrailer =
         (HNodeTrailer *)
         ((char *)pNode + HNODE_SIZE + dwSize);

      dwFlags = HINS(dwFlags, HNODE_FLAG_EXTRA_LEFT, dwExtraLeft/BASIC_UALIGN);
      dwFlags = HINS(dwFlags, HNODE_FLAG_EXTRA_RIGHT, dwExtraRight);
      pNodeTrailer->dwMagic = ((uint32)pNodeTrailer ^ HNODE_TRAILER_MAGIC);
      pNodeTrailer->dwSize = dwSize;
      dwCheck ^= dwSize;
      pNode->pLink1 = pLink1;
      dwCheck ^= (uint32)pLink1;
      pNode->pLink2 = 0;

      if (dwExtraLeft) {
         uint32 dwTemp;
         uint32 i;

         /* Replicate top byte of flags throughout a word */
         dwTemp = dwFlags;
         dwTemp /= ((~(uint32)0)/256)+1;  /* divide   by 0x01000000 */
         dwTemp *= ((~(uint32)0)/255);    /* multiply by 0x01010101 */
         for (i=0; i<dwExtraLeft; i+=sizeof(uint32))
            *(uint32 *)((char *)pNode + HNODE_SIZE + i) = dwTemp;
      }
   } else {
      pNode->pLink1 = pNode;
      pNode->pLink2 = pNode;
   }

   pNode->dwCheck = dwCheck ^ dwFlags;
   pNode->dwSize = dwSize;
   pNode->dwFlags = dwFlags;
}

/*
||  Function h1_alignment_slop() returns the number of bytes at
||   the left side of an HNode which would be wasted if the node
||   was used to satisfy a request for memory aligned to a
||   uAlign-byte boundary.
||
||  Arguments are not checked for validity.  pNode must point at
||   a valid HNode.  uAlign must be a power of 2.  Note that
||   if uAlign <= BASIC_UALIGN, this function should always
||   return zero, assuming that the pNode pointer is valid.
||   Thus, it probably makes sense to have an optimized code path
||   which avoids this computation whenever uAlign <= BASIC_UALIGN.
*/

static __inline uint32 h1_alignment_slop(HNode *pNode, uint32 uAlign)
{
   return (0-(uint32)((char *)pNode+sizeof(HNode))) & (uAlign-1);
}

/*
||  Function h1_rw_access() is intended to check that an entire
||   range of memory is R/W accessible.  All heap structures and
||   blocks of memory allocated from the heap are of course
||   R/W accessible.  By checking for R/W accessibility, we
||   can avoid data access exceptions and return graceful
||   errors in cases of heap corruption or in cases where a
||   caller passes in an invalid memory pointer.
||
||  This function MUST return SUCCESS if the entire range
||   provided by the caller (dwLen bytes beginning at pBuf)
||   is non-empty and R/W accessible.  It SHOULD return
||   EMEMPTR if the arguments describe a potentially valid
||   non-empty memory range, but the entire range is not
||   R/W accessible.  If the arguments do not describe a
||   potentially valid non-empty memory range (pBuf is NULL,
||   dwLen is zero, or overflow occurs when adding
||   dwLen to pBuf), it SHOULD return EBADPARM.
||
||  Code which calls this function should not rely on a
||   specific error return code.  The error code may be
||   used for diagnostic purposes, or to present an
||   informative error message, but flow decisions should be
||   based only on whether the return value is SUCCESS or not.
||
||  Until full memory protection is present, this function
||   remains a stub which always returns SUCCESS.
||
||  If this function grows beyond a few lines of code, it
||   should have the "inline" qualifier removed.
*/

static __inline int h1_rw_access(void *pBuf, uint32 dwLen)
{
   return SUCCESS;
}

/*
||  Function h1_verify_heap() is intended to check that a
||   user-supplied MemHeap pointer is valid.
||
||  This function MUST return SUCCESS if the pointer provided
||   by the caller points to a valid and uncorrupted MemHeap
||   structure.  It SHOULD return EHEAP if the pointer appears
||   to be valid but does not point at an uncorrupted MemHeap
||   structure.  It SHOULD return EBADPARM if the pointer could
||   not point to a valid MemHeap structure.  It SHOULD return
||   EMEMPTR if the pointer appears to be otherwise valid, but
||   does not point at accessible memory.
||
||  Code which calls this function should not rely on a
||   specific error return code.  The error code may be
||   used for diagnostic purposes, or to present an
||   informative error message, but flow decisions should be
||   based only on whether the return value is SUCCESS or not.
*/

static __inline int h1_verify_heap(MemHeap *pHeap)
{
   int nRet;

   /* Check that pointer is non-NULL and aligned correctly */

   if (pHeap == 0)
      return EBADPARM;
   if ((uint32)pHeap & BASIC_UMASK)
      return EBADPARM;

   /* Check that pointer points at valid memory */

   if ((nRet = h1_rw_access(pHeap, sizeof(MemHeap))) != SUCCESS)
      return nRet;

   /* Check that the "checkword" of the structure is valid */

   if (pHeap->dwCheck != ((uint32)pHeap ^ MEMHEAP_MAGIC))
      return EHEAP;

   return SUCCESS;
}

/*
||  Function h1_node_is_free() checks whether a heap node is
||   currently free.  By "free", we mean that it is:  available
||   for allocation AND currently linked into the free list.
||
||  This function returns TRUE if the node is currently
||   available for allocation and linked into the free list.
||   It returns FALSE otherwise.
||
||  This function does not check its argument for validity.
||   If it is passed an invalid node pointer, it may crash
||   or return a useless value.
*/

static __inline boolean h1_node_is_free(HNode *pNode)
{
   return (pNode->dwFlags & HNODE_FLAG_FREE) ? TRUE : FALSE;
}

/*
||  Function h1_node_is_allocated() checks whether a heap node is
||   currently allocated.  A node is allocated if its memory
||   has been previously provided to a caller as a valid
||   allocation, and if the memory has not yet been freed.
||
||  This function returns TRUE if the node is currently
||   allocated for the use of a caller.
||   It returns FALSE otherwise.
||
||  This function does not check its argument for validity.
||   If it is passed an invalid node pointer, it may crash
||   or return a useless value.
*/

static __inline boolean h1_node_is_allocated(HNode *pNode)
{
   return (pNode->dwFlags & HNODE_FLAG_BUSY) ? TRUE : FALSE;
}

static __inline uint32 h1_node_get_ualign(HNode *pNode)
{
   return ((uint32)1) << HEXT(pNode->dwFlags, HNODE_FLAG_UASHIFT);
}

static __inline void h1_node_set_ualign(HNode *pNode, uint32 uAlign)
{
   /* Lookup table for converting 2^n -> n */
   /* If N=2^n, then n = xlate[N%37] */
   static const uint32 xlate[37] = {
      36, 0, 1,26, 2,23,27,32,
       3,16,24,30,28,11,33,13,
       4, 7,17,35,25,22,31,15,
      29,10,12, 6,34,21,14, 9,
       5,20, 8,19,18
   };
   uint32 dwFlagsNew;
   uint32 dwTemp;

   dwTemp = pNode->dwFlags;
   dwFlagsNew = HINS(dwTemp, HNODE_FLAG_UASHIFT, xlate[uAlign%37]);
   dwTemp ^= pNode->dwCheck;
   dwTemp ^= dwFlagsNew;
   pNode->dwFlags = dwFlagsNew;
   pNode->dwCheck = dwTemp;
}

/*
||  Function h1_node_get_lock_count() returns the number of
||   currently existing locks on a heap node.  Note that
||   this function is only useful when applied to a node
||   which is currently allocated.
||
||  This function does not check its argument for validity.
||   If it is passed an invalid node pointer, it may crash
||   or return a useless value.
*/

static __inline uint32 h1_node_get_lock_count(HNode *pNode)
{
   return HEXT(pNode->dwFlags, HNODE_FLAG_LOCKS);
}

/*
||  Function h1_node_set_lock_count() sets the number of
||   currently existing locks on a heap node.  Note that
||   this function is only valid when applied to a node
||   which is currently allocated (or in the process of
||   being allocated).
||
||  The lock count is "clipped" at HNODE_MAX_LOCKS.  Any
||   attempt to set the lock count higher than HNODE_MAX_LOCKS
||   will result in the lock count being set to HNODE_MAX_LOCKS.
||
||  This function does not check its argument for validity.
||   If it is passed an invalid node pointer, it may crash
||   or return a useless value.
*/

static __inline void h1_node_set_lock_count(HNode *pNode, uint32 dwCnt)
{
   uint32 dwFlagsNew;
   uint32 dwTemp;

   if (dwCnt > HNODE_MAX_LOCKS)
      dwCnt = HNODE_MAX_LOCKS;
   dwTemp = pNode->dwFlags;
   dwFlagsNew = HINS(dwTemp, HNODE_FLAG_LOCKS, dwCnt);
   dwTemp ^= pNode->dwCheck;
   dwTemp ^= dwFlagsNew;
   pNode->dwFlags = dwFlagsNew;
   pNode->dwCheck = dwTemp;
}

/*
||  Function h1_node_get_extra_bytes_left() returns the number of
||   "wasted" bytes between the HNode structure and the block
||   allocated for the user.  Note that this function is only
||   useful when applied to a node which is currently allocated.
||
||  This function does not check its argument for validity.
||   If it is passed an invalid node pointer, it may crash
||   or return a useless value.
*/

static __inline uint32 h1_node_get_extra_bytes_left(HNode *pNode)
{
   return HEXT(pNode->dwFlags, HNODE_FLAG_EXTRA_LEFT)*BASIC_UALIGN;
}

/*
||  Function h1_node_get_extra_bytes_right() returns the number of
||   "wasted" bytes between the block allocated for the user and the
||   HNodeTrailer structure.  Note that this function is only
||   useful when applied to a node which is currently allocated.
||
||  This function does not check its argument for validity.
||   If it is passed an invalid node pointer, it may crash
||   or return a useless value.
*/

static __inline uint32 h1_node_get_extra_bytes_right(HNode *pNode)
{
   return HEXT(pNode->dwFlags, HNODE_FLAG_EXTRA_RIGHT);
}

static void h1_add_to_free_list(MemHeap *pHeap, HNode *pNode)
{
   uint32 dwFlags;
   HNode *pNext;
   HNode *pPrev;

   dwFlags = pNode->dwFlags;

   if (!(dwFlags & HNODE_FLAG_FREE)) {
      pPrev = &pHeap->aBuckets[h1_bucket(pNode->dwSize)];
      pNext = (HNode *)(pPrev->pLink2);
      pNode->dwCheck ^= (uint32)pPrev ^ (uint32)pNext ^ HNODE_FLAG_FREE;
      pNode->dwFlags = dwFlags ^ HNODE_FLAG_FREE;
      pNode->pLink1 = (void *)pPrev;
      pNode->pLink2 = (void *)pNext;
      pPrev->dwCheck ^= (uint32)pNode ^ (uint32)pNext;
      pPrev->pLink2 = (void *)pNode;
      pNext->dwCheck ^= (uint32)pNode ^ (uint32)pPrev;
      pNext->pLink1 = (void *)pNode;
      pHeap->dwFreeBytes += pNode->dwSize;
      pHeap->dwFreeBlocks += 1;
   }
}

static void h1_remove_from_free_list(MemHeap *pHeap, HNode *pNode)
{
   uint32 dwFlags;
   HNode *pNext;
   HNode *pPrev;

   dwFlags = pNode->dwFlags;

   if (dwFlags & HNODE_FLAG_FREE) {
      pPrev = (HNode *)(pNode->pLink1);
      pNext = (HNode *)(pNode->pLink2);
      pPrev->dwCheck ^= (uint32)pNode ^ (uint32)pNext;
      pPrev->pLink2 = (void *)pNext;
      pNext->dwCheck ^= (uint32)pNode ^ (uint32)pPrev;
      pNext->pLink1 = (void *)pPrev;
      pNode->dwCheck ^= (uint32)pPrev ^ (uint32)pNext ^ HNODE_FLAG_FREE;
      pNode->dwFlags = dwFlags ^ HNODE_FLAG_FREE;
      pNode->pLink1 = 0;
      pNode->pLink2 = 0;
      pHeap->dwFreeBytes -= pNode->dwSize;
      pHeap->dwFreeBlocks -= 1;
   }
}

static void h1_remove_from_delay_list(MemHeap *pHeap, HNode *pNode)
{
   uint32 dwFlags;
   HNode *pNext;
   HNode *pPrev;

   dwFlags = pNode->dwFlags;

   if (dwFlags & HNODE_FLAG_DELAY) {
      pPrev = (HNode *)(pNode->pLink1);
      pNext = (HNode *)(pNode->pLink2);
      pPrev->dwCheck ^= (uint32)pNode ^ (uint32)pNext;
      pPrev->pLink2 = (void *)pNext;
      pNext->dwCheck ^= (uint32)pNode ^ (uint32)pPrev;
      pNext->pLink1 = (void *)pPrev;
      pNode->dwCheck ^= (uint32)pPrev ^ (uint32)pNext ^ HNODE_FLAG_DELAY;
      pNode->dwFlags = dwFlags ^ HNODE_FLAG_DELAY;
      pNode->pLink1 = 0;
      pNode->pLink2 = 0;
      pHeap->dwDelayCount -= 1;
   }
}

static void h1_add_to_delay_list(MemHeap *pHeap, HNode *pNode)
{
   uint32 dwFlags;
   HNode *pNext;
   HNode *pPrev;

   dwFlags = pNode->dwFlags;

   if (dwFlags & HNODE_FLAG_BUSY) {
      if (!(dwFlags & HNODE_FLAG_DELAY)) {
         pPrev = pHeap->pDelay;
         pNext = (HNode *)(pPrev->pLink2);
         pNode->dwCheck ^=
            (uint32)(pNode->pLink1) ^
            (uint32)(pNode->pLink2) ^
            (uint32)pPrev ^ (uint32)pNext ^ (HNODE_FLAG_DELAY | HNODE_FLAG_BUSY);
         pNode->dwFlags = dwFlags ^ (HNODE_FLAG_DELAY | HNODE_FLAG_BUSY);
         pNode->pLink1 = (void *)pPrev;
         pNode->pLink2 = (void *)pNext;
         pPrev->dwCheck ^= (uint32)pNode ^ (uint32)pNext;
         pPrev->pLink2 = (void *)pNode;
         pNext->dwCheck ^= (uint32)pNode ^ (uint32)pPrev;
         pNext->pLink1 = (void *)pNode;
         pHeap->dwDelayCount += 1;
      }
   }
}

/*
|| Arguments for iPosition in the next function:
||
|| H1_ALLOC_LEFT asks for the node to be placed as far left in
||  the range as possible.  (Used with malloc)
||
|| H1_ALLOC_RIGHT asks for the node to be placed as far right in
||  the range as possible.  (Used with moveable nodes)
||
|| H1_ALLOC_PRESERVE asks for the node to be placed in the
||  same location that it is right now.  (Used with realloc)
*/

#define H1_ALLOC_LEFT     0
#define H1_ALLOC_RIGHT    1
#define H1_ALLOC_PRESERVE 2

static __inline int h1_alloc_from_range(MemHeap *pHeap,
                                        void *pRange,
                                        uint32 dwRangeLen,
                                        uint32 dwSize,
                                        uint32 dwAlign,
                                        uint32 dwFlags,
                                        uint32 dwLockCount,
                                        HNode **ppNode,
                                        int iPosition,
                                        boolean bZero,
                                        void *pSrcBuf,
                                        uint32 dwSrcLen,
                                        HNode *pNodeToFree1,
                                        HNode *pNodeToFree2,
                                        void **pSentinel)
{
   uint32 dwExtraRight;
   uint32 dwExtraLeft;
   void *pRangeEnd;
   HNode *pNode;
   void *pBuf;
   void *pEnd;

   pRangeEnd = (void *)((char *)pRange+dwRangeLen);

   if (iPosition == H1_ALLOC_PRESERVE) {
      if (pSrcBuf == 0 ||
          ((uint32)pSrcBuf & (dwAlign-1)) != 0)
         return ENOMEMORY;
      pBuf = pSrcBuf;
      pEnd = (void *)(((uint32)pBuf+dwSize+BASIC_UMASK)&~BASIC_UMASK);
      pEnd = (void *)((char *)pEnd+HNODETRAILER_SIZE);
   } else if (iPosition == H1_ALLOC_RIGHT) {
      /* Right aligned within block */
      pBuf = (void *)((char *)pRangeEnd-HNODETRAILER_SIZE-dwSize);
      pBuf = (void *)((uint32)pBuf&~(dwAlign-1));
      pEnd = (void *)(((uint32)pBuf+dwSize+BASIC_UMASK)&~BASIC_UMASK);
      pEnd = (void *)((char *)pEnd+HNODETRAILER_SIZE);
   } else {
      /* Left aligned within block */
      pBuf = (void *)((char *)pRange+HNODE_SIZE);
      pBuf = (void *)(((uint32)pBuf+(dwAlign-1))&~(dwAlign-1));
      pEnd = (void *)(((uint32)pBuf+dwSize+BASIC_UMASK)&~BASIC_UMASK);
      pEnd = (void *)((char *)pEnd+HNODETRAILER_SIZE);
   }

   pNode = (HNode *)((char *)pBuf-HNODE_SIZE);

   if (!((char *)pEnd <= (char *)pRangeEnd &&
         (char *)pBuf < (char *)pEnd &&
         (char *)pNode < (char *)pBuf &&
         (char *)pRange <= (char *)pNode))
      return ENOMEMORY;

   if (pNodeToFree1)
      h1_remove_from_free_list(pHeap, pNodeToFree1);
   if (pNodeToFree2)
      h1_remove_from_free_list(pHeap, pNodeToFree2);

   if (pSrcBuf == 0) {
      if (bZero) memset(pBuf, 0, dwSize);
   } else if (dwSrcLen < dwSize) {
      if (pSrcBuf != pBuf) memmove(pBuf, pSrcBuf, dwSrcLen);
      if (bZero) memset((char *)pBuf+dwSrcLen, 0, dwSize-dwSrcLen);
   } else {
      if (pSrcBuf != pBuf) memmove(pBuf, pSrcBuf, dwSize);
   }

   dwExtraLeft = (char *)pNode - (char *)pRange;
   dwExtraRight = (char *)pRangeEnd - (char *)pEnd;
   if (dwSize & BASIC_UMASK)
      dwExtraRight += BASIC_UALIGN - (dwSize & BASIC_UMASK);

   if (dwExtraLeft >= MINIMUM_NODE_SIZE) {
      h1_create_node(pHeap,
                     pRange, 0, dwExtraLeft - NODE_OVERHEAD,
                     HNODE_FLAG_SIG | (dwFlags & HNODE_FLAG_LEFT) | HNODE_FLAG_RIGHT,
                     0, 0);
      h1_add_to_free_list(pHeap, (HNode *)pRange);
      dwFlags |= HNODE_FLAG_LEFT;
      dwExtraLeft = 0;
   } else {
      pNode = (HNode *)pRange;
   }

   if (dwExtraRight >= MINIMUM_NODE_SIZE) {
      h1_create_node(pHeap,
                     pEnd, 0, (dwExtraRight&~BASIC_UMASK) - NODE_OVERHEAD,
                     HNODE_FLAG_SIG | (dwFlags & HNODE_FLAG_RIGHT) | HNODE_FLAG_LEFT,
                     0, 0);
      h1_add_to_free_list(pHeap, (HNode *)pEnd);
      dwFlags |= HNODE_FLAG_RIGHT;
      dwExtraRight &= BASIC_UMASK;
   } else {
      pEnd = pRangeEnd;
   }

   h1_create_node(pHeap,
                  pNode, pSentinel, ((char *)pEnd - (char *)pNode) - NODE_OVERHEAD,
                  HNODE_FLAG_SIG | dwFlags,
                  dwExtraLeft, dwExtraRight);

   h1_node_set_ualign(pNode, dwAlign);
   h1_node_set_lock_count(pNode, dwLockCount);

   *ppNode = pNode;

   if (pSentinel)
      *pSentinel = (void *)((uint32)(*pSentinel) + (uint32)pBuf - (uint32)pSrcBuf);

   if (pHeap->pWalkFixed) {
      if ((char *)(pHeap->pWalkFixed) >= (char *)pRange) {
         if ((char *)(pHeap->pWalkFixed) < (char *)pRangeEnd) {
            if (pEnd == pRangeEnd) {
               /* No "right" node was created */
               pHeap->pWalkFixed = pNode+1;
            } else {
               pHeap->pWalkFixed = ((HNode *)pEnd)+1;
            }
         }
      }
   }

   return SUCCESS;
}

/*
||  h1_verify_node(pHeap, pNode) -- verify that pNode
||   points at a valid HNode structure.  Do everything
||   possible to verify within the node, but do
||   not look outside the node.
||
||  Return SUCCESS if pNode appears to be proper.
||
||  Return EMEMPTR if pNode appears to be improper.
||   In this case, also call out to the function
||   pointed to by pHeap->pCorruptFunc.  (But only
||   if pHeap itself appears to point at a verified
||   valid MemHeap structure.)
||   If unable to call out to pHeap->pCorruptFunc,
||   use DBGPRINTF to output a warning message.
*/

static int h1_verify_node(MemHeap *pHeap, HNode *pNode)
{
   HNodeTrailer *pNodeTrailer;
   void *pEndMinimum;
   void *pEndActual;
   void *pLink1;
   void *pLink2;
   uint32 dwCheck;
   uint32 dwSize;
   uint32 dwFlags;
   uint32 dwExtra;
   uint32 eno;

#define VN_ASSERT(e,x) do{if(!(x)){eno=e;goto vn_error;}}while(0)

   VN_ASSERT(1, pNode != 0);
   VN_ASSERT(2, ((uint32)pNode & BASIC_UMASK) == 0);
   pEndMinimum = (char *)pNode + MINIMUM_NODE_SIZE;
   VN_ASSERT(3, (uint32)pEndMinimum > (uint32)pNode);
   VN_ASSERT(4, h1_rw_access(pNode, MINIMUM_NODE_SIZE) == SUCCESS);
   dwCheck = pNode->dwCheck;
   pLink1 = pNode->pLink1;
   dwCheck ^= (uint32)pNode;
   pLink2 = pNode->pLink2;
   dwCheck ^= (uint32)pLink1;
   dwSize = pNode->dwSize;
   dwCheck ^= (uint32)pLink2;
   dwFlags = pNode->dwFlags;
   dwCheck ^= dwSize;
   VN_ASSERT(5, dwCheck == dwFlags);

   /* Header checksum is okay. */

   dwExtra = dwSize - BASIC_UALIGN;

   if (dwExtra) {
      VN_ASSERT(6, (dwSize & BASIC_UMASK) == 0);
      /* Check that (MINIMUM_NODE_SIZE+dwExtra) does not overflow... */
      /*              (Note that ~dwExtra == 0xFFFFFFFF-dwExtra ...) */
      VN_ASSERT(7, ~dwExtra >= MINIMUM_NODE_SIZE);
      pEndActual = (char *)pEndMinimum + dwExtra;
      VN_ASSERT(8, (uint32)pEndActual > (uint32)pNode);
      VN_ASSERT(9, h1_rw_access(pEndMinimum, dwExtra) == SUCCESS);
      pNodeTrailer = (HNodeTrailer *)((char *)pEndActual - sizeof(HNodeTrailer));
   } else {
      pNodeTrailer = (HNodeTrailer *)((char *)pEndMinimum - sizeof(HNodeTrailer));
   }

   VN_ASSERT(10, pNodeTrailer->dwMagic == ((uint32)pNodeTrailer ^ HNODE_TRAILER_MAGIC));
   VN_ASSERT(11, pNodeTrailer->dwSize == dwSize);

   /* Trailer is as expected. */

   /* Check Link1, Link2, and Flags for validity */

   VN_ASSERT(12, (dwFlags & HNODE_FLAG_SIG_MASK) == HNODE_FLAG_SIG);
   if (dwFlags & HNODE_FLAG_FREE) {
      VN_ASSERT(13, (dwFlags & (HNODE_FLAG_BUSY |
                                HNODE_FLAG_RESERVED |
                                HNODE_FLAG_LOCKS |
                                HNODE_FLAG_EXTRA_RIGHT |
                                HNODE_FLAG_EXTRA_LEFT)) == 0);
      VN_ASSERT(14, pLink1 != 0);  /* Maybe check for pLink1 too high */
      VN_ASSERT(15, ((uint32)pLink1 & BASIC_UMASK) == 0);
      VN_ASSERT(16, pLink2 != 0);  /* Maybe check for pLink2 too high */
      VN_ASSERT(17, ((uint32)pLink2 & BASIC_UMASK) == 0);
   } else if (dwFlags & HNODE_FLAG_BUSY) {
      VN_ASSERT(18, (dwFlags & (HNODE_FLAG_RESERVED)) == 0);
      dwExtra =
         HEXT(dwFlags, HNODE_FLAG_EXTRA_LEFT) * 4 +
         HEXT(dwFlags, HNODE_FLAG_EXTRA_RIGHT);
      VN_ASSERT(19, dwExtra < dwSize);
      VN_ASSERT(20, ((uint32)pLink1 & BASIC_UMASK) == 0);
      VN_ASSERT(21, pLink2 == 0);
   } else if (dwFlags & HNODE_FLAG_DELAY) {
      VN_ASSERT(22, (dwFlags & (HNODE_FLAG_RESERVED)) == 0);
      dwExtra =
         HEXT(dwFlags, HNODE_FLAG_EXTRA_LEFT) * 4 +
         HEXT(dwFlags, HNODE_FLAG_EXTRA_RIGHT);
      VN_ASSERT(23, dwExtra < dwSize);
      if (pLink1 == 0 && pLink2 == 0) {
         /* It's okay.  Looks like one of our boundary blocks */
      } else {
         VN_ASSERT(24, pLink1 != 0);  /* Maybe check for pLink1 too high */
         VN_ASSERT(25, ((uint32)pLink1 & BASIC_UMASK) == 0);
         VN_ASSERT(26, pLink2 != 0);  /* Maybe check for pLink2 too high */
         VN_ASSERT(27, ((uint32)pLink2 & BASIC_UMASK) == 0);
      }
   } else {
      VN_ASSERT(28, 0);
   }

   return SUCCESS;

 vn_error:
   if (h1_verify_heap(pHeap) == SUCCESS &&
       pHeap->pCorruptFunc != 0) {
      (*pHeap->pCorruptFunc)(pHeap, pNode, eno);
   } else {
      DBGPRINTF("h1: invalid HNode 0x%X, reason %u",
                (unsigned)pNode, (unsigned)eno);
   }

   return EMEMPTR;
}

/*
||  Given a pointer to a memory block, return the address of the
||   HNode structure which "controls" the block, or NULL if there
||   is a problem.
*/

static __inline HNode *h1_ptr_to_verified_node(MemHeap *pHeap, void *p)
{
   HNodeTrailer *pNodeTrailer;
   uint32 dwLeft;
   HNode *pNode;

   /* If the pointer isn't aligned, it's bogus */

   if ((uint32)p & BASIC_UMASK)
      return 0;

   /* Get the provisional node pointer */

   pNode = (HNode *)p - 1;

   /* If the provisional node pointer is out of range,
      there's no hope */

   if (pNode > pHeap->pNodeHighLimit)
      return 0;
   if (pNode < pHeap->pNodeLowLimit)
      return 0;

   /* Fetch the size of the left padding */

   dwLeft = h1_node_get_extra_bytes_left(pNode);

   /* If there's left padding, our provisional node pointer
      was wrong, so adjust and recheck */

   if (dwLeft) {
      pNode = (HNode *)((char *)pNode - dwLeft);
      if (pNode < pHeap->pNodeLowLimit)
         return 0;
   }

   /* If the checkword fails, we're no good */

   if (((uint32)(pNode) ^
        (uint32)(pNode->pLink1) ^
        (uint32)(pNode->pLink2) ^
        (uint32)(pNode->dwSize) ^
        (uint32)(pNode->dwFlags)) != pNode->dwCheck)
      return 0;

   /* Checkword passed.  Now check the node trailer */

   pNodeTrailer = (HNodeTrailer *)((char *)pNode +
                                   HNODE_SIZE +
                                   pNode->dwSize);

   if (pNodeTrailer->dwSize != pNode->dwSize)
      return 0;

   if (pNodeTrailer->dwMagic !=
       ((uint32)pNodeTrailer ^ HNODE_TRAILER_MAGIC))
      return 0;

   return pNode;
}

/*
||  Given a pointer to an HNode, return a pointer to its "left"
||   HNode (the HNode which is contiguous to the original HNode
||   at a lower address).  Return NULL if no such left node
||   exists, which may or may not be an error.
*/

static __inline HNode *h1_left_verified_node(MemHeap *pHeap, HNode *me)
{
   HNodeTrailer *pNodeTrailer;
   HNode *pNode;

   if (!(me->dwFlags & HNODE_FLAG_LEFT))
      return 0;

   pNode =
      (HNode *)
      ((char *)me -
       ((HNodeTrailer *)me)[-1].dwSize -
       NODE_OVERHEAD);

   if (pNode > pHeap->pNodeHighLimit)
      return 0;
   if (pNode < pHeap->pNodeLowLimit)
      return 0;

   /* If the checkword fails, we're no good */

   if (((uint32)(pNode) ^
        (uint32)(pNode->pLink1) ^
        (uint32)(pNode->pLink2) ^
        (uint32)(pNode->dwSize) ^
        (uint32)(pNode->dwFlags)) != pNode->dwCheck)
      return 0;

   /* Checkword passed.  Now check the node trailer */

   pNodeTrailer = (HNodeTrailer *)((char *)pNode +
                                   HNODE_SIZE +
                                   pNode->dwSize);

   if (pNodeTrailer->dwSize != pNode->dwSize)
      return 0;

   if (pNodeTrailer->dwMagic !=
       ((uint32)pNodeTrailer ^ HNODE_TRAILER_MAGIC))
      return 0;

   return pNode;
}

/*
||  Given a pointer to an HNode, return a pointer to its "right"
||   HNode (the HNode which is contiguous to the original HNode
||   at a higher address).  Return NULL if no such right node
||   exists, which may or may not be an error.
*/

static __inline HNode *h1_right_verified_node(MemHeap *pHeap, HNode *me)
{
   HNodeTrailer *pNodeTrailer;
   HNode *pNode;

   if (!(me->dwFlags & HNODE_FLAG_RIGHT))
      return 0;

   pNode =
      (HNode *)
      ((char *)me +
       me->dwSize +
       NODE_OVERHEAD);

   if (pNode > pHeap->pNodeHighLimit)
      return 0;
   if (pNode < pHeap->pNodeLowLimit)
      return 0;

   /* If the checkword fails, we're no good */

   if (((uint32)(pNode) ^
        (uint32)(pNode->pLink1) ^
        (uint32)(pNode->pLink2) ^
        (uint32)(pNode->dwSize) ^
        (uint32)(pNode->dwFlags)) != pNode->dwCheck)
      return 0;

   /* Checkword passed.  Now check the node trailer */

   pNodeTrailer = (HNodeTrailer *)((char *)pNode +
                                   HNODE_SIZE +
                                   pNode->dwSize);

   if (pNodeTrailer->dwSize != pNode->dwSize)
      return 0;

   if (pNodeTrailer->dwMagic !=
       ((uint32)pNodeTrailer ^ HNODE_TRAILER_MAGIC))
      return 0;

   return pNode;
}

/*
||  Given a heap node, find the range of contiguous memory
||   which would be available if the node were to be moved
||   or freed.
||  This function is used when reallocating or relocating
||   an existing heap node.
||  Fill in *ppRange and *pdwSize with a description of the
||   range.
||  Fill in *ppLeftNode and *ppRightNode with pointers to
||   the neighboring nodes (if any) which contribute to
||   the reported range.
||  Fill in *pdwFlags with the "provisional" flags which
||   would be used to initialize the reallocated or
||   relocated node in the reported range.
*/

static __inline void h1_get_free_range(MemHeap *pHeap,
                                       HNode *pNode,
                                       void **ppRange,
                                       uint32 *pdwSize,
                                       HNode **ppLeftNode,
                                       HNode **ppRightNode,
                                       uint32 *pdwFlags)
{
   HNode *pNodeExtra;

   *ppRange = (void *)pNode;
   *pdwSize = pNode->dwSize + NODE_OVERHEAD;
   *ppLeftNode = 0;
   *ppRightNode = 0;
   *pdwFlags =
      (HNODE_FLAG_SIG |
       HNODE_FLAG_BUSY |
       (pNode->dwFlags & (HNODE_FLAG_LEFT | HNODE_FLAG_RIGHT | HNODE_FLAG_LOCKS)));

   pNodeExtra = h1_left_verified_node(pHeap, pNode);
   if (pNodeExtra && h1_node_is_free(pNodeExtra)) {
      *ppRange = (void *)pNodeExtra;
      *pdwSize += pNodeExtra->dwSize + NODE_OVERHEAD;
      *ppLeftNode = pNodeExtra;
      *pdwFlags &= (pNodeExtra->dwFlags | ~HNODE_FLAG_LEFT);
   }

   pNodeExtra = h1_right_verified_node(pHeap, pNode);
   if (pNodeExtra && h1_node_is_free(pNodeExtra)) {
      *pdwSize += pNodeExtra->dwSize + NODE_OVERHEAD;
      *ppRightNode = pNodeExtra;
      *pdwFlags &= (pNodeExtra->dwFlags | ~HNODE_FLAG_RIGHT);
   }
}

/*
||  Given a heap, a required buffer size, a required
||   alignment, and an optional existing node, find the
||   "best" location which can be used to satisfy the
||   allocation request.
||  If pNodeOrig is not NULL, the allocation algorithm
||   should consider "reusing" the space which is currently
||   part of that node.  This is used when reallocating or
||   relocating an existing node, so that the reallocated
||   or relocated node may partially or completely overlap
||   the existing node.
||
||  Fill in *ppRange and *pdwSize with a description of the
||   range to be used.
||  Fill in *ppLeftNode and *ppRightNode with pointers to
||   the neighboring nodes (if any) which contribute to
||   the reported range.
||  Fill in *pdwFlags with the "provisional" flags which
||   would be used to initialize the allocated node
||   in the reported range.
*/

static __inline int h1_find_range_for_alloc(MemHeap *pHeap,
                                            uint32 dwSize,
                                            uint32 dwAlign,
                                            HNode *pNodeOrig,
                                            void **ppRange,
                                            uint32 *pdwSize,
                                            HNode **ppLeftNode,
                                            HNode **ppRightNode,
                                            uint32 *pdwFlags)
{
   uint32 dwSizeAdjusted;
   uint32 dwNodeSize;
   uint32 dwBestSize;
   HNode *pNodeBest;
   HNode *pNode;
   int nBucket;

   dwSizeAdjusted = ((dwSize+BASIC_UMASK)&~BASIC_UMASK);

   if (pNodeOrig) {
      h1_get_free_range(pHeap, pNodeOrig, ppRange, pdwSize,
                        ppLeftNode, ppRightNode, pdwFlags);
      /* If it's a perfect fit including alignment, take it */
      if (*pdwSize == dwSizeAdjusted+NODE_OVERHEAD) {
         if ((((uint32)(*ppRange)+HNODE_SIZE) & (dwAlign-1)) == 0) {
            /* All done.  Range is a perfect fit and is already
               filled in for the caller. */
            return SUCCESS;
         }
      }
      /* Otherwise do a full search looking for a better fit */
      /* If the range can't possibly hold the allocation
         requested, zero pNodeOrig */
      if (((((uint32)(*ppRange)+HNODE_SIZE+(dwAlign-1))&~(dwAlign-1))+dwSizeAdjusted+HNODETRAILER_SIZE)>((uint32)(*ppRange)+(*pdwSize)))
         pNodeOrig = 0;
   }

   pNodeBest = 0;
   dwBestSize = ~(uint32)0;
   for (nBucket = h1_bucket(dwSizeAdjusted); nBucket < BUCKET_COUNT; nBucket++) {
      pNode = (HNode *)pHeap->aBuckets[nBucket].pLink2;
      while (h1_node_is_free(pNode) && h1_verify_node(pHeap, pNode) == SUCCESS) {
         dwNodeSize = pNode->dwSize;
         if (dwNodeSize >= dwSizeAdjusted) {
            if (dwNodeSize - dwSizeAdjusted >= h1_alignment_slop(pNode, dwAlign)) {
               if (dwNodeSize == dwSizeAdjusted) {
                  pNodeBest = pNode;
                  break;
               }
               if (dwNodeSize < dwBestSize) {
                  dwBestSize = dwNodeSize;
                  pNodeBest = pNode;
               }
            }
         }
         pNode = (HNode *)pNode->pLink2;
      }
      if (pNodeBest)
         break;
   }

   if (pNodeOrig &&
       (pNodeBest == 0 ||
        pNodeBest->dwSize + NODE_OVERHEAD >= *pdwSize)) {
      return SUCCESS;
   }

   if (pNodeBest) {
      /* Found a block that fits.  Use it. */
      *ppRange = (void *)pNodeBest;
      *pdwSize = pNodeBest->dwSize + NODE_OVERHEAD;
      *ppLeftNode = pNodeBest;
      *ppRightNode = 0;
      *pdwFlags =
         (HNODE_FLAG_SIG |
          HNODE_FLAG_BUSY |
          (pNodeBest->dwFlags & (HNODE_FLAG_LEFT | HNODE_FLAG_RIGHT)));
      return SUCCESS;
   }

   return ENOMEMORY;
}

static HNode *h1_node_to_ptr(MemHeap *pHeap, HNode *pNode)
{
   void *pRet;

   pRet = (void *)((char *)pNode+sizeof(HNode)+h1_node_get_extra_bytes_left(pNode));

   if (h1_verify_node(pHeap, pNode) != SUCCESS)
      pRet = 0;

   return pRet;
}

static void h1_create_empty_heap(MemHeap *pHeap, uint32 dwBytes, HNode *pNode)
{
   uint32 nBucket;

   pHeap->dwCheck = MEMHEAP_MAGIC ^ (uint32)pHeap;
   pHeap->pFirstNode = pNode;
   pHeap->pCorruptFunc = 0;
   pHeap->pNodeLowLimit = pNode;
   pHeap->pNodeHighLimit = (HNode *)((char *)pNode+(pNode->dwSize-BASIC_UALIGN));
   pHeap->pWalkLast = 0;
   pHeap->pWalkFixed = 0;

   for (nBucket = 0; nBucket < BUCKET_COUNT; nBucket++)
      h1_create_node(pHeap,
                     &pHeap->aBuckets[nBucket], 0, 0,
                     HNODE_FLAG_SIG, 0, 0);

   h1_create_node(pHeap,
                  pHeap->pDelay, 0, 0,
                  HNODE_FLAG_SIG, 0, 0);

   pHeap->uAlign = DEFAULT_USER_ALIGN;
   pHeap->dwDelayMax = DEFAULT_DELAY_MAX;
   pHeap->dwDelayCount = 0;
   pHeap->dwTotalBytes = dwBytes;
   pHeap->dwFreeBytes = 0;
   pHeap->dwFreeBlocks = 0;

   h1_add_to_free_list(pHeap, pNode);
}

static uint32 h1_heap_biggest_free_block(MemHeap *pHeap)
{
   uint32 dwSizeSlop;
   unsigned nBucket;
   uint32 dwBig;
   HNode *pNode;

   nBucket = BUCKET_COUNT;
   dwBig = 0;

   do {
      --nBucket;
      if (dwBig && h1_bucket(dwBig) > nBucket)
         break;
      pNode = (HNode *)pHeap->aBuckets[nBucket].pLink2;
      if (h1_node_is_free(pNode)) {
         dwSizeSlop = h1_alignment_slop(pNode, pHeap->uAlign);
         if (dwSizeSlop < pNode->dwSize)
            dwBig = pNode->dwSize - dwSizeSlop;
         for (;;) {
            pNode = (HNode *)pNode->pLink2;
            if (!h1_node_is_free(pNode))
               break;
            dwSizeSlop = h1_alignment_slop(pNode, pHeap->uAlign);
            if (dwSizeSlop < pNode->dwSize)
               if (dwBig < pNode->dwSize - dwSizeSlop)
                  dwBig = pNode->dwSize - dwSizeSlop;
         }
      }
   } while (nBucket);

   return dwBig;
}

static void h1_coalesce_and_add_to_free_list(MemHeap *pHeap, HNode *pNode)
{
   HNode *pNode2;
   uint32 dwCheck;
   uint32 dwTemp;

   dwCheck = pNode->dwCheck;
   dwCheck ^= (uint32)pNode->pLink1;
   pNode->pLink1 = 0;
   dwCheck ^= (uint32)pNode->pLink2;
   pNode->pLink2 = 0;
   dwTemp = pNode->dwFlags & (HNODE_FLAG_BUSY |
                              HNODE_FLAG_UASHIFT |
                              HNODE_FLAG_LOCKS |
                              HNODE_FLAG_EXTRA_RIGHT |
                              HNODE_FLAG_EXTRA_LEFT);
   pNode->dwCheck = dwCheck ^ dwTemp;
   pNode->dwFlags ^= dwTemp;

   pNode2 = h1_right_verified_node(pHeap, pNode);
   if (pNode2) {
      if (pNode2->dwFlags & HNODE_FLAG_FREE) {
         h1_remove_from_free_list(pHeap, pNode2);
         h1_create_node(pHeap,
                        pNode, 0,
                        pNode->dwSize + pNode2->dwSize + NODE_OVERHEAD,
                        HNODE_FLAG_SIG |
                        (pNode->dwFlags & HNODE_FLAG_LEFT) |
                        (pNode2->dwFlags & HNODE_FLAG_RIGHT), 0, 0);
      }
   }

   pNode2 = h1_left_verified_node(pHeap, pNode);
   if (pNode2) {
      if (pNode2->dwFlags & HNODE_FLAG_FREE) {
         h1_remove_from_free_list(pHeap, pNode2);
         h1_create_node(pHeap,
                        pNode2, 0,
                        pNode->dwSize + pNode2->dwSize + NODE_OVERHEAD,
                        HNODE_FLAG_SIG |
                        (pNode2->dwFlags & HNODE_FLAG_LEFT) |
                        (pNode->dwFlags & HNODE_FLAG_RIGHT), 0, 0);
         pNode = pNode2;
      }
   }

   if (pHeap->pWalkFixed) {
      if ((char *)(pHeap->pWalkFixed) >= (char *)pNode) {
         if ((char *)(pHeap->pWalkFixed) < (char *)pNode + pNode->dwSize + NODE_OVERHEAD) {
            pHeap->pWalkFixed = pNode+1;
         }
      }
   }

   h1_add_to_free_list(pHeap, pNode);
}

static int h1_maybe_move_node(MemHeap *pHeap, HNode *pNode, int iPosition)
{
   uint32 dwCurrentAddr;
   uint32 dwRightAddr;
   HNode *pRightNode;
   HNode *pLeftNode;
   uint32 uSizeOrig;
   uint32 dwFlags;
   uint32 dwSize;
   void *pRange;

   h1_get_free_range(pHeap, pNode,
                     &pRange, &dwSize,
                     &pLeftNode, &pRightNode, &dwFlags);

   uSizeOrig =
      pNode->dwSize -
      (h1_node_get_extra_bytes_left(pNode)+
       h1_node_get_extra_bytes_right(pNode));

   /* Quick check to see if there is a chance to move the block right */

   dwCurrentAddr = (uint32)h1_node_to_ptr(pHeap, pNode);

   dwRightAddr = ((uint32)pRange +
                  dwSize -
                  HNODETRAILER_SIZE -
                  uSizeOrig)&~(h1_node_get_ualign(pNode)-1);

   if (dwRightAddr <= dwCurrentAddr)
      return SUCCESS;    /* SUCCESS here indicates a lack of error,
                            rather than an actual successful move
                            of the block. */

   return h1_alloc_from_range(pHeap, pRange, dwSize,
                              uSizeOrig, h1_node_get_ualign(pNode), dwFlags, 0,
                              &pNode,
                              iPosition,
                              FALSE,
                              h1_node_to_ptr(pHeap, pNode), uSizeOrig,
                              pLeftNode, pRightNode, pNode->pLink1);
}

static void *h1_malloc(MemHeap *pHeap,
                       uint32 uSize,
                       uint32 uAlign,
                       boolean bTestOnly,
                       boolean bZero)
{
   HNode *pNode;
   void *pRange;
   uint32 dwRangeLen;
   HNode *pLeftNode;
   HNode *pRightNode;
   uint32 dwFlags;

   if (h1_find_range_for_alloc(pHeap, uSize, uAlign,
                               0, &pRange, &dwRangeLen, &pLeftNode,
                               &pRightNode, &dwFlags) != SUCCESS)
      return 0;

   if (bTestOnly)
      return pRange;

   if (h1_alloc_from_range(pHeap, pRange, dwRangeLen, uSize, uAlign,
                           dwFlags, 1, &pNode, H1_ALLOC_LEFT, bZero, 0, 0,
                           pLeftNode, pRightNode, 0) != SUCCESS)
      return 0;

   return h1_node_to_ptr(pHeap, pNode);
}

static int h1_free_one_delayed_block(MemHeap *pHeap)
{
   HNode *pNode;
   int nErr;

   pNode = (HNode *)(pHeap->pDelay->pLink1);

   nErr = h1_verify_node(pHeap, pNode);

   if (nErr == SUCCESS) {
      h1_remove_from_delay_list(pHeap, pNode);
      h1_coalesce_and_add_to_free_list(pHeap, pNode);
   }

   return nErr;
}

static void *h1_free(MemHeap *pHeap, HNode *pNode)
{
   if (h1_node_is_allocated(pNode)) {
      if (pHeap->dwDelayMax == 0) {
         /* Clear any pending delayed frees */
         while (pHeap->dwDelayCount)
            if (h1_free_one_delayed_block(pHeap) != SUCCESS)
               break;
         h1_coalesce_and_add_to_free_list(pHeap, pNode);
      } else {
         while (pHeap->dwDelayCount >= pHeap->dwDelayMax)
            if (h1_free_one_delayed_block(pHeap) != SUCCESS)
               break;
         h1_add_to_delay_list(pHeap, pNode);
      }
   } else {
      /* TBD - give a warning about a bogus free... */
   }

   return 0;
}

static void *h1_realloc(MemHeap *pHeap, HNode *pNode, void *p, uint32 uSizeNew, boolean bZero)
{
   uint32 dwRangeLen;
   uint32 uSizeOrig;
   HNode *pLeftNode;
   HNode *pRightNode;
   uint32 dwFlags;
   void *pRange;
   HNode *pNodeRet;

   if (!h1_node_is_allocated(pNode))
      return 0;

   uSizeOrig =
      pNode->dwSize -
      (h1_node_get_extra_bytes_left(pNode)+
       h1_node_get_extra_bytes_right(pNode));
   if (uSizeNew == uSizeOrig)
      return p;

   h1_get_free_range(pHeap, pNode,
                     &pRange, &dwRangeLen,
                     &pLeftNode, &pRightNode, &dwFlags);

   if ((char *)pRange + dwRangeLen >= (char *)p + uSizeNew + HNODETRAILER_SIZE) {
      /* It's worth trying to realloc it in place */

      if (h1_alloc_from_range(pHeap, pRange, dwRangeLen,
                              uSizeNew, h1_node_get_ualign(pNode), dwFlags,
                              h1_node_get_lock_count(pNode),
                              &pNode,
                              H1_ALLOC_PRESERVE,
                              bZero,
                              p, uSizeOrig,
                              pLeftNode, pRightNode, pNode->pLink1) == SUCCESS) {
         return p;
      }
   }

   /* Unable to realloc in place */

   if (h1_find_range_for_alloc(pHeap, uSizeNew, h1_node_get_ualign(pNode),
                               pNode, &pRange, &dwRangeLen, &pLeftNode,
                               &pRightNode, &dwFlags) != SUCCESS)
      return 0;

   if (h1_alloc_from_range(pHeap, pRange, dwRangeLen,
                           uSizeNew, h1_node_get_ualign(pNode), dwFlags,
                           h1_node_get_lock_count(pNode),
                           &pNodeRet,
                           H1_ALLOC_LEFT,
                           bZero,
                           p, uSizeOrig,
                           pLeftNode, pRightNode, pNode->pLink1) != SUCCESS)
      return 0;

   /* If original node is outside the range we just allocated from,
      then free the original node */

   if ((char *)pNode < (char *)pRange ||
       (char *)pNode >= (char *)pRange + dwRangeLen)
      h1_coalesce_and_add_to_free_list(pHeap, pNode);

   return h1_node_to_ptr(pHeap, pNodeRet);
}

static int heapOne_Init(MemHeap *pHeap,
                        void *(*pfnSbrk)(void *pSbrkCx,
                                         int32 nIncrement),
                        void *pSbrkCx)
{
   uint32 dwBytes;
   uint32 dwSize;
   HNode *pNode;
   void *pEnd;

   if (pHeap == 0)
      return EFAILED;
   else if ((uint32)pHeap & BASIC_UMASK)
      return EFAILED;
   else if ((uint32)pHeap > MAX_ADDRESS_OF_HEAP_START)
      return EFAILED;
   else if (pfnSbrk)
      pEnd = (*pfnSbrk)(pSbrkCx, 0);
   else if (pSbrkCx)
      pEnd = pSbrkCx;
   else
      return EFAILED;

   pEnd = ((char *)pEnd-0);

   if ((uint32)pEnd <= (uint32)pHeap)
      return EFAILED;

   dwBytes = (uint32)pEnd - (uint32)pHeap;

   if (dwBytes < MINIMUM_HEAP_SIZE)
      return EFAILED;

   /* Optionally check here that the entire block is R/W memory
      before we go building a heap in the block.  This might
      permit us to catch problems now rather than crashing
      at some later point. */

   pNode = (HNode *)((uint32)pHeap+sizeof(MemHeap));

   dwSize = ((((uint32)pEnd - (uint32)pNode) - NODE_OVERHEAD) & ~BASIC_UMASK);

   h1_create_node(pHeap,
                  pNode, 0, dwSize,
                  HNODE_FLAG_SIG, 0, 0);

   h1_create_empty_heap(pHeap, dwBytes, pNode);

   /* Allocate blocks on 16 MB boundaries if needed */

#ifdef FEATURE_HEAP_RESERVE_16MB_BOUNDARY_BLOCKS
   if (1) {
#else
   if (0) {
#endif
      void *pMem;

      /* Repeatedly allocate 1 byte on a 16MB boundary until
         we can't allocate any more.  This should cover all of
         the possibilities for returning a block which crosses
         such a memory boundary */

      /* After each block is allocated, swap it from allocated
         to "delayed", but don't put it on the delayed list
         and don't count it as a delayed block.  This will
         cause the block to be reported as free for the
         purposes of walking the heap, etc., but the block
         will never be coalesced with neighbors, nor will it
         ever transition to the free state. */

      /* Don't keep track of the blocks allocated.  It is not
         anticipated that the blocks will ever need to be freed */

      while ((pMem = h1_malloc(pHeap, 1, 16*1024*1024, FALSE, FALSE)) != 0) {
         if ((pNode = h1_ptr_to_verified_node(pHeap, pMem)) != 0) {
            pNode->dwFlags ^= HNODE_FLAG_BUSY | HNODE_FLAG_DELAY;
            pNode->dwCheck ^= HNODE_FLAG_BUSY | HNODE_FLAG_DELAY;
         }
      }
   }

   return SUCCESS;
}

static void *heapOne_Realloc(MemHeap *pHeap,
                             void *p,
                             uint32 dwSizeEncoded)
{
   boolean bZero;
   uint32 dwSize;
   HNode *pNode;
   void *pRet;

   dwSize = (dwSizeEncoded & ~ALLOC_NO_ZMEM);
   pRet = 0;

   if (dwSize == 0) {
      if (p == 0) {
         /* free(NULL) -- do nothing */
      } else {
         pNode = h1_ptr_to_verified_node(pHeap, p);
         if (pNode) {
            pRet = h1_free(pHeap, pNode);
         } else {
            /* TBD - give a warning about possible corrupt heap */
         }
      }
   } else {
      bZero = (dwSizeEncoded & ALLOC_NO_ZMEM) ? FALSE : TRUE;
      if (p == 0) {
         pRet = h1_malloc(pHeap, dwSize, pHeap->uAlign, FALSE, bZero);
      } else {
         pNode = h1_ptr_to_verified_node(pHeap, p);
         if (pNode) {
            pRet = h1_realloc(pHeap, pNode, p, dwSize, bZero);
         } else {
            /* TBD - give a warning about possible corrupt heap */
         }
      }
   }

   return pRet;
}

static unsigned long heapOne_Lock(MemHeap *pHeap,
                                  HNode *pNode)
{
   uint32 dwRet;

   dwRet = h1_node_get_lock_count(pNode);

   if (dwRet == HNODE_MAX_LOCKS) {
      /* Once the lock count reaches the maximum, we start
         ignoring all lock and unlock requests, since we
         can no longer guarantee an accurate lock count. */
   } else {
      h1_node_set_lock_count(pNode, dwRet+1);
   }

   return dwRet;
}

static unsigned long heapOne_Unlock(MemHeap *pHeap,
                                    HNode *pNode)
{
   uint32 dwRet;

   dwRet = h1_node_get_lock_count(pNode);

   if (dwRet >= 2) {
      if (dwRet == HNODE_MAX_LOCKS) {
         /* Once the lock count reaches the maximum, we start
            ignoring all lock and unlock requests, since we
            can no longer guarantee an accurate lock count. */
      } else {
         h1_node_set_lock_count(pNode, dwRet-1);
      }
   } else {
      if (dwRet == 0) {
         /* Lock count was already zero.  Nothing to do. */
      } else {
         h1_node_set_lock_count(pNode, 0);
         if (pNode->pLink1)
            h1_maybe_move_node(pHeap, pNode, H1_ALLOC_RIGHT);
      }
   }

   return dwRet;
}

static void **heapOne_Sentinel(MemHeap *pHeap,
                               HNode *pNode,
                               void **pHandle)
{
   void **ppRet;

   ppRet = (void **)pNode->pLink1;

   pNode->pLink1 = (void *)pHandle;
   pNode->dwCheck ^= (uint32)pHandle ^ (uint32)ppRet;
   if (pHandle && h1_node_get_lock_count(pNode) == 0)
      h1_maybe_move_node(pHeap, pNode, H1_ALLOC_RIGHT);

   return ppRet;
}

static int heapOne_NodeInfo(MemHeap *pHeap,
                            AEEkHeapNodeInfo *pNodeInfoBuf,
                            unsigned uNodeInfoBufSize)
{
   HNode *pNode;

   if (pHeap == 0 ||
       pNodeInfoBuf == 0 ||
       pNodeInfoBuf->p == 0 ||
       uNodeInfoBufSize < sizeof(AEEkHeapNodeInfo))
      return EBADPARM;

   pNode = h1_ptr_to_verified_node(pHeap, pNodeInfoBuf->p);
   if (pNode == 0)
      return EMEMPTR;

   if (h1_node_is_free(pNode) ||
       (pNode->dwFlags & HNODE_FLAG_DELAY) != 0) {
      pNodeInfoBuf->ppSentinel = 0;
      pNodeInfoBuf->uFlags = KHNF_FREE;
      pNodeInfoBuf->uLockCount = 0;
      pNodeInfoBuf->uOverhead = NODE_OVERHEAD;
      pNodeInfoBuf->uSize = pNode->dwSize;
   } else {
      pNodeInfoBuf->ppSentinel = pNode->pLink1;
      pNodeInfoBuf->uFlags = 0;
      pNodeInfoBuf->uLockCount = h1_node_get_lock_count(pNode);
      pNodeInfoBuf->uOverhead =
         NODE_OVERHEAD+
         h1_node_get_extra_bytes_left(pNode)+
         h1_node_get_extra_bytes_right(pNode);
      pNodeInfoBuf->uSize =
         NODE_OVERHEAD+
         pNode->dwSize-
         pNodeInfoBuf->uOverhead;
   }

   return SUCCESS;
}

static void heapOne_Compress(MemHeap *pHeap)
{
   HNode *pNodeHold;
   HNode *pNode;

   /* Walk the heap from RIGHT to LEFT, and for each moveable
      allocated block, relocate it to the right as far as
      possible without "hopping" over another allocated block */

   /* This is by no means guaranteed to be optimal -- doing
      an optimal packing of moveable blocks into a fragmented
      heap is probably both ambiguous (what is optimal?) and
      impractical (NP-hard?) */

   pNode = (HNode *)(((uint32)pHeap + pHeap->dwTotalBytes) & ~BASIC_UMASK);
   pNodeHold = 0;

   for (;;) {
      pNode =
         (HNode *)
         ((char *)pNode -
          ((HNodeTrailer *)pNode)[-1].dwSize -
          NODE_OVERHEAD);
      if (h1_node_is_allocated(pNode)) {
         if (pNodeHold &&
             pNodeHold->pLink1 &&
             h1_node_get_lock_count(pNodeHold) == 0)
            h1_maybe_move_node(pHeap, pNodeHold, H1_ALLOC_RIGHT);
         pNodeHold = pNode;
      }
      if (!(pNode->dwFlags & HNODE_FLAG_LEFT)) {
         if (pNodeHold &&
             pNodeHold->pLink1 &&
             h1_node_get_lock_count(pNodeHold) == 0)
            h1_maybe_move_node(pHeap, pNodeHold, H1_ALLOC_RIGHT);
         break;
      }
   }
}

static int heapOne_Walk(MemHeap *pHeap,
                        AEEkHeapNodeInfo *pWalkBuf,
                        unsigned uWalkBufSize)
{
   HNode *pNode;

   if (pWalkBuf->p == 0) {
      pWalkBuf->p = h1_node_to_ptr(pHeap, pHeap->pFirstNode);
   } else {
      if (pWalkBuf->p == pHeap->pWalkLast)
         pWalkBuf->p = pHeap->pWalkFixed;
      pNode = h1_ptr_to_verified_node(pHeap, pWalkBuf->p);
      if (pNode == 0) {
         pHeap->pWalkLast = 0;
         pHeap->pWalkFixed = 0;
         return EHEAP;
      }
      pNode = h1_right_verified_node(pHeap, pNode);
      if (pNode == 0) {
         pHeap->pWalkLast = 0;
         pHeap->pWalkFixed = 0;
         return ENOMEMORY;
      }
      pWalkBuf->p = h1_node_to_ptr(pHeap, pNode);
   }

   pHeap->pWalkLast = pWalkBuf->p;
   pHeap->pWalkFixed = pWalkBuf->p;

   return heapOne_NodeInfo(pHeap, pWalkBuf, uWalkBufSize);
}

static boolean heapOne_CheckMemAvail(MemHeap *pHeap,
                                     uint32 dwSizeEncoded)
{
   uint32 dwSize;
   boolean bRet;

   dwSize = (dwSizeEncoded & ~ALLOC_NO_ZMEM);
   bRet = TRUE;

   if (dwSize != 0) {
      if (h1_malloc(pHeap, dwSize, pHeap->uAlign, TRUE, FALSE) == 0) {
         bRet = FALSE;
      }
   }

   return bRet;
}

static uint32 heapOne_GetMemStats(MemHeap *pHeap,
                                  uint32 *pdwTotal,
                                  uint32 *pdwMax)
{
   if (pdwTotal)
      *pdwTotal = pHeap->dwTotalBytes;

   if (pdwMax)
      *pdwMax = h1_heap_biggest_free_block(pHeap);

   return pHeap->dwFreeBytes;
}

static __inline int heapOne_ValidateAndAcquireHeap(MemHeap *pHeap)
{
   int nRet;

   /* Validate that the heap pointer provided is valid */

   if ((nRet = h1_verify_heap(pHeap)) != SUCCESS)
      return nRet;

   /* If the heap is protected by a mutual exclusion semaphore
      or something similar, acquire it here */

   /* ACQUIRE_SEMAPHORE(pHeap->Semaphore); */

   /* For now, do nothing */

   return SUCCESS;
}

static __inline void heapOne_ReleaseHeap(MemHeap *pHeap)
{
   /* If the heap is protected by a mutual exclusion semaphore
      or something similar, release it here */

   /* RELEASE_SEMAPHORE(pHeap->Semaphore); */

   /* For now, do nothing */
}

/*
||  The "public" entry points below are "wrapper" functions for the
||   private functions above.  The "AEEkHeap" functions below provide
||   parameter validation and mutual exclusion, then call the private
||   "heapOne" functions above.
*/

int AEEkHeap_Init(AEEkHeap *pKHeap,
                  void *(*pfnSbrk)(void *pSbrkCx,
                                   int32 nIncrement),
                  void *pSbrkCx)
{
   MemHeap *pHeap;
   int nRet;

   pHeap = (MemHeap *)pKHeap;

   nRet = heapOne_Init(pHeap, pfnSbrk, pSbrkCx);

   return nRet;
}

void *AEEkHeap_Realloc(AEEkHeap *pKHeap,
                       void *p,
                       uint32 uSize)
{
   MemHeap *pHeap;
   void *pRet;

   pHeap = (MemHeap *)pKHeap;
   pRet = 0;

   if (heapOne_ValidateAndAcquireHeap(pHeap) == SUCCESS) {
      pRet = heapOne_Realloc(pHeap, p, uSize);
      heapOne_ReleaseHeap(pHeap);
   }

   return pRet;
}

uint32 AEEkHeap_Lock(AEEkHeap *pKHeap,
                     void *p)
{
   MemHeap *pHeap;
   HNode *pNode;
   uint32 uRet;

   pHeap = (MemHeap *)pKHeap;
   uRet = 0;

   if (heapOne_ValidateAndAcquireHeap(pHeap) == SUCCESS) {
      pNode = h1_ptr_to_verified_node(pHeap, p);
      if (pNode && h1_node_is_allocated(pNode)) {
         uRet = heapOne_Lock(pHeap, pNode);
      }
      heapOne_ReleaseHeap(pHeap);
   }

   return uRet;
}

uint32 AEEkHeap_Unlock(AEEkHeap *pKHeap,
                       void *p)
{
   MemHeap *pHeap;
   HNode *pNode;
   uint32 uRet;

   pHeap = (MemHeap *)pKHeap;
   uRet = 0;

   if (heapOne_ValidateAndAcquireHeap(pHeap) == SUCCESS) {
      pNode = h1_ptr_to_verified_node(pHeap, p);
      if (pNode && h1_node_is_allocated(pNode)) {
         uRet = heapOne_Unlock(pHeap, pNode);
      }
      heapOne_ReleaseHeap(pHeap);
   }

   return uRet;
}

void **AEEkHeap_Sentinel(AEEkHeap *pKHeap,
                         void *p,
                         void **ppSentinel)
{
   MemHeap *pHeap;
   HNode *pNode;
   void **ppRet;

   pHeap = (MemHeap *)pKHeap;
   ppRet = 0;

   if (heapOne_ValidateAndAcquireHeap(pHeap) == SUCCESS) {
      pNode = h1_ptr_to_verified_node(pHeap, p);
      if (pNode && h1_node_is_allocated(pNode)) {
         ppRet = heapOne_Sentinel(pHeap, pNode, ppSentinel);
      }
      heapOne_ReleaseHeap(pHeap);
   }

   return ppRet;
}

int AEEkHeap_NodeInfo(AEEkHeap *pKHeap,
                      AEEkHeapNodeInfo *pNodeInfoBuf,
                      unsigned uNodeInfoBufSize)
{
   MemHeap *pHeap;
   int nRet;

   pHeap = (MemHeap *)pKHeap;

   if ((nRet = heapOne_ValidateAndAcquireHeap(pHeap)) == SUCCESS) {
      nRet = heapOne_NodeInfo(pHeap, pNodeInfoBuf, uNodeInfoBufSize);
      heapOne_ReleaseHeap(pHeap);
   }

   return nRet;
}

void AEEkHeap_Compress(AEEkHeap *pKHeap)
{
   MemHeap *pHeap;

   pHeap = (MemHeap *)pKHeap;

   if (heapOne_ValidateAndAcquireHeap(pHeap) == SUCCESS) {
      heapOne_Compress(pHeap);
      heapOne_ReleaseHeap(pHeap);
   }

   return;
}

int AEEkHeap_Walk(AEEkHeap *pKHeap,
                  AEEkHeapNodeInfo *pWalkBuf,
                  unsigned uWalkBufSize)
{
   MemHeap *pHeap;
   int nRet;

   pHeap = (MemHeap *)pKHeap;
   nRet = EBADPARM;

   if (pWalkBuf != 0 &&
       uWalkBufSize >= sizeof(*pWalkBuf) &&
       (nRet = heapOne_ValidateAndAcquireHeap(pHeap)) == SUCCESS) {
      nRet = heapOne_Walk(pHeap, pWalkBuf, uWalkBufSize);
      heapOne_ReleaseHeap(pHeap);
   }

   return nRet;
}

boolean AEEkHeap_CheckMemAvail(AEEkHeap *pKHeap,
                               uint32 uSize)
{
   MemHeap *pHeap;
   int bRet;

   pHeap = (MemHeap *)pKHeap;
   bRet = FALSE;

   if (heapOne_ValidateAndAcquireHeap(pHeap) == SUCCESS) {
      bRet = heapOne_CheckMemAvail(pHeap, uSize);
      heapOne_ReleaseHeap(pHeap);
   }

   return bRet;
}

uint32 AEEkHeap_GetMemStats(AEEkHeap *pKHeap,
                            uint32 *pdwTotal,
                            uint32 *pdwMax)
{
   MemHeap *pHeap;
   uint32 uRet;

   pHeap = (MemHeap *)pKHeap;
   uRet = 0;

   if (heapOne_ValidateAndAcquireHeap(pHeap) == SUCCESS) {
      uRet = heapOne_GetMemStats(pHeap, pdwTotal, pdwMax);
      heapOne_ReleaseHeap(pHeap);
   }

   return uRet;
}

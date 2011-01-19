/*
  ========================================================================

  FILE:  Vector.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Vector object - objects that manages an array
                       of void pointers and allows dynamic appending,
                       insertions, deletions,etc.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "wutil.h"

/////////////////////////////////////////////////////////////////////////////
// Vector

typedef struct Vector {
   void **        ppItems;          // pointer to array of void* items
   uint16         nItems;           // number of valid items in array
   int            cbSize;           // allocation size of ppItems (for MemGrow)
   int            cbUsed;           // number of bytes used in ppItems (for MemGrow)
   PFNNOTIFY      pfnFree;          // free function (called in Vector_Dtor)
   int            nGrowBy;          // grow-by size for subsequent allocations
} Vector;


// special value for SetPfnFree, indicates that stored ptrs are
// BREW interfaces and should be Released
#define VF_RELEASE   ((PFNNOTIFY)-1)

// generic free memory function
#define VF_FREE      (GET_HELPER()->free)

/* NOTE:
   Interface pointers are not handled any differently than
   any other type of pointer, e.g., they are not AddRefed 
   when inserted or added to the Vector. 
   
   For the following functions -- Delete(), ReplaceAt(), 
   Reset(), and Release(),the installed Free function is 
   called on the held pointer before it is relinquished. 
   If the installed Free function is set to VF_RELEASE, 
   the effective free function is IBASE_Release().
*/

int   Vector_New            (Vector **ppo);
void  Vector_Delete         (Vector *me);
void  Vector_Dtor           (Vector *me);
int   Vector_GetAt          (const Vector *me, uint32 nIndex, void **ppoItem);
int   Vector_ReplaceAt      (Vector *me, uint32 nIndex, void *pvItem);
int   Vector_InsertAt       (Vector *me, uint32 nIndex, void *pvItem);
int   Vector_DeleteAt       (Vector *me, uint32 nIndex);
void  Vector_DeleteAll      (Vector *me);
int   Vector_EnsureCapacity (Vector *me, uint32 nRequired, uint32 nGrowBy);

static __inline 
void Vector_Ctor(Vector *me) {
   me->nGrowBy = 10;
}

// note: pfnFree == VF_RELEASE means call IBASE_Release
static __inline
PFNNOTIFY Vector_SetPfnFree(Vector *me, PFNNOTIFY pfnFree) {
   PFNNOTIFY pfnOld = me->pfnFree;
   me->pfnFree = pfnFree;
   return pfnOld; // return old pfn
}

static __inline
int Vector_Size(const Vector *me) {
   return me->nItems;
}

#endif   //__VECTOR_H__


/*
  ========================================================================

  FILE:  Vector.c
  
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
#include "Vector.h"
#include "AEEStdLib.h"


// local protos
static __inline int Vector_FreeItem(Vector *me, uint32 nIndex) {
   return Vector_ReplaceAt(me, nIndex, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// Vector


int Vector_New(Vector **ppo)
{
   Vector *me = MALLOCREC(Vector);

   *ppo = me;
   if (!me) {
      return ENOMEMORY;
   }

   Vector_Ctor(me);
   return SUCCESS;
}


void Vector_Delete(Vector *me)
{
   Vector_Dtor(me);
   FREE(me);
}


void Vector_Dtor(Vector *me)
{
   Vector_DeleteAll(me);
   FREE(me->ppItems);
}


int Vector_GetAt(const Vector *me, uint32 nIndex, void **ppoItem)
{
   if (nIndex >= me->nItems) {
      *ppoItem = NULL;
      return EBADPARM;
   }

   *ppoItem = me->ppItems[nIndex];
   return SUCCESS;
}


int Vector_ReplaceAt(Vector *me, uint32 nIndex, void *pvNew)
{
   void *pvItem = NULL;
   int nErr = Vector_GetAt(me, nIndex, &pvItem);

   if (!nErr) {

      if (pvItem && me->pfnFree) {
         if (me->pfnFree == VF_RELEASE) {
            IBASE_Release(pvItem);
         } else {
            me->pfnFree(pvItem);
         }
      }
      me->ppItems[nIndex] = pvNew;
   }

   return nErr;
}


static void Vector_ExpandItems(Vector *me, uint32 nIndex, boolean bExpand)
{
   int nDir = bExpand ? 1 : -1;

   if (nIndex < me->nItems) {

      uint32 nIndexSrc = nIndex;
      uint32 nIndexDest = nIndex;

      if (bExpand) {
         nIndexDest++;
      } else {
         nIndexSrc++;
      }

      MEMMOVE(me->ppItems+nIndexDest, me->ppItems+nIndexSrc, (me->nItems - nIndexSrc) * sizeof(void*));
   }

   me->nItems += nDir;
   me->cbUsed = me->nItems * sizeof(void*);
}


int Vector_InsertAt(Vector *me, uint32 nIndex, void *pvItem)
{
   int nErr;

   if (nIndex > me->nItems) {
      nIndex = me->nItems;
   }

   nErr = MemGrowEx((void*)&me->ppItems, &me->cbSize, me->cbUsed, 
                    sizeof(void*), (me->nGrowBy * sizeof(void*)));

   if (!nErr) {
      Vector_ExpandItems(me, nIndex, 1);
      me->ppItems[nIndex] = pvItem;
   }

   return nErr;
}

int Vector_DeleteAt(Vector *me, uint32 nIndex)
{
   int nErr = Vector_FreeItem(me, nIndex);
   if (!nErr) {
      Vector_ExpandItems(me, nIndex, 0);
   }
   return nErr;
}


void Vector_DeleteAll(Vector *me)
{
   uint16 i;
   for (i=0; i < me->nItems; i++) {
      Vector_FreeItem(me, i);
   }
   me->nItems = 0;
   me->cbUsed = 0;
}


int Vector_EnsureCapacity(Vector *me, uint32 nRequired, uint32 nGrowBy)
{
   int nErr = SUCCESS;

   uint32 nAlloc  = me->cbSize * sizeof(void*);

   me->nGrowBy = nGrowBy;

   if (nAlloc < nRequired) {
      int cbAlloc = (nRequired - nAlloc) * sizeof(void*);
      nErr = MemGrow((void*)&me->ppItems, &me->cbSize, me->cbSize, cbAlloc);
   }
   return nErr;
}






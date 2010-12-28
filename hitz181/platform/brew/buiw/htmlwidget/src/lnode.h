/*
  ===========================================================================

  FILE:  lnode.h
  
  SERVICES:  
    List node definition for circular doubly linked lists
  
  GENERAL DESCRIPTION:
    
  
  REVISION HISTORY: 
    

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#ifndef __LNODE_H__
#define __LNODE_H__


typedef struct LNode LNode;
struct LNode {
   LNode *     pNext;
   LNode *     pPrev;
};


#define LNODE_CTOR(p)       ((p)->pNext = (p)->pPrev = (p))

// Insert 'pnew' after 'p'
#define LNODE_INSERTAFTER(p,pnew) \
( (pnew)->pPrev = (p),        \
  (pnew)->pNext = (p)->pNext, \
  (p)->pNext->pPrev = (pnew), \
  (p)->pNext = (pnew) )

// Insert 'pnew' before 'p'
#define LNODE_INSERTBEFORE(p,pnew) \
( (pnew)->pPrev = (p)->pPrev,     \
  (pnew)->pNext = (p),            \
  (p)->pPrev->pNext = (pnew),     \
  (p)->pPrev = (pnew) )


// Remove node 'p' from list
#define LNODE_REMOVE(p) \
( (p)->pPrev->pNext = (p)->pNext, \
  (p)->pNext->pPrev = (p)->pPrev )

// traverse list as if it was non-circular
//#define LNODE_GETNEXT(pn,ph)     (pn) = ((pn)->pNext == (ph) ? NULL : (pn)->pNext)
//#define LNODE_GETPREV(pn,ph)     (pn) = ((pn)->pPrev == (ph) ? NULL : (pn)->pPrev)
//#define LNODE_ENUM_FWD(pn,ph)    for (pn=ph; LNODE_GETNEXT(pn,ph); )
//#define LNODE_ENUM_REV(pn,ph)    for (pn=ph; LNODE_GETPREV(pn,ph); )
#define LNODE_ENUM_FWD(pn,ph)    for ((pn) = (ph)->pNext; (pn) != (ph); (pn) = (pn)->pNext)
#define LNODE_ENUM_REV(pn,ph)    for ((pn) = (ph)->pPrev; (pn) != (ph); (pn) = (pn)->pPrev)

#define LNODE_INLIST(pn)         ((pn)->pNext != (pn))
#define LNODE_EMPTY(pn)          ((pn)->pNext == (pn))


#endif   //__LNODE_H__



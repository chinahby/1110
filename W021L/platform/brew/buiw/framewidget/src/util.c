/*
  ========================================================================

  FILE: util.c
  
  SERVICES: 

  GENERAL DESCRIPTION:  


  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "util.h"

// extract coordinates of an AEERect
#define RCRIGHT(prc)          (((prc)->x) + ((prc)->dx) - 1)
#define RCBOTTOM(prc)         (((prc)->y) + ((prc)->dy) - 1)
#define RCCOORD(prc,l,t,r,b)  (l)=(prc)->x,(t)=(prc)->y,(r)=RCRIGHT(prc),(b)=RCBOTTOM(prc)


// calculate the intersection of two rectangles
boolean IntersectRect(AEERect *prcDest, const AEERect *prcSrc1, const AEERect *prcSrc2)
{
   int aleft, atop, aright, abottom;   // coordinates of prcSrc1
   int bleft, btop, bright, bbottom;   // coordinates of prcSrc2

   // get the left, top, right, bottom coordinates of each source rect
   RCCOORD(prcSrc1, aleft, atop, aright, abottom);
   RCCOORD(prcSrc2, bleft, btop, bright, bbottom);

   // early rejection test
   if (aleft > bright || atop > bbottom || bleft > aright || btop > abottom) {
      if (prcDest) {
         SETAEERECT(prcDest, 0, 0, 0, 0);
      }
      return 0;   // empty intersection
   }

   if (prcDest) {
      prcDest->x  = MAX(aleft, bleft);
      prcDest->y  = MAX(atop, btop);
      prcDest->dx = MIN(aright, bright) - prcDest->x + 1;
      prcDest->dy = MIN(abottom, bbottom) - prcDest->y + 1;
   }
   
   return 1;
}



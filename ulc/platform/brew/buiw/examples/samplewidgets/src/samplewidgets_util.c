/*======================================================
FILE:       samplewidgets_util.c

SERVICES:   Utility functions used by the sample widgets

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "samplewidgets_util.h"

//--------------------------------------------------------------------
//  IntersectRect
//--------------------------------------------------------------------

// RCCOORD without the subtracting one
#define GETCORNERS(prc,l,t,r,b)  (l)=(prc)->x,(t)=(prc)->y,(r)=(l)+(prc)->dx,(b)=(t)+(prc)->dy

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

///--------------------------------------------------------------------
//  SubtractRect
//--------------------------------------------------------------------
boolean SubtractRect(AEERect *prcResult, const AEERect *prcBase, const AEERect *prcSub)
{
   int nEdges = 0;
   boolean bLeftEdge = 0;
   boolean bRightEdge = 0;
   boolean bTopEdge = 0;
   boolean bBottomEdge = 0;
   
   int bleft, btop, bright, bbottom;   // prcBase coordinates
   int sleft, stop, sright, sbottom;   // prcSub coordinates

   GETCORNERS(prcBase, bleft, btop, bright, bbottom);
   GETCORNERS(prcSub, sleft, stop, sright, sbottom);

   // count which base rect edges fall between the sub rect edges
   if (BETWEEN(bleft, sleft, sright+1)) {
      bLeftEdge = 1;
      nEdges++;
   }
   if (BETWEEN(bright, sleft, sright+1)) {
      bRightEdge = 1;
      nEdges++;
   }
   if (BETWEEN(btop, stop, sbottom+1)) {
      bTopEdge = 1;
      nEdges++;
   }
   if (BETWEEN(bbottom, stop, sbottom+1)) {
      bBottomEdge = 1;
      nEdges++;
   }

   // empty rect result
   if (nEdges == 4) {
      // set result to empty rect
      bleft = bright = btop = bbottom = 0;
   
   } else if (nEdges == 3) {

      if (!bTopEdge) {
         bbottom = stop;
      } 
      if (!bBottomEdge) {
         btop = sbottom;
      } 
      if (!bLeftEdge) {
         bright = sleft;
      } 
      if (!bRightEdge) {
         bleft = sright;
      }
   }

   SETAEERECT(prcResult, bleft, btop, bright-bleft, bbottom-btop);
   return (boolean)(nEdges > 2);
}

//--------------------------------------------------------------------
//  MonoDIB methods
//--------------------------------------------------------------------

// create a mono DIB
int CreateMonoDIB(IDisplay *piDisplay, IDIB **ppiDIB, uint16 width, uint16 height, boolean bAllocData)
{
   IDIB *piDIB = NULL;
   int nErr = SUCCESS;

   // overallocate to store palette data in pBmp memory
   // this allocates a bitmap data array of 64 bits that we
   // will use to store the 2 dwords of the palette array.
   // the pointers are fixed up below...
   uint16 nAllocWidth = 8;    // 8 bits per byte
   uint16 nAllocHeight = 8;   // 8 bytes in 2 dwords (for palette array)

   // if we were also requested to allocate the data array, add this
   // to the allocation args
   if (bAllocData) {
      nAllocWidth += width;
      nAllocHeight += height;
   }

   nErr = IDISPLAY_CreateDIBitmap(piDisplay, &piDIB, 1, nAllocWidth, nAllocHeight);
   if (nErr == SUCCESS) {

      uint32 *pAlloc = (uint32*)piDIB->pBmp;     // memory allocated for our use

      piDIB->cx      = width;
      piDIB->cy      = height;
      piDIB->nPitch  = 1;
      piDIB->pRGB    = pAlloc;   // first 2 dwords of pAlloc is used for the palette
      piDIB->cntRGB  = 2;
      piDIB->pBmp    = (uint8*)(bAllocData ? (pAlloc + 2) : NULL);
   }

   *ppiDIB = piDIB;  // return piDIB
   return nErr;
}

// Initialize a DIB's palette
void InitMonoDIBPalette(IDIB *piDIB, RGBVAL rgb)
{
   // Set color and flush palette
   piDIB->pRGB[1] = NTOHL(rgb);
   piDIB->pRGB[0] = ~piDIB->pRGB[1];
   piDIB->ncTransparent = 0;  // pRGB[0] is the transparent color by definition
   IDIB_FlushPalette(piDIB);
}
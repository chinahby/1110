/*======================================================
FILE:       wutil.c

SERVICES:   Utility functions for example widgets

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "wutil.h"
#include "AEEStdLib.h"
#include "AEEShell.h"
#include "AEEImage.h"
#include "modres.h"

#define IDIB_COLORSCHEME_8888 32

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

/*
|| SubtractRect
|| 
|| subtraction can only succeed if the result will be a rectangular 
|| region. for example, subtracting rectangle 1 from rectangle 2 
|| will not result in a rectangular region so the result will be 
|| all of rectangle 2. In contrast, subtracting rectangle 3 from 
|| rectangle 4 will result in a rectangular result so the result 
|| will be rectangle 4 reduced in size, and with the origin 
|| offset in the y direction..
|| 
|| 1               3
|| +---+           +----------+
|| |   |  2        |  +----+  |
|| | +-|-+         |  |    |  |
|| | | | |  - NO   +----------+  - YES
|| +---+ |            |    |
||   |   |            +----+ 4
||   +---+
|| 
||
|| returns TRUE if subtraction succeeded
|| with result placed in prcResult
*/
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


/* returns FALSE if either source rectangle is empty */
void UnionRect(AEERect *prcResult, const AEERect *prcSrc1, const AEERect *prcSrc2)
{

   int aleft, atop, aright, abottom;   // coordinates of prcSrc1
   int bleft, btop, bright, bbottom;   // coordinates of prcSrc2


   // get the left, top, right, bottom coordinates of each source rect
   GETCORNERS(prcSrc1, aleft, atop, aright, abottom);
   GETCORNERS(prcSrc2, bleft, btop, bright, bbottom);

   prcResult->x = MIN(aleft, bleft);
   prcResult->dx = MAX(aright, bright) - prcResult->x;

   prcResult->y = MIN(atop, btop);
   prcResult->dy = MAX(abottom, bbottom) - prcResult->y;
}
    

// origin constants for DrawStairStep
#define DSSO_LEFT          0
#define DSSO_RIGHT         1
#define DSSO_TOP           0
#define DSSO_BOTTOM        2

#define DSSO_BOTTOM_LEFT   (DSSO_BOTTOM|DSSO_LEFT)    // origin in bottom/left of rectangle
#define DSSO_BOTTOM_RIGHT  (DSSO_BOTTOM|DSSO_RIGHT)   // origin in bottom/right...
#define DSSO_TOP_LEFT      (DSSO_TOP|DSSO_LEFT)       // ...
#define DSSO_TOP_RIGHT     (DSSO_TOP|DSSO_RIGHT)      // ...

/* fill a rectangle with a stairstep pattern starting from origin
   and working to opposite corner with step height always increasing
   
                           x  x
   DSSO_BOTTOM_LEFT       xx  xx       DSSO_BOTTOM_RIGHT
           (origin) -->  xxx  xxx  <-- (origin)

           (origin) -->  xxx  xxx  <-- (origin)
      DSSO_TOP_LEFT       xx  xx       DSSO_TOP_RIGHT
                           x  x
*/   
static 
void DrawStairStep(IDisplay *piDisplay, AEERect *prc, int nDSSOrigin, RGBVAL rgb, uint8 nAlpha)
{
   int xOrigin, yOrigin;
   AEERect rc;
   int i;

   xOrigin = prc->x;
   if (nDSSOrigin & DSSO_RIGHT) {
      xOrigin += prc->dx-1;
   }

   yOrigin = prc->y;
   if (nDSSOrigin & DSSO_BOTTOM) {
      yOrigin += prc->dy-1;
   }

   SETAEERECT(&rc, xOrigin, yOrigin, 1, 1);
   
   for (i=0; i < prc->dx; i++) {
//      IDISPLAY_DrawRect(piDisplay, &rc, RGB_NONE, rgb, IDF_RECT_FILL);
      BlendRect(piDisplay, &rc, RGBA_STRIPALPHA(rgb), nAlpha);
      rc.dy++;
      if (nDSSOrigin & DSSO_RIGHT) {
         rc.x--;
      } else {
         rc.x++;
      }
      if (nDSSOrigin & DSSO_BOTTOM) {
         rc.y--;
      }
   }

}

void DrawAndInsetBorderEx(IDisplay *piDisplay, AEERect *prc, int nInset, int nThick, RGBVAL rgb, uint8 nAlpha, boolean bBeveled)
{
   AEERect rc;
   AEERect rco = *prc;  // outer rectangle

   nThick = MIN(nThick, nInset);    // dont' allow nThick to exceed nInset

   INFLATERECT(prc, -nInset, -nInset);
   nInset -= nThick;
   INFLATERECT(&rco, -nInset, -nInset);

   rc.x = bBeveled ? prc->x : rco.x;
   rc.y = rco.y;
   rc.dx = bBeveled ? prc->dx : rco.dx;
   rc.dy = nThick;
   BlendRect(piDisplay, &rc, rgb, nAlpha);

   rc.y += prc->dy + nThick;
   BlendRect(piDisplay, &rc, rgb, nAlpha);

   rc.x = rco.x;
//   rc.y = bBeveled ? prc->y : rco.y;
   rc.y = prc->y;
   rc.dx = nThick;
//   rc.dy = bBeveled ? prc->dy : rco.dy;
   rc.dy = prc->dy;
   BlendRect(piDisplay, &rc, rgb, nAlpha);

   rc.x += prc->dx + nThick;
   BlendRect(piDisplay, &rc, rgb, nAlpha);

   ///////////////////////////
   // draw beveled corners
   ///////////////////////////
   if (bBeveled) {
      SETAEERECT(&rc, rco.x+1, rco.y+1, nThick-1, nThick-1);
      DrawStairStep(piDisplay, &rc, DSSO_BOTTOM_LEFT, rgb, nAlpha);
   
      rc.x += (prc->dx + rc.dx);
      DrawStairStep(piDisplay, &rc, DSSO_BOTTOM_RIGHT, rgb, nAlpha);
   
      rc.x = rco.x+1;
      rc.y += (prc->dy + rc.dy);
      DrawStairStep(piDisplay, &rc, DSSO_TOP_LEFT, rgb, nAlpha);
   
      rc.x += (prc->dx + rc.dx);
      DrawStairStep(piDisplay, &rc, DSSO_TOP_RIGHT, rgb, nAlpha);
   }
}


/* Rounded Corners

   To perform the SIN operation  to draw rounded corners, we use the following
   precomputed table. This table stores sin(x) for integral degrees in [0, 90].  
   The values stored in the table are  x/2^15.  For example, 572 really means 
   0.0175.  Note, except for SIN(90), all values are int16s.
*/
const int32 WUTIL_SIN[91] = {
   0, 572, 1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126, 
   5690,   6252,  6813,  7371,  7927,  8481,  9032,  9580, 10126, 
   10668, 11207, 11743, 12275, 12803, 13328, 13848, 14365, 14876, 
   15384, 15886, 16384, 16877, 17364, 17847, 18324, 18795, 19261, 
   19720, 20174, 20622, 21063, 21498, 21926, 22348, 22763, 23170, 
   23571, 23965, 24351, 24730, 25102, 25466, 25822, 26170, 26510, 
   26842, 27166, 27482, 27789, 28088, 28378, 28660, 28932, 29197, 
   29452, 29698, 29935, 30163, 30382, 30592, 30792, 30983, 31164, 
   31336, 31499, 31651, 31795, 31928, 32052, 32166, 32270, 32365, 
   32449, 32524, 32588, 32643, 32688, 32723, 32748, 32763, 32768 
};

#define SIN_ACCURACY_INVERSE        0x8000         // 32768, or 2^15
#define HALF_SIN_ACCURACY_INVERSE   0x4000         // 32768/2, or 2^14

/*


*/
static __inline int CircleGetX(int *pwAngle, int nRadius, int y) {
   
   int p = 0, q = 0;
   for (; (*pwAngle <= 90) && (q < y); (*pwAngle)++) {
      p = nRadius * WUTIL_SIN[90 - *pwAngle] / SIN_ACCURACY_INVERSE;
      q = nRadius * WUTIL_SIN[*pwAngle] / SIN_ACCURACY_INVERSE;
   }
   return (p);
}

/* Draw the four rounded corners of a rounded rectangle.

*/   
static 
void DrawRoundedCorners(IDisplay *piDisplay, AEERect *pRect, int nThick, int nRadius, RGBVAL rgb, uint8 nAlpha)
{
   int wScanLine = 0;      // which scanline we're on
   int wArcWidth;          // width of the intersection of the arc and this scanline
   AEERect rcArc;          // the portion of the scanline representing the arc
   int wXFirst = -1;       // first x value on this scanline 
   int wAngle = 0;         // sweep from 0 to 90 degrees ...
   int wInnerAngle;

   for (wScanLine = 0; wScanLine <= nRadius; wScanLine++) {

      wXFirst = CircleGetX(&wAngle, nRadius + 1, wScanLine);

      if (wScanLine >= pRect->dy - nThick) {
         // we're at the "bottom" of the arc and we need to extend
         // all the way to the straight border edge
         wArcWidth = wXFirst;
      } else {
         wInnerAngle = wAngle;
         wArcWidth = wXFirst - CircleGetX(&wInnerAngle, (nRadius - nThick + 1), wScanLine);
      }

      // now draw all four round corners ...

      // top left
      SETAEERECT(&rcArc, 
                 pRect->x + nRadius - wXFirst, 
                 pRect->y + nRadius - wScanLine, 
                 wArcWidth, 
                 1);
      BlendRect(piDisplay, &rcArc, rgb, nAlpha);

      // bottom left
      rcArc.y = pRect->y + pRect->dy - nRadius + wScanLine - 1, 
      BlendRect(piDisplay, &rcArc, rgb, nAlpha);

      // bottom right
      rcArc.x = pRect->x + pRect->dx - nRadius + wXFirst - wArcWidth;
      BlendRect(piDisplay, &rcArc, rgb, nAlpha);

      // top right
      rcArc.y = pRect->y + nRadius - wScanLine;
      BlendRect(piDisplay, &rcArc, rgb, nAlpha); 
   }
}

/* Draw a rounded rectangle border.  nRadius determines the radius
   of the rounded corners.  nThick determines the thickness of the
   border.
*/   
void DrawAndInsetRoundedBorderEx(IDisplay *piDisplay, AEERect *prc, int nInset, 
                                 int nThick, int nRadius, RGBVAL rgb, uint8 nAlpha)
{
   AEERect rc;
   AEERect rco = *prc;  // outer rectangle

   // make sure radius is no more than half of the shortest border edge
   nRadius = MAX(1,MIN(nRadius, MIN(prc->dx/2, prc->dy/2)));  

   // make sure radius is at least nThick
   // to get rid of this constraint, extra drawing will have to be done.
   nRadius = MAX(nThick, nRadius);

   // dont' allow nThick to exceed nInset
   nThick = MIN(nThick, nInset);    
 
   // add additional inset so no drawing is done outside of the rounded corners

   INFLATERECT(prc, -nInset, -nInset);
   nInset -= nThick;
   INFLATERECT(&rco, -nInset, -nInset);

   // draw the border only if it's thickness is non-zero
   if (nThick > 0) {

      ///////////////////////////
      // draw straight edges
      ///////////////////////////

      // Top edge
      rc.x = rco.x + nRadius;
      rc.y = rco.y;
      rc.dx = rco.dx - 2*nRadius;
      rc.dy = nThick;
      BlendRect(piDisplay, &rc, rgb, nAlpha);

      // Bottom edge
      rc.y = rco.y + rco.dy - nThick;
      rc.dy = nThick;
      BlendRect(piDisplay, &rc, rgb, nAlpha);
   
      // Left edge
      rc.x = rco.x;
      rc.y = rco.y + nRadius;
      rc.dx = nThick;
      rc.dy = rco.dy - 2*nRadius;
      BlendRect(piDisplay, &rc, rgb, nAlpha);

      // Right edge
      rc.x = rco.x + rco.dx - nThick;;
      rc.dx = nThick;
      BlendRect(piDisplay, &rc, rgb, nAlpha);

      ///////////////////////////
      // draw rounded corners
      ///////////////////////////

      DrawRoundedCorners(piDisplay, &rco, nThick, nRadius, rgb, nAlpha);
   }
}

// Draw a border inside the supplied rectangle, and inset the rectangle by that amount.
//
void DrawAndInsetBorder(IDisplay *piDisplay, AEERect *prc, int nThick, RGBVAL rgb)
{
   AEERect rcb = *prc;

   prc->x += nThick;
   prc->y += nThick;
   prc->dx -= nThick*2;
   prc->dy -= nThick*2;

   rcb.dy = nThick;
   IDISPLAY_DrawRect(piDisplay, &rcb, RGB_NONE, RGBA_STRIPALPHA(rgb), IDF_RECT_FILL);

   rcb.dy = nThick;
   rcb.y += prc->dy + nThick;
   IDISPLAY_DrawRect(piDisplay, &rcb, RGB_NONE, RGBA_STRIPALPHA(rgb), IDF_RECT_FILL);

   rcb.dx = nThick;
   rcb.y = prc->y;
   rcb.dy = prc->dy;
   IDISPLAY_DrawRect(piDisplay, &rcb, RGB_NONE, RGBA_STRIPALPHA(rgb), IDF_RECT_FILL);

   rcb.dx = nThick;
   rcb.x = prc->x + prc->dx;
   IDISPLAY_DrawRect(piDisplay, &rcb, RGB_NONE, RGBA_STRIPALPHA(rgb), IDF_RECT_FILL);
}

void IDisplay_DrawVLine(IDisplay *me, int x, int y, int nLen, int nThickness, RGBVAL rgb)
{
   AEERect rc;
   SETAEERECT(&rc, x, y, nThickness, nLen);
   IDISPLAY_FillRect(me, &rc, RGBA_STRIPALPHA(rgb));
}


void IDisplay_DrawHLine(IDisplay *me, int x, int y, int nLen, int nThickness, RGBVAL rgb)
{
   AEERect rc;
   SETAEERECT(&rc, x, y, nLen, nThickness);
   IDISPLAY_FillRect(me, &rc, RGBA_STRIPALPHA(rgb));
}


void IBitmap_SetPixelsEx(IBitmap *me, AEEPoint *pPoint, unsigned cnt, RGBVAL rgb)
{
   NativeColor nc = IBITMAP_RGBToNative(me, rgb);
   IBITMAP_SetPixels(me, cnt, pPoint, nc, AEE_RO_COPY);
}


void IDisplay_SetPixels(IDisplay *me, AEEPoint *pPoint, unsigned cnt, RGBVAL rgb)
{
   IBitmap *pib = IDISPLAY_GetDestination(me);
   if (pib) {
      IBitmap_SetPixelsEx(pib, pPoint, cnt, rgb);
      IBITMAP_Release(pib);
   }
}

/*=============================================================================
FUNCTION: IDisplay_FillRectGradient

DESCRIPTION: GradientDecorator helper for filling a rectangle with a gradient

PARAMETERS:
   piDisplay   [in]: IDisplay to draw to
   rc          [in]: AEERect to fill
   rgbStart    [in]: Starting RGB value
   rgbEnd      [in]: Ending RGB value
   bVertical   [in]: TRUE for vertical fill, otherwise horizontal fill.
   bCenter     [in]: TRUE for center fill mode
   bAlpha      [in]: TRUE to enable alpha channel blending

RETURN VALUE:

COMMENTS:
   Fills a rectangle with either a horizontal or vertical gradient pattern, left-to-right
   or top to bottom, respectively, with the color changing from rgbStart to rgbEnd.

SIDE EFFECTS:
   None.

SEE ALSO:
   None

=============================================================================*/
void IDisplay_FillRectGradient(IDisplay *pDisplay, const AEERect *prc, RGBVAL rgbStart, RGBVAL rgbEnd, 
                               boolean bVertical, boolean bCenter, boolean bAlpha)
{
   register int32 rs, gs, bs, as;
   int32 re, ge, be, ae;
   int16  rinc, ginc, binc, ainc;
   int16 nScanWidth, nOrigin, nWidth;
   AEERect rcDraw;
   RGBVAL rgb=0;
   int16 alpha=0;
   int16 *pn = 0;
   int i;

   //divide by zero sucks
   if (!prc->dy || !prc->dx) {
      return;
   }

   // extract the RGB values.  We'll use 8bit fixed point math so that
   // we have enough resolution to account for small gradients over large
   // pixel deltas... so EXTRACT_RGB_FP8 shifts left 8 less than normal
   EXTRACT_RGB_FP8(rgbStart, rs, gs, bs, as);
   EXTRACT_RGB_FP8(rgbEnd, re, ge, be, ae);

   rcDraw = *prc;

   // setup direction dependent vars
   if (bVertical) {
      rcDraw.dy = 1;
      nScanWidth = nWidth = prc->dy;
      nOrigin = rcDraw.y;
      pn = &rcDraw.y;
   } else {
      rcDraw.dx = 1;
      nScanWidth = nWidth = prc->dx;
      nOrigin = rcDraw.x;
      pn = &rcDraw.x;
   }

   if (bCenter) {
      nScanWidth /= 2;
   }

   if (!nScanWidth) {
      return;  // divz still sucks
   }

   // get color and alpha increments
   rinc = (int16) ((re - rs) / nScanWidth);
   ginc = (int16) ((ge - gs) / nScanWidth);
   binc = (int16) ((be - bs) / nScanWidth);
   ainc = (int16) ((ae - as) / nScanWidth);

   for (i=0; i < nScanWidth; i++) {

      // using 8 bit fixed point, so shift that out... the ARM uses a 
      // barrel shifter that can do these shifts in 1 clock, so this
      // is very inexpensive (faster than a ram lookup)
      rgb = MAKE_RGB_FP8(rs, gs, bs);

      *pn = nOrigin + i;      // shift position of draw rectangle

      if (bAlpha) {
         alpha = (as >> 8) & 0xff;
         as += ainc;
         BlendRect(pDisplay, &rcDraw, rgb, alpha);
      } else {
         IDISPLAY_FillRect(pDisplay, &rcDraw, RGBA_STRIPALPHA(rgb));
      }

      // if center fill mode, we draw the line in the same color 
      // from symmetrical positions on the left & right or top & bottom sides
      if (bCenter) {
         *pn = nOrigin + (nWidth-i-1);    // move draw rectangle symmetrically
         if (bAlpha) {
            BlendRect(pDisplay, &rcDraw, rgb, alpha);
         } else {
            IDISPLAY_FillRect(pDisplay, &rcDraw, RGBA_STRIPALPHA(rgb));
         }
      }
      rs += rinc;
      gs += ginc;
      bs += binc;
   }

   // handle center-fill problem where dividing an odd nScanWidth 
   // causes loss of precision and results in a hole in center
   if (bCenter && (nWidth & 1)) {
      *pn = nOrigin + i;
      if (bAlpha) {
         BlendRect(pDisplay, &rcDraw, rgb, alpha);
      } else {
         IDISPLAY_FillRect(pDisplay, &rcDraw, RGBA_STRIPALPHA(rgb));
      }
   }

}

/*=============================================================================
FUNCTION: IDisplay_FillRoundedRectGradient

DESCRIPTION: GradientDecorator helper for filling a rectangle with a gradient.
             Call with rgbStart == rgbEnd for a fast solid color fill.

             First, the two strips inside the rounded corners are filled.  If
             a solid color fill is being performed, the top and bottom strips
             are filled in at the same time.  If a vertical gradient fill
             is being performed, the top and bottom strips are filled with
             a gradient.  If a horizontal gradient fill is being performed,
             the left and right strips are filled with a gradient.

             Last, the center of the rect is filled.  If a solid color fill
             is being performed, the rest of the rounded rect is filled with
             a single BlendRect() call.  If a gradient fill is performed, the
             interior is filled either top to bottom or left to right with
             a gradient pattern.

PARAMETERS:
   piDisplay   [in]: IDisplay to draw to
   rc          [in]: AEERect to fill
   nRadius     [in]: Radius of rounded corners to stay inside of
   rgbStart    [in]: Starting RGB value
   rgbEnd      [in]: Ending RGB value
   bVertical   [in]: TRUE for vertical fill, otherwise horizontal fill.
   bCenter     [in]: TRUE for center fill mode
   bAlpha      [in]: TRUE to enable alpha channel blending

RETURN VALUE:
   None

COMMENTS:
   Fills a rectangle with either a horizontal or vertical gradient pattern, 
   left-to-right or top to bottom, respectively, with the color changing from 
   rgbStart to rgbEnd.

   If rgbStart == rgbEnd, the gradient code is skipped and a fast solid
   color fill is performed.

SIDE EFFECTS:
   None.

SEE ALSO:
   None

=============================================================================*/

typedef struct _RoundedRectGradientInfo
{
   int nGradientWidth;
   int32 rs, gs, bs, as;
   int16 rinc, ginc, binc, ainc;
} RoundedRectGradientInfo;

// return the color of a scanline
static __inline void GetScanlineGradientColor(RoundedRectGradientInfo *pi, int wLine, RGBVAL *rgb, uint8 *nAlpha)
{
   int32 r,g,b,a;

   // center fill, and we've passed the center.
   // reverse gradient!
   if (wLine > pi->nGradientWidth) {
      wLine = pi->nGradientWidth * 2 - wLine + 1;
   }

   // calculate color for this line ...
   r = pi->rs + wLine * pi->rinc;
   g = pi->gs + wLine * pi->ginc;
   b = pi->bs + wLine * pi->binc;
   a = pi->as + wLine * pi->ainc;

   // return color and alpha separately
   *rgb = MAKE_RGB_FP8(r, g, b);
   *nAlpha = (a >> 8) & 0xff;
}


void IDisplay_FillRoundedRectGradient(IDisplay *piDisplay, const AEERect *prc, int nRadius, RGBVAL rgbStart, 
                                      RGBVAL rgbEnd, boolean bVertical, boolean bCenter, boolean bAlpha)
{
  AEERect rcDraw;
   int wScanLine;
   int wInset;
   int wAngle = 0;      // sweep from 0 to 90 degrees ...
   RGBVAL rgb = rgbStart;
   uint8 nAlpha = (uint8)((rgbStart) & 0xFF);
   int bGradient = (rgbStart != rgbEnd);
   RoundedRectGradientInfo rrgi;
   int32 re, ge, be, ae;

   // sanity check radius
   nRadius = MAX(1,MIN(nRadius, MIN(prc->dx/2, prc->dy/2)));  

   //
   // set up gradient
   //
   if (bGradient) {

      // extract RGB values ...
      EXTRACT_RGB_FP8(rgbStart, rrgi.rs, rrgi.gs, rrgi.bs, rrgi.as);
      EXTRACT_RGB_FP8(rgbEnd, re, ge, be, ae);

      // determine gradient width
      if (bVertical) {
         rrgi.nGradientWidth = prc->dy;
      } else {
         rrgi.nGradientWidth = prc->dx;
      }
      if (bCenter) {
         rrgi.nGradientWidth /= 2;
      }

      // get color and alpha increments
      rrgi.rinc = (int16) ((re - rrgi.rs) / rrgi.nGradientWidth);
      rrgi.ginc = (int16) ((ge - rrgi.gs) / rrgi.nGradientWidth);
      rrgi.binc = (int16) ((be - rrgi.bs) / rrgi.nGradientWidth);
      rrgi.ainc = (int16) ((ae - rrgi.as) / rrgi.nGradientWidth);
   }

   //
   // fill the two strips that lie inside the rounded rectangles
   //
   for (wScanLine = 1; wScanLine <= nRadius; wScanLine++) {

      // calculate the inset for this scanline
      wInset = nRadius - CircleGetX(&wAngle, (nRadius + 1), wScanLine);

      if (bGradient) {

         //
         // Perform a gradient fill
         //

         GetScanlineGradientColor(&rrgi, nRadius - wScanLine, &rgb, &nAlpha);
         if (bVertical) {
            // top
            SETAEERECT(&rcDraw, 
                       prc->x + wInset, 
                       prc->y + nRadius - wScanLine, 
                       prc->dx - 2 * (wInset), 
                       1);
         } else {
            // left
            SETAEERECT(&rcDraw, 
                       prc->x + nRadius - wScanLine, 
                       prc->y + wInset, 
                       1, 
                       prc->dy - 2 * (wInset));
         }
         BlendRect(piDisplay, &rcDraw, rgb, nAlpha);

         if (bVertical) {
            // bottom
            GetScanlineGradientColor(&rrgi, wScanLine + prc->dy - nRadius, &rgb, &nAlpha);
            SETAEERECT(&rcDraw, 
                       prc->x + wInset, 
                       prc->y + prc->dy - nRadius + wScanLine - 1, 
                       prc->dx - 2 * (wInset), 
                       1);
         } else {
            // right
            GetScanlineGradientColor(&rrgi, wScanLine + prc->dx - nRadius, &rgb, &nAlpha);
            SETAEERECT(&rcDraw, 
                       prc->x + prc->dx - nRadius + wScanLine - 1, 
                       prc->y + wInset, 
                       1, 
                       prc->dy - 2 * (wInset));
         }
         BlendRect(piDisplay, &rcDraw, rgb, nAlpha);

      } else {

         //
         // perform a solid color fill
         //

         // top
         SETAEERECT(&rcDraw, 
                    prc->x + wInset, 
                    prc->y + nRadius - wScanLine, 
                    prc->dx - 2 * (wInset), 
                    1);
         BlendRect(piDisplay, &rcDraw, rgb, nAlpha);

         // bottom
         SETAEERECT(&rcDraw, 
                    prc->x + wInset, 
                    prc->y + prc->dy - nRadius + wScanLine - 1, 
                    prc->dx - 2 * (wInset), 
                    1);
         BlendRect(piDisplay, &rcDraw, rgb, nAlpha);
         
      }
   }

   //
   // fill the center of the rounded rectangle
   //
   if (bGradient) {

      //
      // perform a gradient fill
      //
      if (bVertical) {

         // top to bottom
         for (--wScanLine; wScanLine < prc->dy - nRadius; wScanLine++) {
            GetScanlineGradientColor(&rrgi, wScanLine, &rgb, &nAlpha);
            SETAEERECT(&rcDraw, prc->x, prc->y + wScanLine, prc->dx, 1);
            BlendRect(piDisplay, &rcDraw, rgb, nAlpha);
         }

      } else {

         // left to right
         for (; wScanLine <= prc->dx - nRadius; wScanLine++) {
            GetScanlineGradientColor(&rrgi, wScanLine, &rgb, &nAlpha);
            SETAEERECT(&rcDraw, prc->x + wScanLine - 1, prc->y, 1, prc->dy);
            BlendRect(piDisplay, &rcDraw, rgb, nAlpha);
         }
     
      }
   } else {

      //
      // perform a solid color fill
      //
      SETAEERECT(&rcDraw, prc->x, prc->y + nRadius, prc->dx, prc->dy - 2*nRadius);
      BlendRect(piDisplay, &rcDraw, rgb, nAlpha);
   }

}

/*=============================================================================
FUNCTION: IDisplay_DrawStyledRectShadow

DESCRIPTION: Draw a shadow behind a beveled or rounded rectangle.

PARAMETERS:
   piDisplay   [in]: IDisplay to draw to
   prc               [in]: rect into which the shadow is to be drawn
   prcShadowCaster   [in]: rect of the object casting the shadow
   nRadius           [in]: Radius or Thickness of the object's borders
   rgb               [in]: shadow rgb color
   bRounded          [in]: TRUE if object has rounded corners

RETURN VALUE:
   None

COMMENTS:
   None.

SIDE EFFECTS:
   None.

SEE ALSO:
   None

=============================================================================*/

typedef void (*PFNSHADOWSCANLINEFUNC)(const AEERect *prc, int nRadius, int y, AEERect *prcScanLine);

static void RoundedRectShadowScanline(const AEERect *prc, int nRadius, int y, AEERect *prcScanLine)
{
   int wAngle = 0;
   int wInset;

   if (y < nRadius) {
      wInset = nRadius - CircleGetX(&wAngle, (nRadius + 1), nRadius - y);
   } else if (y > prc->dy - nRadius) {
      wInset = nRadius - CircleGetX(&wAngle, (nRadius + 1), y - prc->dy + nRadius + 1);
   } else {
      wInset = 0;
   }
   SETAEERECT(prcScanLine, 
              prc->x + wInset, 
              prc->y + y, 
              prc->dx - 2 * (wInset), 
              1);
}

static void BeveledRectShadowScanline(const AEERect *prc, int nRadius, int y, AEERect *prcScanLine)
{
   int wInset;

   if (y < nRadius) {
      wInset = nRadius - y;
   } else if (y >= prc->dy - nRadius) {
      wInset = y - prc->dy + nRadius + 1;
   } else {
      wInset = 0;
   }
   SETAEERECT(prcScanLine, 
              prc->x + wInset, 
              prc->y + y, 
              prc->dx - 2 * (wInset), 
              1);
}

void IDisplay_DrawStyledRectShadow(IDisplay *piDisplay, const AEERect *prc, const AEERect *prcShadowCaster, int nRadius, RGBVAL rgb, boolean bRounded)
{
   PFNSHADOWSCANLINEFUNC pfnScanLineAtY;
   int wScanLine;
   AEERect rcShadow;
   AEERect rcShadowCaster;
   AEERect rcDraw;
   uint8 nAlpha = (uint8)((rgb) & 0xFF);
   int yOffset = prc->y - prcShadowCaster->y;
   
   // how we compute the shadow scanlines depends on what
   // kind of rect the shadow caster is ...
   if (bRounded) {
      pfnScanLineAtY = RoundedRectShadowScanline;
   } else {
      pfnScanLineAtY = BeveledRectShadowScanline;
   }

   // sanity check radius
   nRadius = MAX(1,MIN(nRadius, MIN(prc->dx/2, prc->dy/2)));  

   for (wScanLine = 0; wScanLine < prc->dy; wScanLine++) {

      // build the current shadow scanline
      pfnScanLineAtY(prc, nRadius, wScanLine, &rcShadow);

      // build the current shadow caster scanline
     if ((prc->y + wScanLine >= prcShadowCaster->y) &&
         (prc->y + wScanLine < prcShadowCaster->y + prcShadowCaster->dy)) {   
         pfnScanLineAtY(prcShadowCaster, nRadius, wScanLine + yOffset, &rcShadowCaster);
      } else {
         // no overlap.
         SETAEERECT(&rcShadowCaster, 0, 0, 0, 0);
      }

      // Draw  the visible part of the shadow scanline
      // There may be sections visible at the beginning and at the end
      if (rcShadowCaster.dx) {
         if (rcShadow.x < rcShadowCaster.x) {
            rcDraw.x = rcShadow.x;
            rcDraw.y = rcShadow.y;
            rcDraw.dx = rcShadowCaster.x - rcShadow.x;
            rcDraw.dy = 1;
            BlendRect(piDisplay, &rcDraw, rgb, nAlpha);
         }

         if ((rcShadow.x + rcShadow.dx) > (rcShadowCaster.x + rcShadowCaster.dx)) {
            rcDraw.x = rcShadowCaster.x + rcShadowCaster.dx;
            rcDraw.y = rcShadow.y;
            rcDraw.dx = rcShadow.x + rcShadow.dx - rcShadowCaster.x - rcShadowCaster.dx;
            rcDraw.dy = 1;
            BlendRect(piDisplay, &rcDraw, rgb, nAlpha);
         }

      } else {
         // no overlap
         BlendRect(piDisplay, &rcShadow, rgb, nAlpha);
      } 
   }
}

//--------------------------------------------------------------------
//  MonoDIB methods
//--------------------------------------------------------------------

int CreateMonoDIB(IDisplay *piDisplay, IDIB **ppiDIB, uint16 width, uint16 height, 
                  boolean bAllocData)
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


void InitMonoDIBPalette(IDIB *piDIB, RGBVAL rgb)
{
   // Set color and flush palette
   piDIB->pRGB[1] = NTOHL(rgb);
   piDIB->pRGB[0] = ~piDIB->pRGB[1];
   piDIB->ncTransparent = 0;  // pRGB[0] is the transparent color by definition
   IDIB_FlushPalette(piDIB);
}


//--------------------------------------------------------------------
//  BlendAlpha
//--------------------------------------------------------------------


#define BITSPREAD(u,msk)     (((u) | ((u) << 16)) & (msk) )
#define SPREAD565     0x07e0f81f
#define SPREAD555     0x03e07c1f
#define BIAS565       0x02008010
#define BIAS555       0x02004010
#define DWADD(pw,i)   ((uint32*)( ((byte*)(pw)) + (i) ))
#define DWB(pw,i)     (*DWADD(pw,i))
#define WADD(pw,i)    ((uint16*)( ((byte*)(pw)) + (i) ))
#define WB(pw,i)      (*WADD(pw,i))

#define MASK666RB 0x0003F03F
#define MASK666G  0x00000FC0
#define BIAS666RB 0x00010010
#define BIAS666G  0x00000400
#define MASK888RB 0x00FF00FF
#define MASK888G  0x0000FF00
#define BIAS888RB 0x00100010
#define BIAS888G  0x00001000

// Reduce 0...255 alpha value to a 0...32 alpha
//
#define REDUCEALPHA(a)    (((a)*32 + 144) >> 8)

// handles IDIB_COLORSCHEME_555 and IDIB_COLORSCHEME_565
// 
// See documentation for DIB_AlphaBlit16 for more details.  Works
// the way, except here we're blending in a single color rather
// than a source image.
//
static int DIB_AlphaFill16(IDIB *me, int x, int cx, int y, int cy, NativeColor nc, int nAlpha)
{
   uint16 *pwRow  = (uint16*) (me->pBmp + y*me->nPitch + x*2);
   int cbRow      = cx * 2;
   int nAlphaLo   = REDUCEALPHA(nAlpha);

   if ((nc == 0 || nc == 0xFFFF) && (nAlphaLo == 16 || nAlphaLo == 24)) {

      // 50% or 75% black/white: simplified case much faster

      uint16 *pw = pwRow;
      uint32 uMask = 0x84108410;         // erase top bit of each field
      
      if (me->nColorScheme == IDIB_COLORSCHEME_555)
         uMask = 0x42104210;

      if (nc == 0) {

         // BLACK
         uMask = ~uMask;     // erase top bit

         do {
         
            for (pw = pwRow, y = cy; --y >= 0; pw = WADD(pw, me->nPitch)) {
               int i = cbRow;

               // right-most non-aligned word
               if ((((uint32)pw) + i) & 2) {
                  i -= 2;
                  WB(pw,i) = (WB(pw,i) >> 1 ) & (uint16) uMask;
               }
               // 32 bits at a time:  3 instructions/pixel
               while ( (i -= 4) >= 0) {
                  DWB(pw,i) = (DWB(pw,i) >> 1) & uMask;
               }
               // now:  i == -4 (done) or -2 (one more pixel)
               if ( (i += 2) == 0) {
                  WB(pw,i) = (WB(pw,i) >> 1 ) & (uint16) uMask;

               }
            }
            // repeat once to handle 75% black case
         } while ( (nAlphaLo -= 16) > 0);

      } else {

         // WHITE = same as black, but ORing instead of ANDing the top bit

         do {
            for (pw = pwRow, y = cy; --y >= 0; pw = WADD(pw, me->nPitch)) {
               int i = cbRow;
               if ((((uint32)pw) + i) & 2) {
                  i -= 2;
                  WB(pw,i) = (WB(pw,i) >> 1 ) | (uint16) uMask;
               }
               while ( (i -= 4) >= 0) {
                  DWB(pw,i) = (DWB(pw,i) >> 1) | uMask;
               }
               if ( (i += 2) == 0) {
                  WB(pw,i) = (WB(pw,i) >> 1 ) | (uint16) uMask;
               }
            }
         } while ( (nAlphaLo -= 16) > 0);
      }
      
   } else if (nAlphaLo != 0) {
   
      uint16 *pw = pwRow;
      register int nBeta32 = 32 - nAlphaLo;
      register uint32 uSrcProduct;
      register uint32 uMask = SPREAD565;
      uint32 uBias = BIAS565;

      if (me->nColorScheme == IDIB_COLORSCHEME_555) {
         uMask = SPREAD555;
         uBias = BIAS555;
      }
      
      uSrcProduct = BITSPREAD(nc,uMask) * nAlphaLo;
      uSrcProduct += uBias;

      for (y = cy; --y >= 0; pw = WADD(pw,me->nPitch) ) {
         int i = cbRow;

         while ( (i -= 2) >= 0) {
            uint32 u = WB(pw,i);
            u = BITSPREAD(u,uMask);        // construct three 10- or 11-bit fields
            u *= nBeta32;
            u += uSrcProduct;
            u = (u >> 5) & uMask;
            u = u | (u >> 16);
            WB(pw,i) = (uint16) u;
         }
      }
   }
   return 0;
}

#ifndef IDIB_COLORSCHEME_666
#define IDIB_COLORSCHEME_666  18   // 6 red, 6 green, 6 blue
#endif


// handles IDIB_COLORSCHEME_666 and IDIB_COLORSCHEME_888
//
// See documentation for DIB_AlphaBlit32 for more details. Works
// the way, except here we're blending in a single color rather
// than a source image.
//
static int DIB_AlphaFill32(IDIB *me, int x, int cx, int y, int cy, NativeColor nc, int nAlpha)
{
   uint32 *pwRow  = (uint32*) (me->pBmp + y*me->nPitch + x*4);
   int cbRow      = cx * 4;
   int nAlphaLo   = REDUCEALPHA(nAlpha);

   // TODO: add 50% and 75% alpha optimizations

   if (nAlphaLo != 0) {
   
      uint32 *pw = pwRow;
      register int nBeta32 = 32 - nAlphaLo;
      register uint32 uSrcProductrb;
      register uint32 uSrcProductg;
      uint32 maskrb = MASK666RB;
      uint32 maskg = MASK666G;
      uint32 biasrb = BIAS666RB;
      uint32 biasg = BIAS666G;

      if (me->nColorScheme == IDIB_COLORSCHEME_888) {
         maskrb = MASK888RB;
         maskg = MASK888G;
         biasrb = BIAS888RB;
         biasg = BIAS888G;
      }

      // red and blue channels
      uSrcProductrb = (nc & maskrb) * nAlphaLo;
      uSrcProductrb += biasrb;

      // green channel
      uSrcProductg = (nc & maskg) * nAlphaLo;
      uSrcProductg += biasg;

      for (y = cy; --y >= 0; pw = DWADD(pw,me->nPitch) ) {
         int i = cbRow;

         while ( (i -= 4) >= 0) {
            // get pixel
            uint32 u = DWB(pw,i);
            uint32 rb, g;

            // red and blue
            rb = u & maskrb;  
            rb *= nBeta32;
            rb += uSrcProductrb;
            rb = (rb >> 5) & maskrb;

            // green
            g = u & maskg;
            g *= nBeta32;
            g += uSrcProductg;
            g = (g >> 5) & maskg;

            // recombine rb and g
            u = rb | g;

            // set pixel
            DWB(pw,i) = u;
         }
      }
   }
   return 0;
}

// handles IDIB_COLORSCHEME_8888
//
// See documentation for DIB_AlphaBlit32 for more details. Works
// the way, except here we're blending in a single color rather
// than a source image. Format is BGRA not ARGB
//
static int DIB_AlphaFill32a(IDIB *me, int x, int cx, int y, int cy, NativeColor nc, int nAlpha)
{
   uint32 *pwRow  = (uint32*) (me->pBmp + y*me->nPitch + x*4);
   int cbRow      = cx * 4;
   int nAlphaLo   = REDUCEALPHA(nAlpha);
   uint32 srca;

   // TODO: add 50% and 75% alpha optimizations

   if (nAlphaLo != 0) {
   
      uint32 *pw = pwRow;
      register int nBeta32 = 32 - nAlphaLo;
      register uint32 uSrcProductrb;
      register uint32 uSrcProductg;
      uint32 maskrb = MASK888RB;
      uint32 maskg = MASK888G;
      uint32 biasrb = BIAS888RB;
      uint32 biasg = BIAS888G;


      srca = (nc & 0xff) * nAlphaLo;
      // red and blue channels
      nc = nc >> 8;
      uSrcProductrb = (nc & maskrb) << 5;
      uSrcProductrb += biasrb;

      // green channel
      uSrcProductg = (nc & maskg) << 5;
      uSrcProductg += biasg;

      for (y = cy; --y >= 0; pw = DWADD(pw,me->nPitch) ) {
         int i = cbRow;

         while ( (i -= 4) >= 0) {
            // get pixel
            uint32 u = DWB(pw,i);
            uint32 rb, g;
            uint32 a = u & 0xff;
            uint32 ua = (REDUCEALPHA(a) * nBeta32) >> 5;

            u = u >> 8;


            // red and blue
            rb = u & maskrb;  
            rb *= ua;
            rb += uSrcProductrb;
            rb = (rb >> 5) & maskrb;

            // green
            g = u & maskg;
            g *= ua;
            g += uSrcProductg;
            g = (g >> 5) & maskg;

            // recombine rb and g
            u = rb | g;
            u = u << 8;


            // Calculate new alpha component
            a = (srca + (a * nBeta32)  ) >> 5;
            
            u = u | a;

            // set pixel
            DWB(pw,i) = u;
         }
      }
   }
   return 0;
}


// Alpha blend a rect and a single color.  Performs clipping, then
// hands off to the right DIB_AlphaFillxxx() routine to do the
// actual blending.
//
static int DIB_AlphaFill(IDIB *me, const AEERect *prc, NativeColor nc, int nAlpha)
{
   int x = prc->x;
   int y = prc->y;
   int xMax = x + prc->dx;
   int yMax = y + prc->dy;
   int cx, cy;
   
   // clipping
   if (x < 0)         x = 0;
   if (xMax > me->cx) xMax = me->cx;
   if (y < 0)         y = 0;
   if (yMax > me->cy) yMax = me->cy;

   cx = xMax - x;
   cy = yMax - y;
   
   // blending
   if (cx > 0) {
      // 16 bit
      if (me->nColorScheme == IDIB_COLORSCHEME_555 || me->nColorScheme == IDIB_COLORSCHEME_565) 
         return DIB_AlphaFill16(me, x, cx, y, cy, nc, nAlpha);
      // 32 bit
      if (me->nColorScheme == IDIB_COLORSCHEME_666 || me->nColorScheme == IDIB_COLORSCHEME_888)
         return DIB_AlphaFill32(me, x, cx, y, cy, nc, nAlpha);
      if (me->nColorScheme == IDIB_COLORSCHEME_8888)
         return DIB_AlphaFill32a(me, x, cx, y, cy, nc, nAlpha);
   }
   return EFAILED;
}

// Obtain a DIB and invalidate a rectangle in it.  
//
int GetDIB(IBitmap *pib, const AEERect *prc, IDIB **ppdib)
{
   int nErr;

   nErr = IBITMAP_QueryInterface(pib, AEECLSID_DIB, (void**)ppdib);

#ifdef AEECLSID_DIB_20
   // We're compiling with 2.1+, where AEECLSID_DIB is non-invalidating
   // Try to invalidate if it succeeded, and try the old DIB if it failed
   if (nErr == SUCCESS) {
      // invalidate bitmap
      if (prc) {
         IBITMAP_Invalidate(pib, prc);
      }
   } else {
      nErr = IBITMAP_QueryInterface(pib, AEECLSID_DIB_20, (void**)ppdib);
   }
#endif

   return nErr;
}


// Blend a color with the contents of a rectangle.  Works on DIBs of colorscheme
// IDIB_COLORSCHEME_555, IDIB_COLORSCHEME_565, IDIB_COLORSCHEME_666 and
// IDIB_COLORSCHEME_888.
//
int BlendRect(IDisplay *pid, const AEERect *prc, RGBVAL rgb, int nAlpha)
{
   IBitmap *pb;
   IDIB *pdib;
   int nErr;
   AEERect rcClip, rcFill;

   if (nAlpha == 255) {
      IDISPLAY_FillRect(pid, prc, RGBA_STRIPALPHA(rgb));
      return SUCCESS;
   }

   // Since we purport to draw to IDisplay, we must honor its clipping.
   IDISPLAY_GetClipRect(pid, &rcClip);
   if (!IntersectRect(&rcFill, &rcClip, prc)) {
      return SUCCESS;
   }

   pb = IDISPLAY_GetDestination(pid);
   if (!pb) {
      return EUNSUPPORTED;
   }

   nErr = GetDIB(pb, &rcFill, &pdib);
   if (SUCCESS == nErr) {
      if (pdib->nColorScheme == IDIB_COLORSCHEME_555 ||
          pdib->nColorScheme == IDIB_COLORSCHEME_565 ||
          pdib->nColorScheme == IDIB_COLORSCHEME_666 ||
          pdib->nColorScheme == IDIB_COLORSCHEME_888 ||
          pdib->nColorScheme == IDIB_COLORSCHEME_8888) {
         NativeColor nc = IBITMAP_RGBToNative(pb, rgb);
         DIB_AlphaFill(pdib, &rcFill, nc, nAlpha);
      } else {
         // If the device can't do blending, then just treat as opaque
         IDISPLAY_FillRect(pid, prc, RGBA_STRIPALPHA(rgb));
         nErr = EUNSUPPORTED;
      }
      IDIB_Release(pdib);
   } else {
      // If the device can't do blending, then just treat as opaque
      IDISPLAY_FillRect(pid, prc, RGBA_STRIPALPHA(rgb));
   }
   IBITMAP_Release(pb);

   return nErr;
}

// Blending 5-5-5 and 5-6-5 data:
//
// R, G, B fields are spread out across a 32-bit value in 11- and 10-bit
// fields, so one multiplication can generate products for each component.
//
// Alpha and "beta" values are calcuated for the source and destination,
// respectively.  The 10- or 11-bit result for each color component is given
// by src*alpha + dest*beta.  This is then shifted right 5 bits to get the
// final result.
//
//   A) When alpha and beta add to 33, the maximum result for each component
//   will be 33 * 31 = 1023.  Since 1023 = 32*32 - 1, dividing by 32 gives a
//   value nicely distributed across the values 0...31.  However, 33*63 yields
//   a result larger than 32*64, so this cumulative factor of 33 is unsuitable
//   for 5-6-5 bitmaps.
//
//   B) When alpha and beta add to 32, 5-bit components fill the range 0...992
//   and six-bits fill the range 0...2016.  These results fill their
//   respective 10- and 11-bit fields with the exception of the higher 31
//   values, so a bias correction of 15 or 16 should be added to the values to
//   prevent a bias toward dark images.
//
// Strategy B leaves the minimum and maximum values less likely to be chosen
// by a factor of 50%, which is what a simple nearest-match algorithm would
// produce, so this is most likely a benefit, not an aberration.
//
// Alpha values from 0..32 *inclusive* are calculated from original 0..255
// values.  As with color values, this is done using a nearest-match
// algorithm, since that yields results very close to optimal (i.e. if 8-bit
// alpha values were used).
//
static int DIB_AlphaBlit16(IDIB *me, int x, int cx, int y, int cy, IDIB *pdibSrc, int xSrc, int ySrc, int nAlphaHi)
{
   uint32 uAlphaLo;
   
   nAlphaHi &= 255;
   uAlphaLo = REDUCEALPHA(nAlphaHi);

   // if (uAlphaLo == 32) DDBBLIT();

   if (cx > 0 && uAlphaLo != 0) {

      int cbRow = cx * 2;
      uint16 *pw    = (uint16*) (me->pBmp + y*me->nPitch + x*2);
      uint16 *pwSrc = (uint16*) (pdibSrc->pBmp + ySrc*pdibSrc->nPitch + xSrc*2);

      register uint32 uBetaLo = 32 - uAlphaLo;
      register uint32 uMask = SPREAD565;
      uint32 uBias = BIAS565;

      if (me->nColorScheme == IDIB_COLORSCHEME_555) {
         uMask = SPREAD555;
         uBias = BIAS555;
      }
         
      for (y = cy; --y >= 0;  ) {
         int i = cbRow;

         while ( (i -= 2) >= 0) {
            uint32 u, v;
               
            u = WB(pw,i);
            u = BITSPREAD(u,uMask);        // three 10- or 11-bit fields
            u *= uBetaLo;

            v = WB(pwSrc,i);
            v = BITSPREAD(v,uMask);
            v *= uAlphaLo;
                  
            u += v + uBias;
            u = (u >> 5) & uMask;
            u = u | (u >> 16);
            WB(pw,i) = (uint16) u;
         }
         pw = WADD(pw,me->nPitch);
         pwSrc = WADD(pwSrc, pdibSrc->nPitch);
      }
   }

   return SUCCESS;
}

// Blending 6-6-6 and 8-8-8 data:
//
// This works like DIB_AlphaBlit16(), only on 666 and 888 color schemes.
// The 555 and 565 cases handled by DIB_AlphaBlit16() involve spreading
// the bits of the color values across a 32bit integer so there's
// enough room (5 bits) between each of them to do the alpha blend
// calculations in a single pass.
//
// 32 bit color values can't be spread apart adequately to do the
// alpha blending using the same algorithm.  A 666 color value requires 
// 3*(5+6)=33 bits to spread far enough apart.  Similarly, an 888 color 
// value requires (3*(5+8))=39 bits.  Both color schemes require more room 
// than a uint32 provides.  
//
// So we'll isolate and blend the individual channels in separate steps.
// We isolate the red and blue channels with a mask and blend them
// simultaneously.  There's 6 or 8 bits between these two channels already
// and plenty of room in the msb of the uint32 color value, so there's no 
// need to shift bits around at all.  Once the red and blue channels are
// calculated, the green channel is isolated and blended in the same way.
// Combining the alpha blended results is a simple OR operation, since
// nothing's been shifted. 
//
// The alpha blend step involves calculating (d * alpha) + (s * (1-alpha))
// where alpha is a value from 0...1.  We opt to convert alpha values 
// to a range from 0...32 and then divide the result by 32 when we're 
// done.  This allows us to do the calculation with reasonable 
// precision using integers.  And it requires only 5 extra bits per
// channel.  Our calculation is actually:
//
// ((d * (alpha * 32)) + (s * (32 - alpha*32)))/32
//
// Since divsion rounds down, we factor in a small bias in each
// channel to spread the results better.
//
// The actual algorithm is as follows:
//
// Reduce alpha (0x00...0xFF) to a 5 bit value (0...32)
// for each pixel
//    get source pixel
//    get destination pixel
//    isolate red and blue channels
//    alpha blend them
//    isolate green channel
//    alpha blend it
//    combine results, store in destination
//
static int DIB_AlphaBlit32(IDIB *me, int x, int cx, int y, int cy, IDIB *pdibSrc, int xSrc, int ySrc, int nAlphaHi)
{
   uint32 uAlphaLo;
   
   nAlphaHi &= 255;
   uAlphaLo = REDUCEALPHA(nAlphaHi);

   // if (uAlphaLo == 32) DDBBLIT();

   if (cx > 0 && uAlphaLo != 0) {

      int cbRow = cx * 4;
      uint32 *pw    = (uint32*) (me->pBmp + y*me->nPitch + x*4);
      uint32 *pwSrc = (uint32*) (pdibSrc->pBmp + ySrc*pdibSrc->nPitch + xSrc*4);

      register uint32 uBetaLo = 32 - uAlphaLo;
      register uint32 maskrb = MASK666RB;
      register uint32 maskg = MASK666G;
      uint32 biasrb = BIAS666RB;
      uint32 biasg = BIAS666G;

      if (me->nColorScheme == IDIB_COLORSCHEME_888) {
         maskrb = MASK888RB;
         maskg = MASK888G;
         biasrb = BIAS888RB;
         biasg = BIAS888G;
      }

      for (y = cy; --y >= 0;  ) {
         int i = cbRow;

         while ( (i -= 4) >= 0) {
            // get pixels
            uint32 u = DWB(pw,i);
            uint32 v = DWB(pwSrc,i);
            uint32 urb, ug;
            uint32 vrb, vg;
               
            // red and blue
            urb = u & maskrb;
            urb *= uBetaLo;

            vrb = v & maskrb;
            vrb *= uAlphaLo;
                  
            urb += vrb + biasrb;
            urb = (urb >> 5) & maskrb;

            // green
            ug = u & maskg;
            ug *= uBetaLo;

            vg = v & maskg;
            vg *= uAlphaLo;
                  
            ug += vg + biasg;
            ug = (ug >> 5) & maskg;
   
            // recombine rb and g
            u = urb | ug;

            // set pixel
            DWB(pw,i) = u;
         }
         pw = DWADD(pw,me->nPitch);
         pwSrc = DWADD(pwSrc, pdibSrc->nPitch);
      }
   }

   return SUCCESS;
}


// Blend a 32bit DIB including alpha channel
static int DIB_AlphaBlit32a(IDIB *me, int x, int cx, int y, int cy, IDIB *pdibSrc, int xSrc, int ySrc, int nAlphaHi)
{
   uint32 uAlphaLo;
   
   nAlphaHi &= 255;
   uAlphaLo = REDUCEALPHA(nAlphaHi);

   // if (uAlphaLo == 32) DDBBLIT();

   if (cx > 0 && uAlphaLo != 0) {
 
      int cbRow = cx * 4;
      uint32 *pw    = (uint32*) (me->pBmp + y*me->nPitch + x*4);
      uint32 *pwSrc = (uint32*) (pdibSrc->pBmp + ySrc*pdibSrc->nPitch + xSrc*4);

      // Use the same -32 alpha calculation as everything else, and the same RGB masks
      // as IDIB_COLORSCHEME_888
      register uint32 uBetaLo = 32 - uAlphaLo;
      register uint32 maskrb = MASK888RB;
      register uint32 maskg = MASK888G;
      uint32 biasrb = BIAS888RB;
      uint32 biasg = BIAS888G;

      for (y = cy; --y >= 0;  ) {
         int i = cbRow;

         while ( (i -= 4) >= 0) {
            // get pixels
            uint32 u = DWB(pw,i);
            uint32 v = DWB(pwSrc,i);
            uint32 urb, ug;
            uint32 vrb, vg;
            uint32 ua, va;
            uint32 mua;
            
            ua = u & 0xff;
            va = v & 0xff;
            u = u >> 8;
            v = v >> 8;

            mua = (REDUCEALPHA(ua) * uBetaLo) >> 5;
           
            // red and blue
            urb = u & maskrb;
            urb *= mua;

            vrb = (v & maskrb) << 5;
                  
            urb += vrb + biasrb;
            urb = (urb >> 5) & maskrb;

            // green
            ug = u & maskg;
            ug *= mua;

            vg = (v & maskg) << 5;
                   
            ug += vg + biasg;
            ug = (ug >> 5) & maskg;
   
            // recombine rb and g
            u = urb | ug;

            u = u << 8;

            // Calculate new alpha component
            ua = ua * uBetaLo;
            va = va * uAlphaLo;
            ua += va;
            
            u = u | (ua >> 5);

            // set pixel
            DWB(pw,i) = u;
         }
         pw = DWADD(pw,me->nPitch);
         pwSrc = DWADD(pwSrc, pdibSrc->nPitch);
      }
   }

   return SUCCESS;
}

// Alpha blend two bitmpas.  Performs clipping, then
// hands off to the right DIB_AlphaBlitxxx() routine to do the
// actual blending.
//
static int DIB_AlphaBlit(IDIB *me, const AEERect *prc, IDIB *pdibSrc, int xSrc, int ySrc, int nAlphaHi)
{
   int x = prc->x;
   int y = prc->y;
   int xMax = x + prc->dx;
   int yMax = y + prc->dy;
   int cx, cy;
   
   // check dest
   if (x < 0)         { xSrc -= x; x = 0; }
   if (y < 0)         { ySrc -= y; y = 0; }
   if (xMax > me->cx) { xMax = me->cx; }
   if (yMax > me->cy) { yMax = me->cy; }

   // check src
   if (xSrc < 0)      { x -= xSrc; xSrc = 0; }
   if (ySrc < 0)      { y -= ySrc; ySrc = 0; }
   if (xMax > x + pdibSrc->cx - xSrc) { xMax = x + pdibSrc->cx - xSrc; }
   if (yMax > y + pdibSrc->cy - ySrc) { yMax = y + pdibSrc->cy - ySrc; }

   cx = xMax - x;
   cy = yMax - y;

   if (pdibSrc->nDepth != me->nDepth ||
       pdibSrc->nColorScheme != me->nColorScheme) {
      // Unsupported bit depth or colorscheme combination
      return EUNSUPPORTED;
   }

   // do the actual blend
   if (me->nColorScheme == IDIB_COLORSCHEME_555 || me->nColorScheme == IDIB_COLORSCHEME_565) 
      return DIB_AlphaBlit16(me, x, cx, y, cy, pdibSrc, xSrc, ySrc, nAlphaHi);
      
   if (me->nColorScheme == IDIB_COLORSCHEME_666 || me->nColorScheme == IDIB_COLORSCHEME_888)
      return DIB_AlphaBlit32(me, x, cx, y, cy, pdibSrc, xSrc, ySrc, nAlphaHi);
   if (me->nColorScheme == IDIB_COLORSCHEME_8888)
      return DIB_AlphaBlit32a(me, x, cx, y, cy, pdibSrc, xSrc, ySrc, nAlphaHi);
        

   return EFAILED;
}

// Alpha Blend two bitmaps.  Works on DIBs of colorscheme
// IDIB_COLORSCHEME_555, IDIB_COLORSCHEME_565, IDIB_COLORSCHEME_666 and
// IDIB_COLORSCHEME_888.
//
int BlendBlit(IBitmap *pibDest, const AEERect *prcDest, IBitmap *pibSrc, int xSrc, int ySrc, int nAlpha)
{
   IDIB *pdibDest, *pdibSrc;
   int nErr = EUNSUPPORTED;

   if (nAlpha != 255) {
      if (SUCCESS == GetDIB(pibDest, prcDest, &pdibDest)) {
         if (SUCCESS == GetDIB(pibSrc, NULL, &pdibSrc)) {
            nErr = DIB_AlphaBlit(pdibDest, prcDest, pdibSrc, xSrc, ySrc, nAlpha);
            IDIB_Release(pdibSrc);
         }
         IDIB_Release(pdibDest);
      }
   }

   if (nErr != SUCCESS) {
      nErr = IBITMAP_BltIn(pibDest, prcDest->x, prcDest->y, prcDest->dx, prcDest->dy,
                           pibSrc, xSrc, ySrc, AEE_RO_COPY);
   }

   return nErr;
}



IFont *IDISPLAY_GetFont(IDisplay *piDisplay, AEEFont fontId)
{
   IFont *piFont;

   piFont = IDISPLAY_SetFont(piDisplay, fontId, 0);
   IDISPLAY_SetFont(piDisplay, fontId, piFont);
   IFONT_AddRef(piFont);
   return piFont;
}


// returns required size in bytes
int IShell_GetResStringSize(IShell *piShell, const char *pszFile, uint16 idRes)
{
   uint32 cbBufsize = 0;
   ISHELL_LoadResDataEx(piShell, pszFile, idRes, RESTYPE_STRING, (void*)-1, &cbBufsize);
   cbBufsize *= sizeof(AECHAR);
   return (int)cbBufsize;
}


int ISHELL_CreateFont(IShell *piShell, AEECLSID fontClass, IFont **ppo)
{
   int result;
   IFont *piFont = 0;
   IDisplay *piDisplay = 0;

   if (!piShell)
      return EBADPARM;

   result = ISHELL_CreateInstance(piShell, fontClass, (void **)&piFont);

   if (result == ECLASSNOTSUPPORT 
       && ISHELL_CreateInstance(piShell, AEECLSID_DISPLAY, (void **)&piDisplay) == 0) {

      switch(fontClass) {
      case AEECLSID_FONTSYSNORMAL:
         piFont = IDISPLAY_GetFont(piDisplay, AEE_FONT_NORMAL);
         result = 0;
         break;
      case AEECLSID_FONTSYSBOLD:
         piFont = IDISPLAY_GetFont(piDisplay, AEE_FONT_BOLD);
         result = 0;
         break;
      case AEECLSID_FONTSYSLARGE:
         piFont = IDISPLAY_GetFont(piDisplay, AEE_FONT_LARGE);
         result = 0;
         break;
	  case AEECLSID_FONTSYSITALIC:
	  case AEECLAID_FONTSMALL:
         piFont = IDISPLAY_GetFont(piDisplay, AEE_FONT_SMALL);
         result = 0;
         break;	 
      }
   }

   *ppo = piFont;
   RELEASEIF(piDisplay);
   return result;
}


void IDisplay_DrawRectangleEx(IDisplay *me, const AEERect *prc, RGBVAL rgbFrame, RGBVAL rgbFill, boolean bRounded)
{
   AEERect rc;

   rc.x = prc->x+1;
   rc.y = prc->y+1;
   rc.dx = prc->dx-2;
   rc.dy = prc->dy-2;

   if (rgbFill != RGB_NONE) {
      IDISPLAY_FillRect(me, &rc, RGBA_STRIPALPHA(rgbFill));
   }

   if (rgbFrame != RGB_NONE) {
      AEERect rcFrame;

      if (!bRounded) {
         rc = *prc;       // rc = wid & hgt of border lines
      }

      rcFrame = rc;
      rcFrame.y = prc->y;
      rcFrame.dy = 1;
      IDISPLAY_FillRect(me, &rcFrame, RGBA_STRIPALPHA(rgbFrame));

      rcFrame.y += prc->dy - 1;
      IDISPLAY_FillRect(me, &rcFrame, RGBA_STRIPALPHA(rgbFrame));

      rcFrame = rc;
      rcFrame.x = prc->x;
      rcFrame.dx = 1;
      IDISPLAY_FillRect(me, &rcFrame, RGBA_STRIPALPHA(rgbFrame));

      rcFrame.x += prc->dx - 1;
      IDISPLAY_FillRect(me, &rcFrame, RGBA_STRIPALPHA(rgbFrame));
   }
}


void IImage_DrawClipped(IImage * pii, IDisplay *piDisplay, int x, int y, AEERect *prcImage, AEERect * prcClip)
{
   AEERect rc;
   AEERect rci;

   // if source rectangle not specified, use full image size
   if (!prcImage) {
      AEEImageInfo aii;
      IIMAGE_GetInfo(pii, &aii);
      SETAEERECT(&rci, 0, 0, aii.cx, aii.cy);
      prcImage = &rci;
   }

   // set the screen rectangle of the image
   SETAEERECT(&rc, x, y, prcImage->dx, prcImage->dy);

   // find intersection w/ clipping rect
   if (IntersectRect(&rc, &rc, prcClip)) {
      int xoff = prcImage->x;
      int yoff = prcImage->y;
      if (x < rc.x) 
         xoff += (rc.x - x);
      if (y < rc.y)
         yoff += (rc.y - y);

#ifndef IPARM_DISPLAY
#define IPARM_DISPLAY   8
#endif
      // same as IIMAGE_SetDisplay(pii, piDisplay)
      IIMAGE_SetParm(pii, IPARM_DISPLAY, (int)(piDisplay), 0);
      IIMAGE_SetOffset(pii, xoff, yoff);
      IIMAGE_SetDrawSize(pii, rc.dx, rc.dy);
      IIMAGE_DrawFrame(pii, -1, rc.x, rc.y);
   }
}

void IImage_DrawImage(IImage *pii, IDisplay *piDisplay, int x, int y, AEERect *rect)
{
   IIMAGE_SetParm(pii, IPARM_DISPLAY, (int)(piDisplay), 0);
   IIMAGE_SetOffset(pii, rect->x, rect->y);
   IIMAGE_SetDrawSize(pii, rect->dx, rect->dy);
   IIMAGE_DrawFrame(pii, -1, x, y);
}


void IImage_DrawTiled(IImage *pii, IDisplay *piDisplay, int x, int y, AEERect *prcImage, AEERect *prcClip)
{
   uint16 iXWidth, iYWidth;
   uint16 i, j, ix, iy;

   // don't divide by zero and save time if the image wouldn't display anyway
	 if(prcImage->dx <= 0 || prcImage->dy <= 0 ||
	    prcClip->dx <= 0 || prcClip->dy <= 0)
      return; 

   iXWidth = (prcClip->dx / prcImage->dx)+1;      //better to overdraw than to under.  clip will exclude extra
   iYWidth = (prcClip->dy / prcImage->dy)+1;
   ix = prcImage->x;
   iy = prcImage->y;
   
   for (i = 0; i < iXWidth; i++) {
         ix = i * prcImage->dx + x;
      for (j = 0; j < iYWidth; j++) {
         iy = j * prcImage->dy + y;
         IImage_DrawClipped(pii, piDisplay, ix, iy, prcImage, prcClip);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// tiled image drawing

void TiledImage_SetBitmap(TiledImage *me, IBitmap *piBitmap, int nNumTiles)
{
   RELEASEIF(me->piBitmap);

   me->piBitmap = piBitmap;

   ADDREFIF(me->piBitmap);

   FREEIF(me->pRGB); // free copy of the palette

   if (me->piBitmap) {
      AEEBitmapInfo abi;
      NativeColor nc;
      nc = IBITMAP_RGBToNative(me->piBitmap, MAKE_RGB(255,0,255));
      IBITMAP_SetTransparencyColor(me->piBitmap, nc);

      IBITMAP_GetInfo(me->piBitmap, &abi, sizeof(abi));
      me->cxTile     = abi.cx / nNumTiles;
      me->cyTile     = abi.cy;
      me->nNumTiles  = nNumTiles;
   }
}

int TiledImage_LoadResBitmap(TiledImage *me, IShell *piShell, const char *pszResFile, uint16 idRes, int nNumTiles)
{
   IBitmap *pib = WidgetLib_LoadResBitmap(piShell, pszResFile, idRes);
   if (!pib) {
      return ERESOURCENOTFOUND;
   }
   TiledImage_SetBitmap(me, pib, nNumTiles);
   IBITMAP_Release(pib);

   return SUCCESS;
}


// usage: 
// 
//    when IDF_ALIGN_CENTER or IDF_ALIGN_RIGHT are specified, dx is used
//    to calculate the x origin of the destination using the rectangle defined
//    by x and dx. 
//    
//    when IDF_ALIGN_MIDDLE or IDF_ALIGN_BOTTOM are specified, dy is used
//    to calculate the y origin of the destination using the rectangle defined
//    by y and dy. 
//    
//     example:
//    
//     IDF_ALIGN_RIGHT | IDF_ALIGN_BOTTOM
//        
//    x,y     dx
//     +-------------+
//     |             |
//     |     +-------| dy
//     |     |image  |
//     |     | tile  |
//     +-----+-------+
//
int TiledImage_DrawTileEx(TiledImage *me, IDisplay *piDisplay, int x, int y, int nTile,
                          int dx, int dy, uint32 dwAlign)
{
   if (me->piBitmap && (nTile >= 0) && (nTile < me->nNumTiles)) {
      
      if (dx && (dwAlign & IDF_ALIGNHORZ_MASK)) {
         if (dwAlign & IDF_ALIGN_RIGHT) {
            x += dx - me->cxTile;
         } else if (dwAlign & IDF_ALIGN_CENTER) {
            x += (dx - me->cxTile) / 2;
         }
      }

      if (dy && (dwAlign & IDF_ALIGNVERT_MASK)) {
         if (dwAlign & IDF_ALIGN_BOTTOM) {
            y += dy - me->cyTile;
         } else if (dwAlign & IDF_ALIGN_MIDDLE) {
            y += (dy - me->cyTile) / 2;
         }  
      }

      {
         AEERect rc, rcClip, rcSrc;
         IBitmap *pibDest = 0;

         // create the destination rectangle
         SETAEERECT(&rc, x, y, me->cxTile, me->cyTile);

         // get the clip rect from the IDisplay
         IDISPLAY_GetClipRect(piDisplay, &rcClip);

         // intersect with the destination rectangle
         IntersectRect(&rcClip, &rc, &rcClip);

         // normalize to bitmap dimension by subtracting x,y
         rcSrc = rcClip;
         rcSrc.x -= x;
         rcSrc.y -= y;

         rcSrc.x += (me->cxTile * nTile);

         pibDest = IDISPLAY_GetDestination(piDisplay);
         if (pibDest) {
            // do a bltIn into the IDisplay's bitmap
            IBITMAP_BltIn(pibDest, rcClip.x, rcClip.y, rcSrc.dx, rcSrc.dy, 
                          me->piBitmap, rcSrc.x, rcSrc.y, 
                          AEE_RO_TRANSPARENT);

            RELEASEIF(pibDest);
         }
      }

      return SUCCESS;
   }
   return EFAILED;
}

int TiledImage_DrawTile(TiledImage *me, IDisplay *piDisplay, int x, int y, int nTile)
{
   return TiledImage_DrawTileEx(me, piDisplay, x, y, nTile, 0, 0, 0);
}


void TiledImage_Dtor(TiledImage *me)
{
   RELEASEIF(me->piBitmap);
   RELEASEIF(me->piDib);
   FREEIF(me->pRGB);
}


int TiledImage_ReplaceColor(TiledImage *me, RGBVAL rgbOld, RGBVAL rgbNew)
{
   int nErr = 0;

   if (me->piBitmap) {

      if (!me->piDib) {
         // is our bitmap a DIB?
         nErr = IBITMAP_QueryInterface(me->piBitmap, AEEIID_DIB, (void**)&me->piDib);
      }
      
      if (!nErr && !me->pRGB && me->piDib && me->piDib->cntRGB) {
         // make a copy of the palette in the DIB
         me->pRGB = MALLOC(me->piDib->cntRGB * sizeof(uint32*));
         if (!me->pRGB) {
            nErr = ENOMEMORY;
         } else {
            MEMCPY(me->pRGB, me->piDib->pRGB, me->piDib->cntRGB * sizeof(uint32*));
         }
      }
      
      if (!nErr && me->pRGB) {
         NativeColor nc;

         if (me->rgbLast == rgbOld) {
            nc = me->ncLast;
         } else {
            // first replace palette with original (unmodified) copy of palette
            uint32 *pRGBSave = me->piDib->pRGB;
            me->piDib->pRGB = me->pRGB;
            nc = IBITMAP_RGBToNative(me->piBitmap, rgbOld);
            me->piDib->pRGB = pRGBSave;    // swap back modified palette
         }
         // modify the palette entry
         if (nc < me->piDib->cntRGB) {
            me->piDib->pRGB[nc] = NTOHL(rgbNew);
         }
         IDIB_FlushPalette(me->piDib);
      }
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////

AECHAR IShell_GetEllipsis(IShell *piShell)
{
   AEEDeviceInfo di;
   ISHELL_GetDeviceInfo(piShell, &di);
   if (di.wEncoding == AEE_ENC_UNICODE) {
      return 0x2026;
   } else if (di.wEncoding == AEE_ENC_S_JIS) {
      return 0x8163;
   }
   return (AECHAR)' ';
}



/////////////////////////////////////////////////////////////////////////////
// Mem grow/append

int MemGrowEx(void **ppMem, int *pcb, int cbUsed, int cbMore, int cbAllocExtra)
{
   void *pMem = *ppMem;
   int cb     = *pcb;
   int cbFree = cb - cbUsed;
   
   if (cbFree < cbMore) {

      cb += cbMore - cbFree + cbAllocExtra;

      pMem = REALLOC(pMem, cb);
      if (!pMem) {
         return ENOMEMORY;
      }
   
      *ppMem = pMem;
      *pcb   = cb;
   }

   return SUCCESS;
}


#define PVINDEX(pv,n)  (void*)((byte*)(pv) + (n))

int MemAppendEx(void **ppMem, int *pcb, int *pcbUsed, void *pAppend, int cbAppend, int cbAllocExtra)
{
   int nErr;
   int cbUsed = *pcbUsed;

   nErr = MemGrowEx(ppMem, pcb, cbUsed, cbAppend, cbAllocExtra);

   if (!nErr) {
      MEMCPY(PVINDEX(*ppMem,cbUsed), pAppend, cbAppend);
      *pcbUsed += cbAppend;
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// Mem miscellaneous utils


int Mem_LoadResString(void **ppMem, int *pcb, int *pcbUsed, IShell *piShell, 
                      const char *pszResFile, uint16 idRes)
{
   int nErr = SUCCESS;
   int cbUsed = *pcbUsed;

   int cb = IShell_GetResStringSize(piShell, pszResFile, idRes);
   
   if (!cb) {
      nErr = ERESOURCENOTFOUND;
   } else {
      nErr = MemGrow(ppMem, pcb, cbUsed, cb);
   }

   if (!nErr) {
      ISHELL_LoadResString(piShell, pszResFile, idRes, PVINDEX(*ppMem,cbUsed), cb);
      *pcbUsed += cb;
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// Determine how a paragraph of text should be aligned.  Assumes pszText
// contains Unicode characters
uint32 DetermineTextAlignment(IFontBidiUtil *pIFontBidiUtil, AECHAR *pwText, int dwLen)
{
   if (IFONTBIDIUTIL_Characterize(pIFontBidiUtil, pwText, dwLen, 0, NULL) == BIDICHARTYPECAT_STRONGRIGHT)
      return (IDF_ALIGN_RIGHT);
   else
      return (IDF_ALIGN_LEFT);
}

/////////////////////////////////////////////////////////////////////////////
// GetFontBidiUtil - get the FontBidiUtil object that will allow us to
// perform various calculations for displyed bidirectional text.
extern int FontBidiUtil_New(IShell *pIShell, void **ppObj);
IFontBidiUtil *GetFontBidiUtil(IShell *pIShell)
{
   IFontBidiUtil *pifbu = NULL;
   
   // try to instantiate the default IFontBidiUtil
   FontBidiUtil_New(pIShell, (void **)&pifbu);
   return (pifbu);    
}

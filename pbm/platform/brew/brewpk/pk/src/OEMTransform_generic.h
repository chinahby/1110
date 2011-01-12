/*=====================================================================
FILE:  OEMTransform_generic.h

SERVICES:  OEMTransform reference implementation

DESCRIPTION:
   Bit depth independent reference ITransform implementation.  The
   implementation in this file is built on the macros defined in
   OEMBitmapN.c, where N is the bit depth.  The file is included
   from OEMBitmap_generic.h.

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/


/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

static int OEMTransform_TransformBltSimple(ITransform *po, int xDst, int yDst,
                                           IBitmap *pSrc, int xSrc, int ySrc,
                                           unsigned dxSrc, unsigned dySrc,
                                           uint16 unTransform, uint8 unComposite);
static int OEMTransform_TransformBltComplex(ITransform *po, int xDst, int yDst,
                                            IBitmap *pSrc, int xSrc, int ySrc,
                                            unsigned dxSrc, unsigned dySrc,
                                            const AEETransformMatrix *pMatrixTransform,
                                            uint8 unComposite);

static const VTBL(ITransform) gOEMTransformFuncs = {
   AEEBASE_AddRef(ITransform), // AEEBase versions defer to OEMBitmap_xxx
   AEEBASE_Release(ITransform),
   AEEBASE_QueryInterface(ITransform),
   OEMTransform_TransformBltSimple,
   OEMTransform_TransformBltComplex
};



/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

static int
OEMTransform_TransformBltComplex(ITransform *po, int xDst, int yDst,
                                 IBitmap *pSrc, int xSrc, int ySrc,
                                 unsigned dxSrc, unsigned dySrc,
                                 const AEETransformMatrix *pMatrixTransform,
                                 uint8 unComposite)
{
   OEMBitmap *pMe    = ((OEMTransform*)po)->pMe;

#if defined(TRANSFORM_COMPLEX)
   return TRANSFORM_COMPLEX(pMe, xDst, yDst, pSrc, xSrc, ySrc, dxSrc, dySrc, pMatrixTransform, unComposite);
#else
   if (AEEGETPVTBL(pSrc, IBitmap) == pMe->m_dib.pvt) {
      OEMBitmap *pbmSrc = (OEMBitmap*)pSrc;
      uint16 i, j;
      int32 determinant;
      int32 A;
      int32 B;
      int32 C;
      int32 D;
      int32 xStart, xEnd, yStart, yEnd;
      int32 xBound[4];
      int32 yBound[4];

      // First, we invert the matrix.  This is done because we will do the
      // transformations backwards, from destination coordinates to source
      // coordinates.

      determinant = ((int32)pMatrixTransform->A * (int32)pMatrixTransform->D - (int32)pMatrixTransform->B * (int32)pMatrixTransform->C) >> 8;

      if (determinant == 0) {
         // Matrix is not invertible
         return EBADPARM;
      }

      // [ A B
      //   C D ] represents the inverted transformation matrix.
      A = ( (int32)pMatrixTransform->D << 8) / determinant;
      B = (-(int32)pMatrixTransform->B << 8) / determinant;
      C = (-(int32)pMatrixTransform->C << 8) / determinant;
      D = ( (int32)pMatrixTransform->A << 8) / determinant;

      // Now we find the bounding box of the bitmap in destination coordinates.
      
      // Find all of the X coordinates (the four corners of the bitmap)
      xBound[0] = (((int32)pMatrixTransform->A * -((int32)dxSrc / 2) + (int32)pMatrixTransform->B * -((int32)dySrc / 2)) >> 8) + xDst + dxSrc / 2;
      xBound[1] = (((int32)pMatrixTransform->A * -((int32)dxSrc / 2) + (int32)pMatrixTransform->B * ((int32)dySrc / 2)) >> 8) + xDst + dxSrc / 2;
      xBound[2] = (((int32)pMatrixTransform->A * ((int32)dxSrc / 2) + (int32)pMatrixTransform->B * ((int32)dySrc / 2)) >> 8) + xDst + dxSrc / 2;
      xBound[3] = (((int32)pMatrixTransform->A * ((int32)dxSrc / 2) + (int32)pMatrixTransform->B * -((int32)dySrc / 2)) >> 8) + xDst + dxSrc / 2;

      // Find the minimum X coordinate
      for (i = 0, xStart = pMe->m_dib.cx; i < 4; i++) {
         if (xBound[i] < xStart) {
            xStart = xBound[i];
         }
      }

      // Find the maximum X coordinate
      for (i = 0, xEnd = 0; i < 4; i++) {
         if (xBound[i] > xEnd) {
            xEnd = xBound[i];
         }
      }

      // Find all of the Y coordinates (the four corners of the bitmap)
      yBound[0] = (((int32)pMatrixTransform->C * -((int32)dxSrc / 2) + (int32)pMatrixTransform->D * -((int32)dySrc / 2)) >> 8) + yDst + dySrc / 2;
      yBound[1] = (((int32)pMatrixTransform->C * -((int32)dxSrc / 2) + (int32)pMatrixTransform->D * ((int32)dySrc / 2)) >> 8) + yDst + dySrc / 2;
      yBound[2] = (((int32)pMatrixTransform->C * ((int32)dxSrc / 2) + (int32)pMatrixTransform->D * ((int32)dySrc / 2)) >> 8) + yDst + dySrc / 2;
      yBound[3] = (((int32)pMatrixTransform->C * ((int32)dxSrc / 2) + (int32)pMatrixTransform->D * -((int32)dySrc / 2)) >> 8) + yDst + dySrc / 2;

      // Find the minimum Y coordinate
      for (i = 0, yStart = pMe->m_dib.cy; i < 4; i++) {
         if (yBound[i] < yStart) {
            yStart = yBound[i];
         }
      }

      // Find the maximum Y coordinate
      for (i = 0, yEnd = 0; i < 4; i++) {
         if (yBound[i] > yEnd) {
            yEnd = yBound[i];
         }
      }

      // Clip the bounding box to the boundries of the destination bitmap.
      if (IS_OUTSIDE_DIRTY_RECT(pMe, xStart, yStart, xEnd - xStart + 1, yEnd - yStart + 1)) {
         if (xStart < 0) {
            xStart = 0;
         }
         if (yStart < 0) {
            yStart = 0;
         }
         if (xEnd >= pMe->m_dib.cx) {
            xEnd = pMe->m_dib.cx - 1;
         }
         if (yEnd >= pMe->m_dib.cy) {
            yEnd = pMe->m_dib.cy - 1;
         }
         if (xStart > xEnd || yStart > yEnd) {
            // nothing to draw
            return SUCCESS;
         }
         EXPAND_DIRTY_RECT(pMe, xStart, yStart, xEnd - xStart + 1, yEnd - yStart + 1);
      } else if (xStart > xEnd || yStart > yEnd) {
         // nothing to draw
         return SUCCESS;
      }


      {
#if defined(PIXELITER_SETUP)
         PIXELITER_SETUP(pMe, xStart, yStart, 0, 0, 0);
#endif

         // This is where the pixels are actually copied
      
         if (unComposite != COMPOSITE_KEYCOLOR) {

            // Opaque blit.  Don't have to worry about key color.

            // Iterate through destination pixels
            for (j = (uint16)yStart; j <= yEnd; j++) {
               for (i = (uint16)xStart; i <= xEnd; i++) {
                  uint16 x;
                  uint16 y;

                  // Transform destination coordinates, so that the origin
                  // is at the center of where the copied part of the source
                  // bitmap would be for an untransformed blit.
                  int32 nX1 = i - xDst - dxSrc / 2;
                  int32 nY1 = j - yDst - dySrc / 2;

                  // Apply reverse transformation to coordinate of pixel
                  // in the source bitmap to copy.
                  int32 nXNum = A * nX1 + B * nY1;
                  int32 nYNum = C * nX1 + D * nY1;
                  x = (uint16)((nXNum >> 8) + dxSrc / 2);
                  y = (uint16)((nYNum >> 8) + dySrc / 2);

                  // Verify that the coordinate is within the bounds of
                  // the area that we are copying from the source.
                  if (x < dxSrc && y < dySrc) {
#if defined(PIXELITER_SETUP)
                     PIXELITER_SET(GETPIXEL(pbmSrc, xSrc + x, ySrc + y));
#else
                     SETPIXEL(pMe, i, j, GETPIXEL(pbmSrc, xSrc + x, ySrc + y));
#endif
                  }
#if defined(PIXELITER_SETUP)
                  PIXELITER_NEXTX();
#endif
               }
#if defined(PIXELITER_SETUP)
               PIXELITER_NEXTY();
#endif
            }

         } else {

            // Transparent blit.  Must compare each pixel in source
            // to key color.

            NativeColor transparent = pbmSrc->m_dib.ncTransparent;

            // Iterate through destination pixels
            for (j = (uint16)yStart; j <= yEnd; j++) {
               for (i = (uint16)xStart; i <= xEnd; i++) {
                  uint16 x;
                  uint16 y;

                  // Transform destination coordinates, so that the origin
                  // is at the center of where the copied part of the source
                  // bitmap would be for an untransformed blit.
                  int32 nX1 = i - xDst - dxSrc / 2;
                  int32 nY1 = j - yDst - dySrc / 2;

                  // Apply reverse transformation to coordinate of pixel
                  // in the source bitmap to copy.
                  int32 nXNum = A * nX1 + B * nY1;
                  int32 nYNum = C * nX1 + D * nY1;
                  x = (uint16)((nXNum >> 8) + dxSrc / 2);
                  y = (uint16)((nYNum >> 8) + dySrc / 2);

                  // Verify that the coordinate is within the bounds of
                  // the area that we are copying from the source.
                  if (x < dxSrc && y < dySrc) {
                     NativeColor pixel = GETPIXEL(pbmSrc, xSrc + x, ySrc + y);

                     // Copy only if the pixel is not transparent.
                     if (pixel != transparent) {
#if defined(PIXELITER_SETUP)
                        PIXELITER_SET(pixel);
#else
                        SETPIXEL(pMe, i, j, pixel);
#endif
                     }

                  }
#if defined(PIXELITER_SETUP)
                  PIXELITER_NEXTX();
#endif
               }
#if defined(PIXELITER_SETUP)
               PIXELITER_NEXTY();
#endif
            }

         }

      }
         
      return SUCCESS;
   }

   return EUNSUPPORTED;
#endif //TRANSFORM_COMPLEX
}

static int
OEMTransform_TransformBltSimple(ITransform *po, int xDst, int yDst,
                                IBitmap *pSrc, int xSrc, int ySrc,
                                unsigned dxSrc, unsigned dySrc,
                                uint16 unTransform, uint8 unComposite)
{
   OEMBitmap *pMe    = ((OEMTransform*)po)->pMe;

#if defined(TRANSFORM_SIMPLE)
   return TRANSFORM_SIMPLE(pMe, xDst, yDst, pSrc, xSrc, ySrc, dxSrc, dySrc, unTransform, unComposite);
#else
   if (unTransform == 0) {
      // no transformation, call standard bitblt routine because it
      // should be faster
      return IBITMAP_BltIn(OEMBITMAP_TO_IBITMAP(pMe), xDst, yDst, dxSrc, dySrc, pSrc, xSrc, ySrc,
                           unComposite == COMPOSITE_KEYCOLOR ? AEE_RO_TRANSPARENT : AEE_RO_COPY);
   }

   if (AEEGETPVTBL(pSrc, IBitmap) == pMe->m_dib.pvt) {
      OEMBitmap *pbmSrc = (OEMBitmap*)pSrc;

      int dx = dxSrc;
      int dy = dySrc;

      int i, j;
      int nDstPitch = pMe->m_dib.cx;
      int nSrcPitch = pbmSrc->m_dib.cx;
      int nSrcDownAdjust = nSrcPitch;
      int nSrcRightAdjust = 1;
      int nDst, nDst2, nSrc, nSrc2;
      NativeColor ncTransparent = pbmSrc->m_dib.ncTransparent;
      int nScaleNum = 1;
      int nScaleDen = 1;
      int xNum, yNum;
      int nTmp;

      // Clip to source
      if (xSrc < 0) {
         dx += xSrc;
         xSrc = 0;
      }
      if (ySrc < 0) {
         dy += ySrc;
         ySrc = 0;
      }
      if (xSrc + dx > pbmSrc->m_dib.cx) {
         dx = pbmSrc->m_dib.cx - xSrc;
      }
      if (ySrc + dy > pbmSrc->m_dib.cy) {
         dy = pbmSrc->m_dib.cy - ySrc;
      }

      nSrc = xSrc + ySrc * nSrcDownAdjust;

      // Here's where we take care of the rotation and reflaction
      // raster ops.  We handle these be changing the order in
      // which we iterate through the pixels in the source bitmap.

      if (unTransform & 0x04) {
         // reflect over x axis
         nSrc += nSrcDownAdjust * (dySrc - 1);
         nSrcDownAdjust = -nSrcDownAdjust;
      }

      switch (unTransform & 0x03) {
      case 0x00:
         // no rotation
         break;
      case 0x01:
         // rotate 90 degrees counter clockwise
         nSrc += dxSrc - 1;
         nSrcRightAdjust = nSrcDownAdjust;
         nSrcDownAdjust = -1;
         nTmp = (dx - dy) / 2;
         xDst += nTmp;
         yDst -= nTmp;
         nTmp = dx;
         dx = dy;
         dy = dxSrc;
         break;
      case 0x02:
         // rotate 180 degrees counter clockwise
         nSrc += nSrcDownAdjust * (dySrc - 1) + dxSrc - 1;
         nSrcRightAdjust = -1;
         nSrcDownAdjust = -nSrcDownAdjust;
         break;
      case 0x03:
         // rotate 270 degrees counter clockwise
         nSrc += nSrcDownAdjust * (dySrc - 1);
         nSrcRightAdjust = -nSrcDownAdjust;
         nSrcDownAdjust = 1;
         nTmp = (dx - dy) / 2;
         xDst += nTmp;
         yDst -= nTmp;
         nTmp = dx;
         dx = dy;
         dy = dxSrc;
         break;
      }

      // Here's where we take care of the scaling transformations.
      // This involves adjusting the destination rectangle, and setting
      // a scaling numerator and denominator, which we use to figure out
      // how many pixels to skip in each iteration for the destination
      // and source bitmaps.
      
      switch (unTransform & 0x38) {
      case 0x00:
         // no scaling
         break;
      case 0x08:
         // 2X
         nScaleNum = 2;
         xDst -= dx / 2;
         yDst -= dy / 2;
         dy <<= 1;
         dx <<= 1;
         break;
      case 0x10:
         // 4X
         nScaleNum = 4;
         xDst -= (dx / 2) * 3;
         yDst -= (dy / 2) * 3;
         dy <<= 2;
         dx <<= 2;
         break;
      case 0x18:
         // 8X
         nScaleNum = 8;
         xDst -= (dx / 2) * 7;
         yDst -= (dy / 2) * 7;
         dy <<= 3;
         dx <<= 3;
         break;
      case 0x28:
         // 1/8
         nScaleDen = 8;
         xDst += (dx * 7) >> 4;
         yDst += (dy * 7) >> 4;
         dy >>= 3;
         dx >>= 3;
         break;
      case 0x30:
         // 1/4
         nScaleDen = 4;
         xDst += (dx * 3) >> 3;
         yDst += (dy * 3) >> 3;
         dy >>= 2;
         dx >>= 2;
         break;
      case 0x38:
         // 1/2
         nScaleDen = 2;
         xDst += dx >> 2;
         yDst += dy >> 2;
         dy >>= 1;
         dx >>= 1;
         break;
      }

      // Now that we know the destination coordinates, we calculate where
      // to start in the destination bitmap.
      nDst = xDst + yDst * nDstPitch;

      // Clip to destination
      if (IS_OUTSIDE_DIRTY_RECT(pMe, xDst, yDst, dx, dy)) {
         if (xDst < 0) {
            nDst -= xDst;
            nSrc += (((-xDst) * nScaleDen) / nScaleNum) * nSrcRightAdjust;
            dx += xDst;
            xDst = 0;
         }
         if (yDst < 0) {
            nDst -= yDst * nDstPitch;
            nSrc += (((-yDst) * nScaleDen) / nScaleNum) * nSrcDownAdjust;
            dy += yDst;
            yDst = 0;
         }
         if (xDst + dx > pMe->m_dib.cx) {
            dx = pMe->m_dib.cx - xDst;
         }
         if (yDst + dy > pMe->m_dib.cy) {
            dy = pMe->m_dib.cy - yDst;
         }

         if (0 >= dx || 0 >= dy) {
            return SUCCESS;
         }
         EXPAND_DIRTY_RECT(pMe, xDst, yDst, dx, dy);
      } else if (0 >= dx || 0 >= dy) {
         return SUCCESS;
      }

      nDst2 = nDst;
      nSrc2 = nSrc;

      nSrcRightAdjust *= nScaleDen;
      nSrcDownAdjust *= nScaleDen;
      
      yNum = 0;

      // This is where the pixels are actually copied

      if (unComposite != COMPOSITE_KEYCOLOR) {

         // Opaque blit.  Don't have to worry about key color.

         // Iterate through destination pixels
         for (j = dy - 1; j >= 0; j--) {
            xNum = 0;
            for (i = dx - 1; i >= 0; i--) {
               SETPIXEL(pMe, nDst2 % nDstPitch, nDst2 / nDstPitch,
                        GETPIXEL(pbmSrc, nSrc2 % nSrcPitch, nSrc2 / nSrcPitch));
               nDst2++;
               xNum++;
               if (xNum == nScaleNum) {
                  xNum -= nScaleNum;
                  nSrc2 += nSrcRightAdjust;
               }
            }
            nDst2 = nDst += nDstPitch;
            yNum++;
            if (yNum == nScaleNum) {
               yNum -= nScaleNum;
               nSrc += nSrcDownAdjust;
            }
            nSrc2 = nSrc;
         }

      } else {

         // Transparent blit.  Must compare each pixel in source
         // to key color.

         // Iterate through destination pixels
         for (j = dy - 1; j >= 0; j--) {
            xNum = 0;
            for (i = dx - 1; i >= 0; i--) {
               NativeColor pixel = GETPIXEL(pbmSrc, nSrc2 % nSrcPitch, nSrc2 / nSrcPitch);
               if (pixel != ncTransparent) {
                  SETPIXEL(pMe, nDst2 % nDstPitch, nDst2 / nDstPitch, pixel);
               }
               nDst2++;
               xNum++;
               if (xNum == nScaleNum) {
                  xNum -= nScaleNum;
                  nSrc2 += nSrcRightAdjust;
               }
            }
            nDst2 = nDst += nDstPitch;
            yNum++;
            if (yNum == nScaleNum) {
               yNum -= nScaleNum;
               nSrc += nSrcDownAdjust;
            }
            nSrc2 = nSrc;
         }

      }

      return SUCCESS;
   }

   return EUNSUPPORTED;
#endif //TRANSFORM_SIMPLE
}


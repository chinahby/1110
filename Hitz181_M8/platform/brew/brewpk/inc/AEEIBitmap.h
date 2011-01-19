#ifndef  AEEIBITMAP_H
#define  AEEIBITMAP_H
/*=============================================================================

FILE:          AEEIBitmap.h

SERVICES:      IBitmap interface

DESCRIPTION:

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2001-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*-----------------------------------------------------------------------------
      Include Files
-----------------------------------------------------------------------------*/

#include "AEEIQI.h"
#include "AEERGBVAL.h"
#include "AEERasterOp.h"
#include "AEEPoint.h"
#include "AEERect.h"

// This is used for the native color values, whose meanings are local (native)
// to the bitmap in which they are used.
typedef uint32 NativeColor;

typedef struct {
   uint32  cx;
   uint32  cy;
   uint32  nDepth;
} AEEBitmapInfo;

#define AEEIID_IBitmap 0x01001021

#define INHERIT_IBitmap(iname) \
   INHERIT_IQI(iname); \
   NativeColor (*RGBToNative)     (iname *po, RGBVAL rgb); \
   RGBVAL      (*NativeToRGB)     (iname *po, NativeColor clr); \
   int  (*DrawPixel)              (iname *po, unsigned x, unsigned y, NativeColor color, AEERasterOp rop); \
   int  (*GetPixel)               (iname *po, unsigned x, unsigned y, NativeColor *pColor); \
   int  (*SetPixels)              (iname *po, unsigned cnt, AEEPoint *pPoint, NativeColor color, AEERasterOp rop); \
   int  (*DrawHScanline)          (iname *po, unsigned y, unsigned xMin, unsigned xMax, NativeColor color, AEERasterOp rop); \
   int  (*FillRect)               (iname *po, const AEERect *prc, NativeColor color, AEERasterOp rop); \
   int  (*BltIn)                  (iname *po, int xDst, int yDst, int dx, int dy, \
                                   IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop); \
   int  (*BltOut)                 (iname *po, int xDst, int yDst, int dx, int dy, \
                                   IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop); \
   int  (*GetInfo)                (iname *po, AEEBitmapInfo *pinfo, int nSize); \
   int  (*CreateCompatibleBitmap) (iname *po, IBitmap **ppIBitmap, uint16 w, uint16 h); \
   int  (*SetTransparencyColor)   (iname *po, NativeColor color); \
   int  (*GetTransparencyColor)   (iname *po, NativeColor *pColor)


AEEINTERFACE_DEFINE(IBitmap);

static __inline uint32
IBitmap_AddRef(IBitmap *po)
{
   return AEEGETPVTBL(po,IBitmap)->AddRef(po);
}

static __inline uint32
IBitmap_Release(IBitmap *po)
{
   return AEEGETPVTBL(po,IBitmap)->Release(po);
}

static __inline int
IBitmap_QueryInterface(IBitmap *po, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(po,IBitmap)->QueryInterface(po, cls, ppo);
}

static __inline NativeColor
IBitmap_RGBToNative(IBitmap *po, RGBVAL rgb)
{
   return AEEGETPVTBL(po,IBitmap)->RGBToNative(po, rgb);
}

static __inline RGBVAL
IBitmap_NativeToRGB(IBitmap *po, NativeColor clr)
{
   return AEEGETPVTBL(po,IBitmap)->NativeToRGB(po, clr);
}

static __inline int
IBitmap_DrawPixel(IBitmap *po, unsigned x, unsigned y, NativeColor color,
                  AEERasterOp rop)
{
   return AEEGETPVTBL(po,IBitmap)->DrawPixel(po, x, y, color, rop);
}

static __inline int
IBitmap_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor)
{
   return AEEGETPVTBL(po,IBitmap)->GetPixel(po, x, y, pColor);
}

static __inline int
IBitmap_SetPixels(IBitmap *po, unsigned cnt, AEEPoint *pPoint,
                  NativeColor color, AEERasterOp rop)
{
   return AEEGETPVTBL(po,IBitmap)->SetPixels(po, cnt, pPoint, color, rop);
}

static __inline int
IBitmap_DrawHScanline(IBitmap *po, unsigned y, unsigned xMin, unsigned xMax,
                      NativeColor color, AEERasterOp rop)
{
   return AEEGETPVTBL(po,IBitmap)->DrawHScanline(po, y, xMin, xMax, color,
                                                 rop);
}

static __inline int
IBitmap_FillRect(IBitmap *po, const AEERect *prc, NativeColor color,
                 AEERasterOp rop)
{
   return AEEGETPVTBL(po,IBitmap)->FillRect(po, prc, color, rop);
}

static __inline int
IBitmap_BltIn(IBitmap *po, int xDst, int yDst, int dx, int dy,
              IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop)
{
   return AEEGETPVTBL(po,IBitmap)->BltIn(po, xDst, yDst, dx, dy,
                                         pSrc, xSrc, ySrc, rop);
}

static __inline int
IBitmap_BltOut(IBitmap *po, int xDst, int yDst, int dx, int dy,
               IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop)
{
   return AEEGETPVTBL(po,IBitmap)->BltOut(po, xDst, yDst, dx, dy,
                                          pDst, xSrc, ySrc, rop);
}

static __inline int
IBitmap_GetInfo(IBitmap *po, AEEBitmapInfo *pinfo, int nSize)
{
   return AEEGETPVTBL(po,IBitmap)->GetInfo(po, pinfo, nSize);
}

static __inline int
IBitmap_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap,
                               uint16 w, uint16 h)
{
   return AEEGETPVTBL(po,IBitmap)->CreateCompatibleBitmap(po, ppIBitmap, w, h);
}

static __inline int
IBitmap_SetTransparencyColor(IBitmap *po, NativeColor color)
{
   return AEEGETPVTBL(po,IBitmap)->SetTransparencyColor(po, color);
}

static __inline int
IBitmap_GetTransparencyColor(IBitmap *po, NativeColor *pColor)
{
   return AEEGETPVTBL(po,IBitmap)->GetTransparencyColor(po, pColor);
}

static __inline int
IBitmap_Invalidate(IBitmap *po, const AEERect *prc)
{
   return AEEGETPVTBL(po,IBitmap)->FillRect(po, prc, 0, AEE_RO_TRANSPARENT);
}

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

AEEBitmapInfo

Description:
   This structure contains all of the information regarding the dimensions of a
bitmap, for both DIB and device dependent bitmap (DDB).

Definition:

typedef struct {
   uint32  cx;
   uint32  cy;
   uint32  nDepth;
} AEEBitmapInfo;

Members:

cx:      Number of pixels per row.
cy:      Number of pixel per column.
nDepth:  Number of significant bits per pixel.  This differs from the nDepth field
         in IDIB in that this one does not include padding bits.

Comments:
   None.

See Also:
   None.

===============================================================================

NativeColor

Description:
    The NativeColor type is used to represent the value of a single pixel in the
bitmap. The interpretation of this value as a color is dependent of the format of
the bitmap. You should not rely on this being in a particular format. Instead, you
should use IBitmap_NativeToRGB() and IBitmap_RGBToNative() to access a NativeColor.

Definition:
    typedef uint32 NativeColor;

Comments:
    None.

See Also:
    IBitmap_RGBToNative()
    IBitmap_NativeToRGB()

===============================================================================
*/

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IBitmap Interface

Description:

   This interface provides a way to manipulate bitmaps.  Each IBitmap instance
   represents a bitmap.

   IBitmap is an interface with multiple implementations.  Device-independent
   bitmaps (DIBs) created with IDisplay_CreateDIBitmap() are one class, and
   bitmaps that represent the handset's display are of another class.  While
   both classes implement the IBitmap interface, each has different
   capabilities.  Both DIBs and display bitmaps can be used in blit
   operations.  Display bitmaps support all drawing operations, but DIBs do
   not generally support drawing (except blitting), and return EUNSUPPORTED
   from most functions.

   All functions that return an error code can potentially return
   EUNSUPPORTED.  Users should be prepared for all types of error codes,
   however.

===============================================================================

Function: IBitmap_AddRef()

This function is inherited from IQI_AddRef().

===============================================================================

Function: IBitmap_Release()

This function is inherited from IQI_Release().

===============================================================================

Function: IBitmap_QueryInterface()

This function is inherited from IQI_QueryInterface().

===============================================================================

Function: IBitmap_RGBToNative()

Description:

   This function converts a RGBVAL into a native color value (pixel value).

   Native color values are the values stored in the pixel array; the mapping
   between native and RGB values is a property of the bitmap.  If no
   NativeColor corresponds exactly to the specified RGBVAL, a close match is
   returned.  This function is not required to return the closest match, and
   for performance reasons a close match (but not the closest) could be returned.

Prototype:
   NativeColor IBitmap_RGBToNative(IBitmap *po, RGBVAL RGBColor)

Parameters:
   po [in]: Pointer to IBitmap interface.

   RGBColor [in]: The RGB value to be converted from.  Only true RGB values
   are supported.  Color table indices are not supported.

Return Value:

   The correponding native color value.

Comments:

   If the bitmap's lacks palette information, the result is undefined.


Side Effects:
   None

See Also:
   RGBVAL, IBitmap_NativeToRGB()

===============================================================================

Function:   IBitmap_NativeToRGB

Description:

   This function obtains the R-G-B definition of a NativeColor value, in
   RGBVAL format.

   Each valid NativeColor corresponds to an R-G-B value.  The mapping of
   NativeColor values is a propery of the bitmap.

Prototype:
   RGBVAL   IBitmap_NativeToRGB(IBitmap *po, NativeColor clr);

Parameter:
   po [in]:    Pointer to IBitmap interface.

   clr [in]:   Native color value.

Return Value:

   It returns the corresponding RGBVAL.

Comments:

   If the NativeColor provided is not associated with a specific RGB value,
   the return value from this function is undefined.

Side Effect:
   None.

See Also:
   RGBVAL, IBitmap_RGBToNative()

===============================================================================

Function: IBitmap_DrawPixel()

Description:
   Draws a single pixel in the bitamp.

Prototype:
   int IBitmap_DrawPixel(IBitmap *po, unsigned x, unsigned y, NativeColor color, AEERasterOp rop)

Parameters:
   po [in]: Pointer to IBitmap interface to which pixel will be drawn.

   x [in]: X coordinate of pixel.

   y [in]: Y coordinate of pixel.

   color [in]: Specifies the color to be used to draw the pixel.  This is a
   native color, which can be obtained with IBitmap_RGBToNative().

   rop [in]: Specifies the raster operation that will be used to draw the
   pixel.  Only AEE_RO_COPY and AEE_RO_XOR are valid.

Return Value:
   SUCCESS:  on success.
   Return EUNSUPPORTED if the operation is not supported by the IBitmap.
   Return EBADPARM if raster op is invalid.~
   Other error codes can be returned (implementation specific).

Comments:

   If x or y are outside the bounds of the bitmap, nothing is drawn, and
   SUCCESS is returned.

Side Effects:
   None.

===============================================================================

Function: IBitmap_GetPixel()

Description:
   Retrieves the value of the specified pixel.

Prototype:
   int IBitmap_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor)

Parameters:
   po [in]: Pointer to IBitmap interface to which pixel will be drawn.

   x [in]: X coordinate of pixel.

   y [in]: Y coordinate of pixel.

   pColor [out]: Color of specified pixel.

Return Value:

   SUCCESS:  on success.~
   Return EBADPARM is coordinate is out of range.~
   Return EUNSUPPORTED if operation is not supported.~
   Other error codes can be returned (implementation specific).

Comments:
   None

Side Effects:
   None

See Also:
   None

===============================================================================

Function: IBitmap_SetPixels()

Description:
   Draws multiple pixels with the same color.

Prototype:
   int IBitmap_SetPixels(IBitmap *po, unsigned cnt, AEEPoint *pPoint,
                         NativeColor color, AEERasterOp rop);

Parameters:
   po [in]:       Pointer to IBitmap interface to which pixel will be drawn.

   cnt [in]:      Number of pixels.

   pPoint [in]:   Array of 2D points.

   color [in]: Specifies the color to be used to draw the input multiple pixels.
   This is a native color, which is obtained with IBitmap_RGBToNative.

   rop [in]: Specifies the raster operation that will be used to
   draw the pixel.

Return Value:
   SUCCESS:  on success, otherwise returns error code.~
   Other error codes can be returned (implementation specific).

Comments:
   Only AEE_RO_COPY and AEE_RO_XOR are valid raster operations.  If any value
   other these two are passed in as parameters, they will be treated as AEE_RO_COPY.

Side Effects:
   None

See Also:
   None

===============================================================================

Function: IBitmap_DrawHScanline()

Description:
   This function draws a horizontal line.

Prototype:
   int IBitmap_DrawHScanline(IBitmap *po, unsigned y, unsigned xMin, unsigned xMax,
                             NativeColor color, AEERasterOp rop)

Parameters:
   po [in]: Pointer to IBitmap interface object to be used to draw the
   horizontal line.

   y  [in]: Y coordinate of the line.

   xMin  [in]: X coordinate of left end of line.

   xMax  [in]: X coordinate of right end of line.

   color [in]: Specifies the color to be used to draw the line.  This
   is a native color, which is obtained with IBitmap_RGBToNative.

   rop [in]: Specifies the raster operation that will be used to
   draw the line.  Only AEE_RO_COPY and AEE_RO_XOR are valid.

Return Value:
   SUCCESS:  on success.~
   Return EBADPARM if raster op is invalid.~
   Other error codes can be returned (implementation specific).

Comments:
   None

Side Effects:
   None

See Also:
   None

===============================================================================

Function: IBitmap_FillRect()

Description:
   This function draws a solid rectangle of the specified color.

Prototype:
   int IBitmap_FillRect(IBitmap *po, const AEERect *prc, NativeColor color, AEERasterOp rop)

Parameters:
   po [in]: Pointer to IBitmap interface object to be used to fill the rectangle.

   prc: [in]: A pointer to a rectangle that needs to be filled with the
   specified color.

   color: [in]: Specifies the color to be used to fill the rectangle.  This
   is a native color, which is obtained with IBitmap_CovertColor.

   rop: [in]: Specifies the raster operation that will be used
   while drawing the rectangle.  Only AEE_RO_COPY and AEE_RO_XOR are valid.

Return Value:
   SUCCESS:  on success.~
   Return EBADPARM if raster op is invalid.~
   Other error codes can be returned (implementation specific).

Comments:

   'prc' must be a valid pointer.

   Any portions of the rectangle that fall outside the bitmap's bounds will be
   silently ignored (no errors will be generated).

Side Effects:
   None

See Also:
   None

===============================================================================

Function: IBitmap_BltIn()

Description:

  This function performs a bit-block transfer of the data corresponding to a
  rectangle of pixels from the specified source bitmap into this bitmap.

  Each pixel in the source is associated with a corresponding pixel in the
  destination.  A logical operation is performed on each pair of source and
  destination pixels, and the result is written over the destination pixel.


Prototype:
   int    IBitmap_BltIn(IBitmap * po,
                            int xDst,
                            int yDst,
                            int dx,
                            int dy,
                            IBitmap *pSrc,
                            int xSrc,
                            int ySrc,
                            AEERasterOp rop)

Parameters:
   po :[in]: Pointer to the IBitmap interface object into which the bit-block
   transfer needs to be done.

   xDst: [in]: Specifies the x-coordinates of the upper left corner of the
   destination rectangular area.

   yDst: [in]: Specifies the y-coordinates of the upper left corner of the
   destination rectangular area.

   dx: [in]: Specifies the width of the destination and source
   rectangles. Negative values will be treated

   dy: [in]: Specifies the height of the destination and source rectangles.
   Negative values will be treated as zero.

   pSrc: [in]: Pointer to another IBitmap interface that represents the
   source bitmap.

   xSrc: [in]: Specifies the x-coordinate of the upper left corner of the
   source bitmap from where the bit-block transfer must begin.

   ySrc: [in]: Specifies the y-coordinate of the upper left corner of the
   source bitmap from where the bit-block transfer must begin.

   rop: [in]: Specifies the raster operation that will be used while doing the
   bit-block transfer.

Return Value:

   SUCCESS:  if successful.

   EUNSUPPORTED:  or some other error code if the operation is not~
   supported.  This might be due to the format of the source bitmap or the~
   type of raster operation that was requested or a combination of the two.

Comments:

   When either dx or dy is negative, nothing will be written to the
   destination bitmap.  (The rectangles are treated as empty.)

   It is legal for all or part of the source or destination rectangles that
   fall outside the corresponding bitmap bounds (including negative
   coordinates).  When parts of the source or destination rectangles exceed a
   bitmap's bounds they will be ignored silently (clipped).  Clipping will not
   affect the mapping from source to destination of any unclipped portions,
   and will not result in an error code, even when everything is clipped.

   When the width and height of the source bitmap are not known and one wishes
   to blit the entire bitmap, very large values can be supplied for dx and dy,
   and clipping will limit the rectangle to the size of the source.

   The source bitmap may or may not be the same format as the destination
   bitmap, but not all source formats are necessarily supported.


Side Effects:
   None

See Also:
   None

===============================================================================

Function: IBitmap_BltOut()

Description:

  Perform a bit-block transfer from this bitmap into a specified destination
  bitmap.

  Users would not normally call this function directly.  Instead, the
  destination bitmap's IBitmap_BltIn() member function should be called,
  because that will succeed in more cases.

Prototype:
   int      IBitmap_BltOut(IBitmap * po,
                            int xDst,
                            int yDst,
                            int dx,
                            int dy,
                            IBitmap *pDst,
                            int xSrc,
                            int ySrc,
                            AEERasterOp rop)

Parameters:
   po :[in]: Pointer to the IBitmap interface object from which the bit-block
   transfer needs to be done.

   xDst: [in]: Specifies the x-coordinates of the upper left corner of the
   destination rectangular area.

   yDst: [in]: Specifies the y-coordinates of the upper left corner of the
   destination rectangular area.

   dx: [in]: Specifies the width of the destination rectangle.

   dy: [in]: Specifies the height of the destination rectangle.

   pDst: [in]: Pointer to another IBitmap interface that represents the
   destination bitmap.

   xSrc: [in]: Specifies the x-coordinate of the upper left corner of the
   source bitmap from where the bit-block transfer must begin.

   ySrc: [in]: Specifies the y-coordinate of the upper left corner of the
   source bitmap from where the bit-block transfer must begin.

   rop: [in]: Specifies the raster operation that will be used while doing the
   bit-block transfer.

Return Value:

   SUCCESS:  if successful.
   EUNSUPPORTED:  if the operation is not supported.~
   Other error codes can be returned (implementation specific).

Comments:

   There is no need to call this function except from the implementation of
   IBitmap_BltIn().

   IBitmap_BltOut() exists to help IBitmap_BltIn().  IBitmap_BltIn() will
   delegate to the source bitmap's IBitmap_BltOut() when the destination does
   not support the operation and the bitmaps are of different classes.  In
   this manner, both classes have the opportunity to perform the operation,
   and it will succeed as long as either class supports it.  Note that
   IBitmap_BltOut() cannot delegate likewise to IBitmap_BltIn(), because that
   would lead to infinite recusrion.

Side Effects:
   None

See Also:
   None

===============================================================================

Function: IBitmap_GetInfo()

Description:
   This function retrieves the dimension of the bitmap.

Prototype:
   int IBitmap_GetInfo(IBitmap *po, AEEBitmapInfo *pinfo, int nSize);

Parameters:
   po [in]:       Pointer to IBitmap interface.

   pinfo [out]:   Pointer to AEEBitmapInfo result.

   nSize [in]:    The size of AEEBitmapInfo in the current version.  AEEBitmapInfo STRUCT
                  may grow over time.  This field will allow backward compatibility.

Return Value:
   SUCCESS:  on success.
   EUNSUPPORTED:  if the size is not recognized by the bitmap.

Comments:

   This function should *always* succeed when nSize is equal to
   sizeof(AEEBitmapInfo), and when pinfo is a valid pointer.

Side Effects:
   None

See Also:
   AEEBitmapInfo

===============================================================================

Function: IBitmap_CreateCompatibleBitmap()

Description:

   This function creates a new bitmap compatible with this bitmap interface
   (the first parameter).

   "Compatible" means having equivalent pixel sizes and the same mapping
   between pixel values (native colors) and R-G-B values.  Width and height do
   not affect compatibility.

   A blit operation involving two compatible bitmaps will be reasonably fast,
   because it does not need to perform complex translations of pixel data.
   Being a common case, this type of operation is typically highly optimized.
   Also, copies between compatible bitmaps do not result in the loss of any
   color information.

   A compatible bitmap will generally support all the same drawing operations
   that the original bitmap supports.

Prototype:

  int IBitmap_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap, uint16 w, uint16 h);

Parameters:
  po         [in]: Pointer to the current bitmap interface.
  ppIBitmap [out]: Pointer to IBitmap interface of the new bitmap.
  w          [in]: Width of new bitmap.
  h          [in]: Height of new bitmap.

Return Value:
  SUCCESS: is returned if function executed correctly.
  ENOMEMORY: if there was not enough memory for the operation.~
  Other error codes can be returned (implementation specific).

Comments:

   The new bitmap inherits the transparency color of the bitmap from which it
   is created.

Side Effects:
  None.

See Also:
  None.

===============================================================================

Function: IBitmap_SetTransparencyColor()

Description:

  This function sets the transparency color of the bitmap.  This is used when
  this bitmap is the source bitmap of a transparent bit blit operation.  For
  pixels that contain this NativeColor, the correponding destination pixel
  will be unaffected.

Prototype:
  int IBitmap_SetTransparencyColor(IBitmap *pMe, NativeColor color);

Parameters:
  pMe        [in]: Pointer to the current bitmap interface.
  color      [in]: Color to make transparent.

Return Value:
  SUCCESS: is returned if function executed correctly.
  EUNSUPPORTED: if the operation is not supported.~
  Other error codes can be returned (implementation specific).

Comments:
  None.

Side Effects:
  None.

See Also:
  IBitmap_GetTransparencyColor(), IBitmap_BltIn(), IBitmap_BltOut()

===============================================================================

Function: IBitmap_GetTransparencyColor()

Description:

  This function gets the current transparency color of the bitmap.  This is
  used when this bitmap is the source bitmap of a transparent bit blit
  operation.

Prototype:
  int IBitmap_GetTransparencyColor(IBitmap *pMe, NativeColor *pColor);

Parameters:
  pMe        [in]: Pointer to the current bitmap interface.

  pColor    [out]: Transparency color.

Return Value:
  SUCCESS: is returned if function executed correctly.
  EBADPARM: is return if pColor is NULL.
  EUNSUPPORTED: if the operation is not supported.~
  Other error codes can be returned (implementation specific).

Comments:

  The transparency color is a NativeColor value, not an RGBVAL.

Side Effects:
  None.

See Also:
  IBitmap_SetTransparencyColor(), IBitmap_BltIn(), IBitmap_BltOut()

===============================================================================

Function: IBitmap_Invalidate()

Description:
  This function expands the bitmap's dirty rectangle to include the rectangle
  passed in the prc parameter.

Prototype:
  int IBitmap_Invalidate(IBitmap *po, const AEERect *prc);

Parameters:
  po         [in]: Pointer to the bitmap interface.

  prc       [out]: Pointer to the rectangle that should be included in the dirty
                   region.

Return Value:
  SUCCESS: is returned if function executed correctly.
  EUNSUPPORTED: if dirty rectangle optimizations are not implemented.

Comments:
  Some implementations of the device IBitmap optimize screen updates by keeping
  track of what parts of the screen have changed since the last update.  This
  "dirty rectangle" is automatically expanded by each IBitmap drawing function,
  and reset on update.  If an application obtains an IDIB from this IBitmap and
  modifies the pixel buffer directly, there is no way for the IBitmap implementation
  to know when a change has occurred.  It is therefore necessary to call this
  function to expand the dirty rectangle to include the pixels that have changed
  before an update occurs.  It is not necessary to call this function if the pixel
  buffer is not being modified directly through IDIB.

Side Effects:
  None.

See Also:
  IDIB

===============================================================================
*/

#endif /* AEEIBITMAP_H */


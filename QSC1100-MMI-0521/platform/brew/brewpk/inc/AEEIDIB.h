#ifndef  AEEIDIB_H
#define  AEEIDIB_H
/*=============================================================================

FILE:          AEEIDIB.h

SERVICES:      IDIB interface

DESCRIPTION:

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2001-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEIBitmap.h"

// IDIB : BREW Device Independent Bitmap (DIB) format.
//
// This is a struct with public members.  It is also an interface so that the
// behavior of QueryInterface() is consistent.

#define IDIB_COLORSCHEME_NONE   0   // no fixed mapping for pixel values

#define IDIB_COLORSCHEME_332    8   // 3 bits red, 3 bits green, 2 bits blue
#define IDIB_COLORSCHEME_444   12   // 4 red, 4 green, 4 blue
#define IDIB_COLORSCHEME_555   15   // 5 red, 5 green, 5 blue
#define IDIB_COLORSCHEME_565   16   // 5 red, 6 green, 5 blue
#define IDIB_COLORSCHEME_666   18   // 6 red, 6 green, 6 blue
#define IDIB_COLORSCHEME_888   24   // 8 red, 8 green, 8 blue


#define AEEIID_IDIB        0x01001045
#define AEEIID_DIB_20      0x0100102C // interface ID for DIB in 2.0

typedef struct IDIB        IDIB;

struct IDIB {
   AEEVTBL(IBitmap) *pvt;
   IQI * pPaletteMap;    // cache for computed palette mapping info
   byte *            pBmp;           // pointer to top row
   uint32 *          pRGB;           // palette
   NativeColor       ncTransparent;  // 32-bit native color value
   uint16            cx;             // number of pixels in width
   uint16            cy;             // number of pixels in height
   int16             nPitch;         // offset from one row to the next
   uint16            cntRGB;         // number of palette entries
   uint8             nDepth;         // size of pixel in bits
   uint8             nColorScheme;   // IDIB_COLORSCHEME_...
   uint8             reserved[6];    // initialize to 0 when constructing a DIB; ignore when using a DIB
};

// This is just for type safe casting of an IDIB* to an IBitmap*.
static __inline IBitmap *IDIB_to_IBitmap(const IDIB *pIDIB)
{
   return (IBitmap*)(void*)pIDIB;
}

static __inline uint32
IDIB_AddRef(IDIB *po)
{
   return AEEGETPVTBL(IDIB_to_IBitmap(po), IBitmap)->
      AddRef(IDIB_to_IBitmap(po));
}

static __inline uint32
IDIB_Release(IDIB *po)
{
   return AEEGETPVTBL(IDIB_to_IBitmap(po), IBitmap)->
      Release(IDIB_to_IBitmap(po));
}

static __inline int
IDIB_QueryInterface(IDIB *po, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(IDIB_to_IBitmap(po), IBitmap)->
      QueryInterface(IDIB_to_IBitmap(po), cls, ppo);
}

static __inline void
IDIB_FlushPalette(IDIB *po)
{
   IQI_RELEASEIF(po->pPaletteMap);
}

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IDIB Interface

Description:

IDIB is a structure and an interface. This structure defines the BREW device-independent bitmap format.

IDIB inherits all of the member functions
of IBitmap, so an IDIB may be used as an IBitmap by type casting. (The
IDIB_TO_IBITMAP() inline function is supplied for type safe casting.) Unlike other
BREW interfaces, IDIB also has public data members. These data members can be
used to efficiently read or modify image data.

An application typically obtains an IDIB pointer from an IBitmap pointer by
calling IBITMAP_QueryInterface() with the class ID AEECLSID_DIB. Not all
IBitmap classes support IDIB, and in those cases QueryInterface() will return
an error code. On success, an IDIB pointer is returned, which must be
released when the caller has finished using it.

A bitmap consists of a 2-dimensional array of pixels. IDIB contains members
that indicate where in memory the pixels are, and how the pixel values are to
be interpreted.

===H2>
     Pixel array structure
===/H2>
===p>

The locations and sizes of pixels in the pixel array are described by the
pBmp, nPitch, and nDepth members.

The nPitch field specifies the distance (in bytes) from the beginning of any
row to the beginning of the next row.  Pitch is typically a function of the
bitmap width, the padding being applied, and whether it is a top-down or
bottom-up bitmap.  Bottom-up bitmaps will have a negative pitch value.
Users of an IDIB should honor the nPitch value and make no assumptions about
padding or directionality in the bitmap.

pBmp points to the first scanline (that is, y = 0) in the memory buffer that
holds the pixel data of a DIB. For a top-down DIB, it points to the
beginning of the first row.  For a bottom-up DIB, it points to the beginning
of the last row in memory.
===H2>
Usage example:
===/H2>
===p>
    For a bitmap of color depth 8 (one byte per pixel), width of 9, and
    height of 10, the following representations are possible (among others):
===pre>
                               nPitch    pBmp - start of bit array
    Top-down BMP file:           12           0
    Bottom-up BMP file:         -12          96
    Top-down packed bitmap:       9           0
    Bottom-up packed bitmap:     -9          72
===/pre>

    In *all* of these cases, and in fact for any 8-bit DIB, the code for
    reading a pixel remains the same:~
===center>
       COLORVALUE = pdib->pBmp[ y * pdib->nPitch + x ]
===/center>

Alignment: Rows will typically start at 32-bit boundaries, but alignment is
not guaranteed except in two cases.  When nDepth is 32, rows should be 32-bit
aligned, and when nDepth is 16, each row must be aligned on a 16-bit boundary.

Within a row, the left-most (x=0) pixel begins at the most significant bit of
the first byte.  Pixels are packed, bitwise, and split across bytes if
necessary.  1, 2, 4, 8, and 16 bit bitmaps minimize splitting of pixel values
across bytes, and yield the most efficiency.  While possible and well-defined,
sizes that tend to map irregularly to byte boundaries (like 3 or 12) will lead
to reduced efficiency.
~
NOTE: When modifying the pixel buffer, it is important to call
IBITMAP_Invalidate in case dirty rectangle optimizations have been implemented.
*
===h2>
     Pixel values
===/h2>
===p>

The type "NativeColor" is defined to represent values stored in pixels in the
pixel array.  Palette information (cntRGB and pRGB, or alternatively
nColorScheme) describes how pixel values map to red, green, and blue intensity
values.

When cntRGB is nonzero, the pixel values are treated as indices into the
palette, which is an array of 32-bit R-G-B color values.  (See pRGB, below.)

When nColorScheme is nonzero, it identifies a mapping of pixel values to
R-G-B values.  This can be thought of as a hard-coded palette.  (See
nColorScheme, below.)

When both cntRGB and nColorScheme are zero, the bitmap's color mapping is
"empty."  Blits to or from DIBs with empty color mapping result in pixel
values being copied literally, without translation, from source to
destination.  If the destination pixels are too small to hold source pixel
values, the results are undefined.  When the color mapping is empty, the color
conversion functions (RGBToNative() and NativeToRGB()) produce undefined
results.

Both cntRGB and nColorScheme can be non-zero, but if they conflict, the
bitmap's color mapping is undefined (not empty).  Undefined color mapping leads
to unpredictable results from blits and color conversion functions.

Note that this mapping between pixel values and colors is somewhat independent
of pixel size.  A 4096 color bitmap requires 12 bits to represent all color
values, but these could be stored in 12-bit (a "packed" representation) or
16-bit pixels (unpacked).

Transparency is a special case in interpreting pixel values.  For operations
that support transparency, such as IBITMAP_BltIn() when called with the
AEE_RO_TRANSPARENT raster op, pixels whose values match the transparent color
(ncTransparent) are treated as transparent, which means that the corresponding
pixels in the destination are left unmodified.  ncTransparent is a NativeColor
value, and the matching is performed on pixel values, not on the corresponding
R-G-B intensities.
===h2>
     Palette Map
===/h2>
===p>

The pPaletteMap member is provided as a way for driver software to cache
information computed from the bitmap's palette.  This is not simply a function
of the palette; it depends also on the graphics algorithm, and any other
bitmaps with which the DIB might interact.  This member is set to NULL when
there is no palette mapping object associated with the IDIB.

Every IDIB implementation must ensure that the pPaletteMap object (if set) is
released when the IDIB itself is deleted.  The IDIB_FlushPalette() macro
performed this task.

Any pPaletteMap must be released whenever an IDIB is modified such that the
interpretation of pixel values are affected (i.e. when cntRGB, pRGB[],
nColorScheme, or ncTransparent are modified).  Otherwise, a graphics algorithm
might proceed to use stale data or perhaps even corrupt memory on a subsequent
call.

Generally, users can ignore this value unless they modify a DIB's palette data
after it has been used (that is, graphics operations, such as IBitmap member
functions, have been used on it).  In that case, IDIB_FlushPalette() must be
called.

===h2>
     Software Support
===/h2>
===p>

IDIB presents a wide latititude of possibilites for bitmap layouts, but
practical constraints limit the number of formats that are supported by other
software in the handset.  Of the layouts that are supported, not all of those
will be fully optimized.  These limitations can differ from handset to
handset.  The purpose of DIBs, however, is to communicate bitmap data to or
from the graphics driver, so the target device's level of support for
particular formats is important to application software.  Here are some
general guidelines:
~
   Each handset's primary display (whether monochrome, greyscale, or color)
   should support blitting from palette-based 1-, 2-, 4-, and 8-bit DIBs.
   Color mapping generally will be done in an expedient (that is, not the most
   accurate) fashion.  1-bit and 8-bit operations should be particularly well
   optimized.  Additionally, 16-bit handsets will support 16-bit DIB formats
   using either IDIB_COLORSCHEME_555 or IDIB_COLORSCHEME_565 color schemes,
   but blitting a 5-5-5 DIB to a 5-6-5 device, or vice versa, will incur a
   performance penalty.

   8-bit palette-based DIBs should work well with all color displays, and are
   much smaller than 16-bit images, so are recommended for color images.
   Since each 8-bit image can have its own selection of 256 colors, color
   accuracy should not be a problem.
*

Definition:

OBJECT(IDIB) {
   AEEVTBL(IDIB)  *pvt;
   IQI * pPaletteMap;
   byte *            pBmp;
   uint32 *          pRGB;
   NativeColor       ncTransparent;
   uint16            cx;
   uint16            cy;
   int16             nPitch;
   uint16            cntRGB;
   uint8             nDepth;
   uint8             nColorScheme;
   uint8             reserved[6];
}


Members:

Members:

  pvt:
~
    pointer to the v-table.  Users should not access this directly; macros are
    provided for all the member functions (IDIB_AddRef, IDIB_Release, and
    IDIB_QueryInterface).
*
  pPaletteMap:
~
    Cache for computed palette mapping data.


    Generally, developers can ignore this field.

    This member provided for graphics operations that read from or write to
    the DIB.  Some algorithms, like optimized DIB to native blits, involve
    complicated initialization steps that transform palette data to a more
    readily accessible foramt.  Such an algorithm can store the initialization
    data in pPaletteMap to avoid the need for recomputing the next time it
    runs.  Anyone replacing a non-NULL pointer must release the pre-existing
    pointer (with IQI_Release()), and when the DIB is deleted any non-NULL
    pointer will be released.
*
  pBmp:
~
    Pointer to the top row of the pixel array.
*
  pRGB:
~
    Pointer to the color palette.



    The color palette is an array of 32-bit color values.  The size of the
    palette array is given in the 'cntRGB' member.

    A palette defines the meaning of pixel values in the bitmap data.  A pixel
    value of N corresponds to the color at index N in the palette.  Any pixel
    value greater than the size of the palette is undefined.

    Palette color values are not RGBVAL values.  Palette values are specified
    in terms of memory layout.  The first byte is blue, the second byte is
    green, the third bute is red, and the fourth byte is ignored.  On a
    little-endian processor, a palette value is the same as NTOHL(rgb).  On
    a big-endian processor, it is compatible with RGBVAL.
*
  ncTransparent:
~
    Transparent color for the DIB.  Note that this is not in RGB form; it is
    specified as a NativeColor, which corresponds directly to values in the
    pixel array.
*
  cx:
~
    Width of bitmap in pixels.  Reading or writing pixels at or above this
    indexn must be avoided.
*
  cy:
~
    Height of bitmap in pixels.  Reading or writing pixels at or above this
    index must be avoided.
*
  nPitch:
~
    Offset from any row to the row below it.
*
  cntRGB:
~
    Number of entries in the palette.  If this is zero, the bitmap contains no
    palette.  If this is nonzero, then pRGB points to an array of palette
    entries.
*
  nDepth:
~
    Size of each pixel, in bits.
*
  nColorScheme:
~
    If nonzero, describes mapping from pixel values to R-G-B values.




    The following currently defined values for nColorScheme describe how bit
    fields within each pixel value represent red, green, and blue intenstiy
    values:
===pre>
      IDIB_COLORSCHEME_332 : 3 bits red, 3 bits green, 2 bits blue
      IDIB_COLORSCHEME_444 : 4 red, 4 green, 4 blue
      IDIB_COLORSCHEME_555 : 5 red, 5 green, 5 blue
      IDIB_COLORSCHEME_565 : 5 red, 6 green, 5 blue
      IDIB_COLORSCHEME_666 : 6 red, 6 green, 6 blue
      IDIB_COLORSCHEME_888 : 8 red, 8 green, 8 blue
===/pre>

    In each case, the blue bits occupy the least significant bits of the pixel
    value, the green bits the next most significant, and then the red bits.
    Any leftover most significant bits are unused.
*
  reserved:
~
    Reserved bytes for future version.  Initialize these bits to zeros when
    constructing a DIB; ingnore the value when parsing a DIB.

*

Comments:

   R-G-B intensity values correlate with actual perceived color, but the
   precise relationship is complex and dependent upon the display hardware.
   IBitmap and IDIB are unconcerned with such issues.

See Also:

   IBitmap Interface
   IQI Interface
   IDIB_TO_IBITMAP()
   IBITMAP_Invalidate()

===============================================================================

Function: IDIB_to_IBitmap()

Description:
  This function provides type safe casting from IDIB interface pointers to
  IBitmap interface pointers.  This function should be used when passing
  an IDIB interface to an IBitmap function.  This is safer than a simple
  cast, since the compiler will verify the pointer type.

Prototype:
  __inline IBitmap *IDIB_TO_IBITMAP(const IDIB *pIDIB);

Parameters:
  pIDIB      [in]: Pointer to an IDIB interface.

Return Value:
  Returns pIDIB cast to an IBitmap*.

Comments:
  None.

See Also:
  IDIB

===============================================================================

Function: IDIB_FlushPalette()

Description:
  This macro is used to flush the palette map of an IDIB structure.  The palette
  map stores cached palette conversion information.  It is necessary call this
  macro after making any change to the IDIB's palette.  If this is not done,
  the palette map may contain information that conflicts with the palette.

Prototype:
  #define IDIB_FlushPalette(pdib) if ((pdib)->pPaletteMap) {IQI_Release((pdib)->pPaletteMap); (pdib)->pPaletteMap = 0;}

Parameters:
  pdib       [in]: Pointer to an IDIB structure

Return Value:
  None.

Comments:
  None.

See Also:
  IDIB

===============================================================================
*/

#endif /* AEEIDIB_H */


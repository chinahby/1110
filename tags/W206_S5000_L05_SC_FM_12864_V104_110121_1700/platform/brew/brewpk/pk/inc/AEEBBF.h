#ifndef _AEEBBF_H_
#define _AEEBBF_H_
/*======================================================
FILE:  AEEBBF.h

SERVICES:  BREW Bitmapped Font (BBF) file format

DESCRIPTION:  Internal structure of BBF file

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright ?1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEComdef.h"

//------------------------    BBF file format    ------------------------
//
//
// An BREW bitmapped font file consists of the following sections:
//
//    <header>      : AEEBBFHdr structure
//    <additional>  : array of bytes
//    <ranges>      : array of AEEBBFRange structures
//    <glyphwidths> : array of bytes [Or overloaded as array of words]
//    <bitmap>      : bitmap data
//    <bitmap2>     : BBF Version 4 [byVersion 3] only. Monospaced bitmap aligned to 4 bytes.
//
// Each of the above sections appear one after another.  Although the content
// of a given section might consist of an odd number of bytes, one to three
// bytes of padding are added between sections to ensure that each section
// begins on a 32-bit boundary.
//
// All numeric values are stored in an LSB-first format.
//
// <header> section
// ----------------
//
//   The header section consists of an AEEBBFHdr structure.  (See the
//   AEEBBFHdr description.)
//
// <additional> section
// --------------------
//
//   The additional section is reserved for backwards-compatible evolution of
//   the file format.  In the future, this might hold information pertinent to
//   font registration, or information necessary for implemenation of font
//   variations (auto-generated bold or italic versions) or color fonts.
//
//   Software that reads a BBF file should not assume this section is empty,
//   and should quietly skip any additional data (and of course padding that
//   might be required).
//
// <ranges> section
// ----------------
//
//   This section consists of an array of AEEBBFRange structures.  This
//   array maps character values to glyph indices.  The number of entries
//   is given in the wRangeCount member of the header.
//
//   See the AEEBBFRange description for more information.
//
// <glyphwidths> section
// ---------------------
//
//   BBF Version 1 [AEEBBFHdr.byVersion == 0]:
//   This section consists of an array of bytes, each of which holds the width
//   of one or more glyphs.  Values for the elements of the array are as
//   follows:
//
//       0...127 = width of next glyph
//     128 & 129 = reserved
//     130...159 = repeat last width 2 to 31 times
//     160...191 = repeat last width (1 to 32) * 32 times
//     192...255 = reserved
//
//   The number of entries in this array is given by the cbWidths member of
//   the header section.  (This does not include the padding that might be
//   required for the next section to begin on a 32-bit boundary.)
//
//   The total of all glyph widths assigned must not exceed the width of the
//   bitmap section.
//
//
//   BBF Version 2 [AEEBBFHdr.byVersion == 1]:
//   This section consists of an array of words, each of which holds the
//   left-most position of each glyph.
//
//   The number of entries in this array is given by the cbWidths member of
//   the header section.  (This does not include the padding that might be
//   required for the next section to begin on a 32-bit boundary.)
//   This is useful for execution RAM savings. It works by identifying widths 
//   directly from the BBF data stream without needing to uncompress the data 
//   into RAM. The side effect of using this format style is an increased BBF 
//   file size.
//
//   The left-most position of the glyphs assigned must not exceed the width 
//   of the bitmap section.
//
//   BBF Version 3 [AEEBBFHdr.byVersion == 2]:
//   This section consists of a single byte sized width entry. This describes
//   the glyph cell size, half-width characters will consume a full cell in
//   the bitmap except that the glyph will only occupy the left half of the
//   cell.
//
// <bitmap> section
// ----------------
//
//   Version 1 and 2 [AEEBBFHdr.byVersion == 0 || AEEBBFHdr.byVersion == 1]:
//   The bitmap section consists of a pixel array that holds the character
//   cells for each of the glyphs in the font.  The first glyph (index 0) is
//   leftmost; subsequent glyphs follow to the right at positions described in
//   the <glyphwidths> section.
//
//   Version 3 [AEEBBFHdr.byVersion == 2]:
//   The bitmap section consists of a pixel array that holds the character
//   cells for each of the glyphs in the font.  The first glyph (index 0) is
//   leftmost; subsequent glyphs follow to the right at the position described in
//   the <glyphwidths> section up to and including the 256th character. After 256
//   characters, the next glyph (index 256) will be leftmost and below the first row.
//   Each character cell is a fixed width and occupies the same height which is calculated
//   based on number of characters and the bitmap height. If half-width characters are
//   used only the left half of the cell is used for display and measurement purposes for
//   the half-width entries. The cell width will be the same as that for full width 
//   characters. The <glyphwidths> section contains one entry to describe this cell width.
//
//   Layout:
//   Packing and layout of the image data are consistent with Windows bitmaps:
//   The bottommost rows appear first.  Within each row, the leftmost pixels
//   appear first.  Within each byte, the leftmost pixels are assigned the
//   highest order bits.  Each row is padded to the next 32-bit boundary.
//
//   Currently, only 1-bit-deep bitmaps are supported.
//


// Values for AEEBBFHdr.byFlags
//
// AEEBBF_CP1252 => BBF describes CP1252 and requests IFont to provide mapping
//     for 2-byte Unicode characters that correspond to CP1252 characters.
//
// AEEBBF_BEWIDTHS => BBF Version 2 uint16 widths entries are big endian
//
#define AEEBBF_CP1252        0x1
#define AEEBBF_BEWIDTHS      0x2


// Special values for AEEBBFRange.wCount :
//
#define AEEBBF_INDEXRESET    ((uint16)0xFFFF)
#define AEEBBF_INDEXFREEZE   ((uint16)0xFFFE)

// Special values for character widths
//
#define AEEBBF_REPBASE        128    // 130...159 => repeat (by-128) times
#define AEEBBF_REP32BASE      159    // 160...191 => repeat (by-159)*32 times

#define AEEBBF_MAXGLYPHWIDTH  127


//---- AEEBBFHdr
//
//   byCookie0, byCookie1, byVersion : These values distinguish this file
//     format from other common data formats.  These values must exactly match
//     the values 'B', 'F', and ['\0' or '\1'], respectively.  It is expected that
//     enhancements that are not backward compatible will increment the
//     version number.
//
//   byFlags : Enhancements that are backward compatible with older software
//     will be indicated by bits in this member.  Zero bits indicate that an
//     option is not selected.  Consumers should ignore flags they do not
//     understand.
//
//   byDescent : Distance from the bottom of each character cell to the
//     baseline of each character.
//
//   byDepth : Number of bits per pixel.  This corresponds to IDIB color
//     depth values.  Intially, only 1-bit-deep fonts are supported.
//
//   wWidth : number of 1-pixel wide columns of bitmap data.
//
//   wHeight : height of bitmap data included in the font.
//
//   wUndefGlyph : index of the glyph to be used for undefined characters.
//
//   wGlyphCount : Number of glyphs described in the font.  More precisely,
//     this is the number of rectangular areas described in the bitmap data (a
//     figure that appears twice in the bitmap counts as two glyphs).
//
//   cbAdditional : This indicates the size of the content of the <additional>
//     section of the file.  To obtain the space occupied by the entire
//     <additional> section, round this up to the nearest multiple of four.
//     See the data following AEEBBFHdr for description of BBF version 2
//     (AEEBBFHdr.byVersion == 1) additional usage.
//
//   cbWidths : number of entries in the <glyphwidths> section. Note that
//     since a width value can describe more than one glyph, cbWidths can
//     (and usually will) differ from wGlyphCount.
//
//   wRangeCount : number of elements in the <ranges> portion.
//
typedef struct AEEBBFHdr {
   uint8    byCookie0;        // 'B'
   uint8    byCookie1;        // 'F'
   uint8    byVersion;        // zero
   uint8    byFlags;          // unsupported flags will be quietly ignored by BREW
   uint8    byDescent;        // text descent (baseline)
   uint8    byDepth;          // bit depth of bitmap  (must be 1)
   uint16   wWidth;           // width of bitmap
   uint16   wHeight;          // height of bitmap (including marker row)
   uint16   wUndefGlyph;      // index of glyph to use for undefined characters
   uint16   wGlyphCount;      // number of glyphs in the font
   uint16   cbAdditional;     // number of additional bytes to skip before range data
   uint16   wRangeCount;      // number of ranges
   uint16   cbWidths;         // number of glyph width entries
} AEEBBFHdr;

// Version 3 [byVersion 2] BBF for Fixed Width (including half-width support) Additional data
typedef struct AEEMonoData {
   uint16   wHalfWidths;      // Value after the last of the half-width characters (0 for none)
   uint16   wReserved;        // Be nice for ROM based placement
} AEEMonoData;

// Version 4 [byVersion 3] BBF for Fixed Width (including half-width support) Additional data
typedef struct AEEMonoBitmapData {
   uint16      wMaxVarWidth;       // Value (plus 1) of highest variable width character
   uint8       cbMonoWidth;        // Width of a full-size monospaced character cell
   uint8       cbDescent;          // Descent for mono-spaced characters
   uint32      cbMonoBMPOffset;    // Offset to Mono-spaced bitmap in BBF v 4 [byVersion 3]
   uint32      cbMonoBMPBytes;     // Number of bytes in bitmap
   AEEMonoData mdInfo;             // Info for the Monospaced bitmap
} AEEMonoBitmapData;

//---- AEEBBFRange
//
// Normally, each range specifies a starting numeric character value and a
// number of characters, and each of these characters are mapped to glyphs,
// starting at the next available glyph.
//
// The exceptions to this general rule occur when a range's wCount value is
// one of the following:
//
// AEEBBF_INDEXRESET : This causes the "next glyph" pointer to be reset
//   to the value given in wCharBase, allowing the next range record to map
//   characters to a range of glyphs that were already used.
//
// AEEBBF_INDEXFREEZE : This causes the "next glyph" pointer to be
//   reset, and disables the automatic incrementing of glyph indices, so all
//   the characters in subsequent ranges will map to the same glyph.  This
//   incrementing remains disabled until an AEEBBF_INDEXRESET entry is
//   encounterd.
//
typedef struct AEEBBFRange {
   uint16   wCharBase;   // first code position (character value) in range
   uint16   wCount;      // number of characters in range
} AEEBBFRange;


//---- AEEBBFFile : describes font file contents after being read, or
//                      before being written.
//
typedef struct AEEBBFFile {
   AEEBBFHdr     hdr;
   const byte *  pbyAdditional;     // info.cbAdditional
   AEEBBFRange * pRanges;           // info.wRangeCount
   const byte *  pbyWidths;         // info.cbWidths
   const byte *  pbyBitmap;         // cbBitmap
   int           cbBitmap;          // number of bytes of bitmap data
} AEEBBFFile;


#endif /* _AEEBBF_H_ */

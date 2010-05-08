/*
  ========================================================================

  FILE:  CharSet.c
  
  SERVICES: Character set encoding and decoding

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "charset.h"


//----  Macros

#ifndef CHARISIN
#define CHARISIN(ch, cBtm, cTop)   \
   ( (unsigned char) ((ch) - (cBtm)) <= ((unsigned char) (cTop) - (cBtm)) )
#endif   

// Is c the first byte of a double-byte character?
//
#define EUC_DOUBLE(c)      (((c) & '\200') != 0)

// Is c the first byte of a double-byte character?
//
#define SJIS_DOUBLE(c)     (CHARISIN((c), 0x81, 0x9F) || CHARISIN((c), 0xE0, 0xFC))

// EUC-{KR,CN} : Break at all double-byte characters
//
#define EUC_ISBREAK(c)     EUC_DOUBLE(c)

// SJIS: Break at Hiragana, Katakana, or Kanji
//
#define SJIS_ISBREAK(c)    (CHARISIN(c, 0x7f, 0xff))

// UTF-8: Break at CJK & Hangul characters:
//
//   U+3000..U+E000 = E3 80 00 ... EE 80 80 :  This range includes CJK & Hangul
//
#define UTF8_ISBREAK(ch)   (CHARISIN(ch, 0xE3, 0xED))

// UTF-8: Thai character leading byte is 0xE0 and followed with 0xB8 or 0xB9
#define UTF8_ISTHAILOWRANGE(c)      ( (c) == 0xB8E0 )
#define UTF8_ISTHAIHIRANGE(c)       ( (c) == 0xB9E0 )

// UTF-8: Thai character range is 0xE0B8__ or 0xE0B9__
// The following byte has the range of modifiers in each range
#define IS_THAI_MODIFIERUTFLOW(c)   ( (c) == 0xB1 || ( CHARISIN((c), 0xB3, 0xBA) ) )
#define IS_THAI_MODIFIERUTFHI(c)    ( CHARISIN((c), 0x87, 0x8E) )

#define UNKNOWN_CHAR      '?'


//---- Inline utils

/*
:: Constrain: Constrain val to range nMin...nMax (inclusive).
::
:: When nMax < nMin, nMin will be returned.
*/
static INLINE int Constrain(int val, int nMin, int nMax)
{
   if (val > nMax) val = nMax;
   if (val < nMin) val = nMin;
   return val;
}



//---- Encoding support
//
// Sources of media type info:
//
//   1. IHTMLVIEWER_SetType()
//
//   2. <meta http-equiv=content-type content="text/html; charset=us-ascii">
//
//   3. <?xml charset=...>
//
// New error notifications from HTML Viewer:
//
//   - Unrecognized type
//
//   - Unrecognized encoding
//
//
// BREW 2.0 strategy:
//
//  - Accept and store data in original encoding (byte stream)
//      8859-1, 8859-2, 8859-5, cp1252, euc-kr, euc-cn, sjis, koi8r
//
//  - Convert to native encoding on measure/draw
//
//  - Convert character entity/references to original encoding from UCS
//
//
//
// Functions:
//
//    PFNDECODE      - DecodeXXX()   : Decodes encoded text to AECHAR array
//    PFNENCSPLIT    - ???()    : Finds word break
//    PFNENCCHARSIZE - ???() : Determines width of character
//    PFNCOUNT       - ...
//
//    General properties:  Characters starting with byte 00...7F are literal ASCII(*).
//    So:
//      1. HTML-significant names will work as if they are fed ASCII
//      2. PFNCHARSIZE() returns 1 for ASCII text
//
//    *: S-JIS replaces "\" with yen symbol.
//


// SBMap:  Describes mapping from single-byte charsets to Unicode.
//
typedef struct {
   const AECHAR *pwMap;    // array of AECHARs
   byte byBase;            // first byte mapped to array
   byte byCount;           // number of values to be mapped
} SBMap;


// CP1250 (Windows Eastern European)

static const AECHAR gawDecodeCP1250[] = {
   0x20AC, 0x0081, 0x201A, 0x0083, 0x201E, 0x2026, 0x2020, 0x2021,
   0x0088, 0x2030, 0x0160, 0x2039, 0x015A, 0x0164, 0x017D, 0x0179,
   0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
   0x0098, 0x2122, 0x0161, 0x203A, 0x015B, 0x0165, 0x017E, 0x017A,
   0x00A0, 0x02C7, 0x02D8, 0x0141, 0x00A4, 0x0104, 0x00A6, 0x00A7,
   0x00A8, 0x00A9, 0x015E, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x017B,
   0x00B0, 0x00B1, 0x02DB, 0x0142, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
   0x00B8, 0x0105, 0x015F, 0x00BB, 0x013D, 0x02DD, 0x013E, 0x017C,
   0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
   0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
   0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
   0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
   0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
   0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
   0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
   0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9,
};


// CP1251  (Windows Cyrillic)

static const AECHAR gawDecodeCP1251[] = {
   0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
   0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
   0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
   0x0098, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
   0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
   0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
   0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
   0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
   0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
   0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
   0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
   0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
   0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
   0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
   0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
   0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F, 
};


// CP1252  (Windows Western European)

static const AECHAR gawDecodeCP1252[] = {
   0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
   0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F,
   0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
   0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178,
};


// 8859-2:  ISO Latin 2

static const AECHAR gawDecode8859_2[] = {
   0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7,
   0x00A8, 0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B,
   0x00B0, 0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7,
   0x00B8, 0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C,
   0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
   0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
   0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
   0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
   0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
   0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
   0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
   0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9,
};


// 8859-5:  "Cyrillic"

static const AECHAR gawDecode8859_5[] = {
   0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407,
   0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F,
   0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
   0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
   0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
   0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
   0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
   0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
   0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
   0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
   0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
   0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F,
};


// KOI8:  Map {80..FF}
//
// Here we use the 'unified' character set proposed by Peter Cassetta of
// Fingertip Software.  This is point-compatible with all *letters* in the
// koi8-* variants and includes some useful Windows characters (double quotes,
// etc.), omitting some of the more complicated box drawing characters koi8
// variants inherited from DOS.  As a result, we can use one table for all
// koi8-* variants.
//

static const AECHAR gawDecodeKOI8[] = {
   0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524,
   0x252C, 0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590,
   0x2591, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
   0x00A9, 0x2122, 0x00A0, 0x00BB, 0x00AE, 0x00AB, 0x00B7, 0x00A4,
   0x00A0, 0x0452, 0x0453, 0x0451, 0x0454, 0x0455, 0x0456, 0x0457,
   0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x0491, 0x045E, 0x045F,
   0x2116, 0x0402, 0x0403, 0x0401, 0x0404, 0x0405, 0x0406, 0x0407,
   0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x0490, 0x040E, 0x040F,
   0x044E, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
   0x0445, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E,
   0x043F, 0x044F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
   0x044C, 0x044B, 0x0437, 0x0448, 0x044D, 0x0449, 0x0447, 0x044A,
   0x042E, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
   0x0425, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E,
   0x041F, 0x042F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
   0x042C, 0x042B, 0x0417, 0x0428, 0x042D, 0x0429, 0x0427, 0x042A,
};


// Expand single-byte encodings
//
static int SBDecode(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed,
                        const SBMap *pmap)
{
   AECHAR *pwc = pwcDest;
   byte *  pby = (byte *) pcSrc;
   int     cbMax = Constrain(cwDest, 0, cbSrc);
   byte *  pbyMax = pby + cbMax;
   byte    byBase = pmap->byBase;
   byte    bySize = pmap->byCount;
   const AECHAR *pwCvt = pmap->pwMap - byBase;
   
   while (pby < pbyMax) {
      byte by = *pby++;
      if ( (unsigned)by - (unsigned)byBase < (unsigned)bySize ) {
         *pwc = pwCvt[by];
      } else {
         *pwc = by;
      }
      ++pwc;
   }

   *pcbUsed = cbMax;
   return cbMax;
}


// Find w in pwSrc[cnt]
//
static int FindWord(const uint16 *pwSrch, int cnt, int w, int nDefault)
{
   const uint16 *pw = pwSrch;
   const uint16 *pwMax = pw+cnt;

   while (pw < pwMax) {
      if (*pw == w)
         return pw - pwSrch;
      ++pw;
   }
   return nDefault;
}


// Encode:  Compress single-byte strings
//
static int SBEncode(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed,
                        const SBMap *pmap)
{
   int     cbMax = Constrain(cbDest, 0, cwSrc);
   uint16  wMapBase  = pmap->byBase;
   uint16  wMapMax   = wMapBase + pmap->byCount;
   uint16  wCount    = pmap->byCount;
   const AECHAR *pwcMap = pmap->pwMap;
   const AECHAR *pwc = pwcSrc;
   const AECHAR *pwcMax = pwcSrc + cbMax;

   // wLookup:  ALL characters >= wLookup must be looked up (test shortcut in the loop)
   uint16  wLookup = wMapBase;
   if (wMapMax < 256)
      wLookup = 256;

   // Any wide character > 255 OR inside a mapped range must be looked up.

   while (pwc < pwcMax) {
      AECHAR w = *pwc++;

      // Test is optimized for the case where the lookup doesn't happen (w < wMapBase)
      if (w >= wMapBase && (w >= wLookup || w < wMapMax)) {
         w = FindWord(pwcMap, wCount, w, ((int)UNKNOWN_CHAR - wMapBase)) + wMapBase;
      }
      *pcDest++ = (char) (byte)w;
   }

   *pnUsed = cbMax;
   return cbMax;
}


//
//--------  Decoding Functions
//
// "Expand" string to format used by IDISPLAY_DrawText()  (Not really
// Unicode, but 2 bytes per character.)
//
// Unlike STREXPAND(), this works on any type of handset, and it returns the
// number of characters consumed and output.  (STREXPAND() should be called
// only for KSC5601 and S_JIS handsets.)  STREXPAND() is also inefficient, and
// takes the size of the AECHAR array in bytes, not AECHARs.  It also performs
// unaligned memory accesses (barf!).  Another difference is that this
// function does not support overlaying source and destination.
//
// On exit:
//    Return value = number of AECHARs output
//    pwcDest[] = expanded characters
//    *pcbUsed = number of bytes consumed
//
// NOTE: Output text is not necessarily zero-terminated.  If the input included
//       a terminating zero, and if that fit in the buffer, then the result
//       will be zero-terminated.




// Decode EUC strings to BREW wide strings
//
//   Wide characters of the form aabb:
//
//     1) aa = 00,       bb = {00..7f}      Single byte bb
//     2) aa = {a0..ff}, bb = {a0..ff}      Double-byte (memory layout preserved in the 
//
//
//     euc-kr  -->  ks c 5601
//     euc-cn  -->  gb 2312
//
// Note: SS2 and SS3 are not handled since CN and KR variants do not use G2 and G3.
//       euc-jp support would require determination of how to encode G3 in AECHAR values.
//
static int DecodeEUC(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   AECHAR *pwc = pwcDest;
   byte *  pby = (byte *) pcSrc;
   AECHAR *pwcMax = pwc + cwDest;
   byte *  pbyMax = pby + cbSrc;
   byte by;

   while (pwc < pwcMax && pby < pbyMax) {
      by = pby[0];
      if (EUC_DOUBLE(by)) {
         ((byte*)pwc)[0] = by;
         ((byte*)pwc)[1] = pby[1];
         ++pwc;
         pby += 2;
      } else {
         *pwc++ = by;
         ++pby;
      }
   }

   if (pby > pbyMax) {
      // read one past the end: improperly formatted input
      // discard last character of output
      pby -= 2;
      --pwc;
   }

   *pcbUsed = pby - (byte *)pcSrc;
   return pwc - pwcDest;
}


// Expand Shift JIS to JIS X 208 strings
//
static int DecodeSJIS(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   AECHAR *pwc = pwcDest;
   byte *  pby = (byte *) pcSrc;
   AECHAR *pwcMax = pwc + cwDest;
   byte *  pbyMax = pby + cbSrc;

   while (pwc < pwcMax && pby < pbyMax) {

      byte by = pby[0];

      if (SJIS_DOUBLE(by)) {
         ((byte*)pwc)[0] = by;
         ((byte*)pwc)[1] = pby[1];
         ++pwc;
         pby += 2;
      } else {
         *pwc++ = by;
         ++pby;
      }
   }

   if (pby > pbyMax) {
      // read one past the end: improperly formatted input
      // discard last character of output
      pby -= 2;
      --pwc;
   }
      
   *pcbUsed = pby - (byte *)pcSrc;
   return pwc - pwcDest;
}


// Expand Latin1 to Unicode
//
static int Decode8859_1(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   AECHAR *pwc = pwcDest;
   byte *  pby = (byte *) pcSrc;
   int     cbMax = Constrain(cwDest, 0, cbSrc);
   byte *  pbyMax = pby + cbMax;

   while (pby < pbyMax) {
      uint16 w = *pby++;
      *pwc++ = w;
   }

   *pcbUsed = cbMax;
   return cbMax;
}

static int Decode8859_2(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecode8859_2;
   sbm.byBase  = 0xa0;
   sbm.byCount = 96;
   return SBDecode(pwcDest, cwDest, pcSrc, cbSrc, pcbUsed, &sbm);
}


static int Decode8859_5(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecode8859_5;
   sbm.byBase  = 0xa0;
   sbm.byCount = 96;
   return SBDecode(pwcDest, cwDest, pcSrc, cbSrc, pcbUsed, &sbm);
}


static int DecodeCP1250(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecodeCP1250;
   sbm.byBase  = 0x80;
   sbm.byCount = 128;
   return SBDecode(pwcDest, cwDest, pcSrc, cbSrc, pcbUsed, &sbm);
}


static int DecodeCP1251(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecodeCP1251;
   sbm.byBase  = 0x80;
   sbm.byCount = 128;
   return SBDecode(pwcDest, cwDest, pcSrc, cbSrc, pcbUsed, &sbm);
}


static int DecodeCP1252(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecodeCP1252;
   sbm.byBase  = 0x80;
   sbm.byCount = 32;
   return SBDecode(pwcDest, cwDest, pcSrc, cbSrc, pcbUsed, &sbm);
}


static int DecodeKOI8(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecodeKOI8;
   sbm.byBase  = 0x80;
   sbm.byCount = 128;
   return SBDecode(pwcDest, cwDest, pcSrc, cbSrc, pcbUsed, &sbm);
}


// Expand UTF-8 to Unicode
//
static int DecodeUTF8(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed)
{
   AECHAR *pwc = pwcDest;
   byte *  pby = (byte *) pcSrc;
   AECHAR *pwcMax = pwc + cwDest;
   byte *  pbyMax = pby + cbSrc;

   while (pwc < pwcMax && pby < pbyMax) {
      
      uint32 w = *pby++;

      if (w >= 0xC0) {
         int nExtra;
         
         if ( (w & 0x20) == 0) {

            // 2 bytes (11 bits)
            
            nExtra = 1;
            w = (w & 0x1f) << 6;
            w |= pby[0] & 0x3F;

         } else if ( (w & 0x10) == 0) {

            // 3 bytes (16 bits)
            
            nExtra = 2;
            w = (w & 0x0f) << 12;
            w |= (pby[0] & 0x3F) << 6;
            w |= (pby[1] & 0x3F);
            
         } else if ( (w & 0x08) == 0) {

            // 4 bytes (21 bits)

            nExtra = 3;
            w = (w & 0x07) << 18;
            w |= (pby[0] & 0x3F) << 12;
            w |= (pby[1] & 0x3F) << 6;
            w |= (pby[2] & 0x3F);

         } else {

            nExtra = 0;
         }

         pby += nExtra;

         if (pby > pbyMax) {
			// don't consume first half of a character
            pby -= nExtra+1;
            break; 
         }

         if (w >= 0x10000) {
            // Output high-order surrgate character & compute low-order character
            //   High Surrogates: U+D800..U+DBFF
            //   Low Surrogates:  U+DC00..U+DFFF
            //
            // Note that 4-byte UTF-8 represents 21 bits, while 2-word UTF-16
            // supports just over a 20 bit range (00000 ... 10FFFF).

            *pwc++ = (AECHAR) (0xD7c0 + ((w >> 10) & 0x3FF));
            w = (w & 0x3FF) | 0xDC00;

            if (pwc == pwcMax) {
               // result won't fit
               pby -= (nExtra + 1);
               --pwc;
               break;
            }
         }
      }
      *pwc++ = (AECHAR) w;
   }

   *pcbUsed = pby - (byte *)pcSrc;
   return pwc - pwcDest;
}


//
//--------  Encoding Functions
//



static int EncodeUTF8(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   char *pc = pcDest;
   char *pcMax = pc + cbDest;
   const AECHAR *pw = pwcSrc;
   const AECHAR *pwMax = pw + cwSrc;
   
   while (pc < pcMax && pw < pwMax) {

      AECHAR w = *pw;
      
      if (w < 0x80) {

         *pc++ = (char)w;

      } else if (w < 0x800) {

         if (pc+2 > pcMax)
            break;

         pc[0] = (char) ( 0xC0 | (w >> 6) );
         pc[1] = (char) ( 0x80 | (w & 0x3F) );
         pc += 2;

      } else {

         // NOTE: This is not UTF-16 aware.  Surrogate pairs are not converted
         // to 4-byte UTF8 sequences.
         
         if (pc + 3 > pcMax)
            break;

         pc[0] = (char) ( 0xe0 | (w >>12) );
         pc[1] = (char) ( 0x80 | ((w >>6) & 0x3f) );
         pc[2] = (char) ( 0x80 | (w & 0x3F) );
         pc += 3;
      }
      ++pw;
   }

   *pnUsed = pw - pwcSrc;
   return pc - pcDest;
}


static int EncodeEUC(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   char *pc = pcDest;
   char *pcLast = pc + cbDest - 1;
   const AECHAR *pw = pwcSrc;
   const AECHAR *pwMax = pw + cwSrc;

   while (pc <= pcLast && pw < pwMax) {

      AECHAR w = *pw;

      if (w & 0x8080) {
         if (pc == pcLast)
            break;
         pc[0] = ((byte*)pw)[0];
         pc[1] = ((byte*)pw)[1];
         pc += 2;
      } else {
         *pc++ = (w & 0x7F);
      }
      pw++;
   }

   *pnUsed = pw - pwcSrc;
   return pc - pcDest;
}

static int EncodeSJIS(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   char *pc = pcDest;
   char *pcLast = pc + cbDest - 1;
   const AECHAR *pw = pwcSrc;
   const AECHAR *pwMax = pw + cwSrc;

   while (pc <= pcLast && pw < pwMax) {

      byte by = ((byte*)pw)[0];
      if (SJIS_DOUBLE(by)) {
         if (pc == pcLast)
            break;
         pc[0] = by;
         pc[1] = ((byte*)pw)[1];
         pc += 2;
      } else {
         *pc++ = by;
      }
      pw++;
   }

   *pnUsed = pw - pwcSrc;
   return pc - pcDest;
}


static int Encode8859_1(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   char *pc = pcDest;
   char *pcLast = pc + cbDest - 1;
   const AECHAR *pw = pwcSrc;
   const AECHAR *pwMax = pw + cwSrc;

   while (pc <= pcLast && pw < pwMax) {
      AECHAR w = *pw++;
      if (w > 0xFF)
         w = UNKNOWN_CHAR;
      *pc++ = (byte) w;
   }

   *pnUsed = pw - pwcSrc;
   return pc - pcDest;
}


static int EncodeCP1250(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecodeCP1250;
   sbm.byBase  = 0x80;
   sbm.byCount = 128;
   return SBEncode(pcDest, cbDest, pwcSrc, cwSrc, pnUsed, &sbm);
}


static int EncodeCP1251(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecodeCP1251;
   sbm.byBase  = 0x80;
   sbm.byCount = 128;
   return SBEncode(pcDest, cbDest, pwcSrc, cwSrc, pnUsed, &sbm);
}


static int EncodeCP1252(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecodeCP1252;
   sbm.byBase  = 0x80;
   sbm.byCount = 32;
   return SBEncode(pcDest, cbDest, pwcSrc, cwSrc, pnUsed, &sbm);
}


static int Encode8859_2(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecode8859_2;
   sbm.byBase  = 0xa0;
   sbm.byCount = 96;
   return SBEncode(pcDest, cbDest, pwcSrc, cwSrc, pnUsed, &sbm);
}

static int Encode8859_5(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecode8859_5;
   sbm.byBase  = 0xa0;
   sbm.byCount = 96;
   return SBEncode(pcDest, cbDest, pwcSrc, cwSrc, pnUsed, &sbm);
}


static int EncodeKOI8(char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   SBMap sbm;
   sbm.pwMap   = gawDecodeKOI8;
   sbm.byBase  = 0x80;
   sbm.byCount = 128;
   return SBEncode(pcDest, cbDest, pwcSrc, cwSrc, pnUsed, &sbm);
}


//
//-------- Sizing functions
//


static int CharSizeSingle(char ch)
{
   return 1;
}

static int CharSizeEUC(char ch)
{
   return 1 + EUC_DOUBLE(ch);
}

static int CharSizeSJIS(char ch)
{
   return 1 + SJIS_DOUBLE(ch);
}

static int CharSizeUTF8(char ch)
{
   static const byte bySizes[] = { 1, 1, 1, 1,          // 00xx = single-byte
                                   1, 1, 1, 1,          // 01xx = single-byte
                                   1, 1, 1, 1,          // 10xx = error
                                   2, 2, 3, 4 };        // 11xx = start of 2, 3, or 4 byte

   return bySizes[ ((byte)ch) >> 4];
}


//
//--------  Find split: find breaking characters
//



// Look for breaking characters within a word, starting at pcBrk, up and
// including pcClip but not pcMax.
//
// pcBrk = position after last word break (last valid break point) (0 => no break)
// pcClip = number of characters that fit
// pcMax = end of text
//
static const char *FindSplitEUC(const char *pcBrk, const char *pcClip, const char *pcMax)
{
   const char *pcSplit = pcBrk;

   // Go through word, noting last break position

   // Split after any Korean character (not proper typography, but
   // practical and preferred on a phone screen)
      
   while (pcBrk < pcClip) {
      if (EUC_DOUBLE(*pcBrk)) {
         pcBrk += 2;
         pcSplit = pcBrk;
      } else {
         ++pcBrk;
      }
   }

   // Also, split before CJK/Hangul...

   if (pcBrk < pcMax && EUC_ISBREAK(*pcBrk)) {
      pcSplit = pcBrk;
   }

   return pcSplit;
}


static const char *FindSplitSJIS(const char *pcBrk, const char *pcClip, const char *pcMax)
{
   const char *pcSplit = pcBrk;

   // Go through word, noting last break position

   // Split after every Japanese character
      
   while (pcBrk < pcClip) {
      if (SJIS_DOUBLE(*pcBrk)) {
         pcBrk += 2;
         pcSplit = pcBrk;
      } else if (SJIS_ISBREAK(*pcBrk)) {
         pcBrk += 1;
         pcSplit = pcBrk;
      } else {
         pcBrk++;
      }
   }

   // Also, split before CJK/Hangul...
   
   if (pcBrk < pcMax && SJIS_ISBREAK(*pcBrk)) {
      pcSplit = pcBrk;
   }

   return pcSplit;
}


static const char *FindSplitUTF8(const char *pcBrk, const char *pcClip, const char *pcMax)
{
   const char *pcSplit = pcBrk;

   // Go through word, noting last break position

   // Split after every CJK or Hangul character...
      
   while (pcBrk < pcClip) {
      byte by = (byte)*pcBrk;
      
      if (by < 0xC0) {
         // not a split point
         pcBrk++;
      } else if ( (by & 0x20) == 0 ) {
         // 2-byte: not a split point
         pcBrk += 2;
      } else if ( (by & 0x10) == 0) {
         // 3-byte: maybe a split point
         pcBrk += 3;
         if (UTF8_ISBREAK(by)) {
            pcSplit = pcBrk;
         } else {
            uint16 wHighWord;

            MEMCPY((void *)&wHighWord, (void *)pcBrk, sizeof(uint16));
            if ( UTF8_ISTHAILOWRANGE(wHighWord) ) {
               if( !IS_THAI_MODIFIERUTFLOW(*(pcBrk+2)) ){
                  pcSplit = pcBrk;
               }
            } else if ( UTF8_ISTHAILOWRANGE(wHighWord) ){
               if( !IS_THAI_MODIFIERUTFHI(*(pcBrk+2)) ){
                  pcSplit = pcBrk;
               }
            }
         }

      } else if ( (by & 0x08) == 0) {
         pcBrk += 4;
      } else {
         ++pcBrk;
      }
   }

   // Also, split before CJK/Hangul...
   
   if (pcBrk < pcMax && UTF8_ISBREAK(*pcBrk)) {
      pcSplit = pcBrk;
   }

   return pcSplit;
}




//-------- Charset name lookup
//
// Here we group the encode/decode functions into structures that can be
// indexed by charset names.  Names are those registered with IANA and
// others added for compatibility with historical usage.
//
// NOTE: When the native encoding is Korean, Japanese, or Chinese, we don't
// have anything to lookup.  For each of those native charsets there is only
// one supported encoding.
//


// These DoxCharsets convert between UCS and a MIME charset
//



//-------- Map UCS to other wide character sets
//

typedef struct {
   unsigned short a;
   unsigned short b;
} CharMap;


// This table maps UCS characters used in HTML entities to SJIS wide
// characters.  (Stopgap until we have more complete encoding support.)
//
// Entries with comments are compatibility entries (non-exact mappings).
//
// ** NBSP is mapped to IDEOGRAPHIC SPACE, which is not collapsed (as on
//    Mozilla or IE).  Unfortunately it is double the width of normal space,
//    and is a text wrapping position.
//
static const CharMap gSJISEntsMap[] =
{
   0x00a0, 0x4081, // NBSP -> IDEOGRAPHIC SPACE  (**)
   0x00a2, 0x9181,
   0x00a3, 0x9281,
   0x00a5, 0x005c,
   0x00a6, 0x6281, // "brvbar"
   0x00a7, 0x9881,
   0x00a8, 0x4e81,
   0x00ab, 0x7381, // "laquo"
   0x00ac, 0xca81,
   0x00af, 0x5081, // "macr"
   0x00b0, 0x8b81,
   0x00b1, 0x7d81,
   0x00b4, 0x4c81,
   0x00b5, 0xca83, // "micro"
   0x00b6, 0xf781,
   0x00b7, 0x4581, // "middot"
   0x00bb, 0x7481, // "raquo"
   0x00d7, 0x7e81,
   0x00f7, 0x8081,
   0x0391, 0x9f83,
   0x0392, 0xa083,
   0x0393, 0xa183,
   0x0394, 0xa283,
   0x0395, 0xa383,
   0x0396, 0xa483,
   0x0397, 0xa583,
   0x0398, 0xa683,
   0x0399, 0xa783,
   0x039a, 0xa883,
   0x039b, 0xa983,
   0x039c, 0xaa83,
   0x039d, 0xab83,
   0x039e, 0xac83,
   0x039f, 0xad83,
   0x03a0, 0xae83,
   0x03a1, 0xaf83,
   0x03a3, 0xb083,
   0x03a4, 0xb183,
   0x03a5, 0xb283,
   0x03a6, 0xb383,
   0x03a7, 0xb483,
   0x03a8, 0xb583,
   0x03a9, 0xb683,
   0x03b1, 0xbf83,
   0x03b2, 0xc083,
   0x03b3, 0xc183,
   0x03b4, 0xc283,
   0x03b5, 0xc383,
   0x03b6, 0xc483,
   0x03b7, 0xc583,
   0x03b8, 0xc683,
   0x03b9, 0xc783,
   0x03ba, 0xc883,
   0x03bb, 0xc983,
   0x03bc, 0xca83,
   0x03bd, 0xcb83,
   0x03be, 0xcc83,
   0x03bf, 0xcd83,
   0x03c0, 0xce83,
   0x03c1, 0xcf83,
   0x03c3, 0xd083,
   0x03c4, 0xd183,
   0x03c5, 0xd283,
   0x03c6, 0xd383,
   0x03c7, 0xd483,
   0x03c8, 0xd583,
   0x03c9, 0xd683,
   0x2018, 0x6581,
   0x2019, 0x6681,
   0x201c, 0x6781,
   0x201d, 0x6881,
   0x2020, 0xf581,
   0x2021, 0xf681,
   0x2026, 0x6381,
   0x2030, 0xf181,
   0x2032, 0x8c81,
   0x2033, 0x8d81,
   0x203e, 0x007e,
   0x2190, 0xa981,
   0x2191, 0xaa81,
   0x2192, 0xa881,
   0x2193, 0xab81,
   0x21d2, 0xcb81,
   0x21d4, 0xcc81,
   0x2200, 0xcd81,
   0x2202, 0xdd81,
   0x2203, 0xce81,
   0x2207, 0xde81,
   0x2208, 0xb881,
   0x220b, 0xb981,
   0x2211, 0x9487, // "sum"
   0x2212, 0x7c81,
   0x221a, 0xe381,
   0x221d, 0xe581,
   0x221e, 0x8781,
   0x2220, 0xda81,
   0x2227, 0xc881,
   0x2228, 0xc981,
   0x2229, 0xbf81,
   0x222a, 0xbe81,
   0x222b, 0xe781,
   0x2234, 0x8881,
   0x2260, 0x8281,
   0x2261, 0xdf81,
   0x2282, 0xbc81,
   0x2283, 0xbd81,
   0x2286, 0xba81,
   0x2287, 0xbb81,
   0x22a5, 0xdb81,
};

static const CharMap gKSCEntsMap[] =
{
  0x00a0, 0x00a0,
  0x00a1, 0xa2ae,
  0x00a2, 0xa1cb,
  0x00a3, 0xa1cc,
  0x00a4, 0xa2b4,
  0x00a5, 0xa1cd,
  0x00a6, 0xa3fc,
  0x00a7, 0xa1d7,
  0x00a8, 0xa1a7,
  0x00a9, 0xa8cf,
  0x00ab, 0xa1ec,
  0x00ac, 0xa1fe,
  0x00ad, 0xa1a9,
  0x00ae, 0xa8de,
  0x00b1, 0xa1be,
  0x00b2, 0xa9f7,
  0x00b3, 0xa9f8,
  0x00b4, 0xa2a5,
  0x00b5, 0xa5ec,
  0x00b6, 0xa2d2,
  0x00b7, 0xa2ab,
  0x00b8, 0xa3ac,
  0x00b9, 0xa9f6,
  0x00ba, 0xa8ac,
  0x00bb, 0xa1ed,
  0x00bc, 0xa8f9,
  0x00bd, 0xa8f6,
  0x00be, 0xa8fa,
  0x00bf, 0xa2af,
  0x00c5, 0xa1ca,
  0x00c6, 0xa8a1,
  0x00d0, 0xa8a2,
  0x00d7, 0xa1bf,
  0x00d8, 0xa8aa,
  0x00de, 0xa8ad,
  0x00df, 0xa9ac,
  0x00e6, 0xa9a1,
  0x00f0, 0xa9a3,
  0x00f7, 0xa1c0,
  0x00f8, 0xa9aa,
  0x00fe, 0xa9ad,
  0x0391, 0xa5c1,
  0x0392, 0xa5c2,
  0x0393, 0xa5c3,
  0x0394, 0xa5c4,
  0x0395, 0xa5c5,
  0x0396, 0xa5c6,
  0x0397, 0xa5c7,
  0x0398, 0xa5c8,
  0x0399, 0xa5c9,
  0x039a, 0xa5ca,
  0x039b, 0xa5cb,
  0x039c, 0xa5cc,
  0x039d, 0xa5cd,
  0x039e, 0xa5ce,
  0x039f, 0xa5cf,
  0x03a0, 0xa5d0,
  0x03a1, 0xa5d1,
  0x03a3, 0xa5d3,
  0x03a4, 0xa5d4,
  0x03a5, 0xa5d5,
  0x03a6, 0xa5d6,
  0x03a7, 0xa5d7,
  0x03a8, 0xa5d8,
  0x03a9, 0xa5d9,
  0x03b1, 0xa5e1,
  0x03b2, 0xa5e2,
  0x03b3, 0xa5e3,
  0x03b4, 0xa5e4,
  0x03b5, 0xa5e5,
  0x03b6, 0xa5e6,
  0x03b7, 0xa5e7,
  0x03b8, 0xa5e8,
  0x03b9, 0xa5e9,
  0x03ba, 0xa5ea,
  0x03bb, 0xa5eb,
  0x03bc, 0xa5ec,
  0x03bd, 0xa5ed,
  0x03be, 0xa5ee,
  0x03bf, 0xa5ef,
  0x03c0, 0xa5f0,
  0x03c1, 0xa5f1,
  0x03c2, 0xa5f2,
  0x03c3, 0xa5f3,
  0x03c4, 0xa5f4,
  0x03c5, 0xa5f5,
  0x03c6, 0xa5f6,
  0x03c7, 0xa5f7,
  0x03c8, 0xa5f8,
  0x03c9, 0xa5f9,
  0x2018, 0xa1ae,
  0x2019, 0xa1af,
  0x201c, 0xa1a8,
  0x201d, 0xa3a2,
  0x2020, 0xa2d3,
  0x2021, 0xa2d4,
  0x2026, 0xa1a6,
  0x2030, 0xa2b6,
  0x2032, 0xa1c7,
  0x2033, 0xa1c8,
  0x2190, 0xa1e7,
  0x2191, 0xa1e8,
  0x2192, 0xa1e6,
  0x2193, 0xa1e9,
  0x2194, 0xa1ea,
  0x21d2, 0xa2a1,
  0x21d4, 0xa2a2,
  0x2200, 0xa2a3,
  0x2202, 0xa1d3,
  0x2203, 0xa2a4,
  0x2207, 0xa1d4,
  0x2208, 0xa1f4,
  0x220b, 0xa1f5,
  0x220f, 0xa2b3,
  0x2211, 0xa2b2,
  0x221a, 0xa1ee,
  0x221d, 0xa1f0,
  0x221e, 0xa1c4,
  0x2220, 0xa1d0,
  0x2227, 0xa1fc,
  0x2228, 0xa1fd,
  0x2229, 0xa1fb,
  0x222a, 0xa1fa,
  0x222b, 0xa1f2,
  0x2234, 0xa1c5,
  0x223c, 0xa1ad,
  0x2260, 0xa1c1,
  0x2261, 0xa1d5,
  0x2264, 0xa1c2,
  0x2265, 0xa1c3,
  0x2282, 0xa1f8,
  0x2283, 0xa1f9,
  0x2286, 0xa1f6,
  0x2287, 0xa1f7,
  0x22a5, 0xa1d1,
  0x2660, 0xa2bc,
  0x2663, 0xa2c0,
  0x2665, 0xa2be,
};

static const CharMap gAsciiCompatMap[] =
{
   0x00a0, 0x0020, // "nbsp"
   0x00a1, 0x0021, // "iexcl"
   0x00a9, 0x0063, // "copy"
   0x00aa, 0x0061, // "ordf"
   0x00ae, 0x0052, // "reg"
   0x00b2, 0x0032, // "sup2"
   0x00b3, 0x0033, // "sup3"
   0x00b8, 0x002e, // "cedil"
   0x00b9, 0x0031, // "sup1"
   0x00ba, 0x006e, // "ordm"
   0x00bf, 0x003f, // "iquest"
   0x00c0, 0x0041, // "Agrave"
   0x00c1, 0x0041, // "Aacute"
   0x00c2, 0x0041, // "Acirc"
   0x00c3, 0x0041, // "Atilde"
   0x00c4, 0x0041, // "Auml"
   0x00c5, 0x0041, // "Aring"
   0x00c6, 0x0041, // "AElig"
   0x00c7, 0x0043, // "Ccedil"
   0x00c8, 0x0045, // "Egrave"
   0x00c9, 0x0045, // "Eacute"
   0x00ca, 0x0045, // "Ecirc"
   0x00cb, 0x0045, // "Euml"
   0x00cc, 0x0049, // "Igrave"
   0x00cd, 0x0049, // "Iacute"
   0x00ce, 0x0049, // "Icirc"
   0x00cf, 0x0049, // "Iuml"
   0x00d0, 0x0044, // "ETH"
   0x00d1, 0x004e, // "Ntilde"
   0x00d2, 0x004f, // "Ograve"
   0x00d3, 0x004f, // "Oacute"
   0x00d4, 0x004f, // "Ocirc"
   0x00d5, 0x004f, // "Otilde"
   0x00d6, 0x004f, // "Ouml"
   0x00d8, 0x004f, // "Oslash"
   0x00d9, 0x0055, // "Ugrave"
   0x00da, 0x0055, // "Uacute"
   0x00db, 0x0055, // "Ucirc"
   0x00dc, 0x0055, // "Uuml"
   0x00dd, 0x0059, // "Yacute"
   0x00de, 0x0054, // "THORN"
   0x00df, 0x0059, // "szlig"
   0x00e0, 0x0061, // "agrave"
   0x00e1, 0x0061, // "aacute"
   0x00e2, 0x0061, // "acirc"
   0x00e3, 0x0061, // "atilde"
   0x00e4, 0x0061, // "auml"
   0x00e5, 0x0061, // "aring"
   0x00e6, 0x0061, // "aelig"
   0x00e7, 0x0063, // "ccedil"
   0x00e8, 0x0065, // "egrave"
   0x00e9, 0x0065, // "eacute"
   0x00ea, 0x0065, // "ecirc"
   0x00eb, 0x0065, // "euml"
   0x00ec, 0x0069, // "igrave"
   0x00ed, 0x0069, // "iacute"
   0x00ee, 0x0069, // "icirc"
   0x00ef, 0x0069, // "iuml"
   0x00f0, 0x0064, // "eth"
   0x00f1, 0x006e, // "ntilde"
   0x00f2, 0x006f, // "ograve"
   0x00f3, 0x006f, // "oacute"
   0x00f4, 0x006f, // "ocirc"
   0x00f5, 0x006f, // "otilde"
   0x00f6, 0x006f, // "ouml"
   0x00f8, 0x006f, // "oslash"
   0x00f9, 0x0075, // "ugrave"
   0x00fa, 0x0075, // "uacute"
   0x00fb, 0x0075, // "ucirc"
   0x00fc, 0x0075, // "uuml"
   0x00fd, 0x0079, // "yacute"
   0x00fe, 0x0074, // "thorn"
   0x00ff, 0x0079, // "yuml"
};


static word CharMap_XLat(const CharMap *pmap, int cntMap, word ch)
{
   int nMin = 0;
   int nMax = cntMap;

   while (nMin < nMax) {
      int n = (nMin+nMax)>>1;

      if (pmap[n].a < ch) {
         nMin = n+1;
      } else if (pmap[n].a > ch) {
         nMax = n;
      } else {
         return pmap[n].b;
      }
   }
   return 0;
}


#define GAIJI_UCSMAX   (0xE000 + 188*10)     // highest Unicode value used for Gaiji (0xE758)

// Note: Our wide SJIS is byte-reversed from normal word representation
//
static INLINE uint16 CvtUCSToGaiji(AECHAR unicodeChar)
{
   unsigned char byLo, byHi;
   
   if (unicodeChar < 0xe000 || unicodeChar >= GAIJI_UCSMAX)
      return 0;

   // Get numeric value for each byte
   unicodeChar -= 0xE000;
   
   byHi = unicodeChar / 188;
   byLo = unicodeChar % 188;

   // Convert to characters
   
   byLo += 0x40;
   if (byLo >= 0x7F)
      ++byLo;

   byHi += 0xF0;

   return (byLo << 8) + byHi;
}

   
static uint16 CvtUCSToSJIS(uint16 chIn)
{
   uint16 ch = chIn;
   if (ch > 0x7F) {
      ch = CharMap_XLat(gSJISEntsMap, ARRAY_SIZE(gSJISEntsMap), chIn);
      if (!ch) {
         ch = CharMap_XLat(gAsciiCompatMap, ARRAY_SIZE(gAsciiCompatMap), chIn);
         if (!ch) {
            ch = CvtUCSToGaiji(chIn);
         }
      }
   }
   return ch;
}


static uint16 CvtUCSToKSC(uint16 chIn)
{
   uint16 ch = chIn;
   if (ch > 0x7F) {
      ch = CharMap_XLat(gKSCEntsMap, ARRAY_SIZE(gKSCEntsMap), chIn);
      if (!ch) {
         ch = CharMap_XLat(gAsciiCompatMap, ARRAY_SIZE(gAsciiCompatMap), chIn);
      }
   }
   return ch;
}


static uint16 CvtUCSToAscii(uint16 ch)
{
   if (ch > 0x7f) {
      ch = CharMap_XLat(gAsciiCompatMap, ARRAY_SIZE(gAsciiCompatMap), ch);
   }
   return ch;
}


/////////////////////////////////////////////////////////////////////////////
// CharSet object


// handler types (returned from FindUCSCharset)
enum {
   CSH_UTF8 = 0,     // UTF-8
   CSH_CP1250,       // CP1250
   CSH_CP1251,       // CP1251
   CSH_CP1252,       // CP1252
   CSH_8859_1,       // 8859-1
   CSH_8859_2,       // 8859-2
   CSH_8859_5,       // 8859-5
   CSH_KOI8_UNI,     // koi8-uni
   CSH_UCS_MAX = CSH_KOI8_UNI,   // Max UCS charset
   CSH_EUC_KR,       // EUC-KR
   CSH_EUC_CN,       // EUC-CN 
   CSH_S_JIS         // Shift-JIS
};


// Charset name lookup
//
// Here we list the charset names that are used to determine the encode/decode 
// functions that are used by the charset conversion functions.
// Names are those registered with IANA and others added for compatibility 
// with historical usage.
//
// NOTE: When the native encoding is Korean, Japanese, or Chinese, we don't
// have anything to lookup.  For each of those native charsets there is only
// one supported encoding.
//
//
// UCS (Unicode) charset converters
//
// Annotations:
//   "name" = name of an IANA encoding, and beginning of a series of aliases.
//   "MIME" = this name/alias is the "preferred MIME name"
//   "non-std" = non-standard alias not listed in the IANA documents
//
//
// Reference on IE: "Character Set Recognition":
//    http://msdn.microsoft.com/library/default.asp?url=/workshop/author/dhtml/reference/charsets/charset4.asp
//
static const char aszzUCSCharsets[] = {

   // CSH_UTF8

   "UTF-8\0"                                  // name

   // CSH_CP1250

   "windows-1250,"                            // name
   "CP1250\0"                                 // non-std  (historical)

   // CSH_CP1251
   
   "windows-1251,"                            // name
   "CP1251\0"                                 // non-std  (historical)

   // CSH_CP1252
   
   "windows-1252,"                            // name
   "CP1252\0"                                 // non-std  (historical)

   // CSH_8859_1
   
   "ISO_8859-1:1987,"                         // name
   "ISO-8859-1,"                              // MIME
   "ISO_8859-1," 
   "iso-ir-100,"
   "latin1,"
   "l1,"
   "IBM819,"
   "CP819,"
   "iso-latin-1,"                             // non-std
   
   "ANSI_X3.4-1968,"                          // name    [ASCII handled as iso-8859-1]
   "US-ASCII,"                                // MIME
   "ASCII,"
   "us,"
   "iso-ir-6,"
   "ANSI_X3.4-1986,"
   "ISO_646.irv:1991,"
   "ISO646-US,"
   "IBM367,"
   "cp367\0"

   // CSH_8859_2
   
   "ISO_8859-2:1987,"                         // name
   "iso-ir-101,"
   "ISO-8859-2,"                              // MIME
   "ISO_8859-2,"
   "latin2,"
   "l2"
   "iso-latin-2\0"                            // non-std

   // CSH_8859_5

   "ISO_8859-5:1988,"                         // name
   "ISO-8859-5,"                              // MIME
   "iso-ir-144,"
   "ISO_8859-5,"
   "cyrillic,"
   "GOST_19768-74,"                           // name   [iso-8859-5]
   "ST_SEV_358-88,"
   "iso-ir-153,"
   "iso-latin-5\0"                            // non-std

   // CSH_KOI8_UNI
   
   "ECMA-cyrillic,"                           // name   [koi8-uni]
   "iso-ir-111,"
   "KOI8-R,"                                  // name, MIME   [koi8-uni]
   "KOI8-U,"                                  // name, MIME   [koi8-uni]
   "KOI8-RU,"                                 // non-std
   "KOI8-UNI,"                                // non-std
   "KOI8-E"                                   // non-std
   "KOI8\0"                                   // non-std
};

//-- These encodings are supported, but only when they match the native
//   encoding:
//
// S-JIS
//  "Shift_JIS",                               // name, MIME
//  "MS_Kanji ",
//
// EUC-KR
//  "EUC-KR",                                  // name, MIME
//  "KS_C_5601-1987",                          // name  [euc-kr??????]
//  "iso-ir-149",
//  "KS_C_5601-1989",
//  "KSC_5601",
//  "korean",
//
// EUC-CN
//  "GB2312",                                  // name, MIME
//  "GB_2312-80",                              // name
//  "iso-ir-58",
//  "chinese",



/*
:: Obtain a UCS charset handler index, given a charset name.
::
:: Returns one of the CSH_XXX handler types
::
:: If not found, returns CSH_UTF8 (default)
*/
static int FindUCSCharset(const char *pszName, int cbName)
{
   const char *pszN = aszzUCSCharsets;
   const char *psz;
   int len;
   int ndx = 0;

   while (0 != (len = STRLEN(pszN))) {
      
      psz = pszN;
      
      while (*psz) {
         int cb = STRCHREND(psz, ',') - psz;
         if (cb == cbName && STRNICMP(psz, pszName, cb)==0) {
            goto done;
         }
         psz += cb;
         if (*psz == ',')
            ++psz;
      }

      pszN += len + 1;
      ndx++;
   }

done:
   // return default UTF8 if not found
   if (ndx > CSH_UCS_MAX) {
      ndx = CSH_UTF8;      
   }
   
   return ndx;
}



#define CSASSIGN(pe,pd,pc,pf,pu) \
   me->pfnEncode      = (pe);\
   me->pfnDecode      = (pd);\
   me->pfnCharSize    = (pc);\
   me->pfnFindSplit   = (pf);\
   me->pfnFromUnicode = (pu)
      
/*
:: CharSet_SetCharset
*/
void CharSet_SetCharset(CharSet *me, const char *pszCharset, int cbCharset)
{
   int nHandler = CSH_UTF8;

   if (me->wEncoding == AEE_ENC_EUC_KR) {
      nHandler = CSH_EUC_KR;
   
   } else if (me->wEncoding == AEE_ENC_EUC_CN) {
      nHandler = CSH_EUC_CN; 
   
   } else if (me->wEncoding == AEE_ENC_S_JIS) {
      nHandler = CSH_S_JIS;
   
   } else if (pszCharset) {
      nHandler = FindUCSCharset(pszCharset, cbCharset);
   }

   switch (nHandler) {

      case CSH_EUC_KR: 
         CSASSIGN(EncodeEUC, DecodeEUC, CharSizeEUC, FindSplitEUC, CvtUCSToKSC);
         break;
      
      case CSH_EUC_CN:
         CSASSIGN(EncodeEUC, DecodeEUC, CharSizeEUC, FindSplitEUC, CvtUCSToAscii);
         break;
      
      case CSH_S_JIS:
         CSASSIGN(EncodeSJIS, DecodeSJIS, CharSizeSJIS, FindSplitSJIS, CvtUCSToSJIS);
         break;
      
      case CSH_UTF8:
         CSASSIGN(EncodeUTF8, DecodeUTF8, CharSizeUTF8, FindSplitUTF8, 0);
         break;
      
      case CSH_CP1250:
         CSASSIGN(EncodeCP1250, DecodeCP1250, CharSizeSingle, 0, 0);
         break;
      
      case CSH_CP1251:
         CSASSIGN(EncodeCP1251, DecodeCP1251, CharSizeSingle, 0, 0);
         break;
      
      case CSH_CP1252:
         CSASSIGN(EncodeCP1252, DecodeCP1252, CharSizeSingle, 0, 0);
         break;
      
      case CSH_8859_1:
         CSASSIGN(Encode8859_1, Decode8859_1, CharSizeSingle, 0, 0);
         break;
      
      case CSH_8859_2:
         CSASSIGN(Encode8859_2, Decode8859_2, CharSizeSingle, 0, 0);
         break;
      
      case CSH_8859_5:
         CSASSIGN(Encode8859_5, Decode8859_5, CharSizeSingle, 0, 0);
         break;
      
      case CSH_KOI8_UNI:
         CSASSIGN(EncodeKOI8, DecodeKOI8,  CharSizeSingle, 0, 0);
         break;
   }
}


/*
:: CharSet_SetEncoding
*/
void CharSet_SetEncoding(CharSet *me, uint16 wEncoding)
{
   me->wEncoding = wEncoding;
   CharSet_SetCharset(me, NULL, 0);
}


/*
:: CharSet_Encode
*/
int CharSet_Encode(CharSet *me, char *pcDest, int cbDest, 
                          const AECHAR *pwcSrc, int cwSrc, int *pnUsed) {

   PFNENCODE Encode = me->pfnEncode;

   int nUsed = 0;
   int cch = 0;

   if (cwSrc < 0) {
      cwSrc = WSTRLEN(pwcSrc);
   }

   // counting only?
   if (!pcDest || !cbDest) {
      int used = 0;
      char buf[32];
      const AECHAR *pwc = pwcSrc;
      while (cwSrc > 0) {
         cch += Encode(buf, sizeof(buf), pwc, cwSrc, &used);
         pwc += used;
         cwSrc -= used;
         nUsed += used;
      }

   } else {
      cch = Encode(pcDest, cbDest, pwcSrc, cwSrc, &nUsed);
   }

   if (pnUsed) {
      *pnUsed = nUsed;
   }

   return cch;
}


/*
:: CharSet_EncodeTerminate
*/
int CharSet_EncodeTerminate(CharSet *me, char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   int cch;

   if (cbDest) {
      cbDest--;   // allow for zero-terminator unless just counting
   }

   cch = CharSet_Encode(me, pcDest, cbDest, pwcSrc, cwSrc, pnUsed);
   if (pcDest) {
      pcDest[cch] = (char)0;
   } else {
      cch++;      // just counting, count zero-terminator also
   }
   return cch;    // return number of AECHARs written/counted
}


/*
:: CharSet_Decode
:: 
*/
int CharSet_Decode(CharSet *me, AECHAR *pwDest, int cwDest, 
                           const char *pcSrc, int cbSrc, int *pcbUsed) {
   
   PFNDECODE Decode = me->pfnDecode;

   int nUsed = 0;
   int cch = 0;

   if (cbSrc < 0) {
      cbSrc = STRLEN(pcSrc);
   }

   // counting only?
   if (!pwDest || !cwDest) {

      AECHAR wBuf[32];
      const char *p = pcSrc;
      int used;

      while (cbSrc > 0) {
         cch += Decode(wBuf, sizeof(wBuf), p, cbSrc, &used);
         pcSrc += used;
         cbSrc -= used;
         nUsed += used;
      }

   } else {
      cch = Decode(pwDest, cwDest, pcSrc, cbSrc, &nUsed);
   }

   if (pcbUsed) {
      *pcbUsed = nUsed;
   }

   return cch;
}


/*
:: CharSet_DecodeTerminate
*/
int CharSet_DecodeTerminate(CharSet *me, AECHAR *pwDest, int cwDest, const char *pcSrc, int cbSrc, int *pnUsed)
{
   int cch;

   if (cwDest) {
      cwDest--;   // allow for zero-terminator unless just counting
   }

   cch = CharSet_Decode(me, pwDest, cwDest, pcSrc, cbSrc, pnUsed);
   if (pwDest) {
      pwDest[cch] = (AECHAR)0;
   }
   return ++cch;    // return number of AECHARs written/counted incl. zero-term
}



/////////////////////////////////////////////////////////////////////////////
// CharSet creation


/*
:: CharSet_Ctor
*/
void CharSet_Ctor(CharSet *me, uint16 wEncoding)
{
   CharSet_SetEncoding(me, wEncoding);
}


/*
:: CharSet_New
*/
int CharSet_New(CharSet **ppo, uint16 wEncoding)
{
   CharSet *me = MALLOCREC(CharSet);
   if (!me) {
      return ENOMEMORY;
   }
   CharSet_Ctor(me, wEncoding);
   return SUCCESS;
}



/////////////////////////////////////////////////////////////////////////////


#ifdef DOX_UNITTEST
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Unit Test



static INLINE boolean StrEQ(const char *s1, const char *s2)
{
   return (s1 && s2 && STRCMP(s1,s2) == 0);
}


static INLINE int IsHexDigit(char ch)
{
   return BETWEEN(ch, '0', '9'+1) || BETWEEN((ch|32), 'a', 'g');
}


static INLINE int HexValue(char ch)
{
   ch = (ch | 32) - '0';

   if (ch > 9)
      ch -= ('a' - '9' - 1);

   return ch;
}


static int ReadHex(const char **ppch, int *pn)
{
   const char *pch = *ppch;
   int nResult = 0;

   if (IsHexDigit(*pch)) {
      while (IsHexDigit(*pch)) {
         nResult = (nResult << 4) | HexValue(*pch);
         ++pch;
      }

      *pn = nResult;
      *ppch = pch;

      return 1;
   }

   return 0;
}


static int ReadWords(uint16 *pwBuf, const char **ppch)
{
   const char *pch = *ppch;
   unsigned int n;
   uint16 *pw = pwBuf;
   
   while (1) {
      while (*pch == 32)
         ++pch;

      if (!ReadHex(&pch, (int*)&n) || n > 0xFFFF) {
         break;
      }

      *pw++ = n;
   }

   *ppch = pch;
   return pw - pwBuf;
}


static int ReadBytes(byte *pbyBuf, const char **ppch)
{
   const char *pch = *ppch;
   int n;
   byte *pby = pbyBuf;
   
   while (1) {
      while (*pch == 32)
         ++pch;

      if (!ReadHex(&pch, &n) || (unsigned)n > 0xFF) {
         break;
      }

      *pby++ = n;
   }

   *ppch = pch;
   return pby - pbyBuf;
}


// EncTestCase
//
// Test case strings contain comma-delimited tests.  Each case looks like:
//
//    x x x = x x [+x]
//
//  x = any hex value.  LHS = bytes;  RHS = words (expanded)
//
//  Optional "+x" indicates leftover bytes expected.
//
typedef struct {
   char *    pszName;   // name of the encoding
   char *    pszTest;   // test cases
} EncTestCase;


static const char *CharsetTest(EncTestCase *ptc)
{
   const char *psz = ptc->pszTest;
   int ii;
   CharSet charset;

   if (StrEQ("SJIS", ptc->pszName)) {
      CharSet_SetEncoding(pcs, AEE_ENC_S_JIS);
   
   } else if (StrEQ("EUC", ptc->pszName)) {
      CharSet_SetEncoding(pcs, AEE_ENC_EUC_CN);
   
   } else {
      CharSet_SetCharset(pcs, ptc->pszName, STRLEN(ptc->pszName));
   }

   while (*psz) {

      // Parse test string

      char   achBytes[100];    // test bytes (LHS)
      AECHAR awWords[100];     // test words (RHS)
      uint16 wUnused = 0;

      int cntBytes, cntWords;
      int bDecode = 1, bEncode = 1;

      cntBytes = ReadBytes((byte*)achBytes, &psz);
      if (*psz == '>') {
         bEncode = 0;              // decode only
      } else if (*psz == '<') {
         bDecode = 0;              // encode only
      } else if (*psz != '=') {
         return "internal1";
      }
      ++psz;   // skip '=', '>', or '<'

      cntWords = ReadWords(awWords, &psz);

      if (*psz == '+') {
         // Get number of bytes expected to remain unused
         ++psz;
         ReadWords(&wUnused, &psz);
      }

      // expect end of string
      if (*psz) {
         if (*psz != ',') {
            return "internal2";
         }
         ++psz;   // skip ','
      }
      
      // Test decode

      if (bDecode) {
         int nLimit = cntWords + 4;
         AECHAR awTmp[100];
         int cntTmp, cntUsed;

         for (; nLimit >= 0; --nLimit) {

            awTmp[nLimit] = 0x5a5a;
     
            cntTmp = CharSet_Decode(&charset, awTmp, nLimit, achBytes, cntBytes, &cntUsed);
           
            if (cntTmp > nLimit || awTmp[nLimit] != 0x5a5a || cntTmp < 0) {
               return "decodeBounds!";
            }
           

            if (cntTmp < cntWords) {
               int cntUsed2;

               // Output was truncated:  resume where we left off with no size limie

               cntTmp += CharSet_Decode(&charset, awTmp+cntTmp, cntWords-cntTmp,
                                        achBytes+cntUsed, cntBytes-cntUsed, &cntUsed2);
               cntUsed += cntUsed2;
            }

            if (cntUsed != cntBytes - wUnused) {
               return "unusedBytes";       // Should use all bytes except what we expect to be unused
            }

            // compare output with expected
            if (cntTmp != cntWords) {
               // resulting size not what expected
               return "decodeOutputSize";
            }

            for (ii = 0; ii < cntTmp; ++ii) {
               if (awTmp[ii] != awWords[ii]) {
                  return "decodeOutput";
               }
            }
         }

      }

      // Test encode

      if (bEncode) {
         int cntExpected = cntBytes - wUnused;
         int nLimit = cntExpected + 4;
         char ach[100];   // temp output buffer
         int  cnt;        // temp otuput count
         int  cwUsed;

         for (; nLimit >= 0; --nLimit) {

            ach[nLimit] = 0x5a;
     
            cnt = CharSet_Encode(&charset, ach, nLimit, awWords, cntWords, &cwUsed);
           
            if (cnt > nLimit || ach[nLimit] != 0x5a || cnt < 0) {
               return "encodeBounds!";
            }

            if (cnt < cntExpected) {
               int cwUsed2;

               // Output was truncated:  resume where we left off with no size limie

               cnt += CharSet_Encode(&charset, ach+cnt, cntExpected-cnt,
                                     awWords+cwUsed, cntWords-cwUsed, &cwUsed2);
               cwUsed += cwUsed2;
            }

            if (cwUsed != cntWords) {
               return "unusedWords";         // Should use all bytes except what we expect to be unused
            }

            // compare output with expected
            if (cnt != cntExpected) {
               // resulting size not what expected
               return "encodeOutputSize";
            }

            for (ii = 0; ii < cnt; ++ii) {
               if (ach[ii] != achBytes[ii]) {
                  return "encodeOutput";
               }
            }
         }
      }
   }


   return 0;
}


static void TestEnc(void)
{
   EncTestCase tests[] = {
      {
         "EUC",  //  hardcoded for "EUC <-> BREW-EUC"

         "00 7f 80 81 ff fe = 0000 007f 8180 feff,"
         "41 80 = 0041 +1",
      },

      {
         "SJIS",

         "00 7f 81 40  81 ff  9f 40  9f ff  a0 df  e0 40  ef ff  f0 40  fc fc  ff ff ="
         "00 7f  4081   ff81   409f   ff9f  a0 df   40e0   ffef   40f0   fcfc  ff ff,"

         "20 81 = 20 +1,"
         "e0 = +1,"

         // bogus input:  80 xx => 80 xx or xx80 ?
      },

      {
         "utf-8",  // this will find utf-8 <-> Unicode converters
        
        // 1-byte form
        "00 01 7f = 00 01 7f,"

        // 2-byte
        "c0 80  c1 80  c2 80  c0 bf  df 80  df bf > 0000 0040 0080 003f 07c0 07ff,"
      
        // 3-byte
        "e0 80 80  e0 80 bf  e0 bf 80  e0 bf bf  ef 80 80  ef bf bf > 0000 003f 0fc0 0fff f000 ffff,"
        "00        3f        e0 bf 80  e0 bf bf  ef 80 80  ef bf bf < 0000 003f 0fc0 0fff f000 ffff,"

        // 4-byte
        "f0 80 80 80 > 0000,"
        "f3 bf bf bf > DBBF DFFF,"   // encoding is not yet utf-16-aware

        // incomplete
        "c0 = +1,"       // 2-byte   "+1" => 1 byte unused
        "40 c0 = 40 +1,"
        "df = +1,"

        "e0 = +1,"       // 3-byte
        "e0 80 = +2,"

        "f0 = +1,"       // 4-byte
        "f0 80 = +2,"
        "f0 80 80 = +3,"
        "f7 80 80 = +3,"

        // invalid 1-byte: treat as single-byte
        // "80 bf = 80 bf,"

        // invalid 2-byte
        // "c0 00  c0 ff = 0000 003f"
      },

      {
         "iso-8859-1",

         // one-to-one with Unicode
         "00 1f 20 7f 80 9f a0 a1 b0 dd ef ff = 00 1f 20 7f 80 9f a0 a1 b0 dd ef ff"
      },

      {
         "iso-8859-2",

         // One-to-one up to 0xA1
         "00 1f 20 7f 80 9f  a0  a1  bb  cc  dd  ee  ff ="
         "00 1f 20 7f 80 9f  a0 104 165 11a  dd  ee 2d9",
         
      },

      
      {
         "iso-8859-5",
         
         // One-to-one up to 0xA1
         "00 1f 20 7f 80 9f  a0  a1  bb  cc  dd  ee  ff ="
         "00 1f 20 7f 80 9f  a0 401 41b 42c 43d 44e 45f",
      },

      {
         "windows-1250",

         "00 1f 20 7f   80   81   82   99   9f  a0  a1  bb  cc  dd  ee  ff ="
         "00 1f 20 7f 20ac   81 201a 2122 017a  a0 2c7  bb 11a  dd  ee 2d9"
      },

      
      {
         "windows-1251",

         "00 1f 20 7f   80   81   82   99   9f  a0  a1  bb  cc  dd  ee  ff ="
         "00 1f 20 7f 0402 0403 201a 2122 045f  a0 40e  bb 41c 42d 43e 44f"
      },

      
      {
         "windows-1252",

         "00 1f 20 7f   80   81   82   99   9f  a0  a1  bb  cc  dd  ee  ff ="
         "00 1f 20 7f 20ac   81 201a 2122 0178  a0  a1  bb  cc  dd  ee  ff"
      },

      
      {
         "KOI8-R",
         
         "00 1f 20 7f   80   81   82   99   9f  a1  bb  cc  dd  ee  ff ="
         "00 1f 20 7f 2500 2502 250c 2122   a4 452 40b 43b 449 41d 42a,"

         "9a a0 > a0 a0,"   // reverse could be A0 or 9A

         "3f < 9999"       // map unknown to '?'

      }      
   };

   int ii;
   EncTestCase *ptc = tests;

   for (ii = 0; ii < ARRAY_SIZE(tests); ++ii, ++ptc) {

      const char *pszError = CharsetTest(ptc);

      if (pszError) {
         DBGPRINTF("  ERROR enc %s (%s)", pszError, ptc->pszName);
         break;
      }
   }
   DBGPRINTF("  SUCCESS enc");
}



static void TestParam(void)
{
   struct {
      const char *pszName;
      const char *pszValue;
      const char *pszMedia;
   } tests[] = {
      { "charset", "utf-8", "text/html;charset=utf-8" },
      { "charset", "utf-8", "text/html;charset=\"utf-8\"" },
      { "charset", "utf-8", " text/html ;charset=utf-8 " },
      { "charset", "utf-8", " text/html ; charset=utf-8 " },
      { "charset", "utf-8", " text/html ; format=flowed; charset = \"utf-8\" " },
      { "charset", "utf-8", " text/html ; format=flowed ;charset = \"utf-8\" " },
      { "charset", "utf-8", "text/plain ; format=\"flowed\";charset=utf-8 " },
      { "charset", "utf-8", "text/plain ; format=\"flowed\" ;charset=utf-8 " },
   };

   int ii;

   for (ii = 0; ii < ARRAY_SIZE(tests); ++ii) {

      int cb;
      
      const char *psz = FindParamValue(tests[ii].pszMedia, tests[ii].pszName, &cb);

      if (!psz ||
          cb != (int) STRLEN(tests[ii].pszValue) ||
          STRNCMP(psz, tests[ii].pszValue, cb) != 0) {
         DBGPRINTF("  ERROR FindParamValue %d", ii);
         return;
      }
   }

   DBGPRINTF("  SUCCESS FindParamValue");
}


// Test FindUCSCharset() function.
//
static const char * TestFindCharsetCases(void)
{
   const CharSet *pdc;

   const struct {
      const char *pszName;
      PFNENCODE pfnEnc;
      PFNDECODE pfnDec;
   } tv[] = {
      "utf-8",            EncodeUTF8,      DecodeUTF8,
      "windows-1250",     EncodeCP1250,    DecodeCP1250,
      "windows-1251",     EncodeCP1251,    DecodeCP1251,
      "windows-1252",     EncodeCP1252,    DecodeCP1252,
      "cp1252",           EncodeCP1252,    DecodeCP1252,
      "KOI8-R",           EncodeKOI8,      DecodeKOI8,
      "iso-8859-5",       Encode8859_5,    Decode8859_5,
      "cyrillic",         Encode8859_5,    Decode8859_5,
      "iso-8859-1",       Encode8859_1,    Decode8859_1,
      "ISO_8859-2:1987",  Encode8859_2,    Decode8859_2,
   };
      
   int ii;

   // junk at end not part of name
   if (!FindUCSCharset("utf-8x", 5, &pdc)) {
      return "unkA";
   }

   // ensure mismatch
   if (FindUCSCharset("iso-8859-1", 2, &pdc)) {
      return "matchesTooShort";
   }

   // ensure mismatch
   if (FindUCSCharset("windows-1252", 0, &pdc)) {
      return "matchesEmptyString";
   }

   if (pdc->pfnEncode != EncodeUTF8 || pdc->pfnDecode != DecodeUTF8) {
      return "badDefault";
   }
   
   // Try all
   for (ii = 0; ii < ARRAY_SIZE(tv); ++ii) {

      if (!FindUCSCharset(tv[ii].pszName, STRLEN(tv[ii].pszName), &pdc)) {
         return "unkCharset";
      }

      if (pdc->pfnEncode != tv[ii].pfnEnc || pdc->pfnDecode != tv[ii].pfnDec) {
         return "utf8.mismatch";
      }
   }

   return NULL;
}


// Run tests and print any error message.
//
static void TestFindCharset(void)
{
   const char *pszTest = "FindCharset";
   const char *pszError = TestFindCharsetCases();

   if (pszError) {
      DBGPRINTF("  ERROR %s %s", pszTest, pszError);
   } else {
      DBGPRINTF("  SUCCESS %s", pszTest);
   }
}


// Test DoxPrevItem() and DoxParent() against DoxNextItem()
//
static void TestNav(Dox *me)
{
   DoxItem i;
   const char *pchSE[MAXELEMNEST];
   int cntSE = 0;

   DoxItem_Ctor(&i, me, 0);

   while (DoxItem_Next(&i)) {
      char ch = i.pc[0];
      const char *pc;
      
      // Try PrevItem in all places in this item
      for (pc = i.pc + i.cb; pc > i.pc; --pc) {
         if (xDoxPrevItem(pc) != i.pc) {
            DBGPRINTF("  ERROR Dox_Test:PrevItem");
         }
      }

      // Track elements
      
      if (ch == DOX_SE) {

         // Start new element
         pchSE[cntSE++] = i.pc;

      } else if (ch == DOX_EE) {

         // End element
         --cntSE;

         if (cntSE < 0) {
            DBGPRINTF("  ERROR Dox_Test: bad nesting!");
         } else {
            // Try all child positions in the just-handled element
            const char *pchParent;

            pchParent = pchSE[cntSE];
            
            for (pc = i.pc; pc > pchParent; --pc) {
               const char *pcUp = pc;

               do {
                  pcUp = DoxParent(pcUp, me->pcBuf-1);
               } while (pcUp > pchParent);

               if (pcUp != pchParent) {
                  DBGPRINTF("  ERROR Dox_Test: DoxParent error");
               }
            }
         }
      }
   }
   DBGPRINTF("Dox_Test:  Tested %d bytes", (i.pc - me->pcBuf));
}



void Dox_Test(Dox *me)
{
   TestNav(me);   // test curent contents of buffer...
   TestParam();
   TestFindCharset();
   TestEnc();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#endif // DOX_UNITTEST



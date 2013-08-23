/*===================================================================
FILE: dox.c

SERVICES:  Structured Document Object, supporting XML/HTML

DESCRIPTION:

PUBLIC CLASSES AND FUNCTIONS:

        Copyright 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===================================================================*/

#include "AEE.h"
#include "AEEStdLib.h"
#include "stdarg.h"
#include "doxhtml.h"
#include "AEEWidget.h"
#include "AEEDisplayCanvas.h"
#include "DoxPos.h"
#include "htmlwidget_res.h"

#define INLINE __inline

#define RIGHT(prc)  ((prc)->x + (prc)->dx)
#define BOTTOM(prc)  ((prc)->y + (prc)->dy)

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

// Pen Tracking Hit flags 8-bit (1 byte)
#define  PTRCK_HIT_ABOVE   (0x01)      // Above the thumb, y position < thumb.y OR x position < thumb.x
#define  PTRCK_HIT_THUMB   (0x02)      // On the thumb
#define  PTRCK_HIT_ITEM    (0x02)      // Matches above, overloaded for non SB entities
#define  PTRCK_HIT_BELOW   (0x04)      // Below the thumb, y position > thumb.y + thumb.dy OR x position < thumb.x + thumb.dx
#define  PTRCK_HIT_SCRL    (0x08)      // 1: Hit Scrollbar; 0: Didn't hit Scrollbar
#define  PTRCK_NOTTHUMB    (PTRCK_HIT_BELOW | PTRCK_HIT_ABOVE)

// Pen Tracking General flags
#define  PTRCK_GEN_TMRSET  (0x01)   // Timer is set
#define  PTRCK_GEN_HITCTL  (0x02)   // Stored Pos is a ICtlObj



#define DoxIsAtElement(pd,pos)   (DOX_SE==Dox_BufGetChar((pd),(pos)))


// forward decls
static void    Dox_CheckFocus    (Dox *me);
static boolean Dox_Jump          (Dox *me, int posLink); 
static int     Dox_NextItem      (Dox *me, int pos);
static void    Dox_SetScrollPos  (Dox *me, int32 yTop);


static __inline void Dox_SendScrollNotify(Dox *me) {
   uint16 visible = (uint16) me->dsi.yRectHeight;
   uint16 range   = (uint16) me->dsi.yDocHeight;
   uint16 pos     = (uint16) me->dsi.yTop;
   IHTMLVIEWMODEL_NotifyScroll(me->piModel, range, visible, pos);
}


/*=============================================================================

   Items and iteration

 =============================================================================*/


typedef struct {
   int pos;
   int cb;
   int cbUsed;
   const char *pc;
   Dox *pdox;
} DoxItem;


typedef struct {
   DoxItem     doxitem;
   int         nLevel;
   char        chElem;
// test only //////////
//   int         nn;
} DoxIter;


static void     DoxIter_Ctor   (DoxIter *me, Dox *pdox, int pos);
static boolean  DoxIter_Next   (DoxIter *me);
static boolean  DoxIter_Find   (DoxIter *me, char chElem);



//
//----  Dox Utility Functions
//


boolean xDoxStrEQI(const char *s1, const char *s2)
{
   return (s1 && s2 && STRICMP(s1,s2) == 0);
}



// DoxConstrain: Constrain val to range nMin...nMax (inclusive).
//
// When nMax < nMin, nMin will be returned.
//
static int DoxConstrain(int val, int nMin, int nMax)
{
   if (val > nMax)
      val = nMax;
   if (val < nMin)
      val = nMin;

   return val;
}


/*=============================================================================

   Character set encoding and decoding

 =============================================================================*/


//----  Macros


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





//-------- Charset parsing
//
// This section supports parsing MIME media types, including parameters such
// as 'charset'.
//
// Media type syntax per [HTTP1/1]:
//
//      media-type    = token "/" token *( ";" parameter)
//      parameter     = token "=" (token | quoted-string)
//      quoted-string = <"> *(qdtext) <">
//      qdtext        = <any text except <">>
//
//   - no space between attribute and value in a parameter  [HTTP/1.1]
//
//   - charset = 40 characters, case insensitive  [http://www.iana.org/assignments/character-sets]
//


//#define TYPEMATCH_DELIM(c)  ((unsigned char)(c) <= 32 || (c) == ';')
//
//
//static int TypeMatch(const char *pszMedia, const char *pszType)
//{
//   int nLen = STRLEN(pszType);
//   
//   return STRNICMP(pszType, pszMedia, nLen) == 0 && TYPEMATCH_DELIM(pszMedia[nLen]);
//}


#define ISWHITESPACE(ch)   (CHARISIN((ch), 1, 32))


typedef struct {
   const char *pszName;
   int         cbName;
   const char *pszValue;
   int         cbValue;
} TypeParam;


static int GetNextParam(TypeParam *ptp, const char **ppsz)
{
   const char *psz = *ppsz;
   
   ZEROAT(ptp);
   
   do {

      // Get Name

      while (ISWHITESPACE(*psz))
         ++psz;

      if (*psz == '\0')
         break;

      ptp->pszName = psz;

      while (*psz && !ISWHITESPACE(*psz) && *psz != ';' && *psz != '=')
         ++psz;

      ptp->cbName = psz - ptp->pszName;

      // Get Value

      while (ISWHITESPACE(*psz))
         ++psz;
      
      if (*psz == '=') {

         ++psz;
         while (ISWHITESPACE(*psz))
            ++psz;

         if (*psz == '\"') {
            ++psz;

            ptp->pszValue = psz;

            while (*psz && *psz != '\"')
               ++psz;

            ptp->cbValue = psz - ptp->pszValue;

            if (*psz == '\"')
               ++psz;
         } else {

            ptp->pszValue = psz;

            while (*psz && !ISWHITESPACE(*psz) && *psz != ';')
               ++psz;

            ptp->cbValue = psz - ptp->pszValue;
         }

         while (ISWHITESPACE(*psz))
            ++psz;
      }

      while (*psz && *psz != ';')
         ++psz;

      if (*psz == ';')
         ++psz;

   } while (ptp->cbName == 0 && ptp->cbValue == 0);

   *ppsz = psz;

   return (ptp->cbName || ptp->cbValue);
}



static const char *FindParamValue(const char *pszMedia, const char *pszName, int *pcb)
{
   int cbName = STRLEN(pszName);
   TypeParam tp;

   while (GetNextParam(&tp, &pszMedia)) {
      if (tp.cbName == cbName && STRNICMP(tp.pszName, pszName, cbName) == 0) {
         *pcb = tp.cbValue;
         return tp.pszValue;
      }
   }

   *pcb = 0;
   return NULL;
}




/*=============================================================================
 =============================================================================*/


// DoxDrawTextA : Draw an 8-bit string.
//
// Unsupported flags:
//   Any horizontal alignment except IDF_ALIGN_LEFT
//
static int DoxDrawTextA(IDisplay *pid, AEEFont fnt, const char * pcText, int nChars,
                        int x, int y, const AEERect * prcBg, uint32 dwFlags, PFNDECODE pfnDecode)
{
   int rv = SUCCESS;
   int cbUsed;

   for (;;) {
      int cb;

      AECHAR gaBuffer[32];     // temp buffer for DoxExpandChars

//      BRIDLE_ASSERT_SAFE_PFN(pfnDecode);
      cb = pfnDecode(gaBuffer, ARRAYSIZE(gaBuffer), pcText, nChars, &cbUsed);

      rv = IDISPLAY_DrawText(pid, fnt, gaBuffer, cb, x, y, prcBg, dwFlags);
//      DBGPRINTF("Dox-DrawText @ y=%d", y);

      nChars -= cbUsed;
      if (nChars <= 0 || cbUsed == 0)
         return rv;

      pcText += cbUsed;
      dwFlags &= ~(IDF_RECT_MASK);   // already drawn
      x += IDISPLAY_MeasureTextEx(pid, fnt, gaBuffer, cb, -1, NULL);
   }
}


// Measure an 8-bit string
//
static int DoxMeasureTextA(IDisplay *pid, IFont *piFont, const char *pc, int cb,
                           int nLineWid, int *pcbFits, PFNDECODE pfnDecode)
{
   int wid = 0;
   int cbFits = 0;
   int cbExp, cw, cwnpix;

   if (nLineWid == 0) {
      // Work around flaw in IDISPLAY (width of 0 is special case there, so we
      // have to waste our time checking for it elsewhere)
      *pcbFits = 0;
      return 0;
   }
         
   while (cb) {      
      int cwOut;
      AECHAR gaBuffer[32];
	  //BRIDLE_ASSERT_SAFE_PFN(pfnDecode);
      cwOut = pfnDecode(gaBuffer, ARRAYSIZE(gaBuffer), pc, cb, &cbExp);
      //wid += IDISPLAY_MeasureTextEx(pid, AEE_FONT_NORMAL, gaBuffer, cwOut, nLineWid - wid, &cw);
	  IFONT_MeasureText( piFont, gaBuffer, cwOut, (nLineWid - wid), &cw, &cwnpix);
	  wid += cwnpix;
      if (cw < cwOut) {
         // Find how many 8-bit characters it takes to make cw wide characters
		 // BRIDLE_ASSERT_SAFE_PFN(pfnDecode);
         pfnDecode(gaBuffer, cw, pc, cb, &cbExp);
         cbFits += cbExp;
         break;
      }

      if (cbExp == 0) {
         // @@ Ended in incomplete multibyte character... ignore.
         break;
      }
      
      cbFits += cbExp;

      cb -= cbExp;
      pc += cbExp;
   }

   *pcbFits = cbFits;
   return wid;
}




//
//---  DoxItem:  item enumeration functions
//


static void DoxItem_Ctor(DoxItem *me, Dox *pdox, int pos)
{
   me->cb = 0;
   me->cbUsed = 0;
   me->pos = pos;
   me->pc = pdox->pcBuf + pos;
   me->pdox = pdox;
}


// On entry:
//   pos = start of previous item
//   cb = size of previous item
//
// On exit:
//   pos     = start of item (position in document)
//   pc      = start of item (pointer)
//   cb      = size of item
//   cbUsed  = size of item
//
static int DoxItem_Next(DoxItem *me)
{
   int cbUsed = me->cbUsed;
   int pos = me->pos + cbUsed;
   int cb = me->cb - cbUsed;

   if (cb <= 0) {
      cb = Dox_NextItem(me->pdox, pos) - pos;
      if (cb < 0)
         cb = 0;
   }
   
   me->pos = pos;
   me->pc += cbUsed;
   me->cb = cb;
   me->cbUsed = cb;

   return cb;
}

#define LFCHAR '\012'

// Also stops at LF
//
static int DoxItem_NextBR(DoxItem *me)
{
   int cbUsed = me->cbUsed;
   int pos = me->pos + cbUsed;
   int cb = me->cb - cbUsed;

   if (cb <= 0) {
      const char *pc = me->pdox->pcBuf + pos;

      if (!DoxIsElemDelim(*pc)) {
         if (pc[0] == LFCHAR) {
            cb = 1;
         } else {
            // Find delim or LFCHAR
            const char *pcStart = pc;

            while (!CHARISIN(*pc, 1, LFCHAR))   // DOX_EE, DOX_SE, ...
               ++pc;

            cb = pc - pcStart;
         }
      } else {
         cb = Dox_NextItem(me->pdox, pos) - pos;
      }
      if (cb < 0)
         cb = 0;
   }
   
   me->pos = pos;
   me->pc += cbUsed;
   me->cb = cb;
   me->cbUsed = cb;

   return cb;
}



//
//---  Invalid range
//

#ifdef DOXEDITING

INLINE int DoxInvalRange_IsModified(DoxInvalRange *me)
{
   return (me->posA != me->posB) || (me->nIns != 0);
}


// Set to "unmodified" state.
//
INLINE void DoxInvalRange_Reset(DoxInvalRange *me)
{
   me->posA = 0;
   me->posB = 0;
   me->nIns = 0;
}


// Record 'replace' modification
//   pos = start of modification
//   cbDel = number of characters deleted after pos
//   cbIns = number of characters inserted after pos
//
// Caller asserts "Text that WAS after pos+cbDel NOW resides at pos+cbIns."
//
INLINE void DoxInvalRange_Replace(DoxInvalRange *me, int pos, int cbDel, int cbIns)
{
   int posA = me->posA;
   int posB = me->posB;
   int nIns = cbIns - cbDel;  // net insertion
   
   if (!cbDel && !cbIns)
      // nothing modified
      return;

   if (posA == posB && me->nIns == 0) {
      // nothing invalid
      me->posA = pos;
      me->posB = pos + cbIns;
      me->nIns = nIns;
      return;
   }

   if (pos < posA)
      me->posA = pos;

   // extend posB ?
   if (pos + cbDel > posB)
      posB = pos + cbDel;

   me->posB = posB + nIns;
   me->nIns += nIns;
}

#endif // DOXEDITING


//
//---  Buffer manipulation
//


// Error: Record an error condition
//
void xDox_ErrorSet(Dox *me, int fError)
{
   me->fErrors |= fError;
}


// Error: Record an error condition
//
//static void Dox_ErrorClear(Dox *me, int fError)
//{
//   me->fErrors &= ~fError;
//}


#if defined(DOXEDITING)

// BufReplace: Delete and/or insert data
//
//   pos = position in buffer to modify (will be trimmed to nearest valid value)
//   cbDel = number of bytes starting at pos to delete (will be trimmed to nearest valid value)
//   cbIns = number of bytes to insert at pos
//   pcIns[] = bytes to insert
// 
// If inserted text will not fit, the buffer is left unmodified to help
// callers maintain integrity of data (BufReplace is an all-or-nothing
// function).
//
// Return value = error code (0 on success)
//
int Dox_BufReplace(Dox *me, int pos, int cbDel, const char *pcIns, int cbIns)
{
   char *pcRplc;
   int cbData = me->cbData;

   pos = DoxConstrain(cbDel, 0, cbData);
   cbDel = DoxConstrain(cbDel, 0, cbData - pos);

   if (cbData - cbDel + cbIns  > me->cbBuf) {
      // Insertion will not fit
      Dox_Error(me, DOX_ERROR_BUFTRUNC);
      return DOX_ERROR_BUFTRUNC;
   }

   pcRplc = me->pcBuf + pos;
   
   MEMMOVE(pcRplc+cbIns, pcRplc+cbDel, cbData - (pos + cbDel));

   MEMMOVE(pcRplc, pcIns, cbIns);

   // Update "master" invalid region (which in turn can be propagated to
   // others when needed)

   DoxInvalRange_Replace(&me->inv, pos, cbDel, cbIns);

   return 0;
}


// Get pointer to position in document and number of contiguous bytes present
// at that position in memory.
//
//char * Dox_BufGetPtrLen(Dox *me, int pos, int *pcbLen)
//{
//   pos = DoxConstrain(pos, 0, me->cbData);
//
//   *pcbLen = me->cbData - pos;
//   return me->pcBuf + pos;
//}

#endif // DOXEDITING

static char Dox_BufGetChar(Dox *me, int pos)
{
   return me->pcBuf[pos];
}


int xDox_BufGetSize(Dox *me)
{
   return me->cbData;
}



// Find attribute value in list.
//
// On entry:
//   pszElem = pointer to start of element (DOX_SE char)
//
// On exit:
//    Return value: pointer to value, if found.  NULL if attr not present.
//
//    If first character of value is DOX_NV, it indicates that no value was
//    specified for the attribute.  Otherwise, value will be a zero-terminated
//    string.
//
const char *xDoxGetAttrValue(const char *pszElem, int nAttrID)
{
   char chID = DoxIDChar(nAttrID);
   const char *pszAttrList = pszElem + 2;  // all elems have same format (so far)
   char ch;
   
   while ( (ch = *pszAttrList), !DoxIsAttrDelim(ch) ) {
      if (ch == chID) {
         return pszAttrList+1;
      }
      if (pszAttrList[1] != DOX_NV) {
         // skip SZ value
         do {
            ++pszAttrList;
         } while (*pszAttrList);
         ++pszAttrList;
      } else {
         pszAttrList += 2;
      }
   }
   return NULL;
}


// Return zero-terminated string value for attribute.
//
// If attribute is missing or without a value (DOX_NV), pszDefault is returned.
//
const char *xDoxGetAttrString(const char *pszElem, int nAttrID, const char *pszDefault)
{
   const char *psz = xDoxGetAttrValue(pszElem, nAttrID);

   // selective return
   if(NULL != pszDefault){
      if(NULL == psz || *psz == DOX_NV){
         psz = pszDefault;
      }
   }   
   return psz;
}


// Advance past the current "item" to the next "item" in the buffer.
// This performs an in-order traversal of the document hierarchy.
//
// There are three types of items, and they can be indentified by the initial
// character (list in parens below):
//
//   1. Start Item (DOX_SE) : begins element; item includes element id,
//      attribute list and perhaps DOX_CD.
//   2. End Item (DOX_EE) : ends element; includes single character.
//   3. Data Item (other) : data (i.e. regular text)
//
// The content of a Data item ends at the start of the next element.  Two data
// items cannot occur consecutively; a data item must be followed by a Start
// Element or End Element item.  As a result, the Data item content ends at
// either a DOX_SE or DOX_EE character (or at pchMax), and a zero-length Data
// Item is not possible.
//
// On entry:
//   pchItem = pointer to start of an item
//   pchMax = buffer pointer + size of buffer
//
// On exit:
//   result = pointer to next item, or pchMax if the end of the buffer
//            has been reached.
//
const char *xDoxNextItem(const char *pchItem, const char *pchMax)
{
   char ch = *pchItem;

   if (ch == DOX_EE) {
      ++pchItem;
   } else {
   
      // look for next DOX_CD, DOX_SE, or DOX_EE
      do {
         ch = *++pchItem;
      } while (!DoxIsDelim(ch));
   
      if (ch == DOX_CD)
         ++pchItem;
   }

   if (pchItem > pchMax)
      pchItem = pchMax;

   return pchItem;
}


// Return next item at same level (Data Item or Start Item), or DOX_EE if
// there are no more siblings.
//
// Return value:
//    pchMax => no more siblings
//    pointer to End Item => no more siblings
//    pointer to Start or Data item => next sibling
//
static const char *DoxNextSibling(const char *pch, const char *pchMax)
{
   int lvl = 0;

   while (pch < pchMax) {
      if (*pch == DOX_SE) {
         ++lvl;
      } else if (*pch == DOX_EE) {
         --lvl;
         if (lvl < 0)
            return pch;
      }
      pch = xDoxNextItem(pch, pchMax);

      if (lvl == 0)
         break;
   }

   return pch;
}


// Types of items and (delim characters).
//
//   Item    Follows  Contains   Precedes
//   -----   -------  --------  --------
//   Start   ...      SE ...    CD|EE
//   Data    CD|EE    ....      EE|SE
//   End     ...      EE        ...


// Find the closest item start preceding pch.
//
// If the start of the document is passed in, then a pointer to
// the character preceding it will be returned.
// 
const char *xDoxPrevItem(const char *pch)
{
   char ch = *--pch;

   // Check immediately-prior character
   //   EE: End item consists of a single EE character.
   //   SE: SE could only happen at the start of the buffer (the SE that precedes
   //       all content) or when we were passed an invalid item pointer, and
   //       instead one that points into the middle of the SE item; return the
   //       SE in that case.

   if (!DoxIsElemDelim(ch)) {

      // Skip to start of data item or CD-terminated SE item
      do {
         ch = *--pch;
      } while (!DoxIsDelim(ch));

      // EE or CD : data item starts at next character
      if (DoxIsAttrDelim(ch))
         ++pch;
   }

   return pch;
}



#define DoxBufParent(me)    ((me)->pcBuf - 1)    // The "parent" of the entire buffer



// Return pointer to parent element, or (pchBuf-1) if not found.
//
// For any properly-constructed buffer, there should be a valid parent element
// unless it is a top-level element, in which case (pchBuf-1) is returned, which
// points to the DOX_SE that precedes the buffer content.
//
// pchBuf = pointer to start of buffer
//
static const char *DoxParent(const char *pch, const char *pchBuf)
{
   int lvl = 0;

   // ASSERT(pchBuf[-1] == DOX_SE)

   while (pch >= pchBuf) {
      char ch;

      pch = xDoxPrevItem(pch);

      ch = *pch;

      if (ch == DOX_SE) {
         // Backing *out* of an element
         if (--lvl < 0)
            break;
      } else if (ch == DOX_EE) {
         // Backing *into* an element
         ++lvl;
      }
   }

   return pch;
}



static int Dox_NextSibling(Dox *me, int pos)
{
   char *pc = me->pcBuf;
   return DoxNextSibling(pc + pos, pc + me->cbData) - pc;
}


static int Dox_NextItem(Dox *me, int pos)
{
   char *pc = me->pcBuf;
   return xDoxNextItem(pc + pos, pc + me->cbData) - pc;
}


// Find the nearest ancestor of a given element ID
//
// Returns index of element, or -1 if not found.
//
static int Dox_FindAncestor(Dox *me, int pos, int nID)
{
   char chID = DoxIDChar(nID);
   const char *pcBuf = me->pcBuf;
   const char *pc = pcBuf + pos;    // okay when posFocus==-1  (see below)

   while (pc >= pcBuf && (pc[0] != DOX_SE || pc[1] != chID))
      pc = DoxParent(pc, pcBuf);

   return pc - pcBuf;
}



//
//--- DoxIter : Iterates elements and descends into child elements 
//                within one level of the document
// On entry:
//    pos = initial element, or -1 to iterate through the whole document
static void DoxIter_Ctor(DoxIter *me, Dox *pdox, int pos)
{
   DoxItem_Ctor(&me->doxitem, pdox, pos);

   if (pos == -1) {
      me->doxitem.cbUsed = 1;
   } else {
      DoxItem_Next(&me->doxitem);
   }

   me->nLevel = 0;

//// test only
//   me->nn = 0;
}

// Iterate thru descendants of the starting element
//
// On exit:
//    me->doxitem = DoxItem describing an SE item
static boolean DoxIter_Next(DoxIter *me)
{
   while (DoxItem_Next(&me->doxitem)) {
      char ch = me->doxitem.pc[0];
      
      if (ch == DOX_SE) {
         unsigned nElem = DoxGetID(me->doxitem.pc[1]);

         ++me->nLevel;
         me->chElem = (char)nElem;

//         ++me->nn;
         return 1;

      } else if (ch == DOX_EE) {
         if (me->nLevel) {
            --me->nLevel;
         } else {
            me->doxitem.cbUsed = 0;
            break;
         }
      }
   }
   return 0;
}

// Find the next 'chElem' element within starting element or document
static boolean DoxIter_Find(DoxIter *me, char chElem)
{
   while (DoxIter_Next(me)) {
//      DBGPRINTF("%d: elem=%s level=%d", me->nn,
//                Dox_GetElemName(me->doxitem.pdox, (int)me->chElem),
//                me->nLevel);
      if (me->chElem == chElem)
         return 1;
   }
   return 0;
}


static int32 DoxCopyString(char *pcBuf, int32 cbBuf, const char *pc, int32 cb)
{
   if (cb < 0)
      cb = STRLEN(pc);

   if (cb >= cbBuf)
      cb = cbBuf-1;

   if (cb >= 0) {
      MEMMOVE(pcBuf, pc, cb);
      pcBuf[cb] = '\0';
   } else {
      cb = 0;
   }

   return cb;
}


int32 Dox_FindElem(Dox *me, const char *pszElem, int32 nIndex)
{
   int32 posMax = xDox_BufGetSize(me);
   const char *pchBuf = me->pcBuf;
   int32 pos = 0;
   int nID = Dox_GetElemID(me, pszElem);
   char chID =  DoxIDChar(nID);

   if (nID < 0) {
      return HVM_BADELEMENT;
   }
   
   while (pos < posMax) {
      if (pchBuf[pos] == DOX_SE
          && pchBuf[pos+1] == chID) {
         // matching element
         if (!nIndex)
            return pos;
         --nIndex;
      }
      pos = Dox_NextItem(me, pos);
   }

   return HVM_NOTFOUND;
}


int32 Dox_GetElemText(Dox *me, int32 nElem, char *pchBuf, int32 cbBuf)
{
   const char *pchDox = me->pcBuf;
   int32 pos, posNext = nElem;
   int lvl = 0;
   int cbOut;
   char ch;

   if (pchBuf) {
      if (cbBuf) {
      pchBuf[0] = '\0';
      }
      cbOut = 0;
   } else {
      cbOut = 1;
   }
   
   if (Dox_BufGetChar(me, nElem) != DOX_SE) {
      return HVM_BADELEMENT;
   }

   for (;;) {
      pos = posNext;
      posNext = Dox_NextItem(me, pos);
      ch = pchDox[pos];

      if (ch == DOX_SE) {
         // Start Element
         ++lvl;
      } else if (ch == DOX_EE) {
         // End Element
         --lvl;
         if (lvl <= 0)
            return cbOut;
      } else {
         // Text
         if (pchBuf) {
            cbOut += DoxCopyString(pchBuf + cbOut, cbBuf - cbOut, pchDox + pos, posNext - pos);
         } else {
            cbOut += (posNext - pos);
         }
      }
   }
}



//static boolean DoxIter_NextFormItem(DoxIter *me, IFormControl **ppo, const char **ppszType)
//{
//   boolean ret = DoxIter_Next(me);
//
//   IFormControl* pifc = NULL;
//   const char * pszType = NULL;
//
//   unsigned nElem = me->chElem;
//   int fDisp = acmHtmlElems[nElem].fDisp;
//
//   if (fDisp & ELEMDISP_OBJECT) {
//      pifc = xDox_ObtainObject(me->doxitem.pdox, me->doxitem.pos);
//      if (nElem == ELEM_INPUT) {
//         pszType = xDoxGetAttrString(me->doxitem.pc, ATTR_TYPE, "text");
//      } else if (nElem == ELEM_TEXTAREA) {
//         pszType = "textarea";
//      } else {
//         pszType = "";
//      }
//   } else {
//      pifc = 0;
//   }
//
//   if (ppo)
//      *ppo = pifc;
//   if (ppszType)
//      *ppszType = pszType;
//
//   return ret;
//}

static boolean DoxIter_GetNextFormItem(DoxIter *me, IFormControl **ppo, const char **ppszType)
{
   boolean ret = DoxIter_Next(me);

   IFormControl* pifc = NULL;
   const char * pszType = NULL;

   unsigned nElem = me->chElem;
   int fDisp = acmHtmlElems[nElem].fDisp;

   if (fDisp & ELEMDISP_OBJECT) {
      pifc = xDox_ObtainObject(me->doxitem.pdox, me->doxitem.pos);
      if (nElem == ELEM_INPUT) {
         pszType = xDoxGetAttrString(me->doxitem.pc, ATTR_TYPE, "text");
      } else if (nElem == ELEM_TEXTAREA) {
         pszType = "textarea";
      } else {
         pszType = "";
      }
   } else {
      pifc = 0;
   }

   if (ppo)
      *ppo = pifc;
   if (ppszType)
      *ppszType = pszType;

   return ret;
}

int xDox_MakeUrl(Dox *me, int pos, const char *cpszRel, char *pResult, int cbResultLen)
{
   const char *cpc, *cpszBase;
   
   if (!me->piWebUtil) {
      return 0;
   }
   
   // set default base to document URL
   cpszBase = me->pszUrl;
   
   // search backwards in document from cpszRel for previous BASE elem
   cpc = me->pcBuf + pos;
   while ((cpc = xDoxPrevItem(cpc)) >= me->pcBuf) {
      if (DOX_SE == cpc[0] && DoxIDChar(ELEM_BASE) == cpc[1]) {
         const char *cpsz;
         
         cpsz = xDoxGetAttrString(cpc, ATTR_HREF, 0);
         
         if (cpsz) { // found base href
            UrlParts up;
            
            IWEBUTIL_ParseUrl(me->piWebUtil,cpsz,&up);
            
            // only take it if it's absolute, has a scheme
            if (UP_HASSCHM(&up)) { 
               cpszBase = cpsz;
               break;
            }
         }
      }
   }

   return IWEBUTIL_MakeUrl(me->piWebUtil, cpszBase, cpszRel, pResult, cbResultLen);
}

/*
#if 0
static void Dox_ResetForm(Dox *me, int posSubmit)
{
   DoxIter  di;
   IFormControl* pifc;

   int posForm = Dox_FindAncestor(me, posSubmit, ELEM_FORM);
   
   if (posForm < 0)
      return;

   DoxIter_Ctor(&di, me, posForm);

   while (DoxIter_NextFormItem(&di, &pifc, NULL)) {
      if (pifc) {
         IFORMCONTROL_Reset(pifc);
      }
   }
}
#endif
*/


#define CSTRLEN(s)   (sizeof(s) - 1)

// @@@@
//static void Dox_SubmitForm(Dox *me, int posSubmit)
//{
//   char *         pszFields;
//   DStr           dsFields;
//   DoxIter        di;
//   IFormControl*  pifc;
//   const char *   pszType;
//
//   int  posForm = Dox_FindAncestor(me, posSubmit, ELEM_FORM);
//
//   if (posForm < 0) {
//      return;
//   }
//
//   DStr_Ctor(&dsFields, &me->dstrClass);
//   DStr_Append(&dsFields, "&", 1);
//
//   DoxIter_Ctor(&di, me, posForm);
//   while (DoxIter_NextFormItem(&di, &pifc, &pszType)) {
//
//      if (pifc) {
//
//         // skip submit buttons other than the one pressed
//         if (DoxStrEQI(pszType, "submit") && (posSubmit != di.doxitem.pos)) {
//            continue;
//         }
//         // get data from the control in the form "name=value"
//         if (IFORMCONTROL_GetData(pifc, &dsFields)) {
//            DStr_Append(&dsFields, "&", 1);
//         }
//
//      // special case - handle hidden fields
//      } else if (DoxStrEQI(pszType, "hidden")) {
//         const char *pc = Dox_BufGetPtr(me, di.doxitem.pos);
//         const char *pszName = xDoxGetAttrString(pc, ATTR_NAME, NULL);
//         const char *pszValue = xDoxGetAttrString(pc, ATTR_VALUE, NULL);
//         if (pszName && pszValue)
//            DStr_Printf(&dsFields, "%q=%q&", pszName, pszValue);
//      } else if (di.chElem == ELEM_A) {
//         const char *pc = Dox_BufGetPtr(me, di.doxitem.pos);
//         const char *pszHRef = xDoxGetAttrString(pc, ATTR_HREF, NULL);
//         if (pszHRef && STRIBEGINS("x-submit:", pszHRef)) {
//            const char *pszName = pszHRef + CSTRLEN("x-submit:");
//            const char *pszValue = STRCHR(pszName, '=');
//            if (pszValue) {
//               // zero terminate the name=value string
//               DStr ds;
//               DStr_Ctor(&ds, &me->dstrClass);
//               DStr_Append(&ds, pszName, pszValue-pszName);
//               DStr_Printf(&dsFields, "%q=", DSTR_STR(&ds));
//               DStr_Replace(&ds, pszValue+1, -1);
//               DStr_Printf(&dsFields, "%q&", DSTR_STR(&ds));
//               DStr_Dtor(&ds);
//            }
//         }
//      }
//   }
//
//   // remove the trailing '&' unless it's 
//   // the only thing in the DStr
//   if (DSTR_LEN(&dsFields) > 1) {
//      char *pe = DSTR_END(&dsFields) - 1;
//      if (*pe == '&') {
//         DStr_Trim(&dsFields, 1);
//      }
//   }
//
//   if (NULL != (pszFields = DSTR_STR(&dsFields))) {
//      DStr        dsAction;
//      const char *pszAction;
//      const char *pszMethod;
//
//      DStr_Ctor(&dsAction, &me->dstrClass);
//
//  //    DBGPRINTF("Submit string: %s", pszFields + 1);
//
//      pszAction = xDoxGetAttrString(me->pcBuf+posForm, ATTR_ACTION, "");
//      
//      xDox_MakeUrl(me,posForm,pszAction,&dsAction);
//      pszAction = DSTR_SAFESTR(&dsAction);
// 
//      pszMethod = xDoxGetAttrString(me->pcBuf+posForm, ATTR_METHOD, "GET");
//
//      if (DoxStrEQI(pszMethod,"get")) {
//
//         // change leading '&' to '?'
//         *pszFields = '?';
//
//         // if the action has a '?', MakeUrl() will resolve with pszFields
//         DStr_MakeUrl(&dsAction, pszAction, pszFields); 
//         pszAction = DSTR_SAFESTR(&dsAction);
//
//         pszFields = 0; // no data for POST
//      } else {
//         pszFields++; // index past first '&' for POST
//      }
//
//      // notify the application
//      IHTMLVIEWMODEL_NotifySubmit(me->piModel, posForm, pszAction, pszMethod, pszFields);
//
//  //    DBGPRINTF("Action string: %s", pszAction);
//
//      // free the DStr memory
//      DStr_Dtor(&dsAction);
//   }
//
//   // free the DStr memory
//   DStr_Dtor(&dsFields);
//}


//void Dox_CheckRadios(Dox *me, int posRadio)
//{
//   int posForm = Dox_FindAncestor(me, posRadio, ELEM_FORM);
//   const char *pszRadioName = xDoxGetAttrString(me->pcBuf+posRadio, ATTR_NAME, 0);
//   DoxIter di;
//   ICtlObj *pco;
//   const char * pszType;
//
//   if (!pszRadioName)
//      return;
//
//   DoxIter_Ctor(&di, me, posForm);
//
//   while (DoxIter_NextFormItem(&di, &pco, &pszType)) {
//      if (pco
//          && DoxStrEQI(pszType, "radio")
//          && DoxStrEQI(pszRadioName, xDoxGetAttrString(di.doxitem.pc, ATTR_NAME, ""))) {
//
//         // Same group
//         ICTLOBJ_SetChecked(pco, (boolean)(posRadio == di.doxitem.pos));
//         ICTLOBJ_Redraw(pco);
//      }
//   }
//}



//void Dox_NotifyCB(void *p, CtlNotify * pcn)
//{ 
//   Dox *me = (Dox *)p;
//   int pos;
//  
//   switch (pcn->code) {
//      
//      case CN_OBJECTLOADED: {
//         
//         if (me->nFirstPage) {
//            uint32 dwVal = ICTLOBJ_GetParam(pcn->pco);
//            me->nFirstPage = MAX(0, me->nFirstPage - (int32)dwVal);
//            
//            // notify on transition to zero
//            if (!me->nFirstPage) {
//               // notify the application 
//               IHTMLVIEWMODEL_NotifyEvent(me->piModel, EVT_HVM_PAGEDONE);
//            }
//         }
//
//         if (0 == --me->nRefImages) {
//            // notify the application 
//            IHTMLVIEWMODEL_NotifyEvent(me->piModel, EVT_HVM_CONTENTDONE);
//         }
//      } break;
//
//      case CN_REWRAP:
//         Dox_Invalidate(me, DOX_INVALWRAP);
//         break;
//
//      case CN_REDRAWSCREEN:
//         Dox_Invalidate(me, 0);
//         break;
//
//      case CN_FULLSCREENEDIT: {
//  //       HViewNotify hvn;
//  //       hvn.code = HVN_FULLSCREEN_EDIT;
//  //       IHtmlViewer_Notify(me->piView, &hvn);
//         me->pcoFull = pcn->pco; // save off the control that has gone full-screen
//      } break;
//
//      case CN_FULLSCREENEDITDONE: 
//         me->pcoFull = NULL;     // full-screen mode is over
//         break;
//      
//      case CN_INVALIDATE:
//         Dox_Invalidate(me, 0);
//         break;
//
//      case CN_RADIOSELECT:
//         if (0 < (pos = Dox_GetObjectPos(me, pcn->pco))) {
//            Dox_CheckRadios(me, pos);
//            Dox_Invalidate(me, 0);
//         }
//         break;
//
//      case CN_SUBMIT:
//         if (0 < (pos = Dox_GetObjectPos(me, pcn->pco))) {
//            Dox_SubmitForm(me, pos);
//         }
//         break;
//
//      case CN_RESET:
//         if (0 < (pos = Dox_GetObjectPos(me, pcn->pco))) {
//            Dox_ResetForm(me, pos);
//            // force a redraw
//            Dox_Invalidate(me, 0);
//         }
//         break;
//   }
//}

// OLD STUFF
//
//int Dox_GetObjectPos(Dox *me, ICtlObj * pco)
//{
//   int nn = 0;
//   DoxObjectTbl * pdot = me->otbl;
//
//   for (; nn < ARRAYSIZE(me->otbl) && pdot[nn].nID != 0; nn++) {
//      if (pdot[nn].pco == pco)
//         return pdot[nn].nID-1;
//   }
//
//   return -1;
//}
//
//            
//
////////////////////////////////////////////////////////////////////////////////
// NEW STUFF (+)

// form submission
static void Dox_ResetForm(Dox *me, int posSubmit)
{
   DoxIter        di;
   IFormControl * pifc;
   IValueModel *  pivm;

   int            posForm = 0;
   const char*    pszType = NULL;
   char           paszData[] = {0};

   // find the immediate FORM ancestor
   posForm = Dox_FindAncestor(me, posSubmit, ELEM_FORM);

   if (posForm < 0) {
      return;
   }

   DoxIter_Ctor(&di, me, posForm);
   while (DoxIter_GetNextFormItem(&di, &pifc, &pszType)) {
      // get specific types
      if (xDoxStrEQI(pszType, "text")) {
         IFORMCONTROL_SetData( pifc, paszData );
      } else if (xDoxStrEQI(pszType, "radio") || xDoxStrEQI(pszType, "checkbox")) {
         if (SUCCESS == IFORMCONTROL_GetModel( pifc, AEEIID_VALUEMODEL, (IModel**)&pivm ) ) {
            IVALUEMODEL_SetBool( pivm, FALSE );
            RELEASEIF(pivm);
         }
      }
   }
}
// form submission
static void Dox_SubmitForm(Dox *me, int posSubmit)
{
   DoxIter        di;
   IFormControl * pifc;
   IValueModel *  pivm;

   boolean        fMemOut = FALSE;

   int            posForm;
   char *         pcDelim;
   const char *   pszType;
   char *         paszTempbuf = NULL;
   char           aszName[100];
   char *         pszQS;                  // query string buffer
   int            nQS = 100;
   int            nQSUsed = 0;
   int            nQSXtra = 20;
   char *         pszValueBuf;            // form control value buffer
   int            nValueBuf = 50;
   int            nValueBufUsed = 0;
   int            nValueBufXtra = 20;
   int            nReqValueBuf = 0;
   char *         pszEncValueBuf;         // encoded value buffer
   int            nInLenEnc = 0;
   int            nOutLenEnc = 0;
   int            nEncValueBuf = 50;
   int            nEncValueBufUsed = 0;
   int            nEncValueBufXtra = 20;

   // buffer allocation for query string
   pszQS = (char *)MALLOC( nQS );
   if (NULL == pszQS) {
		fMemOut = TRUE;
   }

   // buffer allocation for query string
   pszValueBuf = (char *)MALLOC( nValueBuf );
   if (NULL == pszValueBuf) {
		fMemOut = TRUE;
   }

   // buffer allocation for encoded string
   pszEncValueBuf = (char *)MALLOC( nEncValueBuf );
   if (NULL == pszEncValueBuf) {
		fMemOut = TRUE;
   }

   // find the immediate FORM ancestor
   posForm = Dox_FindAncestor(me, posSubmit, ELEM_FORM);

   if (posForm < 0) {
      return;
   }

   DoxIter_Ctor(&di, me, posForm);
   while (FALSE == fMemOut && DoxIter_GetNextFormItem(&di, &pifc, &pszType)) {
      if (pifc) {
         // skip submit buttons other than the one pressed
         if (xDoxStrEQI(pszType, "submit") && (posSubmit != di.doxitem.pos)) {
            continue;
         }

         // get specific types
         if ( xDoxStrEQI(pszType, "text") || xDoxStrEQI(pszType, "submit") || xDoxStrEQI(pszType, "radio") || xDoxStrEQI(pszType, "checkbox")) {

            // if radio or checkbox, test if this is selected by the user
            if (xDoxStrEQI(pszType, "radio") || xDoxStrEQI(pszType, "checkbox")) {
               if (SUCCESS == IFORMCONTROL_GetModel( pifc, AEEIID_VALUEMODEL, (IModel**)&pivm ) ) {
                  if (FALSE == IVALUEMODEL_GetBool(pivm)) {
                     continue;
                  }
                  RELEASEIF(pivm);
               } else {
                  continue;
               }
            }

            // get name
            MEMSET( (void *) aszName, 0, sizeof(aszName) );
			   Dox_GetAttrString( di.doxitem.pdox, di.doxitem.pos, "name", aszName, sizeof(aszName) );
            if(NULL == *aszName){
               continue;
            }
			   paszTempbuf = aszName;
			   while (0 != *paszTempbuf) {
				   if ( SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
                  *(pszQS + nQSUsed) = *paszTempbuf++;
					   nQSUsed++;
				   } else {
				      fMemOut = TRUE;
				      break;
				   }
			   }

            // =
			   if (SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
               *(pszQS + nQSUsed) = '=';
				   nQSUsed++;
			   } else {
			      fMemOut = TRUE;
			      break;
			   }

            // get value
				nValueBufUsed = 0;
            nReqValueBuf = 0;
            if (SUCCESS == IFORMCONTROL_GetData( pifc, NULL, &nReqValueBuf) ) {
               if (SUCCESS == MemGrowEx( (void **)&pszValueBuf, &nValueBuf, nValueBufUsed, nReqValueBuf, nValueBufXtra) ) {
                  if (SUCCESS == IFORMCONTROL_GetData( pifc, pszValueBuf, &nValueBuf) ) {
                     // counting..
                     nInLenEnc = STRLEN(pszValueBuf);
                     nOutLenEnc = 2*nInLenEnc;
                     MEMSET ((void *)pszEncValueBuf, 0, nEncValueBuf);
                     IWEBUTIL_UrlEncode(me->piWebUtil, pszValueBuf, &nInLenEnc, NULL, &nOutLenEnc);
                     nEncValueBufUsed = 0;
                     if (SUCCESS == MemGrowEx( (void **)&pszEncValueBuf, &nEncValueBuf, nEncValueBufUsed, nOutLenEnc, nEncValueBufXtra) ) {
                        // encode value
                        IWEBUTIL_UrlEncode(me->piWebUtil, pszValueBuf, &nInLenEnc, pszEncValueBuf, &nOutLenEnc);
                        paszTempbuf = pszEncValueBuf;
                        while (0 != *paszTempbuf) {
				               if ( SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
                              *(pszQS + nQSUsed) = *paszTempbuf++;
					               nQSUsed++;
				               } else {
				                  fMemOut = TRUE;
				                  break;
				               }
                        }
                     } else {
                        fMemOut = TRUE;
                        break;
                     }
                  } else {
                     fMemOut = TRUE;
                     break;
                  }
               } else {
                  fMemOut = TRUE;
                  break;                  
               }
            }
         }      
      }else if (xDoxStrEQI(pszType, "hidden")) {

         // get name
         MEMSET( (void *) aszName, 0, sizeof(aszName) );
			Dox_GetAttrString( di.doxitem.pdox, di.doxitem.pos, "name", aszName, sizeof(aszName) );
         if(NULL == *aszName){
            continue;
         }
			paszTempbuf = aszName;
			while (0 != *paszTempbuf) {
				if ( SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
               *(pszQS + nQSUsed) = *paszTempbuf++;
					nQSUsed++;
				} else {
				   fMemOut = TRUE;
				   break;
				}
			}

         // =
			if (SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
            *(pszQS + nQSUsed) = '=';
				nQSUsed++;
			} else {
			   fMemOut = TRUE;
			   break;
			}

         // get value
	      nValueBufUsed = 0;
         nReqValueBuf = 256;
         if (SUCCESS == MemGrowEx( (void **)&pszValueBuf, &nValueBuf, nValueBufUsed, nReqValueBuf, nValueBufXtra) ) {            
            MEMSET ((void *)pszValueBuf, 0, (unsigned int)nValueBuf);
			   Dox_GetAttrString( di.doxitem.pdox, di.doxitem.pos, "value", pszValueBuf, nValueBuf );
            // counting..
            IWEBUTIL_UrlEncode(me->piWebUtil, pszValueBuf, &nInLenEnc, NULL, &nOutLenEnc);
            nEncValueBufUsed = 0;
            if (SUCCESS == MemGrowEx( (void **)&pszEncValueBuf, &nEncValueBuf, nEncValueBufUsed, nOutLenEnc, nEncValueBufXtra) ) {
               // encode value
               nInLenEnc = STRLEN(pszValueBuf);
               nOutLenEnc = 2*nInLenEnc;
               MEMSET ((void *)pszEncValueBuf, 0, nEncValueBuf);
               IWEBUTIL_UrlEncode(me->piWebUtil, pszValueBuf, &nInLenEnc, pszEncValueBuf, &nOutLenEnc);
               paszTempbuf = pszEncValueBuf;
               while (0 != *paszTempbuf) {
				      if ( SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
                     *(pszQS + nQSUsed) = *paszTempbuf++;
					      nQSUsed++;
				      } else {
				         fMemOut = TRUE;
				         break;
				      }
               }
            } else {
               fMemOut = TRUE;
               break;
            }
         } else {
            fMemOut = TRUE;
            break;
         }
      // special case handling of <A> as a submit button
      } else if (di.chElem == ELEM_A) {
         nValueBufUsed = 0;
         nReqValueBuf = 256;
         if (SUCCESS == MemGrowEx( (void **)&pszValueBuf, &nValueBuf, nValueBufUsed, nReqValueBuf, nValueBufXtra) ) {
            MEMSET ((void *)pszValueBuf, 0, (unsigned int)nValueBuf);
			   Dox_GetAttrString( di.doxitem.pdox, di.doxitem.pos, "href", pszValueBuf, nValueBuf );
            if (NULL != *pszValueBuf && 0==STRNCMP("x-submit:", pszValueBuf, STRLEN("x-submit:")) ) {
               char *pszName = pszValueBuf + CSTRLEN("x-submit:");
               char *pszValue = STRCHR(pszName, '=');
               if (pszValue) {
                  int i = (pszValue - pszName);
                  paszTempbuf = pszName;
                  while (i-- > 0) {
				         if ( SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
                        *(pszQS + nQSUsed) = *paszTempbuf++;
					         nQSUsed++;
				         } else {
				            fMemOut = TRUE;
				            break;
				         }
                  }
                  // =
			         if (SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
                     *(pszQS + nQSUsed) = '=';
				         nQSUsed++;
			         } else {
			            fMemOut = TRUE;
			            break;
			         }
                  // get value
                  paszTempbuf = pszValue + 1;
                  while (0 != *paszTempbuf) {
				         if ( SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
                        *(pszQS + nQSUsed) = *paszTempbuf++;
					         nQSUsed++;
				         } else {
				            fMemOut = TRUE;
				            break;
				         }
                  }
               }
            }
         }
      }
      // &
      if ( nQSUsed > 0 && *(pszQS + nQSUsed - 1) != '&' ) {
		   if (SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
            *(pszQS + nQSUsed) = '&';
			   nQSUsed++;
		   } else {
			   fMemOut = TRUE;
			   break;
		   }
      }
   }

   // remove the trailing '&' unless it's 
   // not the only character in query string
   if (nQSUsed > 1) {
      if ('&' == *(pszQS + nQSUsed - 1)) {
         *(pszQS + nQSUsed -1) = 0;
         nQSUsed--;
      }
   }

   // null terminate
	if (SUCCESS == MemGrowEx( (void **)&pszQS, &nQS, nQSUsed, 1, nQSXtra) ) {
      *(pszQS + nQSUsed) = 0;
		nQSUsed++;
	} else {
		fMemOut = TRUE;
	}   

   // get "action" and "method" for this form
   if (NULL != pszQS && FALSE == fMemOut) {

      char *pszAction = NULL;
      int nAction = 0;
      int nActionUsed = 0;
      char *pszMethod = NULL;

      nValueBufUsed = 0;
      nReqValueBuf = 256;
      if (SUCCESS == MemGrowEx( (void **)&pszValueBuf, &nValueBuf, nValueBufUsed, nReqValueBuf, nValueBufXtra) ) {
         MEMSET ((void *)pszValueBuf, 0, (unsigned int)nValueBuf);
         Dox_GetAttrString( me, posForm, "action", pszValueBuf, nValueBuf );
      } else {
         fMemOut = TRUE;
      }
      
      nAction = xDox_MakeUrl(me, posForm, pszValueBuf, NULL, 0);
      if (nAction > 0) {
         pszAction = (char*) MALLOC ( nAction );
         if (NULL != pszAction) {
            xDox_MakeUrl(me, posForm, pszValueBuf, pszAction, nAction);
            nActionUsed = nAction - 1;
         }
      }
      
      // remove trailing '?' in action url
      pcDelim = STRCHR(pszAction, '?');
      if ( NULL != pcDelim &&  ( *(pcDelim+1) == ' ' || *(pcDelim+1) == 0 ) ) {
         *pcDelim = 0;
         nActionUsed = STRLEN(pszAction);
      }

      nValueBufUsed = 0;
      nReqValueBuf = 256;
      if (SUCCESS == MemGrowEx( (void **)&pszValueBuf, &nValueBuf, nValueBufUsed, nReqValueBuf, nValueBufXtra) ) {
         MEMSET ((void *)pszValueBuf, 0, (unsigned int)nValueBuf);
         Dox_GetAttrString( me, posForm, "method", pszValueBuf, nValueBuf );
         if (STRLEN(pszValueBuf) > 0) {
            pszMethod = (char *) MALLOC (STRLEN(pszValueBuf) + 1);
         } else {
            pszMethod = (char *) MALLOC (STRLEN("get") + 1);
            STRLCPY(pszMethod, "get", STRLEN("get"));
         }
         STRLCPY(pszMethod, pszValueBuf, (STRLEN(pszValueBuf)+1));
      } else {
         fMemOut = TRUE;
      }

      if (xDoxStrEQI(pszMethod,"get")) {
         // replace the null terminator with ?
         *(pszAction + nActionUsed - 1) = '?';
         // append the query string to the action url
         paszTempbuf = pszQS;
         while (0 != *paszTempbuf) {
			   if ( SUCCESS == MemGrowEx( (void **)&pszAction, &nAction, nActionUsed, 20, 20) ) {
               *(pszAction + nActionUsed) = *paszTempbuf++;
				   nActionUsed++;
			   } else {
				   fMemOut = TRUE;
				   break;
			   }
         }
         // free query string. we dont need it now
         FREE(pszQS);
         pszQS = NULL;
      }

      // notify the application
      IHTMLVIEWMODEL_NotifySubmit(me->piModel, posForm, pszAction, pszMethod, pszQS);

      FREE(pszAction);
      pszAction = NULL;

      FREE(pszMethod);
      pszMethod = NULL;
   }

   // -- clean up temp buffers
   FREE(pszValueBuf);
   pszValueBuf = NULL;

   FREE(pszEncValueBuf);
   pszEncValueBuf = NULL;

   FREE(pszQS);
   pszQS = NULL;

}

void Dox_CheckRadios(Dox *me, int posRadio) {
   DoxIter  di;
   IFormControl* pifc;
   IValueModel *pivm = 0;
   
   const char *pszName = NULL;
   //const char *pszValue = NULL;
   const char *pszType = NULL;
   int posForm = Dox_FindAncestor(me, posRadio, ELEM_FORM);

   // current radio name/value pair
   pszName = xDoxGetAttrString( me->pcBuf+posRadio, ATTR_NAME, 0 );
   //pszValue = xDoxGetAttrString( me->pcBuf+posRadio, ATTR_VALUE, 0 );

   if (NULL == pszName) {
      return;
   }

   // start from the closest form tag
   DoxIter_Ctor(&di, me, posForm);
   while ( DoxIter_GetNextFormItem( &di, &pifc, &pszType ) ) {
      if ( pifc && xDoxStrEQI(pszType, "radio") && xDoxStrEQI(pszName, xDoxGetAttrString(di.doxitem.pc, ATTR_NAME, ""))) {         
         if (SUCCESS == IFORMCONTROL_GetModel( pifc, AEEIID_VALUEMODEL, (IModel**)&pivm ) ) {
            IVALUEMODEL_SetBool( pivm, (boolean)(posRadio == di.doxitem.pos) );
            RELEASEIF(pivm);
         }
      }
   }
}

/*
   Form Element State helper functions   
*/
typedef struct _formelementstate {
   int      ncParam;
   int      nCurrent;
   int      nOffset;
   int      nBufxtra;
   int      nBufused[FS_ELEMSTATECOUNT];
   int      nBufsize[FS_ELEMSTATECOUNT];
   char     *pszState[FS_ELEMSTATECOUNT];

} formelementstate;

static int Dox_ScrollInfoFormElemState (const char* pcFESS) {
   char *pcBufScroll = NULL;
   int nBufScroll = 1;
   int nBufScrollUsed = 0;
   int nBufScrollXtra = 1;
   int nScrollPos = 0;

   pcBufScroll = (char *)MALLOC( nBufScroll );
   if (NULL == pcBufScroll) {
		return nScrollPos;
   }

   while ( *pcFESS != NULL && *pcFESS != FS_SOH && *pcFESS != FS_STX && *pcFESS != FS_ETX ) {
      if (SUCCESS == MemGrowEx( (void **)&pcBufScroll, &nBufScroll, nBufScrollUsed, 1, nBufScrollXtra) ) {
         *(pcBufScroll + nBufScrollUsed) = *pcFESS;
         nBufScrollUsed++;
         pcFESS++;
      } else {
         FREE(pcBufScroll);
         return nScrollPos;
      }
   }

   if (SUCCESS == MemGrowEx( (void **)&pcBufScroll, &nBufScroll, nBufScrollUsed, 1, nBufScrollXtra) ) {
      *(pcBufScroll + nBufScrollUsed) = NULL;
      nBufScrollUsed++;
   }

   nScrollPos = ATOI ( pcBufScroll );
   FREE(pcBufScroll);
   return nScrollPos;
}

static boolean Dox_InitFormElemState (void *pFE) {

   int i = 0;
   formelementstate *pFES = NULL;
   pFES = (formelementstate *)pFE;

   pFES->ncParam = FS_ELEMSTATECOUNT;
   pFES->nCurrent = 0;
   pFES->nOffset = 0;
   pFES->nBufxtra = 10;

   for (i=0; i < FS_ELEMSTATECOUNT; i++) {
      pFES->nBufused[i] = 0;
      pFES->nBufsize[i] = 0;
      pFES->pszState[i] = (char *)MALLOC(pFES->nBufxtra);
      if (!pFES->pszState[i]) {
         return FALSE;
      }
   }
   return TRUE;
}

static void Dox_ReleaseFormElemState (void *pFE, boolean bFreeMem) {
   int i = 0;
   formelementstate *pFES = (formelementstate *)pFE;

   pFES->ncParam = FS_ELEMSTATECOUNT;
   pFES->nCurrent = 0;
   pFES->nOffset = 0;
   pFES->nBufxtra = 10;

   for (i=0; i < FS_ELEMSTATECOUNT; i++) {
      pFES->nBufused[i] = 0;
      pFES->nBufsize[i] = 0;
      if (TRUE == bFreeMem) {
         FREE(pFES->pszState[i]);
      }
   }
}

static boolean Dox_NextFormElemState(const char* pcFESS, void* pFE) {
   int i = 0;
   int nCount = 0;
   formelementstate *pFES = (formelementstate *)pFE;

   pcFESS += pFES->nOffset;
   do {
      if (*pcFESS == FS_SOH) {
         nCount++;
         if ( 1 == (nCount - pFES->nCurrent)) {
            pFES->nCurrent = nCount;
            break;
         }
      }
   } while (*pcFESS++ != NULL);

   if (*pcFESS != NULL) {
      pcFESS++;
   } else {
      return FALSE;
   }

   while (*pcFESS != FS_SOH && *pcFESS != NULL) {
      if (*pcFESS == FS_STX) {
         MEMSET((void *)pFES->pszState[i], 0, pFES->nBufsize[i]);
         pFES->nBufused[i] = 0;
      } else if (*pcFESS == FS_ETX) {
         // null terminate the string
         if (SUCCESS == MemGrowEx((void **)&pFES->pszState[i], &pFES->nBufsize[i], pFES->nBufused[i], 1, pFES->nBufxtra)) {
            *(pFES->pszState[i] + pFES->nBufused[i]) = 0;
            pFES->nBufused[i]++;
         }
         i++;
      } else {
         // build the string
         if (SUCCESS == MemGrowEx((void **)&pFES->pszState[i], &pFES->nBufsize[i], pFES->nBufused[i], 1, pFES->nBufxtra)) {
            *(pFES->pszState[i] + pFES->nBufused[i]) = *pcFESS;
            pFES->nBufused[i]++;
         }
      }         
      pcFESS++;
   }

   return TRUE;
}

void Dox_SetFormElemState(Dox *me, const char *pszState){
   DoxIter              di;
   IFormControl         *pifc;
   IValueModel          *pivm = 0;
   formelementstate     pFE;
   formelementstate     *pFES = 0;

   const char           *pszType;
   char                 aszName[200];
   char                 aszValue[200];

   pFES = &pFE;

   if (TRUE == Dox_InitFormElemState( (void *)pFES) ) {
      // pass -1 as we will iterate through all elems that can change state
      DoxIter_Ctor( &di, me, -1 );
      while (DoxIter_GetNextFormItem( &di, &pifc, &pszType) ) {
         if (pifc) {
	        // get only text, radio and checkbox
            if (0 == STRCMP( "text", pszType ) || 0 == STRCMP( "radio", pszType ) || 0 == STRCMP( "checkbox", pszType ) ) {
               MEMSET( (void *) aszName, 0, sizeof(aszName) );
               MEMSET( (void *) aszValue, 0, sizeof(aszValue) );
			      Dox_GetAttrString( di.doxitem.pdox, di.doxitem.pos, "name", aszName, sizeof(aszName) );
               if(NULL == *aszName){
                  continue;
               }
               if (0 == STRICMP( "radio", pszType ) || 0 == STRICMP( "checkbox", pszType ) ) {
                  Dox_GetAttrString( di.doxitem.pdox, di.doxitem.pos, "value", aszValue, sizeof(aszValue) );
               }
               // reset parameters
               Dox_ReleaseFormElemState ( (void *)pFES, FALSE );
               while ( TRUE == Dox_NextFormElemState( pszState, (void *)pFES) ) {
                  // find the element based on name and type
                  if (0 == STRICMP( aszName, pFES->pszState[FSINDX_NAME] ) && 0 == STRICMP( pszType, pFES->pszState[FSINDX_TYPE] ) ) {
                     if (0 == STRICMP( "text", pszType) ) {
                        IFORMCONTROL_SetData( pifc, (const char *)pFES->pszState[FSINDX_VALUE] );
                        break;
                     } else if (0 == STRICMP( "radio", pszType ) || 0 == STRICMP( "checkbox", pszType) ) {
                        // for radio and checkbox we need to check for value as well
                        if (0 == STRICMP( aszValue, pFES->pszState[FSINDX_VALUE]) ) {
                           if (SUCCESS == IFORMCONTROL_GetModel( pifc, AEEIID_VALUEMODEL, (IModel**)&pivm ) ) {
                              if ( *pFES->pszState[2] == FS_CHECKED ) {
                                 IVALUEMODEL_SetBool( pivm, TRUE );
                              } else if ( *pFES->pszState[FSINDX_STATE] == FS_UNCHECKED ) {
                                 IVALUEMODEL_SetBool( pivm, FALSE );
                              }
                              RELEASEIF(pivm);
                           }
                           break;
                        }
                     }
                  }
               }
            }
         }
      }
   }
   // release resources
   Dox_ReleaseFormElemState ( (void *)pFES, TRUE );

   // set scroll position
   Dox_SetScrollPos(me, Dox_ScrollInfoFormElemState(pszState));
}

/* 
   Get the form element state
*/
int Dox_GetFormElemState(Dox *me, char *pszBuf, int cbBuf){

   DoxIter  di;
   IFormControl* pifc;
   IValueModel *pivm = 0;

   // temp dynamic buffer
   char *pcDynbuf = NULL;			// final string
   int nDynbuf = 100;
   int nDynbufUsed = 0;
   int nDynbufMore = 20;
   int nDynbufXtra = 100;
   char *pcTDynbuf = NULL;
   int nTDynbuf = 50;
   int nReqTDynbuf = 0;
   int nTDynbufUsed = 0;
   char aszTempbuf[100];			
   char *paszTempbuf;
   const char *pszType;
   char *pcBufScroll = NULL;
   int nTBufScroll = 0;
   int nBufScroll = 1;
   int nBufScrollUsed = 0;
   int nBufScrollXtra = 1;
   int nTotalBufSize = 0;

   boolean fMemOut = FALSE;

   // dynamic memory for scroll info
   pcBufScroll = (char *)MALLOC( nBufScroll );
   if (NULL == pcBufScroll) {
		fMemOut = TRUE;
   }
   nTBufScroll = SNPRINTF(NULL, 0, "%d", me->dsi.yTop);
   if (SUCCESS == MemGrowEx( (void **)&pcBufScroll, &nBufScroll, nBufScrollUsed, nTBufScroll, nBufScrollXtra) ) {
      nTBufScroll = SNPRINTF(pcBufScroll, nBufScroll, "%d", me->dsi.yTop);
      nBufScrollUsed += nTBufScroll;
   } else {
      fMemOut = TRUE;
   }

   // dynamic memory for state string
   pcDynbuf = (char *)MALLOC( (nDynbuf + nBufScrollUsed) );
   if (NULL == pcDynbuf) {
	   fMemOut = TRUE;;
   }

   // dynamic memory for encoding
   pcTDynbuf = (char *)MALLOC(nTDynbuf);
   if (NULL == pcTDynbuf) {
		fMemOut = TRUE;
   }
   
   // pass -1 as we will iterate through all elems that can change state
   DoxIter_Ctor(&di, me, -1);
   while ( DoxIter_GetNextFormItem( &di, &pifc, &pszType ) && FALSE == fMemOut ) {
      if (pifc) {

	     // get only text, radio and checkbox
         if (0 == STRCMP( "text", pszType ) || 0 == STRCMP( "radio", pszType ) || 0 == STRCMP( "checkbox", pszType ) ) {
            // [SOH]
			   if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
               *(pcDynbuf + nDynbufUsed) = FS_SOH;
				   nDynbufUsed++;
			   } else {
			      fMemOut = TRUE;
			      break;
			   }

            // [STX]
			   if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
               *(pcDynbuf + nDynbufUsed) = FS_STX;
				   nDynbufUsed++;
			   } else {
			      fMemOut = TRUE;
			      break;
			   }

            // name
            MEMSET( (void *) aszTempbuf, 0, sizeof(aszTempbuf) );
			   Dox_GetAttrString( di.doxitem.pdox, di.doxitem.pos, "name", aszTempbuf, sizeof(aszTempbuf) );
			   paszTempbuf = aszTempbuf;
			   while (0 != *paszTempbuf) {
				   if ( SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                  *(pcDynbuf + nDynbufUsed) = *paszTempbuf++;
					   nDynbufUsed++;
				   } else {
				      fMemOut = TRUE;
				      break;
				   }
			   }

			   // [ETX]
			   if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
               *(pcDynbuf + nDynbufUsed) = FS_ETX;
				   nDynbufUsed++;
			   } else {
			      fMemOut = TRUE;
			      break;
			   }

            // [STX]
			   if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
               *(pcDynbuf + nDynbufUsed) = FS_STX;
				   nDynbufUsed++;
			   } else {
			      fMemOut = TRUE;
			      break;
			   }

		       // value
			   if (0 == STRCMP("radio", pszType) || 0 == STRCMP("checkbox", pszType)) {
               MEMSET( (void *) aszTempbuf, 0, sizeof(aszTempbuf) );
		          // get the name attr
			      Dox_GetAttrString(di.doxitem.pdox, di.doxitem.pos, "value", aszTempbuf, sizeof(aszTempbuf) );
			      paszTempbuf = aszTempbuf;
			      while (0 != *paszTempbuf) {
				      if ( SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                     *(pcDynbuf + nDynbufUsed) = *paszTempbuf++;
					      nDynbufUsed++;
				      } else {
				         fMemOut = TRUE;
				         break;
				      }
			      }
			   } else {
				   // clear the temp dynamic buffer
				   MEMSET( (void *)pcTDynbuf, 0, nTDynbuf );
				   nTDynbufUsed = 0;
               nReqTDynbuf = 0;
               if (SUCCESS == IFORMCONTROL_GetData( pifc, NULL, &nReqTDynbuf) ) {
                  if (SUCCESS == MemGrowEx( (void **)&pcTDynbuf, &nTDynbuf, nTDynbufUsed, nReqTDynbuf, nDynbufXtra) ) {
                     if (SUCCESS == IFORMCONTROL_GetData( pifc, pcTDynbuf, &nTDynbuf) ) {
                        paszTempbuf = pcTDynbuf;
                        while (0 != *paszTempbuf) {
                           if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                              *(pcDynbuf + nDynbufUsed) = *paszTempbuf++;
                              nDynbufUsed++;
                           } else {
                              fMemOut = TRUE;
                              break;
                           }
                        }
                     } else {
                        fMemOut = TRUE;
                        break;
                     }
                  } else {
                     fMemOut = TRUE;
                     break;                  
                  }

               }
            }

			   // [ETX]
			   if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                  *(pcDynbuf + nDynbufUsed) = FS_ETX;
				  nDynbufUsed++;
			   } else {
			      fMemOut = TRUE;
			      break;
			   }

            // [STX]
			   if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                  *(pcDynbuf + nDynbufUsed) = FS_STX;
				  nDynbufUsed++;
			   } else {
			      fMemOut = TRUE;
			      break;
			   }

            // state
			   if ( 0 == STRCMP( "radio", pszType ) || 0 == STRCMP( "checkbox", pszType ) ) {
			      if (SUCCESS == IFORMCONTROL_GetModel( pifc, AEEIID_VALUEMODEL, (IModel**)&pivm ) ) {
				      if (TRUE == IVALUEMODEL_GetBool(pivm)) {
			            if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                           *(pcDynbuf + nDynbufUsed) = FS_CHECKED;
				           nDynbufUsed++;
			            } else {
			               fMemOut = TRUE;
			               break;
			            }
                  } else {
			            if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                           *(pcDynbuf + nDynbufUsed) = FS_UNCHECKED;
				           nDynbufUsed++;
			            } else {
			               fMemOut = TRUE;
			               break;
			            }
                  }
                  RELEASEIF(pivm);
               }
            } else {
			  if (SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                 *(pcDynbuf + nDynbufUsed) = FS_NONE;
				  nDynbufUsed++;
			  } else {
			     fMemOut = TRUE;
			     break;
			  }
            }

		   // [ETX]
		   if ( SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
              *(pcDynbuf + nDynbufUsed) = FS_ETX;
			  nDynbufUsed++;
		   } else {
			  fMemOut = TRUE;
			  break;
		   }

            // [STX]
			   if ( SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                  *(pcDynbuf + nDynbufUsed) = FS_STX;
				  nDynbufUsed++;
			   } else {
			      fMemOut = TRUE;
			      break;
			   }

            // type
			while (0 != *pszType) {
			   if ( SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra) ) {
                  *(pcDynbuf + nDynbufUsed) = *pszType++;
				  nDynbufUsed++;
			   } else {
			      fMemOut = TRUE;
			      break;
			   }
		   }

		   // [ETX]
		   if ( SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, 1, nDynbufXtra ) ) {
              *(pcDynbuf + nDynbufUsed) = FS_ETX;
			   nDynbufUsed++;
		   } else {
		      fMemOut = TRUE;
		      break;
		   }
         }
      }
   }

	// null terminate the string
	if ( SUCCESS == MemGrowEx( (void **)&pcDynbuf, &nDynbuf, nDynbufUsed, nDynbufMore, nDynbufXtra ) ) {
      *(pcDynbuf + nDynbufUsed) = 0;
	   nDynbufUsed++;
	} else {
		fMemOut = TRUE;
	}

   nTotalBufSize = nDynbufUsed + nBufScrollUsed;

   // copy to buffer if NOT out of memory OR counting mode
   if ( FALSE == fMemOut && NULL != pszBuf && cbBuf >= nDynbufUsed ) {
      STRLCPY( pszBuf, pcBufScroll, nTotalBufSize );
      STRLCAT( pszBuf, pcDynbuf, nTotalBufSize );             // state
   }

   // free temporary buffer
   FREE(pcTDynbuf );
   FREE(pcDynbuf );
   FREE(pcBufScroll );

   RELEASEIF(pifc);
   return (nTotalBufSize);
}


int Dox_GetImage(Dox *me, int16 idImage, IImage **ppii)
{
   ImgNode *pn;
   IImage *pii = 0;

   // search for a match in the list
   for (pn=me->pImages; pn; pn = pn->pNext) {
      if (pn->id == idImage) {
         *ppii = pn->pii;
         IIMAGE_AddRef(pn->pii);
         return SUCCESS;
      }
   }

   pii = ISHELL_LoadResImage(me->piShell, HTMLWIDGET_RES_FILE, idImage);
   if (!pii) {
      return ERESOURCENOTFOUND;
   }

   pn = MALLOCREC(ImgNode);
   if (!pn) {
      RELEASEIF(pii);
      return ENOMEMORY;
   }

   pn->pNext   = me->pImages;
   me->pImages = pn;
   pn->id      = idImage;
   pn->pii     = pii;

   {
      uint32 dw;
      (void)dw;
      dw = IIMAGE_AddRef(pn->pii);    // extra addref because it's also in the table
   }

   *ppii       = pii;

   return SUCCESS;
}


void Dox_FctNotify(Dox *me, IFormControl *pifc, int nCode)
{
   switch (nCode) {
      
      case FCN_REWRAP:
         // asynchronously schedule a relayout
         xDox_Invalidate(me, DOX_INVALWRAP);
         break;
   
      case FCN_SUBMIT:
         Dox_SubmitForm(me, DoxObjMgr_GetObjectPos(&(me->doxObjMgr), pifc));
         break;
      
      case FCN_RESET:
         Dox_ResetForm(me, DoxObjMgr_GetObjectPos(&(me->doxObjMgr), pifc));
         break;
      
      case FCN_RADIOSELECT:
         Dox_CheckRadios(me, DoxObjMgr_GetObjectPos(&(me->doxObjMgr), pifc));
         break;
         
      case FCN_OBJECTLOADED:
         break;
   }

}


/* xDox_ObtainObject
|| finds or creates a form control object (widget)
||
*/
IFormControl *xDox_ObtainObject(Dox *me, int pos)
{
   if (!DoxIsAtElement(me, pos)) {
      return NULL;
   }
   return DoxObjMgr_ObtainObject(&me->doxObjMgr, pos);
}


// (-) NEW STUFF
////////////////////////////////////////////////////////////////////////////////



void xDox_SetCharset(Dox *me, const char *pszCharset, int cbCharset)
{
   CharSet_SetCharset(&me->cs, pszCharset, cbCharset);
}


//
//----  Ctor/Dtor
//


static const AEECLSID gfnts[5] = {
   AEECLSID_FONTSYSNORMAL,   // normal font

#ifdef AEECLSID_FONTSYSBOLD
   AEECLSID_FONTSYSBOLD,     // bold font
#else
   AEECLSID_FONTSYSNORMAL,   // normal font
#endif

#ifdef AEECLSID_FONTSYSLARGE
   AEECLSID_FONTSYSLARGE,    // large font
#else
   AEECLSID_FONTSYSNORMAL,   // normal font
#endif

#ifdef AEECLAID_FONTSMALL
   AEECLAID_FONTSMALL,    // SMALL font
#else
   AEECLSID_FONTSYSNORMAL,   // normal font
#endif

#ifdef AEECLSID_FONTSYSITALIC
   AEECLSID_FONTSYSITALIC,   // italic font
#else
   AEECLSID_FONTSYSNORMAL,   // normal font
#endif
};


static int Dox_CreateFont(Dox *me, int ndxFont, IFont **ppo)
{
   int nErr;
   ndxFont = CONSTRAIN(ndxFont, 0, ARRAYSIZE(gfnts)-1);
   nErr = ISHELL_CreateInstance(me->piShell, gfnts[ndxFont], (void**)ppo);
   if (nErr) {
      // fallback to normal font if above failed
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_FONTSYSNORMAL, (void**)ppo);
   }
   return nErr;
}


// ElemStyle flags shortcuts:
#define ESF_Font(f)    (gfnts[(f) & 3])
#define ESF_Ascent(f)  (me->fntMetrics[(f) & 3].a)
#define ESF_Descent(f) (me->fntMetrics[(f) & 3].d)
#define ESF_Hgt(f)     (me->fntMetrics[(f) & 3].hgt)
#define ESF_SpcWid(f)  (me->fntMetrics[(f) & 3].spcwid)
#define ESF_IFont(f)   (me->fntMetrics[(f) & 3].piFont)


// Set/initialize character set
//
void xDox_SetType(Dox *me, const char *pszMediaType)
{
   const char *pszCharset = NULL;
   int cbCharset = 0;
   
   // Okay, we're just an HTML viewer so there's not much to do with this
   // except look for a character set.

   if (pszMediaType)
      pszCharset = FindParamValue(pszMediaType, "charset", &cbCharset);

   CharSet_SetCharset(&me->cs, pszCharset, cbCharset);
}


static uint32 IContainer_AddRef(IContainer *po)
{
   return 1;
}

static uint32 IContainer_Release(IContainer *po)
{
   return 1;
}

static int IContainer_QueryInterface(IContainer *po, AEECLSID id, void **ppo)
{
   if ((id == AEECLSID_QUERYINTERFACE) || 
       (id == AEEIID_CONTAINER) ) {
      *ppo = po;
      return SUCCESS;
   }
   *ppo = 0;
   return ECLASSNOTSUPPORT;
}

static void IContainer_Invalidate(IContainer *po, IWidget *piw, 
                                     const AEERect *prc, uint32 dwFlags)
{
   Dox *me = (Dox*)po;

   // invalidate up to our parent
   {
      IContainer *pic = 0;
      IWIDGET_GetParent(me->piw, &pic);
      if (pic) {
         ICONTAINER_Invalidate(pic, piw, NULL, 0);
         ICONTAINER_Release(pic);
      }
   }
}

static int IContainer_Locate(IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRelative)
{
   return EFAILED;
}


// allocate our parse buffer
static int Dox_AllocBuffer(Dox *me, uint32 dwSize, boolean bInit)
{
   int nErr = 0;
   char *pc = me->pcBuf ? me->pcBuf-1 : 0;

   pc = REALLOC(pc, dwSize);
   if (!pc) {
      nErr = ENOMEMORY;
   }

   if (!nErr) {
      me->cbBuf = dwSize - 2;
      me->pcBuf = pc + 1;

      if (me->cbData > me->cbBuf) {
         me->cbData = me->cbBuf;
      }

      if (bInit) {
         pc[0] = DOX_SE;
         pc[1] = DOX_EE;
      }
   }

   return nErr;
}

RGBVAL xDox_GetLinkColor(Dox *me)
{
   return me->cfg.rgbLink;
}

void xDox_SetLinkColor(Dox *me, RGBVAL rgbLink)
{
   me->cfg.rgbLink = rgbLink;
   (void)xDox_Invalidate(me, DOX_INVALDOC);
}

RGBVAL xDox_GetDocBGColor(Dox *me)
{
   return me->cfg.rgbBackground;
}

void xDox_SetDocBGColor(Dox *me, RGBVAL rgbDocBg)
{
   me->cfg.rgbBackground = rgbDocBg;
   (void)xDox_Invalidate(me, DOX_INVALDOC);
}

void xDox_SetActive(Dox *me, boolean bActive)
{
   if (bActive != me->bActive) {
      me->bActive = bActive;
      (void)xDox_Invalidate(me, DOX_INVALFOCUS);
   }
}

boolean xDox_IsActive(Dox *me)
{   
   return me->bActive;
}

void xDox_SetScrollMLLink(Dox *me, boolean b)
{
   me->bScrollMLLink = b;
}

boolean xDox_GetScrollMLLink(Dox *me)
{
   return me->bScrollMLLink;
}

int xDox_SetBufferSize(Dox *me, uint32 dwSize)
{
   return Dox_AllocBuffer(me, dwSize, 0);
}

uint32 xDox_GetBufferSize(Dox *me)
{
   return me->cbBuf + 2;
}

int Dox_GetFontMetrics(Dox *me)
{
   int i;
   int nErr = 0;
   for (i=0; !nErr && i < ARRAYSIZE(me->fntMetrics); ++i) {

     if (NULL == me->fntMetrics[i].piFont)
     {
        nErr = Dox_CreateFont(me, i, &me->fntMetrics[i].piFont);
     }
      if (!nErr) {
         AEEFontInfo fi;
         int fits, width;
         AECHAR wc = (AECHAR)' ';   // space character

         IFONT_GetInfo(me->fntMetrics[i].piFont, &fi, sizeof(fi));
         IFONT_MeasureText(me->fntMetrics[i].piFont, &wc, 1, 999, &fits, &width);

         me->fntMetrics[i].a = fi.nAscent;
         me->fntMetrics[i].d = fi.nDescent;
         me->fntMetrics[i].hgt = fi.nAscent + fi.nDescent;
         me->fntMetrics[i].spcwid = width;
      }
   }
   return nErr;
}

void Dox_SetFont(Dox *me, AEEFont fnt, IFont *piFont)
{
  uint32 nIdx = (fnt & 3);
  if (NULL != piFont)
  {
    RELEASEIF(me->fntMetrics[nIdx].piFont);
    me->fntMetrics[nIdx].piFont = piFont;
    ADDREFIF(piFont);

    (void)Dox_GetFontMetrics(me);
    // don't invalidate, assuming font is set before document text is
  }
}

int Dox_Construct(Dox *me, IShell *piShell, IHtmlViewModel *piModel, IWidget *piHtmlWidget)
{
//   char *pc;
//   int i;
   int nErr = 0;

   // allocate our parse buffer
   nErr = Dox_AllocBuffer(me, DOX_DEFAULTSIZE, 1);
   if (nErr) {
      return nErr;
   }

   // allocate a temp buffer for character set conversions
   me->cchTemp = DOX_TEMPBUFFERSIZE;

   me->pwTemp = MALLOC(DOX_TEMPBUFFERSIZE * sizeof(AECHAR));
   if (!me->pwTemp) {
      return ENOMEMORY;
   }

   me->vt.AddRef         = IContainer_AddRef;
   me->vt.Release        = IContainer_Release;
   me->vt.QueryInterface = IContainer_QueryInterface;
   me->vt.Invalidate     = IContainer_Invalidate;
   me->vt.Locate         = IContainer_Locate;
   me->pvt = &me->vt;

   me->piModel = piModel;
   IHTMLVIEWMODEL_AddRef(piModel);

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->piw = piHtmlWidget;    // no AddRef or Release on this!!

   me->bActive = 1;
   
   me->posFocus = -1;                   // -1 => no focus item chosen
   me->posFocusActive = -1;             // -1 => no focus item visible/active
   me->bScrollMLLink = TRUE;

   me->cyScrollInc = 12;
   me->fAccelScroll = 0;
   
   me->cbData  = 0;

   {
      AEEDeviceInfo di;
      ISHELL_GetDeviceInfo(piShell, &di);
      CharSet_Ctor(&me->cs, di.wEncoding);
   }

//   DStrClass_Ctor(&me->dstrClass, piShell);
  nErr = Dox_GetFontMetrics(me);

   // Default configuration
   
   me->cfg.xIndent = me->fntMetrics[0].a;
   me->cfg.ySpace = (me->fntMetrics[0].hgt + 1) >> 1;
   me->cfg.rgbLink = MAKE_RGBA(0, 0, 0xFF, 0);        // links are blue by default
   me->cfg.rgbText = CLR_SYS_ITEM_TEXT;               // use system default text color
   me->cfg.rgbBackground = RGBA_WHITE;                // initial background will be transparent;

   // Set type (and initialize charset)
   xDox_SetType(me, "text/html");

   nErr = ISHELL_CreateInstance(piShell, AEECLSID_WEBUTIL, (void**)&me->piWebUtil);

   if (!nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_DISPLAY, (void**)&me->piDisplay);
   }

   if (!nErr) {
      nErr = DoxObjMgr_Construct(&me->doxObjMgr, me, (PFNFCTNOTIFY)Dox_FctNotify, me);
   }

   if (nErr) {
      xDox_Dtor(me);
   }

   return nErr;
}


void xDox_StopObjects(Dox *me)
{
// @@@
//   int i;
//   
//   for (i=0; i < ARRAYSIZE(me->otbl) && me->otbl[i].nID; ++i) {
//      if (me->otbl[i].pco) {
//         ICTLOBJ_Stop(me->otbl[i].pco);
//      }
//   }
}


static void Dox_ReleaseObjects(Dox *me)
{
   DoxObjMgr_Reset(&me->doxObjMgr);
   me->pifcActive = 0;
}


// Change the 'placeholder' focus item.  Actual displayed focus item will depend
// upon visibility and bActive.
//
// posItem = focus position, or some other document position (valid
//           text or object position).  -1 => no focus
//
static void Dox_SetFocusItem(Dox *me, int posItem)
{
   if (posItem != me->posFocus) {
      me->posFocus = posItem;
      xDox_Invalidate(me, DOX_INVALFOCUS);
   }
}


//static int Dox_GetFocusPos(Dox *me)
//{
//   return me->posFocus;
//}


//static void Dox_SetActive(Dox *me, boolean bActive)
//{
//   if (bActive != me->bActive) {
//      me->bActive = bActive;
//      Dox_Invalidate(me, DOX_INVALFOCUS);
//   }
//}


//static boolean Dox_IsActive(Dox *me)
//{   
//   return me->bActive;
//}


static void Dox_FreeImages(Dox *me)
{
   // free image nodes
   while (me->pImages) {
      ImgNode *pf = me->pImages;
      me->pImages = pf->pNext;
      RELEASEIF(pf->pii);
      FREE(pf);
   }
}


// Reset values relating to buffer content (called before loading new document)
//
// bInval should generally be TRUE to redraw the document area properly,
//   but FALSE can be passed if it is known that another invalidation is
//   pending.  (When re-loading a new document, there is no particular reason
//   to invalidate before the new document is loaded.)
//
void xDox_Reset(Dox *me, boolean bInval)
{
   me->fInvalid = DOX_INVALALL;  // prevent callbacks (@)

   Dox_SetFocusItem(me, -1);
   Dox_SetScrollPos(me, 0);

   me->fErrors = 0;
   me->cbData = 0;
   me->dsi.yTop = 0;
   me->dsi.yDocHeight = 0;
   me->cntFocusTbl = 0;
   me->cntPlacements = 0;

   me->fInvalid = 0;

   me->nRefImages = 0;
   me->pImgList = NULL;

   Dox_FreeImages(me);

   me->fLayoutDone = 0;
   me->fSendContentDone = 0;
   me->nFirstPage = 0;

   //me->cfg.rgbBackground = CLR_SYS_WIN;

   //MEMSET((void *)&me->ptTracker, 0, sizeof(me->ptTracker));
   Dox_ReleaseObjects(me);

   if (bInval)
      xDox_Invalidate(me, 0);
}


void xDox_Dtor(Dox *me)
{
   int i;
   char *pcFree = me->pcBuf - 1;

   // free image nodes
   Dox_FreeImages(me);

   RELEASEIF(me->piShell);
   RELEASEIF(me->piWebUtil);
   RELEASEIF(me->pIWeb);
   RELEASEIF(me->piModel);
   RELEASEIF(me->piDisplay);

   // release our fonts
   for (i=0; i < ARRAYSIZE(me->fntMetrics); i++) {
      RELEASEIF(me->fntMetrics[i].piFont);
   }

   FREE(pcFree);
   FREE(me->pszUrl);
   FREE(me->pwTemp);

   DoxObjMgr_Dtor(&me->doxObjMgr);
}


AECHAR *Dox_GetTempBuffer(Dox *me, int *pnSize)
{
   if (pnSize) {
      *pnSize = me->cchTemp;
   }
   return me->pwTemp;
}

//
//----  Layout
//


// Layout/drawing:
//
// Layout visits all elements and calculates their positions (and positions of
// subsections when elements are broken, as when lines of text are wrapped).
// This is required for drawing and for other functions (such as finding the
// x/y position of an item).
//
// If drawing is done when the layout routine calculcates the positions, there
// is no need to maintain a data structure of positions for all components.
//
// Likewise, the layout routine can be used to provide results as needed for
// other functions.
//
// Repeated recalculation (and slow performance) is the apparent drawback of
// this approach, and this can be ameliorated with a cache.  Such a cache
// might resemble in some ways a data struture of positions, but the key
// differences are:
//
//  1. A cache can be of a fixed size and still support an unlimited document
//     size.
//
//  2. Changes in the underlying data structure might have complicated
//     repercussions on data structure (or in recalculation), but with a
//     cache, portions can be easily invalidated.
//
//  3. A fixed data structure provides accelerated access to one set of data
//     (the "end result") when there are actually many such data sets involved
//     in the layout operation.  In implementing a cache, we can choose from
//     among different places to cache results.
//
//  4. Caches have a minor impact on the layout algorithm.
//
// In reality, there is probably a spectrum of design choices between
// "caching" and maintaining a "results" data structure, and in implementing a
// cache there is no limit to how far one may go in the direction of making
// the data persistent.  But deferring that implementation work to a later
// stage, and basing its implementation on quantitative metrics is advisable.
//



// acmHtmlElems


// Notes:
//
// Complexity phases:
//
//  1 - Sequence of chunks; wrapping within chunks; const. horizontal alignment
//  2 - Objects; Vertical spacing; text styles & block spacing
//  3 - Multiple chunks/objects per line
//
//
// To construct algorihtm, state invariants at different steps (i.e. each
// element start/stop, etc.), and construct loop around that.
//
//
/*
  
Phase 1:


   // PHASE 1 : Wrap within contiguous text blocks

   DoxItem i;
   int y = 0;

   
   DoxItem_Ctor(&i, 0);

   while (DoxItem_Next(&i, me)) {

      if (!DoxIsElemDelim( i.pc[0] )) {
		  const char *pc = i.pc;
		  int cb = i.cb;
		  int pos = i.pos;

         // Constant in this loop: LINEWID, LEFT, ALIGNMENT
   
         while (cb) {
            int x, widFits, cbFits, nHeight, cbOut;

            // Vars:
            //   pc[0..cb] = text to wrap
            //   y = position of next line

   
            MeasureText(pc, cb, nWid, &widFits, &cbFits, &nHeight);

            cbOut = cbFits;

            if (cbOut < cb) {
               // back up to word break
               while (cbOut && pc[cbOut] != SPC)
                  --cbOut;

               if (cbOut == 0) {
                  // word doesn't fit: break inside word!
                  cbOut = cbFits;
                  if (cbOut == 0) {
                     // first character doesn't fit(!) : include at least one
                     ++cbOut;
                  }
               }
            }

            // Trim trailing spaces
            while (cbOut && pc[cbOut-1] == SPC)
               --cbOut;
      
            // Output line
            x = xAlignShift(ALIGNMENT, widFits, nWid);
            Dox_Place(me, pos, pc, cbOut, x, y);

            // Skip trailing spaces
            while (cbOut < cb && pc[cbOut] == SPC)
               ++cbOut;

            y += nHeight;
            pc += cbOut;
			pos += cbOut;
            cb -= cbOut;
         }
      } else {
         // SE/EE
      }
   }



Phase 2:


   ElemStyle etbl[MAXELEMNEST];
   ElemStyle *pes = &etbl[0];
   int ndxNest = 0;

   ElemStyle_Ctor(pes);

   dp.yLineBtm = -100;
   
   DoxItem_Ctor(&i, 0);
   
   while (DoxItem_Next(&i, me)) {
      const char *pc = i.pc;
      int cb = i.cb;
      int pos = i.pos;

      if (!DoxIsElemDelim( i.pc[0] )) {

         // Constant in this loop: LINEWID, LEFT
   
         while (cb) {
            int widFits, cbFits, nHeight, nAscent, cbOut;

            // Vars:
            //   pc[0..cb] = text to wrap
            //   y = position of next line

   
            Dox_MeasureText(me, pc, cb, nWidth, pes, &widFits, &cbFits,
                            &nHeight, &nAscent);

            cbOut = cbFits;

            if (cbOut < cb) {
               // back up to word break
               while (cbOut && pc[cbOut] != SPC)
                  --cbOut;

               if (cbOut == 0) {
                  // word doesn't fit: break inside word!
                  cbOut = cbFits;
                  if (cbOut == 0) {
                     // first character doesn't fit(!) : include at least one
                     ++cbOut;
                  }
               }
            }

            // Trim trailing spaces
            while (cbOut && pc[cbOut-1] == SPC)
               --cbOut;
      
            // Output line
            dp.pos = pos;
            dp.cb = cbOut;
            dp.x = xAlignShift(pes, widFits, nWidth);
            dp.y = y;
            dp.yLineTop = y;
            dp.yLineBtm = y + nHeight;
            
            BRIDLE_ASSERT_SAFE_PFN(pfnPlace);
            pfnPlace(me, &dp, pc, NULL, pes);

            // Skip trailing spaces
            while (cbOut < cb && pc[cbOut] == SPC)
               ++cbOut;

            y += nHeight;
            pc += cbOut;
            pos += cbOut;
            cb -= cbOut;
         }
      } else if (pc[0] == DOX_SE) {
         // SE
         if (pc[1] == DoxIDChar(ELEM_INPUT) ||
             pc[1] == DoxIDChar(ELEM_TEXTAREA) ||
             pc[1] == DoxIDChar(ELEM_SELECT) ) {

            // Inline object represents element
            ICtlObj * pco;
            i.cb = cb = Dox_NextSibling(me, pos) - pos;
            pco = xDox_ObtainObject(me, pos);

            if (pco) {
               CtlExtent ce;
               ICTLOBJ_GetExtent(pco, &ce);

               dp.pos = pos;
               dp.cb = cb;
               dp.x = xAlignShift(pes, ce.nWidth, nWidth);
               dp.y = y;
               dp.yLineTop = y;
               dp.yLineBtm = y + ce.nHeight;

               BRIDLE_ASSERT_SAFE_PFN(pfnPlace);
               pfnPlace(me, &dp, pc, pco, pes);

               y = dp.yLineBtm;
            }
         } else if (pc[1] == DoxIDChar(ELEM_BR)) {
            // <BR>
            dp.yLineBtm += ESF_Hgt(0);
            y += ESF_Hgt(0);
         }

         if (pc[cb] == DOX_EE) {
            // Empty element: Skip end delimiter; don't nest
            ++i.cb;
         } else {
            // Nest
               
            ++ndxNest;
            if (ndxNest < ARRAYSIZE(etbl)) {
               pes = etbl+ndxNest;
               Dox_ElemStyleNest(me, etbl+ndxNest, etbl+ndxNest-1, pos, pc);

               if (ElemStyle_IsBlock(pes)) {
                  if (y < dp.yLineBtm + pes->Space) {
                     // Enter block
                     y = dp.yLineBtm + pes->ySpace;
                  } else {
                     nWidth = me->rc.dx - pes->xLft - pes->xRgt;
                  }
               }
            }
         }
      } else {
         // EE

         if (ndxNest < ARRAYSIZE(etbl) &&
             y < dp.yLineBtm + pes->ySpace) {
            // Close blockp
            y = dp.yLineBtm + pes->ySpace;
         }
         
         --ndxNest;
         if (ndxNest < 0)
            ndxNest = 0;

         if (ndxNest < ARRAYSIZE(etbl)) {
            pes = etbl+ndxNest;

            nWidth = me->rc.dx - pes->xLft - pes->xRgt;
         }
      }
   }



*/


#define LINEWID     100
#define SPC         32

#ifndef MAXSHORT
#define MAXSHORT  32767
#endif


static void ElemStyle_Ctor(ElemStyle *me, Dox *pdox)
{
   SETZERO(me);

//   me->rgb = CLR_SYS_ITEM_TEXT;
   me->rgb = pdox->cfg.rgbText;
   me->nSize = ELEMSTYLE_SIZEDFLT;
}


#define DoxElemIsBlock(n)  ((acmHtmlElems[n].fClass & (CONTENT_BLCK | CONTENT_TD | CONTENT_TR)) != 0)


// Construct a new ElemStyle from a parent style and element
//
static void ElemStyle_CtorNested(ElemStyle *pes, ElemStyle *pesParent, const char *pcElem, Dox *pdox)
{
   int nElem = DoxGetID(pcElem[1]);
   uint8 flags = pesParent->flags;
   int fDisp;
   const char *pcAttr;

   *pes = *pesParent;

   if (nElem >= ELEM_MAX)
      return;
   
   pes->rgb = pdox->cfg.rgbText;

   flags |= acmHtmlElems[nElem].flags;
   
   if (nElem == ELEM_A) {

      flags |= ELEMFLAG_LINK;

      if (xDoxGetAttrValue(pcElem, ATTR_HREF)) {
         flags |= ELEMFLAG_U;
      }

      pes->rgb = pdox->cfg.rgbLink;
   }


   if (nElem == ELEM_FONT) {
      // COLOR
      RGBVAL rgb = DoxGetAttrColor(pcElem, ATTR_COLOR, RGB_NONE);
      if (rgb != RGB_NONE) {
         pes->rgb = rgb;
      }

      // SIZE
      pcAttr = xDoxGetAttrValue(pcElem, ATTR_SIZE);
      if (pcAttr) {
         int nSize = ELEMSTYLE_SIZEDFLT;
         if (CHARISIN(pcAttr[0], '0', '9')) {
            nSize = DoxGetInt(pcAttr, ELEMSTYLE_SIZEDFLT);
         } else if (pcAttr[0] == '+') {
            nSize += DoxGetInt(pcAttr+1, 0);
         } else if (pcAttr[0] == '-') {
            nSize -= DoxGetInt(pcAttr+1, 0);
         }
         pes->nSize = DoxConstrain(nSize, 1, 7);

         if (pes->nSize > ELEMSTYLE_SIZEDFLT) {
            flags |= ELEMFLAG_BIG;
         } else {
            flags &= ~ELEMFLAG_BIG;
         }
      }
   }
   
   fDisp = acmHtmlElems[nElem].fDisp;

   if (fDisp & ELEMDISP_BLOCK) {

      // BLOCK ELEMENT

      int nMarg = pdox->cfg.xIndent;
      int ySpace = 0;
      

      if (fDisp & ELEMDISP_HSPACE) {
         // Left Indent
         pes->xLft += nMarg;
         if (nElem == ELEM_BLOCKQUOTE)
            pes->xRgt += nMarg;
      }


      if (fDisp & ELEMDISP_VSPACE) {
         ySpace = pdox->cfg.ySpace;
      } else if (nElem == ELEM_CENTER) {
         // (CENTER is not a VSPACE element)
         ESF_SetAlign(flags, ELEMFLAG_ALIGNCENTER);
      }
      
      if (fDisp & ELEMDISP_ALIGN) {
         pcAttr = xDoxGetAttrValue(pcElem, ATTR_ALIGN);
         if (pcAttr) {
            if (xDoxStrEQI(pcAttr, "center"))
               ESF_SetAlign(flags, ELEMFLAG_ALIGNCENTER);
            else if (xDoxStrEQI(pcAttr, "right"))
               ESF_SetAlign(flags, ELEMFLAG_ALIGNRIGHT);
            else if (xDoxStrEQI(pcAttr, "left"))
               ESF_SetAlign(flags, 0);
         }
      }
      pes->ySpace = ySpace;
      
   } else {

      // Not a block
      pes->ySpace = -1;

   }

   pes->flags = flags;
}


// Measure characters that fit in widMax, up to cb.
//
// On exit:
//   Return value = width of measured characters
//   *pcbFits     = number of characters measured
//
static int Dox_MeasureText(Dox * me, const char *pc, int cb, int widMax, ElemStyle *pes, int *pcbFits)
{
   int nWidth, cbFits;

   //AEEFont fnt = ESF_Font(pes->flags);

   nWidth = DoxMeasureTextA(me->piDisplay, ESF_IFont(pes->flags), pc, cb, widMax, &cbFits, 
                            CharSet_GetDecoder(&me->cs));

   *pcbFits = cbFits;

   return nWidth;
}


static int xAlignShift(ElemStyle *pes, int nElemWid, int nLineWid)
{
   int x = 0;

   if (pes->flags & (ELEMFLAG_ALIGNCENTER | ELEMFLAG_ALIGNRIGHT)) {
      x = DoxConstrain(nLineWid - nElemWid, 0, MAXSHORT);
      if (pes->flags & ELEMFLAG_ALIGNCENTER)
         x /= 2;
   }
   return x + pes->xLft;
}


static void Dox_PlaceItem(Dox *me, DoxPlacement *pdp)
{
   int ndxPlc = me->cntPlacements;
   if (ndxPlc < ARRAYSIZE(me->ptbl)) {
      me->ptbl[ndxPlc] = *pdp;
      me->cntPlacements = ++ndxPlc;

      if (pdp->bFocus) {

         // Add focusable item to focus table
         
         int ndx = me->cntFocusTbl;

         if (ndx < ARRAYSIZE(me->ftbl)) {
            me->ftbl[ndx].pos = pdp->pos;
            me->ftbl[ndx].yTop = pdp->yLineTop;
            me->ftbl[ndx].yBtm = pdp->yLineBtm;

            me->cntFocusTbl = ndx+1;
         }

         if (ndx == 0)
            xDox_Invalidate(me, DOX_INVALFOCUS);  // auto-focus to first item?
      }
   }
}

static void Dox_DrawItem(Dox *me, ICanvas *piCanvas, DoxPlacement *pdp, const char *pc, ElemStyle *pes)
{
   int yScroll = me->dsi.yTop;
   int xScrn, yScrn;
   IDisplay *piDisplay = 0;
   
   if (pdp->yLineBtm < yScroll || pdp->yLineTop > yScroll + me->dsi.yRectHeight) {
      return;
   }

   xScrn = pdp->x + me->rc.x;
   yScrn = pdp->y + me->rc.y - yScroll;

   if (pc[0] == DOX_SE) {

      if(pc[1] == DoxIDChar(ELEM_HR)) {
         AEERect rc;
         AEERect trc;
         AEERect drc;
         IBitmap *pib = NULL;
         if (SUCCESS != ICANVAS_GetBitmap(piCanvas, &pib)) {
            return;
         }
         rc.x = me->rc.x+2;
         rc.dx = me->rc.dx-4;
         //rc.y = (int8)(me->rc.y + pdp->yLineTop - 8 - me->dsi.yTop);
         rc.y = (int8)(me->rc.y + pdp->y - me->dsi.yTop);
         rc.dy = 1;
         (void)ICANVAS_GetClipRect(piCanvas, &trc);
         if(TRUE == IntersectRect(&drc, &trc, &rc)){
            (void)IBITMAP_FillRect(pib, &drc, IBITMAP_RGBToNative(pib, RGB_BLACK), AEE_RO_COPY);
         }
         RELEASEIF(pib);
      }else{
         // OBJECT
         IFormControl *pifc = xDox_ObtainObject(me, pdp->pos);
         if (pifc) {
            IFORMCONTROL_Draw(pifc, piCanvas, xScrn, yScrn);
         }
      }
   } else if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {      
      uint32 fflg = 0;
      int pos1 = -1;
      int pos2 = -1;

      // TEXT
      RGBVAL rgbSave, rgbBgSave;
      IFont *piFontTemp = IDISPLAY_SetFont(piDisplay, AEE_FONT_NORMAL, ESF_IFont(pes->flags));

	  // check if link
      if ((pes->flags & ELEMFLAG_U)){
         fflg |= IDF_TEXT_UNDERLINE;
      }

      if (me->posFocusActive >= 0) {
         pos1 = Dox_FindAncestor(me, me->posFocusActive, ELEM_A);
         if (pos1 >= 0)
            pos2 = Dox_FindAncestor(me, pdp->pos, ELEM_A);
      }
	  
      if (me->posFocusActive == pdp->pos || (pos1 >= 0 && pos2 >= 0 && pos1 == pos2)) {
		  if(me->bActive){           
           rgbSave = IDISPLAY_SetColor(piDisplay, CLR_USER_TEXT, me->cfg.rgbBackground & 0xFFFFFF00);
           rgbBgSave = IDISPLAY_SetColor(piDisplay, CLR_USER_BACKGROUND, pes->rgb & 0xFFFFFF00);
		  }else{
			  fflg |= IDF_TEXT_TRANSPARENT;
           rgbSave = IDISPLAY_SetColor(piDisplay, CLR_USER_TEXT, pes->rgb & 0xFFFFFF00);
           rgbBgSave = IDISPLAY_SetColor(piDisplay, CLR_USER_BACKGROUND, me->cfg.rgbBackground & 0xFFFFFF00);
		  }
      } else {
         fflg |= IDF_TEXT_TRANSPARENT;
         rgbSave = IDISPLAY_SetColor(piDisplay, CLR_USER_TEXT, pes->rgb & 0xFFFFFF00);
         rgbBgSave = IDISPLAY_SetColor(piDisplay, CLR_USER_BACKGROUND, me->cfg.rgbBackground & 0xFFFFFF00);
      }

      // @ Need to check up on why we sometimes get a zero-length item
      // (probably has to do with space-trimming)
   
      if (pdp->cb) {
         DoxDrawTextA(piDisplay, AEE_FONT_NORMAL, pc, pdp->cb, xScrn, yScrn, &me->rc, fflg, 
                      CharSet_GetDecoder(&me->cs));
      }
   
      (void)IDISPLAY_SetFont(piDisplay, AEE_FONT_NORMAL, piFontTemp);
      (void)IDISPLAY_SetColor(piDisplay, CLR_USER_TEXT, rgbSave);
      (void)IDISPLAY_SetColor(piDisplay, CLR_USER_BACKGROUND, rgbBgSave);
   }

   RELEASEIF(piDisplay);
}


static void IBitmap_FillRectEx(IBitmap *me, int x, int y, int dx, int dy, RGBVAL rgb)
{
   AEERect rc;
   SETAEERECT(&rc, x, y, dx, dy);
   IBITMAP_FillRect(me, &rc, IBITMAP_RGBToNative(me, rgb), AEE_RO_COPY);
}


// Draw tag for LI element
//   pdp = first placement in the LI
//   pes = style active in LI
//
static void Dox_DrawListTag(Dox *me, IBitmap *pib, int pos, DoxPlacement *pdp, ElemStyle *pes)
{
   AEERect crc, drc, src;
   int yScroll = me->dsi.yTop;
   int yOffset = me->rc.y - yScroll;
   IDISPLAY_GetClipRect(me->piDisplay, &crc);
   src.x = pdp->x + me->rc.x - 5;
   src.y = yOffset + (pdp->yLineBtm + pdp->yLineTop) / 2 - 1;
   src.dx = 2;
   src.dy = 2;
   if(TRUE == IntersectRect(&drc, &crc, &src)){
      /*
      IBitmap_FillRectEx(pib, 
                         pdp->x + me->rc.x - 5,    // x
                         yOffset + (pdp->yLineBtm + pdp->yLineTop) / 2 - 1, // y
                         2, 2, RGB_BLACK);         // dx, dy, rgb
      */
      IBitmap_FillRectEx(pib, drc.x, drc.y, drc.dx, drc.dy, RGB_BLACK);
   }
}



// Measure whatever text fits in nWidth
//
// On exit:
//
//   pds->cbClip = number of characters that fit
//   pds->cbBrk  = position after last word break (last "valid" break point) (0 => no break)
//   pds->cbWord = position before last word break (0 => no break, or no word preceding it)
//   pds->widWord = width to cbWord
//   pds->widBrk  = width to cbBrk
//
static void Dox_LayoutText(Dox *me, DoxSize *pds, const char *pc, int cb, int nWidth, ElemStyle *pes)
{
   int cbClip, cbWord, cbBrk, widWord;

   // Vars:
   //   pc[0..cb] = text to wrap
   //   y = position of next line
   //   nWidth = size 

   pds->widClip = Dox_MeasureText(me, pc, cb, nWidth, pes, &cbClip);

   pds->nAscent = ESF_Ascent(pes->flags);
   pds->nDescent = ESF_Descent(pes->flags);

   if (cbClip == 0) {
	   // work around emulator bug
	   pds->widClip = 0;
   }

   cbWord = cbClip;

   // Find last space
   while (cbWord && pc[cbWord] != SPC)
      --cbWord;

   cbBrk = cbWord;

   // Back up to end of word
   while (cbWord && pc[cbWord-1] == SPC)
      --cbWord;
   
   // Run to end of spaces  (could extend past cbClip; cb should never end at space)
   while (pc[cbBrk] == SPC)
      ++cbBrk;

   if (NULL != CharSet_GetFindSplit(&me->cs)) {
      // Look for breaking CJK characters
      int cbSplit;

      cbSplit = CharSet_FindSplit(&me->cs, pc+cbBrk, pc+cbClip, pc+cb) - pc;

      if (cbSplit > cb)
         cbSplit = cb;
      
      if (cbSplit > cbBrk)
         cbWord = cbBrk = cbSplit;
   }
   
   // Measure word (if different from clip)

   if (cbWord == 0) {
      widWord = 0;
   } else if (cbWord >= cbClip) {
      widWord = pds->widClip;
   } else {
      int nDummy;
      if (cbWord * 2 <= cbClip) {
         widWord = Dox_MeasureText(me, pc, cbWord, nWidth, pes, &nDummy);
      } else {
         // subtract remainder
         int widRem = Dox_MeasureText(me, pc+cbWord, cbClip-cbWord, nWidth, pes, &nDummy);
         widWord = pds->widClip - widRem;
      }
   }

   pds->cbClip = cbClip;
   pds->cbBrk = cbBrk;
   pds->cbWord = cbWord;
   pds->widWord = widWord;
   pds->widBrk = widWord + ESF_SpcWid(pes->flags) * (cbBrk - cbWord);
}

//
//----  Intra-line state
//

static void DoxLine_SetElemStyle(DoxLine *me, ElemStyle *pes)
{
   me->nDocWidth = me->pdox->rc.dx - pes->xLft - pes->xRgt;
   me->pes = pes;
}


static void DoxLine_Ctor(DoxLine *me, Dox *pdox, ElemStyle *pes)
{
   me->yNext       = 0;               // top of line being laid
   me->yLineBtm    = -100;            // previous line bottom
   me->ndxItem     = 0;
   me->widMeasured = 0;
   me->pdox        = pdox;
   DoxLine_SetElemStyle(me, pes);
}


//
//   bBreak => break last item at cbWord
//
static boolean DoxLine_Place(DoxLine *me, int cntItems, int bBreak)
{
   DoxLineItem dliSave, *pli;
   DoxPlacement dp;
   int nWidth = 0;
   int nAscent = 0;
   int nDescent = 0;
   int nn;
   int cntMove;

   if (cntItems == -1)
      cntItems = me->ndxItem;

   if (cntItems > me->ndxItem || cntItems < 1)
      return 0;
   
   if (bBreak) {
      // Break at cbWord
      dliSave = me->dli[cntItems-1];
      me->dli[cntItems-1].ds.cbClip =  dliSave.ds.cbWord;
      me->dli[cntItems-1].ds.widClip = dliSave.ds.widWord;
   }
   

   // Measure included items
   for (nn = 0; nn < cntItems; ++nn) {
      nAscent = MAX(nAscent, me->dli[nn].ds.nAscent);
      nDescent = MAX(nDescent, me->dli[nn].ds.nDescent);
      nWidth += me->dli[nn].ds.widClip;
   }

   // Place items
   
   dp.x = xAlignShift(me->pes, nWidth, me->nDocWidth);
   dp.yLineTop = me->yNext;
   dp.yLineBtm = dp.yLineTop + nAscent + nDescent;

   for (nn = 0, pli = me->dli; nn < cntItems; ++nn, ++pli) {
      dp.pos    = pli->pos;
      dp.cb     = pli->ds.cbClip;
      dp.y      = dp.yLineTop + nAscent - pli->ds.nAscent;
      dp.bFocus = pli->bFocus;

      // Text links that span multiple lines should only make
      // first line focusable.
      if (pli->bFocus && pli->bText && bBreak && nn == (cntItems-1)) {
         dp.bFocus = 0;
      }

      Dox_PlaceItem(me->pdox, &dp);

      dp.x += pli->ds.widClip;
   }

   // Move leftover items to next line

   if (bBreak) {
      if (dliSave.ds.cbBrk < dliSave.ds.cbClip) {
         // Leftover chunk
         dliSave.pos += dliSave.ds.cbBrk;     // skip to break
         
         dliSave.ds.cbClip  -= dliSave.ds.cbBrk;
         dliSave.ds.widClip -= dliSave.ds.widBrk;
         dliSave.ds.cbBrk    = 0;
         dliSave.ds.widBrk   = 0;
         dliSave.ds.cbWord   = 0;
         dliSave.ds.widWord  = 0;

         --cntItems;
         me->dli[cntItems] = dliSave;
      }
   }

   cntMove = DoxConstrain(me->ndxItem - cntItems, 0, ARRAYSIZE(me->dli) - cntItems);
   if (cntMove) {
      MEMMOVE(me->dli, me->dli+cntItems, cntMove * sizeof(me->dli[0]));
   }
   me->ndxItem = cntMove;


   // reset measured width
   nWidth = 0;
   for (nn = 0; nn < me->ndxItem; ++nn)
      nWidth += me->dli[nn].ds.widClip;


   me->yNext = me->yLineBtm = dp.yLineBtm;
   me->widMeasured = nWidth;

   return 1;
}



static boolean DoxLine_Break(DoxLine *me, int yMargin)
{
   boolean bBreak = FALSE;
   int yMin;

   if (yMargin >= 0) {

      bBreak = DoxLine_Place(me, -1, FALSE);

      yMin = me->yLineBtm + yMargin;

      if (me->yNext < yMin)
         me->yNext = yMin;
   }
   return bBreak;
}


static void DoxLine_HR (DoxLine *me, int pos, int cb){
   DoxPlacement dp;
   int nAscent, nDescent, yMargin;
   nAscent = nDescent = 4;
   yMargin = 3;

   dp.x = (uint32)((int)me->pdox->rc.x + 1);
   dp.yLineTop = (int16)me->yNext + (int16)yMargin;
   dp.yLineBtm = (uint32)(dp.yLineTop + nAscent + nDescent);
   dp.y = (int16)(dp.yLineTop + nAscent);

   dp.pos    = (int16)pos;
   dp.cb     = (int16)cb;
   dp.bFocus = 0;

   Dox_PlaceItem(me->pdox, &dp);

   me->yNext = me->yLineBtm = dp.yLineBtm;
   me->widMeasured = 0;
}

static void DoxLine_Space(DoxLine *me, int yAdd)
{
   me->yLineBtm += yAdd;
   me->yNext += yAdd;
}



// Return number of bytes 'used' of the current item.  For objects, this will
// always be the total size.  For text, this may be a portion of the chunk or
// even zero.  (In the case of returning zero, the number of items on the line
// decreases, guaranteeing progress.)
//
//
static int DoxLine_AddItem(Dox *pDox, DoxSize *pds, int pos, int cbTotal, int bText, int bFocus)
{
   DoxLine *me = &pDox->dl;
   DoxLineItem *pli = &me->dli[me->ndxItem];
   int cbUsed;

   pli->pos    = pos;
   pli->bText  = bText;
   pli->bFocus = bFocus;
   pli->ds     = *pds;

   me->widMeasured += pds->widClip;

   cbUsed = pds->cbClip;

   ++me->ndxItem;
   
   // Full line?
   
   if (bText && cbUsed < cbTotal) {
      int cntItems = me->ndxItem;
      int bBreak = TRUE;
      int nn;

      // Text clipped => output line and continue measuring at break

      if (pds->cbBrk) {
         // Break within this most recent item
         cbUsed = pds->cbBrk;
         pli->ds.cbClip = pds->cbBrk;   // don't leave any leftovers
      } else {
         // look for break earlier; discard this text item
         for (nn = cntItems-1; nn > 0; --nn) {
            if (me->dli[nn-1].ds.cbBrk) {
               break;
            }
         }

         if (nn == 0) {
            // no breaks found: use all of this item
            bBreak = FALSE;
            if (cntItems == 1 && cbUsed == 0) {
               // Ensure progress
               cbUsed = CharSet_CharSize(&pDox->cs, Dox_BufGetChar(me->pdox, me->dli[0].pos));
               pli->ds.cbClip = 1;
               pli->ds.widClip = (me->nDocWidth - me->widMeasured);
            }
         } else {
            // break at nn; discard traliing item
            cntItems = nn;
            --me->ndxItem;
            cbUsed = 0;
         }
      }
      
      DoxLine_Place(me, cntItems, bBreak);

   } else if (!bText && me->widMeasured > me->nDocWidth) {

      if (me->ndxItem == 1) {
         // Place everything
         DoxLine_Place(me, -1, FALSE);
      } else {
         // Place up to, not including this object
         DoxLine_Place(me, me->ndxItem-1, FALSE);
      }
   } else if (me->ndxItem == ARRAYSIZE(me->dli)) {
      // Output everything just because our array is full
      DoxLine_Place(me, me->ndxItem, FALSE);
   }

   return cbUsed;
}



// Traverse document, measure & place everything.
//
// Returns height of document.
//
void xDox_Layout(Dox *me)
{
   DoxItem i;
   DoxSize ds;

//   boolean bInForm = 0;

   ElemStyle etbl[MAXELEMNEST];    // ~120 bytes
   ElemStyle *pes = &etbl[0];
   int ndxStyle = 0;

   me->cntFocusTbl = 0;
   me->cntPlacements = 0;

   ElemStyle_Ctor(pes, me);
   DoxLine_Ctor(&me->dl, me, pes);
   DoxItem_Ctor(&i, me, 0);

   if (me->rc.dx == 0)
      i.cbUsed = me->cbData;   // skip everything

   while (DoxItem_NextBR(&i)) {
      
      const char *pc = i.pc;

      if (!DoxIsElemDelim(pc[0])) {

         if (pc[0] == LFCHAR) {

            if (!DoxLine_Break(&me->dl, 0))
               DoxLine_Space(&me->dl, ESF_Hgt(pes->flags));
            
         } else {

            Dox_LayoutText(me, &ds, pc, i.cb, me->dl.nDocWidth - me->dl.widMeasured, pes);

            i.cbUsed = DoxLine_AddItem(me, &ds, i.pos, i.cb, TRUE,
                                       ((pes->flags&ELEMFLAG_LINK)!=0));
         }
         
      } else if (pc[0] == DOX_SE) {

         // SE

         unsigned nElem = DoxGetID(pc[1]);
         int fDisp;

         if (nElem >= ELEM_MAX)
            break;
         
         fDisp = acmHtmlElems[nElem].fDisp;

         if (fDisp & (ELEMDISP_OBJECT | ELEMDISP_IGNORE)) {
               
            i.cbUsed = Dox_NextSibling(me, i.pos) - i.pos;

            if (fDisp & ELEMDISP_OBJECT) {
               // OBJECT:  form control, etc.
               IFormControl *pifc = xDox_ObtainObject(me, i.pos);
               if (pifc) {
                  WExtent we;
                  int nCtlType;
                  int nBaseline;

                  int bFocus = (nElem != ELEM_IMG ||              // form controls are focus items
                                ((pes->flags&ELEMFLAG_LINK)!=0)); // IMG gets focus when in links
                  
                  IFORMCONTROL_GetType(pifc, &nCtlType);
                  IFORMCONTROL_GetExtent(pifc, &we);
                  IFORMCONTROL_GetBaseline(pifc, &nBaseline);

                  ds.cbClip = i.cbUsed;
                  ds.cbBrk  = i.cbUsed;
                  ds.cbWord = i.cbUsed;
                  ds.widClip = we.width;
                  ds.widWord = we.width;
                  ds.nAscent = nBaseline;
                  ds.nDescent = we.height-nBaseline;

                  i.cbUsed = DoxLine_AddItem(me, &ds, i.pos, i.cbUsed, FALSE, bFocus);
                  
                  // check for the following conditions:
                  // 1) first time thru Dox_Layout ?
                  // 2) object is an image ?
                  // 3) object appears within first page (screenful) of document ?
                  // if conditions are met, then increment count of first page
                  // images and increment the objects 'Param' value, which counts
                  // the object's appearance in the first page
                  if (!me->fLayoutDone && nCtlType == FCT_IMAGE) {
                     
                     int yTop = 0;
                     // if we haven't placed a line yet (cntPlacements == 0),
                     // then image will be placed at the top of the screen

                     if (me->cntPlacements > 0) {
                        yTop = me->ptbl[me->cntPlacements-1].y;
                     }

                     if (yTop < me->rc.dy) {
                        // increment this objects "first-page" ref count
                        uint32 dwVal;
                        IFORMCONTROL_GetExtra(pifc, &dwVal);
                        IFORMCONTROL_SetExtra(pifc, dwVal+1);
                        // increment count of first-page objects
                        me->nFirstPage++;
                     }
                  }
               }
            } // else IGNORE 

         } else if (pc[i.cb] == DOX_EE) {

            // EMPTY ELEMENT : Don't nest  (Ignore empty items, like line items)

            ++i.cbUsed;
            if (pc[1] == DoxIDChar(ELEM_BR)) {
               if (!DoxLine_Break(&me->dl, 0))
                  DoxLine_Space(&me->dl, ESF_Hgt(pes->flags));
               
            } else if (pc[1] == DoxIDChar(ELEM_HR)) {
               DoxLine_Break(&me->dl, 0);
               DoxLine_HR(&me->dl, i.pos, i.cb);      // all HR end with break
               (void)DoxLine_Break(&me->dl, 0);
            }               
         } else {
               
            // ENTER ELEMENT
            ++ndxStyle;

            if (nElem == ELEM_BODY) {
               me->cfg.rgbBackground = DoxGetAttrColor(pc, ATTR_BGCOLOR, MAKE_RGBA(0xFF, 0xFF, 0xFF, 0));
               me->cfg.rgbLink = DoxGetAttrColor(pc, ATTR_LINK, MAKE_RGBA(0, 0, 0xFF, 0));
            }

            if (ndxStyle < ARRAYSIZE(etbl)) {
               pes = etbl+ndxStyle;
               ElemStyle_CtorNested(etbl+ndxStyle, etbl+ndxStyle-1, pc, me);

               // Top margin for entered block
               DoxLine_Break(&me->dl, pes->ySpace);
               DoxLine_SetElemStyle(&me->dl, pes);
            }
         }
         
      } else if (pc[0] == DOX_EE) {

         // EE: EXIT ELEMENT

         if (ndxStyle > 0) {
            --ndxStyle;
            if (ndxStyle < ARRAYSIZE(etbl)-1) {
               // Bottom margin for exited block
               DoxLine_Break(&me->dl, pes->ySpace);
               pes = etbl+ndxStyle;
               DoxLine_SetElemStyle(&me->dl, pes);
            }
         }
      }
   }


   DoxLine_Place(&me->dl, -1, FALSE);

   if (me->dsi.yDocHeight != me->dl.yNext) {
      me->dsi.yDocHeight =  me->dl.yNext;
      // notify the application
      Dox_SendScrollNotify(me);
   }
   xDox_Invalidate(me, DOX_INVALFOCUS);

   // send HVN_CONTENTDONE
   if (me->fSendContentDone) {

      me->fSendContentDone = 0;

      // if there are no images or no images on first page
      if (!me->pImgList || !me->nFirstPage) {
         // notify the application 
         IHTMLVIEWMODEL_NotifyEvent(me->piModel, EVT_HVM_PAGEDONE);
      }

      // if there are no images
      if (!me->pImgList) {
         // notify the application 
         IHTMLVIEWMODEL_NotifyEvent(me->piModel, EVT_HVM_CONTENTDONE);
      }

   }

   me->fLayoutDone = 1;
}


// Draw placed items
//
void Dox_Draw(Dox *me, ICanvas *piCanvas, int x, int y)
{
   DoxItem i;
   ElemStyle etbl[MAXELEMNEST];    // ~120 bytes
   ElemStyle *pes = &etbl[0];
   int ndxStyle = 0;
   const char *pcBuf = me->pcBuf;
   int yStop;
   DoxPlacement *pp;
   DoxPlacement *ppMax;
   IBitmap *pib = 0;

   me->rc.x = x;
   me->rc.y = y;

   if (SUCCESS != ICANVAS_GetBitmap(piCanvas, &pib)) {
      return;
   }
   
   // set the page background
  (void)IBITMAP_FillRect(pib, &me->rc, IBITMAP_RGBToNative(pib, me->cfg.rgbBackground), AEE_RO_COPY);

   // yStop is in document-related coordinates, as are all of our coordinates
   // throughout Draw() and related functions.
   yStop = me->dsi.yTop + me->rc.dy;

   pp = me->ptbl;
   ppMax = me->ptbl + me->cntPlacements;

   ElemStyle_Ctor(pes, me);
   DoxItem_Ctor(&i, me, 0);

   while (DoxItem_Next(&i) && pp < ppMax && pp->y < yStop) {
      const char *pc = i.pc;

      while (pp < ppMax && pp->pos >= i.pos && pp->pos < i.pos+i.cb) {
         Dox_DrawItem(me, piCanvas, pp, pcBuf + pp->pos, pes);
         ++pp;
      }

      if (pc[0] == DOX_SE) {

         // SE

         unsigned nElem = DoxGetID(pc[1]);
         int fDisp;

         if (nElem >= ELEM_MAX)
            break;
         
         fDisp = acmHtmlElems[nElem].fDisp;

         if (fDisp & (ELEMDISP_OBJECT | ELEMDISP_IGNORE)) {

            // Skip OBJECT and IGNORE elements

            i.cbUsed = Dox_NextSibling(me, i.pos) - i.pos;

         } else if (pc[i.cb] == DOX_EE) {

            // EMPTY ELEMENT : Don't nest   (don't draw list items)
            ++i.cbUsed;
               
         } else {
               
            // ENTER ELEMENT
               
            ++ndxStyle;
            if (ndxStyle < ARRAYSIZE(etbl)) {
               pes = etbl+ndxStyle;
               ElemStyle_CtorNested(etbl+ndxStyle, etbl+ndxStyle-1, pc, me);
            }

            if (nElem == ELEM_LI) {
               // Draw list tag: ideally, we would know the height of the
               // first line before doing this, but that would seriously
               // complicate things.  We take the height of the current font.

               Dox_DrawListTag(me, pib, i.pos, pp, pes);
            }
         }
         
      } else if (pc[0] == DOX_EE) {

         // EE: EXIT ELEMENT

         if (ndxStyle > 0) {
            --ndxStyle;
            if (ndxStyle < ARRAYSIZE(etbl)-1) {
               // Bottom margin for exited block
               pes = etbl+ndxStyle;
            }
         }
      }
   }

   RELEASEIF(pib);
}


//
//----  UI Interface
//

// Find the corresponding focus item
//
// Returns index into me->ftbl[],  or -1 if no match.
//
// If there is no exact match, it finds the immediately previous entry (if
// any).  This should only happen on resize and when link text has be wrapped
// at different places.  In that case, the initial portion of the link will
// always occur at the same place, and any "orphaned" focus item positions
// will be >= the start of the link, so mathing the previous entry should
// always result in the same link being selected.
//
static DoxFocusPlot *Dox_FindFocus(Dox *me, int posFocusItem)
{
   DoxFocusPlot *pfp;
   int i;

   for (pfp = me->ftbl, i = 0; i < me->cntFocusTbl; i++, pfp++) {
      if (pfp->pos >= posFocusItem)
         return pfp;
   }

   return NULL;
}

// FindNextFocusItem: returns the next focus item (document position).  If
// current me->posFocus value is -1, returns first or last link.
//
// bForward = TRUE for forward; FALSE for backward
//
// Returns NULL if no more links in that direction.
//         
static DoxFocusPlot *Dox_FindNextFocusItem(Dox *me, int bForward)
{
   DoxFocusPlot *pfp;
   int ndx=0;

   pfp = Dox_FindFocus(me, me->posFocus);

   if(pfp){
      if(0==(pfp - me->ftbl) && me->posFocus < 0 ){
         return pfp;
      }
      ndx = (pfp - me->ftbl) + (bForward ? 1 : -1);
   } else {
      ndx = (bForward ? 0 : me->cntFocusTbl-1);
   }

   if (BETWEEN_LT(ndx, 0, me->cntFocusTbl)) {
      pfp =  me->ftbl + ndx;
   } else {
      pfp = NULL;
   }

   //if (pfp) {
   //   ndx = (pfp - me->ftbl) + (bForward ? 1 : -1);
   //   if(me->posFocus == -1){
   //      ndx=0;
   //   }
   //} else {
   //   ndx = (bForward ? 0 : me->cntFocusTbl-1);
   //   if(bForward!=1){
   //      me->posFocus=-1;
   //   }
   //}

   //if (BETWEEN_LT(ndx, 0, me->cntFocusTbl)) {
   //   pfp =  me->ftbl + ndx;
   //} else {
   //   pfp = NULL;
   //}

   return pfp;
}


// Find the 'A' element parent of the current focus item, or -1 if not found.
// 

#define Dox_GetFocusLink(me)     Dox_FindAncestor((me), (me)->posFocus, ELEM_A)


// Item is deemed visible when it fits completely in the viewed area, or when
//    it starts precisely at the top of the viewed area.
//
#define FOCUSPLOT_VISIBLE(pfp,top,btm)   ((pfp)->yTop >= (top) &&    \
                                          ((pfp)->yBtm <= (btm) ||   \
                                           ((pfp)->yTop == (top))))

static boolean Dox_IsFocusVisible(Dox *me)
{
   DoxFocusPlot *pfp = Dox_FindFocus(me, me->posFocus);
   return (pfp && FOCUSPLOT_VISIBLE(pfp, me->dsi.yTop, me->dsi.yTop + me->dsi.yRectHeight));
}
  
// adjust the scroll pos (me->dsi.yTop) to keep focus item visible
static void Dox_MakeFocusVisible(Dox *me)
{
   DoxFocusPlot *pfp = Dox_FindFocus(me, me->posFocus);
   if (pfp) {
      if (pfp->yTop < me->dsi.yTop)
         me->dsi.yTop = pfp->yTop;
      else if (pfp->yBtm > me->dsi.yTop + me->dsi.yRectHeight)
         me->dsi.yTop = MAX(0, pfp->yBtm - me->dsi.yRectHeight);
   }
}


// sets position and size (screen coordinates)
//
void xDox_SetRect(Dox *me, AEERect *prc)
{
   boolean bFocusVisible = 0;
   boolean bVisiblePrev = 0;
   (void)bVisiblePrev;

   if (prc) {

      // copy prc, constrain dx, then test
      AEERect rc = *prc;

      if (!AEERect_EQ(&rc, &me->rc)) {
         int fInval = DOX_INVALFOCUS;  // always redraw SB & document and check focus
   
         if (rc.dx != 0 && me->rc.dx != rc.dx)
            fInval |= DOX_INVALWRAP;                   // re-wrap only if width changed
   
         // check if focus item is visible *before*
         // the rectangle changes
         bFocusVisible = Dox_IsFocusVisible(me);

         bVisiblePrev = (me->rc.dx && me->rc.dy);
   
         me->rc = rc;
         me->dsi.yRectHeight = me->rc.dy;
   
         me->bVisible = (me->rc.dx && me->rc.dy);
   
         // adjust the scroll pos so that:
         //  a) there is no blank space at the bottom of the document
         //  b) the current focus item, if any, remains visible
         if (me->bVisible && (me->dsi.yDocHeight >= me->dsi.yRectHeight)) {
            // adjust the scroll pos for window height
            int maxy = me->dsi.yDocHeight - me->dsi.yRectHeight;
            if (me->dsi.yTop > maxy)
               me->dsi.yTop = maxy;
            // adjust the scroll pos to keep focus item visible
            if (bFocusVisible)
               Dox_MakeFocusVisible(me);
         }
   
         xDox_Invalidate(me, fInval);
      }
   }
//   DBGPRINTF("Dox_SetRect, dx=%d, dy=%d, dx1=%d, dy1=%d", me->rc.dx, me->rc.dy, 
//      prc->dx, prc->dy);
}


static void Dox_SetScrollPos(Dox *me, int32 yTop)
{
   if (yTop != me->dsi.yTop) {

      yTop = CONSTRAIN(yTop, 0, me->dsi.yDocHeight - me->dsi.yRectHeight);

      // must be done for scrolling text controls
      if (me->pifcActive) {
         IWIDGET_SetFocus((IWidget*)me->pifcActive, 0);
      }

      me->dsi.yTop = yTop;
	  // notify scroll
	  Dox_SendScrollNotify(me);
      (void)xDox_Invalidate(me, DOX_INVALFOCUS);   // check focus item too
   }
}


static void Dox_ScrollPosToTop(Dox *me, int pos)
{
   DoxPlacement *pdpMax  = me->ptbl + me->cntPlacements;
   DoxPlacement *pdp;

   // find nearest pos in DoxPlacement array
   for (pdp = me->ptbl; pdp < pdpMax; pdp++)
      if (pdp->pos >= pos)
         break;

   // if found...
   if (pdp < pdpMax) {
      Dox_SetScrollPos(me, pdp->y);
   }
}


static void Dox_ScrollToBottom(Dox *me)
{
   DoxPlacement *pdp = me->ptbl + (me->cntPlacements-1);
   Dox_SetScrollPos(me, pdp->y);
}


void xDox_SetScrollPos(Dox *me, int32 pos)
{
   if (pos < 0) {
      Dox_ScrollToBottom(me);
   } else {
      Dox_ScrollPosToTop(me, pos);
   }
}

int32 xDox_GetScrollPos(Dox *me)
{
   DoxPlacement *pdpMax  = me->ptbl + me->cntPlacements;
   DoxPlacement *pdp;

   // find nearest pos in DoxPlacement array
   for (pdp = me->ptbl; pdp < pdpMax; pdp++) {
      if (me->dsi.yTop >= pdp->y && me->dsi.yTop < pdp->yLineBtm) {
         return pdp->pos;
      }
   }

   return 0;
}

int xDox_GetScrollIncrement(Dox *me)
{
   return me->cyScrollInc;
}

void xDox_GetScrollInfo(Dox *me, DoxScrollInfo *pdsi)
{
   *pdsi = me->dsi;
}

#if 0
// This handles the Pen Events we care about. It doesn't care so much about stale moves
// This will not cause a redraw, but will ask for one if focus, selection
boolean Dox_HandlePens(Dox * me, AEEEvent evtCode, uint16 wParam, uint32 dwParam)
{
   uint8 cbHit = 0,
         cbFlags = 0;
   int16 wXPos = AEE_GET_X(dwParam), 
         wYPos = AEE_GET_Y(dwParam);
   int   nData = 0;

   if( (me->ptTracker.cbFlags & PTRCK_HIT_SCRL) || evtCode == EVT_PEN_DOWN ){
      boolean bRet = IHtmlViewer_HandlePenInSB(me->piView, evtCode, wParam, dwParam);

      if( evtCode == EVT_PEN_UP ){
         MEMSET((void *)&me->ptTracker, 0, sizeof(me->ptTracker));
      }
      if( evtCode != EVT_PEN_DOWN ){
         return bRet;
      }
      if( (bRet && evtCode == EVT_PEN_DOWN) ){
         me->ptTracker.cbFlags = PTRCK_HIT_SCRL;
         return bRet;
      }
   }
   switch( evtCode ){
   case EVT_PEN_DOWN:
      if( (cbHit = DoxMapPointToItem(me, wXPos, wYPos, &nData, &cbFlags)) != 0 ){
         Dox_SetFocusItem(me, nData);
         me->ptTracker.cbHit = cbHit;
         me->ptTracker.nData = nData;
         if( cbFlags & PTRCK_GEN_HITCTL ){
            ICtlObj * pco;
            me->ptTracker.cbFlags |= PTRCK_GEN_HITCTL;
            if( (pco = xDox_ObtainObject(me, nData)) != NULL ){
               return ICTLOBJ_HandleEvent(pco,evtCode,wParam,dwParam);
            }
         }else{
            me->ptTracker.cbFlags &= ~PTRCK_GEN_HITCTL;
            return TRUE;
         }
      }
      break;
   case EVT_PEN_UP:
      if( (cbHit = DoxMapPointToItem(me, wXPos, wYPos, &nData, &cbFlags)) != 0 ){
         if( me->ptTracker.nData == nData ){
            if( cbFlags & PTRCK_GEN_HITCTL ){
            }else{
               if (nData >= 0) {
                  int nJump = Dox_FindAncestor(me, nData, ELEM_A);
                  if( nJump >= 0 ){
                     return Dox_Jump(me,nJump);
                  }
               }
            }
         }
      }
      MEMSET((void *)&me->ptTracker, 0, sizeof(me->ptTracker));
      break;
   default:
      break;
   }
   return FALSE;
}

boolean Dox_GetPenTracking(Dox * me, HTMLPenTracker * ptTracker)
{
   boolean bRet = (boolean)(me->ptTracker.cbHit == 0 ? FALSE : TRUE);
   if( ptTracker ){
      MEMCPY((void *)ptTracker, (void *)&me->ptTracker, sizeof(HTMLPenTracker));
   }
   return bRet;
}
uint8 DoxMapPointToItem(Dox * me, int16 wXPos, int16 wYPos, int * pnData, uint8 * pcbFlags)
{
   const char *pcBuf = me->pcBuf;
   DoxPlacement *pp;
   DoxPlacement *ppMax;
   int   yScroll = me->dsi.yTop,
         yStop = yScroll + me->rc.dy; // yStop is in document-related coordinates

   // if a control is in full-screen mode, of course the pen had hit it
   if (me->pcoFull) {
      *pnData = 0;
      *pcbFlags |= PTRCK_GEN_HITCTL;
      return PTRCK_HIT_ITEM;
   }

   if( wYPos > (me->rc.y + yStop) ){
      return FALSE;
   }

   ppMax = me->ptbl + me->cntPlacements;

   for (pp = me->ptbl; pp < ppMax && pp->y < yStop; pp++) {

      if( pp->yLineBtm >= yScroll ){
         ICtlObj * pco;
         if (pp->yLineTop > yScroll + me->dsi.yRectHeight) {
            break;
         }
         // Handle objects
         if( (pco = xDox_ObtainObject(me, pp->pos)) != NULL ){
            AEERect rctObj;
            CtlExtent ce;

            rctObj.x = pp->x + me->rc.x;
            rctObj.y = pp->y + me->rc.y - yScroll;
            ICTLOBJ_GetExtent(pco, &ce);
            ICTLOBJ_GetType(pco);
            rctObj.dx = ce.nWidth;
            rctObj.dy = ce.nHeight;
            if( rctObj.x + rctObj.dx > (me->rc.x + me->rc.dx) ){
               rctObj.dx = me->rc.x + me->rc.dx;
            }
            if( rctObj.y + rctObj.dy > (me->rc.y + me->rc.dy) ){
               rctObj.dy = me->rc.y + me->rc.dy;
            }
            if( PT_IN_RECT(wXPos, wYPos, rctObj) ){
               DoxFocusPlot * pfp = Dox_FindFocus(me, pp->pos);

               // See if focus is hidden, or partially obscured                  
               if (pfp) {
                  // Found ... completely visible?
                  if (FOCUSPLOT_VISIBLE(pfp, yScroll, yStop)) {
                     // A clickable link takes priority over the control
                     *pnData = pp->pos;
                     if( Dox_FindAncestor(me, pp->pos, ELEM_A) >= 0 ){
                        *pcbFlags &= ~PTRCK_GEN_HITCTL;
                     }else{
                        *pcbFlags |= PTRCK_GEN_HITCTL;
                     }
                     return PTRCK_HIT_ITEM;
                  }
               }else{ // Not really possible
                  break;
               }
            }
         }
         // Handling Links as they are responsive to pen, but not an ICTLOBJ
         if( Dox_FindAncestor(me, pp->pos, ELEM_A) >= 0 ){
            AEERect        rctLink;

            // Prepare comparison bounds
            rctLink.y = pp->y + me->rc.y - yScroll;
            rctLink.dy = pp->yLineBtm - pp->yLineTop;
            if( rctLink.y + rctLink.dy > (me->rc.y + me->rc.dy) ){
               rctLink.dy = me->rc.y + me->rc.dy;
            }
            // Only measure X bounds if the Y area was hit by the pen
            if( wYPos >= rctLink.y && wYPos <= (rctLink.y + rctLink.dy) ){
               int            nWidth, cbFits;
               AEEFont        fnt;
               uint8          fntFlag  = 0;
               const char *   pcTop    = NULL, 
                          *   pcAbsTop = (pcBuf + pp->pos);

               // Construct ElemStyle::flags to get font, Highest parent wins basically.
               // This does what the layout formatting does pretty much.
               while( (pcAbsTop = DoxParent(pcAbsTop, pcBuf)) != DoxBufParent(me) ){
                  int nElem;
                  pcTop = pcAbsTop;
                  nElem = DoxGetID(pcTop[1]);
                  if( nElem < ELEM_MAX ){
                     fntFlag |= acmHtmlElems[nElem].flags;
                  }
               }
               fnt = ESF_Font(fntFlag);
               
               nWidth = DoxMeasureTextA(me->piDisplay, ESF_IFont(pes->flags), pcBuf + pp->pos, pp->cb, me->dl.nDocWidth - pp->x, &cbFits, me->dc.pfnDecode);
               rctLink.x = pp->x + me->rc.x;
               rctLink.dx = rctLink.x + nWidth;
               if( rctLink.x + rctLink.dx > (me->rc.x + me->rc.dx) ){
                  rctLink.dx = me->rc.x + me->rc.dx;
               }
               if( wXPos >= rctLink.x && wXPos <= (rctLink.x + rctLink.dx) ){
                  DoxFocusPlot * pfp = Dox_FindFocus(me, pp->pos);

                  // See if focus is hidden, or partially obscured                  
                  if (pfp) {
                     // Found ... completely visible?
                     if (FOCUSPLOT_VISIBLE(pfp, yScroll, yStop)) {
                        *pnData = pp->pos;
                        *pcbFlags &= ~PTRCK_GEN_HITCTL;
                        return PTRCK_HIT_ITEM;
                     }
                  }else{ // Not really possible
                     break;
                  }
               }
            }
         }
      }
   }
   return 0;
}

#endif

// Update: perform a deferred action (i.e. draw invalidated text & controls;
// activate focus control.
//
// Update only performs one such action, and returns a bit mask of other
// actions remaining to be performed.  The caller is responsible for 
// calling Dox_Update() again (whenever) until it returns 0.
//
// There is a strict priority to handling of update flags (INVALWRAP,
// etc.).  To handle the cases where handling of one update flag SETS
// another update flag, we do not simply test for one after another in a
// sequence of if's.  After each one is handled, we must check that the
// higher-priority flags (or that flag itself) have not been re-asserted.
//
int xDox_Update(Dox *me)
{
   int fFirst = me->fInvalid;

   fFirst = (fFirst & ~(fFirst-1));   // get first non-zero bit (or 0 if me->fInvalid == 0)

   me->fInvalid -= fFirst;
   me->bUpdating = TRUE;              // suppress unnecessary callback scheduling

   // The order of the cases has nothing to do with priority

   switch (fFirst) {

   case DOX_INVALWRAP:
      xDox_Layout(me);
      break;

   case DOX_INVALFOCUS:
      Dox_CheckFocus(me);
      break;
   }

   me->bUpdating = FALSE;
   return me->fInvalid;
}


// returns the next scroll position (to allow scrolling by whole line increments)
// If at the "end", the input value is returned.
static int Dox_GetNextScrollPos(Dox *me, int yTop, int bNext)
{
   yTop += (bNext ? me->cyScrollInc : -me->cyScrollInc);
   return DoxConstrain(yTop, 0, me->dsi.yDocHeight - me->dsi.yRectHeight);
}


// Updates me->posFocus, me->posFocusActive, me->pifcActive.
//
// Driven by DOX_INVALFOCUS
//
// Affected by:
//   - posFOcus
//   - bActive
//   - Document layout & visible rectangle
//
// Description:
//
//    If posFocus is -1 or not visible, a visible link will be selected (if
//    available).
//
//    posFocusActive will be the same as posFocus, as long as is is visible
//    and bActive is TRUE.
//
//    When posFocusActive refers to an object, pifcActive will point to that
//    object.
//
static void Dox_CheckFocus(Dox *me)
{
   DoxFocusPlot *pfp;
   int yTop = me->dsi.yTop;
   int yBtm = yTop + me->rc.dy;
   int posFocus = me->posFocus;
   int posFocusActive;

   int bFocusVisible = FALSE;


   // See if focus is hidden

   if (posFocus >= 0) {

      pfp = Dox_FindFocus(me, posFocus);

      if (pfp) {
         // Found ... visible?
         if (FOCUSPLOT_VISIBLE(pfp, yTop, yBtm)) {
            // At least partially visible
            bFocusVisible = TRUE;
         }
      } else {
           // not found (shouldn't happen)
         posFocus = -1;
      }
   }
   
   // If no visible focus, look for one
   
   if (!bFocusVisible) {
      int nn;
      for (nn = me->cntFocusTbl, pfp = me->ftbl; nn; --nn, ++pfp) {
         if (FOCUSPLOT_VISIBLE(pfp, yTop, yBtm)) {
            posFocus = pfp->pos;
            bFocusVisible = TRUE;
			break;
         }
      }
   }

   me->posFocus = posFocus;
   
   // Update posFocusActive

   posFocusActive = -1;

   if (me->bActive && bFocusVisible)
      posFocusActive = posFocus;

   // Update pifcActive
   // tell current control it is no longer active
   if (me->pifcActive) {
      IWIDGET_SetFocus((IWidget*)me->pifcActive, 0);
      me->pifcActive = 0;
   }
   
   if (posFocusActive >= 0) {
      me->pifcActive = xDox_ObtainObject(me, posFocusActive);
      if (me->pifcActive) {
         IWIDGET_SetFocus((IWidget*)me->pifcActive, 1);
      }
   }

   if (posFocusActive != me->posFocusActive) {

      int nType = HVFOCUS_NONE;
      int posElem = posFocusActive;
      int nState = 0;
      IValueModel *pivm = NULL;

      me->posFocusActive = posFocusActive;
      
      // Invalidate screen

      //Dox_Invalidate(me, 0);
      
      // Notify owner
   
      if (me->pifcActive) {
         
         // Object
         int nCtlType;
         IFORMCONTROL_GetType(me->pifcActive, &nCtlType);
         nType =  (nCtlType == FCT_TEXT)     ? HVFOCUS_EDIT : 
                  (nCtlType == FCT_CHECKBOX) ? HVFOCUS_CHECKBOX : 
                  (nCtlType == FCT_RADIO)    ? HVFOCUS_RADIO : 
                  (nCtlType == FCT_SUBMIT)   ? HVFOCUS_SUBMIT : 
                  (nCtlType == FCT_IMAGE)    ? HVFOCUS_SUBMIT : 
                  (nCtlType == FCT_RESET)    ? HVFOCUS_RESET : 
                  (nCtlType == FCT_SELECT)   ? HVFOCUS_LIST : HVFOCUS_NONE;

         // check for state 
         if (HVFOCUS_RADIO == nType || HVFOCUS_CHECKBOX == nType){
            if (SUCCESS == IFORMCONTROL_GetModel( me->pifcActive, AEEIID_VALUEMODEL, (IModel**)&pivm ) ) {
               nState = (IVALUEMODEL_GetBool( pivm )) ? 1 : 0;
               IVALUEMODEL_Release(pivm);
            }
         }

      } else if (posFocusActive >= 0) {
         // Link
         posElem = Dox_GetFocusLink(me);
         nType = HVFOCUS_LINK;
      }
      
      IHTMLVIEWMODEL_NotifyFocus(me->piModel, posElem, nType, nState);
   }
}

// Multi line link hop helper
typedef struct _aelem
{
   int yyTop;
   int yyBtm;
   int nPosTop;
   int nPosBtm;
   int nPosA;
   int nRefPos;
   boolean bFound;
}AElem;
static void Dox_GetAElem(Dox *me, AElem *paelem)
{
   boolean bFound = FALSE;
   DoxPlacement *tpdp = NULL;
   const char* pcNow = me->pcBuf + paelem->nRefPos;
   while(pcNow >= me->pcBuf && FALSE==bFound){
      if(DOX_SE == pcNow[0]){
         if(ELEM_A == DoxGetID(pcNow[1])) {
            int nCountText = 0;
            int nA = Dox_GetElemText(me, pcNow-me->pcBuf, NULL, 0)-1;

            paelem->nPosA = pcNow-me->pcBuf;

            tpdp = me->ptbl;
            while(tpdp < me->ptbl+me->cntPlacements) {
               if(tpdp->pos >= (pcNow - me->pcBuf)){
                  bFound=TRUE;
                  
                  // get top of the line
                  paelem->nPosTop = tpdp->pos;
                  paelem->yyTop = tpdp->yLineTop;

                  // get bottom of the line
                  if(0==nCountText && nA == tpdp->cb){
                     // one liner
                     nCountText += tpdp->cb;
                     paelem->nPosBtm = tpdp->pos;
                     paelem->yyBtm = tpdp->yLineBtm;
                  }else{
                     while(tpdp < me->ptbl+me->cntPlacements && nCountText<=nA){
                        nCountText += tpdp->cb;
                        if(nCountText==nA){
                           break;
                        }
                        if(nCountText>nA){
                           tpdp--;
                           break;
                        }
                        tpdp++;
                     }

                     paelem->nPosBtm = tpdp->pos;
                     paelem->yyBtm = tpdp->yLineBtm;

                     break;
                  }
               }
               tpdp++;
            }
         }else{
            bFound = FALSE;
            break;
         }
      }
      pcNow--;
   }

   paelem->bFound = bFound;
}

// Advance to next focus item IF it is visible
//
// Return TRUE if next focus item was selected.
//
static boolean Dox_NextFocus(Dox *me, boolean bFwd)
{
   AElem ainfo;
   boolean bScrolled=FALSE;
   DoxFocusPlot *pfp=NULL;
   uint16 yyTop, yyBtm;
   boolean byyTop, byyBtm;
   yyTop=yyBtm=0;
   byyTop = byyBtm = FALSE;

   pfp = Dox_FindNextFocusItem(me, bFwd);

   if(pfp && FALSE==me->bScrollMLLink){
      ainfo.nRefPos = pfp->pos;
      Dox_GetAElem(me, &ainfo);
      // if it is a link
      if(TRUE == ainfo.bFound){
         if( me->posFocus == ainfo.nPosTop || 
            me->posFocus == ainfo.nPosBtm || 
            (me->posFocus < ainfo.nPosBtm && me->posFocus > ainfo.nPosTop)
            )
         {
            // we are still on the same link
            // requery to get next link
            if(TRUE == bFwd){
               me->posFocus = ainfo.nPosBtm;
            }else{
               me->posFocus = ainfo.nPosTop;
            }

            pfp = Dox_FindNextFocusItem(me, bFwd);
            if(NULL != pfp){
               ainfo.nRefPos = pfp->pos;
               Dox_GetAElem(me, &ainfo);
            }
         }
         if(NULL != pfp)
         {
            byyBtm = byyTop = TRUE;
            yyTop = (uint16)ainfo.yyTop;
            yyBtm = (uint16)ainfo.yyBtm;
         }
      }
   }



   //compensate for scrolling because of touch enabling
   if (pfp && FALSE==me->bScrollMLLink && TRUE==byyTop && TRUE==byyBtm && yyBtm > yyTop){

      // view is up, see if we can bring him down
      if(yyTop >= (me->dsi.yTop+me->dsi.yRectHeight) || yyBtm >= (me->dsi.yTop+me->dsi.yRectHeight))
      {
         if( (yyTop - (me->dsi.yTop+me->dsi.yRectHeight)) <= 0)
         {
	         me->dsi.yTop = yyBtm - me->dsi.yRectHeight;
	         bScrolled = TRUE;
         }
      }
      // view is down, see if we can bring him up
      else if (me->dsi.yTop > yyTop){         
         if( me->dsi.yTop <= yyBtm && me->dsi.yRectHeight > (yyBtm - yyTop))
		   {
            me->dsi.yTop = yyTop;
			   bScrolled = TRUE;
         }         
      }

	   if(TRUE==bScrolled){
		   Dox_SendScrollNotify(me);
	   }
   }


   if (pfp &&
       pfp->yTop >= me->dsi.yTop &&
       pfp->yBtm <= me->dsi.yTop + me->dsi.yRectHeight) 
   {
      // fits completely on screen
      Dox_SetFocusItem(me, pfp->pos);

      return TRUE;
   }
   
   return FALSE;

}


boolean xDox_Move(Dox *me, boolean bFwd)
{
   boolean bFocusMoved = Dox_NextFocus(me, bFwd);
   boolean bScrolled = 0;

   if (!bFocusMoved) {
      int y = Dox_GetNextScrollPos(me, me->dsi.yTop, bFwd);
      bScrolled = (y != me->dsi.yTop);
      Dox_SetScrollPos(me, y);
      bFocusMoved = Dox_NextFocus(me, bFwd);
      me->fAccelScroll = 1;
      if (bScrolled) {
         Dox_SendScrollNotify(me);
      }
   }

   return (bFocusMoved || bScrolled);  // we handled this if we moved or scrolled
}


static __inline boolean STRBEGINSI(const char *psz, const char *pszBegins) {
   return (0 == STRNICMP(psz, pszBegins, STRLEN(pszBegins)));
}


static boolean Dox_Jump(Dox *me, int posLink)
{
   char aBuffer[50];
   char *pBuffer = aBuffer;
   
   const char *pszHRef = xDoxGetAttrString(me->pcBuf+posLink, ATTR_HREF, 0);

   int nLen = xDox_MakeUrl(me, posLink, pszHRef, 0, 0);

   if (nLen > sizeof(aBuffer)) {
      pBuffer = MALLOC(nLen);
   }

   if (!pBuffer) {
      return FALSE;
   }

   xDox_MakeUrl(me, posLink, pszHRef, pBuffer, nLen);

   if (STRBEGINSI(pBuffer, "x-submit:")) {
      Dox_SubmitForm(me, posLink);
   } else {
      IHTMLVIEWMODEL_NotifyJump(me->piModel, posLink, pBuffer);
   }
   
   if (pBuffer != aBuffer) {
      FREE(pBuffer);
   }

   return TRUE;
}

static int Dox_GetAccessKeyLink(Dox *me, char key)
{
   DoxIter di;

   DoxIter_Ctor(&di, me, -1);

   while (DoxIter_Find(&di,ELEM_A)) {
      const char *cpsz = xDoxGetAttrString(di.doxitem.pc,ATTR_ACCESSKEY,"");

      if (*cpsz == key) {
         return di.doxitem.pos;
      }
   }

   return -1;

}

static int Dox_GetAccessKeyInput(Dox *me, char key)
{
   DoxIter di;

   DoxIter_Ctor(&di, me, -1);

   while (DoxIter_Find(&di,ELEM_INPUT)) {
      const char *cpsz = xDoxGetAttrString(di.doxitem.pc,ATTR_ACCESSKEY,"");

      if (*cpsz == key) {
         return di.doxitem.pos;
      }
   }

   return -1;
}


void xDox_SetFocusPos(Dox *me, int pos)
{
   Dox_SetFocusItem(me, pos);
}


int xDox_GetFocusPos(Dox *me)
{
   return me->posFocus;
}


boolean xDox_HandleEvent(Dox *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   if (!me->bActive || !me->bVisible) {
      return FALSE;
   }
   
    // CONTROL
   if (me->pifcActive &&
      IFORMCONTROL_HandleEvent(me->pifcActive, evt, wParam, dwParam))
      return TRUE;

#if 0
   if( evt >= EVT_PEN_DOWN && evt <= EVT_PEN_STALE_MOVE ){
      if( Dox_HandlePens(me, evt, wParam, dwParam) ){
         return TRUE;
      }
      // Below here not handling STALE_MOVE as precision is not necessary
      if( evt == EVT_PEN_STALE_MOVE ){
         return FALSE;
      }
   }
#endif

   if (evt == EVT_KEY) {
      int posLink = -1;
      
      if (wParam >= AVK_0 && wParam <= AVK_9) {

         posLink = Dox_GetAccessKeyLink(me,(char)('0'+wParam-AVK_0));

         if (posLink >= 0) {

            Dox_SetFocusItem(me, posLink);
            Dox_MakeFocusVisible(me);

            if (!xDoxGetAttrString(me->pcBuf+posLink, ATTR_HREF, 0))
               return TRUE;

         } else {

            posLink = Dox_GetAccessKeyInput(me,(char)('0'+wParam-AVK_0));

            if (posLink >= 0) {

               Dox_SetFocusItem(me, posLink);
               Dox_MakeFocusVisible(me);
               return TRUE;
            }
         }

      } else if (wParam == AVK_SELECT) {

         posLink = Dox_GetFocusLink(me);

         if (posLink >= 0 && !xDoxGetAttrString(me->pcBuf+posLink, ATTR_HREF, 0))
            return TRUE;

      } else {

         posLink = -1;

      }

      if (posLink >= 0) {
         return Dox_Jump(me,posLink);
      }
   }

   return FALSE;
}


int xDox_Invalidate(Dox *me, int fInval)
{
   if (fInval & ~me->fInvalid) {
      me->fInvalid |= fInval;
      if (!me->bUpdating && me->pfCB) {
         me->pfCB(me->pvCB);
      }
   }
   return me->fInvalid;
}


void xDox_SetInvalCB(Dox *me, DoxInvalFn pfn, void *pvData)
{
   me->pfCB = pfn;
   me->pvCB = pvData;

   if (me->fInvalid && pfn) {
//      BRIDLE_ASSERT_SAFE_PFN(pfn);
      pfn(pvData);
   }
}


uint32 Dox_GetFlags(Dox *me)
{
   return me->dwFlags;
}


void Dox_SetFlags(Dox *me, uint32 dwFlags)
{   
   me->dwFlags = dwFlags;
}

int xDox_SetUrl(Dox *me, const char *pszUrl)
{
   FREE(me->pszUrl);
   
   if ((char *)0 == pszUrl) {
      return SUCCESS;
   }
   
   me->pszUrl = STRDUP(pszUrl);

   if ((char *)0 == pszUrl) {
      return ENOMEMORY;
   }

   return SUCCESS;
}


void xDox_GetIWeb(Dox *me, IWeb ** ppIWeb)
{

   if (!me->pIWeb) {
      // create a default IWeb for Dox internal use
      ISHELL_CreateInstance(me->piShell, AEECLSID_WEB, (void**)&me->pIWeb);
   }

   *ppIWeb = me->pIWeb;

   if (me->pIWeb) {
      IWEB_AddRef(*ppIWeb);
   }
}

void xDox_SetIWeb(Dox *me, IWeb * pIWeb)
{
   if (pIWeb) {
      IWEB_AddRef(pIWeb);        // addref the new one
   }
   
   if (me->pIWeb) {
      IWEB_Release(me->pIWeb);   // release the old one
   }

   me->pIWeb = pIWeb;    // set the new IWeb
}



   
// Dox encode function
// Note: Decode neither zero-terminates nor counts the null when 
// returning decoded string length 
int Dox_Encode(Dox *me, char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
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
         cch += CharSet_Encode(&me->cs, buf, sizeof(buf), pwc, cwSrc, &used);
         pwc += used;
         cwSrc -= used;
         nUsed += used;
      }

   } else {
      cch = CharSet_Encode(&me->cs, pcDest, cbDest, pwcSrc, cwSrc, &nUsed);
   }

   if (pnUsed) {
      *pnUsed = nUsed;
   }

   return cch;
}



// Dox decode function
// Note: Decode neither zero-terminates nor counts the null when 
// returning decoded string length 
int Dox_Decode(Dox *me, AECHAR *pwDest, int cwDest, const char *pcSrc, int cbSrc, int *pnUsed)
{
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
         cch += CharSet_Decode(&me->cs, wBuf, sizeof(wBuf), p, cbSrc, &used);
         pcSrc += used;
         cbSrc -= used;
         nUsed += used;
      }

   } else {
      cch = CharSet_Decode(&me->cs, pwDest, cwDest, pcSrc, cbSrc, &nUsed);
   }

   if (pnUsed) {
      *pnUsed = nUsed;
   }

   return cch;

}


int Dox_DecodeTerminate(Dox *me, AECHAR *pwDest, int cwDest, const char *pcSrc, int cbSrc, int *pnUsed)
{
   int cch;

   if (cwDest) {
      cwDest--;   // allow for zero-terminator unless just counting
   }

   cch = Dox_Decode(me, pwDest, cwDest, pcSrc, cbSrc, pnUsed);
   if (pwDest) {
      pwDest[cch] = (AECHAR)0;
   }
   return ++cch;    // return number of AECHARs written/counted incl. zero-term
}



int Dox_EncodeTerminate(Dox *me, char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed)
{
   int cch;

   if (cbDest) {
      cbDest--;   // allow for zero-terminator unless just counting
   }

   cch = Dox_Encode(me, pcDest, cbDest, pwcSrc, cwSrc, pnUsed);
   if (pcDest) {
      pcDest[cch] = (char)0;
   } else {
      cch++;      // just counting, count zero-terminator also
   }
   return cch;    // return number of AECHARs written/counted
}





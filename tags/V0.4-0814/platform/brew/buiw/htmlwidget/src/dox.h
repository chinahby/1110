/*===================================================================
FILE: dox.h

SERVICES:  Structured Document Object, supporting XML/HTML

DESCRIPTION:

PUBLIC CLASSES AND FUNCTIONS:

        Copyright 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===================================================================*/
#ifndef DOX_H
#define DOX_H

typedef struct Dox Dox;

#include "AEEWeb.h" // for IWebUtil 
#include "AEEHtmlViewModel.h" 
#include "AEEContainer.h"
#include "FormControl.h"
#include "charset.h"
#include "wutil.h"

//#define DOX_UNITTEST

// Form state encode char
#define FS_SOH            1
#define FS_STX            2
#define FS_ETX            3
#define FS_NONE           '0'
#define FS_CHECKED        '1'
#define FS_UNCHECKED      '2'
#define FS_ELEMSTATECOUNT 4
#define FSINDX_NAME       0
#define FSINDX_VALUE      1
#define FSINDX_STATE      2
#define FSINDX_TYPE       3

boolean xDoxStrEQI(const char *s1, const char *s2);
int     DoxGetInt(const char *psz, int nDefault);
PCSTR   HexAToUint32(PCSTR pszIn, uint32 *pValOut);


// GetAttrValue = get 'raw' attribute value (NULL, ptr to DOX_NV, or psz)
//
const char *xDoxGetAttrValue(const char *pszElem, int nAttrID);

// GetAttrInt = get int specified for attribute, or return default
//
int         DoxGetAttrInt(const char *pch, int nAttrID, int nDefault);

// GetAttrString = get string specified for attribute, or return default
//                 value if attribute missing or if it has no value
//
const char *xDoxGetAttrString(const char *pch, int nAttrID, const char *pszDefault);

// GetAttrColor = get an RGBVAL specified for an attribute
//
RGBVAL      DoxGetAttrColor(const char *pch, int nAttrID, RGBVAL rgbDefault);


const char *xDoxNextItem(const char *pchItem, const char *pchMax);
const char *xDoxPrevItem(const char *pch);

   
#ifndef SETZERO
# define SETZERO(ptr)  (MEMSET((ptr), 0, sizeof(*(ptr))))
#endif


#ifndef ARRAYSIZE
# define ARRAYSIZE(a)   (sizeof(a) / sizeof(a[0]))
#endif


#ifndef MALLOCREC
# define MALLOCREC(type)   ( (type *) MALLOC(sizeof(type)) )
#endif


#ifndef SETAEERECT
# define SETAEERECT(prc,l,t,w,h)   (prc)->x=(int16)(l),(prc)->y=(int16)(t),(prc)->dx=(int16)(w),(prc)->dy=(int16)(h)
#endif


#define BETWEEN_LT(val, nMinGE, nMaxLT)   \
        ((unsigned) ((val) - (nMinGE)) < (unsigned) ((nMaxLT) - (nMinGE)))

#define BETWEEN_LE(val, nMinGE, nMaxLT)   \
        ((unsigned) ((val) - (nMinGE)) <= (unsigned) ((nMaxLT) - (nMinGE)))


#define AEERect_EQ(pr1, pr2)  ((pr1)->x == (pr2)->x && (pr1)->dx == (pr2)->dx && \
                               (pr1)->y == (pr2)->y && (pr1)->dy == (pr2)->dy)



typedef struct {
   IFont *  piFont;
   int      a;       // ascent
   int      d;       // descent
   int      hgt;     // height (a+d)
   int      spcwid;  // width of space char
} FontMetric;

extern FontMetric gfntMetrics[4];

//extern int gnFontDescent;   // Descent of normal font



//
//----  DoxInvalRange : Tracks changes to the buffer
//

// An InvalRange summarizes changes to the buffer since the InvalRange was
// last reset.  Typically, an InvalRange will be reset when some derived data
// structure is validated, and later the InvalRange will be examined to update
// the derived data efficiently.  Only one InvalRange is updated by
// BufReplace.  If multiple derived data structures are to be maintained
// independently, they could each be assigned their own InvalRanges, and the
// 'master' one could be "added to" all of them whenever one will be examined
// and reset.
//
// InvalRange's assertion of the condition of the document since the last time
// the structure was reset could be summaried by the following:
//
//   "The first posA bytes are the same, and all bytes following posB are the
//   same as what WAS at (posB-nIns)."

typedef struct DoxInvalRange {
   int posA;           // bytes up to posA (non-inclusive) have not changed
   int posB;           // bytes after posB have not changed
   int nIns;           // number of bytes that have been inserted
                       // (negative = deleted) between posA and posB
} DoxInvalRange;



//
//----  Dox
//

// Dox object holds the structured document with all its attributes, plus
// layout information and other transient data used to optimize access to
// and manipulation of the document.
//

#define MAXOBJTBL       40
#define MAXFOCUSTBL    100
#define MAXELEMNEST     30
#define MAXPLACEMENTS  400
#define MAXLINEITEMS    20     // max. number of items per line
#define MAXFIELDDATA   100     // max. number of bytes per form control




typedef struct DoxObject DoxObject;


//#if 0
//typedef struct {
//   int         nID;     // pos+1
//   ICtlObj *   pco;     // Created form control
//} DoxObjectTbl;
//#else
//
//typedef struct {
//   int            nID;        // pos+1
//   IFormControl*  pifc;       // form control (widget)
//   boolean        bRedraw;    // invalidated - needs redrawing
//   int            x;          // x position for invalidate/redraw
//   int            y;          // y position for invalidate/redraw
//} DoxObjectTbl;
//
//#endif

typedef struct {
   int yTop;
   int yRectHeight;
   int yDocHeight;
} DoxScrollInfo;


typedef struct {
   uint16 pos;
   uint16 yTop;
   uint16 yBtm;
} DoxFocusPlot;


typedef struct {
   int32  xIndent;    // basic indentation
   int32  ySpace;     // default paragraph spacing
   RGBVAL rgbLink;
   RGBVAL rgbText;
   RGBVAL rgbBackground;
} DoxConfig;


typedef void (*DoxInvalFn)(void *pvCxt);

typedef struct {
   int16   pos;
   int16   cb;
   int16   y;
   int16   yLineTop;
   int     yLineBtm:16; // co-exist with following fields (no short bit fields in SDT/ADS)
   int     x:12;
   int     bFocus:1;    // focus-able item
} DoxPlacement;


// ElemStyle
//
// This stores the values pertaining to a particular element that are
// dynamically generated during wrapping and/or that may propagate to child
// elements.
// 
typedef struct {
   // text style
   RGBVAL  rgb;         // inherited
   uint8   flags;       // inherited
   uint8   nSize;       // inherited

   // block attributes
   int16   xLft;        // left margin (current block)
   int16   xRgt;        // right margin (current block)
   int16   ySpace;      // top & bottom margins;  -1 => inline  (this elem)
} ElemStyle;


typedef struct {
   int16     cbClip;      // number of character that fit
   int16     cbBrk;       // position after last break character (0 => no word break)
   int16     cbWord;      // end of last word (0 => no break, or no word preceding the break)
   int16     widClip;     // width of cbClip characters
   int16     widBrk;      // width of cbBrk characters
   int16     widWord;     // width of cbWord characters

   int16     nAscent;     // ascent
   int16     nDescent;    // descent
} DoxSize;


typedef struct {
   int     pos:16;
   int     bText:1;   // TRUE => search within for break pos;  FALSE => object (break before or after)
   int     bFocus:1;  // TRUE => focus item
   DoxSize ds;
} DoxLineItem;


typedef struct {
   DoxLineItem dli[MAXLINEITEMS];
   int         ndxItem;
   int         yNext;
   int         yLineBtm;
   int         widMeasured;
   int         nDocWidth;
   Dox *       pdox;
   ElemStyle * pes;
} DoxLine;


typedef struct ImgNode ImgNode;
struct ImgNode {
   ImgNode *   pNext;
   int16       id;
   IImage *    pii;
};


struct Dox {

   AEEVTBL(IContainer)* pvt;     // vtable pointer
   AEEVTBL(IContainer)  vt;      // vtable

   IWidget *      piw;           // the HtmlWidget (used for invalidations)

   IShell *       piShell;
   IWebUtil *     piWebUtil;
   IDisplay *     piDisplay;

   IHtmlViewModel * piModel;     // for notifications

   IWeb *         pIWeb;

   ImgNode *      pImages;       // IImage cache (used by form controls)

   // buffer members
   int            fErrors;         // bitmask
   char *         pcBuf;
   int            cbBuf;          // size of cbBuf
   int            cbData;         // number of bytes used in pcBuf[]

   // drawing/layout
   
   boolean        bActive;
   boolean        bUpdating;
   DoxConfig      cfg;
   DoxScrollInfo  dsi;            // scroll info
   boolean        bScrollMLLink;  // multi line link scroll
   AEERect        rc;             // rectangle we draw into
   int            posFocus;       // focus item (placeholder)
   int            posFocusActive; // active focus item (depends on visibility and bActive)

   IFormControl*  pifcActive;
   IFormControl*  pcoFull;

   int8           fInvalid;
   int8           bVisible;       // reflects rc size
   DoxInvalFn     pfCB;
   void *         pvCB;           // callback context pointer

   int            cntFocusTbl;    // maximum index
   int            cntPlacements;

   int            cyScrollInc;      // current scroll increment
   int            cyRawScrollInc;   // to acheive slower ramping

   DoxPlacement   ptbl[MAXPLACEMENTS];
   DoxFocusPlot   ftbl[MAXFOCUSTBL];

   // Inline objects
//   DoxObjectTbl   otbl[MAXOBJTBL];
   DoxObjMgr      doxObjMgr;     // inline DoxObjMgr object

   DoxLine        dl;

   uint32         dwFlags;

   char *         pszUrl;

   void *         pImgList;      // (opaque) head of list for reusable objects
   int            nRefImages;    // refcount for form objects (images) (++ when loading, -- when done)
   int            nFirstPage;    // count of number of objects (images) in first visible page

//   HTMLPenTracker ptTracker;
   
   CharSet        cs;

   FontMetric     fntMetrics[4];       // used to be gfntMetrics

   char           szName[32];          // used for retval of DoxGetName

   AECHAR *       pwTemp;              // temp buffer (used for char conversions)
   int            cchTemp;             // size of temp buffer (in AECHAR)

   flg            fAccelScroll : 1;    // accelerated scroll mode
   flg            fLayoutDone : 1;     // Dox_Layout has been performed
   flg            fSendContentDone : 1;   // HVN_CONTENTDONE & HVN_PAGEDONE need to be sent
};


// Error codes are defined as bits in a mask.  Error conditions can be reset;
// anything that resets errors should make this clear to its caller.
//

#define DOX_ERROR_TRUNC        1       // Insertion failed

#define DOX_ERROR_ALL          0xFF    // all errors


int Dox_BufReplace(Dox *me, int pos, int cbDel, const char *pcIns, int cbIns);

//char * Dox_BufGetPtrLen(Dox *me, int pos, int *pcbLen);


#define DOX_DEFAULTSIZE       8192

// size of temp buffer (in AECHARs) used for char conversions
#define DOX_TEMPBUFFERSIZE    128

//
//----  Internal Structure Representation
//


#define DOX_SE      '\001'      // start element
#define DOX_CD      '\003'      // content delimiter: terminates attribute list
#define DOX_EE      '\002'      // end element
#define DOX_NV      '\006'      // follows attr ID when no value was specified


#define DOX_EMPTYBUF  "\001\002"   // pcBuf[0] == EE,  pcBuf[-1] == SE


// Get character representing an element or attribute ID
//
#define DoxIDChar(nID)     ((nID) + 32)

// Get ID from character
//
#define DoxGetID(ch)        ( (unsigned char) ((ch) - 32) )
#define IDFromChar(ch)      DoxGetID(ch)
#define CharFromID(nID)     ((nID) + 32)



// CHARISIN() =>  ch >= cBtm && ch <= cTop
//
#define CHARISIN(ch, cBtm, cTop)   \
   ( (unsigned char) ((ch) - (cBtm)) <= ((unsigned char) (cTop) - (cBtm)) )

   
// DoxIsDelim => ch is one of { DOX_SE | DOX_CD | DOX_EE }
//
#define DoxIsDelim(ch)        CHARISIN(ch, DOX_SE, DOX_CD)


// DoxIsElemDelim => ch is one of { DOX_SE | DOX_EE }
//
#define DoxIsElemDelim(ch)    CHARISIN(ch, DOX_SE, DOX_EE)


// DoxIsAttrDelim => ch is one of { DOX_EE | DOX_CD }  [terminates attr list]
//
#define DoxIsAttrDelim(ch)    CHARISIN(ch, DOX_EE, DOX_CD)



//
// Simplified Syntax:
//
//    content  := (data | element)*
//    element  := SE eid attr* [CD content] EE
//    attr     := aid aval
//    aval     := sz | NV
//
//
// Definitions:
//
//    eid = element ID (one byte value, legal values starting at 32)
//    aid = attribute ID (one byte value, legal values starting at 32)
//    sz = zero-terminated string; cannot begin with NV
//    data = text; cannot include any EE or SE characters.
//
//
//
// Example:
//
//   HTML:       "<font size=2>this is a test</font>"
//   Internal:   SE ELEM_FONT ATTR_SIZE "3" 0 CD "this is a test" EE
//


const char *Dox_GetAttrName(Dox *me, int nID);
const char *Dox_GetElemName(Dox *me, int nID);


//---- Buffer access/manipulation functions


int         xDox_BufGetSize(Dox *me);



//----  Misc. Dox functions

#define ELEMSTYLE_SIZEDFLT      3


#define ElemStyle_IsBlock(pes)   ((pes)->ySpace >= 0)


typedef void (*DoxPlaceFn)(Dox *me, DoxPlacement *pdp, const char *pc, IFormControl *pifc, ElemStyle *pes);


int Dox_Construct(Dox *me, IShell *piShell, IHtmlViewModel *piModel, IWidget *piHtmlWidget);
void xDox_Dtor(Dox *me);

AECHAR *Dox_GetTempBuffer(Dox *me, int *pnSize);

void xDox_ErrorSet(Dox *me, int fError);
//void Dox_ErrorClear(Dox *me, int fError);

void xDox_Layout(Dox *me);

void xDox_Reset(Dox *me, boolean bInvalidate);

IFormControl* xDox_ObtainObject(Dox *me, int pos);

int Dox_ParseString(Dox *me, const char *pszIn, int cbLen);

int32 Dox_GetElemID(Dox *me, const char *pszElem);
int32 Dox_GetAttrID(Dox *me, const char *pszAttr);
int32 Dox_GetAttrString(Dox *me, int nElem, const char *pszAttr, char *pchBuf, int cbBuf);

int xDox_SetUrl(Dox *me, const char *pszUrl);

void xDox_GetIWeb(Dox *me, IWeb ** ppIWeb);
void xDox_SetIWeb(Dox *me, IWeb * pIWeb);

int xDox_MakeUrl(Dox *me, int pos, const char *cpszRel, char *pResult, int cbResultLen);

void xDox_SetType(Dox *me, const char *pszMediaType);
void xDox_SetCharset(Dox *me, const char *pszCharset, int cbCharset);

void Dox_Draw(Dox *me, ICanvas *piCanvas, int x, int y);

int Dox_GetImage(Dox *me, int16 idImage, IImage **ppii);

int Dox_GetFormElemState(Dox *me, char *pszBuffer, int cbBuffer);
void Dox_SetFormElemState(Dox *me, const char *pszState);
void Dox_CheckRadios(Dox *me, int posRadio);

int32 Dox_GetElemText(Dox *me, int32 nElem, char *pchBuf, int32 cbBuf);
int32 Dox_FindElem(Dox *me, const char *pszElem, int32 nIndex);

void Dox_SetFont(Dox *me, AEEFont fnt, IFont *piFont);

//---- Dox Inline Objects


struct DoxObject {
   int    wid;
   int    hgt;
   int    baseline;
   RGBVAL color;
};


void        DoxObject_GetSize(DoxObject *me, int *pnWid, int *pnHgt,int *pnBaseline);
DoxObject * DoxObject_New(Dox *me, int posElem, int cb);
void        DoxObject_Delete(DoxObject *me);


//---- UI Interface


// Invalidation flags, and the actions they result in.
//
// These are strictly prioritized; lower bits are handled first.
//
#define DOX_INVALWRAP      1   // layout document (this usually sets DOX_INVALDRAW)
#define DOX_INVALFOCUS     2   // verify new focus & notify host if necessary
#define DOX_INVALDOC       4   // redraw the document
#define DOX_INVALSB        8   // redraw the scroll bar
#define DOX_INVALDISPLAY   16  // update the display

#define DOX_INVALALL       31

#define DOX_INVALDRAW      (DOX_INVALDOC | DOX_INVALSB)


int       xDox_Invalidate(Dox *me, int nInvalFlags);
void      xDox_SetRect(Dox *me, AEERect *prc);
int       xDox_Update(Dox *me);
void      xDox_GetScrollInfo(Dox *me, DoxScrollInfo *psi);
int       xDox_GetScrollIncrement(Dox *me);
void      xDox_SetInvalCB(Dox *me, DoxInvalFn pfn, void *pvData);
boolean   xDox_HandleEvent(Dox *me, AEEEvent evt, uint16 wParam, uint32 dwParam);
uint32    Dox_GetFlags(Dox *me);
void      Dox_SetFlags(Dox *me, uint32 dwFlags);
void      Dox_TestParser(Dox *me);
boolean   xDox_Move(Dox *me, boolean bFwd);
void      Dox_Test(Dox *me);

void      xDox_SetActive(Dox *me, boolean bActive);
boolean   xDox_IsActive(Dox *me);
int       xDox_SetBufferSize(Dox *me, uint32 dwSize);
uint32    xDox_GetBufferSize(Dox *me);

void      xDox_SetLinkColor(Dox *me, RGBVAL rgbLink);
void      xDox_SetDocBGColor(Dox *me, RGBVAL rgbDocBg);
RGBVAL    xDox_GetLinkColor(Dox *me);
RGBVAL    xDox_GetDocBGColor(Dox *me);
void      xDox_SetScrollMLLink(Dox *me, boolean b);
boolean   xDox_GetScrollMLLink(Dox *me);

void      xDox_SetFocusPos(Dox *me, int pos);
int       xDox_GetFocusPos(Dox *me);


void      xDox_SetScrollPos(Dox *me, int32 yTop);
int32     xDox_GetScrollPos(Dox *me);


// returns the next scroll position (to allow scrolling by whole line increments) 
//int Dox_GetNextScrollPos(Dox *me, int yScrollPos, int bNext);
// FocusItems: not necessarily the same as normal "items".  May break a text
// item into multiple lines, with each line treated as a focus item.
// returns the next focus item (document position) 
//int32 Dox_GetNextFocusItem(Dox *me, int bForward, int *pyTop, int *pyBtm);
// returns the position and size of the focus item 
//void Dox_GetFocusItemPos(Dox *me, int32 posItem, AEERect *prc);
// This can affect layout; Dox should handle updates

#if 0
boolean Dox_HandlePens(Dox * me, AEEEvent evtCode, uint16 wParam, uint32 dwParam);
// Returns TRUE if pen is being tracked, FALSE if not
// ptTracker is optional. To be filled if provided.
boolean Dox_GetPenTracking(Dox * me, HTMLPenTracker * ptTracker);
uint8 DoxMapPointToItem(Dox * me, int16 wXPos, int16 wYPos, int * pnData, uint8 * pcbFlags);
#endif

// Form context
void     xDox_StopObjects      (Dox *me);



//////////////////////////////////////////////
// Character set encoding and decoding
//
int Dox_Encode      (Dox *me, char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed);
int Dox_Decode      (Dox *me, AECHAR *pwDest, int cwDest, const char *pcSrc, int cbSrc, int *pnUsed);
int Dox_DecodeTerminate (Dox *me, AECHAR *pwDest, int cwDest, const char *pcSrc, int cbSrc, int *pnUsed);
int Dox_EncodeTerminate (Dox *me, char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed);


#endif   //DOX_H

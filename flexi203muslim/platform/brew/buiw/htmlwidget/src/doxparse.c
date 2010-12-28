/*===================================================================
FILE: doxparse.c

SERVICES:  DoxParse object

DESCRIPTION:  HTML/XML Parser for dox.

        Copyright 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===================================================================*/

#include "AEE.h"
#include "AEEStdLib.h"

#include "doxparse.h"

// To do:
//
// - memory management (reallocation?)
// - XML changes:
//    PI's
//    <![CDATA[ ... ]]>
//    arbitrary names (valid name chars...)
//    entity declarations
//    external entities


//
//-- Elements -------------------------------------------------------------
//


// Shorthands for ELEMFLAG combinations:
//
#define ELEMFLAG_NORM   0
#define ELEMFLAG_HDR    (ELEMFLAG_B | ELEMFLAG_BIG)
#define ELEMFLAG_CENT   (ELEMFLAG_ALIGNCENTER)


// Shorthands for ELEMDISP combinations:
//
#define ELEMDISP_INLINE   0
#define ELEMDISP_VBLOCK   (ELEMDISP_BLOCK | ELEMDISP_VSPACE)
#define ELEMDISP_HBLOCK   (ELEMDISP_BLOCK | ELEMDISP_HSPACE)
#define ELEMDISP_VHBLK    (ELEMDISP_BLOCK | ELEMDISP_VSPACE | ELEMDISP_HSPACE)
#define ELEMDISP_HDRS     (ELEMDISP_BLOCK | ELEMDISP_VSPACE | ELEMDISP_ALIGN)
#define ELEMDISP_P        (ELEMDISP_BLOCK | ELEMDISP_VSPACE | ELEMDISP_ALIGN)
#define ELEMDISP_DIV      (ELEMDISP_BLOCK | ELEMDISP_ALIGN)
#define ELEMDISP_LI       (ELEMDISP_BLOCK | ELEMDISP_DECORATE)
#define ELEMDISP_HR       (ELEMDISP_VBLOCK | ELEMDISP_DECORATE)
#define ELEMDISP_PRE      (ELEMDISP_VBLOCK | ELEMDISP_SPACED)
#define ELEMDISP_TXAREA   (ELEMDISP_OBJECT | ELEMDISP_SPACED)

#define ELEMCONT_FORM     (ELEMCONT_NEST | CONTENT_TR | CONTENT_TD)


#define EC(a,b,c,d,e) \
   { CONTENT_##a, ELEMCONT_##b, MLST_##c, ELEMFLAG_##d, ELEMDISP_##e}


const ContentModel acmHtmlElems[ELEM_MAX] = {
   EC(INLN, NONE,   NONE, NORM, IGNORE),   // XMLDECL  (?xml)
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // A
   EC(PARG, INLN, PCDATA, I   , BLOCK ),   // ADDRESS
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // APPLET
   EC(INLN, NONE, PCDATA, NORM, INLINE),   // AREA
   EC(INLN, INLN, PCDATA, B   , INLINE),   // B
   EC(INLN, NONE,   NONE, NORM, INLINE),   // BASE
   EC(INLN, INLN, PCDATA, BIG , INLINE),   // BIG
   EC(NEST, NEST, PCDATA, NORM, VHBLK ),   // BLOCKQUOTE
   EC(NEST, ALL , PCDATA, NORM, INLINE),   // BODY
   EC(INLN, NONE,   NONE, NORM, VBLOCK),   // BR
   EC(TR  , NEST, PCDATA, NORM, INLINE),   // CAPTION
   EC(PARG, INLN, PCDATA, CENT, BLOCK ),   // CENTER
   EC(INLN, INLN, PCDATA, I   , INLINE),   // CITE
   EC(INLN, INLN, PCDATA, TT  , INLINE),   // CODE
   EC(NEST, NEST, PCDATA, NORM, HBLOCK),   // DD
   EC(INLN, INLN, PCDATA, I   , INLINE),   // DFN
   EC(LIST, NEST, PCDATA, NORM, VHBLK ),   // DIR
   EC(NEST, NEST, PCDATA, NORM, DIV   ),   // DIV
   EC(LIST, NEST, PCDATA, NORM, VBLOCK),   // DL
   EC(NEST, NEST, PCDATA, NORM, BLOCK ),   // DT
   EC(INLN, INLN, PCDATA, I   , INLINE),   // EM
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // FONT
   EC(NEST, FORM, PCDATA, NORM, VBLOCK),   // FORM
   EC(PARG, INLN, PCDATA, HDR , HDRS  ),   // H1
   EC(PARG, INLN, PCDATA, HDR , HDRS  ),   // H2
   EC(PARG, INLN, PCDATA, HDR , HDRS  ),   // H3
   EC(PARG, INLN, PCDATA, B   , HDRS  ),   // H4
   EC(PARG, INLN, PCDATA, B   , HDRS  ),   // H5
   EC(PARG, INLN, PCDATA, B   , HDRS  ),   // H6
   EC(NEST, ALL , PCDATA, NORM, INLINE),   // HEAD
   EC(PARG, NONE,   NONE, NORM, HR    ),   // HR
   EC(NEST, ALL , PCDATA, NORM, INLINE),   // HTML
   EC(INLN, INLN, PCDATA, I   , INLINE),   // I
   EC(INLN, NONE,   NONE, NORM, OBJECT),   // IMG
   EC(INLN, NONE,   NONE, NORM, OBJECT),   // INPUT
   EC(INLN, NONE,   NONE, NORM, INLINE),   // ISINDEX
   EC(INLN, INLN, PCDATA, TT  , INLINE),   // KBD
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // LABEL
   EC(NEST, NEST, PCDATA, NORM, LI    ),   // LI
   EC(INLN, NONE,   NONE, NORM, INLINE),   // LINK
   EC(INLN, NONE,   NONE, NORM, INLINE),   // MAP
   EC(LIST, NEST, PCDATA, NORM, VHBLK ),   // MENU
   EC(INLN, NONE,   NONE, NORM, INLINE),   // META
   EC(LIST, NEST, PCDATA, NORM, VHBLK ),   // OL
   EC(PARG, INLN, RCDATA, NORM, INLINE),   // OPTION
   EC(PARG, INLN, PCDATA, NORM, P     ),   // P
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // PARAM ?
   EC(NEST, NEST, RCDATA, TT  , PRE   ),   // PRE
   EC(INLN, INLN, PCDATA, TT  , INLINE),   // SAMP
   EC(INLN, DATA,  CDATA, NORM, IGNORE),   // SCRIPT
   EC(INLN, NEST, RCDATA, NORM, OBJECT),   // SELECT
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // SMALL
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // STRIKE
   EC(INLN, INLN, PCDATA, B,    INLINE),   // STRONG
   EC(INLN, DATA,  CDATA, NORM, IGNORE),   // STYLE
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // SUB
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // SUP
   EC(INLN, TBL , PCDATA, NORM, BLOCK ),   // TABLE
   EC(TD  , NEST, PCDATA, NORM, INLINE),   // TD
   EC(INLN, INLN,  CDATA, NORM, TXAREA),   // TEXTAREA
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // TEXTFLOW ? ?
   EC(TD  , NEST, PCDATA, I   , INLINE),   // TH
   EC(INLN, DATA, PCDATA, NORM, IGNORE),   // TITLE : contents=NONE, text!=RCDATA
   EC(TR  , TR  , PCDATA, NORM, INLINE),   // TR
   EC(INLN, INLN, PCDATA, NORM, INLINE),   // TT
   EC(INLN, INLN, PCDATA, U   , INLINE),   // U
   EC(LIST, NEST, PCDATA, NORM, VHBLK ),   // UL
   EC(INLN, INLN, PCDATA, I   , INLINE),   // VAR
   EC(NEST, DATA,  CDATA, TT  , SPACED),   // XMP
};




static const char szElemNames[] = {
   "?xml,"
   "a,"
   "address,"
   "applet,"
   "area,"
   "b,"
   "base,"
   "big,"
   "blockquote,"
   "body,"
   "br,"
   "caption,"
   "center,"
   "cite,"
   "code,"
   "dd,"
   "dfn,"
   "dir,"
   "div,"
   "dl,"
   "dt,"
   "em,"
   "font,"
   "form,"
   "h1,"
   "h2,"
   "h3,"
   "h4,"
   "h5,"
   "h6,"
   "head,"
   "hr,"
   "html,"
   "i,"
   "img,"
   "input,"
   "isindex,"
   "kbd,"
   "label,"
   "li,"
   "link,"
   "map,"
   "menu,"
   "meta,"
   "ol,"
   "option,"
   "p,"
   "param,"
   "pre,"
   "samp,"
   "script,"
   "select,"
   "small,"
   "strike,"
   "strong,"
   "style,"
   "sub,"
   "sup,"
   "table,"
   "td,"
   "textarea,"
   "textflow,"
   "th,"
   "title,"
   "tr,"
   "tt,"
   "u,"
   "ul,"
   "var,"
   "xmp,"
};



//-- Attributes -----------------------------------------------------------
//


static const char szAttrNames[] = {
   "accesskey,"
   "action,"
   "align,"
   "alink,"
   "alt,"
   "background,"
   "bgcolor,"
   "border,"
   "checked,"
   "class,"
   "clear,"
   "color,"
   "cols,"
   "compact,"
   "content,"
   "encoding,"
   "enctype,"
   "for,"
   "height,"
   "href,"
   "href2,"
   "hspace,"
   "http-equiv,"
   "id,"
   "ismap,"
   "istyle,"
   "language,"
   "link,"
   "maxlength,"
   "method,"
   "multiple,"
   "name,"
   "noshade,"
   "prompt,"
   "rel,"
   "rev,"
   "rows,"
   "selected,"
   "size,"
   "src,"
   "style,"
   "text,"
   "title,"
   "type,"
   "usemap,"
   "value,"
   "version,"
   "vlink,"
   "vspace,"
   "width,"
};


//-- Entities -------------------------------------------------------------
//


static const char gszEntityNames[] = {
   "amp,"
   "gt,"
   "lt,"
   "quot,"
   "AMP,"
   "GT,"
   "LT,"
   "QUOT,"
   "apos,"
   "bull,"
   "trade,"
   "copy,"
   "reg,"
   "euro,"
   "yen,"
   "pound,"
   "cent,"
   "ndash,"
   "mdash,"
   "ldquo,"
   "rdquo,"
   "lsquo,"
   "rsquo,"
   "AElig,"
   "Aacute,"
   "Acirc,"
   "Agrave,"
   "Alpha,"
   "Aring,"
   "Atilde,"
   "Auml,"
   "aacute,"
   "acirc,"
   "acute,"
   "aelig,"
   "agrave,"
   "alefsym,"
   "alpha,"
   "and,"
   "ang,"
   "aring,"
   "asymp,"
   "atilde,"
   "auml,"
   "Beta,"
   "bdquo,"
   "beta,"
   "brvbar,"
   "Ccedil,"
   "Chi,"
   "cap,"
   "ccedil,"
   "cedil,"
   "chi,"
   "circ,"
   "clubs,"
   "cong,"
   "crarr,"
   "cup,"
   "curren,"
   "Dagger,"
   "Delta,"
   "dArr,"
   "dagger,"
   "darr,"
   "deg,"
   "delta,"
   "diams,"
   "divide,"
   "ETH,"
   "Eacute,"
   "Ecirc,"
   "Egrave,"
   "Epsilon,"
   "Eta,"
   "Euml,"
   "eacute,"
   "ecirc,"
   "egrave,"
   "empty,"
   "emsp,"
   "ensp,"
   "epsilon,"
   "equiv,"
   "eta,"
   "eth,"
   "euml,"
   "exist,"
   "fnof,"
   "forall,"
   "frac12,"
   "frac14,"
   "frac34,"
   "frasl,"
   "Gamma,"
   "gamma,"
   "ge,"
   "hArr,"
   "harr,"
   "hearts,"
   "hellip,"
   "Iacute,"
   "Icirc,"
   "Igrave,"
   "Iota,"
   "Iuml,"
   "iacute,"
   "icirc,"
   "iexcl,"
   "igrave,"
   "image,"
   "infin,"
   "int,"
   "iota,"
   "iquest,"
   "isin,"
   "iuml,"
   "Kappa,"
   "kappa,"
   "Lambda,"
   "lArr,"
   "lambda,"
   "lang,"
   "laquo,"
   "larr,"
   "lceil,"
   "le,"
   "lfloor,"
   "lowast,"
   "loz,"
   "lrm,"
   "lsaquo,"
   "Mu,"
   "macr,"
   "micro,"
   "middot,"
   "minus,"
   "mu,"
   "Ntilde,"
   "Nu,"
   "nabla,"
   "nbsp,"
   "ne,"
   "ni,"
   "not,"
   "notin,"
   "nsub,"
   "ntilde,"
   "nu,"
   "OElig,"
   "Oacute,"
   "Ocirc,"
   "Ograve,"
   "Omega,"
   "Omicron,"
   "Oslash,"
   "Otilde,"
   "Ouml,"
   "oacute,"
   "ocirc,"
   "oelig,"
   "ograve,"
   "oline,"
   "omega,"
   "omicron,"
   "oplus,"
   "or,"
   "ordf,"
   "ordm,"
   "oslash,"
   "otilde,"
   "otimes,"
   "ouml,"
   "Phi,"
   "Pi,"
   "Prime,"
   "Psi,"
   "para,"
   "part,"
   "permil,"
   "perp,"
   "phi,"
   "pi,"
   "piv,"
   "plusmn,"
   "prime,"
   "prod,"
   "prop,"
   "psi,"
   "Rho,"
   "rArr,"
   "radic,"
   "rang,"
   "raquo,"
   "rarr,"
   "rceil,"
   "real,"
   "rfloor,"
   "rho,"
   "rlm,"
   "rsaquo,"
   "Scaron,"
   "Sigma,"
   "sbquo,"
   "scaron,"
   "sdot,"
   "sect,"
   "shy,"
   "sigma,"
   "sigmaf,"
   "sim,"
   "spades,"
   "sub,"
   "sube,"
   "sum,"
   "sup1,"
   "sup2,"
   "sup3,"
   "sup,"
   "supe,"
   "szlig,"
   "THORN,"
   "Tau,"
   "Theta,"
   "tau,"
   "there4,"
   "theta,"
   "thetasym,"
   "thinsp,"
   "thorn,"
   "tilde,"
   "times,"
   "Uacute,"
   "Ucirc,"
   "Ugrave,"
   "Upsilon,"
   "Uuml,"
   "uArr,"
   "uacute,"
   "uarr,"
   "ucirc,"
   "ugrave,"
   "uml,"
   "upsih,"
   "upsilon,"
   "uuml,"
   "weierp,"
   "Xi,"
   "xi,"
   "Yacute,"
   "Yuml,"
   "yacute,"
   "yuml,"
   "Zeta,"
   "zeta,"
   "zwj,"
   "zwnj,"
};

static const AECHAR gawEntityValues[] = {
   38,
   62,
   60,
   34,
   38,
   62,
   60,
   34,
   39,    // apos
   8226,
   8482,
   169,
   174,
   8364,
   165,
   163,
   162,
   8211,
   8212,
   8220,
   8221,
   8216,
   8217,
   198,
   193,
   194,
   192,
   913,
   197,
   195,
   196,
   225,
   226,
   180,
   230,
   224,
   8501,
   945,
   8743,
   8736,
   229,
   8776,
   227,
   228,
   914,
   8222,
   946,
   166,
   199,
   935,
   8745,
   231,
   184,
   967,
   710,
   9827,
   8773,
   8629,
   8746,
   164,
   8225,
   916,
   8659,
   8224,
   8595,
   176,
   948,
   9830,
   247,
   208,
   201,
   202,
   200,
   917,
   919,
   203,
   233,
   234,
   232,
   8709,
   8195,
   8194,
   949,
   8801,
   951,
   240,
   235,
   8707,
   402,
   8704,
   189,
   188,
   190,
   8260,
   915,
   947,
   8805,
   8660,
   8596,
   9829,
   8230,
   205,
   206,
   204,
   921,
   207,
   237,
   238,
   161,
   236,
   8465,
   8734,
   8747,
   953,
   191,
   8712,
   239,
   922,
   954,
   923,
   8656,
   955,
   9001,
   171,
   8592,
   8968,
   8804,
   8970,
   8727,
   9674,
   8206,
   8249,
   924,
   175,
   181,
   183,
   8722,
   956,
   209,
   925,
   8711,
   160,
   8800,
   8715,
   172,
   8713,
   8836,
   241,
   957,
   338,
   211,
   212,
   210,
   937,
   927,
   216,
   213,
   214,
   243,
   244,
   339,
   242,
   8254,
   969,
   959,
   8853,
   8744,
   170,
   186,
   248,
   245,
   8855,
   246,
   934,
   928,
   8243,
   936,
   182,
   8706,
   8240,
   8869,
   966,
   960,
   982,
   177,
   8242,
   8719,
   8733,
   968,
   929,
   8658,
   8730,
   9002,
   187,
   8594,
   8969,
   8476,
   8971,
   961,
   8207,
   8250,
   352,
   931,
   8218,
   353,
   8901,
   167,
   173,
   963,
   962,
   8764,
   9824,
   8834,
   8838,
   8721,
   185,
   178,
   179,
   8835,
   8839,
   223,
   222,
   932,
   920,
   964,
   8756,
   952,
   977,
   8201,
   254,
   732,
   215,
   218,
   219,
   217,
   933,
   220,
   8657,
   250,
   8593,
   251,
   249,
   168,
   978,
   965,
   252,
   8472,
   926,
   958,
   221,
   376,
   253,
   255,
   918,
   950,
   8205,
   8204
};


//
//-- Standard HTML color names --------------------------------------------
//

static const char szColorNames[] =
   {"aqua,black,blue,fuchsia,"
    "gray,green,lime,maroon,"
    "navy,olive,purple,red,"
    "silver,teal,white,yellow,"};


//   (BREW RGBVALs have a "bbggrr00" layout)
//
static const RGBVAL argbColorValues[] = {
   0xffff0000, 0x00000000, 0xff000000, 0xff00ff00,
   0x80808000, 0x00800000, 0x00ff0000, 0x00008000,
   0x80000000, 0x00808000, 0x80008000, 0x0000ff00,
   0xc0c0c000, 0x80800000, 0xffffff00, 0x00ffff00
};


//
//-- Macros  --------------------------------------------------------------
//


// SGML separators:    32, 13, 10, 9
//                     [SPACE, SEPCHAR, RE, and implicitly RS ... p. 345]
//
// SGML names (regex): (A-Za-z)(A-Za-z0-9-.)*   [p. 297]
//
// SGML start tag:     "<" GROUP NAME ATTRSPEC* S* ">"
//                     ATTRSPEC      = S* ( NAME S* VI S* )? ATTRVALUESPEC
//                     ATTRVALUESPEC =  ATTRVALUE | '"' RCDATA '"' | "'" RCDATA "'"
//                     ATTRVALUE     = a big mess

#define SPACECHAR          ('\040')
#define LFCHAR             ('\012')
#define TABCHAR            ('\011')


#define ISALPHA(c)         CHARISIN( ToLower(c), 'a', 'z')    // ASCII letter
#define ISSPACE(c)         ( ((unsigned char) c) <= 32 )
#define ISDIGIT(c)         CHARISIN( (c), '0', '9')
#define ISXDIGIT(c)        ( CHARISIN( (c), '0', '9') || CHARISIN( ToLower(c), 'a', 'f') )


// XML definitions
//
// ISLETTER() includes more chars than the XML spec lists, but all such
//    non-letter characters would be illegal in the context of names, so
//    grouping them as part of a name will allow checking for validity later
//    (perhaps more efficiently).
//
// ISDIGIT() includes fewer characters than actual XML, but all such are
//    included in ISLETTER().
//
#define ISLETTER(c)        ( ISALPHA(c) || ((c) & '\200') )
#define ISXMLNMCHAR(c)     ( ISLETTER(c) || ISDIGIT(c) || (c)=='.' || (c)=='-' || (c)=='_' || (c)==':' )
#define ISXMLNMSTART(c)    ( ISLETTER(c) || (c)=='_' || (c)==':' )


#ifdef STRICT_HTML_COMPAT
//
// These are not currently used because they conflict with XML syntax, but we
// could later make this a mode based on MIME type.  This would be of minimal
// value; only difference is handling some rare malformed markup more like IE
// and Mozilla.
//
//   ISNMCHAR  = XML version minus ':' and non-ASCII letters
//   ISNMSTART = XML version minus ':' and non-ASCII letters
#else
# define ISNMCHAR(c)        ISXMLNMCHAR(c)
# define ISNMSTART(c)       ISXMLNMSTART(c)
#endif


#define DOX_BADID    DoxIDChar(-1)    // IDChar resulting from unknown entity/element


//
//-- Utility Functions -----------------------------------------------------
//

char ToLower(char c)
{
   uint8   x = (uint8) c;
   if ((x >= 'A') && (x <= 'Z')) {
      x = 'a' + (x - 'A');
   } else if ((x >= 192) /* À (capital a with grave) */ && 
              (x <= 222) /* Þ (capital thorn) */ && 
              (x != 215) /* × (multiplication sign) */) {
      x = 224 + (x - 192);
   }
   return (char)x;
}




#define FINDNAME_USECASE   (0)
#define FINDNAME_IGNCASE   (32)

#define FINDNAME_NOMATCH   -1

// Find name in list of comma-delimited values
//
// Note: Case-insensitive mode will fail to distinguish non-ASCII characters from 
//       each other, and fail for the following ASCII characters:  @ [ \ ] ^ _
//
static int DoxFindName(const char *pszList, const char *pszName, char cCaseMode)
{
   const char *psz = pszList;
   int nName = 0;

   do {
      const char *pszTest = pszName;
      while (*psz == (*pszTest | cCaseMode)) {
         ++psz;
         ++pszTest;
      }
      
      if ((*pszTest|cCaseMode) != *psz) {
         if (*psz == ',') {
            if (*pszTest == '\0') {
               return nName;
            }
         } else {
            while (*psz != ',')
               ++psz;
         }
         // else name in list is shorter than search name; try next entry
         ++psz;
      } else {
         // name in list < pszName
         while (*psz++ != ',')
            ;
      }

      ++nName;
   } while (*psz);

   return FINDNAME_NOMATCH;
}


static int xHexValue(char ch)
{
   int cRet = (ch |= 32);

   if (CHARISIN(cRet, '0', '9'))
      cRet -= '0';
   else if (CHARISIN(cRet, 'a', 'z'))
      cRet -= ('a' - 10);
   else
      cRet = -1;

   return cRet;
}


//
//-- DoxParse Methods -----------------------------------------------------
//


#define OUTCHAR(c) (void)((me->pchOut < me->pchOutMax) && (boolean)(*me->pchOut++ = (c)))


void DoxParse_Ctor(DoxParse *me, Dox *pdox)
{
   int cbMax = pdox->cbBuf;

   SETZERO(me);

   xDox_Reset(pdox, FALSE);
   
   me->pdox = pdox;
   me->stData = MLST_PCDATA;
   me->st = MLST_PCDATA;
   me->cntStack = 0;

   // Dox buffer invariants:
   //
   //   me->pcBuf[-1]         == DOX_SE
   //   me->pcBuf[me->cbData] == DOX_EE
   //
   // For safety, a DOX_EE character must occur before the end of allocated
   // memory.  For accuracy, a DOX_EE character should occur immediately
   // following cbData bytes.  DoxParse ensures that these are in effect after
   // it completes.  During parsing, a character at the end of the buffer
   // should hold DOX_EE.
   //

   me->pchOut = pdox->pcBuf;
   me->pchOutMax = pdox->pcBuf + cbMax - 1;
   me->pchTerm = me->pchOut;

   me->pchOutMax[1] = DOX_EE;     // DoxParse should not overwrite this; pchOutMax[0] is not safe
   
   if (cbMax == 0) {
      // could be const data
      me->st = MLST_FULL;
   }
}


void DoxParse_Resize(DoxParse *me, Dox *pdox)
{
   int cbMax = pdox->cbBuf;

   me->pchOut     = pdox->pcBuf;
   me->pchOutMax  = pdox->pcBuf + cbMax - 1;

   me->pchOutMax[1] = DOX_EE;     // DoxParse should not overwrite this; pchOutMax[0] is not safe

   if (cbMax == 0 || pdox->cbData >= pdox->cbBuf) {
      // could be const data
      me->st = MLST_FULL;
   }
}


// Return element we just closed
//
static DoxTag DoxParse_Pop(DoxParse *me)
{
   if (me->cntStack)
      --me->cntStack;

   if (me->cntStack < MAXSTACKSIZE) {
      return me->anStack[me->cntStack];
   } else {
      return me->anStack[MAXSTACKSIZE-1];
   }
}


static __inline void DoxParse_Push(DoxParse *me, DoxTag nTag)
{
   if (me->cntStack < MAXSTACKSIZE) {
      me->anStack[me->cntStack] = nTag;
      ++me->cntStack;
   }
}


// See if a close tag for nElem can be handled at this point.
//
// If it is at the top of the stack, or if in the stack and all intervening
// elements are allowed to be closed by it, then return the 'destination'
// stack index.
//
static int DoxParse_FindClose(DoxParse *me, int nElem)
{
   int cntStack = me->cntStack;

   while (--cntStack >= 0) {
      int nParent;

      if (cntStack >= MAXSTACKSIZE)
         return cntStack;
      
      nParent = me->anStack[cntStack];

      if (nParent == nElem) {
         return cntStack;
      }

      // Cannot close parent when it can't contain parent.
      // Also... NEST elements cannot close LIST elements.
      if ((acmHtmlElems[nElem].fContents & acmHtmlElems[nParent].fClass) == 0 ||
          (acmHtmlElems[nElem].fClass == CONTENT_NEST &&
           acmHtmlElems[nParent].fClass == CONTENT_LIST)) {
         break;
      }
   }

   return -1;
} 


// Process a parsed close tag
//
void DoxParse_Close(DoxParse *me)
{
   int cntStack = DoxParse_FindClose(me, me->nTag);
   if (cntStack >= 0) {

      // Got match ... POP down to cntStack
      while (me->cntStack > cntStack) {
         OUTCHAR( DOX_EE );
         --me->cntStack;
      }
            
      if (cntStack && cntStack <= MAXSTACKSIZE) {
         int nTagCur = me->anStack[cntStack-1];    // 'current' or 'parent' element
         me->stData = acmHtmlElems[nTagCur].stData;
      } else {
         me->stData = MLST_PCDATA;
      }
   } else {

      // No match:  just rewind
      me->pchOut = me->pchElem;
   }
}


void DoxParse_Open(DoxParse *me, int bSlashEnd)
{
   DoxTag nTag = me->nTag;

   if (nTag < 0) {
      // ignore
      me->pchOut = me->pchElem;
      return;
   }
   
   if (acmHtmlElems[nTag].fContents == ELEMCONT_NONE) {
      // Empty element
      OUTCHAR( DOX_EE );

      // Assuming [x]HTML...
      if (me->pchElem[1] == DoxIDChar(ELEM_META)) {
         // Look for HTTP-EQUIV="Content-type"
         const char *pszEquiv = xDoxGetAttrValue(me->pchElem, ATTR_HTTP_EQUIV);
         if (xDoxStrEQI(pszEquiv, "content-type")) {
            const char *pszType = xDoxGetAttrString(me->pchElem, ATTR_CONTENT, NULL);
            if (pszType) {
               xDox_SetType(me->pdox, pszType);
            }
         }
      }

      // Assuming XML
      if (me->pchElem[1] == DoxIDChar(ELEM_XMLDECL)) {
         // Look for HTTP-EQUIV="Content-type"
         const char *pszEnc = xDoxGetAttrString(me->pchElem, ATTR_ENCODING, NULL);
         if (pszEnc) {
            xDox_SetCharset(me->pdox, pszEnc, STRLEN(pszEnc));
         }
      }

   } else {

      // Begin non-empty element

      ElemCat fClass = acmHtmlElems[nTag].fClass;

      // Close outer element if it should not contain this element
      
      while (me->cntStack >= 1) {

         int nOuterTag  = me->anStack[me->cntStack-1];
         int fOuterCont = acmHtmlElems[nOuterTag].fContents;

         if ((fOuterCont & fClass) == 0 ||
             (nTag == ELEM_LI && DoxParse_FindClose(me, nTag) >= 0)) {

            // Close current tag:
            //  1. Output SE (prior to current pchElem)

            if (me->pchOut == me->pchOutMax)
               break;
            
            MEMMOVE(me->pchElem+1, me->pchElem, me->pchOut-me->pchElem);
            me->pchElem[0] = DOX_EE;
            ++me->pchElem;
            ++me->pchOut;
            
            //  2. Decrement cntStack

            --me->cntStack;
            
         } else {

            break;
         }
      }

      // Go ahead and proceed with this element
      
      DoxParse_Push(me, nTag);
      me->stData = acmHtmlElems[nTag].stData;
      OUTCHAR( DOX_CD );

      // Handle:  <APPLET />
      if (bSlashEnd) {
         DoxParse_Close(me);
      }
   }
}


void DoxParse_Write(DoxParse *me, const char *pc, int cb)
{
   MLSt st = me->st;

   CharSet *pCharSet = &me->pdox->cs;
   
   while (cb) {
      char c = *pc++;
      --cb;

   processchar:         // for immediate transition to st
      
      switch (st) {

         // Data states:
         //
         //    me->stEnt  = state to which to return after translating entity
         
      case MLST_PCDATA:
      case MLST_RCDATA:
      case MLST_CDATA:
         if (c == '<') {
            me->pchElem = me->pchOut;
            OUTCHAR(c);
            st = MLST_STAGO;
         } else if (c == '&')
            goto getentity;
         else if ((unsigned char)c >= (unsigned char)SPACECHAR
                  || c == LFCHAR
                  || c == TABCHAR) {
            OUTCHAR(c);
         }
         break;

      getentity:
         // Read all of an entity (or potential entity) and return to 'st' when done
         me->pchEnt = me->pchOut;
         me->stEnt = st;
         OUTCHAR(c);
         st = MLST_OE;
         break;

         // OE:  After '&' character ; read and translate entity
         //
         //    me->stEnt  = state to which to return after translating entity
         
      case MLST_OE:
         me->chEnt = 0;
         if (c == '#') {
            OUTCHAR(c);
            st = MLST_ENTDEC;
            break;
         }
         st = MLST_ENTITY;
         // FALLTHROUGH

         // ENTITY:  After '&' ; read alphanum characters
         //
         //    me->pchEnt = position of '&' in buffer
         //    me->chEnt  = 0
         //    me->stEnt  = state to which to return after translating entity
         
      case MLST_ENTITY:
         if (ISALPHA(c) || ISDIGIT(c)) {
            // For XML, we will need to handle any XML "name" here, but this
            // is more efficient and better for HTML compatibility.
            OUTCHAR(c);
            break;
         }
         if (me->pchOut >= me->pchOutMax) {
            st = MLST_FULL;
            break;
         } else {
            int nEnt;
            me->pchOut[0] = 0;
            nEnt = DoxFindName(gszEntityNames, me->pchEnt+1, FINDNAME_USECASE);
            if (nEnt >= 0) {
               me->chEnt = gawEntityValues[nEnt];
            }
         }

      stuffchar:
         if (NULL != CharSet_GetFromUnicode(pCharSet)) {
            // convert from Unicode to native wide encoding (pfn is NULL when Unicode IS native)
            me->chEnt = CharSet_FromUnicode(pCharSet, me->chEnt);
            if (me->chEnt == 0 && st != MLST_ENTITY) {
               // Leave named entities in place; "&#nn;" is not left in place
               me->chEnt = '?';
            }
         }
         st = me->stEnt;
         // encode character into buffer
         if (me->chEnt) {
            int nUsed;  // dummy variable
            int cb;
            cb = CharSet_Encode(pCharSet, me->pchEnt, me->pchOutMax - me->pchEnt, &me->chEnt, 1, &nUsed);
            me->pchOut = me->pchEnt + cb;
            if (c == ';') {
               break;
            }
         } // else leave unrecognized entity in place
         goto processchar;


         // ENTDEC : After "&#" : Read decimal digits
         //
         //   [ENTITY invariants]
         //   me->chEnt  = value of all digits seen so far
         
      case MLST_ENTDEC:
         if (ISDIGIT(c)) {
            me->chEnt = me->chEnt * 10 + (c - '0');
            break;
         } else if (ToLower(c) == 'x' && me->chEnt == 0) {
            st = MLST_ENTHEX;
            break;
         }
         goto stuffchar;


         // ENTHEX : After "&#x" : Read hex digits       [CHARREF invariants apply]
         //
         
      case MLST_ENTHEX:
         if (ISXDIGIT(c)) {
            me->chEnt = me->chEnt * 16 + xHexValue(c);
            break;
         }
         goto stuffchar;


         // STAGO:  First character following "<"
         // 
         //   me->pchElem = pointer to "<" in buffer
         
      case MLST_STAGO:
         st = MLST_TAGNAME;
         if (ISNMSTART(c) || c == '/' || c == '!' || c == '?') {
            // '/' allowed only as first character
            OUTCHAR(c);
            break;
         }
         goto tagname_done;

         // TAGNAME: Character within tag name  ("<...")
         //
         //   me->pchElem = pointer to "<" in buffer
         
      case MLST_TAGNAME:
         if ( !ISSPACE(c) && c != '<' && c !=  '>' && c != '/') {
            // Eat valid & invalid characters here
            // Take everything up to the space in a name character, even if
            // not a valid name character (valid name charater rules applied
            // elsewhere).
            OUTCHAR(c);
            break;
         }
         // else end of tag name

      tagname_done:

         {
            char *psz = me->pchElem + 1;
            DoxTag nTag;

            // Validate psz (pchElem+1)
            if (psz >= me->pchOutMax) {
               st = MLST_FULL;
               break;
            }

            *me->pchOut = 0;

            // Open or close tag?
            st = MLST_TAGGAP;
            if (psz[0] == '/') {
               st = MLST_TAGEND;    // next state;  for now, this implies "/" was seen
               ++psz;
            }

            // HTML compatibility: treat invalid "<" literally
            else if (!psz[0]) {
               st = me->stData;
               goto processchar;
            }

            // SGML comment
            else if (psz[0] == '!' && psz[1] == '-' && psz[2] == '-' && me->stData != MLST_CDATA) {
               st = MLST_COMMENT;
               if (me->pchOut[-1] == '-') {
                  st = MLST_COMMENT1;
                  if (me->pchOut[-2] == '-') {
                     st = MLST_COMMENT2;
                  }
               }
               me->pchOut = me->pchElem;
               goto processchar;   // process character, which could be '>'
            }

            // Look up tag name
            nTag = (DoxTag) DoxFindName(szElemNames, psz, FINDNAME_IGNCASE);

            // CDATA : Treat "<..." as literal text when it is not a matching close tag.
            if (me->stData == MLST_CDATA && (MLST_TAGEND != st || nTag != me->nTag)) {
               st = me->stData;
               goto processchar;
            }

            // rewind over "<ELEMNAME"
            me->pchOut = me->pchElem;
            me->nTag = nTag;

            if (MLST_TAGEND == st) {
               goto processchar;    // handle "</...>"
            }

            OUTCHAR( DOX_SE );
            OUTCHAR( DoxIDChar(nTag) );
         }
         // FALLTHROUGH!

         // TAGGAP: within open tag awaiting attribute name or tag close
         //
         //   me->pchElem = start of open tag data (pointer to SE character)
         //   me->nTag = tag ID
         //

      case MLST_TAGGAP:
         if (c == '>') {
            DoxParse_Open(me, FALSE);
            st = me->stData;
         } else if (c == '/') {
            st = MLST_TAGGAPSLASH;
         } else if ( ISSPACE(c)) {
            // ignore
         } else {
            // Treat all else as an attribute name character.   Many are in fact illegal.
            me->pchAttr = me->pchOut;
            OUTCHAR(c);
            st = MLST_ATTRNAME;
         }
         break;

         // TAGGAPSLASH: after "/" in TAGGAP         [TAGGAP invariants apply]
         //
         
      case MLST_TAGGAPSLASH:
         if (c == '>') {
            DoxParse_Open(me, TRUE);
            st = me->stData;
            break;
         }
         // ignore error
         st = MLST_TAGGAP;
         goto processchar;
         

      case MLST_TAGEND:
         if (c == '>') {
            DoxParse_Close(me);
            st = me->stData;
         }
         break;


         // ATTRNAME : Within an attribute name      [TAGGAP invariants apply]
         //
         //   me->pchAttr = start of attribute name

      case MLST_ATTRNAME:
         if (!ISSPACE(c) && c != '=' && c != '>') {
            OUTCHAR(c);
            break;
         } else {
            int nAttr;

            // end of attribute name
         
            me->pchOut[0] = 0;

            nAttr = DoxFindName(szAttrNames, me->pchAttr, FINDNAME_IGNCASE);

            me->pchOut = me->pchAttr;

            OUTCHAR( DoxIDChar(nAttr) );
         }
         st = MLST_ATTRGAP;
         // FALLTHROUGH!

         // ATTRGAP : gap after attribute name       [TAGGAP invariants apply]
         //
         //   me->pchAttr[0..] = attribute ID char
         //   me->pchOut = me->pchAttr + 1  (attribute value is pending)

      case MLST_ATTRGAP:
         if (ISSPACE(c)) {
            // ignore
            break;
         } else if (c == '=') {
            st = MLST_ATTREQGAP;
            break;
         }
         // ">" or new attribute name => complete attrspec
         // FALLTHROUGH!

         // ATTRDONE : After complete attrspec      [AGAP invariants, except:]
         //
         //    me->pchOut = after complete attrspec definition

      case MLST_ATTRDONE:
      process_ADONE:
         // Dump entire attrspec?
         if (me->pchAttr[0] == DOX_BADID ) {
            me->pchOut = me->pchAttr;
         } else if (me->pchOut == me->pchAttr + 1) {
            OUTCHAR( DOX_NV );
         }
         st = MLST_TAGGAP;
         goto processchar;


         // ATTREQGAP : gap after "="                   [AGAP invariants apply]
         //

      case MLST_ATTREQGAP:
         if (ISSPACE(c)) {
            // ignore
         } else if (c == '>') {
            // NV or empty string?  Let's do NV...
            goto process_ADONE;
         } else if (c == '\'' || c == '\"') {
            me->cQuote = c;
            st = MLST_ATTRQVAL;
         } else {
            st = MLST_ATTRVAL;
            goto processchar;
         }
         break;

         
         // ATTRVAL : inside unquoted attribute value    [AGAP invariants apply]
         //

      case MLST_ATTRVAL:
         if (ISSPACE(c) || c == '>') {
            OUTCHAR( '\0' );
            goto process_ADONE;
         } else if (c == '&') {
            goto getentity;
         } else {
            OUTCHAR( c );
         }
         break;
         

         // ATTRQVAL : inside quoted attribute value      [AGAP invariants apply]
         //
         //    me->pchOut = next value character
         
      case MLST_ATTRQVAL:
         if (c == me->cQuote) {
            OUTCHAR( '\0' );
            st = MLST_ATTRDONE;
         } else if (c == '&') {
            goto getentity;
         } else if (!DoxIsDelim(c)) {
            OUTCHAR( c );
         }
         break;


      case MLST_FULL:
         break;

      case MLST_COMMENT:
         if (c == '-')
            st = MLST_COMMENT1;
         break;

      case MLST_COMMENT1:
         if (c == '-')
            st = MLST_COMMENT2;
         else
            st = MLST_COMMENT1;
         break;

      case MLST_COMMENT2:
         if (c == '>')
            st = me->stData;
         else if (c != '-')
            st = MLST_COMMENT;
         break;
      }
   }
   me->st = st;
}


// Cleanly terminate parsing
//
void DoxParse_Terminate(DoxParse *me)
{
   int cb;
   char *pcIn, *pcOut, *pcMax;
   int bSpace;
   char abSpace[MAXSTACKSIZE];
   uint8 fDisp;

   // Cancel entity or 

   if (me->st != MLST_PCDATA && me->st != MLST_CDATA && me->st != MLST_RCDATA) {
      DoxParse_Write(me, "", 1);
   }

   // Close open elements

   while (me->cntStack) {
      --me->cntStack;
      OUTCHAR( DOX_EE );
   }
   if (me->st == MLST_FULL) {
      xDox_ErrorSet(me->pdox, DOX_ERROR_TRUNC);
   }

   // Handle spaces, CRLF->LF, LF after open/before close
   //      (and NBSP -> 32 when we have entities)

   pcIn = me->pchTerm;
   pcOut = pcIn;
   pcMax = me->pchOut;
   bSpace = 1;
   fDisp = 0;           // current element

   if (pcMax[0] != DOX_EE)
      pcMax[0] = DOX_EE;

   while (pcIn < pcMax) {
      char c = *pcIn;

      if (c == DOX_SE) {

         // SE : Enter element
         
         const char *pcNext = (char *) xDoxNextItem(pcIn, pcMax);

         if (me->cntStack < MAXSTACKSIZE)
            abSpace[me->cntStack] = bSpace;
         DoxParse_Push(me, fDisp);   // push old element
         
         // Inherit 'spaced' attribute
         fDisp = (fDisp & ELEMDISP_SPACED) | acmHtmlElems[DoxGetID(pcIn[1])].fDisp;

         if (fDisp & ELEMDISP_BLOCK)
            bSpace = 1;

         while (pcIn < pcNext)
            *pcOut++ = *pcIn++;

         // Skip LF after open tag
         if (*pcIn == LFCHAR)
            ++pcIn;
         
      } else if (c == DOX_EE) {
         
         // EE : Exit element

         *pcOut++ = DOX_EE;
         pcIn++;
         
         // close this element
         if (fDisp & ELEMDISP_BLOCK) {
            bSpace = 1;
         } else if (fDisp & ELEMDISP_IGNORE) {
            if (me->cntStack <= MAXSTACKSIZE)
               bSpace = abSpace[me->cntStack-1];
         } else if (fDisp & ELEMDISP_OBJECT) {
            bSpace = 0;
         }

         fDisp = DoxParse_Pop(me);

      } else {

         if (fDisp & ELEMDISP_SPACED) {
            // Copy text, retaining spaces and LF
            do {
               *pcOut++ = c;
               c = *++pcIn;
            } while (!DoxIsElemDelim(c));

            // Skip LF before close tag
            if (c == DOX_EE && pcOut[-1] == LFCHAR)
               --pcOut;
            
         } else {
            // Copy Text, collapsing spaces

            for (;;) {
               // Copy non-spaces
               if (((unsigned char)*pcIn) > 32) {
                  do {
                     *pcOut++ = *pcIn++;
                  } while (((unsigned char)*pcIn) > 32);
                  bSpace = 0;
               }

               if (CHARISIN((c = *pcIn), TABCHAR, 32)) {

                  if (c == LFCHAR && pcIn[1] == DOX_EE) {
                     // Skip LF before close tag
                     // This only matters when there is a single LF between non-space and EE
                     ++pcIn;
                     break;
                  }

                  if (!bSpace)
                     *pcOut++ = 32;
                  do {
                     ++pcIn;
                  } while (CHARISIN((c = *pcIn), 9, 32));
                  bSpace = 1;
               } else {
                  if (DoxIsElemDelim(*pcIn))
                     break;
                  ++pcIn;   // skip control character
               }
            }
         }
      }
   }
   
   
   // Update Dox

   cb = pcOut - me->pdox->pcBuf;

   if (pcOut[0] != DOX_EE)
      pcOut[0] = DOX_EE;

   me->pchOut = pcOut;
   me->pchTerm = pcOut;

   if (cb != me->pdox->cbData) {
      me->pdox->cbData = cb;

      xDox_Invalidate(me->pdox, DOX_INVALWRAP);
   }
}


void DoxParse_Dtor(DoxParse *me)
{
   // Nothing to delete/release
}



//-- Dox Functions ---------------------------------------------------------
//


// Return a specific name from a list of names.
//
// On entry:
//   pszNames = szElemNames[] or szAttrNames[]
//   ndx = element/attribute ID (0-based index)
//
static const char *DoxGetName(Dox *me, int ndx, const char *pszNames)
{
	char *pd = me->szName;
	const char *psz = pszNames;

	while (ndx != 0 && *psz) {
		while (*psz != ',')
			++psz;
		++psz;
		--ndx;
	}

	if (*psz) {
		while (*psz != ',') {
			*pd++ = *psz++;
		}
		*pd = 0;
		return me->szName;
	} else {
		return "";
	}
}


const char *Dox_GetAttrName(Dox *me, int nID)
{
   return DoxGetName(me, nID, szAttrNames);
}

const char *Dox_GetElemName(Dox *me, int nID)
{
   return DoxGetName(me, nID, szElemNames);
}


int32 Dox_GetAttrID(Dox *me, const char *pszAttr)
{
   return DoxFindName(szAttrNames, pszAttr, FINDNAME_IGNCASE);
}

int32 Dox_GetElemID(Dox *me, const char *pszElem)
{
   return DoxFindName(szElemNames, pszElem, FINDNAME_IGNCASE);
}

int32 Dox_GetAttrString(Dox *me, int nElem, const char *pszAttr, char *pchBuf, int cbBuf)
{
	int nAttrID = 0;
	const char * pszAttrValue = 0;

   // see if we are on an element
   if(DOX_SE != me->pcBuf[nElem]){
      return HVM_BADELEMENT;
   }
	// retrieve the id of this attribute
	nAttrID = Dox_GetAttrID(me, pszAttr);
   if(FINDNAME_NOMATCH == nAttrID){
      return HVM_BADATTR;
   }

	pszAttrValue = xDoxGetAttrString(me->pcBuf+nElem, nAttrID, 0);

   if(NULL == pszAttrValue){
      return HVM_NOTFOUND;
   }else if(DOX_NV == pszAttrValue[0]){
      return HVM_NOVALUE;
   }else{
      STRLCPY(pchBuf, pszAttrValue, (unsigned int)cbBuf);
      return SUCCESS;
   }
}

//
//----  Attribute parsing
//


int DoxGetInt(const char *psz, int nDefault)
{
   int n = 0;

   // For DOX_NV and empty string, return nDefault.  Other cases?
   
   if (psz && *psz > SPACECHAR) {
      while (CHARISIN(*psz, '0', '9')) {
         n = n*10 + *psz - '0';
         ++psz;
      }
      return n;
   }
   return nDefault;
}
   

int DoxGetAttrInt(const char *pch, int nAttrID, int nDefault)
{
   const char *psz = xDoxGetAttrValue(pch, nAttrID);

   if (psz)
      nDefault = DoxGetInt(psz, nDefault);
   return nDefault;
}

// Return hex value of character, or 16 if not a valid hex character.
//
static RGBVAL DoxGetColor(const char *psz, RGBVAL rgbDefault)
{
   RGBVAL rgb = 0;
   int nShift = 28;
   int val;
   
   if (psz) {
      if (*psz == '#' ) {
         while (nShift >= 8
                && (val = xHexValue(*++psz)) < 16) {
            rgb |= val << nShift;
            nShift -= 4;
         }
         if (nShift >= 16) {
            // one, two, or three digits => one digit per component

            // Convert to BREW format            Start with:  rgb.....
            rgb = (rgb >> 16) | (rgb >> 4) | (rgb << 8);  //  brgbrgb.
            rgb &= 0xf0f0f000;                            //  b.g.r...
            rgb |=  (rgb >> 4);                           //  bbggrr..
         } else {
            // four, five of six => two digits per component

            // convert from "rrggbb.." to  "bbggrr.."
            rgb = (rgb & 0xFF000000) >> 16 | (rgb & 0x00FF0000) | (rgb << 16);
         }
      } else if (ISALPHA(*psz)) {
         int ndx = DoxFindName(szColorNames, psz, FINDNAME_IGNCASE);
         if (ndx != FINDNAME_NOMATCH) {
            return argbColorValues[ndx];
         }
      }
   }

   if (nShift >= 20) {
      // 0, 1, or 2 digits
      rgb = rgbDefault;
   }
   return rgb;
}


RGBVAL DoxGetAttrColor(const char *pc, int nAttrID, RGBVAL rgbDefault)
{
   const char *psz = xDoxGetAttrValue(pc, nAttrID);

   if (psz)
      return DoxGetColor(psz, rgbDefault);
   else
      return rgbDefault;
}


// Parse HTML document from buffer.
//
// On entry:
//   pszIn = zero-terminated string containing HTML
//   pchOut, cbOut = output buffer & size limit
//
// On exit:
//   pchOut[] = parsed HTML in internal representation
//   return value = size of result
//
int Dox_ParseString(Dox *me, const char *pszIn, int cbLen)
{
   // char *pchOut, int cbOut, const char *pszIn)

   DoxParse dp;

   if (cbLen < 0)
      cbLen = STRLEN(pszIn);

   DoxParse_Ctor(&dp, me);

   DoxParse_Write(&dp, pszIn, cbLen);

   DoxParse_Terminate(&dp);

   DoxParse_Dtor(&dp);

   return xDox_BufGetSize(me);
}

#undef DOX_UNITTEST
#ifdef DOX_UNITTEST


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//   Unit Tests
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


  
typedef struct ParseTestVector {
   const char *a;
   const char *b;
} ParseTestVector;



//////////////////////////////////////////////////////////////////////////
//
// ptvEqHTML contains pairs of samples for the parser. the results of each
// pair are expected to match.  Sample can be HTML, or if they begin with "="
// a special syntax defined below:
//
//   content  ::=  text | elemdesc
//   elemdesc ::=  "{" elem attrspec* ("|" content)? "}"
//   attrspec ::=  ":" attr ("=" val)?
//
// This is translated to the internal structure thus:
//
//    {         -> SE
//    elem      -> ELEMID(elem)
//    attr=val  -> ATTRID(attr) val \0
//    attr      -> ATTRID(attr) DOX_NV       [no '=val' before ":" or "|" or "}"]
//    |         -> CD
//    }         -> EE
//    text      -> text
//
// Example:
//
//    {a:href=foo|xxx}   ->  SE IDChar(ELEM_A) IDChar(ATTR_HREF) "foo" \0 CD "xxx" EE
//
//------------------------------------------------------------
//
// Parser objectives:
//
//  1. Handle all well-formed HTML as per HTML4.01 spec, except where IE
//     diverges, and where standard compliance could interfere with
//     compatibility.  (It is expected that any such divergence will be
//     minimal, and in cases that are discouraged by the standard, anyway.)
//
//  2. For illegal HTML, follow behavior HTML browsers as a model as 
//     much as is reasonable.
//
static ParseTestVector ptvEqHTML[] = {
   
   // -- Whitespace handling : conversion and elimination of duplicate
   // characters occurs during parsing, so layout and display do not
   // have to treat spaces specially.
   
   "a b",         "a  b",
   "a b",         "a   b",
   "a b",         "a\011 b",   // TAB (9)
   "a b",         "a\012 b",   // LF  (10)
   "a b",         "a\013 b",   // VT  (11)
   "a b",         "a\014 b",   // FF  (12)
   "a b",         "a\015 b",   // CR  (13)
   "a b",         "a \015b",
   "a b",         "a\012\015b",
   "a b",         "a\015\012b",
   
   // Elements that introduce vertical whitespace: initial spaces
   // should be trimmed.
   "a<p>b</p>",     "a<p> b</p>",
   "a<h1>b</h1>",   "a<h1> b</h1>",
   
   // "&#32;" same as space for eliminating duplicate space characters
   "a&#32;b",       "a &#32; b",

   // Basic parsing
   "<hr><hr><hr>",                 "={hr}{hr}{hr}",
   "a<hr><hr>b",                   "=a{hr}{hr}b",

   // Elements whose content is not displayed should not affect
   // space-elimination.
   "<title>a</title>b",            "<title>a</title> b",
   "<title>a</title><p>b</p>",     "<title>a</title> <p>b</p>",

   "<script>a</script>b",          "<script>a</script> b",
   "x<script>a </script> b",       "=x{script|a } b",
   "x<script>a </script>b",        "=x{script|a }b",
   
   // Elements where spaces should be preserved.  Elements nested
   // inside should not affect them.

   "<b>1<i>2</i>3</b>4",           "={b|1{i|2}3}4",
   "<script>a</script>",           "={script|a}",
   "<pre> a  b </pre>",            "={pre| a  b }",
   "<pre> a <b> b  c </b> </pre>", "={pre| a {b| b  c } }",
   "<pre>a\n\tb</pre>",            "={pre|a\n\tb}",
   "<pre>a\nb\r\n</pre>",          "={pre|a\nb}",         // LF before close tag should be ignored
   "<textarea> a  b </textarea>",  "={textarea| a  b }",
   "<textarea>a \n b</textarea>",  "={textarea|a \n b}",

   // Misc space collapsing tests
   "a <img> b",                    "=a {img} b",
   "a <input> b",                  "=a {input} b",
   "a <select> </select> b",       "=a {select|} b",
   "a <textarea> </textarea> b",   "=a {textarea| } b",

   // Whitespace and control character handling
   //   tab, lf, ...  -> space
   "a\nb\tc d\001\020e",           "a b c de",

   // "<script> a </script>",         "={script| a }",      // instead we see "={script|a }"

   // Attributes

   "<meta http-equiv=a content='x'>",    "={meta:http-equiv=a:content=x}",
   "<p class=a align=\"r\">test</p>",    "={p:class=a:align=r|test}",

   "<img alt = a>",                      "={img:alt=a}",
   "<img src alt=a>",                    "={img:src:alt=a}",
   "<img src alt=>",                     "={img:src:alt}",


   
   // RCDATA content: handle at higher level, not in parser...
   //   "<form><select><option>a<img>b",       "={form|{select|{option|ab}}}",

   // Invalid attribute characters
   
   "<a href=a=b>x</a>",                  "={a:href=a=b|x}",
   "<a href=a\"b>x</a>",                 "={a:href=a\"b|x}",
   "<a href=a<b>x</a>",                  "={a:href=a<b|x}",
   "<a href=a/>x</a>",                   "={a:href=a/|x}",
   "<a href=a?>x</a>",                   "={a:href=a?|x}",

   // Invalid "<" and "&" occurrences

   "a < b",                              "=a < b",
   "a << b",                             "=a << b",
   "a <& b",                             "=a <& b",
   "a <<b>c",                            "=a <{b|c}",
   "a </<b>c",                           "=a c",
   "a & b",                              "=a & b",
   "a && b",                             "=a && b",
   "a&&amp;",                            "=a&&",
   "<img src='&lt'>",                    "={img:src=<}",
   "<img src='&'>",                      "={img:src=&}",

   // Either are okay:
   "<img<b>>a</b>",                      "={img}>a",   // Mozilla
   // "<img<b>>a</b>",                      "=a",         // IE

   // Invalid elements

   "a<wq>b",                             "=ab",
   "a</wq>b",                            "=ab",

   // Discard SGML comments
   //    Non-standard handling as per IE

   "a<!-- comment -->z",                 "=az",
   "a<!-- comment -> --> -->",           "=a -->",
   "a<!-->b-->",                         "=ab-->",
   "a<!--->b-->",                        "=ab-->",
   "a<!---->b-->",                       "=ab-->",
   "a<!----->b-->",                      "=ab-->",
   "a<!------>b-->",                     "=ab-->",

   // Ignore DOCTYPE
   
   "<!DOCTYPE doc SYSTEM \"doc.dtd\">z",               "z",

   // XML declaration/processing instructions
   //
   //   - Recognize "?xml" declaration as an empty element named "?xml"
   //   - Handle "?>" with or without whitespace and at end of PI or decl name
   //   - Ignore unknown PIs
   // 29b

   "<?xml version=\"1.0\" encoding=\"utf-8\" ?>z",     "={?xml:version=1.0:encoding=utf-8}z",
   //   "<?xml ?><?xml?>z",                                 "={?xml}{?xml}z",
   "<?xml version=\"1.0\"?>z",                         "={?xml:version=1.0}z",
   "a<?pi?><?pi ?><?p a=\"x\"?><?p a=\"y\" ?>z",       "=az",

   // CDATA content
   //   XMP:       entities, elements, and SGML comments literal
   //   TEXTAREA:  entities processed;  elements and comments literal

   "<textarea><a></textarea>",          "={textarea|<a>}",
   "<textarea><!--test--></textarea>",  "={textarea|<!--test-->}",
   "<textarea>a &amp; b</textarea>",    "={textarea|a & b}",
   "<xmp>a<b>c</b><!-- --></xmp>",      "={xmp|a<b>c</b><!-- -->}",

   // Full XMP compatibility not supported; counter to standard anyway
   //   "<xmp>&amp;</xmp>",                  "={xmp|&amp;}",


   // "NODATA" content
   //    Browsers now recognize content in TABLE

   "<table>a</table>",                   "={table|a}",

   // Element nesting
   //
   //  - Properly handle properly nested XHTML
   //  - Provide missing close tags at end of document
   //  - Nest P within DIV
   
   "<select><option>a</option><option>b</option></select>",  "={select|{option|a}{option|b}}",
   "<ul><li>1</li><li>2</li></ul>",                          "={ul|{li|1}{li|2}}",
   "<div>1<p>2",                                             "={div|1{p|2}}",

   // Mis-matched open & close tags

   "<b>a<i>b</b>c</b>",                               "={b|a{i|b}}c",
   "<select><option>a<option>b</select>",             "={select|{option|a}{option|b}}",
   "<ul><li>1</ul>",                                  "={ul|{li|1}}",
   "<ul><li>1<li>2</ul>",                             "={ul|{li|1}{li|2}}",
   "<p>a</b>b</p>",                                   "={p|ab}",
   "<b>a<table><tr><td>b</b>c</table>c</b>d",         "={b|a{table|{tr|{td|bc}}}c}d",

   // Duplicate non-nesting block-level elements
   
   "<p>1<p>2",                                        "={p|1}{p|2}",
   "<p>1<div>2",                                      "={p|1}{div|2}",
   "<blockquote>1<blockquote>2",                      "={blockquote|1{blockquote|2}}",
   "<select><option>a<option>b</option></select>",    "={select|{option|a}{option|b}}",

   // Entities
   //   (Multi-byte entities will require additional test code support, as
   //   they may vary from encoding to encoding.)

   "&lt;&amp;&quot;&gt;&LT;&AMP;&QUOT;&GT;",          "=<&\"><&\">",
   "a&gt;b",                                          "=a>b",

   // Character references
   //
   //   - Recognize decimal and hex character references from 0x0A through 0xFFFF

   "&#65;&#x42;&#xa;&#x7e;",            "=AB \176",

   // mal-formed character references

   "&#99 a &#99b &#99.2",            "=c a cb c.2",              // decimal: terminate at non-digit
   "&#97&#98<br>&#99",               "=ab{br}c",                 //   also check special characters
   "&# &#foo",                       "=&# &#foo",                // garbage: don't translate

   // Either are okay:
   "&#x63 &#x63g &#x63&gt;",         "=c cg c>",                 // Mozilla: translate
   //"&#x63 &#x63g &#x63&gt;",         "=&#x63 &#x63g &#x63>",     // IE: require ";"

   // THESE TESTS ONLY WORK IN UTF-8 SKINS!

   "&#xFF;",      "=\303\277",       // 00FF = 1111 1111           -> 110.00011  10.111111
   "&#x2022;",    "=\342\200\242",   // 2022 = 0010 0000 0010 0010 -> 1110.0010 10.000000 10.100010
   "&#8226;",     "=\342\200\242",   // 2022 = 0010 0000 0010 0010 -> 1110.0010 10.000000 10.100010
   
   // XML tagends
   //
   //   - Recognize <TAGNAME/> when representing an HTML empty element
   //   - Recognize <TAGNAME/> when representing an HTML "non-empty" element that has no content
   //   - Treat <tag attr=val/> as <tar attr="val/"> for HTML compatibility
   //   - Treat <tag attr="val"/> as slash tagend

   "<br/>a<hr>b<style/>c",                            "={br}a{hr}b{style|}c",
   "<img src=\"f\"/>a",                               "={img:src=f}a",

   // XML CDATA
   
   //"a<!CDATA[[]]>z",                                  "=az",
   //"a<![CDATA[v>\"0\" && v<\"10\" ?1:0]]>z",          "=av>\"0\" && v<\"10\" ?1:0]]>z",

   // XML spacing:preserve ?

};



static int xFindName(const char *pszNames, const char **ppsz, const char *pszDelims, int flags)
{
   char buf[32];
   const char *psz = *ppsz;
   const char *pszEnd = STRCHRSEND(psz, pszDelims);
   int cb = DoxConstrain(pszEnd - psz, 0, sizeof(buf)-1);

   MEMCPY(buf, psz, cb);
   buf[cb] = 0;
   
   *ppsz = pszEnd;
   return DoxFindName(pszNames, buf, FINDNAME_IGNCASE);
}


// Obtain simplified (internal) format, given HTML or special syntax.
// 
static int xParseStr(Dox *pDox, char *pcDest, int cbDest, const char *pszSrc)
{
   const char *psz = pszSrc;
   char ch;
   char *pc = pcDest;
   
   // Parse HTML
   
   if (*pszSrc != '=') {
      int cb;
      
      xDox_Reset(pDox, 0);
      cb = Dox_ParseString(pDox, pszSrc, -1);
      MEMCPY(pcDest, pDox->pcBuf, cb);

      if (cb < cbDest) {
         pcDest[cb] = 0;  // terminate for readability in the debugger
      }
      return cb;
   }


   // Parse {elem|content} form

   ++psz;

   while ((ch = *psz++) != 0) {
      if (ch == '{') {

         // Get element ID
         int nTag = xFindName(szElemNames, &psz, ":|}", FINDNAME_IGNCASE);
         if (nTag < 0) {
            return 0;
         }

         *pc++ = DOX_SE;
         *pc++ = DoxIDChar(nTag);

         while (*psz == ':') {
            int nAttr;

            // Read attr

            ++psz;
            nAttr = xFindName(szAttrNames, &psz, "=:|}", FINDNAME_IGNCASE);
            if (nAttr < 0) {
               return 0;
            }
            *pc++ = DoxIDChar(nAttr);

            if (*psz == '=') {
               // val = zero-terminated string
               ++psz;
               while (*psz && *psz != ':' && *psz != '|' && *psz != '}') {
                  *pc++ = *psz++;
               }
               *pc++ = 0;
            } else {
               // "no-value" attrivute
               *pc++ = DOX_NV;
            }
         }

      } else if (ch == '}') {
         *pc++ = DOX_EE;
      } else if (ch == '|') {
         *pc++ = DOX_CD;
      } else {
         *pc++ = ch;
      }
   }

   if (pc < pcDest + cbDest) {
      pc[0] = 0;   // terminate for readability in the debugger
   }

   return pc - pcDest;
}


static const char * TestParse(Dox *pDox)
{
   ParseTestVector *ptv = ptvEqHTML;
   int i, cntErrors = 0;
   char bufA[80];
   char bufB[80];

   for (i = 0; i < ARRAY_SIZE(ptvEqHTML); ++i, ++ptv) {
      
      int cbA = xParseStr(pDox, bufA, sizeof(bufA), ptv->a);
      int cbB = xParseStr(pDox, bufB, sizeof(bufB), ptv->b);

      if (cbA != cbB || MEMCMP(bufA, bufB, cbA) != 0) {
         DBGPRINTF("  FAIL %d [%s]", i, ptv->a);
         ++cntErrors;
      }
   }

   if (cntErrors) {
      return "logged errors";
   }

   return 0;
}


static const char *TestFindName(void)
{
   static struct {
      const char *psz;
      int         id;
   } tv[] = {
      szElemNames,         9999,
      "?xml",              ELEM_XMLDECL,
      "a",                 ELEM_A,
      "address",           ELEM_ADDRESS,
      "applet",            ELEM_APPLET,
      "area",              ELEM_AREA,
      "b",                 ELEM_B,
      "base",              ELEM_BASE,
      "big",               ELEM_BIG,
      "blockquote",        ELEM_BLOCKQUOTE,
      "body",              ELEM_BODY,
      "br",                ELEM_BR,
      "caption",           ELEM_CAPTION,
      "center",            ELEM_CENTER,
      "cite",              ELEM_CITE,
      "code",              ELEM_CODE,
      "dd",                ELEM_DD,
      "dfn",               ELEM_DFN,
      "dir",               ELEM_DIR,
      "div",               ELEM_DIV,
      "dl",                ELEM_DL,
      "dt",                ELEM_DT,
      "em",                ELEM_EM,
      "font",              ELEM_FONT,
      "form",              ELEM_FORM,
      "h1",                ELEM_H1,
      "h2",                ELEM_H2,
      "h3",                ELEM_H3,
      "h4",                ELEM_H4,
      "h5",                ELEM_H5,
      "h6",                ELEM_H6,
      "head",              ELEM_HEAD,
      "hr",                ELEM_HR,
      "html",              ELEM_HTML,
      "i",                 ELEM_I,
      "img",               ELEM_IMG,
      "input",             ELEM_INPUT,
      "isindex",           ELEM_ISINDEX,
      "kbd",               ELEM_KBD,
      "label",             ELEM_LABEL,
      "li",                ELEM_LI,
      "link",              ELEM_LINK,
      "map",               ELEM_MAP,
      "menu",              ELEM_MENU,
      "meta",              ELEM_META,
      "ol",                ELEM_OL,
      "option",            ELEM_OPTION,
      "p",                 ELEM_P,
      "param",             ELEM_PARAM,
      "pre",               ELEM_PRE,
      "samp",              ELEM_SAMP,
      "script",            ELEM_SCRIPT,
      "select",            ELEM_SELECT,
      "small",             ELEM_SMALL,
      "strike",            ELEM_STRIKE,
      "strong",            ELEM_STRONG,
      "style",             ELEM_STYLE,
      "sub",               ELEM_SUB,
      "sup",               ELEM_SUP,
      "table",             ELEM_TABLE,
      "td",                ELEM_TD,
      "textarea",          ELEM_TEXTAREA,
      "textflow",          ELEM_TEXTFLOW,
      "th",                ELEM_TH,
      "title",             ELEM_TITLE,
      "tr",                ELEM_TR,
      "tt",                ELEM_TT,
      "u",                 ELEM_U,
      "ul",                ELEM_UL,
      "var",               ELEM_VAR,
      "xmp",               ELEM_XMP,

      "ab",                -1,
      "ax",                -1,
      "A",                 ELEM_A,
      "XMP",               ELEM_XMP,
      "",                  -1,
      "are",               -1,

      szAttrNames, 9999,

      "accesskey",         ATTR_ACCESSKEY,
      "action",            ATTR_ACTION,
      "align",             ATTR_ALIGN,
      "alink",             ATTR_ALINK,
      "alt",               ATTR_ALT,
      "background",        ATTR_BACKGROUND,
      "bgcolor",           ATTR_BGCOLOR,
      "border",            ATTR_BORDER,
      "checked",           ATTR_CHECKED,
      "class",             ATTR_CLASS,
      "clear",             ATTR_CLEAR,
      "color",             ATTR_COLOR,
      "cols",              ATTR_COLS,
      "compact",           ATTR_COMPACT,
      "content",           ATTR_CONTENT,
      "encoding",          ATTR_ENCODING,
      "enctype",           ATTR_ENCTYPE,
      "for",               ATTR_FOR,
      "height",            ATTR_HEIGHT,
      "href",              ATTR_HREF,
      "href2",             ATTR_HREF2,
      "hspace",            ATTR_HSPACE,
      "http-equiv",        ATTR_HTTP_EQUIV,
      "id",                ATTR_ID,
      "ismap",             ATTR_ISMAP,
      "istyle",            ATTR_ISTYLE,
      "language",          ATTR_LANGUAGE,
      "link",              ATTR_LINK,
      "maxlength",         ATTR_MAXLENGTH,
      "method",            ATTR_METHOD,
      "multiple",          ATTR_MULTIPLE,
      "name",              ATTR_NAME,
      "noshade",           ATTR_NOSHADE,
      "prompt",            ATTR_PROMPT,
      "rel",               ATTR_REL,
      "rev",               ATTR_REV,
      "rows",              ATTR_ROWS,
      "selected",          ATTR_SELECTED,
      "size",              ATTR_SIZE,
      "src",               ATTR_SRC,
      "style",             ATTR_STYLE,
      "text",              ATTR_TEXT,
      "title",             ATTR_TITLE,
      "type",              ATTR_TYPE,
      "usemap",            ATTR_USEMAP,
      "value",             ATTR_VALUE,
      "version",           ATTR_VERSION,
      "vlink",             ATTR_VLINK,
      "vspace",            ATTR_VSPACE,
      "width",             ATTR_WIDTH,

      gszEntityNames, 9998,

      "amp",              38,
      "gt",               62,
      "lt",               60,
      "quot",             34,
      "AMP",              38,
      "GT",               62,
      "LT",               60,
      "QUOT",             34,
      "apos",             39,
      "AElig",            198,
      "Aacute",           193,
      "Acirc",            194,
      "Agrave",           192,
      "Alpha",            913,
      "Aring",            197,
      "Atilde",           195,
      "Auml",             196,
      "aacute",           225,
      "acirc",            226,
      "acute",            180,
      "aelig",            230,
      "agrave",           224,
      "alefsym",          8501,
      "alpha",            945,
      "and",              8743,
      "ang",              8736,
      "aring",            229,
      "asymp",            8776,
      "atilde",           227,
      "auml",             228,
      "Beta",             914,
      "bdquo",            8222,
      "beta",             946,
      "brvbar",           166,
      "bull",             8226,
      "Ccedil",           199,
      "Chi",              935,
      "cap",              8745,
      "ccedil",           231,
      "cedil",            184,
      "cent",             162,
      "chi",              967,
      "circ",             710,
      "clubs",            9827,
      "cong",             8773,
      "copy",             169,
      "crarr",            8629,
      "cup",              8746,
      "curren",           164,
      "Dagger",           8225,
      "Delta",            916,
      "dArr",             8659,
      "dagger",           8224,
      "darr",             8595,
      "deg",              176,
      "delta",            948,
      "diams",            9830,
      "divide",           247,
      "ETH",              208,
      "Eacute",           201,
      "Ecirc",            202,
      "Egrave",           200,
      "Epsilon",          917,
      "Eta",              919,
      "Euml",             203,
      "eacute",           233,
      "ecirc",            234,
      "egrave",           232,
      "empty",            8709,
      "emsp",             8195,
      "ensp",             8194,
      "epsilon",          949,
      "equiv",            8801,
      "eta",              951,
      "eth",              240,
      "euml",             235,
      "euro",             8364,
      "exist",            8707,
      "fnof",             402,
      "forall",           8704,
      "frac12",           189,
      "frac14",           188,
      "frac34",           190,
      "frasl",            8260,
      "Gamma",            915,
      "gamma",            947,
      "ge",               8805,
      "hArr",             8660,
      "harr",             8596,
      "hearts",           9829,
      "hellip",           8230,
      "Iacute",           205,
      "Icirc",            206,
      "Igrave",           204,
      "Iota",             921,
      "Iuml",             207,
      "iacute",           237,
      "icirc",            238,
      "iexcl",            161,
      "igrave",           236,
      "image",            8465,
      "infin",            8734,
      "int",              8747,
      "iota",             953,
      "iquest",           191,
      "isin",             8712,
      "iuml",             239,
      "Kappa",            922,
      "kappa",            954,
      "Lambda",           923,
      "lArr",             8656,
      "lambda",           955,
      "lang",             9001,
      "laquo",            171,
      "larr",             8592,
      "lceil",            8968,
      "ldquo",            8220,
      "le",               8804,
      "lfloor",           8970,
      "lowast",           8727,
      "loz",              9674,
      "lrm",              8206,
      "lsaquo",           8249,
      "lsquo",            8216,
      "Mu",               924,
      "macr",             175,
      "mdash",            8212,
      "micro",            181,
      "middot",           183,
      "minus",            8722,
      "mu",               956,
      "Ntilde",           209,
      "Nu",               925,
      "nabla",            8711,
      "nbsp",             160,
      "ndash",            8211,
      "ne",               8800,
      "ni",               8715,
      "not",              172,
      "notin",            8713,
      "nsub",             8836,
      "ntilde",           241,
      "nu",               957,
      "OElig",            338,
      "Oacute",           211,
      "Ocirc",            212,
      "Ograve",           210,
      "Omega",            937,
      "Omicron",          927,
      "Oslash",           216,
      "Otilde",           213,
      "Ouml",             214,
      "oacute",           243,
      "ocirc",            244,
      "oelig",            339,
      "ograve",           242,
      "oline",            8254,
      "omega",            969,
      "omicron",          959,
      "oplus",            8853,
      "or",               8744,
      "ordf",             170,
      "ordm",             186,
      "oslash",           248,
      "otilde",           245,
      "otimes",           8855,
      "ouml",             246,
      "Phi",              934,
      "Pi",               928,
      "Prime",            8243,
      "Psi",              936,
      "para",             182,
      "part",             8706,
      "permil",           8240,
      "perp",             8869,
      "phi",              966,
      "pi",               960,
      "piv",              982,
      "plusmn",           177,
      "pound",            163,
      "prime",            8242,
      "prod",             8719,
      "prop",             8733,
      "psi",              968,
      "Rho",              929,
      "rArr",             8658,
      "radic",            8730,
      "rang",             9002,
      "raquo",            187,
      "rarr",             8594,
      "rceil",            8969,
      "rdquo",            8221,
      "real",             8476,
      "reg",              174,
      "rfloor",           8971,
      "rho",              961,
      "rlm",              8207,
      "rsaquo",           8250,
      "rsquo",            8217,
      "Scaron",           352,
      "Sigma",            931,
      "sbquo",            8218,
      "scaron",           353,
      "sdot",             8901,
      "sect",             167,
      "shy",              173,
      "sigma",            963,
      "sigmaf",           962,
      "sim",              8764,
      "spades",           9824,
      "sub",              8834,
      "sube",             8838,
      "sum",              8721,
      "sup1",             185,
      "sup2",             178,
      "sup3",             179,
      "sup",              8835,
      "supe",             8839,
      "szlig",            223,
      "THORN",            222,
      "Tau",              932,
      "Theta",            920,
      "tau",              964,
      "there4",           8756,
      "theta",            952,
      "thetasym",         977,
      "thinsp",           8201,
      "thorn",            254,
      "tilde",            732,
      "times",            215,
      "trade",            8482,
      "Uacute",           218,
      "Ucirc",            219,
      "Ugrave",           217,
      "Upsilon",          933,
      "Uuml",             220,
      "uArr",             8657,
      "uacute",           250,
      "uarr",             8593,
      "ucirc",            251,
      "ugrave",           249,
      "uml",              168,
      "upsih",            978,
      "upsilon",          965,
      "uuml",             252,
      "weierp",           8472,
      "Xi",               926,
      "xi",               958,
      "Yacute",           221,
      "Yuml",             376,
      "yacute",           253,
      "yen",              165,
      "yuml",             255,
      "Zeta",             918,
      "zeta",             950,
      "zwj",              8205,
      "zwnj",             8204,
   };

   int i;
   const char *pszList = "";
   int bEnts = 0;

   for (i=0; i < ARRAY_SIZE(tv); ++i) {
      int ndx;
      
      if (tv[i].id > 9990) {
         pszList = tv[i].psz;
         bEnts = (tv[i].id == 9998);
         continue;
      }

      ndx = DoxFindName(pszList, tv[i].psz, (char)(bEnts ? FINDNAME_USECASE : FINDNAME_IGNCASE));

      if (bEnts && ndx >= 0) {
         // match character
         ndx = gawEntityValues[ndx];
      }

      if (ndx != tv[i].id) {
         return tv[i].psz;
      }
   }
   

   return 0;
}


static void LogError(const char *pszTest, const char *pszError)
{
   if (!pszError)
      DBGPRINTF("  SUCCESS: %s", pszTest);
   else
      DBGPRINTF("  ERROR: %s [%s]", pszTest, pszError);
}


// Parser unit tests
//
void Dox_TestParser(Dox *me)
{
   // Test Findname

   LogError("DoxFindName", TestFindName());
   LogError("DoxParse",    TestParse(me));
}

#endif

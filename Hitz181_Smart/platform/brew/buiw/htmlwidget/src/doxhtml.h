// doxhtml.h  : HTML content model


#include "dox.h"


enum HTMLElemID {
   ELEM_NONE = -1,
   ELEM_XMLDECL,
   ELEM_A,
   ELEM_ADDRESS,
   ELEM_APPLET,
   ELEM_AREA,
   ELEM_B,
   ELEM_BASE,
   ELEM_BIG,
   ELEM_BLOCKQUOTE,
   ELEM_BODY,
   ELEM_BR,
   ELEM_CAPTION,
   ELEM_CENTER,
   ELEM_CITE,
   ELEM_CODE,
   ELEM_DD,
   ELEM_DFN,
   ELEM_DIR,
   ELEM_DIV,
   ELEM_DL,
   ELEM_DT,
   ELEM_EM,
   ELEM_FONT,
   ELEM_FORM,
   ELEM_H1,
   ELEM_H2,
   ELEM_H3,
   ELEM_H4,
   ELEM_H5,
   ELEM_H6,
   ELEM_HEAD,
   ELEM_HR,
   ELEM_HTML,
   ELEM_I,
   ELEM_IMG,
   ELEM_INPUT,
   ELEM_ISINDEX,
   ELEM_KBD,
   ELEM_LABEL,
   ELEM_LI,
   ELEM_LINK,
   ELEM_MAP,
   ELEM_MENU,
   ELEM_META,
   ELEM_OL,
   ELEM_OPTION,
   ELEM_P,
   ELEM_PARAM,
   ELEM_PRE,
   ELEM_SAMP,
   ELEM_SCRIPT,
   ELEM_SELECT,
   ELEM_SMALL,
   ELEM_STRIKE,
   ELEM_STRONG,
   ELEM_STYLE,
   ELEM_SUB,
   ELEM_SUP,
   ELEM_TABLE,
   ELEM_TD,
   ELEM_TEXTAREA,
   ELEM_TEXTFLOW,
   ELEM_TH,
   ELEM_TITLE,
   ELEM_TR,
   ELEM_TT,
   ELEM_U,
   ELEM_UL,
   ELEM_VAR,
   ELEM_XMP,
   ELEM_MAX
};

enum HTMLAttrID {
   ATTR_NONE = -1,
   ATTR_ACCESSKEY,
   ATTR_ACTION,
   ATTR_ALIGN,
   ATTR_ALINK,
   ATTR_ALT,
   ATTR_BACKGROUND,
   ATTR_BGCOLOR,
   ATTR_BORDER,
   ATTR_CHECKED,
   ATTR_CLASS,
   ATTR_CLEAR,
   ATTR_COLOR,
   ATTR_COLS,
   ATTR_COMPACT,
   ATTR_CONTENT,
   ATTR_ENCODING,
   ATTR_ENCTYPE,
   ATTR_FOR,
   ATTR_HEIGHT,
   ATTR_HREF,
   ATTR_HREF2,
   ATTR_HSPACE,
   ATTR_HTTP_EQUIV,
   ATTR_ID,
   ATTR_ISMAP,
   ATTR_ISTYLE,
   ATTR_LANGUAGE,
   ATTR_LINK,
   ATTR_MAXLENGTH,
   ATTR_METHOD,
   ATTR_MULTIPLE,
   ATTR_NAME,
   ATTR_NOSHADE,
   ATTR_PROMPT,
   ATTR_REL,
   ATTR_REV,
   ATTR_ROWS,
   ATTR_SELECTED,
   ATTR_SIZE,
   ATTR_SRC,
   ATTR_STYLE,
   ATTR_TEXT,
   ATTR_TITLE,
   ATTR_TYPE,
   ATTR_USEMAP,
   ATTR_VALUE,
   ATTR_VERSION,
   ATTR_VLINK,
   ATTR_VSPACE,
   ATTR_WIDTH,
   ATTR_MAX
};


// Content models
//
// Content models are important to infer the appropriate nesting of elements
// when tags are omitted, as implied by an out-of-place close or open tag.  In
// SGML, rigorous definitions of omitted tag behavior allow notational
// conveniences.  In HTML, end tags are strongly recommended, and handling of
// omitted end tags should be compatible with existing browsers (which is not
// necessarily as defined by SGML?).
//
// The SGML rules used by formal HTML standards are not in themselves
// adequate as many documents are non-compliant and the results are not
// defined by formal specs.
//
// The approach should match the standard behavior for compliant documents,
// and to match existing browser behavior for other cases, when reasonable.
// As a first stab at the problem, here we do the following:
//
//   - "Unexpected" close tags (i.e. tags that do not match the current
//     element) that match open elements somewhere on the stack can imply close
//     tags of the intervening elements.
//
//     In order for this to be applied, the implicitly closed elements must be
//     lower on the hierarchy than the closing element.  The hierarchy is
//     defined by numeric values of the element categories (ELEMCAT_...).
//
//         /P can implicitly close B, I, etc.
//         /TABLE can implicitly close TR and TD
//         /I cannot implicity close P, BLOCKQUOTE, TD or TR
// 
//  - Unexpected open tags (i.e. for elements that are not in the current
//    element's content model) can implicitly close other elements when
//    the element is not allowed in the corresponding context but would be
//    allowed in an ancestor context.
//
//
// Misc things to cover:
//   LI, P terminated by LI, P
//   <LI><P> same as <LI>  -- <P><P> treated as 'first' <P> instead of second <P> ?
//


// Each element belongs to ONE of the following categories:
//
#define CONTENT_DATA       1    // can contain data (text)
#define CONTENT_INLN       2    // inline elements and text (can be included is most other elements)
#define CONTENT_PARG       4    // P, H1..6, ADDRESS, CENTER, ...
#define CONTENT_NEST       8    // DIV, BLCKQUOTE, LI, DD, DT, ...
#define CONTENT_LIST      16    // UL, OL, DL, MENU, ...
#define CONTENT_TD        32    // can occur only inside TR
#define CONTENT_TR        64    // can only occur inside TABLE


// Each element can contain other elements as described by the following sets:
//
//  Note:  Empty elements can contain NO elements and NO text.
//         empty element => no bits are set
//
//  Note:  Parent elements will be closed until this is satisfied!!!  That is why
//         TABLE allows P below, although technically it doesn't.  Inferred open
//         tags require a different approach.
//
#define ELEMCONT_NONE      0                                  // empty element
#define ELEMCONT_DATA      CONTENT_DATA
#define ELEMCONT_INLN     (CONTENT_DATA | CONTENT_INLN)
#define ELEMCONT_NEST     (CONTENT_DATA | CONTENT_INLN | CONTENT_PARG | CONTENT_NEST | CONTENT_LIST)
#define ELEMCONT_TR       (ELEMCONT_NEST | CONTENT_TD)
#define ELEMCONT_TBL      (ELEMCONT_TR | CONTENT_TR)
#define ELEMCONT_ALL      (ELEMCONT_TBL)

typedef unsigned char ElemCat;   // element category



// Style flags
//
// These flags are designed so that '1' bits override inherited values, so
// a nested element's style is decribed by (outerflags | elementflags), plus
// some other flags that are not simple element properties.  Some flags can
// also (or only) be specified by attributes.  Some examples:
//
//   ELEMFLAG_LINK: set by <A>'s that have ATTR_HREF values.
//   ELEMFLAG_ALIGNCENTER: property of <CENTER>, but also set ALIGN=CENTER
//   ELEMFLAG_BIG: property of ELEM_H1..6, but also set by <FONT SIZE=4..7>
//
//

#define ELEMFLAG_NONE           0
#define ELEMFLAG_TT             0   // not currently supported
#define ELEMFLAG_I              0   // not currently supported

#define ELEMFLAG_B              1
#define ELEMFLAG_BIG            2
#define ELEMFLAG_U              4


#define ELEMFLAG_ALIGNCENTER    16
#define ELEMFLAG_ALIGNRIGHT     32
#define ELEMFLAG_ALIGNMENT      48

#define ELEMFLAG_LINK           64    // linked text


#define ESF_SetAlign(f,a)      ((f) = (((f) & ~ELEMFLAG_ALIGNMENT) | (a)))


// Display flags
//
// These flags affect processing for display, but aren't inherited properties
// of elements.
//

#define ELEMDISP_NONE       0    // inline element
#define ELEMDISP_IGNORE     1    // contents should not be displayed
#define ELEMDISP_OBJECT     2    // try to instantiate an inline object
#define ELEMDISP_BLOCK      4    // this is a block element

#define ELEMDISP_VSPACE     8    // includes a vertical gap before & after
#define ELEMDISP_HSPACE    16    // indented on the left (& right if BLOCKQUOTE)

#define ELEMDISP_ALIGN     32    // accepts the ALIGN flag for horiz. alignment

#define ELEMDISP_DECORATE  64    // draw more than just content: HR, LI

#define ELEMDISP_SPACED    128   // spaces and newlines are significant


// Parser states
//
// Some of these states are used in the content model to represent the type of
// data supported.
//
//   - MLST_PCDATA, MLST_CDATA, MLST_RCDATA: as described in SGML
//   - MLST_NODATA is used for elements that legally cannot contain data and
//     whose data contents should be ignored (like TABLE).
//   - MLST_NONE is used for empty elements.
//
typedef enum {
   MLST_NONE,
   MLST_PCDATA,          // PCDATA element content
   MLST_CDATA,           // CDATA element content
   MLST_RCDATA,          // RCDATA element content

   MLST_OE,              // After '&'
   MLST_ENTITY,          // Entity name
   MLST_ENTDEC,          // decimal character reference
   MLST_ENTHEX,          // hex character reference
   
   MLST_STAGO,           // After "<"
   MLST_TAGNAME,         // After "<..."
   MLST_TAGGAP,          // awaiting ATTRSPEC
   MLST_TAGGAPSLASH,     // '/' in taggap
   MLST_TAGEND,
   
   MLST_ATTRNAME,        // in attr name
   MLST_ATTRGAP,         // after attrname
   MLST_ATTRDONE,        // character after attrspec; immediate transition to TAGGAP
   MLST_ATTREQGAP,       // after "attr ="
   MLST_ATTRVAL,         // unquoted attr value
   MLST_ATTRQVAL,        // quoted attr value

   MLST_FULL,
   MLST_COMMENT,
   MLST_COMMENT1,
   MLST_COMMENT2,
} MLSt;


typedef struct ContentModel {
   ElemCat fClass;        // class of this element
   ElemCat fContents;     // set of classes this element can contain
   MLSt    stData;        // type of data contained (PCDATA, RCDATA, CDATA, NODATA, NONE)
   uint8   flags;         // ELEMFLAG_... appearance flags implied by element
   uint8   fDisp;
} ContentModel;


extern const ContentModel acmHtmlElems[ELEM_MAX];


#ifndef AEEISVGDOM_H
#define AEEISVGDOM_H

/*============================================================================
FILE: AEEISVGDOM.h

SERVICES:  BREW SVG DOM APIs

DESCRIPTION:
   This file defines interface for BREW SVG DOM, ISVGDOM.

PUBLIC CLASSES:  AEEISVGDOM

NOTE: This interface/feature is under construction. It has been included in
      this release to provide early access and to generate feedback.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

============================================================================*/

/*===================================================================
      Include Files
===================================================================*/
#include "AEEIModel.h"
#include "AEERGBVAL.h"

/*==========================================================================
   Type definitions
==========================================================================*/

// Interface ID
#define AEEIID_ISVGDOM  0x01038466

// Class ID for JSR226 1.1 behavior
#define AEECLSID_SVGJSR226_1_1    	0x01044bc2
// MIME type prefix
#define MT_SVGDOM          "image/svg"

typedef uint32    AEEDOMNodeID;

/* AEEDOMString */
typedef struct AEEDOMString
{
   AECHAR  *pwBuf;
   int      cchBuf;
} AEEDOMString;

/* SVGRect */
typedef struct SVGRect
{
   int32    x, y;
   int32    dx, dy;
} SVGRect;

/* SVGPathHandle: Represents a handle to SVGPath */
typedef uint32       SVGPathHandle;

/* SVGPoint */
typedef struct SVGPoint
{
   int32    cx;
   int32    cy;
} SVGPoint;

/* SVGMatrix */
typedef struct SVGMatrix
{
   int32 a;
   int32 b;
   int32 c;
   int32 d;
   int32 e;
   int32 f;
} SVGMatrix;

/* Mouse Event */
typedef struct SVGMouseEvent
{
   int32 screenX;
   int32 screenY;
   int32 clientX;
   int32 clientY;
   int32 button;
} SVGMouseEvent;


/* ProgressEvent */
typedef struct SVGProgressEvent
{
   AEEDOMString  *pTypeArgBuf;
   uint32         dwLoaded;
   uint32         dwTotal;
   boolean        bLengthComputable;
} SVGProgressEvent;

/* SVGElementInstance */
typedef struct SVGElementInstance
{
    AEEDOMNodeID correspondingElement;
    AEEDOMNodeID correspondingUseElement;
}SVGElementInstance;

/* SVGEvent */
typedef struct SVGEvent
{
   int32        nType;  // IMediaSVG event code (MM_SVG_EVENT_XXX)
   AEEDOMString pType;  // string describing event
   AEEDOMNodeID nTarget;
   AEEDOMNodeID nCurrentTarget;

   SVGElementInstance * pElementInstanceObj;

   AEEDOMString namespaceURI; //Not Supported
   boolean cancelable;        //Not Supported
   boolean defaultPrevented;  //Not Supported

   union
   {
      SVGMouseEvent     mouse;            //Mouse Events
      AEEDOMString      text;             //Text Events
      AEEDOMString      keyId;            //Keyboard Events
      AEEDOMString      connection;       //Connection Events   - Not Supported
      int32             nDetail;          //Time & UI Events    - Only Time is Supported
      int32             nWheelDelta;      //Wheel Events        - Not Supported
      SVGProgressEvent  progress;         //Progress Events     - Not Supported
   } u;

} SVGEvent;

typedef void (*PFNDOMEVENTHANDLER)(SVGEvent *evt, void *pUserData);

typedef struct AEEDOMEventListener
{
    PFNDOMEVENTHANDLER evtHandler;
    void* userData;
} AEEDOMEventListener;

/* SVGColor */
typedef struct SVGColor
{
   uint32 red;
   uint32 green;
   uint32 blue;
} SVGColor;

typedef struct SVGAsyncURLStatus
{
    boolean         success;
    AEEDOMString    contentType;
    AEEDOMString    content;
}SVGAsyncURLStatus;

typedef void (*PFNDOMOPERATIONCOMPLETE)(SVGAsyncURLStatus status, void * pUserData);

typedef struct AEEDOMAsyncStatusCallback
{
    PFNDOMOPERATIONCOMPLETE operationComplete;
    void * pUserData;
} AEEDOMAsyncStatusCallback;

#define MAKE_SVGColor(rgb, pc)         \
   (pc)->red   = ((rgb) >> 8)  & 0xFF; \
   (pc)->green = ((rgb) >> 16) & 0xFF; \
   (pc)->blue  = ((rgb) >> 24) & 0xFF;

/*===================================================================
    Interface Definition
===================================================================*/

#define INHERIT_ISVGDOM(iname)\
   INHERIT_IQI(iname);\
   \
   int (*CreateElementNS)(iname *po, const AEEDOMString *cpwNamespaceURI, const AEEDOMString *cpwQualifiedName, AEEDOMNodeID *pidElement);\
   int (*GetDocumentElement)(iname *po, AEEDOMNodeID *pidRoot);\
   int (*GetElementById)(iname *po, const AEEDOMString *cpwId, AEEDOMNodeID *pidElement);\
   int (*DeleteElement)(iname *po, AEEDOMNodeID pidElement);\
   \
   int (*GetNamespaceURI)(iname *po, AEEDOMNodeID idThis, AEEDOMString *cpwNamespaceURI);\
   int (*GetLocalName)(iname *po, AEEDOMNodeID idThis, AEEDOMString *cpwLocalName);\
   int (*GetParent)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidParent);\
   int (*GetOwnerDocument)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidOwnerDocument);\
   int (*GetContent)(iname *po, AEEDOMNodeID idThis, AEEDOMString *pwBuf, int nchIndex, int *pnUsed);\
   int (*SetContent)(iname *po, AEEDOMNodeID idThis, AEEDOMString *pwBuf, int nchIndex, int *pnSet);\
   \
   int (*AppendChild)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild);\
   int (*RemoveChild)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID idOldChild);\
   int (*InsertBefore)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild, AEEDOMNodeID idRefChild);\
   \
   int (*GetNextSibling)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidSibling);\
   int (*GetPrevSibling)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidSibling);\
   int (*GetFirstChild)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidChild);\
   int (*GetLastChild)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidChild);\
   \
   int (*AddEventListener)(iname *po, AEEDOMNodeID idThis, AEEDOMString *eventType, AEEDOMEventListener* listener, boolean useCapture);\
   int (*RemoveEventListener)(iname *po, AEEDOMNodeID idThis, AEEDOMString *eventType, AEEDOMEventListener* listener, boolean useCapture);\
   \
   int (*GetAttr)(iname *po, uint32 idThis, int32 nAttrID, void *p1, void *p2, void *p3);\
   int (*SetAttr)(iname *po, uint32 idThis, int32 nAttrID, void *p1, void *p2, void *p3);\
   \
   int (*BeginElement)(iname *po,  AEEDOMNodeID idThis);\
   int (*BeginElementAt)(iname *po,  AEEDOMNodeID idThis,uint32 dwFixedOffset);\
   int (*EndElement)(iname *po, AEEDOMNodeID idThis);\
   int (*EndElementAt)(iname *po, AEEDOMNodeID idThis, uint32 dwFixedOffset);\
   int (*PauseElement)(iname *po, AEEDOMNodeID idThis);\
   int (*ResumeElement)(iname *po, AEEDOMNodeID idThis);\
   int (*IsPaused)(iname *po, AEEDOMNodeID idThis, boolean *pbPaused);\
   \
   int (*CreateSVGMatrix)(iname *po, AEEDOMNodeID idThis, SVGMatrix *pIn, SVGMatrix **ppOut);\
   int (*FreeSVGMatrix)(iname *po, AEEDOMNodeID idThis, SVGMatrix *pMatrix);\
   int (*CreateSVGRect)(iname *po, AEEDOMNodeID idThis, SVGRect **ppRect);\
   int (*FreeSVGRect)(iname *po, AEEDOMNodeID idThis, SVGRect *pRect);\
   int (*CreateSVGPath)(iname *po, AEEDOMNodeID idThis, SVGPathHandle *phPath);\
   int (*FreeSVGPath)(iname *po, AEEDOMNodeID idThis, SVGPathHandle hPath);\
   int (*CreateSVGRGBColor)(iname *po, AEEDOMNodeID idThis, RGBVAL v, SVGColor **pvOut);\
   int (*FreeSVGRGBColor)(iname *po, AEEDOMNodeID idThis, SVGColor *pv);\
   \
   int (*SetFocus)(iname *po, AEEDOMNodeID idThis);\
   int (*MoveFocus)(iname *po, AEEDOMNodeID idThis, int nMotionType);\
   int (*FocusOn)(iname *po, AEEDOMNodeID idThis);\
   int (*GetCurrentFocusedObject)(iname *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidElement);\
   \
   int (*StopPropagation)(iname *po, SVGEvent * pEvent);\
   int (*PreventDefault)(iname *po, SVGEvent * pEvent);\
   \
   int (*GoToLocation)(iname *po, AEEDOMString * pwszNewIRI);\
   int (*GetURL)(iname *po, AEEDOMString * pwszIRI, AEEDOMAsyncStatusCallback * pfnCallback);\
   int (*PostURL)(iname *po, AEEDOMString * pwszIRI, AEEDOMString * pwszData, AEEDOMAsyncStatusCallback * pfnCallback, AEEDOMString * pwszType, AEEDOMString * pwszEncoding)

AEEINTERFACE_DEFINE(ISVGDOM);

/*==========================================================================
   ISVGDOM Attributes
==========================================================================*/

#define SVGDOM_ATTR_ELEMENT_ID                        0  // Get/Set: SVGElement::Id

// TraitAccess
#define SVGDOM_ATTR_TRAIT                             1  // GetTrait(), SetTrait(). p1=AEEDOMString *pName
#define SVGDOM_ATTR_TRAIT_NS                          2  // GetTraitNS(), SetTraitNS(). p1=AEEDOMString *pNS, p2=AEEDOMString *pName, p3=AEEDOMString *pOut
#define SVGDOM_ATTR_TRAIT_FLOAT                       3  // GetFloatTrait(), SetFloatTrait(). p1=AEEDOMString *pName, p2=int32 *pfOut
#define SVGDOM_ATTR_TRAIT_MATRIX                      4  // GetMatrixTrait(), SetMatrixTrait(). p1 = AEEDOMString *pName, p2=SVGMatrix *pOut
#define SVGDOM_ATTR_TRAIT_RECT                        5  // GetRectTrait(), SetRectTrait(). p1 = AEEDOMString *pName, p2=SVGRect *
#define SVGDOM_ATTR_TRAIT_PATH                        6  // GetPathTrait(), SetPathTrait(). p1 = AEEDOMString *pName, p2=SVGPathHandle *
#define SVGDOM_ATTR_TRAIT_RGBCOLOR                    7  // GetRGBColorTrait(), SetRGBColorTrait(). p1 = AEEDOMString *pName, p2=RGBVAL *
#define SVGDOM_ATTR_TRAIT_PRES                        8  // GetPresentationTrait(), SetPresentationTrait()
#define SVGDOM_ATTR_TRAIT_PRES_NS                     9  // GetPresentationTraitNS(), SetPresentationTraitNS()
#define SVGDOM_ATTR_TRAIT_PRES_FLOAT                  10 // GetFloatPresentationTrait(), SetFloatPresentationTrait()
#define SVGDOM_ATTR_TRAIT_PRES_MATRIX                 11 // GetMatrixPresentationTrait(), SetMatrixPresentationTrait()
#define SVGDOM_ATTR_TRAIT_PRES_RECT                   12 // GetRectPresentationTrait(), SetRectPresentationTrait()
#define SVGDOM_ATTR_TRAIT_PRES_PATH                   13 // GetPathPresentationTrait(), SetPathPresentationTrait()
#define SVGDOM_ATTR_TRAIT_PRES_RGBCOLOR               14 // GetRGBColorPresentationTrait(), SetRGBColorPresentationTrait()

#define SVGDOM_ATTR_MATRIX_OP_GETCOMPONENT            15 // SVGMatrix::getComponent(). p1=int nIndex, p2=int32 *pf
#define SVGDOM_ATTR_MATRIX_OP_MULTIPLY                16 // SVGMatrix::mMultiply(). id = SVGMatrix *pThis, p1=SVGMatrix *pSecond
#define SVGDOM_ATTR_MATRIX_OP_INVERSE                 17 // SVGMatrix::mInverse(). id = SVGMatrix *pThis
#define SVGDOM_ATTR_MATRIX_OP_TRANSLATE               18 // SVGMatrix::mTranslate(). id = SVGMatrix *pThis, p1 = SVGPoint *
#define SVGDOM_ATTR_MATRIX_OP_SCALE                   19 // SVGMatrix::mScale(). id = SVGMatrix *pThis, p1 = uint32 dwScaleFactor
#define SVGDOM_ATTR_MATRIX_OP_ROTATE                  20 // SVGMatrix::mRotate(). id = SVGMatrix *pThis, p2 = uint32 fAngle

#define SVGDOM_ATTR_LOCATABLE_OP_GETBBOX              21 // SVGLocatableElement::getBBox(). p1 = SVGRect *
#define SVGDOM_ATTR_LOCATABLE_OP_GETSCREENCTM         22 // SVGLocatableElement::getScreenCTM(). p1 = SVGMatrix *
#define SVGDOM_ATTR_LOCATABLE_OP_GETSCREENBBOX        23 // SVGLocatableElement::getScreenBBox(). p1 = SVGRect *

#define SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_SCALE       24 // Get/Set: SVGSVGElement::currentScale
#define SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_ROTATE      25 // Get/Set: SVGSVGElement::currentRotate
#define SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TRANSLATE   26 // Get/Set: SVGSVGElement::currentTranslate
#define SVGDOM_ATTR_SVGSVGELEMENT_VIEWPORT            27 // Get/Set: SVGSVGElement::viewport
#define SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TIME        28 // Get/Set: SVGSVGElement::currentTime

#define SVGDOM_ATTR_PATH_NUMBER_OF_SEGMENTS           29 // Get: SVGPath::numberOfSegments
#define SVGDOM_ATTR_PATH_OP_GETSEGMENT                30 // SVGPath::getSegment(). id = SVGPathHandle, p2 = int cmdIndex, p3 = int32 *
#define SVGDOM_ATTR_PATH_OP_GETSEGMENTPARAM           31 // SVGPath::getSegmentParam(). id = SVGPathHandle, p1 = int cmdIndex, p2 = int paramIndex, p3 = int32 *f
#define SVGDOM_ATTR_PATH_OP_MOVETO                    32 // SVGPath::moveTo(). id = SVGPathHandle, p1 = SVGPoint *
#define SVGDOM_ATTR_PATH_OP_LINETO                    33 // SVGPath::lineTo(). id = SVGPathHandle, p1 = SVGPoint *
#define SVGDOM_ATTR_PATH_OP_QUADTO                    34 // SVGPath::quadTo(). id = SVGPathHandle, p1 = SVGPoint *p1, p2 = SVGPoint *p2
#define SVGDOM_ATTR_PATH_OP_CURVETO                   35 // SVGPath::curveTo(). id = SVGPathHandle, p1 = SVGPoint *p1, p2 = SVGPoint *p2, p3 = SVGPoint *p3
#define SVGDOM_ATTR_PATH_OP_CLOSE                     36 // SVGPath::Close(). id = SVGPathHandle

#define SVGDOM_ATTR_ATTRIBUTE_NS                      37 // Get/Set: Element::AttributeNS. p1 = AEEDOMString *pNS, p2=AEEDOMString *pName, p3=AEEDOMString *pOut)

/*==========================================================================
   ISVGDOM Error Codes
==========================================================================*/
#define SVGDOM_ERROR_BASE                      MM_ERROR_BASE + 0xF0

#define SVGDOM_ERROR_INDEX_SIZE                SVGDOM_ERROR_BASE + 0x01
#define SVGDOM_ERROR_HIERARCHY_REQUEST         SVGDOM_ERROR_BASE + 0x02
#define SVGDOM_ERROR_WRONG_DOCUMENT            SVGDOM_ERROR_BASE + 0x03
#define SVGDOM_ERROR_INVALID_MODIFICATION      SVGDOM_ERROR_BASE + 0x04
#define SVGDOM_ERROR_INVALID_ACCESS            SVGDOM_ERROR_BASE + 0x05
#define SVGDOM_ERROR_MATRIX_NOT_INVERTABLE     SVGDOM_ERROR_BASE + 0x06

/*==========================================================================
   ISVGDOM Focus Directions
==========================================================================*/

#define SVGDOM_NAV_BASE           0
#define SVGDOM_NAV_AUTO           (SVGDOM_NAV_BASE + 1)
#define SVGDOM_NAV_NEXT           (SVGDOM_NAV_BASE + 2)
#define SVGDOM_NAV_PREV           (SVGDOM_NAV_BASE + 3)
#define SVGDOM_NAV_UP             (SVGDOM_NAV_BASE + 4)
#define SVGDOM_NAV_UP_RIGHT       (SVGDOM_NAV_BASE + 5)
#define SVGDOM_NAV_RIGHT          (SVGDOM_NAV_BASE + 6)
#define SVGDOM_NAV_DOWN_RIGHT     (SVGDOM_NAV_BASE + 7)
#define SVGDOM_NAV_DOWN           (SVGDOM_NAV_BASE + 8)
#define SVGDOM_NAV_DOWN_LEFT      (SVGDOM_NAV_BASE + 9)
#define SVGDOM_NAV_LEFT           (SVGDOM_NAV_BASE + 10)
#define SVGDOM_NAV_UP_LEFT        (SVGDOM_NAV_BASE + 11)

/*===============================================================================
    API Definition
===============================================================================*/

/* IQI */
static __inline uint32 ISVGDOM_AddRef(ISVGDOM *me)
{
   return AEEGETPVTBL(me,ISVGDOM)->AddRef(me);
}

static __inline uint32 ISVGDOM_Release(ISVGDOM *me)
{
   return AEEGETPVTBL(me,ISVGDOM)->Release(me);
}

static __inline uint32 ISVGDOM_QueryInterface(ISVGDOM *me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,ISVGDOM)->QueryInterface(me, cls, ppo);
}

/* ISVGDOM Tree operations */
static __inline int ISVGDOM_CreateElementNS(ISVGDOM *me, const AEEDOMString *cpwNamespaceURI, const AEEDOMString *cpwQualifiedName, AEEDOMNodeID *pidElement)
{
   return AEEGETPVTBL(me,ISVGDOM)->CreateElementNS(me, cpwNamespaceURI, cpwQualifiedName, pidElement);
}

static __inline int ISVGDOM_GetDocumentElement(ISVGDOM *me, AEEDOMNodeID *pidRoot)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetDocumentElement(me, pidRoot);
}

static __inline int ISVGDOM_GetElementById(ISVGDOM *me, const AEEDOMString *cpwId, AEEDOMNodeID *pidElement)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetElementById(me, cpwId, pidElement);
}

static __inline int ISVGDOM_DeleteElement(ISVGDOM *me, AEEDOMNodeID pidElement)
{
   return AEEGETPVTBL(me,ISVGDOM)->DeleteElement(me, pidElement);
}

static __inline int ISVGDOM_GetNamespaceURI(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMString *cpwNamespaceURI)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetNamespaceURI(me, idThis, cpwNamespaceURI);
}

static __inline int ISVGDOM_GetLocalName(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMString *cpwLocalName)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetLocalName(me, idThis, cpwLocalName);
}

static __inline int ISVGDOM_GetParent(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID *pidParent)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetParent(me, idThis, pidParent);
}

static __inline int ISVGDOM_GetOwnerDocument(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID *pidOwnerDocument)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetOwnerDocument(me, idThis, pidOwnerDocument);
}

static __inline int ISVGDOM_GetContent(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMString *pwBuf, int nchIndex, int *pnUsed)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetContent(me, idThis, pwBuf, nchIndex, pnUsed);
}

static __inline int ISVGDOM_SetContent(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMString *pwBuf, int nchIndex, int *pnSet)
{
   return AEEGETPVTBL(me,ISVGDOM)->SetContent(me, idThis, pwBuf, nchIndex, pnSet);
}

static __inline int ISVGDOM_AppendChild(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild)
{
   return AEEGETPVTBL(me,ISVGDOM)->AppendChild(me, idThis, idNewChild);
}

static __inline int ISVGDOM_RemoveChild(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID idOldChild)
{
   return AEEGETPVTBL(me,ISVGDOM)->RemoveChild(me, idThis, idOldChild);
}

static __inline int ISVGDOM_InsertBefore(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild, AEEDOMNodeID idRefChild)
{
   return AEEGETPVTBL(me,ISVGDOM)->InsertBefore(me, idThis, idNewChild, idRefChild);
}

static __inline int ISVGDOM_GetNextSibling(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID *pidSibling)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetNextSibling(me, idThis, pidSibling);
}

static __inline int ISVGDOM_GetPrevSibling(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID *pidSibling)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetPrevSibling(me, idThis, pidSibling);
}

static __inline int ISVGDOM_GetFirstChild(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID *pidChild)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetFirstChild(me, idThis, pidChild);
}

static __inline int ISVGDOM_GetLastChild(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID *pidChild)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetLastChild(me, idThis, pidChild);
}

/* ISVGDOM Event handling */
static __inline int ISVGDOM_AddEventListener(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMString *eventType, AEEDOMEventListener* listener,
                                             boolean useCapture)
{
   return AEEGETPVTBL(me,ISVGDOM)->AddEventListener(me, idThis, eventType, listener, useCapture);
}

static __inline int ISVGDOM_RemoveEventListener(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMString *eventType, AEEDOMEventListener* listener,
                                                boolean useCapture)
{
   return AEEGETPVTBL(me,ISVGDOM)->RemoveEventListener(me, idThis, eventType, listener, useCapture);
}

/* Attribute handling */
static __inline int ISVGDOM_GetAttr(ISVGDOM *me, uint32 idThis, int32 idAttr, void *p1, void *p2, void *p3)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetAttr(me, idThis, idAttr, p1, p2, p3);
}

static __inline int ISVGDOM_SetAttr(ISVGDOM *me, uint32 idThis, int32 idAttr, void *p1, void *p2, void *p3)
{
   return AEEGETPVTBL(me,ISVGDOM)->SetAttr(me, idThis, idAttr, p1, p2, p3);
}

/* SVGAnimationElement operations */
static __inline int ISVGDOM_BeginElement(ISVGDOM *me,  AEEDOMNodeID idThis)
{
   return AEEGETPVTBL(me,ISVGDOM)->BeginElement(me, idThis);
}

static __inline int ISVGDOM_BeginElementAt(ISVGDOM *me,  AEEDOMNodeID idThis, uint32 dwFixedOffset)
{
   return AEEGETPVTBL(me,ISVGDOM)->BeginElementAt(me, idThis, dwFixedOffset);
}

static __inline int ISVGDOM_EndElement(ISVGDOM *me, AEEDOMNodeID idThis)
{
   return AEEGETPVTBL(me,ISVGDOM)->EndElement(me, idThis);
}

static __inline int ISVGDOM_EndElementAt(ISVGDOM *me, AEEDOMNodeID idThis, uint32 dwFixedOffset)
{
   return AEEGETPVTBL(me,ISVGDOM)->EndElementAt(me, idThis, dwFixedOffset);
}

static __inline int ISVGDOM_PauseElement(ISVGDOM *me, AEEDOMNodeID idThis)
{
   return AEEGETPVTBL(me,ISVGDOM)->PauseElement(me, idThis);
}

static __inline int ISVGDOM_ResumeElement(ISVGDOM *me, AEEDOMNodeID idThis)
{
   return AEEGETPVTBL(me,ISVGDOM)->ResumeElement(me, idThis);
}

static __inline int ISVGDOM_IsPaused(ISVGDOM *me, AEEDOMNodeID idThis, boolean *pbPaused)
{
   return AEEGETPVTBL(me,ISVGDOM)->IsPaused(me, idThis, pbPaused);
}

/* SVGSVGElement operations */
static __inline int ISVGDOM_CreateSVGMatrix(ISVGDOM *me, AEEDOMNodeID idThis, SVGMatrix *pIn, SVGMatrix **ppOut)
{
   return AEEGETPVTBL(me,ISVGDOM)->CreateSVGMatrix(me, idThis, pIn, ppOut);
}

static __inline int ISVGDOM_FreeSVGMatrix(ISVGDOM *me, AEEDOMNodeID idThis, SVGMatrix *pMatrix)
{
   return AEEGETPVTBL(me,ISVGDOM)->FreeSVGMatrix(me, idThis, pMatrix);
}

static __inline int ISVGDOM_CreateSVGRect(ISVGDOM *me, AEEDOMNodeID idThis, SVGRect **pRect)
{
   return AEEGETPVTBL(me,ISVGDOM)->CreateSVGRect(me, idThis, pRect);
}

static __inline int ISVGDOM_FreeSVGRect(ISVGDOM *me, AEEDOMNodeID idThis, SVGRect *pRect)
{
   return AEEGETPVTBL(me,ISVGDOM)->FreeSVGRect(me, idThis, pRect);
}

static __inline int ISVGDOM_CreateSVGPath(ISVGDOM *me, AEEDOMNodeID idThis, SVGPathHandle *phPath)
{
   return AEEGETPVTBL(me,ISVGDOM)->CreateSVGPath(me, idThis, phPath);
}

static __inline int ISVGDOM_FreeSVGPath(ISVGDOM *me, AEEDOMNodeID idThis, SVGPathHandle hPath)
{
   return AEEGETPVTBL(me,ISVGDOM)->FreeSVGPath(me, idThis, hPath);
}

static __inline int ISVGDOM_CreateSVGRGBColor(ISVGDOM *me, AEEDOMNodeID idThis, RGBVAL v, SVGColor **pvOut)
{
   return AEEGETPVTBL(me,ISVGDOM)->CreateSVGRGBColor(me, idThis, v, pvOut);
}

static __inline int ISVGDOM_FreeSVGRGBColor(ISVGDOM *me, AEEDOMNodeID idThis, SVGColor *pv)
{
   return AEEGETPVTBL(me,ISVGDOM)->FreeSVGRGBColor(me, idThis, pv);
}

static __inline int ISVGDOM_SetFocus(ISVGDOM *me, AEEDOMNodeID idThis)
{
   return AEEGETPVTBL(me,ISVGDOM)->SetFocus(me, idThis);
}

static __inline int ISVGDOM_MoveFocus(ISVGDOM *me, AEEDOMNodeID idThis, int nMotionType)
{
   return AEEGETPVTBL(me,ISVGDOM)->MoveFocus(me, idThis, nMotionType);
}

static __inline int ISVGDOM_FocusOn(ISVGDOM *me, AEEDOMNodeID idThis)
{
   return AEEGETPVTBL(me,ISVGDOM)->FocusOn(me, idThis);
}

static __inline int ISVGDOM_GetCurrentFocusedObject(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMNodeID *pidElement)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetCurrentFocusedObject(me, idThis, pidElement);
}

static __inline int ISVGDOM_StopPropagation(ISVGDOM *me, SVGEvent *pEvent)
{
   return AEEGETPVTBL(me,ISVGDOM)->StopPropagation(me, pEvent);
}

static __inline int ISVGDOM_PreventDefault(ISVGDOM *me, SVGEvent *pEvent)
{
   return AEEGETPVTBL(me,ISVGDOM)->PreventDefault(me, pEvent);
}

static __inline int ISVGDOM_GoToLocation(ISVGDOM *me, AEEDOMString *pwszNewIRI)
{
   return AEEGETPVTBL(me,ISVGDOM)->GoToLocation(me, pwszNewIRI);
}

static __inline int ISVGDOM_GetURL(ISVGDOM *me, AEEDOMString *pwszIRI, AEEDOMAsyncStatusCallback *pfnCallback)
{
   return AEEGETPVTBL(me,ISVGDOM)->GetURL(me, pwszIRI, pfnCallback);
}

static __inline int ISVGDOM_PostURL(ISVGDOM *me, AEEDOMString *pwszIRI, AEEDOMString *pwszData, AEEDOMAsyncStatusCallback *pfnCallback, AEEDOMString *pwszType, AEEDOMString *pwszEncoding)
{
   return AEEGETPVTBL(me,ISVGDOM)->PostURL(me, pwszIRI, pwszData, pfnCallback, pwszType, pwszEncoding);
}

/* SVGElement TraitAccess */
static __inline int ISVGDOM_GetTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, AEEDOMString *pValue)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT, (void*)pName, pValue, 0);
}

static __inline int ISVGDOM_SetTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, AEEDOMString *pValue)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_TRAIT, (void*)pName, pValue, 0);
}

static __inline int ISVGDOM_GetTraitNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_NS, (void*)pNS, (void*)pName, pValue);
}

static __inline int ISVGDOM_SetTraitNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_TRAIT_NS, (void*)pNS, (void*)pName, pValue);
}

static __inline int ISVGDOM_GetFloatTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, int32 *pf)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_FLOAT, (void*)pName, pf, 0);
}

static __inline int ISVGDOM_SetFloatTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, int32 f)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_TRAIT_FLOAT, (void*)pName, (void*)f, 0);
}

static __inline int ISVGDOM_GetMatrixTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGMatrix *pm, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_MATRIX, (void*)pName, pm, pIsNull);
}

static __inline int ISVGDOM_SetMatrixTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGMatrix *pm)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_TRAIT_MATRIX, (void*)pName, pm, 0);
}

static __inline int ISVGDOM_GetRectTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGRect *pr, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_RECT, (void*)pName, pr, pIsNull);
}

static __inline int ISVGDOM_SetRectTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGRect *pr)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_TRAIT_RECT, (void*)pName, pr, 0);
}

static __inline int ISVGDOM_GetPathTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGPathHandle *ph, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_PATH, (void*)pName, ph, pIsNull);
}

static __inline int ISVGDOM_SetPathTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGPathHandle h)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_TRAIT_PATH, (void*)pName, (void*)h, 0);
}

static __inline int ISVGDOM_GetRGBColorTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGColor *pv, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_RGBCOLOR, (void*)pName, pv, pIsNull);
}

static __inline int ISVGDOM_SetRGBColorTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGColor *v)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_TRAIT_RGBCOLOR, (void*)pName, v, 0);
}

static __inline int ISVGDOM_GetSVGElementID(ISVGDOM *me, AEEDOMNodeID idElement, AEEDOMString *pwID)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_ELEMENT_ID, pwID, 0, 0);
}

static __inline int ISVGDOM_SetSVGElementID(ISVGDOM *me, AEEDOMNodeID idElement, AEEDOMString *pwID)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_ELEMENT_ID, pwID, 0, 0);
}

static __inline int ISVGDOM_GetPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, AEEDOMString *pValue)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_PRES, (void*)pName, pValue, 0);
}

static __inline int ISVGDOM_GetPresentationTraitNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_PRES_NS, (void*)pNS, (void*)pName, pValue);
}

static __inline int ISVGDOM_GetFloatPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, int32 *pf)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_PRES_FLOAT, (void*)pName, pf, 0);
}

static __inline int ISVGDOM_GetMatrixPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGMatrix *pm, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_PRES_MATRIX, (void*)pName, pm, pIsNull);
}

static __inline int ISVGDOM_GetRectPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGRect *pr, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_PRES_RECT, (void*)pName, pr, pIsNull);
}

static __inline int ISVGDOM_GetPathPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGPathHandle *ph, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_PRES_PATH, (void*)pName, ph, pIsNull);
}

static __inline int ISVGDOM_GetRGBColorPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGColor *pv, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_TRAIT_PRES_RGBCOLOR, (void*)pName, pv, pIsNull);
}


/* SVGLocatableElement operations */
static __inline int ISVGDOM_GetBBox(ISVGDOM *me, AEEDOMNodeID idElement, SVGRect *pr, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_LOCATABLE_OP_GETBBOX, pr, pIsNull, 0);
}

static __inline int ISVGDOM_GetScreenCTM(ISVGDOM *me, AEEDOMNodeID idElement, SVGMatrix *pm, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_LOCATABLE_OP_GETSCREENCTM, pm, pIsNull, 0);
}

static __inline int ISVGDOM_GetScreenBBox(ISVGDOM *me, AEEDOMNodeID idElement, SVGRect *pr, boolean *pIsNull)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_LOCATABLE_OP_GETSCREENBBOX, pr, pIsNull, 0);
}

/* SVGSVGElement operations */
static __inline int ISVGDOM_GetCurrentScale(ISVGDOM *me, AEEDOMNodeID idElement, int32 *pf)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_SCALE, pf, 0, 0);
}

static __inline int ISVGDOM_SetCurrentScale(ISVGDOM *me, AEEDOMNodeID idElement, int32 f)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_SCALE, (void*)f, 0, 0);
}

static __inline int ISVGDOM_GetCurrentRotate(ISVGDOM *me, AEEDOMNodeID idElement, int32 *pf)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_ROTATE, pf, 0, 0);
}

static __inline int ISVGDOM_SetCurrentRotate(ISVGDOM *me, AEEDOMNodeID idElement, int32 f)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_ROTATE, (void*)f, 0, 0);
}

static __inline int ISVGDOM_GetCurrentTranslate(ISVGDOM *me, AEEDOMNodeID idElement, SVGPoint *pPt)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TRANSLATE, pPt, 0, 0);
}

static __inline int ISVGDOM_SetCurrentTranslate(ISVGDOM *me, AEEDOMNodeID idElement, SVGPoint *pPt)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TRANSLATE, pPt, 0, 0);
}

static __inline int ISVGDOM_GetViewPort(ISVGDOM *me, AEEDOMNodeID idElement, SVGRect *pr)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_VIEWPORT, pr, 0, 0);
}

static __inline int ISVGDOM_SetViewPort(ISVGDOM *me, AEEDOMNodeID idElement, SVGRect *pr)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_VIEWPORT, pr, 0, 0);
}

static __inline int ISVGDOM_GetCurrentTime(ISVGDOM *me, AEEDOMNodeID idElement, int32 *pf)
{   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TIME, pf, 0, 0);
}

static __inline int ISVGDOM_SetCurrentTime(ISVGDOM *me, AEEDOMNodeID idElement, int32 f)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TIME, (void*)f, 0, 0);
}

/* SVGMatrix support */
static __inline int ISVGDOM_MatrixGetComponent(ISVGDOM *me, SVGMatrix *pThis, int nIndex, int32 *pf)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, (uint32)pThis, SVGDOM_ATTR_MATRIX_OP_GETCOMPONENT, (void*)nIndex, pf, 0);
}

static __inline int ISVGDOM_MatrixMultiply(ISVGDOM *me, SVGMatrix *pThis, const SVGMatrix *pSecond)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, (uint32)pThis, SVGDOM_ATTR_MATRIX_OP_MULTIPLY, (void*)pSecond, 0, 0);
}

static __inline int ISVGDOM_MatrixInverse(ISVGDOM *me, SVGMatrix *pThis)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, (uint32)pThis, SVGDOM_ATTR_MATRIX_OP_INVERSE, 0, 0, 0);
}

static __inline int ISVGDOM_MatrixTranslate(ISVGDOM *me, SVGMatrix *pThis, uint32 x, uint32 y)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, (uint32)pThis, SVGDOM_ATTR_MATRIX_OP_TRANSLATE, (void*)x, (void*)y, 0);
}

static __inline int ISVGDOM_MatrixScale(ISVGDOM *me, SVGMatrix *pThis, uint32 dwScaleFactor)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, (uint32)pThis, SVGDOM_ATTR_MATRIX_OP_SCALE, (void*)dwScaleFactor, 0, 0);
}

static __inline int ISVGDOM_MatrixRotate(ISVGDOM *me, SVGMatrix *pThis, uint32 fAngle)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, (uint32)pThis, SVGDOM_ATTR_MATRIX_OP_ROTATE, (void*)fAngle, 0, 0);
}

/* SVGPath support */
static __inline int ISVGDOM_PathGetNumberOfSegments(ISVGDOM *me, SVGPathHandle hThis, int *pnSegments)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, hThis, SVGDOM_ATTR_PATH_NUMBER_OF_SEGMENTS, pnSegments, 0, 0);
}

static __inline int ISVGDOM_PathGetSegment(ISVGDOM *me, SVGPathHandle hThis, int nIdx, int32 *pfParam)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, hThis, SVGDOM_ATTR_PATH_OP_GETSEGMENT, (void*)nIdx, pfParam, 0);
}

static __inline int ISVGDOM_PathGetSegmentParam(ISVGDOM *me, SVGPathHandle hThis, int nCmdIdx, int nParamIdx, int32 *pfParam)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, hThis, SVGDOM_ATTR_PATH_OP_GETSEGMENTPARAM, (void*)nCmdIdx, (void*)nParamIdx, pfParam);
}

static __inline int ISVGDOM_PathMoveTo(ISVGDOM *me, SVGPathHandle hThis, SVGPoint *p)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, hThis, SVGDOM_ATTR_PATH_OP_MOVETO, p, 0, 0);
}

static __inline int ISVGDOM_PathLineTo(ISVGDOM *me, SVGPathHandle hThis, SVGPoint *p)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, hThis, SVGDOM_ATTR_PATH_OP_LINETO, p, 0, 0);
}

static __inline int ISVGDOM_PathQuadTo(ISVGDOM *me, SVGPathHandle hThis, SVGPoint *p1, SVGPoint *p2)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, hThis, SVGDOM_ATTR_PATH_OP_QUADTO, p1, p2, 0);
}

static __inline int ISVGDOM_PathCurveTo(ISVGDOM *me, SVGPathHandle hThis, SVGPoint *p1, SVGPoint *p2, SVGPoint *p3)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, hThis, SVGDOM_ATTR_PATH_OP_CURVETO, p1, p2, p3);
}

static __inline int ISVGDOM_PathClose(ISVGDOM *me, SVGPathHandle hThis)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, hThis, SVGDOM_ATTR_PATH_OP_CLOSE, 0, 0, 0);
}

/* Node support */
static __inline int ISVGDOM_GetAttributeNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, idElement, SVGDOM_ATTR_ATTRIBUTE_NS, (void*)pNS, (void*)pName, pValue);
}

static __inline int ISVGDOM_SetAttributeNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, idElement, SVGDOM_ATTR_ATTRIBUTE_NS, (void*)pNS, (void*)pName, pValue);
}

static __inline int ISVGDOM_IsInsertedInTree(ISVGDOM *me, AEEDOMNodeID node, boolean* isInserted)
{
   return AEEGETPVTBL(me, ISVGDOM)->GetAttr(me, (uint32)node, (int32)isInserted, 0, 0, 0);
}

static __inline int ISVGDOM_SetNodeLock(ISVGDOM *me, AEEDOMNodeID node, boolean lock)
{
   return AEEGETPVTBL(me, ISVGDOM)->SetAttr(me, node, lock, 0, 0, 0);
}
/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEEDOMString

Description:
  This structure is used to represent non-null terminated wide strings.

Definition:
  typedef struct AEEDOMString
  {
    AECHAR  *pwBuf;
    int      cchBuf;
  } AEEDOMString;

Members:
  pwBuf:    pointer to memory buffer that contains the wide string characters
  cchBuf:   number of valid characters in the buffer

Comments:
  None.

See Also:
  ISVGDOM_GetNamespaceURI()
==========================================================================

SVGRect

Description:
  This structure is used to represent a rectangle.

Definition:
  typedef struct SVGRect
  {
    int32    x, y;
    int32    dx, dy;
  } SVGRect;

Members:
  x:        x coordinate of the upper left corner of the rectangle
  y:        y coordinate of the upper left corner of the rectangle
  dx:       width of the rectangle
  dy:       height of the rectangle

Comments:
  None.

See Also:
  ISVGDOM_CreateSVGRect()
==========================================================================

SVGPoint

Description:
  This structure is used to represent a point.

Definition:
  typedef struct SVGPoint
  {
     int32    cx;
     int32    cy;
  } SVGPoint;

Members:
  cx:       x coordinate of the point
  cy:       y coordinate of the point

Comments:
  None.

See Also:
  ISVGDOM_GetCurrentTranslate()
  ISVGDOM_SetCurrentTranslate()
==========================================================================

SVGMatrix

Description:
  Info from SVG DOM 1.1 specifications:
  SVGMatrix is identified by an affine transform. It can be used to read and modify the values of transform attribute
  as per SVG specification. Note that the mTranslate, inverse, mMultiply, mScale and mRotate methods in mutate the SVGMatrix object and
  return a reference to the SVGMatrix instance itself, after performing the necessary matrix operation.
  This matrix transforms source coordinates (x, y) into destination coordinates (x', y') by considering them to be a column vector and
  multiplying the coordinate vector by the matrix according to the following process:

  [ x' ]    [  a  c  e  ]   [ x ]    [ a.x + c.y + e ]
  [ y' ] =  [  b  d  f  ]   [ y ] =  [ b.x + d.y + f ]
  [ 1  ]    [  0  0  1  ]   [ 1 ]    [        1      ]

Definition:
  typedef struct SVGMatrix
 {
    int32 a;      // 16.16 fixed notation
    int32 b;      // 16.16 fixed notation
    int32 c;      // 16.16 fixed notation
    int32 d;      // 16.16 fixed notation
    int32 e;      // 16.16 fixed notation
    int32 f;      // 16.16 fixed notation
  } SVGMatrix;

Members:
  a:      [0,0] element of the matrix
  b:      [1,0] element of the matrix
  c:      [0,1] element of the matrix
  d:      [1,1] element of the matrix
  e:      [0,2] element of the matrix
  f:      [1,2] element of the matrix

Comments:
  None.

See Also:
  ISVGDOM_CreateSVGMatrix()
==========================================================================

SVGMouseEvent

Description:
  This structure is used to represent a mouse event.

Definition:
  typedef struct SVGMouseEvent
  {
    int32 screenX;
    int32 screenY;
    int32 clientX;
    int32 clientY;
    int32 button;
  } SVGMouseEvent;

Members:
  screenX:     screen x coordinate of the mouse event
  screenY:     screen y coordinate of the mouse event
  clientX:     document x coordinate of the mouse event
  clientY:     document y coordinate of the mouse event
  button:      button number that caused the event

Comments:
  None.

See Also:
  ISVGDOM_AddEventListener()
  ISVGDOM_RemoveEventListener()
==========================================================================

SVGProgressEvent

Description:
  This structure is used to represent a download progress event.

Definition:
  typedef struct SVGProgressEvent
  {
    AEEDOMString  *pTypeArgBuf;
    uint32         dwLoaded;
    uint32         dwTotal;
    boolean        bLengthComputable;
  } SVGProgressEvent;

Members:
  pTypeArgBuf:           arguments that were used for the download command
  dwLoaded:              number of bytes download so far
  dwTotal:               total length of the data to be downloaded
  bLengthComputable:     TRUE if the total length of the data to be downloaded is known

Comments:
  None.

See Also:
  ISVGDOM_AddEventListener()
  ISVGDOM_RemoveEventListener()
==========================================================================

SVGElementInstance

Description:
  This structure is used to represent the SVGElementInstance
  objects.

Definition:
  typedef struct SVGElementInstance
  {
    AEEDOMNodeID correspondingElement;
    AEEDOMNodeID correspondingUseElement;
  }SVGElementInstance;

Members:
  correspondingElement:    the corresponding element of a instance of object
  correspondingUseElement: the corresponding 'use' element of a instance of object

Comments:
  None.

See Also:
  ISVGDOM_AddEventListener()
  ISVGDOM_RemoveEventListener()
==========================================================================

SVGEvent

Description:
  This structure is used to represent a SVGDOM event.

Definition:
  typedef struct SVGEvent
  {
    int32                nType;
    AEEDOMString         pType;
    AEEDOMNodeID         nTarget;
    AEEDOMNodeID         nCurrentTarget;
    SVGElementInstance * nElementInstsanceObj;

    AEEDOMString         namespaceURI;
    boolean              cancelable;
    boolean              defaultPrevented;

    union
    {
      SVGMouseEvent     mouse;
      AEEDOMString      text;
      AEEDOMString      keyId;
      AEEDOMString      connection;
      int32             nDetail;
      int32             nWheelDelta;
      SVGProgressEvent  progress;
    } u;
  } SVGEvent;

Members:
  nType:           IMediaSVG event code (MM_SVG_EVENT_XXX)
  pType:           type of the event
  nTarget:         node ID that generated the event
  nCurrentTarget:  node ID of the current event
  nEventTarget:    the element instance object
  namespaceURI:    the namespaceURI which related to its event
  cancelable:      true to indicate the default action can be prevented
  defaultPrevented:true to indicate the preventDevault() function as been called
  u:               additional data associated with the event
     mouse:        Mouse Event, see also SVGMouseEvent
     text:         Text Event, contains a text string
     keyId:        Keyboard Event, contains a key code
     connection:   Connection Event, contains an URL
     nDetail:      Time Event, contains a time value
     nWheelDelta:  Wheel Event, contains a delta from previous wheel position
     progress:     Progress Event, see also SVGProgressEvent

Comments:
  None.

See Also:
  ISVGDOM_AddEventListener()
  ISVGDOM_RemoveEventListener()

==========================================================================

SVGColor

Description:
  This structure is used to represent live SVG RGB Color.

Definition:
  typedef strcuct SVGColor
  {
     uint32 red;
     uint32 green;
     uint32 blue;
  } SVGColor;

Members:
  red:           Red component of RGB color
  green:         Green component of RGB color
  blue:          Blue component of RGB color

Comments:
  Use MAKE_SVGColor(rgb, pc) to convert Brew color to SVG color
  Use rgb = MAKE_RGB(pc->red, pc->green, pc->blue) to convert SVG color to Brew color

See Also:
  ISVGDOM_CreateSVGRGBColor()

=====================================================================
  INTERFACES   DOCUMENTATION
=====================================================================

ISVGDOM interface provides the ability to access, traverse and manipulate the DOM tree of an SVG media document.
It is an extension of IMediaSVG interface. User can QueryInterface IMediaSVG to obtain ISVGDOM object.

Overall goals:
--------------
(1) To provide easy and synchronous access to SVG DOM tree as specified in w3c SVG DOM specifications
(2) To leverage existing BREW mechanisms
(3) To avoid creation of too many objects to access, navigate and manipulate the DOM tree

DOM tree and objects:
---------------------
User needs only one ISVGDOM object to access and edit the SVG DOM tree.

Each element is identified by AEEDOMNodeID. Most of the APIs operate on an element which is identified by "idThis".
This is almost analogus to the "this" pointer of C++ but here it refers to the element.

AEEDOMString:
-------------
(1) Strings are not assumed to be NULL terminated
(2) All allocations are made by user of ISVGDOM. User first needs to obtain the required size by setting
    pwBuf = NULL (or pointing it to NULL string) and the returned cchBuf will contain the required number of AECHARs. User
    has to malloc the memory and is responsible to free it

Floating point numbers:
-----------------------
These are represented as int32 in fixed point 1:15:16 format.

Event Handling:
---------------
Event handling is used callback functionality which is registed by ISVGDOM_AddEventListener().
Removing the event use ISVGDOM_RemoveEventListener().

ISVGDOM object lifecycle:
------------------------
In order to create an ISVGDOM object, an IMediaSVG object must already exist.

// Create an IMediaSVG object by
nErr = ISHELL_CreateInstance(pIShell, AEECLSID_MEDIASVG, (void **)pIMediaSVG);

// Attach an SVG document to the newly created IMediaSVG object by
nErr = IMedia_SetMediaData(pIMediaSVG, pAEEMediaData);

// or create an empty SVG document for the newly created IMediaSVG object by
nErr = IMediaSVG_SetDOMEmptyDoc(pIMediaSVG, TRUE);

// Set the frame generation mode to client by
nErr = IMediaSVG_EnableClientFrameGeneration(pIMediaSVG, TRUE);

// Setup media notification callback by
nErr = IMedia_RegisterNotify(pIMediaSVG, cbMediaNotify, pMe);

// Get an ISVGDOM object by
nErr = IMedia_QueryInterface(pIMediaSVG, AEECLSID_SVGDOM, (void**)&pISVGDOM);

The newly created ISVGDOM is now ready for all operations.
The associated IMediaSVG gets transferred to Play state during the creation of ISVGDOM if
it is not already in the Play state.
IMediaSVG will remain in Play state until the associated ISVGDOM object is released.
A consequence of this is that IMedia_Stop(pIMediaSVG) will always return EFAILED if an
associated ISVGDOM object exist.
When ISVGDOM object is released, IMediaSVG gets trasferred back to Ready state.

// When the ISVGDOM object is no loger needed, cleanup by
ISVGDOM_Release(pISVGDOM);
IMedia_Release(pIMediaSVG);

ISVGDOM Error Codes:
--------------------

  SVGDOM_ERROR_INDEX_SIZE               - Index or size is negative, or greater than the
                                          allowed value.
  SVGDOM_ERROR_HIERARCHY_REQUEST        - A node is inserted somewhere it doesn't belong.
  SVGDOM_ERROR_WRONG_DOCUMENT           - A node is used in a different document than the
                                          one that created it.
  SVGDOM_ERROR_INVALID_MODIFICATION     - An attempt is made to modify the type of the
                                          underlying object.
  SVGDOM_ERROR_INVALID_ACCESS           - A parameter or an operation is not supported by the
                                          underlying object.
  SVGDOM_ERROR_MATRIX_NOT_INVERTABLE    - The DOM matrix cannot be be inverted.


  ENOTALLOWED                           - An attempt is made to modify an object where
                                          modifications are not allowed
  ENOSUCH                               - An attempt is made to reference a node in a context
                                          where it does not exist.
  EUNSUPPORTED                          - The implementation does not support the requested
                                          type of object or operation.
  ENOTYPE                               - The type of an object is incompatible with the
                                          expected type of the parameter associated to the object.
  EBADSTATE                             - An attempt is made to use an object that is not,
                                          or is no longer, usable.
  EBADITEM                              - The value supplied for an operation is invalid.

General Brew Error Codes that can be returned by any method:
------------------------------------------------------------
  SUCCESS                               - Not an error
  EBADPARM                              - The supplied parameter value is invalid.
  ENOMEMORY                             - Out of memory.
  EFAILED                               - Unspecified error condition.

===============================================================================

ISVGDOM_AddRef()

Description:
  Increments object reference count and returns the new incremented reference count.

Prototype:
  uint32 ISVGDOM_AddRef ( ISVGDOM * pme );

Parameters:
  pme             this pointer

Return Value:
  Current object reference count

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_Release()

=============================================================================

ISVGDOM_Release()

Description:
  Decreases the number of references to the ISVGDOM object. If the
  last reference is removed, it deallocates the object.

Prototype:
   uint32 ISVGDOM_Release ( ISVGDOM * pme );

Parameters:
  pme             this pointer

Return Value:
  Current object reference count

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_AddRef()

=============================================================================

ISVGDOM_QueryInterface()

Description:
  Allows applications to query if ISVGDOM supports other interfaces and then
  returns a pointer to that new interface supported via ISVGDOM.

Prototype:
   int ISVGDOM_QueryInterface ( ISVGDOM * pme, AEECLSID idReq, void ** ppo );

Parameters:
  pme                this pointer
  idReq              class id of requested object
  ppo                pointer to returned object

Return Value:
  EBADPARM           if any of the parameters are NULL
  ECLASSNOTSUPPORT   if requested class is not supported by this object
  SUCCESS            with requested interface pointer

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

===========================================================================

ISVGDOM_CreateElementNS()

Description:
  Creates a new Element based on the specified (qualified) SVG tag name.

Prototype:
  int ISVGDOM_CreateElementNS ( ISVGDOM * pme, const AEEDOMString * cpwNamespaceURI,
                                     const AEEDOMString * cpwQualifiedName, AEEDOMNodeID * pidElement );

Parameters:
  pme                this pointer
  cpwNamespaceURI    the namespace uri for the newly created element.
                     This should always be the SVG namespace URI "http://www.w3.org/2000/svg".
  cpwQualifiedName   the qualified name for the newly created element (For example: "rect", to create a <rect> element)
  pidElement         pointer to returned element

Return Value:
  SUCCESS            with the newly created SVG Element.
  EUNSUPPORTED       if the type of element is not supported by the implementation.
  ENOMEMORY          when out of memory
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_GetNamespaceURI()

=============================================================================

ISVGDOM_GetDocumentElement()

Description:
  Gets a child element of this document Node which corresponds to the top-most tag in XML file.

Prototype:
  int ISVGDOM_GetDocumentElement ( ISVGDOM * pme, AEEDOMNodeID * pidRoot )p

Parameters:
  pme              this pointer
  pidRoot          pointer to returned element

Return Value:
  SUCCESS          with the root Element associated with this document.
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_GetElementById()

=============================================================================

ISVGDOM_GetElementById()

Description:
  Gets the Element in the current document with the given unique ID.

Prototype:
  int ISVGDOM_GetElementById ( ISVGDOM * pme, const AEEDOMString * cpwId, AEEDOMNodeID * pidElement );

Parameters:
  pme              this pointer
  cpwId            the ID of the element to be retrieved
  pidElement       pointer to returned element

Return Value:
  SUCCESS          with the Element that matches with the given ID, or
                   NULL if the ID is not present.
  ENOMEMORY        when out of memory
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_GetDocumentElement()

=============================================================================

ISVGDOM_DeleteElement()

Description:
  Deletes and releases all the resources associated with an element.

Prototype:
  int ISVGDOM_DeleteElement ( ISVGDOM * pme, AEEDOMNodeID pidElement );

Parameters:
  pme              this pointer
  pidElement       element to delete

Return Value:
  SUCCESS          element was deleted
  ENOSUCH          element doesn't exist
  EUNSUPPORTED     unsupported operation
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_RemoveChild()
  ISVGDOM_CreateElementNS()

=============================================================================

ISVGDOM_GetNamespaceURI()

Description:
  Gets the namespace URI of the Node.

Prototype:
  int ISVGDOM_GetNamespaceURI ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMString * cpwNamespaceURI );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  cpwNamespaceURI  pointer to returned namespace URI

Return Value:
  SUCCESS          with the namespace URI of the Node.
  ENOMEMORY        when out of memory
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_CreateElementNS()

=============================================================================

ISVGDOM_GetLocalName()

Description:
  Gets unprefixed Node name.

Prototype:
  int ISVGDOM_GetLocalName ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMString * cpwLocalName );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  cpwLocalName     pointer to returned node name

Return Value:
  SUCCESS          for an SVGElement, returns the tag name without a prefix.
  ENOMEMORY        when out of memory
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetParent()

Description:
  Gets the parent Node of this Node.

Prototype:
  int ISVGDOM_GetParent ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID * pidParent );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  pidParent        pointer to returned parent element

Return Value:
  SUCCESS          with the parent node or NULL if there is no parent (i.e. if a node
                   has just been created and not yet added to the tree, or if it has been
                   removed from the tree)
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_GetFirstChild()
  ISVGDOM_GetLastChild()

=============================================================================

ISVGDOM_GetOwnerDocument()

Description:
  Get the document associated with the element

Prototype:
  int ISVGDOM_GetOwnerDocument ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID * pidOwnerDocument );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  pidOwnerDocument   pointer to returned document

Return Value:
  SUCCESS            with document handle.
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetContent()

Description:
  Get the TextContent of a Node. This function may be called iteratively with a small
  buffer size to retrieve all of the text.

Prototype:
  int ISVGDOM_GetContent ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMString * pwBuf, int nchIndex, int * pnUsed);

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  pwBuf            pointer to returned text
  nchIndex         index to returned data from where to start copying to pwBuf
  pnUsed           pointer to number of AECHARs returned

Return Value:
  SUCCESS          with text content of the Node.
  ENOMEMORY        when out of memory
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_SetContent()

=============================================================================

ISVGDOM_SetContent()

Description:
  Set the TextContent of a Node

Prototype:
  int ISVGDOM_SetContent ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMString * pwBuf, int nchIndex, int * pnSet );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  pwBuf            pointer to source text
  nchIndex         index to souce data from where to start copying
  pnUsed           pointer to number of AECHARs copied

Return Value:
  SUCCESS          if the operation succeeded
  ENOMEMORY        when out of memory
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_GetContent()

=============================================================================

ISVGDOM_AppendChild()

Description:
  Appends a child to this element.

Prototype:
  int ISVGDOM_AppendChild ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  idNewChild       the ID of the element to be appended to idThis.

Return Value:
  SUCCESS                         if the operation succeeded
  SVGDOM_ERROR_HIERARCHY_REQUEST  if this node is of a type that does not allow children of
                                  the type of the idNewChild node, or if the node to append is
                                  one of this node's ancestors or this node itself, or
                                  if this node is of type Document and the DOM application attempts to
                                  append a second Element node.
  SVGDOM_ERROR_WRONG_DOCUMENT     if idNewChild was created from a different document than
                                  the one that created this node.
  EUNSUPPORTED                    if the idNewChild node is a child of the Document node or
                                  if the child is of a type that cannot be created with ISVGDOM_CreateElementNS().
  EBADSTATE                       if the idNewChild node would cause the document to go into error,
                                  e.g. when the idNewChild contains a <use> element with an invalid xlink:href attribute.
  EBADPARM                        arguments are invalid
  EFAILED                         general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_InsertBefore()
  ISVGDOM_RemoveChild()

=============================================================================

ISVGDOM_RemoveChild()

Description:
  Removes the specified child associated with this node.

Prototype:
  int ISVGDOM_RemoveChild ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID idOldChild );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  idOldChild       the ID of the element that is to be removed

Return Value:
  SUCCESS                         if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS     if the element being removed or one of its decendants have non-null id.
  ENOSUCH                         if idOldChild is not a child of this node.
  EUNSUPPORTED                    if this node is of type Document or if the child, or any of its
                                  descendants is of a type that cannot be created with ISVGDOM_CreateElementNS().
  EBADPARM                        arguments are invalid
  EFAILED                         general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_AppendChild()

=============================================================================

ISVGDOM_InsertBefore()

Description:
  Inserts newChild before refChild in the child list for this node.

Prototype:
  int ISVGDOM_InsertBefore ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild, AEEDOMNodeID idRefChild );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  idRefChild       the ID of the element after which idThis should be added

Return Value:
  SUCCESS                         if the operation succeeded
  SVGDOM_ERROR_HIERARCHY_REQUEST  if this node is of a type that does not allow children of the type
                                  of the idNewChild node, or if the node to append is one of this node's
                                  ancestors or this node itself, or if this node is of type Document and
                                  the DOM application attempts to append a second Element node.
  SVGDOM_ERROR_WRONG_DOCUMENT     if idNewChild was created from a different document than the one
                                  that created this node.
  ENOSUCH                         if idRefChild is not a child of this node.
  EUNSUPPORTED                    if the newChild node is a child of the Document node or
                                  if the child is of a type that cannot be created with ISVGDOM_CreateElementNS().
  EBADSTATE                       if the idNewChild node would cause the document to go into error,
                                  e.g.: when the newChild contains a <use> element with an invalid xlink:href attribute.
  EBADPARM                        arguments are invalid
  EFAILED                         general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_AppendChild()
  ISVGDOM_RemoveChild()

=============================================================================

ISVGDOM_GetNextSibling()

Description:
  Gets the next sibling element node of this element.

Prototype:
  int ISVGDOM_GetNextSibling ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID * pidSibling );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  pidSibling       pointer to returned element

Return Value:
  SUCCESS          with the next sibling element node of this element, or
                   NULL if this element has no element sibling nodes that come after
                   this one in the document tree.
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_GetPrevSibling()

=============================================================================

ISVGDOM_GetPrevSibling()

Description:
  Gets the previous sibling element node of this element.

Prototype:
  int ISVGDOM_GetPrevSibling ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID * pidSibling );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  pidSibling       pointer to returned element


Return Value:
  SUCCESS          with the next sibling element node of this element, or
                   NULL if this element has no element sibling nodes that come before
                   this one in the document tree.
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_GetNextSibling()

=============================================================================

ISVGDOM_GetFirstChild()

Description:
  Gets the first child element node of this element.

Prototype:
  int ISVGDOM_GetFirstChild ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID * pidChild );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  pidChild         pointer to returned element

Return Value:
  SUCCESS          with the first child element node of this element, or
                   NULL if this element has no child elements.
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_GetLastChild()

=============================================================================

ISVGDOM_GetLastChild()

Description:
  Gets the last child element node of this element.

Prototype:
  int ISVGDOM_GetLastChild ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID * pidChild );

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  pidChild         pointer to returned element, or
                   NULL if this element has no child elements.

Return Value:
  SUCCESS          with the last child element node of this element.
                   null if this element has no child elements.
  EBADPARM         arguments are invalid
  EFAILED          general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_GetFirstChild()

=============================================================================

ISVGDOM_AddEventListener()

Description:
  Adds a DOM event listener to notify of events of the specified type for the
  given node

Prototype:
  int ISVGDOM_AddEventListener(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMString *eventType,
                               AEEDOMEventListener* listener, boolean useCapture)

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  eventType        the event as a string
  listener         structure describing the listener
  useCapture       TRUE if user wishes to add the event listener for the capture
                   phase only, FALSE otherwise

Return Value:
  SUCCESS          with listener successfully added
  EBADPARAM        arguments are invalid
  ENOMEMORY        when out of memory
  EFAILED          general failure
  EUNSUPPORTED     if not supported on this platform

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None
=============================================================================

ISVGDOM_RemoveEventListener()

Description:
  Removes a DOM event listener of the specified type for the given node

Prototype:
  int ISVGDOM_RemoveEventListener(ISVGDOM *me, AEEDOMNodeID idThis, AEEDOMString *eventType,
                                  AEEDOMEventListener* listener, boolean useCapture)

Parameters:
  pme              this pointer
  idThis           the ID of the element for this operation
  eventType        the event to be removed from the listener as a string
  listener         structure describing the listener
  useCapture       TRUE if user is removing the event listener for the capture
                   phase only, FALSE otherwise

Return Value:
  SUCCESS          with listener successfully removed
  EBADPARAM        arguments are invalid
  ENOMEMORY        when out of memory
  EFAILED          general failure
  EUNSUPPORTED     if not supported on this platform

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None
=============================================================================

ISVGDOM_GetTrait()

Description:
  Gets the element's trait value as a string.

Prototype:
  int ISVGDOM_GetTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, AEEDOMString *pValue);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get, e.g. "rect"
  pValue             pointer to the returned trait value

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to a string.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetTraitNS()

Description:
  Gets the element's namespaced trait value as a string.

Prototype:
  int ISVGDOM_GetTraitNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pNS                the name space
  pName              the name of the trait to get
  pValue             pointer to the returned trait value

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to a string.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetFloatTrait()

Description:
  Gets the element's trait value as a float.

Prototype:
  int ISVGDOM_GetFloatTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, int32 *pf);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  pf                 pointer to the returned trait value

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to a float.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetMatrixTrait()

Description:
  Gets the element's trait value as an SVGMatrix.

Prototype:
  int ISVGDOM_GetMatrixTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGMatrix *pm, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  pm                 pointer to the returned trait value
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to an SVGMatrix.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetRectTrait()

Description:
  Gets the element's trait value as an SVGRect.

Prototype:
  int ISVGDOM_GetRectTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGRect *pr, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  pr                 address of pointer to the returned trait value
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to an SVGRect.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetPathTrait()

Description:
  Gets the element's trait value as an SVGPathHandle.

Prototype:
  int ISVGDOM_GetPathTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGPathHandle *ph, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  ph                 pointer to the returned trait value
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to an SVGPathHandle.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetRGBColorTrait()

Description:
  Gets the element's trait value as an SVGColor.

Prototype:
  int ISVGDOM_GetRGBColorTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGColor *pv, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  pv                 pointer to the returned trait value
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to an SVGColor.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetSVGElementID()

Description:
  Gets the element's id.

Prototype:
  int ISVGDOM_GetSVGElementID(ISVGDOM *me, AEEDOMNodeID idElement, AEEDOMString *pwID);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pwID               pointer to the returned ID

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetPresentationTrait()

Description:
  Gets the element's trait presentation value as a string.

Prototype:
  int ISVGDOM_GetPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, AEEDOMString *pValue);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  pValue             pointer to the returned trait value

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to a string.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetPresentationTraitNS()

Description:
  Gets the element's namespaced trait presentation value as a string.

Prototype:
  int ISVGDOM_GetPresentationTraitNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pNS                the name space
  pName              the name of the trait to get
  pValue             pointer to the returned trait value

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to a string.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetFloatPresentationTrait()

Description:
  Gets the element's trait presentation value as a float.

Prototype:
  int ISVGDOM_GetFloatPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, int32 *pf);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  pf                 pointer to the returned trait value

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to a float.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetMatrixPresentationTrait()

Description:
  Gets the element's trait presentation value as an SVGMatrix.

Prototype:
  int ISVGDOM_GetMatrixPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGMatrix *pm, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  pm                 pointer to the returned trait value
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to an SVGMatrix.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetRectPresentationTrait()

Description:
  Gets the element's trait presentation value as an SVGRect.

Prototype:
  int ISVGDOM_GetRectPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGRect *pr, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  pr                 pointer to the returned trait valuex
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to an SVGRect.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetPathPresentationTrait()

Description:
  Gets the element's trait presentation value as an SVGPathHandle.

Prototype:
  int ISVGDOM_GetPathPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGPathHandle *ph, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  ph                 pointer to the returned trait value
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to an SVGPathHandle.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetRGBColorPresentationTrait()

Description:
  Gets the element's trait presentation value as an SVGColor.

Prototype:
  int ISVGDOM_GetRGBColorPresentationTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGColor *pv, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              the name of the trait to get
  pv                 pointer to the returned trait value
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if the requested trait is not supported on this element or null.
  ENOTYPE            if requested trait's computed value cannot be converted to an SVGColor.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetBBox()

Description:
  Gets the element's tight bounding box in current user coordinate space.

Prototype:
  int ISVGDOM_GetBBox(ISVGDOM *me, AEEDOMNodeID idElement, SVGRect *pr, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pr                 pointer to the returned bounding box
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if bounding is not supported on this element or null.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetScreenCTM()

Description:
  Gets the element's transformation matrix in current user units.

Prototype:
  int ISVGDOM_GetScreenCTM(ISVGDOM *me, AEEDOMNodeID idElement, SVGMatrix *pm, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pm                 pointer to the returned transformation matrix
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if transformation matrix is not supported on this element or null.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetScreenBBox()

Description:
  Gets the element's tight bounding box in screen coordinate space.

Prototype:
  int ISVGDOM_GetScreenBBox(ISVGDOM *me, AEEDOMNodeID idElement, SVGRect *pr, boolean *pIsNull);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pr                 pointer to the returned bounding box
  pIsNull            pointer to a Flag indicating a NULL return

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if bounding box is not supported on this element or null.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  When the API returns SUCCESS, the user is supposed to check the IsNull
  flag for NULL returns

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetCurrentScale()

Description:
  Gets current user agent scale (zoom) coefficient.

Prototype:
  int ISVGDOM_GetCurrentScale(ISVGDOM *me, AEEDOMNodeID idElement, int32 *pf);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pf                 pointer to the returned scale coefficient

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if scaling is not supported on this element or null.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetCurrentRotate()

Description:
  Gets current user agent scale rotation angle in degrees.

Prototype:
  int ISVGDOM_GetCurrentRotate(ISVGDOM *me, AEEDOMNodeID idElement, int32 *pf);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pf                 pointer to the returned rotation angle

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if rotation is not supported on this element or null.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetCurrentTranslate()

Description:
  Gets current user agent translation used for scrolling or panning.

Prototype:
  int ISVGDOM_GetCurrentTranslate(ISVGDOM *me, AEEDOMNodeID idElement, SVGPoint *pPt);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pPt                pointer to the returned translation point

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if translation is not supported on this element or null.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetViewPort()

Description:
  Gets the element's current viewport.

Prototype:
  int ISVGDOM_GetViewPort(ISVGDOM *me, AEEDOMNodeID idElement, SVGRect *pr);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pr                 pointer to the returned viewport rectangle

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if viewport is not supported on this element or null.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetCurrentTime()

Description:
  Gets current animation timeline time in seconds.

Prototype:
  int ISVGDOM_GetCurrentTime(ISVGDOM *me, AEEDOMNodeID idElement, int32 *pf);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pf                 pointer to the returned time value

Return Value:
  SUCCESS            with trait value
  EUNSUPPORTED       if time is not supported on this element or null.
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_GetAttributeNS()

Description:
  Gets the element's namespaced attribute value as a string.

Prototype:
  int ISVGDOM_GetAttributeNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pNS                the name space
  pName              the name of the attribute to get
  pValue             pointer to the returned attribute value

Return Value:
  SUCCESS            with attribute value
  EUNSUPPORTED       if the implementation does not support the feature "XML"
                     and the language exposed through the Document does not support XML Namespaces
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_IsInsertedInTree()

Description:
  Returns TRUE if the given node is part of the DOM tree, and FALSE otherwise

Prototype:
  int ISVGDOM_IsInsertedInTree(ISVGDOM *me, AEEDOMNodeID node, boolean* isInserted)

Parameters:
  pme                this pointer
  node               the ID of the node for this operation
  isInserted         boolean indicating whether node is inserted or not

Return Value:
  SUCCESS            with correct boolean value
  EUNSUPPORTED       if the implementation does not support this functionality
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None
=============================================================================

ISVGDOM_SetNodeLock()

Description:
    Used to toggle the lock state of a node.
    When a node is locked, the underlying implementation will not delete any
    memory associated with the node when a delete operation is encountered for
    the node. When the node is unlocked, subsequent delete operations on the
    node will free the memory. This is useful for garbage collecting systems
    such as Java, where the order of deleting nodes may not be predicatable.
    For example, if there's a parent node with a child, and the parent is
    deleted, the child node is also deleted. If later the garbage collector
    attempts to delete the child, a double-free will have occurred on the child.
    If however, the node lock is set on the child, then when the parent is deleted,
    the child node is not deleted at that time. The future invokation of the
    the garbage collector on the child node, should first remove the node lock, then
    delete the child node.

Prototype:
  int ISVGDOM_SetNodeLock(ISVGDOM *me, AEEDOMNodeID node, boolean lock)

Parameters:
  me                 this pointer
  node               the ID of the node for this operation
  lock               TRUE to lock the node, FALSE to unlock the node

Return Value:
  SUCCESS            if node locked successfully
  EUNSUPPORTED       if the implementation does not support this functionality
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================


ISVGDOM_PathGetNumberOfSegments()

Description:
  Gets the number of segments in this path.

Prototype:
  int ISVGDOM_PathGetNumberOfSegments(ISVGDOM *me, SVGPathHandle hThis, int *pnSegments);

Parameters:
  pme                this pointer
  hThis              the path handle for this operation
  pnSegments         pointer to the returned segment count

Return Value:
  SUCCESS            with trait value
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_PathGetSegment()

Description:
  Gets path segment by zero-based command index.

Prototype:
  int ISVGDOM_PathGetSegment(ISVGDOM *me, SVGPathHandle hThis, int nIdx, int32 *pfParam);

Parameters:
  pme                this pointer
  hThis              the path handle for this operation
  nIdx               index of the segment to get
  pfParam            pointer to the returned path segment (SVGDOM_PATH_MOVE_TO/SVGDOM_PATH_LINE_TO/...)

Return Value:
  SUCCESS                   with trait value
  SVGDOM_ERROR_INDEX_SIZE   if segment index out of bounds.
  ENOMEMORY                 when out of memory
  EBADPARM                  if arguments are invalid
  EFAILED                   general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_PathGetSegmentParam()

Description:
  Gets segment parameter by zero-based command index and zero-based parameter index.

Prototype:
  int ISVGDOM_PathGetSegmentParam(ISVGDOM *me, SVGPathHandle hThis, int nCmdIdx, int nParamIdx, int32 *pfParam);

Parameters:
  pme                this pointer
  hThis              the path handle for this operation
  nCmdIdx            command index
  nParamIdx          index of the segment
  pfParam            pointer to the returned segement parameter

Return Value:
  SUCCESS                   with trait value
  SVGDOM_ERROR_INDEX_SIZE   if segment index out of bounds or param index out of bounds for this segment's type
  ENOMEMORY                 when out of memory
  EBADPARM                  if arguments are invalid
  EFAILED                   general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_MatrixGetComponent()

Description:
  Gets a component of the matrix by component's zero-based index.

Prototype:
  int ISVGDOM_MatrixGetComponent(ISVGDOM *me, SVGMatrix *pThis, int nIndex, int32 *pf);

Parameters:
  pme                this pointer
  pThis              the matrix for this operation
  nIndex             index of matrix component to get
  pf                 pointer to the returned matrix component

Return Value:
  SUCCESS                   with trait value
  SVGDOM_ERROR_INDEX_SIZE   if the index is invalid.
  ENOMEMORY                 when out of memory
  EBADPARM                  if arguments are invalid
  EFAILED                   general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_MatrixMultiply()

Description:
  Matrix multiplication operation.

Prototype:
  int ISVGDOM_MatrixMultiply(ISVGDOM *me, SVGMatrix *pThis, const SVGMatrix *pSecond);

Parameters:
  pme                this pointer
  pThis              first and returned matrix for this multiplication
  pSecond            second matrix for this multiplication

Return Value:
  SUCCESS            with trait value
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_MatrixInverse()

Description:
  Matrix inversion operation.

Prototype:
  int ISVGDOM_MatrixInverse(ISVGDOM *me, SVGMatrix *pThis);

Parameters:
  pme                this pointer
  pThis              matrix to be inverted

Return Value:
  SUCCESS                              with trait value
  SVGDOM_ERROR_MATRIX_NOT_INVERTABLE   if a DOM matrix cannot be inverted.
  ENOMEMORY                            when out of memory
  EBADPARM                             if arguments are invalid
  EFAILED                              general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_MatrixTranslate()

Description:
  Post-multiplies a translation transformation on the current matrix.

Prototype:
  int ISVGDOM_MatrixTranslate(ISVGDOM *me, SVGMatrix *pThis, uint32 x, uint32 y);

Parameters:
  pme                this pointer
  pThis              matrix to be translated
  x                  x coordinate for the translation
  y                  y coordinate for the translation

Return Value:
  SUCCESS            with trait value
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_MatrixScale()

Description:
  Post-multiplies a uniform scale transformation on the current matrix.

Prototype:
  int ISVGDOM_MatrixScale(ISVGDOM *me, SVGMatrix *pThis, uint32 dwScaleFactor);

Parameters:
  pme                this pointer
  pThis              matrix to be scaled
  dwScaleFactor      scaling constant

Return Value:
  SUCCESS            with trait value
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_MatrixRotate()

Description:
  Post-multiplies a rotation transformation on the current matrix.

Prototype:
  int ISVGDOM_MatrixRotate(ISVGDOM *me, SVGMatrix *pThis, uint32 fAngle);

Parameters:
  pme                this pointer
  pThis              matrix to be rotated
  fAngle             rotation angle

Return Value:
  SUCCESS            with trait value
  ENOMEMORY          when out of memory
  EBADPARM           if arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetTrait()

Description:
  Sets the element's trait value as a string.

Prototype:
  int ISVGDOM_SetTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, AEEDOMString *pValue);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              name of the trait to set, e.g. "rect"
  pValue             pointer to trait value

Return Value:
  SUCCESS                           if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS       if the input value is an invalid value for the given trait or null
  ENOTALLOWED                       if an attempt is made to change a readonly trait.
  ENOTYPE                           if the requested trait's value cannot be represented as a string.
  EUNSUPPORTED                      if the requested trait is not supported on this element or null.
  ENOMEMORY                         out of memory
  EBADPARM                          idElement is invalid
  EFAILED                           general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetTraitNS()

Description:
  Sets the element's namespaced trait value as a string.

Prototype:
  int ISVGDOM_SetTraitNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pNS                the name space
  pName              name of the trait to set
  pValue             pointer to trait value

Return Value:
  SUCCESS                           if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS       if the input value is an invalid value for the given trait or null,
                                    or when the <use> element is hooked into the document tree and
                                    the value of xlink:href is set invalid.
  ENOTALLOWED                       if an attempt is made to change a readonly trait.
  ENOTYPE                           if the requested trait's value cannot be represented as a string.
  EUNSUPPORTED                      if the requested trait is not supported on this element or null.
  ENOMEMORY                         when out of memory
  EBADPARM                          if idElement is invalid
  EFAILED                           general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetFloatTrait()

Description:
  Sets the element's trait value as float.

Prototype:
  int ISVGDOM_SetFloatTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, int32 f);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              name of the trait to set
  f                  float value

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid value for the given trait or null
  ENOTYPE                       if the requested trait's value cannot be specified as a numeric float
  EUNSUPPORTED                  if float trait is not supported on this element or null.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetMatrixTrait()

Description:
  Sets the element's trait value as SVGMatrix.

Prototype:
  int ISVGDOM_SetMatrixTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGMatrix *pm);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              name of the trait to set
  pm                 pointer to matrix value

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is null
  ENOTYPE                       if the requested trait's value cannot be specified as an SVGMatrix.
  EUNSUPPORTED                  if the matrix trait is not supported on this element or null.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetRectTrait()

Description:
  Sets the element's trait value as SVGRect.

Prototype:
  int ISVGDOM_SetRectTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGRect *pr);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              name of the trait to set
  pr                 pointer to rectangle value

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid value for the given trait or null.
                                An SVGRect is invalid if the width or height values are set to negative.
  ENOTYPE                       if the requested trait's value cannot be specified as an SVGRect.
  EUNSUPPORTED                  if the rectangle trait is not supported on this element or null.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetPathTrait()

Description:
  Sets the element's trait value as SVGPathHandle.

Prototype:
  int ISVGDOM_SetPathTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGPathHandle h);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              name of the trait to set
  h                  path handle

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid value for the given trait or null.
                                An SVGPath is invalid if it begins with any segment other than MOVE_TO segment.
  ENOTYPE                       if the requested trait's value cannot be specified as an SVGPath.
  EUNSUPPORTED                  if path trait is not supported on this element or null.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetRGBColorTrait()

Description:
  Sets the element's trait value as SVGColor.

Prototype:
  int ISVGDOM_SetRGBColorTrait(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pName, SVGColor *v);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              name of the trait to set
  v                  pointer to the RGB color value

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is null
  ENOTYPE                       if the requested trait's value cannot be specified as an SVGColor.
  EUNSUPPORTED                  if color trait is not supported on this element or null.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetSVGElementID()

Description:
  Sets the element's id attribute.

Prototype:
  int ISVGDOM_SetSVGElementID(ISVGDOM *me, AEEDOMNodeID idElement, AEEDOMString *pwID);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pName              name of the trait to set
  pwID               pointer to element ID attribute

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid element id or null.
  EUNSUPPORTED                  if element id is not supported on this element.
  ENOTALLOWED                   if element id cannot be set on this element.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetCurrentScale()

Description:
  Sets current user agent scale (zoom) coefficient.

Prototype:
  int ISVGDOM_SetCurrentScale(ISVGDOM *me, AEEDOMNodeID idElement, int32 f);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  f                  scaling constant

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the scale value is set to zero,
  EUNSUPPORTED                  if scaling is not supported on this element.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetCurrentRotate()

Description:
  Sets current user agent rotation angle in degrees.

Prototype:
  int ISVGDOM_SetCurrentRotate(ISVGDOM *me, AEEDOMNodeID idElement, int32 f);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  f                  rotation angle

Return Value:
  SUCCESS                       if the operation succeeded
  EUNSUPPORTED                  if rotation is not supported on this element.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetCurrentTranslate()

Description:
  Sets current user agent translation used for scrolling or panning.

Prototype:
  int ISVGDOM_SetCurrentTranslate(ISVGDOM *me, AEEDOMNodeID idElement, SVGPoint *pPt);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pPt                pointer to translation point

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid or null.
  EUNSUPPORTED                  if translation is not supported on this element.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetViewPort()

Description:
  Sets the element's current viewport.

Prototype:
  int ISVGDOM_SetViewPort(ISVGDOM *me, AEEDOMNodeID idElement, SVGRect *pr);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pr                 pointer to viewport rectangle

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid value or null.
  EUNSUPPORTED                  if viewport is not supported on this element.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetCurrentTime()

Description:
  Sets current animation timeline time.

Prototype:
  int ISVGDOM_SetCurrentTime(ISVGDOM *me, AEEDOMNodeID idElement, int32 f);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  f                  animation timeline value

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid value.
  EUNSUPPORTED                  if time is not supported on this element.
  ENOMEMORY                     when out of memory
  EBADPARM                      if idElement is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_SetAttributeNS()

Description:
  Sets the element's namespaced attribute value as a string.

Prototype:
  int ISVGDOM_SetAttributeNS(ISVGDOM *me, AEEDOMNodeID idElement, const AEEDOMString *pNS, const AEEDOMString *pName, AEEDOMString *pValue);

Parameters:
  pme                this pointer
  idElement          the ID of the element for this operation
  pNS                the name space
  pName              name of the attribute to set
  pValue             pointer to attribute value

Return Value:
  SUCCESS            if the operation succeeded
  EUNSUPPORTED       if the implementation does not support the feature "XML"
                     and the language exposed through the Document does not support XML Namespaces
  ENOMEMORY          when out of memory
  EBADPARM           if idElement is invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_PathMoveTo()

Description:
  Appends 'M' (absolute move) segment to the path.

Prototype:
  int ISVGDOM_PathMoveTo(ISVGDOM *me, SVGPathHandle hThis, SVGPoint *p);

Parameters:
  pme                this pointer
  hThis              the path handle for this operation
  p                  pointer to the destination point

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid value or null.
  EUNSUPPORTED                  if path is not supported on this element.
  ENOMEMORY                     out of memory
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_PathLineTo()

Description:
  Appends 'L' (absolute line) segment to the path.

Prototype:
  int ISVGDOM_PathLineTo(ISVGDOM *me, SVGPathHandle hThis, SVGPoint *p);

Parameters:
  pme                this pointer
  hThis              the path handle for this operation
  p                  pointer to the destination point

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid value or null.
  EUNSUPPORTED                  if path is not supported on this element.
  ENOMEMORY                     when out of memory
  EBADPARM                      if hThis is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_PathQuadTo()

Description:
  Appends 'Q' (absolute quadratic curve) segment to the path.

Prototype:
  int ISVGDOM_PathQuadTo(ISVGDOM *me, SVGPathHandle hThis, SVGPoint *p1, SVGPoint *p2);

Parameters:
  pme                this pointer
  hThis              the path handle for this operation
  p1                 pointer to the control point
  p2                 pointer to the destination point

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid value or null.
  EUNSUPPORTED                  if path is not supported on this element.
  ENOMEMORY                     when out of memory
  EBADPARM                      if hThis is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_PathCurveTo()

Description:
  Appends 'C' (absolute cubic curve) segment to the path.

Prototype:
  int ISVGDOM_PathCurveTo(ISVGDOM *me, SVGPathHandle hThis, SVGPoint *p1, SVGPoint *p2, SVGPoint *p3);

Parameters:
  pme                this pointer
  hThis              the path handle for this operation
  p1                 pointer to the control point at the beginning of the curve
  p2                 pointer to the control point at the end of the curve
  p3                 pointer to the destination point

Return Value:
  SUCCESS                       if the operation succeeded
  SVGDOM_ERROR_INVALID_ACCESS   if the input value is an invalid value or null.
  EUNSUPPORTED                  if path is not supported on this element.
  ENOMEMORY                     when out of memory
  EBADPARM                      if hThis is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================
ISVGDOM_PathClose()

Description:
  Appends 'Z' (close path) segment to the path.

Prototype:
  int ISVGDOM_PathClose(ISVGDOM *me, SVGPathHandle hThis);

Parameters:
  pme                this pointer
  hThis              the path handle for this operation

Return Value:
  SUCCESS                       if the operation succeeded
  ENOMEMORY                     when out of memory
  EBADPARM                      if hThis is invalid
  EFAILED                       general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  None

=============================================================================

ISVGDOM_BeginElement()

Description:
  Causes this element to begin it's animation.

Prototype:
  int ISVGDOM_BeginElement ( ISVGDOM * pme,  AEEDOMNodeID idThis );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_BeginElementAt()

=============================================================================

ISVGDOM_BeginElementAt()

Description:
  Causes this element to begin the local timeline at a given offset.

Prototype:
  int ISVGDOM_BeginElementAt ( ISVGDOM * pme,  AEEDOMNodeID idThis, uint32 dwFixedOffset );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  dwFixedOffset      time offset

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_BeginElement()

=============================================================================

ISVGDOM_EndElement()

Description:
  Causes this element to end it's animation.

Prototype:
  int ISVGDOM_EndElement ( ISVGDOM * pme, AEEDOMNodeID idThis );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_EndElementAt()

=============================================================================

ISVGDOM_EndElementAt()

Description:
  Causes this element to end the local timeline at the specified offset (in seconds)
  from the current time when the method is called.

Prototype:
  int ISVGDOM_EndElementAt ( ISVGDOM * pme, AEEDOMNodeID idThis, uint32 dwFixedOffset );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  dwFixedOffset      time offset

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_EndElement()

=============================================================================

ISVGDOM_PauseElement()

Description:
  Causes this elements animation to get suspended.

Prototype:
  int ISVGDOM_PauseElement ( ISVGDOM * pme, AEEDOMNodeID idThis );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_ResumeElement()

=============================================================================

ISVGDOM_ResumeElement()

Description:
  Causes this elements animation to continue.

Prototype:
  int ISVGDOM_ResumeElement ( ISVGDOM * pme, AEEDOMNodeID idThis );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_PauseElement()

=============================================================================

ISVGDOM_IsPaused()

Description:
  Check this elements animation state

Prototype:
  int ISVGDOM_IsPaused ( ISVGDOM * pme, AEEDOMNodeID idThis, boolean * pbPaused );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  pbPaused           pointer to returned boolean

Return Value:
  SUCCESS            with TRUE if animation is paused.
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_PauseElement()

=============================================================================

ISVGDOM_CreateSVGMatrix()

Description:
  Creates a new matrix component.

Prototype:
  int ISVGDOM_CreateSVGMatrix ( ISVGDOM * pme, AEEDOMNodeID idThis, SVGMatrix * pIn, SVGMatrix **ppOut );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  pIn                input SVGMatrix
  ppOut              pointer to returned SVGDOM owned SVGMatrix

Return Value:
  SUCCESS            with the created matrix.
  ENOMEMORY          when out of memory
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_FreeSVGMatrix()

=============================================================================

ISVGDOM_FreeSVGMatrix()

Description:
  Free a matrix component.

Prototype:
  int ISVGDOM_FreeSVGMatrix ( ISVGDOM * pme, AEEDOMNodeID idThis, SVGMatrix * pMatrix );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  pMatrix            pointer to SVGDOM owned SVGMatrix

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_CreateSVGMatrix()

=============================================================================

ISVGDOM_CreateSVGRect()

Description:
  Creates a new rectangle component.

Prototype:
  int ISVGDOM_CreateSVGRect ( ISVGDOM * pme, AEEDOMNodeID idThis, SVGRect ** ppRect );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  ppRect             pointer to returned SVGDOM owned SVGRect

Return Value:
  SUCCESS            with the created rectangle
  ENOMEMORY          when out of memory
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_FreeSVGRect()

=============================================================================

ISVGDOM_FreeSVGRect()

Description:
  Frees a rectangle component.

Prototype:
  int ISVGDOM_FreeSVGRect ( ISVGDOM * pme, AEEDOMNodeID idThis, SVGRect * pRect );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  pRect              pointer to SVGDOM owned SVGRect

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_CreateSVGRect()

=============================================================================

ISVGDOM_CreateSVGPath()

Description:
  Creates a new path component.

Prototype:
  int ISVGDOM_CreateSVGPath ( ISVGDOM *pme, AEEDOMNodeID idThis, SVGPathHandle *phPath );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  phPath             pointer to returned SVGDOM owned SVGPathHandle

Return Value:
  SUCCESS            with the created path.
  ENOMEMORY          when out of memory
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_FreeSVGPath()

=============================================================================

ISVGDOM_FreeSVGPath()

Description:
  Frees a path component.

Prototype:
  int ISVGDOM_FreeSVGPath ( ISVGDOM * pme, AEEDOMNodeID idThis, SVGPathHandle hPath );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  hPath              pointer to SVGDOM owned SVGPathHandle

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_CreateSVGPath()

=============================================================================

ISVGDOM_CreateSVGRGBColor()

Description:
  Creates a new color component.

Prototype:
  int ISVGDOM_CreateSVGRGBColor ( ISVGDOM * pme, AEEDOMNodeID idThis, RGBVAL v, SVGColor ** pvOut );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  v                  input color
  pvOut              pointer to returned SVGDOM owned color

Return Value:
  SUCCESS            with the created RGB color.
  ENOMEMORY          when out of memory
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_FreeSVGRGBColor()

=============================================================================

ISVGDOM_FreeSVGRGBColor()

Description:
  Frees a color component.

Prototype:
  int ISVGDOM_FreeSVGRGBColor ( ISVGDOM * pme, AEEDOMNodeID idThis, SVGColor * pv );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  pv                 pointer to SVGDOM owned color

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_CreateSVGRGBColor()

=============================================================================

ISVGDOM_SetFocus()

Description:
  Sets focus on an element.

Prototype:
  int ISVGDOM_SetFocus ( ISVGDOM * pme, AEEDOMNodeID idThis );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation

Return Value:
  SUCCESS            if the operation succeeded
  EUNSUPPORTED       if the element is not focusable
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_MoveFocus()

=============================================================================

ISVGDOM_MoveFocus()

Description:
  Moves focus to an element.

Prototype:
  int ISVGDOM_MoveFocus ( ISVGDOM * pme, AEEDOMNodeID idThis, int nMotionType );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  nMotionType        the type of motion, the one of the below:
                        * SVGDOM_NAV_AUTO
                        * SVGDOM_NAV_NEXT
                        * SVGDOM_NAV_PREV
                        * SVGDOM_NAV_UP
                        * SVGDOM_NAV_UP_RIGHT
                        * SVGDOM_NAV_RIGHT
                        * SVGDOM_NAV_DOWN_RIGHT
                        * SVGDOM_NAV_DOWN
                        * SVGDOM_NAV_DOWN_LEFT
                        * SVGDOM_NAV_LEFT
                        * SVGDOM_NAV_UP_LEFT

Return Value:
  SUCCESS            if the operation succeeded
  EUNSUPPORTED       if the element is not focusable
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_SetFocus()

=============================================================================

ISVGDOM_GetCurrentFocusedObject()

Description:
  Get the element that current has the focus.

Prototype:
  static int OEMSVGDOM_GetCurrentFocusedObject ( ISVGDOM * pme, AEEDOMNodeID idThis, AEEDOMNodeID * pidElement );

Parameters:
  pme                this pointer
  idThis             the ID of the element for this operation
  pidElement         pointer to returned element

Return Value:
  SUCCESS            with currently focused element.
  EBADPARM           arguments are invalid
  EFAILED            general failure

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_SetFocus()

=============================================================================

ISVGDOM_StopPropagation()

Description:
  Prevent event listeners of the same group to be triggered

Prototype:
  static int ISVGDOM_StopPropagation(ISVGDOM *pme, SVGEvent * pEvent);

Parameters:
  pme                this pointer
  pEvent             pointer to a valid SVGEvent datastructure

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure
  EUNSUPPORTED       functionality not supported

Comments:
  None

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_AddEventListener()

=============================================================================

ISVGDOM_PreventDefault()

Description:
  Prevents any default action on occurrence of this event.

Prototype:
  static int ISVGDOM_PreventDefault(ISVGDOM *pme, SVGEvent * pEvent);

Parameters:
  pme                this pointer
  pEvent             pointer to a valid SVGEvent datastructure

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure
  EUNSUPPORTED       functionality not supported

Comments:
  For non-cancelable events, this API has no effect.

Version:
  Introduced BREW Client 3.1.5

See Also:
  ISVGDOM_AddEventListener()

=============================================================================

ISVGDOM_GoToLocation()

Description:
  Traverse to the inputted IRI.

Prototype:
  static int ISVGDOM_GoToLocation(ISVGDOM *pme, AEEDOMString * pwszNewIRI);

Parameters:
  pme                this pointer
  pwszNewURI         pointer to the IRI to be traversed.

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure
  EUNSUPPORTED       functionality not supported

Comments:
  None.

Version:
  Introduced BREW Client 3.1.5

See Also:
  None.

=============================================================================

ISVGDOM_GetURL()

Description:
  Attempts to fetch the resource at the inputted IRI using the HTTP GET method.
  Once the fetch completes, the callback is called.

Prototype:
  static int ISVGDOM_GetURL(ISVGDOM *pme, AEEDOMString * pwszIRI, AEEDOMAsyncStatusCallback * pfnCallback);

Parameters:
  pme                this pointer
  pwszIRI            pointer to the IRI of the resource that is being requested.
  pfnCallback        The callback that is called upon completion of the request.

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure
  EUNSUPPORTED       functionality not supported

Comments:
  None.

Version:
  Introduced BREW Client 3.1.5

See Also:
  None.

=============================================================================

ISVGDOM_PostURL()

Description:
  Sends the data to the inputted IRI as the body of an HTTP POST request using
  the requested media type and content encoding. Once the post completes,
  the callback is called.

Prototype:
  static int ISVGDOM_PostURL(ISVGDOM *pme, AEEDOMString * pwszIRI, AEEDOMString * pwszData, AEEDOMAsyncStatusCallback * pfnCallback, AEEDOMString * pwszType, AEEDOMString * pwszEncoding);

Parameters:
  pme                this pointer
  pwszIRI            pointer to the IRI of the resource that is being requested.
  pwszData           pointer to the data that will be the body of the POST request.
  pfnCallback        The callback that is called upon completion of the request.
  pwszType           pointer to the content type of the POST request.
  pwszEncoding       pointer to the encoding of the POST request.

Return Value:
  SUCCESS            if the operation succeeded
  EBADPARM           arguments are invalid
  EFAILED            general failure
  EUNSUPPORTED       functionality not supported

Comments:
  None.

Version:
  Introduced BREW Client 3.1.5

See Also:
  None.

============================================================================= */

#endif // AEEISVGDOM_H

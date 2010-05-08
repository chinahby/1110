/*=====================================================================

             B R E W   S V G  D O M  S E R V I C E S

FILE
    OEMSVGDOM.h

GENERAL DESCRIPTION
   This file defines DOM (Document Object Model) services
   for Scalable Vector Graphics (SVG) media
   
EXTERNALIZED FUNCTIONS
    None
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

      Copyright (c) 2006 by QUALCOMM, Inc.  All Rights Reserved.
======================================================================*/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/graphics/svg/platform/brew/isvgdom/main/latest/inc/OEMSVGDOM.h#1 $
  $DateTime: 2008/06/24 02:53:37 $
  $Author: ranjhith $ 

when       who      what, where, why 
--------   ---      -------------------------------------------------------- 
05/30/06   ae       Initial version of file.

======================================================================*/ 

#ifndef OEMSVG_OEMSVGDOM_H
#define OEMSVG_OEMSVGDOM_H

/*=====================================================================
                     INCLUDE FILES FOR MODULE
======================================================================*/

#include "AEEISVGDOM.h"
#include "AEESVGDOM.BID"
#include "OEMSVG.h"

/*=====================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
======================================================================*/

// copy struct from SVGALMatrix to BREW SVGMatrix
#define COPY_MATRIX(matrix_out, matrix_in) \
    (matrix_out).a = (matrix_in)->a;       \
    (matrix_out).b = (matrix_in)->b;       \
    (matrix_out).c = (matrix_in)->c;       \
    (matrix_out).d = (matrix_in)->d;       \
    (matrix_out).e = (matrix_in)->e;       \
    (matrix_out).f = (matrix_in)->f;       \

// copy struct from SVGALRect to BREW SVGRect
#define COPY_RECT(rect_out, rect_in) \
    (rect_out).x  = rect_in->x;      \
    (rect_out).y  = rect_in->y;      \
    (rect_out).dx = rect_in->dx;     \
    (rect_out).dy = rect_in->dy;     \

// copy struct from SVGALColor24 to BREW SVGColor
#define COPY_COLOR(color_out, color_in) \
    (color_out).red   = (color_in)->r;  \
    (color_out).green = (color_in)->g;  \
    (color_out).blue  = (color_in)->b;  \

// copy struct from SVGALPoint to BREW SVGPoint
#define COPY_POINT(point_out, point_in) \
    (point_out).cx   = (point_in)->x;  \
    (point_out).cy  = (point_in)->y;  \

/*=====================================================================
               FUNCTION DECLARATIONS FOR MODULE
======================================================================*/

int  CSVGDOMInit(CMediaSVG* svg, CSVGDOM** domOut);
void CSVGDOMFreeDom(CSVGDOM* dom);

uint32  OEMCSVGDOMAddRef(ISVGDOM* iDom);
uint32  OEMCSVGDOMRelease(ISVGDOM* iDom);
int OEMCSVGDOMQueryInterface(ISVGDOM* iDom, AEECLSID clsId, void** outInterface);
int OEMCSVGDOMCreateElementNS(ISVGDOM *po, const AEEDOMString *cpwNamespaceURI, const AEEDOMString *cpwQualifiedName, AEEDOMNodeID *pidElement);
int OEMCSVGDOMGetDocumentElement(ISVGDOM *po, AEEDOMNodeID *pidRoot);
int OEMCSVGDOMGetElementById(ISVGDOM *po, const AEEDOMString *cpwId, AEEDOMNodeID *pidElement);
int OEMCSVGDOMDeleteElement(ISVGDOM *po, AEEDOMNodeID pidElement);
int OEMCSVGDOMGetNamespaceURI(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *cpwNamespaceURI);
int OEMCSVGDOMGetLocalName(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *cpwLocalName);
int OEMCSVGDOMGetParent(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidParent);
int OEMCSVGDOMGetOwnerDocument(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidOwnerDocument);
int OEMCSVGDOMGetContent(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *pwBuf, int nchIndex, int *pnUsed);
int OEMCSVGDOMSetContent(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *pwBuf, int nchIndex, int *pnSet);
int OEMCSVGDOMAppendChild(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild);
int OEMCSVGDOMRemoveChild(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID idOldChild);
int OEMCSVGDOMInsertBefore(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild, AEEDOMNodeID idRefChild);
int OEMCSVGDOMGetNextSibling(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidSibling);
int OEMCSVGDOMGetPrevSibling(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidSibling);
int OEMCSVGDOMGetFirstChild(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidChild);
int OEMCSVGDOMGetLastChild(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidChild);
int OEMCSVGDOMGetModel(ISVGDOM *po, AEEDOMNodeID idThis, IModel **ppModel);
int OEMCSVGDOMGetAttr(ISVGDOM *po, uint32 idThis, int32 nAttrID, void *p1,void *p2, void *p3);
int OEMCSVGDOMSetAttr(ISVGDOM *po, uint32 idThis, int32 nAttrID, void *p1, void *p2, void *p3);
int OEMCSVGDOMBeginElement(ISVGDOM *po,  AEEDOMNodeID idThis);
int OEMCSVGDOMBeginElementAt(ISVGDOM *po,  AEEDOMNodeID idThis,uint32 dwFixedOffset);
int OEMCSVGDOMEndElement(ISVGDOM *po, AEEDOMNodeID idThis);
int OEMCSVGDOMEndElementAt(ISVGDOM *po, AEEDOMNodeID idThis, uint32 dwFixedOffset);
int OEMCSVGDOMPauseElement(ISVGDOM *po, AEEDOMNodeID idThis);
int OEMCSVGDOMResumeElement(ISVGDOM *po, AEEDOMNodeID idThis);
int OEMCSVGDOMIsPaused(ISVGDOM *po, AEEDOMNodeID idThis, boolean *pbPaused);
int OEMCSVGDOMCreateSVGMatrix(ISVGDOM *po, AEEDOMNodeID idThis, SVGMatrix *pIn, SVGMatrix **ppOut);
int OEMCSVGDOMFreeSVGMatrix(ISVGDOM *po, AEEDOMNodeID idThis, SVGMatrix *pMatrix);
int OEMCSVGDOMCreateSVGRect(ISVGDOM *po, AEEDOMNodeID idThis, SVGRect **ppRect);
int OEMCSVGDOMFreeSVGRect(ISVGDOM *po, AEEDOMNodeID idThis, SVGRect *pRect);
int OEMCSVGDOMCreateSVGPath(ISVGDOM *po, AEEDOMNodeID idThis, SVGPathHandle *phPath);
int OEMCSVGDOMFreeSVGPath(ISVGDOM *po, AEEDOMNodeID idThis, SVGPathHandle hPath);
int OEMCSVGDOMCreateSVGRGBColor(ISVGDOM *po, AEEDOMNodeID idThis, RGBVAL v, SVGColor **pvOut);
int OEMCSVGDOMFreeSVGRGBColor(ISVGDOM *po, AEEDOMNodeID idThis, SVGColor *pv);
int OEMCSVGDOMSetFocus(ISVGDOM *po, AEEDOMNodeID idThis);
int OEMCSVGDOMMoveFocus(ISVGDOM *po, AEEDOMNodeID idThis, int nMotionType);
int OEMCSVGDOMFocusOn(ISVGDOM *po, AEEDOMNodeID idThis);
int OEMCSVGDOMGetCurrentFocusedObject(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidElement);
int OEMCSVGDOMAddEventListener(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *pEventType, 
                                AEEDOMEventListener * pListener, boolean useCapture);
int OEMCSVGDOMRemoveEventListener(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *pEventType, 
                                AEEDOMEventListener * pListener, boolean useCapture);

int OEMCSVGDOMStopPropagation(ISVGDOM *me, SVGEvent * pEvent);
int OEMCSVGDOMPreventDefault(ISVGDOM *me, SVGEvent * pEvent);
int OEMCSVGDOMGoToLocation(ISVGDOM *me, AEEDOMString * pwszNewIRI);
int OEMCSVGDOMGetURL(ISVGDOM *me, AEEDOMString * pwszIRI, AEEDOMAsyncStatusCallback * pfnCallback);
int OEMCSVGDOMPostURL(ISVGDOM *me, AEEDOMString * pwszIRI, AEEDOMString * pwszData, AEEDOMAsyncStatusCallback * pfnCallback, AEEDOMString * pwszType, AEEDOMString * pwszEncoding);

#endif //OEMSVG_OEMSVGDOM_H


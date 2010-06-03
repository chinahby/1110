/*=====================================================================

             B R E W   S V G  D O M  S E R V I C E S

FILE
    OEMSVGDOM.c

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

  $Header: //source/qcom/qct/multimedia/graphics/svg/platform/brew/isvgdom/main/latest/src/OEMSVGDOM.c#4 $
  $DateTime: 2008/08/01 03:30:54 $
  $Author: ranjhith $

when       who      what, where, why
--------   ---      --------------------------------------------------------
05/30/06   ae       Initial version of file.

======================================================================*/

/*=====================================================================
                     INCLUDE FILES FOR MODULE
======================================================================*/

#include "OEMSVGDOM.h"
#include "OEMSVG.h"
#include "svgengineALpublic.h"
#include "AEEMedia.h"

/*=====================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
======================================================================*/

/*=====================================================================
               FUNCTION DECLARATIONS FOR MODULE
======================================================================*/


SVGAL CSVGDOMAddLiveElement(
    CSVGDOM *       pDom, 
    AEEDOMNodeID    Element);
    
SVGAL CSVGDOMRemoveLiveElement(
    CSVGDOM *       pDom, 
    AEEDOMNodeID    Element);

SVGAL CSVGDOMDestroyLiveElements(
    CSVGDOM *       pDom);

SVGAL CSVGDOMCreateEventContext(
    CSVGDOM *             pDom, 
    AEEDOMNodeID          NodeID,
    AEEDOMString *        pEventType,
    AEEDOMEventListener * pListener,
    boolean               useCapture,
    SVGDOMEventContext ** ppEventContext);

SVGAL CSVGDOMDeleteEventContext(
    CSVGDOM *             pDom, 
    AEEDOMNodeID          NodeID,
    AEEDOMString *        pEventType,
    AEEDOMEventListener * pListener,
    boolean               useCapture,
    SVGDOMEventContext *  pEventContext);

SVGAL CSVGDOMGetEventContext(
    CSVGDOM *             pDom, 
    AEEDOMNodeID          NodeID,
    AEEDOMString *        pEventType,
    AEEDOMEventListener * pListener,
    boolean               useCapture,
    SVGDOMEventContext ** ppEventContext);

SVGAL CSVGDOMAddLiveEventContext(
    CSVGDOM *             pDom,
    SVGDOMEventContext *  pEventContext);

SVGAL CSVGDOMRemoveLiveEventContext(
    CSVGDOM *             pDom,
    SVGDOMEventContext *  pEventContext);

SVGAL CSVGDOMDestroyLiveEventContexts(
    CSVGDOM *             pDom);

int32 OEMSVGDOM_GetStrLength(
  SVGALDOMStrBuf strBuf);

void OEMSVGDOM_EventHandler(
    void *                pUserData,
    SVGALDOMEvent *       pInEvt);

int CSVGDOMHandleMatrix(
    ISVGDOM * po, 
    uint32    idThis, 
    int32     nAttrID, 
    void *    p1,
    void *    p2);

/*====================================================================== *


======================================================================*/
int CSVGDOMAddLiveRef(CSVGDOM* dom, SVGDOMLiveReferenceType type, AEEDOMNodeID nodeID, void* ptr)
{
    SVGDOMLiveReference* ref = (SVGDOMLiveReference*) MALLOC(sizeof(SVGDOMLiveReference));

    if(ref)
    {
        //add to front of list
        ref->type = type;
        ref->nodeID = nodeID;
        ref->ptr = ptr;
        ref->pNext = dom->liveRefs;
        dom->liveRefs = ref;

        return SUCCESS;
    }
    else
    {
        return ENOMEMORY;
    }
}


/*====================================================================== *


======================================================================*/
int CSVGDOMDeleteLiveRefByType(CSVGDOM* dom, SVGDOMLiveReferenceType type, 
                               AEEDOMNodeID nodeID, void* ptr)
{
    int ret = SUCCESS; 
    switch(type)
    {
    case SVGDOM_LIVE_REF_PATH:        
        ret = SVGALDOM_FreeSVGPath(dom->alCtx, nodeID, (SVGPathHandle)ptr);
        break;
    case SVGDOM_LIVE_REF_MATRIX:        
        ret = SVGALDOM_FreeSVGMatrix(dom->alCtx, nodeID, (SVGALMatrix *)ptr);
        break;
    case SVGDOM_LIVE_REF_COLOR:
        ret = SVGALDOM_FreeSVGRGBColor(dom->alCtx, nodeID, (SVGALRGB24 *)ptr);
        break;
    case SVGDOM_LIVE_REF_RECT:
        ret = SVGALDOM_FreeSVGRect(dom->alCtx, nodeID, (SVGALRect*)ptr);
        break;
    default:
        OEMSVG_OUTPUT("OEMSVGDOM: Unknown live reference type found in cleanup");
        ret = EFAILED;
        break;
    }

    return ret; 
    
}

/*====================================================================== *
//checks the live reference list for the given node id and memory ptr
//and removes the node from the list if there's a match
======================================================================*/
int CSVGDOMRemoveLiveReference(CSVGDOM* dom, AEEDOMNodeID idThis, void* ptr)
{
    SVGDOMLiveReference* ref = dom->liveRefs; 
    SVGDOMLiveReference* prevRef = NULL; 

    while(ref)
    {
        if(ref->nodeID == idThis && ref->ptr == ptr)
            break; 

        prevRef = ref; 
        ref = ref->pNext; 
    }

    if(ref)
    {
        if(prevRef)
            prevRef->pNext = ref->pNext; 
        else
        {
            //removing the head
            dom->liveRefs = ref->pNext;
        }           
        
        FREE(ref); 
        
        return SUCCESS; 
         
    }
    
    return EFAILED; 
}


/*======================================================================


======================================================================*/
int CSVGDOMDestroyLiveReferences(CSVGDOM* dom)
{
    int ret;
    int retVal = SUCCESS;
    SVGDOMLiveReference* ref = dom->liveRefs;
    SVGDOMLiveReference* refTemp;

    while(ref != NULL)
    {
        ret = CSVGDOMDeleteLiveRefByType(dom, ref->type, ref->nodeID, ref->ptr); 

        if(ret != SUCCESS)
        {
            retVal = EFAILED;
            OEMSVG_OUTPUT("OEMSVGDOM: Warning, couldn't free live reference");
        }

        refTemp = ref;
        ref = ref->pNext;

        FREE(refTemp);
    }

    dom->liveRefs = NULL;

    return retVal;
}



/*======================================================================


======================================================================*/
int CSVGDOMInit(CMediaSVG* svg, CSVGDOM** domOut)
{
    CSVGDOM* dom;
    AEEVTBL(ISVGDOM) *    domFuncs;

    *domOut = NULL;

    dom = (CSVGDOM*) MALLOC(sizeof(CSVGDOM)+sizeof(AEEVTBL(ISVGDOM)));
    if(dom == NULL)
        return ENOMEMORY;

    MEMSET(dom, 0, sizeof(CSVGDOM)+sizeof(AEEVTBL(ISVGDOM)));

    if ( SVGALDOMInit(svg->alCtx) != SVGAL_SUCCESS)
    {
        OEMSVG_OUTPUT("OEMSVGDOM Error initializing DOM object");
        return EFAILED;
    }

    domFuncs = (AEEVTBL(ISVGDOM) *)((byte *)dom + sizeof(CSVGDOM));

    domFuncs->AddRef =                      OEMCSVGDOMAddRef;
    domFuncs->Release =                     OEMCSVGDOMRelease;
    domFuncs->CreateElementNS =             OEMCSVGDOMCreateElementNS;
    domFuncs->GetDocumentElement =          OEMCSVGDOMGetDocumentElement;
    domFuncs->GetElementById =              OEMCSVGDOMGetElementById;
    domFuncs->DeleteElement =               OEMCSVGDOMDeleteElement;
    domFuncs->GetNamespaceURI =             OEMCSVGDOMGetNamespaceURI;
    domFuncs->GetLocalName =                OEMCSVGDOMGetLocalName;
    domFuncs->GetParent =                   OEMCSVGDOMGetParent;
    domFuncs->GetOwnerDocument =            OEMCSVGDOMGetOwnerDocument;
    domFuncs->GetContent =                  OEMCSVGDOMGetContent;
    domFuncs->SetContent =                  OEMCSVGDOMSetContent;
    domFuncs->AppendChild =                 OEMCSVGDOMAppendChild;
    domFuncs->RemoveChild =                 OEMCSVGDOMRemoveChild;
    domFuncs->InsertBefore =                OEMCSVGDOMInsertBefore;
    domFuncs->GetNextSibling =              OEMCSVGDOMGetNextSibling;
    domFuncs->GetPrevSibling =              OEMCSVGDOMGetPrevSibling;
    domFuncs->GetFirstChild =               OEMCSVGDOMGetFirstChild;
    domFuncs->GetLastChild =                OEMCSVGDOMGetLastChild;    
    domFuncs->GetAttr =                     OEMCSVGDOMGetAttr;
    domFuncs->SetAttr =                     OEMCSVGDOMSetAttr;
    domFuncs->BeginElement =                OEMCSVGDOMBeginElement;
    domFuncs->BeginElementAt =              OEMCSVGDOMBeginElementAt;
    domFuncs->EndElement =                  OEMCSVGDOMEndElement;
    domFuncs->EndElementAt =                OEMCSVGDOMEndElementAt;
    domFuncs->PauseElement =                OEMCSVGDOMPauseElement;
    domFuncs->ResumeElement =               OEMCSVGDOMResumeElement;
    domFuncs->IsPaused =                    OEMCSVGDOMIsPaused;
    domFuncs->CreateSVGMatrix =             OEMCSVGDOMCreateSVGMatrix;
    domFuncs->FreeSVGMatrix =               OEMCSVGDOMFreeSVGMatrix;
    domFuncs->CreateSVGRect =               OEMCSVGDOMCreateSVGRect;
    domFuncs->FreeSVGRect =                 OEMCSVGDOMFreeSVGRect;
    domFuncs->CreateSVGPath =               OEMCSVGDOMCreateSVGPath;
    domFuncs->FreeSVGPath =                 OEMCSVGDOMFreeSVGPath;
    domFuncs->CreateSVGRGBColor =           OEMCSVGDOMCreateSVGRGBColor;
    domFuncs->FreeSVGRGBColor =             OEMCSVGDOMFreeSVGRGBColor;
    domFuncs->SetFocus =                    OEMCSVGDOMSetFocus;
    domFuncs->MoveFocus =                   OEMCSVGDOMMoveFocus;
    domFuncs->FocusOn =                     OEMCSVGDOMFocusOn;
    domFuncs->GetCurrentFocusedObject =     OEMCSVGDOMGetCurrentFocusedObject;

    domFuncs->AddEventListener =            OEMCSVGDOMAddEventListener;           //addEventListener
    domFuncs->RemoveEventListener =         OEMCSVGDOMRemoveEventListener;        //removeEventListener

    domFuncs->StopPropagation =             OEMCSVGDOMStopPropagation;
    domFuncs->PreventDefault =              OEMCSVGDOMPreventDefault;
    domFuncs->GoToLocation =                OEMCSVGDOMGoToLocation;
    domFuncs->GetURL =                      OEMCSVGDOMGetURL;
    domFuncs->PostURL =                     OEMCSVGDOMPostURL;

    //set the function table for the DOM interface
    AEEINITVTBL(dom, ISVGDOM, *domFuncs);

    dom->svg = svg;
    IMEDIA_AddRef( (IMedia*) svg);
    dom->m_nRefs = 1;

    //share the AL pointer with mediasvg
    dom->alCtx = svg->alCtx;

    dom->liveRefs = NULL;

    *domOut = dom;

    return SUCCESS;
}



/*======================================================================


======================================================================*/
uint32 OEMCSVGDOMAddRef(ISVGDOM* iDom)
{
    CSVGDOM* dom = (CSVGDOM*) iDom;
    return ++dom->m_nRefs;

}


/*======================================================================


======================================================================*/
void CSVGDOMFreeDom(CSVGDOM* dom)
{

    if(dom)
    {
        FREE(dom);
    }
}


/*======================================================================
//note: OEMCSVGDOMRelease only gets called by the app. When MediaSVG wants to delete
//the dom object it calls CSVGDOMFreeDom
======================================================================*/
uint32 OEMCSVGDOMRelease(ISVGDOM* iDom)
{
    CSVGDOM* dom = (CSVGDOM*) iDom;
    dom->m_nRefs--;

    if(dom->m_nRefs != 0)
    {
        //we don't cal IMEDIASVG_Release because there's more DOM references in use
        //which use the IMEDIASVG object
        return dom->m_nRefs;
    }
    else
    {
        //Remove All Live References
        CSVGDOMDestroyLiveReferences(dom);
        CSVGDOMDestroyLiveElements(dom);
        CSVGDOMDestroyLiveEventContexts(dom);
        //no more references for this DOM object, let's remove it.

        if(dom->svg)
        {
            //notiy mediasvg object that DOM object is destroyed by setting the DOM pointer
            //in the MEDIASVG object to NULL
            dom->svg->m_dom = NULL;

            //when this dom object was created, we incremented the mediasvg ref count, release now
            //note: this release should *never* destroy the MEDIASVG object, as MEDIASVG
            //always maintins one reference until it needs to be destroyed
            IMEDIA_Release( (IMedia*) dom->svg);
            dom->svg = NULL;
        }

        //remove memory used by this dom object
        CSVGDOMFreeDom(dom);
    }

    return 0;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMQueryInterface(ISVGDOM* iDom, AEECLSID clsId, void** outInterface)
{
    return ECLASSNOTSUPPORT;
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMCreateElementNS(ISVGDOM *po, const AEEDOMString *cpwNamespaceURI, const AEEDOMString *cpwQualifiedName,
                              AEEDOMNodeID *pidElement)
{
    SVGAL ret;
    SVGALDOMString wStrNamespace;
    SVGALDOMString wQualifiedName;
    CSVGDOM* dom = (CSVGDOM*) po;

    if(!po || !cpwNamespaceURI || !cpwQualifiedName || !pidElement)
    {
        return EBADPARM;
    }

    *pidElement = 0;

    wStrNamespace.str = cpwNamespaceURI->pwBuf;
    wStrNamespace.len = cpwNamespaceURI->cchBuf;

    wQualifiedName.str = cpwQualifiedName->pwBuf;
    wQualifiedName.len = cpwQualifiedName->cchBuf;

    //call into the AL DOM layer
    ret = SVGALDOM_Document_CreateElementNS(dom->alCtx, &wStrNamespace, &wQualifiedName, (SVGALDomObj*)pidElement);
    if(SVGAL_SUCCESS != ret)
    {
        return CMediaSVGALErrorToApp(ret);
    }

    //Add this live Element & track it for deletion    
    ret = CSVGDOMAddLiveElement(dom, *pidElement);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Delete the Live Element, as it cannot be tracked.
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMGetDocumentElement(ISVGDOM *po, AEEDOMNodeID *pidRoot)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM*) po;

    if(!po || !pidRoot)
        return EBADPARM;

    *pidRoot = 0;

    ret = SVGALDOM_Document_GetDocumentElement(dom->alCtx, (SVGALDomObj*)pidRoot);
    if(SVGAL_SUCCESS != ret)
    {
        return CMediaSVGALErrorToApp(ret);
    }

    //Add this live Element & track it for deletion    
    ret = CSVGDOMAddLiveElement(dom, *pidRoot);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Delete the Live Element, as it cannot be tracked.
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMGetElementById(ISVGDOM *po, const AEEDOMString *cpwId, AEEDOMNodeID *pidElement)
{
    SVGAL ret;
    SVGALDOMString idStr;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !cpwId || !pidElement)
    {
        return EBADPARM;
    }

    *pidElement = 0;

    idStr.str = cpwId->pwBuf;
    idStr.len = cpwId->cchBuf;

    ret = SVGALDOM_Document_GetElementById(dom->alCtx, &idStr, (SVGALDomObj*)pidElement);
    if(SVGAL_SUCCESS != ret)
    {
        return CMediaSVGALErrorToApp(ret);
    }

    //Add this live Element & track it for deletion    
    ret = CSVGDOMAddLiveElement(dom, *pidElement);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Delete the Live Element, as it cannot be tracked.
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMDeleteElement(ISVGDOM *po, AEEDOMNodeID pidElement)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    // need to check if htis element have any event listeners
    // FIX ME: Related to IModel

    if(!po || !pidElement)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_Document_DeleteElement(dom->alCtx, pidElement);

    if(SVGAL_SUCCESS == ret)
    {
        //Remove Live Reference
        ret = CSVGDOMRemoveLiveElement(dom, pidElement);
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int CSVGDOMSetAppReturnString(CSVGDOM* dom, AEEDOMString* aeeDOMStr, SVGALDOMString alDOMStr, int offset)
{
    if(aeeDOMStr == NULL || offset < 0)
    {
        return 0;
    }

    if(aeeDOMStr->pwBuf != NULL)
    {
        //caller passed in an allocated string, wants the string returned
        int wcharsToCopy;

        if(alDOMStr.str == NULL)
            return 0;

        //find number of characters to copy
        if(offset < alDOMStr.len)
        {
            if( (alDOMStr.len - offset) > aeeDOMStr->cchBuf)
                wcharsToCopy = aeeDOMStr->cchBuf;
            else
                wcharsToCopy = alDOMStr.len - offset;

            MEMCPY(aeeDOMStr->pwBuf, &alDOMStr.str[offset], wcharsToCopy*sizeof(AECHAR));
            return wcharsToCopy;
        }
    }
    else
    {
        if(alDOMStr.str == NULL)
        {
            aeeDOMStr->cchBuf = 0;
            return 0;
        }

        //caller needs the length of the string only
        aeeDOMStr->cchBuf = alDOMStr.len - offset;
    }

    return 0;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMGetNamespaceURI(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *cpwNamespaceURI)
{
    SVGAL ret;
    SVGALDOMString namespaceURI;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !cpwNamespaceURI)
    {
        return EBADPARM;
    }

    if( (ret = SVGALDOM_Node_GetNamespaceURI(dom->alCtx, (SVGALDomObj) idThis, &namespaceURI)) != SVGAL_SUCCESS)
    {
        return ret;
    }

    //at this point we have the requested string. Get what the caller requested
    CSVGDOMSetAppReturnString(dom, cpwNamespaceURI, namespaceURI, 0);

    //IMP! free the AL layer DOM string
    SVGALFreeMem(namespaceURI.str);

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMGetLocalName(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *cpwLocalName)
{
    SVGALDOMString localname;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !cpwLocalName)
    {
        return EBADPARM;
    }

    if(SVGALDOM_Node_GetLocalName(dom->alCtx, (SVGALDomObj) idThis, &localname) != SVGAL_SUCCESS)
    {
        return EFAILED;
    }

    //at this point we have the requested string. Get what the caller requested
    CSVGDOMSetAppReturnString(dom, cpwLocalName, localname, 0);

    
    SVGALFreeMem(localname.str);

    return SUCCESS;

}

/*======================================================================


======================================================================*/
int OEMCSVGDOMGetParent(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidParent)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pidParent)
    {
        return EBADPARM;
    }

    *pidParent = 0;

    ret = SVGALDOM_Node_GetParent(dom->alCtx, idThis, (SVGALDomObj*)pidParent);
    if(SVGAL_SUCCESS != ret)
    {
        return CMediaSVGALErrorToApp(ret);
    }

    //Add this live Element & track it for deletion    
    ret = CSVGDOMAddLiveElement(dom, *pidParent);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Delete the Live Element, as it cannot be tracked.
    }

    return CMediaSVGALErrorToApp(ret);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMGetOwnerDocument(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidOwnerDocument)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pidOwnerDocument)
    {
        return EBADPARM;
    }

    *pidOwnerDocument = 0;

    ret = SVGALDOM_Node_GetOwnerDocument(dom->alCtx, idThis, (SVGALDomObj*)pidOwnerDocument);
    return CMediaSVGALErrorToApp(ret);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMGetContent(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *pwBuf, int nchIndex, int *pnUsed)
{
    SVGALDOMString content;
    int charsCopied = 0;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pwBuf || !pnUsed)
    {
        return EBADPARM;
    }

    *pnUsed = 0;

    if(SVGALDOM_Node_GetContent(dom->alCtx, (SVGALDomObj) idThis, &content) != SVGAL_SUCCESS)
    {
        return EFAILED;
    }

    //at this point we have the requested string. Get what the caller requested
    charsCopied = CSVGDOMSetAppReturnString(dom, pwBuf, content, nchIndex);
    *pnUsed = charsCopied;

    SVGALFreeMem(content.str);

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMSetContent(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMString *pwBuf, int nchIndex, int *pnSet)
{
    CSVGDOM* dom = (CSVGDOM *) po;
    SVGALDOMString DOMStr;

    if(!po || !idThis || !pwBuf || !pnSet)
    {
        return EBADPARM;
    }

    *pnSet = 0;

    DOMStr.str = pwBuf->pwBuf;
    DOMStr.len = pwBuf->cchBuf;

	if(SVGALDOM_Node_SetContent(dom->alCtx, (SVGALDomObj) idThis, &DOMStr) != SVGAL_SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMAppendChild(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !idNewChild)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_Node_AppendChild(dom->alCtx, (SVGALDomObj)idThis, (SVGALDomObj)idNewChild);

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMRemoveChild(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID idOldChild)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !idOldChild)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_Node_RemoveChild(dom->alCtx, (SVGALDomObj)idThis, (SVGALDomObj)idOldChild);

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMInsertBefore(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID idNewChild, AEEDOMNodeID idRefChild)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !idNewChild)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_Node_InsertBefore(dom->alCtx, (SVGALDomObj)idThis, (SVGALDomObj)idNewChild, (SVGALDomObj)idRefChild);

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMGetNextSibling(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidSibling)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pidSibling)
    {
        return EBADPARM;
    }

    *pidSibling = 0;

    ret = SVGALDOM_ElementTraversal_GetNextElementSibling(dom->alCtx, (SVGALDomObj)idThis, (SVGALDomObj*)pidSibling);
    if(SVGAL_SUCCESS != ret)
    {
        return CMediaSVGALErrorToApp(ret);
    }

    //Add this live Element & track it for deletion    
    ret = CSVGDOMAddLiveElement(dom, *pidSibling);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Delete the Live Element, as it cannot be tracked.
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMGetPrevSibling(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidSibling)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pidSibling)
    {
        return EBADPARM;
    }

    *pidSibling = 0;

    ret = SVGALDOM_ElementTraversal_GetPreviousElementSibling(dom->alCtx, (SVGALDomObj)idThis, (SVGALDomObj*)pidSibling);
    if(SVGAL_SUCCESS != ret)
    {
        return CMediaSVGALErrorToApp(ret);
    }

    //Add this live Element & track it for deletion    
    ret = CSVGDOMAddLiveElement(dom, *pidSibling);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Delete the Live Element, as it cannot be tracked.
    }

    return CMediaSVGALErrorToApp(ret);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMGetFirstChild(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidChild)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pidChild)
    {
        return EBADPARM;
    }

    *pidChild = 0;

    ret = SVGALDOM_ElementTraversal_GetFirstElementChild(dom->alCtx, (SVGALDomObj)idThis, (SVGALDomObj*)pidChild);
    if(SVGAL_SUCCESS != ret)
    {
        return CMediaSVGALErrorToApp(ret);
    }

    //Add this live Element & track it for deletion    
    ret = CSVGDOMAddLiveElement(dom, *pidChild);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Delete the Live Element, as it cannot be tracked.
    }

    return CMediaSVGALErrorToApp(ret);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMGetLastChild(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidChild)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pidChild)
    {
        return EBADPARM;
    }

    *pidChild = 0;

    ret = SVGALDOM_ElementTraversal_GetLastElementChild(dom->alCtx, (SVGALDomObj)idThis, (SVGALDomObj*)pidChild);
    if(SVGAL_SUCCESS != ret)
    {
        return CMediaSVGALErrorToApp(ret);
    }

    //Add this live Element & track it for deletion    
    ret = CSVGDOMAddLiveElement(dom, *pidChild);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Delete the Live Element, as it cannot be tracked.
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
uint32 OEMCSVGDOMModelAddRef(IModel* pm)
{
    CSVGDOMModel* model = (CSVGDOMModel*) pm;
    return ++model->m_nRefs;
}


/*======================================================================


======================================================================*/
uint32 OEMCSVGDOMModelRelease(IModel* pm)
{
    return 0;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMModelQueryInterface(IModel* pm, AEECLSID clsId, void** outInterface)
{
    return ECLASSNOTSUPPORT;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMModelAddListener(IModel* pm, ModelListener* li)
{
    return SUCCESS;
}


/*======================================================================


======================================================================*/
void OEMCSVGDOMModelNotify(IModel *p, ModelEvent *pe)
{

}


/*======================================================================


======================================================================*/
int OEMCSVGDOMGetModel(ISVGDOM *po, AEEDOMNodeID idThis, IModel **ppModel)
{
    CSVGDOMModel*       newModel;
    AEEVTBL(IModel)*    modelFuncs;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!dom || !ppModel)
        return EBADPARM;

    newModel = (CSVGDOMModel*) MALLOC(sizeof(CSVGDOMModel)+sizeof(AEEVTBL(IModel)));
    if(newModel == NULL)
        return ENOMEMORY;

    MEMSET(newModel, 0, sizeof(newModel)+sizeof(AEEVTBL(IModel)));


    modelFuncs = (AEEVTBL(IModel) *)((byte *)newModel + sizeof(CSVGDOMModel));

    modelFuncs->AddRef =              OEMCSVGDOMModelAddRef;
    modelFuncs->Release =              OEMCSVGDOMModelRelease;
    modelFuncs->QueryInterface =      OEMCSVGDOMModelQueryInterface;
    modelFuncs->AddListener =         OEMCSVGDOMModelAddListener;
    modelFuncs->Notify =              OEMCSVGDOMModelNotify;


    //set the function table for the IModel interface
    AEEINITVTBL(newModel, IModel, *modelFuncs);

    newModel->m_nRefs = 1;

    newModel->pOwner = dom;
    ISVGDOM_AddRef( (ISVGDOM*) dom );
    newModel->next = dom->modelList; //insert the new model into the list
    dom->modelList = newModel;

    *ppModel = (IModel*) newModel;

    return SUCCESS;

}


/*======================================================================


======================================================================*/
int OEMCSVGDOMGetAttr(ISVGDOM *po, uint32 idThis, int32 nAttrID, void *p1,void *p2, void *p3)
{
    SVGALDOMString alDomString;
    SVGAL ret = SVGAL_SUCCESS;
    CSVGDOM* dom = (CSVGDOM *) po;

    SVGALDOMString wStrName = {0};
    SVGALDOMString wStrNameB;
    SVGALDOMString retStr;
    SVGALDomObj    retObj;
    SVGALFixed     retFixed;
    SVGALMatrix*   retMatrix;
    SVGALRect*     retRect;
    SVGALRGB24*    retColor;
    SVGALPoint*    retPoint;    

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    // common code for TraitAccess
    switch(nAttrID)
    {

    case SVGDOM_ATTR_TRAIT:
    case SVGDOM_ATTR_TRAIT_FLOAT:
    case SVGDOM_ATTR_TRAIT_PRES:
    case SVGDOM_ATTR_TRAIT_PRES_FLOAT:

        if (!p1)
        {
            return EUNSUPPORTED;
        }
        wStrName.str = ((AEEDOMString*)p1)->pwBuf;
        wStrName.len = ((AEEDOMString*)p1)->cchBuf;

        if(!p2)
        {
            return EBADPARM;
        }
        break;

    case SVGDOM_ATTR_ELEMENT_ID:
    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_ROTATE:
    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TRANSLATE:
    case SVGDOM_ATTR_SVGSVGELEMENT_VIEWPORT:
    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TIME:
    case SVGDOM_ATTR_MATRIX_OP_MULTIPLY:
    case SVGDOM_ATTR_PATH_NUMBER_OF_SEGMENTS:

        if(!p1)
        {
            return EBADPARM;
        }
        break;

    case SVGDOM_ATTR_TRAIT_RECT:
    case SVGDOM_ATTR_TRAIT_MATRIX:
    case SVGDOM_ATTR_TRAIT_PATH:
    case SVGDOM_ATTR_TRAIT_RGBCOLOR:
    case SVGDOM_ATTR_TRAIT_PRES_MATRIX:
    case SVGDOM_ATTR_TRAIT_PRES_RECT:
    case SVGDOM_ATTR_TRAIT_PRES_PATH:
    case SVGDOM_ATTR_TRAIT_PRES_RGBCOLOR:
        if((!p1) || (!p2) || (!p3))
        {
            return EBADPARM;
        }

        wStrName.str = ((AEEDOMString*)p1)->pwBuf;
        wStrName.len = ((AEEDOMString*)p1)->cchBuf;

        break;

    case SVGDOM_ATTR_LOCATABLE_OP_GETBBOX:
    case SVGDOM_ATTR_LOCATABLE_OP_GETSCREENCTM:
    case SVGDOM_ATTR_LOCATABLE_OP_GETSCREENBBOX:
        if((!p1) || (!p2))
        {
            return EBADPARM;
        }
        break;

    default:
        break;
    }


    switch(nAttrID)
    {

    case SVGDOM_ATTR_TRAIT: // return String

        if ((ret = SVGALDOM_TraitAccess_getTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retStr)) == SVGAL_SUCCESS)
        {
            CSVGDOMSetAppReturnString(dom, ((AEEDOMString*)p2), retStr, 0);
            
            if(retStr.str)
            {                
                SVGALFreeMem(retStr.str);
            }
        }

        break;

    case SVGDOM_ATTR_TRAIT_NS: // return String

        if (!p2)
        {
            return EUNSUPPORTED;
        }
        if (!p1 || !p3)
        {
            return EBADPARM;
        }
        wStrName.str = ((AEEDOMString*)p1)->pwBuf;
        wStrName.len = ((AEEDOMString*)p1)->cchBuf;

        wStrNameB.str = ((AEEDOMString*)p2)->pwBuf;
        wStrNameB.len = ((AEEDOMString*)p2)->cchBuf;

        if ((ret = SVGALDOM_TraitAccess_getTraitNS(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &wStrNameB, &retStr)) == SVGAL_SUCCESS)
        {
            CSVGDOMSetAppReturnString(dom, ((AEEDOMString*)p3), retStr, 0);
            
            if(retStr.str)
            {                
                SVGALFreeMem(retStr.str);
            }
        }

        break;

    case SVGDOM_ATTR_TRAIT_FLOAT: // return Float

        if ((ret = SVGALDOM_TraitAccess_getFloatTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retFixed)) == SVGAL_SUCCESS)
        {
            *((uint32*)p2) = retFixed;
        }
        break;

    case SVGDOM_ATTR_TRAIT_MATRIX: // return Matrix

        if ((ret = SVGALDOM_TraitAccess_getMatrixTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retMatrix)) == SVGAL_SUCCESS)
        {
          if(retMatrix)
          {
            ((SVGMatrix *)p2)->a  = retMatrix->a;
            ((SVGMatrix *)p2)->b  = retMatrix->b;
            ((SVGMatrix *)p2)->c  = retMatrix->c;
            ((SVGMatrix *)p2)->d  = retMatrix->d;
            ((SVGMatrix *)p2)->e  = retMatrix->e;
            ((SVGMatrix *)p2)->f  = retMatrix->f;
            
            SVGALDOM_FreeSVGMatrix(dom->alCtx, (SVGALDomObj)idThis, retMatrix);
            *(boolean *)p3 = FALSE;
          }else
          {
            *(boolean *)p3 = TRUE;
          }
        }
        break;

    case SVGDOM_ATTR_TRAIT_RECT: // return Rect

        if ((ret = SVGALDOM_TraitAccess_getRectTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retRect)) == SVGAL_SUCCESS)
        {
            if(retRect)
            {
                ((SVGRect *)p2)->x  = retRect->x;
                ((SVGRect *)p2)->y  = retRect->y;
                ((SVGRect *)p2)->dx = retRect->dx;
                ((SVGRect *)p2)->dy = retRect->dy;
                
                SVGALDOM_FreeSVGRect(dom->alCtx, (SVGALDomObj)idThis, retRect);

                *(boolean *)p3 = FALSE;
            }else
            {
                *(boolean *)p3 = TRUE;
            }
        }
        break;

    case SVGDOM_ATTR_TRAIT_PATH: // return Path

        if ((ret = SVGALDOM_TraitAccess_getPathTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retObj)) == SVGAL_SUCCESS)
        {
            if(0 != retObj)
            {
                *((SVGPathHandle*) p2) = ((SVGPathHandle) retObj); 

                //keep track of live references for cleanup
                if(CSVGDOMAddLiveRef(dom, SVGDOM_LIVE_REF_PATH, idThis, (void*) retObj) != SUCCESS)
                {                    
                    SVGALDOM_FreeSVGPath(dom->alCtx, (SVGALDomObj)idThis, (SVGPathHandle)retObj);
                    OEMSVG_OUTPUT("OEMSVGDOM Out of memory allocating live ref in OEMCSVGDOMCreateSVGPath");
                    return ENOMEMORY;
                }

                *(boolean *)p3 = FALSE;
            }else
            {
                *(boolean *)p3 = TRUE;
            }
        }

        break;

    case SVGDOM_ATTR_TRAIT_RGBCOLOR: // return RGBColor

        if ((ret = SVGALDOM_TraitAccess_getRGBColorTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retColor)) == SVGAL_SUCCESS)
        {
            if(retColor)
            {
                ((SVGColor *)p2)->red   = retColor->r;
                ((SVGColor *)p2)->green = retColor->g;
                ((SVGColor *)p2)->blue  = retColor->b;
                                
                SVGALDOM_FreeSVGRGBColor(dom->alCtx, (SVGALDomObj)idThis, retColor);

                *(boolean *)p3 = FALSE;

            }else
            {
                *(boolean *)p3 = TRUE;
            }
        }

        break;

    case SVGDOM_ATTR_TRAIT_PRES: // return String

        if ((ret = SVGALDOM_TraitAccess_getPresentationTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retStr)) == SVGAL_SUCCESS)
        {
            CSVGDOMSetAppReturnString(dom, ((AEEDOMString*)p2), retStr, 0);

            if(retStr.str)
            {
                SVGALFreeMem(retStr.str);
            }
        }

        break;

    case SVGDOM_ATTR_TRAIT_PRES_NS: // return String

        if (!p2)
        {
            return EUNSUPPORTED;
        }
        if (!p1 || !p3)
        {
            return EBADPARM;
        }
        wStrName.str = ((AEEDOMString*)p1)->pwBuf;
        wStrName.len = ((AEEDOMString*)p1)->cchBuf;

        wStrNameB.str = ((AEEDOMString*)p2)->pwBuf;
        wStrNameB.len = ((AEEDOMString*)p2)->cchBuf;

        if ((ret = SVGALDOM_TraitAccess_getPresentationTraitNS(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &wStrNameB, &retStr)) == SVGAL_SUCCESS)
        {
            CSVGDOMSetAppReturnString(dom, ((AEEDOMString*)p3), retStr, 0);

            if(retStr.str)
            {                
                SVGALFreeMem(retStr.str);
            }
        }

        break;

    case SVGDOM_ATTR_TRAIT_PRES_FLOAT: // return Float

        if ((ret = SVGALDOM_TraitAccess_getFloatPresentationTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retFixed)) == SVGAL_SUCCESS)
        {
            *((uint32*)p2) = retFixed;
        }
        break;

    case SVGDOM_ATTR_TRAIT_PRES_MATRIX: // return Matrix  

        if ((ret = SVGALDOM_TraitAccess_getMatrixPresentationTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retMatrix)) == SVGAL_SUCCESS)
        {
            if(retMatrix)
            {
                ((SVGMatrix *)p2)->a  = retMatrix->a;
                ((SVGMatrix *)p2)->b  = retMatrix->b;
                ((SVGMatrix *)p2)->c  = retMatrix->c;
                ((SVGMatrix *)p2)->d  = retMatrix->d;
                ((SVGMatrix *)p2)->e  = retMatrix->e;
                ((SVGMatrix *)p2)->f  = retMatrix->f;
                
                SVGALDOM_FreeSVGMatrix(dom->alCtx, (SVGALDomObj)idThis, retMatrix);
                *(boolean *)p3 = FALSE;

            }else
            {
                *(boolean *)p3 = TRUE;
            }
        }

        break;

    case SVGDOM_ATTR_TRAIT_PRES_RECT: // return Rect

        if ((ret = SVGALDOM_TraitAccess_getRectPresentationTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retRect)) == SVGAL_SUCCESS)
        {
            if(retRect)
            {
                ((SVGRect *)p2)->x  = retRect->x;
                ((SVGRect *)p2)->y  = retRect->y;
                ((SVGRect *)p2)->dx = retRect->dx;
                ((SVGRect *)p2)->dy = retRect->dy;
                
                SVGALDOM_FreeSVGRect(dom->alCtx, (SVGALDomObj)idThis, retRect);

                *(boolean *)p3 = FALSE;

            }else
            {
                *(boolean *)p3 = TRUE;
            }
        }

        break;

    case SVGDOM_ATTR_TRAIT_PRES_PATH: // return Path

        if ((ret = SVGALDOM_TraitAccess_getPathPresentationTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retObj)) == SVGAL_SUCCESS)
        {
            if(0 != retObj)
            {
                *((SVGPathHandle*) p2) = ((SVGPathHandle) retObj); 

                //keep track of live references for cleanup
                if(CSVGDOMAddLiveRef(dom, SVGDOM_LIVE_REF_PATH, idThis, (void*) retObj) != SUCCESS)
                {                    
                    SVGALDOM_FreeSVGPath(dom->alCtx, (SVGALDomObj)idThis, (SVGPathHandle)retObj);

                    OEMSVG_OUTPUT("OEMSVGDOM Out of memory allocating live ref in OEMCSVGDOMCreateSVGPath");
                    return ENOMEMORY;
                }

                *(boolean *)p3 = FALSE;
            }else
            {
                *(boolean *)p3 = TRUE;
            }
        }
        break;

    case SVGDOM_ATTR_TRAIT_PRES_RGBCOLOR:  // return RGBColor

        if ((ret = SVGALDOM_TraitAccess_getRGBColorPresentationTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &retColor)) == SVGAL_SUCCESS)
        {
            if(retColor)
            {
                ((SVGColor *)p2)->red   = retColor->r;
                ((SVGColor *)p2)->green = retColor->g;
                ((SVGColor *)p2)->blue  = retColor->b;
                
                SVGALDOM_FreeSVGRGBColor(dom->alCtx, (SVGALDomObj)idThis, retColor);

                *(boolean *)p3 = FALSE;

            }else
            {
                *(boolean *)p3 = TRUE;
            }
        }
        break;

    case SVGDOM_ATTR_ELEMENT_ID:

        if( (ret = SVGALDOM_SVGElement_GetId(dom->alCtx, (SVGALDomObj)idThis, &alDomString)) == SVGAL_SUCCESS)
        {
            CSVGDOMSetAppReturnString(dom, p1, alDomString, 0);

            if(alDomString.str)
            {                
                SVGALFreeMem(alDomString.str);
            }
        }

        break;

    case SVGDOM_ATTR_LOCATABLE_OP_GETBBOX:

        if((ret = SVGALDOM_SVGLocatable_getBBox(dom->alCtx, (SVGALDomObj)idThis, &retRect)) == SVGAL_SUCCESS)
        {
            //No need to keep track of live references for cleanup
            if(retRect)
            {
                ((SVGRect *)p1)->x  = retRect->x;
                ((SVGRect *)p1)->y  = retRect->y;
                ((SVGRect *)p1)->dx = retRect->dx;
                ((SVGRect *)p1)->dy = retRect->dy;
                
                SVGALDOM_FreeSVGRect(dom->alCtx, (SVGALDomObj)idThis, retRect);
                *(boolean *)p2 = FALSE;

            }else
            {
                *(boolean *)p2 = TRUE;
            }
        }
        break;

    case SVGDOM_ATTR_LOCATABLE_OP_GETSCREENCTM:

        if((ret = SVGALDOM_SVGLocatable_getScreenCTM(dom->alCtx, (SVGALDomObj)idThis, &retMatrix)) == SVGAL_SUCCESS)
        {
            if(retMatrix)
            {
                ((SVGMatrix *)p1)->a  = retMatrix->a;
                ((SVGMatrix *)p1)->b  = retMatrix->b;
                ((SVGMatrix *)p1)->c  = retMatrix->c;
                ((SVGMatrix *)p1)->d  = retMatrix->d;
                ((SVGMatrix *)p1)->e  = retMatrix->e;
                ((SVGMatrix *)p1)->f  = retMatrix->f;
                
                SVGALDOM_FreeSVGMatrix(dom->alCtx, (SVGALDomObj)idThis, retMatrix);

                *(boolean *)p2 = FALSE;

            }else
            {
                *(boolean *)p2 = TRUE;
            }
        }
        break;

    case SVGDOM_ATTR_LOCATABLE_OP_GETSCREENBBOX:

        if((ret = SVGALDOM_SVGLocatable_getScreenBBox(dom->alCtx, (SVGALDomObj)idThis, &retRect)) == SVGAL_SUCCESS)
        {
            if(retRect)
            {
                ((SVGRect *)p1)->x  = retRect->x;
                ((SVGRect *)p1)->y  = retRect->y;
                ((SVGRect *)p1)->dx = retRect->dx;
                ((SVGRect *)p1)->dy = retRect->dy;
                
                SVGALDOM_FreeSVGRect(dom->alCtx, (SVGALDomObj)idThis, retRect);

                *(boolean *)p2 = FALSE;

            }else
            {
                *(boolean *)p2 = TRUE;
            }
        }
        break;

    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_SCALE:
        if((ret = SVGALDOM_SVGSVGElement_GetCurrentScale(dom->alCtx, (SVGALDomObj)idThis, &retFixed)) == SVGAL_SUCCESS)
        {
            *((uint32*)p1) = retFixed;
        }
        break;

    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_ROTATE:
        if((ret = SVGALDOM_SVGSVGElement_GetCurrentRotate(dom->alCtx, (SVGALDomObj)idThis, &retFixed)) == SVGAL_SUCCESS)
        {
            *((uint32*)p1) = retFixed;
        }
        break;

    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TRANSLATE:
        if((ret = SVGALDOM_SVGSVGElement_GetCurrentTranslate(dom->alCtx, (SVGALDomObj)idThis, &retPoint)) == SVGAL_SUCCESS)
        {            
            if(retPoint)
            {
                COPY_POINT((*((SVGPoint*)p1)), retPoint);
                SVGALDOM_FreeSVGPoint(dom->alCtx, (SVGALDomObj) idThis, retPoint); 
            }

        }
        break;

    case SVGDOM_ATTR_SVGSVGELEMENT_VIEWPORT:
        if((ret = SVGALDOM_SVGSVGElement_GetViewport(dom->alCtx, (SVGALDomObj)idThis, &retRect)) == SVGAL_SUCCESS)
        {
            if(retRect)
            {
                COPY_RECT((*((SVGRect*)p1)), retRect);
                
                SVGALDOM_FreeSVGRect(dom->alCtx, (SVGALDomObj)idThis, retRect);
            }
        }

        break;

    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TIME:
        if((ret = SVGALDOM_SVGSVGElement_GetCurrentTime(dom->alCtx, (SVGALDomObj)idThis, &retFixed)) == SVGAL_SUCCESS)
        {
            *((uint32*)p1) = retFixed;
        }

        break;

    case SVGDOM_ATTR_MATRIX_OP_GETCOMPONENT:
    case SVGDOM_ATTR_MATRIX_OP_MULTIPLY:
    case SVGDOM_ATTR_MATRIX_OP_INVERSE:
    case SVGDOM_ATTR_MATRIX_OP_TRANSLATE:
    case SVGDOM_ATTR_MATRIX_OP_SCALE:
    case SVGDOM_ATTR_MATRIX_OP_ROTATE:
        ret = CSVGDOMHandleMatrix(po, idThis, nAttrID, p1, p2);
      break;

    case SVGDOM_ATTR_PATH_NUMBER_OF_SEGMENTS:
        //Use break; if you remove return
        return (SVGALDOM_SVGPath_GetNumberOfSegments(dom->alCtx, (SVGALDomObj)idThis, (unsigned int *) p1));

    case SVGDOM_ATTR_PATH_OP_GETSEGMENT:
        if (!p2)
        {
            return EBADPARM;
        }
        if ((ret = SVGALDOM_SVGPath_GetSegment(dom->alCtx, (SVGALDomObj)idThis, (unsigned int)p1, &retObj)) == SVGAL_SUCCESS)
        {
            *((uint32*)p2) = retObj;
        }
        break;

    case SVGDOM_ATTR_PATH_OP_GETSEGMENTPARAM:
        if (!p3)
        {
            return EBADPARM;
        }
        if ((ret = SVGALDOM_SVGPath_GetSegmentParam(dom->alCtx, (SVGALDomObj)idThis, (unsigned int)p1, (unsigned int)p2, &retFixed)) == SVGAL_SUCCESS)
        {
            *((uint32*)p3) = retFixed;
        }
        break;

    case SVGDOM_ATTR_ATTRIBUTE_NS:
        if (!p2)
        {
            return EUNSUPPORTED;
        }
        
        if (!p1 || !p3)
        {
            return EBADPARM;
        }
        
        wStrName.str = ((AEEDOMString*)p1)->pwBuf;
        wStrName.len = ((AEEDOMString*)p1)->cchBuf;

        wStrNameB.str = ((AEEDOMString*)p2)->pwBuf;
        wStrNameB.len = ((AEEDOMString*)p2)->cchBuf;

        if ((ret = SVGALDOM_Element_GetAttributeNS(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &wStrNameB, &retStr)) == SVGAL_SUCCESS)
        {
            CSVGDOMSetAppReturnString(dom, ((AEEDOMString*)p3), retStr, 0);
            
            if(retStr.str)
            {                
                SVGALFreeMem(retStr.str);
            }
        }

        break;

    default:
        return EUNSUPPORTED;

    }

    return CMediaSVGALErrorToApp(ret);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMSetAttr(ISVGDOM *po, uint32 idThis, int32 nAttrID, void *p1, void *p2, void *p3)
{

    SVGPoint* point1;
    SVGPoint* point2;
    SVGPoint* point3;
    SVGALDOMString wStrName = {0};
    SVGALDOMString wStrNamespaceURI;
    SVGALDOMString wStrValue;
    SVGALRect rectValue;
    SVGALRGB24 colorValue;
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis)
    {
        //MSG_ERROR ( "OEMCSVGDOMSetAttr: NULL idThis", 0, 0, 0 );
        return EBADPARM;
    }

    switch(nAttrID)
    {

    case SVGDOM_ATTR_TRAIT_FLOAT:

        if (!p1)
        {
            return EUNSUPPORTED;
        }
        wStrName.str = ((AEEDOMString*)p1)->pwBuf;
        wStrName.len = ((AEEDOMString*)p1)->cchBuf;

        break;

    case SVGDOM_ATTR_TRAIT:
    case SVGDOM_ATTR_TRAIT_MATRIX:
    case SVGDOM_ATTR_TRAIT_RECT:
    case SVGDOM_ATTR_TRAIT_PATH:
    case SVGDOM_ATTR_TRAIT_RGBCOLOR:

        if (!p1)
        {
            return EUNSUPPORTED;
        }

        if (!p2)
        {
            return SVGDOM_ERROR_INVALID_ACCESS;
        }

        wStrName.str = ((AEEDOMString*)p1)->pwBuf;
        wStrName.len = ((AEEDOMString*)p1)->cchBuf;
        break;

    case SVGDOM_ATTR_PATH_OP_MOVETO:
    case SVGDOM_ATTR_PATH_OP_LINETO:

        if (!p1)
        {
            return SVGDOM_ERROR_INVALID_ACCESS;
        }
        break;

    case SVGDOM_ATTR_PATH_OP_QUADTO:

        if (!p1 || !p2)
        {
            return SVGDOM_ERROR_INVALID_ACCESS;
        }
        break;

    case SVGDOM_ATTR_PATH_OP_CURVETO:

        if (!p1 || !p2 || !p3)
        {
            return SVGDOM_ERROR_INVALID_ACCESS;
        }
        break;


    default:
        //continue
        break;
    }

    switch(nAttrID)
    {

    case SVGDOM_ATTR_TRAIT:

        wStrValue.str = ((AEEDOMString*)p2)->pwBuf;
        wStrValue.len = ((AEEDOMString*)p2)->cchBuf;

        ret = SVGALDOM_TraitAccess_setTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, &wStrValue);
        break;

    case SVGDOM_ATTR_TRAIT_NS:

        if (!p2)
        {
            return EUNSUPPORTED;
        }

        if (!p1 || !p3)
        {
            return SVGDOM_ERROR_INVALID_ACCESS;
        }

        wStrNamespaceURI.str = ((AEEDOMString*)p1)->pwBuf;
        wStrNamespaceURI.len = ((AEEDOMString*)p1)->cchBuf;

        wStrName.str = ((AEEDOMString*)p2)->pwBuf;
        wStrName.len = ((AEEDOMString*)p2)->cchBuf;

        wStrValue.str = ((AEEDOMString*)p3)->pwBuf;
        wStrValue.len = ((AEEDOMString*)p3)->cchBuf;

        ret = SVGALDOM_TraitAccess_setTraitNS(dom->alCtx, (SVGALDomObj)idThis, &wStrNamespaceURI, &wStrName, &wStrValue);
        break;

    case SVGDOM_ATTR_TRAIT_FLOAT:

        ret = SVGALDOM_TraitAccess_setFloatTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, (SVGALFixed)p2);
        break;

    case SVGDOM_ATTR_TRAIT_MATRIX:

        ret = SVGALDOM_TraitAccess_setMatrixTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, (SVGALDomObj)p2);
        break;

    case SVGDOM_ATTR_TRAIT_RECT:

        rectValue.x  = ((SVGRect*)p2)->x;
        rectValue.y  = ((SVGRect*)p2)->y;
        rectValue.dx = ((SVGRect*)p2)->dx;
        rectValue.dy = ((SVGRect*)p2)->dy;

        ret = SVGALDOM_TraitAccess_setRectTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, rectValue);

        break;

    case SVGDOM_ATTR_TRAIT_PATH:

        ret = SVGALDOM_TraitAccess_setPathTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, (SVGALDomObj)p2);
        break;

    case SVGDOM_ATTR_TRAIT_RGBCOLOR:

        colorValue.r = ((SVGColor*)p2)->red; 
        colorValue.g = ((SVGColor*)p2)->green;
        colorValue.b = ((SVGColor*)p2)->blue;  

        ret = SVGALDOM_TraitAccess_setRGBColorTrait(dom->alCtx, (SVGALDomObj)idThis, &wStrName, colorValue);
        
        break;

    case SVGDOM_ATTR_ELEMENT_ID:
        if (!p1)
        {
            return SVGDOM_ERROR_INVALID_ACCESS;
        }

        wStrValue.str = ((AEEDOMString*)p1)->pwBuf;
        wStrValue.len = ((AEEDOMString*)p1)->cchBuf;

        ret = SVGALDOM_SVGElement_SetId(dom->alCtx, (SVGALDomObj)idThis, &wStrValue);

        break;

    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_SCALE:

        if (!p1)
        {
            return SVGDOM_ERROR_INVALID_ACCESS;
        }
        ret = SVGALDOM_SVGSVGElement_SetCurrentScale(dom->alCtx, (SVGALDomObj)idThis, (SVGALFixed)p1);
        break;

    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_ROTATE:
        ret = SVGALDOM_SVGSVGElement_SetCurrentRotate(dom->alCtx, (SVGALDomObj)idThis, (SVGALFixed)p1);
        break;

    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TRANSLATE:
        ret = SVGALDOM_SVGSVGElement_SetCurrentTranslate(dom->alCtx, (SVGALDomObj)idThis, (SVGALPoint*)p1);
        break;

    case SVGDOM_ATTR_SVGSVGELEMENT_VIEWPORT:
        // In W3C uDOM spec, viewport is readonly attribute
        // setViewport() is not supported
        return EUNSUPPORTED;
        //break;

    case SVGDOM_ATTR_SVGSVGELEMENT_CURRENT_TIME:
        ret = SVGALDOM_SVGSVGElement_SetCurrentTime(dom->alCtx, (SVGALDomObj)idThis, (SVGALFixed)p1);
        break;

    case SVGDOM_ATTR_PATH_OP_MOVETO:

        point1 = (SVGPoint*) p1;
        ret = SVGALDOM_SVGPath_MoveTo(dom->alCtx, idThis, point1->cx, point1->cy);
        break;

    case SVGDOM_ATTR_PATH_OP_LINETO:

        point1 = (SVGPoint*) p1;
        ret = SVGALDOM_SVGPath_LineTo(dom->alCtx, idThis, point1->cx, point1->cy);
        break;

    case SVGDOM_ATTR_PATH_OP_QUADTO:

        point1 = (SVGPoint*) p1;
        point2 = (SVGPoint*) p2;
        ret = SVGALDOM_SVGPath_QuadTo(dom->alCtx, idThis, point1->cx, point1->cy, point2->cx, point2->cy);
        break;

    case SVGDOM_ATTR_PATH_OP_CURVETO:

        point1 = (SVGPoint*) p1;
        point2 = (SVGPoint*) p2;
        point3 = (SVGPoint*) p3;
        ret = SVGALDOM_SVGPath_CurveTo(dom->alCtx, idThis, point1->cx, point1->cy, point2->cx, point2->cy, point3->cx, point3->cy);
        break;

    case SVGDOM_ATTR_PATH_OP_CLOSE:
        ret = SVGALDOM_SVGPath_Close(dom->alCtx, idThis);
        break;

    case SVGDOM_ATTR_ATTRIBUTE_NS:
        if (!p2)
        {
            return EUNSUPPORTED;
        }

        if (!p1 || !p3)
        {
            return SVGDOM_ERROR_INVALID_ACCESS;
        }

        wStrNamespaceURI.str = ((AEEDOMString*)p1)->pwBuf;
        wStrNamespaceURI.len = ((AEEDOMString*)p1)->cchBuf;

        wStrName.str = ((AEEDOMString*)p2)->pwBuf;
        wStrName.len = ((AEEDOMString*)p2)->cchBuf;

        wStrValue.str = ((AEEDOMString*)p3)->pwBuf;
        wStrValue.len = ((AEEDOMString*)p3)->cchBuf;

        ret = SVGALDOM_Element_SetAttributeNS(dom->alCtx, (SVGALDomObj)idThis, &wStrNamespaceURI, &wStrName, &wStrValue);
        break;

    default:
        return EBADPARM;

    }

    //INFO: Optimized Frame Redraws                  
    //no more frames pending
    dom->svg->m_isFramePending = FALSE;
    if((SVGAL_SUCCESS == ret) && (dom->svg->m_pMediaSVGState == MEDIASVG_STATE_PLAYING))
    {
        //and we're in playing state, continue the animation
        CMediaSVGNextFrame(dom->svg, TRUE, 0);
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMBeginElement(ISVGDOM *po,  AEEDOMNodeID idThis)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_ElementTimeControl_beginElement(dom->alCtx, (SVGALDomObj)idThis);

    
    //INFO: Optimized Frame Redraws                  
    //no more frames pending
    dom->svg->m_isFramePending = FALSE;
    if((SVGAL_SUCCESS == ret) && (dom->svg->m_pMediaSVGState == MEDIASVG_STATE_PLAYING))
    {
      //and we're in playing state, continue the animation
      CMediaSVGNextFrame(dom->svg, TRUE, 0);
    }
    
    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMBeginElementAt(ISVGDOM *po,  AEEDOMNodeID idThis,uint32 dwFixedOffset)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_ElementTimeControl_beginElementAt(dom->alCtx, (SVGALDomObj)idThis, dwFixedOffset);

    
    //INFO: Optimized Frame Redraws                  
    //no more frames pending
    dom->svg->m_isFramePending = FALSE;
    if((SVGAL_SUCCESS == ret) && (dom->svg->m_pMediaSVGState == MEDIASVG_STATE_PLAYING))
    {
        //and we're in playing state, continue the animation
        CMediaSVGNextFrame(dom->svg, TRUE, 0);
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMEndElement(ISVGDOM *po, AEEDOMNodeID idThis)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_ElementTimeControl_endElement(dom->alCtx, (SVGALDomObj)idThis);

    //INFO: Optimized Frame Redraws                  
    //no more frames pending
    dom->svg->m_isFramePending = FALSE;
    if((SVGAL_SUCCESS == ret) && (dom->svg->m_pMediaSVGState == MEDIASVG_STATE_PLAYING))
    {
        //and we're in playing state, continue the animation
        CMediaSVGNextFrame(dom->svg, TRUE, 0);
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMEndElementAt(ISVGDOM *po, AEEDOMNodeID idThis, uint32 dwFixedOffset)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_ElementTimeControl_endElementAt(dom->alCtx, (SVGALDomObj)idThis, dwFixedOffset);

    //INFO: Optimized Frame Redraws                  
    //no more frames pending
    dom->svg->m_isFramePending = FALSE;
    if((SVGAL_SUCCESS == ret) && (dom->svg->m_pMediaSVGState == MEDIASVG_STATE_PLAYING))
    {
        //and we're in playing state, continue the animation
        CMediaSVGNextFrame(dom->svg, TRUE, 0);
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMPauseElement(ISVGDOM *po, AEEDOMNodeID idThis)
{
    // not support by BF
    // according to BF, W3C will remove this API will the final spec
    return EUNSUPPORTED;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMResumeElement(ISVGDOM *po, AEEDOMNodeID idThis)
{
    // not support by BF
    // according to BF, W3C will remove this API will the final spec
    return EUNSUPPORTED;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMIsPaused(ISVGDOM *po, AEEDOMNodeID idThis, boolean *pbPaused)
{
    // not support by BF
    // according to BF, W3C will remove this API will the final spec
    return EUNSUPPORTED;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMCreateSVGMatrix(ISVGDOM *po, AEEDOMNodeID idThis, SVGMatrix *pIn, SVGMatrix **ppOut)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pIn || !ppOut)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_SVGSVGElement_CreateSVGMatrix(dom->alCtx, (SVGALDomObj) idThis,
                                                 (SVGALMatrix *) pIn, (SVGALMatrix **) ppOut );

    if(ret == SVGAL_SUCCESS)
    {
        //keep track of live references for cleanup
        if(CSVGDOMAddLiveRef(dom, SVGDOM_LIVE_REF_MATRIX, idThis, (void*) *ppOut) != SUCCESS)
        {
            SVGALDOM_FreeSVGMatrix(dom->alCtx, (SVGALDomObj) idThis, (SVGALMatrix *)*ppOut);

            OEMSVG_OUTPUT("OEMSVGDOM Out of memory allocating live ref in OEMCSVGDOMCreateSVGMatrix");
            return ENOMEMORY;
        }
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMFreeSVGMatrix(ISVGDOM *po, AEEDOMNodeID idThis, SVGMatrix *pMatrix)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!pMatrix)
    {
        return SUCCESS;
    }

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_FreeSVGMatrix(dom->alCtx, idThis, (SVGALMatrix *) pMatrix);
    
    //also remove from the live references list, ok to do this after the free
    //since removing from list doesn't touch any memory, just removes a copy
    //of the node address from the list
    if(ret == SVGAL_SUCCESS)
    {
        CSVGDOMRemoveLiveReference(dom, idThis, pMatrix);
    }
        
    
    return CMediaSVGALErrorToApp(ret);
}

int CSVGDOMHandleMatrix(ISVGDOM *po, uint32 idThis, int32 nAttrID, void *p1,void *p2)
{  
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;
    SVGALDomObj retObj;
    SVGALFixed retFixed;

    if((SVGDOM_ATTR_MATRIX_OP_GETCOMPONENT == nAttrID) && (NULL == p2))
    {
        return EBADPARM;
    }

    ret = SVGALDOM_Document_GetDocumentElement(dom->alCtx, &retObj);
    if(SVGAL_SUCCESS != ret)
    {
        return EFAILED;
    }

    switch(nAttrID)
    {
        case SVGDOM_ATTR_MATRIX_OP_GETCOMPONENT:
            ret = SVGALDOM_SVGMatrix_getComponent(dom->alCtx, retObj, (SVGALMatrix *)idThis, (unsigned int)p1, &retFixed);
            if (ret == SVGAL_SUCCESS)
            {
                *((uint32*)p2) = retFixed;
            }
            break;

        case SVGDOM_ATTR_MATRIX_OP_MULTIPLY:
            ret = SVGALDOM_SVGMatrix_multiply(dom->alCtx, retObj, (SVGALMatrix*) idThis, (SVGALMatrix*)p1);
            break;

        case SVGDOM_ATTR_MATRIX_OP_INVERSE:
            ret = SVGALDOM_SVGMatrix_inverse(dom->alCtx, retObj, (SVGALMatrix*) idThis);
            break;

        case SVGDOM_ATTR_MATRIX_OP_TRANSLATE:
            ret = SVGALDOM_SVGMatrix_translate(dom->alCtx, retObj, (SVGALMatrix*) idThis, (SVGALFixed)p1, (SVGALFixed)p2);
            break;

        case SVGDOM_ATTR_MATRIX_OP_SCALE:
            ret = SVGALDOM_SVGMatrix_scale(dom->alCtx, retObj, (SVGALMatrix*) idThis, (SVGALFixed)p1);
            break;

        case SVGDOM_ATTR_MATRIX_OP_ROTATE:
            ret = SVGALDOM_SVGMatrix_rotate(dom->alCtx, retObj, (SVGALMatrix*) idThis, (SVGALFixed)p1);
            break;

        default:
            //This condition would never occur
            ret = SVGAL_BAD_PARAM;
            break;
    }

    if(SVGAL_SUCCESS != ret)
    {
        return EFAILED;
    }else
    {
        return SUCCESS;
    }
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMCreateSVGRect(ISVGDOM *po, AEEDOMNodeID idThis, SVGRect **ppRect)
{
    //not supported on target
//  return EUNSUPPORTED;

    SVGAL     ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !ppRect)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_SVGSVGElement_CreateSVGRect(dom->alCtx, (SVGALDomObj)idThis, (SVGALRect**)ppRect);

    if(ret == SVGAL_SUCCESS)
    {
        //keep track of live references for cleanup
        if(CSVGDOMAddLiveRef(dom, SVGDOM_LIVE_REF_RECT, idThis, (void*) *ppRect) != SUCCESS)
        {
            SVGALDOM_FreeSVGRect(dom->alCtx, (SVGALDomObj)idThis, (SVGALRect*)*ppRect);

            OEMSVG_OUTPUT("OEMSVGDOM Out of memory allocating live ref in OEMCSVGDOMCreateSVGRect");
            return ENOMEMORY;
        }
    }

    return CMediaSVGALErrorToApp(ret);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMFreeSVGRect(ISVGDOM *po, AEEDOMNodeID idThis, SVGRect *pRect)
{
    //not suupported on target
    //return EUNSUPPORTED;
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!pRect)
    {
        return SUCCESS;
    }

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_FreeSVGRect(dom->alCtx, idThis, (SVGALRect*)pRect);

    //also remove from the live references list, ok to do this after the free
    //since removing from list doesn't touch any memory, just removes a copy
    //of the node address from the list
    if(ret == SVGAL_SUCCESS)
    {
        CSVGDOMRemoveLiveReference(dom, idThis, (void *)pRect);
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMCreateSVGPath(ISVGDOM *po, AEEDOMNodeID idThis, SVGPathHandle *phPath)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !phPath)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_SVGSVGElement_CreatePath(dom->alCtx, (SVGALDomObj) idThis, (SVGALDomObj*)phPath);

    if(ret == SVGAL_SUCCESS)
    {
        //keep track of live references for cleanup
        if(CSVGDOMAddLiveRef(dom, SVGDOM_LIVE_REF_PATH, idThis, (void*) *phPath) != SUCCESS)
        {
            SVGALDOM_FreeSVGPath(dom->alCtx, (SVGALDomObj) idThis, (SVGPathHandle)*phPath);

            OEMSVG_OUTPUT("OEMSVGDOM Out of memory allocating live ref in OEMCSVGDOMCreateSVGPath");
            return ENOMEMORY;
        }
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMFreeSVGPath(ISVGDOM *po, AEEDOMNodeID idThis, SVGPathHandle hPath)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!hPath)
    {
        return SUCCESS;
    }

    if(!po || !idThis || !hPath)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_FreeSVGPath(dom->alCtx, idThis, hPath);
    if(ret == SVGAL_SUCCESS)
    {
        CSVGDOMRemoveLiveReference(dom, idThis, (void*) hPath); 
    }
    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMCreateSVGRGBColor(ISVGDOM *po, AEEDOMNodeID idThis, RGBVAL v, SVGColor **pvOut)
{
    SVGAL ret;
    SVGColor cIn;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pvOut)
    {
        return EBADPARM;
    }

    MAKE_SVGColor(v, &cIn);

    ret = SVGALDOM_SVGSVGElement_CreateSVGRGBColor(dom->alCtx, (SVGALDomObj) idThis,
                                                   (SVGALRGB24*) &cIn, (SVGALRGB24**) pvOut);

    if(ret == SVGAL_SUCCESS)
    {
        //keep track of live references for cleanup
        if(CSVGDOMAddLiveRef(dom, SVGDOM_LIVE_REF_COLOR, idThis, (void*) *pvOut) != SUCCESS)
        {
            SVGALDOM_FreeSVGRGBColor(dom->alCtx, (SVGALDomObj) idThis, (SVGALRGB24 *)*pvOut);

            OEMSVG_OUTPUT("OEMSVGDOM Out of memory allocating live ref in OEMCSVGDOMCreateSVGRGBColor");
            return ENOMEMORY;
        }
    }

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMFreeSVGRGBColor(ISVGDOM *po, AEEDOMNodeID idThis, SVGColor *pv)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!pv)
    {
        return SUCCESS;
    }

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_FreeSVGRGBColor(dom->alCtx, idThis, (SVGALRGB24 *) pv);
    if(ret == SUCCESS)
    {
        CSVGDOMRemoveLiveReference(dom, idThis, pv); 
    }
    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMSetFocus(ISVGDOM *po, AEEDOMNodeID idThis)
{
    SVGAL ret;
    AEEDOMNodeID rootElem;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    if(ISVGDOM_GetDocumentElement(po, &rootElem) != SUCCESS)
    {
        OEMSVG_OUTPUT("OEMSVGDOM OEMCSVGDOMSetFocus(), can't get document element");
        return EFAILED;
    }

    ret = SVGALDOM_SVGSVGElement_SetFocus(dom->alCtx, rootElem, idThis);

    
    //INFO: Optimized Frame Redraws                  
    //no more frames pending
    dom->svg->m_isFramePending = FALSE;
    if((SVGAL_SUCCESS == ret) && (dom->svg->m_pMediaSVGState == MEDIASVG_STATE_PLAYING))
    {
      //and we're in playing state, continue the animation
      CMediaSVGNextFrame(dom->svg, TRUE, 0);
    }
    
    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMMoveFocus(ISVGDOM *po, AEEDOMNodeID idThis, int nMotionType)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;
    SVGALFocusDirection dir;

    if(!po || !idThis)
    {
        return EBADPARM;
    }

    switch(nMotionType)
    {
        case SVGDOM_NAV_AUTO:
            return EUNSUPPORTED;

        case SVGDOM_NAV_NEXT:
            dir = SVGAL_FOCUS_EVENT_NEXT;
            break;

        case SVGDOM_NAV_PREV:
            dir = SVGAL_FOCUS_EVENT_PREV;
            break;

        case SVGDOM_NAV_UP:
            dir = SVGAL_FOCUS_EVENT_UP;
            break;

        case SVGDOM_NAV_UP_RIGHT:
            dir = SVGAL_FOCUS_EVENT_UP_RIGHT;
            break;

        case SVGDOM_NAV_RIGHT:
            dir = SVGAL_FOCUS_EVENT_RIGHT;
            break;

        case SVGDOM_NAV_DOWN_RIGHT:
            dir = SVGAL_FOCUS_EVENT_DOWN_RIGHT;
            break;

        case SVGDOM_NAV_DOWN:
            dir = SVGAL_FOCUS_EVENT_DOWN;
            break;

        case SVGDOM_NAV_DOWN_LEFT:
            dir = SVGAL_FOCUS_EVENT_DOWN_LEFT;
            break;

        case SVGDOM_NAV_LEFT:
            dir = SVGAL_FOCUS_EVENT_LEFT;
            break;

        case SVGDOM_NAV_UP_LEFT:
            dir = SVGAL_FOCUS_EVENT_UP_LEFT;
            break;

        default:
            return EBADPARM;
    }

    ret = SVGALDOM_SVGSVGElement_MoveFocus(dom->alCtx, idThis, dir);

    //INFO: Optimized Frame Redraws                  
    //no more frames pending
    dom->svg->m_isFramePending = FALSE;
    if((SVGAL_SUCCESS == ret) && (dom->svg->m_pMediaSVGState == MEDIASVG_STATE_PLAYING))
    {
        //and we're in playing state, continue the animation
        CMediaSVGNextFrame(dom->svg, TRUE, 0);
    }

    return CMediaSVGALErrorToApp(ret); ;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMFocusOn(ISVGDOM *po, AEEDOMNodeID idThis)
{
    SVGAL ret;
    AEEDOMNodeID rootElem;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po)
    {
        return EBADPARM;
    }

    if(ISVGDOM_GetDocumentElement(po, &rootElem) != SUCCESS)
    {
        OEMSVG_OUTPUT("OEMSVGDOM OEMCSVGDOMFocusOn(), can't get document element");
        return EFAILED;
    }

    ret = SVGALDOM_SVGSVGElement_FocusOn(dom->alCtx, rootElem, idThis);

    
    //INFO: Optimized Frame Redraws                  
    //no more frames pending
    dom->svg->m_isFramePending = FALSE;
    if((SVGAL_SUCCESS == ret) && (dom->svg->m_pMediaSVGState == MEDIASVG_STATE_PLAYING))
    {
      //and we're in playing state, continue the animation
      CMediaSVGNextFrame(dom->svg, TRUE, 0);
    }
    
    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMGetCurrentFocusedObject(ISVGDOM *po, AEEDOMNodeID idThis, AEEDOMNodeID *pidElement)
{
    SVGAL ret;
    CSVGDOM* dom = (CSVGDOM *) po;

    if(!po || !idThis || !pidElement)
    {
        return EBADPARM;
    }

    ret = SVGALDOM_SVGSVGElement_GetCurrentFocusedObject(dom->alCtx, idThis, (SVGALDomObj*)pidElement);

    return CMediaSVGALErrorToApp(ret);
}


/*======================================================================


======================================================================*/
SVGAL CSVGDOMAddLiveElement(
    CSVGDOM *       pDom, 
    AEEDOMNodeID    Element)
{
    SVGDOMLiveElement * pNewNode      = NULL;
    SVGDOMLiveElement * pSearchNode   = NULL;
    boolean             isInList      = FALSE;
    SVGDOMLiveElement * pPrevNode  = NULL;

    if(NULL == pDom)
    {
        return (SVGAL_BAD_PARAM);
    }

    if(0 == Element)
    {
        return (SVGAL_SUCCESS);
    }

    //Get the List Head
    pSearchNode = pDom->liveElements;

    //Check whether the Element exists already in the List 
    while(NULL != pSearchNode)
    {
        if(Element == pSearchNode->ElementID)
        {
            isInList = TRUE;
            break;
        }else
        {
            pPrevNode   = pSearchNode;
            pSearchNode = pSearchNode->pNext;
        }
    }

    //If not in the List, add it.
    if(FALSE == isInList)
    {
        pNewNode = (SVGDOMLiveElement *)MALLOC(sizeof(SVGDOMLiveElement));
        if(NULL == pNewNode)
        {
            return (SVGAL_NO_MEM);
        }

        //Adding to te Tail
        pNewNode->ElementID   = Element;
        pNewNode->pNext       = NULL; 

        if(NULL != pPrevNode)
        {
            pPrevNode->pNext      = pNewNode;
        }else
        {
            pDom->liveElements    = pNewNode;
        }
    }

    return (SVGAL_SUCCESS);
}


/*======================================================================


======================================================================*/
SVGAL CSVGDOMRemoveLiveElement(
    CSVGDOM *       pDom, 
    AEEDOMNodeID    Element)
{
    //SVGDOMLiveElement * pListHead = NULL;
    SVGDOMLiveElement * pRemoveNode  = NULL;
    SVGDOMLiveElement * pPrevNode  = NULL;

    if(NULL == pDom)
    {
        return (SVGAL_BAD_PARAM);
    }

    if(0 == Element)
    {
        return (SVGAL_SUCCESS);
    }

    //Get the List Head
    pRemoveNode = pDom->liveElements;

    while(NULL != pRemoveNode)
    {
        if(Element == pRemoveNode->ElementID)
        {
            //Got the Node to Remove
            if(NULL == pPrevNode)
            {
                //If this node is the 1st node, update the Head
                pDom->liveElements = pRemoveNode->pNext;
            }else
            {
                pPrevNode->pNext = pRemoveNode->pNext;
            }

            //Free the Node
            FREEIF(pRemoveNode);
            return (SVGAL_SUCCESS);

        }else
        {
            pPrevNode   = pRemoveNode;
            pRemoveNode = pRemoveNode->pNext;
        }
    }

    //End of the loop indicates request to Remove a non-existing Node
    return (SVGAL_NOT_EXIST);
}


/*======================================================================


======================================================================*/
SVGAL CSVGDOMDestroyLiveElements(
    CSVGDOM *       pDom)
{
    SVGDOMLiveElement * pNode       = NULL;
    SVGDOMLiveElement * pNodeNext   = NULL;

    if(NULL == pDom)
    {
        return (SVGAL_BAD_PARAM);
    }

    //Get the List Head
    pNode = pDom->liveElements;

    while(NULL != pNode)
    {
        SVGALDOM_Document_DeleteElement(pDom->alCtx, pNode->ElementID);
        pNodeNext = pNode->pNext;

        //Free Memory
        FREEIF(pNode);

        //Goto Next Node
        pNode = pNodeNext;
    }

    pDom->liveElements = NULL;

    return (SVGAL_SUCCESS);
}


/*======================================================================

//Added for OEMs to Control Events

======================================================================*/
void OEMSVGDOM_EventHandler(
    void *                pUserData,
    SVGALDOMEvent *       pInEvt)
{
    int32 strLen;
    SVGEvent * pAppEvt;
    SVGDOMEventContext * pEventContext = (SVGDOMEventContext *)pUserData;

    if((NULL == pUserData) || (NULL == pInEvt))
    {
        return;
    }

    //Get App Event Descriptor
    pAppEvt = &(pEventContext->eventAppData);
    if(NULL == pAppEvt)
    {
        return;
    }


    //Filter Unknown/Other Events
    switch(pInEvt->evtType)
    {
    case SVGALDOM_EVENT_MOUSE:
    case SVGALDOM_EVENT_TEXT:
    case SVGALDOM_EVENT_KEYBOARD:
    case SVGALDOM_EVENT_CONNECTION:
    case SVGALDOM_EVENT_TIME:
    case SVGALDOM_EVENT_UI:
    case SVGALDOM_EVENT_WHEEL:
    case SVGALDOM_EVENT_PROGRESS:
        break;

    default:
        //TODO: Unknown Event. Don't Post it! Is this right?
        return;
    }
  
    if(NULL != pInEvt->pType)
    {
        //Update Event String
        strLen = OEMSVGDOM_GetStrLength(pInEvt->pType);
        if((pEventContext->EventTypeStrBuf.cchBuf < strLen) ||
            (NULL == pEventContext->EventTypeStrBuf.pwBuf))
        {
            //Space not Available, realloc
            FREEIF(pEventContext->EventTypeStrBuf.pwBuf);

            //Account space for NULL termination & allocate (strLen + 1)
            pEventContext->EventTypeStrBuf.pwBuf = MALLOC((strLen + 1) * sizeof(AECHAR));
            if(NULL == pEventContext->EventTypeStrBuf.pwBuf)
            {
                pEventContext->EventTypeStrBuf.cchBuf = 0;
                return; //TODO: Return here?
            }

            pEventContext->EventTypeStrBuf.cchBuf = strLen;
        }

        pAppEvt->pType.pwBuf = pEventContext->EventTypeStrBuf.pwBuf;

        if(NULL !=  pAppEvt->pType.pwBuf)
        {
            //Copy space is available
            WSTRNCOPYN((AECHAR *)pAppEvt->pType.pwBuf, 
            pEventContext->EventTypeStrBuf.cchBuf + 1,
            (AECHAR *)pInEvt->pType, (strLen + 1));

            //Update the length
            pAppEvt->pType.cchBuf = strLen;
        }
    }

    if(NULL != pInEvt->namespaceURI)
    {
        //Update NameSpace URI String
        strLen = OEMSVGDOM_GetStrLength(pInEvt->namespaceURI);        
        if(pEventContext->NameSpaceURIStrBuf.cchBuf < strLen)
        {
            //Space not Available, realloc
            FREEIF(pEventContext->NameSpaceURIStrBuf.pwBuf);

            //Account space for NULL termination & allocate (strLen + 1)
            pEventContext->NameSpaceURIStrBuf.pwBuf = MALLOC((strLen + 1) * sizeof(AECHAR));
            if(NULL == pEventContext->NameSpaceURIStrBuf.pwBuf)
            {
                pEventContext->NameSpaceURIStrBuf.cchBuf = 0;
                return; //TODO: Return here?
            }

            pEventContext->NameSpaceURIStrBuf.cchBuf = strLen;
        }

        pAppEvt->namespaceURI.pwBuf = pEventContext->NameSpaceURIStrBuf.pwBuf;

        if(NULL !=  pAppEvt->namespaceURI.pwBuf)
        {
            //Copy space is available
            WSTRNCOPYN((AECHAR *)pAppEvt->namespaceURI.pwBuf, 
                pEventContext->NameSpaceURIStrBuf.cchBuf + 1,
                (AECHAR *)pInEvt->namespaceURI, (strLen + 1));

            //Update the length
            pAppEvt->namespaceURI.cchBuf = strLen;
        }
    }

    //Update nTarget, nCurrentTarget
    pAppEvt->nTarget         = (AEEDOMNodeID) pInEvt->nTarget;
    pAppEvt->nCurrentTarget  = (AEEDOMNodeID) pInEvt->nCurrentTarget;
    pAppEvt->pElementInstanceObj    = (SVGElementInstance*) pInEvt->pElementInstanceObj;
    //Update Cancellable & defaultPrevented
    pAppEvt->cancelable       = pInEvt->cancelable;
    pAppEvt->defaultPrevented = pInEvt->defaultPrevented;

    switch(pInEvt->evtType)
    {

    case SVGALDOM_EVENT_MOUSE:
        {
            pAppEvt->nType = MM_SVG_EVENT_MOUSE;

            pAppEvt->u.mouse.screenX = pInEvt->u.mouse.screenX;
            pAppEvt->u.mouse.screenY = pInEvt->u.mouse.screenY;
            pAppEvt->u.mouse.clientX = pInEvt->u.mouse.clientX;
            pAppEvt->u.mouse.clientY = pInEvt->u.mouse.clientY;
            pAppEvt->u.mouse.button  = pInEvt->u.mouse.button;
        }

        break;

    case SVGALDOM_EVENT_TEXT:
        {
            pAppEvt->nType = MM_SVG_EVENT_TEXT;

            //Update Event String
            strLen = OEMSVGDOM_GetStrLength(pInEvt->u.text);
            if(pEventContext->EventStrBuf.cchBuf < strLen)
            {
                //Space not Available, realloc
                FREEIF(pEventContext->EventStrBuf.pwBuf);

                //Account space for NULL termination & allocate (strLen + 1)
                pEventContext->EventStrBuf.pwBuf = MALLOC((strLen + 1) * sizeof(AECHAR));
                if(NULL == pEventContext->EventStrBuf.pwBuf)
                {
                    pEventContext->EventStrBuf.cchBuf = 0;
                    return; //TODO: Return here?
                }

                pEventContext->EventStrBuf.cchBuf = strLen;
            }

            pAppEvt->u.text.pwBuf = pEventContext->EventStrBuf.pwBuf;

            if(NULL !=  pAppEvt->u.text.pwBuf)
            {
                //Copy space is available
                WSTRNCOPYN((AECHAR *)pAppEvt->u.text.pwBuf, 
                    pEventContext->EventStrBuf.cchBuf + 1,
                    (AECHAR *)pInEvt->u.text, (strLen + 1));

                //Update the length
                pAppEvt->u.text.cchBuf = strLen;
            }
        }
        break;

    case SVGALDOM_EVENT_KEYBOARD:
        {
            pAppEvt->nType = MM_SVG_EVENT_KEYBOARD;

            //Update Event String
            strLen = OEMSVGDOM_GetStrLength(pInEvt->u.keyId);
            if(pEventContext->EventStrBuf.cchBuf < strLen)
            {
                //Space not Available, realloc
                FREEIF(pEventContext->EventStrBuf.pwBuf);

                //Account space for NULL termination & allocate (strLen + 1)
                pEventContext->EventStrBuf.pwBuf = MALLOC((strLen + 1) * sizeof(AECHAR));
                if(NULL == pEventContext->EventStrBuf.pwBuf)
                {
                    pEventContext->EventStrBuf.cchBuf = 0;
                    return; //TODO: Return here?
                }

                pEventContext->EventStrBuf.cchBuf = strLen;
            }

            pAppEvt->u.keyId.pwBuf = pEventContext->EventStrBuf.pwBuf;

            if(NULL !=  pAppEvt->u.keyId.pwBuf)
            {
                //Copy space is available
                WSTRNCOPYN((AECHAR *)pAppEvt->u.keyId.pwBuf, 
                pEventContext->EventStrBuf.cchBuf + 1,
                (AECHAR *)pInEvt->u.keyId, (strLen + 1));

                //Update the length
                pAppEvt->u.keyId.cchBuf = strLen;
            }
        }
        break;

    case SVGALDOM_EVENT_TIME:
        {
            pAppEvt->nType = MM_SVG_EVENT_TIME;
            pAppEvt->u.nDetail = pInEvt->u.nDetail;
        }
        break;

    case SVGALDOM_EVENT_CONNECTION:
    case SVGALDOM_EVENT_UI:
    case SVGALDOM_EVENT_WHEEL:
    case SVGALDOM_EVENT_PROGRESS:
        break;

    default:
        //INFO: This code shouldn't execute
        break;
    }

    //Call App Event Handler
    (pEventContext->pfnAppEventHandler)(&(pEventContext->eventAppData), pEventContext->pAppUserData);

    return;
}


/*======================================================================


======================================================================*/
int OEMCSVGDOMAddEventListener(
    ISVGDOM *             po, 
    AEEDOMNodeID          idThis, 
    AEEDOMString *        pEventType, 
    AEEDOMEventListener * pListener, 
    boolean               useCapture)
{
    SVGAL                   ret;
    SVGALDOMString          wStrEventType;
    SVGDOMEventContext *    pEventContext   = NULL;
    CSVGDOM *               dom             = (CSVGDOM *) po;
    SVGALDOMEventListenerContext evtListenerCtxt;

    if(!po || !idThis || !pEventType)
    {
        return EBADPARM;
    }

    //Allocate Memory
    ret = CSVGDOMCreateEventContext(dom, idThis, pEventType, pListener, useCapture, &pEventContext);
    if((SVGAL_SUCCESS != ret) || (NULL == pEventContext))
    {
        return ENOMEMORY;
    }

    wStrEventType.str = pEventType->pwBuf;
    wStrEventType.len = pEventType->cchBuf;

    evtListenerCtxt.pfnOEMEventHandler  = OEMSVGDOM_EventHandler;         //OEM Layer Event Handler
    evtListenerCtxt.pOEMUserData        = pEventContext;                  //EventContext as UserData 
    evtListenerCtxt.pSVGALDOMEventData  = &(pEventContext->eventOEMData); // OEM Event Data

    ret = SVGALDOM_AddEventListener(dom->alCtx, (SVGALDomObj)idThis, &wStrEventType, &evtListenerCtxt, useCapture);
    if(SVGAL_SUCCESS != ret)
    {
        CSVGDOMDeleteEventContext(dom, idThis, pEventType, pListener, useCapture, pEventContext);
        return CMediaSVGALErrorToApp(ret);
    }

    //Track the memory for this Event Context
    ret = CSVGDOMAddLiveEventContext(dom, pEventContext);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Cannot Track memory. So Delete Live Context?
    }

    return CMediaSVGALErrorToApp(ret); ;
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMStopPropagation(
    ISVGDOM *   me, 
    SVGEvent *  pEvent)
{
    return (EUNSUPPORTED);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMPreventDefault(
    ISVGDOM *   me, 
    SVGEvent *  pEvent)
{
    return (EUNSUPPORTED);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMGoToLocation(
    ISVGDOM *       me, 
    AEEDOMString *  pwszNewIRI)
{
    return (EUNSUPPORTED);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMGetURL(
    ISVGDOM *                   me, 
    AEEDOMString *              pwszIRI, 
    AEEDOMAsyncStatusCallback * pfnCallback)
{
    return (EUNSUPPORTED);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMPostURL(
    ISVGDOM *                   me, 
    AEEDOMString *              pwszIRI, 
    AEEDOMString *              pwszData, 
    AEEDOMAsyncStatusCallback * pfnCallback, 
    AEEDOMString *              pwszType, 
    AEEDOMString *              pwszEncoding)
{
    return (EUNSUPPORTED);
}

/*======================================================================


======================================================================*/
int OEMCSVGDOMRemoveEventListener(
    ISVGDOM *             po, 
    AEEDOMNodeID          idThis, 
    AEEDOMString *        pEventType, 
    AEEDOMEventListener * pListener, 
    boolean               useCapture)
{
    SVGAL ret;
    SVGALDOMString wStrEventType;
    CSVGDOM * dom = (CSVGDOM *) po;
    SVGDOMEventContext *         pEventContext;
    SVGALDOMEventListenerContext evtListenerCtxt;

    if(!po || !idThis || !pEventType)
    {
        return EBADPARM;
    }    
    
    //Get Event Context from the List
    ret = CSVGDOMGetEventContext(dom, idThis, pEventType, pListener, useCapture, &pEventContext);
    if(SVGAL_SUCCESS != ret)
    {
        return CMediaSVGALErrorToApp(ret);
    }

    wStrEventType.str = pEventType->pwBuf;
    wStrEventType.len = pEventType->cchBuf;

    evtListenerCtxt.pfnOEMEventHandler  = OEMSVGDOM_EventHandler;
    evtListenerCtxt.pOEMUserData        = pEventContext;
    evtListenerCtxt.pSVGALDOMEventData  = &(pEventContext->eventOEMData);

    ret = SVGALDOM_RemoveEventListener(dom->alCtx, (SVGALDomObj)idThis, &wStrEventType, &evtListenerCtxt, useCapture);
    if(SVGAL_SUCCESS != ret)
    {
        //TODO: Did not Delete?
        return CMediaSVGALErrorToApp(ret);
    }

    //TODO: Check Error
    CSVGDOMRemoveLiveEventContext(dom, pEventContext);
    CSVGDOMDeleteEventContext(dom, idThis, pEventType, pListener, useCapture, pEventContext);

    return CMediaSVGALErrorToApp(ret); ;
}


/*======================================================================


======================================================================*/
SVGAL CSVGDOMCreateEventContext(
    CSVGDOM *             pDom, 
    AEEDOMNodeID          NodeID,
    AEEDOMString *        pEventType,
    AEEDOMEventListener * pListener,
    boolean               useCapture,
    SVGDOMEventContext ** ppEventContext)
{
  SVGDOMEventContext * pEventContext = NULL;

  if((NULL == pDom)         ||
      (NULL == pEventType)  ||
      (NULL == pListener)   ||
      (NULL == ppEventContext))
  {
    return (SVGAL_BAD_PARAM);
  }

    *ppEventContext = NULL;

    pEventContext = (SVGDOMEventContext *)MALLOC(sizeof(SVGDOMEventContext));
    if(NULL == pEventContext)
    {
        return (SVGAL_NO_MEM);
    }

    //MALLOC for pEventType String
    pEventContext->EventType.pwBuf = (AECHAR *)MALLOC((pEventType->cchBuf + 1) * sizeof(AECHAR));
    if(NULL == pEventContext->EventType.pwBuf)
    {
        FREEIF(pEventContext);
        return (SVGAL_NO_MEM);
    }

    //Copy the Incoming Parameters  
    pEventContext->pfnAppEventHandler = pListener->evtHandler;
    pEventContext->pAppUserData       = pListener->userData;

    pEventContext->NodeID = NodeID;

    //Copying Event Type
    //TODO: Check Error, So much to do to just copy?
    WSTRNCOPYN(pEventContext->EventType.pwBuf, (pEventType->cchBuf + 1), pEventType->pwBuf, (pEventType->cchBuf + 1));
    pEventContext->EventType.cchBuf = pEventType->cchBuf;

    pEventContext->useCapture = useCapture;

    MEMSET(&(pEventContext->eventAppData),        0x00, sizeof(SVGEvent));
    MEMSET(&(pEventContext->EventTypeStrBuf),     0x00, sizeof(AEEDOMString));
    MEMSET(&(pEventContext->EventStrBuf),         0x00, sizeof(AEEDOMString));
    MEMSET(&(pEventContext->NameSpaceURIStrBuf),  0x00, sizeof(AEEDOMString));
    MEMSET(&(pEventContext->eventOEMData),        0x00, sizeof(SVGALDOMEvent));

    //Update the return pointer
    *ppEventContext = pEventContext;

    return (SVGAL_SUCCESS);
}


/*======================================================================


======================================================================*/
SVGAL CSVGDOMDeleteEventContext(
    CSVGDOM *             pDom, 
    AEEDOMNodeID          NodeID,
    AEEDOMString *        pEventType,
    AEEDOMEventListener * pListener,
    boolean               useCapture,
    SVGDOMEventContext *  pEventContext)
{
    if((NULL == pDom)         ||
      (NULL == pEventType)    ||
      (NULL == pListener)     ||
      (NULL == pEventContext))
    {
      return (SVGAL_BAD_PARAM);
    }
    
    FREEIF(pEventContext->EventType.pwBuf);
    FREEIF(pEventContext->EventStrBuf.pwBuf);
    FREEIF(pEventContext->EventTypeStrBuf.pwBuf);
    FREEIF(pEventContext->NameSpaceURIStrBuf.pwBuf);
    FREEIF(pEventContext);
  
    return (SVGAL_SUCCESS);
}


/*======================================================================


======================================================================*/
SVGAL CSVGDOMGetEventContext(
    CSVGDOM *             pDom, 
    AEEDOMNodeID          NodeID,
    AEEDOMString *        pEventType,
    AEEDOMEventListener * pListener,
    boolean               useCapture,
    SVGDOMEventContext ** ppEventContext)
{
    SVGDOMLiveEventContext * pNode = NULL;

    if( (NULL == pDom)        ||
        (NULL == pEventType)  ||
        (NULL == pListener)   ||
        (NULL == ppEventContext))
    {
        return (SVGAL_BAD_PARAM);
    }

    //Set to NULL
    *ppEventContext = NULL;

    //Get the List Head
    pNode = pDom->liveEventContexts;

    while(NULL != pNode)
    {
        if( (NodeID == pNode->pEventContext->NodeID) &&
            (pListener->evtHandler == pNode->pEventContext->pfnAppEventHandler) &&
            (useCapture == pNode->pEventContext->useCapture) &&
            (0 == WSTRCMP(pEventType->pwBuf, pNode->pEventContext->EventType.pwBuf)))
        { 
            *ppEventContext = pNode->pEventContext;
            return (SVGAL_SUCCESS);

        }else
        {
            pNode = pNode->pNext;
        }
    }

    //End of the loop indicates request to Remove a non-existing Node
    return (SVGAL_NOT_EXIST);
}


/*======================================================================


======================================================================*/
SVGAL CSVGDOMAddLiveEventContext(
    CSVGDOM *             pDom,
    SVGDOMEventContext *  pEventContext)
{

    SVGDOMLiveEventContext * pNewNode;

    if( (NULL == pDom) ||
        (NULL == pEventContext))
    {
        return (SVGAL_BAD_PARAM);
    }

    pNewNode = (SVGDOMLiveEventContext *)MALLOC(sizeof(SVGDOMLiveEventContext));
    if(NULL == pNewNode)
    {
        return (SVGAL_NO_MEM);
    }

    //Adding to te Head - Would it be faster when removing?
    pNewNode->pEventContext   = pEventContext;
    pNewNode->pNext           = pDom->liveEventContexts; 
    pDom->liveEventContexts   = pNewNode;

    return (SVGAL_SUCCESS);
}

SVGAL CSVGDOMRemoveLiveEventContext(
    CSVGDOM *             pDom,
    SVGDOMEventContext *  pEventContext)
{
    //SVGDOMLiveEventContext * pListHead = NULL;
    SVGDOMLiveEventContext * pRemoveNode  = NULL;
    SVGDOMLiveEventContext * pPrevNode  = NULL;

    if(NULL == pDom)
    {
        return (SVGAL_BAD_PARAM);
    }

    //Get the List Head
    pRemoveNode = pDom->liveEventContexts;

    while(NULL != pRemoveNode)
    {
        if(pEventContext == pRemoveNode->pEventContext)
        {
            //Got the Node to Remove
            if(NULL == pPrevNode)
            {
                //If this node is the 1st node, update the Head
                pDom->liveEventContexts = pRemoveNode->pNext;
            }else
            {
                pPrevNode->pNext = pRemoveNode->pNext;
            }

            //Free the Node
            FREEIF(pRemoveNode);
            return (SVGAL_SUCCESS);

        }else
        {
            pPrevNode   = pRemoveNode;
            pRemoveNode = pRemoveNode->pNext;
        }
    }

    //End of the loop indicates request to Remove a non-existing Node
    return (SVGAL_NOT_EXIST);
}


/*======================================================================


======================================================================*/
SVGAL CSVGDOMDestroyLiveEventContexts(
    CSVGDOM *             pDom)
{
    SVGDOMLiveEventContext *  pNode         = NULL;
    SVGDOMLiveEventContext *  pNodeNext     = NULL;
    SVGDOMEventContext *      pEventContext = NULL;
    SVGALDOMString                  wStrEventType;
    SVGALDOMEventListenerContext    evtListenerContext;

    if(NULL == pDom)
    {
        return (SVGAL_BAD_PARAM);
    }

    //Get the List Head
    pNode = pDom->liveEventContexts;

    while(NULL != pNode)
    {
        pEventContext = pNode->pEventContext;

        wStrEventType.str = pEventContext->EventType.pwBuf;
        wStrEventType.len = pEventContext->EventType.cchBuf;   

        evtListenerContext.pfnOEMEventHandler  = OEMSVGDOM_EventHandler;
        evtListenerContext.pOEMUserData        = pEventContext;
        evtListenerContext.pSVGALDOMEventData  = &(pEventContext->eventOEMData);

        SVGALDOM_RemoveEventListener(pDom->alCtx, (SVGALDomObj)pEventContext->NodeID, 
          &wStrEventType, &evtListenerContext, pEventContext->useCapture);

        FREEIF(pEventContext->EventType.pwBuf);
        FREEIF(pEventContext->EventStrBuf.pwBuf);
        FREEIF(pEventContext->EventTypeStrBuf.pwBuf);
        FREEIF(pEventContext->NameSpaceURIStrBuf.pwBuf);
        FREEIF(pEventContext);

        pNodeNext = pNode->pNext;

        //Free Memory
        FREEIF(pNode);

        //Goto Next Node
        pNode = pNodeNext;
    }

    pDom->liveEventContexts = NULL;

    return (SVGAL_SUCCESS);
}


/*======================================================================


======================================================================*/
int32 OEMSVGDOM_GetStrLength(
  SVGALDOMStrBuf strBuf)
{
    int32 count = 0;
    AECHAR * pin = (AECHAR *) strBuf;

    while(*pin)
    {
        count++;
        pin++;
    }

    return (count);
}


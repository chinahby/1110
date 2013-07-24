/*=====================================================================

             B R E W   S V G   S E R V I C E S

FILE
    OEMSVG.c

GENERAL DESCRIPTION
   This file defines Scalable Vector Graphics (SVG) services.

EXTERNALIZED FUNCTIONS
    None

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

      Copyright (c) 2006, 2007 by QUALCOMM, Inc.  All Rights Reserved.
======================================================================*/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/graphics/svg/platform/brew/imediasvg/main/latest/src/OEMSVG.c#11 $
  $DateTime: 2009/02/24 01:01:46 $
  $Author: ranjhith $

when       who      what, where, why
--------   ---      --------------------------------------------------------
05/30/06   ae       Initial version of file.
21/09/06   rk       Updated with support for PARM_RECT & PRAM_CAPS
======================================================================*/

/*=====================================================================
                     INCLUDE FILES FOR MODULE
======================================================================*/


//implements the OEM layer of the SVG SDK extension. The application (AEE layer) will call into the
//OEM functions here

#ifndef SVG_DYNAMICEXTENSION
#include "OEMFeatures.h"
#endif

#include "AEEClassIDs.h"
#include "AEE.h"

#include "OEMSVG.h"
#if defined(FEATURE_SVGDOM)
#include "OEMSVGDOM.h"
#endif //defined(FEATURE_SVGDOM)
#include "svgengineALpublic.h"
#include "AEEFile.h"

#ifndef SVG_DYNAMICEXTENSION
#include "OEMMediaFormats.h"
#include "OEMMedia.h"
#endif

#include "AEEMimeTypes.h"

/*=====================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
======================================================================*/

const uint32 OEMSVG_MAX_URL_LENGTH = 512;
const uint32 OEMSVG_ENGINE_DPI = 72;
const char mediaSVGEmptyDoc[] = "<svg version=\"1.2\" baseProfile=\"tiny\"> </svg>";

/*=====================================================================
               FUNCTION DECLARATIONS FOR MODULE
======================================================================*/

#ifndef SVG_DYNAMICEXTENSION
void IMediaSVG_Init(IShell * ps);
int  IMediaSVG_New(IShell * pIShell, AEECLSID ClsId, void **ppObj);
#endif


int CMediaSVGAllocateFrameBufferCb(void* userData, unsigned short* width, unsigned short* height, unsigned char bpp,
                                   void** frameBuffer, int* isGreaterThanScreen);
int CMediaSVGBindContent(CMediaSVG * pMediaSVG, SVGALMediaSpec * pMediaSpec);
int  CMediaSVGSeekToTime(CMediaSVG* pMediaSVG, MediaSVGSeekMode seekMode, int32 seekMS);
void CMediaSVGUpdateAnimationTime(CMediaSVG * pMediaSVG);
void CMediaSVGUpdateAnimation(void* pData);
int  CMediaSVGCreateNotifyMsg(CMediaSVG* pMediaSVG, int nCmd, int nSubCmd, int nStatus, void* pData,
                              uint32 pDataSize, boolean async, boolean asyncSend);
void CMediaSVGCancelNextFrame(CMediaSVG* pMediaSVG);

CMediaSVGListNode* CMediaSVGURLRequestListGetNode(CMediaSVGList* list, const char* urlStr);
boolean CMediaSVGListRemove(CMediaSVGList* list, CMediaSVGListNode* node);
boolean CMediaSVGListInsert(CMediaSVGList* list, CMediaSVGListNode* node);
int CMediaSVGGetMediaSpec(SVGALMediaSpec * pSVGALMediaSpecData, AEEMediaSVGSpec * pMediaSpec);

int OEMCMediaSVGStop(IMedia* pm);
int OEMCMediaSVGDocumentVerify(IMedia* pm, SVGDocumentVerify* verify);

int CMediaSVGHandleSetMediaData(CMediaSVG* pMediaSVG, int32 param1, int32 param2);

/*======================================================================


======================================================================*/
int CMediaSVGALErrorToApp(SVGAL error)
{
    int ret;
    switch(error)
    {
    case SVGAL_SUCCESS:
        ret = SUCCESS;
        break;
    case SVGAL_INDEX_SIZE:
        ret = SVGDOM_ERROR_INDEX_SIZE;
        break;
    case SVGAL_HIERARCHY:
        ret = SVGDOM_ERROR_HIERARCHY_REQUEST;
        break;
    case SVGAL_WRONG_DOC:
        ret = SVGDOM_ERROR_WRONG_DOCUMENT;
        break;
    case SVGAL_INVALID_MOD:
        ret = SVGDOM_ERROR_INVALID_MODIFICATION;
        break;
    case SVGAL_INVALID_ACCESS:
        ret = SVGDOM_ERROR_INVALID_ACCESS;
        break;
    case SVGAL_INVALID_VALUE:
//        ret = SVGDOM_ERROR_INVALID_VALUE;
        ret = EFAILED;
        break;
    case SVGAL_MATRIX_NOT_INVERTABLE:
        ret = SVGDOM_ERROR_MATRIX_NOT_INVERTABLE;
        break;
    case SVGAL_MOD_NOT_ALLOWED:
        ret = ENOTALLOWED;
        break;
    case SVGAL_NOT_EXIST:
        ret = ENOSUCH;
        break;
    case SVGAL_UNSUPPORTED:
        ret = EUNSUPPORTED;
        break;
    case SVGAL_TYPE_MISMATCH:
        ret = ENOTYPE;
        break;
    case SVGAL_BAD_STATE:
        ret = EBADSTATE;
        break;
    case SVGAL_NO_MEM:
        ret = ENOMEMORY;
        break;
    case SVGAL_BAD_PARAM:
        ret = EBADPARM;
        break;
    default:
        ret = EFAILED;
        break;

    }

    return ret;

}


/*======================================================================


======================================================================*/
int CMediaSVGHandleClientFrameGenerationEnable(CMediaSVG* pMediaSVG, int32 param1, int32 param2)
{
    boolean enable = (boolean) param1;

    if(pMediaSVG == NULL)
    {
        return EBADPARM;
    }

    if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAYING)
    {
        OEMSVG_OUTPUT("OEMSVG EnableClientFrameGen() called while in play state, must call Stop() first");
        return EBADSTATE;
    }


    if(enable == TRUE)
        pMediaSVG->m_isClientFrameGenEnabled = TRUE;
    else
        pMediaSVG->m_isClientFrameGenEnabled = FALSE;

    return SUCCESS;

}


/*======================================================================


======================================================================*/
int CMediaSVGHandleDOMCreateEmptyDoc(CMediaSVG* pMediaSVG, int32 param1, int32 param2)
{
    boolean enable = (boolean) param1;

    if(enable != pMediaSVG->m_isDomEmptyDocEnabled)
    {
        //can only modify in idle state, (ie. only when SetMediaData has not been called)
        if(pMediaSVG->m_pMediaSVGState != MEDIASVG_STATE_IDLE)
        {
            OEMSVG_OUTPUT("OEMSVG Attempt to enable DOM Empty Doc when not in Idle state");
            return EBADSTATE;
        }

        pMediaSVG->m_isDomEmptyDocEnabled = enable;

        if(TRUE == enable)
        {
            //this is where we attach an empty document to player if this is enabled
            AEEMediaData md;

            md.clsData = MMD_BUFFER;
            md.pData = (void *) mediaSVGEmptyDoc;
            md.dwSize = STRLEN(mediaSVGEmptyDoc);

            CMediaSVGHandleSetMediaData(pMediaSVG, (int32) &md, 0);
        }
    }

    return SUCCESS;
}

/*======================================================================


======================================================================*/
int CMediaSVGHandleGenerateFrame(CMediaSVG* pMediaSVG, int32 param1, int32 param2)
{
    uint32 nFrameTime = (uint32) param1;

    if(pMediaSVG == NULL)
    {
        return EBADPARM;
    }

    if(pMediaSVG->m_isClientFrameGenEnabled == FALSE)
    {
        OEMSVG_OUTPUT("OEMSVG CMediaSVGHandleGenerateFrame() called without enabling client frame gen");
        return EBADSTATE;
    }

    switch(pMediaSVG->m_pMediaSVGState)
    {
    case MEDIASVG_STATE_READY:
        //seek to the desired animation time
        CMediaSVGSeekToTime(pMediaSVG, MEDIASVG_SEEKMODE_BEGIN, nFrameTime);

        //generate the frame
        CMediaSVGNextFrame(pMediaSVG, TRUE, 0);
        return SUCCESS;
    case MEDIASVG_STATE_IDLE:
        OEMSVG_OUTPUT("OEMSVG GenerateFrame() called while in idle state, need to set media data first");
        return EBADSTATE;
    case MEDIASVG_STATE_PLAYING:
        OEMSVG_OUTPUT("OEMSVG GenerateFrame() called while in play state, must call Stop() first");
        return EBADSTATE;
    case MEDIASVG_STATE_PLAY_PAUSE:
        OEMSVG_OUTPUT("OEMSVG GenerateFrame() called while in pause state, must resume first");
        return EBADSTATE;
    default:
        OEMSVG_OUTPUT("OEMSVG GenerateFrame() Bad state encountered");
        return EFAILED;

    }

}



/*======================================================================


======================================================================*/
int CMediaSVGHandleInputEventKey(CMediaSVG* pMediaSVG, int32 keyEvent, SVGKeyData* keyData)
{
    SVGALKeyEvent alEvent;
    unsigned int handled;

    if(keyData->nVirtualKey != MM_SVG_VK_UNDEFINED || keyData->nModifier != MM_SVG_KEY_MOD_NONE)
    {
        //only SVG Tiny 1.1 supports key modifiers and virtual keys
        return EUNSUPPORTED;
    }

    switch(keyEvent)
    {
    case MM_SVG_EVENT_TEXT_INPUT:
        alEvent = SVGAL_KEY_EVENT_TEXT_INPUT;
        break;
    case MM_SVG_EVENT_KEY_DOWN:
        alEvent = SVGAL_KEY_EVENT_DOWN;
        break;
    case MM_SVG_EVENT_KEY_UP:
        alEvent = SVGAL_KEY_EVENT_UP;
        break;
    default:
        return EBADPARM;
    }

    SVGALHandleKeyEvent(pMediaSVG->alCtx, alEvent, keyData->nKey, &handled);

    return SUCCESS;

}


/*======================================================================


======================================================================*/
int CMediaSVGHandleInputEventFocus(CMediaSVG* pMediaSVG, int16 focusEvent)
{
    int dir;
    SVGALFocusEvent alEvent;
    switch(focusEvent)
    {

    case MM_SVG_FOCUS_EVENT_ACTIVATE:
        alEvent = SVGAL_FOCUS_EVENT_ACTIVATE;
        dir = 0;
        break;

    case MM_SVG_FOCUS_EVENT_NEXT:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_NEXT;
        break;

    case MM_SVG_FOCUS_EVENT_PREVIOUS:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_PREV;
        break;

    case MM_SVG_FOCUS_EVENT_UP:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_UP;
        break;

    case MM_SVG_FOCUS_EVENT_UP_RIGHT:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_UP_RIGHT;
        break;

    case MM_SVG_FOCUS_EVENT_RIGHT:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_RIGHT;
        break;

    case MM_SVG_FOCUS_EVENT_DOWN_RIGHT:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_DOWN_RIGHT;
        break;

    case MM_SVG_FOCUS_EVENT_DOWN:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_DOWN;
        break;

    case MM_SVG_FOCUS_EVENT_DOWN_LEFT:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_DOWN_LEFT;
        break;

    case MM_SVG_FOCUS_EVENT_LEFT:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_LEFT;
        break;

    case MM_SVG_FOCUS_EVENT_UP_LEFT:
        alEvent = SVGAL_FOCUS_EVENT_MOVE;
        dir = SVGAL_FOCUS_EVENT_UP_LEFT;
        break;

    default:
        return EBADPARM;

    }

    if(SVGALHandleFocusEvent(pMediaSVG->alCtx, alEvent, dir) != SVGAL_SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int CMediaSVGHandleInputEventPointer(CMediaSVG* pMediaSVG, int32 pointerEvent, SVGPointerData* pointerData)
{
    SVGALPointerEvent alEvent;

    switch(pointerEvent)
    {
    case MM_SVG_EVENT_POINTER_MOVE:
        alEvent = SVGAL_POINTER_EVENT_MOVE;
        break;
    case MM_SVG_EVENT_POINTER_DOWN:
        alEvent = SVGAL_POINTER_EVENT_DOWN;
        break;
    case MM_SVG_EVENT_POINTER_UP:
        alEvent = SVGAL_POINTER_EVENT_UP;
        break;
    case MM_SVG_EVENT_POINTER_CLICK:
        alEvent = SVGAL_POINTER_EVENT_CLICK;
        break;
    default:
        return EBADPARM;

    }

    if ( SVGALHandlePointerEvent(pMediaSVG->alCtx, alEvent, pointerData->x, pointerData->y) == SVGAL_FAIL )
    {
        return EFAILED;
    }

//    CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_SETMEDIAPARM, MM_PARM_SVG_INPUT, MM_STATUS_DONE,
//                             (void*) pointerEvent, sizeof(pointerEvent) );

    return SUCCESS;

}



/*======================================================================


======================================================================*/
int CMediaSVGHandleUserInput(CMediaSVG* pMediaSVG, int32 param1, int32 param2)
{
    int ret = EBADSTATE;
    SVGRotateData* rotData;
    SVGZoomData* zoomData;
    SVGPanData* panData;
    boolean zoomCenter;
    boolean panFast;
    int32 inputEvent = param1;


    if(pMediaSVG == NULL)
    {
        return EBADPARM;
    }

    if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_IDLE)
    {
        OEMSVG_OUTPUT("OEMSVG CMediaSVGHandleUserInput() called while in idle state");
        return EBADSTATE;
    }
    else if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAY_PAUSE)
    {
        OEMSVG_OUTPUT("OEMSVG CMediaSVGHandleUserInput() called while in idle state");
        return EBADSTATE;
    }

    //Added to update animation
    CMediaSVGUpdateAnimationTime(pMediaSVG);

    switch(inputEvent)
    {

    case MM_SVG_EVENT_KEY:
    case MM_SVG_EVENT_KEY_DOWN:
    case MM_SVG_EVENT_KEY_UP:
        ret = CMediaSVGHandleInputEventKey(pMediaSVG, inputEvent, (SVGKeyData*) param2);
        break;

    case MM_SVG_EVENT_POINTER_MOVE:
    case MM_SVG_EVENT_POINTER_DOWN:
    case MM_SVG_EVENT_POINTER_UP:
    case MM_SVG_EVENT_POINTER_CLICK:
        ret = CMediaSVGHandleInputEventPointer(pMediaSVG, inputEvent, (SVGPointerData*) param2);
        break;

    case MM_SVG_EVENT_ROTATE:
        rotData = (SVGRotateData *) param2;
        SVGALRotate(pMediaSVG->alCtx, (int) rotData->cx, (int) rotData->cy, (int) rotData->nAngle);
        //CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_SETMEDIAPARM, MM_PARM_SVG_INPUT, MM_STATUS_DONE ,
        //                        (void*) MM_SVG_EVENT_ROTATE, sizeof(int32) );
        ret = SUCCESS;
        break;

    case MM_SVG_EVENT_FOCUS:
        ret = CMediaSVGHandleInputEventFocus(pMediaSVG, (int16) param2);
        break;

    case MM_SVG_EVENT_ZOOM:
        zoomData = (SVGZoomData *) param2;
        if(zoomData->flgCentered == MM_SVG_CENTER_MODE_CENTERED)
            zoomCenter = TRUE;
        else
            zoomCenter = FALSE;
        SVGALZoom(pMediaSVG->alCtx, zoomData->x, zoomData->y, zoomData->nZoomFactor, zoomCenter);
        ret = SUCCESS;
        break;

    case MM_SVG_EVENT_PAN:
        panData = (SVGPanData *) param2;
        if(panData->flgScrollMode == MM_SVG_SCROLL_MODE_FAST)
            panFast = TRUE;
        else
            panFast = FALSE;
        SVGALPan(pMediaSVG->alCtx, panData->x, panData->y, panFast);
        ret = SUCCESS;
        break;

    case MM_SVG_EVENT_RESET_FRAME:
        SVGALReset(pMediaSVG->alCtx);
        ret = SUCCESS;
        break;
    }
    
    //INFO: Optimized Frame Redraws                  
    //no more frames pending
    pMediaSVG->m_isFramePending = FALSE;
    if((SUCCESS == ret) && (pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAYING))
    {
      //and we're in playing state, continue the animation
      CMediaSVGNextFrame(pMediaSVG, TRUE, 0);
    }

    return ret;

}


/*======================================================================


======================================================================*/
void CMediaSVGHandleDynamicRequest(void* userData)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) userData; 
    
    if( SVGALGetExternalReferences(pMediaSVG->alCtx, pMediaSVG->m_resourceLocation) != SVGAL_SUCCESS)
    {
        //not much we can do if there was an error here. 
        OEMSVG_OUTPUT("OEMSVG:: Error getting dynamic requests");
    }
}


/*======================================================================

//This function gets called by AL layer to notify OEM layer that there is
//a dynamic request to be processed. 
//NOTE: this cb function *may not* call back into the AL layer. It must
//set a timer or post an event to be processed as done below. 

======================================================================*/
void CMediaSVGDynamicRequestToProcessCb(void* userData)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) userData; 

    ISHELL_SetTimer(pMediaSVG->m_pIShell, 0, CMediaSVGHandleDynamicRequest, pMediaSVG);  
}


/*======================================================================


======================================================================*/
int CMediaSVGSetURLHandling(CMediaSVG* pMediaSVG, int32 param1, int16 param2)
{
    boolean appHandleURL = (boolean) param1;
    int16 resLocation = (int16) param2;

    if(appHandleURL == TRUE)
    {
        pMediaSVG->m_isClientURLHandling = TRUE;
        pMediaSVG->m_resourceLocation = resLocation;
    }
    else
    {
        pMediaSVG->m_isClientURLHandling = FALSE;
        pMediaSVG->m_resourceLocation = 0;
    }

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int CMediaSVGHandleURLReply(CMediaSVG* pMediaSVG, int32 param1, int32 param2)
{
    SVGAL ret;
    boolean bAck;
    SVGURIResourceDataAck dataAck;
    //CMediaSVGListNode* node;
#ifdef OEMSVG_DISABLE_HANDLING_REQUIRED_RESOURCES
    boolean sendPrepareDone = FALSE;
#endif //OEMSVG_DISABLE_HANDLING_REQUIRED_RESOURCES
    SVGURIResourceData* id = (SVGURIResourceData *) param1;

    if(id == NULL)
        return EBADPARM;

    //URL can't be NULL for a reply, but allow buffer to be NULL and size to be 0
    //to indicate a failed request or an empty request
    if(id->pURL == NULL)
        return EBADPARM;

    ret = SVGALHandleURLRequestReply(pMediaSVG->alCtx, id->pURL, &id->pBuffer[0], id->dwBufferSize);

    if(ret == SVGAL_SUCCESS)
        bAck = TRUE;
    else
        bAck = FALSE;

#ifdef OEMSVG_DISABLE_HANDLING_REQUIRED_RESOURCES
    //check if the URL is in the required node pending list
    node = CMediaSVGURLRequestListGetNode(&pMediaSVG->m_requiredURLRequestQueue, id->pURL);

    if(node)
    {
        boolean rem = CMediaSVGListRemove(&pMediaSVG->m_requiredURLRequestQueue, node);
        if(rem)
        {
            FREE(node->data);
            FREE(node);

            //reduce the count of pending required URLs
            pMediaSVG->m_numPendingRequiredURLs--;

            if(pMediaSVG->m_numPendingRequiredURLs == 0)
            {
                //if no more pending requests, we can send the prepare done (if we haven't done so already),
                //and begin the animation (again if we haven't done so already)
                if(pMediaSVG->m_sentPrepareDone == FALSE)
                {
                    sendPrepareDone = TRUE;
                }

            }
        }
    }
#endif //OEMSVG_DISABLE_HANDLING_REQUIRED_RESOURCES

    dataAck.pURL = STRDUP(id->pURL);
    dataAck.bLoaded = bAck;

    CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_SVG_URI_DATA, &dataAck,
                             sizeof(SVGURIResourceDataAck), FALSE, FALSE);

#ifdef OEMSVG_DISABLE_HANDLING_REQUIRED_RESOURCES
    if(sendPrepareDone)
    {
        //Send the prepare done, and if not in client controlled mode start the animation playback

        pMediaSVG->m_sentPrepareDone = TRUE;

        CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_SVG_PREPARE_DONE, NULL,
                                         0, TRUE, TRUE); //send prepare done as all external requests are complete

        if(pMediaSVG->m_isClientFrameGenEnabled == FALSE)
        {
            CMediaSVGNextFrame(pMediaSVG, TRUE, 0); //start automatic playback
        }
    }
#endif

    FREE(dataAck.pURL);

    return SUCCESS;

}


/*======================================================================


======================================================================*/
int CMediaSVGHandleBkColor(CMediaSVG* pMediaSVG, int32 param1, int32 param2)
{
    uint32 red;
    uint32 green;
    uint32 blue;
    RGBVAL rgb = (RGBVAL) param1;

    if(pMediaSVG == NULL)
    {
        return EBADPARM;
    }

    red     = ( (rgb >> 8)  & 0xFF);
    green   = ( (rgb >> 16) & 0xFF);
    blue    = ( (rgb >> 24) & 0xFF);

    // Since this is going to be a BackGround color, setting Alpha to be fully opaque
    SVGALSetBackgroundColor(pMediaSVG->alCtx, (uint32)0xFF, red, green, blue);

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int CMediaSVGHandleSetMediaData(CMediaSVG* pMediaSVG, int32 param1, int32 param2)
{
    if(pMediaSVG == NULL)
    {
        return EBADPARM;
    }

    switch(pMediaSVG->m_pMediaSVGState)
    {
        case MEDIASVG_STATE_IDLE:
        {
            //can only SetMediaData once per MEDIASVG instance and only from IDLE set
            AEEMediaData *md;
            IFile* svgFile = NULL;
            int32 numMediaExEntries = param2;

            if ( numMediaExEntries != 0 )
            {
                //caller is trying to use IMEDIA_SetMediaDataEx()
                return EUNSUPPORTED;
            }

            //we have an AEEMediaData
            md = (AEEMediaData *) param1;

            if ( md == NULL || md->pData == NULL )
            {
                return EBADPARM;
            }

            if( md->clsData != MMD_FILE_NAME && md->clsData != MMD_BUFFER)
            {
                return EUNSUPPORTED;
            }

            if ( md->clsData == MMD_FILE_NAME )
            {
                //make sure we can open the file
                svgFile = IFILEMGR_OpenFile(pMediaSVG->m_pFileMgr, md->pData, _OFM_READ);
                if(svgFile == NULL)
                {
                    OEMSVG_OUTPUT("OEMSVG Unable to open file named: %s", md->pData);
                    return EBADPARM;
                }

                IFILE_Release(svgFile);
            }

            pMediaSVG->m_currMediaData.clsData = md->clsData;
            pMediaSVG->m_currMediaData.pData = md->pData;
            pMediaSVG->m_currMediaData.dwSize = md->dwSize;

            pMediaSVG->m_isContentBound = FALSE;            

            //we've seen the content to be existing, so move to ready state
            pMediaSVG->m_pMediaSVGState = MEDIASVG_STATE_READY;

            return SUCCESS;
        }
        case MEDIASVG_STATE_READY:
        case MEDIASVG_STATE_PLAYING:
        case MEDIASVG_STATE_PLAY_PAUSE:
        OEMSVG_OUTPUT("OEMSVG SetMediaData() called twice, Illegal");
        return EBADSTATE;

        default:
        OEMSVG_OUTPUT("OEMSVG SetMediaData() Bad state encountered");
        return EFAILED;
    }

}


/*======================================================================


======================================================================*/
int CMediaSVGFileToBufferCb(void* userData, const char* filenameIn, void** bufferOut, unsigned int* bufSize)
{
    FileInfo fileInfo;
    IFile* file;
    void* fileBuffer;
    CMediaSVG* pMediaSVG = (CMediaSVG*) userData;

    file = IFILEMGR_OpenFile(pMediaSVG->m_pFileMgr, filenameIn, _OFM_READ);

    if(file != NULL)
    {
        IFILE_GetInfo(file, &fileInfo);

        fileBuffer = MALLOC(fileInfo.dwSize);

        if(fileBuffer != NULL)
        {
            IFILE_Read(file, fileBuffer, fileInfo.dwSize);

            *bufferOut = fileBuffer;
            *bufSize = fileInfo.dwSize;
        }
        else
        {
            return 0;
        }

        IFILE_Release(file);
    }
    else
    {
        return 0;
    }

    return 1; //success

}


/*======================================================================


======================================================================*/
int CMediaSVGURLRequestNotifier(void* userData, const SVGALURLRequest* urlArray, int numURLs)
{
    int i;
    SVGURIResourceRequestData* svgImageReq;
    boolean last = FALSE;
    CMediaSVG* pMediaSVG = (CMediaSVG*) userData;

    if(numURLs < 1)
        return 0;

    for( i=0; i < numURLs; i++)
    {
        if( i == numURLs-1 )
        {
            last = TRUE;
        }

//disable handling of required resources as BF will handle internally. 
#ifdef OEMSVG_DISABLE_HANDLING_REQUIRED_RESOURCES
        if(urlArray[i].required)
        {
            //the URL request is required. Insert the request into a list of pending URLs that are
            //required to be complete.
            CMediaSVGListNode* node = MALLOC(sizeof(CMediaSVGListNode));

            if(node)
            {
                node->data = STRDUP(urlArray[i].urlStr);

                if(node->data)
                {
                    if(CMediaSVGListInsert(&pMediaSVG->m_requiredURLRequestQueue, node) != TRUE)
                    {
                        OEMSVG_OUTPUT("OEMSVG Unable to insert into required request queue");
                        FREE(node->data);
                        FREE(node);
                        continue; //try the next request
                    }

                    pMediaSVG->m_numPendingRequiredURLs++;
                }
                else
                {
                    FREE(node);
                    continue;
                }
            }

        }
#endif //OEMSVG_DISABLE_HANDLING_REQUIRED_RESOURCES


        //send an asynchronous message to the app notifier indicating there's a URL image request
        svgImageReq = MALLOC(sizeof(SVGURIResourceRequestData));

        if(svgImageReq)
        {
            svgImageReq->pURL = STRDUP(urlArray[i].urlStr);

            if(svgImageReq->pURL)
            {
                svgImageReq->bMoreRequests = !last;


                //CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_SVG_URL_IMAGE, svgImageReq,
                //                         sizeof(SVGURIResourceRequestData), TRUE, last);

                CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_SVG_URI_RESOURCE, svgImageReq,
                                         sizeof(SVGURIResourceRequestData), FALSE, FALSE);

                FREE(svgImageReq->pURL); 
                FREE(svgImageReq); 
            }
            else
            {
                FREE(svgImageReq);
            }

        }
    }

    return 1; //success
}


/*======================================================================


======================================================================*/
CMediaSVGListNode* CMediaSVGURLRequestListGetNode(CMediaSVGList* list, const char* urlStr)
{
    CMediaSVGListNode* nodePtr;

    if(list == NULL || urlStr == NULL || list->head == NULL)
        return NULL;

    nodePtr = list->head;

    while(nodePtr != NULL)
    {
        char* req = (char*) nodePtr->data;

        if( STRNCMP(req, urlStr, OEMSVG_MAX_URL_LENGTH) == 0)
        {
            return nodePtr;
        }
        else
        {
            nodePtr = nodePtr->next;
        }
    }

    return NULL;

}


/*======================================================================

//inserts at end of list, caller must allocate data

======================================================================*/
boolean CMediaSVGListInsert(CMediaSVGList* list, CMediaSVGListNode* node)
{
    if(list == NULL || node == NULL)
        return FALSE;

    node->next = NULL;

    if(list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
                list->tail = node;
    }

    return TRUE;
}

/*======================================================================

//removes the node from the list, caller is responsible for freeing data

======================================================================*/
boolean CMediaSVGListRemove(CMediaSVGList* list, CMediaSVGListNode* node)
{
    CMediaSVGListNode* nodePtr;

    if(list == NULL || node == NULL || list->head == NULL)
        return FALSE;


    if(list->head == node)
    {
        //node we want is at head
        list->head = list->head->next;

        if(list->tail == node)
        {
            //if only 1 element, set tail to NULL also
            list->tail = NULL;
        }

        return TRUE;
    }

    nodePtr = list->head;

    while(nodePtr->next != NULL)
    {
        if(nodePtr->next == node)
        {
                nodePtr->next = nodePtr->next->next;

                if(nodePtr->next == NULL)
                {
                    //removed the tail
                    list->tail = nodePtr;
                }

                return TRUE;

        }
        else
        {
            nodePtr = nodePtr->next;
        }

    }

    return FALSE;

}


/*======================================================================


======================================================================*/
void CMediaSVGListDestroy(CMediaSVGList* list)
{
    CMediaSVGListNode* nextNode;
    CMediaSVGListNode* nodePtr = list->head;

    while(nodePtr)
    {
        nextNode = nodePtr->next;
        FREE(nodePtr->data);
        FREE(nodePtr);
        nodePtr = nextNode;
    }

    list->head = NULL;
    list->tail = NULL;

}

/*======================================================================


======================================================================*/
//removes the head element, caller must free data
boolean CMediaSVGListDequeue(CMediaSVGList* list, CMediaSVGListNode** node)
{
    CMediaSVGListNode* tempNode;

    if(list == NULL || node == NULL || list->head == NULL)
        return FALSE;

    tempNode = list->head;
    list->head = list->head->next;

    if(list->head == NULL)
        list->tail = NULL;

    *node = tempNode;

    return TRUE;

}


/*======================================================================


======================================================================*/
void CMediaSVGFreeCmdNotifyData(CMediaSVGListNode* node)
{
//    AEEMediaCmdNotify* cmdNotify = (AEEMediaCmdNotify*) node->data;

    if(node)
    {
        if(node->data)
        {
            AEEMediaCmdNotify* cmdNotify = (AEEMediaCmdNotify*) node->data;

            switch(cmdNotify->nStatus)
            {
                case MM_STATUS_SVG_URI_RESOURCE:
                {
                    SVGURIResourceRequestData* svgImageReq = cmdNotify->pCmdData;

                    if(svgImageReq)
                    {
                        if(svgImageReq->pURL)
                        {
                            FREE(svgImageReq->pURL);
                            svgImageReq->pURL = NULL;
                        }

                        FREE(svgImageReq);
                        svgImageReq = NULL;

                    }
                }
                default:
                    break;

            }

            FREE(node->data);
        }

        FREE(node);
    }

}


/*======================================================================


======================================================================*/
void CMediaSVGAsyncCallAppNotifyCb(void* data)
{
    AEEMediaCmdNotify* cmdNotify;
    CMediaSVG* pMediaSVG = (CMediaSVG*) data;

    while(1)
    {
        CMediaSVGListNode* node;

        //dequeue will remove the head element from the list. Caller still needs to free memory
        boolean haveEvent = CMediaSVGListDequeue(&pMediaSVG->m_appEventQueue, &node);

        if(haveEvent)
        {
            cmdNotify = (AEEMediaCmdNotify *) node->data;

            pMediaSVG->m_aeeMediaNotifyCallback(pMediaSVG->m_aeeMediaNotifyCallbackData, cmdNotify);

            //As cmds are popped off the queue, free any data that may have been previously allocated
            CMediaSVGFreeCmdNotifyData(node);
        }
        else
            break;

    }

}


/*======================================================================


======================================================================*/
int CMediaSVGCreateNotifyMsg(CMediaSVG* pMediaSVG, int nCmd, int nSubCmd, int nStatus, void* pData,
                             uint32 pDataSize, boolean async, boolean asyncSend)
{
    AEEMediaCmdNotify cmdNotify;

    if(pMediaSVG->m_aeeMediaNotifyCallback != NULL)
    {
        MEMSET(&cmdNotify, 0, sizeof(cmdNotify));
        cmdNotify.clsMedia = AEECLSID_MEDIASVG;
        cmdNotify.pIMedia  = (IMedia*) pMediaSVG;
        cmdNotify.nCmd       = nCmd;
        cmdNotify.nSubCmd  = nSubCmd;
        cmdNotify.nStatus  = nStatus;
        cmdNotify.pCmdData = pData;
        cmdNotify.dwSize   = pDataSize;

        if(async)
        {
            //we have an asynchronous message to deliver to the app. Allocate the node and insert it into
            //the list of pending events
            CMediaSVGListNode* node = MALLOC(sizeof(CMediaSVGListNode));
            node->data = MALLOC(sizeof(AEEMediaCmdNotify));

            MEMCPY(node->data, &cmdNotify, sizeof(AEEMediaCmdNotify));

            if(CMediaSVGListInsert(&pMediaSVG->m_appEventQueue, node) != TRUE)
            {
                OEMSVG_OUTPUT("OEMSVG Unable to insert into app event queue");
            }

            if(asyncSend)
            {
                //Cancel the previously Registered Event
                if(pMediaSVG->m_AsyncAppNotifyCb.pfnCancel)
                {
                    pMediaSVG->m_AsyncAppNotifyCb.pfnCancel(&pMediaSVG->m_AsyncAppNotifyCb);
                }

                //Register the New Event
                pMediaSVG->m_AsyncAppNotifyCb.pfnNotify = CMediaSVGAsyncCallAppNotifyCb;
                pMediaSVG->m_AsyncAppNotifyCb.pNotifyData = pMediaSVG;
                pMediaSVG->m_AsyncAppNotifyCb.pfnCancel = NULL;
                ISHELL_Resume(pMediaSVG->m_pIShell, &pMediaSVG->m_AsyncAppNotifyCb);
            }
        }
        else
        {
            //send the info to the app
            pMediaSVG->m_aeeMediaNotifyCallback(pMediaSVG->m_aeeMediaNotifyCallbackData, &cmdNotify);

        }
    }

    return SUCCESS;

}


/*======================================================================


======================================================================*/
int OEMSVGInit(CMediaSVG* pMediaSVG, void** ppalCtx, uint32 width, uint32 height, uint32 dpi)
{
    IBitmap * deviceBmp;
    AEEBitmapInfo bi;

    if(ISHELL_CreateInstance(pMediaSVG->m_pIShell, AEECLSID_FILEMGR, (void**)(&pMediaSVG->m_pFileMgr)) != SUCCESS)
    {
        OEMSVG_OUTPUT("OEMSVG Unable to create FileMgr");
        return EFAILED;
    }

    if( SVGALInit(ppalCtx, width, height, dpi, TRUE) != SVGAL_SUCCESS)
    {
        OEMSVG_OUTPUT("OEMSVG Unable to initialize SVG abstraction layer");
        return EFAILED;
    }

    pMediaSVG->m_frameBufferDib = NULL;
    pMediaSVG->m_startPlay = FALSE;
    pMediaSVG->m_isMediaNotifyCbRegistered = FALSE;
    pMediaSVG->m_isFrameCbEnabled = FALSE;
    pMediaSVG->m_isDomEmptyDocEnabled = FALSE;
    pMediaSVG->m_isFramePending = FALSE;
    pMediaSVG->m_autoStopEnable = TRUE;
    pMediaSVG->m_frameDelay = 0;
    pMediaSVG->m_progressiveEnable = FALSE;
    pMediaSVG->m_sentPrepareDone = FALSE;
    pMediaSVG->m_isContentBound = FALSE;
    pMediaSVG->m_numPendingRequiredURLs = 0;
    pMediaSVG->m_enableTransp = TRUE; 
    pMediaSVG->m_transpVal = 0x10000; //transparency has original value of 1 in fixed point

    //Tick Time defaults
    pMediaSVG->m_tickTime           = 1000; //1 sec
    pMediaSVG->m_tickElapsedTime    = 0;
    pMediaSVG->m_TotalPlaybackTime  = (uint32)0x0FFFFFFF;

    //Media Spec Initialization
    pMediaSVG->pMediaSpec           = NULL;

    pMediaSVG->m_pMediaSVGState = MEDIASVG_STATE_IDLE;
    pMediaSVG->m_currDocTime = 0;
    pMediaSVG->m_pfnSVGAnimUpdate = CMediaSVGUpdateAnimation;

    pMediaSVG->m_isClientURLHandling = FALSE;
    pMediaSVG->m_resourceLocation = 0;

    //Get the Display Screen Size & store it
    //the default rect is the Screen Size
    SETAEERECT(&pMediaSVG->screenSize, 0, 0, width, height);
    SETAEERECT(&pMediaSVG->destRect, 0, 0, width, height);

    //Set the IMEDIA Capabilities
    pMediaSVG->dwCaps = 0;
    pMediaSVG->dwCaps |= (MM_CAPS_VIDEO);

    //Initialize dataStructures
    MEMSET(&pMediaSVG->m_AsyncAppNotifyCb, 0, sizeof(pMediaSVG->m_AsyncAppNotifyCb));
    MEMSET(&pMediaSVG->m_appEventQueue, 0, sizeof(pMediaSVG->m_appEventQueue));

    IDISPLAY_GetDeviceBitmap(pMediaSVG->m_pIDisplay, &deviceBmp);
    IBITMAP_GetInfo(deviceBmp, &bi, sizeof(bi));
    SVGALSetDefaultCanvas(*ppalCtx, (unsigned short) bi.cx, (unsigned short) bi.cy);

    pMediaSVG->m_deviceBmp = deviceBmp;

    //set a callback to handle allocation of new frame buffers
    SVGALSetAllocateFrameBufferCb(*ppalCtx, CMediaSVGAllocateFrameBufferCb, pMediaSVG);

    //set a callback to be notified of dynamic URL requests
    SVGALSetDynamicRequestCb(*ppalCtx, CMediaSVGDynamicRequestToProcessCb, pMediaSVG); 
    
    //set a callback to receive when there's a file request. Return the
    //file in a buffer
    SVGALSetFileToBufferCb(*ppalCtx, CMediaSVGFileToBufferCb, pMediaSVG);

    //set callback to receive when there are URL requests to be handled
    SVGALSetURLRequestCb(*ppalCtx, CMediaSVGURLRequestNotifier, pMediaSVG);

    SVGALEnableAlpha(*ppalCtx, pMediaSVG->m_enableTransp);

    return SUCCESS;
}


/*======================================================================


======================================================================*/
uint32 OEMCMediaSVGAddRef(IMedia* pm)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;
	
#ifdef FEATURE_VERSION_K212
	snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_MUTED, SND_MUTE_MUTED, NULL, NULL);	
    snd_set_device(SND_DEVICE_STEREO_HEADSET, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);	
			
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
	
    clk_busy_wait(30*1000);
	
	gpio_tlmm_config(GPIO_OUTPUT_10);
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
	clk_busy_wait(1);
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
	clk_busy_wait(5);
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
	clk_busy_wait(1);
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
	clk_busy_wait(1);
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
	clk_busy_wait(1);
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
	clk_busy_wait(1);
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
#endif

    return (pMediaSVG->m_nRefs++);
}


/*======================================================================


======================================================================*/
uint32 OEMCMediaSVGRelease(IMedia* pm)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;
#ifdef FEATURE_VERSION_K212
   gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
   snd_set_device(SND_DEVICE_STEREO_HEADSET, SND_MUTE_MUTED, SND_MUTE_MUTED, NULL, NULL);	
   snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
#endif

    pMediaSVG->m_nRefs--;

    if(pMediaSVG->m_nRefs != 0)
        return pMediaSVG->m_nRefs;

    //time to clean up everything
    //cancel any updates
    CMediaSVGCancelNextFrame(pMediaSVG);

    SVGALDestroy(pMediaSVG->alCtx);

    //check any queues and free the required data
    {
        CMediaSVGListDestroy(&pMediaSVG->m_requiredURLRequestQueue);
        CMediaSVGListDestroy(&pMediaSVG->m_appEventQueue);
    }

    //MediaSpec DataStructure free
    FREEIF(pMediaSVG->pMediaSpec);

    if(pMediaSVG->m_dom)
    {
        //doesn't matter if DOM object has any references,
        //the MEDIASVG object is going away, so kill the DOM object
#if defined(FEATURE_SVGDOM)
        CSVGDOMFreeDom(pMediaSVG->m_dom);
#endif //defined(FEATURE_SVGDOM)
        pMediaSVG->m_dom = NULL;
    }

    if(pMediaSVG->m_deviceBmp)
    {
        IBITMAP_Release(pMediaSVG->m_deviceBmp);
        pMediaSVG->m_deviceBmp = NULL;
    }

    if(pMediaSVG->m_frameBufferDib)
    {
        IDIB_Release(pMediaSVG->m_frameBufferDib);
        pMediaSVG->m_frameBufferDib = NULL;
    }

    if( pMediaSVG->m_pFileMgr )
        IFILEMGR_Release(pMediaSVG->m_pFileMgr);

    if( pMediaSVG->m_pIDisplay )
        IDISPLAY_Release(pMediaSVG->m_pIDisplay);

    // Release interfaces
    ISHELL_Release(pMediaSVG->m_pIShell);
#ifdef SVG_DYNAMICEXTENSION
    IMODULE_Release(pMediaSVG->m_pIModule);
#endif
    //Free the object itself
    FREE_VTBL(pMediaSVG, IModule);
    FREE( pMediaSVG );

    return SUCCESS;

}


/*======================================================================

//The SVGAL layer calls this function to request a frame of the given width and height.
//This is the frame that the SVG AL layer will draw into. On return, the OEM layer will set the *Actual* values
//of width and height, and return the buffer. Thus, the OEM layer can specify a differnt width and height
//than the one requested by the AL layer. The frame size returned by this function should take into account
//the display size, but is not required too. Note: only one frame is requested at a time, so if a request for
//a frame arrives, then the current frame can be freed (if it exists). isGreaterThanScreen tells if the
//requested frame size has a dimentions greater than the corresponding dimension of the screen size

======================================================================*/
int CMediaSVGAllocateFrameBufferCb(void* userData, unsigned short* width, unsigned short* height, unsigned char bpp,
                                   void** frameBuffer, int* isGreaterThanScreen)
{
    AEEBitmapInfo bi;
    IBitmap* deviceBmp;
    IDIB* frameDib;
    CMediaSVG* pMediaSVG = (CMediaSVG*) userData;
    int ret;
    uint32 widthActual;
    uint32 heightActual;
    boolean bGreater = FALSE;

    if(pMediaSVG == NULL || frameBuffer == NULL || width == NULL || height == NULL || isGreaterThanScreen == NULL )
        return 0;

    if((bpp != 32 && bpp != 16))
        return 0;

    widthActual = *width;
    heightActual = *height;

    if(pMediaSVG->m_frameBufferDib)
    {
        if(pMediaSVG->m_frameBufferDib->cx != widthActual || pMediaSVG->m_frameBufferDib->cy != heightActual
            || pMediaSVG->m_frameBufferDib->nDepth != bpp)
        {
            //Free the currently allocated frame as the newly requested frame differs from the current
            IDIB_Release(pMediaSVG->m_frameBufferDib);
            pMediaSVG->m_frameBufferDib = NULL;

            //now continue and allocat the new frame
        }
        else
        {
            //width/height are already correct, set the frameBuffer output ptr to the current frame
            *isGreaterThanScreen = 0;
            *frameBuffer = pMediaSVG->m_frameBufferDib->pBmp;
            return 1;
        }
    }

    IDISPLAY_GetDeviceBitmap(pMediaSVG->m_pIDisplay, &deviceBmp);
    IBITMAP_GetInfo(deviceBmp, &bi, sizeof(bi));


    //check if the dimensinons are greater than display
    if(widthActual > bi.cx)
    {
        widthActual = bi.cx;
        bGreater = TRUE;
    }

    if(heightActual > bi.cy)
    {
        heightActual = bi.cy;
        bGreater = TRUE;
    }

    IBITMAP_Release(deviceBmp);

    ret = IDISPLAY_CreateDIBitmap(pMediaSVG->m_pIDisplay, &frameDib, bpp, (uint16) widthActual, (uint16) heightActual);
    if(ret != SUCCESS)
    {
        OEMSVG_OUTPUT("OEMSVG Unable to allocate frame buffer");
        return 0;
    }

    if(bpp == 32)
    {
        frameDib->nColorScheme = IDIB_COLORSCHEME_888;
        MEMSET(frameDib->pBmp, 0xFF, widthActual*heightActual*4);
    }
    else if(bpp == 16)
    {
        frameDib->nColorScheme = IDIB_COLORSCHEME_565;
        MEMSET(frameDib->pBmp, 0xFF, widthActual*heightActual*2);
    }


    pMediaSVG->m_frameBufferDib = frameDib;

    if(bGreater)
        *isGreaterThanScreen = 1;
    else
        *isGreaterThanScreen = 0;

    *width = (unsigned short) widthActual;
    *height = (unsigned short) heightActual;
    *frameBuffer = pMediaSVG->m_frameBufferDib->pBmp;

    return 1;
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGRegisterNotify(IMedia* pm, PFNMEDIANOTIFY pfnNotify, void* pUser)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;

    if(pMediaSVG == NULL)
    {
        return EFAILED;
    }

    if(pfnNotify != NULL)
    {
        pMediaSVG->m_aeeMediaNotifyCallback = pfnNotify;
        pMediaSVG->m_aeeMediaNotifyCallbackData = pUser;
        pMediaSVG->m_isMediaNotifyCbRegistered = TRUE;
    }
    else
    {
        pMediaSVG->m_isMediaNotifyCbRegistered = FALSE;
    }

    return SUCCESS;
}

/*======================================================================


======================================================================*/
int    OEMCMediaSVGSetMediaParam(IMedia* pm, int nParamID, int32 param1, int32 param2)
{
    int ret = SUCCESS;
    SVGAL alRet;
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;


    if(pMediaSVG == NULL)
        return EBADPARM;

    //nParamID defines the msg type passes from the AEE layer
    switch(nParamID)
    {

    case MM_PARM_MEDIA_DATA:

        if(pMediaSVG->m_isDomEmptyDocEnabled)
        {
            OEMSVG_OUTPUT("OEMSVG Attemp to set media data wile Dom Empty Doc is enabled");
            return EBADSTATE;
        }
        ret = CMediaSVGHandleSetMediaData(pMediaSVG, param1, param2);
        break;

    case MM_PARM_FRAME:
        {
            pMediaSVG->m_isFrameCbEnabled = (boolean) param1;
            ret = SUCCESS;
        }

        break;

    case MM_PARM_SVG_CLIENT_FRAME_GENERATION:
        ret = CMediaSVGHandleClientFrameGenerationEnable(pMediaSVG, param1, param2);
        SVGALSetParam(pMediaSVG->alCtx, SVGAL_PARAM_CLIENTFRAME, param1, param2);
        break;

    case MM_PARM_SVG_DEFAULT_CANVAS:
    case MM_PARM_SVG_VIEWPORT:
        {

            AEERect* rect = (AEERect*) param1;


            if(!rect || rect->x != 0 || rect->y != 0)
                return EBADPARM;

            if(nParamID == MM_PARM_SVG_DEFAULT_CANVAS)
                alRet = SVGALSetDefaultCanvas(pMediaSVG->alCtx, (unsigned short) rect->dx, (unsigned short) rect->dy);
            else
                alRet = SVGALSetViewport(pMediaSVG->alCtx, (unsigned short) rect->dx, (unsigned short) rect->dy);

            if(alRet == SVGAL_SUCCESS)
                ret = SUCCESS;
            else
                ret = EFAILED;

        }
        break;

    case MM_PARM_SVG_AUTO_STOP:

        //can only set auto stop in ready state
        pMediaSVG->m_autoStopEnable = (boolean) param1;
        SVGALSetParam(pMediaSVG->alCtx, SVGAL_PARAM_AUTOSTOP, param1, param2);
        ret = SUCCESS;

        break;

    case MM_PARM_SVG_PROGRESSIVE_RENDER:

        if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_READY)
        {
            //only set progressive rendering in ready state
            pMediaSVG->m_progressiveEnable = (boolean) param1;
            ret = SUCCESS;
        }
        else
            ret = EBADSTATE;

        break;

    case MM_PARM_SVG_GENERATE_FRAME:
        ret = CMediaSVGHandleGenerateFrame(pMediaSVG, param1, param2);
        break;

    case MM_PARM_SVG_DOM_EMPTY_DOC:
        ret = CMediaSVGHandleDOMCreateEmptyDoc(pMediaSVG, param1, param2);
        break;

    case MM_PARM_SVG_FRAMEDELAY:
        pMediaSVG->m_frameDelay = (uint32)param1;
        ret = SUCCESS;
        break;

    case MM_PARM_SVG_BACKGROUND_COLOR:
        ret = CMediaSVGHandleBkColor(pMediaSVG, param1, param2);
        break;

    case MM_PARM_SVG_ENABLE_TRANSPARENCY:        
        
        if( ((boolean) (param1)) != pMediaSVG->m_enableTransp )
        {
            alRet = SVGALEnableAlpha(pMediaSVG->alCtx, (boolean) param1);
            if(alRet == SVGAL_SUCCESS)
            {
                pMediaSVG->m_enableTransp = (boolean) param1; 
                ret = SUCCESS;                
            }
            else
                ret = EFAILED;
        }
        
        break;

    case MM_PARM_SVG_TRANSPARENCY:

        if( (uint32) param1 > 0x10000)
            return EBADPARM;
        pMediaSVG->m_transpVal = param1;
        SVGALSetParam(pMediaSVG->alCtx, SVGAL_PARAM_TRANSPARENCY_NEWVAL, param1, param2);
        break;

    case MM_PARM_SVG_INPUT:
        ret = CMediaSVGHandleUserInput(pMediaSVG, param1, param2);
        break;

    case MM_PARM_SVG_URIRESOURCE_HANDLED:
        ret = CMediaSVGSetURLHandling(pMediaSVG, param1, (int16)param2);
        SVGALSetParam(pMediaSVG->alCtx, SVGAL_PARAM_URIRESOURCE_HANDLED, param1, param2);
        break;

    case MM_PARM_SVG_URIRESOURCE_DATA:
        ret = CMediaSVGHandleURLReply(pMediaSVG, param1, param2);
        break;

    case MM_PARM_RECT:
        {
            AEERect * newDestRect = (AEERect *)param1; 
            AEERect * screenSize    = &pMediaSVG->screenSize;

            if(NULL == newDestRect)
            {
                ret = EBADPARM;
            }else if((0 > newDestRect->dx) || 
                    (0 > newDestRect->dy) ||
                    (newDestRect->x >= (screenSize->x + screenSize->dx)) ||
                    (newDestRect->y >= (screenSize->y + screenSize->dy)))
            {
                ret = EBADPARM;
            }else
            {
                SETAEERECT(&pMediaSVG->destRect, newDestRect->x, 
                        newDestRect->y, newDestRect->dx, newDestRect->dy);
    
                SVGALSetParam(pMediaSVG->alCtx, SVGAL_PARAM_RECT, param1, param2);
            }
        }
        break;

    case MM_PARM_TICK_TIME:
        {
            uint32 tickTime = (uint32)param1;
            pMediaSVG->m_tickTime = tickTime;                
        }
        break;

    case MM_PARM_VOLUME:    
        ret = EUNSUPPORTED;
        break;

    default:
        ret = EBADPARM;
        break;

    }

    return ret;
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGGetMediaParam(IMedia* pm, int nParamID, int32* pParam1, int32* pParam2)
{
    int ret = SUCCESS;
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;

    if(pMediaSVG == NULL)
        return EBADPARM;

    switch(nParamID)
    {

    case MM_PARM_CLSID:
        {
            if(pParam1 == NULL)
                return EBADPARM;

            *pParam1 = (int32) AEECLSID_MEDIASVG;

            ret = SUCCESS;
        }
        break;

    case MM_PARM_FRAME:
        {
            IBitmap** bmpParamOut = (IBitmap**) pParam1;
            if(bmpParamOut == NULL)
                return EBADPARM;

            *bmpParamOut = IDIB_TO_IBITMAP(pMediaSVG->m_frameBufferDib);
            IBITMAP_AddRef(*bmpParamOut);

            //no more frames pending
            pMediaSVG->m_isFramePending = FALSE;

            ret = SUCCESS;
        }
        break;

    case MM_PARM_SVG_VIEWPORT:
        {
            SVGAL alRet;
            unsigned short width, height;
            AEERect* rect = (AEERect*) pParam1;
            if(!rect)
                return EBADPARM;

            alRet = SVGALGetViewport(pMediaSVG->alCtx, &width, &height);
            if(alRet == SVGAL_SUCCESS)
            {
                rect->x = rect->y = 0;
                rect->dx = width;
                rect->dy = height;
                ret = SUCCESS;
            }
            else
                ret = EFAILED;

        }
        break;

    case MM_PARM_CAPS:
        {
          if(NULL == pParam1)
          {
            ret = EBADPARM;
          }else
          {
            *pParam1 = (int32) (pMediaSVG->dwCaps);
            ret = SUCCESS;
            
            SVGALGetParam(pMediaSVG->alCtx, SVGAL_PARAM_CAPS, pParam1, pParam2);
          }
        }
        break;

    case MM_PARM_RECT:
        {
          AEERect * destRect = (AEERect *) pParam1;

          if(NULL == destRect)
          {
            ret = EBADPARM;
          }else
          {
            SETAEERECT(destRect, pMediaSVG->destRect.x, pMediaSVG->destRect.y, 
                pMediaSVG->destRect.dx, pMediaSVG->destRect.dy);
            ret = SUCCESS;

            SVGALGetParam(pMediaSVG->alCtx, SVGAL_PARAM_RECT, pParam1, pParam2);
          }
        }
        break;

    case MM_SVG_DOCUMENT_VERIFY:
        {
            SVGDocumentVerify* docVerify = (SVGDocumentVerify*) pParam1;

            if(docVerify == NULL)
            {
                ret = EBADPARM;
            }

            ret = OEMCMediaSVGDocumentVerify((IMedia*)pMediaSVG, docVerify);
        }
        break;

    case MM_PARM_FOCUS_INFO:
        {
            SVGFocusInfo * pFocusInfo = (SVGFocusInfo *) pParam1;
            SVGALFocusInfo alFocusInfo;

            if(pFocusInfo == NULL)
            {
                ret = EBADPARM;
            }

            SVGALGetFocusInfo(pMediaSVG->alCtx, &alFocusInfo);
            
            pFocusInfo->bHighlightFocus = alFocusInfo.bHighlightFocus;
            
            pFocusInfo->boundingBox.x1 = alFocusInfo.boundingRegion.x1;
            pFocusInfo->boundingBox.y1 = alFocusInfo.boundingRegion.y1;
            pFocusInfo->boundingBox.x2 = alFocusInfo.boundingRegion.x2;
            pFocusInfo->boundingBox.y2 = alFocusInfo.boundingRegion.y2;
            
            ret = SUCCESS;
        }
        break;
        
    case MM_PARM_VOLUME:
      ret = EUNSUPPORTED;
      break;

    case MM_PARM_SVG_VERSION:
       {
          if(NULL == pParam1)
          {
            ret = EBADPARM;
          }else
          {
            *pParam1 = (int32) SVGT_VERSION_12;
            ret = SUCCESS;
          }
        }
      break;

    default:
        ret = EBADPARM;
        break;

    }

    return ret;
}


/*======================================================================


======================================================================*/
void CMediaSVGDisplayUpdate(CMediaSVG* pMediaSVG)   
{
    //Copying frame only within the Rectangle that is set
    if(IBITMAP_BltIn(pMediaSVG->m_deviceBmp, pMediaSVG->destRect.x, pMediaSVG->destRect.y, 
          OEMSVG_MIN(pMediaSVG->destRect.dx, pMediaSVG->m_frameBufferDib->cx), 
          OEMSVG_MIN(pMediaSVG->destRect.dy, pMediaSVG->m_frameBufferDib->cy),
          IDIB_TO_IBITMAP(pMediaSVG->m_frameBufferDib), 0, 0, AEE_RO_COPY) != SUCCESS)
    {
        OEMSVG_OUTPUT("OEMSVG Failed to update display");
    }

    IDISPLAY_UpdateEx(pMediaSVG->m_pIDisplay, FALSE);
}


/*======================================================================


======================================================================*/
void CMediaSVGHandleTransparency(CMediaSVG* pMediaSVG)
{

    if(pMediaSVG->m_transpVal != 0x10000)   //only scale alpha if scale is not 1
    {
        int width = pMediaSVG->m_frameBufferDib->cx;
        int height = pMediaSVG->m_frameBufferDib->cy;
        unsigned char* ptr = pMediaSVG->m_frameBufferDib->pBmp;
        unsigned char* end = ptr+width*height*4;

        uint32 appAlpha = pMediaSVG->m_transpVal & 0xFFFF;

        if(!pMediaSVG->m_enableTransp)
        {
            //modify the alpha value only
            while(ptr < end)
            {
                ptr[3] = ((unsigned char) ((ptr[3] * appAlpha) / 0x10000));
                ptr += 4;
            }
        }
        else
        {
            //modify the R,G,B components
            while(ptr < end)
            {
                ptr[0] = ((unsigned char) ((ptr[0] * appAlpha) / 0x10000));
                ptr[1] = ((unsigned char) ((ptr[1] * appAlpha) / 0x10000));
                ptr[2] = ((unsigned char) ((ptr[2] * appAlpha) / 0x10000));
                ptr += 4;
            }

        }
    }
}


/*======================================================================


======================================================================*/
void CMediaSVGHandleFrameDone(CMediaSVG* pMediaSVG, boolean redraw)
{
    //check if redraw is required, if yes this will update the
    //frame buffer, but *not* update the display
    if( redraw==TRUE )
    {
        if (SVGALRedraw(pMediaSVG->alCtx) != SVGAL_SUCCESS)
        {
            OEMSVG_OUTPUT("OEMSVG SVGALRedraw() Failed");
            //return EFAILED;
        }
    }

    CMediaSVGHandleTransparency(pMediaSVG);

    //Check if frame callback is enabled. If it is we call the cb, and we don't
    //do any display updates. The app is responsible for getting the frame with
    //IMEDIA_GetFrame() and displaying it.
    if(pMediaSVG->m_isFrameCbEnabled)
    {
        if(pMediaSVG->m_isMediaNotifyCbRegistered)
        {
            //Frame callbacks are enabled and the notify function is registered,
            //let the calling app know we're done a frame.

            //NOTE: we can't render the next frame until the app calls GetFrame in
            //response to the MM_STATUS_FRAME we're about to send here. Otherwise,
            //we'll overwrite the current frame that just completed. So set a
            //flag to indicate this and prevent update animaton function from doing
            //anything until GetFrame has been called for the current frame

            pMediaSVG->m_isFramePending = TRUE;


            CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_FRAME, NULL, 0, FALSE, FALSE);
        }
    }
    else if( redraw == TRUE )
    {
        //frame callback was not enabled, but a redraw is needed, we update the display
        CMediaSVGDisplayUpdate(pMediaSVG);

    }

}

void CMediaSVGUpdateAnimationTime(CMediaSVG * pMediaSVG)
{
    unsigned int msElapsed;
    unsigned int redraw;

    //we can't render the next frame if there is one still pending
    //ie. application hasn't called GetFrame
    if(!pMediaSVG->m_isFramePending)
    {
        if(pMediaSVG->m_startPlay == TRUE)
        {
            //We want msElapsed to be 0 the first time the animation is *started* from it's current
            //position. We're either starting from time 0, or we seeked to another position, and are
            //starting the animation from there, and 0 ms have elapsed since SVGALAdvanceAnimationTime()
            //has been called (this is the first time). We use the m_startPlay flag in the caller to
            //indicate it's the first frame
            pMediaSVG->m_startPlay = FALSE;
            msElapsed = 0;
            pMediaSVG->m_tickElapsedTime = 0;
        }else
        {
            //if not the first frame, calculate the time elapsed since
            //we updated the animation
            msElapsed = GETUPTIMEMS() - pMediaSVG->m_lastAnimUpdateTime;
            //DBGPRINTF("msElapsed is: %d", msElapsed);

        }

        //store the time that we updated the animation
        pMediaSVG->m_lastAnimUpdateTime = GETUPTIMEMS();

        //update the animation
        SVGALAdvanceAnimationTime(pMediaSVG->alCtx, msElapsed, &redraw, ((unsigned int*) &(pMediaSVG->m_currDocTime)));

        pMediaSVG->m_currDocTime += msElapsed;
    }
    
    return;
}


/*======================================================================


======================================================================*/
void CMediaSVGUpdateAnimation(void* pData)
{
    unsigned int msElapsed;
    unsigned int redraw;
    boolean bRedraw;
    SVGALAnimStateData animStateData;
    CMediaSVG* pMediaSVG = (CMediaSVG*) pData;

    //we can't render the next frame if there is one still pending
    //ie. application hasn't called GetFrame
    if(!pMediaSVG->m_isFramePending)
    {
        if(pMediaSVG->m_startPlay == TRUE)
        {
            //We want msElapsed to be 0 the first time the animation is *started* from it's current
            //position. We're either starting from time 0, or we seeked to another position, and are
            //starting the animation from there, and 0 ms have elapsed since SVGALAdvanceAnimationTime()
            //has been called (this is the first time). We use the m_startPlay flag in the caller to
            //indicate it's the first frame
            pMediaSVG->m_startPlay = FALSE;
            msElapsed = 0;
            pMediaSVG->m_tickElapsedTime = 0;
        }
        else
        {
            //if not the first frame, calculate the time elapsed since
            //we updated the animation
            msElapsed = GETUPTIMEMS() - pMediaSVG->m_lastAnimUpdateTime;
            //DBGPRINTF("msElapsed is: %d", msElapsed);

        }

        //store the time that we updated the animation
        pMediaSVG->m_lastAnimUpdateTime = GETUPTIMEMS();

        //update the animation
        SVGALAdvanceAnimationTime(pMediaSVG->alCtx, msElapsed, &redraw, ((unsigned int*) &(pMediaSVG->m_currDocTime)));

        pMediaSVG->m_currDocTime += msElapsed;

        if(redraw == 1)
            bRedraw = TRUE;
        else
            bRedraw = FALSE;

        if((pMediaSVG->m_isMediaNotifyCbRegistered) &&
                (0 != pMediaSVG->m_tickTime))
        {
            pMediaSVG->m_tickElapsedTime += msElapsed;
            if(pMediaSVG->m_tickElapsedTime >= pMediaSVG->m_tickTime)
            {
                CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_TICK_UPDATE, NULL, 0, FALSE, FALSE);
                pMediaSVG->m_tickElapsedTime = 0;
            }
        }
        
        //Frame is done, handle necessary frame updates and callbacks
        CMediaSVGHandleFrameDone(pMediaSVG, bRedraw);      

        //get the new animation state
        SVGALGetAnimationState(pMediaSVG->alCtx, &animStateData);

        if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAYING || pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_READY)
        {
            boolean firstInSequence = FALSE;

            //we include MEDIASVG_STATE_READY above for case that we're in client frame generation
            if(animStateData.state == SVGAL_ANIMSTATE_STOPPED)
            {
                //we find the animation has ended
                if(pMediaSVG->m_autoStopEnable)
                {
                    //autoStop is enabled, time to stop
                    OEMCMediaSVGStop((IMedia*) pMediaSVG);
                    return;
                }
                else
                    firstInSequence = TRUE;
            }

#ifdef SVG_ENABLE_OPTIMIZED_FRAME_DRAW
            if((!pMediaSVG->m_isClientFrameGenEnabled) && ((unsigned int)-1 != animStateData.msNextFrameDelay))
            {  
                
                //continue playing if not in client controlled mode
                CMediaSVGNextFrame(pMediaSVG, firstInSequence, animStateData.msNextFrameDelay);
            }
#else
            if(!pMediaSVG->m_isClientFrameGenEnabled)
            {
                //continue playing if not in client controlled mode
                CMediaSVGNextFrame(pMediaSVG, firstInSequence, animStateData.msNextFrameDelay);
            }
#endif
        }   
    }
}


/*======================================================================


======================================================================*/
int CMediaSVGNextFrame(CMediaSVG* pMediaSVG, boolean isFirstFrameInSequence, uint32 delay)
{
    uint32 frameDelay = pMediaSVG->m_frameDelay;

    pMediaSVG->m_startPlay = isFirstFrameInSequence;

    if((0 == frameDelay) || (delay > frameDelay))
    {
        //render the next frame in "delay" milliseconds
        ISHELL_SetTimer(pMediaSVG->m_pIShell, delay, pMediaSVG->m_pfnSVGAnimUpdate, pMediaSVG); //call the animation update function
    }else
    {
        ISHELL_SetTimer(pMediaSVG->m_pIShell, frameDelay, pMediaSVG->m_pfnSVGAnimUpdate, pMediaSVG); //call the animation update function
    }

    return SUCCESS;
}


/*======================================================================


======================================================================*/
void CMediaSVGCancelNextFrame(CMediaSVG* pMediaSVG)
{
    ISHELL_CancelTimer(pMediaSVG->m_pIShell, pMediaSVG->m_pfnSVGAnimUpdate, pMediaSVG);
}


/*======================================================================


======================================================================*/
int CMediaSVGBindContent(CMediaSVG * pMediaSVG, SVGALMediaSpec * pMediaSpec)
{
    char* filename = "";
    unsigned int bufSize = 0;
    SVGAL ret;
    void* svgDataBuffer = NULL;
    FileInfo svgFileInfo;
    IFile* svgFile = NULL;
    AEEMediaData* md = &pMediaSVG->m_currMediaData;

    if(md == NULL || md->pData == NULL)
    {
        OEMSVG_OUTPUT("OEMSVG CMediaSVGBindContent Null media data");
        return EFAILED;
    }

    if(md->clsData != MMD_FILE_NAME && md->clsData != MMD_BUFFER)
    {
        OEMSVG_OUTPUT("OEMSVG CMediaSVGBindContent Bad clsData");
        return EFAILED;
    }

    if(md->clsData == MMD_FILE_NAME)
    {
        svgFile = IFILEMGR_OpenFile(pMediaSVG->m_pFileMgr, md->pData, _OFM_READ);
        if(svgFile == NULL)
        {
            OEMSVG_OUTPUT("OEMSVG Unable to open file named: %s", md->pData);
            return EFAILED;
        }

        IFILE_GetInfo(svgFile, &svgFileInfo);

        //Here we allocate a buffer for the entire file, but data can be sent in chunks to SVGAL
        svgDataBuffer = MALLOC(svgFileInfo.dwSize);
        if(svgDataBuffer == NULL)
        {
            OEMSVG_OUTPUT("OEMSVG Unable to allocate file chunk buffer of size %d", svgFileInfo.dwSize);
            IFILE_Release(svgFile);
            return EFAILED;
        }

        IFILE_Read(svgFile, svgDataBuffer, svgFileInfo.dwSize);
        filename = md->pData;
        bufSize = svgFileInfo.dwSize;
    }
    else if( md->clsData == MMD_BUFFER)
    {
        filename = "buffer";
        svgDataBuffer = md->pData;
        bufSize = md->dwSize;
    }

    //Initialize DataStructure 
    MEMSET(pMediaSpec, 0, sizeof(SVGALMediaSpec));

    //create a new document
    ret = SVGALCreateNewDocument(pMediaSVG->alCtx, filename, pMediaSpec);
    if(ret == SVGAL_FAIL)
    {
        OEMSVG_OUTPUT("OEMSVG Unable to create new document");
        if(md->clsData == MMD_FILE_NAME)
        {
            if(svgFile)
                IFILE_Release(svgFile);

            if(svgDataBuffer)
                FREE(svgDataBuffer);

        }
        return EFAILED;
    }

    //it's the first and only chunk, and also the last chunk, set 4th param to TRUE
    ret = SVGALSetContentChunk(pMediaSVG->alCtx, svgDataBuffer, bufSize, TRUE, pMediaSVG->m_resourceLocation, pMediaSpec);

    if(md->clsData == MMD_FILE_NAME)
    {
        if(svgFile)
            IFILE_Release(svgFile);

        if(svgDataBuffer)
            FREE(svgDataBuffer);
    }

    if(ret == SVGAL_FAIL)
    {
        OEMSVG_OUTPUT("OEMSVG Failed to set SVG content chunk");
        SVGALDeleteDocument(pMediaSVG->alCtx);
        return EFAILED;
    }

    return SUCCESS;
}


/*======================================================================

======================================================================*/

int CMediaSVGGetMediaSpec(SVGALMediaSpec * pSVGALMediaSpecData, AEEMediaSVGSpec * pMediaSpec)
{
    if((NULL == pSVGALMediaSpecData) || (NULL == pMediaSpec))
    {
        return EFAILED;
    }

    //Struct Size
    pMediaSpec->nStructSize = sizeof(AEEMediaSVGSpec);

    //Color Scheme
    switch(pSVGALMediaSpecData->nColorScheme)
    {
        case SVGAL_MEDIASPEC_COLORSCHEME_RGBA8888:
            pMediaSpec->nColorScheme = MM_SVG_COLOR_SCHEME_RGBA8888;
            break;

        case SVGAL_MEDIASPEC_COLORSCHEME_RGB565:
            pMediaSpec->nColorScheme = MM_SVG_COLOR_SCHEME_RGB565;
            break;

        case SVGAL_MEDIASPEC_COLORSCHEME_UNKNOWN:
        default:
            pMediaSpec->nColorScheme = MM_SVG_COLOR_SCHEME_NONE;
    }

    //Features
    pMediaSpec->flgFeatures = 0;

    if(SVGAL_MEDIASPEC_FEATURE_ANIMATED & pSVGALMediaSpecData->flgFeatures)
    {
        pMediaSpec->flgFeatures |= MM_SVG_SPEC_FEATURE_ANIMATED;
    }

    if(SVGAL_MEDIASPEC_FEATURE_ZOOMPANALLOW & pSVGALMediaSpecData->flgFeatures)
    {
        pMediaSpec->flgFeatures |= MM_SVG_SPEC_FEATURE_ZOOMPANEVENTS;
    }

    if(SVGAL_MEDIASPEC_FEATURE_AUTOSTOP & pSVGALMediaSpecData->flgFeatures)
    {
        pMediaSpec->flgFeatures |= MM_SVG_SPEC_FEATURE_AUTOSTOP;
    }

    //Title
    //TODO: Add a Macro for StrLen
    STRLCPY(pMediaSpec->szTitle, pSVGALMediaSpecData->szTitle, 128);

    //Width & Height
    pMediaSpec->nFrameWidth     = (uint32) pSVGALMediaSpecData->nFrameWidth;
    pMediaSpec->nFrameHeight    = (uint32) pSVGALMediaSpecData->nFrameHeight;

    return SUCCESS;
}

/*======================================================================

======================================================================*/
int OEMCMediaSVGPlay(IMedia* pm)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;

    if(pMediaSVG == NULL)
        return EBADPARM;

    if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_IDLE)
    {
        //caller trying to play without having set content first
        OEMSVG_OUTPUT("OEMSVG Play() called without valid content set");
        return EBADSTATE;
    }
    else if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAYING || pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAY_PAUSE)
    {
        //already playing a file, caller needs to call stop first
        OEMSVG_OUTPUT("OEMSVG Play() called while already in play/pause state; must call stop first");
        return EBADSTATE;
    }
    else if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_READY)
    {
        SVGALMediaSpec SVGALMediaSpecData;

        //Bind content, if not done before (might have got bound in GetTotalTime())
        if(FALSE == pMediaSVG->m_isContentBound)
        {
            //Bind the Content
            if (CMediaSVGBindContent(pMediaSVG, &SVGALMediaSpecData) != SUCCESS)
            {
                return EBADPARM;
            }

            if(NULL == pMediaSVG->pMediaSpec)
            {
                pMediaSVG->pMediaSpec = (AEEMediaSVGSpec *) MALLOC(sizeof(AEEMediaSVGSpec));
            }

            if(NULL == pMediaSVG->pMediaSpec)
            {
                return EBADPARM;
            }

            //Extract The Spec
            CMediaSVGGetMediaSpec(&SVGALMediaSpecData, pMediaSVG->pMediaSpec);

            //Flag that a valid content has been bound
            pMediaSVG->m_isContentBound = TRUE;
        }

        //Inform the Lower Layers that Playback is started
        if(SVGAL_SUCCESS != SVGALPlay(pMediaSVG->alCtx, pMediaSVG->m_isClientFrameGenEnabled))
        {
            OEMSVG_OUTPUT("OEMSVG Play() Engine cannot play!");
            return EFAILED;
        }

        //Reset DocumentTime
        pMediaSVG->m_currDocTime = 0;

        CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_START, NULL, 0, FALSE, FALSE);

        if(!pMediaSVG->m_isClientFrameGenEnabled)
        {
            //only transition to play state if client frame generation is disabled
            pMediaSVG->m_pMediaSVGState = MEDIASVG_STATE_PLAYING;
        }

        if(pMediaSVG->m_numPendingRequiredURLs == 0 || (pMediaSVG->m_progressiveEnable == TRUE) )
        {
            //If there are no required pending requests, we can send the prepare done immediatly,
            //and start animation if necessary. If there are pending requests, then progressive
            //rendering determines if we send prepare done or not

            pMediaSVG->m_sentPrepareDone = TRUE;

            //send PREPARE_DONE to app
            CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_SVG_PREPARE_DONE, NULL, 0, FALSE, FALSE); 

            if(NULL != pMediaSVG->pMediaSpec)
            {
                //Send MEDIA_SPEC to app
                CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_MEDIA_SPEC, 
                        pMediaSVG->pMediaSpec, sizeof(AEEMediaSVGSpec), FALSE, FALSE);
            }

            if(!pMediaSVG->m_isClientFrameGenEnabled)
            {
                //begin the animation if we're not in client controlled mode. Otherwise the application will
                //request the next frame
                CMediaSVGNextFrame(pMediaSVG, TRUE, 0); //start automatic playback
            }
        }
    }else
    {
        OEMSVG_OUTPUT("OEMSVG Play() Unknown state encountered");
        return EBADSTATE;
    }

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGRecord(IMedia* pm)
{

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGStop(IMedia* pm)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;

    if(pMediaSVG == NULL)
        return EBADPARM;

    if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAYING || pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAY_PAUSE)
    {
        //cancel any outstanding frame updates
        CMediaSVGCancelNextFrame(pMediaSVG);

        //back to content active state before we send the MM_STATUS_DONE event, since STOP might be
        //called again. 
        pMediaSVG->m_pMediaSVGState = MEDIASVG_STATE_READY;

        if(NULL != pMediaSVG->pMediaSpec)
        {
            //Reset content chunk, as content with User Event cannot be seeked
            if(0 == (SVGAL_MEDIASPEC_FEATURE_AUTOSTOP & pMediaSVG->pMediaSpec->flgFeatures))
            { 
                pMediaSVG->m_isContentBound = FALSE;
            }else
            {
                //seek file back to beginning
                CMediaSVGSeekToTime(pMediaSVG, MEDIASVG_SEEKMODE_BEGIN, 0);
            }
        }

        //Inform the Lower Layers that Stop is being issued
        SVGALStop(pMediaSVG->alCtx);

        //notify app we're done
        if(pMediaSVG->m_isMediaNotifyCbRegistered)
        {
            CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_DONE, NULL, 0, FALSE, FALSE);
        }       

        return SUCCESS;

    }
    else if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_READY && pMediaSVG->m_isClientFrameGenEnabled)
    {
        //above should be changed so that client frame generation works in its own state. 
        //todo. 

        //we need to handle stop from ready state because client frame generation operates
        //in ready state, and we need to send the status done event to the app notifier

        //cancel any outstanding frame updates
        CMediaSVGCancelNextFrame(pMediaSVG);

        if(NULL != pMediaSVG->pMediaSpec)
        {
            //Reset content chunk, as content with User Event cannot be seeked
            if(0 == (SVGAL_MEDIASPEC_FEATURE_AUTOSTOP & pMediaSVG->pMediaSpec->flgFeatures))
            { 
                pMediaSVG->m_isContentBound = FALSE;
            }else
            {
                //seek file back to beginning
                CMediaSVGSeekToTime(pMediaSVG, MEDIASVG_SEEKMODE_BEGIN, 0);
            }
        }

        //Inform the Lower Layers that Stop is being issued
        SVGALStop(pMediaSVG->alCtx);

        //notify app we're done
        if(pMediaSVG->m_isMediaNotifyCbRegistered)
        {
            CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_DONE, NULL, 0, FALSE, FALSE);
        }

        return SUCCESS;
    }

    return EFAILED;
}


/*======================================================================


======================================================================*/
int CMediaSVGSeekToTime(CMediaSVG* pMediaSVG, MediaSVGSeekMode seekMode, int32 seekMS)
{
    int32 newDocTime;
    int32 curDocTime;
    if(pMediaSVG == NULL)
        return EBADPARM;

    switch(seekMode)
    {

    //for SEEKMODE_BEING, seekMS represents time to seek to from beginning of
    //animation, must be positive
    case MEDIASVG_SEEKMODE_BEGIN:
        if(seekMS < 0)
            newDocTime = 0;                    //seek to time 0
        else
            newDocTime = seekMS;            //seek to time seekMS

        break;

    //for SEEKMODE_CURRENT, seekMS represents time to seek to from current doc time.
    //seekMS can be negative
    case MEDIASVG_SEEKMODE_CURRENT:
        curDocTime = (int32) pMediaSVG->m_currDocTime;    //doc time is always positive
        if(curDocTime + seekMS < 0)
            newDocTime = 0;                            //seek to time 0
        else
            newDocTime = curDocTime + seekMS; //seek to time current+seekMS

        break;

    default:
        OEMSVG_OUTPUT("OEMSVG Unknown seek mode encountered");
        return EBADPARM;
        //break;
    }

    //safety check
    if(newDocTime < 0)
        newDocTime = 0;

    //now seek to the given time
    SVGALSeekToTime(pMediaSVG->alCtx, newDocTime);

    pMediaSVG->m_currDocTime = newDocTime;

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGSeek(IMedia* pm, AEEMediaSeek eSeek, int32 lSeekValue)
{
    MediaSVGSeekMode seekMode;
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;

    if(pMediaSVG == NULL)
        return EBADPARM;

    switch(eSeek)
    {
    case MM_SEEK_START:
        seekMode = MEDIASVG_SEEKMODE_BEGIN;
        break;
    case MM_SEEK_CURRENT:
        seekMode = MEDIASVG_SEEKMODE_CURRENT;
        break;
    case MM_SEEK_END:
    default:
        return EBADPARM;

    }

    //seek to SVG document to the given time. The next time the animation
    //update function is called, the animation will be at the new time.
    //There might currently be a pending timer callback to call the anim
    //update function, OR if client frame generation is enabled, calling
    //GenerateFrame() will call the anim update function.
    CMediaSVGSeekToTime(pMediaSVG, seekMode, lSeekValue);
    pMediaSVG->m_startPlay = TRUE;

    //Is someone trying to seek more than the content time?
    if(pMediaSVG->m_currDocTime > pMediaSVG->m_TotalPlaybackTime)
    {
        CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_SEEK, (void*) pMediaSVG->m_TotalPlaybackTime, sizeof(pMediaSVG->m_TotalPlaybackTime), TRUE, TRUE);
    }else
    {
        CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_SEEK, (void*) pMediaSVG->m_currDocTime, sizeof(pMediaSVG->m_currDocTime), TRUE, TRUE);
    }

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGPause(IMedia* pm)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;

    if(pMediaSVG == NULL)
        return EBADPARM;

    if((pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAYING) ||
            ((pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_READY) && (pMediaSVG->m_isClientFrameGenEnabled)))
    {

        //If Non-Client Controlled Mode
        if(FALSE == pMediaSVG->m_isClientFrameGenEnabled)
        {
            pMediaSVG->m_pMediaSVGState = MEDIASVG_STATE_PLAY_PAUSE;
            
            //we're in the playing state, cancel any outstanding frame updates, so animation stops
            CMediaSVGCancelNextFrame(pMediaSVG);
        }

        //Inform the Lower Layers that Pause is being issued
        SVGALPause(pMediaSVG->alCtx);
        
        //call notify function; as a parameter send the current document time
        CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_PAUSE, (void*) pMediaSVG->m_currDocTime,
                                 sizeof(pMediaSVG->m_currDocTime), TRUE, TRUE);
    }

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGResume(IMedia* pm)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;

    if(pMediaSVG == NULL)
        return EBADPARM;

    //Be it Client Controlled mode or Standalone mode
    if((pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAY_PAUSE) ||
        ((pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_READY) && (pMediaSVG->m_isClientFrameGenEnabled)))
    {

        //Inform the Lower Layers that Resumeis started
        if(SVGAL_SUCCESS != SVGALResume(pMediaSVG->alCtx, pMediaSVG->m_isClientFrameGenEnabled))
        {
            OEMSVG_OUTPUT("OEMSVG Resume() Engine cannot resume!");
            return EFAILED;
        }

        if(pMediaSVG->m_isClientFrameGenEnabled)
        {
            //between pause and resume, client frame generation may have changed, so check it's value here
            //to see if we need to change state back to ready or back to playing
            pMediaSVG->m_pMediaSVGState = MEDIASVG_STATE_READY;
        }
        else
        {
            pMediaSVG->m_pMediaSVGState = MEDIASVG_STATE_PLAYING;

            //we're resuming from a pause call. resume playback
            CMediaSVGNextFrame(pMediaSVG, TRUE, 0);
        }

        //call the notify function
        CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_PLAY, 0, MM_STATUS_RESUME, (void*) pMediaSVG->m_currDocTime,
                                 sizeof(pMediaSVG->m_currDocTime), TRUE, TRUE);
    }

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGGetTotalTime(IMedia* pm)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;
    int32 totalTime = -1;

    if(NULL == pMediaSVG)
    {
        return EBADPARM;
    }

    if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_IDLE)
    {
        OEMSVG_OUTPUT("OEMSVG OEMCMediaSVGGetTotalTime() called while in idle state");
        return EBADSTATE;
    }
    else if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAY_PAUSE)
    {
        OEMSVG_OUTPUT("OEMSVG OEMCMediaSVGGetTotalTime() called while in PlayPause state");
        return EBADSTATE;
    }else if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_READY)
    {
        SVGALMediaSpec SVGALMediaSpecData;

        //Bind the content, if not done before
        if(FALSE == pMediaSVG->m_isContentBound)
        {
            //Bind the Content
            if (CMediaSVGBindContent(pMediaSVG, &SVGALMediaSpecData) != SUCCESS)
            {
                return EBADPARM;
            }

            //MediaSpec is not generated & the ContentBound Flag is not set
            //This is because, GetTotalTime() modifies the animation state of 
            //elements & results in broken animation
            //
            //This is prevented by re-bounding the content with the engine

        }

        SVGALGetTotalTime(pMediaSVG->alCtx, &totalTime);
        if((-1 == totalTime) || (0x0FFFFFFF < totalTime))
        {
            //TODO: Fix this value for indefinite case
            //TODO: USe Macro
            pMediaSVG->m_TotalPlaybackTime = (uint32)0x0FFFFFFF;
        }else
        {
            pMediaSVG->m_TotalPlaybackTime = (uint32)totalTime;
        }

        CMediaSVGCreateNotifyMsg(pMediaSVG, MM_CMD_GETTOTALTIME, 0, MM_STATUS_DONE, 
                (void*) pMediaSVG->m_TotalPlaybackTime, sizeof(pMediaSVG->m_TotalPlaybackTime), TRUE, TRUE);

    }else if(pMediaSVG->m_pMediaSVGState == MEDIASVG_STATE_PLAYING)
    {    
        OEMSVG_OUTPUT("OEMSVG OEMCMediaSVGGetTotalTime() called while in Play state");
        return EBADSTATE;
    }else
    {
        OEMSVG_OUTPUT("OEMSVG OEMCMediaSVGGetTotalTime() called while in Unknown state");
        return EBADSTATE;
    }

    return SUCCESS;
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGGetState(IMedia* pm, boolean* pbStateChanging)
{
    CMediaSVG* pMediaSVG  = (CMediaSVG*) pm;
    int state             = EBADSTATE;

    if((NULL == pMediaSVG) || (NULL == pbStateChanging))
    {
        return (state);
    }

    //The State isn't changing
    *pbStateChanging = FALSE;

    switch(pMediaSVG->m_pMediaSVGState)
    {
        case MEDIASVG_STATE_IDLE:
            state = MM_STATE_IDLE;
            break;

        case MEDIASVG_STATE_READY:
            state = MM_STATE_READY;
            break;

        case MEDIASVG_STATE_PLAYING:
            state = MM_STATE_PLAY;
            break;

        case MEDIASVG_STATE_PLAY_PAUSE:
            state = MM_STATE_PLAY_PAUSE;
            break;

        default:
            state = EBADSTATE;
            break;
    }

    return (state);
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGQueryInterface(IMedia* pm, AEECLSID clsId, void** outInterface)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;
    CSVGDOM* domOut;
    int ret;

    if(!outInterface)
        return EFAILED;

    // this classID toggle JSR226 behavior and W3C behavior. By default,
    // the engine behave as W3C specification.
    if(clsId == AEECLSID_SVGJSR226_1_1)
    {
        if(SVGALSetPreferences(pMediaSVG->alCtx, TRUE) == SVGAL_FAIL)
        {
            *outInterface = NULL;
            OEMSVG_OUTPUT("OEMSVG Unable to verify document");
            return EFAILED;
        }
        
        //Add RefCount
        OEMCMediaSVGAddRef(pm);

        *outInterface = pm;
        return SUCCESS;
    }
    else if(clsId == AEECLSID_SVGDOM || clsId == AEEIID_ISVGDOM)
    {
        //apps wants the DOM interface

        if(pMediaSVG->m_dom != NULL)
        {
            //we only allow a single SVGDOM object per MEDIASVG object,
            //references through AddRef() can be added though
            *outInterface = NULL;
            return EFAILED;
        }

        //Check if a valid content is bound already
        if(!pMediaSVG->m_isContentBound)
        {
            //If not the App didn't Play the content            
            OEMSVG_OUTPUT("OEMSVG QueryInterface() called without playing the content");            
            *outInterface = NULL;
            return EBADSTATE;
        }

#if defined(FEATURE_SVGDOM)
        ret = CSVGDOMInit(pMediaSVG, &domOut );
#else
        domOut = NULL;
        ret = ECLASSNOTSUPPORT;
#endif //defined(FEATURE_SVGDOM)

        if(ret == SUCCESS && domOut != NULL)
        {
            pMediaSVG->m_dom = domOut;
            *outInterface = domOut;

            return SUCCESS;
        }
        else
        {
            *outInterface = NULL;
            return ret;
        }
    }
    else
    {
        *outInterface = NULL;
        return ECLASSNOTSUPPORT;
    }
}


/*======================================================================


======================================================================*/
int OEMCMediaSVGDocumentVerify(IMedia* pm, SVGDocumentVerify* verify)
{
    CMediaSVG* pMediaSVG = (CMediaSVG*) pm;
    SVGALDocVerify docVerify;

    if(!verify)
        return EBADPARM;

    MEMSET(&docVerify, 0, sizeof(docVerify));

    if(SVGALDocumentVerify(pMediaSVG->alCtx, &docVerify) == SVGAL_FAIL)
    {
        OEMSVG_OUTPUT("OEMSVG Unable to verify document");
        return EFAILED;
    }

    verify->flgDocumentVerify = docVerify.flgDocumentVerify;

    return SUCCESS;
}


/*======================================================================


======================================================================*/

#ifdef SVG_DYNAMICEXTENSION
int SVGExtensionNew(int16 nSize, IShell *pIShell, IModule* pIModule, IModule ** ppMod)
#else
int SVGExtensionNew(int16 nSize, IShell *pIShell, IModule ** ppMod)
#endif
{
    CMediaSVG* pMe;
    AEEVTBL(IMedia) *    modFuncs;
    AEEDeviceInfo di;
    int ret = SUCCESS;

    if( !ppMod || !pIShell)
        return EFAILED;

#ifdef SVG_DYNAMICEXTENSION
    if(!pIModule)
        return EFAILED;
#endif

    *ppMod = NULL;

    if( nSize < sizeof(CMediaSVG) )
        nSize += sizeof(CMediaSVG);


    pMe = (CMediaSVG*) MALLOC(nSize + sizeof(AEEVTBL(IMedia)));
    if(pMe == NULL)
        return ENOMEMORY;

    MEMSET(pMe, 0, sizeof(CMediaSVG)+sizeof(AEEVTBL(IMedia)));

    modFuncs = (AEEVTBL(IMedia) *)((byte *)pMe + nSize);

    modFuncs->AddRef =            OEMCMediaSVGAddRef;
    modFuncs->Release =            OEMCMediaSVGRelease;
    modFuncs->QueryInterface =  OEMCMediaSVGQueryInterface;
    modFuncs->RegisterNotify =  OEMCMediaSVGRegisterNotify;
    modFuncs->SetMediaParm =    OEMCMediaSVGSetMediaParam;
    modFuncs->GetMediaParm =    OEMCMediaSVGGetMediaParam;
    modFuncs->Play =            OEMCMediaSVGPlay;
    modFuncs->Record =            OEMCMediaSVGRecord;
    modFuncs->Stop =            OEMCMediaSVGStop;
    modFuncs->Seek =            OEMCMediaSVGSeek;
    modFuncs->Pause =            OEMCMediaSVGPause;
    modFuncs->Resume =            OEMCMediaSVGResume;
    modFuncs->GetTotalTime =    OEMCMediaSVGGetTotalTime;
    modFuncs->GetState =        OEMCMediaSVGGetState;

    //set the vtable for the newly instantiated CMediaSVG class
    INIT_VTBL(pMe, IMedia, *modFuncs);

    // initialize data members
    pMe->m_nRefs      = 1;
    pMe->m_pIShell    = pIShell;
#ifdef SVG_DYNAMICEXTENSION
    pMe->m_pIModule   = pIModule;
#endif

    //add reference to shell and module instances that we're using
    if( ISHELL_AddRef(pIShell) < 1)
        return EFAILED;

#ifdef SVG_DYNAMICEXTENSION
    if( IMODULE_AddRef(pIModule) < 1)
        return EFAILED;
#endif

    //create an IDisplay instance used for drawing to the screen.
    if( ISHELL_CreateInstance(pIShell, AEECLSID_DISPLAY, (void **)&pMe->m_pIDisplay) != SUCCESS )
        return EFAILED;

    ISHELL_GetDeviceInfo(pMe->m_pIShell, &di);

    //pass in width, height of the device and DPI for the SVG player
    ret = OEMSVGInit(pMe, &pMe->alCtx, di.cxScreen, di.cyScreen, OEMSVG_ENGINE_DPI);
    if(ret != SUCCESS)
    {
        return ret;
    }

    // Set the pointer to the newly created class as a return value in the parameter
    *ppMod = (IModule*)pMe;

	

    return SUCCESS;

}


/*======================================================================
//Below function called by ISHELL_CreateInstance. Dynamic and static extensions
//have different definition for the entry point as defined below
======================================================================*/
#ifdef SVG_DYNAMICEXTENSION
int AEEClsCreateInstance(AEECLSID ClsId,IShell* pIShell,IModule* po,void** ppObj)
#else
int IMediaSVG_New(IShell * pIShell, AEECLSID ClsId, void **ppObj)
#endif
{
    *ppObj = NULL;

    if(ClsId == AEECLSID_MEDIASVG )
    {
#ifdef SVG_DYNAMICEXTENSION
        if( SVGExtensionNew(sizeof(CMediaSVG), pIShell, po, (IModule**) ppObj) == SUCCESS)
            return AEE_SUCCESS;
#else
        if( SVGExtensionNew(sizeof(CMediaSVG), pIShell, (IModule**) ppObj) == SUCCESS)
            return AEE_SUCCESS;
#endif
    }

    return EFAILED;
}

/*======================================================================

//this function called once on AEE initialization
======================================================================*/
#ifndef SVG_DYNAMICEXTENSION
void IMediaSVG_Init(IShell * ps)
{
    //Deregister any existing handler for the MIME type then register MEDIASVG
    AEEMedia_Init(ps, MT_VIDEO_SVG, AEECLSID_MEDIASVG);
    AEEMedia_Init(ps, MT_VIDEO_SVGZ, AEECLSID_MEDIASVG);
#if defined(FEATURE_SVGDOM)
    AEEMedia_Init(ps, MT_SVGDOM, AEECLSID_MEDIASVG);
#endif

}
#endif //SVG_DYNAMICEXTENSION




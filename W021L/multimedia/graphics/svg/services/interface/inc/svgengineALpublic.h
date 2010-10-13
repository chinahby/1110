/*=====================================================================

                      S V G

FILE
    svgengineALpublic.h

GENERAL DESCRIPTION
    SVG

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

  $Header: //source/qcom/qct/multimedia/graphics/svg/services/interface/main/latest/inc/svgengineALpublic.h#3 $
  $DateTime: 2009/01/08 04:15:50 $
  $Author: ranjhith $

when       who      what, where, why
--------   ---      --------------------------------------------------------
05/30/06   ae       Initial version of file.

======================================================================*/

#ifndef SVGENGINEAL_PUBLIC_H
#define SVGENGINEAL_PUBLIC_H

/*=====================================================================
                     INCLUDE FILES FOR MODULE
======================================================================*/

//Event Logging
#ifndef SVG_FEATURE_EVENT_LOGGING_DISABLE

#define SVG_FEATURE_EVENT_LOGGING   
#include "event.h"

#endif

/*=====================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
======================================================================*/


#define SVGAL_TOTALTIME_RESOLUTION      1000            // 1s
#define SVGAL_TOTALTIME_TIMELINE_MIN    100             // 100 ms
#define SVGAL_TOTALTIME_TIMELINE_MID    30000           // 30s
#define SVGAL_TOTALTIME_TIMELINE_MAX    60000           // 60s

//Calculate the Absolute Value
#define SVGAL_ABS(x)   ((x) > 0? x: ((x) * -1))

typedef struct SVGALURLRequest
{
    char* urlStr;
    boolean required;
} SVGALURLRequest;


typedef enum SVGALAnimState
{
    SVGAL_ANIMSTATE_PLAYING,
    SVGAL_ANIMSTATE_STOPPED,
    SVGAL_ANIMSTATE_ERROR

} SVGALAnimState;

typedef struct SVGALAnimStateData
{
    SVGALAnimState  state; 
    unsigned int    msNextFrameDelay; 
} SVGALAnimStateData;

typedef struct SVGALMediaSpec
{
    uint16  flgFeatures;        //Content Features
    int16   nColorScheme;       //Color Format
    char    szTitle[128];       //TODO: Put 128 as a Macro
    uint16  nFrameWidth;        //Frame Width
    uint16  nFrameHeight;       //Frame Height
}SVGALMediaSpec;

typedef enum SVGAL
{
    SVGAL_SUCCESS,
    SVGAL_INDEX_SIZE,
    SVGAL_HIERARCHY,
    SVGAL_WRONG_DOC,
    SVGAL_INVALID_MOD,
    SVGAL_INVALID_ACCESS,
    SVGAL_MATRIX_NOT_INVERTABLE,
    SVGAL_MOD_NOT_ALLOWED,
    SVGAL_NOT_EXIST,
    SVGAL_UNSUPPORTED,
    SVGAL_TYPE_MISMATCH,
    SVGAL_BAD_STATE,
    SVGAL_NO_MEM,
    SVGAL_BAD_PARAM,
    SVGAL_FAIL,
    SVGAL_INVALID_VALUE

} SVGAL;


typedef enum SVGALPointerEvent
{
    SVGAL_POINTER_EVENT_MOVE,
    SVGAL_POINTER_EVENT_DOWN,
    SVGAL_POINTER_EVENT_UP,
    SVGAL_POINTER_EVENT_CLICK
} SVGALPointerEvent;

typedef enum SVGALKeyEvent
{
    SVGAL_KEY_EVENT_TEXT_INPUT,
    SVGAL_KEY_EVENT_UP,
    SVGAL_KEY_EVENT_DOWN

} SVGALKeyEvent;


typedef enum SVGALFocusEvent
{
    SVGAL_FOCUS_EVENT_ACTIVATE,
    SVGAL_FOCUS_EVENT_MOVE,
    SVGAL_FOCUS_EVENT_LOCATE

} SVGALFocusEvent;

typedef enum SVGALFocusDirection
{
    SVGAL_FOCUS_EVENT_NEXT,
    SVGAL_FOCUS_EVENT_PREV,
    SVGAL_FOCUS_EVENT_UP,
    SVGAL_FOCUS_EVENT_UP_RIGHT,
    SVGAL_FOCUS_EVENT_RIGHT,
    SVGAL_FOCUS_EVENT_DOWN_RIGHT,
    SVGAL_FOCUS_EVENT_DOWN,
    SVGAL_FOCUS_EVENT_DOWN_LEFT,
    SVGAL_FOCUS_EVENT_LEFT,
    SVGAL_FOCUS_EVENT_UP_LEFT

}SVGALFocusDirection;

typedef enum SVGALParam
{
    SVGAL_PARAM_CLIENTFRAME,
    SVGAL_PARAM_AUTOSTOP,
    SVGAL_PARAM_TRANSPARENCY_NEWVAL,
    SVGAL_PARAM_URIRESOURCE_HANDLED,
    SVGAL_PARAM_RECT,
    SVGAL_PARAM_CAPS
}SVGALParam;

#define SVGAL_DOC_VERIFY_BASE                  0
#define SVGAL_DOC_VERIFY_NO_ERROR              (SVGAL_DOC_VERIFY_BASE)       // No flags
#define SVGAL_DOC_VERIFY_UNKNOWN_OPCODE        (SVGAL_DOC_VERIFY_BASE + 1)   // an element contains unknown attributes.or properties
#define SVGAL_DOC_VERIFY_MISSING_REQ_ATTRI     (SVGAL_DOC_VERIFY_BASE + 2)   // an element is missing a required attribute
#define SVGAL_DOC_VERIFY_CIR_REF               (SVGAL_DOC_VERIFY_BASE + 4)   // the document contians a circular reference
#define SVGAL_DOC_VERIFY_LOCAL_IMAGE_REF       (SVGAL_DOC_VERIFY_BASE + 8)   // the document contains a local image reference.
#define SVGAL_DOC_VERIFY_DUP_ID                (SVGAL_DOC_VERIFY_BASE + 16)  // the document contains elements with a duplicate ID.
#define SVGAL_DOC_VERIFY_ANIMATION_ERROR       (SVGAL_DOC_VERIFY_BASE + 32)  // the document contains an error in animation
#define SVGAL_DOC_VERIFY_UNSUPPORED_VAL        (SVGAL_DOC_VERIFY_BASE + 64)  // the document contains an element with an unsupported value in XML 1.0 specifiction

#define SVGAL_MEDIASPEC_BASE                    0
#define SVGAL_MEDIASPEC_COLORSCHEME_UNKNOWN     (SVGAL_MEDIASPEC_BASE + 1)
#define SVGAL_MEDIASPEC_COLORSCHEME_RGBA8888    (SVGAL_MEDIASPEC_BASE + 2)
#define SVGAL_MEDIASPEC_COLORSCHEME_RGB565      (SVGAL_MEDIASPEC_BASE + 3)

#define SVGAL_MEDIASPEC_FEATURE_BASE            (SVGAL_MEDIASPEC_BASE + 0)
#define SVGAL_MEDIASPEC_FEATURE_SEEKABLE        (SVGAL_MEDIASPEC_FEATURE_BASE + 1)
#define SVGAL_MEDIASPEC_FEATURE_TEXTONLY        (SVGAL_MEDIASPEC_FEATURE_BASE + 2)
#define SVGAL_MEDIASPEC_FEATURE_AUTOSTOP        (SVGAL_MEDIASPEC_FEATURE_BASE + 4)
#define SVGAL_MEDIASPEC_FEATURE_ZOOMPANALLOW    (SVGAL_MEDIASPEC_FEATURE_BASE + 8)
#define SVGAL_MEDIASPEC_FEATURE_ANIMATED        (SVGAL_MEDIASPEC_FEATURE_BASE + 16)
#define SVGAL_MEDIASPEC_FEATURE_USEREVENTS      (SVGAL_MEDIASPEC_FEATURE_BASE + 32)

#ifdef SVG_FEATURE_EVENT_LOGGING

//Event Error Codes
#define SVGAL_EVENT_STATUS_BASE
#define SVGAL_EVENT_STATUS_SUCCESS              0
#define SVGAL_EVENT_STATUS_NULL_INPUT           1
#define SVGAL_EVENT_STATUS_INVALID_INPUT        2
#define SVGAL_EVENT_STATUS_ENGINE_ERROR         3
#define SVGAL_EVENT_STATUS_UNKNOWN_ERROR        4
#define SVGAL_EVENT_STATUS_ILLEGAL_OPERATION    5

//Content Set Types
#define SVGAL_EVENT_CONTENT_SET_INTERMEDIATE    1
#define SVGAL_EVENT_CONTENT_SET_FINAL           2

//Playback Mode
#define SVGAL_EVENT_CONTENT_PLAY_CLIENT         1
#define SVGAL_EVENT_CONTENT_PLAY_STANDALONE     2

//UserEvent Type
#define SVGAL_EVENT_CONTENT_USEREVENT_POINTER   1
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS     2
#define SVGAL_EVENT_CONTENT_USEREVENT_KEYBOARD  3
#define SVGAL_EVENT_CONTENT_USEREVENT_TEXT      4

//Pointer Motion Types
#define SVGAL_EVENT_CONTENT_POINTER_UNKNOWN     0
#define SVGAL_EVENT_CONTENT_POINTER_MOVE        1
#define SVGAL_EVENT_CONTENT_POINTER_CLICK       2

//Transform Types
#define SVGAL_EVENT_CONTENT_TRANSFORM_ZOOM      1
#define SVGAL_EVENT_CONTENT_TRANSFORM_PAN       2
#define SVGAL_EVENT_CONTENT_TRANSFORM_ROTATE    3
#define SVGAL_EVENT_CONTENT_TRANSFORM_RESET     4

//Pan Scroll Type
#define SVGAL_EVENT_CONTENT_PAN_NORMAL          1
#define SVGAL_EVENT_CONTENT_PAN_FAST            2

//Focus Type
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_NONE        1
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_NEXT        2
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_PREVIOUS    3
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_ACTIVATE    4
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_UP          5
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_UP_RIGHT    6
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_RIGHT       7
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_DOWN_RIGHT  8
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_DOWN        9
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_DOWN_LEFT   10
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_LEFT        11
#define SVGAL_EVENT_CONTENT_USEREVENT_FOCUS_UP_LEFT     12

//Get Params
#define SVGAL_EVENT_GET_PARAM_VIEWPORT  1
#define SVGAL_EVENT_GET_PARAM_CAPS      2
#define SVGAL_EVENT_GET_PARAM_RECT      3
#define SVGAL_EVENT_GET_PARAM_DOCVERIFY 4

//Set Params
#define SVGAL_EVENT_SET_PARAM_CLIENTFRAME_ENABLE    1
#define SVGAL_EVENT_SET_PARAM_CLIENTFRAME_DISABLE   2
#define SVGAL_EVENT_SET_PARAM_DEFAULTCANVAS         3
#define SVGAL_EVENT_SET_PARAM_VIEWPORT              4
#define SVGAL_EVENT_SET_PARAM_AUTOSTOP_ENABLE       5
#define SVGAL_EVENT_SET_PARAM_AUTOSTOP_DISABLE      6
#define SVGAL_EVENT_SET_PARAM_BACKGROUNDCOLOR       7
#define SVGAL_EVENT_SET_PARAM_TRANSPARENCY_ENABLE   8
#define SVGAL_EVENT_SET_PARAM_TRANSPARENCY_DISABLE  9
#define SVGAL_EVENT_SET_PARAM_TRANSPARENCY_NEWVAL   10
#define SVGAL_EVENT_SET_PARAM_URIRESOURCE_HANDLED   11
#define SVGAL_EVENT_SET_PARAM_URIRESOURCE_DATA      12
#define SVGAL_EVENT_SET_PARAM_RECT                  13

typedef struct SVGALEventContentSet
{
    uint8 status;
    uint8 contentSetType;
}SVGALEventContentSet;

typedef struct SVGALEventContentPlay
{
    uint8 status;
    uint8 playMode;
}SVGALEventContentPlay;

typedef struct SVGALEventContentResume
{
    uint8 status;
    uint8 playMode;
}SVGALEventContentResume;

typedef struct SVGALEventContentStop
{
    uint8 status;
}SVGALEventContentStop;

typedef struct SVGALEventContentPause
{
    uint8 status;
}SVGALEventContentPause;

typedef struct SVGALEventKeyInfo
{
    uint16  key; 
    int16   virtualKey;
    int16   keyModifier;
    int16   dummy; //TODO: This 16 bits is unnecessary
}SVGALEventKeyInfo;

typedef struct SVGALEventPointerInfo
{
    int16 x; 
    int16 y;
    int16 keyModifier;
    int16 motionType; 
}SVGALEventPointerInfo;

typedef union SVGALEventUserEventInfo
{
    uint16                  focusInfo;
    uint16                  textInfo;
    SVGALEventKeyInfo       keyInfo;
    SVGALEventPointerInfo   pointerInfo;
}SVGALEventUserEventInfo;

typedef struct SVGALEventContentUserEvent
{
    uint8                   status;
    uint8                   eventType;
    SVGALEventUserEventInfo eventInfo;
}SVGALEventContentUserEvent;

typedef struct SVGALEventContentGetURIData
{
    uint8 status;
    uint8 URIType;
    uint8 URIDataType;
}SVGALEventContentGetURIData;

typedef struct SVGALEventPanInfo
{
    int16 x; 
    int16 y;
    int16 scrollMode;
    int16 dummy; //TODO: This 16 bits is unnecessary
}SVGALEventPanInfo;

typedef struct SVGALEventZoomInfo
{
    int16 x; 
    int16 y;
    int32 zoomFactor; // 16.16 format
}SVGALEventZoomInfo; //TODO: No Space to say centered or uncentered

typedef struct SVGALEventRotateInfo
{
    int16 cx; 
    int16 cy;
    int16 angle; 
    int16 dummy; //TODO: This 16 bits is unnecessary
}SVGALEventRotateInfo;

typedef union SVGALEventContentTransformInfo
{
    SVGALEventPanInfo       panInfo;
    SVGALEventZoomInfo      zoomInfo;
    SVGALEventRotateInfo    rotateInfo;
}SVGALEventContentTransformInfo;

typedef struct SVGALEventContentTransform
{
    uint8                           status;
    uint8                           transformType;
    SVGALEventContentTransformInfo  transformInfo;
}SVGALEventContentTransform;

typedef struct SVGALEventGetParam
{
    uint8   status;
    uint16  paramType;
}SVGALEventGetParam;

typedef struct SVGALEventSetParam
{
    uint8   status;
    uint16  paramType;
}SVGALEventSetParam;
#endif //SVG_FEATURE_EVENT_LOGGING

typedef struct SVGALDocVerify
{
    uint16 flgDocumentVerify;
} SVGALDocVerify;


/*=====================================================================
               FUNCTION DECLARATIONS FOR MODULE
======================================================================*/

//width and height of the device screen, dpi of the physical screen
SVGAL SVGALInit(void** ppalCtx, unsigned int width, unsigned int height, unsigned int dpi, boolean bClockUp);

SVGAL SVGALDestroy(void* ctxIn);

typedef int (*OEMSVGAllocateFrameBufferCb) (void* userData, unsigned short* width, unsigned short* height,
                                            unsigned char bpp, void** frameBuffer, int* isGreaterThanScreen);
SVGAL SVGALSetAllocateFrameBufferCb(void* ctxIn, OEMSVGAllocateFrameBufferCb pfnCb, void* pfnCbData);


typedef void (*OEMSVGDynamicRequestCb) (void* userData); 
SVGAL SVGALSetDynamicRequestCb(void* ctxIn, OEMSVGDynamicRequestCb pfnCb, void* pfnCbData);

//set a callback function to be notified when a file needs to be read and converted to a buffer
typedef int (*OEMSVGFileToBufferCb) (void* userData, const char* filenameIn, void** bufferOut, unsigned int* bufSizeOut);
SVGAL SVGALSetFileToBufferCb(void* ctxIn, OEMSVGFileToBufferCb pfnCb, void* pfnCbData);

typedef int (*OEMSVGURLRequestNotifierCb) (void* userData, const SVGALURLRequest* urlArray, int numURLs);
SVGAL SVGALSetURLRequestCb(void* ctxIn, OEMSVGURLRequestNotifierCb pfnCb, void* pfnCbData);

//get current animation state, done playing, etc.
//returns SVGAL_ANIMSTATE_PLAYING if still playing, SVGAL_ANIMSTATE_STOPPED if anim ended, SVGAL_ANIMSTATE_ERROR if error
SVGAL SVGALGetAnimationState(void* ctxIn, SVGALAnimStateData* animStateOut);

//filename of the svg file. (not opened), user responsible for sending chunks of data
SVGAL SVGALCreateNewDocument(void* ctxIn, char* filename, SVGALMediaSpec * pMediaSpec);

//Set a new chunk of file contnet data, lastChunk is set to TRUE if it's last chunk of data, otherwise FALSE
SVGAL SVGALSetContentChunk(void* ctxIn, void* svgBufferData, unsigned int bufSize, boolean lastChunk, short resLocation, SVGALMediaSpec * pMediaSpec);

SVGAL SVGALSetDefaultCanvas(void* ctxIn, unsigned short width, unsigned short height);

SVGAL SVGALSetViewport(void* ctxIn, unsigned short width, unsigned short height);

SVGAL SVGALGetViewport(void* ctxIn, unsigned short *width, unsigned short *height);

SVGAL SVGALGetExternalReferences(void* ctxIn, int16 resLocation); 


//Update the frame buffer. Caller must update the display
SVGAL SVGALRedraw(void* ctxIn);

//Must be called to update the animation state
SVGAL SVGALAdvanceAnimationTime(void* ctxIn, unsigned int msElapsed, unsigned int* redrawNeeded, unsigned int* docTime);

//delete the current docuement created with SVGALCreateNewDocument(). Function will check if a document is
//active before deleting it so caller doesn't need to keep track of this
SVGAL SVGALDeleteDocument(void* ctxIn);

//seek the document to the time given in seekTime. Time is in ms and measured from beginning of file
SVGAL SVGALSeekToTime(void* ctxIn, uint32 seekTime);

//enable disable alpha mode
SVGAL SVGALEnableAlpha(void* ctxIn, boolean enable);

//set the background color
SVGAL SVGALSetBackgroundColor(void* ctxIn, uint32 a, uint32 r, uint32 g, uint32 b);

SVGAL SVGALRotate(void* ctxIn, int cx, int cy, int angle);

SVGAL SVGALHandleKeyEvent(void* ctxIn, SVGALKeyEvent event, unsigned short deviceKey, unsigned int *handledOut);

SVGAL SVGALHandlePointerEvent(void* ctxIn, SVGALPointerEvent event, short x, short y);

SVGAL SVGALHandleFocusEvent(void* ctxIn, SVGALFocusEvent event, int dir);

SVGAL SVGALZoom(void* ctxIn, short x, short y, unsigned int zoomFactor, boolean bCenter);

SVGAL SVGALPan(void* ctxIn, short dx, short dy, boolean fastScroll);

//reset zoom, pan, and scale parameters
SVGAL SVGALReset(void* ctxIn);

SVGAL SVGALHandleURLRequestReply(void* ctxIn, char* URL, void* buffer, unsigned int bufSize);

//document verify
SVGAL SVGALDocumentVerify(void* ctxIn, SVGALDocVerify* docVerifyOut);

SVGAL SVGALSetPreferences(void* ctxIn, boolean isJSR226);

SVGAL SVGALPlay(void* ctxIn, boolean isClientControlled);
SVGAL SVGALPause(void* ctxIn);
SVGAL SVGALStop(void* ctxIn);
SVGAL SVGALResume(void* ctxIn, boolean isClientControlled);
SVGAL SVGALGetParam(void* ctxIn, SVGALParam paramID, int32* pParam1, int32* pParam2);
SVGAL SVGALSetParam(void* ctxIn, SVGALParam paramID, int32 Param1, int32 Param2);
SVGAL SVGALGetTotalTime(void* ctxIn, int32 * pTotalTime);

/***************************************DOM funtions **************************/

/*=====================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
======================================================================*/

//typedef void* SVGALDomObj;
typedef unsigned int SVGALDomObj;

typedef unsigned int SVGALFixed;

typedef struct SVGALRectRegion
{
    int32    x1, y1;
    int32    x2, y2;
} SVGALRectRegion;


typedef struct SVGALFocusInfo
{
   SVGALDomObj *    pFocusElem;
   SVGALRectRegion  boundingRegion;
   boolean          bHighlightFocus;
} SVGALFocusInfo;

typedef struct SVGALPoint
{
    SVGALFixed x;
    SVGALFixed y;

} SVGALPoint;

typedef struct SVGALRect
{
   SVGALFixed    x, y;
   SVGALFixed    dx, dy;
} SVGALRect;

typedef struct SVGALMatrix
{
   SVGALFixed    a, b, c, d, e, f;

} SVGALMatrix;

typedef struct SVGALRGB24
{
    unsigned int r, g, b;
} SVGALRGB24;

typedef struct SVGALDOMString
{
    unsigned short* str;
    int len;
} SVGALDOMString;

typedef unsigned short* SVGALDOMStrBuf;

typedef enum SVGALDOMEventType
{  
    SVGALDOM_EVENT_MOUSE,
    SVGALDOM_EVENT_TEXT,
    SVGALDOM_EVENT_KEYBOARD, 
    SVGALDOM_EVENT_CONNECTION,
    SVGALDOM_EVENT_TIME,     
    SVGALDOM_EVENT_UI,       
    SVGALDOM_EVENT_WHEEL,    
    SVGALDOM_EVENT_PROGRESS 
}SVGALDOMEventType;

/* Mouse Event */
typedef struct SVGALDOMMouseEvent
{
   int32 screenX;
   int32 screenY;
   int32 clientX;
   int32 clientY;
   int32 button;
} SVGALDOMMouseEvent;


/* ProgressEvent */
typedef struct SVGALDOMProgressEvent 
{
   SVGALDOMStrBuf pTypeArgBuf;
   uint32         dwLoaded;
   uint32         dwTotal;
   boolean        bLengthComputable;
} SVGALDOMProgressEvent;


typedef struct SVGALDOMEvent SVGALDOMEvent;

typedef void (*PFNOEMDOMEVENTHANDLER)(void *pUserdata, SVGALDOMEvent * pSVGALDOMEvent);

/* SVGElementInstance */
typedef struct SVGALElementInstance
{
    SVGALDomObj correspondingElement;
    SVGALDomObj correspondingUseElement;
}SVGALElementInstance;

/* SVGEvent */
typedef struct SVGALDOMEvent
{
   void *                pfnSVGALEventHandler;  //Always to be at the top of the structure - Generic
   void *                pEventInfo;            //Event Info
   PFNOEMDOMEVENTHANDLER pfnOEMEventHandler;
   void *                pOEMUserData;

   SVGALDOMEventType     evtType;     
   SVGALDomObj           nTarget;
   SVGALDomObj           nCurrentTarget;   

   SVGALElementInstance* pElementInstanceObj;

   SVGALDOMStrBuf        pType;             // string describing event

   SVGALDOMStrBuf        namespaceURI;
   boolean               cancelable;
   boolean               defaultPrevented;

   union
   {
      SVGALDOMMouseEvent  mouse;        //Mouse Events
      SVGALDOMStrBuf      text;         //Text Events
      SVGALDOMStrBuf      keyId;        //Keyboard Events
      SVGALDOMStrBuf      connection;   //Connection Events   - Not Supported
      int32               nDetail;      //Time & UI Events    - Only Time is Supported
      int32               nWheelDelta;  //Wheel Events        - Not Supported
      SVGALDOMProgressEvent  progress;  //Progress Events     - Not Supported
   } u;   

   void *         alCtx;            //AL Context
} SVGALDOMEvent;

typedef struct SVGALDOMEventListenerContext
{
  PFNOEMDOMEVENTHANDLER pfnOEMEventHandler;
  void *                pOEMUserData;
  SVGALDOMEvent *       pSVGALDOMEventData;
}SVGALDOMEventListenerContext;

/*=====================================================================
               FUNCTION DECLARATIONS FOR MODULE
======================================================================*/

SVGAL SVGALDOMInit(void* alCtx);

/* Node Interface */

SVGAL SVGALDOM_Node_GetOwnerDocument(void* ctxIn, SVGALDomObj thisElem, SVGALDomObj* ownerOut);
SVGAL SVGALDOM_Node_AppendChild(void* ctxIn, SVGALDomObj thisElem, SVGALDomObj childElem);
SVGAL SVGALDOM_Node_GetParent(void* ctxIn, SVGALDomObj thisElem, SVGALDomObj* parentOut);
SVGAL SVGALDOM_Node_RemoveChild(void* ctxIn, SVGALDomObj thisElem, SVGALDomObj oldChildElem);
SVGAL SVGALDOM_Node_GetNamespaceURI(void* ctxIn, SVGALDomObj thisElem, SVGALDOMString* namespaceOut);
SVGAL SVGALDOM_Node_GetLocalName(void* ctxIn, SVGALDomObj thisElem, SVGALDOMString* localnameOut);
SVGAL SVGALDOM_Node_GetContent(void* ctxIn, SVGALDomObj thisElem, SVGALDOMString* contentOut);
SVGAL SVGALDOM_Node_SetContent(void* ctxIn, SVGALDomObj thisElem, SVGALDOMString* contentIn);
SVGAL SVGALDOM_Node_InsertBefore(void* ctxIn, SVGALDomObj thisElem, SVGALDomObj newChildElem, SVGALDomObj refChildElem);

/* Element Interface */
SVGAL SVGALDOM_Element_GetAttributeNS(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString *namespaceURIIn, SVGALDOMString *nameIn, SVGALDOMString *valueOut);
SVGAL SVGALDOM_Element_SetAttributeNS(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString *namespaceURIIn, SVGALDOMString *nameIn, SVGALDOMString *valueIn);

/* Document Interface */

SVGAL SVGALDOM_Document_CreateElementNS(void* ctxIn, SVGALDOMString* namespaceURI, SVGALDOMString* qualifiedName,
                                        SVGALDomObj* elemOut);
SVGAL SVGALDOM_Document_GetDocumentElement(void* ctxIn, SVGALDomObj* elemOut);
SVGAL SVGALDOM_Document_GetElementById(void* ctxIn, SVGALDOMString* elemId, SVGALDomObj* elemOut);
SVGAL SVGALDOM_Document_DeleteElement(void* ctxIn, SVGALDomObj elem);


/* EventTarget Interface */


/* Event Interface */


/* MouseEvent Interface */


/* TextEvent Interface */


/* Keyboard Event Interface */


/* TimeEvent Interface */


/* ElementTimeControl Interface */

SVGAL SVGALDOM_ElementTimeControl_beginElement(void* ctxIn, SVGALDomObj pElement);
SVGAL SVGALDOM_ElementTimeControl_beginElementAt(void* ctxIn, SVGALDomObj pElement, uint32 offset);
SVGAL SVGALDOM_ElementTimeControl_endElement(void* ctxIn, SVGALDomObj pElement);
SVGAL SVGALDOM_ElementTimeControl_endElementAt(void* ctxIn, SVGALDomObj pElement, uint32 offset);


/* SVGSVGElement Interface */

SVGAL SVGALDOM_SVGSVGElement_CreateSVGMatrix(void* ctx, SVGALDomObj thisElem, SVGALMatrix* matIn, SVGALMatrix** matOut);
SVGAL SVGALDOM_FreeSVGMatrix(void* ctx, SVGALDomObj thisElem, SVGALMatrix* mat);
SVGAL SVGALDOM_SVGSVGElement_CreateSVGRect(void* ctx, SVGALDomObj thisElem, SVGALRect** rectOut );
//use naming SVALDOM_FreeSVGRect because not part of W3C spec
SVGAL SVGALDOM_FreeSVGRect(void* ctx, SVGALDomObj thisElem, SVGALRect* rect);
SVGAL SVGALDOM_SVGSVGElement_CreatePath(void* ctxIn, SVGALDomObj elemThis, SVGALDomObj* pathOut);
SVGAL SVGALDOM_FreeSVGPath(void* ctx, SVGALDomObj thisElem, SVGALDomObj path);
SVGAL SVGALDOM_SVGSVGElement_CreateSVGRGBColor(void* ctxIn, SVGALDomObj elemThis, SVGALRGB24* cIn, SVGALRGB24** colorOut);
SVGAL SVGALDOM_FreeSVGRGBColor(void* ctxIn, SVGALDomObj elemThis,  SVGALRGB24* color);
SVGAL SVGALDOM_FreeSVGPoint(void* ctx, SVGALDomObj thisElem, SVGALPoint* point);
//eleThis is the document SVGSVGElement, can use GetDocumentElement
SVGAL SVGALDOM_SVGSVGElement_SetFocus(void* ctxIn, SVGALDomObj svgsvgElem, SVGALDomObj focusNode);
SVGAL SVGALDOM_SVGSVGElement_MoveFocus(void* ctxIn, SVGALDomObj svgsvgElem,  SVGALFocusDirection dir);
SVGAL SVGALDOM_SVGSVGElement_FocusOn(void* ctxIn, SVGALDomObj svgsvgElem, SVGALDomObj focusNode);
SVGAL SVGALDOM_SVGSVGElement_GetCurrentFocusedObject(void* ctxIn, SVGALDomObj elemThis, SVGALDomObj* focusElemOut);
SVGAL SVGALDOM_SVGSVGElement_GetCurrentScale(void* ctx, SVGALDomObj thisElem, SVGALFixed* scaleOut);
SVGAL SVGALDOM_SVGSVGElement_SetCurrentScale(void* ctx, SVGALDomObj thisElem, SVGALFixed scale);
SVGAL SVGALDOM_SVGSVGElement_GetCurrentRotate(void* ctx, SVGALDomObj thisElem, SVGALFixed* rotOut);
SVGAL SVGALDOM_SVGSVGElement_SetCurrentRotate(void* ctx, SVGALDomObj thisElem, SVGALFixed rotate);
SVGAL SVGALDOM_SVGSVGElement_GetCurrentTranslate(void* ctx, SVGALDomObj thisElem, SVGALPoint** transOut);
SVGAL SVGALDOM_SVGSVGElement_SetCurrentTranslate(void* ctx, SVGALDomObj thisElem, SVGALPoint* translate);
SVGAL SVGALDOM_SVGSVGElement_GetViewport(void* ctx, SVGALDomObj thisElem, SVGALRect** rectOut);
SVGAL SVGALDOM_SVGSVGElement_SetViewport(void* ctx, SVGALDomObj thisElem, SVGALRect* rect);
SVGAL SVGALDOM_SVGSVGElement_GetCurrentTime(void* ctx, SVGALDomObj thisElem, SVGALFixed* timeOut);
SVGAL SVGALDOM_SVGSVGElement_SetCurrentTime(void* ctx, SVGALDomObj thisElem, SVGALFixed time);


/* SVGPath Interface */
SVGAL SVGALDOM_SVGPath_GetNumberOfSegments(void* ctxIn, SVGALDomObj pathThis, unsigned int* segOut);
SVGAL SVGALDOM_SVGPath_GetSegment(void* ctxIn, SVGALDomObj pathThis, unsigned int index, SVGALDomObj* segOut);
SVGAL SVGALDOM_SVGPath_GetSegmentParam(void* ctxIn, SVGALDomObj pathThis, unsigned int cmdIndex, unsigned int paramIndex,
                                    SVGALFixed* paramOut);
SVGAL SVGALDOM_SVGPath_MoveTo(void* ctxIn, SVGALDomObj pathThis, SVGALFixed x1, SVGALFixed y1);
SVGAL SVGALDOM_SVGPath_LineTo(void* ctxIn, SVGALDomObj pathThis, SVGALFixed x1, SVGALFixed y1);
SVGAL SVGALDOM_SVGPath_QuadTo(void* ctxIn, SVGALDomObj pathThis, SVGALFixed x1, SVGALFixed y1, SVGALFixed x2, SVGALFixed y2);
SVGAL SVGALDOM_SVGPath_CurveTo(void* ctxIn, SVGALDomObj pathThis, SVGALFixed x1, SVGALFixed y1, SVGALFixed x2, SVGALFixed y2,
                                SVGALFixed x3, SVGALFixed y3);
SVGAL SVGALDOM_SVGPath_Close(void* ctxIn, SVGALDomObj pathThis);


/* SVGMatrix Interface */

SVGAL SVGALDOM_SVGMatrix_getComponent(void* ctxIn, SVGALDomObj elemIn, SVGALMatrix * matrixIn, unsigned int indexIn, SVGALFixed *valueOut);
SVGAL SVGALDOM_SVGMatrix_multiply(void* ctxIn, SVGALDomObj elemIn, SVGALMatrix * matrixInOutA, SVGALMatrix * matrixInB);
SVGAL SVGALDOM_SVGMatrix_inverse(void* ctxIn, SVGALDomObj elemIn, SVGALMatrix * matrixInOut);
SVGAL SVGALDOM_SVGMatrix_translate(void* ctxIn, SVGALDomObj elemIn, SVGALMatrix * matrixInOut, SVGALFixed x, SVGALFixed y);
SVGAL SVGALDOM_SVGMatrix_scale(void* ctxIn, SVGALDomObj elemIn, SVGALMatrix * matrixInOut, SVGALFixed scaleFactor);
SVGAL SVGALDOM_SVGMatrix_rotate(void* ctxIn, SVGALDomObj elemIn, SVGALMatrix * matrixInOut, SVGALFixed angle);

/* SVGLocatable Interface */

SVGAL SVGALDOM_SVGLocatable_getBBox(void* ctxIn, SVGALDomObj elemIn, SVGALRect** valueOut);
SVGAL SVGALDOM_SVGLocatable_getScreenCTM(void* ctxIn, SVGALDomObj elemIn, SVGALMatrix** valueOut);
SVGAL SVGALDOM_SVGLocatable_getScreenBBox(void* ctxIn, SVGALDomObj elemIn, SVGALRect** valueOut);

/* TraitAccess Interface */

SVGAL SVGALDOM_TraitAccess_getTraitNS(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* namespaceURIIn, SVGALDOMString* nameIn, SVGALDOMString* valueOut);
SVGAL SVGALDOM_TraitAccess_getTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALDOMString* valueOut);
SVGAL SVGALDOM_TraitAccess_getFloatTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALFixed* valueOut);
SVGAL SVGALDOM_TraitAccess_getMatrixTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALMatrix** valueOut);
SVGAL SVGALDOM_TraitAccess_getRectTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALRect** valueOut);
SVGAL SVGALDOM_TraitAccess_getPathTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALDomObj* valueOut);
SVGAL SVGALDOM_TraitAccess_getRGBColorTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALRGB24** valueOut);
SVGAL SVGALDOM_TraitAccess_getPresentationTraitNS(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* namespaceURIIn, SVGALDOMString* nameIn, SVGALDOMString* valueOut);
SVGAL SVGALDOM_TraitAccess_getPresentationTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALDOMString* valueOut);
SVGAL SVGALDOM_TraitAccess_getFloatPresentationTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALFixed* valueOut);
SVGAL SVGALDOM_TraitAccess_getMatrixPresentationTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALMatrix** valueOut);
SVGAL SVGALDOM_TraitAccess_getRectPresentationTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALRect** valueOut);
SVGAL SVGALDOM_TraitAccess_getPathPresentationTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALDomObj* valueOut);
SVGAL SVGALDOM_TraitAccess_getRGBColorPresentationTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* nameIn, SVGALRGB24** valueOut);

SVGAL SVGALDOM_TraitAccess_setTraitNS(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* namespaceURIIn, SVGALDOMString* nameIn, SVGALDOMString* valueIn);
SVGAL SVGALDOM_TraitAccess_setTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* name, SVGALDOMString* value);
SVGAL SVGALDOM_TraitAccess_setFloatTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* name, SVGALFixed value);
SVGAL SVGALDOM_TraitAccess_setMatrixTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* name, SVGALDomObj value);
SVGAL SVGALDOM_TraitAccess_setRectTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* name, SVGALRect value);
SVGAL SVGALDOM_TraitAccess_setPathTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* name, SVGALDomObj value);
SVGAL SVGALDOM_TraitAccess_setRGBColorTrait(void* ctxIn, SVGALDomObj elemIn, SVGALDOMString* name, SVGALRGB24 value);


/* ElementTraversal Interface */

SVGAL SVGALDOM_ElementTraversal_GetFirstElementChild (void* ctxIn, SVGALDomObj elemIn, SVGALDomObj* elemOut);
SVGAL SVGALDOM_ElementTraversal_GetLastElementChild(void* ctxIn, SVGALDomObj elemIn, SVGALDomObj* elemOut);
SVGAL SVGALDOM_ElementTraversal_GetPreviousElementSibling(void* ctxIn, SVGALDomObj elemIn, SVGALDomObj* elemOut);
SVGAL SVGALDOM_ElementTraversal_GetNextElementSibling(void* ctxIn, SVGALDomObj elemIn, SVGALDomObj* elemOut);
//SVGAL SVGALDOMAppendChild(void* ctxIn, SVGALDomObj nodeIn, SVGALDomObj nodeNewChild);


/* SVGElement Interface*/
SVGAL SVGALDOM_SVGElement_GetId(void* ctxIn, SVGALDomObj thisElem, SVGALDOMString* strOut);
SVGAL SVGALDOM_SVGElement_SetId(void* ctxIn, SVGALDomObj thisElem, SVGALDOMString* strId);

/* SVGEventListener Interface */

SVGAL SVGALDOM_AddEventListener(
    void *                          ctxIn,
    SVGALDomObj                     thisElem,
    SVGALDOMString *                pEventType,
    SVGALDOMEventListenerContext *  pListenerContext,
    boolean                         useCapture);

SVGAL SVGALDOM_RemoveEventListener(
    void *                          ctxIn,
    SVGALDomObj                     thisElem,
    SVGALDOMString *                pEventType,
    SVGALDOMEventListenerContext *  pListenerContext,
    boolean                         useCapture);

//Frees memory created by AMSS
SVGAL SVGALFreeMem(void * pFree);

//Get Focus Info
SVGAL SVGALGetFocusInfo(void * ctxIn, SVGALFocusInfo * pFocusInfo);

/* SVGGloabel Interface */

#endif //SVGENGINEAL_PUBLIC_H



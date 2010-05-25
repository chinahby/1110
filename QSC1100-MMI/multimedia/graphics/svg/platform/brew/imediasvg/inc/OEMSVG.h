/*=====================================================================

              B R E W   S V G   S E R V I C E S

FILE
    OEMSVG.h

GENERAL DESCRIPTION
   This file defines Scalable Vector Graphics (SVG) services.

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

  $Header: //source/qcom/qct/multimedia/graphics/svg/platform/brew/imediasvg/main/latest/inc/OEMSVG.h#4 $
  $DateTime: 2008/08/01 03:22:21 $
  $Author: ranjhith $

when       who      what, where, why
--------   ---      --------------------------------------------------------
05/30/06   ae       Initial version of file.
09/19/06   rk       Added Min(), Max() macro functions & AEErect in 
                      CMediaSVG DataStructure
09/20/06   rk       Added dwCaps in CMediaSVG Structure
======================================================================*/

#ifndef OEMSVG_H
#define OEMSVG_H

/*=====================================================================
                     INCLUDE FILES FOR MODULE
======================================================================*/

#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEMedia.h"
#include "AEESVGDOM.BID"
#include "AEEISVGDOM.h"
#include "AEEIMEDIASVG.h"
#include "svgengineALpublic.h"

/*=====================================================================
               FUNCTION DECLARATIONS FOR MODULE
======================================================================*/

int CMediaSVGALErrorToApp(SVGAL error);

/*=====================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
======================================================================*/

#define OEMSVG_OUTPUT DBGPRINTF

#define OEMSVG_MIN(a, b) ((a) > (b) ? (b) : (a))
#define OEMSVG_MAX(a, b) ((a) > (b) ? (a) : (b))

typedef enum MediaSVGState
{
    MEDIASVG_STATE_IDLE,
    MEDIASVG_STATE_READY,
    MEDIASVG_STATE_PLAYING,
    MEDIASVG_STATE_PLAY_PAUSE

} MediaSVGState;

typedef enum MediaSVGSeekMode
{
    MEDIASVG_SEEKMODE_BEGIN,     //seek from the beginning
    MEDIASVG_SEEKMODE_CURRENT    //seek from current position

} MediaSVGSeekMode;

typedef struct _CMediaSVG CMediaSVG;
typedef struct _CSVGDOM CSVGDOM;
typedef struct _CSVGDOMModel CSVGDOMModel;

typedef struct CMediaSVGListNode CMediaSVGListNode;
typedef struct CMediaSVGListNode
{
    void* data;
    CMediaSVGListNode* next;

} CMediaSVGListNode;

typedef struct CMediaSVGList
{
    CMediaSVGListNode* head;
    CMediaSVGListNode* tail;
}CMediaSVGList;


//define the concrete class for the media svg instance
typedef struct _CMediaSVG
{
    // Declare our VTable
    // by doing so, we have placed function pointers
    // in the begininng of this structure
    DECLARE_VTBL(IMedia)


    // Class member variables
    uint32          m_nRefs;
    IShell*         m_pIShell;
    IDisplay*       m_pIDisplay;
#ifdef SVG_DYNAMICEXTENSION
    IModule *       m_pIModule;
#endif
    IFileMgr*       m_pFileMgr;
    IBitmap*        m_deviceBmp;
    IDIB*           m_frameBufferDib;
    uint16          m_frameBufferWidth;
    uint16          m_frameBufferHeight;

    AEEMediaData    m_currMediaData;

    boolean         m_startPlay;
    boolean         m_repeatLastFrame;
    uint32          m_lastAnimUpdateTime;

    boolean         m_isFrameCbEnabled;

    boolean         m_isMediaNotifyCbRegistered;
    PFNMEDIANOTIFY  m_aeeMediaNotifyCallback;
    void*           m_aeeMediaNotifyCallbackData;

    MediaSVGState   m_pMediaSVGState;

    boolean         m_isClientFrameGenEnabled;
    uint32          m_currDocTime;

    boolean         m_autoStopEnable;
    boolean         m_progressiveEnable;

    boolean         m_isDomEmptyDocEnabled;

    boolean         m_isFramePending;

    uint32          m_frameDelay;

    PFNNOTIFY       m_pfnSVGAnimUpdate;

    boolean         m_isClientURLHandling;
    int16           m_resourceLocation;

    boolean         m_isContentBound;

    boolean         m_sentPrepareDone;

    CMediaSVGList   m_appEventQueue;

    CMediaSVGList   m_requiredURLRequestQueue;
    uint32          m_numPendingRequiredURLs;

    AEECallback     m_AsyncAppNotifyCb;

    boolean         m_enableTransp;
    uint32          m_transpVal;

    void*           alCtx; // SVG abstraction layer context pointer

    CSVGDOM*        m_dom;

    AEERect         destRect;     // Used when the engine directly renders into the display
    AEERect         screenSize;   // Display Screen Size
    uint32          dwCaps;       // Only Video Capability

    uint32          m_tickTime;
    uint32          m_tickElapsedTime;

    uint32          m_TotalPlaybackTime; //Content Time

    AEEMediaSVGSpec * pMediaSpec; // For MediaSpec Data
} CMediaSVG;


typedef enum SVGDOMLiveReferenceType
{
    SVGDOM_LIVE_REF_PATH,
    SVGDOM_LIVE_REF_MATRIX,
    SVGDOM_LIVE_REF_COLOR,
    SVGDOM_LIVE_REF_RECT
} SVGDOMLiveReferenceType;


typedef struct SVGDOMLiveReference SVGDOMLiveReference;
typedef struct SVGDOMLiveReference
{
    SVGDOMLiveReferenceType     type;
    AEEDOMNodeID                nodeID;
    void                        *ptr;
    SVGDOMLiveReference         *pNext;
} SVGDOMLiveReference;

typedef struct SVGDOMLiveElement SVGDOMLiveElement;
typedef struct SVGDOMLiveElement
{
    AEEDOMNodeID                ElementID;
    SVGDOMLiveElement           *pNext;
}SVGDOMLiveElement;

typedef struct SVGDOMEventContext
{
    PFNDOMEVENTHANDLER          pfnAppEventHandler; //APP Handler
    void *                      pAppUserData;       //User Data
    SVGEvent                    eventAppData;       //Event Data for App  

    AEEDOMString                EventTypeStrBuf;
    AEEDOMString                EventStrBuf;
    AEEDOMString                NameSpaceURIStrBuf;

    AEEDOMNodeID                NodeID;
    AEEDOMString                EventType;
    boolean                     useCapture;

    SVGALDOMEvent               eventOEMData;
}SVGDOMEventContext;


typedef struct SVGDOMLiveEventContext SVGDOMLiveEventContext;
typedef struct SVGDOMLiveEventContext
{
    SVGDOMEventContext *        pEventContext;
    SVGDOMLiveEventContext *    pNext;
}SVGDOMLiveEventContext;

typedef struct _CSVGDOM
{
    AEEVTBL(ISVGDOM) *          pvt;

    // Class member variables
    uint32                      m_nRefs;
    CMediaSVG*                  svg;       //pointer to the owning MEDIASVG instance
    CSVGDOMModel*               modelList; //List of active models to which events can be delivered

    SVGDOMLiveReference*        liveRefs;
    SVGDOMLiveElement*          liveElements;       //list of live Elements
    SVGDOMLiveEventContext*     liveEventContexts;  //list of live Event Contexts

    void*                       alCtx;
} CSVGDOM;


typedef struct _CSVGDOMModel
{
   AEEVTBL ( IModel ) *         pvt;
   uint32                       m_nRefs;
   //ACONTEXT *                 appCtx;
   CSVGDOM *                    pOwner;        // who created me
   AEEDOMNodeID                 node;          // Node to which this model is attached
   
   ModelListener *              pListeners;    // list of application event listeners
   CSVGDOMModel*                next;

} CSVGDOMModel;

int  CMediaSVGNextFrame(CMediaSVG* pMediaSVG, boolean isFirstFrameInSequence, uint32 delay);

#endif //OEMSVG_H



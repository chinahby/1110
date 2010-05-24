#ifndef _QTVPLAYER_DISPATCHAPI_H_
#define _QTVPLAYER_DISPATCHAPI_H_

/* =======================================================================
                               QtvPlayerDispatch.h
DESCRIPTION
 Dispatch API for QTV / REAL contents when DRM_DCF is enabled.

Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History
$Header: //source/qcom/qct/multimedia/qtv/utils/dispatch/main/latest/inc/qtvplayerdispatch.h#7 $
$DateTime: 2008/11/24 20:32:49 $
$Change: 791893 $
========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "QtvPlayer.h"
#include "Mpeg4Player.h"
#include "qcplayer.h"
#include "Events.h"
#include "comdef.h"

#ifdef FEATURE_QTV_DRM_DCF

  /*
  * Include required DRM suite header files.
  */
  #include "IxStream.h"
  #include "IxStreamFactory.h"
  #include "IxCallbackDef.h"
  #include "IxErrno.h"
  #include "IxContentDef.h"

/*
* As per DRM folks, ContentBrowserClient and anything else
* to do with installation of context will not be available in future.
* Moving all of the code related to this under FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
* In future, if there is an alternative, need to modify accordingly.
* FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT should not be defined[always turned off]
*/

#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
  #include "IxContentMgrClient.h"
  #include "IxContentInfo.h"
  #include "IxContentAttrDef.h"
  #include "IxContent.h"
  #include "IxContentClient.h"
  #include "IxContentBrowserClient.h"
#endif//FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT

  #include "rex.h"
  #include "zrex.h"

  #define               IxStreamCallbackArrayLength 3
  #define               VIDEO_INDEX 0
  #define               AUDIO_INDEX 1
  #define               TEXT_INDEX  2

  typedef void* QtvPlayerDispatchMessageHandleType;

  typedef zrex_event_type QTVDispatchMessageType;

  class QtvPlayerDispatch
  {
  public:


    /*
    * QtvPlayerDispatch class constructor.
    */
    QtvPlayerDispatch(QtvPlayer *player);

    /*
    * QtvPlayerDispatch class destructor.
    */
    ~QtvPlayerDispatch();

    /*
    * Gets the Mpeg4Player*(Mpeg4Player handle)to post message/event later when content Mime Type is
    * QTV_PLAYER_MIME_TYPE.
    */
    void SetMpeg4PlayerHandle(Mpeg4Player*);

    /*
    * DRM/DCF is applicable only for local file playback. Following routine checks file extensions
    * as we want to bypass DRM/DCF suite when filename passed in has one of the following extensions.
    * a) .url|.URL,
    * b) .pvx|.PVX,
    * c) .sdp|.SDP,
    * d) .flo|.FLO
    *
    * Returns true if extension is not one of the above, otherwise, returns false;
    */
    static bool isLocalFileMedia(const char*);

    /*
    * Checks for (.DCF|.dcf) extension.
    * Returns true if file has (.dcf|.DCF) extension, otherwise, returns false;
    */
     static bool IsDrmDcfMedia(const char*);

    void NotifyCallback( QtvPlayer::PlayerStatusT status,    /* player status code */
                         void *      pClientData,
                         QtvPlayer::PlayerStateRecordT state /* current player state. */
                       );
    /*
    * Handler for QTV_IXSTREAM_OPEN_EVENT event.
    */
    static void QTV_IXSTREAM_OPEN_EVENT_handler(QtvPlayerDispatchMessageHandleType handle);

    /*
    *
    * Handler for PV_OPEN_CONTENT_URN event. Event contains filename for audio/video/text.
    * Filename could be protected (.dcf) or unprotected content.
    * Routine will create single IxStream and then will determine whether we need reference IxStreams based
    * on file name(s) passed in.
    *
    * This will make asynchronous calls into DRM and eventually, IPC signal
    * will be set for this TCB by DRM suite. This IPC signal will be handled in 'QtvDispatchThread' routine,which
    * will help to invoke the callbacks provided in asynchronous calls.
    *
    */
    static void  PV_OPEN_CONTENT_URN_handler(QtvPlayerDispatchMessageHandleType handle);

  // Removed RVCT 2.2 update protected:

    /*
    * Variables to keep track of whether we need to create
    * reference Ixstream for audio/video/text.
    *
    * We will create reference IxStreams under following circumstances:
    * a) When user passes the same filename to play audio/video/text. This file name
    *    could be protected /unprotected content.
    * b) When IxStream* is passed into QTV from BREW based OEM's.
    *
    * The reason we create reference streams is because IxStream code is not reentrant,
    * so we can't use the same handle across audio/video/text threads.
    */
    bool m_bNeedAudioRefStream;
    bool m_bNeedTextRefStream;
    bool m_bNeedVideoRefStream;

    /*
    * When set to true, QTVDispatchThread will be stopped, if running.
    */
    bool m_bStopToBeDone;

    /*
    * Mpeg4Player* to post messages to Mpeg4Player thread. When content mime is determined,
    * it is routed to Mpeg4Player /Real Player. If content mime type is unsupported,
    * error will be reported.
    */
    Mpeg4Player* m_pMpeg4Player;
    QtvPlayer *m_pQtvPlayer;


    /*
    * We might have to create separate IxStream when user
    * passes different file names, in that case, we will need
    * separate OpenRspT or IxCallback  for audio/video/text.
    * In each of the following array,
    * 1st element,(at index 0):Video
    * 2nd element,(at index 1):Audio,
    * 3rd element,(at index 2):Text.
    */
    IxStreamOpenRspT      IxStreamopenRspArray[IxStreamCallbackArrayLength];
    IxCallback            IxStreamIxCallbacksArray[IxStreamCallbackArrayLength];

    /*
    * Variables used to keep track of number of IxStreams created,
    * and acknowledged(successful/fail)for protected/unprotected contents.
    *
    * Once m_nIxStreamsCreated == m_nIxStreamSuccessfulCallbacks,
    * content mime type will be determined and control will be routed
    * Mpeg4Player / Real Player.
    *
    * When (m_nIxStreamFailedCallbacks + m_nIxStreamSuccessfulCallbacks) == m_nIxStreamsCreated,
    * and (m_nIxStreamFailedCallbacks > 0), error occured while creating one of the IxStream
    * or when creating reference IxStream.
    */

    int m_nIxStreamSuccessfulCallbacks;
    int m_nIxStreamsCreated;
    int m_nIxStreamFailedCallbacks;

    /*
    * Information about each IxStream( reference IxStream or an actual instance)
    * will be stored in following structure.
    */
    typedef struct
    {
      /*
      * Identify whether this stream is used for audio/video/text.
      */
      QtvPlayer::TrackType trackType;

      /*
      * Valid only if
      * 'm_bNeedVideoRefStream'   ||
      * 'm_bNeedAudioRefStream'   ||
      * 'm_bNeedTextRefStream' (@see above) is TRUE.
      *
      * Identify the trackType which is used to create this reference IxStream.
      */
      QtvPlayer::TrackType referenceOfTrackType;

      /*
      * When all commit/callbacks are acknowledgeed successfully,
      * stream is ready for IO.
      */
      bool isStreamReadyforIO;

      /*
      * When 'commit' is called for an IxStream(for a/v/t), following flag is set
      * to TRUE which gets reset when callback specified in 'commit' gets called.
      */
      bool isWaitingForCommitAcked;

      /*
      * URI identifying the content that is being installed.
      */
      IxContentDef::UriT           uri;

      /*
      * Handle to the IxStream
      */
      IxStream* pStream;

#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
      /*
      * IxContentMgrClient needed when installing content in Content database.
      */
      IxContentMgrClient           *pContentMgrClient;
#endif

      IxMedia*                     ixMediaHandle;
      /*
      * 'pContentMgrClient' and 'uri' declared above are needed when 'commit' callback is
      * fired after installing content attributes.
      */

      /*
      *Need this variable to check that we have CreateRef'ed this stream before deallocation.
      */
      bool isStreamOpened;
      /*
      * Pointer to QtvPlayerDispatch to be able to handle multiple instances.
      */
      QtvPlayerDispatch* pMySelf;
    }QtvIxStreamInfoT;

    QtvIxStreamInfoT QtvIxStreamInfo[IxStreamCallbackArrayLength];

    /*
    * Critical section to control access to Dispatch thread/class members.
    *
    */
    QCCritSectType dQTVDispatchThread_CS;

    /*
    * Dispatch thread TC.
    */
    QCThreadControl dQTVDispatchThreadTC;

    /*
    * Dispatch thread message receive block.
    */
    QCMessageReceive dQTVDispatchThreadER;

    /*
    * Dispatch thread startup sync signal.
    */
    QCConditionType dQTVDispatchThreadStartSync;

    /*
    * Dispatch Thread done/stop sync signal.
    */
    QCConditionType dQTVDispatchThreadIODoneSync;

    /*
    * Initializes QtvPlayerDispatch class members.
    */
    void QtvPlayerDispatchInit();

    /*
    * Posts an event for Mpeg4Player which contains IxStream* for audio/video/text.
    */
    QtvPlayer::ReturnT OpenInputStream(dcf_ixstream_type vstream, dcf_ixstream_type astream,dcf_ixstream_type tstream);

    /*
    * Routine invokes DetermineContentMimeType and routes to Mpeg4Player/Real Player(not yet supported)
    */
     void StartProcessingFromIxStream(dcf_ixstream_type pstream);

#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
    /*
    * If the content is already installed in the content database, following routine returns
    * true otherwise, returns false.
    *
    * Before trying to register the conetnt, we should invoke following to see if it is
    * already installed in the database.
    *
    * If the content is not already installed, returns false.
    *
    * Please @see IxContentMgrClient and IxContent.h for more information.
    */
    bool isContentInstalledInContentDataBase(IxContentDef::UriT*,QtvPlayer::TrackType trackType);

    /*
    * Registers the content with contentManager by installing content attributes.
    *
    * Argumets:
    * fileName: Filename to identify the content
    * trackType: Associates the IxStream instance that will be created to audio/video/text
    */
    bool RegisterContentToContMgr(const char* fileName,QtvPlayer::TrackType);
#endif
    /*
    * When user wants to play audio/video/text from the same file or when OEM passes single
    * IxStream instance, we will create reference IxStreams in following routine. This is because
    * IxStream is nor reentrant so can't use single handle among these threads.
    *
    * m_bNeedAudioRefStream, m_bNeedTextRefStream, m_bNeedVideoRefStream declared above
    * are used to determine whether we need to create reference Ixstream for audio/video/text.
    *
    */
    void CreateReferenceIxStreams();

    /*
    *
    * Read first few bytes to determine content mime type.
    * This is required to route to Mpeg4Player or RealPlayer.
    * This function updates QtvPlayer::CurrentMIMEType with the content mime type.
    *
    */
    bool DetermineContentMimeType(dcf_ixstream_type pstream);

#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
    /*
    *
    * Callback function to be supplied to DRM suite.
    *
    */
     static void IxCallBackWrapper(IxCallback* pcb);
#endif

    /*
    *
    * Callback function to be supplied to DRM suite when
    * creating IxStream via StreamFactory::CreateStream
    * @see IxStreamFactory.h for more details.
    *
    */
     static void IxStreamCreateCallbackWrapper(void* user);

    /*
    *
    * Callback function to be supplied to DRM suite when
    * creating reference IxStream.
    * @see IxStream.h for more details.
    *
    */
     static void CreateRefIxStreamCallbackWrapper(void* user);


};

class DispatchThread
{
  public:

    static DispatchThread* GetSingletonInstance();
    static void DestroyInst();

    bool IsQTVDispatchThreadActive();
     bool StartQTVDispatchThread();
    bool QTVDispatchThreadPostMessage(QTVDispatchMessageType *pEvent);
    static void  QTVDispatchThread_STOP_EVENT_handler (QtvPlayerDispatchMessageHandleType handle);

  // Removed RVCT 2.2 update protected:
    DispatchThread();
    DispatchThread(const DispatchThread&);
    DispatchThread& operator= (const DispatchThread&);
    ~DispatchThread();

    static uint32 m_objRefCnt;
    static DispatchThread* pInstance;

    bool m_bStopToBeDone;
    bool m_bThreadStarted;

    /*
    * Critical section to control access to Dispatch thread/class members.
    *
    */
    QCCritSectType QTVDispatchThread_CS;

    /*
    * Dispatch thread TC.
    */
    QCThreadControl QTVDispatchThreadTC;

    /*
    * Dispatch thread message receive block.
    */
    QCMessageReceive QTVDispatchThreadER;

    /*
    * Dispatch thread startup sync signal.
    */
    QCConditionType QTVDispatchThreadStartSync;

    /*
    * Dispatch Thread done/stop sync signal.
    */
    QCConditionType QTVDispatchThreadIODoneSync;

    void QTVDispatchThread();
     static void  TheQTVDispatchThread(unsigned long param);
    void QTVDispatchThreadStopAndDestroy();
};
#endif //#ifdef FEATURE_QTV_DRM_DCF

#endif//#ifndef _QTVPLAYER_DISPATCHAPI_H_


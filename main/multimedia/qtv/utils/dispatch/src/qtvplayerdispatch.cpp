/* =======================================================================
                               QtvPlayerDispatch.cpp
DESCRIPTION
  Dispatch API for QTV / REAL contents when DRM_DCF is enabled.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History
$Header: //source/qcom/qct/multimedia/qtv/utils/dispatch/main/latest/src/qtvplayerdispatch.cpp#8 $
$DateTime: 2010/06/08 02:52:59 $
$Change: 1325960 $
========================================================================== */

/* ==========================================================================

                    INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#ifdef FEATURE_QTV_DRM_DCF

 #include "QtvPlayerDispatch.h"

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  #include "QtvRealPlayer.h"
  #include "QtvRAMFilePlayer.h"
  #include "ctype.h"
#endif

  /*Declare extern variables, if any */

#if defined(FEATURE_IXCONTENTSVC)
  #include "ixipc.h"
  #ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
    extern "C" void IxStream_ProcessMsg (ixipc_msg_type** ipcRspMsg );
    extern "C" void IxHTTPClnt_ProcessMsg( ixipc_msg_type**  msgPtr );
    extern "C" void IxContentClient_ProcessMsg( ixipc_msg_type**  msgPtr );
    extern "C" void IxContentMgrClient_ProcessMsg( ixipc_msg_type**  msgPtr );
    extern "C" void IxContentBrowserClient_ProcessMsg( ixipc_msg_type**  msgPtr );
  #endif
#endif /* defined(FEATURE_IXCONTENTSVC) */

DispatchThread* DispatchThread::pInstance = NULL;
uint32 DispatchThread::m_objRefCnt = 0;

  /*
  * Constants to be used when setting up EVENT TABLE / EVENT SINK / EVENT_SOURCE.
  */
  static const int QTV_API_EVENT_PRI = 1;
  static const int QTV_INTERNAL_EVENT_PRI = QTV_API_EVENT_PRI+1;
  static const int QTV_INTERNAL_EVENT_PRI_HIGH = QTV_INTERNAL_EVENT_PRI+1;
  static const int QTV_EVENT_PRI_URGENT = QTV_INTERNAL_EVENT_PRI_HIGH;

  //Event table for QTVPlayerDispatch Thread
  EVENT_TABLE( QtvPlayerEventTable, NULL )

  /*
  * Event sink/handler for various events when FEATURE_QTV_DRM_DCF is defined.(In custmp4.h)
  */

  EVENT_SINK( PV_OPEN_CONTENT_URN, QtvPlayerDispatch::PV_OPEN_CONTENT_URN_handler,
              QTV_API_EVENT_PRI, TRUE ) /*lint !e641 */

  EVENT_SINK( QTVPLAYER_STOP_EVENT,DispatchThread::QTVDispatchThread_STOP_EVENT_handler,
              QTV_API_EVENT_PRI, TRUE)

  EVENT_SINK ( QTV_IXSTREAM_OPEN_EVENT, QtvPlayerDispatch::QTV_IXSTREAM_OPEN_EVENT_handler,
               QTV_API_EVENT_PRI, TRUE )

  EVENT_TABLE_END

  EVENT_SOURCE( PV_OPEN_INPUTSTREAM_URN, PV_API_POOL )
  EVENT_SOURCE( QTVPLAYER_STOP_EVENT, PV_API_POOL )
  EVENT_SOURCE( QTV_IXSTREAM_OPEN_EVENT, PV_API_POOL)
  EVENT_SOURCE( PV_OPEN_CONTENT_URN, PV_API_POOL )

/* ======================================================================
FUNCTION
  QtvPlayerDispatch::QtvPlayerDispatch

DESCRIPTION
  Constructor for QtvPlayerDispatch class.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
QtvPlayerDispatch::QtvPlayerDispatch(QtvPlayer *player)
{
  m_pQtvPlayer = player;
  QtvPlayerDispatchInit();
}
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::QtvPlayerDispatchInit

DESCRIPTION
  Initialize callback structures and other class members.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void QtvPlayerDispatch::QtvPlayerDispatchInit()
{
  /*
   * Initialize various callback structures used when
   * creating IxStreams / reference IxStreams or when
   * registering protected contents.
   */
   int i = 0;
   for (i = 0;i < IxStreamCallbackArrayLength; i++)
   {
     memset(&IxStreamopenRspArray[i], 0, sizeof(IxStreamOpenRspT));
     memset(&IxStreamIxCallbacksArray[i] ,0, sizeof(IxCallback));

     memset(&QtvIxStreamInfo[i], 0, sizeof(QtvIxStreamInfoT));

     QTV_MSG_PRIO2(QTVDIAG_GENERAL,
     QTVDIAG_PRIO_MED,
     "i= %d &IxStreamopenRspArray[i] = %x",i,&IxStreamopenRspArray[i]);

     QTV_MSG_PRIO2(QTVDIAG_GENERAL,
     QTVDIAG_PRIO_MED,
     "i= %d &IxStreamIxCallbacksArray[i] = %x",i,&IxStreamIxCallbacksArray[i]);

     QTV_MSG_PRIO2(QTVDIAG_GENERAL,
     QTVDIAG_PRIO_MED,
     "i= %d &QtvIxStreamInfo[i] = %x",i,&QtvIxStreamInfo[i]);

     switch(i)
     {
       case VIDEO_INDEX:
         QtvIxStreamInfo[i].trackType = QtvPlayer::VideoTrack;
         break;

       case AUDIO_INDEX:
         QtvIxStreamInfo[i].trackType = QtvPlayer::AudioTrack;
         break;

       case TEXT_INDEX:
         QtvIxStreamInfo[i].trackType = QtvPlayer::TextTrack;
         break;
     }
     QtvIxStreamInfo[i].pMySelf = this;
     IxStreamopenRspArray[i].pUserData = (void*)&QtvIxStreamInfo[i];
     IxStreamIxCallbacksArray[i].AEECallback.pNotifyData = (void*)&QtvIxStreamInfo[i];
     IxStreamIxCallbacksArray[i].AEECallback.pUserData =(uint32)&QtvIxStreamInfo[i];

     QTV_MSG_PRIO2(QTVDIAG_GENERAL,
     QTVDIAG_PRIO_MED,
     "i= %d IxStreamIxCallbacksArray[i].AEECallback.pNotifyData = %x",
     i,
     IxStreamIxCallbacksArray[i].AEECallback.pNotifyData);

     QTV_MSG_PRIO2(QTVDIAG_GENERAL,
     QTVDIAG_PRIO_MED,"i= %d IxStreamIxCallbacksArray[i].AEECallback.pUserData = %x",
     i,
     IxStreamIxCallbacksArray[i].AEECallback.pUserData);
   }
   m_nIxStreamSuccessfulCallbacks = 0;
   m_nIxStreamsCreated = 0;
   m_nIxStreamFailedCallbacks = 0;

   QtvIxStreamInfo[AUDIO_INDEX].isStreamOpened = false;
   QtvIxStreamInfo[VIDEO_INDEX].isStreamOpened = false;
   QtvIxStreamInfo[TEXT_INDEX].isStreamOpened = false;

   m_bNeedVideoRefStream = false;
   m_bNeedAudioRefStream = false;
   m_bNeedTextRefStream = false;

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayerDispatchInit");
}
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::~QtvPlayerDispatch()

DESCRIPTION
  Destructor QtvPlayerDispatch class.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
QtvPlayerDispatch::~QtvPlayerDispatch()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"~QtvPlayerDispatch");
  for (int i = 0;i < IxStreamCallbackArrayLength; i++)
  {
#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
     if(QtvIxStreamInfo[i].pContentMgrClient)
     {
       QTV_Delete(QtvIxStreamInfo[i].pContentMgrClient);
       QtvIxStreamInfo[i].pContentMgrClient = NULL;
     }
#endif
     if(QtvIxStreamInfo[i].uri.Ptr)
     {
       QTV_Free(QtvIxStreamInfo[i].uri.Ptr);
       QtvIxStreamInfo[i].uri.Ptr = NULL;
     }
  }
  if(QtvIxStreamInfo[AUDIO_INDEX].isStreamOpened)
     QtvIxStreamInfo[AUDIO_INDEX].pStream->Close();
  if(QtvIxStreamInfo[VIDEO_INDEX].isStreamOpened)
     QtvIxStreamInfo[VIDEO_INDEX].pStream->Close();
  if(QtvIxStreamInfo[TEXT_INDEX].isStreamOpened)
     QtvIxStreamInfo[TEXT_INDEX].pStream->Close();
}
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::SetMpeg4PlayerHandle

DESCRIPTION
  Stores the Mpeg4Player handle to be used later to post messages/events for Mpeg4Player.

DEPENDENCIES
  None

INPUT/OUTPUT:

  ->Mpeg4Player* mpeg4PlayerPtr: Pointer/handle to the Mpeg4Player class.
  <- None.

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void QtvPlayerDispatch::SetMpeg4PlayerHandle(Mpeg4Player* mpeg4PlayerPtr)
{
  m_pMpeg4Player = mpeg4PlayerPtr;

  QTV_MSG_PRIO(QTVDIAG_GENERAL,
  QTVDIAG_PRIO_MED,
  ">>QtvPlayerDispatch::SetMpeg4PlayerHandle");
}
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::CreateReferenceIxStreams

DESCRIPTION
  Creates reference IxStream from the 'pstream' passed in.

INPUT/OUTPUT:
 -> dcf_ixstream_type pstream: A valid IxStream* to be used for creating reference streams.
 <- None.

DEPENDENCIES
 @see Include files above.

RETURN VALUE
  None

SIDE EFFECTS
========================================================================== */
void QtvPlayerDispatch::CreateReferenceIxStreams()
{
  IxErrnoType errorCode = E_FAILURE;
  IxStream* streamPtr = NULL;
  uint32 size = 0;

  if(m_bNeedAudioRefStream)
  {
    if( QtvIxStreamInfo[AUDIO_INDEX].referenceOfTrackType == QtvPlayer::VideoTrack )
    {
      streamPtr = QtvIxStreamInfo[VIDEO_INDEX].pStream;
    }
    if(streamPtr)
    {
      IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pUserData = (uint32)&QtvIxStreamInfo[AUDIO_INDEX];
      if(streamPtr->IsProgressive())
      {
        size = 0x7FFFFFFF;
      }
      else
      {
        (void)streamPtr->Size(&size);
      }
      errorCode = streamPtr->CreateRef(0,size,&QtvIxStreamInfo[AUDIO_INDEX].pStream,
                                     QtvPlayerDispatch::CreateRefIxStreamCallbackWrapper,
                                     &IxStreamIxCallbacksArray[AUDIO_INDEX]);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " CreateReferenceIxStreams for audio.. &IxStreamIxCallbacksArray[AUDIO_INDEX]  = %x",
      &IxStreamIxCallbacksArray[AUDIO_INDEX]);

      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " CreateReferenceIxStreams for audio.. IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pUserData  = %x",
      IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pUserData);

    }
    if((errorCode == E_SUCCESS)||(errorCode == E_AGAIN))
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      "CreateRef for audio stream successful,errorCode %d",errorCode);

      /*
       * Even though we are creating references here, increment the count as
       * we want to wait for callback specified below via IPC handler.
       * Creating reference is very much like creating IxStream,
       * where new IxStream(reference)is subset of the original and
       * gets it's own access control.
       */
      m_nIxStreamsCreated++;
      QtvIxStreamInfo[AUDIO_INDEX].isStreamOpened = true;
      //The callback may not fired as CreateRef can act synchronous/asynchronous.
      if(errorCode == E_SUCCESS)
      {
          /*
          * When we get E_AGAIN, it's asynchronous and callback will be fired subsequently.
          * Otherwise, E_SUCCESS indicates that CreateRef is successful and we are good to go.
          */
        m_nIxStreamSuccessfulCallbacks++;
        QtvIxStreamInfo[AUDIO_INDEX].isStreamReadyforIO = true;
       }
    }
    else
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      "CreateRef for audio stream failed..errorCode %d",errorCode);
      QtvIxStreamInfo[AUDIO_INDEX].isStreamOpened = false;
    }
    m_bNeedAudioRefStream = false;
  }
  errorCode = E_FAILURE;

  if(m_bNeedTextRefStream)
  {
    if( QtvIxStreamInfo[TEXT_INDEX].referenceOfTrackType == QtvPlayer::VideoTrack )
    {
      streamPtr = QtvIxStreamInfo[VIDEO_INDEX].pStream;
    }
    else if( QtvIxStreamInfo[TEXT_INDEX].referenceOfTrackType == QtvPlayer::AudioTrack )
    {
      streamPtr = QtvIxStreamInfo[AUDIO_INDEX].pStream;
    }
    if(streamPtr)
    {
      IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData = (uint32)&QtvIxStreamInfo[TEXT_INDEX];
      if(streamPtr->IsProgressive())
      {
        size = 0x7FFFFFFF;
      }
      else
      {
        (void)streamPtr->Size(&size);
      }
      errorCode = streamPtr->CreateRef(0,size,&QtvIxStreamInfo[TEXT_INDEX].pStream,
                                     QtvPlayerDispatch::CreateRefIxStreamCallbackWrapper,
                                     &IxStreamIxCallbacksArray[TEXT_INDEX]);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " CreateReferenceIxStreams for text.. &IxStreamIxCallbacksArray[TEXT_INDEX]  = %x",
      &IxStreamIxCallbacksArray[TEXT_INDEX]);

      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED," CreateReferenceIxStreams for text.. IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData  = %x",
      IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData);
    }

    if((errorCode == E_SUCCESS)||(errorCode == E_AGAIN))
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"CreateRef for text stream successful,errorCode %d",errorCode);
      /*
       * Even though we are creating references here, increment the count as
       * we want to wait for callback specified below via IPC handler.
       * Creating reference is very much like creating IxStream,
       * where new IxStream(reference)is subset of the original and
       * gets it's own access control.
       */
       m_nIxStreamsCreated++;
       QtvIxStreamInfo[TEXT_INDEX].isStreamOpened = true;
       //The callback may not fired as CreateRef can act synchronous/asynchronous.
        if(errorCode == E_SUCCESS)
        {
          /*
           * When we get E_AGAIN, it's asynchronous and callback will be fired subsequently.
             * Otherwise, E_SUCCESS indicates that CreateRef is successful and we are good to go.
             */
           m_nIxStreamSuccessfulCallbacks++;
           QtvIxStreamInfo[TEXT_INDEX].isStreamReadyforIO = true;
        }
    }
    else
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      "CreateRef for text stream failed..errorCode %d",errorCode);
      QtvIxStreamInfo[TEXT_INDEX].isStreamOpened = false;
    }
    m_bNeedTextRefStream = false;
  }
  QTV_MSG_PRIO2(QTVDIAG_GENERAL,
  QTVDIAG_PRIO_MED,
  " Leaving CreateRefIxStreams,m_nIxStreamSuccessfulCallbacks =%d m_nIxStreamsCreated= %d ",
  m_nIxStreamSuccessfulCallbacks,m_nIxStreamsCreated);
}

/* ======================================================================
FUNCTION
  QtvPlayerDispatch::StartProcessingFromIxStream

DESCRIPTION
  For valid IxStream, invoke DetermineContentMimeType to get content Mime type
  and  route to RealPlayer if Real content, otherwise to Mpeg4Player if QTV mime type
  otherwise report unknown content.

INPUT/OUTPUT:
 ->dcf_ixstream_type pstream: Valid IxStream*.
 <-: None.

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS:
None
========================================================================== */
void QtvPlayerDispatch::StartProcessingFromIxStream(dcf_ixstream_type pstream)
{
  //DetermineContentMimeType determines MIME type by reading first few bytes
  //and sets CurrentMIMEType appropriately. Returns True if successful in identifying
  //MimeType else returns false.

  QtvPlayer::PlayerStateRecordT ps;
  QtvPlayer::GetPlayerState(ps, (QtvPlayer::InstanceHandleT)m_pQtvPlayer);

  if(DetermineContentMimeType(pstream))
  {
  #ifdef FEATURE_REAL_PLAYER_USE_QTV_API
    ASSERT(m_pQtvPlayer);
    QtvRAMPlayer_Init(m_pQtvPlayer);
    m_pQtvPlayer->Init(m_pQtvPlayer->MallocBufferFunction,
                       m_pQtvPlayer->FreeBufferFunction,
                       m_pQtvPlayer->CurrentMIMEType);
    if (m_pQtvPlayer->CurrentMIMEType)
    {
      if(strcmp(m_pQtvPlayer->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
      {
        /*
        * Pass IxStream from here to REAL Player.
        */
        uint32 size = 0;
        if( ( (IxStream*)pstream )->IsProgressive() )
        {
          size = 0x7FFFFFFF;
        }
        else
        {
          ((IxStream*)pstream)->Size(&size);
        }

        QTV_MSG_PRIO(QTVDIAG_GENERAL,
        QTVDIAG_PRIO_MED,
        " StartProcessingFromIxStream,REAL_MEDIA_MIME_TYPE");

        QtvRealPlayer_OpenIxStream( (ixstream_type_ptr)pstream,(unsigned long)size);

        return;
        }
    }//if (CurrentMIMEType)
  #else
      ASSERT(m_pQtvPlayer);
      m_pQtvPlayer->Init(m_pQtvPlayer->MallocBufferFunction,
                         m_pQtvPlayer->FreeBufferFunction,
                         m_pQtvPlayer->CurrentMIMEType);
  #endif

  /*
  * 3GP/WM MimeType,route to Mpeg4Player.
  * Invoke OpenInputStream, which will posts an event for Mpeg4Player.
  */

  OpenInputStream((dcf_ixstream_type)QtvIxStreamInfo[VIDEO_INDEX].pStream,
                                 (dcf_ixstream_type)QtvIxStreamInfo[AUDIO_INDEX].pStream,
                                 (dcf_ixstream_type)QtvIxStreamInfo[TEXT_INDEX].pStream);

  }//if(DetermineContentMimeType(pstream))

  else if(strcmp(m_pQtvPlayer->CurrentMIMEType,QTV_UNKNOWN_MIME_TYPE)==0)
  {
    /*
    *
    * If MIME type is not recognized, report an error and return.
    * This mighthappen if user passes a DCF file name but the content is not REAL/3GP/WM.
    */
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_ERROR,
    "CurrentMIMEType is QTV_UNKNOWN_MIME_TYPE");

    NotifyCallback(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED,
                   m_pQtvPlayer->CallbackClientData,ps);
  }
}

/* ======================================================================
FUNCTION
  QtvPlayerDispatch::DetermineContentMimeType

DESCRIPTION
  Reads first few bytes to determine content Mime Type.

DEPENDENCIES
  None

INPUT/OUTPUT:
 ->dcf_ixstream_type dcfstream : IxStream* to identify the content to be played.
 <-: True is mime type is recognized otherwise, returns false.

RETURN VALUE
  True if mime type is recognized otherwise, returns false.
  updates QtvPlayer::CurrentMIMEType to reflect the mime type detected.

SIDE EFFECTS
  Will update QtvPlayer::CurrentMIMEType.

========================================================================== */
bool QtvPlayerDispatch::DetermineContentMimeType(dcf_ixstream_type dcfstream)
{
  uint32 nRead = 0;
  bool   isEndofStream = false;
  bool bRet = false;
  uint32 reqSizeToRead = 16;
  uint8 buffer[16];

  if(!m_pQtvPlayer->CurrentMIMEType)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_FATAL,
    "DetermineContentMimeType CurrentMIMEType is NULL..");

    return bRet;
  }
  m_pQtvPlayer->CurrentMIMEType = QTV_UNKNOWN_MIME_TYPE;

  if(dcfstream == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_FATAL,
    "DetermineContentMimeType dcfstream passed in is NULL..");

    return bRet;
  }

  IxStream* pStream = (IxStream*)dcfstream;

  /*
  * This function is used only to read first few bytes to decide whether it's an ASF/3GP/RM file.
  */

  IxErrnoType errorCode = E_FAILURE;
  errorCode = E_FAILURE;
  errorCode = pStream->Read((byte*)buffer,reqSizeToRead,&nRead,&isEndofStream);
  if(errorCode == E_SUCCESS)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_FATAL,
    "QtvPlayerDispatch::DetermineContentMimeType nRead %d",nRead );

    errorCode = pStream->Seek(0,IX_STRM_SEEK_START);

     if (memcmp(buffer, FMT_RM_MARK, 4) == 0)
     {
       m_pQtvPlayer->CurrentMIMEType = REAL_MEDIA_MIME_TYPE;
       bRet = true;
     }
     else if (memcmp(buffer, FMT_WM_MARK, 16) == 0)
     {
       m_pQtvPlayer->CurrentMIMEType = QTV_PLAYER_MIME_TYPE;
       bRet = true;
     }
     if (memcmp(buffer+4, FMT_MP4_MARK, 4) == 0)
     {
       m_pQtvPlayer->CurrentMIMEType = QTV_PLAYER_MIME_TYPE;
       bRet = true;
    }
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_FATAL,
    "DetermineContentMimeType:pStream->Read failed error code %d",
    errorCode );
  }
  return bRet;
}

/* ======================================================================
FUNCTION
  QtvPlayerDispatch::PV_OPEN_CONTENT_URN_handler

DESCRIPTION

Handler for PV_OPEN_CONTENT_URN event. It creates IxStream for protected/unprotected content.

INPUT PARAMETERS:
 ->PV_OPEN_CONTENT_URN_type*: PV_OPEN_CONTENT_URN event containing audio/video/text file names.
 <-: None.

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void QtvPlayerDispatch::PV_OPEN_CONTENT_URN_handler(QtvPlayerDispatchMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL,
  QTVDIAG_PRIO_HIGH,
  "QtvPlayerDispatch::PV_OPEN_CONTENT_URN_handler" );

  PV_OPEN_CONTENT_URN_type* pEvent = (PV_OPEN_CONTENT_URN_type*)handle;

  if(pEvent == NULL)
  {
    /*Report the error and return*/
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "QtvPlayerDispatch::PV_OPEN_CONTENT_URN_handler got NULL handle" );

    return;
  }

  const char* videoURN = pEvent->videoURN;
  const char* audioURN = pEvent->audioURN;
  const char* textURN  = pEvent->textURN;
  const char *URN = (videoURN ? videoURN : audioURN);
  if(pEvent->DispatchHandle == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "PV_OPEN_CONTENT_URN_handler got NULL DispatchHandle.." );

    return;
  }
  QtvPlayerDispatch* myself = (QtvPlayerDispatch*)pEvent->DispatchHandle;
  QtvPlayer::PlayerStateRecordT ps;
  QtvPlayer::GetPlayerState(ps, (QtvPlayer::InstanceHandleT)myself->m_pQtvPlayer);

  URN = URN ? URN : textURN;

  char* newVideoMediaURN = NULL;
  char* newAudioMediaURN = NULL;
  char* newTextMediaURN  = NULL;

  bool  runThroughIxStream = true;
  bool  bIsDCFMedia = false;
  IxErrnoType errorCode = E_FAILURE;
  IxStreamFactory streamFactory;
  bool bIsLocalFile = isLocalFileMedia(URN);

  /*
  * By default, we won't create separate IxStream for audio/video unless
  * user has specified different file names.
  */
  bool bNeedSeparateAudioVideoIxStream = false;

  /*Make sure the file name passed in is for local file*/
  if(bIsLocalFile == true)
  {
    if(videoURN && audioURN)
   {
     //User has specified audio,video file name
     if(strcmp(videoURN,audioURN)!=0)
     {
       //audio, video to be played from different file,
       //so we will need separate IxStream for audio,video
       bNeedSeparateAudioVideoIxStream =true;
      }
    }
    if( QtvPlayerDispatch::IsDrmDcfMedia(URN) )
    {
      bIsDCFMedia = true;
      runThroughIxStream = false;
   }

   /*
    * Modify the filename passed as IxStream suite needs filename to start
    * with "file:/"
    */
      if(videoURN)
     {
        if(bIsDCFMedia)
        {
          //For DCF media, we get IxStream by installing
          //the content, if not already installed,
          //and invoking GetObject. In this case,
          //DRM suite just needs the actual path,
          // /brew/mod/.. etc. It should not start with "file:/"
          //For non DCF media, when creating IxStream from StreamFactory
          //need to specify "file:/"
          newVideoMediaURN = (char*)QTV_Malloc(strlen(videoURN) + 2);
        }
        else
        {
          //For non DCF media, file URL should start with "file:/"
          newVideoMediaURN = (char*)QTV_Malloc(strlen(videoURN)+strlen("file:/")+1);
        }
        if(newVideoMediaURN == NULL)
       {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "QTV_Malloc failed for video");
          //Weird, malloc failed, report the error and return
          myself->NotifyCallback(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED,
                                 myself->m_pQtvPlayer->CallbackClientData,ps);
         return;
        }
        if(bIsDCFMedia)
        {
          std_strlcpy(newVideoMediaURN+0, "/", strlen("/"));
          std_strlcpy(newVideoMediaURN+1, videoURN, strlen(videoURN) + 1);
        }
        else
        {
          std_strlcpy(newVideoMediaURN+0, "file:/", strlen("file:/"));
          std_strlcpy(newVideoMediaURN+strlen("file:/"),videoURN, strlen(videoURN) + 1);
        }
      }
      if(audioURN)
      {
        if(bIsDCFMedia)
        {
          newAudioMediaURN = (char*)QTV_Malloc(strlen(audioURN) + 2);
        }
        else
        {
          newAudioMediaURN = (char*)QTV_Malloc(strlen(audioURN)+strlen("file:/")+1);
        }

        if(newAudioMediaURN == NULL)
        {
          if(newVideoMediaURN)
          {
            QTV_Free(newVideoMediaURN);
          }
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "QTV_Malloc failed for audio");
          //Weird, malloc failed, report the error and return
          myself->NotifyCallback(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED,
                                 myself->m_pQtvPlayer->CallbackClientData,ps);

          return;
        }
        if(bIsDCFMedia)
        {
          std_strlcpy(newAudioMediaURN+0, "/", strlen("/"));
          std_strlcpy(newAudioMediaURN+1, audioURN, strlen(audioURN) + 1);
        }
        else
        {
          std_strlcpy(newAudioMediaURN+0, "file:/", strlen("file:/"));
          std_strlcpy(newAudioMediaURN+strlen("file:/"),audioURN, strlen(audioURN) + 1);
        }
      }
      if(textURN)
      {
        if(bIsDCFMedia)
        {
          newTextMediaURN = (char*)QTV_Malloc(strlen(textURN) + 2);
        }
        else
        {
          newTextMediaURN = (char*)QTV_Malloc(strlen(textURN)+strlen("file:/")+1);
        }

        if(newTextMediaURN == NULL)
        {
          if(newVideoMediaURN)
          {
            QTV_Free(newVideoMediaURN);
          }
          if(newAudioMediaURN)
          {
            QTV_Free(newAudioMediaURN);
          }

          QTV_MSG_PRIO(QTVDIAG_GENERAL,
          QTVDIAG_PRIO_ERROR,
          "QTV_Malloc failed for text");

          myself->NotifyCallback(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED,
                                 myself->m_pQtvPlayer->CallbackClientData,ps);

        return;
        }
        if(bIsDCFMedia)
        {
          std_strlcpy(newTextMediaURN+0, "/", strlen("/"));
          std_strlcpy(newTextMediaURN+1, textURN, strlen(textURN) + 1);
        }
        else
        {
          std_strlcpy(newTextMediaURN+0, "file:/", strlen("file:/"));
          std_strlcpy(newTextMediaURN+strlen("file:/"),textURN, strlen(textURN) + 1);
        }
      }
    myself->m_nIxStreamSuccessfulCallbacks = 0;
    myself->m_nIxStreamsCreated = 0;
    myself->m_nIxStreamFailedCallbacks = 0;

    if(bIsDCFMedia)
    {
      //Media is DCF media(DCF extension).
      //Register the content with IxContentManager and get the associated IxStream.

      /*
       * Register and get the IxStream for Video.
       */
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_LOW,
      "PV_OPEN_CONTENT_URN_handler Invoking RegisterContentToContMgr" );

      if(newVideoMediaURN)
      {
        bool nRet = false;
#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
        nRet = (IxStream*)myself->RegisterContentToContMgr((const char*)newVideoMediaURN,
                                                                   QtvPlayer::VideoTrack);
#endif

        if(!nRet)
       {
         //Error occured, report QTV_PLAYER_VIDEO_IXSTREAM_FAILED and return
          QTV_MSG_PRIO(QTVDIAG_GENERAL,
          QTVDIAG_PRIO_LOW,
          "RegisterContentToContMgr for video failed.." );

          myself->NotifyCallback(QtvPlayer::QTV_PLAYER_VIDEO_IXSTREAM_FAILED,
                                 myself->m_pQtvPlayer->CallbackClientData,ps);
          //Error occured, free video/audio/text URNs, if allocated.
          if(newVideoMediaURN!= NULL)
          {
            QTV_Free(newVideoMediaURN);
          }
          if(newAudioMediaURN)
          {
            QTV_Free(newAudioMediaURN);
          }
          if(newTextMediaURN)
          {
            QTV_Free(newTextMediaURN);
          }

          return;
        }

        myself->m_bNeedVideoRefStream = false;

        if(!bNeedSeparateAudioVideoIxStream)
        {
          /*
           * If user has specified non NULL audioURN, need to play the audio.
           * So, will need reference stream for audio as
           * audio/video is from same file.
           */
           if(newAudioMediaURN)
           {
             myself->m_bNeedAudioRefStream = true;
             myself->QtvIxStreamInfo[AUDIO_INDEX].referenceOfTrackType = QtvPlayer::VideoTrack;
           }
        }//if(!bNeedSeparateAudioVideoIxStream)

        //Check to see if text is also to be played from the same file as of audio/video
        if(textURN)
        {
          //Don't need separate IxStream for text but rather reference IxStream will be
          //created from video/audio IxStream.
          if(videoURN && (strcmp(textURN,videoURN) == 0) )
          {
            myself->m_bNeedTextRefStream = true;
            myself->QtvIxStreamInfo[TEXT_INDEX].referenceOfTrackType = QtvPlayer::VideoTrack;
          }
        }
      }//if(newVideoMediaURN)

      /*
       * Register and get the IxStream for Audio.
       */
      if(newAudioMediaURN)
      {
        if( ((!bNeedSeparateAudioVideoIxStream) && (newVideoMediaURN == NULL)) ||
             (bNeedSeparateAudioVideoIxStream) )
        {
          bool nRet = false;
#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
          const char* temp_urn =  newVideoMediaURN ? newVideoMediaURN : newAudioMediaURN;
             nRet = (IxStream*)myself->RegisterContentToContMgr(temp_urn,
                                                              QtvPlayer::AudioTrack);
#endif
          if(!nRet)
           {
             //Error occured, report QTV_PLAYER_AUDIO_IXSTREAM_FAILED and return
             QTV_MSG_PRIO(QTVDIAG_GENERAL,
            QTVDIAG_PRIO_LOW, "RegisterContentToContMgr for audio failed.." );

            myself->NotifyCallback(QtvPlayer::QTV_PLAYER_AUDIO_IXSTREAM_FAILED,
                                   myself->m_pQtvPlayer->CallbackClientData,ps);
            //Error occured, free video/audio/text URNs, if allocated.
            if(newVideoMediaURN!= NULL)
            {
              QTV_Free(newVideoMediaURN);
            }
            if(newAudioMediaURN)
            {
              QTV_Free(newAudioMediaURN);
            }
            if(newTextMediaURN)
            {
              QTV_Free(newTextMediaURN);
            }
             return;
          }
          /*
           * Create reference ixstream for text from audio stream only if
           * QtvIxStreamInfo[VIDEO_INDEX].pStream is NULL and text
           * is also from the same file as that of audio.
           */
          myself->m_bNeedAudioRefStream = false;

          if( (textURN) && (strcmp(textURN,audioURN) == 0) )
          {
            if(!myself->m_bNeedTextRefStream)
            {
              myself->m_bNeedTextRefStream = true;
              myself->QtvIxStreamInfo[TEXT_INDEX].referenceOfTrackType = QtvPlayer::AudioTrack;
            }
          }
        }
      }//if(newAudioMediaURN)

      if( (newTextMediaURN) && (!myself->m_bNeedTextRefStream) )
      {
       bool nRet = false;
#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
           nRet = (IxStream*)myself->RegisterContentToContMgr(newTextMediaURN,
                                                            QtvPlayer::TextTrack);
#endif
        myself->m_bNeedTextRefStream = false;

        if(!nRet)
         {
           //Error occured Report QTV_PLAYER_TEXT_IXSTREAM_FAILED and return
           QTV_MSG_PRIO(QTVDIAG_GENERAL,
          QTVDIAG_PRIO_LOW,
          "RegisterContentToContMgr for text failed.." );

          myself->NotifyCallback(QtvPlayer::QTV_PLAYER_TEXT_IXSTREAM_FAILED,
                                 myself->m_pQtvPlayer->CallbackClientData,ps);
          //Error occured, free video/audio/text URNs, if allocated.
          if(newVideoMediaURN!= NULL)
          {
            QTV_Free(newVideoMediaURN);
          }
          if(newAudioMediaURN)
          {
            QTV_Free(newAudioMediaURN);
          }
          if(newTextMediaURN)
          {
            QTV_Free(newTextMediaURN);
          }

           return;
        }
     }//if( (newTextMediaURN) && (!myself->m_bNeedTextRefStream) )
    }//if(bIsDCFMedia)

    else if(runThroughIxStream)
    {
      //User has configured to run unprotected content through IxStream

      //Create the IxStream for video
      if(newVideoMediaURN)
      {

        UxURI* UxURIPtr = QTV_New_Args( UxURI, ((const char*)newVideoMediaURN) );

        errorCode = streamFactory.CreateStream( UxURIPtr,
                                    &myself->QtvIxStreamInfo[VIDEO_INDEX].pStream,
                                    QtvPlayerDispatch::IxStreamCreateCallbackWrapper,
                                    &myself->IxStreamopenRspArray[VIDEO_INDEX]);

        if(UxURIPtr)
        {
          QTV_Delete(UxURIPtr);
        }


        if( (errorCode == E_SUCCESS )||(errorCode == E_AGAIN ) )
       {
         QTV_MSG_PRIO1(QTVDIAG_GENERAL,
          QTVDIAG_PRIO_MED,
          "Video:streamFactory->CreateStream successful??, errorcode =%d",
          errorCode);

          //Increment count of number of IxStreams created
          myself->m_nIxStreamsCreated++;
          myself->m_bNeedVideoRefStream = false;
          myself->QtvIxStreamInfo[VIDEO_INDEX].isStreamOpened = true;
          if(!bNeedSeparateAudioVideoIxStream)
          {
            /*
             * If user has specified non NULL audioURN, need to play the audio.
             * So,will need reference stream for audio as audio/video is from same file.
             */
            if(newAudioMediaURN)
            {
              myself->m_bNeedAudioRefStream = true;
              myself->QtvIxStreamInfo[AUDIO_INDEX].referenceOfTrackType = QtvPlayer::VideoTrack;
            }
            }//if(!bNeedSeparateAudioVideoIxStream)

          //Check to see if text is also to be played from the same file as of audio/video
          if(textURN)
          {
            //Don't need separate IxStream for text but rather reference IxStream will be
            //created from video/audio IxStream.
            if(videoURN && (strcmp(textURN,videoURN) == 0) )
            {
              myself->m_bNeedTextRefStream = true;
              myself->QtvIxStreamInfo[TEXT_INDEX].referenceOfTrackType = QtvPlayer::VideoTrack;
            }
          }
       }//createStream for video successful
       else
       {
         //Error occuredr,report QTV_PLAYER_VIDEO_IXSTREAM_FAILED and return
           QTV_MSG_PRIO1(QTVDIAG_GENERAL,
          QTVDIAG_PRIO_MED,
          "Video:streamFactory->CreateStream() failed:errorcode??? %d",
          errorCode);

          myself->QtvIxStreamInfo[VIDEO_INDEX].isStreamOpened = false;
          myself->NotifyCallback(QtvPlayer::QTV_PLAYER_VIDEO_IXSTREAM_FAILED,
                                 myself->m_pQtvPlayer->CallbackClientData,ps);
          //Error occured, free video/audio/text URNs, if allocated.
          if(newVideoMediaURN!= NULL)
          {
            QTV_Free(newVideoMediaURN);
          }
          if(newAudioMediaURN)
          {
            QTV_Free(newAudioMediaURN);
          }
          if(newTextMediaURN)
          {
            QTV_Free(newTextMediaURN);
          }
          return;
        }
     }//if(newVideoMediaURN)

      if(newAudioMediaURN)
      {
        if( ((!bNeedSeparateAudioVideoIxStream) && (myself->QtvIxStreamInfo[VIDEO_INDEX].pStream == NULL)) ||
            (bNeedSeparateAudioVideoIxStream) )
         {
          //Create IxStream for audio
          UxURI* UxURIPtr = QTV_New_Args( UxURI, ((const char*)newAudioMediaURN) );

           errorCode = streamFactory.CreateStream( UxURIPtr,
                                       &myself->QtvIxStreamInfo[AUDIO_INDEX].pStream,
                                       QtvPlayerDispatch::IxStreamCreateCallbackWrapper,
                                       &myself->IxStreamopenRspArray[AUDIO_INDEX]);

          if(UxURIPtr)
          {
            QTV_Delete(UxURIPtr);
          }

           if( (errorCode == E_SUCCESS )||(errorCode == E_AGAIN ) )
           {
             QTV_MSG_PRIO1(QTVDIAG_GENERAL,
            QTVDIAG_PRIO_MED,
            "AUDIO:streamFactory->CreateStream successful??, errorcode =%d",
            errorCode);

            myself->m_nIxStreamsCreated++;
            myself->QtvIxStreamInfo[AUDIO_INDEX].isStreamOpened = true;

            //Check to see if text is also to be played from the same file as of audio/video
            myself->m_bNeedAudioRefStream = false;

            if( (textURN) && (strcmp(textURN,audioURN) == 0) )
            {
              if(!myself->m_bNeedTextRefStream)
              {
                myself->m_bNeedTextRefStream = true;
                myself->QtvIxStreamInfo[TEXT_INDEX].referenceOfTrackType = QtvPlayer::AudioTrack;
              }
            }
           }
           else
           {
             //Error occured, report QTV_PLAYER_AUDIO_IXSTREAM_FAILED and return

            QTV_MSG_PRIO1(QTVDIAG_GENERAL,
            QTVDIAG_PRIO_MED,
            "AUDIO:streamFactory->CreateStream() failed:errorcode???? %d",
            errorCode);

            myself->NotifyCallback(QtvPlayer::QTV_PLAYER_AUDIO_IXSTREAM_FAILED,
                                   myself->m_pQtvPlayer->CallbackClientData,ps);
            myself->QtvIxStreamInfo[AUDIO_INDEX].isStreamOpened = false;
            //Error occured, free video/audio/text URNs, if allocated.
            if(newVideoMediaURN!= NULL)
            {
              QTV_Free(newVideoMediaURN);
            }
            if(newAudioMediaURN)
            {
              QTV_Free(newAudioMediaURN);
            }
            if(newTextMediaURN)
            {
              QTV_Free(newTextMediaURN);
            }
            return;
          }
         }
     }//if(newAudioMediaURN)

      if( (newTextMediaURN) && (!myself->m_bNeedTextRefStream) )
      {
        //Create IxStream for text
        UxURI* UxURIPtr = QTV_New_Args( UxURI, ((const char*)newTextMediaURN) );

         errorCode = streamFactory.CreateStream( UxURIPtr,
                                     &myself->QtvIxStreamInfo[TEXT_INDEX].pStream,
                                     QtvPlayerDispatch::IxStreamCreateCallbackWrapper,
                                     &myself->IxStreamopenRspArray[TEXT_INDEX]);
        if(UxURIPtr)
        {
          QTV_Delete(UxURIPtr);
        }

        myself->m_bNeedTextRefStream = false;

         if( (errorCode == E_SUCCESS )|| (errorCode == E_AGAIN ) )
         {
           QTV_MSG_PRIO1(QTVDIAG_GENERAL,
          QTVDIAG_PRIO_MED,
          "Text:streamFactory->CreateStream successful??, errorcode =%d",
          errorCode);

          myself->m_nIxStreamsCreated++;
          myself->QtvIxStreamInfo[TEXT_INDEX].isStreamOpened = true;
         }
         else
         {
           //Error occured, report QTV_PLAYER_TEXT_IXSTREAM_FAILED and return
          QTV_MSG_PRIO1(QTVDIAG_GENERAL,
          QTVDIAG_PRIO_MED,
          "TEXT:streamFactory->CreateStream() failed:errorcode???? %d",
          errorCode);

          myself->NotifyCallback(QtvPlayer::QTV_PLAYER_TEXT_IXSTREAM_FAILED,
                                 myself->m_pQtvPlayer->CallbackClientData,ps);
          myself->QtvIxStreamInfo[TEXT_INDEX].isStreamOpened = false;
          //Error occured, free video/audio/text URNs, if allocated.
          if(newVideoMediaURN!= NULL)
          {
            QTV_Free(newVideoMediaURN);
          }
          if(newAudioMediaURN)
          {
            QTV_Free(newAudioMediaURN);
          }
          if(newTextMediaURN)
          {
            QTV_Free(newTextMediaURN);
          }
           return;
        }
     }//if( (newTextMediaURN) && (!myself->m_bNeedTextRefStream) )
    }//else if(runThroughIxStream)

    //Free the memory allocated for filenames
    if(newVideoMediaURN!= NULL)
    {
      QTV_Free(newVideoMediaURN);
    }
    if(newAudioMediaURN)
    {
      QTV_Free(newAudioMediaURN);
    }
    if(newTextMediaURN)
    {
      QTV_Free(newTextMediaURN);
    }
  }//if(bIsLocalFile == true)
}
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::QTV_IXSTREAM_OPEN_EVENT_handler

DESCRIPTION
  Handler for QTV_IXSTREAM_OPEN_EVENT

DEPENDENCIES
  None

INPUT PARAMETERS:
->PV_IXSTREAM_OPEN_EVENT: An event containing IxStream* for audio/video/text and track selection mask.
<-: None.

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void QtvPlayerDispatch::QTV_IXSTREAM_OPEN_EVENT_handler(QtvPlayerDispatchMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL,
               QTVDIAG_PRIO_LOW,
               "QtvPlayerDispatch::QTV_IXSTREAM_OPEN_EVENT_handler" );

  dcf_ixstream_type ix_stream_type = NULL;

  bool bPlayVideo = true;
  bool bPlayAudio = true;
  bool bPlayText  = true;

  if(!handle)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_LOW,
                 "QTV_IXSTREAM_OPEN_EVENT_handler found NULL handle!!" );

    return;
  }
  QTV_IXSTREAM_OPEN_EVENT_type *pStreamEvent =
  (QTV_IXSTREAM_OPEN_EVENT_type*)handle;

  ix_stream_type = pStreamEvent->pStream;
  if(pStreamEvent->DispatchHandle == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_LOW,
                 "QTV_IXSTREAM_OPEN_EVENT_handler found NULL DispatchHandle!!" );
    return;
  }
  QtvPlayerDispatch* myself = (QtvPlayerDispatch*)pStreamEvent->DispatchHandle;
  QtvPlayer::PlayerStateRecordT ps;
  QtvPlayer::GetPlayerState(ps, (QtvPlayer::InstanceHandleT)myself->m_pQtvPlayer);


  //check for Non NULL.
  if(!ix_stream_type)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_MED,
                 "QTV_IXSTREAM_OPEN_EVENT_handler got NULL ix_stream ");

    myself->NotifyCallback(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED,
                           myself->m_pQtvPlayer->CallbackClientData,ps);

    return ;
  }

  /*
  * DetermineContentMimeType reads first few bytes from IxStream
  * to determine content mime type and
  * sets CurrentMIMEType accordingly. If the the mime type is not REAL/QTV,
  * returns false otherwise returns true.
  */
  if(myself->DetermineContentMimeType(ix_stream_type))
  {
    /*
    * DetermineContentMimeType(..) returns True only when mime type detected
    * is REAL/3GP/WM, otherwise returns False.
    */

    //CurrentMIMEType is set by DetermineContentMimeType
    if(strcmp(myself->m_pQtvPlayer->CurrentMIMEType,QTV_PLAYER_MIME_TYPE)==0)
    {
      /*
       * Do the track selection based on values provided by the user
       */
      if(!(pStreamEvent->dwTrackSelected & QTV_VIDEO_TRACK_SELECTED))
      {
        //Don't play Video
        bPlayVideo = false;
      }
      if(!(pStreamEvent->dwTrackSelected & QTV_AUDIO_TRACK_SELECTED))
      {
        //Don't play Audio
        bPlayAudio = false;
      }
      if(!(pStreamEvent->dwTrackSelected & QTV_TEXT_TRACK_SELECTED))
      {
        //Don't play Text
        bPlayText = false;
      }
      //Make sure at least one of the track has been selected
      if( (bPlayVideo) || (bPlayAudio) || (bPlayText) )
      {
        //We have already received an IxStream, so set our counters to 1.
        //If we end up creating more references, counters will be adjusted accordingly later on.
        myself->m_nIxStreamSuccessfulCallbacks = myself->m_nIxStreamsCreated = 1;
      }

      /*
      * Determine whether we need to create reference IxStream from ix_stream_type
      * based on the track selction made above.
      */
      if(bPlayVideo)
      {
        myself->QtvIxStreamInfo[VIDEO_INDEX].pStream = (IxStream*)ix_stream_type;
        if(bPlayAudio)
        {
          //User wants to play the audio;We need to create reference
          //IxStream from QtvIxStreamInfo[VIDEO_INDEX].pStream
          //to play audio and video both. This is needed because
          //of IxStream code is not reentrant.
          myself->m_bNeedAudioRefStream = true;
          myself->QtvIxStreamInfo[AUDIO_INDEX].referenceOfTrackType = QtvPlayer::VideoTrack;
        }
        if(bPlayText)
        {
          //User wants to play the text;We need to create reference
          //IxStream from QtvIxStreamInfo[VIDEO_INDEX].pStream
          //to play text and video both. This is needed because of
          //IxStream code is not reentrant.
          myself->m_bNeedTextRefStream = true;
          myself->QtvIxStreamInfo[TEXT_INDEX].referenceOfTrackType = QtvPlayer::VideoTrack;
        }
        myself->m_bNeedVideoRefStream = false;
      }//if(bPlayVideo)

      else if(bPlayAudio)
      {
        myself->QtvIxStreamInfo[AUDIO_INDEX].pStream = (IxStream*)ix_stream_type;
        myself->m_bNeedAudioRefStream = false;
        if(bPlayText)
        {
          //User wants to play the text;We need to create reference
          //IxStream fromQtvIxStreamInfo[AUDIO_INDEX]
          //to play audio and text both.
          //This is needed because of IxStream code is not reentrant.
          myself->m_bNeedTextRefStream = true;
          myself->QtvIxStreamInfo[TEXT_INDEX].referenceOfTrackType = QtvPlayer::AudioTrack;
        }
      }
      else if(bPlayText)
      {
        myself->QtvIxStreamInfo[TEXT_INDEX].pStream = (IxStream*)ix_stream_type;
        myself->m_bNeedVideoRefStream = false;
        myself->m_bNeedAudioRefStream = false;
        myself->m_bNeedTextRefStream  = false;
      }

      /*
      * See if we need to create reference IxStreams, if yes,
      * invoke CreateReferenceIxStreams to create reference IxStreams.
      * When all references are created successfully,
      * QtvPlayerDispatch::StartProcessingFromIxStream gets called.
      * Based on content MIME type, it will route to Mpeg4Player/RealPlayer.
      */
      if(myself->m_bNeedVideoRefStream ||
         myself->m_bNeedAudioRefStream ||
         myself->m_bNeedTextRefStream)
      {
        myself->CreateReferenceIxStreams();
      }
      //If all IxStreams are ready, kick the processing.
      if( (myself->m_nIxStreamsCreated) && (myself->m_nIxStreamSuccessfulCallbacks == myself->m_nIxStreamsCreated) )
      {
        myself->StartProcessingFromIxStream(ix_stream_type);
      }
    }//if(strcmp(CurrentMIMEType,QTV_PLAYER_MIME_TYPE)==0)

    /*
    * We don't need reference IxStream for REAL.
    */
    #ifdef FEATURE_REAL_PLAYER_USE_QTV_API
    else if(strcmp(myself->m_pQtvPlayer->CurrentMIMEType,REAL_MEDIA_MIME_TYPE)==0)
    {
      //MimeType is REAL, route to RealPlayer
      if( strcmp(myself->m_pQtvPlayer->CurrentMIMEType,REAL_MEDIA_MIME_TYPE)==0 )
      {
        if(!(pStreamEvent->dwTrackSelected & QTV_AUDIO_TRACK_SELECTED))
        {
          //User doesn't want to play AUDIO
          OEMMediaReal_DisableAudio(TRUE);
        }
        else
        {
          OEMMediaReal_DisableAudio(FALSE);
        }
      }//if( strcmp(QtvPlayer::CurrentMIMEType,REAL_MEDIA_MIME_TYPE)==0 )

      QTV_MSG_PRIO(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      "QTV_IXSTREAM_OPEN_EVENT_handler no reference IxStreams needed,invoking QtvRealPlayer_OpenIxStream");

      uint32 size = 0;
      if( ( (IxStream*)ix_stream_type )->IsProgressive() )
      {
        size = 0x7FFFFFFF;
      }
      else
      {
        ((IxStream*)ix_stream_type)->Size(&size);
      }
      QtvRealPlayer_OpenIxStream( ix_stream_type,(unsigned long)size);
      return;
      }
    #endif//#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  }//if(QtvPlayerDispatch::DetermineContentMimeType(ix_stream))
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    "MimeType not recognized..");

    myself->NotifyCallback(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED,
                           myself->m_pQtvPlayer->CallbackClientData,ps);
  }
}
#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::IxCallBackWrapper

DESCRIPTION
  Callback function from IxContentManager. This callback is specified when registering
  content with ContentManager.

INPUT/OUTPUT:

 ->IxCallback* pcb: Ixcallback* provided in asynchronous call to DRM suite. @See "IxcallbackDef.h"
 <-: None.

DEPENDENCIES
 @see Include files above.

RETURN VALUE
  None

SIDE EFFECTS

========================================================================== */
void QtvPlayerDispatch::IxCallBackWrapper( IxCallback* pcb )
{
   //Variables to keep track of instance(a/v/t) to which current callback invokation belongs.
   bool isAudio = false;
   bool isVideo = false;
   bool isText = false;

   /*
   * Variables to keep track of whether we are currently processing 'commot' callbacks for
   * a/v/t instance respectively.
   */
   bool isAudioCommitBeingProcessed = false;
   bool isVideoCommitBeingProcessed = false;
   bool isTextCommitBeingProcessed  = false;

   IxContentDef::UriT           uri;
   TxList<IxContent*>           contentItr;
   IxErrnoType                  nErr;

   if(!pcb)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL,
     QTVDIAG_PRIO_HIGH,
     "IxCallBackWrapper:FATAL Error:pcb IS NULL");
     return;
   }
   QTV_MSG_PRIO1(QTVDIAG_GENERAL,
   QTVDIAG_PRIO_MED,
   " IxCallBackWrapper pcb = %x",pcb);

   if( (pcb->AEECallback.pUserData) &&
       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData) )
   {
     if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->trackType == QtvPlayer::VideoTrack)
     {
       isVideo = true;
     }
     if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->trackType == QtvPlayer::AudioTrack)
     {
       isAudio = true;
     }
     if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->trackType == QtvPlayer::TextTrack)
     {
       isText = true;
     }
   }

   if( !(isAudio || isVideo || isText) )
   {
     //Something is wrong, as pcb didn't match with any of
     //the IxCallback for audio/video/text.
     QTV_MSG_PRIO(QTVDIAG_GENERAL,
     QTVDIAG_PRIO_HIGH,
     "IxCallBackWrapper:error:pcb didn't MATCH any IxCallback for a/v/t.");
     return;
   }

   if( (pcb) && (pcb->AEECallback.pUserData) && (pcb->NotifyStatus == E_SUCCESS) )
   {
     if( (isVideo) && ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked)
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL,
                    QTVDIAG_PRIO_MED,
                    " Ixcallbackwrapper for VIDEO COMMIT");

       /*
       * This is callback for 'commit' on video instance IxStream.
       * Now need to invoke GetObject to get IxMedia which will give us IxStream
       */
       isVideoCommitBeingProcessed = true;

       uri = ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->uri;

       IxContentBrowserClient* ixContentBrowserClientPtr = QTV_New(IxContentBrowserClient);
       if(ixContentBrowserClientPtr == NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
                      QTVDIAG_PRIO_HIGH,
                      "IxCallbackWrapper:ixContentBrowserClientPtr is NULL");
         return;
       }
       IxContentInfo* ixContentInfoPtr = NULL;
       nErr = ixContentBrowserClientPtr->FindContent(&uri,&ixContentInfoPtr);
       if( (ixContentInfoPtr == NULL)||(nErr != E_SUCCESS) )
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
                      QTVDIAG_PRIO_HIGH,
                      "IxCallbackWrapper:ixContentInfoPtr is NULL");
         if(ixContentBrowserClientPtr!= NULL)
         {
           QTV_Delete(ixContentBrowserClientPtr);
           ixContentBrowserClientPtr = NULL;
         }
         return;
       }
       IxContentClient* IxContentClientPtr = new IxContentClient(&uri,ixContentInfoPtr);
       if( IxContentClientPtr == NULL )
       {
         if(ixContentBrowserClientPtr!= NULL)
         {
           QTV_Delete(ixContentBrowserClientPtr);
           ixContentBrowserClientPtr = NULL;
         }

         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,
         "ixCallbackWrapper:IxContentClientPtr is NULL");

         return;
       }
       nErr = IxContentClientPtr->GetObject(IxDRMPermission::PERM_PLAY,
                                            &( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->QtvIxStreamInfo[VIDEO_INDEX].ixMediaHandle),
                                            &( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->IxStreamIxCallbacksArray[VIDEO_INDEX]) );
       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_MED,
       " GetObject INvoked for VIDEO");

       //Reset waitingForCommit flag.
       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked = false;

       if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pContentMgrClient )
       {
         QTV_Delete( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pContentMgrClient );
         ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pContentMgrClient = NULL;
       }
       if(ixContentBrowserClientPtr!= NULL)
       {
         QTV_Delete(ixContentBrowserClientPtr);
         ixContentBrowserClientPtr = NULL;
       }
       if(IxContentClientPtr!= NULL)
       {
         QTV_Delete(IxContentClientPtr);
         IxContentClientPtr = NULL;
       }

       if(nErr != E_SUCCESS)
       {
         QTV_MSG_PRIO1(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"IxCallbackWrapper:IxContentClientPtr->GetObject..errorCode %d",
         nErr);

         return;
       }
       return;
     }//if( (isVideo) && ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked)

     else if( (isAudio) && ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked)
     {
       /*
       * This is callback for 'commit' on audio instance IxStream.
       * Now need to invoke GetObject to get IxMedia which will give us IxStream
       */
       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_MED," Ixcallbackwrapper for AUDIO COMMIT");

       isAudioCommitBeingProcessed = true;

       uri = ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->uri;

       IxContentBrowserClient* ixContentBrowserClientPtr = QTV_New(IxContentBrowserClient);
       if(ixContentBrowserClientPtr == NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"IxCallbackWrapper:ixContentBrowserClientPtr is NULL");

         return;
       }
       IxContentInfo* ixContentInfoPtr = NULL;
       nErr = ixContentBrowserClientPtr->FindContent(&uri,&ixContentInfoPtr);
       if( (ixContentInfoPtr == NULL)||(nErr != E_SUCCESS) )
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"IxCallbackWrapper:ixContentInfoPtr is NULL");
         if(ixContentBrowserClientPtr!= NULL)
         {
           QTV_Delete(ixContentBrowserClientPtr);
           ixContentBrowserClientPtr = NULL;
         }
         return;
       }
       IxContentClient* IxContentClientPtr = new IxContentClient(&uri,ixContentInfoPtr);
       if( IxContentClientPtr == NULL )
       {
         if(ixContentBrowserClientPtr!= NULL)
         {
           QTV_Delete(ixContentBrowserClientPtr);
           ixContentBrowserClientPtr = NULL;
         }
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"ixCallbackWrapper:IxContentClientPtr is NULL");
         return;
       }
       nErr = IxContentClientPtr->GetObject(IxDRMPermission::PERM_PLAY,
                                            &( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->QtvIxStreamInfo[AUDIO_INDEX].ixMediaHandle),
                                            &( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->IxStreamIxCallbacksArray[AUDIO_INDEX]) );

       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_MED,
       " GetObject INvoked for AUDIO");

       //Reset waitingForCommit flag.
       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked = false;

       if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pContentMgrClient )
       {
         QTV_Delete( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pContentMgrClient );
         ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pContentMgrClient = NULL;
       }
       if(ixContentBrowserClientPtr!= NULL)
       {
         QTV_Delete(ixContentBrowserClientPtr);
         ixContentBrowserClientPtr = NULL;
       }
       if(IxContentClientPtr!= NULL)
       {
         QTV_Delete(IxContentClientPtr);
         IxContentClientPtr = NULL;
       }

       if(nErr != E_SUCCESS)
       {
         QTV_MSG_PRIO1(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,
         "IxCallbackWrapper:IxContentClientPtr->GetObject..errorCode %d",nErr);
         return;
       }
       return;
     }//else if( (isAudio) && ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked)

     else if( (isText) && ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked)
     {
       /*
       * This is callback for 'commit' on text instance IxStream.
       * Now need to invoke GetObject to get IxMedia which will give us IxStream
       */
       isTextCommitBeingProcessed = true;
       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_MED," Ixcallbackwrapper for TEXT COMMIT");

       uri = ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->uri;

       IxContentBrowserClient* ixContentBrowserClientPtr = QTV_New(IxContentBrowserClient);
       if(ixContentBrowserClientPtr == NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"IxCallbackWrapper:ixContentBrowserClientPtr is NULL");
         return;
       }
       IxContentInfo* ixContentInfoPtr = NULL;
       nErr = ixContentBrowserClientPtr->FindContent(&uri,&ixContentInfoPtr);
       if( (ixContentInfoPtr == NULL)||(nErr != E_SUCCESS) )
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"IxCallbackWrapper:ixContentInfoPtr is NULL");
         if(ixContentBrowserClientPtr!= NULL)
         {
           QTV_Delete(ixContentBrowserClientPtr);
           ixContentBrowserClientPtr = NULL;
         }
         return;
       }
       IxContentClient* IxContentClientPtr = new IxContentClient(&uri,ixContentInfoPtr);
       if( IxContentClientPtr == NULL )
       {
         if(ixContentBrowserClientPtr!= NULL)
         {
           QTV_Delete(ixContentBrowserClientPtr);
           ixContentBrowserClientPtr = NULL;
         }
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"ixCallbackWrapper:IxContentClientPtr is NULL");
         return;
       }
       nErr = IxContentClientPtr->GetObject(IxDRMPermission::PERM_PLAY,
                                            &( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->QtvIxStreamInfo[TEXT_INDEX].ixMediaHandle),
                                            &( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->IxStreamIxCallbacksArray[TEXT_INDEX]) );

       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," GetObject INvoked for TEXT");
       //Reset waitingForCommit flag.
       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked = false;

       if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pContentMgrClient )
       {
         QTV_Delete( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pContentMgrClient );
         ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pContentMgrClient = NULL;
       }
       if(ixContentBrowserClientPtr!= NULL)
       {
         QTV_Delete(ixContentBrowserClientPtr);
         ixContentBrowserClientPtr = NULL;
       }
       if(IxContentClientPtr!= NULL)
       {
         QTV_Delete(IxContentClientPtr);
         IxContentClientPtr = NULL;
       }

       if(nErr != E_SUCCESS)
       {
         QTV_MSG_PRIO1(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"IxCallbackWrapper:IxContentClientPtr->GetObject..errorCode %d",nErr);
         return;
       }
       return;
     }//else if( (isText) && ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked)

     else if( (isVideo)                                              &&
              (! ( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked) )&&
              (isVideoCommitBeingProcessed == false) )
     {

       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," Ixcallbackwrapper for VIDEO GetObject");
       if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->ixMediaHandle == NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"ixMediaHandle NULL for Video..");
         return;
       }
       if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->ixMediaHandle->DataStream == NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"ixMediaHandle->DataStream NULL for Video..");
         return;
       }
       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pStream = ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->ixMediaHandle->DataStream;

       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_HIGH,"Callback for Video OK, VideoIxStream Ready");

       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isStreamReadyforIO = true;
        ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_nIxStreamSuccessfulCallbacks++;
     }
     else if( (isAudio)                                              &&
              (! ( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked) )&&
              (isAudioCommitBeingProcessed == false) )
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_MED," Ixcallbackwrapper for AUDIO GetObject");

       if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->ixMediaHandle == NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"ixMediaHandle NULL for Video..");
         return;
       }
       if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->ixMediaHandle->DataStream == NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"ixMediaHandle->DataStream NULL for Video..");
         return;
       }

       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_HIGH,"Callback for Audio OK, AudioIxStream Ready");

       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pStream = ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->ixMediaHandle->DataStream;
       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isStreamReadyforIO = true;
        ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_nIxStreamSuccessfulCallbacks++;
     }
      else if( (isText)                                              &&
              (! ( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isWaitingForCommitAcked) )&&
              (isTextCommitBeingProcessed == false) )
     {

       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_MED," Ixcallbackwrapper for TEXT GetObject");

       if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->ixMediaHandle == NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"ixMediaHandle NULL for Video..");
         return;
       }
       if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->ixMediaHandle->DataStream == NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
         QTVDIAG_PRIO_HIGH,"ixMediaHandle->DataStream NULL for Video..");
         return;
       }
       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_HIGH,"Callback for Text OK, TextIxStream Ready");

       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pStream = ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->ixMediaHandle->DataStream;
       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->isStreamReadyforIO = true;
        ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_nIxStreamSuccessfulCallbacks++;
     }

    /*
    * If reference streams are to be created, invoke CreateReferenceIxStreams to create them.
    */
    int nStreamsCreated = ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_nIxStreamsCreated;
    int nStreamsAcked = ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_nIxStreamSuccessfulCallbacks;

    if( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->DetermineContentMimeType( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pStream) )
    {
      if(strcmp(((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_pQtvPlayer->CurrentMIMEType,REAL_MEDIA_MIME_TYPE)==0)
      {
        //MimeType is REAL, don't need reference IxStreams
        ( (QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_bNeedAudioRefStream = false;
        ( (QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_bNeedVideoRefStream = false;
        ( (QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_bNeedTextRefStream  = false;

        QTV_MSG_PRIO(QTVDIAG_GENERAL,
        QTVDIAG_PRIO_MED,
        " IxCallbackWrapper,REAL MEDIA, resetting all create reference IxStreams flags..");
        }
    }//if(((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->DetermineContentMimeType(pstream))

    QTV_MSG_PRIO2(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " IxCallbackWrapper ,nStreamsAcked =%d nStreamsCreated= %d ",nStreamsAcked,nStreamsCreated);

    if(((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_bNeedAudioRefStream ||
       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_bNeedVideoRefStream ||
       ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_bNeedTextRefStream)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " IxCallbackWrapper,calling CreateReferenceIxStreams");

      ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->CreateReferenceIxStreams();
    }

     else if( ( nStreamsAcked == nStreamsCreated ) &&
             ( nStreamsCreated > 0)               &&
             (!(((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_bNeedAudioRefStream ||
               ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_bNeedVideoRefStream  ||
               ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->m_bNeedTextRefStream)
             )
           )
     {
       /*
       * We are done with IxStream/reference IxStream creation, start processing now..
       */
        dcf_ixstream_type pStream = (dcf_ixstream_type)( ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->QtvIxStreamInfo[VIDEO_INDEX].pStream ?
                                                        ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->QtvIxStreamInfo[VIDEO_INDEX].pStream :
                                                        ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->QtvIxStreamInfo[AUDIO_INDEX].pStream);
       QTV_MSG_PRIO(QTVDIAG_GENERAL,
       QTVDIAG_PRIO_MED,
       " IxCallbackWrapper,calling StartProcessingFromIxStream");

        ((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->StartProcessingFromIxStream(pStream);
     }
   }
   else
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL,
     QTVDIAG_PRIO_HIGH,
     "pcb->AEECallback || pcb->AEECallback.pNotifyData is NULL || pcb->NotifyStatus != E_SUCCESS");
   }
}//IxCallBackWrapper( IxCallback* pcb )
#endif//#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT

/* ======================================================================
FUNCTION
  QtvPlayerDispatch::CreateRefIxStreamCallbackWrapper

DESCRIPTION
  Wrapper callback specified when creating reference IxStream
  by IxStream->CreateRef.

INPUT/OUTPUT:
 ->void* user: Callback structures provided in asynchronous DRM suite call.
               Callback structures that can be specifed for this type of callback is
               IxCallback. @See "IxStream.h" for details.

 <-: None.

DEPENDENCIES
 @see Include files above.

RETURN VALUE
  None

SIDE EFFECTS
========================================================================== */
void QtvPlayerDispatch::CreateRefIxStreamCallbackWrapper(void* user)
{
  bool isAudio = false;
  bool isVideo = false;
  bool isText =  false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL,
  QTVDIAG_PRIO_HIGH,
  "QtvPlayerDispatch::CreateRefIxStreamCallbackWrapper");

  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
  QTVDIAG_PRIO_MED,
  " QtvPlayerDispatch::CreateRefIxStreamCallbackWrapper, (void* user)= %x",user);

  if(user == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "CreateRefIxStreamCallbackWrapper received NULL userdata");
    return;
  }

  if( ((IxCallback*)user)->AEECallback.pUserData == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "CreateRefIxStreamCallbackWrapper extracted NULL pUserData");
    return;
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
  QTVDIAG_PRIO_MED,
  " CreateRefIxStreamCallbackWrapper, ((QtvIxStreamInfoT*)((IxCallback*)user)->AEECallback.pUserData)= %x",
  (((QtvIxStreamInfoT*)((IxCallback*)user)->AEECallback.pUserData)));

  if(  ((QtvIxStreamInfoT*)((IxCallback*)user)->AEECallback.pUserData)->trackType == QtvPlayer::VideoTrack)
  {
    isVideo = true;
  }
  else if(  ((QtvIxStreamInfoT*)((IxCallback*)user)->AEECallback.pUserData)->trackType == QtvPlayer::AudioTrack)
  {
    isAudio = true;
  }
  else if(  ((QtvIxStreamInfoT*)((IxCallback*)user)->AEECallback.pUserData)->trackType == QtvPlayer::TextTrack)
  {
    isText = true;
  }

  QtvPlayerDispatch* myself = ((QtvIxStreamInfoT*)((IxCallback*)user)->AEECallback.pUserData)->pMySelf;
  QtvPlayer::PlayerStateRecordT ps;
  QtvPlayer::GetPlayerState(ps, (QtvPlayer::InstanceHandleT)myself->m_pQtvPlayer);
  if(!myself)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_FATAL,
                 "CreateRefIxStreamCallbackWrapper myself is NULL..");
    return;
  }

  if(((IxCallback*)user)->NotifyStatus == E_SUCCESS)
  {
    if(myself == NULL)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_HIGH,
      "CreateRefIxStreamCallbackWrapper extracted NULL myself");
      return;
    }

    //Increment count of successful streams acknowledged
    myself->m_nIxStreamSuccessfulCallbacks++;

    if(isVideo)
    {
      myself->QtvIxStreamInfo[VIDEO_INDEX].isStreamReadyforIO = true;
    }
    if(isAudio)
    {
      myself->QtvIxStreamInfo[AUDIO_INDEX].isStreamReadyforIO = true;
    }
    if(isText)
    {
      myself->QtvIxStreamInfo[TEXT_INDEX].isStreamReadyforIO = true;
    }

    QTV_MSG_PRIO2(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " CreateRefIxStreamCallbackWrapper ,m_nIxStreamSuccessfulCallbacks =%d m_nIxStreamsCreated= %d ",
    myself->m_nIxStreamSuccessfulCallbacks,myself->m_nIxStreamsCreated);

    if(myself->m_bNeedAudioRefStream ||
       myself->m_bNeedVideoRefStream ||
       myself->m_bNeedTextRefStream)
    {
      myself->CreateReferenceIxStreams();
    }

    else if( (myself->m_nIxStreamSuccessfulCallbacks == myself->m_nIxStreamsCreated) &&
             (myself->m_nIxStreamsCreated > 0)                                       &&
             (!(myself->m_bNeedAudioRefStream ||
                myself->m_bNeedVideoRefStream ||
                myself->m_bNeedTextRefStream)
             ) )
    {
       dcf_ixstream_type pStream =  (dcf_ixstream_type)(myself->QtvIxStreamInfo[VIDEO_INDEX].pStream ?
                                                       myself->QtvIxStreamInfo[VIDEO_INDEX].pStream :
                                                       myself->QtvIxStreamInfo[AUDIO_INDEX].pStream);
       myself->StartProcessingFromIxStream(pStream);
    }
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "$$$CreateRefIxStreamCallbackWrapper status != E_SUCCESS,errorCode= %d",
    (((IxCallback*)user)->NotifyStatus));

    myself->m_nIxStreamFailedCallbacks++;
    if( (myself->m_nIxStreamSuccessfulCallbacks +
        myself->m_nIxStreamFailedCallbacks)     ==  myself->m_nIxStreamsCreated)
    {
       /*
       * We are done with all callbacks for each of the stream created
       * via PV_OPEN_CONTENT_URN_handler. At this point,
       * we don't know which IxStream failed(unless callbacks come in the same
       * same order as of their creation). Just report an error and return.
       *
       * We could just return from here as soon as we get first
       * unsuccessful callback. Waiting for all of them might help to
       * identify how many actually failed.
       */
      myself->NotifyCallback(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED,
                             myself->m_pQtvPlayer->CallbackClientData,ps);
       return;
    }
  }
}//CreateRefIxStreamCallbackWrapper
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::IxStreamCreateCallbackWrapper

DESCRIPTION
  Wrapper callback specified when creating IxStream via StreamFactory::CreateStream
  or when creating reference IxStreams by IxStream->CreateRef.

INPUT/OUTPUT:
 ->void* user: Callback structures provided in asynchronous DRM suite call.
               Callback structures that can be specifed for this type of callback are
               IxStreamOpenRspT or IxCallback. @See "IxStream.h" for details.

 <-: None.

DEPENDENCIES
 @see Include files above.

RETURN VALUE
  None

SIDE EFFECTS
========================================================================== */

void QtvPlayerDispatch::IxStreamCreateCallbackWrapper(void* user)
{
  bool isAudio = false;
  bool isVideo = false;
  bool isText =  false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL,
  QTVDIAG_PRIO_HIGH,
  "QtvPlayerDispatch::IxStreamCreateCallbackWrapper");

  if(user == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "IxStreamCreateCallbackWrapper received NULL userdata");
    return;
  }
  IxStreamOpenRspT* createStreamCallbackPtr = (IxStreamOpenRspT*)user;
  if(createStreamCallbackPtr->pUserData == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "IxStreamCreateCallbackWrapper received NULL createStreamCallbackPtr->pUserData");
    return;
  }
  QtvIxStreamInfoT* qtvIxStreaminfoPtr = (QtvIxStreamInfoT*)createStreamCallbackPtr->pUserData;
  QtvPlayerDispatch* myself = qtvIxStreaminfoPtr->pMySelf;

  if(myself == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "IxStreamCreateCallbackWrapper extracted NULL myself");
    return;
  }

  if(qtvIxStreaminfoPtr->trackType == QtvPlayer::VideoTrack)
  {
    isVideo = true;
  }
  else if(qtvIxStreaminfoPtr->trackType == QtvPlayer::AudioTrack)
  {
    isAudio = true;
  }
  else if(qtvIxStreaminfoPtr->trackType == QtvPlayer::TextTrack)
  {
    isText = true;
  }
  QtvPlayer::PlayerStateRecordT ps;
  QtvPlayer::GetPlayerState(ps, (QtvPlayer::InstanceHandleT)myself->m_pQtvPlayer);

  if(createStreamCallbackPtr->error == E_SUCCESS)
  {
    //Increment count of successful streams acknowledged
    myself->m_nIxStreamSuccessfulCallbacks++;

    if(isVideo)
    {
      myself->QtvIxStreamInfo[VIDEO_INDEX].isStreamReadyforIO = true;
    }
    if(isAudio)
    {
      myself->QtvIxStreamInfo[AUDIO_INDEX].isStreamReadyforIO = true;
    }
    if(isText)
    {
      myself->QtvIxStreamInfo[TEXT_INDEX].isStreamReadyforIO = true;
    }

    if( myself->DetermineContentMimeType(qtvIxStreaminfoPtr->pStream) )
    {
      if(strcmp(myself->m_pQtvPlayer->CurrentMIMEType,REAL_MEDIA_MIME_TYPE)==0)
      {
        //MimeType is REAL, don't need reference IxStreams
        myself->m_bNeedVideoRefStream = false;
        myself->m_bNeedAudioRefStream = false;
        myself->m_bNeedTextRefStream   = false;

        QTV_MSG_PRIO(QTVDIAG_GENERAL,
        QTVDIAG_PRIO_MED,
        " IxStreamCreateCallbackWrapper,REAL MEDIA, resetting all create reference IxStreams flags..");
        }
    }//if(((QtvIxStreamInfoT*)pcb->AEECallback.pUserData)->pMySelf->DetermineContentMimeType(pstream))

    QTV_MSG_PRIO2(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " IxStreamCreateCallbackWrapper,m_nIxStreamSuccessfulCallbacks =%d m_nIxStreamsCreated= %d ",
    myself->m_nIxStreamSuccessfulCallbacks,myself->m_nIxStreamsCreated);

    if(myself->m_bNeedAudioRefStream ||
       myself->m_bNeedVideoRefStream ||
       myself->m_bNeedTextRefStream)
    {
      myself->CreateReferenceIxStreams();
    }

    else if( (myself->m_nIxStreamSuccessfulCallbacks == myself->m_nIxStreamsCreated) &&
             (myself->m_nIxStreamsCreated > 0)                                             &&
             (!(myself->m_bNeedAudioRefStream ||
                myself->m_bNeedVideoRefStream ||
                myself->m_bNeedTextRefStream)
             ) )
    {
       dcf_ixstream_type pStream =  (dcf_ixstream_type)(myself->QtvIxStreamInfo[VIDEO_INDEX].pStream ?
                                                       myself->QtvIxStreamInfo[VIDEO_INDEX].pStream :
                                                       myself->QtvIxStreamInfo[AUDIO_INDEX].pStream);
       myself->StartProcessingFromIxStream(pStream);
    }
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "$$$IxStreamCreateCallbackWrapper status != E_SUCCESS,errorCode= %d",
    createStreamCallbackPtr->error);

    myself->m_nIxStreamFailedCallbacks++;
    if( (myself->m_nIxStreamSuccessfulCallbacks +
        myself->m_nIxStreamFailedCallbacks)     ==  myself->m_nIxStreamsCreated)
    {
       /*
       * We are done with all callbacks for each of the stream created
       * via PV_OPEN_CONTENT_URN_handler. At this point,
       * we don't know which IxStream failed(unless callbacks come in the same
       * same order as of their creation). Just report an error and return.
       *
       * We could just return from here as soon as we get first
       * unsuccessful callback. Waiting for all of them might help to
       * identify how many actually failed.
       */
      myself->NotifyCallback(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED,
                             myself->m_pQtvPlayer->CallbackClientData,ps);
       return;
    }
  }
}//IxStreamCreateCallbackWrapper

#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::isContentInstalledInContentDataBase

DESCRIPTION
 If the content is already installed in the content database, following routine returns
 true otherwise, returns false. Before trying to register the conetnt, we should invoke
 following to see if it is already installed in the database.

 Please @see IxContentMgrClient and IxContent.h for more information.

INPUT/OUTPUT:

 ->IxContentDef::UriT* uri: URI identifying the content
 ->QtvPlayer::TrackType trackType: a/v/t instance to be associated with this invokation.

 <-true|false: True if content is already exists in database, otherwise, returns false.

DEPENDENCIES
 Please @see IxContentBrowserClient.h and IxContentInfo.h for more information.

RETURN VALUE
  Valid Content info object or NULL;

SIDE EFFECTS
========================================================================== */
bool QtvPlayerDispatch::isContentInstalledInContentDataBase(IxContentDef::UriT* uri,
                                                            QtvPlayer::TrackType trackType)
{
  IxErrnoType  errorCode = E_FAILURE;
  IxContentMgrClient contMgrClient;
  TxList<IxContent*>           contentItr;
  bool isVideo = false;
  bool isAudio = false;
  bool isText  = false;
  bool nRet = false;
  IxContentInfo* ixContentInfoPtr = NULL;
  IxContentBrowserClient* ixContentBrowserClientPtr = NULL;
  IxContentClient* IxContentClientPtr = NULL;
  IxContentAttrDef* attr_typePtr = NULL;
  IxContentDef::UrlT           url;

  //check to make sure we have non null URI.
  if(uri == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "isContentInstalledInContentDataBase:ERROR:NULL URI..");
    return nRet;
  }
  ixContentBrowserClientPtr = QTV_New(IxContentBrowserClient);
  if(ixContentBrowserClientPtr == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "isContentInstalledInContentDataBase:ixContentBrowserClientPtr is NULL");
    return nRet;
  }
  errorCode = ixContentBrowserClientPtr->FindContent(uri,&ixContentInfoPtr);
  if( (ixContentInfoPtr == NULL)||(errorCode != E_SUCCESS) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "isContentInstalledInContentDataBase:ixContentInfoPtr is NULL");
    if(ixContentBrowserClientPtr!= NULL)
    {
      QTV_Delete(ixContentBrowserClientPtr);
      ixContentBrowserClientPtr = NULL;
    }
    return nRet;
  }

  IxContentClientPtr = new IxContentClient(uri,ixContentInfoPtr);
  if( IxContentClientPtr == NULL )
  {
    if(ixContentBrowserClientPtr!= NULL)
    {
      QTV_Delete(ixContentBrowserClientPtr);
      ixContentBrowserClientPtr = NULL;
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "isContentInstalledInContentDataBase:IxContentClientPtr is NULL");
    return nRet;
  }

  attr_typePtr = new IxContentAttrDef(CONTENTATTR_TYPE,(void*)&url);

  if( attr_typePtr == NULL )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "isContentInstalledInContentDataBase:attr_typePtr is NULL");

    if(ixContentBrowserClientPtr!= NULL)
    {
      QTV_Delete(ixContentBrowserClientPtr);
      ixContentBrowserClientPtr = NULL;
    }
    if(IxContentClientPtr!= NULL)
    {
      QTV_Delete(IxContentClientPtr);
      IxContentClientPtr = NULL;
    }
    return nRet;
  }
  errorCode = IxContentClientPtr->GetAttr(attr_typePtr);
  if(errorCode != E_SUCCESS)
  {
    if(ixContentBrowserClientPtr!= NULL)
    {
      QTV_Delete(ixContentBrowserClientPtr);
      ixContentBrowserClientPtr = NULL;
    }
    if(IxContentClientPtr!= NULL)
    {
      QTV_Delete(IxContentClientPtr);
      IxContentClientPtr = NULL;
    }
    if(attr_typePtr!= NULL)
    {
      QTV_Delete(attr_typePtr);
      attr_typePtr = NULL;
    }
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "IxContentClientPtr->GetAttr failed..errorCode %d",errorCode);
    return nRet;
  }

  if(trackType == QtvPlayer::VideoTrack)
  {
    isVideo = true;
  }
  else if(trackType == QtvPlayer::AudioTrack)
  {
    isAudio = true;
  }
  else if(trackType == QtvPlayer::TextTrack)
  {
    isText = true;
  }

  if(isVideo)
  {
    IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pNotifyData =   (void*)&QtvIxStreamInfo[VIDEO_INDEX];
    IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pfnNotify = QtvPlayerDispatch::IxCallBackWrapper;
    IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pUserData =(uint32)&QtvIxStreamInfo[VIDEO_INDEX];

    QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr = (utf8*)QTV_Malloc(uri->Length+1);
    if(QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr == NULL)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " isContentInstalled.. QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr is NULL..");
      //Error occured, free memory if allocated.
      if(ixContentBrowserClientPtr!= NULL)
      {
        QTV_Delete(ixContentBrowserClientPtr);
        ixContentBrowserClientPtr = NULL;
      }
      if(IxContentClientPtr!= NULL)
      {
        QTV_Delete(IxContentClientPtr);
        IxContentClientPtr = NULL;
      }
      if(attr_typePtr!= NULL)
      {
        QTV_Delete(attr_typePtr);
        attr_typePtr = NULL;
      }
      return nRet;
    }
    memcpy(QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr,uri->Ptr,uri->Length);
    *(QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr+uri->Length)='\0';
    QtvIxStreamInfo[VIDEO_INDEX].uri.Length = uri->Length;

    errorCode = IxContentClientPtr->GetObject( IxDRMPermission::PERM_PLAY,
                                            &(QtvIxStreamInfo[VIDEO_INDEX].ixMediaHandle),
                                            &IxStreamIxCallbacksArray[VIDEO_INDEX] );

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. &IxStreamIxCallbacksArray[VIDEO_INDEX] in GetObject = %x",
    &IxStreamIxCallbacksArray[VIDEO_INDEX]);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pNotifyData in GetObject = %x",
    IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pNotifyData);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pfnNotify in GetObject = %x",
    IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pfnNotify);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pUserData in GetObject = %x",
    IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pUserData);

  }
  else if(isAudio)
  {
    IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pNotifyData =   (void*)&QtvIxStreamInfo[AUDIO_INDEX];
    IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pfnNotify = QtvPlayerDispatch::IxCallBackWrapper;
    IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData =(uint32)&QtvIxStreamInfo[AUDIO_INDEX];

    errorCode = IxContentClientPtr->GetObject(IxDRMPermission::PERM_PLAY,
                                            &(QtvIxStreamInfo[AUDIO_INDEX].ixMediaHandle),
                                            &(IxStreamIxCallbacksArray[AUDIO_INDEX]) );

    QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr = (utf8*)QTV_Malloc(uri->Length+1);
    if(QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr == NULL)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " isContentInstalled.. QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr is NULL..");
      //Error occured, free memory if allocated.
      if(ixContentBrowserClientPtr!= NULL)
      {
        QTV_Delete(ixContentBrowserClientPtr);
        ixContentBrowserClientPtr = NULL;
      }
      if(IxContentClientPtr!= NULL)
      {
        QTV_Delete(IxContentClientPtr);
        IxContentClientPtr = NULL;
      }
      if(attr_typePtr!= NULL)
      {
        QTV_Delete(attr_typePtr);
        attr_typePtr = NULL;
      }
      return nRet;
    }
    memcpy(QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr,uri->Ptr,uri->Length);
    *(QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr+uri->Length)='\0';
    QtvIxStreamInfo[AUDIO_INDEX].uri.Length = uri->Length;

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. &IxStreamIxCallbacksArray[AUDIO_INDEX] in GetObject = %x",
    &IxStreamIxCallbacksArray[AUDIO_INDEX]);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pNotifyData in GetObject = %x",
    IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pNotifyData);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pfnNotify in GetObject = %x",
    IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pfnNotify);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pUserData in GetObject = %x",
    IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pUserData);

  }
  else if(isText)
  {
    IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pNotifyData =   (void*)&QtvIxStreamInfo[TEXT_INDEX];
    IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pfnNotify = QtvPlayerDispatch::IxCallBackWrapper;
    IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData =(uint32)&QtvIxStreamInfo[TEXT_INDEX];

    errorCode = IxContentClientPtr->GetObject(IxDRMPermission::PERM_PLAY,
                                            &(QtvIxStreamInfo[TEXT_INDEX].ixMediaHandle),
                                            &(IxStreamIxCallbacksArray[TEXT_INDEX]));

    QtvIxStreamInfo[TEXT_INDEX].uri.Ptr = (utf8*)QTV_Malloc(uri->Length+1);
    if(QtvIxStreamInfo[TEXT_INDEX].uri.Ptr == NULL)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " isContentInstalled.. QtvIxStreamInfo[TEXT_INDEX].uri.Ptr is NULL..");
      //Error occured, free memory if allocated.
      if(ixContentBrowserClientPtr!= NULL)
      {
        QTV_Delete(ixContentBrowserClientPtr);
        ixContentBrowserClientPtr = NULL;
      }
      if(IxContentClientPtr!= NULL)
      {
        QTV_Delete(IxContentClientPtr);
        IxContentClientPtr = NULL;
      }
      if(attr_typePtr!= NULL)
      {
        QTV_Delete(attr_typePtr);
        attr_typePtr = NULL;
      }
      return nRet;
    }
    memcpy(QtvIxStreamInfo[TEXT_INDEX].uri.Ptr,uri->Ptr,uri->Length);
    *(QtvIxStreamInfo[TEXT_INDEX].uri.Ptr+uri->Length)='\0';
    QtvIxStreamInfo[TEXT_INDEX].uri.Length = uri->Length;


    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. &IxStreamIxCallbacksArray[TEXT_INDEX] in GetObject = %x",
    &IxStreamIxCallbacksArray[TEXT_INDEX]);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pNotifyData in GetObject = %x",
    IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pNotifyData);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pfnNotify in GetObject = %x",
    IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pfnNotify);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    " isContentInstalled.. IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData in GetObject = %x",
    IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData);

  }

  if(ixContentBrowserClientPtr!= NULL)
  {
    QTV_Delete(ixContentBrowserClientPtr);
    ixContentBrowserClientPtr = NULL;
  }
  if(IxContentClientPtr!= NULL)
  {
    QTV_Delete(IxContentClientPtr);
    IxContentClientPtr = NULL;
  }
  if(attr_typePtr!= NULL)
  {
    QTV_Delete(attr_typePtr);
    attr_typePtr = NULL;
  }

  if ( IxSUCCESS != errorCode )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "IxContentClientPtr->GetObject failed error code %d",errorCode);
  }
  else
  {
    nRet = true;
    m_nIxStreamsCreated++;
  }
  return nRet;
}//isContentInstalledInContentDataBase
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::RegisterContentToContMgr

DESCRIPTION
  Registers the content/clip identified by @param const char* fileName with the
  IxContentManager. Sets the attribute for this content and returns IxStream* asssociated with
  the registered content. If content registration fails, returns NULL.

INPUT/OUTPUT:

 ->const char* fileName: Content file name
 ->QtvPlayer::TrackType trackType: Track type to be associated with IxStream* returned, if successful.

 <-: IxStream* if registration is successful, otherwise, returns NULL.

DEPENDENCIES
 @see Include files above.

RETURN VALUE
  Valid IxStream* or NULL;

SIDE EFFECTS
========================================================================== */
bool QtvPlayerDispatch::RegisterContentToContMgr(const char* fileName,
                                                 QtvPlayer::TrackType trackType)
{
  IxContentDef::UriT           uri;
  IxContentDef::UrlT           url;
  TxList<IxContent*>           contentItr;
  IxContentMgrClient           *pContentMgrClient;
  IxErrnoType                  nErr;
  TxList<IxContentAttrDef> attrdef;
  char* mimeType = (char*)"video/mp4";

  //Make sure we have valid filename to register content with content manager
  if(fileName == NULL)
  {
    return false;
  }
  /*
  * Construct the URI from the file name being passed in.
  */
  uri.Length = strlen(fileName);
  uri.Ptr = (utf8*)fileName;

  /*
  * Check if the content identified by URI is already installed in the content database.
  * if yes, update 'pStream' passed in and return true.
  */

  if(isContentInstalledInContentDataBase(&uri,trackType))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "isContentInstalledInContentDataBase returned IxStream* skipping registration..");
    return true;
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL,
  QTVDIAG_PRIO_HIGH,
  "Content not in content Database, begining to install attributes/content..");

  pContentMgrClient = QTV_New(IxContentMgrClient);

  if(pContentMgrClient == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "RegisterContentToContMgr, pContentMgrClient == NULL");
    return false;
  }
  url.Ptr = (utf8 *)fileName;
  url.Length = strlen( fileName);

  IxContentAttrDef* contattrdef = new IxContentAttrDef(CONTENTATTR_LOCAL_URL,(void*)&url);

  attrdef.push_front( *contattrdef);

  nErr = pContentMgrClient->AddContentAttr( &uri,&attrdef );

  if ( IxSUCCESS != nErr )
  {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_HIGH,
      "AddContentAttr Failed while adding CONTENT_ATTR_LOCAL_URL error code %d",nErr);
      if(pContentMgrClient)
      {
        QTV_Delete(pContentMgrClient);
        pContentMgrClient = NULL;
      }
      if(contattrdef)
      {
        delete(contattrdef);
        contattrdef = NULL;
      }
      return false;
  }

  attrdef.clear();

  url.Ptr = (utf8*)mimeType;
  url.Length = strlen( mimeType);

  IxContentAttrDef* contattrdefAttrName = new IxContentAttrDef(CONTENTATTR_MIME_TYPE,(void*)&url);

  attrdef.push_front( *contattrdefAttrName);
  nErr = pContentMgrClient->AddContentAttr( &uri,&attrdef );

  if ( IxSUCCESS != nErr )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "AddContentAttr Failed while adding CONTENT_ATTR_LOCAL_URL error code %d",nErr);
    if(pContentMgrClient)
    {
      QTV_Delete(pContentMgrClient);
      pContentMgrClient = NULL;
    }
    if(contattrdef)
    {
      delete(contattrdef);
      contattrdef = NULL;
    }
    if(contattrdefAttrName)
    {
      delete(contattrdefAttrName);
      contattrdefAttrName = NULL;
    }
    return false;
  }
  attrdef.clear();
  if(contattrdefAttrName)
  {
    delete(contattrdefAttrName);
    contattrdefAttrName = NULL;
  }
  contattrdefAttrName = new IxContentAttrDef(CONTENTATTR_STORAGE_MIME_TYPE,(void*)&url);
  attrdef.push_front( *contattrdefAttrName);
  nErr = pContentMgrClient->AddContentAttr( &uri,&attrdef );

  if ( IxSUCCESS != nErr )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,
    "AddContentAttr Failed while adding CONTENTATTR_STORAGE_MIME_TYPE error code %d",nErr);

    if(pContentMgrClient)
    {
      QTV_Delete(pContentMgrClient);
      pContentMgrClient = NULL;
    }
    if(contattrdef)
    {
      delete(contattrdef);
      contattrdef = NULL;
    }
    if(contattrdefAttrName)
    {
      delete(contattrdefAttrName);
      contattrdefAttrName = NULL;
    }
    return false;
  }
  if(contattrdef)
  {
    delete(contattrdef);
    contattrdef = NULL;
  }
  if(contattrdefAttrName)
  {
    delete(contattrdefAttrName);
    contattrdefAttrName = NULL;
  }
  switch(trackType)
  {
    case QtvPlayer::VideoTrack:
      QtvIxStreamInfo[VIDEO_INDEX].pContentMgrClient = pContentMgrClient;

      QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr = (utf8*)QTV_Malloc(strlen(fileName)+1);
      if(QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr == NULL)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
        QTVDIAG_PRIO_MED,
        " RegisterContent.. QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr is NULL..");
        break;
      }
      memcpy(QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr,fileName,strlen(fileName));
      *(QtvIxStreamInfo[VIDEO_INDEX].uri.Ptr+strlen(fileName))='\0';
      QtvIxStreamInfo[VIDEO_INDEX].uri.Length = strlen(fileName);

      QtvIxStreamInfo[VIDEO_INDEX].isWaitingForCommitAcked = true;
      IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pNotifyData =   (void*)&QtvIxStreamInfo[VIDEO_INDEX];
      IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pUserData = (uint32)&QtvIxStreamInfo[VIDEO_INDEX];

      IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pfnNotify = QtvPlayerDispatch::IxCallBackWrapper;
      nErr = QtvIxStreamInfo[VIDEO_INDEX].pContentMgrClient->Commit( &uri, &IxStreamIxCallbacksArray[VIDEO_INDEX] );

      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " RegisterContent.. IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pNotifyData in commit = %x",
      IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pNotifyData);

      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " RegisterContent.. IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pUserData in commit = %x",
      IxStreamIxCallbacksArray[VIDEO_INDEX].AEECallback.pUserData);

      break;

    case QtvPlayer::AudioTrack:
      QtvIxStreamInfo[AUDIO_INDEX].pContentMgrClient = pContentMgrClient;

      QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr = (utf8*)QTV_Malloc(strlen(fileName)+1);
      if(QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr == NULL)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
        QTVDIAG_PRIO_MED,
        " RegisterContent.. QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr is NULL..");
        break;
      }
      memcpy(QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr,fileName,strlen(fileName));
      *(QtvIxStreamInfo[AUDIO_INDEX].uri.Ptr+strlen(fileName))='\0';
      QtvIxStreamInfo[AUDIO_INDEX].uri.Length = strlen(fileName);

      QtvIxStreamInfo[AUDIO_INDEX].isWaitingForCommitAcked = true;
      IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pNotifyData =   (void*)&QtvIxStreamInfo[AUDIO_INDEX];
      IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pUserData = (uint32)&QtvIxStreamInfo[AUDIO_INDEX];

      IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pfnNotify = QtvPlayerDispatch::IxCallBackWrapper;
      nErr = QtvIxStreamInfo[AUDIO_INDEX].pContentMgrClient->Commit( &uri, &IxStreamIxCallbacksArray[AUDIO_INDEX] );

      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " RegisterContent.. &IxStreamIxCallbacksArray[AUDIO_INDEX]in commit = %x",
      &IxStreamIxCallbacksArray[AUDIO_INDEX]);

      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " RegisterContent.. IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pUserData in commit = %x",
      IxStreamIxCallbacksArray[AUDIO_INDEX].AEECallback.pUserData);

      break;

    case QtvPlayer::TextTrack:
      QtvIxStreamInfo[TEXT_INDEX].pContentMgrClient = pContentMgrClient;

      QtvIxStreamInfo[TEXT_INDEX].uri.Ptr = (utf8*)QTV_Malloc(strlen(fileName)+1);
      if(QtvIxStreamInfo[TEXT_INDEX].uri.Ptr == NULL)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
        QTVDIAG_PRIO_MED,
        " RegisterContent.. QtvIxStreamInfo[TEXT_INDEX].uri.Ptr is NULL..");

        break;
      }
      memcpy(QtvIxStreamInfo[TEXT_INDEX].uri.Ptr,fileName,strlen(fileName));
      *(QtvIxStreamInfo[TEXT_INDEX].uri.Ptr+strlen(fileName))='\0';
      QtvIxStreamInfo[TEXT_INDEX].uri.Length = strlen(fileName);

      QtvIxStreamInfo[TEXT_INDEX].isWaitingForCommitAcked = true;
      IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pNotifyData =   (void*)&QtvIxStreamInfo[TEXT_INDEX];

      IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData = (uint32)&QtvIxStreamInfo[TEXT_INDEX];

      IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pfnNotify = QtvPlayerDispatch::IxCallBackWrapper;
      nErr = QtvIxStreamInfo[TEXT_INDEX].pContentMgrClient->Commit( &uri, &IxStreamIxCallbacksArray[TEXT_INDEX] );

      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " RegisterContent.. &IxStreamIxCallbacksArray[TEXT_INDEX]in commit = %x",
      &IxStreamIxCallbacksArray[TEXT_INDEX]);

      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      " RegisterContent.. IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData in commit = %x",
      IxStreamIxCallbacksArray[TEXT_INDEX].AEECallback.pUserData);

      break;
  }

  if ( IxSUCCESS != nErr )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_HIGH,"Commit failed error code %d",nErr);

    if(pContentMgrClient)
     {
       QTV_Delete(pContentMgrClient);
       pContentMgrClient = NULL;
      if(trackType == QtvPlayer::VideoTrack)
      {
        QtvIxStreamInfo[VIDEO_INDEX].pContentMgrClient = NULL;
      }
      if(trackType == QtvPlayer::AudioTrack)
      {
        QtvIxStreamInfo[AUDIO_INDEX].pContentMgrClient = NULL;
      }
      if(trackType == QtvPlayer::TextTrack)
      {
        QtvIxStreamInfo[TEXT_INDEX].pContentMgrClient = NULL;
      }
    }
    return false;
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL,
  QTVDIAG_PRIO_HIGH,
  "Commit successful.. Callback will be fired from IPC SIG is SET..");

  m_nIxStreamsCreated++;
  return true;
}//RegisterContentToContMgr
#endif//#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT

/* ======================================================================
FUNCTION
  bool QtvPlayerDispatch::isLocalFileMedia(const char*)

DESCRIPTION
  This function looks at file extension and returns TRUE
  if it's local file such as (.asf,.wma,.wmv,.3gp,.3g2,.amc etc.)
  This is needed as we want to bypass IxStream stuff when fileName is
  some URL and not the local file.

INPUT/OUTPUT:

 ->const char* fileName: FileName of the content to be played through IxStream.
 <-: True if local file otherwise, returns false.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if extension is .dcf else returns FALSE.

SIDE EFFECTS
  none

========================================================================== */
bool QtvPlayerDispatch::isLocalFileMedia(const char* fileName)
{
  bool nRet = true;

  /*
  * Check if file extension is one of the following:
  * If yes, it's not local playback.
  * .url, .sdp, .flo, .pvx., .ram
  *
  *
  * Also, check if filename starts with one of the following.
  * If yes, it's not local playback.
  * http:, rtsp:, isdb:, sdp:.
  *
  * This could happen when .url is reposted by RealPlayer to QtvPlayer.
  * In this case, file contents are posted not the file name.
  *
  */

  if(fileName && strlen(fileName))
  {
    if( (strlen(fileName)>4)&&(
                               (
                                 (strincmp(fileName+(strlen(fileName)-4), ".url",4) == 0)||
                                 (strincmp(fileName+(strlen(fileName)-4), ".sdp",4) == 0)||
                                 (strincmp(fileName+(strlen(fileName)-4), ".flo",4) == 0)||
                                 (strincmp(fileName+(strlen(fileName)-4), ".ram",4) == 0)||
                                 (strincmp(fileName+(strlen(fileName)-4), ".pvx",4) == 0)
                               )||
                               (
                                 (strincmp(fileName, "http:",5) == 0)||
                                 (strincmp(fileName, "rtsp:",5) == 0)||
                                 (strincmp(fileName, "isdb:",5) == 0)||
                                 (strincmp(fileName, "sdp:",4) == 0)
                               )
                             )
      )
    {
      //Not local file playback, should return false in this case.
       nRet = false;
    }
  }
  return nRet;
}
/* ======================================================================
FUNCTION
  bool QtvPlayerDispatch::IsDrmDcfMedia(const char*)

DESCRIPTION
  This function examines the file extension and returns TRUE if extension is .dcf else returns FALSE.

INPUT/OUTPUT:

 ->const char* fileName: FileName of the content to be played through IxStream.
 <-: True if file extension is DCF|dcf, otherwise, returns false.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if extension is .dcf else returns FALSE.

SIDE EFFECTS
  none

========================================================================== */
bool QtvPlayerDispatch::IsDrmDcfMedia(const char* fileName)
{
  bool nRet = false;
  if(fileName && strlen(fileName))
  {
    if ( (ZUtils::FindR(fileName, ".dcf") != ZUtils::npos) ||
         (ZUtils::FindR(fileName, ".DCF") != ZUtils::npos)
       )
    {
       nRet = true;
    }
  }
  return nRet;
}
/* ======================================================================
FUNCTION
  QtvPlayerDispatch::OpenInputStream

DESCRIPTION
  Method to post PV_OPEN_INPUTSTREAM_URN to Mpeg4Player.

INPUT/OUTPUT:

 ->dcf_ixstream_type vStream: IxStream* for video instance.
 ->dcf_ixstream_type aStream: IxStream* for audio instance.
 ->dcf_ixstream_type tStream: IxStream* for text  instance.

 <-: QtvPlayer::QTV_RETURN_ERROR if error occurs while posting an event
     to Mpeg4Player, otherwise returns QtvPlayer::QTV_RETURN_OK.

DEPENDENCIES
  None.

RETURN VALUE
  @See QtvPlayer::ReturnT

SIDE EFFECTS
  None.

========================================================================== */
QtvPlayer::ReturnT QtvPlayerDispatch::OpenInputStream(dcf_ixstream_type vStream,
                                                      dcf_ixstream_type aStream,
                                                      dcf_ixstream_type tStream)
{
  QtvPlayer::ReturnT nReturn = QtvPlayer::QTV_RETURN_ERROR;

    if (m_pMpeg4Player && m_pMpeg4Player->StartThread())
    {
      PV_OPEN_INPUTSTREAM_URN_type *pEvent = QCCreateMessage(PV_OPEN_INPUTSTREAM_URN,m_pMpeg4Player);
      if (pEvent)
      {
        pEvent->videoStream = vStream;
        pEvent->audioStream = aStream;
        pEvent->textStream  = tStream;

        //Reset the abort flag before issueing playback.
        m_pMpeg4Player->SetMediaAbortFlag(false);
        m_pMpeg4Player->PostMessage(pEvent);
        nReturn = QtvPlayer::QTV_RETURN_OK;
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_MED,
      "QtvPlayerDispatch::OpenInputStream could not post msg. to Mpeg4Player");
    }
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    ">>QtvPlayerDispatch::OpenInputStream(a,v,t) return %d",nReturn);

    return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayerDispatch::NotifyCallback

DESCRIPTION
  Notify to the client about player status.

INPUT/OUTPUT:

->PlayerStatusT status: player status code.
  void *      pClientData : client registered data.
  PlayerStateRecordT state: current player state.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */

void QtvPlayerDispatch::NotifyCallback( QtvPlayer::PlayerStatusT status,    /* player status code */
                                        void *      pClientData,
                                        QtvPlayer::PlayerStateRecordT state /* current player state. */
                                       )
{
  if(m_pQtvPlayer && m_pQtvPlayer->CallbackFunction)
  {
    m_pQtvPlayer->CallbackFunction(status, pClientData, state);
  }
  else if(m_pQtvPlayer && m_pQtvPlayer->CallbackInstanceFunction)
  {
    m_pQtvPlayer->CallbackInstanceFunction(status, pClientData, state,
                                          (QtvPlayer::InstanceHandleT)m_pQtvPlayer,
                                          m_pQtvPlayer->CallbackUserData);
  }
  return;
}

/* ======================================================================
FUNCTION
  DispatchThread::DispatchThread()

DESCRIPTION
  Non public constructor for DispatchThread class.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
DispatchThread::DispatchThread()
{
}//DispatchThread()

/* ======================================================================
FUNCTION
  DispatchThread::GetSingletonInstance()

DESCRIPTION
  Static method to return singleton instance.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */

DispatchThread* DispatchThread::GetSingletonInstance()
{
 if(pInstance == NULL)
 {
   pInstance = QTV_New(DispatchThread);
   QTV_MSG_PRIO(QTVDIAG_GENERAL,
   QTVDIAG_PRIO_FATAL,
   "DispatchThread::GetSingletonInstance creating pInstance..");
 }
 ++m_objRefCnt;
 return pInstance;
}//GetSingletonInstance()


/* ======================================================================
FUNCTION
  DispatchThread::DestroyInst()

DESCRIPTION
  Call the destructor if ref count is zero, else just decrement the ref cnt..

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void DispatchThread::DestroyInst()
{
  if(m_objRefCnt)
  {
    --m_objRefCnt;
  }

  if(!m_objRefCnt && pInstance)
  {
    pInstance->QTVDispatchThreadStopAndDestroy();
    QTV_Delete(pInstance);
    pInstance = NULL;
  }
}

/* ======================================================================
FUNCTION
  DispatchThread::~DispatchThread()

DESCRIPTION
  destructor for DispatchThread class.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
DispatchThread::~DispatchThread()
{
}//~DispatchThread()

/* ======================================================================
FUNCTION
  DispatchThread::StartQTVDispatchThread()

DESCRIPTION
  creates thread

INPUT/OUTPUT:

 ->None.
 <-: True if QtvPlayerDispatch thread is already running or is started successfully,
     otherwise, returns false.

DEPENDENCIES
 None

RETURN VALUE
 True when thread is started successfully otherwise, returns false.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool DispatchThread::StartQTVDispatchThread()
{
  //only one can be active at a time.
  if(!QCUtils::IsThreadActive(&QTVDispatchThreadTC))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "DispatchThread::StartQTVDispatchThread" );

    m_bStopToBeDone = false;
    m_bThreadStarted = false;

    QTVDispatchThreadTC.threadHandle = NULL;
    QTVDispatchThreadTC.pThreadFunction = (QCThreadFuncType)DispatchThread::TheQTVDispatchThread;

    QTVDispatchThreadTC.stackSize = COMMON_STACK_SIZE;
    QCUtils::SetThreadName(&QTVDispatchThreadTC,Common::QTV_UTIL_THREAD_NAME);

    QCUtils::CreateCondition(&QTVDispatchThreadStartSync,false,false);
    QCUtils::CreateCondition(&QTVDispatchThreadIODoneSync, false, false );
    QCUtils::InitCritSect(&QTVDispatchThread_CS);

    //start the thread.
    QCUtils::ResetCondition(&QTVDispatchThreadStartSync);
    QTVDispatchThreadTC.priority = QTVPlayerPriority;

     if (!QCUtils::CreateThread(&QTVDispatchThreadTC,
                                (unsigned long)this,
                                false))
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL,
      QTVDIAG_PRIO_ERROR,
      "DispatchThread::StartThread failed");
       return false;
     }
     //don't return until the thread has set its event dispatch table.
     unsigned long sleepTotal = 0;
    long EVENT_DISPATCH_TABLE_CREATION_POLL_INTERVAL = 10;

    //don't return until the thread has set its event dispatch table.
    while ( !m_bThreadStarted && (sleepTotal <= COMMON_THREAD_START_TIMEOUT_MSEC) )
    {
      sleepTotal += EVENT_DISPATCH_TABLE_CREATION_POLL_INTERVAL;
      rex_sleep( EVENT_DISPATCH_TABLE_CREATION_POLL_INTERVAL );
    }

    if(sleepTotal > COMMON_THREAD_START_TIMEOUT_MSEC)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "DispatchThread::StartThread failed, start thread timeout");
      return false;
    }
    else
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                    "DispatchThread::StartThread successful, time taken to start the Thread %d", sleepTotal);
    }
  }
  if (!QCUtils::IsThreadActive(&QTVDispatchThreadTC))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_ERROR,
    "DispatchThread::StartThread failed thread not active");
     return false;
  }
  return true;
}//StartQtvDispatchThread

/* ======================================================================
FUNCTION
  DispatchThread::TheQTVDispatchThread

DESCRIPTION
  Invokes actual message/event processing routine.

INPUT/OUTPUT:

 ->unsigned long param: Parameter specified when creating thread.
 <-: None.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void DispatchThread::TheQTVDispatchThread(unsigned long param)
{
   QTV_MSG_PRIO1(QTVDIAG_GENERAL,
   QTVDIAG_PRIO_LOW,
   "DispatchThread::::TheQTVPlayerThread param 0x%x", param );
   ((DispatchThread*)param)->QTVDispatchThread();

}//TheQTVDispatchThread

/* ======================================================================
FUNCTION
  DispatchThread::QTVDispatchThread

DESCRIPTION
  Event/messge processing/dispatching routine for QtvDispatch thread.

INPUT/OUTPUT:

 ->:None.
 <-:None.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void DispatchThread::QTVDispatchThread(void)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL,
   QTVDIAG_PRIO_LOW, "DispatchThread::QTVDispatchThread");

   QCUtils::SetMessageDispatchTable( QtvPlayerEventTable, &QTVDispatchThreadER );

   //set the event to indicate that our dispatch table is activated

   QTV_MSG_PRIO(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_ERROR,
                "Done with SetMessageDispatchTable");
   QCUtils::SetCondition(&QTVDispatchThreadStartSync);

   QTV_MSG_PRIO(QTVDIAG_GENERAL,
   QTVDIAG_PRIO_ERROR,
   "Done with QCUtils::SetCondition for QTVDispatchThreadStartSync");

   QTVDispatchMessageType *pEvent;

   //Register QTVPlayer Task for IPC and with NS
   ixipc_init(0,QTV_DRM_DCF_IPC_SIG) ;
   m_bThreadStarted = true;

   while (1)
   {
     // Message Loop
      pEvent = QCUtils::GetNextMessage(&QTVDispatchThreadER);
      if (pEvent)
      {
        if(m_bStopToBeDone)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL,
          QTVDIAG_PRIO_ERROR, "QTVThread detected m_bStopToBeDone to be TRUE" );
           break;
          }
        QCUtils::DispatchMessage( pEvent, &QTVDispatchThreadER);
      }
      else if(ixipc_message_waiting(QTV_DRM_DCF_IPC_SIG))
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
        QTVDIAG_PRIO_ERROR,
        "QTVThread gets NULL event but IS_IPC_MSG_READY is TRUE" );

#ifdef FEATURE_IXCONTENTSVC

        ixipc_msg_type* ipc_msg_ptr = ixipc_receive( IPC_MSG_ANY );

#ifdef FEATURE_QTV_DRM_DCF_CONTENT_MANAGEMENT
        if( IPCSVC_GET_SVC_ID(*((ipc_msg_id_type*) ipc_msg_ptr)) == IPCSVC_IXCONTENT )
        {
          IxContentClient_ProcessMsg( &ipc_msg_ptr );
        }
        else if( IPCSVC_GET_SVC_ID(*((ipc_msg_id_type*) ipc_msg_ptr)) == IPCSVC_IXCONTENTMGR )
        {
          IxContentMgrClient_ProcessMsg( &ipc_msg_ptr );
        }
        else if (IPCSVC_GET_SVC_ID(*((ipc_msg_id_type*) ipc_msg_ptr)) == IPCSVC_IXCONTENTBROWSER)
        {
          IxContentBrowserClient_ProcessMsg( &ipc_msg_ptr );
        }
        else
#endif
        {
           //IPC signal is set, invoke dispatcher to.
            //fire callback provided by signal owner TCB.
          //Here ipc_receive is already done, so pass in the ipc_msg_ptr to IxStreamDispatchMsg
            IxStreamDispatchMsg(ipc_msg_ptr,QTV_DRM_DCF_IPC_SIG);
          ixipc_delete( (ixipc_msg_type**)&ipc_msg_ptr );
        }
#else
        //IPC signal is set, invoke dispatcher to
          //fire callback provided by signal owner TCB
        //Here ipc_receive is not yet done, so pass in NULL as a first parameter to IxStreamDispatchMsg
        //this will make it invoke ipc_receive
          IxStreamDispatchMsg(NULL,QTV_DRM_DCF_IPC_SIG);
#endif
       }//else if(IS_IPC_MSG_READY(QTV_DRM_DCF_IPC_SIG))
   }//while (1)

   //This task/thread is done with communicating with DRM,so
   //unregister IPC signnal and unregister with NS

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
   "UnRegistering IPC Signal QTV_DRM_DCF_IPC_SIG");
   //when done, de-register with ixipc.
   ixipc_terminate();

   QCUtils::SetCondition( &QTVDispatchThreadIODoneSync);

   // Suspend the thread, so it can be killed by the Terminate function
   m_bStopToBeDone = false;
   QCUtils::PauseThread(&QTVDispatchThreadTC);
}//QTVDispatchThread
/* ======================================================================
FUNCTION
  DispatchThread::IsQTVDispatchThreadActive()

DESCRIPTION
  Checks if QtvPlayer thread is active or idle.

INPUT/OUTPUT:

 ->: None.
 <-: True if the Dispatch thread is active otherwise, returns false.

DEPENDENCIES
  None

RETURN VALUE
  True if the thread is active otherwise, returns false;

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool DispatchThread::IsQTVDispatchThreadActive()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL,
   QTVDIAG_PRIO_LOW,
   "DispatchThread::::IsThreadActive" );

   return QCUtils::IsThreadActive(&QTVDispatchThreadTC);
}//IsQTVDispatchThreadActive

/* ======================================================================
FUNCTION
  DispatchThread::QTVDispatchThreadPostMessage

DESCRIPTION
  Post an event QtvPlayerDispatch thread, if running.

INPUT/OUTPUT:

->QTVDispatchMessageType *pEvent: Event to be posted.
<-: True event is posted, otherwise, returns false.


DEPENDENCIES
  None

RETURN VALUE
  Truer if thread is running and event is posted,
  otherwise, returns false.

SIDE EFFECTS
  None

========================================================================== */

bool DispatchThread::QTVDispatchThreadPostMessage(QTVDispatchMessageType *pEvent)
{
   if(IsQTVDispatchThreadActive())
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL,
     QTVDIAG_PRIO_LOW,
     "QTVThreadPostMessage" );
     QCUtils::PostMessage(pEvent, QTV_EVENT_PRI_URGENT, &QTVDispatchThreadER);
     return true;
   }
   return false;
}//QTVDispatchThreadPostMessage

/* ======================================================================
FUNCTION
  DispatchThread::QTVDispatchThread_STOP_EVENT_handler

DESCRIPTION
  Handler for QTVPLAYER_STOP_EVENT.

INPUT/OUTPUT:
 ->QtvPlayerDispatchMessageHandleType handle: QTVPLAYER_STOP_EVENT event type.
 <-: None.

DEPENDENCIES
 None

RETURN VALUE
  none

SIDE EFFECTS
None

========================================================================== */
void DispatchThread::QTVDispatchThread_STOP_EVENT_handler (QtvPlayerDispatchMessageHandleType handle)
{
  if(handle != NULL)
  {
    QTVPLAYER_STOP_EVENT_type *pStopEvent = (QTVPLAYER_STOP_EVENT_type*)handle;
    DispatchThread* myself = (DispatchThread*)pStopEvent->DispatchHandle;

    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_MED,
    ">>DispatchThread::QTVPLAYER_STOP_EVENT_handler");

    QCUtils::EnterCritSect(&(myself->QTVDispatchThread_CS));
     myself->m_bStopToBeDone = true;
     QCUtils::LeaveCritSect(&(myself->QTVDispatchThread_CS));
  }
}
/* ======================================================================
FUNCTION
  DispatchThread::QTVDispatchThreadStopAndDestroy()

DESCRIPTION
  Stops and destroys QTVDispatchThread if running otherwise does nothing.

INPUT/OUTPUT:

->: None.
<-: None.

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
  Stops thr QtvPlayer thread, if running.

========================================================================== */
void DispatchThread::QTVDispatchThreadStopAndDestroy()
{
  if(IsQTVDispatchThreadActive())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
    QTVDIAG_PRIO_ERROR,
    "Stopping and destroying DispatchThread::::QTVThread");

    QCUtils::EnterCritSect(&QTVDispatchThread_CS);
    QTVPLAYER_STOP_EVENT_type *pEvent = QCCreateMessage(QTVPLAYER_STOP_EVENT,this);
    if (pEvent != NULL && pEvent->DispatchHandle != NULL)
    {
      pEvent->DispatchHandle = (void*)this;
    } 
    m_bStopToBeDone = true;
    QCUtils::LeaveCritSect(&QTVDispatchThread_CS);

    if (NULL != pEvent)
    {
      QTVDispatchThreadPostMessage(pEvent);
    }
    unsigned long sleepTotal = 0;
    long EVENT_DISPATCH_TABLE_CREATION_POLL_INTERVAL = 10;
    while ( m_bStopToBeDone && (sleepTotal <= COMMON_THREAD_START_TIMEOUT_MSEC) )
    {
      sleepTotal += EVENT_DISPATCH_TABLE_CREATION_POLL_INTERVAL;
      rex_sleep( EVENT_DISPATCH_TABLE_CREATION_POLL_INTERVAL );
    }

    if(sleepTotal > COMMON_THREAD_START_TIMEOUT_MSEC)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Dispatch Thread STOP Timed OUT");
    }
    else
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "DispatchThread stopped successfully, time taken  %d", sleepTotal);
      QCUtils::KillThread(&QTVDispatchThreadTC);
      QCUtils::DestroyCondition(&QTVDispatchThreadStartSync);
      QCUtils::DestroyCondition(&QTVDispatchThreadIODoneSync);

      QCUtils::EnterCritSect(&QTVDispatchThread_CS);
      m_bThreadStarted = false;
      QCUtils::LeaveCritSect(&QTVDispatchThread_CS);
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"DispatchThread::::QTVThread cleanup done..");
    }
  }
}//QTVDispatchThreadStopAndDestroy

#endif //#ifdef FEATURE_QTV_DRM_DCF

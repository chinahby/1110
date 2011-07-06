/* =======================================================================
                            GenericAudioFile.cpp 
DESCRIPTION 
  Meaningful description of the definitions contained in this file.
Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                     Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/audiofileparser/main/latest/src/genericaudiofile.cpp#14 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Common definitions                      */
#include "assert.h"             /* ASSERT definitions                      */
#include <stdio.h>
#include "fileMedia.h"
#include "GenericAudioFile.h"
#include "ztl.h"
#include "utf8conv.h"
#include "atomdefs.h"
#include "oscl_string.h"

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
extern "C" {
#include "event.h"
}
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

/* ==========================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MIN_NOBYTES_REQUIRED_TOSEND_TOCMX_FOR_PD 50

#define MAX_GENERIC_DATA_BUFFER_SIZE 2560

#define DEFAULT_SEEK_TIME_MSEC 10000
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================== */
/* <EJECT> */
/*===========================================================================

FUNCTION  GenericAudioFile

DESCRIPTION
This is the GenericAudioFile class constructor - initializes the class members.
===========================================================================*/
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
GenericAudioFile::GenericAudioFile(  OSCL_STRING filename,
                       Mpeg4Player *pMpeg4Player,
                       unsigned char *pFileBuf,
                       uint32 bufSize,
                       bool /*playVideo*/,
                       bool playAudio,
                       bool /*playText*/,
                       AudioDataFormatType audioFormatType
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
                       ,bool bHttpStream
                       ,uint32 wBufferOffset
                       ,uint32 wStartupTime
                       ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize
                       ,QtvPlayer::FetchBufferedDataT FetchBufferedData
                       ,QtvPlayer::InstanceHandleT handle
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
                     )
{
  // Initialize  
  GenericAudioFile::InitData();
  m_pMpeg4Player= pMpeg4Player;
  m_audioFormatType = audioFormatType;
  m_playAudio = playAudio;
  m_bAudioPresentInClip = true;
  m_bVideoPresentInClip = false;
  m_bTextPresentInClip  = false;
  m_bitRate = 0;
  m_totalPbTime = 0;
  m_timeStamp = 0;
  m_repositioned = false;
  m_repositioningAllowed = true;

  if(filename.size() && pFileBuf)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
         "GenericAudioFile::GenericAudioFile filename and pFileBuf both non NULL");
  }

  if(!(filename.size()) && !pFileBuf
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
     && !FetchBufferedDataSize
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
       "GenericAudioFile::GenericAudioFile filename and pFileBuf both NULL");
  }  
  m_pFileBuf  = pFileBuf;
  m_fileSize  = bufSize;
  m_filename  = filename;

  /* this is for the push interface and if we use the stack from Media Player 
     bufSize is going to be the file size.*/
  if(bufSize >0)
  {
    m_nPreRollDataBytes = ((uint32) ((((float) (bufSize)) / 100.0) * 18.0));  
  }

  if(m_pFileBuf)
  {
    m_parseFilePtr = OSCL_FileOpen(m_pFileBuf, m_fileSize);
  }

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  else if(FetchBufferedDataSize)
  {    
    m_parseFilePtr = OSCL_FileOpen(NULL, 0, FetchBufferedData, 0, handle);
  }
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  else
  {
    m_parseFilePtr = OSCL_FileOpen(m_filename, (OSCL_TCHAR *) _T("rb"));
  }

  if(!m_parseFilePtr)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Failed to open file");
    _success = false;
    return;
  }

//  // File Size calculation
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if(FetchBufferedDataSize)
  {
    //Out here we dont know the file size   
    m_fileSize = 0xFFFFFFFF;
  }
  else if(!m_fileSize)
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  {
    //we have to remove this while checking in
    m_fileSize = OSCL_FileSize( m_filename );
  }
  AtomUtils::fileSize   = (int32)m_fileSize;

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  bHttpStreaming    = bHttpStream;
  m_wBufferOffset   = wBufferOffset;
  m_startupTime     = wStartupTime;
  m_fpFetchBufferedDataSize    = FetchBufferedDataSize;
  m_fpFetchBufferedData        = FetchBufferedData;   
  m_QtvInstancehandle  = NULL;
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  //if(playAudio)
  //{
    /*allocate buffer to serve the data request from CMX to get the total
      playback time and the audio spec freeing this in the destructor*/
    m_buffer = QTV_New_Array(uint8,MAX_GENERIC_DATA_BUFFER_SIZE);
  //}
  /* Create a condition to wait to get the response from CMX */
  QCUtils::CreateCondition(&genericAudio_wakeupSync,false,false);
}

/* ======================================================================
FUNCTION
  GenericAudioFile::resetInitialization

DESCRIPTION
  Resets m_bMediaInitialized to indicate that media is not initialized.
  Currently, only used for generic audio formats.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

========================================================================== */
void GenericAudioFile::resetInitialization()
{
  m_bMediaInitialized = false;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
         "GenericAudioFile::resetInitialization");
}
/*===========================================================================

FUNCTION  InitData

DESCRIPTION
  Initialize all members.
===========================================================================*/
void GenericAudioFile::InitData()
{  

  _success = true;

  m_playAudio            = false;
  m_hasAudio             = false;
  m_pFileBuf            = NULL;
  m_fileSize            = 0;
  m_filename            = NULL;
  m_parseFilePtr        = NULL;
  AtomUtils::fileSize   = 0;
  m_seekingAllowed      = FALSE;
  /* Set the handle to send to cmx while getting time and audio spec */
  memset(&handle,0,sizeof(cmx_handle_type ));
  
  //Will be initialized only when we get audio spec in GetAudioSpecCallback
  m_bMediaInitialized = false;
  
  //Updated when we enter buffering/rebuffering
  m_nDataOffsetWhenUnderRun = 0;
  /*
  *Calculated once for each clip when file size is being passed.
  *This is 18% of the clip size.When data is not sufficient,
  *player enters buffering and m_nDataOffsetWhenUnderRun gets updated.

  *We come out of buffering,if (downloaded data size == file size)or
  *(m_nPreRollDataBytes(which is 18%of clip size) + m_nDataOffsetWhenUnderRun) >= size of data being downloaded
  */
  m_nPreRollDataBytes = 0;

//#ifdef FEATURE_QTV_DRM_DCF
//  m_inputStream         = NULL;
//#endif

  _fileErrorCode        = (int32)EVERYTHING_FINE;

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  bHttpStreaming               = false;
  m_wBufferOffset              = 0;
  bGetMetaDataSize             = FALSE;
  bDataIncomplete              = FALSE;
  bQtvPlayerPaused             = TRUE;
  bsendParseFragmentCmd        = FALSE;
  m_currentParseFragment       = 0;
  m_minOffsetRequired          = 0;
  parserState                  = Common::PARSER_IDLE;
  m_pbTime                     = 0;
  m_startupTime                = 0;
  m_fpFetchBufferedDataSize    = NULL;
  m_fpFetchBufferedData        = NULL;
  m_QtvInstancehandle          = NULL;
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
}

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/*===========================================================================

FUNCTION  parseFirstFragment

DESCRIPTION
Here evern though we don't have any fragments/atom just we are following the calling
sequence and names as mpeg4file.cpp

===========================================================================*/
void GenericAudioFile::parseFirstFragment()
{
  if(!initializeVideoFMT())
  {
    _fileErrorCode = (int32)READ_FAILED; // Read past EOF
    _success = false;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VIDEO_FMT_FAILURE");
  }
}

/*===========================================================================

FUNCTION  initializeVideoFMT

DESCRIPTION
Here evern though we are not using any VideoFmt services to parse
just we are following the calling sequence and names as mpeg4file.cpp

===========================================================================*/
boolean GenericAudioFile::initializeVideoFMT ( void )
{
  boolean returnStatus = false;
  if (bHttpStreaming)
  {
    bDataIncomplete = TRUE;
    (void)parseHTTPStream();
    return true;
  }
  return returnStatus;
}

/*===========================================================================

FUNCTION  parseHTTPStream

DESCRIPTION
Public method used to parse the Http Stream.

===========================================================================*/
bool GenericAudioFile::parseHTTPStream ( void )
{
  bool returnStatus = true;  
  if (CanPlayTracks(m_startupTime))
  {  
    SetTotalTime();
    GetAudioSpec();
    //QTV_PS_PARSER_STATUS_READY
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_READY, m_playAudio=%d",m_playAudio);
    sendParserEvent(Common::PARSER_READY);
  }
  else
  {    
    //QTV_PS_PARSER_STATUS_PAUSED
    sendParserEvent(Common::PARSER_PAUSE);
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_PAUSE, m_playaudio=%d",m_playAudio);
    returnStatus = false;
  }
  return returnStatus;
}

/*===========================================================================

FUNCTION  CanPlayTracks

DESCRIPTION
Public method used to determine if the given clip can be played for
the specified amouunt of time.

===========================================================================*/
bool GenericAudioFile::CanPlayTracks(uint32 pbTime)
{
  bool returnStatus = false;
  
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
         "GenericAudioFile::CanPlayTracks pbTime %d m_pbTime %d",pbTime,m_pbTime);

  if(m_fpFetchBufferedDataSize)
  {    
     boolean bEndOfData = false;
    //Pull interface so pull dnld data size from OEM
    (*m_fpFetchBufferedDataSize)(0,&m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);
  }
  
  if(pbTime <= m_pbTime)
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "CanPlayTracks: pbTime=%d, m_pbTime=%d", pbTime, m_pbTime);
    return true;
  }
  
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
         "GenericAudioFile::CanPlayTracks m_wBufferOffset %d m_fileSize %d",m_wBufferOffset,m_fileSize);

  if( m_wBufferOffset == m_fileSize )
  {   
    return true;
  }
  if(!m_pbTime)
  {
    //when playback time is zero.
   
    /* Here we are computing pre rolled time based on the document TD910
	  That a progressive download buffer for audio-only files SHOULD be
    The greater of 80kilobytes or 18% of the file size. */
        
    m_minOffsetRequired = ((uint32) ((((float) (m_fileSize)) / 100.0) * 18.0)) > 81920 ?
                                     ((uint32) ((((float) (m_fileSize)) / 100.0) * 18.0)) :
                                     81920;           
    
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
         "GenericAudioFile::CanPlayTracks m_minOffsetRequired calculated as %d",m_minOffsetRequired);

    if( m_wBufferOffset >= m_minOffsetRequired )
    {
      QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
         "GenericAudioFile::CanPlayTracks returning TRUE as m_wBufferOffset %d >= m_minOffsetRequired %d updating m_pbTime %d",m_wBufferOffset,m_minOffsetRequired,pbTime);
      m_pbTime = pbTime;
      returnStatus = true;
    }
  }
  else
  {    
    if(m_nDataOffsetWhenUnderRun > 0)
    {      
      //We must be in BUFFERING/REBUFFERING as m_nDataOffsetWhenUnderRun is > 0
      if(m_fpFetchBufferedDataSize)
      {        
        //Pull interface so pull dnld data size from OEM
        boolean bEndOfData = false;        
        (*m_fpFetchBufferedDataSize)(0,&m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);
      }
      if(m_bitRate)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "valid m_bitRate");
        /* while playing calculating the pbtime based on the bitrate */
        if(pbTime <= ((uint32)(( (float)m_wBufferOffset * 8 )/((float) m_bitRate ))))
      {
        m_pbTime = pbTime;
        returnStatus = true;        
      }
    }
      else if((m_nDataOffsetWhenUnderRun + m_nPreRollDataBytes) <= m_wBufferOffset)
      {
        m_pbTime = pbTime;
        returnStatus = true;
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
           "canPlayTracks returning TRUE based on m_nDataOffsetWhenUnderRun + m_nPreRollDataBytes %d m_wBufferOffset %d",(m_nDataOffsetWhenUnderRun + m_nPreRollDataBytes),m_wBufferOffset);
        m_nDataOffsetWhenUnderRun = 0;
      }
    }    
  }
  return returnStatus;
}

/*===========================================================================

FUNCTION  setFileSize

DESCRIPTION
Public method used to set the file size because in progressive download
we don't know the file size.

===========================================================================*/
int32 GenericAudioFile::setFileSize(uint32 fileSize)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
         " GenericAudioFile::setFileSize  fileSize passed in %d",fileSize);
  m_fileSize =  fileSize;
  m_nPreRollDataBytes = ((uint32) ((((float) (m_fileSize)) / 100.0) * 18.0));  
  return m_fileSize;
}

/*===========================================================================

FUNCTION  updateBufferWritePtr

DESCRIPTION
Public method used to update the write buffer offset during Http streaming.

===========================================================================*/
void GenericAudioFile::updateBufferWritePtr ( uint32 writeOffset )
{
  //Executing in the UI thread context.
  m_wBufferOffset = writeOffset;  
  if((parserState == Common::PARSER_PAUSE) || (parserState == Common::PARSER_RESUME))
  {
    sendParseHTTPStreamEvent();
  }
}

/*===========================================================================

FUNCTION  sendParseHTTPStreamEvent

DESCRIPTION
Public method used to switch contexts and call the parseHttpStream.

===========================================================================*/
void GenericAudioFile::sendParseHTTPStreamEvent(void)
{
  QTV_PROCESS_HTTP_STREAM_type *pEvent = QCCreateMessage(
                                           QTV_PROCESS_HTTP_STREAM,
                                           m_pMpeg4Player);
  if (pEvent)
  {
    pEvent->bHasAudio = true;
    pEvent->bHasVideo = false;
    pEvent->bHasText = false;
    QCUtils::PostMessage(pEvent, 0, NULL);
  }
}

/*===========================================================================

FUNCTION  sendHTTPStreamUnderrunEvent

DESCRIPTION
Public method used to switch contexts and notify the player about buffer-underrun.

===========================================================================*/
void GenericAudioFile::sendHTTPStreamUnderrunEvent(void)
{
  QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_type *pEvent = QCCreateMessage(
                                                         QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT,
                                                         m_pMpeg4Player);

  if (pEvent)
  {
    pEvent->bAudio =  true;
    pEvent->bVideo =  false;
    pEvent->bText =  false;
    QCUtils::PostMessage(pEvent, 0, NULL);
  }
}

/*===========================================================================

FUNCTION  sendParserEvent

DESCRIPTION
Public method used send parser events

===========================================================================*/
void GenericAudioFile::sendParserEvent(Common::ParserStatusCode status)
{
  parserState = status;
  QTV_HTTP_PARSER_STATUS_EVENT_type *pEvent = QCCreateMessage(
                                                QTV_HTTP_PARSER_STATUS_EVENT,
                                                m_pMpeg4Player);
  if (pEvent)
  {
    pEvent->status = status;
    pEvent->bHasAudio = true;
    pEvent->bHasVideo = false;
    pEvent->bHasText = false;
    QCUtils::PostMessage(pEvent, 0, NULL);
  }
}
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

/*===========================================================================

FUNCTION  SetTotalTime

DESCRIPTION
public method to get the total play back time from CMX.

===========================================================================*/
void GenericAudioFile::SetTotalTime()
{
 #ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if( bHttpStreaming && m_fpFetchBufferedDataSize)
  {
    boolean bEndOfData = false;
    //Pull interface so pull dnld data size from OEM
    (*m_fpFetchBufferedDataSize)(0,&m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);

    if( (m_fileSize) && (m_wBufferOffset != m_fileSize) )        
    {        
      /*
      * If entire file is not being downloaded, do not issue
      * cmx_audfmt_get_time. This is becasue, CMX need to scan entire file to
      * complete cmx_audfmt_get_time API. Stopping in the middle by passing NULL buffer
      * when we run out of data helps to stop CMX from asking more data but then GetAudioSpec fails.
      *
      * Thus, if we know cmx_audfmt_get_time is going to fail, do not invoke it.
      */      
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"File isn't downloaded fully,not issuing cmx_audfmt_get_time");
      return;
    }            
  }
#endif          

  memset( &handle, 0, sizeof(handle) );
  //
  // Set cmx handle
  //
  /* prepare the handle to send to CMX */
  handle.source_type         = CMX_SOURCE_CLIENT;
  handle.client.source_type  = CMX_SOURCE_CLIENT;
  handle.client.data_config  = CMX_CLIENT_DATA_REAL_TIME;
  /* Call back to handle the events from CMX */
  handle.client.data_req_ptr = GetToatalTimeDataRequestCallback;
  handle.client.data_len = m_fileSize;  
  
  /* Call CMX API to get the total playback time */
#ifndef FEATURE_WINCE
  cmx_audfmt_get_time(&handle, GetTotalTimeCallback, (void *)this);  
#endif
  (void)QCUtils::WaitForCondition(&genericAudio_wakeupSync,COMMON_IDLE_WAIT_TIMEOUT_MSEC);
  /* Reset the file read position */  
}

/* ======================================================================
FUNCTION
  GenericAudioFile::GetToatalTimeDataRequestCallback

DESCRIPTION
  Data request call back from CMX.

DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

========================================================================== */
void GenericAudioFile::GetToatalTimeDataRequestCallback(
  uint32 offset,
  uint32 max_len,
  cmx_client_req_cb_func_ptr_type data_req_cb,
  const void *server_client_data,
  const void *client_data
)
{
  GenericAudioFile * pThis = (GenericAudioFile *)client_data;
  //m_buffer is allocated with MAX_GENERIC_DATA_BUFFER_SIZE, So make sure we never try to read more than that.
  //If CMX requests less than MAX_GENERIC_DATA_BUFFER_SIZE, we should use it rather than trying to read 2560.
  uint32 dataBytesNeeded = MIN(MAX_GENERIC_DATA_BUFFER_SIZE,max_len);
  
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if(pThis->bHttpStreaming && pThis->m_fpFetchBufferedDataSize)
  {
    boolean bEndOfData = false;
    //Pull interface so pull dnld data size from OEM
   (*(pThis->m_fpFetchBufferedDataSize))(0,&(pThis->m_wBufferOffset),&bEndOfData, pThis->m_QtvInstancehandle);
  }
#endif

  if((dataBytesNeeded + offset) > pThis->m_fileSize)
  {
    //CMX is asking beyond the file size, adjust number of bytes to read
    dataBytesNeeded = pThis->m_fileSize - offset;    
  }
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  else if( pThis->bHttpStreaming && ((offset + dataBytesNeeded ) > pThis->m_wBufferOffset ) )
  {
    //CMX wants to read more than what is being downloaded,
    //so we should return NULL buffer to CMX with 0 number of bytes as being read.
     dataBytesNeeded = 0;
     (*data_req_cb)(CMX_FAILURE, NULL, 0, server_client_data);  
     QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Enough DATA not AVAILABLE, sending NULL buffer to CMX m_wBufferOffset %d dataBytesNeeded %d offset %d",pThis->m_wBufferOffset,dataBytesNeeded,offset);     
     /*
     * Wake up waiting TCB as SetTotalTime might be called in Mpeg4Player context and 
     * when sufficient data is not available, we need to start playing.
     * If we don't do this, player will hang for-ever.
     */

     QCUtils::SetCondition(&pThis->genericAudio_wakeupSync);
     return;
  }
#endif
    
  uint32 numOfBytes = 0;  
#ifdef SHOW_CMX_DEBUG_INFO_ALL
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "CMX Audio DataReq maxLen %d",max_len);
#endif

  /* Read data from the file */

  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," GetToatalTimeDataRequestCallback,reading %d bytes from offset %d",dataBytesNeeded,offset);
  numOfBytes = readFile(pThis->m_parseFilePtr,pThis->m_buffer, offset, dataBytesNeeded );
  
  if(numOfBytes == 0)
  {
    (*data_req_cb)(CMX_FAILURE, 
                   pThis->m_buffer, 0, 
                   server_client_data);          
  }
  else
  {
    /* Send data to CMX */
    (*data_req_cb)(CMX_SUCCESS,
	                 pThis->m_buffer,
                   numOfBytes,
                   server_client_data);  
  }
}


/* ======================================================================
FUNCTION
  GenericAudioFile::GetTotalTimeCallback

DESCRIPTION
  CMX is notifying us of results of Gettotaltime.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void GenericAudioFile::GetTotalTimeCallback(
  cmx_status_type status,
  const void * client_data,
  uint32             time_ms        /* Total play time of requested CMX file */
)
{
  GenericAudioFile * pThis = (GenericAudioFile *)client_data;  
  //catch state changes.
  switch(status)
  {
    case CMX_DONE:
      /* Set the total playback time with the time with the time_ms */      
      pThis->m_totalPbTime = time_ms;
      /* Set the condition to wake up */
      QCUtils::SetCondition(&pThis->genericAudio_wakeupSync);
      break;

    case CMX_ABORTED:
    case CMX_FAILURE:
      /* Not received the Play back time. */      
      QCUtils::SetCondition(&pThis->genericAudio_wakeupSync);
      break;
    default:
      break;
  }
}


/*===========================================================================

FUNCTION  GetAudioSpec

DESCRIPTION
public method to get the Audio Spec from CMX.


===========================================================================*/
void GenericAudioFile::GetAudioSpec()
{  
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "GenericAudioFile::GetAudioSpec");

  memset( &handle, 0, sizeof(handle) );
  /* prepare the handle to send to CMX */
  handle.source_type         = CMX_SOURCE_CLIENT;
  handle.client.source_type  = CMX_SOURCE_CLIENT;
  handle.client.data_config  = CMX_CLIENT_DATA_REAL_TIME;
  /* Call back to handle the events from CMX */
  handle.client.data_req_ptr = GetAudioSpecDataRequestCallback;
  handle.client.data_len = m_fileSize;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "created the wait condition in GenericAudioFile");
  /* Call CMX API to get the audio spec */
#ifndef FEATURE_WINCE
  cmx_audfmt_get_specs(&handle, GetAudioSpecCallback, (void *)this);  
#endif
  (void)QCUtils::WaitForCondition(&genericAudio_wakeupSync,COMMON_IDLE_WAIT_TIMEOUT_MSEC);    
}

/* ======================================================================
FUNCTION
  GenericAudioFile::GetAudioSpecDataRequestCallback

DESCRIPTION
Data request call back from CMX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void GenericAudioFile::GetAudioSpecDataRequestCallback(
  uint32 offset,
  uint32 max_len,
  cmx_client_req_cb_func_ptr_type data_req_cb,
  const void *server_client_data,
  const void *client_data
)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," GenericAudioFile::GetAudioSpecDataRequestCallback");
   
  uint32 numOfBytes = 0;
  GenericAudioFile * pThis = (GenericAudioFile *)client_data;

  //m_buffer is allocated with MAX_GENERIC_DATA_BUFFER_SIZE, So make sure we never try to read more than that.
  //If CMX requests less than MAX_GENERIC_DATA_BUFFER_SIZE, we should use it rather than trying to read 2560.
  uint32 dataBytesNeeded = MIN(MAX_GENERIC_DATA_BUFFER_SIZE,max_len);
  
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if(pThis->bHttpStreaming && pThis->m_fpFetchBufferedDataSize)
  {
    boolean bEndOfData = false;
    //Pull interface so pull dnld data size from OEM
    (*(pThis->m_fpFetchBufferedDataSize))(0,&(pThis->m_wBufferOffset),&bEndOfData, pThis->m_QtvInstancehandle);
  }
#endif

  if((dataBytesNeeded + offset) > pThis->m_fileSize)
  {
    //CMX is asking beyond the file size, adjust number of bytes to read
    dataBytesNeeded = pThis->m_fileSize - offset;    
  }
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  else if( pThis->bHttpStreaming && ((offset + dataBytesNeeded ) > pThis->m_wBufferOffset ) )
  {
    //CMX wants to read more than what is being downloaded,
    //so we should return NULL buffer to CMX with 0 number of bytes as being read.
     dataBytesNeeded = 0;
     (*data_req_cb)(CMX_FAILURE, NULL, 0, server_client_data);  
     QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Enough DATA not AVAILABLE, sending NULL buffer to CMX m_wBufferOffset %d dataBytesNeeded %d offset %d",pThis->m_wBufferOffset,dataBytesNeeded,offset);          
     return;
  }
#endif

#ifdef SHOW_CMX_DEBUG_INFO_ALL
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "CMX Audio DataReq maxLen %d",max_len);
#endif
  /* Read data from file */
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," GetAudioSpecDataRequestCallback,reading %d bytes from offset %d",dataBytesNeeded,offset);
  numOfBytes = readFile(pThis->m_parseFilePtr,pThis->m_buffer, offset, dataBytesNeeded );

  if(numOfBytes == 0)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Failed to readFile");
    (*data_req_cb)(CMX_FAILURE, pThis->m_buffer, 0, server_client_data);  
  }
  else
  {
    /* Send data to CMX */
    (*data_req_cb)(CMX_SUCCESS,
                   pThis->m_buffer,
                   numOfBytes,
                   server_client_data);  
  }
}

/* ======================================================================
FUNCTION
  GenericAudioFile::GetAudioSpecCallback

DESCRIPTION
  CMX is notifying us of results of Gettotaltime.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void GenericAudioFile::GetAudioSpecCallback(
  cmx_status_type status,
  const void * client_data,
  cmx_af_audio_spec_type *audio_spec   /* Audio spec of requested file        */
)
{    
  GenericAudioFile * pThis = (GenericAudioFile *)client_data;
  //catch state changes.
  switch(status)
  {
    case CMX_SUCCESS:
      //This indicates CMX accepted the command.
      break;
    case CMX_DONE:
      GenericSetAudioSpec( client_data, (cmx_af_audio_spec_type *)audio_spec);
      pThis->m_bMediaInitialized = true;
      QCUtils::SetCondition(&pThis->genericAudio_wakeupSync);
      break;
    case CMX_ABORTED:
    case CMX_FAILURE:
      pThis->m_bMediaInitialized = false;
      QCUtils::SetCondition(&pThis->genericAudio_wakeupSync);
      break;
    default:
      break;
  }
}

/* ======================================================================
FUNCTION
  GenericAudioFile::GenericSetAudioSpec

DESCRIPTION
  Private method to set the audio spec parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void GenericAudioFile::GenericSetAudioSpec(const void * client_data,cmx_af_audio_spec_type * psd)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," GenericAudioFile::GenericSetAudioSpec");
  GenericAudioFile * pThis = (GenericAudioFile *)client_data;
  /* If the file type is of MP3 */
  if (CMX_AF_FILE_MP3 == psd->file_type)
  {
    cmx_af_mp3_spec_type *  pc = (cmx_af_mp3_spec_type *)psd;
    /* Here we need to set all the parameters that we received from CMX
       based on the requirement we can add more */
    pThis->m_bitRate = (uint32)pc->bitrate;
    if( CMX_AF_SUPRTED_OPS_MASK_SEEK & pc->supported_ops)
    {
      pThis->m_seekingAllowed = TRUE;
    }

  }
  /* If the file type is of AAC */
  else if( CMX_AF_FILE_AAC == psd->file_type )
  {
    cmx_af_aac_spec_type *  pc = (cmx_af_aac_spec_type *)psd;
    /* Here we need to set all the parameters that we received from CMX
       based on the requirement we can add more */
    pThis->m_bitRate = (uint32)pc->bit_rate;
    if( CMX_AF_SUPRTED_OPS_MASK_SEEK & pc->supported_ops)
    {
      pThis->m_seekingAllowed = TRUE;
    }
  }
  /* If the file type is of AMR */
  else if(CMX_AF_FILE_AMR == psd->file_type )
  {
    cmx_af_amr_spec_type *  pc = (cmx_af_amr_spec_type *)psd;
    /* Here we need to set all the parameters that we received from CMX
       based on the requirement we can add more */
    if( CMX_AF_SUPRTED_OPS_MASK_SEEK & pc->supported_ops)
    {
      pThis->m_seekingAllowed = TRUE;
    }
  }
  /* If the file type is of QCP_13K */
  else if(CMX_AF_FILE_QCP_13K == psd->file_type )
  {
    cmx_af_qcp_spec_type *  pc = (cmx_af_qcp_spec_type *)psd;
	/* Here we need to set all the parameters that we received from CMX
    based on the requirement we can add more */
    if( CMX_AF_SUPRTED_OPS_MASK_SEEK & pc->supported_ops)
    {
      pThis->m_seekingAllowed = TRUE;
    }
  }
  /* If the file type is of QCP_EVRC */
  else if(CMX_AF_FILE_QCP_EVRC == psd->file_type )
  {
    /* Here we need to set all the parameters that we received from CMX
       based on the requirement we can add more */
  }
  /* If the file type is of QCP_AMR */
  else if(CMX_AF_FILE_QCP_AMR == psd->file_type )
  {
    /* Here we need to set all the parameters that we received from CMX
       based on the requirement we can add more */
  }
  /* If the file type is of QCP_EFR */
  else if(CMX_AF_FILE_QCP_EFR == psd->file_type )
  {
    /* Here we need to set all the parameters that we received from CMX
       based on the requirement we can add more */
  }
  /* If the file type is of QCP_FR */
  else if(CMX_AF_FILE_QCP_FR == psd->file_type )
  {
    /* Here we need to set all the parameters that we received from CMX
       based on the requirement we can add more */
  }
  /* If the file type is of QCP_HR */
  else if(CMX_AF_FILE_QCP_HR == psd->file_type )
  {
    /* Here we need to set all the parameters that we received from CMX
    based on the requirement we can add more */
  }  
  /* If the file type is of MIDI */
  else if(CMX_AF_FILE_MIDI == psd->file_type )
  {
    /* Here we need to set all the parameters that we received from CMX
    based on the requirement we can add more */
  }  
  return;
}

/*===========================================================================

FUNCTION  getNextGenericAudioMediaSample

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
int32 GenericAudioFile::getNextGenericAudioMediaSample(uint32 /*id*/,
                                                       uint8 *buf,
                                                       uint32 size,
                                                       uint32 offset,
                                                       uint32 & /*index*/)
{
  uint32 numOfBytes = 0;
  /* Always send waht ever the data that CMX is asking to avoid any buffer
     overflows and crashes in CMX. */  
  uint32 size_to_send = size;   
   
  if( m_fileSize && (offset >= m_fileSize) && (size_to_send >= m_fileSize) )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Chunk size=%ld bigger than File size=%ld", size_to_send, m_fileSize);
    return READ_FAILURE;
  }

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if( bHttpStreaming )
  {            
    if( size_to_send > (m_fileSize - m_parseFilePtr->fileReadPos - 1) )
    {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," getNextGenericAudioMediaSample,m_fileSize %d (m_parseFilePtr->fileReadPos-1) %d ",m_fileSize,(m_parseFilePtr->fileReadPos-1));
      size_to_send = m_fileSize - m_parseFilePtr->fileReadPos - 1;
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," adjusted size_to_send %d ",size_to_send);
    }
    if(m_fpFetchBufferedDataSize)
    {
      boolean bEndOfData = false;
      //Pull interface so pull dnld data size from OEM
      (*m_fpFetchBufferedDataSize)(0,&m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);
    }    
    
    if( bHttpStreaming &&
       ( m_wBufferOffset < m_parseFilePtr->fileReadPos + MIN_NOBYTES_REQUIRED_TOSEND_TOCMX_FOR_PD ) &&
       ( m_fileSize != m_parseFilePtr->fileReadPos + 1))
    {
      /*Inform data-underrun to the player.*/
      sendHTTPStreamUnderrunEvent();      
      m_nDataOffsetWhenUnderRun = m_parseFilePtr->fileReadPos;
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "updated m_nDataOffsetWhenUnderRun %d",m_nDataOffsetWhenUnderRun);
      return INSUFFICIENT_DATA;
    }
  }
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," getNextGenericAudioMediaSample,reading %d bytes from offset %d",size_to_send,offset);
  numOfBytes = readFile( m_parseFilePtr, buf, offset, size_to_send );  

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if( bHttpStreaming )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," getNextGenericAudioMediaSample,curr m_parseFilePtr->fileReadPos %d bytes numOfBytes read %d",m_parseFilePtr->fileReadPos,numOfBytes);
    m_parseFilePtr->fileReadPos += numOfBytes;    
  }
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if( !numOfBytes )
  {
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to readFile");
  }  
  return numOfBytes;  
}

/* ======================================================================
FUNCTION
GenericAudioFile::resetMediaPlayback

DESCRIPTION
Thorough, meaningful description of what this function does.

DEPENDENCIES
List any dependencies for this function, global variables, state,
resource availability, etc.

RETURN VALUE
Enumerate possible return values

SIDE EFFECTS
Detail any side effects.

========================================================================== */
bool GenericAudioFile::resetMediaPlayback(uint32 /*id*/)
{
  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getMediaTimestampForCurrentSample

DESCRIPTION
Public method used to request the timestamp for the sample currently processed

===========================================================================*/
uint32 GenericAudioFile::getMediaTimestampForCurrentSample(uint32 /*id*/)
{
  return 1;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  resetPlayback

DESCRIPTION
Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/
void GenericAudioFile::resetPlayback()
{
  return;
}
/*===========================================================================
 
 FUNCTION  skipNSyncSamples
 
 DESCRIPTION
 Public method used to skip N number of Sync Samples.
 
 ===========================================================================*/
#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
 uint32 GenericAudioFile::skipNSyncSamples( int , uint32 , bool *, 
                                            uint32  )
 {
   return 0;
 }
#endif /* FEATURE_QTV_REPOSITION_SYNC_FRAME */

/* ======================================================================
FUNCTION
  GenericAudioFile::SeekCallback

DESCRIPTION
  CMX is notifying us of results of rewind/fforward.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void GenericAudioFile::SeekCallback(cmx_status_type status,
                                    const void * client_data)
{  
  GenericAudioFile * pThis = (GenericAudioFile *)client_data;
  //catch state changes.
  switch(status)
  {
    case CMX_SUCCESS:      
      /* Set the condition to wake up */     
      QCUtils::SetCondition(&pThis->genericAudio_wakeupSync);
      break;

    case CMX_ABORTED:
    case CMX_FAILURE:
      /* Not done the seeking */      
      QCUtils::SetCondition(&pThis->genericAudio_wakeupSync);      
      break;
    default:
      break;
  }
}

/* ======================================================================
FUNCTION
resetPlayback

DESCRIPTION
Thorough, meaningful description of what this function does.

DEPENDENCIES
List any dependencies for this function, global variables, state,
resource availability, etc.

RETURN VALUE
Enumerate possible return values

SIDE EFFECTS
Detail any side effects.

========================================================================== */
uint32 GenericAudioFile::resetPlayback (
                                 uint32 res_time,
                                 uint32 /*id*/,
                                 bool /*bSetToSyncSample*/,
                                 bool *bError,
                                 uint32 currentPosTimeStamp
                                 )
{
  *bError = FALSE;
  uint32 time_ms;  
  bool bIsFwdOK = true;
  bool isSeekIssued = false;  
   
  if(!m_bMediaInitialized)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"GenericAudioFile::resetPlayback m_bMediaInitialized is FALSE,need to wait on genericAudio_wakeupSync");    
    //Reset the condition, to invalidate if it has been set previously..
    QCUtils::ResetCondition(&genericAudio_wakeupSync);
    /* wait for condition, this condition will be set via GetAudioSpecCallback or getTrackAverageBitRate*/
    (void)QCUtils::WaitForCondition(&genericAudio_wakeupSync,COMMON_IDLE_WAIT_TIMEOUT_MSEC);      
  }
  if(!m_bMediaInitialized)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Failed to m_bMediaInitialized media,returning FALSE");    
    *bError = TRUE;
    return res_time;
  }

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if( bHttpStreaming && m_fpFetchBufferedDataSize)
  {
    boolean bEndOfData = false;
    //Pull interface so pull dnld data size from OEM
    (*m_fpFetchBufferedDataSize)(0,&m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);
  }
#endif    

  if(m_repositioningAllowed)
  {
    /* if both the reposition time and the current time stamp are equal then 
       it is resuming from pause */
    if((res_time != 0) && ( res_time == currentPosTimeStamp))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"GenericAudioFile::resetPlayback calling cmx_audfmt_fforward as res_time==currentPosTimeStamp");        
#ifndef FEATURE_WINCE
      cmx_audfmt_fforward( res_time, SeekCallback, (void *) this );   
#endif
      isSeekIssued = true;
    }
    else
    {    

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      if(bHttpStreaming)
      {
        //In PD, FWD is not allowed untill we download the entire clip.
        if( (m_fileSize) && (m_wBufferOffset != m_fileSize) &&(res_time > currentPosTimeStamp))          
        {        
          //FWD is not yet allowed, set bError so that reposition will fail and playback will continue.
          bIsFwdOK = false;
          *bError = TRUE;
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"ResetPlayback CAN't FWD as file isn't downloaded fully.. %d",m_wBufferOffset);          
        }            
      }
#endif    
      if(*bError == FALSE)
      {
        QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                      "ResetPlayback m_totalPbTime %d m_timeStamp %d res_time %d",m_totalPbTime,m_timeStamp,res_time);          
      
        //Please note that in case of PD, m_totalPbTime will 
        //not be available (will be 0)untill clip is downloaded fully.
      
        m_timeStamp = res_time;

        //Nonzero value indicates that m_totalPbTime is valid,
        //Cap out requested time with clip duration if applicable.      
        if(m_totalPbTime && (res_time >= m_totalPbTime))
        {        
          res_time = m_totalPbTime;
          m_timeStamp = res_time;
          QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"ResetPlayback updated m_timeStamp %d res_time %d",m_timeStamp,res_time);          
        }

        //if the requested time stamp is > current time stamp then fast forward.
        if( res_time > currentPosTimeStamp )
        {
          if(bIsFwdOK)
          {
            time_ms = res_time - currentPosTimeStamp;           
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"calling cmx_audfmt_fforward with time_ms %d",time_ms);          
#ifndef FEATURE_WINCE
            cmx_audfmt_fforward( time_ms, SeekCallback, (void *) this );
#endif
            isSeekIssued = true;
            m_repositioned = true;
          }
        }
        //if the requred new time stamp is less than the current time stamp then rewind
        else
        {
        /* This is for rapid rewind case and we keep on getting res_time and currentPosTimeStamp with 0.
           In this case call cmx_audfmt_rewind with DEFAULT_SEEK_TIME_MSEC as time_ms */
        if((res_time == 0) && (currentPosTimeStamp == 0))
        {
          time_ms = DEFAULT_SEEK_TIME_MSEC;			
        }
        else
        {
          time_ms = currentPosTimeStamp - res_time;        
        }
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"calling cmx_audfmt_rewind with time_ms %d",time_ms);          
#ifndef FEATURE_WINCE
          cmx_audfmt_rewind( time_ms, SeekCallback, (void *) this );
#endif
          isSeekIssued = true;
          m_repositioned = true;
        }    
      }    
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"GenericAudioFile::resetPlayback failed Repositiong");    
  //Need to wait for condition only if reposition command was issued to CMX.
  }
  if(isSeekIssued)
  {   
    /* wait for condition we are setting this condition in the SeekCallback*/
    (void)QCUtils::WaitForCondition(&genericAudio_wakeupSync,COMMON_IDLE_WAIT_TIMEOUT_MSEC);  
    //When reposition, reset file read offset to 0;
    m_parseFilePtr->fileReadPos = 0;
  }    
  return res_time;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  peekCurSample

DESCRIPTION

===========================================================================*/
uint32 GenericAudioFile::peekCurSample(uint32 /*trackid*/, 
                                       file_sample_info_type* /*pSampleInfo*/)
{
  return (uint32)DEFAULT_ERROR;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTitle

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getTitle() const
{
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getAuthor

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getAuthor()const
{
  return NULL;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getVersion

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getVersion()const
{
  return NULL;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getCopyright

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getCopyright()const
{
  return NULL;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getDescription

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getDescription()const
{
  return NULL;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getRating

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getRating() const
{
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getperf

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getPerf() const
{
  return NULL;
}

/*===========================================================================

FUNCTION  getGenre

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getGenre() const
{
  return NULL;
}

/*===========================================================================

FUNCTION  getClsf

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getClsf() const
{
  return NULL;
}
/*===========================================================================

FUNCTION  getKywd

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getKywd() const
{
  return NULL;
}

/*===========================================================================

FUNCTION  getLoci

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getLoci() const
{
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getCreationDate

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING GenericAudioFile::getCreationDate()const
{
  return NULL;
}

/*===========================================================================

FUNCTION  getAlbum

DESCRIPTION
Public method used to request Album

===========================================================================*/
OSCL_STRING GenericAudioFile::getAlbum() const
{
  return NULL;
}

/*===========================================================================

FUNCTION  getYrrc

DESCRIPTION
Public method used to request Year of recording

===========================================================================*/
OSCL_STRING GenericAudioFile::getYrrc() const
{
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  ~GenericAudioFile

DESCRIPTION
Destructor for the GenericAudioFile class

===========================================================================*/
GenericAudioFile::~GenericAudioFile()
{
  if(m_parseFilePtr)
  {
    (void)OSCL_FileClose( m_parseFilePtr ); 
  }
  if(m_buffer)
  {
    QTV_Delete_Array(m_buffer);
    m_buffer = NULL;
  }
  /* Destroy the condition */
  QCUtils::DestroyCondition(&genericAudio_wakeupSync);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getMovieDuration

DESCRIPTION
Get total movie duration

===========================================================================*/
//Here we have calculate the movie duration based on the way that they are calculating in CMX.
uint32 GenericAudioFile::getMovieDuration() const
{ 
 return m_totalPbTime;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackVideoFrameRate

DESCRIPTION
Get total movie duration

===========================================================================*/
float  GenericAudioFile::getTrackVideoFrameRate(uint32 /*id*/)
{
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackContentVersion

DESCRIPTION
Get total movie duration

===========================================================================*/
int16 GenericAudioFile::getTrackContentVersion(uint32 /*id*/)
{
  return 0;
} // from 'cver' atom at track level

/* <EJECT> */
/*===========================================================================

FUNCTION  trackRandomAccessDenied

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8 GenericAudioFile::trackRandomAccessDenied(uint32 /*id*/)
{
  return 0; // by default allow random access
}// from 'rand' atom at track level

/* <EJECT> */
/*===========================================================================

FUNCTION  trackDependsOn

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32  GenericAudioFile::trackDependsOn(uint32 /*id*/)
{
  return 0;
}

/* ======================================================================
FUNCTION
getTrackMediaDuration

DESCRIPTION
Thorough, meaningful description of what this function does.

DEPENDENCIES
List any dependencies for this function, global variables, state,
resource availability, etc.

RETURN VALUE
Enumerate possible return values

SIDE EFFECTS
Detail any side effects.

========================================================================== */
uint32 GenericAudioFile::getTrackMediaDuration(uint32 /*id*/)
{
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackMediaTimescale

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 GenericAudioFile::getTrackMediaTimescale(uint32 /*id*/)
{
  return 0;
}

/*===========================================================================

FUNCTION  getTrackAudioSamplingFreq

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 GenericAudioFile::getTrackAudioSamplingFreq(uint32 /*id*/)
{
  return 8000;
}

/* <EJECT> */

/*===========================================================================

FUNCTION  getAudioSamplesPerFrame

DESCRIPTION
Get total number of audio (codec) samples per MPEG4 sample entry
(which may contain one or more MPEG4 frames).

NOTE: Each MPEG4 frame contains a number of codec samples

===========================================================================*/
uint32 GenericAudioFile::getAudioSamplesPerFrame(uint32 /*id*/)
{
  return 2560;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  isGenericAudioFileInstance

DESCRIPTION
Return that it is a geniric Audio instance

===========================================================================*/
bool GenericAudioFile::isGenericAudioFileInstance()
{
  return true;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackOTIType

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8  GenericAudioFile::getTrackOTIType(uint32 /*id*/)
{
  uint8 format = 0xFF;
  if ( m_audioFormatType )
  {
    switch(m_audioFormatType)
    {
      case AUDIO_MIDI:
        format = (uint8)MIDI_AUDIO;
        break;
      case NONMP4_AUDIO_MP3:
        format = (uint8)NONMP4_MP3_AUDIO;
        break;
      case NONMP4_AUDIO_AAC:
        format = (uint8)NONMP4_AAC_AUDIO;
        break;
      case AUDIO_QCP:
        format = (uint8)QCP_AUDIO;
        break;
      case AUDIO_VND_QCELP:
        format = (uint8)VND_QCELP_AUDIO;
        break;
      case AUDIO_QCF:
        format = (uint8)QCF_AUDIO;
        break;
      case AUDIO_MMF:
        format = (uint8)MMF_AUDIO;
        break;
      case AUDIO_AAC:
        format = (uint8)NONMP4_AAC_AUDIO;
        break;
      case AUDIO_IMELODY:
        format = (uint8)IMELODY_AUDIO;
        break;
      case AUDIO_ADPCM:
        format = (uint8)ADPCM_AUDIO;
        break;
      case AUDIO_AMR:
        format = (uint8)NONMP4_AMR_AUDIO;
        break;
      case AUDIO_WMA:
        format = (uint8)WMA_AUDIO;
        break;
      case AUDIO_HVS:
        format = (uint8)HVS_AUDIO;
        break;
      case AUDIO_SAF:
        format = (uint8)SAF_AUDIO;
        break;
      case AUDIO_XMF:
        format = (uint8)XMF_AUDIO;
        break;
      case AUDIO_DLS:
        format = (uint8)DLS_AUDIO;
        break;
      case QCP_QLCM:
        format = (uint8)QCP_QLCM_AUDIO;
        break;
      default:
        break;
    }
  }
  return format;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTracAudioFormat

DESCRIPTION
Gets the audio format

===========================================================================*/
uint8 GenericAudioFile::getTrackAudioFormat(uint32 /*id*/)
{
  return 0xFF;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackMaxBufferSizeDB

DESCRIPTION
Get total movie duration
This function takes care of long overflow by taking MAX.

===========================================================================*/
//need to change to the max expected sample size in CMX.
int32 GenericAudioFile::getTrackMaxBufferSizeDB(uint32 /*id*/)
{
  return 2560;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackAverageBitrate

DESCRIPTION
Get total movie duration

===========================================================================*/
int32 GenericAudioFile::getTrackAverageBitrate(uint32 /*id*/)
{
  return m_bitRate;
}


/*===========================================================================

FUNCTION  getTrackAverageBitrate

DESCRIPTION
Get total movie duration

===========================================================================*/
int32 GenericAudioFile::setTrackAverageBitrate(uint32 bitRate)
{
  m_bitRate =  bitRate;  
  if(!m_bMediaInitialized)
  {    
    m_bMediaInitialized = true;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "GenericAudioFile::setTrackAverageBitrate,setting m_bMediaInitialized to TRUE");
    QCUtils::SetCondition(&genericAudio_wakeupSync);
  }
  return true;
}

/*===========================================================================

FUNCTION  setRepositioningAllowedFlag

DESCRIPTION
Set the repositining allowed gflag

===========================================================================*/
void GenericAudioFile::setRepositioningAllowedFlag(bool repositioningAllowed)
{
  if(repositioningAllowed)
  {
    m_repositioningAllowed = true;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "GenericAudioFile::setRepositioningAllowedFlag,setting m_repositioningAllowed to TRUE");  
  }
  else
  {
    m_repositioningAllowed = false;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "GenericAudioFile::setRepositioningAllowedFlag,setting m_repositioningAllowed to FALSE");  
  }
  return;
}

/*===========================================================================

FUNCTION  IsGenericAudioFileMediaInitialized

DESCRIPTION
Return the value of m_bMediaInitialized flag

===========================================================================*/
bool GenericAudioFile::IsGenericAudioFileMediaInitialized()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
         "GenericAudioFile::IsGenericAudioFileMediaInitialized");
  return m_bMediaInitialized;
}

/*===========================================================================

FUNCTION  IsGenericAudioFileRepositioned

DESCRIPTION
Return the value of m_repositioned flag

===========================================================================*/
bool GenericAudioFile::IsGenericAudioFileRepositioned()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
         "GenericAudioFile::IsGenericAudioFileRepositioned");
  return m_repositioned;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackMaxBitrate

DESCRIPTION
Get total movie duration

===========================================================================*/
int32 GenericAudioFile::getTrackMaxBitrate(uint32 /*id*/)
{
  return 128;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getMovieTimescale

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 GenericAudioFile::getMovieTimescale() const
{
  return 1000;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackVideoFrameWidth

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 GenericAudioFile::getTrackVideoFrameWidth(uint32 /*id*/)
{
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackVideoFrameHeight

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 GenericAudioFile::getTrackVideoFrameHeight(uint32 /*id*/)
{
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  GetNumAudioChannels

DESCRIPTION
returns the number of audio channels

==========================================================================*/
unsigned long GenericAudioFile::GetNumAudioChannels(int /*id*/)
{
  return 1;
}


/* <EJECT> */
/*===========================================================================

FUNCTION  randomAccessDenied

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8 GenericAudioFile::randomAccessDenied()
{
  //return  !m_seekingAllowed;
	return false;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getAllowVideoOnly

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8 GenericAudioFile::getAllowVideoOnly()
{
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getAllowAudioOnly

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8 GenericAudioFile::getAllowAudioOnly()
{
  return 1;
}

/*===========================================================================

FUNCTION  getNumTracks

DESCRIPTION
Get number of tracks

===========================================================================*/
int32 GenericAudioFile::getNumTracks()
{
  m_bAudioPresentInClip = TRUE;
  m_bVideoPresentInClip = FALSE;
  m_bTextPresentInClip = FALSE;
  m_playAudio = TRUE;
  return 1;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  isAudioPresentInClip()

DESCRIPTION
  Returns TRUE if AUDIO track is detected in the Clip.
  This is useful in Notifying UNSUPPORTED_AUDIO_TRACK if
  we fail to detect correct codec type but clip has AUDIO TRACK in it.

===========================================================================*/
bool  GenericAudioFile::isAudioPresentInClip()
{
  return m_bAudioPresentInClip;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackWholeIDList

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 GenericAudioFile::getTrackWholeIDList(uint32* /*ids*/)
{
  return 1;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  GetTimeStampFromMedia

DESCRIPTION
Get the currenty time stamp
===========================================================================*/
uint32 GenericAudioFile::GetTimeStampFromMedia()
{
  /* Resetting m_repositioned to false. It tells that we issued a 
    reposition command to CMX and also we read the new updated m_timeStamp */
  m_repositioned = false;
  return m_timeStamp;
}

/*===========================================================================

FUNCTION  getLargestFrameSize

DESCRIPTION
  Public method used to find the largest frame size in a given track.

===========================================================================*/
uint32 GenericAudioFile::getLargestFrameSize ( uint32 /*id*/ )
{
  return 0;
}


#endif /*FEATURE_QTV_GENERIC_AUDIO_FORMAT*/

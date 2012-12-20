/* =======================================================================
                               VDECODER.CPP
DESCRIPTION
  Implementation of the VDEC API shim functions

EXTERNALIZED FUNCTIONS
  Everything in vdecoder.h.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  - call vdec_parse_media_header to read the 'preamble' in the data.
  - hand the VDEC_CONFIG returned by the previous step to vdec_create.
  - using the handle returned by the previous state, call vdec_queue
    and vdec_use_frame_buffer to provide input and output buffers
    to the decoder.
  - call vdec_decode (more than once).
  - call vdec_destroy to clean up.

Copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/core/rel/2.0/src/vdecoder.cpp#11 $
$DateTime: 2009/10/20 05:30:33 $
$Change: 1058225 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/08/09    vs      Remove the dependencies of other modules.
--------   ---      ---------------------------------------------------------
06/30/09    as      Added Function level profiling feature
========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "vdecoder.h"
#include "vdecoder_i.h"


/* Determines whether codec supports only single instance of itself or not */
#define IS_WMV_CODEC(x) \
        ((strncmp(x,"wmv1",VDEC_FOURCC_CHARS) == 0) ||  \
        (strncmp(x,"wmv2",VDEC_FOURCC_CHARS) == 0) ||  \
        (strncmp(x,"wmv3",VDEC_FOURCC_CHARS) == 0))

#define IS_REAL_CODEC(x) (strncmp(x,"rv40",VDEC_FOURCC_CHARS) == 0)

/* Initialize the list to default for each instance */
VDEC_INSTANCE_LIST gInstanceList[MAX_NO_OF_INSTANCES] = 
                   {{FALSE,0,""},{FALSE,0,""}};

rex_crit_sect_type gInstanceList_cs;
bool gInstanceListCSInit = false;
/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

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
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/* ======================================================================
FUNCTION
  vdec_create

DESCRIPTION
  Creates a new decoder.

PARAMETERS
 const char * const      pFourcc
   The fourcc parameter specifies the FOURCC code for the media type. Without 
   the correct FOURCC code, this function will be unable to create the video 
   decoder correctly

 VDEC_STREAM_ID *const   pStreamID
   After the decoder has been successfully created vdec_create will return
   a handle to the created decoder via pStreamID. If an error occurs, pStreamID
   will be NULL and more information will be available via return error code.   

RETURN VALUE
   This function returns a VDEC_ERROR that describes the outcome of the 
   function call

========================================================================== */
VDEC_ERROR vdec_create
(
  const char * const      pFourcc,
  VDEC_STREAM_ID *const   pStreamID
)
{ 
  VDEC_ERROR  err = VDEC_ERR_OPERATION_UNSUPPORTED;
  uint32  uLoop;
  if(gInstanceListCSInit == FALSE)
  {
    rex_init_crit_sect( &gInstanceList_cs);
    for (uLoop=0; uLoop < MAX_NO_OF_INSTANCES; uLoop++)
    {
      rex_init_crit_sect(&gInstanceList[uLoop].cs);  
    }
    gInstanceListCSInit = TRUE;
  }

#ifdef FEATURE_QTV_VIDEO_DECODER_PROFILING
#error code not present
#endif

  rex_enter_crit_sect( &gInstanceList_cs );
  for (uLoop = 0; uLoop < MAX_NO_OF_INSTANCES; uLoop++)
  {
    if (gInstanceList[uLoop].bValid == TRUE)
    {
      if((IS_WMV_CODEC((char *)pFourcc) && IS_WMV_CODEC(gInstanceList[uLoop].fourCC)) ||
         (IS_REAL_CODEC((char *)pFourcc) && IS_REAL_CODEC(gInstanceList[uLoop].fourCC)))
      {
        /* Can not support more than one instance for WMV and REAL */
        err = VDEC_ERR_OPERATION_UNSUPPORTED;
        rex_leave_crit_sect( &gInstanceList_cs );
        return err;
      }
    }
  }

  for (uLoop = 0; uLoop < MAX_NO_OF_INSTANCES; uLoop++)
  {
    if (gInstanceList[uLoop].bValid == FALSE)
    {
      *pStreamID = (VDEC_STREAM_ID)VideoDecoder::Create(pFourcc, &err);
      if(*pStreamID == NULL)
      {
        err = VDEC_ERR_OUT_OF_MEMORY;
      }
      if (err == VDEC_ERR_EVERYTHING_FINE)
      {
        gInstanceList[uLoop].bValid = TRUE;
        std_strlcpy((char *)&gInstanceList[uLoop].fourCC,pFourcc,
                                         VDEC_FOURCC_STRLEN);
        gInstanceList[uLoop].StreamID = *pStreamID;
        break;
      }
    }
  }
  rex_leave_crit_sect( &gInstanceList_cs );

  return err;
}

/* ======================================================================
FUNCTION
  vdec_initialize

DESCRIPTION
  Initializes an already created decoder.

PARAMETERS
  VDEC_STREAM_ID            stream
    The stream id of the decoder which has to be initialized. This id 
    is obtained by a previous successful call to vdec_create.

  VDEC_BLOB *               pConfig
    Optional pointer to configuration information for the video stream 
    obtained from a previous successful call to vdec_dump; if the configuration 
    information is not being restored, this parameter must be set to NULL 
  
  uint16                    nLayers
    The number of layers in the video stream.  For simple streams,
    this will be 1.

  VDEC_MALLOC_FN            pfnFrameBufferMalloc
  VDEC_FREE_FN              pfnFrameBufferFree
    Pointers to malloc and free functions to be used by the decoder
    to manage frame buffers.  If the application wants control over
    where frame buffers are allocated, it can control that through
    these function pointers.

  VDEC_EVENT_CB_FN          pfnEventCb
    Callback function to be called by the decoder when an event is generated, 
    such as when a decoded frame is available; when frame buffers are passed 
    to the application, they remain property of the decoder, and it is up to 
    the application to return them to the decoder via vdec_reuse_frame_buffer.

  void *             pEventCbData
    Client data passed to the event callback; unused by the decoder provided 
    as a convenience for the application

  VDEC_DIMENSIONS           frameSize
    The source format of video frames in this stream; the decoder will use 
    this value if the source format cannot be obtained from the bitstream

  VDEC_CONCURRENCY_CONFIG  concurrencyConfig
    The concurrency requirements associated with this video stream. 

RETURN VALUE
    This function returns a VDEC_ERROR that describes the outcome of the 
    function call

========================================================================== */
VDEC_ERROR vdec_initialize
(
  const VDEC_STREAM_ID      stream,
  VDEC_BLOB *               pConfig,
  uint16                    nLayers,
  VDEC_MALLOC_FN            pfnFrameBufferMalloc,
  VDEC_FREE_FN              pfnFrameBufferFree,
  VDEC_EVENT_CB_FN          pfnEventCb,
  void *                    pEventCbData,
  VDEC_DIMENSIONS           frameSize,
  VDEC_CONCURRENCY_CONFIG   concurrencyConfig
)
{
  VideoDecoder *pDec;

  if (VideoDecoder::IsInstanceValid(stream) == -1)
 {
    return VDEC_ERR_INVALID_STREAM_ID;
 }
  pDec = ( VideoDecoder * )stream;

 return pDec->Initialize( pConfig,
                          nLayers,
                          pfnFrameBufferMalloc,
                          pfnFrameBufferFree,
                          pfnEventCb,
                          pEventCbData,
                          frameSize,
                          concurrencyConfig);
}

/* ======================================================================
FUNCTION
  vdec_destroy

DESCRIPTION
  Destroys an existing decoder.

PARAMETERS
  const VDEC_STREAM_ID stream
    The ID of the of the decoder to be destroyed.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

SIDE EFFECTS
  The stream ID provided to this function will be invalid after
  this function is called.

========================================================================== */
VDEC_ERROR vdec_destroy
(
  const VDEC_STREAM_ID stream
)
{
  VideoDecoder *pDec;
  int nIndex;
  nIndex = VideoDecoder::IsInstanceValid(stream);
  if (nIndex == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  
#ifdef FEATURE_QTV_VIDEO_DECODER_PROFILING
#error code not present
#endif

  pDec = ( VideoDecoder * )stream;
  rex_enter_crit_sect(&gInstanceList[nIndex].cs);
  pDec->Destroy();
  Vdec_Delete( pDec );
  rex_leave_crit_sect(&gInstanceList[nIndex].cs);

  rex_enter_crit_sect( &gInstanceList_cs );
  gInstanceList[nIndex].bValid = FALSE;
  gInstanceList[nIndex].StreamID = NULL ;
  rex_leave_crit_sect( &gInstanceList_cs );

  return VDEC_ERR_EVERYTHING_FINE;
}

/* ======================================================================
FUNCTION
  vdec_dump

DESCRIPTION
  Serializes a running decoder into a buffer and returns the buffer to
  the application.

PARAMETERS
  const VDEC_STREAM_ID stream
    VDEC_STREAM_ID of the decoder; must be a stream identifier obtained 
	from a previous successful call to vdec_create.

  VDEC_BLOB * const       pBlob
    Handle through which the buffer is returned

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

SIDE EFFECTS
  The buffer provided to the application becomes the property of the app.
  It's up to the caller to free the buffer when it's no longer needed by
  calling vdec_free_blob

========================================================================== */
VDEC_ERROR vdec_dump
(
  const VDEC_STREAM_ID    stream,
  VDEC_BLOB * const       pBlob
)
{
  
  if (VideoDecoder::IsInstanceValid(stream) == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }

  VDEC_USE_ARG1(pBlob);

  //return pDec->Dump( ppBuf_ret, pBufSize_ret );
  return VDEC_ERR_OPERATION_UNSUPPORTED;
}

/* ======================================================================
FUNCTION
  vdec_restore

DESCRIPTION
  Creates a decoder from a previous dump.

PARAMETERS
  VDEC_BYTE * pBuf
    Pointer to the buffer containing the previous decoder's dump.

  unsigned long bufSize
    The size of the buffer.

  VDEC_ERROR * pErr_out
    A pointer through which detailed errors can be returned.

RETURN VALUE
  The VDEC_STREAM_ID for the new decoder.  If an error occurs, the stream
  ID will be NULL and more information will be available via pErr_out.

========================================================================== */
VDEC_STREAM_ID vdec_restore
(
  VDEC_BYTE *   pBuf,
  unsigned long /* UNUSED right now: bufSize */,
  VDEC_ERROR *  pErr_out
)
{
  if ( pBuf == NULL || pErr_out == NULL )
  {
    if (pErr_out != NULL)
    {
      *pErr_out = VDEC_ERR_NULL_POINTER;
    }
    return NULL;
  }

  *pErr_out = VDEC_ERR_OPERATION_UNSUPPORTED;
  return NULL;
}

/* ======================================================================
FUNCTION
  vdec_reuse_frame_buffer

DESCRIPTION
  Returns a frame buffer to the decoder for reuse in output.

PARAMETERS
  const VDEC_STREAM_ID stream
    The stream ID of the decoder.

  VDEC_FRAME * const   pFrame
    The frame being returned to the decoder.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

SIDE EFFECTS
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

VDEC_ERROR vdec_reuse_frame_buffer
(
  const VDEC_STREAM_ID stream,
  VDEC_FRAME *const   pFrame
)
{
  VideoDecoder *pDec;

  if (VideoDecoder::IsInstanceValid(stream) == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;

  return pDec->ReuseFrameBuffer( pFrame );
}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION
  vdec_queue

DESCRIPTION
  Queues input bitstreams for consumption by the decoder.  Note that no
  actual decoding of queued data will occur until vdec_decode is called.
  Exactly one natural unit of data must be submitted with queue, with 
  the exception of H.264 where it can contain a multiple number of natural 
  units.  The definition of a natural unit depends on the decoder type; 
  for MPEG-4, JPEG, Windows Media Video, and RealVideo based decoders, 
  a natural unit consists of all the data associated for one frame; 
  for H.264, it is one NAL.

PARAMETERS
  const VDEC_STREAM_ID      stream
    The stream ID of the decoder.

  VDEC_INPUT_BUFFER * const pInput
    Data from the bitstream(s).

  VDEC_QUEUE_CB_FN          pfnQueueCb
    A callback function to be called by the decoder when this input buffer
    has been processed.  This is how the decoder returns 'used' buffers
    to the application.

  void *  const             pQueueCbData
    Client data passed to the callback.  Unused by the decoder - provided
    as a convenience for the application.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

VDEC_ERROR vdec_queue( const VDEC_STREAM_ID      stream,
                       VDEC_INPUT_BUFFER * const pInput,
                       VDEC_QUEUE_CB_FN          pfnQueueCb,
                       void *  const             pQueueCbData )
{
  VDEC_ERROR    err  ( VDEC_ERR_EVERYTHING_FINE );
  VideoDecoder *pDec;

  if (VideoDecoder::IsInstanceValid(stream) == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;
  err = pDec->Queue( pInput, pfnQueueCb, pQueueCbData );
  return err;

}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION
  vdec_queue_eos

DESCRIPTION
  queues a EOS buffer.

PARAMETERS
  const VDEC_STREAM_ID stream
    The stream ID of the decoder.

  nLayers Number of layers

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

SIDE EFFECTS
  None.
========================================================================== */

VDEC_ERROR vdec_queue_eos( const VDEC_STREAM_ID      stream,
                           uint32 nLayers)
{
  VDEC_ERROR    err  ( VDEC_ERR_EVERYTHING_FINE );
  VideoDecoder *pDec;

  if (VideoDecoder::IsInstanceValid(stream) == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;

  /* check whether decoder supports EOS */
  err = pDec->Queue_eOS();

  if(err == VDEC_ERR_EVERYTHING_FINE)
  {
    err = pDec->Queue_eOS_Buffer(nLayers);
  }
  return err;
}

/* ======================================================================
FUNCTION
  vdec_decode

DESCRIPTION
  Decodes video frames from queued input buffers.  Frames, if any become
  available are provided to the application via the provided callback.

PARAMETERS
  const VDEC_STREAM_ID stream
    The stream ID of the decoder.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked, and why we returned
  ( used up all the input data?  or timed out? )

NOTES
  Decode is not guaranteed to produce a completed video frame.  Each call
  to decode will do 'some' work.  It's up to the application to keep
  calling Decode until the video frame it wants is returned.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

VDEC_ERROR vdec_decode
(
  const VDEC_STREAM_ID stream
)
{
  VDEC_ERROR ErrorCode;
  VideoDecoder *pDec;
  int nIndex;
  nIndex = VideoDecoder::IsInstanceValid(stream);
  if (nIndex == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;
  rex_enter_crit_sect(&gInstanceList[nIndex].cs);
  ErrorCode = pDec->Decode( );
  rex_leave_crit_sect(&gInstanceList[nIndex].cs);
  return ErrorCode;
}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  vdec_flush

DESCRIPTION
  Flushes input and output queues, and resets the decoder in preparation
  for new input.

PARAMETERS
  const VDEC_STREAM_ID stream
    The stream ID of the decoder.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR vdec_flush
(
  const VDEC_STREAM_ID stream
)
{
  VideoDecoder *pDec;
  VDEC_ERROR ErrorCode = VDEC_ERR_INVALID_STREAM_ID;
  int nIndex = -1;

  nIndex = VideoDecoder::IsInstanceValid(stream);
  if (nIndex == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;

  rex_enter_crit_sect(&gInstanceList[nIndex].cs);
  ErrorCode = pDec->Flush( );   
  rex_leave_crit_sect(&gInstanceList[nIndex].cs);

  return ErrorCode;
 
}

/* ======================================================================
FUNCTION
  vdec_free_blob

DESCRIPTION
  Frees the buffer stored inside a BLOB.  Applications should call this
  to free buffers inside a decoder-allocated BLOB.

PARAMETERS
  VDEC_BLOB *pBlob
    A pointer to the BLOB.

RETURN VALUE
  None.

========================================================================== */
VDEC_ERROR  vdec_free_blob
(
  VDEC_BLOB *pBlob
)
{
  VDEC_ERROR retCode;
  retCode = VideoDecoder::FreeBLOB( pBlob );
  return retCode;
}

/* ======================================================================
FUNCTION
  vdec_suspend

DESCRIPTION
  This function suspends decoding and brings the decoder to a state to 
  allow for another  application to be launched . It can be called to 
  serialize the decoder prior to a call to vdec_dump, and must be called 
  prior to vdec_destroy

PARAMETERS
  const VDEC_STREAM_ID stream
    The stream ID of the decoder.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR vdec_suspend
(
  const VDEC_STREAM_ID stream
)
{
  VideoDecoder *pDec;

  if (VideoDecoder::IsInstanceValid(stream) == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;

  return pDec->Suspend( );
}

/* ======================================================================
FUNCTION
  vdec_resume

DESCRIPTION
  This function resumes decoding after it has been suspended by vdec_suspend. 

PARAMETERS
  const VDEC_STREAM_ID stream
    The stream ID of the decoder.
  VDEC_CONCURRENCY_CONFIG concurrencyConfig
    The concurrency requirements. This value was originally specified to 
	the decoder as a parameter to vdec_initialize.
	
RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR vdec_resume
(
  const VDEC_STREAM_ID    stream,
  VDEC_CONCURRENCY_CONFIG concurrencyConfig
)
{
  VideoDecoder *pDec;

  if (VideoDecoder::IsInstanceValid(stream) == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;

  return pDec->Resume( concurrencyConfig );
}

/* ======================================================================
FUNCTION
  vdec_set_parameter

DESCRIPTION
  Sets codec-specific parameters

PARAMETERS
  VDEC_STREAM_ID         stream
    The stream ID of the decoder.
  VDEC_PARAMETER_ID      parameter
    The parameter ID to set.
  VDEC_PARAMETER_DATA *  const pParameterData
    The parameter data.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR vdec_set_parameter
(
  const VDEC_STREAM_ID        stream,
  const VDEC_PARAMETER_ID     parameter,
  VDEC_PARAMETER_DATA * const pParameterData
)
{
  VideoDecoder *pDec;

  if (VideoDecoder::IsInstanceValid(stream) == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;

  return pDec->SetParameter( parameter, pParameterData );
}

/* ======================================================================
FUNCTION
  vdec_get_parameter

DESCRIPTION
  Sets codec-specific parameters

PARAMETERS
  VDEC_STREAM_ID        stream
    The stream ID of the decoder.
  VDEC_PARAMETER_ID     parameter
    The parameter ID to get.
  VDEC_PARAMETER_DATA * pParameterData
    The parameter data.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR vdec_get_parameter
(
  const VDEC_STREAM_ID  stream,
  const VDEC_PARAMETER_ID  parameter,
  VDEC_PARAMETER_DATA * pParameterData
)
{
  VideoDecoder *pDec;

  if (VideoDecoder::IsInstanceValid(stream) == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;

  return pDec->GetParameter( parameter, pParameterData );
}


/* ======================================================================
FUNCTION
  vdec_scale

DESCRIPTION
  Start the dsp xScalar post processing. Start scaling all the queued frames
  which are still pending decode.

PARAMETERS
  VDEC_STREAM_ID stream
    The stream ID of the decoder.
  VDEC_DIMENSIONS outFrameSize
    output dimensions of the scaled (up/down) video frame.
  VDEC_SCALE_CB_FN scaleCbFn
    callback function to return the status of the
    vdec_scale function call. We need this since the decoder might not know
    the source frame input dimension until it decodes the first frame. The 
    client may call vdec_create -> vdec_scale -> vdec_queue -> vdec_decode
    Decode will use the callback to specify if vdec_scale was successful or
    not.
  void * const pCbData
    the client data the application passed in.


RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR vdec_scale( VDEC_STREAM_ID stream, 
                       VDEC_DIMENSIONS outFrameSize,
                       VDEC_SCALE_CB_FN scaleCbFn,
                       void * const     pCbData
                     )
{
  VideoDecoder *pDec;

  if (VideoDecoder::IsInstanceValid(stream) == -1)
  {
    return VDEC_ERR_INVALID_STREAM_ID;
  }
  pDec = ( VideoDecoder * )stream;

  return pDec->Scale( outFrameSize, scaleCbFn, pCbData );
}


/* =======================================================================
                               VDECODER_I.CPP
DESCRIPTION
  Implementation of the VDEC API MP4 base class.

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

$Header: //source/qcom/qct/multimedia/qtv/decoder/core/rel/2.0/src/vdecoder_i.cpp#12 $
$DateTime: 2009/07/28 03:13:21 $
$Change: 979818 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/08/09    vs      Remove the dependencies of other modules.
--------   ---      ---------------------------------------------------------
========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "vdecoder_i.h"
#include "vdecoder_utils.h"
#include "assert.h"
#include "comdef.h"
#include "AEEstd.h"

#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A_TURBO
#error code not present
#endif

extern VDEC_INSTANCE_LIST gInstanceList[MAX_NO_OF_INSTANCES];
extern rex_crit_sect_type gInstanceList_cs;

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

char VideoDecoder::m_fourcc[5];
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

// As of the time I write this (20050420) VideoPlayer/VideoDec still
// expect the nonstandard behavior of always returning the input buffer
// after a decode even if some layers in the buffer have not been consumed.
//
// Defining this symbol causes VideoDecoder objects to release input buffers
// early, as soon as they're touched.  Once VideoPlayer/VideoDec get sorted
// out, we'll undefine this to return to the 'correct' behavior.

#define ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS

const int DEFAULT_MAX_STATS_QUEUE_LENGTH = 4;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
list_type VideoDecoder::sm_ftypRegistry;
bool      VideoDecoder::sm_registryIsInitialized =
  VideoDecoder::InitializeFtypRegistry();

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
#ifndef T_MSM7500
//extern "C" void qdsp_memory_dump_on_video_fatal(void);
//extern "C" void qdsp_set_dspwdog_timeout_memory_dump(void);
#endif /* T_MSM7500 */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  VideoDecoder::InitializeFtypRegistry

DESCRIPTION
  Initializes the static ftyp registry.

PARAMETERS
  None.

RETURN VALUE
  bool: true good, false bad.
========================================================================== */
bool VideoDecoder::InitializeFtypRegistry( void )
{
  list_init( &sm_ftypRegistry );

#ifdef FEATURE_MP4_DECODER
  extern VideoDecoder::VDEC_FTYP_HANDLER_RECORD MP4Decoder_handlerRecord;
  RegisterWithFtypRegistry( &MP4Decoder_handlerRecord );
  
  extern VideoDecoder::VDEC_FTYP_HANDLER_RECORD H263Decoder_handlerRecord;
  RegisterWithFtypRegistry( &H263Decoder_handlerRecord );    
  
  #ifdef FEATURE_QTV_DIVX_DECODER
#error code not present
  #endif //end of FEATURE_QTV_DIVX_DECODER
#endif

#ifdef FEATURE_H264_DECODER
#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A_TURBO
#error code not present
#endif
  {
    extern VideoDecoder::VDEC_FTYP_HANDLER_RECORD H264Decoder_handlerRecord;
    RegisterWithFtypRegistry( &H264Decoder_handlerRecord );
  }
#endif

#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#endif /* FEATURE_QTV_OSCAR_DECODER */

#ifdef FEATURE_REAL_PLAYER_RV9_USE_DSP
  extern VideoDecoder::VDEC_FTYP_HANDLER_RECORD REALDecoder_handlerRecord;
  RegisterWithFtypRegistry( &REALDecoder_handlerRecord );
#endif /* FEATURE_REAL_PLAYER_RV9_USE_DSP */

#if defined (FEATURE_QTV_SKT_MOD) && !defined (PLATFORM_LTK)
  extern VideoDecoder::VDEC_FTYP_HANDLER_RECORD JPEGDecoder_handlerRecord;
  RegisterWithFtypRegistry( &JPEGDecoder_handlerRecord );
#endif

#ifdef FEATURE_QTV_WINDOWS_MEDIA
  extern VideoDecoder::VDEC_FTYP_HANDLER_RECORD WMVDecoder_handlerRecord;
  RegisterWithFtypRegistry( &WMVDecoder_handlerRecord );
#endif

  return true;
}

/* ======================================================================
FUNCTION
  VideoDecoder::RegisterWithFtypRegistry

DESCRIPTION
  Stores a VDEC_FTYP_HANDLER_RECORD in the vdec registry.

PARAMETERS
  None.

RETURN VALUE
  None.
========================================================================== */
bool VideoDecoder::RegisterWithFtypRegistry
(
  VDEC_FTYP_HANDLER_RECORD* const pRecord
)
{
  ASSERT( pRecord != NULL );
  list_push_back( &sm_ftypRegistry, &( pRecord->link ) );
  return true;
}

/* ======================================================================
FUNCTION
  VideoDecoder::FindFtypInRegistry

DESCRIPTION
  Locates a VDEC_FTYP_HANDLER_RECORD for the given ftyp in the vdec registry.

PARAMETERS
  const char* const ftyp
    the ftyp we're trying to match.

RETURN VALUE
  VDEC_FTYP_HANDLER_RECORD*
    a pointer to the handler record we found.
========================================================================== */
VideoDecoder::VDEC_FTYP_HANDLER_RECORD* VideoDecoder::FindFtypInRegistry
(
  const char* const ftyp
)
{
  VDEC_FTYP_HANDLER_RECORD* pRecord;

  ASSERT( ftyp != NULL );

  // Walk the registry list
  //
  for
  (
    pRecord = ( VDEC_FTYP_HANDLER_RECORD* )list_peek_front( &sm_ftypRegistry );
    pRecord != NULL;
    pRecord = ( VDEC_FTYP_HANDLER_RECORD* )list_peek_next( &sm_ftypRegistry,
                                                           &( pRecord->link ) )
  )
  {
    // For each record we obtain, walk the ftyp list and look for a match.
    //
    for
    (
      const char * pFtyp = pRecord->ftyps;
      *pFtyp != NULL; /* pointing at a NULL means we're at EOS */
      pFtyp += FTYP_SIZE
    )
    {
      if( strncmp( pFtyp, ftyp, FTYP_SIZE ) == 0 )
      {
        // We've found a match.  Report our glorious victory.
        //
        return pRecord;
      }
    }
  }

  // No match.  So sad.
  //
  return NULL;
}

/* ======================================================================
FUNCTION:
  VideoDecoder::IsInstanceValid

DESCRIPTION:
  Checks the validity of stream ID

PARAMETERS:
   stream : stream ID

RETURN VALUE:
  TRUE/FALSE

SIDE EFFECTS:
  none.
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

int VideoDecoder::IsInstanceValid(VDEC_STREAM_ID stream)
{
  uint32 uLoop;
  
  if (stream == NULL)
  {
    return(-1);
  }
  rex_enter_crit_sect( &gInstanceList_cs );  
  for (uLoop = 0; uLoop < MAX_NO_OF_INSTANCES; uLoop++)
  {
    if (gInstanceList[uLoop].bValid == TRUE)
    {
      if (gInstanceList[uLoop].StreamID == stream)
      {
        rex_leave_crit_sect( &gInstanceList_cs );
        return uLoop;
      }
    }
  }
  rex_leave_crit_sect( &gInstanceList_cs );
  return(-1);
}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  VideoDecoder::Create

DESCRIPTION
  Creates a new decoder.

PARAMETERS
  const char * const      pFourcc
    The fourcc parameter specifies the FOURCC code for the media type. Without 
   the correct FOURCC code, this function will be unable to create the video 
   decoder correctly

  VDEC_ERROR * pErr_out
    A pointer to a VDEC_ERROR which allows vdec_create to provide more
    information in case creation fails.

RETURN VALUE
  The VDEC_STREAM_ID for the newly created decoder. If an error occurs the
  stream ID will be NULL and more information will be available via pErr_out.
========================================================================== */
VideoDecoder* VideoDecoder::Create
(
	const char * const      pFourcc,
	VDEC_ERROR *            pErr_out
)
{

  VDEC_FTYP_HANDLER_RECORD* pHandler = NULL;
  if ( pErr_out == NULL )
  {
    return NULL;
  }

  pHandler = FindFtypInRegistry( pFourcc );
  if ( pHandler == NULL )
  {
    *pErr_out = VDEC_ERR_UNKNOWN_FTYP;
    return NULL;
  }

   std_strlcpy(VideoDecoder::m_fourcc,pFourcc,5);
   
   return (pHandler->pCreateFn )( pErr_out );
}

/* ======================================================================
FUNCTION
  VideoDecoder::VideoDecoder

DESCRIPTION
  Creates a new decoder.

PARAMETERS
  const DECODER_TYPE decoderType
    Indicates the decoder type :- mp4, H264 , wmv, jpeg etc

RETURN VALUE
  None, it's a constructor.  However, error codes are returned through
  pErr_out.
========================================================================== */
VideoDecoder::VideoDecoder(const DECODER_TYPE decoderType)
: m_maxStatsQueueLength(DEFAULT_MAX_STATS_QUEUE_LENGTH),
  cm_decoderType( decoderType ),
  m_isxScalerEnabled(false)
{
  ClearStats( );

   //Turn on this flag when decoder is initialized via a call to vdec_initialize.
   //This flag is used to ensure that the application initializes the decoder by
   //calling vdec_initialize before queuing and decoding bitstream data.
   m_bDecoderInitialized = false;
#ifdef FEATURE_QTV_XSCALE_VIDEO
  /* the OEM enabled xScaler, flip the flag */
  m_isxScalerEnabled = true;
#endif /* FEATURE_QTV_XSCALE_VIDEO */
  
  /* Initilize EOS buffer */
  m_eOS_pInputBuffer = &m_eOS_InputBuffer;
  isEoSBufferQueued = false;
}

/* ======================================================================
FUNCTION
  VideoDecoder::~VideoDecoder

DESCRIPTION
  Destroys an existing decoder.

PARAMETERS
  None.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

SIDE EFFECTS
  The stream ID provided to this function will be invalid after
  this function is called.

========================================================================== */
VideoDecoder::~VideoDecoder( void )
{
  FrameQNode *pF;
  q_type* pQ = NULL;

  Flush();

  pQ = m_frameQ.Lock();
  // Empty the frame queue, mark each frame unused, and return them.
  //
  while ( ( pF = ( FrameQNode * )q_get( pQ ) ) != NULL )
  {
    ReleaseNode( pF );
  }
  m_frameQ.Unlock( pQ );

}

/* ======================================================================
FUNCTION
  VideoDecoder::Dump

DESCRIPTION
  Serializes a running decoder into a buffer and returns the buffer to
  the application.

PARAMETERS
  VDEC_BYTE ** ppBuf_ret
    Handle through which the buffer is returned.

  unsigned long * pBufSize_ret
    Pointer through which the buffer size is returned.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

SIDE EFFECTS
  The buffer provided to the application becomes the property of the app.
  It's up to the caller to free the buffer when it's no longer needed.

========================================================================== */
VDEC_ERROR VideoDecoder::Dump
(
  VDEC_BYTE ** const ppBuf_ret,
  unsigned long *    pBufSize_ret
)
{
  if ( ppBuf_ret == NULL || pBufSize_ret == NULL )
  {
    return VDEC_ERR_NULL_POINTER;
  }

  *ppBuf_ret = NULL;
  *pBufSize_ret = 0;

  return VDEC_ERR_OPERATION_UNSUPPORTED;
}

/* ======================================================================
FUNCTION
  VideoDecoder::Queue

DESCRIPTION
  Queues input bitstreams for consumption by the decoder.  Note that no
  actual decoding of queued data will occur until vdec_decode is called.

PARAMETERS
  VDEC_INPUT_BUFFER * pInput
    Data from the bitstream(s).

  unsigned short nLayer
    the layer to which this buffer belongs

  VDEC_QUEUE_CB_FN pFnCb
    A callback function to be called by the decoder when this input buffer
    has been processed.  This is how the decoder returns 'used' buffers
    to the application.

  void * pCbData
    Client data passed to the callback.  Unused by the decoder - provided
    as a convenience for the application.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

VDEC_ERROR VideoDecoder::Queue
(
  VDEC_INPUT_BUFFER * const pInput,
  VDEC_QUEUE_CB_FN          pFnCb,
  void * const              pCbData
)
{
  InputQNode* pNode    ( 0 );
  q_type*     pInputQ  ( 0 );
  bool        bHaveData( false );
  int         i        ( 0 );

  if ( pInput == NULL || pFnCb == NULL )
  {
    return VDEC_ERR_NULL_POINTER;
  }

  if(m_bDecoderInitialized == false)
  {
      VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR,
           "Error - vdec_initialize not called before calling vdec_queue!" );
	  return VDEC_ERR_RESOURCE_UNAVAILABLE;
  }

  // Check and make sure there's at least one layer's worth of data here.
  // If we've been given an empty buffer, spit it back.
  // 
  for ( i = 0; i < pInput->layers; ++i )
  {
    if ( pInput->buffer_size[ i ] > 0 )
    {
      bHaveData = true;
      break;
    }
  }

  if ( !bHaveData )
  {
    VDEC_MSG_PRIO( VDECDIAG_VIDEO_TASK, 
                  VDECDIAG_PRIO_LOW, 
                  "vdec_queue given empty buffer, rejecting..." );
    return VDEC_ERR_NO_INPUT_AVAILABLE;
  }
  
  // Mark all layers as frame-not-pending...we'll set those flags true
  // as appropriate during decode.
  //
  for ( i = 0; i < pInput->layers; ++i )
  {
    pInput->bFrameEventPending[ i ] = false;

    /*default eos indicator to false always*/
    pInput->eOSIndicator[ i ] = false;
  }

  pNode = AllocateInputNode( pInput, pFnCb, pCbData, this );
  if ( pNode == NULL )
  {
    return VDEC_ERR_OUT_OF_MEMORY;
  }

#ifndef PLATFORM_LTK
  for ( i = 0; i < pInput->layers; ++i )
  {
      if ( pInput->buffer_size[ i ] > 0 )
      { 
        if (std_strcmp(VideoDecoder::m_fourcc,"avc1"))
        {
           log_bitstream_buffer( pInput->buffer[i],
                    pInput->buffer_size[i] ); 
        }
        break;
      }
  }
#endif /* PLATFORM_LTK */ 

  pInputQ = m_inputQ.Lock();
  q_put( pInputQ, &( pNode->link ) );
  m_inputQ.Unlock( pInputQ );

  return VDEC_ERR_EVERYTHING_FINE;
}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION
  VideoDecoder::Queue_eOS

DESCRIPTION
  deafult function.always return unsupported if the underlying decoder 
  doesnt support

PARAMETERS


RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR VideoDecoder::Queue_eOS()
{
    //default no support for eOS buffer
    return VDEC_ERR_OPERATION_UNSUPPORTED;
}

/* ======================================================================
FUNCTION
  VideoDecoder::Queue_eOS_Buffer

DESCRIPTION
  Internal function within vdec to queue a eos buffer to decoder

PARAMETERS
  nLayers - number of input layers

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR VideoDecoder::Queue_eOS_Buffer(uint32 nLayers)
{
  int i=0;
  InputQNode* pNode    ( 0 );
  q_type*     pInputQ  ( 0 );
  //Queue the eOSBuffer
  if (nLayers == 0)
  {
    return VDEC_ERR_OPERATION_UNSUPPORTED;
  }

  if(m_eOS_pInputBuffer == NULL || isEoSBufferQueued)
  {
    //Buffer already queued or already queued once and we didnt
    // receive any flush..so why queue again and again
    return VDEC_ERR_EVERYTHING_FINE;

    //todo: remove isEoSBufferQueued.it is responsibility of APP
    // to call eos once.
  }

  for (i = 0;i < nLayers; i++)
  {
    m_eOS_InputBuffer.bFrameEventPending[i] = false;
    m_eOS_InputBuffer.buffer[i] = NULL;
    m_eOS_InputBuffer.buffer_pos[i] = 0;
    m_eOS_InputBuffer.buffer_size[i] = 10; //just dummy
    m_eOS_InputBuffer.eOSIndicator[i] = true;
    m_eOS_InputBuffer.layers = nLayers;
    m_eOS_InputBuffer.timestamp[i] = 0;
    m_eOS_InputBuffer.delta[i] = 0;
    m_eOS_InputBuffer.userData[i] = NULL;
  }

  //Allocate input buffer
  pNode = AllocateInputNode( &m_eOS_InputBuffer, eOSBufferFreeCallback, this, this );
  if (pNode == NULL)
  {
    return VDEC_ERR_OUT_OF_MEMORY;
  }
  m_eOS_pInputBuffer = NULL; //our buffer is now out
  pInputQ = m_inputQ.Lock();
  q_put( pInputQ, &( pNode->link ) );
  m_inputQ.Unlock( pInputQ );
  isEoSBufferQueued = true;
  return VDEC_ERR_EVERYTHING_FINE;
}

/* ======================================================================
FUNCTION
  VideoDecoder::eOSBufferFreeCallback

DESCRIPTION
  Inputbuffer free handler for EOS buffer

PARAMETERS
  VDEC_STREAM_ID
  VDEC_INPUT_BUFFER
  void*

RETURN VALUE
  void
========================================================================== */
void VideoDecoder::eOSBufferFreeCallback
(
const VDEC_STREAM_ID      /* unused */ ,
VDEC_INPUT_BUFFER * const pBuffer,
void * const              pCbData
)
{
  VideoDecoder * const pThis = ( VideoDecoder * )pCbData;
  ASSERT( pThis );

  
  pThis->eOSBufferFreeHandler(pBuffer);
  
}

/* ======================================================================
FUNCTION
  VideoDecoder::eOSBufferFreeHandler

DESCRIPTION
  Inputbuffer free handler for EOS buffer

PARAMETERS
  VDEC_INPUT_BUFFER

RETURN VALUE
  void
========================================================================== */
void VideoDecoder::eOSBufferFreeHandler(VDEC_INPUT_BUFFER * const pBuffer)
{
  ASSERT( pBuffer );
  //Buffer returned back..we can reuse it now.
  m_eOS_pInputBuffer = pBuffer;
}

/* ======================================================================
FUNCTION
  VideoDecoder::ReuseFrameBuffer

DESCRIPTION
  Returns a frame buffer to the decoder for reuse in output.

PARAMETERS
  VDEC_FRAME * pFrame
    The frame being returned to the decoder.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

SIDE EFFECTS
  None.

========================================================================== */
VDEC_ERROR VideoDecoder::ReuseFrameBuffer
(
  VDEC_FRAME * const pFrame
)
{
  FrameQNode *pNode;
  q_type* pFrameQ = NULL;

  if ( pFrame == NULL )
  {
    return VDEC_ERR_NULL_POINTER;
  }

  pFrameQ = m_frameQ.Lock();

  // Scan the frame queue for the node pointing to this frame, and move
  // it to the free queue since the application is now done with it.
  //
  for
  (
    pNode  = ( FrameQNode * )q_check( pFrameQ );
    pNode != NULL;
    pNode  = ( FrameQNode * )q_next( pFrameQ, &( pNode->link ) )
  )
  {
    if ( pFrame == &( pNode->frame ) )
    {
      q_delete( pFrameQ, &( pNode->link ) );
      ReleaseNode( pNode );
      break;
    }
  }

  m_frameQ.Unlock( pFrameQ );

  return VDEC_ERR_EVERYTHING_FINE;
}

/* ======================================================================
FUNCTION
  VideoDecoder::Flush

DESCRIPTION
  Flushes input and output queues, and resets the decoder in preparation
  for new input.

PARAMETERS
  None.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

VDEC_ERROR VideoDecoder::Flush( void )
{
  InputQNode *pI;

  q_type* pQ = NULL;

  pQ = m_inputQ.Lock();

  // Empty the input queue and return all the buffers.
  //
  while ( ( pI = ( InputQNode * )q_get( pQ ) ) != NULL )
  {
    ReleaseNode( pI );
  }

  m_inputQ.Unlock( pQ );
  isEoSBufferQueued = false;

  return VDEC_ERR_EVERYTHING_FINE;
}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION
  VideoDecoder::AllocateBLOB

DESCRIPTION
  Initializes a BLOB structure and allocates its pData memory.

PARAMETERS
  const VDEC_BLOBTYPE type
    The type this BLOB is to have.
  const uint32        size
    How many bytes to allocate for the BLOB's data.

RETURN VALUE
  A pointer to the new VDEC_BLOB.

========================================================================== */
VDEC_BLOB* VideoDecoder::AllocateBLOB
(
  const char * const  fourcc,
  const VDEC_BLOBTYPE type,
  uint32              size
)
{
  VDEC_BLOB *pBlob;

  if ( size == 0 )
  {
    // They may not have any data to save in the BLOB, but because
    // of the way we defined the struct, you'll always get at least one
    // byte anyway...setting size to 1 here prevents us from allocating
    // too little memory to hold all of the VDEC_BLOB header structure.
    //
    size = 1;
  }

  // Allocate the header (the BLOB struct) and the data in one
  // contiguous block.
  //
  // The -1 accounts for the fact that we had to waste a byte while
  // declaring the data array to be of size 1 in the struct definition.
  //
  pBlob = (VDEC_BLOB *)Vdec_Malloc( sizeof( VDEC_BLOB ) + size - 1 );
  if ( pBlob != NULL )
  {
    memset( (VDEC_BLOB *)pBlob, 0, ( sizeof( VDEC_BLOB ) + size - 1 ) );
    std_strlcpy( pBlob->fourcc, fourcc, VDEC_FOURCC_STRLEN );
    pBlob->type   = type;
    pBlob->size   = size;
  }

  return pBlob;
}

/* ======================================================================
FUNCTION
  VideoDecoder::FreeBLOB

DESCRIPTION
  Frees BLOB-associated memory allocated by AllocateBLOB.

PARAMETERS
  VDEC_BLOB &blob
    The BLOB!

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR VideoDecoder::FreeBLOB
(
  VDEC_BLOB *pBlob
)
{

  if ( pBlob == NULL )
    return VDEC_ERR_CORRUPT_BLOB;

    // Because the blob was allocated as only one chunk of memory,
    // we only need one call to 'free'.
    //
    Vdec_Free( pBlob );

  return VDEC_ERR_EVERYTHING_FINE;
}

/* ======================================================================
FUNCTION
  VideoDecoder::GetStats

DESCRIPTION
  Fetches performance statistics for the decoder.

PARAMETERS
  VDEC_STATS * pStats_out
    A pointer to the structure through which the stats will be returned.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR VideoDecoder::GetStats
(
  VDEC_STATS * const pStats_out
)
{
  if ( pStats_out == NULL )
  {
    return VDEC_ERR_NULL_POINTER;
  }

  // If a simple copy won't do for a subclass, they'll have to override
  // this member.  I'm guessing we won't see that case, though.
  //
  memcpy( pStats_out, &m_stats, sizeof( VDEC_STATS ) );

  return VDEC_ERR_EVERYTHING_FINE;
}

/* ======================================================================
FUNCTION
  VideoDecoder::ClearStats

DESCRIPTION
  Resets performance statistic counters for the decoder.

PARAMETERS
  None.

RETURN VALUE
  None.

========================================================================== */
void VideoDecoder::ClearStats( void )
{
  // If just clearing the structure isn't enough in specialized cases,
  // subclasses are welcome to override this member.
  //
  memset( &m_stats, 0, sizeof( VDEC_STATS ) );
}

/* ======================================================================
FUNCTION
  VideoDecoder::GetLayerIDWithEarliestTimeStamp

DESCRIPTION
  Takes an array buffer pointers containing a buffers per layer and their
  corresponding timestamps and returns the layer that has the earliest time
  stamp. This is used by the decoder to send the earliest buffer to be
  decoded.

PARAMETERS
  **buffer   -> pointer to a set of buffer pointers (one per layer)
  *size      -> size of each of the buffers
  *timestamp -> timestamp of each layer
  nlayers    -> total number of layers in the clip

RETURN VALUE
  layer id for the layer that is next in line to be decoded.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

int VideoDecoder::GetLayerIDWithEarliestTimeStamp(
  uint32 NumBytes[MAX_MP4_LAYERS],        /* array of nr. of bytes to decode        */
  uint64 TimeStamp[MAX_MP4_LAYERS],       /* array of timestamps returned w/ rgb    */
  uint8  nlayers                          /* Number of layers in current clip       */
)
{

  unsigned long long   EarliestTimeStamp;
  int      EarliestTimeStampID;

#ifdef PLATFORM_LTK
#error code not present
#else  
  EarliestTimeStamp = 0xFFFFFFFFFLL;
#endif /* PLATFORM_LTK */

  EarliestTimeStampID = -1;   /* indicate nothing selected */

  for(int count =0; count <nlayers; count ++)
  {
    if(NumBytes[count ] )
    {
      /*
      ** Assuming fExternClock == 1
      */
      if ( (TimeStamp[count ] &0x0FFFFFFFFULL) < EarliestTimeStamp ) {
        EarliestTimeStamp = TimeStamp[count ];
        EarliestTimeStampID = count ;
      }
    }
  }

  return EarliestTimeStampID;
}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  VideoDecoder::WatchdogCallback

DESCRIPTION
  A generic DSP-watchdog callback.  This just creates a
  MP4_DECODE_FATAL_ERROR message and sends it to the application via
  m_pFrameCb.

PARAMETERS
  pThis:  a this pointer!

RETURN VALUE
  None.
========================================================================== */
void VideoDecoder::WatchdogCallback( void *pThisAsVoid )
{
  VideoDecoder *pThis = ( VideoDecoder* )pThisAsVoid;

  ASSERT( pThis );

  VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR,
           "DSP watchdog timeout!  Generating VDEC_STATUS_FATAL_ERROR" );

#ifndef T_MSM7500
  /* Set the dsp fatal error code to dsp_watchdog_timeout and 
   * dump the dsp memory to efs.
  */
  //qdsp_set_dspwdog_timeout_memory_dump();
  //qdsp_memory_dump_on_video_fatal();
#endif /* T_MSM7500 */

#ifdef FEATURE_QTV_DSP_ERRS_ARE_FATAL
  /* Use this feature with caution! It is not desirable or necessary to
   * assert here because without it we will report the error and exit
   * gracefully. This feature should only be used to help debug obscure
   * problems when it is not possible to set a breakpoint (for example
   * when doing field trials with FFAs).
   */
  ASSERT(0);
#endif

  if ( pThis->m_pEventCb )
  {
    ( pThis->m_pEventCb )( pThis,
                           VDEC_STATUS_FATAL_ERROR,
                           NULL,
                           NULL,
                           pThis->m_pEventCbData );
  }
}

/* ======================================================================
FUNCTION
  VideoDecoder::DecoderType

DESCRIPTION
  Return the type of this decoder object.

PARAMETERS
  None.

RETURN VALUE
  VideoDecoder::DECODER_TYPE
    The type of the decoder.
========================================================================== */
VideoDecoder::DECODER_TYPE VideoDecoder::DecoderType( void ) const
{
  return cm_decoderType;
}

/* ======================================================================
FUNCTION
  VideoDecoder::AllocateInputNode

DESCRIPTION
  Obtains and initializes an inputQNode for use by the decoder.
  If one's available on the free queue, great, we'll use that.
  Otherwise, we malloc a new one.

PARAMETERS
  VDEC_INPUT_BUFFER * const pBuf,
  VDEC_QUEUE_CB_FN          pFnCb,
  void * const              pCbData,
  VideoDecoder * const      pDecoder

    Initializers for the inputQ structure.

RETURN VALUE
  InputQNode*: the newly 'allocated' node.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

VideoDecoder::InputQNode* VideoDecoder::AllocateInputNode
(
  VDEC_INPUT_BUFFER * const pBuf,
  VDEC_QUEUE_CB_FN          pFnCb,
  void * const              pCbData,
  VideoDecoder * const      pDecoder
)
{
  InputQNode * pNode( Vdec_New( InputQNode ) );
  if ( pNode != NULL )
  {
    q_link( pNode, &( pNode->link ) );

    pNode->pBuf     = pBuf;
    pNode->pFnCb    = pFnCb;
    pNode->pCbData  = pCbData;
    pNode->pDecoder = pDecoder;
  }

  return pNode;
}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION
  VideoDecoder::ReleaseNode

DESCRIPTION
  Returns a now-unused queue node to the free queue for reuse.

PARAMETERS
  InputQNode* const pNode
    The node to be released.

RETURN VALUE
  None.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

void VideoDecoder::ReleaseNode( InputQNode* const pNode )
{
  if ( pNode )
  {
    // Return the buffer to the application for reuse.

    ( pNode->pFnCb )( this, pNode->pBuf, pNode->pCbData );

    Vdec_Delete( pNode );
  }
}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION
  VirtualInputBuffer::VirtualInputBuffer

DESCRIPTION
  Constructor. Scans the input queue and produces a virtual input buffer
  containing the first record in each layer - see header file for details.

PARAMETERS
  VideoDecoder&
    The VideoDecoder using this node manager.  We'll use this decoder's
    queues.

RETURN VALUE
  None.

========================================================================== */
VideoDecoder::VirtualInputBuffer::VirtualInputBuffer( VideoDecoder &decoder )
: m_decoder( decoder )
{
  InputQNode *pNode;
  int         layersRemaining = 0;
  int         i;
  q_type*     pInputQ = NULL;

  // Null everything out first.

  memset( &m_virtualBuffer, NULL, sizeof( m_virtualBuffer ) );
  memset( &m_aNodes,        NULL, sizeof( m_aNodes ) );

  // Get a reference to the head of the input queue.  We'll use this guy's
  // 'layers' value to know how many layers we need to populate.  This
  // should be plenty safe - if we're getting input buffers with fluctuating
  // numbers of layers, something is seriously broken.  ( A layer with no
  // data in it is still in the buffer array, it's just NULLed and has
  // a zero buffer_size...)

  pInputQ = m_decoder.m_inputQ.Lock();
  pNode = ( InputQNode* )q_check( pInputQ );
  if ( pNode == NULL )
  {
    // If there's nothing on the input queue, well, heck, we're already done.
    m_decoder.m_inputQ.Unlock( pInputQ );
    return;
  }

  m_virtualBuffer.layers = pNode->pBuf->layers;
  layersRemaining        = pNode->pBuf->layers;

  // Now, walk the list until we've filled in our input buffer.

  do
  {
    ASSERT( m_virtualBuffer.layers == pNode->pBuf->layers );

    for ( i = 0; i < m_virtualBuffer.layers; ++i )
    {
      // If we don't have a reference node at this layer yet and
      // the buffer_size in this node at this layer is non-zero,
      // we'll reference this node at this layer.

      if ( m_aNodes[ i ] == NULL && pNode->pBuf->buffer_size[ i ] != 0 )
      {
        m_aNodes[ i ] = pNode;

        m_virtualBuffer.buffer     [ i ] = pNode->pBuf->buffer     [ i ];
        m_virtualBuffer.timestamp  [ i ] = pNode->pBuf->timestamp  [ i ];
        m_virtualBuffer.delta      [ i ] = pNode->pBuf->delta      [ i ];
        m_virtualBuffer.buffer_size[ i ] = pNode->pBuf->buffer_size[ i ];
        m_virtualBuffer.buffer_pos [ i ] = pNode->pBuf->buffer_pos [ i ];
        m_virtualBuffer.userData   [ i ] = pNode->pBuf->userData   [ i ];
        m_virtualBuffer.bFrameEventPending[i] = pNode->pBuf->bFrameEventPending[i];
        m_virtualBuffer.eOSIndicator[i] = pNode->pBuf->eOSIndicator[i];

        // Since we found a match, we can decrement the count of
        // layers remaining.  If this hits zero, we can avoid scanning
        // the rest of the queue.
        --layersRemaining;

      } // if we'll copy this layer into the virtual buffer
    } // for all layers in the virtual buffer

    pNode = ( InputQNode* )q_next( pInputQ, &( pNode->link ) );
  }
  while ( pNode != NULL && layersRemaining > 0 );

  m_decoder.m_inputQ.Unlock( pInputQ );
}

/* ======================================================================
FUNCTION
  VirtualInputBuffer::~VirtualInputBuffer

DESCRIPTION
  Destructor.  Propogate changes from the virtual buffer back to the
  real ones.  Release any buffers which end up completely consumed.

PARAMETERS
  None.

RETURN VALUE
  None.

========================================================================== */
VideoDecoder::VirtualInputBuffer::~VirtualInputBuffer( void )
{
  InputQNode *pNode = NULL;
  q_type* pInputQ = NULL;
#ifndef ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS
  bool        bAllLayersConsumed;
#endif // !ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS
#ifdef ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS  
  bool        abLayerConsumed[ VDEC_MAX_LAYERS ];
#endif
  pInputQ = m_decoder.m_inputQ.Lock();

  // A note on the ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS stuff here:
  // You'll note that we don't check to see if anything was actually
  // CONSUMED when ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS is on.
  // If the input buffer was referenced at all, we're returning it.
  // To do the check correctly, we'd want to see if a non-zero buffer
  // size became zero.
  //
  // However, we don't need to do it correctly when
  // ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS is defined, because there
  // will never be more than one input buffer in the queue.
  // We'll always return that buffer, so it's sufficient to just
  // copy the output data and return the node without thinking about it.

  // Propogate changes back into the real buffers

  for ( int vLayer = 0; vLayer < m_virtualBuffer.layers; ++vLayer )
  {
    pNode = m_aNodes[ vLayer ];

    // Was data consumed at this layer?  If so, the sizes won't match.
    //
#ifdef ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS  
    abLayerConsumed[ vLayer ] = 
      ( 
        pNode && pNode->pBuf &&
        ( 
          pNode->pBuf->buffer_size[ vLayer ] != 
          m_virtualBuffer.buffer_size[ vLayer ]
        )
      );
#endif
    if ( ( pNode != NULL ) && ( pNode->pBuf != NULL ) )
    {
      // We only need to copy output values here.  The other stuff
      // won't change, so why copy it?

      pNode->pBuf->buffer_size[ vLayer ] =
        m_virtualBuffer.buffer_size[ vLayer ];
      pNode->pBuf->bFrameEventPending[ vLayer ] =
        m_virtualBuffer.bFrameEventPending[ vLayer ];
      pNode->pBuf->buffer_pos[ vLayer ] =
        m_virtualBuffer.buffer_pos[ vLayer ];
      pNode->pBuf->timestamp[ vLayer ] =
        m_virtualBuffer.timestamp[ vLayer ];
      pNode->pBuf->delta[ vLayer ] =
        m_virtualBuffer.delta[ vLayer ];

#ifndef ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS
      // Has this change resulted in the complete consumption of this
      // input buffer?  If it has, release the buffer and return
      // the node to the free queue.

      bAllLayersConsumed = true;
      for ( int i = 0; i < pNode->pBuf->layers; ++i )
      {
        if ( pNode->pBuf->buffer_size[ i ] != 0 )
        {
          bAllLayersConsumed = false;
        }
      }

      // Hey!  Wait a second!  Why aren't we worried about returning
      // input buffers in proper FIFO order?  By just returning whole buffers
      // as soon as we notice they're exhausted, couldn't we end up
      // returning them out of order?
      //
      // The answer to this question is yes, we could, IF the decoder
      // ever consumed more than one frame per decode, which it doesn't.
      // Since at most one frame will be consumed, at most one buffer
      // will come free, and there are no ordering concerns.

      if ( bAllLayersConsumed == true )
      {
        // Pull the node out of the input queue.

        q_delete( pInputQ, &( pNode->link ) );

        m_decoder.ReleaseNode( pNode );
      }
#endif // !ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS
    } // if pNode valid
  } // for every virtual-buffer layer

#ifdef ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS

   /* In the ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS case, there should be
     exactly one input buffer on the queue.  All our layer references
     should be into that node.
     Of course, there can also be zero if we tried a decode on an empty queue.
     Nothing wrong with that. */

  if (q_cnt(pInputQ ) > 1)
  {
     VDEC_MSG_PRIO1(VDECDIAG_GENERAL, VDECDIAG_PRIO_HIGH,
                  "Multiple input buffers queued: %d",q_cnt(pInputQ));
  }

  // However, in temporal scalability it's possible to have a NULL
  // reference at one layer and not the other (think end-of-clip)
  // so we gotta scan for the non-null reference before freeing.

  for ( int vLayer = 0; vLayer < m_virtualBuffer.layers; ++vLayer )
  {
    pNode = m_aNodes[ vLayer ];
    if ( abLayerConsumed[ vLayer ] ) // if this is true, valid pNode implied.
    {
      q_delete( pInputQ, &( pNode->link ) );

      m_decoder.ReleaseNode( pNode );

      break; // there's only one node, so there will be only one match
    }
  }

#endif // ALWAYS_RETURN_TOUCHED_INPUT_BUFFERS

  m_decoder.m_inputQ.Unlock( pInputQ );
}

/* ======================================================================
FUNCTION
  VirtualInputBuffer::operator->
  VirtualInputBuffer::GetBuffer

DESCRIPTION
  Return the VDEC_INPUT_BUFFER encapsulated by the VirtualInputBuffer.
  The operator-> allows virtual buffers to be used in place of their
  contained pointers.

PARAMETERS
  None.

RETURN VALUE
  InputQNode*

========================================================================== */
VDEC_INPUT_BUFFER* VideoDecoder::VirtualInputBuffer::operator->( void )
{
  return &m_virtualBuffer;
}

VDEC_INPUT_BUFFER* VideoDecoder::VirtualInputBuffer::GetBuffer( void )
{
  return operator->();
}

/* ======================================================================
FUNCTION
  VideoDecoder::AllocateFrameNode

DESCRIPTION
  Obtains and initializes a frameQNode.

PARAMETERS
  None.

RETURN VALUE
  FrameQNode*: the newly 'allocated' node.

========================================================================== */
VideoDecoder::FrameQNode* VideoDecoder::AllocateFrameNode( void )
{
   FrameQNode* pNode( ( FrameQNode* )Vdec_Malloc( sizeof( FrameQNode ) ) );

   if ( pNode )
   {
      q_link( pNode, &pNode->link );
      std_memset( &pNode->frame, 0, sizeof( VDEC_FRAME ) );
   }

   return ( pNode );
}

/* ======================================================================
FUNCTION
  VideoDecoder::ReleaseNode

DESCRIPTION
  Returns a now-unused queue node to the free queue for reuse.

PARAMETERS
  InputQNode* const pNode
    The node to be released.

RETURN VALUE
  None.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

void VideoDecoder::ReleaseNode( FrameQNode* const pNode )
{
   if ( pNode )
   {
      Vdec_Free( pNode );
   }
}

/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#ifndef PLATFORM_LTK
/* ======================================================================
FUNCTION
  LogArmDecodeTime_stats

DESCRIPTION
  Logs the Peak and Average Arm decode time for decoding.

PARAMETERS
  average and peak decode time in ms and video type
RETURN VALUE
  None.

NOTES
  None.
========================================================================== */
void VideoDecoder::LogArmDecodeTime_stats(
                                const long average_dec_time_ms,
                                const long peak_dec_time_ms,
                                const uint8 Video_type)

{
  log_vdec_arm_decode_stats_type *pBuf = NULL ;


  /* Check if logging is turned on */
  if (!log_status( LOG_VDEC_ARM_VIDEO_DECODE_STATS ) )
  {
      return;
  }
  /* Allocate a buffer */
  pBuf = (log_vdec_arm_decode_stats_type*) log_alloc( LOG_VDEC_ARM_VIDEO_DECODE_STATS, sizeof(*pBuf) );

  if ( pBuf )
  {
    /* Fill in the statistics */
    pBuf->average_dec_time_ms = average_dec_time_ms;
    pBuf->peak_dec_time_ms = peak_dec_time_ms;
    pBuf->video_type_played = Video_type;
    /* Log the buffer */
    log_commit( pBuf );
  }
}
#endif /* PLATFORM_LTK */

/* ======================================================================
FUNCTION
  VideoDecoder::Suspend

DESCRIPTION
  Suspends ... currently no behavior identified for base class.

PARAMETERS
  None.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR VideoDecoder::Suspend( void )
{
  return VDEC_ERR_EVERYTHING_FINE;
}

/* ======================================================================
FUNCTION
  VideoDecoder::Resume

DESCRIPTION
  Resumes ... currently no behavior identified for base class.

PARAMETERS
  VDEC_CONCURRENCY_CONFIG concurrencyConfig
    The concurrency requirements associated with this video stream.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR VideoDecoder::Resume( VDEC_CONCURRENCY_CONFIG )
{
  return VDEC_ERR_EVERYTHING_FINE;
}

/* ======================================================================
FUNCTION
  VideoDecoder::Scale

DESCRIPTION
  Scale the frame... currently no behavior identified for base class.

PARAMETERS
  VDEC_DIMENSIONS outFrameSize
    output dimensions of the scaled (up/down) video frame.
  VDEC_SCALE_CB_FN scaleCbFn
    callback function to return the status of the
    Scale() function call. We need this since the decoder might not know
    the source frame input dimension until it decodes the very first frame.
  void * const pCbData
    the client data the application passed in.

RETURN VALUE
  A VDEC_ERROR tells the world if everything worked.

========================================================================== */
VDEC_ERROR VideoDecoder::Scale( VDEC_DIMENSIONS ,
                                VDEC_SCALE_CB_FN scaleCbFn,
                                void * const     pCbData )
{
  //by default scaling is not supported
  if(scaleCbFn)
  {
    (* scaleCbFn)(this,FALSE,pCbData);
  }

  return VDEC_ERR_OPERATION_UNSUPPORTED;
}

/* ======================================================================
FUNCTION
  LogQTVFrameDecodeStats

DESCRIPTION
  Logs the Frame statistics.

PARAMETERS
  Frame PTS : PTS of the video frame

RETURN VALUE
  None.

NOTES
  None.
========================================================================== */

void VideoDecoder::LogQTVFrameDecodeStats(uint32 videoFramePTS )
{
#ifndef PLATFORM_LTK

  log_vdec_VideoFrame_DecodeStats_type *pBuf = NULL;
  
  /* Check the log code status and allocate buffer for the log*/
  if(log_status( LOG_VDEC_VIDEO_FRAME_DECODE_INFO_C ))
  {
    pBuf = (log_vdec_VideoFrame_DecodeStats_type*) 
           log_alloc( LOG_VDEC_VIDEO_FRAME_DECODE_INFO_C, 
           sizeof(log_vdec_VideoFrame_DecodeStats_type) );
  }

  if ( pBuf )
  {
    /* Update the buffer statistics  */
    /* Macro block count's in base and Enhanced layer */
    pBuf->FrameType = vdec_framedecode_info.FrameType;
    pBuf->SuperFrameID = vdec_framedecode_info.SuperFrameID;
    pBuf->Layer_Decoded = vdec_framedecode_info.LayerDecoded;

    pBuf->NumIntraMBinBase = 
                    vdec_framedecode_info.intra_mb_count[VDEC_BASE_LAYER];
    pBuf->NumInterMBinBase = 
                    vdec_framedecode_info.inter_mb_count[VDEC_BASE_LAYER];
    pBuf->NumErrorMBinBase = 
                    vdec_framedecode_info.error_mb_count[VDEC_BASE_LAYER];
      
    if(MAX_MP4_LAYERS > VDEC_ENH_LAYER)
    {
      pBuf->NumIntraMBinEnh = 
                    vdec_framedecode_info.intra_mb_count[VDEC_ENH_LAYER];
      pBuf->NumInterMBinEnh = 
                    vdec_framedecode_info.inter_mb_count[VDEC_ENH_LAYER];
      pBuf->NumErrorMBinEnh = 
                    vdec_framedecode_info.error_mb_count[VDEC_ENH_LAYER];
    }

    //Video frame PTS
    pBuf->VideoFramePTS = videoFramePTS;
      
    // frame interpolated flag
    pBuf->isInterpolated = vdec_framedecode_info.isInterpolated;
      
    /* Log the buffer */
    log_commit( pBuf );
  }
#endif //!PLATFORM_LTK
}

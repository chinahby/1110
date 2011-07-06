/* =======================================================================

                          frame_buffer_pool.cpp

DESCRIPTION
  Implementation of the FrameBufferPool class, the means by which VideoDecoder
  subclasses allocate and manage frame buffers.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/core/rel/2.0/src/frame_buffer_pool.cpp#4 $
$DateTime: 2009/05/04 03:44:08 $
$Change: 902920 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/08/09    vs      Remove the dependencies of other modules.
--------   ---      ---------------------------------------------------------

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "frame_buffer_pool.h"
#include "vdecoder_log.h" 
#include "vdecoder_utils.h" // for Vdec_Malloc and Vdec_Free
#include "assert.h"    // for ASSERT

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_32BIT_ALIGNMENT
#define MASK_32BIT_BOUNDARY 0x00000003
#define IS_NOT_ALIGNED(ptr, n) \
  (((int)(ptr) & MASK_32BIT_BOUNDARY) != 0)
#else
#define MASK_25BIT_BOUNDARY 0xFE000000
#define IS_NOT_ALIGNED(ptr, n) \
  (((int)(ptr) & MASK_25BIT_BOUNDARY) != (((int)(ptr) + (n) - 1) & MASK_25BIT_BOUNDARY))
#endif

#ifdef FEATURE_QTV_DECODER_INVALIDATE_CACHE
#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE         32
#endif //JPEG_DATA_CACHE_LINES
#endif //CACHE_POLICY_WRITE_BACK

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
FUNCTION:
  FrameBufferPool::FrameBufferPool

DESCRIPTION:
  Constructor.  Allocates a buncha-buncha (muncha crunch) frame buffers.

PARAMETERS:
  VDEC_MALLOC_FN         pfnMalloc
  VDEC_FREE_FN           pfnFree
    Application-supplied functions to be used to malloc and free memory
    for frame buffers.

  const VDEC_DIMENSIONS &frameSize
    Dimensions of video frames, in pixels.  This information is needed
    to properly size frame buffers.

  const unsigned         numYUVBuffers
    The number of reference YUV buffers to allocate - these are used
    for referential decoding (P, B), so if we're also using them for output,
    take care not to change stuff in 'em!

  const unsigned       numOutputYUV_buffers
    The number of output YUV buffers: these are not used for references,
    so we can muck with them, for instance, if we're doing post-loop deblocking,
    we'd put that output in one of these so as not to pollute the reference
    frames.

  const unsigned       numRGB_buffers
    If we don't have an MDP and we need to allocate RGB output buffers,
    this is how many we'll have.  Modern platforms will set this to 0.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
===========================================================================*/
FrameBufferPool::FrameBufferPool
(
  const VDEC_STREAM_ID   stream,
  VDEC_MALLOC_FN         pfnMalloc,
  VDEC_FREE_FN           pfnFree,
  void *                 pCbData,
  const VDEC_DIMENSIONS &frameSize,
  const unsigned         numReferenceYUV_buffers,
  const unsigned         numOutputYUV_buffers,
  const unsigned         numOutputRGB_buffers
)
: m_pfnMalloc ( pfnMalloc ),
  m_pfnFree   ( pfnFree ),
  m_streamID  ( stream ),
  m_pCbData   (pCbData)
{
  memset( &m_buf, 0, sizeof( MP4BufType ) );
#ifdef FEATURE_QTV_DECODER_INVALIDATE_CACHE  
  pOutputYUVBuf_malloc = NULL;
  pYUVBuf_malloc = NULL;
  pFilteredYUVBuf_malloc = NULL;
#endif
  AllocateAllBuffers
  (
    frameSize,
    numReferenceYUV_buffers,
    numOutputYUV_buffers,
    numOutputRGB_buffers
  );
}

/* ======================================================================
FUNCTION:
  FrameBufferPool::FrameBufferPool

DESCRIPTION:
  Constructor.  Allocates a buncha-buncha (muncha crunch) frame buffers.

PARAMETERS:
  VDEC_MALLOC_FN         pfnMalloc
  VDEC_FREE_FN           pfnFree
    Application-supplied functions to be used to malloc and free memory
    for frame buffers.

  const VDEC_DIMENSIONS &referenceFrameSize
    Dimensions of reference video frames, in pixels.  This information is needed
    to properly size frame buffers.

  const unsigned         numYUVBuffers
    The number of reference YUV buffers to allocate - these are used
    for referential decoding (P, B), so if we're also using them for output,
    take care not to change stuff in 'em!

  const VDEC_DIMENSIONS &outputFrameSize
    Dimensions for the output video frames, in pixels.  This is seperate so that
    output frame dimensions aren't constrained based on the reference frame size.

  const unsigned       numOutputYUV_buffers
    The number of output YUV buffers: these are not used for references,
    so we can muck with them, for instance, if we're doing post-loop deblocking,
    we'd put that output in one of these so as not to pollute the reference
    frames.

  const VDEC_DIMENSIONS &rgbFrameSize
    Dimensions for the output RGB frames for targets without MDP.

  const unsigned       numRGB_buffers
    If we don't have an MDP and we need to allocate RGB output buffers,
    this is how many we'll have.  Modern platforms will set this to 0.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
===========================================================================*/
FrameBufferPool::FrameBufferPool
(
  const VDEC_STREAM_ID   stream,
  VDEC_MALLOC_FN         pfnMalloc,
  VDEC_FREE_FN           pfnFree,
  void *                 pCbData,
  const VDEC_DIMENSIONS &referenceFrameSize,
  const unsigned         numReferenceYUV_buffers,
  const VDEC_DIMENSIONS &outputFrameSize,
  const unsigned         numOutputYUV_buffers,
  const VDEC_DIMENSIONS &rgbFrameSize,
  const unsigned         numOutputRGB_buffers
)
: m_pfnMalloc ( pfnMalloc ),
  m_pfnFree   ( pfnFree ),
  m_streamID  ( stream ),
  m_pCbData   (pCbData)
{
  memset( &m_buf, 0, sizeof( MP4BufType ) );

  AllocateAllBuffers
  (
    referenceFrameSize,
    numReferenceYUV_buffers,
    outputFrameSize,
    numOutputYUV_buffers,
    rgbFrameSize,
    numOutputRGB_buffers
  );
}

/* ======================================================================
FUNCTION:
  FrameBufferPool::~FrameBufferPool

DESCRIPTION:
  Destructor.  Frees all buffers associated with this object.

PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
===========================================================================*/
FrameBufferPool::~FrameBufferPool( void )
{
  FreeAllBuffers();
}

/* ======================================================================
FUNCTION:
  FrameBufferPool::operator MP4BufType*

DESCRIPTION:
  Implicit conversion operator to MP4BufType*, allowing these objects
  to be used in mp4codec calls in place of the lower-level structure.

PARAMETERS:
  None.

RETURN VALUE:
  MP4BufType*

SIDE EFFECTS:
  None.
===========================================================================*/
FrameBufferPool::operator MP4BufType*( void )
{
  return &m_buf;
}

/* ======================================================================
FUNCTION:
  FrameBufferPool::AllocateAllBuffers

DESCRIPTION:
  Allocates frame buffer memory for later use by decoders.

PARAMETERS:
  const VDEC_DIMENSIONS &frameSize
    The pixel dimensions of video frames.

  const unsigned         numYUVBuffers
    The number of reference YUV buffers to allocate - these are used
    for referential decoding (P, B), so if we're also using them for output,
    take care not to change stuff in 'em!

  const unsigned       numOutputYUV_buffers
    The number of output YUV buffers: these are not used for references,
    so we can muck with them, for instance, if we're doing post-loop deblocking,
    we'd put that output in one of these so as not to pollute the reference
    frames.

  const unsigned       numRGB_buffers
    If we don't have an MDP and we need to allocate RGB output buffers,
    this is how many we'll have.  Modern platforms will set this to 0.

RETURN VALUE:
  bool
    true if we allocated OK, false if there was a problem.

SIDE EFFECTS:
  this->m_pBuf is allocated (and populated)
===========================================================================*/
bool FrameBufferPool::AllocateAllBuffers
(
  const VDEC_DIMENSIONS &frameSize,
  const unsigned         numReferenceYUV_buffers,

  // For these buffer-count arguments, we're doing a little magic to avoid
  // compiler warnings.  If the #ifdefs line up such that we won't use
  // a particular argument, we want to make sure it's unnamed in the arg
  // list to avoid warnings about unused values.  The arguments are still
  // there - lordy loo, I hate APIs that change as features are turned on
  // or off - but they're not being used sometimes.

#if defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || \
    defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1
  const unsigned         numOutputYUV_buffers,
#else
  const unsigned         /* numOutputYUV_buffers - unused */,
#endif
#if !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK)
  const unsigned         numOutputRGB_buffers /* numOutputRGB_buffers - unused */
#else
  const unsigned         /* numOutputRGB_buffers - unused */
#endif /* !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK) */
)
{
  // RGB buffers exist in MP4BufType structures only on platforms without
  // an MDP.
  //
#ifdef FEATURE_QTV_DECODER_INVALIDATE_CACHE  
  uint32 cnt;
#endif
#if !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK)
  size_t nBytesPerRgbFrame;
#endif /* !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK) */
  size_t nBytesPerYuvFrame;

  // If we're calling Allocate repeatedly, we need to avoid losing old buffers.
  //
  FreeAllBuffers();

  m_buf.Height     = frameSize.height;
  m_buf.Width      = frameSize.width;
  m_buf.RedBpp     = MP4_RED_BPP;
  m_buf.GreenBpp   = MP4_GREEN_BPP;
  m_buf.BlueBpp    = MP4_BLUE_BPP;
  m_buf.ColorOrder = RGB_LENDIAN_ORDER;

#if !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK)
  nBytesPerRgbFrame = m_buf.Height * m_buf.Width * MP4_RGB_BITS_PER_PIXEL / 8;

  // for IPL based scaling DSP does not need bigger buffer
  //
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) && !defined(FEATURE_QTV_IPL_SCALING)
  // allocate 4 times more memory to support 2X Up Scaling
  //
  nBytesPerRgbFrame *= 4;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS && !FEATURE_QTV_IPL_SCALING */
#endif /* !FEATURE_QTV_MDP */

  nBytesPerYuvFrame = m_buf.Height * m_buf.Width * MP4_YUV_BITS_PER_PIXEL / 8;

#ifdef FEATURE_QTV_DECODER_INVALIDATE_CACHE
  nBytesPerYuvFrame       = nBytesPerYuvFrame       + CACHE_LINE_SIZE;
#endif //CACHE_POLICY_WRITE_BACK

  if
  (
    AllocateBufferArray( m_buf.pYUVBuf,
                         nBytesPerYuvFrame,
                         numReferenceYUV_buffers,
                         m_buf.numYUVBuffers )
#if !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK)
    && AllocateBufferArray( m_buf.pRGBBuf,
                            nBytesPerRgbFrame,
                            numOutputRGB_buffers,
                            m_buf.numRGB_buffers )
#endif /* !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK) */
#if defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || \
    defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1
    && AllocateBufferArray( m_buf.pOutputYUVBuf,
                            nBytesPerYuvFrame,
                            numOutputYUV_buffers,
                            m_buf.numOutputYUV_buffers )
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || 
          FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1 */
#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C
    && ( m_buf.pFilteredYUVBuf = AllocateOneBuffer( nBytesPerYuvFrame ) )
       != NULL
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_C */
  )
  {
 #ifdef FEATURE_QTV_DECODER_INVALIDATE_CACHE
      pYUVBuf_malloc = ( void** )Vdec_Malloc( numReferenceYUV_buffers * sizeof( void* ) );
      pOutputYUVBuf_malloc  = ( void** )Vdec_Malloc( numOutputYUV_buffers * sizeof( void* ) );
      pFilteredYUVBuf_malloc =  m_buf.pFilteredYUVBuf;
      for(cnt = 0; cnt < numReferenceYUV_buffers; cnt++)
      {
        pYUVBuf_malloc[cnt] = m_buf.pYUVBuf[cnt];
        m_buf.pYUVBuf[cnt] = (void*)((((uint32)m_buf.pYUVBuf[cnt] + CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
      }
      for(cnt = 0; cnt < numOutputYUV_buffers; cnt++)
      {
        pOutputYUVBuf_malloc[cnt] = m_buf.pOutputYUVBuf[cnt];
        m_buf.pOutputYUVBuf[cnt] = (void*)((((uint32)m_buf.pOutputYUVBuf[cnt] + CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
      }
      m_buf.pFilteredYUVBuf = (void*)((((uint32)m_buf.pFilteredYUVBuf + CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
 #endif
    return true;
  }
  else
  {
    FreeAllBuffers();
    VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR, "Unable to allocate buffers (out of memory)" );
    return false;
  }
}

/* ======================================================================
FUNCTION:
  FrameBufferPool::AllocateAllBuffers

DESCRIPTION:
  Allocates frame buffer memory for later use by decoders.

PARAMETERS:
  const VDEC_DIMENSIONS &referenceFrameSize
    Dimensions of reference video frames, in pixels.  This information is needed
    to properly size frame buffers.

  const unsigned         numYUVBuffers
    The number of reference YUV buffers to allocate - these are used
    for referential decoding (P, B), so if we're also using them for output,
    take care not to change stuff in 'em!

  const VDEC_DIMENSIONS &outputFrameSize
    Dimensions for the output video frames, in pixels.  This is seperate so that
    output frame dimensions aren't constrained based on the reference frame size.

  const unsigned       numOutputYUV_buffers
    The number of output YUV buffers: these are not used for references,
    so we can muck with them, for instance, if we're doing post-loop deblocking,
    we'd put that output in one of these so as not to pollute the reference
    frames.

  const VDEC_DIMENSIONS &rgbFrameSize
    Dimensions for the output RGB frames for targets without MDP.

  const unsigned       numRGB_buffers
    If we don't have an MDP and we need to allocate RGB output buffers,
    this is how many we'll have.  Modern platforms will set this to 0.

RETURN VALUE:
  bool
    true if we allocated OK, false if there was a problem.

SIDE EFFECTS:
  this->m_pBuf is allocated (and populated)
===========================================================================*/
bool FrameBufferPool::AllocateAllBuffers
(
  const VDEC_DIMENSIONS &referenceFrameSize,
  const unsigned         numReferenceYUV_buffers,

  // For these buffer-count arguments, we're doing a little magic to avoid
  // compiler warnings.  If the #ifdefs line up such that we won't use
  // a particular argument, we want to make sure it's unnamed in the arg
  // list to avoid warnings about unused values.  The arguments are still
  // there - lordy loo, I hate APIs that change as features are turned on
  // or off - but they're not being used sometimes.

#if defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || \
    defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1
  const VDEC_DIMENSIONS &outputFrameSize,
  const unsigned         numOutputYUV_buffers,
#else
  const VDEC_DIMENSIONS &/* outputFrameSize - unused */,
  const unsigned         /* numOutputYUV_buffers - unused */,
#endif
#ifdef FEATURE_QTV_MDP
  const VDEC_DIMENSIONS &/* rgbFrameSize - unused */,
  const unsigned         /* numOutputRGB_buffers - unused */
#else
  const VDEC_DIMENSIONS &rgbFrameSize,
  const unsigned         numOutputRGB_buffers
#endif
)
{
  // RGB buffers exist in MP4BufType structures only on platforms without
  // an MDP.
  //
#ifndef FEATURE_QTV_MDP
  size_t nBytesPerRgbFrame;
#endif /* FEATURE_QTV_MDP */
  size_t nBytesPerYuvFrame;
  size_t nBytesPerOutputYuvFrame;
#ifdef FEATURE_QTV_DECODER_INVALIDATE_CACHE
  uint32 cnt;
#endif
  // If we're calling Allocate repeatedly, we need to avoid losing old buffers.
  //
  FreeAllBuffers();

  m_buf.Height       = referenceFrameSize.height;
  m_buf.Width        = referenceFrameSize.width;
  m_buf.OutputHeight = outputFrameSize.height;
  m_buf.OutputWidth  = outputFrameSize.width;
  m_buf.RedBpp       = MP4_RED_BPP;
  m_buf.GreenBpp     = MP4_GREEN_BPP;
  m_buf.BlueBpp      = MP4_BLUE_BPP;
  m_buf.ColorOrder   = RGB_LENDIAN_ORDER;

#ifndef FEATURE_QTV_MDP
  nBytesPerRgbFrame = m_buf.Height * m_buf.Width * MP4_RGB_BITS_PER_PIXEL / 8;

  // for IPL based scaling DSP does not need bigger buffer
  //
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) && !defined(FEATURE_QTV_IPL_SCALING)
  // allocate 4 times more memory to support 2X Up Scaling
  //
  nBytesPerRgbFrame *= 4;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS && !FEATURE_QTV_IPL_SCALING */
#endif /* !FEATURE_QTV_MDP */

  nBytesPerYuvFrame = m_buf.Height * m_buf.Width * MP4_YUV_BITS_PER_PIXEL / 8;
  nBytesPerOutputYuvFrame = outputFrameSize.height * outputFrameSize.width *
                            MP4_YUV_BITS_PER_PIXEL / 8;

#ifdef FEATURE_QTV_DECODER_INVALIDATE_CACHE
  nBytesPerYuvFrame       = nBytesPerYuvFrame       + CACHE_LINE_SIZE;
  nBytesPerOutputYuvFrame = nBytesPerOutputYuvFrame + CACHE_LINE_SIZE;
#endif //FEATURE_QTV_DECODER_INVALIDATE_CACHE

  if
  (
    AllocateBufferArray( m_buf.pYUVBuf,
                            nBytesPerYuvFrame,
                         numReferenceYUV_buffers,
                         m_buf.numYUVBuffers )
#ifndef FEATURE_QTV_MDP
    && AllocateBufferArray( m_buf.pRGBBuf,
                            nBytesPerRgbFrame,
                            numOutputRGB_buffers,
                            m_buf.numRGB_buffers )
#endif /* !FEATURE_QTV_MDP */
#if defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || \
    defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1
    && AllocateBufferArray( m_buf.pOutputYUVBuf,
                            nBytesPerOutputYuvFrame,
                            numOutputYUV_buffers,
                            m_buf.numOutputYUV_buffers )
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || 
          FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1 */
#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C
    && ( m_buf.pFilteredYUVBuf = AllocateOneBuffer( nBytesPerYuvFrame ) )
       != NULL
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_C */
  )
  {
 #ifdef FEATURE_QTV_DECODER_INVALIDATE_CACHE
    pYUVBuf_malloc = ( void** )Vdec_Malloc( numReferenceYUV_buffers * sizeof( void* ) );
    pOutputYUVBuf_malloc  = ( void** )Vdec_Malloc( numOutputYUV_buffers * sizeof( void* ) );
    pFilteredYUVBuf_malloc =  m_buf.pFilteredYUVBuf;
    for(cnt = 0; cnt < numReferenceYUV_buffers; cnt++)
    {
      pYUVBuf_malloc[cnt] = m_buf.pYUVBuf[cnt];
      m_buf.pYUVBuf[cnt] = (void*)((((uint32)m_buf.pYUVBuf[cnt] + CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
    }
    for(cnt = 0; cnt < numOutputYUV_buffers; cnt++)
    {
      pOutputYUVBuf_malloc[cnt] = m_buf.pOutputYUVBuf[cnt];
      m_buf.pOutputYUVBuf[cnt] = (void*)((((uint32)m_buf.pOutputYUVBuf[cnt] + CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
    }
    m_buf.pFilteredYUVBuf = (void*)((((uint32)m_buf.pFilteredYUVBuf + CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
 #endif//FEATURE_QTV_DECODER_INVALIDATE_CACHE

    return true;
  }
  else
  {
    FreeAllBuffers();
    VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR, "Unable to allocate buffers (out of memory)" );
    return false;
  }
}

/* ======================================================================
FUNCTION:
  FrameBufferPool::AllocateBufferArray

DESCRIPTION:
  Populates an array of buffer pointers.

PARAMETERS:
  void** &array_out
    Pointer to the buffer array allocated.

  const size_t   nBytesPerBuffer
    Size of each individual buffer.

  const unsigned numBuffers
    The desired array size; we'll try to allocate this many buffers.

  uint16&  numBuffers_out
    A reference to a uint16 in which we'll put the number of buffers
    we successfully allocated.

RETURN VALUE:
  bool
    true if all desired buffers were obtained, false if we couldn't
    get as many as we wanted.

SIDE EFFECTS:
  None.
===========================================================================*/
bool FrameBufferPool::AllocateBufferArray
(
  void**         &array_out,
  const size_t    nBytesPerBuffer,
  const unsigned  numBuffers,
  uint16         &numBuffers_out
)
{

  if( 0 == numBuffers )
  {
    return true;
  }

  numBuffers_out = 0;

  if(!numBuffers)
  {
    //nothing to allocate
    array_out = NULL;
    return true;
  }

  if( array_out != NULL )
  {
    Vdec_Free( array_out );
    array_out = NULL;
  }

  array_out = ( void** )Vdec_Malloc( numBuffers * sizeof( void* ) );
  {
    if ( array_out != NULL )
    {
      memset( array_out, 0, numBuffers * sizeof( void* ) );
    }
    else
    {
      return false;
    }
  }

  for ( int i = 0; i < numBuffers; ++i )
  {
    array_out[ i ] = AllocateOneBuffer( nBytesPerBuffer );
    if ( array_out[ i ] != NULL )
    {
      ++numBuffers_out;
    }
    else
    {
      return false; // assuming that future attempts will fail too.
    }
  }

  return true;
}

/* ======================================================================
FUNCTION:
  FrameBufferPool::AllocateOneBuffer

DESCRIPTION:
  Allocate a single frame buffer.

PARAMETERS:
  const size_t nBytes
    The size of the buffer to be allocated.

RETURN VALUE:
  void*
    The buffer pointer.

SIDE EFFECTS:
  None.
===========================================================================*/
void* FrameBufferPool::AllocateOneBuffer( const size_t nBytes )
{
  void *pBuf = m_pfnMalloc( m_streamID,m_pCbData,nBytes );
#ifndef FEATURE_QDSP_RTOS
  void *pTempBuf = NULL;
  if ( IS_NOT_ALIGNED( pBuf, nBytes ) )
  {
    /* Buffer allocated was not bit aligned,try once again */
    pTempBuf = pBuf;
    pBuf = m_pfnMalloc( m_streamID,m_pCbData,nBytes );
    if ( IS_NOT_ALIGNED( pBuf, nBytes ) )
    {
      /* Buffer allocated was not bit aligned again, give up.
         Free this buffer and return NULL, the heap manager should be
         aware of this limitation and not return us a buffer of this kind. */
      VDEC_MSG_PRIO1(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR, 
       "Allocated buffer crosses 25bit boundary(0x%x), Change to heap alloc needed", pBuf);
      m_pfnFree( m_streamID, m_pCbData, pBuf );
      m_pfnFree( m_streamID, m_pCbData, pTempBuf );
      pBuf = NULL;
      ASSERT(0);
   }
   else
   {
      /* Free the unaligend buffer which we got first time */
      m_pfnFree( m_streamID, m_pCbData, pTempBuf );
   }        
  } 
#endif
  // Memset Data to NULL, So that no JUNK data is present.
  if(pBuf){
    memset(pBuf, 0, nBytes);
  }
  return pBuf;
}

/* ======================================================================
FUNCTION:
  FrameBufferPool::FreeAllBuffers

DESCRIPTION:
  Frees all the memory tied up in our MP4BufType structure.

PARAMETERS:
  None.  We already know where m_buf lives.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
===========================================================================*/
void FrameBufferPool::FreeAllBuffers( void )
{
#ifdef FEATURE_QTV_DECODER_INVALIDATE_CACHE
  FreeBufferArray( pYUVBuf_malloc, m_buf.numYUVBuffers );
  Vdec_Free(m_buf.pYUVBuf);
  m_buf.pYUVBuf = NULL;
  #if defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || \
    defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1
  FreeBufferArray( pOutputYUVBuf_malloc, m_buf.numOutputYUV_buffers );
  Vdec_Free(m_buf.pOutputYUVBuf);
  m_buf.pOutputYUVBuf = NULL;
  #endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || 
          FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1*/
  #ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C
  m_pfnFree( m_streamID,m_pCbData,pFilteredYUVBuf_malloc );
  m_buf.pFilteredYUVBuf = NULL;
  #endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_C */
#else //FEATURE_QTV_DECODER_INVALIDATE_CACHE
  FreeBufferArray( m_buf.pYUVBuf, m_buf.numYUVBuffers );
#if defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || \
    defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1
  FreeBufferArray( m_buf.pOutputYUVBuf, m_buf.numOutputYUV_buffers );
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || 
          FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1*/
#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C
  m_pfnFree( m_streamID,m_pCbData,m_buf.pFilteredYUVBuf );
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_C */
#endif //FEATURE_QTV_DECODER_INVALIDATE_CACHE
  
#if !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK)
  FreeBufferArray( m_buf.pRGBBuf, m_buf.numRGB_buffers );
#endif /* !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK) */

  // Zero the whole thing out - ain't nobody here nohow no more.
  //
  memset( &m_buf, 0, sizeof( MP4BufType ) );
}

/* ======================================================================
FUNCTION:
  FrameBufferPool::FreeBufferArray

DESCRIPTION:
  Frees all the buffers stored in a provided array.

PARAMETERS:
  void * &array
    The array to clear - the array itself must be freed too.

  const unsigned numBuffers
    Number of elements to zap.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
===========================================================================*/
void FrameBufferPool::FreeBufferArray
(
  void**         &array,
  const unsigned numBuffers
)
{
  if ( array != NULL )
  {
    for ( int i = 0; i < numBuffers; ++i )
    {
      m_pfnFree( m_streamID, m_pCbData, array[ i ] );
    }

    Vdec_Free( array );
    array = NULL;
  }
}

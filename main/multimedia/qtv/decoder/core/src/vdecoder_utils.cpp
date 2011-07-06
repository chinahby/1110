/* =======================================================================

                          vdecoder_utils.cpp

DESCRIPTION
  Definitions of 'utility' functions used on both sides of the VDEC API.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/core/rel/2.0/src/vdecoder_utils.cpp#11 $
$DateTime: 2010/07/06 22:18:35 $
$Change: 1358007 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/09/09    vs      Remove the dependencies of other modules.
--------   ---      ---------------------------------------------------------
========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include <comdef.h>  // common type definitions
#include <stdlib.h>
#include <rex.h>
#include <task.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "vdecoder_utils.h"
extern "C"
{
#include "TL_types.h"
}
#include "MP4_Types.h"

#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif
#if( defined(FEATURE_QTV_MEMORY_LEAK_DEBUG) || defined (FEATURE_QTV_MEMORY_DEBUG) )
# include "vdecoder_log.h"
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_BREW_3_0
# include "AEE_OEMHeap.h"
# include "OEMHeap.h"
#else
# include "AEE_OEM.h"
#endif /* FEATURE_BREW_3_0 */

#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define LOG_MPEG4_LOG_ALLOC_WAIT_MS 20
#define LOG_MPEG4_LOG_ALLOC_MAX_WAIT_MS 400

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
struct VDEC2DSP_AUDIO_EQUIV
{
  QtvQDSPAudioType  dsp;
  VDEC_CONCURRENCY_CONFIG vdec;
};

struct VDEC2VDL_CONCURRENCY_EQUIV
{
  VDL_Concurrency_Type  dlAudio;
  VDEC_CONCURRENCY_CONFIG vdec;
};

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
const VDEC2DSP_AUDIO_EQUIV s_dspAudioMap[] =
{
  { QTVQDSP_AUDIO_EVRC8K,      VDEC_CONCURRENT_AUDIO_EVRC },
  { QTVQDSP_AUDIO_QCELP13K,    VDEC_CONCURRENT_AUDIO_QCELP },
  { QTVQDSP_AUDIO_AMR,         VDEC_CONCURRENT_AUDIO_AMR },
  { QTVQDSP_AUDIO_AAC,         VDEC_CONCURRENT_AUDIO_AAC },
  { QTVQDSP_AUDIO_MP3,         VDEC_CONCURRENT_AUDIO_MP3 },
  { QTVQDSP_AUDIO_CONC,        VDEC_CONCURRENT_AUDIO_CONC },
  { QTVQDSP_AUDIO_REAL,        VDEC_CONCURRENT_AUDIO_REAL },
  { QTVQDSP_AUDIO_AMR_WB,      VDEC_CONCURRENT_AUDIO_AMR_WB },
  { QTVQDSP_AUDIO_AMR_WB_PLUS, VDEC_CONCURRENT_AUDIO_AMR_WB_PLUS },
  { QTVQDSP_VIDEO_MPEG4_ENCODER,   VDEC_CONCURRENT_VIDEO_MPEG4_ENCODER },
  { QTVQDSP_VIDEO_H264_ENCODER,    VDEC_CONCURRENT_VIDEO_H264_ENCODER },
  { QTVQDSP_AUD_OVER_ENC_QCELP13K, VDEC_CONCURRENT_AUD_OVER_ENC_QCELP13K },
  { QTVQDSP_AUD_OVER_ENC_AMR,      VDEC_CONCURRENT_AUD_OVER_ENC_AMR },
  { QTVQDSP_AUD_OVER_ENC_EVRC,     VDEC_CONCURRENT_AUD_OVER_ENC_EVRC },
  { QTVQDSP_QVP_OVER_ENC_EVRC,     VDEC_CONCURRENT_QVP_OVER_ENC_EVRC },
  { QTVQDSP_QVP_OVER_ENC_EVRC05,   VDEC_CONCURRENT_QVP_OVER_ENC_EVRC05},  
  { QTVQDSP_QVP_OVER_ENC_EVRC1P,   VDEC_CONCURRENT_QVP_OVER_ENC_EVRC1P },
  { QTVQDSP_QVP_OVER_ENC_AMR,      VDEC_CONCURRENT_QVP_OVER_ENC_AMR },
  { QTVQDSP_QVP_OVER_ENC_AMR05,    VDEC_CONCURRENT_QVP_OVER_ENC_AMR05 },  
  { QTVQDSP_QVP_OVER_ENC_AMR1P,    VDEC_CONCURRENT_QVP_OVER_ENC_AMR1P },
  { QTVQDSP_QVP_OVER_ENC_4GV,      VDEC_CONCURRENT_QVP_OVER_ENC_4GV },
  { QTVQDSP_QVP_OVER_ENC_4GV1P,    VDEC_CONCURRENT_QVP_OVER_ENC_4GV1P },
  { QTVQDSP_QVP_OVER_ENC_4GVWB,      VDEC_CONCURRENT_QVP_OVER_ENC_4GVWB },
  { QTVQDSP_QVP_OVER_ENC_4GVWB1P,    VDEC_CONCURRENT_QVP_OVER_ENC_4GVWB1P },
  { QTVQDSP_AUDIO_NONE,            VDEC_CONCURRENT_NONE }
};

/* ----------------------------------------------------------------------- */

const VDEC2VDL_CONCURRENCY_EQUIV s_dlAudioMap[] =
{
  { VDL_AUDIO_EVRC8K,          VDEC_CONCURRENT_AUDIO_EVRC },
  { VDL_AUDIO_QCELP13K,        VDEC_CONCURRENT_AUDIO_QCELP },
  { VDL_AUDIO_AMR,             VDEC_CONCURRENT_AUDIO_AMR },
  { VDL_AUDIO_AAC,             VDEC_CONCURRENT_AUDIO_AAC },
  { VDL_AUDIO_BSAC,            VDEC_CONCURRENT_AUDIO_BSAC },
  { VDL_AUDIO_MP3,             VDEC_CONCURRENT_AUDIO_MP3 },
  { VDL_AUDIO_CONC,            VDEC_CONCURRENT_AUDIO_CONC },
  { VDL_AUDIO_REAL,            VDEC_CONCURRENT_AUDIO_REAL },
  { VDL_AUDIO_AMR_WB,          VDEC_CONCURRENT_AUDIO_AMR_WB },
  { VDL_AUDIO_AMR_WB_PLUS,     VDEC_CONCURRENT_AUDIO_AMR_WB_PLUS },
  { VDL_VIDEO_MPEG4_ENCODER,   VDEC_CONCURRENT_VIDEO_MPEG4_ENCODER },
  { VDL_VIDEO_H264_ENCODER,    VDEC_CONCURRENT_VIDEO_H264_ENCODER },
  { VDL_AUD_OVER_ENC_QCELP13K, VDEC_CONCURRENT_AUD_OVER_ENC_QCELP13K },
  { VDL_AUD_OVER_ENC_AMR,      VDEC_CONCURRENT_AUD_OVER_ENC_AMR },
  { VDL_AUD_OVER_ENC_EVRC,     VDEC_CONCURRENT_AUD_OVER_ENC_EVRC },
  { VDL_QVP_OVER_ENC_EVRC,          VDEC_CONCURRENT_QVP_OVER_ENC_EVRC },
  { VDL_QVP_OVER_ENC_EVRC05,        VDEC_CONCURRENT_QVP_OVER_ENC_EVRC05},  
  { VDL_QVP_OVER_ENC_EVRC1P,        VDEC_CONCURRENT_QVP_OVER_ENC_EVRC1P },
  { VDL_QVP_OVER_ENC_AMR,           VDEC_CONCURRENT_QVP_OVER_ENC_AMR },
  { VDL_QVP_OVER_ENC_AMR05,         VDEC_CONCURRENT_QVP_OVER_ENC_AMR05},  
  { VDL_QVP_OVER_ENC_AMR1P,         VDEC_CONCURRENT_QVP_OVER_ENC_AMR1P },
  { VDL_QVP_OVER_ENC_4GV,           VDEC_CONCURRENT_QVP_OVER_ENC_4GV },
  { VDL_QVP_OVER_ENC_4GV1P,         VDEC_CONCURRENT_QVP_OVER_ENC_4GV1P },
  { VDL_QVP_OVER_ENC_4GVWB,           VDEC_CONCURRENT_QVP_OVER_ENC_4GVWB },
  { VDL_QVP_OVER_ENC_4GVWB1P,         VDEC_CONCURRENT_QVP_OVER_ENC_4GVWB1P },
  { VDL_AUDIO_NONE,            VDEC_CONCURRENT_NONE }
};
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

/* Keep track of the total bytes allocated by Qtv.  This does not take into
 * account qtv_realloc calls unless FEATURE_QTV_MEMORY_LEAK_DEBUG is defined.
 */
static uint32        vdec_malloc_total_bytes = 0; 
int32 vdec_dummy_var;

#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif

/* ----------------------------------------------------------------------- */

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

DESCRIPTION:

PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
===========================================================================*/

/* ======================================================================
FUNCTION:
  ConvertDSPAudioToVdec

DESCRIPTION:
  Converts a QTVQDSP_AUDIO_ audio codec #define (from mp4qdsp_drv.h) into a
  VDEC_AUDIO_CONFIG (from vdecoder_types.h)

PARAMETERS:
  const int dspAudio
    The QTVQDSP_AUDIO_ #define.

RETURN VALUE:
  VDEC_CONCURRENCY_CONFIG
    The corresponding vdec type.

SIDE EFFECTS:
  None.
===========================================================================*/
VDEC_CONCURRENCY_CONFIG ConvertDSPAudioToVdec( const int dspAudio )
{
  VDEC_CONCURRENCY_CONFIG vdecAudio = VDEC_CONCURRENT_NONE;

  for ( int i = 0; i < ARR_SIZE(s_dspAudioMap); ++i )
  {
    if ( s_dspAudioMap[ i ].dsp == (QtvQDSPAudioType) dspAudio )
    {
      vdecAudio = s_dspAudioMap[ i ].vdec;
      break;
    }
  }

  return vdecAudio;
}

/* ======================================================================
FUNCTION:
  ConvertVdecAudioToDSP

DESCRIPTION:
  Converts a VDEC_AUDIO_CONFIG (from vdecoder_types.h) audio codec
  into a QTVQDSP_AUDIO_ #define (from mp4qdsp_drv.h)

PARAMETERS:
  const VDEC_CONCURRENCY_CONFIG vdecAudio
    The vdec audio type.

RETURN VALUE:
  int
    The QTVQDSP_AUDIO_ #define corresponding to the vdec type.

SIDE EFFECTS:
  none.
===========================================================================*/
int ConvertVdecAudioToDSP( const VDEC_CONCURRENCY_CONFIG vdecAudio )
{
  int dspAudio = (int) QTVQDSP_AUDIO_NONE;

  for ( int i = 0; i < ARR_SIZE(s_dspAudioMap); ++i )
  {
    if ( s_dspAudioMap[ i ].vdec == vdecAudio )
    {
      dspAudio = (int) s_dspAudioMap[ i ].dsp;
      break;
    }
  }

  return dspAudio;
}

/* ======================================================================
FUNCTION:
  ConvertVDLConcurrencyTypeToDSP

DESCRIPTION:
  Converts a VDEC_AUDIO_CONFIG (from vdecoder_types.h) audio codec
  into a VDL_AUDIO_TYPE #define (from VDL_types.h)

PARAMETERS:
  const VDEC_CONCURRENCY_CONFIG vdecAudio
    The vdec audio type.

RETURN VALUE:
  int
    The VDEC2VDL_CONCURRENCY_EQUIV #define corresponding to the vdec type.

SIDE EFFECTS:
  none.
===========================================================================*/
int ConvertVDLConcurrencyTypeToDSP( const VDEC_CONCURRENCY_CONFIG vdecAudio,VDEC_AUDIO_VTYPE audio_vtype)
{
  int dlAudio = (int) VDL_AUDIO_NONE;

  for ( int i = 0; i < ARR_SIZE(s_dlAudioMap); ++i )
  {
    if ( s_dlAudioMap[ i ].vdec == vdecAudio )
    {
      dlAudio = (int) s_dlAudioMap[ i ].dlAudio;
      break;
    }
  }

  if((dlAudio == (int)VDL_AUDIO_CONC) && (audio_vtype != VTYPE_NONE) )
  {
    switch(audio_vtype)
    {
      case VTYPE_AMR:
        dlAudio = (int)VDL_AUDIO_CONC_AMR;
      break;
      case VTYPE_13K:
        dlAudio = (int)VDL_AUDIO_CONC_13K;
      break;
      case VTYPE_EVRC:
        dlAudio = (int)VDL_AUDIO_CONC_EVRC;
      break;
      case VTYPE_4GV:
        dlAudio = (int)VDL_AUDIO_CONC_4GV;
      break;
      default:
        dlAudio = (int)VDL_AUDIO_CONC;
      break;
    }
  }
  return dlAudio;
}

/* ======================================================================
FUNCTION:
  ConvertStatsToVdecFrame

DESCRIPTION:
  Converts a DecodeStatsType (from mp4codec.h) into a VDEC_FRAME
  (from vdecoder_types.h)

PARAMETERS:
   const DecodeStatsType &stats_in
     The stats structure to convert
   VDEC_FRAME               &frame_out
     The VDEC_FRAME to populate!

RETURN VALUE:
  none.

SIDE EFFECTS:
  none.
===========================================================================*/
void ConvertStatsToVdecFrame
(
  const DecodeStatsType &stats_in,
  VDEC_FRAME               &frame_out
)
{
  // Copy data from the stats buffer to a shiny VDEC_FRAME.
  //
  // Easy stuff first
  //
  frame_out.stream       = 0; // Caller has to set this one...

#ifdef FEATURE_QTV_DECODER_XSCALE_VIDEO
  if(stats_in.ScaledWidth && stats_in.ScaledHeight)
  {
    //Use the scaled decoded frame dimensions
    frame_out.dimensions.width    = stats_in.ScaledWidth;
    frame_out.dimensions.height   = stats_in.ScaledHeight;

    //Use the requested scaling ratio. mp4stats_in.Width/Height will
    //always store the requested scaled width/scaled height irrespective
    // of whether the original clip dimension is mod/non-mod 16. Essentially it
    // becomes our cropping window for the scaled frame
    frame_out.cwin.x2   = stats_in.ScaledWidth - 1;
    frame_out.cwin.y2   = stats_in.ScaledHeight - 1;
  }
  else
#endif //FEATURE_QTV_DECODER_XSCALE_VIDEO
  {
    //Use the decoded frame dimensions
    frame_out.dimensions.width = stats_in.Width;
    frame_out.dimensions.height   = stats_in.Height;

    frame_out.cwin.x2 = stats_in.SrcWidth - 1;
    frame_out.cwin.y2 = stats_in.SrcHeight - 1;
  }

  //Presently setting top-left coordinate to 0,0 until all
  //  codecs set it as per the standard.
  frame_out.cwin.x1   = 0;
  frame_out.cwin.y1   = 0;

  frame_out.timestamp     = (uint64)((uint64)stats_in.TimeStampValidityField <<32 | stats_in.FrameDisplayTime);;
  frame_out.layer         = stats_in.LayerID;
  frame_out.extFrame.numConcealedMbs = stats_in.NumConcealedMB;
  frame_out.extFrame.timeIncrement = stats_in.TimeIncrement;
  frame_out.extFrame.derivedTimestamp = stats_in.derivedTimestamp;
  frame_out.extFrame.numIntraMbs = stats_in.NumIntraMBs;
  frame_out.extFrame.streamStatus = (VDEC_STREAM_STATUS)stats_in.StreamStatus;
  frame_out.pMetaData = stats_in.pMetaData;
#ifdef FEATURE_MPEG4_DEBLOCKER
  //frame_out.extFrame.pPostFilterMbInfo = (VDEC_POST_FILTER_MB_INFO *) stats_in.pMbInfo;
#endif // FEATURE_MPEG4_DEBLOCKER
  frame_out.extFrame.pPostFilterMbInfo = NULL;

  /* reserved for adaptive backlight feature */
  frame_out.extFrame.pPostFilterFrameInfo = NULL;

  frame_out.frameType     = (VDEC_FRAMETYPE)stats_in.FrameType;

  // Which buffer we return and the color format depend on hardware...
  // If we're using the MDP, we need a YUV buffer.  If we're not
  // we want the RGB buffers.
  //
#if defined (FEATURE_QTV_MDP) && !defined ( PLATFORM_LTK )
#if defined( FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A )
#error code not present
#elif defined( FEATURE_VIDEO_PLAYER_INTERFACE_REV_C )
#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1
    frame_out.pBuf            = ( VDEC_BYTE * )stats_in.pOutputYUVBuf;
#else
  frame_out.pBuf            = ( VDEC_BYTE * )stats_in.pYUVBuf;
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1 */
#else
  #error DSP hardware rev not supported
#endif
  frame_out.format.fourcc    = VDEC_FOURCC_I420   /*VDEC_COLOR_YUV_12BPP*/;
  frame_out.format.bbp       = 12;
#else // FEATURE_QTV_MDP
  frame_out.pBuf            = ( VDEC_BYTE * )stats_in.pRGBBuf;
  frame_out.format.fourcc   = VDEC_FOURCC_BI_RGB  /* VDEC_COLOR_RGB_16BPP*/;
  frame_out.format.bbp      = 16;
#endif // FEATURE_QTV_MDP
  frame_out.frameStatus = 0;
}

#ifndef PLATFORM_LTK
/*===========================================================================
Function: wait_for_bitstream_log_buffer

Description: Obtain a log buffer.  If log_alloc() is out of memory wait until
             the memory is available.  Return NULL if logging is disabled.

Return Value:
   NULL or a pointer to a log buffer.

Side Effects:

============================================================================*/
log_vdec_video_bitstream_type* wait_for_bitstream_log_buffer( void )
{
  log_vdec_video_bitstream_type *pBitstreamBuf = NULL;
  uint32 TotalWaitTime = 0;

  /* Because log_alloc() will return NULL when it is out of memory we
   * need to keep looping until we get memory.  We'll monitor
   * log_status() just in case the log gets disabled.
   */
  while ( !pBitstreamBuf && log_status( LOG_VDEC_VIDEO_BITSTREAM ) )
  {
    pBitstreamBuf = (log_vdec_video_bitstream_type*) log_alloc( LOG_VDEC_VIDEO_BITSTREAM, sizeof(*pBitstreamBuf) );
    if(!pBitstreamBuf)
    {
      if (TotalWaitTime < LOG_MPEG4_LOG_ALLOC_MAX_WAIT_MS)
      {
        rex_sleep(LOG_MPEG4_LOG_ALLOC_WAIT_MS);
        TotalWaitTime += LOG_MPEG4_LOG_ALLOC_WAIT_MS;
      }
      else
      {
        VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR, 
                      "Couldn't get memory to log bitstream" );
        break;
      }
    }
  }

  return pBitstreamBuf;
}
#endif /* PLATFORM_LTK */

/*===========================================================================
Function: log_bitstream_buffer

Description: Log bitstream on a frame basis by breaking it up into chuncks acceptable
             to the logging subsystem.  Won't return until the entire buffer has
             been logged or the logging is turned off by the DM.

Return Value:
   None.

Side Effects:
============================================================================*/
void log_bitstream_buffer( uint8 *pBitstream, uint32 numBytes )
{
#ifndef PLATFORM_LTK
  uint16 LogChunkCount = 0;
  uint16 LogSize = (uint32) MIN( numBytes, LOG_BITSTREAM_MAX_SIZE );

  log_vdec_video_bitstream_type *pLogBitstream = wait_for_bitstream_log_buffer();

  if (pLogBitstream)
  {
    while (pLogBitstream && pBitstream)
    {
      ASSERT(pLogBitstream);
      ASSERT(pBitstream);

      pLogBitstream->ChunkNum = LogChunkCount++;
      pLogBitstream->NBytes = LogSize;
      memcpy( (void *) pLogBitstream->Data, pBitstream, LogSize );
      log_commit( pLogBitstream );

      numBytes -= LogSize;
      pBitstream += LogSize;

      if (numBytes > 0)
      {
        LogSize = (uint32) MIN( numBytes, LOG_BITSTREAM_MAX_SIZE );

        pLogBitstream = wait_for_bitstream_log_buffer();
      }
      else
      {
        pLogBitstream = NULL;
      }
    } /* while there is more bitstream to log */
  }
#else
#error code not present
#endif /* PLATFORM_LTK */
}

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

/* ======================================================================
FUNCTION 
  vdec_malloc

DESCRIPTION
  Allocate memory from the heap.

  This function should not be called directly from vdecoder code.  Instead, call
  Vdec_Malloc() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  none

RETURN VALUE
  Returns a pointer to numBytes of memory or NULL if unable to allocate memory.

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void* vdec_malloc(uint32 numBytes, const char *file, uint16 line)
{
  void *pMemory;
#if( defined(FEATURE_QTV_MEMORY_DEBUG) || defined(FEATURE_QTV_MEMORY_LEAK_DEBUG) )
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL)
  {
    pFile = strrchr(file, '/'); 
  } /* in case of linux, use forward slash */
  if(pFile == NULL) 
  { // if still null, print error msg, page fault about to ensue
    pFile = file; 
  }
  else
  {
    pFile++; 
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG || FEATURE_QTV_MEMORY_LEAK_DEBUG */
  
#ifdef T_WINNT
#error code not present
#endif

  /* Allocate the memory from the heap */
  pMemory = malloc(numBytes);

#ifdef T_WINNT
#error code not present
#endif

  if( pMemory != NULL )
  {
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_DEBUG
    VDEC_MSG_SPRINTF_4( VDECDIAG_DEBUG, "[%s:%d] qtv_malloc(%d), %x",
                       pFile, line, numBytes, pMemory );
#endif /* FEATURE_QTV_MEMORY_DEBUG */
    
    /* Increment the number of bytes allocated by Qtv */
    vdec_malloc_total_bytes += numBytes;
  }
  
  return(pMemory);
}


#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif 


/* ======================================================================
FUNCTION 
  vdec_realloc

DESCRIPTION
  Reallocate memory of a different size from the heap.  If possible, the memory
  will be in the same location.  Otherwise the old memory will be free'd and a
  new chunk allocated.

  This function should not be called directly from Qtv code.  Instead, call
  VDEC_Realloc() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  vdec_realloc will not update any of the byte counters (total, current, or max)
  unless FEATURE_QTV_MEMORY_LEAK_DEBUG is defined.
  
RETURN VALUE
  Returns a pointer to numBytes of memory or NULL if unable to allocate memory.

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void* vdec_realloc(void *pOldMemory, uint32 numBytes, const char *file, uint16 line)
{
  void *pMemory;
#if( defined(FEATURE_QTV_MEMORY_DEBUG) || defined(FEATURE_QTV_MEMORY_LEAK_DEBUG) )
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL) 
  {
    pFile = strrchr(file, '/'); 
  } /* in case of linux, use forward slash */
  if(pFile == NULL) 
  { // if still null, print error msg, page fault about to ensue
    pFile = file;
  }
  else 
  {
    pFile++; 
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG || FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef T_WINNT
#error code not present
#endif

  /* Reallocate the memory */
  pMemory = realloc( pOldMemory, numBytes );
  
#ifdef T_WINNT
#error code not present
#endif

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_DEBUG
  VDEC_MSG_SPRINTF_5( VDECDIAG_DEBUG, "[%s:%d], vdec_realloc(%x,%d), %x",
                     pFile, line, pMemory, numBytes, pMemory ); /*lint!e449 */
#endif /* FEATURE_QTV_MEMORY_DEBUG */

  return(pMemory);
}

/* ======================================================================
FUNCTION 
  vdec_free

DESCRIPTION
  Free memory back to the heap to be reused later.

  This function should not be called directly from Qtv code.  Instead, call
  VDEC_Free() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to VDEC_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void vdec_free(void* pMemory, const char* file, uint16 line)
{
#ifdef FEATURE_QTV_MEMORY_DEBUG
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL) 
  {
    pFile = strrchr(file, '/'); 
  } /* in case of linux, use forward slash */
  if(pFile == NULL) 
  { // if still null, print error msg, page fault about to ensue
    pFile = file;
  }
  else 
  {
    pFile++; 
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG */

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef T_WINNT
#error code not present
#endif

  /* Free the memory */
  free(pMemory);

#ifdef T_WINNT
#error code not present
#endif
  VDEC_USE_ARG1(line);

}

/* ======================================================================
FUNCTION 
  vdec_new

DESCRIPTION
  Allocate memory from the heap.  This function replaces the standard C++ 'new'
  operator in order to track memory usage by Qtv.

  This function should not be called directly from Qtv code.  Instead, call
  VDEC_New() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to Vdec_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void *vdec_new(void* pMemory, uint32 numBytes, const char *file, uint16 line)
{
#if( defined(FEATURE_QTV_MEMORY_DEBUG) || defined(FEATURE_QTV_MEMORY_LEAK_DEBUG) )
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL) 
  {
    pFile = strrchr(file, '/'); 
  } /* in case of linux, use forward slash */
  if(pFile == NULL)
  { // if still null, print error msg, page fault about to ensue
    pFile = file;
  }
  else 
  {
    pFile++; 
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG || FEATURE_QTV_MEMORY_LEAK_DEBUG */

  if( pMemory != NULL )
  {
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_DEBUG
    VDEC_MSG_SPRINTF_4( VDECDIAG_DEBUG, "[%s:%d] vdec_new(%d), %x",
                       pFile, line, numBytes, pMemory );
#endif /* FEATURE_QTV_MEMORY_DEBUG */

    /* Increment the number of bytes allocated by Qtv */
    vdec_malloc_total_bytes += numBytes;
  }
    
  return(pMemory);
}

/* ======================================================================
FUNCTION 
  vdec_delete

DESCRIPTION
  Free memory back to the heap to be reused later.  This function assumes that
  the memory has already been deleted.  This function just keeps track of
  information for debugging memory errors.

  This function should not be called directly from Qtv code.  Instead, call
  VDEC_Delete() to be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to Vdec_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void vdec_delete(void* pMemory, const char *file, uint16 line)
{
#ifdef FEATURE_QTV_MEMORY_DEBUG
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL)
  {
    pFile = strrchr(file, '/');
  } /* in case of linux, use forward slash */
  if(pFile == NULL)
  { // if still null, print error msg, page fault about to ensue
    pFile = file;
  }
  else
  {
    pFile++;
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG */

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_DEBUG
  VDEC_MSG_SPRINTF_3( VDECDIAG_DEBUG, "[%s:%d] vdec_delete(%x)", pFile, line, pMemory );
#endif /* FEATURE_QTV_MEMORY_DEBUG */
}

/* ======================================================================
FUNCTION
  Vdec_GetTotalBytesAllocated

DESCRIPTION
  Return the total number of bytes allocated via calls to qtv_malloc and
  vdec_new.  Basically a sum total of all bytes allocated.

DEPENDENCIES
  none 

RETURN VALUE
  Total bytes allocated via Vdec_Malloc and Vdec_New.

SIDE EFFECTS
  none
========================================================================== */
uint32 Vdec_GetTotalBytesAllocated(void)
{
  return vdec_malloc_total_bytes;
}

/* ======================================================================
FUNCTION
  Vdec_ClearBytesAllocated

DESCRIPTION
  Clear the total number of bytes allocated via calls to vdec_malloc and
  vdec_new.

DEPENDENCIES
  none 

RETURN VALUE
  none

SIDE EFFECTS
  none
========================================================================== */
uint32 Vdec_ClearBytesAllocated(void)
{
  vdec_malloc_total_bytes = 0;
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG*/
  return 0;
}

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_ASSERT
/* ======================================================================
FUNCTION
  Vdec_AssertFailed

DESCRIPTION
  Empty function to place breakpoints in during de-bugging. Function is
  called before the program is halted / fatal message logged.

DEPENDENCIES
  none 

RETURN VALUE
  none

SIDE EFFECTS
  none
========================================================================== */
void Vdec_AssertFailed(void) {
  return;
}

int vdec_assert_level = 0; /* 0 causes error fatal */
                          /* 1 causes mesg fatal  */
#endif /* FEATURE_QTV_ASSERT */

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

$Header: //source/qcom/qct/multimedia/qtv/decoder/core/rel/2.0/src/vdecoder_utils.cpp#4 $
$DateTime: 2008/12/03 04:32:47 $
$Change: 796990 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "vdecoder_utils.h"
extern "C"
{
#include "TL_types.h"
#include "VDL_Types.h"
}
#include "MP4_Types.h"

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

struct VDEC2MEDIA_AUDIO_EQUIV
{
  Media::CodecType  media;
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

const VDEC2MEDIA_AUDIO_EQUIV s_mediaAudioMap[] =
{
  { Media::UNKNOWN_CODEC,     VDEC_CONCURRENT_NONE },
  { Media::EVRC_CODEC,        VDEC_CONCURRENT_AUDIO_EVRC },
  { Media::QCELP_CODEC,       VDEC_CONCURRENT_AUDIO_QCELP },
  { Media::AAC_CODEC,         VDEC_CONCURRENT_AUDIO_AAC },
  { Media::BSAC_CODEC,        VDEC_CONCURRENT_AUDIO_BSAC },
  { Media::GSM_AMR_CODEC,     VDEC_CONCURRENT_AUDIO_AMR },
  { Media::MP3_CODEC,         VDEC_CONCURRENT_AUDIO_MP3 },
  { Media::WMA_CODEC,         VDEC_CONCURRENT_NONE }, // Right now, no WMA support. This will change soon.
  { Media::CONC_CODEC,        VDEC_CONCURRENT_AUDIO_CONC },
  { Media::AMR_WB_CODEC,      VDEC_CONCURRENT_AUDIO_AMR_WB },
  { Media::AMR_WB_PLUS_CODEC, VDEC_CONCURRENT_AUDIO_AMR_WB_PLUS },
  { Media::EVRC_NB_CODEC,     VDEC_CONCURRENT_AUDIO_EVRC },
  { Media::EVRC_WB_CODEC,     VDEC_CONCURRENT_AUDIO_EVRC }
};


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
  ConvertMediaAudioToVdec

DESCRIPTION:
  Converts a Media::CodecType (from media.h) into a
  VDEC_AUDIO_CONFIG (from vdecoder_types.h)

PARAMETERS:
  const Media::CodecType mediaAudio
    The audio type to convert.

RETURN VALUE:
  VDEC_CONCURRENCY_CONFIG
    The corresponding vdec type.

SIDE EFFECTS:
  None.
===========================================================================*/
VDEC_CONCURRENCY_CONFIG ConvertMediaAudioToVdec( const Media::CodecType mediaAudio )
{
  VDEC_CONCURRENCY_CONFIG vdecAudio = VDEC_CONCURRENT_NONE;

  for ( int i = 0; i < ARR_SIZE(s_mediaAudioMap); ++i )
  {
    if ( s_mediaAudioMap[ i ].media == mediaAudio )
    {
      vdecAudio = s_mediaAudioMap[ i ].vdec;
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

}


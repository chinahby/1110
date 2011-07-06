#ifndef TL_COMMON_H
#define TL_COMMON_H
/* =======================================================================
                               Tl_common.h
DESCRIPTION
  This file defines the types and functions which are common to TL of 
  all codecs.

Copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/transform/common/rel/2.0/inc/tl_common.h#8 $
$DateTime: 2010/09/06 05:34:39 $
$Change: 1428535 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "log.h"
#include "log_codes.h"
extern "C" {
#include "crc.h"
}
#include "rex.h" 

#define DEC_QCIF_WIDTH 176
#define DEC_QCIF_HEIGHT 144

#define MB_COUNT_QCIF   99
#define MB_COUNT_QVGA  300
#define MB_COUNT_WQVGA 375
#define MB_COUNT_CIF   400
#define MB_COUNT_HVGA  690

#ifdef FEATURE_VIDEO_NPA
#error code not present
#endif

#ifdef FEATURE_QTV_DYNAMIC_CPU_SWITCHING
extern boolean qtv_cfg_enable_dynamic_cpu_switching;
#endif 
extern boolean qtv_cfg_init_frames_to_black;

typedef enum{
  VIDEO_MP4,
  VIDEO_H264,
  VIDEO_WMV,
  VIDEO_H263,
  VIDEO_REAL
}VideoDecoderType;

typedef enum{
  VIDEO_QCIF,
  VIDEO_QVGA,
  VIDEO_WQVGA,
  VIDEO_CIF,
  VIDEO_HVGA
}VideoResType;

#ifndef PLATFORM_LTK

/* YUV frame data */
#define LOG_MPEG4_MAX_YUV_SIZE  512
#define LOG_MPEG4_LOG_ALLOC_WAIT_MS  20
#define LOG_MPEG4_LOG_ALLOC_MAX_WAIT_MS 400
LOG_RECORD_DEFINE(LOG_MPEG4_YUV_FRAME_C)
  uint16 FrameNumber;          /* Frame number within current video */
  uint16 ChunkNumber;          /* Data block within current frame */
  uint16 DataSize;             /* Number of valid bits in the yuv buffer */
  uint8  YUV[LOG_MPEG4_MAX_YUV_SIZE];    /* Dump of YUV data from frame */
LOG_RECORD_END
typedef LOG_MPEG4_YUV_FRAME_C_type log_yuv_type;

LOG_RECORD_DEFINE(LOG_DEC_CRC_FRAME_C)
  uint16 FrameNumber;          /* Frame number within current video :decodeing order */
  uint16 LogCrcValue;
  uint32 TimeStamp;
LOG_RECORD_END
typedef LOG_DEC_CRC_FRAME_C_type log_dec_crc_type;

LOG_RECORD_DEFINE(LOG_DEC_XSCALE_YUV_FRAME_C)
  uint16 FrameNumber;          /* Frame number within current video */
  uint16 ChunkNumber;          /* Data block within current frame */
  uint16 DataSize;             /* Number of valid bits in the yuv buffer */
  uint8  YUV[LOG_MPEG4_MAX_YUV_SIZE];    /* Dump of YUV data from frame */
LOG_RECORD_END
typedef LOG_DEC_XSCALE_YUV_FRAME_C_type log_dec_xscale_yuv_type;

LOG_RECORD_DEFINE(LOG_DEC_XSCALE_CRC_FRAME_C)
  uint16 FrameNumber;          /* Frame number within current video :decodeing order */
  uint16 LogCrcValue;          /* CRC value for the xscaled yuv buffer */
  uint32 ScaledWidth;          /* xScaled width */
  uint32 ScaledHeight;         /* xScaled height */
LOG_RECORD_END
typedef LOG_DEC_XSCALE_CRC_FRAME_C_type log_dec_xscale_crc_type;

log_yuv_type * WaitForYuvLogBuffer( void );
log_dec_crc_type * WaitForCRCLogBuffer( void );
log_dec_xscale_yuv_type *WaitForXscaleYUVLogBuffer( void );
log_dec_xscale_crc_type *WaitForXscaleCRCLogBuffer( void );

#endif

/* =======================================================================
**                        Function Declarations
** ======================================================================= */
void LogYuvBuffer( uint8 *pYUVBuf, uint32 FrameNumber, uint16 DecWidth, uint16 DecHeight );
void LogYuvBufferCRC( uint8 *pYUVBuf, uint32 FrameNumber, uint32 FrameDisplayTime,
                             uint16 DecWidth, uint16 DecHeight );
/*Log dunctions for xscalar*/
void LogXScalerYUVBuffer( uint8 *pYUVBuf, uint16 FrameNumber, 
                                       uint32 scaledWidth, uint32 scaledHeight );
void LogXScalerCRCBuffer( uint8 *pYUVBuf, uint16 FrameNumber,
                                           uint32 ScaledWidth,
                                           uint32 ScaledHeight );


#if defined (FEATURE_QTV_MDP) 

/*===========================================================================

FUNCTION:
  colour_convert_planar_to_reverse_interlaced

DESCRIPTION:
  Does the colour conversion from YCbCr 420 planar to reverse colour interlaced
  for MDP. Presently for ARM only solution on 6260 targets.

INPUT/OUTPUT PARAMETERS:
  uint8*            pSrcBuffer
  uint8*            pTempYCbCrBuf
  uint32            width
  uint32            Height

RETURN VALUE:
   None.
   
SIDE EFFECTS:
  None.

============================================================================*/
void colour_convert_planar_to_reverse_interlaced
(
  uint8* pSrcBuffer,            /* Source buffer that gets modified */
  uint8* pTempYCbCrBuf,     /* temp buffer required to swap */
  uint32 width,                 /* Width of frame */
  uint32 height                 /* Height of frame */
);
#endif  /* FEATURE_QTV_MDP */

/*===========================================================================

FUNCTION:
  check_input_dimensions

DESCRIPTION:
  Checks for input dimension..currently it rejects QVGA for LCU..
  Later add for other on need

INPUT/OUTPUT PARAMETERS:
  uint32            width
  uint32            Height

RETURN VALUE:
   boolean.
   
SIDE EFFECTS:
  None.

============================================================================*/
boolean check_allowed_input_dimensions(uint32 Width,uint32 Height,VideoDecoderType VideoType);
#endif


/*=============================================================================
      TL_common.cpp

DESCRIPTION:
  This file defines the functions which are common to "Transform Layer" of all
  codecs.

   LIMITATIONS:

   ABSTRACT:

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  is only needed if the order of operations is important.

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

============================================================================*/
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/transform/common/rel/2.0/src/tl_common.cpp#14 $
$DateTime: 2010/12/01 00:09:49 $
$Change: 1536495 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/08/09    vs      Remove the dependencies of other modules.
--------   ---      ---------------------------------------------------------
========================================================================== */

#include "assert.h"
#include "TL_common.h"
#include "vdecoder_utils.h"

#ifdef FEATURE_QTV_LCU_QVGA_DYNAMIC_DISABLING
#error code not present
#endif /* FEATURE_QTV_LCU_QVGA_DYNAMIC_DISABLING */

#if defined(T_QSC6055) || defined(T_QSC6065) || defined(T_QSC6075) || defined(T_QSC6085) 
extern "C" {
#include "hw.h"
}
#endif /* T_QSC6055 */

#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A_TURBO
#error code not present
#endif

/* QTV config variables */

boolean qtv_cfg_enable_wmv3_optimizations = FALSE;

#ifdef FEATURE_QTV_WINDOWS_MEDIA_SMCDB_SUPPORT
boolean qtv_cfg_enable_wmv3_smcdb_support = TRUE;
#endif

#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
/* This flag is set to false to disable temporal scalability by performing
** only base layer decoding for clips that have more than one video layer.*/
boolean qtv_cfg_enable_ts = TRUE;
#endif

/* When joining a live-stream, some OEMs want video to be displayed right away,
** someothers want to wait for the first available I-frame. If
** TRUE: Start playing from the first frame (regardless of P or I frame)
** FALSE: Start playing from the first available I-frame */
boolean qtv_cfg_enable_video_at_first_frame = TRUE;

#ifdef FEATURE_MPEG4_B_FRAMES
/* B-frame support for MP4 */
boolean qtv_cfg_mp4_b_frames = TRUE;
#endif 

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
/* This global variable should be set to true by OEM if it wants Qtv to 
** release the DSP with the subsequent call to Qtv pause.*/
boolean qtv_cfg_enable_dsp_release = FALSE;
#endif 

#ifdef FEATURE_MP4_AAC_PLUS
/* Flag used to switch between MPEG4-AAC & MPEG4-AAC+ DSP images. If
** TRUE: MPEG4-AAC+ image is loaded and post-processing is done by ARM 
** FALSE: MPEG4-AAC image is loaded and post-processing is done by DSP */
boolean qtv_cfg_enable_aacplus = TRUE;
#endif

/* Flag used to enable or disable H263 Profile 3 on 6250. If 
** TRUE: Profile 3 is enabled and the H263 DSP image is used to decode H263
** FALSE: Profile 3 is disabled and the MPEG-AAC+ DSP image is used to decode 
** H263 */
boolean qtv_cfg_enable_h263_profile3 = FALSE;


#ifdef FEATURE_MPEG4_DEBLOCKER
/* This flag is used to turn on/turn off ARM Deblocking on Qtv. Will be in use 
** for customers wanting to do their own deblocking. Note that this refers to 
** Post loop deblocking.*/
boolean qtv_cfg_ARMDeblockingEnable = FALSE;
#endif 

boolean qtv_cfg_DSPDeblockingEnable;

#ifdef FEATURE_DIVX_311_ENABLE
/* Divx decoder is enabled */
boolean qtv_cfg_divx_311_enable = TRUE;
#endif

/* Flag used to enable playing a clip with unsupported Profile Level */
boolean qtv_cfg_play_unsupported_profile_level = TRUE;

#ifdef FEATURE_QTV_DYNAMIC_CPU_SWITCHING
/* Flag to indicate whether the dyanmic ARM/AHB clock switching is enabled.
** TRUE: Dynamic clock switching is enabled (for optimal power performance)
** FALSE: Dynamic clock switching is disabled (for optimial CPU performance)
*/
boolean qtv_cfg_enable_dynamic_cpu_switching = TRUE;
#endif /* FEATURE_QTV_DYNAMIC_CPU_SWITCHING */

boolean qtv_cfg_init_frames_to_black = FALSE;
/*===========================================================================
Function: LogYuvBuffer

Description: Log a complete YUV buffer by breaking it up into chuncks acceptable
             to the logging subsystem.  Won't return until the entire buffer has
             been logged or the logging is turned off by the DM.

Return Value:
   None.

Side Effects:
    Executed in the graph task context {mp4_process_decode_done()}.

============================================================================*/
void LogYuvBuffer( uint8 *pYUVBuf, uint32 FrameNumber,uint16 DecWidth, uint16 DecHeight )
{
#ifndef PLATFORM_LTK
  int32  YUVSize = DecHeight * DecWidth * 12/8;
  uint16 LogChunkCount = 0;
  uint16 LogSize = (uint16) MIN( YUVSize, LOG_MPEG4_MAX_YUV_SIZE );

  log_yuv_type *pLogYUVFrame = WaitForYuvLogBuffer();

  if (pLogYUVFrame)
  {
    /*
    **  Verify that we got a destination buffer, and that a clip reset hasn't
    **  de-allocated the source YUV input buffer.
    */
    while (pLogYUVFrame && pYUVBuf)
    {
      ASSERT(pLogYUVFrame);
      ASSERT(pYUVBuf);

      pLogYUVFrame->FrameNumber = FrameNumber;
      pLogYUVFrame->ChunkNumber = LogChunkCount++;
      pLogYUVFrame->DataSize = LogSize;
      memcpy( (void *) pLogYUVFrame->YUV, pYUVBuf, LogSize );
      log_commit( pLogYUVFrame );

      YUVSize -= LogSize;
      pYUVBuf += LogSize;

      if (YUVSize > 0)
      {
        LogSize = (uint16) MIN( YUVSize, LOG_MPEG4_MAX_YUV_SIZE );

        pLogYUVFrame = WaitForYuvLogBuffer();
      }
      else
      {
        pLogYUVFrame = NULL;
      }
    } /* while there are more frames to log */
  }
#endif
}

#ifndef PLATFORM_LTK
/*===========================================================================
Function: WaitForYuvLogBuffer

Description: Obtain a log buffer.  If log_alloc() is out of memory wait until
             the memory is available.  Return NULL if logging is disabled.

Return Value:
   NULL or a pointer to a log buffer.

Side Effects:

============================================================================*/
log_yuv_type* WaitForYuvLogBuffer( void )
{
  log_yuv_type *pBuf = NULL;
  uint32 TotalWaitTime = 0;

  /* Because log_alloc() will return NULL when it is out of memory we
   * need to keep looping until we get memory.  We'll monitor
   * log_status() just in case the log gets disabled.
   */
  while (!pBuf && log_status( LOG_MPEG4_YUV_FRAME_C ))
  {
    pBuf = (log_yuv_type*) log_alloc( LOG_MPEG4_YUV_FRAME_C, sizeof(*pBuf) );
    if (!pBuf)
    {
      if (TotalWaitTime < LOG_MPEG4_LOG_ALLOC_MAX_WAIT_MS)
      {
        rex_sleep(LOG_MPEG4_LOG_ALLOC_WAIT_MS);
        TotalWaitTime += LOG_MPEG4_LOG_ALLOC_WAIT_MS;        
      }
      else
      {
        VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR, 
                      "Couldn't get memory to log YUV buffer" );
        break;
      }  
    }
  }

  return pBuf;
}
#endif
/*===========================================================================
Function: LogYuvBufferCRC

Description: Log a complete YUV buffer by breaking it up into chuncks acceptable
             to the logging subsystem.  Won't return until the entire buffer has
             been logged or the logging is turned off by the DM.

Return Value:
   None.

Side Effects:
    Executed in the graph task context {mp4_process_decode_done()}.

============================================================================*/
void LogYuvBufferCRC( uint8 *pYUVBuf, uint32 FrameNumber, uint32 FrameDisplayTime,
                                     uint16 DecWidth, uint16 DecHeight )
{
#ifndef PLATFORM_LTK
  //the seed value to start with
  uint16 CRC_INIT = 0xFFFF;
  uint32  YUVSize = DecHeight * DecWidth * 12/8;
  log_dec_crc_type *pLogYUVFrame = WaitForCRCLogBuffer();

  if (pLogYUVFrame)
  {
    /*
    **  Verify that we got a destination buffer, and that a clip reset hasn't
    **  de-allocated the source YUV input buffer.
    */
      ASSERT(pLogYUVFrame);
      ASSERT(pYUVBuf);

    pLogYUVFrame->LogCrcValue = crc_16_l_step( CRC_INIT, pYUVBuf, YUVSize);
    pLogYUVFrame->FrameNumber = FrameNumber;
    pLogYUVFrame->TimeStamp = FrameDisplayTime;

    log_commit( pLogYUVFrame );
  }
  #endif
}
#ifndef PLATFORM_LTK
/*===========================================================================
Function: WaitForCRCLogBuffer

Description: Obtain a log buffer.  If log_alloc() is out of memory wait until
             the memory is available.  Return NULL if logging is disabled.

Return Value:
   NULL or a pointer to a log buffer.

Side Effects:

============================================================================*/
log_dec_crc_type* WaitForCRCLogBuffer( void )
{
  log_dec_crc_type *pBuf = NULL;
  uint32 TotalWaitTime = 0;

  /* Because log_alloc() will return NULL when it is out of memory we
   * need to keep looping until we get memory.  We'll monitor
   * log_status() just in case the log gets disabled.
   */
  while (!pBuf && log_status( LOG_DEC_CRC_FRAME_C ))
  {
    pBuf = (log_dec_crc_type*) log_alloc( LOG_DEC_CRC_FRAME_C, sizeof(*pBuf) );
    if (!pBuf)
    {
      if (TotalWaitTime < LOG_MPEG4_LOG_ALLOC_MAX_WAIT_MS)
      {    
        rex_sleep(LOG_MPEG4_LOG_ALLOC_WAIT_MS);
        TotalWaitTime += LOG_MPEG4_LOG_ALLOC_WAIT_MS;		
      }
      else
      {
        VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR, 
                      "Couldn't get memory to log CRC buffer" );
        break;
      }  	  
    }
  }

  return pBuf;
}

/*===========================================================================
Function: WaitForXscaleYUVLogBuffer

Description: Obtain a log buffer.  If log_alloc() is out of memory wait until
             the memory is available.  Return NULL if logging is disabled.

Return Value:
   NULL or a pointer to a log buffer.

Side Effects:

============================================================================*/
log_dec_xscale_yuv_type *WaitForXscaleYUVLogBuffer( void )
{
  log_dec_xscale_yuv_type *pBuf = NULL;
  uint32 TotalWaitTime = 0;    

  /* Because log_alloc() will return NULL when it is out of memory we
   * need to keep looping until we get memory.  We'll monitor
   * log_status() just in case the log gets disabled.
   */
  while (!pBuf && log_status( LOG_DEC_XSCALE_YUV_FRAME_C ))
  {
    pBuf = (log_dec_xscale_yuv_type*) log_alloc( LOG_DEC_XSCALE_YUV_FRAME_C, sizeof(*pBuf) );
    if (!pBuf)
    {
      if (TotalWaitTime < LOG_MPEG4_LOG_ALLOC_MAX_WAIT_MS)
      {        
        rex_sleep(LOG_MPEG4_LOG_ALLOC_WAIT_MS);  
		TotalWaitTime += LOG_MPEG4_LOG_ALLOC_WAIT_MS;
      }
      else
      {
        VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR, 
                      "Couldn't get memory to log Xscale YUV buffer" );
        break;
      }	  
    }
  }

  return pBuf;
}

/*===========================================================================
Function: WaitForXscaleCRCLogBuffer

Description: Obtain a log buffer.  If log_alloc() is out of memory wait until
             the memory is available.  Return NULL if logging is disabled.

Return Value:
   NULL or a pointer to a log buffer.

Side Effects:

============================================================================*/
log_dec_xscale_crc_type *WaitForXscaleCRCLogBuffer( void )
{
  log_dec_xscale_crc_type *pBuf = NULL;
  uint32 TotalWaitTime = 0;  

  /* Because log_alloc() will return NULL when it is out of memory we
   * need to keep looping until we get memory.  We'll monitor
   * log_status() just in case the log gets disabled.
   */
  while (!pBuf && log_status( LOG_DEC_XSCALE_CRC_FRAME_C ))
  {
    pBuf = (log_dec_xscale_crc_type*) log_alloc( LOG_DEC_XSCALE_CRC_FRAME_C, sizeof(*pBuf) );
    if (!pBuf)
    {
      if (TotalWaitTime < LOG_MPEG4_LOG_ALLOC_MAX_WAIT_MS)
      {    
        rex_sleep(LOG_MPEG4_LOG_ALLOC_WAIT_MS);  
        TotalWaitTime += LOG_MPEG4_LOG_ALLOC_WAIT_MS; 		
      }
      else
      {
        VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR, 
                      "Couldn't get memory to log Xscale CRC buffer" );
        break;
      }	  
    }
  }

  return pBuf;
}
#endif

/*===========================================================================
Function: LogXScalerYUVBuffer

Description: Log a complete xScaled YUV buffer by breaking it up into chuncks acceptable
             to the logging subsystem.  Won't return until the entire buffer has
             been logged or the logging is turned off by the DM.

Return Value:
   None.

Side Effects:
    Executed in the graph task context {mp4_process_decode_done()}.

============================================================================*/
LOCAL void LogXScalerYUVBuffer( uint8 *pYUVBuf, uint16 FrameNumber, 
                                       uint32 scaledWidth, uint32 scaledHeight )
{
#ifndef PLATFORM_LTK
  int32  YUVSize = scaledWidth * scaledHeight * 12/8;
  uint16 LogChunkCount = 0;
  uint16 LogSize = (uint16) MIN( YUVSize, LOG_MPEG4_MAX_YUV_SIZE );

  log_dec_xscale_yuv_type *pLogYUVFrame = WaitForXscaleYUVLogBuffer();

  if (pLogYUVFrame)
  {
    /*
    **  Verify that we got a destination buffer, and that a clip reset hasn't
    **  de-allocated the source YUV input buffer.
    */
    while (pLogYUVFrame && pYUVBuf)
    {
      ASSERT(pLogYUVFrame);
      ASSERT(pYUVBuf);

      pLogYUVFrame->FrameNumber = FrameNumber;
      pLogYUVFrame->ChunkNumber = LogChunkCount++;
      pLogYUVFrame->DataSize = LogSize;
      memcpy( (void *) pLogYUVFrame->YUV, pYUVBuf, LogSize );
      log_commit( pLogYUVFrame );

      YUVSize -= LogSize;
      pYUVBuf += LogSize;

      if (YUVSize > 0)
      {
        LogSize = (uint16) MIN( YUVSize, LOG_MPEG4_MAX_YUV_SIZE );

        pLogYUVFrame = WaitForXscaleYUVLogBuffer();
      }
      else
      {
        pLogYUVFrame = NULL;
      }
    } /* while there are more frames to log */
  }
#endif
} /* end LogXScalerYUVBuffer */

/*===========================================================================
Function: LogXScalerCRCBuffer

Description: Log the CRC value of the xScaled YUV buffer and the scaled 
             dimensions.

Return Value:
   None.

Side Effects:
    Executed in the graph task context {mp4_process_decode_done()}.

============================================================================*/
LOCAL void LogXScalerCRCBuffer( uint8 *pYUVBuf, uint16 FrameNumber,
                                           uint32 ScaledWidth,
                                           uint32 ScaledHeight )
{
#ifndef PLATFORM_LTK
  //the seed value to start with
  uint16 CRC_INIT = 0xFFFF;
  uint32  YUVSize = ScaledWidth * ScaledHeight * 12/8;
  log_dec_xscale_crc_type *pLogYUVFrame = WaitForXscaleCRCLogBuffer();
  
  if (pLogYUVFrame)
  {
    /*
    **  Verify that we got a destination buffer, and that a clip reset hasn't
    **  de-allocated the source YUV input buffer.
    */
      ASSERT(pLogYUVFrame);
      ASSERT(pYUVBuf);
        
    pLogYUVFrame->LogCrcValue = crc_16_l_step( CRC_INIT, pYUVBuf, YUVSize);
    pLogYUVFrame->FrameNumber = FrameNumber;
    pLogYUVFrame->ScaledWidth = ScaledWidth;
    pLogYUVFrame->ScaledHeight = ScaledHeight;

    log_commit( pLogYUVFrame );    
  }
#endif
} /* end LogXScalerCRCBuffer */
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
#if defined (FEATURE_QTV_MDP)
void colour_convert_planar_to_reverse_interlaced
(
  uint8* pSrcBuffer,            /* Source buffer that gets modified */
  uint8* pTempYCbCrBuf,     /* temp buffer required to swap */
  uint32 width,                 /* Width of frame */
  uint32 height                 /* Height of frame */
)
{
  uint32 CbAndCrSize = width * height / 2;
  uint32 CbOrCrSize  = CbAndCrSize / 2;
  uint8 *pCb;
  uint8 *pCr;
  uint8 *pByte; // pointer to the CbCr data

  if ((pSrcBuffer == NULL) || pTempYCbCrBuf == NULL)
  {
    VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_HIGH, "mp4_colour_convert_planar_to_reverse_interlaced: \
                 Memory pointers NULL" );
    return ;
  }

  if (pTempYCbCrBuf)
  { 
    memcpy(pTempYCbCrBuf, pSrcBuffer + width * height, CbAndCrSize);
    pByte = pSrcBuffer + width * height;
    pCb   = pTempYCbCrBuf;
    pCr   = pTempYCbCrBuf + CbOrCrSize;
    while (pCr < pTempYCbCrBuf + CbAndCrSize)
    { 
      // Because of a bug in the MDP, Cb and Cr have to be swapped
      pByte[0] = *pCr; // This would normally be Cb
      pByte[1] = *pCb; // This would normally be Cr
      pByte += 2;
      pCb++;
      pCr++;
    }
  }
}
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
boolean check_allowed_input_dimensions(uint32 Width,uint32 Height,VideoDecoderType VideoType)
{

  VDEC_USE_ARG1(VideoType);
   
#ifdef FEATURE_QTV_LCU_QVGA_DYNAMIC_DISABLING
  if (hw_msm_id() == HW_QSC6240)
  {
    if ( (Width > DEC_QCIF_WIDTH) || ((Height * Width ) > (DEC_QCIF_WIDTH * DEC_QCIF_HEIGHT)) )
    {
      return FALSE;
    }
  }
#elif (defined T_QSC6055)
  /* This is to disable video for SC2X low tier 6055 targets */
  hw_rev_type rev_sc2x;

  hw_partnum_version( &rev_sc2x );
  if ( (rev_sc2x.partnum == 0xFF)     /*0xFF indicates fuse burned => 6055*/
#if !(defined T_QSC6065) && !(defined T_QSC6075) && !(defined T_QSC6085) 
     || (rev_sc2x.partnum == 0xF0)      /* 0xF0 indicates superchip (this is here as superchip could be used to run 6055 build)*/
#endif /*!(defined T_QSC6065) && !(defined T_QSC6075) && !(defined T_QSC6085) */
      )    
  {
    VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_HIGH, "Video not supported on 6055" );
    return false;
  }

  if ( ((Width > DEC_QCIF_WIDTH) || (Height * Width ) > (DEC_QCIF_WIDTH * DEC_QCIF_HEIGHT)) && (VideoType !=VIDEO_MP4)
#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A_TURBO
#error code not present
#endif
     )
  {
    return FALSE;
  }
#else /* T_QSC6055 */
  VDEC_USE_ARG2(Width, Height);
#endif /* FEATURE_QTV_LCU_QVGA_DYNAMIC_DISABLING */

#ifdef FEATURE_VIDEO_NPA
#error code not present
#else
 if ((((Width * Height) >> 8) > MB_COUNT_QVGA) &&  VIDEO_WMV == VideoType)
#endif
 {
   return FALSE;
 }

  return TRUE;
}


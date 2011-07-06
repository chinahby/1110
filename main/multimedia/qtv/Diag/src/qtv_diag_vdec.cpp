/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          QTV Diagnostic Packet Service Client Routines

General Description
  This file contains implementations for QTV Diag test commands via the
  DMSS diagnostics packet service.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

$Header: //source/qcom/qct/multimedia/qtv/diag/main/latest/src/qtv_diag_vdec.cpp#15 $
$DateTime: 2011/02/04 05:02:15 $
$Change: 1607101 $
                           Edit History

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/05   seansw  Initial Revision
===========================================================================*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_diag_vdec.h"
#include "qtv_diag_display.h"
#include "vdecoder.h"
#include "vdecoder_types.h"
#include "vdecoder_utils.h"
#include "fileMedia.h"
#include "ITrackList.h"
#include "ReferenceCountedPointer.h"
#ifndef FEATURE_WINCE
#include "OEMMediaMPEG4.h"
#endif // #ifndef FEATURE_WINCE
#include "qtv_event.h"
#include "mp4buf.h"
#include "Mp4_Types.h"

extern "C" {
  #include "queue.h"
  #include "event.h"
  #include "event_defs.h"
#ifndef FEATURE_WINCE
  #include "fs.h"
#else
#error code not present
#endif
  #include "assert.h"
}


/*==========================================================================*
 * Type declarations/structs                                                *
 *==========================================================================*/
typedef struct queue_el
{
  q_link_type       link;
  VDEC_INPUT_BUFFER *pBuffer;
  bool              bQueued;
} InputQElem;

struct DecodeBuffer
{
  uint8* pData[COMMON_MAX_LAYERS];
  uint32 maxSize[COMMON_MAX_LAYERS];
  int32  size[COMMON_MAX_LAYERS];
  uint32 timestamp[COMMON_MAX_LAYERS];
  bool bUseAgain[COMMON_MAX_LAYERS];
};

typedef struct qtvdiag_handle_cmd_struct
{
  qtvdiag_command_e_type api_id;
  VDEC_ERROR (*handler)(vdec_diag_cmd_type *cmd_ptr);
  const char * diag_msg;
} qtvdiag_handler_type;

/* used for storing recording preferences
 * this struct MUST BE PACKED as it is memcpy
 * from the PACKED diag packet struct
 */
record_type recInfo;

struct vdec_info_struct
{
  int                layers;
  VDEC_BLOB **       ppBlob;
  VDEC_BLOB *              pConfig;
  VDEC_CONCURRENCY_CONFIG  audio;
  VDEC_STREAM_ID     streamID;
  VDEC_FRAME *       pFrame;
};

/*==========================================================================*
 * Functions                                                                *
 *==========================================================================*/
static VDEC_ERROR qtvdiag_vdec_create( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_initialize( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_decode( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_destroy( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_dump( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_flush( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_free_blob( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_parse( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_queue( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_reuse_frame_buffer( vdec_diag_cmd_type * );
VDEC_ERROR qtvdiag_vdec_cleanup( vdec_diag_cmd_type * );
static VDEC_ERROR qtvdiag_vdec_diag_init( vdec_diag_cmd_type *cmd_ptr );

static Media::MediaStatus get_stream_header(VDEC_INPUT_BUFFER *pBuffer);
static Media::MediaStatus get_h264_stream_header(VDEC_INPUT_BUFFER *input);
static Media::MediaStatus get_mp4_stream_header(VDEC_INPUT_BUFFER *input);
static Media::MediaStatus get_wmv_stream_header(VDEC_INPUT_BUFFER *input);
#ifdef FEATURE_H264_DECODER
static bool PrependNalLen(uint8 * buf, uint32 size, uint32 sizeOfNalLengthField);
#endif
static InputQElem *get_next_input_buffer( boolean bParseStreamHeader );

/*==========================================================================*
 * Global Data Declarations                                                 *
 * Used to pass information between seperate test function calls            *
 *==========================================================================*/
static vdec_info_struct vdec_info;
static q_type inputQ;

/*==========================================================================*
 * Because I hate having to look up error codes                             *
 * This'll need to be updated if VDEC_ERROR ever changes...                 *
 *==========================================================================*/
static char (errors[])[40] =
{ "Everything fine",
  "Operation Unsupported",
  "NULL Stream ID",
  "NULL pointer",
  "Unknown FTYP",
  "Out of memory",
  "DSP initialization timeout",
  "Unknown error",
  "Layer out of bounds",
  "No input available to decode",
  "Bad parameter",
  "Bad BLOB type passed",
  "Corrupt BLOB"
};

static bool bInitialized = FALSE;
static Media *pMedia = NULL;
static uint32 videoMaxSize[COMMON_MAX_LAYERS];
static int frameCount=0;
static int filedes = 0;

/*===========================================================================
=                         Handler function table                            =
===========================================================================*/
static const qtvdiag_handler_type handler_table[] = {
  {QTVDIAG_COMMAND_VDEC_CREATE,qtvdiag_vdec_create,
    "VDEC_DIAG: vdec_create"},
  {QTVDIAG_COMMAND_VDEC_INITIALIZE,qtvdiag_vdec_initialize,
    "VDEC_DIAG: vdec_initialize"},
  {QTVDIAG_COMMAND_VDEC_DECODE,qtvdiag_vdec_decode,
    "VDEC_DIAG: vdec_decode"},
  {QTVDIAG_COMMAND_VDEC_DESTROY,qtvdiag_vdec_destroy,
    "VDEC_DIAG: vdec_destroy"},
  {QTVDIAG_COMMAND_VDEC_DUMP,qtvdiag_vdec_dump,
    "VDEC_DIAG: vdec_dump"},
  {QTVDIAG_COMMAND_VDEC_FLUSH,qtvdiag_vdec_flush,
    "VDEC_DIAG: vdec_flush"},
  {QTVDIAG_COMMAND_VDEC_FREE_BLOB,qtvdiag_vdec_free_blob,
    "VDEC_DIAG: vdec_free_blob"},
  {QTVDIAG_COMMAND_VDEC_PARSE_MEDIA_HEADER,qtvdiag_vdec_parse,
    "VDEC_DIAG: vdec_parse_media_header"},
  {QTVDIAG_COMMAND_VDEC_QUEUE,qtvdiag_vdec_queue,
    "VDEC_DIAG: vdec_queue"},
  {QTVDIAG_COMMAND_VDEC_REUSE_FRAME_BUFFER,qtvdiag_vdec_reuse_frame_buffer,
    "VDEC_DIAG: vdec_reuse_frame_buffer"},
  {QTVDIAG_COMMAND_VDEC_SET_DIAG_VARS,qtvdiag_vdec_diag_init,
    "Setting internal state"},
  {QTVDIAG_COMMAND_VDEC_CLEAR_DIAG_VARS,qtvdiag_vdec_cleanup,
    "Clearing internal state."}};

/*===========================================================================
=                             Helper Functions                              =
===========================================================================*/
/*===========================================================================

FUNCTION qCompareFn

DESCRIPTION
   Compare function used for linear search of the input queue.

DEPENDENCIES
  None.

RETURN VALUE
  Whether or not compare_val is the same as the queue item's input buffer.

SIDE EFFECTS
  None.

===========================================================================*/
int qCompareFn(void *item_ptr, void *compare_val)
{
  return (((InputQElem *)item_ptr)->pBuffer==compare_val);
}

/*===========================================================================

FUNCTION qtvdiag_vdecQueue_CbFn

DESCRIPTION
   This is the callback function used to register with VDEC to get queue
   events back from the API executed

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern "C" void qtvdiag_vdecQueue_CbFn(
  const VDEC_STREAM_ID      stream,
  VDEC_INPUT_BUFFER * const pInput,
  void * const              /*pCbData, unused*/)
{
  uint32 size=0;
  int i;
  InputQElem *pElem;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"VDEC_DIAG: Queue Callback");
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"VDEC_DIAG: stream ID %d",stream);

  if (stream == vdec_info.streamID)
  {
    /* this is the stream we expect, let's see if the decoder is done with this */
    for(i=0;i<vdec_info.layers;i++)
    {
      size=pInput->buffer_size[i];
      if(size > 0)
      {
        break;
      }
    }
    if (size==0)
    {
      /* get the item off the queue */
      pElem=(InputQElem *)q_linear_search(&inputQ, qCompareFn, pInput);
      if (pElem)
      {
      q_delete(&inputQ, &pElem->link);
        QTV_Free(pElem);
      }

      /* decoder is done, let's clean this up */
      for(i=0;i<vdec_info.layers;i++)
      {
        QTV_Free(pInput->buffer[i]);
      }
      QTV_Free(pInput);
    }

  }
  else
  {
    /* not what we expect, what happened? */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: QC - stream ID does not match expected");
  }
}

/*===========================================================================

FUNCTION qtvdiag_vdecFrame_CbFn

DESCRIPTION
   This is the callback function used to register with VDEC to get frame
   events back from the API executed.
   This function also displays the decoded frame to the screen as well
   as saving to EFS, if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern "C" void qtvdiag_vdecFrame_CbFn(
  const VDEC_STREAM_ID stream,
  VDEC_CB_STATUS    status,
  VDEC_FRAME * const   pFrame,
  uint32                nBytesConsumed,
  void * const         /*pCbData, unused */)
{
  bool bFileWrite=false;
  fs_off_t   result;
  fs_size_t write_result;
  vdec_diag_callback_status_type ret_status;

  QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"VDEC_DIAG: stream ID %d, status %d, pFrame 0x%x",
           stream,status,pFrame);

  /* preload reply defaults */
  ret_status.error = status;
  ret_status.wr_status = 0; /* no write done */

  /* check incoming stream ID */
  if (stream == vdec_info.streamID)
  {
    if (status!=VDEC_STATUS_GOOD_FRAME)
    {
      if (status==VDEC_STATUS_FLUSH)
      {
        /* vdec_flush was called during decode */
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Frame flushed");
      }
      else
      {
        /* badness happened, we can't do the rest */
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Bad frame");
      }
    }
    else if (pFrame==NULL)
    {
      /* good frame, but NULL pointer? nonsense */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Frame pointer is NULL, but status is GOOD");
    }
    else
    {
      /* this is the stream we expect, save its pointer, increment frame count */
      vdec_info.pFrame = pFrame;
      frameCount++;

      /* Display the frame centered on the screen
       * Note: a valid frame can have NO buffer.
       * Let's check for this and not crash the phone. */
      if(pFrame->pBuf)
      {
        qtvdiag_display_frame ((unsigned short *)pFrame->pBuf,
                               pFrame->cwin.y2 - pFrame->cwin.y1,pFrame->cwin.x2 - pFrame->cwin.x1);
      }

      if (recInfo.units == FRAME)
      {
        /* Frame */
        bFileWrite = ((int)recInfo.start <= frameCount) && ((int)recInfo.stop >= frameCount);
      }
      else
      {
        /* Timestamp */
        bFileWrite = (recInfo.start <= pFrame->timestamp) &&
                     (recInfo.stop >= pFrame->timestamp);
      }

      /* write to EFS file test */
      if(bFileWrite && filedes>0)
      {
        ret_status.wr_status = 2; /* write failed */

        /* get current file position */
        result = efs_lseek(filedes,0,SEEK_CUR);
        if(result >= 0)
        {
          /* write frame "header" */
          write_result = efs_write(filedes,pFrame,sizeof(VDEC_FRAME));

          /* check for write success, and an image buffer to write */
          if(write_result >= 0 && pFrame->pBuf)
          {
            /* write frame data */
            int frame_byte_size;
            if(pFrame->format.fourcc == VDEC_FOURCC_I420 && pFrame->format.bbp == 12)
            {
              frame_byte_size=(pFrame->cwin.x2 - pFrame->cwin.x1)*(pFrame->cwin.y2 - pFrame->cwin.y1)* MP4_YUV_BITS_PER_PIXEL / 8;
            }
            else if(pFrame->format.fourcc==VDEC_FOURCC_BI_RGB && pFrame->format.bbp == 16)
            {
              frame_byte_size=(pFrame->cwin.x2 - pFrame->cwin.x1)*(pFrame->cwin.y2 - pFrame->cwin.y1)* MP4_RGB_BITS_PER_PIXEL / 8;
            }
            else
            {
              frame_byte_size=0;
            }
            write_result = efs_write(filedes,pFrame->pBuf,frame_byte_size);
            if(write_result >= 0)
            {
              /* write succeded */
              ret_status.wr_status = 1;
            }
            else
            {
              /* write failed, truncate to frame boundary */
              efs_ftruncate(filedes,result);
              efs_close(filedes);
              filedes=0;
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: Output file closed, fs out of space.");
            }
          }
          else
          {
            /* write failed, truncate to frame boundary */
            efs_ftruncate(filedes,result);
            efs_close(filedes);
            filedes=0;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: Output file closed, fs out of space.");
          } /* if header write succeeded */
        }
        else
        {
          /* efs_lseek failed */
          efs_close(filedes);
          filedes=0;
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: Output file closed, file error.");
        } /* if efs_lseek succeeded */
      } /* write to efs check */
    } /* status check */
  }
  else
  {
    /* not what we expect, what happened? */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: FC - stream ID does not match expected");
  } /* stream ID check */
  /* "nBytesConsumed" variable declared but not used in the function, 
       below statement is added to clear the compiler warning. */
  QTV_USE_ARG1(nBytesConsumed);
  /* report the callback to DIAG */
  event_report_payload(EVENT_QTV_VDEC_DIAG_DECODE_CALLBACK,
                       sizeof(vdec_diag_callback_status_type),
                       &ret_status);
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_create

DESCRIPTION
  Handles the call to vdec_create.

DEPENDENCIES
  qtvdiag_vdec_diag_init and handle_parse must be called first for correct operation.

RETURN VALUE
  The return status from vdec_create.

SIDE EFFECTS
  None.

===========================================================================*/
static VDEC_ERROR qtvdiag_vdec_create( vdec_diag_cmd_type * )
{
  VDEC_ERROR vdec_error=VDEC_ERR_NULL_POINTER;
  const char *fourcc[] ={"isom","h264","wmv1","wmv2","wmv3","rv40","jpeg"};
  uint32 diag_ret = 0;
  unsigned int index = 0;
  bool bOK = true;

  if (pMedia)
  {
    switch (pMedia->GetVideoCodecType())
    {
      case Media::H263_CODEC : 
      case Media::MPEG4_CODEC: index = 0; break;
      case Media::H264_CODEC : index = 1; break;
      case Media::WMV1_CODEC : index = 2; break;
      case Media::WMV2_CODEC : index = 3; break;
      case Media::WMV3_CODEC : index = 4; break;
      case Media::RV40_CODEC : index = 5; break;
      case Media::JPEG_CODEC : index = 6; break;
      default: 
        bOK = false;
        vdec_error = VDEC_ERR_INVALID_PARAMETER;
        QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,"DoGenericVdecCreate failed!! error - unknown Codec type");
    }

    if (bOK == true)
    {
      vdec_error = vdec_create(fourcc[index],&vdec_info.streamID);
      if (vdec_error != VDEC_ERR_EVERYTHING_FINE)
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failure in vdec_create: %d", vdec_error );
      }
    }
  }
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);
  return vdec_error;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_initialize

DESCRIPTION
  Handles the call to vdec_initialize.

DEPENDENCIES
  qtvdiag_vdec_create  must be called first for correct operation.

RETURN VALUE
  The return status from vdec_initialize.

SIDE EFFECTS
  None.

===========================================================================*/
VDEC_ERROR qtvdiag_vdec_initialize( vdec_diag_cmd_type * )
{
  VDEC_ERROR vdec_error = VDEC_ERR_NULL_POINTER;
  VDEC_DIMENSIONS frameSize;
  VDEC_CONCURRENCY_CONFIG concurrencyConfig;
  VDEC_PARAMETER_DATA parm_data;
  Media::CodecType videoCodec;
  bool bOK = true;
  uint32 width,height,diag_ret;

  if (vdec_info.streamID == NULL)
  {
    vdec_error = VDEC_ERR_NULL_STREAM_ID;
    bOK = false;
  }

  if (pMedia == NULL)
  {
    vdec_error = VDEC_ERR_NULL_POINTER;
    bOK = false;
  }

  if (bOK == true)
  {
    ASSERT( vdec_info.layers <= MAX_MP4_LAYERS );
    concurrencyConfig = vdec_info.audio;
    videoCodec = pMedia->GetVideoCodecType();

    if (videoCodec == Media::H264_CODEC)
    {
#ifdef FEATURE_H264_DECODER
      parm_data.sizeOfNalLengthField.sizeOfNalLengthField = pMedia->GetSizeOfNALLengthField(0);
#endif
      vdec_set_parameter(vdec_info.streamID,VDEC_PARM_SIZE_OF_NAL_LENGTH_FIELD,&parm_data);

#ifdef FEATURE_QTV_BCAST_ISDB
#error code not present
#endif /* FEATURE_QTV_BCAST_ISDB */
      {
        parm_data.startCodeDetection.bStartCodeDetectionEnable = false;     
      }
      vdec_set_parameter(vdec_info.streamID,VDEC_PARM_START_CODE_DETECTION,&parm_data);
    }//end of H264 specific initialization

    parm_data.deblockerEnable.bDeblockerEnable = TRUE;
    vdec_set_parameter(vdec_info.streamID,VDEC_PARM_DEBLOCKER_ENABLE,&parm_data);

    pMedia->GetVideoDimensions( &width, &height );
    frameSize.height = (unsigned short)width;
    frameSize.height = (unsigned short)height;

    vdec_error = vdec_initialize(vdec_info.streamID,
                                 vdec_info.pConfig,
                                 vdec_info.layers,
                                 AllocateOutputBuffer,
                                 FreeOutputBuffer,
                                 qtvdiag_vdecFrame_CbFn,
                                 NULL,
                                 frameSize,
                                 concurrencyConfig);
    if (vdec_error != VDEC_ERR_EVERYTHING_FINE)
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,"vdec_initialize failed!! error - %d",vdec_error);
    }
  }
  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);
  return vdec_error;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_decode

DESCRIPTION
  calls the vdec_decode function.

DEPENDENCIES
  vdec_create must have been already called.

RETURN VALUE
  the return status from vdec_decode.

SIDE EFFECTS
  None.

===========================================================================*/
static VDEC_ERROR qtvdiag_vdec_decode( vdec_diag_cmd_type * )
{
  VDEC_ERROR vdec_error;
  int32 diag_ret;

  vdec_error = vdec_decode( vdec_info.streamID );

  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_destroy

DESCRIPTION
  calls the vdec_destroy function.

DEPENDENCIES
  vdec_create must have been already called.

RETURN VALUE
  the return status from vdec_destroy.

SIDE EFFECTS
  None.

===========================================================================*/
static VDEC_ERROR qtvdiag_vdec_destroy( vdec_diag_cmd_type * )
{
  VDEC_ERROR vdec_error;
  int32 diag_ret;

  vdec_error = vdec_destroy( vdec_info.streamID );

  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_dump

DESCRIPTION
  call the vdec_dump function.

DEPENDENCIES
  vdec_create must have been already called.

RETURN VALUE
  the return status of vdec_dump.

SIDE EFFECTS
  None.

===========================================================================*/
static VDEC_ERROR qtvdiag_vdec_dump( vdec_diag_cmd_type * )
{
  VDEC_ERROR vdec_error = VDEC_ERR_OPERATION_UNSUPPORTED;
  int32 diag_ret;

  /* vdec_error = vdec_dump( vdec_info.streamID, vdec_info.ppBlob ); */

  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_flush

DESCRIPTION
  call the vdec_flush function.

DEPENDENCIES
  vdec_create must have been already called.

RETURN VALUE
  The return status of vdec_flush.

SIDE EFFECTS
  None.

===========================================================================*/
static VDEC_ERROR qtvdiag_vdec_flush( vdec_diag_cmd_type * )
{
  VDEC_ERROR vdec_error;
  int32 diag_ret;

  vdec_error = vdec_flush(vdec_info.streamID);

  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_free_blob

DESCRIPTION
  calls vdec_free_blob function, returning the BLOB to the decoder.

DEPENDENCIES
  None.

RETURN VALUE
  the return status of vdec_free_blob.

SIDE EFFECTS
  None.

===========================================================================*/
static VDEC_ERROR qtvdiag_vdec_free_blob( vdec_diag_cmd_type * )
{
  VDEC_ERROR vdec_error = VDEC_ERR_EVERYTHING_FINE;
  int32 diag_ret;

  for (int idx=0;idx<vdec_info.layers;idx++)
  {
    vdec_free_blob( vdec_info.ppBlob[idx] );
  }
  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

/* ======================================================================
FUNCTION:
  RefillDecodeBuf

DESCRIPTION:
  Refills the MP-4 decode buffer

INPUT/OUPUT PARAMETERS:
  layer

RETURN VALUE:
  The result of the refill operation.

SIDE EFFECTS:
  None.

ORIGINAL COPY/PASTE:
  From RefillMpeg4DecodeBuf - videoDec.cpp
========================================================================== */
static Media::MediaStatus RefillDecodeBuf(
  const int layer,
  VDEC_INPUT_BUFFER *input)
{
  Media::MediaStatus retval=Media::DATA_OK;
  static Media::MediaStatus dataState[COMMON_MAX_LAYERS] = {Media::DATA_OK};

  if (!input)
  {
    /* need to have something to add this layer to... */
    return Media::DATA_NULLFILL;
  }
  input->buffer_size[layer] = 0;
  if (pMedia)
  {
    /* allocate space for layer buffer */
    if (!input->buffer[layer])
    {
      input->buffer[layer] = (uint8 *)QTV_Malloc(videoMaxSize[layer]);
      if (!input->buffer[layer])
      {
        return Media::DATA_NULLFILL;
      }

    }
    if (input->buffer[layer])
    {
      Media::MediaStatus status = Media::DATA_OK;
      long nBytes=0;

      //get sample

#ifdef FEATURE_H264_DECODER
      if (pMedia->GetVideoCodecType() == Media::H264_CODEC)
      {
        nBytes = pMedia->GetNextParamSetNAL( status,
                                             layer,
                                             input->buffer[layer],
                                             videoMaxSize[layer] );
      }
      else
      {
        status = Media::DATA_END;
      }

      if (status == Media::DATA_END)
#endif
      {
        nBytes = pMedia->GetNextLayeredVideoSample( status,
                                                    layer,
                                                    input->buffer[layer],
                                                    videoMaxSize[layer]);
      }

      //check status
      switch (status)
      {
        case Media::DATA_OK:
          //get timestamp.  this does an unsigned to signed conversion,
          //if the timestamp is unknown it will be (-1) here.
          input->timestamp[layer] = pMedia->
                                    GetTimestampForCurrentLayeredVideoSample(layer);
          //decoder wants msec input.
          input->timestamp[layer] = pMedia->
                                    ConvertVideoTimestamp(input->timestamp[layer],layer);

          if (!pMedia->CheckError(Common::GETVIDEOTIME) && nBytes > 0)
          {
            //this sample is good.
            input->buffer_size[layer] = nBytes;
            input->layers = vdec_info.layers;
            dataState[layer]=Media::DATA_OK;
          }
          else
          {
            retval = Media::DATA_ERROR;
            dataState[layer]=Media::DATA_ERROR;
          }
          break;

        case Media::DATA_ERROR:
          //get error detail
          (void)pMedia->CheckError(Common::GETVIDEO);
          /*lint -fallthrough */   
        case Media::DATA_END:
          dataState[layer] = status;
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
          {
            int b,tmp = 0;
            for (b=0; b<vdec_info.layers; b++)
            {
              if (dataState[b] != Media::DATA_OK)
                ++tmp;  // DATA_END = 1
            }
            /* if all layer done, set overall state*/
            if (tmp == b)
            {
              retval = Media::DATA_END;
            }
          }
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */
          break;

#ifdef FEATURE_FILE_FRAGMENTATION
        case Media::DATA_FRAGMENT:
          dataState[layer]=Media::DATA_FRAGMENT;
          retval = Media::DATA_FRAGMENT;
          break;
#endif  /* FEATURE_FILE_FRAGMENTATION */
        case Media::DATA_REALLOCATE_BUFFER:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Data Reallocate Buffer");
          for (int idx = 0; idx < vdec_info.layers; idx++)
          {
            uint32 bufferSize = pMedia->GetMaxVideoBufferSizeDB(idx);
            if (bufferSize > videoMaxSize[idx])
            {
              videoMaxSize[idx] = bufferSize;
              if (input->buffer[idx])
              {
                QTV_Free(input->buffer[idx]);
                input->buffer[idx] = NULL;
              }
              input->buffer[idx] = (uint8*)QTV_Malloc(videoMaxSize[idx]);
              if (input->buffer[idx] == NULL)
              {
                QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "MEMORY ERROR: Malloc Failed %d Bytes", videoMaxSize[idx] );
                return Media::DATA_END;
              }
              else
              {
                input->buffer_size[layer] = 0;
              }
            }
          }
          break;
        default:
          //unexpected status.
          retval = Media::DATA_ERROR;
          dataState[layer]=Media::DATA_ERROR;
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Unexpected status encountered: %d",status);
          break;
      }
    }
  }

  return retval;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_parse, and helpers

DESCRIPTION
  calls vdec_queue and queues the stream header for decode.

DEPENDENCIES
  qtvdiag_vdec_diag_init must have been already called.

RETURN VALUE
  the return status of vdec_parse_media_header.

SIDE EFFECTS
  None.

===========================================================================*/
static VDEC_ERROR qtvdiag_vdec_parse( vdec_diag_cmd_type * )
{
  InputQElem *item=NULL;
  VDEC_ERROR vdec_error=VDEC_ERR_NULL_POINTER;
  int32 diag_ret;

  if (pMedia)
  {
    /* get next buffer to send */
    item=get_next_input_buffer(true);
    if (item)
    {
      vdec_error=vdec_queue( vdec_info.streamID,
                             item->pBuffer,
                             qtvdiag_vdecQueue_CbFn,
                             NULL);
      if (vdec_error==VDEC_ERR_EVERYTHING_FINE)
      {
        item->bQueued=true;
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: vdec_queue - unable to create input buffer");
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: vdec_queue - no media file loaded");
  }

  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

/* ======================================================================
FUNCTION:     get_next_input_buffer

DESCRIPTION:
  Gets the next buffer needing to be sent to the decoder.
  If there is not one currently queued, one will be created.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  a pointer to the next VDEC_INPUT_BUFFER to be vdec_queue'd

SIDE EFFECTS:
  It may increase the size of the input queue.
========================================================================== */
static InputQElem *get_next_input_buffer( boolean bParseStreamHeader )
{
  InputQElem *iter,*item;
  VDEC_INPUT_BUFFER *input=NULL;
  Media::MediaStatus retval = Media::DATA_OK;
  int i=0,size=0;

  /* parse the list looking for unfinished or unqueued items
   * technically, any unfinished items will be at the end
   * barring some really big problem, but we should check anyways
   * no big deal if the list itself is NULL, we'll catch that
   */
  for (iter=(InputQElem *)q_check(&inputQ);iter;
      iter=(InputQElem *)q_next(&inputQ,&iter->link))
  {
    /* unqueued item, from either PrepPlay_H263, or a failed vdec_queue call
     * let's try to use this */
    if (!iter->bQueued)
    {
      return iter;
    }
    /* previously queued, let's check if any layers are length zero */
    for(i=0;(i<vdec_info.layers) && (i < COMMON_MAX_LAYERS);i++)
    {
      size=iter->pBuffer->buffer_size[i];
      if (size==0)
      {
        /* zero length layer - must have been consumed by decoder
         * break out to refill this used layer. */
        break;
      }
    }
  }

  if (!iter)
  {
    /* looks like we need to make a new input buffer */
    input = (VDEC_INPUT_BUFFER *)QTV_Malloc(sizeof(VDEC_INPUT_BUFFER));
    /* if malloc failed, there are bigger problems, get out */
    if (!input)
    {
      return NULL;
    }
    memset((void *)input,0,sizeof(VDEC_INPUT_BUFFER));

    if (bParseStreamHeader == true)
    {
      retval = get_stream_header(input);
    }
    else
    {
      /* fill this buffer */
      for(i=0;(i<vdec_info.layers) && (i < COMMON_MAX_LAYERS);i++)
      {
        /* skip zero length frames, and reallocates */
        do
        {
          retval=RefillDecodeBuf(i,input);
        } while (retval==Media::DATA_OK && input->buffer_size[i]==0);
        if (retval!=Media::DATA_OK)
        {
          break;
        }
      }
    }
  }
  else
  {
    if (bParseStreamHeader == true)
    {
      retval = get_stream_header(iter->pBuffer);
    }
    else
    {
      /* use old buffer, now fill the used layer.
       * skip zero length frames, and reallocates */
      do
      {
        retval=RefillDecodeBuf(i,iter->pBuffer);
      } while (retval==Media::DATA_OK && iter->pBuffer->buffer_size[i]==0);
    }
  }

  /* check if fill operation succeeded */
  if (retval==Media::DATA_OK)
  {
    /* check for new input buffer creation */
    if (!iter)
    {
      item = (InputQElem *)QTV_Malloc(sizeof(InputQElem));
      if (!item)
      {
        /* allocation failure, clean up and get out */
        for (int j=0;j<i;j++)
        {
          if (input->buffer[j]) QTV_Free(input->buffer[j]);
        }

        QTV_Free(input);
        return NULL;
      }
      item->pBuffer=input;
      item->bQueued=false;
      (void)q_link(item,&item->link);
      q_put(&inputQ, &item->link);
      return item;
    }
    else
    {
      /* partial fill, we're done */
      return iter;
    }
  }
  else
  {
    /* report end of clip */
    qtv_event_clip_ended_type qtv_event_clip_ended_payload;
    qtv_event_clip_ended_payload.NumFrameErrors = 0;
    if (retval==Media::DATA_END)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"End of clip reached");
      qtv_event_clip_ended_payload.end_type = QTV_EVENT_CLIP_ENDED_SUCCESS;
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Data error retrieving next frame");
      qtv_event_clip_ended_payload.end_type = QTV_EVENT_CLIP_ENDED_ERROR_ABORT;
    }
    event_report_payload(EVENT_QTV_CLIP_ENDED,
                         sizeof(qtv_event_clip_ended_type),
                         (void *)&qtv_event_clip_ended_payload);

    /* Clean up our media object, since it is no longer needed */
    QTV_Delete(pMedia);
    pMedia=NULL;

    /* free anything we've allocated on a new buffer */
    if (!iter)
    {
      for (int j=0;j<i;j++)
      {
        if (input->buffer[j]) QTV_Free(input->buffer[j]);
      }
      QTV_Free(input);
    }
    return NULL;
  }
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_queue

DESCRIPTION
  call the vdec_queue function.


DEPENDENCIES
  vdec_create must have been called, and a buffer must be available.

RETURN VALUE
  the return status of vdec_queue, or VDEC_ERR_NULL_POINTER if no input
  could be created.

SIDE EFFECTS
  None.

===========================================================================*/
static VDEC_ERROR qtvdiag_vdec_queue( vdec_diag_cmd_type * )
{
  InputQElem *item=NULL;
  VDEC_ERROR vdec_error=VDEC_ERR_NULL_POINTER;
  int32 diag_ret;

  if (pMedia)
  {
    /* get next buffer to send */
    item=get_next_input_buffer(false);
    if (item)
    {
      vdec_error=vdec_queue( vdec_info.streamID,
                             item->pBuffer,
                             qtvdiag_vdecQueue_CbFn,
                             NULL);
      if (vdec_error==VDEC_ERR_EVERYTHING_FINE)
      {
        item->bQueued=true;
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: vdec_queue - unable to create input buffer");
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: vdec_queue - no media file loaded");
  }

  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_frame_buffer

DESCRIPTION
  call the vdec_reuse_frame_buffer function

DEPENDENCIES
  vdec_decode must have called back, providing us with a frame buffer.

RETURN VALUE
  the return status of vdec_reuse_frame_buffer.

SIDE EFFECTS
  None.

===========================================================================*/
static VDEC_ERROR qtvdiag_vdec_reuse_frame_buffer( vdec_diag_cmd_type * )
{
  VDEC_ERROR vdec_error;
  int32 diag_ret;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"VDEC_DIAG: Reuse Frame Buffer");
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"VDEC_DIAG: stream ID %d, pFrame 0x%x",vdec_info.streamID,vdec_info.pFrame);
  vdec_error=vdec_reuse_frame_buffer( vdec_info.streamID, vdec_info.pFrame );
  if (vdec_error == VDEC_ERR_EVERYTHING_FINE)
  {
    vdec_info.pFrame = NULL;
  }

  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_cleanup

DESCRIPTION
  clear out all internal varibles, freeing them if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  All internal variables are set to invalid (initial) values.

===========================================================================*/
VDEC_ERROR qtvdiag_vdec_cleanup( vdec_diag_cmd_type * )
{
  fs_rsp_msg_type fs_rsp;
  InputQElem *pElem;
  VDEC_ERROR vdec_error=VDEC_ERR_EVERYTHING_FINE;
  int32 diag_ret;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Clearing stored variables.");
  /* only do this if we've been initialized */
  if (bInitialized)
  {
    /* clear out our variables, freeing them if they exist. */
    frameCount=0;
    if (vdec_info.ppBlob)
    {
      (void)qtvdiag_vdec_free_blob(NULL);
      vdec_info.ppBlob=NULL;
    }
    vdec_info.audio = VDEC_CONCURRENT_NONE;

    /* clear out the queued items, if there are any */
    while ((pElem=(InputQElem *)q_get(&inputQ))!=NULL)
    {
      for (int i=0;i<vdec_info.layers;i++)
      {
        QTV_Free(pElem->pBuffer->buffer[i]);
      }
      QTV_Free(pElem->pBuffer);
      QTV_Free(pElem);
    }
    vdec_info.layers=0;
    if (vdec_info.pFrame)
    {
      (void)qtvdiag_vdec_reuse_frame_buffer(NULL);
      vdec_info.pFrame=NULL;
    }

    if (vdec_info.streamID)
    {
      (void)qtvdiag_vdec_destroy(NULL);
      vdec_info.streamID=NULL;
    }

    if (pMedia)
    {
      QTV_Delete(pMedia);
      pMedia=NULL;
    }

    if (filedes>=0)
    {
      efs_close(filedes);
      filedes=0;
    }
  }

  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

class VideoTrackSelectionPolicy : public ITrackSelectionPolicy
{
  IMPLEMENT_IREFERENCECOUNTABLE_MT(VideoTrackSelectionPolicy)

public:

  VideoTrackSelectionPolicy() { }
  virtual ~VideoTrackSelectionPolicy() { }  

  virtual void SelectTracks(ITrackList *trackList /* in */, bool reselect)
  {
    QTV_USE_ARG1(reselect);

    int selectedVideoTrack = -1;
  
    QTV_NULL_PTR_CHECK(trackList, RETURN_VOID);
  
    int32 numTracks = trackList->GetNumTracksAvailable();

    bool hasBaseMpeg4 = false;
  
    // Look for MPEG-4 base track.
    for (int32 i = 0; i < numTracks; i++)
    {
      if (!trackList->IsPlayable(i))
      {
        continue;
      }

      Media::CodecType codecType = trackList->GetCodecType(i);

      //If an Mpeg4 base track. (Still image IS mpeg4!)
      if (((codecType == Media::MPEG4_CODEC) &&
           (trackList->GetDependsOnID(i) == -1)) ||
          (codecType == Media::STILL_IMAGE_CODEC))
      {
        // Set base mpeg4 track flag. 
        // CHECKME: Should we save the track index and use it to compare with the
        // dependsID of enhancement tracks?
        hasBaseMpeg4 = true;
        //Select track.
        if (trackList->SelectTrack(i))
        {          
          selectedVideoTrack = i;
        }
      }
    }
    
    //Mark tracks to play based on the following:
    //first MPEG4 track cannot be an enhanced track,
    //all MPEG4 tracks that depend on the first MPEG4 track.
    for (int32 i = 0; i < numTracks; i++)
    {
      //Check whether player can play this track.
      if (!trackList->IsPlayable(i))
      {
        continue;
      }

      Media::CodecType codecType = trackList->GetCodecType(i);

      //Video selection block.
      //If base Mpeg4 track has been selected, select all tracks that depend on it.
      if (hasBaseMpeg4)
      {
        //If this is an enhancement track for the base track, select it.
        if ((codecType == Media::MPEG4_CODEC) &&
            (trackList->GetDependsOnID(i) != -1))
        {
  // This is messing up video timestamps when streaming Enhanced Layer Clips
  // Don't select the enhancement layer(s) for QTV MSM 6050. Base Layer Video
  // Only.
  #ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
          //Select enhancement track.
          if (trackList->SelectTrack(i))
          {          
            selectedVideoTrack = i;
          }
  #else
          //Don't Select enhancement track.
          trackList->SelectTrack(i, false);
  #endif  /* FEATURE_MP4_TEMPORAL_SCALABILITY */
        }
      }
      else
      {      
        if (((selectedVideoTrack < 0) &&
            (codecType == Media::H263_CODEC))||(codecType == Media::STILL_IMAGE_H263_CODEC))
        {
          //Select track.
          if (trackList->SelectTrack(i))
          {          
            selectedVideoTrack = i;
          }
        }
  #ifdef FEATURE_H264_DECODER
        else if (codecType == Media::H264_CODEC)
        {
          //Select track.
          if (trackList->SelectTrack(i))
          {          
            selectedVideoTrack = i;
          }
        }
  #endif /* FEATURE_H264_DECODER */
  #ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
  #endif /* FEATURE_QTV_OSCAR_DECODER */
  #ifdef FEATURE_QTV_SKT_MOD
        else if ((selectedVideoTrack < 0) && 
                 (codecType == Media::JPEG_CODEC))
        {
          //Select track.
          if (trackList->SelectTrack(i))
          {          
            selectedVideoTrack = i;        
          }
        }
  #endif /* FEATURE_QTV_SKT_MOD */
  #ifdef FEATURE_QTV_WINDOWS_MEDIA
        else if ( (selectedVideoTrack < 0) && 
                  ((codecType == Media::WMV1_CODEC) ||
                   (codecType == Media::WMV2_CODEC) ||
                   (codecType == Media::WMV3_CODEC)) )
        {
          //Select track.
          if (trackList->SelectTrack(i))
          {          
            selectedVideoTrack = i;
          }
        }
  #endif /* FEATURE_QTV_WINDOWS_MEDIA */
      } // if (hasBaseMpeg4)
  
  #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      // Always select timed text track if present
      if (codecType == Media::TIMED_TEXT_CODEC)
      {
        (void) trackList->SelectTrack(i);
      }
  #endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
    } //track loop     
  }

  virtual bool Notify(int eventType, void *value) 
  { 
    QTV_USE_ARG1(eventType);
    QTV_USE_ARG1(value);
    return false; 
  }
};


/*===========================================================================

FUNCTION      DescribeAndSelectTracks

DESCRIPTION
  Check and select tracks that we can play.
DEPENDENCIES
  None.

RETURN VALUE
  A boolean, specifying whether or not an error occured.

SIDE EFFECTS
  None.

ORIGINAL COPY/PASTE:
  From DescribeAndSelectTracks - Mpeg4Player.cpp
  Sections removed involving audio track selection.
===========================================================================*/
static bool DescribeAndSelectTracks(Media * p, bool &bAudioSelected, 
                                    bool &bVideoSelected)
{
  bool bError = false;
  bAudioSelected = false;
  bVideoSelected = false;

  ReferenceCountedPointer<ITrackList> trackList;

  //Get track information.
  if (p != NULL)
  {
    VideoTrackSelectionPolicy *videoTrackSelectionPolicy = 
      QTV_New_Args(VideoTrackSelectionPolicy, ());

    (void) p->SetTrackSelectionPolicy(videoTrackSelectionPolicy);

    if (!p->GetSelectedTrackList(MakeOutPointer(trackList)) || 
        (trackList == NULL) ||
        p->CheckError(Common::DESCRIBE))
    {
      bError = true;
    }
  }
  else
  {
    bError = true;
  }

  if (!bError)
  {
    // Determine if any video tracks were chosen.
    for (int32 i = 0; i < trackList->GetNumTracksAvailable(); i++)
    {
      if (trackList->IsTrackSelected(i))
      {
        if (Media::IsVideoCodec(trackList->GetCodecType(i)))
        {
          bVideoSelected = true;
        } // if (isVideoCodec)
      } // if (track selected)
    } // for (int i = 0; i < numTracks; i++)
  }

  if (bError)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "DescribeAndSelectTracks failed");
  }

  return !bError;
}

/*===========================================================================

FUNCTION      qtvdiag_vdec_init

DESCRIPTION
  primes the internal variables for use of the passed video file.
  appropriate video tracks are selected for use, but no audio tracks are
  ever selected.
  a decode output file is created, if opening of the video file was
  successful and the user requires decoded frames to be saved.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
static VDEC_ERROR qtvdiag_vdec_diag_init( vdec_diag_cmd_type *cmd_ptr )
{
  char filename[QTV_MAX_URN_BYTES+1];
  char outputFile[QTV_MAX_URN_BYTES+11];
  bool audio=false;
  bool video=false;
  VDEC_ERROR vdec_error=VDEC_ERR_EVERYTHING_FINE;
  int32 diag_ret;

  /* initialize internal variables */
  bInitialized = TRUE;
  memset(&vdec_info,0,sizeof(vdec_info_struct));
  (void)q_init(&inputQ);

  /* get recording preference */
  memcpy((void *)&recInfo,(void*)&cmd_ptr->rec_info,sizeof(recInfo)); /*lint!e420 */ 

  /* get filename of clip to play */
  (void)std_strlcpy(filename,(char*)cmd_ptr->buffer,cmd_ptr->bufsize);
  QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Setting parse buffer: %s",filename);

  pMedia = Media::OpenFile(filename,(Mpeg4Player *)NULL,true,false);
  if (pMedia)
  {
    /* describe */
    (void)DescribeAndSelectTracks(pMedia,audio,video);

    vdec_info.layers=pMedia->GetNumberOfVideoTracks();
    //vdec_info.audio = ConvertMediaAudioToVdec(pMedia->GetAudioCodecType());
    switch(pMedia->GetAudioCodecType())
    {
    case Media::UNKNOWN_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_NONE;
    break;
    case Media::EVRC_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_EVRC;
    break;
    case Media::QCELP_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_QCELP;
    break;
    case Media::AAC_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_AAC;
    break;
    case Media::BSAC_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_BSAC;
    break;
    case Media::GSM_AMR_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_AMR;
    break;
    case Media::MP3_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_MP3;
    break;
    case Media::WMA_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_NONE; //Right now, no WMA support. This will change soon.
    break;
    case Media::CONC_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_CONC;
    break;
    case Media::AMR_WB_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_AMR_WB;
    break;
    case Media::AMR_WB_PLUS_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_AMR_WB_PLUS;
    break;
    case Media::EVRC_NB_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_EVRC;
    break;
    case Media::EVRC_WB_CODEC:
      vdec_info.audio = VDEC_CONCURRENT_AUDIO_EVRC;
    break;
    default:
      vdec_info.audio = VDEC_CONCURRENT_NONE;
    break;
    }

    /* see if they wanted to save the decoded frame and act accordingly */
    if (recInfo.start != 0 || recInfo.stop != 0)
    {
      char *temp;
      /* open output file at this time */
      (void)std_strlcpy(outputFile,filename,QTV_MAX_URN_BYTES+11);
      temp = std_strchr(outputFile,'.');
      *temp = '\0';
      (void)std_strlcat(outputFile,".dec", QTV_MAX_URN_BYTES+11);
      filedes = efs_open(outputFile,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
      if (filedes < 0)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Output file create failure");
      }
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"File open successful");
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"File open failure");
  }

  /* event report to diag */
  diag_ret = (uint32)vdec_error;
  event_report_payload(EVENT_QTV_VDEC_DIAG_STATUS,
                       sizeof(uint32),&diag_ret);

  return vdec_error;
}

/*===========================================================================

FUNCTION QTVDIAG_HANDLE_VDEC_API_CMD

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to VDEC_API. This function then looks further into
  the command to see which specific VDEC APIs is to be tested. Results in the
  direct calling of VDEC APIs.

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None
===========================================================================*/
PACKED void * qtvdiag_handle_vdec_api_cmd(
                                         PACKED void * req_pkt_ptr,
                                         word) /*param*/
{
  // char *packet = (char *) req_pkt_ptr;

  // Get the API id (same as subsystem command code)
  qtvdiag_command_e_type api_id = (qtvdiag_command_e_type) 
                                  diagpkt_subsys_get_cmd_code(req_pkt_ptr);  

  // Skip the diag packet header.
  // packet += sizeof(diagpkt_subsys_header_type);

  qtvdiag_cmd_error_code_e_type diag_error_code = 
	QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
  // QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;

  // qtvdiag_cmd_rsp_type *rsp_ptr;
  vdec_diag_cmd_type *cmd_ptr = (vdec_diag_cmd_type *) req_pkt_ptr;
  // qtvdiag_command_e_type api_id = (qtvdiag_command_e_type) cmd_ptr->api_id; 
  VDEC_ERROR retval=VDEC_ERR_MYSTERY_ERROR; 
  int i;

  /*  A response pointer */
  /* rsp_ptr = (qtvdiag_cmd_rsp_type *)
            diagpkt_subsys_alloc( DIAG_SUBSYS_QTV,
                                  QTV_DIAG_VDEC_API_CMD,
                                  sizeof(qtvdiag_cmd_rsp_type) );
  ASSERT(rsp_ptr); */

  /* search the function table */
  for (i=0;(uint32)i<ARRAY_SIZE(handler_table);i++)
  {
    if (api_id==handler_table[i].api_id)
    {
      /* found it, get out! */
      break;
    }
  }
  if (i<(int)ARRAY_SIZE(handler_table))
  {
    /* found an entry, process it */
    QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"%s",handler_table[i].diag_msg);
    retval=handler_table[i].handler(cmd_ptr);

    /* let QXDM know what the status was */
    if (retval < sizeof(errors)/40)
    {
      QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Return value: %s",&(errors[retval][0]));
    }
  }
  else
  {
    /* unknown command */
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Unknown API command %d", api_id);
  }

  // Acknowledge command with a packet.
  qtvdiag_command_response_s_type *response_ptr =
    qtvdiag_alloc_response_packet((uint16) api_id);


  if (NULL == response_ptr)
  {
    return NULL;
  }

  response_ptr->diag_error_code = (uint8) diag_error_code;
  response_ptr->qtv_return_code = (uint32) retval;

  diagpkt_commit(response_ptr);

  return(PACKED void *) response_ptr;
}

/*===========================================================================

FUNCTION get_stream_header

DESCRIPTION
  Function retrieves the stream header.

DEPENDENCIES
  None.

RETURN VALUE
  Media::MediaStatus 

SIDE EFFECTS
  None
===========================================================================*/
Media::MediaStatus get_stream_header(VDEC_INPUT_BUFFER *pBuffer)
{
  Media::MediaStatus retval=Media::DATA_OK;
  Media::CodecType videoCodec = pMedia->GetVideoCodecType();

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"VDEC_DIAG: get_stream_header");

  switch (videoCodec)
  {
    case Media::H264_CODEC: 
      retval = get_h264_stream_header(pBuffer);
      break;

    case Media::H263_CODEC:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: No Header for H263");
      retval=Media::DATA_NULLFILL;
      break;

    case Media::MPEG4_CODEC:
      retval = get_mp4_stream_header(pBuffer);
      break;

    case Media::WMV3_CODEC:
    case Media::WMV2_CODEC:
    case Media::WMV1_CODEC:
      retval = get_wmv_stream_header(pBuffer);
      break;

    default:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Error- Unknown Codec type");
      retval = Media::DATA_ERROR;

  }

  if (retval != Media::DATA_OK)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"VDEC_DIAG: get_h264_stream_header failed!!");
  }
  return retval;
}

/*===========================================================================

FUNCTION get_h264_stream_header

DESCRIPTION
  Function retrieves the H264 stream header.

DEPENDENCIES
  None.

RETURN VALUE
  Media::MediaStatus 

SIDE EFFECTS
  None
===========================================================================*/
Media::MediaStatus get_h264_stream_header(VDEC_INPUT_BUFFER *input)
{
  unsigned int nBytes=0;
  Media::MediaStatus status = Media::DATA_ERROR;
  uint8 *buf_pos = NULL;

  QTV_USE_ARG2(status, buf_pos);

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"VDEC_DIAG: get_h264_stream_header");

  for (int layer =0; layer < vdec_info.layers; layer++)
  {
    input->buffer_size[layer] = 0;
    status = Media::DATA_OK;
#ifdef FEATURE_H264_DECODER
    // reset parameter set index
    (void)pMedia->ResetParamSetNAL(layer);

    /* allocate space for layer buffer */
    if (!input->buffer[layer])
    {
      input->buffer[layer] = (uint8 *)QTV_Malloc(videoMaxSize[layer]);
    }

    if (input->buffer[layer])
    {
      nBytes = pMedia->GetNextParamSetNAL( status,
                                           layer,
                                           input->buffer[layer],
                                           videoMaxSize[layer] );      
    }
#endif
    if (status != Media::DATA_OK)
    {
      //If there was an error getting the param set,
      //  or if we have all of them
      break;
    }

    buf_pos = input->buffer[layer];

    if (!pMedia->CheckError(Common::GETVIDEOTIME) && nBytes > 0)
    {
      input->buffer_size[layer] = nBytes;
      input->layers = vdec_info.layers;
      //We need to prepend NAL length if it is not there so the decoder
      // can separate pic and seq headers
      //Make sure that we have enough memory to Prepend the NAL Length.
 #ifdef FEATURE_H264_DECODER
      ASSERT( (uint32)input->buffer_size[layer] + H264_NAL_LENGTH_SIZE <
              videoMaxSize[layer] );

      (void)PrependNalLen(buf_pos, nBytes,H264_NAL_LENGTH_SIZE);
      input->buffer_size[layer] += H264_NAL_LENGTH_SIZE;
      buf_pos += H264_NAL_LENGTH_SIZE;
#endif
    }
  }//for

  return status;
}

#ifdef FEATURE_H264_DECODER
/*===========================================================================

FUNCTION PrependNalLen

DESCRIPTION
  Function prepends the NAL len before NAL data.

DEPENDENCIES
  None.

RETURN VALUE
  Media::MediaStatus 

SIDE EFFECTS
  None
===========================================================================*/
bool PrependNalLen
(
  uint8 * buf, uint32 size, uint32 sizeOfNalLengthField
)
{
  uint8* psize = (uint8*) &size;
  //Need to prepend a 4 byte integer to the bitstream signifying the size
  (void)memmove(buf + sizeOfNalLengthField, buf, size);
  //ARM is big-endian, and decoder expects size in little-endian
  for (int i = 0; i < sizeOfNalLengthField; i++)
  {
    buf[i] = psize[ sizeOfNalLengthField - 1 - i ];
  }
  return true;
}
#endif

/*===========================================================================

FUNCTION get_mp4_stream_header

DESCRIPTION
  Function retrieves the MPEG4 stream header.

DEPENDENCIES
  None.

RETURN VALUE
  Media::MediaStatus 

SIDE EFFECTS
  None
===========================================================================*/
Media::MediaStatus get_mp4_stream_header(VDEC_INPUT_BUFFER *input)
{
  unsigned char *pvolData = NULL;
  Media::MediaStatus status = Media::DATA_OK;
//  unsigned int nBytes=0;
  int volSize =0;
//  QTV_USE_ARG1(nBytes);
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"VDEC_DIAG: get_mp4_stream_header");

  for (int layer =0; layer < vdec_info.layers; layer++)
  {
    pvolData = pMedia->GetVOLHeader(layer,(int &)volSize);
    if (pvolData==NULL)
    {
      status = Media::DATA_NULLFILL;
      break;
    }

    if (volSize > videoMaxSize[layer])
    {
      status = Media::DATA_ERROR;
      break;
    }

    /* allocate space for layer buffer */
    if (!input->buffer[layer])
    {
      input->buffer[layer] = (uint8 *)QTV_Malloc(videoMaxSize[layer]);
    }

    if (input->buffer[layer])
    {
      memcpy(input->buffer[layer],pvolData,volSize);
      input->buffer_size[layer] = volSize;
    }
  }
  return status;
}

/*===========================================================================

FUNCTION get_wmv_stream_header

DESCRIPTION
  Function retrieves the WMV stream header.

DEPENDENCIES
  None.

RETURN VALUE
  Media::MediaStatus 

SIDE EFFECTS
  None
===========================================================================*/
Media::MediaStatus get_wmv_stream_header(VDEC_INPUT_BUFFER *input)
{
  Media::MediaStatus status = Media::DATA_OK;
  unsigned char *pvolData = NULL;
  int size = 0;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"VDEC_DIAG: get_wmv_stream_header");

  for (int idx = 0; idx < vdec_info.layers; ++idx)
  {
    pvolData = pMedia->GetVOLHeader( idx, size );
    if (pvolData==NULL)
    {
      status = Media::DATA_NULLFILL;
      break;
    }

    if (size > videoMaxSize[idx])
    {
      status = Media::DATA_ERROR;
      break;
    }

    /* allocate space for layer buffer */
    if (!input->buffer[idx])
    {
      input->buffer[idx] = (uint8 *)QTV_Malloc(videoMaxSize[idx]);
    }

    if (input->buffer[idx])
    {
      memcpy(input->buffer[idx],pvolData,size);
      input->buffer_size[idx] = size;
    }
  }
  return status;
}

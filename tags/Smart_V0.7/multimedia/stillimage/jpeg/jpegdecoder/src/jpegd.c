/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
                           D E C O D E R

GENERAL DESCRIPTION
  This module contains functions to decode Joint Photographers  
  Expert Group (JPEG) image files for MSM6550, 6275 and 6800.

EXTERNALIZED FUNCTIONS
  jpegd_dsp_dec_rev2
    This function is called to invoke the DSP portion of the decoding
 
  jpegd_dsp_callback
    This function performs all callback from DSP
  
  jpegd_process_callback
    This function is called from the processing task to handle the 
    necessary operations.

  jpegd_abort_decode
    This function is called to end the current JPEG decoder session 


INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2002-2004 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegdecoder/main/latest/src/jpegd.c#13 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
21/04/09   kdiv    Added Error fatal if DSP output return address doesn't match with actual address.
23/03/09   kdiv    Make the common release decoder process for ARM and DSP decoder in a common function
01/12/09   kdiv    Resetting flag jpeg_decode_abort shouldn't be done in jpegd_dsp_dec_rev2(), 
                   which may be set in UI task to abort JPEGD.
09/17/08   kdiv    ULC Write back cache changes - ensure buffers shared to DSP are 32byte aligned
09/02/08   kdiv    ULC Write back cache changes
04/15/08   sigu    Integrated JPEGD_USES_DME_LITE for ULC target.
02/19/08   kdiv    Fixed issues with YCbCr Jpeg decoder o/p with odd dimension images
12/18/07   kdiv    Added support for YCbCr Jpegd DSP output and YCrCb final output
11/23/07   saur    Fixed critical lint warning
11/08/07   hap     Fixing RVCT2.2 compiler warnings
08/21/07   arv     Fixed a minor bug related to 
07/09/07   arv     Added scalable jpeg decoder within FEATURE_JPEGD_RESIZE
02/13/07   arv     Fixed the issue related to half buffer not getting filled
                   when the bitstream occurs at the MAX_DATA_BYTES boundary
11/28/06   vma     Fixed compilation error when resize feature is not defined
11/28/06   vma     Return failure in jpegd_init if destination pointer is NULL
11/12/06   arv     Added resize feature for JPEG Decoder
10/06/06   arv     Freeing the memory resource in case of decoder failing during
                   initialization or setting up of decoder parameters.
                   Added the modified ipl color conversion to match the JPEG standard
09/25/06   arv     Fixed the bug related to JPEG Decoder not waiting for IDLE 
                   Complete Message.
                   Fixed a memory leak related to input data buffer in case of 
                   cancelling the decoding session.
07/20/06   arv     Added clkrgm deregistering at the end of decoding session
07/11/06   arv     Added jpeg_cancel_decode function to make sure that DSP decoder
                   goes to IDLE state when the app cancels the decoding session
                   Fixes the graph overflow buffer issue
04/18/06   arv     Modified error codes in accordance with the firmware changes
04/06/06   arv     Use the ipl_convert_image function to avoid using deprecated API
03/23/06   vma     Replaced all mallocs and frees to JPEG_MALLOC's and JPEG_FREE's
03/17/05   dl      Use cache or cache lines based on supported cache.
03/13/06   vma     Fixed all Lint errors
11/04/05   arv     Free of all Lint errors.
11/03/05   arv     Fixed the Lint errors
10/12/05   arv     Modified the code in accordance with the review feedback
09/30/05   arv     Made changes to jpeg_decode to avoid memory leak during jpeg_decode_abort and 
                   at the same time starting another decoding session
09/16/05   arv     Fixed the Lint errors
08/19/05   dl      Continue decoder until decode done register is true.
08/12/05   arv     Modified the error codes aligning with the DSP changes
05/05/05   arv     Made changes to mmu_invalidate_data_cache in such a way
                   that it doesn't invalidate cache globally
04/02/05   arv     Fixed the issue of not clearing up the jpegd error codes.
                   Fixed the issue with the calculation of height for H2V1 images
03/24/05   arv     Issue JPEGD_OUTPUT_CONSUMED command only before the completion of DECODE_DONE
03/22/05   arv     Featurizing the JPEG Decoder - FEATURE_JPEG_DECODER_REV2
03/15/05   arv     Added YCbCr422Lp and YCbCr420Lp  output for DSP based decoder
02/17/05   arv     Added decoder error messages. Fixed the issue where the 
                   CMD_JPEGD_IDLE gets called infinitely.
02/03/05   arv     Fixed the restart marker issue for larger MCU size
12/17/04   arv     Fixed the issue where number of rows to copy at bottom of image 
                   exceeds 16
11/30/04   pdo     Added support for H2V1 format and free the decoder input buffer.
11/23/04   pdo     Supported X16 image in H2V2 format.
10/22/04   pdo     Initial version

===========================================================================*/
//#define JPEG_TIMING_TEST
//#define FEATURE_JPEGD_RESIZE

/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/


#include "jpegd.h"
#include "jpeg_malloc.h"
#include "err.h"
#define jpeg_file_id MALLOC_JPEGD

#ifdef FEATURE_JPEG_DECODER_REV2

#include "jpegi.h"
#include "stdlib.h"
#include "msg.h"                /* Message logging/reporting services      */
#include "graph.h"              /* Graphics task typedefs and prototype    */
#include "ipl.h"
#include "jpegqdsp.h"
#include "clkregim.h"   
#include "timetick.h"

#ifdef JPEG_TIMING_TEST
#include "timetest.h"
#endif


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros, types, variables
and other items needed by this module.

#ifdef FEATURE_JPEG_DECODER
=============================================================================*/ 

/* Enum for the buffer holders */

typedef enum
{
  BUFFER_1,              /* Use buffer1 to hold DSP info   */
  BUFFER_2               /* Use buffer2 to hold DSP info   */
} jpegd_bufferType;


static  uint8                   *inputBitStreamBuffer[2];
static  uint8                   *decodedLumaBuffer[2];
static  uint8                   *decodedChromaBuffer[2];

#ifdef CACHE_POLICY_WRITE_BACK
static  uint8                   *inputBitStreamBuffer_malloc[2];
static  uint8                   *decodedLumaBuffer_malloc[2];
static  uint8                   *decodedChromaBuffer_malloc[2];
#endif //CACHE_POLICY_WRITE_BACK

static  uint8                   *jpegdDestBufPtr;
static  uint8                   *jpegdCurDestPtr;
static  uint8                   *jpegdChromaCurDestPtr;

#ifdef FEATURE_JPEGD_RESIZE
static  uint16                   jpegd_resizeCol = 0;      /* Resize Height */
static  uint16                   jpegd_resizeColCum = 0;   /* Resize Height cumulative */

static  uint16                   jpegd_scaledWidth = 0;      /* Scaled Width */
static  uint16                   jpegd_scaledHeight = 0;     /* Scaled Height */
static  uint16                   jpegd_dspScaledWidth = 0;      /* DSP Scaled Width */


static uint16                    jpegd_localImageWidthEven;

static  uint16                   jpegd_dspLocalImageWidth = 0;     

static  uint16                   jpegd_scalingFactor = 0;    /* Scaling Factor */
static  uint16                   jpegd_dspScalingFactor = 0;    /* Scaling Factor sent to DSP */
#endif

static uint16                    jpegd_localImageWidth;
static uint16                    jpegd_localImageHeight;



static  uint32                   outputLumaSize;
static  uint32                   endofBitStream;
static  uint32                   jpeg_srcFile_pos;
static  uint32                   jpegd_pixRow;
static  boolean                  jpegd_error = FALSE;

static  int32                    jpegd_session_id = 0;
static  uint32                   jpeg_client_id;                /* Current client ID */
static  void                    *handle;

        boolean                  jpeg_decoding = FALSE;
        boolean                  jpeg_decode_abort  = FALSE;
static  boolean                  jpeg_decode_failed = FALSE;
static  boolean                  jpegd_in_shutdown_process = FALSE;
static  boolean                  isJPEGDAbort = FALSE;
static  boolean                  jpegd_armResizeFlag = FALSE;
static  boolean                  jpegd_dspResizeFlag = FALSE;

#ifdef FEATURE_JPEGD_RESIZE
/* Flag to check if JPEG Image can be resizable based on aspect ratio calculation */
static  boolean                  isResizable = FALSE;       
#endif

static  graph_context_req_type   jpegd_context_data;
static  jpegd_bufferType         last_outBuffer_holder,last_inBuffer_holder;
        JPEGDEC_decodeSpecType   localDecodeInfo;   /* decode specification   */

static  JPEGDEC_bufferInfoType   localJpegBufInfo;
static  JPEGD_decodeCmdType      decodeCmdData;     
static  jpeg_cmx_cb_func_type    jpegdClientCB_func = NULL;
static  jpeg_cmx_buf_req_type    jpegd_get_buf;
static  jpeg_cmx_get_data_type   jpegd_get_data;    /* JPEG get data          */
static  JPEGD_outputMsgBufType   jpegd_buffer1, jpegd_buffer2;
static  JPEGDEC_formatType       jpegd_inFormat;
static  ipl_image_type           jpegdIPLIn, jpegdIPLOut;
static  JPEGQDSPReturnCodeType   jpegQdspReturnStatus;
static  JPEGDEC_timing           jpegdTiming;

/* Forward declaration for the required LOCAL functions */
static  void                     jpegd_decoder_terminate(void);
static  void                     jpegd_process_callback(int32 clientId, int32 clientData);
static  void                     jpegd_handle_dsp_CB(JPEGQDSPMsgType  *pMsgInfo);
static  void                     jpegd_send_idle_cmd (void);

extern jpeg_cmx_spec_type        jpeg_spec;
#ifdef JPEGD_USES_DME_LITE
/*Needed for JPEGD_USES_DME_LITE*/
/*This flag will be used to check whether padding (to make it a*/
/*multiple of 4 after using a JPEGD scaling factor of 8) is done.*/
static boolean                   jpegd_lite_adjusted =FALSE;
/*How many bytes padded to make it a multiple of 4*/
static uint8                     jpegd_lite_pad_value = 0;
#endif /*JPEGD_USES_DME_LITE*/



/*===========================================================================

Function: jpegd_dsp_dec_rev2

Description: This function will decode the JPEG bitstream.
             Upon completion of the decoding, a callback
               will be invoked to notify the caller the status of the decoding.

Return Value:
   JPEGDEC_SUCCESS              cmd sucessful
   JPEGDEC_INVALID_CB           cmd contains a NULL cb pointer
   JPEGDEC_INVALID_STATE        cmd issued during invalid state
   
Comments: None

Side Effects: 
    Static variables modified - 
     jpegd_session_id
     jpeg_decoding
     jpeg_decode_abort
     jpeg_decode_failed 
     jpeg_client_id
     localDecodeInfo
     jpegQdspReturnStatus

==============================================================================*/
JPEGDECReturnCodeType   jpegd_dsp_dec_rev2
(
 uint32                   clientId, 
 const JPEGDEC_decodeSpecType  *decodeInfo,   /* decode specification   */
 const JPEGDEC_bufferInfoType  *bufInfo
 )
{
  uint16 cnt;
  JPEGDECReturnCodeType  rntStatus= JPEGDEC_SUCCESS;
  
  MSG_HIGH("JPEGDSPDEC: About to start DSP decoder", 0, 0, 0);

  if (jpeg_decoding)
  {
    MSG_HIGH("JPEGDSPDEC: DECODER is in progress - new decoding not accepted",0 ,0, 0);
    return JPEGDEC_BUSY_STATE;
  }
  
  if (!decodeInfo)
  {
    MSG_ERROR("JPEGDSPDEC: Bad DECODER input parameters detected", 0, 0, 0);
    return JPEGDEC_INVALID_PTR;
  }
  
  /* check for all the essential pass-in data */
  for (cnt = 0; cnt < 4; cnt++)
  {
    if (!(decodeInfo->h_codeCount[cnt]))
    {
      MSG_ERROR("JPEGDSPDEC: Bad Huffman table detected", 0, 0, 0);
      return JPEGDEC_BAD_DATA;
    }
  }
  
  if (!(decodeInfo->q_table[0]) || (!(decodeInfo->q_table[1]))||
    !(decodeInfo->h_dcCodeValue[0]) || (!(decodeInfo->h_dcCodeValue[1]))||
    !(decodeInfo->h_acCodeValue[0]) || (!(decodeInfo->h_acCodeValue[1])) )
  {
    MSG_ERROR("JPEGDSPDEC: Bad Quant or Huffman table detected", 0, 0, 0);
    return JPEGDEC_BAD_DATA;
  }
  
  /* Check for image width and image height */
  
  if ((!decodeInfo->callBack) || (decodeInfo->width > JPGD_MAX_WIDTH)
    || (!bufInfo) 
    || (decodeInfo->bytes_read > decodeInfo->filesize) )
  {
    MSG_ERROR("JPEGDSPDEC: Bad decode parameters detected", 0, 0, 0);
    return JPEGDEC_BAD_DATA;
  }

  if(jpeg_decode_abort == TRUE)	//if the abort flag is set return error else proceed
  {
    MSG_HIGH("JPEGDSPDEC: DSP decoder is not started due to Abort request from OEM", 0, 0, 0);
    jpeg_decode_abort = FALSE;
    return JPEGDEC_BUSY_STATE;
  }
  
  jpegd_session_id++;
  jpeg_decoding          = TRUE;
  jpeg_decode_failed     = FALSE;
  isJPEGDAbort           = FALSE;
  jpeg_client_id         = clientId;
  
  /* Copy the client data for the async. processing */
  localDecodeInfo    = *decodeInfo;
  localJpegBufInfo   = *bufInfo;
  jpegdClientCB_func = localDecodeInfo.callBack;
  handle = localDecodeInfo.client_handle;
  jpegd_inFormat       = localDecodeInfo.subsample;
  jpegd_pixRow       = 0;
  jpeg_srcFile_pos   = localDecodeInfo.bytes_read;
  
  jpegd_localImageWidth = localDecodeInfo.width;
  jpegd_localImageHeight  = localDecodeInfo.height;

  /* Download the necessary JPEGD DSP image */
  jpegQdspReturnStatus = jpeg_qdsp_initialize(jpegd_handle_dsp_CB, JPEG_DECODER_DSP);
  
  if (jpegQdspReturnStatus != JPEGQDSP_SUCCESS)
  {
    MSG_ERROR ("JPEGDSPDEC: Invalid qdsp initialization", 0,0,0);
    return JPEGDEC_BAD_DATA;
  }
  
  return rntStatus;
}

/*===========================================================================

Function: JPEGD_HANDLE_DSP_CB

Description: This function handle all the callbacks from the decoder 
             portion of the JPEGQDSP.
             Based on the message from the DSP, it will queue up a
             corresponding action to the processing task.

Return Value:
   JPEGDEC_SUCCESS              cmd sucessful
   JPEGDEC_INVALID_CB           cmd contains a NULL cb pointer
   JPEGDEC_INVALID_STATE        cmd issued during invalid state
   
Comments: None

Side Effects: 
    jpegd_buffer1
    jpegd_buffer2
    last_outBuffer_holder


==============================================================================*/
static void jpegd_handle_dsp_CB
(
 JPEGQDSPMsgType  *pMsgInfo             /* qdsp service info                */
)
{
  /* Base on the information from the DSP, set the DECODER state */
  if (pMsgInfo)
  {
    switch (pMsgInfo->Code)
    {
      case  JPEGD_IMAGE_READY:
      {
        /* DSP image is ready.   */
        jpegd_context_data.client_data = (int32)JPEGDEC_DECODE_IMG;
        break;
      }
      
      case  JPEGD_OUTPUT_PRODUCED:
      {
        if (last_outBuffer_holder == BUFFER_2)
        {
          /* Use BUFFER1 to hold DSP buffer info */
          jpegd_buffer1 = pMsgInfo->Msg.DecBuf;
          last_outBuffer_holder = BUFFER_1;
          jpegd_context_data.client_data = (int32)JPEGDEC_READ_BUFFER1;
        }
        else /* last_outBuffer_holder == BUFFER_1 */
        {
          /* Use BUFFER2 to hold DSP buffer info */
          jpegd_buffer2 = pMsgInfo->Msg.DecBuf;
          last_outBuffer_holder = BUFFER_2;
          jpegd_context_data.client_data = (int32)JPEGDEC_READ_BUFFER2;
        }
        jpegdTiming.opm_received = (uint32)timetick_get_ms();
        break;
      }
      
      case  JPEGD_INPUT_REQUEST:
      {
        if (last_inBuffer_holder == BUFFER_2)
        {
          /* Send BUFFER1 to DSP this time */
          last_inBuffer_holder = BUFFER_1;
          jpegd_context_data.client_data = (int32)JPEGDEC_SEND_BUFFER1;
        }
        else /* last_inBuffer_holder == BUFFER_1 */
        {
          /* Send BUFFER2 to DSP this time */
          last_inBuffer_holder = BUFFER_2;
          jpegd_context_data.client_data = (int32)JPEGDEC_SEND_BUFFER2;
        }
        break;
      }
      
      case  JPEGD_IDLE_COMPLETE:
      {
        jpegd_context_data.client_data =  (int32)JPEGDEC_IDLE;
        MSG_HIGH ("JPEGD: IDLE Complete Message",0,0,0);
        break;
      }
      
      case  JPEGD_ILLEGAL_CMD:
      {
        /* Not sure what we can recover from here other than log a msg */
        jpegd_context_data.client_data =  (int32)JPEGDEC_OPER_MAX;
        MSG_ERROR("JPEGDSPDEC: DSP rejected command", 0, 0, 0); 
        break;
      }
      
      default:
      {
        jpegd_context_data.client_data =  (int32)JPEGDEC_OPER_MAX;
        MSG_ERROR("JPEGDSPDEC: DSP is lost", 0, 0, 0); 
      }
      
    }
    /* Queue the encoding command on the GRAPH task */
    jpegd_context_data.function_ptr = jpegd_process_callback;
    jpegd_context_data.client_ID = jpegd_session_id;
    
    graph_context_req( jpegd_context_data );
  }
  
}  /*lint !e818: pMsgInfo could be declared as pointing to const but 
    * it would involve changing the interface to outside */

/*===========================================================================

Function: JPEGD_SETUP_DECODE_CMD

Description:  This function prepares the decoding information. Setups the
              necessary data for the DSP decoder.
              
Return Value: JPEGDECReturnCodeType
   
Comments: None

Side Effects:

============================================================================*/
JPEGDECReturnCodeType jpegd_setup_decode_cmd(const JPEGDEC_decodeSpecType *pInfo,JPEGD_decodeCmdType *pCmd )
{
  uint32 cnt;
  
  if (!pInfo || !pCmd)
  {
    MSG_HIGH("JPEGDSPDEC: Null pointer encounter", 0, 0, 0);
    return JPEGDEC_INVALID_PTR;
  }
  
  /* Assign the essential data for the decoder command */
  /* Width has to be x16 since the DSP decodes images of 
    subsampling format H2V2 (16 * 16) and H2V1 (16 * 8) */

  //cnt = ((pInfo->width + 15) / 16) * 16;
  cnt = ((jpegd_localImageWidth + 15) / 16) * 16;

  if( pInfo->subsample == JPEGDEC_H2V2)
  {
    pCmd->WidthHeight =(uint32) ((cnt << 16) | ((jpegd_localImageHeight + 15) / 16) * 16);
  }
  else
  {
    pCmd->WidthHeight =(uint32) ((cnt << 16) | ((jpegd_localImageHeight + 7) / 8) * 8);
  }
  
  pCmd->Subsample = (int) pInfo->subsample;
  
#ifdef FEATURE_JPEGD_RESIZE
  if(jpegd_dspResizeFlag)
    pCmd->Subsample = (uint32) ((jpegd_dspScalingFactor << 16) | (pInfo->subsample) );
#endif

  pCmd->Subsample = (uint32) (pCmd->Subsample  | (pInfo->chromaType << 2) );

  pCmd->Restart   = (uint32) pInfo->restart;
  
  /* Set the input bitstream buffer for the decoder */
  pCmd->Input.Ptr           = inputBitStreamBuffer[0];
  pCmd->Input.Size          = localJpegBufInfo.bufSize;
  pCmd->Input.End_BitStream = endofBitStream;

  JPEG_FLUSH_DATA_CACHE ((uint32*)inputBitStreamBuffer[0], localJpegBufInfo.bufSize);
  
  /* Get the output ping pong buffers for the decoder */
  for (cnt=0; cnt <2; cnt++)
  {
    pCmd->Out[cnt].LumaPtr   = decodedLumaBuffer[cnt];
    pCmd->Out[cnt].Size      = outputLumaSize;
    pCmd->Out[cnt].ChromaPtr = decodedChromaBuffer[cnt];
  }
  
  pCmd->DQT_table[JPEGDEC_LUMA_QUANTTABLE_INDEX] = (uint16 *)pInfo->q_table[JPEGDEC_LUMA_QUANTTABLE_INDEX];
  pCmd->DQT_table[JPEGDEC_CHROMA_QUANTTABLE_INDEX] = (uint16 *)pInfo->q_table[JPEGDEC_CHROMA_QUANTTABLE_INDEX];
  
  pCmd->h_codeCount[JPEGDEC_LUMA_DC_HUFFCODE_CNT_INDEX] = pInfo->h_codeCount[JPEGDEC_LUMA_DC_HUFFCODE_CNT_INDEX] ;
  pCmd->h_codeCount[JPEGDEC_CHROMA_DC_HUFFCODE_CNT_INDEX] = pInfo->h_codeCount[JPEGDEC_CHROMA_DC_HUFFCODE_CNT_INDEX];
  pCmd->h_codeCount[JPEGDEC_LUMA_AC_HUFFCODE_CNT_INDEX] = pInfo->h_codeCount[JPEGDEC_LUMA_AC_HUFFCODE_CNT_INDEX];
  pCmd->h_codeCount[JPEGDEC_CHROMA_AC_HUFFCODE_CNT_INDEX] = pInfo->h_codeCount[JPEGDEC_CHROMA_AC_HUFFCODE_CNT_INDEX];
  
  pCmd->h_dcCodeValue[JPEGDEC_LUMA_DC_HUFFCODE_VAL_INDEX] = pInfo->h_dcCodeValue[JPEGDEC_LUMA_DC_HUFFCODE_VAL_INDEX];
  pCmd->h_dcCodeValue[JPEGDEC_CHROMA_DC_HUFFCODE_VAL_INDEX] = pInfo->h_dcCodeValue[JPEGDEC_CHROMA_DC_HUFFCODE_VAL_INDEX];
  pCmd->h_acCodeValue[JPEGDEC_LUMA_AC_HUFFCODE_VAL_INDEX] = pInfo->h_acCodeValue[JPEGDEC_LUMA_AC_HUFFCODE_VAL_INDEX];
  pCmd->h_acCodeValue[JPEGDEC_CHROMA_AC_HUFFCODE_VAL_INDEX] = pInfo->h_acCodeValue[JPEGDEC_CHROMA_AC_HUFFCODE_VAL_INDEX];
  
  /* Initialize the bufer designation for buffer 1 is first one to be used */
  last_outBuffer_holder = BUFFER_2;
  last_inBuffer_holder  = BUFFER_1;

  return JPEGDEC_SUCCESS;
}

/*===========================================================================

Function: JPEGD_INIT

Description:  This function prepares the decoding information. Setups working
              input and output buffers memory.
              
Return Value: None
   
Comments: 

Side Effects:

============================================================================*/
boolean jpegd_init(JPEGDEC_bufferInfoType jpegBufInfo)
{
  uint32 bufferWidth, bufferHeight, cnt,byteRemain;
  boolean firstHalf;

  jpegd_error = FALSE;

#ifdef FEATURE_JPEGD_RESIZE
  jpegd_resizeCol    = 0;
  jpegd_resizeColCum = 0;
  jpegd_dspScalingFactor = 0;
#ifdef JPEGD_USES_DME_LITE
  /*Make sure that this flag is always turned off for every JPEG session*/
  jpegd_lite_adjusted = FALSE;
#endif /*JPEGD_USES_DME_LITE*/

#endif /*FEATURE_JPEGD_RESIZE*/


  /*  DSP is downloaded and ready for decoding */
  
  /* Allocate the input buffers */
  /* Small file size so one input buffer is sufficient  */
#ifdef CACHE_POLICY_WRITE_BACK
    inputBitStreamBuffer_malloc[0]   = JPEG_MALLOC(jpegBufInfo.bufSize+CACHE_LINE_SIZE);
    inputBitStreamBuffer[0]          = (uint8*)((((uint32)inputBitStreamBuffer_malloc[0]+CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
#else
    inputBitStreamBuffer[0] = JPEG_MALLOC(jpegBufInfo.bufSize);
#endif
  
  if (!inputBitStreamBuffer[0])
  {
    MSG_ERROR("JPEGDSPDEC: Out of memory, buffer 1", 0, 0, 0);
    return FALSE;
  }
  
  if (localDecodeInfo.filesize > jpegBufInfo.bufSize )
  {
    /* File size is larger than the working buffer,       */
    /* need ping pong input buffers                       */
#ifdef CACHE_POLICY_WRITE_BACK
    inputBitStreamBuffer_malloc[1]   = JPEG_MALLOC(jpegBufInfo.bufSize+CACHE_LINE_SIZE);
    inputBitStreamBuffer[1]          = (uint8*)((((uint32)inputBitStreamBuffer_malloc[1]+CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
#else
    inputBitStreamBuffer[1] = JPEG_MALLOC(jpegBufInfo.bufSize);
#endif

    
    if (!inputBitStreamBuffer[1])
    {
      MSG_ERROR("JPEGDSPDEC: Out of memory, buffer 2", 0, 0, 0);
      return FALSE;
    }
  }
  
  /* Fill in the 1st buffer */
  byteRemain = (uint32)(jpegBufInfo.currentPtr - jpegBufInfo.bufPtr);
  
  if (byteRemain >= (jpegBufInfo.bufSize / 2))
  {
    /* current pointer is in 2nd half */
    byteRemain = jpegBufInfo.bufSize - byteRemain;
    firstHalf = FALSE;
  }
  else
  {   
    /* current pointer is in 1st half */
    byteRemain = jpegBufInfo.bufSize/2 - byteRemain;
    firstHalf = TRUE;
  }
  
  /* byteRemain is the un-read bytes in 1/2 of the buffer */
  if (byteRemain < jpegBufInfo.dataSize) 
  {
    uint8 *curPtr;
    /* Copy the remainder in the half buffer */
    memcpy(inputBitStreamBuffer[0],jpegBufInfo.currentPtr,byteRemain);
    
    if (firstHalf)
    {
      curPtr = jpegBufInfo.bufPtr + jpegBufInfo.bufSize/2;
    }
    else
    {
      curPtr = jpegBufInfo.bufPtr;
    }
    
    /* copy the bit-stream in the second half */
    memcpy(inputBitStreamBuffer[0]+byteRemain,curPtr,jpegBufInfo.dataSize-byteRemain);
    
  }
  else
  {
    /* Copy the remainder in the half buffer */
    memcpy(inputBitStreamBuffer[0],jpegBufInfo.currentPtr,jpegBufInfo.dataSize);
  }
  
  if (localDecodeInfo.filesize > localDecodeInfo.bytes_read)
  {
    /* Set the starting point in the buffer */
    jpegd_get_data.buffer   = inputBitStreamBuffer[0]+jpegBufInfo.dataSize;
    jpegd_get_data.position = jpeg_srcFile_pos;
    jpegd_get_data.callback_ptr = NULL;
    jpegd_get_data.client_data  = 0;
    
    if ((jpegBufInfo.bufSize - jpegBufInfo.dataSize) <
      (localDecodeInfo.filesize - localDecodeInfo.bytes_read))
    {
      /* more data from the file, fill up the input bit-stream buffer */
      if (jpegdClientCB_func)
      {
        jpegd_get_data.num_bytes = jpegBufInfo.bufSize-jpegBufInfo.dataSize;
        /* update source file pointer */
        jpeg_srcFile_pos += jpegd_get_data.num_bytes;
        /* Get more bit-stream data into the input buffer */
        jpegdClientCB_func(JPEG_CMX_GET_DATA, handle, &jpegd_get_data);
      }
      else
      {
        MSG_ERROR("JPEGDSPDEC: Can't fill the buffer", 0, 0, 0);
      }
      endofBitStream = 0; /* Still more bitstream to come      */
    }
    else
    {
      /* more data from the file, fill up the input bit-stream buffer */
      if (jpegdClientCB_func)
      {
        jpegd_get_data.num_bytes = localDecodeInfo.filesize - jpeg_srcFile_pos;
        
        /* Get more bit-stream data into the input buffer */
        jpegdClientCB_func(JPEG_CMX_GET_DATA, handle, &jpegd_get_data);
      }
      
      endofBitStream = 1; /* This is the last buffer to DSP    */
    }
    
  }
  else
  {
    endofBitStream = 1; /* This is the last buffer to DSP    */
  }

  /* Setup the output ping pong buffer for the decoded data */
  bufferWidth = ((localDecodeInfo.width+15)/16)*16;
  bufferHeight= 24;  
  
  /* Flags to determine if resize is done by 
  ARM or DSP or combination of ARM and DSP */
  
  jpegd_armResizeFlag = FALSE;
  jpegd_dspResizeFlag = FALSE;


#ifdef FEATURE_JPEGD_RESIZE
  if(localDecodeInfo.jpegdResizeFlag)
  {
    isResizable = TRUE;
    /* Validate Resize Dimensions */
    isResizable = jpegd_checkResizeDimensions();

    /* If aspect ratio is requested, modify the resize 
     dimensions by calculating the new dimensions in 
     accordance with aspect ratio */
         
      if(localDecodeInfo.aspectRatioFlag && isResizable)
        isResizable = jpegd_calcPixelAspectRatio();

    if (isResizable)
    {
      jpegd_calcScalingFactor();
      if (jpegd_dspResizeFlag)
      {
        bufferWidth = jpegd_dspScaledWidth;

#ifdef JPEGD_USES_DME_LITE
        /*This is  only for JPEGD DME Lite*/
        /*Need to make buffer width multiple of 4 for DSP Scaling 8*/
        if (jpegd_dspScalingFactor == 3)  /*DSP Scaling of 8 */
        {
            /*Make buffer width multiple of 4 for DSP Scaling 8*/
            bufferWidth = (((jpegd_dspScaledWidth) + 3) /4) * 4; 
            /*Need to make sure whether we have padded & how much we have padded*/
            if (jpegd_dspScaledWidth != bufferWidth)
            {
                /*Yes, the padding is done.& now update the adjusted*/
                /*flag to be True and pad width carrespondingly.*/
                jpegd_lite_adjusted =TRUE;
                jpegd_lite_pad_value = bufferWidth - jpegd_dspScaledWidth;
                MSG_HIGH("JPEGD_USES_DME_LITE: Adjusted the pad length by %d",jpegd_lite_pad_value,0,0);
            }
            else 
            {
                MSG_HIGH("JPEGD_USES_DME_LITE: Not adjusted the pad length, since buffer width is already a multiple of 4",0,0,0);

            }
        }
#endif /*JPEGD_USES_DME_LITE*/

        bufferHeight= 16;    /* one MCU row in each fragment. ((1/16)+3)*16 */
      }
      localDecodeInfo.width = localDecodeInfo.resizeImageWidth;
      localDecodeInfo.height = localDecodeInfo.resizeImageHeight;

      jpegd_localImageWidthEven  = (jpegd_localImageWidth & 1) ? (jpegd_localImageWidth + 1):jpegd_localImageWidth;
    }
  }
#endif /*FEATURE_JPEGD_RESIZE*/

                        
  outputLumaSize = bufferWidth * bufferHeight;
  
  /* Purposely allocate bigger size for the Chroma. H2V2=H2V1 */
  for (cnt=0; cnt < 2; cnt++)
  {
#ifdef CACHE_POLICY_WRITE_BACK
    decodedLumaBuffer_malloc[cnt]   = JPEG_MALLOC(outputLumaSize+CACHE_LINE_SIZE);
    decodedLumaBuffer[cnt]          = (uint8*)((((uint32)decodedLumaBuffer_malloc[cnt]+CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
    decodedChromaBuffer_malloc[cnt] = JPEG_MALLOC(outputLumaSize+CACHE_LINE_SIZE);
    decodedChromaBuffer[cnt]        = (uint8*)((((uint32)decodedChromaBuffer_malloc[cnt]+CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
#else
    decodedLumaBuffer[cnt]  = JPEG_MALLOC(outputLumaSize);
    decodedChromaBuffer[cnt]= JPEG_MALLOC(outputLumaSize);
#endif //CACHE_POLICY_WRITE_BACK
    
    if ((!decodedLumaBuffer[cnt])||(!decodedChromaBuffer[cnt]) )
    {
      MSG_ERROR("JPEGDSPDEC: Out of memory, output buffers", 0, 0, 0);
      return FALSE;
    }
  }

#ifdef JPEGD_USES_DME_LITE
  if (jpegd_lite_adjusted)
  /*Once the allocation is over, revert back the outputLumaSize*/
  outputLumaSize = bufferHeight* jpegd_dspScaledWidth;
#endif /*JPEGD_USES_DME_LITE*/
  
  jpegd_get_buf.callback_ptr = NULL; /* synchronous call request */
  jpegd_get_buf.num_bytes    = jpeg_spec.width * jpeg_spec.height * 2;
  
  jpegdDestBufPtr            = NULL;
  jpegd_get_buf.buf_handle   = &jpegdDestBufPtr;
  
  if (jpegdClientCB_func)
  {
    jpegdClientCB_func(JPEG_CMX_GET_BUF, handle, &jpegd_get_buf);
  }
  
  if (!jpegdDestBufPtr)
  {
    jpeg_decode_failed = TRUE;
    MSG_ERROR("JPEGDSPDEC: Null destination buffer pointer", 0, 0, 0);
    return FALSE;
  }
  else
  {
    jpegdCurDestPtr = jpegdDestBufPtr; 
    jpegdChromaCurDestPtr = jpegdDestBufPtr + jpeg_spec.width * jpeg_spec.height;
  }
  
  return TRUE;
}


#ifdef FEATURE_JPEGD_RESIZE
/*===========================================================================

Function: JPEGD_CHECKRESIZEDIMENSIONS

Description:  Checks the resize dimensions and validate it
              
Return Value: None
   
Comments: If the dimensions are invalid, it ignores the input resize dimensions
          and uses the original jpeg dimensions.

Side Effects:

============================================================================*/
 
boolean jpegd_checkResizeDimensions()
{
 // Resize Dimensions cannot be zero 
  if (!(localDecodeInfo.resizeImageWidth) || !(localDecodeInfo.resizeImageHeight))
  {
    MSG_HIGH("JPEGDSPDEC: Image Width or Height cannot be zero, reverting to original dimensions",0,0,0);
    return FALSE;
  }

  if (((localDecodeInfo.resizeImageWidth %2) != 0 ) || ((localDecodeInfo.resizeImageHeight % 2) != 0 ))
  {
    MSG_HIGH("JPEGDSPDEC: No support for odd resize resolution, reverting to original dimensions",0,0,0);
    return FALSE;
  }
  
  // No upsampling support 
  if ((localDecodeInfo.resizeImageWidth > localDecodeInfo.width ) || 
    (localDecodeInfo.resizeImageHeight > localDecodeInfo.height))
  {
    MSG_HIGH("JPEGDSPDEC: No support for upscaling, reverting to original dimensions",0,0,0);
    return FALSE;
  }
  
  return TRUE;
}

/*===========================================================================

Function: JPEGD_CALCPIXELASPECTRATIO

Description:  Calculates the aspect ratio
              
Return Value: None
   
Comments: This function calculates the aspect ratio and recomputes the resize
          dimensions if required

Side Effects:

============================================================================*/
boolean jpegd_calcPixelAspectRatio()
{
  uint32 resizeImageHeight;
  uint32 resizeImageWidth;

   
  /* Aspect Ratio calculation for resizeHeight based on resizeWidth */
  resizeImageHeight = (localDecodeInfo.resizeImageWidth * localDecodeInfo.height ) / (localDecodeInfo.width);
  
  /* If aspect ratio calculation yields an resizeHeight dimension greater than 
    the requested height, compute the resizeWidth based on resizeHeight. This 
    calculation will always ensure that the resultant resizeWidth and resizeHight 
    will be always less than the requsted dimensions */

  if (resizeImageHeight > localDecodeInfo.resizeImageHeight)
  {
    resizeImageWidth = (localDecodeInfo.resizeImageHeight * localDecodeInfo.width ) / (localDecodeInfo.height);
    resizeImageHeight = localDecodeInfo.resizeImageHeight;
  }
  else
  {
    resizeImageWidth = localDecodeInfo.resizeImageWidth;
  }

  /* If width and height are odd, make it even to work for YCbCr data */
  localDecodeInfo.resizeImageWidth = (resizeImageWidth & 1)? (resizeImageWidth + 1):resizeImageWidth;
  localDecodeInfo.resizeImageHeight = (resizeImageHeight & 1)? (resizeImageHeight + 1):resizeImageHeight;

  MSG_HIGH("JPEGDSPDEC: Aspect Ratio Width: %d Height: %d", localDecodeInfo.resizeImageWidth, localDecodeInfo.resizeImageHeight, 0);

  if ((localDecodeInfo.resizeImageWidth < JPGD_MIN_SCALED_WIDTH ) || (localDecodeInfo.resizeImageHeight < JPGD_MIN_SCALED_HEIGHT))
  {
    MSG_HIGH("JPEGDSPDEC: Invalid resize dimensions, reverting to original dimensions",0,0,0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

Function: JPEGD_CALCSCALINGFACTOR

Description:  Calculates the scaling factor
              
Return Value: None
   
Comments: This function calculates the scaling factor required for dsp to resize

Side Effects:

============================================================================*/
void jpegd_calcScalingFactor()
{
  uint16 minFactor, wratio, hratio;
  int16 cnt = -1;
  
  jpegd_dspLocalImageWidth = (((jpegd_localImageWidth+15)/16) * 16);
 
  wratio = jpegd_localImageWidth / localDecodeInfo.resizeImageWidth;
  hratio = jpegd_localImageHeight / localDecodeInfo.resizeImageHeight;

  minFactor = (wratio < hratio) ? wratio: hratio;
  
  /* 
  Calculation of jpeg dsp scaling factor:
  a) DSP Scaling factor of 0 indicates no dsp scaling
  b) DSP Scaling factor of 1 indicates dsp scale by 2
  c) DSP Scaling factor of 2 indicates dsp scale by 4
  d) DSP Scaling factor of 3 indicates dsp scale by 8

  Example for (d): 
  jpegd_dspScalingFactor = 3 and jpegd_scalingFactor = 8
  */
  
  do {
      minFactor = minFactor >> 1;
      cnt++;
      if (cnt > 2) break;
     }
  while (minFactor > 0);

  jpegd_dspScalingFactor = (uint16)cnt;
  jpegd_scalingFactor = ((jpegd_dspScalingFactor > 0) && (jpegd_dspScalingFactor < 4)) ? (2 << (jpegd_dspScalingFactor - 1)):1;

  jpegd_dspScaledWidth = jpegd_dspLocalImageWidth / jpegd_scalingFactor;

  jpegd_scaledWidth  =   jpegd_localImageWidth / jpegd_scalingFactor;
  jpegd_scaledHeight =   jpegd_localImageHeight / jpegd_scalingFactor;

  jpegd_scaledWidth = (jpegd_scaledWidth & 1)? (jpegd_scaledWidth + 1):jpegd_scaledWidth;
  jpegd_scaledHeight = (jpegd_scaledHeight & 1)? (jpegd_scaledHeight + 1):jpegd_scaledHeight;

  /* 
    Decision to use DSP scaler or ARM scaler or a combination of DSP and ARM scaler
  */
  if (jpegd_dspScalingFactor) 
    jpegd_dspResizeFlag = TRUE;
  
  if((localDecodeInfo.resizeImageWidth != (jpegd_scaledWidth)) || (localDecodeInfo.resizeImageHeight != (jpegd_scaledHeight)))
    jpegd_armResizeFlag = TRUE;
#ifdef JPEGD_USES_DME_LITE
  /*Again a typical case will be like*/
  /*an input width say 1296 (162 *8) & height to be 1296*/
  /*& a reuested o/p is 162x162*/
  /*DSP, if using DME_LITE feature, will give 164*164, (because of DME_LITE limitation)*/
  /*which eventually means jpegd_armResizeFlag should be turned on for IPL croping, & color conversion*/
  /*check whether we have jpegd_lite_adjusted & turn on jpegd_armResizeFlag*/
  /*Also pls make sure to place this chunk of code after init*/
  if (jpegd_lite_adjusted)
  {
    jpegd_armResizeFlag = TRUE;
  }
#endif /*JPEGD_USES_DME_LITE*/

  MSG_HIGH("JPEGDSPDEC: Original Dimensions Width: %d  Height: %d",jpegd_localImageWidth,jpegd_localImageHeight,0);
  MSG_HIGH("JPEGDSPDEC: Expected scaled Dimensions Width: %d  Height: %d",localDecodeInfo.resizeImageWidth,localDecodeInfo.resizeImageHeight,0);

  if(jpegd_dspResizeFlag)
  {
    MSG_HIGH("JPEGDSPDEC: Utilizing DSP scaler",0,0,0);
    
    if (jpegd_inFormat == JPEGDEC_H2V2) 
    {
      MSG_HIGH("JPEGDSPDEC: DSP input  Dimensions - xMCU Width: %d  Height: %d",jpegd_dspLocalImageWidth,(((jpegd_localImageHeight+15)/16) * 16),0);
      MSG_HIGH("JPEGDSPDEC: DSP scaled Dimensions Width: %d  Height: %d",jpegd_dspScaledWidth,((((jpegd_localImageHeight+15)/16) * 16) / jpegd_scalingFactor),0);
    }
    else
    {
      MSG_HIGH("JPEGDSPDEC: DSP input  Dimensions - xMCU Width: %d  Height: %d",jpegd_dspLocalImageWidth,(((jpegd_localImageHeight+7)/8) * 8),0);
      MSG_HIGH("JPEGDSPDEC: DSP scaled Dimensions Width: %d  Height: %d",jpegd_dspScaledWidth,((((jpegd_localImageHeight+7)/8) * 8) / jpegd_scalingFactor),0);
    }

    MSG_HIGH("JPEGDSPDEC: DSP scaling factor: %d  Actual Scaling Factor: %d",jpegd_dspScalingFactor, jpegd_scalingFactor,0);
    
  }

  if(jpegd_armResizeFlag)
  {
    MSG_HIGH("JPEGDSPDEC: Utilizing ARM scaler",0,0,0);
    MSG_HIGH("JPEGDSPDEC: ARM scaled Dimensions Width: %d  Height: %d",localDecodeInfo.resizeImageWidth,localDecodeInfo.resizeImageHeight,0);
  }

}
#endif


/*===========================================================================

Function: JPEGD_SEND_INPUT

Description:  Writes the encoded bit-stream to the thumbnail image location
              in the temporary buffer.
              
Return Value: None
   
Comments: This function knows when the thumbnail encoding is done and it 
          will transition the state to encode the main image. An idle command
          is issued to signify the end of thumbnail encoding and the starting 
          of the main image encoding.

Side Effects:

============================================================================*/
void jpegd_send_input(jpegd_bufferType buffer)
{
  JPEGD_InputBufType       input_buf_cmd;
 
  if (isJPEGDAbort) 
  {
    isJPEGDAbort = FALSE;
    return;
  }

  MSG_HIGH("JPEGDSPDEC: sending input buffer %d", buffer, 0, 0);
  
  switch (buffer)
  {
    case BUFFER_1:
    {
      input_buf_cmd.Ptr  = inputBitStreamBuffer[0];
      input_buf_cmd.Size = localJpegBufInfo.bufSize; 
      input_buf_cmd.End_BitStream = endofBitStream;
      
      jpegd_get_data.buffer = inputBitStreamBuffer[1];
      
      break;
    }
    
    case BUFFER_2:
    {
      input_buf_cmd.Ptr  = inputBitStreamBuffer[1];
      input_buf_cmd.Size = localJpegBufInfo.bufSize;
      input_buf_cmd.End_BitStream = endofBitStream;
      
      jpegd_get_data.buffer = inputBitStreamBuffer[0];
      
      break;
    }
    
    default:
    {
      input_buf_cmd.Size = 0;
      MSG_ERROR("JPEGDSPDEC: invalid input buffer encounter", 0, 0, 0);
    }
  }

  JPEG_FLUSH_DATA_CACHE ((uint32*)input_buf_cmd.Ptr, input_buf_cmd.Size);
  /* Send in the input buffer so it can continue */
  jpegQdspReturnStatus = jpeg_qdsp_command(JPEGD_INPUT_CMD, &input_buf_cmd);
  
  if (jpegQdspReturnStatus != JPEGQDSP_SUCCESS)
  {
    MSG_ERROR ("JPEGDSPDEC: Invalid Input Command", 0,0,0);
    jpegd_send_idle_cmd();
  }
  
  if (!endofBitStream)
  {
    /* Have callback function to get more jpeg data */
    /* Keep track of the JPEG bit-stream buffer */
    jpegd_get_data.position = jpeg_srcFile_pos;
   
    if ((localDecodeInfo.filesize - jpeg_srcFile_pos) > localJpegBufInfo.bufSize )
    {
      jpegd_get_data.num_bytes= localJpegBufInfo.bufSize;
      jpeg_srcFile_pos += localJpegBufInfo.bufSize;
    }
    else
    {
      /* May not be constant, whatever left */
      jpegd_get_data.num_bytes= localDecodeInfo.filesize - jpeg_srcFile_pos;
      endofBitStream = 1;
    }
    
    /* This is a synchronous get data call */
    jpegd_get_data.callback_ptr = NULL;
    jpegd_get_data.client_data  = 0;
    
    if (jpegdClientCB_func)
    {
      /* Get more bit-stream data into the input buffer */
       jpegdClientCB_func(JPEG_CMX_GET_DATA, handle, &jpegd_get_data);
    }
  }
}

/*===========================================================================

  Function: JPEGD_PROCESS_OUTPUT_BUFFER
  
  Description:  Read the ping pong buffer and convert it to proper format.
    
  Return Value: None
   
  Comments: This function for now will convert the the DSP decoded buffer data  
  in YCbCr line packed into RGB565 for BREW layer to handle with IDIB
  until the BREW layer can handle the YCbCr .
        
  Side Effects:
          
============================================================================*/
void jpegd_process_output_buffer(jpegd_bufferType buffer)
{
  boolean decodeDone = FALSE;
  uint32  row;
  
  static char *jpegd_err_msg_1[] =
  {
    "JPEGDSPDEC: Buffer1 - End of Image marker error"  ,
      "JPEGDSPDEC: Buffer1 - Restart marker error" ,
      "JPEGDSPDEC: Buffer1 - Incorrect Marker error" ,
      "JPEGDSPDEC: Buffer1 - No Huffman code matched 16 consecutive input bits",
      "JPEGDSPDEC: Buffer1 - Run length Decoding error" ,
      "JPEGDSPDEC: Buffer1 - RLE with more than 64 entries" 
      
  };
  
  static char *jpegd_err_msg_2[] =
  {
      "JPEGDSPDEC: Buffer2 - End of Image marker error" ,
      "JPEGDSPDEC: Buffer2 - Restart marker error" ,
      "JPEGDSPDEC: Buffer2 - Incorrect Marker error" ,
      "JPEGDSPDEC: Buffer2 - No Huffman code matched 16 consecutive input bits" ,
      "JPEGDSPDEC: Buffer2 - Run length Decoding error" ,
      "JPEGDSPDEC: Buffer2 - RLE with more than 64 entries" 
  };
  
  JPEGD_OutBufType  output_consummed;
  static ipl_rect_type copy_area, paste_area;
  static ipl_rect_type cropIn;

  MSG_LOW("JPEGDSPDEC: process output buffer", 0, 0, 0);
  
  switch (buffer)
  {
  case BUFFER_1:
    {
      if ((jpegd_buffer1.LumaPtr != decodedLumaBuffer[0]) ||
        (jpegd_buffer1.ChromaPtr != decodedChromaBuffer[0]) )
      {
        MSG_HIGH("JPEGDSPDEC: LumaPtr1 returned by DSP is = %d", jpegd_buffer1.LumaPtr , 0, 0);
        MSG_HIGH("JPEGDSPDEC: ChromaPtr1 returned by DSP is = %d", jpegd_buffer1.ChromaPtr , 0, 0);
        ERR_FATAL("JPEGDSPDEC: buffer1 pointer not original", 0, 0, 0);
      }
      
      output_consummed.LumaPtr   = jpegd_buffer1.LumaPtr;
      output_consummed.ChromaPtr = jpegd_buffer1.ChromaPtr;
      output_consummed.Size      = jpegd_buffer1.Luma_size;
      JPEG_INVALIDATE_DATA_CACHE ((uint32*)jpegd_buffer1.LumaPtr, outputLumaSize); /*lint !e826 */
      JPEG_INVALIDATE_DATA_CACHE ((uint32*)jpegd_buffer1.ChromaPtr, outputLumaSize); /*lint !e826 */
      decodeDone = (boolean) jpegd_buffer1.Dec_done; 
      
      break;
    }
    
  case BUFFER_2:
    {
      if ((jpegd_buffer2.LumaPtr != decodedLumaBuffer[1]) ||
        (jpegd_buffer2.ChromaPtr != decodedChromaBuffer[1]) )
      {
        MSG_HIGH("JPEGDSPDEC: LumaPtr2 returned by DSP is = %d", jpegd_buffer2.LumaPtr , 0, 0);
        MSG_HIGH("JPEGDSPDEC: ChromaPtr2 returned by DSP is = %d", jpegd_buffer2.ChromaPtr , 0, 0);
        ERR_FATAL("JPEGDSPDEC: buffer2 pointer not original", 0, 0, 0);
      }
      output_consummed.LumaPtr   = jpegd_buffer2.LumaPtr;
      output_consummed.ChromaPtr = jpegd_buffer2.ChromaPtr;
      output_consummed.Size      = jpegd_buffer2.Luma_size;
      JPEG_INVALIDATE_DATA_CACHE ((uint32*)jpegd_buffer2.LumaPtr, outputLumaSize); /*lint !e826 */
      JPEG_INVALIDATE_DATA_CACHE ((uint32*)jpegd_buffer2.ChromaPtr, outputLumaSize); /*lint !e826 */
      decodeDone = (boolean) jpegd_buffer2.Dec_done; 
      
      break;
    }
    
  default:
    {
      output_consummed.LumaPtr = NULL;
      output_consummed.ChromaPtr = NULL;
      MSG_ERROR("JPEGDSPDEC: Error in output buffer", 0, 0, 0);
    }
  }
  
  if(jpegd_buffer1.Decode_err)
  {
    jpegd_error = TRUE;
    MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "%s", jpegd_err_msg_1[jpegd_buffer1.Decode_err - 1]);
  }
  
  jpegd_buffer1.Decode_err = 0;
  
  if(jpegd_buffer2.Decode_err)
  {
    jpegd_error = TRUE;
    MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "%s", jpegd_err_msg_2[jpegd_buffer2.Decode_err - 1]);
  }
  
  jpegd_buffer2.Decode_err = 0;
  
  copy_area.x = 0;
  copy_area.y = 0;
  paste_area.x = 0;
  
  jpegdIPLIn.imgPtr  = output_consummed.LumaPtr;
  jpegdIPLIn.clrPtr  = output_consummed.ChromaPtr;
  jpegdIPLIn.dx = ((jpegd_localImageWidth+15)/16) * 16;
  copy_area.dx = jpegd_localImageWidth;

  jpegdIPLOut.imgPtr  = jpegdCurDestPtr;
  jpegdIPLOut.clrPtr  = jpegdChromaCurDestPtr;
  jpegdIPLOut.dx      = jpeg_spec.width;
  paste_area.dx       = jpeg_spec.width;
  
#ifdef FEATURE_JPEGD_RESIZE
  if (jpegd_armResizeFlag)
  {
    if (jpegd_dspResizeFlag)
    {
    /* Compute the resizeCol adaptively and round off the downsample height if it results in a floating point number. If the 
      resizecol computation results in a overshoot for the current iteration, it tries to undershoot for the next iteration
      so that the operation of decoding and resizing in tandem doesn't get thrown off balance.
      This calculation is used when ARM and DSP scaler are invoked */
      jpegd_resizeCol = (int)((((localDecodeInfo.resizeImageHeight - jpegd_resizeColCum) * 16) / ((float)(jpegd_scaledHeight- jpegd_pixRow))) + 0.5); 
    }
    else
    {
      /* Compute the resizeCol adaptively and round off the downsample height if it results in a floating point number. If the 
      resizecol computation results in a overshoot for the current iteration, it tries to undershoot for the next iteration
      so that the operation of decoding and resizing in tandem doesn't get thrown off balance.
      This calculation is used only when ARM scaler is invoked */
      jpegd_resizeCol = (int)((((localDecodeInfo.resizeImageHeight - jpegd_resizeColCum) * 16) / ((float)(jpegd_localImageHeight- jpegd_pixRow))) + 0.5); 
    }

    //jpegd_resizeCol = (jpegd_resizeCol == 0)? 1:jpegd_resizeCol;

    if ((localDecodeInfo.outFormat == JPEGDEC_YCbCr420) || (localDecodeInfo.outFormat == JPEGDEC_YCrCb420))
    {
      jpegd_resizeCol = ((jpegd_resizeCol % 2) == 0)? jpegd_resizeCol: (jpegd_resizeCol - 1);
    }
  }
  else
  {
    jpegd_resizeCol = 16;
  }
#endif  

#ifdef FEATURE_JPEGD_RESIZE
  if (jpegd_armResizeFlag)
  {
    cropIn.x = 0;
    cropIn.dx = jpegd_localImageWidthEven;
    cropIn.y  = 0;
    cropIn.dy = 16;
  }

  if(jpegd_dspResizeFlag)
  {
    jpegdIPLIn.dx = jpegd_dspScaledWidth;
    copy_area.dx =  jpegd_scaledWidth;
    cropIn.dx = jpegd_scaledWidth;
  }
#endif
  
  if (decodeDone) /* Done with image decoding */
  {
    /* Determine the number of rows to copy at the bottom of image */
      
    row = jpeg_spec.height - jpegd_pixRow;

    if (jpegd_dspResizeFlag)
    {
#ifdef FEATURE_JPEGD_RESIZE
      row = jpegd_scaledHeight - jpegd_pixRow;
#endif
    }
       
    row = (row < 16) ? row : 16;
    jpegdIPLIn.dy  = row;
    copy_area.dy = row;
    paste_area.dy = row;
    jpegdIPLOut.dy = row;
    cropIn.dy = row;

    /* Calculates the remaining number of rows to be copied at the bottom
    of image */
#ifdef FEATURE_JPEGD_RESIZE
    if (jpegd_armResizeFlag)
    {
      jpegdIPLOut.dy = localDecodeInfo.resizeImageHeight - jpegd_resizeColCum;
    //  jpegdIPLOut.dy = (jpegdIPLOut.dy == 0)? 1:jpegdIPLOut.dy;
      paste_area.dy = jpegdIPLOut.dy;
      
    }
#endif    
    
  }
  else
  {
    jpegd_pixRow += 16;
    jpegdIPLIn.dy = 16;
    copy_area.dy = 16;
    jpegdIPLOut.dy = 16;
    paste_area.dy = 16;
   
#ifdef FEATURE_JPEGD_RESIZE
    if (jpegd_armResizeFlag)
    {
      jpegdIPLOut.dy = jpegd_resizeCol;
      paste_area.dy = jpegd_resizeCol;
      jpegd_resizeColCum =  jpegd_resizeColCum +  jpegd_resizeCol;
    }
#endif
  }

  if (jpegd_armResizeFlag)
      jpegdIPLIn.dy = 16;
  
#ifdef JPEGD_USES_DME_LITE
if (jpegd_lite_adjusted)
{
    /*if we have adjusted something, we have to correct it and give it to IPL*/
    /*A typical case can be 162, which is not a multiple of 4 , got adjusted and now it is 164*/
    /*So update the In image width by in Image width + pad value*/
    /*Also update the Crop value with the pad value*/
    /*Make sure that this chunk of code is placed after JPEGD_RESIZE calculations*/
    jpegdIPLIn.dx= jpegdIPLIn.dx+jpegd_lite_pad_value;
    cropIn.dx = cropIn.dx + jpegd_lite_pad_value;
    MSG_HIGH ("Adjusting the dimensions before calling IPL",0,0,0);
    MSG_HIGH ("jpegdIPLIn.dx = %d & cropIn.dx = %d",jpegdIPLIn.dx,cropIn.dx,0);
}
#endif /*JPEGD_USES_DME_LITE*/

  switch (localDecodeInfo.outFormat)
  {  
  case JPEGDEC_RGB565:
    {
      jpegdIPLOut.cFormat = IPL_RGB565;
      if (jpegd_inFormat == JPEGDEC_H2V2)
      {
        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
           jpegdIPLIn.cFormat = IPL_YCbCr420_LINE_PK;
        else
           jpegdIPLIn.cFormat = IPL_YCrCb420_LINE_PK;
        

        if (jpegd_armResizeFlag)
        {
          jpegdTiming.armresize_start = (uint32)timetick_get_ms();
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize and convert 420 to RGB565", 0, 0, 0);
          }
#endif /*FEATURE_JPEGD_RESIZE*/
          jpegdTiming.armresize_stop = (uint32)timetick_get_ms();
        }
        else
        {
          if (ipl3_convert_ycrcb420lp_to_rgb565(&jpegdIPLIn, &jpegdIPLOut) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to convert 420 to RGB565", 0, 0, 0);
          }
        }
      }
      else
      {
        jpegdIPLOut.cFormat = IPL_RGB565;

        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
           jpegdIPLIn.cFormat = IPL_YCbCr422_LINE_PK;
        else
           jpegdIPLIn.cFormat = IPL_YCrCb422_LINE_PK;
        

        if (jpegd_armResizeFlag)
        {
          jpegdTiming.armresize_start = (uint32)timetick_get_ms();
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize and convert 422 to RGB565", 0, 0, 0);
          }
#endif
          jpegdTiming.armresize_stop = (uint32)timetick_get_ms();
        }
        else
        {
          if (ipl3_convert_ycrcb422lp_to_rgb565(&jpegdIPLIn, &jpegdIPLOut) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to convert 422 to RGB565", 0, 0, 0);
          }
        }
      }
      break;
    }
    
  case JPEGDEC_YCbCr422:
    {
      jpegdIPLOut.cFormat = IPL_YCbCr422_LINE_PK;
      if (jpegd_inFormat == JPEGDEC_H2V1)
      {
        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
           jpegdIPLIn.cFormat = IPL_YCbCr422_LINE_PK;
        else
           jpegdIPLIn.cFormat = IPL_YCrCb422_LINE_PK;
    
        if (jpegd_armResizeFlag)
        {
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize and convert 422 to 422", 0, 0, 0);
          }
#endif
        }
        else
        {
          if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to convert YCrCb422 to YCbCr422", 0, 0, 0);
            }
        }
      }
      else if (jpegd_inFormat == JPEGDEC_H2V2)
      {
        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
           jpegdIPLIn.cFormat = IPL_YCbCr420_LINE_PK;
        else
           jpegdIPLIn.cFormat = IPL_YCrCb420_LINE_PK;

        if (jpegd_armResizeFlag)
        {
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize and convert 422 to 422", 0, 0, 0);
          }
#endif
        }
        else
        {
          if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
          {  
            MSG_ERROR("JPEGDSPDEC: Failed to convert YCrCb422 to YCbCr420", 0, 0, 0);
          }
        }
      }
      
      break;
    }
    
  case JPEGDEC_YCbCr420:
    {
      jpegdIPLOut.cFormat = IPL_YCbCr420_LINE_PK;

      if (jpegd_inFormat == JPEGDEC_H2V2)
      {
        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
           jpegdIPLIn.cFormat = IPL_YCbCr420_LINE_PK;
        else
           jpegdIPLIn.cFormat = IPL_YCrCb420_LINE_PK;
        if (jpegd_armResizeFlag)
        {        
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize YCbCr420 to YCbCr420", 0, 0, 0);
          }
#endif
        }

        else
        {
          if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to convert YCrCb420 to YCbCr420", 0, 0, 0);
          }
        }
      }
      else if (jpegd_inFormat == JPEGDEC_H2V1)
      {
        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
           jpegdIPLIn.cFormat = IPL_YCbCr422_LINE_PK;
        else
           jpegdIPLIn.cFormat = IPL_YCrCb422_LINE_PK;
        
        if (jpegd_armResizeFlag)
        {
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize and convert 422 to 420", 0, 0, 0);
          }
#endif
        }

        else
        {
          if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to convert YCrCb420 to YCbCr422", 0, 0, 0);
          }
        }
      }

      break;
    }

  case JPEGDEC_YCrCb422:
    {
      jpegdIPLOut.cFormat = IPL_YCrCb422_LINE_PK;

      if (jpegd_inFormat == JPEGDEC_H2V2)
      {
        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
          jpegdIPLIn.cFormat = IPL_YCbCr420_LINE_PK;
        else
          jpegdIPLIn.cFormat = IPL_YCrCb420_LINE_PK;

        if (jpegd_armResizeFlag)
        {        
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize and convert 420 to 422", 0, 0, 0);
          }
#endif
        }
        else
        {
          if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to convert YCrCb420 to YCbCr422", 0, 0, 0);
          }
        }
      }
      else if (jpegd_inFormat == JPEGDEC_H2V1)
      {
        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
           jpegdIPLIn.cFormat = IPL_YCbCr422_LINE_PK;
        else
           jpegdIPLIn.cFormat = IPL_YCrCb422_LINE_PK;
        
        if (jpegd_armResizeFlag)
        {
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize and convert 422 to 420", 0, 0, 0);
          }
#endif
        }
        else
        {
          if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to convert YCrCb420 to YCbCr422", 0, 0, 0);
          }
        }
      }

      break;
    }

  case JPEGDEC_YCrCb420:
    {
      jpegdIPLOut.cFormat = IPL_YCrCb420_LINE_PK;

      if (jpegd_inFormat == JPEGDEC_H2V2)
      {
        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
          jpegdIPLIn.cFormat = IPL_YCbCr420_LINE_PK;
        else
          jpegdIPLIn.cFormat = IPL_YCrCb420_LINE_PK;

        if (jpegd_armResizeFlag)
        {        
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize and convert 420 to RGB565", 0, 0, 0);
          }
#endif
        }
        else
        {
          if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to convert YCrCb420 to YCbCr422", 0, 0, 0);
          }
        }
      }
      else if (jpegd_inFormat == JPEGDEC_H2V1)
      {
        if(localDecodeInfo.chromaType == JPEGDEC_CbCr)
           jpegdIPLIn.cFormat = IPL_YCbCr422_LINE_PK;
        else
           jpegdIPLIn.cFormat = IPL_YCrCb422_LINE_PK;
        
        if (jpegd_armResizeFlag)
        {
#ifdef FEATURE_JPEGD_RESIZE
          if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to resize and convert 422 to 420", 0, 0, 0);
          }
#endif
        }
        else
        {
          if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
          {
            MSG_ERROR("JPEGDSPDEC: Failed to convert YCrCb420 to YCbCr422", 0, 0, 0);
          }
        }
      }

      break;
    }
  }
  
  if (decodeDone)
  {
    jpegd_process_callback( jpegd_session_id, (int32)JPEGDEC_DONE );
  }
  else
  {
    switch (localDecodeInfo.outFormat)
    {
    case JPEGDEC_RGB565:
      {
#ifdef FEATURE_JPEGD_RESIZE
        if(jpegd_armResizeFlag)
          jpegdCurDestPtr = jpegdCurDestPtr + jpeg_spec.width * jpegd_resizeCol * 2;
#endif
        if(!jpegd_armResizeFlag)
          jpegdCurDestPtr = jpegdCurDestPtr + jpeg_spec.width*16*2; 
        
        break;
      }
      
    case JPEGDEC_YCbCr422:
    case JPEGDEC_YCrCb422:
      {
        
#ifdef FEATURE_JPEGD_RESIZE
        if(jpegd_armResizeFlag)
        {
          jpegdCurDestPtr = jpegdCurDestPtr + jpeg_spec.width * jpegd_resizeCol; 
          jpegdChromaCurDestPtr = jpegdChromaCurDestPtr + jpeg_spec.width * jpegd_resizeCol; 
        }
#endif
        if(!jpegd_armResizeFlag)
        {
          jpegdCurDestPtr = jpegdCurDestPtr + jpeg_spec.width * 16; 
          jpegdChromaCurDestPtr = jpegdChromaCurDestPtr + jpeg_spec.width * 16; 
        }
        
        break;
      }
      
    case JPEGDEC_YCbCr420:
    case JPEGDEC_YCrCb420:
      {
#ifdef FEATURE_JPEGD_RESIZE
        if(jpegd_armResizeFlag)
        {
          jpegdCurDestPtr = jpegdCurDestPtr + jpeg_spec.width * jpegd_resizeCol; 
          jpegdChromaCurDestPtr = jpegdChromaCurDestPtr + (jpeg_spec.width * jpegd_resizeCol)/ 2; 
        }
#endif
        if(!jpegd_armResizeFlag)
        {
          jpegdCurDestPtr = jpegdCurDestPtr + jpeg_spec.width * 16; 
          jpegdChromaCurDestPtr = jpegdChromaCurDestPtr + (jpeg_spec.width * 16)/ 2;
        }
        break;
      }
    }
    
   // if (jpegdTiming.opm_received > jpegdTiming.occ_sent)
     // MSG_HIGH("JPEGDSPDEC: JPEG Decoding timing for 16 rows: %d msec", (jpegdTiming.opm_received - jpegdTiming.occ_sent), 0, 0);
    
    if (jpegdTiming.armresize_stop > jpegdTiming.armresize_start)
      MSG_HIGH("JPEGDSPDEC: JPEG Decoding ARM Resize timing: %d msec", (jpegdTiming.armresize_stop - jpegdTiming.armresize_start), 0, 0);

    /* Notify DSP with an output consummed command */ 
    output_consummed.Size = outputLumaSize;
    jpegQdspReturnStatus = jpeg_qdsp_command(JPEGD_OUTPUT_CONSUMED, &output_consummed);
    
    jpegdTiming.occ_sent = (uint32)timetick_get_ms();

    if (jpegQdspReturnStatus != JPEGQDSP_SUCCESS)
    {
      MSG_ERROR ("JPEGDSPDEC: Invalid Output Consumed Command", 0,0,0);
      jpegd_send_idle_cmd();
    }
  }
}

/*===========================================================================

Function: JPEGD_PROCESS_CALLBACK

Description:  This function executes in the GRAPH task and it behaves as a
              state machine for the JPEG DECODER.
              
Return Value: None
   
Comments:  

Side Effects:

============================================================================*/
static void jpegd_process_callback(int32 clientId, int32 clientData)
{
  JPEGDEC_operationType  operation;
  
  MSG_LOW("JPEGDSPDEC: in callback function %d %d %d", 0, 0, clientData);
  
  if (clientId != jpegd_session_id)
  {
    MSG_HIGH("JPEGDSPDEC: invalid callback session ID",0,0,0);
    return;
  }
  
  /* Abort is external command and failure is from decoding  */
  if (jpeg_decode_abort || jpeg_decode_failed)
  {
    jpegd_in_shutdown_process = TRUE;
    jpeg_decode_abort = FALSE;   
    jpeg_decode_failed = FALSE;
    isJPEGDAbort = TRUE;
  
    /* Command the DSP to IDLE state and ready to terminate the DECODER */ 
    jpegQdspReturnStatus = jpeg_qdsp_command(JPEGD_IDLE, NULL);
    
    if (jpegQdspReturnStatus != JPEGQDSP_SUCCESS)
    {
      MSG_ERROR ("JPEGDSPDEC: Invalid Idle command", 0,0,0);
    }
    
    return;   
  }
  
  operation = (JPEGDEC_operationType) clientData;

  switch (operation)
  {
    case JPEGDEC_DECODE_IMG:
    {
      /* Image just downloaded. Setup necessary buffers */
      if (jpegd_init(localJpegBufInfo) == FALSE)
      {
        MSG_ERROR ("JPEGDSPDEC: JPEG Decoder initialization failed", 0,0,0);
        jpegd_send_idle_cmd();
      }
      else if( jpegd_setup_decode_cmd(&localDecodeInfo, &decodeCmdData) == JPEGDEC_INVALID_PTR )
      {
        MSG_ERROR ("JPEGDSPDEC: JPEG Decoder invalid ptr", 0,0,0);
        jpegd_send_idle_cmd();
      }
      else
      {
        jpegQdspReturnStatus = jpeg_qdsp_command( JPEGD_DECODE_CMD, &decodeCmdData); 
        
        if (jpegQdspReturnStatus != JPEGQDSP_SUCCESS)
        {
          MSG_ERROR ("JPEGDSPDEC: Invalid Decode command", 0,0,0);
          jpegd_send_idle_cmd();
        }
      
        if (!endofBitStream)
        {
          /* Have callback function to get more jpeg data */
          /* Keep track of the JPEG bit-stream buffer */
          jpegd_get_data.buffer   = inputBitStreamBuffer[1];
          jpegd_get_data.position = jpeg_srcFile_pos;

          if ((localDecodeInfo.filesize - jpeg_srcFile_pos) > localJpegBufInfo.bufSize )
          {
            jpegd_get_data.num_bytes= localJpegBufInfo.bufSize;
            jpeg_srcFile_pos += localJpegBufInfo.bufSize;
          }
          else
          {
            /* May not be constant, whatever left */
            jpegd_get_data.num_bytes= localDecodeInfo.filesize - jpeg_srcFile_pos;
            endofBitStream = 1;
          }
        
          /* This is a synchronous get data call */
          jpegd_get_data.callback_ptr = NULL;
          jpegd_get_data.client_data  = 0;
        
          if (jpegdClientCB_func)
          {
            /* Get more bit-stream data into the input buffer */
            jpegdClientCB_func(JPEG_CMX_GET_DATA, handle, &jpegd_get_data);
          }
        }
      }
      
      break;
    }
    
    case JPEGDEC_READ_BUFFER1:
    {
      /* Process the DSP's output buffer in buffer1 */
      jpegd_process_output_buffer(BUFFER_1);
      break;
    }
    
    case JPEGDEC_READ_BUFFER2:
    {
      /* Process the DSP's output buffer in buffer2 */
      jpegd_process_output_buffer(BUFFER_2);
      break;
    }
    
    case JPEGDEC_SEND_BUFFER1:
    {
      jpegd_send_input(BUFFER_1);
      break;
    }
    
    case JPEGDEC_SEND_BUFFER2:
    {
      jpegd_send_input(BUFFER_2);
      break;
    }
    
    case JPEGDEC_IDLE:
    {
      if (jpegd_in_shutdown_process)
      {
        jpegd_decoder_terminate();
        
        if (jpegdClientCB_func)
        {
          jpegdClientCB_func(JPEG_CMX_ABORT, handle, NULL);
        }
        else
        {
          MSG_HIGH("JPEGDSPDEC: Null function pointer", 0, 0, 0);
        }
        
        return;
      }
      
      MSG_HIGH("JPEGDSPDEC: JPEG Decoder in Idle state", 0, 0, 0);
      break;
    }
  
    case JPEGDEC_DONE:
    {
      /* Clean up the jpeg DECODER space */
      jpegd_decoder_terminate();
   
      JPEG_FLUSH_DATA_CACHE ((uint32*)jpegdDestBufPtr, jpegd_get_buf.num_bytes);
   
      MSG_HIGH("JPEGDSPDEC: JPEG Decoding Done",0,0,0); 

      /* Send decode done status to client */
      if (jpegdClientCB_func)
      {
        if(!jpegd_error)
          jpegdClientCB_func(JPEG_CMX_DONE, handle, NULL);
        else
          jpegdClientCB_func(JPEG_CMX_INVALID, handle, NULL);
      }
      else
      {
        MSG_HIGH("JPEGDSPDEC: Null function pointer", 0, 0, 0);
      }
      
      break;
    }
    
    default:
    {
      MSG_HIGH("JPEGDSPDEC: unrecognize decoding operation",0,0,0);
    }
  }
}


/*===========================================================================

Function: JPEGED_DECODER_TERMINATE

Description:  This function disables the DSP, free all memory and reset the 
              operation states.
              
Return Value: None
   
Comments:  

Side Effects:

============================================================================*/
static void jpegd_decoder_terminate()
{
  int i;
  
  jpeg_decoding = FALSE;
  jpeg_decode_abort  = FALSE;
  jpeg_decode_failed = FALSE;
  jpegd_in_shutdown_process = FALSE;

#ifdef JPEGD_USES_DME_LITE
  /*Turning off the jpegd_lite_adjusted flag*/
  jpegd_lite_adjusted = FALSE;
#endif /*JPEGD_USES_DME_LITE*/

  jpegd2_terminate();
  
  for (i = 0; i < 2; i++)
  {
#ifdef CACHE_POLICY_WRITE_BACK
    inputBitStreamBuffer[i] = inputBitStreamBuffer_malloc[i];
    inputBitStreamBuffer_malloc[i] = NULL;
#endif
    if (inputBitStreamBuffer[i])
    {
      JPEG_FREE(inputBitStreamBuffer[i]);
      inputBitStreamBuffer[i] = NULL;
    }
#ifdef CACHE_POLICY_WRITE_BACK
    decodedLumaBuffer[i] = decodedLumaBuffer_malloc[i];
    decodedLumaBuffer_malloc[i] = NULL;
#endif
    if (decodedLumaBuffer[i])
    {
      JPEG_FREE(decodedLumaBuffer[i]);
      decodedLumaBuffer[i] = NULL;
    }
    
#ifdef CACHE_POLICY_WRITE_BACK
    decodedChromaBuffer[i] = decodedChromaBuffer_malloc[i];
    decodedChromaBuffer_malloc[i] = NULL;
#endif
    if (decodedChromaBuffer[i])
    {
      JPEG_FREE(decodedChromaBuffer[i]);
      decodedChromaBuffer[i] = NULL;
    }
  }
    
  release_memory();

}

/*===========================================================================

Function: JPEGD_DECODER_ABORT

Description:  This function is used to abort the on going encoding session.
              
Return Value:
   JPEGDEC_SUCCESS      - cmd sucessful
   JPEGDEC_BAD_ID       - Bad client ID for this command
   
Comments: If JPEGDEC_SUCCESS, caller should wait for the callback from the
           encoding session to properly abort the session. 

Side Effects:

============================================================================*/
void  jpegd_decoder_abort
(
   uint16      ClientId                 /* Use the same client ID for the   */
)                                       /*  encoding session                */
{    
  if (ClientId == jpeg_client_id)
  {
    jpeg_decode_abort = TRUE;
  }
}

/*===========================================================================

Function: JPEGD_SEND_IDLE_CMD

Description:  This function is used to queue in graph task the 
IDLE Command request to DSP
              
Return Value:
   NONE
   
Comments:  

Side Effects:

============================================================================*/
static void jpegd_send_idle_cmd
(
  void       
)
{    
  jpeg_decode_failed = TRUE;
  jpegd_context_data.client_data = (int16)JPEGDEC_IDLE;
  jpegd_context_data.function_ptr = jpegd_process_callback;
  jpegd_context_data.client_ID = jpegd_session_id;
  graph_context_req( jpegd_context_data );
  MSG_HIGH("JPEGDSPDEC: ARM sends IDLE Command to DSP",0,0,0);
}
#endif /* FEATURE_JPEG_DECODER_REV2 */

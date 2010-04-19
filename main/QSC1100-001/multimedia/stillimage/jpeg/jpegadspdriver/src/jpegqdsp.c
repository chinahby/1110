/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  JPEG QDSP Driver Interface for MSM6550

GENERAL DESCRIPTION
  This module contains functions of JPEG driver for the QDSP


EXTERNALIZED FUNCTIONS
  jpegqdsp_initialize
    This function downloads the QDSP image and start the QDSP
  jpegqdsp_command
    This function commands the JPEG QDSP to perform requested operation
  jpegqdsp_terminate
    This function stops the JPEGE operations in QDSP


INITIALIZATION AND SEQUENCING REQUIREMENTS
  See header file for more details

Copyright(c) 2004 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegadspdriver/main/latest/src/jpegqdsp.c#7 $ $Author: kdivvela $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/05/09   kdiv    Make sure to abort the QDSP current session before termination (while the 
                      request comes from QDSP layers)
12/22/08   kdiv    Reverting the CL#770749
11/24/08   sigu    Fixed an issue for not waiting for IDLE response from JPEGD DSP.
11/06/08   kdiv    Enable video clock for jpegd only if firmware uses the hardware block
10/27/08   sigu    Send the IDLE command before disabling QDSP_APP_JPEGE
04/16/07   arv     Added unique qdsp application id for JPEGE and JPEGD 
08/14/06   vma     Fixed lint errors
05/24/06   arv     Modified the firmware image name to JPEGE/JPEGD to make it 
                   uniform across all targets
03/17/06   dl      Use video resources and ADSP module base on target.
01/20/06   dl      Remove check for the last byte of encode stream. It should be
                   check in jpeg encoder.
09/12/05   pdo     Removed lints to enhance code quality
05/02/05   arv     Added the video resource clk to effectively manage turning ON/OFF
                   clk for optimum power consumption
03/22/05   arv     Featurizing the JPEG Encoder/Decoder - FEATURE_JPEG_ENCODER_REV2
                   and FEATURE_JPEG_DECODER_REV2
03/10/05   dl      Removed qdsp reset command before enabling jpeg encoder 
                   and decoder.
02/10/05   pdo     Removed DSP idle command from both encoder/decoder terminate
                   functions.
11/23/04   pdo     Added support for JPEG decoder
10/27/04   pdo     Added debug msg for ARM/DSP workign buffers
10/04/04   pdo     Removed extra byte of (00) inserted by DSP at the end.
07/12/04   pdo     Initial version

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                       INCLUDE FILES FOR MODULE

                
===========================================================================*/

#include "msg.h"             /* Message logging/reporting services         */
#include "qdsp.h"            /* QDSP services                              */
#ifdef JPEG_VIDEO_RESOURCES
#include "video_resources.h" /* Video resource - clk*/
#endif
#include "clk.h"             /* Clk resource*/
#include "jpegqdsp.h"        /* JPEGQDSP driver typedefs and prototype*/
#include "qdspext.h"

#if defined(FEATURE_JPEG_DECODER_REV2) || defined(FEATURE_JPEG_ENCODER_REV2)

#define   CODE_MASK  ((uint32) 0xFFFF0000)
#define   CODE_MSG   ((uint32) 0x00010000)

#define   ENCODE_CMD_CODE      0x08000000
#define   IDLE_CMD_CODE        0x08010000
#define   OUTPUT_CONSUME_CODE  0x08020000

#define   DECODE_CMD_CODE             0x09000000
#define   DECODE_IDLE_CMD_CODE        0x09010000
#define   DECODE_OUTPUT_CONSUME_CODE  0x09020000
#define   DECODE_CONTINUE_CODE        0x09030000

LOCAL void jpege_q2a_isr( void );
LOCAL void jpege_q2a_callback( qdsp_event_data_type *pEvent);

LOCAL void jpegd2_q2a_isr( void );
LOCAL void jpegd2_q2a_callback( qdsp_event_data_type *pEvent);

static boolean ack_reset = FALSE;
static qdsp_app_type current_qdsp_jpeg_app = QDSP_APP_MAX;
extern boolean  jpeg_decode_abort;
extern boolean  jpeg_encode_abort;
/*===========================================================================
    
                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* message object to communicate to ARM via callback */
JPEGQDSPMsgType      JPEGE_Message;

/* callback function pointer provided by ARM in initialize function */
JPEG_msgCBType       JPEGEqdsp_CB = NULL;

/* local QDSP state variable */
JPEGE_stateType      JpegeState = JPEGE_RESET_STATE;

#ifdef FEATURE_QDSP_USES_MODULES
qdsp_isr_func_type jpege_isr_table[QDSP_ISR_MAX] = {NULL, NULL};
#endif /* FEATURE_QDSP_USES_MODULES */


/* the following are for JPEG decoder */
#ifdef FEATURE_JPEG_DECODER

/* callback function pointer provided by ARM in initialize function */
JPEG_msgCBType       JPEGDqdsp_CB = NULL;

/* local QDSP state variable */
JPEGD_stateType      JpegdState = JPEGD_RESET_STATE;

#ifdef FEATURE_QDSP_USES_MODULES
qdsp_isr_func_type jpegd_isr_table[QDSP_ISR_MAX] = {NULL, NULL};
#endif /* FEATURE_QDSP_USES_MODULES */

#ifndef JPEG_USES_QDSP_JPEGID
#define QDSP_APP_JPEGE QDSP_APP_IGG
#endif

#ifndef JPEG_USES_QDSP_JPEGID
#define QDSP_APP_JPEGD QDSP_APP_IGG
#endif

#endif
/*===========================================================================

Function: JPEG_QDSP_INITIALIZE

Description: This function initializes the JPEG DSP images.
             This is an asynchronous function. Caller of this routine
              will receive a return status.
             Upon successful downloading of the QDSP code, a callback
              will be invoked to notify the caller. This may take few hundred
              miliseconds.

Return Value:
   JPEG_SUCCESS              cmd sucessful
   JPEG_INVALID_CB           init cmd contains a NULL cb pointer
   JPEG_INVALID_STATE        init cmd issued during invalid state
   JPEG_DWNLD_FAIL           JPEG download failed

Comments: This function must be called prior to calling any other function
          or issuing any qdsp commands.

Side Effects: None

==============================================================================*/
JPEGQDSPReturnCodeType   jpeg_qdsp_initialize
(
  JPEG_msgCBType        QDSPServiceCb, /* qdsp service call back function */
  JPEG_appImageType     application    /* Either JPEGE or JPEGD service   */
)
{

  if (ack_reset)
  {
    /* We should not get here. This is just to alert if ack_reset is not resetted properly*/
    ack_reset = FALSE;
    qdsp_disable_event_rsp(current_qdsp_jpeg_app);
    MSG_ERROR("Unexpected ack_reset, previous app =%d", current_qdsp_jpeg_app,0,0);
  }

  if(application == JPEG_ENCODER_DSP)
  {
#ifdef JPEG_VIDEO_RESOURCES
    video_resources_clock_on(VIDEO_RESOURCES_CLIENT_JPEGE,CLK_RGM_VIDEO_M);
#endif

#ifdef FEATURE_QDSP_USES_MODULES
    jpege_isr_table[QDSP_ISR_INT0] = jpege_q2a_isr;
#endif /* FEATURE_QDSP_USES_MODULES */

    /* QDSP image is being downloaded */
    JpegeState = JPEGE_RESET_STATE;

    JPEGEqdsp_CB = QDSPServiceCb;

#ifdef FEATURE_QDSP_USES_MODULES
    /*  register the camqdsp ISR and status callback functions           */
    qdsp_register_app( QDSP_APP_JPEGE,
                       jpege_isr_table,
                       jpege_q2a_callback,
                       (uint16)QDSP_EV_COMMAND | (uint16)QDSP_EV_MODULE
                     );
    /* enable the JPEG accelerator application image                     */
    qdsp_enable( QDSP_APP_JPEGE, QDSP_MODULE_JPEGE);
#else
    qdsp_register( QDSP_APP_JPEGE,
                   jpege_q2a_isr,
                   QDSP_ISR_INT0,
                   jpege_q2a_callback,
                   (uint16)QDSP_EV_COMMAND | (uint16)QDSP_EV_MODULE | (uint16)QDSP_EV_IMAGE
                 );
    /* enable the JPEG accelerator application image                                  */
    qdsp_enable( QDSP_APP_JPEGE, QDSP_IMAGE_JPEGE_6550);
#endif /* FEATURE_QDSP_USES_MODULES */

    current_qdsp_jpeg_app = QDSP_APP_JPEGE;
  }
  else if(application == JPEG_DECODER_DSP)
  {
#if defined(JPEG_VIDEO_RESOURCES) && defined(JPEGD_VIDEO_RESOURCES)
#error code not present
#endif

#ifdef FEATURE_QDSP_USES_MODULES
    jpegd_isr_table[QDSP_ISR_INT0] = jpegd2_q2a_isr;
#endif /* FEATURE_QDSP_USES_MODULES */

    /* QDSP image is being downloaded */
    JpegdState = JPEGD_RESET_STATE;

    JPEGDqdsp_CB = QDSPServiceCb;

    /*  register the camqdsp ISR and status callback functions           */
#ifdef FEATURE_QDSP_USES_MODULES
    qdsp_register_app( QDSP_APP_JPEGD,
                       jpegd_isr_table,
                       jpegd2_q2a_callback,
                       (uint16)QDSP_EV_COMMAND | (uint16)QDSP_EV_MODULE
                     );
    /* enable the JPEG accelerator application image                     */
    qdsp_enable( QDSP_APP_JPEGD, QDSP_MODULE_JPEGD);
#else
    qdsp_register( QDSP_APP_JPEGD,
                   jpegd_q2a_isr,
                   QDSP_ISR_INT0,
                   jpegd2_q2a_callback,
                   (uint16)QDSP_EV_COMMAND | (uint16)QDSP_EV_MODULE | (uint16)QDSP_EV_IMAGE
                 );
    /* enable the JPEG accelerator application image                                  */
    qdsp_enable( QDSP_APP_JPEGD, QDSP_IMAGE_JPEGD_6550);
#endif /* FEATURE_QDSP_USES_MODULES */

    current_qdsp_jpeg_app = QDSP_APP_JPEGD;
  }

  return JPEGQDSP_SUCCESS;
} /* jpeg_qdsp_initialize */
/* <EJECT> */
/*===========================================================================

Function: JPEGQDSP_COMMAND

Description:  This function is used to command the QDSP software.
              It is only valid after the QDSP download is successful.
              This is a synchronous operation.

Return Value:
   JPEGQDSP_SUCCESS      - cmd sucessful
   JPEGQDSP_INVALID_CMD  - invalid QDSP command specified
   JPEGQDSP_INVALID_PARM - invalid QDSP command parameter

Comments: See JPEGQDSPCommandType for command options

Side Effects:

============================================================================*/
JPEGQDSPReturnCodeType   jpeg_qdsp_command
(
  JPEGE_commandType     CommandCode,     /* type of command                */
  void                  *pCmd            /* pointer to DSP cmd buffer      */
                                         /* NULL if no cmd buffer          */
)
{
  uint32                 code, cnt;
  uint32                 DSP_commandReg = 0;
  JPEGE_encodeCmdType   *pEncodeCfgData;
  JPEGE_OutBufCfgType   *pOutputData;
  JPEGD_decodeCmdType   *pDecodeCfgData;
  JPEGD_OutBufType      *pDecOutputData;
  JPEGD_InputBufType    *pDecInputData;

  JPEGQDSPReturnCodeType  returnStatus = JPEGQDSP_SUCCESS;

  switch(CommandCode)
  {
  case JPEGE_ENCODE_CMD:
    if(JpegeState != JPEGE_IDLE_STATE)
    {
      return JPEGQDSP_INVALID_STATE;
    }

    MSG_HIGH("JPEG: Prepare to send encode command", 0, 0, 0);

    //Set up necessary parameters to command the encoder
    DSP_commandReg = qdsp_read(QDSP_jpegeCommandReg);
    code = DSP_commandReg & CODE_MASK;
    if( code )
    {
      MSG_HIGH("JPEG: Command register was not 0", 0, 0, 0);
      returnStatus = JPEGQDSP_QDSP_BUSY;
    }

    if(pCmd)
    {
      pEncodeCfgData = (JPEGE_encodeCmdType *)pCmd;

      if(!(pEncodeCfgData->Enc.Fragments)||!(pEncodeCfgData->Img.Width))
      {
        MSG_ERROR("Invalid parameters detected", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }

      for(cnt=0; cnt<pEncodeCfgData->Enc.Fragments; cnt++)
      {
        if(!(pEncodeCfgData->Fragmnt[cnt].Height)||
           !(pEncodeCfgData->Fragmnt[cnt].Luma_ptr)||
           !(pEncodeCfgData->Fragmnt[cnt].Chroma_ptr)  )
        {
          MSG_ERROR("Invalid fragment parameters detected", 0 , 0, 0);
          return JPEGQDSP_INVALID_PARM;
        }
      }

      if(!(pEncodeCfgData->Out[0].Ptr) ||!(pEncodeCfgData->Out[0].Size))
      {
        MSG_ERROR("Invalid Buf0 address/size detected", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }

      if(!(pEncodeCfgData->Out[1].Ptr) ||!(pEncodeCfgData->Out[1].Size))
      {
        MSG_HIGH("Invalid Buf1 address/size detected", 0 , 0, 0);
      }

      if( !(pEncodeCfgData->LumaDQTPtr) ||
          !(pEncodeCfgData->ChromaDQTPtr) )
      {
        MSG_ERROR("Invalid DQT table detected", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }
      else
      {
        /* copy the quantization tables to the QDSP */
        qdsp_block_write(QDSP_jpegeQuantTableLuma, 0,
                         pEncodeCfgData->LumaDQTPtr, 64, FALSE);

        qdsp_block_write(QDSP_jpegeQuantTableChroma, 0,
                         pEncodeCfgData->ChromaDQTPtr, 64, FALSE);

        cnt = 30; //sizeof(JPEGE_encodeCmdType) >> 2; /* number of 32 bit words */

        /* Copy the command buffer data to QDSP */
        qdsp_block_write32(QDSP_jpegeCommandBuf, 0,(uint32 *) pCmd, cnt, FALSE);

        DSP_commandReg = cnt;
        DSP_commandReg |= ENCODE_CMD_CODE; /* Jpeg encode command */

        /* Set Encode command ID */
        qdsp_write(QDSP_jpegeCommandReg, DSP_commandReg);

        /* Assert ARM to DSP interrupt */
        qdsp_up_to_dsp_irq(); 
        MSG_HIGH("JPEG: Sent DSP encode command", 0 , 0, 0);

        JpegeState = JPEGE_ENCODE_STATE;
      }
    }
    else
    {
      MSG_ERROR("Null JPEG Encoder data pointer ", 0 , 0, 0);
      returnStatus = JPEGQDSP_INVALID_PARM;
    }
    break;

  case JPEGE_OUTPUT_CONSUMED:                 /* Start decode the image  */
    if(JpegeState != JPEGE_ENCODE_STATE)
    {
      return JPEGQDSP_INVALID_STATE;
    }

    DSP_commandReg = qdsp_read(QDSP_jpegeCommandReg);
    code = DSP_commandReg & CODE_MASK;
    if( code )
    {
      MSG_HIGH("JPEGE: Command register was not 0", 0, 0, 0);
      return  JPEGQDSP_QDSP_BUSY;
    }

    if(pCmd)
    {
      pOutputData = (JPEGE_OutBufCfgType *) pCmd;

      if(!(pOutputData->Ptr) || !(pOutputData->Size))
      {
        MSG_ERROR("Invalid output consumed data ", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }
      else
      {
        MSG_HIGH("JPEGE: Consummed ptr %d, Size %d", pOutputData->Ptr
                 ,pOutputData->Size, 0);
        /* Set output consumed data */
        qdsp_block_write32(QDSP_jpegeCommandBuf, 0,(uint32 *) pCmd, 2, FALSE);

        DSP_commandReg = 2;
        DSP_commandReg |= OUTPUT_CONSUME_CODE;

        /* Set output consumed command ID */
        qdsp_write(QDSP_jpegeCommandReg, DSP_commandReg);

        /* Assert ARM to DSP interrupt */
        qdsp_up_to_dsp_irq();
      }
    }
    else
    {
      MSG_ERROR("Null JPEG output consumed pointer encouter", 0 , 0, 0);
      returnStatus = JPEGQDSP_INVALID_PARM;
    }

    break;

  case JPEGE_IDLE:                /* Stop the QDSP from encoding */
    if(JpegeState == JPEGE_RESET_STATE)
    {
      return JPEGQDSP_INVALID_STATE;
    }
    /* Check register to make sure DSP is ready for new command */
    DSP_commandReg = qdsp_read(QDSP_jpegeCommandReg);
    code = DSP_commandReg & CODE_MASK;
    if( code )
    {
      MSG_HIGH("JPEGE: Command register was not 0", 0, 0, 0);
      return  JPEGQDSP_QDSP_BUSY;
    }
    else
    {
      DSP_commandReg = IDLE_CMD_CODE;

      /* Set idle command ID */
      qdsp_write(QDSP_jpegeCommandReg, DSP_commandReg);

      /* Assert ARM to DSP interrupt */
      qdsp_up_to_dsp_irq();
    }

    break;

/* ===================================================================*/                                 
/* The following command enum are for the JPEG decoder                */
/* ===================================================================*/                                 
  case JPEGD_DECODE_CMD:

    if(JpegdState != JPEGD_IDLE_STATE)
    {
      return JPEGQDSP_INVALID_STATE;
    }

    MSG_HIGH("JPEG: Prepare to send decode command", 0, 0, 0);

    //Set up necessary parameters to command the decoder
    DSP_commandReg = qdsp_read(QDSP_jpegdCommandReg);
    code = DSP_commandReg & CODE_MASK;
    if( code )
    {
      MSG_HIGH("JPEG: Command register was not 0", 0, 0, 0);
      returnStatus = JPEGQDSP_QDSP_BUSY;
    }

    if(pCmd)
    {
      pDecodeCfgData = (JPEGD_decodeCmdType *)pCmd;

      if(!pDecodeCfgData->WidthHeight)
      {
        MSG_ERROR("Invalid parameters detected", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }

      if(!(pDecodeCfgData->Input.Ptr) ||!(pDecodeCfgData->Input.Size))
      {
        MSG_ERROR("Invalid input buffer detected", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }

      for(cnt=0; cnt<2; cnt++)
      {
        if(!(pDecodeCfgData->Out[cnt].LumaPtr)||
           !(pDecodeCfgData->Out[cnt].Size)||
           !(pDecodeCfgData->Out[cnt].ChromaPtr))
        {
          MSG_ERROR("Invalid output buffers detected", 0 , 0, 0);
          return JPEGQDSP_INVALID_PARM;
        }
      }

      /* Copy all Huffman tables to DSP here */
      if( !(pDecodeCfgData->h_codeCount[0]) ||
          !(pDecodeCfgData->h_codeCount[1]) ||
          !(pDecodeCfgData->h_codeCount[2]) ||
          !(pDecodeCfgData->h_codeCount[3]) )
      {
        MSG_ERROR("Invalid Huff code table detected", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }
      else
      {
        for(cnt =0; cnt<16; cnt++)
        {
          qdsp_write_offset(QDSP_jpegdLumaDcHuffCodeCount,  cnt,
                            pDecodeCfgData->h_codeCount[0][cnt+1]);
          qdsp_write_offset(QDSP_jpegdChromaDcHuffCodeCount,cnt,
                            pDecodeCfgData->h_codeCount[1][cnt+1]);
          qdsp_write_offset(QDSP_jpegdLumaAcHuffCodeCount,  cnt,
                            pDecodeCfgData->h_codeCount[2][cnt+1]);
          qdsp_write_offset(QDSP_jpegdChromaAcHuffCodeCount,cnt,
                            pDecodeCfgData->h_codeCount[3][cnt+1]);
        }
      }

      if( !(pDecodeCfgData->h_dcCodeValue[0]) ||
          !(pDecodeCfgData->h_dcCodeValue[1]) )
      {
        MSG_ERROR("Invalid Huff DC code table detected", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }
      else
      {
        for(cnt =0; cnt<12; cnt++)
        {
          qdsp_write_offset(QDSP_jpegdLumaDcHuffCodeValue,  cnt,
                            pDecodeCfgData->h_dcCodeValue[0][cnt]);
          qdsp_write_offset(QDSP_jpegdChromaDcHuffCodeValue,cnt,
                            pDecodeCfgData->h_dcCodeValue[1][cnt]);
        }
      }

      if( !(pDecodeCfgData->h_acCodeValue[0]) ||
          !(pDecodeCfgData->h_acCodeValue[1]) )
      {
        MSG_ERROR("Invalid Huff AC code table detected", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }
      else
      {
        for(cnt =0; cnt<162; cnt++)
        {
          qdsp_write_offset(QDSP_jpegdLumaAcHuffCodeValue,  cnt,
                            pDecodeCfgData->h_acCodeValue[0][cnt]);
          qdsp_write_offset(QDSP_jpegdChromaAcHuffCodeValue,cnt,
                            pDecodeCfgData->h_acCodeValue[1][cnt]);
        }
      }

      /* Check for Quant and Huffman tables */
      if( !(pDecodeCfgData->DQT_table[0]) ||
          !(pDecodeCfgData->DQT_table[1]) )
      {
        MSG_ERROR("Invalid DQT table detected", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }
      else
      {
        /* copy the quantization tables to the QDSP */
        qdsp_block_write(QDSP_jpegdQuantTableLuma, 0,
                         pDecodeCfgData->DQT_table[0], 64, FALSE);

        qdsp_block_write(QDSP_jpegdQuantTableChroma, 0,
                         pDecodeCfgData->DQT_table[1], 64, FALSE);

        cnt = 12; /* number of 32 bit words for decode command buffer */

        /* Copy the command buffer data to QDSP */
        qdsp_block_write32(QDSP_jpegdCommandBuf, 0,(uint32 *) pCmd, cnt, FALSE);

        DSP_commandReg = cnt;
        DSP_commandReg |= DECODE_CMD_CODE; /* Jpeg decode command */

        /* Set Decode command ID */
        qdsp_write(QDSP_jpegdCommandReg, DSP_commandReg);

        /* Assert ARM to DSP interrupt */
        qdsp_up_to_dsp_irq(); 
        MSG_HIGH("JPEG: Sent DSP decode command", 0 , 0, 0);

        JpegdState = JPEGD_DECODE_STATE;
      }
    }
    else
    {
      MSG_ERROR("Null JPEG decoder data pointer ", 0 , 0, 0);
      returnStatus = JPEGQDSP_INVALID_PARM;
    }
    break;

  case JPEGD_IDLE:
    if(JpegdState == JPEGD_RESET_STATE)
    {
      return JPEGQDSP_INVALID_STATE;
    }

    /* Check register to make sure DSP is ready for new command */
    DSP_commandReg = qdsp_read(QDSP_jpegdCommandReg);
    code = DSP_commandReg & CODE_MASK;
    if( code )
    {
      MSG_HIGH("JPEGD: Command register was not 0", 0, 0, 0);
      return  JPEGQDSP_QDSP_BUSY;
    }
    else
    {
      DSP_commandReg = DECODE_IDLE_CMD_CODE;

      /* Set idle command ID */
      qdsp_write(QDSP_jpegdCommandReg, DSP_commandReg);

      /* Assert ARM to DSP interrupt */
      qdsp_up_to_dsp_irq();
    }
    break;

  case JPEGD_OUTPUT_CONSUMED: 
    if(JpegdState != JPEGD_DECODE_STATE)
    {
      return JPEGQDSP_INVALID_STATE;
    }

    DSP_commandReg = qdsp_read(QDSP_jpegdCommandReg);
    code = DSP_commandReg & CODE_MASK;
    if( code )
    {
      MSG_HIGH("JPEGD: Command register was not 0", 0, 0, 0);
      return  JPEGQDSP_QDSP_BUSY;
    }

    if(pCmd)
    {
      pDecOutputData = (JPEGD_OutBufType *) pCmd;

      if(!(pDecOutputData->LumaPtr) || !(pDecOutputData->Size) 
         || !(pDecOutputData->ChromaPtr))
      {
        MSG_ERROR("Invalid output consumed data ", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }
      else
      {
        /* Set output consumed data */
        qdsp_block_write32(QDSP_jpegdCommandBuf, 0,(uint32 *) pCmd, 3, FALSE);

        DSP_commandReg = 3;
        DSP_commandReg |= DECODE_OUTPUT_CONSUME_CODE;

        /* Set output consumed command ID */
        qdsp_write(QDSP_jpegdCommandReg, DSP_commandReg);

        /* Assert ARM to DSP interrupt */
        qdsp_up_to_dsp_irq();
      }
    }
    break;

  case JPEGD_INPUT_CMD:
    if(JpegdState != JPEGD_DECODE_STATE)
    {
      return JPEGQDSP_INVALID_STATE;
    }

    DSP_commandReg = qdsp_read(QDSP_jpegdCommandReg);
    code = DSP_commandReg & CODE_MASK;
    if( code )
    {
      MSG_HIGH("JPEGD: Command register was not 0", 0, 0, 0);
      return  JPEGQDSP_QDSP_BUSY;
    }

    if(pCmd)
    {
      pDecInputData = (JPEGD_InputBufType *) pCmd;

      if(!(pDecInputData->Ptr) || !(pDecInputData->Size) )
      {
        MSG_ERROR("Invalid input buffer ", 0 , 0, 0);
        return JPEGQDSP_INVALID_PARM;
      }
      else
      {
        /* Set output consumed data */
        qdsp_block_write32(QDSP_jpegdCommandBuf, 0,(uint32 *) pCmd, 3, FALSE);

        DSP_commandReg = 3;
        DSP_commandReg |= DECODE_CONTINUE_CODE;

        /* Set output consumed command ID */
        qdsp_write(QDSP_jpegdCommandReg, DSP_commandReg);

        /* Assert ARM to DSP interrupt */
        qdsp_up_to_dsp_irq();
      }
    }
    break;

  default:

    returnStatus = JPEGQDSP_INVALID_CMD;
    MSG_ERROR("Un-recognized command", 0, 0, 0);
  }

  return returnStatus;
} /* jpegqdsp_command */

/*===========================================================================

FUNCTION JPEGE_Q2A_ISR

DESCRIPTION

  Depends on the current stage of operation, this function notifies ARM the
  state of QDSP, including encoder output produced, encoder idle complete
  and encoder illegal messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void jpege_q2a_isr( void)
{
  uint32  tempReg, code;

  /* We need to call this function to wake up the 5-ms tick to update
   * clk_uptime_val. This value is used by BREW to calculate the amount
   * of time spent in or out of BREW. If this value is not updated correctly,
   * BREW might think it spend more time than allowed. */

#ifdef FEATURE_REX_PREEMPT_INTS
  clk_unexpected_enable();
#endif /* FEATURE_REX_PREEMPT_INTS */

  /* Keep this code in case there is a need for DSP interrupt debugging 
  MSG_HIGH ("JPEGE: DSP sent ISR",0,0,0);
  */

  /* Check DSP encoder's output ready */
  tempReg = qdsp_read(QDSP_jpegeMsgOutputProducedReg);

  code = tempReg & CODE_MASK;
  if(code == CODE_MSG )
  {
    /* Output buffer is ready */
    JPEGE_Message.Code = JPEGE_OUTPUT_PRODUCED;     
    /* Buffer address of the new output buffer */
    JPEGE_Message.Msg.Buf.Ptr   =(uint8 *) qdsp_read(QDSP_jpegeMsgOutputProducedBuf);
    /* Buffer size in bytes */
    JPEGE_Message.Msg.Buf.Size  = qdsp_read_offset(QDSP_jpegeMsgOutputProducedBuf,1);
    /* Cumulative DC bit count */
    JPEGE_Message.Msg.Buf.DC_count = qdsp_read(QDSP_jpegeNumBitsHuffDc);
    /* Status of encoding - 0: more to come; 1: Done with image encoding */
    JPEGE_Message.Msg.Buf.Status= qdsp_read_offset(QDSP_jpegeMsgOutputProducedBuf,2);

    MSG_HIGH ("JPEGE: OutBuf ptr %d Size %d, Status %d",JPEGE_Message.Msg.Buf.Ptr
              ,JPEGE_Message.Msg.Buf.Size,JPEGE_Message.Msg.Buf.Status);

    if( JPEGEqdsp_CB )
    {
      JPEGEqdsp_CB(&JPEGE_Message);
    }
    else
    {
      MSG_HIGH ("JPEGE: Null CB pointer",0,0,0);
    }

    qdsp_write(QDSP_jpegeMsgOutputProducedReg, 0);

    MSG_LOW ("JPEGE: new buffer ready",0,0,0);
  }

  /* Check DSP encoder's idle message */
  tempReg = qdsp_read(QDSP_jpegeMsgIdleCompleteReg);
  code = tempReg & CODE_MASK;
  if(code == CODE_MSG )
  {
    /* DSP is in idle state */
    JPEGE_Message.Code = JPEGE_IDLE_COMPLETE;     

    if(ack_reset == TRUE)
    {
      qdsp_disable_event_rsp( QDSP_APP_JPEGE );
      ack_reset = FALSE;
    }

    if( JPEGEqdsp_CB )
    {
      JPEGEqdsp_CB(&JPEGE_Message);
    }
    else
    {
      MSG_HIGH ("JPEGE: Null CB pointer",0,0,0);
    }
    JpegeState = JPEGE_IDLE_STATE;

    /* Clear the DSP idle complete register */
    qdsp_write(QDSP_jpegeMsgIdleCompleteReg, 0);

    MSG_LOW ("JPEGE: DSP enters idle state",0,0,0);
  }

  /* Check DSP encoder's illegal message */
  tempReg = qdsp_read(QDSP_jpegeMsgIllegalCommandReg);

  code = tempReg & CODE_MASK;
  if(code == CODE_MSG )
  {
    JPEGE_Message.Code = JPEGE_ILLEGAL_CMD;     

    if( JPEGEqdsp_CB )
    {
      JPEGEqdsp_CB(&JPEGE_Message);
    }
    else
    {
      MSG_HIGH ("JPEGE: Null CB pointer",0,0,0);
    }
    MSG_HIGH ("JPEGE: DSP says illegal command",0,0,0);

    /* Clear the DSP illegal command register */
    qdsp_write(QDSP_jpegeMsgIllegalCommandReg, 0);
  }

} /* jpege_q2a_isr */
/*===========================================================================

FUNCTION JPEGE_Q2A_CALLBACK

DESCRIPTION
  This function will be called after the qdsp_enable function has
  completed.  This function is registered as a call back using the
  qdsp_register function.

DEPENDENCIES
  Must be registered with qdsp_register

RETURN VALUE

SIDE EFFECTS: modified JPEGE_Message, JpegeState 
  
===========================================================================*/
LOCAL void jpege_q2a_callback
(
  qdsp_event_data_type *event_ptr
)
{
  switch(event_ptr->event)
  {
  case QDSP_EV_COMMAND:
    MSG_MED("Received QDSP command event (jpege)",0,0,0);
    break;

#ifdef FEATURE_QDSP_USES_MODULES
  case QDSP_EV_MODULE:
    MSG_MED("Received QDSP module event (jpege)",0,0,0);

    if(event_ptr->mod.mod_status == QDSP_MOD_READY)
    {
      if(event_ptr->mod.module == QDSP_MODULE_JPEGE)
      {
        if(JpegeState == JPEGE_RESET_STATE)
        {
          MSG_MED( "JPEGE: image READY event", 0, 0, 0 );

          /* setup to execute the jpeg decoder with different state */
          JPEGE_Message.Code = JPEGE_IMAGE_READY; 
          JpegeState = JPEGE_IDLE_STATE;

          if( JPEGEqdsp_CB )
          {
            JPEGEqdsp_CB(&JPEGE_Message);
          }
          else
          {
            MSG_HIGH ("JPEGE: Null CB pointer",0,0,0);
          }
        }
      }
      else
      {
        MSG_HIGH( "JPEGE: unrecognized image %d ready",0,0,0);
      }
    }
    else
    {
      if( JpegeState == JPEGE_ENCODE_STATE )
      {
        ack_reset = TRUE;
        jpeg_encode_abort = TRUE;
        jpeg_qdsp_command(JPEGE_IDLE, NULL);
        MSG_HIGH("Disable event for JPEGE from QDSP services", 0,0,0);
      }
      else
      {
        qdsp_disable_event_rsp( QDSP_APP_JPEGE );
      }
    }
    break;
#else

  case QDSP_EV_MODULE:
    MSG_MED("Received QDSP module event (jpege)",0,0,0);
    break;

  case QDSP_EV_IMAGE:
    if(event_ptr->img.image_status == QDSP_IMAGE_READY)
    {
      if(event_ptr->img.image == QDSP_IMAGE_JPEGE_6550)
      {
        if(JpegeState == JPEGE_RESET_STATE)
        {
#ifdef JPEG_TIMING_TEST
          outpw(TIMETEST_PORT, 0xf0);
#endif
          MSG_MED( "JPEGE:image READY event", 0, 0, 0 );
          JpegeState = JPEGE_IDLE_STATE;

          /* setup to execute the jpeg decoder with different state */
          JPEGE_Message.Code = JPEGE_IMAGE_READY;     
          JpegeState = JPEGE_IDLE_STATE;

          if( JPEGEqdsp_CB )
          {
            JPEGEqdsp_CB(&JPEGE_Message);
          }
          else
          {
            MSG_HIGH ("JPEGE: Null CB pointer",0,0,0);
          }
        }/* if (JpegState   */
      }
      else
      {
        MSG_HIGH( "JPEGE: unrecognized image %d ready",event_ptr->img.image, 0, 0 );
      }
    }
    else if( event_ptr->img.image_status == QDSP_IMAGE_RESET )
    {
      if( event_ptr->img.image == QDSP_IMAGE_JPEGE )
      {
        if( JpegeState == JPEGE_ENCODE_STATE )
        {
          /* make sure to set the disable flag accordinly */
          jpege_terminate();
        }
        MSG_MED("MM allow reset %d", event_ptr->img.image,0,0);
        qdsp_allow_reset( QDSP_APP_JPEGE );
      }
      else
      {
        qdsp_allow_reset( QDSP_APP_JPEGE );
      }
    }

    break;
#endif /* FEATURE_QDSP_USES_MODULES */
  default:
    MSG_ERROR("Illegal QDSP Event type (jpege)",0,0,0);
    break;
  }
} /*lint !e818 event_ptr could be declared as pointing to const 
   * but it would involve changing the interface to outside */
/* jpege_q2a_callback */

/*===========================================================================

FUNCTION JPEGE_TERMINATE

DESCRIPTION

  This operation terminates the DSP encoder from operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void jpege_terminate( void)
{
#ifdef JPEG_VIDEO_RESOURCES
  video_resources_clock_off(VIDEO_RESOURCES_CLIENT_JPEGE,CLK_RGM_VIDEO_M);
#endif
  qdsp_disable(QDSP_APP_JPEGE);
  current_qdsp_jpeg_app = QDSP_APP_MAX;
}


/*===========================================================================

FUNCTION JPEGD2_Q2A_ISR

DESCRIPTION

  Depends on the current stage of operation, this function notifies ARM the
  state of QDSP, including decoder output produced, decoder idle complete, 
  decoder wait and decoder illegal messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS: modified JPEGE_Message, JpegdState. 
  
===========================================================================*/
LOCAL void jpegd2_q2a_isr( void)
{
  uint32  tempReg, code;
  
  /* We need to call this function to wake up the 5-ms tick to update
   * clk_uptime_val. This value is used by BREW to calculate the amount
   * of time spent in or out of BREW. If this value is not updated correctly,
   * BREW might think it spend more time than allowed. */
#ifdef FEATURE_REX_PREEMPT_INTS
  clk_unexpected_enable();
#endif /* FEATURE_REX_PREEMPT_INTS */

  MSG_LOW ("JPEGD: DSP sent ISR",0,0,0);

  /* Check DSP decoder's output ready */
  tempReg = qdsp_read(QDSP_jpegdMsgOutputProducedReg);

  code = tempReg & CODE_MASK;
  if(code == CODE_MSG )
  {
    /* Output buffer is ready */
    JPEGE_Message.Code = JPEGD_OUTPUT_PRODUCED;     
    /* Luma buffer address of the new output buffer */
    JPEGE_Message.Msg.DecBuf.LumaPtr =(uint8 *) qdsp_read(QDSP_jpegdMsgOutputProducedBuf);
    /* Luma buffer size in bytes */
    JPEGE_Message.Msg.DecBuf.ChromaPtr  =(uint8 *) qdsp_read_offset(QDSP_jpegdMsgOutputProducedBuf,1);
    /* Chroma buffer address of the new output buffer */
    JPEGE_Message.Msg.DecBuf.Luma_size = qdsp_read_offset(QDSP_jpegdMsgOutputProducedBuf,2);
    /* Get next long word - error code and decode flag */
    tempReg = qdsp_read_offset(QDSP_jpegdMsgOutputProducedBuf,3);
    /* Extract error code */
    JPEGE_Message.Msg.DecBuf.Decode_err = (tempReg & CODE_MASK) >> 16;

    if(JPEGE_Message.Msg.DecBuf.Decode_err)
    {
      MSG_ERROR ("JPEGD: DSP detects error",0,0,0);
    }
    /* Status of decoding - 0: more to come; 1: Done with JPEG decoding */
    JPEGE_Message.Msg.DecBuf.Dec_done = (uint16) tempReg;

    if( JPEGDqdsp_CB )
    {
      JPEGDqdsp_CB(&JPEGE_Message);
    }
    else
    {
      MSG_HIGH ("JPEGD: Null CB pointer",0,0,0);
    }

    qdsp_write(QDSP_jpegdMsgOutputProducedReg, 0);

    MSG_LOW ("JPEGD: new buffer ready",0,0,0);

  }

  /* Check DSP decoder's in waiting state */
  tempReg = qdsp_read(QDSP_jpegdMsgInputRequestReg);
  code = tempReg & CODE_MASK;
  if(code == CODE_MSG )
  {
    /* DSP is in wait state */
    JPEGE_Message.Code = JPEGD_INPUT_REQUEST;     

    if( JPEGDqdsp_CB )
    {
      JPEGDqdsp_CB(&JPEGE_Message);
    }
    else
    {
      MSG_HIGH ("JPEGD: Null CB pointer",0,0,0);
    }
    
    /* Clear to send acknowledgement to DSP */
    qdsp_write(QDSP_jpegdMsgInputRequestReg, 0);

    MSG_LOW ("JPEGD: DSP enters wait state",0,0,0);

  }

  /* Check DSP decoder's idle message */
  tempReg = qdsp_read(QDSP_jpegdMsgIdleCompleteReg);
  code = tempReg & CODE_MASK;
  if(code == CODE_MSG )
  {
    /* DSP is in idle state */
    JPEGE_Message.Code = JPEGD_IDLE_COMPLETE;     

    if(ack_reset == TRUE)
    {
      qdsp_disable_event_rsp( QDSP_APP_JPEGD );
      ack_reset = FALSE;
    }

    if( JPEGDqdsp_CB )
    {
      JPEGDqdsp_CB(&JPEGE_Message);
    }
    else
    {
      MSG_HIGH ("JPEGD: Null CB pointer",0,0,0);
    }
    JpegdState = JPEGD_IDLE_STATE;
    /* Clear to send acknowledgement to DSP */
    qdsp_write(QDSP_jpegdMsgIdleCompleteReg, 0);

    MSG_LOW ("JPEGD: DSP enters idle state",0,0,0);

  }

  /* Check DSP decoder's illegal message */
  tempReg = qdsp_read(QDSP_jpegdMsgIllegalCommandReg);

  code = tempReg & CODE_MASK;
  if(code == CODE_MSG )
  {
    JPEGE_Message.Code = JPEGD_ILLEGAL_CMD;     

    if( JPEGDqdsp_CB )
    {
      JPEGDqdsp_CB(&JPEGE_Message);
    }
    else
    {
      MSG_HIGH ("JPEGD: Null CB pointer",0,0,0);
    }
    MSG_HIGH ("JPEGD: DSP says illegal command",0,0,0);

    /* Clear the register to send acknowledgement to DSP */ 
    qdsp_write(QDSP_jpegdMsgIllegalCommandReg, 0);
  }

} /* jpegd2_q2a_isr */
/*===========================================================================

FUNCTION JPEGD2_Q2A_CALLBACK

DESCRIPTION
  This function will be called after the qdsp_enable function has
  completed.  This function is registered as a call back using the
  qdsp_register function.

DEPENDENCIES
  Must be registered with qdsp_register

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void jpegd2_q2a_callback
(
qdsp_event_data_type *event_ptr
)
{
  switch(event_ptr->event)
  {
  case QDSP_EV_COMMAND:
    MSG_MED("Received QDSP command event (jpegd)",0,0,0);
    break;
#ifdef FEATURE_QDSP_USES_MODULES
  case QDSP_EV_MODULE:
    MSG_MED("Received QDSP module event (jpegd)",0,0,0);

    if(event_ptr->mod.mod_status == QDSP_MOD_READY)
    {
      if(event_ptr->mod.module == QDSP_MODULE_JPEGD)
      {
        if(JpegdState == JPEGD_RESET_STATE)
        {
          MSG_MED( "JPEGD: image READY event", 0, 0, 0 );

          /* setup to execute the jpeg decoder with different state */
          JPEGE_Message.Code = JPEGD_IMAGE_READY; 
          JpegdState = JPEGD_IDLE_STATE;

          if( JPEGDqdsp_CB )
          {
            JPEGDqdsp_CB(&JPEGE_Message);
          }
          else
          {
            MSG_HIGH ("JPEGD: Null CB pointer",0,0,0);
          }
        }
      }
      else
      {
        MSG_HIGH( "JPEGD: unrecognized image %d ready",0,0,0);
      }
    }
    else
    {
      if( JpegdState == JPEGD_DECODE_STATE )
      {
        ack_reset = TRUE;
        jpeg_decode_abort = TRUE;
        jpeg_qdsp_command(JPEGD_IDLE, NULL);      
        MSG_HIGH("Disable event for JPEGD from QDSP services", 0,0,0);
      }
      else
      {
        qdsp_disable_event_rsp( QDSP_APP_JPEGD );
      }
    }
    break;

#else

  case QDSP_EV_MODULE:
    MSG_MED("Received QDSP module event (jpegd)",0,0,0);
    break;

  case QDSP_EV_IMAGE:
    if(event_ptr->img.image_status == QDSP_IMAGE_READY)
    {
      if(event_ptr->img.image == QDSP_IMAGE_JPEGD_6550)
      {
        if(JpegdState == JPEGD_RESET_STATE)
        {
#ifdef JPEG_TIMING_TEST
          outpw(TIMETEST_PORT, 0xf0);
#endif
          MSG_MED( "JPEGD:image READY event", 0, 0, 0 );
          JpegdState = JPEGD_IDLE_STATE;

          /* setup to execute the jpeg decoder with different state */
          JPEGE_Message.Code = JPEGD_IMAGE_READY;     
          JpegdState = JPEGD_IDLE_STATE;

          if( JPEGDqdsp_CB )
          {
            JPEGDqdsp_CB(&JPEGE_Message);
          }
          else
          {
            MSG_HIGH ("JPEGD: Null CB pointer",0,0,0);
          }

        }/* if (JpegState   */
      }
      else
      {
        MSG_HIGH( "JPEGD: unrecognized image %d ready",
                  event_ptr->img.image, 0, 0 );
      }
    }
    else if( event_ptr->img.image_status == QDSP_IMAGE_RESET )
    {
      if( event_ptr->img.image == QDSP_IMAGE_JPEGD )
      {
        if( JpegdState == JPEGD_DECODE_STATE )
        {
          /* make sure to set the disable flag accordinly */
          jpegd2_terminate();
        }
        MSG_MED("MM allow reset %d", event_ptr->img.image,0,0);
        qdsp_allow_reset( QDSP_APP_JPEGD );
      }
      else
      {
        qdsp_allow_reset( QDSP_APP_JPEGD );
      }
    }
    break;
#endif /* FEATURE_QDSP_USES_MODULES */
  default:
    MSG_ERROR("Illegal QDSP Event type (jpegd)",0,0,0);
    break;
  }
} /*lint !e818 event_ptr could be declared as pointing to const 
   * but it would involve changing the interface to outside */
/* jpegd2_q2a_callback */

/*===========================================================================

FUNCTION JPEGD_TERMINATE

DESCRIPTION

  This operation terminates the DSP decoder from operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void jpegd2_terminate( void)
{
#if defined(JPEG_VIDEO_RESOURCES) && defined(JPEGD_VIDEO_RESOURCES)
#error code not present
#endif
  qdsp_disable(QDSP_APP_JPEGD);
  current_qdsp_jpeg_app = QDSP_APP_MAX;
}


#endif /* FEATURE_JPEG_DECODER_REV2 || FEATURE_JPEG_ENCODER_REV2 */

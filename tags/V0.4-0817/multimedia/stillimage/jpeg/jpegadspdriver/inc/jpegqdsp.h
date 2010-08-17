#ifndef JPEGQDSP_H
#define JPEGQDSP_H

/*=============================================================================
FILE:       JPGQDSP_DRV.h

SERVICES:   JPEG QDSP Driver Interface

DESCRIPTION:
   This file provides necessary interfaces for the JPEG interfaces, 
   JPEG encoder and JPEG decoder for the MSM6550.
   
   This file provides the necessary interfaces for the JPEG encoder (JPEGE) and 
   the JPEG decoder (JPEGD) 

LIMITATIONS:
   jpeg_qdsp_initialize() must be called first.


INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegadspdriver/main/latest/inc/jpegqdsp.h#1 $ $DateTime: 2008/05/09 16:22:45 $ $Author: karenp $

============================================================================*/
/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/05   arv     Fixed a compilation issue for ThinUI builds
11/23/04   pdo     Added suport for JPEG decoder
10/27/04   pdo     Enabled JPEGE file-size control parameters
07/07/04   pdo     Initial version

===========================================================================*/

/*---------------------------------------------------------------------------
      Include Files
----------------------------------------------------------------------------*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

/*---------------------------------------------------------------------------
      Constant Declarations
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
      Defines for category type
----------------------------------------------------------------------------*/
/*                                                                          */
/* The following are definition for JPEG decoder rev2                       */
/*                                                                          */
#ifdef FEATURE_JPEG_DECODER
/*
** Define the enum type for the DSP messages 

typedef enum
{
  JPEGD_IMAGE_READY,
  JPEGD_OUTPUT_PRODUCED,
  JPEGD_IDLE_COMPLETE,
  JPEGD_WAITING,
  JPEGD_ILLEGAL_CMD
} JPEGD_msgType;
*/
/*
** Define the enum type for the ARM's commands 
typedef enum
{
  JPEGD_DECODE_CMD,
  JPEGD_IDLE,
  JPEGD_OUTPUT_CONSUMED,
  JPEGD_DECODE_CONTINUE
} JPEGD_commandType;
*/
/*
** Define the enum type for the ARM's commands 
*/
typedef enum
{
  JPEGD_RESET_STATE,
  JPEGD_DECODE_STATE,
  JPEGD_IDLE_STATE
} JPEGD_stateType;

/*
** Define the JPEGD output buffer configuration
*/
typedef struct
{
   uint8   *LumaPtr;           /* Luma decoded output buffer         */
   uint32   Size;              /* MCU count                          */
                               /* M=[floor(image_width/16)+0]*16     */
                               /* N=[floor(rows in fragment/16)+3]*16*/
   uint8   *ChromaPtr;         /* Chroma decoded output buffer       */
} JPEGD_OutBufType;

/*
** Define the JPEGD input buffer configurtion 
*/
typedef struct
{
   uint8   *Ptr;                /* Address of bit-stream intput data  */
   uint32   Size;               /* Size of buffer                     */
   uint32   End_BitStream;      /* 0 - more to come; 1 - this is last */
} JPEGD_InputBufType;

/*
** Define the JPEGD decode command buffer
*/
typedef struct
{
   uint32             WidthHeight;  /* Image height in pixel          */
   uint32             Subsample;    /* JPEG subsample, HxVx           */
   uint32             Restart;      /* restart inverval in MCU unit   */
   JPEGD_InputBufType Input ;       /* bit-stream input buffer        */
   JPEGD_OutBufType   Out[2];       /* Decoded output fragments       */
   uint16            *DQT_table[2];
   /* Huffman tables */
   uint8             *h_codeCount[4];  /* 0 - Luma   DC            */
                                       /* 1 - Chroma DC            */
                                       /* 2 - Luma   AC            */
                                       /* 3 - Chroma AC            */    
   uint8             *h_dcCodeValue[2];/* 0 - Luma DC code value   */
                                       /* 1 - Chroma DC code value */
   uint8             *h_acCodeValue[2];/* 0 - Luma AC code value   */
                                       /* 1 - Chroma AC code value */
} JPEGD_decodeCmdType;

/*
** Define the JPEGD output produced message
*/
typedef struct
{
   uint8            *LumaPtr;   /* pointer to the luma buffer         */
   uint32            Luma_size; /* Number of valid bytes in buffer    */   
   uint8            *ChromaPtr; /* pointer to the chroma buffer       */
   uint16            Decode_err;/* 0x0 - */
                                /* 0xFFFF - Huffman table error       */
                                /* 0xFFFE - bad output buffer         */
                                /* 0xXXXX - Huffman err in MCU 0xXXXX */
   uint16            Dec_done;  /* 0 - not done;  1 - all done        */
} JPEGD_outputMsgBufType;

/*
** Define a qdsp service info structure
*/
//typedef struct
//{
//   JPEGD_msgType          Code; /* message code to hint the content   */
//   JPEGD_outputMsgBufType Buf;  /* Message payload                    */
//} JPEGDMsgType;

/*
** QDSP message processing callback function structure
*/
//typedef void (* JPEGD_msgCBType)
//(
//  JPEGDMsgType  *pMsgInfo       /* qdsp service info                  */
//); 

#endif
/*                                                                          */
/* The following are definition for JPEG encoder rev2                       */
/*                                                                          */
/*
** Define the enum type for the DSP messages 
*/
typedef enum
{
  JPEGE_IMAGE_READY,
  JPEGE_OUTPUT_PRODUCED,
  JPEGE_IDLE_COMPLETE,
  JPEGE_ILLEGAL_CMD,
  /* JPEG decoder messages */
  JPEGD_IMAGE_READY,
  JPEGD_OUTPUT_PRODUCED,
  JPEGD_IDLE_COMPLETE,
  JPEGD_INPUT_REQUEST,
  JPEGD_ILLEGAL_CMD
} JPEGE_msgType;

/*
** Define the enum type for the DSP images 
*/
typedef enum
{
  JPEG_ENCODER_DSP,
  JPEG_DECODER_DSP
} JPEG_appImageType;

/*
** Define the enum type for the ARM's commands 
*/
typedef enum
{
  /* Encode commands */
  JPEGE_ENCODE_CMD,
  JPEGE_IDLE,
  JPEGE_OUTPUT_CONSUMED,
  /* Decode commands */
  JPEGD_DECODE_CMD,
  JPEGD_IDLE,
  JPEGD_OUTPUT_CONSUMED,
  JPEGD_INPUT_CMD
} JPEGE_commandType;

/*
** Define the enum type for the ARM's commands 
*/
typedef enum
{
  JPEGE_RESET_STATE,
  JPEGE_ENCODE_STATE,
  JPEGE_IDLE_STATE
} JPEGE_stateType;

/*
** Define common JPEG QDSP return code enumeration
*/
typedef enum
{
   JPEGQDSP_SUCCESS       = 0,  /* cmd sucessful */

   /* Download done return codes */
   JPEGQDSP_DOWNLD_FAIL,        /* qdsp video codec download failed */
   JPEGQDSP_INVALID_STATE,
   JPEGQDSP_QDSP_BUSY,

   /* cmd pointers errors */
   JPEGQDSP_INVALID_BUF,

   /* QDSP command error return codes */
   JPEGQDSP_INVALID_CMD,        /* invalid QDSP command specified */
   JPEGQDSP_INVALID_PARM        /* invalid QDSP command parameter */
} JPEGQDSPReturnCodeType;

/*
** Define the JPEGE command configuration
*/
/* This command type is to be copied to jpegeCommandReg               */
typedef struct
{
   unsigned int Size      : 16; /* Number of 32 bit words in buffer   */
   unsigned int Code      : 16; /* Command code                       */
                                /* 0x800 - encode command             */
                                /* 0x801 - idle command               */
                                /* 0x802 - output consumed command    */
} JPEGE_commandRegType;

/*
** Define the JPEGE Process configuration
*/
typedef struct
{
   unsigned int Subsample :  2; /* JPEG encoding format               */
                                /* 0 - H2V2                           */
                                /* 1 - H2V1 (0 & 180 rotation only)   */
                                /* 2 - H1V2 (90 & 270 rotation only)  */
                                /* 3 - Not used                       */
   unsigned int Reserved1 :  6; /* Reserved - set to 0                */
   unsigned int Rotation  :  2; /* Counter clockwise rotation         */
                                /* 0 -  no rotation                   */
                                /* 1 -  90 degrees CClockwise rotation*/
                                /* 2 - 180 degrees CClockwise rotation*/
                                /* 3 - 270 degrees CClockwise rotation*/
   unsigned int Fragments :  3; /* Number of fragments                */
   unsigned int Reserved2 :  3; /* Reserved - set to 0                */
   unsigned int Restart   : 16; /* Restart interval count,            */
                                /* 0 - no restart marker              */
} JPEGE_ProcessCfgType;

/*
** Define the JPEGE Fragment width configuration
*/
typedef struct
{
   unsigned int Width     : 12; /* Width of each fragment in pixel,Q0 */
                                /* Must be multiple of 16             */
                                /* larger than image width            */
   unsigned int Reserved  : 20; /* Reserved - Set to 0                */
} JPEGE_FragmentWidthType;

/*
** Define the JPEGE Fragment configuration
*/
typedef struct
{
   uint8   *Luma_ptr;           /* image format                       */
   uint8   *Chroma_ptr;         /* image format                       */
   unsigned int Height    : 12; /* Fragment height in pixel           */
   unsigned int Reserved  : 20; /* Reserved - set to 0                */
} JPEGE_FragmentCfgType;

/*
** Define the JPEGE Output buffer configurtion 
*/
typedef struct
{
   uint8   *Ptr;                /* Address of output encoded data     */
   uint32   Size;               /* Size of buffer                     */
} JPEGE_OutBufCfgType;

/*
** Define the JPEGE Encode command buffer
*/
typedef struct
{
   JPEGE_ProcessCfgType    Enc;        /* image format                */
   JPEGE_FragmentWidthType Img;        /* image format                */
   JPEGE_FragmentCfgType   Fragmnt[8]; /* Fragment height in pixel    */
   JPEGE_OutBufCfgType     Out[2];     /* Reserved - set to 0         */
   uint16               *LumaDQTPtr;   /* Luma quantization table          */
   uint16               *ChromaDQTPtr; /* Chroma quantization table        */
} JPEGE_encodeCmdType;



/*
** Define the JPEGE output produced message
*/
typedef struct
{
   uint8              *Ptr;
   uint32              Size;	     /* Number of valid bytes in buffer  */
   uint32              Status;       /* 0 - more to come; 1 - done encode*/
   uint32              DC_count;     /* DC bit count */
} JPEGE_outputMsgBufType;

/*
** Define a qdsp service info structure
*/
typedef struct
{
   JPEGE_msgType       Code;         /* message code to hint the content */
   union
   {
     uint32                 Data;    /* Data from DSP                    */
     JPEGE_outputMsgBufType Buf;     /* Message payload                  */
#ifdef FEATURE_JPEG_DECODER
     JPEGD_outputMsgBufType DecBuf;
#endif
   } Msg;
} JPEGQDSPMsgType;

/*
** QDSP message processing callback function structure
*/
typedef void (* JPEG_msgCBType)
(
  JPEGQDSPMsgType  *pMsgInfo             /* qdsp service info                */
);

/*===========================================================================*/
/* FUNCTION DEFINITIONS                                                      */
/*===========================================================================*/

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
);

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
);

/*===========================================================================

Function: JPEGE_MSG_ACK

Description: This function acknowledge the DSP messages.

Return Value: None

Comments:

==============================================================================*/
void jpege_msg_ack(JPEGE_msgType msg);

/*===========================================================================

Function: JPEGE_TERMINATE

Description: This function terminates the JPEG QDSP encoder.

Return Value: None

Comments:

Side Effects: None

==============================================================================*/
void jpege_terminate(void);

/*===========================================================================

Function: JPEGD_TERMINATE

Description: This function terminates the JPEG QDSP decoder.

Return Value: None

Comments:

Side Effects: None

==============================================================================*/
void jpegd2_terminate(void);


#endif /* JPEGQDSP_H */

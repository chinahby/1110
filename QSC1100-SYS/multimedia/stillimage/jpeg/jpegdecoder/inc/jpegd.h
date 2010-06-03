#ifndef JPEGD_H
#define JPEGD_H

/*=============================================================================
FILE:       JPEGD.h

SERVICES:   JPEG decoder Interface

DESCRIPTION:
   This file provides necessary interfaces for the JPEG decoder. 
   
   This file provides the necessary interfaces for the JPEG decoder. 

LIMITATIONS:
   

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegdecoder/main/latest/inc/jpegd.h#1 $ $DateTime: 2008/05/09 16:22:45 $ $Author: karenp $

============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/18/07   kdiv    Added support for YCbCr Jpegd DSP output
11/12/06   arv     Added resize feature for JPEG Decoder
04/06/06   arv     Requires a dummy enum for JPEGDEC_outFormatType for decoder's 
                   YCbCr output
03/13/06   vma     Fixed all Lint errors
11/22/05   arv     Featurized the JPEG Decoder for ThinUI builds
10/12/05   arv     Modified the code in accordance with the review feedback
09/19/05   arv     Replaced the jpeg_decoder_abort() with jpegd_decoder_abort()
07/14/05   jj      Added the jpegd_decoder_abort( uint16 ) declaration
                   which DIAG uses.
05/04/05   arv     Added the RGB888 format to JPEGDEC_outFormatType
11/23/04   pdo     Supported X16 image in H2V2 format.
10/22/04   pdo     Initial version

===========================================================================*/

/*---------------------------------------------------------------------------
      Include Files
----------------------------------------------------------------------------*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_JPEG_DECODER_REV2
#include "jpeg.h"


/*---------------------------------------------------------------------------
      Type Declarations
----------------------------------------------------------------------------*/
/*
** Define common JPEG QDSP return code enumeration
*/
typedef enum
{
   JPEGDEC_SUCCESS       = 0,      /* cmd sucessful */
   JPEGDEC_INVALID_PTR,            /* Invalid callback function */
   JPEGDEC_BUSY_STATE,
   JPEGDEC_BAD_DATA
} JPEGDECReturnCodeType;

/*
** Define the processing type for context switch callback 
*/
typedef enum
{
  JPEGDEC_DECODE_IMG,
  JPEGDEC_READ_BUFFER1,
  JPEGDEC_READ_BUFFER2,
  JPEGDEC_SEND_BUFFER1,
  JPEGDEC_SEND_BUFFER2,
  JPEGDEC_IDLE,
  JPEGDEC_DONE,
  JPEGDEC_OPER_MAX
} JPEGDEC_operationType;

/*
** Define the output mode type
*/
typedef enum
{
  JPEGDEC_H2V2 =0,
  JPEGDEC_H2V1,
  JPEGDEC_H1V2,
  JPEGDEC_H1V1
} JPEGDEC_formatType;

/*
** Define the output mode type
*/
typedef enum
{
  JPEGDEC_RGB565 = 0,
  JPEGDEC_RGB888,
  JPEGDEC_YCbCr422,
  JPEGDEC_YCbCr420,
  JPEGDEC_YCrCb420,
  JPEGDEC_YCrCb422
} JPEGDEC_outFormatType;

/*
** Define the output chroma type
*/
typedef enum
{
  JPEGDEC_CrCb = 0,
  JPEGDEC_CbCr
} JPEGDEC_outChromaFormatType;

typedef enum
{
  JPEGDEC_LUMA_DC_HUFFCODE_CNT_INDEX = 0,
  JPEGDEC_CHROMA_DC_HUFFCODE_CNT_INDEX,
  JPEGDEC_LUMA_AC_HUFFCODE_CNT_INDEX,
  JPEGDEC_CHROMA_AC_HUFFCODE_CNT_INDEX
} JPEGDEC_huffCodeType;

typedef enum
{
  JPEGDEC_LUMA_DC_HUFFCODE_VAL_INDEX = 0,
  JPEGDEC_CHROMA_DC_HUFFCODE_VAL_INDEX,
} JPEGDEC_huffDCCodeType;

typedef enum
{
  JPEGDEC_LUMA_AC_HUFFCODE_VAL_INDEX = 0,
  JPEGDEC_CHROMA_AC_HUFFCODE_VAL_INDEX
} JPEGDEC_huffACCodeType;

typedef enum
{
  JPEGDEC_LUMA_QUANTTABLE_INDEX = 0,
  JPEGDEC_CHROMA_QUANTTABLE_INDEX,
} JPEGDEC_QuantType;



/*
** Define the JPEG decoder specification
*/
typedef struct
{
   jpeg_cmx_cb_func_type   callBack;        /* this is the cmx callback */
   void                   *client_handle;
   uint16                  width;
   uint16                  height;
   uint16                  restart;
   JPEGDEC_formatType      subsample;
   JPEGDEC_outFormatType   outFormat;
#ifdef FEATURE_JPEGD_RESIZE   
    uint32     resizeImageWidth;
    uint32     resizeImageHeight;
    boolean    aspectRatioFlag;
    boolean    jpegdResizeFlag;
#endif
   uint32                  filesize;
   uint32                  bytes_read;
   /* Quantization tables */
   int16                  *q_table[2];
   /* Huffman tables */
   uint8                  *h_codeCount[4];  /* 0 - Luma   DC            */
                                            /* 1 - Chroma DC            */
                                            /* 2 - Luma   AC            */
                                            /* 3 - Chroma AC            */    
   uint8                  *h_dcCodeValue[2];/* 0 - Luma DC code value   */
                                            /* 1 - Chroma DC code value */
   uint8                  *h_acCodeValue[2];/* 0 - Luma AC code value   */
                                            /* 1 - Chroma AC code value */
   JPEGDEC_outChromaFormatType chromaType;
} JPEGDEC_decodeSpecType;

/*
** Define the JPEG decoder buffer specification
*/
typedef struct
{
   uint8                  *bufPtr;
   uint8                  *currentPtr;
   uint32                  bufSize;       /* bufSize = 2 * MAX_DATA_BYTES */
   uint32                  dataSize;      /* dataSize = fileSize - bytesRead */  
} JPEGDEC_bufferInfoType;


/* JPEG Decoder Timing */
typedef struct
{
  uint32 opm_received;
  uint32 occ_sent;
  uint32 armresize_start;
  uint32 armresize_stop;
} JPEGDEC_timing;

/*===========================================================================*/
/* FUNCTION DEFINITIONS                                                      */
/*===========================================================================*/

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

Side Effects: None

==============================================================================*/
JPEGDECReturnCodeType   jpegd_dsp_dec_rev2
(
  uint32                   clientId, 
  const JPEGDEC_decodeSpecType  *decodeInfo,  /* decode specification   */
  const JPEGDEC_bufferInfoType  *bufInfo 
);

/*===========================================================================

Function: JPEGD_DECODER_ABORT

Description:  This function is used to abort the on going decoding session.
              
Return Value:
   JPEGDEC_SUCCESS      - cmd sucessful
   JPEGDEC_BAD_ID       - Bad client ID for this command
   
Comments: If JPEGDEC_SUCCESS, caller should wait for the callback from the
           decoding session to properly abort the session. 

Side Effects:

============================================================================*/
void  jpegd_decoder_abort
(
   uint16      ClientId                 /* Use the same client ID for the   */
);                                      /*  encoding session                */

/*===========================================================================

Function: JPEGD_CALCULATEPIXELASPECTRATIO

Description:  Caclulates pixel aspect ratio
              
Return Value:
    TRUE        - 
    FALSE       - 
   
Comments: 

Side Effects:

============================================================================*/
#ifdef FEATURE_JPEGD_RESIZE
boolean jpegd_calcPixelAspectRatio
(
        void 
 );
#endif

/*===========================================================================

Function: JPEGD_CALCSCALINGFACTOR

Description:  Calculates scaling factor

Return Value:
  NONE

Comments:

Side Effects:

============================================================================*/
#ifdef FEATURE_JPEGD_RESIZE
void jpegd_calcScalingFactor
(
      void
);

/*===========================================================================

Function: JPEGD_CHECKRESIZEDIMENSIONS

Description:  Calculates resize dimensions

Return Value:
  NONE

Comments:

Side Effects:

============================================================================*/
boolean jpegd_checkResizeDimensions
(
      void
);
#endif
#endif /* FEATURE_JPEG_DECODER_REV2 */
#endif /* JPEGD_H */

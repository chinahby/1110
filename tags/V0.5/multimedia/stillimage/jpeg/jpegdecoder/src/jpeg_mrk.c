/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
                             D E C O D E R

GENERAL DESCRIPTION
  This module contains functions to decode Joint Photographers Expert Group 
  (JPEG) image files.

EXTERNALIZED FUNCTIONS
  NONE

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegdecoder/main/latest/src/jpeg_mrk.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/27/09   RK      Modified fetchbits function to support the case where the remaining 
                   bits are less than required number of bits
06/08/07   arv     Added featurization for jpegdBoundaryIndex
04/27/07   arv     Factored the SOS length while recalculating the dsp bit stream pointer
04/16/07   arv     Check for component selector values to avoid possible security 
                   exploits
03/27/07   arv     Fixed the issue where the input data pointer can get incremented
                   beyond the boundary buffer for dsp decoding
                   Fixed another issue related to byteoffset overflow
02/13/07   arv     Fixed the issue where the byteoffset can overflow for certain
                   cases
07/18/06   arv     Fixed another corner case related to fetchByte function
06/16/06   arv     Fixed the issue related to image having no quantization tables
04/06/06   arv     Check for table selector values to avoid possible security exploits
03/13/06   vma     Fixed all Lint errors
01/09/06   arv     Fixed the issue for progressive decoding where the AC Scan 
                   index and component selector were passed incorrectly
12/22/05   arv     Fixed the issue where the input bistream ptr is incorrectly pointing 
                   to the second byte after end of SOS Header which results in passing 
                   incorrect input bitstream to DSP decoder.Aligned the bitstream pointer 
                   based on the bitsRemaining parameter.
11/28/05   arv     Fixed Lint errors - Phase 1
10/28/05   arv     Modified the recedeImagePtr() to fix the first half of 
                   inputDataBuffer boundary issue
10/26/05   arv     Fixed the issue where the restart marker can occur at
                   the boundary of input data buffer that causes the decoding
                   to fail
09/09/05   arv     Handle gracefully in case of unsupported YUV 411 pictures 
06/10/05   arv     Initial version

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/




#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Common definitions                      */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */





#ifdef FEATURE_JPEG_DECODER


#include "jpegi.h"
#include "jpeg_mrk.h"
#include "graph.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Extern Variables need to be consolidated */
  
extern uint8   *in_dataPtr;
extern uint32   byteOffset;
extern uint16   jpegd_imageWidth;
extern uint16   jpegd_imageHeight;
extern uint32   jpegd_fileSize, total_byte_read;

#ifdef FEATURE_JPEG_DECODER_REV2
extern uint32   jpegd_totalBytesRead;
extern uint8   *jpegd_dspBstrmPtr;
#endif

extern int16    jpegd_quant[JPGD_MAXQUANTTABLES][64];                /* Pointer to quantization tables */

extern int32    bitsRemaining;
extern uint32   numberOfComponentsInFrame;
extern uint32   numberOfComponentsInScan;
extern uint8    frameCompID[JPGD_MAXCOMPONENTS];     
extern uint32   frameVSamplingFactor[JPGD_MAXCOMPONENTS];      /* Component horizontal sampling factor */
extern uint32   frameHSamplingFactor[JPGD_MAXCOMPONENTS];      /* Component vertical sampling factor */
extern uint8   *in_data_buf;
extern uint16   restart_interval;


extern void    *jpeg_cmx_handle;                 /* JPEG CMX handler */ 
extern jpeg_cmx_get_data_type  jpeg_get_data;        
extern jpeg_cmx_cb_func_type  jpeg_cmx_cb;       /* JPEG CMX callback */
extern JPEGD_STATUS_MSG jpegd_statusMsg;         /* JPEG status messages */

extern uint32   half_buffer_select;
extern uint32   data_byte_remain;
extern uint32   file_position;                       /* Current position within input file */
extern uint32   bitPack;


extern uint32   frameQuantTableSelector[JPGD_MAXCOMPONENTS];   /* Component quantization table selector */
extern uint32   startOfSpectralSel, endOfSpectralSel; 
extern uint32   succApproxBitHigh,succApproxBitLow;
extern uint32   scanCompSelector[JPGD_MAXCOMPSINSCAN];
extern uint32   scanDCTableSelector[JPGD_MAXCOMPONENTS],scanACTableSelector[JPGD_MAXCOMPONENTS];


extern boolean  prog_flag;                          /* Flag to indicate progressive decoding */
extern boolean  isSyncCb;                           /* Flag to indciate synchronous/asynchronous decoding */                        

extern boolean  isDQTTablePresent;

uint32    progCompSel;
uint32    progScanAC;


/*===========================================================================
Function: FETCHBYTE

Description: Fetches a byte from the input stream

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects: byteOffset,file_position,data_byte_remain,in_dataPtr,
              total_byte_read
============================================================================*/

uint8 fetchByte()
{
  uint8 value;

  /* If more data is needed then send out a request  */
  
  /* Changed the condition for byteOffset inorder to accomodate 
  restart markers that can occur in the following combinations:
  a) End of boundary of the half_buffer_select1 (0xFF) and 
     start of half_buffer_select2 (0xD[0-7])
  b) End of boundary of the half_buffer_select2 (0xFF) and 
     start of half_buffer_select1 (0xD[0-7])
  */

  if ( byteOffset > (MAX_DATA_BYTES + 2) )
  {
    byteOffset = byteOffset - (MAX_DATA_BYTES + 2);

    if ( data_byte_remain > 0 )
    {
      /* Set position in input data stream to get data */
      jpeg_get_data.position  = file_position;  

      /* Either request maximum number of bytes or what is left in chunk     */
      if (data_byte_remain > MAX_DATA_BYTES)
      {
        jpeg_get_data.num_bytes = MAX_DATA_BYTES;   /* Max bytes requested   */
        file_position += MAX_DATA_BYTES;            /* Update data position  */
        data_byte_remain -= MAX_DATA_BYTES;         /* Decrement count       */
      }
      else
      {
        jpeg_get_data.num_bytes = data_byte_remain; /* Max bytes requested    */
        file_position += data_byte_remain;          /* Update data position   */
        data_byte_remain = 0;                       /* No more bytes in chunk */
      }

      /* Determine which half of buffer to process */
      if ( half_buffer_select == 1  )
      {
        half_buffer_select = 2; /* Use second half of buffer */
        jpeg_get_data.buffer  = in_data_buf;
      }
      else
      {
        half_buffer_select = 1; /* Use first half of buffer  */
        jpeg_get_data.buffer  = in_data_buf + MAX_DATA_BYTES;
      }

      /* Get more data into the working buffer */
      jpeg_get_data.callback_ptr = NULL;

      if (jpeg_cmx_cb != NULL)
      {
         if( !isSyncCb)
         {
           (void) graph_get_async_data( JPEG_CMX_GET_DATA, jpeg_cmx_handle, &jpeg_get_data);
         }
         else
         {
           jpeg_cmx_cb(JPEG_CMX_GET_DATA, jpeg_cmx_handle, &jpeg_get_data);
         }
      }
   
    }
  }

  /* Get data byte from chunk data stream and update the running CRC       */
  value = *in_dataPtr++;

  /* Boundary check to make sure that decrementing in_data_ptr 
  doesn't overwrite the memory segment before the
  start of half_buffer_select_2 */

  if( in_dataPtr == (in_data_buf + (MAX_DATA_BYTES * 2)))
  {
    in_dataPtr = in_data_buf;
  }

  byteOffset++;                                /* Increment byte offset   */
  total_byte_read++;
 
  return (value);
}


/*===========================================================================
Function: FETCHBYTEM

Description: Fetches a byte from the input stream, but stops when a 
             marker is encountered and returns 0xFF

Return Value: UINT8

Comments: 

Side Effects:
============================================================================*/
uint8 fetchByteM(void)
{
  uint8 byte1;

  byte1 = fetchByte();

  if (byte1 == 0xFF)
  {
    byte1 = fetchByte();

    if (byte1 != 0x00)
    {
      recedeImagePtr();
      recedeImagePtr();
    }
    return (0xFF);
  }
  return (byte1);
}



/*===========================================================================
Function: FETCHBITS

Description:  Returns the number of bits requested from bitpack. 
              Returns 0xFF when a marker is encountered

Return Value: UINT32

Comments: This function assumes that the number of requested bits is always <= 16

Side Effects: bitPack, bitsRemaining
============================================================================*/
uint32 fetchBits(int32 numberofBits, boolean isMarkerStop)
{
  uint32 bits;
  uint8 byte1, byte2;

  // If thare are insuuficient bits to service the request, do a pre-read
  if (bitsRemaining < numberofBits)
  {
    if(!isMarkerStop)
    {
      byte1 = fetchByte();
      byte2 = fetchByte();
    }
    else
    {
      byte1 = fetchByteM();
      byte2 = fetchByteM();
    }
    bitPack =   (bitPack) | ((uint32)byte1 << (24 - (bitsRemaining))) | ((uint32)byte2 << (16 - (bitsRemaining))) ;
    bitsRemaining += 16;
  }

  bits = bitPack >> (32 - numberofBits); 
  bitPack =   ((bitPack)<< ( numberofBits ));

  if ((bitsRemaining-= numberofBits)  <= 16)
  {
    if(!isMarkerStop)
    {
      byte1 = fetchByte();
      byte2 = fetchByte();
    }
    else
    {
      byte1 = fetchByteM();
      byte2 = fetchByteM();
    }
    bitPack =   (bitPack) | ((uint32)byte1 << (24 - (bitsRemaining))) | ((uint32)byte2 << (16 - (bitsRemaining))) ;
    bitsRemaining += 16;
  }

  return bits;
}



/*===========================================================================
Function: RECEDEIMAGEPTR

Description:  Pushes back the image data pointer in case of previously 
              retieved buffer

Return Value: None

Comments: 

Side Effects: in_dataPtr,total_byte_read
============================================================================*/
void recedeImagePtr()
{
  
  --in_dataPtr;

  /* Boundary check to make sure that decrementing in_data_ptr 
  doesn't overwrite into the memory segment before the
  start of half_buffer_select_1 */

  if (in_dataPtr < in_data_buf)
  {
    in_dataPtr = in_data_buf + ((2 * MAX_DATA_BYTES) - 1);
  }

  byteOffset--;
  total_byte_read--;
}


/*===========================================================================
Function: CLAMP

Description:  Clamping for IDCT conversion

Return Value: UINT8

Comments: 

Side Effects:
============================================================================*/
/*lint -save -e737 -e502 -e704*/ 
uint8 clamp(int32 i)
{
  if (i & 0xFFFFFF00)
    i = (((~i) >> 31) & 0xFF);

  return ((uint8) i);
}
/*lint -restore*/


/*===========================================================================
Function: JPEGD_SKIPMARKER

Description:  Skips variable markers

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects:
============================================================================*/

JPEGD_STATUS_MSG jpegd_skipMarker(void)
{
  uint32 left;
 

  left = fetchBits(16,FALSE);

  if ((left < 1) || ( total_byte_read > jpegd_fileSize) )
  {
    jpegd_statusMsg = JPEGD_INVALID_VARIABLE_MARKER  ;
    MSG_ERROR ("JPEG Decoder: Invalid Variable Marker", 0, 0, 0);
    return jpegd_statusMsg;
  }

  if(left > 1)
    left -= 2;
  
  while (left)
  {
    fetchBits(8,FALSE); /*lint !e534 intentionally ignoring return value */
    left--;
  }
  return JPEGD_DECODING_OK;
}

/*===========================================================================
Function: JPEGD_CORRECTBUFFER

Description:  Corrects the input image buffer

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects: bitsRemaining
============================================================================*/


void jpegd_correctBuffer(void)
{
  if (((bitsRemaining == 32)  && (byteOffset < 4)) || 
     ((bitsRemaining == 24)  && (byteOffset < 3)) || 
     ((bitsRemaining == 16)  && (byteOffset < 2)) || 
     ((bitsRemaining == 8)  && (byteOffset < 1)))
  {
    MSG_HIGH("JPEGD: Byte Offset overflow correction",0 ,0, 0);
  }
  else
  {
    if (bitsRemaining == 32)
      recedeImagePtr();

    if (bitsRemaining >= 24)
      recedeImagePtr();

    recedeImagePtr();
    recedeImagePtr();
  
    bitsRemaining = 32;

    fetchBits(16,TRUE); /*lint !e534 intentionally ignoring return value */
    fetchBits(16,TRUE); /*lint !e534 intentionally ignoring return value */
  }
}

/*===========================================================================
Function: JPEGD_FINDSOIMARKER

Description:  Find SOI Marker

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects: 
============================================================================*/
 
JPEGD_STATUS_MSG jpegd_findSOIMarker()
{
  uint32 m1, m2;
  uint32 remainingBytes;

  m1 = fetchBits(8,FALSE);
  m2 = fetchBits(8,FALSE);

  if ((m1 == (uint32)M_JPGM) && (m2 == (uint32)M_SOI))
  {
    return JPEGD_DECODING_OK;
  }
  
  remainingBytes = 256;

  while(remainingBytes)
  {
      m1 = m2;
      m2 = fetchBits(8,FALSE);
      if ((m1 == (uint32)M_JPGM) && (m2 == (uint32)M_SOI))
      {
        break;
      }
      remainingBytes--;
  }

  m2 = (bitPack >> 8) & 0xFF;

  if( (m2 != (uint32)M_JPGM) || (remainingBytes == 0))
  {
    jpegd_statusMsg = JPEGD_INVALID_SOI_MARKER; 
    MSG_ERROR ("JPEG Decoder: JPEG Invalid SOI marker ", 0, 0, 0);
    return jpegd_statusMsg;
  }
  
  MSG_LOW("JPEG Decoder: Sucessfully interpreted SOI marker ", 0, 0, 0);
  return JPEGD_DECODING_OK;
}
 

/*===========================================================================
Function: JPEGD_FINDSOFMARKER

Description:  Find SOF Marker

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects:
============================================================================*/

JPEGD_STATUS_MSG jpegd_findSOFMarker()
{
  int32 m1;
  
  if (jpegd_findSOIMarker() != JPEGD_DECODING_OK)
  {
    jpegd_statusMsg = JPEGD_NOT_JPEG_IMAGE;
    MSG_ERROR ("JPEG Decoder: Not a Valid JPEG Image", 0, 0, 0);
    return jpegd_statusMsg;
  }

  if (( m1 = jpegd_parseMarkers() ) < 0)
  {
    jpegd_statusMsg = JPEGD_ERROR_INTERPRETING_MARKER ;
    MSG_ERROR ("JPEG Decoder: Error Interpreting Marker", 0, 0, 0);
    return jpegd_statusMsg;
  }

  switch(m1)
  {
  case M_SOF0:
    if ((jpegd_interpretSOFMarker()) != JPEGD_DECODING_OK)
    {
      jpegd_statusMsg = JPEGD_INVALID_SOF_MARKER ;
      return jpegd_statusMsg;
    }
    break; 
    
  case M_SOF1:
    if ((jpegd_interpretSOFMarker()) != JPEGD_DECODING_OK)
    {
      jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
      MSG_ERROR ("JPEG Decoder: Error in interpreting extended sequential DCT frame marker, Huffman coding", 0, 0, 0);
      return jpegd_statusMsg;
    }
    break;
    
  case M_SOF2:
    prog_flag = TRUE;
    if ((jpegd_interpretSOFMarker()) != JPEGD_DECODING_OK)
    {
      jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
      MSG_ERROR ("JPEG Decoder: Error in interpreting progressive DCT frame marker, Huffman coding", 0, 0, 0);
      return jpegd_statusMsg;
    }
    break;
    
  case M_SOF3:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported lossless process frame marker, Huffman coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  case M_SOF5:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported differential sequential DCT frame marker, Huffman coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  case M_SOF6:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported differential progressive DCT frame marker, Huffman coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  case M_SOF7:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported differential lossless process frame marker, Huffman coding", 0, 0, 0);        MSG_ERROR ("JPEG Decoder: Error in interpreting baseline DCT process frame marker", 0, 0, 0);
    
    return jpegd_statusMsg;
    
  case M_JPG:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported lossless process frame marker, Huffman coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  case M_SOF9:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported sequential DCT frame marker, arithmetic coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  case M_SOF10:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported progressive DCT frame marker, arithmetic coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  case M_SOF11:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported lossless process frame marker, arithmetic coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  case M_SOF13:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported differential sequential DCT frame marker, arithmetic coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  case M_SOF14:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported differential progressive DCT frame marker, arithmetic coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  case M_SOF15:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported differential lossless process frame marker, arithmetic coding", 0, 0, 0);
    return jpegd_statusMsg;
    
  default:
    jpegd_statusMsg = JPEGD_UNSUPPORTED_SOF_MARKER ;
    MSG_ERROR ("JPEG Decoder: Unsupported SOF Marker", 0, 0, 0);
    return jpegd_statusMsg;
  }
  
  MSG_LOW("JPEG Decoder: Sucessfully located SOF marker ", 0, 0, 0);

  return JPEGD_DECODING_OK;
}


/*===========================================================================
Function: JPEGD_INTERPRETDQTMARKER

Description:  Locates and reads the quantization table information

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects: jpegd_quant
============================================================================*/

JPEGD_STATUS_MSG jpegd_interpretDQTMarker()
{
  uint32  i,j, lengthCalc;
  uint32 length;
  uint32 dataPrecision, QTableDestId;
  uint16 qvalue;
  
  length = fetchBits(16,FALSE);
  lengthCalc = 2;

  for ( i = 0; i < (length / 64) ; i++)
  {
    dataPrecision = fetchBits(4,FALSE);
    QTableDestId = fetchBits(4,FALSE);

    if (dataPrecision == 0)
      qvalue = 8;
    else if (dataPrecision == 1)
      qvalue = 16;
    else
    {
      jpegd_statusMsg = JPEGD_UNSUPPORTED_PRECISION_IN_SOF;
      MSG_ERROR("JPEG Decoder: Unsupported precision in SOF marker", 0, 0, 0 );
      return jpegd_statusMsg;
    }

    if (QTableDestId >= JPGD_MAXQUANTTABLES)
    {
      jpegd_statusMsg = JPEGD_INVALID_DQT_TABLE;
      MSG_ERROR ("JPEG Decoder: Invalid DQT Table", 0, 0, 0);
      return jpegd_statusMsg;
    }

    for (j = 0; j < 64; j++)
    {
      jpegd_quant[QTableDestId][j] = (int16)fetchBits(qvalue,FALSE);
    }

    lengthCalc = lengthCalc + 65 + 64 * dataPrecision ;

    if(lengthCalc > length )
    {   
      jpegd_statusMsg = JPEGD_INVALID_DQT_TABLE;
      MSG_ERROR ("JPEG Decoder: Invalid DQT Table", 0, 0, 0);
      return jpegd_statusMsg;
    }
  }
  
  MSG_LOW("JPEG Decoder: Sucessfully interpreted DQT marker ", 0, 0, 0);
  isDQTTablePresent = TRUE; 
  return JPEGD_DECODING_OK;
}



/*===========================================================================
Function: JPEGD_INTERPRETSOFMARKER

Description:  Reads the SOF Marker

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects: jpegd_imageWidth,jpegd_imageHeight,numberOfComponentsInFrame
              frameCompID,frameHSamplingFactor,frameVSamplingFactor,
              frameQuantTableSelector
============================================================================*/

JPEGD_STATUS_MSG jpegd_interpretSOFMarker()
{
  uint32 i;
  uint32 length;
  uint32 dataPrecision;
  
  length = fetchBits(16,FALSE);
  dataPrecision = fetchBits(8,FALSE);

  if (dataPrecision != 8)
  {
    jpegd_statusMsg = JPEGD_UNSUPPORTED_PRECISION_IN_SOF;
    MSG_ERROR ("JPEG Decoder: Unsupported Precision in SOF", 0, 0, 0);
    return jpegd_statusMsg; 
  }
  
  jpegd_imageHeight = (uint16)fetchBits(16,FALSE);
  jpegd_imageWidth = (uint16)fetchBits(16,FALSE);

  if ( (jpegd_imageWidth == 0) || (jpegd_imageWidth >= JPGD_MAX_WIDTH ) )
  {
    jpegd_statusMsg = JPEGD_INVALID_WIDTH;
    MSG_ERROR ("JPEG Decoder: Picture exceeding the MAX_WIDTH or invalid width", 0, 0, 0);
    return jpegd_statusMsg;
  }
  
  if ( (jpegd_imageHeight == 0) || (jpegd_imageHeight >= JPGD_MAX_HEIGHT ) )
  {
    jpegd_statusMsg = JPEGD_INVALID_HEIGHT;
    MSG_ERROR ("JPEG Decoder: Picture exceeding the MAX_HEIGHT or invalid height", 0, 0, 0);
    return jpegd_statusMsg;
  }
    
  numberOfComponentsInFrame = fetchBits(8,FALSE);

  if((numberOfComponentsInFrame != 1) && (numberOfComponentsInFrame != 3))
  {
    jpegd_statusMsg = JPEGD_INVALID_NUMBER_OF_COMPONENTS;
    MSG_ERROR ("JPEG Decoder: Invalid Number of Components", 0, 0, 0);
    return jpegd_statusMsg;
  }

  if (length != (8 + numberOfComponentsInFrame * 3))
  {
    jpegd_statusMsg = JPEGD_INVALID_NUMBER_OF_COMPONENTS;
    MSG_ERROR ("JPEG Decoder: Invalid Number of Components", 0, 0, 0);
    return jpegd_statusMsg;
  }

  for(i = 0; i < numberOfComponentsInFrame; i++)
  {
    frameCompID[i] = (uint8)fetchBits(8,FALSE);
    frameHSamplingFactor[i] = fetchBits(4,FALSE);     
    frameVSamplingFactor[i] = fetchBits(4,FALSE);     
    frameQuantTableSelector[i] = fetchBits(8,FALSE);      

    if (frameQuantTableSelector[i] > 3)
    {
      jpegd_statusMsg = JPEGD_INVALID_DQT_MARKER;
      MSG_ERROR ("JPEG Decoder: JPEG Decoder: Invalid DQT Table Selector", 0, 0, 0);
      return jpegd_statusMsg;
    }
  }

  if (frameHSamplingFactor[0] == 4)
  {
    jpegd_statusMsg = JPEGD_INVALID_SAMPLING_FACTOR;
    MSG_ERROR ("JPEG Decoder: YUV 411 - Invalid Sampling Factor", 0, 0, 0);
    return jpegd_statusMsg;
  }

  MSG_LOW("JPEG Decoder: Sucessfully interpreted SOF marker ", 0, 0, 0);

  return JPEGD_DECODING_OK;
}



/*===========================================================================
Function: JPEGD_READDRIMARKER

Description:  Reads the DRI Restart Marker

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects:
============================================================================*/

JPEGD_STATUS_MSG jpegd_readDRIMarker(void)
{
  if (fetchBits(16,FALSE) != 4)
  {
    jpegd_statusMsg = JPEGD_INVALID_DRI_LENGTH;
    MSG_ERROR ("JPEG Decoder: Invalid  Length in Restart Marker", 0, 0, 0);
    return jpegd_statusMsg;
  }

  restart_interval = (uint16)fetchBits(16,FALSE);
  return JPEGD_DECODING_OK;
}


/*===========================================================================
Function: JPEGD_READNEXTMARKER

Description:  Reads the Next Marker

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects:
============================================================================*/

int32 jpegd_readNextMarker(void)
{
  uint32 byte1;

  do
  {
    do
    {
      byte1 = fetchBits(8,FALSE);

    } while ((byte1 != 0xFF) && (total_byte_read < (jpegd_fileSize + 4)));

    do
    {
      byte1 = fetchBits(8,FALSE);

    } while ((byte1 == 0xFF) && (total_byte_read < (jpegd_fileSize + 4)));

  } while ((byte1 == 0) && (total_byte_read < (jpegd_fileSize + 4)));
  
  return (int32)byte1;
}


/*===========================================================================
Function: JPEGD_INTERPRETSOSMARKER

Description:  Read SOS Marker

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects: numberOfComponentsInScan,scanCompSelector,scanDCTableSelector,
              scanACTableSelector,startOfSpectralSel,endOfSpectralSel,
              succApproxBitHigh,succApproxBitLow
============================================================================*/

JPEGD_STATUS_MSG jpegd_interpretSOSMarker()
{
  int32 m1;
  uint32 i,j;
  uint8 compSel;
  uint16 length;

#ifdef FEATURE_JPEG_DECODER_REV2
  uint16 jpegdBoundaryIndex;
#endif

  m1 = jpegd_parseMarkers();

  if (m1 == (int32)M_EOI)
    return JPEGD_END_OF_IMAGE ;
  else if (m1 != (int32)M_SOS)
  {
    jpegd_statusMsg = JPEGD_UNEXPECTED_MARKER;
    MSG_ERROR ("JPEG Decoder: Unexpected Marker", 0, 0, 0);
    return jpegd_statusMsg;
  }
  
  length = (uint16)fetchBits(16,FALSE);

#ifdef FEATURE_JPEG_DECODER_REV2 
  jpegdBoundaryIndex = MAX_DATA_BYTES - ((MAX_DATA_BYTES + total_byte_read) % MAX_DATA_BYTES);

  if (bitsRemaining == 32)
  {
    if (jpegdBoundaryIndex < 6)
      jpegd_dspBstrmPtr = in_data_buf - jpegdBoundaryIndex + length - 6;
    else
      jpegd_dspBstrmPtr = in_dataPtr + length - 6;

    jpegd_totalBytesRead = total_byte_read + length - 6;
  }
  else
  {
    if (jpegdBoundaryIndex <= 5)
      jpegd_dspBstrmPtr = in_data_buf - jpegdBoundaryIndex + length - 5;
    else
      jpegd_dspBstrmPtr = in_dataPtr + length - 5;

    jpegd_totalBytesRead = total_byte_read + length - 5;
  }
#endif

  numberOfComponentsInScan = fetchBits(8,FALSE);

  if ((numberOfComponentsInScan != 1) && (numberOfComponentsInScan != 3))
  {
    jpegd_statusMsg = JPEGD_INVALID_COMPONENTS_IN_SOS;
    MSG_ERROR ("JPEG Decoder: Invalid number of components in SOS", 0, 0, 0);
    return jpegd_statusMsg;
  }

  if (length != (6 + numberOfComponentsInScan * 2))
  {
    jpegd_statusMsg = JPEGD_INVALID_COMPONENTS_IN_SOS;
    MSG_ERROR ("JPEG Decoder: Invalid number of components in SOS", 0, 0, 0);
    return jpegd_statusMsg;
  }

  for(i = 0; i < numberOfComponentsInScan; i++)
  {
    compSel = (uint8)fetchBits(8,FALSE);

    if (compSel > JPGD_MAXCOMPONENTS)
    {
      jpegd_statusMsg = JPEGD_INVALID_COMPONENTS_IN_SOS;
      MSG_ERROR ("JPEG Decoder: Invalid Component Selector in SOS", 0, 0, 0);
      return jpegd_statusMsg;
    }

    for (j = 0; j < numberOfComponentsInFrame; j++)
    {
      if(compSel == frameCompID[j])
        break;
    }

    scanCompSelector[i] = j;
    scanDCTableSelector[j] = fetchBits(4,FALSE);     
    scanACTableSelector[j] = fetchBits(4,FALSE);      
    
    if ((scanDCTableSelector[j] > 3) || (scanACTableSelector[j] > 3))
    {
      jpegd_statusMsg = JPEGD_INVALID_TABLE_SELECTOR_IN_SOS;
      MSG_ERROR ("JPEG Decoder: Invalid Table Selector in SOS", 0, 0, 0);
      return jpegd_statusMsg;
    }
    
    if (numberOfComponentsInScan == 1)
    {
      progScanAC = scanACTableSelector[j]; 
      progCompSel = compSel - 1;
    }
  }

  startOfSpectralSel = fetchBits(8,FALSE);
  endOfSpectralSel =  fetchBits(8,FALSE);

  if ((startOfSpectralSel > 63) || (endOfSpectralSel  > 63))
  {
    jpegd_statusMsg = JPEGD_INVALID_SPECTRAL_IN_SOS;
    MSG_ERROR ("JPEG Decoder: Invalid Spectral Selection in SOS", 0, 0, 0);
    return jpegd_statusMsg;
  }

  succApproxBitHigh = fetchBits(4,FALSE);
  succApproxBitLow = fetchBits(4,FALSE);

  if ((succApproxBitHigh > 13) || (succApproxBitLow  > 13))
  {
    jpegd_statusMsg = JPEGD_INVALID_SUCC_APPROX_IN_SOS;
    MSG_ERROR ("JPEG Decoder: Invalid Successive Approximation in SOS", 0, 0, 0);
    return jpegd_statusMsg;
  }

  MSG_LOW("JPEG Decoder: Sucessfully interpreted SOS marker ", 0, 0, 0);
  
  if (!isDQTTablePresent)
  {
    jpegd_statusMsg = JPEGD_INVALID_DQT_TABLE;
    MSG_ERROR ("JPEG Decoder: Invalid Offset to DQT Table / Missing DQT Table", 0, 0, 0);
    return jpegd_statusMsg;
  }

  return JPEGD_DECODING_OK;
}

#endif /* FEATURE_JPEG_DECODER */


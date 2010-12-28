/*============================================================================ 
                 Camera Raw Data

   DESCRIPTIONFileSourceDevice Driver  
     This file contains the definitions needed for the camera raw data
     format.

   Design Note:
     All function calls are executed from the caller's context.

   Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameraraw/main/latest/src/qcamraw.c#2 $ $DateTime: 2009/02/12 03:16:09 $ $Author: c_caluru $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 02/12/09  cgera  Added support for MULTISHOT AEC 1.1
 03/03/07  ven    Fixed return address for chroma buffer
 10/18/06  jch    Preview async mode and new buffer mechanism.
 08/04/06  dl     Added chromaBuffer in qcamrawHeader to allow independent 
                  Luma and Chroma data allocation.
 04/07/06  mvd    Introduced new helper function:  qcamrawGetDataCbCrEndPtr
                  Gets the end of the CbCr data stream.
 02/16/06  dle    Linted.
 05/18/04  drl    Added qcamrawGetYDataPtr and qcamrawGetCbCrDataPtr 
 11/14/03  dle    Added qcamrawGetDataPtr().
 11/10/03  dle    Added padSize and qcamrawIsHeader().
 10/10/03  dle    Added version.
 08/29/03  dle    First revision.

============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "qcamraw.h"
#include "memory.h"

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

/*============================================================================
                        DATA DECLARATIONS
============================================================================*/

uint32 qcamrawHeaderSize;
LOCAL char qcamrawIdString[] = "QCAMRAW ";

/*============================================================================
                        FORWARD DECLARATIONS
============================================================================*/

/*============================================================================
                        EXTERNAL FUNCTIONS
============================================================================*/

/*===========================================================================

FUNCTION      QCAMRAWINITHEADERSIZE

DESCRIPTION
              Used to initialize the raw data format header size.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

uint32 qcamrawInitHeaderSize(void)
{
  qcamrawHeaderSize = sizeof(qcamrawHeaderType);
  qcamrawHeaderSize = ((qcamrawHeaderSize + 3) / 4) * 4;
  return qcamrawHeaderSize;
}

/*===========================================================================

FUNCTION      QCAMRAWSETHEADER

DESCRIPTION
              Used to set the raw data format header.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void qcamrawSetHeader
(
 qcamrawHeaderType *header,
 uint8  buffer_index,
 uint32 dataSize,
 qcamrawDataType dataType,
 qcamrawFrameType frameType,
 qcamrawReflectType reflect,
 uint16 rotation,
 uint16 picWidth,
 uint16 picHeight,
 uint16 dataWidth,
 uint16 dataHeight,
 uint32 redGain,
 uint32 greenGain,
 uint32 blueGain,
 uint32 padSize,
 uint32 YSize,
 uint32 padSize2,
 uint32 CbCrSize,
 uint8 * chromaBuffer 
)
{
  memcpy(header, qcamrawIdString, 8);
  header->buffer_index = buffer_index;
  header->headerSize = qcamrawHeaderSize;
  header->dataSize = dataSize;
  header->dataType = dataType;
  header->frameType = frameType;
  header->reflect = reflect;
  header->rotation = rotation;
  header->picWidth = picWidth;
  header->picHeight = picHeight;
  header->dataWidth = dataWidth;
  header->dataHeight = dataHeight;
  header->redGain = redGain;
  header->greenGain = greenGain;
  header->blueGain = blueGain;
  header->version = QCAMRAW_VERSION;
  header->padSize = padSize;
  header->YSize = YSize;
  header->padSize2 = padSize2;
  header->CbCrSize = CbCrSize;
  header->chromaBuffer = chromaBuffer;
}

/*===========================================================================

FUNCTION      QCAMRAWISHEADER

DESCRIPTION
              Check to see if the buffer has qcamraw header.

DEPENDENCIES
  None

RETURN VALUE
  True if yes
  False if not

SIDE EFFECTS
  None

===========================================================================*/

boolean qcamrawIsHeader(const byte *header)
{
  /*lint -save -e826 area too small */
  if ((*(uint32*)(header) == 0x4D414351) &&
      (*(uint32*)(header+4) == 0x20574152))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
  /*lint -restore */
}

/*===========================================================================

FUNCTION      QCAMRAWGETDATAPTR

DESCRIPTION
              Return with the data pointer in the buffer.

DEPENDENCIES
  None

RETURN VALUE
  Data pointer in the buffer.

SIDE EFFECTS
  None

===========================================================================*/

uint8 *qcamrawGetDataPtr(const byte *header)
{
  /*lint -save -e826 area too small */
  qcamrawHeaderType *myHeader = (qcamrawHeaderType *)header;
  /*lint -restore */
  return (uint8 *)((uint32)header+myHeader->headerSize+myHeader->padSize);
}

/*===========================================================================

FUNCTION      QCAMRAWGETDATAYPTR

DESCRIPTION
              Return with the Y component data pointer in the buffer.

DEPENDENCIES
  None

RETURN VALUE
  Data pointer in the buffer.

SIDE EFFECTS
  None

===========================================================================*/

uint8 *qcamrawGetDataYPtr(const byte *header)
{
  return qcamrawGetDataPtr (header);
}

/*===========================================================================

FUNCTION      QCAMRAWGETDATACbCrPTR

DESCRIPTION
              Return with the CbCr component data pointer in the buffer.

DEPENDENCIES
  None

RETURN VALUE
  Data pointer in the buffer.

SIDE EFFECTS
  None

===========================================================================*/

uint8 *qcamrawGetDataCbCrPtr(byte *header)
{
  /*lint -save -e826 area too small */
  qcamrawHeaderType *myHeader = (qcamrawHeaderType *)header;
  /*lint -restore */
  return (uint8 *)(myHeader->chromaBuffer);
}



/*===========================================================================

FUNCTION      QCAMRAWGETDATACbCrENDPTR

DESCRIPTION
              Return with the location just after the CbCr component data
              pointer in the buffer.  Returns location at the buffer end.

DEPENDENCIES
  None

RETURN VALUE
  Data pointer in the buffer.

SIDE EFFECTS
  None

===========================================================================*/

uint8 *qcamrawGetDataCbCrEndPtr(byte *header)
{
  /*lint -save -e826 area too small */
  qcamrawHeaderType *myHeader = (qcamrawHeaderType *)header;
  /*lint -restore */
  return (uint8 *)((uint32)myHeader->CbCrSize+(qcamrawGetDataCbCrPtr (header)));
}
/*===========================================================================

FUNCTION      QCAMRAW_GET_PAD_SIZE

DESCRIPTION
              Return with the location just after the CbCr component data
              pointer in the buffer.  Returns location at the buffer end.

DEPENDENCIES
  None

RETURN VALUE
  Data pointer in the buffer.

SIDE EFFECTS
  None

===========================================================================*/

uint16 qcamraw_get_pad_size(const byte *header)
{
   /*lint -save -e826 area too small */
  qcamrawHeaderType *myHeader = (qcamrawHeaderType *)header;
  return myHeader->padSize;
}  
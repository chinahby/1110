#ifndef QCAMRAW_H
#define QCAMRAW_H
/*============================================================================
  
                 Qualcom CAMera RAW data format
  
   DESCRIPTION
  
============================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameraraw/main/latest/inc/qcamraw.h#2 $ $DateTime: 2009/02/12 03:16:09 $ $Author: c_caluru $

 when      who    what, where, why   
 --------  -----  ----------------------------------------------------------
 02/12/09  cgera   Added support for MULTISHOT AEC 1.1
 10/18/06  jch    Preview async mode and new buffer mechanism.
 08/04/06  dl     Added chromaBuffer in qcamrawHeader to allow independent 
                  Luma and Chroma data allocation.
 04/26/06  nrw    Replaced SIRIUS_BUILD with SIRIUS_CAMERA.
 04/07/06  mvd    Introduced new helper function:  qcamrawGetDataCbCrEndPtr
                  Gets the end of the CbCr data stream.
 02/16/06  dle    Linted.
 09/07/05  nrw    For SIRIUS_CAMERA only: Added Biomorphic JPEG type to 
                  qcamrawDataType.
 09/21/04  sch    Added enumeration to support 8-bit bayer or 10-bit bayer
                  for 6550 support of raw snapshot.
 05/18/04  drl    Added qcamrawGetYDataPtr and qcamrawGetCbCrDataPtr.
                  Also added QCAMRAW_YCBCR420 and QCAMRAW_YCBCR422.
 11/14/03  dle    Added qcamrawGetDataPtr().
 11/10/03  dle    Added padSize and qcamrawIsHeader().
 10/10/03  dle    Added version.
 08/29/03  dle    Initial revision.

============================================================================*/
#include "comdef.h"
#define QCAMRAW_VERSION 3
typedef enum {
#ifdef SIRIUS_CAMERA
#error code not present
#endif /* SIRIUS_CAMERA */
  /* 16 bits, from MSB to LSB: R5, G6, B5 */
  QCAMRAW_RGB565,
  /* 32 bits, from MSB to LSB: DC14, R6, G6, B6 */
  QCAMRAW_RGB666,
  /* 16 bits, from MSB to LSB: DC4, R4, G4, B4 */
  QCAMRAW_RGB444,
  /* 32 bits, from MSB to LSB: DC8, R8, G8, B8 */
  QCAMRAW_RGB888,
  /* 8 bit pattern of Cb, Y, Cr, Y */
  QCAMRAW_CBYCRY,
  /* 8 bit pattern of Y, Cb, Y, Cr */
  QCAMRAW_YCBYCR,
  /* 8 bit pattern of Cr, Y, Cb, Y */
  QCAMRAW_CRYCBY,
  /* 8 bit pattern of Y, Cr, Y, Cb */
  QCAMRAW_YCRYCB,
  /* Bayer format, First row starts with  R, G, R, G, .... */
  /*               Second row starts with G, B, G, B, .... */
  QCAMRAW_BAYER_RGGB,
  /* Bayer format, First row starts with  G, R, G, R, .... */
  /*               Second row starts with B, G, B, G, .... */
  QCAMRAW_BAYER_GRBG,
  /* Bayer format, First  row starts with G, B, G, B, .... */
  /*               Second row starts with R, G, R, G, .... */
  QCAMRAW_BAYER_GBRG,
  /* Bayer format, First  row starts with B, G, B, G, .... */
  /*               Second row starts with G, R, G, R, .... */
  QCAMRAW_BAYER_BGGR,
  QCAMRAW_H2V1,
  QCAMRAW_H1V2,
  QCAMRAW_H2V2,
  QCAMRAW_H1V1,
  QCAMRAW_BAYER_8BIT,
  QCAMRAW_BAYER_10BIT,
  QCAMRAW_FORMAT_INVALID
} qcamrawDataType;

typedef enum
{
  QCAMRAW_PREVIEW_FRAME,
  QCAMRAW_PICTURE_FRAME
} qcamrawFrameType;

typedef enum
{
  QCAMRAW_NO_REFLECT,
  QCAMRAW_MIRROR_REFLECT,
  QCAMRAW_WATER_REFLECT
} qcamrawReflectType;

typedef struct
{
  /* Identifier, 8 bytes = "QCAMRAW " */
  char idString[8];
  /* Buffer index */
  uint8 buffer_index;
  /* Size of this header and padded to 4-byte boundary */
  dword headerSize;
  /* Size of the data section only */
  dword dataSize;
  qcamrawDataType dataType;
  qcamrawFrameType frameType;
  /* Reflection is applied after rotation */
  qcamrawReflectType reflect;
  /* Degrees of clockwise rotation */
  uint16 rotation;
  /* width of the picture, after resizing, reflection
   * and rotation are applied to the captured data */
  uint16 picWidth;
  /* height of the picture, after resizing, reflection
   * and rotation are applied to the captured data */
  uint16 picHeight;
  /* width of the captured data frame */
  uint16 dataWidth;
  /* height of the captured data frame */
  uint16 dataHeight;
  /*===================================================
   *  The following three variable is only specified when
   *  the data is in bayer format.
   *==================================================*/
  uint32 redGain;
  uint32 greenGain;
  uint32 blueGain;
  uint32 version;
  /* Data follows header and padding size */
  uint32 padSize;
  uint32 YSize;
  uint32 padSize2; /* padding between Y and CbCr */
  uint32 CbCrSize;
  uint8 * chromaBuffer;
} qcamrawHeaderType;

extern uint32 qcamrawHeaderSize;
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
uint32 qcamrawInitHeaderSize(void);

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
);

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

boolean qcamrawIsHeader(const byte *header);

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

uint8 *qcamrawGetDataPtr(const byte *header);

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

uint8 *qcamrawGetDataYPtr(const byte *header);

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

uint8 *qcamrawGetDataCbCrPtr(byte *header);

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

uint8 *qcamrawGetDataCbCrEndPtr(byte *header);

#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */
#endif /* QCAMRAW_H */


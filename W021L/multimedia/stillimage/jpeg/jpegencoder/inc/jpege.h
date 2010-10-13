#ifndef JPEGE_H
#define JPEGE_H

/*=============================================================================
FILE:       JPEGE.h

SERVICES:   JPEG Encoder Interface

DESCRIPTION:
   This file provides necessary interfaces for the JPEG encoder. 

LIMITATIONS:
   
INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegencoder/main/latest/inc/jpege.h#1 $ $DateTime: 2008/05/09 16:22:45 $ $Author: karenp $

============================================================================*/
/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/06   dl      Added clockwise rotation support.
04/04/06   dl      Added API for user to request thumbnail width and height.
03/17/05   dl      Removed extern cache definition.
01/05/06   dl      Mobicat insert info returned size of stream.
09/29/05   kln     Fixed lint erors
02/18/05   kln     Added prototype for function to vertically upsample CrCb 2D array
01/25/05   kln     Removed JPEGENC_filesizeEncodeSpecType definition
01/10/05   mvd     Added support for File Size Control. 
12/07/04   kln     Added original dimensions to Main structure
10/22/04   pdo     Changed JFIF interface and enabled parameters for 
                    file-size control. 
10/19/04   pdo     Added support for JFIF encoder
10/04/04   pdo     Removed extra JPEGENC_CBrtnType parameter in API
09/01/04   pdo     Added restart parameter
07/08/04   pdo     Initial version

===========================================================================*/

/*---------------------------------------------------------------------------
      Include Files
----------------------------------------------------------------------------*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "fs.h"
#include "exif.h"
#include "jpegqdsp.h"

/*---------------------------------------------------------------------------
      Type Declarations
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
      Defines for JPEG encoder catagories
----------------------------------------------------------------------------*/
/*
** Define the message type for the client callback 
*/
typedef enum
{
  JPEGENC_DSP_FAIL,
  JPEGENC_DSP_SUCCESS,
  JPEGENC_DSP_BAD_CMD,
  JPEGENC_IMG_DONE,
  JPEGENC_IMG_ABORT,
  JPEGENC_IMG_FAIL,
  JPEGENC_FILE_SIZE_FAIL
} JPEGENC_msgType;

/*
** Define the processing type for the DSP callback 
*/
typedef enum
{
  JPEGENC_DOWNLOAD,
  JPEGENC_ENCODE_IMG,
  JPEGENC_READ_BUFFER1,
  JPEGENC_READ_BUFFER2,
  JPEGENC_IDLE,
  JPEGENC_DONE,
  JPEGENC_OPER_MAX
} JPEGENC_operationType;

/*
** Define the output mode type
*/
typedef enum
{
  JPEGENC_EFS,
  JPEGENC_MEM
} JPEGENC_outputType;

/*
** Define the enum for color subsampling
*/
typedef enum
{
  JPEGENC_H2V2 =0,
  JPEGENC_H2V1,
  JPEGENC_H1V2
} JPEGENC_formatType;

/*
** Define the enum for image rotation
*/
typedef enum
{
  JPEGENC_NO_ROT = 0,
  JPEGENC_CCLK90,
  JPEGENC_CCLK180,
  JPEGENC_CCLK270,
  JPEGENC_CLK90,
  JPEGENC_CLK180,
  JPEGENC_CLK270
} JPEGENC_OrientType;

/*
** Define common JPEG QDSP return code enumeration
*/
typedef enum
{
   JPEGENC_SUCCESS       = 0,      /* cmd sucessful */
   JPEGENC_INVALID_PTR,            /* Invalid callback function */
   JPEGENC_BUSY_STATE,
   JPEGENC_BAD_DATA,
   JPEGENC_INVALID_INPUT_PARM,      /* Invalid interface fn input parameter */
   JPEGENC_FILE_SIZE_TOO_LOW,
   JPEGENC_ALLOCATED_MEM_TOO_LOW 
} JPEGENCReturnCodeType;

#ifdef FEATURE_CAMERA_MOBICAT
/* Types for mobiCat data  */
typedef enum 
{
    JPEGENC_MC_UINT8  = 0,
    JPEGENC_MC_INT8   = 1,
    JPEGENC_MC_UINT16 = 2,
    JPEGENC_MC_INT16  = 3,
    JPEGENC_MC_UINT32 = 4,
    JPEGENC_MC_INT32  = 5,
    JPEGENC_MC_FLOAT  = 6,
    JPEGENC_MC_DOUBLE = 7
} JPEGENC_mc_type;

/* mobiCat element structure */
typedef struct {
    char *elemName;
    JPEGENC_mc_type type;
    uint16 height;
    uint16 width;
    void *data;
} JPEGENC_mobiCat_type;
#endif

/*
** Define device handle type for the destination
*/
typedef struct
{
  JPEGENC_outputType  device;  
  union
  {
    struct 
    {
      uint8          *buffer;
      uint32          length;
    } mem;
    struct 
    {
      char            filename[FS_FILENAME_MAX_LENGTH_P];
    } efs;
  } handle;
} JPEGENC_handleType;

struct JPEGENC_encodeSpecType_s;
typedef struct JPEGENC_encodeSpecType_s JPEGENC_encodeSpecType;

/*
** Define a JPEG encoder client data structure
*/
typedef struct
{
   int32               clientId;     /* Client ID */
   JPEGENC_msgType     status;
   uint32              dcBitCnt;     /* bit count for DC, used by filesize control */
   uint32              header_size;  /* Actual size of JPEG header */
   JPEGENC_outputType  mode;
   uint8              *outPtr;       /* These two are valid only when    */
   uint32              size;         /*  output mode is JPEGENC_MEM      */
   JPEGENC_encodeSpecType *encodeSpec;   /* The encode spec used in the encoding */
} JPEGENC_CBrtnType;

/*
** QDSP message processing callback function structure
*/
typedef void (* JPEGENC_CBType)
(
  JPEGENC_CBrtnType  *clientData             /* client's useful data     */
);

/*
** Define the JPEG encoder specification
*/
struct JPEGENC_encodeSpecType_s
{
   int32                   ClientId;   /* Client ID */
   JPEGENC_CBType          CallBack;
   JPEGENC_handleType      Dest;
   JPEGENC_OrientType      Rotation;
   uint16                  TargetEncodedSize;
                                       /* in Kbyte unit, i.e. 1 = 1024 bytes */
   struct 
   {
     uint16                Quality;    /* HIGH=90, MED=75, LOW=50 */
     uint16                Width;      /* Padded till multiple of 16 width */
     uint16                Height;     /* Padded till multiple of 16 height */   
     uint16                Restart;
     JPEGENC_formatType    Subsample;
     uint16                FragCnt;    
     JPEGE_FragmentCfgType Fragmnt[8]; /* See jpegqdsp_drv.h for detail */
     uint16                OrigWidth;  /* User requested width   */	 
     uint16                OrigHeight; /* User requested height    */
   } Main;
   boolean                 HasThumbnail;  /* TRUE - thumbnail is requested */
                                       /* FALSE - no thumbnail          */       
   struct 
   {
     uint16                Quality;    /* HIGH=90, MED=75, LOW=50 */
     uint16                Width;      /* Padded till multiple of 16 width */
     uint16                Height;     /* Padded till multiple of 16 height */   
     uint16                OrigWidth;  /* User requested width   */	 
     uint16                OrigHeight; /* User requested height    */
     uint16                Restart;
     JPEGENC_formatType    Subsample;
     JPEGE_FragmentCfgType Data;       /* See jpegqdsp_drv.h for detail */
   } Thumbnail;
};

/*===========================================================================

Function: JPEGE_ENCODE_EXIF

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with EXIF (version 2.2) data embedded into the bit stream.
             This is an asynchronous function but caller will receive 
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.

Return Value:
   JPEGENC_SUCCESS              cmd sucessful
   JPEGENC_INVALID_CB           cmd contains a NULL cb pointer
   JPEGENC_INVALID_STATE        cmd issued during invalid state
   
Comments: None

Side Effects: None

==============================================================================*/
JPEGENCReturnCodeType   jpege_encode_exif
(
  JPEGENC_encodeSpecType  *encodeInfo,   /* encode specification   */
  exif_info_type          *exifDataPtr
);

/*===========================================================================

Function: JPEGE_ENCODE_JFXX

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with JFIF extension (Thumbnail) embedded into the bit stream.
             This is an asynchronous function but caller will receive 
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.

Return Value:
   JPEGENC_SUCCESS              cmd sucessful
   JPEGENC_INVALID_CB           cmd contains a NULL cb pointer
   JPEGENC_INVALID_STATE        cmd issued during invalid state
   
Comments: None

Side Effects: None

==============================================================================*/
JPEGENCReturnCodeType   jpege_encode_jfxx
(
  JPEGENC_encodeSpecType  *encodeInfo   /* encode specification   */
);

/*===========================================================================

Function: JPEGE_ENCODE_FILE_SIZE_EXIF

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with EXIF (version 2.2) data embedded into the bit stream.
             This is an asynchronous function but caller will receive
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.
             This function also performs file size control encoding on the
               image such that the passed in target file size is achieved through
               (potentially) several encoding passes.
             If an encoding quality is provided with this function for the main
               image, then file size control will encode to that quality as a
               maximum (however, the minimum could drop well below this).
               A main image encode quality of zero is ignored.

Return Value:
   JPEGENC_SUCCESS                 // cmd sucessful
   JPEGENC_INVALID_PTR             // Invalid callback function
   JPEGENC_BUSY_STATE
   JPEGENC_BAD_DATA
   JPEGENC_INVALID_INPUT_PARM      // Encoding was given invaild input

Comments: None

Side Effects: None

==============================================================================*/
JPEGENCReturnCodeType   jpege_encode_file_size_exif
(
  JPEGENC_encodeSpecType  *encodeInfo,   /* encode specification   */
  exif_info_type          *exifDataPtr
);

/*===========================================================================

Function: JPEGE_ENCODE_FILE_SIZE_JFXX

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with EXIF (version 2.2) data embedded into the bit stream.
             This is an asynchronous function but caller will receive
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.
             This function also performs file size control encoding on the
               image such that the passed in target file size is achieved through
               (potentially) several encoding passes.
             If an encoding quality is provided with this function for the main
               image, then file size control will encode to that quality as a
               maximum (however, the minimum could drop well below this).
               A main image encode quality of zero is ignored.
             Encoding quailty is ignored in the thumbnail.

Return Value:
   JPEGENC_SUCCESS                 // cmd sucessful
   JPEGENC_INVALID_PTR             // Invalid callback function
   JPEGENC_BUSY_STATE
   JPEGENC_BAD_DATA
   JPEGENC_INVALID_INPUT_PARM      // Encoding was given invaild input

Comments: None

Side Effects: None

==============================================================================*/
JPEGENCReturnCodeType   jpege_encode_file_size_jfxx
(
  JPEGENC_encodeSpecType  *encodeInfo    /* encode specification   */
);

/*===========================================================================

Function: JPEGE_ABORT

Description:  This function is used to abort the on going encoding session.
              
Return Value:
   JPEGENC_SUCCESS      - cmd sucessful
   JPEGENC_BAD_ID       - Bad client ID for this command
   
Comments: If JPEGENC_SUCCESS, caller should wait for the callback from the
           encoding session to properly abort the session. 

Side Effects:

============================================================================*/
void  jpeg_encoder_abort
(
  uint16      ClientId                 /* Use the same client ID for the   */
); 

/*===========================================================================

Function: JPEGE_HORIZONTAL_UPSAMPLE

Description:  This function is upsamples 2D CrCb array in horizontal dimension
              by linear interpolation
              
Return Value:
   None
   
Comments: Upsampling is done in-place (the array should be twice the size of  
           original data
Side Effects:

============================================================================*/
void jpege_horizontal_upsample
(
  uint8 *image, 
  uint16 height, 
  uint16 width
);

/*===========================================================================

Function: JPEGE_VERTICAL_UPSAMPLE

Description:  This function is upsamples 2D CrCb array in the vertical dimension
              by linear interpolation
              
Return Value:
   NOne
   
Comments: Upsampling is done in-place (the array should be twice the size of  
           original data
Side Effects:

============================================================================*/
void jpege_vertical_upsample
(
  uint8 *image, 
  uint16 height, 
  uint16 width
);

/*===========================================================================

Function: JPEGE_GET_DIMENSIONS

Description:  This function computes JPEG dimensions (for MCU padding) 
              subject to max limits              
Return Value:
              None  
Comments:  
Side Effects:

============================================================================*/
void jpege_get_dimensions
(
  uint16 *widthOut, 
  uint16 *heightOut, 
  uint16 origWidth, 
  uint16 origHeight,
  boolean isH2V2, 
  JPEGENC_OrientType rotation, 
  uint16 maxHeight, 
  uint16 maxWidth
);

#ifdef FEATURE_CAMERA_MOBICAT 
/*===========================================================================

Function: JPEGE_MOBICAT_INSERT_INFO

Description:  This function transforms mobiCat parameters structure to
              a bytes stream and mark it to get included in AP7 marker
              
Return Value:
              Size of mobicat stream.
   
Comments:  Data is writen in littel endian format 
Side Effects:

============================================================================*/
uint32 jpege_mobicat_insert_info
(
   JPEGENC_mobiCat_type * elemArray, 
   uint16  len 
);
#endif /* FEATURE_CAMERA_MOBICAT  */
#endif /* JPEGE_H */

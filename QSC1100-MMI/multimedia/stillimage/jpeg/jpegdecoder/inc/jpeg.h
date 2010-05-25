#ifndef JPEG_H
#define JPEG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
                      H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the definitions necessary to interface to the
  Joint Photographers Expert Group (JPEG) decoder and encoder.
  

Copyright(c) 2002-2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegdecoder/main/latest/inc/jpeg.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/08   kdiv    Extract main image width and height in header parsing
09/09/08   kdiv    Extract thumbnail width and height in Exif header parsing
12/18/07   kdiv    Added support for YCbCr Jpegd DSP output and YCrCb final output
10/25/07   KV      Added JPEGD_YCrCb422lp,JPEGD_YCrCb420lp in jpeg_outputFormat_type.
01/31/06   vma     Added jpeg_cancel_decode extern signature for lint warnings
11/12/06   arv     Added resize feature for JPEG Decoder
06/06/06   vma     Added YCbCr formats to jpeg_format_enum_type
03/13/06   vma     Fixed all Lint errors
08/04/05   pdo     Removed memory allocation within extract thumbnail to support
                   L4 virtual memory scheme. Extract thumbnail interface has changed.
01/11/05   ark     Added JPEG_DECODE_GET_BUF state and dstPtr variable to 
                   accomodate changes in JPEG_DECODE_THIN_LAYER 
11/04/04   ark     Changed JPEG_ARM_BASED_DECODE and JPEG_DECODE API to provide proper 
                   playback of PMD files. Added new structures
                   jpeg_outputFormat_type and jpeg_decodeInfo_type.
05/17/04   pdo     Added jpeg_decode_thin_layer and jpeg_extract_thumbnail
05/03/04   pdo     Added sync/async option for DSP version decoder
05/02/04   mvd     For stand alone encoder, fixed jpege_mem_device_type 
                   which was incorrectly declared.
04/28/04   mvd     Added callback code: JPEGE_CB_FILE_SIZE_EXCEEDED for 
                   where the encoding completed but output encoded image 
                   exceeds user requested file size
04/19/04   pdo     changed jpeg_decode() and jpeg_ARM_based_decode API with
                   new parameters.
                   Provided needed interfaces to synchronize the async. 
                   data request. 
04/15/04   mvd     Tweaks for stand-alone JPEG encoder.
03/24/04    st     Added jpeg_cmx_buf_req_type for asynchronous buffer request
                   command; updated jpeg_set_buf() prototype.
03/23/04   mvd     Revised stand-alone JPEG Encoder to call into camera
                   services layer.  Provides encode and abort funtionality.
02/18/04   pdo     New capability - ARM based JPEG decoder
11/29/02    sa     Added JPEG_CMX_GET_BUF to jpeg_cmx_status_enum_type.
03/22/02    sa     Added comments. 
03/22/02    sa     Initial version

===========================================================================*/
#include "target.h"             /* target specific definitions             */
#include "comdef.h"             /* customer specific definitions           */
#include "cmx.h"                /* CMX specific definitions                */
#include "fs.h"
#include "exif.h"

#if defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)


typedef enum 
{
  JPEG_CMX_DATA,
  JPEG_CMX_SPEC,
  JPEG_CMX_GET_FILE_SIZE,
  JPEG_CMX_GET_BUF,
  JPEG_CMX_GET_DATA,
  JPEG_CMX_CONTINUE,
  JPEG_CMX_DONE,
  JPEG_CMX_ABORT,
  JPEG_CMX_FAILURE,
  JPEG_DSP_REQUEST,
  JPEG_CMX_INVALID
} jpeg_cmx_status_enum_type;

/* Structure of JPEG image specifications */
typedef struct
{
  uint32 width;                       /* Width of uncompressed image       */
  uint32 height;                      /* Height of uncompressed image      */
  uint32 num_components;              /* Samples per pixel                 */
  cmx_jpeg_color_type  color_space;   /* color space: YUV, RGB, Grayscale  */
  cmx_jpeg_sample_type sample_factor; /* downsampling factor               */
} jpeg_cmx_spec_type;

typedef struct 
{
  uint8  *buffer;
  uint32 num_bytes;
} jpeg_cmx_data_type;

typedef void (* jpeg_dsp_req_func_type) ( void *client_data );

typedef struct 
{
  jpeg_dsp_req_func_type       function_ptr;
  void                         *client_data;
} jpeg_dsp_req_type;

typedef void (* jpeg_cmx_command_cb_func_type) ( void *client_data );

typedef struct 
{
  uint8  *buffer;
  uint32 position;
  uint32 num_bytes;

  jpeg_cmx_command_cb_func_type callback_ptr;
  void                         *client_data;
} jpeg_cmx_get_data_type;

typedef struct
{
  uint8  **buf_handle;
  uint32 num_bytes;

  jpeg_cmx_command_cb_func_type callback_ptr;
  void                         *client_data;
} jpeg_cmx_buf_req_type;

typedef struct 
{
  jpeg_cmx_command_cb_func_type callback_ptr;
  void                         *client_data;
} jpeg_cmx_continue_type;

typedef struct 
{
  jpeg_cmx_command_cb_func_type callback_ptr;
  void                         *client_data;
} jpeg_cmx_callback_type;

typedef void (* jpeg_cmx_cb_func_type) 
                              (
                              jpeg_cmx_status_enum_type    command,
                              void                        *handle,
                              void                        *server_data
                              );

typedef enum
{
  JPEG_RGB565,
  JPEG_RGB888,
  JPEG_YCbCr422lp,
  JPEG_YCbCr420lp,
  JPEG_YCrCb420lp,
  JPEG_YCrCb422lp,
  JPEG_NUM_FORMAT
} jpeg_format_enum_type;

typedef enum
{
  JPEG_DECODE_DONE,
  JPEG_DECODE_ABORT,
  JPEG_DECODE_GET_BUF
} jpeg_cb_status;

typedef enum
{
  JPEGD_RGB565,
  JPEGD_RGB888,
  JPEGD_YCbCr422lp,
  JPEGD_YCbCr420lp,
  JPEGD_YCrCb420lp,
  JPEGD_YCrCb422lp,
  JPEG_NUM_OUTPUT_FORMAT
} jpeg_outputFormat_type;


typedef struct 
{
  uint8    *image_data_ptr;
  uint32    file_size;
  uint8    *display_ptr;
  uint32    disp_buf_size;
  uint8    *yCbCr_ptr;
  uint32    yCbCr_buf_size;
  boolean   thumbnail;
  jpeg_format_enum_type output_format;
#ifdef FEATURE_JPEGD_RESIZE   
  uint32     resizeImageWidth;
  uint32     resizeImageHeight;
  boolean    aspectRatioFlag;
  boolean    jpegdResizeFlag;
#endif
} jpeg_decode_input_type;


typedef struct 
{
  void    *client_data; /* This is set by the client */
  uint16   image_height;
  uint16   image_width;
  boolean  thumbnail;
  uint8    *dstPtr;
} jpeg_decode_output_type;

typedef struct
{
  boolean  			isSync;
  jpeg_outputFormat_type	outFormat;
  boolean			arm_based;
#ifdef FEATURE_JPEGD_RESIZE   
  uint32     width;
  uint32     height;
  uint32     resizeImageWidth;
  uint32     resizeImageHeight;
  boolean    aspectRatioFlag;
  boolean    jpegdResizeFlag;
#endif
} jpeg_decodeInfo_type;



/*
** callback function structure for client to interface
*/
typedef void (* jpeg_cb_func_type)
(
  jpeg_cb_status  status,
  jpeg_decode_output_type * output_ptr 
);

/* --------------------- ENUMS for Stand-Alone JPEG Encoder ----------------- */

typedef enum
{
  JPEGE_ENCODE,
  JPEGE_CB_DONE,
  JPEGE_CB_ABORT,
  JPEGE_CB_ERROR,
  JPEGE_CB_FILE_SIZE_EXCEEDED 
} jpege_status_enum_type;

typedef enum
{
  JPEGE_SUCCESS,
  JPEGE_CB_FUNC_NOT_SPECIFIED,
  JPEGE_INVALID_PARM,
  JPEGE_INPUT_FORMAT_NOT_SUPPORTED,
  JPEGE_CAMERA_LAYER_CALL_FAILED
} jpege_return_code_type;

typedef enum
{
  JPEGE_DEVICE_MEM,
  JPEGE_DEVICE_EFS,
  JPEGE_DEVICE_MAX
} jpege_device_type;

typedef enum
{
  /* YCbCr, each pixel is two bytes. Two pixels form a unit.
   * MSB is Y, LSB is CB for the first pixel and CR for the second pixel. */
  JPEGE_YCBCR,

// Place-holder, but RGB565 is not supported yet
  /* RGB565, each pixel is two bytes.
   * MS 5-bit is red, the next 6-bit is green. LS 5-bit is blue. */
  JPEGE_RGB565,

// Place-holder, but RGB666 is not supported yet
  /* RGB666, each pixel is four bytes.
   * MS 14 bits are zeros, the next 6-bit is red, then 6-bit of green.
   * LS 5-bit is blue. */
  JPEGE_RGB666,
  /* RGB444, each pixel is 2 bytes. The MS 4 bits are zeros, the next
   * 4 bits are red, the next 4 bits are green. The LS 4 bits are blue. */
  JPEGE_RGB444,
  /* RGB888, each pixel is 3 bytes. R is 8 bits, G is 8 bits,
   * B is 8 bits*/
  JPEGE_RGB888
} jpege_format_type;

/* -------------- Callback function prototype for Stand-Alone JPEG Encoder --- */
typedef void (* jpege_cb_func_type)
                              (
                              jpege_status_enum_type      status,
                              void                        *handle,
                              void                        *server_data
                              );

/* -------------- Typedef Structures for Stand-Alone JPEG Encoder ------------ */
typedef struct
{
  char          filename[FS_FILENAME_MAX_LENGTH_P];

} jpege_efs_device_type;

typedef struct
{
  uint8  *      buffer;
  uint32        length;

} jpege_mem_device_type;

typedef union
{
  jpege_mem_device_type mem;
  jpege_efs_device_type efs;

} jpege_device_spec_type;

/* Structure of JPEG image specifications */
typedef struct
{
  /* ------- Input data specs and pointer                                 */
  jpege_format_type format;          /* Format of the input image         */
  uint32 dx;                         /* Width of uncompressed image       */
  uint32 dy;                         /* Height of uncompressed image      */
  byte * in_data;                    /* Input data pointer                */

  /* ------- Output JPEG image specs and pointer                          */
  jpege_device_type      device;      /* mem or file (efs) device 4 output*/
  jpege_device_spec_type device_spec; /* device specific information      */

  // Support this later
  //jpege_sample_type sample_factor; /* downsampling factor              */
                                      /* Only H2V2 or 4:2:0 is supported  */
                                      /* now.                             */
  /* One of the following two parameters is specified at the exclusion of */
  /* the other.  To select quality, set max_file_size to zero.            */
  /* To select max_file_size, set it to > 0                               */
  int32 quality;                      /* Percent quality of resulting     */
                                      /* image. JPEG Quantization tables  */
                                      /* are scaled accordingly.          */
                                      /* Value range is 1 to 100.         */
  int32 max_file_size;                /* max size of output JPEG file in  */
                                      /* kbytes or 0 if not specifying    */
} jpege_encode_type;


#if defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)
/*===========================================================================

FUNCTION jpeg_register

DESCRIPTION
  This function initializes everything needed for JPEG at the time of bootup.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void jpeg_register(void);
#endif /* FEATURE_JPEG_DECODER || FEATURE_JPEG_ENCODER */



#ifdef FEATURE_JPEG_DECODER
/* <EJECT> */
/*===========================================================================

FUNCTION jpeg_decode

DESCRIPTION
  This function decodes a JPEG image.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void jpeg_decode
(
void                              *handle,
jpeg_cmx_cb_func_type             callback_ptr,
jpeg_decodeInfo_type              jpegdInfo
);

/*===========================================================================

FUNCTION JPEG_ARM_BASED_DECODE

DESCRIPTION
  This function decodes a JPEG image without using DSP resource.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void jpeg_ARM_based_decode
(
void                              *handle,
jpeg_cmx_cb_func_type             callback_ptr,
jpeg_decodeInfo_type              jpegdInfo
);

/*===========================================================================

FUNCTION JPEG_DECODE_THIN_LAYER

DESCRIPTION
  This function performs as a thin interface to both JPEG ARM based 
  and DSP based decoder.
  
DEPENDENCIES
  None

RETURN VALUE
  jpeg_decode_output_type structure

SIDE EFFECTS
  None.

===========================================================================*/
extern void jpeg_decode_thin_layer
(
const jpeg_decode_input_type           *in_decode_ptr,
jpeg_cb_func_type                callback_ptr,
jpeg_decode_output_type          *out_decode_ptr,
boolean                          dsp_engine
);

/*===========================================================================
FUNCTION  jpeg_extract_thumbnail

DESCRIPTION
  Extract the thumbnail image from the EXIF JPEG image.
  
  This function allocates the output buffer. It is the caller's
  responsibility to free the outPtr when data have been consumed.
  
DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE:  succeed with thumbnail extracted
            FALSE: No thumbnail or un-recognize file

SIDE EFFECTS
  None
===========================================================================*/
boolean  jpeg_extract_thumbnail( uint8 *bitStreamPtr, uint8 *outPtr,
                                               uint32 *out_buf_size);

/* <EJECT> */
/*===========================================================================

FUNCTION   JPEG_DECODE_CONTINUE

DESCRIPTION
  This function gets called when decode continues from QDSP image ready signal.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void jpeg_decode_continue( void );

/* <EJECT> */
/*===========================================================================

FUNCTION   JPEG_DECODE_IMAGE_ABORT

DESCRIPTION
  This function is called when the JPEG decode image has been aborted because
  of a QDSP image reset.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void jpeg_decode_image_abort( void );

/* <EJECT> */
/*===========================================================================

FUNCTION jpeg_get_image_specs 

DESCRIPTION
  This function reads the header data from a JPEG image file and passes back
  image specifications to the client. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void jpeg_get_image_specs
(
void                              *handle,
jpeg_cmx_cb_func_type             callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION jpeg_cancel_decode

DESCRIPTION
  This function cancels the current JPEG decode process.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void jpeg_cancel_decode(void);

/* <EJECT> */
/*===========================================================================

FUNCTION jpeg_abort_decode

DESCRIPTION
  This function aborts the current JPEG decode process.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void jpeg_abort_decode (void);

#endif /* FEATURE_JPEG_DECODER */

#ifdef FEATURE_JPEG_ENCODER
/* <EJECT> */
/*===========================================================================

FUNCTION   JPEGE_ENCODE

DESCRIPTION
  This function gets called by client to initiate the JPEG encoder.

ARGUEMENTS
  jpege_encode_type               encoding_spec,
    -Input & output specification
  jpege_cb_func_type              callback_ptr
    -callback handle to report back result to caller

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
jpege_return_code_type jpege_encode_fn
(
  jpege_encode_type       encoding_spec,
  void  *                 handle,
  jpege_cb_func_type      callback_ptr
);


/*===========================================================================

FUNCTION jpeg_abort_encode

DESCRIPTION
  This function aborts the current JPEG encode process.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void jpeg_abort_encode (void);

#endif /* FEATURE_JPEG_ENCODER */

/* <EJECT> */
/*===========================================================================

FUNCTION   jpeg_set_buf

DESCRIPTION
  Function for setting JPEG data buffer ptrs to buffer passed in by client.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 jpeg_set_buf
(
  uint8   *client_buffer,                         /* client buffer         */
  uint32  client_buffer_size,                     /* size of client buffer */
  uint8   *output_buffer
);

/*===========================================================================

Function: jpegd_parse_thumbnail_dimensions

Description: This function parses the thumbnail header to extract the width and height

Return Value:
              None
Comments: 

Side Effects: 

==============================================================================*/
void jpegd_parse_thumbnail_dimensions (exif_info_type *info);

/*===========================================================================

Function: jpegd_parse_main_image_dimensions

Description: This function parses the main image width and height

Return Value:
              None
Comments: 

Side Effects: 

==============================================================================*/
void jpegd_parse_main_image_dimensions (exif_info_type *info);

/*===========================================================================

Function: RELEASE_MEMORY

Description:  Release all dynamically allocated memory blocks

Return Value:  None

Comments: 

Side Effects:
============================================================================*/
void release_memory(void);

#endif /* defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER) */
#endif /* JPEG_H              */


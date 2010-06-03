#ifndef PNGE_H
#define PNGE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          P O R T I B L E    N E T W O R K    G R A P H I C S   (PNG)   
                      H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains the necessary interfaces for the
  PNG encoder.
  

Copyright(c) 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/pnge.h#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/06   vma     Addition of separate chroma pointer in PNG encode spec
07/07/06   vma     Added error enumeration for odd width when input is YCbCr
07/01/05   kln     Added filename to PNGE_spec_type, added YCbCr420 to 
                   PNGE_RawDataType 
05/12/04   mvd     Added status types to PNGE_status_enum_type
12/16/03   mvd     Added Bayer width/height to passed in pnge_spec type
12/10/03   mvd     Added to PNGE_rawDataType.  Removed #def 
                   FEATURE_PNG_ENCODER
11/06/03   pdo     Initial version

===========================================================================*/



#include "target.h"             /* target specific definitions             */
#include "comdef.h"             /* customer specific definitions           */
#include "fs.h"                 /* for file handle types and functions     */

#if defined(FEATURE_PNG_ENCODER)

typedef enum 
{
  PNGE_SUCCESS,
  PNGE_INVALID_PARM,
  PNGE_INPUT_FORMAT_NOT_SUPPORTED,
  PNGE_INVALID_WIDTH_YCBCR_INPUT
} PNGE_returnCodeType;

typedef enum 
{
  PNGE_START,
  PNGE_ABORT,
  PNGE_PROCESSING 
} PNGE_stateType;

typedef enum 
{
  YCrCb420LP,
  YCrCb422LP,
  YCrCb444LP,
  YCbCr422,
  RGB565,
  RGB666,
  RGB444,
  BAYER_BGGR,
  BAYER_GBRG,
  BAYER_GRBG,
  BAYER_RGGB,
  RGB888
} PNGE_rawDataType;


typedef enum 
{
  PNGE_ENCODE,
  PNGE_CB_DONE,
  PNGE_CB_ABORT,
  PNGE_INVALID_PARM_ABORT,
  PNGE_CB_INVALID_PARM_ABORT,
  PNGE_INVALID_INPUT_SIZE_PARM,
  PNGE_CB_INVALID_INPUT_SIZE_ABORT,
  PNGE_CB_MALLOC_FAILED_ABORT,
  PNGE_CB_FAILED_TO_INIT_ABORT,
  PNGE_CB_IPL_FAILURE_ABORT,
  PNGE_CB_FREE_FAILED_ABORT,
  PNGE_CB_FS_FAILED_ABORT
} PNGE_status_enum_type;


typedef enum
{
  PNGE_DEVICE_MEM,
  PNGE_DEVICE_EFS
} pnge_device_type;

typedef struct {
  pnge_device_type   device;
  uint8             *buffer;
  uint32             length;
  fs_rsp_msg_type    fs_rsp;
  void*              appHandle;
} pnge_handle_type;


/*
** PNG encoder callback function structure
*/
typedef void (* PNGE_cb_func_type) 
                              (
                              PNGE_status_enum_type    command,
                              void                     *handle,
                              void                     *pnge_data
                              );
/*
** Define PNG encoding specification 
*/
typedef struct
{  
   PNGE_rawDataType   input_data;   /* Raw data format        */
   uint8 *            input_ptr;    /* pointer to the raw input buffer */ 
   uint8 *            chroma_ptr;   /* pointer to the raw input chroma buffer */
   uint16             image_width;  /* image width in pixels  */
   uint16             image_height; /* image height in pixels */
   uint16             bayer_image_width;  /* raw bayer format image width */
   uint16             bayer_image_height; /* raw bayer format image height */
   uint8 *            output_ptr; 
   char               filename[FS_FILENAME_MAX_LENGTH_P];							 	
} PNGE_spec_type;


/* <EJECT> */
/*===========================================================================

FUNCTION: PNGE_ENCODE

DESCRIPTION: This function checks for the input parameters.
             If no invalid parameter then it queues the request to the
             GRAPH thread for PNG encode processing.

DEPENDENCIES
   None.

RETURN VALUE
   PNGE_SUCCESS              cmd sucessful
   PNGE_INVALID_PARM         cmd with invalid input parameter
   PNGE_INVALID_INPUT_SIZE_PARM cmd with unsupported image resolution

COMMENTS
   If calling from GRAPH thread, caller should exit GRAPH thread for the
   PNG encoder to start.

SIDE EFFECTS
   None
==============================================================================*/
extern PNGE_returnCodeType   pnge_encode
(
  PNGE_spec_type             pnge_spec,
  pnge_handle_type          *handle,
  PNGE_cb_func_type          callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION   PNGE_ENCODE_ABORT

DESCRIPTION
  This function should be called when for some reasons the encoding process
  need to be abort forcefully.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void pnge_encode_abort( void );

extern void pnge_image_encode( void );

#endif /* FEATURE_PNG_ENCODER */
#endif /* PNGE_H              */

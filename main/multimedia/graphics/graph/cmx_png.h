#ifndef CMX_PNG_H
#define CMX_PNG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  P O R T A B L E   N E T W O R K   G R A P H I C S  H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the definitions necessary to interface to the
  PNG decoder.


Copyright(c) 2000-2004 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/cmx_png.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/04    st     Updated png_set_buf() function prototype.
07/17/01    sa     Added prototype for function png_set_buf().
03/13/01    sa     Added PNG_CMX_GET_FILE_SIZE to png_cmx_status_enum_type
02/14/01    sa     Removed scaling factor as parameter to PNG decoder.
06/13/00    sa     Changes to optimize code for speed. 
05/30/00    sa     Cleaned up and added comments.     
05/19/00    sa     Added function prototype for png_get_image_specs() and new 
                   data type, png_cmx_spec_type. Added new enums PNG_CMX_TEXT
                   and PNG_CMX_SPEC.  Also featurized code.
05/09/00    sa     Added new data types and abort function prototype. 
04/14/00    sa     Initial version


===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Customer specific definitions           */
#include "cmx.h"                /* CMX specific definitions                */

#if defined(FEATURE_PNG_ENCODER) || defined(FEATURE_PNG_DECODER) // Gemsea Modify #ifdef FEATURE_PNG_DECODER
typedef enum 
{
  PNG_CMX_DATA,
  PNG_CMX_TEXT,
  PNG_CMX_SPEC,
  PNG_CMX_GET_FILE_SIZE,
  PNG_CMX_GET_DATA,
  PNG_CMX_CONTINUE,
  PNG_CMX_DONE,
  PNG_CMX_ABORT,
  PNG_CMX_FAILURE,
  PNG_CMX_INVALID
} png_cmx_status_enum_type;

typedef void (* png_cmx_command_cb_func_type) ( void *client_data );

typedef struct 
{
  uint8  *buffer;
  uint32 position;
  uint32 num_bytes;

  png_cmx_command_cb_func_type callback_ptr;
  void                         *client_data;
} png_cmx_get_data_type;

typedef struct 
{
  png_cmx_command_cb_func_type callback_ptr;
  void                         *client_data;
} png_cmx_continue_type;

typedef struct 
{
  png_cmx_command_cb_func_type callback_ptr;
  void                         *client_data;
} png_cmx_callback_type;

typedef struct 
{
  uint8  *buffer;
  uint32 num_bytes;
} png_cmx_data_type;

typedef struct
{
  uint32 width;
  uint32 height;
  uint32 bit_depth;
  uint32 color_type;
  uint32 compression;
  uint32 filter;
  uint32 interlace;
} png_cmx_spec_type; 

typedef void (* png_cmx_decode_cb_func_type) (
  png_cmx_status_enum_type    command,
  void                        *handle,
  void                        *server_data
);

/*===========================================================================

FUNCTION png_decode

DESCRIPTION
  This function decodes a PNG image.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void png_decode
(
  void                              *handle,
  png_cmx_decode_cb_func_type       callback_ptr
);

/*===========================================================================

FUNCTION png_decode

DESCRIPTION
  This function decodes a PNG image.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void png_decode_abort (void);

/*===========================================================================

FUNCTION png_get_image_specs 

DESCRIPTION
  This function reads the header data from a PNG image and passes back the 
  image width and height, bit depth, color type, compression method, filter 
  method, and interlace method.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void png_get_image_specs
(
  void                              *handle,
  png_cmx_decode_cb_func_type       callback_ptr
);

/*===========================================================================

FUNCTION png_set_buf 

DESCRIPTION
  This function sets PNG internal buffers to client supplied buffers. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 png_set_buf
(
  uint8 *client_buffer,                     /* client buffer               */
  uint32 client_buffer_size,                /* size of client buffer       */
  uint8 *output_buffer,                     /* output buffer               */
  uint32 output_buffer_size                 /* size of output buffer       */
);

#endif /* FEATURE_PNG_DECODER */
#endif  /* CMX_PNG_H */


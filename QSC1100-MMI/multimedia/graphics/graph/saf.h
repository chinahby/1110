#ifndef SAF_H
#define SAF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

S I M P L E    A N I M A T I O N    F O R M A T  (SAF)  H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the definitions necessary to interface to the
  SAF animation player.


Copyright(c) 2001-2004 by NeoMtel Co., Ltd.   All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/saf.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/04    st     Updated saf_global_init() prototype.
09/24/02    sa     Moved declaration of enum saf_state_type to safi.h
07/17/01    sa     Added SAF_STATE_GET_UNKNOWN_DATA to saf_state_type.
                   Modified saf_global_init() prototype to add buffer used by
                   PNG (or other image decoders).
03/22/01    sa     Cleaned up code
03/20/01    sa     Modified saf_global_init function prototype.   Added
                   SAF_CMX_GET_FILE_SIZE to saf_cmx_status_type.
03/11/01    sa     Initial version

===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Customer specific definitions           */
#include "cmx.h"                /* CMX specific definitions                */

#ifdef FEATURE_SAF

#define  MAX_FRAMES 128                      /* maximum frame number       */ 
#define  MAX_TYPES  128

typedef enum {
  SAF_CMX_ABORT,
  SAF_CMX_GET_FILE_SIZE,
  SAF_CMX_GET_DATA,
  SAF_CMX_FAIL,
  SAF_CMX_INFO,
  SAF_CMX_PREPARE_DONE,
  SAF_CMX_CONTINUE,
  SAF_CMX_FRAME_DONE
} saf_cmx_status_type;

typedef void (*saf_cmx_command_cb_func_ptr_type) ( void *client_data );

typedef void(*saf_cmx_decode_cb_func_ptr_type)
(
  saf_cmx_status_type      status,
  void                     *handle,
  void                     *server_data
);

typedef void(*saf_cmx_frame_cb_func_ptr_type)
(
  int32   frameid, 
  uint8   *framebuf,
  void    *client_data
);

typedef struct 
{
  uint32 pos;
  uint32 size;
} FramePosSize;

typedef enum 
{
  SAF_BITDEPTH_1 = 1,
  SAF_BITDEPTH_2 = 2,
  SAF_BITDEPTH_4 = 4,
  SAF_BITDEPTH_8 = 8,
  SAF_BITDEPTH_16= 16,
  SAF_BITDEPTH_24= 24
} saf_bitdepth_type;

typedef struct
{
  uint8                             *buffer;
  uint32                            position;
  uint32                            num_bytes;
  saf_cmx_command_cb_func_ptr_type  callback_ptr;
  void                              *client_data;
} saf_cmx_get_data_type;

typedef struct 
{
  saf_cmx_command_cb_func_ptr_type  callback_ptr;
  void                              *client_data;
} saf_cmx_continue_type;

typedef struct 
{
  saf_cmx_frame_cb_func_ptr_type    callback_ptr;
  void                              *client_data;
} saf_cmx_prepare_done_type;

typedef struct 
{
  uint32   size_frame;
  uint8    *framebuf;
  uint32   delay_to_nextframe;
  uint32   flag_last_frame; 
} saf_cmx_frame_done_type;

typedef struct 
{
  uint32             width;
  uint32             height;
  saf_bitdepth_type  bitdepth;
  uint32             global_transparency_count;
  uint8              *global_transparency;
  uint32             loopcount;
  uint32             number_of_frames;
  uint32             delay_between_frames[MAX_FRAMES];
  uint32             number_palette_entry;
  uint8              *palette;
  saf_cmx_command_cb_func_ptr_type  callback_ptr;
  void               *client_data;
} saf_cmx_global_info_type;


typedef struct {
  uint32   width;
  uint32   height;
  uint32   bitdepth;
  uint8    *pOperationBuf1;
  uint8    *pOperationBuf2;
} saf_display_resource_info_type;

/*===========================================================================

FUNCTION saf_decode

DESCRIPTION
  This function plays a SAF animation.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void saf_decode
(
  void *handle, 
  saf_cmx_decode_cb_func_ptr_type callback_ptr
);

/*===========================================================================

FUNCTION saf_get_info 

DESCRIPTION
  This function reads the header data from a SAF file and passes back the 
  specifications for the animation contained in the file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void saf_get_info
(
  void *handle, 
  saf_cmx_decode_cb_func_ptr_type callback_ptr
);

/*===========================================================================

FUNCTION saf_decode_abort

DESCRIPTION
  This function aborts the animation playback.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void saf_decode_abort(void);

/*===========================================================================

FUNCTION  saf_decode_stop

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void saf_decode_stop(void);

/*===========================================================================

FUNCTION saf_global_init 

DESCRIPTION
  Inititalizes the SAF decoders global data/buffers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void saf_global_init
(
  saf_display_resource_info_type *display_info_data, 
  uint8 *g_object_buf, 
  uint32 g_object_buf_size,
  uint8 *g_coder_buf, 
  uint32 g_coder_buf_size,
  uint8 *g_info_buf
);

#endif /* FEATURE_SAF */
#endif /* SAF_H       */


/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("SAF Decoder")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  S I M P L E    A N I M A T I O N    F O R M A T  (SAF)   D E C O D E R

GENERAL DESCRIPTION
  This module contains functions to decode and play animation files based an
  the Simple Animation Format (SAF).

EXTERNALIZED FUNCTIONS
  saf_decode
    This function is called to start the SAF decoder amd player

  saf_decoder_abort
    This function is called to end the current SAF decoder session

  saf_get_info
    This function is called to provide information about the animation
    sequence in the given file

  saf_global_init
    This function is called to provide the SAF player with display parameter
    information

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2001-2006 by NeoMtel Co., Ltd.   All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/saf.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/08    kk     Fixed lint errors.
08/16/06    aw     Fixed compiler warning.
11/18/05    kk     Fixed the Coverity warning
04/23/04    lg     Added code to decode into frames of actual size. Added support
                   for SAF_STATE_GET_FILE_SIZE
03/22/04    st     Modified code to return global info to client in client's
                   buffer if available.
11/06/02    sa     Changed for loop indice ii to i in function 
                   EdgeDetection_2bpp(). 
09/24/02    sa     Added support for MLZ decoder.
06/12/02    sa     Changed png.h to cmx_png.h to reflect name change.
09/30/01    sa     Optimized most of the rendering functions for 8-bit color
                   index animations.
08/17/01    st     Removed stdio.h.
07/18/01    sa     Incremented counter variable sis_current_mv_id in function 
                   sis_dec_make_one_frame() prior to return in the two cases
                   where display frame size is smaller than source frame size.
07/17/01    sa     Added support for RLA (arithmetic) image decoder.   Fixed 
                   bugs in functions siz_dec_make_one_frame(),
                   saf_scale_factor_calc_y(), saf_scale_factor_calc_x(), 
                   saf_valid_region_check().  Modified saf_global_init() to
                   accept additional variables for initializing buffers used
                   by PNG and RLA decoders.  Added functions 
                   sis_set_4byte_aligned_ptr(), saf_edge_detection_prepare(),
                   and find_orginal_point_is_internal(). Fixed bugs in 1,2,
                   and 4-bit cases.  Added grayscale palette generation for 
                   8-bit case.  Added check in saf_decode_abort() to see if
                   image decoding in progress and if so then abort decoder.
                   Added function saf_decode_stop() to reset SAF decoder
                   state and flag variables.
03/27/01    sa     Added include file "sm_rla_exp.h" and featurized.  Removed
                   LOCAL designation for sis_GlobalObjectBuffer and 
                   sis_size_GlobalObjectBuffer.
03/23/01    sa     Cleaned up code.  PNG decoder now gets called back with 
                   the actual size of PNG object.  Fixed bugs in functions 
                   saf_valid_region_check, saf_rotation_scale_mirror_124bit,
                   and sis_dec_make_one_frame. 
03/20/01    sa     Set initialization of buffer in saf_global_init. Modified
                   structure names to reflect common SAF conventions
03/13/01    sa     Fixed data type mismatches to get rid of warnings.
03/11/01    sa     Initial version
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Common definitions                      */

#ifdef FEATURE_SAF

#include <string.h>             /* Memory Manipulation routines            */

#include "saf.h"                /* PNG typedefs and prototypes             */
#include "safi.h"               /* PNG internal typedefs and prototypes    */
#include "cmx_png.h"            /* PNG typedefs and prototypes             */

#ifdef FEATURE_RLA_DECODER
#error code not present
#endif

#ifdef FEATURE_MLZ_DECODER
#include "smlzdec.h"
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
typedef void (*FILL_IMAGE_899999_SUB)(uint8 *pObjectbuf, uint8 *pFramebuf,
                           int32 temp, int32 temp1, int32 temp2, int32 temp3,
                           int32 temp4, int32 temp5, int32 temp6,
                           int32 coord_frame_left, int32 coord_frame_right,
                           int32 type_local_TransparencyColor);

FILL_IMAGE_899999_SUB fill_image_899999_sub;

uint8 *sis_GlobalObjectBuffer;              /* Ptr to global object buffer */
uint32 sis_size_GlobalObjectBuffer;         /* Size of global object buffer*/
LOCAL uint32 sis_pos_GlobalObjectBuffer;

saf_imageHeaderType                 saf_header_t;
saf_frameInfo                       saf_frameInfo_t;
saf_typeInfo                        saf_typeInfo_t[MAX_TYPES];
saf_display_resource_info_type      sis_display_info_data;

LOCAL uint8 *sis_current_frame_outbuf;
LOCAL int32 sis_current_mv_id;

#ifdef FEATURE_MLZ_DECODER
LOCAL int32 slzko_begin_pos;
LOCAL int32 slzko_return_state;
#define SMLZ_IN_CHUNK_STEP_SIZE 1024
TS_DECODE_STREAM  slzko_dstrm;
#endif

EDGEDETECTION edgedetection;
CLIPIMAGE     clipimage;
SAF_RO_SC_MI  saf_ro_sc_mi;
uint8         sis_GlobalPaletteTable[MAX_SIS_GLOBALPALETTETABLE];
FramePosSize  frame_pos_size[MAX_FRAMES];
int32         current_frame_id=0;
LOCAL int32   current_processing_type_id;

LOCAL boolean saf_decoder_abort_flag;      /* SAF decoder abort status flag*/
LOCAL boolean saf_image_info_flag;         /* SAF file specification flag  */
LOCAL boolean saf_decode_in_progress;      /* SAF decode in progress flag  */
LOCAL saf_state_type saf_state, saf_state_q1; /* states for state machine  */
static uint32  data_position;         /* current position within input file*/
saf_cmx_decode_cb_func_ptr_type saf_cmx_cb = NULL; /* callback for caller     */
void *saf_cmx_handle;                      /* callback handle for cmx data */
LOCAL uint32  *saf_id = 0;                 /* client data for tracking cb's*/
static uint8   chunk_data[2*MAX_DATA_BYTES];    /* chunk data              */
LOCAL int32   next_hdrsize;
LOCAL int32   embedded_img_size;
LOCAL uint8   *user_frame_buf;
LOCAL uint32  user_frame_id;
static saf_cmx_get_data_type cmx_get_data;
LOCAL saf_cmx_continue_type cmx_continue_data;
LOCAL int32 remained_data_size;
LOCAL uint32  saf_file_size;

#ifdef FEATURE_PNG_DECODER
uint8 *sis_png_inbuf;                    
uint8 *sis_png_outbuf;
LOCAL uint32 sis_png_size;               
LOCAL int32 sis_png_w;
LOCAL int32 sis_png_h;
LOCAL int32 sis_png_bit_depth;
LOCAL int32 sis_png_outpos=0;
LOCAL int32 sis_png_state=0;
LOCAL int32 sis_png_outcnt=0;
LOCAL int32 sis_png_start_pt=0;
LOCAL png_cmx_command_cb_func_type png_saf_cb;

#endif

LOCAL void* client_data;
saf_cmx_global_info_type cmx_saf_info; 
uint8 *saf_info_buf_ptr = NULL;

#ifdef FEATURE_RLA_DECODER
#error code not present
#endif

LOCAL void sis_png_init_var(void);

#ifdef FEATURE_PNG_DECODER
void saf_png_cb (png_cmx_status_enum_type png_status, void *handle, void *server_data);
#endif

LOCAL int16 const isin[256] = {
  0, 402, 804, 1205, 1606, 2006, 2404, 2801, 3196, 3590,
  3981,   4370,   4756,   5139,   5520,   5897,   6270,
  6639,   7005,   7366,   7723,   8076,   8423,   8765,
  9102,   9434,   9760,  10080,  10394,  10702,  11003,  11297,
  11585,  11866,  12140,  12406,  12665,  12916,  13160,
  13395,  13623,  13842,  14053,  14256,  14449,  14635,  14811,
  14978,  15137,  15286,  15426,  15557,  15679,  15791,  15893,
  15986,  16069,  16143,  16207,  16261,  16305,  16340,
  16364,  16379,  16384,  16379,  16364,  16340,  16305,  16261,
  16207,  16143,  16069,  15986,  15893,  15791,  15679,
  15557,  15426,  15286,  15137,  14978,  14811,  14635,  14449,
  14256,  14053,  13842,  13623,  13395,  13160,  12916,
  12665,  12406,  12140,  11866,  11585,  11297,  11003,  10702,
  10394,  10080,   9760,   9434,   9102,   8765,   8423,
  8076,   7723,   7366,   7005,   6639,   6270,   5897,   5520,
  5139,   4756,   4370,   3981,   3590,   3196,   2801,   2404,
  2006,   1606,   1205,    804,    402,      0,   -402,   -804,
  -1205,  -1606,  -2006,  -2404,  -2801,  -3196,  -3590,  -3981,
  -4370,  -4756,  -5139,  -5520,  -5897,  -6270,  -6639,  -7005,
  -7366,  -7723,  -8076,  -8423,  -8765,  -9102,  -9434,
  -9760, -10080, -10394, -10702, -11003, -11297, -11585, -11866,
  -12140, -12406, -12665, -12916, -13160, -13395,
  -13623, -13842, -14053, -14256, -14449, -14635, -14811, -14978,
  -15137, -15286, -15426, -15557, -15679, -15791,
  -15893, -15986, -16069, -16143, -16207, -16261, -16305, -16340,
  -16364, -16379, -16384, -16379, -16364, -16340,
  -16305, -16261, -16207, -16143, -16069, -15986, -15893, -15791,
  -15679, -15557, -15426, -15286, -15137, -14978,
  -14811, -14635, -14449, -14256, -14053, -13842, -13623, -13395,
  -13160, -12916, -12665, -12406, -12140, -11866,
  -11585, -11297, -11003, -10702, -10394, -10080,  -9760,  -9434,
  -9102,  -8765,  -8423,  -8076,  -7723,  -7366,  -7005,
  -6639,  -6270,  -5897,  -5520,  -5139,  -4756,  -4370,  -3981,
  -3590,  -3196,  -2801,  -2404,  -2006,  -1606,  -1205,
  -804,   -402
};

LOCAL int16 const icos[256] = 
{
  16384, 16379, 16364, 16340, 16305, 16261, 16207, 16143, 16069, 
  15986, 15893, 15791, 15679, 15557, 15426, 15286, 15137,
  14978, 14811, 14635, 14449, 14256, 14053, 13842, 13623, 13395,
  13160, 12916, 12665, 12406, 12140, 11866, 11585, 11297,
  11003, 10702, 10394, 10080,  9760,  9434,  9102,  8765,  8423,
  8076,  7723,  7366,  7005,  6639,  6270,  5897,  5520,  5139,
  4756,  4370,  3981,  3590,  3196,  2801,  2404,  2006,  1606,
  1205,   804,   402,     0,  -402,  -804, -1205, -1606, -2006,
  -2404, -2801, -3196, -3590, -3981, -4370, -4756, -5139, -5520,
  -5897, -6270, -6639, -7005, -7366, -7723, -8076, -8423,
  -8765, -9102, -9434, -9760,-10080,-10394,-10702,-11003,-11297,
  -11585,-11866,-12140,-12406,-12665,-12916,-13160,
  -13395,-13623,-13842,-14053,-14256,-14449,-14635,-14811,-14978,
  -15137,-15286,-15426,-15557,-15679,-15791,-15893,
  -15986,-16069,-16143,-16207,-16261,-16305,-16340,-16364,-16379,
  -16384,-16379,-16364,-16340,-16305,-16261,-16207,
  -16143,-16069,-15986,-15893,-15791,-15679,-15557,-15426,-15286,
  -15137,-14978,-14811,-14635,-14449,-14256,-14053,
  -13842,-13623,-13395,-13160,-12916,-12665,-12406,-12140,-11866,
  -11585,-11297,-11003,-10702,-10394,-10080, -9760,
  -9434, -9102, -8765, -8423, -8076, -7723, -7366, -7005, -6639, 
  -6270, -5897, -5520, -5139, -4756, -4370, -3981, -3590,
  -3196, -2801, -2404, -2006, -1606, -1205,  -804,  -402,     0,
  402,   804,  1205,  1606,  2006,  2404,  2801,  3196,  3590,
  3981,  4370,  4756,  5139,  5520,  5897,  6270,  6639,  7005,
  7366,  7723,  8076,  8423,  8765,  9102,  9434,  9760, 10080,
  10394, 10702, 11003, 11297, 11585, 11866, 12140, 12406, 12665,
  12916, 13160, 13395, 13623, 13842, 14053, 14256, 14449,
  14635, 14811, 14978, 15137, 15286, 15426, 15557, 15679, 15791,
  15893, 15986, 16069, 16143, 16207, 16261, 16305, 16340,
  16364, 16379
};

LOCAL uint8 const ysk_reversebit_1bit[256] =
{
  0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240, 8, 136, 72,
  200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248, 4, 132, 68, 196, 36, 164,
  100, 228, 20, 148, 84, 212, 52, 180, 116, 244, 12, 140, 76, 204, 44, 172, 108, 236, 28,
  156, 92, 220, 60, 188, 124, 252, 2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50,
  178, 114, 242, 10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
  6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246, 14, 142, 78, 206,
  46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254, 1, 129, 65, 193, 33, 161, 97, 225,
  17, 145, 81, 209, 49, 177, 113, 241, 9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217,
  57, 185, 121, 249, 5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245,
  13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253, 3, 131, 67, 195,
  35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243, 11, 139, 75, 203, 43, 171, 107, 235,
  27, 155, 91, 219, 59, 187, 123, 251, 7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215,
  55, 183, 119, 247, 15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255
};

LOCAL uint8 const ysk_reversebit_2bit[256] =
{
  0, 64, 128, 192, 16, 80, 144, 208, 32, 96, 160, 224, 48, 112, 176, 240, 4, 68, 132, 196, 20, 84, 148, 212,
  36, 100, 164, 228, 52, 116, 180, 244, 8, 72, 136, 200, 24, 88, 152, 216, 40, 104, 168, 232, 56, 120, 184,
  248, 12, 76, 140, 204, 28, 92, 156, 220, 44, 108, 172, 236, 60, 124, 188, 252, 1, 65, 129, 193, 17, 81, 145,
  209, 33, 97, 161, 225, 49, 113, 177, 241, 5, 69, 133, 197, 21, 85, 149, 213, 37, 101, 165, 229, 53, 117, 181,
  245, 9, 73, 137, 201, 25, 89, 153, 217, 41, 105, 169, 233, 57, 121, 185, 249, 13, 77, 141, 205, 29, 93, 157,
  221, 45, 109, 173, 237, 61, 125, 189, 253, 2, 66, 130, 194, 18, 82, 146, 210, 34, 98, 162, 226, 50, 114, 178,
  242, 6, 70, 134, 198, 22, 86, 150, 214, 38, 102, 166, 230, 54, 118, 182, 246, 10, 74, 138, 202, 26, 90, 154,
  218, 42, 106, 170, 234, 58, 122, 186, 250, 14, 78, 142, 206, 30, 94, 158, 222, 46, 110, 174, 238, 62, 126, 190,
  254, 3, 67, 131, 195, 19, 83, 147, 211, 35, 99, 163, 227, 51, 115, 179, 243, 7, 71, 135, 199, 23, 87, 151, 215,
  39, 103, 167, 231, 55, 119, 183, 247, 11, 75, 139, 203, 27, 91, 155, 219, 43, 107, 171, 235, 59, 123, 187, 251,
  15, 79, 143, 207, 31, 95, 159, 223, 47, 111, 175, 239, 63, 127, 191, 255
};

LOCAL uint8 const ysk_reversebit_4bit[256] =
{
  0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 1, 17, 33, 49, 65, 81, 97, 113, 129, 145,
  161, 177, 193, 209, 225, 241, 2, 18, 34, 50, 66, 82, 98, 114, 130, 146, 162, 178, 194, 210, 226, 242, 3, 19, 35,
  51, 67, 83, 99, 115, 131, 147, 163, 179, 195, 211, 227, 243, 4, 20, 36, 52, 68, 84, 100, 116, 132, 148, 164, 180,
  196, 212, 228, 244, 5, 21, 37, 53, 69, 85, 101, 117, 133, 149, 165, 181, 197, 213, 229, 245, 6, 22, 38, 54, 70,
  86, 102, 118, 134, 150, 166, 182, 198, 214, 230, 246, 7, 23, 39, 55, 71, 87, 103, 119, 135, 151, 167, 183, 199,
  215, 231, 247, 8, 24, 40, 56, 72, 88, 104, 120, 136, 152, 168, 184, 200, 216, 232, 248, 9, 25, 41, 57, 73, 89, 105,
  121, 137, 153, 169, 185, 201, 217, 233, 249, 10, 26, 42, 58, 74, 90, 106, 122, 138, 154, 170, 186, 202, 218, 234,
  250, 11, 27, 43, 59, 75, 91, 107, 123, 139, 155, 171, 187, 203, 219, 235, 251, 12, 28, 44, 60, 76, 92, 108, 124, 140,
  156, 172, 188, 204, 220, 236, 252, 13, 29, 45, 61, 77, 93, 109, 125, 141, 157, 173, 189, 205, 221, 237, 253, 14, 30,
  46, 62, 78, 94, 110, 126, 142, 158, 174, 190, 206, 222, 238, 254, 15, 31, 47, 63, 79, 95, 111, 127, 143, 159, 175,
  191, 207, 223, 239, 255
};

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_prepare_method

DESCRIPTION
  This function sets the appropriate .

DEPENDENCIES
  None

RETURN VALUE
  -1 if ERROR otherwise 0.

SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_prepare_method
(
  saf_imageHeaderType* header
)
{
  saf_ro_sc_mi = saf_rotation_scale_mirror_124bit;
  switch (header->ImageLevel)
  {
    case 1 :
      edgedetection = EdgeDetection_1bpp;
      break;
    case 2 :
      edgedetection = EdgeDetection_2bpp;
      break;
    case 4 :
      break;
    case 8 :
      saf_ro_sc_mi = saf_rotation_scale_mirror_8bit;
      break;
    default :
      return (-1);
  }
  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_prepare_one_frame

DESCRIPTION
  This function sets the appropriate .

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_prepare_one_frame
(
  uint8* raw_frameinfo, 
  uint8* outbuf
)
{
  int32 ret;
  int32 temp = sis_display_info_data.width * sis_display_info_data.height 
               * saf_header_t.ImageLevel / 8;
 
  /* determine the method for generating animation frame                   */         
  ret = sis_dec_prepare_method(&saf_header_t);
  if (ret < 0)
  {
    return (ret);
  }

  /* process frame command header                                          */
  ret = sis_dec_frame_cmd(raw_frameinfo, &saf_frameInfo_t);

  /* intialize frame buffer based on bit depth of animation frame          */
  if (saf_header_t.ImageLevel == 4)
  {
    memset(outbuf, (saf_frameInfo_t.FrameBackGroundColor << 4) 
           + saf_frameInfo_t.FrameBackGroundColor, temp);
  }
  else if (saf_header_t.ImageLevel == 2)
  {
    if (saf_frameInfo_t.FrameBackGroundColor == 0)
      memset(outbuf, 0, temp);
    else if (saf_frameInfo_t.FrameBackGroundColor == 1)
      memset(outbuf, 85, temp);
    else if (saf_frameInfo_t.FrameBackGroundColor == 2)
      memset(outbuf, 170, temp);
    else // 3
      memset(outbuf, 255, temp);
  }
  else if (saf_header_t.ImageLevel == 1)
  {
    if (saf_frameInfo_t.FrameBackGroundColor == 0)
      memset(outbuf, 0, temp);
    else // 1
      memset(outbuf, 255, temp);
  }
  else // 8 bit
  {
    memset(outbuf,saf_frameInfo_t.FrameBackGroundColor, temp);
  }

  sis_current_mv_id = 0;
  sis_current_frame_outbuf = outbuf;

  return (ret);
}


/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_make_one_frame

DESCRIPTION
  This function generates a single animation frame             

DEPENDENCIES
  None

RETURN VALUE
  Number of pixels processed

SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_make_one_frame( void )
{
  saf_typeMotionInfo *mv;
  srect frClipRect;
  srect validRgn;
  srect realRgn;
  srect ClipRgn;
  srect validRgn_original;

  uint8 *inbuf;
  uint8 *outbuf;
  int32 image_size;
  uint8 mv_size;

  image_size = sis_display_info_data.width * sis_display_info_data.height;

  if ( sis_current_mv_id >= saf_frameInfo_t.NumofMovementData)
    return (0);

  outbuf = sis_current_frame_outbuf;

  if (saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos] == 
      SIS_DYNAMIC_EFFECTS_ID_MV)
  {
    mv = &(saf_frameInfo_t.motionData);
    mv_size = sis_dec_mv(&(saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos]),mv);

    if(mv->TypeID >= MAX_TYPES)
    {
      return (-2);
    }
    saf_frameInfo_t.pos += mv_size;
    if (saf_valid_region_check(sis_display_info_data.width, 
                               sis_display_info_data.height,
                 mv,
                 &validRgn,
                 &realRgn,  
                               &(saf_typeInfo_t[mv->TypeID])) != 0)
  {
      sis_current_mv_id++;
      return (image_size);
  }

    if ((validRgn.right - validRgn.left) <= 0 
        || (validRgn.bottom - validRgn.top) <= 0)
  {
      sis_current_mv_id++;
      return (image_size);
  }

    inbuf = saf_typeInfo_t[mv->TypeID].imageData;

    /* valid region adjusting if clip region is set */
    validRgn_original = validRgn;
    if(mv->ClipFlag != 0)
    {
      ClipRgn.left   = mv->clip_left;
      ClipRgn.top    = mv->clip_top;
      ClipRgn.right  = mv->clip_right;
      ClipRgn.bottom = mv->clip_bottom;
      
      if(ClipRgn.left < validRgn.left)
        ClipRgn.left = validRgn.left;
      
      if(ClipRgn.right > validRgn.right)
        ClipRgn.right = validRgn.right;
      
      if(ClipRgn.top < validRgn.top)
        ClipRgn.top = validRgn.top;
      
      if(ClipRgn.bottom > validRgn.bottom)
        ClipRgn.bottom = validRgn.bottom;

      validRgn_original = validRgn;
      validRgn = ClipRgn;
    }

    /* if not overwrite and bit depth is 1 or 2 then set approprite buffers
    ** and prepare to do edge dectection
    */
    if(saf_header_t.ImageLevel == 1 || saf_header_t.ImageLevel == 2)
    {
      memset(sis_display_info_data.pOperationBuf1,
             0,
             (image_size * saf_header_t.ImageLevel) >> 3);
      memset(sis_display_info_data.pOperationBuf2,
             0,
             (image_size * saf_header_t.ImageLevel) >> 3);
      
      if(mv->transparency_type == 1 || mv->transparency_type == 3) 
      {
        if(saf_header_t.ImageLevel == 1)
          saf_edge_detection_prepare_1bit(inbuf,
                                          sis_display_info_data.pOperationBuf2, 
                                          sis_display_info_data.width,
                                          sis_display_info_data.height,
                                          mv,
                                          &(saf_typeInfo_t[mv->TypeID]),
                                          saf_header_t.ImageLevel,
                                          &validRgn, 
                                          &realRgn);
        else
          saf_edge_detection_prepare_2bit(inbuf,
                                          sis_display_info_data.pOperationBuf2, 
                                          sis_display_info_data.width,
                                          sis_display_info_data.height,
                                          mv,
                                          &(saf_typeInfo_t[mv->TypeID]),
                                          saf_header_t.ImageLevel,
                                          &validRgn, 
                                          &realRgn);
        
        saf_rotation_scale_mirror_124bit(inbuf,
                                         sis_display_info_data.pOperationBuf1,
                                         sis_display_info_data.width,
                                         sis_display_info_data.height,
                                         mv,
                                         &(saf_typeInfo_t[mv->TypeID]),
                                         &validRgn_original,
                                         saf_header_t.ImageLevel);
        
        if(mv->ClipFlag != 0)
          saf_clip_rectangular_124bpp(sis_display_info_data.pOperationBuf1,
                                      sis_display_info_data.width,
                                      sis_display_info_data.height,
                                      &validRgn,
                                      0,
                                      saf_header_t.ImageLevel);

        sis_ro_sc_postprocess(outbuf,
                              sis_display_info_data.width,
                              sis_display_info_data.height,
                              mv,
                              &(saf_typeInfo_t[mv->TypeID]),
                              &validRgn,
                              &realRgn);
      }
      else /* mv->transparency_type == 0 */
      {
        if(mv->ClipFlag != 0)
        {
          saf_rotation_scale_mirror_124bit(inbuf,
                                           sis_display_info_data.pOperationBuf1,
                                           sis_display_info_data.width,
                                           sis_display_info_data.height,
                                           mv,
                                           &(saf_typeInfo_t[mv->TypeID]),
                                           &validRgn_original,
                                           saf_header_t.ImageLevel);
          saf_clip_rectangular_124bpp(sis_display_info_data.pOperationBuf1,
                                      sis_display_info_data.width,
                                      sis_display_info_data.height,
                                      &validRgn,
                                      0,
                                      saf_header_t.ImageLevel);

          memset(sis_display_info_data.pOperationBuf2,
                 255,
                 (image_size * saf_header_t.ImageLevel) >> 3);
          saf_clip_rectangular_124bpp(sis_display_info_data.pOperationBuf2,
                                      sis_display_info_data.width,
                                      sis_display_info_data.height,
                                      &validRgn,
                                      0,
                                      saf_header_t.ImageLevel);
          
          sis_ro_sc_postprocess(outbuf,
                                sis_display_info_data.width,
                                sis_display_info_data.height,
                                mv,
                                &(saf_typeInfo_t[mv->TypeID]),
                                &validRgn,
                                &realRgn);
        }
        else
          saf_rotation_scale_mirror_124bit(inbuf,
                                           outbuf,
                                           sis_display_info_data.width,
                                           sis_display_info_data.height,
                                           mv,
                                           &(saf_typeInfo_t[mv->TypeID]),
                                           &validRgn,
                                           saf_header_t.ImageLevel);
      }
    }
    else if(saf_header_t.ImageLevel == 4)
    {
      if(mv->ClipFlag != 0)
      {
          memset(sis_display_info_data.pOperationBuf1,
                 255,
                 (image_size * saf_header_t.ImageLevel) >> 3);

          saf_rotation_scale_mirror_124bit(inbuf,
                                           sis_display_info_data.pOperationBuf1,
                                           sis_display_info_data.width,
                                           sis_display_info_data.height,
                                           mv,
                                           &(saf_typeInfo_t[mv->TypeID]),
                                           &validRgn_original,
                                           saf_header_t.ImageLevel);

          sis_ro_sc_postprocess(outbuf,
                                sis_display_info_data.width,
                                sis_display_info_data.height,
                                mv,
                                &(saf_typeInfo_t[mv->TypeID]),
                                &validRgn,
                                &realRgn);
      }
      else
      {
        if(mv->transparency_type != 0)
        {
      saf_rotation_scale_mirror_4bit_transcolor(inbuf,
                                                    outbuf,
                                                    sis_display_info_data.width,
                                                    sis_display_info_data.height,
                                                    mv,
                                                    &(saf_typeInfo_t[mv->TypeID]),
                                                    &validRgn,
                                                    saf_header_t.ImageLevel);
        }
        else
        {
          saf_rotation_scale_mirror_124bit(inbuf,
                                           outbuf,
                                           sis_display_info_data.width,
                                           sis_display_info_data.height,
                                           mv,
                                           &(saf_typeInfo_t[mv->TypeID]),
                                           &validRgn,
                                           saf_header_t.ImageLevel);
        }
      }
    }
    else /* saf_header_t.ImageLevel == 8 */
    {
    saf_rotation_scale_mirror_8bit(inbuf,
                                       outbuf,
                                   sis_display_info_data.width,
                                       sis_display_info_data.height,
                                       mv,
                                       &(saf_typeInfo_t[mv->TypeID]),
                                       &validRgn,
                                       saf_header_t.ImageLevel);
    }
  }
  else
  {
    switch (saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos])
    {
      case SIS_DYNAMIC_EFFECTS_ID_RECT_CLIP:

        frClipRect.left = (uint8) saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos+2];
        frClipRect.top = (uint8) saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos+3];
        frClipRect.right = (uint8) saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos+4];
        frClipRect.bottom = (uint8) saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos+5];

        if (frClipRect.left >= (int32) sis_display_info_data.width)
          frClipRect.left = sis_display_info_data.width - 1;

        if (frClipRect.right >= (int32) sis_display_info_data.width)
          frClipRect.right = sis_display_info_data.width - 1;

        if (frClipRect.top >= (int32) sis_display_info_data.height)
          frClipRect.top = sis_display_info_data.height - 1;

        if (frClipRect.bottom >= (int32) sis_display_info_data.height)
          frClipRect.bottom = sis_display_info_data.height - 1;

        if (frClipRect.left < frClipRect.right && frClipRect.top < frClipRect.bottom)
        {
          if (saf_header_t.ImageLevel == 8)
            saf_clip_rectangular_8bpp(outbuf,sis_display_info_data.width,
                                      sis_display_info_data.height,&frClipRect,
                                      saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos+6]);
          else
            saf_clip_rectangular_124bpp(outbuf,sis_display_info_data.width,
                                        sis_display_info_data.height,&frClipRect,
                                        saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos+6], 
                                        saf_header_t.ImageLevel);
        }
        break;

      case SIS_DYNAMIC_EFFECTS_ID_INVERSION:

        saf_inversion(outbuf,
                      sis_display_info_data.width*saf_header_t.ImageLevel / 8,
                      sis_display_info_data.height);
        break;
      default:
        break;
    }
    saf_frameInfo_t.pos +=  (saf_frameInfo_t.DynamicDataPt[saf_frameInfo_t.pos + 1] + 2);
  }

  sis_current_mv_id++;

  return (sis_display_info_data.width*sis_display_info_data.height);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_g_header

DESCRIPTION
  This function sets the appropriate .

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_g_header
(
  uint8 *inbuf, 
  saf_imageHeaderType *info
)
{
  int32 cmd_size;

  if ( inbuf[0] != SIS_GLOBAL_HEADER_CMD )
    return (-1);

  cmd_size = inbuf[1];

  info->sizeX = inbuf[2];
  info->sizeY = inbuf[3];
  info->ImageLevel = inbuf[4];
  info->CompressionScheme = inbuf[5];
  info->numberOfLayer = inbuf[6] ;
  info->LoopCount = inbuf[7];

  return (cmd_size+2);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_g_tr_cmd

DESCRIPTION
  This function sets the appropriate .

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_g_tr_cmd
(
  uint8 *inbuf, 
  saf_imageHeaderType *info
)
{
  int32 cmd_size;

  if (inbuf[0] != SIS_GLOBAL_TRANSPARENCY_CMD)
    return (-1);

  cmd_size = inbuf[1];
  info->global_transparency_count = inbuf[2];

  if (info->ImageLevel <=8)
    memcpy(info->global_transparency,&inbuf[3], 
           info->global_transparency_count * 2);
  else if (info->ImageLevel == 24)
    memcpy(info->global_transparency,&inbuf[3], 
           info->global_transparency_count * 4);

  info->UseGlobalTransparency = 1;

  return (cmd_size+2);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_g_image_cmd

DESCRIPTION
  
  This function processes the global palette data for 256 color and grayscale

DEPENDENCIES
  None

RETURN VALUE
  Command data size
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_g_image_cmd
(
  uint8* inbuf,
  saf_imageHeaderType* info
)
{
  int32 cmd_size;
  int32 pal_size=0;
  int32 i;

  cmd_size = inbuf[1];                            /* set size of command   */

  info->numberOfImageTypes = inbuf[2];            
  info->paletteType = (inbuf[3] & 0x60) >> 5;    /* get palette type       */

  /* Palette types include:
  **   1 - 256 color palette
  **   2 - 256 grayscale palette
  **   3 - Reserved palette
  */
  if (info->paletteType==1)
  {
    pal_size = (((int32) inbuf[4]) << 8) | inbuf[5]; /* extract plte size  */ 
    memcpy(sis_GlobalPaletteTable, (char *)&(inbuf[6]), pal_size);
    cmd_size += pal_size + 2;
  }
  else if (info->paletteType == 2)
  {
    /* if grayscale palette then generate palette where each palette 
    ** entry contains the same component value
    */
    for(i = 0; i < 256; i++)
    {
       sis_GlobalPaletteTable[3 * i] = i;
       sis_GlobalPaletteTable[3 * i + 1] = i;
       sis_GlobalPaletteTable[3 * i + 2] = i;
    }
  }
  else if (info->paletteType ==3)
  {
    pal_size = (((int32)inbuf[4])<<8) | inbuf[5];
    cmd_size += pal_size + 2;
  }

  return (cmd_size+2);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_g_frame_info_cmd

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_g_frame_info_cmd
(
  uint8 *inbuf,
  saf_imageHeaderType *info
)
{
  int32 cmd_size;

  if ( inbuf[0] != SIS_GLOBAL_FRAME_INFO_CMD )
    return (-1);

  cmd_size = inbuf[1];

  info->numberOfFrames = inbuf[2];
  info->Best = inbuf[3];
  info->MvDataSize = inbuf[4];

  return (cmd_size+2);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_g_delay_info_cmd

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_g_delay_info_cmd
(
  uint8 *inbuf, 
  saf_imageHeaderType *info
)
{
  int32 cmd_size;
  int32 i;

  if ( inbuf[0] != SIS_GLOBAL_DELAY_INFO_CMD )
    return -1;

  cmd_size = inbuf[1];
  info->delay[0] = (inbuf[2]<<8) | inbuf[3];
  if (info->delay[0] != 0)
  {
    info->variableDelayorNot=0;
  }
  else
  {
    info->variableDelayorNot=1;
    for (i=0; i < info->numberOfFrames; i++)
      info->delay[i] = inbuf[2 * i + 4] << 8 | inbuf[2 * i + 5];
  }
  return (cmd_size+2);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_frame_cmd

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_frame_cmd
(
  uint8 *inbuf, 
  saf_frameInfo *frame
)
{
  int32 size;
  int32 pos;

  if (inbuf[0] != SIS_FRAME_CMD)
    return (-1);

  size = ((int32) inbuf[1] << 8) | inbuf[2];
  frame->FrameBackGroundColor = inbuf[3];
  frame->NumofMovementData = inbuf[4];
  pos=5;
  frame->DynamicDataPt = &inbuf[pos];
  frame->pos = 0;

  return (size + 3);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_mv

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_mv
(
  uint8 *inbuf, 
  saf_typeMotionInfo *mv
)
{
  int32 pos=0;
  int32 cmdsize;
  int32 xoffset_sign,yoffset_sign;

  //inbuf[0] is id
  cmdsize = inbuf[1];
  mv->TypeID = inbuf[2];

  if ((inbuf[3] & 0x80) !=0)
    xoffset_sign = -1;
  else
    xoffset_sign = 1;

  if ((inbuf[3] & 0x40) !=0)
    yoffset_sign = -1;
  else
    yoffset_sign = 1;

  if ((inbuf[3] & 0x20) != 0)
    mv->rightToLeft = TRUE;
  else
    mv->rightToLeft = FALSE;

  if ((inbuf[3] & 0x10) != 0)
    mv->upsideDown = TRUE;
  else
    mv->upsideDown = FALSE;

  if ((inbuf[3] & 0x08) != 0)
    mv->Rotation = TRUE;
  else
    mv->Rotation = FALSE;

  if ((inbuf[3] & 0x04) != 0)
    mv->scaling_algorithm = 1;
  else
    mv->scaling_algorithm = 0;

  if ((inbuf[3] & 0x02) != 0)
    mv->ClipFlag = TRUE;
  else
    mv->ClipFlag = FALSE;

  if ((inbuf[3] & 0x01) != 0)
    mv->rotation_angle_flag = 1;
  else
    mv->rotation_angle_flag = 0;


  mv->x_scale_type = (inbuf[4] >> 6) & 0x03;
  mv->y_scale_type = (inbuf[4] >> 4) & 0x03;
  mv->transparency_type = (inbuf[4]) & 0x0f;

  mv->xPosition = xoffset_sign * inbuf[5];
  mv->yPosition = yoffset_sign * inbuf[6];

  pos = 7;
  if (mv->x_scale_type != 0)
    mv->x_scalefactor = inbuf[pos++];
  else
    mv->x_scalefactor = 0;

  if (mv->y_scale_type != 0)
    mv->y_scalefactor = inbuf[pos++];
  else
    mv->y_scalefactor = 0;

  if (mv->rotation_angle_flag != 0)
    mv->rotation_angle=inbuf[pos++];
  else
    mv->rotation_angle = 0;

  if (mv->ClipFlag)
  {
    mv->clip_left = inbuf[pos++];
    mv->clip_right = inbuf[pos++];
    mv->clip_top = inbuf[pos++];
    mv->clip_bottom = inbuf[pos++];
  }
  else
  {
    mv->clip_left = 0;
    mv->clip_right = 0;
    mv->clip_top = 0;
    mv->clip_bottom = 0;
  }

  return (cmdsize + 2);
}


/* <EJECT> */
/*===========================================================================

FUNCTION  saf_global_init

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void saf_global_init
(
  saf_display_resource_info_type *display_info_data,
  uint8 *g_object_buf,
  uint32 g_object_buf_size,
  uint8 *g_coder_buf,
  uint32 g_coder_buf_size,
  uint8 *g_info_buf
)
{
  memcpy(&sis_display_info_data, display_info_data, 
         sizeof(saf_display_resource_info_type));
  sis_GlobalObjectBuffer = g_object_buf;
  sis_size_GlobalObjectBuffer = g_object_buf_size;

  png_set_buf(g_coder_buf, g_coder_buf_size, NULL, 0);

#ifdef FEATURE_RLA_DECODER
#error code not present
#endif
#ifdef FEATURE_MLZ_DECODER
  SMLZ_InitDecoder_static(g_coder_buf,(unsigned short)g_coder_buf_size);
#endif

  saf_info_buf_ptr = g_info_buf;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_decode_init

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void saf_decode_init ( void )
{
  data_position = 0;
  sis_pos_GlobalObjectBuffer = 0;
  current_frame_id = 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_saf

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void process_saf(void *cb_data)
{
  saf_state_type saf_state_q2 = SAF_STATE_INIT;
  boolean exit_saf_decoder = FALSE;
  saf_cmx_prepare_done_type cmx_done_data;      /* prepare done structure  */
  saf_cmx_frame_done_type cmx_frame_done_data;  /* frame done structure    */
  int32 command_type;             /* command ID for 2 byte size command    */
  int32 command_hdr_length;       /* length of command header in bytes     */
  int32 ret_from_frame;           /* return code from make one frame       */
#ifdef FEATURE_RLA_DECODER
#error code not present
#endif

  /* make sure that ID's match if not then abort current SAF decode        */
  if (cb_data != saf_id)          
    return;

  /* if abort flag set, then abort decoder                                 */
  if (saf_decoder_abort_flag)
  {
    if (saf_cmx_cb != NULL)
      saf_cmx_cb(SAF_CMX_ABORT, saf_cmx_handle, 0);
    saf_state = SAF_STATE_INIT;
    saf_decoder_abort_flag = FALSE;
    saf_cmx_cb = NULL;
    saf_decode_in_progress = FALSE;
    saf_file_size = 0;
    return;
  }

  do
  {
    switch ( saf_state )
    {
      /* SAF initialization state.  Initialize decoder variables           */
      case SAF_STATE_INIT :

        saf_decode_init();
        cmx_continue_data.callback_ptr =
        (saf_cmx_command_cb_func_ptr_type) process_saf;
        cmx_continue_data.client_data = (void*)saf_id;

        cmx_get_data.buffer = chunk_data;
        cmx_get_data.num_bytes = SAF_PREHEADER_SIZE;   /* signature size   */
        cmx_get_data.position = data_position;
        cmx_get_data.callback_ptr =
        (saf_cmx_command_cb_func_ptr_type) process_saf;
        cmx_get_data.client_data = (void *) saf_id;
        data_position += SAF_PREHEADER_SIZE;
        saf_state = SAF_STATE_GET_FILE_SIZE;
        saf_state_q1 = SAF_STATE_GET_DATA;
        break;

        /*Get SAF file size */
      case SAF_STATE_GET_FILE_SIZE :

        if (saf_cmx_cb != NULL)
          saf_cmx_cb(SAF_CMX_GET_FILE_SIZE, saf_cmx_handle, &saf_file_size);
        saf_state = saf_state_q1;
        saf_state_q1 = SAF_STATE_CHECK_PREHEADER;
        break;

      /* Get SAF data state.   Request data from CMX client                */
      case SAF_STATE_GET_DATA :

        if (saf_cmx_cb != NULL)
          saf_cmx_cb(SAF_CMX_GET_DATA, saf_cmx_handle, &cmx_get_data);
        saf_state = saf_state_q1;
        saf_state_q1 = saf_state_q2;
        exit_saf_decoder = TRUE;
        break;

      /* check the preheader for the appropriate signature                 */
      case SAF_STATE_CHECK_PREHEADER:

        /* if not proper signature then abort SAF session                  */
        if (sis_dec_preheader((uint8 *) chunk_data, &(saf_header_t.ver), 
                              &(saf_header_t.size)) < 0)
        {

          if (saf_cmx_cb != NULL)
            saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);

          saf_state = SAF_STATE_INIT;
          exit_saf_decoder = TRUE;
          saf_decode_in_progress = FALSE;
          saf_file_size = 0;
          saf_cmx_cb = NULL;
          break;
        }
        saf_state = SAF_STATE_GET_NEXT_COMMAND;
        break;

      /* get next SAF command from CMX client                              */
      case SAF_STATE_GET_NEXT_COMMAND :

        cmx_get_data.buffer = chunk_data;
        cmx_get_data.num_bytes = 3;           /* includes ID and size bytes*/
        cmx_get_data.position = data_position;
        cmx_get_data.callback_ptr =
        (saf_cmx_command_cb_func_ptr_type) process_saf;
        cmx_get_data.client_data = (void *) saf_id;
        data_position += 3;
        saf_state = SAF_STATE_GET_DATA;
        saf_state_q1 = SAF_STATE_CHECK_COMMAND;
        break;

      /* check SAF state command for processing command                    */
      case SAF_STATE_CHECK_COMMAND :

        command_type = chunk_data[0];            /* command ID             */
        /* length of command                                               */
        command_hdr_length = chunk_data[1];      
        command_hdr_length = (command_hdr_length << 8) | chunk_data[2];

        /* process global header command.  Contains subcommands to process */
        /* header, image, frame, transparency, and delay information       */
        if (command_type == SIS_TOTAL_HEADER_CMD)
        {
          saf_state_q1 = SAF_STATE_PROCESS_TOTAL_HEADER;
          cmx_get_data.buffer = chunk_data;
          cmx_get_data.num_bytes = command_hdr_length;
          cmx_get_data.position = data_position;
          cmx_get_data.callback_ptr =
          (saf_cmx_command_cb_func_ptr_type) process_saf;
          cmx_get_data.client_data = (void *) saf_id;
          data_position += command_hdr_length;
          saf_state = SAF_STATE_GET_DATA;
          break;
        }  /* Image object command processing                              */
        else if (command_type == SIS_NORMAL_TYPE_CMD)
        {
          saf_state_q1 = SAF_STATE_PROCESS_IMAGE;
          cmx_get_data.buffer = chunk_data;
          cmx_get_data.num_bytes = command_hdr_length;
          cmx_get_data.position = data_position;
          cmx_get_data.callback_ptr =
          (saf_cmx_command_cb_func_ptr_type) process_saf;
          cmx_get_data.client_data = (void *) saf_id;
          data_position += command_hdr_length;
          saf_state = SAF_STATE_GET_DATA;
          break;
        }  /* process frame command                                        */
        else if (command_type == SIS_FRAME_CMD)
        {
          /* save frame position and size.  Adjust by 3: 1 command byte and 
          ** 2 command header bytes
          */
          saf_func_save_frame_pos_and_size(data_position-3, 
                                                    command_hdr_length+3);
          saf_state_q1 = SAF_STATE_PROCESS_FRAME_INFO;
          cmx_get_data.buffer = chunk_data;
          cmx_get_data.num_bytes = command_hdr_length;
          cmx_get_data.position = data_position;
          cmx_get_data.callback_ptr =
          (saf_cmx_command_cb_func_ptr_type) process_saf;
          cmx_get_data.client_data = (void *) saf_id;
          data_position += command_hdr_length;
          saf_state = SAF_STATE_GET_DATA;
          break;
        } /* end of animation command                                      */
        else if (command_type == SIS_END_CMD)
        {
          saf_state = SAF_STATE_END;
          break;
        }
#ifdef FEATURE_MLZ_DECODER
  else if(command_type == SIS_D_HUFF_TABLE_CMD)
  {
    saf_state_q1 = SAF_STATE_D_HUFF_TABLE_CMD;
    cmx_get_data.buffer = chunk_data;
          cmx_get_data.num_bytes = command_hdr_length;
          cmx_get_data.position = data_position;
          cmx_get_data.callback_ptr =
          (saf_cmx_command_cb_func_ptr_type) process_saf;
          cmx_get_data.client_data = (void *) saf_id;
          data_position += command_hdr_length;
          saf_state = SAF_STATE_GET_DATA;
      break;
  }
#endif   
        /* skip commands not supported by this player                   */
        else
        {
          /* if not appropriate size then abort player                     */
          if (data_position >= saf_header_t.size)
          {
            if (saf_cmx_cb != NULL)
              saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
            saf_state = SAF_STATE_INIT;
            exit_saf_decoder = TRUE;
            saf_decode_in_progress = FALSE;
            saf_file_size = 0;
            saf_cmx_cb = NULL;
            break;
          }
          /* if length is smaller than 2*MAX_DATA_BYTES then process data  */
          /* else process the data in multiple chunks                      */
          if (command_hdr_length <= 2*MAX_DATA_BYTES)
          {
            saf_state_q1 = SAF_STATE_GET_NEXT_COMMAND;
            cmx_get_data.buffer = chunk_data;
            cmx_get_data.num_bytes = command_hdr_length;
            cmx_get_data.position = data_position;
            cmx_get_data.callback_ptr =
            (saf_cmx_command_cb_func_ptr_type) process_saf;
            cmx_get_data.client_data = (void *) saf_id;
            data_position += command_hdr_length;
            saf_state = SAF_STATE_GET_DATA;
          }
          else
          {
            remained_data_size = command_hdr_length;
            saf_state_q1 = SAF_STATE_GET_UNKNOWN_DATA;
            cmx_get_data.buffer = chunk_data;
            cmx_get_data.num_bytes = 2*MAX_DATA_BYTES;
            remained_data_size -= 2*MAX_DATA_BYTES;
            cmx_get_data.position = data_position;
            cmx_get_data.callback_ptr = (saf_cmx_command_cb_func_ptr_type) process_saf;
            cmx_get_data.client_data = (void *) saf_id;
            data_position += 2*MAX_DATA_BYTES;
            saf_state = SAF_STATE_GET_DATA;
          }

        }
        break;
#ifdef FEATURE_MLZ_DECODER
    case SAF_STATE_D_HUFF_TABLE_CMD:
      if(SMLZ_DecodeTable(chunk_data,cmx_get_data.num_bytes)!=0)
      {
        if (saf_cmx_cb != NULL)
          saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
        saf_state = SAF_STATE_INIT;
        exit_saf_decoder = TRUE;
        saf_decode_in_progress = FALSE;
        saf_file_size = 0;
        saf_cmx_cb = NULL;
        break;
      }
      saf_state = SAF_STATE_GET_NEXT_COMMAND;
      break;
#endif
      /* Process data commands larger than 2*MAX_DATA_BYTES                */
      case SAF_STATE_GET_UNKNOWN_DATA:

        if (remained_data_size > 2*MAX_DATA_BYTES)
        {
          saf_state_q1 = SAF_STATE_GET_UNKNOWN_DATA;
          cmx_get_data.buffer = chunk_data;
          cmx_get_data.num_bytes = 2*MAX_DATA_BYTES;
          remained_data_size -= 2*MAX_DATA_BYTES;
          cmx_get_data.position = data_position;
          cmx_get_data.callback_ptr = (saf_cmx_command_cb_func_ptr_type) process_saf;
          cmx_get_data.client_data = (void *) saf_id;
          data_position += 2*MAX_DATA_BYTES;
          saf_state = SAF_STATE_GET_DATA;
        }
        else
        {
          saf_state_q1 = SAF_STATE_GET_NEXT_COMMAND;
          cmx_get_data.buffer = chunk_data;
          cmx_get_data.num_bytes = remained_data_size;
          cmx_get_data.position = data_position;
          cmx_get_data.callback_ptr = (saf_cmx_command_cb_func_ptr_type) process_saf;
          cmx_get_data.client_data = (void *) saf_id;
          data_position += remained_data_size;
          saf_state = SAF_STATE_GET_DATA;
        }
        break;
      
      /* extract 2 byte length for sum length of the subcommands           */
      case SAF_STATE_PROCESS_TOTAL_HEADER :

        if (( next_hdrsize =
              saf_func_check_total_header((uint8*)chunk_data) ) < 0)
        {

          if (saf_cmx_cb != NULL)
            saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
          saf_state = SAF_STATE_INIT;
          exit_saf_decoder = TRUE;
          saf_decode_in_progress = FALSE;
          saf_file_size = 0;
          saf_cmx_cb = NULL;
          break;
        }
        cmx_get_data.buffer = chunk_data;
        cmx_get_data.num_bytes = next_hdrsize;
        cmx_get_data.position = data_position;
        cmx_get_data.callback_ptr =
        (saf_cmx_command_cb_func_ptr_type) process_saf;
        cmx_get_data.client_data = (void*) saf_id;
        data_position += next_hdrsize;
        saf_state = SAF_STATE_GET_DATA;
        saf_state_q1 = SAF_STATE_PROCESS_HEADERS;
        break;

      /* process the global header to get palette data, dimensions, ...    */
      case SAF_STATE_PROCESS_HEADERS :

        if ( sis_dec_process_header_group((uint8*)chunk_data, next_hdrsize) < 0 )
        {
          if (saf_cmx_cb != NULL)
            saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);

          saf_state = SAF_STATE_INIT;
          exit_saf_decoder = TRUE;
          saf_decode_in_progress = FALSE;
          saf_file_size = 0;
          saf_cmx_cb = NULL;
          break;
        }
        /* return global data to client including frame size and palette   */
        if (saf_cmx_cb != NULL)
        {
          saf_func_get_global_info(&cmx_saf_info);
          saf_cmx_cb(SAF_CMX_INFO, saf_cmx_handle, (void *) &cmx_saf_info);
        }
        if (saf_image_info_flag)
        {
          saf_image_info_flag = FALSE;
          saf_state = SAF_STATE_INIT;
          exit_saf_decoder = TRUE;
          saf_decode_in_progress = FALSE;
          saf_file_size = 0;
          saf_cmx_cb = NULL;
          break;
        }
        saf_state = SAF_STATE_GET_NEXT_COMMAND;
        break;

      case SAF_STATE_PROCESS_IMAGE :
        /* process object header information                               */
        if ( sis_dec_get_normal_type_header(chunk_data,saf_typeInfo_t) < 0 )
        {
          if (saf_cmx_cb != NULL)
            saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
          saf_state = SAF_STATE_INIT;
          exit_saf_decoder = TRUE;
          saf_decode_in_progress = FALSE;
          saf_file_size = 0;
          saf_cmx_cb = NULL;
          break;
        }
        
        /* setup request for compressed image                              */
        cmx_get_data.buffer = chunk_data;
        cmx_get_data.num_bytes = 2;
        cmx_get_data.position = data_position;
        cmx_get_data.callback_ptr =
        (saf_cmx_command_cb_func_ptr_type) process_saf;
        cmx_get_data.client_data = (void *) saf_id;
        data_position += 2;
        saf_state = SAF_STATE_GET_DATA;
        saf_state_q1 = SAF_STATE_START_EMBEDDED_IMAGE;
        break;

      case SAF_STATE_START_EMBEDDED_IMAGE:

        /* calculate size of embedded image and set ptr to image data     */
        embedded_img_size = (chunk_data[0] << 8) | chunk_data[1];
        saf_func_embedded_img_init();

        /* if PNG compression used then call PNG decoder                  */
        if (saf_typeInfo_t[current_processing_type_id].imageType==SIS_IMAGE_TYPE_PNG)
        {
#ifdef FEATURE_PNG_DECODER
          sis_png_start_pt = data_position;
          sis_png_init_var();
          sis_png_outbuf= &(sis_GlobalObjectBuffer[sis_pos_GlobalObjectBuffer]);
          sis_png_size = embedded_img_size;
          png_decode(sis_png_inbuf,(png_cmx_decode_cb_func_type)saf_png_cb);
          exit_saf_decoder = TRUE;
#else
          data_position += embedded_img_size;
          saf_typeInfo_t[current_processing_type_id].sizeX=0;
          saf_typeInfo_t[current_processing_type_id].sizeY=0;
          saf_typeInfo_t[current_processing_type_id].imageData = &sis_GlobalObjectBuffer[sis_pos_GlobalObjectBuffer];
          saf_state = SAF_STATE_GET_NEXT_COMMAND;
#endif

        }
        else if (saf_typeInfo_t[current_processing_type_id].imageType==SIS_IMAGE_TYPE_RLA)
        {
#ifdef FEATURE_RLA_DECODER
#error code not present
#else
          /* if image is RLA encoded then abort player                     */
          if (saf_cmx_cb != NULL)
            saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
          saf_state = SAF_STATE_INIT;
          exit_saf_decoder = TRUE;
          saf_decode_in_progress = FALSE;
          saf_file_size = 0;
          saf_cmx_cb = NULL;
#endif
        }
        else if (saf_typeInfo_t[current_processing_type_id].imageType==SIS_IMAGE_TYPE_MLZ)
        {
#ifdef FEATURE_MLZ_DECODER
           slzko_begin_pos = data_position;
           slzko_dstrm.size_out = saf_typeInfo_t[current_processing_type_id].sizeX*
           saf_typeInfo_t[current_processing_type_id].sizeY* 
           saf_header_t.ImageLevel/8;
           slzko_dstrm.buf_out = saf_typeInfo_t[current_processing_type_id].imageData;
           slzko_dstrm.state = DECODE_INIT;
           if(embedded_img_size>=SMLZ_IN_CHUNK_STEP_SIZE)
             cmx_get_data.num_bytes = SMLZ_IN_CHUNK_STEP_SIZE;
           else
             cmx_get_data.num_bytes = embedded_img_size;
           cmx_get_data.buffer = chunk_data;
           cmx_get_data.position = data_position;
           cmx_get_data.callback_ptr =
             (saf_cmx_command_cb_func_ptr_type) process_saf;
           cmx_get_data.client_data = (void *) saf_id;
           data_position += cmx_get_data.num_bytes;
           saf_state = SAF_STATE_GET_DATA;
           saf_state_q1 = SAF_STATE_PROCESS_MLZ;
#else
           if (saf_cmx_cb != NULL)
             saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
           saf_state = SAF_STATE_INIT;
           exit_saf_decoder = TRUE;
           saf_decode_in_progress = FALSE;
           saf_file_size = 0;
           saf_cmx_cb = NULL;
#endif
        }        
        else
        {
          /* if unknown compression type then abort player                 */
          if (saf_cmx_cb != NULL)
            saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
          saf_state = SAF_STATE_INIT;
          exit_saf_decoder = TRUE;
          saf_decode_in_progress = FALSE;
          saf_file_size = 0;
          saf_cmx_cb = NULL; 
        }
        break;

      case SAF_STATE_CONTINUE:
        /* exit player to allow CMX task to process queued commands        */
        if (saf_cmx_cb != NULL)
          saf_cmx_cb(SAF_CMX_CONTINUE, saf_cmx_handle, &cmx_continue_data);
        saf_state = saf_state_q1;
        exit_saf_decoder = TRUE;
        break;
#ifdef FEATURE_MLZ_DECODER
      case SAF_STATE_PROCESS_MLZ:
        slzko_dstrm.buf_in = chunk_data;
        slzko_dstrm.size_in = cmx_get_data.num_bytes;
        slzko_return_state = SMLZ_DecodeBlock(&slzko_dstrm);
        switch(slzko_return_state)
        {
          case DECODE_LZ_CONTINUE:
          slzko_dstrm.state = DECODE_CONTINUE;
          if(data_position < slzko_begin_pos+embedded_img_size)
          {
            if(data_position+SMLZ_IN_CHUNK_STEP_SIZE<slzko_begin_pos+embedded_img_size)
              cmx_get_data.num_bytes = SMLZ_IN_CHUNK_STEP_SIZE;
            else 
              cmx_get_data.num_bytes = slzko_begin_pos+embedded_img_size-data_position;
          }
          else
            slzko_return_state = -1;
            cmx_get_data.buffer = chunk_data;
            cmx_get_data.position = data_position;
            cmx_get_data.callback_ptr =
             (saf_cmx_command_cb_func_ptr_type) process_saf;
            cmx_get_data.client_data = (void *) saf_id;
            data_position += cmx_get_data.num_bytes;
            saf_state = SAF_STATE_GET_DATA;
            saf_state_q1 = SAF_STATE_PROCESS_MLZ;
            break;
          case DECODE_LZ_FINISH:
            saf_func_embedded_img_done();
            break;
          default:
            break;
         }
         if(slzko_return_state<0)
         {
            if (saf_cmx_cb != NULL)
              saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
            saf_state = SAF_STATE_INIT;
            exit_saf_decoder = TRUE;
            saf_decode_in_progress = FALSE;
            saf_file_size = 0;
            saf_cmx_cb = NULL; 
         }
         break;
#endif
#ifdef FEATURE_RLA_DECODER
#error code not present
#endif
      case SAF_STATE_PROCESS_PNG:
       /* callback to PNG decoder to continue PNG decoding operation.      */
#ifdef FEATURE_PNG_DECODER
        png_saf_cb(client_data);
        if (saf_state != SAF_STATE_PROCESS_PNG_DONE)
        {
          exit_saf_decoder = TRUE;
          break;
        }
#endif

      case SAF_STATE_PROCESS_PNG_DONE:
        /* if decoding of PNG image is complete then get next command      */
        saf_func_embedded_img_done();
        break;

      case SAF_STATE_PROCESS_FRAME_INFO:
        saf_state = SAF_STATE_GET_NEXT_COMMAND;
        break;

      case SAF_STATE_END :

        /* if here, then SAF file decoding was successful and 
        * let calling routine know we are done 
        */
        cmx_done_data.callback_ptr =
        (saf_cmx_frame_cb_func_ptr_type) process_one_frame;
        cmx_done_data.client_data = (void*) saf_id;
        saf_state = SAF_STATE_INIT;
        exit_saf_decoder = TRUE;
        saf_decode_in_progress = FALSE;
        saf_file_size = 0;
        if (saf_cmx_cb != NULL)
        {
          saf_cmx_cb(SAF_CMX_PREPARE_DONE, saf_cmx_handle, 
                     (void*) &cmx_done_data);
        }
        break;

      case SAF_STATE_PREPARE_ONE_FRAME:
        /* prepare to generate animation frame.   Initialize 
        ** frame buffer.
        */ 
        if ( sis_dec_prepare_one_frame(chunk_data, user_frame_buf) < 0 )
        {
          if (saf_cmx_cb != NULL)
            saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
          saf_state = SAF_STATE_INIT;
          exit_saf_decoder = TRUE;
          saf_decode_in_progress = FALSE;
          saf_file_size = 0;
          saf_cmx_cb = NULL;
          break;
        }
        saf_state = SAF_STATE_MAKE_ONE_FRAME;
        break;

      case SAF_STATE_MAKE_ONE_FRAME:
        /* generate animation frame                                        */
        if ((ret_from_frame = sis_dec_make_one_frame()) < 0)
        {
          if (saf_cmx_cb != NULL)
          {
            saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
          }
          saf_state = SAF_STATE_INIT;
          exit_saf_decoder = TRUE;
          saf_decode_in_progress = FALSE;
          saf_file_size = 0;
          saf_cmx_cb = NULL;
          break;
        }
        else if (ret_from_frame == 0)
        {
          cmx_frame_done_data.framebuf = user_frame_buf;
          cmx_frame_done_data.size_frame = (sis_display_info_data.width
                                            *sis_display_info_data.height*saf_header_t.ImageLevel)/8;
          
          cmx_frame_done_data.delay_to_nextframe =
          cmx_saf_info.delay_between_frames[user_frame_id];

          if (user_frame_id == cmx_saf_info.number_of_frames-1)
            cmx_frame_done_data.flag_last_frame = 1;
          else
            cmx_frame_done_data.flag_last_frame = 0;

          if ( saf_cmx_cb != NULL )
          {
            saf_cmx_cb(SAF_CMX_FRAME_DONE, saf_cmx_handle, 
                       (void*) &cmx_frame_done_data);
          }
          exit_saf_decoder = TRUE;
        }
        else
        {
          saf_state = SAF_STATE_MAKE_ONE_FRAME;
          cmx_continue_data.callback_ptr =
          (saf_cmx_command_cb_func_ptr_type) process_saf;
          cmx_continue_data.client_data = (void*)saf_id;

          if (saf_cmx_cb != NULL)
            saf_cmx_cb(SAF_CMX_CONTINUE, saf_cmx_handle, 
                       (void*) &cmx_continue_data);
          exit_saf_decoder = TRUE;
        }
        break;

      default :
        /* hopefully should never reach here */
        if (saf_cmx_cb != NULL)
        {
          saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
        }
        saf_state = SAF_STATE_INIT;
        exit_saf_decoder = TRUE;
        saf_decode_in_progress = FALSE;
        saf_file_size = 0;
        saf_cmx_cb = NULL;
        break;
    }
  } while ( !exit_saf_decoder );
}

/* <EJECT> */
/*===========================================================================
FUNCTION  sis_set_4byte_aligned_ptr

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
uint8* sis_set_4byte_aligned_ptr(uint8* ptr)
{
  uint32 addr = (uint32)ptr;
  uint32 tmp = addr & 0x03;
  if (tmp)
    addr = addr + (4-tmp);
  return (uint8*) addr;
}

/*===========================================================================

FUNCTION  process_one_frame

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void process_one_frame
(
  uint32 frameid, 
  uint8 *framebuf, 
  void *cb_data
)
{
  int32 fail_flag = 0;

  /* if client ids don't match, then exit from decoder                     */
  if (cb_data != saf_id)
    return;

  /* if frame ID exceeds the max number of frames in animation data then
  ** exit decoder
  */
  if (frameid >= cmx_saf_info.number_of_frames)
    fail_flag = 1;

  /* if 1-bit then assign buffer for temporary usage for edge detection    */
  if (saf_header_t.ImageLevel == 1)
  {
    if ((sis_size_GlobalObjectBuffer - sis_pos_GlobalObjectBuffer) <
        (sis_display_info_data.width * sis_display_info_data.height) / 4 + 8)
    {
      fail_flag = 1;
    }
    else
    {
      sis_display_info_data.pOperationBuf1 =
      sis_set_4byte_aligned_ptr(&(sis_GlobalObjectBuffer[sis_pos_GlobalObjectBuffer]));

      sis_display_info_data.pOperationBuf2 =
      sis_set_4byte_aligned_ptr(&(sis_GlobalObjectBuffer[sis_pos_GlobalObjectBuffer
                                                  + sis_display_info_data.width
                                                  * sis_display_info_data.height / 8]));
    }
  } /* if 2-bit then assign buffer for temporary usage for edge detection  */
  else if ( saf_header_t.ImageLevel==2 )
  {
    if ((sis_size_GlobalObjectBuffer-sis_pos_GlobalObjectBuffer) <
                (sis_display_info_data.width*sis_display_info_data.height) / 2 + 8)
    {
      fail_flag = 1;
    }
    else
    {
      sis_display_info_data.pOperationBuf1 =
      sis_set_4byte_aligned_ptr(&(sis_GlobalObjectBuffer[sis_pos_GlobalObjectBuffer]));
      sis_display_info_data.pOperationBuf2 =
      sis_set_4byte_aligned_ptr(&(sis_GlobalObjectBuffer[sis_pos_GlobalObjectBuffer
                                                  + sis_display_info_data.width
                                                  * sis_display_info_data.height / 4]));
    }
  }
  if (fail_flag != 0)
  {
    if (saf_cmx_cb != NULL)
      saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
    saf_state = SAF_STATE_INIT;
    saf_decode_in_progress = FALSE;
    saf_file_size = 0;
    saf_cmx_cb = NULL;
    return;
  }
  user_frame_id = frameid;
  user_frame_buf = framebuf;
  cmx_get_data.buffer = chunk_data;
  cmx_get_data.num_bytes = saf_func_get_frame_size(frameid);
  cmx_get_data.position = saf_func_get_frame_pos(frameid);
  cmx_get_data.callback_ptr = (saf_cmx_command_cb_func_ptr_type) process_saf;
  cmx_get_data.client_data = (void *) saf_id;
  data_position += saf_func_get_frame_size(frameid);
  saf_state = SAF_STATE_PREPARE_ONE_FRAME;
  if (saf_cmx_cb != NULL)
    saf_cmx_cb(SAF_CMX_GET_DATA, saf_cmx_handle, &cmx_get_data);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_decode

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
extern void saf_decode
(
  void *handle, 
  saf_cmx_decode_cb_func_ptr_type callback_ptr
)
{
  if (saf_cmx_cb != NULL)
    saf_cmx_cb(SAF_CMX_ABORT, saf_cmx_handle, 0);

  saf_id++;                                    /* client id for this run   */
  saf_decode_in_progress = TRUE;               /* set decode in progress   */
  saf_file_size = 0;
  saf_decoder_abort_flag = FALSE;              /* reset abort flag         */
  saf_image_info_flag = FALSE;                 /* reset specs flag         */
  saf_cmx_handle = handle;
  saf_cmx_cb = callback_ptr;                   /* callers callback ptr     */
  saf_state = SAF_STATE_INIT;                  /* set saf to init state    */
  process_saf((void *) saf_id);                /* call saf decoder         */
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_get_info

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
extern void saf_get_info
(
void *handle, 
saf_cmx_decode_cb_func_ptr_type callback_ptr
)
{
  if (saf_cmx_cb != NULL)
    saf_cmx_cb(SAF_CMX_ABORT, saf_cmx_handle, 0);

  saf_id++;
  saf_decode_in_progress = TRUE;
  saf_file_size = 0;
  saf_cmx_handle = handle;
  saf_cmx_cb = callback_ptr;
  saf_decoder_abort_flag = FALSE;
  saf_image_info_flag = TRUE;
  saf_state = SAF_STATE_INIT;
  process_saf((void *)saf_id);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_decode_abort

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
extern void saf_decode_abort(void)
{
  saf_decoder_abort_flag = TRUE;
  if(saf_decode_in_progress == TRUE)
  {
    png_decode_abort();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_decode_stop

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void saf_decode_stop(void)
{
  saf_state = SAF_STATE_INIT;
  saf_decoder_abort_flag = FALSE;
  saf_cmx_cb = NULL;
  saf_decode_in_progress = FALSE;
  saf_file_size = 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_png_cb

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_PNG_DECODER
void saf_png_cb
(
  png_cmx_status_enum_type png_status, 
  void *handle, 
  void *server_data
)
{
  png_cmx_get_data_type *png_get_data;
  png_cmx_data_type *png_data;
  png_cmx_spec_type *png_spec;
  png_cmx_callback_type *png_cb;

  sis_png_state = png_status;

  switch (png_status)
  {
    case PNG_CMX_TEXT:
      break;

    case PNG_CMX_SPEC:
      png_spec = (png_cmx_spec_type*)server_data;
      sis_png_w = png_spec->width;
      sis_png_h = png_spec->height;
      sis_png_bit_depth=png_spec->bit_depth;
      break;

    case PNG_CMX_GET_FILE_SIZE:
      if (server_data != NULL)
      {
        *(uint32 *) server_data = sis_png_size;
      }
      break;

    case PNG_CMX_DATA:
      png_data = (png_cmx_data_type*)server_data;
      if (strncmp((const char*)png_data->buffer,"tran",4) ==0
          ||strncmp((const char*)png_data->buffer,"bkgd",4) ==0 )
        sis_png_outcnt--;

      if (strncmp((const char*)png_data->buffer,"plte",4) ==0)
        sis_png_outcnt -= 2;

      if (sis_png_outcnt>0)
      {
        memcpy(&(sis_png_outbuf[sis_png_outpos]),png_data->buffer,png_data->num_bytes);
        sis_png_outpos += png_data->num_bytes;
      }
      sis_png_outcnt++;
      break;

    case PNG_CMX_GET_DATA:

      png_get_data = (png_cmx_get_data_type*)server_data;
      png_saf_cb = (png_cmx_command_cb_func_type)png_get_data->callback_ptr;
      client_data = png_get_data->client_data;

      if (png_get_data->position<sis_png_size)
      {
        if ((png_get_data->position+png_get_data->num_bytes)<=sis_png_size)
          cmx_get_data.num_bytes=png_get_data->num_bytes;
        else
          cmx_get_data.num_bytes=sis_png_size-png_get_data->position;
      }
      else
      {
        if (saf_cmx_cb != NULL)
        {
          saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
        }
        saf_state = SAF_STATE_INIT;
        if(saf_decode_in_progress == TRUE)
        {
          png_decode_abort();
        }
        saf_decode_in_progress = FALSE;
        saf_file_size = 0;
        saf_cmx_cb = NULL;
        break;
      }

      cmx_get_data.buffer = png_get_data->buffer;
      cmx_get_data.position = sis_png_start_pt+png_get_data->position;

      if (png_saf_cb != NULL)
      {
        cmx_get_data.callback_ptr = (saf_cmx_command_cb_func_ptr_type) process_saf;
      }
      else
      {
        cmx_get_data.callback_ptr = NULL;
      }

      cmx_get_data.client_data = (void*) saf_id;
      data_position = cmx_get_data.position+cmx_get_data.num_bytes;

      saf_state = SAF_STATE_PROCESS_PNG;    
      if (saf_cmx_cb != NULL)
      {
        saf_cmx_cb(SAF_CMX_GET_DATA, saf_cmx_handle, &cmx_get_data);
      }

      break;
    case PNG_CMX_CONTINUE:

      png_cb = (png_cmx_callback_type*)server_data;
      png_saf_cb = (png_cmx_command_cb_func_type)png_cb->callback_ptr;
      client_data = png_cb->client_data;

      cmx_continue_data.callback_ptr = (saf_cmx_command_cb_func_ptr_type) process_saf;
      cmx_continue_data.client_data =(void*) saf_id;

      if (saf_cmx_cb != NULL)
      {
        saf_cmx_cb(SAF_CMX_CONTINUE, saf_cmx_handle, &cmx_continue_data);
      }
      saf_state = SAF_STATE_PROCESS_PNG;
      break;

    case PNG_CMX_ABORT:
      break;

    case PNG_CMX_DONE:
      saf_state = SAF_STATE_PROCESS_PNG_DONE;
      break;

    case PNG_CMX_FAILURE:

      if (saf_cmx_cb != NULL)
      {
        saf_cmx_cb(SAF_CMX_FAIL, saf_cmx_handle, 0);
      }
      saf_state = SAF_STATE_INIT;
      saf_decode_in_progress = FALSE;
      saf_file_size = 0;
      saf_cmx_cb = NULL;
      break;

    default :
      break;
  }
  return;
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_png_init_var

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_PNG_DECODER
void sis_png_init_var(void)
{
  sis_png_w = 0;
  sis_png_h = 0;
  sis_png_bit_depth = 0;
  sis_png_outpos = 0;
  sis_png_state = 0;
  sis_png_outcnt = 0;
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_preheader

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_preheader
(
  uint8 *inbuf, 
  int32 *ver, 
  int32 *size
)
{
  if (inbuf[0] !='S' || inbuf[1] != 'A' || inbuf[2] != 'F' || inbuf[3] !=0x00)
    return (-1);
  else
  {
    *ver = inbuf[4];
    if (*ver != SAF_INIT_VERSION && *ver != SAF_MLZ_VERSION)
      return (-1);
    *size = 0;
    if( saf_file_size == 0){
      *size = (((int32) inbuf[5]) << 8) | inbuf[6];
    }else{
      *size = saf_file_size;
    }

  }
  return (SIS_PREHEADER_LENGTH);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_func_check_total_header

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_func_check_total_header
(
  uint8 *inbuf
)
{
  int32 nextsize;

  nextsize = inbuf[0];
  nextsize = (nextsize << 8) | inbuf[1];
  return nextsize;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_process_header_group

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_process_header_group
(
  uint8 *inbuf, 
  int32 total_hdrsize
)
{
  uint8 cmd;
  int32 pos = 0;

  while (pos < total_hdrsize)
  {
    cmd = inbuf[pos];
    switch (cmd)
    {
      /* process global header data command including frame size and       */
      /* bit depth                                                         */
      case SIS_GLOBAL_HEADER_CMD:
        pos += sis_dec_g_header(&inbuf[pos], &saf_header_t);
        sis_display_info_data.width = saf_header_t.sizeX;
        sis_display_info_data.height = saf_header_t.sizeY;
        break;
      /* extract global image data including global palette                */
      case SIS_GLOBAL_IMAGE_CMD:
        pos += sis_dec_g_image_cmd(&inbuf[pos], &saf_header_t);
        break;
      /* extract global frame information including number of frames       */
      case SIS_GLOBAL_FRAME_INFO_CMD:
        pos += sis_dec_g_frame_info_cmd(&inbuf[pos], &saf_header_t);
        break;
      /* process global transparency information for animation frames      */
      case SIS_GLOBAL_TRANSPARENCY_CMD:
        pos += sis_dec_g_tr_cmd(&inbuf[pos], &saf_header_t);
        break;
      /* process interframe delay information                              */
      case SIS_GLOBAL_DELAY_INFO_CMD:
        pos += sis_dec_g_delay_info_cmd(&inbuf[pos], &saf_header_t);
        break;
      /* ignore unknown subcommands                                        */
      default: 
        pos += (inbuf[pos+1] + 2);
        break;
    }
  }
  /* if bit depth is larger than display bit depth abort player            */
  if (saf_header_t.ImageLevel > sis_display_info_data.bitdepth)
    return (-1);
  else
    return (pos);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_dec_get_normal_type_header

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_dec_get_normal_type_header
(
  uint8 *inbuf,
  saf_typeInfo *typeArray
)
{
  int32 id, pos;
  
  id = inbuf[0] & 0x7f;                         /* object ID               */
  /* if ID number exceeds MAX_TYPES then abort                             */
  if (id >= MAX_TYPES)
    return (-1);

  /* set global ID for processing objects                                  */
  current_processing_type_id = id;
  typeArray[id].DecompressOrNot = (inbuf[0] & 0x80) >> 7;
  typeArray[id].typeID = id;

  /* save width of object                                                  */
  typeArray[id].sizeX = inbuf[1];
  if ((int32) typeArray[id].sizeX <= 0)
    return (-1);

  /* save height of object                                                 */
  typeArray[id].sizeY=inbuf[2];
  if ((int32) typeArray[id].sizeY <= 0)
    return (-1);

  /* compression method                                                    */
  typeArray[id].imageType = inbuf[3];

  /* object information such as transparency                               */
  pos = 4;
  typeArray[id].TransparencyColor = inbuf[pos++];
  typeArray[id].stored = 0;

  if ((sis_pos_GlobalObjectBuffer + typeArray[id].sizeX * typeArray[id].sizeY 
       * saf_header_t.ImageLevel / 8) > sis_size_GlobalObjectBuffer)
    return (-1);

  return (typeArray[id].imageDataSize);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_func_get_global_info

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_func_get_global_info
(
  saf_cmx_global_info_type *info
)
{
  int32 i, index = 0;
  info->bitdepth = (saf_bitdepth_type) saf_header_t.ImageLevel;

  if (saf_header_t.UseGlobalTransparency != 0)
  {
    info->global_transparency_count = saf_header_t.global_transparency_count;
    if(saf_info_buf_ptr != NULL) {
      index = sizeof(saf_header_t.global_transparency);
      memcpy(saf_info_buf_ptr, saf_header_t.global_transparency, index);
      info->global_transparency = saf_info_buf_ptr;
    } else {
      info->global_transparency = saf_header_t.global_transparency;
    }
  }
  else
    info->global_transparency_count = 0;

  info->width = saf_header_t.sizeX;
  info->height = saf_header_t.sizeY;

  info->loopcount = saf_header_t.LoopCount;
  info->number_of_frames = saf_header_t.numberOfFrames;

  if (saf_header_t.variableDelayorNot == 0)
  {
    for (i = 0; i < saf_header_t.numberOfFrames; i++)
      info->delay_between_frames[i] = saf_header_t.delay[0];
  }
  else
  {
    for (i = 0; i < saf_header_t.numberOfFrames; i++)
      info->delay_between_frames[i] = saf_header_t.delay[i];
  }

  if (saf_header_t.paletteType == 0 || saf_header_t.paletteType == 2)
  {
    info->number_palette_entry = 0;
  }
  else if (saf_header_t.paletteType == 1)
  {
    info->number_palette_entry = 256;
    if(saf_info_buf_ptr != NULL) {
      memcpy(&saf_info_buf_ptr[index], sis_GlobalPaletteTable,
             MAX_SIS_GLOBALPALETTETABLE);
      info->palette = &saf_info_buf_ptr[index];
    } else {
      info->palette = sis_GlobalPaletteTable;
    }
  }
  else
  {
    saf_info_buf_ptr = NULL;
    return (-1);
  }

  saf_info_buf_ptr = NULL;
  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_func_save_frame_pos_and_size

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_func_save_frame_pos_and_size
(
  int32 pos, 
  int32 size
)
{
  if (current_frame_id >= MAX_FRAMES)
    return (-1);

  frame_pos_size[current_frame_id].pos = pos;
  frame_pos_size[current_frame_id].size = size;
  current_frame_id++;

  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_func_get_frame_size

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_func_get_frame_size
(
  int32 frameid
)
{
  return frame_pos_size[frameid].size;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_func_get_frame_pos

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_func_get_frame_pos
(
  int32 frameid
)
{
  return frame_pos_size[frameid].pos;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_func_embedded_img_init

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void saf_func_embedded_img_init(void)
{
  saf_typeInfo_t[current_processing_type_id].imageData =
  &sis_GlobalObjectBuffer[sis_pos_GlobalObjectBuffer];
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_func_embedded_img_done

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void saf_func_embedded_img_done(void)
{
  saf_typeInfo_t[current_processing_type_id].imageDataSize = 
    (((saf_typeInfo_t[current_processing_type_id].sizeX) * (saf_header_t.ImageLevel) ) / 8) 
    * (saf_typeInfo_t[current_processing_type_id].sizeY);
  sis_pos_GlobalObjectBuffer += saf_typeInfo_t[current_processing_type_id].imageDataSize;
  saf_state = SAF_STATE_GET_NEXT_COMMAND;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  notyskmemchr_up

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void *notyskmemchr_up
(
  const void * src_void, 
  uint32 c, 
  int32 length
)
{
  const uint8 *src = (const uint8 *) src_void;

  while (--length >= 0)
  {
    if (*src != c)
      return ((void *)src);
    src++;
  }
  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  notyskmemchr_down

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void *notyskmemchr_down
(
  const void * src_void, 
  uint32 c, 
  int32 length
)
{
  const uint8 *src = (const uint8 *) src_void;

  while (--length >= 0)
  {
    if (*src != c)
      return (void *)src;
    src--;
  }
  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  EdgeDetection_1bpp

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL uint8 i1[256], i2[256];
LOCAL uint8 j1[256], j2[256];
LOCAL uint8 iix[32]; /*  32 = (256 pixel / 8)  */

void EdgeDetection_1bpp_mask_draw_setting_along_x(uint8 *outbuf,
                            int32 xWidthOfImage,
                            int32 Y_start, int32 Y_end
                            )
{
  int32 i, ii, nY;

  i = (Y_start - 1) * xWidthOfImage;
 
  for(nY = Y_start; nY <= Y_end; nY++)
  {
    i += xWidthOfImage;

    if(iix[nY >> 3] & (0x80 >> (nY & 7)))
    {
      if((i2[nY] >> 3) == (i1[nY] >> 3))
        outbuf[(i1[nY] >> 3) + i] = (0xFF >> (i1[nY] & 7)) & (0xFF << (7 - (i2[nY] & 7)));
      else
      {
        for (ii = (i1[nY] >> 3) + 1; ii <= (i2[nY] >> 3) - 1; ii++)
          outbuf[ii + i] = 255;

        ii = i1[nY] & 7;
        outbuf[(i1[nY] >> 3) + i] = 0xFF >> ii;
        
        ii = i2[nY] & 7;
        outbuf[(i2[nY] >> 3) + i] = (uint8) (0xFF << (7 - ii));
      }
    }
  }
  return;
}

void EdgeDetection_1bpp_mask_draw_resetting_along_y(uint8 *outbuf,
                            srect *coord_frame, 
                            int32 xWidthOfImage,
                            int32 X_start, int32 X_end
                            )
{
  int32 nX, nY;

  for(nX = X_start; nX <= X_end; nX++)
  {
    if(j1[nX] <= j2[nX])
    {
      for (nY = coord_frame->top; nY < j1[nX]; nY++)
        RESET_1BIT(outbuf, nX, nY, xWidthOfImage);
    
      for (nY = j2[nX] + 1; nY <= coord_frame->bottom; nY++)
        RESET_1BIT(outbuf, nX, nY, xWidthOfImage);
    }
    else if(j1[nX] == coord_frame->bottom && j2[nX] == coord_frame->top)
    {
      for (nY = coord_frame->top; nY <= coord_frame->bottom; nY++)
        RESET_1BIT(outbuf, nX, nY, xWidthOfImage);
    }
  }
  return;
}

void EdgeDetection_1bpp
(
 uint8 *inbuf, 
 int32 yHeightOfImage, 
 int32 xWidthOfImage,
 srect *coord_frame, 
 srect *realRgn, 
 uint8 *outbuf,
 saf_typeMotionInfo *mv,
 saf_typeInfo *type
)
{
  int32 nX, nY, temp1, temp2;
  int32 X_start, X_end;
  int32 Y_start, Y_end;
  int32 pre_temp1, pre_temp2;
  int32 i, ii=0, iii, h1, h2;

  memset(i1, 0 , 256);
  memset(i2, 0 , 256);
  memset(j1, 0 , 256);
  memset(j2, 0 , 256);
  memset(iix, 0 ,32);

  xWidthOfImage >>= 3;

  /* mask buffer resetting along X-direction */
  pre_temp1 = -999;
  pre_temp2 = -999;
  
  if((coord_frame->top == 0 && realRgn->top < 0) || mv->ClipFlag != 0)
  {
    Y_start = coord_frame->top;

    for(i = 0; i < xWidthOfImage; i++)
    {
      iii = outbuf[coord_frame->top * xWidthOfImage + i];
      if(iii != 0)
      {
        for(ii = 0; ii < 8; ii++)
        {
          if((0x80 >> ii) & iii)
            break;
        }
        pre_temp1 = (i << 3) + ii;
        break;
      }
    }

    for(i = xWidthOfImage - 1; i >= 0; i--)
    {
      iii = outbuf[coord_frame->top * xWidthOfImage + i];
      if(iii != 0)
      {
        for(ii = 0; ii < 8; ii++)
        {
          if((0x01 << ii) & iii)
            break;
        }
        pre_temp2 = (i << 3) + 7 - ii;
        break;
      }
    }
  }
  else
  {
    Y_start = coord_frame->top;
  }

  Y_end = -999;
  
  for(i = 0; i < xWidthOfImage; i++)
  {
    if(outbuf[coord_frame->bottom * xWidthOfImage + i] != 0)
    {
      Y_end = coord_frame->bottom;
      break;
    }
  }

  if(Y_end == -999)
  {
    for(i = (coord_frame->bottom); i >= (coord_frame->top); i--)
    {
      for(ii = (coord_frame->left >> 3); ii <= (coord_frame->right >> 3); ii++)
      {
        if(inbuf[ii + i * xWidthOfImage] != 0)
        {
          Y_end = i;
          break;
        }
      }
      if(Y_end != -999)
        break;
    }
  }

  if(Y_end == -999)
    return;

  i = (Y_start - 1) * xWidthOfImage;

  for (nY = Y_start; nY <= Y_end; nY++)
  {
    i += xWidthOfImage;

    temp1 = -1;
    temp2 = -1;

    h1 = -999;
    h2 = -999;

    if (CHECK_1BIT(outbuf, coord_frame->left, nY, xWidthOfImage) != 0)
      temp1 = coord_frame->left;
    else
    {
      for(ii = (coord_frame->left >> 3) ; ii <= (coord_frame->right >> 3); ii++)
      {
        if(inbuf[ii + i] != 0)
        {
          for(iii = 0; iii < 8; iii++)
          {
            if((0x80 >> iii) & inbuf[ii + i])
              break;
          }
          temp1 = (ii << 3) + iii;
          break;
        }
      }
    }
    
    if (CHECK_1BIT(outbuf, coord_frame->right, nY, xWidthOfImage) != 0)
      temp2 = coord_frame->right;
    else if(temp1 == -1)
      temp2 = -1;
    else
    {
      for(ii = (coord_frame->right >> 3) ; ii >= (coord_frame->left >> 3); ii--)
      {
        if(inbuf[ii + i] != 0)
        {
          for(iii = 0; iii < 8; iii++)
          {
            if((0x01 << iii) & inbuf[ii + i])
              break;
          }
          temp2 = (ii << 3) + 7 - iii;
          break;
        }
      }
    }
    
    if(temp1 != -1 && pre_temp1 != -999 && temp1 - pre_temp1 > 5)
    {
      if(find_original_point_is_internal_1bit(mv, type, temp1 - 3, nY) != 0
        || find_original_point_is_internal_1bit(mv, type, pre_temp1 + 5, nY) != 0)
        temp1 = pre_temp1;
    }
    
    if(temp2 != -1 && pre_temp2 != -999 && pre_temp2 - temp2 > 5)
    {
      if(find_original_point_is_internal_1bit(mv, type, temp2 + 3, nY) != 0
        || find_original_point_is_internal_1bit(mv, type, pre_temp2 - 5, nY) != 0)
        temp2 = pre_temp2;
    }
    
    if(temp1 != -1)
      pre_temp1 = temp1;
    
    if(temp2 != -1)
      pre_temp2 = temp2;
    
    if(temp1 == -1 && temp2 != -1)
    {
      h1 = pre_temp1;
      h2 = coord_frame->right;
    }
    else if(temp1 != -1 && temp2 == -1)
    {
      h1 = coord_frame->left;
      h2 = pre_temp2;
    }
    else if(temp1 != -1 && temp2 != -1)
    {
      h1 = temp1;
      h2 = temp2;
    }
    else
    {
      if(pre_temp1 != -999 && pre_temp2 != -999)
      {
        for(ii = (pre_temp1 + 1); ii < pre_temp2; ii++)
        {
          if(CHECK_1BIT(inbuf, ii, (nY - 1), xWidthOfImage) == 0)
          {
            h1 = pre_temp1;
            h2 = pre_temp2;
            break;
          }
        }
      }
      if(ii == pre_temp2)
      {
        pre_temp1 = -999;
        pre_temp2 = -999;
      }
    }

    if(h1 != -999 && h2 != -999)
    {
      i1[nY] = h1;
      i2[nY] = h2;
      iix[nY >> 3] |= (0x80 >> (nY & 7));
    }
  }
  /* mask buffer setting along Y-direction */
  pre_temp1 = -999;
  pre_temp2 = -999;
  
  if((coord_frame->left == 0 && realRgn->left < 0) || mv->ClipFlag != 0)
  {
    X_start = coord_frame->left;

    for(i = 0; i < yHeightOfImage; i++)
    {
      if(CHECK_1BIT(outbuf, coord_frame->left, i, xWidthOfImage) != 0)
      {
        pre_temp1 = i;
        break;
      }
    }
    
    for(i = yHeightOfImage - 1; i >= 0; i--)
    {
      if(CHECK_1BIT(outbuf, coord_frame->left, i, xWidthOfImage) != 0)
      {
        pre_temp2 = i;
        break;
      }
    }
  }
  else
  {
    X_start = coord_frame->left;
  }

  X_end = -999;

  for(ii = 0; ii < yHeightOfImage; ii++)
  {
    if(CHECK_1BIT(outbuf, coord_frame->right, ii, xWidthOfImage) != 0)
    {
      X_end =coord_frame->right;
      break;
    }
  }

  if(X_end == -999)
  {
    for(ii = (coord_frame->right); ii >= (coord_frame->left); ii--)
    {
      for(iii = coord_frame->top; iii <= coord_frame->bottom; iii++)
      {
        if(CHECK_1BIT(inbuf, ii, iii, xWidthOfImage) != 0)
        {
          X_end = ii;
          break;
        }
      }
      if(X_end != -999)
        break;
    }
  }

  for (nX = X_start; nX <= X_end; nX++)
  {
    temp1 = -1;
    temp2 = -1;

    h1 = -999;
    h2 = -999;
    
    if (CHECK_1BIT(outbuf, nX, coord_frame->top, xWidthOfImage) != 0)
        temp1 = coord_frame->top;
    else
    {
      for (nY = coord_frame->top; nY <= coord_frame->bottom; nY++)
      {
        if (CHECK_1BIT(inbuf, nX, nY, xWidthOfImage))
        {
          temp1 = nY;
          break;
        }
      }
    }
    
    if (CHECK_1BIT(outbuf, nX, coord_frame->bottom, xWidthOfImage) != 0)
      temp2 = coord_frame->bottom;
    else if(temp1 == -1)
      temp2 = -1;
    else
    {
      for (nY = coord_frame->bottom; nY >= coord_frame->top; nY--)
      {
        if (CHECK_1BIT(inbuf, nX, nY, xWidthOfImage))
        {
          temp2 = nY;
          break;
        }
      }
    }
    
    if(temp1 != -1 && pre_temp1 != -999 && temp1 - pre_temp1 > 5)
    {
      if(find_original_point_is_internal_1bit(mv, type, nX, temp1 - 3)
        || find_original_point_is_internal_1bit(mv, type, nX, pre_temp1 + 5))
        temp1 = pre_temp1;
    }
    
    if(temp2 != -1 && pre_temp2 != -999 && pre_temp2 - temp2 > 5)
    {
      if(find_original_point_is_internal_1bit(mv, type, nX, temp2 + 3)
        || find_original_point_is_internal_1bit(mv, type, nX, pre_temp2 - 5))
        temp2 = pre_temp2;
    }

    if(temp1 != -1)
      pre_temp1 = temp1;
    
    if(temp2 != -1)
      pre_temp2 = temp2;
    
    if(temp1 == -1 && temp2 != -1)
    {
      h1 = pre_temp1;
      h2 = coord_frame->bottom;
    }
    else if(temp1 != -1 && temp2 == -1)
    {
      h1 = coord_frame->top;
      h2 = pre_temp2;
    }
    else if(temp1 != -1 && temp2 != -1)
    {
      h1 = temp1;
      h2 = temp2;
    }
    else if(temp1 == -1 && temp2 == -1)
    {
      if(pre_temp1 != -999 && pre_temp2 != -999)
      {
        for(i = (pre_temp1 + 1); i <= (pre_temp2 - 1); i++)
        {
          if(CHECK_1BIT(inbuf, (nX - 1), i, xWidthOfImage) == 0)
          {
            h1 = pre_temp1;
            h2 = pre_temp2;
            break;
          }
        }
      }

      if(i == pre_temp2)
      {
        pre_temp1 = -999;
        pre_temp2 = -999;
      }

      if(h1 == -999 && h2 == -999)
      {
        h1 = coord_frame->bottom;
        h2 = coord_frame->top;
      }
    }

    j1[nX] = h1;
    j2[nX] = h2;
  }
  /* Mask draw */
  EdgeDetection_1bpp_mask_draw_setting_along_x(outbuf, xWidthOfImage, Y_start, Y_end);
  EdgeDetection_1bpp_mask_draw_resetting_along_y(outbuf, coord_frame, xWidthOfImage, X_start, X_end);

  return;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  EdgeDetection_2bpp

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void EdgeDetection_2bpp_mask_draw_setting_along_x(uint8 *outbuf,
                            int32 xWidthOfImage,
                            int32 Y_start, int32 Y_end
                            )
{
  int32 i, ii, nY;

  i = (Y_start - 1) * xWidthOfImage;
 
  for(nY = Y_start; nY <= Y_end; nY++)
  {
    i += xWidthOfImage;

    if(iix[nY >> 3] & (0x80 >> (nY & 7)))
    {
      if((i2[nY] >> 2) == (i1[nY] >> 2))
        outbuf[(i1[nY] >> 2) + i] = (0xFF >> ((i1[nY] & 3) << 1)) & (0xFF << ((3 - (i2[nY] & 3)) << 1));
      else
      {
        for (ii = (i1[nY] >> 2) + 1; ii <= (i2[nY] >> 2) - 1; ii++)
          outbuf[ii + i] = 255;

        ii = i1[nY] & 3;
        outbuf[(i1[nY] >> 2) + i] = 0xFF >> (ii << 1);
        
        ii = i2[nY] & 3;
        outbuf[(i2[nY] >> 2) + i] = (uint8) (0xFF << ((3 - ii) << 1));
      }
    }
  }
  return;
}

void EdgeDetection_2bpp_mask_draw_resetting_along_y(uint8 *outbuf,
                            srect *coord_frame, 
                            int32 xWidthOfImage,
                            int32 X_start, int32 X_end
                            )
{
  int32 nX, nY;

  for(nX = X_start; nX <= X_end; nX++)
  {
    if(j1[nX] <= j2[nX])
    {
      for (nY = coord_frame->top; nY < j1[nX]; nY++)
        RESET_2BIT(outbuf, (2 * nX), nY, xWidthOfImage);
    
      for (nY = j2[nX] + 1; nY <= coord_frame->bottom; nY++)
        RESET_2BIT(outbuf, (2 * nX), nY, xWidthOfImage);
    }
    else if(j1[nX] == coord_frame->bottom && j2[nX] == coord_frame->top)
    {
      for (nY = coord_frame->top; nY <= coord_frame->bottom; nY++)
        RESET_2BIT(outbuf, (2 * nX), nY, xWidthOfImage);
    }
  }
  return;
}

void EdgeDetection_2bpp
(
 uint8 *inbuf, 
 int32 yHeightOfImage, 
 int32 xWidthOfImage,
 srect *coord_frame, 
 srect *realRgn, 
 uint8 *outbuf,
 saf_typeMotionInfo *mv,
 saf_typeInfo *type
)
{
  int32 nX, nY, temp1, temp2;
  int32 X_start, X_end;
  int32 Y_start, Y_end;
  int32 pre_temp1, pre_temp2;
  int32 i, ii=0, iii, h1, h2;

  memset(i1, 0 , 256);
  memset(i2, 0 , 256);
  memset(j1, 0 , 256);
  memset(j2, 0 , 256);
  memset(iix, 0 ,32);

  xWidthOfImage >>= 2;
  
  /* mask buffer setting along X-direction */
  pre_temp1 = -999;
  pre_temp2 = -999;
  
  if((coord_frame->top == 0 && realRgn->top < 0) || mv->ClipFlag != 0)
  {
    Y_start = coord_frame->top;

    for(i = 0; i < xWidthOfImage; i++)
    {
      iii = outbuf[coord_frame->top * xWidthOfImage + i];
      if(iii != 0)
      {
        for(ii = 0; ii < 4; ii++)
        {
          if((0xC0 >> (ii << 1)) & iii)
            break;
        }
        pre_temp1 = (i << 2) + ii;
        break;
      }
    }
    
    for(i = xWidthOfImage - 1; i >= 0; i--)
    {
      iii = outbuf[coord_frame->top * xWidthOfImage + i];
      if(iii != 0)
      {
        for(ii = 0; ii < 4; ii++)
        {
          if((0x03 << (ii << 1)) & iii)
            break;
        }
        pre_temp2 = (i << 2) + 3 - ii;
        break;
      }
    }
  }
  else
  {
    Y_start = coord_frame->top;
  }

  Y_end = -999;

  for(i = 0; i < xWidthOfImage; i++)
  {
    if(outbuf[coord_frame->bottom * xWidthOfImage + i] != 0)
    {
      Y_end =coord_frame->bottom;
      break;
    }
  }

  if(Y_end == -999)
  {
    for(i = (coord_frame->bottom); i >= (coord_frame->top); i--)
    {
      for(ii = (coord_frame->left >> 2); ii <= (coord_frame->right >> 2); ii++)
      {
        if(inbuf[ii + i * xWidthOfImage] != 0)
        {
          Y_end = i;
          break;
        }
      }
      if(Y_end != -999)
        break;
    }
  }

  if(Y_end == -999)
    return;

  i = (Y_start - 1) * xWidthOfImage;

  for (nY = Y_start; nY <= Y_end; nY++)
  {
    i += xWidthOfImage;

    temp1 = -1;
    temp2 = -1;

    h1 = -999;
    h2 = -999;

    if (CHECK_2BIT(outbuf, (2 * coord_frame->left), nY, xWidthOfImage) != 0)
      temp1 = coord_frame->left;
    else
    {
      for(ii = (coord_frame->left >> 2); ii <= (coord_frame->right >> 2); ii++)
      {
        if(inbuf[ii + i] != 0)
        {
          for(iii = 0; iii < 4; iii++)
          {
            if((0xC0 >> (iii << 1)) & inbuf[ii + i])
              break;
          }
          temp1 = (ii << 2) + iii;
          break;
        }
      }
    }
    
    if (CHECK_2BIT(outbuf, (2 * coord_frame->right), nY, xWidthOfImage) != 0)
      temp2 = coord_frame->right;
    else if(temp1 == -1)
      temp2 = -1;
    else
    {
      for(ii = (coord_frame->right >> 2); ii >= (coord_frame->left >> 2); ii--)
      {
        if(inbuf[ii + i] != 0)
        {
          for(iii = 0; iii < 4; iii++)
          {
            if((0x03 << (iii << 1)) & inbuf[ii + i])
              break;
          }
          temp2 = (ii << 2) + 3 - iii;
          break;
        }
      }
    }
    
    if(temp1 != -1 && pre_temp1 != -999 && temp1 - pre_temp1 > 5)
    {
      if(find_original_point_is_internal_2bit(mv, type, temp1 - 3, nY) != 0
        || find_original_point_is_internal_2bit(mv, type, pre_temp1 + 5, nY) != 0)
        temp1 = pre_temp1;
    }
    
    if(temp2 != -1 && pre_temp2 != -999 && pre_temp2 - temp2 > 5)
    {
      if(find_original_point_is_internal_2bit(mv, type, temp2 + 3, nY) != 0
        || find_original_point_is_internal_2bit(mv, type, pre_temp2 - 5, nY) != 0)
        temp2 = pre_temp2;
    }
    
    if(temp1 != -1)
      pre_temp1 = temp1;
    
    if(temp2 != -1)
      pre_temp2 = temp2;
    
    if(temp1 == -1 && temp2 != -1)
    {
      h1 = pre_temp1;
      h2 = coord_frame->right;
    }
    else if(temp1 != -1 && temp2 == -1)
    {
      h1 = coord_frame->left;
      h2 = pre_temp2;
    }
    else if(temp1 != -1 && temp2 != -1)
    {
      h1 = temp1;
      h2 = temp2;
    }
    else
    {
      if(pre_temp1 != -999 && pre_temp2 != -999)
      {
        for(ii = (pre_temp1 + 1); ii < pre_temp2; ii++)
        {
          if(CHECK_2BIT(inbuf, (ii * 2), (nY - 1), xWidthOfImage) == 0)
          {
            h1 = pre_temp1;
            h2 = pre_temp2;
            break;
          }
        }
      }
      if(ii == pre_temp2)
      {
        pre_temp1 = -999;
        pre_temp2 = -999;
      }
    }

    if(h1 != -999 && h2 != -999)
    {
      i1[nY] = h1;
      i2[nY] = h2;
      iix[nY >> 3] |= (0x80 >>(nY & 7));
    }
  }

  pre_temp1 = -999;
  pre_temp2 = -999;

  if((coord_frame->left == 0 && realRgn->left < 0) || mv->ClipFlag != 0)
  {
    X_start = coord_frame->left;

    for(i = 0; i < yHeightOfImage; i++)
    {
      if(CHECK_2BIT(outbuf, (coord_frame->left * 2), i, xWidthOfImage) != 0)
      {
        pre_temp1 = i;
        break;
      }
    }
    
    for(i = yHeightOfImage - 1; i >= 0; i--)
    {
      if(CHECK_2BIT(outbuf, (coord_frame->left * 2), i, xWidthOfImage) != 0)
      {
        pre_temp2 = i;
        break;
      }
    }
  }
  else
  {
    X_start = coord_frame->left;
  }

  X_end = -999;

  for(ii = 0; ii < yHeightOfImage; ii++)
  {
    if(CHECK_2BIT(outbuf, (coord_frame->right * 2), ii, xWidthOfImage) != 0)
    {
      X_end =coord_frame->right;
      break;
    }
  }

  if(X_end == -999)
  {
    for(ii = (coord_frame->right); ii >= (coord_frame->left); ii--)
    {
      for(iii = coord_frame->top; iii <= coord_frame->bottom; iii++)
      {
        if(CHECK_2BIT(inbuf, ii * 2, iii, xWidthOfImage) != 0)
        {
          X_end = ii;
          break;
        }
      }
      if(X_end != -999)
        break;
    }
  }

  for (nX = X_start; nX <= X_end; nX++)
  {
    temp1 = -1;
    temp2 = -1;

    h1 = -999;
    h2 = -999;
    
    if (CHECK_2BIT(outbuf, (nX * 2), coord_frame->top, xWidthOfImage) != 0)
    {
      temp1 = coord_frame->top;
    }
    else
    {
      for (nY = coord_frame->top; nY <= coord_frame->bottom; nY++)
      {
        if (CHECK_2BIT(inbuf, nX * 2, nY, xWidthOfImage))
        {
          temp1 = nY;
          break;
        }
      }
    }
    
    if (CHECK_2BIT(outbuf, (nX * 2), coord_frame->bottom, xWidthOfImage) != 0)
    {
      temp2 = coord_frame->bottom;
    }
    else if(temp1 == -1)
      temp2 = -1;
    else
    {
      for (nY = coord_frame->bottom; nY >= coord_frame->top; nY--)
      {
        if (CHECK_2BIT(inbuf, (nX * 2), nY, xWidthOfImage))
        {
          temp2 = nY;
          break;
        }
      }
    }
    
    if(temp1 != -1 && pre_temp1 != -999 && temp1 - pre_temp1 > 5)
    {
      if(find_original_point_is_internal_2bit(mv, type, nX, temp1 - 3)
        || find_original_point_is_internal_2bit(mv, type, nX, pre_temp1 + 5))
        temp1 = pre_temp1;
    }
    
    if(temp2 != -1 && pre_temp2 != -999 && pre_temp2 - temp2 > 5)
    {
      if(find_original_point_is_internal_2bit(mv, type, nX, temp2 + 3)
        || find_original_point_is_internal_2bit(mv, type, nX, pre_temp2 - 5))
        temp2 = pre_temp2;
    }

    if(temp1 != -1)
      pre_temp1 = temp1;
    
    if(temp2 != -1)
      pre_temp2 = temp2;
    
    if(temp1 == -1 && temp2 != -1)
    {
      h1 = pre_temp1;
      h2 = coord_frame->bottom;
    }
    else if(temp1 != -1 && temp2 == -1)
    {
      h1 = coord_frame->top;
      h2 = pre_temp2;
    }
    else if(temp1 != -1 && temp2 != -1)
    {
      h1 = temp1;
      h2 = temp2;
    }
    else if(temp1 == -1 && temp2 == -1)
    {
      if(pre_temp1 != -999 && pre_temp2 != -999)
      {
        for(i = (pre_temp1 + 1); i <= (pre_temp2 - 1); i++)
        {
          if(CHECK_2BIT(inbuf, ((nX - 1) * 2), i, xWidthOfImage) == 0)
          {
            h1 = pre_temp1;
            h2 = pre_temp2;
            break;
          }
        }
      }

      if(i == pre_temp2)
      {
        pre_temp1 = -999;
        pre_temp2 = -999;
      }

      if(h1 == -999 && h2 == -999)
      {
        h1 = coord_frame->bottom;
        h2 = coord_frame->top;
      }
    }
    j1[nX] = h1;
    j2[nX] = h2;
  }
  /* Mask draw */
  EdgeDetection_2bpp_mask_draw_setting_along_x(outbuf, xWidthOfImage, Y_start, Y_end);
  EdgeDetection_2bpp_mask_draw_resetting_along_y(outbuf, coord_frame, xWidthOfImage, X_start, X_end);

  return;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_inversion

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_inversion
(
  uint8 *buf, 
  int32 w, 
  int32 h
)
{
  int32 size;
  int32 i;
  uint32 *p;

  size = w * h;
  p = (uint32 *) buf;

  for (i = 0; i < size; i+=4)
  {
    *p = ~(*p);
    p++;
  }

  if ((i - size) == 3)
    buf[size - 1] = ~buf[size - 1];
  else if ((i - size) == 2)
  {
    buf[size - 1]=~buf[size - 1];
    buf[size - 2]=~buf[size - 2];
  }
  else if ((i - size) == 1)
  {
    buf[size - 1]=~buf[size - 1];
    buf[size - 2]=~buf[size - 2];
    buf[size - 3]=~buf[size - 3];
  }
  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_clip_rectangular_8bpp

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_clip_rectangular_8bpp
(
  uint8 *inputstream,
  int32 x_pixel_num, 
  int32 y_pixel_num,
  srect* rect,
  uint8 outer_region_color
)
{
  int32 j;
  uint8 *address;
  int32 x1,x2,y1,y2;

  x1 = rect->left;
  x2 = rect->right;
  y1 = rect->top;
  y2 = rect->bottom;

  if (x1 > x2)
  {
    j = x1;
    x1= x2;
    x2 = j;
  }

  if (y1 > y2)
  {
    j = y1;
    y1= y2;
    y2 = j;
  }

  if (x1 < 0)
    x1 =0;

  if (x1 >= x_pixel_num)
    x1 = x_pixel_num - 1;

  if (x2 < 0)
    x2 =0;

  if (x2 >= x_pixel_num)
    x2 =x_pixel_num - 1;

  if (y1 < 0)
    y1 =0;

  if (y1 >= y_pixel_num)
    y1 = y_pixel_num - 1;

  if (y2 < 0)
    y2 =0;

  if (y2 >= y_pixel_num)
    y2 =y_pixel_num - 1;

  if ((x1 == x2) || (y1 == y2))
    return(0);

  memset(inputstream, outer_region_color, x_pixel_num * y1);
  memset(inputstream + x_pixel_num * (y2 + 1), outer_region_color, 
         x_pixel_num * (y_pixel_num - (y2 + 1)));

  address = inputstream + y1 * x_pixel_num;

  for (j = y1; j <= y2; j++)
  {
    memset(address, outer_region_color, x1);
    memset(address + x2 + 1, outer_region_color, x_pixel_num - x2 - 1);
    address = address + x_pixel_num;
  }
  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_clip_rectanglar_124bpp

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_clip_rectangular_124bpp(uint8 *inputstream,
                                  int32 x_pixel_num, int32 y_pixel_num,
                                  srect* rect,
                                  uint8 outer_region_color,
                                  int32 color_bit)
{
  int32 j;
  int32 address;
  int32 x1,x2,y1,y2;
  uint8 value1, value2;

  if (color_bit == 1)
  {
    if (outer_region_color != 0)
      outer_region_color = 255;
  }
  else if (color_bit == 2)
  {
    if (outer_region_color == 0)
      outer_region_color = 0;
    else if (outer_region_color == 1)
      outer_region_color = 85;
    else if (outer_region_color == 2)
      outer_region_color = 170;
    else if (outer_region_color == 3)
      outer_region_color = 255;
  }
  else if (color_bit == 4)
  {
    outer_region_color = (outer_region_color << 4) + outer_region_color;
  }

  x1 = rect->left * color_bit;
  x2 = rect->right * color_bit + color_bit - 1;
  y1 = rect->top;
  y2 = rect->bottom;

  x_pixel_num *= color_bit;

  if (x1 > x2)
  {
    j = x1;
    x1= x2;
    x2 = j;
  }

  if (y1 > y2)
  {
    j = y1;
    y1= y2;
    y2 = j;
  }

  if (x1 < 0)
    x1 =0;

  if (x1 >= x_pixel_num)
    x1 = x_pixel_num - 1;

  if (x2 < 0)
    x2 =0;

  if (x2 >= x_pixel_num)
    x2 =x_pixel_num - 1;

  if (y1 < 0)
    y1 =0;

  if (y1 >= y_pixel_num)
    y1 = y_pixel_num - 1;

  if (y2 < 0)
    y2 = 0;

  if (y2 >= y_pixel_num)
    y2 =y_pixel_num - 1;


  if ((x1 == x2) || (y1 == y2))
    return(0);

  memset(inputstream, outer_region_color, ((x_pixel_num >> 3) * y1));
  if (y2 < (y_pixel_num - 1))
    memset(inputstream + (x_pixel_num >> 3) * (y2 + 1), outer_region_color, ((x_pixel_num >> 3) * (y_pixel_num - (y2 + 1))));


  address = y1 * (x_pixel_num >> 3);
  value2 = outer_region_color & ((255 >> (8 - (x1 & 7))) << (8 - (x1 & 7)));
  for (j = y1; j <= y2; j++)
  {
    memset(inputstream + address, outer_region_color, (x1 >> 3));
    value1 = inputstream[(address + (x1 >> 3))] & (255 >> (x1 & 7));
    inputstream[(address + (x1 >> 3))] = value1 | value2;
    address += (x_pixel_num >> 3);
  }

  if (x2 < x_pixel_num - 1)
  {
    address = y1 * (x_pixel_num >> 3);
    value2 = (outer_region_color) >> (1 + (x2 & 7));
    for (j = y1; j <= y2; j++)
    {
      memset(inputstream + address + 1 + (x2 >> 3), outer_region_color, (x_pixel_num >> 3) - ((x2 >> 3) + 1));
      value1 = inputstream[(address + (x2 >> 3))] & (~(255 >> (1 + (x2 & 7))));
      inputstream[(address + (x2 >> 3))] = value1 | value2;
      address += (x_pixel_num >> 3);
    }
  }
  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_edge_detection_prepare_1bit

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void saf_edge_detection_prepare_1bit(uint8 *inbuf, uint8 *outbuf,
                                     int32 frame_width, int32 frame_height,
                                     saf_typeMotionInfo *motion,
                                     saf_typeInfo *type,
                                     int32 ImageLevel, srect *validRgn, srect *realRgn)
{
  int32 i=0, j=0;
  int32 value=0, temp=0;
  
  if(realRgn->left < 0 || motion->ClipFlag != 0)
  {
    value = 0x80;
    
    if(motion->ClipFlag != 0)
      i = motion->clip_left;
    else
      i = 0;
    
    temp = i;
    
    for (j = validRgn->top; j <= validRgn->bottom; j++)
    {
      if (find_original_point_is_internal_1bit(motion, type, i, j) == 1)
        outbuf[j * (frame_width >> 3) + (temp >> 3)] = (value >> (i & 7));
    }
  }
  
  if(realRgn->right >= frame_width || (motion->ClipFlag != 0 && motion->clip_right < frame_width))
  {
    value = 0x01;
    
    if(motion->ClipFlag != 0)
      i = motion->clip_right;
    else
      i = frame_width - 1;
    
    temp = i;
    
    for (j = validRgn->top; j <= validRgn->bottom; j++)
    {
      if (find_original_point_is_internal_1bit(motion, type, i, j) == 1)
        outbuf[j * (frame_width >> 3) + (temp >> 3)] = (value << (7 - (i & 7)));
    }
  }
  
  if(realRgn->top < 0 || motion->ClipFlag != 0)
  {
    if(motion->ClipFlag != 0)
      j = motion->clip_top;
    else
      j = 0;
    
    for (i = validRgn->left; i <= validRgn->right; i++)
    {
      if (find_original_point_is_internal_1bit(motion, type, i, j) == 1)
      {
        SET_1BIT(outbuf, i, j, frame_width >> 3);
      }
    }
  }
  
  if(realRgn->bottom >= frame_height || (motion->ClipFlag != 0 && motion->clip_bottom < frame_height))
  {
    if(motion->ClipFlag != 0)
      j = motion->clip_bottom;
    else
      j = frame_height - 1;
    
    for (i = validRgn->left; i <= validRgn->right; i++)
    {
      if (find_original_point_is_internal_1bit(motion, type, i, j) == 1)
      {
        SET_1BIT(outbuf, i, j, frame_width >> 3);
      }
    }
    return;
  }
}

/*===========================================================================

FUNCTION  saf_edge_detection_prepare_2bit

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void saf_edge_detection_prepare_2bit(uint8 *inbuf, uint8 *outbuf,
                                int32 frame_width, int32 frame_height,
                                saf_typeMotionInfo *motion,
                                saf_typeInfo *type,
                                int32 ImageLevel, srect *validRgn, srect *realRgn)
{
  int32 i=0, j=0;
  int32 value=0, temp=0;

  if(realRgn->left < 0 || motion->ClipFlag != 0)
  {
    value = 0xC0;

    if(motion->ClipFlag != 0)
      i = motion->clip_left;
    else
      i = 0;

    temp = i;

    for (j = validRgn->top; j <= validRgn->bottom; j++)
    {
      if (find_original_point_is_internal_2bit(motion, type, i, j) == 1)
        outbuf[j * (frame_width >> 2) + (temp >> 2)] = (value >> ((i & 3) << 1));
    }
  }

  if(realRgn->right >= frame_width || (motion->ClipFlag != 0 && motion->clip_right < frame_width))
  {
    value = 0x03;
    
    if(motion->ClipFlag != 0)
      i = motion->clip_right;
    else
      i = frame_width - 1;
    
    temp = i;
    
    for (j = validRgn->top; j <= validRgn->bottom; j++)
    {
      if (find_original_point_is_internal_2bit(motion, type, i, j) == 1)
        outbuf[j * (frame_width >> 2) + (temp >> 2)] = (value << (6 - ((i & 3) << 1)));
    }
  }

  if(realRgn->top < 0 || motion->ClipFlag != 0)
  {
    if(motion->ClipFlag != 0)
      j = motion->clip_top;
    else
      j = 0;

    for (i = validRgn->left; i <= validRgn->right; i++)
    {
      if (find_original_point_is_internal_2bit(motion, type, i, j) == 1)
      {
        SET_2BIT(outbuf, i << 1, j, frame_width >> 2);
      }
    }
  }

  if(realRgn->bottom >= frame_height || (motion->ClipFlag != 0 && motion->clip_bottom < frame_height))
  {
    if(motion->ClipFlag != 0)
      j = motion->clip_bottom;
    else
      j = frame_height - 1;

    for (i = validRgn->left; i <= validRgn->right; i++)
    {
      if (find_original_point_is_internal_2bit(motion, type, i, j) == 1)
      {
        SET_2BIT(outbuf, i << 1, j, frame_width >> 2);
      }
    }
  }
  return;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  find_original_point_is_internal_1bit

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 find_original_point_is_internal_1bit(saf_typeMotionInfo *motion, 
                                           saf_typeInfo *type, 
                                           int32 i_motion, 
                                           int32 j_motion)
{
  int32 i, j, temp = 0;
  int32 Xoffset = motion->xPosition;
  int32 Xposition, Yposition;
  int32 cos_angle=0, sin_angle=0;
  int32 xscalefactor=0, yscalefactor=0;
  int32 Object_Xsize = type->sizeX;
  int32 Object_Ysize = type->sizeY;
  uint8 *imagedata = type->imageData;

  if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
  {
    xscalefactor = saf_scale_factor_calc_x(motion);
    yscalefactor = saf_scale_factor_calc_y(motion);
  }

  if (motion->rotation_angle != 0)
  {
    cos_angle = icos[motion->rotation_angle];
    sin_angle = isin[motion->rotation_angle];
  }

  Xposition = i_motion - Xoffset;
  Yposition = j_motion - motion->yPosition;

  if (motion->rotation_angle != 0)
  {
    temp = Xposition;
    Xposition = (cos_angle * temp + sin_angle * Yposition + 8192) >> 14;
    Yposition = (cos_angle * Yposition - sin_angle * temp + 8192) >> 14;
  }

  if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
  {
    Xposition = Xposition * xscalefactor >> 8;
    Yposition = Yposition * yscalefactor >> 8;
  }

  if (motion->rightToLeft)
    Xposition = Object_Xsize - 1 - Xposition;

  if (motion->upsideDown)
    Yposition = Object_Ysize - 1 - Yposition;

  if (Xposition < 0 || Xposition >= Object_Xsize || Yposition < 0 || Yposition >= Object_Ysize)
    return(-1);

  Object_Xsize >>= 3;

  for (i = Xposition; i < (Object_Xsize << 3); i++)
  {
    temp = CHECK_1BIT(imagedata, i, Yposition, Object_Xsize);
    if (temp == 1)
      break;
  }
  
  if (temp == 0)
    return(0);
  
  temp = 0;
  for (i = Xposition; i >= 0; i--)
  {
    temp = CHECK_1BIT(imagedata, i, Yposition, Object_Xsize);
    if (temp == 1)
      break;
  }
  
  if (temp == 0)
    return(0);
  
  temp = 0;
  for (j = Yposition; j < Object_Ysize; j++)
  {
    temp = CHECK_1BIT(imagedata, Xposition, j, Object_Xsize);
    if (temp == 1)
      break;
  }
  
  if (temp == 0)
    return(0);
  
  temp = 0;
  for (j = Yposition; j >= 0; j--)
  {
    temp = CHECK_1BIT(imagedata, Xposition, j, Object_Xsize);
    if (temp == 1)
      break;
  }
  if (temp == 0)
    return(0);
  
  return(1);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  find_original_point_is_internal_2bit

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 find_original_point_is_internal_2bit(saf_typeMotionInfo *motion, 
                                           saf_typeInfo *type, 
                                           int32 i_motion, 
                                           int32 j_motion)
{
  int32 i, j, temp=0;
  int32 Xoffset = motion->xPosition;
  int32 Xposition, Yposition;
  int32 cos_angle=0, sin_angle=0;
  int32 xscalefactor=0, yscalefactor=0;
  int32 Object_Xsize = type->sizeX;
  int32 Object_Ysize = type->sizeY;
  uint8 *imagedata = type->imageData;

  if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
  {
    xscalefactor = saf_scale_factor_calc_x(motion);
    yscalefactor = saf_scale_factor_calc_y(motion);
  }

  if (motion->rotation_angle != 0)
  {
    cos_angle = icos[motion->rotation_angle];
    sin_angle = isin[motion->rotation_angle];
  }

  Xposition = i_motion - Xoffset;
  Yposition = j_motion - motion->yPosition;

  if (motion->rotation_angle != 0)
  {
    temp = Xposition;
    Xposition = (cos_angle * temp + sin_angle * Yposition + 8192) >> 14;
    Yposition = (cos_angle * Yposition - sin_angle * temp + 8192) >> 14;
  }

  if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
  {
    Xposition = Xposition * xscalefactor >> 8;
    Yposition = Yposition * yscalefactor >> 8;
  }

  if (motion->rightToLeft)
    Xposition = Object_Xsize - 1 - Xposition;

  if (motion->upsideDown)
    Yposition = Object_Ysize - 1 - Yposition;

  if (Xposition < 0 || Xposition >= Object_Xsize || Yposition < 0 || Yposition >= Object_Ysize)
    return(-1);

  Object_Xsize >>= 2;

  for (i = Xposition; i < (Object_Xsize << 2); i++)
  {
    temp = CHECK_2BIT(imagedata, i << 1, Yposition, Object_Xsize);
    if (temp != 0x00)
      break;
  }
  
  if (temp == 0)
    return(0);
  
  temp = 0;
  for (i = Xposition; i >= 0; i--)
  {
    temp = CHECK_2BIT(imagedata, i << 1, Yposition, Object_Xsize);
    if (temp != 0x00)
      break;
  }
  
  if (temp == 0)
    return(0);
  
  temp = 0;
  for (j = Yposition; j < Object_Ysize; j++)
  {
    temp = CHECK_2BIT(imagedata, Xposition << 1, j, Object_Xsize);
    if (temp != 0x00)
      break;
  }
  
  if (temp == 0)
    return(0);
  
  temp = 0;
  for (j = Yposition; j >= 0; j--)
  {
    temp = CHECK_2BIT(imagedata, Xposition << 1, j, Object_Xsize);
    if (temp != 0x00)
      break;
  }
  if (temp == 0)
    return(0);
  
  return(1);
}

/* <EJECT> */
/*===========================================================================
FUNCTION  saf_valid_region_check

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_valid_region_check
(
  int32 frameX, 
  int32 frameY, 
  saf_typeMotionInfo *a, 
  srect *b, 
  srect *d, 
  saf_typeInfo *c
)   
{
  int32 x_min, x_max;
  int32 y_min, y_max;
  int32 temp1, temp2, temp3, temp4;
  int32 x0 = 0, x1 = 0, x2 = 0, x3 = 0, y0 = 0, y1 = 0, y2 = 0, y3 = 0;
  int32 xscalefactor, yscalefactor;

  xscalefactor = a->x_scalefactor;
  yscalefactor = a->y_scalefactor;

  if(a->x_scale_type != 0 && xscalefactor == 0)
    xscalefactor = 256;

  if(a->y_scale_type != 0 && yscalefactor == 0)
    yscalefactor = 256;

  if (c->sizeX==0 || c->sizeY==0)
    return (1);

  if (a->rotation_angle != 0)
  {
    x_min = (int32) icos[a->rotation_angle] * (int32) (c->sizeX - 1);
    y_max = (int32) isin[a->rotation_angle] * (int32) (c->sizeY - 1);

    x_max = (int32) isin[a->rotation_angle] * (int32) (c->sizeX - 1);
    y_min = (int32) icos[a->rotation_angle] * (int32) (c->sizeY - 1);

    if (a->x_scale_type == 2)
    {
      temp1 = x_min * 256 / xscalefactor;
      temp3 = x_max * 256 / xscalefactor;
    }
    else if (a->x_scale_type == 1)
    {
      temp1 = x_min * xscalefactor / 256;
      temp3 = x_max * xscalefactor / 256;
    }
    else
    {
      temp1 = x_min;
      temp3 = x_max;
    }

    if (a->y_scale_type == 2)
    {
      temp2 = y_max * 256 / yscalefactor;
      temp4 = y_min * 256 / yscalefactor;
    }
    else if (a->y_scale_type == 1)
    {
      temp2 = y_max * yscalefactor / 256;
      temp4 = y_min * yscalefactor / 256;
    }
    else
    {
      temp2 = y_max;
      temp4 = y_min;
    }

    x0 = a->xPosition;
    x1 = ((temp1 + 8192) >> 14) + x0;
    x2 = ((temp1 - temp2 + 8192) >> 14)  + x0;
    x3 = ((8192 - temp2) >> 14)  + x0;

    y0 = a->yPosition;
    y1 = ((temp3 + 8192) >> 14)  + y0;
    y2 = ((temp3 + temp4 + 8192) >> 14) + y0;
    y3 = ((temp4 + 8192) >> 14) + y0;

    if (a->rotation_angle < 64)
    {
      x_max = x1;
      x_min = x3;
      y_max = y2;
      y_min = y0;
    }
    else if (a->rotation_angle >= 64 && a->rotation_angle < 128)
    {
      x_max = x0;
      x_min = x2;
      y_max = y1;
      y_min = y3;
    }
    else if (a->rotation_angle >= 128 && a->rotation_angle < 192)
    {
      x_max = x3;
      x_min = x1;
      y_max = y0;
      y_min = y2;
    }
    else
    {
      x_max = x2;
      x_min = x0;
      y_max = y3;
      y_min = y1; 
    }

    temp1 = x_min - a->xPosition;
    temp2 = y_min - a->yPosition;

    a->xPosition -= temp1;
    a->yPosition -= temp2;

    x_max -= temp1;
    x_min -= temp1;

    y_max -= temp2;
    y_min -= temp2;
  }
  else
  {
    x0 = a->xPosition;
    x3 = x0;

    if (a->x_scale_type == 2)
      x1 = ((int32) (c->sizeX - 1) * 256 / xscalefactor) + x0;
    else if (a->x_scale_type == 1)
      x1 = ((int32) (c->sizeX - 1) * xscalefactor) / 256 + x0;
    else // no scaling
      x1 = (int32) (c->sizeX - 1) + x0;
    x2 = x1;

    y0 = a->yPosition;
    y1 = y0;
    if (a->y_scale_type == 2)
      y2 = (int32) (c->sizeY - 1) * 256 / yscalefactor + y0;
    else if (a->y_scale_type == 1)
      y2 = (int32) (c->sizeY - 1) * yscalefactor / 256 + y0;
    else
      y2 = (int32) (c->sizeY - 1) + y0;
    y3 = y2;

    x_max = x1;
    x_min = x3;
    y_max = y2;
    y_min = y0;
  }

  if (d != NULL)
  {
    d->left = x_min;
    d->right = x_max;
    d->top = y_min;
    d->bottom = y_max;
  }

  if (x_min < 0)
    b->left = 0;
  else if (x_min >= frameX)
    b->left = frameX - 1;
  else
    b->left = x_min;

  if (x_max < 0)
    b->right = 0;
  else if (x_max >= frameX)
    b->right = frameX - 1;
  else
    b->right = x_max;

  if (y_min < 0)
    b->top = 0;
  else if (y_min >= frameY)
    b->top = frameY - 1;
  else
    b->top = y_min;

  if (y_max < 0)
    b->bottom = 0;
  else if (y_max >= frameY)
    b->bottom = frameY - 1;
  else
    b->bottom = y_max;

  /*if (a->ClipFlag)
  {
    if ((int32)a->clip_left > b->right || (int32)a->clip_right < b->left || (int32)a->clip_top > b->bottom || (int32)a->clip_bottom < b->top)
      return(1);  // There is no valid region.

    if ((int32)a->clip_left > b->left)
      b->left = (int32)a->clip_left;
    if ((int32)a->clip_right < b->right)
      b->right = (int32)a->clip_right;
    if ((int32)a->clip_top > b->top)
      b->top = (int32)a->clip_top;
    if ((int32)a->clip_bottom < b->bottom)
      b->bottom = (int32)a->clip_bottom;
  }*/
  return(0); // success
}

/* <EJECT> */
/*===========================================================================

FUNCTION  sis_ro_sc_postprocess

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 sis_ro_sc_postprocess(uint8 *pFrameBuf,
                            int32 Frame_Xsize, 
                            int32 Frame_Ysize,
                            saf_typeMotionInfo *motion,
                            saf_typeInfo *type,
                            srect *coord_frame,
                            srect *realRgn)
{
  int32 i, j;
  uint32 value1, value2, value3;
  int32 flimit;
  uint8 trans_color1, trans_color2, trans_color3;

  flimit = (Frame_Xsize * Frame_Ysize * saf_header_t.ImageLevel) >> 3;

  if(saf_header_t.ImageLevel == 4)
  {
    if(motion->transparency_type == 0)
    {
      for (j = coord_frame->top; j <= coord_frame->bottom; j++)
      {
        for (i = ((coord_frame->left + 1) >> 1) << 1; i <= ((coord_frame->right - 1) >> 1) << 1; i = i + 2)
        {
          value1 = sis_display_info_data.pOperationBuf1[j * (Frame_Xsize >> 1) + (i >> 1)];
          pFrameBuf[j * (Frame_Xsize >> 1) + (i >> 1)] = value1;
        }
        
        if(coord_frame->left & 0x01) /* odd number */
        {
          value1 = sis_display_info_data.pOperationBuf1[j * (Frame_Xsize >> 1) + (coord_frame->left >> 1)] & 0x0F;
          value2 = pFrameBuf[j * (Frame_Xsize >> 1) + (coord_frame->left >> 1)] & 0xF0;
          pFrameBuf[j * (Frame_Xsize >> 1) + (coord_frame->left >> 1)] = value1 | value2;
        }
        
        if((coord_frame->right & 0x01) == 0) /* even number */
        {
          value1 = sis_display_info_data.pOperationBuf1[j * (Frame_Xsize >> 1) + (coord_frame->right >> 1)] & 0xF0;
          value2 = pFrameBuf[j * (Frame_Xsize >> 1) + (coord_frame->right >> 1)] & 0x0F;
          pFrameBuf[j * (Frame_Xsize >> 1) + (coord_frame->right >> 1)] = value1 | value2;
        }
      }
    }
    else
    {
      trans_color1 = type->TransparencyColor;
      trans_color2 = type->TransparencyColor << 4;
      trans_color3 = trans_color1 | trans_color2;

      for (j = coord_frame->top; j <= coord_frame->bottom; j++)
      {
        for (i = ((coord_frame->left + 1) >> 1) << 1; i <= ((coord_frame->right - 1) >> 1) << 1; i = i + 2)
        {
          value1 = sis_display_info_data.pOperationBuf1[j * (Frame_Xsize >> 1) + (i >> 1)];
          if(value1 == trans_color3)
            continue;
          if((value1 & 0xF0) == trans_color2)
            value1 = (value1 & 0x0F) | (pFrameBuf[j * (Frame_Xsize >> 1) + (i >> 1)] & 0xF0);
          else if((value1 & 0x0F) == trans_color1)
            value1 = (value1 & 0xF0) | (pFrameBuf[j * (Frame_Xsize >> 1) + (i >> 1)] & 0x0F);

          pFrameBuf[j * (Frame_Xsize >> 1) + (i >> 1)] = value1;
        }
        
        if(coord_frame->left & 0x01) /* odd number */
        {
          value1 = sis_display_info_data.pOperationBuf1[j * (Frame_Xsize >> 1) + (coord_frame->left >> 1)] & 0x0F;
          if(value1 != trans_color1)
          {
            value2 = pFrameBuf[j * (Frame_Xsize >> 1) + (coord_frame->left >> 1)] & 0xF0;
            pFrameBuf[j * (Frame_Xsize >> 1) + (coord_frame->left >> 1)] = value1 | value2;
          }
        }
        
        if((coord_frame->right & 0x01) == 0) /* even number */
        {
          value1 = sis_display_info_data.pOperationBuf1[j * (Frame_Xsize >> 1) + (coord_frame->right >> 1)] & 0xF0;
          if(value1 != trans_color2)
          {
            value2 = pFrameBuf[j * (Frame_Xsize >> 1) + (coord_frame->right >> 1)] & 0x0F;
            pFrameBuf[j * (Frame_Xsize >> 1) + (coord_frame->right >> 1)] = value1 | value2;
          }
        }
      }
    }
  }
  else if (motion->transparency_type == 1)
  {
    edgedetection(sis_display_info_data.pOperationBuf1, Frame_Ysize, Frame_Xsize, coord_frame, realRgn, sis_display_info_data.pOperationBuf2, motion, type);
    for (i = 0; i < flimit; i = i + 4)
    {
      value1 = (*((uint32 *) (sis_display_info_data.pOperationBuf2 + i)));
      if (value1 != 0)
      {
        value2 = (*(uint32 *) (pFrameBuf + i));
        value3 = (*((uint32 *) (sis_display_info_data.pOperationBuf1 + i)));
        *((uint32 *) (pFrameBuf + i)) = ((~value1) & value2) | value3;
      }
    }
  }
  else if (motion->transparency_type == 3)
  {

    for (i = 0; i < flimit; i = i + 4)
    {
      value1 = (*((uint32 *) (pFrameBuf + i)));
      value2 = (*((uint32 *) (sis_display_info_data.pOperationBuf1 + i)));
      (*((uint32 *) (pFrameBuf + i))) = value1 | value2;
    }
  }
  else if (motion->transparency_type == 0)
  {

    for (i = 0; i < flimit; i = i + 4)
    {
      value1 = (*((uint32 *) (sis_display_info_data.pOperationBuf2 + i)));
      value2 = (*(uint32 *) (pFrameBuf + i));
      value3 = (*((uint32 *) (sis_display_info_data.pOperationBuf1 + i)));

      *((uint32 *) (pFrameBuf + i)) = ((value1) & value3) | ((~value1) & value2);
    }
  }
  return(0);
}     

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_990000

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_990000(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp4, posy, i, j;

  Object_Xsize >>= 3;
  Frame_Xsize >>= 3;
  posy = (Y0 - 1) * Frame_Xsize;

  temp1 = Xoffset & 7;
  temp4 = 8 - temp1;


  if (temp1 == 0)
  {
    for (j = Y0; j <= Y2; j++)
    {
      posy += Frame_Xsize;
      memcpy(pOutBuf + posy + (X0 >> 3), pObjectBuf + (j - motion->yPosition) * Object_Xsize + ((X0 - Xoffset) >> 3), ((X2 - X0 + 1) >> 3));
    }
  }
  else
  {
    temp = (Y0 - 1 - motion->yPosition) * Object_Xsize;
    for (j = Y0; j <= Y2; j++)
    {
      posy += Frame_Xsize;
      temp += Object_Xsize;
      if(Xoffset >= 0)
        pOutBuf[posy + (X0 >> 3)] = ((pOutBuf[posy + (X0 >> 3)] >> temp4) << temp4)
                                  | (pObjectBuf[temp + ((X0 - Xoffset) >> 3)] >> temp1);

      for (i = X0; i <= (X2 - 8 + (Xoffset < 0)); i = i + 8)
      {
        pOutBuf[posy  +  (i  >>  3)  +  (Xoffset  >=  0)] =
        (uint8) (pObjectBuf[temp + ((i - Xoffset) >> 3)] << temp4)
        | (pObjectBuf[temp + ((i - Xoffset) >> 3) + 1] >> temp1);
      }
      if (X2 != ((Frame_Xsize << 3) - 1))
      {
       pOutBuf[posy + (X2 >> 3)] = ((uint8) (pOutBuf[posy + (X2 >> 3)] << temp1) >> temp1) 
                                    | ((uint8) (pObjectBuf[temp + ((X2 - Xoffset) >> 3)] << temp4));
      }
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_990001

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_990001(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp4, posy, i, j;

  Object_Xsize >>= 3;
  Frame_Xsize >>= 3;
  posy = (Y0 - 1) * Frame_Xsize;

  temp1 = Xoffset & 7;
  temp4 = 8 - temp1;


  if (temp1 == 0)
  {
    for (j = Y0; j <= Y2; j++)
    {
      posy += Frame_Xsize;
      memcpy(pOutBuf + posy + (X0 >> 3), pObjectBuf + ((int32) type_local->sizeY - 1 - j + motion->yPosition)
        * Object_Xsize + ((X0 - Xoffset) >> 3), (X2 - X0 + 1) >> 3);
    }
  }
  else
  {
    temp = ((int32) type_local->sizeY - 1 - (Y0 - 1) + motion->yPosition) * Object_Xsize;
    for (j = Y0; j <= Y2; j++)
    {
      posy += Frame_Xsize;
      temp -= Object_Xsize;
      if(Xoffset >= 0)
        pOutBuf[posy + (X0 >> 3)] = ((pOutBuf[posy + (X0 >> 3)] >> temp4) << temp4)
                                  |  (pObjectBuf[temp + ((X0 - Xoffset) >> 3)] >> temp1);

      for (i = X0; i <= (X2 - 8 + (Xoffset < 0)); i = i + 8)
      {
        pOutBuf[posy + (i >> 3) + (Xoffset >= 0)] =
        (uint8) (pObjectBuf[temp + ((i - Xoffset) >> 3)] << temp4)
        | (pObjectBuf[temp + ((i - Xoffset) >> 3) + 1] >> temp1);
      }
      if (X2 != ((Frame_Xsize << 3) - 1))
      {
        pOutBuf[posy + (X2 >> 3)] = ((uint8) (pOutBuf[posy + (X2 >> 3)] << temp1) >> temp1) 
                                    | ((uint8) (pObjectBuf[temp + ((X2 - Xoffset) >> 3)] << temp4));
      }
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_990010

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_990010(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, temp5, temp6, posy, i, j, Xposition;
  uint8 * ysk_reversebit;

  if (saf_header_t.ImageLevel == 4)
    ysk_reversebit = (uint8 *) ysk_reversebit_4bit;
  else if (saf_header_t.ImageLevel == 2)
    ysk_reversebit = (uint8 *) ysk_reversebit_2bit;
  else if (saf_header_t.ImageLevel == 1)
    ysk_reversebit = (uint8 *) ysk_reversebit_1bit;
  else
    return(1);

  Object_Xsize >>= 3;
  Frame_Xsize >>= 3;

  posy = (Y0 - 1) * Frame_Xsize;

  temp1 = Xoffset & 7;
  temp2 = (Object_Xsize << 3) - 1 + Xoffset - (X0 - 8);
  temp = (Y0 - 1 - (int32) motion->yPosition) * Object_Xsize;
  for (j = Y0; j <= Y2; j++)
  {
    posy += Frame_Xsize;
    temp += Object_Xsize;
    temp3 = pOutBuf[posy + (X0 >> 3)];
    temp4 = temp + (((Object_Xsize << 3) - 1 - X0 + Xoffset) >> 3);
    if(Xoffset >= 0)
      pOutBuf[posy + (X0 >> 3)] = ((temp3 >> (8 - temp1)) << (8 - temp1))
                                | (ysk_reversebit[(pObjectBuf[temp4])] >> temp1);
    if(temp1 == 0 && Xoffset < 0)
    Xposition = temp2 + 8;
  else
    Xposition = temp2;

    for (i = X0; i <= (X2 - 8 + (Xoffset < 0)); i = i + 8)
    {
      Xposition -= 8;
      temp5 = pObjectBuf[temp + (Xposition >> 3)];
      temp6 = pObjectBuf[temp + (Xposition >> 3) - 1];
      pOutBuf[posy + (i >> 3) + (Xoffset >= 0)] = ((uint8) (ysk_reversebit[temp5] << (8 - temp1)))
                                                  | ((uint8) (ysk_reversebit[temp6] >> temp1));
    }
    if (X2 != ((Frame_Xsize << 3) - 1))
    {
      temp5 = pOutBuf[posy + (X2 >> 3)] << temp1;
      temp6 = pObjectBuf[temp + (((Object_Xsize << 3) - 1 - X2 + Xoffset) >> 3)];
      pOutBuf[posy + (X2 >> 3)] = (((uint8) temp5) >> temp1)
                                  | ((uint8) (ysk_reversebit[temp6] << (8 - temp1)));
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_990011

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_990011(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, temp5, temp6, posy, i, j, Xposition;
  uint8 * ysk_reversebit;

  if (saf_header_t.ImageLevel == 4)
    ysk_reversebit = (uint8 *) ysk_reversebit_4bit;
  else if (saf_header_t.ImageLevel == 2)
    ysk_reversebit = (uint8 *) ysk_reversebit_2bit;
  else if (saf_header_t.ImageLevel == 1)
    ysk_reversebit = (uint8 *) ysk_reversebit_1bit;
  else
    return(1);

  Object_Xsize = Object_Xsize >> 3;
  Frame_Xsize = Frame_Xsize >> 3;
  posy = (Y0 - 1) * Frame_Xsize;

  temp1 = Xoffset & 7;
  temp2 = (Object_Xsize << 3) - 1 + Xoffset - (X0 - 8);
  temp = ((int32) type_local->sizeY + motion->yPosition - Y0) * Object_Xsize;
  for (j = Y0; j <= Y2; j++)
  {
    posy += Frame_Xsize;
    temp -= Object_Xsize;
    temp3 = pOutBuf[posy + (X0 >> 3)];
    temp4 = temp + (((Object_Xsize << 3) - 1 - X0 + Xoffset) >> 3);
    if(Xoffset >= 0)
      pOutBuf[posy + (X0 >> 3)] = ((temp3 >> (8 - temp1)) << (8 - temp1))
                                | (ysk_reversebit[(pObjectBuf[temp4])] >> temp1);
    if(temp1 == 0 && Xoffset < 0)
    Xposition = temp2 + 8;
  else
    Xposition = temp2;

    for (i = X0; i <= (X2 - 8 + (Xoffset < 0)); i = i + 8)
    {
      Xposition -= 8;
      temp5 = pObjectBuf[temp + (Xposition >> 3)];
      temp6 = pObjectBuf[temp + (Xposition >> 3) - 1];
      pOutBuf[posy + (i >> 3) + (Xoffset >= 0)] = ((uint8) (ysk_reversebit[temp5] << (8 - temp1)))
                                                  | ((uint8) (ysk_reversebit[temp6] >> temp1));
    }
    if (X2 != ((Frame_Xsize << 3) - 1))
    {
      temp5 = pOutBuf[posy + (X2 >> 3)] << temp1;
      temp6 = pObjectBuf[temp + (((Object_Xsize << 3) - 1 - X2 + Xoffset) >> 3)];
      pOutBuf[posy + (X2 >> 3)] = (((uint8) temp5) >> temp1)
                                  | ((uint8) (ysk_reversebit[temp6] << (8 - temp1)));
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_190100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_190100(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp2 = cos_angle * ((X0 - 1) - Xoffset) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * ((X0 - 1) - Xoffset);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 = j * (Frame_Xsize >> 3);
    temp = temp2 + sin_angle * j;
    temp3 = temp4 + cos_angle * j;
    for (i = X0; i <= X2; i++)
    {
      temp += cos_angle;
      temp3 -= sin_angle;
      Xposition = temp >> 14;
      Yposition = temp3 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (0xffffff7f >> (i & 7));

      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)] << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_290100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_290100(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp3 = (((cos_angle * ((X0 - 2) - Xoffset)) >> 1) - sin_angle * motion->yPosition + 8192);
  temp4 = 8192 - cos_angle * motion->yPosition - ((sin_angle * ((X0 - 2) - Xoffset)) >> 1);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 = j * (Frame_Xsize >> 3);
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    for (i = X0; i <= X2; i = i + 2)
    {
      temp += cos_angle;
      temp2 -= sin_angle;
      Xposition = (temp >> 14) << 1;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(192 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));
      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_490100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_490100(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp3 = cos_angle * (((X0 - 4) - Xoffset) >> 2) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * (((X0 - 4) - Xoffset) >> 2);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += cos_angle;
      temp2 -=sin_angle;
      Xposition = (temp >> 14) << 2;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(240 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return (0);
}


/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_190101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_190101(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp2 = cos_angle * (X0 - 1 - Xoffset) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * (X0 - 1 - Xoffset);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = temp2 + sin_angle * j;
    temp1 += (Frame_Xsize >> 3);
    temp3 = temp4 + cos_angle * j;
    for (i = X0; i <= X2; i++)
    {
      temp += cos_angle;
      temp3 -= sin_angle;
      Xposition = temp >> 14;
      Yposition = temp3 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (0xffffff7f >> (i & 7));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;  // now
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_290101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_290101(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp3 = (((cos_angle * ((X0 - 2) - Xoffset)) >> 1) - sin_angle * motion->yPosition + 8192);
  temp4 = 8192 - cos_angle * motion->yPosition - ((sin_angle * ((X0 - 2) - Xoffset)) >> 1);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 += (Frame_Xsize >> 3);
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    for (i = X0; i <= X2; i = i + 2)
    {
      temp += cos_angle;
      temp2 -= sin_angle;
      Xposition = (temp >> 14) << 1;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));
      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;  // now
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_490101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_490101(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp3 = cos_angle * (((X0 - 4) - Xoffset) >> 2) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * (((X0 - 4) - Xoffset) >> 2);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += cos_angle;
      temp2 -=sin_angle;
      Xposition = (temp >> 14) << 2;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;
      Yposition  =  ( int32)  type_local->sizeY  -  1  -  Yposition;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xF0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_190110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_190110(uint8 *pObjectBuf,
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp2 = cos_angle * (X0 - 1 - Xoffset) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192- cos_angle * motion->yPosition - sin_angle * (X0 - 1 - Xoffset);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 += (Frame_Xsize >> 3);
    temp = temp2 + sin_angle * j;
    temp3 = temp4 + cos_angle * j;
    for (i = X0; i <= X2; i++)
    {
      temp += cos_angle;
      temp3 -= sin_angle;
      Xposition = temp >> 14;
      Yposition = temp3 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }
      Xposition = Object_Xsize - 1 - Xposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (0xffffff7f >> (i & 7));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;  // now
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_290110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_290110(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp3 = (((cos_angle * ((X0 - 2) - Xoffset)) >> 1) - sin_angle * motion->yPosition + 8192);
  temp4 = 8192 - cos_angle * motion->yPosition - ((sin_angle * ((X0 - 2) - Xoffset)) >> 1);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 = j * (Frame_Xsize >> 3);
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    for (i = X0; i <= X2; i = i + 2)
    {
      temp += cos_angle;
      temp2 -= sin_angle;
      Xposition = (temp >> 14) << 1;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = Object_Xsize - 1 - Xposition - 1;

      status = 1;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));
      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;  // now
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_490110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_490110(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp3 = cos_angle * (((X0 - 4) - Xoffset) >> 2) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * (((X0 - 4) - Xoffset) >> 2);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += cos_angle;
      temp2 -=sin_angle;
      Xposition = (temp >> 14) << 2;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = Object_Xsize - 1 - Xposition - 3;
      status = 1;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xf0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION fill_image_190111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_190111(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp2 = cos_angle * ((X0 - 1) - Xoffset) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192- cos_angle * motion->yPosition - sin_angle * ((X0 - 1) - Xoffset);

  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 += (Frame_Xsize >> 3);
    temp = temp2 + sin_angle * j;
    temp3 = temp4 + cos_angle * j;
    for (i = X0; i <= X2; i++)
    {
      temp += cos_angle;
      temp3 -= sin_angle;
      Xposition = temp >> 14;
      Yposition = temp3 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = Object_Xsize - 1 - Xposition;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0x80 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_290111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_290111(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp3 = (((cos_angle * ((X0 - 2) - Xoffset)) >> 1) - sin_angle * motion->yPosition + 8192);
  temp4 = 8192 - cos_angle * motion->yPosition - ((sin_angle * ((X0 - 2) - Xoffset)) >> 1);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 = j * (Frame_Xsize >> 3);
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    for (i = X0; i <= X2; i = i + 2)
    {
      temp += cos_angle;
      temp2 -= sin_angle;
      Xposition = (temp >> 14) << 1;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;
      Xposition = Object_Xsize - 2 - Xposition;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));
      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_490111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_490111(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  int32 status;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  uint32 longvalue;

  temp3 = cos_angle * (((X0 - 4) - Xoffset) >> 2) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * (((X0 - 4) - Xoffset) >> 2);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += cos_angle;
      temp2 -=sin_angle;
      Xposition = (temp >> 14) << 2;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;

      Xposition = Object_Xsize - 1 - Xposition - 3;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xf0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_scale_factor_calc_x

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_scale_factor_calc_x
(
  saf_typeMotionInfo* motion
)
{
  int32 xscalefactor;

  if(motion->x_scale_type != 0 && motion->x_scalefactor == 0)
  {
    xscalefactor = 256;
    return(xscalefactor);
  }

  if (motion->x_scale_type == 2)
    xscalefactor = motion->x_scalefactor;
  else if (motion->x_scale_type == 1)
    xscalefactor = ((256 * 256) / motion->x_scalefactor);
  else
    xscalefactor = 256;
  return(xscalefactor);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_scale_factor_calc_y
DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_scale_factor_calc_y
(
  saf_typeMotionInfo* motion
)
{
  int32 yscalefactor;

  if(motion->y_scale_type != 0 && motion->y_scalefactor == 0)
  {
    yscalefactor = 256;
    return(yscalefactor);
  }

  if (motion->y_scale_type == 2)
    yscalefactor = motion->y_scalefactor;
  else if (motion->y_scale_type == 1)
    yscalefactor = ((256 * 256) / motion->y_scalefactor);
  else
    yscalefactor = 256;
  return(yscalefactor);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_191000

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_191000(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp4, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp2 = (Y0 - 1 - motion->yPosition) * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    temp2 += yscalefactor;
    temp = ((temp2) >> 8) * (Object_Xsize >> 3);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; (i & 7) != 0; i++)
    {
      Xposition = ((i - Xoffset) * xscalefactor) >> 8;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (0xffffff7f >> (i & 7));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)] << (Xposition & 7)) >> 7) << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }

    for (i = i; i < X2 - 8; i = i + 8)
    {
      temp4 = 0;
      Xposition = ((i - Xoffset) * xscalefactor) >> 8;

      temp4 += ((uint8) (pObjectBuf[temp + (Xposition >> 3)] << (Xposition & 7)) >> 7);
      temp4 <<= 1;
      Xposition = ((i + 1 - Xoffset) * xscalefactor) >> 8;
      temp4 += ((uint8) (pObjectBuf[temp + (Xposition >> 3)] << (Xposition & 7)) >> 7);
      temp4 <<= 1;
      Xposition = ((i + 2 - Xoffset) * xscalefactor) >> 8;
      temp4 += ((uint8) (pObjectBuf[temp + (Xposition >> 3)] << (Xposition & 7)) >> 7);
      temp4 <<= 1;
      Xposition = ((i + 3 - Xoffset) * xscalefactor) >> 8;
      temp4 += ((uint8) (pObjectBuf[temp + (Xposition >> 3)] << (Xposition & 7)) >> 7);
      temp4 <<= 1;
      Xposition = ((i + 4 - Xoffset) * xscalefactor) >> 8;
      temp4 += ((uint8) (pObjectBuf[temp + (Xposition >> 3)] << (Xposition & 7)) >> 7);
      temp4 <<= 1;
      Xposition = ((i + 5 - Xoffset) * xscalefactor) >> 8;
      temp4 += ((uint8) (pObjectBuf[temp + (Xposition >> 3)] << (Xposition & 7)) >> 7);
      temp4 <<= 1;
      Xposition = ((i + 6 - Xoffset) * xscalefactor) >> 8;
      temp4 += ((uint8) (pObjectBuf[temp + (Xposition >> 3)] << (Xposition & 7)) >> 7);
      temp4 <<= 1;
      Xposition = ((i + 7 - Xoffset) * xscalefactor) >> 8;
      temp4 += ((uint8) (pObjectBuf[temp + (Xposition >> 3)] << (Xposition & 7)) >> 7);

      pOutBuf[temp1 + (i >> 3)] = (uint8) temp4;
    }

    for (i = i; i <= X2; i++)
    {
      Xposition = ((i - Xoffset) * xscalefactor) >> 8;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (0xffffff7f >> (i & 7));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_291000

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_291000(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, i, j, Xposition;
  uint8 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp = (((j - motion->yPosition) * yscalefactor) >> 8) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 2)
    {
      Xposition = (((i - Xoffset) * xscalefactor) >> 8);
      Xposition &= 0xfffffffe;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (0xffffff3f >> (i & 7));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_491000

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_491000(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp = (((j - motion->yPosition) * yscalefactor) >> 8) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = (((i - Xoffset) * xscalefactor) >> 8);
      Xposition &= 0xfffffffc;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));
      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_191001

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_191001(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp = ((int32) type_local->sizeY - 1 - ((j - motion->yPosition) * yscalefactor >> 8)) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i++)
    {
      Xposition = (((i - Xoffset) * xscalefactor) >> 8);

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0x80 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_291001

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_291001(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp = ((int32) type_local->sizeY - 1 - ((j - motion->yPosition) * yscalefactor >> 8)) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 2)
    {
      Xposition = ((((i - Xoffset) * xscalefactor) >> 9)) << 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_491001

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_491001(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, i, j, Xposition;
  uint8 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp1 = j * (Frame_Xsize >> 3);
    temp = ((int32) type_local->sizeY - 1 - (((j - motion->yPosition) * yscalefactor) >> 8)) * (Object_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = (((i - Xoffset) * xscalefactor) >> 10) << 2;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));
      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_191010

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_191010(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp2 = (Y0 - 1 - motion->yPosition) * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    temp2 += yscalefactor;
    temp = (temp2 >> 8) * (Object_Xsize >> 3);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i++)
    {
      Xposition = ((i - Xoffset) * xscalefactor >> 8);
      Xposition = Object_Xsize - 1 - Xposition;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0x80 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_291010

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_291010(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp2= (Y0 - 1 - motion->yPosition) * yscalefactor;

  for (j = Y0; j <= Y2; j++)
  {
    temp2 += yscalefactor;
    temp = (temp2 >> 8) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 2)
    {
      Xposition = (((i - Xoffset) * xscalefactor) >> 9)  << 1;
      Xposition = Object_Xsize - 1 - Xposition - 1;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_491010

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_491010(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp2 = (Y0 - 1 - motion->yPosition) * yscalefactor;

  for (j = Y0; j <= Y2; j++)
  {
    temp2 += yscalefactor;
    temp = (temp2 >> 8) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = Object_Xsize - 4 - ((((i - Xoffset) * xscalefactor) >> 10) << 2);

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));
      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_191011

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_191011(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp = ((int32) type_local->sizeY - 1 - ((j - motion->yPosition) * yscalefactor >> 8)) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i++)
    {
      Xposition = Object_Xsize - 1 - (((i - Xoffset) * xscalefactor) >> 8);

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0x80 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_291011

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_291011(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp = ((int32) type_local->sizeY - 1 - ((j - motion->yPosition) * yscalefactor >> 8)) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 2)
    {
      Xposition = Object_Xsize - 2 - ((((i - Xoffset) * xscalefactor) >> 9) << 1);

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_491011

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_491011(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, i, j, Xposition;
  uint32 longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp1 = j * (Frame_Xsize >> 3);
    temp = ((int32) type_local->sizeY - 1 - (((j - motion->yPosition) * yscalefactor) >> 8)) * (Object_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = Object_Xsize - 4 - ((((i - Xoffset) * xscalefactor) >> 10) << 2);

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0f << (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[temp + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));
      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_191100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_191100(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, temp5, temp6, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp3 = cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  temp5 = (cos_angle * ((X0 - 1) - Xoffset) - sin_angle * motion->yPosition + 8192) * xscalefactor;
  temp6 = (8192 - cos_angle * motion->yPosition - sin_angle * ((X0 - 1) - Xoffset)) * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 += (Frame_Xsize >> 3);
    temp = temp5 + sin_angle * xscalefactor * j;
    temp2 = temp6 + cos_angle * j * yscalefactor;
    for (i = X0; i <= X2; i++)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(128 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_291100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_291100(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp3 = 2 * cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = ((cos_angle * ((X0 - 2 - Xoffset)) + 2 * sin_angle * (j - motion->yPosition) + 16384) ) * xscalefactor;
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 2 - Xoffset) >> 1) + 8192) ) * yscalefactor;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 2)
    {
      temp += temp3;
      temp2 -= temp4;

      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition &= 254;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_491100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_491100(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (4 * cos_angle * xscalefactor);
  temp4 = (sin_angle * yscalefactor);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = (((cos_angle * ((X0 - 4 - Xoffset) >> 2) + sin_angle * (j - motion->yPosition) + 8192) << 2) * xscalefactor);
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 4 - Xoffset) >> 2) + 8192) * yscalefactor);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 2) << 2;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_191101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_191101(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, temp5, temp6, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp3 = cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  temp5 = (cos_angle * ((X0 - 1) - Xoffset) - sin_angle * motion->yPosition + 8192) * xscalefactor;
  temp6 = (8192 - cos_angle * motion->yPosition - sin_angle * ((X0 - 1) - Xoffset)) * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 += (Frame_Xsize >> 3);
    temp = temp5 + sin_angle * xscalefactor * j;
    temp2 = temp6 + cos_angle * j * yscalefactor;
    for (i = X0; i <= X2; i++)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(128 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_291101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_291101(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp3 = 2 * cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = ((cos_angle * ((X0 - 2 - Xoffset)) + 2 * sin_angle * (j - motion->yPosition) + 16384) ) * xscalefactor;
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 2 - Xoffset) >> 1) + 8192) ) * yscalefactor;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 2)
    {
      temp += temp3;
      temp2 -= temp4;

      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 1) << 1;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_491101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_491101(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (4 * cos_angle * xscalefactor);
  temp4 = (sin_angle * yscalefactor);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = (((cos_angle * ((X0 - 4 - Xoffset) >> 2) + sin_angle * (j - motion->yPosition) + 8192) << 2) * xscalefactor);
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 4 - Xoffset) >> 2) + 8192) * yscalefactor);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 2) << 2;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_191110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_191110(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, temp5, temp6, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp3 = cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  temp5 = (cos_angle * ((X0 - 1) - Xoffset) - sin_angle * motion->yPosition + 8192) * xscalefactor;
  temp6 = (8192 - cos_angle * motion->yPosition - sin_angle * ((X0 - 1) - Xoffset)) * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 += (Frame_Xsize >> 3);
    temp = temp5 + sin_angle * xscalefactor * j;
    temp2 = temp6 + cos_angle * j * yscalefactor;
    for (i = X0; i <= X2; i++)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = Object_Xsize - 1 - Xposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (0xffffff7f >> (i & 7));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_291110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_291110(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp3 = 2 * cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = ((cos_angle * ((X0 - 2 - Xoffset)) + 2 * sin_angle * (j - motion->yPosition) + 16384) ) * xscalefactor;
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 2 - Xoffset) >> 1) + 8192) ) * yscalefactor;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 2)
    {
      temp += temp3;
      temp2 -= temp4;

      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 1) << 1;
      Xposition = Object_Xsize - 1 - Xposition - 1;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_491110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_491110(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (4 * cos_angle * xscalefactor);
  temp4 = (sin_angle * yscalefactor);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = (((cos_angle * ((X0 - 4 - Xoffset) >> 2) + sin_angle * (j - motion->yPosition) + 8192) << 2) * xscalefactor);
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 4 - Xoffset) >> 2) + 8192) * yscalefactor);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 2) << 2;
      Xposition = Object_Xsize - 1 - Xposition - 3;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_191111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_191111(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, temp5, temp6, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp3 = cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  temp5 = (cos_angle * ((X0 - 1) - Xoffset) - sin_angle * motion->yPosition + 8192) * xscalefactor;
  temp6 = (8192 - cos_angle * motion->yPosition - sin_angle * ((X0 - 1) - Xoffset)) * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp1 += (Frame_Xsize >> 3);
    temp = temp5 + sin_angle * xscalefactor * j;
    temp2 = temp6 + cos_angle * j * yscalefactor;
    for (i = X0; i <= X2; i++)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }
      Xposition = Object_Xsize - 1 - Xposition;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(128 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 7)
                   << (7 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_291111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_291111(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  temp3 = 2 * cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = ((cos_angle * ((X0 - 2 - Xoffset)) + 2 * sin_angle * (j - motion->yPosition) + 16384) ) * xscalefactor;
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 2 - Xoffset) >> 1) + 8192) ) * yscalefactor;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 2)
    {
      temp += temp3;
      temp2 -= temp4;

      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = ((Xposition >> 1) << 1);
      Xposition = Object_Xsize - 1 - Xposition - 1;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xC0 >> (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 6)
                   << (6 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}

/* <EJECT> */

/*===========================================================================

FUNCTION  fill_image_491111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_491111(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, i, j, Xposition, Yposition;
  uint32 longvalue;
  int32 cos_angle = icos[motion->rotation_angle];
  int32 sin_angle = isin[motion->rotation_angle];
  int32 xscalefactor, yscalefactor;
  int32 status;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (4 * cos_angle * xscalefactor);
  temp4 = (sin_angle * yscalefactor);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = (((cos_angle * ((X0 - 4 - Xoffset) >> 2) + sin_angle * (j - motion->yPosition) + 8192) << 2) * xscalefactor);
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 4 - Xoffset) >> 2) + 8192) * yscalefactor);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 2) << 2;
      //Xposition = Object_Xsize - 4 - Xposition - 3;
      Xposition = Object_Xsize - 1 - Xposition - 3;

      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      status = 1;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (
                   ((uint8) (pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                             << (Xposition & 7)) >> 4)
                   << (4 - (i & 7)));

      pOutBuf[temp1 + (i >> 3)] = (uint8) longvalue;
    }
  }
  return(0);
}
/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_420000

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_420000(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1;
  int32 i, j;
  int32 Xposition;
  uint8 value;

  for (j = Y0; j <= Y2; j++)
  {
    temp = (j - motion->yPosition) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);

    Xposition = ((X0 - 4) - Xoffset);

    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition += 4;
      value = (uint8) (((uint32) pObjectBuf[temp + (Xposition >> 3)]
        << (24 + (Xposition & 7))) >> 28);

      if (value == type_local->TransparencyColor)
        continue;

      if((i & 7) == 0)
        pOutBuf[temp1 + (i >> 3)] = ((pOutBuf[temp1 + (i >> 3)]) & 0x0F) | (value << 4);
      else
        pOutBuf[temp1 + (i >> 3)] = (pOutBuf[temp1 + (i >> 3)] & 0xF0) | value;
    }
  }       
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_420001

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_420001(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1;
  int32 i, j;
  int32 Xposition;
  uint8 value, longvalue;

  temp = ((int32) type_local->sizeY - Y0 + motion->yPosition) * (Object_Xsize >> 3);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    temp -= (Object_Xsize >> 3);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = i - Xoffset;

      value = (uint8) ((((uint32) pObjectBuf[temp + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xF0 >> (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }       
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_420010

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_420010(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1;
  int32 i, j;
  int32 Xposition;
  uint8 value, longvalue;

  for (j = Y0; j <= Y2; j++)
  {
    temp = (j - motion->yPosition) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = Object_Xsize - 4 + Xoffset - i;

      value = (uint8) ((((uint32) pObjectBuf[temp + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));
      if (value == type_local->TransparencyColor)
        continue;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xF0 >> (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_420011

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_420011(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo* motion,
                        saf_typeInfo* type_local)
{
  int32 temp, temp1;
  int32 i, j;
  int32 Xposition;
  uint8 value, longvalue;

  for (j = Y0; j <= Y2; j++)
  {
    temp = ((int32) type_local->sizeY - 1 + motion->yPosition - j) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = (Object_Xsize - 4 + Xoffset - i);
      value = (uint8) ((((uint32) pObjectBuf[temp + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));
      if (value == type_local->TransparencyColor)
        continue;
      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= (~(0xF0 >> (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_420100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_420100(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 i, j, status;
  int32 Xposition, Yposition;
  uint8 value, longvalue;
  int32 cos_angle, sin_angle;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  temp3 = cos_angle * (((X0 - 4) - Xoffset) >> 2) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * (((X0 - 4) - Xoffset) >> 2);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += cos_angle;
      temp2 -=sin_angle;
      Xposition = (temp >> 14) << 2;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      //Xposition = (Xposition >> 2) << 2;
      status = 1;

      value = (uint8) ((((uint32) pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xF0 >> (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_420101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_420101(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 i, j, status;
  int32 Xposition, Yposition;
  uint8 value, longvalue;
  int32 cos_angle, sin_angle;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  temp3 = cos_angle * (((X0 - 4) - Xoffset) >> 2) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * (((X0 - 4) - Xoffset) >> 2);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += cos_angle;
      temp2 -=sin_angle;
      Xposition = (temp >> 14) << 2;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;

      value = (uint8) ((((uint32) pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xF0 >> (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_420110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_420110(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 i, j, status;
  int32 Xposition, Yposition;
  uint8 value, longvalue;
  int32 cos_angle, sin_angle;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  temp3 = cos_angle * (((X0 - 4) - Xoffset) >> 2) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * (((X0 - 4) - Xoffset) >> 2);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += cos_angle;
      temp2 -=sin_angle;
      Xposition = (temp >> 14) << 2;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
     {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = Object_Xsize - 1 - Xposition - 3;
      status = 1;

      value = (uint8) ((((uint32) pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xF0 >> (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_420111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_420111(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 i, j, status;
  int32 Xposition, Yposition;
  uint8 value, longvalue;
  int32 cos_angle, sin_angle;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  temp3 = cos_angle * (((X0 - 4) - Xoffset) >> 2) - sin_angle * motion->yPosition + 8192;
  temp4 = 8192 - cos_angle * motion->yPosition - sin_angle * (((X0 - 4) - Xoffset) >> 2);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);

  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = temp3 + sin_angle * j;
    temp2 = temp4 + cos_angle * j;
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += cos_angle;
      temp2 -=sin_angle;
      Xposition = (temp >> 14) << 2;
      Yposition = temp2 >> 14;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      status = 1;

      Xposition = Object_Xsize - 1 - Xposition - 3;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;

      value = (uint8) ((((uint32) pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xF0 >> (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_421000

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_421000(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1;
  int32 i, j;
  int32 Xposition;
  uint8 value, longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp = (((j - motion->yPosition) * yscalefactor) >> 8) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = (((i - Xoffset) * xscalefactor) >> 8);
      Xposition = (Xposition >> 2) << 2;

      value = (uint8) ((((uint32)pObjectBuf[temp + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &=  (~(0xF0 >> (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_421001

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_421001(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1;
  int32 i, j;
  int32 Xposition;
  uint8 value, longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp1 = j * (Frame_Xsize >> 3);
    temp = ((int32) type_local->sizeY - 1 - (((j - motion->yPosition) * yscalefactor) >> 8)) * (Object_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = (((i - Xoffset) * xscalefactor) >> 10) << 2;

      value = (uint8) ((((uint32)pObjectBuf[temp + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (  value << (4 - (i & 7))); 
      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_421010

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_421010(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1;
  int32 i, j;
  int32 Xposition;
  uint8 value, longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp = (((j - motion->yPosition) * yscalefactor) >> 8) * (Object_Xsize >> 3);
    temp1 = j * (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = (((i - Xoffset) * xscalefactor) >> 10) << 2;
      Xposition = Object_Xsize - 1 - Xposition - 3;

      value = (uint8) ((((uint32)pObjectBuf[temp + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 
      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_421011

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_421011(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1;
  int32 i, j;
  int32 Xposition;
  uint8 value, longvalue;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  for (j = Y0; j <= Y2; j++)
  {
    temp1 = j * (Frame_Xsize >> 3);
    temp = ((int32) type_local->sizeY - 1 - (((j - motion->yPosition) * yscalefactor) >> 8)) * (Object_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      Xposition = (((i - Xoffset) * xscalefactor) >> 10) << 2;
      Xposition = Object_Xsize - 1 - Xposition - 3;

      value = (uint8) ((((uint32) pObjectBuf[temp + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (  value << (4 - (i & 7))); 
      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_421100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_421100(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 i, j, status;
  int32 Xposition, Yposition;
  uint8 value, longvalue;
  int32 cos_angle, sin_angle;
  int32 xscalefactor, yscalefactor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (4 * cos_angle * xscalefactor);
  temp4 = (sin_angle * yscalefactor);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = (((cos_angle * ((X0 - 4 - Xoffset) >> 2) + sin_angle * (j - motion->yPosition) + 8192) << 2) * xscalefactor);
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 4 - Xoffset) >> 2) + 8192) * yscalefactor);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 2) << 2;
      status = 1;

      value = (uint8) ((((uint32) pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_421101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_421101(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 i, j, status;
  int32 Xposition, Yposition;
  uint8 value, longvalue;
  int32 cos_angle, sin_angle;
  int32 xscalefactor, yscalefactor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (4 * cos_angle * xscalefactor);
  temp4 = (sin_angle * yscalefactor);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = (((cos_angle * ((X0 - 4 - Xoffset) >> 2) + sin_angle * (j - motion->yPosition) + 8192) << 2) * xscalefactor);
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 4 - Xoffset) >> 2) + 8192) * yscalefactor);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 2) << 2;
      Yposition = type_local->sizeY - 1 - Yposition;
      status = 1;

      value = (uint8) ((((uint32) pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_421110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_421110(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, 
                        int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 i, j, status;
  int32 Xposition, Yposition;
  uint8 value, longvalue;
  int32 cos_angle, sin_angle;
  int32 xscalefactor, yscalefactor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (4 * cos_angle * xscalefactor);
  temp4 = (sin_angle * yscalefactor);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = (((cos_angle * ((X0 - 4 - Xoffset) >> 2) + sin_angle * (j - motion->yPosition) + 8192) << 2) * xscalefactor);
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 4 - Xoffset) >> 2) + 8192) * yscalefactor);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
      {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 2) << 2;
      Xposition = Object_Xsize - 1 - Xposition - 3;
      status = 1;

      value = (uint8) ((((uint32) pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));
      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_421111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 fill_image_421111(uint8 *pObjectBuf, 
                        uint8 *pOutBuf, 
                        int32 Frame_Xsize, 
                        int32 Object_Xsize,
                        int32 X0, int32 X2, 
                        int32 Y0, 
                        int32 Y2,
                        int32 Xoffset,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 i, j, status;
  int32 Xposition, Yposition;
  uint8 value, longvalue;
  int32 cos_angle, sin_angle;
  int32 xscalefactor, yscalefactor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (4 * cos_angle * xscalefactor);
  temp4 = (sin_angle * yscalefactor);
  temp1 = (Y0 - 1) * (Frame_Xsize >> 3);
  for (j = Y0; j <= Y2; j++)
  {
    status = 0;
    temp = (((cos_angle * ((X0 - 4 - Xoffset) >> 2) + sin_angle * (j - motion->yPosition) + 8192) << 2) * xscalefactor);
    temp2 = ((cos_angle * (j - motion->yPosition) - sin_angle * ((X0 - 4 - Xoffset) >> 2) + 8192) * yscalefactor);
    temp1 += (Frame_Xsize >> 3);
    for (i = X0; i <= X2; i = i + 4)
    {
      temp += temp3;
      temp2 -= temp4;
      Xposition = temp >> 22;
      Yposition = temp2 >> 22;

      if ((uint32) Xposition >= Object_Xsize || (uint32) Yposition >= type_local->sizeY)
     {
        if ( status == 1)
          break;
        else
          continue;
      }

      Xposition = (Xposition >> 2) << 2;
      Xposition = Object_Xsize - 4 - Xposition;
      Yposition = (int32) type_local->sizeY - 1 - Yposition;
      status = 1;

      value = (uint8) ((((uint32) pObjectBuf[Yposition * (Object_Xsize >> 3) + (Xposition >> 3)]
                         << (24 + (Xposition & 7))) >> 28));

      if (value == type_local->TransparencyColor)
        continue;

      longvalue = pOutBuf[temp1 + (i >> 3)];
      longvalue &= ((0x0F << (i & 7)));
      longvalue |= (value << (4 - (i & 7))); 

      pOutBuf[temp1 + (i >> 3)] = longvalue;
    }
  }
  return(0);
}



/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_820000

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_820000_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                           int32 temp, int32 temp1, int32 temp2, int32 temp3, 
                           int32 temp4, int32 temp5, int32 temp6,
                           int32 coord_frame_left, int32 coord_frame_right,
                           int32 type_local_TransparencyColor)
{
  int32 x_frame;
  int32 src;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    src = pObjectbuf[temp + x_frame];
    if (src != type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_800000_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                           int32 temp, int32 temp1, int32 temp2, int32 temp3, 
                           int32 temp4, int32 temp5, int32 temp6,
                           int32 coord_frame_left, int32 coord_frame_right,
                           int32 type_local_TransparencyColor)
{
  memcpy(pFramebuf + temp1 + coord_frame_left, pObjectbuf + temp + coord_frame_left, coord_frame_right - coord_frame_left + 1);
  return;
}

int32 fill_image_890000(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1;
  int32 y_frame;

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp = (coord_frame->top - 1 - motion->yPosition) * type_local->sizeX - motion->xPosition;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp += type_local->sizeX;
    temp1 += Xframe_size;
    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, 0, 0, 0, 0, 0,
                          coord_frame->left, coord_frame->right,
                          type_local->TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_821000

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_821000_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                           int32 temp, int32 temp1, int32 temp2, int32 xscalefactor,
                           int32 temp4, int32 temp5, int32 temp6,
                           int32 coord_frame_left, int32 coord_frame_right,
                           int32 type_local_TransparencyColor)
{
  int32 x_frame;
  int32 src;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp2 += xscalefactor;
    src = pObjectbuf[temp + (temp2 >> 8)];
    if (src != type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_801000_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                           int32 temp, int32 temp1, int32 temp2, int32 xscalefactor,
                           int32 temp4, int32 temp5, int32 temp6,
                           int32 coord_frame_left, int32 coord_frame_right,
                           int32 type_local_TransparencyColor)
{
  int32 x_frame;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp2 += xscalefactor;
    pFramebuf[temp1 + x_frame] = pObjectbuf[temp + (temp2 >> 8)];
  }
  return;
}

int32 fill_image_891000(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3;
  int32 y_frame;
  int32 xscalefactor, yscalefactor;

  /* added */
  uint32 coord_frame_right, type_local_TransparencyColor;
  coord_frame_right = coord_frame->right;
  type_local_TransparencyColor = type_local->TransparencyColor;
  /* */

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp3 = (coord_frame->left - 1 - motion->xPosition) * xscalefactor;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = (((y_frame - motion->yPosition) * yscalefactor) >> 8) * type_local->sizeX;
    temp2 = temp3;
    temp1 += Xframe_size;

    fill_image_899999_sub(pObjectbuf, pFramebuf,
      temp, temp1, temp2, xscalefactor, 0, 0, 0,
      coord_frame->left, coord_frame_right, type_local_TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_820100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_820100_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                           int32 temp, int32 temp1, int32 temp2,
                           uint32 type_local_sizeX, int32  type_local_sizeY,
                           int32 cos_angle, int32 sin_angle,
                           uint32 coord_frame_left, uint32 coord_frame_right,
                           uint32 type_local_TransparencyColor)
{    
  int32 src;
  int32 x_frame;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += cos_angle;
    temp2 -= sin_angle;
    
    if ((uint32) temp >= (type_local_sizeX << 14) || (uint32) temp2 >= (type_local_sizeY << 14))
      continue;
    
    src = pObjectbuf[(temp2 >> 14) * type_local_sizeX + (temp >> 14)];
    if (src != (int32) type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_800100_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                           int32 temp, int32 temp1, int32 temp2,
                           uint32 type_local_sizeX, int32  type_local_sizeY,
                           int32 cos_angle, int32 sin_angle,
                           uint32 coord_frame_left, uint32 coord_frame_right,
                           uint32 type_local_TransparencyColor)
{    
  int32 x_frame;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += cos_angle;
    temp2 -= sin_angle;
    
    if ((uint32) temp >= (type_local_sizeX << 14) || (uint32) temp2 >= (type_local_sizeY << 14))
      continue;
   
    pFramebuf[temp1 + x_frame] = pObjectbuf[(temp2 >> 14) * type_local_sizeX + (temp >> 14)];
  }
  return;
}

int32 fill_image_890100(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 y_frame;

  int32 cos_angle, sin_angle;

  uint32 type_local_sizeX, type_local_sizeY, coord_frame_right, type_local_TransparencyColor;
  type_local_sizeX = type_local->sizeX;
  type_local_sizeY = type_local->sizeY;
  coord_frame_right = coord_frame->right;
  type_local_TransparencyColor = type_local->TransparencyColor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp3 = (8192 + cos_angle * (coord_frame->left - 1 - motion->xPosition) - sin_angle * motion->yPosition);
  temp4 = (8192 - cos_angle * motion->yPosition - sin_angle * (coord_frame->left - 1 - motion->xPosition));

  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = (temp3 + sin_angle * y_frame);
    temp2 = (temp4 + cos_angle * y_frame);
    temp1 += Xframe_size;

    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2,
                          (int32) type_local_sizeX, (int32) type_local_sizeY,
                          cos_angle, sin_angle,
                          coord_frame->left, coord_frame_right,
                          type_local_TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_821100

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_821100_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3, int32 temp4,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          uint32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 src, x_frame;    
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += temp3;
    temp2 -= temp4;
    
    if ((uint32) temp >= (type_local_sizeX << 22)|| (uint32) temp2 >= (type_local_sizeY << 22))
      continue;
    src = pObjectbuf[(temp2 >> 22) * type_local_sizeX + (temp >> 22)];
    if (src != (int32) type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_801100_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3, int32 temp4,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          uint32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 x_frame;    
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += temp3;
    temp2 -= temp4;
    
    if ((uint32) temp >= (type_local_sizeX << 22)|| (uint32) temp2 >= (type_local_sizeY << 22))
      continue;
    pFramebuf[temp1 + x_frame] = pObjectbuf[(temp2 >> 22) * type_local_sizeX + (temp >> 22)];
  }
  return;
}

int32 fill_image_891100(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 y_frame;
  int32 cos_angle, sin_angle;
  int32 xscalefactor, yscalefactor;

  uint32 type_local_sizeX, type_local_sizeY, coord_frame_right, type_local_TransparencyColor;
  type_local_sizeX = type_local->sizeX;
  type_local_sizeY = type_local->sizeY;
  coord_frame_right = coord_frame->right;
  type_local_TransparencyColor = type_local->TransparencyColor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp3 = cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = ((cos_angle * (coord_frame->left - 1 - motion->xPosition) + sin_angle * (y_frame - motion->yPosition) + 8192) ) * xscalefactor;
    temp2 = ((cos_angle * (y_frame - motion->yPosition) - sin_angle * (coord_frame->left - 1 - motion->xPosition) + 8192) ) * yscalefactor;
    temp1 += Xframe_size;

    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2, temp3, temp4,
                          (int32) type_local_sizeX, (int32) type_local_sizeY,
                          coord_frame->left, coord_frame_right,
                          type_local_TransparencyColor);

  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_820001

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_820001_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2,int32 temp3,
                          int32 temp4,int32 temp5,int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 src, x_frame;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    src = pObjectbuf[temp + x_frame];
    if (src != type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_800001_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2,int32 temp3,
                          int32 temp4,int32 temp5,int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  memcpy(pFramebuf + temp1 + coord_frame_left, pObjectbuf + temp + coord_frame_left, coord_frame_right - coord_frame_left + 1);
  return;
}

int32 fill_image_890001(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1;
  int32 y_frame;

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp = (type_local->sizeY - coord_frame->top + motion->yPosition) * type_local->sizeX - motion->xPosition;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp -= type_local->sizeX;
    temp1 += Xframe_size;

    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, 0, 0, 0, 0, 0,
                          coord_frame->left, coord_frame->right,
                          type_local->TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_821001

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_821001_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 xscalefactor,
                          int32 temp4, int32 temp5, int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 src;
  int32 x_frame;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp2 += xscalefactor;
    src = pObjectbuf[temp + (temp2 >> 8)];
    if (src != type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_801001_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 xscalefactor,
                          int32 temp4, int32 temp5, int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 x_frame;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp2 += xscalefactor;
    pFramebuf[temp1 + x_frame] = pObjectbuf[temp + (temp2 >> 8)];
  }
  return;
}

int32 fill_image_891001(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3;
  int32 y_frame;

  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (coord_frame->left - 1 - motion->xPosition) * xscalefactor;
  temp1 = (coord_frame->top - 1) * Xframe_size;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = ((int32) type_local->sizeY - 1 - (((y_frame - motion->yPosition) * yscalefactor) >> 8))
      * (int32) type_local->sizeX;
    temp2 = temp3;
    temp1 += Xframe_size;
    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2, xscalefactor, 0, 0, 0,
                          coord_frame->left, coord_frame->right,
                          type_local->TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_820101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_820101_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 cos_angle, int32 sin_angle,
                          uint32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 src, x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += cos_angle;
    temp2 -= sin_angle;
    if ((uint32) temp >= (type_local_sizeX << 14) || (uint32) temp2 >= (type_local_sizeY << 14))
      continue;
    src = pObjectbuf[(type_local_sizeY - 1 - (temp2 >> 14)) * type_local_sizeX + (temp >> 14)];
    if (src != (int32) type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_800101_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 cos_angle, int32 sin_angle,
                          uint32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += cos_angle;
    temp2 -= sin_angle;
    if ((uint32) temp >= (type_local_sizeX << 14) || (uint32) temp2 >= (type_local_sizeY << 14))
      continue;
    pFramebuf[temp1 + x_frame] = pObjectbuf[(type_local_sizeY - 1 - (temp2 >> 14)) * type_local_sizeX + (temp >> 14)];
  }
  return;
}

int32 fill_image_890101(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 y_frame;

  int32 cos_angle, sin_angle;

  uint32 type_local_sizeX, type_local_sizeY, coord_frame_left, coord_frame_right, type_local_TransparencyColor;
  type_local_sizeX = type_local->sizeX;
  type_local_sizeY = type_local->sizeY;
  coord_frame_left = coord_frame->left;
  coord_frame_right = coord_frame->right;
  type_local_TransparencyColor = type_local->TransparencyColor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp3 = (8192 + cos_angle * (coord_frame->left - 1 - motion->xPosition) - sin_angle * motion->yPosition);
  temp4 = (8192 - cos_angle * motion->yPosition - sin_angle * (coord_frame->left - 1 - motion->xPosition));

  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = (temp3 + sin_angle * y_frame);
    temp2 = (temp4 + cos_angle * y_frame);
    temp1 += Xframe_size;

    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2,
                          (int32) type_local_sizeX, (int32) type_local_sizeY,
                          cos_angle, sin_angle,
                          coord_frame_left, coord_frame_right,
                          type_local_TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_821101

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_821101_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3, int32 temp4,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          uint32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 src, x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += temp3;
    temp2 -= temp4;
    
    if ((uint32) temp >= (type_local_sizeX << 22) || (uint32) temp2 >= (type_local_sizeY << 22))
      continue;
    
    src = pObjectbuf[(type_local_sizeY - 1 - (temp2 >> 22)) * type_local_sizeX + (temp >> 22)];
    if (src != (int32) type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_801101_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3, int32 temp4,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          uint32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += temp3;
    temp2 -= temp4;
    
    if ((uint32) temp >= (type_local_sizeX << 22) || (uint32) temp2 >= (type_local_sizeY << 22))
      continue;
    pFramebuf[temp1 + x_frame] = pObjectbuf[(type_local_sizeY - 1 - (temp2 >> 22)) * type_local_sizeX + (temp >> 22)];
  }
  return;
}

int32 fill_image_891101(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 y_frame;
  int32 cos_angle, sin_angle;
  int32 xscalefactor, yscalefactor;
/* Modified*/
  uint32 type_local_sizeX, type_local_sizeY, coord_frame_right, type_local_TransparencyColor;
  type_local_sizeX = type_local->sizeX;
  type_local_sizeY = type_local->sizeY;
  coord_frame_right = coord_frame->right;
  type_local_TransparencyColor = type_local->TransparencyColor;
/* */

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp3 = cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = ((cos_angle * (coord_frame->left - 1 - motion->xPosition) + sin_angle * (y_frame - motion->yPosition) + 8192) * xscalefactor);
    temp2 = ((cos_angle * (y_frame - motion->yPosition) - sin_angle * (coord_frame->left - 1 - motion->xPosition) + 8192) * yscalefactor);
    temp1 += Xframe_size;

    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2, temp3, temp4,
                          (int32) type_local_sizeX, (int32) type_local_sizeY,
                          coord_frame->left, coord_frame_right,
                          type_local_TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_820010

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_820010_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3,
                          int32 temp4, int32 temp5, int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 src, x_frame;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    src = pObjectbuf[temp - x_frame];
    if (src != type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_800010_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3,
                          int32 temp4, int32 temp5, int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 x_frame;

  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
    pFramebuf[temp1 + x_frame] = pObjectbuf[temp - x_frame];
  return;
}

int32 fill_image_890010(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1;
  int32 y_frame;

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp = (coord_frame->top - motion->yPosition) * type_local->sizeX - 1 + motion->xPosition;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp1 += Xframe_size;
    temp += type_local->sizeX;
    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, 0, 0, 0, 0, 0,
                          coord_frame->left, coord_frame->right,
                          type_local->TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_821010

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_821010_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 xscalefactor,
                          int32 temp4, int32 temp5, int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 src, x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp2 += xscalefactor;
    src = pObjectbuf[temp - (temp2 >> 8)];
    if (src != type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_801010_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 xscalefactor,
                          int32 temp4, int32 temp5, int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp2 += xscalefactor;
    pFramebuf[temp1 + x_frame] = pObjectbuf[temp - (temp2 >> 8)];
  }
  return;
}

int32 fill_image_891010(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3;
  int32 y_frame;
  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp3 = (coord_frame->left - 1 - motion->xPosition) * xscalefactor;
  temp1 = (coord_frame->top - 1) * Xframe_size;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = (((y_frame - motion->yPosition) * yscalefactor) >> 8) * type_local->sizeX + type_local->sizeX - 1;
    temp2 = temp3;
    temp1 += Xframe_size;
    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2, xscalefactor, 0, 0, 0,
                          coord_frame->left, coord_frame->right,
                          type_local->TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_820110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_820110_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 cos_angle, int32 sin_angle,
                          uint32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 src, x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += cos_angle;
    temp2 -= sin_angle;
    
    if ((uint32) temp >= (type_local_sizeX << 14) || (uint32) temp2 >= (type_local_sizeY << 14))
      continue;
    
    src = pObjectbuf[(temp2 >> 14) * type_local_sizeX + (int32) type_local_sizeX - 1 - (temp >> 14)];
    if (src != (int32) type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_800110_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 cos_angle, int32 sin_angle,
                          uint32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += cos_angle;
    temp2 -= sin_angle;
    
    if ((uint32) temp >= (type_local_sizeX << 14) || (uint32) temp2 >= (type_local_sizeY << 14))
      continue;
    
    pFramebuf[temp1 + x_frame] = pObjectbuf[(temp2 >> 14) * type_local_sizeX + (int32) type_local_sizeX - 1 - (temp >> 14)];
  }
  return;
}

int32 fill_image_890110(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 y_frame;

  int32 cos_angle, sin_angle;

  uint32 type_local_sizeX, type_local_sizeY, coord_frame_right, type_local_TransparencyColor;
  type_local_sizeX = type_local->sizeX;
  type_local_sizeY = type_local->sizeY;
  coord_frame_right = coord_frame->right;
  type_local_TransparencyColor = type_local->TransparencyColor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp3 = (8192 + cos_angle * (coord_frame->left - 1 - motion->xPosition) - sin_angle * motion->yPosition);
  temp4 = (8192 - cos_angle * motion->yPosition - sin_angle * (coord_frame->left - 1 - motion->xPosition));

  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = (temp3 + sin_angle * y_frame);
    temp2 = (temp4 + cos_angle * y_frame);
    temp1 += Xframe_size;

    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2,
                          (int32) type_local_sizeX, (int32) type_local_sizeY,
                          cos_angle, sin_angle,
                          coord_frame->left, coord_frame_right,
                          type_local_TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_821110

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_821110_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3, int32 temp4,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 src, x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += temp3;
    temp2 -= temp4;
    
    if ((uint32) temp >= (type_local_sizeX << 22) || (uint32) temp2 >= (type_local_sizeY << 22))
      continue;
    
    src = pObjectbuf[(temp2 >> 22) * (int32) type_local_sizeX + (int32) type_local_sizeX - 1 - (temp >> 22)];
    if (src != (int32) type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_801110_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3, int32 temp4,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 coord_frame_left, uint32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += temp3;
    temp2 -= temp4;
    
    if ((uint32) temp >= (type_local_sizeX << 22) || (uint32) temp2 >= (type_local_sizeY << 22))
      continue;
    
    pFramebuf[temp1 + x_frame] = pObjectbuf[(temp2 >> 22) * (int32) type_local_sizeX + (int32) type_local_sizeX - 1 - (temp >> 22)];
  }
  return;
}

int32 fill_image_891110(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 y_frame;

  int32 cos_angle, sin_angle;
  int32 xscalefactor, yscalefactor;

  uint32 type_local_sizeX, type_local_sizeY, coord_frame_right, type_local_TransparencyColor;
  type_local_sizeX = type_local->sizeX;
  type_local_sizeY = type_local->sizeY;
  coord_frame_right = coord_frame->right;
  type_local_TransparencyColor = type_local->TransparencyColor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp3 = cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = ((cos_angle * (coord_frame->left - 1 - motion->xPosition) + sin_angle * (y_frame - motion->yPosition) + 8192) ) * xscalefactor;
    temp2 = ((cos_angle * (y_frame - motion->yPosition) - sin_angle * (coord_frame->left - 1 - motion->xPosition) + 8192) ) * yscalefactor;
    temp1 += Xframe_size;

    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2, temp3, temp4,
                          type_local_sizeX, type_local_sizeY,
                          coord_frame->left, coord_frame_right,
                          type_local_TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_820011

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_820011_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3,
                          int32 temp4, int32 temp5, int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 src, x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    src = pObjectbuf[temp - x_frame];
    if (src != type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_800011_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3,
                          int32 temp4, int32 temp5, int32 temp6,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
    pFramebuf[temp1 + x_frame] = pObjectbuf[temp - x_frame];
  return;
}

int32 fill_image_890011(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1;
  int32 y_frame;

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp = (type_local->sizeY + motion->yPosition - coord_frame->top + 1) * (int32) type_local->sizeX - 1 + motion->xPosition;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp -= (int32) type_local->sizeX;
    temp1 += Xframe_size;
    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, 0, 0, 0, 0, 0,
                          coord_frame->left, coord_frame->right,
                          type_local->TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_821011

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_821011_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp4, int32 xscalefactor,
                          int32 temp2, int32 temp3, int32 temp5,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 src, x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp4 += xscalefactor;
    src = pObjectbuf[temp - ((temp4) >> 8)];
    if (src != type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_801011_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp4, int32 xscalefactor,
                          int32 temp2, int32 temp3, int32 temp5,
                          int32 coord_frame_left, int32 coord_frame_right,
                          int32 type_local_TransparencyColor)
{
  int32 x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp4 += xscalefactor;
    pFramebuf[temp1 + x_frame] = pObjectbuf[temp - ((temp4) >> 8)];
  }
  return;
}

int32 fill_image_891011(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4, temp5;
  int32 y_frame;

  int32 xscalefactor, yscalefactor;

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp2 = (int32) type_local->sizeX * (int32) type_local->sizeY - 1;
  temp3 = (coord_frame->top - 1 - motion->yPosition) * yscalefactor;
  temp5 = (coord_frame->left - 1 - motion->xPosition) * xscalefactor;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp3 += yscalefactor;
    temp = temp2 - (int32) type_local->sizeX * (temp3 >> 8);
    temp1 += Xframe_size;
    temp4 = temp5;
    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp4, xscalefactor, 0, 0, 0,
                          coord_frame->left, coord_frame->right,
                          type_local->TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_820111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_820111_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 cos_angle, int32 sin_angle,
                          int32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 src, x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += cos_angle;
    temp2 -= sin_angle;
    
    if ((uint32) temp >= (type_local_sizeX << 14) || (uint32) temp2 >= (type_local_sizeY << 14))
      continue;
    
    src = pObjectbuf[(type_local_sizeY - (temp2 >> 14)) * (int32) type_local_sizeX - 1 - (temp >> 14)];
    if (src != (int32) type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_800111_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 cos_angle, int32 sin_angle,
                          int32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += cos_angle;
    temp2 -= sin_angle;
    
    if ((uint32) temp >= (type_local_sizeX << 14) || (uint32) temp2 >= (type_local_sizeY << 14))
      continue;
    
    pFramebuf[temp1 + x_frame] = pObjectbuf[(type_local_sizeY - (temp2 >> 14)) * (int32) type_local_sizeX - 1 - (temp >> 14)];
  }
  return;
}

int32 fill_image_890111(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 y_frame;

  int32 cos_angle, sin_angle;

  uint32 type_local_sizeX, type_local_sizeY, coord_frame_right, type_local_TransparencyColor;
  type_local_sizeX = type_local->sizeX;
  type_local_sizeY = type_local->sizeY;
  coord_frame_right = coord_frame->right;
  type_local_TransparencyColor = type_local->TransparencyColor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp3 = (8192 + cos_angle * (coord_frame->left - 1 - motion->xPosition) - sin_angle * motion->yPosition);
  temp4 = (8192 - cos_angle * motion->yPosition - sin_angle * (coord_frame->left - 1 - motion->xPosition));

  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = (temp3 + sin_angle * y_frame);
    temp2 = (temp4 + cos_angle * y_frame);
    temp1 += Xframe_size;
    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2,
                          type_local_sizeX, type_local_sizeY,
                          cos_angle, sin_angle,
                          coord_frame->left, coord_frame_right,
                          type_local_TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  fill_image_821111

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void fill_image_821111_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3, int32 temp4,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 src, x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += temp3;
    temp2 -= temp4;
    
    if ((uint32) temp >= (type_local_sizeX << 22) || (uint32) temp2 >= (type_local_sizeY << 22))
      continue;
    
    src = pObjectbuf[((int32) type_local_sizeY - (temp2 >> 22)) * (int32) type_local_sizeX - 1 - (temp >> 22)];
    if (src != (int32) type_local_TransparencyColor)
      pFramebuf[temp1 + x_frame] = src;
  }
  return;
}

void fill_image_801111_sub(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 temp, int32 temp1, int32 temp2, int32 temp3, int32 temp4,
                          uint32 type_local_sizeX, uint32 type_local_sizeY,
                          int32 coord_frame_left, uint32 coord_frame_right,
                          uint32 type_local_TransparencyColor)
{
  int32 x_frame;
  
  for (x_frame = coord_frame_left; x_frame <= coord_frame_right ; x_frame++)
  {
    temp += temp3;
    temp2 -= temp4;
    
    if ((uint32) temp >= (type_local_sizeX << 22) || (uint32) temp2 >= (type_local_sizeY << 22))
      continue;
    
    pFramebuf[temp1 + x_frame] = pObjectbuf[((int32) type_local_sizeY - (temp2 >> 22)) * (int32) type_local_sizeX - 1 - (temp >> 22)];
  }
  return;
}

int32 fill_image_891111(uint8 *pObjectbuf, 
                        uint8 *pFramebuf, 
                        int32 Xframe_size,
                        srect *coord_frame,
                        saf_typeMotionInfo *motion,
                        saf_typeInfo *type_local)
{
  int32 temp, temp1, temp2, temp3, temp4;
  int32 y_frame;

  int32 cos_angle, sin_angle;
  int32 xscalefactor, yscalefactor;

  uint32 type_local_sizeX, type_local_sizeY, coord_frame_right, type_local_TransparencyColor;
  type_local_sizeX = type_local->sizeX;
  type_local_sizeY = type_local->sizeY;
  coord_frame_right = coord_frame->right;
  type_local_TransparencyColor = type_local->TransparencyColor;

  cos_angle = icos[motion->rotation_angle];
  sin_angle = isin[motion->rotation_angle];

  xscalefactor = saf_scale_factor_calc_x(motion);
  yscalefactor = saf_scale_factor_calc_y(motion);

  temp1 = (coord_frame->top - 1) * Xframe_size;
  temp3 = cos_angle * xscalefactor;
  temp4 = sin_angle * yscalefactor;
  for ( y_frame = coord_frame->top; y_frame <= coord_frame->bottom; y_frame++)
  {
    temp = ((cos_angle * (coord_frame->left - 1 - motion->xPosition) + sin_angle * (y_frame - motion->yPosition) + 8192) * xscalefactor);
    temp2 = ((cos_angle * (y_frame - motion->yPosition) - sin_angle * (coord_frame->left - 1 - motion->xPosition) + 8192) * yscalefactor);
    temp1 += Xframe_size;
    fill_image_899999_sub(pObjectbuf, pFramebuf,
                          temp, temp1, temp2, temp3, temp4,
                          type_local_sizeX, type_local_sizeY,
                          coord_frame->left, coord_frame_right,
                          type_local_TransparencyColor);
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_rotation_scale_mirror_8bit

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_rotation_scale_mirror_8bit(uint8 *pObjectbuf, 
                                     uint8 *pFramebuf,
                                     int32 Xframe_size, 
                                     int32 Yframe_size,
                                     saf_typeMotionInfo *motion,
                                     saf_typeInfo *type_local,
                                     srect *coord_frame,
                                     int32 color_bit)
{
    if (motion->rightToLeft)
    {
      if (motion->upsideDown)
      {
        if (motion->rotation_angle !=0)
        {
          if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_801111_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_821111_sub;
            fill_image_891111(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
          else
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_800111_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_820111_sub;
            fill_image_890111(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
        }
        else // motion->rotation_angle == 0
        {
          if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_801011_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_821011_sub;
            fill_image_891011(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
          else // no scaling
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_800011_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_820011_sub;
            fill_image_890011(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
        }
      }
      else  // no upsideDown
      {
        if (motion->rotation_angle !=0)
        {
          if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_801110_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_821110_sub;
            fill_image_891110(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
          else // no scaling
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_800110_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_820110_sub;
            fill_image_890110(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
        }
        else // motion->rotation_angle == 0
        {
          if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_801010_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_821010_sub;
            fill_image_891010(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
          else // no scaling
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_800010_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_820010_sub;
            fill_image_890010(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
        }
      }
    }
    else // no right-to-left
    {
      if (motion->upsideDown)
      {
        if (motion->rotation_angle !=0)
        {
          if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_801101_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_821101_sub;
            fill_image_891101(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
          else // no scaling
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_800101_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_820101_sub;
            fill_image_890101(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
        }
        else // motion->rotation_angle == 0
        {
          if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_801001_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_821001_sub;
            fill_image_891001(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
          else // no scaling
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_800001_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_820001_sub;
            fill_image_890001(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
        }
      }
      else  // no upsideDown
      {
        if (motion->rotation_angle !=0)
        {
          if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_801100_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_821100_sub;
            fill_image_891100(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
          else // no scaling
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_800100_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_820100_sub;
            fill_image_890100(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
        }
        else // motion->rotation_angle == 0
        {
          if (motion->x_scale_type != 0 || motion->y_scale_type != 0)
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_801000_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_821000_sub;
            fill_image_891000(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
          else // no scaling
          {
            if(motion->transparency_type == 0)
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_800000_sub;
            else
              fill_image_899999_sub = (FILL_IMAGE_899999_SUB) fill_image_820000_sub;
            fill_image_890000(pObjectbuf, pFramebuf, Xframe_size, coord_frame, motion, type_local);
          }
        }
      }
    }
  return(0);   // success
}


/* <EJECT> */

/*===========================================================================

FUNCTION  saf_rotation_scale_mirror_4bit_transcolor

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_rotation_scale_mirror_4bit_transcolor(uint8 *pObjectBuf, 
                                                uint8 *pOutBuf,
                                                int32 Frame_Xsize, 
                                                int32 Frame_Ysize,
                                                saf_typeMotionInfo *motion,
                                                saf_typeInfo *type_local,
                                                srect *coord_frame,
                                                int32 color_bit)        
{
  int32 Object_Xsize = (int32) type_local->sizeX;
  int32 Xoffset = motion->xPosition;

  int32 X0, Y0, X2, Y2;

  Object_Xsize *= 4;
  Frame_Xsize *= 4;
  Xoffset *= 4;
  X0 = coord_frame->left * 4;
  Y0 = coord_frame->top;
  X2 = (coord_frame->right * 4) + 3;
  Y2 = coord_frame->bottom;

  if (motion->x_scale_type == 0 && motion->y_scale_type == 0)
  {
    if (!motion->rotation_angle)
    {
      if (!motion->rightToLeft)
      {
        if (!motion->upsideDown)
          fill_image_420000(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else // y flip case
          fill_image_420001(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
      else // X- mirror case
      {
        if (!motion->upsideDown)
          fill_image_420010(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else // y flip case
          fill_image_420011(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
    }
    else // rotation case, no scaling // now
    {
      if (!motion->rightToLeft)
      {
        if (!motion->upsideDown)
          fill_image_420100(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else // Yposition = type_local->sizeY - 1 - Yposition;
          fill_image_420101(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
      else // Xposition = Object_Xsize - 1 - Xposition - (color_bit - 1);
      {
        if (!motion->upsideDown)
          fill_image_420110(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else // Yposition = type_local->sizeY - 1 - Yposition;
          fill_image_420111(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
    }
  }
  else  // scaling
  {
    if (!motion->rotation_angle)
    {
      if (!motion->rightToLeft)
      {
        if (!motion->upsideDown)
          fill_image_421000(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else
          fill_image_421001(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
      else // X- mirror case  // Xposition = Object_Xsize - 1 - Xposition - (color_bit - 1);
      {
        if (!motion->upsideDown)
          fill_image_421010(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else
          fill_image_421011(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
    }
    else // rotation case
    {
      if (!motion->rightToLeft)
      {
        if (!motion->upsideDown)
          fill_image_421100(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else
          fill_image_421101(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
      else // X- mirror case
      {
        if (!motion->upsideDown)
          fill_image_421110(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else
          fill_image_421111(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
    }
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  saf_rotation_scale_mirror_124bit

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
int32 saf_rotation_scale_mirror_124bit(uint8 *pObjectBuf, 
                                       uint8 *pOutBuf,
                                       int32 Frame_Xsize, 
                                       int32 Frame_Ysize,
                                       saf_typeMotionInfo* motion,
                                       saf_typeInfo* type_local,
                                       srect *coord_frame,
                                       int32 color_bit)
{
  int32 Object_Xsize = (int32) type_local->sizeX;
  int32 Xoffset = motion->xPosition;

  int32 X0, Y0, X2, Y2;

  Object_Xsize *= color_bit;
  Frame_Xsize *= color_bit;

  Xoffset *= color_bit;
  X0 = coord_frame->left * color_bit;
  Y0 = coord_frame->top;
  X2 = (coord_frame->right * color_bit) + (color_bit - 1);
  Y2 = coord_frame->bottom;
    
  if(color_bit != 1 && color_bit != 2 && color_bit != 4 && color_bit != 8)
    return(1);  // failure due to color_bit value

  if (motion->x_scale_type == 0 && motion->y_scale_type == 0)
  {
    if (!motion->rotation_angle)
    {
      if (!motion->rightToLeft)
      {
        if (!motion->upsideDown)
          fill_image_990000(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else
          fill_image_990001(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
      else // X- mirror case
      {
        if (!motion->upsideDown)
          fill_image_990010(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        else
          fill_image_990011(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
      }
    }
    else // rotation case, no scaling
    {
      if (!motion->rightToLeft)
      {
        if (!motion->upsideDown)
        {
          if (color_bit == 1)  // 190100
            fill_image_190100(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 290100
            fill_image_290100(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 490100
            fill_image_490100(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);

        }
        else
        {
          if (color_bit == 1)  // 190101
            fill_image_190101(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 290101
            fill_image_290101(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 490101
            fill_image_490101(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
      }
      else
      {
        if (!motion->upsideDown)
        {
          if (color_bit == 1)  // 190110
            fill_image_190110(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 290110
            fill_image_290110(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 490110
            fill_image_490110(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
        else
        {
          if (color_bit == 1)  // 190111
            fill_image_190111(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 290111
            fill_image_290111(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 490111
            fill_image_490111(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
      }
    }
  }
  else  // scaling
  {
    if (!motion->rotation_angle)
    {
      if (!motion->rightToLeft)
      {
        if (!motion->upsideDown)
        {
          if (color_bit == 1)  // 191000
            fill_image_191000(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 291000
            fill_image_291000(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 491000
            fill_image_491000(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
        else
        {
          if (color_bit == 1)  // 191001
            fill_image_191001(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 291001
            fill_image_291001(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 491001
            fill_image_491001(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
      }
      else
      {
        if (!motion->upsideDown)
        {
          if (color_bit == 1)  // 191010
            fill_image_191010(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 291010
            fill_image_291010(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 491010
            fill_image_491010(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
        else
        {
          if (color_bit == 1)  // 191011
            fill_image_191011(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 291011
            fill_image_291011(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 491011
            fill_image_491011(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
      }
    }
    else // rotation case
    {
      if (!motion->rightToLeft)
      {
        if (!motion->upsideDown)
        {
          if (color_bit == 1)  // 191100
            fill_image_191100(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 291100
            fill_image_291100(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 491100
            fill_image_491100(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
        else
        {
          if (color_bit == 1)  // 191101
            fill_image_191101(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 291101
            fill_image_291101(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 491101
            fill_image_491101(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
      }
      else // X- mirror case
      {
        if (!motion->upsideDown)
        {
          if (color_bit == 1)  // 191110
            fill_image_191110(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 291110
            fill_image_291110(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 491110
            fill_image_491110(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
        else
        {
          if (color_bit == 1)  // 191111
            fill_image_191111(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 2)  // 291111
            fill_image_291111(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
          else if (color_bit == 4) // 491111
            fill_image_491111(pObjectBuf, pOutBuf, Frame_Xsize, Object_Xsize, X0, X2, Y0, Y2, Xoffset, motion, type_local);
        }
      }
    }
  }
  return(0);
}
/* <EJECT> */
/*===========================================================================

FUNCTION  SAF_checksum_Checking

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  0 if checksum match (success)
  1 if checksum mismatch (failure)
  2 if checksum field does not exist.
SIDE EFFECTS
  None
===========================================================================*/
int32 SAF_checksum_Checking(uint8 *SAFdata, uint16 SAFSize)
{
  uint16 bufPos, sum;
  uint16 temp;
  temp = 0;
  
  
  if ((*(SAFdata + 8) == 0) && (*(SAFdata + 9) == 2)) 
  {
    return (2);
  }
  else 
  {
    if (SAFSize & 0x01)
    {
      sum = SAFdata[SAFSize - 1] << 8;
      SAFSize--;
    }
    else
      sum = 0;
    
    for (bufPos = 0; bufPos < SAFSize; bufPos++)
    {
      temp = SAFdata[bufPos++];
      temp = (temp << 8) | SAFdata[bufPos];
      sum += temp;
    }
    
    if (sum != 0)
      return(1);  // if fail
    
    return(0);    // if success
  }
}

#endif /* FEATURE_SAF */

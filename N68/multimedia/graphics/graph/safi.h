#ifndef SAFI_H
#define SAFI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    S I M P L E    A N I M A T I O N    F O R M A T  (SAF) I N T E R N A L  
                            H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the internal definitions necessary to support
  the SAF animation player.
  

Copyright(c) 2001, 2002 by NeoMtel Co., Ltd.   All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/safi.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/04    lg     Added SAF_STATE_GET_FILE_SIZE.
09/24/02    sa     Added support for MLZ decoder.  Defined enum saf_state_type.
07/17/01    sa     Added new macros and removed unused macros.  Added prototypes
                   for new functions.
03/20/01    sa     Did some clean up of the code.
03/20/01    sa     Removed MAX_TOTAL_TYPE_SIZE and MAX_SIS_GLOBAL_OBJECT_BUFFER
                   #defines.   Added two new parameters, ver and size, to 
                   saf_imageHeader structure.
03/11/01    sa     Initial version

===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Customer specific definitions           */
#include "cmx.h"                /* CMX specific definitions                */

#ifdef FEATURE_SAF

#include "saf.h"             /* Customer specific definitions           */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define SAF_INIT_VERSION                    1
#define SAF_MLZ_VERSION                     2

#define SIS_VER_BW_218                      0     /* SIS BW version 2.18   */
#define SIS_VER_BW_250                      1     /* SIS BW version 2.50   */
#define SIS_VER_4GRAY_100                   2     /* SIS 4gray version 1.00 
                                                               with wcoder */
#define SIS_VER_256_100                     3     /* SIS 256 COLOR ARITH   */
#define SIS_NUM_BLOCK                       7     /* sis number of block 
                                                       bits for sis BW 2.18*/
#define SIS_FRAMEDATA_DELAY_DEFAULT         3     /*sis frame default time */
#define SIS_FRAMEDATA_BACKCOLOR_DEFAULT     -1;    
#define SIS_TYPE_SPLIT_CRITERIA_DEFAULT     256*256
#define SIS_COMP_SCHEME_HUFF                0     /* sis compression type  */
#define SIS_COMP_SCHEME_WCODE               1     /* sis compression type  */
#define SIS_COMP_SCHEME_ARITH               2     /* sis compression type  */
#define SIS_IMAGE_TYPE_PNG                  0x10  /* saf image type png    */
#define SIS_IMAGE_TYPE_RLA                  0x00  /* sis rla entropy coder */
#define SIS_IMAGE_TYPE_MLZ                  0x01  /* sis mlz entropy coder */
#define SIS_IMAGE_LEVEL_BINARY              1     /* image's bit per pixel */
#define SIS_IMAGE_LEVEL_4GRAY               2     /* image's bit per pixel */
#define SIS_IMAGE_LEVEL_256COLOR            8     /* image's bit per pixel */
#define SIS_VER_256_100_PAL_SIZE            256   /* image's bit per pixel */
#define MAX_SIS_GLOBALPALETTETABLE          256*4 /* palette buffer size   */


#define SIS_TOTAL_HEADER_CMD                0x07
#define SIS_NORMAL_TYPE_CMD                 0x04
#define SIS_FRAME_CMD                       0x06
#define SIS_END_CMD                         0x08
#define SIS_GLOBAL_HEADER_CMD               0x01
#define SIS_GLOBAL_IMAGE_CMD                0x02
#define SIS_GLOBAL_FRAME_INFO_CMD           0x03
#define SIS_GLOBAL_TRANSPARENCY_CMD         0x09
#define SIS_GLOBAL_DELAY_INFO_CMD           0x0A
#define SIS_D_HUFF_TABLE_CMD                0x0B

#define SIS_REPLICA_TYPE_CMD                0x05
#define SIS_GLOBAL_HEADER_CMD_LENGTH        6
#define SIS_GLOBAL_IMAGE_CMD_LENGTH         2
#define SIS_MVDATA_SIZE                     7
#define SIS_NORMAL_TYPE_CMD_LENGTH          5
#define SIS_GLOBAL_SIZE_LOCATION            5
#define SIS_PREHEADER_LENGTH                7
#define SIS_TOTAL_HEADER_COMMAND_LENGTH     4

#define SIS_DEC_ERR_CMD_CODE_NOT_MATCH      -2
#define SIS_DYNAMIC_EFFECTS_ID_MV           0x00
#define SIS_DYNAMIC_EFFECTS_ID_LINE         0x01
#define SIS_DYNAMIC_EFFECTS_ID_ELLIPSE      0x02
#define SIS_DYNAMIC_EFFECTS_ID_RECT         0x03
#define SIS_DYNAMIC_EFFECTS_ID_POLY         0x04
#define SIS_DYNAMIC_EFFECTS_ID_LINE_CLIP    0x11
#define SIS_DYNAMIC_EFFECTS_ID_ELLIPSE_CLIP 0x12
#define SIS_DYNAMIC_EFFECTS_ID_RECT_CLIP    0x13
#define SIS_DYNAMIC_EFFECTS_ID_INVERSION    0x14

#define STREAMING_TYPE_CREATE_IMAGE_POOL    1
#define STREAMING_TYPE_CONSTRUCT_FRAMES     2

#define CHECK_1BIT(bufName, x, y, xSize)  ((int32)((((bufName)[((int32)(x) >> 3) + (y)*(xSize)]) & (0x80 >>  ((x)&0x07))) >> (7-((x)&0x07))))
#define SET_1BIT(bufName, x, y, xSize)    ((bufName)[((int)(x)>>3)+(y)*(xSize)] |= (0x80 >> ((x) & 0x07)))
#define RESET_1BIT(bufName, x, y, xSize)  ((bufName)[((int32)(x)>>3)+(y)*(xSize)] &= ~(0x80 >> ((x) & 0x07)))
#define CHECK_2BIT(bufName, x, y, xSize)  ((int32)((((bufName)[((int32)(x) >> 3) + (y)*(xSize)]) & (0xC0 >>  ((x)&0x07))) >> (6-((x)&0x07))))
#define SET_2BIT(bufName, x, y, xSize)    ((bufName)[((int)(x)>>3)+(y)*(xSize)] |= (0xC0 >> ((x) & 0x07)))
#define RESET_2BIT(bufName, x, y, xSize)  ((bufName)[((int32)(x)>>3)+(y)*(xSize)] &= ~(0xC0 >> ((x) & 0x07)))

#define W  (uint8)                          0
#define B  (uint8)                          1
#define INFINITE_LOOP                       1
#define WHITE  (boolean)                    0
#define BLACK  (boolean)                    1
#define MAX_DATA_BYTES                      1024
#define SAF_PREHEADER_SIZE                  7
#define SAF_TOTAL_HEADER_COMMAND_SIZE       5

typedef enum {
  SAF_STATE_INIT,
  SAF_STATE_GET_FILE_SIZE,
  SAF_STATE_GET_DATA,
  SAF_STATE_CHECK_PREHEADER,
  SAF_STATE_GET_NEXT_COMMAND,
  SAF_STATE_CHECK_COMMAND,
  SAF_STATE_PROCESS_TOTAL_HEADER,
  SAF_STATE_PROCESS_HEADERS,
  SAF_STATE_PROCESS_IMAGE,
  SAF_STATE_START_EMBEDDED_IMAGE,
  SAF_STATE_PROCESS_EMBEDDED_IMAGE,
  SAF_STATE_CONTINUE,
  SAF_STATE_PROCESS_PNG,
  SAF_STATE_PROCESS_RLA,
  SAF_STATE_PROCESS_MLZ,
  SAF_STATE_PROCESS_PNG_DONE,
  SAF_STATE_PROCESS_FRAME_INFO,
  SAF_STATE_PREPARE_ONE_FRAME,
  SAF_STATE_MAKE_ONE_FRAME,
  SAF_STATE_GET_UNKNOWN_DATA,
  SAF_STATE_D_HUFF_TABLE_CMD,
  SAF_STATE_END
} saf_state_type;

struct saf_imageHeader
{
  uint32  sizeX;
  uint32  sizeY;
  uint32  LoopCount;
  uint32  MvDataSize;
  uint32  paletteType;
  uint8*  cur_frame_pos;
  uint8*  init_frame_pos;
  int32   cur_frame_num;
  uint8   ImageLevel;
  uint8   CompressionScheme;
  uint8   numberOfFrames; 
  uint8   numberOfImageTypes; 
  uint8   Best;
  uint8   numberOfXBlocks;  
  uint8   numberOfYBlocks;  
  boolean InversionOrNot; 
  uint8   blockNumLength;  
  boolean MelodyIncluded; 
  uint8   numberOfLayer;  
  uint8   RepeatOrNot;
  uint8   NumReducedFrames;
  int32   BackGroundColor;
  uint32  variableDelayorNot;
  uint32  delay[MAX_FRAMES];
  uint32  UseGlobalTransparency;
  uint32  global_transparency_count;
  uint8   global_transparency[40];
  int32   ver, size;
};

typedef struct saf_imageHeader saf_imageHeaderType;

/* Structure used to retain information related to the ImageType           */
struct saf_typeInformation
{
  uint32   sizeX;
  uint32   sizeY;
  boolean  ModReplica;
  int32    typeID;
  int32    ParentTypeID;
  uint8    *imageData ;
  int32    imageDataSize;
  int32    stored; 
  boolean  DecompressOrNot;
  int32    TransparencyColor;
  int32    imageType;
};

typedef struct saf_typeInformation saf_typeInfo;

/* Structure used to retain information related to the ImageType's Motion  */
struct saf_typeMotionInformation
{
  uint8   TypeID;
  uint8   Layer;
  int32   xPosition;
  int32   yPosition;
  boolean upsideDown;
  boolean rightToLeft;
  boolean Rotation;
  uint32  transparency_type;
  uint8   x_scale_type;
  uint8   y_scale_type;
  uint8   x_scalefactor;
  uint8   y_scalefactor;
  uint8   scaling_algorithm;
  uint8   rotation_angle_flag;
  uint8   rotation_angle;
  int32   ClipFlag;
  uint32 clip_left, clip_right, clip_top, clip_bottom;
};

typedef struct saf_typeMotionInformation saf_typeMotionInfo;

/* Structure used to retain information related to the frames              */
struct saf_frameInformation
{
  boolean            InversionOrNot;
  saf_typeMotionInfo motionData;
  int32              NumofMovementData;
  int32              FrameBackGroundColor;
  uint32             pos;
  uint8              *DynamicDataPt;
};

typedef struct saf_frameInformation saf_frameInfo;

/* Structure used to retain information related to the position            */
typedef struct 
{
  int32 left; 
  int32 top;
  int32 right;
  int32 bottom;
}srect;

typedef int32 (*SIS_DEC_PASTE_IMAGE)(uint8* image,int32 typeWidth,
                                     srect *cliprect, saf_typeMotionInfo* mv,
                                     uint8* outbuf);
typedef int32 (*SIS_DEC_TRANS_DRAW_IMAGE)(uint8* image,int32 typeWidth,
                                          srect* cliprect, saf_typeMotionInfo* mv,
                                          uint8* TransColor,uint8* outbuf);
typedef int32 (*SIS_DEC_ORING_IMAGE)(uint8* image,int32 typeWidth, 
                                     srect *cliprect,saf_typeMotionInfo* mv,
                                     uint8* outbuf);
typedef void (*EDGEDETECTION)(uint8* inbuf, int32 yHeightOfImage, 
                              int32 xWidthOfImage, srect *coord_frame, srect *realRgn, 
                              uint8* outbuf, saf_typeMotionInfo *mv, saf_typeInfo *type);
typedef void (*CLIPIMAGE)(uint8* inbuf, uint8* outbuf, srect* typeClipRect, 
                          int32 width, int32 height, int32 Transparency);
typedef void (*SAF_CLIP_RECT)(uint8 *inputstream,
                              int32 x_pixel_num, int32 y_pixel_num,
                              srect* rect,
                              uint8 outer_region_color);
typedef int32(*SAF_RO_SC_MI)(uint8 *pObjectbuf, uint8 *pFramebuf,
                             int32 Xframe_size, int32 Yframe_size,
                             saf_typeMotionInfo* motion,
                             saf_typeInfo* type_local,
                             srect *coord_frame,
                             int32 color_bit);

int32 sis_dec_frame_cmd(uint8* inbuf,saf_frameInfo* frame);
int32 sis_dec_mv(uint8* inbuf,saf_typeMotionInfo* mv);
int32 sis_dec_get_normal_type_header(uint8 *inbuf,saf_typeInfo* typeArray);
int32 sis_dec_preheader(uint8* inbuf,int32 *ver,int32 *size);
int32 saf_func_save_frame_pos_and_size(int32 pos, int32 size);
int32 saf_func_check_total_header(uint8 *inbuf);
int32 sis_dec_process_header_group(uint8 *inbuf, int32 total_hdrsize);
int32 saf_func_get_global_info(saf_cmx_global_info_type* info);
int32 saf_func_get_frame_size(int32 frameid);
int32 saf_func_get_frame_pos(int32 frameid);
void saf_func_embedded_img_init(void);
void saf_func_embedded_img_done(void);

int32 saf_valid_region_check(int32 frameX, int32 frameY, 
                             saf_typeMotionInfo* a, srect* b, 
                             srect* d, saf_typeInfo* c);
int32 saf_rotation_scale_mirror_124bit(uint8 *pObjectBuf, uint8 *pFrameBuf,
                          int32 Frame_Xsize, int32 Frame_Ysize,
                          saf_typeMotionInfo* motion,
                          saf_typeInfo* type_local,
                          srect *coord_frame,
                          int32 color_bit);

int32 saf_rotation_scale_mirror_8bit(uint8 *pObjectbuf, uint8 *pFramebuf,
                          int32 Xframe_size, int32 Yframe_size,
                          saf_typeMotionInfo* motion,
                          saf_typeInfo* type_local,
                          srect *coord_frame,
                          int32 color_bit);
int32 saf_rotation_scale_mirror_4bit_transcolor(uint8 *pObjectBuf, uint8 *pOutBuf,
                          int32 Frame_Xsize, int32 Frame_Ysize,
                          saf_typeMotionInfo* motion,
                          saf_typeInfo* type_local,
                          srect *coord_frame,
                          int32 color_bit);
int32 sis_ro_sc_postprocess(uint8 *pFrameBuf,
                          int32 Frame_Xsize, int32 Frame_Ysize,
                          saf_typeMotionInfo *motion,
                          saf_typeInfo *type,
                          srect *coord_frame,
                          srect *realRgn);
int32 saf_clip_rectangular_8bpp(uint8 *inputstream,
                        int32 x_pixel_num, int32 y_pixel_num,
                        srect* rect,
                        uint8 outer_region_color);
int32 saf_clip_rectangular_124bpp(uint8 *inputstream,
                        int32 x_pixel_num, int32 y_pixel_num,
                        srect* rect,
                        uint8 outer_region_color,
                        int32 color_bit);
int32 saf_inversion(uint8* buf,int32 w,int32 h);

void EdgeDetection_1bpp(uint8* inbuf, int32 yHeightOfImage, int32 xWidthOfImage, 
                        srect *coord_frame, srect *realRgn, uint8* outbuf, saf_typeMotionInfo *mv, saf_typeInfo *type);

void EdgeDetection_2bpp(uint8* inbuf, int32 yHeightOfImage, int32 xWidthOfImage, 
                        srect *coord_frame, srect *realRgn, uint8* outbuf, saf_typeMotionInfo *mv, saf_typeInfo *type);

void saf_edge_detection_prepare_1bit(uint8 *inbuf, uint8 *outbuf,
                                int32 frame_width, int32 frame_height,
                                saf_typeMotionInfo *motion,
                                saf_typeInfo *type,
                                int32 ImageLevel, srect *validRgn, srect *realRgn);
void saf_edge_detection_prepare_2bit(uint8 *inbuf, uint8 *outbuf,
                                int32 frame_width, int32 frame_height,
                                saf_typeMotionInfo *motion,
                                saf_typeInfo *type,
                                int32 ImageLevel, srect *validRgn, srect *realRgn);

int32 find_original_point_is_internal_1bit(saf_typeMotionInfo *motion, 
                                           saf_typeInfo *type,
                                           int32 i_motion, 
                                           int32 j_motion);
int32 find_original_point_is_internal_2bit(saf_typeMotionInfo *motion, 
                                           saf_typeInfo *type,
                                           int32 i_motion, 
                                           int32 j_motion);


int32 saf_scale_factor_calc_x(saf_typeMotionInfo* motion);
int32 saf_scale_factor_calc_y(saf_typeMotionInfo* motion);

extern void saf_decode_init(void);
extern int32 check_saf_total_header(uint8*);
extern int32 process_saf_headers(uint8*);
extern int32 check_images(uint8*);
extern int32 save_frame_pos_and_size(int32 pos, int32 size);
extern void process_saf(void*);
extern void process_one_frame(uint32 frameid, uint8* framebuf, void *cb_data);

#endif /* FEATURE_SAF */
#endif /* SAFI_H      */

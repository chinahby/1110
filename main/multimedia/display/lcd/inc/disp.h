#ifndef DISP_H
#define DISP_H


/*============================================================================

                   Display Logical Device Driver Header File

   DESCRIPTION
     This file contains the definitions needed for the display logical
     device driver (LDD).

   Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $PVCSPath:  L:/src/asw/MSM6050/vcs/disp.h_v   1.3   10 Apr 2002 11:50:14   dlee  $
 $Header: //source/qcom/qct/multimedia/display/mdp1/lcd_qsc1100/main/latest/inc/disp.h#1 $ $DateTime: 2008/05/06 14:29:11 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/06/07  dw     Add support for Epson Optrex QcifP EBI2 display panel.
 01/23/07  cl     Added support for display under error fatal
 01/19/07  dw     Added support for retrieving backlight level
 07/25/05  trg    Auto detect MDDI/EBI LCD panel
 11/08/04   tb    Changes to support multple MDDI clients.
 10/29/04  trc    Bump up EFFA backlight brightness levels
 08/13/04  trc    Added support for EFFA
 06/07/04  ap     Added FFA support.
 05/21/04  cjr    Added MDP/MDDI support
 05/02/04  cjr    Added dual LCD support
 04/22/04  cjr    Added drvLib support
 07/16/03  dle    Added disp_capture16 and disp_capture32.
 06/23/03  dle    Added support for DISP_DEVICE_18BPP.
 02/27/03  rtp    Added disp_off() and disp_on() functions to turn off
                  and turn on display respectively
 10/24/02  dle    Made disp_crit_sect global so can be shared by GRP
                  device driver.
 07/23/02  dle    Replaced disp_copy with disp_copy16, disp_copy32
 07/16/02  dle    Added disp_copy().
 04/25/02  dle    Added memset16.
 04/10/00  dle    Added _clear_screen() and _clear_screen_area().
 03/15/02  lz     Removed gs.h from list of includes and any references to
                  it in the code.
 03/12/02  dl     Initial release.

============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "disp_options.h"
#include "rex.h"
#include "msm.h"

// physical lcd type
typedef enum
{
#ifndef CUST_EDITION
  TMD_QVGA,              /* EBI2 TMD QVGA LCD */
  EPSON_QCIF,            /* EBI2 EPSON QCIF LCD */
  EPSON_QVGA,            /* EBI2 EPSON QVGA LCD */
  EP_OP_QCIFP_PRI,       /* EBI2 LCD_CS_N Epson+Optrex QCIF+ LCD */
  EP_OP_QCIFP_SEC,       /* EBI2 GP2_CS_N Epson+Optrex QCIF+ LCD */
  MDDI_LCD_PRIM,         /* MDDI Prim LCD */
  MDDI_LCD_SECD,         /* MDDI Secd LCD */
  TM_SCTN128,
  ZGD_TFT128X160,
  ZGD_TFT220X176,
#else
  MDP_LCD,
#endif
//do not change the order of the following enum types
//
  MEM_WR,                /* WR Memory, must be MAX_LCD_NUM - 1 */
  MAX_LCD_NUM,           /*  */
  LCD_NONE = 0xFFFF      /* No LCD */
}LCD_TYPE_PHYS;

//============================================================================
//                        CONSTANT DEFINITIONS
//============================================================================
/****************** LCD TYPE *******************/
/***** do not use enum for this ****************/
//#define LCD_NONE      0xFF
//#define TMD_QVGA      0
//#define EPSON_QCIF    1
//#define MDDI_LCD_PRIM 2
//#define MDDI_LCD_SECD 3
//#define MAX_LCD_NUM   4
//#define MEM_WR      5 /* back to memory write */
#define MDDI_MC4_PRIM MDDI_LCD_PRIM
#define MDDI_MC4_SECD MDDI_LCD_SECD

#ifdef T_FFA6550_SS
#error code not present
#else /* !T_FFA6550_SS */
#define LCD_BACKLIGHT_LVL_0 PM_LED_LCD_SETTING__LEVEL0
#define LCD_BACKLIGHT_LVL_1 PM_LED_LCD_SETTING__LEVEL1
#define LCD_BACKLIGHT_LVL_2 PM_LED_LCD_SETTING__LEVEL2
#define LCD_BACKLIGHT_LVL_3 PM_LED_LCD_SETTING__LEVEL3
#define LCD_BACKLIGHT_LVL_4 PM_LED_LCD_SETTING__LEVEL4
#endif /* !T_FFA6550_SS */

typedef unsigned int LCD_TYPE;
/***********************************************/

// logical lcd type
#define PRIMARY_LCD_TYPE      0      /* logical prim/main LCD */
#define SECONDARY_LCD_TYPE    1      /* logical secd LCD */
//#define MAX_LCD_NUM           2      /* maximum logical LCD device */  

#define MDDI_MC4_PRIM MDDI_LCD_PRIM
#define MDDI_MC4_SECD MDDI_LCD_SECD

/*
#ifdef T_FFA
  #ifdef T_FFA6550_H1
    #define PRIMARY_LCD_TYPE                EPSON_QCIF
    #define PRIMARY_LCD_WIDTH               176
    #define PRIMARY_LCD_HEIGHT              220
    #define PRIMARY_LCD_DIMENSION_WIDTH     0137
    #define PRIMARY_LCD_DIMENSION_HEIGHT    0172

    #define SECONDARY_LCD_TYPE              LCD_NONE
    #define SECONDARY_LCD_WIDTH             0
    #define SECONDARY_LCD_HEIGHT            0
    #define SECONDARY_LCD_DIMENSION_WIDTH   0
    #define SECONDARY_LCD_DIMENSION_HEIGHT  0
  #elif defined(T_FFA6550_H2)
    #define PRIMARY_LCD_TYPE                TMD_QVGA
    #define PRIMARY_LCD_WIDTH               240
    #define PRIMARY_LCD_HEIGHT              320
    #define PRIMARY_LCD_DIMENSION_WIDTH     0120
    #define PRIMARY_LCD_DIMENSION_HEIGHT    0160

    #define SECONDARY_LCD_TYPE              LCD_NONE
    #define SECONDARY_LCD_WIDTH             0
    #define SECONDARY_LCD_HEIGHT            0
    #define SECONDARY_LCD_DIMENSION_WIDTH   0
    #define SECONDARY_LCD_DIMENSION_HEIGHT  0
  #elif (defined(T_FFA6550_H3) || defined(T_FFA6800_H4)) && defined(FEATURE_MDDI)
    #define PRIMARY_LCD_TYPE                MDDI_LCD_PRIM
    #define PRIMARY_LCD_WIDTH               240
    #define PRIMARY_LCD_HEIGHT              320
    #define PRIMARY_LCD_DIMENSION_WIDTH     0175      // 1.75 inch 
    #define PRIMARY_LCD_DIMENSION_HEIGHT    0192     // 1.92 inch 

    #define SECONDARY_LCD_TYPE              LCD_NONE
    #define SECONDARY_LCD_WIDTH             0
    #define SECONDARY_LCD_HEIGHT            0
    #define SECONDARY_LCD_DIMENSION_WIDTH   0
    #define SECONDARY_LCD_DIMENSION_HEIGHT  0
  #elif defined(T_FFA6550_SS)
    // EFFA MDDI (via MC4) displays
    #define PRIMARY_LCD_TYPE                MDDI_LCD_PRIM
    #define PRIMARY_LCD_WIDTH               240
    #define PRIMARY_LCD_HEIGHT              320
    #define PRIMARY_LCD_DIMENSION_WIDTH     0175      // 1.75 inch 
    #define PRIMARY_LCD_DIMENSION_HEIGHT    0192     // 1.92 inch 

    #define SECONDARY_LCD_TYPE              MDDI_LCD_SECD
    #define SECONDARY_LCD_WIDTH             128
    #define SECONDARY_LCD_HEIGHT            96
    #define SECONDARY_LCD_DIMENSION_WIDTH   0088
    #define SECONDARY_LCD_DIMENSION_HEIGHT  0063
  #else
    #error FFA LCD needs to be defined
  #endif
#else*/ /* SURF */
/*#ifdef FEATURE_MDDI
  #define PRIMARY_LCD_TYPE                  MDDI_LCD_PRIM
  #define PRIMARY_LCD_WIDTH                 240
  #define PRIMARY_LCD_HEIGHT                320
  #define PRIMARY_LCD_DIMENSION_WIDTH       0175      // 1.75 inch 
  #define PRIMARY_LCD_DIMENSION_HEIGHT      0192     // 1.92 inch 

  #define SECONDARY_LCD_TYPE                MDDI_LCD_SECD
  #define SECONDARY_LCD_WIDTH               128
  #define SECONDARY_LCD_HEIGHT              96
  #define SECONDARY_LCD_DIMENSION_WIDTH     0088
  #define SECONDARY_LCD_DIMENSION_HEIGHT    0063
#else
  #define PRIMARY_LCD_TYPE                  TMD_QVGA
  #define PRIMARY_LCD_WIDTH                 240
  #define PRIMARY_LCD_HEIGHT                320
  #define PRIMARY_LCD_DIMENSION_WIDTH       0120
  #define PRIMARY_LCD_DIMENSION_HEIGHT      0160

  #define SECONDARY_LCD_TYPE                EPSON_QCIF
  #define SECONDARY_LCD_WIDTH               176
  #define SECONDARY_LCD_HEIGHT              220
  #define SECONDARY_LCD_DIMENSION_WIDTH     0137
  #define SECONDARY_LCD_DIMENSION_HEIGHT    0172
#endif
#endif*/ /* T_FFA */

#define PRIMARY_LCD_NAME  "/dev/lcd01"
#define SECONDARY_LCD_NAME  "/dev/lcd02"
#define MDP_DEV_NAME    "/dev/mdp"

#define MDDI_PRIM_LCD_DIMENSION_WIDTH       175      /* 1.75 inch */
#define MDDI_PRIM_LCD_DIMENSION_HEIGHT      192     /* 1.92 inch */
#define MDDI_SECD_LCD_DIMENSION_WIDTH       88
#define MDDI_SECD_LCD_DIMENSION_HEIGHT      63

#define TMD_QVGA_LCD_DIMENSION_WIDTH        120
#define TMD_QVGA_LCD_DIMENSION_HEIGHT       160

#define EPSON_QVGA_LCD_DIMENSION_WIDTH      120
#define EPSON_QVGA_LCD_DIMENSION_HEIGHT     160

#define EPSON_QCIF_LCD_DIMENSION_WIDTH      137
#define EPSON_QCIF_LCD_DIMENSION_HEIGHT     172


//============================================================================
//                        DATA DECLARATIONS
//============================================================================

/*--- Type Definitions --*/

/* Definition of bits per pixel */
typedef enum
{
  DISP_8BPP,
  DISP_16BPP,
  DISP_18BPP
} disp_bpp_type;

typedef struct
{
   word                    disp_width;
   word                    disp_height;
   disp_bpp_type           bpp;
   boolean                 palette_support;
   boolean                 contrast_support;
   byte                    contrast_min;
   byte                    contrast_max;
   byte                    contrast_default;
   boolean                 backlight_support;
   byte                    backlight_min;
   byte                    backlight_max;
   byte                    backlight_default;
   
   LCD_TYPE_PHYS           lcd_type;
   word                    phys_width;
   word                    phys_height;   
}  disp_info_type;


enum
{
  IOCTL_DISP_GET_INFO,
  IOCTL_DISP_POWERUP,
  IOCTL_DISP_UPDATE,
  IOCTL_DISP_UPDATE_UNDER_ERR_FATAL,
  IOCTL_DISP_SET_CONTRAST,
  IOCTL_DISP_SET_BACKLIGHT,
  IOCTL_DISP_GET_BACKLIGHT,
  IOCTL_DISP_CLEAR_WHOLE_SCREEN,
  IOCTL_DISP_CLEAR_SCREEN_AREA,
#ifdef FEATURE_HS_DISPLAY_CAPTURE
  IOCTL_DISP_ENABLE_CAPTURE_MODE,
  IOCTL_DISP_CAPTURE_SCREEN,
#endif
#if (defined(FEATURE_MP4_DECODER) || defined(FEATURE_CAMERA_NOFULLSCREEN)) //&& !defined(T_QSC1110)
  IOCTL_DISP_LOCK_SCR,
  IOCTL_DISP_UPDATE_LOCK_YUV,
  IOCTL_DISP_UPDATE_LOCK,
#endif
  IOCTL_DISP_OFF,
  IOCTL_DISP_ON
#ifdef FEATURE_DSP
  ,IOCTL_DISP_SET_WINDOWS
#endif
};

typedef struct
{
  void *buf_ptr;
  int16 src_width;
  int16 src_starting_row;
  int16 src_starting_column;
  int16 num_of_rows;
  int16 num_of_columns;
  int16 dst_starting_row;
  int16 dst_starting_column;
  LCD_TYPE dest;
} disp_update_type;


typedef struct
{
  word start_row;
  word end_row;
  word start_column;
  word end_column;
  LCD_TYPE dest;
} disp_cls_type;

#if (defined(FEATURE_MP4_DECODER) || defined(FEATURE_CAMERA_NOFULLSCREEN)) //&& !defined(T_QSC1110)
typedef struct
{
  word start_row;
  word num_row;
  word start_column;
  word num_column;
} disp_lock_type;
#endif
extern rex_crit_sect_type disp_crit_sect;

//============================================================================
//                        MACRO DEFINITIONS
//============================================================================

//============================================================================
//                        EXPORTED FUNCTIONS
//============================================================================

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

FUNCTION      DISP_INIT

DESCRIPTION
              This function initialize the display panel

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_init(void);
void disp_init2(void);


/*===========================================================================

FUNCTION      DISP_GET_INFO

DESCRIPTION
              This function returns display information

DEPENDENCIES
  None

RETURN VALUE
  Display information

SIDE EFFECTS
  None

===========================================================================*/

disp_info_type disp_get_info(void);
disp_info_type disp_get_info2(void);

/*===========================================================================

FUNCTION      DISP_POWERUP

DESCRIPTION
              This function powers up the device.

DEPENDENCIES
  None

RETURN VALUE
  Display buffer address

SIDE EFFECTS
  None

===========================================================================*/

void disp_powerup(void);
void disp_powerup2(void);

/*===========================================================================

FUNCTION      DISP_POWERDOWN

DESCRIPTION
              This function powers down the device.

DEPENDENCIES
  None

RETURN VALUE
  Display buffer address

SIDE EFFECTS
  None

===========================================================================*/

void disp_powerdown(void);
void disp_powerdown2(void);


/*===========================================================================

FUNCTION      DISP_UPDATE

DESCRIPTION
              This function update the display screen from the off screen
              buffer.

              For execution efficiency, the buffer pointer must be at
              4-byte boundary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_update
(
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Source image width */
  int16 src_width,
  /* Source rectangle starting row */
  int16 src_starting_row,
  /* Source rectangle starting column */
  int16 src_starting_column,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns,
  /* Device rectangle starting row */
  int16 dst_starting_row,
  /* Device rectangle starting column */
  int16 dst_starting_column
);

void disp_update2
(
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Source image width */
  int16 src_width,
  /* Source rectangle starting row */
  int16 src_starting_row,
  /* Source rectangle starting column */
  int16 src_starting_column,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns,
  /* Device rectangle starting row */
  int16 dst_starting_row,
  /* Device rectangle starting column */
  int16 dst_starting_column
);

/*===========================================================================

FUNCTION      DISP_SET_CONTRAST

DESCRIPTION
              This function sets the contrast of the display device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_set_contrast(
  word contrast /* Contrast value to set */
);

void disp_set_contrast2(
  word contrast /* Contrast value to set */
);

/*===========================================================================

FUNCTION      DISP_SET_DISPLAY_AREA

DESCRIPTION
              This function sets the display area.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_set_display_area(
  word start_row,
  word end_row
);

void disp_set_display_area2(
  word start_row,
  word end_row
);

/*===========================================================================

FUNCTION      DISP_SET_BACKLIGHT

DESCRIPTION
              This function sets the backlight level.
              If level is 0, backlight is turned off.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_set_backlight(byte level);
void disp_set_backlight2(byte level);

/*===========================================================================

FUNCTION      DISP_CLEAR_WHOLE_SCREEN

DESCRIPTION
              This function clears the whole screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_clear_whole_screen(void);
void disp_clear_whole_screen2(void);

/*===========================================================================

FUNCTION      DISP_CLEAR_SCREEN_AREA

DESCRIPTION
              This function clears an area of the screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_clear_screen_area
(
  word start_row,
  word end_row,
  word start_column,
  word end_column
);

void disp_clear_screen_area2
(
  word start_row,
  word end_row,
  word start_column,
  word end_column
);


/*===========================================================================

FUNCTION      DISP_OFF

DESCRIPTION
              This function turns off the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_off(void);
void disp_off2(void);

/*===========================================================================

FUNCTION      DISP_ON

DESCRIPTION
              This function turns on the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_on(void);
void disp_on2(void);

/*===========================================================================

FUNCTION      DISP_COPY16

DESCRIPTION
              This function copies the bitmap block to LCD device

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_copy16
(
  void  *src_ptr,
  void  *dst_ptr,
  dword count
);

/*===========================================================================

FUNCTION      DISP_COPY32

DESCRIPTION
              This function copies the bitmap block to LCD device

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_copy32
(
  void  *src_ptr,
  void  *dst_ptr,
  dword count
);

/*===========================================================================

FUNCTION      MEMSET16

DESCRIPTION
              This function sets 16-bit word string

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void memset16
(
  void  *ptr,
  dword value,
  dword count
);

/*===========================================================================

FUNCTION      DISP_CAPTURE16

DESCRIPTION
              This function copies from LCD device buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_capture16
(
  void  *src_ptr,
  void  *dst_ptr,
  dword count
);

/*===========================================================================

FUNCTION      DISP_CAPTURE32

DESCRIPTION
              This function copies from LCD device buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void disp_capture32
(
  void  *src_ptr,
  void  *dst_ptr,
  dword count
);


#ifdef FEATURE_TORCH_SUPPORT
void disp_set_torch(void);
void disp_clear_torch(void);
#endif
#if (defined(FEATURE_MP4_DECODER) || defined(FEATURE_CAMERA_NOFULLSCREEN)) //&& !defined(T_QSC1110)
void disp_update_lock
(
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns
);
void disp_update_yuv420
(
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns
);
void disp_lock_screen
(
  word start_row,
  word num_row,
  word start_column,
  word num_column
);
#endif

#ifdef __cplusplus
}
#endif

#endif    /* DISP_H */


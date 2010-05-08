/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Implements the GAPI to display CMX

GENERAL DESCRIPTION
    This file contains the implementation of the GAPI interface
    to display CMX on the display hardware of Zeus.

EXTERNALIZED FUNCTIONS
   cmx_disp_pic_control()
   cmx_disp_saf_header()
   cmx_disp_pic_data()

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None

  Copyright (c) 2000-2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                       EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 $Header: //depot/asic/msmshared/services/hs/cmx_disp.c#21 $DateTime: $Author:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/08   sg      Decouple LCD driver for multiprocessor targets
06/10/08   sg      Fixed Klocwork/lint warnings
04/22/08   pp      added fixes for Klocwork errors
02/06/08   dk      Adding disp2d.h as a part of HALMDP activity
01/23/08   dk      Removed inclusion of OEMCommondisp.h with BREW cleanup
08/13/06   act     Featurized file to FEATURE_WEBAUDIO
11/30/05   vph     Add FEATURE_PNG_DECODER for PNG APIs
11/17/04   ap      Use correct case for include of oemcommondisp
06/07/02   dle     Corrected offset calculation in display_jpeg_pic
05/16/02   lz      Put OEM include files after dmss include files and before
                   AEE include files.
04/10/02   dle     Get display info directly from device driver. Change to
                   use OFFSET_X and OFFSET_Y.

 *
 *    Rev 1.1   25 Sep 2001 17:34:56   stam
 * Modified code to use new #define names. Changes made by Nurit Y.
 *
 *    Rev 1.0   14 Aug 2001 11:36:22   karthick
 * Initial revision.
 *
 *    Rev 1.3   02 Jul 2001 16:59:46   karthick
 * Fixed compile error if F_SAF not defined. Code cleanup
 *
 *    Rev 1.2   10 Apr 2001 17:44:44   karthick
 * SAF palette is set properly. Fixed clear_pic_screen to clear display
 *
 *    Rev 1.1   05 Apr 2001 19:02:40   karthick
 * Fixed SAF co-ordinates
 *
 *    Rev 1.0   05 Apr 2001 18:29:40   karthick
 * Initial revision.
============================================================================*/

/*============================================================================
                       INCLUDE FILES FOR MODULE
============================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

#if defined(FEATURE_BITMAP) && !defined(FEATURE_UI_CORE_REMOVED) && \
    defined(FEATURE_WEBAUDIO)
#if !defined(FEATURE_MULTIPROCESSOR) || (defined(FEATURE_MULTIPROCESSOR) && defined(IMAGE_APPS_PROC))
  #include <string.h>    /* For memcpy */
  #include <ctype.h>     /* Character testing functions */
  #include <stdio.h>     /* Input formatting functions */
  #include "cmx_disp.h"  /* CMX display types */
  #include "cmx.h"
  #include "assert.h"

  #ifdef INC_DISP_H
    #include INC_DISP_H
  #else
    #include "disp.h"
  #endif

  #include "sys_palette.h"
  #include "AEEShell.h"

  #define OFFSET_X 0
  #define OFFSET_Y 80

/*--- Palette structure ---*/
typedef struct
{
  byte       red;    /* Red component in top five bits of byte   */
  byte       green;  /* Green component in top five bits of byte */
  byte       blue;   /* Blue component in top five bits of byte  */
} cmx_disp_palette_rgb_entry_type;

typedef union
{
  cmx_disp_palette_rgb_entry_type   rgb;    /* RGB palette entry        */
  byte                              grey;   /* Grey scale palette entry */
  dword                             entry;  /* Reserved                 */
} cmx_disp_palette_entry_type;

/* Image format type */
typedef enum
{
  CMX_DISP_IMAGE_FORMAT_RGB = 0,
  CMX_DISP_IMAGE_FORMAT_MAX = 0x7FFFFFFF
} cmx_disp_image_format_type;

typedef struct
{
  uint32                 max_palette_entries; /* Maximum palette entries */
  uint32                 pixel_depth;         /* Bits per pixel used for custom
                                                 palette */
  uint8                  dummy;
  uint8                 *palette_buf;         /* Pointer to custom palette buff
                                                 NULL if custom palette is not
                                                 present */
  cmx_disp_image_format_type image_format;    /* Type of image: e.g. RGB, ... */
  cmx_disp_rect_size_type   image_size;       /* Width and height dimension of
                                                 image */
  uint8                 *image;               /* Pointer to the image buffer */
} cmx_disp_image_type;


//============================================================================
//                        DATA DECLARATIONS
//============================================================================

/* Type to hold each incoming picture information */
typedef struct
{
  boolean          header_read; /* Picture header is read */
  boolean          transparency_read; /* Picture transparency is read */
  boolean          background_read;/* Read the background color */
  boolean          isBackground;   /* There is a background color in info */
  uint32           bkg_color; /* Background color for image if any - not
                                 used for now */
  boolean          isPalette; /* There is palette info in colormap */
  boolean          palette_read;   /* Colormap is read */
  boolean          isTransparency; /* There is transparency info in colormap */
  uint32           tran_nvalues;   /* Number of transparency values */
  cmx_disp_image_type       img;   /* Includes picture dimensions, palette information
                                      and image data */
  uint32           num_rows_received; /* How many rows of data have been
                                         received so far */
  cmx_disp_af_pos_enum_type  x_pos;   /* TopLeft, BottomR, Center, etc ... */
  cmx_disp_af_pos_enum_type  y_pos;   /* TopLeft, BottomR, Center, etc ... */
  cmx_disp_point_type        start_position; /* Image drawing start position */
  cmx_disp_rect_size_type    picture_size;   /* Total size available for drawing an image */
  uint8                      x_percent; /* Percentage offset from origin */
  uint8                      y_percent; /* Percentaga offset from origin */
  cmx_disp_point_type        current_position;    /* Where we are in the image */
} cmx_disp_picture_type;

/*============================================================================
              CONSTANTS, MACROS AND STRUCTURE TYPES FOR MODULE
============================================================================*/

/* Picutre Data */
static cmx_disp_picture_type picture;

#ifdef FEATURE_PNG_DECODER
  #define MAX_PNG_WIDTH           256
  #define MAX_PNG_HEIGHT          256
  #define MAX_PNG_SIZE            (MAX_PNG_WIDTH * MAX_PNG_HEIGHT)
  #define MAX_PALETTE_BUF_SIZE    1024

  static byte                     png_data[MAX_PNG_SIZE];
  ALIGN(4) static OEMPixel        png_data2[MAX_PNG_SIZE];
  static byte                     png_palette[MAX_PALETTE_BUF_SIZE];
  static cmx_disp_image_type      png_image;
#endif /* #ifdef FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
  #define MAX_JPEG_WIDTH          160
  #define MAX_JPEG_HEIGHT         160

  ALIGN(4) static OEMPixel        jpeg_data[MAX_JPEG_WIDTH * MAX_JPEG_HEIGHT];
  static cmx_disp_image_type      jpeg_image;
#endif

/*===========================================================================
              LOCAL FUNCTIONS PROTOTYPES
============================================================================*/

static void init_pic_params(cmx_disp_af_pos_enum_type            x_pos,
                            cmx_disp_af_pos_enum_type            y_pos,
                            uint8                                x_percent,
                            uint8                                y_percent,
                            cmx_disp_point_type                 *start_position,
                            cmx_disp_rect_size_type             *pic_size);

static void clear_pic_screen(cmx_disp_point_type      *start_position,
                             cmx_disp_rect_size_type  *pic_size);

#ifdef FEATURE_PNG_DECODER
  static void set_pic_start_position(void);

  static void display_png_pic(cmx_disp_af_pic_data_enum_type pic_data,
                              uint32                         num_bytes,
                              uint8                         *buffer);
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
  static void display_jpeg_pic(cmx_disp_af_pic_data_enum_type pic_data,
                               uint32                         num_bytes,
                               uint8                          *buffer);
#endif

#ifdef FEATURE_PNG_DECODER
  static void display_saf_pic(cmx_disp_af_pic_data_enum_type pic_data,
                              uint32                         num_bytes,
                              uint8                         *buffer);
#endif /* #ifdef FEATURE_PNG_DECODER */

static void display_bmp_pic(cmx_disp_af_pic_data_enum_type pic_data,
                            uint32                         num_bytes,
                            uint8                         *buffer);

#ifdef FEATURE_PNG_DECODER
static void convert_pic_8_to_native
(
  byte     *src_ptr,
  OEMPixel *dst_ptr,
  word      count
);
#endif /* #ifdef FEATURE_PNG_DECODER */

#if defined(FEATURE_SAF) || defined(FEATURE_PNG_DECODER)
static void set_palette(
  /* A pointer to a palette that is copied directly into the palette RAM in
     the Display Device */
  cmx_disp_palette_entry_type *palette
);
#endif /* #if defined(FEATURE_SAF) || defined(FEATURE_PNG_DECODER) */

static void set_system_palette(void);


/*---------------------------------------------------------------------------
 *            External Functions
 *--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION       CMX_DISP_PIC_CONTROL

DESCRIPTION
               This function takes picture control information about
               an image to follow.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmx_disp_pic_control(
  /* What to do: clear image, display image, initialize parameters, etc ... */
  cmx_disp_af_pic_status_enum_type     pic_ctl,
  /* X-coordinate of where to put the picture to follow, e.g top left,
     bottom right, center, or percent from origin */
  cmx_disp_af_pos_enum_type            x_pos,
  /* Y-coordinate of where to put the picture to follow, e.g top left,
     center, bottom right, or percentage from origin of display */
  cmx_disp_af_pos_enum_type            y_pos,
  /* Percentage offset of x-coordinate */
  uint8                                x_percent,
  /* Percentage offset of y-coordinate */
  uint8                                y_percent,
  /* Image drawing start position */
  cmx_disp_point_type                 *start_position,
  /* Total size available for drawing */
  cmx_disp_rect_size_type             *pic_size
)
{
  /* Determine the status of the picture and act accordingly */
  switch (pic_ctl)
  {
    case CMX_DISP_AF_PIC_STATUS_INIT:
      /* Init parameters for new picture to follow */
      init_pic_params(x_pos, y_pos, x_percent, y_percent,
                      start_position, pic_size);
      break;

    case CMX_DISP_AF_PIC_STATUS_CLEAR:
      /* First make sure to reset to the default system palette */
      set_system_palette();
      clear_pic_screen(start_position, pic_size);
      break;

    case CMX_DISP_AF_PIC_STATUS_ENABLE:
      break;

    case CMX_DISP_AF_PIC_STATUS_DISABLE:
      break;

    case CMX_DISP_AF_PIC_STATUS_ERROR:
      break;

    default:
     break;
  }

  return;
} /* End cmx_disp_pic_control */

#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION      CMX_DISP_SAF_HEADER

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmx_disp_saf_header(
  /* Data buffer */
  uint8                            *header,
  /* Number of bytes in this chunck of data */
  uint32                            num_bytes
)
{
  cmx_saf_info_type *saf_header = (cmx_saf_info_type *) header;
  byte *src_ptr, *dst_ptr;
  int i;

  ASSERT(((dword)saf_header & 0x3) == 0);

  /* -----------------------------------------------------------------------*/

  picture.header_read = TRUE;

  /* Width */
  //picture.picture_size.width  = saf_header->width;
  picture.picture_size.width  = 160;

  /* Height */
  //picture.picture_size.height = saf_header->height;
  picture.picture_size.height = 160;

  /* Pixel depth */
  picture.img.pixel_depth = saf_header->bitdepth;

  if (picture.img.pixel_depth == 8)
  {
    if (saf_header->palette && saf_header->num_palette_entry)
    {
      /* Set up the palette */
      src_ptr = saf_header->palette;
      dst_ptr = png_palette;
      for (i = 0; i < saf_header->num_palette_entry; i++) {
        /* Copy R,G,B and skip reserved byte */
        *dst_ptr++ = *src_ptr++;   /* R */
        *dst_ptr++ = *src_ptr++;   /* G */
        *dst_ptr++ = *src_ptr++;   /* B */
        dst_ptr++;             /* Skip transparency entry */
      }
      set_palette((cmx_disp_palette_entry_type *)png_palette);
    }
  }
  picture.img.palette_buf = NULL;
  picture.img.max_palette_entries = 0;

} /* cmx_disp_saf_header() */
#endif /* FEATURE_SAF */

/*===========================================================================

FUNCTION       CMX_DISP_PIC_DATA

DESCRIPTION
    This function aggregates picture data. When all the data has been received,
    passes the data on to a display function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmx_disp_pic_data(
  /* Flag to indicate whether the image is png or not */
  boolean                           ispng,
  /* Either append this chunck of data, or its the last chunck */
  cmx_disp_af_pic_data_enum_type    pic_data,
  /* Picture format, PNG, BMP, etc... */
  cmx_disp_af_pic_enum_type         pic_format,
  /* Number of bytes in this chunck of data */
  uint32                            num_bytes,
  /* Data buffer */
  uint8                            *buffer
)
{

  //clk_uptime_ms (decode_measure);

  switch (pic_format)
  {
#ifdef FEATURE_PNG_DECODER
    case CMX_DISP_AF_PIC_PNG:
      if (pic_data == CMX_DISP_AF_PIC_DATA_DONE )
      {
        if (png_image.pixel_depth == 8)
        {
          if (png_image.palette_buf && png_image.max_palette_entries)
          {
            set_palette((cmx_disp_palette_entry_type *)png_image.palette_buf);
          }

          if (png_image.image_size.width*png_image.image_size.height > MAX_PNG_SIZE)
          {
            png_image.image_size.height = MAX_PNG_SIZE/png_image.image_size.width;
          }
          convert_pic_8_to_native
          (
            png_image.image,
            png_data2,
            png_image.image_size.width*png_image.image_size.height
          );

          disp_update
          (
            png_data2,
            png_image.image_size.width,
            0,
            0,
            png_image.image_size.height,
            png_image.image_size.width,
            picture.start_position.y,
            picture.start_position.x
          );
        }
        else if (png_image.pixel_depth == 16)
        {
          disp_update
          (
            png_image.image,
            png_image.image_size.width,
            0,
            0,
            png_image.image_size.height,
            png_image.image_size.width,
            picture.start_position.y,
            picture.start_position.x
          );
        }
      }
      else
      {
        if ( ispng )
        {
          display_png_pic(pic_data, num_bytes, buffer);
        }
        else
        {
          display_saf_pic(pic_data, num_bytes, buffer);
        }
      }

      break;
#endif /* #ifdef FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
    case CMX_DISP_AF_PIC_JPEG:
      if (pic_data == CMX_DISP_AF_PIC_DATA_DONE)
      {
        if (jpeg_image.pixel_depth == 16)
        {
          disp_update
          (
            jpeg_image.image,
            jpeg_image.image_size.width,
            0,
            0,
            jpeg_image.image_size.height,
            jpeg_image.image_size.width,
            picture.start_position.y,
            picture.start_position.x
          );
        }
      }
      else
      {
        display_jpeg_pic(pic_data, num_bytes, buffer);
      }
      break;
#endif

    case CMX_DISP_AF_PIC_BMP:
      display_bmp_pic(pic_data, num_bytes, buffer);
      break;

    case CMX_DISP_AF_PIC_UNKNOWN:
      break;

    case CMX_DISP_AF_PIC_BIG:
      break;

    default:
      break;
  }

  return;
} /* End cmx_disp_pic_data */


/*---------------------------------------------------------------------------
 *            Local Functions
 *--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION      INIT_PICT_PARAMS

DESCRIPTION
              This function initializes the picture control parameters
              so that it's ready when picture data is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void init_pic_params(
  /* X-coordinate of where to put the picture to follow, e.g top left,
     bottom right, center, or percent from origin */
  cmx_disp_af_pos_enum_type            x_pos,
  /* Y-coordinate of where to put the picture to follow, e.g top left,
     center, bottom right, or percentage from origin of display */
  cmx_disp_af_pos_enum_type            y_pos,
  /* Percentage offset of x-coordinate */
  uint8                                x_percent,
  /* Percentage offset of y-coordinate */
  uint8                                y_percent,
  /* Image drawing start position */
  cmx_disp_point_type                 *start_position,
  /* Total size available for the image */
  cmx_disp_rect_size_type             *pic_size
)
{
  /* Initialize the picture structure */
  picture.header_read = FALSE;
  picture.transparency_read = FALSE;
  picture.background_read = FALSE;
  picture.isBackground = FALSE;
  picture.bkg_color = 0;
  picture.palette_read = FALSE;
  picture.isTransparency = FALSE;
  picture.isPalette = FALSE;
  picture.img.max_palette_entries = 0;
  picture.img.pixel_depth = 0;
  picture.img.palette_buf = NULL;
  picture.img.image_format = CMX_DISP_IMAGE_FORMAT_RGB;
  picture.img.image_size.width = 0;
  picture.img.image_size.height = 0;
  picture.img.image = NULL;
  picture.num_rows_received = 0;
  picture.x_pos = x_pos;
  picture.y_pos = y_pos;
  picture.x_percent = x_percent;
  picture.y_percent = y_percent;
  picture.start_position.x = start_position->x;
  picture.start_position.y = start_position->y;
  picture.picture_size.width = pic_size->width;
  picture.picture_size.height = pic_size->height;
  picture.current_position.x = 0;
  picture.current_position.y = 0;

#ifdef FEATURE_PNG_DECODER
  png_image.max_palette_entries = picture.img.max_palette_entries;
  png_image.pixel_depth = picture.img.pixel_depth;
  png_image.image_format = picture.img.image_format;
  png_image.image = (byte *)png_data;
  png_image.palette_buf = png_palette;
#endif /* #ifdef FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
  jpeg_image.max_palette_entries = 0;
  jpeg_image.pixel_depth = 16;
  jpeg_image.image_format = picture.img.image_format;
  jpeg_image.image = (byte *)jpeg_data;
  jpeg_image.palette_buf = NULL;
#endif

  return;
} /* End init_pic_params */


/*===========================================================================

FUNCTION       CLEAR_PIC_SCREEN

DESCRIPTION
               This function clears the picture portion of the display

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void clear_pic_screen(
  /* Image drawing start position */
  cmx_disp_point_type *start_position,
  /* Image drawing allowable size */
  cmx_disp_rect_size_type *pic_size
)
{
  disp_info_type device_info;

#ifdef FEATURE_PNG_DECODER
  png_image.max_palette_entries = 0;
  png_image.pixel_depth = 0;
  png_image.image_format = CMX_DISP_IMAGE_FORMAT_RGB;
  png_image.image_size.width = pic_size->width;
  png_image.image_size.height = pic_size->height;
  png_image.image = (byte *)png_data;
  memset(png_data, 0xFF, sizeof(png_data));
  memset(png_palette, 0x0, sizeof(png_palette));
#endif /* #ifdef FEATURE_PNG_DECODER */

  // Clear the display area
  device_info = disp_get_info();
  disp_clear_screen_area(OFFSET_Y, device_info.disp_height - 1,
                         0, device_info.disp_width - 1);

#ifdef FEATURE_JPEG_DECODER
  jpeg_image.max_palette_entries = 0;
  jpeg_image.pixel_depth = 16;
  jpeg_image.image_format = CMX_DISP_IMAGE_FORMAT_RGB;
  jpeg_image.image_size.width = pic_size->width;
  jpeg_image.image_size.height = pic_size->height;
  jpeg_image.image = (byte *)jpeg_data;
  memset(jpeg_data, 0xFF, sizeof(jpeg_data));
#endif


  return;
} /* End clear_pic_screen */


#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION      DISPLAY_PNG_PIC

DESCRIPTION
              This function displays a PNG format picture.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void display_png_pic(
  /* Either append this chunck of data, or its the last chunck */
  cmx_disp_af_pic_data_enum_type    pic_data,
  /* Number of bytes in this chunck of data */
  uint32                            num_bytes,
  /* Data buffer */
  uint8                            *buffer
)
{
  char tran_string[] = "tran";
  char bg_string[]   = "bkgd";
  char plte_string[] = "plte";
  uint32 num_bytes_remaining = num_bytes;
  char *tmp_buffer = (char *) buffer;

  /* Make sure the data buffer is not NULL */
  while  ( ( num_bytes_remaining > 0 ) && (tmp_buffer != NULL) )
  {

    /* 1. Header is in the first chunk of data sent */
    if ( picture.header_read == FALSE )
    {
      int32     color_type, width, height, pixel_depth;

      sscanf( tmp_buffer,
              "%ld\n%ld%ld\n%ld\n",
              &color_type,
              &width,
              &height,
              &pixel_depth );

      /* Color type */
/*    --- No processing for color type, for now ---
      if (color_type != 3)
      {
        picture.img.max_palette_entries = (uint32) 0;
        png_image.max_palette_entries = picture.img.max_palette_entries;
      }
*/
      /* Width */
      picture.picture_size.width  =
         MIN((uint32) width, png_image.image_size.width);
      picture.img.image_size.width = picture.picture_size.width;

      /* Height */
      picture.picture_size.height =
         MIN((uint32) height, png_image.image_size.height);

      /* Pixel depth */
      picture.img.pixel_depth = (uint32) pixel_depth;
      png_image.pixel_depth = picture.img.pixel_depth;

      /* Set the curent position to the start of the image based on
          picture control information */
      set_pic_start_position();

      /* Advance by the size information */
      tmp_buffer = strchr(tmp_buffer, '\n') + 1;

      if (NULL != tmp_buffer)
      {
          tmp_buffer = strchr(tmp_buffer, '\n') + 1;
      }

      if (NULL != tmp_buffer)
      {
          tmp_buffer = strchr(tmp_buffer, '\n') + 1;
      }

      num_bytes_remaining = num_bytes - ((uint8 *) tmp_buffer - buffer) ;
      picture.header_read = TRUE;
      continue;
    }

    /* 2. Transperancy is next, if any */
    if ( picture.transparency_read == FALSE )
    {
      uint32 i;
      uint32 max_palette_entries;
      uint8 *palette_ptr;

      /* Get Number of transperancy values */
      if ( strncmp(tran_string, tmp_buffer, strlen(tran_string)) == 0 )
      {
        tmp_buffer += strlen(tran_string);
        sscanf(tmp_buffer, "%d\n", &(picture.tran_nvalues) );
        picture.isTransparency = TRUE;

        /* Fill transperance values in the palette */
        max_palette_entries = (1 << picture.img.pixel_depth);
        palette_ptr = png_image.palette_buf + 3;
        for ( i = 0; i < max_palette_entries; i++ )
        {
          *palette_ptr = (char) 255;
          palette_ptr += 4;
        }

        tmp_buffer = strchr(tmp_buffer, '\n') + 1;
        num_bytes_remaining = num_bytes - ((uint8 *) tmp_buffer - buffer);
        continue;
      }

      if ( picture.isTransparency )
      {
        palette_ptr = png_image.palette_buf + 3;
        if (picture.tran_nvalues <= MAX_PALETTE_BUF_SIZE)
        {
            for ( i = 0; (i < picture.tran_nvalues); i++ )
            {
                *palette_ptr = *buffer++; /* Transparency entry */
                palette_ptr += 4;         /* Skip RGB */
            }
        }

        num_bytes_remaining = num_bytes - ((uint8 *) tmp_buffer - buffer);
        picture.transparency_read = TRUE;
        continue;
      }
    }

    /* 3. Background color is next, if any */
    if ( ( picture.background_read == FALSE ) &&
         ( strncmp(bg_string, tmp_buffer, strlen(bg_string)) == 0 ) )
    {
     tmp_buffer += strlen(bg_string);
      sscanf(tmp_buffer, "%d\n", &(picture.bkg_color));
      picture.isBackground = TRUE;

      tmp_buffer = strchr(tmp_buffer, '\n') + 1;
      num_bytes_remaining = num_bytes - ((uint8 *) tmp_buffer - buffer);
      picture.background_read = TRUE;
      continue;
    }

    /* 4. Palette is next, if any */
    if ( picture.palette_read == FALSE )
    {
      int i, palette_length;
      uint8 *palette_ptr;

      if ( strncmp(plte_string, tmp_buffer, strlen(plte_string)) == 0 )
      {
        tmp_buffer += strlen(plte_string);
        sscanf(tmp_buffer, "%d\n", &palette_length );
        picture.img.max_palette_entries = (uint32) palette_length / 3;
        png_image.max_palette_entries = picture.img.max_palette_entries;
        picture.isPalette = TRUE;

        tmp_buffer = strchr(tmp_buffer, '\n') + 1;
        num_bytes_remaining = num_bytes - ((uint8 *) tmp_buffer - buffer);
        continue;
      }

      if ( picture.isPalette == TRUE )
      {
          palette_ptr = png_image.palette_buf;
          if (picture.img.max_palette_entries <= MAX_PALETTE_BUF_SIZE)
          {
              for ( i = 0; (i < (uint32) picture.img.max_palette_entries) ; i++ )
              {
                  *palette_ptr++ = *buffer++;   /* R */
                  *palette_ptr++ = *buffer++;   /* G */
                  *palette_ptr++ = *buffer++;   /* B */
                  palette_ptr++;                /* Skip transparency entry */
              }
          }

          num_bytes_remaining -= (3 * picture.img.max_palette_entries);
          picture.palette_read = TRUE;
          continue;
      }
    }

    /* 5. Now that we have the palette, we can receive the image one row or a
       a multiple or rows at a time */
    if (picture.num_rows_received < picture.picture_size.height)
    {
      int num_rows = num_bytes_remaining / picture.img.image_size.width;
      int offset, num_bytes_to_copy;

      /* Reset the palette to default, if no palette is supplied with the image */
      if (picture.palette_read == FALSE )
      {
        picture.img.palette_buf = NULL;
        png_image.palette_buf = NULL;
        set_system_palette();
        picture.palette_read = TRUE;
      }

      picture.img.image_size.height = num_rows;
      picture.img.image = buffer;

      /* Copy the image at the current position */
        num_bytes_to_copy = (png_image.image_size.width *
                             num_rows * png_image.pixel_depth) / 8;
        offset = (picture.current_position.x +
                  picture.current_position.y * png_image.image_size.width) *
                  (png_image.pixel_depth/8);
        memcpy(png_image.image + offset, picture.img.image, num_bytes_to_copy);

      /* Update current position to go to next row */
      picture.current_position.y += num_rows;

      /* Increment the number of rows received */
      picture.num_rows_received += num_rows;
    }

    num_bytes_remaining -= num_bytes;
  }  /* End while */
  return;
} /* End display_png_pic */
#endif /* #ifdef FEATURE_PNG_DECODER */


#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION      DISPLAY_JPEG_PIC

DESCRIPTION
              This function displays a JPEG picture.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void display_jpeg_pic(
  /* Either append this chunck of data, or its the last chunck */
  cmx_disp_af_pic_data_enum_type    pic_data,
  /* Number of bytes in this chunck of data */
  uint32                            num_bytes,
  /* Data buffer */
  uint8                            *buffer
)
{
  char      tc_string[] = "P6";
  uint32    num_bytes_remaining = num_bytes;
  char     *tmp_buffer = (char *) buffer;
  int32     width, height, max_pixel_value;
  uint8    *temp_ptr;
  uint8     red, green, blue;
  OEMPixel  value, *dst_ptr;
  uint32    i;
  int       offset;

  /* Make sure the data buffer is not NULL */
  while  ( ( num_bytes_remaining > 0 ) && (tmp_buffer != NULL) )
  {

    if ( strncmp(tc_string, tmp_buffer, strlen(tc_string)) == 0 )
    {
      tmp_buffer += strlen(tc_string);
    }

    /* Header is in the first chunk of data sent */
    if ( picture.header_read == FALSE )
    {
      sscanf( tmp_buffer, "%ld%ld\n%ld\n", &width, &height, &max_pixel_value );

      /* Width */
      picture.picture_size.width  =
         MIN((uint32) width, jpeg_image.image_size.width);
      picture.img.image_size.width = picture.picture_size.width;

      /* Height */
      picture.picture_size.height =
         MIN((uint32) height, jpeg_image.image_size.height);

      /* Pixel depth */
      picture.img.pixel_depth = 16;
      jpeg_image.pixel_depth = picture.img.pixel_depth;

      /* Set the curent position to the start of the image based on
          picture control information */
      set_pic_start_position();

      /* Advance by the size information */
      tmp_buffer = strchr(tmp_buffer, '\n') + 1;

      if (NULL != tmp_buffer)
      {
        tmp_buffer = strchr(tmp_buffer, '\n') + 1;
      }

      if (NULL != tmp_buffer)
      {
        tmp_buffer = strchr(tmp_buffer, '\n') + 1;
      }

      num_bytes_remaining = num_bytes - ((uint8 *) tmp_buffer - buffer);
      picture.header_read = TRUE;
      continue;
    }

    /* receive the image one row or multiple or rows at a time */
    if (picture.num_rows_received < picture.picture_size.height)
    {
      int num_rows = (num_bytes_remaining/3) / picture.img.image_size.width;

      /* Reset the palette to default, if no palette is supplied with the image */
      if (picture.palette_read == FALSE )
      {
        picture.img.palette_buf = NULL;
        jpeg_image.palette_buf = NULL;
        set_system_palette();
        picture.palette_read = TRUE;
      }

      picture.img.image_size.height = num_rows;
      temp_ptr  = buffer;
      offset = picture.current_position.x +
               picture.current_position.y * jpeg_image.image_size.width;
      dst_ptr = jpeg_data + offset;
      if (picture.img.image_size.width <= MAX_PALETTE_BUF_SIZE)
      {
          for(i=0; (i< (uint32) picture.img.image_size.width); i++)
          {
              red = *temp_ptr++;
              green = *temp_ptr++;
              blue = *temp_ptr++;
              value = RGB_TO_NATIVE(red, green, blue);
              *dst_ptr++ = value;
          }
      }

      /* Update current position to go to next row */
      picture.current_position.y += num_rows;

      /* Increment the number of rows received */
      picture.num_rows_received += num_rows;
    }
    num_bytes_remaining -= num_bytes;
  }  /* End while */
  return;
} /* End display_jpeg_pic */
#endif


#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION      DISPLAY_SAF_PIC

DESCRIPTION
              This function displays a PNG format picture.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void display_saf_pic(
  /* Either append this chunck of data, or its the last chunck */
  cmx_disp_af_pic_data_enum_type    pic_data,
  /* Number of bytes in this chunck of data */
  uint32                            num_bytes,
  /* Data buffer */
  uint8                            *buffer
)
{
  int num_rows;

  ASSERT (buffer != NULL);

  /* we receive the whole image */
  if (num_bytes > MAX_PNG_SIZE) num_bytes = MAX_PNG_SIZE;
  num_rows = num_bytes / picture.picture_size.width;

  /* Draw the image in dest_rect
  */
  if (picture.img.pixel_depth == 8)
  {
    convert_pic_8_to_native
    (
      buffer,
      png_data2,
      num_bytes
    );

    disp_update(png_data2,
                picture.picture_size.width,
                0,
                0,
                num_rows,
                picture.picture_size.width,
                picture.start_position.y,
                picture.start_position.x
               );
  }
  else if (picture.img.pixel_depth == 16)
  {
    disp_update(buffer,
                picture.picture_size.width,
                0,
                0,
                num_rows,
                picture.picture_size.width,
                picture.start_position.y,
                picture.start_position.x
               );
  }

  return;

} /* End display_saf_pic */
#endif /* #ifdef FEATURE_PNG_DECODER */


/*===========================================================================

FUNCTION      DISPLAY_BMP_PIC

DESCRIPTION
              This function displays a BMP format picture.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void display_bmp_pic(
  /* Either append this chunck of data, or its the last chunck */
  cmx_disp_af_pic_data_enum_type    pic_data,
  /* Number of bytes in this chunck of data */
  uint32                            num_bytes,
  /* Data buffer */
  uint8                            *buffer
)
{
  return;
} /* End display_bmp_pic */


#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION      SET_PIC_START_POSITION

DESCRIPTION
              This function calculates the location of picture on the
              display based on the picture control that was sent previously.

DEPENDENCIES
  This function assumes that the picture width and height information is
  correct.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void set_pic_start_position()
{
  switch (picture.x_pos)
  {
    case CMX_DISP_AF_POS_LT:
      break;

    case CMX_DISP_AF_POS_RB:
      picture.current_position.x = png_image.image_size.width -
                                   picture.picture_size.width;
      break;

    case CMX_DISP_AF_POS_CENTER:
      picture.current_position.x =
          (png_image.image_size.width - picture.picture_size.width)/2;
      break;

    case CMX_DISP_AF_POS_PERCENT:
      /* The value x_percent is relative to 255 */
      picture.current_position.x =
          ((png_image.image_size.width * picture.x_percent) / 255);
      break;

    default:
      break;
  }

  switch (picture.y_pos)
  {
    case CMX_DISP_AF_POS_LT:
      break;

    case CMX_DISP_AF_POS_RB:
      picture.current_position.y =
          (png_image.image_size.height - picture.picture_size.height);
      break;

    case CMX_DISP_AF_POS_CENTER:
      picture.current_position.y =
          ((png_image.image_size.height - picture.picture_size.height) / 2);
      break;

    case CMX_DISP_AF_POS_PERCENT:
      /* The value y_percent is relative to 255 */
      picture.current_position.y =
          ((png_image.image_size.height * picture.y_percent) / 255);
      break;

    default:
      break;
  }

  return;
} /* End set_pic_start_position */
#endif /* #ifdef FEATURE_PNG_DECODER */

#if defined(FEATURE_SAF) || defined(FEATURE_PNG_DECODER)
  static OEMPixel convert_8_to_native_custom_tbl[256];
#endif /* #if defined(FEATURE_SAF) || defined(FEATURE_PNG_DECODER) */

static OEMPixel convert_8_to_native_system_tbl[256];

static boolean use_sys_palette = FALSE;
static boolean sys_palette_initialized = FALSE;

#ifdef FEATURE_PNG_DECODER
void convert_pic_8_to_native
(
  byte     *src_ptr,
  OEMPixel *dst_ptr,
  word      count
)
{
  int32 i;

  if (use_sys_palette)
  {
    for (i=0; i<count; i++)
    {
      *dst_ptr++ = convert_8_to_native_system_tbl[*src_ptr++];
    }
  }
  else
  {
    for (i=0; i<count; i++)
    {
      *dst_ptr++ = convert_8_to_native_custom_tbl[*src_ptr++];
    }
  }
}
#endif /* #ifdef FEATURE_PNG_DECODER */

#if defined(FEATURE_SAF) || defined(FEATURE_PNG_DECODER)
/*===========================================================================

FUNCTION      SET_PALETTE

DESCRIPTION
              This function sets the palette for the specified number of
              contiguous entries, starting at zero. The palette is used
              for either greyscale or RGB color output.

DEPENDENCIES
  The display must have already been initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void set_palette(
  /* A pointer to a palette that is copied directly into the palette RAM in
     the Display Device */
  cmx_disp_palette_entry_type *palette
)
{
  uint32 i;

  for (i=0; i<256; i++)
  {
    convert_8_to_native_custom_tbl[i] = RGB_TO_NATIVE(palette[i].rgb.red,
                                                      palette[i].rgb.green,
                                                      palette[i].rgb.blue
                                                     );
  }
  use_sys_palette = FALSE;
} /* End set_palette */
#endif /* #if defined(FEATURE_SAF) || defined(FEATURE_PNG_DECODER) */

/*===========================================================================

FUNCTION      SET_SYSTEM_PALETTE

DESCRIPTION
              This function sets the palette for the specified number of
              contiguous entries, starting at zero. The palette is used
              for either greyscale or RGB color output.

DEPENDENCIES
  The display must have already been initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void set_system_palette(void)
{
  uint32 i;
  cmx_disp_palette_entry_type *palette;

  palette = (cmx_disp_palette_entry_type *)system_palette;

  if (!sys_palette_initialized)
  {
    for (i=0; i<256; i++)
    {
      convert_8_to_native_system_tbl[i] = RGB_TO_NATIVE(palette[i].rgb.red,
                                                        palette[i].rgb.green,
                                                        palette[i].rgb.blue
                                                       );
    }
    sys_palette_initialized = TRUE;
  }
  use_sys_palette = TRUE;
} /* End set_system_palette */

#endif /*#if !defined(FEATURE_MULTIPROCESSOR) || 
         (defined(FEATURE_MULTIPROCESSOR) && defined(IMAGE_APPS_PROC))*/
#endif /* FEATURE_BITMAP && FEATURE_WEBAUDIO */


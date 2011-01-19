/*============================================================================
  
                 EXIF Header Generation Interface Header File
  
   GENERAL DESCRIPTION 
     This file contains the definitions needed for the EXIF header
     generation.

   EXTERNALIZED FUNCTIONS
  
   INITIALIZATION AND SEQUENCING REQUIREMENTS
   

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================

                      EDIT HISTORY FOR FILE
                                                                           
 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/exif/main/latest/src/exif.c#8 $ $DateTime: 2009/03/22 22:09:46 $ $Author: kdivvela $

 when      who    what, where, why   
 --------  -----  ----------------------------------------------------------
 11/05/08  kdiv    Extract main image width and height in header parsing
 09/09/08  kdiv   Extract thumbnail width and height in Exif header parsing
 09/04/08  sigu   Fixed an issue w.r.t. mis-alligned write fault
 07/02/08  kdiv   Fixed compiler warnings
 03/26/08  kdiv   Added Geotagging feature
 12/17/07  kdiv   Control of setting Date and Time tags in Exif encoder is given to svcs too
 07/12/07  kdiv   Fixed issue in not able to decode thumbnail image in files having 
                  JFIF and Exif headers  
 04/03/07  arv    Increased the PIM Length to accomodate the PIM parameters.
                  Fixed an issue related to zero memory read while accessing 
                  the PIM string
 02/14/07  arv    Modified string libraries in accordance with security 
                  standards
 01/25/07  arv    Added Print Image Matching Feature and hookup for 
                  Maker Note Exif Tags
 01/09/07  vma    Fixed lint errors
 07/18/06  arv    Fixed the issue related to decoder not decoding thumbnails in 
                  case of EXIF Little Endian format
 04/06/06  arv    Removed PIM Feature
 03/23/06  vma    Replaced all mallocs and frees to JPEG_MALLOC's and JPEG_FREE's
                  Fixed memory leak in jpeg_jfifread() and jpeg_exifread()
 01/05/06  dl     Handle APP1 and APP7 size limitation gracefully.
 09/29/05  kln    Fixed lint erors
 09/16/05  kln    Fixed coverity errors
 08/19/05  kln    Included msg.h file, fixed problem in thumbnail 
 08/18/05  kln    Fixed a problem related to de-referencing non-multiple of 4
                  pointer; set EXIF version to 2.2 
 08/08/05  kln    Fixed memset function 
 08/05/05  kln    Added exif_set_tag_value() function for backward compatibility 
 08/05/05  kln    Fixed a bug related to reading non-supported tags into a 
                  dummy tag
 07/29/05  kln    Added some missing tags support 
 07/18/05  kln    If FEATURE_CAMERA_MOBICAT insert sensor paramater info into 
                  JPEG stream
 06/28/05  kln    Extended EXIF interface to support camera specific attributes, 
                  made tag number in EXIF variable
 05/26/05  pdo    Forced don't care pointers to local address for L4 virtual address
 04/26/05  kln    Added a function to update thumbnail length when JFIF header
 04/18/05  kln    Removed value from exif_tag_type structure (all data is now stored
                  where offset is pointing to). Added fixedLength field indicating 
                  fixed or variable length tag. Made consistent exif init, read and 
                  write. Fixed some bugs related to exif read and write.
 03/02/05  arv    Fixed the issue with unsupported exif tags
 11/22/04  arv    Added the FEATURE_EXIF_PIM3 and FEATURE_EXIF_TEST_PIM3.
                  Fixed the issue of images having an error marker in the
                  zeroth ifd.
 10/26/04  arv    Added PIM - Print Image Matching Feature and provided API to set them
 10/20/04  pdo    Added subsample option to exif_emit_header()
 10/12/04  arv    Changed exif_terminate() and exif_set_string()
 09/23/04  pdo    Terminated deallocated pointer with NULL.
 09/02/04  pdo    Added restart marker and thumbnail subsampling for 6550 JPEG encoder
 08/10/04  arv    Added the EXIF Software tag 
 07/29/04  ckl    Added function JPEG_JFIFREAD, which is called by OEMJPEG.c
                  to decode the thumbnails of JFIF files (if available)
 07/27/04  ckl    Can now successfully support encoding JFIF structures with
                  thumbnail extension.
 07/13/04  ckl    Added functions to allow the getting and setting of exif values 
                  (i.e. make, model, etc.)
 06/23/04  ckl    Functions from OEMJPEG.c now successfully merged here; information
                  decoded is actually saved to exif_info_structure.
 05/21/04  sai    Added EXIF_MAKE_STRING and EXIF_MODEL_STRING.
                  Modified exif_set_string to set exif tags make and model.
 03/10/04  pdo    Added exif_terminate() and removed potential memory leak in
                  exif_set_string()
 02/04/04  mvd    Changed args to exif_emit_header for quality and DQT
                  tables.  Replaced exif_quality_type quality with int16 
                  quality_percent, from exif info type.
 02/02/04  pdo    Changed EXIF make and model strings - KDDI->QCOM
 01/29/04  mvd    Made write_frame_header() function non-static, called
                  by camera.c now for File Size Control.  Also saving 
                  buffer position of frame header to buf_index_dqt_pos var.
 01/23/04  mvd    In exif emit header(), now dynamically generated Q tables
                  are input to the encoder.  Also commented out unused 
                  static Q tables.  Keeping them around for reference.
 11/18/03  dle    Replaced OEM_Malloc and OEM_Free with malloc and free.
 09/23/03  pdo	  changed exif_set_value_tag interface.
 08/27/03  pdo	  Added PrintExif related tags and provided API to set them.
 07/07/03  pdo    Added GPS process method to EXIF header, use HYBRID-FIX
 06/27/03  pdo    Added Interop IFD and digitized time. 
                  Shared the date_time_string for both date time original and 
                  digitized time. Aligned GPS data to even address boundary.
                  Re-order the mandatory chunks.
                  Removed APP0 marker and its associated information.
                  All the above to meet the EXIF2.2 spec.
                  Added a set of quantization tables for 98% quality 
 05/21/03  dle    Remove DSP padding. Make sure short, long, and rational
                  strings are on even bundary.
 05/20/03  pdo    Restructured quantization and Huffman tables to meet
                  EXIF specification and still comply with JFIF.
 02/05/03  dle    Corrected TIFF_BYTE emit. Corrected gps_version_id.
                  If string is not present, do not emit a tag.
 02/04/03  dle    Removed YCrCb positioning in thumbnail IFD.
 02/03/03  dle    Added thumbnail support.
 01/21/03  dle    Moved position conversion here. Added support for more
                  GPS tags.
 01/16/03  dle    Added code for set_position.
 12/18/02  dle    Added exif_set_position() and exif_set_description().
 12/12/02  dle    Added next IFD offset field. Changed YCbCr positioning from
                  consistency to center.
 11/18/02  dle    Added exif_set_date_time_original(). Modified how
                  TIFF_RATIONAL is supported.
 11/14/02  dle    It is now debugged.
 11/13/02  dle    Added exif_emit_app1. Prepend exif_ to variables.
 11/12/02  dle    Added items for IFD0, EXIF IFD and GPS IFD.
 11/04/02  dle    Initial release

============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/

#include "exif.h"
#include "stdlib.h"
#include "string.h"
#include "clk.h"
#include "stdio.h"
#include "mobile.h"
#include "msg.h"                /* Message logging/reporting services      */
#include "jpeg_malloc.h"
#include "jpeg.h"

#if defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)
#ifdef JPEG_USES_LIBSTD
#include "AEEstd.h"
#endif /* JPEG_USES_LIBSTD */
/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/

#define BITS_IN_JSAMPLE  8
#define DCTSIZE2        64  /* DCTSIZE squared; # of elements in a block */

/* Since info can be on stack and not initialized, it is not easy to make
 * sure a single flag can do the job to make sure the info is properly
 * initialized. The following codes are used to mark info as initialized.
 * Hopefully this will work in most cases */
#define CODE1 0x12345678
#define CODE2 0x2345678a
#define CODE3 0x345678ab
#define CODE4 0x456789ab

#define NUM_HUFF_TBL 4

#ifdef FEATURE_EXIF_PIM3
#define PIM_LENGTH     1024
#endif

/*============================================================================
*                         MACRO DEFINITIONS
============================================================================*/

/*============================================================================
*                         DATA DECLARATIONS
============================================================================*/

/* ycbcr_positioning_type */
#define YCBCR_CENTER        1
//#define YCBCR_CONSISTENCY   2

/* color_space_type */
#define COLOR_SPACE_SRGB    1

/* gps_system_type */
#define GPS_ORTHOMETRIC_SYSTEM   0

/* compression_type */
//#define UNCOMPRESSED        1
#define JPEG_COMPRESSION    6

typedef enum
{      /* JPEG marker codes */
  M_SOF0  = 0xc0,
  M_SOF1  = 0xc1,
  M_SOF2  = 0xc2,
  M_SOF3  = 0xc3,
  M_SOF5  = 0xc5,
  M_SOF6  = 0xc6,
  M_SOF7  = 0xc7,
  M_JPG   = 0xc8,
  M_SOF9  = 0xc9,
  M_SOF10 = 0xca,
  M_SOF11 = 0xcb,
  M_SOF13 = 0xcd,
  M_SOF14 = 0xce,
  M_SOF15 = 0xcf,
  M_DHT   = 0xc4,
  M_DAC   = 0xcc,
  M_RST0  = 0xd0,
  M_RST1  = 0xd1,
  M_RST2  = 0xd2,
  M_RST3  = 0xd3,
  M_RST4  = 0xd4,
  M_RST5  = 0xd5,
  M_RST6  = 0xd6,
  M_RST7  = 0xd7,
  M_SOI   = 0xd8,
  M_EOI   = 0xd9,
  M_SOS   = 0xda,
  M_DQT   = 0xdb,
  M_DNL   = 0xdc,
  M_DRI   = 0xdd,
  M_DHP   = 0xde,
  M_EXP   = 0xdf,
  M_APP0  = 0xe0,
  M_APP1  = 0xe1,
  M_APP2  = 0xe2,
  M_APP3  = 0xe3,
  M_APP4  = 0xe4,
  M_APP5  = 0xe5,
  M_APP6  = 0xe6,
  M_APP7  = 0xe7,
  M_APP8  = 0xe8,
  M_APP9  = 0xe9,
  M_APP10 = 0xea,
  M_APP11 = 0xeb,
  M_APP12 = 0xec,
  M_APP13 = 0xed,
  M_APP14 = 0xee,
  M_APP15 = 0xef,
  M_JPG0  = 0xf0,
  M_JPG13 = 0xfd,
  M_COM   = 0xfe,
  M_TEM   = 0x01,
  M_ERROR = 0x100
} JPEG_MARKER;

/* Default EXIF tags */

static char exif_make_string[8] = "QCOM-AA";
static char exif_model_string[8] = "QCAM-AA";
static exif_rational_type exif_x_resolution = {0x48, 0x01};
static exif_rational_type exif_y_resolution = {0x48, 0x01};
static byte exif_version_string[4] = {0x30, 0x32, 0x32, 0x30};
static byte exif_date_time_string[20] = "2002:12:08 12:00:00";
static byte exif_components_configuration_string[4] = {0x01, 0x02, 0x03, 0x00};
static byte exif_flashpix_version_string[4] = {0x30, 0x31, 0x30, 0x30};
static byte exif_latitude_ref_string[4] = {'N', 0x00, 0x00, 0x00};
static byte exif_gps_version_id[4] = {0x02, 0x02, 0x00, 0x00};
static exif_rational_type exif_latitude[3] = {{0, 1}, /* degrees */
  {0, 1}, /* minutes */
  {0, 100} /* centi-seconds */
};
static byte exif_longitude_ref_string[4] = {'E', 0x00, 0x00, 0x00};
static exif_rational_type exif_longitude[3] = {{0, 1}, /* degrees */
  {0, 1}, /* minutes */
  {0, 100} /* centi-seconds */
};
static exif_rational_type exif_altitude = {0, 1}; /* meters */
static exif_rational_type exif_gps_timestamp[3] = {{0, 1}, /* hours */
  {0, 1}, /* minutes */
  {0, 1} /* seconds */
};
static byte exif_map_datum_string[7] = "WGS-84";
static byte exif_gps_datestamp[11] = "2002:12:08";
static byte exif_gps_process_method[18] = { 0x41, 0x53, 0x43, 0x49, 0x49, 0x0, 0x0,
  0x00, 0x48, 0x59, 0x42, 0x52, 0x49,0x44,
  0x2D, 0x46, 0x49, 0x58};
static byte exif_interop_index_string[4] = { 'R','9','8',0x0};
static byte exifR98_version_string[4]    = { '0', '1', '0', '0'};
static exif_rational_type exif_exposure_time = {1, 125};
static exif_rational_type exif_digital_zoom_ratio  =  {0, 2};
static int16 exif_orientation;
static int16 exif_resolution;
static int16 exif_ycbcr_positioning ;
static int16 exif_flash ;
static int16 exif_color_space;
static int32 exif_pixel_x_dimension;
static int32 exif_pixel_y_dimension;
static int16 exif_custom_rendered;
static int16 exif_exposure_mode;
static int16 exif_white_balance;
static int16 exif_scene_capture_type;
static int16 exif_subject_distance_range;
static int8  exif_altitude_ref;
/*We need to make sure that the address of variable pointed by exif_compression*/
/*is 32 bit alligned rather than 16 bit alligned*/
/*if we have used int16 instead of int32 then &exif_compression will provided a 16 bit alligned address*/
/*rather than 32 bit alligned address*/
/*Kindly see further comments at ((uint32*)tag->offset)[0]= jpeg_exifread_get_4bytes(info);*/
static int32 exif_compression;
static int16 exif_resolution_unit;
static int32 exif_jpegi_format;
static int32 exif_jpegi_format_l;
static exif_rational_type exif_f_number;
static exif_rational_type exif_aperture;
static exif_rational_type exif_max_aperture;
static exif_rational_type exif_focal_length;
static int16 exif_focal_length_35mm;
static int16 exif_exposure_program;
static int16 iso_speed_ratings;
static int16 exif_light_source; 
static int16 exif_sensing_method;
static int16 exif_file_source;
static int16 exif_scene_type;
static int16 exif_contrast; 
static int16 exif_saturation;
static int16 exif_metering_mode;
static int16 exif_sharpness;
static int16 exif_oecf;
static int16 exif_gain_control;
static exif_rational_type exif_shutter_speed;
static exif_rational_type exif_brightness;
static exif_rational_type exif_exposure_index;
static exif_rational_type exif_subject_distance;
static int16 exif_subject_location;
static uint32 exif_ifd_pointer;
static uint32 gps_ifd_pointer;
static uint32 interop_ifd_pointer;
static boolean invalidTag = FALSE;
static boolean set_date_time = TRUE;
#ifdef FEATURE_EXIF_PIM3
static char *pim_string;
#endif


#ifdef FEATURE_GEOTAGGING
LOCAL    exif_info_type          jpeg_exif_info;
#endif /* FEATURE_GEOTAGGING */

/* JPEG natural order for the DC coefficient is the order in which
   DCT coefficients are scanned before they are run-length coded */ 

static const int exif_jpeg_natural_order[DCTSIZE2] = {
  0,  1,  8, 16,  9,  2,  3, 10,
  17, 24, 32, 25, 18, 11,  4,  5,
  12, 19, 26, 33, 40, 48, 41, 34,
  27, 20, 13,  6,  7, 14, 21, 28,
  35, 42, 49, 56, 57, 50, 43, 36,
  29, 22, 15, 23, 30, 37, 44, 51,
  58, 59, 52, 45, 38, 31, 39, 46,
  53, 60, 61, 54, 47, 55, 62, 63};

static JHUFF_TBL  exif_huffman_tbl[NUM_HUFF_TBL];
quantization_type  exif_luma_dqt_low[64]= {              /* 50% quality */
  16,11,10,16, 24, 40, 51, 61,
  12,12,14,19, 26, 58, 60, 55,
  14,13,16,24, 40, 57, 69, 56,
  14,17,22,29, 51, 87, 80, 62,
  18,22,37,56, 68,109,103, 77,
  24,35,55,64, 81,104,113, 92,
  49,64,78,87,103,121,120,101,
  72,92,95,98,112,100,103, 99};

quantization_type  exif_chroma_dqt_low[64]={
  17,18,24,47,99,99,99,99,
  18,21,26,66,99,99,99,99,
  24,26,56,99,99,99,99,99,
  47,66,99,99,99,99,99,99,
  99,99,99,99,99,99,99,99,
  99,99,99,99,99,99,99,99,
  99,99,99,99,99,99,99,99,
  99,99,99,99,99,99,99,99};


static jpeg_component_info exif_jpeg_comp_info[3];


/*============================================================================
*                         EXTERNAL FUNCTIONS
============================================================================*/
#ifdef FEATURE_CAMERA_MOBICAT
boolean jpege_emit_app7(exif_info_type   *info);
#endif

/*===========================================================================
FUNCTION  EXIF_EMIT_BYTE

DESCRIPTION
  Emit a byte.

DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_byte (exif_info_type *info, uint8 val)
{
  if (info->buf_index >= info->buf_len) return(FALSE);
  info->buf_ptr[info->buf_index++] = val;
  return(TRUE);
}


/*===========================================================================
FUNCTION  EXIF_EMIT_2BYTES

DESCRIPTION
  Emit a 2-byte integer; these are always MSB first in JPEG files.

DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_2bytes (exif_info_type *info, uint16 value)
{
  if (exif_emit_byte(info, (value >> 8) & 0xFF) == FALSE) return(FALSE);
  if (exif_emit_byte(info, value & 0xFF) == FALSE) return(FALSE);
  return(TRUE);
}

/*===========================================================================
FUNCTION  EXUF_EMIT_4BYTES

DESCRIPTION
  Emit a 4-byte integer; these are always MSB first in JPEG files.

DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_4bytes (exif_info_type *info, uint32 value)
{
  if (exif_emit_byte(info, (value >> 24) & 0xFF) == FALSE) return(FALSE);
  if (exif_emit_byte(info, (value >> 16) & 0xFF) == FALSE) return(FALSE);
  if (exif_emit_byte(info, (value >> 8) & 0xFF) == FALSE) return(FALSE);
  if (exif_emit_byte(info, value & 0xFF) == FALSE) return(FALSE);
  return(TRUE);
}

/*===========================================================================
FUNCTION  EXIF_EMIT_MARKER

DESCRIPTION
  Emit a marker code.

DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_marker (exif_info_type *info, JPEG_MARKER mark)
{
  if (exif_emit_byte(info, 0xFF) == FALSE) return(FALSE);
  if (exif_emit_byte(info, (uint8) mark) == FALSE) return(FALSE);
  return(TRUE);

}

/*===========================================================================
FUNCTION  EXIF_EMIT_SOI

DESCRIPTION
  Emit a SOI marker.

DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_soi (exif_info_type *info)
{
  if (exif_emit_marker(info, M_SOI))
  {
    return(TRUE);
  } else
  {
    return(FALSE);
  }
}

/*===========================================================================
FUNCTION  EXIF_EMIT_DQT

DESCRIPTION
  Emit a DQT marker.

DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_dqt (exif_info_type *info, int index)
{
  quantization_type * qtbl = info->quant_tbl_ptrs[index];
  uint16 i;
  uint32 qval;

  for (i = 0; i < DCTSIZE2; i++)
  {
    /* The table entries must be emitted in zigzag order. */
    qval = qtbl[exif_jpeg_natural_order[i]];
    if (exif_emit_byte(info, (byte) (qval & 0xFF)) == FALSE) return(FALSE);
  }
  return(TRUE);
}

/*===========================================================================
FUNCTION  EXIF_EMIT_DHT

DESCRIPTION
  Emit a DHT marker

DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_dht (exif_info_type *info, uint8 index, boolean is_ac)
{
  JHUFF_TBL * htbl;
  uint16 length, i;

  if (is_ac)
  {
    htbl = info->ac_huff_tbl_ptrs[index];
    index += 0x10;    /* output index has AC bit set */
  } else
  {
    htbl = info->dc_huff_tbl_ptrs[index];
  }

  if (!htbl->sent)
  {
    htbl->sent = TRUE;
    length = 0;
    for (i = 1; i <= 16; i++)
      length += htbl->bits[i];

    if (exif_emit_byte(info, index) == FALSE) return(FALSE);

    for (i = 1; i <= 16; i++)
      if (exif_emit_byte(info, htbl->bits[i]) == FALSE) return(FALSE);

    for (i = 0; i < length; i++)
      if (exif_emit_byte(info, htbl->huffval[i]) == FALSE) return(FALSE);
  }
  return(TRUE);
}

/*===========================================================================
FUNCTION  EXIF_EMIT_SOF

DESCRIPTION
  Emit a SOF marker
  For more info refer to JPEG SOF marker spec
DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_sof (exif_info_type *info, JPEG_MARKER code)
{
  uint16 ci;

  jpeg_component_info *compptr;

  if (exif_emit_marker(info, code) == FALSE) return(FALSE);

  if (exif_emit_2bytes(info, 3 * info->num_components + 2 + 5 + 1) == FALSE)
    return(FALSE);

  if (exif_emit_byte(info, info->data_precision) == FALSE) return(FALSE);

  if (info->emit_thumbnail)
  {
    if (exif_emit_2bytes(info, info->thumbnail_height) == FALSE) return(FALSE);
    if (exif_emit_2bytes(info, info->thumbnail_width) == FALSE) return(FALSE);
  } else
  {
    if (exif_emit_2bytes(info, info->image_height) == FALSE) return(FALSE);
    if (exif_emit_2bytes(info, info->image_width) == FALSE) return(FALSE);
  }

  if (exif_emit_byte(info, info->num_components) == FALSE) return(FALSE);

  for (ci = 0, compptr = info->comp_info; ci < info->num_components;
      ci++, compptr++)
  {
    if (exif_emit_byte(info, compptr->component_id) == FALSE) return(FALSE);

    if (exif_emit_byte(info, (uint8)((compptr->h_samp_factor << 4) +
                                     compptr->v_samp_factor)) == FALSE)
    {
      return(FALSE);
    }
    if (exif_emit_byte(info, compptr->quant_tbl_no) == FALSE) return(FALSE);
  }
  return(TRUE);
}

/*===========================================================================
FUNCTION  EXIF_EMIT_SOS

DESCRIPTION
  Emit DRI chunk and SOS chunk
  For more info refer to JPEG SOS marker spec
DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_sos (exif_info_type *info)
{
  uint8 i, td, ta;
  jpeg_component_info *compptr;

  /* Depend on the restart_cnt and emit the restart marker accordingly */ 
  if (info->restart_cnt)
  {
    if (exif_emit_marker(info, M_DRI) == FALSE) return(FALSE);

    if (exif_emit_2bytes(info, 4) == FALSE) return(FALSE);

    if (exif_emit_2bytes(info, info->restart_cnt) == FALSE) return(FALSE);
  }

  if (exif_emit_marker(info, M_SOS) == FALSE) return(FALSE);

  if (exif_emit_2bytes(info, 2 * info->num_components + 2 + 1 + 3) == FALSE)
    return(FALSE);

  if (exif_emit_byte(info, info->num_components) == FALSE) return(FALSE);

  for (i = 0, compptr = info->comp_info; i < info->num_components;
      i++, compptr++)
  {
    if (exif_emit_byte(info, compptr->component_id) == FALSE) return(FALSE);
    td = compptr->dc_tbl_no;
    ta = compptr->ac_tbl_no;
    if (exif_emit_byte(info, (uint8)((td << 4) + ta)) == FALSE) return(FALSE);
  }

  if (exif_emit_byte(info, info->Ss) == FALSE) return(FALSE);
  if (exif_emit_byte(info, info->Se) == FALSE) return(FALSE);
  if (exif_emit_byte(info, (uint8)((info->Ah << 4) + info->Al)) == FALSE) return(FALSE);
  return(TRUE);
}

/*===========================================================================
FUNCTION  EXIF_EMIT_TIFF_TAG

DESCRIPTION
  Emit a TIFF tag

DEPENDENCIES
  None

RETURN VALUE
  TRUE (if enough space in stream)
  FALSE (if not enough space in stream)

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_tiff_tag
(
exif_info_type *info,
const exif_tag_type *tag
)
{
  uint32 i;

  /* If tag is not set return TRUE */
  if (!tag->active) return(TRUE);
  if (exif_emit_2bytes(info, tag->id) == FALSE) return(FALSE);
  if (exif_emit_2bytes(info, tag->type) == FALSE) return(FALSE);
  if (exif_emit_4bytes(info, tag->count) == FALSE) return(FALSE);

  switch (tag->type)
  {
    case EXIF_ASCII:
    case EXIF_UNDEFINED:
    case EXIF_BYTE:
      if (tag->count)
      {
        /*  Write value to exif header */
        if (tag->count <= 4)
        {
          memcpy(info->buf_ptr + info->buf_index, tag->offset, tag->count);
          info->buf_index += 4;
        } else
        {
          /* Write offset to exif header and value to stream */
          /* Make sure the offset is always at the even boundary */
          if (info->string_index & 1) info->string_index++;
          if (exif_emit_4bytes(info, info->string_index - info->tiff_header_index) == FALSE) return(FALSE);

          if ((info->string_index + tag->count)<info->buf_len)
          {
            memcpy(info->buf_ptr + info->string_index, tag->offset, tag->count);
            info->string_index += tag->count;
          }
        }
      } else
      {
        if (exif_emit_4bytes(info, 0) == FALSE) return(FALSE);
      }
      break;

    case EXIF_SHORT:
      if (tag->count)
      {
        uint16 * ptr;

        /*  Write value to exif header */
        if (tag->count == 1)
        {
          if (exif_emit_2bytes(info, ((uint16*)tag->offset)[0]) == FALSE) return(FALSE);
          if (exif_emit_2bytes(info, 0) == FALSE) return(FALSE);
        } else if (tag->count == 2)
        {
          ptr = (uint16 *)(tag->offset);
          if (exif_emit_2bytes(info, *ptr++) == FALSE) return(FALSE);
          if (exif_emit_2bytes(info, *ptr++) == FALSE) return(FALSE);
        } else
        {
          /* Write offset to exif header and value to stream */
          ptr = (uint16 *)(tag->offset);
          /* Make sure the string index is at even boundary */
          if (info->string_index & 1)
          {
            info->string_index++;
          }
          if (exif_emit_4bytes(info, info->string_index - info->tiff_header_index) == FALSE) return(FALSE);
          info->buf_index_save = info->buf_index;
          info->buf_index = info->string_index;
          for (i=0; i<tag->count; i++)
          {
            if (exif_emit_2bytes(info, *ptr++) == FALSE) return(FALSE);
          }
          info->string_index = info->buf_index;
          info->buf_index = info->buf_index_save;
        }
      } else
      {
        /* Count is 0 */
        if (exif_emit_4bytes(info, 0) == FALSE) return(FALSE);
      }
      break;

    case EXIF_LONG:
      if (tag->count)
      {
        if (tag->count == 1)
        {
          /* Write value to exif header */
          if (exif_emit_4bytes(info, ((uint32*)tag->offset)[0]) == FALSE) return(FALSE);
        } else
        {
          /* Write offset to exif header and value to stream */
          uint32 * ptr = (uint32 *)(tag->offset);
          /* Make sure the string index is at even boundary */
          if (info->string_index & 1)
          {
            info->string_index++;
          }
          if (exif_emit_4bytes(info, info->string_index - info->tiff_header_index) == FALSE) return(FALSE);
          info->buf_index_save = info->buf_index;
          info->buf_index = info->string_index;
          for (i=0; i<tag->count; i++)
            if (exif_emit_4bytes(info, *ptr++) == FALSE) return(FALSE);
          info->string_index = info->buf_index;
          info->buf_index = info->buf_index_save;
        }
      } else
      {
        if (exif_emit_4bytes(info, 0) == FALSE) return(FALSE);
      }
      break;

    case EXIF_RATIONAL:
    case EXIF_SRATIONAL:
      if (tag->count)
      {
        /* Write offset to exif header and value to stream */
        exif_rational_type *ptr = (exif_rational_type *)tag->offset;
        /* Make sure the string index is at even boundary */
        if (info->string_index & 1)
        {
          info->string_index++;
        }
        if (exif_emit_4bytes(info, info->string_index - info->tiff_header_index) == FALSE) return(FALSE);
        info->buf_index_save = info->buf_index;
        info->buf_index = info->string_index;
        for (i=0; i<tag->count; i++, ptr++)
        {
          if (exif_emit_4bytes(info, (uint32)ptr->numerator) == FALSE) return(FALSE);
          if (exif_emit_4bytes(info, (uint32)ptr->denominator) == FALSE) return(FALSE);
        }
        info->string_index = info->buf_index;
        info->buf_index = info->buf_index_save;
      } else
      {
        if (exif_emit_4bytes(info, 0) == FALSE) return(FALSE);
      }
      break;

    default:
      return(FALSE);
  }
  return(TRUE);
}

/*===========================================================================
FUNCTION  EXIF_SET_DATE_TIME

DESCRIPTION
  Set time 

DEPENDENCIES
  None

RETURN VALUE
  exif_date_time_string

SIDE EFFECTS
  None
===========================================================================*/
void exif_set_date_time(uint8 *ptr)
{
  dword seconds;
  clk_julian_type julian;
  int remainder;

  if(set_date_time)
  {
      seconds = clk_read_secs();
      clk_secs_to_julian(seconds, &julian);
    
      /* Set year */
      *ptr++ = (uint8)(julian.year/1000 + '0');
      remainder = julian.year%1000;
      *ptr++ = (uint8)(remainder/100 + '0');
      remainder = remainder%100;
      *ptr++ = (uint8)(remainder/10 + '0');
      *ptr++ = (uint8)(remainder%10 + '0');
      *ptr++ = ':';
    
      /* Emit month */
      *ptr++ = (uint8)(julian.month/10 + '0');
      *ptr++ = julian.month%10 + '0';
      *ptr++ = ':';
    
      /* Emit date */
      *ptr++ = (uint8)(julian.day/10 + '0');
      *ptr++ = julian.day%10 + '0';
      *ptr++ = ' ';
    
      /* Emit time */
      *ptr++ = (uint8)(julian.hour/10 + '0');
      *ptr++ = julian.hour%10 + '0';
      *ptr++ = ':';
      *ptr++ = (uint8)(julian.minute/10 + '0');
      *ptr++ = julian.minute%10 + '0';
      *ptr++ = ':';
      *ptr++ = (uint8)(julian.second/10 + '0');
      *ptr++ = julian.second%10 + '0';
      *ptr = 0;
  }
}

/*===========================================================================
FUNCTION  EXIF_SET_GPS_DATE_TIME

DESCRIPTION
  Set time 

DEPENDENCIES
  None

RETURN VALUE
  exif_gps_datestamp

SIDE EFFECTS
  None
===========================================================================*/
void exif_set_gps_date_time(uint32 seconds)
{
  clk_julian_type julian;
  int remainder;
  uint8 *ptr = exif_gps_datestamp;

  clk_secs_to_julian(seconds, &julian);

  /* Set year */
  *ptr++ = (uint8)(julian.year/1000 + '0');
  remainder = julian.year%1000;
  *ptr++ = (uint8)(remainder/100 + '0');
  remainder = remainder%100;
  *ptr++ = (uint8)(remainder/10 + '0');
  *ptr++ = (uint8)(remainder%10 + '0');
  *ptr++ = ':';

  /* Emit month */
  *ptr++ = (uint8)(julian.month/10 + '0');
  *ptr++ = julian.month%10 + '0';
  *ptr++ = ':';

  /* Emit date */
  *ptr++ = (uint8)(julian.day/10 + '0');
  *ptr++ = julian.day%10 + '0';
  *ptr = 0;

  /* Emit time */
  exif_gps_timestamp[0].numerator = julian.hour;
  exif_gps_timestamp[1].numerator = julian.minute;
  exif_gps_timestamp[2].numerator = julian.second;
}


/*===========================================================================
FUNCTION  EXIF_EMIT_APP0

DESCRIPTION
  Emit a JFIF-compliant APP0 marker. First emits the main JFIF header, and then
  emits a second APP0 marker if the JFIF extension thumbnail is desired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_app0 (exif_info_type *info, exif_emit_type type)
{

  /* Initializing field values for JFIF specification */
  info->JFIF_major_version = 0x01;
  info->JFIF_minor_version = 0x02;
  info->density_unit = 0;
  info->X_density = 0x01;
  info->Y_density = 0x01;

  if (info->write_JFIF_header)
  {
    if (exif_emit_marker(info, M_APP0) == FALSE) return(FALSE);
  }

  if (!info->emit_thumbnail)
  {
    /*length for normal JFIF without extension is 16 bytes */
    if (exif_emit_2bytes(info, 0x10) == FALSE) return(FALSE);

    /* Emit JFIF code */
    if (exif_emit_byte(info, 'J') == FALSE) return(FALSE);
    if (exif_emit_byte(info, 'F') == FALSE) return(FALSE);
    if (exif_emit_byte(info, 'I') == FALSE) return(FALSE);
    if (exif_emit_byte(info, 'F') == FALSE) return(FALSE);
    if (exif_emit_byte(info, 0) == FALSE) return(FALSE);

    /* Current Version */
    if (exif_emit_byte(info, info->JFIF_major_version) == FALSE) return(FALSE);
    if (exif_emit_byte(info, info->JFIF_minor_version) == FALSE) return(FALSE);

    /* Units */
    if (exif_emit_byte(info, info->density_unit) == FALSE) return(FALSE);

    /* Density */
    if (exif_emit_2bytes(info, info->X_density) == FALSE) return(FALSE);
    if (exif_emit_2bytes(info, info->Y_density) == FALSE) return(FALSE);

    /* Thumbnails */
    if (exif_emit_byte(info, 0) == FALSE) return(FALSE);
    if (exif_emit_byte(info, 0) == FALSE) return(FALSE);
  }

  /* Otherwise, if this is JFXX with thumbnail, write this to the file */
  else if (type == JFXX_EMIT_UPTO_THUMBNAIL)
  {

    /* length will be calculated after all operations. bypass for now */
    info->length_index = info->buf_index;
    info->buf_index += 2;

    if (exif_emit_byte(info, 'J') == FALSE) return(FALSE);
    if (exif_emit_byte(info, 'F') == FALSE) return(FALSE);
    if (exif_emit_byte(info, 'X') == FALSE) return(FALSE);
    if (exif_emit_byte(info, 'X') == FALSE) return(FALSE);
    if (exif_emit_byte(info, 0) == FALSE) return(FALSE);

    /* Extension code for Thumbnail coded using JPEG */
    if (exif_emit_byte(info, 0x10) == FALSE) return(FALSE);

    /* Extension data */
    /* Update buffer index so it will continue from the end of string area */
    info->thumbnail_stream_index = info->buf_index;

    /* Emit thumbnail JPEG header */
    if (exif_emit_soi(info) == FALSE) return(FALSE);
    if (exif_write_frame_header(info) == FALSE) return(FALSE);

    if (exif_emit_sos(info) == FALSE) return(FALSE);

    /* Now update the APP0 length field */
    info->buf_index_save = info->buf_index;
    info->buf_index = info->length_index;
    if (exif_emit_2bytes(info, (uint16)(info->buf_index_save - info->length_index)) == FALSE)
      return(FALSE);
    info->buf_index = info->buf_index_save;
  }
  return(TRUE);
}


/*===========================================================================
FUNCTION  EXIF_EMIT_APP1

DESCRIPTION
  Emit a EXIF-compliant APP1 marker

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_emit_app1 (exif_info_type *info)
{
  uint16 count, odd;

 /* Set original date & time */
  exif_set_date_time(exif_date_time_string);

  if (exif_emit_marker(info, M_APP1) == FALSE) return(FALSE);

  /* length will be calculated after all operations. bypass for now */
  info->length_index = info->buf_index;
  info->buf_index += 2;

  /* EXIF ID code */
  if (exif_emit_byte(info, 'E') == FALSE) return(FALSE);
  if (exif_emit_byte(info, 'x') == FALSE) return(FALSE);
  if (exif_emit_byte(info, 'i') == FALSE) return(FALSE);
  if (exif_emit_byte(info, 'f') == FALSE) return(FALSE);
  if (exif_emit_byte(info, 0) == FALSE) return(FALSE);

  /* Pad to 4 byte boundary */
  if (exif_emit_byte(info, 0) == FALSE) return(FALSE);

  /* TIFF header */
  info->tiff_header_index = info->buf_index;
  if (!exif_emit_2bytes(info, 0x4d4d)) return(FALSE);
  if (exif_emit_2bytes(info, 0x002a) == FALSE) return(FALSE);
  if (exif_emit_4bytes(info, 0x00000008) == FALSE) return(FALSE);

  /* Emit 0th IFD */
  /* Number of interoperability */
  count = ZERO_IFD_FIELD_COUNT;
  if (!info->gps_set) count--;
  if (info->zero_ifd.image_description.count == 0) count--;
#ifdef FEATURE_EXIF_PIM3  
  if (info->zero_ifd.pim.count == 0) count--;
#endif
  if (exif_emit_2bytes(info, count) == FALSE)
    return(FALSE);

  /* Strings should follow all the tag fields and next exif offset */
  info->string_index = info->buf_index + EXIF_TAG_SIZE * count + 4;

  if (info->zero_ifd.image_description.offset)
  {
    if (exif_emit_tiff_tag(info, &info->zero_ifd.image_description) == FALSE)
      return(FALSE);
  }

  if (exif_emit_tiff_tag(info, &info->zero_ifd.make) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->zero_ifd.model) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->zero_ifd.orientation) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->zero_ifd.x_resolution) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->zero_ifd.y_resolution) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->zero_ifd.resolution_unit) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->zero_ifd.software) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->zero_ifd.ycbcr_positioning) == FALSE)
    return(FALSE);

  /* Save the EXIF IFD index for update later */
  info->exif_ifd_index = info->buf_index;
  if (exif_emit_tiff_tag(info, &info->zero_ifd.exif_ifd_pointer) == FALSE)
    return(FALSE);

  /* Emit the PIM parameters and cleanup of exif_pim memory */
 
#ifdef FEATURE_EXIF_PIM3   
  if (exif_emit_tiff_tag(info, &info->zero_ifd.pim) == FALSE)
  {
    exif_PIM3_terminate(info);
    return(FALSE);  
  } 
#endif

  if (info->gps_set)
  {
    /* Save the GPS IFD index for update later */
    info->gps_ifd_index = info->buf_index;
    if (exif_emit_tiff_tag(info, &info->zero_ifd.gps_ifd_pointer) == FALSE)
      return(FALSE);
  }
  /* See if thumbnail should be created */
  if (info->emit_thumbnail)
  {
    /* Need to include thumbnail */
    info->thumbnail_ifd_index = info->buf_index;
  }
  /* Next IFD (thumbnail IFD) offset */
  if (exif_emit_4bytes(info, 0) == FALSE)
    return(FALSE);
  /* Update buffer index so it will continue from the end of string area */
  info->buf_index = info->string_index;

  /* Emit EXIF IFD */
  /* Make sure the index is at 2-byte boundary. */
  odd = info->buf_index & 1;
  if (odd)
  {
    if (exif_emit_byte(info, 0) == FALSE) return(FALSE);
  }
  /* Update EXIF_IFD pointer */
  info->buf_index_save = info->buf_index;
  info->buf_index = info->exif_ifd_index + 8;
  if (exif_emit_4bytes(info, info->buf_index_save - info->tiff_header_index)
      == FALSE) return(FALSE);
  info->buf_index = info->buf_index_save;
  /* Number of interoperability */
  count =  exif_get_var_exif_ifd_count(info);
  if (info->exif_ifd.user_comment.count == 0) count--;
  if (info->exif_ifd.maker_note.count == 0) count--;

  if (exif_emit_2bytes(info, count) == FALSE)
    return(FALSE);

  /* Strings should follow all the tag fields and next exif offset */
  info->string_index = info->buf_index + EXIF_TAG_SIZE * count + 4;

  /* Set EXIF version to 2.2 */
  ((uint8*)info->exif_ifd.version.offset)[0] = '0';
  ((uint8*)info->exif_ifd.version.offset)[1] = '2';
  ((uint8*)info->exif_ifd.version.offset)[2] = '2';
  ((uint8*)info->exif_ifd.version.offset)[3] = '0';

  if (exif_emit_tiff_tag(info, &info->exif_ifd.exposure_time) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.f_number) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.exposure_program) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.iso_speed_ratings) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.oecf) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.version) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.date_time_original) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.date_time_digitized) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.components_configuration) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.shutter_speed) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.aperture) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.brightness) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.max_aperture) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.subject_distance) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.metering_mode) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.light_source) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.flash) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.focal_length) == FALSE)
    return(FALSE);
  if (info->exif_ifd.user_comment.count)
  {
    if (exif_emit_tiff_tag(info, &info->exif_ifd.user_comment) == FALSE)
      return(FALSE);
  }
  if (info->exif_ifd.maker_note.count)
  {
    if (exif_emit_tiff_tag(info, &info->exif_ifd.maker_note) == FALSE)
      return(FALSE);
  }
  if (exif_emit_tiff_tag(info, &info->exif_ifd.flashpix_version) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.color_space) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.pixel_x_dimension) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.pixel_y_dimension) == FALSE)
    return(FALSE);

  /* Save the Interop IFD index for update later */
  info->interop_ifd_index = info->buf_index;
  if (exif_emit_tiff_tag(info, &info->exif_ifd.interop_ifd_pointer) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.subject_location) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.exposure_index) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.sensing_method) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.file_source) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.scene_type) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.cfa_pattern) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.custom_rendered) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.exposure_mode) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.white_balance) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.digital_zoom_ratio) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.focal_length_35mm) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.scene_capture_type) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.gain_control) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.contrast) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.saturation) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.sharpness) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->exif_ifd.subject_distance_range) == FALSE)
    return(FALSE);

  /* Next IFD (none) offset */
  if (exif_emit_4bytes(info, 0) == FALSE)
    return(FALSE);
  /* Update buffer index so it will continue from the end of string area */
  info->buf_index = info->string_index;

  /* Update Interoperability IFD pointer */
  info->buf_index_save = info->buf_index;
  info->buf_index = info->interop_ifd_index + 8;
  if (exif_emit_4bytes(info, info->buf_index_save - info->tiff_header_index)
      == FALSE) return(FALSE);
  info->buf_index = info->buf_index_save;

  /* Number of interoperability */
  count = INTEROP_IFD_FIELD_COUNT;

  if (exif_emit_2bytes(info, count) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->interop_ifd.interop_index) == FALSE)
    return(FALSE);
  if (exif_emit_tiff_tag(info, &info->interop_ifd.exifR98_version) == FALSE)
    return(FALSE);
  /* Next IFD (none) offset */
  if (exif_emit_4bytes(info, 0) == FALSE)
    return(FALSE);
  if (info->gps_set)
  {
    /* Emit GPS IFD */
    info->gps_set = FALSE;
    /* Make sure the index is at 2-byte boundary. */
    odd = info->buf_index & 1;
    if (odd)
    {
      if (exif_emit_byte(info, 0) == FALSE) return(FALSE);
    }
    /* Update GPS_IFD pointer */
    info->buf_index_save = info->buf_index;
    info->buf_index = info->gps_ifd_index + 8;
    if (exif_emit_4bytes(info, info->buf_index_save - info->tiff_header_index)
        == FALSE) return(FALSE);
    info->buf_index = info->buf_index_save;
    /* Number of interoperability */
    count = GPS_IFD_FIELD_COUNT;
    if (info->gps_ifd.area_information.count == 0) count--;
    if (exif_emit_2bytes(info, count) == FALSE)
      return(FALSE);
    /* Strings should follow all the tag fields and next exif offset */
    info->string_index = info->buf_index + EXIF_TAG_SIZE * count + 4;
    if (exif_emit_tiff_tag(info, &info->gps_ifd.version_id) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->gps_ifd.latitude_ref) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->gps_ifd.latitude) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->gps_ifd.longitude_ref) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->gps_ifd.longitude) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->gps_ifd.altitude_ref) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->gps_ifd.altitude) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->gps_ifd.timestamp) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->gps_ifd.map_datum) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->gps_ifd.process_method) == FALSE)
      return(FALSE);
    if (info->gps_ifd.area_information.count)
    {
      if (exif_emit_tiff_tag(info, &info->gps_ifd.area_information) == FALSE)
        return(FALSE);
      info->gps_ifd.area_information.count = 0;
      JPEG_FREE(info->gps_ifd.area_information.offset);
      info->gps_ifd.area_information.offset = NULL;
    }
    if (exif_emit_tiff_tag(info, &info->gps_ifd.datestamp) == FALSE)
      return(FALSE);
    /* Next IFD (none) offset */
    if (exif_emit_4bytes(info, 0) == FALSE)
      return(FALSE);
    /* Update buffer index so it will continue from the end of string area */
    info->buf_index = info->string_index;
  }

  if (info->emit_thumbnail)
  {
    /* Emit thumbnail */
    /* Make sure the index is at 2-byte boundary. */
    odd = info->buf_index & 1;
    if (odd)
    {
      if (exif_emit_byte(info, 0) == FALSE) return(FALSE);
    }

    /* Update THUMBNAIL_IFD pointer */
    info->buf_index_save = info->buf_index;
    info->buf_index = info->thumbnail_ifd_index;
    if (exif_emit_4bytes(info, info->buf_index_save - info->tiff_header_index)
        == FALSE) return(FALSE);
    info->buf_index = info->buf_index_save;
    
    /* save the start of thumbnail */
    info->thumbnail_index = info->buf_index_save;


    /* Number of interoperability */
    count = THUMBNAIL_IFD_FIELD_COUNT;
    if (exif_emit_2bytes(info, count) == FALSE)
      return(FALSE);
    /* Strings should follow all the tag fields and next exif offset */
    info->string_index = info->buf_index + EXIF_TAG_SIZE * count + 4;
    if (exif_emit_tiff_tag(info, &info->thumbnail_ifd.compression) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->thumbnail_ifd.x_resolution) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->thumbnail_ifd.y_resolution) == FALSE)
      return(FALSE);
    if (exif_emit_tiff_tag(info, &info->thumbnail_ifd.resolution_unit) == FALSE)
      return(FALSE);
    info->jpegi_format_index = info->buf_index;
    if (exif_emit_tiff_tag(info, &info->thumbnail_ifd.jpegi_format) == FALSE)
      return(FALSE);
    info->jpegi_format_l_index = info->buf_index;
    if (exif_emit_tiff_tag(info, &info->thumbnail_ifd.jpegi_format_l) == FALSE)
      return(FALSE);
    /* Next IFD (none) offset */
    if (exif_emit_4bytes(info, 0) == FALSE)
      return(FALSE);
    /* Update buffer index so it will continue from the end of string area */
    info->thumbnail_stream_index = info->string_index;
    info->buf_index = info->thumbnail_stream_index;

    /* Emit thumbnail JPEG header */
    if (exif_emit_soi(info) == FALSE) return(FALSE);
    if (exif_write_frame_header(info) == FALSE) return(FALSE);
    if (exif_emit_sos(info) == FALSE) return(FALSE);

    /* Update jpegi_format */
    info->buf_index_save = info->buf_index;
    info->buf_index = info->jpegi_format_index + 8;
    if (exif_emit_4bytes(info, info->thumbnail_stream_index - info->tiff_header_index) == FALSE)
      return(FALSE);
    info->buf_index = info->buf_index_save;
  }

  /* Now update the APP1 length field */
  info->buf_index_save = info->buf_index;
  info->buf_index = info->length_index;
  if (exif_emit_2bytes(info, (uint16)(info->buf_index_save - info->length_index)) == FALSE)
    return(FALSE);
  info->buf_index = info->buf_index_save;
  return(TRUE);
} /* exif_emit_app1 */

/*===========================================================================
FUNCTION  EXIF_WRITE_HUFF_TABLES

DESCRIPTION
  Write scan header.
  This consists of DHT or DAC markers, optional DRI, and SOS.
  Compressed data will be written following the SOS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean exif_write_huff_tables (exif_info_type *info)
{
  uint16 i, j, length;
  jpeg_component_info *compptr;
  JHUFF_TBL * htbl;

  for (i = 0; i < NUM_HUFF_TBL; i++)
  {
    exif_huffman_tbl[i].sent = FALSE;
  }

  /* Emit Huffman tables.
   * Note that exif_emit_dht() suppresses any duplicate tables.
   */

  if (exif_emit_marker(info, M_DHT) == FALSE) return(FALSE);

  length = 0;
  for (j=0; j<2; j++)
  {
    htbl = info->dc_huff_tbl_ptrs[j];
    for (i = 1; i <= 16; i++)
      length += htbl->bits[i];

    htbl = info->ac_huff_tbl_ptrs[j];
    for (i = 1; i <= 16; i++)
      length += htbl->bits[i];
  }

  if (exif_emit_2bytes(info, 2 + length + 17*4) == FALSE) return(FALSE);

  for (i = 0, compptr = info->comp_info; i < info->num_components;
      i++, compptr++)
  {
    /* Sequential mode: need both DC and AC tables */
    if (exif_emit_dht(info, compptr->dc_tbl_no, FALSE) == FALSE) return(FALSE);
    if (exif_emit_dht(info, compptr->ac_tbl_no, TRUE) == FALSE) return(FALSE);
  }
  return(TRUE);
}

/*===========================================================================
FUNCTION  EXIF_WRIRE_FRAME_HEADER

DESCRIPTION
  Write frame header.
  This consists of DQT and SOFn markers.
  Note that we do not emit the SOF until we have emitted the DQT(s).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean exif_write_frame_header (exif_info_type *info)
{
  uint16 ci;
  jpeg_component_info *compptr;

  /* Emit one DQT for all necessary quantization tables.
   * Note that exif_emit_dqt() suppresses any duplicate tables.
   */
  if (exif_emit_marker(info, M_DQT) == FALSE) return(FALSE);

  if (exif_emit_2bytes(info, DCTSIZE2*2 + 2 + 2) == FALSE)
    return(FALSE);

  for (ci = 0, compptr = info->comp_info; ci < (info->num_components - 1);
      ci++, compptr++)
  {
    if (exif_emit_byte(info, compptr->quant_tbl_no) == FALSE) return(FALSE);
    if (exif_emit_dqt(info, compptr->quant_tbl_no) == FALSE) return(FALSE);
  }
  if (exif_write_huff_tables(info) == FALSE) return(FALSE);
  if (exif_emit_sof(info, M_SOF0) == FALSE) return(FALSE);
  return(TRUE);
}

/*===========================================================================
FUNCTION  EXIF_ADD_HUFF_TABLE

DESCRIPTION
  Define a Huffman table

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void exif_add_huff_table
(
JHUFF_TBL *htblptr, const uint8 *bits, const uint8 *val
)
{
  uint32 nsymbols, len;

  memcpy(htblptr->bits, bits, sizeof(htblptr->bits));

  nsymbols = 0;
  for (len = 1; len <= 16; len++)
    nsymbols += bits[len];
  memcpy(htblptr->huffval, val, nsymbols * sizeof(uint8));
}

/*===========================================================================
FUNCTION  EXIF_STD_HUFF_TABLES

DESCRIPTION
  Set up the standard Huffman tables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
IMPORTANT: these are only valid for 8-bit data precision! 
===========================================================================*/
static void exif_std_huff_tables (exif_info_type *info)
/* IMPORTANT: these are only valid for 8-bit data precision! */
{
  static const uint8 bits_dc_luminance[17] =
  { /* 0-base */ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
  static const uint8 val_dc_luminance[] =
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

  static const uint8 bits_dc_chrominance[17] =
  { /* 0-base */ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
  static const uint8 val_dc_chrominance[] =
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

  static const uint8 bits_ac_luminance[17] =
  { /* 0-base */ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d};
  static const uint8 val_ac_luminance[] =
  { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa};

  static const uint8 bits_ac_chrominance[17] =
  { /* 0-base */ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77};
  static const uint8 val_ac_chrominance[] =
  { 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa};
  /* Init memory tables allocation */
  info->dc_huff_tbl_ptrs[0] = &exif_huffman_tbl[0];
  info->dc_huff_tbl_ptrs[1] = &exif_huffman_tbl[1];
  info->ac_huff_tbl_ptrs[0] = &exif_huffman_tbl[2];
  info->ac_huff_tbl_ptrs[1] = &exif_huffman_tbl[3];

  exif_add_huff_table(info->dc_huff_tbl_ptrs[0],
                      bits_dc_luminance, val_dc_luminance);
  exif_add_huff_table(info->ac_huff_tbl_ptrs[0],
                      bits_ac_luminance, val_ac_luminance);
  exif_add_huff_table(info->dc_huff_tbl_ptrs[1],
                      bits_dc_chrominance, val_dc_chrominance);
  exif_add_huff_table(info->ac_huff_tbl_ptrs[1],
                      bits_ac_chrominance, val_ac_chrominance);
}

/*===========================================================================
FUNCTION  EXIF_JPEG_SET_COLORSPACE

DESCRIPTION
  Set the JPEG colorspace, and choose colorspace-dependent default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void exif_jpeg_set_colorspace
(
exif_info_type *info,
exif_color_space_type colorspace
)
{
  jpeg_component_info * compptr;
  uint8  subsample_H=0, subsample_V=0;

#define SET_COMP(index,id,hsamp,vsamp,quant,dctbl,actbl)  \
  (compptr = &info->comp_info[index], \
   compptr->component_id = (id), \
   compptr->h_samp_factor = (hsamp), \
   compptr->v_samp_factor = (vsamp), \
   compptr->quant_tbl_no = (quant), \
   compptr->dc_tbl_no = (dctbl), \
   compptr->ac_tbl_no = (actbl) )

  /* For all colorspaces, we use Q and Huff tables 0 for luminance components,
     tables 1 for chrominance components.*/
  info->color_space = colorspace;
  info->write_JFIF_header = FALSE; /* No marker for non-JFIF colorspaces */

  switch (colorspace)
  {
    case EXIF_GRAYSCALE:
      info->write_JFIF_header = TRUE; /* Write a JFIF marker */
      info->num_components = 1;
      /* JFIF specifies component ID 1 */
      SET_COMP(0, 1, 1,1, 0, 0,0);
      break;
    case EXIF_YCbCr:
      info->write_JFIF_header = TRUE; /* Write a JFIF marker */
      info->num_components = 3;
      /* JFIF specifies component IDs 1,2,3 */
      switch (info->subsampling)
      {
        case EXIF_H1V1:
          subsample_H = 1;
          subsample_V = 1;
          break;
        case EXIF_H1V2:
          subsample_H = 1;
          subsample_V = 2;
          break;
        case EXIF_H2V1:
          subsample_H = 2;
          subsample_V = 1;
          break;
        case EXIF_H2V2:
          subsample_H = 2;
          subsample_V = 2;
          break;
        default: break;
      }
      /* Set the subsampling for this SOF */
      SET_COMP(0, 1, subsample_H, subsample_V, 0, 0,0);
      SET_COMP(1, 2, 1,1, 1, 1,1);
      SET_COMP(2, 3, 1,1, 1, 1,1);
      break;
    default: break;
  }
}
/*===========================================================================

FUNCTION      EXIF_INIT_INFO

DESCRIPTION
              Initialize the EXIF info structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void exif_init_info(exif_info_type * info)
{

  char * strPtr;
  char * software_strPtr;
  uint32 strSize; 


  info->code1 = CODE1;
  info->code2 = CODE2;
  info->code3 = CODE3;
  info->code4 = CODE4;
  info->buf_index = 0;

  /* ZERO IFD */
  /* Variable length - no default value */
  info->zero_ifd.image_description.id = (uint16)IMAGE_DESCRIPTION;
  info->zero_ifd.image_description.type = (uint16)EXIF_ASCII;
  info->zero_ifd.image_description.count = 0;
  info->zero_ifd.image_description.offset = 0;
  info->zero_ifd.image_description.fixedLength = FALSE;
  info->zero_ifd.image_description.active = TRUE;

  /* Variable length */
  info->zero_ifd.make.id = (uint16)MAKE;
  info->zero_ifd.make.type = (uint16)EXIF_ASCII;
  strSize = strlen(exif_make_string)+1;
  info->zero_ifd.make.count = strSize;
  strPtr = JPEG_MALLOC(strSize);
  info->zero_ifd.make.active = FALSE;
  if (strPtr)
  {
    memcpy(strPtr, exif_make_string, strSize);
    info->zero_ifd.make.offset = strPtr;
    info->zero_ifd.make.active = TRUE;
  }
  info->zero_ifd.make.fixedLength = FALSE;


  /* Variable Length */
  info->zero_ifd.model.id = (uint16)MODEL;
  info->zero_ifd.model.type = (uint16)EXIF_ASCII;
  info->zero_ifd.model.count = 8;
  info->zero_ifd.model.offset = JPEG_MALLOC(info->zero_ifd.model.count);
  info->zero_ifd.model.active = FALSE;
  if (info->zero_ifd.model.offset != NULL)
  {
    memcpy(info->zero_ifd.model.offset, exif_model_string, info->zero_ifd.model.count);
    info->zero_ifd.model.active = TRUE;
  }
  info->zero_ifd.model.fixedLength = FALSE;

#ifdef FEATURE_EXIF_PIM3
  info->zero_ifd.pim.id = (uint16)PIM_TAG;
  info->zero_ifd.pim.type = (uint16)EXIF_UNDEFINED;
  info->zero_ifd.pim.count = 0; 
  info->zero_ifd.pim.offset = NULL; 
  info->zero_ifd.pim.active = TRUE;
#endif

  /* Fixed length  */
  info->zero_ifd.orientation.id = (uint16)ORIENTATION;
  info->zero_ifd.orientation.type = (uint16)EXIF_SHORT;
  info->zero_ifd.orientation.count = 1;
  exif_orientation = 1;
  info->zero_ifd.orientation.offset = &exif_orientation;
  info->zero_ifd.orientation.fixedLength = TRUE;
  info->zero_ifd.orientation.active = TRUE;

  /* Fixed length */
  info->zero_ifd.x_resolution.id = (uint16)X_RESOLUTION;
  info->zero_ifd.x_resolution.type = (uint16)EXIF_RATIONAL;
  info->zero_ifd.x_resolution.count = 1;
  info->zero_ifd.x_resolution.offset = &exif_x_resolution;
  info->zero_ifd.x_resolution.fixedLength = TRUE;
  info->zero_ifd.x_resolution.active = TRUE;

  /* Fixed length */
  info->zero_ifd.y_resolution.id = (uint16)Y_RESOLUTION;
  info->zero_ifd.y_resolution.type = (uint16)EXIF_RATIONAL;
  info->zero_ifd.y_resolution.count = 1;
  info->zero_ifd.y_resolution.offset = &exif_y_resolution;
  info->zero_ifd.y_resolution.fixedLength = TRUE;
  info->zero_ifd.y_resolution.active = TRUE;

  /* Fixed length */ 
  info->zero_ifd.resolution_unit.id = (uint16)RESOLUTION_UNIT;
  info->zero_ifd.resolution_unit.type = (uint16)EXIF_SHORT;
  info->zero_ifd.resolution_unit.count = 1;
  exif_resolution = 2;
  info->zero_ifd.resolution_unit.offset = &exif_resolution;
  info->zero_ifd.resolution_unit.fixedLength = TRUE;
  info->zero_ifd.resolution_unit.active = TRUE;

  /* Variable Length */
  info->zero_ifd.software.id = (uint16)SOFTWARE;
  info->zero_ifd.software.type = (uint16)EXIF_ASCII;
  strSize = strlen(mob_sw_rev)+1;
  info->zero_ifd.software.count = strSize;
  software_strPtr = JPEG_MALLOC(strSize);
  info->zero_ifd.software.active = FALSE;  
  if (software_strPtr)
  {
    memcpy(software_strPtr, mob_sw_rev, strSize);
    info->zero_ifd.software.offset = software_strPtr;
    info->zero_ifd.software.active = TRUE;
  }
  info->zero_ifd.software.fixedLength = FALSE;


  /* Fixed length */ 
  info->zero_ifd.ycbcr_positioning.id = (uint16)YCBCR_POSITIONING;
  info->zero_ifd.ycbcr_positioning.type = (uint16)EXIF_SHORT;
  info->zero_ifd.ycbcr_positioning.count = 1;
  exif_ycbcr_positioning = (int16)YCBCR_CENTER;
  info->zero_ifd.ycbcr_positioning.offset = &exif_ycbcr_positioning;
  info->zero_ifd.ycbcr_positioning.fixedLength = TRUE;
  info->zero_ifd.ycbcr_positioning.active = TRUE;

  /* Fixed length */ 
  info->zero_ifd.exif_ifd_pointer.id = (uint16)EXIF_IFD_POINTER;
  info->zero_ifd.exif_ifd_pointer.type = (uint16)EXIF_LONG;
  info->zero_ifd.exif_ifd_pointer.count = 1;
  exif_ifd_pointer = 0;
  info->zero_ifd.exif_ifd_pointer.offset = &exif_ifd_pointer;
  info->zero_ifd.exif_ifd_pointer.fixedLength = TRUE;
  info->zero_ifd.exif_ifd_pointer.active = TRUE;

  /* Fixed length */ 
  info->zero_ifd.gps_ifd_pointer.id = (uint16)GPS_IFD_POINTER;
  info->zero_ifd.gps_ifd_pointer.type = (uint16)EXIF_LONG;
  info->zero_ifd.gps_ifd_pointer.count = 1;
  gps_ifd_pointer = 0;
  info->zero_ifd.gps_ifd_pointer.offset = &gps_ifd_pointer;
  info->zero_ifd.gps_ifd_pointer.fixedLength = TRUE;
  info->zero_ifd.gps_ifd_pointer.active = TRUE;

  /* EXIF_IFD */
  /* Fixed length */
  info->exif_ifd.exposure_time.id = (uint16)EXPOSURE_TIME;
  info->exif_ifd.exposure_time.type = (uint16)EXIF_RATIONAL;
  info->exif_ifd.exposure_time.count = 1;
  info->exif_ifd.exposure_time.offset = &exif_exposure_time;
  info->exif_ifd.exposure_time.fixedLength = TRUE;
  info->exif_ifd.exposure_time.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.version.id = (uint16)EXIF_VERSION;
  info->exif_ifd.version.type = (uint16)EXIF_UNDEFINED;
  info->exif_ifd.version.count = 4;
  info->exif_ifd.version.offset = exif_version_string;
  info->exif_ifd.version.fixedLength = TRUE;
  info->exif_ifd.version.active = TRUE;

  /* Fixed length 20 bytes */
  info->exif_ifd.date_time_original.id = (uint16)EXIF_DATE_TIME_ORIGINAL;
  info->exif_ifd.date_time_original.type = (uint16)EXIF_ASCII;
  info->exif_ifd.date_time_original.count = 20;
  info->exif_ifd.date_time_original.offset = exif_date_time_string;
  info->exif_ifd.date_time_original.fixedLength = TRUE;
  info->exif_ifd.date_time_original.active = TRUE;
  set_date_time = TRUE;
  
  /* Fixed length 20 bytes */
  info->exif_ifd.date_time_digitized.id = (uint16)EXIF_DATE_TIME_DIGITIZED;
  info->exif_ifd.date_time_digitized.type = (uint16)EXIF_ASCII;
  info->exif_ifd.date_time_digitized.count = 20;
  info->exif_ifd.date_time_digitized.offset = exif_date_time_string;
  info->exif_ifd.date_time_digitized.fixedLength = TRUE;
  info->exif_ifd.date_time_digitized.active = TRUE;

  /* Fixed length */ 
  info->exif_ifd.components_configuration.id = (uint16)EXIF_COMPONENTS_CONFIGURATION;
  info->exif_ifd.components_configuration.type = (uint16)EXIF_UNDEFINED;
  info->exif_ifd.components_configuration.count = 4;
  info->exif_ifd.components_configuration.offset = exif_components_configuration_string;
  info->exif_ifd.components_configuration.fixedLength = TRUE;
  info->exif_ifd.components_configuration.active = TRUE;

  /* Fixed length */
  info->exif_ifd.flash.id = (uint16)FLASH;
  info->exif_ifd.flash.type = (uint16)EXIF_SHORT;
  info->exif_ifd.flash.count = 1;
  exif_flash = 1;
  info->exif_ifd.flash.offset = &exif_flash;
  info->exif_ifd.flash.fixedLength = TRUE;
  info->exif_ifd.flash.active = FALSE;

  /* Variable Length - no default value */
  info->exif_ifd.user_comment.id = (uint16)EXIF_USER_COMMENT;
  info->exif_ifd.user_comment.type = (uint16)EXIF_UNDEFINED;
  info->exif_ifd.user_comment.count = 0;
  info->exif_ifd.user_comment.offset = 0;
  info->exif_ifd.user_comment.fixedLength = FALSE;
  info->exif_ifd.user_comment.active = TRUE;

  /* Variable Length - no default value */
  info->exif_ifd.maker_note.id = (uint16)EXIF_MAKER_NOTE;
  info->exif_ifd.maker_note.type = (uint16)EXIF_UNDEFINED;
  info->exif_ifd.maker_note.count = 0;
  info->exif_ifd.maker_note.offset = 0;
  info->exif_ifd.maker_note.fixedLength = FALSE;
  info->exif_ifd.maker_note.active = TRUE;

  /* Fixed length  */
  info->exif_ifd.flashpix_version.id = (uint16)EXIF_FLASHPIX_VERSION;
  info->exif_ifd.flashpix_version.type = (uint16)EXIF_UNDEFINED;
  info->exif_ifd.flashpix_version.count = 4;
  info->exif_ifd.flashpix_version.offset = exif_flashpix_version_string;
  info->exif_ifd.flashpix_version.fixedLength = TRUE;
  info->exif_ifd.flashpix_version.active = TRUE;

  /* Fixed length */
  info->exif_ifd.color_space.id = (uint16)EXIF_COLOR_SPACE;
  info->exif_ifd.color_space.type = (uint16)EXIF_SHORT;
  info->exif_ifd.color_space.count = 1;
  exif_color_space = (int16)COLOR_SPACE_SRGB;
  info->exif_ifd.color_space.offset = &exif_color_space;
  info->exif_ifd.color_space.fixedLength = TRUE;
  info->exif_ifd.color_space.active = TRUE;

  /* Fixed length */
  info->exif_ifd.pixel_x_dimension.id = (uint16)EXIF_PIXEL_X_DIMENSION;
  info->exif_ifd.pixel_x_dimension.type = (uint16)EXIF_LONG;
  info->exif_ifd.pixel_x_dimension.count = 1;
  exif_pixel_x_dimension = 0;
  info->exif_ifd.pixel_x_dimension.offset = &exif_pixel_x_dimension;
  info->exif_ifd.pixel_x_dimension.fixedLength = TRUE;
  info->exif_ifd.pixel_x_dimension.active = TRUE;

  /* Fixed length  */
  info->exif_ifd.pixel_y_dimension.id = (uint16)EXIF_PIXEL_Y_DIMENSION;
  info->exif_ifd.pixel_y_dimension.type = (uint16)EXIF_LONG;
  info->exif_ifd.pixel_y_dimension.count = 1;
  exif_pixel_y_dimension = 0;
  info->exif_ifd.pixel_y_dimension.offset = &exif_pixel_y_dimension;
  info->exif_ifd.pixel_y_dimension.fixedLength = TRUE;
  info->exif_ifd.pixel_y_dimension.active = TRUE;

  /* Fixed length */ 
  info->exif_ifd.interop_ifd_pointer.id = (uint16)INTEROP_IFD_POINTER;
  info->exif_ifd.interop_ifd_pointer.type = (uint16)EXIF_LONG;
  info->exif_ifd.interop_ifd_pointer.count = 1;
  interop_ifd_pointer = 0;
  info->exif_ifd.interop_ifd_pointer.offset = &interop_ifd_pointer;
  info->exif_ifd.interop_ifd_pointer.fixedLength = TRUE;
  info->exif_ifd.interop_ifd_pointer.active = TRUE;

  /* Fixed length */ 
  info->exif_ifd.custom_rendered.id = (uint16)CUSTOM_RENDERED;
  info->exif_ifd.custom_rendered.type = (uint16)EXIF_SHORT;
  info->exif_ifd.custom_rendered.count = 1;
  exif_custom_rendered = 0;
  info->exif_ifd.custom_rendered.offset = &exif_custom_rendered;
  info->exif_ifd.custom_rendered.fixedLength = TRUE;
  info->exif_ifd.custom_rendered.active = FALSE;

  /* Fixed length */
  info->exif_ifd.exposure_mode.id = (uint16)EXPOSURE_MODE;
  info->exif_ifd.exposure_mode.type = (uint16)EXIF_SHORT;
  info->exif_ifd.exposure_mode.count = 1;
  exif_exposure_mode = 0;
  info->exif_ifd.exposure_mode.offset = &exif_exposure_mode;
  info->exif_ifd.exposure_mode.fixedLength = TRUE;
  info->exif_ifd.exposure_mode.active = FALSE;

  /* Fixed length */
  info->exif_ifd.white_balance.id = (uint16)WHITE_BALANCE;
  info->exif_ifd.white_balance.type = (uint16)EXIF_SHORT;
  info->exif_ifd.white_balance.count = 1;
  exif_white_balance = 0;
  info->exif_ifd.white_balance.offset = &exif_white_balance;
  info->exif_ifd.white_balance.fixedLength = TRUE;
  info->exif_ifd.white_balance.active = FALSE;

  /* Fixed length */
  info->exif_ifd.digital_zoom_ratio.id = (uint16)DIGITAL_ZOOM_RATIO;
  info->exif_ifd.digital_zoom_ratio.type = (uint16)EXIF_RATIONAL;
  info->exif_ifd.digital_zoom_ratio.count = 1;
  info->exif_ifd.digital_zoom_ratio.offset = &exif_digital_zoom_ratio;
  info->exif_ifd.digital_zoom_ratio.fixedLength = TRUE;
  info->exif_ifd.digital_zoom_ratio.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.scene_capture_type.id = (uint16)SCENE_CAPTURE_TYPE;
  info->exif_ifd.scene_capture_type.type = (uint16)EXIF_SHORT;
  info->exif_ifd.scene_capture_type.count = 1;
  exif_scene_capture_type = 0;
  info->exif_ifd.scene_capture_type.offset = &exif_scene_capture_type;
  info->exif_ifd.scene_capture_type.fixedLength = TRUE;
  info->exif_ifd.scene_capture_type.active = FALSE;

  /* Fixed length  */
  info->exif_ifd.subject_distance_range.id = (uint16)SUBJECT_DISTANCE_RANGE;
  info->exif_ifd.subject_distance_range.type = (uint16)EXIF_SHORT;
  info->exif_ifd.subject_distance_range.count = 1;
  exif_subject_distance_range = 0;
  info->exif_ifd.subject_distance_range.offset = &exif_subject_distance_range;
  info->exif_ifd.subject_distance_range.fixedLength = TRUE;
  info->exif_ifd.subject_distance_range.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.f_number.id = (uint16)F_NUMBER;
  info->exif_ifd.f_number.type = (uint16)EXIF_RATIONAL;
  info->exif_ifd.f_number.count = 1;
  exif_f_number.numerator = 1;
  exif_f_number.denominator = 1;
  info->exif_ifd.f_number.offset = &exif_f_number;
  info->exif_ifd.f_number.fixedLength = TRUE;
  info->exif_ifd.f_number.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.aperture.id = (uint16)APERTURE;
  info->exif_ifd.aperture.type = (uint16)EXIF_RATIONAL;
  info->exif_ifd.aperture.count = 1;
  exif_aperture.numerator = 1;
  exif_aperture.denominator = 1;
  info->exif_ifd.aperture.offset = &exif_aperture;
  info->exif_ifd.aperture.fixedLength = TRUE;
  info->exif_ifd.aperture.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.max_aperture.id = (uint16)MAX_APERTURE;
  info->exif_ifd.max_aperture.type = (uint16)EXIF_RATIONAL;
  info->exif_ifd.max_aperture.count = 1;
  exif_max_aperture.numerator = 1;
  exif_max_aperture.denominator = 1;
  info->exif_ifd.max_aperture.offset = &exif_max_aperture;
  info->exif_ifd.max_aperture.fixedLength = TRUE;
  info->exif_ifd.max_aperture.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.focal_length.id = (uint16)FOCAL_LENGTH;
  info->exif_ifd.focal_length.type = (uint16)EXIF_RATIONAL;
  info->exif_ifd.focal_length.count = 1;
  exif_focal_length.numerator = 1;
  exif_focal_length.denominator = 1;
  info->exif_ifd.focal_length.offset = &exif_focal_length;
  info->exif_ifd.focal_length.fixedLength = TRUE;
  info->exif_ifd.focal_length.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.focal_length_35mm.id = (uint16)FOCAL_LENGTH_35MM;
  info->exif_ifd.focal_length_35mm.type = (uint16)EXIF_SHORT;
  info->exif_ifd.focal_length_35mm.count = 1;
  exif_focal_length_35mm = 1;
  info->exif_ifd.focal_length_35mm.offset = &exif_focal_length_35mm;
  info->exif_ifd.focal_length_35mm.fixedLength = TRUE;
  info->exif_ifd.focal_length_35mm.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.gain_control.id = (uint16)GAIN_CONTROL;
  info->exif_ifd.gain_control.type = (uint16)EXIF_SHORT;
  info->exif_ifd.gain_control.count = 1;
  exif_gain_control= 1;
  info->exif_ifd.gain_control.offset = &exif_gain_control;
  info->exif_ifd.gain_control.fixedLength = TRUE;
  info->exif_ifd.gain_control.active = FALSE;

  /* Fixed length  */
  info->exif_ifd.exposure_program.id = (uint16)EXPOSURE_PROGRAM;
  info->exif_ifd.exposure_program.type = (uint16)EXIF_SHORT;
  info->exif_ifd.exposure_program.count = 1;
  exif_exposure_program = 1;
  info->exif_ifd.exposure_program.offset = &exif_exposure_program;
  info->exif_ifd.exposure_program.fixedLength = TRUE;
  info->exif_ifd.exposure_program.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.iso_speed_ratings.id = (uint16)ISO_SPEED_RATINGS;
  info->exif_ifd.iso_speed_ratings.type = (uint16)EXIF_SHORT;
  info->exif_ifd.iso_speed_ratings.count = 1;
  info->exif_ifd.iso_speed_ratings.offset = &iso_speed_ratings;
  info->exif_ifd.iso_speed_ratings.fixedLength = TRUE;
  info->exif_ifd.iso_speed_ratings.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.light_source.id = (uint16)LIGHT_SOURCE;
  info->exif_ifd.light_source.type = (uint16)EXIF_SHORT;
  info->exif_ifd.light_source.count = 1;
  exif_light_source = 1;
  info->exif_ifd.light_source.offset = &exif_light_source;
  info->exif_ifd.light_source.fixedLength = TRUE;
  info->exif_ifd.light_source.active = FALSE;

  /* Fixed length  */
  info->exif_ifd.sensing_method.id = (uint16)SENSING_METHOD;
  info->exif_ifd.sensing_method.type = (uint16)EXIF_SHORT;
  info->exif_ifd.sensing_method.count = 1;
  exif_sensing_method = 1;
  info->exif_ifd.sensing_method.offset = &exif_sensing_method;
  info->exif_ifd.sensing_method.fixedLength = TRUE;
  info->exif_ifd.sensing_method.active = FALSE;


  /* Fixed length */ 
  info->exif_ifd.file_source.id = (uint16)FILE_SOURCE;
  info->exif_ifd.file_source.type = (uint16)EXIF_UNDEFINED;
  info->exif_ifd.file_source.count = 1;
  exif_file_source = 3;
  info->exif_ifd.file_source.offset = &exif_file_source;
  info->exif_ifd.file_source.fixedLength = TRUE;
  info->exif_ifd.file_source.active = TRUE;

  /* Fixed length */ 
  info->exif_ifd.scene_type.id = (uint16)SCENE_TYPE;
  info->exif_ifd.scene_type.type = (uint16)EXIF_UNDEFINED;
  info->exif_ifd.scene_type.count = 1;
  exif_scene_type = 1;
  info->exif_ifd.scene_type.offset = &exif_scene_type;
  info->exif_ifd.scene_type.fixedLength = TRUE;
  info->exif_ifd.scene_type.active = TRUE;

  /* Fixed length */ 
  info->exif_ifd.contrast.id = (uint16)CONTRAST;
  info->exif_ifd.contrast.type = (uint16)EXIF_SHORT;
  info->exif_ifd.contrast.count = 1;
  exif_contrast = 1;
  info->exif_ifd.contrast.offset = &exif_contrast;
  info->exif_ifd.contrast.fixedLength = TRUE;
  info->exif_ifd.contrast.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.saturation.id = (uint16)SATURATION;
  info->exif_ifd.saturation.type = (uint16)EXIF_SHORT;
  info->exif_ifd.saturation.count = 1;
  exif_saturation = 1;
  info->exif_ifd.saturation.offset = &exif_saturation;
  info->exif_ifd.saturation.fixedLength = TRUE;
  info->exif_ifd.saturation.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.metering_mode.id = (uint16)METERING_MODE;
  info->exif_ifd.metering_mode.type = (uint16)EXIF_SHORT;
  info->exif_ifd.metering_mode.count = 1;
  exif_metering_mode = 1;
  info->exif_ifd.metering_mode.offset = &exif_metering_mode;
  info->exif_ifd.metering_mode.fixedLength = TRUE;
  info->exif_ifd.metering_mode.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.sharpness.id = (uint16)SHARPNESS;
  info->exif_ifd.sharpness.type = (uint16)EXIF_SHORT;
  info->exif_ifd.sharpness.count = 1;
  exif_sharpness = 1;
  info->exif_ifd.sharpness.offset = &exif_sharpness;
  info->exif_ifd.sharpness.fixedLength = TRUE;
  info->exif_ifd.sharpness.active = FALSE;

  /* Variable length - no default value */
  info->exif_ifd.cfa_pattern.id = (uint16)CFA_PATTERN;
  info->exif_ifd.cfa_pattern.type = (uint16)EXIF_UNDEFINED;
  info->exif_ifd.cfa_pattern.count = 0;
  info->exif_ifd.cfa_pattern.offset = 0;
  info->exif_ifd.cfa_pattern.fixedLength = FALSE;
  info->exif_ifd.cfa_pattern.active = FALSE;

  /* Fixed Length */
  info->exif_ifd.oecf.id = (uint16)OECF;
  info->exif_ifd.oecf.type = (uint16)EXIF_UNDEFINED;
  info->exif_ifd.oecf.count = 1;
  exif_oecf = 1;
  info->exif_ifd.oecf.offset = &exif_oecf;
  info->exif_ifd.oecf.fixedLength = TRUE;
  info->exif_ifd.oecf.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.shutter_speed.id = (uint16)SHUTTER_SPEED;
  info->exif_ifd.shutter_speed.type = (uint16)EXIF_SRATIONAL;
  info->exif_ifd.shutter_speed.count = 1;
  exif_shutter_speed.numerator = 1;
  exif_shutter_speed.denominator = 1;
  info->exif_ifd.shutter_speed.offset = &exif_shutter_speed;
  info->exif_ifd.shutter_speed.fixedLength = TRUE;
  info->exif_ifd.shutter_speed.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.brightness.id = (uint16)BRIGHTNESS;
  info->exif_ifd.brightness.type = (uint16)EXIF_SRATIONAL;
  info->exif_ifd.brightness.count = 1;
  exif_brightness.numerator = 1;
  exif_brightness.denominator = 1;
  info->exif_ifd.brightness.offset = &exif_brightness;
  info->exif_ifd.brightness.fixedLength = TRUE;
  info->exif_ifd.brightness.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.exposure_index.id = (uint16)EXPOSURE_INDEX;
  info->exif_ifd.exposure_index.type = (uint16)EXIF_RATIONAL;
  info->exif_ifd.exposure_index.count = 1;
  exif_exposure_index.numerator = 1;
  exif_exposure_index.denominator = 1;
  info->exif_ifd.exposure_index.offset = &exif_exposure_index;
  info->exif_ifd.exposure_index.fixedLength = TRUE;
  info->exif_ifd.exposure_index.active = FALSE;

  /* Fixed length  */
  info->exif_ifd.subject_distance.id = (uint16)SUBJECT_DISTANCE;
  info->exif_ifd.subject_distance.type = (uint16)EXIF_RATIONAL;
  info->exif_ifd.subject_distance.count = 1;
  exif_subject_distance.numerator = 1;
  exif_subject_distance.denominator = 1;
  info->exif_ifd.subject_distance.offset = &exif_subject_distance;
  info->exif_ifd.subject_distance.fixedLength = TRUE;
  info->exif_ifd.subject_distance.active = FALSE;

  /* Fixed length */ 
  info->exif_ifd.subject_location.id = (uint16)SUBJECT_LOCATION;
  info->exif_ifd.subject_location.type = (uint16)EXIF_SHORT;
  info->exif_ifd.subject_location.count = 1;
  exif_subject_location = 1;
  info->exif_ifd.subject_location.offset = &exif_subject_location;
  info->exif_ifd.subject_location.fixedLength = TRUE;
  info->exif_ifd.subject_location.active = FALSE;

  /* Fixed length  */
  info->interop_ifd.interop_index.id = (uint16)INTEROP_INDEX;
  info->interop_ifd.interop_index.type = (uint16)EXIF_ASCII;
  info->interop_ifd.interop_index.count = 4;
  info->interop_ifd.interop_index.offset = exif_interop_index_string;
  info->interop_ifd.interop_index.fixedLength = TRUE;
  info->interop_ifd.interop_index.active = TRUE;

  /* Fixed length */ 
  info->interop_ifd.exifR98_version.id = (uint16)EXIFR98_VERSION;
  info->interop_ifd.exifR98_version.type = (uint16)EXIF_UNDEFINED;
  info->interop_ifd.exifR98_version.count = 4;
  info->interop_ifd.exifR98_version.offset = exifR98_version_string;
  info->interop_ifd.exifR98_version.fixedLength = TRUE;
  info->interop_ifd.exifR98_version.active = TRUE;

  /* Fixed length */ 
  info->gps_ifd.version_id.id = (uint16)GPS_VERSION_ID;
  info->gps_ifd.version_id.type = (uint16)EXIF_BYTE;
  info->gps_ifd.version_id.count = 4;
  info->gps_ifd.version_id.offset = exif_gps_version_id;
  info->gps_ifd.version_id.fixedLength = TRUE;
  info->gps_ifd.version_id.active = TRUE;

  /* Fixed length */ 
  info->gps_ifd.latitude_ref.id = (uint16)GPS_LATITUDE_REF;
  info->gps_ifd.latitude_ref.type = (uint16)EXIF_ASCII;
  info->gps_ifd.latitude_ref.count = 2;
  info->gps_ifd.latitude_ref.offset = exif_latitude_ref_string;
  info->gps_ifd.latitude_ref.fixedLength = TRUE;
  info->gps_ifd.latitude_ref.active = TRUE;

  /* Fixed length */
  info->gps_ifd.latitude.id = (uint16)GPS_LATITUDE;
  info->gps_ifd.latitude.type = (uint16)EXIF_RATIONAL;
  info->gps_ifd.latitude.count = 3;
  info->gps_ifd.latitude.offset = exif_latitude;
  info->gps_ifd.latitude.fixedLength = TRUE;
  info->gps_ifd.latitude.active = TRUE;

  /* Fixed length  */
  info->gps_ifd.longitude_ref.id = (uint16)GPS_LONGITUDE_REF;
  info->gps_ifd.longitude_ref.type = (uint16)EXIF_ASCII;
  info->gps_ifd.longitude_ref.count = 2;
  info->gps_ifd.longitude_ref.offset = exif_longitude_ref_string;
  info->gps_ifd.longitude_ref.fixedLength = TRUE;
  info->gps_ifd.longitude_ref.active = TRUE;

  /* Fixed length */
  info->gps_ifd.longitude.id = (uint16)GPS_LONGITUDE;
  info->gps_ifd.longitude.type = (uint16)EXIF_RATIONAL;
  info->gps_ifd.longitude.count = 3;
  info->gps_ifd.longitude.offset = exif_longitude;
  info->gps_ifd.longitude.fixedLength = TRUE;
  info->gps_ifd.longitude.active = TRUE;

  /* Fixed length */
  info->gps_ifd.altitude_ref.id = (uint16)GPS_ALTITUDE_REF;
  info->gps_ifd.altitude_ref.type = (uint16)EXIF_BYTE;
  info->gps_ifd.altitude_ref.count = 1;
  exif_altitude_ref = (int16)GPS_ORTHOMETRIC_SYSTEM;
  info->gps_ifd.altitude_ref.offset = &exif_altitude_ref;
  info->gps_ifd.altitude_ref.fixedLength = TRUE;
  info->gps_ifd.altitude_ref.active = TRUE;

  /* Fixed length */
  info->gps_ifd.altitude.id = (uint16)GPS_ALTITUDE;
  info->gps_ifd.altitude.type = (uint16)EXIF_RATIONAL;
  info->gps_ifd.altitude.count = 1;
  info->gps_ifd.altitude.offset = &exif_altitude;
  info->gps_ifd.altitude.fixedLength = TRUE;
  info->gps_ifd.altitude.active = TRUE;

  /* Fixed length */
  info->gps_ifd.timestamp.id = (uint16)GPS_TIMESTAMP;
  info->gps_ifd.timestamp.type = (uint16)EXIF_RATIONAL;
  info->gps_ifd.timestamp.count = 3;
  info->gps_ifd.timestamp.offset = exif_gps_timestamp;
  info->gps_ifd.timestamp.fixedLength = TRUE;
  info->gps_ifd.timestamp.active = TRUE;

  /* Variable Length */
  info->gps_ifd.map_datum.id = (uint16)GPS_MAP_DATUM;
  info->gps_ifd.map_datum.type = (uint16)EXIF_ASCII;
  strSize = strlen((char*)exif_map_datum_string)+1;
  info->gps_ifd.map_datum.count = strSize;
  strPtr = JPEG_MALLOC(strSize);
  info->gps_ifd.map_datum.active = FALSE;
  if (strPtr)
  {
    memcpy(strPtr, exif_map_datum_string, strSize);
    info->gps_ifd.map_datum.offset = strPtr;
    info->gps_ifd.map_datum.active = TRUE;
  }
  info->gps_ifd.map_datum.fixedLength = FALSE;


  /* Variable Length - no default value */
  info->gps_ifd.area_information.id = (uint16)GPS_AREA_INFORMATION;
  info->gps_ifd.area_information.type = (uint16)EXIF_UNDEFINED;
  info->gps_ifd.area_information.count = 0;
  info->gps_ifd.area_information.offset = 0;
  info->gps_ifd.area_information.fixedLength = FALSE;
  info->gps_ifd.area_information.active = TRUE;

  /* Fixed length 11 bytes */
  info->gps_ifd.datestamp.id = (uint16)GPS_DATESTAMP;
  info->gps_ifd.datestamp.type = (uint16)EXIF_ASCII;
  info->gps_ifd.datestamp.count = 11;
  info->gps_ifd.datestamp.offset = exif_gps_datestamp;
  info->gps_ifd.datestamp.fixedLength = TRUE;
  info->gps_ifd.datestamp.active = TRUE;

  /* Variable Length */
  info->gps_ifd.process_method.id = (uint16)GPS_PROCESS_METHOD;
  info->gps_ifd.process_method.type = (uint16)EXIF_UNDEFINED;
  strSize = 18;
  info->gps_ifd.process_method.count = strSize;
  strPtr = JPEG_MALLOC(strSize);
  info->gps_ifd.process_method.active = FALSE;
  if (strPtr)
  {
    memcpy(strPtr, exif_gps_process_method, strSize);
    info->gps_ifd.process_method.offset = strPtr;
    info->gps_ifd.process_method.active = TRUE;
  }
  info->gps_ifd.process_method.fixedLength = FALSE;


  info->gps_set = FALSE;

  /* Fixed length */ 
  info->thumbnail_ifd.compression.id = (uint16)COMPRESSION;
  info->thumbnail_ifd.compression.type = (uint16)EXIF_SHORT;
  info->thumbnail_ifd.compression.count = 1;
  /*Here is the place where the addres (which should be 32 bit alligned passed to)*/
  /*info->thumbnail_ifd.compression.offset*/
  /*Kindly see the comments at ((uint32*)tag->offset)[0]= jpeg_exifread_get_4bytes(info);*/
  exif_compression = (int32)JPEG_COMPRESSION;
  info->thumbnail_ifd.compression.offset = &exif_compression;
  info->thumbnail_ifd.compression.fixedLength = TRUE;
  info->thumbnail_ifd.compression.active = TRUE;

  /* Fixed length */
  info->thumbnail_ifd.x_resolution.id = (uint16)X_RESOLUTION;
  info->thumbnail_ifd.x_resolution.type = (uint16)EXIF_RATIONAL;
  info->thumbnail_ifd.x_resolution.count = 1;
  info->thumbnail_ifd.x_resolution.offset = &exif_x_resolution;
  info->thumbnail_ifd.x_resolution.fixedLength = TRUE;
  info->thumbnail_ifd.x_resolution.active = TRUE;

  /* Fixed length */
  info->thumbnail_ifd.y_resolution.id = (uint16)Y_RESOLUTION;
  info->thumbnail_ifd.y_resolution.type = (uint16)EXIF_RATIONAL;
  info->thumbnail_ifd.y_resolution.count = 1;
  info->thumbnail_ifd.y_resolution.offset = &exif_y_resolution;
  info->thumbnail_ifd.y_resolution.fixedLength = TRUE;
  info->thumbnail_ifd.y_resolution.active = TRUE;

  /* Fixed length  */
  info->thumbnail_ifd.resolution_unit.id = (uint16)RESOLUTION_UNIT;
  info->thumbnail_ifd.resolution_unit.type = (uint16)EXIF_SHORT;
  info->thumbnail_ifd.resolution_unit.count = 1;
  exif_resolution_unit = 2;
  info->thumbnail_ifd.resolution_unit.offset = &exif_resolution_unit;
  info->thumbnail_ifd.resolution_unit.fixedLength = TRUE;
  info->thumbnail_ifd.resolution_unit.active = TRUE;

  /* Fixed length  */
  info->thumbnail_ifd.jpegi_format.id = (uint16)JPEGINTERCHANGE_FORMAT;
  info->thumbnail_ifd.jpegi_format.type = (uint16)EXIF_LONG;
  info->thumbnail_ifd.jpegi_format.count = 1;
  exif_jpegi_format = 0;
  info->thumbnail_ifd.jpegi_format.offset = &exif_jpegi_format;
  info->thumbnail_ifd.jpegi_format.fixedLength = TRUE;
  info->thumbnail_ifd.jpegi_format.active = TRUE;

  /* Fixed length */ 
  info->thumbnail_ifd.jpegi_format_l.id = (uint16)JPEGINTERCHANGE_FORMAT_L;
  info->thumbnail_ifd.jpegi_format_l.type =(uint16)EXIF_LONG;
  info->thumbnail_ifd.jpegi_format_l.count = 1;
  exif_jpegi_format_l = 0;
  info->thumbnail_ifd.jpegi_format_l.offset = &exif_jpegi_format_l;
  info->thumbnail_ifd.jpegi_format_l.fixedLength = TRUE;
  info->thumbnail_ifd.jpegi_format_l.active = TRUE;
  info->comp_info = exif_jpeg_comp_info;

  info->data_precision = BITS_IN_JSAMPLE;

  /* Set up two Huffman tables */
  exif_std_huff_tables(info);

  /* Fill in default JFIF marker parameters.  Note that whether the marker
   * will actually be written is determined by exif_jpeg_set_colorspace.
   *
   * By default, the library emits JFIF version code 1.01.
   * An application that wants to emit JFIF 1.02 extension markers should set
   * JFIF_minor_version to 2.  We could probably get away with just defaulting
   * to 1.02, but there may still be some decoders in use that will complain
   * about that; saying 1.01 should minimize compatibility problems.
   */
  info->JFIF_major_version = 1; /* Default JFIF version = 1.01 */
  info->JFIF_minor_version = 1;
  info->density_unit = 0;  /* Pixel size is unknown by default */
  info->X_density = 1;    /* Pixel aspect ratio is square by default */
  info->Y_density = 1;
  info->thumbnail_present = FALSE;
  info->restart_cnt = 0;
  info->subsampling = EXIF_H2V2;
  
#ifdef FEATURE_EXIF_PIM3   
  if(exif_PIM3_enable(info) == FALSE)
    MSG_HIGH("EXIF error emitting PIM3", 0, 0, 0);
#endif

}

/*===========================================================================

FUNCTION      EXIF_SET_POSITION

DESCRIPTION
              Set the position.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void exif_set_position
(
exif_info_type * info,
const exif_position_type *position
)
{
  double float_val;
  int32 latitude_degrees, latitude_minutes, latitude_centi_seconds;
  int32 longitude_degrees, longitude_minutes, longitude_centi_seconds;

  /* Convert GPS info from rad to degrees */
  float_val = position->latitude/186413.5111;
  latitude_degrees = (int32) float_val;
  float_val -= latitude_degrees;
  if (float_val < 0) float_val = -float_val;
  float_val *= 60; /* minutes */
  latitude_minutes = (int32) float_val;
  float_val -= latitude_minutes;
  latitude_centi_seconds = (int32)(float_val * 6000); /* centi-seconds */
  if (latitude_degrees >= 0)
  {
    exif_latitude_ref_string[0] = 'N';
  } else
  {
    exif_latitude_ref_string[0] = 'S';
    latitude_degrees = -latitude_degrees;
  }
  exif_latitude[0].numerator = latitude_degrees;
  exif_latitude[1].numerator = latitude_minutes;
  exif_latitude[2].numerator = latitude_centi_seconds;

  float_val = position->longitude/186413.5111;
  longitude_degrees = (int32) float_val;
  float_val -= longitude_degrees;
  if (float_val < 0) float_val = -float_val;
  float_val *= 60; /* minutes */
  longitude_minutes = (int32) float_val;
  float_val -= longitude_minutes;
  longitude_centi_seconds = (int32)(float_val * 6000); /* centi-seconds */
  if (longitude_degrees >= 0)
  {
    exif_longitude_ref_string[0] = 'E';
  } else
  {
    exif_longitude_ref_string[0] = 'W';
    longitude_degrees = -longitude_degrees;
  }
  exif_longitude[0].numerator = longitude_degrees;
  exif_longitude[1].numerator = longitude_minutes;
  exif_longitude[2].numerator = longitude_centi_seconds;

  exif_altitude.numerator = position->altitude;

  exif_set_gps_date_time(position->timestamp);

  info->gps_set = TRUE;
}

/*===========================================================================

FUNCTION      EXIF_SET_TAG_VALUE

DESCRIPTION
              Set the value of the tag with corresponding Id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void exif_set_tag_value(exif_info_type * info,exif_value_type  id,
                        void* dataPtr)
{
  uint16 * valuePtr = (uint16 *) dataPtr;     

  switch (id)
  {
    case EXIF_IFD_EXPOSURE_TIME:
      memcpy(info->exif_ifd.exposure_time.offset, 
             valuePtr, sizeof(exif_rational_type));
      info->exif_ifd.exposure_time.active = TRUE;
      break;

    case EXIF_IFD_FLASH:
      ((uint16*)info->exif_ifd.flash.offset)[0] = *valuePtr;
      info->exif_ifd.flash.active = TRUE;
      break;

    case EXIF_IFD_CUSTOM_RENDERED:
      ((uint16*)info->exif_ifd.custom_rendered.offset)[0] = *valuePtr;
      info->exif_ifd.custom_rendered.active = TRUE;
      break;

    case EXIF_IFD_EXPOSURE_MODE:
      ((uint16*)info->exif_ifd.exposure_mode.offset)[0] = *valuePtr;
      info->exif_ifd.exposure_mode.active = TRUE;
      break;

    case EXIF_IFD_WHITE_BALANCE:
      ((uint16*)info->exif_ifd.white_balance.offset)[0] = *valuePtr;
      info->exif_ifd.white_balance.active = TRUE;
      break;

    case EXIF_IFD_DIGITAL_ZOOM_RATIO:
      memcpy(info->exif_ifd.digital_zoom_ratio.offset, 
             valuePtr, sizeof(exif_rational_type));
      info->exif_ifd.digital_zoom_ratio.active = TRUE;
      break;

    case EXIF_IFD_SCENE_CAPTURE_TYPE:
      ((uint16*)info->exif_ifd.scene_capture_type.offset)[0] = *valuePtr;
      info->exif_ifd.scene_capture_type.active = TRUE;
      break;

    case EXIF_IFD_SUBJECT_DISTANCE_RANGE:
      ((uint16*)info->exif_ifd.subject_distance_range.offset)[0] = *valuePtr;
      info->exif_ifd.subject_distance_range.active = TRUE;
      break;

    default:
      break;
  }
}

/*===========================================================================

FUNCTION      EXIF_SET_STRING

DESCRIPTION
              Set the named string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void exif_set_string
(
exif_info_type * info,
exif_string_type id,
const char *string,
uint16 count
)
{
  /* An extra character is getting included because of the count being 
   incremented. Hence the line has been commented.*/
  switch (id)
  {
    case EXIF_DESCRIPTION_STRING:
      if (info->zero_ifd.image_description.offset)
      {
        JPEG_FREE(info->zero_ifd.image_description.offset);
      }
      info->zero_ifd.image_description.offset = JPEG_MALLOC(count);
      if (info->zero_ifd.image_description.offset)
      {
        memcpy(info->zero_ifd.image_description.offset, string, count);
        info->zero_ifd.image_description.count = count;
        info->zero_ifd.image_description.active = TRUE;
      }
      break;

    case EXIF_USER_COMMENT_STRING:
      if (info->exif_ifd.user_comment.offset)
      {
        JPEG_FREE(info->exif_ifd.user_comment.offset);
      }
      info->exif_ifd.user_comment.offset  = JPEG_MALLOC(count);
      if (info->exif_ifd.user_comment.offset)
      {
        memcpy(info->exif_ifd.user_comment.offset, string, count);
        info->exif_ifd.user_comment.count = count;
        info->exif_ifd.user_comment.active = TRUE;
      }
      break;

    case EXIF_MAKER_NOTE_STRING:
      if (info->exif_ifd.maker_note.offset)
      {
        JPEG_FREE(info->exif_ifd.maker_note.offset);
      }
      info->exif_ifd.maker_note.offset  = JPEG_MALLOC(count);
      if (info->exif_ifd.maker_note.offset)
      {
        memcpy(info->exif_ifd.maker_note.offset, string, count);
        info->exif_ifd.maker_note.count = count;
        info->exif_ifd.maker_note.active = TRUE;
      }
      break;

    case EXIF_GPS_AREA_INFORMATION_STRING:
      if (info->gps_ifd.area_information.offset)
      {
        JPEG_FREE(info->gps_ifd.area_information.offset);
      }
      info->gps_ifd.area_information.offset = JPEG_MALLOC(count);
      if (info->gps_ifd.area_information.offset)
      {
        memcpy(info->gps_ifd.area_information.offset, string, count);
        info->gps_ifd.area_information.count = count;
        info->gps_ifd.area_information.active = TRUE;
      }
      break;

    case EXIF_MAKE_STRING:
      if (info->zero_ifd.make.offset)
      {
        JPEG_FREE(info->zero_ifd.make.offset);
      }
      info->zero_ifd.make.offset = JPEG_MALLOC(count);

      if (info->zero_ifd.make.offset)
      {
        memcpy(info->zero_ifd.make.offset, string, count);
        info->zero_ifd.make.count = count;
        info->zero_ifd.make.active = TRUE;
      }
      break;

    case EXIF_MODEL_STRING:
      if (info->zero_ifd.model.offset)
      {
        JPEG_FREE(info->zero_ifd.model.offset);
      }
      info->zero_ifd.model.offset = JPEG_MALLOC(count);

      if (info->zero_ifd.model.offset)
      {
        memcpy(info->zero_ifd.model.offset, string, count);
        info->zero_ifd.model.count = count;
        info->zero_ifd.model.active = TRUE;
      }
      break;

    case EXIF_SOFTWARE_STRING:
      if (info->zero_ifd.software.offset)
      {
        JPEG_FREE(info->zero_ifd.software.offset);
      }
      info->zero_ifd.software.offset = JPEG_MALLOC(count);

      if (info->zero_ifd.software.offset)
      {
        memcpy(info->zero_ifd.software.offset, string, count); 
        info->zero_ifd.software.count = count;
        info->zero_ifd.software.active = TRUE;
      }
      break;

    case EXIF_DATE_TIME_ORIGINAL_STRING:
#ifdef JPEG_USES_LIBSTD
      (void)std_strlcpy(info->exif_ifd.date_time_original.offset, string, strlen((char *)exif_date_time_string) );
#else
      (void)strncpy(info->exif_ifd.date_time_original.offset, string, strlen((char *)exif_date_time_string) );    
#endif
      info->exif_ifd.date_time_original.count = strlen((char *)exif_date_time_string);
      info->exif_ifd.date_time_original.active = TRUE;        
      break;

    case EXIF_GPS_MAP_DATUM_STRING:
#ifdef JPEG_USES_LIBSTD
      (void)std_strlcpy(info->gps_ifd.map_datum.offset, string, strlen((char *)exif_map_datum_string));
#else
      (void)strncpy(info->gps_ifd.map_datum.offset, string, strlen((char *)exif_map_datum_string));    
#endif
      info->gps_ifd.map_datum.count = strlen((char *)exif_map_datum_string );
      info->gps_ifd.map_datum.active = TRUE;
      break;

    case EXIF_GPS_DATESTAMP_STRING:
#ifdef JPEG_USES_LIBSTD
      (void)std_strlcpy(info->gps_ifd.datestamp.offset, string, strlen((char *)exif_gps_datestamp));
#else
      (void)strncpy(info->gps_ifd.datestamp.offset, string, strlen((char *)exif_gps_datestamp));     
#endif
      info->gps_ifd.datestamp.count = strlen((char *)exif_gps_datestamp);
      info->gps_ifd.datestamp.active = TRUE;
      break;

    default:
      break;
  }
}


/*===========================================================================

FUNCTION      EXIF_EMIT_HEADER

DESCRIPTION
              The header is emitted to the buffer.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if info was initialized and emit header is successful.
  The total header byte count cannot exceed JPEG_MAX_DIMENSION bytes.

SIDE EFFECTS
  None

===========================================================================*/

boolean exif_emit_header
(
exif_info_type   *info,
const exif_header_type *header
)
{
  exif_emit_type type;

  if (!info || !header)
  {
    return(FALSE);
  }

  if ((info->code1 != CODE1) || (info->code2 != CODE2) ||
      (info->code3 != CODE3) || (info->code4 != CODE4))
    return(FALSE);

  type = header->type; 

  if ((type == EXIF_EMIT_UPTO_THUMBNAIL) || (type == EXIF_EMIT_ALL) || (type == JFXX_EMIT_UPTO_THUMBNAIL) || (type == JFXX_EMIT_ALL))
  {
    info->buf_ptr = header->buf_ptr;
    info->buf_len = header->length;
    info->buf_index = 0;
  }

  /* Base on the type and set the emit_thumbnail flag accordingly */
  /* this information is used to set sampling factor in set colorspace */
  if ((type == EXIF_EMIT_UPTO_THUMBNAIL) || (type == JFXX_EMIT_UPTO_THUMBNAIL))
  {
    info->emit_thumbnail = TRUE;
  } else
  {
    info->emit_thumbnail = FALSE;
  }

  info->image_width  = header->main_width;
  info->image_height = header->main_height;
  ((uint32*)info->exif_ifd.pixel_x_dimension.offset)[0] = header->main_width;
  ((uint32*)info->exif_ifd.pixel_y_dimension.offset)[0] = header->main_height;
  info->thumbnail_width  = header->tn_width;
  info->thumbnail_height = header->tn_height;
  info->color_space = header->color_space;
  info->quality_percent = header->quality_percent;
  info->subsampling = header->subsample;

  /* Choose JPEG colorspace based on color space */
  switch (header->color_space)
  {
    case EXIF_GRAYSCALE:
      exif_jpeg_set_colorspace(info, EXIF_GRAYSCALE);
      break;
    case EXIF_RGB:
      exif_jpeg_set_colorspace(info, EXIF_YCbCr);
      break;
    case EXIF_YCbCr:
    default:
      exif_jpeg_set_colorspace(info, EXIF_YCbCr);
      break;
  }

  /* Tables are built dynamically now, no individual tables 
     to reference anymore */
  info->quant_tbl_ptrs[0] = header->luma_dqt_table;
  info->quant_tbl_ptrs[1] = header->chroma_dqt_table;


  info->Ss = 0;
  info->Se = DCTSIZE2-1;
  info->Ah = 0;
  info->Al = 0;

  if ((type == EXIF_EMIT_UPTO_THUMBNAIL) || (type == EXIF_EMIT_ALL))
  {
    if (exif_emit_soi(info) == FALSE) return(FALSE);
    if (exif_emit_app1(info) == FALSE) return(FALSE);
  }

  if ((type == JFXX_EMIT_UPTO_THUMBNAIL) || (type == JFXX_EMIT_ALL))
  {
    if (exif_emit_soi(info) == FALSE) return(FALSE);
    if (exif_emit_app0(info, type) == FALSE) return(FALSE);
  }

  if ((type == EXIF_EMIT_AFTER_THUMBNAIL) || (type == EXIF_EMIT_ALL) || (type == JFXX_EMIT_ALL) || (type == JFXX_EMIT_AFTER_THUMBNAIL))
  {
#ifdef FEATURE_CAMERA_MOBICAT
    if (jpege_emit_app7(info) == FALSE)
    {
      MSG_HIGH("EXIF error emitting APP7", 0, 0, 0);
    }
#endif
    /* Save starting position of Quant tables.  For use by File
       Size Rate Ctrl specifically.                             */
    info->buf_index_dqt_pos = info->buf_index;
    if (exif_write_frame_header(info) == FALSE) return(FALSE);
    if (exif_emit_sos(info) == FALSE) return(FALSE);
  }
  return(TRUE);
}

/*===========================================================================

FUNCTION      EXIF_UPDATE_APP0_THUMBNAIL

DESCRIPTION
              Need to update header after thumbnail encoding is
              done for APP0 (JFIF); updates the length field.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE if failed

SIDE EFFECTS
  None

===========================================================================*/

boolean exif_update_app0_thumbnail(exif_info_type *info, uint32 size)
{
  uint32 index = info->buf_index + size;

  /* If the last byte is 0, then it was padded by DSP. Remove it */
  if (*(info->buf_ptr + index - 1) == 0)
  {
    index--;
  }

  /* update the APP0 length field */
  info->buf_index = info->length_index;
  if (exif_emit_2bytes(info, (uint16)(index - info->length_index)) == FALSE)
    return(FALSE);
  info->buf_index = index;
  return(TRUE);
}
/*===========================================================================

FUNCTION      EXIF_UPDATE_THUMBNAIL

DESCRIPTION
              Need to update header after thumbnail encoding is
              done.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE if failed

SIDE EFFECTS
  None

===========================================================================*/

boolean exif_update_thumbnail(exif_info_type *info, uint32 size)
{
  uint32 index = info->buf_index + size;

  if (index > EXIF_MAX_APP1_LENGTH)
  {
    index = info->thumbnail_index;
    /* update the APP1 length field to start of thumbnail*/
    info->buf_index = info->length_index;
    if (exif_emit_2bytes(info, (uint16)(index - info->length_index)) == FALSE)
      return(FALSE);
    
    /* set THUMBNAIL_IFD pointer to NULL */
    info->buf_index = info->thumbnail_ifd_index;
    if (exif_emit_4bytes(info, 0) == FALSE) 
      return(FALSE);
    info->buf_index = index;
  }
  else
  {
    /* If the last byte is 0, then it was padded by DSP. Remove it */
    if (*(info->buf_ptr + index - 1) == 0)
    {
      index--;
    }

    /* update jpegi_format_l */
    info->buf_index = info->jpegi_format_l_index + 8;
    if (exif_emit_4bytes(info, index - info->thumbnail_stream_index) == FALSE)
      return(FALSE);

    /* update the APP1 length field */
    info->buf_index = info->length_index;
    if (exif_emit_2bytes(info, (uint16)(index - info->length_index)) == FALSE)
      return(FALSE);
    info->buf_index = index;
  }

  return(TRUE);
}

/*===========================================================================

FUNCTION      JFIF_UPDATE_THUMBNAIL

DESCRIPTION
              Need to update header after thumbnail encoding is
              done.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE if failed

SIDE EFFECTS
  None

===========================================================================*/

boolean jfif_update_thumbnail(exif_info_type *info, uint32 size)
{
  uint32 index = info->buf_index + size;

  /* If the last byte is 0, then it was padded by DSP. Remove it */
  if (*(info->buf_ptr + index - 1) == 0)
  {
    index--;
  }

  /* APP0 length */
  info->buf_index = info->length_index; /* Length offset */

  if (exif_emit_2bytes(info, (uint16)(index - info->length_index)) == FALSE)
    return(FALSE);
  info->buf_index = index;
  return(TRUE);
}

/*===========================================================================

FUNCTION      EXIF_TERMINATE

DESCRIPTION
              Clean up any EXIF related memory (only variable length fields)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void exif_terminate(exif_info_type *info)
{
  set_date_time = TRUE;
  if (info->zero_ifd.image_description.offset)
  {
    JPEG_FREE(info->zero_ifd.image_description.offset);
    info->zero_ifd.image_description.offset = NULL;
  }

  if (info->gps_ifd.area_information.offset)
  {
    JPEG_FREE(info->gps_ifd.area_information.offset);
    info->gps_ifd.area_information.offset = NULL;
  }

  if (info->zero_ifd.make.offset)
  {
    JPEG_FREE(info->zero_ifd.make.offset);
    info->zero_ifd.make.offset = NULL;
  }

  if (info->zero_ifd.model.offset)
  {
    JPEG_FREE(info->zero_ifd.model.offset);
    info->zero_ifd.model.offset = NULL;

  }

  if (info->zero_ifd.software.offset)
  {
    JPEG_FREE(info->zero_ifd.software.offset);
    info->zero_ifd.software.offset = NULL;

  }

  if (info->exif_ifd.user_comment.offset)
  {
    JPEG_FREE(info->exif_ifd.user_comment.offset);
    info->exif_ifd.user_comment.offset = NULL;
  }

  if (info->exif_ifd.maker_note.offset)
  {
    JPEG_FREE(info->exif_ifd.maker_note.offset);
    info->exif_ifd.maker_note.offset = NULL;
  }

  if (info->exif_ifd.cfa_pattern.offset)
  {
    JPEG_FREE(info->exif_ifd.cfa_pattern.offset);
    info->exif_ifd.cfa_pattern.offset = NULL;
  }

  if (info->gps_ifd.map_datum.offset)
  {
    JPEG_FREE(info->gps_ifd.map_datum.offset);
    info->gps_ifd.map_datum.offset = NULL;
  }


  if (info->gps_ifd.process_method.offset)
  {
    JPEG_FREE(info->gps_ifd.process_method.offset);
    info->gps_ifd.process_method.offset = NULL;
  }

#ifdef FEATURE_EXIF_PIM3  
  exif_PIM3_terminate(info);
#endif
  
}

/*===========================================================================

FUNCTION      EXIF_PIM3_TERMINATE

DESCRIPTION
              Clean up any EXIF_PIM related memory 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_EXIF_PIM3
void exif_PIM3_terminate(exif_info_type *info)
{
  if (info->zero_ifd.pim.offset)
  {
    JPEG_FREE(info->zero_ifd.pim.offset);
    info->zero_ifd.pim.offset = NULL;
  }
}
#endif


/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_2BYTES

DESCRIPTION
              Get two bytes from the input stream

DEPENDENCIES
  None

RETURN VALUE Return 2 bytes in 16 bit data
  

SIDE EFFECTS
  None

===========================================================================*/

uint16 jpeg_exifread_get_2bytes(exif_info_type *info)
{
  uint16 value;

  /* Read two bytes from the info pointer */
  if (info->is_big_endian)
  {
    value = *info->pointer++;
    value = (uint16)(value << 8) + *info->pointer++;
  } else
  {
    value = *info->pointer++;
    value = value + (*info->pointer++ << 8);
  }
  return(value);
}

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_2BYTES_STREAM

DESCRIPTION
              Get two bytes from the input stream

DEPENDENCIES
  None

RETURN VALUE Return 2 bytes in 16 bit data
  

SIDE EFFECTS
  None

===========================================================================*/

uint16 jpeg_exifread_get_2bytes_stream(const exif_info_type *info, void* ptr)
{
  uint16 value;
  uint8 *stream;

  stream = (uint8*)ptr;

  /* Read two bytes from pointer */
  if (info->is_big_endian)
  {
    value = *stream++;
    value = (uint16)(value << 8) + *stream++;
  } else
  {
    value = *stream++;
    value = value + (*stream++ << 8);
  }
  return(value);
}
/*===========================================================================

FUNCTION      JPEG_EXIFREAD_COMPARE_2BYTES

DESCRIPTION
              Read two bytes from the stream and compare it with 
              the value passed in.

DEPENDENCIES
  None

RETURN VALUE Boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean jpeg_exifread_compare_2bytes(exif_info_type *info, uint16 value)
{
    uint16 my_val = jpeg_exifread_get_2bytes(info);
    if (my_val == value)
        return TRUE;
    else
        return FALSE;
}

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_COMPARE_STRING

DESCRIPTION
              Compare the passed in string with the data in the stream

DEPENDENCIES
  None

RETURN VALUE  Boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean jpeg_exifread_compare_string(exif_info_type *info, const void *string)
{
    boolean compares = FALSE;

    if (info)
    {
        if (!strcmp((void *)info->pointer, string))
            compares = TRUE;
        info->pointer += strlen(string) + 1;
    }
    return compares;
}

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_4BYTES

DESCRIPTION
              Read four bytes from the input stream

DEPENDENCIES
  None

RETURN VALUE Retrun value in 32 bit data
  

SIDE EFFECTS
  None

===========================================================================*/

uint32 jpeg_exifread_get_4bytes(exif_info_type * info)
{
  uint32 value;

  /* Read the data at info */
  if (info->is_big_endian)
  {
    value = *info->pointer++;
    value = (value << 8) + *info->pointer++;
    value = (value << 8) + *info->pointer++;
    value = (value << 8) + *info->pointer++;
  } else
  {
    value = *info->pointer++;
    value = value + (*info->pointer++ << 8);
    value = value + (*info->pointer++ << 16);
    value = value + (*info->pointer++ << 24);
  }
  return(value);
}

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_4BYTES_STREAM

DESCRIPTION
              Read four bytes from a stream

DEPENDENCIES
  None

RETURN VALUE Retrun value in 32 bit data
  

SIDE EFFECTS
  None

===========================================================================*/

uint32 jpeg_exifread_get_4bytes_stream(const exif_info_type * info, void* ptr)
{
  uint32 value;
  uint8 *stream;

  stream = (uint8*)ptr;

  /* Read the data from pointer */
  if (info->is_big_endian)
  {
    value = *stream++;
    value = (value << 8) + *stream++;
    value = (value << 8) + *stream++;
    value = (value << 8) + *stream++;
  } else
  {
    value = *stream++;
    value = value + (*stream++ << 8);
    value = value + (*stream++ << 16);
    value = value + (*stream++ << 24);
  }
  return(value);
}
/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_POINTER

DESCRIPTION
              Get the pointer in the next 4 bytes of data in stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

uint8 * jpeg_exifread_get_pointer(exif_info_type * info)
{
  uint32 pointer;
  pointer = (uint32)jpeg_exifread_get_4bytes(info);

  /* Out of range pointer. That is BAD. Set it to NULL */
  /* to avoid run-away activity */
  if (pointer >= info->stream_length)
  {
    pointer = NULL;
    invalidTag = TRUE;
  }

  pointer += (uint32)info->tiff_header_pointer;
  return(uint8*)pointer;
}

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_TAG

DESCRIPTION
              Get the next TAG information in the input stream and 
              store them in the passed in buffer pointer.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void jpeg_exifread_get_tag(exif_info_type *info, exif_tag_type *tag)
{
  exif_data_type type;
  uint32 tmp,i;
  uint16 typeSize = 1;
  boolean localInvalidTag = FALSE;
  void *ptr; 

  if ((info == NULL)||(tag == NULL))
  {
    return;
  }
  /* On entry, index at type field of the tag */
  type = (exif_data_type)jpeg_exifread_get_2bytes(info);

  tmp = jpeg_exifread_get_4bytes(info);
  if (!tag->fixedLength)
  {
    tag->count = tmp;
  }

  tag->active = TRUE;

  switch (type)
  {
    case EXIF_ASCII:
    case EXIF_UNDEFINED:
    case EXIF_BYTE:
      typeSize = sizeof(uint8);

      /* If tag size is less than 4 bytes */
      /* Read from exif header */
      if (typeSize * tag->count <= 4)
      {
        if (!info->isDummyTag)
        {
          if (!tag->fixedLength)
          {
            if (tag->offset != NULL)
            {
              JPEG_FREE(tag->offset);
              tag->offset = NULL;
            }
            tag->offset = JPEG_MALLOC(4);
          }
          /* tag->count <= 4/typeSize = 4 */
          for (i=0;i < tag->count; i++)
          {
            ((uint8*)tag->offset)[i]= *info->pointer++;
          }
          for (;i < 4; i++)
          {
            (void)*info->pointer++;
          }
        } else
        {

          for (i=0;i < 4; i++)
          {
            (void)*info->pointer++;
          }
        }
      } else
      {
        /* Read from stream */
        ptr = jpeg_exifread_get_pointer(info);

        if (!info->isDummyTag)
        {
          if (!tag->fixedLength)
          {
            if (tag->offset != NULL)
            {
              JPEG_FREE(tag->offset);
              tag->offset = NULL;
            }
            if (tag->count*typeSize < info->stream_length)
            {
              tag->offset = JPEG_MALLOC(tag->count*typeSize);
            } else
            {
              invalidTag = TRUE;
              localInvalidTag = TRUE;
            }
          }
          if (tag->offset != NULL)
          {
            memcpy(tag->offset, ptr, tag->count*typeSize);
          } else
          {
            tag->active = FALSE;
          }
        }
      } 
      break;
    case EXIF_SHORT:
      typeSize = sizeof(uint16);
      /* If tag size is less than 4 bytes */
      /* Read from exif header */ 
      if (typeSize * tag->count <= 4)
      {
        if (!info->isDummyTag)
        {
          if (!tag->fixedLength)
          {
            if (tag->offset != NULL)
            {
              JPEG_FREE(tag->offset);
              tag->offset = NULL;
            }
            tag->offset = JPEG_MALLOC(4);
          }

          /* tag->count <= 4/typeSize = 2 */
          for (i=0;i < tag->count; i++)
          {
            ((uint16*)tag->offset)[i]= jpeg_exifread_get_2bytes(info);
          }
          for (;i < 2; i++)
          {
            (void)jpeg_exifread_get_2bytes(info);
          }
        } else
        {
          for (i=0;i < 2; i++)
          {
            (void)jpeg_exifread_get_2bytes(info);
          }
        }
      } else
      {
        /* REad from stream */
        ptr = jpeg_exifread_get_pointer(info);

        if (!info->isDummyTag)
        {
          if (!tag->fixedLength)
          {
            if (tag->offset != NULL)
            {
              JPEG_FREE(tag->offset);
              tag->offset = NULL;
            }
            if (tag->count*typeSize < info->stream_length)
            {
              tag->offset = JPEG_MALLOC(tag->count*typeSize);
            } else
            {
              invalidTag = TRUE;
              localInvalidTag = TRUE;
            }
          }

          if (tag->offset != NULL)
          {
            for (i=0; i< tag->count;i++)
            {
              ((uint16*)tag->offset)[i] = jpeg_exifread_get_2bytes_stream(info,(uint16*)ptr+i);
            }
          } else
          {
            tag->active = FALSE;
          }

        }
      } 
      break;
    case EXIF_LONG:
      typeSize = sizeof(uint32);

      /* If tag size is less than 4 bytes */
      /* Read from exif header */
      if (typeSize * tag->count <= 4)
      {
        if (!info->isDummyTag)
        {
          if (!tag->fixedLength)
          {
            if (tag->offset != NULL)
            {
              JPEG_FREE(tag->offset);
              tag->offset = NULL;
            }
            tag->offset = JPEG_MALLOC(4);
          }

          if (tag->id == (uint16)EXIF_IFD_POINTER || tag->id == (uint16)JPEGINTERCHANGE_FORMAT || tag->id == (uint16)GPS_IFD_POINTER || tag->id == (uint16)INTEROP_IFD_POINTER)
          {
            ((uint32*)tag->offset)[0] = (uint32)jpeg_exifread_get_pointer(info);
          } else
          {
            /*When we are intaking the input as (uint32*),*/
            /* it Compiler will compile as str, instead of strh*/
            /*that implies tag->offset)[0] should be 32 bit alligned word*/
            /*rather than 16 bit alligned word*/
            ((uint32*)tag->offset)[0]= jpeg_exifread_get_4bytes(info);
          }
        } else
        {
          /* Dummy read to advance the pointer */
          (void)jpeg_exifread_get_4bytes(info);
        }

      } else
      {
        /* Read from stream */
        ptr = jpeg_exifread_get_pointer(info);

        if (!info->isDummyTag)
        {
          if (!tag->fixedLength)
          {
            if (tag->offset != NULL)
            {
              JPEG_FREE(tag->offset);
              tag->offset = NULL;
            }

            if (tag->count*typeSize < info->stream_length)
            {
              tag->offset = JPEG_MALLOC(tag->count*typeSize);
            } else
            {
              invalidTag = TRUE;
              localInvalidTag = TRUE;
            }
          }

          if (tag->offset != NULL)
          {
            for (i=0; i< tag->count;i++)
            {
              ((uint32*)tag->offset)[i] = jpeg_exifread_get_4bytes_stream(info,(uint32*)ptr+i);
            }
          } else
          {
            tag->active = FALSE;
          }
        }
      }
      break;
    case EXIF_RATIONAL:
    case EXIF_SRATIONAL:
      /* Read from stream */
      typeSize = sizeof(exif_rational_type);
      ptr = jpeg_exifread_get_pointer(info);

      if (!info->isDummyTag)
      {
        if (!tag->fixedLength)
        {
          if (tag->offset != NULL)
          {
            JPEG_FREE(tag->offset);
            tag->offset = NULL;
          }

          if (tag->count*typeSize < info->stream_length)
          {
            tag->offset = JPEG_MALLOC(tag->count*typeSize);
          } else
          {
            invalidTag = TRUE;
            localInvalidTag = TRUE;
          }
        }

        if (tag->offset != NULL)
        {
          for (i=0; i< tag->count;i++)
          {
            ((exif_rational_type*)tag->offset)[i].numerator = (int32)jpeg_exifread_get_4bytes_stream(info,(uint32*)ptr+2*i);
            ((exif_rational_type*)tag->offset)[i].denominator = (int32)jpeg_exifread_get_4bytes_stream(info,(uint32*)ptr+2*i+1);
          }
        } else
        {
          tag->active = FALSE;
        }
      }
      break;
    default:
      break;
  }
  if (!localInvalidTag)
    tag->active = TRUE;
}

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_ZERO_IFD

DESCRIPTION
              Read and update the 0th IFD in the input stream.

DEPENDENCIES
  None

RETURN VALUE  Boolean

SIDE EFFECTS
  None

===========================================================================*/

boolean jpeg_exifread_get_zero_ifd(exif_info_type *info)
{
  uint32 count, i;

  if (info == NULL)
  {
    return(FALSE);
  }
  invalidTag = FALSE;
  info->pointer = jpeg_exifread_get_pointer(info);
  count = jpeg_exifread_get_2bytes(info);
  for (i=0; i<count; i++)
  {
    uint16 tag_id;
    tag_id = jpeg_exifread_get_2bytes(info);
    switch (tag_id)
    {
      case  IMAGE_DESCRIPTION:
        jpeg_exifread_get_tag(info, &info->zero_ifd.image_description);
        break;
      case  MAKE:
        jpeg_exifread_get_tag(info, &info->zero_ifd.make);
        break;
      case  MODEL:
        jpeg_exifread_get_tag(info, &info->zero_ifd.model);
        break;
      case  ORIENTATION:
        jpeg_exifread_get_tag(info, &info->zero_ifd.orientation);
        break;
      case  X_RESOLUTION:
        jpeg_exifread_get_tag(info, &info->zero_ifd.x_resolution);
        break;
      case  Y_RESOLUTION:
        jpeg_exifread_get_tag(info, &info->zero_ifd.y_resolution);
        break;
      case  RESOLUTION_UNIT:
        jpeg_exifread_get_tag(info, &info->zero_ifd.resolution_unit);
        break;
      case  YCBCR_POSITIONING:
        jpeg_exifread_get_tag(info, &info->zero_ifd.ycbcr_positioning);
        break;
      case  EXIF_IFD_POINTER:
        jpeg_exifread_get_tag(info, &info->zero_ifd.exif_ifd_pointer);
        break;
      case  GPS_IFD_POINTER:
        jpeg_exifread_get_tag(info, &info->zero_ifd.gps_ifd_pointer);
        break;
      case  SOFTWARE:
        jpeg_exifread_get_tag(info, &info->zero_ifd.software);
        break;

      default:
        info->isDummyTag = TRUE;
        jpeg_exifread_get_tag(info, &info->dummy_tag);
        info->isDummyTag = FALSE;
        break;

    }
    if (invalidTag)
    {
      invalidTag = FALSE;
      MSG_HIGH("EXIF error reading ZERO IFD tag %d ",tag_id ,0, 0);
      break;
    }
  }
  info->thumbnail_ifd_pointer = jpeg_exifread_get_pointer(info);

  /* Calculate EXIF IFD pointer */
  if (((uint32*)info->zero_ifd.exif_ifd_pointer.offset)[0])
  {
    /* Point to offset first, then get the pointer */
    info->exif_ifd_pointer = (uint8*)(((uint32*)info->zero_ifd.exif_ifd_pointer.offset)[0]);
  } else
  {
    info->exif_ifd_pointer = info->tiff_header_pointer;
  }
  /* Calculate GPS IFD pointer */
  if (((uint32*)info->zero_ifd.gps_ifd_pointer.offset)[0])
  {
    /* Point to offset first, then get the pointer */
    info->gps_ifd_pointer = (uint8*)(((uint32*)info->zero_ifd.gps_ifd_pointer.offset)[0]);
  } else
  {
    info->gps_ifd_pointer = info->tiff_header_pointer;
  }
  return(TRUE);
}

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_EXIF_IFD

DESCRIPTION
              Read and update the EXIF IFD from the input stream.

DEPENDENCIES
  None

RETURN VALUE  Boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean jpeg_exifread_get_exif_ifd(exif_info_type *info)
{
  uint32 i, count;

  if (info == NULL)
  {
    return(FALSE);
  }

  if (info->exif_ifd_pointer == info->tiff_header_pointer)
    return(TRUE);

  invalidTag = FALSE;
  info->pointer = info->exif_ifd_pointer;
  count = jpeg_exifread_get_2bytes(info);
  for (i=0; i<count; i++)
  {
    uint16 tag_id;

    tag_id = jpeg_exifread_get_2bytes(info);
    switch (tag_id)
    {
      case  EXIF_VERSION:
        jpeg_exifread_get_tag(info, &info->exif_ifd.version);
        break;
      case  EXIF_DATE_TIME_ORIGINAL:
        jpeg_exifread_get_tag(info, &info->exif_ifd.date_time_original);
        break;
      case  EXIF_COMPONENTS_CONFIGURATION:
        jpeg_exifread_get_tag(info, &info->exif_ifd.components_configuration);
        break;
      case  EXIF_USER_COMMENT:
        jpeg_exifread_get_tag(info, &info->exif_ifd.user_comment);
        break;
      case  EXIF_MAKER_NOTE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.maker_note);
        break;
      case  EXIF_FLASHPIX_VERSION:
        jpeg_exifread_get_tag(info, &info->exif_ifd.flashpix_version);
        break;
      case  EXIF_COLOR_SPACE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.color_space);
        break;
      case  EXIF_PIXEL_X_DIMENSION:
        jpeg_exifread_get_tag(info, &info->exif_ifd.pixel_x_dimension);
        break;
      case  EXIF_PIXEL_Y_DIMENSION:
        jpeg_exifread_get_tag(info, &info->exif_ifd.pixel_y_dimension);
        break;
      case INTEROP_IFD_POINTER:
        jpeg_exifread_get_tag(info, &info->exif_ifd.interop_ifd_pointer);
        break;
      case  EXPOSURE_TIME:
        jpeg_exifread_get_tag(info, &info->exif_ifd.exposure_time);
        break;
      case  EXIF_DATE_TIME_DIGITIZED:
        jpeg_exifread_get_tag(info, &info->exif_ifd.date_time_digitized);
        break;
      case  FLASH:
        jpeg_exifread_get_tag(info, &info->exif_ifd.flash);
        break;
      case  CUSTOM_RENDERED:
        jpeg_exifread_get_tag(info, &info->exif_ifd.custom_rendered);
        break;
      case  EXPOSURE_MODE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.exposure_mode);
        break;
      case  WHITE_BALANCE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.white_balance);
        break;
      case  DIGITAL_ZOOM_RATIO:
        jpeg_exifread_get_tag(info, &info->exif_ifd.digital_zoom_ratio);
        break;
      case  SCENE_CAPTURE_TYPE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.scene_capture_type);
        break;
      case  SUBJECT_DISTANCE_RANGE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.subject_distance_range);
        break;
      case APERTURE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.aperture); 
        break;   
      case MAX_APERTURE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.max_aperture);   
        break;  
      case LIGHT_SOURCE:    
        jpeg_exifread_get_tag(info, &info->exif_ifd.light_source); 
        break;  
      case SENSING_METHOD:    
        jpeg_exifread_get_tag(info, &info->exif_ifd.sensing_method); 
        break;  
      case FILE_SOURCE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.file_source);  
        break; 
      case CFA_PATTERN:
        jpeg_exifread_get_tag(info, &info->exif_ifd.cfa_pattern);  
        break;  
      case FOCAL_LENGTH_35MM:
        jpeg_exifread_get_tag(info, &info->exif_ifd.focal_length_35mm);  
        break;  
      case CONTRAST:
        jpeg_exifread_get_tag(info, &info->exif_ifd.contrast);  
        break;  
      case SATURATION:
        jpeg_exifread_get_tag(info, &info->exif_ifd.saturation); 
        break;  
      case METERING_MODE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.metering_mode);  
        break;  
      case SHARPNESS:
        jpeg_exifread_get_tag(info, &info->exif_ifd.sharpness);   
        break; 
      case GAIN_CONTROL:
        jpeg_exifread_get_tag(info, &info->exif_ifd.gain_control);   
        break; 
      case F_NUMBER:
        jpeg_exifread_get_tag(info, &info->exif_ifd.f_number);   
        break; 
      case OECF:
        jpeg_exifread_get_tag(info, &info->exif_ifd.oecf);   
        break; 
      case SHUTTER_SPEED:
        jpeg_exifread_get_tag(info, &info->exif_ifd.shutter_speed);   
        break; 
      case EXPOSURE_PROGRAM:
        jpeg_exifread_get_tag(info, &info->exif_ifd.exposure_program);   
        break; 
      case BRIGHTNESS:
        jpeg_exifread_get_tag(info, &info->exif_ifd.brightness);   
        break; 
      case FOCAL_LENGTH:
        jpeg_exifread_get_tag(info, &info->exif_ifd.focal_length);   
        break; 
      case EXPOSURE_INDEX:
        jpeg_exifread_get_tag(info, &info->exif_ifd.exposure_index);   
        break;  
      case SUBJECT_DISTANCE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.subject_distance);   
        break;          
      case SUBJECT_LOCATION:
        jpeg_exifread_get_tag(info, &info->exif_ifd.subject_location);   
        break;
      case SCENE_TYPE:
        jpeg_exifread_get_tag(info, &info->exif_ifd.scene_type);   
        break;
      case ISO_SPEED_RATINGS:
        jpeg_exifread_get_tag(info, &info->exif_ifd.iso_speed_ratings);
        break; 
      default:
        info->isDummyTag = TRUE;
        jpeg_exifread_get_tag(info, &info->dummy_tag);
        info->isDummyTag = FALSE;
        break;  
    }

    if (invalidTag)
    {
      /* If invalid tag (pointer out of limits) drop all reads */
      invalidTag = FALSE;
      MSG_HIGH("EXIF error reading EXIF IFD tag %d ",tag_id ,0, 0);
      break;
    }
  }
  return(TRUE);
}

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_GPS_IFD

DESCRIPTION
              Read and update the GPS IFD from the input stream.

DEPENDENCIES
  None

RETURN VALUE  Boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean jpeg_exifread_get_gps_ifd(exif_info_type *info)
{
  uint32 i, count;

  if (info == NULL)
  {
    return(FALSE);
  }

  if (info->gps_ifd_pointer == info->tiff_header_pointer)
    return(TRUE);

  info->pointer = info->gps_ifd_pointer;
  count = jpeg_exifread_get_2bytes(info);
  invalidTag = FALSE;
  for (i=0; i<count; i++)
  {
    uint16 tag_id;

    tag_id = jpeg_exifread_get_2bytes(info);

    switch (tag_id)
    {
      case  GPS_VERSION_ID:
        jpeg_exifread_get_tag(info, &info->gps_ifd.version_id);
        break;
      case  GPS_LATITUDE_REF:
        jpeg_exifread_get_tag(info, &info->gps_ifd.latitude_ref);
        break;
      case  GPS_LATITUDE:
        jpeg_exifread_get_tag(info, &info->gps_ifd.latitude);
        break;
      case  GPS_LONGITUDE_REF:
        jpeg_exifread_get_tag(info, &info->gps_ifd.longitude_ref);
        break;
      case  GPS_LONGITUDE:
        jpeg_exifread_get_tag(info, &info->gps_ifd.longitude);
        break;
      case  GPS_ALTITUDE_REF:
        jpeg_exifread_get_tag(info, &info->gps_ifd.altitude_ref);
        break;
      case  GPS_ALTITUDE:
        jpeg_exifread_get_tag(info, &info->gps_ifd.altitude);
        break;
      case  GPS_TIMESTAMP:
        jpeg_exifread_get_tag(info, &info->gps_ifd.timestamp);
        break;
      case  GPS_MAP_DATUM:
        jpeg_exifread_get_tag(info, &info->gps_ifd.map_datum);
        break;
      case  GPS_AREA_INFORMATION:
        jpeg_exifread_get_tag(info, &info->gps_ifd.area_information);
        break;
      case  GPS_DATESTAMP:
        jpeg_exifread_get_tag(info, &info->gps_ifd.datestamp);
        break;
      default:
        info->isDummyTag = TRUE;
        jpeg_exifread_get_tag(info, &info->dummy_tag);
        info->isDummyTag = FALSE;
        break;
    }
    if (invalidTag)
    {
      /* If invalid tag (pointer out of limits) drop all reads */
      invalidTag = FALSE;
      MSG_HIGH("EXIF error reading GPS IFD tag %d ",tag_id ,0, 0);
      info->gps_set = FALSE;
      return(TRUE);
    }
  }
  info->gps_set = TRUE;
  return(TRUE);
}

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_THUMBNAIL_IFD

DESCRIPTION
              Read and update thumbnail IFD from the input stream.

DEPENDENCIES
  None

RETURN VALUE  Boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean jpeg_exifread_get_thumbnail_ifd(exif_info_type *info)
{
  uint32 count, i;

  if (info == NULL)
  {
    return(FALSE);
  }

  if (info->thumbnail_ifd_pointer == info->tiff_header_pointer)
  {
    info->thumbnail_present = FALSE;
    info->thumbnail_stream = NULL;
    info->thumbnail_stream_length = 0;
    return(TRUE);
  }

  info->pointer = info->thumbnail_ifd_pointer;
  count = jpeg_exifread_get_2bytes(info);
  for (i=0; i<count; i++)
  {
    uint16 tag_id;

    tag_id = jpeg_exifread_get_2bytes(info);
    switch (tag_id)
    {
      case  COMPRESSION:
        jpeg_exifread_get_tag(info, &info->thumbnail_ifd.compression);
        break;
      case  X_RESOLUTION:
        jpeg_exifread_get_tag(info, &info->thumbnail_ifd.x_resolution);
        break;
      case  Y_RESOLUTION:
        jpeg_exifread_get_tag(info, &info->thumbnail_ifd.y_resolution);
        break;
      case  RESOLUTION_UNIT:
        jpeg_exifread_get_tag(info, &info->thumbnail_ifd.resolution_unit);
        break;
      case  JPEGINTERCHANGE_FORMAT:
        jpeg_exifread_get_tag(info, &info->thumbnail_ifd.jpegi_format);
        break;
      case  JPEGINTERCHANGE_FORMAT_L:
        jpeg_exifread_get_tag(info, &info->thumbnail_ifd.jpegi_format_l);
        break;
      default:
        info->isDummyTag = TRUE;
        jpeg_exifread_get_tag(info, &info->dummy_tag);
        info->isDummyTag = FALSE;
        break;
    }
    if (invalidTag)
    {
      /* If invalid tag (pointer out of limits) drop all reads */
      invalidTag = FALSE;
      MSG_HIGH("EXIF error reading THUMBNAIL IFD tag %d ",tag_id ,0, 0);
      break;
    }
  }

  /* Calculate thumbnail stream pointer */
  /* Point to offset first, then get the pointer */
  info->thumbnail_stream =  (uint8*)(((uint32*)info->thumbnail_ifd.jpegi_format.offset)[0]);

  /* Get thumbnail stream length */
  /* Point to offset first, then get value */
  info->thumbnail_stream_length = ((uint32*)info->thumbnail_ifd.jpegi_format_l.offset)[0];

  if ((info->thumbnail_stream > info->tiff_header_pointer) &&
      (info->thumbnail_stream < (info->stream + info->stream_length)))
  {
    info->thumbnail_present = TRUE;

    //extract thumbnail dimensions
    jpegd_parse_thumbnail_dimensions(info);

  } else
  {
    info->thumbnail_present = FALSE;
  }
  return(TRUE);
}


/*===========================================================================

FUNCTION      JPEG_JFIFREAD

DESCRIPTION
              Read the JFIF stream.
              Input is the JFIF file stream and length. Output is stored
              in info. 

DEPENDENCIES
  None

RETURN VALUE  Boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean jpeg_jfifread(exif_info_type *info, uint8 *stream, uint32 stream_length)
{
  uint16 header_length;
  uint8 extension_code;
  boolean ret_value = TRUE;

  if ((info == NULL)||(stream == NULL))
  {
    return(FALSE);
  }

  /* Initialize the JPEG compression object. */
  info->is_exif_stream = FALSE;
  info->is_big_endian = TRUE;
  info->stream = stream;
  info->stream_length = stream_length;
  info->pointer = stream;

  exif_init_info(info);
  if (jpeg_exifread_compare_2bytes(info, SOI) == FALSE)
  {
    ret_value = FALSE;
  }
  /* check that this is a JFIF file */
  else if (jpeg_exifread_compare_2bytes(info, APP0) == FALSE)
  {
    ret_value = FALSE;
  }
  else
  {
    /* Check the length */
    header_length = jpeg_exifread_get_2bytes(info);
    /* If the header length is just 16 bytes, then this is JFIF w/o thumb */
    if (header_length == JFIF_PLAIN_HEADER_LENGTH)
    {
      /* This is just a JFIF without thumbnail */
      if (jpeg_exifread_compare_string(info, "JFIF") == FALSE) 
      {
        ret_value = FALSE;
      }
      else
      {      

        info->JFIF_major_version = *info->pointer++;
        info->JFIF_minor_version = *info->pointer++;
        info->density_unit = *info->pointer++;
        info->X_density = jpeg_exifread_get_2bytes(info);
        info->Y_density = jpeg_exifread_get_2bytes(info);
        info->thumbnail_width = *info->pointer++;
        info->thumbnail_height = *info->pointer++;

        //Check for EXIF header
        if (jpeg_exifread_compare_2bytes(info, APP1) == FALSE)
        {
          ret_value = FALSE;
        }
        else
        {
          header_length = jpeg_exifread_get_2bytes(info);

          if (jpeg_exifread_compare_string(info, "Exif") == FALSE)
          {
            ret_value = FALSE;
          }
          else
          {
            info->is_exif_stream = TRUE;
            info->pointer +=1; /* skip the pad field */
            info->tiff_header_pointer = info->pointer;
            if (jpeg_exifread_compare_2bytes(info, 0x4D4D) == FALSE)
            {
              info->is_big_endian = FALSE;
            }
            if (jpeg_exifread_compare_2bytes(info, 0x002a) == FALSE)
            {
              ret_value = FALSE;
            }
            else if (jpeg_exifread_get_zero_ifd(info) == FALSE)
            {
              ret_value = FALSE;
            }
            else if (jpeg_exifread_get_exif_ifd(info) == FALSE)
            {
              ret_value = FALSE;
            }
            else if (jpeg_exifread_get_gps_ifd(info) == FALSE)
            {
              ret_value = FALSE;
            }
            else if (jpeg_exifread_get_thumbnail_ifd(info) == FALSE)
            {
              ret_value = FALSE;
            }
          }
        }
      }
    }
    else if (header_length > JFIF_PLAIN_HEADER_LENGTH)
    {
      /* This is a JFIF with a thumb */
      if (jpeg_exifread_compare_string(info, "JFXX") == FALSE) 
      {
        ret_value = FALSE;
      }
      else
      {        
        /*Check to see if this is a JPEG-coded Thumbnail */
        extension_code = *info->pointer++;
        if (extension_code == JPEG_THUMBNAIL_EXTENSION_CODE)
        {
          /* This is a thumbnail coded using JPEG */
          info->thumbnail_present = TRUE;
          info->thumbnail_stream_length = header_length;
          info->thumbnail_stream = info->pointer;
        } 
        else
        {
          ret_value = FALSE;
        }
      }
    } 
    else
    {
      ret_value = FALSE;
    }
  }

  if(ret_value == TRUE)
    jpegd_parse_main_image_dimensions(info);
  
  if (ret_value == FALSE)
  {
    exif_terminate(info);
  }
  return ret_value;
}


/*===========================================================================

FUNCTION      JPEG_EXIFREAD

DESCRIPTION
              Read the EXIF stream.
              Input is the EXIF file stream and length. Output is stored
              in info. info->exif_stream should be checked on return to
              make sure the stream conforms to EXIF SPEC.

DEPENDENCIES
  None

RETURN VALUE  Boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean jpeg_exifread
(
exif_info_type *info,
uint8 *stream,
uint32 stream_length
)
{
  boolean ret_value = TRUE;

  if ((info == NULL)||(stream == NULL))
  {
    return(FALSE);
  }
  /* Initialize the JPEG compression object. */
  info->is_exif_stream = FALSE;
  info->is_big_endian = TRUE;
  info->stream = stream;
  info->stream_length = stream_length;
  info->pointer = stream;

  exif_init_info(info);
  if (jpeg_exifread_compare_2bytes(info, SOI) == FALSE)
  {
    ret_value = FALSE;
  }
  else if (jpeg_exifread_compare_2bytes(info, APP1) == FALSE)
  {
    ret_value = FALSE;
  }
  else
  {
    info->pointer +=2; /* skip the APP1 length field */
    if (jpeg_exifread_compare_string(info, "Exif") == FALSE)
    {
      ret_value = FALSE;
    }
    else
    {
      info->is_exif_stream = TRUE;
      info->pointer +=1; /* skip the pad field */
      info->tiff_header_pointer = info->pointer;
      if (jpeg_exifread_compare_2bytes(info, 0x4D4D) == FALSE)
      {
        info->is_big_endian = FALSE;
      }
      if (jpeg_exifread_compare_2bytes(info, 0x002a) == FALSE)
      {
        ret_value = FALSE;
      }
      else if (jpeg_exifread_get_zero_ifd(info) == FALSE)
      {
        ret_value = FALSE;
      }
      else if (jpeg_exifread_get_exif_ifd(info) == FALSE)
      {
        ret_value = FALSE;
      }
      else if (jpeg_exifread_get_gps_ifd(info) == FALSE)
      {
        ret_value = FALSE;
      }
      else if (jpeg_exifread_get_thumbnail_ifd(info) == FALSE)
      {
        ret_value = FALSE;
      }
    }
  }
  
  if(ret_value == TRUE)
    jpegd_parse_main_image_dimensions(info);

  if (ret_value == FALSE)
  {
    exif_terminate(info);
  }
  return ret_value;
}

/*===========================================================================

FUNCTION      EXIF_GET_VALUE  

DESCRIPTION   Given a tag, this function determines which directory the tag is 
              located in (zero_ifd, exif_ifd, or gps_ifd), passes
              the info to the appropriate function, and eventually returns a 
              enum value, either Success, Failure, or unidentified tag. The 
              exif_valueinfo_type structure passed in will have its fields
              updated with the desired return values. Whoever calls this function
              must immediately get out the value from the structure and save it
              on their end.
              
             

DEPENDENCIES
  None

RETURN VALUE  exif_get_value_type
  

SIDE EFFECTS
  None

===========================================================================*/

exif_get_value_type exif_get_value (exif_valueinfo_type *values, const exif_info_type *info,
                                    uint16 tag_id)

{

  /* First we check to see if the tag is 0x202 (258) which checks to see the
     length of the thumbnail. If it is 0, there is no thumb, otherwise there is */
  if (tag_id == THUMBNAIL_LENGTH)
  {
    if (((uint32*)info->thumbnail_ifd.jpegi_format_l.offset)[0] != 0)
    {
      values->type = 3;
      values->count = 1;
      values->value = 1;
      return(EXIF_GET_VALUE_SUCCESS);
    }
  }

  /* if the tag id is less than 0x20, (32 in decimal), that means that this
     tag corresponds to the gps_ifd structure. */
  if (tag_id < GPS_IFD_MAX_VALUE)
  {
    return(exif_get_gps_value(info, values, tag_id));

  }

  /* if the tag id is >= 0x9000 (13284 in decimal) tag is in exif_ifd structure */
  if (tag_id >= EXIF_IFD_MIN_VALUE)
  {
    return(exif_get_exif_value(info, values, tag_id));
  }

  /* if tag id is between 0x104 and 0x129 (260 and 297 dec.) then it is in 
     zero_ifd structure. */
  if (tag_id > ZERO_IFD_MIN_VALUE && tag_id < ZERO_IFD_MAX_VALUE)
  {
    return(exif_get_zero_value(info, values, tag_id));
  } else
  {
    return(EXIF_UNIDENTIFIED_TAG);
  }
}


/*===========================================================================

FUNCTION      EXIF_GETVALUEINFO  

DESCRIPTION   Takes in a info, tag, and value structs. If the count of the 
              size of the data is 4 bytes or less, we place the desired value
              simply into value in the value struct. Otherwise, if the size of 
              the data is greater than 4 bytes, we know that we need to copy 
              the offset pointer to our value struct.
              
             

DEPENDENCIES
  None

RETURN VALUE  exif_get_value_type 
  

SIDE EFFECTS
  None

===========================================================================*/
exif_get_value_type exif_getValueInfo (const exif_tag_type *tag, exif_valueinfo_type *values)
{
  values->type = tag->type;
  values->count = tag->count;
  values->offset = tag->offset;
  return(EXIF_GET_VALUE_SUCCESS);
}

/*===========================================================================

FUNCTION      EXIF_GET_ZERO_VALUE  

DESCRIPTION   This is called by exif_get_value and is passed in a pointer to 
              the info and value structures, along with a tag id. Based on the
              tag id, this function determines which tag this id corresponds to
              within the zero ifd, and returns the necessary info to the value
              structure
              
             

DEPENDENCIES
  None

RETURN VALUE exif_get_value_type 
  

SIDE EFFECTS
  None

===========================================================================*/

exif_get_value_type exif_get_zero_value (const exif_info_type *info,exif_valueinfo_type *values,
                                         uint16 tag_id)
{
  switch (tag_id)
  {
    case  IMAGE_DESCRIPTION:
      return(exif_getValueInfo(&info->zero_ifd.image_description, values));

    case  MAKE:
      return(exif_getValueInfo(&info->zero_ifd.make, values));

    case  MODEL:
      return(exif_getValueInfo(&info->zero_ifd.model, values));

    case  ORIENTATION:
      return(exif_getValueInfo(&info->zero_ifd.orientation, values));

    case  X_RESOLUTION:
      return(exif_getValueInfo(&info->zero_ifd.x_resolution, values));

    case  Y_RESOLUTION:
      return(exif_getValueInfo(&info->zero_ifd.y_resolution, values));

    case  RESOLUTION_UNIT:
      return(exif_getValueInfo(&info->zero_ifd.resolution_unit, values));

    case  YCBCR_POSITIONING:
      return(exif_getValueInfo(&info->zero_ifd.ycbcr_positioning, values));

    case SOFTWARE:
      return(exif_getValueInfo(&info->zero_ifd.software, values));

    default:
      return(EXIF_UNIDENTIFIED_TAG);
  }
}

/*===========================================================================

FUNCTION      EXIF_GET_GPS_VALUE  

DESCRIPTION   This is called by exif_get_value and is passed in a pointer to 
              the info and value structures, along with a tag id. Based on the
              tag id, this function determines which tag this id corresponds to
              within the gps ifd, and returns the necessary info to the value
              structure
              
             

DEPENDENCIES
  None

RETURN VALUE  exif_get_value_type 
  

SIDE EFFECTS
  None

===========================================================================*/

exif_get_value_type exif_get_gps_value (const exif_info_type *info,exif_valueinfo_type *values,
                                        uint16 tag_id)
{
  switch (tag_id)
  {
    case  GPS_VERSION_ID:
      return(exif_getValueInfo(&info->gps_ifd.version_id, values));

    case  GPS_LATITUDE_REF:
      return(exif_getValueInfo(&info->gps_ifd.latitude_ref, values));

    case  GPS_LATITUDE:
      return(exif_getValueInfo(&info->gps_ifd.latitude, values));

    case  GPS_LONGITUDE_REF:
      return(exif_getValueInfo(&info->gps_ifd.longitude_ref, values));

    case  GPS_LONGITUDE:
      return(exif_getValueInfo(&info->gps_ifd.longitude, values));

    case  GPS_ALTITUDE_REF:
      return(exif_getValueInfo(&info->gps_ifd.altitude_ref, values));

    case  GPS_ALTITUDE:
      return(exif_getValueInfo(&info->gps_ifd.altitude, values));

    case  GPS_TIMESTAMP:
      return(exif_getValueInfo(&info->gps_ifd.timestamp, values));

    case  GPS_MAP_DATUM:
      return(exif_getValueInfo(&info->gps_ifd.map_datum, values));

    case  GPS_AREA_INFORMATION:
      return(exif_getValueInfo(&info->gps_ifd.area_information, values));

    case  GPS_DATESTAMP:
      return(exif_getValueInfo(&info->gps_ifd.datestamp, values));

    default:
      return(EXIF_UNIDENTIFIED_TAG);
  }
}

/*===========================================================================

FUNCTION      EXIF_GET_EXIF_VALUE  

DESCRIPTION   This is called by exif_get_value and is passed in a pointer to 
              the info and value structures, along with a tag id. Based on the
              tag id, this function determines which tag this id corresponds to
              within the exif ifd, and returns the necessary info to the value
              structure            

DEPENDENCIES
  None

RETURN VALUE  exif_get_value_type 
  
SIDE EFFECTS
  None

===========================================================================*/

exif_get_value_type exif_get_exif_value (const exif_info_type *info,exif_valueinfo_type *values,
                                         uint16 tag_id)
{
  switch (tag_id)
  {
    case  EXIF_VERSION:
      return(exif_getValueInfo(&info->exif_ifd.version, values));

    case  EXIF_DATE_TIME_ORIGINAL:
      return(exif_getValueInfo(&info->exif_ifd.date_time_original, values));

    case  EXIF_COMPONENTS_CONFIGURATION:
      return(exif_getValueInfo(&info->exif_ifd.components_configuration, values));

    case  EXIF_USER_COMMENT:
      return(exif_getValueInfo(&info->exif_ifd.user_comment, values));

    case  EXIF_MAKER_NOTE:
      return(exif_getValueInfo(&info->exif_ifd.maker_note, values));

    case  EXIF_FLASHPIX_VERSION:
      return(exif_getValueInfo(&info->exif_ifd.flashpix_version, values));

    case  EXIF_PIXEL_X_DIMENSION:
      return(exif_getValueInfo(&info->exif_ifd.pixel_x_dimension, values));

    case  EXIF_PIXEL_Y_DIMENSION:
      return(exif_getValueInfo(&info->exif_ifd.pixel_y_dimension, values));

    case  EXPOSURE_TIME:
      return(exif_getValueInfo(&info->exif_ifd.exposure_time, values));

    case  EXIF_DATE_TIME_DIGITIZED:
      return(exif_getValueInfo(&info->exif_ifd.date_time_digitized, values));

    case  FLASH:
      return(exif_getValueInfo(&info->exif_ifd.flash, values));

    case  CUSTOM_RENDERED:
      return(exif_getValueInfo(&info->exif_ifd.custom_rendered, values));

    case  EXPOSURE_MODE:
      return(exif_getValueInfo(&info->exif_ifd.exposure_mode, values));    

    case  WHITE_BALANCE:
      return(exif_getValueInfo(&info->exif_ifd.white_balance, values));

    case  DIGITAL_ZOOM_RATIO:
      return(exif_getValueInfo(&info->exif_ifd.digital_zoom_ratio, values));

    case  SCENE_CAPTURE_TYPE:
      return(exif_getValueInfo(&info->exif_ifd.scene_capture_type, values));

    case  SUBJECT_DISTANCE_RANGE:
      return(exif_getValueInfo(&info->exif_ifd.subject_distance_range, values));

    case EXIF_COLOR_SPACE:
      return(exif_getValueInfo(&info->exif_ifd.color_space, values));

    default:
      return(EXIF_UNIDENTIFIED_TAG);
  }
}

/*===========================================================================

FUNCTION      EXIF_SET_VALUE  

DESCRIPTION   This function takes in a exif_valueinfo_type struct and sets
              the correct value in our struct with the new, passed-in value.
              Returns FALSE if the boolean was not set successfully, otherwise
              it returns TRUE.
              
             

DEPENDENCIES
  None

RETURN VALUE  boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean exif_set_value (exif_info_type *info, const exif_valueinfo_type *values,
                        uint16 tag_id)
{
  /* if the tag id is less than 0x20, (32 in decimal), that means that this
     tag corresponds to the gps_ifd structure */
  if (tag_id < GPS_IFD_MAX_VALUE)
  {
    return(exif_set_gps_value(info, values, tag_id));
  }

  /* if the tag id is >= 0x9000 (13284 in decimal) tag is in exif_ifd structure */
  if (tag_id >= EXIF_IFD_MIN_VALUE)
  {
    return(exif_set_exif_value(info, values, tag_id));

  }

  /* if tag id is between 0x104 and 0x214  then it is in 
     zero_ifd structure */
  if (tag_id > ZERO_IFD_MIN_VALUE && tag_id < ZERO_IFD_MAX_VALUE)
  {
    return(exif_set_zero_value(info, values, tag_id));
  } else
  {
    return(FALSE);
  }
}

/*===========================================================================

FUNCTION      EXIF_SET_GPS_VALUE  

DESCRIPTION   This is called by exif_set_value and is passed in a pointer to 
              the info and value structures (containing the new information,
              along with a tag id. Based on the
              tag id, this function determines which tag this id corresponds to
              within the gps ifd, and sets the new value appropriately
              
             

DEPENDENCIES
  None

RETURN VALUE  boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean exif_set_gps_value (exif_info_type *info, const exif_valueinfo_type *values,
                            uint16 tag_id)
{
  switch (tag_id)
  {
    case  GPS_VERSION_ID:
      return(exif_change_value(&info->gps_ifd.version_id, values)); 

    case  GPS_LATITUDE_REF:
      return(exif_change_value(&info->gps_ifd.latitude_ref, values)); 

    case  GPS_LATITUDE:
      return(exif_change_value(&info->gps_ifd.latitude, values)); 

    case  GPS_LONGITUDE_REF:
      return(exif_change_value(&info->gps_ifd.longitude_ref, values)); 

    case  GPS_LONGITUDE:
      return(exif_change_value(&info->gps_ifd.longitude, values)); 

    case  GPS_ALTITUDE_REF:
      return(exif_change_value(&info->gps_ifd.altitude_ref, values)); 

    case  GPS_ALTITUDE:
      return(exif_change_value(&info->gps_ifd.altitude, values)); 

    case  GPS_TIMESTAMP:
      return(exif_change_value(&info->gps_ifd.timestamp, values)); 

    case  GPS_MAP_DATUM:
      return(exif_change_value(&info->gps_ifd.map_datum, values)); 

    case  GPS_AREA_INFORMATION:
      return(exif_change_value(&info->gps_ifd.area_information, values)); 

    case  GPS_DATESTAMP:
      return(exif_change_value(&info->gps_ifd.datestamp, values)); 

    default:
      return(FALSE);
  }
}


/*===========================================================================

FUNCTION      EXIF_SET_EXIF_VALUE  

DESCRIPTION   This is called by exif_get_value and is passed in a pointer to 
              the info and value structures (containing the new information), 
              along with a tag id. Based on the
              tag id, this function determines which tag this id corresponds to
              within the exif ifd, and sets the variable to the new value appropriately
             

DEPENDENCIES
  None

RETURN VALUE  void 
  

SIDE EFFECTS
  None

===========================================================================*/

boolean exif_set_exif_value (exif_info_type *info, const exif_valueinfo_type *values,
                             uint16 tag_id)
{
  switch (tag_id)
  {
    case  EXIF_VERSION:
      return(exif_change_value(&info->exif_ifd.version, values)); 

    case  EXIF_DATE_TIME_ORIGINAL:
      return(exif_change_value(&info->exif_ifd.date_time_original, values)); 

    case  EXIF_COMPONENTS_CONFIGURATION:
      return(exif_change_value(&info->exif_ifd.components_configuration, values)); 

    case  EXIF_USER_COMMENT:
      return(exif_change_value(&info->exif_ifd.user_comment, values)); 

    case  EXIF_MAKER_NOTE:
      return(exif_change_value(&info->exif_ifd.maker_note, values)); 

    case  EXIF_FLASHPIX_VERSION:
      return(exif_change_value(&info->exif_ifd.flashpix_version, values)); 

    case  EXIF_PIXEL_X_DIMENSION:
      return(exif_change_value(&info->exif_ifd.pixel_x_dimension, values)); 

    case  EXIF_PIXEL_Y_DIMENSION:
      return(exif_change_value(&info->exif_ifd.pixel_y_dimension, values)); 

    case  EXPOSURE_TIME:
      return(exif_change_value(&info->exif_ifd.exposure_time, values));

    case  EXIF_DATE_TIME_DIGITIZED:
      return(exif_change_value(&info->exif_ifd.date_time_digitized, values));

    case  FLASH:
      return(exif_change_value(&info->exif_ifd.flash, values));

    case  CUSTOM_RENDERED:
      return(exif_change_value(&info->exif_ifd.custom_rendered, values));

    case  EXPOSURE_MODE:
      return(exif_change_value(&info->exif_ifd.exposure_mode, values));    

    case  WHITE_BALANCE:
      return(exif_change_value(&info->exif_ifd.white_balance, values));

    case  DIGITAL_ZOOM_RATIO:
      return(exif_change_value(&info->exif_ifd.digital_zoom_ratio, values));

    case  SCENE_CAPTURE_TYPE:
      return(exif_change_value(&info->exif_ifd.scene_capture_type, values));

    case  SUBJECT_DISTANCE_RANGE:
      return(exif_change_value(&info->exif_ifd.subject_distance_range, values));

    case EXIF_COLOR_SPACE:
      return(exif_change_value(&info->exif_ifd.color_space, values));

    case F_NUMBER:
      return(exif_change_value(&info->exif_ifd.f_number, values));

    case APERTURE:
      return(exif_change_value(&info->exif_ifd.aperture, values));

    case MAX_APERTURE:
      return(exif_change_value(&info->exif_ifd.max_aperture, values));

    case FOCAL_LENGTH:
      return(exif_change_value(&info->exif_ifd.focal_length, values));

    case FOCAL_LENGTH_35MM:
      return(exif_change_value(&info->exif_ifd.focal_length_35mm, values));

    case GAIN_CONTROL:
      return(exif_change_value(&info->exif_ifd.gain_control, values));

    case EXPOSURE_PROGRAM:
      return(exif_change_value(&info->exif_ifd.exposure_program, values));

    case ISO_SPEED_RATINGS:
      return(exif_change_value(&info->exif_ifd.iso_speed_ratings, values));

    case LIGHT_SOURCE:
      return(exif_change_value(&info->exif_ifd.light_source, values));

    case SENSING_METHOD:
      return(exif_change_value(&info->exif_ifd.sensing_method, values));

    case FILE_SOURCE:
      return(exif_change_value(&info->exif_ifd.file_source, values));

    case CONTRAST:
      return(exif_change_value(&info->exif_ifd.contrast, values));

    case SATURATION:
      return(exif_change_value(&info->exif_ifd.saturation, values));

    case METERING_MODE:
      return(exif_change_value(&info->exif_ifd.metering_mode, values));

    case SHARPNESS:
      return(exif_change_value(&info->exif_ifd.sharpness, values));

    case CFA_PATTERN:
      return(exif_change_value(&info->exif_ifd.cfa_pattern, values));

    case OECF:
      return(exif_change_value(&info->exif_ifd.oecf, values));

    case SHUTTER_SPEED:
      return(exif_change_value(&info->exif_ifd.shutter_speed, values));

    case BRIGHTNESS:
      return(exif_change_value(&info->exif_ifd.brightness, values));

    case SUBJECT_DISTANCE:
      return(exif_change_value(&info->exif_ifd.subject_distance, values));

    case SUBJECT_LOCATION:
      return(exif_change_value(&info->exif_ifd.subject_location, values));

    case EXPOSURE_INDEX:
      return(exif_change_value(&info->exif_ifd.exposure_index, values));

    case SCENE_TYPE:
      return(exif_change_value(&info->exif_ifd.scene_type, values));

    default:
      return(FALSE);
  }
}

/*===========================================================================

FUNCTION      EXIF_SET_ZERO_VALUE  

DESCRIPTION   This is called by exif_get_value and is passed in a pointer to 
              the info and value structures (which contain the new info to set, 
              along with a tag id. Based on the
              tag id, this function determines which tag this id corresponds to
              within the zero ifd, and sets the variable to the new information
                          
DEPENDENCIES
  None

RETURN VALUE  boolean; TRUE if set correctly, FALSE otherwise 
  
SIDE EFFECTS
  None
===========================================================================*/

boolean exif_set_zero_value (exif_info_type *info, const exif_valueinfo_type *values,
                             uint16 tag_id)
{
  switch (tag_id)
  {
    case  IMAGE_DESCRIPTION:
      return(exif_change_value(&info->zero_ifd.image_description, values));  

    case  MAKE:
      return(exif_change_value(&info->zero_ifd.make, values)); 

    case  MODEL:
      return(exif_change_value(&info->zero_ifd.model, values)); 

    case  ORIENTATION:        
      return(exif_change_value(&info->zero_ifd.orientation, values)); 

    case  X_RESOLUTION:
      return(exif_change_value(&info->zero_ifd.x_resolution, values)); 

    case  Y_RESOLUTION:
      return(exif_change_value(&info->zero_ifd.y_resolution, values)); 

    case  RESOLUTION_UNIT:
      return(exif_change_value(&info->zero_ifd.resolution_unit, values)); 

    case  YCBCR_POSITIONING:
      return(exif_change_value(&info->zero_ifd.ycbcr_positioning, values)); 

    case SOFTWARE:
      return(exif_change_value(&info->zero_ifd.software, values));

    default:
      return(FALSE);
  }
}  




/*===========================================================================

FUNCTION      EXIF_CHANGE_VALUE  

DESCRIPTION   Given the pointers to the exif_tag_type structure, and an
              exif_valueinfo_type with the new values, this function
              sets the variables to the new values.
              
             

DEPENDENCIES
  None

RETURN VALUE  boolean
  

SIDE EFFECTS
  None

===========================================================================*/

boolean exif_change_value (exif_tag_type *tag, const exif_valueinfo_type *values)
{
  uint16 typesize = 0;

  /* checks that the correct type is being set */
  if (values->type != tag->type)
  {
    return(FALSE);
  }

  switch (tag->type)
  {
    case EXIF_BYTE:
    case EXIF_ASCII:
    case EXIF_UNDEFINED:
      typesize = sizeof(uint8);
      break;
    case EXIF_SHORT:
      typesize = sizeof(uint16);
      break;
    case EXIF_LONG:
    case EXIF_SLONG:
      typesize = sizeof(uint32);
      break;
    case EXIF_RATIONAL:
    case EXIF_SRATIONAL:
      typesize = 2*sizeof(uint32);
      break;
    default:
      typesize = 0;
      break;
  }

  if (!tag->fixedLength)
  {
    JPEG_FREE(tag->offset);
    tag->offset = NULL;
    tag->offset = JPEG_MALLOC(typesize*values->count);
    if (tag->offset != NULL)
    {
      memcpy(tag->offset, values->offset, typesize*values->count); 
    }
  } else
  {
    if (tag->offset != NULL)
    {
      memcpy(tag->offset, values->offset, typesize*tag->count); 
    }
  }

  tag->count = values->count;
  tag->active = TRUE;
  return(TRUE);
}

/*===========================================================================

FUNCTION      EXIF_SET_ATTRIBUTE_INFO  

DESCRIPTION   Set tags corresponding to the attributes that are set in 
              exif_attr
             

DEPENDENCIES
  None

RETURN VALUE  void
  

SIDE EFFECTS
  None

===========================================================================*/
void exif_set_attribute_info(exif_info_type *info, const exif_attributes_type *exif_attr)
{
  exif_valueinfo_type tmp;

  tmp.offset = (void*)JPEG_MALLOC(EXIF_MAX_TAG_SIZE);

  if (!tmp.offset)
  {
    MSG_HIGH("EIXF Error in exif_set_attributes() : Not enough memory", 0, 0, 0);
    return;
  }

  if (exif_attr->sensor_info.exposureTime.active)
  {
    tmp.type = (uint16)EXIF_RATIONAL;
    tmp.count = 1;
    ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->sensor_info.exposureTime.value;
    ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    (void)exif_set_value(info, &tmp, (uint16)EXPOSURE_TIME);
  }

  if (exif_attr->sensor_info.fNumber.active)
  {
    tmp.type = (uint16)EXIF_RATIONAL;
    tmp.count=1;
    ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->sensor_info.fNumber.value;
    ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    (void)exif_set_value(info, &tmp, (uint16)F_NUMBER);
  }

  if (exif_attr->UI_info.exposureProgram.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->UI_info.exposureProgram.value;
    (void)exif_set_value(info, &tmp, (uint16)EXPOSURE_PROGRAM);
  }

  if (exif_attr->sensor_info.aperture.active)
  {
    tmp.type = (uint16)EXIF_RATIONAL;
    tmp.count=1;
    ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->sensor_info.aperture.value;
    ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    (void)exif_set_value(info, &tmp, (uint16)APERTURE);
  }

  if (exif_attr->sensor_info.max_aperture.active)
  {
    tmp.type = (uint16)EXIF_RATIONAL;
    tmp.count=1;
    ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->sensor_info.max_aperture.value;
    ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    (void)exif_set_value(info, &tmp, (uint16)MAX_APERTURE);
  }

  if (exif_attr->camera_info.flash.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = exif_attr->camera_info.flash.value;
    (void)exif_set_value(info, &tmp, (uint16)FLASH);
  }

  if (exif_attr->sensor_info.focalLength.active)
  {
    tmp.type = (uint16)EXIF_RATIONAL;
    tmp.count=1;
    ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->sensor_info.focalLength.value;
    ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    (void)exif_set_value(info, &tmp, (uint16)FOCAL_LENGTH);
  }

  if (exif_attr->sensor_info.sensingMethod.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->sensor_info.sensingMethod.value;
    (void)exif_set_value(info, &tmp, (uint16)SENSING_METHOD);
  }

  if (exif_attr->sensor_info.cfaPattern.active)
  {

    if (exif_attr->sensor_info.cfaPattern.value == GRBG_CFA)
    {
      /* Bayer pattern is GRBG */
      tmp.type = (uint16)EXIF_UNDEFINED;
      tmp.count=6;
      ((int16*)tmp.offset)[0] = 2;
      ((int16*)tmp.offset)[1] = 2;
      ((byte*)tmp.offset)[1] = 1;
      ((byte*)tmp.offset)[1] = 0;
      ((byte*)tmp.offset)[1] = 2;
      ((byte*)tmp.offset)[1] = 1;
      (void)exif_set_value(info, &tmp, (uint16)CFA_PATTERN);
    } else if (exif_attr->sensor_info.cfaPattern.value == GBRG_CFA)
    {
      /* Bayer pattern is GBRG */
      tmp.type = (uint16)EXIF_UNDEFINED;
      tmp.count=6;
      ((int16*)tmp.offset)[0] = 2;
      ((int16*)tmp.offset)[1] = 2;
      ((byte*)tmp.offset)[1] = 1;
      ((byte*)tmp.offset)[1] = 2;
      ((byte*)tmp.offset)[1] = 0;
      ((byte*)tmp.offset)[1] = 1;
      (void)exif_set_value(info, &tmp, (uint16)CFA_PATTERN);
    }
  }

  if (exif_attr->UI_info.customIP.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->UI_info.customIP.value;
    (void)exif_set_value(info, &tmp, (uint16)CUSTOM_RENDERED);
  }

  if (exif_attr->UI_info.exposureMode.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->UI_info.exposureMode.value;
    (void)exif_set_value(info, &tmp, (uint16)EXPOSURE_MODE);
  }

  if (exif_attr->UI_info.whiteBalance.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->UI_info.whiteBalance.value;
    (void)exif_set_value(info, &tmp, (uint16)WHITE_BALANCE);
  }

  if (exif_attr->UI_info.digitalZoomRatio.active)
  {
    tmp.type = (uint16)EXIF_RATIONAL;
    tmp.count=1;       
    if (exif_attr->UI_info.digitalZoomRatio.value != 1)
    {
      ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->UI_info.digitalZoomRatio.value;
      ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    } else
    {
      ((exif_rational_type*)tmp.offset)[0].numerator = 0;
      ((exif_rational_type*)tmp.offset)[0].denominator = 0;
    }
    (void)exif_set_value(info, &tmp, (uint16)DIGITAL_ZOOM_RATIO);
  }

  if (exif_attr->sensor_info.focalLength35mm.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((uint16*)tmp.offset)[0] = exif_attr->sensor_info.focalLength35mm.value;
    (void)exif_set_value(info, &tmp, (uint16)FOCAL_LENGTH_35MM);
  }

  if (exif_attr->UI_info.contrast.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->UI_info.contrast.value;
    (void)exif_set_value(info, &tmp, (uint16)CONTRAST);
  }

  if (exif_attr->UI_info.saturation.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->UI_info.saturation.value;
    (void)exif_set_value(info, &tmp, (uint16)SATURATION);
  }

  if (exif_attr->UI_info.sharpness.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->UI_info.sharpness.value;
    (void)exif_set_value(info, &tmp, (uint16)SHARPNESS);
  }

  if (exif_attr->UI_info.meteringMode.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->UI_info.meteringMode.value;
    (void)exif_set_value(info, &tmp, (uint16)METERING_MODE);
  }

  if (exif_attr->vfe_info.gainControl.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->vfe_info.gainControl.value;
    (void)exif_set_value(info, &tmp, (uint16)GAIN_CONTROL);
  }

  if (exif_attr->sensor_info.oecf.active)
  {
    tmp.type = (uint16)EXIF_UNDEFINED;
    tmp.count=1;
    ((uint16*)tmp.offset)[0] = exif_attr->sensor_info.oecf.value;
    (void)exif_set_value(info, &tmp, (uint16)OECF);
  }

  if (exif_attr->sensor_info.shutterSpeed.active)
  {
    tmp.type = (uint16)EXIF_SRATIONAL;
    tmp.count=1;
    ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->sensor_info.shutterSpeed.value;
    ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    (void)exif_set_value(info, &tmp, (uint16)SHUTTER_SPEED);
  }

  if (exif_attr->UI_info.brightness.active)
  {
    tmp.type = (uint16)EXIF_SRATIONAL;
    tmp.count=1;
    ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->UI_info.brightness.value;
    ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    (void)exif_set_value(info, &tmp, (uint16)BRIGHTNESS);
  }

  if (exif_attr->vfe_info.lightSource.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->vfe_info.lightSource.value;
    (void)exif_set_value(info, &tmp, (uint16)LIGHT_SOURCE);
  }

  if (exif_attr->sensor_info.exposureIndex.active)
  {
    tmp.type = (uint16)EXIF_RATIONAL;
    tmp.count=1;
    ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->sensor_info.exposureIndex.value;
    ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    (void)exif_set_value(info, &tmp, (uint16)EXPOSURE_INDEX);
  }

  if (exif_attr->vfe_info.subjectDistance.active)
  {
    tmp.type = (uint16)EXIF_RATIONAL;
    tmp.count=1;
    ((exif_rational_type*)tmp.offset)[0].numerator = (int32)exif_attr->vfe_info.subjectDistance.value;
    ((exif_rational_type*)tmp.offset)[0].denominator = 1024;
    (void)exif_set_value(info, &tmp, (uint16)SUBJECT_DISTANCE);
  }

  if (exif_attr->vfe_info.subjectLocation.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((uint16*)tmp.offset)[0] = exif_attr->vfe_info.subjectLocation.value;
    (void)exif_set_value(info, &tmp, (uint16)SUBJECT_LOCATION);
  }

  if (exif_attr->UI_info.sceneCaptureType.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->UI_info.sceneCaptureType.value;
    (void)exif_set_value(info, &tmp, (uint16)SCENE_CAPTURE_TYPE);
  }

  if (exif_attr->vfe_info.subjectDistanceRange.active)
  {
    tmp.type = (uint16)EXIF_SHORT;
    tmp.count=1;
    ((int16*)tmp.offset)[0] = (int16)exif_attr->vfe_info.subjectDistanceRange.value;
    (void)exif_set_value(info, &tmp, (uint16)SUBJECT_DISTANCE_RANGE);
  }
  JPEG_FREE(tmp.offset);
}

/*===========================================================================

FUNCTION      EXIF_SET_RESTART  

DESCRIPTION   Set restart count
             

DEPENDENCIES
  None

RETURN VALUE  none
  

SIDE EFFECTS
  None

===========================================================================*/
void exif_set_restart(exif_info_type *info, uint16 dri)
{
  if (info)
  {
    info->restart_cnt = dri;
  }
}


/*===========================================================================

FUNCTION      EXIF_SET_SUBSAMPLE  

DESCRIPTION   Set color subsample
             

DEPENDENCIES
  None

RETURN VALUE  none
  

SIDE EFFECTS
  None

===========================================================================*/
void exif_set_subsample(exif_info_type *info, exif_subsample_type sample)
{
  if (info)
  {
    info->subsampling = sample;
  }
}

/*===========================================================================

FUNCTION     EXIF_GET_VAR_EXIF_IFD_COUNT  

DESCRIPTION   Count active EXIF tags
             

DEPENDENCIES
  None

RETURN VALUE  Number of active EXIF tags
  

SIDE EFFECTS
  None

===========================================================================*/
uint16 exif_get_var_exif_ifd_count(const exif_info_type *info)
{
  uint16 count = 0;

  if (info->exif_ifd.exposure_time.active) count++;
  if (info->exif_ifd.f_number.active) count++;
  if (info->exif_ifd.oecf.active) count++;
  if (info->exif_ifd.shutter_speed.active) count++;
  if (info->exif_ifd.exposure_program.active) count++;
  if (info->exif_ifd.iso_speed_ratings.active) count++;
  if (info->exif_ifd.aperture.active) count++;
  if (info->exif_ifd.brightness.active) count++;
  if (info->exif_ifd.max_aperture.active) count++;
  if (info->exif_ifd.light_source.active) count++;
  if (info->exif_ifd.focal_length.active) count++;
  if (info->exif_ifd.exposure_index.active) count++;
  if (info->exif_ifd.sensing_method.active) count++;
  if (info->exif_ifd.subject_distance.active) count++;
  if (info->exif_ifd.subject_location.active) count++;
  if (info->exif_ifd.cfa_pattern.active) count++;
  if (info->exif_ifd.focal_length_35mm.active) count++;
  if (info->exif_ifd.contrast.active) count++;
  if (info->exif_ifd.saturation.active) count++;
  if (info->exif_ifd.metering_mode.active) count++;
  if (info->exif_ifd.sharpness.active) count++;
  if (info->exif_ifd.gain_control.active) count++;
  if (info->exif_ifd.flash.active) count++;
  if (info->exif_ifd.custom_rendered.active) count++;
  if (info->exif_ifd.exposure_mode.active) count++;
  if (info->exif_ifd.white_balance.active) count++;
  if (info->exif_ifd.digital_zoom_ratio.active) count++;
  if (info->exif_ifd.scene_capture_type.active) count++;
  if (info->exif_ifd.subject_distance_range.active) count++;
  if (info->exif_ifd.file_source.active) count++;
  if (info->exif_ifd.scene_type.active) count++;
  if (info->exif_ifd.version.active) count++;
  if (info->exif_ifd.date_time_original.active) count++;
  if (info->exif_ifd.date_time_digitized.active) count++;
  if (info->exif_ifd.components_configuration.active) count++;
  if (info->exif_ifd.user_comment.active) count++;
  if (info->exif_ifd.maker_note.active) count++;
  if (info->exif_ifd.flashpix_version.active) count++;
  if (info->exif_ifd.color_space.active) count++;
  if (info->exif_ifd.pixel_x_dimension.active) count++;
  if (info->exif_ifd.pixel_y_dimension.active) count++;
  if (info->exif_ifd.interop_ifd_pointer.active) count++;
  return(count);
}


#ifdef FEATURE_EXIF_PIM3
boolean exif_PIM3_enable(exif_info_type *info)
{
  char *pim_id = "PrintIM";
  char *pim_version = "0300";
  uint32 i = 0;
  pim_string = (char *) malloc (PIM_LENGTH);
  
  if (!pim_string)
    return FALSE;

  memset(pim_string, sizeof(PIM_LENGTH), 0);

  memcpy(&(pim_string[i]), pim_id, strlen(pim_id));
  i = strlen(pim_id) + 1;

  memcpy(&(pim_string[i]), pim_version, strlen(pim_version));
  i = i + strlen(pim_version) + 3;

  info->zero_ifd.pim.id   = (uint16)PIM_TAG;
  info->zero_ifd.pim.type = (uint16)EXIF_UNDEFINED;
  info->zero_ifd.pim.count = i + 1; 
  info->zero_ifd.pim.offset = pim_string;
  return TRUE;
}
/*===========================================================================

FUNCTION     EXIF_SET_DATE_TIME_FROM_SVCS

DESCRIPTION   Sets the flag indicating that DATE TIME will be set by services
             

DEPENDENCIES
  None

RETURN VALUE  None
  

SIDE EFFECTS
  None

===========================================================================*/
void exif_set_date_time_from_svcs(boolean is_set_date_time)
{
    set_date_time = is_set_date_time;
}

/*===========================================================================
 
FUNCTION     EXIF_GET_DATE_TIME_STRING  
 
DESCRIPTION   Return exif_date_time_string address
              

DEPENDENCIES
  None

RETURN VALUE  Address of exif_date_time_string
  

SIDE EFFECTS
  None

===========================================================================*/
uint8* exif_get_date_time_string()
{
    return exif_date_time_string;
}


#ifdef FEATURE_GEOTAGGING

/*===========================================================================

Function: EXIF_GEOTAGGING_INIT

Description:  This function allocates header buffer for geotagging
Return Value:
              exif_geotagging_ret_type
   
Comments:  None

============================================================================*/
exif_geotagging_ret_type exif_geotagging_init ()
{
    memset(&jpeg_exif_info, 0, sizeof(exif_info_type));

    jpeg_exif_info.buf_ptr = JPEG_MALLOC(JPEG_EXIF_MAX_HEADER_SIZE);

    if(jpeg_exif_info.buf_ptr)
    {
        return EXIF_GEOTAG_SUCCESS;
    }
    else
    {
        MSG_HIGH("Exif ERROR: Unable to allocate APP1 memory of size 64K", 0, 0, 0);
        return EXIF_GEOTAG_MALLOC_FAIL;
    }
}

/*===========================================================================

Function: EXIF_GEOTAGGING_TERMINATE

Description:  This function deallocates all buffers allocated in geotagging
Return Value:
              NONE
   
Comments:  NONE

============================================================================*/
void exif_geotagging_terminate ()
{
    exif_info_type *info = &jpeg_exif_info;
    exif_terminate(info);
    if(info->buf_ptr)
    {
        JPEG_FREE (info->buf_ptr);
        info->buf_ptr = NULL;
        info->buf_len = 0;
    }
}

/*===========================================================================

Function: EXIF_INSERTGEOTAGS

Description:  This function inserts GPS info to an Exif/Jfif file
                    without decoding the image
Return Value:
              exif_geotagging_ret_type
   
Comments:  None

============================================================================*/
exif_geotagging_ret_type exif_insertGeotags ( exif_position_type *gps_info,
                                            uint8 *jpeg_bstream,
                                            uint32 jpeg_bstream_length,
                                            exif_geotagging_output_type *output_info )
{
    uint8 *pMainImagePtr = NULL;
    uint32 mainImageLength = 0, app1_length= 0, header_length;
    exif_info_type  *info = &jpeg_exif_info;


    
    //parse the Exif info first
    if(jpeg_exifread(&jpeg_exif_info, jpeg_bstream, jpeg_bstream_length) == FALSE)
    {
        if(jpeg_jfifread(&jpeg_exif_info, jpeg_bstream, jpeg_bstream_length) == FALSE)
        {
            exif_init_info (&jpeg_exif_info);
            info->is_exif_stream = FALSE;
            info->is_big_endian = TRUE;
            info->stream = jpeg_bstream;
            info->stream_length = jpeg_bstream_length;
            info->pointer = jpeg_bstream;

            if (jpeg_exifread_compare_2bytes(info, SOI) == FALSE)
            {
                return EXIF_GEOTAG_INVALID_STREAM;
            }
            /* check that this is a JFIF file */
            else if (jpeg_exifread_compare_2bytes(info, APP0) == FALSE)
            {
                return EXIF_GEOTAG_INVALID_STREAM;
            }
            else
            {
                header_length = jpeg_exifread_get_2bytes(info);
                /* If the header length is just 16 bytes, then this is JFIF w/o thumb */
                if (header_length == JFIF_PLAIN_HEADER_LENGTH)
                {
                    /* This is just a JFIF without thumbnail */
                    if (jpeg_exifread_compare_string(info, "JFIF") == FALSE) 
                    {
                        return EXIF_GEOTAG_INVALID_STREAM;
                    }
                    else
                    {      
                        info->JFIF_major_version = *info->pointer++;
                        info->JFIF_minor_version = *info->pointer++;
                        info->density_unit = *info->pointer++;
                        info->X_density = jpeg_exifread_get_2bytes(info);
                        info->Y_density = jpeg_exifread_get_2bytes(info);
                        info->thumbnail_width = *info->pointer++;
                        info->thumbnail_height = *info->pointer++;
                    }
                }
            }
        }
    }

    //Get the APP1 or APP0 marker length to calculate main image offset
    app1_length = ( (jpeg_bstream[4] << 8) | jpeg_bstream[5]);

    //copy the GPS info to jpeg_exif_info
    exif_set_position (&jpeg_exif_info, gps_info);

    if(jpeg_exif_info.buf_ptr)
    {
        jpeg_exif_info.buf_len = JPEG_EXIF_MAX_HEADER_SIZE;

        if(jpeg_exif_info.thumbnail_present == TRUE)
            jpeg_exif_info.emit_thumbnail = TRUE;

        //packing of Exif header starting with SOI
        if (exif_emit_soi(&jpeg_exif_info) == FALSE)
        {
            return EXIF_GEOTAG_64K_EXCEEDED;
        }
        else if (exif_emit_app1(&jpeg_exif_info) == FALSE)
        {
            return EXIF_GEOTAG_64K_EXCEEDED;
        }

        if( (jpeg_exif_info.buf_index + jpeg_exif_info.thumbnail_stream_length) > JPEG_EXIF_MAX_HEADER_SIZE)
        {
            MSG_HIGH("Exif ERROR: Header size exceeds Max limit of 64K", 0, 0, 0);
            return EXIF_GEOTAG_64K_EXCEEDED;
        }

        if(jpeg_exif_info.thumbnail_present == TRUE)
        {
            memcpy(&jpeg_exif_info.buf_ptr[jpeg_exif_info.thumbnail_stream_index], jpeg_exif_info.thumbnail_stream, 
                                                                jpeg_exif_info.thumbnail_stream_length);

            jpeg_exif_info.buf_index = jpeg_exif_info.thumbnail_stream_index + jpeg_exif_info.thumbnail_stream_length;

            exif_update_thumbnail(&jpeg_exif_info, 0);

            /* Update jpegi_format */
            jpeg_exif_info.buf_index_save = jpeg_exif_info.buf_index;
            jpeg_exif_info.buf_index = jpeg_exif_info.jpegi_format_index + EXIF_INDEX_OFFSET;
            if (exif_emit_4bytes(&jpeg_exif_info, jpeg_exif_info.thumbnail_stream_index - jpeg_exif_info.tiff_header_index) == FALSE)
              return EXIF_GEOTAG_64K_EXCEEDED;
            

            jpeg_exif_info.buf_index  = jpeg_exif_info.buf_index_save;
            
        }

        jpeg_exif_info.buf_index_save = jpeg_exif_info.buf_index;
        jpeg_exif_info.buf_index      = jpeg_exif_info.length_index;
        if (exif_emit_2bytes(&jpeg_exif_info, (uint16)(jpeg_exif_info.buf_index_save - jpeg_exif_info.length_index)) == FALSE)
        {
            return EXIF_GEOTAG_64K_EXCEEDED ;
        }
        jpeg_exif_info.buf_index = jpeg_exif_info.buf_index_save;

        //pMainImagePtr   = (jpeg_exif_info.thumbnail_stream + jpeg_exif_info.thumbnail_stream_length);
        pMainImagePtr   = jpeg_bstream + app1_length + 4;     // 4bytes for SOI, APP1 marker
        mainImageLength = jpeg_bstream_length - (uint32)(pMainImagePtr - jpeg_bstream);
    }
    else
    {
        MSG_HIGH("Exif ERROR: Exif Geotagging Init failed", 0, 0, 0);
        return EXIF_GEOTAG_MALLOC_FAIL;
    }

    output_info->header_ptr     = jpeg_exif_info.buf_ptr;
    output_info->header_size    = jpeg_exif_info.buf_index;
    output_info->mainimage_ptr  = pMainImagePtr;
    output_info->mainimage_size = mainImageLength;

   
    return EXIF_GEOTAG_SUCCESS;

} /* exif_insertGeotags */ 

/*===========================================================================

Function: EXIF_ISGPS_PRESENT

Description:  This function verifies the presence of GPS IFD in the input bit stream
                    
Return Value:
              TRUE  if GPS info is present
              FALSE if GPS info is not present
   
Comments:  None

============================================================================*/
boolean exif_IsGPS_present(uint8 *jpeg_bstream,
                                            uint32 jpeg_bstream_length)
{
    if(jpeg_exifread(&jpeg_exif_info, jpeg_bstream, jpeg_bstream_length) == TRUE)
    {
        if(jpeg_exif_info.gps_set)
            return TRUE;
    }

    return FALSE;

} /* exif_IsGPS_present */


#endif /* FEATURE_GEOTAGGING */

#endif

#endif /*(FEATURE_JPEG_DECODER || FEATURE_JPEG_ENCODER)*/


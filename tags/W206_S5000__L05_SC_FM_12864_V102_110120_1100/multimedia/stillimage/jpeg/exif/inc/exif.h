#ifndef EXIF_H                                                   
#define EXIF_H
/*============================================================================
  
                 EXIF Header Generation Interface Header File
  
   GENERAL DESCRIPTION
     This file contains the definitions needed for the EXIF header
     generation.

   EXTERNALIZED FUNCTIONS
  
   INITIALIZATION AND SEQUENCING REQUIREMENTS
     Application must call exif_init_info first before calling exif_emit_header.
     The reason of the two steps is to allow for the future additional fields
       be set before emit info.
   

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/exif/main/latest/inc/exif.h#2 $ $DateTime: 2008/07/02 02:20:06 $ $Author: c_caluru $

 when      who    what, where, why   
 --------  -----  ----------------------------------------------------------
 07/02/08  kdiv   Fixed rvct compiler warnings
 03/26/08  kdiv   Added Geotagging feature 
 12/17/07  kdiv   Control of setting Date and Time tags in Exif encoder is given to svcs too
 12/11/07  kdiv   Fixed the the wrong value of the tag EXIF_ORIENTATION_ROTATE_270
 01/25/06  arv    Added Print Image Matching Feature and hookup for 
                  Maker Note Exif Tags
 04/06/06  arv    Removed PIM Feature
 01/05/06  dl     Handle APP1 and APP7 size limitation gracefully.
 09/29/05  kln    Fixed lint erors
 09/16/05  kln    Fixed coverity errors
 09/06/05  kln    Exposed definition of sensor_info_type structure and added
                  comment about structure members' dimensions
 08/05/05  kln    Added exif_set_tag_value() function for backward compatibility 
 06/28/05  kln    Added a structure for the camera attributes that are supported
                  by EXIF
 04/18/05  kln    Removed value from exif_tag_type structure (all data is now stored
                  where offset is pointing to). Added fixedLength field indicating 
                  if tag has fixed or variable length.
 03/02/05  arv    Fixed the issue with unsupported exif tags
 11/22/04  arv    Added the FEATURE_EXIF_PIM3 and FEATURE_EXIF_TEST_PIM3.
                  Fixed the issue of images having an error marker in the
                  zeroth ifd.
 10/26/04  arv    Added PIM - Print Image Matching Feature
 10/20/04  pdo    Added subsample option to exif_emit_header()
 10/12/04  arv    Changed exif_string_type so as to set GPS data
 09/02/04  pdo    Added restart marker and thumbnail subsampling for 6550 JPEG encoder
 07/13/04  ckl	  Added new constants for get/set and APP0, as well as new
                  enum values for exif_emit_type to support JFIF
 06/24/04  ckl	  Merged the exif_info_type struct with that in OEMJPEG.c
 08/10/04  arv    Added EXIF Software tag to the enumeration exif_tag_id_type.
                  Inserted the EXIF Software tag to zero_ifd structure.
 05/21/04  sai    Added EXIF_MAKE_STRING and EXIF_MODEL_STRING to the 
                  enumeration exif_string_type. 
                  Modified exif_set_string to set exif tags make and model.
 03/10/04  pdo    Added exif_terminate()
 02/04/04  mvd    Changed args to exif_emit_header for quality and DQT 
                  tables.
                  Replaced exif_quality_type quality with int16 quality_
                  percent, from exif info type.  Removed exif_quality_type
                  quality enums are obsolete.
 01/29/04  mvd    Made exif_write_frame_header() external for calling by
                  camera.c.  Added new var to exif struct: buf_index_dqt_pos
 01/23/04  mvd    Added enum entry to EXIF_QUALITY_TYPE for quality values
                  other than low,med, or high.
 09/23/03  pdo	  Added exif_flash_type and changed exif_set_value_tag intrf.
 08/27/03  pdo	  Added PrintExif related tags and provided API to set them.
 02/03/03  dle    Added thumbnail support.
 01/21/03  dle    Added exif_position_type and used it in set_position.
                  Added support for additional GPS tags.
 12/18/02  dle    Added exif_set_position() and exif_set_description().
 11/14/02  dle    Added EXIF_HEADER_SIZE.
 11/12/02  dle    Changed function prototype.
 11/04/02  dle    Initial release

============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/

#include "target.h"
#include "customer.h"
#include "comdef.h"

/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/

#define INTEROP_INDEX                   0x0001
#define EXIFR98_VERSION                 0x0002
#define THUMBNAIL_LENGTH		        0x0102
#define	GPS_IFD_MAX_VALUE		        0x0020
#define EXIF_IFD_MIN_VALUE		        0x8000
#define ZERO_IFD_MIN_VALUE		        0x0104
#define ZERO_IFD_MAX_VALUE		        0x0214
#define JFIF_PLAIN_HEADER_LENGTH	    0x10
#define JPEG_THUMBNAIL_EXTENSION_CODE 	0x10
#define SOI                             0xFFD8
#define APP1                            0xFFE1
#define APP0                            0xFFE0
#define NUM_QUANT_TBLS                  2	/* Quantization tables are numbered 0..3 */
#define NUM_HUFF_TBLS                   2	/* Huffman tables are numbered 0..3 */
#define quantization_type               uint16
#define EXIF_MAX_HEADER_SIZE            65500L  /* a tad under 64K to prevent overflows */
#define EXIF_MAX_TAG_SIZE               100
#define THUMBNAIL_IFD_FIELD_COUNT       6
#define EXIF_TAG_SIZE                   12
#define EXIF_MAX_APP1_LENGTH            0xFFFF
#define EXIF_MAX_APP7_LENGTH            0xFFFF


typedef enum
{
  GPS_VERSION_ID                = 0x0000,
  GPS_LATITUDE_REF              = 0x0001,
  GPS_LATITUDE                  = 0x0002,
  GPS_LONGITUDE_REF             = 0x0003,
  GPS_LONGITUDE                 = 0x0004,
  GPS_ALTITUDE_REF              = 0x0005,
  GPS_ALTITUDE                  = 0x0006,
  GPS_TIMESTAMP                 = 0x0007,
  GPS_MAP_DATUM                 = 0x0012,
  GPS_PROCESS_METHOD            = 0x001B,
  GPS_AREA_INFORMATION          = 0x001c,
  GPS_DATESTAMP                 = 0x001d,
  COMPRESSION                   = 0x0103,
  IMAGE_DESCRIPTION             = 0x010e,
  MAKE                          = 0x010f,
  MODEL                         = 0x0110,
  ORIENTATION                   = 0x0112,
  X_RESOLUTION                  = 0x011a,
  Y_RESOLUTION                  = 0x011b,
  RESOLUTION_UNIT               = 0x0128,
  SOFTWARE                      = 0x0131,
  JPEGINTERCHANGE_FORMAT        = 0x0201,
  JPEGINTERCHANGE_FORMAT_L      = 0x0202,
  YCBCR_POSITIONING             = 0x0213,
  EXPOSURE_TIME                 = 0x829A,
  F_NUMBER                      = 0x829D,
  EXIF_IFD_POINTER              = 0x8769,
  EXPOSURE_PROGRAM              = 0x8822,
  GPS_IFD_POINTER               = 0x8825,
  ISO_SPEED_RATINGS             = 0x8827,
  OECF                          = 0x8828,
  EXIF_VERSION                  = 0x9000,
  EXIF_DATE_TIME_ORIGINAL       = 0x9003,
  EXIF_DATE_TIME_DIGITIZED      = 0x9004,
  EXIF_COMPONENTS_CONFIGURATION = 0x9101,
  SHUTTER_SPEED                 = 0x9201,
  APERTURE                      = 0x9202,
  BRIGHTNESS                    = 0x9203,
  MAX_APERTURE                  = 0x9205,
  SUBJECT_DISTANCE              = 0x9206,
  METERING_MODE                 = 0x9207,
  LIGHT_SOURCE                  = 0x9208,
  FLASH                         = 0x9209,
  FOCAL_LENGTH                  = 0x920A,
  EXIF_MAKER_NOTE               = 0x927C,
  EXIF_USER_COMMENT             = 0x9286,
  EXIF_FLASHPIX_VERSION         = 0xA000,
  EXIF_COLOR_SPACE              = 0xA001,
  EXIF_PIXEL_X_DIMENSION        = 0xA002,
  EXIF_PIXEL_Y_DIMENSION        = 0xA003,
  INTEROP_IFD_POINTER           = 0xA005,
  SUBJECT_LOCATION              = 0xA214,
  EXPOSURE_INDEX                = 0xA215,
  SENSING_METHOD                = 0xA217,
  FILE_SOURCE                   = 0xA300,
  SCENE_TYPE                    = 0xA301,
  CFA_PATTERN                   = 0xA302,
  CUSTOM_RENDERED               = 0xA401,
  EXPOSURE_MODE                 = 0xA402,
  WHITE_BALANCE                 = 0xA403,
  DIGITAL_ZOOM_RATIO            = 0xA404,
  FOCAL_LENGTH_35MM             = 0xA405,
  SCENE_CAPTURE_TYPE            = 0xA406,
  GAIN_CONTROL                  = 0xA407,
  CONTRAST                      = 0xA408,
  SATURATION                    = 0xA409,
  SHARPNESS                     = 0xA40A,
  SUBJECT_DISTANCE_RANGE        = 0xA40C,
#ifdef FEATURE_EXIF_PIM3
  PIM_TAG                       = 0xC4A5
#endif
} exif_tag_id_type;

typedef enum
{
  EXIF_GET_VALUE_SUCCESS  = 1,
  EXIF_GET_VALUE_FAILURE  = 2,
  EXIF_UNIDENTIFIED_TAG = 3
} exif_get_value_type;

typedef enum
{
  EXIF_EMIT_UPTO_THUMBNAIL,
  EXIF_EMIT_AFTER_THUMBNAIL,
  EXIF_EMIT_ALL,
  JFXX_EMIT_ALL,
  JFXX_EMIT_UPTO_THUMBNAIL,
  JFXX_EMIT_AFTER_THUMBNAIL
} exif_emit_type;

/* Camera Attributes enums */
typedef enum 
{
  GBRG_CFA,
  GRBG_CFA
} exif_cfaPattern_en;

typedef enum
{
  EXIF_DAYLIGHT_LIGHT_SOURCE = 1,
  EXIF_FLUORESCENT_LIGHT_SOURCE = 2,
  EXIF_INSANDESCENT_LIGHT_SOURCE = 3,
  EXIF_FLASH_LIGHT_LIGHT_SOURCE = 4,
  EXIF_FINE_WEATHER_LIGHT_SOURCE = 9,
  EXIF_CLOWDY_WEATHER_LIGHT_SOURCE = 10,
  EXIF_SHADE_LIGHT_SOURCE = 11,
  EXIF_D5700_7100_LIGHT_SOURCE = 12,
  EXIF_N46000_5400_LIGHT_SOURCE = 13,
  EXIF_W3900_4500_LIGHT_SOURCE = 14,
  EXIF_WW3200_3700_LIGHT_SOURCE = 15,
  EXIF_STANDARD_A_LIGHT_SOURCE = 17,
  EXIF_STANDARD_B_LIGHT_SOURCE = 18,
  EXIF_STANDARD_C_LIGHT_SOURCE = 19,
  EXIF_D55_LIGHT_SOURCE = 20,
  EXIF_D65_LIGHT_SOURCE = 21,
  EXIF_D75_LIGHT_SOURCE = 22,
  EXIF_D50_LIGHT_SOURCE = 23,
  EXIF_ISO_STUDIO_TUNGSTEN_LIGHT_SOURCE = 24,
  EXIF_OTHER_LIGHT_SOURCE = 255
} exif_lightSource_en;

typedef enum
{
  EXIF_LOW_GAIN_UP = 1,
  EXIF_HIGH_GAIN_UP = 2,
  EXIF_LOW_GAIN_DOWN = 3,
  EXIF_HIGH_GAIN_DOWN = 4
} exif_gainControl_en;

typedef enum
{
  EXIF_UNKNOWN_RANGE = 0,
  EXIF_MACRO_RANGE = 1,
  EXIF_CLOSE_VIEW_RANGE = 2,
  EXIF_DISTANT_VIEW_RANGE = 3
} exif_subjectDistanceRange_en;

typedef enum 
{
  EXIF_NOT_DEFINED_EP = 0,
  EXIF_MANUAL_EP = 1,
  EXIF_NORMAL_PROGRAM_EP = 2,
  EXIF_APERTURE_PRIORITY_EP = 3,
  EXIF_SHUTTER_PRIORITY_EP = 4,
  EXIF_DEPTH_OF_FIELD_EP = 5,
  EXIF_SHUTTER_SPEED_EP = 6,
  EXIF_CLOSEUP_EP = 7,
  EXIF_LANDSCAPE_EP = 8
} exif_exposureProgram_en;

typedef enum
{
  EXIF_UNKNOWN_MM = 0,
  EXIF_AVERAGE_MM  = 1,
  EXIF_CENTER_WEIGHTED_AVERAGE_MM  = 2,
  EXIF_SPOT_MM  = 3,
  EXIF_MULTISPOT_MM  = 4,
  EXIF_PATTERN_MM  = 5,
  EXIF_PARTIAL_MM  = 6,
  EXIF_OTHER_MM  = 255
} exif_meteringMode_en;

typedef enum
{
  EXIF_NORMAL_PROCESS = 0,
  EXIF_CUSTOM_PROCESS = 1
} exif_customIP_en;

typedef enum
{
  EXIF_AUTO_WB = 0,
  EXIF_MANUAL_WB = 1
} exif_whiteBalance_en;

typedef enum
{
  EXIF_STANDARD_SCENE = 0,
  EXIF_LANDSCAPE_SCENE = 1,
  EXIF_PORTRAIT_SCENE = 2,
  EXIF_NIGHT_SCENE = 3
} exif_sceneCaptureType_en;

typedef enum
{
  EXIF_NORMAL_CONTRAST = 0,
  EXIF_SOFT_CONTRAST = 1,
  EXIF_HARD_CONTRAST = 2
} exif_contrast_en;

typedef enum 
{
  EXIF_AUTO_EXPOSURE = 0,
  EXIF_MANUAL_EXPOSURE = 1,
  EXIF_AUTO_BRACKET_EXPOSURE = 2
} exif_exposureMode_en;

typedef enum
{
  EXIF_NORMAL_SATURATION = 0,
  EXIF_LOW_SATURATION = 1,
  EXIF_HIGH_SATURATION = 2
} exif_saturation_en;

typedef enum
{
  EXIF_NORMAL_SHARPNESS = 0,
  EXIF_SOFT_SHARPNESS = 1,
  EXIF_HARD_SHARPNESS = 2
} exif_sharpness_en;

typedef enum
{
  EXIF_NOT_DEFINED_SM = 1,
  EXIF_ONE_CHIP_COLOR_SM = 2,
  EXIF_TWO_CHIP_COLOR_SM = 3 
} exif_sensingMethod_en;

typedef struct 
{
  uint32 value;
  boolean active;
} exif_uint32_type;

typedef struct 
{
  uint16 value;
  boolean active;
} exif_uint16_type;

typedef struct 
{
  byte value;
  boolean active;
} exif_uint8_type;

typedef struct
{
  exif_cfaPattern_en value;
  boolean active;
} exif_cfaPattern_type;

typedef struct
{
  exif_lightSource_en value;
  boolean active;
} exif_lightSource_type;

typedef struct
{
  exif_gainControl_en value;
  boolean active;
} exif_gainControl_type;


typedef struct
{
  exif_subjectDistanceRange_en value;
  boolean active;
} exif_subjectDistanceRange_type;

typedef struct
{
  exif_exposureProgram_en value;
  boolean active;
} exif_exposureProgram_type;

typedef struct
{
  exif_meteringMode_en value;
  boolean active;
} exif_meteringMode_type;

typedef struct
{
  exif_customIP_en value;
  boolean active;
} exif_customIP_type;

typedef struct
{
  exif_exposureMode_en value;
  boolean active;
} exif_exposureMode_type;

typedef struct
{
  exif_whiteBalance_en value;
  boolean active;
} exif_whiteBalance_type;

typedef struct
{
  exif_sceneCaptureType_en value;
  boolean active;
} exif_sceneCaptureType_type;

typedef struct
{
  exif_contrast_en value;
  boolean active;
} exif_contrast_type;

typedef struct
{
  exif_saturation_en value;
  boolean active;
} exif_saturation_type;

typedef struct
{
  exif_sharpness_en value;
  boolean active;
} exif_sharpness_type;

typedef struct
{
  exif_sensingMethod_en value;
  boolean active;
} exif_sensingMethod_type;

/* Sensor Attributes struct */
typedef struct 
{
  /* Amount of time sensor is exposed to light
     Compututed by the line number and pClock 
     Specified in seconds(s) x 1024*/ 
  exif_uint32_type exposureTime;

  /* Dimensionless */
  exif_uint32_type fNumber;

  /* Optico-electric conversion factor
     Relation between optical input and image values */
  exif_uint16_type oecf;

  /* Applies only if mechanical shutter */
  exif_uint32_type shutterSpeed;

  /* aperture = 2*log2(fNumber) */
  exif_uint32_type aperture;
  exif_uint32_type max_aperture;

  /* Specified in mm x 1024 */
  exif_uint32_type focalLength;

  /* TBD */
  exif_uint32_type exposureIndex;

  /* Applies only if one color sensor
     Correspond to the color grid from left 
     to right and from top to bottom */
  exif_cfaPattern_type cfaPattern;

  /* TBD */
  exif_uint16_type focalLength35mm;

  /* Specify how (how many) colors are actually acquired per pixel */
  exif_sensingMethod_type sensingMethod;
} sensor_info_type;

/* Camera Attribute struct */
typedef struct 
{
  sensor_info_type sensor_info;
  struct 
  {
    /* To be computed from the autofocus result
       Last step can be mapped to target distance */
    exif_uint32_type subjectDistance;

    /* Correspond to the illuminant as estimated by the VFE */
    exif_lightSource_type lightSource;

    /* To be computed in case of multispot mettering mode
       as a center of the spot with highest high band energy */
    exif_uint16_type subjectLocation;

    /* TBD */
    exif_gainControl_type gainControl;

    /* To be computed from the autofocus result 
       Last step can be mapped to target distance range */
    exif_subjectDistanceRange_type subjectDistanceRange;
  } vfe_info;

  struct 
  {
    /* The factor that exposure was derived from,
       i,e., aperture, shutter, focal range */
    exif_exposureProgram_type exposureProgram;

    /* The region(s) of the picture used for 
       autofocus */
    exif_meteringMode_type meteringMode;

    /* Flag indicating if custom image processing was
       applied */
    exif_customIP_type customIP;

    /* The mode in which the exposure was computed
       manual, auto, bracket */
    exif_exposureMode_type exposureMode;

    /* The mode in which white balance is computed
       auto, manual */
    exif_whiteBalance_type whiteBalance;

    /* The upsampling ratio that is applied to the
       image in order to digitally zoom it */
    exif_uint32_type digitalZoomRatio;

    /* Set by user based on the perceived conditions */
    exif_sceneCaptureType_type sceneCaptureType;

    /* Pass as number between 1-10 , needs to be set as
       low , medium , high */
    exif_contrast_type contrast;

    /* To be set by user */
    exif_saturation_type saturation;

    /* To be set by user */
    exif_sharpness_type sharpness;

    /* To be set by user */
    exif_uint32_type brightness;
  } UI_info;

  struct 
  {
    /* The state of the flash when image acquired
       Bit 0 - flash fired 
                0 - not fired
                1 - fired
       Bit 2-1 - status of return light
               00 - no strobe return detection function
               01 - reserved
               10 - strobe return light not detected
               11 - strobe return light detected
       Bit 4-3 - flash mode
               00 - unknown
               01 - compulsory flash firing
               10 - compulsory flash suppresion
               11 - auto mode
       Bit 5  - presense of flash function
                0 - flash function present
                1 - no flash function
       Bit 6  - Red-eye mode
                0 - no red-eye reduction mode or unknown
                1 - red-eye reduction supported
                */
    exif_uint8_type flash;
  } camera_info;
} exif_attributes_type;

/*
** Define the Flash word for EXIF flash
*/
typedef struct
{
  unsigned int fired         :  1; /* 0: flash did not fire             */
                                   /* 1: flash fired                    */
  unsigned int return_lite   :  2; /* 0: No strobe detection fucntion   */
                                   /* 1: Reserved                       */
  /* 2: No return strobe detected      */
  /* 3: return strobe detected         */
  unsigned int mode          :  2; /* 0: unknown                        */
                                   /* 1: copulsory flash firing         */
                                   /* 2: compulsary flash suppression   */
  /* 3: Auto mode                      */
  unsigned int function      :  1; /* 0: Flash function supported       */
  /* 1: No flash function supported    */
  unsigned int red_eye       :  1; /* 0: Red-eye reduction not supported*/
  /* 1: Redeye reduction supported     */          
  unsigned int Reserved      :  9;
} exif_flash_type;

typedef enum
{
  EXIF_IFD_EXPOSURE_TIME,     /* RATIONAL type */
  EXIF_IFD_FLASH,         /* SHORT type    */
  EXIF_IFD_CUSTOM_RENDERED,     /* SHORT type    */
  EXIF_IFD_EXPOSURE_MODE,     /* SHORT type    */ 
  EXIF_IFD_WHITE_BALANCE,     /* SHORT type    */
  EXIF_IFD_DIGITAL_ZOOM_RATIO,    /* RATIONAL type */
  EXIF_IFD_SCENE_CAPTURE_TYPE,    /* SHORT type    */
  EXIF_IFD_SUBJECT_DISTANCE_RANGE /* SHORT type    */
} exif_value_type;

typedef enum
{
  EXIF_DESCRIPTION_STRING,
  EXIF_USER_COMMENT_STRING,
  EXIF_MAKER_NOTE_STRING,
  EXIF_GPS_AREA_INFORMATION_STRING,
  EXIF_MAKE_STRING,
  EXIF_MODEL_STRING,
  EXIF_SOFTWARE_STRING,
  EXIF_GPS_MAP_DATUM_STRING,
  EXIF_GPS_DATESTAMP_STRING,
  EXIF_DATE_TIME_ORIGINAL_STRING
} exif_string_type;

typedef enum
{
  EXIF_H2V2 = 0,
  EXIF_H2V1,
  EXIF_H1V2,
  EXIF_H1V1
} exif_subsample_type;

typedef enum 
{
  EXIF_UNKNOWN, /* error/unspecified */
  EXIF_GRAYSCALE, /* monochrome */
  EXIF_RGB,   /* red/green/blue */
  EXIF_YCbCr,   /* Y/Cb/Cr (also known as YUV) */
  EXIF_CMYK,    /* C/M/Y/K */
  EXIF_YCCK   /* Y/Cb/Cr/K */
} exif_color_space_type;

typedef enum
{
  EXIF_ORIENTATION_UP = 1,
  EXIF_ORIENTATION_FLIP_LEFT_RIGHT = 2,
  EXIF_ORIENTATION_FLIP_UP_DOWN = 4,
  EXIF_ORIENTATION_ROTATE_90 = 6,
  EXIF_ORIENTATION_ROTATE180 = 3,
  EXIF_ORIENTATION_ROTATE_270 = 8,
} exif_orientation_type;

typedef struct 
{
  /* These two fields directly represent the contents of a JPEG DHT marker */
  uint8 bits[17];       /* bits[k] = # of symbols with codes of */
  /* length k bits; bits[0] is unused */
  uint8 huffval[256];   /* The symbols, in order of incr code length */
  boolean sent;             /* Suppress duplicates */
} JHUFF_TBL;

/* Basic info about one component (color channel). */
typedef struct 
{
  /* These values are fixed over the whole image. */
  /* For compression, they must be supplied by parameter setup; */
  /* for decompression, they are read from the SOF marker. */
  uint8 component_id;   /* identifier for this component (0..255) */
  uint8 h_samp_factor;    /* horizontal sampling factor (1..4) */
  uint8 v_samp_factor;    /* vertical sampling factor (1..4) */
  uint8 quant_tbl_no;   /* quantization table selector (0..3) */
  /* These values may vary between scans. */
  /* For compression, they must be supplied by parameter setup; */
  /* for decompression, they are read from the SOS marker. */
  /* The decompressor output side may not use these variables. */
  uint8 dc_tbl_no;    /* DC entropy table selector (0..3) */
  uint8 ac_tbl_no;    /* AC entropy table selector (0..3) */
} jpeg_component_info;

typedef enum
{
  EXIF_BYTE = 1,
  EXIF_ASCII = 2,
  EXIF_SHORT = 3,
  EXIF_LONG = 4,
  EXIF_RATIONAL = 5,
  EXIF_UNDEFINED = 7,
  EXIF_SLONG = 9,
  EXIF_SRATIONAL = 10
} exif_data_type;

typedef struct 
{
  uint16 id;
  uint16 type;
  uint32 count;
  void* offset;
  boolean fixedLength;
  boolean active;
} exif_tag_type;

typedef struct
{
  int32 numerator;
  int32 denominator;
} exif_rational_type;

typedef struct 
{
  boolean is_exif_stream; /* Is the stream conform to EXIF spec? */
  boolean is_big_endian; /* Is the EXIF stream in big endianess */
  /* The EXIF stream buffer */
  byte *stream;
  /* The EXIF stream length */
  uint32 stream_length;
  uint8 * pointer;
  uint8 * tiff_header_pointer; /* TIFF header pointer */
  /* Indication whether GPS location is set */
  /* input image dimension */
  uint16 image_width;
  uint16 image_height;
  /* bits of precision in image data */
  uint8 data_precision; 
  /* # of color components in JPEG image */
  uint8 num_components; 
  /* JPEG quality metric in percent */
  int16 quality_percent;
  /* colorspace of JPEG image */
  exif_color_space_type color_space;
  /* The EXIF header buffer */
  byte *buf_ptr;
  /* The EXIF header buffer length */
  uint32 buf_len;
  /* The EXIF header buffer index to write next byte */
  uint32 buf_index;
  uint32 buf_index_save;
  uint32 buf_index_dqt_pos;

  /* index to the start of thumbnail */
  uint32 thumbnail_index;

  /* Index at the TIFF header, for offset calculation */
  uint32 tiff_header_index;
  /* subsample of the compressed image */
  exif_subsample_type subsampling;
  uint16 restart_cnt;

  /* The following index are needed for updating fields */
  uint32 exif_ifd_index;      /* EXIF_IFD pointer index */
  uint32 length_index;        /* APP1 length index */
  uint32 gps_ifd_index;       /* GPS IFD pointer index */
  uint32 interop_ifd_index;   /* Interoperability IDF pointer index */
  uint32 string_index;        /* Where string will be placed */

  /* Indication whether GPS location is set */
  boolean gps_set;

  /* This count must be exactly what is defined in zero_ifd. */

#ifdef FEATURE_EXIF_PIM3  
#define ZERO_IFD_FIELD_COUNT 12
#else
#define ZERO_IFD_FIELD_COUNT 11
#endif

  struct
  {
    exif_tag_type image_description;
    exif_tag_type make;
    exif_tag_type model;
    exif_tag_type orientation;
    exif_tag_type x_resolution;
    exif_tag_type y_resolution;
    exif_tag_type resolution_unit;
    exif_tag_type ycbcr_positioning;
#ifdef FEATURE_EXIF_PIM3
    exif_tag_type pim;
#endif
    exif_tag_type software;
    exif_tag_type exif_ifd_pointer;
    exif_tag_type gps_ifd_pointer;
  } zero_ifd;
  uint8 * exif_ifd_pointer;
  uint8 * gps_ifd_pointer;
  uint8 * thumbnail_ifd_pointer;

  struct
  {
    exif_tag_type exposure_time;
    exif_tag_type f_number;
    exif_tag_type iso_speed_ratings;
    exif_tag_type oecf;
    exif_tag_type shutter_speed;
    exif_tag_type exposure_program;
    exif_tag_type aperture;
    exif_tag_type brightness;
    exif_tag_type max_aperture;
    exif_tag_type light_source;
    exif_tag_type focal_length;
    exif_tag_type exposure_index;
    exif_tag_type sensing_method;
    exif_tag_type subject_distance;
    exif_tag_type subject_location;
    exif_tag_type cfa_pattern;
    exif_tag_type focal_length_35mm;
    exif_tag_type contrast;
    exif_tag_type saturation;
    exif_tag_type metering_mode;
    exif_tag_type sharpness;
    exif_tag_type gain_control;
    exif_tag_type flash;
    exif_tag_type custom_rendered;
    exif_tag_type exposure_mode;
    exif_tag_type white_balance;
    exif_tag_type digital_zoom_ratio;
    exif_tag_type scene_capture_type;
    exif_tag_type subject_distance_range;  
    exif_tag_type file_source;     
    exif_tag_type scene_type;   
    exif_tag_type version;
    exif_tag_type date_time_original;
    exif_tag_type date_time_digitized;
    exif_tag_type components_configuration;
    exif_tag_type user_comment;
    exif_tag_type maker_note;
    exif_tag_type flashpix_version;
    exif_tag_type color_space;
    exif_tag_type pixel_x_dimension;
    exif_tag_type pixel_y_dimension;
    exif_tag_type interop_ifd_pointer;
  } exif_ifd;

  /* This count must be exactly what is defined in interop_ifd. */
  #define INTEROP_IFD_FIELD_COUNT 2

  struct
  {
    exif_tag_type interop_index;
    exif_tag_type exifR98_version;
  } interop_ifd;

  /* This count must be exactly what is defined in gps_ifd. */
  #define GPS_IFD_FIELD_COUNT 12
  struct
  {
    exif_tag_type version_id;
    exif_tag_type latitude_ref;
    exif_tag_type latitude;
    exif_tag_type longitude_ref;
    exif_tag_type longitude;
    exif_tag_type altitude_ref;
    exif_tag_type altitude;
    exif_tag_type timestamp;
    exif_tag_type map_datum;
    exif_tag_type area_information;
    exif_tag_type datestamp;
    exif_tag_type  process_method;
  } gps_ifd;

  jpeg_component_info * comp_info;

  /* ptrs to coefficient quantization tables, or NULL if not defined */
  quantization_type * quant_tbl_ptrs[NUM_QUANT_TBLS];

  /* ptrs to Huffman coding tables, or NULL if not defined */
  JHUFF_TBL * dc_huff_tbl_ptrs[NUM_HUFF_TBLS];
  JHUFF_TBL * ac_huff_tbl_ptrs[NUM_HUFF_TBLS];

  /* Parameters controlling emission of special markers. */
  boolean write_JFIF_header;  /* should a JFIF marker be written? */
  uint8 JFIF_major_version;     /* What to write for the JFIF version number */
  uint8 JFIF_minor_version;

  /* These three values are not used by the JPEG code, merely copied */
  /* into the JFIF APP0 marker.  density_unit can be 0 for unknown, */
  /* 1 for dots/inch, or 2 for dots/cm.  Note that the pixel aspect */
  /* ratio is defined by X_density/Y_density even when density_unit=0. */
  uint8 density_unit;   /* JFIF code for pixel size units */
  uint16 X_density;         /* Horizontal pixel density */
  uint16 Y_density;       /* Vertical pixel density */

  /* The coefficient controller receives data in units of MCU rows as defined
   * for fully interleaved scans (whether the JPEG file is interleaved or not).
   * There are v_samp_factor * DCTSIZE sample rows of each component in an
   * "iMCU" (interleaved MCU) row.
   */
  uint8 Ss, Se, Ah, Al;           /* progressive JPEG parameters for scan */
  uint32 code1, code2, code3, code4;/* Used to make sure this info was init. */

  /* Thumbnail related items */
  /* This count must be exactly what is defined in zero_ifd. */
  struct
  {
    exif_tag_type compression;
    exif_tag_type x_resolution;
    exif_tag_type y_resolution;
    exif_tag_type resolution_unit;
    exif_tag_type jpegi_format;   /* JPEGInterchangeFormat */
    exif_tag_type jpegi_format_l; /* JPEGInterchangeFormatLength */
  } thumbnail_ifd;

  boolean thumbnail_present;    /* thumbnail present */
  uint8 *thumbnail_stream;
  uint32 thumbnail_stream_length;
  exif_tag_type dummy_tag;
  boolean isDummyTag;
  boolean emit_thumbnail;       /* emitting thumbnail */
  uint32 thumbnail_ifd_index;   /* Thumbnail IFD offset */
  uint32 jpegi_format_index;    /* jpegi_format offset */
  uint32 jpegi_format_l_index;  /* jpegi_format_l offset */
  uint32 thumbnail_stream_index;/* thumbnail stream offset */
  /* thumbnail dimension */
  uint16 thumbnail_width;
  uint16 thumbnail_height;
} exif_info_type;

typedef struct
{
  exif_emit_type type;
  exif_subsample_type subsample;
  /* Picture dimension */
  uint16 main_width;
  uint16 main_height;
  /* Thumbnail dimension */
  uint16 tn_width;
  uint16 tn_height;
  /* Color space of source */
  exif_color_space_type color_space;
  /* Quality of JPEG encoding in percent from 1 to 100 */
  int16 quality_percent;
  byte * buf_ptr;
  /* Length of the buffer */
  uint32 length;
  /* Quantization table pointers */
  quantization_type * luma_dqt_table;
  quantization_type * chroma_dqt_table;
} exif_header_type;

typedef struct
{
  uint32 timestamp;
  int32  latitude;
  int32  longitude;
  int16  altitude;
  /*  int16  position_method; */
} exif_position_type;

typedef struct 
{
  uint16 type;
  uint32 count;
  void* offset;
  uint32 value;
} exif_valueinfo_type;



/*============================================================================
*                         MACRO DEFINITIONS
============================================================================*/

/*============================================================================
*                         EXTERNAL VARIABLES
============================================================================*/
extern quantization_type  exif_luma_dqt_low[];
extern quantization_type  exif_chroma_dqt_low[];
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

void exif_init_info
(
exif_info_type * info
);

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
);

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
);


/*===========================================================================

FUNCTION      EXIF_EMIT_HEADER

DESCRIPTION
              The information is emitted to the buffer.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if info was initialized and emit info is successful.
  The total header byte count cannot exceed EXIF_MAX_HEADER_SIZE bytes.

SIDE EFFECTS
  None

===========================================================================*/

boolean exif_emit_header
(
exif_info_type * info,
const exif_header_type *header
);

/*===========================================================================

FUNCTION      EXIF_UPDATE_APP0_THUMBNAIL

DESCRIPTION
              Need to update header after thumbnail encoding is
              done for APP0 (JFIF)

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE if failed

SIDE EFFECTS
  None

===========================================================================*/

boolean exif_update_app0_thumbnail
(
exif_info_type *info,
uint32 size
);

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

boolean exif_update_thumbnail
(
exif_info_type *info,
uint32 size
);

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

boolean jfif_update_thumbnail
(
exif_info_type *info,
uint32 size
);

/*===========================================================================
FUNCTION  EXIF_WRITE_FRAME_HEADER

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
boolean exif_write_frame_header 
(
exif_info_type *info
);

/*===========================================================================

FUNCTION      EXIF_TERMINATE

DESCRIPTION
              Clean up any EXIF related memory 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void exif_terminate
(
exif_info_type *info
);


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

uint16 jpeg_exifread_get_2bytes
(
exif_info_type *info
);         

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

uint16 jpeg_exifread_get_2bytes_stream
(
const exif_info_type *info, void* ptr
);

/*===========================================================================

FUNCTION      JPEG_EXIFREAD_GET_4BYTES_STREAM

DESCRIPTION
              Read four bytes from the input stream

DEPENDENCIES
  None

RETURN VALUE Retrun value in 32 bit data
  

SIDE EFFECTS
  None

===========================================================================*/

uint32 jpeg_exifread_get_4bytes_stream
(
const exif_info_type * info, void* ptr
);

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

boolean jpeg_exifread_compare_2bytes
(
exif_info_type *info, uint16 value
);

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

boolean jpeg_exifread_compare_string
(
exif_info_type *info, 
const void *string
);

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

uint32 jpeg_exifread_get_4bytes
(
exif_info_type * info
);

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

uint8 * jpeg_exifread_get_pointer
(
exif_info_type * info
);

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

void jpeg_exifread_get_tag
(
exif_info_type *info,
exif_tag_type *tag
);


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

boolean jpeg_exifread_get_zero_ifd
(
exif_info_type *info
);

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

boolean jpeg_exifread_get_exif_ifd
(
exif_info_type *info
);

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

boolean jpeg_exifread_get_gps_ifd
(
exif_info_type *info
);

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

boolean jpeg_exifread_get_thumbnail_ifd
(
exif_info_type *info
);


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
);

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

boolean jpeg_jfifread
(
exif_info_type *info,
uint8 *stream,
uint32 stream_length
);

/*===========================================================================

FUNCTION      EXIF_GET_VALUE  

DESCRIPTION   Given a tag, this function determines which directory the tag is 
              located in (zero_ifd, exif_ifd, or gps_ifd), passes
              the info to the appropriate function, and eventually returns a 
              struct exif_valueinfo_type, with all the pertinent info
              
             

DEPENDENCIES
  None

RETURN VALUE  exif_get_value_type
  

SIDE EFFECTS
  None

===========================================================================*/

exif_get_value_type exif_get_value 
(
exif_valueinfo_type *values,
const exif_info_type *info,
uint16 tag_id
);


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
exif_get_value_type exif_getValueInfo
(
const exif_tag_type *tag,
exif_valueinfo_type *values
);

/*===========================================================================

FUNCTION     EXIF_GET_ZERO_VALUE  

DESCRIPTION   This is called by exif_get_value and is passed in a pointer to 
              the info and value structures, along with a tag id. Based on the
              tag id, this function determines which tag this id corresponds to
              within the zero ifd, and returns the necessary info to the value
              structure
              
             

DEPENDENCIES
  None

RETURN VALUE  exif_get_value_type 
  

SIDE EFFECTS
  None

===========================================================================*/

exif_get_value_type exif_get_zero_value 
(
const exif_info_type *info,
exif_valueinfo_type *values,
uint16 tag_id
);

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

exif_get_value_type exif_get_gps_value 
(
const exif_info_type *info,
exif_valueinfo_type *values,
uint16 tag_id
);


/*===========================================================================

FUNCTION      EXIF_GET_EXIF_VALUE  

DESCRIPTION   This is called by exif_get_value and is passed in a pointer to 
              the info and value structures, along with a tag id. Based on the
              tag id, this function determines which tag this id corresponds to
              within the exif ifd, and returns the necessary info to the value
              structure
              
             

DEPENDENCIES
  None

RETURN VALUE  void 
  

SIDE EFFECTS
  None

===========================================================================*/

exif_get_value_type exif_get_exif_value 
(
const exif_info_type *info,
exif_valueinfo_type *values,
uint16 tag_id
);


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

boolean exif_set_value 
(
exif_info_type *info,
const exif_valueinfo_type *values,
uint16 tag_id
);

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

boolean exif_set_gps_value 
(
exif_info_type *info,
const exif_valueinfo_type *values,
uint16 tag_id
);

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

boolean exif_set_exif_value 
(
exif_info_type *info,
const exif_valueinfo_type *values,
uint16 tag_id
);

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

boolean exif_set_zero_value 
(
exif_info_type *info,
const exif_valueinfo_type *values,
uint16 tag_id
);


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

boolean exif_change_value 
(
exif_tag_type *tag,
const exif_valueinfo_type *values
);

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
void exif_set_tag_value
(
exif_info_type * info,
exif_value_type  id,
void           * valuePtr
);
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
void exif_set_attribute_info
(
exif_info_type *info, 
const exif_attributes_type *exif_attr
);


/*===========================================================================

FUNCTION      EXIF_GET_VAR_EXIF_IFD_COUNT  

DESCRIPTION   Count active EXIF tags
             

DEPENDENCIES
  None

RETURN VALUE  Number of active EXIF tags
  

SIDE EFFECTS
  None

===========================================================================*/
uint16 exif_get_var_exif_ifd_count
(
const exif_info_type *info
);

/*===========================================================================

FUNCTION      EXIF_SET_RESTART  

DESCRIPTION   Set restart count
             

DEPENDENCIES
  None

RETURN VALUE  none
  

SIDE EFFECTS
  None

===========================================================================*/
void exif_set_restart
(
 exif_info_type *info, uint16 dri
);


/*===========================================================================

FUNCTION      EXIF_SET_SUBSAMPLE  

DESCRIPTION   Set color subsample
             

DEPENDENCIES
  None

RETURN VALUE  none
  

SIDE EFFECTS
  None

===========================================================================*/
void exif_set_subsample
(
 exif_info_type *info, exif_subsample_type sample
);




/*===========================================================================

FUNCTION      EXIF_PIM3_TEMINATE

DESCRIPTION   Frees EXIF PIM Memory

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_EXIF_PIM3
void exif_PIM3_terminate
(
exif_info_type *info
);

/*===========================================================================

FUNCTION     EXIF_SET_DATE_TIME_FROM_SVCS

DESCRIPTION   Sets the flag indicating that DATE TIME will be set by services
             

DEPENDENCIES
  None

RETURN VALUE  None
  

SIDE EFFECTS
  None

===========================================================================*/
void exif_set_date_time_from_svcs(boolean is_set_date_time_by_svcs);

/*===========================================================================
 
FUNCTION     EXIF_GET_DATE_TIME_STRING  
 
DESCRIPTION   Return exif_date_time_string address
              

DEPENDENCIES
  None

RETURN VALUE  Address of exif_date_time_string
  

SIDE EFFECTS
  None

===========================================================================*/
uint8* exif_get_date_time_string(void);

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
void exif_set_date_time(uint8 *ptr);

boolean exif_PIM3_enable(exif_info_type *info);
#endif

#ifdef FEATURE_GEOTAGGING
#define JPEG_EXIF_MAX_HEADER_SIZE (64*1024)
#define EXIF_INDEX_OFFSET          8


typedef struct
{
  uint8 *header_ptr;
  uint32 header_size;
  uint8 *mainimage_ptr;
  uint32 mainimage_size;
} exif_geotagging_output_type;

typedef enum 
{
  EXIF_GEOTAG_SUCCESS,
  EXIF_GEOTAG_64K_EXCEEDED,
  EXIF_GEOTAG_MALLOC_FAIL,
  EXIF_GEOTAG_INVALID_STREAM
} exif_geotagging_ret_type;

/*===========================================================================

Function: EXIF_GEOTAGGING_INIT

Description:  This function allocates header buffer for geotagging
Return Value:
              exif_geotagging_ret_type
   
Comments:  None

============================================================================*/
exif_geotagging_ret_type exif_geotagging_init (void);

/*===========================================================================

Function: EXIF_GEOTAGGING_TERMINATE

Description:  This function deallocates all buffers allocated in geotagging
Return Value:
              NONE
   
Comments:  NONE

============================================================================*/
void exif_geotagging_terminate (void);

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
                                            exif_geotagging_output_type *output_info );


/*===========================================================================

Function: EXIF_ISGPS_PRESENT

Description:  This function verifies the presence of GPS IFD in the input bit stream
                    
Return Value:
              TRUE  if GPS info is present
              FALSE if GPS info is not present
   
Comments:  None

============================================================================*/
boolean exif_IsGPS_present(uint8 *jpeg_bstream,
                                            uint32 jpeg_bstream_length);


#endif /* FEATURE_GEOTAGGING */



#endif /* EXIF_H */

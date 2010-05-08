/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("PNG Decoder")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  P O R T A B L E   N E T W O R K   G R A P H I C S   (PNG)   D E C O D E R

GENERAL DESCRIPTION
  This module contains functions to decode Portable Network Graphics (PNG)
  image files.

EXTERNALIZED FUNCTIONS
  png_decode
    This function is called to start the PNG decoder 
  png_decoder_abort
    This function is called to end the current PNG decoder session
  png_get_image_specs
    This function is called to extract the header information from the
    PNG image

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2000-2004 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/cmx_png.c#12 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/08   sch     Increase ARM11 clocks for decoding, cr137505. 
09/20/07   vma     Fixed design hole in which PNG contents with many very small
                   IDAT chunks would fail the decoder. Re-implemented how 
                   data are fetched to internal buffers. 
07/05/07   vma     Fix for incorrect filtering in case when color depth is less
                   than 8 in grayscale color type.
06/21/07   sch     Fix for compiler warning.
04/17/07   vma     Fixed banned API - sprintf and other high lint errors
01/24/07   vma     Fixed the bug in read_lengths() by removing the wrong 
                   assumption that the decoded lengths bitstream for literals 
                   and distances table can be treated separately. 
07/26/06   vma     Added boundary error check in png_huffman_decoder()
09/15/05    dl     Resolved coverity issue.
03/22/04    st     Modified code to return data via client data buffer, if
                   available.
04/10/03    sa     Renamed function name crc32() to png_crc32() to remove 
                   conflict with BREW PNG decoder.
06/12/02    sa     Changed png.h and pngi.h to cmx_png.h and cmx_pngi.h 
                   to reflect PNG name changes.
07/18/01    sa     Added check for abort flag in png_get_image_specs().
07/17/01    sa     LZ window buffer now provided by PNG client.  Added function
                   png_set_buf() for setting ptr to buffer. Added check for 
                   abort flag in png_decode().
03/23/01    sa     Added the 8 PNG signature bytes to the running byte count
03/19/01    sa     Added error checks to minimize decoding problems when
                   handling corrupted PNG files.
03/13/01    sa     Added code to request PNG file size.  File size is used to
                   check against a running byte count based on the length of
                   each of the PNG chunk types processed.  If at any time the
                   file size is smaller than the running byte count the decoder
                   will be aborted.  Also, fixed data type mismatches to get rid
                   of warnings.
03/08/01    sa     Went on a "RAM"page to save RAM.  Fixed bug in transparency
                   chunk processing.
02/14/01    sa     Added TrueColor (24-bit), TrueColor Alpha, and 16-bit
                   bit depth support.  Removed image scaling and support for
                   interlaced images.  Many optimizations of code including
                   dynamic huffman table generation and huffman decoding.
01/26/01    st     Moved #ifdef FEATURE_PNG_DECODER to remove compiler warnings
                   when FEATURE_PNG_DECODER is not defined.
01/22/01    sa     Fixed bug that would result in incorrect IDAT chunk 
                   processing and invalid CRC calculation for any IDAT chunk
                   length of exactly MAX_DATA_BYTES.
07/20/00    sa     Moved the CRC calculation for IDAT blocks smaller than 
                   MAX_DATA_BYTES from get_idat_byte() to the state machine.
                   Uncompressed image data now written to lz_window table. 
07/13/00    sa     No longer generate fixed huffman tables.  Tables are in 
                   program space.   More optimizations of code.
06/19/00    sa     Fixed CRC calculation bug for IDAT chunk lengths between 
                   MAX_DATA_BYTES and (2*MAX_DATA_BYTES).
06/13/00    sa     Changes to optimize code for speed. 
05/31/00    sa     Added comments and cleaned up code.
05/30/00    sa     Fixed running CRC calculation for IDAT block. 
05/19/00    sa     Modified algorithm to reduce RAM usage.  Added tEXt, 
                   tRNS, and bKGD chunk type processing.
05/10/00    sa     Added error checking for PNG image specifications not 
                   supported in implementation. 
05/09/00    sa     Created state machine.  Added read data, write data, and 
                   continue callbacks.  Removed cmx_handle_type.
04/28/00    st     Modified code to use new cmx_handle_type definition.
04/14/00    sa     Initial version

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Common definitions                      */

#ifdef FEATURE_PNG_DECODER
#include "msg.h"                /* Message logging/reporting services      */
#include <string.h>             /* Memory Manipulation routines            */
#include <stdio.h>              /* Standard I/O routines                   */
 

#include "cmx_png.h"            /* PNG typedefs and prototypes             */
#include "cmx_pngi.h"           /* PNG internal typedefs and prototypes    */

#ifdef PNG_USES_LIBSTD
#include "AEEstd.h"
#endif

#ifdef FEATURE_DCVS_APPS
#include "clkrgm_devman.h" /* DCVS APPS prototype */
#include "dcvsapps.h"
#endif  /* FEATURE_DCVS_APPS */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* The following variables are associated with the chunk processing        */
LOCAL uint32 chunk_length;                      /* length of the chunk     */
LOCAL uint8  chunk_type[4];                     /* chunk type              */
LOCAL uint8  chunk_data[2*MAX_DATA_BYTES];      /* chunk data              */

LOCAL cmx_png_idat_buffer idat_buffer[2]; /* data buffer to hold IDAT data */
LOCAL uint32 idat_buffer_select;  /* indicates which half of buffer in use */   

/*
** Huffman Data Declarations.
*/
LOCAL boolean literal_mode;             /* indicates if literal stream     */
LOCAL uint32  literal_count ;           /* number remaining literal bytes  */
LOCAL uint8   dyn_literals[288];        /* dynamic literals                */
LOCAL const literal_huffman_decoder_data_type *ht_literal_table;
LOCAL const literal_huffman_decoder_data_type *ht_distance_table;

/*
** Dynamic Huffman Table Declarations.
*/
LOCAL literal_huffman_decoder_data_type dynamic_literal_table ;
LOCAL dist_huffman_decoder_data_type dynamic_distance_table ;
LOCAL ht_huffman_decoder_data_type ht;

/*
** Fixed Huffman Literal Table Declaration.
*/
LOCAL literal_huffman_decoder_data_type const fixed_literal_table =
{
  { 
    -1, -1, -1, -1, -1, -1, 23, 199, 511, -1, -1, -1, -1, -1, -1
  },
  
  {
    BADCODE, BADCODE, BADCODE, BADCODE, BADCODE, BADCODE, 0, 48, 400,
    BADCODE, BADCODE, BADCODE, BADCODE, BADCODE, BADCODE
  },

  { 
    0, 0, 0, 0, 0, 0, 0, 24, 176, 0, 0, 0, 0, 0, 0
  },

  /* these are the huffman values that define the fixed
  ** literal huffman table
  */
  {
    256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269,
    270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 0, 1, 2, 3, 4, 5, 6,
    7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
    44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
    62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
    98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
    113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
    127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
    141, 142, 143, 280, 281, 282, 283, 284, 285, 286, 287, 144, 145, 146,
    147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
    161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
    175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
    189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202,
    203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216,
    217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
    231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244,
    245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
  }
};

/*
** Fixed Huffman Distance Table Declaration.
*/
LOCAL dist_huffman_decoder_data_type const fixed_distance_table =
{
  { 
    -1, -1, -1, -1, 31, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
  },

  {
    BADCODE, BADCODE, BADCODE, BADCODE, 0, BADCODE, BADCODE, BADCODE,
    BADCODE, BADCODE, BADCODE, BADCODE, BADCODE, BADCODE, BADCODE
  },

  { 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* these are the huffman values that define the fixed
  ** distance huffman table
  */
  {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
  }
};

/* variables associated with background chunk processing                   */ 
LOCAL boolean background_read;      /* ensures only one bKGD chunk in file */
LOCAL uint32 bkgd_color[3];         /* background color for all image types*/

/* variables associated with transparency chunk processing for grayscale,  */ 
/* indexed color and RGB images                                            */ 
LOCAL uint8 transparency_value[256];
LOCAL uint32 transparency_length;

/* IDAT chunk processing variables                                         */
LOCAL uint32 bit_offset;            /* bit offset into current IDAT byte   */
LOCAL uint32 byte_offset;           /* byte offset into current IDAT chunk */
LOCAL uint32 bit_buffer;            /* contains current IDAT byte          */

/* Image specifications from the header                                    */
LOCAL uint32 image_height;                      /* height of image         */
LOCAL uint32 image_width;                       /* width of image          */
LOCAL uint32 image_depth;                       /* bit depth of image      */
LOCAL uint32 image_color_type;                  /* color type of image     */
LOCAL uint32 image_compression_method;          /* compression method used */
LOCAL uint32 image_filter_method;               /* filter method used      */
LOCAL uint32 image_interlace_method;            /* interlaced method used  */


/* temp buffer for palette and decoded image buffer 
** WARNING: This buffer must remain at least 768 bytes in size for the 
** palette
*/

LOCAL uint8  temp_buf[IMAGE_WIDTH_MAX*2*IMAGE_DEPTH_MAX/CHAR_BIT];

/* working buffer when decoding image                                      */
LOCAL uint8 row_buffers [2][IMAGE_WIDTH_MAX*4*IMAGE_DEPTH_MAX/CHAR_BIT];
LOCAL uint32 current_row_buffer;         /* current working buffer used    */
LOCAL uint32 row_buffer_width;           /* width of working row buffer    */ 

LOCAL uint32 palette_size ;                 /* size of color map for image */      

LOCAL uint32 crc_value;       /* calculated CRC for each chunk             */ 

LOCAL uint8  *lz_window;                    /* LZ77 window ptr             */ 

LOCAL png_cmx_get_data_type cmx_get_data;       /* PNG get data            */
LOCAL png_cmx_continue_type cmx_continue;       /* PNG continue            */
LOCAL png_state_type png_state, png_state_q1;   /* states for state machine*/ 
LOCAL uint32 row_count;                         /* row being decoded       */
LOCAL uint32 data_position;     /* current position within input file      */   

LOCAL boolean data_read ;           /* ensures only one IDAT stream in file*/
LOCAL boolean palette_read ;        /* ensures only one palette in file    */
LOCAL boolean transparency_read ;   /* ensures only one tRNS chunk in file */
LOCAL boolean new_dataset_flag;     /* indicates new data set encountered  */
LOCAL boolean png_decoder_abort_flag;   /* PNG decoder abort status flag   */ 
LOCAL boolean png_image_specs_flag;     /* PNG file specification flag     */
LOCAL boolean png_decode_in_progress;   /* PNG decode in progress flag     */
LOCAL boolean get_idat_data;            /* get more IDAT data flag         */ 
LOCAL boolean final_data_set;           /* indicates last IDAT data set    */
LOCAL boolean lz_init_flag;             /* LZ77 variables initialized      */
LOCAL boolean png_crc_failed;    /* flag indicating whether crc has failed */
LOCAL uint32  idat_offset;              
               /* offset of next-to-be-read byte in the current idat chunk */

LOCAL boolean png_set_buffer_flag = FALSE; /* PNG internal buffers set     */

uint32 png_file_size;       /* PNG file size                               */
uint32 file_byte_count;     /* running count of chunk bytes read           */

void *png_cmx_handle;       /* callback handle for cmx data get and put    */
png_cmx_decode_cb_func_type  png_cmx_cb = NULL;   /* callback for caller   */
const void                   *png_client_data;    /* PNG client data       */
LOCAL uint32 *png_id = 0;   /* PNG client data for tracking callbacks      */

/* The following variables are used to keep track of client buffers. If
** client buffers are not available, then internal buffers are used.
*/
LOCAL uint8  *png_output_buffer          = NULL;  /* Client output buffer  */
LOCAL uint32 png_output_buffer_size      = 0;     /* Size of output buffer */
LOCAL uint8  *png_output_buffer_pend     = NULL;  /* Pending client buffer */
LOCAL uint32 png_output_buffer_size_pend = 0;     /* Size of pending buf   */

void process_png_image (void *);
uint32 decode_byte ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION  png_crc32

DESCRIPTION
  This function updates the running CRC32 value for the chunk.

DEPENDENCIES
  None

RETURN VALUE
  The updated CRC value.

SIDE EFFECTS
  None

===========================================================================*/
uint32 png_crc32
(
  uint32 crc,                  /* current CRC32 value                      */
  uint8  *buffer,              /* data buffer to add to the CRC value      */
  uint32 length                /* data buffer length                       */
)
{
  uint32 result = crc;

  static uint32 const crc_table[] = 
  {
    0x0,        0x77073096, 0xee0e612c, 0x990951ba, 0x76dc419,  0x706af48f, 
    0xe963a535, 0x9e6495a3, 0xedb8832,  0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 
    0x9b64c2b,  0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x1db7106, 
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x6b6b51f,  0x9fbfe4a5, 0xe8b8d433, 
    0x7807c9a2, 0xf00f934,  0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x86d3d2d, 
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x3b6e20c,  0x74b1d29a, 
    0xead54739, 0x9dd277af, 0x4db2615,  0x73dc1683, 0xe3630b12, 0x94643b84, 
    0xd6d6a3e,  0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0xa00ae27,  0x7d079eb1, 
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e, 
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x26d930a,  0x9c0906a9, 0xeb0e363f, 
    0x72076785, 0x5005713,  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0xcb61b38, 
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0xbdbdf21,  0x86d3d2d4, 0xf1d4e242, 
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
  };

  /* update current CRC value                                              */
  for ( ; length != 0; length--)
  {
    result = crc_table[(result ^ *buffer++) & 0xFF] ^ (result >> 8);
  }
  return (result);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  png_decode_init

DESCRIPTION
  This function initializes the variables used in the PNG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void png_decode_init (void)
{

  chunk_length       = 0;       /* zero out chunk length                   */
  row_count          = 0;       /* zero out row count                      */
  data_position      = 0;       /* zero out data position                  */  
  file_byte_count    = 0;       /* zero out file byte count                */
  
  /* set various status flags to the appropriate value                     */
  data_read              = FALSE;
  palette_read           = FALSE;
  transparency_read      = FALSE;
  background_read        = FALSE;
  new_dataset_flag       = FALSE;
  final_data_set         = FALSE; 
  get_idat_data          = FALSE;
  lz_init_flag           = FALSE;
  png_crc_failed         = FALSE;

  /* Use client buffer if available */
  if(png_output_buffer_pend != NULL) {
    png_output_buffer           = png_output_buffer_pend;
    png_output_buffer_size      = png_output_buffer_size_pend;
    png_output_buffer_pend      = NULL;
    png_output_buffer_size_pend = 0;
  } else {
    png_output_buffer      = temp_buf;
    png_output_buffer_size = IMAGE_WIDTH_MAX*2*IMAGE_DEPTH_MAX/CHAR_BIT;
  }

  /* clear memory for png_output_buffer, background color, and transparency */
  memset ((uint8 *) png_output_buffer, 0, png_output_buffer_size);
  memset (bkgd_color, 0, sizeof(bkgd_color));
  memset (transparency_value, 0, sizeof(transparency_value));
}

/* <EJECT> */
/*===========================================================================

FUNCTION   process_png_header 

DESCRIPTION
  This function processes an IHDR chuck.  The IHDR chunk processes the image
  header which contains the image specifications.  The color table for 
  grayscale images is also created.  

DEPENDENCIES
  None

RETURN VALUE
  0 - header processed successfully
  1 - header processing failed   

SIDE EFFECTS
  None

===========================================================================*/
uint32 process_png_header (void)
{
  uint8 *cd_ptr = chunk_data;

  /* do byte swapping to get width of image                                */
  image_width = *cd_ptr++;
  image_width = (image_width << 8) | *cd_ptr++;
  image_width = (image_width << 8) | *cd_ptr++;
  image_width = (image_width << 8) | *cd_ptr++;

  /* do byte swapping to get height of image                               */
  image_height = *cd_ptr++;
  image_height = (image_height << 8) | *cd_ptr++;
  image_height = (image_height << 8) | *cd_ptr++;
  image_height = (image_height << 8) | *cd_ptr++;

  image_depth              = *cd_ptr++;           /* bit depth of image    */
  image_color_type         = *cd_ptr++;           /* image color type      */
  image_compression_method = *cd_ptr++;           /* compression type used */
  image_filter_method      = *cd_ptr++;           /* filter method used    */
  image_interlace_method   = *cd_ptr++;           /* interlace method used */

  /* ensure that the image header values are valid                         */
  switch (image_depth)
  {
    case 1: case 2: case 4: case 8: case 16: break ;
    default: 
      MSG_ERROR("Invalid Image Depth: %d", image_depth, 0, 0) ;
      return (1);
  }

  if( image_width > IMAGE_WIDTH_MAX )
  {
    MSG_ERROR("IMAGE WIDTH of %d > MAX WIDTH of %d", 
                         image_width, IMAGE_WIDTH_MAX, 0) ;
    return (1);
  }

  if( image_height > IMAGE_HEIGHT_MAX )
  {
    MSG_ERROR("IMAGE HEIGHT of %d > MAX HEIGHT of %d", 
                         image_width, IMAGE_HEIGHT_MAX, 0) ;
    return (1);
  }

  /* this decoder implementation supports only non-interlaced images       */ 
  if( image_interlace_method )
  {
    MSG_ERROR("Interlaced Images Not Supported", 0, 0, 0); 
    return (1);
  }

  /* there is only one compression method that is valid in PNG spec        */
  if ( image_compression_method ) 
  {
    MSG_ERROR("Invalid Compression Method",0,0,0) ;
    return (1);
  }

  /* there is only one filter method that is valid in PNG spec             */
  if ( image_filter_method ) 
  {
    MSG_ERROR("Invalid Filter Method ", 0,0,0) ;
    return (1);
  }

  return (0) ;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  get_byte_from_idat_buffer

DESCRIPTION
  Helper function of get_idat_byte which manages the ping-pong buffers
  holding fetched idat data.

DEPENDENCIES
  None

RETURN VALUE
  uncompress IDAT chunk data byte 

SIDE EFFECTS
  None

===========================================================================*/
uint8 get_byte_from_idat_buffer ( void )
{
  uint8 value;

  if (png_decoder_abort_flag)
    return 0xFF;

  /* get more data if current buffer is exhausted */
  if (idat_buffer[idat_buffer_select].buf_size == 0)
  {
    /* set position in input data stream to get data                     */ 
    cmx_get_data.position = data_position + 
      idat_buffer[1-idat_buffer_select].buf_size;  

    cmx_get_data.num_bytes = MAX_DATA_BYTES;
    if (cmx_get_data.num_bytes > png_file_size - cmx_get_data.position)
    {
      cmx_get_data.num_bytes = png_file_size - cmx_get_data.position;
    }
    cmx_get_data.buffer = (idat_buffer_select) ? chunk_data + MAX_DATA_BYTES : chunk_data;

    idat_buffer[idat_buffer_select].buf_ptr = cmx_get_data.buffer;
    idat_buffer[idat_buffer_select].buf_size = cmx_get_data.num_bytes;
    idat_buffer[idat_buffer_select].clean = FALSE;

    if (cmx_get_data.num_bytes > 0)
    {
      get_idat_data = TRUE;                    /* set flag to get more data*/
    }
    idat_buffer_select = 1 - idat_buffer_select;
  }

  /* under normal circumstance, the secondary buffer should contain valid data */
  /* bail out if otherwise */
  if (idat_buffer[idat_buffer_select].buf_size == 0 || 
      !idat_buffer[idat_buffer_select].clean)
  {
    MSG_ERROR ("Insufficient data in IDAT-chunk. Decoding failed.", 0, 0, 0);
    png_decoder_abort_flag = TRUE;            /* set abort flag            */
    return 0xFF;
  }
                                                            
  /* get idat byte from chunk data stream                                  */
  value = *(idat_buffer[idat_buffer_select].buf_ptr)++; 

  idat_buffer[idat_buffer_select].buf_size--;
  data_position++;

  return value;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  get_idat_byte

DESCRIPTION
  This function returns the next uncompressed data byte within the IDAT
  block data stream. The value returned is either the next byte in the
  current IDAT block or the first byte in the next IDAT block in the
  input stream.

DEPENDENCIES
  None

RETURN VALUE
  uncompress IDAT chunk data byte 

SIDE EFFECTS
  None

===========================================================================*/
uint32 get_idat_byte ( void )
{
  uint8 data_byte;

  if (idat_offset == 0xFFFFFFFF)
    return idat_offset;
  
  data_byte = get_byte_from_idat_buffer ();

  /* update running CRC                                                    */
  crc_value = png_crc32 (crc_value, &data_byte, 1); 

  idat_offset++;

  /* finished current idat chunk                                           */
  if (idat_offset == chunk_length)
  {    
    uint32 chunk_crc;
    /* finalize the running CRC value from the IDAT chunk                  */
    crc_value ^= 0xFFFFFFFF;

    /* end of chunk contains CRC so assemble by byte swapping data         */
    chunk_crc = get_byte_from_idat_buffer ();
    chunk_crc = (chunk_crc << 8) | get_byte_from_idat_buffer ();
    chunk_crc = (chunk_crc << 8) | get_byte_from_idat_buffer ();
    chunk_crc = (chunk_crc << 8) | get_byte_from_idat_buffer ();

    /* CRC Check                                                           */
    if (crc_value != chunk_crc) 
    {
      png_crc_failed = TRUE;      
      MSG_ERROR("Invalid CRC: %lx, %lx", crc_value, chunk_crc, 0) ;
    }

    /* parse next idat chunk                                               */
    idat_offset = 0xFFFFFFFF;
    chunk_length = get_byte_from_idat_buffer ();
    chunk_length = (chunk_length << 8) | get_byte_from_idat_buffer ();
    chunk_length = (chunk_length << 8) | get_byte_from_idat_buffer ();
    chunk_length = (chunk_length << 8) | get_byte_from_idat_buffer ();

    chunk_type[0] = get_byte_from_idat_buffer ();
    chunk_type[1] = get_byte_from_idat_buffer ();
    chunk_type[2] = get_byte_from_idat_buffer ();
    chunk_type[3] = get_byte_from_idat_buffer ();

    if (strncmp((char *) chunk_type, "IDAT", 4) == 0)
    {
      file_byte_count += (chunk_length + 12);
      idat_offset = 0;
      crc_value = 0xFFFFFFFF;
      crc_value = png_crc32 (crc_value, chunk_type, 4);
    }
    else
    {
      data_position -= 8;
    }
  }

  bit_offset = CHAR_BIT ;                       /* reset bit offset        */

  return (uint32)data_byte ;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  get_bits

DESCRIPTION
  This function returns a number of uncompressed bits from the
  IDAT block data stream.

DEPENDENCIES
  None

RETURN VALUE
  A value based on the number of bits asked to assembled.

SIDE EFFECTS
  None

===========================================================================*/
uint32 get_bits
(
  uint32 count                    /* number of bits to extract             */
)
{
  uint32         ii;
  uint32         value = 0;       /* data value of combined bits extracted */
  uint32         local_offset = bit_offset;

  for (ii = 0; ii < count; ii++, local_offset++)
  {
    /* see if we have consumed all the bits in the current byte.
    ** if so get the next byte from the input data stream
    */ 
    if (local_offset >= CHAR_BIT)
    {
      bit_buffer = get_idat_byte ();
      local_offset = 0;
    }
    /* check if the bit is set and if so then update data value            */ 
    if ((bit_buffer >> local_offset) & 01) 
    {
      value |= (1<<ii);
    }
  }

  bit_offset = local_offset;

  return ( value );
}

/* <EJECT> */
/*===========================================================================

FUNCTION  make_dynamic_huffman_table

DESCRIPTION
  This function creates a dynamic Huffman table from an array of code lengths.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void make_dynamic_huffman_table 
(
  literal_huffman_decoder_data_type *table,
  uint32 valuecount,                           /* number of huffman values */
  uint8  *codelengths                          /* length of huffman code   */
)
{
  uint32 ii;
  uint32 last;
  uint32 last_length;
  uint32 code;
  uint32 sf;

  /* table->mincode [n] : The smallest Huffman code of length n + 1.
  ** table->maxcode [n] : The largest Huffman code of length n + 1.
  ** table->valptr [n]  : Index into the values array. First value with a code
  **                      of length n + 1.
  */
  for (ii = 0 ; ii < MAX_HUFFMAN_CODE_LENGTH ; ii++)
  {
    table->valptr [ii] = 0 ;
    table->mincode [ii] = BADCODE ;
    table->maxcode [ii] = -1 ;
  }

  for (ii = 0, last_length=0, code=0, last = 0 ; ii < valuecount ; ii++)
  {
    if (last_length < codelengths[ii])
    {
       sf = codelengths[ii] - last_length;
       last_length += sf;
       code <<= sf;
    }
 
    if (last != codelengths[ii])
    {
       last = codelengths[ii] ;
       table->valptr [last-1] = (uint16) ii ;
       table->mincode [last-1] = code ;
    }
    if (last != 0)
    {
      table->maxcode [last-1] = code ;
    }
    
    if (last_length != 0) code++;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  png_huffman_decoder

DESCRIPTION
  This function decodes the next Huffman-encoded value in the input
  stream.

DEPENDENCIES
  None

RETURN VALUE
  Huffman decoded value    

SIDE EFFECTS
  None

===========================================================================*/
uint32 png_huffman_decoder
(
  const literal_huffman_decoder_data_type *table
) 
{
   uint32 index;
   uint32 code;
   uint32 codelength; 
   uint32 lbit_offset = bit_offset;
   uint32 lbit_buffer = bit_buffer;

   /* see if we have consumed all the bits in the current byte.
   ** if so get the next byte from the input data stream
   */ 
   if (lbit_offset >= CHAR_BIT)
   {
     lbit_buffer = get_idat_byte ();
     lbit_offset = 0;
   }

   /* decode the next byte in the input stream using the Huffman table     */
   code = (lbit_buffer >> lbit_offset++) & 01;

   /* take advantage of the fact that 1 bits are used as
   ** a prefix to the longer codes. codelength should never increment
   ** beyond MAX_HUFFMAN_CODE_LENGTH
   */
   for (codelength=0; ((codelength < MAX_HUFFMAN_CODE_LENGTH) &&
                       ((int) code > table->maxcode [codelength])); codelength++) 
   { 
     if (lbit_offset >= CHAR_BIT)
     {
       lbit_buffer = get_idat_byte ();
       lbit_offset = 0;
     }
     code = (code << 1) | ((lbit_buffer >> lbit_offset++) & 01);
   }

   /* should not happen if table->maxcode has valid entries 
   ** the design of the interface does not allow a failure to be returned
   ** for now simply returning one of the huff_value similar to the
   ** error-handling below: index = 0
   */
   if (codelength == MAX_HUFFMAN_CODE_LENGTH)
   {
     return (table->huff_values[0]);
   }

   bit_offset = lbit_offset;
   bit_buffer = lbit_buffer;
   
   /* Now we have a Huffman code of length (codelength + 1) that
   ** is somewhere in the range
   ** table->mincode [codelength]..table->maxcode [codelength].
   */ 
   index = table->valptr[codelength] + code - table->mincode[codelength];

   /* make sure that index never exceeds the maximum huffman values for 
   ** the table being used. huff_values is defined as uint16 so need to 
   ** divide by 2.
   */
   if(index >= (sizeof(table->huff_values) >> 1)) index = 0;

   /* table->valptr [codelength] is the first code of length (codelength + 1)
   ** so now we can look up the value for the Huffman code in the table.
   */
   return (table->huff_values [index]); 
}

/* <EJECT> */
/*===========================================================================

FUNCTION  read_lengths 

DESCRIPTION
  This function reads Huffman-encoded code lengths for the
  Huffman table. The Huffman encoded values range from 0..18.
  The values have the following meanings:

     0..15 -> A literal length value
     16 -> Repeat the last code N times. N is found by reading
           the next 2 bits and adding 3.
     17 -> Set the N length codes to zero. N is found by reading the
           next 3 bits and adding 3.
     18 -> Set the N length codes to zero. N is found by reading the
           next 7 bits and adding 11.

DEPENDENCIES
  None

RETURN VALUE
  Return the number of bytes written to the length array
  0 indicates an error 

SIDE EFFECTS
  None

===========================================================================*/
uint32 read_lengths 
(
  literal_huffman_decoder_data_type *table,
  uint8        *lengths,        /* The output length values (sorted)       */
  uint32       lengthcount,     /* The number of length values to read     */
  uint8*       length_array,    /* The output length values (raw)          */
  uint32       write_start_offset, /* The offset in the length array to start writing */
  uint32       length_array_size   /* The size of the length_array */
)
{
  uint32 written = 0;
  uint32 count, command;
  uint8  *length_ptr;
  uint32 max_command = 0;
  int32 index = lengthcount - write_start_offset;

  length_ptr = length_array + write_start_offset;

  while ( index > 0 )
  {
    command = png_huffman_decoder ((literal_huffman_decoder_data_type *) &ht);

    if (written == length_array_size) 
    {
      return 0; /* error - insufficient space in length_array to store lengths */
    }
    /* for values 0..15 command is actual length value                     */
    if (command < 16)
    {
      *length_ptr++ = (uint8) command;           /* literal length                 */
      written++;
      index--;
      if (command > max_command)         /* keep track of max length found */
      {
         max_command = command;
      }
    }
    else if (command == 16)
    {
      /* 16 -> Repeat the last code N times. N is found by reading
      **       the next 2 bits and adding 3.
      */
      count = get_bits (2) + 3;
      index -= count;
      command = *(length_ptr-1);
      if (count + written > length_array_size) 
      {
        return 0; /* error - insufficient space in length_array to store lengths */
      }
      for( ; count != 0; count--)
      {
        *length_ptr++ = (uint8) command;
        written++;
      }
    }
    else if (command == 17)
    {
      /* 17 -> Set the N length codes to zero. N is found by reading the
      **       next 3 bits and adding 3.
      */
      count = get_bits (3) + 3;
      index -= count;
      if (count + written > length_array_size) 
      {
        return 0; /* error - insufficient space in length_array to store lengths */
      }
      for( ; count != 0; count--)
      {
        *length_ptr++ = 0;
        written++;
      }
    }
    else if (command == 18)
    {
      /* 18 -> Set the N length codes to zero. N is found by reading the
      **       next 7 bits and adding 11.
      */
      count = get_bits (7) + 11;
      index -= count;
      if (count + written > length_array_size) 
      {
        return 0; /* error - insufficient space in length_array to store lengths */
      }
      for( ; count != 0; count--)
      {
        *length_ptr++ = 0;
        written++;
      }
    }
    else
    {
      MSG_ERROR("Bad Code",0,0,0) ;
      return 0;
    }
  }

  /* arrange the code lengths in increasing order and record the index     */ 
  for (index=0, count=0; index <= max_command; index++)
  {
    length_ptr = length_array;
    for(command=0; command < lengthcount; command++)
    {
      if(*length_ptr++ == index)
      {
        *lengths++ = (uint8) index;
        table->huff_values[count++] = (uint16) command;
      }
    }
  } 
  return written;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  start_new_data_set

DESCRIPTION

  This function processes the start of a new data set within a compressed
  stream. The start of a data set has the following format:

     final:             1-bit (1 => this is the last data set)
     compression type:  2-bits

     The remainding data depends upon the compression type.

     Compression Type:  Uncompressed

       Advance to the next byte boundary. The next two bytes is the
       length of the uncompressed data. The following two bytes
       are the ones complement of the length.  [length] uncompressed
       bytes follow.

     Compression Type:  Fixed Huffman Codes

       The huffman encoded data bits immediately follow the type field. The
       data is encoded using the huffman lengh codes defined in the deflate
       specification.

     Compression Type: Dynamic Huffman Codes

       The trick here is that the literal and distance Huffman
       tables are Huffman-encoded. The next values in the input
       stream are:

          number of literal codes: 5-bits + 257
          number of distance codes: 5-bits + 1
          number of code lengths: 4-bits + 4

          Code Lengths: 3-bits * (code-lengths + 4)

       The code lengths are used to create a huffman table that encodes
       the literal table followed by the length table.

DEPENDENCIES
  None

RETURN VALUE
  0 - new data set processed successfully
  1 - new data set processing failed   

SIDE EFFECTS
  None

===========================================================================*/
uint32 start_new_data_set ( void )
{
  uint32 ii, jj, kk;
  uint8  lengths [PNG_LENGTH_SIZE] = {0};
  uint8  dyn_distances [32] = {0};
  uint32 HLIT;
  uint32 HDIST;
  uint32 HCLEN;
  uint32 testcount;
  uint8 *length_ptr;
  uint32 const *lo_ptr;
  compression_type compression_method;  /* current compression method used */
  uint32 lengths_read;

  static uint32 const png_length_order[PNG_LENGTH_SIZE] =
  {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
  };
  static uint8 length_array[MAX_NUMBER_HUFFMAN_CODES+MAX_NUMBER_DISTANCE_CODES] = {0};

  /* check if final data set                                               */
  if (get_bits (1) == 0) 
  {
    final_data_set = FALSE;
  } else {
    final_data_set = TRUE;
  }

  /* get bits indicating compression method used                           */
  compression_method = (compression_type) get_bits (2); 

  if (compression_method == UNCOMPRESSED)
  {
    literal_mode = TRUE;
    literal_count = get_idat_byte () | (get_idat_byte () << CHAR_BIT);
    testcount = get_idat_byte () | (get_idat_byte () << CHAR_BIT);

    if ((literal_count & 0xFFFF) != (~testcount & 0xFFFF)) 
    {
      MSG_ERROR("Invalid Literal Count",0,0,0) ;
      return (1);
    }
  }
  else if (compression_method == FIXEDHUFFMANCODES)
  {
    ht_literal_table = &fixed_literal_table;
    ht_distance_table = (literal_huffman_decoder_data_type *) 
                                                &fixed_distance_table;
    literal_mode = FALSE;
  }
  else if (compression_method == DYNAMICHUFFMANCODES)
  {
   
    HLIT =  get_bits (5) + 257;
    HDIST = get_bits (5) + 1;
    HCLEN = get_bits (4) + 4;

    /* Read the length codes used to huffman encode the literal and
    ** distance tables. The unusual thing here is the Huffman values
    ** are not in the order 0..18 but rather the order defined by
    ** the lengthindices array.
    */
    if (HCLEN > PNG_LENGTH_SIZE) 
    {
      MSG_ERROR("Invalid Huffman Code Length",0,0,0);
      return (1);
    }

    lo_ptr = png_length_order;

    /* get code lengths.  use dyn_distances as a temporary buffer          */
    while ( HCLEN-- != 0 )
    {
      *(dyn_distances + *lo_ptr++) = (uint8) get_bits (3);
    }

    length_ptr = lengths;

    /* arrange the code lengths in increasing order and record the index   */ 
    for (ii=0, kk=0; ii <= 8; ii++)
    {
      for(jj=0; jj<PNG_LENGTH_SIZE; jj++)
      {
        if(dyn_distances[jj] == ii)
        {
          *length_ptr++ = (uint8) ii;
          ht.huff_values[kk++] = (uint16) jj;
        }
      }
    } 

    make_dynamic_huffman_table ((literal_huffman_decoder_data_type *) &ht, 
                                                    PNG_LENGTH_SIZE, lengths);

    /* using the Huffman table we just created read the length/literals
    ** and distances Huffman tables.
    */
    lengths_read = read_lengths (&dynamic_literal_table, dyn_literals, HLIT, length_array, 
                                    0, MAX_NUMBER_HUFFMAN_CODES+MAX_NUMBER_DISTANCE_CODES);
    if (lengths_read == 0) 
    {
      MSG_ERROR("Error in read_lengths()", 0, 0, 0);
      return (1);
    }
    lengths_read = read_lengths ((literal_huffman_decoder_data_type *) &dynamic_distance_table,
                                    dyn_distances, HDIST, length_array+HLIT, lengths_read-HLIT,
                                    MAX_NUMBER_HUFFMAN_CODES+MAX_NUMBER_DISTANCE_CODES-lengths_read);
    if (lengths_read == 0) 
    {
      MSG_ERROR("Error in read_lengths()", 0, 0, 0);
      return (1);
    }

    make_dynamic_huffman_table (&dynamic_literal_table, HLIT, dyn_literals);
                                                         
    make_dynamic_huffman_table (
                  (literal_huffman_decoder_data_type *) &dynamic_distance_table, 
                                                          HDIST, dyn_distances);
                                                         
    ht_literal_table = &dynamic_literal_table;
    ht_distance_table = 
                  (literal_huffman_decoder_data_type *) &dynamic_distance_table;

    literal_mode = FALSE;
  }
  else
  {
    MSG_ERROR("Invalid Compression Type",0,0,0) ;
    return (1);
  }
  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  idat_end 

DESCRIPTION
  This function is called after IDAT image has been decoded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 idat_end ( void )
{
  uint32 value;

  if ( !literal_mode )
  {
    /* when we arrive here we should have read the last data byte. The next
    ** code in the stream should be the end marker.
    */
    value = png_huffman_decoder (ht_literal_table);
    if (value != 256) 
    {
      MSG_ERROR("Missing Last End Marker\n",0,0,0);
      return (1);
    }
  }
  else
  {
    if ( literal_count != 0 ) 
    {
      MSG_ERROR("Extra data",0,0,0);
      return (1);
    }
  }

  /* read any empty data sets following the image data                     */
  while ( !final_data_set )
  {
    (void) start_new_data_set ();
    if (literal_mode)
    {
       if ( literal_count != 0 ) 
       {
         MSG_ERROR("Extra data",0,0,0);
         return (1);
       }
    }
    else
    {
      value = png_huffman_decoder (ht_literal_table);
      if ( value != 256 ) 
      {
        MSG_ERROR("Missing End Marker",0,0,0);
        return (1);
      }
    }
  }

  /* bypass 4 adler checksum bytes after the end marker                    */
  (void) get_idat_byte ();
  (void) get_idat_byte ();
  (void) get_idat_byte ();
  (void) get_idat_byte ();

  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  decode_byte

DESCRIPTION
  This function returns the next encoded byte in the deflate stream.
  First determine if we are in a literal or compressed data set
  then call a function to handle the appropriate type.

DEPENDENCIES
  None

RETURN VALUE
  Decoded byte

SIDE EFFECTS
  None

===========================================================================*/
uint32 decode_byte ( void )
{
  uint32 value;
  uint32 length ;
  uint32 distance ;
  int32  extra;
  uint32 index;
  static uint32 window_position;           /* current LZ window position   */ 
  static uint32 copy_position;          /* current LZ window copy position */
  static uint32 copy_count;             /* bytes to copy within LZ window  */
  
  /* number of extra bits for (code-257)                                   */
  static uint32 const length_extra [] =    
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
    4, 4, 4, 4, 5, 5, 5, 5, 0
  };

  /* The smallest length value for code-257. The actual length value is
  ** the sum of this value and the extra bits.
  */
  static uint32 const length_base [] =
  {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
    67, 83, 99, 115, 131, 163, 195, 227, 258
  };

  /* The number of extra bits for a distance code.                         */
  static uint32 const distance_extra [] =
  {
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
    9, 9, 10, 10, 11, 11, 12, 12, 13, 13
  };

  /* The smallest value for a distance code.                               */
  static uint32 const distance_base [] =
  {
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129,
    193, 257, 385, 513, 769, 1025, 1537,  2049,  3073,  4097,
    6145, 8193, 12289, 16385, 24577
  };

  if (!lz_init_flag)
  {
    /* initialize the LZ77 compression state variables                     */
    window_position = 0;
    copy_position   = 0;
    copy_count      = 0;
    memset (lz_window, 0, PNG_WINDOW_SIZE);
    lz_init_flag = TRUE;
  }

  /* Handle the easy case of a literal mode data stream. Compressed
  ** data sets are handled below.
  */
  do 
  {
    new_dataset_flag = FALSE;

    /* decode either literal byte or compressed byte stream                */ 
    if ( literal_mode )
    {
      /* The following code returns the next byte in an uncompressed data
      ** stream.
      */

      /* if count is 0 then possibly the start of a new data set else      */
      /* get next byte in data stream                                      */
      if (literal_count == 0)
      {
        if (final_data_set)
        {
          MSG_ERROR("Premature End-Of-File 1",0,0,0);
        }
        else
        {
          new_dataset_flag = TRUE;                /* set new data set flag */
        }
        value = 0;
      }
      else
      {
        value = get_idat_byte();           /* get next byte in data stream */
        lz_window [window_position] = (uint8) value;
  
        if((window_position += 1) >= 32768) window_position = 0;
        literal_count--;                                
      }
    }
    else
    {
      /* The following code decodes the next value in the input stream. The
      ** stream format is:
      ** 
      ** Huffman encoded length/literal value. This value can be:
      **   Data byte:   0..255
      **   End Marker:  256
      **   Length Code: 257..285
      ** 
      ** If the code is a length code then it is followed by 0..13
      ** additional literal bytes. This is followed by a huffman encoded
      ** distance value which is followed by 0..13 literal bits.
      ** 
      ** This code assumes that the data value is in the compressed stream.
      */

      /* see if we are still performing a copy operation                   */
      if ( copy_count )
      {
        /* copy the value into the LZ window                               */
        value = lz_window [copy_position];
        lz_window [window_position] = (uint8) value;
        copy_count-- ;

        /* advance the copy and window positions                           */
        if((copy_position += 1) >= 32768) copy_position = 0;
        if((window_position += 1) >= 32768) window_position = 0;
      }
      else
      {

        value = png_huffman_decoder (ht_literal_table);

        if (value < 256)
        {
          /* This is a data value. Add the value to the LZ window.
          */
          lz_window [window_position] = (uint8) value ;

          if((window_position += 1) >= 32768) window_position = 0;
        }
        else if (value == 256)
        {
          /* We just read the end marker. There should be another data set in
          ** the input stream that contains the data value
          */
          if ( final_data_set )
          {
            /* The current data set end the final bit set. That means there
            ** should be no more data sets in the stream
            */
            MSG_ERROR("Premature End-Of-File 2",0,0,0);
          }
          else
          {
            /* the data value is in the next data set                        */
            new_dataset_flag = TRUE;
          }
          value = 0;
        }
        else if (value < 286)
        {
          index = value-257;

          /* The code specifies a length value. Read the extra bits
          ** to find the actual length value.
          */
          length = length_base [index];
          extra = length_extra [index];
          if ( extra )
          {
            length += get_bits (length_extra [index]);
          }

          /* The length value is followed by the distance value. Decode the
          ** value then add the extra bits to get the distance value.
          */
          value = png_huffman_decoder (ht_distance_table);

          if (value > 29) 
          {
            MSG_ERROR("Invalid Huffman Distance Value",0,0,0);
            break;
          }

          extra    = distance_extra [value];
          distance = distance_base [value];
          if ( extra ) 
          {
            distance += get_bits ( distance_extra [value] );
          }

          /* set the state variables that are used to find the following copied
          ** bytes
          */
          copy_position = (PNG_WINDOW_SIZE + window_position - distance) & 0x7FFF;
          copy_count = length ;

          /* return the first copy byte                                      */
          value = lz_window [copy_position];
          lz_window [window_position] = (uint8) value;

          if((copy_position += 1) >= 32768) copy_position = 0;
          if((window_position += 1) >= 32768) window_position = 0;

          copy_count--;
        }
        else
        {
          MSG_ERROR("Invalid Huffman Literal Value",0,0,0);
          value = 0;
        }
      }
    }

    if ( new_dataset_flag )
    {
       (void) start_new_data_set();    
    }

  } while ( new_dataset_flag );
  
  return ( value );
}

/*===========================================================================

FUNCTION  paeth_predictor

DESCRIPTION
  Computes a simple linear function of three neighboring pixels then chooses
  as predictor the neighboring pixel closest to the computed value.

DEPENDENCIES
  None

RETURN VALUE
  Pixel from selected position 

SIDE EFFECTS
  None

===========================================================================*/

uint32 paeth_predictor
(
  int32 left,             /* value for the pixel left of the current pixel */
  int32 above,            /* value for the pixel above the current pixel   */
  int32 upperleft         /* pixel above and to the left of current pixel  */
)
{
  int32 pp;
  int32 pa, pb, pc;

  pp = left + above - upperleft;                /* initial estimate        */
 
  /* find distances to left, above, and upper left                         */
  if (pp > left)
  {
    pa = pp - left;
  }
  else
  {
    pa = left - pp;
  }

  if (pp > above)
  {
    pb = pp - above;
  }
  else
  {
    pb = above - pp;
  }

  if (pp > upperleft)
  {
    pc = pp - upperleft;
  }
  else
  {
    pc = upperleft - pp;
  }

  /* return the nearest of pa, pb, pc.  Break ties in order pa, pb, pc.    */ 
  if (pa <= pb && pa <= pc)
  {
    return ( left );
  }
  else if (pb <= pc)
  {
    return ( above );
  }
  else
  {
    return ( upperleft );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  filter_row

DESCRIPTION
  This function filters a row of data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void filter_row 
(
  uint32 filter
)
{
  uint32 left, above, aboveleft ;
  int32 vv;
  uint32 lastrow;                             /* Index of the previous row */
  uint32 col;                                 /* current column            */
  uint32 offset = 0; 
  uint32 pp;
  uint32 left2;
  uint32 above2;

  lastrow = ! current_row_buffer;             /* index of the previous row */

  /* Filtering is done on corresponding items within a record. Determine
  ** the number of bytes between corresponding items.
  */
  switch (image_color_type)
  {
    case PALETTE:
      offset = 1;
      break;
    case GRAYSCALE: 
      offset = (image_depth + CHAR_BIT - 1 ) / CHAR_BIT;
      break;
    case GRAYSCALE_ALPHA:
      offset = 2 * image_depth / CHAR_BIT;
      break;
    case RGB:
      offset = 3 * image_depth / CHAR_BIT;
      break;
    case RGB_ALPHA:
      offset = 4 * image_depth / CHAR_BIT;
      break;
    default:
      MSG_ERROR("Invalid Color Type",0,0,0);
      break;
  }

  /* Filter the row based upon the filter type.                            */
  switch (filter)
  {
    case FILTERNONE:
      break;

    case FILTERSUB:
      /* The value is the difference from the value to the left            */
      for (col = offset; col < row_buffer_width; col++)
      {
        row_buffers [current_row_buffer][col] =
                       (row_buffers [current_row_buffer][col] +
                       row_buffers [current_row_buffer][col-offset]) & 0xFF;
      }
      break ;

    case FILTERUP:
      /* the value is the difference from the value in the previous row.   */
      for (col = 0; col < row_buffer_width; col++)
      {
        row_buffers [current_row_buffer][col] =
                                      (row_buffers [current_row_buffer][col]
                                      + row_buffers [lastrow][col]) & 0xFF;
      }
      break ;

    case FILTERAVERAGE:
      for (col = 0; col < row_buffer_width; col++)
      {
        above2 = row_buffers [lastrow][col];
        if (col < offset)
        {
          left2 = 0;
        }
        else
        {
          left2 = row_buffers [current_row_buffer][col-offset];
        }
        row_buffers [current_row_buffer][col] = (uint8)
                                     ((row_buffers [current_row_buffer][col]
                                     + (left2 + above2) / 2) & 0xFF);
      }
      break;

    case FILTERPAETH:
      for (col = 0 ; col < row_buffer_width ; ++ col)
      {
        above = row_buffers [lastrow][col];
        if (col < offset)
        {
          left = 0;
          aboveleft = 0;
        }
        else
        {
          left = row_buffers [current_row_buffer][col-offset];
          aboveleft = row_buffers [lastrow][col-offset];
        }
        vv = (int) row_buffers [current_row_buffer][col];
        pp = paeth_predictor (left, above, aboveleft);
        row_buffers [current_row_buffer][col] = (uint8) ((pp + vv) & 0xFF);
      }
      break;

    default:
      MSG_ERROR("Invalid Filter Method 2",0,0,0) ;
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  copy_noninterlaced_row_to_image 

DESCRIPTION
  This function copies the data from a non-interlaced row to the image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void copy_noninterlaced_row_to_image ( void ) 
{
  uint32 ii;
  uint32 bitoffset=0;

  switch (image_color_type)
  {
    case GRAYSCALE:
    case PALETTE:
      switch (image_depth)
      {
        case 1:
          for (ii=0; ii < image_width; ii++, bitoffset++)
          {
            if ( bitoffset == 8 ) bitoffset = 0;

            png_output_buffer[ii] = 
              (row_buffers [current_row_buffer][ii >> 3] >> (1 * (7 - bitoffset))) & 0x1;
          }
          break;

        case 2:
          for (ii=0; ii < image_width; ii++, bitoffset++)
          {
            if ( bitoffset == 4 ) bitoffset = 0;

            png_output_buffer[ii] = 
              (row_buffers [current_row_buffer][ii >> 2] >> (2 * (3 - bitoffset))) & 0x3;
          }
          break ;

        case 4:
          for (ii=0; ii < image_width; ii++, bitoffset++)
          {
            if ( bitoffset == 2 ) bitoffset = 0;

            png_output_buffer[ii] = 
              (row_buffers [current_row_buffer][ii >> 1] >> (4 * (1 - bitoffset))) & 0xF;
          }
          break ;

        case 8:
          memcpy ( png_output_buffer, &row_buffers[current_row_buffer][0], 
                                                       row_buffer_width );
          break ;

        case 16:
          for (ii=0; ii < image_width; ii++ )
          {
            png_output_buffer[ii] = row_buffers[current_row_buffer][2*ii];
          }
          break ;

        default:
          MSG_ERROR("Invalid Bit Depth",0,0,0) ;
          break;
      }
      break ;

    case GRAYSCALE_ALPHA:
      /* check if image depth is 8 bits.  If not, then must be 16 bits     */
      if (image_depth == 8)
      {
         memcpy ( png_output_buffer, &row_buffers[current_row_buffer][0], 
                                                       row_buffer_width );
      }
      else
      { 
        for (ii=0; ii < 2*image_width; ii++)
        {
           png_output_buffer[ii] = row_buffers[current_row_buffer][2*ii];
        }
      }
      break ;

    case RGB:
      /* check if image depth is 8 bits.  If not, then must be 16 bits     */
      if (image_depth == 8)
      {
         memcpy ( png_output_buffer, &row_buffers[current_row_buffer][0], 
                                                       row_buffer_width );
      }
      else
      { 
        for (ii=0; ii < 3*image_width; ii++)
        {
           png_output_buffer[ii] = row_buffers[current_row_buffer][2*ii];
        }
      }
      break ;

    case RGB_ALPHA:
      /* check if image depth is 8 bits.  If not, then must be 16 bits     */
      if (image_depth == 8)
      {
         memcpy ( png_output_buffer, &row_buffers[current_row_buffer][0], 
                                                       row_buffer_width );
      }
      else
      { 
        for (ii=0; ii < 4*image_width; ii++)
        {
           png_output_buffer[ii] = row_buffers[current_row_buffer][2*ii];
        }
      }
      break ;

    default:
      MSG_ERROR("Invalid Color Type",0,0,0) ;
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION   output_image_header

DESCRIPTION
  
  The image header information includes:
     image type: 0 - GRAYSCALE
                 2 - RGB (Truecolor)
                 3 - Color-Index (Palette based)
                 4 - GRAYSCALE ALPHA
                 6 - RGB-ALPHA
     image width 
     image height 
     image depth
     transparency values (NOTE: if tRNS chunk is processed then this field 
                         will only be present for grayscale, indexed color,
                         and RGB images.                                   ) 
     background color (NOTE: present only if contained bKGD chunk processed.
                       One background color (or index) for grayscale and
                       color-index images and three colors for RGB images  )
     palette length   (NOTE: only present if palette present               )
     palette          (NOTE: present only for color-index and possibly RGB 
                       images                                              )

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void output_image_header( void )
{
  uint32 tmp;
#ifdef PNG_USES_LIBSTD
  int tstring_size;
#endif
  char tstring[50], *tstring_ptr;
  png_cmx_data_type cmx_data;
  boolean external_buffer;

  if(png_cmx_cb != NULL) 
  {
    if(png_output_buffer == temp_buf) 
    {
      tstring_ptr     = tstring;
      external_buffer = FALSE;
#ifdef PNG_USES_LIBSTD
      tstring_size = 50;
#endif
    } 
    else 
    {
      tstring_ptr     = (char *) png_output_buffer;
      external_buffer = TRUE;
#ifdef PNG_USES_LIBSTD
      tstring_size = png_output_buffer_size;
#endif
    }

#ifdef PNG_USES_LIBSTD
    (void) std_strlprintf(tstring_ptr, tstring_size, "%d\n%d %d\n%d\n\0", (int)image_color_type,
            (int)image_width, (int)image_height, (int)image_depth);
#else
    (void) sprintf(tstring_ptr, "%d\n%d %d\n%d\n\0", (int)image_color_type,
            (int)image_width, (int)image_height, (int)image_depth);
#endif
    cmx_data.buffer    = (uint8 *) tstring_ptr;
    cmx_data.num_bytes = strlen(tstring_ptr);
    png_cmx_cb(PNG_CMX_DATA, png_cmx_handle, (void *) &cmx_data);

    if( transparency_read && (transparency_length > 0) )
    {
#ifdef PNG_USES_LIBSTD
      (void) std_strlprintf(tstring_ptr, tstring_size, "tran %d\n\0", (int)transparency_length);
#else
      (void) sprintf(tstring_ptr, "tran %d\n\0", (int)transparency_length);
#endif

      cmx_data.buffer    = (uint8 *) tstring_ptr;
      cmx_data.num_bytes = strlen(tstring_ptr);
      png_cmx_cb(PNG_CMX_DATA, png_cmx_handle, (void *) &cmx_data);
      if(external_buffer) {
        memcpy (png_output_buffer, transparency_value, transparency_length); 
        cmx_data.buffer    = (uint8 *) png_output_buffer;
      } else {
        cmx_data.buffer    = (uint8 *) transparency_value;
      }
      cmx_data.num_bytes = transparency_length;
      png_cmx_cb(PNG_CMX_DATA, png_cmx_handle, (void *) &cmx_data);
#ifdef PNG_USES_LIBSTD
      (void) std_strlprintf(tstring_ptr, tstring_size, "\n\0");
#else
      (void) sprintf(tstring_ptr, "\n\0");
#endif
      cmx_data.buffer    = (uint8 *) tstring_ptr;
      cmx_data.num_bytes = strlen(tstring_ptr);
      png_cmx_cb(PNG_CMX_DATA, png_cmx_handle, (void *) &cmx_data);
    }
  
    if ( background_read )
    {
      switch (image_color_type)
      {
        case PALETTE:
        case GRAYSCALE: 
        case GRAYSCALE_ALPHA:
#ifdef PNG_USES_LIBSTD
          (void) std_strlprintf(tstring_ptr, tstring_size, "bkgd %d\n\0", (int)bkgd_color[0]);
#else
          (void) sprintf(tstring_ptr, "bkgd %d\n\0", (int)bkgd_color[0]);
#endif
          break;

        case RGB:
        case RGB_ALPHA:
#ifdef PNG_USES_LIBSTD
          (void) std_strlprintf(tstring_ptr, tstring_size, "bkgd %d %d %d\n\0", 
                                (int)bkgd_color[0], (int)bkgd_color[1], (int)bkgd_color[2]);
#else
          (void) sprintf(tstring_ptr, "bkgd %d %d %d\n\0", 
                                (int)bkgd_color[0], (int)bkgd_color[1], (int)bkgd_color[2]);
#endif
          
          break;
      }

      cmx_data.buffer    = (uint8 *) tstring_ptr;
      cmx_data.num_bytes = strlen(tstring_ptr);
      png_cmx_cb(PNG_CMX_DATA, png_cmx_handle, (void *) &cmx_data);
    } 
 
    /* output palette for index color images and possibly RGB images         */
    if ( palette_read && (palette_size > 0) )
    { 
      tmp = 3*palette_size;
#ifdef PNG_USES_LIBSTD
      (void) std_strlprintf(tstring_ptr, tstring_size, "plte %d\n\0", (int)tmp);
#else
      (void) sprintf(tstring_ptr, "plte %d\n\0", (int)tmp);
#endif

      /* output palette                                                      */
      cmx_data.buffer    = (uint8 *) tstring_ptr;
      cmx_data.num_bytes = strlen(tstring_ptr);
      png_cmx_cb(PNG_CMX_DATA, png_cmx_handle, (void *) &cmx_data);
      if(external_buffer) {
        memcpy (png_output_buffer, temp_buf, tmp); 
        cmx_data.buffer    = (uint8 *) png_output_buffer;
      } else {
        cmx_data.buffer    = (uint8 *) temp_buf;
      }
      cmx_data.num_bytes = tmp;
      png_cmx_cb(PNG_CMX_DATA, png_cmx_handle, (void *) &cmx_data);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION   output_decoded_image

DESCRIPTION
  This function outputs the color index matrix for palette based images
  and pixel data for grayscale and truecolor images.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void output_decoded_image( void )
{
  png_cmx_data_type cmx_data;

  switch (image_color_type)
  {
    case PALETTE:
    case GRAYSCALE:
      cmx_data.num_bytes = image_width;
      break;
    case GRAYSCALE_ALPHA:
      cmx_data.num_bytes = 2*image_width;
      break;
    case RGB:
      cmx_data.num_bytes = 3*image_width;
      break; 
    case RGB_ALPHA:
      cmx_data.num_bytes = 4*image_width;
      break;
    default:
      MSG_ERROR("Invalid Color Type\n",0,0,0);
      break;
  }

  /* output row of image if not scaled                                   */
  if(png_cmx_cb != NULL) 
  {
    cmx_data.buffer    = (uint8 *) png_output_buffer;
    png_cmx_cb(PNG_CMX_DATA, png_cmx_handle, (void *) &cmx_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  png_decode_noninterlaced_rows 

DESCRIPTION
  This function reads all the rows for a noninterlaced pass.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 png_decode_noninterlaced_rows ( void )
{
  uint8 *row_buf_ptr;
  uint32 row_width;
  filter_type filter;

  filter = (filter_type) decode_byte ();
  switch (filter)
  {
    case FILTERNONE: case FILTERSUB: case FILTERUP: case FILTERAVERAGE:
    case FILTERPAETH: break;
    default:
      MSG_ERROR("Invalid Filter Type: %d", filter, 0, 0);
      return (1);
  } 

  row_buf_ptr = row_buffers [current_row_buffer];
  row_width = row_buffer_width;

  for ( ; row_width != 0; row_width--)
  {
    *row_buf_ptr++ = (uint8) decode_byte ();
  }

  filter_row ( filter );
  copy_noninterlaced_row_to_image ();

  output_decoded_image();

  /* switch row buffer                                                     */
  current_row_buffer = !current_row_buffer;     

  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_idat_header 

DESCRIPTION
  This function processes the header of the IDAT data stream.

  The Deflate compression system supports many options that are not
  permitted in PNG. This is the reason for checking for specific
  parameter values.

DEPENDENCIES
  None

RETURN VALUE
  0 - header of IDAT data processed successfully
  1 - header of IDAT data processed failed    

SIDE EFFECTS
  None

===========================================================================*/
uint32 process_idat_header ( void )
{
  uint32 data1, data2;
  uint32 CM, CINFO;
  int32 FDICT;
  uint32 windowsize;

  /* ensure that we only have one IDAT stream in an image stream.          */
  if ( data_read ) 
  {
    MSG_ERROR("Image Contains Multiple Data Segments",0,0,0) ;
    return (1);
  }
  data_read = TRUE ;

  /* If the image requires a palette then it must have been read by
  ** the time we get here.
  */
  if (image_color_type == PALETTE && !palette_read ) 
  {
    MSG_ERROR("PLTE block must occur before IDAT",0,0,0) ;
    return (1);
  }

  byte_offset = 0;
  bit_buffer = 0;
  bit_offset  = CHAR_BIT;

  /* Read the compressed stream header                                     */
  data1 = get_idat_byte ();
  CM = (data1 & 0x0F) ; // Compression Method
  CINFO = (data1 & 0xF0) >> 4;
  windowsize = (1 << (CINFO + 8));

  data2 = get_idat_byte () ;
  FDICT = (data2 & (1 << 5)) != 0;

  /* Make sure the header values are valid for PNG.                        */
  if (CM != 8) 
  {
    MSG_ERROR("Invalid Compression Method - Not (8) Deflate",0,0,0);
    return (1);
  }

  if ((data2 | (data1 << 8)) % 31 != 0) 
  {
    MSG_ERROR("Invalid IDAT flags",0,0,0);
    return (1);
  }

  if (windowsize > (1 << 15)) 
  {
    MSG_ERROR("Invalid Compression Window",0,0,0);
    return (1);
  }

  if (FDICT) 
  {
    MSG_ERROR("Preset dictionary flag set",0,0,0);
  }

  /* Read the start of the new Deflate data set                            */
  if( start_new_data_set () )
  {
    MSG_ERROR("Error in start new data set ",0,0,0);
    return (1);
  }

  /* This block determines the number of bytes needed to store each
  ** data row of the image. Then allocate two buffers to hold row
  ** data.  We need two row buffers to support filtering.
  */
  switch (image_color_type)
  {
    case GRAYSCALE:
    case PALETTE:
      row_buffer_width = (image_width * image_depth + CHAR_BIT - 1) / CHAR_BIT;
      break ;
    case GRAYSCALE_ALPHA:
      row_buffer_width = 2 * image_width * image_depth / CHAR_BIT;
      break;
    case RGB:
      row_buffer_width = 3 * image_width * image_depth / CHAR_BIT;
      break;
    case RGB_ALPHA:
      row_buffer_width = 4 * image_width * image_depth / CHAR_BIT;
      break;
    default:
      MSG_ERROR("Invalid Color Type",0,0,0);
      break;
  }
  
  /* Initialize the input buffers.                                         */
  current_row_buffer = 0 ;
  memset (row_buffers [0], 0, row_buffer_width);
  memset (row_buffers [1], 0, row_buffer_width);

  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_png_palette 

DESCRIPTION
  This function processes the data in a PLTE block. A PLTE block
  defines the color palette for the image.

DEPENDENCIES
  None

RETURN VALUE
  0 - palette chunk processed successfully
  1 - palette chunk processed failed    

SIDE EFFECTS
  None

===========================================================================*/
uint32 process_png_palette ( void )
{
  uint32 ii;

  /* Grayscale images should not have an palette                           */
  if (image_color_type == GRAYSCALE || image_color_type == GRAYSCALE_ALPHA)
  {
    MSG_ERROR("ERROR: Grayscale image contains a PLTE block", 0, 0, 0);
    return (1);
  }
  
  /* There can only be one palette for the image.                          */
  if ( palette_read ) 
  {
    MSG_ERROR("Duplicate PLTE block", 0, 0, 0);
    return (1);
  }
    
  palette_read = TRUE;

  if (chunk_length == 0)
  {
    MSG_ERROR("PLTE chunk contains no data", 0, 0, 0);
    return (0);
  }

  /* The palette size must be divisable by 3.                              */
  if (chunk_length % 3 != 0)
  {
    MSG_ERROR("PLTE chunk length not divisible by 3", 0, 0, 0);
    return (1);
  }

  if ((image_color_type == PALETTE) && (chunk_length > (uint32) (3*(1<<image_depth))))
  {
    MSG_ERROR("PLTE chunk length too large for Palette image", 0, 0, 0);
    return (1);
  }
    
  palette_size = chunk_length / 3;

  if (palette_size > 256)
  {
    MSG_ERROR("PLTE chunk length too large", 0, 0, 0);
    return (1);
  }

  /* Store the palette                                                     */
  for (ii = 0; ii < chunk_length; ii++)
  {
    temp_buf[ii] = chunk_data[ii];
  }

  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_png_transparency 

DESCRIPTION
  This function processes the data in a tRNS block. A tRNS block
  defines the transparency for the image.

DEPENDENCIES
  None

RETURN VALUE
  0 - transparency chunk processed successfully
  1 - transparency chunk processed failed    

SIDE EFFECTS
  None

===========================================================================*/
uint32 process_png_transparency ( void )
{
  uint32 tmp_trans;

  /* Check to make sure the palette has been read                          */
  if ((image_color_type == PALETTE) && (!palette_read)) 
  {
    MSG_ERROR("PLTE Chunk has not been read before the tRNS chunk", 0, 0, 0);
    return (1);
  }

  /* There can only be one palette for the image                           */
  if ( transparency_read ) 
  {
    MSG_ERROR("Duplicate tRNS chunk", 0, 0, 0);
    return (1);
  }

  transparency_read = TRUE;

  if (chunk_length == 0)
  {
    MSG_ERROR("tRNS chunk contains no data", 0, 0, 0);
    return (0);
  }

  transparency_length = chunk_length;
  
  switch (image_color_type)
  {
    case PALETTE:

      /* tRNS chunk length cannot be larger than 256 or cannot be 
      ** larger than the size of the palette 
      */
      if (chunk_length > 256 || chunk_length > palette_size)
      {
        MSG_ERROR("tRNS chunk length too large", 0, 0, 0) ;
        return (1);
      }
  
      memcpy (transparency_value, chunk_data, transparency_length); 

      break;  

    case GRAYSCALE:

      if (chunk_length != 2)
      {
        MSG_ERROR("tRNS chunk length incorrect size for GrayScale ",
                                                                 0, 0, 0) ;
        return (1);
      }
    
      transparency_value[0] = chunk_data[0];
      transparency_value[1] = chunk_data[1];

      tmp_trans = chunk_data[0];
      tmp_trans = (tmp_trans << 8) | chunk_data[1];

      if (tmp_trans >= (uint32) ((1<<image_depth)))
      {
        MSG_ERROR("tRNS value too large for grayscale", 0, 0, 0);
        return (1);
      }

      break;

    case RGB:

      if (chunk_length != 6)
      {
        MSG_ERROR("tRNS chunk length incorrect size for RGB image ",
                                                                 0, 0, 0) ;
        return (1);
      }
    
      transparency_value[0] = chunk_data[0];
      transparency_value[1] = chunk_data[1];

      tmp_trans = chunk_data[0];
      tmp_trans = (tmp_trans << 8) | chunk_data[1];

      if (tmp_trans >= (uint32) ((1<<image_depth)))
      {
        MSG_ERROR("tRNS value too large for RGB image", 0, 0, 0);
        return (1);
      }

      transparency_value[2] = chunk_data[2];
      transparency_value[3] = chunk_data[3];

      tmp_trans = chunk_data[2];
      tmp_trans = (tmp_trans << 8) | chunk_data[3];

      if (tmp_trans >= (uint32) ((1<<image_depth)))
      {
        MSG_ERROR("tRNS value too large for RGB image", 0, 0, 0);
        return (1);
      }

      transparency_value[4] = chunk_data[4];
      transparency_value[5] = chunk_data[5];

      tmp_trans = chunk_data[4];
      tmp_trans = (tmp_trans << 8) | chunk_data[5];

      if (tmp_trans >= (uint32) ((1<<image_depth)))
      {
        MSG_ERROR("tRNS value too large for RGB image", 0, 0, 0);
        return (1);
      }

      break;

    case GRAYSCALE_ALPHA:
    case RGB_ALPHA:
      transparency_read = FALSE;
      MSG_ERROR("tRNS chunk for Grayscale and RGB Alpha Images Prohibited", 
                                                                   0, 0, 0);
      return(1);

    default:
      transparency_read = FALSE;
      MSG_ERROR("Invalid color type when processing tRNS chunk", 0, 0, 0);
      return(1);
  }

  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_png_background 

DESCRIPTION
  This function processes a bKGD chuck. This chunk defines the background
  color for the image. We only use this for Alpha channel processing.

DEPENDENCIES
  None

RETURN VALUE
  0 - background chunk processed successfully
  1 - background chunk processed failed    

SIDE EFFECTS
  None

===========================================================================*/
uint32 process_png_background (void)
{
  /* There can only be one palette for the image                           */
  if ( background_read ) 
  {
    MSG_ERROR("Duplicate bKGD chunk", 0, 0, 0);
    return (1);
  }

  background_read = TRUE;

  switch (image_color_type)
  {

    case GRAYSCALE:
    case GRAYSCALE_ALPHA:

      if (chunk_length != 2)
      {
        MSG_ERROR("bKGD chunk length incorrect size for Grayscale", 0, 0, 0);
        return (1);
      }

      /* assemble background color index for palette based images          */ 
      bkgd_color[0] = chunk_data[0];
      bkgd_color[0] = (bkgd_color[0] << 8) | chunk_data[1];

      if (bkgd_color[0] >= (uint32) ((1<<image_depth)))
      {
        MSG_ERROR("bKGD grayscale value too large", 0, 0, 0);
        return (1);
      }
      
      break;

    case PALETTE:

      if (chunk_length != 1)
      {
        MSG_ERROR("bKGD chunk length incorrect size for Palette", 0, 0, 0);
        return (1);
      }
      
      bkgd_color[0] = chunk_data[0] ;

      if (bkgd_color[0] >= (uint32) ((1<<image_depth)))
      {
        MSG_ERROR("bKGD palette index too large", 0, 0, 0);
        return (1);
      }
      
      break ;

    case RGB:
    case RGB_ALPHA:

      if (chunk_length != 6)
      {
        MSG_ERROR("bKGD chunk length incorrect size for RGB", 0, 0, 0);
        return (1);
      }
      
      /* assemble background color index for palette based images          */ 
      bkgd_color[0] = chunk_data[0];
      bkgd_color[0] = (bkgd_color[0] << 8) | chunk_data[1];

      if (bkgd_color[0] >= (uint32) ((1<<image_depth)))
      {
        MSG_ERROR("bKGD RGB color too large", 0, 0, 0);
        return (1);
      }
      
      bkgd_color[1] = chunk_data[2];
      bkgd_color[1] = (bkgd_color[1] << 8) | chunk_data[3];

      if (bkgd_color[1] >= (uint32) ((1<<image_depth)))
      {
        MSG_ERROR("bKGD RGB color too large", 0, 0, 0);
        return (1);
      }

      bkgd_color[2] = chunk_data[4];
      bkgd_color[2] = (bkgd_color[2] << 8) | chunk_data[5];

      if (bkgd_color[2] >= (uint32) ((1<<image_depth)))
      {
        MSG_ERROR("bKGD RGB color too large", 0, 0, 0);
        return (1);
      }

      break ;

    default:
      MSG_ERROR("bKGD chunk not supported for color type %d", 
                                                    image_color_type, 0, 0);
      break;
  }

  return (0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_png_text 

DESCRIPTION
  This function processes the data in a tEXt block.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_png_text ( void )
{
  png_cmx_data_type cmx_data;

  /* Output the text from the tEXt chunk in the image                      */
  if(png_cmx_cb != NULL) 
  {
    if(png_output_buffer == temp_buf) {
      cmx_data.buffer    = (uint8 *) chunk_data;
      cmx_data.num_bytes = chunk_length;
    } else {
      cmx_data.num_bytes = MIN(chunk_length, png_output_buffer_size);
      memcpy (png_output_buffer, chunk_data, cmx_data.num_bytes);
      cmx_data.buffer    = png_output_buffer;
    }
    png_cmx_cb(PNG_CMX_TEXT, png_cmx_handle, (void *) &cmx_data);
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION   process_png_image 

DESCRIPTION
  This function determines the chunk type read and calls the appropriate
  function based on that type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_png_image ( void * cb_data )
{
  png_state_type png_state_q2 = PNG_STATE_INIT;
  boolean exit_png_decoder = FALSE;
  uint32 index;
  uint32 chunk_crc;
  png_cmx_spec_type cmx_spec;
  const uint8 png_signature[PNG_SIGNATURE_SIZE] =
                                         { 137, 80, 78, 71, 13, 10, 26, 10 } ;

  /* if client ids don't match the exit from decode                        */
  if ( cb_data != png_id )
  {
#ifdef FEATURE_DCVS_APPS
  if (!png_decode_in_progress)
  {
    clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_PNG);
  }
#endif /* FEATURE_DCVS_APPS */
     return;
  }

  /* if abort flag set or LZ buffer not set then abort decoder             */
  if ( png_decoder_abort_flag || (png_set_buffer_flag != TRUE)) 
  {
     if(png_cmx_cb != NULL)
     {
       png_cmx_cb(PNG_CMX_ABORT, png_cmx_handle, NULL);
     }
     png_state = PNG_STATE_INIT;
     png_decoder_abort_flag = FALSE;
     png_cmx_cb = NULL;
     png_decode_in_progress = FALSE;
     MSG_HIGH ("PNG decoder was aborted",0,0,0) ;

#ifdef FEATURE_DCVS_APPS
    if (!png_decode_in_progress)
    {
      clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_PNG);
    }
#endif /* FEATURE_DCVS_APPS */

     return;
  }

  /* PNG decoder state machine                                             */
  do
  {
    switch ( png_state )
    {
      case PNG_STATE_INIT:

        png_decode_init();              /* initialize PNG variables        */

        /* setup to read PNG header data                                   */
        cmx_get_data.buffer    = chunk_data;
        cmx_get_data.num_bytes = 16;
        cmx_get_data.position = data_position;
        cmx_get_data.callback_ptr = 
                          (png_cmx_command_cb_func_type) process_png_image;
        cmx_get_data.client_data = (void *) png_id; 

        data_position += 16;
        png_state = PNG_GET_DATA;
        png_state_q1 = PNG_CHECK_HEADER;
        break;


      /* state for requesting more data from calling routine               */
      case PNG_GET_DATA:

        if(png_cmx_cb != NULL)
        {
          png_cmx_cb(PNG_CMX_GET_DATA, png_cmx_handle, &cmx_get_data);
        }
        png_state = png_state_q1;
        png_state_q1 = png_state_q2;
        exit_png_decoder = TRUE;
        break;

         
      /* process and check PNG file signature and initial header 
      ** information
      */    
      case PNG_CHECK_HEADER:

        if (memcmp (chunk_data, png_signature, PNG_SIGNATURE_SIZE) != 0) 
        {
          if(png_cmx_cb != NULL)
          {
            png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("PNG Signature Invalid:  Not a PNG file", 0, 0, 0) ;
          break;
        }
        
        /* jump to the IHDR chunk length and do byte swapping              */
        index = 8;
        chunk_length = chunk_data[index++];
        chunk_length = (chunk_length << 8) | chunk_data[index++];
        chunk_length = (chunk_length << 8) | chunk_data[index++];
        chunk_length = (chunk_length << 8) | chunk_data[index++];
        
        /* if length is not 13 then exit                                   */
        if (chunk_length != 13) 
        {
          if(png_cmx_cb != NULL)
          {
            png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("Invalid IHDR Chunk Length, %ld", chunk_length, 0, 0) ;
          break;
        }

        /* additional 20 bytes are for signature, chunk length, chunk type, 
        ** and CRC
        */
        file_byte_count += (chunk_length + 20);
        
        /* if running byte count is larger than png_file_size then exit    */
        if (file_byte_count > png_file_size)
        {
          if(png_cmx_cb != NULL)
          {
            png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("PNG file byte count exceeds file size", 0, 0, 0);
          break;
        }

        memcpy (chunk_type, &chunk_data[index], sizeof (chunk_type)) ; 

        /* If IHDR is not the first chunk of PNG file then exit decoder    */
        if (strncmp((char *) chunk_type, "IHDR", 4) != 0) 
        {
          if(png_cmx_cb != NULL)
          {
             png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("Invalid PNG File: Missing IHDR block",0,0,0) ;
          break;
        }

        /* set up to read IHDR chunk                                       */
        cmx_get_data.buffer    = chunk_data;
        cmx_get_data.num_bytes = chunk_length+4;
        cmx_get_data.position  = data_position;
        cmx_get_data.callback_ptr = 
                          (png_cmx_command_cb_func_type) process_png_image;
        cmx_get_data.client_data = (void *) png_id; 

        data_position += (chunk_length+4);
        png_state = PNG_GET_DATA;
        png_state_q1 = PNG_CRC_CHECK;
        png_state_q2 = PNG_PROCESS_HEADER;

        break;

      case PNG_PROCESS_HEADER:

        /* process header information                                      */
        if( process_png_header () )
        {
          if(png_cmx_cb != NULL)
          {
             png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("Error in Processing Header Chunk",0,0,0) ;
          break;
        }

        /* should always pass file specifications back to the caller       */
        if(png_cmx_cb != NULL)
        {
          cmx_spec.width       = image_width;
          cmx_spec.height      = image_height;
          cmx_spec.bit_depth   = image_depth;
          cmx_spec.color_type  = image_color_type;
          cmx_spec.compression = image_compression_method;
          cmx_spec.filter      = image_filter_method;
          cmx_spec.interlace   = image_interlace_method;
          png_cmx_cb(PNG_CMX_SPEC, png_cmx_handle, (void *) &cmx_spec);
        }

        /* if only getting specs for file then exit decoder                */
        if(png_image_specs_flag)
        {
          png_image_specs_flag = FALSE;
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          break;
        }

        png_state = GET_NEXT_CHUNK_TYPE;     /* set state for next chunk   */
        break;

      case PNG_CHUNK_TYPE:

        /* assemble chunk length.  Need to do byte swapping in process     */ 
        index = 0;
        chunk_length = chunk_data[index++];
        chunk_length = (chunk_length << 8) | chunk_data[index++];
        chunk_length = (chunk_length << 8) | chunk_data[index++];
        chunk_length = (chunk_length << 8) | chunk_data[index++];

        /* additional 12 bytes are for chunk length, chunk type, and CRC   */
        file_byte_count += (chunk_length + 12);
        
        /* if running byte count is larger than png_file_size then exit    */
        if (file_byte_count > png_file_size)
        {
          if(png_cmx_cb != NULL)
          {
            png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("PNG file byte count exceeds file size", 0, 0, 0);
          break;
        }

        memcpy (chunk_type, &chunk_data[index], sizeof (chunk_type)) ; 

        /* if chunk length too large and chunk type is other than IDAT
        ** then exit.   Additional 4 bytes for CRC within each chunk.
        */
        if ( ( (chunk_length+4) > (2*MAX_DATA_BYTES)) && 
                               (strncmp((char *) chunk_type, "IDAT", 4) != 0) ) 
        {
          if(png_cmx_cb != NULL)
          {
             png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("Chunk length too large, %ld", chunk_length, 0, 0) ;
          break;
        }

        /* start setting up to read data within chunk                      */
        cmx_get_data.buffer    = chunk_data;
        cmx_get_data.num_bytes = chunk_length+4; /*  4 bytes for chunk CRC */
        cmx_get_data.position  = data_position;
        cmx_get_data.callback_ptr = 
                          (png_cmx_command_cb_func_type) process_png_image;
        png_state = PNG_GET_DATA;
        png_state_q1 = PNG_CRC_CHECK;

        /* determine which chunk type processing and setup for that state  */ 
        if (strncmp((char *) chunk_type, "IHDR", 4) == 0) 
          png_state = PNG_HEADER_CHUNK; 
        else if (strncmp((char *) chunk_type, "PLTE", 4) == 0) 
          png_state_q2 = PNG_PALETTE_CHUNK; 
        else if (strncmp((char *) chunk_type, "tRNS", 4) == 0) 
          png_state_q2 = PNG_TRANSPARENCY_CHUNK; 
        else if (strncmp((char *) chunk_type, "bKGD", 4) == 0) 
          png_state_q2 = PNG_BACKGROUND_CHUNK; 
        else if (strncmp((char *) chunk_type, "tEXt", 4) == 0) 
          png_state_q2 = PNG_TEXT_CHUNK; 
        else if (strncmp((char *) chunk_type, "IDAT", 4) == 0) 
        {
          /* since IDAT block are typically the largest blocks then 
          ** setup to get as much data as we can given the limited 
          ** buffer size 
          */
          png_state_q1 = PNG_IMAGE_DATA_CHUNK;
          cmx_get_data.num_bytes = 2 * MAX_DATA_BYTES;
          if (cmx_get_data.num_bytes > png_file_size - data_position)
          {
            cmx_get_data.num_bytes = png_file_size - data_position;
          }
          idat_buffer[0].buf_ptr = chunk_data;
          idat_buffer[1].buf_ptr = chunk_data + MAX_DATA_BYTES;

          if (cmx_get_data.num_bytes > MAX_DATA_BYTES)
          {
             idat_buffer[0].buf_size = MAX_DATA_BYTES;
             idat_buffer[1].buf_size = cmx_get_data.num_bytes - MAX_DATA_BYTES;
          }
          else
          {
            idat_buffer[0].buf_size = cmx_get_data.num_bytes;
            idat_buffer[1].buf_size = 0;
          }
         
          break;
        } /* chunks not supported in this implementation                   */
        else if (strncmp((char *) chunk_type, "IEND", 4) == 0) 
          png_state_q2 = PNG_END_CHUNK; 
        else if ( (strncmp((char *) chunk_type, "cHRM", 4) == 0) ||
                  (strncmp((char *) chunk_type, "gAMA", 4) == 0) ||
                  (strncmp((char *) chunk_type, "iCCP", 4) == 0) ||
                  (strncmp((char *) chunk_type, "sBIT", 4) == 0) ||
                  (strncmp((char *) chunk_type, "sRGB", 4) == 0) ||
                  (strncmp((char *) chunk_type, "hIST", 4) == 0) ||
                  (strncmp((char *) chunk_type, "pHYs", 4) == 0) ||
                  (strncmp((char *) chunk_type, "sPLT", 4) == 0) ||
                  (strncmp((char *) chunk_type, "tIME", 4) == 0) ||
                  (strncmp((char *) chunk_type, "iTXt", 4) == 0) ||
                  (strncmp((char *) chunk_type, "zTXt", 4) == 0) ) 
        {
          png_state_q2 = GET_NEXT_CHUNK_TYPE;
          MSG_ERROR("Chunk type not supported", 0, 0, 0) ;
        }
        else
        { /* did not match any of the standard chunk types so skip it      */
          png_state_q2 = GET_NEXT_CHUNK_TYPE;
          MSG_ERROR("Private chunk type not processed",0,0,0) ;
        }

        data_position += (chunk_length+4);
        break;

      case PNG_CRC_CHECK:

        /* Calculate the CRC value from the data.                          */
        crc_value = 0xFFFFFFFF;
        crc_value = png_crc32 (crc_value, chunk_type, 4);
        crc_value = png_crc32 (crc_value, chunk_data, chunk_length);
        crc_value ^= 0xFFFFFFFF;

        /* end of chunk contains CRC so assemble by byte swapping data     */
        index = chunk_length;
        chunk_crc = chunk_data[index++];
        chunk_crc = (chunk_crc << 8) | chunk_data[index++];
        chunk_crc = (chunk_crc << 8) | chunk_data[index++];
        chunk_crc = (chunk_crc << 8) | chunk_data[index++];

        /* Ensure that the CRC value in the data stream is the same is
        ** that CRC value calculated from the data.
        */
        if (crc_value != chunk_crc) 
        {
          if(png_cmx_cb != NULL)
          {
            png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("Invalid CRC: %lx, %lx", crc_value, chunk_crc, 0) ;
          break;
        }
        png_state = png_state_q1;
        break;

      case PNG_HEADER_CHUNK: 
        
        /* if here then error.  PNG images can contain only one IHDR chunk */
        if(png_cmx_cb != NULL)
        {
          png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
        }
        png_state = PNG_STATE_INIT;
        exit_png_decoder = TRUE;
        png_decode_in_progress = FALSE;
        png_cmx_cb = NULL;
        MSG_ERROR("Invalid PNG File:  Multiple IHDR Chunks",0,0,0) ;
        break;
      
      case PNG_PALETTE_CHUNK:
    
        /* process the palette chunk of the image                          */
        if( process_png_palette () )
        {
          if(png_cmx_cb != NULL)
          {
             png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("Error in Processing Palette Chunk",0,0,0) ;
          break;
        }

        png_state = GET_NEXT_CHUNK_TYPE;     /* set state for next chunk   */
        break;
 
      case PNG_TRANSPARENCY_CHUNK:
    
        /* process the transparency chunk of the image                     */
        if( process_png_transparency () )
        {
          if(png_cmx_cb != NULL)
          {
             png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("Error in Processing Transparency Chunk",0,0,0) ;
          break;
        }

        png_state = GET_NEXT_CHUNK_TYPE;     /* set state for next chunk   */
        break;

      case PNG_BACKGROUND_CHUNK:
 
        if(process_png_background ())
        {
          if(png_cmx_cb != NULL)
          {
             png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("Error in Processing Background Chunk",0,0,0) ;
          break;
        }

        png_state = GET_NEXT_CHUNK_TYPE;     /* set state for next chunk   */
        break;
   
      case PNG_TEXT_CHUNK:
    
        /* process the text chunk of the image                             */
        process_png_text () ;

        png_state = GET_NEXT_CHUNK_TYPE;     /* set state for next chunk   */
        break;

      case PNG_IMAGE_DATA_CHUNK:

        /* keep running CRC for IDAT chunk                                 */
        crc_value = 0xFFFFFFFF;
        crc_value = png_crc32 (crc_value, chunk_type, 4);

        idat_buffer_select = 0;      /* use first half of IDAT data buffer */  
        idat_offset = 0;
        idat_buffer[0].clean = TRUE;
        idat_buffer[1].clean = TRUE;

        /* process first part of IDAT block                                */
        if( process_idat_header () )
        {
          if(png_cmx_cb != NULL)
          {
             png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
          }
          png_state = PNG_STATE_INIT;
          exit_png_decoder = TRUE;
          png_decode_in_progress = FALSE;
          png_cmx_cb = NULL;
          MSG_ERROR("Error at start of IDAT processing",0,0,0);
          break;
        }

        /* output header including image type, image size, and other image */
        /* specific parameters                                             */
        output_image_header();

        /* NOTE: This decoder only supports processing of non-interlaced
        ** images
        */  
        row_count = 0;
        png_state = PNG_IMAGE_DATA_DECODE;

        break;


      case PNG_IMAGE_DATA_DECODE:

        /* NOTE: This decoder only supports processing of non-interlaced
        ** images
        */  
        if ( row_count < image_height ) 
        {
          if( png_decode_noninterlaced_rows () )
          {
            if(png_cmx_cb != NULL)
            {
              png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
            }
            png_state = PNG_STATE_INIT;
            exit_png_decoder = TRUE;
            png_decode_in_progress = FALSE;
            png_cmx_cb = NULL;
            MSG_ERROR("Error in Processing Image Data",0,0,0);
            break;
          }

          row_count++;

          /* if running byte count is larger than png_file_size then exit  */
          if ((file_byte_count > png_file_size) || png_crc_failed)
          {
            if(png_cmx_cb != NULL)
            {
              png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
            }
            png_state = PNG_STATE_INIT;
            exit_png_decoder = TRUE;
            png_decode_in_progress = FALSE;
            png_cmx_cb = NULL;
            if (!png_crc_failed)
            {
              MSG_ERROR("PNG file byte count exceeds file size", 0, 0, 0);
            }            
            break;
          }

          if ( get_idat_data )
          {
            get_idat_data = FALSE;
            idat_buffer[1 - idat_buffer_select].clean = TRUE;
            cmx_get_data.callback_ptr = 
                        (png_cmx_command_cb_func_type) process_png_image;
            cmx_get_data.client_data = (void *) png_id; 
            if(png_cmx_cb != NULL)
            {
              png_cmx_cb(PNG_CMX_GET_DATA, png_cmx_handle, &cmx_get_data);
            }
          }
          else
          {
            cmx_continue.callback_ptr = 
                          (png_cmx_command_cb_func_type) process_png_image;
            cmx_continue.client_data = (void *) png_id; 
            if(png_cmx_cb != NULL)
            {
              png_cmx_cb(PNG_CMX_CONTINUE, png_cmx_handle, &cmx_continue);
            }
          }
          exit_png_decoder = TRUE;
        }
        else
        {
          /* completed the IDAT non-interlace processing. Do final  
          ** processing checks.
          */
          if( idat_end () || png_crc_failed)
          {
            if(png_cmx_cb != NULL)
            {
              png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
            }
            png_state = PNG_STATE_INIT;
            exit_png_decoder = TRUE;
            png_decode_in_progress = FALSE;
            png_cmx_cb = NULL;
            if (!png_crc_failed)
            {
              MSG_ERROR("Error in final processing checks",0,0,0);
            }            
            break;
          }

          png_state = GET_NEXT_CHUNK_TYPE;
        }
        break;

      case PNG_END_CHUNK:

        /* if here then PNG file decoding was successful and let calling
        ** routine know we are done
        */
        if(png_cmx_cb != NULL) 
        {
           png_cmx_cb(PNG_CMX_DONE, png_cmx_handle, NULL);
        }  
        png_state = PNG_STATE_INIT;
        exit_png_decoder = TRUE;
        png_decode_in_progress = FALSE;
        png_cmx_cb = NULL;
        MSG_MED("PNG decoding done",0,0,0);
        break;

      case GET_NEXT_CHUNK_TYPE:
 
        /* setup to read next chunk in PNG data stream                     */
        cmx_get_data.buffer    = chunk_data;
        cmx_get_data.num_bytes = 8;
        cmx_get_data.position  = data_position;
        cmx_get_data.callback_ptr = 
                          (png_cmx_command_cb_func_type) process_png_image;
        cmx_get_data.client_data = (void *) png_id; 
        data_position += 8;
        png_state = PNG_GET_DATA;
        png_state_q1 = PNG_CHUNK_TYPE;
        break;

      default:
        /* hopefully should never reach here                               */
        if(png_cmx_cb != NULL)
        {
           png_cmx_cb(PNG_CMX_FAILURE, png_cmx_handle, NULL);
        }
        png_state = PNG_STATE_INIT;
        exit_png_decoder = TRUE;
        png_decode_in_progress = FALSE;
        png_cmx_cb = NULL;
        MSG_HIGH ("Invalid PNG state",0,0,0) ;
        break;
    }

  } while (!exit_png_decoder);


#ifdef FEATURE_DCVS_APPS
  if (!png_decode_in_progress)
  {
    clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_PNG);
  }
#endif /* FEATURE_DCVS_APPS */
}

/* <EJECT> */
/*===========================================================================

FUNCTION png_decode 

DESCRIPTION
  This is the main for the PNG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void png_decode 
(
  void                              *handle,
  png_cmx_decode_cb_func_type       callback_ptr
)
{

  if ( (png_decode_in_progress == TRUE) && (png_decoder_abort_flag == FALSE) )
  {
     if(png_cmx_cb != NULL)
     {
       png_cmx_cb(PNG_CMX_ABORT, png_cmx_handle, NULL);
     }
  }

#ifdef FEATURE_DCVS_APPS
  clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_PNG);
#endif /* FEATURE_DCVS_APPS */

  png_id++;                                    /* client id for this run   */

  png_decode_in_progress = TRUE;               /* set decode in progress   */

  png_decoder_abort_flag = FALSE;              /* reset abort flag         */
  png_image_specs_flag = FALSE;                /* reset specs flag         */

  png_cmx_handle  = handle;
  png_cmx_cb      = callback_ptr;              /* callers callback ptr     */
  
  png_state  = PNG_STATE_INIT;                 /* set png to init state    */

  /* get file size for PNG image                                           */
  png_cmx_cb(PNG_CMX_GET_FILE_SIZE, png_cmx_handle, &png_file_size);

  MSG_HIGH("Starting decoding",0,0,0);

  process_png_image( (void *) png_id );        /* call png decoder         */
}
 
/* <EJECT> */
/*===========================================================================

FUNCTION png_decode_abort 

DESCRIPTION
  Abort function for the PNG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void png_decode_abort ()
{
  png_decoder_abort_flag = TRUE;            /* set abort flag              */
}

/* <EJECT> */
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
void png_get_image_specs
(
  void                              *handle,
  png_cmx_decode_cb_func_type       callback_ptr
)
{

  /* if a decode or get specs is in progress then abort it                 */
  if ( (png_decode_in_progress == TRUE) && (png_decoder_abort_flag == FALSE) )
  {
     if(png_cmx_cb != NULL)
     {
       png_cmx_cb(PNG_CMX_ABORT, png_cmx_handle, NULL);
     }
  }

  png_id++;                                     /* client id for this run  */

  png_decode_in_progress = TRUE;                /* set decode in progress  */

  png_cmx_handle  = handle;
  png_cmx_cb      = callback_ptr;

  png_decoder_abort_flag = FALSE;               /* reset abort flag         */
  png_image_specs_flag = TRUE;                  /* set flag to get specs    */

  png_state  = PNG_STATE_INIT;                  /* set PNG state to INIT    */

  /* get file size for PNG image                                            */
  png_cmx_cb(PNG_CMX_GET_FILE_SIZE, png_cmx_handle, &png_file_size);

  process_png_image( (void *) png_id );         /* call png decoder         */

}

/* <EJECT> */
/*===========================================================================

FUNCTION png_set_buf 

DESCRIPTION
  Function for setting the LZ window to buffer passed in by calling
  routine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 png_set_buf 
( 
  uint8 *client_buffer,	                    /* client buffer               */
  uint32 client_buffer_size,                /* size of client buffer       */
  uint8 *output_buffer,	                    /* output buffer               */
  uint32 output_buffer_size                 /* size of output buffer       */
)
{
  /* if client_buffer_size is less than PNG_WINDOW_SIZE then exit                      */
  if ((client_buffer_size < PNG_WINDOW_SIZE) ||
      ((output_buffer != NULL) &&
       (output_buffer_size < IMAGE_WIDTH_MAX*2*IMAGE_DEPTH_MAX/CHAR_BIT)))
  {
    png_set_buffer_flag = FALSE;            /* set buffer flag             */
    MSG_ERROR("Buffer allocated for LZ window is too small", 0, 0, 0);
    return (1);
  }

  lz_window = client_buffer;		    /* set LZ buffer to client buf */

  png_output_buffer_pend      = output_buffer;
  png_output_buffer_size_pend = output_buffer_size;

  png_set_buffer_flag = TRUE;               /* set buffer flag             */

  return (0);
}

#endif /* FEATURE_PNG_DECODER */

#ifndef CMX_PNGI_H
#define CMX_PNGI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               P N G   I N T E R N A L  H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the definitions necessary to interface to the
  PNG decoder.


Copyright(c) 2000, 2001 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/cmx_pngi.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/20/07   vma     Fixed design hole in which PNG contents with many very small
                   IDAT chunks would fail the decoder. Re-implemented how 
                   data are fetched to internal buffers. 
05/14/07   vma     Changed maximum height and width to 800. Fixed MAX_DATA_BYTES
                   to be computed based on maximum row width. 
03/08/01    sa     Created two new huffman table structures to save RAM.
                   Removed color_map_type structure as part of RAM savings.
02/14/01    sa     Added TrueColor (24-bit), TrueColor Alpha, and 16-bit
                   bit depth support.  Removed image scaling and support for
                   interlaced images.
06/13/00    sa     Changes to optimize code for speed 
05/30/00    sa     Clean up code and added comments
05/19/00    sa     Added new enums to png_state_type and "trans" variable to 
                   ColorMap struct for transparency support.  Also featurized
                   code. 
05/10/00    sa     Added PNG_INTERLACED_IMAGE to png_state_type  
05/09/00    sa     Initial version

===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Customer specific definitions           */
#include "cmx.h"                /* CMX specific definitions                */

#ifdef FEATURE_PNG_DECODER

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define PNG_WINDOW_SIZE         32768L  /* maximum LZ window size          */
#define PNG_LENGTH_SIZE         19      /* maximum number of code lengths  */
#define PNG_SIGNATURE_SIZE      8       /* signature size                  */
#define CHAR_BIT                8       /* size in bits of a character     */
#define IMAGE_WIDTH_MAX         800L    /* maximum image width supported   */ 
#define IMAGE_HEIGHT_MAX        800L    /* maximum image height supported  */
#define MAX_DATA_BYTES          (IMAGE_WIDTH_MAX * 4)   /* maximum length of working buffer*/
#define IMAGE_DEPTH_MAX         16      /* maximum image depth             */
#define BADCODE                 0xFFFFFFFL

/* Huffman decoder defines                                                 */
#define MAX_HUFFMAN_CODE_LENGTH   15
#define MAX_NUMBER_HUFFMAN_CODES  288
#define MAX_NUMBER_DISTANCE_CODES 34
typedef enum
{
  PNG_STATE_INIT,
  PNG_GET_DATA,
  PNG_CHECK_HEADER,
  PNG_PROCESS_HEADER,
  PNG_CHUNK_TYPE,
  PNG_CRC_CHECK,
  PNG_HEADER_CHUNK,
  PNG_PALETTE_CHUNK,
  PNG_TRANSPARENCY_CHUNK,
  PNG_BACKGROUND_CHUNK,
  PNG_TEXT_CHUNK,
  PNG_IMAGE_DATA_CHUNK,
  PNG_IMAGE_DATA_DECODE,
  PNG_IMAGE_DATA_DECODE_END,
  PNG_END_CHUNK,
  GET_NEXT_CHUNK_TYPE
} png_state_type;

enum PngColorType
{
  GRAYSCALE       = 0,
  RGB             = 2,
  PALETTE         = 3,
  GRAYSCALE_ALPHA = 4,
  RGB_ALPHA       = 6
};

/* Filter type defintions                                                  */
typedef enum
{
  FILTERNONE      = 0,
  FILTERSUB       = 1,
  FILTERUP        = 2,
  FILTERAVERAGE   = 3,
  FILTERPAETH     = 4
} filter_type;

typedef struct 
{
  int32 maxcode [MAX_HUFFMAN_CODE_LENGTH];       /* max Huffman code value */
  int32 mincode [MAX_HUFFMAN_CODE_LENGTH];       /* min Huffman code value */
  uint16 valptr [MAX_HUFFMAN_CODE_LENGTH];       /* index into "values"    */
  uint16 huff_values [MAX_NUMBER_HUFFMAN_CODES]; /* huffman values         */
} literal_huffman_decoder_data_type;

typedef struct 
{
  int32 maxcode [MAX_HUFFMAN_CODE_LENGTH];       /* max Huffman code value */
  int32 mincode [MAX_HUFFMAN_CODE_LENGTH];       /* min Huffman code value */
  uint16 valptr [MAX_HUFFMAN_CODE_LENGTH];       /* index into "values"    */
  uint16 huff_values [20];                       /* huffman values         */
} ht_huffman_decoder_data_type;

typedef struct 
{
  int32 maxcode [MAX_HUFFMAN_CODE_LENGTH];       /* max Huffman code value */
  int32 mincode [MAX_HUFFMAN_CODE_LENGTH];       /* min Huffman code value */
  uint16 valptr [MAX_HUFFMAN_CODE_LENGTH];       /* index into "values"    */
  uint16 huff_values [33];                       /* huffman values         */
} dist_huffman_decoder_data_type;

typedef struct 
{
  uint8* buf_ptr;
  uint32 buf_size;
  boolean clean;
} cmx_png_idat_buffer;

typedef enum
{
  UNCOMPRESSED        = 0,
  FIXEDHUFFMANCODES   = 1,
  DYNAMICHUFFMANCODES = 2
} compression_type;

#endif /* FEATURE_PNG_DECODER */

#endif  /* CMX_PNGI_H */

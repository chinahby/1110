/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("PNG Encoder")
*/

#ifndef PNGGLOBAL_H
#define PNGGLOBAL_H


/*===========================================================================

                    V I D E O    S E R V I C E S

                       P N G    L I B R A R Y


FILE
   pngglobal.h

DESCRIPTION
   For PNG encoder subsystem, this file holds the definitions, structures, 
   enums and return codes that are used throughout the program

  
EXTERNALIZED FUNCTIONS
   NONE.

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/pngglobal.h#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/05   kln     Fixed lint errors
07/01/04   mvd     Added "pnge" prefix to all structures, local, and extern
                   global variables.
05/12/04   mvd     Removed: DYNAMIC_CRC_TABLE, DEFLATE_DEBUG, FASTEST
                   MAX_W_BITS
01/23/04   mvd     Undefined DEBUG_STATS for strip file review.
12/11/03   mvd     Featurized entire file under FEATURE_PNG_ENCODER
                   Added params to internal encode struct: 
                   numOutputBytesWritten and maxOutputBytesToWrite so that 
                   we can manage output streams. 
11/07/03   pdo     Initial version

===========================================================================*/


#include <stdio.h>
#include "comdef.h"
#include "customer.h"
#include "fs.h" /* for fs_handle_type */



#if defined(FEATURE_PNG_ENCODER)

/*==========================================================================
                         BUILD SWITCHES   
==========================================================================*/


#define PNG_SCRATCH_IDAT_CHUNK_SIZE 0x2000

// define to see things like mem allocation etc ..
//#define DEBUG_STATS

// Max number of bytes of output to write for any one image
// 5 MB
#define PNGE_DEFAULT_MAX_NUM_OUTPUT_BYTES 0x500000



// must be an even number because Bayer format looks ahead by 1 row
// (operates in unit of 2 rows at a time when doing conversion)
#define DEFAULT_NUM_ROWS_PER_ITER 20


/* -------------- Deflate / Trees configuration params --------------- */
/* Maximum value for windowBits in deflateInit2 and inflateInit2.
 * WARNING: reducing MAX_WBITS makes minigzip unable to extract .gz files
 * created by gzip. (Files created by minigzip can still be extracted by
 * gzip.)
 */


/* Flags for png_set_filter() to say which filters to use.  The flags
 * are chosen so that they don't conflict with real filter types
 * below, in case they are supplied instead of the #defined constants.
 * These values should NOT be changed.
 */
#define PNG_NO_FILTERS     0x00
#define PNG_FILTER_NONE    0x08
#define PNG_FILTER_SUB     0x10
#define PNG_FILTER_UP      0x20
#define PNG_FILTER_AVG     0x40
#define PNG_FILTER_PAETH   0x80
#define PNG_ALL_FILTERS (PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_UP | \
                         PNG_FILTER_AVG | PNG_FILTER_PAETH)

#define ZLIB_CONFIG_MEM_LEVEL 8

// Determines which filter(s) to run on the data
// PAETH has been loosely determined to give good performace
// without running all of the others
#define PNG_DEFAULT_FILTER_SELECTION PNG_FILTER_PAETH



/*===========================================================================
                              TYPEDEFS 
===========================================================================*/


/*===========================================================================
                              MACROS 
===========================================================================*/



#define Trace(x)
#define Tracev(x)
#define Tracevv(x)
#define Tracec(c,x)
#define Tracecv(c,x)
#define PNG_DEFLATE_MSG_FATAL(c,x,y,z)


/*==========================================================================
                               CONSTANTS   
==========================================================================*/
/* color types.  Note that not all combinations are legal */

#define   PNG_COLOR_TYPE_GRAY        0
#define   PNG_COLOR_TYPE_RGB         2
#define   PNG_COLOR_TYPE_PALETTE     3
#define   PNG_COLOR_TYPE_RGB_ALPHA   4
#define   PNG_COLOR_TYPE_GRAY_ALPHA  6

#define   DEFAULT_INTERLACED         PNG_NON_INTERLACED
#define   PNG_NON_INTERLACED         0
#define   PNG_ADAM7_INTERLACED       1

#define   BIT_DEPTH_1                1
#define   BIT_DEPTH_2                2
#define   BIT_DEPTH_4                4
#define   BIT_DEPTH_8                8
#define   BIT_DEPTH_16               16
#define   DEFAULT_BIT_DEPTH          8

// Must be zero
#define   DEFAULT_COMPRESSION_TYPE   0
#define   DEFAULT_FILTER_METHOD      0

// "tRNS" in hex ASCII 
#define PNG_tRNS_CHUNK_NAME 0x74524E53
// "IEND" in hex ASCII 
#define PNG_IEND_CHUNK_NAME 0x49454E44
// "PLTE" in hex ASCII 
#define PNG_PLTE_CHUNK_NAME 0x504C5445
// "IHDR" in hex ASCII 
#define PNG_IHDR_CHUNK_NAME 0x49484452
// "IDAT" in hex ASCII 
#define PNG_IDAT_CHUNK_NAME 0x49444154

//
#define IHDR_CHUNK_SIZE  13
#define IEND_CHUNK_SIZE  0



/* Filter values (not flags) - used in pngwrite.c, pngwutil.c for now.
 * These defines should NOT be changed.
 */
#define PNG_FILTER_VALUE_NONE  0
#define PNG_FILTER_VALUE_SUB   1
#define PNG_FILTER_VALUE_UP    2
#define PNG_FILTER_VALUE_AVG   3
#define PNG_FILTER_VALUE_PAETH 4
#define PNG_FILTER_VALUE_LAST  5



#define PNG_MAXSUM (~((uint32) 0) >> 1)
#define PNG_HISHIFT 10
#define PNG_LOMASK ((uint32)0xffffL)
#define PNG_HIMASK ((uint32)(~PNG_LOMASK >> PNG_HISHIFT))



/*===========================================================================
                    EXTERNAL DEFINITIONS AND TYPES
===========================================================================*/


typedef enum 
{
   MEMORY_POINTER,
   FILE_POINTER
} pngeOutputFormatType;

typedef enum 
{
   PNGENC_SUCCESS = 0,
   PNGENC_MEM_ALLOCATE_FAILURE,
   PNGENC_WRITE_IHDR_FAILURE,
   PNGENC_PARAM_FORMAT_ERROR,
   PNGENC_ENCODE_ROWS_NOT_SUPPORTED,
   PNGENC_DEFLATE_DEALLOCATE_ERROR,
   PNGENC_UNKNOWN_ERROR
} pngeEncRetCodeType;



typedef struct
{

   // not ready to specify the "z_stream *" here - so just
   // don't tell the compiler about it
   void * pngDeflateStructPtr;

   // ---------------- Input/Output Struct Members ---------------------
   // mem pointer to entire input image 
   uint8 * inputPtr;

   // Pointer to specific row of the inputPtr that is currently being worked on
   // will be updated after every row encoded.
   uint8 * inputWorkingPtr;
   // Selecting either a file or memory pointer as the output type
   pngeOutputFormatType outputFormat;
   // If selected new PNG image output is written here
   fs_handle_type * outputFilePtr;

   // If selected new PNG image output is written here
   uint8 * outputPtr;
   // Pointer to specific row of the outputPtr that is currently being worked on
   // will be updated after every byte written.
   uint8 * outputWorkingPtr;

   // Num rows encoded and written to output already
   uint16 numRowsProcessed;

   // to hold row as it is being compressed
   uint8 * scratchRowPtr;
   uint32   scratchRowSize;

   // Defined as: = (( pngEncPtr->width * numComponents * pngEncPtr->bitDepth + 7) >> 3);
   uint32 bytesPerRow;

   // Total of all output bytes written (to file pointer or memory)
   // Includes all chunks, headers, CRCs etc ...
   uint32 numOutputBytesWritten;

   // This is the max num bytes available for the encoded PNG image.
   // Either file OR buffer writes will NOT exceed this number of bytes
   // (even at the expense of not completing the encoding)
   // An error flag is returned if this limit is hit while encoding.
   uint32 maxOutputBytesToWrite;

   // ---------------- PNG Standard Struct Members ---------------------
   // Width of target image in pixels
   uint32 width;
   // height of target image in pixels
   uint32 height;
   // sample precision
   uint8  bitDepth;
   // PNG Standard Color Type
   uint8 colorType;
   // PNG Standard Interlace Method
   uint8 interlaceMethod;

   // not sure about these yet
   //uint8 filterMethod;
   //uint8 compressionMethod;

   // Used when CRC is being summed over current chunk data
   uint32 crc;

   // --------------------- PNG Filter Support --------------------------
   uint8 filterSelectionMask;
   uint8 * currRowPtr;
   uint8 * prevRowPtr;
   uint8 * subFilterPtr;
   uint8 * upFilterPtr;
   uint8 * avgFilterPtr;
   uint8 * paethFilterPtr;
   uint8 *  filteredRowPtr;



} pngeEncodeType;




#endif /* FEATURE_PNG_ENCODER   */
#endif /* PNGGLOBAL_H           */

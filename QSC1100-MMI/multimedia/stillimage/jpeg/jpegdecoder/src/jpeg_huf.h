
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
               D E C O D E R   H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains the definitions necessary to interface to the
  JPEG decoder.
  

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegdecoder/main/latest/src/jpeg_huf.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/06   vma     Fixed all Lint errors
11/28/05   arv     Fixed Lint errors - Phase 1
11/07/05   pdo     Removed lint warnings
03/18/05   pdo     Initial version

===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef FEATURE_JPEG_DECODER
#include "jpegi.h"               /* jpeg decoder specific definitions     */

/* Huffman coding tables. */

typedef struct 
{
  /* These two fields directly represent the contents of a JPEG DHT marker */
  uint8 bits[17];		    /* bits[k] = # of symbols with codes of */
                            /* length k bits; bits[0] is unused */
  uint8 huffval[256];		/* The symbols, in order of incr code length */
  boolean read;             /* indicate huffman table has been read in   */

} jpeg_huff_tbl;

/* Derived data constructed for each Huffman table */

#define HUFF_LOOKAHEAD	8	/* # of bits of lookahead */

typedef struct {
  /* Basic tables: (element [0] of each array is unused) */
  int32 maxcode[18];		/* largest code of length k (-1 if none) */
  /* (maxcode[17] is a sentinel to ensure jpeg_huff_decode terminates) */
  uint32 offset[17];		/* huffval[] offset for codes of length k */
  /* valoffset[k] = huffval[] index of 1st symbol of code length k, less
   * the smallest code of length k; so given a code of length k, the
   * corresponding symbol is huffval[code + valoffset[k]]
   */

  /* Link to public Huffman table (needed only in jpeg_huff_decode) */
  jpeg_huff_tbl *pub;

  /* Lookahead tables: indexed by the next HUFF_LOOKAHEAD bits of
   * the input data stream.  If the next Huffman code is no more
   * than HUFF_LOOKAHEAD bits long, we can obtain its length and
   * the corresponding symbol directly from these tables.
   */
  uint32 look_nbits[1<<HUFF_LOOKAHEAD]; /* # bits, or 0 if too long */
  uint8  look_sym[1<<HUFF_LOOKAHEAD];   /* symbol, or unused */
} jpeg_derived_tbl;

typedef enum
{
  JPEGD_GRAY = 1,       /*enum value should be same as cmx_jpeg_sample_type values */
  JPEG_H1V1,
  JPEG_H2V1,
  JPEG_H1V2,
  JPEG_H2V2
} JPEG_subsample_type;

typedef struct 
{
  /* This structure contains information about Minimum Codeable Unit (MCU) */
  uint32 in_row;		     /* order of MCU in the MCU row    */
  uint32 in_col;             /* order of MCU in the MCU column */
  uint32 per_row;		     /* Number of MCUs in each row for this image */
  uint32 per_col;            /* Number of MCUs in the vertical column for this image   */
  uint32 blk_count;          /* Number of blocks in each MCU - depending on subsample  */
  JPEG_subsample_type subsample; //Phan 6/16
} MCU_info_type;


boolean jpeg_read_dht(uint8 bits[],uint8 huff_val[]);

JPEGD_STATUS_MSG jpeg_check_huff_tables(uint32);

/* Check Huff_tbl type */
int32 jpeg_huff_decode( jpeg_derived_tbl *huff_tbl);

boolean jpeg_dec_mcu_row ( int32 num_row, uint32 MCU_per_row);

boolean jpeg_decode_prog_scan(MCU_info_type mcu, uint32 num_comp, const uint32 compSel[]);

JPEGD_STATUS_MSG jpegd_initializeScan(void);

void jpeg_huff_init(void);

#endif




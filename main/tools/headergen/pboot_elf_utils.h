#ifndef __ELF_UTILS_H__
#define __ELF_UTILS_H__
/*=============================================================================
        P R O G R E S S I V E   B O O T   E L F   U T I L I T I E S

Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/pboot/pboot_elf_utils.h#9 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/31/06   rjain   Modified for Simple elf support
08/08/05   ih      Created
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include "miprogressive.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/
#define MAX_RAM_SIZE 0x4000000

#define PAGE_SIZE 0x1000

#ifndef ERROR
#define ERROR (-1)
#endif

#define ELF_HDR_SIZE 52
#define ELF_BLOCK_ALIGN 0x1000

/* Strings used to detect if a segment is pageable,
 ** not-pageable or not-used.
 */
#define RO              "RO"
#define RW              "RW"
#define ZI              "ZI"
#define PAGEABLE        "PAGED"
#define NOTPAGEABLE     "NOTPAGED"
#define SWAPABLE        "SWAPPED"
#define SWAP_POOL       "SWAP_POOL"
#define RESERVED        "RESERVED"
#define HASHTBL         "HASH"
#define SHARED          "SHARED"
#define NOTUSED         "NOTUSED"
#define BOOT_SEGMENT    "BOOT_SEGMENT"
#define CODE            "CODE"
#define L4BSP           "L4BSP"
#define POOL_INDEX_0    "INDEX_0"
#define POOL_INDEX_1    "INDEX_1"

#define DEBUG(fmt,x,y,z) if(debug_print) fprintf(stderr, fmt, x,y,z);

#define OPENFILE(file, fp, mode) \
      do { \
        if((fp = fopen(file, mode)) == NULL) { \
          fprintf(stderr, "Unable to open file '%s' in mode '%s'", file, mode); \
          perror(" "); \
          exit(-1); \
        } \
      } while(0)

#define FSEEK(fp, pos, mode) \
      do { \
        if( fseek(fp, pos, mode) < 0) { \
          fprintf(stderr, "Unable to seek file to position %d", pos); \
          perror(" "); \
          exit(-1); \
        } \
      } while(0)

#define MAX_SEGMENTS MI_PBT_MAX_SEGMENTS
#define PHDRS_COUNT  MI_PBT_MAX_SEGMENTS



typedef enum
{
  false = 0,
  true
} bool;

typedef uint32_t segment_flag_t;

typedef struct
{
  segment_flag_t flag;
  uint32_t       start_addr;
} segment_info_s;

typedef struct {
  int num_segments;
  segment_info_s segments[MAX_SEGMENTS];
} segments_t;

struct elf_file_info
{
   int        fd;
   Elf32_Ehdr elf_hdr;

   //index 0 is used for initial calculation only
   Elf32_Phdr phdr_table[PHDRS_COUNT+1];
};

/*===========================================================================
  FUNCTION  PROCESS_ELF_HEADER

  DESCRIPTION
  Read and verify the source ELF header.

  DEPENDENCIES
  None

  RETURN VALUE
  !=0 on error
  else 0

  SIDE EFFECTS
  None
  ===========================================================================*/
int process_elf_header
(
  FILE *src_elf,
  struct elf_file_info *src_file
);

/*===========================================================================
FUNCTION  FCPY

DESCRIPTION
    Copy len bytes from in_pos in file in_fp to out_pos in file out_fp

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int fcpy(FILE *in_fp, uint32_t in_pos, FILE *out_fp, uint32_t out_pos, size_t len);

/*===========================================================================
FUNCTION  FREADN

DESCRIPTION
    Read len bytes from in_pos of in_fp 1 PAGE at a time

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int freadn(char *buf, FILE *in_fp, uint32_t in_pos, size_t len);

/*===========================================================================
FUNCTION  FPAD

DESCRIPTION
    Pad len bytes starting from out_pos in out_fp

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int fpad(FILE *out_fp, uint32_t out_pos, size_t len);

/*===========================================================================
FUNCTION  VERIFY_ELF_HEADER

DESCRIPTION
    verify elf header

DEPENDENCIES
    None.

RETURN VALUE
    ERROR on error
    else 0

SIDE EFFECTS

===========================================================================*/
int verify_elf_header(Elf32_Ehdr *elf_hdr);

/*===========================================================================

FUNCTION    OFFSET_ALIGN

DESCRIPTION
    Helper to pboot_melf. Resolution is always a power of 2


DEPENDENCIES
    None.

RETURN VALUE
    errno or ERROR   On error
    0                Success

SIDE EFFECTS
    None.

===========================================================================*/
inline uint32_t offset_align(uint32_t position,uint32_t resolution);

/*===========================================================================
FUNCTION  READ_ELF_FILE

DESCRIPTION
    PRINTS THE ELF HEADER AND PROGRAM HEADERS INFO OF A GIVEN FILE STREAM

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/

int read_elf_file(FILE *src_elf);


/*===========================================================================
FUNCTION  READ_HEADER_FILE

DESCRIPTION
    Reads the Image Header and Prints it.

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int read_header_file(FILE *img_hd);

/*===========================================================================

FUNCTION    IS_PAGEABLE

DESCRIPTION

    This is a helper function to to determine if the segment represented by
    the string is pageable, not-pageable, not-used, RO, RW, ZI, AMSS or L4 segment.

DEPENDENCIES
    None.

RETURN VALUE


SIDE EFFECTS
    None.

===========================================================================*/
uint32 is_pageable(char *seg_info);

/*===========================================================================
FUNCTION  DEBUG_PRINT_SEGMENTS

DESCRIPTION
    Debug function to print out the segments parsed from the scl file

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void debug_print_segments(segments_t *seg);

/*===========================================================================

FUNCTION    PARSE_SCL_FILE

DESCRIPTION

    This function is used to read our amss scl file and determine
    whether a segment is pageable, non-pageable or not-used. It will also
    determine the starting address for every segment in our amss scl file.

DEPENDENCIES
    None.

RETURN VALUE
    errno or ERROR   On error
    0                Success

SIDE EFFECTS
    None.

===========================================================================*/
int parse_scl_file(FILE *fp, segments_t *segs);


#endif


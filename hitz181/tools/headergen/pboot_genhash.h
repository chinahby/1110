/*===========================================================================

                         S E C U R I T Y    S E R V I C E S
   
                          P R O G R E S S I V E   B O O T
                             
                              H E A D E R  F I L E

FILE:  pboot_genhash.h


DESCRIPTION

  This file is the header file for the hash verfication function for the 
  Progressive Boot.
  
 
REFERENCE: 

  Progressive Boot Software design Document
  

EXTERNALIZED FUNCTIONS : SECPROGBOOT_VERIFY_HASH
   

    Copyright © 2004 QUALCOMM Incorported. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE
$Header:
$DateTime:
$Author:

when       who             what, where, why
--------   ----            -------------------------------------------
1/12/05   John Alayari    Initial Creation

===========================================================================*/
#ifndef SECPROGBOOTGEN_H_
#define SECPROGBOOTGEN_H_

#include "filetool.h"

/*===========================================================================
                           
                             INCLUDE FILES

============================================================================*/

/*===========================================================================

                             CONSTANT DEFINITIONS

============================================================================*/


#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */

#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))

#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


#define MEMSET  memset
#define MEMCMP  memcmp
#define TRUE    1
#define FALSE   0
#define PROG_BOOT_DIGEST_SIZE 20
#define MAX_NUM_HASHES 13107  /* 256*1024/20 */
#define BLOCK_SIZE     4096 

/*===========================================================================

                             TYPE DEFINITIONS

============================================================================*/

typedef struct
{
  unsigned int   count[2];
  unsigned long  state[5];
  unsigned char  buffer[64];
  unsigned char  scratchbuffer[64];   
  
} SHA1_CTX;


/*===========================================================================

                             FUNCTION DEFINITIONS

============================================================================*/


/*=============================================================
FUNCTION SECPROGBOOT_GENERATE_HASHES()

DESCRIPTION
  This function is used to generate a set of SHA-1 hashes over
  the given data. The data is divided into block_size chunks.
 
PARAMETERS
  data_addr   - Pointer to the data to be hashed
  data_len    - Length of the data in bytes
  num_hashes  - number of hashes to generate
  table_ptr   - Double pointer to the table containing the
                resulting digests

RETURN VALUE
  Boolean - True means digests filled into the table pointed by 
            table_ptr.
            False means parameters are invalid.

DEPENDENCIES
  For a block size smaller than the data length 
  data_len % block_size = 0 is required. 
===============================================================*/
int secprogboot_generate_hashes
(
  uint8*      data_addr,
  uint32      data_len,
  uint32      num_hashes,
  uint8*      table_ptr
);



#endif

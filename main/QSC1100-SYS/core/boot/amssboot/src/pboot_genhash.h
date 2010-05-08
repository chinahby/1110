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

#include "comdef.h"
#include "target.h"

#ifdef SEC_BOOT
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

FUNCTION Prog_Boot_SHA_1_Hash()

DESCRIPTION
   
      Main interface to SHA-1 

PARAMETERS
     buff_ptr
     buff_size
     digest_ptr

RETURN VALUE
  None.

DEPENDENCIES
  None. 
===============================================================*/
void Prog_Boot_SHA_1_Hash(unsigned char *buff_ptr, 
                       unsigned buff_size, 
                       unsigned char *digest_ptr);
#endif
#endif

/*===========================================================================

                             S E C U R I T Y    S E R V I C E S

                              P R O G R E S S I V E   B O O T

                            G E N E R A T E   H A S H   T A B L E
                                     
                                       M O D U L E

FILE: pboot_genhash.c
 
DESCRIPTION

  The module imleplements the hash table function for the Progressive
  Boot.
  
 
REFERENCE: 

  Progressive Boot Software design Document
  

EXTERNALIZED FUNCTIONS : SECPROGBOOT_VERIFY_HASH

    
 Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

 
/*===========================================================================

                      EDIT HISTORY FOR FILE

 $Header:

 $DateTime: 

 $Author: 

 
when       who               what, where, why

--------   ---             -------------------------------------------------
1/12/05   John Alayari      Created the module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "pboot_genhash.h"
#include "filetool.h"

/*===========================================================================

                             INTERNAL FUNCTIONS

============================================================================*/

/*=============================================================

FUNCTION SHA1TRANSFORM()

DESCRIPTION
   Hash a single 512-bit block. This is the core of the algorithm

PARAMETERS
   state
   buffer

RETURN VALUE
  None.

DEPENDENCIES
  None. 
===============================================================*/
void SHA1Transform(unsigned long state[5], 
                   unsigned char buffer[64])
{
  unsigned long a, b, c, d, e;
  typedef union { unsigned long l[16];} 
                CHAR64LONG16;
                CHAR64LONG16* block;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - -- - - */

    block = (CHAR64LONG16*)buffer;
    
    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    
     //Round 0
    R0(a,b,c,d,e, 0); 
    R0(e,a,b,c,d, 1); 
    R0(d,e,a,b,c, 2); 
    R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); 
    R0(a,b,c,d,e, 5); 
    R0(e,a,b,c,d, 6); 
    R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); 
    R0(b,c,d,e,a, 9); 
    R0(a,b,c,d,e,10); 
    R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); 
    R0(c,d,e,a,b,13); 
    R0(b,c,d,e,a,14); 
    R0(a,b,c,d,e,15);

    R1(e,a,b,c,d,16); 
    R1(d,e,a,b,c,17); 
    R1(c,d,e,a,b,18); 
    R1(b,c,d,e,a,19);

    // Round 2
    R2(a,b,c,d,e,20); 
    R2(e,a,b,c,d,21); 
    R2(d,e,a,b,c,22); 
    R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); 
    R2(a,b,c,d,e,25); 
    R2(e,a,b,c,d,26); 
    R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); 
    R2(b,c,d,e,a,29); 
    R2(a,b,c,d,e,30); 
    R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); 
    R2(c,d,e,a,b,33); 
    R2(b,c,d,e,a,34); 
    R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); 
    R2(d,e,a,b,c,37); 
    R2(c,d,e,a,b,38); 
    R2(b,c,d,e,a,39);
    
    // Round 3
    R3(a,b,c,d,e,40); 
    R3(e,a,b,c,d,41); 
    R3(d,e,a,b,c,42); 
    R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); 
    R3(a,b,c,d,e,45); 
    R3(e,a,b,c,d,46); 
    R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); 
    R3(b,c,d,e,a,49); 
    R3(a,b,c,d,e,50); 
    R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); 
    R3(c,d,e,a,b,53); 
    R3(b,c,d,e,a,54); 
    R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); 
    R3(d,e,a,b,c,57); 
    R3(c,d,e,a,b,58); 
    R3(b,c,d,e,a,59);
    
    // Round 4
    R4(a,b,c,d,e,60); 
    R4(e,a,b,c,d,61); 
    R4(d,e,a,b,c,62); 
    R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); 
    R4(a,b,c,d,e,65); 
    R4(e,a,b,c,d,66); 
    R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); 
    R4(b,c,d,e,a,69); 
    R4(a,b,c,d,e,70); 
    R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); 
    R4(c,d,e,a,b,73); 
    R4(b,c,d,e,a,74); 
    R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); 
    R4(d,e,a,b,c,77); 
    R4(c,d,e,a,b,78); 
    R4(b,c,d,e,a,79);

    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    /* Wipe variables */
    a = b = c = d = e = 0;
}

/*=============================================================

FUNCTION SHA1INIT()

DESCRIPTION
   Initializes the context

PARAMETERS
   context

RETURN VALUE
  None.

DEPENDENCIES
  None. 
===============================================================*/
void SHA1Init(SHA1_CTX* context)
{
    
    memset(context, 0 , sizeof(SHA1_CTX));  

    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}

/*=============================================================

FUNCTION SHA1UPDATE()

DESCRIPTION
   Run the data through this

PARAMETERS
   context

RETURN VALUE
  None.

DEPENDENCIES
  None. 
===============================================================*/

void SHA1Update(SHA1_CTX* context, 
                unsigned char* data, 
                unsigned int len)
{
    unsigned int i, j;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    j = (context->count[0] >> 3) & 63;
    
    if ((context->count[0] += len << 3) < (len << 3)) 
        context->count[1]++;
    
    context->count[1] += (len >> 29);
    
    if ((j + len) > 63) 
    {
        memcpy(&context->buffer[j], data, (i = 64-j));
        
        SHA1Transform(context->state, context->buffer);
        
        for ( ; i + 63 < len; i += 64) 
        {
            memset(context->scratchbuffer, 0, 64);
            memcpy(context->scratchbuffer, &data[i], 64);
            SHA1Transform(context->state, context->scratchbuffer);
        }
        j = 0;
    }
    else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}

/*=============================================================

FUNCTION SHA1FINAL()

DESCRIPTION
   

PARAMETERS
   context

RETURN VALUE
  None.

DEPENDENCIES
  None. 
===============================================================*/
void SHA1Final(unsigned char *digest_ptr, SHA1_CTX* context)
{
  unsigned long i;
  unsigned char finalcount[8];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    for (i = 0; i < 8; i++) 
    {
      finalcount[i] = ((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }

      SHA1Update(context, (unsigned char *)"\200", 1);
    
    while ((context->count[0] & 504) != 448) 
    {
      SHA1Update(context, (unsigned char *)"\0", 1);
    }

      SHA1Update(context, finalcount, 8);  /* Should cause a SHA1Transform() */
    for (i = 0; i < 20; i++) 
    {
        *digest_ptr = (unsigned char)
                        ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
        digest_ptr++;
    }
    /* Wipe variables */
    i = 0;
    memset(context->buffer, 0, 64);
    memset(context->state, 0, 20);
    memset(context->count, 0, 8);
    memset(finalcount, 0, 8);
}

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
                       unsigned char *digest_ptr)
{
    SHA1_CTX context;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    SHA1Init(&context);
    SHA1Update(&context, buff_ptr, buff_size);
    SHA1Final(digest_ptr, &context);
}

/*===============================================================

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
)
{
  uint32  i;
  uint32 blk_size = BLOCK_SIZE;

    /* ===================================== */
    /* check number of parameters.           */
    /* num_hashes must be a positive         */
    /* number less than or equal (256 kb)/20 */
    /* ===================================== */

    if ( (data_addr  == NULL)          ||
         (data_len   <  BLOCK_SIZE)    ||
         (num_hashes >  MAX_NUM_HASHES)||
         (num_hashes <= 0)             ||
         (table_ptr  == NULL)
       ) 
    {
      return ERROR;
    };
   
    /* Calculate the hash for the entire segment if the segment is NOT-Paged.*/
    if (num_hashes == 1)
    {
      blk_size = data_len;
    }

    for (i = 0; i < num_hashes; i++)
    {
      /* Hash the input data*/
      Prog_Boot_SHA_1_Hash(data_addr,
                           blk_size,
                           table_ptr);
      data_len  -= blk_size;
      data_addr += blk_size;
      table_ptr = table_ptr + PROG_BOOT_DIGEST_SIZE;
      if (data_len < BLOCK_SIZE)
      {
        blk_size = data_len;
      }
    }
   

  return OK;

}


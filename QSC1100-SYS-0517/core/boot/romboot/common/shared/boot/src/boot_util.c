/*=============================================================================
                         BOOT UTILITY 

GENERAL DESCRIPTION
     
  This module defines miscellaneous utility function for boot software.
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  $
  $Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_util.c#4 $

when       who             what, where, why
--------   ---         --------------------------------------------------
8/30/05   John Alayari        Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_util.h"
 
/*=========================================================================
                            
                      EXTERNAL FUNCTION DEFINITIONS

=========================================================================*/

/*===========================================================================
FUNCTION bLoad8

DESCRIPTION
  
  This function takes a byte pointer and it returns the byte value in 
  in 32 bits. 
  
  This function ensures that read opeartions from memory are 32 bit 
  aligned.

DEPENDENCIES
  None

PARAMETERS
  p        - a Byte  Pointer.
  
RETURN VALUE
  Byte value in 32 bits.

SIDE EFFECTS
  None
===========================================================================*/
uint32 bLoad8 (const uint8* p)
{  
  /* align the address to 32 bits boundary */
  uint32 aligned_p = (uint32) p & (~0x00000003); 
  /* byte position in 32 bit address */  
  uint32 index = (uint32) p & 0x00000003;
  /* - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - */
  
  /* return the byte in 32 bits after masking other bytes with zero */  
 // return (( *((uint32*) aligned_p) & (0xFF << (index<<3))) >> (index<<3));

return (( *((uint32*) aligned_p) >> (index<<3)) &  0x000000FF ) ;


}
/*===========================================================================
FUNCTION bStore8

DESCRIPTION
  
  This function takes 32 bits input value and stores it at the address
  pointed by the input byte pointer.
  
  This function ensures that write opeation to memory are 32 bit aligned.

DEPENDENCIES
  None

PARAMETERS
  p        - Byte pointer.
  val      - 32 bit data to be stored in memory.
RETURN VALUE
 None.

SIDE EFFECTS
  None
===========================================================================*/
void bStor8 (uint8* p, uint32 val)
{  
  /* align the address to 32 bits boundary */
  uint32 aligned_p = (uint32) p & (~0x00000003);
  /* byte position in 32 bit address */  
  uint32 index = (uint32) p & 0x00000003;
  /* read the current data at 32 bit aligned address */
  uint32 current_value = *((uint32*) aligned_p);
  /* - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - */
  
  /* ------------------------------------------------------------------ */
  /* store the input value after shifting it in the proper position and */
  /* ORing it with current_value after the target byte in 32 bit        */
  /* current_value is cleared by masking it with zero                   */
  /* ------------------------------------------------------------------ */
  *((uint32*) aligned_p) =  ((current_value & ~(0xFF << (index<<3)))) | 
                             (val<< (index <<3));
}
/*===========================================================================
FUNCTION bByteCompare

DESCRIPTION
  
  This function compares two buffers byte by byte.

  This function replaces the memcmp utility in the C library and
  ensures that read opeartions from memory are 32 bit aligned.

  Note: 
  
  Same optimization as bByteCopy could be applied here. but
  this is not a high runner code in secure boot, optimization is 
  left for future.

DEPENDENCIES
  None

PARAMETERS
  value1_ptr  - Pointer to buffer 1.
  value1_ptr  - Pointer to buffer 2.
  byte_len    - Lenght to compare
RETURN VALUE
 None.

SIDE EFFECTS
  None
===========================================================================*/
uint32 bByteCompare
(
  const uint8*  value1_ptr,
  const uint8*  value2_ptr,
  const uint32  byte_len
)
{
  int   i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( i=0; i< (int) byte_len; i++ )
  {
    if ( bLoad8((uint8*) (value1_ptr++)) != 
         bLoad8((uint8*) (value2_ptr++)) 
       ) 
      return (uint32)(i+1);
  }

  return 0;
} /* bByteCompare */

/*===========================================================================
FUNCTION qmemcpy

DESCRIPTION
  
  This function copies a block of memory, handling overlap.
  This routine implements memcpy, and memmove. And ensures 
  that read and write operations from/to memory are 32 bit 
  aligned.

DEPENDENCIES
  None.

PARAMETERS
  source_ptr           - Pointer to source buffer.
  destination_ptr      - Pointer to destination buffer.
  len                  - Length to copy.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmemcpy( void* destination_ptr, const void* source_ptr, uint32 len )
{
  uint8       *dst_ptr = (uint8*) destination_ptr;
  const uint8 *src_ptr = (const uint8*) source_ptr;
  uint32      remainder;
  uint32      *src32_ptr;
  uint32      *dst32_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - */
  do
  {
    if (len == 0 || (uint32)dst_ptr == (uint32)src_ptr) 
        break;  /* nothing to do */

    if ((uint32)dst_ptr < (uint32)src_ptr) 
    {
        /* Copy forward */
        remainder = (uint32)src_ptr;   /* only need low bits */
        if ((remainder | (uint32)dst_ptr) & WORDMASK)
        {
            /* Try to align operands.  This cannot be done */
            /* unless the low bits match.                  */
            if ((remainder ^ (uint32)dst_ptr) & WORDMASK || len < WORDSIZE)
                remainder = len;
            else
               remainder = WORDSIZE - (remainder & WORDMASK);               
          
            len -= remainder;
            if (remainder)
                do
                   bStor8(dst_ptr++, bLoad8((uint8*)src_ptr++));
                while (--remainder);
        }
    
        /* Copy whole words. */
        
        /*  remainder = len div 4 */
        remainder = len >> 2;    
        if (remainder)
        {
           src32_ptr = (uint32*) src_ptr;
           dst32_ptr = (uint32*) dst_ptr;
           do 
              *dst32_ptr++ = *src32_ptr++;
           while (--remainder);
          
           /* update the byte pointers */
           src_ptr = (uint8*) src32_ptr;
           dst_ptr = (uint8*) dst32_ptr;
        }
 
        /* Then copy any trailing bytes. */
        remainder = len & WORDMASK;
        if (remainder)
            do 
               bStor8(dst_ptr++, bLoad8((uint8*)src_ptr++));
            while (--remainder);
    } 
    else 
    {
        /* Copy backwards.  Otherwise essentially the same. */
        /* Alignment works as before, except that it takes  */
        /* (remainder&WORDMASK) bytes to align, not         */ 
        /* WORDSIZE-(remainder&WORDMASK).                   */
        src_ptr += len;
        dst_ptr += len;
        remainder = (uint32)src_ptr;
        if ((remainder | (uint32)dst_ptr) & WORDMASK) 
        {
            if ((remainder ^ (uint32)dst_ptr) & WORDMASK || len <= WORDSIZE)
               remainder = len;
            else
               remainder &= WORDMASK;
            len -= remainder;
            if (remainder)
                do
                  bStor8(--dst_ptr, bLoad8((uint8*)--src_ptr));
                while (--remainder);
        }
        
        /* Copy whole words. */
        
        /*  remainder = len div 4 */
        remainder = len >> 2;    
        if (remainder)
        {
           src32_ptr = (uint32*) src_ptr;
           dst32_ptr = (uint32*) dst_ptr;
           do 
              *--dst32_ptr = *--src32_ptr;
           while (--remainder);
          
           /* update the byte pointers */
           src_ptr = (uint8*) src32_ptr;
           dst_ptr = (uint8*) dst32_ptr;
        }

        /* Then copy any trailing bytes. */
        remainder = len & WORDMASK;
        if (remainder)
            do 
               bStor8(--dst_ptr, bLoad8((uint8*)--src_ptr));
            while (--remainder);
    }
  } while (0);

}/* qmemcpy */


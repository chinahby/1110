#ifndef BOOT_UTIL_H_
#define BOOT_UTIL_H_
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
  $Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_util.h#4 $

when       who          what, where, why
--------   ---          --------------------------------------------------
8/30/05   John Alayari        Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
//#if defined(WIN32) || defined(_WIN32)
//    #include "secboot_i.h"
//#else
    #include "boot_comdef.h"
//#endif

#undef  WORDSIZE
#define WORDSIZE   sizeof(int)
#undef  WORDMASK
#define WORDMASK   (WORDSIZE - 1)

typedef unsigned long BLONG;



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
uint32 bLoad8 (const uint8* p);

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
void bStor8 (uint8* p, uint32 val);


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
uint32 bByteCompare(
  const uint8*  value1_ptr,
  const uint8*  value2_ptr,
  const uint32  byte_len
);


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
  src_ptr           - Pointer to source buffer.
  dst_ptr           - Pointer to destination buffer.
  len               - Length to copy.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmemcpy( void*, const void*, uint32 );

/*===========================================================================
FUNCTION qmemset

DESCRIPTION
  
  This function set a block of memory to agiven value.
 
  This routine implements memset and ensures that read 
  and write operations from/to memory are 32 bit aligned 
  This function sets Len bytes of destination to input value.

 
DEPENDENCIES
  None.

PARAMETERS
  dst_ptr         - Pointer to destination buffer.
  val             - Value  to be set
  len             - Lenght to set
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmemset (
  void* dst_ptr,
  uint32 val,
  uint32 len
);


/*===========================================================================

FUNCTION BOOT_GET_BIT_SET_POSITION

DESCRIPTION
  Given a unsigned integer that is a power of 2, this function returns
  the nth bit set.

DEPENDENCIES
  None

RETURN VALUE
  N'th bit set

SIDE EFFECTS
  None

===========================================================================*/
__inline uint32 boot_get_bit_set_position
(
  uint32 num
)
{
  uint32 shift = 0;

  __asm
  {
    CLZ shift, num
  }

  shift = 31 - shift;

  return shift;

} /* boot_get_bit_set_position() */

#endif /* __BOOT_UTIL_H */


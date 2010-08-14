/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Q D S P   M E M O R Y   A C C E S S 
                       U T I L I T I E S   M O D U L E
                     
GENERAL DESCRIPTION
  QDSP memory access utilities.

EXTERNALIZED FUNCTIONS
  qdsp_memcpy
    Copies a block of 16-bit values to or from the QDSP/ARM shared memory area.
  qdsp_byte_read
    Copies a buffer of bytes from the QDSP to the ARM memory area.
  qdsp_byte_write
    Copies a buffer of bytes from the ARM to the QDSP memory area.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2000 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/core/qdspservices/6k/ver1/qdsputils/main/latest/src/qdspmem.c#3 $ $DateTime: 2008/05/20 10:10:08 $ $Author: vparasar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/07   ymc     Declare source as constant in qdsp_memcpy and qdsp_memcpy32.
           ymc     Fixed MEMC memory read and write access failure.
           ymc     Updated Copyright.
11/29/04    sg     Added support for using memcpy for 32bit words since the ARM
                   compiler implements this using more efficient burst copies.
01/27/03    sg     Fixed bug in qdsp_memcpy32() where half of each 32-bit word
                   was lost because of the insufficient size of the
                   working buffer.
01/25/02    sm     Added qdsp_memcpy32(), qdsp_memset(), qdsp_memset32()
12/05/00    ro     Change lengths to uint32 from int.
11/06/00    ro     First version. Created from qdspfunc.c.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#include "assert.h"    /* Assert macro                           */
#include "msg.h"       /* Message logging/reporting services     */
#include "err.h"       /* Error logging macro                    */
#include "qdspext.h"

/* <EJECT> */
/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the QDSP Services module.

===========================================================================*/

/*===========================================================================

FUNCTION QDSP_MEMCPY

DESCRIPTION
  Copies a block of 16-bit values to or from the QDSP/ARM shared memory area.

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_memcpy (
  /* Destination address */
  uint16 *dest,
  /* Source address */
  const uint16 *src,
  /* Length of data to copy, in words */
  uint32 wlen,
  /* Boolean to swap when copying to destination */
  boolean swap
)
{
  uint16  tword;

  ASSERT( (dest != NULL) && (src != NULL) );
  ASSERT(!( ((uint32)dest & 1) && ((uint32)src & 1) ));
    
  /* Prepare for loops */
  wlen++;

  if(!( ((uint32)dest & 1) || ((uint32)src & 1) )) {
    /* Dest and Src are aligned */
    if(swap) {
      while (--wlen) {
        *dest++ = (*src << 8) | (*src >> 8);
        src++;
      }
    } else {
      while (--wlen) {
        *dest++ = *src++;
      }
    }
  } else {
    if((uint32)dest & 1) {
      /* Dest is misaligned */
      MSG_LOW("Odd alignment on dest pointer",0,0,0);
      if(swap) {
        while (--wlen) {
          tword = *src++;
          ((uint8*)dest)[0] = (uint8)(tword >> 8);
          ((uint8*)dest)[1] = (uint8)tword;
          dest++;
        }
      } else {
        while (--wlen) {
          tword = *src++;
          ((uint8*)dest)[0] = (uint8)tword;
          ((uint8*)dest)[1] = (uint8)(tword >> 8);
          dest++;
        }
      }
    } else {
      /* Src is misaligned */
      MSG_LOW("Odd alignment on src pointer",0,0,0);
      if(swap) {
        while (--wlen) {
          tword = (((uint16) ((const uint8*)src)[0]) << 8) |
                  ((uint16) ((const uint8*)src)[1]);
          *dest++ = tword;
          src++;
        }
      } else {
        while (--wlen) {
          tword = (((uint16) ((const uint8*)src)[1]) << 8) |
                  ((uint16) ((const uint8*)src)[0]);
          *dest++ = tword;
          src++;
        }
      }
    }
  }
} /* qdsp_memcpy */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_MEMCPY32

DESCRIPTION
  Copies a block of 32-bit values to or from the QDSP/ARM shared memory area.

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_memcpy32 (
  /* Destination address */
  void *dest,
  /* Source address */
  const void *src,
  /* Length of data to copy, in dwords */
  uint32 len,
  /* Boolean to swap when copying to destination */
  boolean swap
)
{
  uint32  tdword;
  const uint16 *src16;
  const uint32 *src32;
  uint32 *dst32;

  ASSERT( (dest != NULL) && (src != NULL) );
  ASSERT(!( ((uint32)dest & 3) && ((uint32)src & 3) ));
    
  /* Prepare for loops */
  len++;

  if(!( ((uint32)dest & 3) || ((uint32)src & 3) )) {
    /* Dest and Src are aligned */
    src32 = (const uint32*)src;
    dst32 = (uint32*)dest;

    if(swap) {
      src16 = (const uint16*)src;
      while (--len) {
        *dst32++ = ((uint32)((*src16 << 8) | (*src16 >> 8)) << 16) |
                   ( ( (*(src16+1) << 8) | (*(src16+1) >> 8) ) & 0xFFFF );
        src16+=2;
      }
    } else {
#ifdef FEATURE_QDSP_USES_NATIVE_MEMCPY 
      len--;
      memcpy(dst32, src32, len*4);
#else
      while (--len) {
        *dst32++ = *src32++;
      }
#endif
    }
  } else {
    if((uint32)dest & 3) {
      /* Dest is misaligned */
      MSG_LOW("Odd alignment on dest pointer",0,0,0);
      src32 = (const uint32*)src;

      if(swap) {
        while (--len) {
          tdword = *src32++;
          ((uint8*)dest)[0] = (uint8)(tdword >> 24);
          ((uint8*)dest)[1] = (uint8)(tdword >> 16);
          ((uint8*)dest)[2] = (uint8)(tdword >> 8);
          ((uint8*)dest)[3] = (uint8)tdword;
          dest = (void*)((uint32)dest + 4);
        }
      } else {
        while (--len) {
          tdword = *src32++;
          ((uint8*)dest)[0] = (uint8)tdword;
          ((uint8*)dest)[1] = (uint8)(tdword >> 8);
          ((uint8*)dest)[2] = (uint8)(tdword >> 16);
          ((uint8*)dest)[3] = (uint8)(tdword >> 24);
          dest = (void*)((uint32)dest + 4);
        }
      }
    } else {
      /* Src is misaligned */
      MSG_LOW("Odd alignment on src pointer",0,0,0);
      dst32 = (uint32*)dest;

      if(swap) {
        while (--len) {
          tdword = (((uint32) ((const uint8*)src)[0]) << 24) |
                   (((uint32) ((const uint8*)src)[1]) << 16) |
                   (((uint32) ((const uint8*)src)[2]) << 8)  |
                   ((uint32) ((const uint8*)src)[3]);
          *dst32++ = tdword;
          src = (const void*)((uint32)src + 4);
        }
      } else {
        while (--len) {
          tdword = (((uint32) ((const uint8*)src)[3]) << 24) |
                   (((uint32) ((const uint8*)src)[2]) << 16) |
                   (((uint32) ((const uint8*)src)[1]) << 8)  |
                   ((uint32) ((const uint8*)src)[0]);
          *dst32++ = tdword;
          src = (const void*)((uint32)src + 4);
        }
      }
    }
  }
} /* qdsp_memcpy */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_MEMSET

DESCRIPTION
  Initializes a block of 16-bit values in the QDSP memory area to a designated
  value.

DEPENDENCIES
  Applications must call qdsp_enable() before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_memset (
  /* Destination address */
  uint16 *dest,
  /* */
  uint16 val,
  /* Length of data to set, in words */
  uint32 wlen
)
{
  /* Prepare for loops */
  wlen++;

  while (--wlen) {
    *dest++ = val;
  }

} /* qdsp_memset */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_MEMSET32

DESCRIPTION
  Initializes a block of 32-bit values in the QDSP memory area to a designated
  value.

DEPENDENCIES
  Applications must call qdsp_enable() before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_memset32 (
  /* Destination address */
  uint32 *dest,
  /* */
  uint32 val,
  /* Length of data to set, in dwords */
  uint32 len
)
{
  /* Prepare for loops */
  len++;

  while (--len) {
    *dest++ = val;
  }

} /* qdsp_memset32 */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BYTE_READ32

DESCRIPTION
  Reads a buffer of bytes from the the QDSP/ARM shared memory area.
  Note: This routine is for DIAG support. 

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_byte_read32 (
  /* The start address of the QDSP memory block to read */
  void* src_ptr,
  /* The address of the ARM destination buffer */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
)
{
  uint8 *buffer = (uint8*)dst_ptr;
  uint32 *head;
  uint32 byte_cnt;
  volatile uint32 peek_data;


  if ((src_ptr==NULL) || (dst_ptr==NULL) || (byte_length==0))
  {
    return;
  }


  byte_cnt = (uint32)src_ptr & 3;
  head = (uint32*)((uint32)src_ptr >> 2 << 2);

  /* src not dword aligned */
  if (byte_cnt != 0)
  {
    peek_data = *head++;
    
    switch (byte_cnt)
    {
      case 1:
        *buffer++ = (uint8)(peek_data >> 8);
        if (--byte_length==0)
          { return; }

        /* fall thru */

      case 2:
        *buffer++ = (uint8)(peek_data >> 16);
        if (--byte_length==0)
          { return; }

        /* fall thru */

      case 3:
        *buffer++ = (uint8)(peek_data >> 24);
        if (--byte_length==0)
          { return; }

        break;

      default:
        MSG_FATAL("Program error.  read32 failed %d", byte_cnt, 0, 0);
        return;
    }
  }


  /* no. of dwords */
  byte_cnt = byte_length >> 2;

  if (byte_cnt > 0)
  {
    qdsp_memcpy32((void*)buffer, (void*)head, byte_cnt, FALSE);
  }


  head += byte_cnt;
  buffer += (byte_cnt << 2);


  /* remaining bytes */
  byte_cnt = byte_length & 3;
  
  if (byte_cnt != 0)
  {
    peek_data = *head;
    
    switch (byte_cnt)
    {
      case 3:
        *buffer++ = (uint8)(peek_data);
        peek_data >>= 8;
        /* fall thru */

      case 2:
        *buffer++ = (uint8)(peek_data);
        peek_data >>= 8;
        /* fall thru */

      case 1:
        *buffer++ = (uint8)(peek_data);
        peek_data >>= 8;
        break;

      default:
        MSG_FATAL("Program error.  read32 failed %d", byte_cnt, 0, 0);
        return;
    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BYTE_WRITE32

DESCRIPTION
  Writes a buffer of bytes to the QDSP/ARM shared memory area.
  Note: This routine is for DIAG support.  

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_byte_write32 (
  /* The address of the source buffer to write to the QDSP */
  void* src_ptr,
  /* The start address of the QDSP memory block to write */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
)
{
  uint8 *buffer = (uint8*)src_ptr;
  uint32 *head;
  uint32 byte_cnt;
  volatile uint32 poke_data;


  if ((src_ptr==NULL) || (dst_ptr==NULL) || (byte_length==0))
  {
    return;
  }


  byte_cnt = (uint32)dst_ptr & 3;
  head = (uint32*)((uint32)dst_ptr >> 2 << 2);

  
  /* dst_ptr not aligned */
  if (byte_cnt != 0)
  {
    poke_data = *head;

    switch (byte_cnt)
    {
      case 1:
        poke_data = (poke_data & 0xffff00ff) | ((uint32)(*buffer++) << 8);
        if (--byte_length==0)
        {
          *head = poke_data;
          return;
        }
        
        /* fall thru */

      case 2:
        poke_data = (poke_data & 0xff00ffff) | ((uint32)(*buffer++) << 16);
        if (--byte_length==0)
        {
          *head = poke_data;
          return;
        }
        
        /* fall thru */

      case 3:
        poke_data = (poke_data & 0x00ffffff) | ((uint32)(*buffer++) << 24);
        *head++ = poke_data;
        if (--byte_length==0)
          { return; }
        
        break;

      default:
        MSG_FATAL("Program error.  write32 failed %d", byte_cnt, 0, 0);
        return;
    }
  }


  /* no. of dwords */
  byte_cnt = byte_length >> 2;

  if (byte_cnt > 0)
  {
    qdsp_memcpy32((void*)head, (void*)buffer, byte_cnt, FALSE);
  }

  head += byte_cnt;
  buffer += (byte_cnt << 2);


  /* remaining bytes */
  byte_cnt = byte_length & 3;
 
  if (byte_cnt != 0)
  {
    poke_data = *head;

    switch (byte_cnt)
    {
      case 3:
        poke_data = (poke_data & 0xff00ffff) | ((uint32)buffer[2] << 16);
        
        /* fall thru */

      case 2:
        poke_data = (poke_data & 0xffff00ff) | ((uint32)buffer[1] << 8);
        
        /* fall thru */

      case 1:
        poke_data = (poke_data & 0xffffff00) | (uint32)buffer[0];
        *head = poke_data;
        
        break;

      default:
        MSG_FATAL("Program error.  write32 failed %d", byte_cnt, 0, 0);
        return;
    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BYTE_READ

DESCRIPTION
  Reads a buffer of bytes from the the QDSP/ARM shared memory area.
  Note: This routine is for DIAG support.  Use qdsp_block_write for normal
  block transfers.

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_byte_read16 (
  /* The start address of the QDSP memory block to read */
  void* src_ptr,
  /* The address of the ARM destination buffer */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
)
{
  /* Convert from void* */
  uint32                  address = (uint32)src_ptr; 
  byte*                   buffer  = (byte*)dst_ptr;
  
  uint32                  buf_index  = 0;
  boolean                 odd_length = FALSE;
  volatile uint16         peek_data;


  /* Check for odd address alignment */
  if ( address % 2 != 0 )
  {
    address--;
    peek_data = *(uint16*)address;
    buffer[buf_index++] = (byte)( (peek_data & 0xff00) >> 8 );
    
    address = address + 2;
    byte_length--;
  }
   
  /* Check for odd length */
  if ( byte_length % 2 != 0 )
  {
    odd_length = TRUE;
    byte_length--;
  }
  byte_length = byte_length / 2;
   
  /* Read 16-bit data from QDSP */
  qdsp_memcpy((uint16*)buffer, (uint16*)address, byte_length, FALSE);
  address += 2*byte_length;
  buf_index += 2*byte_length;
   
  /* Handle odd length */
  if ( odd_length == TRUE )
  {
    peek_data = *(uint16*)address;
    buffer[buf_index++] = (byte)(peek_data & 0x00ff );
  }         
   
} /* qdsp_byte_read16 */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BYTE_WRITE

DESCRIPTION
  Writes a buffer of bytes to the QDSP/ARM shared memory area.
  Note: This routine is for DIAG support.  Use qdsp_block_read for normal
  block transfers.

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_byte_write16 (
  /* The address of the source buffer to write to the QDSP */
  void* src_ptr,
  /* The start address of the QDSP memory block to write */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
)
{
  /* Convert from void* */
  uint32                  address = (uint32)dst_ptr; 
  byte*                   buffer  = (byte*)src_ptr;
  
  uint32                  buf_index  = 0;
  boolean                 odd_length = FALSE;
  volatile uint16         peek_data;

  /* Check for odd address alignment */
  if ( address % 2 != 0 )
  {
    address--;
    peek_data = *(uint16*)address;
    *(uint16*)address = ( ( buffer[buf_index++] << 8 ) | 
                          ( peek_data & 0x00ff ) );
   
    address = address + 2;
    byte_length--;
  }
   
  /* Check for odd length */
  if ( byte_length % 2 != 0 )
  {
    odd_length = TRUE;
    byte_length--;
  }
  byte_length = byte_length / 2;
   
  /* Write 16-bit data to QDSP */
  qdsp_memcpy((uint16*)address, (uint16*)(buffer+buf_index), byte_length, FALSE);
  address += 2*byte_length;
  buf_index += 2*byte_length;
   
  /* Handle odd length */
  if ( odd_length == TRUE )
  {
    peek_data = *(uint16*)address;
    *(uint16*)address = ( ( peek_data & 0xff00 ) |
                          ( buffer[buf_index++] ) );
  }         

} /* qdsp_byte_write16 */


/* <EJECT> */
// in bytes
const uint8 bank_word[QDSP_BANK_RAMI+1] = {2, 2, 4, 4};
// in bytes
// const uint32 bank_size[QDSP_BANK_RAMI+1] = {40960, 40960, 32768, 49152};
const uint32 bank_size[QDSP_BANK_RAMI+1] 
= {0x200000, 0x200000, 0x400000, 0x400000};

/*===========================================================================

FUNCTION QDSP_BYTE_READ

DESCRIPTION
  Reads a buffer of bytes from the the QDSP/ARM shared memory area.
  Note: This routine is for DIAG support.  Use qdsp_block_write for normal
  block transfers.

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_byte_read (
  /* The start address of the QDSP memory block to read */
  void* src_ptr,
  /* The address of the ARM destination buffer */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
)
{
  qdsp_bank_type bank;
  void *bank_start;
  void *bank_end;
  uint32 byte_cnt;


  if ((src_ptr==NULL) || (dst_ptr==NULL) || (byte_length==0))
  {
    return;
  }


  for (bank=QDSP_BANK_RAMA;bank<=QDSP_BANK_RAMI;bank++)
  {
    bank_start = (void*)qdsp_bank_address(bank, 0);
    bank_end = (void*)((uint32)bank_start + bank_size[bank]);

    if (bank_start >= bank_end)
    {
      continue;
    }


    if (src_ptr >= bank_end)
    {
      continue;
    }

    if (src_ptr < bank_start)
    {
      byte_cnt = (uint32)bank_start - (uint32)src_ptr;
      if (byte_cnt >= byte_length)
      { 
        return;
      }

      byte_length -= byte_cnt;
      dst_ptr = (void*)((uint32)dst_ptr + byte_cnt);
      src_ptr = bank_start;
    }

    byte_cnt = (uint32)bank_end - (uint32)src_ptr;
    if (byte_length < byte_cnt)
    {
      byte_cnt = byte_length;
    }

    switch (bank_word[bank])     
    {
      case 1:
        memcpy(dst_ptr, src_ptr, byte_cnt);
        break;
 
      case 2:
        qdsp_byte_read16(src_ptr, dst_ptr, byte_cnt);
        break;

      case 4:
        qdsp_byte_read32(src_ptr, dst_ptr, byte_cnt);
        break;

      default:
        MSG_ERROR("word size %d bank %d copy unsupported", \
                  bank_word[bank], bank, 0);
        return;
    }

    
    src_ptr = (void*)((uint32)src_ptr + byte_cnt);
    dst_ptr = (void*)((uint32)dst_ptr + byte_cnt);

    byte_length -= byte_cnt;
    if (byte_length == 0)
    {
      return;
    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BYTE_WRITE

DESCRIPTION
  Writes a buffer of bytes to the QDSP/ARM shared memory area.
  Note: This routine is for DIAG support.  Use qdsp_block_read for normal
  block transfers.

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_byte_write (
  /* The address of the source buffer to write to the QDSP */
  void* src_ptr,
  /* The start address of the QDSP memory block to write */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
)
{
  qdsp_bank_type bank;
  void *bank_start;
  void *bank_end;
  uint32 byte_cnt;


  if ((src_ptr==NULL) || (dst_ptr==NULL) || (byte_length==0))
  {
    return;
  }


  for (bank=QDSP_BANK_RAMA;bank<=QDSP_BANK_RAMI;bank++)
  {
    bank_start = (void*)qdsp_bank_address(bank, 0);
    bank_end = (void*)((uint32)bank_start + bank_size[bank]);

    if (bank_start >= bank_end)
    {
      continue;
    }


    if (dst_ptr >= bank_end)
    {
      continue;
    }

    if (dst_ptr < bank_start)
    {
      byte_cnt = (uint32)bank_start - (uint32)dst_ptr;
      if (byte_cnt >= byte_length)
      { 
        return;
      }

      byte_length -= byte_cnt;
      src_ptr = (void*)((uint32)src_ptr + byte_cnt);
      dst_ptr = bank_start;
    }

    byte_cnt = (uint32)bank_end - (uint32)dst_ptr;
    if (byte_length < byte_cnt)
    {
      byte_cnt = byte_length;
    }

    switch (bank_word[bank])     
    {
      case 1:
        memcpy(dst_ptr, src_ptr, byte_cnt);
        break;
 
      case 2:
        qdsp_byte_write16(src_ptr, dst_ptr, byte_cnt);
        break;

      case 4:
        qdsp_byte_write32(src_ptr, dst_ptr, byte_cnt);
        break;

      default:
        MSG_ERROR("word size %d bank %d copy unsupported", \
                  bank_word[bank], bank, 0);
        return;
    }

    
    src_ptr = (void*)((uint32)src_ptr + byte_cnt);
    dst_ptr = (void*)((uint32)dst_ptr + byte_cnt);

    byte_length -= byte_cnt;
    if (byte_length == 0)
    {
      return;
    }
  }
}

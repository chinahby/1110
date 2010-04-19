#ifndef QDSPMEM_H
#define QDSPMEM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Q D S P   M E M O R Y   A C C E S S 
                       U T I L I T I E S   M O D U L E
                     
GENERAL DESCRIPTION
  QDSP memory access utilities.

REFERENCES

Copyright (c) 2000-2007 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/qdspmem.h_v   1.1   25 Jan 2002 12:52:42   rmarkey  $
$Header: //source/qcom/qct/multimedia/core/qdspservices/6k/ver1/qdsputils/main/latest/src/qdspmem.h#3 $ $DateTime: 2008/05/20 10:10:08 $ $Author: vparasar $

when       who     what, where, why
--------   ---     ---------------------------------------------  
09/21/07   sud     Added protos for qdsp_byte_read16, qdsp_byte_write16, 
                   qdsp_byte_read32 and qdsp_byte_write32
08/27/07   ymc     Declared source as const in qdsp_memcpy and qdsp_memcpy32.
           ymc     Fixed MEMC read and write access failure.
           ymc     Updated Copyright.
01/25/02    sm     Added protos for qdsp_memcpy32(), qdsp_memset(),
                   qdsp_memset32()
12/05/00    ro     Change lengths to uint32 from int.
11/06/00    ro     First version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"    /* Definitions for byte, word, etc.      */

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS AND DECLARATIONS FOR MODULE

   This section contains definitions for constants, macros, types, variables
   and other items needed by this module.

===========================================================================*/

/*===========================================================================

FUNCTION QDSP_MEMCPY

DESCRIPTION
  Copies a block of 16-bit values to or from the QDSP/ARM shared memory area.

DEPENDENCIES
  Applications must call qdsp_enable() before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_memcpy (
  /* Destination address */
  uint16 *dest,
  /* Source address */
  const uint16 *src,
  /* Length of data to copy, in words */
  uint32 wlen,
  /* Boolean to swap when copying to destination */
  boolean swap
);

/*===========================================================================

FUNCTION QDSP_MEMCPY32

DESCRIPTION
  Copies a block of 32-bit values to or from the QDSP/ARM shared memory area.

DEPENDENCIES
  Applications must call qdsp_enable() before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_memcpy32 (
  /* Destination address */
  void *dest,
  /* Source address */
  const void *src,
  /* Length of data to copy, in dwords */
  uint32 len,
  /* Boolean to swap when copying to destination */
  boolean swap
);

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
extern void qdsp_memset (
  /* Destination address */
  uint16 *dest,
  /* */
  uint16 val,
  /* Length of data to set, in words */
  uint32 len
);

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
extern void qdsp_memset32 (
  /* Destination address */
  uint32 *dest,
  /* */
  uint32 val,
  /* Length of data to set, in words */
  uint32 len
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BYTE_READ

DESCRIPTION
  Reads a buffer of bytes from the the QDSP/ARM shared memory area.
  Note: This routine is for DIAG support.  Use qdsp_block_write for normal
  block transfers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_byte_read (
  /* The start address of the QDSP memory block to read */
  void* src_ptr,
  /* The address of the ARM destination buffer */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BYTE_WRITE

DESCRIPTION
  Writes a buffer of bytes to the QDSP/ARM shared memory area.
  Note: This routine is for DIAG support.  Use qdsp_block_read for normal
  block transfers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_byte_write (
  /* The address of the source buffer to write to the QDSP */
  void* src_ptr,
  /* The start address of the QDSP memory block to write */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BYTE_READ16

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
extern void qdsp_byte_read16 (
  /* The start address of the QDSP memory block to read */
  void* src_ptr,
  /* The address of the ARM destination buffer */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BYTE_WRITE16

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
extern void qdsp_byte_write16 (
  /* The address of the source buffer to write to the QDSP */
  void* src_ptr,
  /* The start address of the QDSP memory block to write */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
);

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
extern void qdsp_byte_read32 (
  /* The start address of the QDSP memory block to read */
  void* src_ptr,
  /* The address of the ARM destination buffer */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
);

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
extern void qdsp_byte_write32 (
  /* The address of the source buffer to write to the QDSP */
  void* src_ptr,
  /* The start address of the QDSP memory block to write */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
);

#endif /* QDSPMEM_H */


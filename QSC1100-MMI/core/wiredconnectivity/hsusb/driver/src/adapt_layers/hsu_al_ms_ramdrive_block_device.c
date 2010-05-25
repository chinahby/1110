/*==============================================================================

High Speed USB Ram Drive Block Device

GENERAL DESCRIPTION
Implementation of the ramdrive block device, which provides functions
that access a block device based on a buffer in RAM.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_ms_ramdrive_block_device.c#2 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_ms_ramdrive_block_device.h"
#include "hsu_common.h"
#include <string.h>

/*==============================================================================
Constants and Macros
==============================================================================*/
#define BLOCK_SIZE	        512

#define DEVICE_CAPACITY	        (2*1024) /* In Kilobytes */

#define KILOBYTE                1024
#define MEMORY_BUFFER_SIZE	(KILOBYTE * DEVICE_CAPACITY)

/* RamDrive will present itself to the host as having EMULATED_NUM_BLOCKS 
   blocks.
   Physically, the drive only has NUM_BLOCKS blocks.
   If the host writes/reads to/from "phantom" blocks (block num > NUM_BLOCKS),
   the command is ignored.
   The emulation works fine as long as the host is able to physically manage
   the FAT on the drive. Of-course, data written to "phantom" blocks cannot be
   correctly read back.
*/
#define NUM_BLOCKS          (MEMORY_BUFFER_SIZE / BLOCK_SIZE)
#define EMULATED_NUM_BLOCKS (2*1024*1024) /* yields emulated drive size of 1GB */

/*==============================================================================
Variables
==============================================================================*/
static uint8 mem_buffer[MEMORY_BUFFER_SIZE];


/*==============================================================================

FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_ms_ramdrive_block_device_read

DESCRIPTION
Read blocks.

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_ms_ramdrive_block_device_read 
(
  void*   context, /* not used */
  uint32  start_address, /* Block number to start from */
  uint16  num_blocks, /* Number of blocks to read */
  byte*   buff /* Buffer to read into */
)
{
  HSU_USE_PARAM(context);
  if ((start_address + num_blocks) > EMULATED_NUM_BLOCKS)
  {
    return FALSE ; 
  }

  if ((start_address + num_blocks) > NUM_BLOCKS)
  {
    /* ignore read from "phantom" blocks */
    return TRUE; 
  }

  memcpy(buff, &mem_buffer[start_address * BLOCK_SIZE],
    num_blocks * BLOCK_SIZE);
  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_ms_ramdrive_block_device_write

DESCRIPTION
Write blocks.

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_ms_ramdrive_block_device_write
(
  void*   context, /* not used */
  uint32  start_address, 
  uint16  num_blocks, 
  byte*   buff
)
{
  HSU_USE_PARAM(context);
  HSU_PARAM_COULD_BE_CONST(buff);

  if ((start_address + num_blocks) > EMULATED_NUM_BLOCKS)
  {
    return FALSE ; 
  }

  if ((start_address + num_blocks) > NUM_BLOCKS)
  {
    /* ignore read from "phantom" blocks */
    return TRUE; 
  }
  memcpy(&mem_buffer[start_address * BLOCK_SIZE], buff, 
    num_blocks * BLOCK_SIZE);

  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_ms_ramdrive_block_device_get_capacity

DESCRIPTION
Get the block size and number of blocks for the current media

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_ms_ramdrive_block_device_get_capacity
(
  void*   context, /* not used */
  uint32* block_size, /* out parameter */
  uint32* num_blocks /* out parameter */
)
{
  HSU_USE_PARAM(context);

  *block_size = BLOCK_SIZE;
  *num_blocks = EMULATED_NUM_BLOCKS;
  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_ms_ramdrive_block_device_lock

DESCRIPTION
Lock the drive so that applications cannot use it

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_ms_ramdrive_block_device_lock
(
  void* context /* not used */
)
{
  HSU_USE_PARAM(context);

  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_ms_ramdrive_block_device_unlock

DESCRIPTION
Unlock the sdcc drive so that applications can use it

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_ms_ramdrive_block_device_unlock
(
  void* context
)
{
  HSU_USE_PARAM(context);

  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_ms_ramdrive_block_device_media_is_present

DESCRIPTION
Check if media is present (always succeeds).

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_ms_ramdrive_block_device_media_is_present
(
  void* context,
  boolean was_present /* previous state of the media */
)
{
  HSU_USE_PARAM(context);
  HSU_USE_PARAM(was_present);

  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_ms_ramdrive_block_device_open

DESCRIPTION
Open the device, before we start using it.

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_ms_ramdrive_block_device_open
(
 void* context
)
{
  HSU_USE_PARAM(context);

  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_ms_ramdrive_block_device_close

DESCRIPTION
Close the device, once we've stopped using it.

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_ms_ramdrive_block_device_close
(
  void* context
)
{
  HSU_USE_PARAM(context);

  return TRUE;
}

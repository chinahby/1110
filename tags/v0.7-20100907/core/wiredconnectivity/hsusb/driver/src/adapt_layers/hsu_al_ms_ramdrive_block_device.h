#ifndef HSU_AL_MS_RAMDRIVE_BLOCK_DEVICE_H
#define HSU_AL_MS_RAMDRIVE_BLOCK_DEVICE_H

/*==============================================================================

High Speed USB Ram Drive Block Device

GENERAL DESCRIPTION
Defines externalized functions for the ramdrive block device.

EXTERNALIZED FUNCTIONS
hsu_al_ms_ramdrive_block_device_read
hsu_al_ms_ramdrive_block_device_write
hsu_al_ms_ramdrive_block_device_get_capacity
hsu_al_ms_ramdrive_block_device_lock
hsu_al_ms_ramdrive_block_device_unlock
hsu_al_ms_ramdrive_block_device_media_is_present
hsu_al_ms_ramdrive_block_device_open
hsu_al_ms_ramdrive_block_device_close

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_ms_ramdrive_block_device.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------

==============================================================================*/

#include "comdef.h"

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
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
);

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
);

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
);

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
);

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
);

/*===========================================================================

FUNCTION hsu_al_ms_ramdrive_block_device_media_is_present

DESCRIPTION
  Try to access the media.

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
);

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
);

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
);

#endif /* HSU_AL_MS_RAMDRIVE_BLOCK_DEVICE_H */

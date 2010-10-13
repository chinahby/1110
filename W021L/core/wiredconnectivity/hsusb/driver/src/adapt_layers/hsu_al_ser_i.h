#ifndef _HSU_AL_SER_I_H_
#define _HSU_AL_SER_I_H_

/*==============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
                            Internal Header File

GENERAL DESCRIPTION
Contains internal definitions needed for the serial functions' adaptation layer.

EXTERNALIZED FUNCTIONS
hsu_al_ser_i_alloc_buffer_for_read_write
hsu_al_ser_i_free_buffer_from_read_write

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/20/06 esh  First revision of file  
==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_ser_i_alloc_buffer_for_read_write

DESCRIPTION
Allocates from a given pool the requested amount of memory.

DEPENDENCIES
None

RETURN VALUE
Pointer to the requested memory. NULL if request cannot be fulfilled.

SIDE EFFECTS
None

===========================================================================*/
void* hsu_al_ser_i_alloc_buffer_for_read_write
(
  byte* mem_pool_ptr,                     /* Mem pool to alloc buffer from */
  juint32_t buff_size                     /* Amount of bytes requested     */
);

/*===========================================================================

FUNCTION hsu_al_ser_i_free_buffer_from_read_write

DESCRIPTION
Reclaims given buffer by returning it to its pool.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_i_free_buffer_from_read_write
(
  byte* mem_pool_ptr,                        /* Mem pool to return buffer  */
  void* buffer_ptr                           /* Pointer to returned buffer */
);

#endif /* _HSU_AL_SER_I_H_ */

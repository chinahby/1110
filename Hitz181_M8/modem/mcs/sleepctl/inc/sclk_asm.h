#ifndef SCLK_ASM_H
#define SCLK_ASM_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S C L K   M S M   R E G   W R I T E   R O U T I N E S

GENERAL DESCRIPTION
  This file contains prototypes for writing to MSM sclk-clocked registers

  The functions are self-caching functions written in thumb assembly,
  to prevent ADSP burst operation from stalling the ARM during these time-
  critical functions.

  All functions must be called from INTLOCK'd context.

EXTERNALIZED FUNCTIONS (Regional)
  sclk_reg_set_min
  sclk_reg_set_sync
  sclk_reg_set_min_sync

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005
              by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/sleepctl/main/latest/inc/sclk_asm.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   -------------------------------------------------------
08/15/2005   ajn   Initial version

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "comdef.h"


/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/* None */


/*==========================================================================

                    EXTERN VARIABLES and FUNCTIONS

This section contains variables that are accessible outside this module

==========================================================================*/



/*==========================================================================

FUNCTION SCLK_REG_SET_MIN

DESCRIPTION
  This function writes a value to a sclk-clocked MSM register.

  The value to be written to the register is compared with a value read
  from another MSM register, and possibly modified to be at least "min"
  greater than the value read.

DEPENDENCIES
  Must be called from INTLOCK'd context.

RETURN VALUE
  The (possibly modified) value written to MSM register

SIDE EFFECTS
  None

==========================================================================*/

uint32 sclk_reg_set_min
(
  /* Value to be written to "write_addr", after thresholding */
  uint32                    value,

  /* Minimum amount "value" must be above "read_addr" contents by */
  uint32                    minimum,

  /* Address (MSM register) to write the value to */
  uint32                    write_addr,

  /* Address (MSM register) to read the related value from */
  uint32                    read_addr
);



/*==========================================================================

FUNCTION SCLK_REG_SET_SYNC

DESCRIPTION
  This function writes a value to a sclk-clocked MSM register.

  After the write operation, the sync status register is read.  If the
  mask bit is not set, the write is repeated until it is set.

DEPENDENCIES
  Must be called from INTLOCK'd context.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void sclk_reg_set_sync
(
  /* Value to be written to "write_addr" */
  uint32                    value,

  /* Mask which must be present in the status register after the write */
  uint32                    mask,

  /* Address (MSM register) to write the value to */
  uint32                    write_addr,

  /* Address (MSM register) of the sync status register for the write */
  uint32                    sync_addr
);



/*==========================================================================

FUNCTION SCLK_REG_SET_MIN_SYNC

DESCRIPTION
  This function writes a value to a sclk-clocked MSM register.

  The value to be written to the register is compared with a value read
  from another MSM register, and possibly modified to be at least "min"
  greater than the value read.

  After the write operation, the sync status register is read.  If the
  mask bit is not set, the write is repeated until it is set.

DEPENDENCIES
  Must be called from INTLOCK'd context.

RETURN VALUE
  The (possibly modified) value written to MSM register

SIDE EFFECTS
  None

==========================================================================*/

uint32 sclk_reg_set_min_sync
(
  /* Value to be written to "write_addr", after thresholding */
  uint32                    value,

  /* Minimum amount "value" must be above "read_addr" contents by */
  uint32                    minimum,

  /* Mask which must be present in the status register after the write */
  uint32                    mask,

  /* Address (MSM register) to write the value to */
  uint32                    write_addr,

  /* Address (MSM register) to read the related value from */
  uint32                    read_addr,

  /* Address (MSM register) of the sync status register for the write */
  uint32                    sync_addr
);


#endif /* SCLK_ASM_H */


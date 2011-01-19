#ifndef BOOTHW_6800_H
#define BOOTHW_6800_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B O O T    H A R D W A R E    M S M 6 5 0 0    S P E C I F I C
                I N I T I A L I Z A T I O N    H E A D E R

GENERAL DESCRIPTION
  This header file contains type and macro definitions specific for the
  MSM6500. These definitions can be used by other modules in DMSS.

Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msm6800/drivers/boot/2.1/boothw_6800.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/05   rp      Removed boothw_boot_mode_enum_type and 
                   boot_hw_get_bootmode and moved them to 
                   boothw.c
07/29/04   jwh     Added boot_hw_get_max_ram_bandwidth() prototype.
04/15/04   AP      Ported for MSM6550 from MSM6500.
===========================================================================*/

#include "target.h"
#include "comdef.h"

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/* --------------------------------------------------------------------------
   Watchdog timer circuit reset macro.
-------------------------------------------------------------------------- */

#define BOOTHW_KICK_WATCHDOG()                                               \
            HWIO_OUT(WDOG_RESET, HWIO_FMSK(WDOG_RESET,WATCH_DOG));

/* --------------------------------------------------------------------------
   MEM_CTLR_SEL_SEL register configuration parameters.
-------------------------------------------------------------------------- */

/* XMEMC and MPMC memory controllers. */
#define EBI_MEMORY_CONTROLLER_XMEMC                 0
#define EBI_MEMORY_CONTROLLER_MPMC                  1

/* --------------------------------------------------------------------------
   Structure of registers that need to be programmed when the memory bus
   clock frequency is changed.
-------------------------------------------------------------------------- */

typedef struct
{
  uint32   ebi1_cs0_cfg0;
  uint32   ebi1_cs0_cfg1;
  uint32   ebi1_cs1_cfg0;
  uint32   ebi1_cs1_cfg1;
  uint32   ebi1_cs2_cfg0;
  uint32   ebi1_cs2_cfg1;
  uint32   ebi1_cs3_cfg0;
  uint32   ebi1_cs3_cfg1;
  uint32   ebi2_cs0_cfg0;
  uint32   ebi2_cs1_cfg0;
  uint32   ebi2_cs2_cfg0;
  uint32   ebi2_cs3_cfg0;
  uint32   ebi2_lcd_cfg0;
  uint32   ebi2_lcd_cfg1;
} boothw_ebi_regs_type;
/*===========================================================================

  FUNCTION BOOT_HW_COMPUTE_EBI_REGS

  DESCRIPTION
    Computes the values of the EBI registers that include fields which are
    dependent on the memory bus clock frequency.

  PARAMETERS
    Pointer to EBI register structure.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void boot_hw_compute_ebi_regs
(
  boothw_ebi_regs_type*   reg_struct_ptr,
  uint32                  bus_freq_in_khz
);

/*===========================================================================

  FUNCTION BOOT_HW_PROGRAM_EBI_REGS

  DESCRIPTION
    Program EBI registers with values provided by caller.

  PARAMETERS
    Pointer to structure containing EBI register values.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    EBI register programming is updated.

===========================================================================*/
void boot_hw_program_ebi_regs
(
  boothw_ebi_regs_type*   reg_struct_ptr
);
/*===========================================================================

  FUNCTION BOOT_HW_GET_MAX_RAM_BANDWIDTH()

  DESCRIPTION
   This function calculates the bandwidth in bytes per second for the
   read write memory.Note that the bandwidth calculated is the maximum
   bandwidth calculated based on the time taken for a cache line fill and a
   100% bus utilization.
   The real bandwidth will be much lower than the value calculated here and
   will depend on many hardware conditions other than just cache fill time.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    The bandwidth in bytes per second for the read write memory.

  SIDE EFFECTS

===========================================================================*/
uint32 boot_hw_get_max_ram_bandwidth(void );


/*===========================================================================

  FUNCTION boot_hw_target_init()

  DESCRIPTION
    Initializes MSM specific hardware that cannot be put inot
	common boot code.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The MSM chip is initialized.
===========================================================================*/
extern void boot_hw_target_init(void);



#endif /*BOOTHW_6800_H*/

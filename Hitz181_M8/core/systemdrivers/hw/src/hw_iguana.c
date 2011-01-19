/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           H A R D W A R E   U T I L I T I E S   F O R   I G U A N A

GENERAL DESCRIPTION
  This module contains hardware specific utilites.

EXTERNALIZED FUNCTIONS
  hw_ig_ui_client_is_wombat

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2004      by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/hw/hw_iguana.c#1 $
  $DateTime: 2007/05/17 13:25:59 $
  $Author: hvo $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04-11-30   tjw     Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Definition for target specific defs  */
#include "comdef.h"             /* Definitions for byte, word, etc.     */

/*!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!

As switching between UI clients is only a temporary requirement I have not added
the prototype to a header file - just cut and paste these two lines to wherever it is used

!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!*/
#ifdef FEATURE_IG_UI_CLIENT_SELECT
#include "hw.h"
#ifdef L4LINUX_6800_KERNEL_PORT
#else
#include "cpld_6550.h"          /* to read the SURF DIP switch */
#endif
#ifdef COMPILING_IGUANA
#include <driver.h>
#include <iguana/hardware.h>
#include <iguana/memsection.h>
#endif
#ifdef L4LINUX_6800_KERNEL_PORT
#error code not present
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION HW_IG_UI_CLIENT_IS_WOMBAT

DESCRIPTION
  This function returns the selected keypad/display client.
  For the SURF it reads dipswitch S6-8 (the AUTO_EN bit) and returns
  state to the caller: left AMSS, right Wombat
  For the FFA it returns the client selected at build time by defining
  (or not) FEATURE_IG_UI_CLIENT_IS_WOMBAT.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean:
    TRUE  Client is Wombat
    FALSE Client is AMSS

SIDE EFFECTS
  None

===========================================================================*/
boolean hw_ig_ui_client_is_wombat(void)
{
#ifdef L4LINUX_6800_KERNEL_PORT
#else
  #ifdef T_SURF
  static boolean dipSwitch;
  static boolean read_dipSwitch_once = FALSE;

  /* Just read the hardware once. Keep a local
     copy for future calls to this function. */
  if (read_dipSwitch_once == FALSE)
  {
    #ifdef COMPILING_IGUANA
    uintptr_t                      FPGA_base_addr;
    uintptr_t                      FPGA_memory_section;
    #ifdef L4LINUX_TODO
#error code not present
    #endif /* L4LINUX_TODO */
    /* These are required to use the AMSS macros (which use flat memory addresses)
       with the FPGA mapped through virtual memory */
    #define FPGA_PHYS_BASE 0x30000000
    #undef  FPGA_BASE
    #define FPGA_BASE      (FPGA_base_addr)
    FPGA_memory_section = memsection_create(0x40000, &FPGA_base_addr);
    hardware_back_memsection(FPGA_memory_section, FPGA_PHYS_BASE, 1);
    #endif /* COMPILING_IGUANA */

    read_dipSwitch_once = TRUE;
    dipSwitch = FPGA_IS_EN(FPGA_CONFIG1_RH, AUTO_EN_MASK);
  }

  return(dipSwitch);

  #else /* FFA - select at build time */

    #ifdef FEATURE_IG_UI_CLIENT_IS_WOMBAT
      return(TRUE);
    #else
      return(FALSE);
    #endif

  #endif
#endif /*HUGO_PORT*/
  return(FALSE);
} /* end of hw_ig_ui_client_is_wombat */
#endif /* defined(FEATURE_IG_UI_CLIENT_SELECT)  */

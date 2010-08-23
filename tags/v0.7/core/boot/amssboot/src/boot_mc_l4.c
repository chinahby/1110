/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  BOOT MAIN CONTROL 

 GENERAL DESCRIPTION
   This file contains the necessary functionality which needs to be performed
   in AMSS boot process. Please be aware that the order of functions is 
   very important.


 INITIALIZATION AND SEQUENCING REQUIREMENTS
   Routines within this module automatically obtain control of the processor
   following a boot sequence. Control may also return to this module 
   if main() exits unexpectedly, if an unexpected hardware or software 
   interrupt occurs, or if a fatal error occurs.

 Copyright (c) 2004     by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*============================================================================

                       EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $PVCSPath:  $
  $Header: //depot/asic/msm6800/drivers/boot/2.1/boot_mc_l4.c#1 $

 when      who     what, where, why
============================================================================
05/06/05   tbg     Added support for reset detection and logging.
12/08/04   tkuo    Removed boot_hw_tlmm_init.
10/19/04   tkuo    Created for MSM6275
============================================================================*/

/*============================================================================

                             MODULE IMPORTS

============================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_USES_CLK_BUSY_WAIT_NOPS
#include "clk.h"
#endif

/*============================================================================

                             MODULE EXPORTS

============================================================================*/
/*===========================================================================

FUNCTION  BOOT_MAIN_CTL

DESCRIPTION
   The AMSS BOOT Main Controller performs the following functions:
        
      - Sets up the hardware with the configuration required.
        for AMSS execution.
      - Perform RAM initialization.
      - Reads off-chip configuration data from IRAM into external RAM. 

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE-means succeed. FLASE-means boot failed. 
SIDE EFFECTS
  None

===========================================================================*/
void boot_main_ctl(void) 
{
#ifdef FEATURE_USES_CLK_BUSY_WAIT_NOPS
/* Initialize the  uses_clk_buys_wait_nops to indicate that now it is
   OK to use NOP loops for clk_busy_wait();
*/   
   uses_clk_buys_wait_nops  = CLK_BUSY_WAIT_NOPS_INITIALIZED_VAL;
#endif 
 return;
}/* boot_main_ctl() */

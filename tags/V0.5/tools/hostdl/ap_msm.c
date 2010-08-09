/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      MSM6250 Initialization (for Flashprg only)

GENERAL DESCRIPTION
  This module contains startup/memory initialization required to
  execute flashprg standalone.  Some of these functions are NOT
  required for standard operation.
  
  This file will need to be customized for each MSM architecture.

EXTERNALIZED FUNCTIONS
  asb_and_msm_init
    This function initializes the bus sizer and MSM registers for boot
    operation.  It is called from the assembly language wrapper code.
  hw_id_and_init
    This function identifies which MSM is in use, and sets up some
    hardware and hardware register pointers accordingly.  It is called
    from main_c.
  memory_init
    This function initializes data memory (zero init) and initialzed 
    data for C code.
    It is called from the assembly language wrapper code.
  msm_reset
    This function resets the MSM and is hardware specific

  Copyright (c) 2008 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/tools/hostdl/HOSTDL.14.00/ap_msm.c#1 $ $DateTime: 2008/01/21 21:01:41 $ $Author: opham $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/MSM6275/ap_msm.c#7 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
01/03/08   op      NOR flash driver unification 
============================================================================*/

//--------------------------------------------------------------------------
// Include Files
//--------------------------------------------------------------------------

#include "ap_startup.h"
#include "ap_armprg.h"
#include "ap_msm.h"
#include "ap_flash.h"


//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Extern Definitions
//--------------------------------------------------------------------------

// These symbols are created by the ARM linker and are referenced by using 
// these special characters ($$).                      
/*lint -e27 -e10 -e19 -save*/
extern byte *Image$$APP_RAM$$ZI$$Base;
extern byte *Image$$APP_RAM$$ZI$$Limit;
/*lint -restore*/


//--------------------------------------------------------------------------
// Global Data Declarations
//--------------------------------------------------------------------------

__global_reg (1)
     dword *dst32;              /*lint !e102 !e745 */
__global_reg (2)
     dword *src32;



//--------------------------------------------------------------------------
// Helper Functions
//--------------------------------------------------------------------------


/***************************************************************************/
/* Delay in increments of this subroutine's execution time.  (crude !)     */
/***************************************************************************/
void
mdelay (uint32 delay)
{
  register uint32 delvar = delay;
  while (delvar)
  {
    delvar--;
    delvar++;
    delvar--;
  }
}


//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------



/*===========================================================================

FUNCTION asb_and_msm_init

DESCRIPTION
  This function initializes any hardware necessary to be able to see all
  the memory space.

DEPENDENCIES
  None

RETURN VALUE
  None.  

SIDE EFFECTS
  None.

===========================================================================*/
void 
asb_and_msm_init(void)
{
  /*
   *  Do anything here necessary to set up to be able to see RAM or
   *  flash, or anyting else which is BASIC hardware init.
   *
   *  DO NOT try to identify the MSM.
   *  DO NOT make any function calls.
   *  DO NOT set any variables.
   *
   *  Following this call, memory_init will be called and it will zero
   *  out all ZI data area.  Also, we do not have a stack set up yet.
   */
  
  return;
}



/*===========================================================================

FUNCTION hw_id_and_init

DESCRIPTION
   This function identifies the target MSM and sets up PS_HOLD.

   To work around a bug in release 00.96, this function also enables
   and sets the PS_HOLD bit to ensure that power remains on during
   operation.

   In the same register, the GPIO bit used on TGP to enable Flash
   programming is also set high.

   The other GPIOs in that register are left as inputs.
DEPENDENCIES
  None

RETURN VALUE
  None.  

SIDE EFFECTS


===========================================================================*/
void 
hw_id_and_init (void)
{


}                               /* asb_and_msm_init() */

/*lint -restore*/

/*===========================================================================

FUNCTION memory_init

DESCRIPTION
  This function initializes initializes RAM for C code operation.
  
  This function is called from assembly.

DEPENDENCIES
  This function cannot make any call's since the stack has NOT been
  initialized.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.

===========================================================================*/

void
memory_init (void)
{
  // *************************************************************************
  // Warning: This routine can make no function calls and reference no global
  // data since we are still in rom and have no stacks yet.                  
  // *************************************************************************

  // The main purpose of this routine is to initialize the ram memory
  // interface. This should include setting up the bus sizer in a real
  // phone target.

  for (dst32 = (dword *) & Image$$APP_RAM$$ZI$$Base; 
       dst32 < (dword *) & Image$$APP_RAM$$ZI$$Limit; dst32++)      /*lint !e52 */
  {
    *dst32 = 0;
    // kick watchdog at least every 500 ms?
    KICK_WATCHDOG ();
  }


}                               /* memory_init() */


/*===========================================================================

FUNCTION msm_reset

DESCRIPTION
  This function causes the MSM to reset

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void msm_reset (void)
{
  for (;;)
  {
    // let the watch dog timer expire here...
  }

  /* does not return */
}


/*===========================================================================

FUNCTION CLK_PAUSE

DESCRIPTION
  Perform a busy-wait.  Not accurate, but accurate enough for the pause
  we want to do

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
clk_pause (int microseconds)
{
  while (microseconds > 50)
    {
      /* If greater accuracy is needed, adjust the pause value here by
         the amount of overhead in the loop.  Measurements indicate
         that this value is small (< 1 microsecond). */
      HWIO_OUT(PAUSE_TIMER, 50);
      microseconds -= 50;
    }

  /* Delay for any remaining time.  This involves a multiplication, so
   will be slower than expected. */
  if (microseconds > 0)
    HWIO_OUT(PAUSE_TIMER, microseconds);
}





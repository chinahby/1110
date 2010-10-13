/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H A R D W A R E   U T I L I T I E S

GENERAL DESCRIPTION
  This module contains hardware specific utilites.

EXTERNALIZED FUNCTIONS
  hw_valid_addr
    This function verifies that an address is within legal range.
  hw_power_off
    This function turns off the power to the phone.
  hw_reset
    This function resets the phone.
  hw_force_offline
    See if the phone is to be forced into offline.
  hw_version
    Return the hardware version
  hw_init
    Initialize hw_capability
  hw_capability
    Report the hardware options

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hw_init() must be called before hw_capability().

Copyright (c) 1992-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*---------------------------------------------------------------------------
                      HARDWARE CONFIGURATION VALUES

The following are the capabilities of the Vocoder for a particular target
They are returned as a mask from voc_capability() and pass as single values
to voc_config() to configure a particular mode.  An enum is used so that
type checking can be performed.
---------------------------------------------------------------------------*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6050/vcs/hw.c_v   1.6   13 Mar 2002 07:22:54   dlee  $
  $Header: //depot/asic/qsc1100/drivers/hw/hw.c#9 $
  $DateTime: 2009/05/21 01:41:55 $
  $Author: ovyas $

when       who     what, where, why
--------   ---     ----------------------------------------------------------   
11/12/08   hvo     Turned off VREG_MSMA locally due to KIP B0 problem
12/20/06   gmy     SC2x now drops PS_HOLD enabling power-off to complete
12/13/06   cr      sc2x partnum updates. Add a conditionally compiled test
                   stub.
10/21/05   dd      Remove unused hw_reset from L4 kernel build. This
                   eliminates dependency on clk regime from kernel.
07/23/05   wd      L4 Merge.
05/31/05   ejv     Remove include of dmod.h.
05/19/05   wd      Support for L4 linex.
05/04/04   bmg     Removed FFA/6500 features that were no longer relavent
                   and were causing compile errors
10/28/03   dnn     Undefine FFA not relevant to MSM6500 FFA 
04/25/03   ejv     Added hw_partnum_version(...).
01/15/03   cr      update hw_valid_addr() to return true for MDSP and ADSP 
                   addresses.
12/13/02   cr      update hw_valid_addr() to return true for MSM registers.
03/12/02   dl      Changed to keypad_is_power_key_pressed().
02/06/02   rmd     Included hs.h to get hs_is_power_key_pressed() prototype
                   definition.
01/30/02   rmd     Updated hw_power_off() so that it supports the power key on
                   both the PMIC and the MSM.
12/13/01   dsb/ro  Changes for MSM6050. hw_valid_addr checks whether any part
                   of the memory block is in the EFS area.
12/12/00   rmd     Changed hw_version() to support MSMHW_EXTENDED_HW_REVISION_NUMBER.
11/10/00   rmd     Moved in shadow register image from dmod.c
10/09/00   dsb     Removed TG== code.  Mainlined up to and including T_MSM31.
                   Mainlined T_ARM.
07/20/00   aks     Disable Shundan during power down, for FFA3300.
06/28/00   aks     Merged with PP3020.
06/08/00   dsb     Temporarily disabled Shundan Disable feature until we pull
                   in newer PM1000 code to link with.
03/06/00   jc      Wait for user to release power key before removing PS_HOLD
03/01/00   jc      Use logical BIO register names.
02/23/00   jc      Disable shundan feature in PM1000 when powering phone off.
01/17/00   mk      Included fs.h if FEATURE_EFS is defined.
12/14/99   jc      Added RAM_BASE to RAM_END.
12/07/99   mk/jc   Reinstated hw_valid_addr() for MSM3100 targets.
09/23/99   mk/cs   Changed MSG_LOW to MSG_ERROR.
08/05/99   mk      Using MSM_ macros in hw_version() for MSM3100.
08/02/99   jc      Removed gpio_xxx MSM3100 support.
07/15/99   jc      New gpio_xxx MSM3100 support.
05/12/99   mk      Added initial MSM3100 support.
10/01/98   dlb     Added support for MSM3 and ARM.
06/25/98   kss     Added MSM2310 support to hw_version()
03/31/98   aks     Configured for Odie (T_O).
06/17/97   dhh     Added target T_Q for Q Phone.
03/14/97   jjn     Configured for Module (T_MD)
11/02/96   jah     Configured for TGP (T_T)
03/12/96   jah     Changed hw_reset to not jump to download (and power-off)
                   now that download powers off on exit instead of resetting.
                   Corrected comments from 2/28.
02/28/96   dna     Removed hw_valid_addr() for builds without memory mapped
                   I/O.
11/28/95   jah     Added hw_init() and hw_capability().
05/10/95   jah     Added hw_version(), and Gemini support
07/06/93   jah     Changed boot_isr() reference to boot_reset().
07/06/93   jah     Removed the hw_force_offline() support for T_P, since it
                   uses car-kit sense, and we will soon have car kits.
05/11/93   jah     Added conditional compilation for Beta II (T_B2).
04/27/93   jah     Changed T_P hw_power_off() to loop kicking the dog until
                   the user removed the PS_HOLD override.  Changed hw_reset()
                   for T_DLOAD, to jump to boot_isr.
01/20/93   jah     Changed T_P to go offline if it is in a hands-free adapter
                   this should be a temporary change while the phone is out
                   for the first round of demo's with no cradle available.
01/07/93   jah     Changed hw_force_offline() to return FALSE on T_P, and to
                   have the message level there as LOW instead of ERR.
09/18/92   jah     Removed stretch tri-state for BIO_PORT_7.
08/24/92   jah     Added Tri-state enable for powerdown gpio outputs.
07/27/92   jah     Converted to new BIO macros, added code review changes.
05/29/92   jah     Created/Ported from the brassboard.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Definition for target specific defs  */
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "hw.h"                 /* Hardware prototypes and definitions  */
#include "hwi.h"                /* Hardware prototypes and definitions  */
#include "hw_mm.h"              /* multi-media related hw features      */
#include "err.h"                /* Error logging macros                 */
#include "dog.h"                /* Watchdog signal and time definitions */
#include "boothw.h"             /* Watchdog signal and time definitions */
#include CLKRGM_H
#ifndef FEATURE_GSM_PLT  
#ifndef FEATURE_L4_KERNEL
// L4LINUX_TODO
#include "keypad.h"                 /* Is power key pressed?                */
#endif
                                /*     Prototype definition.        */
#endif // FEATURE_GSM_PLT

#ifdef FEATURE_L4_KERNEL
#error code not present
#endif

#ifdef FEATURE_EFS
#include "fs.h"                 /* Definitions for Embedded File System */
#endif

#include "msm.h"                /* Registers for HW/SW version          */
#include "bio.h"                /* Bit I/O macros                       */

#ifdef FEATURE_PMIC
#include "pm.h"
#endif

#ifdef FEATURE_EFS
#ifndef BOOT_TARGET
#include "qdspext.h"
#include "mdspext.h"
#endif
#endif
#include "msm_drv.h"
#include "vbatt.h"


#ifdef FEATURE_DLOAD_HW_RESET_DETECT 
#include "boot_hw_reset.h"
#endif


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* QSC11x0 MSM/PMIC Version */
hw_qsc11x0_rev_type qsc11x0_hw_rev;

#ifdef FEATURE_SUBOPTIMAL_SHADOW
#error code not present
#endif /* FEATURE_SUBOPTIMAL_SHADOW */
/* Hardware capabilities available on this unit.
** This variable is initialized to the base configuration.
*/
LOCAL hw_capability_type hw_capability_value = HW_CAP_BASE_V;

#ifndef FEATURE_L4
/*
 * Pointers to the base and length of BB_RAM segments
 *
 * These symbols are generated by the linker with $'s instead of _'s.  We
 * re-name the symbol to use _'s to make valid C identifiers.  We embed
 * those symbols in bootsys.s so that we have them in the flash.
 */
extern byte *Image__BB_RAM__Base;
extern byte *Image__BB_RAM__Length;
extern byte *Image__ZI_REGION__ZI__Base;
extern byte *Image__ZI_REGION__ZI__Length;

/*
 * Pointers to the base and length of APP_RAM segments
 *
 * These symbols are generated by the linker with $'s instead of _'s.  We
 * re-name the symbol to use _'s to make valid C identifiers.  We embed
 * those symbols in bootsys.s so that we have them in the flash.
 */
extern byte *Image__APP_RAM__Base;
extern byte *Image__APP_RAM__Length;
#endif /* FEATURE_L4 */


/*===========================================================================

FUNCTION HW_VALID_ADDR

DESCRIPTION
  This procedure verifies that an address is within legal range. "Legal range"
  means physical RAM not containing code, or MSM register memory.
  EFS lives in FLASH/ROM part. Code lives in ROM or SDRAM. In any event,
  an inclusionary test on RAM addresses as described in by scatter loader
  symbols will determine if address is "legal".

DEPENDENCIES
  If the scatter loader file is changed to add new RAM segments, then this
  function will have to be changed.

RETURN VALUE
  True if the address is legal, False if not.

SIDE EFFECTS
  None

===========================================================================*/

/* Determine RAM and ROM size in bytes
*/
boolean hw_valid_addr
(
  void *ptr,            /* Address to validate                       */
  word len              /* Length of memory block pointed to by *ptr */
)
{
#ifndef FEATURE_L4
  /* the end of the memory block we're pointing to */
  byte* end_ptr = (byte*)ptr + len;
  byte* p=(byte*)ptr;

#ifdef FEATURE_EFS
#ifndef BOOT_TARGET
  /* allow access to physical ram */
  if (end_ptr >= Image__APP_RAM__Base &&
        end_ptr <= (byte*)((dword)Image__APP_RAM__Base+(dword)Image__APP_RAM__Length) &&
        p >= Image__APP_RAM__Base &&
        p <= (byte*)((dword)Image__APP_RAM__Base+(dword)Image__APP_RAM__Length))
      return TRUE;
  if (end_ptr >= Image__BB_RAM__Base &&
        end_ptr <= (byte*)((dword)Image__BB_RAM__Base+(dword)Image__BB_RAM__Length) &&
        p >= Image__BB_RAM__Base &&
        p <= (byte*)((dword)Image__BB_RAM__Base+(dword)Image__BB_RAM__Length))
      return TRUE;
  if (end_ptr >= Image__ZI_REGION__ZI__Base &&
        end_ptr <= (byte*)((dword)Image__ZI_REGION__ZI__Base+(dword)Image__ZI_REGION__ZI__Length) &&
        p >= Image__ZI_REGION__ZI__Base &&
        p <= (byte*)((dword)Image__ZI_REGION__ZI__Base+(dword)Image__ZI_REGION__ZI__Length))
      return TRUE;
  /* allow access to MSM registers */
  if (HWIO_IS_MSM((dword)ptr) && HWIO_IS_MSM((dword)end_ptr))
      return TRUE;
  /* allow access to DSP memory */
  if (qdsp_valid_address(ptr,len))
	   return TRUE;
  if (mdsp_valid_address(ptr,len))
	   return TRUE;
  return FALSE;

#endif /* ifndef BOOT_TARGET */
  return TRUE;
#else
  return TRUE;
#endif
#else
#error code not present
#endif /* FEATURE_L4 */

}


#ifdef FEATURE_EFS2
  /* This function allows EFS to complete its last transaction on good
   * powerdowns. */
  extern void efs_wait_for_xact (void);
#endif


/*===========================================================================

FUNCTION HW_POWER_OFF

DESCRIPTION
  Turn off the power to the phone.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/
void hw_power_off( void )
{
  word isave;


#ifdef FEATURE_EFS2
  /* Allow EFS to complete its last transaction.
  */
  efs_wait_for_xact ();
#endif

  INTLOCK_SAV (isave);
  /* isave not used, as interrupts are not reenabled. */

  #if !defined (__GNUC__)
  #if defined(FEATURE_PMIC_MOMENTARY_SUDDEN_PWR_LOSS)
  /* Disable Sudden Momentary power loss driver to prevent the phone from
     automatically restarting */
  pm_smpld_switch(PM_OFF_CMD);

  #endif
  
  // Gemsea Add For PowerDown
  // 此处必须关掉PM的DOG，否则将导致关机变重启
  // 原来的处理在POWERKEY的检测部分，局限性太大，因此转移到此处
  #if defined(FEATURE_PMIC_WDOG)
  pm_watchdog_reset_detect_switch(PM_OFF_CMD);
  #endif
  #endif

  #ifndef FEATURE_GSM_PLT    
  #ifndef BOOT_TARGET
  /* Wait for the user to release power key before removing PS_HOLD */
  while(keypad_is_power_key_pressed())
  {
      dog_kick();
  }
  #endif /* ifndef BOOT_TARGET */
  #endif // FEATURE_GSM_PLT

   if( qsc11x0_hw_rev.pmic_hw_version >= 3 )
  {
  /* Switch off RTC if  Vbattery < 3.35V */
  if(vbatt_read_mv() < 3350)
    pm_hal_rtc_stop();
  }

   /* Set the Regulator to 2.5V - Only for KIP B1 Revision */
   if( qsc11x0_hw_rev.pmic_hw_version > 3)
  {
   pm_vreg_set_level(PM_VREG_TCXO_ID,2500);
  }

  if( qsc11x0_hw_rev.pmic_hw_version == 3 )
  {
	  outpdw(0x80018008,0x000a002e);  // disable VREG MSMA locally as KIP B0 has problem
  }

  #if defined(FEATURE_PMIC)
  /* Soft reset the PMIC to make sure that the PMIC goes into the power
     down state. Similar to UVLO reset. */
  pm_soft_reset();
  #endif




#ifdef FEATURE_DLOAD_HW_RESET_DETECT   
  /* Clear magic number to prevent entering into the download mode */
  boot_hw_reset_shared_info_reset();
  #endif
  /* Turn off PS_HOLD, this line feeds into the voltage regulator.
  ** The power will turn off in ~50ms when the capacitor drains.
  */
  hw_set_ps_hold(HW_PS_HOLD_DISABLE);

  /* Wait for the regulator to drop
  */
  for (;;)
  {
    dog_kick();
  }

} /* end of hw_power_off */


/*===========================================================================

FUNCTION HW_RESET

DESCRIPTION
  Reset the phone.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/
#ifdef CUST_EDITION
#include "GlobalData.h"
#endif
#ifndef FEATURE_L4_KERNEL
void hw_reset( void )
{
  word isave;

#ifdef FEATURE_EFS2
  /* Allow EFS to complete its last transaction.
  */
  efs_wait_for_xact ();
#endif

  INTLOCK_SAV (isave);
#if !defined(T_FFA) && ! defined(T_MSM6100B)
  outpw(0x20100080,0);
#endif /*!defined(T_FFA) ! defined(T_MSM6100B)*/
#ifdef CUST_EDITION
  g_BootResetFlag = BOOT_RESET_MAGIC;
#endif
  while(TRUE);
#ifdef FEATURE_DLOAD_HW_RESET_DETECT   
  /* Clear magic number to prevent entering into the download mode */
  boot_hw_reset_shared_info_reset();
  #endif

} /* end of hw_reset */
#endif


/*===========================================================================

FUNCTION HW_FORCE_OFFLINE

DESCRIPTION
  See if the phone is to be forced into offline.

DEPENDENCIES
  None

RETURN VALUE
  True if the phone is to be forced offline, False if not.

SIDE EFFECTS
  None

===========================================================================*/
boolean hw_force_offline( void )
{
  MSG_ERROR("hw_force_offline does not work on this target", 0, 0, 0);
  return( FALSE );
} /* end of hw_force_offline */


/*===========================================================================

FUNCTION HW_VERSION

DESCRIPTION
  Return the hardware version

DEPENDENCIES
  None

RETURN VALUE
  16-bit hardware version

SIDE EFFECTS
  None

===========================================================================*/
word hw_version( void )
{

  static word version;
  static boolean read_version_once = FALSE;

  /* Just read the hardware version once. Keep a local
     copy for future calls to this function. */
  if (read_version_once == FALSE)
  {
    read_version_once = TRUE;

    /* For MSM6500 and newer, pass this value back which will allow
       "old-school" version checks to pass, but further ID info must
       be retrieved by calling hw_partnum_version(...) */
    version = 0xFF00;
  }

  return( version );

} /* end of hw_version */


/*===========================================================================

FUNCTION HW_PARTNUM_VERSION

DESCRIPTION
  Fill in the hardware partnumber and version ID from the MSM.

DEPENDENCIES
  None

RETURN VALUE
  None - data is passed by filling in pointer data structure

SIDE EFFECTS
  None

===========================================================================*/
void hw_partnum_version( hw_rev_type * rev_ptr )
{

  rev_ptr->partnum     = VERSION_GET_MSM_ID( );

  rev_ptr->version     = VERSION_GET_VERSION_ID( );

  rev_ptr->hw_register = VERSION_GET();


} /* hw_partnum_version */


/*===========================================================================

FUNCTION HW_CHECK_PARTNUM_VERSION

DESCRIPTION
  Compare hardware partnumber and version ID passed as parameter with the MSM.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE  - Part number and version passed as parameter match MSM

SIDE EFFECTS
  None

===========================================================================*/
boolean hw_check_partnum_version( uint16 partnum, uint8  version )
{
static uint16 hw_partnum = 0;
static uint8  hw_version = 0; 
    
  
  /* Store the part number and version on first access */
  if (hw_partnum == 0) 
  {
    hw_partnum = VERSION_GET_MSM_ID( ); 
    hw_version = VERSION_GET_VERSION_ID( );
  }

  if (partnum == hw_partnum &&
      version == hw_version)
  {
     return TRUE;
  }
  else
  {
     return FALSE;
  }

} /* hw_check_partnum_version */

/*===========================================================================

FUNCTION HW_INIT

DESCRIPTION
  This function determines the options this hardware supports,
  and returns that information to the caller.

DEPENDENCIES
  None

RETURN VALUE
  Mask of options the hardware is supports.

SIDE EFFECTS
  None

===========================================================================*/
void hw_init
(
  dword config_value /* Configuration value read from NVM */
)
{
  #define HW_TEST
  #ifdef HW_TEST
    hw_rev_type rev;
    hw_partnum_version( &rev );
  #endif

  hw_capability_value = config_value;

} /* end of hw_init */


/*===========================================================================

FUNCTION HW_CAPABILITY

DESCRIPTION
  This function determines the options this hardware supports,
  and returns that information to the caller.

DEPENDENCIES
  hw_init() must be called before hw_capability() is called.

RETURN VALUE
  Mask of options the hardware is supports.

SIDE EFFECTS
  None

===========================================================================*/
hw_capability_type hw_capability( void )
{
  return( hw_capability_value );

} /* end of hw_capability */

/*===========================================================================

FUNCTION HW_GET_VFE_TYPE

DESCRIPTION
  This function determines the VFE options this hardware supports,
  and returns that information to the caller.

DEPENDENCIES
  None.

RETURN VALUE
  VFE type supported by HW.

SIDE EFFECTS
  None

===========================================================================*/
hw_vfe_type hw_get_vfe_type( void )
{
  return ( VFE_EAGLE_05);

} /* end of hw_get_vfe_type */

/*===========================================================================

FUNCTION HW_INIT_QSC11X0_HW_REV

DESCRIPTION
  This function initialized MSM/PMIC version in the 

DEPENDENCIES
  pm_init() must be called before hw_init_qsc110x_hw_rev() is called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hw_init_qsc11x0_hw_rev(void)
{  
  // Get the MSM hardware revision
  qsc11x0_hw_rev.msm_hw_version = VERSION_GET_VERSION_ID();

  // Get PMIC hardware revision
  qsc11x0_hw_rev.pmic_hw_version = pm_get_pmic_revision();

  if(qsc11x0_hw_rev.msm_hw_version == 0x0 && qsc11x0_hw_rev.pmic_hw_version == 1)
  {
#ifdef QSC11X0_PG2_POWER_GRID
     qsc11x0_hw_rev.pg = 2;   // PG2
#else
     qsc11x0_hw_rev.pg = 1;   // PG1
#endif
  }
  else if(qsc11x0_hw_rev.msm_hw_version == 0x1 && qsc11x0_hw_rev.pmic_hw_version == 1)
  {
#ifdef QSC11X0_PG2_POWER_GRID
     qsc11x0_hw_rev.pg = 2;   // PG2
#else
     qsc11x0_hw_rev.pg = 1;   // PG1
#endif
  }
  else if(qsc11x0_hw_rev.msm_hw_version == 0x1 && qsc11x0_hw_rev.pmic_hw_version >= 3)
  {
#ifdef QSC11X0_PG2_POWER_GRID
     qsc11x0_hw_rev.pg = 2;   // PG2
#else
     qsc11x0_hw_rev.pg = 3;   // PG1
#endif
  }
}


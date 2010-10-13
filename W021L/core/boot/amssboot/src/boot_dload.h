#ifndef BOOT_DLOAD_H
#define BOOT_DLOAD_H

/*===========================================================================

                       B o o t  D o w n l o a d e r   
                           H e a d e r  F i l e

DESCRIPTION
 Contains common defintion used by boot downloader.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2005, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/boot_dload.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/08   tnk     Modified structure to exclude hu_usb during osbl boot_build and include in amss build.
05/08/07   ty      Modify structure saved to IRAM to include serial number
01/24/07   ly      Add HS_USB support
10/13/05   sd      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "comdef.h"
#endif /* _ARM_ASM_ */


#ifndef BUILD_BOOT_CHAIN
#ifdef FEATURE_HS_USB
#include "hsu_conf_sel_types.h"
#endif
#endif
/*===========================================================================

                     PUBLIC TYPE DEFINITIONS

===========================================================================*/

typedef struct
{
  uint32 magic_1;
  uint32 magic_2;

} dload_id_type;

#ifndef BUILD_BOOT_CHAIN
#ifdef FEATURE_HS_USB
/* Structure to contain DLOAD IRAM data in High-Speed USB mode. */
typedef struct 
{
  dload_id_type                  dload_id;
  /* The version number of the IRAM data.
  ** The ARMPROG should use this version number to verify that it can
  ** use the new IRAM data.
  */
  uint32                         hsu_version_num;
  /* Set to FALSE before going into download mode.
  ** Set to TRUE by the HS-USB DLOAD adaptation layer.
  ** Used for indicating to ARMPRG whether DLOAD worked over HSU or not.
  ** FALSE means that DLOAD worked over UART.
  */
  boolean                        dload_used_hsu; 
  /* The product ID used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD and ARMPRG), so that the phone
  ** does not switch com ports.
  */
  uint16                         hsu_product_id;
  /* The Serial Number used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD and ARMPRG), so that the phone
  ** does not switch com ports.
  */
  char                           serial_number[HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN];

} dload_hsu_iram_data_type;
#endif  /* FEATURE_HS_USB */
#endif

/* Structure to define ID to enter download mode. */ 


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifndef _ARM_ASM_
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifndef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_DLOAD_TRANSITION

DESCRIPTION
  This function initiates the reset of the chip and transition to the
  boot downloader in the OEMSBL during boot.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Never returns and causes chip reset to occur.

===========================================================================*/
void boot_dload_transition( void );
#endif /* BUILD_BOOT_CHAIN */

#ifdef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_DLOAD_ENTRY

DESCRIPTION
  Determines if identifier is present in IRAM to tell OEMSBL to enter
  the boot downloader, instead on continuing the normal boot process.
  
DEPENDENCIES
  Data in IRAM is retained across a reset.
  
RETURN VALUE
  TRUE indicates downloader should be entered.

SIDE EFFECTS
  None

===========================================================================*/
boolean boot_dload_entry( void );
#endif /* BUILD_BOOT_CHAIN */

#endif /* _ARM_ASM_ */

#endif /* BOOT_DLOAD_H */

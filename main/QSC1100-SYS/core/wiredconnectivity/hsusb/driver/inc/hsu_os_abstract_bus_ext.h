#ifndef HSU_OS_ABSTRACT_BUS_EXT_H
#define HSU_OS_ABSTRACT_BUS_EXT_H

/*==============================================================================

                H S U _ O S _ A B S T R A C T _ B U S _ E X T . H
                                

GENERAL DESCRIPTION
  High Speed USB OS abstraction layer bus externalized API declarations.
  The APIs declared in this file are intended for the layers above the OS
  abstraction layer, and not for the HS-USB core driver.

EXTERNALIZED FUNCTIONS
  Externalized to outside of the OS abstraction layer:
    hsu_os_bus_read_from_phy
    hsu_os_bus_write_to_phy


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_os_abstract_bus_ext.h#9 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
03/10/08  ds      Initial version

==============================================================================*/


/*==============================================================================

                               INCLUDE FILES

==============================================================================*/
#include <comdef.h>
#include "port.h"

/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
  Typedefs
==============================================================================*/

/* Interrupt resource info type. */
typedef struct 
{
  /* The index of the core which owns this interrupt. */
  uint32            core_index;

  /* User supplied interrupt handler.                 */
  interrupt_handler isr_func;

  /* Argument to be passed to the ISR function.       */
  void*             isr_arg;

  /* User supplied deferred service routine handler.  */
  jthread_func      dsr_func;

  /* Argument to be passed to the DSR function.       */
  void*             dsr_arg;

  #if defined(FEATURE_HS_USB_PMIC_PHY) || defined(FEATURE_HS_USB_LEGACY_DEVICE)
  /* events posted  */
  uint32            event;
  #endif /* FEATURE_HS_USB_PMIC_PHY || FEATURE_HS_USB_LEGACY_DEVICE */

} interrupt_resource_info_type;

/*==============================================================================
  Constants and Macros
==============================================================================*/
  
#if defined(FEATURE_HS_USB_PMIC_PHY) || defined(FEATURE_HS_USB_LEGACY_DEVICE)

  /* events : add bit mappted events below. */
#define HSU_OS_BUS_DISCONNECT_EVT 0x00000001
#define HSU_OS_BUS_CONNECT_EVT    0x00000002
#define HSU_OS_BUS_RESUME_EVT     0x00000004

#endif /* FEATURE_HS_USB_PMIC_PHY || FEATURE_HS_USB_LEGACY_DEVICE */


/*==============================================================================

                          FUNCTION DECLARATIONS

==============================================================================*/


/*===========================================================================
FUNCTION HSU_OS_BUS_READ_FROM_PHY

DESCRIPTION
  Read from a PHY register using the USB core's ULPI_VIEWPORT register.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the PHY read operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_read_from_phy(uint8 phy_register, uint8* read_value);


/*===========================================================================
FUNCTION HSU_OS_BUS_WRITE_TO_PHY

DESCRIPTION
  Write to a PHY register using the USB core's ULPI_VIEWPORT register.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the PHY write operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_write_to_phy(uint8 phy_register, uint8 written_value);

/*===========================================================================
FUNCTION HSU_OS_BUS_SAVE_MEM_DUMP

DESCRIPTION
This function saves the :
-USB device controller registers
-USB host controller registers
-ULPI registers
in a pre-defined static memory.

DEPENDENCIES
None.

RETURN VALUE
None

SIDE EFFECTS
None.
===========================================================================*/
#ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
void hsu_os_bus_save_mem_dump(void);
#endif /* ! FEATURE_HS_USB_MEM_OPTIMIZATION */

#if defined(FEATURE_HS_USB_PMIC_PHY) || defined(FEATURE_HS_USB_LEGACY_DEVICE)
/*===========================================================================
FUNCTION HSU_OS_BUS_HANDLE_PMIC_EVENT

DESCRIPTION
  This function handles PMIC PHY events when the PMIC PHY is used.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_handle_pmic_event(uint32 core_index, uint32 event);
#endif /* FEATURE_HS_USB_PMIC_PHY || FEATURE_HS_USB_LEGACY_DEVICE */

/*===========================================================================
FUNCTION HSU_HANDLE_SLEEP_VOTE

DESCRIPTION
  This function is called when entering LPM. This function votes for sleep
  if all cores are in LPM.
  This function is also disable the vote for sleep if one of the core
  exit form LPM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_handle_sleep_vote(boolean assert_okts);

#endif /* HSU_OS_ABSTRACT_BUS_EXT_H */

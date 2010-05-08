#ifndef HSU_OS_ABSTRACT_BUS_H
#define HSU_OS_ABSTRACT_BUS_H

/*==============================================================================

                   H S U _ O S _ A B S T R A C T _ B U S . H
                                

GENERAL DESCRIPTION
  High Speed USB OS abstraction layer bus management module declarations.
  OS abstraction layer regional header file.

EXTERNALIZED FUNCTIONS
  Externalized internally in the OS abstraction layer:
    hsu_os_bus_init
    hsu_os_bus_uninit

  Externalized to other parts of the HS-USB module:
    hsu_os_bus_poll_interrupt
    hsu_os_bus_handle_interrupt
    os_bus_start_stop_lpm_timer
    hsu_os_notify_stack_ready
    hsu_os_core_hw_init
    hsu_os_core_hw_uninit
    hsu_os_bus_lpm_enable
    hsu_os_bus_lpm_disable
    hsu_os_bus_enter_lpm
    hsu_os_bus_exit_lpm
    hsu_port_is_attached

    hsu_os_bus_interrupt_init
    hsu_os_bus_interrupt_uninit

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/os_abstract/rex/hsu_os_abstract_bus.h#17 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
05/15/06  ds      Initial version

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES

==============================================================================*/

/* AMSS include files. */
#include <comdef.h>

/* High-Speed USB stack include files. */
#include <jtypes.h>


/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/
enum
{
#ifndef T_REXNT

#if !defined (FEATURE_HS_USB_NO_HS_OTG_CORE) || defined (FEATURE_HS_USB_BASIC)
  HS_USB_CORE_INDEX,
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE || FEATURE_HS_USB_BASIC */

#if defined(FEATURE_HS_USB_LEGACY_HOST) || defined(FEATURE_HS_USB_LEGACY_DEVICE)
  FS_USB_CORE_INDEX,
#endif /* FEATURE_HS_USB_LEGACY_HOST || FEATURE_HS_USB_LEGACY_HOST */

#ifdef FEATURE_HS_USB_UICC
  UICC_USB_CORE_INDEX,
#endif /*FEATURE_HS_USB_UICC*/

#else /* T_REXNT */
#error code not present
#endif  /* T_REXNT */

  NUMBER_OF_CORES
};

/*==============================================================================

                    FUNCTION DECLARATIONS

==============================================================================*/


/*===========================================================================
FUNCTION HSU_OS_BUS_INIT

DESCRIPTION
  Initializes the bus module. Called by the OS abstraction layer's main
  initialization function (hsu_os_init).

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_bus_init(void);

/*===========================================================================
FUNCTION HSU_OS_BUS_UNINIT

DESCRIPTION
  Un-initialize the bus module. Called by the OS abstraction layer's main
  un-initialization function (hsu_os_uninit).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_uninit(void);


#ifdef FEATURE_HS_USB_BASIC

/*===========================================================================
FUNCTION HSU_OS_BUS_POLL_INTERRUPT

DESCRIPTION
  Checks whether a High-Speed USB controller interrupt
  has occurred.

  This function accesses High-Speed USB controller interrupt status registers
  in order to determine whether an interrupt has occurred.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an interrupt has occurred. FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_poll_interrupt(void);

#endif  /* FEATURE_HS_USB_BASIC */


#ifdef FEATURE_HS_USB_BASIC

/*===========================================================================
FUNCTION HSU_OS_BUS_HANDLE_INTERRUPT

DESCRIPTION
  This function handle a High-Speed USB interrupt.

  NOTE - This function should only be caller when a High-Speed USB 
  interrupt is detected.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_handle_interrupt(void);

#endif  /* FEATURE_HS_USB_BASIC */

/*===========================================================================
FUNCTION HSU_OS_NOTIFY_STACK_READY

DESCRIPTION
  Tell the OS abstraction layer that the USB stack is fully initialized.
  This notification is used for suspending the USB bus and for power management.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_notify_stack_ready(void);

/*===========================================================================
FUNCTION HSU_OS_CORE_HW_INIT

DESCRIPTION
  call add_generic_controller for the core specified by core_index param.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_core_hw_init(uint32 core_index);

/*===========================================================================
FUNCTION HSU_OS_CORE_HW_UNINIT

DESCRIPTION
  Remove a specific controller, call del_generic_controller to delete it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_core_hw_uninit(uint32 core_index);

/*===========================================================================
FUNCTION OS_BUS_START_STOP_LPM_TIMER

DESCRIPTION
This function start or stop the LPM timer depend on the received parameter

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void os_bus_start_stop_lpm_timer(boolean start_timer);

/*===========================================================================
FUNCTION HSU_OS_BUS_ENTER_LPM

DESCRIPTION
  Enter High-Speed USB PHY into low-power mode state.
  This function is invoked when the High-Speed USB bus is suspended, in 
  order to save battery power.
  This function stops the High-Speed USB core PHY clock and enables
  asynchronous interrupts from the PHY.
  After the PHY is entered into a low power-state, this function asserts
  the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_enter_lpm(uint32 core_index, 
                          boolean called_by_lpm_timer,
                          boolean analog_audio_mode);

/*===========================================================================
FUNCTION HSU_OS_BUS_EXIT_LPM

DESCRIPTION
  Exit High-Speed USB PHY from the low-power mode state.
  This function is invoked when the High-Speed USB bus is resumed from a
  suspend state, in order to return the High-Speed USB PHY back to an 
  operational state.
  This function negates the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_bus_exit_lpm(uint32 core_index);

/*===========================================================================
FUNCTION HSU_OS_BUS_GET_NUM_CORES

DESCRIPTION
  This function returns the number of cores supported.

DEPENDENCIES
  None.

RETURN VALUE
  Number of supported cores.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 hsu_os_bus_get_num_cores(void);


/*===========================================================================
FUNCTION HSU_OS_BUS_LPM_DISABLE

DESCRIPTION
  Disable the LPM feature. Exit from low-power mode if needed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_lpm_disable(uint32 core_index);


/*===========================================================================
FUNCTION HSU_OS_BUS_LPM_ENABLE

DESCRIPTION
  Enable the low-power mode feature.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_lpm_enable(uint32 core_inde);

#ifndef T_REXNT
/*===========================================================================
FUNCTION HSU_OS_BUS_READ_FROM_PHY

DESCRIPTION
Read from a PHY register using the USB core's ULPI_VIEWPORT register.

DEPENDENCIES
hsu_os_bus_init() must have been called before using this function.

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
hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
TRUE if the PHY write operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_os_bus_write_to_phy(uint8 phy_register, uint8 written_value);
#endif /* T_REXNT */

/*===========================================================================
FUNCTION HSU_PORT_IS_ATTACHED

DESCRIPTION
This function checks whether the High-Speed USB port is attached.

DEPENDENCIES
None.

RETURN VALUE
TRUE if the high-speed USB port is attached. Otherwise, returns FALSE.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_port_is_attached(uint32 core_index);


/*===========================================================================
FUNCTION HSU_OS_BUS_INTERRUPT_INIT

DESCRIPTION
Initializes a USB core's interrupt. Called when the core's interrupt is
registered its first interrupt service routine (using the 
os_interrupt_setup() call).

DEPENDENCIES
None.

RETURN VALUE
Returns 0 on success, otherwise returns one of the error codes 
defined in jerrno.h

SIDE EFFECTS
None.
===========================================================================*/
jresult_t hsu_os_bus_interrupt_init(uint32 core_index);


/*===========================================================================
FUNCTION HSU_OS_BUS_INTERRUPT_UNINIT

DESCRIPTION
Un-initializes High-Speed USB interrupt. Called by the bus module
un-initialization function (hsu_os_bus_uninit).

DEPENDENCIES
None.

RETURN VALUE
Returns 0 on success, otherwise returns one of the error codes 
defined in jerrno.h

SIDE EFFECTS
None.
===========================================================================*/
jresult_t hsu_os_bus_interrupt_uninit(uint32 core_index);


#endif /* HSU_OS_ABSTRACT_BUS_H */


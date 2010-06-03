#ifndef HSU_EVENT_DEFS_H
#define HSU_EVENT_DEFS_H
/* =======================================================================
                               hsu_event_defs.h
DESCRIPTION
  Type definitions used to support the Debug Diagnostic events 
  (related to High-Speed USB) reside in this file.
  
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_event_defs.h#1 $
$DateTime: 2008/02/05 12:01:56 $ $Author: lmanor $

$Change: 

when       who          what, where, why
--------   -----       ----------------------------------------------------------
08/05/07   tl           Created

===========================================================================*/

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Used for EVENT_HS_USB_HID_CONNECT event.
 * Specifies the type of the HID device that was attached
 */
typedef enum
{
  HS_USB_ATTACHED_HID_KEYBOARD,
  HS_USB_ATTACHED_HID_MOUSE,
  HS_USB_ATTACHED_HID_JOYSTICK,
  HS_USB_ATTACHED_HID_GAMEPAD 
}hsu_event_hid_attached_type;

/* Used for EVENT_HS_USB_STACK_SUSPENDED and EVENT_HS_USB_STACK_RESUMED event.
 * Specifies the type of the stack that was suspended/resumed (host/periphera)
 */
typedef enum
{
  HS_USB_HOST_STACK,  
  HS_USB_PERIPHERAL_STACK  
}hsu_event_stack_suspend_resume_type;

/* Used for EVENT_HS_USB_OPERATIONAL_ERROR  event. 
 * Specifies the type of the error occurred while trying to connect a 
 * peripheral device to the USB port.
 */
typedef enum
{
  HS_USB_UNKNOWN_HID,
  HS_USB_ADD_PERIF_NO_FREE_ENTRY,
  HS_USB_CANT_ENABLE_HID,
  HS_USB_CANT_ADD_MS 
}hsu_event_operational_error_type;

#endif /* HSU_EVENT_DEFS_H */

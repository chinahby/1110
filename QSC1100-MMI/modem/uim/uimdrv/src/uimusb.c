/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        U I M     U S B     L A Y E R

GENERAL DESCRIPTION
  This module contains the procedures for supporting USB technology based
  UICC cards.The module interacts with the USB driver at power up to
  determine if USB enumeration is successful. If USB enumeration is successful
  the module acts as a shim layer between the UIM server and the USB driver
  translating UIM commands and responses from UIM server APIs to USB driver
  APIs and vice versa.

EXTERNALIZED FUNCTIONS




INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008-2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimusb.c#2 $ $DateTime: 2009/02/04 21:12:01 $ $Author: viveks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/09   vs      Added access to historical bytes in ATR
11/04/08   vs      Updated poll flag clearing sequence to fix power up issue
10/07/08   vs      Updated callback APIs based on changes in USB driver code
09/23/08   js      Added LTK support for USB UICC
09/17/08   vs      Updated UIM USB ICCD poll delay timer logic
09/17/08   js      Implemented UIM Card interface selection based on first
                   instruction class NV
09/17/08   vs      Fixed Klocwork errors
09/15/08   vs      Added support for UIM USB ICCD pre-suspend timer
09/05/08   vs      Added cool down period between failed USB enumeration and
                   ISO fallback. Privatized fallback related function. Added
                   memory management logic. Fixed pointer variable names.
09/05/08   js      Fixed SW handling functions for SW1 = 0x61 and 0x6c
08/22/08   vs      Fixed names of statics. Redesigned callbacks to remove
                   rex_sleep(). Added powerup waiting time get function. Added
                   support for ISO fallback. Removed reference to ptr after
                   NULL check
08/22/08   js      Re-designed R-APDU status word processing
08/05/08   vs      Added calls to lock USB driver mutex before making an API
                   call and unlocking it after completing the call. Added
                   definition for uim_usb_is_usb_atr_received(). Moved
                   declarations to uimusb header.
07/24/08   vs      Fixed featurization for uim_usb_enumeration_failed()
07/18/08   vs      Moved uimusb.h header inclusion out of featurization.
                   Explicitly cast callback function arg in USB driver calls
07/17/08   vs      Created the UIM USB module to support the UIM USB UICC
                   ICCD interface

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

This section contains header inclusions

===========================================================================*/
#include "target.h"
#include "customer.h"
/* The uimusb.h header inclusion should be kept out side the featurization.
   This is to ensure that when FEATURE_UIM_USB_UICC is defined in target
   specific cust files it gets included */
#include "uimusb.h"

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */


/*===========================================================================

FUNCTION UIM_USB_ENUMERATION_FAILED

DESCRIPTION
  This function is called by the USB driver when either the USB connect fails
  or the USB ICCD interface enumeration fails. The UIM_USB_ENUMERATION_FAIL_SIG
  is set

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_usb_enumeration_failed
(
  void
)
{
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
} /* uim_usb_enumeration_failed */

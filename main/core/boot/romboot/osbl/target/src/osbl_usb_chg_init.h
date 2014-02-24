#ifndef OSBLUSB_INIT_H
#define OSBLUSB_INIT_H

/*===========================================================================

                  Operating System  B o o t   L o a d e r
                       USB Battery Charging Handler 
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the OEM-SBL
  USB charging 
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/osbl/osbl_usb_chg_init.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/09   dk      Removing compiler warnings.
05/30/06   cng     Initial revision

============================================================================*/
/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "osbl.h"
#include "qhsusb_al_chg_api.h"



/* Set the battery alarm upper and lower threshold
 * BAT_UPR_THRESH = 3600 mV = threshold for detection of battery absence
 * BAT_LWR_THRESH = 3200 mV = threshold for stopping charging
 */
#ifndef OSBL_USB_CHG_BAT_UPR_THRESH
#define OSBL_USB_CHG_BAT_UPR_THRESH 3300
#endif

#ifndef OSBL_USB_CHG_BAT_LWR_THRESH
#define OSBL_USB_CHG_BAT_LWR_THRESH 3200
#endif
#define OSBL_USB_CHG_BAT_WIN_UPR_THRESH 3300
#define OSBL_USB_CHG_BAT_WIN_MID_THRESH 3200
#define OSBL_USB_CHG_BAT_WIN_LWR_THRESH 2800


typedef enum
{
    LowSpeed,

    HighSpeed
} clkrgm_usb_chg_mode_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  OSBL_USB_CHG_INIT

DESCRIPTION
  This function performs USB battery charging if necessary

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/
void 
osbl_usb_chg_init( bl_shared_data_type *bl_shared_data );

void
osbl_delay_in_ms ( uint32 wait_in_ms ) ;

void
osbl_usb_chg_set_led(boolean value) ;

void 
osbl_power_down ( void ) ;  
void osbl_usb_chg_set_clock_mode(clkrgm_usb_chg_mode_type clk_regime);

uint32 
get_osbl_ahb_bus_freq(void);

#endif /* OSBLUSB_INIT_H */

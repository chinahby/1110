#ifndef _HSU_MODEM_PROC_SERVICES_H_
#define _HSU_MODEM_PROC_SERVICES_H_

/*==============================================================================

     High Speed USB Module APIs on Modem processor 
      H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This module defines High Speed USB APIs that are implemented on the modem 
    processor and used by other components on the Modem processor. 

  EXTERNALIZED FUNCTIONS
  hsu_modem_proc_services_set_product_id
  hsu_modem_proc_services_get_product_id
  hsu_modem_proc_services_set_serial_number
  hsu_modem_proc_services_get_serial_number

  INITALIZATION AND SEQUENCING REQUIREMENTS
    TODO.
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_modem_proc_services.h#3 $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  05/02/07  me  Initial version
    
==============================================================================*/
#include "customer.h"
#include "hsu_common.h"
/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/*==============================================================================
FUNCTION      HSU_MODEM_PROC_SERVICES_SET_PRODUCT_ID

DESCRIPTION
This function updates the product id in the ARM9 static variable

DEPENDENCIES
TOOD.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_product_id(unsigned short product_id);

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_GET_PRODUCT_ID

DESCRIPTION
This function returns the product id stored in the ARM9 static variable

DEPENDENCIES
TOOD.

RETURN VALUE
The product_id (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/
unsigned short hsu_modem_proc_services_get_product_id(void);

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_SET_SERIAL_NUMBER

DESCRIPTION
This function sets the serial number in the ARM9 static variable

DEPENDENCIES
TOOD.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_serial_number(const char* serial_number);

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_GET_SERIAL_NUMBER

DESCRIPTION
This function returns the serial number stored in the ARM9 static variable

DEPENDENCIES
TOOD.

RETURN VALUE
The serial number (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/
char* hsu_modem_proc_services_get_serial_number(void);

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_SET_IS_SERIAL_NUMBER_NULL

DESCRIPTION
This function updates the hsu_is_serial_number_null boolean stored in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
None

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_is_serial_number_null(boolean is_serial_number_null);

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_IS_SERIAL_NUMBER_NULL

DESCRIPTION
This function returns whether the serial number stored in the ARM9 static variable is NULL

DEPENDENCIES
None.

RETURN VALUE
TRUE if serial number should be set to NULL (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_modem_proc_services_is_serial_number_null(void);

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_INIT

DESCRIPTION
This function performs HSUSB initializations needed on ARM9 processor

DEPENDENCIES
This function should be called from tmc_init for Linux targets.

RETURN VALUE
TRUE for success.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_modem_proc_services_init(void);


#endif /* _HSU_MODEM_PROC_SERVICES_H_ */

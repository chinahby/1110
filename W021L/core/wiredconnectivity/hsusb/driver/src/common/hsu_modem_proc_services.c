
/*==============================================================================

High Speed USB Module APIs on Modem processor 
H E A D E R   F I L E

GENERAL DESCRIPTION
This module defines High Speed USB APIs that are implemented on the modem 
processor and used by other components on the Modem processor. 
This file compiles ONLY for the main ARM9 image.

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
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_modem_proc_services.c#5 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  05/02/07  me  Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_modem_proc_services.h"
#include "comdef.h"
#include "hsu_conf_sel_types.h"
#include "AEEstd.h"
#include "hsu_conf_sel_comp_utils.h"
#ifdef FEATURE_EXPORT_HSU_PROC_COMM_APIS
  #include "hsu_mdm_apis_pcil.h"
#endif /* FEATURE_EXPORT_HSU_PROC_COMM_APIS */

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
static uint16 hsu_product_id = HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM;
static char hsu_serial_number[HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN] = HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER;
static boolean hsu_is_serial_number_null = TRUE;

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_SET_PRODUCT_ID

DESCRIPTION
This function updates the product id in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_product_id(unsigned short product_id)
{
  hsu_product_id = product_id;
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_GET_PRODUCT_ID

DESCRIPTION
This function returns the product id stored in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
The product_id (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/
unsigned short hsu_modem_proc_services_get_product_id(void)
{
  return hsu_product_id;
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_SET_SERIAL_NUMBER

DESCRIPTION
This function updates the serial number in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_serial_number(const char* serial_number)
{
  if (serial_number != NULL)
  {
    (void)std_strlcpy(hsu_serial_number, serial_number, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
  }
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_GET_SERIAL_NUMBER

DESCRIPTION
This function returns the serial number stored in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
The serial number (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/
char* hsu_modem_proc_services_get_serial_number(void)
{
  return hsu_serial_number;
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_SET_IS_SERIAL_NUMBER_NULL

DESCRIPTION
This function updates the is serial number null boolean stored in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
None

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_is_serial_number_null(boolean is_serial_number_null)
{
  hsu_is_serial_number_null = is_serial_number_null;
}

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
boolean hsu_modem_proc_services_is_serial_number_null(void)
{
  return hsu_is_serial_number_null;
}

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
boolean hsu_modem_proc_services_init(void)
{
#ifdef FEATURE_EXPORT_HSU_PROC_COMM_APIS
  return hsu_register_pcil_cmds();
#else
  return TRUE;
#endif /* FEATURE_EXPORT_HSU_PROC_COMM_APIS */
}


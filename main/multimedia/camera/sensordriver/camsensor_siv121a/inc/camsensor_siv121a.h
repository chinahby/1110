#ifndef CAMSENSOR_SIV121A_H
#define CAMSENSOR_SIV121A_H

/*============================================================================

                 Camera Interface Device Driver Source File

   DESCRIPTION
     This file contains the definitions needed for the camera interface
     device driver.

============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 10/06/29 wangliang    Initial Version


============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "camsensor.h"
#include "camctrl.h"

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_INIT

DESCRIPTION
              Initialize the camera sensor;

DEPENDENCIES
  None

RETURN VALUE
  if successful
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
uint8 camsensor_SIV121A_ycbcr_active(void);
uint8 camsensor_SIV121A_ycbcr_unactive(void);

boolean camsensor_siv121a_init(camsensor_function_table_type *camsensor_function_table_ptr,
                               camctrl_tbl_type              *camctrl_tbl_ptr);

#endif /* CAMSENSOR_SIV100B_H */



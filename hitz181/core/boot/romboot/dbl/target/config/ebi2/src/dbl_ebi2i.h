#ifndef DBL_EBI2I_H
#define DBL_EBI2I_H

/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                            H e a d e r  F i l e
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/config/ebi2/dbl_ebi2i.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*=========================================================================== 
                           INCLUDE FILES
===========================================================================*/
#include "dbl_flash_if.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/


/*===========================================================================

FUNCTION  DBL_EBI2_CONFIGURE_NOR

DESCRIPTION
 This function will configure the controller on the EBI2 interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_ebi2_configure_nor
(
  dbl_flash_device_driver_type *flash_if
);

/*===========================================================================

FUNCTION  DBL_EBI2_CONFIGURE_MDOC

DESCRIPTION
 This function will configure the controller on the EBI2 interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_ebi2_configure_mdoc
(
  dbl_flash_device_driver_type *flash_if
);

/*===========================================================================

FUNCTION  DBL_EBI2_CONFIGURE_ONENAND

DESCRIPTION
 This function will configure the controller on the EBI2 interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_ebi2_configure_onenand
(
  dbl_flash_device_driver_type *flash_if
);

/*===========================================================================

FUNCTION  DBL_EBI2_CONFIGURE_NAND

DESCRIPTION
 This function will configure the controller on the EBI2 interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_ebi2_configure_nand
(
  dbl_flash_device_driver_type *flash_if
);

#endif  /* DBL_EBI2I_H */



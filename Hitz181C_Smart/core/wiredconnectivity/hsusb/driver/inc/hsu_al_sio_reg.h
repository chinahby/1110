#ifndef _HSU_SIO_REG_H_
#define _HSU_SIO_REG_H_

/*==============================================================================

High Speed USB SIO functions registration 
H E A D E R   F I L E

GENERAL DESCRIPTION
This module is responsible for registering HSUSB driver into SIO.

EXTERNALIZED FUNCTIONS
hsu_sio_reg_hsusb_driver

INITALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_al_sio_reg.h#1 $
$DateTime: 2009/02/04 04:55:04 $ $Author: smalichi $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
02/04/09  sm      inital varsion

==============================================================================*/
#include "sio_priv.h"
#include "sio.h"

/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
Constants
==============================================================================*/

#define SIO_MAJOR_HSUSB_EEM 8
#define SIO_MINOR_HSUSB_EEM 2
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

FUNCTION          HSU_SIO_REG_HSUSB_DRIVER

DESCRIPTION   
This function responsible for mapping HSUSB driver functions to SIO.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
void hsu_al_sio_reg_hsusb_driver (sio_register_device_driver_cb_type register_driver_fn);

#endif /* _HSU_SIO_REG_H_ */

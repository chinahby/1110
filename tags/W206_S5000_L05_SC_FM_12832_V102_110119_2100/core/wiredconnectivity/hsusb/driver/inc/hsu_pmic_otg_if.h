#ifndef HSU_PMIC_OTG_IF_H
#define HSU_PMIC_OTG_IF_H

/*===========================================================================

hsu_pmic_otg_if.h

DESCRIPTION :                                       

High Speed USB PMIC inter operation definition.

This file contains call back funcion implementation of PMIC for HS USB

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

EDIT HISTORY FOR FILE

$Header:
$DateTime:
=============================================================================
when      who  what, where, why
--------  ---  -------------------------------------------------------------
06/04/08  tk   File created. 

---------------------------------------------------------------------------*/

/*==============================================================================

FUNCTION hsu_pmic_otg_if_init_vbus_handler

DESCRIPTION
  This function registers the VBUS interrupt handlers and set up the intial state of D+/D-.

DEPENDENCIES
  For polling mode operation, the function only configures the D+/D- line as FS mode. And the status of D+/
  D- will not change.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void hsu_pmic_otg_if_init(void);

#endif /* !HSU_PMIC_OTG_IF_H */


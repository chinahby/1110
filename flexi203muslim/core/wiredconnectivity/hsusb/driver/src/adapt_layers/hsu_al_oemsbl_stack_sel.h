#ifndef HSU_AL_OEMSBL_STACK_SEL_H
#define HSU_AL_OEMSBL_STACK_SEL_H

/*==============================================================================

HSU_AL_OEMSBL_STACK_SEL

GENERAL DESCRIPTION
Dispatches the correct obex_init function, according to selected stack mode,
DLOAD or FAST_ENUMERATION. 
The obex_init function is called by Jungo OBEX FD.

EXTERNALIZED FUNCTIONS
hsu_al_oemsbl_stack_sel_set_mode
obex_init

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_oemsbl_stack_sel.h#1 $
$DateTime: 2008/04/09 08:54:32 $ $Author: ablay $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
04/06/08  ab      Created.
==============================================================================*/

#ifndef BUILD_HOSTDL

#include "comdef.h"

/*==============================================================================
Typedefs
==============================================================================*/
typedef enum
{   
  HSU_AL_OEMSBL_STACK_MODE_DLOAD,         /* Software Download */
  HSU_AL_OEMSBL_STACK_MODE_FAST_ENUM,     /* Fast enumeration */
  HSU_AL_OEMSBL_STACK_MODE_LAST
} hsu_al_oemsbl_stack_mode_enum;

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_oemsbl_stack_sel_set_mode

DESCRIPTION
Set the stack mode to one of the two: DLOAD for SW DL, or FAST_ENUMERATION 
for fast enumeration for data cards feature.

DEPENDENCIES
Feature flag FEATURE_HS_USB_FAST_ENUMERATION should be defined in order to use
FAST_ENUMERATION.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_al_oemsbl_stack_sel_set_mode(hsu_al_oemsbl_stack_mode_enum stack_mode);

#endif /* !BUILD_HOSTDL */
#endif /* HSU_AL_OEMSBL_STACK_SEL_H */

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

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_oemsbl_stack_sel.c#3 $
$DateTime: 2008/04/30 23:55:21 $ $Author: omanos $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
04/06/08  ab      Created.
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/

#ifndef BUILD_HOSTDL

#include "comdef.h"
#include "hsu_common.h"
#include "hsu_al_oemsbl_stack_sel.h"
#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"
#include "jerrno.h"
#include "hsu_al_dload.h"

#ifdef FEATURE_HS_USB_FAST_ENUMERATION
#include "hsu_al_fast_enum.h"
#endif /* FEATURE_HS_USB_FAST_ENUMERATION */

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/

/* Default value of OEMSBL HS-USB stack, if not set by 
   hsu_al_oemsbl_stack_sel_set_mode, is DLOAD stack. */
static hsu_al_oemsbl_stack_mode_enum 
  hsu_al_oemsbl_stack_mode = HSU_AL_OEMSBL_STACK_MODE_DLOAD;

/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

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
void hsu_al_oemsbl_stack_sel_set_mode(hsu_al_oemsbl_stack_mode_enum stack_mode)
{
    hsu_al_oemsbl_stack_mode = stack_mode;
}

/*===========================================================================

FUNCTION obex_init

DESCRIPTION
OBEX initialization function - called by the OBEX function driver
on initialization. Delegate the call to the correct obex_init function, 
according to stack state (DLOAD or FAST_ENUMERATION).

DEPENDENCIES
None.

RETURN VALUE
0 for success, error number for failure.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t obex_init(cdc_handle_t handle, 
                    obex_callbacks_t* cbs, 
                    obex_init_info_t* info, 
                    cdc_appctx_t* ctx)
{
  switch (hsu_al_oemsbl_stack_mode)
  {
     case HSU_AL_OEMSBL_STACK_MODE_DLOAD:
       return hsu_al_dload_obex_init(handle, cbs, info, ctx);
  
#ifdef FEATURE_HS_USB_FAST_ENUMERATION
     case HSU_AL_OEMSBL_STACK_MODE_FAST_ENUM:
       return hsu_al_fast_enum_obex_init(handle, cbs, info, ctx);
#endif /* FEATURE_HS_USB_FAST_ENUMERATION */

     default:
       HSU_ASSERT("hsu_al_oemsbl_stack_sel: Stack mode not supported" == NULL);
       return JEINVAL;
  }
}

#else /* BUILD_HOSTDL */
#include "hsu_donothing.h"
#endif /* BUILD_HOSTDL */

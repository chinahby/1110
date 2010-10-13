#ifndef HSU_AL_FAST_ENUM_H
#define HSU_AL_FAST_ENUM_H

/*==============================================================================

High Speed USB FAST ENUMERATION Adaptation Layer

GENERAL DESCRIPTION
Implements enumeration in OEMSBL as degenerated OBEX. Defines externalized 
functions for the use of boot module. 

EXTERNALIZED FUNCTIONS

hsu_al_fast_enum_init
hsu_al_fast_enum_shutdown
hsu_al_fast_enum_poll
hsu_al_fast_enum_obex_init

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_al_fast_enum.h#1 $
$DateTime: 2008/04/09 08:54:32 $ $Author: ablay $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
04/06/08  ab      Created.
==============================================================================*/

#ifndef BUILD_HOSTDL

#include "comdef.h"
#include "jtypes.h"
#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_fast_enum_init

DESCRIPTION
This function initializes HS-USB support for FAST ENUMERATION. It starts the
HS-USB stack with one OBEX interface.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_al_fast_enum_init(void);

/*===========================================================================

FUNCTION hsu_al_dload_shutdown

DESCRIPTION
This function shuts down the HS-USB stack, and disconnects from the host.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_fast_enum_shutdown(void);

/*===========================================================================

FUNCTION hsu_al_fast_enum_poll

DESCRIPTION
This function uses OS abstraction functions in order to poll for interrupts, 
and call the ISR and DSR if an interrupt occurred.

DEPENDENCIES
None.

RETURN VALUE
FALSE - Enumeration has not been completed yet.
TRUE  - Enumeration has been completed.   

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_fast_enum_poll(void);

/*===========================================================================

FUNCTION hsu_al_fast_enum_obex_init

DESCRIPTION
OBEX initialization function - called by hsu_al_oemsbl_stack_sel, which is 
called by the function driver on initialization, sets application context 
and callbacks.

DEPENDENCIES
None.

RETURN VALUE
0 for success, error number for failure.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t hsu_al_fast_enum_obex_init(cdc_handle_t handle, 
                                     obex_callbacks_t* cbs, 
                                     obex_init_info_t* info, 
                                     cdc_appctx_t* ctx);

#endif /* !BUILD_HOSTDL */
#endif /* HSU_AL_FAST_ENUM_H */

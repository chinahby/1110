#ifndef _OEMCALL_H_
#define _OEMCALL_H_
/*=============================================================================

FILE: OEMCall.h

SERVICES: Functions to compose call params for AMSS callmanager

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright (c) 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "OEMFeatures.h"
#include "BREWVersion.h"
#include "cm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
FUNCTION: OEMCall_compose_setup_params

DESCRIPTION:  
  Compose call setup params suitable to accept voice calls.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCall_compose_setup_params(cm_setup_res_params_s_type *setup_params_ptr);

/*=============================================================================
FUNCTION: OEMCall_compose_orig_params

DESCRIPTION:  
  compose call orig params suitable for voice calls.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCall_compose_orig_params(cm_gw_cs_orig_params_s_type *orig_params_ptr);

#ifdef __cplusplus
}
#endif

#endif /* !_OEMCALL_H_ */


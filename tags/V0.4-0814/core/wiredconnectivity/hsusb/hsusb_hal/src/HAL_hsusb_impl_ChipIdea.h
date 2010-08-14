#ifndef HAL_HSUSB_IMPL_CHIPIDEA
#define HAL_HSUSB_IMPL_CHIPIDEA
/*
===========================================================================

FILE:         HAL_hsusb_impl_ChipIdea.h

DESCRIPTION:  
This is the external interface for the HS-USB ChipIdea core implementation.

===========================================================================

===========================================================================
Copyright © 2008 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INTERFACES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl_Default.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           INTERFACES DESCRIPTION
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

void HAL_hsusb_ConstructImplChipIdea(struct HAL_HSUSB_CoreIfType* this);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HSUSB_IMPL_CHIPIDEA */

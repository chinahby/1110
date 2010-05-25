/********************************************************************************
**                                                                              *
**  Name        bcm2045_prm_int.h                                               *
**                                                                              *
**  Function    This file contains patch ram loader header file                 *
**                                                                              *
**                                                                              *
**  Copyright (c) 1999-2007, Broadcom Inc., All Rights Reserved.                *
**  Proprietary and confidential.                                               *
**                                                                              *
*********************************************************************************/


#ifndef BRM2045_PRM_INT_H
#define BRM2045_PRM_INT_H

#include "bt_types.h"
#include "bt_target.h"
#include "gki.h"
#include "bcm2045_prm_api.h"


/* Define a structure for 2045 PRM Control Block
*/
typedef struct
{
    tBCM2045_PRM_STATE  state;                /* download state */
    BOOLEAN             internal_patch;       /* internal patch or not */
    UINT16              patch_length;         /* total patch length need to download*/
    const UINT8*        p_patch_data;         /* pointer to orginal patch data */
    UINT16              tx_patch_length;      /* patch length send to controller */
    tBCM2045_PRM_CBACK  *p_cb;                /* call back for patch ram status */
    UINT32              dest_ram;
#if !defined(BCM2045_REGISTER_TIMER) && (BCM2045_USE_DELAY == FALSE)
    /* This only applies when we use btu timer mechanism */
    TIMER_LIST_ENT      timer;
#endif
} tBCM2045_PRM_CB;

extern const int bcm2045_patch_ram_length;
extern const UINT8 bcm2045_patchram[];
extern const char bcm2045_patch_version[];

#endif


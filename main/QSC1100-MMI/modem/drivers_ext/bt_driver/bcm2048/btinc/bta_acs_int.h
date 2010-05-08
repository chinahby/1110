/*****************************************************************************
**
**  Name:           bta_acs_int.h
**
**  Description:    This is the private header file for the Advanced Camera
**                  Server (ACS).
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_ACS_INT_H
#define BTA_ACS_INT_H

#include "bt_target.h"
#include "bta_sys.h"

#include "bta_ac_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/



/* ACS control block */
typedef struct
{
    UINT8               bi_hdl;     /* handle to BTA BIS */
    tBTA_ACS_CBACK     *p_cback;   /* the callback from application */
} tBTA_ACS_CB;

/*****************************************************************************
**  Global data
*****************************************************************************/

/* ACC control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_ACS_CB  bta_acs_cb;
#else
extern tBTA_ACS_CB *bta_acs_cb_ptr;
#define bta_acs_cb (*bta_acs_cb_ptr)
#endif


/*****************************************************************************
**  Function prototypes
*****************************************************************************/
extern void bta_acs_bi_cback(tBTA_BIS_EVT event, tBTA_BIS *p_data);

#endif /* BTA_ACS_INT_H */


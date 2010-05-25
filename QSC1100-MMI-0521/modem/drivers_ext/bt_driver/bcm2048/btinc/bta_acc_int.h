/*****************************************************************************
**
**  Name:           bta_acc_int.h
**
**  Description:    This is the private header file for the Advanced Camera
**                  Client (ACC).
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_ACC_INT_H
#define BTA_ACC_INT_H

#include "bt_target.h"
#include "bta_sys.h"

#include "bta_ac_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/



/* ACC control block */
typedef struct
{
    UINT8               bi_hdl;     /* handle to BTA BIC */
    tBTA_ACC_CBACK     *p_cback;    /* the callback from application */
} tBTA_ACC_CB;

/*****************************************************************************
**  Global data
*****************************************************************************/

/* ACC control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_ACC_CB  bta_acc_cb;
#else
extern tBTA_ACC_CB *bta_acc_cb_ptr;
#define bta_acc_cb (*bta_acc_cb_ptr)
#endif


/*****************************************************************************
**  Function prototypes
*****************************************************************************/
extern void bta_acc_bi_cback(tBTA_BIC_EVT event, tBTA_BIC *p_data);

#endif /* BTA_ACC_INT_H */

/*****************************************************************************
**
**  Name:           bta_sys_int.h
**
**  Description:    This is the private interface file for the BTA system
**                  manager.
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_SYS_INT_H
#define BTA_SYS_INT_H

#if (BTU_STACK_LITE_ENABLED == FALSE)
#include "ptim.h"
#endif

/*****************************************************************************
**  Constants and data types
*****************************************************************************/



/* system manager control block */
typedef struct
{
#if (BTU_STACK_LITE_ENABLED == FALSE)
    tBTA_SYS_REG            *reg[BTA_ID_MAX];       /* registration structures */
    BOOLEAN                 is_reg[BTA_ID_MAX];     /* registration structures */
    tPTIM_CB                ptim_cb;                /* protocol timer list */
    BOOLEAN                 timers_disabled;        /* TRUE if sys timers disabled */
#endif
    UINT8                   task_id;                /* GKI task id */
#if (BTU_STACK_LITE_ENABLED == FALSE)
    tBTA_SYS_CONN_CBACK     *prm_cb;                 /* role management callback registered by DM */
    tBTA_SYS_CONN_CBACK     *ppm_cb;                 /* low power management callback registered by DM */
    tBTA_SYS_CONN_CBACK     *compress_cb;            /* compress callback registered by DM */
#if (BTM_LISBON_INCLUDED == TRUE)&&( BTM_EIR_SERVER_INCLUDED == TRUE )&&(BTA_EIR_CANNED_UUID_LIST != TRUE)
    tBTA_SYS_EIR_CBACK      *eir_cb;                /* add/remove UUID into EIR */
#endif
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    tBTA_SYS_SYNC_RESULT_CBACK      *sync_result_cback;
    tBTA_SYS_NOTIFY_REG_CBACK       *notify_reg_cback;
    tBTA_SYS_IPC_REG_CBACK          *ipc_reg_cback;
    tBTA_SYS_SWITCH_STATUS_CBACK    *switch_status_cback;
    tBTA_SYS_SYNC_REG_CBACK         *sync_reg_cback;
#endif
#endif
} tBTA_SYS_CB;

/*****************************************************************************
**  Global variables
*****************************************************************************/

/* system manager control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_SYS_CB bta_sys_cb;
#else
extern tBTA_SYS_CB *bta_sys_cb_ptr;
#define bta_sys_cb (*bta_sys_cb_ptr)
#endif


/* system manager configuration structure */
extern tBTA_SYS_CFG *p_bta_sys_cfg;

 
#endif /* BTA_SYS_INT_H */

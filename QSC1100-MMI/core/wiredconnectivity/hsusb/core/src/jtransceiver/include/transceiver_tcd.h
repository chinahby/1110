/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _TRANSCEIVER_TCD_H_
#define _TRANSCEIVER_TCD_H_

#ifdef CONFIG_JOTG
#include <transceiver_otg.h>
#endif

#include "transceiver_common.h"

typedef void *tcd_ctx_h;

typedef enum {
    TRCV_TYPE_OTG,
    TRCV_TYPE_DEVICE,
    TRCV_TYPE_HOST
} trcv_type_t;

typedef struct {
#ifdef CONFIG_JOTG
    jresult_t (*tcd_srp_detection_enable)(tcd_ctx_h tcd_ctx, jbool_t enable);
    jresult_t (*tcd_vbus_charge)(tcd_ctx_h tcd_ctx, jbool_t charge);
    jresult_t (*tcd_srp_initiate)(tcd_ctx_h tcd_ctx);
    jresult_t (*tcd_state_get)(tcd_ctx_h tcd_ctx, otg_ctrl_t *otg_ctrl);
    jresult_t (*tcd_local_dplus_set)(tcd_ctx_h tcd_ctx, jbool_t enable);
    jresult_t (*tcd_local_dminus_set)(tcd_ctx_h tcd_ctx, jbool_t enable);
    jresult_t (*tcd_vbus_drive)(tcd_ctx_h tcd_ctx, jbool_t drive);
#endif

#ifdef CONFIG_JHOST
    jresult_t (*tcd_host_port_set)(tcd_ctx_h tcd_ctx, jint_t value,
        jbool_t set);
    jresult_t (*tcd_host_port_status_get)(tcd_ctx_h tcd_ctx, juint16_t *status,
        juint16_t *mask);
    jresult_t (*tcd_host_port_change_get)(tcd_ctx_h tcd_ctx, juint16_t *change,
        juint16_t *mask);
#endif

#ifdef CONFIG_JSLAVE
    void (*tcd_device_enable)(tcd_ctx_h tcd_ctx, jbool_t enable);
#endif

    jresult_t (*tcd_enable)(tcd_ctx_h tcd_ctx, trcv_type_t type);
    void (*tcd_disable)(tcd_ctx_h tcd_ctx);
    void (*tcd_mode_set)(tcd_ctx_h tcd_ctx, jbool_t mode);
    void (*tcd_interface_type_get)(tcd_ctx_h tcd_ctx, 
        trcv_interface_type_t *type);
} tcd_ops_t;

void trcv_otg_intr_handle(trcv_ctx_h trcv_core_ctx);
void trcv_host_intr_handle(trcv_ctx_h trcv_core_ctx);
void trcv_device_intr_handle(trcv_ctx_h trcv_core_ctx, juint16_t status);

#endif

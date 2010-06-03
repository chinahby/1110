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
#ifndef _TRANSCEIVER_OTG_H_
#define _TRANSCEIVER_OTG_H_

#include "transceiver_common.h"

typedef struct otg_ctrl_t
{
    unsigned vbus_vld     : 1;
    unsigned sess_vld     : 1;
    unsigned id           : 1;
    unsigned b_sess_vld   : 1;
    unsigned b_sess_end   : 1;
    unsigned srp_detected : 1;
} otg_ctrl_t;

jresult_t trcv_otg_init(jdevice_t otg, trcv_ctx_h *trcv_core_ctx);
void trcv_otg_uninit(trcv_ctx_h trcv_core_ctx);

jresult_t trcv_enable(trcv_ctx_h trcv_core_ctx);
void trcv_disable(trcv_ctx_h trcv_core_ctx);

jresult_t trcv_srp_detection_enable(trcv_ctx_h trcv_core_ctx, jbool_t enable);
jresult_t trcv_vbus_drive(trcv_ctx_h trcv_core_ctx, jbool_t drive);
jresult_t trcv_vbus_charge(trcv_ctx_h trcv_core_ctx, jbool_t charge);
jresult_t trcv_srp_initiate(trcv_ctx_h trcv_core_ctx);
jresult_t trcv_state_get(trcv_ctx_h trcv_core_ctx, otg_ctrl_t *otg_ctrl);
jresult_t trcv_local_dplus_set(trcv_ctx_h trcv_core_ctx, jbool_t enable);
jresult_t trcv_local_dminus_set(trcv_ctx_h trcv_core_ctx, jbool_t enable);

#endif

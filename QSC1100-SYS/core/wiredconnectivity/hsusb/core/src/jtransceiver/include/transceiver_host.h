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
#ifndef _TRANSCEIVER_HOST_H_
#define _TRANSCEIVER_HOST_H_

#include "transceiver_common.h"

jresult_t trcv_host_init(jdevice_t host, jint_t port_max_num, 
    trcv_ctx_h *trcv_core_ctx);
void trcv_host_uninit(trcv_ctx_h trcv_core_ctx);

void trcv_host_port_set(trcv_ctx_h trcv_core_ctx, juint8_t index, jint_t value, 
    jbool_t set);
void trcv_host_port_status_get(trcv_ctx_h trcv_core_ctx, juint8_t index,
    juint16_t *status);
void trcv_host_port_change_get(trcv_ctx_h trcv_core_ctx, juint8_t index,
    juint16_t *change);

#endif

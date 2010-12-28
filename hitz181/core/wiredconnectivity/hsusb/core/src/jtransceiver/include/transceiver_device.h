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
#ifndef _TRANSCEIVER_DEVICE_H_
#define _TRANSCEIVER_DEVICE_H_

#include "transceiver_common.h"

jresult_t trcv_device_init(jdevice_t device, trcv_ctx_h *trcv_core_ctx);
void trcv_device_uninit(trcv_ctx_h trcv_core_ctx);

void trcv_device_enable(trcv_ctx_h trcv_core_ctx, jbool_t enable);

#define TRCV_DEVICE_BSESS_VALID 0x0001
#define TRCV_DEVICE_BSESS_END   0x0002

#endif

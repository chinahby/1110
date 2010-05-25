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
#ifndef _TRANSCEIVER_COMMON_H_
#define _TRANSCEIVER_COMMON_H_

typedef void *trcv_ctx_h;

#define TRCV_MODE_HOST   0 
#define TRCV_MODE_DEVICE 1 
void trcv_mode_set(trcv_ctx_h trcv_core_ctx, jbool_t mode);

typedef enum {
    TRCV_INTRFACE_TYPE_DEFAULT,
    TRCV_INTRFACE_TYPE_UTMI,
    TRCV_INTRFACE_TYPE_ULPI,
    TRCV_INTRFACE_TYPE_SERIAL
} trcv_interface_type_t;
void trcv_interface_type_get(trcv_ctx_h trcv_core_ctx, 
    trcv_interface_type_t *type);

#endif

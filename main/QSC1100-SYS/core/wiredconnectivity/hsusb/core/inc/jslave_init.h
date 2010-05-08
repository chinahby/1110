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
#ifndef _JSLAVE_INIT_H_
#define _JSLAVE_INIT_H_

#include <jos.h>

/* Initialize the USB stack */
jresult_t jslave_init(uw_args_t *args);

/* Enable stack */
jresult_t jslave_enable(juint8_t core_number);

/* Disable stack */
void jslave_disable(juint8_t core_number);

/* Re-Enable the stack with different function driver configuration */
typedef void (*reconfig_complete_cb_t)(void *arg, jresult_t status);
jresult_t jslave_reconfig(juint8_t core_number, device_params_t *new_config,
    reconfig_complete_cb_t cb, void *arg);

/* Causes complete un-initialization, will require init() before used again */
void jslave_uninit(void);

/* Allows for remote wakeup */
jresult_t jslave_wakeup_host(juint8_t core_number);
juint8_t jcore_get_curr_configuration(juint8_t core_number);
#endif


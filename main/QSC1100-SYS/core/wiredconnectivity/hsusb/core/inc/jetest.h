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
#ifndef _JETEST_H_
#define _JETEST_H_

#include <usbdi.h>

typedef enum {
    TEST_MODE_DISABLE = 0,
    TEST_MODE_J_STATE,
    TEST_MODE_K_STATE,
    TEST_MODE_SE0NAK,
    TEST_MODE_PACKET,
    TEST_MODE_FORCE_ENABLE,
    TEST_MODE_SUSPEND_RESUME,
    TEST_MODE_SINGLE_STEP_SET_FEATURE
} etest_mode_t;

typedef struct
{
    usbd_device_handle test_dev;
    etest_mode_t mode;
} etest_param_t;

#endif

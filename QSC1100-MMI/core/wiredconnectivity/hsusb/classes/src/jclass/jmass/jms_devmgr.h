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
#ifndef _JMS_DEVMGR_H_
#define _JMS_DEVMGR_H_

#define MAX_POSSIBLE_LUNS 9

#include "jms_scsi.h"

void device_lock(void *devmgr);
void device_unlock(void *devmgr);

jresult_t jms_init(void);
void jms_uninit(void);

jresult_t jms_devmgr_suspend_device(ms_device_t *device);
jresult_t jms_devmgr_resume_device(ms_device_t *device);
    
#endif


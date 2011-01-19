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
#ifndef _JMS_TRANSPORT_H_
#define _JMS_TRANSPORT_H_

/* Functions for use by upper layer */
jresult_t jms_transfer(void *dev_h, juint8_t lun, 
    juint8_t *cmd, jint_t cmdlen, jint_t is_read, juint32_t timeout, 
    void *data, juint32_t data_len, juint8_t flags, juint32_t *actual_data_len);

/* Transport calls these functions on device attach/detach */
jresult_t jms_devmgr_attach(void *transport, void *usbd_dev, juint8_t maxlun, 
    juint8_t subclass, void **context);

void jms_devmgr_detach(void *context);

/* Device power management */
jresult_t jms_devmgr_suspend(void *context);
void jms_devmgr_resume(void *context);

jresult_t jms_transport_suspend_self(void *dev_h);
jresult_t jms_transport_resume_self(void *dev_h);

#endif


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
#ifndef __OTG_CORE_DEVICE_H__
#define __OTG_CORE_DEVICE_H__

/* The Device Controller must register itself with the OTG Core during attach */
jresult_t jotg_register_device(jdevice_t device, jdevice_t *otg);

/* The Device Controller must unregister itself from the OTG Core before the OCD
 * is unregistered  */
jresult_t jotg_unregister_device(jdevice_t otg);

#endif

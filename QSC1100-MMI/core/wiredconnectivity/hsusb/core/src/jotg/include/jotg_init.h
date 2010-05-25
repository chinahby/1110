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

/* *  Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com          */
#ifndef _J_OTG_INIT_H_
#define _J_OTG_INIT_H_

jresult_t jotg_init(uw_args_t *args);
void jotg_uninit(void);

#endif

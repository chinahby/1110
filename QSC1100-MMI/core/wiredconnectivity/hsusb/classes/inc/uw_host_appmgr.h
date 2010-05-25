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
#ifndef _UW_HOST_APPMGR_H_
#define _UW_HOST_APPMGR_H_

#include <jtypes.h>
#include <dev_monitor.h>

/* Application Registration Handle */
typedef void *uwh_app_h;

/* Application Probe Method */
typedef juint_t (*uwh_app_probe_cb_t)(devmon_info_t *desc);

#endif


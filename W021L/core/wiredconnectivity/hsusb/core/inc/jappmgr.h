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
#ifndef _APP_MGR_H_
#define _APP_MGR_H_

#include <jtypes.h>
#include <uw_host_appmgr.h>

typedef void *appmgr_h;
typedef void *app_cbs_h;

uwh_app_h appmgr_add(appmgr_h *appmgr, app_cbs_h cbs, uwh_app_probe_cb_t probe);

void appmgr_remove(appmgr_h *appmgr, uwh_app_h app_h);

app_cbs_h appmgr_probe(appmgr_h appmgr, mdev_t dev);

#endif


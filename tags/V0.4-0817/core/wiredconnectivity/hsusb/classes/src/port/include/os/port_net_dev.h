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

#ifndef _PORT_NET_DEV_H_
#define _PORT_NET_DEV_H_

#include <jtypes.h>

typedef void *netdev_h;
typedef void *app_ctx_h;

#ifdef CONFIG_JHOST_WIFI
#include "port_net_wifi.h"
#endif

#define UW_MAX_ETHER_SIZE 1514
#define UW_MAC_ADDR_LEN 6

struct net_macaddr {
    juint8_t bytes[UW_MAC_ADDR_LEN];
};

typedef void (*net_complete_cb_t)(netdev_h dev, void *net_priv, 
    void *buffer);
typedef void (*net_transfer_cb_t)(app_ctx_h app_ctx, void *buffer, 
    juint32_t size, jresult_t status, void *priv);

typedef struct {
    jresult_t (*tx)(app_ctx_h app_ctx, void *buffer, juint32_t size, 
        net_complete_cb_t cb, void *net_priv);
    void (*suspend)(app_ctx_h app_ctx);
    jresult_t (*resume)(app_ctx_h app_ctx);
    void (*abort_tx)(app_ctx_h app_ctx);
#ifdef CONFIG_JHOST_WIFI
    uwh_wifi_extention_cb_t *wifi_ext;
#endif
} net_app_ops_t;

jresult_t net_rx_complete(netdev_h dev, void *buffer, juint32_t size, 
    net_transfer_cb_t cb, void *priv);

jresult_t net_attach(app_ctx_h app_ctx, net_app_ops_t *ops, 
    struct net_macaddr *mac_addr, netdev_h *dev);
void net_detach(netdev_h dev);

jresult_t net_enable(netdev_h dev);
void net_disable(netdev_h dev);

void net_enable_tx(netdev_h dev);
void net_disable_tx(netdev_h dev);

jresult_t net_dev_init(void);
void net_dev_uninit(void);

#endif /*_PORT_NET_DEV_H_*/

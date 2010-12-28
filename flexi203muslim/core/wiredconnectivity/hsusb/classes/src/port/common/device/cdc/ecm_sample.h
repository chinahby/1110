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

#ifndef _JSLAVE_PORT_NET_
#define _JSLAVE_PORT_NET_

/*
 * Terminology:
 *
 * Upper layer - networking layer (net_dev.c)
 * Lower layer - the bus registering with us (e.g. USB)
 * 
 */

typedef void* netdev_h;
typedef void* lower_ctx_h;

struct net_macaddr
{
    juint8_t bytes[6];
};

/* Lower layer operations (used by networking layer) */
typedef struct
{
    jint_t (*rx)(lower_ctx_h ctx, void *buf, juint32_t size);
    jint_t (*tx)(lower_ctx_h ctx, void *buf, juint32_t size);
    jint_t (*abort_rx)(lower_ctx_h ctx);
    jint_t (*abort_tx)(lower_ctx_h ctx);
} net_lower_ops_t;

/* Network upper layer API */
jint_t net_bind(netdev_h *dev, net_lower_ops_t *cbs, struct net_macaddr*,
    lower_ctx_h lower_ctx);
void net_unbind(netdev_h dev);
void net_disable(netdev_h dev);
void net_enable(netdev_h dev);
jint_t net_tx_complete(netdev_h dev, jint_t status);
jint_t net_rx_complete(netdev_h dev, juint32_t size, jint_t status);

enum net_stats
{
    NET_STATS_RX_PACKETS, NET_STATS_RX_BYTES, NET_STATS_RX_ERRORS,
    NET_STATS_TX_PACKETS, NET_STATS_TX_BYTES, NET_STATS_TX_ERRORS
};

juint32_t net_get_stats(netdev_h dev, enum net_stats stat);

#endif /*_JSLAVE_PORT_NET_*/

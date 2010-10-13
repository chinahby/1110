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
#define LINUX24
#define ERRORNO_INCLUDED
#include "includes.h"
#include <jos.h>
#include <jconfig.h>
#include <kernel.h>
#include <byteorder.h>
#include <io.h>
#include <irq.h>
#include <system.h>
#include <uaccess.h>
#include <unaligned.h>
#include <netdevice.h>
#include <etherdevice.h>
#include <ethtool.h>
#include <wireless.h>
#include <if_arp.h>
#include <iw_handler.h>

#include <port_net_dev.h>

#define MIN_PACKET sizeof(struct ethhdr)
#define MAX_PACKET 1500
#define ETH_HLEN 14

enum os_net_stats {
    NET_STATS_RX_PACKETS, NET_STATS_RX_BYTES, NET_STATS_RX_ERRORS,
    NET_STATS_TX_PACKETS, NET_STATS_TX_BYTES, NET_STATS_TX_ERRORS
};

typedef struct
{
    struct net_device_stats stats;

    /* lower layer */
    app_ctx_h app_ctx;
    net_app_ops_t app_ops;
    jbool_t opened;
    
#ifdef CONFIG_JHOST_WIFI
    jbool_t scanning;
#endif
} net_ctx_t;

enum net_action_type {
    NET_ACTION_TYPE_TX,
    NET_ACTION_TYPE_ABORT,
};

typedef struct {
    struct sk_buff *skb;
    struct net_device *net;
    jbool_t done;
    enum net_action_type type;
} jtask_args_t;

static jtask_h transfer_task = NULL;
static jtask_args_t jtask_args_array[CONFIG_MAX_NET_TRANSFERS];
static jint_t curr_free_possition = 0;
static jbool_t pending_abort = 0;

static void perform_transfer(void *arg);

#ifdef CONFIG_JHOST_WIFI
static int wifi_net_dev_ioctl_giwname(struct net_device *net_dev,
    struct iw_request_info *info, char *name, char *extra)
{
    jresult_t rc = 0;
    net_ctx_t *ctx = (net_ctx_t*)net_dev->priv;
    uwh_wifi_extention_cb_t *wifi_ext = ctx->app_ops.wifi_ext;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_giwname: Enter\n"));

    if (!wifi_ext || !wifi_ext->uwh_wifi_get_vendor_name)
        return -JEINVAL;

    rc  = wifi_ext->uwh_wifi_get_vendor_name(ctx->app_ctx, (juint8_t *)name);

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_giwname: OK\n"));

    return res_uw_to_os(rc);
}

static int wifi_net_dev_ioctl_giwrange(struct net_device *net_dev,
    struct iw_request_info *info, struct iw_point *data, char *extra)
{
    struct iw_range *range = (struct iw_range *) extra;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_giwrange: Enter\n"));

    data->length = sizeof(struct iw_range);
    memset(range, 0, sizeof(struct iw_range));

    range->txpower_capa = IW_TXPOW_DBM;
    range->we_version_compiled = WIRELESS_EXT;
    range->we_version_source = 14;

    range->retry_capa = IW_RETRY_LIMIT;
    range->retry_flags = IW_RETRY_LIMIT;
    range->min_retry = 0;
    range->max_retry = 255;

    range->max_qual.qual = 100; 
    range->max_qual.level = 0;
    range->max_qual.noise = 0;

    range->avg_qual.qual = 20;
    range->avg_qual.level = -60;
    range->avg_qual.noise = -95;
    range->sensitivity = 3;

    range->max_encoding_tokens = 4;
    range->num_encoding_sizes = 2;
    range->encoding_size[0] = 5;
    range->encoding_size[1] = 13;

    range->min_rts = 0;
    range->max_rts = 2347;
    range->min_frag = 256;
    range->max_frag = 2346;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_giwrange: Enter\n"));

    return 0;
}

static int wifi_net_dev_ioctl_siwap(struct net_device *net_dev,
    struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    net_ctx_t *ctx = (net_ctx_t*)net_dev->priv;
    juint8_t bssid[ETH_ALEN];
    jresult_t rc;
    uwh_wifi_extention_cb_t *wifi_ext = ctx->app_ops.wifi_ext;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_siwap: Enter\n"));

    if (wrqu->ap_addr.sa_family != ARPHRD_ETHER)
        return -JEINVAL;

    if (!wifi_ext || !wifi_ext->uwh_wifi_associate_bssid || 
        !wifi_ext->uwh_wifi_disassociate)
    {
        return -JEINVAL;
    }

    memset(bssid, 0, ETH_ALEN);

    if (memcmp(wrqu->ap_addr.sa_data, bssid, ETH_ALEN))
    {
        memcpy(bssid, wrqu->ap_addr.sa_data, ETH_ALEN);

        DBG_I(DPORT_NET, ("wifi_net_dev_ioctl_siwap: Associating with "
            "%02x:%02x:%02x:%02x:%02x:%02x\n", bssid[0], bssid[1], bssid[2], 
            bssid[3], bssid[4], bssid[5]));

        rc = wifi_ext->uwh_wifi_associate_bssid(ctx->app_ctx, bssid);
    }
    else
    {
        DBG_I(DPORT_NET, ("wifi_net_dev_ioctl_siwap: "
            "Disassociating\n"));

        rc = wifi_ext->uwh_wifi_disassociate(ctx->app_ctx);
    }

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_siwap: OK(%d)\n", rc));

    return -1 * res_uw_to_os(rc);
}

static int wifi_net_dev_ioctl_giwap(struct net_device *net_dev,
    struct iw_request_info *info, struct sockaddr *ap_addr, char *extra)
{
    net_ctx_t *ctx = (net_ctx_t*)net_dev->priv;
    jresult_t rc;
    uwh_wifi_extention_cb_t *wifi_ext = ctx->app_ops.wifi_ext;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_giwap: Enter\n"));

    if (!wifi_ext || !wifi_ext->uwh_wifi_get_bssid)
        return -JEINVAL;

    rc = wifi_ext->uwh_wifi_get_bssid(ctx->app_ctx, 
        (juint8_t *)ap_addr->sa_data);

    switch (rc)
    {
    case JENODEV:
        return ENOTCONN;
        break;
    case 0:
        ap_addr->sa_family = ARPHRD_ETHER;
        break;
    default:
        break;
    }

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_giwap: OK(%d)\n", rc));

    return -1 * res_uw_to_os(rc);
}

static int wifi_net_dev_ioctl_siwscan(struct net_device *net_dev,
    struct iw_request_info *info, struct iw_point *data, char *extra)
{
    net_ctx_t *ctx = (net_ctx_t*)net_dev->priv;
    uwh_wifi_extention_cb_t *wifi_ext = ctx->app_ops.wifi_ext;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_siwscan: Enter\n"));

    if (!wifi_ext || !wifi_ext->uwh_wifi_bssid_list_scan)
        return -JEINVAL;

    if (ctx->scanning)
        return 0;
   
    if (wifi_ext->uwh_wifi_bssid_list_scan(ctx->app_ctx))
        return -JEINVAL;
        
    ctx->scanning = 1;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_siwscan: OK\n"));

    return 0;
}

static int wifi_net_dev_ioctl_giwscan(struct net_device *net_dev,
    struct iw_request_info *info, struct iw_point *data, char *extra)
{
    net_ctx_t *ctx = (net_ctx_t*)net_dev->priv;
    int rc = 0;
    uwh_wifi_bss_t bss;
    char *current_ev = extra;
    char *previous_ev = extra;
    char *end_buf = extra + IW_SCAN_MAX_DATA; 
    char *current_val;
    uwh_wifi_extention_cb_t *wifi_ext = ctx->app_ops.wifi_ext;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_siwscan: Enter\n"));

    if (!wifi_ext || !wifi_ext->uwh_wifi_bssid_list_get_first ||
        !wifi_ext->uwh_wifi_bssid_list_get_next)
    {
        return -JEINVAL;
    }

    if (ctx->scanning)
        return -JEAGAIN;

    rc = wifi_ext->uwh_wifi_bssid_list_get_first(ctx->app_ctx, &bss);
    if (rc == JENOENT)
        return 0;

    if (rc)
        return -1 * res_uw_to_os(rc);

    for (;;)
    {
        struct iw_event iwe;

        if (current_ev >= end_buf)
            break;

        /* MAC address */
        memset(&iwe, 0, sizeof(iwe));

        iwe.cmd = SIOCGIWAP;
        iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
        memcpy(iwe.u.ap_addr.sa_data, bss.bssid, ETH_ALEN);

        previous_ev = current_ev;
        current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, 
            IW_EV_ADDR_LEN);

        if (current_ev == previous_ev)
            break;

        /* ESSID */
        memset(&iwe, 0, sizeof(iwe));

        iwe.cmd = SIOCGIWESSID;
        iwe.u.data.length = bss.ssid_len;
        iwe.u.data.flags = 1;

        previous_ev = current_ev;
        current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, 
            (char *)bss.ssid);

        if (current_ev == previous_ev)
            break;

        /* Network Type */
        memset(&iwe, 0, sizeof(iwe));

        iwe.cmd = SIOCGIWMODE;

        switch (bss.bss_type)
        {
        case UWH_WIFI_BSS_ADHOC:
            iwe.u.mode = IW_MODE_ADHOC;
            break;
        case UWH_WIFI_BSS_INFRA:
            iwe.u.mode = IW_MODE_INFRA;
            break;
        default:
            iwe.u.mode = IW_MODE_AUTO;
            break;
        }

        iwe.len = IW_EV_UINT_LEN;

        previous_ev = current_ev;
        current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe,  
            IW_EV_UINT_LEN);

        if (current_ev == previous_ev)
            break;

        /* Channel and Frequency */
        memset(&iwe, 0, sizeof(iwe));

        iwe.cmd = SIOCGIWFREQ;
        iwe.u.freq.m = bss.channel;
        iwe.u.freq.e = 0;
        iwe.u.freq.i = 0;

        previous_ev = current_ev;
        current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe,
            IW_EV_FREQ_LEN);

        if (current_ev == previous_ev)
            break;

        /* Encyption key */
        memset(&iwe, 0, sizeof(iwe));

        iwe.cmd = SIOCGIWENCODE;

        if (bss.is_secured)
            iwe.u.data.flags =IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
        else
            iwe.u.data.flags = IW_ENCODE_DISABLED;

        previous_ev = current_ev;
        current_ev = iwe_stream_add_point(current_ev, end_buf,&iwe,
            (char *)"");

        if (current_ev == previous_ev)
            break;

        /* Bit Rate */
        memset(&iwe, 0, sizeof(iwe));

        iwe.cmd = SIOCGIWRATE;

        current_val = current_ev + IW_EV_LCP_LEN;

        iwe.u.bitrate.value = bss.rate_mbps;
        iwe.u.bitrate.disabled = 0;

        current_val = iwe_stream_add_value(current_ev, current_val, end_buf,
            &iwe, IW_EV_PARAM_LEN);

        if((current_val - current_ev) > IW_EV_LCP_LEN)
            current_ev = current_val;
        else
            break;

        /* Done */
        memset(&iwe, 0, sizeof(iwe));

        rc = wifi_ext->uwh_wifi_bssid_list_get_next(ctx->app_ctx, &bss);
        if (rc == JENOENT)
            break;

        if (rc)
            return -1 * res_uw_to_os(rc);
    }

    data->length = current_ev - extra;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_giwscan: OK\n"));

    return 0;
}

static int wifi_net_dev_ioctl_siwessid(struct net_device *net_dev,
    struct iw_request_info *info, struct iw_point *data, char *essid)
{
    net_ctx_t *ctx = (net_ctx_t*)net_dev->priv;
    jresult_t rc;
    uwh_wifi_extention_cb_t *wifi_ext = ctx->app_ops.wifi_ext;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_siwessid: Enter flags = %08x\n",
        data->flags));

    if (!wifi_ext || !wifi_ext->uwh_wifi_associate_ssid || 
        !wifi_ext->uwh_wifi_disassociate)
    {
        return -JEINVAL;
    }

    if (data->flags)
    {
        DBG_I(DPORT_NET, ("wifi_net_dev_ioctl_siwessid: "
            "Associating with %s\n",  essid));

        rc = wifi_ext->uwh_wifi_associate_ssid(ctx->app_ctx,
            (juint8_t *)essid, (juint8_t)strlen(essid));
    }
    else
    {
        DBG_I(DPORT_NET, ("wifi_net_dev_ioctl_siwessid: "
            "Disassociating with \n"));

        rc = wifi_ext->uwh_wifi_disassociate(ctx->app_ctx);
    }

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_siwessid: OK(%d)\n", rc));

    return -1 * res_uw_to_os(rc);
}

static int wifi_net_dev_ioctl_giwessid(struct net_device *net_dev,
    struct iw_request_info *info, struct iw_point *data, char *essid)
{
    net_ctx_t *ctx = (net_ctx_t*)net_dev->priv;
    juint8_t ssid_len;
    jresult_t rc;
    uwh_wifi_extention_cb_t *wifi_ext = ctx->app_ops.wifi_ext;

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_giwessid: Enter\n"));
    
    if (!wifi_ext || !wifi_ext->uwh_wifi_get_ssid)
        return -JEINVAL;

    data->flags = 1;

    rc = wifi_ext->uwh_wifi_get_ssid(ctx->app_ctx, (juint8_t *)essid, &ssid_len);

    DBG_V(DPORT_NET, ("wifi_net_dev_ioctl_giwessid: OK(%d)\n", rc));

    return -1 * res_uw_to_os(rc);
}

static int wifi_net_dev_ioctl_siwencode(struct net_device *net_dev, 
    struct iw_request_info *info, struct iw_point *data, juint8_t *keybuf)
{
    net_ctx_t *ctx = (net_ctx_t*)net_dev->priv;
    jresult_t rc;
    int index;
    uwh_wifi_extention_cb_t *wifi_ext = ctx->app_ops.wifi_ext;

    DBG_E(DPORT_NET, ("wifi_net_dev_ioctl_siwencode: Enter\n"));
    
    if (!wifi_ext || !wifi_ext->uwh_wifi_wep_set_key)
        return -JEINVAL;

    /* Enable crypto. */
    if (data->length > IFNAMSIZ)
        return -JEINVAL;

    index = (data->flags & IW_ENCODE_INDEX);

    if (index < 0)
        return -JEINVAL;         

    rc = wifi_ext->uwh_wifi_wep_set_key(ctx->app_ctx, index, data->length, 
        keybuf);

    DBG_E(DPORT_NET, ("wifi_net_dev_ioctl_siwencode: OK(%d)\n", rc));

    return -1 * res_uw_to_os(rc);
}

static const iw_handler wifi_net_dev_iw_handler[] =
{
    (iw_handler)NULL,                           /* SIOCSIWCOMMIT */
    (iw_handler)wifi_net_dev_ioctl_giwname,     /* SIOCGIWNAME  1 */
    (iw_handler)NULL,                           /* SIOCSIWNWID */
    (iw_handler)NULL,                           /* SIOCGIWNWID */
    (iw_handler)NULL, // rt_ioctl_siwfreq,      /* SIOCSIWFREQ */
    (iw_handler)NULL, // rt_ioctl_giwfreq,      /* SIOCGIWFREQ  5 */
    (iw_handler)NULL, // rt_ioctl_siwmode,      /* SIOCSIWMODE */
    (iw_handler)NULL, // rt_ioctl_giwmode,      /* SIOCGIWMODE */
    (iw_handler)NULL,                           /* SIOCSIWSENS */
    (iw_handler)NULL,                           /* SIOCGIWSENS */
    (iw_handler)NULL /* not used */,            /* SIOCSIWRANGE */
    (iw_handler)wifi_net_dev_ioctl_giwrange,    /* SIOCGIWRANGE 11 */
    (iw_handler)NULL /* not used */,            /* SIOCSIWPRIV */
    (iw_handler)NULL /* kernel code */,         /* SIOCGIWPRIV */
    (iw_handler)NULL /* not used */,            /* SIOCSIWSTATS */
    (iw_handler)NULL /* kernel code */,         /* SIOCGIWSTATS f */
    (iw_handler)NULL,                           /* SIOCSIWSPY */
    (iw_handler)NULL,                           /* SIOCGIWSPY */
    (iw_handler)NULL,                           /* -- hole -- */
    (iw_handler)NULL,                           /* -- hole -- */
    (iw_handler)wifi_net_dev_ioctl_siwap,       /* SIOCSIWAP */
    (iw_handler)wifi_net_dev_ioctl_giwap,       /* SIOCGIWAP */
    (iw_handler)NULL,                           /* -- hole --   0x16 */
    (iw_handler)NULL, // rt_ioctl_iwaplist,     /* SIOCGIWAPLIST */
    (iw_handler)wifi_net_dev_ioctl_siwscan,     /* SIOCSIWSCAN  0x18*/
    (iw_handler)wifi_net_dev_ioctl_giwscan,     /* SIOCGIWSCAN */
    (iw_handler)wifi_net_dev_ioctl_siwessid,    /* SIOCSIWESSID */
    (iw_handler)wifi_net_dev_ioctl_giwessid,    /* SIOCGIWESSID */
    (iw_handler)NULL, // rt_ioctl_siwnickn,     /* SIOCSIWNICKN */
    (iw_handler)NULL, // rt_ioctl_giwnickn,     /* SIOCGIWNICKN 1d*/
    (iw_handler)NULL,                           /* -- hole -- */
    (iw_handler)NULL,                           /* -- hole -- */
    (iw_handler)NULL,                           /* SIOCSIWRATE  20*/
    (iw_handler)NULL,                           /* SIOCGIWRATE */
    (iw_handler)NULL, // rt_ioctl_siwrts,       /* SIOCSIWRTS */
    (iw_handler)NULL, // rt_ioctl_giwrts,       /* SIOCGIWRTS */
    (iw_handler)NULL, // rt_ioctl_siwfrag,      /* SIOCSIWFRAG */
    (iw_handler)NULL, // rt_ioctl_giwfrag,      /* SIOCGIWFRAG  25*/
    (iw_handler)NULL,                           /* SIOCSIWTXPOW */
    (iw_handler)NULL,                           /* SIOCGIWTXPOW */
    (iw_handler)NULL,                           /* SIOCSIWRETRY */
    (iw_handler)NULL,                           /* SIOCGIWRETRY  29*/
    (iw_handler)wifi_net_dev_ioctl_siwencode,   /* SIOCSIWENCODE 2a*/
    (iw_handler)NULL, // rt_ioctl_giwencode,    /* SIOCGIWENCODE 2b*/
    (iw_handler)NULL,                           /* SIOCSIWPOWER  2c*/
    (iw_handler)NULL,                           /* SIOCGIWPOWER  2d*/
};

static struct iw_handler_def wifi_net_dev_iw_handlers = 
{
    .standard = (iw_handler *)wifi_net_dev_iw_handler,
    .num_standard = sizeof(wifi_net_dev_iw_handler) / sizeof(iw_handler),
};

void net_scan_finished(netdev_h dev, jresult_t status)
{
    struct net_device *netdev = (struct net_device*)dev;
    net_ctx_t *ctx = (net_ctx_t*)netdev->priv;

    ctx->scanning = 0;
}

#endif

juint32_t net_get_stats(netdev_h dev, enum os_net_stats stat)
{
    struct net_device *netdev = (struct net_device*)dev;
    net_ctx_t *ctx = (net_ctx_t*)netdev->priv;
    
    switch(stat)
    {
    case NET_STATS_RX_PACKETS:
        return ctx->stats.rx_packets;
    case NET_STATS_RX_BYTES:
        return ctx->stats.rx_bytes;
    case NET_STATS_RX_ERRORS:
        return ctx->stats.rx_errors;
    case NET_STATS_TX_PACKETS:
        return ctx->stats.tx_packets;
    case NET_STATS_TX_BYTES:
        return ctx->stats.tx_bytes;
    case NET_STATS_TX_ERRORS:
        return ctx->stats.tx_errors;
    default:
        return 0;
    }
}

/*==============================================================*/
/*      L I N U X   N E T W O R K I N G   L A Y E R             */
/*==============================================================*/

/*
 * Linux netif (upper) interface
 */
static jint_t eth_change_mtu(struct net_device *net, jint_t new_mtu)
{
    DBG_X(DPORT_NET, ("eth_change_mtu: entered (cur %u)\n", net->mtu));
    if (new_mtu <= MIN_PACKET || new_mtu > MAX_PACKET)
        return -ERANGE;

    DBG_X(DPORT_NET, ("eth_change_mtu: new mtu %u\n", new_mtu));
    net->mtu = new_mtu;
    return 0;
}

static struct net_device_stats *eth_get_stats(struct net_device *net)
{
    DBG_X(DPORT_NET, ("eth_get_stats: entered\n"));
    net_ctx_t *ctx = (net_ctx_t*)net->priv;
    return &ctx->stats;
}

static jint_t eth_ioctl(struct net_device *net, struct ifreq *rq, jint_t cmd)
{
    DBG_X(DPORT_NET, ("eth_ioctl: entered\n"));
    switch (cmd)
    {
    default:
        return -JEOPNOTSUPP;
    }
}

jresult_t net_rx_complete(netdev_h dev, void *buffer, juint32_t size, 
    net_transfer_cb_t cb, void *priv)
{
    struct net_device *netdev = (struct net_device*)dev;
    net_ctx_t *ctx = (net_ctx_t*)netdev->priv;
    jint_t rc = 0;
    struct sk_buff *rx_skb;

    DBG_X(DPORT_NET, ("net_rx_complete: entered size is %ld\n", size));
    
    if (!ctx)
    {
        DBG_E(DPORT_NET, ("net_rx_complete: Invalid context\n"));
        return JEIO;
    }

    if (!ctx->opened)
    {
        DBG_W(DPORT_NET, ("net_rx_complete: interface not opened yet\n"));
        return 0;
    }
    
    if (MIN_PACKET > size || size > (MAX_PACKET + ETH_HLEN)) 
    {
        ctx->stats.rx_errors++;
        ctx->stats.rx_length_errors++;
        DBG_E(DPORT_NET, ("net_rx_complete: invalid length %u\n", size));

        return JEIO;
    }

    if ((rx_skb = dev_alloc_skb (size)) == 0)
    {
        DBG_E(DPORT_NET, ("rx_submit: no skb\n"));
        rc = JENOMEM;
        goto Error;
    }

    jos_memcpy(skb_put(rx_skb, size), buffer, size);
    rx_skb->len = size;
    rx_skb->dev = netdev;
    rx_skb->protocol = eth_type_trans (rx_skb, netdev);
    ctx->stats.rx_packets++;
    ctx->stats.rx_bytes += rx_skb->len;

    netif_rx(rx_skb);

    goto Done;

Error:
    if (rx_skb)
        dev_kfree_skb(rx_skb);

Done:
    DBG_X(DPORT_NET, ("net_rx_complete: done\n"));
    return rc;
}

void net_tx_complete(netdev_h dev, void *net_priv, void *buffer)
{
    struct net_device *netdev = (struct net_device*)dev;
    net_ctx_t *ctx = (net_ctx_t*)netdev->priv;
    struct sk_buff *tx_skb = (struct sk_buff *)net_priv;

    DBG_X(DPORT_NET, ("net_tx_complete: entered\n"));

    if (tx_skb)
    {
        ctx->stats.tx_packets++;
        ctx->stats.tx_bytes += tx_skb->len;
        dev_kfree_skb(tx_skb);
    }
}

static jint_t schedule_task(struct sk_buff *skb, struct net_device *net, 
    enum net_action_type type)
{
    jresult_t rc;

    if (pending_abort)
        return 1;

    jtask_args_array[curr_free_possition].skb = skb;
    jtask_args_array[curr_free_possition].net = net;
    jtask_args_array[curr_free_possition].done = 0;
    jtask_args_array[curr_free_possition].type = type;

    rc = jtask_schedule(transfer_task, 0, perform_transfer, NULL);
    if (rc)
    {
        jtask_args_array[curr_free_possition].done = 1;
        DBG_E(DHOST_CDC_UTILS, ("schedule_task: Failed to schedule xfer [%d]"
            "\n", rc));
        return rc;
    }

    curr_free_possition++;

    if (curr_free_possition == CONFIG_MAX_NET_TRANSFERS)
        curr_free_possition = 0;

    if (!jtask_args_array[curr_free_possition].done)
    {
        curr_free_possition--;

        if (curr_free_possition == -1)
            curr_free_possition = CONFIG_MAX_NET_TRANSFERS;
                
        /* TODO: Check linux error return val */
        return 1;
    }

    return 0;
}

static jint_t eth_start_xmit(struct sk_buff *skb, struct net_device *net)
{
    return schedule_task(skb, net, NET_ACTION_TYPE_TX);
}

static jint_t eth_start_xmit_handler(struct sk_buff *skb, 
    struct net_device *net)
{
    jint_t rc = 0;
    net_ctx_t *ctx = (net_ctx_t*)net->priv;

    DBG_X(DPORT_NET, ("eth_start_xmit: entered\n"));

    if (!netif_carrier_ok(net))
    {
        DBG_X(DPORT_NET, ("eth_start_xmit: carrier off - disabling xmit\n"));
        netif_stop_queue(net);
        goto Error;
    }

    if (ctx->app_ops.tx(ctx->app_ctx, skb->data, skb->len, net_tx_complete, 
        skb))
    {
        DBG_E(DPORT_NET, ("eth_start_xmit: failed on lower tx()\n"));
        rc = -JEIO;
        netif_wake_queue(net);
        goto Error;
    }

    net->trans_start = jiffies;
    return 0;
    
Error:
    ctx->stats.tx_dropped++;
    dev_kfree_skb(skb);
    return rc;
}

static void abort_tx(struct net_device *net)
{
    jint_t i;
    net_ctx_t *ctx = (net_ctx_t*)net->priv;

    pending_abort = 1;
    
    for (i = 0; i < CONFIG_MAX_NET_TRANSFERS; i++)
    {
        if (jtask_args_array[i].net && !jtask_args_array[i].done)
        {
            if (jtask_args_array[i].net->priv == ctx)
                jtask_args_array[i].done = 1;
        }
    }
    
    pending_abort = 0;

    schedule_task(NULL, net, NET_ACTION_TYPE_ABORT);
}

static void eth_tx_timeout(struct net_device *net)
{
    abort_tx(net);    
}

static jint_t eth_open(struct net_device *net)
{
    net_ctx_t *ctx = (net_ctx_t*)net->priv;
    
    DBG_X(DPORT_NET, ("eth_open: entered\n"));

    if (netif_carrier_ok(net))
    {
        DBG_X(DPORT_NET, ("eth_open: Waking up queue\n"));
        netif_wake_queue(net);
    }
    else
    {
        DBG_X(DPORT_NET, ("eth_open: Carrier not OK\n"));
        return -JEBUSY;
    }

    ctx->opened = 1;

    return 0;
}

static jint_t eth_stop(struct net_device *net)
{
    DBG_X(DPORT_NET, ("eth_stop: entered - net_device %p\n", net));
    net_ctx_t *ctx = (net_ctx_t*)net->priv;
    
    netif_stop_queue(net);

    ctx->opened = 0;

    return 0;
}

/*
 * Upper layer API for lower layer use
 */
jresult_t net_enable(netdev_h dev)
{
    struct net_device *netdev = (struct net_device*)dev;
    DBG_X(DPORT_NET, ("net_enable: entered\n"));
    netif_carrier_on(netdev);
    netif_wake_queue(netdev);

    printk("%s: Link Up\n", netdev->name);

    return 0;
}

void net_disable(netdev_h dev)
{
    struct net_device *netdev = (struct net_device*)dev;
    DBG_X(DPORT_NET, ("net_disable: entered\n"));

    netif_carrier_off(netdev);
    
    printk("%s: Link Down\n", netdev->name);
}

void net_detach(netdev_h dev)
{
    struct net_device *netdev = (struct net_device*)dev;
    DBG_X(DPORT_NET, ("net_detach: entered\n"));

    printk("%s: Device detached\n", netdev->name);
    
    unregister_netdev(netdev);
    dev_put(netdev);
    kfree(netdev);
}

jresult_t net_attach(app_ctx_h app_ctx, net_app_ops_t *ops, 
    struct net_macaddr *mac_addr, netdev_h *dev)
{
    net_ctx_t *ctx;
    struct net_device *netdev;
    jint_t rc = JENOMEM;

    DBG_X(DPORT_NET, ("net_attach: entered\n"));
    
    netdev = alloc_etherdev(sizeof (net_ctx_t));
    if (!netdev)
        goto Error;

    /* setup private data */
    *dev = netdev;
    ctx = (net_ctx_t *)netdev->priv;
    memset(ctx, 0, sizeof(net_ctx_t));
    memcpy(&ctx->app_ops, ops, sizeof(net_app_ops_t));
    ctx->app_ctx = app_ctx;

    if (!ctx->app_ops.tx)
    {
        DBG_E(DPORT_NET, ("net_attach: missing lower_ops\n"));
        rc = JEINVAL;
        goto Error;
    }

    /* network device setup */
    memcpy(netdev->dev_addr, mac_addr->bytes, sizeof(struct net_macaddr));
    netdev->change_mtu = eth_change_mtu;
    netdev->get_stats = eth_get_stats;
    netdev->hard_start_xmit = eth_start_xmit;
    netdev->tx_timeout = eth_tx_timeout;
    netdev->watchdog_timeo = 1000;      /* in jiffies */
    netdev->open = eth_open;
    netdev->stop = eth_stop;
    netdev->do_ioctl = eth_ioctl;
#ifdef CONFIG_JHOST_WIFI
    netdev->wireless_handlers = 
        (struct iw_handler_def *)&wifi_net_dev_iw_handlers;
#endif

    netif_stop_queue(netdev);
    netif_carrier_off(netdev);

    rc = register_netdev(netdev);
    if (rc)
    {
        DBG_E(DPORT_NET, ("net_attach: register_netdev failed, %d\n", rc));
        goto Error;
    }

    printk("%s: Device attached\n", netdev->name);
    DBG_I(DPORT_NET, ("net_attach: registered network device\n"));
    return 0;

Error:
    if (netdev)
        kfree(netdev);
    return rc;
}

void net_enable_tx(netdev_h dev)
{
    struct net_device *netdev = (struct net_device*)dev;

    if (netif_carrier_ok(netdev))
        netif_wake_queue (netdev);
}

void net_disable_tx(netdev_h dev)
{
    struct net_device *netdev = (struct net_device*)dev;

    netif_stop_queue(netdev);
}

static void perform_transfer(void *arg)
{
    jint_t i;
    jtask_args_t *curr_arg;
   
    for (i = 0; i < CONFIG_MAX_NET_TRANSFERS; i++)
    {
        curr_arg = &jtask_args_array[i];

        if (curr_arg->done)
            continue;

        if (pending_abort)
            break;

        switch (curr_arg->type)
        {
        case NET_ACTION_TYPE_TX:
            eth_start_xmit_handler(curr_arg->skb, curr_arg->net);
            curr_arg->done = 1;
            break;
        case NET_ACTION_TYPE_ABORT:
            {
                net_ctx_t *ctx = (net_ctx_t*)curr_arg->net->priv;

                if (ctx->app_ops.abort_tx)
                    ctx->app_ops.abort_tx(ctx->app_ctx);

                break;
            }
        }
    }
}

jresult_t net_dev_init(void)
{
    jresult_t rc;
    jint_t i;
    
    DBG_V(DPORT_NET, ("net_dev_init: Entered\n"));

    for (i = 0; i< CONFIG_MAX_NET_TRANSFERS; i++)
        jtask_args_array[i].done = 1;

    rc = jtask_init(&transfer_task, TASK_CLASS_DRIVER);
#ifdef JDEBUG
    if (rc)
    {
        DBG_E(DPORT_NET, ("net_dev_init: Failed initializing transfer_task"
            "\n"));
    }
#endif
    
   return rc;
}

void net_dev_uninit(void)
{  
    jtask_uninit(transfer_task);
}

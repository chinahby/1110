#ifndef _IFACE_H
#define _IFACE_H

/*===========================================================================
                                I F A C E . H

DESCRIPTION

   Copyright (c) 1995-2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/iface.h_v   1.1   08 Oct 2002 23:18:14   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/iface.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/13/02    usb    Removed FEATURE_DS_NET_MODEL if FEATURE_DATA_MM is defined
08/13/02    mvl    compile fix for MM.
08/04/02    mvl    MM featurized.
07/25/02    mvl    removed netmodel dependency.  Updates for PPP renaming.
04/17/02    rc     Added ip_pkt_hdlr_func_ptr IP packet handler callback
                   to struct iface.
10/22/99    mvl    Added support for network model related statistics.
10/27/98    ldg    Removed #include "tcp.h"
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

#include "customer.h"       /* Customer Specific Features */
#include "ps_ppp_defs.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
enum if_status_t {
  ifsUp = 1,
  ifsDown,
  ifsTesting,
  ifsUnknown,
  ifsDormant
};


/* Interface control structure */
struct iface {
  char *name;      /* Ascii string with interface name */
  uint32 index;    /* interface number for ifMib */

  uint32 addr;       /* IP address */
  uint32 broadcast;  /* Broadcast address */
  uint32 netmask;    /* Network mask */
  uint32 peeraddr;   /* peer address, if assigned */

  uint32 mru;                     /* Maximum receive unit size */
  uint32 mtu;                     /* Maximum transmission unit size */
#define MXU_DEFAULT     1500                /* default for both MRU and MTU */
#define MXU_MINIMUM  576          /* minimum for both MRU and MTU */
#define MTU_SLIP     1006         /* SLIP default for MTU */

  enum if_status_t AdminStatus;
  enum if_status_t OperStatus;

  uint16 flags;                  /* Configuration flags */
#define IFF_CONNECTED  0x0001    /* Send datagrams in connected mode */
#define IFF_ADDRESS    0x0100    /* address assigned by command */
#define IFF_DYNAMIC    0x0200    /* address dynamically assigned */
#define IFF_PEER       0x0400    /* peer address assigned by command */
#define IFF_POOL       0x0800    /* peer address assigned from pool */

  /* Counters */
  uint32 InIP;           /* IP datagrams sent */
  uint32 OutIP;          /* IP datagrams received */

  uint32 lastsent;       /* Clock time of last send */
  uint32 lastrecv;       /* Clock time of last receive */

  uint32 Speed;          /* current bandwidth, bits per second */

  uint32 UnframedPkts[PPP_MAX_DEV];/* # of pkts unframed per iface     */
  uint32 FramedPkts[PPP_MAX_DEV];  /* # of pkts not unframed per iface */
  uint32 TotalPkts[PPP_MAX_DEV];   /* total # packets per iface        */

  int    (*ip_pkt_hdlr_func_ptr)(       /* Ip Packet recv -handler function*/
                struct dsm_item_s *bp,  /* Input packet                    */
                uint8  *source,         /* media source address if any     */
                int    broadcast, /*True if pkt had link broadcast address */
                void   *hdlr_context_data/*Context passed when calling hdlr*/
                );  
  void *ip_pkt_context_data;       /* Context data for the IP hdlr    */

  uint32 InOctets;       /* # octets received */
  uint32 InUnicast;      /* # unicast packets */
  uint32 InMulticast;    /* # multicast packets */
  uint32 InBroadcast;    /* # broadcast packets */
  uint32 InDiscards;     /* # discards without error */
  uint32 InErrors;       /* # error packets */

  uint32 OutOctets;      /* # octets sent */
  uint32 OutUnicast;     /* # unicast packets */
  uint32 OutMulticast;   /* # multicast packets */
  uint32 OutBroadcast;   /* # broadcast packets */
  uint32 OutDiscards;    /* # discards without error */
  uint32 OutErrors;      /* # errors on send */
};
extern struct iface if_cb;



#endif  /* _IFACE_H */

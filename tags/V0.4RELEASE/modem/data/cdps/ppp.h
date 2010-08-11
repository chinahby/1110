#ifndef PPP_H
#define PPP_H
/*===========================================================================
                                  P P P . H

DESCRIPTION
  Header file for the PPP protocol suite Interfacing functions.

  Copyright 1992-1995 William Allen Simpson
    Licensed to Qualcomm -- 95 Jan 21
  Copyright (c) 1995 by QUALCOMM Incorporated.  All Rights Reserved.
  Copyright (c) 1997-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
#ifndef FEATURE_DATA_MM

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ppp.h_v   1.0   01 Oct 2002 16:37:06   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ppp.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/05/03    jd     Removed dependency on dsiface_def.h
08/04/02    mvl    wrapped in MM feature so file disappears if defined.
07/16/02    cc     Rename ppp_hw_frame_tx() to ppp_hw_frame()
                   and ppp_sw_frame_tx() to ppp_sw_frame(). 
05/03/02    mvl    File cleanup and added meta_info to ppp_outgoing()
05/20/02     vr    Added support for hardware HDLC framer/deframer
03/18/02    vas    Added declaration for callback func to call when CHAP on
                   HDR AN iface succeeds / fails.
02/12/02    vsk    Defined internal ppp_ip_pkt_hdlr_f_ptr_type to make it
                   build for all targets
02/07/02    vsk    Added ppp_install_ip_pkt_handler() & 
                   ppp_uninstall_ip_pkt_handler() prototypes
11/17/01    aku    Changed 2nd arg in ppp_initiate_resync() to boolean type
11/12/01    dwp    Added ppp_incoming() prototype.
11/05/01    vr     During MIP re-reg, forward non-reg pkts to laptop
09/14/01    jd     Added iface mode parameter to ppp_initiate_resync
08/02/01    mvl    modified ppp_discard() signature and code cleanup.
07/31/01    na     Added prototype for ppp_initiate_resync().
07/30/01    mvl    added prototype for ppp_get_iface_opts() code cleanup.
07/27/01    mvl    Modified to use pstimers, added code to support getting
                   the options negotiated on a particular interface.
07/24/01    mvl    modified the signatures of ppp_close() and ppp_initiate() 
05/04/00    mvl    changed interface to network model version of
                   ppp_outgoing() to fix bug where item was freed in this
                   function then freed again lower down.
12/10/99    mvl    Added comment in front of netmodel PPP stats.
10/28/99    mvl    Added code review changes
10/22/99    mvl    Added support for network model related statistics.
08/25/99    mvl    Added header for ppp_asy_in_init().
08/09/99    mvl    Added network model changes.
10/27/98    ldg    ARM porting changes: ROM to ROM const.
07/31/97    ldg    Added accessor function for UI to see some PPP vars.
06/12/97    ldg    Added new FEATURE_DS_PSTATS
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "dsm.h"
#include "psi.h"
#ifdef FEATURE_DS_NET_MODEL
#include "dsnetmdl.h"
#endif
#include "pppfsm.h"
#include "pppvalue.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  defines for HDLC header
---------------------------------------------------------------------------*/
#define HDLC_ALL_ADDR           0xff  /* HDLC all-station                  */
#define HDLC_UI                 0x03  /* HDLC Unnumbered Information       */

/*---------------------------------------------------------------------------
  defines for the PPP protocol field
---------------------------------------------------------------------------*/
#define PPP_IP_PROTOCOL       0x0021  /* Internet Protocol                 */
#define PPP_OSINL_PROTOCOL    0x0023  /* OSI Network Layer                 */
#define PPP_XNS_PROTOCOL      0x0025  /* Xerox NS IDP                      */
#define PPP_DN4_PROTOCOL      0x0027  /* DECnet Phase IV                   */
#define PPP_AT_PROTOCOL       0x0029  /* Appletalk                         */
#define PPP_IPX_PROTOCOL      0x002b  /* Novell IPX                        */
#define PPP_VJCH_PROTOCOL     0x002d  /* Van Jacobson Compressed TCP/IP    */
#define PPP_VJUCH_PROTOCOL    0x002f  /* Van Jacobson Uncompressed TCP/IP  */
#define PPP_BPDU_PROTOCOL     0x0031  /* Bridging PDU                      */
#define PPP_ST2_PROTOCOL      0x0033  /* Stream Protocol (ST-II)           */
#define PPP_BV_PROTOCOL       0x0035  /* Banyan Vines                      */

#define PPP_8021D_PROTOCOL    0x0201  /* 802.1d Hello Packets              */
#define PPP_LUXCOM_PROTOCOL   0x0231  /* Luxcom                            */
#define PPP_SIGMA_PROTOCOL    0x0233  /* Sigma Network Systems             */

#define PPP_IPCP_PROTOCOL     0x8021  /* Internet Protocol Control Protocol*/
#define PPP_OSINLCP_PROTOCOL  0x8023  /* OSI Network Layer Control Protocol*/
#define PPP_XNSCP_PROTOCOL    0x8025  /* Xerox NS IDP Control Protocol     */
#define PPP_DN4CP_PROTOCOL    0x8027  /* DECnet Phase IV Control Protocol  */
#define PPP_ATCP_PROTOCOL     0x8029  /* Appletalk Control Protocol        */
#define PPP_IPXCP_PROTOCOL    0x802b  /* Novell IPX Control Protocol       */
#define PPP_BNCP_PROTOCOL     0x8031  /* Bridging NCP                      */
#define PPP_ST2CP_PROTOCOL    0x8033  /* Stream Protocol Control Protocol  */
#define PPP_BVCP_PROTOCOL     0x8035  /* Banyan Vines Control Protocol     */

#define PPP_LCP_PROTOCOL      0xc021  /* Link Control Protocol             */
#define PPP_PAP_PROTOCOL      0xc023  /* Password Authentication Protocol  */
#define PPP_CHAP_PROTOCOL     0xc223  /* Challenge Handshake A P           */
#define PPP_LQR_PROTOCOL      0xc025  /* Link Quality Report               */

/*---------------------------------------------------------------------------
  Offsets for certain fields in a PPP packet (assume FF03 is stripped off.
  Also assume no compressions in the PPP header.)
---------------------------------------------------------------------------*/
#define PPP_IPSADDR_OFFSET     12     /* Location of Source IP addr.       */
#define PPP_IPPROTO_OFFSET      9     /* Location of IP protocol field     */

/*---------------------------------------------------------------------------
  The PPP header data structure
---------------------------------------------------------------------------*/
struct ppp_hdr
{
  uint8  addr;
  uint8  control;
  uint16 protocol;
};
typedef struct ppp_hdr ppp_hdr_type;

#define PPP_HDR_LEN  4  /* Max bytes for PPP/HDLC envelope header          */
#define PPP_PADDING  8  /* Max bytes for PPP/HDLC padding                  */

/*---------------------------------------------------------------------------
  HDLC envelope constants
---------------------------------------------------------------------------*/
#define HDLC_ENVLEN     8      /* Max bytes for HDLC envelope (outgoing)   */

#define HDLC_FLAG       0x7e   /* HDLC async start/stop flag               */
#define HDLC_ESC_ASYNC  0x7d   /* HDLC transparency escape flag for async  */
#define HDLC_ESC_COMPL  0x20   /* HDLC transparency bit complement mask    */

#define HDLC_FCS_START  0xffff /* Starting bit string for FCS calculation  */
#define HDLC_FCS_FINAL  0xf0b8 /*FCS when summed over frame and sender FCS */

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

/*---------------------------------------------------------------------------
  This is the type declaration for the callback function that is
  registered for processing IP packets received on a PPP interface
---------------------------------------------------------------------------*/
typedef int (*ppp_ip_pkt_hdlr_fn_ptr_type)(iface_stack_enum_type iface, 
                                                         /* Input interface */
                struct dsm_item_s *item_ptr,                 /*Input packet */
                uint8  *source,               /*media source address if any */
                int    broadcast,  /*True if pkt had link broadcast address */
                void   *context_data           /* context data for callback */
                );  



#ifdef FEATURE_HDR_AN_AUTH
#error code not present
#endif /* FEATURE_HDR_AN_AUTH */



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PPP_POWERUP_INIT()

DESCRIPTION
  This function doe the power up initialization for PPP

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_powerup_init(void);


/*===========================================================================
FUNCTION PPP_INIT()

DESCRIPTION
  Initialize PPP control structures for a Point-to-Point interface (called
  from setencap)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_init(void);


/*===========================================================================
FUNCTION PPP_DISCARD()

DESCRIPTION
  This function will queue a PPP Discard packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_discard
(
  iface_stack_enum_type iface,
  dsm_item_type *item_ptr
);


/*===========================================================================
FUNCTION PPP_GET_IFACE_OPTS()

DESCRIPTION
  This function will copy the configuration options for the interface to the
  data structure that was passed in.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_get_iface_opts
(
  iface_stack_enum_type iface,
  ppp_iface_opts_type *opts_var_ptr
);

/*===========================================================================
FUNCTION PPP_START()

DESCRIPTION
  This function

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_start
(
  iface_stack_enum_type iface,      /* The interface (Rm or Um)            */
  ppp_iface_opts_type   *opts_ptr,  /* the options to start PPP with       */
  boolean try_mip
);


/*===========================================================================
FUNCTION PPP_INITIATE()

DESCRIPTION
  This function will initiate the starting of a PPP session for IS-99.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_initiate
(
  iface_stack_enum_type iface       /* The interface (Rm or Um)            */
);

/*===========================================================================
FUNCTION PPP_CLOSE()

DESCRIPTION
  This function will initiate the closing of a PPP session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_close
(
  iface_stack_enum_type iface       /* The interface (Rm or Um)            */
);

/*===========================================================================
FUNCTION RESET_PPP_PKT() 

DESCRIPTION
  This function will delete the packet attached to the ppp_pkt_head_ptr
  global and set the variable to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void reset_ppp_pkt(void);

/*===========================================================================
FUNCTION PPP_INCOMING()

DESCRIPTION
  This function will process the passed PPP packet as an inbound packet.
  Makes sure that the PFC and ACFC compression are not on when they shouldn't
  be.  When in a network model call, checks to see if there are any callvacks
  associated with this protocol and message pair.  If this is not a network
  model call, the mode will be internal and the snooping is not done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_incoming
(
   iface_stack_enum_type iface,       /* iface (Rm or Um) pkt arrived on   */
   dsm_item_type *item_ptr            /* Pointer to the packet             */
);

#ifdef FEATURE_DS_NET_MODEL
/*===========================================================================
FUNCTION PPP_OUTGOING()

DESCRIPTION
  This function will process the passed datagram into a PPP packet for TX.

  This function is defined when FEATURE_DS_NET_MODEL is compiled in.

  Input Conditions:

  1. iface specifies the iface( Rm or Um) where the PPP pkt. is to be sent.

  2. The 'kind' field of the incoming_pkt_ptr specifies the protocol
     of this packet. This field can be:
     DSM_IP_COMPRESS: For IP datagrams. This will cause the packets to go
     through the VJ compressor.
     PPP_IP_PROTOCOL: For IP datagrams. IP packets will not go through
     the VJ compressor.
     PPP_LCP_PROTOCOL: For LCP proto.
     PPP_IPCP_PROTOCOL: For IPCP
     PPP_PAP_PROTOCOL: For PAP.
     PPP_VJCH_PROTOCOL: For VJ Compressed pkts.
     PPP_VJUCH_PROTOCOL: For VJ Uncompressed pkts.

     If it is not a known field, then the protocol parameter is used.

  3. For all packets the address and control
     fields(FF03) and the protocol fields should be stripped.

   Output Conditions:
     This function takes care to free the passed packet chain even if there
     is a error. If there is no error, it passes the packet chain to the
     lower layer.

DEPENDENCIES
  None

RETURN VALUE
  -1 on error.
   0 on success

SIDE EFFECTS
  None
===========================================================================*/
int ppp_outgoing
(
  iface_stack_enum_type out_iface,              /* iface to be sent on     */
  uint16                protocol,               /* protocol of packet      */
  dsm_item_type       **incoming_pkt_ptr,       /* ptr to PPP packet chain */
  ps_meta_info_type    *meta_info_ptr
);

#else /* if FEATURE_DS_NET_MODEL not defined */

/*===========================================================================
FUNCTION PPP_OUTGOING()

DESCRIPTION
  This function is compiled in when FEATURE_DS_NET_MODEL is not defined.

  This function will process the passed datagram into a PPP packet for TX.

DEPENDENCIES
  None

RETURN VALUE
  -1 on error.
   0 on success.

SIDE EFFECTS
  None
===========================================================================*/
int ppp_outgoing
(
  dsm_item_type *incoming_pkt_ptr
);
#endif  


#ifdef FEATURE_DS_NET_MODEL
/*===========================================================================
FUNCTION PPP_ASY_IN()

DESCRIPTION
  This function will Packetize PPP input from asynchronous device

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_asy_in
(
  iface_stack_enum_type iface,        /* iface (Rm or Um) pkt arrived on   */
  dsm_item_type       **item_head_ptr /* address of ptr to a dsm_item_type */
);

#else /* if FEATURE_DS_NET_MODEL not defined */

/*===========================================================================
FUNCTION PPP_ASY_IN()

DESCRIPTION
  This function will Packetize PPP input from asynchronous device

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_asy_in
(
  dsm_item_type **bpp             /* address of ptr to a dsm_item_type */
);

#endif /* FEATURE_DS_NET_MODEL */


#ifdef FEATURE_DS_NET_MODEL
/*===========================================================================
FUNCTION PPP_ASY_IN_INIT()

DESCRIPTION
  This function will free any packets that ppp_asy_in() had hanging around
  from the previous call.  Also clear out the various other params.

DEPENDENCIES
Should only be called when entering Autodetect mode!

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_asy_in_init
(
  iface_cb_type *iface_cb_ptr           /* the iface control block to init */ 
);
#endif



#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */
/*===========================================================================
FUNCTION PPP_INITIATE_RESYNC()

DESCRIPTION
  This function initiates a PPP resync on the given interface by spoofing
  a LCP C-REQ
  
  This should always be executed in the context of the PS task.

PARAMETERS
  iface: Interface to spoof C-Req on.
  mode:  set to PSEUDO_NETWORK for end-to-end resync, INTERNAL for one iface

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_initiate_resync
(  
  iface_stack_enum_type iface,      /* The interface (Rm or Um)            */
  boolean try_mip                   /* Attempt PPP setup with MIP?         */
);

/*===========================================================================
FUNCTION PPP_INSTALL_IP_PKT_HANDLER()

DESCRIPTION

  This function allows a callback function to be installed with PPP to 
  process IP packets received on an interface
  If no callback fucntion is installed,the default behavior applies for
  the interface (call ip_route).

PARAMETERS
  iface: Interface to install the callback for
  ip_pkt_hndlr:  callback function which will handler IP packet processing
  callback_contextdata : Context data which is passed to the callback function

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_install_ip_pkt_handler
(  
  iface_stack_enum_type         iface,           
  ppp_ip_pkt_hdlr_fn_ptr_type   ip_pkt_hndlr,
  void                          *callback_contextdata
);

/*===========================================================================
FUNCTION PPP_UNINSTALL_IP_PKT_HANDLER()

DESCRIPTION
  This function unisntalls the IP packet handler callback for the specified 
  interface.

PARAMETERS
  iface: Interface to install the callback for

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_uninstall_ip_pkt_handler
(  
  iface_stack_enum_type         iface
);

/*===========================================================================
FUNCTION PPP_SW_FRAME()

DESCRIPTION
  This function will frame the PPP packet based on the specified accm, 
  append the CRC and trailing 7E flag and send it on the specified interface. 
  It will do the entire process in software.

  The input packet pointer should be the complete PPP packet to be framed
  including HDLC header and PPP protocol field (if we desire to send them),
  but without the CRC.
 
DEPENDENCIES
  This function should only be called from ppp_outgoing.

RETURN VALUE
  -1 on error, 0 on success

SIDE EFFECTS
  None
===========================================================================*/
int ppp_sw_frame
(
  iface_stack_enum_type iface,                  /* outgoing iface          */
  uint32                accm,               /* accm to use for this iface  */
  dsm_item_type **incoming_pkt_ptr,             /* pkt to send             */
  boolean         doing_lcp             /* Indication that LCP is happening*/
);


#ifdef FEATURE_PPP_HW_ACCEL
/*===========================================================================
FUNCTION PPP_HW_FRAME()

DESCRIPTION
  This function will use the hardware HDLC framer to frame the
  PPP packet based on the specified interface, append the CRC and trailing 
  7E flag and send it on the specified interface. 

  The input packet pointer should be the complete PPP packet to be framed
  including HDLC header and PPP protocol field (if we desire to send them),
  but without the CRC.
 
DEPENDENCIES
  This function should only be called from ppp_outgoing.

RETURN VALUE
  -1 on error, 0 on success

SIDE EFFECTS
  None
===========================================================================*/
int ppp_hw_frame
(
  iface_stack_enum_type iface,                  /* outgoing iface          */
  uint32                accm,               /* accm to use for this iface  */
  dsm_item_type **incoming_pkt_ptr              /* pkt to send             */
);
#endif /* FEATURE_PPP_HW_ACCEL */


#ifdef FEATURE_PPP_HW_ACCEL
/*===========================================================================
FUNCTION PPP_HDLC_HW_UNFRAME()

DESCRIPTION
  This function will Packetize PPP input from asynchronous device, and 
  unframe the packets using the hardware deframer block. Currently, this 
  should ONLY be called for sockets/full-network model calls. It will pass 
  the deframed packet up to ppp_incoming for processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_hdlc_hw_unframe
(
  iface_stack_enum_type iface,        /* iface (Rm or Um) pkt arrived on   */
  dsm_item_type       **item_head_ptr /* address of ptr to a dsm_item_type */
);

#endif /* FEATURE_PPP_HW_ACCEL */

#endif /* FEATURE_DATA_MM */

#endif /* PPP_H */


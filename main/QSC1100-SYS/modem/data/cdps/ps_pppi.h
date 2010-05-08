#ifndef PS_PPPI_H
#define PS_PPPI_H
/*===========================================================================
                              P S _ P P P I . H

DESCRIPTION
  Header file for the internal PPP protocol suite functions.

  Copyright 1992-1995 William Allen Simpson
    Licensed to Qualcomm -- 95 Jan 21
  Copyright (c) 1995 by QUALCOMM Incorporated.  All Rights Reserved.
  Copyright (c) 1997-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_pppi.h_v   1.11   03 Feb 2003 13:59:24   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_pppi.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/07/03    usb    Added prototype for pppi_receive()
04/02/03    mvl    Complete modularization of the authentication code.
01/27/03    jd     Added prototype for process_chap_pkt()
01/27/03    usb    Added pppi_set_mode() to set PPP mode in PS context
11/14/02    mvl    Moved the protocol definitions to the defs file.
10/25/02    usb    Modified prototype of pppi_outgoing to take meta info as 
                   double pointer
10/11/02    usb    Modified HDLC hw feature to FEATURE_HDLC_HW_ACCEL 
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user_data to ppp tx and rx function callbacks
09/02/02    mvl    added declaration for the ps iface tx function.
08/13/02    mvl    added session handle support.
07/25/02    mvl    removed netmodel dependency.  Updates for PPP renaming.
07/11/02    mvl    Updates for new PPP architecture.
05/22/02    mvl    Renamed the file and moved all of the truly public stuff
                   out into the new file ppp.h
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
#include "ps_ppp_defs.h"
#include "ps_ppp_task.h"
#include "ps_iface.h"
#include "ps_meta_info.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  This array contains the default signals for PPP instances to use.
---------------------------------------------------------------------------*/
extern const ppp_sig_type ppp_dev_to_default_sig_array[PPP_MAX_DEV];

/*---------------------------------------------------------------------------
TYPEDEF PPPI_START_INFO_TYPE

DESCRIPTION
  Type that is used to pass the ppp device and the session handle from the
  callers context to the PPP execution context.
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint16 device;
  uint16 session_handle;
} pppi_start_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PPPI_MODE_INFO_TYPE

DESCRIPTION
  Type that is used to pass the ppp device and the PPP mode from the
  callers context to the PPP execution context.
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint16 device;
  uint16 mode;
} pppi_mode_info_type;

/*---------------------------------------------------------------------------
  defines for HDLC header
---------------------------------------------------------------------------*/
#define HDLC_ALL_ADDR           0xff  /* HDLC all-station                  */
#define HDLC_UI                 0x03  /* HDLC Unnumbered Information       */

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
typedef int (*ppp_ip_pkt_hdlr_fn_ptr_type)(ppp_dev_enum_type iface,
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
FUNCTION PPPI_POWERUP_INIT()

DESCRIPTION
  This function doe the power up initialization for PPP

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_powerup_init(void);


/*===========================================================================
FUNCTION PPPI_DISCARD()

DESCRIPTION
  This function will queue a PPP Discard packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int pppi_discard
(
  ppp_dev_enum_type device,
  dsm_item_type    *item_ptr
);


/*===========================================================================
FUNCTION PPPI_START()

DESCRIPTION
  This function will actively start PPP (though potentially in passive
  mode).

PARAMETERS
  start_info: parameters to use with start

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_start
(
  pppi_start_info_type start_info
);

/*===========================================================================
FUNCTION PPPI_STOP()

DESCRIPTION
  This function will initiate the closing of a PPP session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_stop
(
  ppp_dev_enum_type device         /* The interface (Rm or Um)             */
);

/*===========================================================================
FUNCTION PPPI_ABORT()

DESCRIPTION
  This function will clean up after PPP is finished.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_abort
(
  ppp_dev_enum_type device          /* The interface (Rm or Um)            */
);

/*===========================================================================
FUNCTION PPPI_SET_MODE()

DESCRIPTION
  This function will change the PPP mode as specified. It assumes that the
  mode being set is correct and validation on the mode has been performed
  before posting the cmd to call this function.
  
PARAMETERS
  mode_info: parameters passed through the cmd required to set mode

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_set_mode
(
  pppi_mode_info_type mode_info
);

/*===========================================================================
FUNCTION PPPI_HDLC_UNFRAME_INIT() (was PPP_ASY_IN_INIT())

DESCRIPTION
  This function will free any packets that pppi_asy_in() had hanging around
  from the previous call.  Also clear out the various other params.

DEPENDENCIES
  Should only be called when entering Autodetect mode!

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_hdlc_unframe_init
(
  ppp_dev_enum_type device           /* ppp device pkt arrived on        */
);

/*===========================================================================
FUNCTION PPPI_HDLC_SW_UNFRAME() (was PPP_ASY_IN())

DESCRIPTION
  This function will Packetize PPP input from asynchronous device, and does
  network model processing.  It uses a state machine to accomplish this
  network model processing.  The state machine is illustrated below:

                              -------------
                             |             |
            -----------------|  UNFRAMING  |<------------------
           /                 |             |                   \
  Dispatched packet to        -------------                     \
    pppi_incoming() or               ^                      Found matching
     found FCS error                |                         protocol
       /                   Found start of packet                   \
       |                    (Full Network Mode)                     \
       v                            |                                |
  ------------                -------------                     ------------
 |            |              |             |  Found start of   |            |
 | FINDING_7E |-- Found 7E ->| FINDING_SOP |- packet (Pseudo ->|  SNOOPING  |
 |            |              |             |  Network  Mode)   |            |
  ------------                -------------                     ------------
       ^                        /      ^                            |
       |                        |   Found 7E                       /
        \                        \----/                           /
   Dispatched packet                                        No matching
   to out_iface_tx()          -------------               protocol found
           \                 |             |                   /
            \----------------| FINDING_EOP |<-----------------/
                             |             |
                              -------------

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_hdlc_sw_unframe
(
  ppp_dev_enum_type iface,        /* iface (Rm or Um) pkt arrived on   */
  dsm_item_type       **item_head_ptr /* address of ptr to a dsm_item_type */
);

#ifdef FEATURE_HDLC_HW_ACCEL
/*===========================================================================
FUNCTION PPPI_HDLC_HW_UNFRAME()

DESCRIPTION
  This function will Packetize PPP input from asynchronous device, and
  unframe the packets using the hardware deframer block. Currently, this
  should ONLY be called for sockets/full-network model calls. It will pass
  the deframed packet up to pppi_incoming for processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_hdlc_hw_unframe
(
  ppp_dev_enum_type   iface,          /* iface (Rm or Um) pkt arrived on   */
  dsm_item_type     **item_head_ptr   /* address of ptr to a dsm_item_type */
);

#endif /* FEATURE_HDLC_HW_ACCEL */

/*===========================================================================
FUNCTION PPPI_INCOMING()

DESCRIPTION
  This function will process the passed PPP packet as an inbound packet.
  Makes sure that the PFC and ACFC compression are not on when they shouldn't
  be.  When in a network model call, checks to see if there are any callbacks
  associated with this protocol and message pair.  If this is not a network
  model call, the mode will be internal and the snooping is not done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_incoming
(
   ppp_dev_enum_type device,           /* ppp device pkt arrived on        */
   dsm_item_type **item_ref_ptr        /* Pointer to ref to pkt            */
);

/*===========================================================================
FUNCTION PPPI_RECEIVE()

DESCRIPTION
  This is the PPP packet receiving function in the stack. It processes each 
  PPP packet based upon the protocol type and pass it up to the appropriate 
  protocol engine in the stack.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_receive
(
  ppp_dev_enum_type device,           /* ppp device pkt arrived on         */
  uint16            protocol,               /* protocol of packet          */
  dsm_item_type     **item_ref_ptr        /* Pointer to ref to pkt         */
);

/*===========================================================================
FUNCTION PPPI_IFACE_TX_FUNCTION()

DESCRIPTION
  This function is registered with the associated PS Iface when a PPP
  instance is up (this is done in ipcp_openeing()).  It is a ps_iface tx
  command. 

PARAMETERS
  this_iface_ptr: pointer to the interface which is doing the TX
  pkt_chain_ptr: reference to a dsm item pointer: this is payload for PPP
  meta_info_ref_ptr: reference to a meta info item.
  tx_cmd_info: in the case of PPP this should point to the PPP instance this
               should be transmitted on.

RETURN VALUE
  Same as pppi_outgoing
   0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int pppi_iface_tx_function
(
  ps_iface_type      *this_iface_ptr,
  dsm_item_type     **item_ref_ptr,
  ps_meta_info_type **meta_info_ref_ptr,
  void               *tx_cmd_info
);


/*===========================================================================
FUNCTION PPPI_OUTGOING()

DESCRIPTION
  This function will process the passed datagram into a PPP packet for TX.

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
int pppi_outgoing
(
  ppp_dev_enum_type     out_iface,              /* iface to be sent on     */
  uint16                protocol,               /* protocol of packet      */
  dsm_item_type       **incoming_pkt_ptr,       /* ptr to PPP packet chain */
  ps_meta_info_type   **meta_info_ref_ptr       /* ptr to Meta Info        */
);


/*===========================================================================
FUNCTION PPPI_HDLC_SW_FRAME()

DESCRIPTION
  This function will frame the PPP packet based on the specified accm,
  append the CRC and trailing 7E flag and send it on the specified interface.
  It will do the entire process in software.

  The input packet pointer should be the complete PPP packet to be framed
  including HDLC header and PPP protocol field (if we desire to send them),
  but without the CRC.

DEPENDENCIES
  This function should only be called from pppi_outgoing.

RETURN VALUE
  -1 on error, 0 on success

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *pppi_hdlc_sw_frame
(
  ppp_dev_enum_type iface,                  /* outgoing iface          */
  uint32                accm,               /* accm to use for this iface  */
  dsm_item_type **incoming_pkt_ptr,             /* pkt to send             */
  boolean         doing_lcp             /* Indication that LCP is happening*/
);


#ifdef FEATURE_HDLC_HW_ACCEL
/*===========================================================================
FUNCTION PPPI_HDLC_HW_FRAME()

DESCRIPTION
  This function will use the hardware HDLC framer to frame the
  PPP packet based on the specified interface, append the CRC and trailing
  7E flag and send it on the specified interface.

  The input packet pointer should be the complete PPP packet to be framed
  including HDLC header and PPP protocol field (if we desire to send them),
  but without the CRC.

DEPENDENCIES
  This function should only be called from pppi_outgoing.

RETURN VALUE
  -1 on error, 0 on success

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *pppi_hdlc_hw_frame
(
  ppp_dev_enum_type iface,                      /* outgoing iface          */
  uint32            accm,                   /* accm to use for this iface  */
  dsm_item_type   **incoming_pkt_ptr            /* pkt to send             */
);
#endif /* FEATURE_HDLC_HW_ACCEL */

/*===========================================================================
FUNCTION PPPI_DEFAULT_RX()

DESCRIPTION
  This function will simply return NULL indicating to the caller that there
  was no data to receive.

PARAMETERS
  None

RETURN VALUE
  NULL

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *pppi_default_rx
(
  void *user_data
);

/*===========================================================================
FUNCTION PPPI_DEFAULT_TX()

DESCRIPTION
  This function will free any packet that is passed in.

PARAMETERS
  item_ref_ptr: pointer to a reference to a dsm item chain.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_default_tx
(
  dsm_item_type **item_ref_ptr,
  void *user_data
);

/*===========================================================================
FUNCTION PPPI_PS_IFACE_GET_PEER_ADDR_CB()

DESCRIPTION
  This function is intended to be registered with ps_iface to allow the
  determination of the PPP peer address.

  NOTE: This should only ever be called through the callback pointer in PS
        Iface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be
  link_info:      void pointer that will contain the PPP device in question

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_ps_iface_get_peer_addr_cback
(
  ps_iface_type *this_iface_ptr,
  ip_addr_type  *ip_addr_ptr,
  void          *link_info
);

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */
#endif  /* PS_PPPI_H */


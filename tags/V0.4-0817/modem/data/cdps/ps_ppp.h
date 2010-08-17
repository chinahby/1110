#ifndef PS_PPP_H
#define PS_PPP_H
/*===========================================================================
                               P S _ P P P . H

DESCRIPTION
  Header file for the PPP protocol suite Interfacing functions.

  Copyright (c) 2002-2009 by QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp.h_v   1.9   15 Nov 2002 23:35:24   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp.h#2 $
  $Author: nsivakum $ $DateTime: 2009/05/27 05:07:18 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Ported support for call throttle feature (DCTM).
04/11/03    mvl    Added declaration of ppp_set_auth_server_opts().  Removed
                   INLINE to fix link issue.
04/07/03    usb    Renamed ppp_set_resync_opts() to 
                   ppp_update_ipcp_config(), Added ppp_inject_packet()
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP. 
11/14/02   jd/usb  Removed ppp_get_dev_opts prototype
                   Added ppp_get_dev_setup prototype
                   Added ppp_get_protocol_opts prototype
                   Added ppp_set_resync_opts prototype
                   Added ppp_resync prototype
11/04/02    mvl    Added the JCDMA guard time value in #define.
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user data for tx and rx functions to ppp dev options
09/19/02    mvl    Added ppp_set_v4_server_opts() to make server config.
09/18/02    mvl    Added user_data to ppp event callbacks.  Added explicit
                   configuration for client vs. server behavior.  Removed
                   do_mip.
09/12/02    mvl    Added setup timeout field to config structure and removed
                   the pend_rx/tx fields.
08/13/02    mvl    Session handle support and moved set_mode function from
                   netmdl.
08/06/02    mvl    Reworked authentication.
07/25/02    mvl    Removed depenency on ps_ppp_fsm.h.  Updates for PPP
                   renaming.
07/24/02    mvl    Name change in prototypes.
07/16/02    aku    Removed #define for ppp_config
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Added event callbacks to the configuration structure.
05/22/02    mvl    created file with all of the external information for
                   users of ppp.
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
#include "ps_ppp_fsm.h"

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_JCDMA_1X
#define JCDMA_MAX_PPP_SETUP_TIME 40 /* JCDMA 40s guard time as per
                                       80-V5266-1 rev B sec 4.8            */
#endif /* FEATURE_JCDMA_1X */

/*---------------------------------------------------------------------------
  Structure for setting/getting the negotiated options on a particular
  interface.  This strucuture should be made a part of the ppp_cb - and
  remove all of the duplicated fields.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    All of the LCP information
  -------------------------------------------------------------------------*/
  struct
  {
    lcp_value_type  want_vals[FSM_SIDE_MAX];
    lcp_value_type  work_vals[FSM_SIDE_MAX];
    negotiate_t     want_mask[FSM_SIDE_MAX];
    negotiate_t     will_mask[FSM_SIDE_MAX];
    cp_mode_type    mode;

  } lcp_info;

  /*-------------------------------------------------------------------------
    All of the IPCP information
  -------------------------------------------------------------------------*/
  struct
  {
    ipcp_value_type want_vals[FSM_SIDE_MAX];
    ipcp_value_type work_vals[FSM_SIDE_MAX];
    negotiate_t     want_mask[FSM_SIDE_MAX];
    negotiate_t     will_mask[FSM_SIDE_MAX];
    cp_mode_type    mode;

  } ipcp_info;

  /*------------------------------------------------------------------------
    the auth information
  -------------------------------------------------------------------------*/
  ppp_auth_info_type auth_info;

  /*-------------------------------------------------------------------------
    PPP RX and TX information
  -------------------------------------------------------------------------*/
  tx_f_ptr_type     tx_f_ptr;       /* function user to tx PPP packets     */
  rx_f_ptr_type     rx_f_ptr;       /* Function to receive next data item  */
  void             *tx_user_data;   /* User data passed to tx_f_ptr        */
  void             *rx_user_data;   /* User data passed to rx_f_ptr        */

  ps_iface_type    *rx_iface_ptr;   /* interface to associate with         */
  ppp_sig_type      rx_signal;      /* the RX signal to use                */
  ppp_dev_enum_type bridge_dev;     /* device to bridge to (netmodel)      */
  boolean           serves_addrs;   /* does this serve IP addresses?       */  
  boolean           auths_peer;     /* does this authenticate peer?        */

  /*-------------------------------------------------------------------------
    HDLC handling, including the unframing mode, and pointers to functions
    used to forward packets in various states of HDLC unframing.  The
    function that forwards unframed packets could be used by the network
    model callbacks - rather than forwarding them directly.
  -------------------------------------------------------------------------*/
  hdlc_mode_enum_type hdlc_mode;        /* HDLC framing mode               */

  /*-------------------------------------------------------------------------
    total setup time - seconds
  -------------------------------------------------------------------------*/
  uint8 setup_timeout;

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */


} ppp_dev_opts_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PPP_GET_DEFAULT_OPTS()

DESCRIPTION
  This function will populate the options data structure with the default
  values for a PPP session.  The intention is to allow users to get a default
  configuration and modify only the fields that they wish and the fields that
  are set to invalid values (the owner of the configuration structure must
  set these values by hand).

  The following fields are set to invalid values:
    lcp_info.mode   -> PPP_CP_INVALID_MODE
    ipcp_info.mode  -> PPP_CP_INVALID_MODE
    rx_f_ptr        -> NULL
    rx_iface_ptr    -> NULL
    hdlc_mode       -> HDLC_INVALID_MODE
    bridge_dev      -> PPP_INVALID_DEV
    serves_addrs    -> FALSE
    auths_peer      -> FALSE
    rx_signal       -> PPP_INVALID_SIGNAL

  NOTE: authentication is not requested in LCP, the desired local IPCP values
  for compression are used.

PARAMETERS
  config_ptr: configuration structure to fill in with the default information

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_get_default_opts
(
  ppp_dev_opts_type *opts_ptr
);


/*===========================================================================
FUNCTION PPP_GET_PROTOCOL_OPTS()

DESCRIPTION
  This function will copy the framing options for a particular PPP
  device into the option structure provided by the caller.

PARAMETERS
  device: PPP device to copy
  config_ptr: pointer to configuration structure in which to put information
  swap: should devices local information be put in the configuration
        strucuture's remote slot and visa versa?

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_protocol_opts
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
);

int ppp_get_bridge_dev_prot_opts
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
);

/*===========================================================================
FUNCTION PPP_GET_DEV_SETUP()

DESCRIPTION
  This function will copy the device internal options for a particular PPP
  device into the provided option structure.

PARAMETERS
  device: PPP device to copy settings from
  config_ptr: pointer to configuration structure in which to put information

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_dev_setup
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *opts_ptr
);


/*===========================================================================
FUNCTION PPP_GET_AUTH_INFO()

DESCRIPTION
  This function will return a pointer to a data structure containing all of
  th authentication information.  The macro PPP_AUTH_WAS_CHAP() can be used
  to determine if CHAP or PAP was done.  The return value will be NULL if the
  device is out of bounds OR no authentication was performed.

  NOTE: this informaion should be treated as READ-ONLY changing values of the
        pointer information has unspecified effects.

PARAMETERS
  device: the PPP device from which to return the auth information.

RETURN VALUE
  pointer to the auth information in question
  NULL  if the device is out of bounds OR no authentication was performed.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_auth_info_type *ppp_get_auth_info
(
  ppp_dev_enum_type  device
);


/*===========================================================================
MACRO PPP_AUTH_WAS_CHAP()

DESCRIPTION
  This function will indicate if CHAP was used.  It is assumed that it will
  be used in conjuction with ppp_get_auth_info().  Since that will return
  FALSE if no authentication was done then it can be assumed that if this
  indicates that CHAP was not done then PAP was.

PARAMETERS
  auth_info_ptr: pointer to the auth info returned from get_auth_info()
                 NOTE: no checking is done, so this will blow up if NULL is
                 passed in.

RETURN VALUE
  TRUE  if chap was done
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PPP_AUTH_WAS_CHAP(auth_info_ptr)                                \
  (((struct ppp_cb_auth_s*)auth_info_ptr)->mode == PPP_DO_CHAP)


/*===========================================================================
FUNCTION PPP_UPDATE_IPCP_CONFIG()

DESCRIPTION
  This function reconfigures IPCP options for the specified PPP device. 
  This prepares PPP device for subsequent resyncs after it has already come 
  up or allows reconfiguration while PPP is in the middle of coming up. 
  
PARAMETERS
  device:        PPP device to configure
  config_ptr:    configuration to use

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies IPCP options for the given PPP device
===========================================================================*/
void ppp_update_ipcp_config
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
);


/*===========================================================================
FUNCTION PPP_SET_V4_SERVER_OPTS()

DESCRIPTION
  This function will change the configuration passed in to serve IP
  addresses.  This includes setting the serves_addrs field, and setting the
  appropriate IPCP bits.

PARAMETERS
  config_ptr:    configuration to convert
  local_addr:    the local IP address to use
  remote_addr:   the address to assign
  primary_dns:   the primary DNS address to advertise
  secondary_dns: the primary DNS address to advertise

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_v4_server_opts
(
  ppp_dev_opts_type *config_ptr,
  uint32             local_addr,
  uint32             remote_addr,
  uint32             primary_dns,
  uint32             secondary_dns
);


/*===========================================================================
FUNCTION PPP_SET_AUTH_SERVER_OPTS()

DESCRIPTION
  This function will setup the configuration to enable the receiving PPP
  instance to authenticate the peer.

PARAMETERS
  config_ptr: pointer to the configuration being modified
  challenge name: string indicating the name that should be sent as part of
      the chap challenge.  This can be NULL or the empty string if no name is
      needed (PAP), or wants to be sent.

RETURN VALUE
  -1 on error
  the number of bytes of challenge_name copied
      NOTE: this may be less than strlen(challenge_name) if that is greater
      than PPP_CHAP_CHAL_NAME_LEN.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_set_auth_server_opts
(
  ppp_dev_opts_type *config_ptr,
  char              *challenge_name
);


/*===========================================================================
FUNCTION PPP_REG_EVENT()

DESCRIPTION
  This function will register a callback function for a specific PPP event.

PARAMETERS
  device:     the PPP instance to register for
  event:      the event to trigger the callback
  cback_func: the function to call
  old_cb_f:   value return argument, the value of the previous callback is
              written here

RETURN VALUE
 -1 on error
  0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_reg_event
(
  ppp_dev_enum_type           device,
  ppp_event_e_type            event,
  ppp_event_cback_f_ptr_type  cback_f,
  void                       *user_data
);


/*===========================================================================
FUNCTION PPP_START()

DESCRIPTION
  This function starts the PPP state machine.  If the opts_ptr field is NULL
  it will get the default configuration for PPP use that.  It also registers
  a callback that is associated with the signal for PPP instance being
  started.

PARAMETERS
  device: the ppp instance to configure
  opts_ptr: pointer to a variable containing the optsion to configure with

RETURN VALUE
  -1: if the ppp instance cannot be configured
  session handle of 0 or greater on success.  This can be used to associate
  PPP events with this instance of PPP.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_start
(
  ppp_dev_enum_type  device,        /* The interface (Rm or Um)            */
  ppp_dev_opts_type *config_ptr     /* the options to start PPP with       */
);

/*===========================================================================
FUNCTION PPP_STOP()

DESCRIPTION
  This function will gracefully terminate PPP.

PARAMETERS
  device: the ppp instance to configure

RETURN VALUE
  -1: if the ppp instance cannot be configured
   0: on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_stop
(
  ppp_dev_enum_type  device
);

/*===========================================================================
FUNCTION PPP_RESYNC()

DESCRIPTION
  This function will restart PPP - forcing it to renegotiate.  It REQUIRES
  that the LCP mode be active.  NOTE: though a configuration can be passed in
  it is NOT possible to change the RX signal to use.

PARAMETERS
  device: the PPP device to be resync'ed
  config_ptr: pointer to the configuration with which to resync PPP device
          'device'

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or some configuration is bad.

DEPENDENCIES
  None

SIDE EFFECTS
  If configuration is passed in, it will be copied to the PPP device
  IMMEDIATELY (if the device is free).  This means that the PPP context is
  modified in the callers task context - it also means that the memory for
  configuration passed in is free as soon as this function returns.
===========================================================================*/
int ppp_resync
(
  ppp_dev_enum_type device,
  ppp_protocol_e_type protocol,
  ppp_dev_opts_type *config_ptr

);
/*===========================================================================
FUNCTION PPP_ABORT()

DESCRIPTION
  This function is used by PPP clients to abort a particular PPP device.
  
PARAMETERS
  device: the PPP device to be aborted

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or a bqad device was passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  the 'device' field in the configuration structure will be set to the value
  of the 'device' parameter passed in.
===========================================================================*/
int ppp_abort
(
  ppp_dev_enum_type  device
);


/*===========================================================================
FUNCTION PPP_INJECT_PACKET()

DESCRIPTION
  This function processes a PPP packet as if the packet has been 
  received from the underlying device. The processing is performed as 
  soon as this function is called before other packets that are already 
  queued in the rx queue of the device. Only exception is the packet that is
  currrently being processed in PS context. This function posts a cmd to PS 
  so that packet processing always takes place in PS. This function allows 
  replaying previously stored PPP packets.
  
PARAMETERS
  device                ppp device to pass pkt to     
  protocol              protocol of packet            
  ppp_pkt_ptr           Pointer to PPP pkt            

RETURN VALUE
  0: on success
 -1: if an invalid or unconfigured PPP device specified

DEPENDENCIES
  This function should not be called while already processing a PPP 
  packet. That means this function should not be called from pppi_incoming(), 
  pppi_receive(), pppi_hdlc_sw/hw_frame/unframe() functions.
  It expects an unframed PPP packet, without FCS and ASYNC FLAG (7E)

SIDE EFFECTS
  Processes the specified PPP packet out of order
===========================================================================*/
int ppp_inject_packet
(
  ppp_dev_enum_type device,
  uint16            protocol,
  dsm_item_type     *ppp_pkt_ptr
);


/*===========================================================================
FUNCTION PPP_SET_MODE()

DESCRIPTION
  This function takes a new mode and interface as arguments and changes the
  mode of the interface appropriately.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_mode
(
  ppp_dev_enum_type  device,             /* device to change              */
  ppp_mode_enum_type new_mode            /* new mode for that device      */
);

/*===========================================================================
FUNCTION PPP_GET_MODE()

DESCRIPTION
  This function returns the current mode of the PPP device in question.

DEPENDENCIES
  None

RETURN VALUE
  The mode of the requested interface.

SIDE EFFECTS
  None
===========================================================================*/
ppp_mode_enum_type ppp_get_mode
(
  ppp_dev_enum_type device              /* device to check              */
);

/*===========================================================================
FUNCTION PPP_GET_PHASE()

DESCRIPTION
  This function returns the current phase of the interface in question.

DEPENDENCIES
  None

RETURN VALUE
  The phase of the requested interface.

SIDE EFFECTS
  None
===========================================================================*/
ppp_link_phase_type ppp_get_phase
(
  ppp_dev_enum_type device              /* device to check              */
);

#endif /* PS_PPP_H */


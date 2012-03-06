/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               P S _ P P P . C

GENERAL DESCRIPTION
  All of the external PPP protocol suite interface functions.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002-2009 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp.c_v   1.16   31 Jan 2003 18:46:32   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp.c#3 $
  $Author: nsivakum $ $DateTime: 2009/05/27 05:07:18 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Ported support for call throttle feature (DCTM).
02/22/08    pp     Klocwork fixes.
03/08/04    usb    Setting ps_iface ptr to Null only in PPP task context.
01/09/04    jd     In ppp_get_dev_opts(), return auth info if device 
                   authenticates, and clear auth info if not.
                   In set_auth_server_opts, if no challenge name provided,
                   set challenge name length to 0.
08/22/03    usb    Pass the registered userdata in PPP RESYNC event callback
05/28/03    usb    Fixed ppp_resync to prevent potential race conditions
05/22/03    usb    Make sure PPP device is configured before changing mode
                   in ppp_set_mode(), also warpped under TASKLOCK.
05/14/03    usb    Corrected auth parameter validation in ppp_i_config.
04/23/03    usb    Fixed ppp_resync() to pass in the correct device and 
                   session handle wih the cmd to PS. Fixed 
                   ppp_set_v4_server_opts() to not assign DNS addresses if 
                   not asked by peer.
04/18/03    vsk    Return NULL if Auth was not performed in ppp_get_auth_info()
04/18/03    vsk    initialize serves_address in ppp_update_ipcp_config()
04/11/03    mvl    Added support for SNOOP_INTERNAL mode to set_mode(), Removed
                   INLINE to fix link issue and added the
                   set_auth_server_opts().
04/08/03    mvl    Cleanup up file
04/07/03    usb    Renamed ppp_set_resync_opts() to 
                   ppp_update_ipcp_config(), Added ppp_inject_packet()
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP. 
03/10/03    usb    Fixed ppp_set_resnc_options() to complete set of options,
                   Fixed ppp_set_v4_server_opts() to appropriately turn the
                   bits off for LOCAL side and made local_addr optional.
01/27/03    usb    PPP mode is always set in PS context, added 
                   PPP_SET_MODE_CMD
12/10/02    usb    Moved setting PPP mode to NONE from ppp_abort to 
                   pppi_abort (PS context).
11/14/02    jd/usb Split get_dev_opts into get_dev_setup & get_protocol_opts
                   removed 'swap' param from get dev opts, added 
                   get_bridge_dev_opts for this purpose.
                   Added set_resync_opts for getting iface ready for snoop
                   resync (sets will/want with given config)
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user data to ppp config options so that they can be 
                   passed on to ppp tx and rx functions
09/19/02    mvl    Added ppp_set_v4_server_opts() to make server config.
                   Making sure that the bridge device is set when switching
                   to psuedo or full net mode.  Fixed some issues with new
                   serves_addr field in configuration.
09/18/02    mvl    Added user_data to ppp event callbacks.  Setting mode in
                   start() and abort() calls, not internally.  Added explicit
                   configuration for client vs. server behavior.  Removed
                   do_mip.
09/12/02    mvl    Added support for a setup timeout, TASKLOCK()ed
                   ppp_set_mode().
09/06/02    mvl    fixed assert()s.
08/13/02    mvl    Session handle support and moved set_mode function from
                   netmdl.
08/06/02    mvl    Reworked authentication.
07/31/02    usb    Renamed psmisc.h to ps_utils.h
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
07/29/02    mvl    Setting do_auth variable when configuring PPP, fixed
                   TASKLOCK()s.
07/25/02    mvl    Fixed bug in get_default_opts() where values were not
                   being set correctly.  Updates for PPP renaming.
07/24/02    mvl    Fixed TASKLOCK() bug, and can now handle stopping/aborting
                   a device that is not running.
07/16/02    mvl    Configuration related fixes.
07/11/02    mvl    Updates for new PPP architecture.
07/02/02    mvl    Changes to decouple from PS.  Added ppp_reg_event().
05/22/02    mvl    created file with all of the external information for
                   users of ppp.
===========================================================================*/


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_PS
#include "assert.h"
#include "dsm.h"
#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_ppp_task.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_lcp.h"
#include "ps_ppp_ipcp.h"
#include "ps_utils.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
static int ppp_i_config
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *opts_ptr
);

#define PPP_BRANCH_TASKFREE() {TASKFREE()

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
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
  ppp_dev_opts_type *config_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(config_ptr, 0, sizeof(ppp_dev_opts_type));

  /*-------------------------------------------------------------------------
    The other default values have been set to zero by the memset above, but
    the HDLC mode, the bridge dev and the rx_signal need to be set explicitly
    to their non zero invalid settings.
  -------------------------------------------------------------------------*/
  config_ptr->hdlc_mode  = HDLC_INVALID_MODE;
  config_ptr->bridge_dev = PPP_INVALID_DEV;
  config_ptr->rx_signal  = PPP_INVALID_SIGNAL;

  /*-------------------------------------------------------------------------
    Set up LCP options to first request defaults - set the mode to active,
    then setup the masks: we want to negotiate ACCM, ACFC. PFC and the magic
    number (but NOT auth).  For the peer we will not force them to do
    anything, but will accept anything.  We will accept whatever is defined
    in lcp_accept.
  -------------------------------------------------------------------------*/
  config_ptr->lcp_info.mode = PPP_CP_ACTIVE;
  config_ptr->lcp_info.will_mask[FSM_REMOTE] = lcp_accept;
  config_ptr->lcp_info.will_mask[FSM_LOCAL]  = lcp_accept;
  config_ptr->lcp_info.want_mask[FSM_REMOTE] = 0;
  config_ptr->lcp_info.want_mask[FSM_LOCAL]  = LCP_N_ACCM | LCP_N_ACFC |
                                               LCP_N_PFC  | LCP_N_MAGIC;

  /*-------------------------------------------------------------------------
    setup the want and work protocol dependent variables using the default
    values - the only exception being the local want, where we prefer to have
    more optimized values for ACCM and we need to initialize our magic
    number.
  -------------------------------------------------------------------------*/
  memcpy((config_ptr->lcp_info.want_vals) + FSM_LOCAL,
         &lcp_default,
         sizeof(lcp_default));
  config_ptr->lcp_info.want_vals[FSM_LOCAL].accm = 0;
  config_ptr->lcp_info.want_vals[FSM_LOCAL].magic_number =
    (uint32)(&lcp_default) + msclock();
  memcpy((config_ptr->lcp_info.work_vals) + FSM_LOCAL,
         (config_ptr->lcp_info.want_vals) + FSM_LOCAL,
         sizeof(lcp_value_type));
  memcpy((config_ptr->lcp_info.want_vals) + FSM_REMOTE,
         &lcp_default,
         sizeof(lcp_default));
  memcpy((config_ptr->lcp_info.work_vals) + FSM_REMOTE,
         (config_ptr->lcp_info.want_vals) + FSM_REMOTE,
         sizeof(lcp_value_type));

  /*-------------------------------------------------------------------------
    Set up IPCP options to first request defaults - set the mode to active,
    then setupthe masks: we want to negotiate compression, an address and the
    DNS server addresses.  We will accept whatever is defined in ipcp_accept.
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.mode = PPP_CP_ACTIVE;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] = ipcp_accept;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL]  = ipcp_accept;
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] = IPCP_N_1ADDRESS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL]  = ipcp_accept;

  /*-------------------------------------------------------------------------
    setup the want and work protocol dependent variables using the default
    values - the only exception being the local want, where we want VJ
    compression and know how to set it up
  -------------------------------------------------------------------------*/
  memcpy((config_ptr->ipcp_info.want_vals) + FSM_LOCAL,
         &ipcp_default,
         sizeof(ipcp_default));
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].compression = PPP_VJCH_PROTOCOL;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].slots = IPCP_SLOT_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].slot_compress =
    IPCP_SLOT_COMPRESS;
  memcpy((config_ptr->ipcp_info.work_vals) + FSM_LOCAL,
         (config_ptr->ipcp_info.want_vals) + FSM_LOCAL,
         sizeof(ipcp_value_type));
  memcpy((config_ptr->ipcp_info.want_vals) + FSM_REMOTE,
         &ipcp_default,
         sizeof(ipcp_default));
  memcpy((config_ptr->ipcp_info.work_vals) + FSM_REMOTE,
         (config_ptr->ipcp_info.want_vals) + FSM_REMOTE,
         sizeof(ipcp_value_type));

} /* ppp_get_default_opts() */



/*===========================================================================
FUNCTION PPP_GET_PROTOCOL_OPTS()

DESCRIPTION
  This function will copy the framing options for a particular PPP
  device into the option structure provided by the caller.

PARAMETERS
  device: PPP device to copy options from
  config_ptr: pointer to configuration structure in which to put information

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure the specified device is valid
  -------------------------------------------------------------------------*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Copy the negotiated options - that is the work fields.
    First copy the protocol dependent variables
  -------------------------------------------------------------------------*/
  memcpy(&(config_ptr->lcp_info.work_vals[FSM_LOCAL]),
         ppp_cb_array[device].fsm[LCP].local.work_pdv,
         sizeof(lcp_value_type));
  memcpy(&(config_ptr->lcp_info.work_vals[FSM_REMOTE]),
         ppp_cb_array[device].fsm[LCP].remote.work_pdv,
         sizeof(lcp_value_type));
  memcpy(&(config_ptr->ipcp_info.work_vals[FSM_LOCAL]),
         ppp_cb_array[device].fsm[IPCP].local.work_pdv,
         sizeof(ipcp_value_type));
  memcpy(&(config_ptr->ipcp_info.work_vals[FSM_REMOTE]),
         ppp_cb_array[device].fsm[IPCP].remote.work_pdv,
         sizeof(ipcp_value_type));
  memcpy(&(config_ptr->lcp_info.want_vals[FSM_LOCAL]),
         ppp_cb_array[device].fsm[LCP].local.want_pdv,
         sizeof(lcp_value_type));
  memcpy(&(config_ptr->lcp_info.want_vals[FSM_REMOTE]),
         ppp_cb_array[device].fsm[LCP].remote.want_pdv,
         sizeof(lcp_value_type));
  memcpy(&(config_ptr->ipcp_info.want_vals[FSM_LOCAL]),
         ppp_cb_array[device].fsm[IPCP].local.want_pdv,
         sizeof(ipcp_value_type));
  memcpy(&(config_ptr->ipcp_info.want_vals[FSM_REMOTE]),
         ppp_cb_array[device].fsm[IPCP].remote.want_pdv,
         sizeof(ipcp_value_type));

  /*-------------------------------------------------------------------------
    Copy the negotatied masks (will and want)
  -------------------------------------------------------------------------*/
  config_ptr->lcp_info.will_mask[FSM_LOCAL] = 
    ppp_cb_array[device].fsm[LCP].local.will;
  config_ptr->lcp_info.will_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[LCP].remote.will;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] =
    ppp_cb_array[device].fsm[IPCP].local.will;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[IPCP].remote.will;
  config_ptr->lcp_info.want_mask[FSM_LOCAL] =
    ppp_cb_array[device].fsm[LCP].local.work;
  config_ptr->lcp_info.want_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[LCP].remote.work;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] =
    ppp_cb_array[device].fsm[IPCP].local.work;
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[IPCP].remote.work;

  return 0;
}



/*===========================================================================
FUNCTION PPP_GET_BRIDGE_DEV_PROT_OPTS()

DESCRIPTION
  This function will copy the framing options from a particular PPP
  device's bridged device into the specified config_ptr, swapping the
  local and remote options and values.

  This provides a means to obtain the options required to start ppp on the
  opposite side of an interface such that compatible ppp options are 
  negotiated on both devices.

PARAMETERS
  device: PPP device whose bridge device to copy options from
  config_ptr: pointer to configuration structure in which to put information

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_bridge_dev_prot_opts
(
  ppp_dev_enum_type bridge_dev,
  ppp_dev_opts_type *config_ptr
)
{
  ppp_type *  ppp_cb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Make sure the specified bridge device is valid
  -------------------------------------------------------------------------*/
  if(bridge_dev >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_ptr = &ppp_cb_array[bridge_dev];
 
  /*-------------------------------------------------------------------------
    Copy the desired options from the bridge device - 
    we want the negotiated, i.e. work options.
  -------------------------------------------------------------------------*/
  memcpy(&(config_ptr->lcp_info.want_vals[FSM_REMOTE]),
         ppp_cb_ptr->fsm[LCP].local.work_pdv,
         sizeof(lcp_value_type));
  memcpy(&(config_ptr->lcp_info.want_vals[FSM_LOCAL]),
         ppp_cb_ptr->fsm[LCP].remote.work_pdv,
         sizeof(lcp_value_type));
  memcpy(&(config_ptr->ipcp_info.want_vals[FSM_REMOTE]),
         ppp_cb_ptr->fsm[IPCP].local.work_pdv,
         sizeof(ipcp_value_type));
  memcpy(&(config_ptr->ipcp_info.want_vals[FSM_LOCAL]),
         ppp_cb_ptr->fsm[IPCP].remote.work_pdv,
         sizeof(ipcp_value_type));

  /*-------------------------------------------------------------------------
    Copy the working options to work options.
  -------------------------------------------------------------------------*/
  memcpy(&(config_ptr->lcp_info.work_vals[FSM_REMOTE]),
         ppp_cb_ptr->fsm[LCP].local.work_pdv,
         sizeof(lcp_value_type));
  memcpy(&(config_ptr->lcp_info.work_vals[FSM_LOCAL]),
         ppp_cb_ptr->fsm[LCP].remote.work_pdv,
         sizeof(lcp_value_type));
  memcpy(&(config_ptr->ipcp_info.work_vals[FSM_REMOTE]),
         ppp_cb_ptr->fsm[IPCP].local.work_pdv,
         sizeof(ipcp_value_type));
  memcpy(&(config_ptr->ipcp_info.work_vals[FSM_LOCAL]),
         ppp_cb_ptr->fsm[IPCP].remote.work_pdv,
         sizeof(ipcp_value_type));

  /*-------------------------------------------------------------------------
    We will only negotiate what the current settings are on the bridge 
    device.  Set want to the current settings on bridge device as well.
  -------------------------------------------------------------------------*/
  config_ptr->lcp_info.will_mask[FSM_REMOTE] = 
    ppp_cb_ptr->fsm[LCP].local.work;
  config_ptr->lcp_info.will_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[LCP].remote.work;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] =
    ppp_cb_ptr->fsm[IPCP].local.work;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[IPCP].remote.work;
  config_ptr->lcp_info.want_mask[FSM_REMOTE] =
    ppp_cb_ptr->fsm[LCP].local.work;
  config_ptr->lcp_info.want_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[LCP].remote.work;
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] =
    ppp_cb_ptr->fsm[IPCP].local.work;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[IPCP].remote.work;

  return 0;
}



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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    MSG_ERROR("Invalid dev 0x%x", device, 0, 0);
    return NULL;
  }
  
  if( ppp_cb_array[device].auth.mode != PPP_NO_AUTH )
  { 
    return &(ppp_cb_array[device].auth.info);
  }
  else
  {
    return NULL;
  }

} /* ppp_get_auth_info() */



/*===========================================================================
FUNCTION PPP_GET_DEV_SETUP()

DESCRIPTION
  This function will copy the configuration options for a particular PPP
  device into the option structure passed in.

PARAMETERS
  device: PPP device to copy settings from
  config_ptr: pointer to configuration structure in which to put information

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_dev_setup
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
)
{
  ppp_type * ppp_cb_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Make sure the specified device is valid
  -------------------------------------------------------------------------*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get fields from specified device.
  -------------------------------------------------------------------------*/
  ppp_cb_ptr = &ppp_cb_array[device];

  config_ptr->rx_f_ptr      = ppp_cb_ptr->rx_f_ptr;
  config_ptr->tx_f_ptr      = ppp_cb_ptr->tx_f_ptr;
  config_ptr->rx_user_data  = ppp_cb_ptr->rx_user_data;
  config_ptr->tx_user_data  = ppp_cb_ptr->tx_user_data;
  config_ptr->rx_iface_ptr  = ppp_cb_ptr->rx_iface_ptr;
  config_ptr->hdlc_mode     = ppp_cb_ptr->hdlc_mode;
  config_ptr->rx_signal     = ppp_cb_ptr->rx_signal;
  config_ptr->bridge_dev    = ppp_cb_ptr->bridge_dev;
  config_ptr->serves_addrs  = ppp_cb_ptr->serves_addrs;
  config_ptr->setup_timeout = ppp_cb_ptr->setup_timeout;

  /*-------------------------------------------------------------------------
    if the auth flags indicate that we authenticate the remote side then the
    auths_peer field is true, otherwise not.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->auth.flags & PPP_AP_REMOTE)
  {
    config_ptr->auths_peer = TRUE;
    /*-----------------------------------------------------------------------
      Copy the auth info in this case
    -----------------------------------------------------------------------*/
    config_ptr->auth_info = ppp_cb_ptr->auth.info;
  }
  else
  {
    config_ptr->auths_peer = FALSE;
    /*-----------------------------------------------------------------------
      Null out the auth info in this case
    -----------------------------------------------------------------------*/
    memset(&(config_ptr->auth_info), 0, sizeof(ppp_auth_info_type));
  }

  return 0;

} /* ppp_get_dev_opts() */



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
)
{
  ppp_fsm_type * fsm_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  /*-------------------------------------------------------------------------
    Make sure the specified device is valid
  -------------------------------------------------------------------------*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }

  if(config_ptr == NULL)
  {
    MSG_ERROR("Invalid config", 0, 0, 0);
    return;
  }

  fsm_ptr = &(ppp_cb_array[device].fsm[IPcp]);

  /*-------------------------------------------------------------------------
    set local will, want and work masks as provided
  -------------------------------------------------------------------------*/
  fsm_ptr->local.will  = config_ptr->ipcp_info.will_mask[FSM_LOCAL];
  fsm_ptr->local.want  = config_ptr->ipcp_info.want_mask[FSM_LOCAL];
  fsm_ptr->remote.will = config_ptr->ipcp_info.will_mask[FSM_REMOTE];
  fsm_ptr->remote.want = config_ptr->ipcp_info.want_mask[FSM_REMOTE];

  fsm_ptr->local.work  = fsm_ptr->local.want;
  fsm_ptr->remote.work = fsm_ptr->remote.want;

  /*-------------------------------------------------------------------------
    Copy the want and work values as provided
  -------------------------------------------------------------------------*/
  memcpy(fsm_ptr->remote.want_pdv,
         &(config_ptr->ipcp_info.want_vals[FSM_REMOTE]),
         sizeof(ipcp_value_type));
  memcpy(fsm_ptr->local.want_pdv,
         &(config_ptr->ipcp_info.want_vals[FSM_LOCAL]),
         sizeof(ipcp_value_type));
  memcpy(fsm_ptr->remote.work_pdv,
         &(config_ptr->ipcp_info.work_vals[FSM_REMOTE]),
         sizeof(ipcp_value_type));
  memcpy(fsm_ptr->local.work_pdv,
         &(config_ptr->ipcp_info.work_vals[FSM_LOCAL]),
         sizeof(ipcp_value_type));
  
  /*-------------------------------------------------------------------------
     Copy the flag which indicates if we are IPCP client or server
  -------------------------------------------------------------------------*/
  ppp_cb_array[device].serves_addrs = config_ptr->serves_addrs;

} /* ppp_update_ipcp_config() */


/*===========================================================================
FUNCTION PPP_SET_V4_SERVER_OPTS()

DESCRIPTION
  This function will change the configuration passed in to serve IP
  addresses.  This includes setting the serves_addrs field, and setting the
  appropriate IPCP bits. An address must be passed in to be assigned to the
  peer. Other fields could be NULL and if so the corresponding bits are
  turned off.

PARAMETERS
  config_ptr:    configuration to convert
  local_addr:    the local IP address to advertize
  remote_addr:   the IP address to assign to peer
  primary_dns:   the primary DNS address to assign to peer
  secondary_dns: the secondary DNS address to assign to peer

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  if(config_ptr  == NULL ||
     remote_addr == 0)
  {
    MSG_ERROR("Invalid config 0x%x 0x%x", config_ptr, remote_addr, 0);
    return;
  }

  config_ptr->serves_addrs = TRUE;

  /*-------------------------------------------------------------------------
    Set the remote address,  and associated bits in the masks. Since we
    are acting as server we must have an address to assign.
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
  config_ptr->ipcp_info.work_vals[FSM_REMOTE].address = remote_addr;
  config_ptr->ipcp_info.want_vals[FSM_REMOTE].address = remote_addr;

  /*-------------------------------------------------------------------------
    if the local address is available, assign the values and set the 
    LOCAL bits. Else turn them off since we don't want to ask for our
    address while acting as server.
  -------------------------------------------------------------------------*/
  if(local_addr != 0)
  {
    config_ptr->ipcp_info.want_mask[FSM_LOCAL] |= IPCP_N_1ADDRESS;
    config_ptr->ipcp_info.will_mask[FSM_LOCAL] |= IPCP_N_1ADDRESS;
    config_ptr->ipcp_info.work_vals[FSM_LOCAL].address = local_addr;
    config_ptr->ipcp_info.want_vals[FSM_LOCAL].address = local_addr;
  }
  else
  {
    config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
    config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
  }

  /*-------------------------------------------------------------------------
    If we have dns address to assign to peer then assign the values and set 
    the REMOTE bits. LOCAL bits should always be turned off since we don't
    want to ask for DNS addresses while acting as server. 
  -------------------------------------------------------------------------*/
  if(primary_dns != 0)
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_PRIMARY_DNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].primary_dns =
      primary_dns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].primary_dns =
      primary_dns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_DNS;
  }
  
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_DNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_DNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_DNS;

  if(secondary_dns != 0)
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_SECONDARY_DNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].secondary_dns =
      secondary_dns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].secondary_dns =
      secondary_dns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_DNS;
  }

  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_DNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_DNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_DNS;

} /* ppp_set_v4_server_opts() */



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
)
{
  int copy_len = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(config_ptr == NULL)
  {
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set all of the authentication options up to do server style CHAP.
  -------------------------------------------------------------------------*/
  config_ptr->auths_peer = TRUE;
  config_ptr->lcp_info.work_vals[FSM_LOCAL].authentication =
    PPP_CHAP_PROTOCOL;
  config_ptr->lcp_info.work_vals[FSM_LOCAL].chap_digest = PPP_CHAP_MD5;
  config_ptr->lcp_info.want_vals[FSM_LOCAL].authentication =
    PPP_CHAP_PROTOCOL;
  config_ptr->lcp_info.want_vals[FSM_LOCAL].chap_digest = PPP_CHAP_MD5;
#ifdef FEATURE_NO_PPP_AUTH //Gemsea Add for Disable PPP Auth
  config_ptr->lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;
  config_ptr->lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
#else
  config_ptr->lcp_info.want_mask[FSM_LOCAL] |= LCP_N_AP;
  config_ptr->lcp_info.will_mask[FSM_LOCAL] |= LCP_N_AP;
#endif
  /*-------------------------------------------------------------------------
    If the challenge name is non-NULL and the length is non-zero copy the
    challenge name.  Otherwise, set the challenge name length to 0.
  -------------------------------------------------------------------------*/
  if(challenge_name != NULL)
  {
    copy_len = strlen(challenge_name);
    copy_len = MIN(copy_len, PPP_CHAP_CHAL_NAME_LEN);
    config_ptr->auth_info.challenge_name_len = copy_len;
    memcpy(config_ptr->auth_info.challenge_name, challenge_name, copy_len);
  }
  else
  {
    config_ptr->auth_info.challenge_name_len = 0;
  }

  return copy_len;

} /* ppp_set_auth_server_opts() */


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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV ||
     event >= PPP_MAX_EV)
  {
    ASSERT(0);
    return -1;
  }

  TASKLOCK();
  ppp_cb_array[device].event_info[event].cback = cback_f;
  ppp_cb_array[device].event_info[event].user_data = user_data;
  TASKFREE();

  return 0;

} /* ppp_reg_event() */



/*===========================================================================
FUNCTION PPP_START()

DESCRIPTION
  This function is used by PPP clients to start a particular PPP client with
  a specific configuration.  The caller is ALWAYS required to pass in a
  configuration which should be initialized to some set of values - the
  ppp_get*_opts() function can be used to configure the configuration
  structure.

  Fields that MUST be set in the config structure:
    rx_iface_ptr
    rx_f_ptr

PARAMETERS
  device: the PPP device to be started
  config_ptr: pointer to the configuration with which to start PPP device
          'device'

RETURN VALUE
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or some configuration is bad.
  session handle of 0 or greater on success.  This can be used to associate
  PPP events with this instance of PPP.

DEPENDENCIES
  None

SIDE EFFECTS
  The configuration that is passed in will be copied to the PPP device
  IMMEDIATELY (if the device is free).  This means that the PPP context is
  modified in the callers task context - it also means that the memory for
  configuration passed in is free as soon as this function returns.
===========================================================================*/
int ppp_start
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
)
{
  static uint16 session_handle = 1;
  ppp_type *ppp_cb_ptr;
  int start_info;
  pppi_start_info_type *start_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV ||
     config_ptr == NULL)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_ptr = ppp_cb_array + device;
  start_info_ptr  = (pppi_start_info_type*)(&start_info);
  /*-------------------------------------------------------------------------
    make sure the rx_f_ptr, and the rx_iface_ptr pointers are set
  -------------------------------------------------------------------------*/
  if(config_ptr->rx_f_ptr == NULL ||
     config_ptr->tx_f_ptr == NULL ||
     config_ptr->rx_iface_ptr == NULL)
  {
    MSG_HIGH("rx_f %x, tx_f %x or dest_if %x invalid!",
             config_ptr->rx_f_ptr,
             config_ptr->tx_f_ptr,
             config_ptr->rx_iface_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Configure PPP device
  -------------------------------------------------------------------------*/
  if(ppp_i_config(device, config_ptr) < 0)
  {
    MSG_HIGH("ppp dev %d config failed!", device, 0, 0);
    return -1;
  }

  if( ppp_cb_ptr->session_handle == 0 )
  {
    ppp_cb_ptr->session_handle = session_handle++;
  }

  /*-------------------------------------------------------------------------
    Send the command to the PPP task which will do the actual processing
  -------------------------------------------------------------------------*/
  start_info_ptr->device = device;
  start_info_ptr->session_handle = ppp_cb_ptr->session_handle;
  PPP_SEND_CMD(PPP_START_CMD, (void*)start_info);

  return start_info_ptr->session_handle;

} /* ppp_start() */



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

)
{
  ppp_type *ppp_cb_ptr;
  int start_info;
  pppi_start_info_type *start_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_ptr = ppp_cb_array + device;
  start_info_ptr  = (pppi_start_info_type*)(&start_info);


  TASKLOCK();
  /*-------------------------------------------------------------------------
    Make sure that the device is configured
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->session_handle == 0)
  {
    MSG_MED("ppp_resync on free device(%d)!", device, 0, 0);
    PPP_BRANCH_TASKFREE();
    return -1;
  }

  /*-------------------------------------------------------------------------
    If configuration is passed in try configuring the interface before
    resyncing
  -------------------------------------------------------------------------*/
  if(config_ptr != NULL)
  {
    if(config_ptr->lcp_info.mode != PPP_CP_ACTIVE ||
       config_ptr->rx_f_ptr == NULL               ||
       config_ptr->tx_f_ptr == NULL               ||
       config_ptr->rx_iface_ptr == NULL)
    {
      MSG_HIGH("mode inactive or rx_f %x, tx_f %x dest_if %x invalid!",
               config_ptr->rx_f_ptr,
               config_ptr->tx_f_ptr,
               config_ptr->rx_iface_ptr);
      PPP_BRANCH_TASKFREE();
      return -1;
    }

    config_ptr->rx_signal = ppp_cb_ptr->rx_signal;
    if(ppp_i_config(device, config_ptr) < 0)
    {
      MSG_HIGH("resync failed because config is bad!", 0, 0, 0);
      PPP_BRANCH_TASKFREE();
      return -1;
    }
  } /* if(re-configuing) */

  /*-------------------------------------------------------------------------
    we are resyncing so call the callback.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->event_info[PPP_RESYNC_EV].cback != NULL)
  {
    MSG_MED ("Resync PPP",0,0,0);
    ppp_cb_ptr->event_info[PPP_RESYNC_EV].cback(
      device,
      protocol,
      PPP_RESYNC_EV,
      ppp_cb_ptr->event_info[PPP_RESYNC_EV].user_data,
      ppp_cb_ptr->session_handle,
      PPP_FAIL_REASON_NONE
    );
  }
  TASKFREE();
  /*-------------------------------------------------------------------------
    Send the command to the executing task
  -------------------------------------------------------------------------*/
  start_info_ptr->device = device;
  start_info_ptr->session_handle = ppp_cb_ptr->session_handle;
  PPP_SEND_CMD(PPP_START_CMD, (void*)start_info);

  return 0;

} /* ppp_resync() */



/*===========================================================================
FUNCTION PPP_STOP()

DESCRIPTION
  This function is used by PPP clients to stop a particular PPP device.
  NOTE: if the graful_stop flag for this PPP device is false, this will cause
  an abort() rather than doing stop() processing.

PARAMETERS
  device: the PPP device to be closeed

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
int ppp_stop
(
  ppp_dev_enum_type  device
)
{
  ppp_type *ppp_cb_ptr;
  int ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_ptr = ppp_cb_array + device;

  TASKLOCK();
  if(ppp_cb_ptr->session_handle == 0)
  {
    MSG_MED("ppp_stop(%d) on free device!", device, 0, 0);
  }
  else
  {
    /*-----------------------------------------------------------------------
      If we are not supposed to stop gracefully and the physical link is down
      then do abort processing, otherwise send stop_cmd
    -----------------------------------------------------------------------*/
    if(ps_iface_phys_link_state(ppp_cb_ptr->rx_iface_ptr) != PHYS_LINK_UP &&
       ppp_cb_ptr->rx_iface_ptr->graceful_dormant_close == FALSE)
    {
      ret_val = ppp_abort(device);
    }
    else
    {
      PPP_SEND_CMD(PPP_STOP_CMD, (void*)device);
    }
  }
  TASKFREE();

  return ret_val;
} /* ppp_stop() */



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
)
{
  ppp_type *ppp_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_ptr = ppp_cb_array + device;

  TASKLOCK();
  if(ppp_cb_ptr->session_handle == 0)
  {
    MSG_MED("ppp_abort(%d) on free device!", device, 0, 0);
  }
  else
  {
    PPP_DISABLE_SIG(ppp_cb_ptr->rx_signal);
    /*-----------------------------------------------------------------------
      As the device is being released, reset the rx_iface pointer to indicate
      this, and set the rx and tx function pointers to the default values.
    -----------------------------------------------------------------------*/
    ps_iface_reset_peer_data(ppp_cb_ptr->rx_iface_ptr);
    ppp_cb_ptr->rx_f_ptr = pppi_default_rx;
    ppp_cb_ptr->tx_f_ptr = pppi_default_tx;
    ppp_cb_ptr->rx_user_data = NULL;
    ppp_cb_ptr->tx_user_data = NULL;
    ppp_cb_ptr->session_handle = 0;

    PPP_SEND_CMD(PPP_ABORT_CMD, (void*)device);
  }
  TASKFREE();

  return 0;

} /* ppp_abort() */


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
)
{
  ppp_type *ppp_cb_ptr;
  int mode_info;
  pppi_mode_info_type *mode_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }

  ppp_cb_ptr = ppp_cb_array + device;

  TASKLOCK()
  if(ppp_cb_ptr->session_handle == 0)
  {
    MSG_MED("ppp_set_mode(%d) on free device(%d)!", new_mode, device, 0);
  }
  else
  {
    MSG_MED("dev %d mode from %d to %d",
            device,
            ppp_cb_ptr->mode,
            new_mode);
  
    switch(new_mode)
    {
      case PPP_PSEUDO_NET_MODE:
      case PPP_FULL_NET_MODE:
        if(ppp_cb_ptr->bridge_dev >= PPP_MAX_DEV)
        {             
          ERR("PPP %d could not set mode %d, no bridge dev",
              device, new_mode, 0 ); 
          ASSERT(0);
          break;
        }
        /* fall through */
  
      case PPP_RELAY_MODE:
      case PPP_INTERNAL_MODE:
      case PPP_INTERNAL_SNOOP_MODE:
      case PPP_NONE_MODE:
        /*-------------------------------------------------------------------
          Since changing a mode can impact how PPP processes data, it should
          only be changed in the same context in which PPP is running to 
          avoid any race conditions. If the caller is calling from the same 
          context, set the new mode here else send a cmd to the PPP task.
        -------------------------------------------------------------------*/
        if(PPP_IS_CURR_TASK_OWNER())
        {
          ppp_cb_ptr->mode = new_mode;
        }
        else
        {
          mode_info_ptr = (pppi_mode_info_type*) &mode_info;
          mode_info_ptr->device = device;
          mode_info_ptr->mode = new_mode;
          PPP_SEND_CMD(PPP_SET_MODE_CMD, (void*)mode_info);
        }
  
        break;
  
      default:
        /*-------------------------------------------------------------------
          Invalid mode!
        -------------------------------------------------------------------*/
        ERR("PPP %d, invalid mode %d", device, new_mode, 0); 
        ASSERT(0);
    } /* switch(device mode) */
  }
  TASKFREE();

} /* ppp_set_mode() */



/*===========================================================================
FUNCTION PPP_GET_MODE()

DESCRIPTION
  This function returns the current mode of the interface in question.

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    MSG_ERROR("Invalid dev 0x%x", device, 0, 0);
    return PPP_NONE_MODE;
  }

  return ppp_cb_array[device].mode;

} /* ppp_get_mode() */


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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid dev 0x%x in get phase", device, 0, 0);
    ASSERT(0);
    return pppDEAD;
  }

  return ppp_cb_array[device].phase;
} /* ppp_get_phase() */


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
  device:        PPP device to configure
  ppp_pkt_ptr:   Pointer to PPP packet to be injected

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
  ppp_dev_enum_type device,               /* PPP device to pass pkt to     */
  uint16            protocol,             /* Protocol of packet            */
  dsm_item_type     *ppp_pkt_ptr          /* Pointer to PPP pkt            */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    MSG_ERROR("Invalid dev 0x%x", device, 0, 0);
    return -1;
  }
  
  if(ppp_cb_array[device].session_handle == 0)
  {
    MSG_MED("ppp_inject_packet(%d) on free device!", device, 0, 0);
    return -1;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Put device in lower 16 bit and protocol in upper 16 bit of the 
      app_field of the pkt ptr. Post cmd to PS to start processing the 
      specified pkt and pass the pkt ptr as the cmd param.
    -----------------------------------------------------------------------*/
    ppp_pkt_ptr->app_field = (protocol << 16) | device;
    PPP_SEND_CMD(PPP_INJECT_PKT_CMD, (void*)ppp_pkt_ptr);
  }

  return 0;
} /* ppp_inject_packet() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPPI_CONFIG()

DESCRIPTION
  This function is used by PPP clients to abort a particular PPP device.

PARAMETERS
  opts_ptr: pointer to the configuration options to config with

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  This is expected to be called from one of the public functions and so
  parameter checking is not performed.

SIDE EFFECTS
  None
===========================================================================*/
static int ppp_i_config
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
)
{
  ppp_type *ppp_cb_ptr;

#ifdef FEATURE_HDLC_HW_ACCEL
  int loop;
#endif /* FEATURE_HDLC_HW_ACCEL */
  int ret_val = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    MSG_ERROR("Invalid dev 0x%x", device, 0, 0);
    return -1;
  }

  ppp_cb_ptr = ppp_cb_array + device;

  TASKLOCK();
  /*-------------------------------------------------------------------------
    Make sure PPP device is free or being set up for same iface
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->rx_iface_ptr != NULL &&
     ppp_cb_ptr->rx_iface_ptr != config_ptr->rx_iface_ptr)
  {
    ASSERT(0);
    ERR("PPP Device %d already in use!", device, 0, 0);
    ret_val = -1;
  }

  /*-------------------------------------------------------------------------
    The device is free, so make sure configuration is good.
  -------------------------------------------------------------------------*/
  else if(
    config_ptr->rx_f_ptr       == NULL                                  ||
    config_ptr->tx_f_ptr       == NULL                                  ||
    config_ptr->hdlc_mode      == HDLC_INVALID_MODE                     ||
    config_ptr->lcp_info.mode  == PPP_CP_INVALID_MODE                   ||
    config_ptr->lcp_info.mode  == PPP_CP_DISABLED                       ||
    config_ptr->ipcp_info.mode == PPP_CP_INVALID_MODE                   ||
    (config_ptr->lcp_info.want_mask[FSM_LOCAL]   & (~lcp_accept))  != 0 ||
    (config_ptr->lcp_info.will_mask[FSM_LOCAL]   & (~lcp_accept))  != 0 ||
    (config_ptr->ipcp_info.want_mask[FSM_LOCAL]  & (~ipcp_accept)) != 0 ||
    (config_ptr->ipcp_info.will_mask[FSM_LOCAL]  & (~ipcp_accept)) != 0 ||
    (config_ptr->lcp_info.want_mask[FSM_REMOTE]  & (~lcp_accept))  != 0 ||
    (config_ptr->lcp_info.will_mask[FSM_REMOTE]  & (~lcp_accept))  != 0 ||
    (config_ptr->ipcp_info.want_mask[FSM_REMOTE] & (~ipcp_accept)) != 0 ||
    (config_ptr->ipcp_info.will_mask[FSM_REMOTE] & (~ipcp_accept)) != 0 ||
    config_ptr->auth_info.user_id_len        > PPP_MAX_USER_ID_LEN      ||
    config_ptr->auth_info.passwd_len         > PPP_MAX_PASSWD_LEN       ||
    config_ptr->auth_info.challenge_len      > PPP_CHAP_CHAL_LEN        ||
    config_ptr->auth_info.challenge_name_len > PPP_CHAP_CHAL_NAME_LEN)
  {
    ERR("Invalid configuration param(s)!", 0, 0, 0);
    ret_val = -1;
  }

  /*-------------------------------------------------------------------------
    The the confugration is good and device is free so configure it.
  -------------------------------------------------------------------------*/
  else
  {
    ppp_cb_ptr->phase     = pppDEAD;
    ppp_cb_ptr->auth.mode = PPP_NO_AUTH;
    ppp_cb_ptr->rx_iface_ptr   = config_ptr->rx_iface_ptr;
    ppp_cb_ptr->pend_tx_f_ptr  = config_ptr->tx_f_ptr;
    ppp_cb_ptr->pend_rx_f_ptr  = config_ptr->rx_f_ptr;
    ppp_cb_ptr->pend_tx_user_data  = config_ptr->tx_user_data;
    ppp_cb_ptr->pend_rx_user_data  = config_ptr->rx_user_data;
    ppp_cb_ptr->bridge_dev     = config_ptr->bridge_dev;
    ppp_cb_ptr->serves_addrs   = config_ptr->serves_addrs;
    ppp_cb_ptr->fsm[LCP].mode  = config_ptr->lcp_info.mode;
    ppp_cb_ptr->fsm[IPCP].mode = config_ptr->ipcp_info.mode;

    /*-----------------------------------------------------------------------
      if the auths peer variable is set then set the flags field in the auth
      info section of the control block to indicate that we are
      authenticating the peer - otherwise set it to indicate that we are
      being authenticated.

      NOTE: this implies that mutual authentication is NOT possible, i.e. we
            can only be a client OR a server not both.
    -----------------------------------------------------------------------*/
    if(config_ptr->auths_peer)
    {
      ppp_cb_ptr->auth.flags = PPP_AP_REMOTE;
    }
    else
    {
      ppp_cb_ptr->auth.flags = PPP_AP_LOCAL;
    }

    /*-----------------------------------------------------------------------
      if the value of the rx_signal is invalid then use the default 1x signal
      otherwise use the passed in value
    -----------------------------------------------------------------------*/
    if(config_ptr->rx_signal == PPP_INVALID_SIGNAL)
    {
      ppp_cb_ptr->rx_signal = ppp_dev_to_default_sig_array[device];
    }
    else
    {
      ppp_cb_ptr->rx_signal = config_ptr->rx_signal;
    }

    /*-----------------------------------------------------------------------
      If the setup timeout is set to a value less than the MIN_SETUP_TIME
      then set it to that value.  This enforces that the value is at least
      set to something reasonable.
    -----------------------------------------------------------------------*/
#define PPP_MIN_SETUP_TIME 5
    if(config_ptr->setup_timeout > 0 &&
       config_ptr->setup_timeout < PPP_MIN_SETUP_TIME)
    {
      config_ptr->setup_timeout = PPP_MIN_SETUP_TIME;
    }
    ppp_cb_ptr->setup_timeout  = config_ptr->setup_timeout * 1000;

    /*-----------------------------------------------------------------------
      Only set the HDLC mode if it is not already configured, HDLC mode 
      cannot be switched between HW and SW in the middle of a call.
      If trying for hardware, then eith ignore if the feature is not defined,
      or check if the framer is already in use...
    -----------------------------------------------------------------------*/
    if(ppp_cb_ptr->hdlc_mode == HDLC_INVALID_MODE)
    {
      if(config_ptr->hdlc_mode == HDLC_HW_PREF)
      {
#ifdef FEATURE_HDLC_HW_ACCEL
        ppp_cb_ptr->hdlc_mode = HDLC_HW;
        /*-------------------------------------------------------------------
          If HW accelleration is supported, have to make sure that no one 
          else is using it.  If any block is using hardware already then set 
          this one up to use SW - skip our own control block.
        -------------------------------------------------------------------*/
        for(loop = 0; loop < PPP_MAX_DEV; loop++)
        {
          if(loop != device &&  /* no need to check device being configured  */
             ppp_cb_array[loop].rx_iface_ptr != NULL &&
             ppp_cb_array[loop].hdlc_mode == HDLC_HW)
          {
            ppp_cb_ptr->hdlc_mode = HDLC_SW;
            break;
          }
        } /* for(all PPP devs) */
#else
        /*-------------------------------------------------------------------
          There is no HW support, so set to software
        -------------------------------------------------------------------*/
        ppp_cb_ptr->hdlc_mode = HDLC_SW;
#endif /* FEATURE_HDLC_HW_ACCEL */
      }  /* if(tried for HW) */

      /*---------------------------------------------------------------------
        Not trying for hardware, so setup to fulfill request
      ---------------------------------------------------------------------*/
      else
      {
        ppp_cb_ptr->hdlc_mode = config_ptr->hdlc_mode;
      }
    } /* if(hdlc_mode already set) */

    /*-----------------------------------------------------------------------
      copy the auth information
    -----------------------------------------------------------------------*/
    memcpy(&(ppp_cb_ptr->auth.info),
           &(config_ptr->auth_info),
           sizeof(ppp_auth_info_type));

    /*-----------------------------------------------------------------------
      Initialize LCP and IPCP and enable the signal in the exectuing task
    -----------------------------------------------------------------------*/
    lcp_init(device, config_ptr);
    ipcp_init(device, config_ptr);

  } /* else(ppp device free) */

  TASKFREE();
  return ret_val;

} /* ppp_i_config() */

#endif /* FEATURE_DATA_PS */

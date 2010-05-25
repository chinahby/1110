#ifndef PS_PPP_NETMODEL_H
#define PS_PPP_NETMODEL_H
/*===========================================================================

                      P S _ P P P _ N E T M O D E L . H

DESCRIPTION

 The PPP Network model header file. Contains shared variables and enums, as
 well as function prototypes.

Copyright (c) 1998-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_netmodel.h_v   1.3   15 Nov 2002 23:37:30   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_netmodel.h#1 $
  $Author: nsivakum $ $DateTime: 2007/11/05 03:25:37 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/26/03    mvl    removed authentication featurization
11/14/02   jd/usb  Added protoype for netmdl_set_resync_behaviour() and
                   netmdl_drop_behav_enum_type for parameter
11/04/02    mvl    Moved some #defines that were only used in .c to the .c.
08/13/02    mvl    Removed the iface_cb_array (all the other items are in the
                   PPP cb) moved the set/get_mode() functions to pppi.h.
07/25/02    mvl    Moved input params to ppp control block.  Updates for PPP
                   renaming. 
07/11/02    mvl    Updated for new PPP architecture.
05/23/02    mvl    Renamed iface_stack_type.
05/20/02     vr    Removed hdlc_enabled flag from iface_cb_type
02/07/02    vsk    Added ip_pkt_hdlr_f_ptr IP packet handler callback
                   to iface_cb_type.
11/12/01    dwp    Added hdlc_enabled to iface_cb_type.
07/24/01    sjy    Added prototypes for netmdl_compare_lcp_opts() and 
                   netmdl_compare_ipcp_opts().
07/22/01    mvl    removed iface_stack_enum_type and accompanying macro as
                   they have been moved to dsiface_def.h
06/21/01    mvl    Added prototypes for netmdl_get_meta_state() and
                   netmdl_reset_ppp()
05/18/01    mvl    Added prototype for netmdl_get_mode().
04/24/00    snn    Added support for the CHAP resync.
10/27/99    mvl    Code review changes and clean up.
08/20/99    mvl    Changed OUT_IFACE() macro so compiler doesn't throw
                   warnings.
08/10/99    snn    added support for DS_PAP feature in network model.
04/12/99    mvl    moved the OUT_IFACE() macro from the C file to this file.
10/04/98    mvl    created file
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ps_ppp_defs.h"
#include "dsm.h"


/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Enum for the PPP state.
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_OOC_META_STATE,                             /* the out of call state */
  PPP_INIT_META_STATE,                            /* the init state        */
  PPP_UP_META_STATE,                              /* the UP state          */
  PPP_RESYNC_META_STATE,                          /* the resync state      */
  PPP_RELAY_META_STATE                            /* the relay state       */
} netmdl_meta_state_enum_type;

/*---------------------------------------------------------------------------
  Enum for the PPP resync behaviour.
---------------------------------------------------------------------------*/
typedef enum
{
  NETMDL_RESYNC_MIN = 0,
  NETMDL_RESYNC_DFLT = 0,        /* default behaviour */
  NETMDL_RESYNC_DROP_CALL = 1,   /* force call to drop on resync opt mismatch */
  NETMDL_RESYNC_MAX
} netmdl_drop_behav_enum_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION   NETMDL_SET_META_STATE()

DESCRIPTION
  This function sets the state of the PPP Meta state machine.  This is mostly
  registereing the callbacks that are associated with that state.  The
  notable exception to this is the Resync State, which doesn't have any
  callback associated with it, but does have a fairly complex algorithm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void netmdl_set_meta_state
(
  netmdl_meta_state_enum_type new_ppp_meta_state /* the new meta state     */
);

/*===========================================================================
FUNCTION NETMDL_GET_META_STATE()

DESCRIPTION
  This function returns the current state of the network meta state machine.

PARAMETERS
  None

RETURN VALUE
  The current network model meta state machine state.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
netmdl_meta_state_enum_type netmdl_get_meta_state
(
  void
);


#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION NETMDL_RESET_PPP()

DESCRIPTION
  This function will spoof Term Requests over both the Um adn Rm interfaces.

PARAMETERS
  kill_ppp: if this is TRUE tear down the call, otherwise spoof a C-Req to
  reset the connection.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void netmdl_reset_ppp
(
  boolean kill_ppp
);

/*===========================================================================
  FUNCTION NETMDL_COMPARE_LCP_OPTS()

  DESCRIPTION
    Compares the LCP options on the Rm and Um.

  PARAMETERS
    None

  RETURN VALUE
    TRUE - Rm and Um LCP options match
    FALSE - Rm and Um LCP options differ

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean netmdl_compare_lcp_opts
(
  void
);

/*===========================================================================
  FUNCTION NETMDL_COMPARE_ICPC_OPTS()

  DESCRIPTION
    Compares the ICPC options on the Rm and Um.

  PARAMETERS
    TRUE - Rm and Um ICPC options match
    FALSE - Rm and Um ICPC options differ

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean netmdl_compare_ipcp_opts
(
  void
);

#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
  FUNCTION NETMDL_SET_RESYNC_BEHAVIOUR

  DESCRIPTION
    Accessor function to force resync failure to drop call

  PARAMETERS
    NETMDL_RESYNC_DLFT      - do the default behaviour
    NETMDL_RESYNC_DROP_CALL - force call to drop on resync failure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void netmdl_set_resync_behaviour
(
  netmdl_drop_behav_enum_type behav
);

#endif /* PS_PPP_NETMODEL_H */

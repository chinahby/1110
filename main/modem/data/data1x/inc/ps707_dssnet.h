#ifndef PS707_DSSNET_H
#define PS707_DSSNET_H
/*===========================================================================

             S O C K E T S  N E T W O R K   H E A D E R  F I L E
                   
DESCRIPTION

  The Data Services Sockets Network state machine header file (This is an
  INTERNAL sockets file)

EXTERNALIZED FUNCTIONS
 
  dssnet_sm_post_event()
    Post events to the DSSNET state machine
  dssnet_sm_init()
    Initialize the DSSNET State machine

Copyright (c) 1998-2011 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps707_dssnet.h_v   1.3   28 Feb 2003 10:28:02   sramacha  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ps707_dssnet.h#3 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
04/29/09    sn     Ported support for call throttle feature (DCTM).
05/05/03    jd     Removed dssnet_get_iface_status, dssnet_set_close_reason,
04/10/03    ss     Moved dss_iface_status_type typedef to this file from
                   dssocket.h
02/21/03    jd     moved & renamed ppp_auth_get_from_nv() from ps_ppp_auth.c
                   to is707_get_ppp_auth_info_from_nv() in this file
11/14/02    jd     added dssnet_override_qcmip()
08/06/02    mvl    Added include for ps_ppp_defs
08/02/02    aku    Added interface to notify dssnet that it is a QNC call.
07/25/02    mvl    Added PVCS Header and removed netmodel include.
07/22/02    aku    Added ppp_dev_enum_type arg to dssnet_get_iface_status()
                   to be backward compatible.
                   Added new event DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD_EV.
07/16/02    aku    Remove SOCKETS featurization.
07/14/02    aku    Initial version for multimode PS.

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "dssocki.h"
#include "ps_ppp_defs.h"
#include "ps_ppp.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
#define QCMIP_SIP_ONLY         0        /* qcmip value set to SIP only     */
#define QCMIP_MIP_PREFERRED    1        /* qcmip value set to MIP preferred
                                           with option to fallback to SIP  */
#define QCMIP_MIP_ONLY         2        /* qcmip value set to MIP only     */

/*---------------------------------------------------------------------------
 Type and static variable defining events received by the dssnet_sm.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSNET_PPP_OPEN_CMD_EV = 0,
  DSSNET_PHY_IFACE_UP_EV = 1,
  DSSNETI_PHY_IFACE_TIMER_EXP_EV = 2,
  DSSNET_LINK_LAYER_UP_WITH_MIP_EV = 3,
  DSSNET_LINK_LAYER_UP_WITH_SIP_EV = 4,
  DSSNET_MIP_UP_SUCCESS_EV = 5,
  DSSNET_MIP_FAILURE_EV = 6,
  DSSNET_NET_IFACE_DOWN_EV = 7,
  DSSNET_PPP_CLOSE_CMD_EV = 8,
  DSSNET_PHY_IFACE_DOWN_EV = 9,
  DSSNET_PPP_RESYNC_EV = 10,
  DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD_EV = 11,
  DSSNET_MIP_DEREGED_EV = 12,
  
  DSSNET_NUM_OF_EV = 13
} dssnet_sm_event_type;

/*---------------------------------------------------------------------------
  For backwards compatibility. This type is depricated - dsiface_status_type
  should be used instead.
---------------------------------------------------------------------------*/
typedef dsiface_status_type dss_iface_status_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSNET_SM_POST_EVENT()

DESCRIPTION
  This function is used to post events to the dssnet_sm by other modules

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dssnet_sm_post_event
(
  dssnet_sm_event_type  event                         /* dssnet event type */
);

/*===========================================================================
FUNCTION DSSNET_SM_INIT()

DESCRIPTION
  This function is is used to initialize the dssnet state machine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dssnet_sm_init
(
  void
);

/*===========================================================================
FUNCTION IS707_GET_PPP_AUTH_INFO_FROM_NV

DESCRIPTION
  This function retrieves the 1x PPP user ID and password from NV
  and stores them in the provided PPP configuration structure.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
  Each call to 'nv_get' results in a Wait and watchdog kicking
===========================================================================*/
extern void is707_get_ppp_auth_info_from_nv
(
  ppp_dev_opts_type *ppp_config,
  nv_stat_enum_type (* nv_get)(nv_items_enum_type, nv_item_type *)
);

/*===========================================================================
FUNCTION DSSNET_SET_IN_QNC_CALL()

DESCRIPTION
  This function notifies dssnet if the mobile is in a QNC call. Called by
  ds707_pkt_mgr.

DEPENDENCIES
  None.

PARAMETERS
  boolean     in_qnc_call 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dssnet_set_in_qnc_call
(
  boolean in_qnc_call
);

/*===========================================================================
FUNCTION DSSNET_OVERRIDE_QCMIP()

DESCRIPTION
  Override dsat707_qcmip value with 2.

  RMSM calls this to force dssnet to close in the qcmip=1 case, 
  so it can bring up netmodel SimpleIP TE2 calls on its own.

  RMSM calls this to force_sip_fallback_to_fail_so_rmsm_can_do_it()

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dssnet_override_qcmip
( 
  void
);

/*===========================================================================
FUNCTION DSSNET_SET_TE_CLIENT()

DESCRIPTION
  This is a flag set by RMSM to allow dssnet to check if the
  call is a laptop call or a sockets call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dssnet_set_te_client
(
  void
);

#endif /* PS707_DSSNET_H */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ P P P _ I P C P . C

GENERAL DESCRIPTION
  Code that supports IPCP negotiation.

EXTERNALIZED FUNCTIONS
  ipcp_save_work_opt()
    This function stores the value of the IPCP option that is passed in. It
    is being saved into the work and work_pdv the want values are saved by
    the caller.
  ipcp_set_net_model_resync_fnc()
    This function sets the callback function pointer for the IPCP established
    function.
  ipcp_ppp_in_resync()
    This function returns the state of the ipcp_ppp_resync_fnc_ptr variable
  ipcp_init()
    This function will Initialize the IPCP configuration structure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 1992-1994 William Allen Simpson
  Licensed to Qualcomm -- 95 Jan 21

  Acknowledgements and correction history may be found in PPP.C

  Copyright (c) 1995-2009 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_ipcp.c_v   1.13   13 Feb 2003 14:06:22   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_ipcp.c#3 $
  $Author: nsivakum $ $DateTime: 2009/05/27 05:07:18 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Ported support for call throttle feature (DCTM).
02/22/08    pp     Klocwork fixes.
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
04/02/03    mvl    Complete modularization of the authentication code.
03/27/03    mvl    Added comments to the ipcp_constants data structure.
                   Re-ordered the functions to separate the internal from
                   external.
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added IPCP events posting. 
02/11/03    usb    Initializing slhc for each device
02/06/03    mvl    Correctly storing number of VJ slots negotiated during
                   call setup of netmodel call.  Removed check if resyncing
                   when timer expires and removed function to check for
                   resync as it is no longer used.
02/06/03    usb    fixed compilation error byremoving LOCAL from variables
                   used outside this file
11/14/02    jd     removed check if (!MIP) before setting negotiated value 
                   to iface IP addr - should always happen if negotiated
09/19/02    mvl    removed ipcp_set_ip_addr_opt().
09/18/02    aku    Removed global Ip_addr.
09/18/02    mvl    Added user_data to ppp event callbacks.  Added explicit
                   configuration for client vs. server behavior.  Removed
                   do_mip.
09/12/02    mvl    mainlined some netmodel code, generified the network model
                   resync callcack.  Removed netmodel featurization.
09/06/02    mvl    fixed assert()s.
09/02/02    mvl    PPP now registering a TX function with ps_iface when IPCP
                   is up.
08/13/02    mvl    Session handle support.
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
07/25/02    mvl    Removed netmodel dependency.  Updates for PPP renaming.
07/15/02    aku    Added inlude for pstimer.h
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Modified ipcp_opening() to do IPCP comletion processing
                   using new architecture.  Also generates event rather than
                   calling dsiface_up() call.
05/15/02   vsk/rc  In ipcp_init(), initialize IPCP will options from what is
                   passed in.
03/05/02    pjb    Finished Accept Listen Code Review Items
02/22/02    pjb    Added accept and listen
02/20/02    sjy    Mobile IP performance now featurized under
                   FEATURE_DS_MOBILE_IP_PERF
02/07/02    sjy    Added support for Mobile IP call performance metrics.
02/07/02    vsk    Initialize slhcp to NULL in ipcp_init()
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module, wrap old/new DS
                   Mgr dependencies w/FEATURE_DATA (namely PS to DS cmd i/f)
10/29/01    igt    Force the ppp state to fsmINITIAL in ipcp_init()
09/20/01    mvl    Reset the value of remote.work to 0 in starting() - to
                   ensure that we don't reject the othe guys desires.
09/13/01    mvl    Modified message to indicate that unsupported option was
                   negotiated.  Added copy of want to work in starting.
08/29/01    mvl    Added support for DNS options.
08/19/01    sjy    Referenced the correct IPCP configuration values in
                   ipcp_powerup_init() for when netmodel feature is not
                   defined.
08/12/01    sjy    Featurized reference to do_mip variable in PPP control
                   block
08/02/01    mvl    General code cleanup
07/30/01    mvl    Added set_ip_addr_opt() to add an IP addr and the bit in
                   the mask to the options data structure passed in.
                   Completely revamped the initialization so that LCP can be
                   initialized per interface and with passed in options, or
                   the defaults.
                   Support for assigning addresses on the Um interface
                   Iface IP address only assigned if iface not doing MIP.
07/29/01    na     Fixed ipcp_opening() to use generic interfaces.
07/27/01    mvl    Modifications to use PS timers, checnged the retry timer
                   to not multiply the header value by 1000.
07/24/01    mvl    compiling out parts of fsm_constants_initializer struct to
                   conserve memory, initing fsm.iface field and code cleanup
07/23/01    pjb    Change location/prefix of command names.
06/27/01    na     Removed extra event to Meta SM in ipcp_opening().
                   Now calling dsiface_up() from ipcp_opening()
06/05/01    mvl    Made some fixes to where our IP address was being zeroed
                   out when we were handing off.
06/01/01    mvl    fixed bug where mobile rejects IWF address if different
                   than the address we have stored. Also made sure Mip is
                   only kicked off if QCMIP is != 0.
05/18/01    mvl    Added hook to initiate Mobile IP when IPCP is complete and
                   general clean up.
04/16/01    js     Added inclusion for dsiface.h. Instead of setting Ip_addr
                   directly, dsiface_set_ip_addr() is invoked.
04/06/01    js     Modified ipcp_opening() so that
                   dssocki_socket_event_occurred() is called when Ip_addr is
                   assigned a value. The purpose of this change is to
                   support socket option DSS_SO_IP_ADDR_OK.
02/07/01    snn    In IPCP starting, modified code to request for an IP address
                   even if the current IP address is not Zero. This is needed
                   because, if current IP address != new ip address, we close
                   all the TCP connections.
                   Moved the call to dsstcp_new_ip_upcall() after
                   intitializing the header compression parameters in the
                   function ipcp_opening.
03/22/00   rc/na   Added a call to dsstcp_new_ip_upcall() which is called
                   when a new IP address (different from the old) is
                   received after a PPP resync.
10/28/99    mvl    Added code review changes
08/20/99    ak     commented out code to handle new IP address, since psmgr
                   punts the sockets call on a ppp re-sync anyways.
08/20/99    mvl    Added ipcp_save_work_opt() function
08/18/99    ak     Made fixes so compiles when FEATURE_DS turned off.
08/17/99    smp    Added include files msg.h & err.h.
08/11/99   ak/rc   Code to take care of getting a new IP address.
08/09/99    mvl    Added network model changes.
04/08/99    na     Changed PPP_IPCP_PROTOCOL to DSM_IPCP_PROTOCOL where the
                   protocol was used in the kind field.
03/25/99    hcg    Removed doipcp_default() function, as it was not being
                   referenced anywhere in the code.
01/27/98    ldg    Removed the DS_SLIM switch.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
03/13/97    jjn    included target.h and customer.h
02/19/96    na     UP bug fixes
11/14/96    jjw    Changes to support faster TCP open
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#if 1 // defined (FEATURE_DS) || defined (FEATURE_DATA_PS)

#include "dsbyte.h"
#include "ps_pppi.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_ipcp.h"
#include "pstimer.h"


#if 0
#include "assert.h"
#include "dsbyte.h"
#include "dsm.h"
#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_ipcp.h"
#include "msg.h"
#include "err.h"
#include "ps_iface.h"
#include "ps_ppp_fsm.h"
#include "ps_ip_addr.h"
#include "pstimer.h"
#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip_metasm.h"
#endif /* FEATURE_DS_MOBILE_IP */
#endif /* 0 */

#ifdef FEATURE_DATA
#include "dstask.h"  /* DS3G commands */
#endif /* FEATURE_DATA */

#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#include "ps_ppp_events.h"

/*===========================================================================

           GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
ipcp_value_type ipcp_local_want_cb[PPP_MAX_DEV];
ipcp_value_type ipcp_local_work_cb[PPP_MAX_DEV];
ipcp_value_type ipcp_remote_want_cb[PPP_MAX_DEV];
ipcp_value_type ipcp_remote_work_cb[PPP_MAX_DEV];


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
LOCAL net_model_resync_fptr_type ipcp_net_model_resync_fnc[PPP_MAX_DEV];

/*---------------------------------------------------------------------------
  These defaults are defined in the PPP RFCs, and must not be changed
---------------------------------------------------------------------------*/
ipcp_value_type ipcp_default =
{
  0,      /* no source address */
  0,      /* no primary dns server address */
  0,      /* no seconday dns server address */
  0,      /* no compression protocol */
  0,      /* no slots */
  0       /* no slot compression */
};

negotiate_t ipcp_accept = IPCP_N_1ADDRESS    | IPCP_N_COMPRESS     |
                          IPCP_N_PRIMARY_DNS | IPCP_N_SECONDARY_DNS;

/*---------------------------------------------------------------------------
  these lengths are the minimum required for the option
---------------------------------------------------------------------------*/
static uint8 option_length[] =
{
  IPCP_COMPRESS_SIZ,        /*   2 - compression */
  IPCP_1ADDRESS_SIZ,        /*   3 - address     */
  IPCP_PRIMARY_DNS_SIZ,     /* 129 - primary dns server address */
  IPCP_SECONDARY_DNS_SIZ    /* 131 - secondary dns server address */
};

/*---------------------------------------------------------------------------
  these are the PPP options numbers based on the option mask. (N's above)
----------------------------------------------------------------------------*/
static uint8 option_number[] =
{
  IPCP_COMPRESS,            /*   2 - compression */
  IPCP_1ADDRESS,            /*   3 - address     */
  IPCP_PRIMARY_DNS,         /* 129 - primary dns server address */
  IPCP_SECONDARY_DNS        /* 131 - secondary dns server address */
};

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--*/
static void ipcp_option
(
  ppp_fsm_type      *fsm_ptr,
  void              *vp,
  struct dsm_item_s **bpp,
  uint8             o_type
);

static int ipcp_check
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp,
  struct dsm_item_s **bpp,
  int               request
);

static int ipcp_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp
);

static void ipcp_starting
(
  ppp_fsm_type *fsm_ptr
);

static void ipcp_stopping
(
  ppp_fsm_type *fsm_ptr
);

static void ipcp_closing
(
  ppp_fsm_type *fsm_ptr
);

static void ipcp_opening
(
  ppp_fsm_type *fsm_ptr
);

static void ipcp_free
(
  ppp_fsm_type *fsm_ptr
);

static void ipcpi_timer_cb
(
  void *arg_ptr
);


/*---------------------------------------------------------------------------
  IPCP constants - this MUST follow the forward declarations because many of
  these functions are used in this data structure.
---------------------------------------------------------------------------*/
static struct fsm_constant_s ipcp_constants =
{
#ifndef T_ARM
  "IPCP",                   /* Name of protocol                     */
  fsmCodes,                 /* names of codes                       */
#endif /* T_ARM */
  PPP_IPCP_PROTOCOL,        /* Protocol number                      */
  DSM_PS_IPCP_PROTOCOL,     /* The DSM kind field for this protocol */
  0x00FE,                   /* codes 1-11 recognized                */

  IPCP,                     /* Finite State Machine index           */
  IPCP_REQ_TRY,             /* # tries for configure                */
  IPCP_NAK_TRY,             /* # tries for nak substitutes          */
  IPCP_TERM_TRY,            /* # tries for terminate                */
  IPCP_TIMEOUT,             /* Time for timeouts (milliseconds)     */

  IPCP_OPTION_LIMIT,        /* maximum option number                */
  IPCP_N_OPTION_LIMIT,      /* maximum mask number                  */
  option_number,            /* mask table                           */

  ipcp_free,                /* to free structure                    */
  fsm_down,                 /* layer layer down event               */

  ipcp_starting,            /* this layer start: enter starting     */
  ipcp_opening,             /* this layer opening: enter opening    */
  ipcp_closing,             /* this layer closing: enter closing    */
  ipcp_stopping,            /* this layers stopping: leave opened   */

  ipcp_option,              /* add option to message                */
  ipcp_check,               /* validate option and store if good    */
  ipcp_reject               /* process rx'ed config reject          */
};


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION IPCP_POWERUP_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  FSM for LCP on the interface specified, this includes getting a timer.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ipcp_powerup_init
(
  ppp_dev_enum_type device /* the interface to initialize */
)
{
  ppp_fsm_type *fsm_ptr;
  static ipcp_pdv_type ipcp_pdv[PPP_MAX_DEV];
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);

  /*-------------------------------------------------------------------------
    set the network model resync function to NULL
  -------------------------------------------------------------------------*/
  ipcp_net_model_resync_fnc[device] = NULL;

  /*-------------------------------------------------------------------------
    reset the FSM for this interface, and LCP
  -------------------------------------------------------------------------*/
  memset(fsm_ptr, 0, sizeof(ppp_fsm_type));

  /*-------------------------------------------------------------------------
    Initialize the protocol dependent constants and variable pointer.
  -------------------------------------------------------------------------*/
  fsm_ptr->pdc = &ipcp_constants;
  fsm_ptr->pdv = &ipcp_pdv[device];
  fsm_ptr->local.want_pdv = &(ipcp_local_want_cb[device]);
  fsm_ptr->local.work_pdv = &(ipcp_local_work_cb[device]);
  fsm_ptr->remote.want_pdv = &(ipcp_remote_want_cb[device]);
  fsm_ptr->remote.work_pdv = &(ipcp_remote_work_cb[device]);

  /*-------------------------------------------------------------------------
    Initialize the device field to the device passed in
  -------------------------------------------------------------------------*/
  fsm_ptr->device = device;

  /*-------------------------------------------------------------------------
    Get the timer for this instance of IPCP - return 0 if OK or -1 if failed
  -------------------------------------------------------------------------*/
  fsm_ptr->timer = ps_timer_alloc(ipcpi_timer_cb, &(ppp_cb_array[device]));

  if( fsm_ptr->timer == PS_TIMER_FAILURE)
  {
    return -1;
  }
  else
  {
    return 0;
  }

} /* ipcp_powerup_init() */



/*===========================================================================
FUNCTION IPCP_INIT()

DESCRIPTION
  This function will Initialize the IPCP configuration structure on the
  given interface(Rm or Um).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_init
(
  ppp_dev_enum_type  device,   /* device to initalize         */
  ppp_dev_opts_type   *opts_ptr /* options to initialize with */
)
{
  ppp_fsm_type *fsm_ptr;
  ipcp_value_type *lwant_ptr;
  ipcp_value_type *lwork_ptr;
  ipcp_value_type *rwant_ptr;
  ipcp_value_type *rwork_ptr;
  ipcp_pdv_type *ipcp_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV ||
     opts_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  fsm_ptr = &(ppp_cb_array[device].fsm[IPcp]);
  lwant_ptr = (ipcp_value_type*)fsm_ptr->local.want_pdv;
  lwork_ptr = (ipcp_value_type*)fsm_ptr->local.work_pdv;
  rwant_ptr = (ipcp_value_type*)fsm_ptr->remote.want_pdv;
  rwork_ptr = (ipcp_value_type*)fsm_ptr->remote.work_pdv;
  ipcp_ptr    = (ipcp_pdv_type *)fsm_ptr->pdv;

  /*-------------------------------------------------------------------------
    Zeroing out the variables for this interface.
  -------------------------------------------------------------------------*/
  memset(lwant_ptr, 0, sizeof(ipcp_value_type));
  memset(rwant_ptr, 0, sizeof(ipcp_value_type));
  memset(lwork_ptr, 0, sizeof(ipcp_value_type));
  memset(rwork_ptr, 0, sizeof(ipcp_value_type));

  fsm_ptr->state = fsmINITIAL;
  ipcp_ptr->slhcp = NULL;

  /*-------------------------------------------------------------------------
    Initialize with the options that have been passed in.
  -------------------------------------------------------------------------*/
  MSG_MED("ipcp_init(device %d)", device, 0, 0);

  /*-------------------------------------------------------------------------
    Copy the variables and mask over - not the local needs to be copied to
    the remote and visa versa
  -------------------------------------------------------------------------*/
  memcpy(rwant_ptr,
         &(opts_ptr->ipcp_info.want_vals[FSM_REMOTE]),
         sizeof(ipcp_value_type));
  memcpy(lwant_ptr,
         &(opts_ptr->ipcp_info.want_vals[FSM_LOCAL]),
         sizeof(ipcp_value_type));
  memcpy(rwork_ptr,
         &(opts_ptr->ipcp_info.work_vals[FSM_REMOTE]),
         sizeof(ipcp_value_type));
  memcpy(lwork_ptr,
         &(opts_ptr->ipcp_info.work_vals[FSM_LOCAL]),
         sizeof(ipcp_value_type));
  fsm_ptr->local.want = opts_ptr->ipcp_info.want_mask[FSM_LOCAL];
  fsm_ptr->remote.want = opts_ptr->ipcp_info.want_mask[FSM_REMOTE];
  fsm_ptr->local.will = opts_ptr->ipcp_info.will_mask[FSM_LOCAL];
  fsm_ptr->remote.will = opts_ptr->ipcp_info.will_mask[FSM_REMOTE];

  /*-------------------------------------------------------------------------
    Make sure that work is setup so that we negotiate for it it
  -------------------------------------------------------------------------*/
  fsm_ptr->local.work = fsm_ptr->local.want;
  fsm_ptr->remote.work = fsm_ptr->remote.want;

  fsm_init(fsm_ptr);

} /* ipcp_init() */



/*===========================================================================
FUNCTION IPCP_START()

DESCRIPTION
  This function will Check for PPP Network-Layer Protocol Phase

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_start
(
  ppp_type *ppp_cb_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    we are bringing up IPCP
  -------------------------------------------------------------------------*/
  ppp_cb_ptr->phase = pppNETWORK;

  /*-------------------------------------------------------------------------
    If IPCP is disabled and the PPP UP callback is registered call it,
    otherwise start IPCP - get the interface from one of the FSMs.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_DISABLED)
  {
    TASKLOCK();
    if(ppp_cb_ptr->event_info[PPP_UP_EV].cback != NULL)
    {
      ppp_cb_ptr->event_info[PPP_UP_EV].cback(
        ppp_cb_ptr->fsm[LCP].device,
        PPP_PROTOCOL_IPCP,
        PPP_UP_EV,
        ppp_cb_ptr->event_info[PPP_UP_EV].user_data,
        ppp_cb_ptr->session_handle,
        PPP_FAIL_REASON_NONE);
    }
    TASKFREE();

    /*-----------------------------------------------------------------------
      since PPP is done, then cancel the setup timer.
    -----------------------------------------------------------------------*/
    ps_timer_cancel(ppp_cb_ptr->setup_timer);
  }
  else
  {
    fsm_up(&(ppp_cb_ptr->fsm[IPCP]));
  }
} /* ipcp_start() */



/*===========================================================================
FUNCTION IPCP_SET_NET_MODEL_RESYNC_FNC()

DESCRIPTION
  This function sets the callback function pointer for the IPCP established
  function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_set_net_model_resync_fnc
(
  net_model_resync_fptr_type fnc_ptr,
  ppp_dev_enum_type          device
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }

  ipcp_net_model_resync_fnc[device] = fnc_ptr;

} /* ipcp_set_net_model_resync_fnc() */



/*===========================================================================
FUNCTION IPCP_SAVE_WORK_OPT()

DESCRIPTION
   This function stores the value of the IPCP option that is passed in. It is
   being saved into the work and work_pdv the want values are saved by the
   caller.

DEPENDENCIES
  ipcp_init() should have been called before this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_save_work_opt
(
  ppp_fsm_side_type *opts,       /* pointer to where the values are stored */
  byte opt_num,                                       /* the option number */
  byte *opt_val                                 /* the value of the option */
)
{
  ipcp_value_type *work_opts = (ipcp_value_type*)(opts->work_pdv);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Determine option value
  -------------------------------------------------------------------------*/
  switch(opt_num)
  {
    /*-----------------------------------------------------------------------
      VJ compression
    -----------------------------------------------------------------------*/
  case IPCP_COMPRESS:
    work_opts->compression = ntohs((*((uint16*)opt_val)));
    work_opts->slots = *(opt_val+2) + 1;
    work_opts->slot_compress = *(opt_val+3);
    opts->work |= IPCP_N_COMPRESS;
    break;

    /*-----------------------------------------------------------------------
      IP address
    -----------------------------------------------------------------------*/
  case IPCP_1ADDRESS:
    work_opts->address = ntohl((*((ip4a*)opt_val)));
    opts->work |= IPCP_N_1ADDRESS;
    break;

    /*-----------------------------------------------------------------------
      primary dns server address
    -----------------------------------------------------------------------*/
  case IPCP_PRIMARY_DNS:
    work_opts->primary_dns = ntohl((*((ip4a*)opt_val)));
    opts->work |= IPCP_N_PRIMARY_DNS;
    break;

    /*-----------------------------------------------------------------------
      secondary dns server address
    -----------------------------------------------------------------------*/
  case IPCP_SECONDARY_DNS:
    work_opts->secondary_dns = ntohl((*((ip4a*)opt_val)));
    opts->work |= IPCP_N_SECONDARY_DNS;
    break;

    /*-----------------------------------------------------------------------
      default
    -----------------------------------------------------------------------*/
  default:
    MSG_MED("Got unknown option %d - ignoring", opt_num, 0, 0);

  } /* switch(option) */
} /* ipcp_save_work_opt() */



/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPCP_OPTION()

DESCRIPTION
  This function performs IPCP option processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_option
(
  ppp_fsm_type   *fsm_ptr,
  void           *vp,
  dsm_item_type **bpp,
  uint8           opt_num
)
{
  ipcp_value_type *value_p = (ipcp_value_type *)vp;
  struct dsm_item_s *bp = *bpp;
  register uint8 *cp;
  int n_type   = fsm_n_option(fsm_ptr->pdc,opt_num);
  int opt_len = option_length[n_type];
  register int used = opt_len - OPTION_HDR_LEN;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    this only tests for the minimum option length
  -------------------------------------------------------------------------*/
  if ( bp->used + opt_len > bp->size )
  {
    return;
  }
  cp    = bp->data_ptr + bp->used;
  *cp++ = opt_num;
  *cp++ = opt_len;

  /*-------------------------------------------------------------------------
    Determine what the option type is, and process appropriately
  -------------------------------------------------------------------------*/
  switch( opt_num )
  {
    /*-----------------------------------------------------------------------
      IP address
    -----------------------------------------------------------------------*/
  case IPCP_1ADDRESS:
    cp    = put32(cp, value_p->address);
    used -= 4;
    break;

    /*-----------------------------------------------------------------------
      primary dns server address
    -----------------------------------------------------------------------*/
  case IPCP_PRIMARY_DNS:
    cp    = put32(cp, value_p->primary_dns);
    used -= 4;
    break;

    /*-----------------------------------------------------------------------
      secondary dns server address
    -----------------------------------------------------------------------*/
  case IPCP_SECONDARY_DNS:
    cp    = put32(cp, value_p->secondary_dns);
    used -= 4;
    break;

    /*-----------------------------------------------------------------------
      VJ compression
    -----------------------------------------------------------------------*/
  case IPCP_COMPRESS:
    cp    = put16(cp, value_p->compression);
    used -= 2;

    if ( value_p->compression == PPP_VJCH_PROTOCOL )
    {
      *cp++ = value_p->slots - 1;
      *cp++ = value_p->slot_compress;
      used -= 2;
    }
    break;

    /*-----------------------------------------------------------------------
      default
    -----------------------------------------------------------------------*/
  default:
    MSG_HIGH( "Got IPCP option %d", opt_num, 0, 0);
    break;
  } /* switch(option) */;

  opt_len                    -= used;
  bp->data_ptr[bp->used + 1]  = opt_len;  /* length may be modified */
  bp->used                   += opt_len;

} /* ipcp_option() */



/*===========================================================================
FUNCTION IPCP_CHECK()

DESCRIPTION
  This function will Check the options, updating the working values.

DEPENDENCIES
  None

RETURN VALUE
  Returns -1 if ran out of data, ACK/NAK/REJ as appropriate.

SIDE EFFECTS
  None
===========================================================================*/
static int ipcp_check
(
  ppp_fsm_type       *fsm_ptr,
  struct option_hdr  *ohp,
  struct dsm_item_s **bpp,
  int                 request
)
{

  ppp_fsm_side_type * side_p;
  ipcp_value_type * swant_ptr;
  ipcp_value_type * swork_ptr;
  int used = 0;
  int option_result = CONFIG_ACK; /* Assume good values */
  int test;
  int n_type;
  ppp_type * ppp_cb_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(bpp == NULL || *bpp == NULL )
  {
    MSG_ERROR("Invalid DSM", 0, 0, 0);
    return -1;
  }

  if( fsm_ptr == NULL || 
      ohp == NULL || 
      ohp->length < OPTION_HDR_LEN ||
      fsm_ptr->device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid args", 0, 0, 0);
    dsm_free_packet(bpp);
    return -1;
  }

  used = ohp->length - OPTION_HDR_LEN; 
  side_p = (request ? &(fsm_ptr->remote) : &(fsm_ptr->local));
  swant_ptr = (ipcp_value_type *)(side_p->want_pdv);
  swork_ptr = (ipcp_value_type *)(side_p->work_pdv);
  ppp_cb_ptr = ppp_cb_array + fsm_ptr->device;

  n_type = fsm_n_option(fsm_ptr->pdc,ohp->type);

  /*-------------------------------------------------------------------------
    see if we allow this option
  -------------------------------------------------------------------------*/
  if(ohp->type > IPCP_OPTION_LIMIT          ||
     ohp->length < option_length[n_type]    ||
     !((side_p->will | side_p->want) & (1 << n_type)))
  {
    /*-------------------------------------------------------------------------
      toss any bytes in option
    -------------------------------------------------------------------------*/
    while( used-- > 0 )
    {
      if(dsm_pull8(bpp) == -1 )
      {
        return -1;
      }
    } /* while(bytes to toss) */

    return CONFIG_REJ;

  } /* if(invalid option) */

  switch( ohp->type)
  {
    /*-----------------------------------------------------------------------
      IP address option
    -----------------------------------------------------------------------*/
  case IPCP_1ADDRESS:
    swork_ptr->address = dsm_pull32(bpp);
    used -= 4;
    if(!request )
    {
      /*---------------------------------------------------------------------
        Accept whatever the peer wants, unless specifically configured to
        refuse.
      ---------------------------------------------------------------------*/
      if(!(side_p->will & IPCP_N_1ADDRESS))
      {
        /*-------------------------------------------------------------------
          no changes allowed
        -------------------------------------------------------------------*/
        swork_ptr->address = swant_ptr->address;
      }
      else if(swork_ptr->address == 0L)
      {
        /*-------------------------------------------------------------------
          set current address
        -------------------------------------------------------------------*/
        if((swork_ptr->address = swant_ptr->address) == 0L)
        {
          swork_ptr->address = PS_IFACE_IP_V4_ADDR(ppp_cb_ptr->rx_iface_ptr);
        }
      }
      break;
    } /* if(!request) */

    /*-----------------------------------------------------------------------
      REQUEST:
      If the peer asks for a 0 address and we don't have one for them then we
      have to config reject because we don't have one to assign.
    -----------------------------------------------------------------------*/
    if(swork_ptr->address == swant_ptr->address &&
       swork_ptr->address == 0)
    {
      option_result = CONFIG_REJ;
    }

    if(ppp_cb_array[fsm_ptr->device].serves_addrs == TRUE &&
       swant_ptr->address != swork_ptr->address           &&
       swant_ptr->address != 0)
    {
      swork_ptr->address = swant_ptr->address;
      option_result = CONFIG_NAK;
    }
    break;

    /*-----------------------------------------------------------------------
      primary DNS address
    -----------------------------------------------------------------------*/
  case IPCP_PRIMARY_DNS:
    swork_ptr->primary_dns = dsm_pull32(bpp);
    used -= 4;
    if ( !request )
    {
      /*---------------------------------------------------------------------
        Accept whatever the peer wants, unless specifically configured to
        refuse.
      ---------------------------------------------------------------------*/
      if ( !(side_p->will & IPCP_N_PRIMARY_DNS) )
      {
        /*-------------------------------------------------------------------
          no changes allowed
        -------------------------------------------------------------------*/
        swork_ptr->primary_dns = swant_ptr->primary_dns;
      }
      else if ( swork_ptr->primary_dns == 0L )
      {
        /*-------------------------------------------------------------------
          set current address
        -------------------------------------------------------------------*/
        if ( (swork_ptr->primary_dns = swant_ptr->primary_dns) == 0L )
        {
          swork_ptr->primary_dns = 0;
        }
      }
      break;
    } /* if(!request) */

    /*-----------------------------------------------------------------------
      Request
    -----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------
      If the peer asks for a 0 address and we don't have one for them then we
      have to config reject because we don't have one to assign.
    -----------------------------------------------------------------------*/
    if (swork_ptr->primary_dns == swant_ptr->primary_dns &&
    swork_ptr->primary_dns == 0)
    {
      option_result = CONFIG_REJ;
    }

    /*-----------------------------------------------------------------------
      If on he RM interface, require that the peer use configured address
    -----------------------------------------------------------------------*/
    if (fsm_ptr->device == PPP_RM_DEV &&
    swork_ptr->primary_dns != swant_ptr->primary_dns &&
    swant_ptr->primary_dns != 0L )
    {
      swork_ptr->primary_dns = swant_ptr->primary_dns;
      option_result = CONFIG_NAK;
    }
    break;

    /*-----------------------------------------------------------------------
      secondary DNS address
    -----------------------------------------------------------------------*/
  case IPCP_SECONDARY_DNS:
    swork_ptr->secondary_dns = dsm_pull32(bpp);
    used -= 4;
    if ( !request )
    {
      /*---------------------------------------------------------------------
        Accept whatever the peer wants, unless specifically configured to
        refuse.
      ---------------------------------------------------------------------*/
      if ( !(side_p->will & IPCP_N_SECONDARY_DNS) )
      {
        /*-------------------------------------------------------------------
          no changes allowed
        -------------------------------------------------------------------*/
        swork_ptr->secondary_dns = swant_ptr->secondary_dns;
      }
      else if ( swork_ptr->secondary_dns == 0L )
      {
        /*-------------------------------------------------------------------
          set current address
        -------------------------------------------------------------------*/
        if ( (swork_ptr->secondary_dns = swant_ptr->secondary_dns) == 0L )
        {
          swork_ptr->secondary_dns = 0;
        }
      }
      break;
    } /* if(!request) */


    /*-----------------------------------------------------------------------
      Request
    -----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------
      If the peer asks for a 0 address and we don't have one for them then we
      have to config reject because we don't have one to assign.
    -----------------------------------------------------------------------*/
    if (swork_ptr->secondary_dns == swant_ptr->secondary_dns &&
    swork_ptr->secondary_dns == 0)
    {
      option_result = CONFIG_REJ;
    }

    /*-----------------------------------------------------------------------
      If on he RM interface, require that the peer use configured address
    -----------------------------------------------------------------------*/
    if (fsm_ptr->device == PPP_RM_DEV &&
    swork_ptr->secondary_dns != swant_ptr->secondary_dns &&
    swant_ptr->secondary_dns != 0L )
    {
      swork_ptr->secondary_dns = swant_ptr->secondary_dns;
      option_result = CONFIG_NAK;
    }
    break;

    /*-----------------------------------------------------------------------
      VJ compression
    -----------------------------------------------------------------------*/
  case IPCP_COMPRESS:
    swork_ptr->compression = dsm_pull16(bpp);
    used -= 2;

    /*-----------------------------------------------------------------------
      Check if requested type is acceptable
    -----------------------------------------------------------------------*/
    switch ( swork_ptr->compression )
    {
    case PPP_VJCH_PROTOCOL:
      if((test = dsm_pull8(bpp)) == -1 )
      {
        return -1;
      }

      if((swork_ptr->slots = test + 1) < IPCP_SLOT_LO)
      {
        swork_ptr->slots = IPCP_SLOT_LO;
        option_result = CONFIG_NAK;
      }
      else if (swork_ptr->slots > IPCP_SLOT_HI)
      {
        swork_ptr->slots = IPCP_SLOT_HI;
        option_result = CONFIG_NAK;
      }

      if((test = dsm_pull8(bpp)) == -1 )
      {
        return -1;
      }

      if((swork_ptr->slot_compress = test) > 1 )
      {
        swork_ptr->slot_compress = 1;
        option_result = CONFIG_NAK;
      }
      used -= 2;
      break;

    default:
      if(side_p->want & IPCP_N_COMPRESS )
      {
        swork_ptr->compression = swant_ptr->compression;
        swork_ptr->slots = swant_ptr->slots;
        swork_ptr->slot_compress = swant_ptr->slot_compress;
      }
      else
      {
        swork_ptr->compression = PPP_VJCH_PROTOCOL;
        swork_ptr->slots = IPCP_SLOT_DEFAULT;
        swork_ptr->slot_compress = IPCP_SLOT_COMPRESS;
      }
      option_result = CONFIG_NAK;
      break;
    } /* switch(compression type) */
    break;

    /*-----------------------------------------------------------------------
      default
    -----------------------------------------------------------------------*/
  default:
    option_result = CONFIG_REJ;
    break;
  } /* switch(option) */

  if(used < 0 )
  {
    return -1;
  }

  if(used > 0 )
  {
    /*-----------------------------------------------------------------------
      toss extra bytes in option
    -----------------------------------------------------------------------*/
    while( used-- > 0 )
    {
      if(dsm_pull8(bpp) == -1 )
      {
        return -1;
      }
    }
  }
  return (option_result);

} /* ipcp_check() */



/*===========================================================================
FUNCTION IPCP_REJECT()

DESCRIPTION
  This function will process a configuration reject sent by the other side.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int ipcp_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return 0;

} /* ipcp_reject() */



/*===========================================================================
FUNCTION IPCP_STARTING()

DESCRIPTION
  This function will Prepare to begin configuration exchange

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_starting
(
  ppp_fsm_type *fsm_ptr
)
{
  ipcp_value_type *lwant_ptr = (ipcp_value_type*)(fsm_ptr->local.want_pdv);
  ipcp_value_type *lwork_ptr = (ipcp_value_type*)(fsm_ptr->local.work_pdv);
  ppp_event_payload_type  ppp_event_payload;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Fill in the fileds of ppp_event_payload and generate the event.
  -------------------------------------------------------------------------*/
  ppp_event_payload.ppp_event_device = fsm_ptr->device;
  ppp_event_payload.ppp_event_protocol = PPP_EV_IPCP;
  ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
  ppp_event_payload.ppp_event_state = PS_PPP_EVENT_STARTING;
  event_report_payload(EVENT_PPP, 
                       sizeof(ppp_event_payload),
                       &ppp_event_payload);


  /*-------------------------------------------------------------------------
    If we don't already want a particular address, and we currently have
    another address, retry previous assignment for continuity
  -------------------------------------------------------------------------*/
  if(fsm_ptr->local.work & IPCP_N_1ADDRESS &&
     lwant_ptr->address == 0             &&
     lwork_ptr->address != 0)
  {
    lwant_ptr->address = lwork_ptr->address;
  }

  if(fsm_ptr->local.work & IPCP_N_PRIMARY_DNS &&
     lwant_ptr->primary_dns == 0 &&
     lwork_ptr->primary_dns != 0)
  {
    lwant_ptr->primary_dns = lwork_ptr->primary_dns;
  }

  if(fsm_ptr->local.work & IPCP_N_SECONDARY_DNS &&
     lwant_ptr->secondary_dns == 0 &&
     lwork_ptr->secondary_dns != 0)
  {
    lwant_ptr->secondary_dns = lwork_ptr->secondary_dns;
  }

  fsm_ptr->local.work = fsm_ptr->local.want;

  fsm_ptr->remote.work = FALSE;

} /* ipcp_starting() */



/*===========================================================================
FUNCTION IPCP_OPENING()

DESCRIPTION
  This function completes IPCP configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_opening
(
  ppp_fsm_type *fsm_ptr
)
{
  ip_addr_type     ip_addr;
  ipcp_pdv_type   *ipcp_ptr;
  ipcp_value_type *lworkp;
  ipcp_value_type *rworkp;
  ppp_type        *ppp_cb_ptr;
  int              rslots = 0;
  int              tslots = 0;
  ppp_dev_enum_type device;
  ppp_event_payload_type  ppp_event_payload;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( fsm_ptr == NULL || 
      fsm_ptr->device >= PPP_MAX_DEV ||
      fsm_ptr->pdv == NULL ||
      fsm_ptr->local.work_pdv == NULL ||
      fsm_ptr->remote.work_pdv == NULL )
  {
    MSG_ERROR("Invalid fsm_ptr", 0, 0, 0);
    return;
  }

  device = fsm_ptr->device;
  ppp_cb_ptr = ppp_cb_array + device;

  ipcp_ptr = (ipcp_pdv_type *)(fsm_ptr->pdv);
  lworkp = (ipcp_value_type *)(fsm_ptr->local.work_pdv);
  rworkp = (ipcp_value_type *)(fsm_ptr->remote.work_pdv);

  /*-------------------------------------------------------------------------
    reset to total setup timer
  -------------------------------------------------------------------------*/
  ps_timer_cancel(ppp_cb_ptr->setup_timer);

  /*-------------------------------------------------------------------------
    Store # of compression slots if they were negotiated and initialize the
    SLHC
  -------------------------------------------------------------------------*/
  if(fsm_ptr->local.work & IPCP_N_COMPRESS)
  {
    rslots = lworkp->slots;
  }
  if(fsm_ptr->remote.work & IPCP_N_COMPRESS)
  {
    tslots = rworkp->slots;
  }
  if(rslots != 0 || tslots != 0)
  {
    ipcp_ptr->slhcp = slhc_init( device, rslots, tslots );
  }

  TASKLOCK();
  /*-------------------------------------------------------------------------
    Store IP address from negotiated option
  -------------------------------------------------------------------------*/
  if(fsm_ptr->local.work & IPCP_N_1ADDRESS)
  {
    MSG_LOW("IPCP: Got IPv4 Address = %x",lworkp->address,0,0);

    /*-------------------------------------------------------------------------
      Initialize the ip_addr vairable
    -------------------------------------------------------------------------*/
    ip_addr.type    = IPV4_ADDR;
    ip_addr.addr.v4 = lworkp->address;

    /*-----------------------------------------------------------------------
      set the local IP address 
    -----------------------------------------------------------------------*/
    ps_iface_set_addr(ppp_cb_ptr->rx_iface_ptr, &ip_addr);
  }


  /*-------------------------------------------------------------------------
    Store the PPP peer information in the associated iface CB and register
    the PPP PS Iface tx function with the interface.
  -------------------------------------------------------------------------*/
  ps_iface_set_peer_data(ppp_cb_ptr->rx_iface_ptr,
                         pppi_ps_iface_get_peer_addr_cback,
                         ppp_cb_ptr);
  ps_iface_set_tx_function(ppp_cb_ptr->rx_iface_ptr,
                           pppi_iface_tx_function,
                           (void*)device);  

  /*-------------------------------------------------------------------------
    If DNS was negotiated then store this information in the interface
  -------------------------------------------------------------------------*/
  if(lworkp->primary_dns != 0)
  {
    ppp_cb_ptr->rx_iface_ptr->v4_net_info.primary_dns = lworkp->primary_dns;
  }
  if(lworkp->secondary_dns != 0)
  {
    ppp_cb_ptr->rx_iface_ptr->v4_net_info.secondary_dns =
      lworkp->secondary_dns;
  }
  TASKFREE();

#ifdef FEATURE_DS_MOBILE_IP_PERF
  /*-------------------------------------------------------------------------
    Notify M.IP call performance that Um/Rm PPP is established.
  -------------------------------------------------------------------------*/
  if (device == PPP_UM_SN_DEV)
  {
    mip_perf_delay(&mip_perf_info[PERF_UM_PPP_DELAY],
                   PERF_UM_PPP_DELAY,
                   PERF_END_TS);
  }
  else if (device == PPP_RM_DEV)
  {
    mip_perf_delay(&mip_perf_info[PERF_RM_PPP_DELAY],
                   PERF_RM_PPP_DELAY,
                   PERF_END_TS);
  }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
  
  MSG_HIGH("IPCP established on dev %d", device, 0, 0);

  /*-------------------------------------------------------------------------
    Fill in the fileds of ppp_event_payload and generate the event.
  -------------------------------------------------------------------------*/
  ppp_event_payload.ppp_event_device = fsm_ptr->device;
  ppp_event_payload.ppp_event_protocol = PPP_EV_IPCP;
  ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
  ppp_event_payload.ppp_event_state = PS_PPP_EVENT_OPEN;
  event_report_payload(EVENT_PPP, 
                       sizeof(ppp_event_payload),
                       &ppp_event_payload);


  TASKLOCK();
  /*-----------------------------------------------------------------------
    If a callback was registered for resyncs, then execute that code,
    otherwise set the IP addresses
  -----------------------------------------------------------------------*/
  if( ipcp_net_model_resync_fnc[device] != NULL)
  {
    ipcp_net_model_resync_fnc[device](device);
  }

  /*-------------------------------------------------------------------------
    If the ppp up callback is set, call it.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->event_info[PPP_UP_EV].cback != NULL)
  {
    ppp_cb_ptr->event_info[PPP_UP_EV].cback(
      device,
      PPP_PROTOCOL_IPCP,
      PPP_UP_EV,
      ppp_cb_ptr->event_info[PPP_UP_EV].user_data,
      ppp_cb_ptr->session_handle,
      PPP_FAIL_REASON_NONE);
  }
  TASKFREE();

} /* ipcp_opening() */


/*===========================================================================
FUNCTION IPCP_CLOSING()

DESCRIPTION
  This function will initiate the closing of IPCP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_closing
(
  ppp_fsm_type *fsm_ptr
)
{
  ipcp_pdv_type *ipcp_ptr = (ipcp_pdv_type *)fsm_ptr->pdv;

  ipcp_ptr->slhcp = NULL;

} /* ipcp_closing() */


/*===========================================================================
FUNCTION IPCP_STOPPING()

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_stopping
(
  ppp_fsm_type *fsm_ptr
)
{
  ppp_event_payload_type  ppp_event_payload;
  ppp_type               *ppp_cb_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( fsm_ptr == NULL || fsm_ptr->device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid fsm_ptr in ipcp_stopping", 0, 0, 0);
    return;
  }
  
  ppp_cb_ptr = &ppp_cb_array[fsm_ptr->device];

  TASKLOCK();
  if(ppp_cb_ptr->event_info[PPP_DOWN_EV].cback != NULL)
  {
    ppp_cb_ptr->event_info[PPP_DOWN_EV].cback(
      fsm_ptr->device,
      PPP_PROTOCOL_IPCP,
      PPP_DOWN_EV,
      ppp_cb_ptr->event_info[PPP_DOWN_EV].user_data,
      ppp_cb_ptr->session_handle,
      ppp_cb_ptr->fsm[IPCP].fail_reason);
  }
  TASKFREE();

  /*-------------------------------------------------------------------------
    Fill in the fileds of ppp_event_payload
  -------------------------------------------------------------------------*/
  ppp_event_payload.ppp_event_device = fsm_ptr->device;
  ppp_event_payload.ppp_event_protocol = PPP_EV_IPCP;
  ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
  ppp_event_payload.ppp_event_state = PS_PPP_EVENT_CLOSING;
  event_report_payload(EVENT_PPP, 
                       sizeof(ppp_event_payload),
                       &ppp_event_payload);

  ps_timer_cancel(fsm_ptr->timer);

} /* ipcp_stopping() */



/*===========================================================================
FUNCTION IPCPI_TIMER_CB()

DESCRIPTION
  This function

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcpi_timer_cb
(
  void *arg_ptr
)
{
  ppp_type *ppp_cb_ptr = (ppp_type*)arg_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("dev %d IPCP Timer Expired state =%d",
          ppp_cb_ptr->fsm[IPCP].device,
          ppp_cb_ptr->fsm[IPCP].state,
          0);


  /*-------------------------------------------------------------------------
    If the PPP FSM has given up AND we are in a network model kill the PPP
    instance.
  -------------------------------------------------------------------------*/
  if(fsm_timeout( &(ppp_cb_ptr->fsm[IPCP])) == FALSE)
  {
    MSG_MED("IPCP timed out during resync",0,0,0);
    ppp_cb_ptr->fsm[IPCP].fail_reason = PPP_FAIL_REASON_TIMEOUT;
    ppp_abort(ppp_cb_ptr->fsm[IPCP].device);
  }

} /* ipcpi_timer_cb() */



/*===========================================================================
FUNCTION IPCP_FREE()

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_free
(
  ppp_fsm_type *fsm_ptr
)
{
  ipcp_pdv_type *ipcp_ptr = (ipcp_pdv_type *)fsm_ptr->pdv;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ipcp_ptr->slhcp = NULL;

} /* ipcp_free() */

#endif /* FEATURE_DS */

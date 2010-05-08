
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ M I P _ T A S K . C

GENERAL DESCRIPTION

  This is the file that defines all of the MIP functions that are tied to
  executing MIP in some other task context.

EXTERNALIZED FUNCTIONS

  mip_task_powerup_init()

    This function must be called from PS initialization function to 
    register the mip command handlers with multimode PS.
    
  Copyright (c)2002-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MM_DATA/vcs/ps_mip_task.c_v   1.5   30 Dec 2002 17:17:40   jeffd  $
$Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_mip_task.c#4 $ $DateTime: 2009/04/03 01:11:27 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/18/09    sn     Registered callback function with SEC to be called when 
                   random data is available.
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
09/19/06    as     Added mip_task_rand_data_ready() API for secutil to
                   indicate when random data is available.
04/11/03    ak     Updated for new 707 RMSM interfaces.
12/30/02    jd     Enable mip signal handling at runtime
12/29/02    jd     Fixed includes for multimode DMU builds
                   Register signal handlers for MIP signals
11/25/02    usb    Moved code which gets drs_opt_nv_item from NV to
                   ds707_drs. 
11/14/02    jd     Get DRS alg. preference at powerup
09/11/02    jay    Created Module
===========================================================================*/



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          INCLUDE FILES FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DS_MOBILE_IP

/*---------------------------------------------------------------------------
  The general includes
---------------------------------------------------------------------------*/
#include "assert.h"
#include "msg.h"
#include "ps_svc.h"
#include "ps_mip_task.h"
#include "dsmip_io.h"
#include "dsmip_metasm.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds707_rmsm.h"
#endif
#include "ps_ppp_fsm.h"  // jd - hack

#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_dmu.h"
#include "secapi.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         INTERNAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
  
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
LOCAL void mip_task_process_start
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

LOCAL void mip_task_process_stop
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            EXTERNAL DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION MIP_TASK_POWERUP_INIT()

DESCRIPTION
  This function does the powerup initialization for the PPP module.  This
  includes registering handlers with PS (the task executing it) for the PPP
  RX signals.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_task_powerup_init( void )
{
#ifdef FEATURE_DS_MOBILE_IP_DMU
  secerrno_enum_type sec_errno;
#endif /* FEATURE_DS_MOBILE_IP_DMU */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    register the signal handlers for MIP
  -------------------------------------------------------------------------*/
  ps_set_sig_handler(PS_MIP_SOCK_EVENT_SIGNAL,mipio_handle_socket_event,NULL);
  ps_enable_sig (PS_MIP_SOCK_EVENT_SIGNAL);
  ps_set_sig_handler(PS_MIP_NET_EVENT_SIGNAL,mipio_handle_network_event,NULL);
  ps_enable_sig (PS_MIP_NET_EVENT_SIGNAL);

#ifdef FEATURE_DS_MOBILE_IP_DMU
  ps_set_sig_handler(PS_MIP_DMU_RAND_SIGNAL,dmu_rand_sig_handler,NULL);
  ps_enable_sig (PS_MIP_DMU_RAND_SIGNAL);

  /*---------------------------------------------------------------------------
    Register callback function with SEC to get called when random data is 
    available.
    E_FAILURE indicates SECRND task is ready and random data is available.
    E_NO_MEMORY indicates cb function can't be registered as MAX FP is reached.
    E_INVALID_ARG indicates callback function is NULL.
  ---------------------------------------------------------------------------*/
  sec_errno = secapi_get_random_set_cb( mip_task_rand_data_ready, NULL );
  if(sec_errno == E_FAILURE)
  {
    /* Random data is available. Set signal to regenerate new key data. */
    mip_task_rand_data_ready(NULL);
  }
  else if(sec_errno != E_SUCCESS)
  {
    /* Callback registration failed. */
    MSG_ERROR("Could not register cb function with SEC. Error is %d.", 
              sec_errno, 0, 0);
  }
#endif /* FEATURE_DS_MOBILE_IP_DMU */

  /*-------------------------------------------------------------------------
    register the handlers for the various PPP commands that must be handled
    in the task context.
  -------------------------------------------------------------------------*/
  ps_set_cmd_handler(MIPIO_PPP_OPEN_CMD, mip_task_process_start);
  ps_set_cmd_handler(MIPIO_PPP_CLOSE_CMD, mip_task_process_stop);
  
} /* mip_task_powerup_init() */



/*===========================================================================
FUNCTION MIP_TASK_PROCESS_START()

DESCRIPTION
  This function starts the mobile ip process

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void mip_task_process_start
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  boolean mip_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != MIPIO_PPP_OPEN_CMD)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Start the MIP IO operation  
  -------------------------------------------------------------------------*/
  mip_status = mipio_ppp_open();

  /*-------------------------------------------------------------------------
    If MIP couldn't open because MIP was up in sockets mode or pppopen 
    failed, let RMSM know.  pppopen isn't called for MIP in sockets mode.
  -------------------------------------------------------------------------*/
  if (mip_status != PPP_OPEN_SUCCESS)
  {
    ds707_rmsm_post_event(DS707_RMSM_UM_MIP_DOWN_EV);
  }
} /* mip_task_process_start() */


/*===========================================================================
FUNCTION MIP_TASK_PROCESS_STOP()

DESCRIPTION
  This function stops the mobile ip process

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void mip_task_process_stop
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != MIPIO_PPP_CLOSE_CMD)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Close MIP IO operation.
  -------------------------------------------------------------------------*/
#if 0
  /*-------------------------------------------------------------------------
    Shuold be done this way to avoid ppp_close getting called twice, but 
    this breaks the case when DTR drops and MSM is still in closed state,
    in that case it does not call ppp_close.  Should fix it later.  
    // JD-rename or add new event
  -------------------------------------------------------------------------*/
  mip_meta_sm_post_event (MSM_IFACE_DOWN_EV);
#else
  mipio_ppp_close();
#endif

} /* mip_task_process_stop() */


/*===========================================================================
FUNCTION    MIP_TASK_RAND_DATA_READY

DESCRIPTION     
  This function is called by secutil when random data is available via
  secutil_get_rand().

PARAMETERS
  none

DEPENDENCIES    
  none

RETURN VALUE
  none

SIDE EFFECTS  
  None
===========================================================================*/
void mip_task_rand_data_ready
(
  void *user_data_ptr
) 
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    If DMU is enabled, signal the DMU subtask to regenerate new key data
  -------------------------------------------------------------------------*/
  MSG_MED ("Random data is available",0,0,0);

#ifdef FEATURE_DS_MOBILE_IP_DMU
    #ifdef FEATURE_USE_OLD_PS_RAND_IFACE
      PS_SET_SIGNAL (PS_MIP_DMU_RAND_SIGNAL);
    #else
      MIP_SEND_CMD( DMU_PREENCRYPT_CMD, NULL );
    #endif /* FEATURE_USE_OLD_PS_RAND_IFACE */
#endif /* FEATURE_DS_MOBILE_IP_DMU */
}
#endif /* FEATURE_DS_MOBILE_IP */

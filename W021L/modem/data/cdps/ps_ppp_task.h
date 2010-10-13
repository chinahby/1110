#ifndef PS_PPP_TASK_H
#define PS_PPP_TASK_H
/*===========================================================================
                          P S _ P P P _ T A S K . H

DESCRIPTION
  Header file containing the PPP task specific information.  This is intended
  to isolate all of the task specific information to a single module.

  Copyright 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_task.h_v   1.4   31 Jan 2003 18:45:14   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_task.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/07/03    usb    Added PPP_INJECT_PACKET_CMD to replay an out of order PPP 
                   packet for PPP receive processing.
01/27/03    usb    Added PPP_SET_MODE_CMD to set PPP mode in PS context only
09/23/02    mvl    Modified signal usage.
09/12/02    mvl    Removed netmodel featurization.
08/02/02    mvl    Added macro for setting a signal handler in PS.
07/25/02    mvl    Updates for PPP renaming.
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Added macros to make rest of PPP more independent of task.
05/22/02    mvl    created file
===========================================================================*/
#include "ps_svc.h"
#include "ps_ppp_defs.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                   MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Define the task functions in terms of the equivalent functions for the
  hosting task: in this case PS
---------------------------------------------------------------------------*/
#define PPP_SET_SIG_HANDLER( signal, cback, user_data ) \
  ps_set_sig_handler((signal),(cback), (user_data))

#define PPP_SEND_CMD( cmd, data_ptr )           \
  ps_send_cmd((cmd), (void *) (data_ptr))

#define PPP_ENABLE_SIG( signal )                \
  ps_enable_sig( (signal) )

#define PPP_DISABLE_SIG( signal )               \
  ps_disable_sig( (signal) )

#define PPP_IS_CURR_TASK_OWNER()                \
  (rex_self() == &ps_tcb)
  
/*---------------------------------------------------------------------------
  define all of the PPP related commands that can be sent in terms of the
  commands for the hosting task: in this case PS
---------------------------------------------------------------------------*/  
#define PPP_RESYNC_CMD          PS_PPP_START_CMD
#define PPP_START_CMD           PS_PPP_START_CMD
#define PPP_STOP_CMD            PS_PPP_STOP_CMD
#define PPP_ABORT_CMD           PS_PPP_ABORT_CMD
#define PPP_SET_MODE_CMD        PS_PPP_SET_MODE_CMD
#define PPP_INJECT_PKT_CMD      PS_PPP_INJECT_PKT_CMD

/*---------------------------------------------------------------------------
  define all of the signal related stuff.
---------------------------------------------------------------------------*/
typedef ps_sig_enum_type ppp_sig_type;

#define PPP_INVALID_SIGNAL PS_MAX_SIGNALS

#define PPP_UM_1_RX_SIGNAL PS_PPP_UM_SN_RX_SIGNAL
#define PPP_RM_RX_SIGNAL    PS_PPP_RM_RX_SIGNAL
#ifdef FEATURE_HDR_AN_AUTH
#error code not present
#endif

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PPP_TASK_POWERUP_INIT()

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
void ppp_task_powerup_init( void );

#endif /* PS_PPP_TASK_H */


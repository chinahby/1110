#ifndef PS_SVC_H
#define PS_SVC_H
/*===========================================================================

                        P S _ S V C . H

DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  all the definitions, functions, and structures for other software tasks
  to communicate with the Data Protocol Task.

EXTERNALIZED FUNCTIONS
  ps_enable_sig()
    Allow a signal to be processed
    
  ps_disable_sig()
    Disallow a signal to be processed
    
  ps_set_sig_handler()    
    Set a handler to process a signal in PS context
    
  ps_send_cmd()
    Post a command to the Data Protocol Task
    
  ps_set_cmd_handler()    
    Set a handler to process a cmd in PS context

Copyright (c) 1998-2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_svc.h_v   1.13   13 Feb 2003 14:14:26   ubabbar  $
    $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_svc.h#5 $ $DateTime: 2011/03/09 07:43:48 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported support for MOBILE_IP_DEREG feature.
06/09/07    scb    Added PS_UIM_CMD_SIGNAL and PS_GSDI_EVENT_HANDLER_CMD.
11/23/07    sn     Added MIP DMU command PS_DMU_PREENCRYPT_CMD definition.
04/15/03    ss     Added PS_LO_TX_DATA_Q_SIGNAL
04/24/03    usb    Added PS_PPP_INJECT_PACKET_CMD for replaying a PPP packet
02/11/03    usb    Added commands for IP snooping state machine
01/24/03    usb    Added PS_PPP_SET_MODE_CMD for setting PPP mode
12/29/02    jd     Added PS_MIP_DMU_RAND_SIGNAL
12/24/02    aku    Added PS_SOCKET_PHYS_LINK_UP_EVENT_CMD and
                   PS_SOCKET_PHYS_LINK_DOWN_EVENT_CMD
12/22/02    aku    Added PS_SOCKET_NETWORK_UP_EVENT_CMD and 
                   PS_SOCKET_NETWORK_DOWN_EVENT_CMD
11/19/02    jd     Added global ps_nv_status for use in PS task
11/14/02    jd     Added global ps_nv_item for use in PS task
                   Added MIP open and close commands
                   Removed unused PS_IFACE commands
11/13/02    rc     Added SNDCP signals for GPRS.
11/01/02    ifk    Added PS commands for DNS.
10/07/02    aku    Added PS_SOCKET_NETWORK_EVENT_CMD, 
                   PS_SOCKET_IFACE_ADDR_CHANGED_CMD and                   
                   PS_SOCKET_FLOW_ENABLED_SIG.
10/03/02    rc     Added PDP signals for UMTS.
08/19/02    atp    Added PS_707_ASYNC_UM_FLOW_ENABLE_CMD for 707 async.
08/05/02    usb    Moved PS_NV_CMD_SIGNAL out of FEATURE_MIP
07/22/02    aku    Added PS_DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD
07/19/02    na     Fixed START/STOP sig enumeration
07/16/02    aku    Removed SOCKET featurization for dssnet cmds.
07/16/02    atp    Renamed signal PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL.
07/15/02    na     Removed FEATURE_DATA_PS and PS_SOCKET_STATE_TIMER_SIGNAL
07/13/02    atp    Added/changed names of commands, signals for 707 async.
03/14/02    usb    Initial version of file
===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "rex.h"
#include "task.h"
#include "nv.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
extern nv_item_type         ps_nv_item;
extern nv_stat_enum_type    ps_nv_status;

/*--------------------------------------------------------------------------
  Data Protocol Task Signal Definitions.  The value of existing signals
  should not be changed while adding or deleting a signal.  Some signals
  (marked Reserved) have fixed value that should stay as it is.
--------------------------------------------------------------------------*/
typedef enum
{
  PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL=0,   /* Incoming TCP data signal      */
  PS_CMD_Q_SIGNAL                 =  1,   /* PS command Q signal           */
  PS_PPP_UM_SN_RX_SIGNAL          =  2,   /* Um PPP Rx data signal         */
  PS_PPP_RM_RX_SIGNAL             =  3,   /* Rm PPP RX data signal         */
  PS_DOG_RPT_TIMER_SIGNAL         =  4,   /* Watchdog Report signal        */
  PS_STATE_TIMER_SIGNAL           =  5,   /* PS State Timer signal         */
                                                                           
#ifdef FEATURE_DS_SOCKETS                                                  
  PS_SOCKET_RX_Q_SIGNAL           =  6,   /* Socket API has data to send   */
#endif /* FEATURE_DS_SOCKETS */                                            
                                                                           
  PS_MEM_EVENT_SIG_HANDLER        =  7,   /* Memory pool event occurred    */
  PS_TIMER_TICK_SIGNAL            =  8,   /* API Timer expired             */
  PS_NV_CMD_SIGNAL                =  9,   /* NV cmd event signal           */

#ifdef FEATURE_DS_MOBILE_IP                                                
  /*-------------------------------------------------------------------------
    MIP_SOCK_EVENT signal should follow the PS_TIMER_TICK_SIG to 
    maximize efficiency if the reg. holdoff timer and AAM arrive in
    the same loop through ps_task() (else we would dump the RRQ)
  -------------------------------------------------------------------------*/
  PS_MIP_SOCK_EVENT_SIGNAL        = 10,   /* Mobile IP socket event        */
  PS_MIP_NET_EVENT_SIGNAL         = 11,   /* Mobile IP network event       */
#endif /* FEATURE_DS_MOBILE_IP */                                          
                                                                           
/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  PS_TASK_OFFLINE_SIGNAL          = 13,   /* Reserved TASK_OFFLINE_SIG     */
  PS_TASK_STOP_SIGNAL             = 14,   /* Reserved TASK_STOP_SIG        */
  PS_TASK_START_SIGNAL            = 15,   /* Reserved TASK_START_SIG       */
                                                                           
#ifdef FEATURE_HDR_AN_AUTH                                                 
#error code not present
#endif /* FEATURE_HDR_AN_AUTH */

  PS_PDP0_UM_RX_Q_SIGNAL          = 18,   /* UMTS Um PDP0 Rx data Signal   */
  PS_PDP1_UM_RX_Q_SIGNAL          = 19,   /* UMTS Um PDP1 Rx data signal   */

#ifdef FEATURE_DS_SOCKETS
  PS_SOCKET_FLOW_ENABLED_SIG      = 20,  /* Flow enabled for apps          */
#endif /* FEATURE_DS_SOCKETS */

  PS_SNDCP_CMD_Q_SIGNAL           = 21,   /* GPRS CMD Q Signal             */
  PS_SNDCP_LL_DL_Q_SIGNAL         = 22,   /* GPRS DL Data Signal           */

#ifdef FEATURE_DS_MOBILE_IP_DMU
  PS_MIP_DMU_RAND_SIGNAL          = 23,   /* DMU Random data ready signal  */
#endif /* FEATURE_DS_MOBILE_IP_DMU */

  PS_LO_TX_DATA_Q_SIGNAL          = 24,   /* loopback data q signal        */ 
#if defined(FEATURE_UIM_SUPPORT_3GPD)
  PS_UIM_CMD_SIGNAL               = 25,
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  PS_EFS_SIGNAL                   = 30,   /* Reserved EFS -  signal        */
                                                                           
  PS_SIG_MASK_CHANGE_SIGNAL       = 31,   /* Indicates sig mask has changed*/
                                                                           
  PS_MAX_SIGNALS                          /* use next value for enum       */
} ps_sig_enum_type;

/*---------------------------------------------------------------------------
  Data Task Command Definitions. These are the commands that can be sent to
  one.
---------------------------------------------------------------------------*/

typedef enum
{
   PS_707_ASYNC_TCP_CTL_OPEN_ACTIVE_CMD  = 0/* Open async TCP conn (active)*/
  ,PS_707_ASYNC_TCP_CTL_OPEN_PASSIVE_CMD = 1/* Open async TCP conn(passive)*/
  ,PS_707_ASYNC_PTCL_ABORT_CMD           = 2/* Abort async protocols       */
  ,PS_707_ASYNC_PTCL_CLOSE_CMD           = 3/* Close async protocols       */
  ,PS_707_ASYNC_PPP_DOWN_CMD             = 4/* Async handle PPP down       */
  ,PS_707_ASYNC_PPP_UP_CMD               = 5/* Async handle PPP up         */
  ,PS_707_ASYNC_PPP_RESYNC_CMD           = 6/* Async handle PPP resync     */
  ,PS_707_ASYNC_UM_FLOW_ENABLE_CMD       = 7/* Async enable Um flow        */

#ifdef FEATURE_DS_SOCKETS
  ,PS_SOCKET_NET_DOWN_CMD        = 8  /* Network is down for the socket    */
  // why are there two open commands?
  ,PS_SOCKET_OPEN_PPP_CMD        = 9  /* Bring up PPP                      */
#endif /* FEATURE_DS_SOCKETS */
  ,PS_DSSNET_PPP_OPEN_CMD        = 10  /* Open PPP command                 */
  ,PS_DSSNET_PHY_IFACE_UP_CMD    = 11 /* Physical layer is up              */
  ,PS_DSSNET_LINK_LAYER_UP_WITH_MIP_CMD = 12 /* Link up with MIP command   */
  ,PS_DSSNET_LINK_LAYER_UP_WITH_SIP_CMD = 13 /* Link up with SIP command   */
  ,PS_DSSNET_MIP_UP_SUCCESS_CMD  = 14 /* MIP registration successful       */
  ,PS_DSSNET_MIP_FAILURE_CMD     = 15 /* MIP failure successful            */
  ,PS_DSSNET_NET_IFACE_DOWN_CMD  = 16 /* Network iface down                */
  ,PS_DSSNET_PPP_CLOSE_CMD       = 17 /* Close PPP command                 */
  ,PS_DSSNET_PHY_IFACE_TIMER_EXP_CMD = 18  /* Timer expiry command         */
  ,PS_DSSNET_PHY_IFACE_DOWN_CMD  = 19 /* Physical layer down               */
  ,PS_DSSNET_PPP_RESYNC_CMD      = 20 /* PPP Resync command                */
  ,PS_DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD= 21 /* PPP Abort command          */
  ,PS_SOCKET_NETWORK_UP_EVENT_CMD = 22       /* Iface has gone up          */
  ,PS_SOCKET_NETWORK_DOWN_EVENT_CMD = 23     /* Iface has gone down        */
  ,PS_SOCKET_IFACE_ADDR_CHANGED_CMD = 24     /* Iface address changed      */
  ,PS_SOCKET_PHYS_LINK_UP_EVENT_CMD = 25       /* Phys link has gone up    */
  ,PS_SOCKET_PHYS_LINK_DOWN_EVENT_CMD = 26     /* Phys link has gone down  */



  // These are the new commands for PPP handling - these
  // replace all of the other PPP commands that are in here.
  ,PS_PPP_START_CMD              = 27 /* start a PPP device                */
  ,PS_PPP_STOP_CMD               = 28 /* gracefully close a PPP device     */
  ,PS_PPP_ABORT_CMD              = 29 /* abort a PPP device                */
  ,PS_PPP_SET_MODE_CMD           = 30 /* change PPP operational mode       */
  ,PS_PPP_INJECT_PKT_CMD         = 31 /* inject a PPP pkt into the device  */

#ifdef FEATURE_DS_MOBILE_IP
  ,PS_MIPIO_PPP_OPEN_CMD         = 32 /* Bring up PPP for M.IP             */
  ,PS_MIPIO_PPP_CLOSE_CMD        = 33 /* Close PPP for MIP                 */
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
    Domain Name System resolver commands to provide PS task context.
  -------------------------------------------------------------------------*/
  ,PS_DNS_NET_CB_CMD             = 34 /* DNS network callback              */
  ,PS_DNS_SOCK_CB_CMD            = 35 /* DNS socket callback               */
  ,PS_DNS_START_CMD              = 36 /* DNS query received, start layers  */
  ,PS_DNS_STOP_CMD               = 37 /* DNS query released, stop layers   */
  
  ,PS_SNOOP_IP_PPP_DEV_UP_CMD    = 38  /* PPP device is up for IP          */
  ,PS_SNOOP_IP_PPP_DEV_DOWN_CMD  = 39  /* PPP device is down               */
  ,PS_SNOOP_IP_FLTR_ENABLED_CMD  = 40  /* IP filtering enabled on iface    */
  ,PS_SNOOP_IP_FLTR_DISABLED_CMD = 41  /* IP filtering disabled on iface   */
  ,PS_SNOOP_IP_COMP_ON_CMD       = 42  /* handle VJ compressed pkts        */
  ,PS_SNOOP_IP_COMP_OFF_CMD      = 43  /* No more VJ compressed pkts       */

#if (defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DS_MOBILE_IP_DMU))
  ,PS_DMU_PREENCRYPT_CMD         = 44 /* DMU Random event                  */
#endif /* FEATURE_DS_MOBILE_IP && FEATURE_DS_MOBILE_IP_DMU */

  ,PS_GSDI_EVENT_HANDLER_CMD     = 45  /* To handle SIM_INIT_COMPLETED ev  */
#if (defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DS_MOBILE_IP_DEREG))
  ,PS_DSSNET_MIP_DEREGED_EV_CMD  = 46  /* To handle MIP Dereg event */
#endif /* FEATURE_DS_MOBILE_IP && FEATUER_DS_MOBILE_IP_DEREG */

  ,PS_MAX_DEFINED_CMD_TYPES

} ps_cmd_enum_type;


/*--------------------------------------------------------------------------
  Type definition for the signal handler.
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
---------------------------------------------------------------------------*/

typedef boolean (*ps_sig_handler_type)
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*--------------------------------------------------------------------------
  Type definition for the command handler
---------------------------------------------------------------------------*/

typedef void (*ps_cmd_handler_type)
(
  ps_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr           /* Command specific user parameters       */
);


/*--------------------------------------------------------------------------
  Only the following macros should be used to set and clear signals from
  PS task context.
---------------------------------------------------------------------------*/
#define PS_SET_SIGNAL( sig ) \
  rex_set_sigs( &ps_tcb, (rex_sigs_type) 1 << ( sig ) )

#define PS_CLR_SIGNAL( sig ) \
  rex_clr_sigs( &ps_tcb, (rex_sigs_type) 1 << ( sig ) )


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION PS_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the PS task uses for rex_wait()

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void ps_enable_sig
(
  ps_sig_enum_type sig                        /* Signal to be enabled      */
);

/*===========================================================================

FUNCTION PS_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void ps_disable_sig
(
  ps_sig_enum_type sig                        /* Signal to be disabled     */
);

/*===========================================================================

FUNCTION PS_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

extern ps_sig_handler_type ps_set_sig_handler
(
  ps_sig_enum_type sig,              /* Signal assocaited with handler     */
  ps_sig_handler_type sig_handler,   /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
);

/*===========================================================================

FUNCTION PS_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS task context.  The cmd is
  processed by calling the registered cmd handler, if any.

  NOTE: The passed command will be copied to a PS task
  command buffer local to the PS Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ps_send_cmd
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================

FUNCTION PS_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  The cmd handling
  can be disabled by setting a NULL handler

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/

extern ps_cmd_handler_type ps_set_cmd_handler
(
  ps_cmd_enum_type cmd,              /* Command name                      */
  ps_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
);

#endif /* PS_SVC_H */


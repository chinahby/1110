/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               P S _ S V C . C

GENERAL DESCRIPTION
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

Copyright (c) 1993-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_svc.c_v   1.2   19 Nov 2002 17:29:10   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_svc.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/19/02    jd     Added global ps_nv_status for use in PS task
11/14/02    jd     Added global ps_nv_item for use in PS task
03/18/02    usb    Created Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_DATA_PS
#include "assert.h"
#include "rex.h"
#include "queue.h"
#include "msg.h"
#include "err.h"
#include "ps_svc.h"
#include "psi.h"


/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
nv_item_type         ps_nv_item;
nv_stat_enum_type    ps_nv_status;


/*===========================================================================

                           FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION PS_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask 
  which the PS task uses for rex_wait()
  
DEPENDENCIES
  A valid signal handler should be registered with PS before enabling the 
  signal.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ps_enable_sig
(
  ps_sig_enum_type sig               /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( psi_sig_handler[sig].sig_handler == default_sig_handler )
  {
    MSG_ERROR( "Signal %d enabled without a handler", sig, 0, 0 ); 
  }

  psi_enabled_sig_mask |= ( (rex_sigs_type) 1 << sig );

  PS_SET_SIGNAL( PS_SIG_MASK_CHANGE_SIGNAL );
} /* ps_enable_sig() */


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

void ps_disable_sig
(
  ps_sig_enum_type sig                        /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  psi_enabled_sig_mask &= ~( 1 << sig );
} /* ps_disable_sig() */


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
ps_sig_handler_type ps_set_sig_handler
(
  ps_sig_enum_type sig,              /* Signal assocaited with handler     */
  ps_sig_handler_type sig_handler,   /* Signal handler function pointer    */ 
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  ps_sig_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( sig_handler != NULL );

  old_handler = psi_sig_handler[sig].sig_handler;

  psi_sig_handler[sig].sig_handler = sig_handler;  
  psi_sig_handler[sig].user_data_ptr = user_data_ptr;

  return old_handler;
} /* ps_set_sig_handler() */


/*===========================================================================

FUNCTION PS_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS task context.  The cmd is 
  processed by calling the registered cmd handler, if any.

  NOTE: The passed command will be copied to a PS task
  command item local to the PS Task.  
  
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
)
{
  psi_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links 
    available, else copy buffer to cmd queue and signal PS task
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ( psi_cmd_type * ) q_get( &psi_cmd_free_q ) ) == NULL )
  {
    ERR_FATAL( "PS task cmd buffers unavailable", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;
    q_put( &psi_cmd_q, &cmd_ptr->link );
    PS_SET_SIGNAL( PS_CMD_Q_SIGNAL );
  }
} /* ps_send_cmd() */


/*===========================================================================

FUNCTION PS_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  If a NULL 
  function pointer is passed the cmd handling is disabled.
  
DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/
ps_cmd_handler_type ps_set_cmd_handler
(
  ps_cmd_enum_type cmd,              /* Command name                      */
  ps_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */ 
)
{
  ps_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_handler = psi_cmd_handler[cmd];

  psi_cmd_handler[cmd] = ( cmd_handler != NULL) ? 
                          cmd_handler : 
                          default_cmd_handler;

  return old_handler;
} /* ps_set_cmd_handler() */

#endif /* FEATURE_DATA_PS */


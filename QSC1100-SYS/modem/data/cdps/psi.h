#ifndef PSI_H
#define PSI_H
/*===========================================================================

                           P S I . H

DESCRIPTION

  Protocol Task local header file. All definitions, structures, and functions
  needed internal to protocol task services.
  
EXTERNALIZED FUNCTIONS  
  
  psi_process_cmd()
    Procsses a PS task cmd and calls appropriate handler function


 Copyright (c) 1993,1994 by QUALCOMM Incorporated.  All Rights Reserved.
 Copyright (c) 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
 Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/psi.h_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/psi.h#2 $ $DateTime: 2007/12/10 23:14:44 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/07   scb     Added new function prototype psi_gsdi_init()
08/05/02   usb     Added new function prototype psi_wait()
07/15/02   aku     Include dssocket.h for sockets build.
10/24/94   jjw     Initial version of file


===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_DATA_PS
#include "ps_svc.h"
#include "rex.h"
#include "queue.h"
#ifdef FEATURE_DS_SOCKETS
#include "dssocket.h"
#endif /* FEATURE_DS_SOCKETS */


/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/


/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/

typedef struct
{
  ps_sig_handler_type sig_handler;
  void *user_data_ptr;
} psi_sig_handler_info_type;

extern psi_sig_handler_info_type psi_sig_handler[PS_MAX_SIGNALS];
extern ps_cmd_handler_type psi_cmd_handler[PS_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  PS Task Command queues and Command Data Buffers
---------------------------------------------------------------------------*/

typedef struct     
{
  q_link_type link;		           /* For linking onto cmd queue             */
  ps_cmd_enum_type cmd;          /* Actual command to be processed         */
  void *user_data_ptr;           /* Command specific user parameters       */
} psi_cmd_type;

/*--------------------------------------------------------------------------
  Set the command buffer count to 30 (for general purpose use) and two for
  each application (one for sending PPP_OPEN_CMD and the other for sending
  the PPP_CLOSE_CMD)
---------------------------------------------------------------------------*/

/* TODO: Revisit buffer count computation, how do we get 30 */
#ifdef FEATURE_DS_SOCKETS
#define PSI_CMD_BUF_CNT  (30 + (2 * DSS_MAX_APPS))
#else
#define PSI_CMD_BUF_CNT  (30)
#endif /* FEATURE_DS_SOCKETS */

extern psi_cmd_type psi_cmd_buf[ PSI_CMD_BUF_CNT];

extern q_type psi_cmd_q;
extern q_type psi_cmd_free_q;

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
extern rex_sigs_type psi_enabled_sig_mask;

                                         

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PSI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to PS_CMD_Q_SIGNAL.  It retrieves the
  next command from the PS command queue and processes the command by calling 
  the registered cmd handler.
  
DEPENDENCIES
    An item must be on the PS command queue or an ERR_FATAL will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function 
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean psi_process_cmd
( 
  void
);


/*===========================================================================

FUNCTION PSI_TIMER_TICK_START_CB()

DESCRIPTION
  This is a callback function called by the pstimer module to start a tick
  timer. This function calls the clk_reg, to register the call back structure
  and also reset the ticks_elapsed_cntr to 0 here. Reseting the counter here
  ensures that every time we register the clock callback, the counter 
  restarts.
  
  This function has been made a callback function because, this ensures
  that pstimer module is decoupled from ps task. (one reason is if we
  decide to move the tick_timer to any other task, the changes should
  not effect the pstimer module).
  

DEPENDENCIES
  ps_tick_timer_struct should have been initialized before calling this 
  function. The initialization should be done by calling clk_def().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void psi_timer_tick_start_cb
(
  int4 ps_tick_interval               /* Requested duration of timer ticks */
);


/*===========================================================================

FUNCTION PSI_TIMER_TICK_STOP_CB()

DESCRIPTION
  
  This is a callback function called by the pstimer module to stop a tick
  timer. This function calls clk_dereg to deregister the call back. 
    
  This function has been made a callback function because, this ensures
  that pstimer module is decoupled from ps task. (one reason is if we
  decide to move the tick_timer to any other task, the changes should
  not effect the pstimer module).
  
DEPENDENCIES
  Tick Timer should have been initialized before calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void psi_timer_tick_stop_cb( void );


/*===========================================================================

FUNCTION PSI_WAIT()

DESCRIPTION
  This function performs the PS Task waiting function. This function will 
  not return until at least one signal from the requested mask is set. 
  The watchdog is also kicked at least once.
  
DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated 
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

===========================================================================*/
extern rex_sigs_type psi_wait
( 
  rex_sigs_type requested_mask              /* mask of signals to wait for */
);


/*===========================================================================

FUNCTION DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the 
  signals which do not have a registered handler. It merely prints a warning 
  message.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean default_sig_handler
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */ 
);


/*===========================================================================

FUNCTION DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the 
  commands which do not have a registered handler. It merely prints a warning 
  message.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void default_cmd_handler
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================

FUNCTION PSI_GSDI_INIT()

DESCRIPTION
  Initialization function for the initialization of GSDI module for PS.

DEPENDENCIES
  PS task should have been started.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void psi_gsdi_init
( 
  void 
);
#endif /* FEATURE_DATA_PS */

#endif /* PSI_H */


#ifndef DIAGTARGET_H
#define DIAGTARGET_H
/*==========================================================================

                 Diagnostic Target Specific Prototypes

Description
  Prototypes for functions and data members that must be implemented to make
  the core diagnostics service work.
  

Copyright (c) 2001 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagtarget.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/06   as      Fixed critical lint errors.
12/15/06   as      Fixed lint errors.
09/17/01   jal     Added prototype for diag_mode_change()
09/09/01    gr     Changed the featurization for the extern declaration
                   of the diag stack so that the stack is an array of type
                   rex_stack_word_type by default.
09/07/01    gr     Featurized the declaration of diag_stack. Restored
                   prototypes inadvertently deleted in the last check-in.
09/06/01    gr     Changed the extern declaration of diag_stack to reflect the
                   fact that the stack is now an array of type rex_stack_word_type.
06/27/01   lad     Added prototypes for diag_sleep_allow()/.._forbid().
04/06/01   lad     Added extern definitions of diag_tcb and diag_stack.
                   Changed diag_start_task() to diag_ready_start_task() to 
                   avoid confusion.
                   Added prototype for diag_kick_watchdog().
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "rex.h"

/* Declaration of the 8 bytes in the check block which is the security password.
** In DMSS targets, this symbol is located in the assemply firmware.
*/
extern uint8 const chk_security_code_ptr[];

/* The TCB and stack are defined outside the diag task.  In the MSMs, these are
 * defined in task.h/c
 */
#ifdef FEATURE_WORD_STACKS
   extern uint16 diag_stack [];
#else
#ifndef PLATFORM_LTK
   extern rex_stack_word_type diag_stack [];
#endif
#endif

extern rex_tcb_type        diag_tcb;                    


/*===========================================================================

FUNCTION DIAG_READY_START_TASK

DESCRIPTION
  This procedure is called when the diag task is ready to start.

===========================================================================*/
void diag_ready_start_task (void);



/*===========================================================================

FUNCTION DIAG_SHUTDOWN

DESCRIPTION
  This procedure performs shutdown processing for the Diagnostic task.

  After this procedure is called the Diagnostic task will not be running.  
 
===========================================================================*/
void diag_shutdown (void); 



/*===========================================================================

FUNCTION DIAG_OFFLINE

DESCRIPTION
  This function performs processing required when the diag task receives
  a signal to go offline.

===========================================================================*/
void diag_offline (void);



/*===========================================================================
FUNCTION DIAG_POWERDOWN

DESCRIPTION
  This procedure sends the command to MC to power down the phone.

SIDE EFFECTS
  The phone will power down as a result of this call.
 
===========================================================================*/
void diag_powerdown (void);



/*===========================================================================

FUNCTION DIAG_KICK_WATCHDOG

DESCRIPTION
  This procedure reports to the watchdog and resets the timer.

===========================================================================*/
void diag_kick_watchdog (void);



/*===========================================================================

FUNCTION DIAG_ALLOW_SLEEP

DESCRIPTION
  This function tells the sleep task that it is OK to allow sleep, 
  with regard to diagnostic packet processing.

===========================================================================*/
void diag_allow_sleep (void);



/*===========================================================================

FUNCTION DIAG_FORBID_SLEEP

DESCRIPTION
  This function tells the sleep task that it is *not* OK to allow sleep, 
  with regard to diagnostic packet processing.

===========================================================================*/
void diag_forbid_sleep (void);

#endif /* DIAGTARGET_H */

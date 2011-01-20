#ifndef HSU_OS_ABSTRACT_SYNC_H
#define HSU_OS_ABSTRACT_SYNC_H

/*==============================================================================

		              H S U _ O S _ A B S T R A C T _ S Y N C . H

  
GENERAL DESCRIPTION
  High Speed USB stack OS abstraction layer internal header file.

  Declarations of the High Speed USB stack REX OS abstraction layer 
  synchronization module, which are internal to the OS abstraction layer.
	
EXTERNALIZED FUNCTIONS
  Externalized internally in the OS abstraction layer:
    hsu_os_sync_init
    hsu_os_sync_uninit

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.
		
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/06   ds      Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <rex.h>
#include <jtypes.h>


/*==============================================================================

                DECLARATIONS

==============================================================================*/


/*==============================================================================
  Constants and Macros
==============================================================================*/

/* NOTE -Signals defined here must not conflict with the High-Speed USB stack 
** Adaptation layer task signals (defined in 'hsu_al_task.c'). This is 
** because the adaptation layer task calls user-supplied functions 
** which may in turn call High-Speed USB stack core API, which may 
** internally use the OS abstraction layer API.
** Additionally, these signals must not conflict with signals which are 
** in use by the application tasks.
*/

#define HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL 0x80000000 /*=REX_SLEEP_SIG*/
/*==============================================================================
  Typedefs
==============================================================================*/



/*==============================================================================
  External Variables
==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION DECLARATIONS

==============================================================================*/


/* ==========================================================================
FUNCTION HSU_OS_SYNC_INIT

DESCRIPTION
   Initializes the REX OS abstraction layer thread synchronization module.

DEPENDENCIES
   None.

RETURN VALUE
   If the initialization is successful, returns zero. Otherwise, returns 
   a non-zero error code.

SIDE EFFECTS
   Creates new REX threads.
============================================================================= */
jresult_t hsu_os_sync_init(void);

/* ==========================================================================
FUNCTION HSU_OS_SYNC_UNINIT

DESCRIPTION
  Un-initializes the REX OS abstraction layer thread synchronization module.

DEPENDENCIES
  The caller must make sure that all the synchronization resources 
  (e.g., threads) were released (using the proper un-init 
  functions) prior to calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  Rex tasks in the OS abstraction layer 'thread pool' are not killed during
  the OS abstraction layer un-initialization, since the behavior of 
  'rex_kill_task' is unknown.
============================================================================= */
void hsu_os_sync_uninit(void);

#endif /* HSU_OS_ABSTRACT_SYNC_H */

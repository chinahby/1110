#ifndef ACMTASK_H
#define ACMTASK_H


#include "comdef.h"     /* Definitions for byte, word, etc.     */


/*===========================================================================
FUNCTION acm_task

DESCRIPTION
  This function is entry point for the ACM task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  atprx_tcb
===========================================================================*/
extern void acm_task
(
  dword ignored     /* Parameter received from Main Control task - ignored */
);
#endif
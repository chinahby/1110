/*==========================================================================

                      R E X S W M    H E A D E R    F I L E

DESCRIPTION
   API for stack water mark feature. 


Copyright (c) 1990-1999 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 2000-2003 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/core/kernel/rex/main/latest/rexswm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/01   cr      Created module.

===========================================================================*/
#ifndef REX_SWM_H
#define REX_SWM_H

#ifdef FEATURE_SI_STACK_WM
#error code not present
#else

#define rex_swm_init(a)
#define rex_swm_update(a)
#define rex_swm_get_wm(a) 0
#define rex_swm_update_get_wm(a) 0
#define rex_swm_get_int_stack_wm(a) 0
#define rex_swm_update_all()
#define rex_swm_task(a)

#endif /* FEATURE_SI_STACK_WM */

#endif /* REX_SWM_H */

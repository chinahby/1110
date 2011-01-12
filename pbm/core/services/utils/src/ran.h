#ifndef RAN_H
#define RAN_H
/*===========================================================================

         P S E U D O - R A N D O M    N U M B E R    S E R V I C E S

DESCRIPTION
  This file contains declarations for use with the Pseudo-random Number
  Services.

Copyright (c) 1991, 1992 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998,1999  by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/COMMON/vcs/ran.h_v   1.0   Aug 21 2000 11:51:08   lpetrucc  $
$Header: //depot/asic/msmshared/services/common/ran.h#3 $ $DateTime: 2004/12/27 10:23:19 $ $Author: ckrishna $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/23/04   ty      Added function prototype for ran_init()
12/16/98   jct     Removed 80186 support
09/21/98   jct     Updated copyright, now includes comdef.h
09/01/98   jct     Added cdecl declaration
05/06/91   rdb     Created module.

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_SET_RAN_SEED
extern void   ran_init ( rex_tcb_type  *task_ptr,
                         rex_sigs_type task_wait_sig,
                         void (*task_wait_handler)(rex_sigs_type) );
#endif
extern void   ran_seed ( dword seed );
extern dword  ran_next ( void );
extern word   ran_dist ( dword  ran_num, word lo_val, word hi_val );

extern word   hash     ( dword hash_key, word  n, word  decorr );

#endif /* RAN_H */


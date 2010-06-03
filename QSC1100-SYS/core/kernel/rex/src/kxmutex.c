/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    KxMutex on REX

GENERAL DESCRIPTION
   KxMutex is mutex API cross all platforms, such as REXL4, WM, Linux 
   and Native REX. This file contains the implementation of KxMutex on 
   Native REX.

=============================================================================

API FUNCTIONS

  KxMutex_Init
     Initilalzes a KxMutex object.

  KxMutex_Free
     Free a KxMutex object.

  KxMutex_Lock
     Locks a KxMutex object. If the mutex is not available, the requestiong
     thread will be blocked.  It will be resumed once the mutex becomes 
     avaiable.

  KxMutex_Unlock
     Unlocks a KxMutex object

  KxMutex_TryLock
     Locks a KxMutex object. If the mutex is not available, the function
     will just return FALSE. Otherwise it returns TRUE.

Copyright (c) 2008 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //source/qcom/qct/core/kernel/rex/main/latest/kxmutex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/08   cz      Added KxMutex API for Native REX.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "kxmutex.h"
#include "assert.h"

/*
 * KxMutex_Init
 */
boolean KxMutex_Init(KxMutex* pMutex, const char *cpszName) 
{
   const char * name;

   ASSERT( pMutex != NULL );
   rex_init_crit_sect((rex_crit_sect_type *)pMutex);

   /* suppress lint error */
   name = cpszName;
   return TRUE;
}

/*
 * KxMutex_Free
 */
void KxMutex_Free(KxMutex* pMutex)
{
   ASSERT( pMutex != NULL );

   /* do nothing here since there is no corresponding function in REX crit_sect */
}

/*
 * KxMutex_Lock
 */
boolean KxMutex_Lock(KxMutex* pMutex)
{
   ASSERT( pMutex != NULL );

   rex_enter_crit_sect((rex_crit_sect_type *)pMutex);
   return TRUE;   
}

/*
 * KxMutex_TryLock
 */
boolean KxMutex_TryLock(KxMutex* pMutex)
{
   ASSERT( pMutex != NULL );
   return rex_try_enter_crit_sect((rex_crit_sect_type *)pMutex);
}

/*
 * KxMutex_Unlock
 */
void KxMutex_Unlock(KxMutex* pMutex)
{
   ASSERT( pMutex != NULL );
   rex_leave_crit_sect((rex_crit_sect_type *)pMutex);
}


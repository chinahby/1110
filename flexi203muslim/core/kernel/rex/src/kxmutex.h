#ifndef KXMUTEX_H
#define KXMUTEX_H
/*==========================================================================

                      KXMUTEX    on   R E X    H E A D E R    F I L E

DESCRIPTION
   KxMutex API on Native REX

Copyright (c) 2008 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/core/kernel/rex/main/latest/kxmutex.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/08   cz      Added KxMutex API for Native REX.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "rex.h"

typedef rex_crit_sect_type KxMutex;

/*======================================================================

  FUNCTION         KxMutex_Init()

  DESCRIPTION      This function is used to initialize
                   a IntraPD KxMutex object. 

  DEPENDENCIES     The user thread is responsible for allocating 
                   the memory used by a this KxMutex object.

  PARAMETERS       cpszName : Name of the newly-created mutex object.

  RETURN VALUE     Returns TRUE if the mutex object is successfully
                   initilized.  Otherwise returns FALSE.  

  SIDE EFFECTS     None

======================================================================*/
boolean KxMutex_Init(KxMutex* pMutex, const char * name);

/*======================================================================

  FUNCTION         KxMutex_Free()

  DESCRIPTION      For IntraPD Mutex, this function free the object. 
                   
  DEPENDENCIES     None

  PARAMETERS       me : Pointer of the mutex object.

  RETURN VALUE     None

  SIDE EFFECTS     None

======================================================================*/
void KxMutex_Free(KxMutex* pMutex);

/*======================================================================

  FUNCTION         KxMutex_Lock()

  DESCRIPTION      This function is called by a KxThread to lock a mutex
                   object.

  DEPENDENCIES     None

  PARAMETERS       pMutex : Pointer of the mutex object to be locked.

  RETURN VALUE     Returns TRUE if the mutex is successfully locked.
                   Otherwise something is wrong.  User need to check.

  SIDE EFFECTS     If the mutex is not available, the requestiong
                   KxThread will be blocked.  It will be resumed once
                   the mutex becomes avaiable.

======================================================================*/
boolean KxMutex_Lock(KxMutex* pMutex);

/*======================================================================

  FUNCTION         KxMutex_TryLock()

  DESCRIPTION      This function is called by a KxThread to lock a mutex
                   object.  This function is a non-blocking call - If
                   the mutex object is not available, the requestiong
                   KxThread will not be blocked.

  DEPENDENCIES     None

  PARAMETERS       pMutex : Pointer of the mutex object to be locked.

  RETURN VALUE     Returns TRUE if the mutex is successfully locked.  If
                   the mutex is not available, returns FALSE.

  SIDE EFFECTS     None

======================================================================*/
boolean KxMutex_TryLock(KxMutex* pMutex);

/*======================================================================

  FUNCTION         KxMutex_Unlock()

  DESCRIPTION      This function is called by a KxThread to unlock a
                   mutex object.  If this function is called by a
                   KxThread other than the owner of the mutex, a fault
                   will be triggered.

  DEPENDENCIES     None

  PARAMETERS       pMutex : Pointer of the mutex object to be unlocked.

  RETURN VALUE     None

  SIDE EFFECTS     If a KxThread with higher priority is waiting for the
                   mutex object, it will run before this function
                   returns.

======================================================================*/
void KxMutex_Unlock(KxMutex* pMutex);

#endif /* END KXMUTEX_H */

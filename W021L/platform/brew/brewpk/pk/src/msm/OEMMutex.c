/*=============================================================================

FILE: OEMMutex.c

SERVICES: OEM Functions for muteces

GENERAL DESCRIPTION:
Provides methods for acquiring and releasing muteces.
  
PUBLIC CLASSES AND STATIC FUNCTIONS:
   OEMMutex_Enter
   OEMMutex_Leave

INITIALIZATION AND SEQUENCING REQUIREMENTS:
RegisterMutexForSWI() must have previously been called.

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "comdef.h"        // Get DMSS type declarations.
#include "rex.h"
#include "OEMMutex.h"
#include "OEMHeap.h"

/*===========================================================================
                      TYPE DEFINITIONS
 ===========================================================================*/

typedef struct {
   rex_crit_sect_type crit_sect;
} Mutex;

/*===========================================================================
                      STATIC DATA
 ===========================================================================*/

/*===========================================================================
                      STATIC PROTOTYPES
 ===========================================================================*/

/*===========================================================================
                      FUNCTION DEFINITIONS
 ===========================================================================*/


/*=====================================================================
Function: OEMMutex_Create

Description:
   Dynamically allocates and initializes a mutex.
=====================================================================*/
OEMMutex OEMMutex_Create(void)
{
   Mutex* me;
   dword dwSave;

   // the heap is not thread safe...
   INTLOCK_SAV(dwSave); {

      me = sys_malloc(sizeof(Mutex));
      if ((Mutex*)0 == me) {
         INTFREE_SAV(dwSave);
         return (Mutex*)0;
      }

   } INTFREE_SAV(dwSave);

   rex_init_crit_sect(&me->crit_sect);
   return me;
}

/*=====================================================================
Function: OEMMutex_Lock

Description:
   Acquires a mutex.  Sleeps if another task already holds the mutex.
=====================================================================*/
void OEMMutex_Lock(OEMMutex m)
{
   rex_enter_crit_sect(&((Mutex*)m)->crit_sect);
}

/*=====================================================================
Function: OEMMutex_Unlock

Description:
   Releases a mutex.  Awakes any task waiting to acquire the mutex.
=====================================================================*/
void OEMMutex_Unlock(OEMMutex m)
{
   rex_leave_crit_sect(&((Mutex*)m)->crit_sect);
}

/*=====================================================================
Function: OEMMutex_Destroy

Description:
   Uninitializes and dynamically deallocates a mutex.
=====================================================================*/
void OEMMutex_Destroy(OEMMutex m)
{
   dword dwSave;

   // the heap is not thread safe...
   INTLOCK_SAV(dwSave); {

      sys_free(m);

   } INTFREE_SAV(dwSave);
}


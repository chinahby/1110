/*=============================================================================

FILE: OEMCriticalSection.c

SERVICES: OEM Functions for critical sections

GENERAL DESCRIPTION:
Provides methods for entering and leaving critical sections.
  
PUBLIC CLASSES AND STATIC FUNCTIONS:
   OEMCriticalSection_Enter
   OEMCriticalSection_Leave

INITIALIZATION AND SEQUENCING REQUIREMENTS:
RegisterCriticalSectionForSWI() must have previously been called.

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "comdef.h"
#include "rex.h"
#include "OEMCriticalSection.h"

typedef struct {
   boolean bInit;
   rex_crit_sect_type crit_sect;
} CriticalSection;

// Translate the OEMCriticalSection handle the user gave us into a
// CriticalSection pointer.  This includes initializing it the first
// time.  This looks like it is doing a lot of work, but there is
// actually not much to it the nominal case - it is mostly handling
// the exception cases.
static __inline CriticalSection* CriticalSection_FromOEMCriticalSection(OEMCriticalSection* pm)
{ 
   CriticalSection* me = (CriticalSection*)pm;

   // Initialize once, in a thread safe manner
   if (FALSE == me->bInit) {
      dword dwSave = 0;

      // prevent two tasks from initializing the same critical_section
      INTLOCK_SAV(dwSave);
      if (FALSE == me->bInit) {
         rex_init_crit_sect(&me->crit_sect);
         me->bInit = TRUE;
      }
      INTFREE_SAV(dwSave);
   }

   return me;
}

/*=====================================================================
Function: OEMCriticalSection_Enter

Description:
   Enters a critical_section.  Sleeps if another task already in the
   critical section.
=====================================================================*/
void OEMCriticalSection_Enter(OEMCriticalSection* pm)
{
   CriticalSection* me = CriticalSection_FromOEMCriticalSection(pm);

   rex_enter_crit_sect(&me->crit_sect);
}

/*=====================================================================
Function: OEMCriticalSection_Leave

Description:
   Leaves a critical section.  Awakes and task waiting to enter the
   critical section
=====================================================================*/
void OEMCriticalSection_Leave(OEMCriticalSection* pm)
{
   CriticalSection* me = CriticalSection_FromOEMCriticalSection(pm);

   rex_leave_crit_sect(&me->crit_sect);
}


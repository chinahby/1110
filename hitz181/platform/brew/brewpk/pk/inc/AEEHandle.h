#ifndef AEEHANDLE_H
#define AEEHANDLE_H

/*===========================================================================

FILE:      AEEHANDLE.h
 
SERVICES:  Handles
 
DESCRIPTION: Translates between an OEMINSTANCE and a small integer handle.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: AEEHandleList struct must
   be initialized (see below).

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "AEE_OEMComdef.h"
#include "AEEComdef.h"

typedef struct {
   OEMINSTANCE  i;              // actual instance
   boolean      bValid;         // is this instance valid?
} AEEInstance;

typedef struct {
   AEEInstance* pInstances;     // (sparse) array of instances
   uint16       dwCount;        // number of entries in pInstances
   uint16       dwNext;         // hint for new search start
} AEEInstanceList;

#if defined(__cplusplus)
extern "C" {
#endif

/**************************************************************************/
/*                     OEM MUTEX EXPORTED METHODS                         */
/**************************************************************************/

extern boolean AEEHandle_To(AEEInstanceList* pIL, OEMINSTANCE i, uint32* ph);
extern boolean AEEHandle_Lookup(const AEEInstanceList* pIL, OEMINSTANCE m, uint32* pHandle);
static __inline boolean AEEHandle_From(AEEInstanceList* pIL, uint32 h, OEMINSTANCE* pi)
{
   // invalid handle??
   if (0 == h || h > pIL->dwCount ||
       FALSE == pIL->pInstances[h-1].bValid) {
      return FALSE;
   }

   *pi = pIL->pInstances[h-1].i; // one based indexing...
   return TRUE;
}
extern boolean AEEHandle_Clear(AEEInstanceList* pIL, uint32 h);

#if defined(__cplusplus)
}
#endif


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEEInstance

Description:
   Declares storage space for the user instance, plus a flag for
   whether or not the instance is valid.

Definition:
typedef struct {
   OEMINSTANCE  i;
   boolean      bValid;
} AEEInstance;

Members:
   i      - actual instance
   bValid - is this instance valid?

Comments:
   Note that bValid could be compressed into a separate bit array
   within AEEInstanceList for space reasons, but at the expense of ease
   of (re)allocation and implementation, so we waste 31 bits per
   instance.

See Also:
AEEInstanceList

=======================================================================

AEEInstanceList

Description:
   Declares storage space for the instance list.  The pInstances member
   must point to enough space for dwCount AEEInstances. dwLast should
   be zero initialized and then never accessed by the client.

Definition:
typedef struct {
   AEEInstance* pInstances;     // (sparse) array of instances
   uint16       dwCount;        // number of entries in pInstances
   uint16       dwLast;         // hint for new search start
} AEEInstanceList;

Members:
   pInstances - pointer to an array of AEEInstances
   dwCount    - number of entries in pInstances
   dwLast     - hint for new search start

Comments:
1) The contents of AEEInstanceList can be allocated either statically or
   dynamically.  For example:

   static AEEInstance gpInstances[16];
   static AEEInstanceList gHandleList = { gpInstances,
                       sizeof(gpInstances)/sizeof(gpInstances[0]), 0 };

   - or -

   gHandleList.pInstances = malloc(16*sizeof(AEEInstance));
   if ((AEEInstance*)0 != gHandleList.pInstances) {
      gHandleList.dwCount = 16;
   }
   gHandleList.dwLast = 0;

2) If the contents of AEEInstanceList are dynamically allocated, it is 
   possible to reallocate it, for example when AEEHandle_To() returns 0.
   If reallocation is used, care must be taken if thread safety is
   required (for example, by protecting all access to the AEEInstanceList with
   an OEMMutex).

3) dwCount should be larger (for example, 2x) than the maximum number of handles
   expected to be in use at any one time.  If not, the same handle may be
   used twice is quick succession, which could make validation difficult.

See Also:
AEEHandle_To()
AEEHandle_From()
AEEHandle_Clear()

=======================================================================
  INTERFACES DOCUMENTATION
=======================================================================
AEEHandle Interface
=======================================================================

Function: AEEHandle_To()

Description:
   Translates from an OEMINSTANCE to a small integer handle.

Prototype:
   boolean AEEHandle_To(AEEInstanceList* pIL, OEMINSTANCE m, uint32* ph);

Parameters:
   pIL - pointer to a instance list
   m   - the instance to translate
   ph  - pointer to handle to be filled in

Return Value:
   TRUE in the case of success
   FALSE in the case that there are no available slots in the instance list

Comments:
   The AEEInstanceList pointer must be initialized before first use.  See
   the documentation for AEEInstanceList() for more details.

Side Effects:
   None

See Also:
   AEEInstanceList
   AEEHandle_From()

=======================================================================

Function: AEEHandle_Lookup()

Description:
   Looks up an existing handle from an instance.

Prototype:
   uint32 AEEHandle_Lookup(const AEEInstanceList* pIL, OEMINSTANCE m);

Parameters:
   pIL - pointer to a instance list
   m   - the instance to look up

Return Value:
   TRUE in the case of success
   FALSE in the case that the instance was not found

Comments:
   The AEEInstanceList pointer must be initialized before first use.  See
   the documentation for AEEInstanceList() for more details.

Side Effects:
   None

See Also:
   AEEInstanceList
   AEEHandle_From()

=======================================================================

Function: AEEHandle_From()

Description:
   Translates from a small integer handle to an OEMINSTANCE.

Prototype:
   boolean AEEHandle_From(AEEInstanceList* pIL, uint32 h, OEMINSTANCE* pi);

Parameters:
   pIL - pointer to a valid instance list
   h   - the handle to translate
   pi  - pointer to instance to be filled in

Return Value:
   TRUE in the case of success
   FALSE in the case that there is no instance for this handle

Comments:
   The handle to translate must have been returned from AEEHandle_To().

Side Effects:
   None

See Also:
   AEEInstanceList
   AEEHandle_To()

=======================================================================

Function: AEEHandle_Clear()

Description:
   Clears the OEMINSTANCE for a handle, making the handle available for reuse.

Prototype:
   boolean AEEHandle_Clear(AEEInstanceList* pIL, uint32 h);

Parameters:
   pIL - pointer to a valid instance list
   h   - the handle to clear

Return Value:
   TRUE in the case of a valid handle
   FALSE in the case of an invalid handle

Comments:
   The handle to clear must have been returned from AEEHandle_To().

Side Effects:
   None.

See Also:
   AEEInstanceList
   AEEHandle_To()

=========================================================================*/

#endif /* AEEHANDLE_H */

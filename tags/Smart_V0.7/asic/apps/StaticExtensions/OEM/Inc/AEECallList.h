/*=============================================================================

FILE: OEMCallHistory.h
 
SERVICES:  OEM Call History  
 
GENERAL DESCRIPTION:
 
PUBLIC CLASSES AND STATIC FUNCTIONS:           
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:         
 
(c) COPYRIGHT 2002 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //tsg/UI/main/TSGBREWOEM/inc/OEMCallHistory.h#3 $
$DateTime: 2002/09/30 18:17:47 $
  $Author: sbairi $
  $Change: 61514 $
                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/30/02   snb     Removed some of the macros and put them in .c file.
09/13/02   snb     Added support to categorize all the recent calls.
                   Added OEM_CallHistoryEditEntry() to edit a number and 
                   OEM_CallHistoryDeleteEntry to delete the entry.
02/01/02   mjv     CR #273: Added Unicode support to the OEM Call History API. 
01/24/02   mjv     Created this file from OEMAppFuncs.h, renamed 
                   OEM_MergeRUIMCallHistory() to OEM_CallHistoryMergeRUIM(),
                   added OEM_CallHistoryInit().
 
=============================================================================*/

#ifndef AEECALLLIST_H
#define AEECALLLIST_H

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

// comdef.h unconditionally defines ARR_SIZE.
//
// Ensure ARR_SIZE is not defined (by AEEComdef.h) if comdef.h has not
// been included yet as the next include will cause comdef.h to be included 
// indirectly. 
#ifndef COMDEF_H
#undef ARR_SIZE
#endif
#ifndef WIN32
#include "nv.h"
#endif
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
////
// Max number of digits in a call history entry 
#define CALLHISTORY_MAXDIGITS             (33+2)  //two bytes for number count

////
// Max number of letters of an call history entry 
// (including the NULL byte)
#define CALLHISTORY_MAXLETTERS            33

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/


typedef enum _CallHistoryStatus{
  CALLHISTORY_NULL = 0,   // Not a real calltype

  CALLHISTORY_INCOMING=0,
  CALLHISTORY_OUTGOING,
  CALLHISTORY_MISSED,
  //CALLHISTORY_WAITING,
  CALLHISTORY_INCOMING_3WAY,
  CALLHISTORY_OUTGOING_3WAY,

  // This calltype contains data from the RUIM card.  They are OUTGOING calls
  // but do not contain a valid timestamp or call duration
  CALLHISTORY_FROM_RUIM
} CallHistoryStatus;

//define all the categories here
typedef enum _CallHistoryCategory{
  CALLHISTORY_MISSED_CATEGORY= 0,     /* Missed category   */
  CALLHISTORY_INCOMING_CATEGORY,      /* Incoming category */
  CALLHISTORY_OUTGOING_CATEGORY,      /* Outgoing category */
  CALLHISTORY_ALL_CATEGORY            /* All category */
} CallHistoryCategory;


typedef struct _OEMCallHistoryEntry{
  CallHistoryStatus  status;             
  dword              time_stamp;                 // years to seconds
  AECHAR             digits[CALLHISTORY_MAXDIGITS];  
  AECHAR             letters[CALLHISTORY_MAXLETTERS];
  dword              duration;                   // Duration of call in seconds
} OEMCallHistoryEntry;


typedef struct ICallList ICallList;

AEEINTERFACE(ICallList)
{
    INHERIT_IQueryInterface(ICallList);
    void (*Init)(ICallList *pCallList);
    boolean (*InitEnd)(ICallList *pCallList);
    boolean (*EditEntry)(ICallList *pCallList,
                         CallHistoryCategory  nCat,
                         byte   nElement,
                         const AECHAR   *wPhone);
    boolean (*DeleteEntry)(ICallList *pCallList,
                           CallHistoryCategory  nCategory,
                           byte  nElement);
    void (*AddEntry)(ICallList *pCallList,
                     CallHistoryStatus  calltype,
                     const AECHAR  *wAlpha,
                     const AECHAR  *wPhone,
                     dword   durationMS);
    boolean (*GetEntry)(ICallList *pCallList,
                        CallHistoryCategory  nCat,
                        byte   nElement,
                        OEMCallHistoryEntry  *ent);
    void (*Reset)(ICallList *pCallList);
#ifdef FEATURE_RUIM_PHONEBOOK     
    void (*MergeRUIM)(ICallList *pCallList);  
#endif //FEATURE_RUIM_PHONEBOOK      
    void (*ClearCalls)(ICallList *pCallList, const byte status);
};

#define ICALLLIST_AddRef(p)                    AEEGETPVTBL((p),ICallList)->AddRef((p)) 
#define ICALLLIST_Release(p)                   AEEGETPVTBL((p),ICallList)->Release((p))
#define ICALLLIST_QueryInterface(p,i,o)        AEEGETPVTBL((p),ICallList)->QueryInterface((p),(i),(o))
#define ICALLLIST_Init(p)                      AEEGETPVTBL((p),ICallList)->Init((p))
#define ICALLLIST_InitEnd(p)                   AEEGETPVTBL((p),ICallList)->InitEnd((p))
#define ICALLLIST_EditEntry(p, nc, ne, wp)     AEEGETPVTBL((p),ICallList)->EditEntry((p),(nc),(ne),(wp))
#define ICALLLIST_DeleteEntry(p, nc, ne)       AEEGETPVTBL((p),ICallList)->DeleteEntry((p),(nc),(ne))
#define ICALLLIST_AddEntry(p,c,wa,wp,d)        AEEGETPVTBL((p),ICallList)->AddEntry((p),(c),(wa),(wp),(d))
#define ICALLLIST_GetEntry(p,n,ne,e)           AEEGETPVTBL((p),ICallList)->GetEntry((p),(n),(ne),(e))
#define ICALLLIST_Reset(p)                     AEEGETPVTBL((p),ICallList)->Reset((p))
#ifdef FEATURE_RUIM_PHONEBOOK 
#define ICALLLIST_MergeRUIM(p)                 AEEGETPVTBL((p),ICallList)->MergeRUIM((p))
#endif //FEATURE_RUIM_PHONEBOOK 
#define ICALLLIST_ClearCalls(p,s)              AEEGETPVTBL((p),ICallList)->ClearCalls((p),(s))
/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION:  ICALLLIST_Init
 
DESCRIPTION:  Initializes the OEM Call History.  
 
PARAMETERS:
   None
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION:  ICALLLIST_EditEntry

DESCRIPTION:
   This function will just modify the digits in the entry. 
   
   If the call type is any one of the outgoing types ie. 
   CALLHISTORY_FROM_RUIM, CALLHISTORY_OUTGOING, CALLHISTORY_OUTGOING_3WAY,
   we wil also modify the matching number in equivalent location in the RUIM.

PARAMETERS:
   nCat  [in]: Category of the entry MISSED CALLS, INCOMING CALLS,OUTGOING CALLS
   nElement - index of the element in this category. 
   wPhone - new phone number 
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION:  ICALLLIST_DeleteEntry

DESCRIPTION:
   This function will delete an entry from the stack. 
   
   If the call type is any one of the outgoing types ie. 
   CALLHISTORY_FROM_RUIM, CALLHISTORY_OUTGOING, CALLHISTORY_OUTGOING_3WAY,
   we wil also delete the entry in equivalent location in the RUIM.

PARAMETERS:
   nCategory  [in]: Category of the entry MISSED CALLS, INCOMING CALLS,
                    OUTGOING CALLS
   nElement - index of the element in this category. 
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION:  ICALLLIST_AddEntry

DESCRIPTION:
  Add an entry in call list. mostly copied from uinscall.c,
  remove dependency on global variable stack_data.n, stack_data.pse

  Note that if the calltype is CALLHISTORY_FROM_RUIM, the call history index
  (CFGI_LAST_CALL) is not actaully saved to NV.  The caller must to this.

PARAMETERS:
   calltype:
   *alphatag: 
   *phoneNumber: 
   durationMS:  call duration in ms (only for incoming/outgoing)

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION:  ICALLLIST_GetEntry

DESCRIPTION: Get a Call History entry.

PARAMETERS:
   nCat  [in]: Category of the entry MISSED CALLS, INCOMING CALLS,OUTGOING CALLS
   index [in]: Which entry to read.  This should be a value >= 0
   *ent [out]: Returns the contents of the next call history item.

RETURN VALUE:
boolean: TRUE if *ent contains the next call history item
         FALSE if index does not refer to a valid call history entry

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION:  ICALLLIST_Reset

DESCRIPTION: Delete the NV call history

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

//增加删除全部电话功能函数
//add this function for deleting all  calls
/*===========================================================================
FUNCTION  ICALLLIST_ClearCalls

DESCRIPTION
  Delete all callls from the NV call history.

  Note that the call history index is not actaully saved
  to NV.  The caller must to this using OEMPriv_CallHistory_SaveIndex()

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
===========================================================================*/

/*=============================================================================
FUNCTION:  ICALLLIST_MergeRUIM

DESCRIPTION:
   Tries to merge RUIM call history numbers with
   the NV call history using the following logic:

   Check all the LND numbers on the RUIM against the MO numbers in the
   NV call history, which start at CALLHISTORY_OUTGOING_START_INDEX,
   in chronological order.  If any numbers differ
   (the Alpha field is not compared, only the Digit field), delete all
   the NV MO numbers and replace them with the RUIM ones in the outgoing 
   category
   If there are no NV MO numbers but there are RUIM numbers, behave as
   described in the above paragraph. (except without deleting the NV MO
   numbers because there aren't any).  This can only happen when you change 
   the RUIM.

   If there are more RUIM numbers then NV numbers,delete all
   the NV MO numbers and replace them with the RUIM ones.This can only happen
   when you change the RUIM.

   If there are more NV numbers than RUIM numbers (and all the found
   RUIM/NV numbers match), do nothing.

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

#endif /* AEECALLLIST_H */

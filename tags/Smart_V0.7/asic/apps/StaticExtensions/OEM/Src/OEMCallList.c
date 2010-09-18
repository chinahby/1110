/*lint -e551 -e533 -e550 -e830 -e529 -e715 -e534*/
/*=============================================================================

FILE: OEMCallHistory.c
 
SERVICES: OEM Call History 
 
GENERAL DESCRIPTION:        
 
PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
   The phone MUST be in the ONLINE state before calling any Call History
   function except OEMCallList_Init() and OEMCallList_MergeRUIM()

 
(c) COPYRIGHT 2002 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //tsg/UI/main/TSGBREWOEM/src/OEMCallHistory.c#7 $
$DateTime: 2002/10/11 10:24:39 $
  $Author: sbairi $
  $Change: 63551 $
                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/10/02   snb     CALLHISTORY_OUTGOING_3WAY calltypes should also be added to 
                   RUIM LND entries
09/30/02   snb     Added some of the macros from the .h file here
09/25/02   snb     Added support for categories in call history
09/17/02   mjv     Include OEMConfig.h instead of OEMCFGI.h
09/13/02   mjv     Added Dynamic R-UIM support
08/27/02   mjv     Split the recent call timer into MO and MT calls.
08/05/02   kjm     Define NVRUIM_STACK_I_RUIMSTATUS if it's not defined
05/20/02   mjv     CR #489:  Fixed array bounds overflow in 
                   OEMCallList_MergeRUIM() when the entire NV call history
                   is full of MT or missed calls.
05/09/02   mjv     Ported to BREW 1.2
05/09/02   mjv     Load the Call History in the background on startup
03/09/02   mjv     OEMCallList_Init() is called twice during startup, but
                   make sure it only loads the call history from NV the first
                   time it's called. 
03/03/02   mjv     Fixed a bug in OEMPriv_ClassHistoryClearMO() that would 
                   cause the wrong call history entries to be deleted in some 
                   cases.
                   Added more ASSERT()s to the code. 
                   Added the EMPTY_INDEX #define to better clarify what an 
                   'empty' call history index should be.
                   Added an 'OEMPriv_' prefix to the local functions.
02/26/02   mjv     Call history alphatag is no longer assumed to be stored
                   as an ASCIIZ string.
02/22/02   mjv     Linted the file.
02/01/02   mjv     CR #273: Added Unicode support to the OEM Call History API. 
01/24/02   mjv     Created this file from OEMAppFuncs.h, renamed 
                   OEM_MergeRUIMCallHistory() to OEM_CallHistoryRUIMMerge(),
                   added OEMCallList_Init().

 
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"
#include "OEMClassIDs.h"
#include "OEMDB.h"
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEECallList.h"
//#include "OEMMacros.h"
#include "OEMNV.h"
#include "AEERUIM.h"
#include "OEMHeap.h"
#include "OEMOS.h"
#include "OEMConfig.h"
#include "OEMCFGI.h"
//#include "oemcfgi.h"
//#include "OEMStartupJob.h"
//#include "AEE_OEMDispatch.h"
#include "assert.h"
#include"AEE_OEM.h"
#ifdef FEATURE_RUIM_PHONEBOOK

#ifndef FEATURE_UIM
#error FEATURE_UIM needs to be defined for FEATURE_RUIM_PHONEBOOK
#endif

#include "nvruimi.h"
#ifndef NVRUIM_STACK_I_RUIMSTATUS
#define NVRUIM_STACK_I_RUIMSTATUS  0xFF
#endif

#include "Appscommon.h"

#endif /* FEATURE_RUIM_PHONEBOOK */


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

#if CALLHISTORY_MAXDIGITS <= NV_MAX_DIAL_DIGITS
#error CALLHISTORY_MAXDIGITS should be larger than NV_MAX_DIAL_DIGITS
#endif

#if CALLHISTORY_MAXLETTERS <= NV_MAX_LTRS
#error CALLHISTORY_MAXLETTERS should be larger than NV_MAX_LTRS
#endif

//Marker used in index stack that indicates that this slot is empty
#define EMPTY_INDEX ((byte)(0xFF))



////
// The numbers in the stack is divided into 3 categories MISSED,INCOMING 
//OUTGOING. 
#define CALLHISTORY_NUM_CATEGORY          3

//The total number of calls that can be logged in each category 
#define MAX_CALLS_IN_CATEGORY (NV_MAX_STACK_DIALS/CALLHISTORY_NUM_CATEGORY)

//The base index where the MISSED calls start in the stack
#define CALLHISTORY_MISSED_START_INDEX    0

//The base index where the INCOMING calls start in the stack
#define CALLHISTORY_INCOMING_START_INDEX  (MAX_CALLS_IN_CATEGORY)

//The base index where the OUTGOING calls start in the stack
#define CALLHISTORY_OUTGOING_START_INDEX  (2*MAX_CALLS_IN_CATEGORY)

static boolean          gbCacheStartInit   = FALSE;
static boolean          gbCacheInitEnd = FALSE;
//boolean           gbCacheInited;     //Has cache been read in? Don't check cache pointer itself 
                                     //  because there may not be any records to put in cache
static AEECallback      gCacheInitCB;
//static uint16           gCacheInitPrevRec;  
//#define OEMCALLLIST_RESUMECB(p) AEE_ResumeCallback(p,AEE_RESUME_CB_SYS)
#define OEMCALLLIST_RESUMECB(p) AEE_ResumeCallback(p,0)
/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

/*===========================================================================

                    PRIVATE STRUCTURE DEFINITIONS

===========================================================================*/
struct ICallList
{
    ICallListVtbl   *pvt;
    IShell   *m_pShell;
    uint32    m_nRefs;
    IRUIM    *m_pRUIM;
};

/*===========================================================================

                    LOCAL/STATIC DATA 

===========================================================================*/

//
// This structure holds the raw NV call history information
//

static struct {
  // Call stack index.  Each entry in this index refers to an entry into
  // st[], an entry == EMPTY_INDEX refers to an empty entry.
  nv_stack_idx_type sidx;
  nv_stdial_type st[NV_MAX_STACK_DIALS]; // Call stack data
} callstack_data;


////
// The next entry to be loaded. Used by OEMPriv_CallHistory_LoadNextFromNV
static byte sLoadIdx;

////
// Total number of NV entries in each category MISSED,INCOMING,OUTGOING.  
// Used by OEMPriv_CallHistory_LoadNextFromNV
static byte sNumEntries[CALLHISTORY_NUM_CATEGORY];


/*===========================================================================

                    LOCAL FUNCTION DECLARATIONS

===========================================================================*/
static uint32 OEMCallList_AddRef(ICallList *pMe) ;
static uint32 OEMCallList_Release(ICallList *pMe) ;
static int OEMCallList_QueryInterface(ICallList *pMe, AEECLSID id,
                                  void **ppNew) ;

static void OEMCallList_Init(ICallList *pMe);
static boolean OEMCallList_EditEntry(ICallList *pMe,
                                     CallHistoryCategory  nCat,
                                     byte                 nElement,
                                     const AECHAR        *wPhone);
                                     
static boolean OEMCallList_DeleteEntry(ICallList *pMe,
                                       CallHistoryCategory  nCategory,
                                       byte                 nElement);   
                                       
static void OEMCallList_AddEntry(ICallList *pMe,
                                 CallHistoryStatus  calltype,
                                 const AECHAR      *wAlpha,
                                 const AECHAR      *wPhone,
                                 dword              durationMS);       
                                                                                           
static void OEMPriv_CallHistoryAddEntryEx(ICallList *pMe,
                                          CallHistoryStatus  calltype,
                                          const byte        *alphatag,
                                          const byte        *phoneNumber,
                                          byte               num_len,
                                          dword              durationMS);
                                          
static boolean OEMCallList_GetEntry(ICallList *pMe,
                                    CallHistoryCategory  nCat,
                                    byte                 nElement,
                                    OEMCallHistoryEntry *ent);
                                   
static void OEMCallList_Reset(ICallList *pMe);       
                         
#ifdef FEATURE_RUIM_PHONEBOOK
static void OEMPriv_CallHistoryClearMO(void);
static void OEMCallList_MergeRUIM(ICallList *pMe);
#endif

static boolean OEMCallList_InitEnd(ICallList *pMe);
static void OEMPriv_CallHistory_SaveIndex(void);
static void OEMCallList_ClearCalls(ICallList *pMe,const byte status);
static void OEMPriv_CallHistory_LoadNextFromNV(void);
static int      InitAddrBkCacheEx(void);

static ICallList gcalllist;

static const VTBL(ICallList) gOEMCallListFuncs = {
    OEMCallList_AddRef,
    OEMCallList_Release,
    OEMCallList_QueryInterface,
    OEMCallList_Init,
    OEMCallList_InitEnd,
    OEMCallList_EditEntry,
    OEMCallList_DeleteEntry,
    OEMCallList_AddEntry,
    OEMCallList_GetEntry,
    OEMCallList_Reset,
#ifdef FEATURE_RUIM_PHONEBOOK 
    OEMCallList_MergeRUIM,
#endif //FEATURE_RUIM_PHONEBOOK    
    OEMCallList_ClearCalls
};
/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION OEMCallList_New
  
===========================================================================*/
int OEMCallList_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
    ICallList *pMe = NULL;

#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif))
   {
      return EBADPARM;
   }
   
   if (cls != AEECLSID_CALLLIST)
   {
      return EUNSUPPORTED;
   }
    
   pMe = &gcalllist;

   if (0 == pMe->m_nRefs)
   {
        // Must initialize the object
        MEMSET(pMe, 0, sizeof(ICallList));
        INIT_VTBL(pMe, ICallList, gOEMCallListFuncs);
        
        pMe->m_nRefs   = 0;
        pMe->m_pShell  = pIShell;
        
        (void)ISHELL_AddRef(pIShell);
        (void)ISHELL_CreateInstance(pMe->m_pShell,
                              AEECLSID_RUIM,
                              (void **)&pMe->m_pRUIM);
        OEMCallList_Init(pMe);
   }
   
   ++pMe->m_nRefs;
   *ppif = pMe;
   return AEE_SUCCESS;
}

/*===========================================================================

FUNCTION OEMCallList_AddRef
   <see ICallList_AddRef() documentation in AEECallList.h>

===========================================================================*/
static uint32 OEMCallList_AddRef(ICallList *pMe)
{
    if (pMe) 
    {
        return (++pMe->m_nRefs);
    }
    return 0;
}

/*===========================================================================

FUNCTION OEMCallList_Release
   <see ICallList_Release() documentation in AEECallList.h>

===========================================================================*/
static uint32 OEMCallList_Release(ICallList *pMe)
{
    if (!pMe) 
    {
        return (0);
    }
    
    if (--pMe->m_nRefs != 0) 
    {
        return pMe->m_nRefs;
    }
    
    if (pMe->m_pRUIM != NULL)
    {
        (void) IRUIM_Release(pMe->m_pRUIM);
        pMe->m_pRUIM = NULL;
    }
     
    (void)ISHELL_Release(pMe->m_pShell);
    
    // Ref count is zero.  Release memory
    sys_free(pMe);
    return (0);
}

/*===========================================================================

FUNCTION OEMCallList_QueryInterface
   <see ICallList_QueryInterface() documentation in AEECallList.h>

===========================================================================*/
static int OEMCallList_QueryInterface(ICallList *pMe, AEECLSID id, void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!pMe) || (!ppNew)) {
      return EFAILED;
   }

   switch (id) {
      case AEECLSID_CALLLIST:
         (void)OEMCallList_AddRef(pMe);
         *ppNew = pMe;
         break;
      default:
         *ppNew = NULL;
         ret_val = ECLASSNOTSUPPORT;
         break;
   }
   return ret_val;
}

/*===========================================================================

FUNCTION: InitAddrBkCacheEx

DESCRIPTION
  This function initializes the addressbook-cache. It reads all the records
  from the address book and places them inside the cache in a sorted order.
  All untitled records are placed at the beginning of the cache.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if successful

SIDE EFFECTS
  none
===========================================================================*/
static int      InitAddrBkCacheEx(void)
{
    if(gbCacheStartInit)
    {
        return SUCCESS;
    }
    
    //Read each record in the address book and place its ID and Name in the appropriate
    //place in the cache
//    gCacheInitPrevRec = AEE_DB_RECID_NULL;
    
    CALLBACK_Init( &gCacheInitCB, 
                  (PFNNOTIFY)OEMPriv_CallHistory_LoadNextFromNV,
                   NULL);
    
    gbCacheStartInit = TRUE;
    
    if (!CALLBACK_IsQueued(&gCacheInitCB))
    {
        OEMCALLLIST_RESUMECB(&gCacheInitCB);
    }
}//InitAddrBkCacheEx


/*===========================================================================

FUNCTION: OEMPriv_CallHistory_LoadNextFromNV

DESCRIPTION
  This function initializes the addressbook-cache. It reads all the records
  from the address book and places them inside the cache in a sorted order.
  All untitled records are placed at the beginning of the cache.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if successful

SIDE EFFECTS
  none
===========================================================================*/
static void OEMPriv_CallHistory_LoadNextFromNV(void)
{
   //get the category number. 
   byte nCategory = sLoadIdx/MAX_CALLS_IN_CATEGORY; 
   
   //actual number of indices used in this category
   byte nActualIndices = (byte)((nCategory*MAX_CALLS_IN_CATEGORY)+
                          sNumEntries[nCategory]);
                       
   if(!gbCacheStartInit)
   {
        // Unavailable call
       return; 
   }
   
   if ( sLoadIdx >= NV_MAX_STACK_DIALS)
   {
      gbCacheInitEnd = TRUE;
      return;
   }
   else if ((EMPTY_INDEX == callstack_data.sidx.index[sLoadIdx]))
   {
      //if the index is empty, then go start with the other category
      sLoadIdx = (byte)((nCategory+1) * MAX_CALLS_IN_CATEGORY);
      OEMCALLLIST_RESUMECB(&gCacheInitCB);
      return;
   }

   // Every index entry must be smaller than the total number of indices
   // in that category.
   if (callstack_data.sidx.index[sLoadIdx] >= nActualIndices) {
      // Mark this index as empty..
      callstack_data.sidx.index[sLoadIdx] = EMPTY_INDEX;

      // ...and mark all subequent indices in this category as empty too
      sNumEntries[nCategory] = 0;

   } else {
      callstack_data.st[callstack_data.sidx.index[sLoadIdx]].status = 
                                                        (int) CALLHISTORY_NULL;
      callstack_data.st[callstack_data.sidx.index[sLoadIdx]].address =
                                           callstack_data.sidx.index[sLoadIdx];
     (void) OEMNV_Get(NV_STACK_I,
                      (nv_item_type *)(void *)
                      &callstack_data.st[callstack_data.sidx.index[sLoadIdx]]);
   }
     
   sLoadIdx++;
   OEMCALLLIST_RESUMECB(&gCacheInitCB);
}


/*=============================================================================
FUNCTION:  OEMCallList_Init
 
DESCRIPTION:  Initializes the OEM Call History.  
 
PARAMETERS:
   None
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:       

=============================================================================*/
static void OEMCallList_Init(ICallList *pMe)
{
  static boolean callhistory_init = FALSE;     //Intialization variable
  byte           idxFound[NV_MAX_STACK_DIALS]; 
  boolean        callHistoryBad;
  byte           numEntries;
  byte           i; 
  byte           nCat;                 //number of categories. used in for loop
  byte           nCategoryMaxIndex;    //max index possible in that category
  byte           nCategoryIndexStart;  //starting index in that category
        
  if (!pMe)
  {
     return; 
  }
                    
  // OEM_InitPreference() is currently called twice during
  // startup.  This function takes approx one second to execute,
  // so calling it a second time will delay phone start up
  // unnecessarily by one second.
  if (TRUE == callhistory_init) {
     return;
  } 
  callhistory_init = TRUE;
  // Lint complains that nv_item_type is larger than 
  // callstack_data.sidx.  This is correct, but it 
  // really doesn't matter.
    
  /*lint -save -e826*/
  (void) OEMNV_Get(NV_STACK_IDX_I, 
                   (nv_item_type *) &callstack_data.sidx);
  /*lint -restore*/

  MEMSET(idxFound, EMPTY_INDEX, sizeof(idxFound));

  callHistoryBad = FALSE;

  //Calculate the number of indexes used in each category. Will be useful
  // when we read each one from the nv file
  for(nCat = CALLHISTORY_MISSED_CATEGORY; nCat <= CALLHISTORY_OUTGOING_CATEGORY; 
      nCat++){
     // Count the number of valid indices in the index
     // and verify there aren't any duplicate indices
     numEntries = 0;
     
     //Maximum index that is possible for this category
     nCategoryMaxIndex = (byte)(((nCat+1) * MAX_CALLS_IN_CATEGORY) -1);
     
     //Index start for this category
     nCategoryIndexStart = (byte)(nCat * MAX_CALLS_IN_CATEGORY) ;
     
     for (;;) {
        if (numEntries >= MAX_CALLS_IN_CATEGORY) {
           break;
        }
        
        //index should not be greater than the max index of that category
        if (callstack_data.sidx.index[nCategoryIndexStart+numEntries] 
            > nCategoryMaxIndex) {
           // Be paranoid and make sure the rest of the indices
           // contain valid data
           for (i = nCategoryIndexStart+numEntries; i <= nCategoryMaxIndex; 
                i++) {
              callstack_data.sidx.index[i] = EMPTY_INDEX;
           }
           break;
        }
   
        if (idxFound[callstack_data.sidx.index[nCategoryIndexStart+numEntries]]
             != EMPTY_INDEX) {
           // duplicate entries!
           callHistoryBad = TRUE;
           break;
        }
   
        // Mark this index as used
        idxFound[callstack_data.sidx.index[nCategoryIndexStart+numEntries]] = 
           nCategoryIndexStart+numEntries; 
        
        numEntries++;
     }
     if (callHistoryBad) {
        OEMCallList_Reset(pMe);
     }
     sNumEntries[nCat] = numEntries;
  }
  //If call history is not corrupted then read each entry allocated in each,
  // of the category
  if (!callHistoryBad) {
     sLoadIdx = 0;

     (void)InitAddrBkCacheEx();

  }
} // OEM_CallHistoryInit()


/*=============================================================================
FUNCTION:  OEMCallList_InitEnd

DESCRIPTION:
   
PARAMETERS:
   
RETURN VALUE:
=============================================================================*/
static boolean OEMCallList_InitEnd(ICallList *pMe)
{
    if (!pMe)
    {
        return FALSE; 
    }    
    
    return(gbCacheInitEnd);
}
/*=============================================================================
FUNCTION:  OEMCallList_EditEntry

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
static boolean OEMCallList_EditEntry(ICallList     *pMe,
                                     CallHistoryCategory  nCat,
                                     byte                 nElement,
                                     const AECHAR        *wPhone)
{
   byte     num_len;                            //length of phone number
   byte     nIndex;
   char     digits[CALLHISTORY_MAXDIGITS];
   byte     actualIndex;
   boolean  retVal = FALSE;
   
   if (!pMe)
   {
       return FALSE; 
   }
   
   //based on the element find the index in the index stack
   nIndex = (byte)((nCat * MAX_CALLS_IN_CATEGORY)+nElement) ;

   //Sanity checks
   if( nIndex >= NV_MAX_STACK_DIALS){
      return FALSE;
   }

   if (wPhone != NULL) {
      uint32 len = (uint32) WSTRLEN(wPhone);
      num_len= (byte)MIN(len, CALLHISTORY_MAXDIGITS);
   } else {
      return FALSE;
   }
   //Initalise the buffers
   MEMSET(digits,0,sizeof(digits));
   
   //convert the phone number from AEChar to char/byte
   WSTR_TO_STR(wPhone,
               digits,
               (int)(num_len + 1));

   //Find the actual Index where this index item is store from index stack.
   actualIndex = callstack_data.sidx.index[nIndex];

   //reset the digits
   MEMSET((char*)callstack_data.st[actualIndex].digits,
           0,
           NV_MAX_DIAL_DIGITS);
   
   //Find the right length for the digits
   if (num_len > sizeof(callstack_data.st[0].digits)) {
      callstack_data.st[actualIndex].num_digits = 
         sizeof(callstack_data.st[0].digits);
   } else {
      callstack_data.st[actualIndex].num_digits = num_len;
   }
   
   //Now change the digits at this location
   MEMCPY((char*)callstack_data.st[actualIndex].digits,
           digits,
           callstack_data.st[actualIndex].num_digits); 

   //Put it back on NV
   if(OEMNV_Put(NV_STACK_I,
               (nv_item_type *)(void *)
               &callstack_data.st[actualIndex])==NV_DONE_S){
      retVal = TRUE;
   }
   return retVal;

}

/*=============================================================================
FUNCTION:  OEMCallList_DeleteEntry

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
static boolean OEMCallList_DeleteEntry(ICallList   *pMe,
                                       CallHistoryCategory  nCategory,
                                       byte                 nElement)
{
   byte      actualIndex,highestIndex,stackIndexId;
   boolean   retVal = FALSE;
   byte      nIndex;
   int       j;           
   byte      nCategoryMaxIndex;    //max index possible in that category
   byte      nCategoryIndexStart;  //starting index in that category
   
   if (!pMe)
   {
       return FALSE; 
   }
   
   //based on the element find the index in the index stack
   nIndex = (byte)((nCategory * MAX_CALLS_IN_CATEGORY)+nElement) ;


   //Sanity checks
   if( nIndex >= NV_MAX_STACK_DIALS){
      return FALSE;
   }
  
   //Maximum index that is possible for this category
   nCategoryMaxIndex = (byte)(((nCategory+1) * MAX_CALLS_IN_CATEGORY) -1);
  
   //Index start for this category
   nCategoryIndexStart = (byte)(nCategory * MAX_CALLS_IN_CATEGORY) ;

   //Find the actual Index where this index item is store from index stack.
   actualIndex = callstack_data.sidx.index[nIndex];

   //Mark old index as empty, move all the index up and save index stack
   for(j=nIndex+1;j<=nCategoryMaxIndex;j++){
      if(j < NV_MAX_STACK_DIALS) {
         callstack_data.sidx.index[j-1] = callstack_data.sidx.index[j];
      }
   }
   //Mark the last one was empty
   if (nCategoryMaxIndex < NV_MAX_STACK_DIALS) {
      callstack_data.sidx.index[nCategoryMaxIndex] = EMPTY_INDEX;
   }
   
   //Now move the entries in the stack so that we do not causes holes in the
   //stack entries. For this, we will see which is the highest index in this
   //category. We will move that nv item at actualIndex location.
   highestIndex = EMPTY_INDEX;
   stackIndexId = EMPTY_INDEX;
   for ( j =nCategoryIndexStart ; j<= nCategoryMaxIndex; j++){
      //this condtion makes lint happy
      if(j < NV_MAX_STACK_DIALS){
         if(callstack_data.sidx.index[j] == EMPTY_INDEX) {
            break;
         }
         if((highestIndex <  callstack_data.sidx.index[j]) ||
            (highestIndex == EMPTY_INDEX)){
            highestIndex = (byte)callstack_data.sidx.index[j];
            stackIndexId = (byte)j;
         }
      }
   }
  
   //if this category list empty, after deleting, 
   if (highestIndex== EMPTY_INDEX){
#if 0      
      #ifdef FEATURE_RUIM_PHONEBOOK
      if (nCategory == CALLHISTORY_OUTGOING_CATEGORY)
      {      
         if (IRUIM_IsCardConnected(pMe->m_pRUIM))
         { 
            nv_item_type      nvi_ruim;
            MEMSET((void*)&nvi_ruim, 0, sizeof(nvi_ruim));
            nvi_ruim.stack.address = (byte) actualIndex - CALLHISTORY_OUTGOING_START_INDEX;//(byte) 0;
            nvi_ruim.stack.status = NVRUIM_STACK_I_RUIMSTATUS;
            OEMNV_Put(NV_STACK_I,
                      &nvi_ruim);
         }
      }       
      #endif        
#endif      
      retVal = TRUE;
   } else {
     
      //Move the nv item at highestIndex to actualIndex(deleted Index)
      if(highestIndex > actualIndex){
         if((highestIndex < NV_MAX_STACK_DIALS) &&
            (stackIndexId < NV_MAX_STACK_DIALS) &&
            (actualIndex  < NV_MAX_STACK_DIALS )){
            callstack_data.sidx.index[stackIndexId] = actualIndex;
      
            MEMCPY((void*)&callstack_data.st[actualIndex],
                   (void*)&callstack_data.st[highestIndex],
                   sizeof(callstack_data.st[0]));
      
            callstack_data.st[actualIndex].address = actualIndex;
            /*lint -save -e826*/
            (void) OEMNV_Put(NV_STACK_I, 
                            (nv_item_type *) &callstack_data.st[actualIndex]);
            /*lint -restore*/
#if 0
            #ifdef FEATURE_RUIM_PHONEBOOK
            if (nCategory == CALLHISTORY_OUTGOING_CATEGORY)
            {
               if (IRUIM_IsCardConnected(pMe->m_pRUIM))
               { 
                  nv_item_type      nvi_ruim;
                  MEMSET((void*)&nvi_ruim, 0, sizeof(nvi_ruim));
                  nvi_ruim.stack.address = (byte) actualIndex - CALLHISTORY_OUTGOING_START_INDEX;
                  nvi_ruim.stack.status = NVRUIM_STACK_I_RUIMSTATUS;
                  OEMNV_Put(NV_STACK_I,
                            &nvi_ruim);
               }
            } 
            #endif        
#endif
         }
         
      }
      retVal = TRUE;
   }
   //save the index stack
   OEMPriv_CallHistory_SaveIndex();
   return retVal;

}                

/*=============================================================================
FUNCTION:  OEMCallList_AddEntry

DESCRIPTION:
  Add an entry in call list. mostly copied from uinscall.c,
  remove dependency on global variable callstack_data.n, callstack_data.pse

  Note that if the calltype is CALLHISTORY_FROM_RUIM, the call history index
  is not actaully saved to NV.  The caller must to this.

PARAMETERS:
   calltype:
   alphatag: 
   phoneNumber:
   durationMS:  call duration in ms (only for incoming/outgoing)

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMCallList_AddEntry(ICallList     *pMe,
                                 CallHistoryStatus  calltype,
                                 const AECHAR      *wAlpha,
                                 const AECHAR      *wPhone,
                                 dword              durationMS)
{
   char   alpha[CALLHISTORY_MAXLETTERS];
   uint32 num_len;
   char   digits[CALLHISTORY_MAXDIGITS];
   
   if (!pMe)
   {
       return; 
   }
   
   //convert the AECHAR to byte/char
   OEMNV_WSTR_TO_RUIMAlpha(wAlpha,
                           (byte*) alpha,
                           sizeof(alpha));

   if (wPhone != NULL) {
      uint32 len = (uint32) WSTRLEN(wPhone);

      num_len= MIN(len, CALLHISTORY_MAXDIGITS);
   } else {
      num_len = 0;
   }

   //convert the AECHAR to byte/char
   WSTR_TO_STR(wPhone,
               digits,
               (int)(num_len + 1));

   //Add this entry in the nv file
   OEMPriv_CallHistoryAddEntryEx(pMe,
                                  calltype,
                                 (byte*) alpha,
                                 (byte*) digits,
                                 (byte) num_len,
                                 durationMS);
}


/*=============================================================================
FUNCTION:  OEMPriv_CallHistoryAddEntryEx
 
DESCRIPTION: Internal function to add an entry to the call history
 
PARAMETERS:
    calltype: 
   *alphatag: Note that the buffer pointed to by alphatag is assumed to be 
              larger than or equal to NV_MAX_LTRS bytes.
   *phoneNumber: 
    num_len: 
    durationMS: 
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMPriv_CallHistoryAddEntryEx(ICallList *pMe,CallHistoryStatus  calltype,
                                          const byte        *alphatag,
                                          const byte        *phoneNumber,
                                          byte               num_len,
                                          dword              durationMS)
{
   int  j;
   int  last_index;
   byte nCat, nCategoryMaxIndex,nCategoryIndexStart;
   
   //Determine which category should this entry be added to based on the
   //calltype
   switch(calltype){
      case CALLHISTORY_MISSED:
         nCat = CALLHISTORY_MISSED_CATEGORY;
         nCategoryIndexStart =  CALLHISTORY_MISSED_START_INDEX;
         break;
         
      case CALLHISTORY_INCOMING:
      case CALLHISTORY_INCOMING_3WAY:
         nCat = CALLHISTORY_INCOMING_CATEGORY;
         nCategoryIndexStart =  CALLHISTORY_INCOMING_START_INDEX;
         break;

      case CALLHISTORY_OUTGOING:
         nCat = CALLHISTORY_OUTGOING_CATEGORY;
         nCategoryIndexStart =  CALLHISTORY_OUTGOING_START_INDEX; 
      	 break;
      case CALLHISTORY_OUTGOING_3WAY:
      default :
         nCat = CALLHISTORY_OUTGOING_CATEGORY;
         nCategoryIndexStart =  CALLHISTORY_OUTGOING_START_INDEX;
         break;


   }

   //Maximum index that is possible for this category
   nCategoryMaxIndex = (byte)(((nCat+1) * MAX_CALLS_IN_CATEGORY) -1);

   //find the last index in this category
   last_index = callstack_data.sidx.index[nCategoryMaxIndex];
   
   
   // Make room for the new entry at the start of the index list
   // by shifting off the last index.
   for (j = nCategoryMaxIndex; j > nCategoryIndexStart; j--) {
      callstack_data.sidx.index[j] = callstack_data.sidx.index[j-1];
   }

   // if the call history is not yet full use the first free entry,
   // otherwise reuse the entry that was just shifted off the index.
   if (last_index == EMPTY_INDEX) {
      last_index = nCategoryMaxIndex;
      while (callstack_data.sidx.index[last_index] == EMPTY_INDEX) {
         if (nCategoryIndexStart == last_index) {
            break;
         }

         last_index--;
      }
   }

   ASSERT(last_index < NV_MAX_STACK_DIALS);

   // Setup the entry pointers correctly
   callstack_data.sidx.index[nCategoryIndexStart] = (byte) last_index;
   callstack_data.st[last_index].address = (byte) last_index;

   callstack_data.st[last_index].status = (byte) calltype;

   if (calltype != CALLHISTORY_FROM_RUIM) {
      callstack_data.st[last_index].duration = durationMS;
      callstack_data.st[last_index].time_stamp =
                       GETTIMESECONDS() - durationMS/1000;  // current time
   } else {
      // when adding from the RUIM, the duration and time_stamp 
      // are unknown and it is always a MO calltype
      callstack_data.st[last_index].duration = 0;
      callstack_data.st[last_index].time_stamp = 0;
   }

   //Initialise the digits and numbers in the call list item
   MEMSET((void*)callstack_data.st[last_index].digits,
          0,
          sizeof(callstack_data.st[0].digits));

   MEMSET((void*)callstack_data.st[last_index].letters,
          0,
          sizeof(callstack_data.st[0].letters));
   
   //Find the right length for the digits
   if (num_len > sizeof(callstack_data.st[0].digits)) {
      callstack_data.st[last_index].num_digits = 
         sizeof(callstack_data.st[0].digits);
   } else {
      callstack_data.st[last_index].num_digits = num_len;
   }

   //Copy the phone number and the name to the call list item
   MEMCPY((void *)callstack_data.st[last_index].digits,
          phoneNumber,
          callstack_data.st[last_index].num_digits);

   MEMCPY((void *) callstack_data.st[last_index].letters,
          alphatag,
          sizeof(callstack_data.st[0].letters));

   // Save the entry index if not adding an 'FROM_RUIM' call history type.
   // The caller is responsible for saving the entry index in that case.
   if (calltype != CALLHISTORY_FROM_RUIM) {
      OEMPriv_CallHistory_SaveIndex();
   }

   // Save the new call data entry
   
   /*lint -save -e826*/
   (void) OEMNV_Put(NV_STACK_I, 
                    (nv_item_type *) &callstack_data.st[last_index]);
   /*lint -restore*/
#if 0
#ifdef FEATURE_RUIM_PHONEBOOK
   // Write MO calls on the RUIM too
   if ((CALLHISTORY_OUTGOING == calltype) ||
       (CALLHISTORY_OUTGOING_3WAY == calltype))
   {
      if (IRUIM_IsCardConnected(pMe->m_pRUIM))
      {
         //save the call type status. We will need to restore the original
         //call type after we write to the RUIM
         byte oldStatus = calltype;
    
         callstack_data.st[last_index].address = (byte) last_index - CALLHISTORY_OUTGOING_START_INDEX; //(byte) 0;
         callstack_data.st[last_index].status = NVRUIM_STACK_I_RUIMSTATUS;
         /*lint -save -e826*/
         (void) OEMNV_Put(NV_STACK_I, 
                          (nv_item_type *) &callstack_data.st[last_index]);
         /*lint -restore*/
         // restore original calltype
         callstack_data.st[last_index].status = (int) oldStatus;
      }
   }
#endif
#endif
   // increment the call timers in the preference file
   if ( durationMS > 0 &&
        ((calltype == CALLHISTORY_INCOMING) ||
         (calltype == CALLHISTORY_OUTGOING)) ) {

      uint32        timerMS;
      AEEConfigItem item;

      timerMS = 0;
      (void) OEM_GetConfig(CFGI_ALL_CALL_TIMER, &timerMS, sizeof(timerMS));
      timerMS += durationMS;
      (void) OEM_SetConfig(CFGI_ALL_CALL_TIMER, &timerMS, sizeof(timerMS));

      if (calltype == CALLHISTORY_INCOMING) {
         item = CFGI_RECENT_MT_CALL_TIMER;
      } else if (calltype == CALLHISTORY_OUTGOING) {
         item = CFGI_RECENT_MO_CALL_TIMER;
      } else {
         return; // bail out!
      }

      timerMS = 0;
      (void) OEM_GetConfig(item, &timerMS, sizeof(timerMS));
      timerMS += durationMS;
      (void) OEM_SetConfig(item, &timerMS, sizeof(timerMS));

   }

}


/*=============================================================================
FUNCTION:  OEMCallList_GetEntry

DESCRIPTION: Get a Call History entry.

PARAMETERS:
   nCat    [in]: Category of the entry MISSED CALLS, INCOMING CALLS,
                    OUTGOING CALLS
   nElement[in] - index of the element in this category. 
   *ent [out]: Returns the contents of the next call history item.

RETURN VALUE:
boolean: TRUE if *ent contains the next call history item
         FALSE if index does not refer to a valid call history entry

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean OEMCallList_GetEntry(ICallList    *pMe,
                                    CallHistoryCategory  nCat,
                                    byte                 nElement,
                                    OEMCallHistoryEntry *ent)
{
   byte entry_index, index;
   
   if (!pMe)
   {
       return FALSE; 
   }
   
   //based on the element find the index in the index stack
   index = (byte)((nCat * MAX_CALLS_IN_CATEGORY)+nElement) ;
   
   //Sanity checks
   if (NULL == ent) {
      return FALSE;
   }
   
   // if above maximum possible or if the index is empty then abort 
   if ( (index >= NV_MAX_STACK_DIALS) ||
        (index >= (byte)((nCat+1) * MAX_CALLS_IN_CATEGORY)) ||
        (callstack_data.sidx.index[index] == EMPTY_INDEX) ) {
      return FALSE;
   }

   entry_index = callstack_data.sidx.index[index];

   ent->status     = (CallHistoryStatus) callstack_data.st[entry_index].status;
   ent->time_stamp = callstack_data.st[entry_index].time_stamp;
   ent->duration   = callstack_data.st[entry_index].duration;

   //convert the char to AECHARs
   STRTOWSTR((char*)callstack_data.st[entry_index].digits,
               ent->digits,
               sizeof(AECHAR)*MIN(callstack_data.st[entry_index].num_digits+1,
                                  CALLHISTORY_MAXDIGITS));
   //Convert the char to AECHAR 
   OEMNV_RUIMAlpha_TO_WSTR((byte*)callstack_data.st[entry_index].letters,
                           sizeof(callstack_data.st[entry_index].letters),
                           ent->letters,
                           sizeof(ent->letters));
   return TRUE;
}


/*=============================================================================
FUNCTION:  OEMCallList_Reset

DESCRIPTION: Delete the call history (from both NV and the RUIM card)

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMCallList_Reset(ICallList *pMe)
{
   int               j;
#ifdef FEATURE_RUIM_PHONEBOOK
   nv_item_type      nvi_ruim;
   nv_stat_enum_type ret;
#endif
   if (!pMe)
   {
       return; 
   }
   // Clear the index
   for (j = 0; j < NV_MAX_STACK_DIALS; j++) {
      callstack_data.sidx.index[j] = EMPTY_INDEX;
   }

   // Save the index to NV
   OEMPriv_CallHistory_SaveIndex();

#ifdef FEATURE_RUIM_PHONEBOOK
   //Blank out all the ruim entries
   if (IRUIM_IsCardConnected(pMe->m_pRUIM)) {
      for (j = 0, ret = NV_DONE_S; NV_DONE_S == ret; j++) {
         MEMSET((void*)&nvi_ruim, 0, sizeof(nvi_ruim));
         nvi_ruim.stack.address = (byte) j;
         nvi_ruim.stack.status = NVRUIM_STACK_I_RUIMSTATUS;
         ret = OEMNV_Put(NV_STACK_I,
                         &nvi_ruim);
      }
   }
#endif
}


#ifdef FEATURE_RUIM_PHONEBOOK
/*===========================================================================
FUNCTION  OEMPriv_CallHistoryClearMO

DESCRIPTION
  Delete all outgoing calls (MO calls) from the NV call history.

  Note that the call history index is not actaully saved
  to NV.  The caller must to this using OEMPriv_CallHistory_SaveIndex()

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
===========================================================================*/
static void OEMPriv_CallHistoryClearMO(void)
{
   int  j;

   // Clear the index
   for (j = CALLHISTORY_OUTGOING_START_INDEX; j < NV_MAX_STACK_DIALS; j++) {
      callstack_data.sidx.index[j] = EMPTY_INDEX;
   }

} // OEMPriv_CallHistoryClearMO()


/*=============================================================================
FUNCTION:  OEMCallList_MergeRUIM

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
static void OEMCallList_MergeRUIM(ICallList *pMe)
{
  enum {
     INVALID_EXIT_STATUS, /* Default status                 */
     END_OF_RUIM,         /* If the items in the are over   */
     END_OF_NV_ENTRIES,   /* Occupied Entries in the stack are over */
     END_OF_NV_STACK,     /* Total entries that the stack can hold are over */
     NUM_MISMATCH         /* Number in RUIM mismatch with the entry in NV MO */
  }exit_status;
  
  nv_stat_enum_type ret;
  nv_item_type      nvi_ruim;
  nv_stdial_type   *nv_num;
  int               ruim_i;
  int               nv_i;
  int               i;
  int               ruim_entry_count = 0;
  
  if (!pMe)
  {
      return;  
  }
  
  // Make sure the NV call history has been fully loaded first
//  OEM_StartupJob_ForceComplete(OEMJOB_LOAD_NV_CALLHISTORY);
//
//  if (!OEM_StartupJob_IsComplete(OEMJOB_LOAD_NV_CALLHISTORY)) {
//     ·ÀÖ¹ESN=0ÇÒÎ´²å¿¨Ê±³öÏÖÑÏÖØ´íÎó¡£ ERR_FATAL("NV call history not loaded", 0, 0, 0);
//  }

  // Do nothing when no R-UIM is inserted.
  if (!IRUIM_IsCardConnected(pMe->m_pRUIM)) {
     return;
  }

  // KLUDGE:
  //    Do a RUIM read to trick the nvruim code into initializing
  //    itself so we can call nvruim_num_lnd_rec() and get the real value
  MEMSET((void*)&nvi_ruim, 0, sizeof(nvi_ruim));
  nvi_ruim.stack.status = NVRUIM_STACK_I_RUIMSTATUS;
  (void) OEMNV_Get(NV_STACK_I, &nvi_ruim);


  //ruim_entry_count = nvruim_num_lnd_rec();
  if (ruim_entry_count <= 0) {
     return;
  }

  exit_status = INVALID_EXIT_STATUS;
  ruim_i = 0;
  //Start at the point where the outgoing calls index start
  nv_i = CALLHISTORY_OUTGOING_START_INDEX; 

  for (;;) {
     if (ruim_i > ruim_entry_count) {
        //ASSERT_NOT_REACHABLE
        exit_status = END_OF_RUIM;
        break;
     }

     //
     // Read the next RUIM entry
     //
     MEMSET((void*)&nvi_ruim, 0, sizeof(nvi_ruim));
     // the address is 1 based (not zero based)
     nvi_ruim.stack.address = (byte)(ruim_entry_count - (ruim_i+1));
     nvi_ruim.stack.status = NVRUIM_STACK_I_RUIMSTATUS;

     ret = OEMNV_Get(NV_STACK_I, &nvi_ruim);
     if (ret != NV_DONE_S) {
        // reached the end of the R-UIM card
        exit_status = END_OF_RUIM;
        break;
     }

     if ( (nvi_ruim.stack.num_digits == 0) ||
          (nvi_ruim.stack.num_digits >= sizeof(nvi_ruim.stack.digits)) ) {
        // empty or bad R-UIM data, assume we've reached the end of the card
        exit_status = END_OF_RUIM;
        break;
     }

     // find the next MO entry in the NV call history
     for (;;) {
        nv_num = NULL;

        //If the total stack entries that can be held are over
        if (nv_i >= CALLHISTORY_OUTGOING_START_INDEX+MAX_CALLS_IN_CATEGORY){
           exit_status = END_OF_NV_STACK;
           break;
        }
        //if the entries are over, stack has some more spaces
        if(EMPTY_INDEX == callstack_data.sidx.index[nv_i])  {
           exit_status = END_OF_NV_ENTRIES;
           break;
        }

        ASSERT(callstack_data.sidx.index[nv_i] < NV_MAX_STACK_DIALS);
        
        nv_num = &callstack_data.st[callstack_data.sidx.index[nv_i]];
        nv_i++;

        if ( ((int) CALLHISTORY_FROM_RUIM == nv_num->status) ||
             ((int) CALLHISTORY_OUTGOING  == nv_num->status) ||
             ((int) CALLHISTORY_OUTGOING_3WAY  == nv_num->status) ) {
           // found one
           break;
        }
        // not a MO entry, try again
     }

     if (NULL == nv_num) {
        break;
     }

     //
     // Compare the two numbers
     //

     if (nv_num->num_digits != nvi_ruim.stack.num_digits) {
        // different length number
        exit_status = NUM_MISMATCH;
        break;
     }

     if (0 != MEMCMP((char*)nv_num->digits,
                     (char*)nvi_ruim.stack.digits,
                     nvi_ruim.stack.num_digits)) {
        // different digits
        exit_status = NUM_MISMATCH;
        break;
     }

     // Looks good, check the next one
     ruim_i++;
  }

  switch (exit_status) {
  case END_OF_RUIM:
  case END_OF_NV_STACK:
     // This is not an error, continue normally
     break;


  case NUM_MISMATCH:
  case END_OF_NV_ENTRIES:
     //Delete all the NV MO entries
     OEMPriv_CallHistoryClearMO();
     
     //This for loop will copy all the RUIM recent calls log onto the phone nv 
     //items. Note that this items will not have the duration field.
     for (i = 0; ; i++) {
        MEMSET((void*)&nvi_ruim, 0, sizeof(nvi_ruim));
        nvi_ruim.stack.address = (byte) i;
        nvi_ruim.stack.status = NVRUIM_STACK_I_RUIMSTATUS;

        ret = OEMNV_Get(NV_STACK_I, &nvi_ruim);
        if (ret != NV_DONE_S) {
           // reached the end of the R-UIM card
           break;
        }

        if ( (nvi_ruim.stack.num_digits > 0) &&
             (nvi_ruim.stack.num_digits < sizeof(nvi_ruim.stack.digits)) ) {
           //Add this entry in the NV
           OEMPriv_CallHistoryAddEntryEx(pMe,CALLHISTORY_FROM_RUIM,
                                         (byte*)nvi_ruim.stack.letters,
                                         (byte*)nvi_ruim.stack.digits,
                                         nvi_ruim.stack.num_digits,
                                         0);
        }
     }

     // Save the index to NV
     OEMPriv_CallHistory_SaveIndex();
     break;

  default:
    break;
    // ASSERT_NOT_REACHABLE
  }
} // OEM_MCallHistoryMergeRUIM()


#endif /* FEATURE_RUIM_PHONEBOOK */


/*=============================================================================
FUNCTION:  OEMPriv_CallHistory_SaveIndex
 
DESCRIPTION:  Saves the current call history index to NV
 
PARAMETERS:
   None
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMPriv_CallHistory_SaveIndex(void)
{
   // Lint complains that nv_item_type is larger than 
   // callstack_data.sidx.  This is correct, but it 
   // really doesn't matter.
    
   /*lint -save -e826*/
   (void) OEMNV_Put(NV_STACK_IDX_I, 
                    (nv_item_type *) &callstack_data.sidx);
   /*lint -restore*/
}

/*===============================================================================================
FUNCTION  OEMPriv_CallHistoryClearCalls

DESCRIPTION
  Delete all callls from the NV call history.

  Note that the call history index is not actaully saved
  to NV.  The caller must to this using OEMPriv_CallHistory_SaveIndex()

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
===========================================================================*/
static void OEMCallList_ClearCalls(ICallList *pMe, const byte status)
{
   int  j;
    
   if (!pMe)
   {
       return; 
   }
   
   switch(status)
   	{
   	 case CALLHISTORY_MISSED:
           // Clear the miss callsindex
           for (j = CALLHISTORY_MISSED_START_INDEX; j < MAX_CALLS_IN_CATEGORY; j++)
           {
              callstack_data.sidx.index[j] = EMPTY_INDEX;
           }
           break;

        case CALLHISTORY_INCOMING:
           // Clear the index
           for (j = CALLHISTORY_INCOMING_START_INDEX; j < (MAX_CALLS_IN_CATEGORY * 2); j++)
           {
              callstack_data.sidx.index[j] = EMPTY_INDEX;
           }
           break;

        case CALLHISTORY_OUTGOING:
           // Clear the index
           for (j = CALLHISTORY_OUTGOING_START_INDEX; j <( MAX_CALLS_IN_CATEGORY * 3); j++)
           {
              callstack_data.sidx.index[j] = EMPTY_INDEX;
               #ifdef FEATURE_RUIM_PHONEBOOK
               if (IRUIM_IsCardConnected(pMe->m_pRUIM))
               {
                  nv_item_type      nvi_ruim;
                  MEMSET((void*)&nvi_ruim, 0, sizeof(nvi_ruim));
                  nvi_ruim.stack.address = (byte) (j-CALLHISTORY_OUTGOING_START_INDEX);
                  nvi_ruim.stack.status = NVRUIM_STACK_I_RUIMSTATUS;
                  OEMNV_Put(NV_STACK_I,
                            &nvi_ruim);
               }
               #endif
           }
           break;
        default:
           break;
   	}
   OEMPriv_CallHistory_SaveIndex();

}

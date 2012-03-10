/*======================================================
FILE:  OEMCallHistory.c

SERVICES:  OEM Call History interface

GENERAL DESCRIPTION:
   Provides a mechanism to store, update, remove, and view items in
   the device's Call History.

PUBLIC CLASSES AND STATIC FUNCTIONS:
   See AEECallHistory.h for exported functions

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright ?1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/**************************************************************************
**
**   INCLUDE FILES
**
**************************************************************************/
#include "OEMFeatures.h"

#include "AEE_OEM.h"
#include "AEECallHistory.h"
#include "AEEDB.h"
#include "AEE_OEMDispatch.h"
#include "AEEStdLib.h"
#include "OEMDB.h"
#include "AEEWebOpts.h"        // needed to include AEETelephoneTypes.h
#include "AEETelephoneTypes.h" // needed for AEETPI type declaration

#ifdef FEATURE_SYNCED_CALLHISTORY
#include "OEMPBM.h"
#endif

/**************************************************************************
** 
**   COMPILE-TIME MODIFIABLE PARAMETERS
**
**************************************************************************/
// We're CATEGORIZED_CALLHISTORY if we keep separate INCOMING/MISSED/OUTGOING
// lists.
#define CATEGORIZED_CALLHISTORY

//The database name
#define CH_DB_NAME "shared/callhist"

//This is the total maximum number of records of all types in a file
#ifdef CATEGORIZED_CALLHISTORY
#ifdef FEATURE_UIONE_HDK
#define CH_RECS_PER_TYPE           80
#else
#define CH_RECS_PER_TYPE           20 //10
#endif // FEATURE_UIONE_HDK
#else /* CATEGORIZED_CALLHISTORY */
#define CH_RECS_PER_TYPE           20
#endif /* CATEGORIZED_CALLHISTORY */

//Define this if you want to restrict the call entries so that there is never
//a case where you have the same number under the same type.  If a second entry
//has an identical phone number to a previous one, then the second entry is
//deleted before the new entry is added.  This insures there's always n unique
//numbers in a call history.

//Set the maximum number of unicode characters for the name associated
//with a call
#define CH_MAX_NAME  64

//The cache size to use for the database
#define CH_DB_CACHE_SIZE 4096

/*************************************************************************
**
**   NON-MUTABLE MACROS
**
*************************************************************************/
// Maximum size of a phone number, based on the SIM/RUIM standards
#define CH_MAX_PHONE_NUMBER 42

// Special index value indicating db indexes
#define CH_INVALID_DB_INX 0xffff 
#define CH_INVALID_TYPE   0xffff

// Map names that make sense for call history to the databases' set of 
// field names
#define CHFIELD_CLSID_CHANGE  AEEDBFIELD_PREF_VER
#define CHFIELD_CHID          AEEDBFIELD_PREF_ID
#define CHFIELD_DATA          AEEDBFIELD_PREF_DATA
#define CHFIELD_NUMBEREXDATA  AEEDBFIELD_TEXT
#define CHFIELD_CALLTYPE      AEEDBFIELD_CATEGORY

#define PARAM_NOT_REF(x) /*lint -esym(715,x) */

/*************************************************************************
**   IMPLEMENTATION DOCUMENTATION
**************************************************************************
**
** ASSUMPTIONS:
**   Based on the current IDatabase implementation, though it is not
**   not documented as such, I'm assuming that lower recordIDs indicate
**   older (i.e. added earlier) entries, and that enumeration of entries
**   goes from oldest to newest.
**
** DATABASE ARCHITECTURE:
**   Each call history entry is mapped onto a database entry.  Each of 
**   the entries has a format as follows:
**     The record has the concept of a current ClassID.  This defaults
**     to 0, the built-in class.  This current ClassID is changed when
**     a CHFIELD_CLSID_CHANGE field is encountered.  This field contains
**     the new 32-bit classID to become current.  For each call history
**     field we add two fields to the database, CHFIELD_CHID, which 
**     identifies the wID parameter of the AEECallHistoryField (ClsID
**     is implied by the current ClassID), and a CHFIELD_DATA
**     field which contains the data (and the size of that data).
**
**     Exceptions to these are the phone numbers and type fields, as they
**     must be accessed quickly.  Types are named CHFIELD_CALLTYPE,
**     and phone numbers are CHFIELD_NUMBEREXDATA.
** 
*************************************************************************/
/*************************************************************************
**
**   PRIVATE STRUCTURE DEFINITIONS
**
*************************************************************************/
typedef struct _OEMCallHistory OEMCallHistory;
struct _OEMCallHistory 
{
   // Standard stuff
   AEEVTBL(ICallHistory) *pvt;

   // System callback stuff
   AEECallback       m_cbSysObject;
   OEMCallHistory*   m_pNextObj;
   ACONTEXT*         m_pAContext;

   IShell           *m_pShell;
   uint32            m_nRefs;

   // Enumeration stuff
   uint16           *m_pwEnumData;
   int               m_nEnumDataEnts;
   int               m_nEnumRecord;
   AEECallHistoryEntry m_EnumData;

   AEECallback       m_cbOurs;
   AEECallback*      m_pcbTheirs;

   // Database pointer.  Database is always opened while obj instantiated
   IDatabase        *m_pDB;
};

typedef struct _RecXRef {
   uint16   wDBRecId;
   uint16   wCHRecId;
} RecXRef;

/**************************************************************************
**
**  STATIC FUNCTION DECLARATION PROTOTYPES
**
**************************************************************************/

// Interface Functions:
static uint32 OEMCallHistory_AddRef(ICallHistory * po);
static uint32 OEMCallHistory_Release(ICallHistory * po);
static int OEMCallHistory_QueryInterface(ICallHistory *po, AEECLSID id, void **ppo);
static int OEMCallHistory_AddEntry(ICallHistory *pICallHistory, const AEECallHistoryEntry *pCHEnt);
static int OEMCallHistory_EnumInit(ICallHistory *po);
static int OEMCallHistory_EnumInitByCallType(ICallHistory *po, uint16 type);
static const AEECallHistoryEntry* OEMCallHistory_EnumNext(ICallHistory *po, int *pnErr);
static int OEMCallHistory_UpdateEntry(ICallHistory *pICallHistory, const AEECallHistoryEntry *pCHEnt);
static int OEMCallHistory_Clear(ICallHistory *po);
static int OEMCallHistory_ClearEntry(ICallHistory *po);
static int OEMCallHistory_Notify(ICallHistory *po, AEECallback *pcb);
static int OEMCallHistory_GetRecCountByType(ICallHistory *po, uint16 type);
static AECHAR* OEMCallHistory_GetLastRecNumByType(ICallHistory *po, uint16 type);
// Non-Interface Functions:
static void SysObjectCB(void *pUsr);
static void MarkAsUpdated(OEMCallHistory *pme);
static void CancelNotify(AEECallback *pcb);
static void ChangeCallback(void *pUsr);
static void FreeOEMCallHistory(OEMCallHistory *pme, boolean bFreeDB); 
static int ReallocCHEntry(AEECallHistoryEntry *pCHEntry, IDBRecord *pDBRec);
static int CopyIntoCHEntry(AEECallHistoryEntry *pCHEntry, IDBRecord *pDBRec);
static int MakeEntry(const AEECallHistoryEntry *pEntry, AEEDBField **ppDBFields, int *pnDBFields);
static int VerifyFields(AEECallHistoryEntry *pEntry);
static boolean BuildEnumTable(OEMCallHistory *pme);
static boolean BuildEnumByCallTypeTable(OEMCallHistory *pme, uint16 type);
#ifdef CATEGORIZED_CALLHISTORY
static boolean AdvanceToField(IDBRecord *pRec, AEECLSID cls, uint16 wID);
static uint16 RecCallType(IDBRecord *pRec);
static int GetRecordCountByType(OEMCallHistory *pme, uint16 wCallType);
static void DeleteOldestOfType(OEMCallHistory *pme, uint16 wCallType);
static boolean CheckReqFields(const AEECallHistoryEntry *pEntry, uint16 *pdwCallType);
#endif /* CATEGORIZED_CALLHISTORY */
static int ConvNumEXToNum(AECHAR *pwzNumEx, uint16 wzSize, byte *pszNum); 
static int ConvNumToNumEX(AECHAR *pwzDst, char *pSrc, int nLen);

// Item verification functions.  Called via function pointers
static int ItemVerify_CallType(byte *pData, uint16 nDataLen);
static int ItemVerify_NumberType(byte *pData, uint16 nDataLen);
static int ItemVerify_NumberPlan(byte *pData, uint16 nDataLen);
static int ItemVerify_DateTime(byte *pData, uint16 nDataLen); 
static int ItemVerify_CallDuration(byte *pData, uint16 nDataLen);
static int ItemVerify_Number(byte *pData, uint16 nDataLen); 
static int ItemVerify_Name(byte *pData, uint16 nDataLen);
static int ItemVerify_ClsId(byte *pData, uint16 nDataLen);
static int ItemVerify_NumberEx(byte *pData, uint16 nDataLen); 

// Item verification functions for user-defined fields.
static int ItemVerify_PresentationIndicator(byte *pData, uint16 nDataLen); 
static int ItemVerify_Id(byte *pData, uint16 nDataLen);
static int ItemVerify_CallActive(byte *pData, uint16 nDataLen); 
static int ItemVerify_DurationKnown(byte *pData, uint16 nDataLen); 
static int ItemVerify_NotViewed(byte *pData, uint16 nDataLen); 
static int ItemVerify_TechType(byte *pData, uint16 nDataLen);
static int ItemVerify_Counter(byte *pData, uint16 nDataLen);


static int ItemVerify_WStrField(byte *pData, uint16 nDataLen); 
static int ItemVerify_BooleanField(byte *pData, uint16 nDataLen); 

static uint16 DupCHEntryWithoutNumberField(
   const AEECallHistoryEntry *pEntry,
   AEECallHistoryField **ppNewItems
);

/****************************************************************************
**
**   FILE STATIC DATA
**
****************************************************************************/
static VTBL(ICallHistory)  gOEMCallHistoryFuncs = 
{
   OEMCallHistory_AddRef,
   OEMCallHistory_Release,
   OEMCallHistory_QueryInterface,
   OEMCallHistory_AddEntry,
   OEMCallHistory_EnumInit,
   OEMCallHistory_EnumNext,
   OEMCallHistory_UpdateEntry,
   OEMCallHistory_Clear,
   OEMCallHistory_EnumInitByCallType,
   OEMCallHistory_ClearEntry,
   OEMCallHistory_Notify,
   OEMCallHistory_GetRecCountByType,
   OEMCallHistory_GetLastRecNumByType
};

typedef int (*PFNRAWDATA)(byte* pData, uint16 nDataLen);

// Table of functions to verify the different entry types
//static PFNRAWDATA gpVerifyFns[AEECALLHISTORY_FIELD_TECH_TYPE -
//                             AEECALLHISTORY_FIELD_NONE] = 
static PFNRAWDATA gpVerifyFns[AEECALLHISTORY_FIELD_MAX -
                             AEECALLHISTORY_FIELD_NONE] = 
{
   ItemVerify_CallType,
   ItemVerify_NumberType,
   ItemVerify_NumberPlan,
   ItemVerify_DateTime,
   ItemVerify_CallDuration,
   ItemVerify_Number,
   ItemVerify_Name,
   ItemVerify_ClsId,
   ItemVerify_NumberEx,
   ItemVerify_PresentationIndicator,
   ItemVerify_Id,
   ItemVerify_CallActive,
   ItemVerify_DurationKnown,
   ItemVerify_NotViewed,
   ItemVerify_TechType
   ,ItemVerify_Counter
};

static uint16 gwFTNumberEx = AEECALLHISTORY_FIELD_NUMBER_EX;
static uint32 gConstZero = 0;
static OEMCallHistory* gCHObjects = NULL;

/**************************************************************************
**
**  INTERFACE FUNCTION DEFINITIONS
**
**************************************************************************/
void OEMCallHistory_Init(IShell *pShell)
{
   IDBMgr *pDBMgr;
   IDatabase *pDB;
   IDBRecord *pRec = NULL;   
   AEECallHistoryEntry EnumData;
   const AEECallHistoryEntry *pCHEntry = NULL;
   AEECallHistoryField *pItems = NULL;
   AEEDBField *pDBFields;
   int i, nErr, nDBFields;

   MEMSET(&EnumData, 0, sizeof(EnumData));

   // Open the database
   nErr = ISHELL_CreateInstance(pShell, AEECLSID_DBMGR, (void**)&pDBMgr);
   if (nErr != SUCCESS) 
   {
      return;
   }
   IDBMGR_SetCacheSize(pDBMgr, CH_DB_CACHE_SIZE);
   pDB = IDBMGR_OpenDatabase(pDBMgr, CH_DB_NAME, TRUE);
   IDBMGR_Release(pDBMgr);
   
   if (pDB == NULL) 
   {
      return;
   }

   // Mark all calls as inactive
   IDATABASE_Reset(pDB);

   while ((pRec=IDATABASE_GetNextRecord(pDB)) != NULL) {
      boolean foundActiveCall = FALSE;

      // Make a buffer for the CH data; function frees old data
      if (ReallocCHEntry(&EnumData, pRec) == SUCCESS) 
	  {
         // Copy the data into the buffer
         if (CopyIntoCHEntry(&EnumData, pRec) != SUCCESS) 
		 {  
            // Shouldn't happen
            break;
         }
      } 
	  else 
	  {
         break;
      }

      pCHEntry = (const AEECallHistoryEntry *)&EnumData;

      // Check it the call is still active
      for (i = 0, pItems = pCHEntry->pFields; i < pCHEntry->wNumFields; i++, pItems++) 
	  {
         if (pItems->ClsId == 0 && pItems->wID == AEECALLHISTORY_FIELD_CALLACTIVE) 
		 {
            // If the call is still active (phone was probably crashed in 
            // the middle of a call), set it to inactive.
            if (*(boolean *)pItems->pData) 
			{
               *(boolean *)pItems->pData = FALSE;
               foundActiveCall = TRUE;
            }
            break;
         }
      }
      
      if (foundActiveCall) 
	  {
         boolean bNumExFound = FALSE;
         AEECallHistoryEntry newCHEntry;
         // Initialize the newEntry
         ZEROAT(&newCHEntry);
         for (i = 0, pItems = pCHEntry->pFields; i < pCHEntry->wNumFields; i++, pItems++) 
		 {
            if (pItems->ClsId == 0 && pItems->wID == AEECALLHISTORY_FIELD_DURATIONKNOWN) {
               // After setting the active call to inactive,
               // set the call duration to unknown.
               if (*(boolean *)pItems->pData) {
                  *(boolean *)pItems->pData = FALSE;
               }
            }
            if (pItems->ClsId == 0 && pItems->wID == AEECALLHISTORY_FIELD_NUMBER_EX) {
               bNumExFound = TRUE;
            }
         }
         // Update the entry
         if (bNumExFound) {
            // If _NUMBER_EX field exists, we need to remove the _NUMBER field
            // and let the OEMCallHistory handle the _NUMBER field for us
            // to keep the numbe string stored in the _NUMBER_EX field intact. 
            
           
            newCHEntry.wNumFields = 
               DupCHEntryWithoutNumberField(pCHEntry, &newCHEntry.pFields);

            nErr = MakeEntry((AEECallHistoryEntry*) &newCHEntry, 
                           &pDBFields, 
                           &nDBFields);
         } else {
            nErr = MakeEntry((AEECallHistoryEntry*)pCHEntry, 
                           &pDBFields, 
                           &nDBFields);
         }                  
         if (nErr == SUCCESS) {
            (void)IDBRECORD_Update(pRec, pDBFields, nDBFields);
         }
         FREEIF(newCHEntry.pFields);
         FREEIF(pDBFields);
      }
      IDBRECORD_Release(pRec);
      pRec = NULL;
   }

   if (pRec != NULL) {
      IDBRECORD_Release(pRec);
      pRec = NULL;
   }

   if (pCHEntry && pCHEntry->pFields) {
      FREE(pCHEntry->pFields[0].pData); //data holder
      FREE(pCHEntry->pFields);   
   }

   (void)IDATABASE_Release(pDB);
}


int OEMCallHistory_New(IShell *pIShell, AEECLSID ClsId, void **ppObj)
{
   OEMCallHistory *pNew;
   int nErr;
   IDBMgr *pDBMgr;

   if (ClsId == AEECLSID_CALLHISTORY || ClsId == AEECLSID_CALLHISTORY_30) {
      // Set ppobj to null so we can return
      *ppObj = NULL;

      // Make room
      /*lint -e740 */
      pNew = (OEMCallHistory*)AEE_NewClass((IBaseVtbl*)&gOEMCallHistoryFuncs,
                                           sizeof(OEMCallHistory));
      /*lint +e740 */
      if (!pNew)
         return ENOMEMORY;

      // Initialize
      pNew->m_pShell = pIShell;
      ISHELL_AddRef(pIShell);
      CALLBACK_Init(&pNew->m_cbOurs, ChangeCallback, (void*)pNew);
      pNew->m_nRefs = 1;

      // Open the database
      nErr = ISHELL_CreateInstance(pIShell, AEECLSID_DBMGR, (void**)&pDBMgr);
      if (nErr != SUCCESS) {
         FREE(pNew);
         return nErr;
      }
      IDBMGR_SetCacheSize(pDBMgr, CH_DB_CACHE_SIZE);
      pNew->m_pDB = IDBMGR_OpenDatabase(pDBMgr, CH_DB_NAME, TRUE);
      IDBMGR_Release(pDBMgr);
      if (pNew->m_pDB == NULL) {
         FREE(pNew);
         return ENOMEMORY;
      }

      // Link into the object
      CALLBACK_Init(&pNew->m_cbSysObject, SysObjectCB, (void*)pNew);
      AEE_LinkSysObject(&pNew->m_cbSysObject);
      pNew->m_pNextObj = gCHObjects;
      gCHObjects = pNew;
      pNew->m_pAContext = AEE_GetAppContext();

      // Return
      *ppObj = (void*)pNew;
      return 0;
   }
   *ppObj = NULL;
   return EUNSUPPORTED;   
}

static uint32 OEMCallHistory_AddRef(ICallHistory * po) 
{
   /*lint -e740 */
   OEMCallHistory *pMe = (OEMCallHistory*)po;
   /*lint +e740 */

   return(++pMe->m_nRefs);
}

static uint32 OEMCallHistory_Release(ICallHistory * po) 
{
   uint32 nRefs;
   /*lint -e740 */
   OEMCallHistory *pMe = (OEMCallHistory*)po;
   /*lint +e740 */

   nRefs = --pMe->m_nRefs;
   if (nRefs == 0) {
      FreeOEMCallHistory(pMe, TRUE);
   }

   return (nRefs);
}

static int OEMCallHistory_QueryInterface(ICallHistory *po, AEECLSID id, 
                                         void **ppo)
{
   switch(id) {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_CALLHISTORY_30:
      case AEECLSID_CALLHISTORY:
         *ppo = (void*)po;
         (void) IQI_AddRef(*ppo);
         return SUCCESS;

      default:
         break;
        
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
}

/*======================================================================
Function: OEMCallHistory_AddEntry()

Description: 
   This function adds a new entry to the appropriate call history
   phonebook. 

Prototype:

   int OEMCallHistory_AddEntry(ICallHistory *pICallHistory,
                             const AEECallHistoryEntry *pEntry)

Parameters:

   pICallHistory:      [in]: Pointer to an ICallHistory interface object
   pEntry:             [in]: new entry

Return Value:

   SUCCESS if successful
   EFAILED or another BREW error if an error occurs (ENOMEMORY, etc.)

Comments:  
   The memory specified in pEntry is copied by OEMCallHistory_AddEntry,
   and need not be maintained by the caller after the call to AddEntry()

Side Effects: 
   None

See Also: 
   None
   
=======================================================================*/
static int OEMCallHistory_AddEntry(ICallHistory *pICallHistory, 
                                   const AEECallHistoryEntry *pEntry)
{
   OEMCallHistory *pme = (OEMCallHistory*)pICallHistory;
   IDBRecord *pRec;
   AEEDBField *pDBFields;
   int nDBFields;
   int nErr;
#ifdef CATEGORIZED_CALLHISTORY
   uint16 wCallType;
#else /* CATEGORIZED_CALLHISTORY */
   int nRecCount;
#endif /* CATEGORIZED_CALLHISTORY */

   // See if we have a valid storage type
   if (pme->m_pDB == NULL)
      return EFAILED;

   // Make sure they sent us valid data
   if (pEntry->wNumFields == 0)
      return EBADPARM;

   // Make sure the field values make sense
   nErr = VerifyFields((AEECallHistoryEntry*)pEntry);
   if (nErr != SUCCESS && nErr != EUNSUPPORTED)
      return nErr;

#ifdef CATEGORIZED_CALLHISTORY
   // Check for required fields
   if (!CheckReqFields(pEntry, &wCallType))
      return EFAILED;
#endif /* CATEGORIZED_CALLHISTORY */

   // Make a memory block with all of the records in it
   nErr = MakeEntry((AEECallHistoryEntry*)pEntry, &pDBFields, &nDBFields);
   if (nErr != SUCCESS)
      return nErr;

   // Delete the last record if we already are full on records.  Keep in
   // mind our database has a special record that indicates the next index
   // index.

#ifdef CATEGORIZED_CALLHISTORY
   if (GetRecordCountByType(pme, wCallType) >= CH_RECS_PER_TYPE) {
      DeleteOldestOfType(pme, wCallType);
   }
#else
   IDATABASE_Reset(pme->m_pDB);
   nRecCount = IDATABASE_GetRecordCount(pme->m_pDB);
   if (nRecCount >= (CH_RECS_PER_TYPE)) {
      // Since the database is ordered, its the first entry returned
      pRec = IDATABASE_GetNextRecord(pme->m_pDB);
      if (pRec == NULL)
         return EFAILED;
      IDBRECORD_Remove(pRec);
   }
#endif

   // Add this record to our tables and mark it to be read
   pRec = IDATABASE_CreateRecord(pme->m_pDB, pDBFields, nDBFields);
   if (pRec == NULL) 
   {
      nErr = EFAILED;
   } 
   else 
   {
      IDBRECORD_Release(pRec);
      nErr = SUCCESS;
   }

   // Notify anyone who wants to know about changes
   MarkAsUpdated(pme);

   // Free all of the memory used
   FREE(pDBFields);

   // If the OEM is keeping their Call History in parallel, update that
#ifdef FEATURE_SYNCED_CALLHISTORY
   if (nErr == SUCCESS)
      OEMPBM_AddEntry(pEntry);
#endif /* FEATURE_SYNCED_CALLHISTORY */
   return nErr;
}

/*======================================================================= 
Function: OEMCallHistory_EnumInit()

Description: 
  This function initializes/resets the enumeration in the Call History list.

Prototype:

   int OEMCallHistory_EnumInit(ICallHistory *po)
                                               
Parameters:

   po: [in]: Pointer to an ICallHistory interface object

  
Return Value:

   SUCCESS if successful
   EFAILED or another BREW error if an error occurs (ENOMEMORY, etc.)

Comments:
   None

Side Effects: 
   None

See Also: 
   OEMCallHistory_EnumNext()


=======================================================================*/
static int OEMCallHistory_EnumInit(ICallHistory *po)
{
   /*lint -e740 */
   OEMCallHistory *pMe = (OEMCallHistory*)po;
   /*lint +e740 */

   pMe->m_nEnumRecord = 0;

   // Get a buffer
   if (!BuildEnumTable(pMe))
      return EFAILED;

   return SUCCESS;
}

/*======================================================================= 
Function: static int OEMCallHistory_EnumInitByCallType()

Description: 
  This function initializes/resets the enumeration in the Call History list.

Prototype:

   static int OEMCallHistory_EnumInitByCallType(ICallHistory *po, uint16 type)
                                               
Parameters:

   po: [in]: Pointer to an ICallHistory interface object

  
Return Value:

   SUCCESS if successful
   EFAILED or another BREW error if an error occurs (ENOMEMORY, etc.)

Comments:
   None

Side Effects: 
   None

See Also: 
   OEMCallHistory_EnumNext()


=======================================================================*/
static int OEMCallHistory_EnumInitByCallType(ICallHistory *po, uint16 type)
{
   /*lint -e740 */
   OEMCallHistory *pMe = (OEMCallHistory*)po;
   /*lint +e740 */

   pMe->m_nEnumRecord = 0;
  
   // Get a buffer
   if (!BuildEnumByCallTypeTable(pMe, type))
   {
		return EFAILED;
   }
   return SUCCESS;
}

/*======================================================================= 
Function: OEMCallHistory_EnumNext()

Description: 
   This function retrieves the next entry in the Call History list from
   PBM. OEMCallHistory_EnumInit() must be called before
   any successive calls to this function. 

Prototype:

   const AEECallHistoryStatus OEMCallHistory_EnumNext(ICallHistory *po,
                                                    int *pErr);

Parameters:
   po: [in]:        Pointer to an ICallHistory interface object
   int *pErr: [out]: Pointer to an integer to hold any error value, set
                        to: SUCCESS if successful or at end of enumeration
                            EFAILED or another BREW error if an error occurs

Return Value:

   the "next" callhistory entry, if applicable
   NULL if we're at the last entry or an error occurs
  
Comments:  
   The memory pointed to by the returned AEECallHistoryEntry is not owned by
   the user.  Its contents will stay valid until the next call
   to OEMCallHistory_EnumNext(), OEMCallHistory_EnumInit() or 
   OEMCallHistory_Release().  The contents of the returned pointer will not
   be modified by the caller.

Side Effects: 
   None

See Also: 
   None

=======================================================================*/
static const AEECallHistoryEntry* OEMCallHistory_EnumNext(ICallHistory *po, 
                                                          int *pnErr)
{
   /*lint -e740 */
   OEMCallHistory *pMe = (OEMCallHistory*)po;
   /*lint +e740 */
   IDBRecord *pRec;

   // If the enumeration table isn't there, then they probably
   // did EnumInit on an empty callhistory.  Return SUCCESS.
   if (pMe->m_pwEnumData == NULL) 
   {
      *pnErr = SUCCESS;
      return NULL;
   }

   // First find a non-0xFFFF index
   while (pMe->m_nEnumRecord < pMe->m_nEnumDataEnts) 
   {
      if (pMe->m_pwEnumData[pMe->m_nEnumRecord] != CH_INVALID_DB_INX)
      {
      	break; 
      }

      // That was blank (record deleted after EnumInit) go on to the next
      pMe->m_nEnumRecord++;
   }

   // If we've reached the end of the enumeration, tell the user that
   if (pMe->m_nEnumRecord >= pMe->m_nEnumDataEnts) 
   {
      *pnErr = SUCCESS;
      return NULL;
   }

   // Open that record
   pRec = IDATABASE_GetRecordByID(pMe->m_pDB, pMe->m_pwEnumData[pMe->m_nEnumRecord]);
   
   if (pRec == NULL) 
   {
      // For whatever reason we can't open that record
      *pnErr = EFAILED;
      return NULL;
   }

   // Make a buffer for the returned data
   *pnErr = SUCCESS;
   if (ReallocCHEntry(&pMe->m_EnumData, pRec) == SUCCESS) 
   {
      // Copy the data into the buffer
      if (CopyIntoCHEntry(&pMe->m_EnumData, pRec) == SUCCESS) 
	  {  
         // Inrement the iterator
         pMe->m_nEnumRecord++;
      } 
	  else 
	  {
         // Shouldn't happen
         *pnErr = EFAILED;
      }
   } 
   else 
   {
      *pnErr = ENOMEMORY;
   }

   // Close the record
   IDBRECORD_Release(pRec);

   // Check for errors
   if (*pnErr != SUCCESS) 
   {
      return NULL;
   }

   // Return a pointer to the data
   return ((const AEECallHistoryEntry*)&pMe->m_EnumData);
}

/*======================================================================= 
Function: OEMCallHistory_UpdateEntry()

Description: 
   This function replaces the current call history entry with the one 
   provided.  The current entry is defined as the entry that was returned 
   during the most recent call to OEMCallHistory_EnumNext().

Prototype:

   int OEMCallHistory_UpdateEntry(ICallHistory *po, 
                                const AEECallHistoryEntry *pEntry);
   
Parameters:
 
    po: [in]:  Pointer to an ICallHistory interface object
    pEntry: [in]:  New data to replace existing entry

Return Value:

   SUCCESS if successful
   EFAILED or some other BREW error if failed to update entries

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=======================================================================*/
static int OEMCallHistory_UpdateEntry(ICallHistory *po, 
                                      const AEECallHistoryEntry *pEntry)
{
   OEMCallHistory *pme = (OEMCallHistory*)po;
   IDBRecord *pRec;
   int nEnumInx;
   AEEDBField *pDBFields;
   int nDBFields;
   int nErr;
#ifdef CATEGORIZED_CALLHISTORY
   uint16 wCallTypeNew;
   uint16 wCallTypeOld;
#endif /* CATEGORIZED_CALLHISTORY */

   // See if we have a valid storage type
   if (pme->m_pDB == NULL)
      return EFAILED;

   // Make sure they sent us valid data
   nErr = VerifyFields((AEECallHistoryEntry*)pEntry);
   if (nErr != SUCCESS && nErr != EUNSUPPORTED)
      return nErr;

   // Make sure we have enum data
   if (pme->m_pwEnumData == NULL)
      return EFAILED;

   // Get the db index.  It's probably m_nEnumRecord - 1, but sometimes positions get
   // invalidated (marked by a 0xFFFF check value).  Keep going backwards until we
   // find a non-invalidated entry.
   for (nEnumInx = pme->m_nEnumRecord - 1; nEnumInx >= 0; nEnumInx--) {
      if (pme->m_pwEnumData[nEnumInx] != CH_INVALID_DB_INX)
         break;      
   }
   if (nEnumInx < 0)
      return EFAILED;

   // Check for required fields
#ifdef CATEGORIZED_CALLHISTORY
   if (!CheckReqFields(pEntry, &wCallTypeNew))
      return EFAILED;
#endif

   // Use the enumerator inx to get the db inx to get the record handle
   pRec = IDATABASE_GetRecordByID(pme->m_pDB, pme->m_pwEnumData[nEnumInx]);
   if (pRec == NULL)
      return EFAILED;

#ifdef CATEGORIZED_CALLHISTORY
   // See if we need to delete one of the new type to make room
   wCallTypeOld = RecCallType(pRec);
   if (wCallTypeNew != wCallTypeOld && 
      GetRecordCountByType(pme, wCallTypeNew) >= CH_RECS_PER_TYPE)
   {
      DeleteOldestOfType(pme, wCallTypeNew);
   }
#endif

   nErr = MakeEntry((AEECallHistoryEntry*)pEntry, &pDBFields, &nDBFields);
   if (nErr != SUCCESS) {
      IDBRECORD_Release(pRec);
      return nErr;
   }

   // Call the database's update function
   nErr = IDBRECORD_Update(pRec, pDBFields, nDBFields);

   // Notify people who care that the CallHistory has been updated
   if (nErr == SUCCESS)
      MarkAsUpdated(pme);

   // Clean up
   IDBRECORD_Release(pRec);
   FREE(pDBFields);

   return nErr;
}

/*======================================================================= 

Function: OEMCallHistory_Clear()

Description: 
   This function deletes all entries from the Call History phonebooks in
   PBM

Prototype:

   int OEMCallHistory_Clear(ICallHistory *po)
   
Parameters:
 
    po: [in]:  Pointer to an ICallHistory interface object

Return Value:

   SUCCESS if successful
   EFAILED

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=======================================================================*/
static int OEMCallHistory_Clear(ICallHistory *po) 
{
   OEMCallHistory *pme = (OEMCallHistory*)po;
   IDBMgr *pDBMgr;
   int nErr;
   int nDBRemoveErr;
   IDBRecord *pRec;
   int i;
   AEECallHistoryField *pItems = NULL;
   const AEECallHistoryEntry *pCHEntry = NULL;
   boolean callsAreActive = FALSE;

   // If any call is active, the database can't be deleted
   IDATABASE_Reset(pme->m_pDB);
   while ((pRec=IDATABASE_GetNextRecord(pme->m_pDB)) != NULL) {
      // Make a buffer for the CH data
      if (ReallocCHEntry(&pme->m_EnumData, pRec) == SUCCESS) {
         // Copy the data into the buffer
         if (CopyIntoCHEntry(&pme->m_EnumData, pRec) != SUCCESS) {  
            // Shouldn't happen
            break;
         }
      } else {
         break;
      }

      pCHEntry = (const AEECallHistoryEntry *)&pme->m_EnumData;
      // Check if the call is active.
      for (i = 0, pItems = pCHEntry->pFields; i < pCHEntry->wNumFields; 
           i++, pItems++) {
         if (pItems->ClsId == 0 && pItems->wID == AEECALLHISTORY_FIELD_CALLACTIVE) {
            if (*(boolean *)pItems->pData) {
               callsAreActive = TRUE;
               IDBRECORD_Release(pRec);
            } else {
               IDBRECORD_Remove(pRec); //also releases pRec
            }
            pRec = NULL;
            break;
         }
      }

      // Didn't find AEECALLHISTORY_FIELD_CALLACTIVE field, remove the 
      // entry anyway!
      if (pRec != NULL) {
         IDBRECORD_Remove(pRec); //also releases pRec
         pRec = NULL;
      }

   }
   
   if (pRec != NULL) {
      IDBRECORD_Release(pRec);
      pRec = NULL;
   }

   // Invalidiate and free enumeration data
   if (pme->m_pwEnumData != NULL)
   {
      FREE(pme->m_pwEnumData);
      pme->m_pwEnumData = NULL;
      pme->m_nEnumDataEnts = 0;
   }

   if (callsAreActive) {
      MarkAsUpdated(pme);
      return EITEMBUSY;
   }
 
   // Make a new database manager
   nErr = ISHELL_CreateInstance(pme->m_pShell, AEECLSID_DBMGR, (void**)&pDBMgr);
   if (nErr != SUCCESS)
      return nErr;   

   // Close the current database.
   if (pme->m_pDB != NULL)
      IDATABASE_Release(pme->m_pDB);
   pme->m_pDB = NULL;

   // Delete it.  It may not be deleted if someone else has the database open.
   // If this is the case, we'll remove the entries below.
   nDBRemoveErr = IDBMGR_Remove(pDBMgr, CH_DB_NAME);
   if (nDBRemoveErr == SUCCESS)
      MarkAsUpdated(pme);

   // Make a new one by opening it
   pme->m_pDB = IDBMGR_OpenDatabase(pDBMgr, CH_DB_NAME, TRUE);
   IDBMGR_Release(pDBMgr);

   // Return based on the result of the open operation
   if (pme->m_pDB == NULL)
      return EFAILED;

   // Remove all of the entries.  In most cases, this is a new database and will
   // be a non-issue.  But sometimes we can't delete the database, and this
   // is the only way.
   IDATABASE_Reset(pme->m_pDB);
   while ((pRec=IDATABASE_GetNextRecord(pme->m_pDB)) != NULL) {
      IDBRECORD_Remove(pRec); //also releases pRec
   }

   // Sanity check that all records are gone
   if (IDATABASE_GetRecordCount(pme->m_pDB) != 0)
      return EFAILED; 

   // Let the user know things have changed
   MarkAsUpdated(pme);

#ifdef FEATURE_SYNCED_CALLHISTORY
   if (nErr == SUCCESS)
      OEMPBM_Clear();
#endif /* FEATURE_SYNCED_CALLHISTORY */

   return SUCCESS;
}

/*======================================================================= 

Function: OEMCallHistory_ClearEntry()

Description: 
   This function clear a particular entry from the Call History phonebooks in
   PBM

Prototype:

   int OEMCallHistory_Clear(ICallHistory *po)
   
Parameters:
 
    po: [in]:  Pointer to an ICallHistory interface object

Return Value:

   SUCCESS if successful
   EFAILED

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=======================================================================*/
static int OEMCallHistory_ClearEntry(ICallHistory *po) 
{
   OEMCallHistory *pme = (OEMCallHistory*)po;
   IDBRecord *pRec;
   int nEnumInx;

   // Make sure an enumeration has started
   if (pme->m_pwEnumData == NULL)
   {
      return EFAILED;
   }

   // Get the db index.  It's probably m_nEnumRecord - 1, but sometimes positions get
   // invalidated (marked by a 0xFFFF check value).  Keep going backwards until we
   // find a non-invalidated entry.
   for (nEnumInx = pme->m_nEnumRecord - 1; nEnumInx >= 0; nEnumInx--) {
      if (pme->m_pwEnumData[nEnumInx] != CH_INVALID_DB_INX)
         break;      
   }
   if (nEnumInx < 0)
      return EFAILED;

   // Get the record handle
   pRec = IDATABASE_GetRecordByID(pme->m_pDB,
                                  pme->m_pwEnumData[nEnumInx]);
   if (pRec == NULL)
      return EFAILED;
   
   do {
      int i;
      AEECallHistoryField *pItems = NULL;
      const AEECallHistoryEntry *pCHEntry = NULL;

      // Make a buffer for the CH data
      if (ReallocCHEntry(&pme->m_EnumData, pRec) == SUCCESS) {
         // Copy the data into the buffer
         if (CopyIntoCHEntry(&pme->m_EnumData, pRec) != SUCCESS) {  
            // Shouldn't happen
            break;
         }
      } else {
         break;
      }

      // Check if the call is active 
      pCHEntry = (const AEECallHistoryEntry *)&pme->m_EnumData;
      for (i = 0, pItems=pCHEntry->pFields; i < pCHEntry->wNumFields; 
           i++, pItems++){
         if (pItems->ClsId == 0 && pItems->wID == AEECALLHISTORY_FIELD_CALLACTIVE) {
            if (*(boolean *)pItems->pData) {
               // Close the record
               IDBRECORD_Release(pRec);
               pRec = NULL;
               return EITEMBUSY;
            }
            break;
         }
      }
   } while (0);
   
   // Mark record as removed in enum list
   pme->m_pwEnumData[nEnumInx] = CH_INVALID_DB_INX;

   // Mark the call history as updated.
   MarkAsUpdated(pme);
   
   // Remove record and return
   return IDBRECORD_Remove(pRec);
}

/*======================================================================= 

Function: OEMCallHistory_Notify

Description: 
   This function deletes all entries from the Call History phonebooks in
   PBM

Prototype:

   int static int OEMCallHistory_Notify(ICallHistory *po, AEECallback *pcb)
   
Parameters:
 
    po: [in]:  Pointer to an ICallHistory interface object
    pcb: [in]: Pointer to an AEECallback 

Return Value:

   SUCCESS if successful
   EFAILED

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=======================================================================*/
static int OEMCallHistory_Notify(ICallHistory *po, AEECallback *pcb)
{
   OEMCallHistory *pme = (OEMCallHistory*)po;

   // Check params
   if (pcb == NULL)
      return EFAILED;

   // Set up cancel data
   CALLBACK_Cancel(pcb);
   pcb->pfnCancel = CancelNotify;
   pcb->pCancelData = (void*)pme;

   // Prepend to list
   pcb->pNext = pme->m_pcbTheirs;
   pme->m_pcbTheirs = pcb;

   // Return SUCCESS
   return SUCCESS;
}

/*======================================================================= 

Function: OEMCallHistory_GetRecCountByType

Description: 
   This function returns the record count of a certain type.

Prototype:
   
Parameters:
 
    po: [in]:  Pointer to an ICallHistory interface object
    type: [in]: call type

Return Value:
    int: record count.

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=======================================================================*/
static int OEMCallHistory_GetRecCountByType(ICallHistory *po, uint16 type)
{
   OEMCallHistory *pme = (OEMCallHistory*)po;
   int nCount = 0;
   
   if(type == AEECALLHISTORY_CALL_TYPE_TO ||
        type == AEECALLHISTORY_CALL_TYPE_FROM ||
        type == AEECALLHISTORY_CALL_TYPE_MISSED)
   {/*get the count of a certain type*/
        nCount = GetRecordCountByType(pme, type); 
   }
   else /*AEECALLHISTORY_CALL_TYPE_ALL*/
   {/*get the total count of all type*/
        nCount += GetRecordCountByType(pme, AEECALLHISTORY_CALL_TYPE_TO);
        nCount += GetRecordCountByType(pme, AEECALLHISTORY_CALL_TYPE_FROM);
        nCount += GetRecordCountByType(pme, AEECALLHISTORY_CALL_TYPE_MISSED);
   }
   
   return nCount;
}

/*======================================================================= 

Function: OEMCallHistory_GetLastRecNumByType

Description: 
   This function returns the last record number of a certain type.

Prototype:
   
Parameters:
 
    po: [in]:  Pointer to an ICallHistory interface object
    type: [in]: call type

Return Value:
    AECHAR: pointer of the last record number.

Comments:  
   The return pointer need to free after using.

Side Effects: 
   None

See Also: 
   None

=======================================================================*/
static AECHAR* OEMCallHistory_GetLastRecNumByType(ICallHistory *po, uint16 type)
{
    AEECallHistoryEntry *pEntry = NULL;
    AECHAR wstrBuffer[AEECALLHISTORY_MAXDIGITS];
    AECHAR *pwstrNumber = NULL;
    int nFieldNum = 0, nRet = 0;
    boolean bRecordExisted = FALSE;
   
    switch(type)
    {
        case AEECALLHISTORY_CALL_TYPE_TO:
        case AEECALLHISTORY_CALL_TYPE_FROM:
        case AEECALLHISTORY_CALL_TYPE_MISSED:
        {
            if(OEMCallHistory_EnumInitByCallType(po, type) != SUCCESS)
            {
                return NULL;
            }
        }
        break;

        case AEECALLHISTORY_CALL_TYPE_ALL:
        {
            if(OEMCallHistory_EnumInit(po) != SUCCESS)
            {
                return NULL;
            }
        }
        break;

        default:
            return NULL;
    }
   
    pEntry = (AEECallHistoryEntry *)OEMCallHistory_EnumNext(po, &nRet);
    while (NULL != pEntry)
    {
        for (nFieldNum=0; nFieldNum<pEntry->wNumFields; nFieldNum++)
        {
            if(pEntry->pFields[nFieldNum].wID == AEECALLHISTORY_FIELD_NUMBER)
            {
                if(pEntry->pFields[nFieldNum].pData != NULL)
                {
                    MEMSET(wstrBuffer, 0, sizeof(wstrBuffer));
                    (void) STRTOWSTR((char*)pEntry->pFields[nFieldNum].pData, wstrBuffer, sizeof(wstrBuffer));
                    pwstrNumber = WSTRDUP(wstrBuffer);
                    bRecordExisted = TRUE;
                    break;
                }
            }
        }
        if (bRecordExisted)
        {
            break;
        }
        pEntry = (AEECallHistoryEntry *)OEMCallHistory_EnumNext(po, &nRet);
    }
    return (AECHAR*)pwstrNumber;
}
/**********************************************************************
**
** NON-INTERFACE/HELPER FUNCTION DEFINITIONS
**
**********************************************************************/
static void SysObjectCB(void *pUsr)
{
   FreeOEMCallHistory((OEMCallHistory*)pUsr, FALSE);
}

/*==================================================================
Function: MarkAsUpdated

Description:
   Send notices to all that are interested that the call history has been
   updated.

Prototype:
   static void MarkAsUpdated(OEMCallHistory *pme)

Parameter(s):  
   pme - pointer to OEMCallHistory object associated with CH database

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None
==================================================================*/
static void MarkAsUpdated(OEMCallHistory *pme)
{
   OEMCallHistory *pCHCurr;
   ACONTEXT *pCtxOld;

   // Go through all of the call history objects instantiated on BREW and
   // call their "Notify Notify" functions (i.e. Our notify which in turn 
   // will call multiple of their notifies
   for (pCHCurr = gCHObjects; pCHCurr != NULL; pCHCurr = pCHCurr->m_pNextObj)
   {
      if (pCHCurr->m_pcbTheirs)
      {
         pCtxOld = AEE_EnterAppContext(pCHCurr->m_pAContext);
         ISHELL_Resume(pCHCurr->m_pShell, &pCHCurr->m_cbOurs);
         AEE_LeaveAppContext(pCtxOld);
      }
   }
}

/*==================================================================
Function: CancelNotify

Description:
   Cancel the notification request registered

Prototype:
   static void CancelNotify(AEECallback *pcb)

Parameter(s):  
   pcb - Callback structure to be deregistered

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None
==================================================================*/
static void CancelNotify(AEECallback *pcb)
{
   OEMCallHistory *pme = (OEMCallHistory*)pcb->pCancelData;
   AEECallback **ppcb;

   if (pme == NULL)
      return;

   // Find the pointer 
   for (ppcb = &pme->m_pcbTheirs; *ppcb != NULL; ppcb = &((*ppcb)->pNext))
      if (*ppcb == pcb)
         break;
   if (*ppcb == NULL)
      return;

   // Unlink it
   *ppcb = pcb->pNext;

   // Remove cancel stuff
   pcb->pfnCancel = NULL;
   pcb->pCancelData = NULL;
}

/*==================================================================
Function: ChangeCallback

Description:
   Called when the Call history is changed.  Calls all of the user callbacks
   directly.

Prototype:
   static void ChangeCallback(void *pUsr)

Parameter(s):  
   pUsr - The callhistory object cast to a void*

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None
==================================================================*/
static void ChangeCallback(void *pUsr)
{
   OEMCallHistory *pme = (OEMCallHistory*)pUsr;
   AEECallback *pCurr;

   for (pCurr = pme->m_pcbTheirs; pCurr != NULL; pCurr = pCurr->pNext)
   {
      // Call directly.  Do not use ISHELL_Resume, because the callbacks
      // should remain persistent until the user calls cancel.
      pCurr->pfnNotify(pCurr->pNotifyData);
   }
}

/*==================================================================
Function: FreeOEMCallHistory

Description:
   Frees resources associated with an OEMCallHistory object, as well as
   FREE()'ing the object itself.

Prototype:
   static void FreeOEMCallHistory(OEMCallHistory *pme, boolean bFreeDB);

Parameter(s):  
   pme - pointer to object being FREE'd
   bFreeDB - boolean of wheter or now we should free the database.  If
     BREW is calling us to clean up, the database may already have been
     deallocated, so we shouldn't _Release it, otherwise we may access
     invalid pointers/data.

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None
==================================================================*/
static void FreeOEMCallHistory(OEMCallHistory *pme, boolean bFreeDB) 
{
   OEMCallHistory **ppList;
   
   CALLBACK_Cancel(&pme->m_cbOurs);

   // Unlink from the global list
   for (ppList = &gCHObjects; *ppList; ppList = &((*ppList)->m_pNextObj))
      if (*ppList == pme)
         break;
   if (*ppList != NULL)
      *ppList = (*ppList)->m_pNextObj;

   // Unlink from the system object list
   CALLBACK_Cancel(&pme->m_cbSysObject);

   // If we got called back via the system object callback, don't 
   // free the database because that pointer may be valid.
   if (bFreeDB)
      IDATABASE_Release(pme->m_pDB);

   // Free the local data
   ISHELL_Release(pme->m_pShell);
   FREE(pme->m_pwEnumData);
   if (pme->m_EnumData.pFields) {
      FREE(pme->m_EnumData.pFields[0].pData);
      FREE(pme->m_EnumData.pFields);
   }
   FREE(pme);
}

/*==================================================================
Function: ReallocCHEntry

Description:
   Given a pointer to a CH Entry, reallocate memory to store data from
   the given database record in it.

Prototype:
   static int CopyIntoCHEntry(AEECallHistoryEntry *pCHEntry, 
                              IDBRecord *pDBRec);

Parameter(s):  
   pCHEntry - Pointer to CH entry to allocate memory for.
   pDBRec - Pointer to a database record to size allocation against

Return Value:
   SUCCESS if memory could be allocated
   ENOMEMORY if the phone does not have enough memory
   EFAILED for other errrors

Comments:
   Data for payload is allocated at pointed to by the pData member of the
   first field in the CH entry.

Side Effects:
See Also:
   CopyIntoCHEntry
==================================================================*/
static int ReallocCHEntry(AEECallHistoryEntry *pCHEntry, IDBRecord *pDBRec)
{
   uint16 wDataSize;
   int nFields;
   AEEDBFieldName fn;
   AEEDBFieldType ft;
   uint16 wLen;
   uint16 wNumExSize = 0;
   uint16 wCHType = AEECALLHISTORY_FIELD_NONE;
   AECHAR *pwzNumEX = NULL;


   // First, deallocate any memory left over from last time
   if (pCHEntry->pFields)
      FREE(pCHEntry->pFields[0].pData); //data holder
   FREE(pCHEntry->pFields);   
   pCHEntry->pFields = NULL;

   nFields = 0;
   wDataSize = 0;
   IDBRECORD_Reset(pDBRec);
   while ((ft = IDBRECORD_NextField(pDBRec, &fn, &wLen)) != AEEDB_FT_NONE) {
      switch (fn) {
         // Ignore type and change-of-classid markers
         case CHFIELD_CLSID_CHANGE:
            break;

        case CHFIELD_CHID:
           // Remember what field type this is
           IDBRECORD_GetFieldWord(pDBRec, &wCHType);
           break;
            
         // We care about data fields
         case CHFIELD_DATA:
         case CHFIELD_NUMBEREXDATA:
         case CHFIELD_CALLTYPE:
            // Increment the field count and total size.  Keep in mind that
            // we word align everything
            nFields++;
            wDataSize += wLen;
            
            if (wCHType == AEECALLHISTORY_FIELD_NAME)
            {
               // A terminating NULL will be added so make room
               wDataSize += 2;
            }
            else if (wCHType == AEECALLHISTORY_FIELD_NUMBER_EX)
            {
               // A terminating NULL will be added so make room
               wDataSize += 2;
               // assume only one number or numberex field
               wNumExSize = wLen;
               pwzNumEX = (AECHAR*)IDBRECORD_GetField(pDBRec, &fn, &ft, &wLen);
            }else if(wCHType == AEECALLHISTORY_FIELD_NUMBER){
               // A terminating NULL will be added so make room
               wDataSize++;
            }else if (wCHType == AEECALLHISTORY_FIELD_ID) {
              // A terminating NULL will be added so make room
               wDataSize += 2;
            }
            
            wDataSize = ((wDataSize + 3) & (~3));
            break;

         // Anything else is invalid
         default:
            return EFAILED;
      }
   }
   
   // Now add either the number or numberex, which ever was not in record
   if (pwzNumEX != NULL)
   {
      // add number
      wDataSize += ConvNumEXToNum(pwzNumEX, wNumExSize, NULL) + 1;
      wDataSize = ((wDataSize + 3) & (~3));
      nFields++;
   }

   // Update the entry with the new memory
   if (nFields == 0)
      return EFAILED;
   pCHEntry->wNumFields = nFields;
   pCHEntry->pFields = MALLOC(sizeof(AEECallHistoryField) * nFields);
   if (pCHEntry->pFields) {
      pCHEntry->pFields[0].pData = MALLOC(wDataSize);
      if (!pCHEntry->pFields[0].pData) {
         FREE(pCHEntry->pFields);
         pCHEntry->pFields = NULL;
         return ENOMEMORY;
      }
   } else
      return ENOMEMORY;
   return SUCCESS;
}

/*==================================================================
Function: CopyIntoCHEntry

Description:
   Copy records out of database format and into CallHistory format

Prototype:
   static int CopyIntoCHEntry(AEECallHistoryEntry *pCHEntry, 
                              IDBRecord *pDBRec);

Parameter(s):  
   pCHEntry - Pointer to prepared CH entry to fill in
   pDBRec - Pointer to a database record

Return Value:
   SUCCESS if database record was formatted correctly and data could be
   copied to the call history entry.

   EFAILED if the database record was not formatted correctly, resulting
   in an abort of the copy process.

Comments:
   This function must only be called after ReallocCHEntry.

Side Effects:
See Also:
   ReallocCHEntry
==================================================================*/
static int CopyIntoCHEntry(AEECallHistoryEntry *pCHEntry, IDBRecord *pDBRec)
{
   int nCurrRec = 0;
   byte *pWrite;
   byte *pDBData;
   uint32 dwClsID = 0;
   AEEDBFieldType ft;
   AEEDBFieldName fn;
   uint16 wSize;
   uint16 wNumExSize = 0;
   uint16 wCHType;
   boolean bHaveType = FALSE;
   AECHAR *pwzNumEX = NULL;

   // We assume the memory is exactly the size we want.
   pWrite = pCHEntry->pFields[0].pData;

   // Reset the DBEntry so we get the first field
   IDBRECORD_Reset(pDBRec);

   // Do the for loop here
   while ((ft = IDBRECORD_NextField(pDBRec, &fn, &wSize)) != AEEDB_FT_NONE) 
   {   	  
      switch(fn) 
	  {
         /* Change of classid marker */
         case CHFIELD_CLSID_CHANGE:
         {
		 	if (IDBRECORD_GetFieldDWord(pDBRec, &dwClsID) == FALSE)
               return EFAILED; // all ClsIDs are dwords, this isn't
            break;
         }

         /* Field type identifier */
         case CHFIELD_CHID:
            // make sure we don't already have a type for the upcoming data
            if (bHaveType)
               return EFAILED; 

            // Extract the CH Field ID
            if (IDBRECORD_GetFieldWord(pDBRec, &wCHType) == FALSE)
               return EFAILED; // CH field type must be a word
            bHaveType = TRUE;
            break;

         /* Data (payload) fields */
         case CHFIELD_CALLTYPE:
         case CHFIELD_NUMBEREXDATA:
         case CHFIELD_DATA:
            // Must know what the type is corresponding to this data
            if (!bHaveType)
               return EFAILED;

            // Get the binary data out of the database
            pDBData = IDBRECORD_GetField(pDBRec, &fn, &ft, &wSize);
            if (pDBData == NULL)
               return EFAILED;
            bHaveType = FALSE;
			
            // Fill in the record
            pCHEntry->pFields[nCurrRec].ClsId = dwClsID;
            pCHEntry->pFields[nCurrRec].wID = wCHType;
            pCHEntry->pFields[nCurrRec].wDataLen = wSize;
            pCHEntry->pFields[nCurrRec].pData = pWrite;

            nCurrRec++;

            // Copy the data
            MEMCPY(pWrite, pDBData, wSize);
            
            if (wCHType == AEECALLHISTORY_FIELD_NAME)
            {
               // Add UNICODE NULL
               *(uint16*)(pWrite + wSize) = '\0';
               wSize += 2;
            }
			else if (wCHType == AEECALLHISTORY_FIELD_NUMBER_EX)
			{
               // Add UNICODE NULL
               *(uint16*)(pWrite + wSize) = '\0';
               wSize += 2;
               wNumExSize = wSize;
               pwzNumEX = (AECHAR*)pWrite;
            }
			else if (wCHType == AEECALLHISTORY_FIELD_NUMBER)
			{
               // Add ASCII NULL
               *(byte*)(pWrite + wSize) = '\0';
               wSize++;
            } 
			else if (wCHType == AEECALLHISTORY_FIELD_ID)
			{
               // Add UNICODE NULL
               *(uint16*)(pWrite + wSize) = '\0';
               wSize += 2;
            }

            // Move write pointer forward
            pWrite = (byte*)((((uint32)pWrite) + wSize + 3) & (~3));
            break;

         /* We don't understand this field name */
         default:
            // We've encountered a record name we don't support
            return EFAILED;
      }
   }

   // Now add the number field if we found a numberEx
   if (pwzNumEX != NULL)
   {
      wSize = ConvNumEXToNum(pwzNumEX, wNumExSize, pWrite);
      *(char*)(pWrite + wSize) = '\0';

      // Fill in the record
      pCHEntry->pFields[nCurrRec].ClsId = 0;
      pCHEntry->pFields[nCurrRec].wID = AEECALLHISTORY_FIELD_NUMBER;
      pCHEntry->pFields[nCurrRec].wDataLen = wSize;
      pCHEntry->pFields[nCurrRec].pData = pWrite;
	  
      nCurrRec++;
   }
   
   // Make sure we've filled in all of the records
   if (nCurrRec != pCHEntry->wNumFields)
   {
   		return EFAILED;
   }

   // We're done return success
   return SUCCESS;
}

/*==================================================================
Function: MakeEntry

Description:
   Given an array of AEECallHistoryFields, allocate and fill in a
   database structure to hold all of the data.

Prototype:
   static int MakeEntry(uint16 wEntryID, const AEECallHistoryEntry *pEntry,
                        AEEDBField **ppDBFields, int *pnDBFields)

Parameter(s):  
   wEntryID - Indicates location in circular buffer
   pEntry - Pointer to struct containing pFields array
   ppDBFields - Pointer to array pointer for database fields
   pnDBFields - Number of array items in *ppDBFields

Return Value:
   SUCCESS/Failure code

Comments:
   Memory is allocated for *ppDBFields.  The caller is expected to free 
   this memory.
Side Effects:  None 
See Also:      None
==================================================================*/
static int MakeEntry(const AEECallHistoryEntry *pEntry,
                     AEEDBField **ppDBFields, int *pnDBFields)
{
   AEEDBField *pRet;
   int nDBFields;
   uint32 dwCurrClsID = 0;
   int i;
   int nBaseAllocSize;
   void *pRealloc;
   uint16 wNewFieldSize;
   boolean bNumFound = FALSE;

   // Set stuff to NULL in case we need to return early
   *ppDBFields = NULL;
   *pnDBFields = 0;

   // We allocate memory for the worst-case scenario, 3 DBfields for each
   // AEECallHistoryField, plus an identifier field
   nBaseAllocSize = sizeof(AEEDBField) * (pEntry->wNumFields * 3 + 1);

   // Allocate, and add a uint16 to store wEntryID in for the id record
   pRet = MALLOC(nBaseAllocSize);
   if (pRet == NULL)
      return ENOMEMORY;

   nDBFields = 0;
 
   // NumberEx/Number field logic:  The NumberEx field was added to provide
   // more capability to BREW applets.  The NumberEx field is, for the most
   // part, up to the OEM in its format.  For simplicity and compatibility
   // reasons, if either a Number or NumberEx field is present in an entry
   // to be stored, both a Number and NumberEx field must be returned in
   // when subsequently enumerated.
   //
   // It is probable that a legacy applet may modify the Number field of 
   // another record, or the current record if doing an 
   // ICALLHISTORY_UpdateEntry() call, and not knowing the format or meaning
   // of the NumberEx field, store that as well.  For this reason, when both
   // the Number and NumberEx fields are present in a record, the NumberEx
   // field is ignored.
   //
   // For the purpose of this implementation, we look ahead through the entry
   // and determine if the entry has a Number field.  If so, the NumberEx
   // field will be ignored.  In either case, the number is stored internally
   // as a NumberEx value, as it can represent either format.

   // Scan the fields for the number type
   for (i=0; i<pEntry->wNumFields; i++) 
   {
      if (pEntry->pFields[i].ClsId == 0 && 
          pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_NUMBER)
      {
         bNumFound = TRUE;
      }
   }
   
   // Just fill it in, allocating along the way 
   for (i=0; i<pEntry->wNumFields; i++) {
     if (pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_NUMBER_EX &&
         pEntry->pFields[i].ClsId == 0 &&
         bNumFound)
     {
        continue;
     }
     
     // First, put in a change-of-classid token if necessary.  Keep in
      // mind that AEECLSID_CALLHISTORY and 0 are equivalent
     if (pEntry->pFields[i].ClsId != dwCurrClsID && 
          (dwCurrClsID != 0 || 
           pEntry->pFields[i].ClsId != AEECLSID_CALLHISTORY_30 ||
           pEntry->pFields[i].ClsId != AEECLSID_CALLHISTORY)) {
         if (pEntry->pFields[i].ClsId == AEECLSID_CALLHISTORY_30 ||
             pEntry->pFields[i].ClsId == AEECLSID_CALLHISTORY) {
            dwCurrClsID = 0;
            pRet[nDBFields].pBuffer = &gConstZero;
         } else {
            dwCurrClsID = pEntry->pFields[i].ClsId;
            pRet[nDBFields].pBuffer = (void*)&pEntry->pFields[i].ClsId;
         }
         pRet[nDBFields].fType = AEEDB_FT_DWORD;
         pRet[nDBFields].fName = CHFIELD_CLSID_CHANGE;
         pRet[nDBFields].wDataLen = sizeof(uint32);
         nDBFields++;
      }

      // Add the type identifier.  Alias Number fields to NumberEx Fields
      pRet[nDBFields].fType = AEEDB_FT_WORD;
      pRet[nDBFields].fName = CHFIELD_CHID;
      pRet[nDBFields].wDataLen = sizeof(uint16);
      if (pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_NUMBER) {
         pRet[nDBFields].pBuffer = (void*)&gwFTNumberEx;
      } else {
         pRet[nDBFields].pBuffer = &pEntry->pFields[i].wID;
      }
      nDBFields++;

      // Add the data as binary, regardless of original type.  Name
      // the field CHFIELD_DATA, unless it's a type or phone number
      if (dwCurrClsID == 0 && 
          pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_CALL_TYPE) {
         pRet[nDBFields].fType = AEEDB_FT_WORD;
         pRet[nDBFields].fName = CHFIELD_CALLTYPE;
      } else if (dwCurrClsID == 0 &&
          (pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_NUMBER_EX ||
           pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_NUMBER)) {
         pRet[nDBFields].fType = AEEDB_FT_BINARY;
         pRet[nDBFields].fName = CHFIELD_NUMBEREXDATA;
      } else {
         pRet[nDBFields].fType = AEEDB_FT_BINARY;
         pRet[nDBFields].fName = CHFIELD_DATA;
      }

      if (dwCurrClsID == 0 &&
          pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_NUMBER)
      {
         // Convert this to a NumberEx for consistent storage.

         // NumberEx is a AECHAR, so double the size of the char*
         // Reallocate the buffer and put it at the end.
         wNewFieldSize = ConvNumToNumEX(NULL, pEntry->pFields[i].pData,
                                        pEntry->pFields[i].wDataLen);

         pRealloc = REALLOC(pRet, nBaseAllocSize + wNewFieldSize);
         if (pRealloc == NULL)
         {
            FREE(pRet);
            return ENOMEMORY;
         }

         // Put the new (converted) number at the end of the newly enlaged
         // allocation.
         pRet = (AEEDBField*)pRealloc;
         pRet[nDBFields].wDataLen = wNewFieldSize;
         pRet[nDBFields].pBuffer = (char*)pRet + nBaseAllocSize;
         pRet[nDBFields].fName = CHFIELD_NUMBEREXDATA;
         nBaseAllocSize += (int)wNewFieldSize;

         ConvNumToNumEX((AECHAR*)pRet[nDBFields].pBuffer,
                        pEntry->pFields[i].pData,
                        pEntry->pFields[i].wDataLen);
      }
      else
      {
         // Standard, no conversions add
         pRet[nDBFields].wDataLen = pEntry->pFields[i].wDataLen;
         pRet[nDBFields].pBuffer = pEntry->pFields[i].pData;
      }
      nDBFields++;
   }

   // Return 
   *ppDBFields = pRet;
   *pnDBFields = nDBFields;
   return SUCCESS;
}

/*==================================================================
Function: VerifyFields

Description:
   Verify that the array of items passed in appears to be correct

Prototype:
   static int VerifyFields(AEECallHistoryEntry *pEntry);

Parameter(s):  
   pEntry - Pointer to struct containing item array

Return Value:
   SUCCESS - All items seem to be correct.  All item types understood.
   EUNSUPPORTED - Known item types seem to be correct, unknown item
                  types exist.
   EBADPARM - One of the fields contains invalid data

Comments:
   Verification is done through a function pointer interface.  For
   each known item type, there is a function

Side Effects:
See Also:
==================================================================*/
static int VerifyFields(AEECallHistoryEntry *pEntry)
{
   boolean bFoundUnsupported = FALSE;
   int nVerifyInx;
   int nErr;
   int i;

   for (i=0; i<pEntry->wNumFields; i++) {
      if (pEntry->pFields[i].wDataLen == 0)
         return EBADPARM;

      if (pEntry->pFields[i].ClsId != 0 && 
          pEntry->pFields[i].ClsId != AEECLSID_CALLHISTORY_30 &&
          pEntry->pFields[i].ClsId != AEECLSID_CALLHISTORY) {
         /* CUSTOM TYPES */
         bFoundUnsupported = TRUE;
      } else {
         /* BUILT-IN TYPES */
         PFNRAWDATA pfnVerify;

         /* Verify that the parameter they're sending us is one of the 
         ** built-in field ids */
         if (pEntry->pFields[i].wID < AEECALLHISTORY_FIELD_CALL_TYPE || 
             pEntry->pFields[i].wID > AEECALLHISTORY_FIELD_MAX) {
            return EBADPARM;
         }

         /* Index into our verification function table for this wID */
         nVerifyInx = pEntry->pFields[i].wID - AEECALLHISTORY_FIELD_NONE - 1;
         pfnVerify = gpVerifyFns[nVerifyInx];

         /* Call verification function specific to this type to make
         ** sure data is correctly formatted.  */
         nErr = pfnVerify((byte*)pEntry->pFields[i].pData, 
                          pEntry->pFields[i].wDataLen);

         if (nErr != EUNSUPPORTED && nErr != SUCCESS)
            return EBADPARM;

         if (nErr == EUNSUPPORTED) {
            /* Rare case -- our type-specific verifcation function thinks
            ** its a valid format, but it just can't understand some or
            ** all of it */
            bFoundUnsupported = TRUE;
         }
      }
   }
   if (bFoundUnsupported)
      return EUNSUPPORTED;
   return SUCCESS;
}

/*==================================================================
Function: BuildEnumTable

Description:
   Builds a table to enumerate through

Prototype:
   static boolean BuildEnumTable(OEMCallHistory *pme);

Parameter(s):  
   pme - pointer to OEMCallHistory object associated with CH database

Return Value:  
   TRUE - Table was successfully built
   FALSE - An error occurred

Comments:      None
Side Effects:  None
See Also:      None
==================================================================*/
static boolean BuildEnumTable(OEMCallHistory *pme)
{
   int nAge;
   int nRecs;
   int nReallocSize;
   void *pRealloc;
   uint16 wDBInx;
   IDBRecord *pRec;

   nRecs = IDATABASE_GetRecordCount(pme->m_pDB);

   // Allocate an array of database indexes.  Don't allocate one for the special record   
   nReallocSize = sizeof(uint16) * nRecs;
   pRealloc = REALLOC(pme->m_pwEnumData, nReallocSize);
   if (nReallocSize == 0) 
   {
      pme->m_pwEnumData = NULL;
      pme->m_nEnumDataEnts = 0;
      return TRUE;
   }
   if (pRealloc == NULL)
      return FALSE;
   // We fill the array in with 0xFFFFs.  Thus any missing records
   // will have a value of zero.  Note that this is an unexpected case.
   MEMSET(pRealloc, 0xFF, nReallocSize);
   pme->m_pwEnumData = (uint16*)pRealloc;
   pme->m_nEnumDataEnts = nRecs;

   // We just need to reverse the order of enumeration.  Set nAge to be the oldest record,
   // and move towards newer below.  Actually set it to be the last index + 1, since we
   // decrement below.
   nAge = pme->m_nEnumDataEnts;

   // Go through the database and put the index in the appropriate spot
   IDATABASE_Reset(pme->m_pDB);

   while ((pRec = IDATABASE_GetNextRecord(pme->m_pDB)) != NULL) {
      // Get the record's ID
      wDBInx = IDBRECORD_GetID(pRec);
      if (wDBInx == AEE_DB_RECID_NULL) {
         IDBRECORD_Release(pRec);
         continue;
      }

      // The next entry will be one newer, i.e. subtract an index.
      nAge--;

      IDBRECORD_Release(pRec);

      // nAge less than zero means we somehow wrote an invalid index.
      // nAge greater than or equal to the number of entries can't be right
      if (nAge < 0 || nAge >= pme->m_nEnumDataEnts)
         continue;

      // Put it in the table based on its age
      pme->m_pwEnumData[nAge] = wDBInx;
   }
   return TRUE;
}

/*==================================================================
Function: BuildEnumByCallTypeTable

Description:
   Builds a table to enumerate through

Prototype:
   static boolean BuildEnumTable(OEMCallHistory *pme);

Parameter(s):  
   pme - pointer to OEMCallHistory object associated with CH database

Return Value:  
   TRUE - Table was successfully built
   FALSE - An error occurred

Comments:      None
Side Effects:  None
See Also:      None
==================================================================*/
static boolean BuildEnumByCallTypeTable(OEMCallHistory *pme, uint16 type)
{
   IDBRecord      *pRec;
   AEEDBFieldName fieldName;
   word           recType;
   
   uint16 *pNewList;
   uint16 *pOldList;   
   uint32 nRecs;
   uint32 newIdx = 0;
   uint32 i;
	
    if (BuildEnumTable(pme) != TRUE)
    {
       return FALSE;
    }
    pOldList = pme->m_pwEnumData;
    nRecs = pme->m_nEnumDataEnts;
      
    // Allocate a new array of database indexes.  
    // Don't allocate one for the special record   
    pNewList = MALLOC(sizeof(uint16) * nRecs);
	
    if (pNewList == NULL)
    {
       return FALSE;  
    }

    // We fill the array in with 0xFFFFs.  Thus any missing records
    // will have a value of zero.  Note that this is an unexpected case.
    MEMSET(pNewList, 0xFF, sizeof(uint16) * nRecs);

    // Determine each records type
    for (i = 0; i < nRecs; i++ ){
      
       if( pOldList[i] != CH_INVALID_DB_INX )
       {
          // Open that record
          pRec = IDATABASE_GetRecordByID(pme->m_pDB, pOldList[i]);
        
          if (pRec == NULL) 
          {
             // For whatever reason we can't open that record
             if (pNewList != NULL)
             {
                  FREE(pNewList);
             }
             return(FALSE);  
          }
        
          // Place record type in new list if correct type
          IDBRECORD_Reset(pRec);
          while (IDBRECORD_NextField(pRec, &fieldName, NULL) != AEEDB_FT_NONE)
          {
             if (fieldName == AEEDBFIELD_CATEGORY)
			 {
                IDBRECORD_GetFieldWord(pRec, &recType);
				
                if (fieldName == AEEDBFIELD_CATEGORY)
                {
                   if( recType == type )
                   {
                      pNewList[newIdx++] = pOldList[i];
                   }
                }
                break;
             }
          }
          IDBRECORD_Release(pRec);
       }
    }

    FREE(pOldList);
    pOldList = NULL;
	
    if (newIdx > 0)
    {
       pOldList = REALLOC(pNewList, sizeof(uint16) * newIdx);
       if (pOldList == NULL)
	   {
          pOldList = pNewList;
       }
    }
    else
    {
       FREE(pNewList);
       pNewList = NULL;
    }
    pme->m_pwEnumData = pOldList;
    pme->m_nEnumDataEnts = newIdx;

    return TRUE;
}

#ifdef CATEGORIZED_CALLHISTORY
static boolean AdvanceToField(IDBRecord *pRec, AEECLSID cls, uint16 wID)
{
   AEEDBFieldName fn;
   uint16 wLen;
   uint16 wIDRec;
   AEECLSID ClsCurr = 0;

   IDBRECORD_Reset(pRec);

   // And every field
   while (IDBRECORD_NextField(pRec, &fn, &wLen) != AEEDB_FT_NONE) {
      if (fn == CHFIELD_CLSID_CHANGE) {
         IDBRECORD_GetFieldDWord(pRec, &ClsCurr);
      } else if (fn == CHFIELD_CHID) {
         IDBRECORD_GetFieldWord(pRec, &wIDRec);
         if (ClsCurr == cls && wIDRec == wID) {
            IDBRECORD_NextField(pRec, &fn, &wLen);
            // Data should always follow CHID
            if (fn != CHFIELD_DATA && fn != CHFIELD_NUMBEREXDATA && fn != CHFIELD_CALLTYPE)
               return FALSE;
            return TRUE;
         }
      }
   } // while we still have fields

   return FALSE;
}

static uint16 RecCallType(IDBRecord *pRec)
{
   uint16 wCallType;

   if (!AdvanceToField(pRec, 0, AEECALLHISTORY_FIELD_CALL_TYPE))
      return 0;

   IDBRECORD_GetFieldWord(pRec, &wCallType);

   return wCallType;   
}

static int GetRecordCountByType(OEMCallHistory *pme, uint16 wCallType)
{
   IDBRecord *pRec;
   int nRet = 0;

   IDATABASE_Reset(pme->m_pDB);
   while ((pRec = IDATABASE_GetNextRecord(pme->m_pDB)) != NULL) 
   {
      if (RecCallType(pRec) == wCallType)
      {
      	nRet++;
      }
      IDBRECORD_Release(pRec);
   }   
   return nRet;
}

static void DeleteOldestOfType(OEMCallHistory *pme, uint16 wCallType)
{
   IDBRecord *pRec;

   IDATABASE_Reset(pme->m_pDB);
   while ((pRec = IDATABASE_GetNextRecord(pme->m_pDB)) != NULL) 
   {
      if (RecCallType(pRec) == wCallType) {
         IDBRECORD_Remove(pRec);
         break;
      }
      IDBRECORD_Release(pRec);
   }
}

static boolean CheckReqFields(const AEECallHistoryEntry *pEntry,
                              uint16 *pdwCallType)
{
   int i;

   // Make sure there's a CALL_TYPE field
   for (i=0; i<pEntry->wNumFields; i++) {
      if (pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_CALL_TYPE)
         break;
   }
   if (i==pEntry->wNumFields)
      return FALSE;
   *pdwCallType = *(uint16*)pEntry->pFields[i].pData;
   return TRUE;
}
#endif /* CATEGORIZED_CALLHISTORY */

/*==================================================================
Function: ConvNumEXToNum

Description:
   Converts the newer NumberEx field into the simpler legacy Number
   field.

Prototype:
   static int ConvNumEXToNum(AECHAR *pwzNumEx, uint16 wSize, byte *pzNum)

Parameter(s):  
   pwzNumEx: [in]: NumberEx field to convert from
   wSize: [in]: size of pwzNumEx, in bytes
   pszNum: [out]: Number field to store converted data

Return Value:  
   length of character string

Comments:      None
Side Effects:  None
See Also:      
   ConvNumToNumEX
==================================================================*/
static int ConvNumEXToNum(AECHAR *pwzNumEx, uint16 wSize, byte *pzNum)
{ 
   char ch; 
   uint32 i, j;

   if (pwzNumEx == NULL) { 
      return 0; 
   } 
  
   wSize /= 2; 
   for (i = 0, j = 0; i < wSize; i++) { 
      ch = (char)pwzNumEx[i]; 
    
      if ((ch >= '0' && ch <= '9') || 
          (ch == '#' || ch == '+' || ch == ',' || ch == '*')) { 
         if (pzNum != NULL) { 
            pzNum[j] = ch; 
         } 
         j++; 
      } else { 
         // Stop at the first non-numeric character 
         break; 
      } 
   } 
  
   return j; 
}

/*==================================================================
Function: ConvNumToNumEX

Description:
   Converts a NumberEx-style phone number to the simpler Number format.

Prototype:
   static int ConvNumToNumEX(AECHAR *pwzDst, char *pSrc, int nLen);

Parameter(s):
   pwzDst: [out]: Destination NumberEx style string to write to, or
          NULL to return the amount of space needed.
   pSrc: [in]: Source Number-style field to be converted from
   nLen: [in]: Length, in bytes, of the source string.  This does
               not include any NULL-termination bytes 

Return Value:  
   length of AECHAR string

Comments:      None
Side Effects:  None
See Also:      
   ConvNumEXToNum
==================================================================*/
static int ConvNumToNumEX(AECHAR *pwzDst, char *pSrc, int nLen)
{
   // Work backwards.  Its easier that way.
   if (pwzDst != NULL)
   {
      while (nLen > 0)
      {
         --nLen;
         pwzDst[nLen] = (AECHAR)pSrc[nLen];
      }
   }
   return nLen * 2;
}


/*==================================================================
**
** Item Verifcation Functions (called via table of pointers)
**
==================================================================*/
static int ItemVerify_CallType(byte *pData, uint16 nDataLen)
{
   uint16 wVal = *pData;
   if (nDataLen != sizeof(uint16))
      return EFAILED;
   if (wVal < AEECALLHISTORY_CALL_TYPE_TO)
      return EFAILED;
   if (wVal > AEECALLHISTORY_CALL_TYPE_MISSED)
      return EFAILED;
   return SUCCESS;
}

static int ItemVerify_NumberType(byte *pData, uint16 nDataLen)
{
   uint16 wVal = *pData;
   if (nDataLen != sizeof(uint16))
      return EFAILED;
   if (wVal < AEECALLHISTORY_NUMBER_TYPE_INTERNATIONAL)
      return EFAILED;
   if (wVal > AEECALLHISTORY_NUMBER_TYPE_QCHAT)
      return EFAILED;
   return SUCCESS;
}

static int ItemVerify_NumberPlan(byte *pData, uint16 nDataLen)
{
   uint16 wVal = *pData;
   if (nDataLen != sizeof(uint16))
      return EFAILED;
   if (wVal < AEECALLHISTORY_NUMBER_PLAN_TELEPHONY)
      return EFAILED;
   if (wVal > AEECALLHISTORY_NUMBER_PLAN_PRIVATE)
      return EFAILED;
   return SUCCESS;
}

static int ItemVerify_DateTime(byte *pData, uint16 nDataLen) 
{
   PARAM_NOT_REF(pData);
   if (nDataLen != sizeof(uint32))
      return EFAILED;
   return SUCCESS;
}

static int ItemVerify_CallDuration(byte *pData, uint16 nDataLen)
{
   PARAM_NOT_REF(pData);
   if (nDataLen != sizeof(uint32))
      return EFAILED;
   return SUCCESS;
}

static int ItemVerify_Number(byte *pData, uint16 nDataLen) 
{
   if (nDataLen == 0)
      return EFAILED;
   while(nDataLen--) {
      if (*pData < '0' || *pData > '9') {
         switch (*pData) {
            case '#':
            case '*':
            case '+':
            case ',':
               break;

            default:
#ifdef FEATURE_SYNCED_CALLHISTORY
               return OEMPBM_VerifyNumber(*pData);
#else
               return EFAILED;
#endif
         }
      }
      pData++;
   }
   return SUCCESS;
}

static int ItemVerify_Name(byte *pData, uint16 nDataLen)
{
   uint16 *pwName = (uint16*)pData;
   int nChars;

   /* Make sure they passed us something */
   if (nDataLen == 0)
      return EFAILED;

   /* Since Unicode is 2 bytes divide nDataLen by 2 */
   if (nDataLen % 2)
      return EFAILED;
   nChars = nDataLen / 2;
   if (nChars > CH_MAX_NAME)
      return EFAILED;

   /* Only invalid character I know of is a 0000.  Check for that, except
   ** at the end */
   while (--nChars) {
      if (*pwName == 0)
         return EFAILED;
      pwName++;
   }
   return SUCCESS;
}

static int ItemVerify_ClsId(byte *pData, uint16 nDataLen)
{
   // Must be four bytes long and non-zero
   if (nDataLen != 4 || *((uint32*)pData) == 0)
      return EFAILED;
   return SUCCESS;
}

static int ItemVerify_NumberEx(byte *pData, uint16 nDataLen) 
{
   uint16 *pwName = (uint16*)pData;
   int nChars;

   /* Make sure they passed us something */
   if (nDataLen == 0)
      return EFAILED;

   /* Since Unicode is 2 bytes divide nDataLen by 2 */
   if (nDataLen % 2)
      return EFAILED;
   nChars = nDataLen / 2;
   if (nChars > CH_MAX_NAME)
      return EFAILED;

   /* Only invalid character I know of is a 0000.  Check for that, except
   ** at the end */
   while (--nChars) {
#ifdef FEATURE_SYNCED_CALLHISTORY
      int nErr = OEMPBM_VerifyNumberEx(*pwName);
      if(SUCCESS != nErr) {
         return nErr;
      }
#else
      if (*pwName == 0)
         return EFAILED;
#endif

      pwName++;
   }
   return SUCCESS;
}

static int ItemVerify_PresentationIndicator(byte *pData, uint16 nDataLen) 
{
   if (nDataLen != sizeof(AEETPI)) {
      return EFAILED;
   }

   if (*(uint32*) pData <= AEET_PI_NOT_AVAIL) {
      // Presentation indicator is an unsigned value, so we only need to 
      // check the upper bound.
      return SUCCESS;
   } 

   return EFAILED;
}

static int ItemVerify_Id(byte *pData, uint16 nDataLen) 
{
   return ItemVerify_WStrField(pData, nDataLen); 
}

static int ItemVerify_CallActive(byte *pData, uint16 nDataLen) 
{
   return ItemVerify_BooleanField(pData, nDataLen);
}

static int ItemVerify_DurationKnown(byte *pData, uint16 nDataLen) 
{
   return ItemVerify_BooleanField(pData, nDataLen); 
}

static int ItemVerify_NotViewed(byte *pData, uint16 nDataLen) 
{
   return ItemVerify_BooleanField(pData, nDataLen); 
}

static int ItemVerify_TechType(byte *pData, uint16 nDataLen)
{
   uint16 wVal = *( (uint16*)pData);
   if (nDataLen != sizeof(uint16))
      return EFAILED;

   if (wVal > AEECALLHISTORY_TECH_TYPE_VIDEO)
      return EFAILED;

   return SUCCESS;
}

static int ItemVerify_Counter(byte *pData, uint16 nDataLen)
{
   if (nDataLen != sizeof(uint16))
      return EFAILED;

   return SUCCESS;
}

static int ItemVerify_WStrField(byte *pData, uint16 nDataLen) 
{
   uint16 *pwName = (uint16*)pData;
   int nChars;

   /* Make sure they passed us something */
   if (nDataLen == 0)
      return EFAILED;

   /* Since Unicode is 2 bytes divide nDataLen by 2 */
   if (nDataLen % 2)
      return EFAILED;
   nChars = nDataLen / 2;
   if (nChars > CH_MAX_NAME)
      return EFAILED;

   /* Only invalid character I know of is a 0000.  Check for that, except
   ** at the end */
   while (--nChars) {
      if (*pwName == 0)
         return EFAILED;
      pwName++;
   }
   return SUCCESS;
}

static int ItemVerify_BooleanField(byte *pData, uint16 nDataLen) 
{
   boolean bValue = *pData;
   
   if (nDataLen != sizeof(boolean) || (bValue != FALSE && bValue != TRUE)) {
      return EFAILED;
   }

   return SUCCESS;
}
/*=============================================================================
FUNCTION: DupCHEntryWithoutNumberField

DESCRIPTION:   Dup the original call history entry fields with _NUMBER field 
               being stripped.

PARAMETERS:
   *pEntry      [in]:  Call History entry 
  **ppNewItems [out]:  Pointer to the new call history fields array containing
                       all the fields of the original entry except _NUMBER 
                       field 
   
RETURN VALUE:
   uint16:   Number of fields in the *ppNewItems.

COMMENTS:

    Caller is responsible for freeing *ppNewItems

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint16 DupCHEntryWithoutNumberField(
   const AEECallHistoryEntry *pEntry,
   AEECallHistoryField **ppNewItems
)
{
   uint16 i; 
   uint16 nItemCount = 0;
   uint16 nNewItemCount = 0;
   AEECallHistoryField *pNewItems = NULL;
   AEECallHistoryField *pNewFields = NULL;
   AEECallHistoryField *pItems = NULL;
   
   do {
   
      if (NULL == pEntry || 0 == pEntry->wNumFields || 
          NULL == pEntry->pFields || NULL == ppNewItems) {
         break;
      }

      *ppNewItems = NULL;
      pItems = pEntry->pFields;
      nItemCount = pEntry->wNumFields;
   
      pNewItems = MALLOC(sizeof(AEECallHistoryField) * (uint32)nItemCount);
      if (NULL == pNewItems) {
         break;
      }

      pNewFields = pNewItems;
      // Remove the _NUMBER field from the original entry that got added by
      // the rules of backward compatibility 
      for (i = 0; i < nItemCount; i++, pItems++) {
         if (0 == pItems->ClsId && AEECALLHISTORY_FIELD_NUMBER == pItems->wID) {
            continue;
         }
         pNewFields->ClsId = pItems->ClsId;
         pNewFields->wID = pItems->wID;
         pNewFields->wDataLen = pItems->wDataLen; 
         pNewFields->pData = pItems->pData;
         pNewFields++;  
         nNewItemCount++;   
      }

      *ppNewItems = pNewItems;

   } while(0);

   return nNewItemCount;
} 



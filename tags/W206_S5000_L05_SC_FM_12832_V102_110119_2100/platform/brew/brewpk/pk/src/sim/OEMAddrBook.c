/*======================================================
FILE:  OEMAddrBook.c

SERVICES:  OEM functions for supporting BREW Address Book Interface

GENERAL DESCRIPTION:
   This file provides the prorotypes of the functions that OEMs must provide in
   order to support the BREW AddressBook interfaces

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "OEMFeatures.h"
#include "OEMHeap.h"
#include "OEMDB.h"
#include "OEMAddrBook.h"
#ifdef FEATURE_ADDRBOOK_RUIM
#include "OEMAddrBookRUIM.h"
#endif
#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEAddrBook.h"
#include "OEMClassIDs.h"
#include "AEEStdLib.h"
#include "assert.h"

//Global Context for AddressBook.
OEMCONTEXT  ghAddrBk = NULL;

OBJECT(COEMAddrBook)
{
   // Standard stuff
   AEEVTBL(IOEMAddrBook) *pvt;

   uint32            m_nRefs;
};

#define OEMADDRBOOK_NAME   "phbk"

//Expected minimum number of records expected in AddressBook. This is used only if we are 
//creating a new address book.
#define OEMADDRBOOK_MINRECS      10

//Expected size of each record. This is used only if we are 
//creating a new address book.
#define OEMADDRBOOK_MINRECSIZE   50


//Number of address categories supported by this address book
#define  OEM_ADDRESS_CATEGORY_COUNT    (sizeof(gSupportedCatList)/sizeof(SupportedCat))

//Number of fields supported in the address book
#define  OEM_ADDRESS_FIELDS_COUNT      (sizeof(gSupportedFieldList)/sizeof(SupportedField))

//Define the following macro to restrict address book fields to those
//explicity supported by the OEM layer.  If undefined, applications may create
//user-defined fields.
#undef OEMADDRBOOK_RESTRICT_FIELDS

typedef struct _AddrBkCache   AddrBkCache;

//Struct to hold supported category list items
typedef struct _SupportedCat
{
   AEEAddrCat cat;
   char *     pszName;
} SupportedCat;

//Struct to hold supported field list items
typedef struct _SupportedField
{
   AEEAddrFieldInfo afi;
   char *           pszName;
} SupportedField;

//Struct holding vital information that will be cached
struct _AddrBkCache
{
   uint16         wRecID;     //ID of the Record
   AECHAR*        szName;     //Name. If its NULL, its an untitled record
   AddrBkCache*   pNext;      //Linked list
};


//Cache of address book in memory
AddrBkCache       *gpCache;          //Cache of vital fields in AddressBook
boolean           gNoCache;          //Not using cache
boolean           gbCacheInited;     //Has cache been read in? Don't check cache pointer itself 
                                     //  because there may not be any records to put in cache

//Properties
uint32            gwProps;

//List of supported categories
static const SupportedCat gSupportedCatList[] =
{
   { AEE_ADDR_CAT_PERSONAL, "Personal" },
   { AEE_ADDR_CAT_BUSINESS, "Business" }
};

//List of supported fields
static const SupportedField gSupportedFieldList[] =
{
   {{ AEE_ADDRFIELD_NAME,          1, AEEDB_FT_STRING, TRUE, TRUE }, "Name" },
   {{ AEE_ADDRFIELD_PHONE_GENERIC, 1, AEEDB_FT_STRING, TRUE, TRUE }, "Phone" },
   {{ AEE_ADDRFIELD_PHONE_HOME,    1, AEEDB_FT_STRING, TRUE, TRUE }, "Home" },
   {{ AEE_ADDRFIELD_PHONE_WORK,    1, AEEDB_FT_STRING, TRUE, TRUE }, "Work" },
   {{ AEE_ADDRFIELD_PHONE_FAX,     1, AEEDB_FT_STRING, TRUE, TRUE }, "Fax" },
   {{ AEE_ADDRFIELD_PHONE_PAGER,   1, AEEDB_FT_STRING, TRUE, TRUE }, "Pager" },
   {{ AEE_ADDRFIELD_PHONE_CELL,    1, AEEDB_FT_STRING, TRUE, TRUE }, "Mobile" },
   {{ AEE_ADDRFIELD_EMAIL,         2, AEEDB_FT_STRING, TRUE, TRUE }, "Email" },
   {{ AEE_ADDRFIELD_URL,           2, AEEDB_FT_STRING, TRUE, TRUE }, "URL" },
   {{ AEE_ADDRFIELD_ADDRESS,       2, AEEDB_FT_STRING, TRUE, TRUE }, "Address" },
   {{ AEE_ADDRFIELD_NOTES,         1, AEEDB_FT_STRING, TRUE, TRUE }, "Notes" },
   {{ AEE_ADDRFIELD_ORG,           1, AEEDB_FT_STRING, TRUE, TRUE }, "Company" },
   {{ AEE_ADDRFIELD_JOBTITLE,      1, AEEDB_FT_STRING, TRUE, TRUE }, "Title" },
   {{ AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE,      1, AEEDB_FT_WORD, TRUE, TRUE }, "FieldIDPrimaryPhone" }
};

//Global variables used in the search
static AEEAddrCat           gSearchCat[OEM_ADDRESS_CATEGORY_COUNT];  // AddressCategory to search on
static AEEAddrFieldID       gSearchFieldID[OEM_ADDRESS_FIELDS_COUNT];// Field ID to search on
static AEEAddrFieldType     gSearchDataType[AEEDB_FT_MAX];           // Data type to search on
static boolean              gbEnumInit;       // Flag to indicate if enumeration has been initialized
static uint16               gwCurRecID;       // Current RecordID in enumeration
static AEEAddrEnumRecInfo   gEnumRecInfo;     // Data values for enhanced searchs
static boolean              gbNameSearch;     // Flag to indicate if its a name-based search


static uint32  OEMAddr_AddRef(IOEMAddrBook * po);
static uint32  OEMAddr_Release(IOEMAddrBook * po);

//Get count of number of categories supported
static  int   OEMAddr_GetCatCount(void);

//Get Category list
static   int   OEMAddr_GetCatList(AEEAddrCat *p, int nSize);

//Get Number of fields supported for this category
static   int   OEMAddr_GetFieldInfoCount(AEEAddrCat);

// Get Fields Info
static   int   OEMAddr_GetFieldInfo(AEEAddrCat c, AEEAddrFieldInfo *pf, int nSize);

//Get Number of Records
static   uint16   OEMAddr_GetNumRecs(void);

// Add a record to the address book
static   uint16  OEMAddr_RecordAdd(AEEAddrCat cat, AEEAddrField * pItems, int nItemCount,int *pErr);

//Remove a record from the address book
static   int   OEMAddr_RecordDelete(uint16 recID);

//Update an existing Record in the address book
static   int  OEMAddr_RecordUpdate(uint16 recID,AEEAddrCat cat,AEEAddrField * pItems, int nItemCount,int *pErr);

//Get a record, given the ID of the record
static   int   OEMAddr_RecordGetByID(uint16 recID,AEEAddrCat *cat,AEEAddrField ** ppItems, int *nItemCount,int *pErr);

//Remove all Records
//If OEMs do not want to support this, just return EUNSUPPORTED
static   int   OEMAddr_RemoveAllRecs(void);

//Search Init
static   int   OEMAddr_EnumRecInit(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize);

//Get ID of Next record that matches the search criteria initialized above.
static   uint16   OEMAddr_EnumNextRec(AEEAddrCat *cat,AEEAddrField ** ppItems, int *nItemCount,int *pErr);

//Set address book properties
static   void   OEMAddr_SetProperties(uint32 dwProps);

//Get address book properties
static   uint32 OEMAddr_GetProperties(void);

static int OEMAddr_GetCategoryName(AEEAddrCat c, AECHAR *pszCatName, int *pnSize);
static int OEMAddr_GetFieldName(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize);


//Helper functions
static boolean  OEMAddrBook_Init(void);
static void     OEMAddrBook_Exit(void);
static byte*    AddressToRawData(AEEAddrCat cat, AEEAddrField *pItems,int nItemCount,int *pErr,uint16 *pSize);
static int      RawDataToAddress(byte *pBuf,word wRecSize, AEEAddrCat *pcat, AEEAddrField** ppItems,int *pnItemCount, int *pErr);
static int      ValidateFieldData(AEEAddrCat cat, AEEAddrField *pItems, int nItemCount);
static boolean  IsSearchMatch(uint16 wRecID,AEEAddrCat *cat,AEEAddrField ** ppItems, int *nCount,int *pErr);
static int      InitAddrBkCache(void);
static AECHAR * GetAddrFieldName(AEEAddrField *pItems,int nCount,int *pIndex);
static int      InsertCache(uint16 wRecID, AECHAR *szName);
static int      RemoveRecCache(uint16 wRecID);
static int      UpdateCache(uint16 wrecID,AECHAR *szName);
static int      ClearAddrBkCache(void);
static uint16   GetNextRecInCache(uint16 recID);
static uint16   GetNextMatchInCache(uint16 recID);
static void     FreeAddrCacheElem(AddrBkCache *pRemove);
static uint16   GetCacheRecCount(void);
static void     GetCache(void);
static void     FreeSearchData(void);
static int      CopyEnhancedEnumData( AEEAddrEnumRecInfo *pUserData );
static int      ConvertToEnhancedEnumData(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize);
static boolean  IsRecEnumDataValid(int *pErr);
static boolean  IsCategorySearchMatch(AEEAddrCat cat);
static boolean  IsFieldIDSearchMatch(AEEAddrFieldID fID);
static boolean  IsFieldTypeSearchMatch(AEEAddrFieldType fType);
static boolean  IsDataSearchMatch(void *pData, uint16 wDataLen );
static boolean  IsSearchAllowedInCategory(AEEAddrCat wCategory);
static boolean  IsSearchAllowedInField(AEEAddrFieldID wFieldID);
static boolean  OEMAddr_WStrStrEx( AECHAR *pHaystack, AECHAR *pNeedle, boolean bCaseInsensitive );

static const VTBL(IOEMAddrBook) gsOEMAddrBookFuncs ={ OEMAddr_AddRef,
                                                      OEMAddr_Release,
                                                      OEMAddr_EnumNextRec,
                                                      OEMAddr_EnumRecInit,
                                                      OEMAddr_GetCatCount,
                                                      OEMAddr_GetCatList,
                                                      OEMAddr_GetFieldInfo,
                                                      OEMAddr_GetFieldInfoCount,
                                                      OEMAddr_GetNumRecs,
                                                      OEMAddr_RecordAdd,
                                                      OEMAddr_RecordDelete,
                                                      OEMAddr_RecordGetByID,
                                                      OEMAddr_RecordUpdate,
                                                      OEMAddr_RemoveAllRecs,
                                                      OEMAddr_SetProperties,
                                                      OEMAddr_GetProperties,
                                                      OEMAddr_GetCategoryName,
                                                      OEMAddr_GetFieldName};


int OEMAddrBook_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj)
{
   COEMAddrBook *pNew;
   int Err = EUNSUPPORTED;

   *ppObj = NULL;

   if (ClsId == AEECLSID_OEMADDRBOOK)
   {
      // Allocate 
      pNew = (COEMAddrBook*)AEE_OEM_NEWCLASSEX((IBaseVtbl*)&gsOEMAddrBookFuncs,
                                          sizeof(COEMAddrBook), TRUE);
      if (!pNew)
      {
         Err = ENOMEMORY;
      }
      else
      {
         // Return
         if( OEMAddrBook_Init() )
         {
            Err = AEE_SUCCESS;
            *ppObj = (void*)pNew;
         }
         else
         {
            Err = EFAILED;
         }
      }
   }
   return Err;
}

static uint32  OEMAddr_AddRef(IOEMAddrBook * po)
{
   COEMAddrBook *pMe = (COEMAddrBook *)po;

   return(++(pMe->m_nRefs));
}

/*===========================================================================


===========================================================================*/
static uint32 OEMAddr_Release(IOEMAddrBook * po)
{
   COEMAddrBook *pMe = (COEMAddrBook *)po;
   if(pMe->m_nRefs)
   {
      if(--pMe->m_nRefs == 0)
      {
        OEMAddrBook_Exit();
        sys_free(po);
      }

   }
   return pMe->m_nRefs;
}

/*========================================================================
Function: OEMAddr_RecordAdd()

Description:
   This function adds a new record to the address book. The fields to be added into
   this record are passed as parameters to this function

Prototype:
   uint16  OEMAddr_RecordAdd(AEEAddrCat cat, AEEAddrField * pItems, int nItemCount,int *pErr)

Parameters:
   cat: [in]: Specifies the address category to which this record belongs to. If concept of
   categories is not supported, this parameter can be ignored.

   pItems: [in]: Pointer to an arry of items that must be added to the record. Each item contains
   information such as: FieldID, DataType, Data, DataLength. For detailed information about
   this structure please refer to AEEAddrBook.h or to the BREW API Reference Guide

   nItemCount: [in]: Number of fields in this record. It also indicates that the array
   pItems contains this many number of fields.

   pErr: [out]: If any error occurs, the error code must be placed into this pointer before
   returning from this function. OEMs must check for this parameter being NULL before storing
   the error value into this.

Return Value:
   If record was successfully added, this function must return the recordID of the newly 
   added record:
   If failed, this function must return AEE_ADDR_RECID_NULL and the parameter *pErr must
   contain the error code. A list of error codes can be found in AEEError.h

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
static uint16 OEMAddr_RecordAdd(AEEAddrCat cat, AEEAddrField * pItems, int nItemCount,int *pErr)
{
   //Stretch pItems into a single contiguous memory block so that we can
   //add into OEMDB

   byte *pBuf;
   uint16            wRecID;
   AEE_DBError       dbErr;
   word              wRecSize;
   int               nErr;

   if(!pItems)
      return(AEE_ADDR_RECID_NULL);

   dbErr = ValidateFieldData(cat, pItems, nItemCount);
   if( dbErr != AEE_SUCCESS)
   {
      if(pErr)
         *pErr = dbErr;

      return AEE_ADDR_RECID_NULL;
   }

   pBuf = AddressToRawData(cat,pItems,nItemCount,pErr,&wRecSize);
   if(!pBuf)
      return(AEE_ADDR_RECID_NULL);

   // Ensure record cache has been initialized
   GetCache();

   //Add record to database
   wRecID = OEM_DBRecordAdd(ghAddrBk,pBuf,wRecSize,&dbErr);

   //Free the buffer
   FREE(pBuf);

   if(wRecID == AEE_DB_RECID_NULL)
   {
      if(pErr)
         *pErr = dbErr;

      return(AEE_ADDR_RECID_NULL);
   }

   //Update cache
   nErr = InsertCache(wRecID,GetAddrFieldName(pItems,nItemCount,NULL));
   if(nErr != AEE_SUCCESS)
   {
      if(pErr)
         *pErr = nErr;
      OEMAddr_RecordDelete(wRecID);
      return(AEE_ADDR_RECID_NULL);
   }

   //Success.
   return(wRecID);
}


/*========================================================================
Function: OEMAddr_RecordUpdate()

Description:
   This function updates all the fields in the record of given ID. It replaces all the existing
   fields in that record with the fields being passed to this function.
   The recID indicates which record needs to be updated.

Prototype:
   int  OEMAddr_RecordUpdate(uint16 recID,AEEAddrCat cat,AEEAddrField * pItems, int nItemCount,int *pErr)

Parameters:
   recID: [in]: ID of the record whose fields need to be updated.

   cat: [in]: Specifies the address category to which this record belongs to. If concept of
   categories is not supported, this parameter can be ignored.

   pItems: [in]: Pointer to an arry of items that must be added to the record. Each item contains
   information such as: FieldID, DataType, Data, DataLength. For detailed information about
   this structure please refer to AEEAddrBook.h or to the BREW API Reference Guide

   nItemCount: [in]: Number of fields in this record. It also indicates that the array
   pItems contains this many number of fields.

   pErr: [out]: If any error occurs, the error code must be placed into this pointer before
   returning from this function. OEMs must check for this parameter being NULL before storing
   the error value into this.

Return Value:
   AEE_SUCCESS: If the record was successfully updated
   EFAILED: if failed

Comments:
   This function is quite similar to the function OEMAddr_RecordAdd() with the main difference
   being that this function updates all the fields of an already existing record while
   OEMAddr_RecordAdd() adds a new record to the address book.

Side Effects:
   None

See Also:
   OEMAddr_RecordAdd

========================================================================*/
static int OEMAddr_RecordUpdate(uint16 wrecID,AEEAddrCat cat,AEEAddrField * pItems, int nItemCount,int *pErr)
{
   byte        *pBuf;
   uint16      wRecSize;
   AEE_DBError dbErr;

   dbErr = ValidateFieldData(cat, pItems, nItemCount);
   if( dbErr != AEE_SUCCESS)
   {
      if(pErr)
         *pErr = dbErr;

      return EFAILED;
   }

   pBuf = AddressToRawData(cat,pItems,nItemCount,pErr,&wRecSize);
   if(!pBuf)
      return(EFAILED);

   OEM_DBRecordUpdate(ghAddrBk,wrecID,pBuf,wRecSize,&dbErr);

   //Free the buffer
   FREE(pBuf);

   if(dbErr != AEE_DB_ERR_NO_ERR)
   {
      if(pErr)
         *pErr = dbErr;
      return(EFAILED);
   }

   //Update Cache
   UpdateCache(wrecID,GetAddrFieldName(pItems,nItemCount,NULL));

   return(AEE_SUCCESS);
}


/*========================================================================
Function: OEMAddr_RecordGetByID()

Description:
   This function retrieves information of a record with given record ID. This function
   returns information about all the fields in this record. This function does NOT
   delete the record from the address book. It just reads the record and returns 
   information (data) of that record to the caller.

Prototype:
   int   OEMAddr_RecordGetByID(uint16 recID,AEEAddrCat *pcat,AEEAddrField ** ppItems, int *pnItemCount,int *pErr)

Parameters:
   recID: [in]: ID of the record whose fields need to be retrieved and returned

   pcat: [in/out]: On input, this is a valid pointer to AEEAddrCat. On return, this pointer
   points to the address category to which the record belongs to. 

   ppItems: [in/out]: Pointer for passing info about the fields. Memory for *ppItems must
   be allocated by the OEMs while implementing this function. This memory will be freed
   by the caller (BREW). Memory must be allocated by the OEMs using the function
   MALLOC(). 

   pnItemCount: [in/out]: On input, this is a valid pointer to an integer. On return,
   this contains the count of the number of fields present in this record. 
   It also indicates that the array  *ppItems contains this many number of fields on return.

   pErr: [out]: If any error occurs, the error code must be placed into this pointer before
   returning from this function. OEMs must check for this parameter being NULL before storing
   the error value into this.

Return Value:
   AEE_SUCCESS: If the record information was successfully retrieved
   EFAILED: if failed. In this case, *pErr must contain the exact error code.

Comments:
   Note that memory for *ppItems must be allocated by implementer of this function. This
   memory will be released by the caller.

Side Effects:
   None

See Also:
   None

========================================================================*/
static int OEMAddr_RecordGetByID(uint16 wrecID,AEEAddrCat *pcat,AEEAddrField ** ppItems, int *pnItemCount,int *pErr)
{
   byte *         pBuf;
   AEE_DBRecInfo  dbRecInfo;
   AEE_DBError    dbErr;
   int            nRet;

   pBuf = OEM_DBRecordGet(ghAddrBk,wrecID,&dbRecInfo,&dbErr);

   if(!pBuf)
   {
      if(pErr)
         *pErr = dbErr;
      return(EFAILED);
   }

   //Parse the buffer into individual address items
   nRet = RawDataToAddress(pBuf,dbRecInfo.wRecSize,pcat,ppItems,pnItemCount,pErr);
   
   //Free the buffer returned by OEM_DBRecordGet. 
   OEM_DBFree(ghAddrBk,pBuf,NULL);

   return(nRet);
}


/*=======================================================================

Function: OEMAddrBook_Init()

Description:
   This function is called when the BREW AddressBook interface is created. This allows
   the OEMs to do any address-book specific initialization

Prototype:
   boolean  OEMAddrBook_Init(void);

Parameters:
   None

Return Value:
   TRUE: If initialization succeeded
   FALSE: If initialization failed

Comments:
   If this function returns FALSE, BREW will not allow the creation of the IAddrBook
   interface and hence will now allow any access to the OEM Address Book.

Side Effects:
   None

See Also:
   None

========================================================================*/
static boolean OEMAddrBook_Init(void)
{
   //If address book already open, return TRUE
   if(ghAddrBk)
      return(TRUE);

   gpCache = NULL;
   gbEnumInit = FALSE;
   gbCacheInited = FALSE;
   gNoCache = TRUE;
   gwProps = AEE_ADDR_DEFAULT_PROPERTIES;
   
   //Initialize the search info pointers to our static buffers
   gEnumRecInfo.pCatList = gSearchCat;
   gEnumRecInfo.pFieldList = gSearchFieldID;
   gEnumRecInfo.pTypeList = gSearchDataType;

   //Open the address book if it already exists. If no, create it.
   ghAddrBk = OEM_DBOpen(AEE_ADDRBOOK_DIR"/"OEMADDRBOOK_NAME, NULL);
   
   if(ghAddrBk)
      return(TRUE);

   //AddressBook not found. Create it.
   ghAddrBk = OEM_DBCreate(AEE_ADDRBOOK_DIR"/"OEMADDRBOOK_NAME, OEMADDRBOOK_MINRECS, OEMADDRBOOK_MINRECSIZE, NULL);

   if(!ghAddrBk)
      return(FALSE);

   return (TRUE);
}


/*========================================================================
Function: OEMAddrBook_Exit()

Description:
   This function is called when the BREW AddressBook interface is deleted. This allows
   OEMs to do any address-book specific clean-up operations.

Prototype:
   void  OEMAddrBook_Exit(void);

Parameters:
   None

Return Value:
   None

Comments:
   If any memory has been allocated (say, iterators) during the address-book operation,
   this is the time to delete them.

Side Effects:
   None

See Also:
   None

========================================================================*/
static void OEMAddrBook_Exit(void)
{
   //Close the address book
   if(ghAddrBk)
   {
      OEM_DBClose(ghAddrBk,NULL);
      ghAddrBk = NULL;
   }
   // Free any record enumeration data we copied and stored
   FreeSearchData();

   // Free the cache
   ClearAddrBkCache();
}


/*========================================================================

Function: OEMAddr_RecordDelete()

Description:
   This function deletes a record of the specified record ID from the address book.

Prototype:
   int   OEMAddr_RecordDelete(uint16 recID)

Parameters:
   recID: [in]: ID of the record to be deleted from the address book.

Return Value:
   AEE_SUCCESS: If the record was successfully deleted
   EFAILED: if failed

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
static int OEMAddr_RecordDelete(uint16 wrecID)
{
   AEE_DBError dbErr;

   OEM_DBRecordDelete(ghAddrBk,wrecID,&dbErr);

   if(dbErr == AEE_DB_ERR_NO_ERR)
   {
      //Remove from Cache
      RemoveRecCache(wrecID);
      return(AEE_SUCCESS);
   }

   return(EFAILED);
}


/*========================================================================

Function: OEMAddr_GetNumRecs()

Description:
   This function returns the total number of records found in the address book.

Prototype:
   uint16   OEMAddr_GetNumRecs(void);

Parameters:
   None

Return Value:
   Count indicating the total number of records currentl found in the address book.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
static uint16 OEMAddr_GetNumRecs(void)
{
   GetCache();   
   if (gNoCache)
   {
      // Not using cache. Get it from DB.
      return(OEM_DBRecordCount(ghAddrBk, NULL));
   }

   //Get Count from Cache
   return(GetCacheRecCount());
}


/*========================================================================
Function: OEMAddr_RemoveAllRecs(void)

Description:
   This function deletes all records from the address book.

Prototype:
   int   OEMAddr_RemoveAllRecs(void)

Parameters:
   None

Return Value:
   AEE_SUCCESS: If all the records were deleted
   EUNSUPPORTED: if this function is not supported
   EFAILED: if failed. 

Comments:
   Since this is a sensitive operation, OEMs can decide not to support it. If this
   function is not supported, the value EUNSUPPORTED must be returned from this function.

Side Effects:
   None

See Also:
   None

========================================================================*/
static int OEMAddr_RemoveAllRecs(void)
{
   AEE_DBError dbErr;

   OEM_DBClose(ghAddrBk,NULL);
   OEM_DBDelete(AEE_ADDRBOOK_DIR"/"OEMADDRBOOK_NAME, &dbErr);

   if(dbErr == AEE_DB_ERR_NO_ERR)
   {
      //Clear the Cache
      ClearAddrBkCache();

      // Recreate AddressBook
      ghAddrBk = OEM_DBCreate(AEE_ADDRBOOK_DIR"/"OEMADDRBOOK_NAME, OEMADDRBOOK_MINRECS, OEMADDRBOOK_MINRECSIZE, NULL);
      if(!ghAddrBk)
         return(EFAILED);

      return(AEE_SUCCESS);
   }

   return(EFAILED);
}


/*========================================================================
Function: OEMAddr_EnumRecInit()

Description:
   This function is used for searching the address book for specific records. It is
   also used for sequentially retrieving all the records in the database.
   This fundtion initializes the enumeration of records in the address book
   based on a specific search criteria.  Once enumeration has been initialized, the function
   OEMAddr_EnumNextRec() will be used to iterate through the records that match
   this search criteria.

Prototype:
   int   OEMAddr_EnumRecInit(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize)

Parameters:
   wCategory: [in]: Category type to be matched. If this is set to AEE_ADDR_CAT_NONE, 
   it should be ignored.

   wFieldID: [in]: AEEAddrFieldID to be matched. If this is set to AEE_ADDRFIELD_NONE, 
   it should be ignored.
   It is likely that the OEMs do not allow searching for records on this field ID (ex: searching
   for records based on EMAIL may not be allowed). In this case, return EFAILED. 
   and IADDRBOOK_EnumNextFieldsInfo().
   
   pData: [in]: if non-null, it specifies the actual data that must be matched. 
   If NULL, it is ignored. For ex: if wFieldID is set to AEE_ADDRFIELD_NAME, pData will contain
   the actual name to be matched.  If wCategory is set to AEE_ADDRCAT_ENHANCED_ENUM
   and wFieldId is set to AEE_ADDRFIELD_ENHANCED_ENUM, then this field points
   to an AEEAddrEnumRecInfo structure.
   
   wDataSize: [in]: Specifies the size of pData

Return Value:
   AEE_SUCCESS: If enumeration was successfully initialized.
   EFAILED: Enumeration failed. 

Comments:
   This function can also be used to enumerate through all
   the records in the database by specifying AEE_ADDR_CAT_NONE for the category parameter and
   AEE_ADDRFIELD_NONE for the field paramter.

   When wCategory is set to AEE_ADDRCAT_ENHANCED_ENUM and wFieldId is set to
   AEE_ADDRFIELD_ENHANCED_ENUM, then pData points to an AEEAddrEnumRecInfo
   structure.  This allows the caller to specify a number of search match
   criteria for the record enumeration, including:
   - A list of categories to search
   - A list of fields to search
   - A list of data types to search
   - Specific data to match
   - Data match criteria such as case sensitivity and exact or partial string matches.

Side Effects:
   None

See Also:
   OEMAddr_EnumNextRec

========================================================================*/
static int OEMAddr_EnumRecInit(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize)
{
   int nErr;
   
   gbEnumInit = FALSE;

   //Free data from previous enum
   FreeSearchData();

   // Make a local copy of the calling application's enumeration
   // initialization data.  If this is and enhanced search we 
   // just copy the AEEAddrEnumRecInfo info passed.  Otherwise, we must
   // convert the args passed to us to the AEEAddrEnumRecInfo format for
   // use by OEMAddr_EnumNextRec().
   if( AEE_ADDRCAT_ENHANCED_ENUM == wCategory &&
       AEE_ADDRFIELD_ENHANCED_ENUM == wFieldID )
   {
      nErr = CopyEnhancedEnumData(pData);
   }
   else
   {
      nErr = ConvertToEnhancedEnumData(wCategory, wFieldID, pData, wDataSize);
   }

   if( nErr != AEE_SUCCESS )
      return nErr;

   if( ! IsRecEnumDataValid(&nErr) )
      return nErr;

   gbEnumInit = TRUE;
   gwCurRecID = AEE_DB_RECID_NULL;
   GetCache();

   // Searches of only AEE_ADDRFIELD_NAME are special since the names
   // and associated record IDs are cached.
   gbNameSearch = gEnumRecInfo.nFields == 1 &&
                  gEnumRecInfo.pFieldList[0] == AEE_ADDRFIELD_NAME;

   return(AEE_SUCCESS);
}


/*========================================================================
Function: OEMAddr_EnumNextRec()

Description:
   This function returns the information about the next record based on the search criteria
   specified in most recent call to OEMAddr_EnumRecInit()

Prototype:
   uint16   OEMAddr_EnumNextRec(AEEAddrCat *cat,AEEAddrField ** ppItems, int *nItemCount,int *pErr);

Parameters:
   cat: [in/out]: On return, if the next record was found, this contains the address category of
   that next record.

   ppItems: [in/out]: On return, if the next record was found, *ppItems contains the 
   list of address fields found in that next record.
   
   nItemCount: [in/out]: On return, if the next record was found, *nItemCount contains the 
   number of address fields found in that next record.

  *pErr: [in/put: On return, contains the error code if an error occurred.


Return Value:
   If next record was successfully found, this function must return the recordID of the newly 
   found record. Also, it must fill up the incoming parameters with the contents of the
   newly found record. 
   If the end of the enumeration has been reached or no more records are found, 
   this function must return AEE_ADDR_RECID_NULL. .

Comments:
   Once the end of the enumeration has been reached,
   the index must NOT be reset to point to the begining of the enumeration. This means, once
   the end has been reached, all subsequent calls to this function will continue to return
   AEE_ADDR_RECID_NULL. The caller must call OEMAddr_EnumRecInit() to re-initialize the
   search criteria

Side Effects:
   None

See Also:
   OEMAddr_EnumRecInit

============================================================================= */
static uint16 OEMAddr_EnumNextRec(AEEAddrCat *pcat,AEEAddrField ** ppItems, int *pnItemCount,int *pErr)
{
   uint16         wRecID;
   AEE_DBError    dbErr;

   *pErr = AEE_SUCCESS;

   //Check if Enum initialized 
   if(!gbEnumInit)
      return(AEE_ADDR_RECID_NULL);

   if(!pcat || !ppItems || !pnItemCount)
   {
      if(pErr)
         *pErr = EBADPARM;
      return(AEE_ADDR_RECID_NULL);
   }

   //Get the next record that matches the given search criteria
   while(TRUE)
   {
      if( gNoCache )
      {
         wRecID = OEM_DBRecordNext(ghAddrBk,gwCurRecID,&dbErr);
      }
      else
      {
         if( gbNameSearch )
            wRecID = GetNextMatchInCache(gwCurRecID);
         else
            wRecID = GetNextRecInCache(gwCurRecID);
      }

      if(wRecID == AEE_DB_RECID_NULL)
      {
         gbEnumInit = FALSE;
         return(AEE_ADDR_RECID_NULL);  // no more records
      }

      gwCurRecID = wRecID;

      //Record found. See if matches search criteria
      if( IsSearchMatch(wRecID,pcat,ppItems,pnItemCount,pErr) )
      {
         return wRecID;
      }

      if( AEE_SUCCESS != *pErr )
         return AEE_ADDR_RECID_NULL;
   }
}


/*========================================================================
Function: OEMAddrBook_GetCatCount()

Description:
   This function returns the number of address-categories supported by the
   address book. Examples of address-categories are: PERSONAL, BUSINESS, etc.
   Each record in the address book will (may) belong to a specific address category.
   If the concept of categories are not supported in the address book, this function
   must return 0.

Prototype:
   int   OEMAddr_GetCatCount(void)

Parameters:
   None

Return Value:
   Number of categories supported

Comments:
   Its perfectly valid to return 0 to this function if the address book does not
   support the concept of categories for each record

Side Effects:
   None

See Also:
   None

========================================================================*/
static int OEMAddr_GetCatCount(void)
{
   return(OEM_ADDRESS_CATEGORY_COUNT);
}

/*========================================================================
Function: OEMAddr_GetCatList()

Description:
   This function returns information about all the address categories supported
   by the handset address book. This function is called onle if OEMAddr_GetCatCount()
   returned a value other than 0.

Prototype:
   int   OEMAddr_GetCatList(AEEAddrCat *p, int nSize);

Parameters:
   p: [in/out]: Pointer (allocated by the caller) that can hold information about the
   address categories.
   nSize: Numnber of AEEAddrCat elements that can fit into the array pointed to by p.

Return Value:
   AEE_SUCCESS: If successful. Even if nSize is less than the total number of categories
   supported, this function must return AEE_SUCCESS as long as nSize > 0.

   EFAILED: If failed.

Comments:
   The categories must be converted from OEM list to the AEE values before returning. A list
   of pre-defined AEEAddressCategories can be found in AEEAddrBook.h. OEMs can add their
   own categories too.

Side Effects:
   None

See Also:
   None

========================================================================*/
static int OEMAddr_GetCatList(AEEAddrCat *p, int nSize)
{
   int i;
   if(!p || !nSize)
      return(EFAILED);

   for(i = 0; i < OEM_ADDRESS_CATEGORY_COUNT && i < nSize; i++)
   {
      switch(i)
      {
      case 0:
         p[i] = AEE_ADDR_CAT_PERSONAL;
         break;

      case 1:
         p[i] = AEE_ADDR_CAT_BUSINESS;
         break;

      default:
         break;
      }
   }
   return(AEE_SUCCESS);
}

/*======================================================================= 
Function: OEMAddr_GetCategoryName()

Description: 
   This method is used to get the AECHAR string name of a category.

Prototype:
   int   OEMAddr_GetCategoryName(AEEAddrCat c, AECHAR *pszName, int *pnSize);
   
Parameters:
   c: [in]: Address category for which the name in string format is returned.
   pszName: [in/out]: On input this is the buffer where the category name will
   be placed.  If NULL, this indicates the caller is querying the size.
   required.  On return, the buffer will contain the category name.
   pnSize: [in/out]: On input contains the size of pszName. On return, it 
   contains the size filled.

Return Value: 
   AEE_SUCCESS: If successful
   EBADPARM:    If bad parameters are passed in
   AEE_ADDR_CAT_NOT_SUPPORTED: If the category is not supported.
   AEE_ADDRBOOK_NOCATSUPPORT:  If the device does not support categories.

Comments:  
   None

Side Effects: 
   None

See Also: 
   OEMAddr_GetCatCount
   OEMAddr_GetCatList
   OEMAddr_GetFieldName   
=======================================================================*/
static int OEMAddr_GetCategoryName(AEEAddrCat c, AECHAR *pszName, int *pnSize)
{
   int i;
   int n;
   char *psz = NULL;

   if(!pnSize)
      return EBADPARM;

   // Search for the passed category
   for(i = 0; i < OEM_ADDRESS_CATEGORY_COUNT; i++)
   {
      if(gSupportedCatList[i].cat == c)
      {
         psz = gSupportedCatList[i].pszName;
         break;
      }
   }

   // If found, determine the name or buffer size required.
   if(psz)
   {
      n = sizeof(AECHAR)*(STRLEN(psz) + 1);
      if( pszName )
      {
         *pnSize = MIN(*pnSize, n);
         STRTOWSTR(psz, pszName, *pnSize);
      }
      else
      {
         *pnSize = n;
      }
      return AEE_SUCCESS;
   }

   // If we fall through to here, the category was not found.
   return AEE_ADDR_CAT_NOT_SUPPORTED;
}

/*========================================================================
Function: OEMAddr_GetFieldInfoCount()

Description:
   This function returns the number of types of fields supported for the give category.
   If the concept of categories is not supported, this function must just
   return the total number of types of fields supported for each record in the address book.
   Examples of fields are: NAME, WORK_NUM, FAX_NUM, URL, ADDRESS, etc.

Prototype:
   int   OEMAddr_GetFieldInfoCount(AEEAddrCat c)

Parameters:
   c: [in]: Address Category for which the number of fields supported is to be
   returned.

Return Value:
   Number of types of fields supported for the given address category.

Comments:
   None

Side Effects:
   None

See Also:
   OEMAddr_GetFieldInfo()

========================================================================*/
static int OEMAddr_GetFieldInfoCount(AEEAddrCat c)
{
   return(OEM_ADDRESS_FIELDS_COUNT);
}


/*========================================================================
Function: OEMAddr_GetFieldInfo()

Description:
   This function returns detiled information about each field-type supported
   for the given category. This function is generally invoked righ after the
   function OEMAddr_GetFieldInfoCount().

Prototype:
   int   OEMAddr_GetFieldInfo(AEEAddrCat c, AEEAddrFieldInfo *pf, int nSize)

Parameters:
   c: [in]: Address Category for which the field info is to be returned
   pf: [in/size]: Pointer to an arry of AEEAddrFieldInfo structs (allocated by caller)
   where information needs to be returned by this function
   nSize: [in]: Number of AEEAddrFieldInfo elements that can fit into the array pointed
   to by pf

Return Value:
   AEE_SUCCESS: If this function was able to successfuly send information about the fields.
   Even if nSize is less than the total number of fields supported, 
   this function must return AEE_SUCCESS as long as nSize > 0.
   EFAILED: If failed.

Comments:
   AEEAddrFieldInfo contains detailed information about the fields. It contains information
   such as FieldID, maximum number of fields of this ID supported in each record, etc.
   Detailed information of this structure can be found in AEEAddrBook.h and in the
   BREW API Reference Guide.

Side Effects:
   None

See Also:
   None

========================================================================*/
static int OEMAddr_GetFieldInfo(AEEAddrCat c, AEEAddrFieldInfo *pf, int nSize)
{
   int i;

   if(!pf)
      return(EBADPARM);

   for(i = 0; i < OEM_ADDRESS_FIELDS_COUNT && i < nSize; i++)
   {
      pf[i].fieldID = gSupportedFieldList[i].afi.fieldID;
      pf[i].nMaxItems = gSupportedFieldList[i].afi.nMaxItems;
      pf[i].fType = gSupportedFieldList[i].afi.fType;
      pf[i].bFieldSearch = gSupportedFieldList[i].afi.bFieldSearch;
      pf[i].bFieldCatSearch = gSupportedFieldList[i].afi.bFieldCatSearch;
   }
   return(AEE_SUCCESS);
}


/*======================================================================= 
Function: OEMAddr_GetFieldName()

Description: 
   This method is used to get the AECHAR string name of a field.

Prototype:
   int OEMAddr_GetFieldName(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize)
   
Parameters:
   f: [in]: AEEAddrFieldID for which the name in string format is returned.
   pszName: [in/out]: On input this is the buffer where the field name will
   be placed.  If NULL, this indicates the caller is querying the size.
   required.
   pnSize: [in/out]: On input contains the size of pszName. On return, it 
   contains the size filled.

Return Value: 
   AEE_SUCCESS: If successful
   EBADPARM:    If bad parameters are passed in
   AEE_ADDR_INVALID_FIELD_ID: If the field is not supported.

Comments:  
   None

Side Effects: 
   None

See Also: 
   OEMAddr_GetFieldInfoCount
   OEMAddr_GetFieldInfo
   OEMAddr_GetCategoryName
=======================================================================*/
static int OEMAddr_GetFieldName(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize)
{
   int i;
   int n;
   char *psz = NULL;

   if(!pnSize)
      return EBADPARM;

   // Search for the passed field
   for(i = 0; i < OEM_ADDRESS_FIELDS_COUNT; i++)
   {
      if(gSupportedFieldList[i].afi.fieldID == f)
      {
         psz = gSupportedFieldList[i].pszName;
         break;
      }
   }

   // If found, determine the name or buffer size required.
   if(psz)
   {
      n = sizeof(AECHAR)*(STRLEN(psz) + 1);
      if( pszFieldName )
      {
         *pnSize = MIN(*pnSize, n);
         STRTOWSTR(psz, pszFieldName, *pnSize);
      }
      else
      {
         *pnSize = n;
      }
      return AEE_SUCCESS;
   }

   // If we fall through to here, the field was not found.
   return AEE_ADDR_INVALID_FIELD_ID;
}


/*========================================================================

Function: OEMAddr_SetProperties()

Description:
   This function sets the properties for the address book.

Prototype:
   void OEMAddr_SetProperties(uint32 dwProps);

Parameters:
   dwProps: [in]: Properties

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
static void OEMAddr_SetProperties(uint32 dwProps)
{
   gwProps = dwProps;
}


/*========================================================================

Function: OEMAddr_GetProperties()

Description:
   This function gets the properties for the address book.

Prototype:
   uint32 OEMAddr_GetProperties();

Parameters:
   None
   
Return Value:
   Properties

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
static uint32 OEMAddr_GetProperties(void)
{
   return gwProps;
}

//###########################################################################
//                            Helper functions
//###########################################################################


/*===========================================================================

FUNCTION: OEM_FreeAddressItems

DESCRIPTION
  Utility Function to free memory associated with the given address fields

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_FreeAddressItems(AEEAddrField *pItems,int nItemCount)
{
   int i;

   if(!pItems || !nItemCount)
      return;

   for(i = 0; i < nItemCount; i++)
   {
      if(pItems[i].pBuffer) 
         FREE(pItems[i].pBuffer);
   }

   FREE(pItems);
}

/*===========================================================================

FUNCTION: AddressToRawData

DESCRIPTION
  Helper function to convert a given set of address fields into a single
  byte buffer

DEPENDENCIES
  none

RETURN VALUE
  If successful, a byte* that contains all the address elements

SIDE EFFECTS
  none
===========================================================================*/
static byte* AddressToRawData(AEEAddrCat cat, AEEAddrField *pItems,int nItemCount,int *pErr,uint16 *pSize)
{
   byte             *pDst,*pBuf,*pSrc;
   int               i;
   uint16            wRecSize = 0;
   AEEAddrFieldStore *ph;
   AEEAddrCat        *pAddrCat;

   //First field is always category
   wRecSize += sizeof(AEEAddrCat);

   pBuf = NULL;

   //Compute size needed to store each field
   for(i = 0; i < nItemCount; i++)
   {
      wRecSize += sizeof(AEEAddrFieldStore);
      wRecSize += pItems[i].wDataLen;

      // Make sure we allow for alignment changes...
      if(wRecSize & 1)
         wRecSize++;
   }


   //Copy data into contiguous buffer
   pBuf = (byte *)MALLOC(wRecSize);
   pDst = pBuf;

   if(!pBuf)
   {
      if(pErr)
         *pErr = ENOMEMORY;
      return(NULL);
   }


   //Store the address category
   pAddrCat = (AEEAddrCat*)pDst;
   *pAddrCat = cat;

   pDst += sizeof(AEEAddrCat);

   //Copy individual fields
   for(i = 0; i < nItemCount; i++)
   {
      //Store the data for each field at the end

      ph = (AEEAddrFieldStore *)pDst;
      
      ph->fID = pItems[i].fID;
      ph->fType = pItems[i].fType;
      ph->wDataLen = pItems[i].wDataLen;

      pDst += sizeof(AEEAddrFieldStore);
      pSrc = (byte*)pItems[i].pBuffer;

      MEMCPY(pDst, pSrc, pItems[i].wDataLen);

      //Point to next field. 
      pDst += pItems[i].wDataLen;

      //If we are on odd boundary, allign it
      if((uint32)pDst & 1)
         pDst++;
   }

   if(pSize)
      *pSize = wRecSize;

   return(pBuf);

}


/*===========================================================================

FUNCTION: RawDataToAddress

DESCRIPTION
  Helper function to convert a given raw buffer into the individual address
  fields.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If succeeded
  EFAILED: if failed

SIDE EFFECTS
  none
===========================================================================*/
static int RawDataToAddress(byte *pBuf,
                            word wRecSize, 
                            AEEAddrCat *pcat, 
                            AEEAddrField** ppItems,
                            int *pnItemCount, 
                            int *pErr)
{

   AEEAddrFieldStore *pfStore;
   AEEAddrField      *pFields, *pTmpField;
   int               nCount = 0;
   word              wSizeProcessed = 0;
   AEEAddrCat*       pCatStore;

   if(!pBuf || !ppItems || !pnItemCount)
      return(EFAILED);

   //Extract the category
   pCatStore = (AEEAddrCat*)pBuf;
   if(pcat)
      *pcat = *pCatStore;
   
   pBuf += sizeof(AEEAddrCat);

   pFields = *ppItems = NULL;

   wSizeProcessed += sizeof(AEEAddrCat);

   while(wSizeProcessed < wRecSize)
   {
      pfStore = (AEEAddrFieldStore*)pBuf;
   
      if(!pFields)
         pTmpField = (AEEAddrField*)MALLOC(sizeof(AEEAddrField));
      else
         pTmpField = (AEEAddrField*)REALLOC(pFields,(nCount+1)*sizeof(AEEAddrField));

      if(!pTmpField)
      {
         //Free already allocated memory
         OEM_FreeAddressItems(pFields,nCount);
         return(ENOMEMORY);
      }

      pFields = pTmpField;

      //Position it to current field
      pTmpField = pFields + nCount;

      pTmpField->fID = pfStore->fID;
      pTmpField->fType = pfStore->fType;

      //Read the data at the end. This is how it has been stored in the raw buffer.
      pTmpField->wDataLen = pfStore->wDataLen;

      pBuf += sizeof(AEEAddrFieldStore);
      wSizeProcessed += sizeof(AEEAddrFieldStore);

      pTmpField->pBuffer = MALLOC(pTmpField->wDataLen);
      if(!pTmpField->pBuffer)
      {
         OEM_FreeAddressItems(pFields,nCount);
         return(ENOMEMORY);
      }
      MEMCPY(pTmpField->pBuffer,pBuf,pTmpField->wDataLen);

      nCount++;
      
      //Advance pBuf
      pBuf += pTmpField->wDataLen;
      wSizeProcessed += pTmpField->wDataLen;

      if((uint32)pBuf & 0x1)
      {
         pBuf++;
         wSizeProcessed++;
      }

   }
   
   *ppItems = pFields;
   *pnItemCount = nCount;

   return(AEE_SUCCESS);
}


/*===========================================================================

FUNCTION: ValidateFieldData

DESCRIPTION
  Helper function to verify field data passed to RecordAdd or RecordUpdate
  is valid.

  NOTE: This implementation assumes that all categories have identical fields.
        If your implementation has different allowed fields for different 
        categories, you must make two changes to this function:
        
        (1) Replace OEM_ADDRESS_FIELDS_COUNT with the value from
            OEMAddr_GetFieldInfoCount.

        (2) Change gSupportedFieldList to contain associated category data
            or convert the implementation from this list-based example to
            a dynamic set of switch/case statements.

DEPENDENCIES
  None

RETURN VALUE
  AEE_SUCCESS if all field data is valid.
  ENOMEMORY if malloc fails
  AEE_ADDR_INVALID_REC if too many items of any one field type
  AEE_ADDR_INVALID_FIELD_ID if an item contain an invalid field type

SIDE EFFECTS
  none
===========================================================================*/
static int ValidateFieldData(AEEAddrCat cat, AEEAddrField *pItems,int nItemCount)
{
   int i;
   int j;
   int FieldCount[OEM_ADDRESS_FIELDS_COUNT];

   // Zero field count
   for( i = 0; i < OEM_ADDRESS_FIELDS_COUNT; i++ )
      FieldCount[i] = 0;

   // Loop across all field items
   for(i = 0; i < nItemCount; i++)
   {
      // Check if the field is supported and if the number of items of this
      // type is <= max allowed
#ifdef OEMADDRBOOK_RESTRICT_FIELDS
      boolean bFound = FALSE;
#endif
      for( j = 0; j < OEM_ADDRESS_FIELDS_COUNT; j++ )
      {
         if( pItems[i].fID == gSupportedFieldList[j].afi.fieldID )
         {
            FieldCount[j]++;
            if( FieldCount[j] > gSupportedFieldList[j].afi.nMaxItems )
            {
               return AEE_ADDR_INVALID_REC;
            }
            if (AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE == pItems[i].fID)
            {
               uint16 nFieldIDPrimaryPhone = *(uint16*)(pItems[i].pBuffer);
               switch (nFieldIDPrimaryPhone)
               {
               case AEE_ADDRFIELD_PHONE_WORK:
               case AEE_ADDRFIELD_PHONE_HOME:
               case AEE_ADDRFIELD_PHONE_FAX:
               case AEE_ADDRFIELD_PHONE_VOICE:
               case AEE_ADDRFIELD_PHONE_PREF:
               case AEE_ADDRFIELD_PHONE_CELL:
               case AEE_ADDRFIELD_PHONE_PAGER:
               case AEE_ADDRFIELD_PHONE_GENERIC:
               case AEE_ADDRFIELD_PHONE_OTHER:
                  break;
               default:
                  return AEE_ADDR_INVALID_FIELD_VALUE;
               }

            }

#ifdef OEMADDRBOOK_RESTRICT_FIELDS
            bFound = TRUE;
#endif
            break;
         }
      }
#ifdef OEMADDRBOOK_RESTRICT_FIELDS
      if( !bFound )
      {
         return AEE_ADDR_INVALID_FIELD_ID;
      }
#endif
   }

   return AEE_SUCCESS;
}


/*===========================================================================

FUNCTION: IsSearchMatch

DESCRIPTION
  Helper function to check if the given record matches the current
  search criteria

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if it matches

SIDE EFFECTS
  none
===========================================================================*/
static boolean IsSearchMatch(uint16 wRecID,AEEAddrCat *pcat,AEEAddrField ** ppItems, int *pnCount,int *pErr)
{
   int            i,nRet;
   AEEAddrField   *pField;

   nRet = OEMAddr_RecordGetByID(wRecID,pcat,ppItems, pnCount,pErr);

   pField = *ppItems;

   if(nRet == AEE_SUCCESS)
   {
      // See if the record matches the search critera set up by EnumInit
      if( IsCategorySearchMatch(*pcat) )
      {
         for(i = 0; i < *pnCount; i++)
         {
            if( IsFieldIDSearchMatch(pField[i].fID) && 
                IsFieldTypeSearchMatch(pField[i].fType) &&
                IsDataSearchMatch(pField[i].pBuffer, pField[i].wDataLen) )
            {
               return TRUE;
            }
         }
      }

      // No match was made.  Free the record we retreived.
      OEM_FreeAddressItems(*ppItems,*pnCount);
   }

   return FALSE;
}


/*===========================================================================

FUNCTION: IsSearchAllowedInCategory

DESCRIPTION
  Helper function to check if search is allowed in the specified category.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: if search is allowed

SIDE EFFECTS
  none
===========================================================================*/
static boolean IsSearchAllowedInCategory(AEEAddrCat wCategory)
{
   //Allow search on any combination of Category, FieldID and FieldValue.
   return(TRUE);
}


/*===========================================================================

FUNCTION: IsSearchAllowedInField

DESCRIPTION

  Helper function to check if search is allowed in the specified field.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: if search is allowed

SIDE EFFECTS
  none
===========================================================================*/
static boolean IsSearchAllowedInField(AEEAddrFieldID wFieldID)
{
   //Allow search on any combination of Category, FieldID and FieldValue.
   return(TRUE);
}


/*===========================================================================

FUNCTION: ClearAddrBkCache

DESCRIPTION
  This function clears the Cache and frees all memory associated with it.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if search is allowed

SIDE EFFECTS
  none
===========================================================================*/
static int ClearAddrBkCache()
{
   AddrBkCache *ptr;

   gbCacheInited = FALSE;
   if(!gpCache)
      return(AEE_SUCCESS);

   while(gpCache)
   {
      ptr = gpCache;
      gpCache = ptr->pNext;
      if(ptr->szName)
         sys_free(ptr->szName);
      sys_free(ptr);
   }
   
   return(AEE_SUCCESS);
}


/*===========================================================================

FUNCTION: AddrBkCompareName

DESCRIPTION
  This function is invoked to compare the two names. The results of this function
  are used in sorting the entries in the Cache.
  If OEMs want to change the logic to do comparison, this function must be modified.
  As long as the return value rules are followed, there is no impact to any
  other part of this code.

DEPENDENCIES
  none

RETURN VALUE
  0 :if both strings are same
  >0: if str1 > str2
  <0: if str1 < str2

SIDE EFFECTS
  none
===========================================================================*/
static int AddrBkCompareName(AECHAR *str1,AECHAR *str2)
{
   if(str1 && !str2)
      return(1);

   if(!str1 && str2)
      return(-1);

   if(!str1 && !str2)
      return(0);

   return(WSTRICMP(str1,str2));

}


/*===========================================================================

FUNCTION: InsertCache

DESCRIPTION
  This function inserts the given record into the right slot inside the cache.
  It ensures that the cache remains sorted in ascending order by name. If the
  szName parameter is NULL (i.e. record does not have a name), the record 
  is placed at the beginning of the cache. This means, all untitled records
  will be at the beginning of the cache. Also, the most recently added
  untitled record will be at the start of the cache.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if succeeded

SIDE EFFECTS
  none
===========================================================================*/
static int InsertCache(uint16 wRecID, AECHAR *szName)
{
   AddrBkCache *pNew,*prev,*cur;
   int nSize;

   GetCache();
   if (gNoCache)
      // Not using cache
      return(AEE_SUCCESS);

   //The cache data must be valid when switching between apps. Hence,
   //use sys_malloc

   pNew = (AddrBkCache*)sys_malloc(sizeof(AddrBkCache));
   if(!pNew)
      return(ENOMEMORY);
   
   pNew->wRecID = wRecID;

   
   //If Name not given, insert at head of list
   if(!szName)
   {
      pNew->szName = NULL;
         pNew->pNext = gpCache;
         gpCache = pNew;
      return(AEE_SUCCESS);
   }


   //Name is specified. Insert at right spot
   nSize = (WSTRLEN(szName)+1)*sizeof(AECHAR);
   pNew->szName = (AECHAR*)sys_malloc(nSize);
   if(!pNew->szName)
   {
      sys_free(pNew);
      return(ENOMEMORY);
   }
   WSTRCPY(pNew->szName,szName);
   
   
   if(!gpCache)   //Empty list.
   {
      pNew->pNext = NULL;
      gpCache = pNew;
      return(AEE_SUCCESS);
   }


   //Insert at appropriate position
   prev = NULL;
   cur = gpCache;

   while(cur)
   {
      //Ask OEMs how to sort.
      if(AddrBkCompareName(szName,cur->szName) >0)
      {
         prev = cur;
         cur = cur->pNext;
      }
      else
      {
         if(prev)
         {
            pNew->pNext = prev->pNext;
            prev->pNext = pNew;
            return(AEE_SUCCESS);
         }
         else
         {
            //Insert at beginning of list
            pNew->pNext = gpCache;
            gpCache = pNew;
            return(AEE_SUCCESS);
         }
      }
   }
   //Insert at end
   prev->pNext = pNew;
   pNew->pNext = NULL;
   return(AEE_SUCCESS);
}

/*===========================================================================

FUNCTION: InitAddrBkCache

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
static int InitAddrBkCache()
{
   uint16         prevRec,curRec;
   AEE_DBError    dbErr;
   int            i,nRet,nCount,nErr, nMaxRecs, nRecs;
   AEEAddrField   *pFields;
   AEEAddrCat     cat;

   //Clear existing cache, if any
   ClearAddrBkCache();
   
   //Set this immediately so GetCache() works properly
   gbCacheInited = TRUE; 

   //Read each record in the address book and place its ID and Name in the appropriate
   //place in the cache
   prevRec = AEE_DB_RECID_NULL;

   //get the total number of records in address book
   nMaxRecs=0;
   nRecs=0;
   nMaxRecs = 0xFFFF & OEM_DBRecordCount(ghAddrBk, NULL);

   while(TRUE)
   {     
      curRec = OEM_DBRecordNext(ghAddrBk,prevRec,&dbErr);
      
      if(curRec == AEE_DB_RECID_NULL)
      {
         //we should always return from here
         return(AEE_SUCCESS);  // no more records
      }
      ++nRecs;

      if (nRecs!=0 && nRecs > nMaxRecs)
      {
         //this means we tried to get more records than OEM_DBRecordCount mentioned were present
         ClearAddrBkCache();
         return(EFAILED);
      }
      
      nRet = OEMAddr_RecordGetByID(curRec,&cat,&pFields, &nCount,&nErr);
      if(nRet == AEE_SUCCESS)
      {

         //Find Name
         for(i = 0; i < nCount; i++)
         {
            if(pFields[i].fID == AEE_ADDRFIELD_NAME)
               break;
         }
         if(i == nCount)
            nRet = InsertCache(curRec,NULL); //No Name.
         else
            nRet = InsertCache(curRec,(AECHAR*)(pFields[i].pBuffer));
  
         //Free mem returned by OEMAddr_RecordGetByID
         OEM_FreeAddressItems(pFields,nCount);

         if(nRet != AEE_SUCCESS)
         {
            //Fail to insert into Cache
            ClearAddrBkCache();
            return(EFAILED);
         }
      }  //Data read successfully from AddressBook

      prevRec = curRec;
   } // Process next record
}


/*===========================================================================

FUNCTION: GetAddrFieldName

DESCRIPTION
  This function searches for the "Name" in the address-record and returns the name.

DEPENDENCIES
  none

RETURN VALUE
  Name, if found
  NULL, if record does not have a Name
  if pIndex is NON-NULL, returns the index in *pIndex.

SIDE EFFECTS
  none
===========================================================================*/
static AECHAR *GetAddrFieldName(AEEAddrField *pItems,int nCount,int *pIndex)
{
   int i;
   if(!pItems || !nCount)
      return(NULL);
 
   for(i = 0; i < nCount; i++)
   {
      if(pItems[i].fID == AEE_ADDRFIELD_NAME)
      {
         if(pIndex)
            *pIndex = i;
         return((AECHAR*)(pItems[i].pBuffer));
      }
   }
   return(NULL);
}


/*===========================================================================

FUNCTION: RemoveRecCache

DESCRIPTION
  This function removes the given record from the Cache

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if successful

SIDE EFFECTS
  none
===========================================================================*/
static int RemoveRecCache(uint16 wRecID)
{
   AddrBkCache   *pCur,*prev,*pRemove;

   GetCache();
   if (gNoCache)
      // Not using cache
      return(AEE_SUCCESS);

   prev = NULL;
   pCur = gpCache;

   while(pCur)
   {
      if(pCur->wRecID == wRecID)
      {
         //Match Found. 
         pRemove = pCur;   //Store for freeing later
         if(prev) // Not the first node
         {
            prev->pNext = pCur->pNext;
            FreeAddrCacheElem(pRemove);
            gwCurRecID = prev->wRecID;
            return(AEE_SUCCESS);
         }
         else
         {
            //First node to be freed
            gpCache = gpCache->pNext;
            FreeAddrCacheElem(pRemove);
            gwCurRecID = AEE_DB_RECID_NULL;
            return(AEE_SUCCESS);
         }
      }
      else
      {
         //Traverse further to look for the node
         prev = pCur;
         pCur = pCur->pNext;
      }
   }
   gwCurRecID = AEE_DB_RECID_NULL;
   return(AEE_SUCCESS);
}

/*===========================================================================

FUNCTION: GetCacheName

DESCRIPTION
  This function returns the name stored in cache for this record ID

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if successful

SIDE EFFECTS
  none
===========================================================================*/
static AECHAR *GetCacheName(uint16   recID)
{
   AddrBkCache *pCur = gpCache;

   GetCache();
   while(pCur)
   {
      if(pCur->wRecID == recID)
         return(pCur->szName);

      pCur = pCur->pNext;
   }
   return(NULL);  //Record not found
}


/*===========================================================================

FUNCTION: UpdateCache

DESCRIPTION
  This function updates the cache entry for the given record. It first checks
  to see if the name has changed. If it has, then the name in the cache is updated

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if successful

SIDE EFFECTS
  none
===========================================================================*/
static int UpdateCache(uint16 wrecID,AECHAR *szName)
{
   //First Check if the name has changed from what we have already cached. If name
   //has not changed, nothing to do.
   AECHAR *szOldName;

   szOldName = GetCacheName(wrecID);   //Current Name

   //If both are NULL or if both are Non-NULL and their names are the same, nothing to do.
   if((szOldName == NULL && szName == NULL) ||
      ((szOldName != NULL && szName != NULL) &&
      !WSTRICMP(szOldName,szName)))
   {
      //No change to name. Nothing to do.
      return(AEE_SUCCESS);
   }

   //Name changed. delete and add
   RemoveRecCache(wrecID);

   return(InsertCache(wrecID,szName));
}


/*===========================================================================

FUNCTION: GetNextRecInCache

DESCRIPTION
  This function retrieves the next record in the Cache following the given
  record ID. This function is used while enumerating the records
  in the database.

DEPENDENCIES
  none

RETURN VALUE
  Record ID of next record, if successful
  AEE_DB_RECID_NULL: If no more records found
  if incoming record ID is set to AEE_DB_RECID_NULL, this function returns
  the first record ID.

SIDE EFFECTS
  none
===========================================================================*/
static uint16 GetNextRecInCache(uint16 recID)
{
   AddrBkCache*   ptr;

   if(!gpCache)
      return(AEE_DB_RECID_NULL);

   //if incoming record ID is set to AEE_DB_RECID_NULL, this function returns
  //the first record ID.

   if(recID == AEE_DB_RECID_NULL)
      return(gpCache->wRecID);   //Return first record


   ptr = gpCache;
   while(ptr)
   {
      if(ptr->wRecID == recID)
      {
         if(ptr->pNext)
            return(ptr->pNext->wRecID);
         else
            return(AEE_DB_RECID_NULL); //No more records
      }
      ptr = ptr->pNext;
   }
   return(AEE_DB_RECID_NULL);
}


/*===========================================================================

FUNCTION: GetNextMatchInCache

DESCRIPTION
  This function retrieves the next record in the Cache that matches the given name.
  following the given recID. If recID == AEE_DB_RECID_NULL, start
  search from first record.

DEPENDENCIES
  none

RETURN VALUE
  Record ID of next record, if successful
  AEE_DB_RECID_NULL: If no more records found
  if incoming record ID is set to AEE_DB_RECID_NULL, this function starts searching from
  the first record ID.

SIDE EFFECTS
  none
===========================================================================*/
static uint16 GetNextMatchInCache(uint16 recID)
{

   AddrBkCache*   ptr, *pTemp;

   if(!gpCache)
      return(AEE_DB_RECID_NULL);

   ptr = gpCache;

   while(ptr)
   {
      pTemp = NULL;

      if(recID == AEE_DB_RECID_NULL)
         pTemp = ptr;
      else if(ptr->wRecID == recID)
         pTemp = ptr->pNext;

      if(pTemp)
      {
         uint16 len = pTemp->szName ? (uint16)((WSTRLEN(pTemp->szName)+1)*sizeof(AECHAR)) : 0;

         if( IsDataSearchMatch(pTemp->szName, len) )
         {
            return(pTemp->wRecID);
         }
      }
      ptr = ptr->pNext;
   }
   return(AEE_DB_RECID_NULL);
}


/*===========================================================================

FUNCTION: FreeAddrCacheElem

DESCRIPTION
  This function frees the memory associated with the given cache entry.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
static void FreeAddrCacheElem(AddrBkCache *pRemove)
{
   if(!pRemove)
      return;

   if(pRemove->szName)
      sys_free(pRemove->szName);
   sys_free(pRemove);
}



/*===========================================================================

FUNCTION: GetCacheRecCount

DESCRIPTION
  This function returns the number of records in cache. Its the same as the number
  of records in the addressbook. This function is used as a faster means to
  obtain the count than going to the DB

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
static uint16 GetCacheRecCount()
{
   uint16 count = 0;
   AddrBkCache *ptr;

   ptr = gpCache;
   while(ptr)
   {
      count++;
      ptr = ptr->pNext;
   }
   return(count);
}


/*===========================================================================

FUNCTION: GetCache

DESCRIPTION
  Make sure that the record cache has been initialized if it is supposed to be

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  May read in record cache
===========================================================================*/
static void GetCache(void)
{
   if (gbCacheInited)
      return;  // Already did it
   
   if (gwProps & ABP_NO_SORT_CACHE)
   {
      gNoCache = TRUE;  // Application doesn't want cache
   }
   else
   {
      gNoCache = FALSE;
      InitAddrBkCache();
   }
}


/*===========================================================================

FUNCTION: FreeSearchData

DESCRIPTION
  Helper function to free any memory that has been allocated by a previous
  search (EnumRecInit).

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void FreeSearchData( void )
{
   if(gEnumRecInfo.pData)
   {
      sys_free(gEnumRecInfo.pData);
      gEnumRecInfo.pData = NULL;
   }
}



/*===========================================================================

FUNCTION: CopyEnhancedEnumData

DESCRIPTION
  Helper function to copy the enhanced enumerate data passed by an
  application to the local gEnumRecInfo structure.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS, if data successfully copied.
  EBADPARM, if one of the passed parameters is invalid.
  ENOMEMORY, if unable to allocate a buffer to hold the match data.

SIDE EFFECTS
  none
===========================================================================*/
static int CopyEnhancedEnumData( AEEAddrEnumRecInfo *pUserData )
{
   int size;

   gEnumRecInfo.nCategories = pUserData->nCategories;
   if( pUserData->nCategories >= 0 && pUserData->nCategories <= OEM_ADDRESS_CATEGORY_COUNT)
   {
      if( pUserData->nCategories )
      {
         size = pUserData->nCategories * sizeof(AEEAddrCat);
         MEMCPY(gEnumRecInfo.pCatList, pUserData->pCatList, size);
      }
   }
   else
   {
      return EBADPARM;
   }

   gEnumRecInfo.nFields = pUserData->nFields;
   if( pUserData->nFields >= 0 && pUserData->nFields <= OEM_ADDRESS_FIELDS_COUNT)
   {
      if( pUserData->nFields )
      {
         size = pUserData->nFields * sizeof(AEEAddrFieldID);
         MEMCPY(gEnumRecInfo.pFieldList, pUserData->pFieldList, size);
      }
   }
   else
   {
      return EBADPARM;
   }

   gEnumRecInfo.nTypes = pUserData->nTypes;
   if( pUserData->nTypes >= 0 && pUserData->nTypes <= AEEDB_FT_MAX)
   {
      if( pUserData->nTypes )
      {
         size = pUserData->nTypes * sizeof(AEEAddrFieldType);
         MEMCPY(gEnumRecInfo.pTypeList, pUserData->pTypeList, size);
      }
   }
   else
   {
      return EBADPARM;
   }

   gEnumRecInfo.dwDataSize = pUserData->dwDataSize;
   if( pUserData->dwDataSize > 0 )
   {
      gEnumRecInfo.pData = sys_malloc(pUserData->dwDataSize);
      if( NULL == gEnumRecInfo.pData )
         return ENOMEMORY;
      MEMCPY(gEnumRecInfo.pData, pUserData->pData, pUserData->dwDataSize);
   }
   else
   {
      if( pUserData->dwDataSize < 0 )
         return EBADPARM;
   }

   gEnumRecInfo.fType = pUserData->fType;
   gEnumRecInfo.flags = pUserData->flags;

   return AEE_SUCCESS;
}


/*===========================================================================

FUNCTION: ConvertToEnhancedEnumData

DESCRIPTION
  Helper function to copy the normal record enumeration data passed by an
  application to the local gEnumRecInfo structure.  This lets the OEM layer
  implementation work in terms of only one search structure, 
  AEEAddrEnumRecInfo.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS, if data successfully copied.
  ENOMEMORY, if unable to allocate a buffer to hold the match data.

SIDE EFFECTS
  none
===========================================================================*/
static int ConvertToEnhancedEnumData(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize)
{
   // If the enumeration is for a specific category, then copy that info
   if( AEE_ADDR_CAT_NONE != wCategory )
   {
      gEnumRecInfo.nCategories = 1;
      gSearchCat[0] = wCategory;
   }
   else
   {
      gEnumRecInfo.nCategories = 0;     
   }

   // If the enumeration is for a specific field, then copy that info
   if( AEE_ADDRFIELD_NONE != wFieldID )
   {
      gEnumRecInfo.nFields = 1;
      gSearchFieldID[0] = wFieldID;
   }
   else
   {
      gEnumRecInfo.nFields = 0;     
   }

   // No data type search is possible using the standard EnumInit parameters,
   gEnumRecInfo.nTypes = 0;

   // If specific field data to match is specified, then copy that info
   if( wDataSize > 0 && pData )
   {
      gEnumRecInfo.pData = sys_malloc(wDataSize);
      if( NULL == gEnumRecInfo.pData )
         return ENOMEMORY;
      MEMCPY(gEnumRecInfo.pData, pData, wDataSize);
      gEnumRecInfo.dwDataSize = wDataSize;
   }
   else
   {
      gEnumRecInfo.dwDataSize = 0;
   }

   // Default search is assumed to be string and exact match.  OEM-unqique
   // implementations that have data types other than AECHAR strings should
   // assign the data type as a function of the field ID.
   gEnumRecInfo.fType = AEEDB_FT_STRING;
   gEnumRecInfo.flags = ABSF_MATCH_EQUAL | ABSF_CASE_SENSITIVE;

   return AEE_SUCCESS;
}


/*===========================================================================

FUNCTION: ConvertToEnhancedEnumData

DESCRIPTION
  Helper function to copy the normal record enumeration data passed by an
  application to the local gEnumRecInfo structure.  This lets the OEM layer
  implementation work in terms of only one search structure, 
  AEEAddrEnumRecInfo.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS, if data successfully copied.
  ENOMEMORY, if unable to allocate a buffer to hold the match data.

SIDE EFFECTS
  none
===========================================================================*/
static boolean IsRecEnumDataValid(int *pErr)
{
   int i;

   for( i = 0; i < gEnumRecInfo.nCategories; i++ )
   {
      if( !IsSearchAllowedInCategory(gEnumRecInfo.pCatList[i]) )
      {
         *pErr = AEE_ADDR_INVALID_ENUM_CAT;
         return FALSE;
      }
   }

   for( i = 0; i < gEnumRecInfo.nFields; i++ )
   {
      if( !IsSearchAllowedInField(gEnumRecInfo.pFieldList[i]) )
      {
         *pErr = AEE_ADDR_INVALID_ENUM_FIELD;
         return FALSE;
      }
   }

   return TRUE;
}


/*===========================================================================

FUNCTION: IsCategorySearchMatch

DESCRIPTION
  Helper function to determine the the passed category matches one of the
  categories specified by the last call to OEMAddr_EnumRecInit() and stored
  in gEnumRecInfo.

DEPENDENCIES
  gEnumRecInfo must contain the desired search criteria

RETURN VALUE
  TRUE, if category matches one of the categories in the category list or
        if the number of categories in the list is 0 (match all categories).
  FALSE, otherwise.

SIDE EFFECTS
  none
===========================================================================*/
static boolean IsCategorySearchMatch(AEEAddrCat cat)
{
   int i;
   
   // If there are no categories listed to search, then all categories match
   if( 0 == gEnumRecInfo.nCategories )
      return TRUE;

   // Loop across the category search list to see if this category is in the list
   for( i = 0; i < gEnumRecInfo.nCategories; i++ )
   {
      if( gEnumRecInfo.pCatList[i] == cat )
         return TRUE;
   }
   return FALSE;
}


/*===========================================================================

FUNCTION: IsFieldIDSearchMatch

DESCRIPTION
  Helper function to determine the the passed field ID matches one of the
  field IDs specified by the last call to OEMAddr_EnumRecInit() and stored
  in gEnumRecInfo.

DEPENDENCIES
  gEnumRecInfo must contain the desired search criteria

RETURN VALUE
  TRUE, if field ID matches one of the field IDs in the category list or
        if the number of field IDs in the list is 0 (match all field IDs).
  FALSE, otherwise.

SIDE EFFECTS
  none
===========================================================================*/
static boolean IsFieldIDSearchMatch(AEEAddrFieldID fID)
{
   int i;
   
   // If there are no field IDs listed to search, then all field IDs match
   if( 0 == gEnumRecInfo.nFields )
      return TRUE;

   // Loop across the category search list to see if this category is in the list
   for( i = 0; i < gEnumRecInfo.nFields; i++ )
   {
      if( gEnumRecInfo.pFieldList[i] == fID )
         return TRUE;
   }
   return FALSE;
}

/*===========================================================================

FUNCTION: IsDataSearchMatch

DESCRIPTION
  Helper function to determine the the passed data matches the data specified
  by the last call to OEMAddr_EnumRecInit() and stored in gEnumRecInfo.

DEPENDENCIES
  gEnumRecInfo must contain the desired search criteria

RETURN VALUE
  TRUE, if pData matches the data in gEnumRecInfo or if wDataLen == 0 
        (match all data).
  FALSE, otherwise.

SIDE EFFECTS
  none
===========================================================================*/
static boolean IsDataSearchMatch(void *pData, uint16 wDataLen )
{
   boolean bCaseInsensitive;
   boolean bIsMatch = FALSE;
   int matchType;

   // If there is no field data to match, the all field data matches
   if( 0 == gEnumRecInfo.dwDataSize )
      return TRUE;

   // Make sure pData is not NULL, just in case
   if( NULL == pData )
      return FALSE;

   // For string comparisons, we need to consider the different match
   // types (case, equals, contains, begins with, etc.)
   switch( gEnumRecInfo.fType )
   {
      case AEEDB_FT_STRING:
      case AEEDB_FT_PHONE:
         bCaseInsensitive = ((gEnumRecInfo.flags & ABSF_CASE_MASK) == ABSF_CASE_INSENSITIVE);
         matchType = gEnumRecInfo.flags & ABSF_MATCH_MASK;
         switch( matchType )
         {
            case ABSF_MATCH_CONTAINS:
               if( wDataLen >= gEnumRecInfo.dwDataSize )
               {
                  if( OEMAddr_WStrStrEx(pData, (AECHAR *)gEnumRecInfo.pData, bCaseInsensitive) )
                     bIsMatch = TRUE;                     
               }
               break;

            case ABSF_MATCH_BEGINS_WITH:
               if( wDataLen >= gEnumRecInfo.dwDataSize &&
                   (bCaseInsensitive && 0==WSTRNICMP(pData, gEnumRecInfo.pData, (gEnumRecInfo.dwDataSize/sizeof(AECHAR))-1) ||
                   (!bCaseInsensitive && 0==WSTRNCMP(pData, gEnumRecInfo.pData, (gEnumRecInfo.dwDataSize/sizeof(AECHAR))-1) ) ) )
                  bIsMatch = TRUE;
               break;

            case ABSF_MATCH_ENDS_WITH:
               if( wDataLen >= gEnumRecInfo.dwDataSize )
               {
                  AECHAR * pwsz = (AECHAR *)pData + (wDataLen-gEnumRecInfo.dwDataSize)/sizeof(AECHAR);
                  if( (bCaseInsensitive && 0==WSTRICMP(pwsz, gEnumRecInfo.pData) ||
                      (!bCaseInsensitive && 0==WSTRCMP(pwsz, gEnumRecInfo.pData) ) ) )
                     bIsMatch = TRUE;
               }
               break;

            case ABSF_MATCH_EQUAL:
            default:
               if( wDataLen == gEnumRecInfo.dwDataSize &&
                   (bCaseInsensitive && 0==WSTRICMP(pData, gEnumRecInfo.pData) ||
                     (!bCaseInsensitive && 0==WSTRCMP(pData, gEnumRecInfo.pData) ) ) )
               {
                   bIsMatch = TRUE;
               }
               break;
         }
         break;

      default:
         if( wDataLen == gEnumRecInfo.dwDataSize &&
             0 == MEMCMP(gEnumRecInfo.pData, pData, wDataLen) )
            bIsMatch = TRUE;
         break;
   }

   return bIsMatch;
}


/*===========================================================================

FUNCTION: IsFieldTypeSearchMatch

DESCRIPTION
  Helper function to determine the the passed field type matches one of the
  field types specified by the last call to OEMAddr_EnumRecInit() and stored
  in gEnumRecInfo.

DEPENDENCIES
  gEnumRecInfo must contain the desired search criteria

RETURN VALUE
  TRUE, if fType matches one of the field types in the field type list or
        if the number of field types in the list is 0 (match all field types).
  FALSE, otherwise.

SIDE EFFECTS
  none
===========================================================================*/
static boolean IsFieldTypeSearchMatch(AEEAddrFieldType fType)
{
   int i;
   
   // If there are no field types listed to search, then all field types match
   if( 0 == gEnumRecInfo.nTypes )
      return TRUE;

   // Loop across the field type search list to see if this field type is in the list
   for( i = 0; i < gEnumRecInfo.nTypes; i++ )
   {
      if( gEnumRecInfo.pTypeList[i] == fType )
         return TRUE;
   }
   return FALSE;
}


/*=============================================================================
FUNCTION: OEMAddr_WStrStrEx

DESCRIPTION:
   Widestring strstr() function with case sensitive compare flag.

PARAMETERS:
   *pHaystack: string in which pNeedle is found
   *pNeedle:  string which is searched in pHaystack

RETURN VALUE:
boolean:
   Returns true if pNeedle is found in pHaystack. 

COMMENTS:

SIDE EFFECTS:

=============================================================================*/
static boolean OEMAddr_WStrStrEx( AECHAR *pHaystack, AECHAR *pNeedle, boolean bCaseInsensitive )
{
   AECHAR *pTempNdl, *pTempHay;
   AECHAR char1[2],char2[2];

   if((pHaystack == NULL)||(pNeedle == NULL))
   {
       return FALSE;
   }

   char1[1] = char2[1] = (AECHAR)0;

   for (;;)
   {
      pTempNdl = pNeedle;
      pTempHay = pHaystack;


      if ((AECHAR)0 == *pHaystack)
      {
         return FALSE;
      }
      
      for(;;)
      {
         char1[0] = *pTempHay;
         char2[0] = *pTempNdl;

         // If case insensitive, then covert both characters to lower case
         // before doing the compare.
         if( bCaseInsensitive )
         {
            WSTRLOWER(char1);
            WSTRLOWER(char2);
         }

         // Compare the characters
         if ((char1[0] == char2[0] )&&(char1[0]!=(AECHAR)0))
         {
            pTempNdl++;
            pTempHay++;
         }
         else
         {
            break;
         }
      }
      
      if ((AECHAR)0 == *pTempNdl) {
         return TRUE;
      }
      
      pHaystack++;
   }
}

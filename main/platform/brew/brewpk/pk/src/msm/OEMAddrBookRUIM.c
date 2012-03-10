/*======================================================
FILE:  OEMAddrBookRUIM.c

SERVICES:  OEM functions for supporting BREW Address
           book interface on the RUIM

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW Address book
  interface on the RUIM

        Copyright ?1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "OEMFeatures.h"
#ifdef FEATURE_ADDRBOOK_RUIM       // DMSS Feature Define for R-UIM.

#include "comdef.h"        // Get DMSS type declarations.

#include "AEE.h"           // BREW headers.
#include "AEE_OEM.h"
#include "AEEAddrBook.h"
#include "AEEClassIDs.h"
#include "AEEComdef.h"
#include "AEEError.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "OEMAddrBook.h"
#include "OEMDB.h"
#include "OEMHeap.h"
#include "OEMClassIDs.h"

//DMSS headers.
#include "rex.h"
#include "uim.h"
#include "nvruimi.h"
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
#include "err.h"
#include "db.h"
#endif
#include "OEMNV.h"
#include "Dstask.h"
// External function prototypes.
int AEEAddrBook_New(IShell *pIShell, AEECLSID ClsId, void **ppObj);

// Local helper function prototypes.
static void OEMRUIM_report (uim_rpt_type *report);
static void OEMRUIM_send_uim_cmd_and_wait (uim_cmd_type *uim_cmd_ptr);
#ifndef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
static uint16 OEMRUIM_phonebook_max_recs(void);
#endif

static uint16 UnusedCacheID(uint16 adn_max_rec);
static int del_adn_rec(uint16 wRecID);
static int read_adn_rec(byte *pBuf, uint16 wRecID,
                        uint16 wRecSize,
                        AEE_DBRecInfo *dbRecInfo);
static int write_adn_rec(const byte *pBuf, uint16 wRecID,
                         uint16 wRecSize);

uint16  AT_OEMRUIMAddr_GetNumRecs(void);
byte*   AT_RUIMRecordGetOne(word wpbID);
byte*   AT_RUIMRecordGetOneName(word wpbID);
uint16  AT_OEMRUIMAddr_RecordAdd(const AECHAR *szName,const AECHAR *szNumber);
int     AT_OEMRUIMAddr_RecordDelete(uint16 wRecID);
int     AT_write_adn_rec(const byte *pBuf, uint16 wRecID,uint16 wRecSize);
int     AT_del_adn_rec(uint16 wRecID);
int     AT_OEMRUIMAddr_RemoveAllRecs(void);
int     AT_OEMRUIMAddr_RecordDelete(uint16 wRecID);
int     AT_OEMRUIMAddr_RemoveAllRecs_UIM(void);
void    AT_OEMRUIM_report (uim_rpt_type *report);
uint16  AT_OEMRUIM_phonebook_max_recs(void);


extern rex_tcb_type ds_tcb;

extern boolean OEM_IsCDMASVCSupport(uim_cdma_svc_table_enum_type eType);

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/
// External global variables.
extern rex_tcb_type ui_tcb;

// Local defines.
#define UI_RUIM_ADDRBK_SIG 0x80000
#define ADN_NUMBER_LEN 10
#define ADN_FILLER_CHAR 0xff

// Local global variables.
static uim_cmd_type   gUimCmd;
static uim_cmd_type   gUimCmdExt;
static uim_rpt_type   gCallBack;
static uim_rpt_type   gCallBackExt;
static int            gAdnRecSize;
static int            gAdnMaxRec=0;
static boolean        gbReInitAddr = FALSE;

OBJECT(COEMAddrBook)
{
   AEEVTBL(IOEMAddrBook) *pvt;

   uint32            m_nRefs;
};

//Global Context for AddressBook.
static boolean  gbRUIMAddrBookOpen = FALSE;

//Number of address categories supported by this address book
#define  OEM_ADDRESS_CATEGORY_COUNT    0

//Number of fields supported in the address book
#define  OEM_ADDRESS_FIELDS_COUNT      2

typedef  AEEAddCacheInfo   RUIMAddrBkCache;

//Cache of address book in memory
static RUIMAddrBkCache  *gpCache=NULL;         //Cache of vital fields in AddressBook

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
static RUIMAddrBkCache  *gpCacheHead=NULL;
extern rex_tcb_type uim_tcb;
#endif

//Properties
static uint32            gwRUIMProps;

//Global varibles used in the search
static AEEAddrCat        gSearchCat;       // AddressCategory to search on
static AEEAddrFieldID    gSearchFieldID;   // Field ID to search on
static boolean           gbEnumInit;       // Flag to indicate if enumeration has been initialized
static uint16            gwCurRecID;       // Current RecordID in enumeration
static void              *gSearchData;     // Data value to search on
static uint16            gSearchDataSize;  // Size of gSearchData
static boolean           gbNameSearch;     // Flag to indicate if its a name-based search

// Separator for the numbers of the cache
#define NUMBER_SEPARATOR    ((AECHAR)',')

// Global varibles used in the cache search
static AEEAddrFieldID   gCacheSearchFieldID;  // Field ID to search on
static boolean          gbEnumCacheInit;      // Flag to indicate if enumeration has been initialized
static AECHAR          *gpCacheSearchData;    // search data
static uint16           gCacheSearchDataSize; // Size of gSearchData
static RUIMAddrBkCache *gpCurCacheElement;    // Will point to current element in cache

AEECallback gInitRuimCachecb;

#ifndef PARAM_NOT_REF
#define PARAM_NOT_REF(x)              
#endif

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
boolean  InitRUIMAddrBkCacheCb(void);
#endif

//Helper functions
static byte *AddressToRawData(AEEAddrCat cat, AEEAddrField *pItems,
                              int nItemCount, int *pErr,
                              uint16 *pSize);
static int RawDataToAddress(byte *pBuf, word wRecSize, AEEAddrCat *pcat,
                            AEEAddrField **ppItems, int *pnItemCount,
                            int *pErr);
static int IsSearchMatch(uint16 wRecID, AEEAddrCat *cat,
                         AEEAddrField **ppItems, int *nCount,
                         int *pErr);
static int IsSearchAllowed(AEEAddrCat wCategory,
                           AEEAddrFieldID wFieldID);
static int InitRUIMAddrBkCache(void);
static AECHAR *GetAddrFieldName(AEEAddrField *pItems, int nCount,
                                int *pIndex);
static AECHAR *GetAddrFieldNumber(AEEAddrField *pItems, int nCount,
                                  int *pIndex);
static int InsertCache(uint16 wRecID, const AECHAR *szName,
                       const AECHAR *szNumber);
static int RemoveRecCache(uint16 wRecID);
static int UpdateCache(uint16 wrecID, const AECHAR *szName,
                       const AECHAR *szNumber);
static int ClearRUIMAddrBkCache(void);
static uint16 GetNextRecInCache(uint16 recID);
static uint16 GetNextMatchInCache(uint16 recID,AECHAR *szMatch);
static void FreeAddrCacheElem(RUIMAddrBkCache *pRemove);
static uint16 GetCacheRecCount(void);
static void OEM_FreeAddressItems(AEEAddrField *pItems,
                                 int nItemCount);
#if defined(FEATURE_CONTACT_APP)
extern int Appscommon_CompareName(AECHAR *wstrNam1,AECHAR *wstrNam2 );
#else
static int AddrBkCompareName(const AECHAR *str1,
                             const AECHAR *str2);
#endif                             

// OEM VTBL functions.

//AddRef
static uint32 OEMRUIMAddr_AddRef(IOEMAddrBook * po);

//Release
static uint32 OEMRUIMAddr_Release(IOEMAddrBook * po);

//Get ID of Next record that matches the search criteria initialized above.
static uint16 OEMRUIMAddr_EnumNextRec(AEEAddrCat *cat, AEEAddrField **ppItems,
                                      int *nItemCount,
                                      int *pErr);

//Search Init
static int OEMRUIMAddr_EnumRecInit(AEEAddrCat wCategory,
                                   AEEAddrFieldID wFieldID,
                                   void *pData,
                                   uint16 wDataSize);

//Get count of number of categories supported
static int OEMRUIMAddr_GetCatCount(void);

//Get Category list
static int OEMRUIMAddr_GetCatList(AEEAddrCat *p, int nSize);

//Get Fields Info
static int OEMRUIMAddr_GetFieldInfo(AEEAddrCat c, AEEAddrFieldInfo *pf,
                                    int nSize);

//Get Number of fields supported for this category
static int OEMRUIMAddr_GetFieldInfoCount(AEEAddrCat);

//Get Number of Records
static uint16 OEMRUIMAddr_GetNumRecs(void);

//Add a record to the address book
static uint16 OEMRUIMAddr_RecordAdd(AEEAddrCat cat, AEEAddrField *pItems,
                                    int nItemCount, int *pErr);

//Remove a record from the address book
static int OEMRUIMAddr_RecordDelete(uint16 recID);

//Get a record, given the ID of the record out of the cache
static int OEMRUIMAddr_RecordGetByID(uint16 recID, AEEAddrCat *cat,
                                     AEEAddrField **ppItems, int *nItemCount,
                                     int *pErr);
                                     
//Get a record, given the ID of the record off the RUIM
static int OEMRUIMAddr_RecordGetByIDOffDevice(uint16 recID, AEEAddrCat *cat,
                                     AEEAddrField **ppItems, int *nItemCount,
                                     int *pErr);
                                     
//Update an existing Record in the address book
static int OEMRUIMAddr_RecordUpdate(uint16 recID, AEEAddrCat cat,
                                    AEEAddrField *pItems, int nItemCount,
                                    int *pErr);

//Remove all Records
//If OEMs do not want to support this, just return EUNSUPPORTED
static int OEMRUIMAddr_RemoveAllRecs(void);

//Init of AddressBook Interface
static boolean OEMRUIMAddrBook_Init(void);

//Exit of AddressBook Interface
static void OEMRUIMAddrBook_Exit(void);

//Get the properties of the Address Book
static uint32 OEMRUIMAddr_GetProperties(void);

//Set the properties of the Address Book
static void OEMRUIMAddr_SetProperties(uint32 dwProps);

//Get category name
static int OEMRUIMAddr_GetCategoryName(AEEAddrCat c, AECHAR *pszCatName, int *pnSize);

//Get field name
static int OEMRUIMAddr_GetFieldName(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize);
static boolean  OEMRUIMAddr_WStrStrEx( AECHAR *pHaystack, AECHAR *pNeedle, boolean bCaseInsensitive );
static int      OEMRUIMAddr_EnumCacheInit( AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void  *pData, uint16 wDataSize);
static int      OEMRUIMAddr_EnumNextCache(void **ppCache);
static uint16   OEMRUIMAddr_ExtractCache( void  *pCache,  AECHAR  **ppName,  AEEAddrCat *pCat);
static uint16   OEMRUIMAddr_GetCapacity(void);

static int OEMRUIMAddr_GetCacheinfoByNumber(AECHAR *pwstrNum, 
                                            AEEAddCacheInfo *pCacheInfo,
                                            PFN_NUMBERMATCH pfnMactch);
static int OEMRUIMAddr_CheckSameRecord(AECHAR *name, boolean* exist);
static const VTBL(IOEMAddrBook) gsOEMRUIMAddrBookFuncs = 
                                                 {OEMRUIMAddr_AddRef,
                                                  OEMRUIMAddr_Release,
                                                  OEMRUIMAddr_EnumNextRec,
                                                  OEMRUIMAddr_EnumRecInit,
                                                  OEMRUIMAddr_GetCatCount,
                                                  OEMRUIMAddr_GetCatList,
                                                  OEMRUIMAddr_GetFieldInfo,
                                                  OEMRUIMAddr_GetFieldInfoCount,
                                                  OEMRUIMAddr_GetNumRecs,
                                                  OEMRUIMAddr_RecordAdd,
                                                  OEMRUIMAddr_RecordDelete,
                                                  OEMRUIMAddr_RecordGetByID,
                                                  OEMRUIMAddr_RecordUpdate,
                                                  OEMRUIMAddr_RemoveAllRecs,
                                                  OEMRUIMAddr_SetProperties,
                                                  OEMRUIMAddr_GetProperties,
                                                  OEMRUIMAddr_GetCategoryName,
                                                  OEMRUIMAddr_GetFieldName,
                                                  OEMRUIMAddr_EnumCacheInit,
                                                  OEMRUIMAddr_EnumNextCache,
                                                  OEMRUIMAddr_ExtractCache,
                                                  OEMRUIMAddr_GetCapacity
                                                  ,OEMRUIMAddr_GetCacheinfoByNumber
                                                  , OEMRUIMAddr_CheckSameRecord
                                                  };

/*===========================================================================

FUNCTION OEMAddrBook_New

===========================================================================*/
int OEMRUIMAddrBook_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj)
{
   COEMAddrBook *pNew;
   int Err = EUNSUPPORTED;

   *ppObj = NULL;

   if (ClsId == AEECLSID_OEMRUIMADDRBOOK)
   {
      // Allocate 
      pNew = (COEMAddrBook*)AEE_NewClassEx((IBaseVtbl*)&gsOEMRUIMAddrBookFuncs,
                                          sizeof(COEMAddrBook), TRUE);
      if (!pNew)
      {
         Err = ENOMEMORY;
      }
      else
      {
         // Return
         if( OEMRUIMAddrBook_Init() )
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

/*===========================================================================

FUNCTION OEMRUIMAddr_AddRef

===========================================================================*/
static uint32  OEMRUIMAddr_AddRef(IOEMAddrBook * po)
{
   COEMAddrBook *pMe = (COEMAddrBook *)po;

   return(++(pMe->m_nRefs));
}

/*===========================================================================

FUNCTION OEMRUIMAddr_Release

===========================================================================*/
static uint32 OEMRUIMAddr_Release(IOEMAddrBook * po)
{
   COEMAddrBook *pMe = (COEMAddrBook *)po;
   if(pMe->m_nRefs)
   {
      if(--pMe->m_nRefs == 0)
      {
        OEMRUIMAddrBook_Exit();
        sys_free(po);
        
        return 0;
      }

   }
   return pMe->m_nRefs;
}

/*===========================================================================

FUNCTION OEMRUIM_report

===========================================================================*/
static void OEMRUIM_report (uim_rpt_type *report)
{
   MEMCPY(&gCallBack, report, sizeof(uim_rpt_type));
   rex_set_sigs( &ui_tcb, UI_RUIM_ADDRBK_SIG);
}

/*===========================================================================

FUNCTION AT_OEMRUIM_report

===========================================================================*/
void AT_OEMRUIM_report (uim_rpt_type *report)
{
   MEMCPY(&gCallBack, report, sizeof(uim_rpt_type));
   rex_set_sigs( &ds_tcb, DS_UIM_CMD_SIG);
}


/*===========================================================================
FUNCTION OEMRUIM_send_uim_cmd_and_wait

===========================================================================*/
static void OEMRUIM_send_uim_cmd_and_wait (uim_cmd_type *uim_cmd_ptr)
{
   (void) rex_clr_sigs( &ui_tcb, UI_RUIM_ADDRBK_SIG);

   // Send the command.
   uim_cmd (uim_cmd_ptr);

   (void) rex_wait( UI_RUIM_ADDRBK_SIG);
}
/*===========================================================================

FUNCTION OEMRUIMAddr_RecordAdd()
   <see OEMAddr_RecordAdd() documentation in OEMAddrBook.h>

===========================================================================*/
static uint16 OEMRUIMAddr_RecordAdd(AEEAddrCat cat, AEEAddrField *pItems,
                                    int nItemCount, int *pErr)
{
   //Stretch pItems into a single contiguous memory block so that we can
   //place it onto the R-UIM.

   byte          *pBuf;
   uint16        wRecID;
   uint16        wRecSize;
   int           status;
   int           i = 0;

   if(cat != AEE_ADDR_CAT_NONE)
      return(AEE_ADDR_RECID_NULL);

   if(!pItems)
      return(AEE_ADDR_RECID_NULL);

   pBuf = AddressToRawData(cat, pItems, nItemCount, pErr, &wRecSize);
   if(!pBuf)
      return(AEE_ADDR_RECID_NULL);

   //Add record to database
   wRecID = UnusedCacheID(gAdnMaxRec);
   status = write_adn_rec(pBuf, wRecID, wRecSize);

   //Free the buffer
   FREE(pBuf);

   if(status != AEE_SUCCESS)
   {
      if(pErr) {
         *pErr = status;
      }
      return(AEE_ADDR_RECID_NULL);
   }

   //Update cache
   if((InsertCache(wRecID, GetAddrFieldName(pItems,nItemCount,NULL),
                   GetAddrFieldNumber(pItems,nItemCount, NULL))) != AEE_SUCCESS)
   {
      OEMRUIMAddr_RecordDelete(wRecID);
      return(AEE_ADDR_RECID_NULL);
   }

   //Success.
   return(wRecID);
}

/*===========================================================================

FUNCTION OEMRUIMAddr_RecordUpdate()
   <see OEMAddr_RecordUpdate() documentation in OEMAddrBook.h>

===========================================================================*/
static int  OEMRUIMAddr_RecordUpdate(uint16 wrecID, AEEAddrCat cat,
                                     AEEAddrField *pItems, int nItemCount,
                                     int *pErr)
{
   byte        *pBuf;
   uint16      wRecSize;
   int         status;

   pBuf = AddressToRawData(cat,pItems,nItemCount,pErr,&wRecSize);
   if(!pBuf)
      return(AEE_ADDR_RECID_NULL);

   status = write_adn_rec(pBuf, wrecID, wRecSize);

   //Free the buffer
   FREE(pBuf);

   if(status != AEE_SUCCESS)
   {
      if(pErr)
         *pErr = status;
      return(EFAILED);
   }

   //Update Cache
   UpdateCache(wrecID,GetAddrFieldName(pItems,nItemCount,NULL),
                                    GetAddrFieldNumber(pItems,nItemCount,NULL));

   return(AEE_SUCCESS);

}

/*===========================================================================

FUNCTION OEMRUIMAddr_RecordGetByIDOffDevice()
   <see OEMAddr_RecordGetByIDOffDevice() documentation in OEMAddrBook.h>

===========================================================================*/
static int OEMRUIMAddr_RecordGetByIDOffDevice(uint16 wrecID, AEEAddrCat *pcat,
                                     AEEAddrField **ppItems, int *pnItemCount,
                                     int *pErr)
{
   byte *         pBuf;
   AEE_DBRecInfo  dbRecInfo;
   int            nRet;

   pBuf = MALLOC(gAdnRecSize);
   if (!pBuf) {
      *pErr = ENOMEMORY;
      return EFAILED;
   }
   nRet = read_adn_rec(pBuf, wrecID, gAdnRecSize, &dbRecInfo);
   if (nRet != AEE_SUCCESS) {
      FREE (pBuf);
      *pErr = EFAILED;
      return EFAILED;
   }

   //Parse the buffer into individual address items
   nRet = RawDataToAddress(pBuf, dbRecInfo.wRecSize, pcat, ppItems, pnItemCount,
                           pErr);
  
   FREE(pBuf);

   return(nRet);
}

/*===========================================================================

FUNCTION OEMRUIMAddr_RecordGetByID()
   <see OEMAddr_RecordGetByIDOffDevice() documentation in OEMAddrBook.h>

===========================================================================*/
static int OEMRUIMAddr_RecordGetByID(uint16 wrecID, AEEAddrCat *pcat,
                                     AEEAddrField **ppItems, int *pnItemCount,
                                     int *pErr)
{
   AEEAddrField      *pFields = NULL;
   RUIMAddrBkCache   *pCur;
   
   if(gpCache) {
      pCur = gpCache;
      while(pCur && (pCur->wRecID != wrecID)) {
         pCur = pCur->pNext;
      }
      if(!pCur) {
         return(EFAILED);
      }
   }
   else {
      return(EFAILED);
   }

   if(!ppItems || !pnItemCount) {
      return(EFAILED);
   }

   // Set the category
   if(pcat) {
      *pcat = AEE_ADDR_CAT_NONE;
   }
 
   
   // Assume that a phone number will be found and allocate 2 AEEAddrField
   // structs.
   pFields = (AEEAddrField *)MALLOC(sizeof(AEEAddrField)*2);
   if(!pFields) {
      if(pErr) {
         *pErr = ENOMEMORY;
      }
      return(EFAILED);
   }
   *ppItems = pFields;
   *pnItemCount = 0;

      
   if(pCur->szName) {      
      // Fill in the name info.
      pFields->fID = AEE_ADDRFIELD_NAME;
      pFields->fType = AEEDB_FT_STRING;
      pFields->wDataLen = (WSTRLEN(pCur->szName) + 1) * sizeof(AECHAR);
      pFields->pBuffer = (AECHAR *) MALLOC(pFields->wDataLen);
      
      if(!(pFields->pBuffer)) {
         if(pErr) {
            *pErr = ENOMEMORY;
         }
         return(EFAILED);
      }
       
      WSTRLCPY(pFields->pBuffer, pCur->szName, pFields->wDataLen/sizeof(AECHAR)); // Gemsea:Size IN AECHAR
      
      *pnItemCount +=1;
      pFields++;
   }

   if(pCur->szNumber) {      
      // Fill in the number info.
      pFields->fID = AEE_ADDRFIELD_PHONE_GENERIC;
      pFields->fType = AEEDB_FT_STRING;
      pFields->wDataLen = (WSTRLEN(pCur->szNumber) + 1) * sizeof(AECHAR);
      pFields->pBuffer = (AECHAR *) MALLOC(pFields->wDataLen);
      
      if(!(pFields->pBuffer)) {
         if(pErr) {
            *pErr = ENOMEMORY;
         }
         return(EFAILED);
      }
       
      WSTRLCPY(pFields->pBuffer, pCur->szNumber, pFields->wDataLen/sizeof(AECHAR));// Gemsea:Size IN AECHAR
      
      *pnItemCount += 1;
      pFields++;
   }


   if (*pnItemCount) {
      return(AEE_SUCCESS);
   } else {
      return(EFAILED);
   }
}

/*===========================================================================

FUNCTION OEMRUIMAddrBook_Init()
   <see OEMAddrBook_Init() documentation in OEMAddrBook.h>

===========================================================================*/
static boolean  OEMRUIMAddrBook_Init(void)
{
   //If address book already open, return TRUE
   if(gbRUIMAddrBookOpen)
      return(TRUE);

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
   gpCache = NULL;
#endif   
   gbEnumInit = FALSE;
   gSearchData = NULL;
   gwRUIMProps = AEE_ADDR_DEFAULT_PROPERTIES;

   //Build Cache
   if(InitRUIMAddrBkCache() == AEE_SUCCESS) {
      gbRUIMAddrBookOpen = TRUE;
      return(TRUE);
   }

   return (FALSE);
}

/*===========================================================================

FUNCTION OEMRUIMAddrBook_Exit()
   <see OEMAddrBook_Exit() documentation in OEMAddrBook.h>

===========================================================================*/
static void OEMRUIMAddrBook_Exit(void)
{
   //Close the address book
   if(gbRUIMAddrBookOpen)
   {
      gbRUIMAddrBookOpen = FALSE;
   }
   if(gSearchData)
   {
      sys_free(gSearchData);
      gSearchData = NULL;
   }
   //Free the cache search data
   if(gpCacheSearchData)
   {
       sys_free(gpCacheSearchData);
       gpCacheSearchData = NULL;
   }
   
   // Cache 一旦建立, 不得清除，只能做好维护工作，直至关机
#ifndef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
   //Free the cache
   ClearRUIMAddrBkCache();
#endif
}

/*===========================================================================

FUNCTION OEMRUIMAddr_RecordDelete()
   <see OEMAddr_RecordDelete() documentation in OEMAddrBook.h>

===========================================================================*/
static int     OEMRUIMAddr_RecordDelete(uint16 wRecID)
{
   int         status;

   status = del_adn_rec(wRecID);
   if(status == AEE_SUCCESS)
   {
      //Remove from Cache
      RemoveRecCache(wRecID);
      return(AEE_SUCCESS);
   }
   return(EFAILED);
}

/*===========================================================================

FUNCTION OEMRUIMAddr_GetNumRecs()
   <see OEMAddr_GetNumRecs() documentation in OEMAddrBook.h>

===========================================================================*/
static uint16   OEMRUIMAddr_GetNumRecs(void)
{
   //Get Count from Cache
   return(GetCacheRecCount());
}

/*===========================================================================

FUNCTION OEMRUIMAddr_RemoveAllRecs()
   <see OEMAddr_RemoveAllRecs() documentation in OEMAddrBook.h>

===========================================================================*/
static int OEMRUIMAddr_RemoveAllRecs(void)
{
   uint16      wRecId;

   while ((wRecId = GetNextRecInCache(AEE_DB_RECID_NULL))
                                                         != AEE_DB_RECID_NULL) {
      OEMRUIMAddr_RecordDelete(wRecId);
   }
   return(AEE_SUCCESS);
}

/*===========================================================================

FUNCTION OEMRUIMAddr_EnumRecInit()
   <see OEMAddr_EnumRecInit() documentation in OEMAddrBook.h>

===========================================================================*/
static int OEMRUIMAddr_EnumRecInit(AEEAddrCat wCategory,
                                   AEEAddrFieldID wFieldID, void *pData,
                                   uint16 wDataSize)
{
   //Free data from previous enum
   if(gSearchData)
   {
      sys_free(gSearchData);
      gSearchData = NULL;
   }

   gbEnumInit = FALSE;
   gbNameSearch = FALSE;

   //Check if search is allowed. This function will also check if its a search
   //by Name.
   if(IsSearchAllowed(wCategory,wFieldID) == EFAILED)
      return(EFAILED);

   if(pData)
   {
      //The search data must be valid when switching between apps. Hence,
      //use sys_malloc
      gSearchData = (void *)sys_malloc(wDataSize);
      if(!gSearchData)
         return(EFAILED);

      MEMCPY(gSearchData,pData,wDataSize);
      gSearchDataSize = wDataSize;
   }

   gSearchCat = wCategory;
   gSearchFieldID = wFieldID;
   gbEnumInit = TRUE;
   gwCurRecID = AEE_DB_RECID_NULL;

   return(AEE_SUCCESS);
}

/*===========================================================================

FUNCTION OEMRUIMAddr_EnumNextRec()
   <see OEMAddr_EnumNextRec() documentation in OEMAddrBook.h>

===========================================================================*/
static uint16 OEMRUIMAddr_EnumNextRec(AEEAddrCat *pcat, AEEAddrField **ppItems,
                                      int *pnItemCount,int *pErr)
{
   uint16         wRecID;
   int            nRet;

   //Check if Enum initialized 
   if(!gbEnumInit)
      return(AEE_ADDR_RECID_NULL);

   if(!pcat || !ppItems || !pnItemCount)
   {
      if(pErr)
         *pErr = EBADPARM;
      return(AEE_ADDR_RECID_NULL);
   }

   //If its search by name, we can get the next recID from cache
   if(gbNameSearch)
   {
      while(TRUE)
      {
         wRecID = GetNextMatchInCache(gwCurRecID,(AECHAR*)gSearchData);

         if(wRecID == AEE_DB_RECID_NULL)
            return(AEE_DB_RECID_NULL);


         gwCurRecID = wRecID;

         nRet = OEMRUIMAddr_RecordGetByID(wRecID,pcat,ppItems, pnItemCount,
                                                                          pErr);
         if(nRet == AEE_SUCCESS)
          return(wRecID);

      }
   }

   //Not Name search. Do sequential search.
   //Get the next record that matches the given search criteria
   while(TRUE)
   {
      wRecID = GetNextRecInCache(gwCurRecID);

      if(wRecID == AEE_DB_RECID_NULL)
      {
         gbEnumInit = FALSE;
         return(AEE_ADDR_RECID_NULL);  // no more records
      }

      gwCurRecID = wRecID;

      //Record found. See if matches search criteria
      nRet = IsSearchMatch(wRecID,pcat,ppItems,pnItemCount,pErr);

      if(nRet == AEE_SUCCESS)
         return(wRecID);

      if(nRet == ENOMEMORY)
         return(AEE_ADDR_RECID_NULL);
   }
}

/*===========================================================================

FUNCTION OEMRUIMAddr_GetCatCount()
   <see OEMAddr_GetCatCount() documentation in OEMAddrBook.h>

===========================================================================*/
static int   OEMRUIMAddr_GetCatCount(void)
{
   return(OEM_ADDRESS_CATEGORY_COUNT);
}

/*===========================================================================

FUNCTION OEMRUIMAddr_GetCatList()
   <see OEMAddr_GetCatList() documentation in OEMAddrBook.h>

===========================================================================*/
static int   OEMRUIMAddr_GetCatList(AEEAddrCat *p, int nSize)
{
    int i;
    
    if (!p || !nSize || nSize<OEM_ADDRESS_CATEGORY_COUNT)
    {
        return(EFAILED);
    }
    
    for (i = 0; i < OEM_ADDRESS_CATEGORY_COUNT && i < nSize; i++)
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

/*===========================================================================

FUNCTION OEMRUIMAddr_GetFieldInfoCount()
   <see OEMAddr_GetFieldInfoCount() documentation in OEMAddrBook.h>

===========================================================================*/
static int   OEMRUIMAddr_GetFieldInfoCount(AEEAddrCat c)
{
	return OEM_ADDRESS_FIELDS_COUNT;
}

/*========================================================================

Function: OEMRUIMAddr_SetProperties()

Description:
   This function sets the properties for the address book.

Prototype:
   void OEMRUIMAddr_SetProperties(uint32 dwProps);

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
static void OEMRUIMAddr_SetProperties(uint32 dwProps)
{
   gwRUIMProps = dwProps;
}

/*========================================================================

Function: OEMRUIMAddr_GetProperties()

Description:
   This function gets the properties for the address book.

Prototype:
   uint32 OEMRUIMAddr_GetProperties();

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
static uint32 OEMRUIMAddr_GetProperties(void)
{
   return gwRUIMProps;
}

/*===========================================================================

FUNCTION OEMRUIMAddr_GetFieldInfo()
   <see OEMAddr_GetFieldInfo() documentation in OEMAddrBook.h>

===========================================================================*/
static int OEMRUIMAddr_GetFieldInfo(AEEAddrCat c, AEEAddrFieldInfo *pf,
                                    int nSize)
{
   int i;

   if(!pf)
      return(EBADPARM);

   for(i = 0; i < OEM_ADDRESS_FIELDS_COUNT && i < nSize; i++)
   {
      switch(i)
      {
      case 0:
         pf[i].bFieldCatSearch = FALSE;
         pf[i].bFieldSearch = TRUE;
         pf[i].fieldID = AEE_ADDRFIELD_NAME;
         pf[i].nMaxItems = 1;
         pf[i].fType = AEEDB_FT_STRING;
         break;

      case 1:
         pf[i].bFieldCatSearch = FALSE;
         pf[i].bFieldSearch = TRUE;
         pf[i].fieldID = AEE_ADDRFIELD_PHONE_GENERIC;
         pf[i].nMaxItems = 1;
         pf[i].fType = AEEDB_FT_STRING;
         break;
      
      default:
         break;
      }
   }
   return(AEE_SUCCESS);
}


/*======================================================================= 
Function: OEMRUIMAddr_GetCategoryName()

Description: 
   This method is used to get the AECHAR string name of a category.

Prototype:
   int   OEMRUIMAddr_GetCategoryName(AEEAddrCat c, AECHAR *pszName, int *pnSize);
   
Parameters:
   c: [in]: Address category for which the name in string format is returned.
   pszName: [in/out]: On input this is the buffer where the category name will
   be placed.  If NULL, this indicates the caller is querying the size.
   required.  On return, the buffer will contain the category name.
   pnSize: [in/out]: On input contains the size of pszName. On return, it 
   contains the size filled.

Return Value: 
   AEE_SUCCESS: if successful
   EBADPARM:    if bad parameters are passed in
   AEE_ADDR_CAT_NOT_SUPPORTED: if the category is not supported.
   AEE_ADDRBOOK_NOCATSUPPORT:  if the device does not support categories.

Comments:  
   Categories are not supported on the RUIM address book.

Side Effects: 
   None

See Also: 
   None
=======================================================================*/
static int OEMRUIMAddr_GetCategoryName(AEEAddrCat c, AECHAR *pszName, int *pnSize)
{
   return AEE_ADDRBOOK_NOCATSUPPORT;
}

/*======================================================================= 
Function: OEMRUIMAddr_GetFieldName()

Description: 
   This method is used to get the AECHAR string name of a field.

Prototype:
   int OEMRUIMAddr_GetFieldName(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize)
   
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
static int OEMRUIMAddr_GetFieldName(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize)
{
   int n;
   char *psz = NULL;

   if(!pnSize)
      return EMEMPTR;

   // Search for the passed field
   switch( f )
   {
      case AEE_ADDRFIELD_NAME:
         psz = "Name";
         break;

      case AEE_ADDRFIELD_PHONE_GENERIC:
         psz = "Number";
         break;

      default:
         psz = NULL;
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

//Helper functions

/*===========================================================================

FUNCTION OEMRUIM_phonebook_max_recs

===========================================================================*/
#ifndef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
static uint16 OEMRUIM_phonebook_max_recs (void)
{
   byte *ruim_buffer;

   ruim_buffer = MALLOC(NVRUIM_EF_BUF_SIZE);
   if (!ruim_buffer) {
      return 0;
   }

   // Prepare to read a phonebook EF.  Only need the number of records in the
   // file.
   gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
   gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
   gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
   gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

   gUimCmd.access_uim.item      = UIM_TELECOM_ADN;
   gUimCmd.access_uim.access    = UIM_READ_F;
   gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
   gUimCmd.access_uim.num_bytes = 1;
   gUimCmd.access_uim.offset    = 1;
   gUimCmd.access_uim.data_ptr  = ruim_buffer;

   // From nvruim_access():  Access an EF, do not signal any task, use no
   // signal, no done queue, use a callback, always report status.

   // Send the command to the R-UIM:
   OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);
   
   FREE(ruim_buffer);

   if ((gCallBack.rpt_type != UIM_ACCESS_R)
                                        || (gCallBack.rpt_status != UIM_PASS)) {
      return 0;
   }

   // Get the number of records and max. size.
   gAdnMaxRec  = gUimCmd.access_uim.num_records_rsp;
   gAdnRecSize = gUimCmd.access_uim.num_bytes;
   return (gAdnMaxRec);
}
#endif

uint16 AT_OEMRUIM_phonebook_max_recs(void)
{
    byte *ruim_buffer;

   ruim_buffer = MALLOC(NVRUIM_EF_BUF_SIZE);
   if (!ruim_buffer) {
      return 0;
   }

   // Prepare to read a phonebook EF.  Only need the number of records in the
   // file.
   gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
   gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
   gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
   gUimCmd.access_uim.hdr.rpt_function       = AT_OEMRUIM_report;

   gUimCmd.access_uim.item      = UIM_TELECOM_ADN;
   gUimCmd.access_uim.access    = UIM_READ_F;
   gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
   gUimCmd.access_uim.num_bytes = 1;
   gUimCmd.access_uim.offset    = 1;
   gUimCmd.access_uim.data_ptr  = ruim_buffer;

   // From nvruim_access():  Access an EF, do not signal any task, use no
   // signal, no done queue, use a callback, always report status.

   // Send the command to the R-UIM:
   (void) rex_clr_sigs( &ds_tcb, DS_UIM_CMD_SIG);
   uim_cmd (&gUimCmd);
   (void) rex_wait( DS_UIM_CMD_SIG);
   
   FREE(ruim_buffer);

   if ((gCallBack.rpt_type != UIM_ACCESS_R)
                                        || (gCallBack.rpt_status != UIM_PASS)) {
      return 0;
   }

   // Get the number of records and max. size.
   gAdnMaxRec  = gUimCmd.access_uim.num_records_rsp;
   gAdnRecSize = gUimCmd.access_uim.num_bytes;
   return (gAdnMaxRec);
}
   
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
static void  OEM_FreeAddressItems(AEEAddrField *pItems,int nItemCount)
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
FUNCTION: AddressToRawData()

Description:
   Helper function to convert a given set of address fields into a byte buffer.

Prototype:

Parameters:

Return Value:
  If successful, a byte* that contains all the address elements

Comments:

Side Effects:
  none

See Also:

===========================================================================*/
static byte *AddressToRawData(AEEAddrCat cat, AEEAddrField *pItems,
                              int nItemCount, int *pErr, uint16 *pSize)
{
   AECHAR   *wBuffer;
   byte     *pBuf;
   byte     *pTempBuf;
   int      i, j;
   int      adn_tag_size;
   int      digits_left;
   int      num_digits;
   //int      str_len;
   boolean  first_name;
   boolean  first_num;
   
   first_name = TRUE;
   first_num = TRUE;
   

   pBuf = MALLOC(gAdnRecSize);
   if (!pBuf) {
      *pErr = ENOMEMORY;
      return (NULL);
   }
   MEMSET(pBuf, ADN_FILLER_CHAR, gAdnRecSize);
   for(i = 0; i < nItemCount; i++)
   {
      switch(pItems[i].fID) {
         case AEE_ADDRFIELD_NAME:
            if(first_name)
            {
               first_name = FALSE;
               adn_tag_size = gAdnRecSize - PHONEBOOK_TAIL_LENGTH;
               if (adn_tag_size < 0) {
                  FREE (pBuf);
                  return NULL;
               }
               wBuffer = (AECHAR *)pItems[i].pBuffer;
#if 0               
               str_len = MIN(adn_tag_size,
                                   ((pItems[i].wDataLen / sizeof(AECHAR)) - 1));
               for (j = 0; j < str_len; j++) {
                  pBuf[j] = wBuffer[j];
               }
               if (str_len < adn_tag_size) {
                  MEMSET(&pBuf[str_len], ADN_FILLER_CHAR,
                                                      (adn_tag_size - str_len));
               }
#else
               OEMNV_WSTR_TO_RUIMAlpha( wBuffer,
                                        pBuf,
                                        adn_tag_size);

#endif
            }
            else
            {
               FREE(pBuf);
               return NULL;
            }
            break;
         case AEE_ADDRFIELD_PHONE_GENERIC:
            if(first_num)
            {
               first_num = FALSE;
               num_digits = MAX((pItems[i].wDataLen / sizeof(AECHAR)) - 1, 0);
               num_digits = MIN(num_digits, ADN_NUMBER_LEN * 2);
               adn_tag_size = gAdnRecSize - PHONEBOOK_TAIL_LENGTH;
               if (num_digits) {
                  // Convert from AECHAR to byte.
                  wBuffer = (AECHAR *)pItems[i].pBuffer;
                  pTempBuf = MALLOC(num_digits);
                  if (!pTempBuf) {
                     FREE (pBuf);
                     return NULL;
                  }
                  for (j = 0; j < num_digits; j++) {
                     pTempBuf[j] = (byte)wBuffer[j];
                  }
                  pBuf[adn_tag_size] = 1 + (num_digits + 1) / 2;
               
                  nvruim_phbook_ascii_to_bcd(num_digits,
                                             pTempBuf,
                                             &pBuf[adn_tag_size + 2]);
                  FREE(pTempBuf);
               }
               digits_left = ADN_NUMBER_LEN - (num_digits + 1) /2;
               if (digits_left > 0) {
                  MEMSET(&pBuf[adn_tag_size + 2 + (num_digits + 1) /2], ADN_FILLER_CHAR,
                                                                   digits_left + 2);
               }
            }
            else
            {
               FREE(pBuf);
               return NULL;
            }
            break;
         default:
            FREE(pBuf);
            return NULL;
            break;
      }
   }

   if(pSize)
      *pSize = gAdnRecSize;

   return(pBuf);
}

/*===========================================================================
FUNCTION: RawDataToAddress()

Description:
   Helper function to convert a given raw buffer into the individual address
   fields.

Prototype:
static int   RawDataToAddress(byte *pBuf,
                              word wRecSize, 
                              AEEAddrCat *pcat, 
                              AEEAddrField** ppItems,
                              int *pnItemCount, 
                              int *pErr)

Parameters:
   pBuf:  [in]:  Raw data buffer.
   wRecSize:  [in]:  Size of pBuf in bytes.
   pcat:  [out]:  category to which the pBuf data belongs.
   ppItems:  [out]:  List of AEEAddrFields contains the data.
   pnItemCount:  [out]:  Number of items in ppItems.
   pErr:  [out]:  Error codes.

Return Value:
   AEE_SUCCESS: If succeeded
   EFAILED: if failed

Comments:
   None.

Side Effects:
   None.

See Also:
   None.
===========================================================================*/
static int   RawDataToAddress(byte *pBuf,
                              word wRecSize, 
                              AEEAddrCat *pcat, 
                              AEEAddrField **ppItems,
                              int *pnItemCount, 
                              int *pErr)
{
   AEEAddrField      *pFields = NULL;
   int               adn_tag_size;
   int               num_chars;  
   AECHAR            *wBuffer;
   byte              digits[PHONEBOOK_DN];
   int               num_digits;
   int               i;
   boolean           bNonBlankNameFound;

   if(!pBuf || !ppItems || !pnItemCount) {
      return(EFAILED);
   }

   // Set the category
   if(pcat) {
      *pcat = AEE_ADDR_CAT_NONE;
   }
 
   // Number of characters read from this record.
   adn_tag_size = wRecSize - PHONEBOOK_TAIL_LENGTH;
   if (adn_tag_size < 0) {
      return EFAILED;
   }

   // Allocate a buffer for the alpha tag.
   wBuffer = MALLOC((adn_tag_size +1) * sizeof(AECHAR));

   // Copy, convert, and validate the alpha tag.
   i = 0;
   num_chars = 0;
   bNonBlankNameFound = FALSE;
#if 0   
   while ((i < adn_tag_size) & (pBuf[i] != 0xff)) {
      if (pBuf[i] != ' '){
         bNonBlankNameFound = TRUE;
      }
      num_chars++;
      wBuffer[i] = (AECHAR)pBuf[i];
      i++;
   }
#else
   // Convert the RUIMAlpha to wstring
   OEMNV_RUIMAlpha_TO_WSTR( pBuf,
                            adn_tag_size,
                            wBuffer,
                            adn_tag_size*sizeof(AECHAR));
                            
   while ((i < adn_tag_size) & (wBuffer[i] != NULL)) {
      if (wBuffer[i] != (AECHAR)' '){
         bNonBlankNameFound = TRUE;
      }
      num_chars++;
      i++;
   }
#endif
   wBuffer[i] = NULL;

   if (bNonBlankNameFound) {
      // Assume that a phone number will be found and allocate 2 AEEAddrField
      // structs.
      pFields = (AEEAddrField *)MALLOC(sizeof(AEEAddrField)*2);

      // Fill in the name info.
      pFields->fID = AEE_ADDRFIELD_NAME;
      pFields->fType = AEEDB_FT_STRING;
      pFields->wDataLen = (num_chars + 1) * sizeof(AECHAR);
      pFields->pBuffer = wBuffer;

      *ppItems = pFields;
      *pnItemCount = 1;
   } else {
      FREE(wBuffer);
      *pnItemCount = 0;
   }

   // Copy & convert digits.
   num_digits = nvruim_phbook_bcd_to_ascii(PHONEBOOK_DN,
                                           &pBuf[adn_tag_size + 2],
                                           digits);

   // If no name and no number, return EFAILED.
   if (!bNonBlankNameFound && !num_digits) {
      return (EFAILED);
   }

   if (pFields) {
      // Memory already allocated, just change the pointer.
      pFields++;
   } else {
      // Allocate memory.
      pFields = (AEEAddrField *)MALLOC(sizeof(AEEAddrField));
      *ppItems = pFields;
   }

   // Extract the phone number info.
   pFields->fID = AEE_ADDRFIELD_PHONE_GENERIC;
   pFields->fType = AEEDB_FT_STRING;

   pFields->wDataLen = (num_digits + 1) * sizeof(AECHAR);
   pFields->pBuffer = MALLOC(pFields->wDataLen);
   wBuffer = (AECHAR *)pFields->pBuffer;

   i = 0;
   while ((i < num_digits) & (digits[i] != 0xff)) {
      wBuffer[i] = (AECHAR)digits[i];
      i++;
   }
   wBuffer[i] = NULL;
   *pnItemCount += 1;

   if (*pnItemCount) {
      return(AEE_SUCCESS);
   } else {
      return(EFAILED);
   }
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
static int  IsSearchMatch(uint16 wRecID,AEEAddrCat *pcat,AEEAddrField ** ppItems, int *pnCount,int *pErr)
{
   int            i,nRet;
   AEEAddrField   *pField;

   nRet = OEMRUIMAddr_RecordGetByID(wRecID,pcat,ppItems, pnCount,pErr);

   pField = *ppItems;


   if(nRet != AEE_SUCCESS)
      return(EFAILED);

   //Match the category IDs
   if(gSearchCat != AEE_ADDR_CAT_NONE && gSearchCat != *pcat)
      goto SearchFailed;

   //Match the FieldIDs
   if(gSearchFieldID != AEE_ADDRFIELD_NONE)
   {
      //Look for the right field ID
      for(i = 0; i < *pnCount; i++)
      {
         if(pField[i].fID == gSearchFieldID)
            break;
      }

      if(i == *pnCount) //Field Not Found
         goto SearchFailed;

      //Match the data
      if(gSearchData == NULL)
         return(AEE_SUCCESS);

      if(pField[i].wDataLen == gSearchDataSize && 
         !MEMCMP((void*)pField[i].pBuffer,(void*)gSearchData,pField[i].wDataLen))
            return(AEE_SUCCESS);
      else
         goto SearchFailed;
   }

   //This is the case where Field ID is not specified.
   if(!gSearchData)
      return(AEE_SUCCESS);


   //FieldID is not specified but data is specified. So, traverse through each field and 
   //check if the data matches
   for(i = 0; i < *pnCount; i++)
   {
      if(pField[i].wDataLen == gSearchDataSize && 
         !MEMCMP((void*)pField[i].pBuffer,(void*)gSearchData,pField[i].wDataLen))
            return(AEE_SUCCESS);
   }

SearchFailed:
   //Free mem returned by RecordGetByID
   OEM_FreeAddressItems(*ppItems,*pnCount);
   return(EFAILED);

}


/*===========================================================================

FUNCTION: IsSearchAllowed

DESCRIPTION

  Helper function to check if search is allowed based on the given search
  criteria. It will also set the flag gbNameSearch to TRUE is this is a
  name-based search

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if search is allowed

SIDE EFFECTS
  none
===========================================================================*/
static int IsSearchAllowed(AEEAddrCat wCategory, AEEAddrFieldID wFieldID)
{

   //Allow search on any combination of Category, FieldID and FieldValue.
   //So, just check if its a name based search and set flag appropriately.

   //Check if it is a search by Name
   if(wFieldID == AEE_ADDRFIELD_NAME)
      gbNameSearch = TRUE;

   return(AEE_SUCCESS);
}


/*===========================================================================

FUNCTION: ClearRUIMAddrBkCache

DESCRIPTION

  This function clears the Cache and frees all memory associated with it.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if search is allowed

SIDE EFFECTS
  none
===========================================================================*/
static int   ClearRUIMAddrBkCache()
{
   RUIMAddrBkCache *ptr;

   if(!gpCache)
      return(AEE_SUCCESS);

   while(gpCache)
   {
      ptr = gpCache;
      gpCache = ptr->pNext;
      if(ptr->szName)
         sys_free(ptr->szName);
      if(ptr->szNumber)
         sys_free(ptr->szNumber);      
      sys_free(ptr);
   }
   return(AEE_SUCCESS);
}

#if !defined(FEATURE_CONTACT_APP)
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
static int AddrBkCompareName(const AECHAR *str1, const AECHAR *str2)
{
   if(str1 && !str2)
      return(1);

   if(!str1 && str2)
      return(-1);

   if(!str1 && !str2)
      return(0);

   return(WSTRICMP(str1,str2));

}
#endif

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
static int   InsertCache(uint16 wRecID,
                         const AECHAR *szName,
                         const AECHAR *szNumber)
{
   RUIMAddrBkCache *pNew,*prev,*cur;
   int nSize;

   //The cache data must be valid when switching between apps. Hence,
   //use sys_malloc
   pNew = (RUIMAddrBkCache*)sys_malloc(sizeof(RUIMAddrBkCache));
   if(!pNew) {
      return(ENOMEMORY);
   }
   
   pNew->store = ADDR_STORE_RUIM;
   pNew->wRecID = wRecID;
   
   if(szName) {
      //Name is specified. Insert at right spot
      nSize = (WSTRLEN(szName)+1)*sizeof(AECHAR);
      pNew->szName = (AECHAR*)sys_malloc(nSize);
      if(!pNew->szName) {
         sys_free(pNew);
         return(EFAILED);
      }
      WSTRCPY(pNew->szName,szName);
   }
   else
      pNew->szName = NULL;
      
   if (szNumber) {
      //Number is specified. Insert at right spot
      nSize = (WSTRLEN(szNumber)+1)*sizeof(AECHAR);
      pNew->szNumber = (AECHAR*)sys_malloc(nSize);
      if(!pNew->szNumber) {
         if (!pNew->szName) {
            sys_free(pNew->szName);
         }
         sys_free(pNew);
         return(EFAILED);
      }
      WSTRCPY(pNew->szNumber,szNumber);
   }
   else
      pNew->szNumber = NULL;
   
   if(!szName && !szNumber) {
      //Null Record do not add
      return(EFAILED);
   }
 
   if(!gpCache) {
      //Empty list.
      pNew->pNext = NULL;
      gpCache = pNew;
      return(AEE_SUCCESS);
   }
   
   //If Name not given, insert at head of list
   if(!szName) {
      pNew->szName = NULL;

      if(!gpCache) {
         pNew->pNext = NULL;
         gpCache = pNew;
      } else {
         pNew->pNext = gpCache;
         gpCache = pNew;
      }
      return(AEE_SUCCESS);
   }

   //Insert at appropriate position
   prev = NULL;
   cur = gpCache;

   while(cur) {
      //Put in list of ascending names.
#if defined(FEATURE_CONTACT_APP)
    if(Appscommon_CompareName((AECHAR *)szName,cur->szName ) >0) {
#else
      if(AddrBkCompareName(szName,cur->szName) >0) {
#endif        
         prev = cur;
         cur = cur->pNext;
      } else {
         if(prev) {
            pNew->pNext = prev->pNext;
            prev->pNext = pNew;
            return(AEE_SUCCESS);
         } else {
            //Insert at beginning of list
            pNew->pNext = gpCache;
            gpCache = pNew;
            return(AEE_SUCCESS);
         }
      }
   }

   prev->pNext = pNew;
   pNew->pNext = NULL;
   return(AEE_SUCCESS);
}

/*===========================================================================

FUNCTION: InitRUIMAddrBkCache

DESCRIPTION

  This function initializes the addressbook-cache. It reads all the records
  from the RUIM phone book and places them inside the cache in a sorted order.
  All untitled records are placed at the beginning of the cache.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: if successful

SIDE EFFECTS
  none
===========================================================================*/
static int   InitRUIMAddrBkCache()
{
   uint16         curRec;
   int            i,nRet,nCount,nErr;
   AEEAddrField   *pFields;
   AEEAddrCat     cat;
   AECHAR         *pPhoneField;
   AECHAR         *pNameField;

#ifndef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
   //Clear existing cache, if any
   ClearRUIMAddrBkCache();
#endif   
#if 0
    if (!OEM_IsRUIMAvailable())
    {
        gAdnMaxRec = 0;
        return(AEE_SUCCESS);
    }
#endif

#ifndef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
    //Get the max. number of possible records on the RUIM.
    gAdnMaxRec = OEMRUIM_phonebook_max_recs();
    if (!gAdnMaxRec) 
    {
        return EFAILED;
    }
#else
    {
        if(db_getuiminitmask()&INITUIMADDMASK)
        {
            //已经在初始化了，则从头重新开始初始化
            gbReInitAddr = TRUE;
        }
        db_setuiminitmask(INITUIMADDMASK);
        return(AEE_SUCCESS);
    }
#endif    

    //Read each record in the address book and place its ID and Name in the
    //appropriate place in the cache.
    curRec = 1;
    while(curRec <= gAdnMaxRec) 
    {
        nRet = OEMRUIMAddr_RecordGetByIDOffDevice(curRec, &cat, &pFields, &nCount, &nErr);

        if(nRet == AEE_SUCCESS) 
        {
            //Find Number
            pPhoneField = NULL;
            for(i = 0; i < nCount; i++) 
            {
                if(pFields[i].fID == AEE_ADDRFIELD_PHONE_GENERIC) 
                {
                    pPhoneField = pFields[i].pBuffer;
                    break;
                }
            }
            
            pNameField = NULL;
            //Find Name
            for(i = 0; i < nCount; i++) 
            {
                if(pFields[i].fID == AEE_ADDRFIELD_NAME) 
                {
                    pNameField = pFields[i].pBuffer;
                    break;
                }
            }
            nRet = InsertCache(curRec, pNameField, pPhoneField);

            //Free mem returned by OEMRUIMAddr_RecordGetByID
            OEM_FreeAddressItems(pFields, nCount);

            if(nRet != AEE_SUCCESS) 
            {
                //Fail to insert into Cache
                ClearRUIMAddrBkCache();
                return(EFAILED);
            }
        }  //Data read successfully from AddressBook
        curRec++;
    } // Process next record
    return(AEE_SUCCESS);
}

/*===========================================================================

FUNCTION: GetAddrFieldName

DESCRIPTION

  This function searches for the "Name" in the address-record and returns the
  name.

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

FUNCTION: GetAddrFieldNumber

DESCRIPTION

  This function searches for the "Number" in the address-record and returns
  the number.

DEPENDENCIES
  none

RETURN VALUE
  Number, if found
  NULL, if record does not have a Number
  if pIndex is NON-NULL, returns the index in *pIndex.

SIDE EFFECTS
  none
===========================================================================*/
static AECHAR *GetAddrFieldNumber(AEEAddrField *pItems,int nCount,int *pIndex)
{
   int i;
   if(!pItems || !nCount)
      return(NULL);
 
   for(i = 0; i < nCount; i++)
   {
      if(pItems[i].fID == AEE_ADDRFIELD_PHONE_GENERIC)
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
static int   RemoveRecCache(uint16 wRecID)
{
   RUIMAddrBkCache   *pCur,*prev,*pRemove;

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
            if(gbEnumInit)
               gwCurRecID = prev->wRecID;
            FreeAddrCacheElem(pRemove);
            return(AEE_SUCCESS);
         }
         else
         {
            //First node to be freed
            gpCache = gpCache->pNext;
            if(gbEnumInit)
               gwCurRecID = AEE_DB_RECID_NULL;
            FreeAddrCacheElem(pRemove);
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
   return(AEE_SUCCESS);
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
static int   UpdateCache(uint16 wrecID,
                         const AECHAR *szName,
                         const AECHAR *szNumber)
{
   int   newRecId;

   //Delete old and add new.
   RemoveRecCache(wrecID);

   newRecId = InsertCache(wrecID,szName,szNumber);
   return(newRecId);
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
static uint16   GetNextRecInCache(uint16 recID)
{
   RUIMAddrBkCache*   ptr;

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
static uint16   GetNextMatchInCache(uint16 recID,AECHAR *szMatch)
{

   RUIMAddrBkCache*   ptr, *pTemp;

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
         if((pTemp->szName == NULL && szMatch == NULL) ||
            ((pTemp->szName != NULL && szMatch != NULL) &&
             !WSTRICMP(pTemp->szName,szMatch)))
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
static void FreeAddrCacheElem(RUIMAddrBkCache *pRemove)
{
   if(!pRemove)
      return;

   if(pRemove->szName)
      sys_free(pRemove->szName);
   if(pRemove->szNumber)
      sys_free(pRemove->szNumber);
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
   RUIMAddrBkCache *ptr = gpCache;

   while(ptr)
   {
      count++;
      ptr = ptr->pNext;
   }
   return(count);
}

/*===========================================================================

FUNCTION: UnusedCacheID

DESCRIPTION

  This function finds the first unused record ID in the cache.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
static uint16 UnusedCacheID(uint16 max_id)
{
   boolean           *nUsedIds;
   int                i;
   RUIMAddrBkCache   *ptr;

   nUsedIds = MALLOC(max_id * sizeof(boolean));
   if (!nUsedIds) {
      return 0;
   }
   MEMSET(nUsedIds, FALSE, (max_id * sizeof(boolean)));
   ptr = gpCache;
   i = 0;
   while (ptr) {
      if (ptr->wRecID <= max_id) {
         nUsedIds[ptr->wRecID - 1] = TRUE;
      }
      ptr = ptr->pNext;
      i++;
   }
   i = 0;
   while((nUsedIds[i]) && (i < max_id)) {
      i++;
   }
   FREE(nUsedIds);
   if (i < max_id) {
      return (i +1);
   } else {
      return 0;
   }
}

/*===========================================================================

FUNCTION: del_adn_rec

DESCRIPTION

  This function deletes a record from the phonebook on the R-UIM.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
static int del_adn_rec(uint16 wRecID)
{
   byte *pBuf;
   int   status;

   pBuf = MALLOC(gAdnRecSize);
   if (!pBuf) {
      return ENOMEMORY;
   }
   MEMSET (pBuf, ADN_FILLER_CHAR, gAdnRecSize);
   status = write_adn_rec(pBuf, wRecID, gAdnRecSize);
   FREE(pBuf);
   return status;
}

/*===========================================================================

FUNCTION: read_adn_rec

DESCRIPTION

  This function reads a record from the phonebook on the R-UIM.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
static int read_adn_rec(byte *pBuf, uint16 wRecID, uint16 wRecSize,
                        AEE_DBRecInfo  *dbRecInfo)
{
   int   status;
   
   gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
   gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
   gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
   gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

   gUimCmd.access_uim.item      = UIM_TELECOM_ADN;
   gUimCmd.access_uim.access    = UIM_READ_F;
   gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
   gUimCmd.access_uim.num_bytes = wRecSize;
   gUimCmd.access_uim.offset    = wRecID;
   gUimCmd.access_uim.data_ptr  = pBuf;

   // Read the entry.
   OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

    if ((gCallBack.rpt_type == UIM_ACCESS_R) && (gCallBack.rpt_status == UIM_PASS) && (pBuf[0] != 0)) 
    {
        status = AEE_SUCCESS;
        dbRecInfo->wRecID = wRecID;
        dbRecInfo->wRecSize = gUimCmd.access_uim.num_bytes_rsp;
        dbRecInfo->dwLastModified = 0;

    } 
    else 
    {
      status = EFAILED;
      dbRecInfo->wRecID = AEE_DB_RECID_NULL;
      dbRecInfo->wRecSize = 0;
      dbRecInfo->dwLastModified = 0;
   }
   return status;
}

/*===========================================================================

FUNCTION: write_adn_rec

DESCRIPTION

  This function writes a record to the phonebook on the R-UIM.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
static int write_adn_rec(const byte *pBuf, uint16 wRecID, uint16 wRecSize)
{
   int status;
   
   gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
   gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
   gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
   gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

   gUimCmd.access_uim.item      = UIM_TELECOM_ADN;
   gUimCmd.access_uim.access    = UIM_WRITE_F;
   gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
   gUimCmd.access_uim.num_bytes = wRecSize;
   gUimCmd.access_uim.offset    = wRecID;
   gUimCmd.access_uim.data_ptr  = (byte *)pBuf;

   // Read the entry.
   OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

   if ((gCallBack.rpt_type == UIM_ACCESS_R)
                                        && (gCallBack.rpt_status == UIM_PASS)) {
      status = AEE_SUCCESS;

   } else {
      status = EFAILED;
   }
   return status;
}

/*==============================================================================
    Extern function
==============================================================================*/
/*=============================================================================
FUNCTION: OEMRUIMAddr_WStrStrEx

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
static boolean OEMRUIMAddr_WStrStrEx( AECHAR *pHaystack, 
                                      AECHAR *pNeedle, 
                                      boolean bCaseInsensitive )
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
            //if ((char1[0] == char2[0] )&&(char1[0]!=(AECHAR)0))
            if ((char1[0] == char2[0] || char1[0] == '*' || char2[0] == '*' )&&(char1[0]!=(AECHAR)0))
            {
                pTempNdl++;
                pTempHay++;
            }
            else
            {
                break;
            }
        }
      
        if ((AECHAR)0 == *pTempNdl) 
        {
            return TRUE;
        }
      
        pHaystack++;
    }
}
/*===========================================================================
FUNCTION : OEMRUIMAddr_EnumCacheInit

DESCRIPTION: Initialize enumeration of OEM cache data
             based on a given search criteria
DEPENDENCIES
  none
  
RETURN VALUE
  SUCCESS     : if success to initilize
  EFAILED     : if failed to initilize
  EBADPARM    : if not pass the parameter
  ENOTALLOWED : if cache not be initilized
    
SIDE EFFECTS
  none
===========================================================================*/
static int      OEMRUIMAddr_EnumCacheInit( AEEAddrCat     wCategory,
                                           AEEAddrFieldID wFieldID, 
                                           void          *pData,
                                           uint16         wDataSize)
{
    PARAM_NOT_REF(wCategory);
    
    gbEnumCacheInit     = FALSE;
    
    //Free data from previous enum
    if(gpCacheSearchData)
    {
        sys_free(gpCacheSearchData);
        gpCacheSearchData = NULL;
    }
    
    // get the search data
    if(pData)
    {
        //The search data must be valid when switching between apps. Hence,
        //use sys_malloc
        gpCacheSearchData = (void *)sys_malloc(wDataSize);
        if(!gpCacheSearchData)
        {
            return(EFAILED);
        }

        MEMCPY(gpCacheSearchData,pData,wDataSize);
        gCacheSearchDataSize = wDataSize;
    };
    
    gCacheSearchFieldID = wFieldID;
    gbEnumCacheInit     = TRUE;
    gpCurCacheElement   = gpCache;
    return SUCCESS;
}// OEMRUIMAddr_EnumCacheInit

/*===========================================================================

FUNCTION : OEMRUIMAddr_EnumNextCache

DESCRIPTION: enumeration of OEM cache data
  pCahe [out]:
DEPENDENCIES
  none
  
RETURN VALUE
  SUCCESS : if success to initilize
  EFAILED : if failed to initilize
  EBADPARM: if not pass the parameter

SIDE EFFECTS
  none
===========================================================================*/
static int      OEMRUIMAddr_EnumNextCache(void **ppCache)
{
    RUIMAddrBkCache *ptr;
    
    if(!ppCache)
    {
        return EBADPARM;
    }
    
    if(gbEnumCacheInit == FALSE)
    {
        return EFAILED;
    }
    
    ptr = gpCurCacheElement;
    
    // Search the match cache
    while(ptr)
    {
        // Search the field
        switch(gCacheSearchFieldID)
        {
            case AEE_ADDRFIELD_NONE:
                // Match all cache whether or not the search data
                *ppCache = (void *)ptr;
                gpCurCacheElement = ptr->pNext;
                return SUCCESS;
                
            case AEE_ADDRFIELD_NAME:
                if(gpCacheSearchData)
                {
                    if(!OEMRUIMAddr_WStrStrEx( ptr->szName, 
                                               gpCacheSearchData, 
                                               TRUE))
                    {
                        break;
                    }
                }//else no search data return success
                
                // Match in the cache
                *ppCache = (void *)ptr;
                gpCurCacheElement = ptr->pNext;
                return SUCCESS;
                
            case AEE_ADDRFIELD_PHONE_GENERIC:
            case AEE_ADDRFIELD_PHONE_HOME:
            case AEE_ADDRFIELD_PHONE_PREF:
            case AEE_ADDRFIELD_PHONE_WORK:
            case AEE_ADDRFIELD_PHONE_FAX:
            case AEE_ADDRFIELD_PHONE_CELL:
                if(gpCacheSearchData)
                {
                    if(!OEMRUIMAddr_WStrStrEx( ptr->szNumber, 
                                               gpCacheSearchData,
                                               FALSE))
                    {
                        // Inverse search
                        if(!OEMRUIMAddr_WStrStrEx( gpCacheSearchData,
                                                   ptr->szNumber,
                                                   FALSE))
                        {
                            break;
                        }
                    }
                }//else no search data return success
                
                // Match in the cache
                *ppCache = (void *)ptr;
                gpCurCacheElement = ptr->pNext;
                return SUCCESS;
                
            default:
                gbEnumCacheInit = FALSE;
                *ppCache = NULL;
                return EFAILED;
                
        }
        ptr = ptr->pNext;
    }
    
    gbEnumCacheInit = FALSE;
    *ppCache = NULL;
    return EFAILED;
}// OEMRUIMAddr_EnumNextCache

/*===========================================================================

FUNCTION : OEMAddr_ExtractCache

DESCRIPTION: Extract the name info from the cache
  pCahe [in] :
  pName [out]: 
DEPENDENCIES
  none
  
RETURN VALUE
  return the record id. return AEE_ADDR_RECID_NULL if error happens
SIDE EFFECTS
  none
===========================================================================*/
static uint16   OEMRUIMAddr_ExtractCache( void       *pCache, 
                                          AECHAR    **ppName,
                                          AEEAddrCat *pCat)
{
    if(pCache == NULL)
    {
        return AEE_ADDR_RECID_NULL;
    }
    
    if(ppName)
    {
        *ppName = ((RUIMAddrBkCache *)pCache)->szName;
    }
    
    if(pCat)
    {
        *pCat = AEE_ADDR_CAT_NONE;
    }
    return ((RUIMAddrBkCache *)pCache)->wRecID;
}// OEMRUIMAddr_ExtractCache

/*===========================================================================

FUNCTION : OEMRUIMAddr_GetCapacity

DESCRIPTION: Get the phone book capacity
  pCahe [in] :
DEPENDENCIES
  none
  
RETURN VALUE
  return capacity of oem define
SIDE EFFECTS
  none
===========================================================================*/
static uint16   OEMRUIMAddr_GetCapacity(void)
{
    return gAdnMaxRec;
}// OEMAddr_GetCapacity

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
extern void DecodeAlphaString(byte *pdata, int nLen, AECHAR *wstrOut, int nBufSize);

static void InsertCache_Ext(RUIMAddrBkCache *pItem)
{
    RUIMAddrBkCache *prev,*cur;
    
    if (NULL == gpCacheHead) 
    {
        //Empty list.
        pItem->pNext = NULL;
        gpCacheHead = pItem;
        return;
    }
    
    if ((NULL == pItem->szName) || (WSTRLEN(pItem->szName) == 0))
    {// 姓名空白，放在链表头部
        pItem->pNext = gpCacheHead;
        gpCacheHead = pItem;
        return;
    }
   
    // 将节点插在适当位置
    prev = NULL;
    cur = gpCacheHead;

    while (cur) 
    {
        //Put in list of ascending names.
#if defined(FEATURE_CONTACT_APP)
        if(Appscommon_CompareName(pItem->szName,cur->szName ) >0)
#else
        if(AddrBkCompareName(pItem->szName,cur->szName) >0) 
#endif            
        {
            prev = cur;
            cur = cur->pNext;
        } 
        else 
        {
            if (prev) 
            {
                pItem->pNext = prev->pNext;
                prev->pNext = pItem;
                return;
            } 
            else 
            {
                //Insert at beginning of list
                pItem->pNext = gpCacheHead;
                gpCacheHead = pItem;
                return;
            }
        }
    }

    prev->pNext = pItem;
    pItem->pNext = NULL;
}

static void OEMRUIM_report_ext (uim_rpt_type *report)
{
   MEMCPY(&gCallBackExt, report, sizeof(uim_rpt_type));
   rex_set_sigs(&uim_tcb, UIMDATA_INIT_SIG);
}

static void read_adn_rec_ext(byte *pBuf, uint16 wRecID, uint16 wRecSize,
                        AEE_DBRecInfo  *dbRecInfo)
{
   MEMSET(&gUimCmdExt, 0, sizeof(gUimCmdExt));
   gUimCmdExt.access_uim.hdr.command            = UIM_ACCESS_F;
   gUimCmdExt.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmdExt.access_uim.hdr.cmd_hdr.sigs       = 0;
   gUimCmdExt.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmdExt.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmdExt.access_uim.hdr.protocol           = UIM_CDMA;
   gUimCmdExt.access_uim.hdr.rpt_function       = OEMRUIM_report_ext;
   
   gUimCmdExt.access_uim.item      = UIM_TELECOM_ADN;
   gUimCmdExt.access_uim.access    = UIM_READ_F;
   gUimCmdExt.access_uim.rec_mode  = UIM_ABSOLUTE;
   gUimCmdExt.access_uim.num_bytes = wRecSize;
   gUimCmdExt.access_uim.offset    = wRecID;
   gUimCmdExt.access_uim.data_ptr  = pBuf;
   
   // Read the entry.
   (void) rex_clr_sigs( &uim_tcb, UIMDATA_INIT_SIG);
   
   // Send the command.
   uim_cmd (&gUimCmdExt);
}


/*==============================================================================
函数:
    InitRUIMAddrBkCacheCb

说明:
    初始卡上电话本的回调函数。

参数:
    none

返回值:
    TRUE : 初始化完成
    FALSE: 初始化未完成

备注:
    注意卡上电话本可能不止开机时会初始化，若支持 UTK ，则 Refresh 命令会导致卡
    上电话本的重新初始化。
        
==============================================================================*/
boolean  InitRUIMAddrBkCacheCb(void)
{
    static uint16  i=0;
    static boolean bInitStart = FALSE;
    static byte *  pBuf = NULL;
    static AEE_DBRecInfo  dbRecInfo;
    static boolean bWaitReport = FALSE;
    int            nAlpha=0;
    RUIMAddrBkCache *pItem = NULL;
    byte           digits[PHONEBOOK_DN];
    int            num_digits;
    boolean        bBlankNum = TRUE;
    boolean        bBlankNam = TRUE;
    int32          nSize;
    db_items_value_type db_item;
    
    db_get(DB_UIMADDINIT, &db_item);
    if (db_item.db_uimaddinited)
    {
        return TRUE;
    }
    
    if (bInitStart == FALSE)
    {
        //Clear existing cache, if any
        ClearRUIMAddrBkCache();
        gpCacheHead = NULL;

        bInitStart = TRUE;
        i = 0;
        // 读记录数及记录的最大尺寸
        pBuf = (byte *)sys_realloc(pBuf, NVRUIM_EF_BUF_SIZE);
        if (NULL == pBuf)
        {
            goto InitRUIMAddrBkCacheCb_Exit;
        }
        MEMSET(pBuf, 0, NVRUIM_EF_BUF_SIZE);
        read_adn_rec_ext(pBuf, 1, 1, NULL);
        bWaitReport = TRUE;
        
        return FALSE;
    }
    
    if ((i == 0) && bWaitReport)
    {
        gAdnMaxRec  = gUimCmdExt.access_uim.num_records_rsp;
        gAdnRecSize = gUimCmdExt.access_uim.num_bytes;
        
        if (0==gAdnMaxRec || gAdnRecSize==0)
        {
            goto InitRUIMAddrBkCacheCb_Exit;
        }
        pBuf = (byte *)sys_realloc(pBuf, gAdnRecSize);
        if (NULL == pBuf)
        {
            goto InitRUIMAddrBkCacheCb_Exit;
        }
        MEMSET(pBuf, 0, gAdnRecSize);
        i++;
        
        bWaitReport = FALSE;
        // 继续下一步
        (void)rex_set_sigs(&uim_tcb, UIMDATA_INIT_SIG);
        return FALSE;
    }
    
    if (bWaitReport == FALSE)
    {
        MEMSET(pBuf, 0, gAdnRecSize);
        read_adn_rec_ext(pBuf, i, gAdnRecSize, &dbRecInfo);
        bWaitReport = TRUE;
        return FALSE;
    }
    
    bWaitReport = FALSE;
    
    if ((gCallBackExt.rpt_type == UIM_ACCESS_R) &&
        (gCallBackExt.rpt_status == UIM_PASS)) 
    {
        dbRecInfo.wRecID = i;
        dbRecInfo.wRecSize = gUimCmdExt.access_uim.num_bytes_rsp;
        dbRecInfo.dwLastModified = 0;
    } 
    else 
    {
        dbRecInfo.wRecID = AEE_DB_RECID_NULL;
        dbRecInfo.wRecSize = 0;
        dbRecInfo.dwLastModified = 0;
        goto InitRUIMAddrBkCacheCb_Exit;
    }
    
    nAlpha = dbRecInfo.wRecSize - PHONEBOOK_TAIL_LENGTH;
    if (nAlpha<0)
    {// 记录不满足强制长度要求
        goto InitRUIMAddrBkCacheCb_Exit;
    }
    
    pItem = (RUIMAddrBkCache *)sys_malloc(sizeof(RUIMAddrBkCache));
    if (NULL == pItem)
    {
        goto InitRUIMAddrBkCacheCb_Exit;
    }
    pItem->store = ADDR_STORE_RUIM;
    pItem->wRecID = i;
    // 解析人名
    if (nAlpha>0)
    {
        nSize = (nAlpha+1) * sizeof(AECHAR);
        pItem->szName = (AECHAR *)sys_malloc(nSize);
        if (NULL != pItem->szName)
        {
            MEMSET(pItem->szName, 0, nSize);
            //nAlpha to (nAlpha + 1), 从卡上读电话本名字，支持最长字节。
            DecodeAlphaString(pBuf, nAlpha, pItem->szName, nAlpha + 1);
            
            if (WSTRLEN(pItem->szName)>0)
            {
                bBlankNam = FALSE;
            }
        }
    }    
    
    // 解析号码
    // Copy & convert digits.
    num_digits = nvruim_phbook_bcd_to_ascii(PHONEBOOK_DN, &pBuf[nAlpha + 2], digits);
    if (num_digits > 0)
    {
        nSize = (num_digits +1) * sizeof(AECHAR);
        pItem->szNumber = (AECHAR *)sys_malloc(nSize);
        if (NULL != pItem->szNumber)
        {
            MEMSET(pItem->szNumber, 0, nSize);
            (void)STRTOWSTR((char *)digits, pItem->szNumber, (num_digits +1) * sizeof(AECHAR));
            if (WSTRLEN(pItem->szNumber)>0)
            {
                bBlankNum = FALSE;
            }
        }
    }
    
    if (bBlankNum && bBlankNam)
    {// 空号码及人名
        if (NULL != pItem->szNumber)
        {
            sys_free(pItem->szNumber);
        }
        if (NULL != pItem->szName)
        {
            sys_free(pItem->szName);
        }
        sys_free(pItem);
    }
    else
    {
        if (TRUE == bBlankNam)
        {
            if (NULL != pItem->szName)
            {
                sys_free(pItem->szName);
            }
        
            nSize = (num_digits +1) * sizeof(AECHAR);
            pItem->szName = (AECHAR *)sys_malloc(nSize);
            WSTRCPY(pItem->szName, pItem->szNumber);
        }
        InsertCache_Ext(pItem);
    }
    
InitRUIMAddrBkCacheCb_Exit:
    i++;
    
    if(gbReInitAddr)
    {
        bInitStart = FALSE;
        gbReInitAddr = FALSE;
        i = 0;
    }
    
    if (i>gAdnMaxRec)
    {
        gpCache = gpCacheHead;
        bInitStart = FALSE;
        if (NULL != pBuf)
        {
            sys_free(pBuf);
            pBuf = NULL;
        }
        i = 0;
        return TRUE;
    }
    else
    {
        // 继续下一步
        (void)rex_set_sigs(&uim_tcb, UIMDATA_INIT_SIG);
        return FALSE;
    }
}
#endif

void OEMRUIMAddr_Refresh(void)
{
    InitRUIMAddrBkCache();
}

/*==============================================================================
函数：
    OEMRUIMAddr_GetCacheinfoByNumber

说明：
    根据号码找出相关 Cache 信息。

参数：
    pwstrNum [in]: 用来查询 Cache 信息的号码
    pCache [in]: 用来返回 Cache 信息的缓冲区
    pfnMactch [in]: 号码匹配规则函数指针
       
返回值：
    查到相关 Cache 信息: SUCCESS
    查询失败: SUCCESS 以外其它值

备注：
       
==============================================================================*/
static int OEMRUIMAddr_GetCacheinfoByNumber(AECHAR *pwstrNum, 
                                            AEEAddCacheInfo *pCacheInfo,
                                            PFN_NUMBERMATCH pfnMactch)
{
    RUIMAddrBkCache *pCache = NULL;
    RUIMAddrBkCache *pCache_Equal = NULL;
    RUIMAddrBkCache *pCache_NumAtCacheTail = NULL;
    RUIMAddrBkCache *pCache_CacheAtNumTail = NULL;
    AECHAR      *pwstrRetnum_Equal = NULL;
    AECHAR      *pwstrRetnum_NumAtCacheTail = NULL;
    AECHAR      *pwstrRetnum_CacheAtNumTail = NULL;
    int         nLen_Equal=0;
    int         nLen_NumAtCacheTail=0;
    int         nLen_CacheAtNumTail=0;
    int         nMatchLen=0;
    
    if(!gpCache)
    {
        return EBADPARM;
    }

    if ((NULL == pwstrNum) || (NULL == pCacheInfo) || (NULL == pfnMactch))
    {
        return(EFAILED);
    }
    
    pCache = gpCache;
    while (pCache != NULL)
    {
        if (pCache->szNumber == NULL)
        {
            pCache = pCache->pNext;
            continue;
        }
        
        switch (pfnMactch(pCache->szNumber, pwstrNum, &nMatchLen))
        {// 号码匹配
            case NUMBERMATCH_EQUAL:                  // 两个号码完全相等
                if (NULL == pCache_Equal)
                {
                    nLen_Equal = nMatchLen;
                    pCache_Equal = pCache;
                    pwstrRetnum_Equal = WSTRDUP(pCache->szNumber);
                }
                break;
                
            case NUMBERMATCH_WSTR1_ISTAIL_OF_WSTR2:  // 号码1是号码2的尾部
                if (NULL == pCache_CacheAtNumTail)
                {
                    nLen_CacheAtNumTail = nMatchLen;
                    pCache_CacheAtNumTail = pCache;
                    pwstrRetnum_CacheAtNumTail = WSTRDUP(pCache->szNumber);
                }
                else if (nMatchLen > nLen_CacheAtNumTail)
                {// 当前号码匹配位数更多
                    FREEIF(pwstrRetnum_CacheAtNumTail);
                    
                    nLen_CacheAtNumTail = nMatchLen;
                    pCache_CacheAtNumTail = pCache;
                    pwstrRetnum_CacheAtNumTail = WSTRDUP(pCache->szNumber);
                }
                break;
                
            case NUMBERMATCH_WSTR2_ISTAIL_OF_WSTR1:  // 号码2是号码1的尾部
                if (NULL == pCache_NumAtCacheTail)
                {
                    pCache_NumAtCacheTail = pCache;
                    pwstrRetnum_NumAtCacheTail = WSTRDUP(pCache->szNumber);
                }
                break;
                
            default:// NUMBERMATCH_IRRELEVANCE       // 两个号码不相关
                break;
        }
        
        // 若最优先的记录已找出，就没必要继续循环了
        if (pCache_Equal != NULL)
        {
            pCache = NULL;
        }
        else
        {
            pCache = pCache->pNext;
        }
    }
    
    // 根据要求返回结果
    if (NULL != pCache_Equal)
    {
        pCache = pCache_Equal;
        pCacheInfo->szNumber = WSTRDUP(pwstrRetnum_Equal);
    }
    else if (NULL != pCache_NumAtCacheTail)
    {
        pCache = pCache_NumAtCacheTail;
        pCacheInfo->szNumber = WSTRDUP(pwstrRetnum_NumAtCacheTail);
    }
    else if (NULL != pCache_CacheAtNumTail)
    {
        pCache = pCache_CacheAtNumTail;
        pCacheInfo->szNumber = WSTRDUP(pwstrRetnum_CacheAtNumTail);
    }
    else
    {
        pCache = NULL;
    }
    
    FREEIF(pwstrRetnum_Equal);
    FREEIF(pwstrRetnum_NumAtCacheTail);
    FREEIF(pwstrRetnum_CacheAtNumTail);
    
    if (NULL != pCache)
    {
        // 填充相关 Cache 信息
        pCacheInfo->wRecID = pCache->wRecID;
        pCacheInfo->szName = WSTRDUP(pCache->szName);
        pCacheInfo->nCat = AEE_ADDR_CAT_NONE;
        
        return SUCCESS;
    }
    
    return EFAILED;
}// OEMRUIMAddr_GetCacheinfoByNumber


uint16   OEMRUIMAddr_GetRUIMMaxNameSize(void)
{
    //return (gAdnRecSize - PHONEBOOK_TAIL_LENGTH -2); 
    return (gAdnRecSize - PHONEBOOK_TAIL_LENGTH);

}
// OEMRUIMAddr_GetRecMaxNameSize

uint16   OEMRUIMAddr_GetRUIMMaxNumberSize(void)
{  
    return (ADN_NUMBER_LEN * 2);
}
// OEMRUIMAddr_GetRUIMMaxNumberSize
/*===========================================================================

FUNCTION OEMRUIMAddr_GetNumRecs()
   Get record count for AT
===========================================================================*/
uint16 AT_OEMRUIMAddr_GetNumRecs(void)
{
   //Get Count from Cache
   return(OEMRUIMAddr_GetNumRecs());
}

/*===========================================================================

FUNCTION AT_RUIMRecordGetOneNumber() 
  Get a the Number of record from RUIM for AT (only number)

===========================================================================*/

byte*  AT_RUIMRecordGetOneNumber(word wpbID)
{
   AECHAR          * pBuf = NULL;
   RUIMAddrBkCache * pCur = NULL;
   int               len = 0;
   int               j = 0;
   
   if(gpCache)
   {
      pCur = gpCache;
      while(pCur && (pCur->wRecID != wpbID)) 
      {
         pCur = pCur->pNext;
      }
      if(!pCur)
      {
         return(NULL);
      }
   }
   else
   {
      return(NULL);
   }
   

   if(pCur->szNumber) 
   {      
      len = (WSTRLEN(pCur->szNumber) + 1) * sizeof(AECHAR);
      pBuf = (AECHAR *) MALLOC(len);
      
      if(!pBuf)
      {  
         return(NULL);
      }
       
      WSTRLCPY(pBuf, pCur->szNumber, len/sizeof(AECHAR)); // Gemsea sizeof IN AECHAR
   }

   return((byte*)pBuf); 

}
/*===========================================================================

FUNCTION AT_RUIMRecordGetOneName()
  Get a the Name of record from RUIM for AT (only name)

===========================================================================*/

byte*  AT_RUIMRecordGetOneName(word wpbID)
{
   AECHAR *          pBuf = NULL;
   RUIMAddrBkCache * pCur = NULL;
   AECHAR            len = 0;
   int               i = 0;
   
   if(gpCache)
   {
      pCur = gpCache;
      while(pCur && (pCur->wRecID != wpbID)) 
      {
         pCur = pCur->pNext;
      }
      if(!pCur)
      {
         return(NULL);
      }
   }
   else
   {
      return(NULL);
   }
   
      
   if(pCur->szName)
   {      
      len = (WSTRLEN(pCur->szName) + 1) * sizeof(AECHAR);
      pBuf = (AECHAR *) MALLOC(len);
      
      if(!pBuf)
      { 
         return(NULL);
      }
       
      WSTRLCPY(pBuf, pCur->szName, len);
   }
   
   return((byte*)pBuf);
 
}

/*===========================================================================

FUNCTION AT_OEMRUIMAddr_RecordAdd() 
   <see OEMAddr_RecordAdd() documentation in OEMAddrBook.h>

===========================================================================*/
uint16 AT_OEMRUIMAddr_RecordAdd(const AECHAR *szName,const AECHAR *szNumber)
{
   //Stretch pItems into a single contiguous memory block so that we can
   //place it onto the R-UIM.
   AEEAddrCat    cat = 0;
   int           nItemCount = 2;
   byte          *pBuf;
   uint16        wRecID;
   uint16        wRecSize;
   int           status;
   int           Err = 0;
   AEEAddrField* pAddrField = NULL;
   AEEAddrField* pAddrField0 = NULL;
   AECHAR*       pBufName = NULL;
   AECHAR*       pBufNumber = NULL;
   int           i = 0;
   //--------------------------------------------------------

   pAddrField = (AEEAddrField *)MALLOC(sizeof(AEEAddrField)*2);
   if( pAddrField == NULL)
   {
        return AEE_ADDR_RECID_NULL;
   }
   pAddrField0 = pAddrField;

   if(cat != AEE_ADDR_CAT_NONE)
      return(AEE_ADDR_RECID_NULL);

    // add name field
   pAddrField->fID = AEE_ADDRFIELD_NAME;
   pAddrField->fType = AEEDB_FT_STRING;
   pAddrField->wDataLen = (WSTRLEN(szName) + 1) * sizeof(AECHAR);
   pAddrField->pBuffer = (AECHAR *) MALLOC(pAddrField->wDataLen);    
   if(!(pAddrField->pBuffer))
   {
      return(AEE_ADDR_RECID_NULL);
   } 
   pBufName = pAddrField->pBuffer;
   WSTRLCPY(pAddrField->pBuffer, szName, pAddrField->wDataLen/sizeof(AECHAR));// Gemsea sizeof IN AECHAR
   pAddrField++;
   

    //add number field
   pAddrField->fID = AEE_ADDRFIELD_PHONE_GENERIC;
   pAddrField->fType = AEEDB_FT_PHONE;
   pAddrField->wDataLen = (WSTRLEN(szNumber) + 1) * sizeof(AECHAR);
   pAddrField->pBuffer = (AECHAR *) MALLOC(pAddrField->wDataLen);    
   if(!(pAddrField->pBuffer))
   {  
      return(AEE_ADDR_RECID_NULL);
   } 
   pBufNumber= pAddrField->pBuffer;
   WSTRLCPY(pAddrField->pBuffer, szNumber, pAddrField->wDataLen/sizeof(AECHAR));// Gemsea sizeof IN AECHAR

   pBuf = AddressToRawData(cat, pAddrField0, nItemCount, &Err, &wRecSize);
   if(!pBuf)
   {
      return(AEE_ADDR_RECID_NULL);
   }
   //Add record to database
  
   wRecID = UnusedCacheID(gAdnMaxRec);
  
   status = AT_write_adn_rec(pBuf, wRecID, wRecSize);  

   if(status != AEE_SUCCESS)
   {
      if(&Err) {
         Err = status;
      }
      return(AEE_ADDR_RECID_NULL);
   }

   //Update cache
   if((InsertCache(wRecID, szName,szNumber)) != AEE_SUCCESS)
   {
      OEMRUIMAddr_RecordDelete(wRecID);

           //Free the buffer
      FREE(pBuf);
      FREE(pBufName); //free number field
      FREE(pBufNumber); //free name field
      FREE(pAddrField);
      return(AEE_ADDR_RECID_NULL);
   }

    //Free the buffer
   FREE(pBuf);
   FREE(pBufName); //free number field
   FREE(pBufNumber); //free name field
   FREE(pAddrField);

   //Success.
   return(wRecID);
}

/*===========================================================================

FUNCTION AT_OEMRUIMAddr_RecordDelete()

===========================================================================*/

int AT_OEMRUIMAddr_RecordDelete(uint16 wRecID)
{
   int status = 999;

   status = AT_del_adn_rec(wRecID);
   if(status == AEE_SUCCESS)
   {
      //Remove from Cache
      RemoveRecCache(wRecID);
      return(AEE_SUCCESS);
   }
   return(EFAILED);
}

/*===========================================================================

FUNCTION: AT_write_adn_rec

DESCRIPTION

  This function writes a record to the phonebook on the R-UIM.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
int AT_write_adn_rec(const byte *pBuf, uint16 wRecID, uint16 wRecSize)
{
   int status;
   
   gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
   gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
   gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
   gUimCmd.access_uim.hdr.rpt_function       = AT_OEMRUIM_report;

   gUimCmd.access_uim.item      = UIM_TELECOM_ADN;
   gUimCmd.access_uim.access    = UIM_WRITE_F;
   gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
   gUimCmd.access_uim.num_bytes = wRecSize;
   gUimCmd.access_uim.offset    = wRecID;
   gUimCmd.access_uim.data_ptr  = (byte *)pBuf;

   // Read the entry.
   //OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);
   (void) rex_clr_sigs( &ds_tcb, DS_UIM_CMD_SIG);
   // Send the command.
   uim_cmd (&gUimCmd);
   (void) rex_wait( DS_UIM_CMD_SIG);

   if ((gCallBack.rpt_type == UIM_ACCESS_R)
                                        && (gCallBack.rpt_status == UIM_PASS)) {
      status = AEE_SUCCESS;

   } else {
      status = EFAILED;
   }
   return status;
}

/*===========================================================================

FUNCTION: AT_del_adn_rec
DESCRIPTION

  This function deletes a record from the phonebook on the R-UIM.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
int AT_del_adn_rec(uint16 wRecID)
{
   byte *pBuf;
   int   status;

   pBuf = MALLOC(gAdnRecSize);
   if (!pBuf) {
      return ENOMEMORY;
   }
   MEMSET (pBuf, ADN_FILLER_CHAR, gAdnRecSize);
   status = AT_write_adn_rec(pBuf, wRecID, gAdnRecSize);
   FREE(pBuf);
   return status;
}

/*===========================================================================

FUNCTION AT_OEMRUIMAddr_RemoveAllRecs()
   <see OEMAddr_RemoveAllRecs() documentation in OEMAddrBook.h>

===========================================================================*/
int AT_OEMRUIMAddr_RemoveAllRecs(void)
{
   /*uint16 wRecIdInit = AEE_DB_RECID_NULL;
   uint16 wRecId = 0;

   while( wRecId!= AEE_DB_RECID_NULL )
   {
      wRecId = GetNextRecInCache(wRecIdInit);  
      if( AT_OEMRUIMAddr_RecordDelete(wRecId) == EFAILED )
      {
          return EFAILED;
      }
      wRecIdInit = wRecId;
   }
   return(AEE_SUCCESS);*/
   return ( AT_OEMRUIMAddr_RemoveAllRecs_UIM() );
}

/*===========================================================================

FUNCTION OEMRUIMAddr_RemoveAllRecs()
   <see OEMAddr_RemoveAllRecs() documentation in OEMAddrBook.h>

===========================================================================*/
int AT_OEMRUIMAddr_RemoveAllRecs_UIM(void)
{
   uint16      wRecId;

   while ((wRecId = GetNextRecInCache(AEE_DB_RECID_NULL))
                                                         != AEE_DB_RECID_NULL) {
      AT_OEMRUIMAddr_RecordDelete(wRecId);
   }
   return(AEE_SUCCESS);
}

/*==============================================================================
函数：
    OEMRUIMAddr_CheckSameRecord

说明：
    重名检查

参数：
    name [in]: 用来查询 Cache 信息的名字
    exist [out]: 用来返回 查询结果
       
返回值： 
    检查成功: SUCCESS
    查询失败: SUCCESS 以外其它值

备注：
       
==============================================================================*/
static int OEMRUIMAddr_CheckSameRecord(AECHAR *name, boolean* exist)
{
    RUIMAddrBkCache *pCache = NULL;
    AECHAR cache_name[32]={0};
    AECHAR cmp_name[32]={0};
    
    if(!gpCache)
    {
        return EBADPARM;
    }

    if ((NULL == name) || (NULL == exist))
    {
        return(EFAILED);
    }
    
    pCache = gpCache;
    while (pCache != NULL)
    {
        if (pCache->szName == NULL)
        {
            pCache = pCache->pNext;
            continue;
        }

        //大小写不敏感
        if(!HaveNoneASCIIChar(name, NULL) && !HaveNoneASCIIChar(pCache->szName, NULL))
        {
            WSTRCPY(cache_name, pCache->szName);
            WSTRCPY(cmp_name, name);
            WSTRLOWER(cache_name);
            WSTRLOWER(cmp_name);

            if(WSTRCMP(cache_name, cmp_name) == 0)
            {
                *exist = TRUE;
                return SUCCESS;
            }
        }

        if(WSTRCMP(pCache->szName, name) == 0)
        {
            *exist = TRUE;
            return SUCCESS;
        }
        
        pCache = pCache->pNext;
    }
    
    return EFAILED;
}// OEMAddr_CheckSameRecord

/*===========================================================================

FUNCTION OEMRUIMAddr_ReadADNByID()

===========================================================================*/
int OEMRUIMAddr_ReadADNByID(uint16 wRecID, AECHAR **ppName, AECHAR **ppNumber)
{
   byte *         pBuf;
   AEE_DBRecInfo  dbRecInfo;
   int            nRet;
   AEEAddrField  *pItems = NULL;
   int            nItemCount = 0;
   int            i;

   if(!OEM_IsCDMASVCSupport(UIM_CDMA_SVC_ADN))
   {
      return EFAILED;
   }
   
   pBuf = MALLOC(gAdnRecSize);
   if (!pBuf) {
      return ENOMEMORY;
   }
   nRet = read_adn_rec(pBuf, wRecID, gAdnRecSize, &dbRecInfo);
   if (nRet != AEE_SUCCESS) {
      FREE (pBuf);
      return EFAILED;
   }

   *ppName = NULL;
   *ppNumber = NULL;
   
   //Parse the buffer into individual address items
   nRet = RawDataToAddress(pBuf, dbRecInfo.wRecSize, NULL, &pItems, &nItemCount, NULL);
   
   FREE(pBuf);
   for(i=0;i<nItemCount;i++)
   {
      if(pItems[i].fID == AEE_ADDRFIELD_NAME)
      {
         if(*ppName)
         {
            FREE(*ppName);
         }
         *ppName = pItems[i].pBuffer;
      }
      else if(pItems[i].fID == AEE_ADDRFIELD_PHONE_GENERIC)
      {
         if(*ppNumber)
         {
            FREE(*ppNumber);
         }
         *ppNumber = pItems[i].pBuffer;
      }
      else
      {
         if(pItems[i].pBuffer)
         {
            FREE(pItems[i].pBuffer);
         }
      }
   }
   
   if(pItems)
   {
      FREE(pItems);
      pItems = NULL;
   }
   return(nRet);
}

int OEMRUIMAddr_WriteADNByID(uint16 wRecID, AECHAR *pName, AECHAR *pNumber)
{
    int nRet;
    uint16 wRecSize;
    byte *pBuf;
    AEEAddrField myItems[2];

    myItems[0].fID      = AEE_ADDRFIELD_NAME;
    myItems[0].fType    = AEEDB_FT_STRING;
    myItems[0].pBuffer  = pName;
    myItems[0].wDataLen = (WSTRLEN(pName)+1)* sizeof(AECHAR);
    myItems[1].fID      = AEE_ADDRFIELD_PHONE_GENERIC;
    myItems[1].fType    = AEEDB_FT_STRING;
    myItems[1].pBuffer  = pNumber;
    myItems[1].wDataLen = (WSTRLEN(pNumber)+1)* sizeof(AECHAR);

    pBuf = AddressToRawData(0, myItems, 2, &nRet, &wRecSize);
    if(!pBuf)
    {
        return ENOMEMORY;
    }
    
    nRet = write_adn_rec(pBuf, wRecID, wRecSize);
    FREE(pBuf);
    return nRet;
}

#endif    // FEATURE_ADDRBOOK_RUIM   


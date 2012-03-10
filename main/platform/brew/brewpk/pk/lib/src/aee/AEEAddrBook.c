/*======================================================
FILE:  AEEAddrBook.c

SERVICES:  AEE Address Book Interface

GENERAL DESCRIPTION:
   Implements the functions in the AEE Address Book interface

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright ?1999-2001 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEEAddrBook.h"
#include "OEMAddrBook.h"
#include "OEMHeap.h"
//#include "AEEPriv.h"
//#include "AEEShell_priv.h"
//#include "AEE_MIF.h"
#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "AEEStdLib.h"
#include "OEMClassIDs.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef struct _AEEAddrBook      AEEAddrBook;
typedef struct _AEEAddrRec       AEEAddrRec;


   // 
   // NOTE
   //
   // We use the Shell's mechanism to register this object if it's associated
   // with a module/app.  This is done only for those objects that allocate underlying
   // system services (files, sockets, etc.).  It is provided so that the resources
   // are deallocated when the underlying module for an applet is closed.
   //
struct _AEEAddrBook
{
   DECLARE_VTBL(IAddrBook)

   AEECLSID          m_ClsId; // Class ID of this address book object
   AEEAddrBook *     m_pNext;
   int32             m_nRefs;
   AEEAddrRec   *    m_pRecList;
   IOEMAddrBook *    m_pIOEMAddrBk;
   IShell *          m_pShell;
   int               m_nErrno;  
   AEEAddrCat*       m_pCatList; // List of categories supported
   int               m_nCurCatIndex; // current Index. used in enumerations
   int               m_nCatCount;   // Number of categories supported
   AEEAddrCat        m_nCachedFieldInfoCat;  //Category for which the fields-info has been cached
   int               m_nCurFieldInfoIndx; //index.used in enumerations of fields-info
   AEEAddrFieldInfo* m_pFieldInfo;        //cached fields-info
   int               m_nFieldInfoCount; // Number of fields for current cat
};

struct _AEEAddrRec
{
   DECLARE_VTBL(IAddrRec)

   AEEAddrRec *   m_pNext;
   int32          m_nRefs;
   AEEAddrBook  * m_pAddrBook;
   uint16         m_wrecID;   //RecID of this record
   AEEAddrCat     m_addrCat;  //Category of this record
   AEEAddrField * m_pFields;  //List of fields in this record
   int            m_nFieldCount; // number of fields in this record.
   int            m_nErrno;
};


//
//IAddrBook interface functions
//
static   uint32           AEEAddrBook_AddRef(IAddrBook * po);
static   uint32           AEEAddrBook_Release(IAddrBook * po);
static   int              AEEAddrBook_EnumCategoryInit(IAddrBook * po);
static   boolean          AEEAddrBook_EnumNextCategory(IAddrBook * po, AEEAddrCat * pc);
static   int              AEEAddrBook_EnumFieldsInfoInit(IAddrBook *po,AEEAddrCat c);
static   boolean          AEEAddrBook_EnumNextFieldsInfo(IAddrBook *po,AEEAddrFieldInfo *pf);
static   IAddrRec *       AEEAddrBook_CreateRec(IAddrBook * po, AEEAddrCat c, AEEAddrField * pItems, int nItemCount);
static   uint16           AEEAddrBook_GetNumRecs(IAddrBook * po);  
static   int              AEEAddrBook_RemoveAllRecs(IAddrBook* po);
static   IAddrRec *       AEEAddrBook_GetRecByID(IAddrBook * po, uint16 wID);
static   int              AEEAddrBook_EnumRecInit(IAddrBook * po,  AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize);
static   IAddrRec *       AEEAddrBook_EnumNextRec(IAddrBook * po);
static   int              AEEAddrBook_GetLastError(IAddrBook *po);
static   void             AEEAddrBook_SetProperties(IAddrBook *po, uint32 dwProps);
static   uint32           AEEAddrBook_GetProperties(IAddrBook *po);
static   int              AEEAddrBook_GetCategoryName(IAddrBook * po, AEEAddrCat c, AECHAR *pszCatName, int *pnSize);
static   int              AEEAddrBook_GetFieldName(IAddrBook * po, AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize);


//
//IAddrRec interface functions
//
static   uint32           AEEAddrRec_AddRef(IAddrRec * po);
static   uint32           AEEAddrRec_Release(IAddrRec * po);
static   AEEAddrCat       AEEAddrRec_GetCategory(IAddrRec* po);
static   int              AEEAddrRec_SetCategory(IAddrRec *po,AEEAddrCat cat);
static   uint16           AEEAddrRec_GetRecID(IAddrRec * po);
static   int              AEEAddrRec_GetFieldCount(IAddrRec *);                
static   AEEAddrField *   AEEAddrRec_GetField(IAddrRec * po, int nFieldIdx);
static   int              AEEAddrRec_UpdateField(IAddrRec *po, int nFieldIdx, AEEAddrField *);
static   int              AEEAddrRec_UpdateAllFields(IAddrRec *po, AEEAddrField *pFields, int nFieldCount);
static   int              AEEAddrRec_AddField(IAddrRec* po, AEEAddrField * pField);
static   int              AEEAddrRec_RemoveField(IAddrRec *po, int nFieldIdx);
static   int              AEEAddrRec_RemoveRec(IAddrRec * po);
static   int              AEEAddrRec_GetLastError(IAddrRec *po);

static   IBase *          AddrBookCls_New(IShell * pShell, AEECLSID ClsId);
static   IAddrRec *       AEEAddrRec_New(AEEAddrBook * pAddrBook);



//Helper functions
static int              AEEAddrRec_SetInfo(IAddrRec *po, uint16 recID,AEEAddrCat c, AEEAddrField * pFields, int nFieldCount,boolean bAllocated,boolean bFreeExisting);
static void             AEEAddrRec_FreeBuffer(AEEAddrRec * pme);
static void             AEEAddrRec_Delete(AEEAddrRec * pme);
static AEEAddrField*    AEEAddrRec_CopyField(AEEAddrField *pDst,AEEAddrField* pSrc);
static void             FreeAddrField(AEEAddrField *pItems,int nItemCount);  
static IAddrRec*        AEEAddrRecFromData(IAddrBook *, uint16 wID, AEEAddrCat cat,AEEAddrField *pItems, int nItemCount,int *pnErr);
#ifdef CUST_EDITION	
static int     AEEAddrBook_EnumCacheInit( IAddrBook *po, AEEAddrCat wCategory, AEEAddrFieldID wFieldID,  void *pData, uint16 wDataSize);
static int     AEEAddrBook_EnumNextCache(IAddrBook * po, void **ppCache);
static uint16  AEEAddrBook_ExtractCache( IAddrBook  *po, void *pCache, AECHAR **ppName, AEEAddrCat *pCat);
static uint16  AEEAddrBook_GetCapacity(IAddrBook * po);

static int AEEAddrBook_GetCacheinfoByNumber(IAddrBook  *po,
                                            AECHAR *pwstrNum, 
                                            AEEAddCacheInfo *pCacheInfo,
                                            PFN_NUMBERMATCH pfnMactch);
static int AEEAddrBook_CheckSameRecord(IAddrBook  *po,
                                            AECHAR *name, 
                                            boolean *exist);
#endif /*CUST_EDITION*/
// Constant Data...
//

#ifdef CUST_EDITION	
static const VTBL(IAddrBook)  gAEEAddrBookFuncs = {AEEAddrBook_AddRef,
                                                   AEEAddrBook_Release,
                                                   AEEAddrBook_EnumCategoryInit,
                                                   AEEAddrBook_EnumNextCategory,
                                                   AEEAddrBook_EnumFieldsInfoInit,
                                                   AEEAddrBook_EnumNextFieldsInfo,
                                                   AEEAddrBook_CreateRec,
                                                   AEEAddrBook_GetNumRecs,
                                                   AEEAddrBook_RemoveAllRecs,
                                                   AEEAddrBook_GetRecByID,
                                                   AEEAddrBook_EnumRecInit,
                                                   AEEAddrBook_EnumNextRec,
                                                   AEEAddrBook_GetLastError,
                                                   AEEAddrBook_SetProperties,
                                                   AEEAddrBook_GetProperties,
                                                   AEEAddrBook_GetCategoryName,
                                                   AEEAddrBook_GetFieldName,
                                                   AEEAddrBook_EnumCacheInit,
                                                   AEEAddrBook_EnumNextCache,
                                                   AEEAddrBook_ExtractCache,
                                                   AEEAddrBook_GetCapacity,
                                                   AEEAddrBook_GetCacheinfoByNumber,
                                                   AEEAddrBook_CheckSameRecord
							};       
#else
static const VTBL(IAddrBook)  gAEEAddrBookFuncs = {AEEAddrBook_AddRef,
                                                   AEEAddrBook_Release,
                                                   AEEAddrBook_EnumCategoryInit,
                                                   AEEAddrBook_EnumNextCategory,
                                                   AEEAddrBook_EnumFieldsInfoInit,
                                                   AEEAddrBook_EnumNextFieldsInfo,
                                                   AEEAddrBook_CreateRec,
                                                   AEEAddrBook_GetNumRecs,
                                                   AEEAddrBook_RemoveAllRecs,
                                                   AEEAddrBook_GetRecByID,
                                                   AEEAddrBook_EnumRecInit,
                                                   AEEAddrBook_EnumNextRec,
                                                   AEEAddrBook_GetLastError,
                                                   AEEAddrBook_SetProperties,
                                                   AEEAddrBook_GetProperties,
                                                   AEEAddrBook_GetCategoryName,
                                                   AEEAddrBook_GetFieldName};							
#endif /*CUST_EDITION*/
static const VTBL(IAddrRec)  gAEEAddrRecFuncs = {AEEAddrRec_AddRef,
                                                 AEEAddrRec_Release,
                                                 AEEAddrRec_GetCategory,
                                                 AEEAddrRec_SetCategory,
                                                 AEEAddrRec_GetRecID,
                                                 AEEAddrRec_GetFieldCount,
                                                 AEEAddrRec_GetField,
                                                 AEEAddrRec_UpdateField,
                                                 AEEAddrRec_UpdateAllFields,
                                                 AEEAddrRec_AddField,
                                                 AEEAddrRec_RemoveField,
                                                 AEEAddrRec_RemoveRec,
                                                 AEEAddrRec_GetLastError};

//
// Static Data
//
// Common IAddrBook object used to access the main or EFS-based address book
static AEEAddrBook                gsAddrBookMain = {0};
// Common IAddrBook object used to access the RUIM-based address book
static AEEAddrBook                gsAddrBookRUIM = {0};



/*===========================================================================

FUNCTION AEEAddrBook_New

DESCRIPTION
  

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
int AEEAddrBook_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj)
{
   IBase * pobj;

   pobj = AddrBookCls_New(pIShell, ClsId);

   *ppObj = pobj;
   if(pobj){
      IBASE_AddRef(pobj);
      return(0);
   }
   return(-1);
}


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
static IBase * AddrBookCls_New(IShell * pShell, AEECLSID ClsId)
{
   AEECLSID cls;
   int err;
   ACONTEXT *pacRestore;
   AEEAddrBook *pAddrBook;


   switch (ClsId)
   {
      case AEECLSID_RUIM:
      case AEECLSID_ADDRBOOK_RUIM:
         cls = AEECLSID_OEMRUIMADDRBOOK;
         pAddrBook = &gsAddrBookRUIM;
         break;

      case AEECLSID_ADDRBOOK:
      case AEECLSID_ADDRBOOK_21:
      case AEECLSID_ADDRBOOK_20:
      default:
         cls = AEECLSID_OEMADDRBOOK;
         pAddrBook = &gsAddrBookMain;
         break;
   }

   if(!pAddrBook->m_nRefs)
   {
      pacRestore = AEE_EnterAppContext(0);
      err = ISHELL_CreateInstance(pShell, cls, (void **)(&pAddrBook->m_pIOEMAddrBk));
      AEE_LeaveAppContext(pacRestore);

      if(SUCCESS == err)
      {
         INIT_VTBL(pAddrBook, IAddrBook, gAEEAddrBookFuncs);

         //Initialize class members
         pAddrBook->m_pShell = pShell;
         pAddrBook->m_nCatCount = 0;
         pAddrBook->m_pCatList = NULL;
         pAddrBook->m_pFieldInfo = NULL;
         pAddrBook->m_ClsId = ClsId;
      }
      else
      {
         return NULL;
      }

   }
   else
      // Reset properties because they are shared by all instances
#ifndef WIN32//wlh 临时修改
      IOEMADDR_SetProperties(pAddrBook->m_pIOEMAddrBk,AEE_ADDR_DEFAULT_PROPERTIES);
#endif//WIN32
   return((IBase *)pAddrBook);
}



/*===========================================================================

                      AEEAddrBook INTERFACE

===========================================================================*/

/*===========================================================================

FUNCTION AEEAddrBook_AddRef

DESCRIPTION
  Increments the reference count for the heap class.   

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static uint32 AEEAddrBook_AddRef(IAddrBook * po)
{
   AEEAddrBook * pme = (AEEAddrBook *)po;
   
   return(++pme->m_nRefs);
}

/*===========================================================================

FUNCTION AEEAddrBook_Release

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static uint32 AEEAddrBook_Release(IAddrBook * po)
{
   AEEAddrBook * pme = (AEEAddrBook *)po;

   if(pme->m_nRefs){
      if(--pme->m_nRefs)
         return(pme->m_nRefs);

      if(pme->m_pCatList)
         sys_free(pme->m_pCatList);

      if(pme->m_pFieldInfo)
         sys_free(pme->m_pFieldInfo);

      if(pme->m_pIOEMAddrBk)
      {
         IOEMADDR_Release(pme->m_pIOEMAddrBk);
         pme->m_pIOEMAddrBk = NULL;
      }
   }
   return(0);
}


/*===========================================================================

FUNCTION : AEEAddrBook_EnumCategoryInit

DESCRIPTION: Initialize enumeration of categories supported in the address book

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrBook_EnumCategoryInit(IAddrBook * po)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;

   //See if we've already got the information
   if(!pme->m_pCatList)
   {
      //Get numnber of categories supported
      pme->m_nCatCount = IOEMADDR_GetCatCount(pme->m_pIOEMAddrBk);

      if(!pme->m_nCatCount)
         return(AEE_ADDRBOOK_NOCATSUPPORT);

      //Allocate mem to store cat-list. This can be shared between multiple apps. So
      //use sys_malloc
      pme->m_pCatList = (AEEAddrCat *)sys_malloc(pme->m_nCatCount*sizeof(AEEAddrCat));
      
      if(!pme->m_pCatList)
         return(ENOMEMORY);
      
      //Get the actual cat-list
      if (IOEMADDR_GetCatList(pme->m_pIOEMAddrBk, pme->m_pCatList, pme->m_nCatCount) != AEE_SUCCESS)
      {
         sys_free(pme->m_pCatList);
         return(EFAILED);
      }
   }
   //Initialize the index
   pme->m_nCurCatIndex = 0;
   return(AEE_SUCCESS);
}

/*===========================================================================

FUNCTION : AEEAddrBook_EnumNextCategory

DESCRIPTION: Enumerate categories supported in the address book

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   boolean  AEEAddrBook_EnumNextCategory(IAddrBook * po, AEEAddrCat * pc)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;

   //Validate
   if(!pc || !pme->m_pCatList || pme->m_nCurCatIndex >= pme->m_nCatCount)
      return(FALSE);

   //Return current cat and update index
   *pc = pme->m_pCatList[pme->m_nCurCatIndex++];

   return(TRUE);
}


/*===========================================================================

FUNCTION : AEEAddrBook_GetCategoryName

DESCRIPTION: Get the string name of a category

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int AEEAddrBook_GetCategoryName(IAddrBook * po, AEEAddrCat c, AECHAR *pCatName, int *pnSize)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;

   //Validate parameters
   if (!AEE_CHECK_PTR("IADDRBOOK_GetCategoryName", pnSize, sizeof(int), TRUE))
   {
      return EMEMPTR;
   }

   return IOEMADDR_GetCategoryName(pme->m_pIOEMAddrBk, c, pCatName, pnSize);
}


/*===========================================================================

FUNCTION : AEEAddrBook_EnumFieldsInfoInit

DESCRIPTION: Initialize enumeration of information about fields supported in the
address book

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrBook_EnumFieldsInfoInit(IAddrBook *po,AEEAddrCat c)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;
   int nCount;

   //See if we have already cached this information
   if(!pme->m_pFieldInfo || pme->m_nCachedFieldInfoCat != c)
   {
      //Free previous cache
      if(pme->m_pFieldInfo)
         sys_free(pme->m_pFieldInfo);
      
      pme->m_pFieldInfo = NULL;
      
      //Get number of fields supported for this category
      nCount = IOEMADDR_GetFieldInfoCount(pme->m_pIOEMAddrBk, c);
      if(!nCount)
         return(AEE_ADDR_CAT_NOT_SUPPORTED);

      //Allocate mem to hold fields info. This can be shared by multiple apps. So,
      //do not use MALLOC
      pme->m_pFieldInfo = (AEEAddrFieldInfo*)sys_malloc(nCount * sizeof(AEEAddrFieldInfo));
      if(!pme->m_pFieldInfo)
         return(ENOMEMORY);

      //Get actual fields info
      if(IOEMADDR_GetFieldInfo(pme->m_pIOEMAddrBk, c, pme->m_pFieldInfo, nCount)
                                                                 != AEE_SUCCESS)
      {
         sys_free(pme->m_pFieldInfo);
         pme->m_pFieldInfo =NULL;
         return(AEE_ADDR_CAT_NOT_SUPPORTED);
      }
      //Set-up cache
      pme->m_nFieldInfoCount = nCount;
      pme->m_nCachedFieldInfoCat = c;
   }
   //Initialize index for enumerations
   pme->m_nCurFieldInfoIndx = 0;
   return(AEE_SUCCESS);
}


/*===========================================================================

FUNCTION : AEEAddrBook_EnumNextFieldsInfo

DESCRIPTION: Enumerate information about fields supported in the
address book

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   boolean  AEEAddrBook_EnumNextFieldsInfo(IAddrBook *po,AEEAddrFieldInfo *pf)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;
   int index;

   //Validate
   if(!pf || !pme->m_pFieldInfo || pme->m_nCurFieldInfoIndx >= pme->m_nFieldInfoCount)
      return(FALSE);

   index = pme->m_nCurFieldInfoIndx;

   //Copy info to given parameter
   pf->fieldID = pme->m_pFieldInfo[index].fieldID;
   pf->nMaxItems = pme->m_pFieldInfo[index].nMaxItems;
   pf->bFieldCatSearch = pme->m_pFieldInfo[index].bFieldCatSearch;
   pf->bFieldSearch = pme->m_pFieldInfo[index].bFieldSearch;
   pf->fType = pme->m_pFieldInfo[index].fType;

   //Update index
   pme->m_nCurFieldInfoIndx++;

   return(TRUE);
}


/*===========================================================================

FUNCTION : AEEAddrBook_GetFieldName

DESCRIPTION: Get the name of a field in string format

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int AEEAddrBook_GetFieldName(IAddrBook * po, AEEAddrFieldID f, AECHAR *pFieldName, int *pnSize)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;

   //Validate parameters
   if (!AEE_CHECK_PTR("IADDRBOOK_GetFieldName", pnSize, sizeof(int), TRUE))
   {
      return EMEMPTR;
   }

   return IOEMADDR_GetFieldName(pme->m_pIOEMAddrBk, f, pFieldName, pnSize);
}


/*===========================================================================

FUNCTION : AEEAddrBook_GetLastError

DESCRIPTION: Get last error

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrBook_GetLastError(IAddrBook *po)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;
   return(pme->m_nErrno);
}


/*===========================================================================

FUNCTION : AEEAddrBook_SetProperties

DESCRIPTION: Set properties

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void AEEAddrBook_SetProperties(IAddrBook *po, uint32 dwProps)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;

   IOEMADDR_SetProperties(pme->m_pIOEMAddrBk, dwProps);   
}


/*===========================================================================

FUNCTION : AEEAddrBook_GetProperties

DESCRIPTION: Get properties

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static uint32 AEEAddrBook_GetProperties(IAddrBook *po)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;

   return IOEMADDR_GetProperties(pme->m_pIOEMAddrBk);   
}


/*===========================================================================

FUNCTION : AEEAddrBook_CreateRec

DESCRIPTION: Create a record and return pointer to the IAddrRec interface

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   IAddrRec *  AEEAddrBook_CreateRec(IAddrBook * po, AEEAddrCat c, AEEAddrField * pFields, int nFieldCount)
{
   IAddrRec *  pr;
   uint16      recID = AEE_ADDR_RECID_NULL;
   int         nErr;
   AEEAddrBook *pme = (AEEAddrBook*)po;

   //Validate parameters
   if (!pFields || (nFieldCount <= 0))
      return(NULL);


   //Create a new IAddrRec interface
   if((pr = AEEAddrRec_New((AEEAddrBook *)po)) != NULL)
   {
      //Store Info in our Record and then add into OEM Address Book
      if(AEEAddrRec_SetInfo(pr, recID,c,pFields, nFieldCount,FALSE,TRUE) != AEE_SUCCESS)
      {
         //Error adding to Address Book. Release Record
         AEEAddrRec_Release(pr);
         pme->m_nErrno = ENOMEMORY;
         return(NULL);
      }

	 
      
      //Add record into address book
      recID = IOEMADDR_RecordAdd(pme->m_pIOEMAddrBk, c, pFields, nFieldCount,
                                                                         &nErr);

      if(recID == AEE_ADDR_RECID_NULL)
      {
         //Error adding to Address Book. Release Record
         pme->m_nErrno = nErr;

         AEEAddrRec_Release(pr);
         return(NULL);
      }
      //Store the record ID
      ((AEEAddrRec *)pr)->m_wrecID = recID;
     
   }
   else
      pme->m_nErrno = ENOMEMORY;

   return(pr);

}

/*===========================================================================

FUNCTION : AEEAddrBook_GetNumRecs

DESCRIPTION: Get number of records in address book

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   uint16   AEEAddrBook_GetNumRecs(IAddrBook * po)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;

   return IOEMADDR_GetNumRecs(pme->m_pIOEMAddrBk);
}


/*===========================================================================

FUNCTION : AEEAddrBook_RemoveAllRecs

DESCRIPTION: Remove all records in the address book

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrBook_RemoveAllRecs(IAddrBook* po)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;

   return IOEMADDR_RemoveAllRecs(pme->m_pIOEMAddrBk);
}

/*===========================================================================

FUNCTION : AEEAddrBook_GetRecByID

DESCRIPTION: Given the record ID, retrieve the record with that ID and return the
corresponding IAddrRec ptr

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   IAddrRec *  AEEAddrBook_GetRecByID(IAddrBook * po, uint16 wID)
{
   AEEAddrBook   *pme = (AEEAddrBook *)po;
   AEEAddrCat     cat;
   AEEAddrField * pItems = NULL;
   int            nItemCount = 0;
   int            nErr;
   IAddrRec *     pr = NULL;
   int            ret;
   
   //First, get Record from OEM
   ret = IOEMADDR_RecordGetByID(pme->m_pIOEMAddrBk, wID,&cat,&pItems,
                                                             &nItemCount,&nErr);

   if(ret != AEE_SUCCESS)
      return(NULL);

   pr = AEEAddrRecFromData(po,wID,cat,pItems, nItemCount,&nErr);

   return(pr);
}

/*===========================================================================

FUNCTION : AEEAddrBook_EnumRecInit

DESCRIPTION: Initialize enumeration of records based on a given search criteria

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrBook_EnumRecInit(IAddrBook * po,  AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;

   return (IOEMADDR_EnumRecInit(pme->m_pIOEMAddrBk, wCategory, wFieldID, pData,
                                                                    wDataSize));
}

/*===========================================================================

FUNCTION : AEEAddrBook_EnumNextRec

DESCRIPTION: Retrieve next record that mathces the search criteria initialized
in previous call to AEEAddrBook_EnumRecInit

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   IAddrRec *  AEEAddrBook_EnumNextRec(IAddrBook * po)
{
   AEEAddrBook   *pme = (AEEAddrBook *)po;
   AEEAddrCat     cat;
   AEEAddrField   *pField;
   int            nItemCount;
   int            nErr;
   uint16         wRecID;
   IAddrRec     * pr;


   wRecID =  IOEMADDR_EnumNextRec(pme->m_pIOEMAddrBk, &cat,&pField, &nItemCount,
                                                                         &nErr);

   if(wRecID == AEE_ADDR_RECID_NULL)
      return(NULL);

   pr = AEEAddrRecFromData(po,wRecID,cat,pField, nItemCount,&nErr);
   return(pr);

}




/*===========================================================================

                      Address RECORD INTERFACE

===========================================================================*/

/*===========================================================================

FUNCTION : AEEAddrRec_New

DESCRIPTION: Create a new instance of IAddrRec class

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static IAddrRec *  AEEAddrRec_New(AEEAddrBook * pAddrBook)
{
   AEEAddrRec   * pme;
  
   if((pme = (AEEAddrRec *)AEE_NewClass((IBaseVtbl *)&gAEEAddrRecFuncs, sizeof(AEEAddrRec))) != NULL)
   {

      
      pme->m_pAddrBook = pAddrBook;
      
      // Set it in the list.  This allows us to free automatically if the refcount drops to 0...
      pme->m_pNext    = pAddrBook->m_pRecList;
      pAddrBook->m_pRecList = pme;

      pme->m_nFieldCount = 0;
      pme->m_pFields = NULL;

      // Removing this like will cause an error when they want to 
      // use the record but close the database!  This will cause unexpected
      // behavior.  Please make sure you check this stuff before doing this type
      // of thing!
      
      AEEAddrBook_AddRef((IAddrBook *)pAddrBook);
   }      
   return((IAddrRec *)pme);
}

/*===========================================================================

FUNCTION AEEAddrRec_AddRef

DESCRIPTION
  Increments the reference count for the heap class.   

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static uint32 AEEAddrRec_AddRef(IAddrRec * po)
{
   AEEAddrRec * pme = (AEEAddrRec *)po;
   
   return(++pme->m_nRefs);
}

/*===========================================================================

FUNCTION AEEAddrRec_Release

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static uint32 AEEAddrRec_Release(IAddrRec * po)
{
   AEEAddrRec   *pme = (AEEAddrRec *)po;
   
   if(pme->m_nRefs > 0){
      if(--pme->m_nRefs)
         return(pme->m_nRefs);
      
      AEEAddrRec_Delete(pme);
   }
   return(0);
}

/*===========================================================================

FUNCTION AEEAddrRec_Delete

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void AEEAddrRec_Delete(AEEAddrRec * pme)
{
   AEEAddrBook *    pdb;
   AEEAddrRec *    pPrev;
   AEEAddrRec *    pNext;
   
   if((pdb = pme->m_pAddrBook) != NULL){
      
      // Remove it from the list of records...
      
      pNext = pme->m_pNext;
      pPrev = pdb->m_pRecList;
      if(pPrev != pme){
         for(; pPrev != NULL ; pPrev = pPrev->m_pNext){
            if(pPrev->m_pNext == pme){
               pPrev->m_pNext = pNext;
               break;
            }
         }
      }
      else
         pdb->m_pRecList = pNext;
      
      AEEAddrRec_FreeBuffer(pme);
      AEEAddrBook_Release((IAddrBook *)pdb);
   }

   FREE(pme);
}


/*===========================================================================

FUNCTION AEEAddrRec_GetCategory

DESCRIPTION: Get the address category of this recotrd


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   AEEAddrCat  AEEAddrRec_GetCategory(IAddrRec* po)
{
   AEEAddrRec *pme = (AEEAddrRec *)po;
   return(pme->m_addrCat);

}


/*===========================================================================

FUNCTION AEEAddrRec_SetCategory

DESCRIPTION: Modify the address category of this recotrd


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrRec_SetCategory(IAddrRec *po,AEEAddrCat cat)
{
   int nErr;
   AEEAddrRec *pme = (AEEAddrRec *)po;

   //Make changes to OEM Address Book
   if(IOEMADDR_RecordUpdate(pme->m_pAddrBook->m_pIOEMAddrBk, pme->m_wrecID,cat,
                       pme->m_pFields, pme->m_nFieldCount,&nErr) != AEE_SUCCESS)
   {
      //Failed
      pme->m_nErrno = nErr;
      return(EFAILED);
   }
   //Succeeded in changing address book. Change our internal storage in the record as well.
   pme->m_addrCat = cat;
   return(AEE_SUCCESS);
}

/*===========================================================================

FUNCTION AEEAddrRec_GetRecID

DESCRIPTION: Retrieve ID of this record


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   uint16   AEEAddrRec_GetRecID(IAddrRec * po)
{
   AEEAddrRec *pme = (AEEAddrRec *)po;
   return pme->m_wrecID;
}

/*===========================================================================

FUNCTION:  AEEAddrRec_GetFieldCount

DESCRIPTION: Get Number of fields in this record


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrRec_GetFieldCount(IAddrRec *po)
{
   AEEAddrRec *pme = (AEEAddrRec*)po;

   return pme->m_nFieldCount;
}

/*===========================================================================

FUNCTION:  AEEAddrRec_GetField

DESCRIPTION: Get Field at given index position. Index is zero based.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   AEEAddrField *   AEEAddrRec_GetField(IAddrRec * po, int nFieldIdx)
{
   AEEAddrRec *pme = (AEEAddrRec *)po;


   //Validate Index.
   if(nFieldIdx < 0 || nFieldIdx >= pme->m_nFieldCount)
      return(NULL);

   return (&(pme->m_pFields[nFieldIdx]));
}


/*===========================================================================

FUNCTION:  AEEAddrRec_UpdateField

DESCRIPTION: Modify the field at the given index position


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrRec_UpdateField(IAddrRec *po, int nFieldIdx, AEEAddrField * pf)
{
   AEEAddrRec *   pme = (AEEAddrRec *)po;
   AEEAddrField * pDest,tmpField;
   int            nErr;

   //Validate Index
   if(nFieldIdx < 0 || nFieldIdx >= pme->m_nFieldCount)
      return(EBADPARM);

   //Get to index to be modified
   pDest = &(pme->m_pFields[nFieldIdx]);

   //Make a copy of this field just in case we need to revert back. The idea is to always
   //keep the fields inside our IAddrRec always in sync with the address book. So,
   //first try modifying the record in the address book. 
   AEEAddrRec_CopyField(&tmpField,pDest);

   pDest->fID = pf->fID;
   pDest->fType = pf->fType;
   pDest->wDataLen = pf->wDataLen;
   pDest->pBuffer = MALLOC(pf->wDataLen);

   if(!pDest->pBuffer)
   {
      //Failed. Revert back to original value of the field in memory.
      AEEAddrRec_CopyField(pDest,&tmpField);
      return(ENOMEMORY);
   }

   MEMCPY(pDest->pBuffer,pf->pBuffer,pf->wDataLen);

   //Commit changes to Address Book
   if(IOEMADDR_RecordUpdate(pme->m_pAddrBook->m_pIOEMAddrBk, pme->m_wrecID,
                            pme->m_addrCat, pme->m_pFields, pme->m_nFieldCount,
                            &nErr) == AEE_SUCCESS)
   {

      //Success. Free previous buffer for this field
      if(tmpField.pBuffer)
         FREE(tmpField.pBuffer);

      return (AEE_SUCCESS);
   }

   //OEM failed to update address book. So, revert back to original value of the field
   //in memory.
   pme->m_nErrno = nErr;


   //Failure in adding record. Revert to previous value of this field
   FREE(pDest->pBuffer);
   AEEAddrRec_CopyField(pDest,&tmpField);

   return EFAILED;
}


/*===========================================================================

FUNCTION:  AEEAddrRec_UpdateAllFields

DESCRIPTION: Updates all fields. Replace current fields with the new fields
passed to this function


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrRec_UpdateAllFields(IAddrRec *po, AEEAddrField *pFields, int nFieldCount)
{
   AEEAddrRec      *pme = (AEEAddrRec *)po;
   AEEAddrField    *pOldFields;
   int             nOldCount;
   int             nErr;


   //Make copy of existing fields. This is so that we can restore if OEM fails to update
   //the record.
   pOldFields = pme->m_pFields;
   nOldCount = pme->m_nFieldCount;

   //Update our record
   if(AEEAddrRec_SetInfo(po, pme->m_wrecID, pme->m_addrCat,pFields, nFieldCount,FALSE,FALSE) != AEE_SUCCESS)
   {
      pme->m_pFields = pOldFields;  // A failed malloc in _SetInfo will destroy m_pFields
      return EFAILED;
   }

   //Update OEM's address book
   if(IOEMADDR_RecordUpdate(pme->m_pAddrBook->m_pIOEMAddrBk, pme->m_wrecID,
                            pme->m_addrCat, pme->m_pFields, pme->m_nFieldCount,
                            &nErr) != AEE_SUCCESS)
   {
      //Restore Old Fields
   
      FreeAddrField(pme->m_pFields,pme->m_nFieldCount);
      pme->m_nFieldCount = nOldCount;
      pme->m_pFields = pOldFields;
      pme->m_nErrno = nErr;

      return(EFAILED);
      
   }

   //Release memory for previous fields
   FreeAddrField(pOldFields,nOldCount);
   return(AEE_SUCCESS);

}

/*===========================================================================

FUNCTION:  AEEAddrRec_AddField

DESCRIPTION: Add a field to this record


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrRec_AddField(IAddrRec* po, AEEAddrField * pField)
{
   AEEAddrField * pF;
   AEEAddrRec *   pme = (AEEAddrRec *)po;
   void *         pBuffer;
   int            nLoc = pme->m_nFieldCount;
   int            nErr;

   if(!pField)
      return(EBADPARM);

   pBuffer = MALLOC(pField->wDataLen);
   if(!pBuffer)
      return(ENOMEMORY);


   pF = (AEEAddrField*)REALLOC(pme->m_pFields,(pme->m_nFieldCount+1)*sizeof(AEEAddrField));
   if(!pF)
   {
      FREE(pBuffer);
      return(ENOMEMORY);
   }
   else
   {
      pme->m_pFields = pF;
   }

   pF[nLoc].fID = pField->fID;
   pF[nLoc].fType = pField->fType;
   pF[nLoc].wDataLen = pField->wDataLen;
   pF[nLoc].pBuffer = pBuffer;
   MEMCPY(pF[nLoc].pBuffer,pField->pBuffer,pField->wDataLen);
   
   
   if(IOEMADDR_RecordUpdate(pme->m_pAddrBook->m_pIOEMAddrBk, pme->m_wrecID,
                            pme->m_addrCat, pF, pme->m_nFieldCount+1,&nErr)
                            != AEE_SUCCESS)
   {
      //Restore Old Fields
      FREE(pBuffer);
      pme->m_nErrno = nErr;
      return(EFAILED);
      
   }
   pme->m_nFieldCount++;
   return AEE_SUCCESS;
}


/*===========================================================================

FUNCTION:  AEEAddrRec_RemoveField

DESCRIPTION: Remove the field at the given index position


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrRec_RemoveField(IAddrRec *po, int nFieldIdx)
{
   AEEAddrRec *   pme = (AEEAddrRec *)po;
   AEEAddrField   oldF;
   int            i,nErr;

   //Validate index
   if(nFieldIdx < 0 || nFieldIdx >= pme->m_nFieldCount)
      return(EFAILED);


   //Get current Field at this position
   oldF = pme->m_pFields[nFieldIdx];

   for(i = nFieldIdx; i < pme->m_nFieldCount-1; i++)
   {
      pme->m_pFields[i].fID = pme->m_pFields[i+1].fID;
      pme->m_pFields[i].fType = pme->m_pFields[i+1].fType;
      pme->m_pFields[i].wDataLen = pme->m_pFields[i+1].wDataLen;
      pme->m_pFields[i].pBuffer = pme->m_pFields[i+1].pBuffer;
   }
   
   if(IOEMADDR_RecordUpdate(pme->m_pAddrBook->m_pIOEMAddrBk, pme->m_wrecID,
                            pme->m_addrCat, pme->m_pFields,
                            pme->m_nFieldCount-1,&nErr) != AEE_SUCCESS)
   {
      //This should never happen. If it does, just restore old field and return field.
      //This is just to ensure that our rec in memory is consistent with that is there in
      //the address book
      for(i = pme->m_nFieldCount-1; i > nFieldIdx; i--)
      {
         pme->m_pFields[i].fID = pme->m_pFields[i-1].fID;
         pme->m_pFields[i].fType = pme->m_pFields[i-1].fType;
         pme->m_pFields[i].wDataLen = pme->m_pFields[i-1].wDataLen;
         pme->m_pFields[i].pBuffer = pme->m_pFields[i-1].pBuffer;
      }
      pme->m_pFields[i].fID = oldF.fID;
      pme->m_pFields[i].fType = oldF.fType;
      pme->m_pFields[i].wDataLen = oldF.wDataLen;
      pme->m_pFields[i].pBuffer = oldF.pBuffer;
      return(EFAILED);
   }

   FREE(oldF.pBuffer);
   pme->m_nFieldCount--;
   return(AEE_SUCCESS);
}

/*===========================================================================

FUNCTION:  AEEAddrRec_RemoveRec

DESCRIPTION: Removes this record


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrRec_RemoveRec(IAddrRec * po)
{
   AEEAddrRec *pme = (AEEAddrRec *)po;
   int nRet;
   IOEMAddrBook *pIOEMAddrBk;
   IAddrBook *pAddrBook;
   uint16 wRecId;

   // Save away the required info in case we release the object.
   pAddrBook = (IAddrBook *)pme->m_pAddrBook;
   wRecId = pme->m_wrecID;
   pIOEMAddrBk = (IOEMAddrBook *)pme->m_pAddrBook->m_pIOEMAddrBk;

   // Increment the reference count on the address book.  Do not allow it
   // to close as the result of the release of the record...

   AEEAddrBook_AddRef(pAddrBook);
   
   if(!AEEAddrRec_Release(po))
   {
      nRet = IOEMADDR_RecordDelete(pIOEMAddrBk, wRecId);
   }
   else
      nRet = AEE_ADDRBOOK_EBADSTATE;

   AEEAddrBook_Release(pAddrBook);

   return(nRet);

}

/*===========================================================================

FUNCTION AEEAddrRec_GetLastError

DESCRIPTION

  Get Last Error

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static   int   AEEAddrRec_GetLastError(IAddrRec *po)
{
   AEEAddrRec *pme = (AEEAddrRec *)po;
   return(pme->m_nErrno);
}

/*===========================================================================

FUNCTION AEEAddrRec_SetInfo

DESCRIPTION

  Helper function to copy the fields into our buffer.

PARAMETERS:
   bAllocated: If true, indicates that the memory for the fields has already been allocated
   by us. So, no need to allocate again. Just store the pointers

   bFreeExisting: If true, indicates that we must free the memory for the current fields before
   storing the new fields.
   
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int AEEAddrRec_SetInfo(IAddrRec *po, uint16 recID,AEEAddrCat c, AEEAddrField * pFields, int nFieldCount,boolean bAllocated,boolean bFreeExisting)
{
   AEEAddrRec *pme = (AEEAddrRec *)po;
   int          i,j;

   if(bFreeExisting)
      AEEAddrRec_FreeBuffer(pme);


   if(bAllocated)
   {
      //Mem already allocated by us. So, just store the pointers
      pme->m_pFields = pFields;
   }
   else
   {
      pme->m_pFields = (AEEAddrField*)MALLOC(nFieldCount * sizeof(AEEAddrField));
      if(!pme->m_pFields)
            return (ENOMEMORY);

      //Copy individual fields
      for(i = 0; i < nFieldCount; i++)
      {
         pme->m_pFields[i].fID = pFields[i].fID;
         pme->m_pFields[i].fType = pFields[i].fType;
         pme->m_pFields[i].wDataLen = pFields[i].wDataLen;
         pme->m_pFields[i].pBuffer = MALLOC(pFields[i].wDataLen);
   
         if(!pme->m_pFields[i].pBuffer)
         {
            //Error. FREE previous buffers and return
            for(j = 0; j < i; j++)
            {
               FREE(pme->m_pFields[j].pBuffer);
            }
            FREE(pme->m_pFields);
            return(ENOMEMORY);
         }
         else
            MEMCPY(pme->m_pFields[i].pBuffer,pFields[i].pBuffer,pFields[i].wDataLen);
      }
     
   }

   

   //Initialize other fields
   pme->m_addrCat = c;
   pme->m_nFieldCount = nFieldCount;
   pme->m_wrecID = recID;

   return AEE_SUCCESS;
}
   


/*===========================================================================

FUNCTION AEEAddrRec_FreeBuffer

DESCRIPTION

  Helper function to free the fields and set field counts approprately

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void AEEAddrRec_FreeBuffer(AEEAddrRec * pme)
{

   FreeAddrField(pme->m_pFields,pme->m_nFieldCount);

   pme->m_nFieldCount = 0;
   pme->m_pFields = NULL;
}

/*===========================================================================

FUNCTION: AEEAddrRec_CopyField

DESCRIPTION

  Utility Function to copy one address field to another

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static AEEAddrField*    AEEAddrRec_CopyField(AEEAddrField* pDst,AEEAddrField *pSrc)
{
   if(!pSrc || !pDst)
      return(NULL);

   pDst->fID = pSrc->fID;
   pDst->fType = pSrc->fType;
   pDst->pBuffer = pSrc->pBuffer;
   pDst->wDataLen = pSrc->wDataLen;

   return(pDst);
}

/*===========================================================================

FUNCTION: FreeAddrField

DESCRIPTION

  Utility Function to free memory associated with the given address fields

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/

void  FreeAddrField(AEEAddrField *pItems,int nItemCount)
{
   int i;

   if(!pItems)
      return;

   for(i = 0; i < nItemCount; i++)
   {
      if(pItems[i].pBuffer) 
         FREE(pItems[i].pBuffer);
   }

   FREE(pItems);
}


/*===========================================================================

FUNCTION: AEEAddrRecFromData

DESCRIPTION

  Helper function that will create a IAddrRec interface based on the given
  address fields data.

DEPENDENCIES
  none

RETURN VALUE
  If successful, a ptr to IAddrRec interface

SIDE EFFECTS
  none
===========================================================================*/
IAddrRec* AEEAddrRecFromData(IAddrBook *po, uint16 wID, AEEAddrCat cat,AEEAddrField *pItems, int nItemCount,int *pnErr)
{
   IAddrRec *pr = NULL;
   boolean          bFailed = TRUE;
   AEEAddrBook *pme = (AEEAddrBook *)po;


   pr = AEEAddrRec_New((AEEAddrBook *)po);

   if(pr)
   {
      //Set the record info into the IAddrRec interface
      if(AEEAddrRec_SetInfo(pr, wID,cat,pItems, nItemCount,TRUE,FALSE) == AEE_SUCCESS)
         bFailed = FALSE;  //Everything Succeeded
   }

   if(bFailed)
   {
      pme->m_nErrno = ENOMEMORY;  // Error setting info into IAddrRec

      //Release info for the fields. This memory was allocated by the OEMs when they
      //retrieved the record
      if(pItems && nItemCount)
         FreeAddrField(pItems,nItemCount);

      //Release the IAddrRec ptr
      if(pr)
         AEEAddrRec_Release(pr);

      return(NULL);
   }

   return((IAddrRec*)pr);
}

/*===========================================================================

FUNCTION : AEEAddrBook_EnumCacheInit

DESCRIPTION: Initialize enumeration of OEM cache data
             based on a given search criteria
DEPENDENCIES
  none
  
RETURN VALUE
  SUCCESS     : if success to initilize
  EFAILED     : if failed to initilize
  EBADPARM    : if not pass the parameter
  ENOTALLOWED : if no cache
  EBADSTATE   : if cache not be initilized
  
SIDE EFFECTS
  none
===========================================================================*/
static int     AEEAddrBook_EnumCacheInit( IAddrBook     *po,
                                          AEEAddrCat     wCategory,
                                          AEEAddrFieldID wFieldID, 
                                          void          *pData,
                                          uint16         wDataSize)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;
#ifndef WIN32    
   return (IOEMADDR_EnumCacheInit( pme->m_pIOEMAddrBk, 
                                   wCategory, 
                                   wFieldID, 
                                   pData,
                                   wDataSize));
#else
   return 0;
#endif
}// AEEAddrBook_EnumCacheInit

/*===========================================================================

FUNCTION : AEEAddrBook_EnumNextCache

DESCRIPTION: enumeration of OEM cache data
  po     [in] :
  ppCahe [out]:
DEPENDENCIES
  none
  
RETURN VALUE
  SUCCESS : if success to initilize
  EFAILED : if failed to initilize
  EBADPARM: if not pass the parameter

SIDE EFFECTS
  none
===========================================================================*/
static int     AEEAddrBook_EnumNextCache(IAddrBook * po, void **ppCache)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;
#ifndef WIN32  
   return (IOEMADDR_EnumNextCache( pme->m_pIOEMAddrBk, ppCache));
#else
   return 0;
#endif
}// AEEAddrBook_EnumNextCache

/*===========================================================================

FUNCTION : AEEAddrBook_ExtractCache

DESCRIPTION: Extract the name info from the cache
  po     [in] :
  pCahe  [in] :
  ppName [out]: 
  pCat   [out]:
DEPENDENCIES
  none
  
RETURN VALUE
  return the record id. return AEE_ADDR_RECID_NULL if error happens
SIDE EFFECTS
  none
===========================================================================*/
static uint16  AEEAddrBook_ExtractCache( IAddrBook  *po, 
                                         void       *pCache, 
                                         AECHAR     **ppName,
                                         AEEAddrCat *pCat)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;
   
   return (IOEMADDR_ExtractCache( pme->m_pIOEMAddrBk, pCache, ppName, pCat));
}// AEEAddrBook_ExtractCache

/*===========================================================================

FUNCTION : AEEAddrBook_GetCapacity

DESCRIPTION: Get the address book capacity
  po     [in] :
DEPENDENCIES
  none
  
RETURN VALUE
  return the capacity
SIDE EFFECTS
  none
===========================================================================*/
static uint16  AEEAddrBook_GetCapacity(IAddrBook  *po)
{
   AEEAddrBook *pme = (AEEAddrBook *)po;
   
   return (IOEMADDR_GetCapacity(pme->m_pIOEMAddrBk));
}// AEEAddrBook_ExtractCache

/*==============================================================================
函数�?
    AEEAddrBook_GetCacheinfoByNumber

说明�?
    根据号码找出相关 Cache 信息�?

参数�?
    po [in]: IAddrBook 接口指针
    pwstrNum [in]: 用来查询 Cache 信息的号�?
    pCache [in/out]: 用来返回 Cache 信息的缓冲区
    pfnMactch [in]: 号码匹配规则函数指针
       
返回值：
    查到相关 Cache 信息: SUCCESS
    查询失败: SUCCESS 以外其它�?

备注�?
       
==============================================================================*/
static int AEEAddrBook_GetCacheinfoByNumber(IAddrBook  *po,
                                            AECHAR *pwstrNum, 
                                            AEEAddCacheInfo *pCacheInfo,
                                            PFN_NUMBERMATCH pfnMactch)
{
    AEEAddrBook *pme = (AEEAddrBook *)po;
    
    return (IOEMADDR_GetCacheinfoByNumber(pme->m_pIOEMAddrBk, pwstrNum, pCacheInfo, pfnMactch));
}// AEEAddrBook_GetCacheinfoByNumber

/*==============================================================================
函数�?
    AEEAddrBook_CheckSameRecord

说明�?
    重名检�?

参数�?
    name [in]: 用来查询 Cache 信息的名�?
    exist [out]: 用来返回 查询结果
       
返回值：
    检查成�? SUCCESS
    查询失败: SUCCESS 以外其它�?

备注�?
       
==============================================================================*/

static int AEEAddrBook_CheckSameRecord(IAddrBook  *po,
                                            AECHAR *name, 
                                            boolean *exist)
{
    AEEAddrBook *pme = (AEEAddrBook *)po;
    
    return (IOEMADDR_CheckSameRecord(pme->m_pIOEMAddrBk, name, exist));
}// AEEAddrBook_CheckSameRecord 
/*===========================================================================*/

/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEEDB.c

SERVICES:  AEE Database Interfaces

GENERAL DESCRIPTION:  Provides support for Database services
in AEE.  These include:

- IDBMgr
- IDatabase
- IDBRecord

PUBLIC CLASSES AND STATIC FUNCTIONS: IGraphics

Copyright © 1999-2002 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gBrew_priv.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEE.h"
#include "AEE_MIF.h"
#include "AEEShell_priv.h"
#include "AEEFile.h"
#include "AEEStdLib.h"
#include "AEEDB.h"
#include "OEMDB.h"
#include "AEEWrapper.h"
#include "AEESpeedTune.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef struct _AEEDBMgr         AEEDBMgr;
typedef struct _AEEDatabase      AEEDatabase;
typedef struct _AEEDBRecord      AEEDBRecord;

struct _AEEDBMgr             
{
   const VTBL(IDBMgr)  *m_pvt;

   uint32         m_nRefs;
   int32          m_nCacheSize;
};

/*===========================================================================

Class Definitions

===========================================================================*/

   // 
   // NOTE
   //
   // We use the Brew's mechanism to register this object if it's associated
   // with a module/app.  This is done only for those objects that allocate underlying
   // system services (files, sockets, etc.).  It is provided so that the resources
   // are deallocated when the underlying module for an applet is closed.
   //

struct _AEEDatabase
{
   const VTBL(IDatabase)  *m_pvt;

   uint32            m_nRefs;
   uint16            m_wCurrentRecIdx;
   OEMCONTEXT        m_hdb;
   AEEDBMgr *        m_pMgr;
   AEEDBRecord *     m_pRecList;
   AEECallback       m_cbSysObj;
};

struct _AEEDBRecord
{
   const VTBL(IDBRecord)    *m_pvt;

   AEEDBRecord *  m_pNext;
   boolean        m_bOEMFree;
   uint32         m_nRefs;
   uint16         m_wIdx;
   AEEDatabase  * m_pDB;
   byte *         m_pField;
   byte *         m_pBuffer;
   AECHAR *       m_pStr;
};

//
// Module method declarations...
//

static uint32            AEEDBMgr_AddRef(IDBMgr * po);
static uint32            AEEDBMgr_Release(IDBMgr * po);
static IDatabase  *     AEEDBMgr_OpenDatabase(IDBMgr * po, const char * pszFile, boolean bCreate, uint32 dwSize, uint16 wRecs);
static int              AEEDBMgr_Remove(IDBMgr * po, const char * pszFile);
static void             AEEDBMgr_SetCacheSize(IDBMgr * po, int nSize);

static uint32            AEEDatabase_AddRef(IDatabase * po);
static uint32            AEEDatabase_Release(IDatabase * po);
static uint16           AEEDatabase_GetRecordCount(IDatabase * po);  
static void             AEEDatabase_Reset(IDatabase * po);
static IDBRecord *      AEEDatabase_GetNextRecord(IDatabase * po);
static IDBRecord *      AEEDatabase_GetRecordByID(IDatabase * po, uint16 wID);
static IDBRecord *      AEEDatabase_CreateRecord(IDatabase * po, AEEDBField * pFields, int nFieldCount);

static uint32            AEEDBRecord_AddRef(IDBRecord * po);
static uint32            AEEDBRecord_Release(IDBRecord * po);
static void             AEEDBRecord_Reset(IDBRecord * po);
static int              AEEDBRecord_Update(IDBRecord * po, AEEDBField * pFields, int nFieldCount);
static int              AEEDBRecord_Remove(IDBRecord * po);
static uint16           AEEDBRecord_GetID(IDBRecord * pRec);
static AEEDBFieldType   AEEDBRecord_NextField(IDBRecord * po, AEEDBFieldName * pName, uint16 * pnLen);
static byte *           AEEDBRecord_GetField(IDBRecord * po, AEEDBFieldName * pName, AEEDBFieldType * pType, uint16 * pnLen);
static boolean          AEEDBRecord_GetFieldWord(IDBRecord * po, uint16 * pw);
static boolean          AEEDBRecord_GetFieldDWord(IDBRecord * po, uint32 * pdw);
static AECHAR *         AEEDBRecord_GetFieldString(IDBRecord * po);

static void             AEEDBRecord_Delete(AEEDBRecord * pme);
static void             AEEDBRecord_FreeBuffer(AEEDBRecord * pme);

static uint16           GetDBFieldLen(AEEDBField * pf, uint16 * pwHeaderSize);
static uint16           GetDBFileFieldLen(AEEDBFileField * pf,uint16 * pwHeaderSize);

static IDatabase *      AEEDatabase_New(AEEDBMgr * pMgr, OEMCONTEXT hdb);
static IDBRecord *      AEEDBRecord_New(AEEDatabase * pDB, uint16 wID, byte * pRec);

//
// Constant Data...
//

//
// File/Database Module 
//

static const VTBL(IDBMgr)   gAEEDBMgrFuncs   =   {  AEEDBMgr_AddRef,
                                                    AEEDBMgr_Release,
                                                    AEEDBMgr_OpenDatabase,
                                                    AEEDBMgr_Remove,
                                                    AEEDBMgr_SetCacheSize};

static const VTBL(IDatabase)  gAEEDatabaseFuncs   = {AEEDatabase_AddRef,
                                                     AEEDatabase_Release,
                                                     AEEDatabase_GetRecordCount,
                                                     AEEDatabase_Reset,
                                                     AEEDatabase_GetNextRecord,
                                                     AEEDatabase_GetRecordByID,
                                                     AEEDatabase_CreateRecord};

static const VTBL(IDBRecord)  gAEEDBRecordFuncs = {AEEDBRecord_AddRef,
                                                   AEEDBRecord_Release,
                                                   AEEDBRecord_Reset,
                                                   AEEDBRecord_Update,
                                                   AEEDBRecord_Remove,
                                                   AEEDBRecord_GetID,
                                                   AEEDBRecord_NextField,
                                                   AEEDBRecord_GetField,
                                                   AEEDBRecord_GetFieldWord,
                                                   AEEDBRecord_GetFieldDWord,
                                                   AEEDBRecord_GetFieldString};

//
// Static Data
//


/*===========================================================================

EXTERNAL - Called to create new instance of IDBMgr

===========================================================================*/
int AEEDBMgr_New(IShell * pShell,AEECLSID cls,void ** ppobj)
{
   AEEDBMgr *pme = MALLOCREC(AEEDBMgr);

   *ppobj = pme;

   if ((AEEDBMgr *)0 == pme) {
      return ENOMEMORY;
   }

   pme->m_pvt = &gAEEDBMgrFuncs;
   pme->m_nRefs = 1;

   return SUCCESS;
}

/*===========================================================================

                      DATABASE MANAGER INTERFACE

===========================================================================*/

/*===========================================================================

Interface Method - Increments Ref Count of IDBMgr

===========================================================================*/
static uint32 AEEDBMgr_AddRef(IDBMgr * po)
{
   AEEDBMgr * pme = (AEEDBMgr *)po;

   return ++pme->m_nRefs;
}

/*===========================================================================

Interface Method - Decrements Ref Count of IDBMgr. Does clean up when ref count
reached 0

===========================================================================*/
static uint32 AEEDBMgr_Release(IDBMgr * po)
{
   AEEDBMgr * pme = (AEEDBMgr *)po;

   if(pme->m_nRefs){
      if(--pme->m_nRefs)
         return pme->m_nRefs;

      FREE(pme);
   }
   return 0;
}

/*===========================================================================

Interface Method - Implements IDBMGR_OpenDatabase

===========================================================================*/
static IDatabase  * AEEDBMgr_OpenDatabase(IDBMgr * po, const char * pszFile, boolean bCreate, uint32 dwMinSize, uint16 wMinRecs)
{
   OEMCONTEXT  hdb;
   AEE_DBError err;
   AEEDBMgr    *pme = (AEEDBMgr*)po;
   IDatabase   *pRes = NULL;

   // BREWStone API weight related
   PERF_API_CALL_COUNT(IDBMGR_OpenDatabase);

   // Speed emulation
   SPEEDEMU_REC_START_TIME();

   // Note: The path we send to the OEM is uncooked.  This will be done
   // in the AEEFile layer...
   
   hdb = OEM_DBOpenEx(pszFile,pme->m_nCacheSize,&err);
      
   if(hdb == NULL){

      // If the error is AEE_DB_ERR_ALREADY_OPEN, just get out of here

      if (err == AEE_DB_ERR_ALREADY_OPEN) 
         return NULL;

      // If the database is unreadable, remove it!
         
      if (err == AEE_DB_ERR_BAD_STATE)
         OEM_DBDelete(pszFile, &err);
         
      if(bCreate && (err == AEE_DB_ERR_NOT_EXIST))
         hdb = OEM_DBCreate(pszFile, wMinRecs, (word) dwMinSize, &err);
   }
      
   // Note: The path we save is the full path returned from BuildPath

   if(hdb != NULL)
      pRes = AEEDatabase_New((AEEDBMgr *)po, hdb);

   // Speed emulation
   SPEEDEMU_INSERT_DELAY(SpeedTune_DBOpenDelay);
   return pRes;
}

/*===========================================================================

Interface Method - Implements IDBMGR_Remove

===========================================================================*/
static int AEEDBMgr_Remove(IDBMgr * po, const char * pszFile)
{
   AEE_DBError    err;
   int            nErr;

   // BREWStone API weight related
   PERF_API_CALL_COUNT(IDBMGR_Remove);

   // Speed emulation
   SPEEDEMU_REC_START_TIME();

   // Note:  We use the uncooked file name here as it will be "cooked" 
   // in the OEM or IFILEMGR layer...

   OEM_DBDelete(pszFile, &err);

   // Translate all db errors to AEE Error "BAD FILENAME"

   if( err != AEE_DB_ERR_NO_ERR )
      nErr = EBADFILENAME;
   else
      nErr = 0;
   
   // Speed emulation
   SPEEDEMU_INSERT_DELAY(SpeedTune_DBRemoveDelay);
   return nErr;
}

/*===========================================================================

===========================================================================*/
static void AEEDBMgr_SetCacheSize(IDBMgr * po, int nSize)
{
   AEEDBMgr * pme = (AEEDBMgr *)po;

   pme->m_nCacheSize = nSize;
}
   

/*===========================================================================

                      DATABASE INTERFACE

===========================================================================*/
static void AEEDatabase_Delete(AEEDatabase *pme)
{
   AEE_DBError    err;

   // This is the last open instance of the database, so we cleanup...
   while(pme->m_pRecList) {
      while (IDBRECORD_Release((IDBRecord *)pme->m_pRecList)>0);
   }
      
   CALLBACK_Cancel(&pme->m_cbSysObj);

   IDBMGR_Release((IDBMgr *)pme->m_pMgr);
      
   if(pme->m_hdb != NULL)
      OEM_DBClose(pme->m_hdb, &err);

   FREE(pme);

}

/*===========================================================================

Helper Method - Creates new instance of IDatabase

===========================================================================*/
static IDatabase *  AEEDatabase_New(AEEDBMgr * pMgr, OEMCONTEXT hdb)
{
   AEEDatabase   * pme;
   
   if((pme = (AEEDatabase *)AEE_NewClass((IBaseVtbl *)&gAEEDatabaseFuncs, sizeof(AEEDatabase))) != NULL){
      pme->m_hdb = hdb;
      pme->m_wCurrentRecIdx = AEE_DB_RECID_NULL;
      pme->m_pMgr = pMgr;
      IDBMGR_AddRef((IDBMgr *)pMgr);

      // NOTE:  Link us into the system object chain,
      //        gotta do this to whack my OEMDB handle...

      CALLBACK_Init(&pme->m_cbSysObj, AEEDatabase_Delete, pme);
      AEE_LinkSysObject(&pme->m_cbSysObj);
   } else {
      AEE_DBError err;
      OEM_DBClose(pme->m_hdb, &err);
   }

   return (IDatabase *)pme;
}

/*===========================================================================

Interface Method - Increments Ref Count of IDatabase

===========================================================================*/
static uint32 AEEDatabase_AddRef(IDatabase * po)
{
   AEEDatabase * pme = (AEEDatabase *)po;
   
   return ++pme->m_nRefs;
}

/*===========================================================================

Interface Method - Decrements Ref Count of IDatabase. Does clean up when ref count
reaches 0

===========================================================================*/
static uint32 AEEDatabase_Release(IDatabase * po)
{
   AEEDatabase   *pme = (AEEDatabase *)po;
   
   if(pme->m_nRefs > 0){
      if(--pme->m_nRefs)
         return pme->m_nRefs;

      AEEDatabase_Delete(pme);
   }
   return 0;
}

/*===========================================================================

Interface Method - Increments IDATABSE_GetRecordCount

===========================================================================*/
static uint16 AEEDatabase_GetRecordCount(IDatabase * po)
{
   AEEDatabase   *   pme = (AEEDatabase *)po;
   AEE_DBError     err;
   
   if(!pme->m_hdb)
      return 0;
   
   return OEM_DBRecordCount(pme->m_hdb,&err);
}

/*===========================================================================

Interface Method - Implements IDatabase_Reset

===========================================================================*/
static void AEEDatabase_Reset(IDatabase * po)
{
   AEEDatabase * pd = (AEEDatabase *)po;
   
   pd->m_wCurrentRecIdx = AEE_DB_RECID_NULL;
}

/*===========================================================================

Interface Method - Implements IDATABASE_GetNextRecord

===========================================================================*/
static IDBRecord * AEEDatabase_GetNextRecord(IDatabase * po)
{
   AEEDatabase *  pd = (AEEDatabase *)po;
   IDBRecord *    pDBRec = NULL;
   AEE_DBError    err;
   AEE_DBRecInfo  recInfo;
   byte  *        pRec;

   // BREWStone API weight related
   PERF_API_CALL_COUNT(IDATABASE_GetNextRecord);

   // Speed emulation
   SPEEDEMU_REC_START_TIME();
   
   if (pd->m_hdb)
   {
      pd->m_wCurrentRecIdx = OEM_DBRecordNext(pd->m_hdb, pd->m_wCurrentRecIdx,&err);
      
      if((pRec = OEM_DBRecordGet(pd->m_hdb,pd->m_wCurrentRecIdx, &recInfo, &err)) != NULL) 
         pDBRec = AEEDBRecord_New(pd, pd->m_wCurrentRecIdx, pRec);
   }

   // Speed emulation
   SPEEDEMU_INSERT_DELAY(SpeedTune_DBGetNextRecordDelay);
   return pDBRec;
}

/*===========================================================================

Interface Method - Implements IDATABASE_GetRecordByID

===========================================================================*/
static IDBRecord * AEEDatabase_GetRecordByID(IDatabase * po, uint16 wIdx)
{
   AEEDatabase *    pd = (AEEDatabase *)po;
   IDBRecord   *  pDBRec = NULL;
   AEE_DBError    err;
   AEE_DBRecInfo  recInfo;
   byte  *        pRec;

   // BREWStone API weight related
   PERF_API_CALL_COUNT(IDATABASE_GetRecordByID);

   // Speed emulation
   SPEEDEMU_REC_START_TIME();
   
   if (pd->m_hdb)
   {
      if(wIdx != AEE_DB_RECID_NULL){
         pRec = OEM_DBRecordGet(pd->m_hdb,wIdx, &recInfo, &err);
         if(pRec)
            pDBRec = AEEDBRecord_New(pd, wIdx, pRec);
      }
   }
   // Speed emulation
   SPEEDEMU_INSERT_DELAY(SpeedTune_DBGetRecordByIDDelay);
   return pDBRec;
}

/*===========================================================================

Interface Method - Implements IDATABASE_CreateRecord

===========================================================================*/
static IDBRecord * AEEDatabase_CreateRecord(IDatabase * po, AEEDBField * pFields, int nFieldCount)
{
   IDBRecord * pr = NULL;

   // BREWStone API weight related
   PERF_API_CALL_COUNT(IDATABASE_CreateRecord);

   // Speed emulation
   SPEEDEMU_REC_START_TIME();

   // Changed so that it returns NULL even if the nFieldCount is 0.
   if (!pFields || (nFieldCount <= 0))
      return NULL;

   if((pr = AEEDBRecord_New((AEEDatabase *)po, 0, NULL)) != NULL)
   {
      if(IDBRECORD_Update(pr, pFields, nFieldCount) != SUCCESS)
      {
         AEEDBRecord_Release(pr);
         pr = NULL;
      }
   }

   // Speed emulation
   SPEEDEMU_INSERT_DELAY(SpeedTune_DBCreateRecordDelay);
   return pr;
}


/*===========================================================================

                      RECORD INTERFACE

===========================================================================*/

/*===========================================================================

Helper Method - Creates new instance of IDBRECORD

===========================================================================*/
static IDBRecord *  AEEDBRecord_New(AEEDatabase * pDB, uint16 wID, byte * pRec)
{
   AEEDBRecord   * pme;
  
   if((pme = (AEEDBRecord *)AEE_NewClass((IBaseVtbl *)&gAEEDBRecordFuncs, sizeof(AEEDBRecord))) != NULL){

      if(pRec)
         pme->m_bOEMFree = TRUE;

      pme->m_pBuffer  = pRec;
      pme->m_wIdx     = wID;
      pme->m_pDB      = pDB;
      
      // Set it in the list.  This allows us to free automatically if the refcount drops to 0...
      pme->m_pNext    = pDB->m_pRecList;
      pDB->m_pRecList = pme;
      
      // MAKE SURE THE DATABASE STAYS OPEN!!!!!!

      // Removing this like will cause an error when they want to 
      // use the record but close the database!  This will cause unexpected
      // behavior.  Please make sure you check this stuff before doing this type
      // of thing!
      
      IDATABASE_AddRef((IDatabase *)pDB);
   }      
   return (IDBRecord *)pme;
}

/*===========================================================================

Interface Method - Increments Ref Count of IDBRecord

===========================================================================*/
static uint32 AEEDBRecord_AddRef(IDBRecord * po)
{
   AEEDBRecord * pme = (AEEDBRecord *)po;
   
   return ++pme->m_nRefs;
}

/*===========================================================================

Interface Method - Decrements Ref Count of IDBRecord. Does clean up when ref count
reaches 0

===========================================================================*/
static uint32 AEEDBRecord_Release(IDBRecord * po)
{
   AEEDBRecord   *   pme = (AEEDBRecord *)po;
   
   if(pme->m_nRefs > 0){
      if(--pme->m_nRefs)
         return pme->m_nRefs;
      
      AEEDBRecord_Delete(pme);
   }
   return 0;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_Delete

===========================================================================*/
static void AEEDBRecord_Delete(AEEDBRecord * pme)
{
   AEEDatabase *    pdb;
   AEEDBRecord *    pPrev;
   AEEDBRecord *    pNext;
   
   if((pdb = pme->m_pDB) != NULL){
      
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
      
      AEEDBRecord_FreeBuffer(pme);
      IDATABASE_Release((IDatabase *)pdb);
   }
   
   if(pme->m_pStr)
      FREE(pme->m_pStr);
  
   FREE(pme);
   
}


/*===========================================================================

Interface Method - Implements IDBRECORD_Reset

===========================================================================*/
static void AEEDBRecord_Reset(IDBRecord * po)
{
   AEEDBRecord * pme = (AEEDBRecord *)po;
   
   pme->m_pField = NULL;

   // Changed so that it makes the first field of the record the current one.
// TAB: This was breaking older apps, so take it out, at least for the time
// being.
//   pme->m_pField = pme->m_pBuffer;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_Remove

===========================================================================*/
static int AEEDBRecord_Remove(IDBRecord * po)
{
   AEEDBRecord *  pme = (AEEDBRecord *)po;
   AEE_DBError    err;
   uint16         wIdx;
   AEEDatabase *  pdb;

   // BREWStone API weight related
   PERF_API_CALL_COUNT(IDBRECORD_Remove);

   // Speed emulation
   SPEEDEMU_REC_START_TIME();

   pdb = pme->m_pDB;

   if(!pdb || !pdb->m_hdb)
      return AEE_DB_EBADSTATE;

   // Increment the reference count on the database.  Do not allow it
   // to close as the result of the release of the record...

   IDATABASE_AddRef((IDatabase *)pdb);
   
   wIdx = pme->m_wIdx;
   
   if(!IDBRECORD_Release(po)) {
      OEM_DBRecordDelete(pdb->m_hdb, wIdx, &err);
      switch(err) {
      case AEE_SUCCESS:
         break;
      default:
         err = (AEE_DBError)AEE_DB_EBADREC;
      }
   }
   else
      err = (AEE_DBError)AEE_DB_EBADSTATE;

   IDATABASE_Release((IDatabase *)pdb);
   // Speed emulation
   SPEEDEMU_INSERT_DELAY(SpeedTune_DBRemoveRecordDelay);
   return err;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_Update

===========================================================================*/
static int AEEDBRecord_Update(IDBRecord * po,AEEDBField * pFields, int nFieldCount)
{
   AEEDBRecord *        pme = (AEEDBRecord *)po;
   AEEDatabase *        pdb = pme->m_pDB;
   AEEDBFileField *     ph;
   AEEDBField *         pf;
   AEE_DBError          err;
   boolean              bAdd;
   uint16               wRecSize,wHeaderSize;
   int                  i,wLen;
   byte                 *pSrc;
   byte                 *pDst;

   // BREWStone API weight related
   PERF_API_CALL_COUNT(IDBRECORD_Update);

   // Speed emulation
   SPEEDEMU_REC_START_TIME();
   
   if(!pdb || !pdb->m_hdb)
      return AEE_DB_EBADSTATE;
   
   if(pFields == NULL)
      return AEE_DB_ENULLFIELD;

   // Changed so that it returns error even if the nFieldCount is 0.
   if (nFieldCount <= 0)
      return AEE_DB_EBADFIELDNUM;

   bAdd = (pme->m_pBuffer == NULL);
   
   AEEDBRecord_FreeBuffer(pme);

   // Calculate the size we need...
   
   for(pf = pFields, wRecSize = i = 0; i < nFieldCount; i++, pf++){
      
      wLen = GetDBFieldLen(pf,&wHeaderSize);
      
      if(pf->fType == AEEDB_FT_STRING){
         if(!IsUNICODE((AECHAR *)pf->pBuffer, wLen)){
            wRecSize++;
            wLen /= sizeof(AECHAR);
         }
         else
            wRecSize += 2;
      }
      
      wRecSize += wLen + wHeaderSize;
      
      // Make sure we allow for alignment changes...
      
      if(wRecSize & 1)
         wRecSize++;
      
   }
   
   wRecSize++; // Room for last field indicator!!!
   
   // Make sure the record is on a 32-byte boundary...
   
   if((pDst = pme->m_pBuffer = (byte *)MALLOC(wRecSize)) != NULL){
      
      // Create a field header and move the contents to the field...
      
      for(i = 0, pf = pFields; i < nFieldCount; i++, pf++){
         ph = (AEEDBFileField *)pDst;
         
         wLen = GetDBFieldLen(pf,&wHeaderSize);
         
         ph->byFieldName  = pf->fName;
         ph->byFieldType  = pf->fType;
         
         if(wHeaderSize == sizeof(AEEDBFileField))
            ph->wFieldLen = (uint16)wLen;
         
         // pSrc = the buffer provided...
         // pDst = the buffer following the header...
         
         pSrc = (byte *)pf->pBuffer;
         pDst += wHeaderSize;
        
         // If it's a string, convert it to either unicode or latin
         // according to the build.  Update the length in the field header...
         
         if(pf->fType == AEEDB_FT_STRING){
            if(!IsUNICODE((AECHAR *)pf->pBuffer,wLen)){
               *pDst = AEE_ENC_ISOLATIN1;
               pDst++;
               wLen /= sizeof(AECHAR);
               ph->wFieldLen = wLen + 1;
               WSTRTOSTR((AECHAR *)pSrc, (char *)pDst, wLen);
               
            }
            else{
               *pDst = AEE_ENC_UNICODE;
               pDst += 2;
               ph->wFieldLen += 2;
               MEMCPY(pDst, pSrc, wLen);
            }
         }
         else
            MEMCPY(pDst, pSrc, wLen);
         
         // Point to the next field.  If we are on an odd boundary, align it...
         
         pDst += wLen;
         if((uint32)pDst & 1)
            pDst++;
         
      }

      *pDst = AEEDB_FT_NONE;

      if(bAdd)
         pme->m_wIdx = OEM_DBRecordAdd(pme->m_pDB->m_hdb, pme->m_pBuffer, wRecSize, &err);
      else
         OEM_DBRecordUpdate(pme->m_pDB->m_hdb, pme->m_wIdx, pme->m_pBuffer, wRecSize, &err);
      
      if(err == AEE_SUCCESS){
         pme->m_pField = NULL;
         // Speed emulation
         SPEEDEMU_INSERT_DELAY(SpeedTune_DBUpdateRecordDelay);
         return SUCCESS;
      }
   }
   // Speed emulation
   SPEEDEMU_INSERT_DELAY(SpeedTune_DBUpdateRecordDelay);
   return ENOMEMORY;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_GetID

===========================================================================*/
static uint16 AEEDBRecord_GetID(IDBRecord * po)
{
   return ((AEEDBRecord *)po)->m_wIdx;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_NextField

===========================================================================*/
static AEEDBFieldType  AEEDBRecord_NextField(IDBRecord * po,  AEEDBFieldName * pName, uint16 * pnLen)
{
   AEEDBRecord *    pme = (AEEDBRecord *)po;
   AEEDBFileField  * pField;
   byte *         pNext;
   uint16         wLen,wHeaderSize;
   AEEDBFieldType    ft;
   
   // If it's the first time, set it to point to the start of the record...
   
   if(!pme->m_pField)
      pNext = pme->m_pField = pme->m_pBuffer;
   else{
      
      // Otherwise, increment the record...
      
      pField = (AEEDBFileField *)pme->m_pField;
      
      // Calculate the next header.  If alignment is odd, then increment it...
      
      wLen = GetDBFileFieldLen(pField,&wHeaderSize);
      
      pNext = pme->m_pField + wHeaderSize + wLen;
      if((uint32)pNext & 0x1)
         pNext++;
      
      pme->m_pField = pNext;
   }
   
   pField = (AEEDBFileField *)pNext;
   
   ft = (AEEDBFieldType)pField->byFieldType;
   
   if(ft >= AEEDB_FT_MAX)
      ft = AEEDB_FT_NONE;
   
   if(ft != AEEDB_FT_NONE){
      if(pName)
         *pName = (AEEDBFieldName)pField->byFieldName;
      if(pnLen)
         *pnLen = GetDBFileFieldLen(pField,&wHeaderSize);
   }
   else
      pme->m_pField = NULL;
   
   return ft;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_GetField.

  Returns the raw data, type, name and length of the field.  Note that the
  data returned does NOT include the header.  The length does not include
  the size of the header.

===========================================================================*/
static byte * AEEDBRecord_GetField(IDBRecord * po, AEEDBFieldName * pName, AEEDBFieldType * pType, uint16 * pnLen)
{
   AEEDBRecord *        pme = (AEEDBRecord *)po;
   AEEDBFileField  *    pField;
   byte *               pData, * pDst;
   uint16               wLen, wHeaderSize,wFillLen;

   // BREWStone API weight related
   PERF_API_CALL_COUNT(IDBRECORD_GetField);

   // Speed emulation
   SPEEDEMU_REC_START_TIME();
   
   if(!pName || !pType || !pnLen)
      return NULL;

   // If it's the first time, set it to point to the start of the record...
   
   if(((pField = (AEEDBFileField *)pme->m_pField) != NULL) && (pField->byFieldType != AEEDB_FT_NONE)){
      
      wLen = GetDBFileFieldLen(pField,&wHeaderSize);
      
      *pName   = (AEEDBFieldName)pField->byFieldName;
      *pnLen   = wLen;
      *pType   = (AEEDBFieldType)pField->byFieldType;
      pData    = (byte *)(pme->m_pField + wHeaderSize);
      
      if(pField->byFieldType == AEEDB_FT_STRING){
         
         // If the strings are stored as characters, we need to expand them!!!!
         
         if(*pData == AEE_ENC_ISOLATIN1){
            pData++;
            
            // Allocate room in the record.  This is freed on the destructor...
            
            if(pme->m_pStr){
               FREE(pme->m_pStr);
               pme->m_pStr = NULL;
            }
            
            // We are copying 2x as many bytes stored...
            
            wFillLen = wLen * sizeof(AECHAR);
            pme->m_pStr = (AECHAR *)MALLOC(wFillLen);
            
            if((pDst = (byte *)pme->m_pStr) != NULL)
               *pnLen = wFillLen;
            else{
               pDst = pData;
               wFillLen = wLen;
            }
            
            STRTOWSTR((char *)pData,(AECHAR *)pDst, wFillLen);
            // Speed emulation
            SPEEDEMU_INSERT_DELAY(SpeedTune_DBGetFieldDelay);
            return pDst;
         }
         else{

            // We must return the string without the UNICODE indicator!!!!!

            if(*pData == AEE_ENC_UNICODE)
               pData += 2;
            // Speed emulation
            SPEEDEMU_INSERT_DELAY(SpeedTune_DBGetFieldDelay);
            return pData;
         }
      }
      
      // Speed emulation
      SPEEDEMU_INSERT_DELAY(SpeedTune_DBGetFieldDelay);
      return pData;
   }

   // Speed emulation
   SPEEDEMU_INSERT_DELAY(SpeedTune_DBGetFieldDelay);
   return NULL;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_GetFieldWord.

===========================================================================*/
static boolean AEEDBRecord_GetFieldWord(IDBRecord * po, uint16 * pwRet)
{
   AEEDBFieldType fType;
   AEEDBFieldName fName;
   uint16      nLen;
   byte     *  pb;
   
   if(!pwRet)
      return FALSE;
   if((pb = IDBRECORD_GetField(po, &fName, &fType, &nLen)) != NULL && fType == AEEDB_FT_WORD){
      MEMCPY(pwRet, pb, sizeof(uint16));
      return TRUE;
   }
   return FALSE;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_GetFieldDWord.

===========================================================================*/
static boolean AEEDBRecord_GetFieldDWord(IDBRecord * po, uint32 * pdwRet)
{
   AEEDBFieldType fType;
   AEEDBFieldName fName;
   uint16      nLen;
   byte     *  pb;
   
   if(pdwRet) {
      if((pb = IDBRECORD_GetField(po, &fName, &fType, &nLen)) != NULL && fType == AEEDB_FT_DWORD){
         MEMCPY(pdwRet, pb, sizeof(uint32));
         return TRUE;
      }
   }
   return FALSE;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_GetFieldString
  Returns a AECHAR * string for a field of that type.  Converts the 
  resource string of UNICODE/LATIN to the correct type for our compiled model.

===========================================================================*/
static AECHAR * AEEDBRecord_GetFieldString(IDBRecord * po)
{
   AEEDBFieldType fType;
   AEEDBFieldName fName;
   uint16      nLen;
   byte  *     pStr;

   if((pStr = (byte *)IDBRECORD_GetField(po, &fName, &fType, &nLen)) != NULL && fType == AEEDB_FT_STRING)
      return (AECHAR *)pStr;
   return NULL;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_GetFieldLen

===========================================================================*/
static uint16 GetDBFieldLen(AEEDBField * pf, uint16 * pwHeaderSize)
{
   uint16   i;
   
   if (!pf || !pwHeaderSize)
      return 0;

   *pwHeaderSize = sizeof(AEEDBFileBasicField);
   
   switch(pf->fType){
      
   case AEEDB_FT_BYTE:
      return sizeof(byte);
      
   case AEEDB_FT_WORD:
      return sizeof(uint16);
      
   case AEEDB_FT_DWORD:
      return sizeof(uint32);
      
   case AEEDB_FT_STRING:
      i = WSTRSIZE((AECHAR *)pf->pBuffer);
      break;
      
   default:
      i = pf->wDataLen;
      break;
   }
   
   *pwHeaderSize = sizeof(AEEDBFileField);
   return i;
}

/*===========================================================================

Interface Method - Implements IDBRECORD_GetFileFieldLen

===========================================================================*/
static uint16 GetDBFileFieldLen(AEEDBFileField * pf,uint16 * pwHeaderSize)
{
   if (!pf || !pwHeaderSize)
      return 0;

   *pwHeaderSize = sizeof(AEEDBFileBasicField);

   switch(pf->byFieldType){
      
   case AEEDB_FT_BYTE:
      return sizeof(byte);
      
   case AEEDB_FT_WORD:
      return sizeof(uint16);
      
   case AEEDB_FT_DWORD:
      return sizeof(uint32);
      
   }
   
   *pwHeaderSize = sizeof(AEEDBFileField);
   return pf->wFieldLen;
}

/*===========================================================================

Helper Method - Frees the Record Buffer

===========================================================================*/
static void AEEDBRecord_FreeBuffer(AEEDBRecord * pme)
{
   AEE_DBError   err;

   if(pme->m_pBuffer){
      if(pme->m_bOEMFree)
         OEM_DBFree(pme->m_pDB->m_hdb, pme->m_pBuffer, &err);
      else
         FREE(pme->m_pBuffer);
      pme->m_pBuffer = NULL;
   }
   pme->m_bOEMFree = FALSE;
}

void AEEDatabase_SetpRecList(IDatabase * po, IDBRecord * pRec)
{
   AEEDatabase       * pMe = (AEEDatabase *)po;

   pMe->m_pRecList = (AEEDBRecord *)pRec; 
} // AEEDatabase_SetpRecList


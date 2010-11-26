/*===========================================================================
                              o e m d b . c

  O E M   D A T A B A S E   R E F E R E N C E   I M P L E M E N T A T I O N
  
  This file contains a reference implementation of a simple database manager. 
    
   This simple database implementation provides a means to store, retrieve,
   query, and update variable length records from a database file. For each
   database that is created, an index file is also created, which makes 
   record operations efficient. If the index file is not found, it is 
   recreated from the database file. Every time a database is opened,
   the index file is validated to ensure the record offsets in the index file
   are correct.
   If file space is not available, the database is compressed by physically
   removing deleted records from the database file. Database compression
   can happen when a database is opened.
   If a database is made Read-Only using OEM_DBMakeReadOnly, the contents
   of the index file are appended to the database file, and the index file
   deleted. This way, we don't need to maintain 2 files for databases which
   won't change. When the database is made read-only, a flag is set in the
   database header, and subsequent add/append/modify operations are disallowed.

   The OEM_DB interface can be divided into 2 categories:

  1. Database operations
         OEM_DBOpen
         OEM_DBCreate
         OEM_DBClose
         OEM_DBDelete
         OEM_DBMakeReadOnly
         OEM_DBRecordCount

  2. Record operations
         OEM_DBRecordGet
         OEM_DBRecordNext
         OEM_DBRecordDelete
         OEM_DBRecordAdd
         OEM_DBRecordUpdate
         OEM_DBFree

        Copyright ? 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

============================================================================*/

#include "OEMFeatures.h"

#if defined (OEMDB)
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEEShell.h"
#include "OEMHeap.h"
#include "AEEDB.h"
#include "OEMDB.h"
#include "AEEFile.h"
#include "AEEHeap.h"


//#define DBG_DB                   1
#define OEM_IDX_EXT              ".idx"
#define DB_COMPRESS_THRESHOLD    0     // Compress database when 5 "junk" records

#ifdef _MSC_VER 
#pragma pack(push,1) 
#endif 

typedef struct _OEMDBase   OEMDBase;

typedef PACKED struct _oemdb_header
{
   word      wCurRecCount;   // Current record id
   word      wMaxRecID;      // Max. value for Record ID in this database
   dword     dwIndexOffset;  // offset of the index region (for RO databases)
} oemdb_header;


PACKED struct _OEMDBase 
{
   OEMDBase *     pNext;                    // Pointer to Next Database
   oemdb_header   h;
   int            nCacheSize;
   int            nIdxCacheSize;
   char           szDBName[MAX_FILE_NAME];  // name of the db
   IFile*         pDBFile;                  // data file handle
   IFile*         pIdxFile;                 // Index file handle
};


typedef PACKED struct _oemdb_rec_header
{
   word    wRecID;          // Record ID
   word    wRecSize;        // size of the record 
} oemdb_rec_header;

#ifdef _MSC_VER 
#pragma pack(pop) 
#endif

typedef dword  oemdb_index;


//=============== Function Prototypes ======================

static void       SetDBError(AEE_DBError *pDBError,AEE_DBError  error);
static boolean    FindOpenDatabase(const char *pszDBName, OEMCONTEXT *pDBContext);
static boolean    CreateDBContext(OEMDBase **pdb);
static IFileMgr * GetFileMgr(void);
static void       ReleaseFileMgr(IFileMgr *pFileMgr);
static word       GetNewRecordID(OEMDBase *pdb);

static IFile *    DB_OpenFile(IFileMgr * pfm,const char * pszFile,int nCacheSize, AEEOpenFileMode m);

static void       DB_Release(OEMDBase *pdb);
static void       DB_CheckCompress(OEMDBase *pdb, IFileMgr * pfm);
static void       DB_PopulateIdxFile(OEMDBase *pdb); 
static word       DB_VerifyIdxFile(OEMDBase *pdb);
static dword      DB_GetIdxRecOffset(OEMDBase *pdb, word wRecId);
static int        DB_AddUpdateRec(OEMCONTEXT pdb,word wRecID,const byte * pbBuf,word wBufSize,AEE_DBError*  pDBErr);
static byte *     DB_GetRecord(OEMCONTEXT pDBContext,word wRecId,AEE_DBRecInfo*   pRecInfo,AEE_DBError* pDBErr, uint32 * pdwOffset);
static boolean    DB_ReadRecHeader(OEMDBase * pdb, uint32 dwOffset, oemdb_rec_header * ph,AEE_DBError* pDBErr);

//=============== End Function Prototypes ==================

static OEMDBase *    gpActiveDBs = NULL;

/*===========================================================================
=============== Begin OEM_DB Interface Methods ============================
===========================================================================*/

/*============================================================================
OEM_DBOpen

   Description: 
   This function opens a database and returns handle to the opened database.
   If the database does not exist, this function returns NULL. It will not 
   create the database.

   Prototype:
   OEMCONTEXT   OEM_DBOpen(const char* szDBName, AEE_DBError* pDBErr)

   Parameter(s):
   szDBName: char*        Name of the database to be opened

   PDBErr  : AEE_DBError*  Place holder to contain error code
                           information on return. 
                           If NULL, no error code is returned.

   Return Value: 
   If successful, a OEMCONTEXT which can be used as the handle to the 
   opened database. If failed, returns NULL. If pDBErr is non-null, the 
   actual error code is placed inside pDBErr. 

   Comments:
   The database szDBName must exist. Use OEM_DBCreate to create a database
   and open it.

   See Also:
   OEM_DBCreate
              
============================================================================*/
OEMCONTEXT   OEM_DBOpen(const char* szDBName,AEE_DBError* pDBErr)
{
   return(OEM_DBOpenEx(szDBName,0,pDBErr));
}

OEMCONTEXT   OEM_DBOpenEx(const char* szDBName,int nCacheSize, AEE_DBError* pDBErr)
{
  
   OEMDBase *     pdb = NULL; //New database context
   IFileMgr *     pfm = NULL; //File Manager Pointer
   char           szIdxFileName[MAX_FILE_NAME]; // Name of the index file
   
   //If the name given is NULL, we just get the heck out of here

   if (szDBName == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return(NULL);
   }
 
   //Now let's check if the database is already open
   //and if it is, get out of here
   if (FindOpenDatabase(szDBName, (OEMCONTEXT *) &pdb)) {
      SetDBError(pDBErr, AEE_DB_ERR_ALREADY_OPEN);
      return(NULL);
   }
   
   
   // Now create a new DB context

   if (!CreateDBContext(&pdb)) {
      SetDBError(pDBErr, AEE_DB_ERR_TOO_MANY_DB);
      return(NULL);
   }
   
   //Create the file manager

   pfm = GetFileMgr();
   if (!pfm) {
      SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
      DB_Release(pdb);
      return(NULL);
   }

   pdb->nCacheSize = nCacheSize;
   if(nCacheSize)
      pdb->nIdxCacheSize = SCS_MIN;
   else
      pdb->nIdxCacheSize = 0;

   // Let's open the file now.

   if((pdb->pDBFile = DB_OpenFile(pfm,(const char *)szDBName,nCacheSize, _OFM_READWRITE)) == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      //Release the DB context 
      ReleaseFileMgr(pfm);
      DB_Release(pdb);
      return(NULL);
   }

   //Read the database header for this database. This will also set the 
   //seek ptr to the beginning of the first record.

   if ((IFILE_Read(pdb->pDBFile, (void *)&pdb->h, sizeof(oemdb_header))) != sizeof(oemdb_header)) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_STATE);
      ReleaseFileMgr(pfm);
      DB_Release(pdb);
      return(NULL);
   }
   
   // Add the database name to the context structure

   strcpy(pdb->szDBName, szDBName);

   // If this is a read-write (not read-only) database, let's create the 
   // index file name from the database name.

   if (!pdb->h.dwIndexOffset) {
      
      strcpy(szIdxFileName, szDBName);
      strcat(szIdxFileName, OEM_IDX_EXT);

      //Open the index file for this database. If index file is missing,
      //we'll just create a new one.

      pdb->pIdxFile = DB_OpenFile(pfm, (const char *)szIdxFileName,pdb->nIdxCacheSize,_OFM_READWRITE);
      
      if (pdb->pIdxFile == NULL) {
         
         pdb->pIdxFile = DB_OpenFile(pfm, (const char *)szIdxFileName,pdb->nIdxCacheSize, _OFM_CREATE);
         
         if (pdb->pIdxFile == NULL) {
            SetDBError(pDBErr, AEE_DB_ERR_NO_FS_SPACE);
            ReleaseFileMgr(pfm);
            DB_Release(pdb);
            return(NULL);
         }
         DB_PopulateIdxFile(pdb);
      }
      else 
         DB_CheckCompress(pdb,pfm);
   }

   //Set error code to success, release the file manager and return

   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   ReleaseFileMgr(pfm);

   return ((OEMCONTEXT) pdb);
   
}

/*============================================================================
OEM_DBCreate

   Description: 
   This function creates a new database and returns handle to the created
   database. If wMinRecCount and wMinRecSize are specified, this function
   also reserves space for this database in memory.

   Prototype:
   OEMCONTEXT   OEM_DBCreate(
                           const char* szDBName,
                           word  wMinRecCount,
                           word  wMinRecSize, 
                           AEE_DBError* pDBErr 
                           );

   Parameter(s):
   SzDBName:       char*        Name of the database to be created

   WMinRecCount:   Word          Minimum number of records that this
                                 database must support. This is used
                                 in conjunction with wminRecSize to 
                                 determine the total space to be reserved
                                 for the database. If this parameter is 0,
                                 it is ignored.

   WMinRecSize:     Word         Minimum size of the record. This is used
                                 in conjunction with the wminRecCount to 
                                 determine the total space to be reserved 
                                 for the database. If this parameter is 0,
                                 it is ignored.

   PDBErr:         AEE_DBError * Place holder to contain error code 
                                 information on return. 
                                 If NULL, no error code is returned.


   Return Value: 
   If successful, a OEMCONTEXT which can be used as handle to the newly
   created database.
   If failed, returns NULL. If pDBErr is non-null, it contains the actual
   error code on return.

   Comments:
   None

   See Also:
   OEM_DBOpen

============================================================================*/
OEMCONTEXT   OEM_DBCreate(
                       const char* szDBName, // name of the database
                       word  wMinRecCount,    // minimum number of records
                       word  wMinRecSize,     // minimum size of each record
                       AEE_DBError* pDBErr    // error status
                       )
{
   OEMDBase *     pdb = NULL; //New database context
   oemdb_header   DBHeader; //Struct into which the DB header (from file)
   IFileMgr *     pfm = NULL; //File Manager Pointer
   char           szIdxFileName[MAX_FILE_NAME]; // Name of the index file
   
   //If the name given is NULL, we just get the heck out of here

   if (szDBName == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return(NULL);
   }

   //If the filename is an empty string, get out

   if (!strlen(szDBName)) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return(NULL);
   }
   
   
   //Now let's check if the database file already exists, and if it does,
   //return an error.
   if (FindOpenDatabase(szDBName, (OEMCONTEXT *) &pdb)) {
      SetDBError(pDBErr, AEE_DB_ERR_ALREADY_OPEN);
      return(NULL);
   }
   
   // Now get a free DB context
   if (!CreateDBContext(&pdb)) {
      SetDBError(pDBErr, AEE_DB_ERR_TOO_MANY_DB);
      return(NULL);
   }
   
   //Create the file manager
   pfm = GetFileMgr();
   if (!pfm) {
      SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
      DB_Release(pdb);
      return(NULL);
   }

   // Let's open the file now.
   if((pdb->pDBFile = DB_OpenFile(pfm, (const char *)szDBName,pdb->nCacheSize, _OFM_CREATE)) == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_ALREADY_EXIST);
      //Release the file manager
      ReleaseFileMgr(pfm);
      //Release the DB context 
      DB_Release(pdb);
      return(NULL);
   }
   
   // Populate the active database struct.
   strcpy(pdb->szDBName, szDBName);
   pdb->h.wCurRecCount  = DBHeader.wCurRecCount  = 0;
   MEMSET((void *)&DBHeader,0,sizeof(DBHeader));
   
   //Write the database header into the database file. This will also 
   //set the write ptr to the beginning of the first record.
   if ((IFILE_Write(pdb->pDBFile, &DBHeader, sizeof(DBHeader))) != sizeof(DBHeader)) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_STATE);
      //Release the file manager
      ReleaseFileMgr(pfm);
      //Release the DB context
      DB_Release(pdb);
      return(NULL);
   }
   
   
   // Let's create the index file name from the database name.
   strcpy(szIdxFileName, szDBName);
   strcat(szIdxFileName, OEM_IDX_EXT);
   
   //Create an index file for this new database.
   pdb->pIdxFile = DB_OpenFile(pfm, (const char *)szIdxFileName,pdb->nIdxCacheSize, _OFM_CREATE);
   if (pdb->pIdxFile == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NO_FS_SPACE);
      //Release the file manager
      ReleaseFileMgr(pfm);
      //Release the DB context
      DB_Release(pdb);
      return(NULL);
   }
   
   //Set error code to success, release file manager, and return
   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   ReleaseFileMgr(pfm);
   return ((OEMCONTEXT) pdb);
                         
}

/*============================================================================
OEM_DBDelete

   Description: 
   This function deletes the database specified by szDBName.

   Prototype:
   void   OEM_DBdelete(const char* szDBName, AEE_DBError* pDBErr);

   Parameter(s):
   SzDBName:       char*          Name of the database to be deleted

   PDBErr:         AEE_DBError *   Place holder to contain error code 
                                   information on return. 
                                   If NULL, no error code is returned.


   Return Value: 
   None

   Comments:
   None

   See Also:
   None

============================================================================*/
void   OEM_DBDelete(const char* szDBName, AEE_DBError* pDBErr)
{
   OEMCONTEXT  pDBContext = NULL;
   IFileMgr *  pfm = NULL;
   char        szIdxFileName[MAX_FILE_NAME];
   
   //If the name given is NULL, we just get the heck out of here
   if (szDBName == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return;
   }

   //If the filename is an empty string, get out
   if (!strlen(szDBName)) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return;
   }
   
   //First check if the database is open. If it is, close it
   //There isn't much we can do if the close wasn't successful.
   //We'll have to delete the file anyway

   if (FindOpenDatabase(szDBName, &pDBContext)) 
      OEM_DBClose(pDBContext, pDBErr);
   
   //Create the file manager
   pfm = GetFileMgr();
   if (!pfm) {
      SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
      return;
   }

   //Delete the database file.
   if (IFILEMGR_Remove(pfm, szDBName) == SUCCESS){
   
	   strcpy(szIdxFileName, szDBName);
	   strcat(szIdxFileName, OEM_IDX_EXT);
   
	   //We can delete the index file because it was already closed (if it was open before)
	   //when we closed the database
	   IFILEMGR_Remove(pfm, szIdxFileName);
	   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   }
   else
	   SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);

   //Release the file manager and return
   ReleaseFileMgr(pfm);
}

/*============================================================================
OEM_DBClose

   Description: 
   This function closes the database specified by pdb. As part of
   closing, the memory associated with this database is freed.

   Prototype:
   void   OEM_DBClose(OEMCONTEXT pdb, AEE_DBError*  pDBErr)

   Parameter(s):
   pdb:      OEMCONTEXT          Handle of the database that needs to
   be closed.

   PDBErr:         AEE_DBError *	   Place holder to contain error code 
                                    information on return. 
                                    If NULL, no error code is returned.


   Return Value: 
   None

   Comments:
   None

   See Also:
   None

============================================================================*/
void   OEM_DBClose(OEMCONTEXT pDBContext, AEE_DBError*  pDBErr)
{
   OEMDBase  *       pdb = (OEMDBase *) pDBContext;
   
   if (pdb == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return;
   }
   
   if (pdb->pDBFile == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_DB_NOT_OPEN);
      return;
   }
   
   //Write the updated database header to the file. This way,
   //when we open it the next time, it will be a happy file.
   //Move to beginning of file
   IFILE_Seek(pdb->pDBFile, _SEEK_START, 0);
   
   IFILE_Write(pdb->pDBFile, &pdb->h, sizeof(oemdb_header));
   
   //Clear the open database structure and free all the assoc. memory
   // and closing the database file and index file
   DB_Release((OEMDBase *) pdb);
   
   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
}

/*============================================================================
OEM_DBRecordCount

   Description: 
   This function returns the number of records in the database specified
   by pdb.

   Prototype:
   word   OEM_DBRecordCount(OEMCONTEXT pdb, AEE_DBError*  pDBErr)

   Parameter(s):
   pdb:      OEMCONTEXT          Handle of the database whose record
   count is required.

   PDBErr:         AEE_DBError *    Place holder to contain error code 
                                    information on return. 
                                    If NULL, no error code is returned.


   Return Value: 
   If successful, returns the number of records in the database.
   If failed, returns 0.

   Comments:
   None

   See Also:
   None

============================================================================*/
word   OEM_DBRecordCount(OEMCONTEXT pDBContext, AEE_DBError*  pDBErr)
{ 
   OEMDBase  *pdb = (OEMDBase *) pDBContext;
   
   if (pdb == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return 0;
   }

   //If we have done everything right (ha ha), we should be able
   //to return the current number of records and it should be OK.

   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   return(pdb->h.wCurRecCount);
}
                  
/*============================================================================
OEM_DBRecordGet

  Description: 
  This function retrieves a specified record from the database. 
  For the given record Id (wRecId), this function retrieves the 
  RecordInformation and the data associated with that record. 
  The RecordInformation is returned via the parameter pRecInfo 
  passed to this function. The data associated with that record 
  is returned as a byte*. The memory for the  byte* is allocated 
  from the heap. It is the caller's responsibility to free the 
  memory later. This function does not remove the record from 
  the database. It just returns a copy of the information stored
  in that record.
  
    Prototype:
    byte*  OEM_DBRecordGet(
                         OEMCONTEXT pdb,
                         word  wRecId, 
                         AEE_DBRecInfo*  pRecInfo,
                         AEE_DBError* pDBErr
                         )

   Parameter(s):
   pdb:      OEMCONTEXT         Handle of the database whose record
   is to be retrieved.

   WRecId:         Word            Specifies the ID of the record that
                                   needs to be retrieved from the database.

   PRecInfo:       AEE_DBRecInfo * Information about the retrieved record
                                   is returned to in this structure to the
                                   caller.

   PDBErr:         AEE_DBError *   Place holder to contain error code 
                                   information on return. 
                                   If NULL, no error code is returned.


   Return Value: 
   If successful, returns a byte* pointing to the data stored in the specified
   record. This is a copy of the actual data stored. This memory has been
   allocated from the heap. A subsequent call to get frees this memory and
   reallocates the requisite amount for that operation. Therefore, after each
   get call, the caller must copy the contents of the buffer returned to their
   own buffer and call OEM_DBFreeGetMem.
   If failed, returns NULL.


   Comments:
   None

   See Also:
   OEM_DBFreeGetMem

============================================================================*/
byte*  OEM_DBRecordGet(
                       OEMCONTEXT          pDBContext,
                       word             wRecId, 
                       AEE_DBRecInfo*   pRecInfo,
                       AEE_DBError*     pDBErr
                       )
{
   return(DB_GetRecord(pDBContext, wRecId, pRecInfo,pDBErr,NULL));
}

static byte * DB_GetRecord(OEMCONTEXT pDBContext,word wRecId,AEE_DBRecInfo*   pRecInfo,AEE_DBError* pDBErr, uint32 * pdwOffset)
{   
   oemdb_rec_header  RecHeaderBuf;
   OEMDBase  *       pdb = (OEMDBase *) pDBContext;
   dword             dwRecOffset;
   byte *            pRecBuf;
   
   if (pDBContext == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return(NULL);
   }
   
   if (pRecInfo == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NO_RECINFO_STRUCT);
      return(NULL);
   }    
   
   //If the requested record ID exceeds the max record ID,
   //there's something wrong
   if (wRecId > pdb->h.wMaxRecID || wRecId == AEE_DB_RECID_NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_RECID);
      return(NULL);
   }
   
   
   //Get the offset of the record from the index file. If the offset
   //is zero, it is a deleted record, so we set error and return
   dwRecOffset = DB_GetIdxRecOffset(pdb, wRecId);

   if(pdwOffset)
      *pdwOffset = dwRecOffset;

   if (dwRecOffset == 0) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_RECID);
      return(NULL);
   }
      
   //Move DB file seek pointer to the beginning of the record

   if(!DB_ReadRecHeader(pdb, dwRecOffset, &RecHeaderBuf,pDBErr))
      return(NULL);
  
   // Verify that the index file is telling the truth. 

   if (RecHeaderBuf.wRecID != AEE_DB_RECID_NULL) {
      if (wRecId != RecHeaderBuf.wRecID) {
         SetDBError(pDBErr, AEE_DB_ERR_BAD_STATE);
         return(NULL);
      }
   }
   else {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_RECID);
      return(NULL);
   }
   
    
   //Allocate memory for the record.

   if ((pRecBuf = (byte *) sys_malloc(RecHeaderBuf.wRecSize)) == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NO_MEMORY);
      return(NULL);
   }
   
   //Read the record from the file into the allocated buffer

   if(IFILE_Read(pdb->pDBFile, (void *)pRecBuf, RecHeaderBuf.wRecSize) == RecHeaderBuf.wRecSize){
      if (pRecInfo) 
         MEMCPY(pRecInfo, (void *) &RecHeaderBuf, sizeof(AEE_DBRecInfo));
      SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   }
   else{
      OEM_Free(pRecBuf);
      pRecBuf = NULL;
   }
   return(pRecBuf);
}
                  
/*============================================================================
OEM_DBRecordNext

   Description: 
   This function retrieves the ID of the record next to the given record ID.
   If the given recordID is OEM_DB_RECID_NULL, this function returns the ID
   of the first record in the given database. It returns OEM_DB_RECID_NULL if
   WCurRecId is the Max. record ID in the database.

   Prototype:
   byte*  OEM_DBRecordNext(
                           OEMCONTEXT pdb,
                           word  wCurRecid, 
                           AEE_DBError* pDBErr
                           )

   Parameter(s):
   pdb:      OEMCONTEXT         Database handle. 

   WCurRecId:      Word            Specifies the ID of the current record.
                                   The record next to this record is 
                                   retrieved from the database.

   PDBErr:         AEE_DBError *   Place holder to contain error code 
                                   information on return. 
                                   If NULL, no error code is returned.


   Return Value: 
   If successful, returns the ID of the record next to the given record ID.
   If failed, returns OEM_DB_RECID_NULL.



   Comments:
   None

   See Also:
   None

============================================================================*/
word  OEM_DBRecordNext(
                       OEMCONTEXT pDBContext,
                       word  wCurRecId, 
                       AEE_DBError* pDBErr
                       )
{
   OEMDBase    *        pdb = (OEMDBase *) pDBContext;
   oemdb_rec_header  RecHeaderBuf;
   word              wNextRecId;
   dword             dwNextRecOffset;
   
   if (pDBContext == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return(AEE_DB_RECID_NULL);
   }
   
   if (pdb->h.wMaxRecID == AEE_DB_RECID_NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NO_RECORD);
      return(AEE_DB_RECID_NULL);
   }

   if (wCurRecId == AEE_DB_RECID_NULL)
      wNextRecId = 0;
   else
      wNextRecId = wCurRecId + 1;
   
   do {

      //This is so that we don't go round in circles
      if (wNextRecId > pdb->h.wMaxRecID || pdb->h.wMaxRecID == AEE_DB_RECID_NULL) {
         SetDBError(pDBErr, AEE_DB_ERR_NO_RECORD);
         return(AEE_DB_RECID_NULL);
      }

      // Get the offset of the next record.      
      dwNextRecOffset = DB_GetIdxRecOffset(pdb, wNextRecId);
      
      // Read the next record.  If the read fails, bail out.  If it succeeds, then
      // allow the check of the record ID as it may be a "deleted" record.

      if (dwNextRecOffset != 0) {
         if(!DB_ReadRecHeader(pdb, dwNextRecOffset, &RecHeaderBuf,NULL))
            return(AEE_DB_RECID_NULL);
      }
      else 
         RecHeaderBuf.wRecID = AEE_DB_RECID_NULL;
      wNextRecId++;
   } while (RecHeaderBuf.wRecID == AEE_DB_RECID_NULL);
   
   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   return(RecHeaderBuf.wRecID);  
}

/*============================================================================
OEM_DBRecordDelete

   Description: 
   This function deletes the specified record from the database.

   Prototype:
   void  OEM_DBRecordDelete(
                           OEMCONTEXT pdb,
                           word  wRecId, 
                           AEE_DBError* pDBErr
                           );

   Parameter(s):
   pdb:      OEMCONTEXT         Database handle. 

   WRecId:         Word            Specifies the ID of the record
   to be deleted.

   PDBErr:         AEE_DBError *   Place holder to contain error code 
                                   information on return. 
                                   If NULL, no error code is returned.


   Return Value: 
   None

   Comments:
   None

   See Also:
   None

============================================================================*/
void  OEM_DBRecordDelete(
                         OEMCONTEXT pDBContext,
                         word  wRecId, 
                         AEE_DBError* pDBErr
                         )
{
   OEMDBase *           pdb = (OEMDBase *) pDBContext;
   oemdb_rec_header  RecHeaderBuf;
   dword             dwRecOffset;
   dword             dwDeletedRecOffset = 0;
   
   if (pDBContext == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return;
   }

   //if the database is read-only, return
   if (pdb->h.dwIndexOffset != 0) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_ALLOWED);
      return;
   }

   //If the requested record ID exceeds the max record ID,
   //there's something wrong
   if (wRecId > pdb->h.wMaxRecID) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_RECID);
      return;
   }
   
   //Get the offset of the record from the index file
   dwRecOffset = DB_GetIdxRecOffset(pdb, wRecId);
   if (dwRecOffset == 0) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_RECID);
      return;
   }
   
   if(!DB_ReadRecHeader(pdb, dwRecOffset, &RecHeaderBuf,pDBErr))
      return;
   
   //Verify that the index file is telling the truth. If not, return 
   if (wRecId != RecHeaderBuf.wRecID) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_RECID);
      return;
   }
   
   //Now delete the record by marking it as invalid. The compression will
   //physically remove the record whenever it is run.
   RecHeaderBuf.wRecID = AEE_DB_RECID_NULL;
   //Decrement the number of records field
   pdb->h.wCurRecCount--;
   
   //Now let's write the record header back.
   dwRecOffset = DB_GetIdxRecOffset(pdb, wRecId);
   IFILE_Seek(pdb->pDBFile, _SEEK_START, dwRecOffset);
   IFILE_Write(pdb->pDBFile, &RecHeaderBuf, sizeof(oemdb_rec_header));
   
   //Fix the index file to show zero offset for deleted record
   IFILE_Seek(pdb->pIdxFile, _SEEK_START, (sizeof(dword) * wRecId));
   IFILE_Write(pdb->pIdxFile, &dwDeletedRecOffset, sizeof(dword));

   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   
}
                                          
/*============================================================================
OEM_DBRecordAdd

   Description: 
   This function adds a new record to the specified database. It returns
   the ID of the newly added record.

   Prototype:
      int  OEM_DBRecordAdd(
                           OEMCONTEXT pdb,
                           const byte*  pbBuf,
                           word  wBufSize,
                           AEE_DBError*   pDBErr
                           )

   Parameter(s):
   pdb:      OEMCONTEXT         Database handle. 

   pbBuf:          const byte *    Pointer to the buffer containing data
   to be stored in the new record.

   wBufSize        word            Size of the buffer pointed to by pbBuf.

   PDBErr:         AEE_DBError *   Place holder to contain error code 
                                   information on return. 
                                   If NULL, no error code is returned.


   Return Value: 
   If successful, this function returns the ID of the newly added record.
   If failed, returns OEM_REC_ID_NULL.


   Comments:
   None

   See Also:
   None

============================================================================*/
int  OEM_DBRecordAdd(
                     OEMCONTEXT pdb,
                     const byte*  pbBuf,
                     word  wBufSize,
                     AEE_DBError*   pDBErr
                     
                     )
{
   return (DB_AddUpdateRec(pdb, AEE_DB_RECID_NULL, pbBuf, wBufSize, pDBErr));
}

/*============================================================================
OEM_DBRecordUpdate

   Description: 
   This function updates the contents of the given record ID.

   Prototype:
   void  OEM_DBRecordUpdate(
                           OEMCONTEXT pdb,
                           word  wRecId,
                           const byte*  pbBuf,
                           word  wBufSize,
                           AEE_DBError*   pDBErr
                           )


   Parameter(s):
   pdb:      OEMCONTEXT         Database handle. 

   wRecId:         word            ID of the record whose contents need to 
   be updated.

   pbBuf:          const byte *    Pointer to the buffer containing updated
                                   data to be stored in the given record.

   wBufSize        word *          Size of the buffer pointed to by pbBuf.

   PDBErr:         AEE_DBError *   Place holder to contain error code 
                                   information on return. 
                                   If NULL, no error code is returned.


   Return Value: 
   None.

   Comments:
   None

   See Also:
   None

============================================================================*/
void   OEM_DBRecordUpdate(
                          OEMCONTEXT pdb,
                          word  wRecId,
                          const byte*  pbBuf,
                          word  wBufSize,
                          AEE_DBError*   pDBErr
                          )
{
   DB_AddUpdateRec(pdb,wRecId,pbBuf,wBufSize,pDBErr);
}
                                          
/*============================================================================
OEM_DBFree

   Description: 
   This function frees the memory allocated for the buffer returned with 
   OEM_DBRecordGet

   Prototype:
   void  OEM_DBFree(
                  OEMCONTEXT pdb,
                  AEE_DBError*   pDBErr
                  )


   Parameter(s):
   pdb:      OEMCONTEXT      Database handle. 

   pbyRecBuf:      byte *          Pointer to previously allocated
                                   record buffer that needs to be freed.

   PDBErr:         AEE_DBError *   Place holder to contain error code 
                                   information on return. 
                                   If NULL, no error code is returned.


   Return Value: 
   None.

   Comments:
      pbyRecBuf is not used in this implementation. However, it gives
      OEMs the flexibility to either save the pointer to the allocated
      memory (as is done in this implementation, in the active database
      structure), or use the user-supplied pointer (pbyRecBuf) to free
      the associated memory.

   See Also:
   None
============================================================================*/
void  OEM_DBFree(
                 OEMCONTEXT pDBContext,
                 byte *  pbyRecBuf,
                 AEE_DBError*   pDBErr
                 ) {
   
    
   if (pbyRecBuf) {
      OEM_Free(pbyRecBuf);
      pbyRecBuf = NULL;
   }
   
   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
}

/*============================================================================
OEM_DBMakeReadOnly

   Description: 
   This function makes the specified database file read-only. The contents of the
   index file are appended to the end of the database, and the database is marked
   as read-only. No add/update/delete operations are subsequently allowed on the
   database.

   Prototype:
   void  OEM_DBMakeReadOnly(
                     const char * szDBName,
                     AEE_DBError * pDBErr
                   )


   Parameter(s):
   szDBName:      char *          Name of the database that needs to be made
                                   read-only. 

   PDBErr:         AEE_DBError *   Place holder to contain error code 
                                   information on return. 
                                   If NULL, no error code is returned.


   Return Value: 
   None.

   Side Efects:
   The specified database becomes read-only

   Comments:
   None

============================================================================*/
void OEM_DBMakeReadOnly(const char * szDBName, AEE_DBError * pDBErr)
{
   OEMCONTEXT     pDBContext = NULL;
   OEMDBase *     pdb;
   oemdb_header * pDBHeader;
   dword          dwEOFPos;
   dword          dwRecIndex;
   IFileMgr *     pfm = NULL;
   char           szIdxFileName[MAX_FILE_NAME];
   boolean        bWasDatabaseOpen = TRUE;

   //If the name given is NULL, we just get the heck out of here
   if (szDBName == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return;
   }

   //Let's see if the database is open. If it is, get the context handle
   // If not, we open it.
   if (!FindOpenDatabase(szDBName, &pDBContext)) {
      if ((pDBContext = OEM_DBOpen(szDBName, pDBErr)) == NULL)
         return;
      bWasDatabaseOpen = FALSE;
   }

   pdb = (OEMDBase *) pDBContext;
   pDBHeader = &pdb->h;

   //Check if the database is already read-only. If it is, just return
   //with no error
   if (pdb->h.dwIndexOffset != 0) {
      SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
      return;
   }

   //Verify that the EOF of the database file is on a 32-bit boundary.
   //If not, return error
   IFILE_Seek(pdb->pDBFile, _SEEK_END, 0);
   dwEOFPos = IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, 0);
   
   //Note the seek pos of end of file in the oem_active_db as the
   //index offset.
   pdb->h.dwIndexOffset = dwEOFPos;

   //Go to the beginning of the index file
   IFILE_Seek(pdb->pIdxFile, _SEEK_START, 0);
   // We will read one index at a time for now
   while ((IFILE_Read(pdb->pIdxFile, (void *)&dwRecIndex, sizeof(dword))) == sizeof(dword)) {
      
      IFILE_Write(pdb->pDBFile, &dwRecIndex, sizeof(dword));
   }

   //Delete index file
   IFILE_Release(pdb->pIdxFile);
   pdb->pIdxFile = NULL;
   
   strcpy(szIdxFileName, pdb->szDBName);
   strcat(szIdxFileName, OEM_IDX_EXT);
      
   //Get the File Manager
   pfm = GetFileMgr();
   if (!pfm) {
      SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
      return;
   }

   //Delete the index file
   IFILEMGR_Remove(pfm, szIdxFileName);
   
   //Release the File Manager
   ReleaseFileMgr(pfm);
 
   //If database was open, leave it that way. Else, close it
   if (bWasDatabaseOpen == FALSE) 
      OEM_DBClose(pDBContext, pDBErr);
   else {
      //Rewrite the database header to the file. This is the earliest we can do it.
      IFILE_Seek(pdb->pDBFile, _SEEK_END, 0);
      IFILE_Write(pdb->pDBFile, pDBHeader, sizeof(oemdb_header));
   }

   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
}

/*===========================================================================
=============== End OEM_DB Interface Methods ==============================
===========================================================================*/

/*============================================================================
SetDBError

   Description: 
   This function dereferences pDBError and sets its value to the enum specified
   by error.


   Prototype:
   void    SetDBError(AEE_DBError *pDBError, AEE_DBError  error)


   Parameter(s):
   PDBErr:         AEE_DBError *   Pointer to enum to be populated
   error:          AEE_DBError     Error value


   Return Value: 
   None.

   Side Efects:
   None

   Comments:
   None

============================================================================*/
static void SetDBError(AEE_DBError *pDBError, AEE_DBError  error) {
   
   if (pDBError)
      *pDBError = error;
}

/*============================================================================
FindOpenDatabase

   Description: 
   This function determines if a database specified by name pszDBName is open.
   If it is, the context of the open database is returned using pDBContext.


   Prototype:
   boolean FindOpenDatabase(const char *pszDBName, OEMCONTEXT *pDBContext)

   Parameter(s):
   pszDBName:     char *            Name of the database
   pDBContext:    OEMCONTEXT *      Context of the database, if open


   Return Value: 
   TRUE, if the specified database is open,
   FALSE, if it isn't.

   Side Efects:
   None

   Comments:
   None

============================================================================*/
static boolean FindOpenDatabase(const char *pszDBName, OEMCONTEXT *pDBContext)
{
   OEMDBase     *pdb;
   
   *pDBContext = NULL;

   for(pdb = gpActiveDBs; pdb; pdb = pdb->pNext) {
      if (!strcmp(pdb->szDBName, pszDBName)) {
         *pDBContext = (OEMCONTEXT) pdb;
         return(TRUE);
      }
   }
   return(FALSE);
}

/*============================================================================
CreateDBContext

   Description: 
   This function gets a free active database structure from the active 
   database array.

   Prototype:
   boolean OEM_DBGetFreeDBContext(OEMDBase **pDBContext)

   Parameter(s):
   pdb:    OEMDBase *      Pointer to active database structure


   Return Value: 
   TRUE, if a free active database structure is found,
   FALSE, if it isn't.

   Side Efects:
   None

   Comments:
   None

============================================================================*/

static boolean CreateDBContext(OEMDBase **pdb)
{
   OEMDBase     *hDBCurrHandle   = gpActiveDBs;
   OEMDBase     *hDBPrevHandle   = gpActiveDBs;
   OEMDBase     *hNewHandle = NULL;
   
   *pdb = NULL;
   
   if ((hNewHandle = (OEMDBase *) sys_malloc(sizeof(OEMDBase))) == NULL)
      return FALSE;

   MEMSET((void *) hNewHandle, 0, sizeof(OEMDBase));
   hNewHandle->h.wMaxRecID     = AEE_DB_RECID_NULL;
   hNewHandle->pNext = NULL;

   if (!gpActiveDBs)
      gpActiveDBs = *pdb = hNewHandle;
   else {
      while(hDBCurrHandle) {
         hDBPrevHandle = hDBCurrHandle;
         hDBCurrHandle = hDBCurrHandle->pNext;
      }
      hDBPrevHandle->pNext = *pdb = hNewHandle;
   }
   return(TRUE);
}

/*============================================================================
DB_Release

   Description: 
   This function releases the specified active database array element and makes
   it available for subsequent OEM_DBOpen or OEM_DBCreate calls. All the 
   assoaciated resources of the database are also released. 

   Prototype:
   void   DB_Release(OEMDBase *pdb)

   Parameter(s):
   pdb:    OEMDBase *      Pointer to active database structure


   Return Value: 
   None

   Side Efects:
   None

   Comments:
   None

============================================================================*/
static void DB_Release(OEMDBase *pdb)
{
   OEMDBase * pFind, * pPrev;

   // Find the entry, unlink it from the list and free it...

   for(pPrev = NULL,pFind = gpActiveDBs; pFind; pFind = pFind->pNext){
      if(pFind == pdb){

         if(pPrev)
            pPrev->pNext = pdb->pNext;
         else
            gpActiveDBs = pdb->pNext;

         if (pdb->pDBFile) 
            IFILE_Release(pdb->pDBFile);
   
         if (pdb->pIdxFile) 
            IFILE_Release(pdb->pIdxFile);

         OEM_Free((void *) pdb);
         return;
      }
      pPrev = pFind;
   }
}

/*============================================================================
DB_CheckCompress

   Description: 
   This function compresses the specified database by removing deleted records
   from the database file. The record IDs are also made contiguous and the
   index file is updated accordingly.

   Prototype:
   void   DB_CheckCompress(OEMDBase *pdb) 

   Parameter(s):
   pdb:    OEMDBase *      Pointer to active database structure


   Return Value: 
   None

   Side Efects:
   None

   Comments:
   None

============================================================================*/
static void DB_CheckCompress(OEMDBase *pdb, IFileMgr * pfm)
{
   IFile *           pITempFile = NULL;
   FileInfo          DBFileInfo;
   oemdb_header      DBHeader;
   oemdb_rec_header  RecHeaderBuf;
   word              wValidRecID = 0;
   word              wMaxRecID = 0;
   byte *            pbyTmpRecBuf;
   char              szTemp[MAX_FILE_NAME];
   word              wRecs;

   // Retrieve the number of records (used and deleted)

   wRecs = DB_VerifyIdxFile(pdb);

   // Only compress if necessary...

   if(wRecs >= AEE_DB_RECID_NULL || wRecs <= pdb->h.wCurRecCount || (wRecs - pdb->h.wCurRecCount) < DB_COMPRESS_THRESHOLD)
      return;

#if defined(DBG_DB)
   DBGPRINTF("Compressing %s (%d vs. %d)", pdb->szDBName,wRecs,pdb->h.wCurRecCount);
#endif
  
   //First see if there's enough room on the EFS to compress this file, since we
   //need room to create the temporary file
   IFILE_GetInfo(pdb->pDBFile, &DBFileInfo);
   if ((IFILEMGR_GetFreeSpace(pfm, NULL)) < DBFileInfo.dwSize) 
      return;

   // SAS - Create a temp file with the same length name!  This
   // will greatly speed the rename function below!!!

   strcpy(szTemp,pdb->szDBName);
   szTemp[strlen(szTemp) - 1] = '~';

   if ((pITempFile = DB_OpenFile(pfm, szTemp,SCS_DEFAULT, _OFM_CREATE)) == NULL) 
      return;

   //Seek to beginning of the first record of the database file.
   IFILE_Seek(pdb->pDBFile, _SEEK_START, sizeof(oemdb_header));
   
   //Leave space for header which we will update at the end of this function.
   IFILE_Seek(pITempFile, _SEEK_START, sizeof(oemdb_header));

   while(1){
   
      if(IFILE_Read(pdb->pDBFile, (void *)&RecHeaderBuf, sizeof(oemdb_rec_header)) != sizeof(oemdb_rec_header))
         break;

      if(RecHeaderBuf.wRecID != AEE_DB_RECID_NULL) {

   // Read and write a valid record...

         if ((pbyTmpRecBuf = (byte *) sys_malloc(RecHeaderBuf.wRecSize)) == NULL)
            break;

         if(IFILE_Read(pdb->pDBFile, (void *)pbyTmpRecBuf, RecHeaderBuf.wRecSize) != RecHeaderBuf.wRecSize){
            OEM_Free(pbyTmpRecBuf);
            break;
         }

         if(RecHeaderBuf.wRecID > wMaxRecID)
            wMaxRecID = RecHeaderBuf.wRecID;

         wValidRecID++;

         IFILE_Write(pITempFile, &RecHeaderBuf, sizeof(oemdb_rec_header));
         IFILE_Write(pITempFile, pbyTmpRecBuf, RecHeaderBuf.wRecSize);
         OEM_Free(pbyTmpRecBuf);
      }
      else 
         IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, RecHeaderBuf.wRecSize);   // Failed - next
   }

   //Build new database header
   DBHeader.wCurRecCount  = wValidRecID;
   DBHeader.wMaxRecID     = wMaxRecID;
   DBHeader.dwIndexOffset = 0;
   
   //Write new database header to the temp. file
   IFILE_Seek(pITempFile, _SEEK_START, 0);
   IFILE_Write(pITempFile, &DBHeader, sizeof(oemdb_header));  

   //Close original database file
   if (pdb->pDBFile) 
      IFILE_Release(pdb->pDBFile);

   //Delete original database file
   IFILEMGR_Remove(pfm, pdb->szDBName);

   //Truncate the index file
   IFILE_Truncate(pdb->pIdxFile, 0);

   // Rename the temp file to the real name!!!

   IFILE_Release(pITempFile);

   IFILEMGR_Rename(pfm, szTemp, pdb->szDBName);

   //Open new database file
   pdb->pDBFile = DB_OpenFile(pfm, pdb->szDBName,pdb->nCacheSize, _OFM_READWRITE);
   
   //Rebuild index file

   DB_PopulateIdxFile(pdb);
}

/*============================================================================
DB_AddUpdateRec

   Description: 
   This function adds a new record to the database if wRecID is OEM_DB_RECID_NULL,
   or updates the contents of the record specified by wRecID.

   Prototype:
   int  DB_AddUpdateRec(
                                OEMCONTEXT       pdb,
                                word          wRecID,
                                const byte*   pbBuf,
                                word          wBufSize,
                                AEE_DBError*  pDBErr
                               )


   Parameter(s):
   pdb:      OEMCONTEXT         Database handle. 

   wRecId:         word            ID of the record whose contents need to 
                                   be updated.

   pbBuf:          const byte *    Pointer to the buffer containing updated
                                   data to be stored in the given record.

   wBufSize        word *          Size of the buffer pointed to by pbBuf.

   PDBErr:         AEE_DBError *   Place holder to contain error code 
                                   information on return. 
                                   If NULL, no error code is returned.


   Return Value: 
   None.

   Comments:
   None

   See Also:
   None

============================================================================*/
static int DB_AddUpdateRec(
                             OEMCONTEXT       pDBContext,
                             word          wRecID,
                             const byte*   pbBuf,
                             word          wBufSize,
                             AEE_DBError*  pDBErr
                             )
{
   OEMDBase     *       pdb = (OEMDBase *) pDBContext;
   oemdb_rec_header  RecHeaderBuf;
   dword             dwRecPos;
   byte              *pbyOrigRecord = NULL;
   IFileMgr *        pfm = NULL;
   AEE_DBRecInfo     OrigRecHeader;
   
   if (pDBContext == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return AEE_DB_RECID_NULL;
   }

   if (pdb->h.dwIndexOffset != 0) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_ALLOWED);
      return AEE_DB_RECID_NULL;
   }

   //Check if max. records are already there in the database
   if (wRecID == AEE_DB_RECID_NULL &&
      pdb->h.wCurRecCount == AEE_DB_MAX_RECNUM) {
      SetDBError(pDBErr, AEE_DB_ERR_TOO_MANY_RECORD);
      return AEE_DB_RECID_NULL;
   }      
   
   if (pbBuf == NULL || wBufSize == 0) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_NEW_RECORD);
      return AEE_DB_RECID_NULL;
   }
   
   //Get the File Manager
   pfm = GetFileMgr();
   if (!pfm) {
      SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
      return AEE_DB_RECID_NULL;
   }

   //Now let's check if there's space to write this file, and if there isn't,
   //we have to abandon
   if ((IFILEMGR_GetFreeSpace(pfm, NULL)) < (sizeof(oemdb_rec_header) + wBufSize)) {
      SetDBError(pDBErr, AEE_DB_ERR_NO_FS_SPACE);
      //Release file manager
      ReleaseFileMgr(pfm);
      return AEE_DB_RECID_NULL;
   }
   
   if (wRecID == AEE_DB_RECID_NULL) {
      //If this is the first record in the database
      if (pdb->h.wMaxRecID == AEE_DB_RECID_NULL) 
         RecHeaderBuf.wRecID = pdb->h.wMaxRecID = 0;
      else {
         RecHeaderBuf.wRecID = GetNewRecordID(pdb);
         if(RecHeaderBuf.wRecID == AEE_DB_RECID_NULL) {
            SetDBError(pDBErr, AEE_DB_ERR_TOO_MANY_RECORD);
            return AEE_DB_RECID_NULL;
         }
      }
      pdb->h.wCurRecCount++;
   }
   else {
      //First check if the record is valid
      pbyOrigRecord = DB_GetRecord(pDBContext, wRecID, &OrigRecHeader, pDBErr, &dwRecPos); 
      if (pbyOrigRecord == NULL) {
         //Release file manager
         ReleaseFileMgr(pfm);
         //The error is already set by OEM_DBRecordGet, so just return
         return AEE_DB_RECID_NULL;
      }
      else 
         //Free the memory allocated for the original record. We don't need it.
         OEM_DBFree(pDBContext, pbyOrigRecord, pDBErr);

   // SAS - See if the size of the records matches.  If so, just update it in place!

      RecHeaderBuf.wRecID = wRecID;

      if(OrigRecHeader.wRecSize == wBufSize){
         IFILE_Seek(pdb->pDBFile, _SEEK_START, dwRecPos + sizeof(oemdb_rec_header));
         IFILE_Write(pdb->pDBFile, pbBuf, wBufSize);
         goto aouDone;
      }
#if defined(DBG_DB)
      DBGPRINTF("DB Poor %d vs %d", OrigRecHeader.wRecSize,wBufSize);
#endif
   
   }
   
   RecHeaderBuf.wRecSize = wBufSize;
   
   //First go to end of file to add record.
   IFILE_Seek(pdb->pDBFile, _SEEK_END, 0);
   //Now determine where we are by doing a tell
   dwRecPos = IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, 0);

   //May be we should do error checks here, but I don't know the enums
   //that IFILE methods will return yet.
   IFILE_Write(pdb->pDBFile, &RecHeaderBuf, sizeof(oemdb_rec_header));
   IFILE_Write(pdb->pDBFile, pbBuf, wBufSize);


   //Update the index file
   if (wRecID != AEE_DB_RECID_NULL) {
      //This is a good time to mark the old record as invalid.
      OEM_DBRecordDelete(pDBContext, wRecID, pDBErr);

      //But delete has decremented pdb->h.wCurRecCount, so we increment it
      pdb->h.wCurRecCount++;
   }

   //Now let's get to the right location in the index file
   IFILE_Seek(pdb->pIdxFile, _SEEK_START, (RecHeaderBuf.wRecID * sizeof(dword)));
   IFILE_Write(pdb->pIdxFile, (byte *) &dwRecPos, sizeof(dword));
   
aouDone:

   //Set error to no error and return happily    
   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   //Release file manager
   ReleaseFileMgr(pfm);
   
   return RecHeaderBuf.wRecID;
   
}

/*============================================================================
DB_PopulateIdxFile

   Description: 
   This function populates the index file for an open database. For an existing
   database, this is useful when a database is shipped without its index file,
   since the index file can be created on the fly.

   Prototype:
   void  DB_PopulateIdxFile(OEMDBase *pdb)

   Parameter(s):
   pdb:    OEMDBase *      Pointer to active database structure


   Return Value: 
   None
   Side Efects:
   None

   Comments:
   None

============================================================================*/
static void  DB_PopulateIdxFile(OEMDBase *pdb)
{
   oemdb_rec_header    DBRecordHeader; 
   word                wMaxRecId = 0;
   word                wRecCount  = 0;
   word                wTotalRecCount = 0;
   dword               dwRecOffset;
   dword               dwReadSize;
   oemdb_header        DBHeader;
   dword               dwBadRecOffset = 0;
   dword               dwNullRecOffset = 0;
   word                count;
   
   //To populate the database file, we first reset the seek pointer to the
   //begninning of the file
   IFILE_Seek(pdb->pDBFile, _SEEK_START, 0);   
   
   //First read the database header. We may need to fix it too,
   //if the max record id or the current record count are out of whack.
   IFILE_Read(pdb->pDBFile, (void *)&DBHeader, sizeof(oemdb_header));
   
   //Do a tell to find the position of the first record
   dwRecOffset = IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, 0);
   dwReadSize = IFILE_Read(pdb->pDBFile, (void *)&DBRecordHeader, sizeof(oemdb_rec_header));
   
   //We must first create an index file with all zero records
   for (count = 0; count < DBHeader.wMaxRecID; count++)
      IFILE_Write(pdb->pIdxFile, &dwNullRecOffset, sizeof(dword));


   while(dwReadSize == sizeof(oemdb_rec_header)) {
      
      if (DBRecordHeader.wRecID != AEE_DB_RECID_NULL) {
         IFILE_Seek(pdb->pIdxFile, _SEEK_START, (sizeof(dword) * (DBRecordHeader.wRecID)));
         IFILE_Write(pdb->pIdxFile, &dwRecOffset, sizeof(dword));
      
         if (DBRecordHeader.wRecID > wMaxRecId) {
            wMaxRecId = DBRecordHeader.wRecID;
         }
              
         wRecCount++;
      }
      else {
         IFILE_Seek(pdb->pIdxFile, _SEEK_START, (sizeof(dword) * wTotalRecCount));
         IFILE_Write(pdb->pIdxFile, &dwBadRecOffset, sizeof(dword));
      }
      

      //Now move to the end of the record.
      dwRecOffset += sizeof(oemdb_rec_header);
      IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, DBRecordHeader.wRecSize);
      dwRecOffset += DBRecordHeader.wRecSize;
      //Read the next record header
      dwReadSize = IFILE_Read(pdb->pDBFile, (void *)&DBRecordHeader, sizeof(oemdb_rec_header));
      wTotalRecCount++;
   }
   
   //Fix the max. record id and current record countfor this database.
   if (wTotalRecCount > 0)
      pdb->h.wMaxRecID = wMaxRecId;
   else
      pdb->h.wMaxRecID = AEE_DB_RECID_NULL;

   pdb->h.wCurRecCount = wRecCount;
}

/*============================================================================
DB_VerifyIdxFile

   Description: 
   This function steps through all the records of the specified database, and
   verifies that the file offsets of the records are in sync with those in the
   index file. If not, the index file is fixed to point to the right records.

   Prototype:
   void  DB_VerifyIdxFile(OEMDBase *pdb)

   Parameter(s):
   pdb:    OEMDBase *      Pointer to active database structure


   Return Value: 
   If successful, returns total number of records. 
   If read-only, returns AEE_DB_RECID_NULL
   Side Efects:
   None

   Comments:
   None

============================================================================*/
static word  DB_VerifyIdxFile(OEMDBase *pdb)
{
   oemdb_rec_header    DBRecordHeader;
   oemdb_header        DBHeader;
   word                wMaxRecId = 0;
   word                wRecCount  = 0;
   word                wTotalRecCount = 0;
   dword               dwRecOffset;
   dword               dwIdxRecOffset;
   dword               dwBadRecOffset = 0;
   dword               dwReadSize;
   
   //To verify the database file, we first reset the seek pointer to the
   //begninning of the file
   IFILE_Seek(pdb->pDBFile, _SEEK_START, 0);

   //First read the database header. We may need to fix it too,
   //if the max record id or the current record count are out of whack.
   IFILE_Read(pdb->pDBFile, (void *)&DBHeader, sizeof(oemdb_header));
   
   //If the database is read-only, return
   if(DBHeader.dwIndexOffset != 0) 
      return AEE_DB_RECID_NULL;

   //Do a tell to find the position of the first record
   dwRecOffset = IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, 0);
   dwReadSize = IFILE_Read(pdb->pDBFile, (void *)&DBRecordHeader, sizeof(oemdb_rec_header));
   
   while(dwReadSize == sizeof(oemdb_rec_header)) {

      if (DBRecordHeader.wRecID != AEE_DB_RECID_NULL) {
         
         //Read the offset in the index file
         IFILE_Seek(pdb->pIdxFile, _SEEK_START, (sizeof(dword) * DBRecordHeader.wRecID));
         IFILE_Read(pdb->pIdxFile, (void *)&dwIdxRecOffset, sizeof(dword));

         if (DBRecordHeader.wRecID > wMaxRecId) {
            wMaxRecId = DBRecordHeader.wRecID;
         }
         
         wRecCount++;
      
         //If the offset does not match, fix it in the index file.
         if (dwIdxRecOffset != dwRecOffset) {
         
            IFILE_Seek(pdb->pIdxFile, _SEEK_START, (sizeof(dword) * DBRecordHeader.wRecID));
            IFILE_Write(pdb->pIdxFile, &dwRecOffset, sizeof(dword));
         }
      }
      else {
         IFILE_Seek(pdb->pIdxFile, _SEEK_START, (sizeof(dword) * wTotalRecCount));
         IFILE_Read(pdb->pIdxFile, (void *)&dwIdxRecOffset, sizeof(dword));

         if (dwIdxRecOffset != 0) {
            IFILE_Seek(pdb->pIdxFile, _SEEK_START, (sizeof(dword) * wTotalRecCount));
            IFILE_Write(pdb->pIdxFile, &dwBadRecOffset, sizeof(dword));
         }
      }
      
      
      //Now move to the end of the record.
      dwRecOffset += sizeof(oemdb_rec_header);
      IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, DBRecordHeader.wRecSize);
      dwRecOffset += DBRecordHeader.wRecSize;
      //Read the next record header
      dwReadSize = IFILE_Read(pdb->pDBFile, (void *)&DBRecordHeader, sizeof(oemdb_rec_header));
      wTotalRecCount++;
   }
   
   //Fix the max. record id and current record countfor this database.
   if (wTotalRecCount > 0) {
      //First truncate the index file if there are holes (from deleted records on the high end)
      //This will keep the index file size in control
      IFILE_Truncate(pdb->pIdxFile, (wMaxRecId + 1)* sizeof(dword));
      pdb->h.wMaxRecID = wMaxRecId;
   }
   else 
      pdb->h.wMaxRecID = AEE_DB_RECID_NULL;
   
   pdb->h.wCurRecCount = wRecCount;
   


   //Set the file pointer of the database file and the index file to the beginning
   IFILE_Seek(pdb->pDBFile, _SEEK_START, 0);
   IFILE_Seek(pdb->pIdxFile, _SEEK_START, 0);
   
   
   return wTotalRecCount;
}

/*============================================================================
DB_GetIdxRecOffset

   Description: 
   This function reads the index file to return the offset of the specified
   record in the database file.

   Prototype:
   dword DB_GetIdxRecOffset(OEMDBase *pdb, word wRecId)

   Parameter(s):
   pdb:    OEMDBase *      Pointer to active database structure
   wRecId:       word                   Record ID of the record whose offset
                                        is requested.


   Return Value: 
   On success, returns the file offset of the record wRecId.
   On failure, returns 0.
   Side Efects:
   None

   Comments:
   None

============================================================================*/
static dword DB_GetIdxRecOffset(OEMDBase *pdb, word wRecId)
{
   dword   dwRecOffset;
   
   if (wRecId > pdb->h.wMaxRecID || wRecId == AEE_DB_RECID_NULL) 
      return 0;

   if (pdb->h.dwIndexOffset != 0) {
      //Go to the beginning of the index section first
      IFILE_Seek(pdb->pDBFile, _SEEK_START, pdb->h.dwIndexOffset);
      //Go to the beginning of the requested index
      IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, (wRecId * sizeof(dword)));
      IFILE_Read(pdb->pDBFile, (void *)&dwRecOffset, sizeof(dword));
   }
   else {
      IFILE_Seek(pdb->pIdxFile, _SEEK_START, (wRecId * sizeof(dword)));
      IFILE_Read(pdb->pIdxFile, (void *)&dwRecOffset, sizeof(dword));
   }
   return dwRecOffset;
}


/*============================================================================
GetFileMgr

   Description:
   This function creates a File Manager object.


   Prototype:
   IFileMgr * GetFileMgr(void)


   Parameter(s):
   None


   Return Value:
   None.

   Side Efects:
   None

   Comments:
   None

============================================================================*/
static IFileMgr * GetFileMgr(void)
{
   IFileMgr *  pFileMgr = NULL;

   ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR, (void **)&pFileMgr);
   return(pFileMgr);
}

/*============================================================================
ReleaseFileMgr

   Description:
   This function releases a File Manager object.


   Prototype:
   IFileMgr * ReleaseFileMgr(void)


   Parameter(s):
   None


   Return Value:
   None.

   Side Efects:
   None

   Comments:
   None

============================================================================*/
static void ReleaseFileMgr(IFileMgr *pFileMgr)
{
    if ( pFileMgr )
        IFILEMGR_Release(pFileMgr);
}

   //Move DB file seek pointer to the beginning of the record

static boolean    DB_ReadRecHeader(OEMDBase * pdb, uint32 dwOffset, oemdb_rec_header * ph,AEE_DBError* pDBErr)
{
   IFILE_Seek(pdb->pDBFile, _SEEK_START, dwOffset);
   if(IFILE_Read(pdb->pDBFile, (void *)ph, sizeof(oemdb_rec_header)) != sizeof(oemdb_rec_header)){
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return(FALSE);
   }
   return(TRUE);
}

/*============================================================================
GetNew RecordID

   Description:
   This function attempts to find a rec ID for a previously deleted record
   and returns it as the new record ID. If no holes are found in the index
   file, it increments the MAX record ID and returns it.


   Prototype:
   IFileMgr * ReleaseFileMgr(void)


   Parameter(s):
   None


   Return Value:
   None.

   Side Efects:
   None

   Comments:
   None

============================================================================*/
static word GetNewRecordID(OEMDBase *pdb) {

   word count;
   dword offset;

   IFILE_Seek(pdb->pIdxFile, _SEEK_START, 0);
   
   for (count = 0; count < pdb->h.wMaxRecID; count++) {
      IFILE_Read(pdb->pIdxFile, (void *) &offset, sizeof(dword));
      if(!offset)
         return count;
   }
   if(pdb->h.wMaxRecID == AEE_DB_MAX_RECNUM)
      return AEE_DB_RECID_NULL;

   //pdb->h.wCurRecCount++;
   return(++pdb->h.wMaxRecID);

}

static IFile * DB_OpenFile(IFileMgr * pfm,const char * pszFile,int nCacheSize,AEEOpenFileMode m)
{
   IFile * pf;

   pf = IFILEMGR_OpenFile(pfm, pszFile, m);
   if(pf && nCacheSize)
      IFILE_SetCacheSize(pf, nCacheSize);
   return(pf);
}

#endif   // AEE_OEMDB

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

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

============================================================================*/

#include "OEMFeatures.h"

#if defined (OEMDB)
#include "OEMHeap.h"
#include "OEMDB.h"
#include "OEMStdLib.h"
#include "AEE_OEM.h"
#include "AEE_OEMFile.h"
#include "AEEFile.h"
#include "AEEHeap.h"
#include "AEEDB.h"

//#define DBG_DB                   1
#define OEM_IDX_EXT                 ".idx"
#define DB_COMPRESS_PERCENT         50    // Compress database when "junk" records are 50% of good records
#define DB_COMPRESS_COUNT           5     // Don't compress unless this many records are deleted
#define DB_INCREASE_SIZE            16	   // Increment used for increasing size of index
#define DB_COMPRESSION_TIME_SLICE   250   // Number of milliseconds to do data base compression in one resume
#define NULL_REC_MAGIC_NUM          0x1a2b3c4d // for null recs > 0xffff bytes.
#define MAX_REC_SIZE                0xffff     // maximum size of a record.

#ifdef _MSC_VER 
#pragma pack(push,1) 
#endif 

typedef struct _OEMDBase   OEMDBase;

typedef PACKED struct _oemdb_header
{
   word      wCurRecCount;   // Current record id
   word      wLastAssignedRecID;      // Max. value for Record ID in this database
   dword     dwIndexOffset;  // offset into the file of the index
} oemdb_header;

// Entry in index
typedef struct _index_entry
{
   uint32     recID;     // record id
   uint32     dwOffset;  // offset of record header in data file
} index_entry;

// Header info for index
typedef PACKED struct _index_header
{
   word      recCount; // number of records in index
   word      delCount; // number of deleted records
} index_header;

// Index (used for locating records in data file)
typedef struct _db_index
{
   index_header  h;         // index header
   index_entry   e[1];       // entries
} db_index;

struct _OEMDBase 
{
   OEMDBase *     pNext;                    // Pointer to Next Database
   oemdb_header   h;
   int            nCacheSize;
   char           *pszFullPath;              // name of the db
   IFile*         pDBFile;                  // data file handle
   db_index*      pIndex;					// an array of offsets into data file
   int            refCount;            // reference count for muliple instances of same file
   AEECallback    cb;                  // Callback
   uint32         dwOffsetFrom;        // Location to start compression from
   uint32         dwOffsetTo;          // Location to start compression to
   word           indexAlloc;               // number of entries allocated for index, >= record count, extras at end
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
static AEE_DBError LoadIndex(OEMDBase *pdb);
static void       RemoveIndexFromFile(OEMDBase *pdb);
static int        CmpRecID(const void *p1, const void *p2);

static IFile *    DB_OpenFile(IFileMgr * pfm,const char * pszFile,int nCacheSize, AEEOpenFileMode m);

static void          DB_Release(OEMDBase *pdb);
static void          DB_CheckCompress(OEMDBase *pdb);
static void          DB_Compress(OEMDBase *pdb);
static AEE_DBError   DB_PopulateIdx(OEMDBase *pdb); 
static dword         DB_GetIdxRecOffset(OEMDBase *pdb, word wRecId);
static AEE_DBError   DB_SetIdxRecOffset(OEMDBase *pdb, word wRecId, dword offset);
static boolean       DB_FindIdxRec(OEMDBase *pdb, word wRecId, word *pResult);
static int           DB_AddUpdateRec(OEMCONTEXT pdb,word wRecID,const byte * pbBuf,word wBufSize,AEE_DBError*  pDBErr);
static byte *        DB_GetRecord(OEMCONTEXT pDBContext,word wRecId,AEE_DBRecInfo*   pRecInfo,AEE_DBError* pDBErr, uint32 * pdwOffset);
static boolean       DB_ReadRecHeader(OEMDBase * pdb, uint32 dwOffset, oemdb_rec_header * ph,AEE_DBError* pDBErr);
static void          DB_MarkRecordDelete(OEMDBase *pdb, word  wRecId, AEE_DBError* pDBErr);

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
   OEMCONTEXT OEM_DBOpen(const char* szDBName, AEE_DBError* pDBErr)

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
OEMCONTEXT OEM_DBOpen(const char* szDBName,AEE_DBError* pDBErr)
{
   return(OEM_DBOpenEx(szDBName,SCS_DEFAULT,pDBErr));	// Open with default file caching
}

OEMCONTEXT OEM_DBOpenEx(const char* pszDB,int nCacheSize, AEE_DBError* pDBErr)
{
  
   OEMDBase *     pdb = NULL; //New database context
   IFileMgr *     pfm = NULL; //File Manager Pointer
   AEE_DBError    error = AEE_DB_ERR_NO_ERR;
   char *         pszFullPath;
   
   //If the name given is NULL, we just get the heck out of here

   if (pszDB == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return(NULL);
   }

   {
      int nFullPathLen;

      if (SUCCESS != AEE_ResolvePath(pszDB, 0, &nFullPathLen)) {
         SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
         return(NULL);         
      }

      pszFullPath = sys_malloc(nFullPathLen);

      if ((char *)0 == pszFullPath) {
         SetDBError(pDBErr, AEE_DB_ERR_NO_MEMORY);
         return(NULL);
      }
      
      if (SUCCESS != AEE_ResolvePath(pszDB, pszFullPath, &nFullPathLen)) {
         sys_free(pszFullPath);
         SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
         return(NULL);         
      }
   }

   //Now let's check if the database is already open
   //and if it is, increment reference count and we're done!
   if (FindOpenDatabase(pszFullPath, (OEMCONTEXT *) &pdb)) {
      pdb->refCount++;
      sys_free(pszFullPath);
      SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
      return ((OEMCONTEXT) pdb);
   }
   
   
   // Now create a new DB context

   if (!CreateDBContext(&pdb)) {
      sys_free(pszFullPath);
      SetDBError(pDBErr, AEE_DB_ERR_TOO_MANY_DB);
      return(NULL);
   }

   pdb->pszFullPath = pszFullPath;
   
   //Create the file manager

   pfm = GetFileMgr();
   if (!pfm) {
      SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
      DB_Release(pdb);
      return(NULL);
   }

   pdb->nCacheSize = nCacheSize;

   // Let's open the file now.

   if((pdb->pDBFile = DB_OpenFile(pfm,(const char *)pszFullPath,nCacheSize, _OFM_READWRITE)) == NULL) {
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
   
   // Load index into RAM

   error = LoadIndex(pdb);

   if (error)
   {
		SetDBError(pDBErr, error);
        ReleaseFileMgr(pfm);
        DB_Release(pdb);
        return(NULL);   
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
                       const char* pszDB, // name of the database
                       word  wMinRecCount,    // minimum number of records
                       word  wMinRecSize,     // minimum size of each record
                       AEE_DBError* pDBErr    // error status
                       )
{
   OEMDBase *     pdb = NULL; //New database context
   oemdb_header   DBHeader; //Struct into which the DB header (from file)
   IFileMgr *     pfm = NULL; //File Manager Pointer
   char *         pszFullPath;
   
   //If the name given is NULL, we just get the heck out of here

   if (pszDB == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return(NULL);
   }

   //If the filename is an empty string, get out

   if (!STRLEN(pszDB)) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return(NULL);
   }
   
   {
      int nFullPathLen;

      if (SUCCESS != AEE_ResolvePath(pszDB, 0, &nFullPathLen)) {
         SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
         return(NULL);         
      }

      pszFullPath = sys_malloc(nFullPathLen);

      if ((char *)0 == pszFullPath) {
         SetDBError(pDBErr, AEE_DB_ERR_NO_MEMORY);
         return(NULL);
      }
      
      if (SUCCESS != AEE_ResolvePath(pszDB, pszFullPath, &nFullPathLen)) {
         sys_free(pszFullPath);
         SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
         return(NULL);         
      }
   }
         
   //Now let's check if the database file already exists, and if it does,
   //return an error.

   if (FindOpenDatabase(pszFullPath, (OEMCONTEXT *) &pdb)) {
      sys_free(pszFullPath);
      SetDBError(pDBErr, AEE_DB_ERR_ALREADY_OPEN);
      return(NULL);
   }
   
   // Now get a free DB context
   if (!CreateDBContext(&pdb)) {
      sys_free(pszFullPath);
      SetDBError(pDBErr, AEE_DB_ERR_TOO_MANY_DB);
      return(NULL);
   }
   
   pdb->pszFullPath = pszFullPath;

   //Create the file manager
   pfm = GetFileMgr();
   if (!pfm) {
      SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
      DB_Release(pdb);
      return(NULL);
   }

   // Let's open the file now.
   if((pdb->pDBFile = DB_OpenFile(pfm, (const char *)pszFullPath,pdb->nCacheSize, _OFM_CREATE)) == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_ALREADY_EXIST);
      //Release the file manager
      ReleaseFileMgr(pfm);
      //Release the DB context 
      DB_Release(pdb);
      return(NULL);
   }
   
   // Populate the active database struct.
   //
   // Note: Store the full path for the code that checks for open
   // databases...

   pdb->h.wCurRecCount = 0;
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
   
   // Create index
   pdb->pIndex = (db_index*)sys_malloc(sizeof(index_header)+DB_INCREASE_SIZE*sizeof(index_entry));
   if (!pdb->pIndex)
   {
      SetDBError(pDBErr, AEE_DB_ERR_NO_MEMORY);
      //Release the file manager
      ReleaseFileMgr(pfm);
      //Release the DB context
      DB_Release(pdb);
      return(NULL);   
   }
   pdb->pIndex->h.recCount = 0;
   pdb->pIndex->h.delCount = 0;
   pdb->indexAlloc = DB_INCREASE_SIZE;   
   
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
void   OEM_DBDelete(const char* cpszPath, AEE_DBError* pDBErr)
{
   OEMCONTEXT  pDBContext = NULL;
   IFileMgr *  pfm = NULL;
   char *      pszFullPath;
   
   //If the name given is NULL, we just get the heck out of here
   if (cpszPath == NULL || !*cpszPath) {
      SetDBError(pDBErr, AEE_DB_ERR_NOT_EXIST);
      return;
   }

   {
      int nFullPathLen;

      if (SUCCESS != AEE_ResolvePath(cpszPath, 0, &nFullPathLen)) {
         SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
         return;
      }

      pszFullPath = sys_malloc(nFullPathLen);

      if ((char *)0 == pszFullPath) {
         SetDBError(pDBErr, AEE_DB_ERR_NO_MEMORY);
         return;
      }
      
      if (SUCCESS != AEE_ResolvePath(cpszPath, pszFullPath, &nFullPathLen)) {
         sys_free(pszFullPath);
         SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
         return;
      }
   }

   //First check if the database is open. If it is, we can't close it

   if (FindOpenDatabase(pszFullPath, &pDBContext)) 
   {
      sys_free(pszFullPath);
      SetDBError(pDBErr, AEE_DB_ERR_ALREADY_OPEN);
      return;
   }
   
   sys_free(pszFullPath);

   //Create the file manager
   pfm = GetFileMgr();
   if (!pfm) {
      SetDBError(pDBErr, AEE_DB_ERR_OTHER_FS_ERR);
      return;
   }

   //Delete the database file.
   if (IFILEMGR_Remove(pfm, cpszPath) == SUCCESS)
	   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
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

   if (--pdb->refCount > 0)
   {
      // There are still references to this. Can't close yet.
      SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
      return;
   }

   if (!pdb->h.dwIndexOffset)
   {
      // Need to write out index array
      if (IFILE_Seek(pdb->pDBFile, _SEEK_END, 0) == SUCCESS)	// Go to end of file
      {
         pdb->h.dwIndexOffset = IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, 0);	// Get offset to end of file
         if (IFILE_Write(pdb->pDBFile,pdb->pIndex,sizeof(index_header)+pdb->pIndex->h.recCount*sizeof(index_entry)) == 0)
            // Write error. Indicate there is no index.
            pdb->h.dwIndexOffset = 0;

         // Write the updated database header to the file. It doesn't
         // need to be written unless index changes
         IFILE_Seek(pdb->pDBFile, _SEEK_START, 0);
         
         IFILE_Write(pdb->pDBFile, &pdb->h, sizeof(oemdb_header));
      }
   }
   
   
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
   allocated from the heap.
   If failed, returns NULL.


   Comments:
   None
   
   See Also:
   OEM_DBFree

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
   dword             dwRecOffset = 0;
   byte *            pRecBuf;
   
   if (pDBContext == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return(NULL);
   }
   
   if (pRecInfo == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NO_RECINFO_STRUCT);
      return(NULL);
   }    
   
   if (wRecId == AEE_DB_RECID_NULL) {
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
   //No need to check if a record with NULL Rec ID can be found here as that condition
   //is ruled out at the start of the function.
   if ((pRecBuf = (byte *) sys_malloc(RecHeaderBuf.wRecSize)) == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_NO_MEMORY);
      return(NULL);
   }
   
   //Read the record from the file into the allocated buffer
   //No need to check if a record with NULL Rec ID can be found here as that condition
   //is ruled out at the start of the function.
   if(IFILE_Read(pdb->pDBFile, (void *)pRecBuf, RecHeaderBuf.wRecSize) == RecHeaderBuf.wRecSize){
      if (pRecInfo) 
         MEMCPY(pRecInfo, (void *) &RecHeaderBuf, sizeof(RecHeaderBuf));
      SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   }
   else{
      sys_free(pRecBuf);
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
   word	idx,endIdx;
   boolean wrapped;
   
   if (pDBContext == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return(AEE_DB_RECID_NULL);
   }

   if (!pdb->pIndex->h.recCount)
   {
      // No records in database
      SetDBError(pDBErr, AEE_DB_ERR_NO_RECORD);
      return(AEE_DB_RECID_NULL);
   }
   
   wrapped = pdb->h.wLastAssignedRecID < pdb->pIndex->e[pdb->pIndex->h.recCount-1].recID;

   if (wCurRecId == AEE_DB_RECID_NULL)
   {
      // First record
      if (wrapped)
      {
         // We have rolled over. Start past rollover mark.
         DB_FindIdxRec(pdb, (word)(pdb->h.wLastAssignedRecID+1), &idx);
      }
      else
         idx = 0;
   }
   else
   {
      if (DB_FindIdxRec(pdb, wCurRecId, &idx))
         // If found, go to next record in index
         // If not found, already pointing to next record
         idx++;	

      endIdx = pdb->pIndex->h.recCount;
      if (wrapped)
      {
         if (idx >= pdb->pIndex->h.recCount)
            idx = 0;
         else if (wCurRecId <= pdb->h.wLastAssignedRecID &&
                  pdb->pIndex->e[idx].recID >  pdb->h.wLastAssignedRecID)
         {
            endIdx = idx;
         }
      }
      
      if (idx == endIdx)
      {
         // No next record
         SetDBError(pDBErr, AEE_DB_ERR_NO_RECORD);
         return(AEE_DB_RECID_NULL);
      }
   }
     
   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   return((word)pdb->pIndex->e[idx].recID);  
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
			
   if (pDBContext == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
      return;
   }

   // Modify record header marking it as deleted
   DB_MarkRecordDelete(pdb, wRecId, pDBErr);
   if (pDBErr && (*pDBErr != AEE_DB_ERR_NO_ERR))
      return;

   pdb->h.wCurRecCount--;
   
   //Delete the index entry
   DB_SetIdxRecOffset(pdb, wRecId, 0);

   // Time to compress?
   DB_CheckCompress(pdb);

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
   (void)DB_AddUpdateRec(pdb,wRecId,pbBuf,wBufSize,pDBErr);
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
   None.

   See Also:
   None
============================================================================*/
void  OEM_DBFree(
                 OEMCONTEXT pDBContext,
                 byte *  pbyRecBuf,
                 AEE_DBError*   pDBErr
                 ) {
   
    
   if (pbyRecBuf) {
      sys_free(pbyRecBuf);
   }
   
   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
}

/*============================================================================
OEM_DBMakeReadOnly

   Description: 
   This feature is no longer supported. The index is now placed at the end of the
   file for all databases, not just read-only ones.

============================================================================*/
void OEM_DBMakeReadOnly(const char * szDBName, AEE_DBError * pDBErr)
{
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
      if (!STRCMP(pdb->pszFullPath, pszDBName)) {
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
   hNewHandle->h.wLastAssignedRecID     = AEE_DB_RECID_NULL;
   hNewHandle->pNext = NULL;
   hNewHandle->refCount = 1;

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

   // Cancel the compression callback
   CALLBACK_Cancel(&pdb->cb);

   // Find the entry, unlink it from the list and free it...

   for(pPrev = NULL,pFind = gpActiveDBs; pFind; pFind = pFind->pNext){
      if(pFind == pdb){

         if(pPrev)
            pPrev->pNext = pdb->pNext;
         else
            gpActiveDBs = pdb->pNext;

         if (pdb->pDBFile) 
            IFILE_Release(pdb->pDBFile);
   
         sys_freeobj((void **)(&pdb->pIndex));
         sys_freeobj((void **)(&pdb->pszFullPath));

         sys_free((void *) pdb);
         return;
      }
      pPrev = pFind;
   }   
}

/*============================================================================
DB_MarkRecordDelete

   Description: 
   This function marks the the specified record as deleted by giving it
   a null record id in the record header. The record will be physically
   removed from the data base file when the file is compressed.

============================================================================*/
static void DB_MarkRecordDelete(
                         OEMDBase *pdb,
                         word  wRecId, 
                         AEE_DBError* pDBErr
                         )
{
   oemdb_rec_header  RecHeaderBuf;
   dword             dwRecOffset;
   
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
   
   //Now delete the record by marking it as invalid.
   RecHeaderBuf.wRecID = AEE_DB_RECID_NULL; 
   
   //Now let's write the record header back.
   dwRecOffset = DB_GetIdxRecOffset(pdb, wRecId);
   if (IFILE_Seek(pdb->pDBFile, _SEEK_START, dwRecOffset)==SUCCESS)
      if (IFILE_Write(pdb->pDBFile, &RecHeaderBuf, sizeof(oemdb_rec_header))==sizeof(oemdb_rec_header))
      {
         pdb->pIndex->h.delCount++;
      }

   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   
}
                                          
/*============================================================================
DB_CheckCompress

   Description: 
   This function compresses the specified database if the number of deleted
   records reaches a certain percentage of the good records.

   Prototype:
   void   DB_CheckCompress(OEMDBase *pdb) 

   Parameter(s):
   pdb:    OEMDBase *      Pointer to active database structure
============================================================================*/
static void DB_CheckCompress(OEMDBase *pdb)
{

   // Only compress if necessary...
   if(pdb->pIndex->h.delCount >= ((word)DB_COMPRESS_PERCENT*((uint32)pdb->pIndex->h.recCount)/100) &&
	   pdb->pIndex->h.delCount >= DB_COMPRESS_COUNT &&
      !pdb->dwOffsetFrom)
   {
      DB_Compress(pdb);
   }
}

/*============================================================================
DB_Compress

   Description: 
   This function compresses the specified database by removing deleted records
   from the database file. The record IDs are also made contiguous and the
   index is updated accordingly.

   Prototype:
   void   DB_Compress(OEMDBase *pdb) 

   Parameter(s):
   pdb:    OEMDBase *      Pointer to active database structure

============================================================================*/
static void DB_Compress(OEMDBase *pdb)
{
   byte *            pbyTmpRecBuf = NULL;
   oemdb_rec_header  RecHeaderBuf, DeletedRecHeaderBuf;
   uint32            dwTime = GETUPTIMEMS();
   boolean           bCompressionComplete = FALSE;
   uint32            dwDelta = 0;

#if defined(DBG_DB)
   DBGPRINTF("Compressing %s %d", pdb->pszFullPath,pdb->h.wCurRecCount);
#endif

   // Make sure we don't still have index on end of file
   RemoveIndexFromFile(pdb);
   
   if (!pdb->dwOffsetFrom)
      pdb->dwOffsetFrom = pdb->dwOffsetTo = sizeof(oemdb_header);
      
   while ((dwTime + DB_COMPRESSION_TIME_SLICE) >= GETUPTIMEMS()){
      
      IFILE_Seek(pdb->pDBFile, _SEEK_START, pdb->dwOffsetFrom);
      if(IFILE_Read(pdb->pDBFile, (void *)&RecHeaderBuf, sizeof(oemdb_rec_header)) != sizeof(oemdb_rec_header))
      {
         bCompressionComplete = TRUE;
         break;
      }

      if(RecHeaderBuf.wRecID != AEE_DB_RECID_NULL) {

         // Only write record if in a different location
         if (pdb->dwOffsetFrom != pdb->dwOffsetTo)
         {
            uint32 dwBuffSz = 0;
            // Read and write a valid record...
            
            // Get a buffer for the record header, record data, and a following
            // record header indicating a deleted record up to the next unmoved record
            if ((pbyTmpRecBuf = (byte*) 
                  sys_malloc(RecHeaderBuf.wRecSize + 2*sizeof(oemdb_rec_header) + 2*sizeof(uint32))) == NULL)
               break;

            if(IFILE_Read(pdb->pDBFile, (void *)(pbyTmpRecBuf+sizeof(oemdb_rec_header)), 
                  RecHeaderBuf.wRecSize) != RecHeaderBuf.wRecSize){
               sys_free(pbyTmpRecBuf);
               break;
            }

            // Put record header before data
            MEMCPY(pbyTmpRecBuf,(void *)&RecHeaderBuf,sizeof(oemdb_rec_header));
               
            // Put deleted record header after data in case system crashes during compaction
            // This will leave a valid database file
            DeletedRecHeaderBuf.wRecID = AEE_DB_RECID_NULL;
            dwBuffSz = RecHeaderBuf.wRecSize + sizeof(oemdb_rec_header);
            dwDelta = (pdb->dwOffsetFrom - pdb->dwOffsetTo - sizeof(oemdb_rec_header));
            
            if (dwDelta <= MAX_REC_SIZE) {
               DeletedRecHeaderBuf.wRecSize = (word)dwDelta;
               MEMCPY(pbyTmpRecBuf + dwBuffSz, (void*)&DeletedRecHeaderBuf, sizeof(oemdb_rec_header));
               dwBuffSz += sizeof(oemdb_rec_header);
            }
            else { /* dwDelta > MAX_REC_SIZE, store record length in payload */
               uint32 dwMagic = (uint32) NULL_REC_MAGIC_NUM;
               DeletedRecHeaderBuf.wRecSize = 0;
               
               MEMCPY(pbyTmpRecBuf + dwBuffSz,(void*)&DeletedRecHeaderBuf,sizeof(oemdb_rec_header));
               dwBuffSz += sizeof(oemdb_rec_header);
               MEMCPY(pbyTmpRecBuf + dwBuffSz,(void*)&dwMagic, sizeof(uint32)); // magic no.
               dwBuffSz += sizeof(uint32);
               MEMCPY(pbyTmpRecBuf + dwBuffSz,(void*)&dwDelta,sizeof(uint32));  // actual size of null rec.
               dwBuffSz += sizeof(uint32);
            }
               
            // Write out record
            IFILE_Seek(pdb->pDBFile, _SEEK_START, pdb->dwOffsetTo);
            IFILE_Write(pdb->pDBFile, pbyTmpRecBuf, dwBuffSz);
            sys_free(pbyTmpRecBuf);

            // Update Record Offset in Record Index
            DB_SetIdxRecOffset(pdb, RecHeaderBuf.wRecID, pdb->dwOffsetTo);
         }
            
         // Advance to next writing position
         pdb->dwOffsetTo += sizeof(oemdb_rec_header) + RecHeaderBuf.wRecSize;
      }
         
      // Go to next reading position
      pdb->dwOffsetFrom += sizeof(oemdb_rec_header);
      
      if ((RecHeaderBuf.wRecID != AEE_DB_RECID_NULL) || (0 != RecHeaderBuf.wRecSize)) {
            pdb->dwOffsetFrom += RecHeaderBuf.wRecSize;
      }
      else { /* null rec with 0 size */
         uint32 dwVal = 0;
         int32 nErr;
         nErr = IFILE_Seek(pdb->pDBFile, _SEEK_START, pdb->dwOffsetFrom);
         if(SUCCESS == nErr) {
            nErr = IFILE_Read(pdb->pDBFile, (void*)&dwVal, sizeof(uint32));
            if ((dwVal == (uint32)NULL_REC_MAGIC_NUM) && (sizeof(uint32) == nErr)) {
               dwVal = 0;
               nErr = IFILE_Read(pdb->pDBFile, (void*)&dwVal, sizeof(uint32));
               if (sizeof(uint32) == nErr) {
                  pdb->dwOffsetFrom += dwVal;
               }
            }
         }
      }
   }

   if (bCompressionComplete)
   {
      // Set end of file
      IFILE_Truncate(pdb->pDBFile, pdb->dwOffsetTo);
      
      // Reset dwOffsetFrom
      pdb->dwOffsetFrom = 0;

      DB_PopulateIdx(pdb);
   }
   else
   {
      if(pdb->dwOffsetFrom != pdb->dwOffsetTo) {
         // Put deleted record header after data in case system crashes while we are waiting
         // for resume. This will leave a valid database file.
         
         uint32 dwDelta = (pdb->dwOffsetFrom - pdb->dwOffsetTo - sizeof(oemdb_rec_header));
         DeletedRecHeaderBuf.wRecID = AEE_DB_RECID_NULL;
         
         IFILE_Seek(pdb->pDBFile, _SEEK_START, pdb->dwOffsetTo);
         if (dwDelta <= MAX_REC_SIZE) {
            DeletedRecHeaderBuf.wRecSize = (word)dwDelta;
            IFILE_Write(pdb->pDBFile, (void*)&DeletedRecHeaderBuf, sizeof(oemdb_rec_header));
         }
         else {
            uint32 dwMagic = (uint32) NULL_REC_MAGIC_NUM;
            byte* pBuf = 0;
            DeletedRecHeaderBuf.wRecSize = 0;
            pBuf = (byte*) sys_malloc(sizeof(oemdb_rec_header) + 2*sizeof(uint32));
            if (pBuf) {
               MEMCPY(pBuf, (void*)&DeletedRecHeaderBuf, sizeof(oemdb_rec_header));
               MEMCPY(pBuf + sizeof(oemdb_rec_header), (void*)&dwMagic, sizeof(uint32));
               MEMCPY(pBuf + sizeof(oemdb_rec_header) + sizeof(uint32), (void*)&dwDelta, sizeof(uint32));
               IFILE_Write(pdb->pDBFile, (void*)pBuf, sizeof(oemdb_rec_header) + 2*sizeof(uint32));
               sys_free(pBuf);
               pBuf = 0;
            }
         }
      }
      
      // Resume Compression
      CALLBACK_Cancel(&pdb->cb);
      CALLBACK_Init(&pdb->cb,DB_Compress,pdb);
      {
         IShell* pIShell = AEE_GetShell();     
         ISHELL_Resume(pIShell ,&pdb->cb);
      }	 
   }
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
   IFileMgr *        pfm = NULL;
   oemdb_rec_header  OrigRecHeader;
   int addedRecord = 0;
   
   if (pDBContext == NULL) {
      SetDBError(pDBErr, AEE_DB_ERR_BAD_HANDLE);
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

   if (wRecID == AEE_DB_RECID_NULL) {
      //If this is the first record in the database
      if (pdb->h.wLastAssignedRecID == AEE_DB_RECID_NULL) 
         RecHeaderBuf.wRecID = pdb->h.wLastAssignedRecID = 0;
      else {
         RecHeaderBuf.wRecID = GetNewRecordID(pdb);
         if(RecHeaderBuf.wRecID == AEE_DB_RECID_NULL) {
            SetDBError(pDBErr, AEE_DB_ERR_TOO_MANY_RECORD);
            return AEE_DB_RECID_NULL;
         }
      }
      addedRecord = 1;
   }
   else {
      // Load original record header
      dwRecPos = DB_GetIdxRecOffset(pdb, wRecID);
      if (!dwRecPos) {
         //Release file manager
         ReleaseFileMgr(pfm);
         SetDBError(pDBErr, AEE_DB_ERR_BAD_RECID);
         return AEE_DB_RECID_NULL;
      }
      if (!DB_ReadRecHeader(pdb, dwRecPos, &OrigRecHeader,pDBErr)) {
         //Release file manager
         ReleaseFileMgr(pfm);
         //The error is already set by OEM_DBRecordGet, so just return
         return AEE_DB_RECID_NULL;
      }
  
   // SAS - See if the size of the records matches.  If so, just update it in place!

      RecHeaderBuf.wRecID = wRecID;

      // Orig record cannot be a NULL Rec.
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

   // Remove index from end of file if it's still there
   RemoveIndexFromFile(pdb);

   //Now let's check if there's space to write this file
   if ((IFILEMGR_GetFreeSpace(pfm, NULL)) < (sizeof(oemdb_rec_header) + wBufSize)) {
      // Insufficient file space   
      SetDBError(pDBErr, AEE_DB_ERR_NO_FS_SPACE);
      ReleaseFileMgr(pfm);
      return AEE_DB_RECID_NULL;
   }
   
   if (addedRecord == 1) 
   {
       pdb->h.wCurRecCount++;

       // DB header is modified. Write it.	
       IFILE_Seek(pdb->pDBFile, _SEEK_START, 0);   
       IFILE_Write(pdb->pDBFile, &pdb->h, sizeof(oemdb_header));
   }

   //First go to end of file to add record.
   IFILE_Seek(pdb->pDBFile, _SEEK_END, 0);
   //Now determine where we are by doing a tell
   dwRecPos = IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, 0);

   //May be we should do error checks here, but I don't know the enums
   //that IFILE methods will return yet.
   IFILE_Write(pdb->pDBFile, &RecHeaderBuf, sizeof(oemdb_rec_header));
   IFILE_Write(pdb->pDBFile, pbBuf, wBufSize);

   if (wRecID != AEE_DB_RECID_NULL) {
      //This is a good time to mark the old record as invalid.
      DB_MarkRecordDelete(pdb, wRecID, pDBErr);
   }

   //Now let's set the right location in the index
   if (DB_SetIdxRecOffset(pdb, RecHeaderBuf.wRecID, dwRecPos))
   {
      // Error adding index
      SetDBError(pDBErr, AEE_DB_ERR_NO_MEMORY);
      ReleaseFileMgr(pfm);
      return AEE_DB_RECID_NULL;
   }
   
   // See if we need to compress the data file
   DB_CheckCompress(pdb);

aouDone:

   //Set error to no error and return happily    
   SetDBError(pDBErr, AEE_DB_ERR_NO_ERR);
   //Release file manager
   ReleaseFileMgr(pfm);
   
   return RecHeaderBuf.wRecID;
   
}

/*============================================================================
DB_PopulateIdx

   Description: 
   This function populates the index for an open database.

   Prototype:
   AEE_DBError  DB_PopulateIdx(OEMDBase *pdb)

   Parameter(s):
   pdb:    OEMDBase *      Pointer to active database structure


   Return Value: 
   SUCCESS, if successful, else EFAILED.
   Side Efects:
   None

   Comments:
   None

============================================================================*/
static AEE_DBError  DB_PopulateIdx(OEMDBase *pdb)
{
   oemdb_rec_header    DBRecordHeader; 
   word                wRecCount  = 0;
   word                wDelCount = 0;
   word                wTotalRecCount = 0;
   dword               dwRecOffset;
   dword               dwReadSize;
   oemdb_header        DBHeader;
   int                 count;
   db_index*           pIndex;

   //To populate the database file, we first reset the seek pointer to the
   //begninning of the file
   IFILE_Seek(pdb->pDBFile, _SEEK_START, 0);   
   
   //First read the database header. We may need to fix it too,
   //if the max record id or the current record count are out of whack.
   count = IFILE_Read(pdb->pDBFile, (void *)&DBHeader, sizeof(oemdb_header));
   if(count != sizeof(oemdb_header))
      return AEE_DB_ERR_BAD_STATE;
   
   // Allocate space for index
   pIndex = (db_index*)sys_realloc(pdb->pIndex,sizeof(index_header)+(pdb->h.wCurRecCount + DB_INCREASE_SIZE)*sizeof(index_entry));
   if (!pIndex)
      return AEE_DB_ERR_NO_MEMORY;
   pdb->pIndex = pIndex;
   pdb->indexAlloc = pdb->h.wCurRecCount + DB_INCREASE_SIZE;
 
   //Do a tell to find the position of the first record
   dwRecOffset = IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, 0);
   dwReadSize = IFILE_Read(pdb->pDBFile, (void *)&DBRecordHeader, sizeof(oemdb_rec_header));
   
   while(dwReadSize == sizeof(oemdb_rec_header)) {
      if (DBRecordHeader.wRecID != AEE_DB_RECID_NULL) {
               
         // Make sure we have room
         if (wRecCount >= pdb->indexAlloc)
         {
            pdb->indexAlloc += DB_INCREASE_SIZE;
            pIndex = (db_index*)sys_realloc(pdb->pIndex,sizeof(index_header)+pdb->indexAlloc*sizeof(index_entry));
            if (!pIndex)
               return AEE_DB_ERR_NO_MEMORY;
            pdb->pIndex = pIndex;
         }
         
         // Save offset in index array
         pdb->pIndex->e[wRecCount].recID = DBRecordHeader.wRecID;
         pdb->pIndex->e[wRecCount].dwOffset = dwRecOffset;
      
         wRecCount++;
      }
      else
         wDelCount++;

      //Now move to the end of the record.
      dwRecOffset += sizeof(oemdb_rec_header);
      
      if ((AEE_DB_RECID_NULL == DBRecordHeader.wRecID) &&
          (0 == DBRecordHeader.wRecSize)) {
          uint32 dwVal = 0;
          int32  nErr;
          nErr = IFILE_Read(pdb->pDBFile, (void*)&dwVal, sizeof(uint32));
          if ((dwVal == NULL_REC_MAGIC_NUM) && (sizeof(uint32) == nErr)) {
             dwVal = 0;
             nErr = IFILE_Read(pdb->pDBFile, (void*)&dwVal, sizeof(uint32));
             if (sizeof(uint32) == nErr) {
                IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, (dwVal - 8));  // as 8 bytes already read.
                dwRecOffset += dwVal; // no need to less by 8 here.
             }
          }
      }
      else {
         IFILE_Seek(pdb->pDBFile, _SEEK_CURRENT, DBRecordHeader.wRecSize);
         dwRecOffset += DBRecordHeader.wRecSize;
      }
      
      //Read the next record header
      dwReadSize = IFILE_Read(pdb->pDBFile, (void *)&DBRecordHeader, sizeof(oemdb_rec_header));
      wTotalRecCount++;
   }
   
   //Fix the max. record id and current record countfor this database.
   if (wTotalRecCount == 0)
      pdb->h.wLastAssignedRecID = AEE_DB_RECID_NULL;

   pdb->h.wCurRecCount = wRecCount;
   pdb->pIndex->h.recCount = wRecCount;
   pdb->pIndex->h.delCount = wDelCount;
   
   QSORT((void*)pdb->pIndex->e,wRecCount,sizeof(index_entry),CmpRecID);

   return AEE_DB_ERR_NO_ERR;
}

/*============================================================================
CmpRecID

   Description: 
   Compare function for use with qsort to sort index by record ID

============================================================================*/
static int CmpRecID(const void *p1, const void *p2)
{
	word recID1 = (word)((index_entry *)p1)->recID;
	word recID2 = (word)((index_entry *)p2)->recID;

	if (recID1 < recID2)
		return -1;
	if (recID1 > recID2)
		return 1;
	return 0;
}

/*============================================================================
DB_GetIdxRecOffset

   Description: 
   This function reads the index array to return the offset of the specified
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
	word	idx;
	
	if (DB_FindIdxRec(pdb, wRecId, &idx))
		return pdb->pIndex->e[idx].dwOffset;
	else
		return 0;
}

/*============================================================================
DB_SetIdxRecOffset

   Description: 
   This function sets the offset of the specified
   record in the database file in the index array.

============================================================================*/
static AEE_DBError DB_SetIdxRecOffset(OEMDBase *pdb, word wRecId, dword offset)
{
	word	idx;
	boolean found = DB_FindIdxRec(pdb, wRecId, &idx);
	
	if (offset)
	{
		if (found)
			// Modify an entry
			pdb->pIndex->e[idx].dwOffset = offset;
		else
		{
			// Add an entry
			if (pdb->pIndex->h.recCount >= pdb->indexAlloc)
			{
				// Need to increase size of index
				db_index* pIndex = (db_index*)sys_realloc(pdb->pIndex,sizeof(index_header)+(pdb->indexAlloc + DB_INCREASE_SIZE)*sizeof(index_entry));
				if (!pIndex)
					return AEE_DB_ERR_NO_MEMORY;
				pdb->pIndex = pIndex;
				pdb->indexAlloc += DB_INCREASE_SIZE;
			}
			idx = pdb->pIndex->h.recCount++;
			pdb->pIndex->e[idx].dwOffset = offset;
			pdb->pIndex->e[idx].recID = wRecId;
			
			// Sort new entry into its correct position
			QSORT((void*)pdb->pIndex->e,pdb->pIndex->h.recCount,sizeof(index_entry),CmpRecID);
		}
	}
	else if (found)
	{
		// Delete entry
		MEMMOVE((void*)&pdb->pIndex->e[idx],(void*)&pdb->pIndex->e[idx+1],(pdb->pIndex->h.recCount-idx-1)*sizeof(index_entry));
		pdb->pIndex->h.recCount--;
	}

   // Index has been updated. It will need to be saved. Make sure it's removed from
   // end of file.
   RemoveIndexFromFile(pdb);
   return AEE_DB_ERR_NO_ERR;
}

/*============================================================================
DB_FindIdxRec

   Description: 
   This function searches the index for the entry with the indicated record ID.

============================================================================*/
static boolean DB_FindIdxRec(OEMDBase *pdb, word wRecId, word *pResult)
{
	int	low, high, mid;
	
	low = 0;
	high = pdb->pIndex->h.recCount;
	
	if (wRecId == AEE_DB_RECID_NULL || !pdb->pIndex)
	{
		*pResult = high;
		return FALSE;
	}

	// Index is sorted so use binary search.
	while (low < high)
	{
		word idxRecId;

		mid = low + (high-low) / 2;
		idxRecId = (word)pdb->pIndex->e[mid].recID;
		if (wRecId < idxRecId)
			high = mid;
		else if (wRecId > idxRecId)
			low = mid + 1;
		else
		{
			// Found!
			*pResult = mid;
			return TRUE;
		}
	}
	*pResult = low;	// Return next item
	return FALSE;
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
   IShell* pIShell = AEE_GetShell();
   
   ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)&pFileMgr);

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

   if(pdb->h.wLastAssignedRecID == AEE_DB_MAX_RECNUM)
      // Rollover
      pdb->h.wLastAssignedRecID = 0;
   else
      pdb->h.wLastAssignedRecID++;
   
   if (pdb->h.wLastAssignedRecID < pdb->pIndex->e[pdb->pIndex->h.recCount-1].recID)
   {
      // We have rolled over. Make sure we have an unused ID.      
      word	idx;
      
      if (DB_FindIdxRec(pdb, pdb->h.wLastAssignedRecID, &idx))
      {
         // This ID is used. Find an unused one
         word	i;
         
         for(i=idx+1,pdb->h.wLastAssignedRecID++;i!=idx;i++,pdb->h.wLastAssignedRecID++)
         {
            if (i >= pdb->pIndex->h.recCount)
            {
               // End of index. End of IDs?
               if (pdb->h.wLastAssignedRecID != AEE_DB_MAX_RECNUM)
                  // ID is ok. Use it.
                  break;

               // Wrap around index and record ID
               pdb->h.wLastAssignedRecID = 0;
               i = 0;
            }
            if (pdb->h.wLastAssignedRecID != pdb->pIndex->e[i].recID)
               break;
         }
         if(i == idx)
            // Unable to find a record ID
            return AEE_DB_RECID_NULL;
     }

   }   
   return(pdb->h.wLastAssignedRecID);
}

static IFile * DB_OpenFile(IFileMgr * pfm,const char * pszFullPath,int nCacheSize,AEEOpenFileMode mode)
{
   IFile *     pf;
   ACONTEXT *  pacRest;

   // Note - Some application may need to share the open datebase. Therefore,
   // we set the access to root and the app context to NULL so the file
   // will not be closed when the current app exits...

   // Since we're leaving the application, we have to check access before 
   //  calling IFileMgr (IFILEMGR_OpenFile() uses the current AppContext
   //  on OpenFile to check, normally.
   
   {
      uint32 dwRights;

      // only ever called with _OFM_READWRITE or OFM_CREATE
      dwRights = AEEFP_READ|AEEFP_WRITE;
      
      if (mode & _OFM_CREATE) {
         dwRights |= AEEFP_CREATE;
      }
            
      if (SUCCESS != AEE_CheckPathAccess(pszFullPath,dwRights,0)) {
         return NULL;
      }
   }

   pacRest = AEE_EnterAppContext(NULL);

   pf = IFILEMGR_OpenFile(pfm, pszFullPath, mode);

   AEE_LeaveAppContext(pacRest);

   if(pf && nCacheSize)
      IFILE_SetCacheSize(pf, nCacheSize);

   return(pf);
}

/*============================================================================
LoadIndex

   Description:
	   Load index from file if exists, else build it.

============================================================================*/
static AEE_DBError LoadIndex(OEMDBase *pdb)
{
	if (pdb->h.dwIndexOffset)
	{
		// Index exists in file. Load it.
		
		// Start with header
		index_header	hdr;
		uint32 count;

		if (IFILE_Seek(pdb->pDBFile, _SEEK_START, pdb->h.dwIndexOffset)!=SUCCESS)
		   return AEE_DB_ERR_BAD_STATE;

		count = IFILE_Read(pdb->pDBFile, (void*)&hdr, sizeof(hdr));
		
		if (count == sizeof(hdr))
		{
			// Got header. Now get rest.
 			db_index *pIndex = (db_index*)sys_realloc(pdb->pIndex,sizeof(index_header)+(hdr.recCount+DB_INCREASE_SIZE)*sizeof(index_entry));
			if (pIndex)
			{
			   pdb->pIndex = pIndex;
				count = IFILE_Read(pdb->pDBFile, (void*)pdb->pIndex->e, hdr.recCount*sizeof(index_entry));
				
				// Put header in index
				pdb->pIndex->h = hdr;
				pdb->indexAlloc = hdr.recCount+DB_INCREASE_SIZE;
				
				if (count == hdr.recCount*sizeof(index_entry))
					return AEE_DB_ERR_NO_ERR;	// Success!
			}
			else
				// Memory error
				return AEE_DB_ERR_NO_MEMORY;
		}
	}

	// Non-memory errors fall through to here to rebuild the index
	
	// Make sure we don't still have index on end of file
	RemoveIndexFromFile(pdb);

	// Regenerate index
	return DB_PopulateIdx(pdb);
}

/*============================================================================
RemoveIndexFromFile

   Description:
	   Remove index from end of file

============================================================================*/
static void RemoveIndexFromFile(OEMDBase *pdb)
{
	if (pdb->h.dwIndexOffset)
	{
		IFILE_Truncate(pdb->pDBFile,pdb->h.dwIndexOffset);
		pdb->h.dwIndexOffset = 0;

		// Write DB header	
		IFILE_Seek(pdb->pDBFile, _SEEK_START, 0);   
		IFILE_Write(pdb->pDBFile, &pdb->h, sizeof(oemdb_header));
	}
}

#endif   // AEE_OEMDB

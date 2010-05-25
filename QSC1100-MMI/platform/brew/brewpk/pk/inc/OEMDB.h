/*****************************************************************************
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

     O E M       D A T A B A S E      I N T E R F A C E

  This file contains the exported interfaces and definitions for a reference
  implementation of a simple database manager.
  Each record in the database has a unique ID and can be variable length.

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*****************************************************************************/

#ifndef OEMDB_H
#define OEMDB_H

// Standard Headers
#include "AEE_OEMComdef.h"
#include "AEEComdef.h"

#define AEE_DB_MAX_RECNUM       0xfffe // Max. record ID


typedef enum _AEE_DBError{
 AEE_DB_ERR_NO_ERR,          // DB is OK, operation succeeded
 AEE_DB_ERR_NO_MEMORY,       // Not enough memory for this operation
 AEE_DB_ERR_BAD_HANDLE,      // DB handle is not valid
 AEE_DB_ERR_BAD_RECID,       // The record ID is not valid
 AEE_DB_ERR_BAD_STATE,       // DB in bad state, suggest closing and reopening
 AEE_DB_ERR_ALREADY_EXIST,   // DB already exists (cannot create)
 AEE_DB_ERR_NOT_EXIST,       // DB does not exist (can not open)
 AEE_DB_ERR_ALREADY_OPEN,    // DB is open already(can not open/recover)
 AEE_DB_ERR_DB_NOT_OPEN,     // DB is not open, and hence cannot be closed
 AEE_DB_ERR_TOO_MANY_DB,     // There are too many open DBs (can not open)
 AEE_DB_ERR_TOO_MANY_RECORD, // There are too many records (can not add record)
 AEE_DB_ERR_NO_RECORD,       // There are no records in the database
 AEE_DB_ERR_UNKNOWN_FORMAT,  // The data file format unknown
 AEE_DB_ERR_ABORTED,         // DB operation is aborted (e.g in async operation)
 AEE_DB_ERR_NO_FS_SPACE,     // No enough space in file system for the operation
 AEE_DB_ERR_BAD_PATH,        // DB exists in a path that is restricted
 AEE_DB_ERR_OTHER_FS_ERR,    // General file system errors (other than no space)
 AEE_DB_ERR_CANNOT_INIT,     // Cannot initialize database
 AEE_DB_ERR_BAD_RECORD,      // The record specified is invalid
 AEE_DB_ERR_NO_RECINFO_STRUCT, // No pointer to RecInfo struct
 AEE_DB_ERR_BAD_NEW_RECORD,  // The new record to be added is invalid
 AEE_DB_ERR_NOT_ALLOWED,     // The requested operation is not allowed on the
                             // specified database (DB could be read-only)
 AEE_DB_ERR_NO_RECBUF,       // No record buffer is allocated for this database
 AEE_DB_ERR_MAX              // For range checking
} AEE_DBError;

// Record Information structure
typedef struct _AEE_DBRecInfo {
    word    wRecID;
    word    wRecSize;         // size of the record
    dword  dwLastModified;    // the time it is modified last
} AEE_DBRecInfo;

//========================================
// Database operations:
//========================================
extern AEE_EXPORTS OEMCONTEXT OEM_DBOpenEx(const char* szDBName,int nCacheSize,AEE_DBError* pDBErr);
extern AEE_EXPORTS OEMCONTEXT OEM_DBOpen(const char* szDBName,AEE_DBError* pDBErr);

extern AEE_EXPORTS OEMCONTEXT OEM_DBCreate(
                                           const char* szDBName, // name of the database
                                           word  wMinRecCount,    // minimum number of records
                                           word  wMinRecSize,     // minimum size of each record
                                           AEE_DBError* pDBErr    // error status
                                          );

extern AEE_EXPORTS void OEM_DBDelete(const char* szDBName, AEE_DBError* pDBErr);

extern AEE_EXPORTS void OEM_DBClose(OEMCONTEXT pDBContext, AEE_DBError*  pDBErr);

extern AEE_EXPORTS word OEM_DBRecordCount(OEMCONTEXT pDBContext, AEE_DBError*  pDBErr);

extern AEE_EXPORTS void OEM_DBMakeReadOnly(const char * szDBName, AEE_DBError * pDBErr);


//========================================
// Record Operations
//========================================
extern AEE_EXPORTS byte* OEM_DBRecordGet(
                                         OEMCONTEXT pDBContext,
                                         word  wRecId,
                                         AEE_DBRecInfo*  pRecInfo,
                                         AEE_DBError* pDBErr
                                        );

extern AEE_EXPORTS word OEM_DBRecordPrev(
                                         OEMCONTEXT pDBContext,
                                         word  wCurRecId,
                                         AEE_DBError* pDBErr
                                        );

extern AEE_EXPORTS word OEM_DBRecordNext(
                                         OEMCONTEXT pDBContext,
                                         word  wCurRecid,
                                         AEE_DBError* pDBErr
                                        );

extern AEE_EXPORTS void OEM_DBRecordDelete(
                                           OEMCONTEXT pDBContext,
                                           word  wRecId,
                                           AEE_DBError* pDBErr
                                          );

extern AEE_EXPORTS int OEM_DBRecordAdd(
                                       OEMCONTEXT pDBContext,
                                       const byte*  pbBuf,
                                       word  wBufSize,
                                       AEE_DBError*   pDBErr
                                      );

extern AEE_EXPORTS void OEM_DBRecordUpdate(
                                           OEMCONTEXT pDBContext,
                                           word  wRecId,
                                           const byte*  pbBuf,
                                           word  wBufSize,
                                           AEE_DBError*   pDBErr
                                          );

extern AEE_EXPORTS void OEM_DBFree(
                                   OEMCONTEXT pDBContext,
                                   byte *  pbyRecBuf,
                                   AEE_DBError*   pDBErr
                                  );

#endif //OEM_DB_H

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEE_DBError

Description:
   AEE_DBError is used to indicate the error in OEMDB to the AEE layer.
   A pointer to this error type is passed to every OEMDB function
   and is populated by every OEMDB function.

Definition:
   typedef enum _AEE_DBError{
      AEE_DB_ERR_NO_ERR,
      AEE_DB_ERR_NO_MEMORY,
      AEE_DB_ERR_BAD_HANDLE,
      AEE_DB_ERR_BAD_RECID,
      AEE_DB_ERR_BAD_STATE,
      AEE_DB_ERR_ALREADY_EXIST,
      AEE_DB_ERR_NOT_EXIST,
      AEE_DB_ERR_ALREADY_OPEN,
      AEE_DB_ERR_DB_NOT_OPEN,
      AEE_DB_ERR_TOO_MANY_DB,
      AEE_DB_ERR_TOO_MANY_RECORD,
      AEE_DB_ERR_NO_RECORD,
      AEE_DB_ERR_UNKNOWN_FORMAT,
      AEE_DB_ERR_ABORTED,
      AEE_DB_ERR_NO_FS_SPACE,
      AEE_DB_ERR_BAD_PATH,
      AEE_DB_ERR_OTHER_FS_ERR,
      AEE_DB_ERR_CANNOT_INIT,
      AEE_DB_ERR_BAD_RECORD,
      AEE_DB_ERR_NO_RECINFO_STRUCT,
      AEE_DB_ERR_BAD_NEW_RECORD,
      AEE_DB_ERR_NOT_ALLOWED,
      AEE_DB_ERR_NO_RECBUF,
      AEE_DB_ERR_MAX
   } AEE_DBError;

Members:
   AEE_DB_ERR_NO_ERR:            DB is OK; operation succeeded.
   AEE_DB_ERR_NO_MEMORY:         There is not enough memory for this operation.
   AEE_DB_ERR_BAD_HANDLE:        The DB handle is not valid.
   AEE_DB_ERR_BAD_RECID:         The record ID is not valid.
   AEE_DB_ERR_BAD_STATE:         DB is in a bad state; suggest closing
                                 and reopening.
   AEE_DB_ERR_ALREADY_EXIST:     DB already exists (cannot create).
   AEE_DB_ERR_NOT_EXIST:         DB does not exist (cannot open).
   AEE_DB_ERR_ALREADY_OPEN:      DB is open already (cannot open/recover).
   AEE_DB_ERR_DB_NOT_OPEN:       DB is not open, and hence cannot be closed.
   AEE_DB_ERR_TOO_MANY_DB:       There are too many open DBs (cannot open).
   AEE_DB_ERR_TOO_MANY_RECORD:   There are too many records (cannot add
                                 record).
   AEE_DB_ERR_NO_RECORD:         There are no records in the database.
   AEE_DB_ERR_UNKNOWN_FORMAT:    The data file format is unknown.
   AEE_DB_ERR_ABORTED:           The DB operation is aborted (for example, in async
                                 operation).
   AEE_DB_ERR_NO_FS_SPACE:       There is not enough space in the file system for the
                                 operation.
   AEE_DB_ERR_BAD_PATH:          DB exists in a path that is restricted.
   AEE_DB_ERR_OTHER_FS_ERR:      There are general file system errors (other than
                                 no space).
   AEE_DB_ERR_CANNOT_INIT:       Cannot initialize database.
   AEE_DB_ERR_BAD_RECORD:        The record specified is invalid.
   AEE_DB_ERR_NO_RECINFO_STRUCT: There is no pointer to RecInfo struct.
   AEE_DB_ERR_BAD_NEW_RECORD:    The new record to be added is invalid.
   AEE_DB_ERR_NOT_ALLOWED:       The requested operation is not allowed on
                                 the specified database (DB could be
                                 read-only).
   AEE_DB_ERR_NO_RECBUF:         No record buffer is allocated for this
                                 database.
   AEE_DB_ERR_MAX:               This is for range-checking.

Comments:
    None.

See Also:
    None

=======================================================================

AEE_DBRecInfo

Description:
   Record Information structure. The OEM returns information on the
   record requested by OEM_DBRecordGet.

Definition:

   typedef struct _AEE_DBRecInfo {
       word    wRecID;
       word    wRecSize;
       dword  dwLastModified;
   } AEE_DBRecInfo;


Members:
   wRecID:         record ID of the record
   wRecSize:       size of the record (in bytes) excluding the header
   dwLastModified: the time (in CDMA time) this record was last modified

Comments:
   None

See Also:
   OEM_DBRecordGet

========================================================================
  INTERFACE DOCUMENTATION
=======================================================================
OEM_DB Interface

This interface allows management of a database.
========================================================================

Function: OEM_DBOpen()

Description:
   This function opens a database and returns a handle to the opened
   database. If the database does not exist, the function returns
   NULL. It does not create the database.

Prototype:
   OEMCONTEXT   OEM_DBOpen(const char* szDBName, AEE_DBError* pDBErr)

Parameters:
   szDBName: name of the database to be opened

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.

Return Value:
   OEMCONTEXT: If successful, this is used as the handle to the
   opened database.
   NULL: if fails. If pDBErr is non-null, the actual error code is placed inside pDBErr.

Comments:
   The database szDBName must exist. Use OEM_DBCreate() to create a database
   and open it.

Side Effects:
   None

See Also:
   OEM_DBCreate()
   OEM_DBClose()

========================================================================

Function: OEM_DBCreate()

Description:
   This function creates a new database and returns a handle to the created
   database. If wMinRecCount and wMinRecSize are specified, the function
   also reserves memory for this database.

Prototype:
   OEMCONTEXT   OEM_DBCreate(
                           const char* szDBName,
                           word  wMinRecCount,
                           word  wMinRecSize,
                           AEE_DBError* pDBErr
                           );

Parameters:
   szDBName: name of the database to be created

   WMinRecCount: minimum number of records that this database must support.
   It is used in conjunction with wminRecSize to determine the total memory
   to be reserved for the database. If this parameter is 0 (zero), it is ignored.

   wMinRecSize: minimum size of the record. This is used in conjunction with the
   wminRecCount to determine the total memory to be reserved for the database.
   If this parameter is 0(zero), it is ignored.

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.


Return Value:
   OEMCONTEXT: If successful, this is used as a handle to the newly
   created database.
   NULL: if fails. If pDBErr is non-null, it contains the actual
   error code on return.

Comments:
   None

Side Effects:
   None

See Also:
   OEM_DBOpen()
   OEM_DBDelete()

===========================================================================

Function: OEM_DBDelete()

Description:
   This function deletes the database specified by szDBName.

Prototype:
   void   OEM_DBdelete(const char* szDBName, AEE_DBError* pDBErr);

Parameters:
   szDBName: name of the database to be deleted

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.


Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   OEM_DBCreate()

==========================================================================

Function: OEM_DBClose()

Description:
   This function closes the database specified by pDBContext. As part of
   closing, the memory associated with this database is freed.

Prototype:
   void   OEM_DBClose(OEMCONTEXT pDBContext, AEE_DBError*  pDBErr)

Parameters:
   pDBContext: handle of the database to be closed

   pDBErr: placeholder to contain error code information on return.
           If NULL, no error code is returned.


Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   OEM_DBOpen()

========================================================================

Function: OEM_DBRecordCount()

Description:
   This function returns the number of records in the database specified
   by pDBContext.

Prototype:
   word   OEM_DBRecordCount(OEMCONTEXT pDBContext, AEE_DBError*  pDBErr)

Parameters:
   pDBContext: handle of the database whose record count is required.

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.


Return Value:
   Number of records in the database: if successful
   0 (zero): if fails.

Comments:
   None

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_DBRecordGet()

  Description:
  This function retrieves a specified record from the database.
  For the given record Id (wRecId), the function retrieves the
  RecordInformation and the data associated with that record.
  The RecordInformation is returned by the parameter pRecInfo
  passed to this function. The data associated with that record
  is returned as a byte*. The memory for the  byte* is allocated
  from the heap. It is the caller's responsibility to free the
  memory later. The function does not remove the record from
  the database. It returns a copy of the information stored
  in that record.

 Prototype:
    byte*  OEM_DBRecordGet(
                         OEMCONTEXT pDBContext,
                         word  wRecId,
                         AEE_DBRecInfo*  pRecInfo,
                         AEE_DBError* pDBErr
                         )

Parameters:
   pDBContext: handle of the database whose record is to be retrieved.

   wRecId: ID of the record to be retrieved from the database

   pRecInfo: information about the retrieved record is returned to the caller
   using the AEE_DBRecInfo structure.

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.


Return Value:
   byte *: If successful, this points to the data stored in the
   specified record.

   NULL: if fails.


Comments:
   The record returned is a copy of the actual data stored. The
   memory for this copy is allocated from the heap.

Side Effects:
   None

See Also:
   None


========================================================================

Function: OEM_DBRecordPrev

Description:
   This function retrieves the ID of the record previous to the given record ID.
   It returns OEM_DB_RECID_NULL if WCurRecId is the first record ID in the database.
   If OEM_DB_RECID_NULL is passed in as WCurRecId, then the last record will be returned
   unless the database is empty.

Prototype:
   byte*  OEM_DBRecordPrev(
                           OEMCONTEXT pdb,
                           word  wCurRecid,
                           AEE_DBError* pDBErr
                           )

Parameters:
   pdb:      OEMCONTEXT         database handle

   WCurRecId:      Word            Specifies the ID of the current record.
                                   The record before this record is
                                   retrieved from the database.

   PDBErr:         AEE_DBError *   placeholder to contain error code
                                   information on return.
                                   If NULL, no error code is returned.


Return Value:
   If successful, returns the ID of the record next to the given record ID.
   If failed, returns OEM_DB_RECID_NULL.


Comments:
   This is not affiliated with AEE in any way right now. This function is not
   required to be implemented. This is a useful helper utility.

See Also:
   None

============================================================================

Function: OEM_DBRecordNext()

Description:
   This function retrieves the ID of the record next to the given
   record ID. If the given recordID is OEM_DB_RECID_NULL, this
   function returns the ID of the first record in the given database.
   It returns OEM_DB_RECID_NULL if wCurRecId is the Max. record ID
   in the database.

Prototype:
   byte*  OEM_DBRecordNext(
                           OEMCONTEXT pDBContext,
                           word  wCurRecid,
                           AEE_DBError* pDBErr
                           )

Parameters:
   pDBContext:handle of the database.

   wCurRecId:  Specifies the ID of the current record.  The record next to
   this record is retrieved from the database.

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.


Return Value:
   ID of the record next to the given record: If successful.
   OEM_DB_RECID_NULL: If fails.


Comments:
   None

Side Effects:
   None

See Also:
   None


========================================================================

Function: OEM_DBRecordDelete()

Description:
   This function deletes the specified record from the database.

Prototype:
   void  OEM_DBRecordDelete(
                           OEMCONTEXT pDBContext,
                           word  wRecId,
                           AEE_DBError* pDBErr
                           );

Parameters:
   pDBContext: handle of the database

   wRecId: ID of the record

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.


Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None


========================================================================

Function: OEM_DBRecordAdd()

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

Parameters:
   pdb: database handle

   pbBuf: pointer to the buffer containing data to be stored in the new record

   wBufSize: Size of the buffer pointed to by pbBuf.

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.


Return Value:
   ID    of the newly added record if successful
   OEM_REC_ID_NULL  If fails


Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEM_DBRecordUpdate()

Description:
   This function updates the contents of the given record ID.

Prototype:
   void  OEM_DBRecordUpdate(
                           OEMCONTEXT pDBContext,
                           word  wRecId,
                           const byte*  pbBuf,
                           word  wBufSize,
                           AEE_DBError*   pDBErr
                           )


Parameters:
   pDBContext: database handle

   wRecId: ID of the record whose contents are to be updated.

   pbBuf: pointer to the buffer containing updated data to be stored in the given record.

   wBufSize: size of the buffer pointed to by pbBuf

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.


Return Value:
   None.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEM_DBFree()

Description:
   This function frees the memory allocated for the buffer returned by
   OEM_DBRecordGet.

Prototype:
   void  OEM_DBFree(
                  OEMCONTEXT pDBContext,
                  byte * pbyRecBuf
                  AEE_DBError * pDBErr
                  )


Parameters:
   pDBContext: handle of the database

   pbyRecBuf: pointer to the previously allocated record buffer that is to be freed.

   pDBErr: placeholder to contain error code information on return.
           If NULL, no error code is returned.


Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_DBMakeReadOnly()

Description:
   This function makes the specified database file read-only.
   The contents of the index file are appended to the end of the database,
   and the database is marked as read-only. No add, update, or delete operations
   are subsequently allowed on the database.

Prototype:
   void  OEM_DBMakeReadOnly(
                     const char * szDBName,
                     AEE_DBError * pDBErr
                   )


Parameters:
   szDBName: name of the database that needs to be made read-only.

   pDBErr: placeholder to contain error code information on return.
   If NULL, no error code is returned.


Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None
==============================================================
AEEDatabase


See the IDatabase Interface, the IDBMgr Interface and the IDBRecord Interface in the BREW API Reference.

========================================================================*/

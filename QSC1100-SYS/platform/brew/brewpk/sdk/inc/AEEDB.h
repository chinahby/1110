#ifndef AEEDB_H
#define AEEDB_H
/*===========================================================================

FILE:      AEEDB.h


SERVICES:  AEE Database Interface services for application developers of mobile devices


DESCRIPTION: This file provides defintions for the Database interfaces made available by
the AEE to application developers. This is a standard header file that must be included by
all applications using the database services of the AEE


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable


        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"

// TBD: Need to get rid of this file. Temporary!!!!!!
//#include "DBTypes.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
#define AEEDB_TYPE_NONE        0
#define AEEDB_TYPE_CONTACT     1
#define AEEDB_TYPE_SCHEDULE    2
#define AEEDB_TYPE_MEMO        3

enum {
   AEEDB_FT_NONE,

   // These types are stored only as values (DBFileBasicField) - no length...

   AEEDB_FT_BYTE,          // 8 bit value
   AEEDB_FT_WORD,          // 16 bit value
   AEEDB_FT_DWORD,         // 32 bit value

   // All types below stored with length first (DBFileField)...

   AEEDB_FT_STRING,        // AECHAR array (0 terminated)
   AEEDB_FT_BINARY,        // binary value
   AEEDB_FT_PHONE,         // Phone number field
   AEEDB_FT_BITMAP,        // QBMP format

   AEEDB_FT_COMPLEX,       // Packed field data [[header][[id|len|type|data].....]]
   AEEDB_FT_MAX
};
#ifdef WIN32
typedef unsigned AEEDBFieldType;
#else
typedef int8 AEEDBFieldType;
#endif


enum {
   AEEDBFIELD_NONE,
   AEEDBFIELD_FULLNAME,
   AEEDBFIELD_LASTNAME,
   AEEDBFIELD_FIRSTNAME,
   AEEDBFIELD_HOME_PHONE,
   AEEDBFIELD_WORK_PHONE,
   AEEDBFIELD_MOBILE_PHONE,
   AEEDBFIELD_FAX,
   AEEDBFIELD_ADDRESS,
   AEEDBFIELD_EMAIL,
   AEEDBFIELD_URL,
   AEEDBFIELD_DATE_TIME,
   AEEDBFIELD_CATEGORY,
   AEEDBFIELD_ALARM,
   AEEDBFIELD_PREF_ID,
   AEEDBFIELD_PREF_VER,
   AEEDBFIELD_PREF_DATA,
   AEEDBFIELD_TITLE,
   AEEDBFIELD_TEXT
};
#ifdef WIN32
typedef unsigned AEEDBFieldName;
#else
typedef int8 AEEDBFieldName;
#endif


// ***************************************************************
//
//   Simple database field header.  Basic data types of known
//   size (word, dword, byte) are stored with no length indicator
//
// ***************************************************************

typedef PACKED struct
{
   byte           byFieldType;
   byte           byFieldName;
} GCCPACKED AEEDBFileBasicField;


// ***************************************************************
//
//  Advanced database field header.  Variable size fields are stored
//  with a parameter to indicate their length...
//
// ***************************************************************

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
typedef PACKED struct
{
   byte           byFieldType;
   byte           byFieldName;
   uint16         wFieldLen;
} GCCPACKED AEEDBFileField;



//Definition of Database Field
typedef struct
{
   AEEDBFieldType    fType;      // Field Type
   AEEDBFieldName    fName;      // Field Name
   uint16            wDataLen;   // Data Length
   void *            pBuffer;    // Data Ptr
} AEEDBField;


//Complex data type item
typedef PACKED struct
{
   uint16   fID;        // Field ID (use uint16 instead of AEEDBFieldName to be compatible with AEEAddrFieldIDs)
   uint16   wDataLen;   // Number of bytes in buf[]
   int8     fType;      // Field type (use int8 instead of AEEDBFieldType to use same size on simulator and on target)
   uint8    buf[1];     // Variable number of bytes of data

} AEEDBSubfield;

//Complex data type
typedef PACKED struct
{
   uint16   wNumSubfields; // Number of AEEDBComplexItem's
   uint8    byVersion;     // Version of the complex type.  Allows future defintion of different subfield types.
   uint8    byReserved;
   uint8    buf[1];        // Variable number of bytes containing AEEDBSubfield structs
} AEEDBComplex;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------
#define AEE_DB_RECID_NULL       0xffff // NULL Record ID

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------


//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================


//===============================================================================
// CLASS DEFINITIONS
//===============================================================================



//-------------------------------------------------------------------
// Interface Definitions
//-------------------------------------------------------------------

typedef struct _IDBMgr        IDBMgr;
typedef struct _IDatabase     IDatabase;
typedef struct _IDBRecord     IDBRecord;

//********************************************************************************************************************************
//
// IDBMgr Interface
//
//********************************************************************************************************************************


QINTERFACE(IDBMgr)
{
   DECLARE_IBASE(IDBMgr)

   IDatabase  *    (*OpenDatabase)(IDBMgr * po, const char * pszFile, boolean bCreate, uint32 dwMinSize, uint16 wMinRecs);
   int             (*Remove)(IDBMgr * po, const char * pszFile);
   void            (*SetCacheSize)(IDBMgr * po, int nSize);
};

// Access Macros for IDBMgr interface

#define IDBMGR_AddRef(p)                              GET_PVTBL(p,IDBMgr)->AddRef(p)
#define IDBMGR_Release(p)                             GET_PVTBL(p,IDBMgr)->Release(p)
#define IDBMGR_OpenDatabase(p, psz, bc)               GET_PVTBL(p,IDBMgr)->OpenDatabase(p, psz, bc,0,0)
#define IDBMGR_OpenDatabaseEx(p, psz, bc,ws,wr)       GET_PVTBL(p,IDBMgr)->OpenDatabase(p, psz, bc,ws,wr) //RameshC
#define IDBMGR_Remove(p, psz)                         GET_PVTBL(p,IDBMgr)->Remove(p, psz)
#define IDBMGR_SetCacheSize(p,n)                      GET_PVTBL(p,IDBMgr)->SetCacheSize(p,n)


//********************************************************************************************************************************
//
// IDatabase Interface
//
//********************************************************************************************************************************

QINTERFACE(IDatabase)
{
   DECLARE_IBASE(IDatabase)

   uint16         (*GetRecordCount)(IDatabase * po);
   void           (*Reset)(IDatabase * po);
   IDBRecord *    (*GetNextRecord)(IDatabase * po);
   IDBRecord *    (*GetRecordByID)(IDatabase * po, uint16 wID);
   IDBRecord *    (*CreateRecord)(IDatabase * po, AEEDBField * pFields, int nFieldCount);
};

//Access Macros for IDatabase Interface
#define IDATABASE_AddRef(p)                     GET_PVTBL(p,IDatabase)->AddRef(p)
#define IDATABASE_Release(p)                    GET_PVTBL(p,IDatabase)->Release(p)
#define IDATABASE_GetRecordCount(p)             GET_PVTBL(p,IDatabase)->GetRecordCount(p)
#define IDATABASE_Reset(p)                      GET_PVTBL(p,IDatabase)->Reset(p)
#define IDATABASE_GetNextRecord(p)              GET_PVTBL(p,IDatabase)->GetNextRecord(p)
#define IDATABASE_GetRecordByID(p, wID)         GET_PVTBL(p,IDatabase)->GetRecordByID(p,wID)
#define IDATABASE_CreateRecord(p,pf,nc)       GET_PVTBL(p,IDatabase)->CreateRecord(p,pf,nc)


//********************************************************************************************************************************
//
// IDBRecord Interface
//
//********************************************************************************************************************************

QINTERFACE(IDBRecord)
{
   DECLARE_IBASE(IDBRecord)

   void           ( *Reset)(IDBRecord * po);
   int            ( *Update)(IDBRecord * po,AEEDBField * pFields, int nFieldCount);
   int            ( *Remove)(IDBRecord * pRec);
   uint16         ( *GetID)(IDBRecord * pRec);

   AEEDBFieldType    ( *NextField)(IDBRecord * po, AEEDBFieldName * pName, uint16 * pnLen);
   byte *         ( *GetField)(IDBRecord * po, AEEDBFieldName * pName, AEEDBFieldType * pType, uint16 * pnLen);
   boolean        ( *GetFieldWord)(IDBRecord * po, uint16 * pw);
   boolean        ( *GetFieldDWord)(IDBRecord * po, uint32 * pdw);
   AECHAR *   (*GetFieldString)(IDBRecord * po);
};


//Access Macros for IDBRecord Interface
#define IDBRECORD_AddRef(p)                   GET_PVTBL(p,IDBRecord)->AddRef(p)
#define IDBRECORD_Release(p)                  GET_PVTBL(p,IDBRecord)->Release(p)
#define IDBRECORD_Reset(p)                    GET_PVTBL(p,IDBRecord)->Reset(p)
#define IDBRECORD_Update(p,pf, nc)            GET_PVTBL(p,IDBRecord)->Update(p, pf, nc)
#define IDBRECORD_Remove(p)                   GET_PVTBL(p,IDBRecord)->Remove(p)
#define IDBRECORD_GetID(p)                    GET_PVTBL(p,IDBRecord)->GetID(p)

#define IDBRECORD_NextField(p,pn,pl)          GET_PVTBL(p,IDBRecord)->NextField(p,pn,pl)
#define IDBRECORD_GetField(p,pn,pt,pl)        GET_PVTBL(p,IDBRecord)->GetField(p,pn,pt,pl)
#define IDBRECORD_GetFieldWord(p,pw)          GET_PVTBL(p,IDBRecord)->GetFieldWord(p,pw)
#define IDBRECORD_GetFieldDWord(p,pdw)        GET_PVTBL(p,IDBRecord)->GetFieldDWord(p,pdw)
#define IDBRECORD_GetFieldString(p)           GET_PVTBL(p,IDBRecord)->GetFieldString(p)


#endif    // AEEDB_H

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEEDBField

Description:
This structure defines fields in a record. This structure is used to retrieve and
update database fields.

Definition:
typedef struct
{
   AEEDBFieldType fType;
   AEEDBFieldName fName;
   uint16 wDataLen;
   void * pBuffer;
} AEEDBField;

Members:

fType : Database field type (documentation)
fName : Database field Name (documentation)
wDataLen : Data Length (excluding header) of the field
pBuffer : Pointer to buffer containing the field data

Comments:
None

See Also:
   AEEDBFieldType
   AEEDBFieldName

=============================================================================

AEEDBFieldName

Description:
AEEDBFieldName is used by IDBRECORD_NextField() and IDBRECORD_GetField() to return
the field name of the field.

Definition:
typedef enum {
   AEEDBFIELD_NONE,
   AEEDBFIELD_FULLNAME,
   AEEDBFIELD_LASTNAME,
   AEEDBFIELD_FIRSTNAME,
   AEEDBFIELD_HOME_PHONE,
   AEEDBFIELD_WORK_PHONE,
   AEEDBFIELD_MOBILE_PHONE,
   AEEDBFIELD_FAX,
   AEEDBFIELD_ADDRESS,
   AEEDBFIELD_EMAIL,
   AEEDBFIELD_URL,
   AEEDBFIELD_DATE_TIME,
   AEEDBFIELD_CATEGORY,
   AEEDBFIELD_ALARM,
   AEEDBFIELD_PREF_ID,
   AEEDBFIELD_PREF_VER,
   AEEDBFIELD_PREF_DATA,
   AEEDBFIELD_TITLE,
   AEEDBFIELD_TEXT
} AEEDBFieldName;

Members:

AEEDBFIELD_FULLNAME : Field contains a full name
AEEDBFIELD_LASTNAME : Field contains a last name
AEEDBFIELD_FIRSTNAME : Field contains a first name
AEEDBFIELD_HOME_PHONE : Field contains a home phone number
AEEDBFIELD_WORK_PHONE : Field contains a work phone number
AEEDBFIELD_MOBILE_PHONE : Field contains a mobile phone number
AEEDBFIELD_FAX : Field contains a fax number
AEEDBFIELD_ADDRESS : Field contains a mailing address
AEEDBFIELD_EMAIL : Field contains an email address
AEEDBFIELD_URL : Field contains a URL
AEEDBFIELD_DATE_TIME : Field contains date and time
AEEDBFIELD_CATEGORY : Field contains a category specification
AEEDBFIELD_ALARM : Field contains an alarm
AEEDBFIELD_PREF_ID : Field contains a Preference ID
AEEDBFIELD_PREF_VER : Field contains a Preference Version Number
AEEDBFIELD_PREF_DATA : Field contains Preference Data (such as user preference)
AEEDBFIELD_TITLE : Field contains a Title
AEEDBFIELD_TEXT : Field contains text


Comments:
None

See Also:
None

=============================================================================

AEEDBFieldType

Description:
AEEDBFieldType is used by IDBRECORD_GetField() to return the field type of the specified
field.

Definition:
typedef enum {
   AEEDB_FT_NONE,
   AEEDB_FT_BYTE,
   AEEDB_FT_WORD,
   AEEDB_FT_DWORD,
   AEEDB_FT_STRING,
   AEEDB_FT_BINARY,
   AEEDB_FT_PHONE,
   AEEDB_FT_BITMAP,
   AEEDB_FT_COMPLEX,
   AEEDB_FT_MAX
} AEEDBFieldType;

Members:

AEEDB_FT_BYTE : Field contains an 8 bit value
AEEDB_FT_WORD : Field contains a 16 bit value
AEEDB_FT_DWORD : Field contains a 32 bit value
AEEDB_FT_STRING : Field contains an AECHAR array (0 terminated)
AEEDB_FT_BINARY : Field contains a Binary value
AEEDB_FT_PHONE : Field contains a Phone number (AECHAR array (0 terminated))
AEEDB_FT_BITMAP : Field contains a bitmap in .BMP format
AEEDB_FT_COMPLEX : Field contains a packed list of fields


Comments:
   The AEEDB_FT_COMPLEX type is a packed array of bytes that contain
a list of fields called subfields.  The purpose of this type is to 
associate data that is unique to one field.  The packed array of bytes
should begin with one AEEDBComplex structure and be followed by a
variable number of AEEDBSubfield structures.

See Also:
   AEEAddrFieldType
   AEEDBComplex
   AEEDBSubfield

=======================================================================

AEEDBComplex

Description:
This structure defines the header field of a complex data type.

Definition:
typedef PACKED struct
{
   uint16   wNumSubfields;
   uint8    byVersion;
   uint8    byReserved;
   uint8    buf[1];
} AEEDBComplex;

Members:

wNumSubfields : Number of AEEDBComplexItem's that follow the header.
byVersion : Version of the complex type.  Allows future defintion of different complex types.
byReserved : Unused.
buf : Variable size array of bytes which contains nSubfields AEEDBSubfield structs.

Comments:
Set the byVersion to 0.

See Also:
   AEEDBFieldType
   AEEDBSubfield

=======================================================================

AEEDBSubfield

Description:
This structure defines a subfields within a field of type AEEDB_FT_COMPLEX.

Definition:
typedef PACKED struct
{
   uint16   fID;
   uint16   wDataLen;
   int8     fType;
   uint8    buf[1];
} AEEDBSubfield;

Members:

fID : Field ID (is uint16 instead of AEEDBFieldName to be compatible with AEEAddrFieldIDs)
wDataLen : Number of bytes in buf[]
fType : Field type (use int8 instead of AEEDBFieldType to use same size on simulator and on target)
buf : Variable size array of bytes which contains the subfield data.

Comments:
None

See Also:
   AEEDBFieldType
   AEEDBComplex

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================


Interface Name: IDBMgr

Description:
    The IDBMgr Interface functions are used to create, open, and remove
databases, which are collections of multi-field records. Once a database
has been opened, you use functions in the IDatabase Interface to create
and retrieve database records and close the database, and you use functions
in the IDBRecord Interface to access and update the fields of individual
records.

CAUTION: Your application must have a privilege level of File or All to
be able to invoke the functions in this interface that create or delete
the database.

The function IDBMGR_OpenDatabase() opens an existing database given its
name, which corresponds to the name of a BREW file that holds the database's
contents. When calling this function, you can request that the database be
created if it does not already exist. IDBMGR_OpenDatabaseEx() is similar,
but it also allows you to specify a minimum record size and minimum number
of records when creating a new database; the function reserves an amount of
memory sufficient to hold the specified number of records. Both of these
functions return a pointer to an instance of the IDatabase Interface ,
which can be used to access the opened database.

The function IDBMGR_Remove() removes a database by deleting the file that
holds its contents. If the database is open, you should first call
IDBMGR_Release() to close it prior to removal.
===H2>
To use the functions in the IDBMgr Interface
===/H2>
===p>
1. Call ISHELL_CreateInstance() to create an instance of the IDBMgr Interface .~
2. Call IDBMGR_OpenDatabase() to open an existing database or to create a new one.~
If you need to specify a minimum size for a newly created database, use
IDBMGR_OpenDatabaseEx().~
3. Using the IDatabase Interface pointer obtained in step 2, call functions in
the IDatabase Interface to create and retrieve records in the database. You
can also use IDBRecord Interface functions to access and modify the fields of
database records.~
4. Call IDATABASE_Release() to close the database when you have completed
accessing it.~
5. Call IDBMGR_Remove() to remove the database when necessary.~
6. Call IDBMGR_Release() when you no longer need the IDBMgr Interface instance.

The following header file is required:

AEEDB.h

==============================================================================

IDBMGR_AddRef()

Description:
    This function increments the reference count of the IDBMgr Interface object.
This allows the object to be shared by multiple callers. The object is freed when
the reference count reaches 0 (zero).

Prototype:
    uint32 IDBMGR_AddRef(IDBMgr * pIDBMgr)

Parameters:
    pIDBMgr : Pointer to the IDBMgr Interface object

Return Value:
    Incremented reference count for the object.

Comments:
    A valid object returns a positive reference count.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDBMGR_Release()

=======================================================================

IDBMGR_OpenDatabase()

Description:
    This function opens the specified database. If the database is already
open, it returns the interface object for the already open database and
increments the reference count for that object. Boolean flag bCreate 
specifies action to be takenif database does not exist. If the specified 
database does not exist, and bCreate is TRUE, this function creates a new 
database with minimum records and minimum record size parameters both set 
to 0 (zero).

Prototype:
    IDatabase * IDBMGR_OpenDatabase(IDBMgr * pif, const char * pszFile, boolean bCreate)

Parameters:
    pif : Pointer to the IDBMgr Interface object
    pszFile : Null terminated string denoting the Database file name
    bCreate : Specifies if the database must be created if the database is not found.
              The database is created only if this flag is set to true.

Return Value:
    pointer : To the IDatabase object, if successful

    NULL : If unsuccessful

Comments:
    Use IDATABASE_Release() for closing a database.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
None

=======================================================================

IDBMGR_OpenDatabaseEx()

Description:
    This function opens the specified database. If the database is already
open, it returns the interface object for the already open database and
increments the reference count for that object. Boolean flag bCreate specifies 
action to be taken if database does not exist. If the specified 
database does not exist, and bCreate is TRUE, this function creates 
a new database. While creating the database, the minimum
record size can be specified by dwMinSize, and the minimum 
number of records can be specified by wMinRecs.

Prototype:
    IDatabase * IDBMGR_OpenDatabaseEx(IDBMgr * pif, const char * pszFile, boolean bCreate, uint32 dwMinSize, uint16 wMinRecs)

Parameters:
    pif : Pointer to the IDBMgr Interface
    pszFile : Null terminated string denoting the Database file name
    bCreate : Specifies if the database must be created if the database is not
              found. The database is created only if this flag is set to true.
    dwMinSize : Minimum size of the records in the database
    wMinRecs : Minimum number of records that the database should hold

Return Value:
    pointer : To the IDatabase object, if successful

    NULL : If unsuccessful

Comments:
    Use IDATABASE_Release() for closing a database.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IDBMGR_Remove()

Description:
    This function removes the specified database.

Prototype:
    int IDBMGR_Remove(IDBMgr * pif, const char * pszFile)

Parameters:
    pif : IDBMgr * pointer to IDBMgr object
    pszFile : Null terminated string specifying the name of the database
              to be removed

Return Value:
    SUCCESS : If database was successfully removed

    EBADFILENAME : If database file could not be found

Comments:
    Once the database is removed, the corresponding database file is deleted.
Subsequent opens or accesses to a removed database fail.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
None

=======================================================================

IDBMGR_SetCacheSize()

Description:
    This function sets the size for file-caching operations on the database
This allows callers to selectively buffer file access to improve performance.
This function must be called prior to calls to IDBGMGR_OpenDatabase.

Prototype:
    void IDBMGR_SetCacheSize(IDBMgr * po, int32 nSize)

Parameters:
    po:  Pointer to IDBMgr object
    nSize:  Size of cache.

   SCS_DEFAULT - Use default file cache size specified by OEM

Return Value:
    None

Comments:
    Availability of this feature is reliant upon the underlying database
implementation and may not be supported on some devices.

Side Effects:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    None

=======================================================================

IDBMGR_Release()

Description:
    This function decrements the reference count of the IDBMgr Interface
object. The object is freed from memory and is no longer valid once the
reference count reaches 0 (zero).

Prototype:
    uint32 IDBMGR_Release(IDBMgr * pIDBMgr)

Parameters:
    pIDBMgr : Pointer to the IDBMgr Interface object

Return Value:
    Reference count : Decremented reference count for the object

    0 (zero) : If the object has been freed and is no longer valid

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDBMGR_AddRef()

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IDatabase

Description:
    The IDatabase Interface functions allow you to create and access
records in databases created and opened with the IDBMgr Interface. To
obtain an instance of the IDatabase Interface , you call
IDBMGR_OpenDatabase() to open the desired database. You then use the
IDatabase Interface pointer returned by this function to access the
database with the operations described below. You can also use
functions in the IDBRecord Interface to access the fields of individual
database records. When you have completed access to the database, you
call IDATABASE_Release() to close it.

CAUTION: Your application must have a privilege level of File or All
to be able to invoke the functions in this interface that modify the
contents of the database.

The IDATABASE_CreateRecord() function creates a new record and adds it
to your database (the function IDBRECORD_Remove() is used to remove a
record from the database). Each record contains one or more fields.
Each field is defined by the AEEDBField structure, which includes the
following elements:~
-The field name is a descriptor of the field's contents, (name, phone
number, email address, etc.) The AEEDBFieldName enumerated type contains
constants for commonly used field names.
-The field type gives the data type of the field (byte, word, double-word,
character string, binary, phone number or bitmap).
-The field buffer pointer is a pointer to the actual contents of the field.
-The field length is the length in bytes of the field contents.

When creating a record, you populate an array of AEEDBField structures to
specify the name, type, contents and length of each field in the record.
You then call IDATABASE_CreateRecord() , supplying a pointer to this array
and the number of AEEDBField structures it contains as input.
IDATABASE_CreateRecord() returns a pointer to an instance of the IDBRecord
Interface that can be used to access and update fields of the record.
The number and type of fields are specified on a per-record basis when
the record is created or updated; there is no requirement that all records
in a given database have the same structure.
Once you have created a database and added records to it, you can use other
IDatabase functions to retrieve records from the database.
===H2>
To retrieve every record in the database
===/H2>
===p>
1. Call IDATABASE_Reset() to set the record index to 0 (zero).~
2. Repeatedly call IDATABASE_GetNextRecord() to obtain an IDBRecord Interface
pointer for each record in the database. IDATABASE_GetNextRecord() returns
NULL when all the records have been enumerated.

Each database record is assigned a unique ID when it is created (the ID of
a record can be obtained with the IDBRECORD_GetID() function). The function
IDATABASE_GetRecordByID() lets you retrieve a database record with a given
ID, returning an IDBRecord pointer that can be used to access the record.
The function IDATABASE_GetRecordCount() returns the number of records in
the database.
===H2>
To use functions in the IDatabase Interface
===/H2>
===p>
1. Call ISHELL_CreateInstance() if necessary to obtain an instance of the
IDBMgr Interface .
2. Call IDBMGR_OpenDatabase() or IDBMGR_OpenDatabaseEx() to obtain an IDatabase
Interface pointer to a new or existing database.
3. Call IDATABASE_CreateRecord() to create new records and add them to the
database opened in step 2.
4. Call IDATABASE_Reset() and IDATABASE_GetNextRecord() if you need to
enumerate all the records in the database, for example, to find all
records that match certain criteria. Call IDATABASE_GetRecordByID()
to retrieve a particular record given its ID.
5. Call IDATABASE_Release() to close the database when you have completed
accessing it.

The following header file is required:

AEEDB.h

==================================================================

IDATABASE_AddRef()

Description:
    This function increments the reference count of IDatabase Interface object.

Prototype:
    uint32 IDATABASE_AddRef(IDatabase * pIDatabase)

Parameters:
    pIDatabase : Pointer to IDatabase Interface object

Return Value:
    Incremented reference count for the object

Comments:
    A valid object returns a positive reference count.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATABASE_Release()

=======================================================================

IDATABASE_CreateRecord()

Description:
    This function creates a new database record with the fields specified
by pDBFields in the database specified by pIDatabase.

Prototype:
    IDBRecord * IDATABASE_CreateRecord( IDatabase * pIDatabase, AEEDBField * pDBFields, int iNumfields)

Parameters:
    pIDatabase : Pointer to the IDatabase Interface object in which the
                 record is being created
    pDBFields : Pointer to the database fields that need to be placed in
                a new record created by this function
    iNumFields : Number of fields in the record

Return Value:
    Pointer : Pointer to the database record created, if successful

    NULL : If unsuccessful

Comments:
    A new record is added to the database. The IDBRecord must be released
    using the IDBRECORD_Release() before the database is released. If records
    are not released, IDATABASE_Release() cannot close the database.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
None

=======================================================================

IDATABASE_GetNextRecord()

Description:
    This function returns the next IDBRecord from the database.

Prototype:
    IDBRecord * IDATABASE_GetNextRecord(IDatabase * pIDatabase)

Parameters:
    pIDatabase : Pointer to the IDatabase Interface object whose next
                 record is requested

Return Value:
    Pointer : Pointer to the database record, if successful
    NULL : If otherwise

Comments:
    The IDBRecord must be released using the IDBRECORD_Release()
before releasing the database. If records are not released,
IDATABASE_Release() cannot close the database.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IDATABASE_GetRecordByID()

Description:
    This function returns a pointer to the record whose record ID is specified.

Prototype:
    IDBRecord * IDATABASE_GetRecordByID( IDatabase * pIDatabase, uint16 u16RecID)

Parameters:
    pIDatabase : Pointer to the IDatabase Interface object whose record is
                 requested
    u16RecID : Index of record to get

Return Value:
    Pointer : Pointer to the database record whose index is specified,
              if successful

    NULL : If unsuccessful

Comments:
    The IDBRecord must be released using the IDBRECORD_Release() before
    releasing the database. If records are not released, IDATABASE_Release()
    cannot close the database.

Side Effects:
None

Version:
   Introduced BREW Client 1.0

See Also:
None

=======================================================================

IDATABASE_GetRecordCount()

Description:
    This function returns the number of records in the database specified
by pIDatabase.

Prototype:
    uint32 IDATABASE_GetRecordCount(IDatabase * pIDatabase)

Parameters:
    pIDatabase : Pointer to the IDatabase Interface object whose record
                 count is requested

Return Value:
    Number : Number of records in the database, if successful

    0 (zero) : If pIDatabase is pointing to IDatabase Interface object with
               invalid database handle

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IDATABASE_Release()

Description:
    This function decrements the reference count of the IDatabase Interface
object. If the reference count reaches 0 (zero), this function closes the
database.

Prototype:
    uint32 IDATABASE_Release(IDatabase * pIDatabase)

Parameters:
    pIDatabase : Pointer to the IDatabase Interface object whose reference
                 count needs to be decremented

Return Value:
    The updated reference count for the object

Comments:
    Before closing a database all the records of the database must be released
using the IDBRECORD_Release() . If records are not released, IDBRECORD_Release()
cannot close the database.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATABASE_AddRef()
    IDBRECORD_Release()
    IDBMGR_OpenDatabase()
    IDBMGR_OpenDatabaseEx()

=======================================================================

IDATABASE_Reset()

Description:
    This function resets the record index of the database specified by
pIDatabase.

Prototype:
    void IDATABASE_Reset(IDatabase * pIDatabase)

Parameters:
    pIDatabase : Pointer to the IDatabase Interface object whose record
                 index needs to be reset

Return Value:
    None

Comments:
    Calling IDATABASE_GetNextRecord() after IDATABASE_Reset() gets the
    first record in the database.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IDBRecord

Description:
    The IDBRecord Interface contains a set of functions that are used to
access and update the fields of database records. You use functions in the
IDatabase Interface to obtain an instance of the IDBRecord Interface for a
particular record. An IDBRecord Interface pointer is returned by
IDATABASE_CreateRecord() when a new record is created, and by
IDATABASE_GetNextRecord() and IDATABASE_GetRecordByID() when existing records
are retrieved from a database. Once you have obtained an IDBRecord Interface
pointer for a record, you can use it to access its fields with the operations
described below. When you have completed access to the database record, you
call IDBRECORD_Release() to close it.

CAUTION: Your application must have a privilege level of File or All to be
able to invoke the functions in this interface that modify the contents of
the database.

Each field of a record contains its name, data type, a pointer to its contents,
and the length of the contents. The record access functions in the IDBRecord
Interface operate on the current field of the record. The function
IDBRECORD_Reset() makes the first field of the record the current one, and
IDBRECORD_NextField() advances the current field to next field in the record.
IDBRECORD_NextField() also returns the name, data type and length (but not the
contents) of the new current field, or an end-of-record indication when all
the fields of the record have been enumerated. The function IDBRECORD_GetField()
returns a pointer to the contents of the current field, and also returns the
field's name, data type and length. The IDBRecord Interface provides some
simpler functions that can be used to access a field's contents when the data
type of the contents is already known: 
IDBRECORD_GetFieldWord(), IDBRECORD_GetFieldWord(), and 
IDBRECORD_GetFieldString() retrieve the contents
of the current field when its type is word, double-word and character-string,
respectively. Each of these functions returns a failure indication if the current
field's contents are not of the appropriate type.

To modify the fields of a record, you use the function IDBRECORD_Update() .
This function accepts the same input as IDATABASE_CreateRecord() : you supply
a pointer to an array of AEEDBField structures that contains new values for
the name, type, contents and length of each field in the record, along with
the number of fields in the array. To update a single field in a record, you
must specify values for all the fields of the record.

The function IDBRECORD_GetID() returns the unique identifier of a record.
The record ID can be used when calling IDATABASE_GetRecordByID() to retrieve
the record. IDBRECORD_Remove() removes the record from the database.
===H2>
To use functions in the IDBRecord Interface
===/H2>
===p>
1. Call ISHELL_CreateInstance() if necessary to obtain an instance of the IDBMgr
Interface .
2. Call IDBMGR_OpenDatabase() or IDBMGR_OpenDatabaseEx() to obtain an IDatabase
Interface pointer to a new or existing database.
3. Call IDATABASE_CreateRecord() , IDATABASE_GetRecordByID() or
IDATABASE_GetNextRecord() to obtain an IDBRecord Interface pointer for the
record you wish to access.
===H2>
To access the record as needed:
===/H2>
===p>
1. Call IDBRECORD_Reset() and IDBRECORD_NextField() to iterate through the fields
of the record and obtain the name, data type and length of each field. To access
the contents of the current field, use one of the IDBRECORD_GetField() functions
described above.
2. Call IDBRECORD_GetID() to obtain the record's unique ID.
3. Call IDBRECORD_Update() to supply new values for all the fields of the record.
4. Call IDBRECORD_Remove() to remove the record from the database.
5. Call IDBRECORD_Release() to close the record when you have completed accessing
it (if you removed the record in step 4, it is not necessary to release it here).

The following header file is required:

AEEDB.h

==============================================================================

IDBRECORD_AddRef()

Description:
    This function increments the reference count of the IDBRecord Interface object.
This allows the object to be shared by multiple callers. The object is freed when
the reference count reaches 0 (zero).

Prototype:
    uint32 IDBRECORD_AddRef(IDBRecord * pIDBRecord)

Parameters:
    pIDBRecord : Pointer to the IDBRecord Interface object

Return Value:
    Incremented reference count for the object.

Comments:
    A valid object returns a positive reference count.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDBRECORD_Release()

=======================================================================

IDBRECORD_GetField()

Description:
    This function returns the raw data, type, name and length of the current
field. The data returned does NOT include the header, and the length returned
is the data length.

Prototype:
    byte * IDBRECORD_GetField( IDBRecord * pIDBRecord,AEEDBFieldName * pName, AEEDBFieldType * pDBFieldType, uint16 * pnLen)

Parameters:
    pIDBRecord : [in] : Pointer to the IDBRecord Interface object whose field
                        is requested
    pName : [out] : Pointer to the field name
    pDBFieldType : [out] : Pointer to the field type
    pnLen : [out] : Pointer to length of the field (in bytes)

Return Value:
    Pointer : To the buffer containing the requested field, if successful

    NULL : If unsuccessful

Comments:
    To illustrate the usage of this function, consider the following example:
    If the field in the database record corresponds to AEEDBFIELD_LASTNAME,
    and contains an AECHAR string wStr of last name "Smith", then this function
    would return a pointer to a byte buffer containing the AECHAR string "Smith",
    and the parameters would be populated as follows:
===pre>
    * pDBFieldName = AEEDBFIELD_LASTNAME
    * pDBFieldType = AEEDB_FT_STRING
    * pDBFieldLen = WStrlen(wStr) * sizeof(AECHAR) = 6 * 2 = 12 (0 terminated).
===/pre>

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IDBRECORD_GetFieldDWord()

Description:
    This function does an IDBRECORD_GetField() operation on the specified
database record, and checks the returned field type. If the field type is
AEE_FT_DWORD, this function sets pdwRet to point to the buffer containing
the dword.

Prototype:
    boolean IDBRECORD_GetFieldDWord( IDBRecord * pIDBRecord, dword * pdwRet)

Parameters:
    pIDBRecord : Pointer to the IDBRecord Interface object whose field is
                 requested
    pdwRet : Pointer to dword returned by this function

Return Value:
    TRUE : If the field is of type AEE_FT_DWORD

    FALSE : If otherwise

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IDBRECORD_GetFieldString()

Description:
    This function returns a AECHAR * string for a field of that type.

Prototype:
    AECHAR * IDBRECORD_GetFieldString( IDBRecord * pIDBRecord)

Parameters:
    pIDBRecord : Pointer to the IDBRecord Interface object whose field
                 is requested

Return Value:
    pointer : To a string containing the requested field, If successful

    NULL : If unsuccessful

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
None

=======================================================================

IDBRECORD_GetFieldWord()

Description:
    This function does an IDBRECORD_GetField() operation on the specified
database record, and checks the returned field type. If the field type is
AEE_FT_WORD, this function sets pwRet to point to the buffer containing
the word.

Prototype:
    boolean IDBRECORD_GetFieldWord( IDBRecord * pIDBRecord, word * pwRet)

Parameters:
    pIDBRecord : Pointer to the IDBRecord Interface object whose field
                 is requested
    pwRet : Pointer to word returned by this function

Return Value:
    TRUE : If the field is of type AEE_FT_WORD

    FALSE : If otherwise

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
None

=======================================================================

IDBRECORD_GetID()

Description:
    This function gets the ID of the specified record.

Prototype:
    uint16 IDBRECORD_GetID( IDBRecord * pIDBRecord)

Parameters:
    pIDBRecord : Pointer to the IDBRecord Interface object whose ID is
                 requested

Return Value:
    ID : Of the record specified, if successful

    AEE_DB_RECID_NULL : If unsuccessful

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IDBRECORD_NextField()

Description:
    This function sets the field pointer to the next (or first if current
position is -1) field in the record. Returns the type, name and the data
length of the field.

Prototype:
    AEEDBFieldType IDBRECORD_NextField( IDBRecord * pIDBRecord, AEEDBFieldName * pName uint16 * pnLen)

Parameters:
    pIDBRecord : Pointer to the IDBRecord Interface object whose next field
                 is requested
    pName : Pointer to the next field
    pnLen : Pointer to length of the next field

Return Value:
    AEEDBFieldType structure containing field type : If successful

    AEEDB_FT_NONE : If unsuccessful

Comments:
    If the (specified or default) field pointer points to the last field,
an invalid field type is returned.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IDBRECORD_Release()

Description:
    This function decrements the reference count for the database record
object. If the reference count reaches 0 (zero), the database record is
freed from the memory.

Prototype:
    uint32 IDBRECORD_Release(IDBRecord * pIDBRecord)

Parameters:
    pIDBRecord : Pointer to the IDBRecord Interface object object whose
                 reference count needs to be decremented

Return Value:
    Updated reference count of the object

Comments:
    The object is freed and is no longer valid if 0 (zero) is returned.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDBRECORD_AddRef()

=======================================================================

IDBRECORD_Remove()

Description:
    This function removes a record from the database and frees the
IDBRecord object.

Prototype:
    int IDBRECORD_Remove( IDBRecord * pIDBRecord)

Parameters:
    pIDBRecord : Pointer to the IDBRecord which needs to be removed

Return Value:
    SUCCESS : If database record was successfully removed

    AEE_DB_EBADREC : If the record to be removed was in a bad state

    AEE_DB_EBADSTATE : If the database is in a bad state

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IDBRECORD_Reset()

Description:
    This function makes the first field of pIDBRecord the current field.

Prototype:
    void IDBRECORD_Reset(IDBRecord * pIDBRecord)

Parameters:
    pIDBRecord : Pointer to the IDBRecord Interface object which needs to
                 be reset

Return Value:
    None

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IDBRECORD_Update()

Description:
    This function updates a record given a new set of field values.

Prototype:
    int IDBRECORD_Update( IDBRecord * pIDBRecord, AEEDBField * pDBFields, int iNumFields)

Parameters:
    pIDBRecord : Pointer to the IDBRecord Interface object which needs to
                 be updated
    pDBFields : Pointer to the new set of field values
    iNumFields : Number of fields in the new set

Return Value:
    SUCCESS : If database record was successfully updated

    ENOMEMORY : If there was not enough memory for this operation

    AEE_DB_ENULLFIELD : If pDBFields is NULL

    AEE_DB_EBADFIELDNUM : If iNumFields < 0

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================*/

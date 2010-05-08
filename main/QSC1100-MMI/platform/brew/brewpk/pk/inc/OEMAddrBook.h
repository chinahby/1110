#ifndef OEMADDRBOOK_H
#define OEMADDRBOOK_H
/*****************************************************************************
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

     O E M       A D D R E S S       B O O K      I N T E R F A C E

  This file contains the exported interfaces and definitions for access to the
  address book on the handset.

        Copyright ©1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*****************************************************************************/


#include "AEE.h"
#include "AEEAddrBook.h"

#define AEE_ADDR_RECID_NULL       0xffff // NULL Record ID
#define AEE_ADDR_MAX_RECNUM       0xfffe // Max. record ID

#define AEE_ADDR_DEFAULT_PROPERTIES 0

//Field header for each field, excluding data
#ifdef _MSC_VER
#pragma pack(push,1)
#endif

//Datastructure used to store address field info in DB. The actual data (void *)is always stored at the
//end of each item.
typedef PACKED struct
{
   AEEAddrFieldID       fID;        //Field ID (ex: WORK_PHONE, URL etc)
   AEEAddrFieldType     fType;      //Data Type (ex: FT_STRING, etc)
   uint16               wDataLen;   // Data Length
} AEEAddrFieldStore;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

// OEM Address book interface.
typedef struct _IOEMAddrBook   IOEMAddrBook;

QINTERFACE(IOEMAddrBook)
{
   INHERIT_IBase(IOEMAddrBook);

   uint16  (*EnumNextRec)(AEEAddrCat *cat, AEEAddrField **ppItems,
                                                     int *nItemCount,int *pErr);
   int     (*EnumRecInit)(AEEAddrCat wCategory, AEEAddrFieldID wFieldID,
                                                 void *pData, uint16 wDataSize);
   int     (*GetCatCount)(void);
   int     (*GetCatList)(AEEAddrCat *p, int nSize);
   int     (*GetFieldInfo)(AEEAddrCat c, AEEAddrFieldInfo *pf, int nSize);
   int     (*GetFieldInfoCount)(AEEAddrCat);
   uint16  (*GetNumRecs)(void);
   uint16  (*RecordAdd)(AEEAddrCat cat, AEEAddrField *pItems, int nItemCount,
                                                                     int *pErr);
   int     (*RecordDelete)(uint16 recID);
   int     (*RecordGetByID)(uint16 recID,AEEAddrCat *cat,
                             AEEAddrField **ppItems, int *nItemCount,int *pErr);
   int     (*RecordUpdate)(uint16 recID, AEEAddrCat cat, AEEAddrField *pItems,
                                                     int nItemCount, int *pErr);
   int     (*RemoveAllRecs)(void);
   void    (*SetProperties)(uint32 dwProps);
   uint32  (*GetProperties)(void);

   int     (*GetCategoryName)(AEEAddrCat c, AECHAR *pszCatName, int *pnSize);
   int     (*GetFieldName)(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize);
   int     (*EnumCacheInit)(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, 
                                           void *pData, uint16 wDataSize);
   int     (*EnumNextCache)(void **ppCache);
   uint16  (*ExtractCache)(void *pCache, AECHAR **ppName, AEEAddrCat *pCat);
   uint16  (*GetCapacity)(void);   
   int (*GetCacheinfoByNumber)(AECHAR *pwstrNum, AEEAddCacheInfo *pCacheInfo, PFN_NUMBERMATCH pfnMactch);
   int (*CheckSameRecord)(AECHAR *name, boolean* exist);
};

#define IOEMADDR_AddRef(p)                    GET_PVTBL(p,IOEMAddrBook)->AddRef(p)
#define IOEMADDR_Release(p)                   GET_PVTBL(p,IOEMAddrBook)->Release(p)
#define IOEMADDR_EnumNextRec(p,pc,pp,nI,pE)   GET_PVTBL(p,IOEMAddrBook)->EnumNextRec(pc,pp,nI,pE)
#define IOEMADDR_EnumRecInit(p,wc,wf,pd,wd)   GET_PVTBL(p,IOEMAddrBook)->EnumRecInit(wc,wf,pd,wd)
#define IOEMADDR_GetCatCount(p)               GET_PVTBL(p,IOEMAddrBook)->GetCatCount()
#define IOEMADDR_GetCatList(p,pp,ns)          GET_PVTBL(p,IOEMAddrBook)->GetCatList(pp,ns)
#define IOEMADDR_GetFieldInfo(p,c,pf,n)       GET_PVTBL(p,IOEMAddrBook)->GetFieldInfo(c,pf,n)
#define IOEMADDR_GetFieldInfoCount(p,c)       GET_PVTBL(p,IOEMAddrBook)->GetFieldInfoCount(c)
#define IOEMADDR_GetNumRecs(p)                GET_PVTBL(p,IOEMAddrBook)->GetNumRecs()
#define IOEMADDR_RecordAdd(p,c,pi,n,pe)       GET_PVTBL(p,IOEMAddrBook)->RecordAdd(c,pi,n,pe)
#define IOEMADDR_RecordDelete(p,r)            GET_PVTBL(p,IOEMAddrBook)->RecordDelete(r)
#define IOEMADDR_RecordGetByID(p,r,c,pp,n,pe) GET_PVTBL(p,IOEMAddrBook)->RecordGetByID(r,c,pp,n,pe)
#define IOEMADDR_RecordUpdate(p,r,c,pi,n,pe)  GET_PVTBL(p,IOEMAddrBook)->RecordUpdate(r,c,pi,n,pe)
#define IOEMADDR_RemoveAllRecs(p)             GET_PVTBL(p,IOEMAddrBook)->RemoveAllRecs()
#define IOEMADDR_SetProperties(p,d)           GET_PVTBL(p,IOEMAddrBook)->SetProperties(d)
#define IOEMADDR_GetProperties(p)             GET_PVTBL(p,IOEMAddrBook)->GetProperties()
#define IOEMADDR_GetCategoryName(p,c,psz,pn)  GET_PVTBL(p,IOEMAddrBook)->GetCategoryName(c,psz,pn)
#define IOEMADDR_GetFieldName(p,f,psz,pn)     GET_PVTBL(p,IOEMAddrBook)->GetFieldName(f,psz,pn)

#define IOEMADDR_EnumCacheInit(p,cat,field,data,wLen)   \
                 GET_PVTBL(p,IOEMAddrBook)->EnumCacheInit(cat,field,data,wLen)
#define IOEMADDR_EnumNextCache(p,pCache)                \
                 GET_PVTBL(p,IOEMAddrBook)->EnumNextCache(pCache)
#define IOEMADDR_ExtractCache(p,pCache,pName,pCat)      \
                 GET_PVTBL(p,IOEMAddrBook)->ExtractCache(pCache,pName,pCat)
#define IOEMADDR_GetCapacity(p)                         \
                 GET_PVTBL(p,IOEMAddrBook)->GetCapacity()

#define IOEMADDR_GetCacheinfoByNumber(p, pNum,pInfo,pfn)  \
                 GET_PVTBL(p,IOEMAddrBook)->GetCacheinfoByNumber(pNum,pInfo,pfn)

#define IOEMADDR_CheckSameRecord(p, n,e)  \
                 GET_PVTBL(p,IOEMAddrBook)->CheckSameRecord(n,e)
#endif      // OEMADDRBOOK_H



/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEE_ADDR_RECID_NULL

Description:
   Definition of a NULL record. This macro is used within BREW Address Book interface
   to denote a NULL record.

Definition:
#define AEE_ADDR_RECID_NULL       0xffff

Members:

Comments:
    None.

See Also:
    None

=======================================================================

=======================================================================
  INTERFACES DOCUMENTATION
=======================================================================

Interface Name: IOEMAddrBook

Description:

This Interface manages the address book that exists in the OEM layer.

The address book module is responsible for connecting the native UI's address or phone book with BREW. It provides BREW interfaces (IAddrBook and IAddrRec) to access, add, or modify address book records. This allows a BREW applet to interface with the native UI's common database of address book records.

Because there are different address book implementations, this module has the flexibility to accommodate most existing methods and fields.

BREW application developers use the IAddrBook interface to access the address book. On a RUIM-based device, there are two address books: one in the main flash memory and one on the RUIM card. This design allows applications to use the same IAddrBook interface to access all address books. ClassIDs differentiate between the locations of these address books. To access the address book on the main flash, use the ClassID AEECLSID_ADDRBOOK when creating an instance of the IAddrBook interface. To access the address book on the RUIM card, the application uses the ClassID AEECLSID_ADDRBOOK_RUIM. In each case, the application receives the IAddrBook interface.

You must implement the IOEMAddrBook class for the main flash memory address book. On a RUIM-based device, you must also implement a separate IOEMAddrBook class for the address book on the RUIM card.

It is important that the appropriate address book interfaces be added to the gBREWStaticClasses[ ] array in OEMModTable.c. There are multiple versions of the IAddrBook interface. The device must support the current version and the older versions.  Therefore, the following entries should appear in gBREWStaticClasses[ ]:
===pre>
{AEECLSID_ADDRBOOK,      0,PL_ADDRBOOK, NULL, AEEAddrBook_New},
{AEECLSID_ADDRBOOK_21,   0,PL_ADDRBOOK, NULL, AEEAddrBook_New},
{AEECLSID_ADDRBOOK_20,   0,PL_ADDRBOOK, NULL, AEEAddrBook_New},
===/pre>

The IOEMAddrBook interface for the main flash memory address book should be listed with a class ID of AEECLSID_OEMADDRBOOK.
The privilege level is PL_SYSTEM. This prevents applications from accessing the IOEMAddrBook interface to the native address book directly.
===pre>
{AEECLSID_OEMADDRBOOK,   0,PL_SYSTEM,   NULL, OEMAddrBook_New},
===/pre>

On devices with an RUIM, the IOEMAddrBook interface for the RUIM-based address book should be listed with a class ID of AEECLSID_OEMRUIMADDRBOOK.
The privilege level is PL_SYSTEM. This prevents applications from accessing the IOEMAddrBook interface to the RUIM-based address book directly.
===pre>
{AEECLSID_OEMRUIMADDRBOOK, 0,PL_SYSTEM, NULL, OEMRUIMAddrBook_New},
===/pre>

===H2>
Customizing reference implementation
===/H2>

The following are the method types of functions that must be implemented:
~
Discovery method: Use to query the device's particular implementation fields and categories.~
Access method: Use to query, search, add, remove, or modify individual records.~
Auxiliary method: Helps the accessing process only. It does not help to access address book records.~
*

===H2>
Discovery method
===/H2>

These functions are mainly used to query the device's supported categories and fields. The address book category is a type that contains related records and is based on the record's contact usage. For example, an address book may have friends, family, business, work, and others as separate categories. The address book field is an individual item within each record, such as name, phone number, and so forth. You can customize the address book’s categories and fields, but they must reflect the actual categories and fields supported by the target device.
The following functions belong in this category:
~
IOEMADDR_GetCatCount()         Obtains the number of supported address book categories.~
IOEMADDR_GetCatList()         Obtains the list of supported address book categories.~
IOEMADDR_GetFieldInfoCount()         Obtains the number of supported address book fields for the given category.~
IOEMADDR_GetFieldInfo()         Obtains the list of supported address book fields for the given category.~
IOEMADDR_GetCategoryName()        Obtains the AECHAR string name of a category.~
IOEMADDR_GetFieldName()        Obtains the AECHAR string name of a field.~
*

===H2>
Access method
===/H2>

These functions are used to add, remove, update, search, or read each record in the address book. The access methods depend on the categories and fields supported by the device.
The following functions belong in this category:
~
IOEMADDR_RecordAdd()         Creates a new record and inserts into the specified category of the address book.~
IOEMADDR_RecordUpdate()         Modifies an existing record in the specified category of the address book.~
IOEMADDR_RecordDelete()         Deletes an existing record in the specified category in the address book.~
IOEMADDR_RecordGetByID()         Retrieves an existing record given a record ID.~
IOEMADDR_GetNumRecs()         Retrieves the total number of records in the address book.~
IOEMADDR_RemoveAllRecs()         Deletes all existing records in the address book.~
*

===H2>
Auxiliary method
===/H2>

These functions are not used directly to access or retrieve data stored in the address book, but play a crucial role in the inner workings of the address book interface.
The following functions belong in this category:
~
IOEMADDR_AddRef()         Increments the reference count of the IOEMAddrBook object.~
IOEMADDR_Release()         Decrements the reference count of the IOEMAddrBook object and frees all memory and resource associated with the IOEMAddrBook object when the reference count reaches zero.~
IOEMAddr_EnumRecInit()         Initializes the address book record iterator for the specified category and search criteria.~
IOEMAddr_EnumNextRect()         Retrieves the next address book record based on the iterator specified by OEMAddr_EnumRectInit().~
IOEMADDR_SetProperties        Sets address book properties.~
IOEMADDR_GetProperties        Obtains address book properties.~
*






=======================================================================
Function: IOEMADDR_AddRef()

Description:
   This function increments the reference count of the IOEMAddrBook Interface object,
   allowing the object to be shared by multiple callers. The object is freed when the
   reference count reaches 0 (zero).

Prototype:
   uint32 IOEMADDR_AddRef(IOEMAddrBook * pIOEMAddrBook)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

Return Value:
   The incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   IOEMADDR_Release()

=======================================================================
Function: IOEMADDR_Release()

Description:
   This function decrements the reference count of the IOEMAddrBook Interface object. The
   object is freed from memory and is no longer valid when the reference count reaches
   0 (zero).

Prototype:
   uint32 IOEMADDR_Release(IOEMAddrBook * pIOEMAddrBook)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

Return Value:
   Decremented reference count for the object
   0 (zero), if the object has been freed and is no longer valid

Comments:
   None

Side Effects:
   None

See Also:
   IOEMADDR_AddRef()

========================================================================
Function: IOEMADDR_GetCatCount()

Description:
   This function returns the number of address-categories supported by the
   address book. Examples of address-categories are PERSONAL and BUSINESS.
   Each record in the address book can belong to a specific address category.
   If the concept of categories is not supported in the address book, this
   function must return 0 (zero).

Prototype:
   int   IOEMADDR_GetCatCount(IOEMAddrBook * pIOEMAddrBook)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

Return Value:
   The number of categories supported

Comments:
   It is valid to return 0 (zero) from this function if the address book does
   not support the concept of categories for each record.

Side Effects:
   None

See Also:
   IOEMADDR_GetCatList()

========================================================================
Function: IOEMADDR_GetCatList()

Description:
   This function returns information about all the address categories supported
   by the address book in the device. This function is called only if IOEMADDR_GetCatCount()
   returned a value other than 0 (zero).

Prototype:
   int   IOEMADDR_GetCatList(IOEMAddrBook * pIOEMAddrBook, AEEAddrCat *p, int nSize);

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object
   p:  [in/out]:  pointer allocated by the caller that can hold information
   about the address categories
   nSize:  [in]:  number of AEEAddrCat elements that can fit into the array
   pointed to by p

Return Value:
   AEE_SUCCESS: if successful. Even if nSize is less than the total number of
   categories supported, this function must return AEE_SUCCESS as long as nSize
   > 0 (zero).

   EFAILED: if fails

Comments:
   The categories must be converted from the OEM list to the AEE values before
   returning. A list of predefined AEEAddressCategories is in AEEAddrBook.h.
   OEMs can add their own categories too.

Side Effects:
   None

See Also:
   IOEMADDR_GetCatCount()

========================================================================
Function: IOEMADDR_GetCategoryName()

Description:
   This method is used to get the AECHAR string name of a category.

Prototype:
   int   IOEMADDR_GetCategoryName(IOEMAddrBook * pIOEMAddrBook, AEEAddrCat c, AECHAR *pszName, int *pnSize);

Parameters:
   pIOEMAddrBook: [in]: Pointer to the IOEMAddrBook Interface object.
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
   None

Side Effects:
   None

See Also:
   IOEMADDR_GetCatCount()
   IOEMADDR_GetCatList()
   IOEMADDR_GetFieldName()

========================================================================
Function: IOEMADDR_GetFieldInfoCount()

Description:
   This function returns the number of types of fields supported for the given
   category.  If the concept of categories is not supported, the function may
   return the total number of types of fields supported for each record in the
   address book.
   Examples of fields are: NAME, WORK_NUM, FAX_NUM, URL, and ADDRESS.

Prototype:
   int   IOEMADDR_GetFieldInfoCount(IOEMAddrBook * pIOEMAddrBook, AEEAddrCat c)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

   c:  [in]:  address category whose number of supported fields is to be
   returned

Return Value:
   Number of types of fields supported for the given address category

Comments:
   None

Side Effects:
   None

See Also:
   IOEMADDR_GetFieldInfo()

========================================================================
Function: IOEMADDR_GetFieldInfo()

Description:
   This function returns detailed information about each field type supported
   for the given category. This function is generally called after the
   IOEMADDR_GetFieldInfoCount() function.

Prototype:
   int   IOEMADDR_GetFieldInfo(IOEMAddrBook * pIOEMAddrBook, AEEAddrCat c, AEEAddrFieldInfo *pf, int nSize)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

   c:  [in]:  address category for which the field information is to be returned

   pf:  [in/out]:  pointer to an array of AEEAddrFieldInfo structures (allocated
   by the caller) where information is to be returned by this function.

   nSize:  [in]:  the number of AEEAddrFieldInfo elements that can fit into the
   array pointed to by pf

Return Value:
   AEE_SUCCESS: if successful. Even if nSize is less than the total number of
   categories supported, this function must return AEE_SUCCESS as long as nSize
   > 0 (zero).

   EFAILED: if failed.

Comments:
   The AEEAddrFieldInfo structure contains detailed information about the fields
   such as FieldID and maximum number of fields of this ID supported in each
   record.
   Detailed information about this structure is in AEEAddrBook.h and the
   BREW API Reference.

Side Effects:
   None

See Also:
   IOEMADDR_GetFieldInfoCount()

========================================================================

Function: IOEMADDR_GetFieldName()

Description:
   This method is used to get the AECHAR string name of a field.

Prototype:
   static int IOEMADDR_GetFieldName(IOEMAddrBook * pIOEMAddrBook, AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

   f: [in]: AEEAddrFieldID for which the name in string format is returned

   pszName: [in/out]: On input this is the buffer where the field name will
   be placed.  If NULL, this indicates the caller is querying the size.
   required.

   pnSize: [in/out]: On input, contains the size of pszName. On return, it
   contains the size filled.

Return Value:
   AEE_SUCCESS: If successful.
   EBADPARM:    If bad parameters are passed in.
   AEE_ADDR_INVALID_FIELD_ID: If the field is not supported.

Comments:
   None

Side Effects:
   None

See Also:
   IOEMADDR_GetFieldInfoCount()
   IOEMADDR_GetFieldInfo()
   IOEMADDR_GetCategoryName()

========================================================================

Function: IOEMADDR_GetNumRecs()

Description:
   This function returns the total number of records found in the address book.

Prototype:
   uint16   IOEMADDR_GetNumRecs(IOEMAddrBook * pIOEMAddrBook);

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

Return Value:
   Count of the total number of records currently found in the address book

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================
Function: IOEMADDR_RecordAdd()

Description:
   This function adds a new record to the address book. The fields to be added
   into this record are passed as parameters to this function.

Prototype:
   uint16 IOEMADDR_RecordAdd(IOEMAddrBook * pIOEMAddrBook, AEEAddrCat cat, AEEAddrField *pItems,
                             int nItemCount, int *pErr)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

   cat:  [in]:  address category to which this record belongs. If the concept of
                categories is not supported, this parameter can be ignored.

   pItems:  [in]:  pointer to an arry of items that must be added to the record.
                   Each item contains information such as FieldID, DataType,
                   Data and DataLength.  NOTE: For detailed information about
                   this structure see the AEEAddrBook.h or BREW API Reference
                   Guide.

   nItemCount:  [in]:  number of fields in this record. It also indicates that
                       the array pItems contains this many number of fields.

   pErr:  [out]:  If an error occurs, the error code must be placed in this
                  pointer before returning from this function.  OEMs must check
                  for this parameter being NULL before storing the error value
                  in it.

Return Value:
   recordID:  if successful.  This function must return the recordID of the
              newly added record.

   AEE_ADDR_RECID_NULL:  if fails.  The parameter pErr must contain the error
                         code.  A list of error codes is in AEEError.h.  This
                         value must be returned.

Comments:
   If categories are supported, the OEM may optionally support categores that
are not explicitly defined by the OEM (categories not returned by
IOEMADDR_GetCatList()).

   The OEM may optionally support fields that are not explicitly defined by the
OEM (fields not returned by IOEMADDR_GetFieldInfo().

Side Effects:
   None

See Also:
   None

========================================================================

Function: IOEMADDR_RecordDelete()

Description:
   This function deletes a specified record from the address book.

Prototype:
   int   IOEMADDR_RecordDelete(IOEMAddrBook * pIOEMAddrBook, uint16 recID)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object
   recID:  [in]:  ID of the record to be deleted from the address book

Return Value:
   AEE_SUCCESS: if record was successfully deleted
   EFAILED: if fails

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================
Function: IOEMADDR_RecordUpdate()

Description:
   This function updates all the fields in the specified record. It replaces
   all  existing fields in that record with the fields being passed to this
   function.

Prototype:
   int  IOEMADDR_RecordUpdate(IOEMAddrBook * pIOEMAddrBook, uint16 recID, AEEAddrCat cat, AEEAddrField *pItems,
                              int nItemCount, int *pErr)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

   recID:  [in]:  ID of the record whose fields are to be updated

   cat:  [in]:  address category to which this record belongs. If the concept of
   categories is not supported, this parameter can be ignored.

   pItems:  [in]:  pointer to an arry of items that are to be added to the
   record. Each item contains information such as FieldID, DataType, Data and
   DataLength.  NOTE:  For detailed information about this structure, see the
   AEEAddrBook.h or BREW API Reference.

   nItemCount:  [in]:  number of fields in this record.  It also indicates tha
   the array pItems contains this number of fields.

   pErr:  [out]:  If an error occurs, the error code must be placed in this
   pointer before returning from this function.  OEMs must check for this
   parameter being NULL before storing the error value in it.

Return Value:
   AEE_SUCCESS: if the record is successfully deleted
   EFAILED: if fails

Comments:
   This function is similar to IOEMADDR_RecordAdd(); the main difference is that
   this function updates all of the fields in an existing record, while
   IOEMADDR_RecordAdd() adds a new record to the address book.

Side Effects:
   None

See Also:
   IOEMADDR_RecordAdd()

========================================================================
Function: IOEMADDR_RecordGetByID()

Description:
   This function retrieves the information about a specified record and returns
   information about all of the fields in this record. This function does NOT
   delete the record from the address book.

Prototype:
   int IOEMADDR_RecordGetByID(IOEMAddrBook * pIOEMAddrBook, uint16 recID, AEEAddrCat *pcat,
                              AEEAddrField **ppItems, int *pnItemCount, int *pErr)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

   recID:  [in]:  ID of the record whose fields are to be retrieved and returned

   pcat:  [in/out]:  On input, this is a valid pointer to AEEAddrCat. On return,
   this pointer points to the address category to which the record belongs.

   ppItems:  [in/out]:  pointer for passing info about the fields.  While
   implementing this function, the OEMs must allocate memory for *ppItems using
   the function MALLOC(). This memory is freed by the caller ( BREW ).

   pnItemCount:  [in/out]:  On input, this is a valid pointer to an integer. On
   return, this pointer contains the count of the number of fields present in
   this record, and indicates that the array  *ppItems contains this number of
   fields on return.

   pErr:  [out]:  If any error occurs, the error code must be placed into this
   pointer before returning from this function.  OEMs must check for this
   parameter being NULL before storing the error value in it.

Return Value:
   AEE_SUCCESS: if the record information is successfully retrieved
   EFAILED: if fails. The parameter *pErr must contain the exact error code.

Comments:
   Memory for *ppItems must be allocated by the implementer of this function.
   This memory is released by the caller.

Side Effects:
   None

See Also:
   None

========================================================================
Function: IOEMADDR_RemoveAllRecs()

Description:
   This function deletes all records from the address book.

Prototype:
   int   IOEMADDR_RemoveAllRecs(IOEMAddrBook * pIOEMAddrBook)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

Return Value:
   AEE_SUCCESS: if all records are successfully deleted
   EUNSUPPORTED: if this function is not supported
   EFAILED: if fails.

Comments:
   Since this is a sensitive operation, OEMs can decide not to support it. If
   this function is not supported, the value EUNSUPPORTED must be returned from
   this function.

Side Effects:
   None

See Also:
   None

========================================================================
Function: IOEMADDR_EnumRecInit()

Description:
   This function searches the address book for specific records, and also
   sequentially retrieves all the records in the database. This function
   initializes the enumeration of records in the address book based on a
   specific search criteria. When enumeration has been initialized, the
   function IOEMADDR_EnumNextRec() is used to iterate through the records
   that match this search criteria.

Prototype:
   int   IOEMADDR_EnumRecInit(IOEMAddrBook * pIOEMAddrBook, AEEAddrCat wCategory, AEEAddrFieldID wFieldID,
                              void *pData, uint16 wDataSize)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object.

   wCategory:  [in]:  category type to be matched. If set to AEE_ADDR_CAT_NONE,
   it is ignored.

   wFieldID:  [in]:  AEEAddrFieldID to be matched. If set to AEE_ADDRFIELD_NONE,
   it is ignored.  Typically, OEMs do not allow searching for records on this
   field ID (for example, searching for records based on EMAIL may not be
   allowed). In this case, return EFAILED and IADDRBOOK_EnumNextFieldsInfo().

   pData:  [in]:  If non-null, the actual data that must be matched.
   For example, if wFieldID is set to AEE_ADDRFIELD_NAME, pData contains the
   actual name to be matched.
   If pData is NULL, any valid address record will match.
   If wCategory is set to AEE_ADDRCAT_ENHANCED_ENUM and wFieldId is set to
   AEE_ADDRFIELD_ENHANCED_ENUM, then this field points to an AEEAddrEnumRecInfo
   structure.

   wDataSize:  [in];  Size of pData

Return Value:
   AEE_SUCCESS: if enumeration is successfully initialized.

   EFAILED: Enumeration failed.

Comments:
   This function can also be used to enumerate all records in the database by
   specifying AEE_ADDR_CAT_NONE for the category parameter and
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
   IOEMADDR_EnumNextRec()

========================================================================
Function: IOEMADDR_EnumNextRec()

Description:
   This function returns the information about the next record based on the
   search criteria specified in most recent call to IOEMADDR_EnumRecInit().

Prototype:
   uint16   IOEMADDR_EnumNextRec(IOEMAddrBook * pIOEMAddrBook, AEEAddrCat *cat, AEEAddrField **ppItems,
                                 int *nItemCount, int *pErr);

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

   cat:  [in/out]:  On return, if the next record was found, this contains the
   address category of that next record.

   ppItems:  [in/out]:  On return, if the next record was found, *ppItems
   contains the list of address fields found in that next record.

   nItemCount:  [in/out]:  On return, if the next record was found, *nItemCount
   contains the number of address fields found in that next record.

   pErr:  [in/out]:  On return, contains the error code if an error occurred.

Return Value:
   recordID: record ID of the newly found record, if the next record is
   successfully found. This function also fills up the incoming parameters with
   the contents of the newly found record.

   AEE_ADDR_RECID_NULL: if the end of the enumeration has been reached or no
   more records are found

Comments:
   When the end of the enumeration has been reached, the index must NOT be reset
   to point to the begining of the enumeration. All subsequent calls to this
   function must continue to return AEE_ADDR_RECID_NULL. The caller must call
   IOEMADDR_EnumRecInit() to re-initialize the search criteria.

Side Effects:
   None

See Also:
   IOEMADDR_EnumRecInit()

=======================================================================
Function: IOEMADDR_GetProperties()

Description:
   This function returns the properties of this IOEMAddrBook interface.

Prototype:
   uint32 IOEMADDR_GetProperties(IOEMAddrBook * pIOEMAddrBook);

Parameters:
   pIOEMAddrBook: [in]: Pointer to the IOEMAddrBook Interface object.

Return Value:
   32-bit value of properties.  See IADDRBOOK_SetProperties()in the
BREW API Reference for more information.

Comments:
   None

Side Effects:
   None

See Also:
   IOEMADDR_SetProperties()

=======================================================================
Function: IOEMADDR_SetProperties()

Description:
    This function sets the properties of this IOEMAddrBook interface.

Prototype:
    void IOEMADDR_SetPropertiesIAddrBook(IOEMAddrBook * pIOEMAddrBook, uint32 dwProps)

Parameters:
   pIOEMAddrBook: [in]: pointer to the IOEMAddrBook Interface object

   dwProps : [in] 32-bit value for properties.  See IADDRBOOK_SetProperties()
             in the BREW API Reference for more information.

Return Value:
    None

Comments:
    None

Side Effects:
   None

See Also:
    IOEMADDR_GetProperties()
===========================================================
AEEAddrBook


See the IAddrBook Interface in the BREW API Reference.


============================================================================= */

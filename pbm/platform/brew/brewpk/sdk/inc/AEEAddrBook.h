#ifndef AEEADDRBOOK_H
#define AEEADDRBOOK_H
/*======================================================
FILE:  AEEAddrBook.h

SERVICES:  BREW Address Book Interface

GENERAL DESCRIPTION:
	Interface definitions, data structures, etc. for BREW Address Book Interface

        Copyright ? 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEDB.h"

#include "ADDRBOOK_RUIM.bid"

//Address Category (Ex: Personal, Business, etc).
typedef  uint16   AEEAddrCat;


//Address Fields  (Ex: HomeNum, Address, Notes, etc).
typedef  uint16   AEEAddrFieldID;


//Pre-defined Address Categories
#define  AEE_ADDR_CAT_NONE                ((AEEAddrCat)0)
#define  AEE_ADDR_CAT_PERSONAL            (AEE_ADDR_CAT_NONE + 1)
#define  AEE_ADDR_CAT_BUSINESS            (AEE_ADDR_CAT_NONE + 2)

// Special record enumeration flag.  This field is used to indicate that
// the pData passed in an IADDRBOOK_EnumRecInit is a pointer to an enhanced
// enumeration structure.  This category ID is used only to perform record
// enumerations.  This category can not be used to store data in a record.
#define  AEE_ADDRCAT_ENHANCED_ENUM        (AEE_ADDR_CAT_NONE+0x7fff)

//Starting range for OEM defined Category IDs
#define  AEE_ADDR_CAT_USER                (AEE_ADDR_CAT_NONE+0x8000)
#ifdef CUST_EDITION	
// 记录组群属性
// VIP
#define AEE_ADDR_CAT_VIP                  (AEE_ADDR_CAT_USER+1)

// Family
#define AEE_ADDR_CAT_HOME                 (AEE_ADDR_CAT_USER+2)

// Colleague
#define AEE_ADDR_CAT_WORK                 (AEE_ADDR_CAT_USER+3)

// other
#define AEE_ADDR_CAT_OTHER                (AEE_ADDR_CAT_NONE)
// Friends
#define AEE_ADDR_CAT_FRIEND               (AEE_ADDR_CAT_USER+4)

#define AEE_ADDR_CAT_UIM                  (AEE_ADDR_CAT_SIM_ADN)
#endif /*CUST_EDITION*/
#define  AEE_ADDR_CAT_RESERVED_SIRIUS_1   AEE_ADDR_CAT_USER
#define  AEE_ADDR_CAT_RESERVED_SIRIUS_32  (AEE_ADDR_CAT_USER+31)


//This value is deprecated. Use AEE_ADDR_CAT_USER instead.
#define  AEE_ADDR_CAT_USER_DEFINED        AEE_ADDR_CAT_USER_DEPRECATED
#define  AEE_ADDR_CAT_USER_DEPRECATED     (AEE_ADDR_CAT_NONE+0xff00)

//Boundary
#define  AEE_ADDR_CAT_ERR                 ((AEEAddrCat)0xffff)



//Pre-defined Address Field IDs. OEMs can add more entries to it. 
#define  AEE_ADDRFIELD_NONE                     ((AEEAddrFieldID)0)
#define  AEE_ADDRFIELD_FIRSTNAME                (AEE_ADDRFIELD_NONE + 1)
#define  AEE_ADDRFIELD_MIDDLENAME               (AEE_ADDRFIELD_NONE + 2)
#define  AEE_ADDRFIELD_LASTNAME                 (AEE_ADDRFIELD_NONE + 3)
#define  AEE_ADDRFIELD_NICKNAME                 (AEE_ADDRFIELD_NONE + 4)
#define  AEE_ADDRFIELD_NAME                     (AEE_ADDRFIELD_NONE + 5)
#define  AEE_ADDRFIELD_ORG                      (AEE_ADDRFIELD_NONE + 6)
#define  AEE_ADDRFIELD_JOBTITLE                 (AEE_ADDRFIELD_NONE + 7)
#define  AEE_ADDRFIELD_ADDRESS                  (AEE_ADDRFIELD_NONE + 8)
#define  AEE_ADDRFIELD_CITY                     (AEE_ADDRFIELD_NONE + 9)
#define  AEE_ADDRFIELD_STATE                    (AEE_ADDRFIELD_NONE + 10)
#define  AEE_ADDRFIELD_COUNTRY                  (AEE_ADDRFIELD_NONE + 11)
#define  AEE_ADDRFIELD_ZIPCODE                  (AEE_ADDRFIELD_NONE + 12)
#define  AEE_ADDRFIELD_URL                      (AEE_ADDRFIELD_NONE + 13)
#define  AEE_ADDRFIELD_DEPARTMENT               (AEE_ADDRFIELD_NONE + 14)
#define  AEE_ADDRFIELD_EMAIL                    (AEE_ADDRFIELD_NONE + 15)
#define  AEE_ADDRFIELD_GENDER                   (AEE_ADDRFIELD_NONE + 16)
#define  AEE_ADDRFIELD_PHONE_WORK               (AEE_ADDRFIELD_NONE + 17)
#define  AEE_ADDRFIELD_PHONE_HOME               (AEE_ADDRFIELD_NONE + 18)
#define  AEE_ADDRFIELD_PHONE_FAX                (AEE_ADDRFIELD_NONE + 19)
#define  AEE_ADDRFIELD_PHONE_VOICE              (AEE_ADDRFIELD_NONE + 20)
#define  AEE_ADDRFIELD_PHONE_PREF               (AEE_ADDRFIELD_NONE + 21)
#define  AEE_ADDRFIELD_PHONE_CELL               (AEE_ADDRFIELD_NONE + 22)
#define  AEE_ADDRFIELD_PHONE_PAGER              (AEE_ADDRFIELD_NONE + 23)
#define  AEE_ADDRFIELD_PHONE_GENERIC            (AEE_ADDRFIELD_NONE + 24)
#define  AEE_ADDRFIELD_PHONE_OTHER              (AEE_ADDRFIELD_NONE + 25)
#define  AEE_ADDRFIELD_BDAY                     (AEE_ADDRFIELD_NONE + 26)

// To specify the family name or given name to be used for national-language-specific sorting of 
// address book entries.
#define  AEE_ADDRFIELD_SORTSTRING               (AEE_ADDRFIELD_NONE + 27)
#define  AEE_ADDRFIELD_NOTES                    (AEE_ADDRFIELD_NONE + 28)

#define  AEE_ADDRFIELD_SIP_URL                  (AEE_ADDRFIELD_NONE + 29)

#define  AEE_ADDRFIELD_DIAL_STRING              (AEE_ADDRFIELD_NONE + 30)
#define  AEE_ADDRFIELD_INDEX                    (AEE_ADDRFIELD_NONE + 31)
#define  AEE_ADDRFIELD_VOICETAG                 (AEE_ADDRFIELD_NONE + 32)
#define  AEE_ADDRFIELD_RINGTONE                 (AEE_ADDRFIELD_NONE + 33)
#define  AEE_ADDRFIELD_SPEED_DIAL               (AEE_ADDRFIELD_NONE + 34)

#define  AEE_ADDRFIELD_NAMESUFFIX               (AEE_ADDRFIELD_NONE + 35)
#define  AEE_ADDRFIELD_NAMEPREFIX               (AEE_ADDRFIELD_NONE + 36)
#define  AEE_ADDRFIELD_OTHERNAME                (AEE_ADDRFIELD_NONE + 37)
#define  AEE_ADDRFIELD_PHOTO                    (AEE_ADDRFIELD_NONE + 38)
#define  AEE_ADDRFIELD_PHOTOURL                 (AEE_ADDRFIELD_NONE + 39)
#define  AEE_ADDRFIELD_POBOX                    (AEE_ADDRFIELD_NONE + 40)
#define  AEE_ADDRFIELD_ADDREXTRA                (AEE_ADDRFIELD_NONE + 41)
#define  AEE_ADDRFIELD_STREET                   (AEE_ADDRFIELD_NONE + 42)
#define  AEE_ADDRFIELD_UID                      (AEE_ADDRFIELD_NONE + 43)
#define  AEE_ADDRFIELD_PUBLICKEY                (AEE_ADDRFIELD_NONE + 44)
#define  AEE_ADDRFIELD_PUBLICKEYSTRING          (AEE_ADDRFIELD_NONE + 45)

#define  AEE_ADDRFIELD_UNIQUE_ID                (AEE_ADDRFIELD_NONE + 46)

#define  AEE_ADDRFIELD_RESERVED_SIRIUS_1        (AEE_ADDRFIELD_NONE + 47)
#define  AEE_ADDRFIELD_RESERVED_SIRIUS_150      (AEE_ADDRFIELD_NONE + 196)
#define  AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE    (AEE_ADDRFIELD_NONE + 197)


// Administrative fields (not user fields)
#define  AEE_ADDRFIELD_ADMIN                    ((AEEAddrFieldID)0x7000)

// Last time this record was modified
#define  AEE_ADDRFIELD_TIME_STAMP               (AEE_ADDRFIELD_ADMIN + 1)

// Last time connection (call) made to this record
#define  AEE_ADDRFIELD_LAST_CONNECT             (AEE_ADDRFIELD_ADMIN + 2)

// Number of connections (calls) made to this record
#define  AEE_ADDRFIELD_CONNECT_COUNT            (AEE_ADDRFIELD_ADMIN + 3)

// Special record enumeration flag.  This field is used to indicate that
// the pData passed in an IADDRBOOK_EnumRecInit is a pointer to an enhanced
// enumeration structure.  This field ID is used only to perform record
// enumerations.  This field can not be used to store data in a record.
#define  AEE_ADDRFIELD_ENHANCED_ENUM            ((AEEAddrFieldID)0x7fff)

// User-defined FieldIDs begin here
#define  AEE_ADDRFIELD_USER                     (AEE_ADDRFIELD_NONE + 0x8000)

// This value is deprecated. Use AEE_ADDRFIELD_USER instead
#define  AEE_ADDRFIELD_USER_DEFINED             AEE_ADDRFIELD_USER_DEPRECATED
#define  AEE_ADDRFIELD_USER_DEPRECATED          (AEE_ADDRFIELD_NONE + 0xff00)


// Last item in list
#define  AEE_ADDRFIELD_ERR                      ((AEEAddrFieldID)0xffff)

// This macro is deprecated.  Use AEE_ADDRFIELD_ZIPCODE instead.
#define  AEE_ADDRFIELD_ZIPCODXE                 AEE_ADDRFIELD_ZIPCODE


// Properties

// Don't read in name cache when creating instance of address book. Makes
// quicker startup but records are not enumerated in sorted order.
// May be overwritten by another application
#define  ABP_NO_SORT_CACHE          0x00000001



// Record enumeration search flags
//
// Flags is a 32-bit fields assigned as follows:
// Bit     00: Case sensitivity
// Bits 01-03: Match type
// Bits 04-31: Reserved
typedef uint32 AEEAddrSearchFlags;

// When enumerating records, this flag may be set to make a case-insensitive
// comparison to the match data passed in IADDRBOOK_EnumRecInit().
#define ABSF_CASE_MASK           0x00000001
#define ABSF_CASE_SENSITIVE      0x00000000
#define ABSF_CASE_INSENSITIVE    0x00000001

// When enumerating records, one of the following flags may be set to select
// the type of comparison to the match data passed in IADDRBOOK_EnumRecInit().
#define ABSF_MATCH_MASK          0x0000000e
#define ABSF_MATCH_EQUAL         0x00000000
#define ABSF_MATCH_CONTAINS      0x00000002
#define ABSF_MATCH_BEGINS_WITH   0x00000004
#define ABSF_MATCH_ENDS_WITH     0x00000006

//Data type used for storing address-field-data
typedef AEEDBFieldType  AEEAddrFieldType;

typedef AEEDBSubfield   AEEAddrSubfield;
typedef AEEDBComplex    AEEAddrFieldComplex;

//Info about each address field supported
//Contains ID of the field and the maximum number of items of this field ID supported
//in each record. For ex: if fieldID is AEE_ADDRFIELD_EMAIL, then maxItems could be 2
//if OEM address book supports only a maximum of 2 email entries per record
typedef struct _AEEAddrFieldInfo
{
   //ID of the field
   AEEAddrFieldID       fieldID;       

   //Maximum allowed number of items of this fieldID in each record
   int                  nMaxItems;      

   //Data type used for storing info about this field
   AEEAddrFieldType     fType;
   
   //Can a search (EnumRec) be done based on this field. i.e. can we make a call to
   //IADDRBOOK_EnumRecInit() to search for all records having a specific value for this
   //given field ID. This search is independent of the category of the record. Ex: Search
   //for all records having a specific value for the field AEE_ADDRFIELD_NAME
   boolean              bFieldSearch;  

   // Can a search (EnumRec) be done based on this field within a category. i.e.
   //can we use IADDRBOOK_EnumRecInit() to search for all records of a specific
   //category  having a specific value for this field.
   //Ex: Search for all AEE_ADDR_CAT_PERSONAL records having
   //a specific value for the field AEE_ADDRFIELD_NAME. If this flag is FALSE and if
   //bFieldSearch is TRUE, then in calls to IADDRBOOK_EnumRecInit(), the value of
   //AEEAddrCat must be set to AEE_ADDR_CAT_NONE so that the category is ignored
   //during the search.
   boolean              bFieldCatSearch;
} AEEAddrFieldInfo;







//Definition of Address Field. An AddressRecord is a collection of one or more AddressFields
typedef struct
{
   AEEAddrFieldID       fID;        //Field ID (ex: WORK_PHONE, URL etc)
   AEEAddrFieldType     fType;      //Data Type (ex: FT_STRING, etc)
   void *               pBuffer;    // Data Ptr. Store it here so that it is always on a 32-bit boundary
   uint16               wDataLen;   // Data Length
} AEEAddrField;


typedef struct
{
   AEEAddrCat *       pCatList;    // Pointer to an array of categories to search.
   int                nCategories; // Number of categories in pCatList[].  0 for none in list.
   AEEAddrFieldID *   pFieldList;  // Pointer to an array of fields to search.
   int                nFields;     // Number of fields in pFieldList[].  0 for none in list.
   AEEAddrFieldType * pTypeList;   // Pointer to an array of field types to search.
   int                nTypes;      // Number of types in pTypeList[].  0 for none in list.
   void *             pData;       // Pointer to data to be matched.  Set to NULL to match all data.
   int                dwDataSize;  // Specifies the size of pData
   AEEAddrFieldType   fType;       // Data type of pData
   AEEAddrSearchFlags flags;       // Search flags to specify data match criteria
} AEEAddrEnumRecInfo;


typedef struct _IAddrBook     IAddrBook;
typedef struct _IAddrRec      IAddrRec;
#ifdef CUST_EDITION	
typedef enum
{
    NUMBERMATCH_EQUAL,                  // 两个号码完全相等
    NUMBERMATCH_WSTR1_ISTAIL_OF_WSTR2,  // 号码1是号码2的尾部
    NUMBERMATCH_WSTR2_ISTAIL_OF_WSTR1,  // 号码2是号码1的尾部
    NUMBERMATCH_IRRELEVANCE             // 两个号码不相关
} Numbermatch_e_Type;

// 号码匹配比较函数类型定义
typedef Numbermatch_e_Type (*PFN_NUMBERMATCH)(const AECHAR * wstrNum1, const AECHAR * wstrNum2, int *pMatchChars);

typedef struct _AEEAddCacheInfo   AEEAddCacheInfo;

typedef enum
{
    ADDR_STORE_DBFILE = 0,      // 手机数据库文件
    ADDR_STORE_RUIM,            // UIM
    ADDR_STORE_MAX
} Addr_Store_e_type;

struct _AEEAddCacheInfo
{
    uint16              wRecID;     // ID of the Record
    AECHAR              *szName;    // Name. If its NULL, its an untitled record
    AEEAddrCat          nCat;       // Record category id
    AECHAR              *szNumber;  // all number of record, only used by cache search   
    Addr_Store_e_type   store;      // record store type
    AEEAddCacheInfo     *pNext;
};
#endif /*CUST_EDITION*/
/*
   Address Book  Interface

*/


QINTERFACE(IAddrBook)
{
   DECLARE_IBASE(IAddrBook)

   //Query Address Categories supported
   int              (*EnumCategoryInit)(IAddrBook * po);
   boolean          (*EnumNextCategory)(IAddrBook * po, AEEAddrCat * pc);


   //Query Address Fields supported for each category
   int              (*EnumFieldsInfoInit)(IAddrBook *po,AEEAddrCat c);
   boolean          (*EnumNextFieldsInfo)(IAddrBook *po,AEEAddrFieldInfo *pf);

   //Operate on Address Records
   IAddrRec *       (*CreateRec)(IAddrBook * po, AEEAddrCat c, AEEAddrField * pItems, int nItemCount);
   uint16           (*GetNumRecs)(IAddrBook * po);  
   int              (*RemoveAllRecs)(IAddrBook* po);

   IAddrRec *       (*GetRecByID)(IAddrBook * po, uint16 wID);

   //Search Records

   //Initialize enumeration of records with given category, field name and value. 
   int              (*EnumRecInit)(IAddrBook * po,  AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize);

   //Enumerates Next address record with given category, field name and value
   IAddrRec *       (*EnumNextRec)(IAddrBook * po);
   
   //Get Last Error
   int   		     (*GetLastError)(IAddrBook *po);
   
   //Set properties
   void             (*SetProperties)(IAddrBook *po, uint32 dwProps);
   
   //Get properties
   uint32           (*GetProperties)(IAddrBook *po);

   int              (*GetCategoryName)(IAddrBook *po, AEEAddrCat c, AECHAR *pCatName, int *pnSize);
   int              (*GetFieldName)(IAddrBook *po, AEEAddrFieldID f, AECHAR *pFieldName, int *pnSize);
#ifdef CUST_EDITION	   
   int              (*EnumCacheInit)(IAddrBook * po,  AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize);
   int              (*EnumNextCache)(IAddrBook * po, void **ppCache);
   uint16           (*ExtractCache)(IAddrBook * po, void *pCache, AECHAR **ppName, AEEAddrCat *pCat);
   uint16           (*GetCapacity)(IAddrBook * po, AEEAddrCat c);
   int              (*GetCacheinfoByNumber)(IAddrBook *po, AECHAR *pwstrNum, AEEAddCacheInfo *pCacheInfo, PFN_NUMBERMATCH pfnMactch);
   int              (*CheckSameRecord)(IAddrBook  *po, AECHAR *name, boolean *exist);
#endif /*CUST_EDITION*/   
};



#define IADDRBOOK_AddRef(p)                           GET_PVTBL(p,IAddrBook)->AddRef(p)
#define IADDRBOOK_Release(p)                          GET_PVTBL(p,IAddrBook)->Release(p)
#define IADDRBOOK_EnumCategoryInit(p)                 GET_PVTBL(p,IAddrBook)->EnumCategoryInit(p)
#define IADDRBOOK_EnumNextCategory(p,pc)              GET_PVTBL(p,IAddrBook)->EnumNextCategory(p,pc)
#define IADDRBOOK_EnumFieldsInfoInit(p,c)             GET_PVTBL(p,IAddrBook)->EnumFieldsInfoInit(p,c)
#define IADDRBOOK_EnumNextFieldsInfo(p,pf)            GET_PVTBL(p,IAddrBook)->EnumNextFieldsInfo(p,pf)
#define IADDRBOOK_CreateRec(p,c,pItems,n)             GET_PVTBL(p,IAddrBook)->CreateRec(p,c,pItems,n)
#define IADDRBOOK_GetNumRecs(p)                       GET_PVTBL(p,IAddrBook)->GetNumRecs(p)
#define IADDRBOOK_RemoveAllRecs(p)                    GET_PVTBL(p,IAddrBook)->RemoveAllRecs(p)
#define IADDRBOOK_GetRecByID(p,w)                     GET_PVTBL(p,IAddrBook)->GetRecByID(p,w)
#define IADDRBOOK_EnumRecInit(p,cat,field,data,wLen)  GET_PVTBL(p,IAddrBook)->EnumRecInit(p,cat,field,data,wLen)
#define IADDRBOOK_EnumRecInitEx(p,data)               GET_PVTBL(p,IAddrBook)->EnumRecInit(p,AEE_ADDRCAT_ENHANCED_ENUM,AEE_ADDRFIELD_ENHANCED_ENUM,data,sizeof(AEEAddrEnumRecInfo))
#define IADDRBOOK_EnumNextRec(p)                      GET_PVTBL(p,IAddrBook)->EnumNextRec(p)
#define IADDRBOOK_GetLastError(p)                     GET_PVTBL(p,IAddrBook)->GetLastError(p)
#define IADDRBOOK_SetProperties(p,d)                  GET_PVTBL(p,IAddrBook)->SetProperties(p,d)
#define IADDRBOOK_GetProperties(p)                    GET_PVTBL(p,IAddrBook)->GetProperties(p)
#define IADDRBOOK_GetCategoryName(p,c,psz,pn)         GET_PVTBL(p,IAddrBook)->GetCategoryName(p,c,psz,pn)
#define IADDRBOOK_GetFieldName(p,c,psz,pn)            GET_PVTBL(p,IAddrBook)->GetFieldName(p,c,psz,pn)
#ifdef CUST_EDITION	
#define IADDRBOOK_EnumCacheInit(p,cat,field,data,wLen) GET_PVTBL(p,IAddrBook)->EnumCacheInit(p,cat,field,data,wLen)
#define IADDRBOOK_EnumNextCache(p,pCache)              GET_PVTBL(p,IAddrBook)->EnumNextCache(p,pCache)
#define IADDRBOOK_ExtractCache(p,pCache,ppName,pCat)   GET_PVTBL(p,IAddrBook)->ExtractCache(p,pCache,ppName,pCat)
#define IADDRBOOK_GetCapacity(p)                       GET_PVTBL(p,IAddrBook)->GetCapacity(p,AEE_ADDR_CAT_NONE)
#define IADDRBOOK_GetCapacityEx(p,c)                   GET_PVTBL(p,IAddrBook)->GetCapacity(p,c)
#define IADDRBOOK_GetCacheinfoByNumber(p,pNum,pInfo,pfn)        \
                  GET_PVTBL(p,IAddrBook)->GetCacheinfoByNumber(p,pNum,pInfo,pfn)
                
#define IADDRBOOK_CheckSameRecord(p,name,exist)        \
                  GET_PVTBL(p,IAddrBook)->CheckSameRecord(p,name,exist)
#endif /*CUST_EDITION*/				  

/* 
   Address Record Interface
*/


QINTERFACE(IAddrRec)
{
   DECLARE_IBASE(IAddrRec)

   //Get and set Address Categories
   AEEAddrCat            ( *GetCategory)(IAddrRec* po);
   int                   ( *SetCategory)(IAddrRec *po,AEEAddrCat cat);

   uint16                ( *GetRecID)(IAddrRec * po);

   //Access to individual fields in the record
   int                   ( *GetFieldCount)(IAddrRec *);                
   AEEAddrField *        ( *GetField)(IAddrRec * po, int nFieldIdx);

   //Update fields
   int                   ( *UpdateField)(IAddrRec *po, int nFieldIdx, AEEAddrField *);
   int                   ( *UpdateAllFields)(IAddrRec *po, AEEAddrField *pFields, int nFieldCount);

   //Add Field
   int                   ( *AddField)(IAddrRec* po, AEEAddrField * pField);

   //Remove field 
   int                   ( *RemoveField)(IAddrRec *po, int nFieldIdx);

   //Delete entire record
   int                   ( *RemoveRec)(IAddrRec * po);

   //Get Last Error
   int   		          ( *GetLastError)(IAddrRec *po);
};

#define IADDRREC_AddRef(p)                 GET_PVTBL(p,IAddrRec)->AddRef(p)
#define IADDRREC_Release(p)                GET_PVTBL(p,IAddrRec)->Release(p)
#define IADDRREC_GetCategory(p)            GET_PVTBL(p,IAddrRec)->GetCategory(p)
#define IADDRREC_SetCategory(p,cat)        GET_PVTBL(p,IAddrRec)->SetCategory(p,cat)
#define IADDRREC_GetRecID(p)               GET_PVTBL(p,IAddrRec)->GetRecID(p)
#define IADDRREC_GetFieldCount(p)          GET_PVTBL(p,IAddrRec)->GetFieldCount(p) 
#define IADDRREC_GetField(p,n)             GET_PVTBL(p,IAddrRec)->GetField(p,n) 
#define IADDRREC_UpdateField(p,n,pf)       GET_PVTBL(p,IAddrRec)->UpdateField(p,n,pf) 
#define IADDRREC_UpdateAllFields(p,pf,n)   GET_PVTBL(p,IAddrRec)->UpdateAllFields(p,pf,n) 
#define IADDRREC_AddField(p,pf)            GET_PVTBL(p,IAddrRec)->AddField(p,pf) 
#define IADDRREC_RemoveField(p,n)          GET_PVTBL(p,IAddrRec)->RemoveField(p,n) 
#define IADDRREC_RemoveRec(p)              GET_PVTBL(p,IAddrRec)->RemoveRec(p)
#define IADDRREC_GetLastError(p)           GET_PVTBL(p,IAddrRec)->GetLastError(p)



/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 
AEEAddrCat

Description:
AEEAddrCat specifies the ID of an address category.

Definition:
typedef  uint16   AEEAddrCat;

Members: 
The following is the pre-defined list of categories

AEE_ADDR_CAT_NONE          :No Category
AEE_ADDR_CAT_PERSONAL      :Personal Record
AEE_ADDR_CAT_BUSINESS      :Business Record
AEE_ADDR_CAT_USER_DEFINED  :Starting range for OEM defined Category IDs
AEE_ADDR_CAT_ERR           :Boundary Limit
 
Comments:
None

See Also:
None

=======================================================================
AEEAddrFieldID

Description:
AEEAddrFieldID specifies the ID of a field.

Definition:
typedef  uint16   AEEAddrFieldID;

Members: 

The following is the pre-defined list of field IDs

AEE_ADDRFIELD_NONE                     
AEE_ADDRFIELD_FIRSTNAME                
AEE_ADDRFIELD_MIDDLENAME               
AEE_ADDRFIELD_LASTNAME                 
AEE_ADDRFIELD_NICKNAME                 
AEE_ADDRFIELD_NAME                     
AEE_ADDRFIELD_ORG                      
AEE_ADDRFIELD_JOBTITLE                 
AEE_ADDRFIELD_ADDRESS                  
AEE_ADDRFIELD_CITY                     
AEE_ADDRFIELD_STATE                    
AEE_ADDRFIELD_COUNTRY                  
AEE_ADDRFIELD_ZIPCODXE                 
AEE_ADDRFIELD_URL                      
AEE_ADDRFIELD_DEPARTMENT               
AEE_ADDRFIELD_EMAIL                    
AEE_ADDRFIELD_GENDER                   
AEE_ADDRFIELD_PHONE_WORK               
AEE_ADDRFIELD_PHONE_HOME               
AEE_ADDRFIELD_PHONE_FAX                
AEE_ADDRFIELD_PHONE_VOICE              
AEE_ADDRFIELD_PHONE_PREF               
AEE_ADDRFIELD_PHONE_CELL               
AEE_ADDRFIELD_PHONE_PAGER              
AEE_ADDRFIELD_PHONE_GENERIC            
AEE_ADDRFIELD_PHONE_OTHER              
AEE_ADDRFIELD_BDAY                     

 To specify the family name or given name to be used for 
national-language-specific sorting of address book entries.
~
AEE_ADDRFIELD_SORTSTRING~
AEE_ADDRFIELD_NOTES     ~               

AEE_ADDRFIELD_SIP_URL
AEE_ADDRFIELD_DIAL_STRING
AEE_ADDRFIELD_INDEX
AEE_ADDRFIELD_VOICETAG
AEE_ADDRFIELD_RINGTONE
AEE_ADDRFIELD_SPEED_DIAL
AEE_ADDRFIELD_NAMESUFFIX
AEE_ADDRFIELD_NAMEPREFIX
AEE_ADDRFIELD_OTHERNAME
AEE_ADDRFIELD_PHOTO
AEE_ADDRFIELD_PHOTOURL
AEE_ADDRFIELD_POBOX
AEE_ADDRFIELD_ADDREXTRA
AEE_ADDRFIELD_STREET
AEE_ADDRFIELD_UID
AEE_ADDRFIELD_PUBLICKEY
AEE_ADDRFIELD_PUBLICKEYSTRING
AEE_ADDRFIELD_UNIQUE_ID
AEE_ADDRFIELD_RESERVED_SIRIUS_1
AEE_ADDRFIELD_RESERVED_SIRIUS_150
AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE
- This field stores two bytes of data.
- The data value of this field is the AEEAddrFieldID of the 
  phone number field that is the primary contact phone number 
  of the address record.

User-defined FieldIDs begin here~
AEE_ADDRFIELD_USER_DEFINED~      

Last item in list~
AEE_ADDRFIELD_ERR~   
*                   

Comments:
None

See Also:
None

=======================================================================
AEEAddrFieldInfo

Description:
    AEEAddrFieldInfo contains information about the fields supported in the address
book. It is used in the function IADDRBOOK_EnumNextFieldsInfo() to get detailed
information about each field supported in the address book.

Definition:
    typedef struct _AEEAddrFieldInfo {
       AEEAddrFieldID fieldID;
       int nMaxItems;
       AEEAddrFieldType fType;
       boolean bFieldSearch;
       boolean bFieldCatSearch;
    } AEEAddrFieldInfo;

Members:
    fieldID:
~
      ID of the field.
*
    nMaxItems:
~
      Maximum allowed number of items of this fieldID in each record.
*
    fType:
~
      Data type used for storing information about this field.
*
    bFieldSearch:
~
      Specifies if a search (EnumRec) can be done based on this field. 
      For example, a call can be made to IADDRBOOK_EnumRecInit() to search 
      for all records having a specific value for this given field ID.
      This search is independent of the category of the record. For example, 
      it can search for all records having a specific value for the field 
      AEE_ADDRFIELD_NAME.
*
    bFieldCatSearch:
~
      Specifies if a search (EnumRec) can be done based on this field 
      within a category. For example, can IADDRBOOK_EnumRecInit() be used 
      to search for all records of a specific category having a specific 
      value for this field. 
      A second example:
      Search for all AEE_ADDR_CAT_PERSONAL records having a specific value 
      for the field AEE_ADDRFIELD_NAME. If this flag is FALSE and if 
      bFieldSearch is TRUE, then in calls to IADDRBOOK_EnumRecInit(), the 
      value of AEEAddrCat must be set to AEE_ADDR_CAT_NONE so that the 
      category is ignored during the search.
*
Comments:
    None

See Also:
    AEEAddrFieldType
    IADDRBOOK_EnumFieldsInfoInit()
    IADDRBOOK_EnumNextFieldsInfo()

=======================================================================
AEEAddrField

Description:
    This structure contains the definition of each address field. An address 
record is a collection of one or more address fields.


Definition:

typedef struct
{
   AEEAddrFieldID       fID;        
   AEEAddrFieldType     fType;      
   void *               pBuffer;    
   uint16               wDataLen;   
} AEEAddrField;

Members:
    fieldID    :ID of the IAddrBook Interface field
    fType      :Data type used to store this field data (such as FT_STRING).
    wDataLen   :Length of the data in bytes.
    pBuffer    :Pointer to the actual data.

Comments:
None

See Also:
    AEEAddrFieldID
    AEEAddrFieldType

=============================================================================
AEEAddrFieldType

Description:
    Specifies the Field Type for a Field in the Adddress Book Record.

Definition:
    typedef AEEDBFieldType AEEAddrFieldType;

Members:
    Please see types list in AEEDBFieldType for details

Comments:
    None

See Also:
    AEEDBFieldType

=============================================================================
AEEAddrEnumRecInfo

Description:
    This structure specifies the search criteria to be used on a record
enumeration intialized using IADDRBOOK_EnumRecInitEx().

Definition:
typedef struct
{
   AEEAddrCat *       pCatList;
   int                nCategories;
   AEEAddrFieldID *   pFieldList;
   int                nFields;
   AEEAddrFieldType * pTypeList;
   int                nTypes;
   void *             pData;
   int                dwDataSize;
   AEEAddrFieldType   fType;
   AEEAddrSearchFlags flags;       // Search flags to specify data match criteria
} AEEAddrEnumRecInfo;

Members:
    pCatList : Pointer to an array of categories to search.
    nCategories : Number of categories in pCatList[].  0 for none in list.
    pFieldList : Pointer to an array of fields to search.
    nFields : Number of fields in pFieldList[].  0 for none in list.
    pTypeList : Pointer to an array of field types to search.
    nTypes : Number of types in pTypeList[].  0 for none in list.
    pData : Pointer to data to be matched.  Set to NULL to match all data.
    dwDataSize : Specifies the size of pData in bytes.
    fType : Data type of pData
    flags : Search flags to specify data match criteria

Comments:
    A record is matched if:

1) It is a category match,

  AND

2) It is a field ID match,

  AND

3) It is a data type match,

  AND

4) It is a data content match.

Where:

A category match exists when the record is in a category that is listed in
pCatList[] OR if nCategories is 0.  When nCategories is 0, all records are
considered category matches.

A field ID match exists when the record contains a field that is listed in 
pFieldList[] OR if nFields is 0.  When nFields is 0, all fields are considered
field matches.

A data type match exists when the record contains a field with a data type
listed in pTypeList[] OR if nTypes is 0.  When nTypes is 0, all data types
are considered data type matches.

A data content match exists when a field which is a category, field ID, and 
data type match, also contains data which matches pData subject to the match
criteria specified in the flags field OR if dwDataSize is 0.  When dwDataSize
is 0, all data content in a field is considered a data content match.


See Also:
    AEEAddrCat
    AEEAddrFieldID
    AEEAddrFieldType
    AEEAddrSearchFlags
    IADDRBOOK_EnumRecInitEx()

=============================================================================
AEEAddrSearchFlags

Description:
    This type specifies the criteria to be used when comparing data in a 
record enumeration.

Definition:
   typedef uint32 AEEAddrSearchFlags;

Members:
When enumerating records, one (and only one) of the flags from the group below
may be set to select the case-sensitivity of the comparison to the match 
data passed in IADDRBOOK_EnumRecInitEx().

ABSF_CASE_SENSITIVE
ABSF_CASE_INSENSITIVE

When enumerating records, one (and only one) of the flags from the group below
may be set to select the type of comparison to the match data passed 
in IADDRBOOK_EnumRecInitEx().
~
ABSF_MATCH_MASK~
ABSF_MATCH_EQUAL~
ABSF_MATCH_CONTAINS~
ABSF_MATCH_BEGINS_WITH~
ABSF_MATCH_ENDS_WITH~
*

Comments:
Use the logical OR operator to combine the search flags from the two groups.  
For example, to perform a case-insensitive search for a string anywhere in a record field
(that is, searching for "ab" would match "cab", "CaB", "absolute", and
"TABLE"):

   ABSF_CASE_INSENSITIVE | ABSF_MATCH_CONTAINS

None

See Also:
    AEEAddrEnumRecInfo
    IADDRBOOK_EnumRecInitEx()

=============================================================================
AEE_ADDR_RECID_NULL

Description:
    This constant defines a NULL record that used in the BREW Address Book interface.

Definition:
    #define AEE_ADDR_RECID_NULL 0xffff

Members:
    None

Comments:
    None

See Also:
    None

=======================================================================
  

======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 


======================================================================= 

IAddrBook Interface

This interface supports the ability to interact with the OEM Address Book, which
is the primary address book on the device. It does not support multiple address
books.

The IAddrBook Interface provides access to a collection of Address Records. An Address
Record contains data stored in a collection of address fields. Each address record
can be of a specific category (such as PERSONAL and BUSINESS). The list of categories
supported is controlled by the OEM. You can use IADDRBOOK_EnumCategoryInit() and
IADDRBOOK_EnumNextCategory() to enumerate the categories supported by the OEM.

An Address Record contains a collection of Address Fields, each of which has a FieldID
and a FieldType associated with it. The FieldIDs are indicated by the data type
AEEAddrFieldID. OEMs can add more field types of their own.

The same Field cannot contain more than one value. If an address record needs to
store more than one HOME PHONE number, then there need to be separate address fields
for each telephone number.

Similarly, if a telephone number is both a work and home number, two separate address
fields are needed to represent the HOME and WORK information:
~
AEE_ADDRFIELD_PHONE_HOME ~
AEE_ADDRFIELD_PHONE_WORK~
*

IAddrBook provides the BREW interface to process entries in a given address book.
An address book is a collection of one or more address records.

This interface offers the capability to:
~
 -    Enumerate the address categories supported.

 -    Enumerate the types of fields supported.

 -    Add new records to an address book.

 -    Search the address book based on specific fields and categories.

 -    Browse through all records in the address book.
*

The IAddrBook interface can be used to access the AddressBook on the device as well
as the address book on the RUIM Card when the devices supports the RUIM Card. To
access the Address Book on the device, the class ID AEECLSID_ADDRBOOK must be used
to create the IAddrBook interface. To access the Address Book on the RUIM Card,
the class ID AEECLSID_ADDRBOOK_RUIM must be used to create an instance of the IAddrBook
interface.

The following header file is required:~
AEEAddrBook.h

 
======================================================================= 

IADDRBOOK_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

See Also:
    IADDRBOOK_Release()

=============================================================================

IADDRBOOK_Release()

Description:
    This function is inherited from IBASE_Release(). 

See Also:
    IADDRBOOK_AddRef()

======================================================================= 
IADDRBOOK_EnumCategoryInit()

Description:
   This function initializes the enumeration of address record categories 
   supported by the address book on the device.

Prototype:
    int IADDRBOOK_EnumCategoryInit(IAddrBook * pIAddrBook)

Parameters:
    pIAddrBook :  Pointer to the IAddrBook Interface object.

Return Value:
    AEE_SUCCESS, if Enumeration initialized.

    AEE_ADDRBOOK_NOCATSUPPORT, if categories not supported.

    ENOMEMORY, if out of memory.

    EFAILED, if any general failure.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    IADDRBOOK_EnumNextCategory()
   
======================================================================= 
IADDRBOOK_EnumNextCategory()

Description:
    This function enumerates the next address record category supported by 
    the address book on the device. The enumeration must have been initialized 
    using IADDRBOOK_EnumCategoryInit() before using this function.

Prototype:
    boolean IADDRBOOK_EnumNextCategory
    (
    IAddrBook * pIAddrBook,
    AEEAddrCat * pc
    )

Parameters:
    pIAddrBook : [in] : Pointer to the IAddrBook Interface object.~
	
    pc : [out] : Contains the category information.

Return Value:
    TRUE, if the new category is found.

    FALSE, if there are no more categories to enumerate or pc is NULL.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrCat
    IADDRBOOK_EnumCategoryInit()

======================================================================= 
IADDRBOOK_EnumFieldsInfoInit()

Description:
    This function initializes the enumeration of the fields supported in 
    the address book for a given category.

Prototype:
    int IADDRBOOK_EnumFieldsInfoInit(IAddrBook * pIAddrBook, AEEAddrCat c)

Parameters:
    pIAddrBook : Pointer to the IAddrBook Interface object.~
	
    c : Specifies the category for which the field support is to be enumerated.
        If this parameter is set to AEE_ADDR_CAT_NONE, this function initializes 
        the enumeration of all the fields supported for all categories.

Return Value:
    AEE_SUCCESS, If enumeration was successfully initialized.

    AEE_ADDR_CAT_NOT_SUPPORTED, if this category is not supported.

    ENOMEMORY, if out of memory.

Comments:
    IADDRBOOK_EnumNextFieldsInfo() can be used to get information of the fields 
    supported.

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrCat
    IADDRBOOK_EnumNextFieldsInfo()

======================================================================= 
IADDRBOOK_EnumNextFieldsInfo()

Description:
    This function enumerates the next field supported in this category. 
    It returns information about the fields supported for the given category. 
    The enumeration must have been initialized using IADDRBOOK_EnumFieldsInfoInit() 
    before using this function.

Prototype:
    boolean IADDRBOOK_EnumNextFieldsInfo
    (
    IAddrBook * pIAddrBook,
    AEEAddrFieldInfo * pf
    )

Parameters:
    pIAddrBook : [in] : Pointer to the IAddrBook Interface object.~
	
    pf : [in/out] : On input, this must be a valid pointer to the AEEAddrFieldInfo
                    structure. If the return value is TRUE, this parameter contains 
                    information about the next type of field supported in this category.

Return Value:
    TRUE, if the new field is found. In this case, * pf contains information about the field.

    FALSE, if there are no more fields to enumerate.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrFieldInfo
    IADDRBOOK_EnumFieldsInfoInit()

======================================================================= 
IADDRBOOK_CreateRec()

Description:
    This function creates a new address record.

Prototype:
    IAddrRec * IADDRBOOK_CreateRec
    (
    IAddrBook * pIAddrBook,
    AEEAddrCat c,
    AEEAddrField * pItems,
    int nItemCount
    )

Parameters:
    pIAddrBook : Pointer to the IAddrBook Interface object.~
	
    c :  Category to which this address belongs. AEE_ADDR_CAT_NONE indicates "don抰
         care". Using AEE_ADDR_CAT_NONE is not recomended because the device may return 
         NULL, or add a new category, or add to an existing default category. ~
		 
    pItems :  Array of field(s) for the new address record to be put into the address book.~
	
    nItemCount :  Number of field(s) in the pItems array of fields

Return Value:
    IAddrRec pointer to the newly created address record, if successful.

    NULL, if fails. In this case, IADDRBOOK_GetLastError() may be called to get
    detailed information about the error. The possible error code is ENOMEMORY.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrCat
    AEEAddrField

======================================================================= 
IADDRBOOK_GetNumRecs()

Description:
    This function returns a count of the number of address records present 
    in this address book.

Prototype:
    int IADDRBOOK_GetNumRecs(IAddrBook * pIAddrBook)

Parameters:
    pIAddrBook :  Pointer to the IAddrBook Interface object.

Return Value:
    Number of address records present in this address book.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    None

======================================================================= 
IADDRBOOK_RemoveAllRecs()

Description:
    This function removes all records in a given address book. Because 
    this function has a huge impact on the address book, it may not be 
    supported on all devices. If this function is not supported, the value 
    EUNSUPPORTED is returned.

Prototype:
    int IADDRBOOK_RemoveAllRecs(IAddrBook * pIAddrBook)

Parameters:
    pIAddrBook :  Pointer to the IAddrBook Interface object.

Return Value:
    AEE_SUCCESS, if successful.

    EUNSUPPORTED, if this feature is not supported.

    EFAILED, if this operation failed.
Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    None
======================================================================= 

IADDRBOOK_GetRecByID()

Description:
    This function retrieves pointer to the address record whose ID is 
    specified. The Record ID of a record can be obtained using IADDRREC_GetRecID().

Prototype:
    IAddrRec * IADDRBOOK_GetRecByID(IAddrBook * pIAddrBook, uint16 wID)

Parameters:
    pIAddrBook : Pointer to the IAddrBook Interface object.~
	
    wID : ID of the record that needs to be retrieved from the address book.~

Return Value:
    IAddrRec Pointer to the address record specified by the given ID, if successful.

    NULL If function fails. In this case, IADDRBOOK_GetLastError() may be called
    to get more information about the error. The possible return code is ENOMEMORY.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    IADDRREC_GetRecID()

======================================================================= 
IADDRBOOK_EnumRecInit()

Description:
    This function initializes the enumeration of records in the address 
    book based on a specified search criteria. After enumeration has been 
    initialized, the IADDRBOOK_EnumNextRec() function can be used to iterate 
    through the records that match this search criteria.

Prototype:
    int IADDRBOOK_EnumRecInit
    (
    IAddrBook * pIAddrBook,
    AEEAddrCat wCategory,
    AEEAddrFieldID wFieldID,
    void * pData,
    uint16 wDataSize
    )

Parameters:
    pIAddrBook : Pointer to the IAddrBook Interface object.~
	
    wCategory : Category type to be matched. If this is set to AEE_ADDR_CAT_NONE, 
                it is ignored.~
				
    wFieldID :  AEEAddrFieldID to be matched. If this is set to AEE_ADDRFIELD_NONE,
                it is ignored. Before specifying a field here, it must be checked 
                if the address book permits searching based on this field. This can 
                be done using the functions IADDRBOOK_EnumFieldsInfoInit() and 
                IADDRBOOK_EnumNextFieldsInfo().~
				
    pData : If non-NULL, pData specifies the actual data that must be matched.
            For example, if wFieldID is set to AEE_ADDRFIELD_NAME, pData contains 
            the actual name to be matched. 
            If NULL, it is ignored. For example, if pData is NULL, any valid address 
            record matchs. 
			
    wDataSize : Specifies the sizeof(pData).~

    To walk through all the records in the address book~
    Use the following code -
===pre>	
    IADDRBOOK_EnumRecInit
       (
       pIAddrBook,
       AEE_ADDR_CAT_NONE,
       AEE_ADDRFIELD_NONE,
       NULL,0
       );
===/pre>

    To walk through all the records that have a specific name
    Use the following code -
===pre>	
    IADDRBOOK_EnumRecInit
       (
       pIAddrBook,
       AEE_ADDR_CAT_NONE,
       AEE_ADDRFIELD_NAME,
       aszName, size
       );
===/pre>
	   
    The parameter aszName is the AECHAR * pointer to the name to be searched for,
and size is the size of the data.

    To look up for record(s) with the specific telephone number
    Use the following code -
===pre>	
    IADDRBOOK_EnumRecInit
       (
       pIAddrBook,
       AEE_ADDR_CAT_NONE,
       AEE_ADDRFIELD_NONE,
       azNum, size
       );
===/pre>
	   
    The parameter azNum is the string representing the number, and size is the size
of the number.

Return Value:
    AEE_SUCCESS, If match found.

    EFAILED, If failed.

Comments:
    This function can also be used to enumerate through all the records in the database
by specifying AEE_ADDR_CAT_NONE for the category parameter and AEE_ADDRFIELD_NONE
for the field parameter. Wild card or parital matches are not performed. The data
must match exactly.

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrCat
    AEEAddrFieldID
    IADDRBOOK_EnumNextRec()
    IADDRBOOK_EnumFieldsInfoInit()
    IADDRBOOK_EnumNextFieldsInfo()

======================================================================= 
IADDRBOOK_EnumRecInitEx()

Description:
    This function initializes the enumeration of records in the address 
    book based on a specified search criteria. After enumeration has been 
    initialized, the IADDRBOOK_EnumNextRec() function can be used to iterate 
    through the records that match this search criteria.

Prototype:
    int IADDRBOOK_EnumRecInit
    (
    IAddrBook * pIAddrBook,
    AEEAddrEnumRecInfo * pEnumRecInfo
    )

Parameters:
    pIAddrBook : Pointer to the IAddrBook Interface object.~
				
    pEnumRecInfo : Pointer to an AEEAddrEnumRecInfo struct that specifies the
                   search criteria.~

    To walk through all the records in the address book~
	Use the following code -
===pre>	
    EnumRecInfo.nCategories = 0;
    EnumRecInfo.nFields = 0;
    EnumRecInfo.nTypes = 0;
    EnumRecInfo.dwDataSize = 0;
    IADDRBOOK_EnumRecInitEx
       (
       pIAddrBook,
       &EnumRecInfo
       );
===/pre>

Return Value:
    AEE_SUCCESS, If match found.

    EFAILED, If failed.

Comments:
    Parital matches may be performed by appropriately setting the flags 
parameter in the AEEAddrEnumRecInfo struct.

Version:
   Introduced BREW Client 3.1

See Also:
    AEEAddrCat
    AEEAddrEnumRecInfo
    AEEAddrFieldID
    IADDRBOOK_EnumNextRec()
    IADDRBOOK_EnumFieldsInfoInit()
    IADDRBOOK_EnumNextFieldsInfo()

======================================================================= 
IADDRBOOK_EnumNextRec()

Description:
    This function enumerates the next record that matches the search criteria 
    that has been specified during the previous call to IADDRBOOK_EnumRecInit(). 
    The IADDRBOOK_EnumRecInit() function must be called successfully before 
    invoking this function.

Prototype:
    IAddrRec * IADDRBOOK_EnumNextRec(IAddrBook * pIAddrBook)

Parameters:
    pIAddrBook :  Pointer to the IAddrBook Interface object.

Return Value:
    IAddrRec pointer to the matched address record, if match found.~
    NULL, if function fails.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    IADDRBOOK_EnumRecInit()

======================================================================= 
IADDRBOOK_GetLastError()

Description:
    This function returns the error that has occurred with the most recent 
    address book operation.

Prototype:
    int IADDRBOOK_GetLastError(IAddrBook * pIAddrBook)

Parameters:
    pIAddrBook :  Pointer to the IAddrBook Interface object.

Return Value:
    Most recently occurred error.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    None

======================================================================= 
IADDRBOOK_GetProperties()

Description:
    This function returns the properties of this IAddrBook interface.

Prototype:
    uint32 IADDRBOOK_GetProperties(IAddrBook * pIAddrBook);

Parameters:
    pIAddrBook :  Pointer to the IAddrBook object

Return Value:
    32-bit value of properties.

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    IADDRBOOK_SetProperties()

======================================================================= 
IADDRBOOK_SetProperties()

Description:
    This function sets the properties of this IAddrBook interface.

Prototype:
    void IADDRBOOK_SetProperties
    (IAddrBook * pIAddrBook, 
    uint32 dwProps
    )

Parameters:
    pIAddrBook :  Pointer to the IAddrBook object
	
    dwProps :  32-bit value for properties
	
               ABP_NO_SORT_CACHE 0x00000001
			   
Normally before enumerating the address book records, 
IADDRBOOK reads in all the records, caching some
information for each record, then sorts the cache so
that records are enumerated in a sorted order. If you
want to avoid the performance degradation caused by
generating this cache, set the ABP_NO_SORT_CACHE
property immediately after creating an instance of the
IAddrBook Interface and before accessing any records.
The records will be enumerated in an unsorted order.


Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    IADDRBOOK_GetProperties()

======================================================================= 
IADDRBOOK_GetCategoryName()

Description: 
   This function retrieives the text name of the specified category.  The
   returned string is placed in the buffer provided.

Prototype:

   int IADDRBOOK_GetCategoryName
   (IAddrBook *po, 
   AEEAddrCat c, 
   AECHAR *pCatName, 
   int *pnSize
   );

Parameters:
   po: [in]: Pointer to the IAddrBook object~
   
   c: [in]: Specifies the category to find the text name for.~
   
   pCatName: [in/out]: On input, this parameter must be a pointer to a valid 
            AECHAR buffer or NULL to query the size of the item. On return, 
            this buffer contains the NULL-terminated AECHAR string name 
            of the specified category.~
			
   pnSize: [in/out]: On input this parameter points to the size of the pCatName 
           buffer in bytes. On return, pnSize is filled with number of bytes 
           written to the pCatName buffer. If pCatName is NULL, pnSize is filled 
           with the number of bytes required to hold this category name.

Return Value:

   AEE_SUCCESS: If successful.

   EBADPARM: If bad parameters are passed in.

   AEE_ADDR_CAT_NOT_SUPPORTED: If the category is not supported.

   AEE_ADDRBOOK_NOCATSUPPORT: If the device does not support categories.

Comments:  
   If the pCatName buffer is too small to hold the entire category name, the name will be
   written to the end of the buffer and will be null terminated.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 
IADDRBOOK_GetFieldName()

Description: 
   This function retrieves the text name of the specified field. The
   returned string is placed in the buffer provided.

Prototype:

   int  IADDRBOOK_GetFieldName
   (IAddrBook *po, 
   AEEAddrFieldID f, 
   AECHAR *pFieldName, 
   int *pnSize);

Parameters:
   po: [in]: Pointer to the IAddrBook object.~

   f: [in]: Specifies the field to find the text name for.~

   pFieldName: [in/out]: On input this parameter must be a pointer to a valid 
               AECHAR buffer or NULL to query the size of the item. On return 
               this buffer contains the text name of the specified field.~

   pnSize: [in/out]: On input this parameter points to the size of the pFieldName 
           buffer in bytes. On return, pnSize is filled with the number of bytes 
           written to the pFieldName buffer. If pFieldName is NULL, pnSize is 
           filled with the number of bytes required to hold this field name.

Return Value:

   AEE_SUCCESS: If successful.

   EBADPARM: If bad parameters are passed in.

   AEE_ADDR_INVALID_FIELD_ID: If the field is not supported.

Comments:  
   If the pFieldName buffer is too small to hold the entire field name, the name 
   will be written up to the end of the buffer and will be null terminated.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IAddrRec Interface

IAddrRec provides the capability to access individual fields inside an address record.
It offers the capability to:
~
   -    Add new fields to a record.

   -    Update fields in an existing address record.

   -    Delete fields in an existing address record.

   -    Change the address category of a record.
*

The following header file is required:~
AEEAddrBook.h

=============================================================================

IADDRREC_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

Version:
   Introduced BREW Client 1.1

See Also:
    IADDRREC_Release()

=============================================================================

IADDRREC_Release()

Description:
    This function is inherited from IBASE_Release(). 

Version:
   Introduced BREW Client 1.1

See Also:
    IADDRREC_AddRef()

======================================================================= 
IADDRREC_GetCategory()

Description:
    This function returns the category of this address record.

Prototype:
    AEEAddrCat IADDRREC_GetCategory(IAddrRec * pIAddrRec)

Parameters:
    pIAddrRec :  Pointer to the IAddrRec Interface object.

Return Value:
    Address category of this record.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrCat
======================================================================= 
IADDRREC_SetCategory()

Description:
    This function sets the address category of this address record.

Prototype:
    int IADDRREC_SetCategory(IAddrRec * pIAddrRec, AEEAddrCat cat)

Parameters:
    pIAddrRec :  Pointer to the IAddrRec Interface object.~
    cat :  New address category.

Return Value:
    AEE_SUCCESS, If successful.

    EFAILED, If failed.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrCat

======================================================================= 
IADDRREC_GetRecID()

Description:
    This function returns the record ID of the record pointed to by this IAddrRec
Interface.

Prototype:
    uint16 IADDRREC_GetRecID(IAddrRec * pIAddrRec)

Parameters:
    pIAddrRec :  [in] :  Pointer to the IAddrRec Interface object.

Return Value:
    ID of this record.

Comments:
    None

Version:
   Introduced BREW Client 1.1

ee Also:
    None
======================================================================= 
IADDRREC_GetFieldCount()

Description:
    This function returns the count of the number of fields found in this record.

Prototype:
    int IADDRREC_GetFieldCount(IAddrRec * pIAddrRec)

Parameters:
    pIAddrRec :  Pointer to the IAddrRec Interface object.

Return Value:
    Count of the number of fields in this record.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    None

======================================================================= 

IADDRREC_GetField()

Description:
    This function retrieves information about the field at the given index. 
    The index is zero based and ranges from zero to NumFields -1 (where 
    NumFields is the number of fields in this record). The pointer returned 
    by this function must not be freed or tampered with by the caller. It is 
    automatically freed when this record is released. If modifications need 
    to be made to this field, the function IADDRREC_UpdateField() must be 
    called for the modifications to take effect.

Prototype:
    AEEAddrField * IADDRREC_GetField(IAddrRec * pIAddrRec, int nFieldIdx)

Parameters:
    pIAddrRec :  Pointer to the IAddrRec Interface object.~
    nFieldIdx :  Specifies the index of the field whose information is to be retrieved.
                 This index is 0 (zero) based.

Return Value:
    Pointer to AEEAddrField at the current index position, if successful.

    NULL, If fails.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrField

======================================================================= 
IADDRREC_UpdateField()

Description:
    This function updates the field at the given index position. The 
    index is zero based.

Prototype:
    int IADDRREC_UpdateField
    (
    IAddrRec * pIAddrRec,
    int nFieldIdx,
    AEEAddrField * pf
    )

Parameters:
    pIAddrRec : [in] : Pointer to the IAddrRec Interface object.~
    nFieldIdx : [in] : Specifies the index at which the new field is to be updated.~
    pf : [in] : Specifies the updated information for the field. This function
                replaces the existing field at this index with this new field.

Return Value:
    AEE_SUCCESS, If successful.

    ENOMEMORY, If no memory to do this operation.

    EFAILED, General failure.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrField
    IADDRREC_UpdateAllFields()
   
======================================================================= 
IADDRREC_UpdateAllFields()

Description:
    This function updates all the fields in this record. It replaces all 
    the fields in this record with the new fields passed to this function.

Prototype:
    int IADDRREC_UpdateAllFields
    (
    IAddrRec * pIAddrRec,
    AEEAddrField * pFields,
    int nFieldCount
    )

Parameters:
    pIAddrRec : Pointer to the IAddrRec Interface object.~

    pFields : Specifies the updated information for the fields. This 
              function replaces all the existing fields with these new fields.~

    nFieldCount : Specifies the number of fields in pFields array that need to
                  be added to the record.

Return Value:
    AEE_SUCCESS, If successful.

    ENOMEMORY, If no memory to do this operation.

    EFAILED, General failure.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrField
    IADDRREC_UpdateField()

======================================================================= 

IADDRREC_AddField()

Description:
    This function adds a new field to this record.

Prototype:
    int IADDRREC_AddField(IAddrRec * pIAddrRec, AEEAddrField * pField)

Parameters:
    pIAddrRec :  Pointer to the IAddrRec Interface object.~
    pField :  The new field to be added to this record.

Return Value:
    AEE_SUCCESS, If successful. 

    EFAILED, General failure.

    ENOMEMORY, Insufficient memory. 

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEAddrField
    IADDRREC_UpdateField()
    IADDRREC_RemoveField()

=======================================================================   
IADDRREC_RemoveField()

Description:
    This function removes the field at the given index position. The index 
    is zero based.

Prototype:
    int IADDRREC_RemoveField(IAddrRec * pIAddrRec, int nFieldIdx)

Parameters:
    pIAddrRec : [in] : Pointer to the IAddrRec Interface object.~
    nFieldx : [in] : Specifies the index position at which the field is to be
                     deleted.

Return Value:
    AEE_SUCCESS, If successful.

    EFAILED, General failure.

    ENOMEMORY, Insufficient memory.

Comments:
    Some fields may be required on some devices. For example, a device may 
    require that a name field always be present, or a device may require that 
    at least one number, email address, or content be present in a record.

Version:
   Introduced BREW Client 1.1

See Also:
    IADDRREC_UpdateField()
    IADDRREC_AddField()
   
=======================================================================   
IADDRREC_RemoveRec()

Description:
    This function removes the address record from the address book.

Prototype:
    int IADDRREC_RemoveRec(IAddrRec * pIAddrRec)

Parameters:
    pIAddrRec :  Pointer to the IAddrRec Interface object.

Return Value:
    AEE_SUCCESS, If successful.

    EFAILED, General failure.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    None

======================================================================= 
IADDRREC_GetLastError()
    This function returns the error that has occurred with the most recent 
    address record operation on this IAddrRec Interface.

Prototype:
    int IADDRREC_GetLastError(IAddrRec * pIAddrRec)

Parameters:
    pIAddrRec :  Pointer to the IAddrRec Interface object.

Return Value:
    Most recently occurred error.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    None


=======================================================================*/

#endif      // AEEADDRBOOK_H




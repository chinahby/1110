#ifndef AEEBTDEVDB_H
#define AEEBTDEVDB_H
/*=============================================================================
FILE:         AEEBTDevDB.h

SERVICES:     IBTDevDB Interfaces

DESCRIPTION:  IBTDevDB is the interface for accessing the Device Database kept
              by BT driver.

===============================================================================
Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#include "AEEBTCommon.h"

/*=============================================================================
 Interface ID
=============================================================================*/

#define AEEIID_BT_DEV_DB          0x01041696

/*=============================================================================
 Enumeration Criteria
=============================================================================*/
#define AEEBT_EC_ALL              0x0000 // no filtering
#define AEEBT_EC_COS              0x0001 // matching class of service only
#define AEEBT_EC_BONDED           0x0002 // matching bond status only
#define AEEBT_EC_BD_ADDR          0x0004 // matching BD address mask only
#define AEEBT_EC_VALUE_1          0x0008 // user defined
#define AEEBT_EC_VALUE_2          0x0010 // user defined
#define AEEBT_EC_VALUE_3          0x0020 // user defined
#define AEEBT_EC_VALUE_4          0x0040 // user defined

// Define a data type to refer Criteria for Find Device operation
typedef uint16     BTEnumCriteria;

/*=============================================================================
 Device Database Entry
=============================================================================*/
typedef struct
{
   boolean            bBonded;
   boolean            bConnected;
   BTSecurityLevel    security;
   uint8              szNickName[ AEEBT_MAX_LEN_NICK_NAME +1 ];
   uint16             u16Value1;
   uint16             u16Value2;
   uint16             u16Value3;
   uint16             u16Value4;
   BTDeviceInfo       deviceInfo;

} BTDBEntry;

/*=============================================================================
 Device Database Enumerator
=============================================================================*/
typedef struct
{
   BTEnumCriteria     what2match;       // criteria to match
   BTClassOfService   cos;              // valid if control == COS
   boolean            bBonded;          // valid if control == BONDED
   BDAddress          bdAddrMask;       // valid if control == BD_ADDR_MASK
   BDAddress          bdAddrValue;      // valid if control == BD_ADDR_MASK
   uint16             u16Value1;        // valid if control == VALUE_1
   uint16             u16Value2;        // valid if control == VALUE_2
   uint16             u16Value3;        // valid if control == VALUE_3
   uint16             u16Value4;        // valid if control == VALUE_4
}  BTDevDBEnumerator;

/*=============================================================================
 Interface Definition
=============================================================================*/
typedef struct IBTDevDB IBTDevDB;

AEEINTERFACE(IBTDevDB)
{
   INHERIT_IQueryInterface(IBTDevDB);

   BTResult (*Add)      (IBTDevDB*                 po,
                         const BTDeviceInfo*       pDeviceInfo);
   BTResult (*Read)     (IBTDevDB*                 po,
                         BTDBEntry*                pEntry);
   BTResult (*Update)   (IBTDevDB*                 po,
                         BTModDevField             fields2Update,
                         const BTDBEntry*          pEntry
                        );
   BTResult (*EnumInit) (IBTDevDB*                 po,
                         const BTDevDBEnumerator*  pEnumerator);
   BTResult (*EnumNext) (IBTDevDB*                 po,
                         BTDBEntry*                pEntry);
   int (*Bond)          (IBTDevDB*                 po,
                         const BDAddress*          pBDAddr,
                         const uint8*              pszPin,
                         boolean                   bInitiating,
                         BTResult*                 pResult,
                         AEECallback*              pResultCb);
};


static __inline void* AEEDEVDB_MEMMOVE(void* pTo, const void* pFrom, unsigned int nLen)
{
   char* pDest = (char *)pTo;
   const char* pSrc = (const char *)pFrom;

   if (nLen == 0 || pTo == pFrom)
   {
      return pTo;
   }

   if (pSrc > pDest)
   {
      for (; nLen > 0; nLen--)
      {
         *pDest++ = *pSrc++;
      }
   }
   else
   {
      for (pDest = pDest + nLen, pSrc = pSrc + nLen; nLen > 0; nLen--)
      {
         *--pDest = *--pSrc;
      }
   }
   return pTo;
}


static __inline int AEEDEVDB_STRLEN(const char* s)
{
   const char *sEnd = s;

   if (!*s)
   {
      return 0;
   }

   while (*++sEnd);

   return sEnd - s;
}



static __inline uint32 IBTDEVDB_AddRef (IBTDevDB* p)
{
   return AEEGETPVTBL((p),IBTDevDB)->AddRef((p));
}

static __inline uint32 IBTDEVDB_Release (IBTDevDB* p)
{
   return AEEGETPVTBL((p),IBTDevDB)->Release((p));
}

static __inline int IBTDEVDB_QueryInterface (IBTDevDB* p,
                                                  AEECLSID clsid,
                                                  void **ppo )
{
   return AEEGETPVTBL((p),IBTDevDB)->QueryInterface((p),(clsid),(ppo));
}

static __inline BTResult IBTDEVDB_Add (IBTDevDB* p,
                                       const BTDeviceInfo* pd)
{
   return AEEGETPVTBL((p),IBTDevDB)->Add((p), (pd));
}

static __inline BTResult IBTDEVDB_Read (IBTDevDB* p,
                                        BTDBEntry* pEntry)
{
   return AEEGETPVTBL((p),IBTDevDB)->Read((p),(pEntry));
}

static __inline BTResult IBTDEVDB_EnumInit (IBTDevDB* p,
                                            const BTDevDBEnumerator* pec)
{
   return AEEGETPVTBL((p),IBTDevDB)->EnumInit((p), (pec));
}

static __inline BTResult IBTDEVDB_EnumNext (IBTDevDB* p,
                                            BTDBEntry* pEntry)
{
   return AEEGETPVTBL((p),IBTDevDB)->EnumNext((p),(pEntry));
}

static __inline int IBTDEVDB_Bond ( IBTDevDB* p,
                                         const BDAddress* pBDAddr,
                                         const uint8* pszPin,
                                         boolean bReplying,
                                         BTResult* pResult,
                                         AEECallback* pResultCb)
{
   return AEEGETPVTBL((p),IBTDevDB)->Bond((p),
                                          (pBDAddr),
                                          (pszPin),
                                          (bReplying),
                                          (pResult),
                                          (pResultCb));
}

static __inline BTResult IBTDEVDB_SetUserValue(IBTDevDB* p,
                                               const BDAddress* pBDAddr,
                                               BTModDevField modDevField,
                                               uint16 value)
{
   BTDBEntry entry;

   if ( NULL == pBDAddr )
   {
      return AEEBT_RESULT_BAD_PARAM;
   }
   switch ( modDevField )
   {
   case AEEBT_MDF_VAL1_B:        
      entry.u16Value1 = value;
      break;
   case AEEBT_MDF_VAL2_B:        
      entry.u16Value2 = value;
      break;
   case AEEBT_MDF_VAL3_B:        
      entry.u16Value3 = value;
      break;
   case AEEBT_MDF_VAL4_B:        
      entry.u16Value4 = value;
      break;
   default:
      return AEEBT_RESULT_BAD_PARAM;    
   }

   entry.deviceInfo.bdAddr = *((BDAddress*)pBDAddr);
   return AEEGETPVTBL((p),IBTDevDB)->Update((p),modDevField,&entry);
}

static __inline BTResult IBTDEVDB_SetNickName(IBTDevDB* p,
                                              const BDAddress* pBDAddr,
                                              const uint8* szName)
{
   BTDBEntry entry;
   uint8 len;

   if ( (NULL == pBDAddr) || (NULL == szName) )
   {
      return AEEBT_RESULT_BAD_PARAM;
   }
   entry.deviceInfo.bdAddr = *pBDAddr;

   len = AEEDEVDB_STRLEN((const char *)szName);

   if ( len  > AEEBT_MAX_LEN_NICK_NAME )
   {
      len = AEEBT_MAX_LEN_NICK_NAME;
   }
   AEEDEVDB_MEMMOVE((void*)entry.szNickName,(const void*)szName,len);

   entry.szNickName[len] = 0;

   return AEEGETPVTBL((p),IBTDevDB)->Update((p),AEEBT_MDF_NICKNAME_B,&entry);
}

static __inline BTResult IBTDEVDB_Unbond(IBTDevDB* p, const BDAddress* pBDAddr)
{
   BTDBEntry entry;

   if ( NULL == pBDAddr )
   {
      return AEEBT_RESULT_BAD_PARAM;
   }
   entry.deviceInfo.bdAddr = *pBDAddr;

   return AEEGETPVTBL((p),IBTDevDB)->Update((p),AEEBT_MDF_BONDED_B,&entry);
}

static __inline BTResult IBTDEVDB_SetSecurity(IBTDevDB*        p, 
                                              const BDAddress* pBDAddr,
                                              BTSecurityLevel  sec)
{
   BTDBEntry entry;
   entry.security          = sec;

   if (NULL == pBDAddr)
   {
      return AEEBT_RESULT_BAD_PARAM;
   }
   entry.deviceInfo.bdAddr = *pBDAddr;

   return AEEGETPVTBL((p),IBTDevDB)->Update((p),AEEBT_MDF_SEC_B,&entry);
}

static __inline BTResult IBTDEVDB_Remove(IBTDevDB* p, const BDAddress* pBDAddr)
{
   BTDBEntry entry;

   if (NULL == pBDAddr)
   {
      return AEEBT_RESULT_BAD_PARAM;
   }
   entry.deviceInfo.bdAddr = *pBDAddr;

   return AEEGETPVTBL((p),IBTDevDB)->Update((p),AEEBT_MDF_VALID_B,&entry);
}


/*=============================================================================
DATA STRUCTURE DOCUMENTATION
===============================================================================
BTDBEntry

Description:
   Specifies the information contained in each entry of the device DB 
   maintained by the BT driver.

typedef struct
{
   boolean            bBonded;
   boolean            bConnected;
   BTSecurityLevel    security;
   uint8              szNickName[ AEEBT_MAX_NICKNAME_LEN+1 ];
   uint16             u16Value1;
   uint16             u16Value2;
   uint16             u16Value3;
   uint16             u16Value4;
   BTDeviceInfo       deviceInfo;
   
} BTDBEntry;

Members:
   bBonded        : Flag indicates if the remote device is bonded or not
   bConnected     : Flag indicates if the remote device is connected or not
   security       : Security level of the connection
   szNickName     : Nick name of the remote device
   u16Value1      : User defined value 1
   u16Value2      : User defined value 2
   u16Value3      : User defined value 3
   u16Value4      : User defined value 4  
   deviceInfo     : Device information

See Also:   
   BTSecurityLevel
   BTDeviceInfo
===============================================================================
BTDevDBEnumerator

Description:
   Specifies the information used in enumerating through the device DB.
   
Definition:
typedef struct
{
   BTEnumCriteria      what2match;       
   BTClassOfService    cos;              
   boolean             bBonded;          
   BDAddress           bdAddrMask;       
   BDAddress           bdAddrValue;      
   uint16              u16Value1;        
   uint16              u16Value2;        
   uint16              u16Value3;        
   uint16              u16Value4;        
}  BTDevDBEnumerator;

Members:
   what2match     : criteria to match
   cos            : valid if control == COS 
   bBonded        : valid if control == BONDED
   bdAddrMask     : valid if control == BD_ADDR_MASK
   bdAddrValue    : valid if control == BD_ADDR_MASK
   u16Value1      : valid if control == VALUE_1
   u16Value2      : valid if control == VALUE_2
   u16Value3      : valid if control == VALUE_3
   u16Value4      : valid if control == VALUE_4
   
   

Example:
   To find all bonded devices whose BD addresses end with 1234:
   what2match  : AEEBT_EC_BONDED | AEEBT_EC_BD_ADDR
   bBonded     : TRUE
   bdAddrMask  : 00 00 00 00 FF FF
   bdAddrValue : 00 00 00 00 12 34

Comments:
   None

See Also:
   BTEnumCriteria
   BTClassOfService
   BDAddress
===============================================================================
BTEnumCriteria

Description:
   Specifies the criteria for enumerating through the Device DB.  User of this
   interface can enumerate through the device DB to find all devices matching
   the criteria specified by this type.

Definition:
   AEEBT_EC_ALL              0x0000 : no filtering
   AEEBT_EC_COS              0x0001 : matching class of service only
   AEEBT_EC_BONDED           0x0002 : matching bond status only
   AEEBT_EC_BD_ADDR          0x0004 : matching BD address mask only
   AEEBT_EC_VALUE_1          0x0008 : user defined
   AEEBT_EC_VALUE_2          0x0010 : user defined
   AEEBT_EC_VALUE_3          0x0020 : user defined
   AEEBT_EC_VALUE_4          0x0040 : user defined

Comments:
   None

See Also:
   BTDevDBEnumerator
   
=============================================================================*/

/*=============================================================================
INTERFACE DOCUMENTATION
===============================================================================
IBTDevDB Interface

Description:
   This interface provides access to the Device Database kept by BT driver.
   User of this interface can add a new entry to the DB, remove an existing
   entry from the DB, retrieve (or read) a particular entry in the DB, update
   certain fields of an entry in the DB, or enumerate through the DB based
   on some criteria.  Note that a new entry in the Device Database will be 
   automatically created by BT driver when the local device is bonded to 
   another device.

===============================================================================
IBTDEVDB_AddRef

Description:
	This function is inherited from IBASE_AddRef().

See Also:
   IBTDEVDB_Release()
   
===============================================================================
IBTDEVDB_Release

Description:
	This function is inherited from IBASE_Release().

See Also:
   IBTDEVDB_AddRef()
   
===============================================================================
IBTDEVDB_QueryInterface

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IBTDevDB-derived class

Prototype:
   int IBTDEVDB_QueryInterface (IBTDevDB*  po, 
                                AEECLSID   clsReq, 
                                void**     ppo);

Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : Pointer to the interface.

Return Value:
   SUCCESS: on success, 
   Otherwise: an error code.

Comments:
   If ppo is back a NULL pointer, the queried interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

===============================================================================
IBTDEVDB_Add()
   
Description:
   This method adds the given device information to the Device DB.
   
Prototype:
   BTResult IBTDEVDB_Add(IBTDevDB* po, const BTDeviceInfo* pDeviceInfo);

Parameters:
   po [in]         : the object
   pDeviceInfo[in] : the device information to add to DB

Return values:
   AEEBT_RESULT_SUCCESS         : entry with given data added to DB
   AEEBT_RESULT_BAD_PARAM       : input pointers are invalid
   AEEBT_RESULT_NO_RESOURCE     : no resource to perform this operation
   AEEBT_RESULT_DB_FULL         : no more room in DB
   AEEBT_RESULT_ENTRY_EXISTS    : entry already exists

See Also:
   BTDeviceInfo
===============================================================================
IBTDEVDB_Remove()
   
Description:
   This method removes from the Device DB the entry with matching BD address.
   The entry is freed up, but not erased.
   
Prototype:
   BTResult IBTDEVDB_Remove(IBTDevDB* po, const BDAddress* pBDAddr);

Parameters:
   po [in]        : the object
   pBDAddr[in]    : the BD address to match

Return values:
   AEEBT_RESULT_SUCCESS            : entry with given BD addr removed
   AEEBT_RESULT_BAD_PARAM          : input pointers are invalid
   AEEBT_RESULT_NO_RESOURCE        : no resource to perform this operation
   AEEBT_RESULT_ENTRY_NOT_FOUND    : entry already exists

See Also:
   BDAddress
===============================================================================
IBTDEVDB_Read()
   
Description:
   This method reads from the Device DB the entry whose BD address is specified.
   
Prototype:
   BTResult IBTDEVDB_Read(IBTDevDB* po, BTDBEntry* pEntry);

Parameters:
   po [in]     : the object
   pEntry[i/o] : [in] the BD address to match
				 [out] the entire entry for that BD address

Return values:
   AEEBT_RESULT_SUCCESS            : entry with given BD address copied to destination
   AEEBT_RESULT_BAD_PARAM          : input pointers are invalid
   AEEBT_RESULT_NO_RESOURCE        : no resource to perform this operation
   AEEBT_RESULT_ENTRY_NOT_FOUND    : entry does not exists

See Also:
   BTDBEntry
===============================================================================
IBTDEVDB_EnumInit()
   
Description:
   This method initializes the Device DB enumerator.

Prototype:
   BTResult IBTDEVDB_EnumInit(IBTDevDB*                 po,
                              const BTDevDBEnumerator*  pEnumerator);

Parameters:
   po [in]          : the object
   pEnumerator[in]  : the criteria and data used in enumerating the DB

Return values:
   AEEBT_RESULT_SUCCESS            : enumerator initialized
   AEEBT_RESULT_BAD_PARAM          : input pointers are invalid

See Also:
   IBTDEVDB_EnumNext()
   BTDevDBEnumerator

===============================================================================
IBTDEVDB_EnumNext()
   
Description:
   This method enumerates through the Device DB to search for entries matching
   certain criteria.

Prototype:
   BTResult IBTDEVDB_EnumNext(IBTDevDB*      po,
                              BTDeviceInfo*  pDeviceInfo);

Parameters:
   po [in]          : the object
   pDeviceInfo[out] : data for the next entry matching search criteria

Return values:
   AEEBT_RESULT_SUCCESS            : entry that matches the given
                                     criteria was found
   AEEBT_RESULT_BAD_PARAM          : input pointers are invalid
   AEEBT_RESULT_NOT_INITIALIZED    : enumerator not initialized
   AEEBT_RESULT_ENTRY_NOT_FOUND    : no more entries that match the given criteria

See Also:
   IBTDEVDB_EnumInit()
   BTDeviceInfo
   
==================================================================================
IBTDEVDB_Bond()

Description:
   This method enables application to send the PIN code to BT driver to be
   used for authenticating the remote device.  The bBonded field of the entry
   for the device with the given address will be set to TRUE if bonding is
   successful.
   
Prototype:
   int IBTDEVDB_Bond(IBTDevDB*         po,
                     const BDAddress*  pBDAddr,
                     const uint8*      pPin,
                     boolean           bInitiating,
                     BTResult*         pResult,
                     AEECallback*      pResultCb);

Parameters:
   po [i]          : the object
   pBDAddr [i]     : BD address of remote device
   pPin [i]        : the PIN code, in UTF-8 format
   bInitiating [i] : TRUE to indicate local device initiates bonding process
   pResult [o]     : bonding result
   pResultCb       : called to deliver the result
                    
Return values:
   SUCCESS  : pending, just wait for pResultCb to be called                    
   EBADPARM : one or more input pointers are invalid

See Also:
   BDAddress
   BTResult 
==================================================================================
IBTDEVDB_Unbond()

Description:
   This method enables application to mark a given device as not bonded.
   
Prototype:
   BTResult IBTDEVDB_Unbond(IBTDevDB* po, const BDAddress* pBDAddr);

Parameters:
   po [i]          : the object
   pBDAddr [i]     : BD address of remote device
                    
Return values:
   AEEBT_RESULT_SUCCESS         : entry with given BD addr marked as unbonded
   AEEBT_RESULT_BAD_PARAM       : input pointers are invalid
   AEEBT_RESULT_NO_RESOURCE     : no resource to perform this operation
   AEEBT_RESULT_ENTRY_NOT_FOUND : entry does not exist
   AEEBT_RESULT_DEV_NOT_BONDED  : entry exists but device not bonded

See Also:
   IBTDEVDB_Bond()
   BDAddress
==================================================================================
IBTDEVDB_SetSecurity()

Description:
   This method updates the security setting for a particular device.
   
Prototype:
   BTResult IBTDEVDB_SetSecurity(IBTDevDB*         po,
                                 const BDAddress*  pBDAddr,
                                 BTSecurityLevel   sec);

Parameters:
   po [i]          : the object
   pBDAddr [i]     : BD address of remote device
   sec [i]         : new security setting
                    
Return values:
   AEEBT_RESULT_SUCCESS            : entry with given BD addr updated 
   AEEBT_RESULT_BAD_PARAM          : input pointers are invalid
   AEEBT_RESULT_NO_RESOURCE        : no resource to perform this operation
   AEEBT_RESULT_ENTRY_NOT_FOUND    : entry does not exist

See Also:
   BTSecurityLevel
   BDAddress
==========================================================================
IBTDEVDB_SetUserValue()

Description:
   This method updates the User defined values 1- 4 for a particular device.

   
Prototype:
   BTResult IBTDEVDB_SetUserValue(IBTDevDB* po,
                                  const BDAddress* pBDAddr,  
                                  BTModDevField modDevField,  
                                  uint16  uValue);


Parameters:
   po [i]              : the object
   pBDAddr [i]         : BD address of remote device
   modDevField [i]     : AEEBT_MDF_VAL1_B/ AEEBT_MDF_VAL2_B/ 
                         AEEBT_MDF_VAL3_B/ AEEBT_MDF_VAL4_B
   uValue [i]          : User defined value to update for the Device.


                    
Return values:
   AEEBT_RESULT_SUCCESS          : entry with given BD address updated 
   AEEBT_RESULT_BAD_PARAM        : input pointers are invalid
   AEEBT_RESULT_NO_RESOURCE      : no resource to perform this operation
   AEEBT_RESULT_ENTRY_NOT_FOUND  : entry does not exist

See Also:
   BDAddress
   BTModDevField
==========================================================================
IBTDEVDB_SetNickName()

Description:
   This method updates the NickName for a particular 
   device.
   
Prototype:
   BTResult IBTDEVDB_SetNickName(IBTDevDB* p,
                                 const BDAddress* pBDAddr,  
                                 const uint8* szName);


Parameters:
   po [i]            : the object
   pBDAddr [i]       : BD address of remote device
   szName [i]        : Nickname in UTF-8 format, whose length should
                       not exceed  AEEBT_MAX_LEN_NICK_NAME


                    
Return values:
   AEEBT_RESULT_SUCCESS          : entry with given BD addr updated 
   AEEBT_RESULT_BAD_PARAM        : input pointers are invalid
   AEEBT_RESULT_NO_RESOURCE      : no resource to perform this operation
   AEEBT_RESULT_ENTRY_NOT_FOUND  : entry does not exist

See Also:
   BDAddress
==========================================================================*/

#endif // AEEBTDEVDB_H


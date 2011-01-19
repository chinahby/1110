#ifndef AEEIADDRINFO_H
#define AEEIADDRINFO_H

/*=============================================================================

FILE:         AEEIAddrInfo.h

SERVICES:     IAddrInfo interface

DESCRIPTION:  

===============================================================================
        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEIQI.h"
#include "AEESocketTypes.h"
#include "AEECallback.h"

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

// GetAddrInfo flags
#define AEE_AI_FLAG_PASSIVE      0x0001   // socket address will be used in bind() call.
#define AEE_AI_FLAG_CANONNAME    0x0002   // return canonical name in first szCanonName.
#define AEE_AI_FLAG_NUMERICHOST  0x0004   // nodename must be a numeric address string.
#define AEE_AI_FLAG_NUMERICSERV  0x0008   // servname must be a numeric port string.
#define AEE_AI_FLAG_V4MAPPED     0x0010   // return IPv4-mapped IPv6 addresses
                                          // on finding no matching IPv6 addresses.
#define AEE_AI_FLAG_ALL          0x0020   // when used with AEE_AI_FLAG_V4MAPPED, then
                                          // return all matching IPv6 and IPv4 addresses.
#define AEE_AI_FLAG_ADDRCONFIG   0x0040   // return only addresses that match the 
                                          // configuration of the device.

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

typedef struct AEEAddrInfo {
   int  dwFlags;           // AEE_AI_FLAG_*
   int  nFamily;           // AEE_AF_UNSPEC, AEE_AF_INET, etc.
   AEESockType nSockType;  // AEE_SOCK_UNSPEC or AEE_SOCK_STREAM, etc.
   int  nProtocol;         // AEE_IPPROTO_UNSPEC or AEE_IPPROTO_* for IPv4 and IPv6
   uint32 dwAddrLen;       // length of psasAddr
   char* szCanonName;      // canonical name for nodename
   union AEESockAddrStorage* psasAddr;    // binary address.
} AEEAddrInfo;


//*********************************************************************************************************
//
// IAddrInfo Interface
//
//*********************************************************************************************************

#define AEEIID_IAddrInfo    0x01036e17

#define INHERIT_IAddrInfo(iname) \
   INHERIT_IQI(iname); \
   int        (*SelectNetwork)(iname* po, int nNetwork); \
   int        (*StartQuery)(iname* po, const char *szNodeName, const char *szServName, \
                             const AEEAddrInfo *paiHints, AEECallback *pcb); \
   int        (*GetResult)(iname* po, int* pnStatus, int* pnAddrNumber, int* pnMaxAddrInfoByteSize); \
   int        (*GetResultAddr)(iname* po, int nAddrIndex, AEEAddrInfo* pai, int* pnAddrInfoBytes); \
   int        (*ExtFunc)(iname* po, AEECLSID id, void *pBuf, int nBufSize) \
              /* Extension Function to allow future enhancement to the interface without breaking its binary compatibility */ \

AEEINTERFACE_DEFINE(IAddrInfo);


//////////////////////
// IAddrInfo methods
//////////////////////

static __inline uint32 IAddrInfo_AddRef(IAddrInfo *me)
{
   return AEEGETPVTBL(me,IAddrInfo)->AddRef(me);
}

static __inline uint32 IAddrInfo_Release(IAddrInfo *me)
{
   return AEEGETPVTBL(me,IAddrInfo)->Release(me);
}

static __inline int IAddrInfo_QueryInterface(IAddrInfo *me, 
                                             AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IAddrInfo)->QueryInterface(me, cls, ppo);
}

static __inline int IAddrInfo_SelectNetwork(IAddrInfo *me, int nNetwork)
{
   return AEEGETPVTBL(me,IAddrInfo)->SelectNetwork(me, nNetwork);
}

static __inline int IAddrInfo_StartQuery(IAddrInfo *me, const char *szNodeName,
                                         const char *szServName, const AEEAddrInfo *paiHints,
                                         AEECallback *pcb)
{
   return AEEGETPVTBL(me,IAddrInfo)->StartQuery(me, szNodeName, szServName, paiHints, pcb);
}

static __inline int IAddrInfo_GetResult(IAddrInfo *me, int* pnStatus, int* pnAddrNumber, int* pnMaxAddrInfoByteSize)
{
   return AEEGETPVTBL(me,IAddrInfo)->GetResult(me, pnStatus, pnAddrNumber, pnMaxAddrInfoByteSize);
}

static __inline int IAddrInfo_GetResultAddr(IAddrInfo *me, int nAddrIndex, AEEAddrInfo* pai, int* pnAddrInfoBytes)
{
   return AEEGETPVTBL(me,IAddrInfo)->GetResultAddr(me, nAddrIndex, pai, pnAddrInfoBytes);
}

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

Address Info Flags

Description:
   This data type defines the flags that can be used to change the default 
   actions of an Address Info query.

Definition:
   #define AEE_AI_FLAG_PASSIVE      0x0001
   #define AEE_AI_FLAG_CANONNAME    0x0002
   #define AEE_AI_FLAG_NUMERICHOST  0x0004
   #define AEE_AI_FLAG_NUMERICSERV  0x0008
   #define AEE_AI_FLAG_V4MAPPED     0x0010
   #define AEE_AI_FLAG_ALL          0x0020
   #define AEE_AI_FLAG_ADDRCONFIG   0x0040

Members:
   AEE_AI_FLAG_PASSIVE:
      the returned socket address will be used for accepting incoming 
      connections (i.e. in bind()) instead of connect().
   AEE_AI_FLAG_CANONNAME:
      return canonical name in first szCanonName.
   AEE_AI_FLAG_NUMERICHOST:
      nodename must be a numeric address string.
   AEE_AI_FLAG_NUMERICSERV:
      servname must be a numeric port string.
   AEE_AI_FLAG_V4MAPPED:
      return IPv4-mapped IPv6 addresses on finding no matching IPv6 addresses.      
   AEE_AI_FLAG_ALL:
      when used with AEE_AI_FLAG_V4MAPPED, then return all matching IPv6 and 
      IPv4 addresses.
   AEE_AI_FLAG_ADDRCONFIG:
      return only addresses that match the configuration of the device.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IAddrInfo_StartQuery()

=============================================================================

AEEAddrInfo

Description:
   This data type represents a socket address information element to be used
   with IAddrInfo_StartQuery() and IAddrInfo_GetResultAddr().
   See RFC 3493 section 6.1 for more details. The main exception is that the
   ai_next member was dropped and that psasAddr and szCanonName are typically
   stored in the same contiguous memory block as the AEEAddrInfo structure itself.

===H2>
   Usage example:
===/H2>
      nErr = IAddrInfo_GetResult(pMe->pIAddrInfo, &nStatus, &nAddrNumber, &nMaxAddrInfoByteSize); ~
      if (SUCCESS != nErr) { ... } ~

      IEnv_ErrMalloc(piEnv, nMaxAddrInfoByteSize, (void**)(&pAddr));
      if (!pAddr) { ... } ~

      nErr = IAddrInfo_GetResultAddr(pMe->pIAddrInfo, 0, pAddr, &nMaxAddrInfoByteSize); ~
      if (SUCCESS != nErr) { ... } ~

Definition:
   typedef struct AEEAddrInfo {
      int  dwFlags;
      int  nFamily;
      AEESockType nSockType;
      int  nProtocol;
      uint32 dwAddrLen;
      char* szCanonName;
      union AEESockAddrStorage* psasAddr;
   } AEEAddrInfo;

Members:
   dwFlags: bit mask integer of AEE_AI_FLAG_*. Used when passed as the hints
            structure to IAddrInfo_StartQuery().
   nFamily: socket address family (AEE_AF_*).
   nSockType: socket type.
   nProtocol: socket protocol (AEE_IPPROTO_*)
   dwAddrLen: length of psasAddr
   szCanonName: canonical name for nodename.
   psasAddr: binary address.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5


See Also:
   IAddrInfo Interface
   IAddrInfo_StartQuery()
   Address Info Flags
   Socket Address Family
   AEESockType

=============================================================================
*/

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IAddrInfo Interface

Description:
   This interface provides host name resolving functionality, allowing the
   retrieving of IP addresses associated with a specified host name.

   This interface attempts to follow the spirit of RFC 3493 section 6.1
   "Protocol-Independent Nodename and Service Name Translation".

   The main exception is that instead of getaddrinfo() and freeaddrinfo() APIs
   IAddrInfo encapsulates a single resolve transaction and thus exposes a 
   somewhat different API.
   getaddrinfo() approximately corresponds to calling StartQuery(), 
   GetResult() and GetResultAddr().
   freeaddrinfo() corresponds to releasing the IAddrInfo instance.

   Another difference is in error values: 
   There are no EAI_NONAME and EAI_AGAIN errors. 
   Errors AEE_NET_EAI_BADREQUEST, AEE_NET_UNKDOMAIN, AEE_NET_BADDOMAIN and
   AEE_NET_BADRESPONSE are roughly equivalent to EAI_NONAME.
   AEE_NET_ETIMEDOUT is roughly equivalent to EGAI_AGAIN.

   There is also a difference in the addrinfo structure. Please see 
   AEEAddrInfo documentation.

   IAddrInfo_StartQuery() is used for setting the address information 
   data for the resolving query and for starting the resolving transaction.
   An AEECallback is used to inform the client when the transaction has completed.

   IAddrInfo_GetResult() is used for retrieving the transaction results, and 
   GetResultAddr() is used to retrieve a specific address in the case of a 
   successful transaction.

   An IAddrInfo instance only supports a single transaction. 
   This means that after transaction start or end, the instance can not be used
   to start a new transaction.
   For each transaction a different IAddrInfo instance is required.

===H2>
   Usage example:
===/H2>

   @@@b> Initiating a query: @@@/b>

      CALLBACK_Init(&pMe->cb, addrInfoResultPfn, pMe); ~
===pre>
      nErr = IAddrInfo_StartQuery(pMe->pIAddrInfo, "hostname.com", NULL, NULL, &pMe->cb); 
      if (AEE_SUCCESS != nErr) { ... } 
===/pre>
   @@@b> Retrieving query results in addrInfoResultPfn: @@@/b>
===pre>
      nErr = IAddrInfo_GetResult(pMe->pIAddrInfo, &queryStatus, &numberOfAddresses, &maxAddrInfoSizeInBytes); 
      if (AEE_SUCCESS != nErr) { ... } 
      if (AEE_SUCCESS != queryStatus) { ... } 

      IEnv_ErrMalloc(piEnv, maxAddrInfoSizeInBytes, (void**)(&pAddr));
      if (!pAddr) { ... } 

      // retrieve the first address (index 0) 
      nErr = IAddrInfo_GetResultAddr(pMe->pIAddrInfo, 0, pAddr, &maxAddrInfoSizeInBytes); 
      if (AEE_SUCCESS != nErr) { ...  } 

===/pre>
   The following header file is required:~
   BREW 4.0 - AEEIAddrInfo.h
   BREW 3.1 - AEEAddrInfo.h

=============================================================================

IAddrInfo_AddRef()

Description:
   This function is inherited from IBase_AddRef(). 

See Also:
   IAddrInfo_Release()

=============================================================================

IAddrInfo_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface(). 

See Also:
   IQI_QueryInterface

=============================================================================

IAddrInfo_Release()

Description:
   This function is inherited from IBase_Release(). 

See Also:
   IAddrInfo_AddRef()

=============================================================================

IAddrInfo_SelectNetwork()

Description:
   This function selects a specific data network.

Prototype:
   int IAddrInfo_SelectNetwork(IAddrInfo *me, int nNetwork)

Parameters:
   me: the interface pointer
   nNetwork: [in] : data network type (AEE_NETWORK_*)
 
Return Value:
   AEE_NET_SUCCESS: the data network was selected
   AEE_EBADSTATE: the network is not valid
   AEE_NET_EOPNOTSUPP: the network has already been selected.
   AEE_EFAILED: other error.

   Other error codes are also possible.

Comments:
   When an IAddrInfo instance is created, no network is selected.
   If an IAddrInfo method that requires a network (i.e. StartQuery) is called before
   IAddrInfo_SelectNetwork(), AEE_NETWORK_DEFAULT will be implicitly selected,
   and the decision of which network to actually use is deferred to lower layers.

   Once a network has been selected, either explicitly via
   IAddrInfo_SelectNetwork(), or implicitly as described above, the network 
   may not be changed. To use a different network, a new IAddrInfo instance is 
   required.

Version:
   Introduced BREW Client 3.1.5

See Also:
   Data networks

=============================================================================

IAddrInfo_StartQuery()

Description:
   This function sets the resolver query information and starts it.
   See RFC 3493 section 6.1 for more details.

   A query can not be started more then once!
  
Prototype:
   int IAddrInfo_StartQuery
   (
      IAddrInfo *me, 
      const char *szNodeName,
      const char *szServName, 
      const AEEAddrInfo *paiHints,
      AEECallback *pcb
   );

Parameters:
   me: the interface pointer
   szNodeName: [in] : NULL, host name, ip address or "\brewloopback"
   szServName: [in] : NULL or decimal port number string
   paiHints: [in] : some hints for the resolve process
   pcb: [in] : user supplied callback that will be resumed upon completion -
               results will be available only then.
               if NULL then starts a synchronous query:
               A synchronous query only returns immediately available results 
               (no network access) and cached results (if an address cache exists).
               if StartQuery() returns SUCCESS, results are immediately available.
               if an address needs to be resolved and is not found in the cache
               AEE_NET_UNKDOMAIN is returned.

Return Value:

   AEE_SUCCESS: 
      The query information was set.

   All other values indicate failure, in which case the
   query information is not set and the instance is unmodified.
   In this context, the following error codes have special meaning:
~
   AEE_EBADSTATE:
      The object is not in the proper state for the operation.
      i.e. StartQuery cannot be called again after a transaction was initiated.
   AEE_NET_EAI_BADFLAGS:
      dwFlags in paiHints had an invalid value.
   AEE_NET_GENERAL_FAILURE:
      A non-recoverable error occurred when processing the parameters.
   AEE_NET_EAFNOSUPPORT:
      The address family was not recognized.
   AEE_NET_ENOMEM:
      There was a memory allocation failure.
   AEE_NET_EAI_BADREQUEST:
      request parameters are in error, i.e. neither szNodeName nor szServName
      were supplied (at least one of these must be supplied).
   AEE_NET_ESOCKNOSUPPORT:
      The intended socket type was not recognized.
   AEE_NET_EAI_UNSUPPFLAGS:
      dwFlags in paiHints had an unsupported value.
   AEE_NET_UNKDOMAIN:
      for an synchronous query, the address needs to be resolved but was not
      found in the address cache.
*
   Other errors might be returned; the caller should verify that IAddrInfo_StartQuery()
   succeeded.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   AEEAddrInfo
   AEECallback

=============================================================================

IAddrInfo_GetResult()

Description:
   This function retrieves the query result, after the resolving transaction
   completes.

   On success, returns the resolve query status, the number of returned addresses
   and the maximum address size.

Prototype:
   int IAddrInfo_GetResult(IAddrInfo *me, int* pnStatus, int* pnAddrNumber, int* pnMaxAddrInfoByteSize)

Parameters:
   me: the interface pointer
   pnStatus: [out] : the query result status:
   ~
      AEE_SUCCESS: 
         The name was successfully resolved.
      AEE_NET_ETIMEDOUT:
         Request timed out.
      AEE_NET_GENERAL_FAILURE:
         A non-recoverable error occurred when attempting to resolve the name.
      AEE_NET_ENOMEM:
         There was a memory allocation failure.
      AEE_NET_EAI_BADREQUEST:
         request parameters are in error.
      AEE_NET_UNKDOMAIN:
         Unknown host domain name or no address records.
      AEE_NET_BADDOMAIN:
         Improperly formatted host domain name.
      AEE_NET_BADRESPONSE:
         Response did not make sense.

      Other error codes are also possible. 
 *  
   pnAddrNumber: [out] : The number of address information elements generated 
      as a result of the query.

   pnMaxAddrInfoByteSize: [out] : The size of the largest address information
      element generated as a result of the query.
 
Return Value:
   AEE_SUCCESS: 
      query results were returned.
   AEE_EBADSTATE:
      The object is not in the proper state for the operation.
      i.e. no query was initiated
   AEE_NET_EFAULT:
      Invalid buffer or argument has been specified.
      
   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

=============================================================================
IAddrInfo_GetResultAddr()

Description:
   This function retrieves a specific address information element query result,
   after the resolving transaction has completed.

Prototype:
   int IAddrInfo_GetResultAddr(IAddrInfo *me, int nAddrIndex, AEEAddrInfo* pai, int* pnAddrInfoBytes)

Parameters:
   me: the interface pointer
   nAddrIndex: [in] : zero based index of the address information element
      to retrieve.
   pai: [out] : pointer to the AEEAddrInfo to be filled in. 
      The memory must be pre-allocated by the client.
   pnAddrInfoBytes: [in/out] : 
      On input: pointer to the size (in bytes) of pai, which needs to be large enough
      to accommodate the address information element including the address itself.
      On output: updated with the actual size of the address information element, including 
      the address itself.
      Note that if *pnAddrInfoBytes is 0, pai is not used.
 
Return Value:
   AEE_SUCCESS: 
      query results were returned.
   AEE_EBADSTATE:
      The object is not in the proper state for the operation.
      i.e. no query was initiated
   AEE_NET_EFAULT:
      Invalid buffer or argument has been specified.
      
   Other error codes are also possible.

Comments:
   In order to get the size of an address information element, use zero *pnAddrInfoBytes, 
   i.e. addressSize = 0; IAddrInfo_GetResultAddr(me, index, NULL, &addressSize);

Version:
   Introduced BREW Client 3.1.5

See Also:
   AEEAddrInfo

=============================================================================
*/

#endif /* #ifndef AEEIADDRINFO_H */

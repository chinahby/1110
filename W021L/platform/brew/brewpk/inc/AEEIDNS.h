#ifndef AEEIDNS_H
#define AEEIDNS_H
/*=============================================================================

FILE:          AEEIDNS.h

SERVICES:      Generalized DNS queries

DESCRIPTION:   IDNS provides a flexible interface into the DNS client, allowing
               the caller to construct compound queries and examine all sections
               of the response record.

===============================================================================
   Copyright (c) 2006-2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/
#include "AEEIQI.h"
#include "AEECallback.h"
#include "AEEDNSTypes.h"

//=============================================================================
//   Defines and Type Declarations
//=============================================================================
typedef int16 AEEDNSType;
typedef int16 AEEDNSQType;

// The types and classes definitions are provided for convenience and should
// not be construed as definitive lists or as a restrictions on IDNS
// capabilities.  IDNS simply passes through any type and class values,
// validity of which is a matter between the server and the app using IDNS.
#define AEEDNSTYPE_A      1   // host address
#define AEEDNSTYPE_NS     2   // authoritative name server
#define AEEDNSTYPE_CNAME  5   // canonical name for an alias
#define AEEDNSTYPE_SOA    6   // start of a zone of authority
#define AEEDNSTYPE_PTR    12  // domain name pointer
#define AEEDNSTYPE_MX     15  // mail exchange
#define AEEDNSTYPE_TXT    16  // text strings
#define AEEDNSTYPE_AAAA   28  // IPv6 host address
#define AEEDNSTYPE_SRV    33  // service location

#define AEEDNSQTYPE_ALL   255 // matches all types; only valid in Questions

// DNS classes
typedef int16 AEEDNSClass;
typedef int16 AEEDNSQClass;

#define AEEDNSCLASS_IN     1
#define AEEDNSQCLASS_ALL   255   // matches all classes; only valid in Questions

// AEEDNSItem : Each item represents a DNS resource record (RR) or Question
// record.  Each section of the response is an array of these.
typedef struct _AEEDNSItem {
   const byte* pbyDomain;   // RR/Question: NAME  (use ParseDomain() to get SZ format)
   AEEDNSType  nType;       // RR/Question: TYPE
   AEEDNSClass nClass;      // RR/Question: CLASS
   int32       nTTL;        // RR-only: TTL value
   const byte* pbyData;     // RR-only: RDATA
   int32       cbData;      // RR-only: size of RDATA
} AEEDNSItem;

// DNS responds
typedef struct _AEEDNSResponse {
   uint16             uFlags;
   uint16             uReserved;
   int16              nQuestions;
   int16              nAnswers;
   int16              nServers;
   int16              nAdditional;
   const AEEDNSItem*  pQuestions;
   const AEEDNSItem*  pAnswers;
   const AEEDNSItem*  pServers;
   const AEEDNSItem*  pAdditional;
} AEEDNSResponse;

//=============================================================================
//   IDNS Interface
//=============================================================================
#define AEEIID_IDNS      0x1005006

#if defined(AEEINTERFACE_CPLUSPLUS)

struct IDNS : public IQI 
{
   virtual int CDECL AddQuestion (AEEDNSQType nType, AEEDNSQClass nClass, const char* pszDomain) = 0;
   virtual int CDECL Start (CallbackNotifyFunc pfn, void* pcxt) = 0;
   virtual int CDECL GetResponse (const AEEDNSResponse** pResp) = 0;
   virtual char CDECL ParseDomain (const byte* pbyDomain, int* pcb) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_IDNS(iname)\
   INHERIT_IQI(iname);\
   int   (*AddQuestion)   (iname* pi, AEEDNSQType nType, AEEDNSQClass nClass, const char* pszDomain);\
   int   (*Start)         (iname* pi, CallbackNotifyFunc pfn, void* pcxt);\
   int   (*GetResponse)   (iname* pi, const AEEDNSResponse** pResp);\
   char *(*ParseDomain)   (iname* pi, const byte* pbyDomain, int* pcb)

AEEINTERFACE_DEFINE(IDNS);

//=============================================================================
//   IDNS methods
//=============================================================================
static __inline uint32 IDNS_AddRef(IDNS *me)
{
   return AEEGETPVTBL(me,IDNS)->AddRef(me);
}

static __inline uint32 IDNS_Release(IDNS *me)
{
   return AEEGETPVTBL(me,IDNS)->Release(me);
}

static __inline int IDNS_QueryInterface(IDNS* me, AEECLSID id, void** ppo)
{
   return AEEGETPVTBL(me,IDNS)->QueryInterface(me, id, ppo);
}

static __inline int IDNS_AddQuestion(IDNS* me, AEEDNSQType nType, AEEDNSClass nClass, const char* pszDomain)
{
   return AEEGETPVTBL(me,IDNS)->AddQuestion(me, nType, nClass, pszDomain);
}

static __inline int IDNS_Start(IDNS* me, CallbackNotifyFunc pfn, void* pcxt)
{
   return AEEGETPVTBL(me,IDNS)->Start(me, pfn, pcxt);
}

static __inline int IDNS_GetResponse(IDNS* me, const AEEDNSResponse** ppResp)
{
   return AEEGETPVTBL(me,IDNS)->GetResponse(me, ppResp);
}

static __inline char* IDNS_ParseDomain(IDNS* me, const byte *pbyData, int* pcb)
{
   return AEEGETPVTBL(me,IDNS)->ParseDomain(me, pbyData, pcb);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=====================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

AEEDNSType

Description:
   This is a 16-bit integer used to hold DNS "type" values. Valid values are those
   defined in Internet standards and supported by the server being queried.

Definition:
   #define AEEDNSTYPE_A      1
   #define AEEDNSTYPE_NS     2
   #define AEEDNSTYPE_CNAME  5
   #define AEEDNSTYPE_SOA    6
   #define AEEDNSTYPE_PTR    12
   #define AEEDNSTYPE_MX     15
   #define AEEDNSTYPE_TXT    16
   #define AEEDNSTYPE_AAAA   28
   #define AEEDNSTYPE_SRV    33

   typedef int16 AEEDNSType;

Members:
   AEEDNSTYPE_A    :    host address
   AEEDNSTYPE_NS   :    authoritative name server
   AEEDNSTYPE_CNAME:    canonical name for an alias
   AEEDNSTYPE_SOA  :    start of a zone of authority
   AEEDNSTYPE_PTR  :    domain name pointer
   AEEDNSTYPE_MX   :    mail exchange
   AEEDNSTYPE_TXT  :    text strings
   AEEDNSTYPE_AAAA :    IPv6 host address
   AEEDNSTYPE_SRV  :    IPv6 host address

Comments:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   IDNS_AddQuestion()

=======================================================================

AEEDNSClass

Description:
   This is a 16-bit integer used to hold DNS "class" values. Valid values are those
   defined in Internet standards and supported by the server being queried.

Definition:
   #define AEEDNSCLASS_IN  1

   typedef int16 AEEDNSClass;

Members:
   AEEDNSCLASS_IN :

Comments:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   IDNS_AddQuestion()

=======================================================================

AEEDNSResponse

Description:
   Contains DNS response data.
   These items correspond directly to fields as described in the DNS protocol
   specification (RFC1035).

Definition:
   typedef struct AEEDNSResponse {
      uint16       uFlags;
      uint16       uReserved;
      int16        nQuestions;
      int16        nAnswers;
      int16        nServers;
      int16        nAdditional;
      AEEDNSItem * pQuestions;
      AEEDNSItem * pAnswers;
      AEEDNSItem * pServers;
      AEEDNSItem * pAdditional
   } AEEDNSResponse;

Members:
   uFlags      : the flag field in the DNS message header
   uReserved   : the reserved field in the DNS message header
   nQuestions  : the number of "Question" records in the message
   nAnswers    : the number of "Answer" records in the message
   nServers    : the number of "Authority" or server records in the message
   nAdditional : the number of "Additional" records in the message
   pQuestions  : pointer to an array of AEEDNSItem structures describing
                 Question records.  The size of this array is given by nQuestions.
                 Note that Question records make use of only a few fields of the
                 AEEDNSItem structure.  The remaining fields are left zero or NULL.
   pAnswers, pServers, pAdditional :
                 pointers to arrays of AEEDNSItem structures describing
                 Resource Records.  The sizes of these array are given by nAnswers,
                 nServers, and nAdditional, respectively.

Comments:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=======================================================================

AEEDNSItem

Description:
   Each AEEDNSItem structure describes either a DNS Question or a DNS Resource
   Record (RR).

Definition:
   typedef struct AEEDNSItem {
      const byte *       pbyDomain;
      AEEDNSType         nType;
      AEEDNSClass        nClass;
      int32              nTTL;
      const byte *       pbyData;
      int32              cbData;
   } AEEDNSItem;

Members:
   pbyDomain: The NAME field of the RR/Question record.  This is given as a
              pointer into the response data, and is in the DNS format for domain
              names (possibly using header compression).  Use IDNS_ParseDomain()
              to obtain a zero-terminated string in dotted notation.
   nType    : The TYPE field of the RR/Question record.
   nClass   : The CLASS field of the RR/Question record.
   nTTL     : RR-only: 32-bit TTL value.  Treat as a signed 32-bit int; note that
              multiple records could have different TTLs, although such server
              behavior is not recommended.
   pbyData  : RR-only: A pointer to the RDATA field of the RR record.  Any
              domain names within this memory range can be decoded using
              IDNS_ParseDomain().
   cbData   : RR-only: Number of bytes in the RDATA record.  Note that when
              pbyData[] contains a domain name, cbData may be much smaller than
              the resulting zero-terminated domain name
              (due to DNS header compression).

Comments:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=======================================================================

=======================================================================
   INTERFACES DOCUMENTATION
=======================================================================

IDNS Interface

Description:
   IDNS provides a way to perform DNS (Domain Name System) queries.

   IDNS provides a more general interface to the DNS client than
   IAddrInfo.  Alternate request types and compound
   queries are supported.

   To override the default settings, use IDNS_QueryInterface() with
   AEECLSID_DNSConfig to obtain an IDNSConfig interface.

   The DNS client will keep track of DNS servers and handle
   retransmission and timeouts.  IDNS converts domain names to the DNS
   protocol representation, and provides a method for decoding
   compressed domain names in the response.

   The user is responsible for specifying the content of DNS Question
   records, locating the data of importance in the response, and
   interpreting those values.

   UDP is the only transport supported.  Requests and responses are
   limited to 512 bytes.

The following header file is required:~
   BREW 4.0 - AEEIDNS.h
   BREW 3.1 and prior - AEEDNS.h

See Also:
   IDNSConfig  Interface
   IDNSConfig2 Interface
   INetwork    Interface
   ISockPort   Interface

=======================================================================
IDNS_AddRef()

Description:
	This function is inherited from IBASE_AddRef().
======================================================================= 
IDNS_Release()

Description:
	This function is inherited from IBASE_AddRef().
======================================================================= 
IDNS_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().
=======================================================================

IDNS_AddQuestion()

Description:
   This adds a question to the set of question records in the request.
   This should not be called after Start().

   This can be called multiple times to construct a request message
   consisting of multiple question records.

Prototype:
    int IDNS_AddQuestion(IDNS *pIDNS, AEEDNSQType nType, AEEDNSClass nClass, const char *pszDomain);

Parameters:
   pIDNS: the interface pointer
   nType:  DNS question type
   nClass:  DNS class
   pszDomain:  zero-terminated string representing a domain name in dotted
      notation (for example, "example.com").  Single dot terminators
      (e.g. "example.com.") are acceptable and treated identically to domain
      names without a terminating dot.  (Domain search paths and relative
      domain names are not supported.)

Return Value:
   AEE_SUCCESS: means that the question was appended to the request
            message.

   All other values indicate failure, in which case the
   DNS request is unmodified.  In this context, the following error
   codes have special meaning:

   AEE_EFAILED :         if the new question would make the DNS message exceed the
                     maximum size (512 bytes for DNS over UDP).
   AEE_EBADPARM:         is the domain name is mal-formed.
   AEE_NET_EINVAL:   if IDNS is not in the proper state for questions to be
                     added. Questions cannot be added after Start() has been called.

   Other errors might be returned; the caller should verify that
   AddQuestion() succeeded.

Comments:
   All domains are treated as fully-qualified domains by IDNS.  There is no
   domain suffix search list; suffixes are never attached to the provided
   domain string.

   Colon (:), slash (/) and comma (,) characters do not delimit the domain
   name as in IAddrInfo.

   When constructing a question record, IDNS does not use the compressed form
   of domain names.

Version:
   Introduced BREW Client 2.0

See Also:
   None

=======================================================================

IDNS_Start()

Description:
  Start the query, and schedule callback to be called.

  In order to prevent the callback from firing, and to cancel the operation,
  the user should release all references to the IDNS.

  Start() should not be called twice.

Prototype:
   IDNS_Start(IDNS *pIDNS, CallbackNotifyFunc pfn, void *pcxt);

Parameters:
   pIDNS :  the IDNS interface pointer
   pfn   :  function to be called after the operation completes.  This will only
            be called if the operation starts successfully (that is, when Start()
            returned SUCCESS).
   pv    :  void pointer to be passed to pfn() when it is called.

Return Value:
    AEE_SUCCESS : operation started.  This should always succeed when called the
              first time.
    AEE_NET_EINVAL : Start() called more than once.

Comments:
   Releasing all references to the IDNS object will cancel the operation and
   prevent the callback from being called.

Version:
   Introduced BREW Client 2.0

See Also:
   None

=======================================================================

IDNS_GetResponse()

Description:
   This function is to be called to obtain the DNS response after the
   query completes.

   On success, it returns a pointer to a structure with the
   description of the DNS response.  Response data includes pointers
   to structures; the referenced memory will be valid only for the
   lifetime of the IDNS object.  None of those pointers should be
   retained or used after the IDNS interface has been released.

Prototype:
   int IDNS_GetResponse(IDNS *pIDNS, const AEEDNSResponse **pResp);

Parameters:
   pIDNS: the IDNS interface pointer
   pResp: [out] a pointer to a structure describing the response.

Return Value:
   AEE_SUCCESS : A response message was received from a DNS server; *pResp
                 describes the result.  Success here does not imply that the
                 query was successful, just that a response was received.  The
                 application must inspect the AEEDNSResponse structure to
                 determine whether the requested data is present in the response.

   In all error cases, *pResp will point to an empty AEEDNSResponse
   structure.  No assumptions should be made about the contents.  Some
   possible error codes include:
   ENOMEMORY : memory allocation failure prevented request or response
   AEE_NET_ETIMEDOUT : retransmission timeout (no servers responded)

   Other error codes are possible, including socket-level error codes,
   such as AEE_NET_ENETNONET.


Comments:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=======================================================================

IDNS_ParseDomain()

Description:
   This function converts a DNS representation of a domain name into a
   zero-terminated string with dot (that is, period, or ".") delimiters.

Prototype:
   char *IDNS_ParseDomain(IDNS *pIDNS, const byte *pbyDomain, int *pcb);

Parameters:
   pIDNS    :  the IDNS interface pointer
   pbyData  : pointer to the start of the domain name.  This pointer
              must point into the DNS response data as described by an
              AEEDNSItem record.  This can be used to decode pbyDomain
              values, or to decode values within the pbyData[] array.
   pcb      : pointer to value to hold the number of bytes occupied by the
              domain name (in the source byte array, not in the resulting
              string).  In the case of a mal-formed domain name, *pcb will
              be set to zero.
              If pcb==NULL, it will be ignored.

Return Value:
   This function returns a zero-terminated string giving the host name, or
   NULL on failure.

   Domain names are returned in dotted notation, with no terminating dot
   character at the end.

   Failure could be due to an allocation failure or a mal-formed domain name;
   *pcb can be used to distinguish between the two.

Comments:
   Returned strings do not include terminating dots, which usually indicate
   fully qualified domain names, because all domains are treated as fully
   qualified by IDNS.  An empty domain is returned as an empty string ("")
   instead of a single dot (".").

Version:
   Introduced BREW Client 2.0

See Also:
   None

=======================================================================*/

#endif /* AEEIDNS_H */

#ifndef AEEX509Chain_H
#define AEEX509Chain_H
/*======================================================

FILE:  AEEX509Chain.h

SERVICES:
        IX509Chain

GENERAL DESCRIPTION:

        This holds and manipulates a chain of X.509 
        certificates. Its main use is to verify trust 
        chain, retrive required attributes out of the 
        certs (e.g. the public key), and to provide 
        access to other parts for display to end user.

=====================================================

Copyright © 2002-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/

#include "AEE.h"
#include "AEEError.h"
#include "AEEWeb.h"
#include "AEEASN1OIDS.h"
#include "AEERSA.h"



typedef struct X509RSAKey
{
   const uint8  *puModulus;
   uint16        uModulusLen;
   const uint8  *puExponent;
   uint16        uExponentLen;
} X509RSAKey;
 

typedef struct X509BasicCert
{
   uint32        uTrustOverrideID;
   uint32        uSigAlgID; 
   uint32        uStartValidity;
   uint32        uEndValidity;
   X509RSAKey    Key;
   uint8         uVersion; // at end to waste less bytes aligning
} X509BasicCert;


#define X509CHAIN_FIELD_RAW_CERT   (0)  // The full unparsed cert  
#define X509CHAIN_FIELD_SIGNED     (1)  // The uparsed bulk of the cert
#define X509CHAIN_FIELD_SERIAL     (2)  // The parsed-out serial number
#define X509CHAIN_FIELD_SIG_TYPE   (3)  // This is uninterpreted; get it out of the basic info instead
#define X509CHAIN_FIELD_ISSUER     (4)  // The unparsed Issuer DN
#define X509CHAIN_FIELD_VALIDITY   (5)  // Uparsed validity dates; use basic info instead
#define X509CHAIN_FIELD_SUBJECT    (6)  // The unparsed Subject DN
#define X509CHAIN_FIELD_PUBKEY     (7)  // Uparsed public key; used basic info or GetRSAKey instead
#define X509CHAIN_FIELD_EXTENSIONS (8)  // The whole unprocessed extension block
#define X509CHAIN_FIELD_SIGN_ALG   (9)  // The second instance of SIG_TYPE
#define X509CHAIN_FIELD_SIGNATURE  (10) // The actual signature by the isser over the cert
#define X509CHAIN_FIELD_MAX        (11)
#define X509CHAIN_FIELD_MD5HASH    (20) // The MD5 hash over the entire cert
#define X509CHAIN_FIELD_SHAHASH    (21) // The SHA hash over the entire cert


typedef struct X509PartRequest
{
   int32        nCertWebOpt;
   int32        nCertIndex;
   uint8        uCertPart;
   const uint8 *pcRawOID;
   uint32       uASNOID;
   int          nInstance;
} X509PartRequest;
 

typedef struct X509CertPart
{
  unsigned int uLength;  
  const uint8 *pcValue;  
  const uint8 *pcRawOID;
  uint32       uASNOID;
  boolean      bCriticallity; 
  uint8        uASN1Type;
  uint8        uRDNIndex;
  int          nNumInstances;
} X509CertPart;



/* 
   Hard upper limit on the length of the chain. NOT the limit on 
   number of certs stored
 */
#define X509CHAIN_MAX_CHAIN       8 


typedef struct X509TrustResult
{
   int    nResult;
   int    nChainLen;
   uint32 auChainErrors[X509CHAIN_MAX_CHAIN];
   uint32 uErrorSummary;
} X509TrustResult;




#define X509CHAIN_TRUST_OK           (0) 
#define X509CHAIN_OVERRIDE_ALL       (0xffffffff)
#define X509CHAIN_CERT_TRUST         (0x00000001) 
#define X509CHAIN_LONG_CHAIN 	  	    (0x00000002)	
#define X509CHAIN_CERT_EXIPRED       (0x00000004)
#define X509CHAIN_BAD_DATE           (0x00000008)
#define X509CHAIN_HOST_MISMATCH      (0x00000010)	
#define X509CHAIN_NO_TARGET_HOST     (0x00000020) 
#define X509CHAIN_NO_HOST_IN_CERT    (0x00000040) 
#define X509CHAIN_BAD_SIGNATURE      (0x00000080) 
#define X509CHAIN_CERT_HASH_UNKNOWN  (0x00000100)
#define X509CHAIN_CERT_HASH_UNSUPP   (0x00000200)
#define X509CHAIN_SIGNER_KEY_BAD     (0x00000400)
#define X509CHAIN_CRITICAL_EXTENSION (0x00080000) 
#define X509CHAIN_BASIC_CONST        (0x00100000)
#define X509CHAIN_EXT_KEY_USE        (0x00200000)
#define X509CHAIN_CRIT_EXT_KEY_USE   (0x00400000)
#define X509CHAIN_KEY_USE            (0x00800000)
#define X509CHAIN_INTEGER_ID         (0x01000000)
#define X509CHAIN_NON_LEAF_EXT_KEY_USE (0x02000000)
#define X509CHAIN_FROM_DATE          (0x04000000)

typedef struct {
   uint32  uCertID;       // from X509BasicCert
   uint32  uOverrideBits; // Which trust errors to override
} X509TrustOverride;




/* ======================================================================= 
   Data structures - The web opts
   ======================================================================= */
// Claim our chunk of the ID space to avoid collision with ISSL & IWebOpts
#define WEBOPT_X509_SZ             0x00012200
#define WEBOPT_X509_SZ_LAST        0x000123ff
#define WEBOPT_X509_32BIT          0x00022200
#define WEBOPT_X509_32BIT_LAST     0x000223ff
#define WEBOPT_X509_VARIABLE       0x10020000
#define WEBOPT_X509_VARIABLE_LAST  0x1002f000
#define WEBOPT_X509_VARBUFFER      0x00042000
#define WEBOPT_X509_VARBUFFER_LAST 0x00042fff
#define WEBOPT_X509_ASN1           0x00052000
#define WEBOPT_X509_ASN1_LAST      0x00052fff



#define WEBOPT_X509_ROOT_CERTS            (WEBOPT_X509_ASN1+1)
#define WEBOPT_X509_BRANCH_CERTS          (WEBOPT_X509_ASN1+2)
#define WEBOPT_X509_LEAF_CERT             (WEBOPT_X509_ASN1+3)
#define WEBOPT_X509_CHAIN_CERT            (WEBOPT_X509_ASN1+4)
#define WEBOPT_X509_HOST                  (WEBOPT_X509_SZ+1)    
#define WEBOPT_X509_OVERRIDE              (WEBOPT_X509_VARIABLE + sizeof(X509TrustOverride))
#define WEBOPT_X509_MAX_CHAIN             (WEBOPT_X509_32BIT+1)
#define WEBOPT_X509_EXT_KEY_USE           (WEBOPT_X509_32BIT+2)
#define WEBOPT_X509_KEY_USE               (WEBOPT_X509_32BIT+3)

// Leave space for backporting new apiOne WebOpts.
#define WEBOPT_X509_RSA_IMPLEMENTATION    (WEBOPT_X509_32BIT+8)

#define WEBOPT_X509_INTEGER_ID            (WEBOPT_X509_VARBUFFER + 1)

// "Header" part of the struct used for specifying set of values - 
// OEM IDs, Carrier IDs, etc.
typedef PACKED struct X509IntegerSet
{
   uint16       uLen;
   uint16       uOIDLen;
   int32        nASNOID;
} X509IntegerSet;


// We use the MSB of the 16 bit OID to indicate that the valueset consists of 16 or 32 bit values
#define SET_DATASET_32BIT(x) (x|=0x8000)                    // Set the MSB of the 16 bit OID len
#define IS_DATASET_32BIT(x) ((boolean)((x&0x8000)==0x8000)) // Check the MSB of the 16 bit OID len - if set, then 32bit values
#define REMOVE_DATASET_32BIT(x) (x &=0x7FFF)                // Remove the MSB of the 16 bit OID len - if set, then 32bit values

/* ======================================================================= 
   The calling interface
   ======================================================================= */

typedef struct IX509Chain IX509Chain;
AEEINTERFACE(IX509Chain) {
   INHERIT_IWebOpts(IX509Chain);

   int   (*AddCert)       (IX509Chain *pMe, int32 nCertWebOpt, const uint8 *pCert, int nCertLen);   
   int   (*GetBasic)      (const IX509Chain *pMe, int32 nCertWebOpt, int32 nCertIndex, X509BasicCert *pBasic);               
   int   (*GetField)      (IX509Chain *pMe, int32 nCertWebOpt, int32 nCertIndex, int nField, const uint8 **ppcField, unsigned int *puFieldLen);
   int   (*GetFieldPart)  (const IX509Chain *pMe, const X509PartRequest *pReq, X509CertPart *pPart);
   int   (*GetRSAKey)     (const IX509Chain *pMe, int32 nWebOpt, int32 nOptIndex, IRSA **piRSA);
   void  (*Verify)        (IX509Chain *pMe, X509TrustResult *pTrustResults, AEECallback *pVerifyCB, ...);
   void  (*VerifyV)       (IX509Chain *pMe, X509TrustResult *pTrustResults, AEECallback *pVerifyCB, const WebOpt *awozList);
};

#define IX509CHAIN_AddRef(p)                  AEEGETPVTBL((p),IX509Chain)->AddRef((p))
#define IX509CHAIN_Release(p)                 AEEGETPVTBL((p),IX509Chain)->Release((p))  

#define IX509CHAIN_QueryInterface(p,clsid,pp) AEEGETPVTBL((p),IX509Chain)->QueryInterface((p),(clsid),(pp))
#define IX509CHAIN_AddOpt(p,o)                AEEGETPVTBL((p),IX509Chain)->AddOpt((p),(o))
#define IX509CHAIN_RemoveOpt(p,i,n)           AEEGETPVTBL((p),IX509Chain)->RemoveOpt((p),(i),(n))
#define IX509CHAIN_GetOpt(p,i,n,o)            AEEGETPVTBL((p),IX509Chain)->GetOpt((p),(i),(n),(o))

#define IX509CHAIN_AddCert(p,i,c,l)           AEEGETPVTBL((p),IX509Chain)->AddCert((p),(i),(c),(l))
#define IX509CHAIN_GetBasic(p,o,i,b)          AEEGETPVTBL((p),IX509Chain)->GetBasic((p),(o),(i),(b))
#define IX509CHAIN_GetField(p,o,i,f,d,l)      AEEGETPVTBL((p),IX509Chain)->GetField((p),(o),(i),(f),(d),(l))
#define IX509CHAIN_GetFieldPart(p,r,o)        AEEGETPVTBL((p),IX509Chain)->GetFieldPart((p),(r),(o))
#define IX509CHAIN_GetRSAKey(p,o,i,k)         AEEGETPVTBL((p),IX509Chain)->GetRSAKey((p),(o),(i),(k))
#define IX509CHAIN_Verify(p,args)             AEEGETPVTBL((p),IX509Chain)->Verify args
#define IX509CHAIN_VerifyV(p,t,a,w)           AEEGETPVTBL((p),IX509Chain)->VerifyV((p),(t),(a),(w))


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

=======================================================================
X509RSAKey

Description:
   Simple format for RSA key.

Definition:
   typedef struct X509RSAKey
   {
      const uint8  *puModulus;
      uint16        uModulusLen;
      const uint8  *puExponent;
      uint16        uExponentLen;
   } X509RSAKey;

Members:
   puModulus            Points to bytes that are modulus
   uModulusLen          Length in bytes of modulus
   puExponent           Pointer to bytes that are exponent
   uExponentLen         Length in bytes of exponent

Comments:
   None

See Also:
   IRSA interface, IX509CHAIN_GetRSAKey

=======================================================================
X509BasicCert

Description:
   Basic certificate information

Definition:
   typedef struct X509BasicCert
   {
      uint32        uTrustOverrideID;
      uint32        uSigAlgID; 
      uint32        uStartValidity;
      uint32        uEndValidity;
      X509RSAKey    Key;
      uint8         uVersion;
   } X509BasicCert;

Members:
   uTrustOverrideID     An integer ID for use with trust override records
   uSigAlgID            The type of signature used to sign the cert. (not
                        the type of key in the cert)
   uStartValidity       The start date of the cert in GPS time
   uEndValidity         The expiration date in GPS time 
   Key                  The key in the cert.
   uVersion             The version of the cert. Value 2 means version 3.

Comments:
   This is about 40 bytes so it can go on the stack. It includes all
   the fields in the cert that are small.

See Also:
   IX509CHAIN_GetRSAKey()


=======================================================================
X509PartRequest

Description:
   Specify which parts of a certificate field to get
   
Definition:
   typedef struct X509PartRequest
   {
      int32        nCertWebOpt;
      int32        nCertIndex;
      uint8        uCertPart;
      const uint8 *pcRawOID;
      uint32       uASNOID;
      int          nInstance;
   } X509PartRequest;

Members:
   nCertWebOpt          Which of WEBOPT_X509_ROOT_CERTS, 
                        WEBOPT_X509_BRANCH_CERTS ...
   nCertIndex           Which cert to fetch parts from
   uCertPart            Which field to get parts from. One of 
                        X509CHAIN_FIELD_ISSUER, X509CHAIN_FIELD_SUBJECT or
                        X509CHAIN_FIELD_EXTENSIONS
   pcRawOID             Points to raw OID of part of subject, issuer or
                        extenions. Must be valid or NULL.
   uASNOID              The constants above as an alternate to the OID. 
                        pcRawOID must be NULL to use this.
   nInstance            0 is the first occurance. (note that response
                        includes total number of occurances)
Comments:
   This is about 20 bytes so it can go on the stack. Use this to 
   specify all four items you need to get a cert part: 1) the WebOpt ID,
   the WebOpt/cert index, the field in the cert, and the OID of the 
   part in the field. This is used for getting parts of the distinguished
   names or the X509 v3 extensions.

   See AEEASN1OIDs.h for an alternate scheme for specifying OIDs for
   parts of fields. ASNOIDs here are a set of integers that correspond
   to OIDs for common OIDs. This is much more convenient than carrying
   arround the full DER/BER encoded OIDs.

See Also:
   IX509CHAIN_GetFieldPart()
   X509CertPart

=======================================================================
X509CertPart

Description:
   Gives the result of getting a certificate part
   
Definition:
   typedef struct X509CertPart
   {
      unsigned int uLength;  
      const uint8 *pcValue;  
      const uint8 *pcRawOID;
      uint32       uASNOID;
      boolean      bCriticallity; 
      uint8        uASN1Type;
      uint8        uRDNIndex;
      int          nNumInstances;
   } X509CertPart;

Members:
   uLength              Is the length of the part. 
   pcValue              Pointer to actual part, persists as long as 
                        IX509Chain
   pcRawOID             Points to unparsed OID starting with the type OID
   uASNOID              Parsed value of OID if we know it. See ASNOID 
                        defines.
   bCriticality         For v3 extensions only, true if they are critical
   uASN1Type            The data type of the value from AEEASN1OIDs.h (e.g.,
                        ASN1_PRINTABLE_STRING_TYPE)
   uRDNIndex            In a DN this counts the RDN's. If the part was
                        found in the first RDN, this is 0.This is always 0
                        for extensions
   nNumInstances        The number of instances of this OID in this field

Comments:
   This is about 20 bytes so it can go on the stack. This is used
   to point to either parts of a DN (distinguished name) or to an
   X.509 v3 extension. Although semantically unrelated, representing
   and fetching them are similar.


See Also:
   IX509CHAIN_GetFieldPart()
   X509PartRequest

=======================================================================
X509TrustResult

Description:
   Gives the results evaluating a certificate chain
  
Definition:
   typedef struct X509TrustResult
   {
      int    nResult;
      int    nChainLen; 
      uint32 auChainErrors[X509CHAIN_MAX_CHAIN];
      uint32 uErrorSummary; 
   } X509TrustResult;

Members:
    nResult             Reports general errors such as out of memory or 
                        lack of a leaf cert that cause no chain verification
                        at all to be performed. The more usual trust errors
                        are in the array and summary below. A value of SUCCESS
								here does not mean the chain is trusted.

    nChainLen           Indicates the number of certs in the chain and thus
                        how far into auChainErrors to examine errors. Note
                        that the chain may be incomplete and not chain up
                        to a root certificate.

    auChainErrors       If nResult is SUCCESS then the results are in
                        auChainErrors. This describes the errors at each
                        level in the chain as a bit field. The bits are
                        X509CHAIN_* as described in trust errors.  The 0th
                        integer is the server's cert, going up to the root
                        cert.
    
    uErrorSummary       The OR'ing of all the trust error bits for the
                        whole chain. If this value is zero the trust is all
                        OK. This value is also useful to report a simpler
                        set of errors to the user.

Comments:
   This structure is about 40 bytes. It is filled in by IX509CHAIN_Verify()
   to report the result of a verify operation.

   You must check both nResult and uErrorSummary to be get the complete
	results of the verify operation.

   Just about any error other than an unparsable or missing leaf or
   server cert is considered a trust error. Trust errors can be overriden.
   The following lists the trust errors:
===pre>
      X509CHAIN_TRUST_OK           No flags set -> In SSL we trust

      X509CHAIN_CERT_TRUST         Couldn't find any more certs to chain up 
                                   to and didn't get to a root

      X509CHAIN_LONG_CHAIN 	  	  Got to maximum chain length (hard limit or
                                   the limit in option) Also may be returned 
                                   if path length in basic constraints 
                                   extension was reached.

      X509CHAIN_CERT_EXPIRED       Cert is either expired or not valid

      X509CHAIN_BAD_DATE           Couldn't get a valid start or end date out
                                   of cert

      X509CHAIN_HOST_MISMATCH      Got a hostname out of cert but it didn't
                                   match set host

      X509CHAIN_NO_TARGET_HOST     No host target hostname was specified. No 
                                   WEBOPT_X509_HOST set.

      X509CHAIN_NO_HOST_IN_CERT    There was no hostname in the leaf 
                                   certificate

      X509CHAIN_BAD_SIGNATURE      Means cert was damaged or tampered
                                   with

      X509CHAIN_CERT_HASH_UNKNOWN  Could not figure which hash is used in 
                                   signature on cert

      X509CHAIN_CERT_HASH_UNSUPP   Figured out what the hash is, but not 
                                   supported

      X509CHAIN_SIGNER_KEY_BAD     The key in the signer cert is syntactically
                                   invalid

      X509CHAIN_CRITICAL_EXTENSION Some critical exention(s) was(were) not
                                   understood

      X509CHAIN_BASIC_CONST        violation in basic constraints extension 
                                   of cert

      X509CHAIN_EXT_KEY_USE        Indicates that a cert in the chain
                                   had an extended key use extension and that
                                   extension did not contain all of the 
                                   intended uses specified in the 
                                   WEBOPT_X509_EXT_KEY_USE option. Or, whether
                                   or not the option is set, this error will
                                   occur if any non-leaf cert as the extended
                                   key use extension marked as critical.

      X509CHAIN_CRIT_EXT_KEY_USE   As above, but extended key use was marked
                                   as critical

      X509CHAIN_KEY_USE            A cert had a Key Use Extension that was 
                                   marked critical and did not contain all
                                   the inteded uses. For leaf certs the
                                   intended uses come from the WebOpt
                                   WEBOPT_X509_KEY_USE. For root and
                                   intermediate certs the usage must be 
                                   ASNOID_KEYUSE_CERTSIGN (if the extension
                                   is critical)

      X509CHAIN_INTEGER_ID         The integer ID or set of IDs specified by
                                   the client did not fall inside the range of 
                                   relevant integer IDs specified in the
                                   set-of-integers extension of one or more
                                   certs in the chain.

===/pre>
See Also:
   IX509CHAIN_Verify()

=======================================================================
X509TrustOverride

Description:
   Structure to specify that certain trust checks be skipped
  
Definition:
   typedef struct {
      uint32  uCertID;       
      uint32  uOverrideBits;
   } X509TrustOverride;

Members:
   uCertID              Comes from X509BasicCert data structure. 
   
                        If uCertID is all zero, then the override applies
                        to all certs in the chain. Note that this is a bit
                        dangerous, though necessary in some cases. When
                        using certs without SSL, say for verify digital
                        signatures, verifying the hostname and checking
                        dates are unecessary. The way to turn off this
                        checking is an override of these two two checks on
                        all certs in the chain.
 
   uOverrideBits        Is a map of bits from X509CHAIN_ indicating what
                        trust failures to override.

Comments:
   One of these represents a set of trust overrides for a particular
   certificate. This is used by adding an option with
   this structure as the value, and WEBOPT_X509_OVERRIDE as the ID. The 
   option may be added to IX509Chain, or may be passed in to the arguments
   to IX509CHAIN_Verify().

   Trust override records are useful for storing trust overrides for the 
   short or long term. That is this binary format won't be changed from
   release to release of Brew.

   The same macros/constants are used for indicating trust errors and
   overrides. The following explains how the bits are used as 
   trust overrides. The generally disable some part of the trust'
   checking.
===pre>
      X509CHAIN_OVERRIDE_ALL       Override all trust processing. This is
                                   dangerous. 

      X509CHAIN_CERT_TRUST         Trust this certificate as a root during
                                   chaining operation

      X509CHAIN_LONG_CHAIN 	  	   Doesn't work as an override. Use 
                                   WEBOPT_X509_MAX_CHAIN instead

      X509CHAIN_CERT_EXIPRED       Don't check the dates on the cert (but
                                   still try to parse them)

      X509CHAIN_BAD_DATE           Turn off all attempts to examine dates
                                   in cert

      X509CHAIN_HOST_MISMATCH      Don't try to compare the names
      
      X509CHAIN_NO_TARGET_HOST     Same as X509CHAIN_HOST_MISMATCH. Don't
                                   try to check the hostname 

      X509CHAIN_NO_HOST_IN_CERT    Same as X509CHAIN_HOST_MISMATCH. Don't
                                   try to check the hostname.

      X509CHAIN_BAD_SIGNATURE      Don't attempt to verify the signature on
                                   this cert; no heavy crypto.

      X509CHAIN_CERT_HASH_UNKNOWN  Same as X509CHAIN_SEVER_CERT_SIG_BAD

      X509CHAIN_CERT_HASH_UNSUPP   Same as X509CHAIN_SEVER_CERT_SIG_BAD
      
      X509CHAIN_SIGNER_KEY_BAD     Stops signature verification on certs 
                                   *signed* by this cert

      X509CHAIN_CRITICAL_EXTENSION Will still try to check the extensions 
                                   that are understood but will silently 
                                   ignore the ones not understood

      X509CHAIN_BASIC_CONST        Don't check the basic constraints

      X509CHAIN_EXT_KEY_USE        Extened key use extension will be ignored
                                   if it is not critical

      X509CHAIN_CRIT_EXT_KEY_USE   Extended key use will be completely ignored

      X509CHAIN_KEY_USE            Disables all key usage processing

  
      X509CHAIN_INTEGER_ID         Ignore integer ID checking

      X509CHAIN_NON_LEAF_EXT_KEY_USE 
                                   When set, extended key use is checked on
                                   the root and intermediate certs. If not set
                                   checking is only done on the leaf certs. This
                                   is really just a flag, not a trust override, 
                                   nor is it ever returned as an error.

      X509CHAIN_FROM_DATE          Ignores the check on the "from" date of
                                   the cert chain.

===/pre>
See Also:
   IX509CHAIN_Verify()

=======================================================================
X509IntegerSet

Description:
   Structure to specify extension that consists of a set of integer
   values.  These values may be 16 or 32bit.  Typical usage includes
   specifying sets of carrier IDs or OEM IDs.
  
   This is the "Header" part of the struct used by the caller for 
   specifying sets of values.The actual values lie outside this struct.

Definition:
   typedef PACKED struct X509IntegerSet
   {
      uint16       uLen;
      uint16       uOIDLen;
      int32        nASNOID;
   } X509IntegerSet;

Members:
   uLen                 The length of the remainder of the data, excluding this field
   
   uOIDLen              The length of the pcOID array in bytes

   nASNOID              The ASNOID


Comments:
   This header is followed by a raw OID with the length specified by uOIDLen.
   Immediately following the OID is integer set data.

   We use the MSB of uOIDLen to indicate whether the set is encoded as an array of 
   16 or 32 bit values.  If this bit is set, it means that the set that
   follows consists of 32bit values.

   If, after the MSB is stripped, uOIDLen is greater than 0, the OID that
   immediately follows the data is used to match the OID in the certificate.
   If it is 0, nASNOID is used instead.

   The first value in the integer set data specifies the number of ranges
   in the set.  It is followed by pairs of values, where each pair specifies
   an inclusive range.  All the remaining values specify individual items
   in the set.  For example, the following array of integers:

   2,0,10,12,14

   is interpreted as:

   0-10, 12, 14

   which means that the set consisted of 0 through 10 as well as 12 and 14.

======================================================================== 
  INTERFACES   DOCUMENTATION
======================================================================= 
IX509Chain Interface

This interface is used for managing and verifying a chain of X.509 certificates.

The IX509Chain Interface performs the following tasks:

 -    Manages the storage of certs, or accomodates your means of storage

 -    Gives access to parsed fields the cert for display to user or other 

 -    verifies cert chain against a set of trusted roots and parameters

 -    Stores a cert using the standard inherited WebOpts functions.

===H2>
To use IX509Chain Interface:
===/H2>
~
   1. Add root certs, the server's cert and the intermediate (branch) certs using
      IX509Chain_AddCert() or IX509CHAIN_AddOpt().

   2. Call IX509CHAIN_Verify() to find out of the chain is trusted

   3. If it's not trusted and you want to override it, add some X509TrustOverride
      records with AddOpt.

   4. Use the IX509CHAIN_GetBasic() or IX509CHAIN_GetRSAKey() function to get the public
      key if you need it for some function such as encrypting a session key.
*
===h2>
To display a single certificate:
===/H2>
~

    1) The cert itself has to be identified out of the collection. This is
       by the certificate type (root, branch or leaf) and an index. The 
       WebOpt ID gives the certificate type (see constants below) and the 
       WebOpt index gives the certificate index.
       
    2) Next the field of the cert must be identied. For example extensions, 
       subject, or serial number. This is either implicit in the parts
       returned when calling GetBasic, or for larger items that won't fit 
       on the stack and need to be treated as binary blobs, by passing the 
       ID of the field to GetField or GetFieldPart. If you're not using 
       GetFieldPart skip the next to steps.
           
    3) Next you have to give the OID of the part of the issuer/subject,
       or the OID for the extension. There are two ways to give the OID, 
       pre-defined IDs in AEEASN1OIDS.h, or the full OID in DER/BER encoded
       ANS.1 format.
       
    4) Finally, you have to specify which instance of the OID in the 
       field you want. For example you might want the second OU (org unit)
       out of the issuer field. You may also iterate over all instances. 
*
 
===H2>
To verify a chain:
===/H2>
~
     1) Add root certificates, the server's cert and the intermediate (branch) 
        certs using IX509CHAIN_AddCert() or IX509CHAIN_AddOpt().

     2) Call IX509CHAIN_Verify() to find out of the chain is trusted

     3) If it's not trusted and you want to override add some X509TrustOverride
        records with AddOpt.

     4) Use the IX509CHAIN_GetBasic() or IX509CHAIN_GetRSAKey() function to get the
        public key if you need it for some function such as encrypting
        a session key.
*  

The following header file is required for IX509Chain:

   AEEX509Chain.h 

============================================================================== 

IX509CHAIN_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

See Also:

    IX509CHAIN_Release()

=============================================================================

IX509CHAIN_Release()

Description:
    This function is inherited from IBASE_Release(). 

See Also:

    IX509CHAIN_AddRef()

=============================================================================

IX509CHAIN_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().
	Since IX509Chain inherits from IWebOpts, you may use IQI_QueryInterface() 
	to get an IWebOpts interface to an IX509Chain.

See Also:
None
=============================================================================
IX509CHAIN_RemoveOpt() 

Description:
	This function is inherited from IWEBOPTS_RemoveOpt().

See Also:
	IX509CHAIN_AddOpt()
=============================================================================
IX509CHAIN_GetOpt()

Description:
	This function is inherited from IWEBOPTS_GetOpt().
See Also:
	IX509CHAIN_AddOpt()
=============================================================================

IX509CHAIN_AddOpt()

Description:
	This function is inherited from IWEBOPTS_AddOpt().

   When certs are set or fetched they are in their binary DER form. Note 
   that this uses the WEBOPT_ASN1 type. The length of the option is derived
   from the length of the first object in the ASN.1 structure. The full 
   length of the cert is actually the length in the ASN.1 structure plus
   the length of the type, plus the length of the length.

   The WebOpt ID for certificates indicates their purpose or role. There
   can be several WebOpts/certificates of each ID / purpose / role. The 
   IDs are:

      WEBOPT_X509_ROOT_CERTS    Indicates this is a trusted root cert

      WEBOPT_X509_BRANCH_CERTS  These are intermediate certs that might be
                                used to complete the chain from the the leaf
                                to the root. 

      WEBOPT_X509_LEAF_CERT     This is the cert that you are actually trying
                                to verify. In SSL the server's cert. This 
                                should be a single value (others than the 
                                first are ignored)

      WEBOPT_X509_CHAIN_CERT    This is a slightly magic WEBOPT type. After 
                                IX509CHAIN_Verify() is called and the 
                                certificates that form the chaine are identified,
								this WEBOPT type can be used to get the certs
								that form the chain. The certificate of this 
								type with index 0 is the leaf or server's cert.
								An error will be returned if you try to fetch a 
								certificate of this type before 
                                IX509CHAIN_Verify() is called or if you request
								an index off the end of the chain. If some of the
								certs are removed using RemoveOpt, fetching 
								certs in the chain might also fail. Basically
				you can iterate from index 0 up until you get
				an error to get the certs in the chain that was
				formed.


   NOTE: You can add the same cert multiple times as different types.
   When completing a cert chain, the root certs are searched first, then
   the branch certs.

   See IX509CHAIN_Verify() to understand how these options
   are used in completing the chain.

Version:
   Introduced BREW Client 2.0

See Also:
   IX509CHAIN_AddCert()

=============================================================================

IX509CHAIN_AddCert()

Description:
   An alternate to IX509CHAIN_AddOpt() to add certs
   
Prototype:
    int IX509CHAIN_AddCert
    (
    IX509Chain *pMe, 
    int32 nCertWebOpt, 
    const uint8 *pCert,
    int nCertLen
    );

Parameters:
    pMe :  The Interface pointer
    nCertWebOpt :  WEBOPT_X509_ROOT_CERT, .._LEAF_CERT or ... _BRANCH_CERT
    pCert :  Pointer to DER encoded certificate in buffer
    nCertLen :  Length of certificate

Return Value:
    SUCCESS: if task is successful

    ENOMEMORY:  can't add due to lack of memory

    EMEMPTR:  ASN.1/DER length of object greater than nCertLen.

Comments: 
   ASN.1/DER formatted certs can be added directly with IX509CHAIN_AddOpt(), 
   however there is no proper buffer length checking when that is done. This 
   interface checks that the internal ASN.1/DER length of the object added is
   less than the length given in nCertLen.

   The ASN.1/DER length is the ASN.1/DER encoded length in pCert that comes after
   the single type ASN.1/DER type byte.

   Another way to add certificates so they are accessible to be parsed individually
   or to be used as part of chain in a verify operation is to put them in 
   another IWebOpts object and add that object as a default to an instance of
   IX509Chain. (add the interface pointer as type WEBOPT_DEFAULTS). Further, the
   IWebOpts object added doesn't necessarily have to implement all of IWebOpts.
   Only the IWEBOPTS_GetOpt() function is called, so the implementation can be
   extremely simple.

Side Effects: 
   None 
    
Version:
   Introduced BREW Client 2.0

See Also:
   IX509CHAIN_AddOpt()

=============================================================================

IX509Chain_GetBasic()

Description:
       Get basic and small fields in a certificate
   
Prototype:
    int IX509CHAIN_GetBasic
    (
    const IX509Chain *pMe, 
    int32 nCertWebOpt, 
    int32 nCertIndex, 
    X509BasicCert *pBasic
    );

Parameters:
    pMe :  The Interface pointer
    nCertWebOpt :  Which WebOpt ID to get them from- WEBOPT_X509_ROOT_CERT, _LEAF_CERT
or ... _BRANCH_CERT or .._CHAIN_CERT
    nCertIndex :  The index of certificate of the given WebOpt ID / type
    pBasic :  Place to put fields retrieved. Caller supplies the storage.

Return Value:
    SUCCESS: if task is successful

    EFAILED:  couldn't find the certificate

    AEE_X509_BAD_CERT:  the certificate was NULL or can't be parsed

    AEE_X509_UNKNOWN_KEY:  the key type is unknown (not RSA)

    AEE_X509_BAD_KEY:  the key can't be parsed

    AEE_X509_BAD_VALIDITY:  the validity dates were bad

    AEE_X509_NO_CHAIN_YET:  tried to get WEBOPT_X509_CHAIN_CERT before IX509CHAIN_Verify()
            was called or no leaf added.

  This returns only the first error encountered for the certificate; there may
  be multiple problems with any given certificate.

Comments:  
   The X.509 parser is hand-coded to X.509v3. It is not based on a general 
   ASN.1 parser.  It will generate correct results from correct input. It will
   not verify  every shred of ASN.1 (ASN.1 is a very redundant syntax and
   thus very large amounts of validation are possible).  On incorrectly
   formatted ASN.1 it will never or run off the end of the buffer and will
   generally return an error.  In rare cases it will return incorrect data
   where a pedantic ASN.1 parser would catch the error.

   The year handling in the dates conforms to RFC 2549/3280. For digit
   years in GeneralizedTime are handled. Two digit years in UTCTime less
   than 50 are consider 2000 and above; greater than or equal to 50 are 
   from 1950 up to 1999.

   The storage for the key has the lifetime of the IX509Chain or until the
   cert is removed from the chain.

   Note that this parser has a minor obscure requirement that the signature at 
   the end of the certificate be at least 4 bytes long or the cert will be
   considered invalid. 

Side Effects: 
   None 
   
Version:
   Introduced BREW Client 2.0

See Also:
   None    

=============================================================================

IX509CHAIN_GetField()

Description:
    This function gets a field out of an X.509 certificate.

Prototype:
    int IX509CHAIN_GetField
    (
    const IX509Chain *pMe, 
    int32 nCertWebOpt, 
    int32 nCertIndex, 
    int nField, 
    const uint8 **ppcField, 
    uint32 *puFieldLen
    );

Parameters:
    pMe :  The Interface pointer
    nCertWebOpt :  Which WebOpt ID set to get them from WEBOPT_X509_ROOT_CERT, _LEAF_CERT
or ... _BRANCH_CERT
    nCertIndex :  The index of certificate of the given WebOpt ID / type
    nField :  Which field in the certificate to get. One of X509CHAIN_FIELD_*
    ppcField :  Place to put pointer to field
    puFieldLen :  Place to length of field

Return Value:
    SUCCESS: if task is successful

    EFAILED:  couldn't find the certificate

    AEE_X509_BAD_CERT:  the certificate was NULL or can't be parsed

    ECLASSNOTSUPPORT:  don't have the hash function for the hash requested

    ENOMEMORY:  out of memory (couldn't instantiate hash function)

    EBADPARM:  requested non-existant field

    AEE_X509_NO_CHAIN_YET:  tried to get WEBOPT_X509_CHAIN_CERT before
    verify was called successfuly

Comments:
   This allows fetching most of the fields in a certificate. In most cases
   the part is unparsed. For example the ASN.1 DER encoding of the date,
   subject or key would be returned unparsed. The serial number is 
   parsed and just the actual serial number is returned. 
  
   The storage for the field has the lifetime of the IX509Chain or until the
   WebOpt storing the certificate is removed.

   This function can also return the MD5 or SHA1 hash over the entire cert.


   The following constants passed in the nField parameter specify
   which parts can be fetched:
 ~  
      X509CHAIN_FIELD_RAW_CERT   The full unparsed cert just as it was added  ~
      X509CHAIN_FIELD_SIGNED     The uparsed signed part of the cert~
      X509CHAIN_FIELD_SERIAL     The parsed-out serial number~
      X509CHAIN_FIELD_SIG_TYPE   The unparsed signature type. This is available
                                 in parsed for in X509BasicCert~
      X509CHAIN_FIELD_ISSUER     The unparsed Issuer DN. Use IX509CHAIN_GetFieldPart()
                                 to get this in parsed form.~
      X509CHAIN_FIELD_VALIDITY   Unparsed validity dates; This is available
                                 in parsed for in X509BasicCert~
      X509CHAIN_FIELD_SUBJECT    The unparsed Subject DN. Use IX509CHAIN_GetFieldPart()
                                 to get this in parsed form.~
      X509CHAIN_FIELD_PUBKEY     Unparsed public key; Use IX509CHAIN_GetRSAKey()
                                 or X509BasicCert for parsed key.~
      X509CHAIN_FIELD_EXTENSIONS The whole unprocessed extension block~
      X509CHAIN_FIELD_SIGN_ALG   Unparsed second instance of SIG_TYPE~
      X509CHAIN_FIELD_SIGNATURE  Unparsed signature by the isser over the cert~
      X509CHAIN_FIELD_MD5HASH    The MD5 hash over the entire cert~
      X509CHAIN_FIELD_SHAHASH    The SHA hash over the entire cert~
*

Side Effects: 
   None 
   
Version:
   Introduced BREW Client 2.0

See Also:
   None

=============================================================================


IX509CHAIN_GetFieldPart()

Description:
    This function gets individual extensions or parts out of the subject or issuer.

Prototype:
    int IX509Chain_GetObjectPart
    (
    const IX509Chain *pMe,
    const X509PartReq *pReq,
    X509Part *pPart
    );

Parameters:
    pMe: Instance of X509Chain
    pReq: The description of the request
    pPart: Filled in with the response.

Return Value:
    SUCCESS: if task is successful

    EFAILED:  couldn't find the certificate

    AEE_X509_BAD_CERT:  the certificate was NULL or can't be parsed

    EBADPARM:  asked for a field that we don't know about

    AEE_X509_NO_PART:  could find an instance of OID requested

    AEE_X509_NO_CHAIN_YET:  tried to get WEBOPT_X509_CHAIN_CERT before
    Verify was called

Comments: 
   While certificate extensions and subject and issuer parts aren't 
   semantically related they are similar enough in structure that this same
   function is used to get them.

   The descriptions above of the data structures describes how to get the
   different parts by OID or by ASN1OID (our local abbreviations for well
   known OIDs). 

   The storage for the field has the lifetime of the IX509Chain or until the
   certificate is removed from the chain.
  
   This can be used to iterate over all parts in a field, or over just the
   values for a particular OID (if there happens to me more than one instances
   of an OID; the OU part it often duplicated in a subject or issuer). To do
   the iteration fetch the 0th instance in a field.  The response will give
   you as bonus info, the number of instances in the field, and thus you
   know how far to iterate. You can also iterate until EFAILED is returned. 

   If you want all parts of a field give ASNOID_ANY as the OID to look for.


Version:
   Introduced BREW Client 2.0

See Also:
   None
=============================================================================

IX509CHAIN_GetRSAKey()

Description:
    This function gets an the RSA key as an IRSA object ready for use.

Prototype:
    int IX509Chain_GetKey
    (
    const IX509Chain *pMe, 
    int32 nCertWebOpt, 
    int32 nCertIndex, 
    IRSA **ppiRSA
    );

Parameters:
    pMe :  Instance of X509Chain
    nCertWebOpt :  Which WebOpt set to get them from WEBOPT_X509_ROOT_CERT, _LEAF_CERT
or ... _BRANCH_CERT
    nCertIndex :  The index of certificate of the given option type to get
    ppiRSA :  place to put pointer to newly created IRSA interface
    
Return Value:
    SUCCESS: if task is successful

    EFAILED               :  couldn't find the certificate

    AEE_X509_BAD_CERT     :  certificate parsed incorrectly

    AEE_X509_NO_CHAIN_YET :  tried to get WEBOPT_X509_CHAIN_CERT before IX509CHAIN_Verify()
                             was called or there was no leaf certificate

    AEE_X509_BAD_KEY      :  key can't be parsed out

    AEE_X509_UNKNOWN_KEY  :  key isn't RSA

    ENOMEMORY             :  Insufficient memory to store key and instantiate IRSA

    ECLASSNOSUPPORT       :  no RSA algorithms support on this device 

    EBADPARM              :  If null ppobj was passed in for piRSA

    AEE_CRYPT_INVALID_KEY :  parsed the key but still something wrong with it

Comments: 
   This returns the RSA key in the cert by instantiating an IRSA so it's ready
   for use. Be sure to release the IRSA instance when you're done.
 
Version:
   Introduced BREW Client 2.0

See Also:
   IRSA inteface

=============================================================================

Function: IX509CHAIN_Verify()

Description:
   Complete and verify the certificate chain
   
Prototype:
   void IX509CHAIN_Verify(IX509Chain *pMe, 509TrustResult *pTrustResult,
                          AEECallback *pVerifyCB, ...);

Parameters:
   pMe: Instance of X509Chain
   pTrustResult: Place to put trust result.
   pVerifyCB: called back when verification is complete.
   options_list: a set of web opts to use while doing VerifyPrototype
===pre>

    void IX509Chain_Verify
    (
    IX509Chain *pMe, 
    509TrustResult *pTrustResult,
    AEECallback *pVerifyCB, 
    ...
    );
===/pre>
Parameters:
    pMe :  Instance of X509Chain
    pTrustResult :  Place to put trust result.
    pVerifyCB :  Called back when verification is complete.
    ... options list :  A set of web opts to use while doing Verify

Return Value:
	None

Comments: 
   This does the heavy crypto to actually verify the signatures on the 
   certs up to a root in the set of roots. Dependending on key length, key
   modulus, processor speed and cert chain length, the time for this
   operation can vary.  For most cert chains it should be a couple of 
   seconds.

   First this builds the cert chain. It does this by finding the leaf cert
   and searching for cert that have a subject matching its issuer. The
   search is conducted over the options tree, first through the options
   passed into the Verify operation on the command line, second through 
   the options stored by the instance itself and on to any default
   options sets. The search is always first for root certs, then branch
   certs. Certs added sooner in the options tree are matched first. 

   When forming the certificate chain there are comparisons of isuer and subject.
   The comparison of issuer distinguished name (DN) to subject DN complies
   with RFC 2459 / 3280 section 6.1 and X.509 section 9. It ignores leading/
   trailing space and case for  attribute values (AVs) that are of type 
   PrintableString and does a binary compare for all other AVs. It is 
   sensitive to the structure/order of the relative distinguished names 
   (RDNs) that make up the DN, but not to the order of the AVs within a DN.
   So the following are equal:
~
    1) (O=fish-tacos, OU=fish, OU=halibut), (CO=mexico,ST=baja)
    2) (OU=halibut,O=fish-tacos, OU=fish) (ST=baja,CO=mexico)
*

   but these are not to each other or those above:
~
    3) (CO=mexico), (ST=baja), (OU=halibut), (O=fish-tacos, OU=fish)
    4) (O=fish-tacos, OU=fish), (OU=halibut), (CO=mexico), (ST=baja)
*

   Once the chain is formed, any certs that are part of the chain, but not
   stored in the instance of IX509Chain will be copied. This is so the
   chain is retained after the verify operation is completed so that it can
   be displayed or such.

   Next the chain is verified. This includes many checks such as validity
   dates, policy constraints imposed by extensions, the host name on the
   leaf certificate and the signatures. The verification does not stop on
   the first error. It continues over the whole computed cert chain. The
   rest of this section describes these checks.

   The errors are accumulated in the bit maps in the response structure.
   All these errors are "soft" trust errors that can be overriden. The
   hard errors are primarily out of memory or lack of a good leaf cert. 

   The individual checks can be turned off or overriden on a per certificate
   basis or for the whole chain verification with TrustOverride records.

   The host name, TrustOverride records and other options are stored in
   WebOpts that can either be stored in the instance itself, or be passed
   on the command line.  Note that by passing an instance of IWebOpts as 
   a new default, a whole tree of options can be passed in on the command
   line. 
 
   When the verify completes, the callback is called. You may cancel the
   verify operation by calling CALLBACK_Cancel() on the callback record passed
   to the verify operation. Note that releasing the IX509Chain object does
   not cancel the callback. The object will persist and the callback will
   complete. Thus the callback should be canceled before releasing the
   IX509Chain object (unless you want the callback to be called after
   the release). Note also that two verify operations on the same IX509Chain
   object may be started in parallel, though the usefulness of that
   seems questionable and it may be removed in the future.
  
   The results are copied into the pTrustResult structure. You must check
	both nResult and uErrorSummary to be get the complete results of the
	verify operation. 

   Once Verify is called you can fetch the certs in chain order using the
   WEBOPT_X509_CHAIN_CERT option type. The ordering is generally valid 
   until you call RemoveOpt, after which it is not guaranteed until you
   call Verify again.

   Note that if you reuse this object for verifying multiple chains by
   adding and deleting the leaf and branch certs, and you are passing the
   roots in as an option parameter, the number of copied roots
   will grow.

   The host name checking is as follows. It is expected that a hostname to match
   against is supplied in the WEBOPT_X509_HOST option. In a typical browser 
   SSL implementation this is the hostname from the URL. If one is not supplied
   an error will be set. Hostname checking is mainly for use with SSL. If you do
   not want the hostname checked then set X509CHAIN_NO_TARGET_HOST 
   as a TrustOverride. 

   The comparrison is to the common name part of the subject distinguished name
   in the leaf certificate. Regular expressions in the CN (common name) are catered
   for as per the following rules defined by Netscape:
   ===pre>
      * matches anything
      ? matches one character
      \ will escape a special character
      $ matches the end of the string
      [abc] matches one occurrence of a, b, or c. The only character that needs 
      to be escaped in this is ], all others are not special.
      [a-z] matches any character between a and z [^az] matches any character 
      except a or z ~ followed by another expression will remove any 
      pattern matching the shell expression from the match list
      (foo|bar) will match either the substring foo, or the substring bar. These 
      can be expressions as well.
===/pre>
   The basic constraints extension is processed as follows. It is ignored
   on the leaf certificate and on all v1 certificates.  On the intermediate
   and root certificates its presence is required, and the value of the cA 
   boolean must be true. If not the error X509CHAIN_BASIC_CONST will be given.
   Also, the pathLen is imposed. If the path is too long, X509CHAIN_LONG_CHAIN
   is given. The criticality of the basic constraints has no affect on its
   processing.
    
   To turn off all basic constraints processing set X509CHAIN_BASIC_CONST as an
   override.

   Brew 2.x processes basic constraints slightly different from Brew 3.0 and higher.
   It ignores basic constraints on root certificates completely. Brew 3.0 
   instead does not flag errors on v1 certificates no matter where they appear
   in the chain. One result is that a root certificate with cA=FALSE will cause
   an error in 3.0, but not 2.x. Another result is that a v1 intermediate
   certificate will cause an error in 2.x, but not 3.0.
   
   The X509v3 Key Use extension is processed as follows. If the extension
   is not marked as critical, it is completely ignored. If it occurs in
   a branch or root (not the leaf) and is critical then it must contain
   ASNOID_KEYUSE_CERTSIGN or the trust error X509CHAIN_KEY_USE will 
   be given. If it occurs in a leaf cert then there must be matching values
   in the extension for every usage bit set in the option WEBOPT_X509_KEY_USE
   or the trust error X509CHAIN_KEY_USE will be set.

   Extended key use processing is turned on if any WebOpts with ID 
	WEBOPT_X509_EXT_KEY_USE are set. If set then certificates in the chain
	with extendedKeyUse extensions are processed against it. No processing is done 
	for certificates that don't have an extendedKeyUse as the implication is there
	is no restriction on their usage.

   By default only the leaf cert is processed. To have the extension processed in
   other than the leaf set the trust override bit X509CHAIN_NON_LEAF_EXT_KEY_USE.
   It is not recommeded that this be set for SSL.

   In processing, the extendedKeyUse section of each cert must include the 
	OIDs for all of the usages given in the WEBOPT_X509_EXT_KEY_USE web opts. If
	one is missing an error is returned. Certs without extendedKeyUse never 
	generate an error.  The error returned will be X509CHAIN_EXT_KEY_USE if the 
	extension is not critical and X509CHAIN_CRIT_EXT_KEY_USE if it is.

   To avoid processing entirely, do not set any WebOpts with ID WEBOPT_X509_EXT_KEY_USE.
	To process them only if they are critical, set the WebOpts and the trust
	override X509CHAIN_EXT_KEY_USE. Setting X509CHAIN_CRIT_EXT_KEY_USE as a trust
	override is the same as not setting any WebOpts with ID WEBOPT_X509_EXT_KEY_USE.
	
   In Brew 2.0 up to Brew 3.0.1, the X509v3 Extended Key Use extension was processed 
	as follows. It is processed whether or not it is marked critical.  If it occured in
	a root or branch cert (not the leaf) and it is critical the X509CHAIN_EXT_KEY_USE
   error is set. If it occurs in the leaf cert then there must be a matching
   usage value in the extension for every usage value given in 
   WEBOPT_X509_EXT_KEY_USE or the X509CHAIN_CRIT EXT_KEY_USE will
   be set. If it is marked critical on a leaf cert the X509CHAIN_CRIT_EXT_KEY_USE
   error will be set. To override all process set X509CHAIN_CRIT_EXT_KEY_USE. 
   To override it only if it's not critical set X509CHAIN_EXT_KEY_USE.

   There are many more types of extensions in certificates. They are all ignored.
   If you wish you may use the IX509Chain_GetFieldPart() function to get and
   process them. If an extenion is marked critical and not understood, then 
   the X509CHAIN_CRITICAL_EXTENSION will be set. 

   The following summarizes the WebOpt IDs that affect chain verification. Also
   see the WebOpt IDs under IX509CHAIN_AddOpt() for the certificate types.
   ~

      WEBOPT_X509_HOST          Set the hostname to compare with the hostname
                                in the common name in the leaf cert in the 
                                chain. It must be a NULL terminated string
                                though the host part that is actually compared
                                stops at the first ":", "/" or "," character. 
                                If this is unset, a pedantic but overridable
                                trust error  will be set. 

      WEBOPT_X509_OVERRIDE      You can have serveral of these to override
                                serveral trust errors. See above discussion
                                of X509TrustOverride for details.

      WEBOPT_X509_MAX_CHAIN     Maximum length we'll chain. Won't chain past
                                X509CHAIN_MAX_CHAIN no matter how large this
                                value is. This is NOT the maximum number of
                                certs that can be  held in IX509Chain. That 
                                is limited only by memory.

      WEBOPT_X509_EXT_KEY_USE   A set of ASN1OIDs (see AEEASN1OIDs.h) that 
                                indicated the intended purpose of the cert 
                                chain. There may be several values for this 
                                option, with one purpose given as a 32 bit 
                                integer in pVal per option. If unset no 
                                checking of the extended key usage extension
                                in the cert will be  performed.  An example 
                                value would be ASNOID_EXTKEYUSE_SRVRAUTH (SSL
                                server authentication)

      WEBOPT_X509_KEY_USE       A single bit map giving the intended use of 
                                the cert should be place in pVal of this opt. 
                                The bit values should be one of 
                                ASNOID_KEYUSE_*.

      WEBOPT_X509_RSA_IMPLEMENTATION 
                                This option allows the client to specify the 
                                RSA implementation to be used. For example, 
                                some platforms may allow support for hardware
                                based modular exponentation, and the client
                                may use this option to take advantage of this
                                capability in order to improve performance.                               

      WEBOPT_X509_INTEGER_ID    This may be used to check that an integer value or 
                                set of integer values falls within the range 
                                specified in the set-of-integers extension. This is 
                                a Brew-specific extension.
*

Side Effects:
   Causes cert chain to be formed so that certs, cert fields, and cert
   field parts can be fetched in cert chain order using WEBOPT_X509_CHAIN_CERT.

Version:
   Introduced BREW Client 2.0

See Also:
   X509TrustResult
   X509TrustOverride
   IX509CHAIN_AddOpt()

=============================================================================  

IX509CHAIN_VerifyV()

Description:
    This function completes and verify the certificate chain

Prototype:
    void IX509Chain_VerifyV
    (
    IX509Chain *pMe, 
    X509TrustResult *pTrustResult,
    AEECallback *pVerifyCB, 
    const WebOpt *awozList
    );

Parameters:
    pMe :  Instance of X509Chain
    pTrustResult :  Place to put trust result.
    pVerifyCB :  Called back when verification is complete.
    awozList :  Set of web opts to use while doing Verify

Return Value:
	None

Comments:
    See the discussion under Comments: in the IX509CHAIN_Verify()

Version:
   Introduced BREW Client 2.0

See Also:
    None

=============================================================================*/

#endif /*  AEEX509Chain_H */      
   

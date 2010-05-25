#ifndef AEEASN1OIDS_H
#define AEEASN1OIDS_H
/*======================================================

FILE:  AEEASN1OIDS.h

SERVICES:
        OIDs for ASN.1 stuff (X.509 certs)

GENERAL DESCRIPTION:
        Constants for mappings of OIDs commonly used in X.509 certs

=====================================================

      Copyright © 2002 QUALCOMM Incorporated 
              All Rights Reserved.
           QUALCOMM Proprietary/GTDR

=====================================================*/

/*
      
   See http://www.alvestrand.no/objectid/ for details on these OIDS 
    
   ASN.1 spaces we collapse into some IDs for convenience when fetching
   some cert parts. The short answer: pick the value below you want
   and pass it to X509Chain_GetFieldPart.
   
   There are some limited number of OID sub-spaces that are interesting to
   us. In each of those spaces a few to maybe a few hundred OID's are of interest.
   Thus we divide up a 32 bit integeter with the upper 8 bits representing
   a sub space, and the lower 24 the values in the subspace. 

   The thing that makes this work is the last byte of an OID is not encoded
   in weird ways like the others so we can pattern match down to a subspace
   and then just OR in the last byte to get the values below (if that makes sense)

   Because there's so many OID's that fit into these spaces that are probably
   of interest to a only a few humans on the planet, we don't define constants
   for all of them, but you if you happen to be an interested human, you can still
   take advantage of this code with the ASNOID_MATCH_SUBSPACE macro.
 */
#define ASNOID_UNDEFINED  0x00        // Unknown to us, good luck and have fun!
#define ASNOID_SUBSPACES  0xff000000  // Mask for subspaces
#define ASNOID_MATCH_SUBSPACE(oid, subspace) ((oid & ASNOID_SUBSPACES) == subspace)

#define ASNOID_UNKNOWN    0xff000001
#define ASNOID_ANY        0xff000002

// 2.5.4 This is the joint ITU/ISO X.500 attribute tree and
// encompases things used in distinguished names
// Binary DER prefix \x06\0x03\x55\x04
#define ASNOID_X500     0x01000000 // The X.500 subspace of ASNOIDs
#define ASNOID_X500_CN  0x01000003 // 2.5.4.3  commonName
#define ASNOID_X500_CO  0x01000006 // 2.5.4.6  country
#define ASNOID_X500_LO  0x01000007 // 2.5.4.7  locality Name
#define ASNOID_X500_ST  0x01000008 // 2.5.4.8  stateOrProvinceName
#define ASNOID_X500_O   0x0100000A // 2.5.4.10 organizationName
#define ASNOID_X500_OU  0x0100000B // 2.5.4.11 organizationUnitName

// 1.2.840.113549.1.1 This is PKCS1 space. Part of PKCS, which is part
// of RSADSI, which is part of the US which is an ISO member body...
// Binary DER prefix is \x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01
#define ASNOID_PKCS1            0x02000000 // The PKCS1 subspace of ASNOIDS
#define ASNOID_PKCS1_RSA        (ASNOID_PKCS1 |  1) // 1.2.840.113549.1.1.1 rsaEncryption
#define ASNOID_PKCS1_RSA_MD2    (ASNOID_PKCS1 |  2) // 1.2.840.113549.1.1.2 MD2 with rsaEncryption
#define ASNOID_PKCS1_RSA_MD5    (ASNOID_PKCS1 |  4) // 1.2.840.113549.1.1.4 md5withRSAEncryption
#define ASNOID_PKCS1_RSA_SHA1   (ASNOID_PKCS1 |  5) // 1.2.840.113549.1.1.5 sha1withRSAEncryption
#define ASNOID_PKCS1_RSA_SHA256 (ASNOID_PKCS1 | 11) // 1.2.840.113549.1.1.11 sha256WithRSAEncryption
#define ASNOID_PKCS1_RSA_SHA384 (ASNOID_PKCS1 | 12) // 1.2.840.113549.1.1.12 sha384WithRSAEncryption
#define ASNOID_PKCS1_RSA_SHA512 (ASNOID_PKCS1 | 13) // 1.2.840.113549.1.1.13 sha256WithRSAEncryption
#define ASNOID_PKCS1_RSA_SHA224 (ASNOID_PKCS1 | 14) // 1.2.840.113549.1.1.14 sha256WithRSAEncryption


// 1.3.14.3.2 This is secsig subspace, part of OIW, part of ISO orgs
// Seems like lots of crypto algorithms identified here. Might be worth
// adding more defines than we have...
// Binary DER prefix is \x06\x05\x2b\x0e\x03x02
#define ASNOID_SECSIG         0x03000000 // The secsig subspace of ASNOIDS
#define ASNOID_SECSIG_SHA1    0x0300001A // 1.3.14.3.2.26 SHA-1 hash

// 2.5.29. This is the joint ITU/ISO X.500 directory tree area for certificate extensions
// There is probably more here that would be useful to define constants for
// Binary DER prefix is \x06\x03\x55\x1d
#define ASNOID_CERTEXT              0x04000000 // The cert extensions subspace of ASNOIDS
#define ASNOID_CERTEXT_POLICIES     0x04000003 // 2.5.29.3  Certificate policies
#define ASNOID_CERTEXT_SUBJKEY_ID   0x0400000E // 2.5.29.14 Subject Key Identifier
#define ASNOID_CERTEXT_KEY_USE      0x0400000F // 2.5.29.15 Key usage
#define ASNOID_CERTEXT_SUBJALTNAME  0x04000010 // 2.5.29.16 Subject Alternate name
#define ASNOID_CERTEXT_ISSALTNAME   0x04000011 // 2.5.29.17 Issuer Alternate name
#define ASNOID_CERTEXT_B_CONSTRAIN  0x04000013 // 2.5.29.19 Basic constraints
#define ASNOID_CERTEXT_CRLDIT       0x0400001F // 2.5.29.31 CRL distribution points
#define ASNOID_CERTEXT_POLICIES2    0x04000020 // 2.5.29.32 Cert policies
#define ASNOID_CERTEXT_AUTHKEY_ID   0x04000023 // 2.5.29.35 Authority Key Identifier
#define ASNOID_CERTEXT_EXT_KEY_USE  0x04000025 // 2.5.29.37 Extended Key usage

// 2.16.840.1.113730.1 ISO/ITU joint assignments by country - USA companies, Netscape
// Netscape certificate extensions
// Binary DER prefix is \x06\x09\x60\x86\x48\x01\x86\xf8\x42\x01
#define ASNOID_NETSCAPE_CERT_EXT      0x05000000  // Netscape substapce of ASNOIDS
#define ASNOID_NETSCAPE_CERT_EXT_TYPE 0x05000001  // 2.16.840.1.113730.1.1 - Netscape cert type
#define ASNOID_NETSCAPE_CERT_EXT_GEN  0x0500000D  // 2.16.840.1.113730.1.13 - Generated by

// 2.16.840.1.113733.1.7.1 ISO/ITU joint assignments by country - USA companies, Verisign
// Certificate policy
// Binary DER prefix is \x06\x0B\x60\x86\x48\x01\x86\xf8\x45\x01\x07\x01
#define ASNOID_VERISIGN_POLICY        0x06000000  // Verisign Certificate Policy
#define ASNOID_VERISIGN_POLICY_QUAL1  0x06000001  // Verisign Policy Qualifier
#define ASNOID_VERISIGN_POLICY_QUAL2  0x06000002  // Verisign Policy Qualifier

// 1.3.6.1.4.1.311.10.3 ISO identified orgs, US DoD, Internet assignments, private,
// IANA regestered enterprises, Microsoft
// Binary DER prefix is \x06\x0A\x2b\x06\x01\x04\x01\x82\x37\x01\x03
#define ASNOID_MICROSOFT              0x07000000  // A subspace of Microsoft
#define ASNOID_MICROSOFT_SGC          0x07000003  // Sever Gated Cryptography

// 2.16.840.1.113730.4 ISO/ITU joint assignments by country - USA companies, Netscape
// International Step up
// Binary DER prefix is \x06\x09\x60\x86\x48\x01\x86\xf8\x42\x04
#define ASNOID_NETSCAPE_STEP        0x08000000  // Netscape subpace of ASNOIDS
#define ASNOID_NETSCAPE_STEP_UP     0x08000001  // 2.16.840.1.113730.4 - International step up

// 1.2.840.113549.1.9 This is PKCS9 space. Part of PKCS, which is part
// of RSADSI, which is part of the US which is an ISO member body...
// Binary DER prefix is \x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x09
#define ASNOID_PKCS9          0x09000000 // The PKCS9 subspace of ASNOIDS
#define ASNOID_PKCS9_EMAIL    0x09000001 // 1.2.840.113549.1.9.1 email address

// 1.3.6.1.5.5.7.1 This ISO identified orgs, US Dod, Internet assignments,
// IANA security, mechanisms, pkix, private cert extensions
// Binary DER prefix is \x06\x08\x2B\x06\x01\x05\x05\x07\x01
#define ASNOID_PKIXPRIV           0x0a000000 // The Private cert extension PKIX subspace of ASNOIDS
#define ASNOID_PKIXPRIV_AUTHINFO  0x0a000001 // 1.3.6.1.5.5.7.1.1 The Authority Info Access field

// 2.16.840.1.113733.1.6 ISO/ITU joint assignments by country - USA companies, Verisign
// Binary DER prefix is \x06\x0a\x60\x86\x48\x01\x86\xf8\x45\x01\x06
#define ASNOID_VERISIGNPRIV          0x0b000000  // Verisign privately assigned
#define ASNOID_VERISIGNPRIV_BREWPERM 0x0b000011  // Brew digital sig permissions

// 1.2.840.113549.2 These are digest algorithms in PKCS1 space. Part of PKCS, which is part
// of RSADSI, which is part of the US which is an ISO member body...
// Binary DER prefix is  \x06\x08\x2a\x86\x48\x86\xf7\x0d\x02
#define ASNOID_PKCS1DIGEST         0x0c000000 
#define ASNOID_PKCS1DIGEST_MD2     0x0c000002 // The MD2 algorithm
#define ASNOID_PKCS1DIGEST_MD5     0x0c000005 // The MD5 algorithm
//      The following OID is wrong, RSA did not define an OID
//      for SHA-1, it defined an OID for HMAC-SHA1.
#define ASNOID_PKCS1DIGEST_SHA1    0x0c000007 // DO NOT USE with new software
#define ASNOID_PKCS1DIGEST_HMACSHA1 0x0c000007 // The HMAC-SHA1 algorithm

// 1.3.6.1.5.5.7.3 This ISO identified orgs, US Dod, Internet assignments,
// IANA security, mechanisms, pkix, extended key purpose
// Binary DER prefix is \x06\x08\x2B\x06\x01\x05\x05\x07\x03
#define ASNOID_EXTKEYUSE           0x0d000000 // Extened key use of PKIX subspace of ASNOIDS
#define ASNOID_EXTKEYUSE_SRVRAUTH  0x0d000001 // 1.3.6.1.5.5.7.3.1 TLS Server Authentication
#define ASNOID_EXTKEYUSE_CLNTAUTH  0x0d000002 // 1.3.6.1.5.5.7.3.2 TLS Client Authentication
#define ASNOID_EXTKEYUSE_CODESIGN  0x0d000003 // 1.3.6.1.5.5.7.3.3 Code Signing
#define ASNOID_EXTKEYUSE_EMAIL     0x0d000004 // 1.3.6.1.5.5.7.3.4 Email Protection
#define ASNOID_EXTKEYUSE_TIMESTMP  0x0d000005 // 1.3.6.1.5.5.7.3.5 Time Stamping
#define ASNOID_EXTKEYUSE_IPSEC     0x0d000007 // 1.3.6.1.5.5.7.3.7 IPsec

// 1.3.6.1.4.1.1449.9.3 This ISO identified orgs, US Dod, Internet assignments,
// IANA regestered enterprises, Qualcomm, QIS, Brew private cert hierarchy X.509 extensions
// Binary DER prefix is \x06\x08\x2B\x06\x01\x04\x01\xb8\x29\x09\x03
#define ASNOID_BREWX509             0x0e000000 // Brew private X.509 extensions
#define ASNOID_BREWX509_CARRIERID   0x0e00000a // 1.3.6.1.4.1.1449.9.3.10 Brew-assigned carriers IDs
// Note that ASNOID_VERISIGNPRIV_BREWPERM above is the former incarnation of this
#define ASNOID_BREWX509_PLATFORMID  0x0e00000b // 1.3.6.1.4.1.1449.9.3.11 Brew-assigned platform IDs
#define ASNOID_BREWX509_BREWCLASSID 0x0e00000c // 1.3.6.1.4.1.1449.9.3.12 Brew class IDs
// -- following are extended key usage in Brew --
#define ASNOID_BREWX509_BREWAPP     0x0e000001 // 1.3.6.1.4.1.1449.9.3.1 Brew extended key usage, app signed permissions
#define ASNOID_BREWX509_TESTENABLE  0x0e000002 // 1.3.6.1.4.1.1449.9.3.2 Brew test enable signature
#define ASNOID_BREWX509_ADSAUTH     0x0e000003 // 1.3.6.1.4.1.1449.9.3.3 Authentication of ADS's
#define ASNOID_BREWX509_TRUEBREW    0x0e000004 // 1.3.6.1.4.1.1449.9.3.4 True Brew


// 2.16.840.1.101.3.4.2 These are the NIST digest algorithms
#define ASNOID_NIST_DIGEST          0x0F000000 // NIST Digest algorithms subspace of ASNOIDS
#define ASNOID_NIST_DIGEST_SHA256   (ASNOID_NIST_DIGEST | 1)
#define ASNOID_NIST_DIGEST_SHA384   (ASNOID_NIST_DIGEST | 2)
#define ASNOID_NIST_DIGEST_SHA512   (ASNOID_NIST_DIGEST | 3)
#define ASNOID_NIST_DIGEST_SHA224   (ASNOID_NIST_DIGEST | 4)

// These are not actually ASNOIDs because they're not integers representing OIDS, but
// they're used and defined in the same context. These are the bit position or named
// bit values for KeyUse.  
#define ASNOID_KEYUSE_DIGSIG       0x0000001
#define ASNOID_KEYUSE_NONREP       0x0000002
#define ASNOID_KEYUSE_KEYENC       0x0000004
#define ASNOID_KEYUSE_DATAENC      0x0000008
#define ASNOID_KEYUSE_KEYAGREE     0x0000010
#define ASNOID_KEYUSE_CERTSIGN     0x0000020
#define ASNOID_KEYUSE_CRLSIGN      0x0000040
#define ASNOID_KEYUSE_ENCONLY      0x0000080
#define ASNOID_KEYUSE_DECONLY      0x0000100



/* ----------------------------------------------------------------------
    A few constants for ASN1 type encoding. These come from ASN1 itself
   --- */
#define ASN1_BIT_STRING_TYPE       (0x03)
#define ASN1_INTEGER_TYPE          (0x02)
#define ASN1_BOOLEAN_TYPE          (0x01)
#define ASN1_OID_TYPE              (0x06)
#define ASN1_SEQUENCE_TYPE         (0x30)
#define ASN1_DATE_TYPE             (0x17)
#define ASN1_UTCTIME               (0x17)
#define ASN1_GENERALTIME           (0x18)
#define ASN1_SET_TYPE              (0x31)
#define ASN1_PRINTABLE_STRING_TYPE (0x13)
#define ASN1_TELETEX_STRING_TYPE   (0x14)
#define ASN1_OPTIONAL_0            (0xa0)
#define ASN1_OPTIONAL_1            (0xa1)
#define ASN1_OPTIONAL_2            (0xa2)
#define ASN1_OPTIONAL_3            (0xa3)
#endif /*  AEEASN1OIDS_H */

/*=============================================================================
     DATA STRUCTURE DOCUMENTATION
===============================================================================
AEEASN1OIDS 

ASN.1 spaces are collapsed into some IDs for convenience when fetching some certificate
parts. The short answer: Pick the value below you want and pass it to X509Chain_GetFieldPart.

There are some limited number of OID subspaces that are interesting to us. In each
of those spaces, a few to maybe a few hundred OIDs are of interest.

Thus we divide up a 32-bit integer with the upper 8 bits representing a subspace,
and the lower 24 the values in the subspace.

The thing that makes this work is that the last byte of an OID is not encoded in weird
ways like the others so we can pattern match down to a subspace and then just OR
in the last byte to get the values below (if that makes sense)

Because there are so many OIDs that fit into these spaces that are probably of interest
to a only a few humans on the planet, we don't define constants for all of them,
but you if you happen to be an interested human, you can still take advantage of
this code with the ASNOID_MATCH_SUBSPACE macro.
===pre>

ASNOID_UNDEFINED        0x00        Unknown to us, good luck and have fun!
 
ASNOID_SUBSPACES        0xff000000  Mask for subspaces
 
ASNOID_MATCH_SUBSPACE(oid, subspace) ((oid & ASNOID_SUBSPACES) == subspace)

ASNOID_UNKNOWN 0xff000001

ASNOID_ANY 0xff000002

2.5.4 This is the joint ITU/ISO X.500 attribute tree and encompasses things used
in distinguished names

Binary DER prefix \x06\0x03\x55\x04


ASNOID_X500             0x01000000  The X.500 subspace of ASNOIDs
 
ASNOID_X500_CN          0x01000003  2.5.4.3 commonName
 
ASNOID_X500_CO          0x01000006  2.5.4.6 country
 
ASNOID_X500_LO          0x01000007  2.5.4.7 locality Name
 
ASNOID_X500_ST          0x01000008  2.5.4.8 stateOrProvinceName
 
ASNOID_X500_O           0x0100000A  2.5.4.10 organizationName
 
ASNOID_X500_OU          0x0100000B  2.5.4.11 organizationUnitName
 
1.2.840.113549.1.1 This is PKCS1 space. Part of PKCS, which is part of RSADSI, which
is part of the US which is an ISO member body...

Binary DER prefix is \x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01

ASNOID_PKCS1            0x02000000  The PKCS1 subspace of ASNOIDS
 
ASNOID_PKCS1_RSA        0x02000001  1.2.840.113549.1.1.1 rsaEncryption
 
ASNOID_PKCS1_RSA_MD2    0x02000002  1.2.840.113549.1.1.2 MD2 with rsaEncryption
 
ASNOID_PKCS1_RSA_MD5    0x02000004  1.2.840.113549.1.1.4 md5withRSAEncryption
 
ASNOID_PKCS1_RSA_SHA1   0x02000005  1.2.840.113549.1.1.5 sha1withRSAEncryption

1.3.14.3.2 This is secsig subspace, part of OIW, part of ISO orgs Seems like lots
of crypto algorithms identified here. Might be worth adding more defines than we
have...

Binary DER prefix is \x06\x05\x2b\x0e\x03x02

ASNOID_SECSIG           0x03000000  The secsig subspace of ASNOIDS
 
ASNOID_SECSIG_SHA1      0x0300001A  1.3.14.3.2.26 SHA-1 hash
 

2.5.29. This is the joint ITU/ISO X.500 directory tree area for certificate extensions

There is probably more here that would be useful to define constants for

Binary DER prefix is \x06\x03\x55\x1d

ASNOID_CERTEXT             0x04000000  The certificate extensions subspace of ASNOIDS
 
ASNOID_CERTEXT_POLICIES    0x04000003  2.5.29.3 Certificate policies
 
ASNOID_CERTEXT_SUBJKEY_ID  0x0400000E  2.5.29.14 Subject Key Identifier
 
ASNOID_CERTEXT_KEY_USE     0x0400000F  2.5.29.15 Key usage
 
ASNOID_CERTEXT_SUBJALTNAME 0x04000010  2.5.29.16 Subject Alternate name
 
ASNOID_CERTEXT_ISSALTNAME  0x04000011  2.5.29.17 Issuer Alternate name
 
ASNOID_CERTEXT_B_CONSTRAIN 0x04000013  2.5.29.19 Basic constraints
 
ASNOID_CERTEXT_CRLDIT      0x0400001F  2.5.29.31 CRL distribution points
 
ASNOID_CERTEXT_POLICIES2   0x04000020  2.5.29.32 Certificate policies
 
ASNOID_CERTEXT_AUTHKEY_ID  0x04000023  2.5.29.35 Authority Key Identifier
 
ASNOID_CERTEXT_EXT_KEY_USE 0x04000025  2.5.29.37 Extended Key usage

2.16.840.1.113730.1 ISO/ITU joint assignments by country - USA companies, Netscape

Netscape certificate extensions

Binary DER prefix is \x06\x09\x60\x86\x48\x01\x86\xf8\x42\x01

ASNOID_NETSCAPE_CERT_EXT      0x05000000  Netscape substapce of ASNOIDS
 
ASNOID_NETSCAPE_CERT_EXT_TYPE 0x05000001  2.16.840.1.113730.1.1 - Netscape certificate type
 
ASNOID_NETSCAPE_CERT_EXT_GEN  0x0500000D  2.16.840.1.113730.1.13 - Generated by
 
2.16.840.1.113733.1.7.1 ISO/ITU joint assignments by country - USA companies, Verisign
Certificate policy

Binary DER prefix is \x06\x0B\x60\x86\x48\x01\x86\xf8\x45\x01\x07\x01

ASNOID_VERISIGN_POLICY        0x06000000  Verisign Certificate Policy
 
ASNOID_VERISIGN_POLICY_QUAL1  0x06000001  Verisign Policy Qualifier
 
ASNOID_VERISIGN_POLICY_QUAL2  0x06000002  Verisign Policy Qualifier

1.3.6.1.4.1.311.10.3 ISO identified orgs, US DoD, Internet assignments, private,
IANA regestered enterprises, Microsoft

Binary DER prefix is \x06\x0A\x2b\x06\x01\x04\x01\x82\x37\x01\x03

ASNOID_MICROSOFT              0x07000000  A subspace of Microsoft
 
ASNOID_MICROSOFT_SGC          0x07000003  Sever Gated Cryptography
 
2.16.840.1.113730.4 ISO/ITU joint assignments by country - USA companies, Netscape
International Step up.

Binary DER prefix is \x06\x09\x60\x86\x48\x01\x86\xf8\x42\x04

ASNOID_NETSCAPE_STEP          0x08000000  Netscape subpace of ASNOIDS
 
ASNOID_NETSCAPE_STEP_UP       0x08000001  2.16.840.1.113730.4 - International step up
 
1.2.840.113549.1.9 This is PKCS9 space. Part of PKCS, which is part of RSADSI, which
is part of the US which is an ISO member body...

Binary DER prefix is \x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x09

ASNOID_PKCS9                  0x09000000  The PKCS9 subspace of ASNOIDS
 
ASNOID_PKCS9_EMAIL            0x09000001  1.2.840.113549.1.9.1 email address

1.3.6.1.5.5.7.1 This ISO identified orgs, US Dod, Internet assignments, IANA security,
mechanisms, pkix, private certificate extensions

Binary DER prefix is \x06\x08\x2B\x06\x01\x05\x05\x07\x01

ASNOID_PKIXPRIV               0x0a000000  The Private certificate extension PKIX subspace of ASNOIDS
 
ASNOID_PKIXPRIV_AUTHINFO      0x0a000001  1.3.6.1.5.5.7.1.1 The Authority Info Access field
 
2.16.840.1.113733.1.6 ISO/ITU joint assignments by country - USA companies, Verisign

Binary DER prefix is \x06\x0a\x60\x86\x48\x01\x86\xf8\x45\x01\x06

ASNOID_VERISIGNPRIV           0x0b000000  Verisign privately assigned
 
ASNOID_VERISIGNPRIV_BREWPERM  0x0b000011  Brew digital sig permissions

1.2.840.113549.2 These are digest algorithms in PKCS1 space. Part of PKCS, which
is part of RSADSI, which is part of the US which is an ISO member body...

Binary DER prefix is \x06\x08\x2a\x86\x48\x86\xf7\x0d\x02

ASNOID_PKCS1DIGEST            0x0c000000
  
ASNOID_PKCS1DIGEST_MD2        0x0c000002  The MD2 algorithm
 
ASNOID_PKCS1DIGEST_MD5        0x0c000005  The MD5 algorithm

This OID is wrong, RSA did not define an OID for SHA-1, this value
is for HMAC-SHA1.
ASNOID_PKCS1DIGEST_SHA1       0x0c000007  Deprecated, do not use for new programs.

ASNOID_PKCS1DIGEST_HMACSHA1   0x0c000007  The HMAC-SHA1 algorithm

1.3.6.1.5.5.7.3 This ISO identified orgs, US Dod, Internet assignments, IANA security,
mechanisms, pkix, extended key purpose.

Binary DER prefix is \x06\x08\x2B\x06\x01\x05\x05\x07\x03

ASNOID_EXTKEYUSE              0x0d000000  Extened key use of PKIX subspace of ASNOIDS
 
ASNOID_EXTKEYUSE_SRVRAUTH     0x0d000001  1.3.6.1.5.5.7.3.1 TLS Server Authentication
 
ASNOID_EXTKEYUSE_CLNTAUTH     0x0d000002  1.3.6.1.5.5.7.3.2 TLS Client Authentication
 
ASNOID_EXTKEYUSE_CODESIGN     0x0d000003  1.3.6.1.5.5.7.3.3 Code Signing
 
ASNOID_EXTKEYUSE_EMAIL        0x0d000004  1.3.6.1.5.5.7.3.4 Email Protection
 
ASNOID_EXTKEYUSE_TIMESTMP     0x0d000005  1.3.6.1.5.5.7.3.5 Time Stamping
 
ASNOID_EXTKEYUSE_IPSEC        0x0d000007  1.3.6.1.5.5.7.3.7 IPsec

1.3.6.1.4.1.1449.9.3 This is ISO identified orgs, US Dod, Internet assignments,
IANA regestered enterprises, Qualcomm, QIS, Brew private cert hierarchy X.509 extensions.

These are OIDs used in Brew private extensions for the Brew app signing system and ADS authentication

Binary DER prefix is \x06\x08\x2B\x06\x01\x04\x01\xb8\x29\x09\x03

ASNOID_BREWX509               0x0e000000  1.3.6.1.4.1.1449.9.3     Brew private X.509 extensions

ASNOID_BREWX509_CARRIERID     0x0e00000a  1.3.6.1.4.1.1449.9.3.10  Brew-assigned carriers IDs. See 
                                                                   also ASNOID_VERISIGNPRIV_BREWPERM

ASNOID_BREWX509_PLATFORMID    0x0e00000b  1.3.6.1.4.1.1449.9.3.11  Brew-assigned platform IDs

ASNOID_BREWX509_BREWCLASSID   0x0e00000c  1.3.6.1.4.1.1449.9.3.12  Brew class IDs

ASNOID_BREWX509_BREWAPP       0x0e000001  1.3.6.1.4.1.1449.9.3.1   Extened key usage. Per-carrier
                                                                   signed Brew app

ASNOID_BREWX509_TESTENABLE    0x0e000002  1.3.6.1.4.1.1449.9.3.2   Extened key usage. Brew test 
                                                                   enable signature

ASNOID_BREWX509_ADSAUTH       0x0e000003  1.3.6.1.4.1.1449.9.3.3   Extened key usage. Authentication
                                                                   of ADS's

ASNOID_BREWX509_TRUEBREW      0x0e000004  1.3.6.1.4.1.1449.9.3.4   Extened key usage. True Brew Tested
 
2.16.840.1.101.3.4.2 These are the NIST digest algorithms

ASNOID_NIST_DIGEST            0x0F000000 // NIST Digest algorithms subspace of ASNOIDS
ASNOID_NIST_DIGEST_SHA256     (ASNOID_NIST_DIGEST | 1)
ASNOID_NIST_DIGEST_SHA384     (ASNOID_NIST_DIGEST | 2)
ASNOID_NIST_DIGEST_SHA512     (ASNOID_NIST_DIGEST | 3)
ASNOID_NIST_DIGEST_SHA224     (ASNOID_NIST_DIGEST | 4)

These are not actually ASNOIDs because they're not integers representing OIDS, but
they're used and defined in the same context. These are the bit position or named
bit values for KeyUse.

ASNOID_KEYUSE_DIGSIG          0x0000001
 
ASNOID_KEYUSE_NONREP          0x0000002
 
ASNOID_KEYUSE_KEYENC          0x0000004
 
ASNOID_KEYUSE_DATAENC         0x0000008
 
ASNOID_KEYUSE_KEYAGREE        0x0000010
 
ASNOID_KEYUSE_CERTSIGN        0x0000020
 
ASNOID_KEYUSE_CRLSIGN         0x0000040
 
ASNOID_KEYUSE_ENCONLY         0x0000080
 
ASNOID_KEYUSE_DECONLY         0x0000100


A few constants for ASN1 type encoding. These come from ASN1 itself

ASN1_BIT_STRING_TYPE          (0x03)
 
ASN1_INTEGER_TYPE             (0x02)
 
ASN1_BOOLEAN_TYPE             (0x01)
 
ASN1_OID_TYPE                 (0x06)
 
ASN1_SEQUENCE_TYPE            (0x30)
 
ASN1_DATE_TYPE                (0x17)
 
ASN1_UTCTIME                  (0x17)
 
ASN1_GENERALTIME              (0x18)
 
ASN1_SET_TYPE                 (0x31)
 
ASN1_PRINTABLE_STRING_TYPE    (0x13)
 
ASN1_TELETEX_STRING_TYPE      (0x14)
 
ASN1_OPTIONAL_0               (0xa0)
 
ASN1_OPTIONAL_1               (0xa1)
 
ASN1_OPTIONAL_2               (0xa2)
 
ASN1_OPTIONAL_3               (0xa3)
===/pre>
============================================================================*/


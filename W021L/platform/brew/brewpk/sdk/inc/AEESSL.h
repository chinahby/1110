#ifndef AEESSL_H
#define AEESSL_H
/*======================================================

FILE:  AEESSL.h

SERVICES:
        ISSL, SSLRootCerts

GENERAL DESCRIPTION:
        Implements the SSL protocol engine over/under the ISocket 
        or Iport interfaces using IX509Chain for certificate chaining
        

  
=====================================================

Copyright © 2002-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
#include "AEE.h"
#include "AEESource.h"
#include "AEEError.h"
#include "AEEWebOpts.h"
#include "AEENet.h"
#include "AEEX509Chain.h"
#include "AEEPort.h"
#include "AEECLSID_SSL.bid"

//********************************************************************************************************************************
//
// SSL Type declarations
//
//********************************************************************************************************************************


// SSL protocol versions
#define SSL_VERSION_30         0x0300
#define SSL_VERSION_TLS10      0x0301
#define SSL_VERSION_UNKNOWN    0xffff


// Constants representing different key exchange protocols
#define SSL_KEYPROT_NONE          (0)   // No key exchange       
#define SSL_KEYPROT_DH_ANONYMOUS  (1)   // anonymous Diffie-Hellman
#define SSL_KEYPROT_DH_EPHEMERAL  (2)   // ephemeral Diffie-Hellman   
#define SSL_KEYPROT_DH_DSS_SIGN   (3)   // Diffie-Hellman w/ DSS signature
#define SSL_KEYPROT_DH_RSA_SIGN   (4)   // Diffie-Hellman w/ RSA signature
#define SSL_KEYPROT_DH_DSS_CERT   (5)   // Diffie-Hellman w/ DSS certificate
#define SSL_KEYPROT_DH_RSA_CERT   (6)   // Diffie-Hellman w/ RSA certificate
#define SSL_KEYPROT_RSA           (7)   // Standard RSA with certs
#define SSL_KEYPROT_UNKNOWN       (100) // Don't know what the exchange was

// Represent different encryption algorithms 
#define SSL_CRYPT_NONE            (0)   // No encryption
#define SSL_CRYPT_RC4             (1)   // ARC4 algorithm
#define SSL_CRYPT_DES             (2)   // DES algorithm
#define SSL_CRYPT_3DES            (3)   // 3DES algorithm
#define SSL_CRYPT_SEED            (4)   // SEED algorithm
#define SSL_CRYPT_AES             (5)   // AES128 algorithm
#define SSL_CRYPT_AES128          (5)   // AES128 algorithm
#define SSL_CRYPT_AES256          (6)   // AES256 algorithm
#define SSL_CRYPT_UNKNOWN         (100) // Don't know what was used 

// The hash or mac algorithm used 
#define SSL_HASH_NONE             (0)   // No hash function 
#define SSL_HASH_SHA_1            (1)   // SHA-1 hash function 
#define SSL_HASH_SHA              SSL_HASH_SHA_1   // SHA-1 hash function 
#define SSL_HASH_MD5              (2)   // MD5 hash function 
#define SSL_HASH_UNKNOWN          (100) // Don't know what was used 

// Cipher suites
#define SSL_CSUITE_RSA_WITH_RC4_128_MD5      (0x0004)
#define SSL_CSUITE_RSA_WITH_RC4_128_SHA      (0x0005)
#define SSL_CSUITE_RSA_WITH_DES_CBC_SHA      (0x0009)
#define SSL_CSUITE_RSA_WITH_3DES_EDE_CBC_SHA (0x000A)
#define SSL_CSUITE_RSA_WITH_SEED_CBC_SHA     (0x0109)
#define SSL_CSUITE_RSA_WITH_AES_128_CBC_SHA  (0x002F)
#define SSL_CSUITE_RSA_WITH_AES_256_CBC_SHA  (0x0035)
#define SSL_CSUITE_UNKNOWN                   (0xffff) 


// Client Certificate Type
//   represents certificate types supported by server 
//   for client authentication
#define SSL_CERT_TYPE_RSA_SIGN      1
#define SSL_CERT_TYPE_DSS_SIGN      2
#define SSL_CERT_TYPE_RSA_FIXED_DH  3
#define SSL_CERT_TYPE_DSS_FIXED_DH  4


// Data structure holding security level info, about 6 bytes
typedef struct SSLSecurityInfo {
   uint16  uProtoVersion; // One of SSL_VERSION_*
   uint8   uKeyExchange;  // One of SSL_KEYPROT_*
   uint8   uCryptAlgo;    // One of SSL_CRYPT_*
   uint8   uHashAlgo;     // One of SSL_HASH_*
   uint16  uCipherSuite;  // One of SSL_CSUITE_*  SSL_CSUITE_UNKNOWN returning this is unsupported
} SSLSecurityInfo;


typedef struct SSLInfo
{
   int             nResult;      // Status of the SSL connection. One of SSL_RESULT_* error codes
   SSLSecurityInfo SecInfo;      // The security level negotiated. Valid on SSL_RESULT_SECURED
   X509TrustResult TrustResult;  // Trust results; see AEEX509Chain.h
   ISocket        *pSSLSocket;   // Open socket representing secured stream

} SSLInfo;

typedef struct SSLCipherMap 
{
   int         uCryptAlgo;    // Cipher algorithm (See SSL_CRYPT_XX)
   AEECLSID    uImplCls;      // AEECLSID which implements algorithm
} SSLCipherMap;

//********************************************************************************************************************************
//
// SSL Options
//
//********************************************************************************************************************************

// Options and parameters in ISSL are handled with WebOpts. Some are
// options you set that control the protocol engine. Some are  values
// you retrieve to find out what the engine has done. Any value you set
// you can also get. 

// ISSL uses IX509Chain for all the certificate chaining. You can and
// should set some IX509Chain options for the negotiation. You can 
// AddOpt them to instance of ISSL, or you can pass them to negotiate.
// They will be passed on to IX509Chain. These options include
// the root certs and the host name you're trying to get to.
//
// By default these options are *not* copied. You have to be sure the 
// storage for the value you set persists. You can set the options to be copied 
// by adding  a copy opts parameter. 
//
// You can add WebOpt defaults (other IWebOpts objects) to the option list
// for negotiation or to ISSL itself to chain sets of options however you see fit.
// For example, one IWebOpt could have all of the options that are
// common to one of several ISSL connections, or as a base
// set of options.


// claim our chunk of the option ID space. This is private stuff actually part of the
// implementation that you don't need to reference to use ISSL
#define WEBOPT_SSL_SZ               0x00012000
#define WEBOPT_SSL_SZ_LAST          0x000121ff //that's 511 possible options
#define WEBOPT_SSL_32BIT            0x00022000
#define WEBOPT_SSL_32BIT_LAST       0x000221ff
#define WEBOPT_SSL_VARIABLE         0x10010000
#define WEBOPT_SSL_VARIABLE_LAST    0x1001f000
#define WEBOPT_SSL_INTERFACE        0x00032000
#define WEBOPT_SSL_INTERFACE_LAST   0x000321ff
#define WEBOPT_SSL_VARBUFFER        0x00040200 //Reserve between 200 and 300
#define WEBOPT_SSL_VARBUFFER_LAST   0x00040300
#define WEBOPT_SSL_ASN1             0x00053000
#define WEBOPT_SSL_ASN1_LAST        0x000530ff


#define WEBOPT_SSL_SEC_INFO               (WEBOPT_SSL_VARIABLE+sizeof(SSLInfo))
#define WEBOPT_SSL_CIPHER_MAP             (WEBOPT_SSL_VARIABLE+WEBOPT_VARIABLE_INC+sizeof(SSLCipherMap))

#define WEBOPT_SSL_CA_DN_LIST             (WEBOPT_SSL_VARBUFFER+1)
#define WEBOPT_SSL_CLIENT_CERTIFICATES    (WEBOPT_SSL_VARBUFFER+2)
#define WEBOPT_SSL_CA_DN_CERT_TYPES       (WEBOPT_SSL_VARBUFFER+3)

#define WEBOPT_SSL_TRUST_MODE             (WEBOPT_SSL_32BIT+1) 
#define    SSL_TRUST_MODE_FAIL   (0L)
#define    SSL_TRUST_MODE_CHECK  (1L)
#define    SSL_TRUST_MODE_IGNORE (2L)
#define    SSL_TRUST_MODE_ALWAYS (3L)
#define WEBOPT_SSL_ALLOWED_CIPHER_SUITES  (WEBOPT_SSL_32BIT+2)                    
#define WEBOPT_SSL_ALLOWED_VERSIONS       (WEBOPT_SSL_32BIT+3)  
#define WEBOPT_SSL_WANT_X509_CHAIN        (WEBOPT_SSL_32BIT+4)  

#define WEBOPT_SSL_NEGOTIATE_HANDLER      (WEBOPT_SSL_32BIT+5)
#define WEBOPT_SSL_NEGOTIATE_HANDLER_DATA (WEBOPT_SSL_32BIT+6)
#define WEBOPT_SSL_RSA_IMPLEMENTATION     (WEBOPT_SSL_32BIT+7)
#define WEBOPT_SSL_CLASSID                (WEBOPT_SSL_32BIT+8)
#define WEBOPT_SSL_FEATURES               (WEBOPT_SSL_32BIT+9)
#define    SSL_FEATURE_CLIENT_AUTH        (0x00000001L)
#define    SSL_FEATURE_CLIENT_AUTH_EZ     (0x00000002L)
#define WEBOPT_SSL_VER                    (WEBOPT_SSL_32BIT+10)

#define WEBOPT_SSL_X509_CHAIN             (WEBOPT_SSL_INTERFACE+1)
#define WEBOPT_SSL_SESSIONDB              (WEBOPT_SSL_INTERFACE+2)
#define WEBOPT_SSL_CLIENT_CERT_RSA        (WEBOPT_SSL_INTERFACE+3)

#define WEBOPT_SSL_CA_DN                  (WEBOPT_SSL_ASN1+1)
#define WEBOPT_SSL_CLIENT_CERTIFICATE     (WEBOPT_SSL_ASN1+2)

#define WEBOPT_SSL_VERSION                (WEBOPT_SSL_SZ+1)

//Reserved
#define ISSL_SOCKET_IOCTL_RESERVED1 0x5555feed

//Negotiation handler function type (NegotiateHandler)
typedef void (*PFNNEGHANDLER)(void *pv, const SSLInfo *psi, IWebOpts *piOpts, int *pnResultCode, AEECallback *pcb);



//********************************************************************************************************************************
//
// SSL Interface
//
//********************************************************************************************************************************

typedef struct ISSL ISSL;
AEEINTERFACE(ISSL) {
   INHERIT_IWebOpts(ISSL);

   void  (*SetSocket)    (ISSL *po, ISocket *pSock);                     
   void  (*Negotiate)    (ISSL *po, SSLInfo *pInfo, IX509Chain **pCerts, AEECallback *pcb, ...);   
   void  (*NegotiateV)   (ISSL *po, SSLInfo *pInfo, IX509Chain **pCerts, AEECallback *pcb, const WebOpt *awozList);   
   void  (*Renegotiate)  (ISSL *po, SSLInfo *pInfo, IX509Chain **pCerts, AEECallback *pcb, ...);   
   void  (*RenegotiateV) (ISSL *po, SSLInfo *pInfo, IX509Chain **pCerts, AEECallback *pcb, const WebOpt *awozList);   
   void  (*Close)        (ISSL *po, SSLInfo *pInfo, AEECallback *pcb);
   void  (*SetPort)      (ISSL *po, IPort *piPort, const void *pvSessionKey, int nSessionKeyLen);
};

#define ISSL_AddRef(p)                  AEEGETPVTBL((p),ISSL)->AddRef((p))
#define ISSL_Release(p)                 AEEGETPVTBL((p),ISSL)->Release((p))  

#define ISSL_QueryInterface(p,clsid,pp) AEEGETPVTBL((p),ISSL)->QueryInterface((p),(clsid),(pp))
#define ISSL_AddOpt(p,o)                AEEGETPVTBL((p),ISSL)->AddOpt((p),(o))
#define ISSL_RemoveOpt(p,i,n)           AEEGETPVTBL((p),ISSL)->RemoveOpt((p),(i),(n))
#define ISSL_GetOpt(p,i,n,o)            AEEGETPVTBL((p),ISSL)->GetOpt((p),(i),(n),(o))

#define ISSL_SetSocket(p,s)             AEEGETPVTBL((p),ISSL)->SetSocket((p),(s))
#define ISSL_Negotiate(p,args)          AEEGETPVTBL((p),ISSL)->Negotiate args
#define ISSL_NegotiateV(p,s, x, cb,ol)  AEEGETPVTBL((p),ISSL)->NegotiateV((p),(s), (x), (cb),(ol))
#define ISSL_Renegotiate(p,args)        AEEGETPVTBL((p),ISSL)->Renegotiate args
#define ISSL_RenegotiateV(p,s,x, cb,ol) AEEGETPVTBL((p),ISSL)->RenegotiateV((p),(s), (x), (cb),(ol))
#define ISSL_Close(p,s,cb)              AEEGETPVTBL((p),ISSL)->Close((p),(s),(cb))

#define ISSL_SetPort(p,s, i, il)        AEEGETPVTBL((p),ISSL)->SetPort((p),(s),(i),(il))

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

=======================================================================
SSLSecurityInfo

Description:
   This data type is a negotiated SSL security level including cipher suite.

Definition:
   typedef struct {
      uint16  uProtoVersion; 
      uint8   uKeyExchange;
      uint8   uCryptAlgo;
      uint8   uHashAlgo;
      uint16  uCipherSuite;
   } SSLSecurityInfo;;

Members:
===pre>
   uProtoVersion        SSL protocol version negotiated. One of SSL_VERSION_*
   uKeyExchagne         The key exchange algorithm. One of SSL_KEYPROT_*
   uCryptAlgo           Symmetric algorithm negotiated. One of SSL_CRYPT_*
   uHashAlgo            Hash function negotiated. One of SSL_HASH_*
   uCipherSuite         Cipher suite negotiated. One of SSL_CSUITE_*  
===/pre>

Comments:
   When the security level negotiated for the current session
   is reported it is in the SSLSecurityInfo data structure.
   
   The following constants define the protocol version. SSL versions
   prior to 3.0 are not supported.
===pre>
   SSL_VERSION_30             SSL version 3.0
   SSL_VERSION_TLS10          TLS version 1.0
   SSL_VERSION_UNKNOWN        Protocol unknown
===/pre>

   The following constants define the key exchange / agreement / protection
   scheme or algorithm. Note that all of these algorithms may not be
   available with a particular version of BREW or on a particular handset. 
===pre>
   SSL_KEYPROT_NONE           No key exchange       
   SSL_KEYPROT_DH_ANONYMOUS   Anonymous Diffie-Hellman
   SSL_KEYPROT_DH_EPHEMERAL   Ephemeral Diffie-Hellman   
   SSL_KEYPROT_DH_DSS_SIGN    Diffie-Hellman w/ DSS signature
   SSL_KEYPROT_DH_RSA_SIGN    Diffie-Hellman w/ RSA signature
   SSL_KEYPROT_DH_DSS_CERT    Diffie-Hellman w/ DSS certificate
   SSL_KEYPROT_DH_RSA_CERT    Diffie-Hellman w/ RSA certificate
   SSL_KEYPROT_RSA            Standard RSA with certs
   SSL_KEYPROT_UNKNOWN        Don't know what the exchange was
===/pre>

   The following constants define the symmetric cipher used. Note
   that just because a constant is defined here doesn't mean that
   a particular version of BREW or handset supports it. ARC4 is
   generally always available and performs well. This is the
   same algorithm in the cipher suite separated out for convenience.
===pre>
   SSL_CRYPT_NONE            No encryption
   SSL_CRYPT_RC4             ARC4 algorithm
   SSL_CRYPT_DES             DES algorithm
   SSL_CRYPT_3DES            3DES algorithm
   SSL_CRYPT_SEED            SEED algorithm
   SSL_CRYPT_AES             AES128 algorithm
   SSL_CRYPT_AES128          AES128 algorithm
   SSL_CRYPT_AES256          AES256 algorithm
   SSL_CRYPT_UNKNOWN         Don't know what was used 
===/pre>

   The following constants define the hash function used. They are
   usually available with any BREW version or handsets. 
===pre>
   SSL_HASH_NONE             No hash function 
   SSL_HASH_SHA              SHA-1 hash function 
   SSL_HASH_MD5              MD5 hash function 
   SSL_HASH_UNKNOWN          Don't know what was used 
===/pre>

   The following constants indicate the cipher suite used. This is some
   what redundant with the above set of constants, but included for
   convenience. Not all cipher suites are supported on a given version
   of BREW or a given handset. They correspond to the standard cipher
   suites for SSL and TLS.
===pre>
   SSL_CSUITE_RSA_WITH_RC4_128_MD5  
   SSL_CSUITE_RSA_WITH_RC4_128_SHA  
   SSL_CSUITE_RSA_WITH_DES_CBC_SHA   
   SSL_CSUITE_RSA_WITH_3DES_EDE_CBC_SHA  
   SSL_CSUITE_RSA_WITH_SEED_CBC_SHA
   SSL_CSUITE_RSA_WITH_AES_128_CBC_SHA
   SSL_CSUITE_RSA_WITH_AES_256_CBC_SHA
   SSL_CSUITE_UNKNOWN  
===/pre>  
   The above constants are also used to request a particular set of protocol
   versions and cipher suites before a negotiation.

See Also:
    ISSL_Negotiate(),
    ISSL_NegotiateV(),
    ISSL_Renegotiate(),
    ISSL_RenegotiateV()

=======================================================================
SSLInfo

Description:
   This data type is the result of the SSL negotiation and includes
   the secured ISocket.

Definition:
   typedef struct SSLInfo
   {
      int             nResult;
      SSLSecurityInfo SecInfo;
      X509TrustResult TrustResult;
      ISocket        *pSSLSocket;
   } SSLInfo;

Members:
===pre>
   nResult           General result of an SSL negotiation. One of SSL_RESULT_* error codes.
                     See ISSL error codes.
   SecInfo           The security level negotiated. Valid on SSL_RESULT_SECURED
   TrustResult       Trust results; see X509TrustResult 
   pSSLSocket        Open socket representing secured stream.  This value is
                     be NULL when using IPort.  To retrieve the pSSLPort, use
                     ISSL_QueryInterface() with AEECLSID_PORT.
===/pre>

Comments:
   This returns the result of a negotiation. The results has two parts, the
   protocol negotiation result, and the trust result from the X509 chain
   verification.  
   
   Trust result is filled in only if nResult is either SSL_RESULT_TRUST_ERR or
   SSL_RESULT_SECURED. If the result is a secured connection there will be no
   trust errors, and only the chain length is of any use in the structure. If the
   error is SSL_RESULT_TRUST_ERR then the details of the error are reported in 
   TrustResult structure. Note that nResult in the TrustResult structure does not
   show the details, see the auChainErrors and summary further down in the structure.
   nResult in the TrustResult structure is generally SUCCESS. Errors such as running out
   of memory or a very corrupt certificate are reported as nResult in the SSLInfo
   structure.

   Trust errors can be overridden and allow the negotiation to continue. Other errors
   cannot be overridden.  
   
   pSSLSocket is the most important part of the result as it is the secured
   stream.

See Also:
   ISSL_Negotiate(), 
   SSLSecurityInfo, 
   IX509CHAIN_Verify(),
   X509TrustResult

=======================================================================
PFNNEGHANDLER
Description: data type for the NegotiateHandler used with
             WEBOPT_SSL_NEGOTIATE_HANDLER.
             
See Also:
   NegotiateHandler~
   ISSL_Negotiate() 

=======================================================================
NegotiateHandler

Description:
   The Negotiate handler is a function set by the client, 
   which is called when ISSL needs more information to continue.

   During the SSL handshake, ISSL may need more information such as 
   1. Trust checking Options (overriding certain errors, etc.)
   2. Client certificate based upon the server supplied list of CA DNs 

   The Negotiate handler is called with appropriate error code in the
   SSLInfo to inform the user.  Once the client supplies relevant information 
   (through setting WebOpts on the given piOpts interface), the handshake is
   continued after the client resumes the given callback pcb.

   The Negotiate handler is also called sometimes to inform the client of the progress.
   When called as such, the pcb and pnResultCode are NULL, as there is no need
   to resume the ISSL operation.  For example, when ISSL successfully completes
   a handshake, it informs the client by the handler.

   The negotiation handler may also be called when the server triggers a 
   renegotiation, or a negotiation is initiated by reading or writing directly 
   to the secure socket/port instead of calling ISSL_Negotiate.  Since this 
   type of negotiation happens independently from a ISSL_Negotiation call, 
   none of the WebOpts which might have been supplied in the ISSL_Negotiation 
   is available.  The negotiation handler must be able to provide the 
   WebOpts, or the WebOpts may be provided before the negotiation using 
   ISSL_AddOpt.  If the negotiation hander may change the trust 
   settings or add root certs, it is necessary that WEBOPT_SSL_TRUST_MODE is 
   set with ISSL_AddOpt, otherwise the default value of SSL_TRUST_MODE_FAIL 
   will prevent the negotiation handler from being called for more information.

   If the client would like to continue the ISSL operation (by supplying more
   information or just to continue), set the ResultCode to SUCCESS.  Any error
   code set here, aborts the ISSL operation.

Definition:
   typedef void (*PFNNEGHANDLER)( void *pv, SSLInfo *psi, IWebOpts *piOpts, 
                                  int *pnResultCode, AEECallback *pcb);

Members:
===pre>
   pv                User data as supplied by the web opt 
                     WEBOPT_SSL_NEGOTIATE_HANDLER_DATA
   SSLInfo           The current result SSL negotiation
   piOpts            Any new webopts will be added to this object
   pnResultCode      The error code set by the user.  
                     Set to SUCCESS to continue negotiation
   pcb               Callback that needs to be resumed to 
                     continue with the negotiation
===/pre>
Comments:

  NegotiateHandler is a function that is called by ISSL, when it 
  needs more information from the user or a particular error needs 
  to be overridden.  This function is set by WEBOPT_SSL_NEGOTIATE_HANDLER 
  and its data argument(pv) is set by WEBOPT_SSL_NEGOTIATE_HANDLER_DATA.

  Currently, this functions gets called for trust overrides and 
  client authentication.  On client authentication, the supplied 
  IWebOpts contains the server's Certificate Authorities Distinguished
  Names list (see WEBOPT_SSL_CA_DN_CERT_TYPES and WEBOPT_SSL_CA_DN
  or WEBOPT_SSL_CA_DN_LIST) and the negotiation handler 
  always gets called so as to give a chance to the client to supply
  the correct client certificate based on the DN list.

  If the error is successfully handled, set the *pnResult code to SUCCESS.
  If client wishes to abort the handshake, set the *pnResultCode to EFAILED.
  Always resume the pcb when it is given (is non-null).

  This function is also called during renegotiation.

See Also:
   ISSL_Negotiate(), 
   ISSL_Renegotiate()
=======================================================================
ClientAuthentication WebOpts

Client Authentication WebOpts are used to configure ISSL for client
authentication.  If an SSL/TLS server requests client authentication during the SSL 
handshake, it supplies a list of CA DNs.  Based on these CA DNs, the 
client answers with a compatible client certificate message and another 
message signed with the client private certificate.  To perform these 
operations, ISSL needs to be given the information supplied by the WebOpt 
WEBOPT_SSL_CLIENT_CERT_RSA and either WEBOPT_SSL_CLIENT_CERTIFICATE or 
WEBOPTS_SSL_CLIENT_CERTIFICATES.  See WEBOPT_SSL_FEATURES described in 
ISSL_GetOpt() on how to identify the device's ability to support SSL 
Client Authentication.  

Description:

WEBOPT_SSL_CLIENT_CERTIFICATE: 
  This WebOpt defines each individual certificate in the client public 
  certificate chain.  The first instance (index 0) of this multi-valued 
  WebOpt is the client certificate in DER (ASN.1) format,  Each subsequent
  certificate in the certificate chain, if provided, should certify the
  preceding certificate.
   
   This WebOpt may be used in place of the more complicated structure requirements
   of WEBOPT_SSL_CLIENT_CERTIFICATES.  Simply add one of these WebOpts for each
   certificate in the client certificate chain.

WEBOPT_SSL_CLIENT_CERTIFICATES: 
  This webopt contains the client public certificate chain. Unlike 
  the WEBOPT_SSL_CLIENT_CERTIFICATE, it uses a fairly complex structure to 
  hold the entire certificate chain.
  
  Except for the first 2 bytes which are required by IWebOpts to identify the length,
  the structure must match the SSL certificate message structure:
===pre>
    opaque ASN.1Cert<1..2^24-1>;
    struct {
      ASN.1Cert certificate_list<0..2^24-1>;
    } Certificate
===/pre>
  
  In other words, each cert must be pre-pended by three bytes giving its
  length. These cert structures are then concatenated into a sequence with each
  certificate certifying the one preceding it. The resulting sequence is prepended
  by the length of the sequence. This is finally pre-pended by the length of
  the bytes encoded so far as a two byte integer. The three byte integers are 
  big endian and per the SSL spec. The two byte integer is the web opt var buffer
  formatting. Diagrammatically:
===pre>
           The value X + Y + 2 * 3 + 3 encoded as a native integer
           The value X + Y + 2 * 3 encoded as three bytes big endian
           X encoded as three bytes big endian
           Cert A of len X
           Y encoded as three bytes big endian
           Cert B of len Y
   where
      A is the client certificate
      X is the length of A
      B is a certificate which certifies A
      Y is the length of B
===/pre>

WEBOPT_SSL_CLIENT_CERT_RSA: 
  Pointer to the IRSA interface initialized with the client private 
  certificate information.  It is used to perform  the RSA 
  signature operation asynchronously. The IRSA interface must implement 
  specific functionality to access the client private key and to perform
  the signing operation. If the client certificate is on a WIM card, 
  then the IRSA implementation must have the ability to access and 
  perform needed functions on this card.

On receiving a client authentication request, and on not finding the relevant 
WebOpts, the ISSL_Negotiate() call fails with the error code 
SSL_RESULT_CLIENT_AUTH_ERR. When failed this way, the WEBOPT_SSL_CA_DN_LIST 
and associated WebOpts WEBOPT_SSL_CA_DN and WEBOPT_SSL_DN_CERT_TYPES
are added to the ISSL interface.

To proceed further, the app needs to issue ISSL_Negotiate() again 
(or if using the NegotiateHandler, just resume the given AEECallback), after 
adding the WebOpts for the client certificate chain and the IRSA interface.
As ISSL keeps a copy of these WebOpts by doing an AddRef, these interfaces are
valid until the end of handshake or connection close.

The WEBOPT_SSL_CA_DN_LIST WebOpt contains the CA DN information as sent
by the server in the SSL Certificate Request message. ISSL parses this
message and provides the parsed information in the WEBOPT_SSL_DN_CERT_TYPES
and WEBOPT_SSL_CA_DN WebOpts.

WEBOPT_SSL_DN_CERT_TYPES format:

This WebOpt's value is a variable buffer where the 1st two bytes are the
standard length of the array, followed by a byte array containing the
types of certificates supported by the server.  The array is sorted by
the server's preference.  Possible ClientCertificateType's:
===pre>
   SSL_CERT_TYPE_RSA_SIGN      RSA certificate
   SSL_CERT_TYPE_DSS_SIGN      DSS certificate
   SSL_CERT_TYPE_RSA_FIXED_DH  RSA certificate with DH parameters
   SSL_CERT_TYPE_DSS_FIXED_DH  DSS certificate with DH parameters
===/pre>

WEBOPT_SSL_CA_DN format:
This WebOpt's value is a Distinguished Name (DN) in X.509 (ASN.1) format.  This WebOpt is
multi-valued - one WebOpt for each DN.

WEBOPT_SSL_CA_DN_LIST format: 

This WebOpt's value is a pointer to binary data. 
The first two bytes indicate the length of the rest of the data and 
encoded in little-endian format. The rest of the data is in the same format 
as CerficateRequest message (refer: TLS RFC 2246).

Structure of this message: 

===pre>
enum { 
   rsa_sign(1), 
   dss_sign(2), 
   rsa_fixed_dh(3), 
   dss_fixed_dh(4), 
   (255) 
} ClientCertificateType; 

opaque DistinguishedName<1..2^16-1>; 

struct { 
   ClientCertificateType certificate_types<1..2^8-1>; 
   DistinguishedName certificate_authorities<3..2^16-1>; 
} CertificateRequest; 
===/pre>

certificate_types: This field is a list of the types of certificates requested, 
sorted in order of the server's preference. See also SSL_CERT_TYPE_*

certificate_authorities: This is a list of the distinguished names of acceptable 
certificate authorities. These distinguished names may specify a desired 
distinguished name for a root CA or for a subordinate CA; 
thus, this message can be used both to describe known roots and 
a desired authorization space. 


Parsing of WEBOPT_SSL_CA_DN_LIST:

To parse the CA DN list, first verify the total binary blob length
(in Little Endian format) in the first two bytes.

The CerficateRequest message follows. 

The first byte is the length of the enum types. rsa_sign must be one of 
the types given. Loop through the enum values and reach the end. 
Once ClientCertificateType is parsed this way, then the certificate_authorities 
should be parsed to get the DNs. The first two bytes after ClientCertificateType 
indicate the length of the rest of the binary blob (Big Endian order). Note that
this length value may not be properly word aligned for the processor. It should NOT
be accessed by directly casting a char * to an short *.

After this, the binary data contains DN encoded in the ASN.1 format and needs 
to be decoded in that way. Please refer X.509 specification regarding 
the DN format.

Example showing a sample WEBOPT_SSL_CA_DN_LIST value:

===pre>
0xF5 0x01 -> 2 bytes - length of the following data in little-Endian byte order
0x02 -> 1 byte - length of the enums
0x01 -> 1 byte - rsa_sign
0x03 -> 1 byte - rsa_fixed_dh 
<end of ClientCertificateType>
0x01 0xF0 -> 2 bytes - length of the following data
<DN in ASN.1 format>
<DN in ASN.1 format> etc
===/pre>

See Also:
   ISSL
   
=======================================================================*/


/*=======================================================================
  INTERFACES   DOCUMENTATION
 ========================================================================

ISSL Interface

This interface provides SSL/TLS security for a network connection. This interface
inherits from IWebOpts. It uses either ISocket or IPort in the interface. Note that 
this functionality is available combined with HTTP to form HTTPS in the IWeb 
interface. The web opts described here and in IX509Chain do apply to https.

The WebOpts are passed all the way down through IWeb to the ISSL or 
IX509Chain used for the https transaction. Also, the WebOpts that give 
the results of an SSL negotiation performed as part of an HTTPS connection 
are available. These are available as options in the IWebResp object 
returned when the web transaction completes. For example you can get the 
security level. You can also request the whole IX509Chain object be 
returned if you wish to examine the cert chain.

Generally, to use https, you should create an IWeb, add an opt or opts 
for root certs, add an opt for the hostname, and give a URL with the scheme
"https".

A set of root certs commonly used with SSL is available via the 
SSLRootCerts interface. This can be passed to IWeb or ISSL as a WebOpt 
of type WEBOPT_DEFAULTS.


=============================================================================

ISSL_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

See Also:
    ISSL_Release()

=============================================================================

ISSL_Release()

Description:
    This function is inherited from IBASE_Release().

Comments:
   If the reference count goes to zero all memory held is freed and all callbacks
   are cancelled. Callbacks should be cancelled explicitly rather than 
   relying on ISSL_Release.  The connection, if active, is closed.

See Also:
    ISSL_AddRef()

=============================================================================

ISSL_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface().

=============================================================================

ISSL_AddOpt()

Description:
    This function is inherited from IWEBOPTS_AddOpt().

See Also:

    WebOpt
    IWebOpts
    ISSL_RemoveOpt()
    ISSL_GetOpt()
    
=============================================================================

ISSL_RemoveOpt()

Description:
    This function is inherited from IWEBOPTS_GetOpt(). 

See Also:
    IWebOpts
    ISSL_AddOpt()
    
=============================================================================

ISSL_GetOpt()

Description:
    This function is inherited from IWEBOPTS_GetOpt(). 
    
   SSL specific webopts:
      WEBOPT_SSL_VERSION:  returns the version of ISSL as an ASCII string.
      
      WEBOPT_SSL_FEATURES: returns a 32 bit bitmask for specific features supported by ISSL
      
      Supported features:
===pre>
         SSL_FEATURE_CLIENT_AUTH:      ISSL supports client authentication negotiations 
                                       with webopts WEBOPT_SSL_CLIENT_CERTIFICATES and 
                                       WEBOPT_SSL_SSL_CA_DN_LIST.
         SSL_FEATURE_CLIENT_AUTH_EZ:   ISSL supports client authentication with the 
                                       simplified WebOpts WEBOPT_SSL_CLIENT_CERTIFICATE, 
                                       WEBOPT_SSL_DN_CERT_TYPES and WEBOPT_SSL_CA_DN.
===/pre>

See Also:
    WebOpt
    IWebOpts Interface
    ISSL_AddOpt()
    ISSL_RemoveOpt()

=============================================================================

ISSL_SetSocket()

Description:
   When using the ISocket interface with SSL, this function must be called to
   supply a socket already open and connected to the SSL server.

Prototype:
    void ISSL_SetSocket(ISSL *pISSL, ISocket *pISocket)

Parameters:
    pISSL :  The Interface pointer
    pISocket :  Pointer to open socket

Return Value:
    none

Comments: 
   The ISocket passed in must be open and connected to a server 
   that is ready to start an SSL negotiation.  Note that the ISocket passed 
   is an object with an ISocket vtable.  It doesn't have to be a standard 
   BREW ISocket, but can instead be an object that implements the ISocket 
   interface.  By this mechanism, ISSL can be used on top of any data stream 
   object.  

Version:
   Introduced BREW Client 2.0

See Also:
    ISSL_SetPort()
    
==============================================================================
ISSL_NegotiateV()

Description:
This function is described with ISSL_Negotiate()
==============================================================================
ISSL_Renegotiate()

Description:
This function is described with ISSL_Negotiate()
==============================================================================
ISSL_RenegotiateV()

Description:
This function is described with ISSL_Negotiate()
==============================================================================
ISSL_Negotiate()

Description:
   The functions ISSL_Negotiate(), ISSL_NegotiateV(), ISSL_Renegotiate(), and 
   ISSL_RenegotiateV(), cause SSL handshaking to progress. If a handshake has 
   not started, it starts it. If one is in progress, it continues it. If a 
   connection has been established, it reports the state of it in relation
   to what was requested.
   
   Renegotiate is exactly the same, except it will redo the negotiation if a 
   connection is established. Renegotiate is currently NOT supported. 

Prototype:

void ISSL_Negotiate
    (
    ISSL *pISSL, 
    SSLInfo *pinfo, 
    IX509Chain **pCerts, 
    AEECallback *pcb, ...
    );

void ISSL_NegotiateV
    (
    ISSL *pISSL, 
    SSLInfo *pinfo, 
    IX509Chain **pCerts,
    AEECallback *pcb,
    WebOpts *pOpts
    );

void ISSL_Renegotiate
    (
    ISSL *pISSL, 
    SSLInfo *pinfo, 
    IX509Chain *pCerts, 
    AEECallback *pcb, ...
    );

void ISSL_RenegotiateV
    (
    ISSL *pISSL, 
    SSLInfo *pinfo, 
    IX509Chain **pCerts,
    AEECallback *pcb,
    WebOpts *pOpts
    );


Parameters:
    pISSL :  The Interface pointer
    pinfo :  Place to put the response structure
    pCerts :  Place for ISSL to return cert chain. 
    pcb :  Place to call back when SSL, succeeds, fails or needs help~
    pOpts or ...  :  The set of WebOpt options against which negotiation is performed.

Return Value:
    None

Comments:  
   In the simplest case, this call negotiates until completion or failure. It uses the 
   options passed on the command line and any options stored in the instance as 
   parameters for the negotiation. The negotiation also does the cert chain 
   verification internally using an instance of IX509Chain. On completion or failure
   the pinfo is filled in and the callback is called.

   When evaluating parameters for the negotiation, the options passed as arguments
   to ISSL_Negotiate are considered first. Note that these may include a default IWebOpts
   that contains a whole tree of options. The ones stored in ISSL are considered
   next. These options are used for the SSL negotiation to determine cipher suite,
   protocol, version and other. They are also passed to the IX509Chain instance 
   internally as options for the cert chain verification. That is you can and should
   add/pass IX509Chain options here. In particular you should pass the root certs
   set and the hostname.

   If you want the IX509Chain instance returned so you can examine the
   certs in the chain or store them, then you can either pass a non-NULL parameter for
   pCerts or set the WebOpt that causes the interface to be put in a WebOpt.

   The negotiation has different behaviors depending on the state of the connection, 
   WEBOPT_SSL_TRUST_MODE, and whether negotiation or renegotiate is called. The three
   connections states are unsecured, suspended negotiation and secured. 

   In the unsecured state the description of WEBOPT_SSL_TRUST_MODE above applies to
   a call to negotiate or renegotiate.  Normally on a trust error ISSL closes
   the connection to the server and cannot be overridden. This is the case if
   WEBOPT_SSL_TRUST_MODE is not set or if it is set to SSL_TRUST_MODE_FAIL. If 
   WEBOPT_SSL_TRUST_MODE is set to SSL_TRUST_MODE_CHECK, the connection is not
   closed. ISSL_Negotiate can then be called again, most likely with some 
   TrustOverrides to continue with the SSL session despite the trust error. If it is
   set to SSL_TRUST_MODE_IGNORE, then all trust errors are ignored at substantial
   risk to the connection security. If the mode is SSL_TRUST_MODE_ALWAYS, the call
   back will always fire after the trust check is complete. This allows you to
   examine the trust chain and reject it even if the chaining engine is able to
   successfully validate it.

   A possible mode of operation is to use SSL_TRUST_MODE_CHECK on the first call to
   negotiate, and after checking the trust errors to use SSL_TRUST_MODE_IGNORE on
   the second call to negotiate. If you set SSL_TRUST_IGNORE on the first call to
   negotiate you are defeating most of SSL's security! The following are the 
   possible trust modes:
===pre>
   SSL_TRUST_MODE_FAIL:       Default, fail on trust errors
   SSL_TRUST_MODE_CHECK:      Suspend on trust error so they can be checked and ignored
   SSL_TRUST_MODE_IGNORE:     Ignore all trust errors
   SSL_TRUST_MODE_ALWAYS:     Always suspend so trust can always be checked.
===/pre>     
   Once in the suspended state, any call to negotiate or renegotiate will continue
   the negotiation to success or failure. 

   Calls in the secured state are where it gets unusual and interesting. If
   renegotiate is supported and is called in the secured state, then a new 
   negotiation takes place. 
 
   If you call negotiate in the secured state, the security level of the connection
   is evaluated against the security level implied by the various options, 
   in particular the cipher suites and root certs. If it matches, then success is
   returned, otherwise failure. On either success or failure the connection it
   self is not disturbed. The purpose of this is to evaluate a kept-alive
   session for reuse. Though this is not commonly done it is used by the internal
   https implementation. 

   If the WEBOPT_SSL_SESSIONDB option is set to an instantiated IRecordStore interface,
   the interface is used to store and fetch session resumption data. When a negotiation is
   successful, a record is stored in the database/cache provided by
   this interface via an add call. Before a negotiation is attempted, Lookup
   calls to this database/cache are performed to find any previously stored
   sessions. The sessions may be stored for the life of the app or for longer. 
   If they are stored longer, care should be taken as the records contain the
   master secret from the session.

   When the negotiation completes, examine the pinfo structure to get details.
   If using the ISocket interface, the secured ISocket is contained in this structure.
   This ISocket can be used like any other ISocket. Any IOCTLs that are not handled by ISSL 
   are passed on to the ISocket passed in to ISSL_SetSocket(). Calling ISOCKET_Release()
   on this pointer is equivalent to calling ISSL_Release().
   
   If using the IPort interface, calling ISSL_QueryInterface() with AEECLSID_PORT
   at this point retrieves the secured IPort interface.  Calling IPORT_Release() on
   this pointer is equivalent to calling ISSL_Release().

   The following summarizes WebOpts that affect negotiation. Note that all the
   WebOpts that affect IX509CHAIN_Verify() also affect negotiation.
===pre>
   WEBOPT_SSL_SEC_INFO               An alternate way to get the SSLInfo result
                                     of a negotiation. This is useful with IWeb
                                     and HTTPS.

   WEBOPT_SSL_TRUST_MODE             Specify general behavior on trust errors.

   WEBOPT_SSL_ALLOWED_CIPHER_SUITES  Multiple options to give an ordered list of
                                     cipher suites to be allowed for the
                                     negotiation. Must be one of SSL_CSUITE_*.
                                     If not provided, the SSL negotiation chooses
                                     the strongest common cipher suite.

   WEBOPT_SSL_CIPHER_MAP             Multiple options to remap the cipher
                                     implementation. This allows overriding the
                                     ICipher implementation, which defaults
                                     to the best implementation as determined
                                     by the ICipherFactory.  This webopt takes 
                                     the SSLCipherMap structure to define the
                                     mapping. uCryptAlgo must be one of SSL_CRYPT_*
                                     and uImplCls must be a cipher AEECLSID which
                                     implements the SSL_CRYPT_* algorithm.
                                     This option does not affect the cipher
                                     suite negotiation (See 
                                     WEBOPT_SSL_ALLOWED_CIPHER_SUITES).

   WEBOPT_SSL_ALLOWED_VERSIONS       Multiple options to give an ordered list of
                                     protocol versions to be allowed for the
                                     negotiation. Must be one of SSL_VERSION_*.
                                     If this is not specified, ISSL defaults to
                                     allowing all supported versions.

   WEBOPT_SSL_WANT_X509_CHAIN        Set this option to get the X509Chain created
                                     internally back in the web opt below. This
                                     allows you to examine the cert chain,
                                     display it or save it. If you set the chain
                                     which uses memory to store all the certs
                                     will persist until this interface is 
                                     destroyed, or the option deleted.

   WEBOPT_SSL_RSA_IMPLEMENTATION     This option allows the client to specify the 
                                     RSA implementation to be used. For example, 
                                     some platforms may allow support for hardware
                                     based modular exponentiation, and the client
                                     may use this option to take advantage of this
                                     capability in order to improve performance.

   WEBOPT_SSL_X509_CHAIN             The option in which a reference to the
                                     IX509Chain object used internally will be
                                     returned. This is a single value and should
                                     not be set.

   WEBOPT_SSL_SESSIONDB              The instantiated interface for session
                                     resumption. This must be an interface to an
                                     IRecordStore, such a IRamCache. If this is 
                                     not set or set to NULL then there will be 
                                     no session resumption.
===/pre>

   During the Negotiation operation, ISSL may need extra information from the client (ex: client
   certificates during client authentication).  If a negotiation handler function is supplied using
   WEBOPT_SSL_NEGOTIATE_HANDLER WebOpt, then it will be called with the error code, so that the 
   user can supply more information.

Side Effects: 
   None  
 
See Also:
   IX509CHAIN_Verify()
   SSLInfo
   NegotiateHandler
   ISocket()
   IPort()

===============================================================================

ISSL_Close()

Description:
    This function closes the SSL session.

Prototype:
    void ISSL_Close(ISSL *pISSL, SSLInfo *pinfo, AEECallback *pcb);

Parameters:
    pISSL :  The Interface pointer
    pinfo :  Place to put the response structure
    pcb :  Place to call back when SSL, succeeds, fails or needs help

Return Value:
    nReturn in SSLInfo indicates the success of the closure

Comments:
   ISSL_Close makes one attempt to send the close notify alert to the
   server. 

   Closure in SSL tends to differ in practice from the documented standard.  
   The standard requires a complete exchange of close alerts.  In practice 
   the secure closure is achieved by the exchange of protocol messages that 
   are part of the protocol secured, not SSL.  This is true of HTTP.  
   Basically it is not that important for complete closure, thus the 
   implementation here simply attempts to send the close notify and not the 
   full close handshake.  

Side Effects: 
   None     

See Also:
   SSLInfo
   SSLSecurityInfo
   X509TrustResult 

=============================================================================

ISSL_SetPort()

Description:
    This function must be called to when using an IPort with ISSL.  It
    provides the port that is open and connected to the SSL server to ISSL.
    It also takes a pointer to a structure that represents the session connection.

Prototype:
    void ISSL_SetPort(ISSL *pISSL, IPort *pIPort, void *pSessionId, int sessionIdLen)

Parameters:
    pISSL :  The Interface pointer
    pIPort :  Pointer to open port
    pSessionId: Pointer to a user defined structure holding a sessionId.  The sessionId 
    is used to identify repeated connections to the same host and port.  It can significantly 
    reduces the secure negotiation overhead when connecting to the same host and port.  The 
    value of this parameter is typically a pointer to a structure containing 1) the hostname 
    or host ip address, and 2) the host port number.  If this pointer is null, ISSL 
    always performs the full negotiation with the server.
    sessionidLen: length of structure pointed to by pSessionId.

Return Value:
    none

Comments:
   The IPort passed in must be open and connected to a server that is ready to
   start an SSL negotiation. Note that the IPort passed is an object with a vtable.
   It doesn't have to be a standard BREW IPort, but can instead be an object that
   implements IPort. By this mechanism ISSL can be used on top of any data stream
   object. 

Version:
   Introduced BREW Client 3.1.4
   
See Also:
    None
    
==============================================================================*/





/*========================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

SSLRootCerts Interface

This interface provides access to an embedded set of common root certificates. It
is a bit unusual in that it isn't really a new interface of its own. Its interface
is exactly that of the IWebOpts Interface. The difference is a set of options are
preset which contain common root certificates. They have WebOpt.nID set to 
WEBOPT_X509_ROOT_CERTS.

These root certificates are provided as a convenience and to save space on the
handset by having only one copy of them, rather than a copy per application that
uses SSL.  If an application requires a root certificate which is not in this set,
it will need to add the certificate(s) (in DER format) as an option of type 
WEBOPT_X509_ROOT_CERTS.

When you call ISHELL_CreateInstance(), pass AEECLSID_SSLROOTCERTS as the class ID.
You'll get back an IWebOpts *. You can uses this as a full instance of IWebOpts
and store other things, such as additional WEBOPT_X509_ROOT_CERTS, if you wish.

Typically, this interface is instantiated and passed to IX509CHAIN_Verify(), 
ISSL_Negotiate(), ISSL_NegotiateV(), ISSL_Renegotiate(), ISSL_RenegotiateV(), 
ISSL_AddOpt or IWEB_GetResponse() as an option of type WEBOPT_DEFAULTS.

See Also:
ISSL
IWebOpts


=============================================================================*/

#endif /*  AEESSL_H */      

   

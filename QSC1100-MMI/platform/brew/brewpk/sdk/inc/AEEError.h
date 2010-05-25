#ifndef AEEERROR_H
#define AEEERROR_H
/*======================================================
FILE:  AEEError.h

SERVICES:  AEE Error Codes

GENERAL DESCRIPTION:

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

//
// Basic Error Codes
//
//

#define  SUCCESS              0        // No error
#define  AEE_SUCCESS          0
#define  EFAILED              1        // General failure
#define  ENOMEMORY            2        // Out of memory
#define  ECLASSNOTSUPPORT     3        // Specified class unsupported
#define  EVERSIONNOTSUPPORT   4        // Version not supported
#define  EALREADYLOADED       5        // Object already loaded
#define  EUNABLETOLOAD        6        // Unable to load object/applet
#define  EUNABLETOUNLOAD      7        // Unable to unload object/applet
#define  EALARMPENDING        8        // Alarm is pending
#define  EINVALIDTIME         9        // Invalid time
#define  EBADCLASS            10       // NULL class object
#define  EBADMETRIC           11       // Invalid metric specified
#define  EEXPIRED             12       // App/Component Expired
#define  EBADSTATE            13       // Invalid state
#define  EBADPARM             14       // Invalid parameter
#define  ESCHEMENOTSUPPORTED  15       // Invalid URL scheme
#define  EBADITEM             16       // Invalid item
#define  EINVALIDFORMAT       17       // Invalid format
#define  EINCOMPLETEITEM      18       // Incomplete item
#define  ENOPERSISTMEMORY     19       // Insufficient flash
#define  EUNSUPPORTED         20       // API is not supported
#define  EPRIVLEVEL           21       // Security violation
#define  ERESOURCENOTFOUND    22
#define  EREENTERED           23
#define  EBADTASK             24
#define  EALLOCATED           25       // App/Module left memory allocated when released.
#define  EALREADY             26       // Operation is already in progress
#define  EADSAUTHBAD          27       // ADS mutual authorization failed
#define  ENEEDSERVICEPROG     28       // Need service programming
#define  EMEMPTR              29       // bad memory pointer
#define  EHEAP                30       // heap corruption
#define  EIDLE                31       // Context (system, interface, etc.) is idle
#define  EITEMBUSY            32       // Context (system, interface, etc.) is busy
#define  EBADSID              33       // Invalid subscriber ID
#define  ENOTYPE              34       // No type detected/found
#define  ENEEDMORE            35       // Need more data/info
#define  EADSCAPS             36       // ADS Capabilities do not match those required for phone
#define  EBADSHUTDOWN         37       // App failed to close properly
#define  EBUFFERTOOSMALL      38       // Destination buffer given is too small
#define  ENOSUCH              39       // No such name/port/socket/service exists or valid
#define  EACKPENDING          40       // ACK pending on application
#define  ENOTOWNER            41       // Not an owner authorized to perform the operation
#define  EINVALIDITEM         42       // Current item is invalid
#define  ENOTALLOWED          43       // Not allowed to perform the operation
#define  ENOPURCHASEAUTH      50

//
// File Error Codes
//

#define  FILE_ERROR           (0x100)
#define  EFILEEXISTS          (FILE_ERROR)      // File exists
#define  EFILENOEXISTS        (FILE_ERROR+1)    // File does not exist
#define  EDIRNOTEMPTY         (FILE_ERROR+2)    // Directory not empty
#define  EBADFILENAME         (FILE_ERROR+3)    // Bad file name
#define  EBADSEEKPOS          (FILE_ERROR+4)    // Bad seek position
#define  EFILEEOF             (FILE_ERROR+5)    // End of file
#define  EFSFULL              (FILE_ERROR+6)    // File system full
#define  EFILEOPEN            (FILE_ERROR+7)    // File already open
#define  EOUTOFNODES          (FILE_ERROR+8)    // No INODES (file slots) available
#define  EDIRNOEXISTS         (FILE_ERROR+9)    // Directory does not exist
#define  EINVALIDOPERATION    (FILE_ERROR+10)   // invalid operation
#define  ENOMEDIA             (FILE_ERROR+11)   // Storage media not present

// BREW Network Error Codes
#include "../../inc/AEENetErr.h"

//
// Database and Address Book Errors
//

#define DB_ERROR_BASE            0x300

#define  AEE_DB_EBADREC                (DB_ERROR_BASE)
#define  AEE_DB_EBADSTATE              (DB_ERROR_BASE + 1)
#define  AEE_DB_ENULLREC               (DB_ERROR_BASE + 2)
#define  AEE_DB_ENULLFIELD             (DB_ERROR_BASE + 3)
#define  AEE_DB_EBADFIELDNUM           (DB_ERROR_BASE + 4)

//AddrBook Error Codes:
#define  AEE_ADDR_INVALID_FIELD_ID     (DB_ERROR_BASE + 10) //Invalid or Unsupported Field ID
#define  AEE_ADDR_INVALID_FIELD_VALUE  (DB_ERROR_BASE + 11) //Invalid Data Value for a Field
#define  AEE_ADDR_REC_NOT_FOUND        (DB_ERROR_BASE + 12) //Record not found
#define  AEE_ADDR_INVALID_REC          (DB_ERROR_BASE + 13) //Invalid Record
#define  AEE_ADDRBOOK_EBADSTATE        (DB_ERROR_BASE + 14) //Addr Book in Bad State
#define  AEE_ADDRBOOK_NOCATSUPPORT     (DB_ERROR_BASE + 15) //No support for categories
#define  AEE_ADDR_CAT_NOT_SUPPORTED    (DB_ERROR_BASE + 16) //This category not supported
#define  AEE_ADDR_FULL                 (DB_ERROR_BASE + 17) //Address book is full
#define  AEE_ADDR_INVALID_FIELD_LEN    (DB_ERROR_BASE + 18) //Field length too large or small
#define  AEE_ADDR_INVALID_ENUM_FIELD   (DB_ERROR_BASE + 19) //Cannot restrict record enumeration to this field
#define  AEE_ADDR_INVALID_ENUM_CAT     (DB_ERROR_BASE + 20) //Cannot restrict record enumeration to this cateogry
#define  AEE_ADDR_INVALID_ENUM_TYPE    (DB_ERROR_BASE + 21) //Cannot restrict record enumeration to this field type
#define  AEE_ADDR_NO_UIM               (DB_ERROR_BASE + 22) //UIM not present
#define  AEE_ADDR_PASSWORD_REQUIRED    (DB_ERROR_BASE + 23) //Password or UIM PIN or unlock code required
#define  AEE_ADDR_BUSY                 (DB_ERROR_BASE + 24) //Addrbook not ready (e.g. busy reading UIM)
#define  AEE_ADDR_ACCESS_RESTRICTED    (DB_ERROR_BASE + 25) //Addrbook access is restricted

#define  AEE_ADDRBOOK_ERR              (-1)   //unsuccessful operation

// Digital Signature Errors
//
#define  SIG_ERROR_BASE                0x400
#define  SIG_INVALID                   (SIG_ERROR_BASE + 1)    // Invalid signature
#define  SIG_FILE_MISSING              (SIG_ERROR_BASE + 2)    // SIG file missing
#define  SIG_FILE_INVALID              (SIG_ERROR_BASE + 3)    // Invalid SIG file format
#define  SIG_EXPIRED                   (SIG_ERROR_BASE + 4)    // Expired signature
#define  SIG_NO_PERMISSION             (SIG_ERROR_BASE + 5)    // No valid permission
#define  SIG_INVALID_ESN               (SIG_ERROR_BASE + 6)    // ESN is not valid
#define  SIG_SIGNED_FILE_MISSING       (SIG_ERROR_BASE + 7)    // signed file missing
#define  SIG_FILE_INVALID_VERSION      (SIG_ERROR_BASE + 8)    // Invalid SIG file major version
#define  SIG_INVALID_VERSION           (SIG_ERROR_BASE + 9)    // invalid signature major version
#define  SIG_BN_ERROR                  (SIG_ERROR_BASE + 10)   // BN lib error
#define  SIG_INVALID_CMS_FORMAT        (SIG_ERROR_BASE + 11)   // fail to parse CMS
#define  SIG_X509_INVALID_ISSUER       (SIG_ERROR_BASE + 12)   // invalid issuer in X509 cert chain
#define  SIG_X509_INVALID_TIME         (SIG_ERROR_BASE + 13)   // Invalid time format in X509 cert
#define  SIG_X509_EXPIRED              (SIG_ERROR_BASE + 14)   // x509 expired
#define  SIG_X509_INVALID              (SIG_ERROR_BASE + 15)   // invalid X509
#define  SIG_X509_INVALID_PUBKEY       (SIG_ERROR_BASE + 16)   // invalid public key
#define  SIG_X509_INVALID_SERIAL       (SIG_ERROR_BASE + 17)   // invalid serial number
#define  SIG_CERT_MISSING              (SIG_ERROR_BASE + 18)   // missing cert
#define  SIG_INVALID_TS_FORMAT         (SIG_ERROR_BASE + 19)   // invalid TimsStampResp
#define  SIG_INVALID_CMS_SIG           (SIG_ERROR_BASE + 20)   // invalid signature in CMS
#define  SIG_INVALID_ROOT_TYPE         (SIG_ERROR_BASE + 21)   // invalid root type
#define  SIG_INVALID_MSG_IMPRINT       (SIG_ERROR_BASE + 22)   // invalid message imprint in TSRequest
#define  SIG_CANNOT_OPEN_CERTCACHE     (SIG_ERROR_BASE + 23)   // cannot create an instance of the cert cache
#define  SIG_FILE_CANNOT_TRUNCATE      (SIG_ERROR_BASE + 24)   // cannot truncate the sig file to get rid of the certs
#define  SIG_INVALID_BYTE_RANGE        (SIG_ERROR_BASE + 25)   // the signed byte range is invalid for the signed files
#define  SIG_INVALID_TYPE              (SIG_ERROR_BASE + 26)   // invalid signature type
#define  SIG_CMS_TOO_MANY_CERTS        (SIG_ERROR_BASE + 27)   // too many certs, just want to protect us from infinite loop
#define  SIG_INVALID_CERT_PERMISSION   (SIG_ERROR_BASE + 28)   // The permission in the cert doesn't match the permission in the signature blob
#define  SIG_X509_NO_PERMISSION        (SIG_ERROR_BASE + 29)   // Permission list is empty in X509
#define  SIG_NO_MEMORY                 (SIG_ERROR_BASE + 30)   // not enough memory for the MD5 hash buffer
#define  SIG_INVALID_HEADER_BYTE_RANGE (SIG_ERROR_BASE + 31)// invalid header signed byte range
#define  SIG_INVALID_CMS_ECONTENT      (SIG_ERROR_BASE + 32)
#define  SIG_INVALID_CMS_TS            (SIG_ERROR_BASE + 33)
#define  SIG_MISSINGMODMIF_IN_SIG       (SIG_ERROR_BASE + 34)
#define  SIG_ERROR_LAST                (SIG_ERROR_BASE + 34)   


//
// Web Errors
//
#define WEB_ERROR_BASE        0x500

#define WEB_ERROR_PROTOCOL    (WEB_ERROR_BASE+1)  /* protocol error, ask response for extended information */ 
#define WEB_ERROR_BADURL      (WEB_ERROR_BASE+2)  /* malformed URL */
#define WEB_ERROR_BADHOSTNAME (WEB_ERROR_BASE+3)  /* malformed hostname in URL */
#define WEB_ERROR_BADPORT     (WEB_ERROR_BASE+4)  /* malformed port in URL */
#define WEB_ERROR_UNSUPSCHEME (WEB_ERROR_BASE+5)  /* unsupported scheme, no handler found */
#define WEB_ERROR_DNSCONFIG   (WEB_ERROR_BASE+6)  /* no DNS servers configured */
#define WEB_ERROR_DNSTIMEOUT  (WEB_ERROR_BASE+7)  /* no response from server */
#define WEB_ERROR_ADDRUNKNOWN (WEB_ERROR_BASE+8)  /* DNS server responded 'address unknown' */
#define WEB_ERROR_CONNECT     (WEB_ERROR_BASE+9)  /* some kind of connect error */
#define WEB_ERROR_SEND        (WEB_ERROR_BASE+10) /* error writing to socket */
#define WEB_ERROR_RECV        (WEB_ERROR_BASE+11) /* error reading from socket */
#define WEB_ERROR_BADRESPONSE (WEB_ERROR_BASE+12) /* mal-formed headers */
#define WEB_ERROR_BODYLENGTH  (WEB_ERROR_BASE+13) /* a request body was specified, but no content length was supplied,
                                                     or an incorrect body length was supplied */
#define WEB_ERROR_PROXYSPEC   (WEB_ERROR_BASE+14) /* invalid proxy spec */
#define WEB_ERROR_SSL         (WEB_ERROR_BASE+15) /* SSL specific erro, , ask response for extended info */
#define WEB_ERROR_BODYTYPE    (WEB_ERROR_BASE+16) /* invalid requests body, it has to be an instance of IPEEK */


//
// Security Errors
//
#define SECURITY_ERROR_BASE         0x600
#define AEE_HASH_INVALID_CTX        (SECURITY_ERROR_BASE + 1) /* invalid hash algorithm context */
#define AEE_HASH_MORE_DATA          (SECURITY_ERROR_BASE + 2) /* buffer not large enough to hold hashed result */
#define AEE_CRYPT_INVALID_KEY       (SECURITY_ERROR_BASE + 3) /* Invalid key for encrypting or decrypting */
#define AEE_CRYPT_INVALID_PADTYPE   (SECURITY_ERROR_BASE + 4) /* Invalid pad type for encrypting or decrypting */
#define AEE_CRYPT_PAD_ERROR         (SECURITY_ERROR_BASE + 5) /* An error occurred adding padding */
#define AEE_CRYPT_ROLLBACKATTACK    (SECURITY_ERROR_BASE + 6) /* SSLv23 Rollback Attack detected */
#define AEE_CRYPT_BUFFER_TOO_SMALL  (SECURITY_ERROR_BASE + 7) /* buffer not large enough to hold encrypted result */
#define AEE_CRYPT_INVALID_IV        (SECURITY_ERROR_BASE + 8) /* Invalid initialization vector for cipher function */
#define AEE_CRYPT_INVALID_SIZE      (SECURITY_ERROR_BASE + 9) /* Size of data to encrypt/decrypt is invalid */

#define AEE_X509_BAD_CERT           (SECURITY_ERROR_BASE + 20) /* Certificate couldn't be parsed (maybe it's not an X.509 cert*/
#define AEE_X509_BAD_VALIDITY       (SECURITY_ERROR_BASE + 21) /* The validity dates in the cert can't be parsed */
#define AEE_X509_BAD_KEY            (SECURITY_ERROR_BASE + 22) /* The public key is understood but can't be parsed */
#define AEE_X509_UNKNOWN_KEY        (SECURITY_ERROR_BASE + 23) /* Don't understand the type of public key (wrong algorithm) */
#define AEE_X509_UNKNOWN_SIG        (SECURITY_ERROR_BASE + 24) /* The signature on the cert is unknown */
#define AEE_X509_NO_LEAF            (SECURITY_ERROR_BASE + 25) /* Couldn't check chain - there's no leaf cert */
#define AEE_X509_NO_PART            (SECURITY_ERROR_BASE + 26) /* Couldn't find cert field part */
#define AEE_X509_NO_CHAIN_YET       (SECURITY_ERROR_BASE + 27) /* Chain hasn't been computed (verify hasn't been called) */

//
// IVocoder Errors
//
#define VOCODER_ERROR_BASE            0x700
#define AEE_VOC_ENOT_STARTED          (VOCODER_ERROR_BASE + 1)
#define AEE_VOC_ENOT_AVAIL            (VOCODER_ERROR_BASE + 2)
#define AEE_VOC_EIN_USE               (VOCODER_ERROR_BASE + 3)
#define AEE_VOC_ENOT_CONFIGED         (VOCODER_ERROR_BASE + 4)
#define AEE_VOC_ERATE_OUT_OF_RANGE    (VOCODER_ERROR_BASE + 5)
#define AEE_VOC_EBUFFER_EMPTY         (VOCODER_ERROR_BASE + 6)
#define AEE_VOC_EBUFFER_FULL          (VOCODER_ERROR_BASE + 7)
#define AEE_VOC_EACTIVE               (VOCODER_ERROR_BASE + 8)
#define AEE_VOC_EBAD_TX_REDUCTION     (VOCODER_ERROR_BASE + 9)

//
// Multimedia (IMedia) Errors
//
#define MM_ERROR_BASE         0x900
#define MM_ERROR              (-1)           // Command failed
#define MM_PENDING            (-2)           // Command accepted (queued). Result returned via registered callback.

#define MM_EBADMEDIADATA      (MM_ERROR_BASE + 1)  // Bad media data
#define MM_ENOMEDIAMEMORY     (MM_ERROR_BASE + 2)  // No memory for IMedia objects
#define MM_EMIDIOUTNOTOPEN    (MM_ERROR_BASE + 3)  // MIDI Out Device is not open
#define MM_EPEEK              (MM_ERROR_BASE + 4)  // IPeek error occurred
#define MM_ENOTENOUGHTDATA    (MM_ERROR_BASE + 5)  // Operation aborted due to insufficient data

//
// SSL Protocol Errors (also see security errors)
//
#define SSL_ERROR_BASE                0xa00
#define SSL_RESULT_SECURED            (SSL_ERROR_BASE+0) // Connection was secured
#define SSL_RESULT_ALGS_ERR           (SSL_ERROR_BASE+1) // No algorithms in common, May SSL_RESULT_HANDSHAKE instead
#define SSL_RESULT_TRUST_ERR          (SSL_ERROR_BASE+2) // Overridable trust error, see IX509Chain
#define SSL_RESULT_CERT_ERR           (SSL_ERROR_BASE+3) // Non overridable cert problem
#define SSL_RESULT_PROTO_ERR	        (SSL_ERROR_BASE+4) // Some fundamental error with the protocol interaction. 
#define SSL_RESULT_CLIENT_AUTH_ERR    (SSL_ERROR_BASE+5) // Client authentication is required by server
#define SSL_RESULT_CLOSED             (SSL_ERROR_BASE+6) // Session was securely closed
#define SSL_RESULT_HANDSHAKE          (SSL_ERROR_BASE+7) // Something went wrong with the handshake, like a bad mac error
#define SSL_RESULT_UNINITIALIZED      (SSL_ERROR_BASE+8) // You forgot to setup something in ISSL, like the server's socket
#define SSL_RESULT_ERR                (SSL_ERROR_BASE+9) // Something very wrong
#define SSL_RESULT_VERS_UNSUP         (SSL_ERROR_BASE+10) // Client doesn't support any of request SSL proto versions
#define SSL_RESULT_CSUITE_UNSUP       (SSL_ERROR_BASE+11) // Client doesn't support any of requested cipher suites
#define SSL_RESULT_SERV_VERS          (SSL_ERROR_BASE+12) // Server doesn't support protocol version, or version
#define SSL_RESULT_TRUST_CHECK        (SSL_ERROR_BASE+13) // Suspended pending trust checks by the client
#define SSL_RESULT_SOCKET_ERROR       (SSL_ERROR_BASE+14) // There is a socket error. Use ISOCKET_GetLastError to get the socket error code
#define SSL_RESULT_SECURITY_LEVEL_ERR (SSL_ERROR_BASE+15) // Security level mismatch when negotiate called on secured ISSL
#define SSL_RESULT_CLOSE_NOTIFY_SENT  (SSL_ERROR_BASE+16) // Close Notify is sent, but has not received server's close notify
#define SSL_RESULT_BAD_SERVER_KEY     (SSL_ERROR_BASE+17) // Server's key is bad (e.g., the key in the server's cert)
#define SSL_RESULT_BAD_STATE          (SSL_ERROR_BASE+18) // SSL is not in the right state. Most likely a previously call is in progress
                                                          // and needs to be explicitly cancelled
// BREW Camera Errors
#define CAM_ERROR_BASE              0xb00
#define CAM_PENDING                 (-1)                 // Command accepted (queued). Result returned via registered callback.
#define CAM_EPREVIEWFAIL            (CAM_ERROR_BASE)     // Start preview failed
#define CAM_ESNAPSHOTFAIL           (CAM_ERROR_BASE + 1) // Record snaphot failed
#define CAM_ESAVEFAIL               (CAM_ERROR_BASE + 2) // Save snapshot failed
#define CAM_EMOVIEFAIL              (CAM_ERROR_BASE + 3) // Start movie failed
#define CAM_EPAUSEFAIL              (CAM_ERROR_BASE + 4) // Pause operation failed
#define CAM_ERESUMEFAIL             (CAM_ERROR_BASE + 5) // Resume operation failed
#define CAM_ESETPARMFAIL            (CAM_ERROR_BASE + 6) // SetParm operation failed

//BREW PORT error codes
#include "../../inc/AEEPortErr.h"
//#define AEEPORT_ERROR_BASE           0xc00
//#define AEEPORT_EINUSE               (AEEPORT_ERROR_BASE + 0)
//#define AEEPORT_ENOTOPEN             (AEEPORT_ERROR_BASE + 1)

//
//BREW Mutual Authentication error codes
//
#define AEEMA_ERROR_BASE           0xd00
#define AEEMA_MUTUALAUTH_FAILED    (AEEMA_ERROR_BASE + 0)
#define AEEMA_MAC_FAILED           (AEEMA_ERROR_BASE + 1)
#define AEEMA_BAD_NONCE            (AEEMA_ERROR_BASE + 2)
#define AEEMA_REREG_AND_DELETE     (AEEMA_ERROR_BASE + 3)
#define AEEMA_CERT_INVALID         (AEEMA_ERROR_BASE + 4)
#define AEEMA_INVALID_CRYPTO_SUITE (AEEMA_ERROR_BASE + 5)
#define AEEMA_REREG                (AEEMA_ERROR_BASE + 6)
#define AEEMA_FILE_ERROR           (AEEMA_ERROR_BASE + 7)

// Gap here for MA background app status codes...

#define AEEMA_UPDATE_NN           (AEEMA_ERROR_BASE + 15)  // Not fatal err - generated by ADS, asking client to get new NN
#define AEEMA_UPDATE_TIME         (AEEMA_ERROR_BASE + 16)  // Not fatal err - generated by ADS, asking client to get new time
#define AEEMA_UPDATE_NN_AND_TIME  (AEEMA_ERROR_BASE + 17)  // Not fatal err - generated by ADS, asking client to get new NN&time

#define AEEMA_NO_NN               (AEEMA_ERROR_BASE + 18)  // Fatal err - ADS has asked us to get new NN, but it is not present or bad
#define AEEMA_NO_TIME             (AEEMA_ERROR_BASE + 19)  // Fatal err - ADS has asked us to get new time, but it is not present or bad

#define AEEMA_BAD_CLIENT_TIME     (AEEMA_ERROR_BASE + 20)  // Fatal err - client supplied time is bad even after multiple attempts
#define AEEMA_BAD_CLIENT_NN       (AEEMA_ERROR_BASE + 21)  // Fatal err - client supplied NN is bad even after multiple attempts

#define AEEMA_TIME_REQUEST_FAILED (AEEMA_ERROR_BASE + 22)  // Fatal err code set by ADS to indicate that it failed to supply time update

#define AEEMA_BUSY                (AEEMA_ERROR_BASE + 23)  // Request cannot be completed at this time. 
                                                           // (Most likely reason being that MA state machine is busy)
/* AEEMA_ERROR_BASE + 24 is reserved for AEEMA_BAD_NNRESP */
#define AEEMA_NOLOCK              (AEEMA_ERROR_BASE + 25)  // Mutex has not been acquired when MA requests are sent. This is an internal error.

// 
// IDRM error codes
//
// The error codes from 0xe00 - 0xeff are reserved
// for IDRM and defined in AEEDRM.h
//

//
// IDownload error codes
//
#define AEEDL_ERROR_BASE		0x1000
#define AEEDL_PURCHASEAUTH_DENIED	(AEEDL_ERROR_BASE + 1)	
#define AEEDL_FOTA_FILE_MISSING		(AEEDL_ERROR_BASE + 2) // fota file doesn't exist in the downloaded package
#define AEEDL_FOTA_FILE_CORRUPT		(AEEDL_ERROR_BASE + 3) // downloaded fota file corrupted -- readable size doesn't match file info size
#define AEEDL_FOTA_NOCREATE	(AEEDL_ERROR_BASE + 4) // cannot create the fota partition for read/write
#define AEEDL_FOTA_NOOPEN	(AEEDL_ERROR_BASE + 5) // cannot open the fota partition 
#define AEEDL_FOTA_NOWRITE	(AEEDL_ERROR_BASE + 6) // write to the FOTA partition failed
#define AEEDL_NETWORK_READ_ERR (AEEDL_ERROR_BASE + 7) // read from network gave error
#define AEEDL_CACHE_READ_ERR (AEEDL_ERROR_BASE + 8) // read from cache file gave error

//
// IResourceCtl (and ResourceArbiter) related error codes
// Reserved range: 0x1200 - 0x13ff
//
#define AEERESCTL_ERROR_BASE		               0x1200
#define AEERESCTL_ERROR_RESOURCE_NOT_GRANTED    (AEERESCTL_ERROR_BASE)  // System/ResArbiter has not granted access to resource

//
// Private Error Codes - Class Defined
//
#define  ERROR_USER           (0x4000)

#endif   // AEEERROR_H

/*============================================================================
   ERROR DOCUMENTATION
==============================================================================
Error Codes

This section lists the AEE errors returned, the codes associated with the errors, and
descriptions of the errors.
===H2>
List of errors
===/H2>
===p>
The error codes are grouped as follows:~
~

Basic error codes

File error codes

SSL error codes

Database error codes

AddrBook error codes

Web error codes

VOCODER error codes

Multimedia error codes

PosDet error codes

ISSL error codes

ICamera error codes

IX509Chain error codes  
*

==================================================================
Basic error codes

Definition:
 
Error                   Code                    Description
 
SUCCESS                 0                       No error.
AEE_SUCCESS             0                       Operation Successful
EFAILED                 1                       General failure.
ENOMEMORY               2                       Out of memory.
ECLASSNOTSUPPORT        3                       Specified class unsupported.
EVERSIONNOTSUPPORT      4                       Version not supported.
EALREADYLOADED          5                       Object already loaded.
EUNABLETOLOAD           6                       Unable to load object/applet.
EUNABLETOUNLOAD         7                       Unable to unload object/applet.
EALARMPENDING           8                       Alarm is pending.
EINVALIDTIME            9                       Invalid time.
EBADCLASS               10                      NULL class object.
EBADMETRIC              11                      Invalid metric specified.
EEXPIRED                12                      Application/Component Expired.
EBADSTATE               13                      Invalid state.
EBADPARM                14                      Invalid parameter.
ESCHEMENOTSUPPORTED     15                      Invalid URL scheme.
EBADITEM                16                      Invalid item.
EINVALIDFORMAT          17                      Invalid format.
EINCOMPLETEITEM         18                      Incomplete item.
ENOPERSISTMEMORY        19                      Insufficient flash.
EUNSUPPORTED            20                      API is not supported.
EPRIVLEVEL              21                      Security violation.
ERESOURCENOTFOUND       22
EREENTERED              23
EBADTASK                24
EALLOCATED              25                      Application/Module left memory allocated when released..
EALREADY                26                      Operation is already in progress.
EADSAUTHBAD             27                      ADS mutual authorization failed.
ENEEDSERVICEPROG        28                      Need service programming.
EMEMPTR                 29                      bad memory pointer.
EHEAP                   30                      heap corruption.
EIDLE                   31                      Context (system, interface, etc.) is idle.
EITEMBUSY               32                      Context (system, interface, etc.) is busy.
EBADSID                 33                      Invalid subscriber ID
ENOTYPE                 34                      No type detected/found
ENEEDMORE               35                      Need more data/info
EADSCAPS                36                      ADS Capabilities do not match those required for device 
EBADSHUTDOWN            37                      Application failed to close properly 
 
==============================================================================

File error codes

Definition:

File error code equals the base FILE_ERROR(0x100) plus the code from the table.


Error                   Code                    Description
 
EFILEEXISTS             0                       File exists.
EFILENOEXISTS           1                       File does not exist.
EDIRNOTEMPTY            2                       Directory not empty.
EBADFILENAME            3                       Bad file name.
EBADSEEKPOS             4                       Bad seek position.
EFILEEOF                5                       End of file.
EFSFULL                 6                       File system full.
EFILEOPEN               7                       File already open.
EOUTOFNODES             8                       No INODES (file slots) available
EDIRNOEXISTS            9                       Directory does not exist
EINVALIDOPERATION      10                       invalid operation
ENOMEDIA               11                       Storage media not present

==============================================================================

Database error codes

Definition:

These error codes are also valid for Address Book error codes.
The database error code equals the base DB_ERROR_BASE(0x300) plus the code from the
table.


Error                         Code                    Description
 
AEE_DB_EBADREC                0
AEE_DB_EBADSTATE              1
AEE_DB_ENULLREC               2
AEE_DB_ENULLFIELD             3
AEE_DB_EBADFIELDNUM           4

==============================================================================

AddrBook error codes

Definition:

The database error code equals the base DB_ERROR_BASE(0x300) plus the code from the
table.

Error                         Code        Description
 
AEE_ADDR_INVALID_FIELD_ID     10          Invalid or Unsupported Field ID.
AEE_ADDR_INVALID_FIELD_VALUE  11          Invalid Data Value for a Field.
AEE_ADDR_REC_NOT_FOUND        12          Record not found.
AEE_ADDR_INVALID_REC          13          Invalid Record.
AEE_ADDRBOOK_EBADSTATE        14          Addr Book in Bad State.
AEE_ADDRBOOK_NOCATSUPPORT     15          No support for categories.
AEE_ADDR_CAT_NOT_SUPPORTED    16          This category not supported.
AEE_ADDR_FULL                 17          Address book is full
AEE_ADDR_INVALID_FIELD_LEN    18          Field length too large or small
AEE_ADDR_INVALID_ENUM_FIELD   19          Cannot restrict record enumeration to this field
AEE_ADDR_INVALID_ENUM_CAT     20          Cannot restrict record enumeration to this cateogry
AEE_ADDR_INVALID_ENUM_TYPE    21          Cannot restrict record enumeration to this field type
AEE_ADDR_NO_UIM               22          UIM not present
AEE_ADDR_PASSWORD_REQUIRED    23          Password or UIM PIN or unlock code required
AEE_ADDR_BUSY                 24          Addrbook not ready (e.g. busy reading UIM)
AEE_ADDR_ACCESS_RESTRICTED    25          Addrbook access is restricted
AEE_ADDRBOOK_ERR              -1          Unsuccessful operation.

==============================================================================

Web error codes

Definition:

The web error code equals the base WEB_ERROR_BASE(0x500) plus the code from the table.

Error                         Code        Description
 
WEB_ERROR_PROTOCOL            1           Protocol error. The value or associated error message is returned in response to
                                          an "ask for extended information" that cannot be processed because the protocol
                                          is invalid.
WEB_ERROR_BADURL              2           Malformed URL.
WEB_ERROR_BADHOSTNAME         3           Malformed hostname in URL.
WEB_ERROR_BADPORT             4           Malformed port in URL.
WEB_ERROR_UNSUPSCHEME         5           Unsupported scheme, no handler found.
WEB_ERROR_DNSCONFIG           6           No DNS servers configured.
WEB_ERROR_DNSTIMEOUT          7           No response from server.
WEB_ERROR_ADDRUNKNOWN         8           DNS server responded 'address unknown.
WEB_ERROR_CONNECT             9           Some kind of connect error.
WEB_ERROR_SEND                10          Error writing to socket.
WEB_ERROR_RECV                11          Error reading from socket.
WEB_ERROR_BADRESPONSE         12          Malformed headers.
WEB_ERROR_BODYLENGTH          13          A request body was specified, but no content length was supplied, or an incorrect
                                          body length was supplied.
WEB_ERROR_PROXYSPEC           14          Invalid proxy spec.
WEB_ERROR_SSL                 15          SSL specific error, ask response for extended info
 
==============================================================================

VOCODER error codes

Definition:

The VOCODER error code equals the base VOCODER_ERROR_BASE (0x700) plus the code from
the table.

Error                         Code        Description
 
AEE_VOC_STOP_PENDING          1
AEE_VOC_ENOT_AVAIL            2
AEE_VOC_EIN_USE               3
AEE_VOC_ENOT_IN_USE           4
AEE_VOC_ENOT_CONFIGED         5
AEE_VOC_ERATE_OUT_OF_RANGE    6
AEE_VOC_EDURATION_TOO_LONG    7
AEE_VOC_EDURATION_TOO_SHORT   8
AEE_VOC_EBUFFER_IN_USE        9
AEE_VOC_EBUFFER_NOT_EMPTY     10
AEE_VOC_EBUFFER_EMPTY         11
AEE_VOC_EBUFFER_FULL          12
AEE_VOC_EINCOMPATIBLE_CONFIG  13
AEE_VOC_EINCOMPATIBLE_OP      14

==============================================================================

Multimedia error codes

Definition:

The multimedia error code equals the base MM_ERROR_BASE (0x900) plus the code from the
table.

Error                         Code        Description
 
MM_ERROR                      -1          Command failed
MM_PENDING                    -2          Command accepted queued. Result returned via registered callback.
MM_EBADMEDIADATA              1           Bad media data
MM_ENOMEDIAMEMORY             2           No memory for IMedia objects
MM_EMIDIOUTNOTOPEN            3           MIDI Out Device is not open 
MM_EPEEK                      4           IPeek error occurred
MM_ENOTENOUGHTDATA            5           Operation aborted due to insufficient data 

==============================================================================

PosDet error codes

Definition:

The PosDet error code equals the base AEEGPS_ERR_BASE (0x00100000) plus the code from
the table.

Error                         Code        Description
 
AEEGPS_ERR_NO_ERR             0           Success
AEEGPS_ERR_GENERAL_FAILURE    1           General failure
AEEGPS_ERR_TIMEOUT            2           Timed out
AEEGPS_ERR_ACCURACY_UNAVAIL   3           Accuracy unavailable
AEEGPS_ERR_INFO_UNAVAIL       4           Information unavailable

==============================================================================

ISSL error codes

Definition:

The ISSL error code equals the base ISSL_ERROR_BASE (0xb00) plus the code from the table.

Error                         Code        Description
 
SSL_RESULT_SECURED            0           Connection was secured
SSL_RESULT_ALGS_ERR           1           No algorithms in common, though this might indicate some other error it is set
                                          when a HANDSHAKE alert is sent which is not always algs error in SSL3. It is in
                                          TLSv1
SSL_RESULT_TRUST_ERR          2           There was some trust error. See the errors TrustResult
SSL_RESULT_CERT_ERR           3           There was some un overridable problem with the certificate chain. See TrustResult
SSL_RESULT_PROTO_ERR          4           Some fundamental error with the protocol interaction. 
SSL_RESULT_CLIENT_AUTH_ERR    5           Client authentication is required by server
SSL_RESULT_CLOSED             6           Session was securely closed
SSL_RESULT_HANDSHAKE          7           Something went wrong with the handshake, like a bad mac error
SSL_RESULT_NOMEM              8           Ran out of memory. Couldn't go on.
SSL_RESULT_UNINITIALIZED      9           You forgot to setup something in ISSL, like the server's socket
SSL_RESULT_ERR                10          You are doing something very wrong, you're lucky we caught this and didn't crash
                                          your device 
SSL_RESULT_BAD_SOCKET         11          This Brew build doesn't support the socket hack needed for SSL. This should go
                                          away eventually
SSL_RESULT_CONFIG             12          Some internal config error. You're probably stuck if you get this
SSL_RESULT_VERS_UNSUP         13          Client doesn't support any of request SSL proto versions
SSL_RESULT_CSUITE_UNSUP       14          Client doesn't support any of requested cipher
                                          suites
SSL_RESULT_SERV_VERS          15          Server doesn't support protocol version, or version
SSL_RESULT_TRUST_CHECK        16          Suspended pending trust checks by the client
SSL_RESULT_SOCKET_ERROR       17          There is a socket error. Use ISOCKET_GetLastError to get the socket error code
SSL_RESULT_BAD_STATE          18          SSL is not in the right state. Most likely a previously call is in progress and
                                          needs to be explicitly cancelled

=================================================================================
ICamera error codes

Definition:

The ICamera error code equals the base CAM_ERROR_BASE (0xa00) plus the code from the
table.

Error                         Code        Description

CAM_PENDING                    -1          Command accepted (queued). Result returned via 
                                           registered callback.
CAM_EPREVIEWFAIL                0          Start preview failed
CAM_ESNAPSHOTFAIL             + 1          Record snaphot failed
CAM_ESAVEFAIL                 + 2          Save snapshot failed
CAM_EMOVIEFAIL                + 3          Start movie failed
CAM_EPAUSEFAIL                + 4          Pause operation failed
CAM_ERESUMEFAIL               + 5          Resume operation failed
CAM_ESETPARMFAIL              + 6          SetParm operation failed

======================================================================================
IX509Chain error codes

Definition:

The IX509Chain error code equals the base SECURITY_ERROR_BASE (0x600) plus the code
from the table.

Error                         Description

AEE_X509_BAD_CERT     + 20        Certificate couldn't be parsed (maybe it's not an X.509 cert
AEE_X509_BAD_VALIDITY + 21        The validity dates in the cert can't be parsed 
AEE_X509_BAD_KEY      + 22        The public key is understood but can't be parsed
AEE_X509_UNKNOWN_KEY  + 23        Don't understand the type of public key (wrong algorithm) 
AEE_X509_UNKNOWN_SIG  + 24        The signature on the cert is unknown 
AEE_X509_NO_LEAF      + 25        Couldn't check chain - there's no leaf cert 
AEE_X509_NO_PART      + 26        Couldn't find cert field part 
AEE_X509_NO_CHAIN_YET + 27        Chain hasn't been computed (verify hasn't been called) 

============================================================================*/

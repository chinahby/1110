#ifndef AEENETERR_H
#define AEENETERR_H
/*
=======================================================================
Copyright © 2005-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=======================================================================
*/

#define NET_ERROR_BASE     (0x200)

/*---------------------------------------------------------------------------
                       Network Errors
---------------------------------------------------------------------------*/

#define AEE_NET_SUCCESS          (0)   //successful operation
#define AEE_NET_ERROR            (-1)   //unsuccessful operation
#define AEE_NET_WOULDBLOCK       (-2)
#define AEE_STREAM_ERROR         (AEE_NET_ERROR)
#define AEE_STREAM_WOULDBLOCK    (AEE_NET_WOULDBLOCK)

#define AEE_NET_EEOF             (NET_ERROR_BASE+0x01) // end of file
#define AEE_NET_EBADF            (NET_ERROR_BASE+0x02) // Invalid socket descriptor
#define AEE_NET_EFAULT           (NET_ERROR_BASE+0x03) // Invalid buffer or argument
#define AEE_NET_EWOULDBLOCK      (NET_ERROR_BASE+0x04) // Operation would block
#define AEE_NET_EAFNOSUPPORT     (NET_ERROR_BASE+0x05) // Address family not supported
#define AEE_NET_EPROTOTYPE       (NET_ERROR_BASE+0x06) // Wrong protocol for socket type
#define AEE_NET_ESOCKNOSUPPORT   (NET_ERROR_BASE+0x07) // Socket parameter not supported
#define AEE_NET_EPROTONOSUPPORT  (NET_ERROR_BASE+0x08) // Protocol not supported
#define AEE_NET_EMFILE           (NET_ERROR_BASE+0x09) // No more sockets available for opening
#define AEE_NET_EOPNOTSUPP       (NET_ERROR_BASE+0x0A) // Operation not supported
#define AEE_NET_EADDRINUSE       (NET_ERROR_BASE+0x0B) // Address already in use
#define AEE_NET_EADDRREQ         (NET_ERROR_BASE+0x0C) // Destination address required
#define AEE_NET_EINPROGRESS      (NET_ERROR_BASE+0x0D) // Connection establishment in progress
#define AEE_NET_EISCONN          (NET_ERROR_BASE+0x0E) // Connection already established
#define AEE_NET_EIPADDRCHANGED   (NET_ERROR_BASE+0x0F) // IP address changed, causing TCP reset
#define AEE_NET_ENOTCONN         (NET_ERROR_BASE+0x10) // socket not connected
#define AEE_NET_ECONNREFUSED     (NET_ERROR_BASE+0x11) // Connection attempt refused
#define AEE_NET_ETIMEDOUT        (NET_ERROR_BASE+0x12) // Connection attempt timed out
#define AEE_NET_ECONNRESET       (NET_ERROR_BASE+0x13) // Connection reset
#define AEE_NET_ECONNABORTED     (NET_ERROR_BASE+0x14) // Connection aborted
#define AEE_NET_EPIPE            (NET_ERROR_BASE+0x15) // Broken pipe
#define AEE_NET_ENETDOWN         (NET_ERROR_BASE+0x16) // Network subsystem unavailable
#define AEE_NET_EMAPP            (NET_ERROR_BASE+0x17) // no more applications available
#define AEE_NET_EBADAPP          (NET_ERROR_BASE+0x18) // Invalid application ID
#define AEE_NET_SOCKEXIST        (NET_ERROR_BASE+0x19) // there are existing sockets
#define AEE_NET_EINVAL           (NET_ERROR_BASE+0x1A) // invalid operation
#define AEE_NET_EMSGSIZE         (NET_ERROR_BASE+0x1B) // message too long
#define AEE_NET_EBADOPTNAME      (NET_ERROR_BASE+0x1C) // bad option name
#define AEE_NET_EBADOPTLEN       (NET_ERROR_BASE+0x1D) // bad option len
#define AEE_NET_EBADOPTVAL       (NET_ERROR_BASE+0x1E) // bad option val
#define AEE_NET_ENOMEM           (NET_ERROR_BASE+0x1F) // out of memory
#define AEE_NET_ESHUTDOWN        (NET_ERROR_BASE+0x20) // connection shutdown
#define AEE_NET_EURGENTFAILED    (NET_ERROR_BASE+0x21) // urgent sendto failed
#define AEE_NET_ENOPROTOOPT      (NET_ERROR_BASE+0x22) // the option is unknown at the level indicated
#define AEE_NET_ENOBUFS          (NET_ERROR_BASE+0x23) // system lacking sufficiant buffer space

/*---------------------------------------------------------------------------
                       Network Subsystem Errors
---------------------------------------------------------------------------*/
#define AEE_NET_ENETISCONN           (NET_ERROR_BASE+0x28) // subsystem established and available
#define AEE_NET_ENETINPROGRESS       (NET_ERROR_BASE+0x29) // subsystem establishment in progress
#define AEE_NET_ENETNONET            (NET_ERROR_BASE+0x2A) // network subsystem unavailable
#define AEE_NET_ENETCLOSEINPROGRESS  (NET_ERROR_BASE+0x2B) // PPP is closing
#define AEE_NET_ENETEXIST            (NET_ERROR_BASE+0x2C) // existing net subsystem resources
#define AEE_NET_GENERAL_FAILURE      (NET_ERROR_BASE+0x2D) // General Failure

/*---------------------------------------------------------------------------
                       DNS / Resolver Errors
---------------------------------------------------------------------------*/
#define AEE_NET_BADDOMAIN            (NET_ERROR_BASE+0x32) // Improperly formatted domain name
#define AEE_NET_UNKDOMAIN            (NET_ERROR_BASE+0x33) // Unknown domain/no address records
#define AEE_NET_BADRESPONSE          (NET_ERROR_BASE+0x34) // Could not make sense of response

/*---------------------------------------------------------------------------
                       SSL Error Codes
---------------------------------------------------------------------------*/
#define AEE_NET_EBADMAC            (NET_ERROR_BASE+0x3C) //Incorrect MAC transerring data
#define AEE_NET_ESSLCLIENTERR      (NET_ERROR_BASE+0x3D) //Internal client-side SSL error
#define AEE_NET_ESSLSERVERERR      (NET_ERROR_BASE+0x3E) //SSL server error: sent an alert, too much data, bad padding...

/*---------------------------------------------------------------------------
                       QoS Error Codes
---------------------------------------------------------------------------*/
#define AEE_NET_EQOS_SPEC_INVALID_OPT (NET_ERROR_BASE+0x50) //Invalid options in a QoS Spec
#define AEE_NET_EQOS_SESSION_INVALID_OPT  (NET_ERROR_BASE+0x51) //Invalid options in a QoS Session
#define AEE_NET_EQOS_NET_CONFLICT         (NET_ERROR_BASE+0x52) //Conflict in QoS Session network types
#define AEE_NET_EQOSUNAWARE               (NET_ERROR_BASE+0x53) //QoS is not supported by the current CDMA system

/*---------------------------------------------------------------------------
                       GetAddrInfo Error Codes
---------------------------------------------------------------------------*/
#define AEE_NET_EAI_BADFLAGS     (NET_ERROR_BASE+0x71)   // The flags parameter had an invalid value
#define AEE_NET_EAI_BADREQUEST   (NET_ERROR_BASE+0x72)   // Request paramters are in error,
                                                          // i.e. nodename and servname are both NULL
#define AEE_NET_EAI_UNSUPPFLAGS  (NET_ERROR_BASE+0x73)   // The flags parameter had an unsupported value

/*---------------------------------------------------------------------------
                       BCMCS DB Error Codes
---------------------------------------------------------------------------*/
#define AEE_NET_EBCMCSDB_FULL             (NET_ERROR_BASE+0x80)   // DB is full
#define AEE_NET_EBCMCSDB_EXIST            (NET_ERROR_BASE+0x81)   // Matching DB record exists

/*---------------------------------------------------------------------------
                       ICMP Error Codes
---------------------------------------------------------------------------*/
#define AEE_NET_ENETUNREACH             (NET_ERROR_BASE+0x90)   // Network is unreachable
#define AEE_NET_EHOSTUNREACH            (NET_ERROR_BASE+0x91)   // Host is unreachable
#define AEE_NET_EHOSTDOWN               (NET_ERROR_BASE+0x92)   // Host is down
#define AEE_NET_ENONET                  (NET_ERROR_BASE+0x93)   // Host is not on the network
#define AEE_NET_EPROTO                  (NET_ERROR_BASE+0x94)   // Protocol error
#define AEE_NET_EACCES                  (NET_ERROR_BASE+0x95)   // Access denied

/*============================================================================
   ERRORS DOCUMENTATION
==============================================================================
Network AEE error codes

Definition:

   Network error code equals the base NET_ERROR_BASE(0x200) plus the code from the
   table.

Error                   Code                    Description

AEE_NET_SUCCESS         0                       successful operation
AEE_NET_ERROR           -1                      unsuccessful operation
AEE_NET_WOULDBLOCK      -2
AEE_STREAM_ERROR
AEE_NET_ERROR
AEE_STREAM_WOULDBLOCK
AEE_NET_WOULDBLOCK
AEE_NET_EEOF            1                       end of file
AEE_NET_EBADF           2                       Invalid socket descriptor
AEE_NET_EFAULT          3                       Invalid buffer or argument
AEE_NET_EWOULDBLOCK     4                       Operation would block
AEE_NET_EAFNOSUPPORT    5                       Address family not supported
AEE_NET_EPROTOTYPE      6                       Wrong protocol for socket type
AEE_NET_ESOCKNOSUPPORT  7                       Socket parameter not supportedL
AEE_NET_EPROTONOSUPPORT 8                       Protocol not supported
AEE_NET_EMFILE          9                       No more sockets available for opening
AEE_NET_EOPNOTSUPP      10                      Operation not supported
AEE_NET_EADDRINUSE      11                      Address already in use
AEE_NET_EADDRREQ        12                      Destination address required
AEE_NET_EINPROGRESS     13                      Connection establishment in progress
AEE_NET_EISCONN         14                      Connection already established
AEE_NET_EIPADDRCHANGED  15                      IP address changed, causing TCP reset
AEE_NET_ENOTCONN        16                      socket not connected
AEE_NET_ECONNREFUSED    17                      Connection attempt refused
AEE_NET_ETIMEDOUT       18                      Connection attempt timed out
AEE_NET_ECONNRESET      19                      Connection reset
AEE_NET_ECONNABORTED    20                      Connection aborted
AEE_NET_EPIPE           21                      Broken pipe
AEE_NET_ENETDOWN        22                      Network subsystem unavailable
AEE_NET_EMAPP           23                      no more applications available
AEE_NET_EBADAPP         24                      Invalid application ID
AEE_NET_SOCKEXIST       25                      there are existing sockets
AEE_NET_EINVAL          26                      invalid operation
AEE_NET_EMSGSIZE        27                      message too long
AEE_NET_EBADOPTNAME     28                      bad option name
AEE_NET_EBADOPTLEN      29                      bad option len
AEE_NET_EBADOPTVAL      30                      bad option val
AEE_NET_ENOMEM          31                      out of memory
AEE_NET_ESHUTDOWN       32                      connection shutdown
AEE_NET_EURGENTFAILED   33                      urgent sendto failed
AEE_NET_ENOPROTOOPT     34                      the option is unknown at the level indicated
AEE_NET_ENOBUFS         35                      system lacking sufficiant buffer space

==============================================================================

Network subsystem error codes

Definition:

   Network error code equals the base NET_ERROR_BASE(0x200) plus the code from the
   table.

Error                         Code                    Description

AEE_NET_ENETISCONN            40                      subsystem established and available.
AEE_NET_ENETINPROGRESS        41                      subsystem establishment in progress.
AEE_NET_ENETNONET             42                      network subsystem unavailable.
AEE_NET_ENETCLOSEINPROGRESS   43                      PPP is closing.
AEE_NET_ENETEXIST             44                      existing net subsystem resources.
AEE_NET_GENERAL_FAILURE       45                      General failure.

==============================================================================

DNS Resolver error codes

Definition:

   Network error code equals the base NET_ERROR_BASE(0x200) plus the code from the
   table.


Error                         Code                    Description

AEE_NET_BADDOMAIN             50                      Improperly formatted domain name.
AEE_NET_UNKDOMAIN             51                      Unknown domain/no address records.
AEE_NET_BADRESPONSE           52                      Could not make sense of response

==============================================================================

SSL error codes

Definition:

Error                         Code                    Description

AEE_NET_EBADMAC               60                      Incorrect MAC transerring data
AEE_NET_ESSLCLIENTERR         61                      Internal client-side SSL error
AEE_NET_ESSLSERVERERR         62                      SSL server error: sent an alert, too much data, bad padding...

==============================================================================

QoS error codes

Definition:

   QoS error code equals the base NET_ERROR_BASE(0x200) plus the code from the
   table.

Error                         Code                    Description

AEE_NET_EQOS_SPEC_INVALID_OPT 80                      Invalid options in a QoS Spec.
AEE_NET_EQOS_SESSION_INVALID_OPT 81                   Invalid options in a QoS Session
AEE_NET_EQOS_NET_CONFLICT        82                   Conflict in QoS Session network types
AEE_NET_EQOSUNAWARE              83                   QoS is not supported by the current CDMA system

==============================================================================

AddrInfo error codes

Definition:

   Network error code equals the base NET_ERROR_BASE(0x200) plus the code from the
   table.


Error                         Code                    Description

AEE_NET_EAI_BADFLAGS          113                     The flags parameter had an invalid value
AEE_NET_EAI_BADREQUEST        114                     Request paramters are in error, i.e. nodename and servname are both NULL
AEE_NET_EAI_UNSUPPFLAGS       115                     The flags parameter had an unsupported value

==============================================================================

==============================================================================

ICMP error codes

Definition:

   ICMP error code equals the base NET_ERROR_BASE(0x200) plus the code from the
   table.

Error                         Code                    Description

AEE_NET_ENETUNREACH           144                     Network is unreachable
AEE_NET_EHOSTUNREACH          145                     Host is unreachable
AEE_NET_EHOSTDOWN             146                     Host is down
AEE_NET_ENONET                147                     Host is not on the network
AEE_NET_EPROTO                148                     Protocol error
AEE_NET_EACCES                149                     Access denied
==============================================================================*/

#endif /* #ifndef AEENETERR_H */

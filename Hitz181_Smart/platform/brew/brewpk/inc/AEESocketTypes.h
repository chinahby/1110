#ifndef AEESOCKETTYPES_H
#define AEESOCKETTYPES_H
/*=============================================================================

FILE:         AEESocketTypes.h

SERVICES:     None

DESCRIPTION:  Common socket types and definitions

===============================================================================
   Copyright (c) 2006-2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEStdDef.h"
#include "AEENetAddrTypes.h"

// Socket Protocol Values / Socket Option levels
#define AEE_IPPROTO_UNSPEC     0 // Unspecified Protocol
#define AEE_IPPROTO_TCP        2 // TCP protocol
#define AEE_IPPROTO_IP         3 // IP protocol
#define AEE_IPPROTO_UDP        4 // UDP protocol
#define AEE_IPPROTO_IPV6       5 // IPv6 protocl

// Socket Option only levels
#define AEE_SOL_SOCKET         1 // socket level

// IoBlock
typedef struct
{
  byte      *pbBuffer;   // Data Buffer
  uint16    wLen;   // Length of Buffer
} SockIOBlock;

// Shutdown "howto" codes
#define AEE_SHUTDOWN_RD   0     // no more reads
#define AEE_SHUTDOWN_WR   1     // no more writes
#define AEE_SHUTDOWN_RDWR 2     // no more reads or writes

//
// Socket Options
//

// pack the setopt/getopt args for funneling through IOCtl()
typedef struct {
   int nLevel;
   int nOptName;
   void* pOptVal;
   int* pnOptSize;
} AEESockOpt;

// optval for AEE_SO_LINGER
typedef struct {
   boolean bOn;                 // linger on or off
   uint32  dwLinger;            // linger time, in seconds
} AEELinger;

// Deprecated options
#define AEE_SO_KEEPALIVE_30        1 // deprecated
#define AEE_SO_LINGER_30           2 // deprecated
#define AEE_SO_RCVBUF_30           3 // deprecated
#define AEE_SO_SNDBUF_30           4 // deprecated
#define AEE_TCP_NODELAY_30         1 // deprecated

// Socket protocol level options
#define AEE_SO_KEEPALIVE       0x101       // interval to test if connection is still alive (seconds)
#define AEE_SO_LINGER          0x102       // linger on close if data to send
#define AEE_SO_RCVBUF          0x103       // receive buffer size
#define AEE_SO_SNDBUF          0x104       // send buffer size
#define AEE_SO_REUSEADDR       0x105       // Enable to reuse a pair of address and port 
                                           // number that is already bound by another socket
#define AEE_SO_DISABLE_FLOW_FWDING   0x106 // disables routing data on best effort 
                                           // flow when QoS is not available
#define AEE_SO_SDB_ACK_CB      0x107       // invoke a callback upon receiving SDB data ack
#define AEE_SO_ERROR_ENABLE    0x01064bca  // Enable retrieving of ICMP errors using SO_ERROR
#define AEE_SO_ERROR           0x01064bcb  // Get ICMP error on the socket

// TCP protocol level options
#define AEE_TCP_NODELAY        0x201 // disable Nagle algorithm
#define AEE_TCP_DELAYED_ACK    0x202 // Enable delayed ack
#define AEE_TCP_SACK           0x203 // Enable SACK
#define AEE_TCP_TIME_STAMP     0x204 // Enable TCP time stamp option
#define AEE_TCP_FIONREAD       0x01062a0d // Retrieve TCP queue size
#define AEE_TCP_MAXSEG         0x205 // Change the TCP maximum segment size

// IP protocol level options
#define AEE_IP_ADD_MEMBERSHIP  0x301 // join a multicast group
#define AEE_IP_DROP_MEMBERSHIP 0x302 // leave a multicast group
#define AEE_IP_TOS             0x303 // Type of service socket option
#define AEE_IPV6_TCLASS        0x304 // Traffic class socket option
#define AEE_IP_TTL             0x305 // Change the Time To Live value
#define AEE_IP_RECVERR         0x306 // Enable retrieving of ICMP error info

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

// SockPort Types supported.
// These defines are for ISockPort only. (For ISocket, use NetSocket)
#define AEE_SOCKPORT_UNSPEC   0  // Unspecified socket type
#define AEE_SOCKPORT_STREAM   1  // TCP - streaming socket
#define AEE_SOCKPORT_DGRAM    2  // UDP - datagram socket

typedef int AEESockType;

typedef struct ISockPortDbgMark
{
   const char* cpszFile;
   int         nLine;
} ISockPortDbgMark;

//
// ISockPort_Sendto flag options
//
#define SOCKPORT_FLAG_SENDTO_URGENT   0x0002    /* send with urgency */
#define SOCKPORT_FLAG_SENDTO_WAKEUP   0x0008    /* wake PPP in conjunction with urgent send */

//
// ISockPort_RecvMsg arguments
//
typedef struct AEESockMsgHdr {
   void*          pName;
   uint16         wNameLen;
   SockIOBlock*   pIov;
   uint16         wIovLen;
   void*          pControl;
   uint16         wControlLen;
   uint32         uFlags;
} AEESockMsgHdr;

typedef struct AEESockCMsgHdr {
   uint32         uLen;
   int            nLevel;
   int            nType;
} AEESockCMsgHdr;

#define AEESOCKCMSG_DATA(cmsg) ((void*)((byte*)(cmsg) + \
                             AEESOCKCMSG_ALIGN(sizeof(AEESockCMsgHdr))))

#define AEESOCKCMSG_NXTHDR(msg, cmsg) AEESockCMsg_NxtHdr (msg, cmsg)

#define AEESOCKCMSG_FIRSTHDR(msg) \
  ((uint32) (msg)->wControlLen >= sizeof (AEESockCMsgHdr) \
   ? (AEESockCMsgHdr*) (msg)->pControl : NULL)

#define AEESOCKCMSG_ALIGN(len) ( ((len) + sizeof(uint32) - 1) \
                              & (uint32) ~(sizeof(uint32) - 1) )

#define AEESOCKCMSG_SPACE(len) ( AEESOCKCMSG_ALIGN(len) \
                              + AEESOCKCMSG_ALIGN(sizeof(AEESockCMsgHdr)) )

#define AEESOCKCMSG_LEN(len) ( AEESOCKCMSG_ALIGN (sizeof(AEESockCMsgHdr)) + (len) )

static __inline AEESockCMsgHdr* AEESockCMsg_NxtHdr(AEESockMsgHdr* msg, AEESockCMsgHdr* cmsg)
{
   if (cmsg->uLen < sizeof (AEESockCMsgHdr)) {
      return NULL;
   }
   
   cmsg = (AEESockCMsgHdr*) ((byte*) cmsg + AEESOCKCMSG_ALIGN(cmsg->uLen));
   if ((byte*) (cmsg + 1) > ((byte*) msg->pControl + msg->wControlLen)) {
      return NULL;
   }
   return cmsg;
}

// Socket extended error structure for ICMP error reporting and related constants.
#define AEE_SO_EE_ORIGIN_LOCAL 1  /* Locally originated error               */
#define AEE_SO_EE_ORIGIN_ICMP  2  /* ICMP error                             */ 
#define AEE_SO_EE_ORIGIN_ICMP6 3  /* ICMPv6 error                           */ 

typedef struct AEESockExtendedErr {
  int    nError;                         // Error code
  uint8  ucOrigin;                       // Error origin - AEE_SO_EE_ORIGIN_*
  uint8  ucType;                         // ICMP packet type - see RFCs 792, 2463
  uint8  ucCode;                         // ICMP packet code - see RFCs 792, 2463
  uint32 uInfo;                          // Discovered MTU for EMSGSIZE errors
  uint32 uData;                          // Not currently used
} AEESockExtendedErr;

typedef struct AEEICMPErrMsg {
   AEESockExtendedErr ee;
   AEESockAddrStorage sas;
} AEEICMPErrMsg;

//
// ISockPort_RecvMsg() flag options 
// 
#define SOCKPORT_FLAG_RECVMSG_ERRQUEUE    0x0001
#define SOCKPORT_FLAG_RECVMSG_MSGTRUNC    0x0002

//
// SDB-Ack socket option definitions
//

// SDB-Ack status definitions
#define AEE_SDB_ACK_NONE                     -1
#define AEE_SDB_ACK_OK                       0
#define AEE_SDB_ACK_HOLD_ORIG_RETRY_TIMEOUT  1
#define AEE_SDB_ACK_HOLD_ORIG                2
#define AEE_SDB_ACK_NO_SRV                   3
#define AEE_SDB_ACK_ABORT                    4
#define AEE_SDB_ACK_NOT_ALLOWED_IN_AMPS      5
#define AEE_SDB_ACK_NOT_ALLOWED_IN_HDR       6
#define AEE_SDB_ACK_L2_ACK_FAILURE           7
#define AEE_SDB_ACK_OUT_OF_RESOURCES         8
#define AEE_SDB_ACK_ACCESS_TOO_LARGE         9
#define AEE_SDB_ACK_DTC_TOO_LARGE            10
#define AEE_SDB_ACK_OTHER                    11
#define AEE_SDB_ACK_ACCT_BLOCK               12
#define AEE_SDB_ACK_L3_ACK_FAILURE           0x0107284b


typedef int32 AEESDBAckStatus;

// struct holding the information associated with an SDB-Ack.
typedef struct AEESDBAckInfo {
   boolean           bOverFlow;  // this field will be set to TRUE by 
   // lower layers, if the number of packets
   // for which mobile is still waiting for an ACK
   // is larger than the number of packets
   // the mobile can handle. 
   AEESDBAckStatus   nStatus;    // acknowledgement's status
} AEESDBAckInfo;

// ISockPort SDB Ack notification function
typedef void (*PFNSDBACKEVENT)(void* pSock, void* pUserData);

// struct used for AEE_SO_SDB_ACK_CB socket option
typedef struct AEESDBAckCBType
{
   PFNSDBACKEVENT pfn;
   void* pUserData;
} AEESDBAckCBType;

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

Socket Protocols

Description:
   Definitions of known socket protocol definitions.

Definition:
   #define AEE_IPPROTO_UNSPEC     0
   #define AEE_IPPROTO_TCP        2
   #define AEE_IPPROTO_IP         3
   #define AEE_IPPROTO_UDP        4

Members:
   AEE_IPPROTO_UNSPEC: Unspecified Protocol (system's default).
                       Used in IAddrInfo_StartQuery().
                       Introduced BREW Client 3.1.5
   AEE_IPPROTO_TCP: TCP protocol
   AEE_IPPROTO_IP:  IP protocol
   AEE_IPPROTO_UDP: UDP protocol

Comments:
   None.

Version:
   Introduced BREW Client 2.0

See Also:
   None

=======================================================================

AEELinger

Description:
    Used as the optval for ISockPort_GetOpt() and/or ISockPort_SetOpt()
when the optname is AEE_SO_LINGER

Definition:
    typedef struct {
    boolean bOn;
    uint32  dwLinger;
    } AEELinger;

Members:
    bOn :  linger on or off
    dwLinger : linger time, in seconds

Version:
   Introduced BREW Client 3.1

See Also:
    ISockPort_GetOpt()
    ISockPort_SetOpt()

=======================================================================

AEESockOpt

Description:
    Used to set/get socket options.

Definition:
    typedef struct {
    int nLevel;
    int nOptName;
    void* pOptVal;
    int* pnOptSize;
    } AEESockOpt;

Members:
    nLevel:    option level (see table below)
    nOptName:  option name (see table below)
    pOptVal:   pointer to the data type appropriate for the option (see table below)
    pnOptSize: pointer to size of pOptVal

Comments:

The socket options available are listed below.  Not all options are supported on all devices.

===pre>
----------------------------------------------------------------------------------------------------------------
Level            | Option                 | Data Type | Description                                             |
-----------------+------------------------+-----------+---------------------------------------------------------|
AEE_SOL_SOCKET   | AEE_SO_KEEPALIVE       | boolean   | periodically test if connection is still alive          |
                 | AEE_SO_LINGER          | AEELinger | linger on close if data to send                         |
                 | AEE_SO_RCVBUF          | int       | receive buffer size                                     |
                 | AEE_SO_SNDBUF          | int       | send buffer size                                        |
                 | AEE_SO_REUSEADDR       | boolean   | enable to reuse a pair of address and port number       |
                 |                        |           | that is already bound by another socket. Currently,     |
                 |                        |           | this socket option is supported only for multicast,     |
                 |                        |           | UDP sockets. In Brew this option is currently not       |
                 |                        |           | supported for Brew loopback sockets and for sockets     |
                 |                        |           | that bind to the AEE_INADDR_ANY address.                |
                 | AEE_SO_DISABLE_FLOW_   | boolean   | disables routing data on best effort flow when          |
                 | FWDING                 |           | QoS is not available                                    |
                 | AEE_SO_ERROR_ENABLE    | boolean   | enable retrieving of ICMP errors using SO_ERROR         |
                 | AEE_SO_ERROR           | int       | get ICMP error on the socket (cannot be set)            |
                 | AEE_SO_SDB_ACK_CB      |           | Only applicable to CDMA networks. Used to retrieve or   |
                 |                        |           | register a callback that is invoked when the mobile     |
                 |                        |           | receives an SDB/DOS data ACK from the base station.     |
-----------------+------------------------+-----------+---------------------------------------------------------|
AEE_IPPROTO_TCP  | AEE_TCP_NODELAY        | boolean   | disable Nagle algorithm                                 |
                 | AEE_TCP_DELAYED_ACK    | boolean   | enable delayed ack                                      |
                 | AEE_TCP_SACK           | boolean   | enable SACK                                             |
                 | AEE_TCP_TIME_STAMP     | boolean   | enable TCP time stamp option                            |
                 | AEE_TCP_FIONREAD       | uint32    | Retrieve TCP queue size (cannot be set)                 |
                 | AEE_TCP_MAXSEG         | int       | Change the TCP maximum segment size                     |
-----------------+------------------------+-----------+---------------------------------------------------------|
AEE_IPPROTO_IP   | AEE_IP_ADD_MEMBERSHIP  | INAddr    | join a multicast group                                  |
                 | AEE_IP_DROP_MEMBERSHIP | INAddr    | leave a multicast group                                 |
                 | AEE_IP_TOS             | int       | determine Type of Service                               |
                 | AEE_IP_TTL             | int       | Change the Time To Live value                           |
                 | AEE_IP_RECVERR         | boolean   | enable retrieving of ICMP error info using              |
                 |                        |           | ISockPort_RecvMsg()                                     |
-----------------+------------------------+-----------+---------------------------------------------------------|
AEE_IPPROTO_IPV6 | AEE_IPV6_TCLASS        | int       | determine Traffic Class                                 |
---------------------------------------------------------------------------------------------------------------
===/pre>

Version:
   Introduced BREW Client 3.1
   
   AEE_IP_TOS introduced BREW Client 3.1.5 SP01~
   AEE_IPV6_TCLASS introduced BREW Client 3.1.5 SP01~
   AEE_IP_TTL introduced BREW Client 3.1.5 SP01~
   AEE_TCP_MAXSEG introduced BREW Client 3.1.5 SP01~
   AEE_SO_REUSEADDR introduced BREW Client 3.1.5 SP01~
   AEE_SO_DISABLE_FLOW_FWDING introduced BREW Client 3.1.5 SP01~
   AEE_SO_ERROR_ENABLE introduced BREW Client 4.0.2~
   AEE_SO_ERROR introduced BREW Client 4.0.2~
   AEE_IP_RECVERR introduced BREW Client 4.0.2~

See Also:
   AEELinger
   INAddr
   ISockPort_GetOpt()
   ISockPort_SetOpt()
   Socket Protocols

=======================================================================

SockIOBlock

Description:
    A single structure describes an individual block of memory from which data is
read or to which data is written.
    Arrays of SockIOBlock structures are used in calls to ISockPort_ReadV() and ISockPort_WriteV()
to describe data that is sent/received as a continuous stream even when, in memory,
it is scattered among several blocks.

Definition:
    typedef struct {
       byte * pbBuffer;
       uint16 wLen;
    } SockIOBlock;

Members:
    pbBuffer  :  Data buffer.
    wLen  :  Length of buffer.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    ISockPort_ReadV()
    ISockPort_WriteV()

=======================================================================

AEESockType

Description:
   This data type is an enumeration of the supported types of sockets.
   This data type is used to indicate the desired ISockPort type in
   ISockPort_OpenEx().

Definition:
   enum {
      AEE_SOCKPORT_UNSPEC,
      AEE_SOCKPORT_STREAM,
      AEE_SOCKPORT_DGRAM
   };

   typedef int AEESockType;

Members:
   AEE_SOCKPORT_UNSPEC: Unspecified socket type.
   AEE_SOCKPORT_STREAM: TCP: streaming socket.
   AEE_SOCKPORT_DGRAM : UDP: datagram socket.

Comments:
   This data type should be used for ISockPort only. (For ISocket, use NetSocket).

Version:
   Introduced BREW Client 3.1.3

See Also:
   ISockPort_OpenEx()

=======================================================================

ISockPortDbgMark

Description:
    Holder for file and line number when marking an ISockPort with debug information.

Definition:
    typedef struct ISockPortDbgMark {
    const char *cpszFile;
    int nLine;
    } ISockPortDbgMark

Members:
    cpszFile :  Set to __FILE__, but could be any static char
    nLine :  Line number

Comments:
    None

Version:
   Introduced BREW Client 3.1.3

See Also:
    ISockPort_IOCtl()

=======================================================================

AEESockMsgHdr

Description:
   This data type is used to pack the arguments to ISockPort_RecvMsg().

Definition:
   typedef struct AEESockMsgHdr {
      void*          pName;
      uint16         wNameLen;
      SockIOBlock*   pIov;
      uint16         wIovLen;
      void*          pControl;
      uint16         wControlLen;
      uint32         uFlags;
   } AEESockMsgHdr;

Members:
   See ISockPort_RecvMsg() for details.

Comments:
   Specified by Posix.1g.

Version:
   Introduced BREW Client 4.0.2

See Also:
   ISockPort_RecvMsg()

=======================================================================

AEESockCMsgHdr

Description:
   This data type is used for ancillary data returned by ISockPort_RecvMsg().

Definition:
   typedef struct AEESockCMsgHdr {
      uint32         uLen;
      int            nLevel;
      int            nType;
   } AEESockCMsgHdr;

Members:
   uLen  : data byte count, including header.
   nLevel: originating protocol.
   nType : protocol-specific type.

Comments:
   The data section of the ancillary data must be accessed using the following
   macros:

   AEESOCKCMSG_FIRSTHDR(AEESockMsgHdr msg) returns a pointer to the first AEESockCMsgHdr in the
   ancillary data buffer associated with the passed msg.

   AEESOCKCMSG_NXTHDR(AEESockMsgHdr msg, AEESockCMsgHdr cmsg) returns the next valid AEESockCMsgHdr
   after the passed cmsg. It returns NULL when there isn't enough space left in
   the buffer.

   AEESOCKCMSG_ALIGN(int len), given a length, returns it including the required alignment.
   This is a constant expression.

   AEESOCKCMSG_SPACE(int len) returns the number of bytes an ancillary element with payload
   of the passed data length occupies. This is a constant expression.

   AEESOCKCMSG_DATA(AEESockCMsgHdr cmsg) returns a pointer to the data portion of a cmsg.

   AEESOCKCMSG_LEN(int len) returns the value to store in the uLen member of the
   AEESockCMsgHdr structure, taking into account any necessary alignment. It takes the
   data length as an argument. This is a constant expression. 

   Specified by Posix.1g.

Version:
   Introduced BREW Client 4.0.2

See Also:
   ISockPort_RecvMsg(), AEESockMsgHdr

=======================================================================

AEESockExtendedErr

Description:
   Socket extended error structure for ICMP error reporting.

Definition:
   typedef struct AEESockExtendedErr {
      int    nError;
      uint8  ucOrigin;
      uint8  ucType;
      uint8  ucCode;
      uint32 uInfo;
      uint32 uData;
   } AEESockExtendedErr;

Members:
   nError   : Error code (AEE_NET_*)
   ucOrigin : Error origin:
                AEE_SO_EE_ORIGIN_LOCAL: Locally originated error
                AEE_SO_EE_ORIGIN_ICMP : ICMP error
                AEE_SO_EE_ORIGIN_ICMP6: ICMPv6 error
   ucType   : ICMP packet type field
   ucCode   : ICMP packet code field
   uInfo    : Discovered MTU for EMSGSIZE errors
   uData    : Not currently used

Comments:
   None.

Version:
   Introduced BREW Client 4.0.2

See Also:
   ISockPort_RecvMsg()

=======================================================================

AEEICMPErrMsg

Description:
   ICMP error message structure, which includes the extended error
   information and the the address of the network object where the
   error originated.

Definition:
   typedef struct AEEICMPErrMsg {
      AEESockExtendedErr ee;
      AEESockAddrStorage sas;
   } AEEICMPErrMsg;

Members:
   ee  : Extended error information
   sas : the address of the network object where the error originated.

Comments:
   None.

Version:
   Introduced BREW Client 4.0.2

See Also:
   ISockPort_RecvMsg(), AEESockExtendedErr, AEESockAddrStorage

=======================================================================

PFNSDBACKEVENT

Description:
   This callback type defines the SDB Ack Callback function
   that is called upon receiving SDB Ack.

Definition:
   typedef void (*PFNSDBACKEVENT)(void* pSock, void* pUserData);

Arguments:
   pSock      : Pointer to the object that registered the callback.
   pUserData  : Pointer to the data that the user provided at the
   registration of the callback.

Comments:
   The pSock argument will point to the object that registered the 
   callback. For example, if a sockport object have registered the
   callback, then, when the callback is called, pSock will point 
   to the same sockport object that registered the callback.

Version:
   Introduced BREW Client 3.1.5 SP01.

See Also:   
   ISockPort_SetOpt() 

=======================================================================
AEESDBAckStatus

Description:
   This data type holds the possible status reported in an SDB-Ack message.

Definition:
   typedef int32 AEESDBAckStatus;

Members:
   AEE_SDB_ACK_NONE:                      Status is not available.
   AEE_SDB_ACK_OK:                        Packet is sent successfully.
   AEE_SDB_ACK_HOLD_ORIG_RETRY_TIMEOUT:   Hold orig timer expired and hence failed to send the packet.
   AEE_SDB_ACK_HOLD_ORIG:                 Unable to process the packet because hold orig is true.
   AEE_SDB_ACK_NO_SRV:                    Failed to send the packet due to lack of service.
   AEE_SDB_ACK_ABORT:                     Aborted the operation.
   AEE_SDB_ACK_NOT_ALLOWED_IN_AMPS:       SDB/DS is not supported in analog mode.
   AEE_SDB_ACK_NOT_ALLOWED_IN_HDR:        SDB/DS is not supported when in a HDR call.
   AEE_SDB_ACK_L2_ACK_FAILURE:            Failed to receive Layer 2 ACK.
   AEE_SDB_ACK_OUT_OF_RESOURCES:          Unable to process the packet because of lack of resources.
   AEE_SDB_ACK_ACCESS_TOO_LARGE:          Packet is too big to be sent over access channel.
   AEE_SDB_ACK_DTC_TOO_LARGE:             Packet is too big to b sent over DTC.
   AEE_SDB_ACK_OTHER:                     Failed for some other reason.
   AEE_SDB_ACK_ACCT_BLOCK:                Access channel is blocked for traffic based on service option.
   AEE_SDB_ACK_L3_ACK_FAILURE             Failed to receive Layer 3 ACK.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01.

See Also:   
   AEESDBAckInfo 

=======================================================================

AEESDBAckInfo

Description:
   This data type holds the information associated with
   an SDB Acknowledgment.

Definition:
   typedef struct AEESDBAckInfo {
      boolean           bOverFlow; 
      AEESDBAckStatus   nStatus;
   } AEESDBAckInfo;

Members:
   bOverFlow: This field will be set to TRUE by lower layers,
   if the number of packets for which mobile is 
   still waiting for an ACK is larger than the number
   of packets the mobile can handle. 
   nStatus: Status of the acknowledgment.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01.

See Also:   
   AEESDBAckStatus

=======================================================================

*/

#endif  // AEESOCKETTYPES_H

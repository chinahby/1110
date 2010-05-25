#ifndef AEENET_H
#define AEENET_H
/*===========================================================================

FILE: AEENet.h

SERVICES:

   AEE NetMgr and Socket Interface services for application developers
   of mobile devices


DESCRIPTION:

   This file provides definitions for the NetMgr and socket interfaces
   made available by the AEE to application developers. This is a
   standard header file that must be included by all applications
   using the NetMgr or Socket services of the AEE


PUBLIC CLASSES:

   Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:

   Not Applicable


   Copyright (c) 1999-2002,2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEShell.h"
#include "AEEError.h"   // Error Code Definitions
#include "AEEStdLib.h"  // FREE
#include "AEENetworkTypes.h"
#include "../../inc/AEESocketTypes.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
typedef struct _INetMgr       INetMgr;
typedef struct _ISocket       ISocket;

// Net State Enum
enum {
   NET_INVALID_STATE,
   NET_PPP_OPENING,
   NET_PPP_OPEN,
   NET_PPP_CLOSING,
   NET_PPP_CLOSED,
   NET_PPP_SLEEPING,
   NET_PPP_ASLEEP,
   NET_PPP_WAKING
};
#ifdef _WIN32
typedef unsigned NetState;
#else
typedef int8 NetState;
#endif

// Socket Types supported.
// This enum is for ISocket only. For ISockPort, use AEESockType
enum {
   AEE_SOCK_STREAM=0,   // TCP - streaming socket
   AEE_SOCK_DGRAM       // UDP - datagram socket
};
#ifdef _WIN32
typedef unsigned NetSocket;
#else
typedef int8 NetSocket;
#endif

//
// Network Notifications
//

#define NMASK_OPENED      0x0001       // Network layer is available
#define NMASK_CLOSED      0x0002       // Network layer is closed
#define NMASK_IDLE        0x0004       // Network layer available and idle
#define NMASK_ASLEEP      0x0008       // Network layer dormant


// UDP listen. Unsupported, deprecated as of BREW 2.0

#define  NMASK_UDP_LISTEN  0x8000      // UNSUPPORTED!

#define GET_NMASK_PORT(dwMask)      (((dwMask) & 0xffff0000L) >> 16)
#define SET_UDP_LISTEN_MASK(wPort)  (((uint32)wPort << 16) | NMASK_UDP_LISTEN)

//
// Deprecated endian macros, use StdLib versions instead
//
#ifdef FEATURE_BIG_ENDIAN

#define AEE_htonl(x) ((uint32)(x))
#define AEE_htons(x) ((uint16)(x))

#else /* #ifdef __BIG_ENDIAN */

#define AEE_htonl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                  (((uint32)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32)(x) & 0xff000000U) >> 24)))

#define AEE_htons(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
                  (((uint16)(x) & 0xff00) >> 8)))

#endif /* #ifdef __BIG_ENDIAN */

#define AEE_ntohs(x) AEE_htons((x))
#define AEE_ntohl(x) AEE_htonl((x))

//
// IOCtl option IDs for ISOCKET are in the range 0x1000 .. 0x1FFF
//

typedef void (*PFNSOCKETCLOSE)(void *pvCxt, ISocket *pSocket);

typedef struct ISocketCloser
{
   PFNSOCKETCLOSE pfn;
   void *         pv;
} ISocketCloser;

typedef struct ISocketQI
{
   AEECLSID  clsId;
   void     *pif;
} ISocketQI;

typedef struct ISocketDbgMark
{
   const char *cpszFile;
   int         nLine;
} ISocketDbgMark;

#define ISOCKET_IOCTL_GCLOSER        0x1001 // dwVal = pointer to ISocketCloser
#define ISOCKET_IOCTL_SCLOSER        0x1002 // dwVal = pointer to ISocketCloser
#define ISOCKET_IOCTL_REALIZE        0x1003 // dwVal = unused
#define ISOCKET_IOCTL_SCONNTIMEOUT   0x1004 // dwVal = connect timeout in milliseconds
#define ISOCKET_IOCTL_SPRIORITY      0x1005 // unsupported
#define ISOCKET_IOCTL_QI             0x1006 // dwVal = pointer to ISocketQI
#define ISOCKET_IOCTL_GDBGMARK       0x1007 // dwVal = pointer to ISocketDbgMark
#define ISOCKET_IOCTL_SDBGMARK       0x1008 // dwVal = pointer to ISocketDbgMark
#define ISOCKET_IOCTL_GCONNTIMEOUT   0x1009 // dwVal = pointer to dword
#define ISOCKET_IOCTL_GPRIORITY      0x100A // unsupported
#define ISOCKET_IOCTL_LISTEN         0x100B // dwVal = int nMaxBacklog
#define ISOCKET_IOCTL_ACCEPT         0x100C // dwVal = pointer to pointer to ISocket
#define ISOCKET_IOCTL_SHUTDOWN       0x100D // dwVal = int nHow (AEE_SHUTDOWN_*)
#define ISOCKET_IOCTL_GETSOCKNAME    0x100E // dwVal = pointer to INSockAddr
#define ISOCKET_IOCTL_CLOSE          0x100F // dwVal = unused
#define ISOCKET_IOCTL_NATDESC        0x1010 // dwVal = pointer to dword
#define ISOCKET_IOCTL_SETSOCKOPT     0x1011 // dwVal = pointer to AEESetSockOpt
#define ISOCKET_IOCTL_GETSOCKOPT     0x1012 // dwVal = pointer to AEEGetSockOpt
// New IOCTLs must use unique IDs to avoid conflicts between several BREW versions under development
#define ISOCKET_IOCTL_GETSDBACKINFO  0x0105f09c // dwVal = pointer to GetSDBAckInfo

#define ISOCKET_IOCTL_CONNTIMEOUT    ISOCKET_IOCTL_SCONNTIMEOUT // deprecated
#define ISOCKET_IOCTL_PRIORITY       ISOCKET_IOCTL_SPRIORITY // deprecated

//
// Network Status Callback Information
//

// IMPORTANT NOTES:
// 1. New event definitions should be added to this enum
//    only for events supported by INetMgr.
// 2. If such new events are added they must be defined using
//    generated "class IDs". This is to avoid conflicting
//    definitions in different development branches (e.g. 3.x and 4.x).
enum {
   NE_NONE,
   NE_PPP,           // uint32 = NetState
   NE_SO_OPENING,    // uint32 = ISocket *
   NE_SO_CONNECTING, // uint32 = ISocket *
   NE_SO_CONNECTED,  // uint32 = ISocket *
   NE_SO_CLOSING,    // uint32 = ISocket *
   NE_SO_CLOSED,     // uint32 = ISocket *
   NE_IP,            // uint32 = new ip address

   NE_QOS_AWARE_UNAWARE       // THIS EVENT IS *NOT* SUPPORTED AND SHOULD *NOT* BE EXPECTED.
                              // Qos Aware/Unaware events are supported only via INetwork.
                              // See NETWORK_EVENT_QOS_AWARE_UNAWARE.

   // IMPORTANT: Use generated "class IDs" for any new event added here
};
#ifdef _WIN32
typedef unsigned NetMgrEvent;
#else
typedef int8 NetMgrEvent;
#endif

typedef void (*PFNNETMGREVENT)(void * pUser, NetMgrEvent evt, uint32 dwData);

//
// ISOCKET_Sendto flag options
//
#define ISOCKET_FLAG_SENDTO_URGENT       SOCKPORT_FLAG_SENDTO_URGENT    /* send with urgency */
#define ISOCKET_FLAG_SENDTO_WAKEUP       SOCKPORT_FLAG_SENDTO_WAKEUP    /* wake PPP in conjunction with urgent send */

// Applications must list NET_URGENT as an external dependency in
// their MIF in order to utilize INetMgr_GetOpt() with
// INET_OPT_UDP_URGENT and/or ISOCKET_SendTo() with
// ISOCKET_FLAG_SENDTO_URGENT
#include "NET_URGENT.BID"

//
// for INetMgr_GetOpt(INET_OPT_UMTS_INFO(pdp))
//
typedef struct AEEUMTSInfo
{
   boolean bValid;
   AECHAR  szName[UMTS_INFO_NAME_SIZE+1];
} AEEUMTSInfo;

// INetMgr Get/Set options
#define INET_OPT_DEF_RLP3         (0)            /* default RLP3 settings */
#define INET_OPT_CUR_RLP3         (1)            /* current RLP3 settings */
#define INET_OPT_NEG_RLP3         (2)            /* negotiated RLP3 settings */
#define INET_OPT_DNS_SERVERS      (3)            /* system DNS servers */
#define INET_OPT_PPP_AUTH         (4)            /* system PPP auth credentials */
#define INET_OPT_UDP_URGENT       (5)            /* urgent sendto info */
#define INET_OPT_DDTM_PREF        (6)            /* Dedicated data transmission mode (HDR only) */
#define INET_OPT_SELECT_NET       (7)            /* multiple data networks */
#define INET_OPT_DEFAULT_NET      (8)            /* default network */
#define INET_OPT_UMTS_COUNT       (9)            /* UMTS: number of Packet Data Profiles */
#define INET_OPT_UMTS_INFO(pdp)   (10|(pdp)<<16) /* UMTS: Packet Data Profile info */
#define INET_OPT_DORMANCY_TIMEOUT (11)           /* IS-707 (cdma2000 1x) packet data idle time before dormancy */
#define INET_OPT_HW_ADDR          (13)           /* hardware address */
#define INET_OPT_LAST_NET_DOWN_REASON  (14)      /* the last network down reason */ 

//********************************************************************************************************************************
//
// INetMgr Interface
//
//********************************************************************************************************************************

QINTERFACE(INetMgr)
{
   INHERIT_INotifier(INetMgr);

   void       (*GetHostByName)(INetMgr * pNetMgr, AEEDNSResult *pres, const char * psz, AEECallback *pcb);
   int        (*GetLastError)(INetMgr * pINetMgr);
   ISocket *  (*OpenSocket)(INetMgr * pINetMgr,NetSocket sType);
   NetState   (*NetStatus)(INetMgr * pINetMgr,AEENetStats * ps);
   INAddr     (*GetMyIPAddr)(INetMgr * pINetMgr);
   uint16     (*SetLinger)(INetMgr * pINetMgr,uint16 wSecs);
   int        (*OnEvent)(INetMgr * po, PFNNETMGREVENT pfn, void *pUser, boolean bRegister);
   int        (*SetOpt)(INetMgr *po, int nOptName, void *pOptVal, int nOptSize);
   int        (*GetOpt)(INetMgr *po, int nOptName, void *pOptVal, int *pnOptSize);
};

// INetMgr methods
#define INETMGR_AddRef(p)                    GET_PVTBL(p, INetMgr)->AddRef(p)
#define INETMGR_Release(p)                   GET_PVTBL(p, INetMgr)->Release(p)
#define INETMGR_GetHostByName(p,psz,pfn,pd)  GET_PVTBL(p, INetMgr)->GetHostByName(p,psz,pfn,pd)
#define INETMGR_GetLastError(p)              GET_PVTBL(p, INetMgr)->GetLastError(p)
#define INETMGR_OpenSocket(p,t)              GET_PVTBL(p, INetMgr)->OpenSocket(p,t)
#define INETMGR_NetStatus(p,ps)              GET_PVTBL(p, INetMgr)->NetStatus(p,ps)
#define INETMGR_GetMyIPAddr(p)               GET_PVTBL(p, INetMgr)->GetMyIPAddr(p)
#define INETMGR_SetLinger(p,wSecs)           GET_PVTBL(p, INetMgr)->SetLinger(p,wSecs)
#define INETMGR_OnEvent(p,pfn,pu,b)          GET_PVTBL(p, INetMgr)->OnEvent(p,pfn,pu,b)
#define INETMGR_SetOpt(p,n,v,s)              GET_PVTBL(p, INetMgr)->SetOpt((p),(n),(v),(s))
#define INETMGR_GetOpt(p,n,v,s)              GET_PVTBL(p, INetMgr)->GetOpt((p),(n),(v),(s))

// pseudo-methods
static __inline int INETMGR_SelectNetwork(INetMgr *me, int nNetwork)
{
   return INETMGR_SetOpt(me,
                         INET_OPT_SELECT_NET,
                         &nNetwork,
                         sizeof(int));
}
static __inline int INETMGR_GetDefaultNetwork(INetMgr *me, int* pnNetwork)
{
   int nOptSize = sizeof(int);

   return INETMGR_GetOpt(me,
                         INET_OPT_DEFAULT_NET,
                         pnNetwork,
                         &nOptSize);
}
static __inline int INETMGR_GetUMTSCount(INetMgr *me, uint16* pwCount)
{
   int nOptSize = sizeof(uint16);

   return INETMGR_GetOpt(me,
                         INET_OPT_UMTS_COUNT,
                         pwCount,
                         &nOptSize);
}
static __inline int INETMGR_GetUMTSInfo(INetMgr *me, uint16 wProfile, AEEUMTSInfo* pInfo)
{
   int nOptSize = sizeof(AEEUMTSInfo);

   return INETMGR_GetOpt(me,
                         INET_OPT_UMTS_INFO(wProfile),
                         pInfo,
                         &nOptSize);
}

static __inline int INETMGR_SetDormancyTimeout(INetMgr *me, uint8 ucSecs)
{
   return INETMGR_SetOpt(me,
                         INET_OPT_DORMANCY_TIMEOUT,
                         &ucSecs,
                         sizeof(ucSecs));
}

static __inline int INETMGR_GetLastNetDownReason(INetMgr *me, AEENetDownReason *pnReason)
{
   int nSize = sizeof(AEENetDownReason);
   return INETMGR_GetOpt(me, INET_OPT_LAST_NET_DOWN_REASON, pnReason, &nSize);
}

//********************************************************************************************************************************
//
// ISocket Interface
//
//********************************************************************************************************************************

typedef void (*PFNCONNECTCB)(void * pUser,int nError);

QINTERFACE(ISocket)
{
   INHERIT_IAStream(ISocket);

   int         (*GetPeerName)(ISocket * pISocket, INAddr * pAddr, INPort * pwPort);
   int         (*GetLastError)(ISocket * pISocket);
   int         (*Connect)(ISocket * po, INAddr a, INPort wPort, PFNCONNECTCB pfn, void * pUser);
   int         (*Bind)(ISocket * pISocket, INAddr a, INPort wPort);
   int32       (*Write)(ISocket * pISocket,  byte * pBuffer, uint32 wBytes);
   int32       (*WriteV)(ISocket * pISocket, SockIOBlock iov[], uint16 wIovCount);
   int32       (*ReadV)(ISocket * pISocket, SockIOBlock iov[], uint16 nIovCount);
   int32       (*SendTo)(ISocket * pISocket,  byte * pBuff,uint16 dwBytes,uint16 wflags,INAddr a, uint16 wPort);
   int32       (*RecvFrom)(ISocket * pISocket,  byte *pbuffer, uint16 dwbytes, uint16 wflags,INAddr * pa, INPort * pwPort);
   void        (*Writeable)(ISocket * pISocket, PFNNOTIFY pfn, void * pUser);
   int         (*IOCtl)(ISocket * pISocket, int nOption, uint32 dwVal);
};

// Standard IAStream Methods

#define ISOCKET_AddRef(p)                       GET_PVTBL(p, ISocket)->AddRef(p)
#define ISOCKET_Release(p)                      GET_PVTBL(p, ISocket)->Release(p)
#define ISOCKET_Readable(p, pfn, u)             GET_PVTBL(p, ISocket)->Readable(p, pfn, u)
#define ISOCKET_Read(p, d, n)                   GET_PVTBL(p, ISocket)->Read(p, d, n)
#define ISOCKET_Cancel(p,pfn,pUser)             GET_PVTBL(p, ISocket)->Cancel(p,pfn,pUser)

// ISocket Methods

#define ISOCKET_GetPeerName(p,pa,pp)            GET_PVTBL(p, ISocket)->GetPeerName(p,pa,pp)
#define ISOCKET_GetLastError(p)                 GET_PVTBL(p, ISocket)->GetLastError(p)
#define ISOCKET_Connect(p, a, port,pfn,pUser)   GET_PVTBL(p, ISocket)->Connect(p, a, port,pfn,pUser)
#define ISOCKET_Bind(p, a, port)                GET_PVTBL(p, ISocket)->Bind(p, a, port)
#define ISOCKET_Write(p, b, n)                  GET_PVTBL(p, ISocket)->Write(p, b, n)
#define ISOCKET_WriteV(p,i,c)                   GET_PVTBL(p, ISocket)->WriteV(p, i, c)
#define ISOCKET_ReadV(p, i, c)                  GET_PVTBL(p, ISocket)->ReadV(p, i, c)
#define ISOCKET_SendTo(p, b, n, f, a, port)     GET_PVTBL(p, ISocket)->SendTo(p, b, n, f, a, port)
#define ISOCKET_RecvFrom(p, b, n, f, a, port)   GET_PVTBL(p, ISocket)->RecvFrom(p, b, n, f, a, port)
#define ISOCKET_Writeable(p, pfn, u)            GET_PVTBL(p, ISocket)->Writeable(p, pfn, u)
#define ISOCKET_IOCtl(p,o,dw)                   GET_PVTBL(p, ISocket)->IOCtl(p,o,dw)

// pseudo-methods
static __inline int ISOCKET_Realize(ISocket *me)
{
   return ISOCKET_IOCtl(me, ISOCKET_IOCTL_REALIZE, 0);
}
#define ISOCKET_DbgMark(p)                      do {\
   ISocketDbgMark m;\
   m.cpszFile = __FILE__;\
   m.nLine    = __LINE__;\
   ISOCKET_IOCtl((p),ISOCKET_IOCTL_SDBGMARK,(uint32)&m);\
} while(0);
static __inline int ISOCKET_Listen(ISocket *me, int nBacklog)
{
   return ISOCKET_IOCtl(me, ISOCKET_IOCTL_LISTEN, (uint32)nBacklog);
}
static __inline int ISOCKET_Accept(ISocket *me, ISocket** pps)
{
   return ISOCKET_IOCtl(me, ISOCKET_IOCTL_ACCEPT, (uint32)pps);
}
static __inline int ISOCKET_Shutdown(ISocket *me, int nHow)
{
   return ISOCKET_IOCtl(me, ISOCKET_IOCTL_SHUTDOWN, (uint32)nHow);
}
static __inline int ISOCKET_GetSockName(ISocket *me, INAddr *pAddr, INPort *pPort)
{
   INSockAddr sa;
   int ret;

   ret = ISOCKET_IOCtl(me, ISOCKET_IOCTL_GETSOCKNAME, (uint32)&sa);
   if ((INAddr*)0 != pAddr) {
      *pAddr = sa.addr;
   }
   if ((INPort*)0 != pPort) {
      *pPort = sa.port;
   }
   return ret;
}
static __inline int ISOCKET_SetOpt(ISocket *me, int nLevel, int nOptName, const void *pOptVal, int nOptSize)
{
   AEESockOpt sso;

   sso.nLevel = nLevel;
   sso.nOptName = nOptName;
   sso.pOptVal = (void*)pOptVal;
   sso.pnOptSize = &nOptSize;

   return ISOCKET_IOCtl(me, ISOCKET_IOCTL_SETSOCKOPT, (uint32)&sso);
}
static __inline int ISOCKET_GetOpt(ISocket *me, int nLevel, int nOptName, void *pOptVal, int *pnOptSize)
{
   AEESockOpt sso;

   sso.nLevel = nLevel;
   sso.nOptName = nOptName;
   sso.pOptVal = pOptVal;
   sso.pnOptSize = pnOptSize;

   return ISOCKET_IOCtl(me, ISOCKET_IOCTL_GETSOCKOPT, (uint32)&sso);
}
static __inline int ISOCKET_GetSDBAckInfo(ISocket *me, AEESDBAckInfo* pSDBAckInfo)
{
   return ISOCKET_IOCtl(me, ISOCKET_IOCTL_GETSDBACKINFO, (uint32)pSDBAckInfo);
}
static __inline int ISOCKET_Close(ISocket *me)
{
   return ISOCKET_IOCtl(me, ISOCKET_IOCTL_CLOSE, 0);
}


/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

INetMgr Interface

The INetMgr Interface functions provide mechanisms to get and set the parameters
associated with the network subsystem of the device. It also provides means to create
multiple ISocket interfaces that use TCP or UDP to transmit and receive data over
the network.

NOTE: Your application must have a privilege level of Network or All to invoke the
functions in this interface.

INETMGR_OpenSocket() opens a TCP or UDP socket. If successful, this function returns
a pointer to an instance of the ISocket Interface. When required the BREW AEE brings
up the network subsystem. The subsystem components include:

 -    CDMA physical layer,

 -    Radio Link Protocol (RLP) for CDMA devices

 -    Point-to-Point Protocol (PPP) connections.

These connections are shared by all of the device's sockets. You close a socket
by calling ISOCKET_Release(). When the last socket on the device is closed, the
BREW AEE terminates the network subsystem connections after a specified linger time.

INETMGR_GetHostByName() takes as input the name of a host (for example, brew.qualcomm.com)
and returns a list of IP addresses that can be used to access that host. Because
retrieving this list requires network communication, INETMGR_GetHostByName() is
asynchronous. You specify the AEECallback input parameter, and the specified function
is called when the IP address list is available.

INETMGR_GetMyIPAddr() returns the device's own IP address.

INETMGR_NetStatus() returns the current status (opening,
open, closing, or closed) of the device's PPP connection and some statistics on
the current performance of the network subsystem.

INETMGR_SetLinger() sets the linger time of the PPP connection.
This is the amount of time that the BREW AEE waits to
terminate the PPP connection after the device's last connected socket is closed
(the default value is 30 seconds).

INETMGR_GetLastError() returns information about the last error detected in executing
INETMGR_OpenSocket(). When INETMGR_OpenSocket() returns something other
than success, INETMGR_GetLastError() provides more detailed information about why
the function failed to perform its task.

===H2>
To use the functions in the INetMgr Interface
===/H2>

1.   Call ISHELL_CreateInstance() to create an instance of the INetMgr Interface.
2.   Use the following functions in the INetMgr Interface as needed:~
~

–   To obtain remote or local IP address information, call INETMGR_GetHostByName()
or INETMGR_GetMyIPAddr().

–   To obtain PPP connection status, call INETMGR_NetStatus().

–   To set the PPP linger time, call INETMGR_SetLinger().
*
===H2>
To set up a TCP or UDP socket and transfer data over it
===/H2>

1.   Call INETMGR_OpenSocket() to open the socket.

2.   For TCP using the pointer returned by INETMGR_OpenSocket(), call ISOCKET_Connect()
to connect the socket to the specified destination address.

3.   Use functions in the ISocket to read and write data on the connection.

4.   Call ISOCKET_Release() to close the connection when you have completed the
necessary exchange of data.

5.   Call INETMGR_Release() when you no longer need the INetMgr Interface instance.


=============================================================================

INETMGR_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

Version:
   Introduced BREW Client 1.0

See Also:
    INETMGR_Release()

=============================================================================

INETMGR_GetDefaultNetwork()

Description:
   This function returns the default data network.

Prototype:
    int INETMGR_GetDefaultNetwork
    (
      INetMgr *pINetMgr,
      int* pnNetwork,
    );

Parameters:
    pINetMgr  :  Pointer to the INetMgr Interface of interest
   pnNetwork :  Pointer to int; will be filled in with AEE_NETWORK_*

Return Value:
   SUCCESS: the default data network was filled in
   EFAILED: (and other errors from AEEError.h): otherwise

Comments:
   Most applications will not need to get the default network.

   The default network may be the CDMA Service Network, the configured default
   UMTS Packet Data Profile or other network depending on the device and/or
   service provider.

   This is not a formal member of INetMgr; instead it is a wrapper
   around INETMGR_GetOpt().

Version:
   Introduced BREW Client 3.1.0

See Also:
   INETMGR_SelectNetwork()
   INETMGR_GetOpt()
   Data networks

=============================================================================

INETMGR_GetHostByName()

Description:
    This function retrieves IP addresses associated with the specified host name.
Results are placed in the result structure and a callback notifies the caller of
completion. An AEECallback record is used to specify and cancel callbacks.

    The memory pres and pcb pointers must remain valid for the entire duration of
the operation (that is, until the completion callback is called, or until the operation
is canceled). Calling INETMGR_Release() does not automatically cancel the callback.
Therefore, if it is to be called before this operation completes, the specified
callback must be canceled first. The result structure does not need to be initialized
before the operation; INETMGR_GetHostByName() assigns its values.

    The AEECallback must be properly initialized. The text string at psz can be
discarded after the call to INETMGR_GetHostByName().

    The call to this function always succeeds in that it guarantees the callback
is called. Any errors related to handling the request are delivered to the callback,
so all error checking can be done there.

Prototype:
    void INETMGR_GetHostByName
    (
    INetMgr * pINetMgr,
    AEEDNSResult * pres,
    const char * psz,
    AEECallback * pcb
    )

Parameters:
    pINetMgr :  Pointer to the INetMgr Interface object.
    pres :  Pointer to the result structure.
    psz :  Domain name to be resolved, terminated by a colon (:),slash (/), or NULL
(\0) character.
    pcb  :  Pointer to the callback.

Return Value:

    On completion:
     -    pres->addrs[] contains the result IP addresses.
     -    pres->nResult contains either the number of addresses obtained (1...AEEDNSMAXADDRS),

         or an error code if the host has no addresses 

         or if it encountered an error while requesting the information.



The following error code definitions are specific to
INETMGR_GetHostByName():

    AEE_NET_BADDOMAIN
     Host name is malformed; not a valid host name.

    AEE_NET_UNKDOMAIN
     Unknown host, or one without IP addresses.

    AEE_NET_ETIMEDOUT
     No response was seen within the maximum time limit.

    EUNSUPPORTED
     DNS servers do not need to be configured on the device for it to work properly,
     otherwise EUNSUPPORTED is returned.

    AEE_NET_GENERAL_FAILURE
     When there is an allocation failure.

    AEE_NET_EOPNOTSUPP
     DNS Look up Not Supported.

    Other error codes (such as Network error codes related to network connection failure,
    and so forth) are also possible. See ISOCKET_SendTo() for some more possible error codes.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    AEEDNSResult
    AEECallback

=============================================================================

INETMGR_GetLastError()

Description:
    This function returns the last error that occurred as part of executing 
INETMGR_OpenSocket().

Prototype:
    int INETMGR_GetLastError(INetMgr * pINetMgr)

Parameters:
    pINetMgr  :  Pointer to the INetMgr Interface object to be used to retrieve
the last error.

Return Value:
    The most recently occurred error.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

INETMGR_GetMyIPAddr()

Description:
   This function returns the IP address of the local host or device, ~
    in NETWORK byte order.

Prototype:
        INAddr INETMGR_GetMyIPAddr(INetMgr * pINetMgr)

Parameters:
    pINetMgr  :  Pointer to the INetMgr Interface object to be used to retrieve
the IP address.

Return Value:
    IP address of the device in NETWORK byte order

Comments:
    In the BREW Simulator, this function returns the result of GetHostByName(GetHostName())
unless the key "IPAddress" is set in the [Settings] section of BREW_Emu.dat

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

INETMGR_GetOpt()

Description:
    This function provides the ability to query certain network options. It is similar
in paradigm to ISOCKET_IOCtl(), but deals more with network-level configurables,
rather than socket-level configurables.

Prototype:
    int INETMGR_GetOpt
    (
    INetMgr *pINetMgr,
    int nOptName,
    void *pOptVal,
    int *pnOptSize
    );

Parameters:
    pINetMgr :  [in] :  Pointer to the INetMgr Interface to be used to retrieve
network options
    nOptName :  [in] :  integer identifying which option to get
    pOptVal :  [out]  :  Filled with option-specific data
    pnOptSize :  [in/out]  :  if pOptVal is non-NULL, on input specifies size of
pOptVal, which must match the required size of the option data.
    if pOptVal is NULL, on input is ignored, on output holds the required size of
the option data.

Return Value:
    SUCCESS: if the option was retrieved
    EPRIVLEVEL: if access to the option is prohibited by applet privilege
    EUNSUPPORTED: if nOptName is unknown
    EBADPARM: if pOptVal is non-null and pnOptSize is set to something smaller than
necessary to hold the fixed-length option data

    other error, depending on the option

Comments:
  Currently supported option ids are:

    -    INET_OPT_DEF_RLP3: default RLP settings, pOptVal must be an AEERLP3Cfg
    -    INET_OPT_CUR_RLP3: current RLP settings, pOptVal must be an AEERLP3Cfg
    -    INET_OPT_NEG_RLP3: negotiated RLP settings, pOptVal must be an AEERLP3Cfg

   RLP options: these options are highly device dependent, and as such may not be able
to be configured, available, or well-behaved. They are protected by the system privilege
level.  Note that the Negotiated RLP settings are read-only, and that the Negotiated
number of forward/reverse NAK rounds may not be exceeded by the Default nor Current RLP
settings.  Note also that getting the Current and Negotiated RLP settings only make sense
during a data call.

     -    INET_OPT_DNS_SERVERS: retrieve the system's configured DNS servers, pOptVal
must be an array of INAddr. pnOptSize is set to the number of bytes filled in pOptVal.
If pnOptSize passed in is less than 4 (size of 1 INAddr), pnOptSize is set to 0
and SUCCESS is returned.

     -    INET_OPT_PPP_AUTH: retrieve the system's configured PPP authentication
credentials (protected by PL_SYSTEM). pOptVal's type is char *, and the format of
the returned string is: "userid\0passwd\0". If the passed-in buffer isn't large
enough, the data is truncated, and not null-terminated. pnOptSize is changed only
when pOptVal is NULL.

     -    INET_OPT_UDP_URGENT: obtain info about out-of-band UDP sendto flag. pOptVal
must be a pointer to AEEUDPUrgent struct. bUrgentSupported indicates whether urgent
sendto may succeed. bUrgentLimit indicates largest payload which might succeed.
Note that applications must list NET_URGENT as an external dependency in their MIF
in order to utilize this option.

     -    INET_OPT_DEFAULT_NET: obtain the default network (AEE_NETWORK_*);
pOptVal must be a pointer to an int

     -    INET_OPT_UMTS_COUNT: obtain the number of Packet Data Profiles (UMTS only);
pOptVal must be a pointer to an uint16

     -    INET_OPT_UMTS_INFO(pdp): obtain the profile information for a given
Packet Data Profile number (UMTS only); pOptVal must be a pointer to an AEEUMTSInfo

    -     INET_OPT_HW_ADDR: obtain the hardware address, pOptVal is a
pointer to an array of bytes.  The availability and format of
a hardware address is very device specific.  For example, on
a WLAN network, the hardware address is a six-byte MAC
address.  On a CDMA network, there is no hardware address.

     -    INET_OPT_LAST_NET_DOWN_REASON: obtain the last network down reason.
Typically called by the application when the data network is CLOSED.
pOptVal must be a pointer to a AEENetDownReason.
Introduced BREW Client 3.1.4 SP1

Side Effects:
    Depend on the option

Version:
   Introduced BREW Client 2.0

See Also:
   INETMGR_SetOpt()
   AEEUDPUrgent

=============================================================================

INETMGR_GetUMTSCount()

Description:
Returns the number of Packet Data Profiles (UMTS only)

Prototype:
    int INETMGR_GetUMTSCount
    (
      INetMgr *pINetMgr,
      uint16* pwCount,
    );

Parameters:
    pINetMgr :  Pointer to the INetMgr Interface of interest
    pwCount  :  filled in with the PDP count

Return Value:
    SUCCESS: if the count was filled in
    AEE_NET_EINVAL: if the network is not valid
    EFAILED: or other error code, otherwise

Comments:
    In an UMTS device/network, there may be several networks
    configured and/or available.  The count of available networks may
    be determined at runtime using this function.  This is useful for
    enumerating the networks via INETMGR_GetUMTSInfo().

    Note that the available profiles are numbered consecutively from
    one to this count, although they may not all be valid.

    This is not a formal member of INetMgr; instead it is a wrapper
    around INETMGR_GetOpt().

Version:
   Introduced BREW Client 3.1.0

See Also:
INETMGR_GetUMTSInfo()
INETMGR_SelectNetwork()
INETMGR_GetOpt()

=============================================================================

INETMGR_GetUMTSInfo()

Description:
Returns the profile information for a given Packet Data Profile number (UMTS only)

Prototype:
    int INETMGR_GetUMTSInfo
    (
      INetMgr*     pINetMgr,
      uint16       wProfile,
      AEEUMTSInfo* pInfo
    );

Parameters:
    pINetMgr : pointer to the INetMgr Interface of interest
    wProfile : the profile number of interest
    pInfo    : will be filled in with the profile information

Return Value:
    SUCCESS: if the information was filled in
    AEE_NET_EINVAL: if the PDP number is not valid
    EFAILED: or other error code, otherwise

Comments:
    In an UMTS device/network, there may be several networks
    configured and/or available.  The count of available networks may
    be determined at runtime using INETMGR_GetUMTSCount().  This is
    useful for enumerating the networks via this function.

    Note that the available profiles are numbered consecutively from
    one to the count, although they may not all be valid.

    This is not a formal member of INetMgr; instead it is a wrapper
    around INETMGR_GetOpt().

Version:
   Introduced BREW Client 3.1.0

See Also:
INETMGR_GetUMTSCount()
INETMGR_SelectNetwork()
INETMGR_GetOpt()
AEEUMTSInfo

=============================================================================

INETMGR_NetStatus()

Description:
    This function returns the current network status as a NetState enum of the type
NetState. If the AEENetStats pointer is valid, the buffer is filled with the current
network connection information such as data rate, active time, bytes sent, and other
items. The caller can view the performance of the current session and all sessions
since the last time the device was reset.

Prototype:
    NetState INETMGR_NetStatus
    (
    INetMgr * pINetMgr,
    AEENetStats * pNetStats
    )

Parameters:
    pINetMgr  :  Pointer to the INetMgr Interface object to be used to get the current
network status.
    pNetStats  :  Pointer to the block to be filled with statistical data.

Return Value:
    A NetState enum indicating the current network status.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
    AEENetStats
    NetState

=============================================================================

INETMGR_OnEvent()

Description:
    This function allows the caller to register to receive notification of in progress
INetMgr operations. These include both network and socket functions.

    This function also allows the caller to deregister from receiving the above 
notifications. When deregistering, if pfn is non-NULL, the callback associated 
with pfn and pUser is deregistered. If pfn is NULL, a callback associated 
with pUser is deregistered. 

    The user-specified callback should always be explicitly deregistered by 
the client; releasing the INetMgr object does not implicitly deregisters 
the callback.

Prototype:
    int INETMGR_OnEvent
    (
    INetMgr * pINetMgr,
    PFNNETMGREVENT pfn,
    void * pUser,
    boolean bRegister
    )

Parameters:
    pINetMgr :  Pointer to the INetMgr Interface.
    pfn :  User-specified callback to call when event occurs.
    pUser :  User-specified context data passed as first argument to callback.
    bRegister :  TRUE, if registering, FALSE if deregistering.

Return Value:
    SUCCESS: if registered
    ENOMEMORY: if allocation failure
    EALREADY: if a callback is already registered. If pfn is non-NULL, a 
              callback associated with pfn and pUser is already registered. If 
              pfn is NULL, a callback associated with pUser is already registered.

    Other error codes are also possible

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
    PFNNETMGREVENT

=============================================================================

INETMGR_OpenSocket()

Description:
    This function creates a socket and returns a pointer to the ISocket Interface.

Prototype:
    ISocket * INETMGR_OpenSocket(INetMgr * pINetMgr, NetSocket Type)

Parameters:
    pINetMgr  :  Pointer to the INetMgr Interface object to be used to create the
socket.
    Type  :  Specifies the socket type:
    AEE_SOCK_STREAM for TCP
    AEE_SOCK_DGRAM for UDP
    Corresponding to stream or datagram sockets respectively

Return Value:
    If successful, a valid pointer to the ISocket Interface

    If failed, NULL. In this case, specific error code can be retrieved by calling
INETMGR_GetLastError().

~
Error Codes:

    AEE_NET_ESOCKNOSUPPORT The specified socket type is not supported in this address
      family.

    AEE_NET_GENERAL_FAILURE  An ISocket could not be allocated

    Other error codes are also possible

*

Comments:
    This function does not generate AEE_NET_EMFILE errors. When the limit on sockets
is exceeded, a subsequent Connect(), Bind(), or SendTo() operation will fail with
an AEE_NET_EMFILE error. (This behavior is due to the way that ISOCKET invisibly
manages network connections ... the underlying OEM socket resources is not actually
created until after the network connection is established.)

Version:
   Introduced BREW Client 1.0

See Also:
    NetSocket

=============================================================================

INETMGR_Release()

Description:
    This function is inherited from IBASE_Release().

Version:
   Introduced BREW Client 1.0

See Also:
    INETMGR_AddRef()

=============================================================================

INETMGR_SetLinger()

Description:
    This function sets the linger time for the network connection specified by pINetMgr
to the value specified by wSecs, and returns the previous value. When the connection
is created, the default linger time is set to 30 seconds.

Prototype:
    uint16 INETMGR_SetLinger(INetMgr * pINetMgr, uint16 wSecs)

Parameters:
    pINetMgr  :  Pointer to the INetMgr Interface object.
    wSecs  :  Linger time in seconds.

Return Value:
    Previous linger time value.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

INETMGR_SelectNetwork()

Description:
    Selects a specific data network.

Prototype:
    int INETMGR_SelectNetwork
    (
      INetMgr *pINetMgr,
      int nNetwork,
    );

Parameters:
    pINetMgr :  Pointer to the INetMgr Interface of interest
    nNetwork :  AEE_NETWORK_*

Return Value:
    SUCCESS: if the data network was selected
    AEE_NET_EINVAL: if the network is not valid
    AEE_NET_EOPNOTSUPP: if the network has already been selected.
    EFAILED: or other error code, otherwise

Comments:
    Most applications will not need to explicitly select the network.

   When an INetMgr instance is created, no network is selected.  
   If an INetMgr method that requires a network is called before
    INETMGR_SelectNetwork(), AEE_NETWORK_DEFAULT will be implicitly
   selected, and the decision of which network to actually use is 
   deferred to lower layers. Decision will be made upon connection creation,
   depending on the device and/or service provider.

    Once a network has been selected, either explicitly via
    INETMGR_SelectNetwork(), or implicitly as described above, the
   network may not be changed. Hence, sockets opened by this INetMgr will
   be attached to the selected network. To use a different network, a new
    INetMgr instance is required.

   Note that ISockPort sockets are not tied to INetMgr, and each ISockPort 
   may use ISOCKPORT_SelectNetwork() independently to select a network.

   INETMGR_SelectNetwork is not a formal member of INetMgr; 
   instead it is a wrapper around INETMGR_SetOpt().

Version:
   Introduced BREW Client 3.1.0

See Also:
    INETMGR_SetOpt()
   Data networks

=============================================================================

INETMGR_SetDormancyTimeout()

Description:
    This function sets the IS-707 (cdma2000 1x) packet data dormancy timeout for
the network connection specified by pINetMgr to the value specified by ucSecs.
When the connection is created, the default dormancy timeout is set to 30 seconds.
If no network traffic occurs during a period of time equals to the dormancy timeout
then the mobile should perform mobile initiated dormancy. 

Prototype:
    int INETMGR_SetDormancyTimeout(INetMgr * pINetMgr, uint8 ucSecs)

Parameters:
    pINetMgr  :  Pointer to the INetMgr Interface object.
    ucSecs  :  Dormant timeout in seconds.
               0 means no timeout (no mobile initiated dormancy should occur)

Return Value:
   SUCCESS: if the timeout was set
   AEE_NET_EOPNOTSUPP: if the operation is not supported (i.e. not a cdma2000 1x network)
   EFAILED: or another error, otherwise

Comments:
   This is not a formal member of INetMgr; instead it is a wrapper
   around INETMGR_SetOpt().

Version:
   Introduced BREW Client 3.1

See Also:
   INETMGR_SetOpt()

=============================================================================

INETMGR_GetLastNetDownReason()

Description:
   This function returns the last network down reason.
Typically called by the application when the data network is CLOSED.

Prototype:
   int INETMGR_GetLastNetDownReason(INetMgr *me, AEENetDownReason *pnReason)

Parameters:
   me :        Pointer to the INetMgr Interface object.
   pnReason :  [out] - pointer to the last network down reason. On AEE_NET_SUCCESS 
               will be filled with the reason value (pre-allocated by caller).

Return Value:
   AEE_NET_SUCCESS: Success. pnNetDownReason filled with last network down reason.
   AEE_NET_EFAULT:  Lower layer returned an unexpected network down reason.
   
   Other AEE desinated error codes might be returned.

Comments:
   This is not a formal member of INetMgr; instead it is a wrapper
   around INETMGR_GetOpt().

Version:
   Introduced BREW Client 3.1.4 SP1

See Also:
   INETMGR_GetOpt()
   AEENetDownReason

=============================================================================

    INETMGR_SetOpt()

Description:
    This function provides the ability to configure certain network options. It
is similar in paradigm to ISOCKET_IOCtl(), but deals more with network-level items
able to be configured, rather than socket-level items able to be configured.

Prototype:
    int INETMGR_SetOpt
    (
    INetMgr *pINetMgr,
    int nOptName,
    void *pOptVal,
    int nOptSize
    );

Parameters:
   pINetMgr :  Pointer to the INetMgr Interface to be used to set network options.
   nOptName :  Integer identifying which option to set
   pOptVal :  Filled with option-specific data
   nOptSize :  Size of pOptVal

Return Value:
    SUCCESS: if the option was set
    EPRIVLEVEL: if access to the option is prohibited by applet privilege
    EUNSUPPORTED: if nOptName is unknown
    EBADPARM: if nOptSize is set to a size that does not correspond to the correct
option data~
    other error, depending on the option

Comments:
  Currently supported option ids are:
~
    RLP options: these options are highly device dependent, and as such may not
be able to be configured, available, or well-behaved. They are protected by the
system privilege level~
~
   INET_OPT_DEF_RLP3~
    Default RLP settings, pOptVal must be an AEERLP3Cfg~

   INET_OPT_CUR_RLP3~
    Current RLP settings, pOptVal must be an AEERLP3Cfg~

   INET_OPT_PPP_AUTH~
    Set the system's configured PPP authentication credentials (protected by PL_SYSTEM).

pOptVal's type is char *, and the format of the inputted string must be: "userid\000passwd\000".~
*

    DDTM option: this options is highly device dependent, and as such may not
be able to be configured, available, or well-behaved. It is protected by the
system privilege level.~
~
   INET_OPT_DDTM_PREF~
    Set the data dedicated transmission mode preference, pOptVal is a pointer to boolean

   INET_OPT_SELECT_NET~
    Select the data network to use.  If this option is not set, AEE_NETWORK_DEFAULT will be used~

   INET_OPT_DORMANCY_TIMEOUT~
    Set the packet data dormancy timeout (idle time until mobile dormancy is initiated) for a CDMA network.
    The default value is set to 30 seconds.

Side Effects:
    Depend on the option. All the above values modify only RAM values, and so they
are lost when the device is reset.

Version:
   Introduced BREW Client 2.0

See Also:
    INETMGR_GetOpt()
    AEEUDPUrgent

=============================================================================

=========================================================================*/

/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

ISocket Interface

The ISocket Interface provides methods to connect, transmit and receive data over,
and close TCP and UDP sockets that are opened using the INETMGR_OpenSocket().

NOTE: Your application must have a privilege level of Network or All to be able
to invoke the functions in this interface.

The function ISOCKET_Connect() is called immediately after opening a TCP socket.
If the network subsystem (physical layer, RLP and PPP) of the device is not active,
this function first establishes the necessary lower-layer protocol connections.
For TCP sockets, ISOCKET_Connect() then sets up a TCP connection to the specified
IP address and port number. For UDP sockets, the caller may specify the destination
IP address and port numbers for each read or write operation, thereby allowing data
to be sent to and received from multiple IP addresses and ports.

After the TCP socket has been connected, read and write operations may be used to
exchange data over it. All data transfer operations are non-blocking and asynchronous.
the callback functions are used to notify the caller of the availability of socket
for read or write operations. The caller can use ISOCKET_Readable() to provide the
address of a callback function that must be invoked when there is data available
to read. Similarly, ISOCKET_Writeable() registers the callback function that gets
invoked when the socket is available for writing. Callback functions are invoked
whenever BREW detects any error conditions on the socket that require the application
using this interface to take action.

The function ISOCKET_Cancel() is used to cancel a pending callback operation, thereby
preventing the application from receiving notification when there is a change in
the status of the socket it is waiting on.

The functions ISOCKET_Read() and ISOCKET_Write() are used for reading data from
and writing data to TCP sockets. If the read or write do not make progress, ISOCKET_Read()
and ISOCKET_Write()return an indication that blocking has occurred. The caller can
then use ISOCKET_Readable() or ISOCKET_Writeable() to arrange to be notified when
blocking is no longer present. If the number of bytes actually read or written is
less than the number requested, repeat calls to ISOCKET_Read() or ISOCKET_Write()
to complete the data transfer.

The functions ISOCKET_ReadV() and ISOCKET_WriteV() are used to receive and send
data on TCP sockets when the application uses multiple, non-contiguous buffers for
reading and writing. In place of the single buffer pointer supplied as a parameter
to ISOCKET_Read() and ISOCKET_Write(), ISOCKET_ReadV() and ISOCKET_WriteV() each
take an array of buffer descriptors as input, with each array element specifying
the length in bytes and starting address of a buffer. These functions attempt to
transfer an amount of data equal to the sum of the buffer lengths, starting with
the first buffer in the array.

The functions ISOCKET_RecvFrom() and ISOCKET_SendTo() are used to exchange data
over UDP sockets. Both these functions allow data to be sent to and received from
multiple IP addresses and port numbers. As with TCP sockets, the application may
call ISOCKET_Readable() or ISOCKET_Writeable() to designate a callback function
if the read or write operation does not make progress immediately

The function ISOCKET_Bind() associates a local port number with a socket (if an
application does not call this function before ISOCKET_Connect(), or ISOCKET_SendTo()
a default value is used for the local port number).

The functions ISOCKET_Listen() and ISOCKET_Accept() are used to implement a server
that can accept incoming TCP connections.

The function ISOCKET_GetLastError() returns the error code for the last error detected
by a function in the ISocket Interface. In cases where such a function returns something
other than success, ISOCKET_GetLastError() provides more detailed information about
why the function failed to perform its task.

The function ISOCKET_GetPeerName() returns the IP address and port number of the
entity with which data was most recently exchanged on the socket.

The function ISOCKET_GetSockName() returns the IP address and port number of the
local socket.

The functions ISOCKET_GetOpt() and ISOCKET_SetOpt() retrieve and specify optional
parameters for a socket and/or the underlying network stack.  See the AEESockOpt
documentation for the supported options.

Receiving multicast datagrams is enabled by setting the AEE_IP_ADD_MEMBERSHIP socket
option.  Note that multicast is only supported on a small subset of devices and
carrier networks; please verify support before making use of this feature.

After completion of data transfer, a call to ISOCKET_Shutdown() or ISOCKET_Close()
gracefully closes a TCP connection.

A call to ISOCKET_Release() also closes any socket connection and releases
the ISocket Interface and frees the associated resources.

See also ISOURCEUTIL_SourceFromSocket().

===H2>
To use the socket services
===/H2>

1.   Call ISHELL_CreateInstance() to create an instance of the INetMgr Interface.

2.   Call INETMGR_OpenSocket() to create an instance of the ISocket Interface (to
open a TCP or UDP socket).

3.   Call ISOCKET_Connect() to establish a TCP socket connection with the network
entity with which the application communicates.

4.   Use the functions in the ISocket Interface to operate on the socket.
===H2>
To read or write data over the socket
===/H2>

1.   Call the relevant (TCP/UDP, single/multiple I/O buffers) read or write functions
described above.

2.   If the function returns a blocking indication, call ISOCKET_Readable() or ISOCKET_Writeable()
to attempt the operation at a later time.

3.   If less than the requested number of bytes were transferred, call the read
or write function again to effect the transfer of the remaining data.

4.   Repeat steps 1-3, until all of your data has been transferred.

5.   Call ISOCKET_Bind() to specify the socket's local port number.

6.   Call ISOCKET_GetPeerName() to obtain the remote IP address and port number
of the connected TCP socket.

7.   Call ISOCKET_Release() to close the socket after completing the data transfer.

8.   Call INETMGR_Release() to release the INetMgr Interface instance.



=============================================================================

ISOCKET_Accept()

Description:
    The accept function is used on a listening TCP socket to retrieve
an incoming connection from the backlog queue (see ISOCKET_Listen()).
The first backlog queued connection is removed from the queue, and
associated with a new socket (as if you called OEMSocket_Open). The
newly created socket is in the connected state. The listening socket
is unaffected and the queue size is maintained (i.e. there is no need
to call listen again.)

Prototype:
   int ISOCKET_Accept(ISocket * ps, ISocket ** pps);

Parameters:
    ps :  [in]  :  Pointer to the listening ISocket interface
    pps :  [out]  :  Pointer to the newly accepted ISocket interface

Return Value:
   AEE_NET_SUCCESS:     A newly accepted socket has been returned via pps.
   AEE_NET_WOULDBLOCK:  No new sockets are available now; try again later.
                        (See ISOCKET_Readable().)
   AEE_NET_ERROR:       The operation failed.  The specific error code can
                        be retrieved by calling ISOCKET_GetLastError().

Comments:
    This is not a formal member of ISocket, instead it is a wrapper around ISOCKET_IOCtl().

Version:
   Introduced BREW Client 2.1

See Also:
    ISOCKET_Listen()
    ISOCKET_Readable()

=============================================================================

ISOCKET_AddRef()

Description:
    This function is inherited from IBASE_AddRef().


=============================================================================

ISOCKET_Bind()

Description:
   This function associates a local address and port with the socket. The bind
   function is used on an unconnected socket.
    
   When a socket is created with a call to the socket function, it exists in a
   name space (address family), but it has no name assigned to it. Passing AEE_INADDR_ANY
   (zero) for the address explicitly requests that the socket be assigned to any local
   address. Passing AEE_BREW_LOOPBACK allows local connections. 
   Passing a multicast group address can be used for receiving 
   multicast traffic destined to that group, if the selected network
   is a multicast network, such as MediaFLO or CDMA BCMCS. On other networks,
   multicast is not supported yet.
  
   NOTE: The ability to bind to a specific local IP address is not presently 
   supported, and the above options are the only valid values for the INAddr 
   parameter. In the case of AEE_INADDR_ANY, the local IP address is assigned
   automatically by the sockets library.

   Also note the possibility of an AEE_NET_WOULDBLOCK result. This typically occurs
   only in cases where a network connection, such as a PPP link, must be established.
   In that event, Bind() returns AEE_NET_WOULDBLOCK, and can be called again to obtain
   the final result (error or success). For notification of when to call Bind() again,
   the ISOCKET_Writeable() call can be used. (Note that just as with Write(), a
   ISOCKET_Writeable() callback does not guarantee that a subsequent call to Bind()
   will complete, so the caller must be prepared to receive AEE_NET_WOULDBLOCK again.)

   The user is not required to call Bind() until a non-WOULDBLOCK result is returned.
   The user can proceed to attempt to connect, send or receive data using ISOCKET_Connect(),
   ISOCKET_SendTo() or ISOCKET_RecvFrom(), in which case the bind operation (and any
   network startup) proceeds in the background, and any failure in startup phases is
   reflected in an ISOCKET_Connect(), ISOCKET_SendTo(), or ISOCKET_RecvFrom() error
   result.

Prototype:
    int ISOCKET_Bind(ISocket * pISocket, INAddr addr, uint16 wPort)

Parameters:
    pISocket: Pointer to the ISocket Interface object that needs to be connected.
    addr: IP Address.
    wPort: Port.

Return Value:
   AEE_NET_SUCCESS: if successful.
   AEE_NET_ERROR: on error.
   AEE_NET_WOULDBLOCK: if operation is blocked. call ISOCKET_Writeable() and ISOCKET_Bind()
when the callback occurs.

Comments:
    The specific error code can be retrieved by calling ISOCKET_GetLastError().
One of the following error codes is returned:


   AEE_NET_EADDRINUSE~
    Local address is already in use.

   AEE_NET_EAFNOSUPPORT~
    Address family not supported.

   AEE_NET_EBADF~
    Invalid socket descriptor is specified.

   AEE_NET_EFAULT~
    Invalid address parameter has been specified.

   AEE_NET_EINVAL~
    Socket is already attached to a local name.

   AEE_NET_EOPNOTSUPP~
    Specific local IP address was requested (not supported) or
    the socket is a TCP socket

   AEE_NET_GENERAL_FAILURE~
    General failure.

   AEE_NET_WOULDBLOCK~
    No data available now; try again later (See ISOCKET_Readable()).



Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ISOCKET_Cancel()

Description:
    This function allows an application to cancel (un-register) a callback function
that has been previously registered using ISOCKET_Readable(), ISOCKET_Writeable()
or ISOCKET_Connect().
Cancellation annuls the effect of the prior registration so that the callback 
function is not called when the socket makes progress. 

    When both the pfn and pUser parameters are NULL, all registered 
callbacks are cancelled.

Prototype:
    void ISOCKET_Cancel(ISocket * pISocket, PFNNOTIFY pfn, void * pUser)

Parameters:
    pISocket  :  Pointer to the ISocket Interface for which the callback function
needs to be de-registered.
    pfn  :  Pointer to the callback function that needs to be cancelled.
    pUser  :  Callback data that was used to register the callback.

Return Value:
    None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
    PFNNOTIFY

=============================================================================

ISOCKET_Close()

Description:
    Causes a socket to be closed. In the case of TCP, any connection will be terminated
gracefully.
    The last ISOCKET_Release() implicitly performs a close in the background. Prior
to the last ISOCKET_Release(), ISOCKET_Close() can be called in order to
     -    Explicitly begin the close process, and/or
     -    Determine when the socket close process is complete.

    The operation will fail if the socket has not been implicitly or explicitly Realized.

Prototype:
   int ISOCKET_Close(ISocket * ps);

Parameters:
   ps:   Pointer to the ISocket interface to close

Return Value:
   AEE_NET_SUCCESS:     The socket has been successfully closed.
   AEE_NET_WOULDBLOCK:  Close in progress; use either ISOCKET_Readable() or
                        ISOCKET_Writeable() to be notified when to try again.
   AEE_NET_ERROR:       The operation failed.  The specific error code can
                        be retrieved by calling ISOCKET_GetLastError().

Comments:
   This is not a formal member of ISocket, instead it is a wrapper around ISOCKET_IOCtl()

Version:
   Introduced BREW Client 2.1

See Also:
    ISOCKET_Readable()
    ISOCKET_Writeable()

=============================================================================

ISOCKET_Connect()

Description:
    For sockets of type AEE_SOCK_STREAM, this attempts to initiate a TCP connection
to the specified address and port.

    Prior to performing any socket-specific behavior, ISOCKET_Connect() will perform
an ISOCKET_Realize() if necessary.

    After establishment of a connection to the Internet, the callback is passed
the error code that describes how the connect operation completed. As with all network
callbacks, the callback is called within the same thread context, at some point
in time after the caller returns control to the AEE event loop.

Prototype:
    int ISOCKET_Connect
    (
    ISocket * pISocket,
    INAddr a,
    INPort wPort,
    PFNCONNECTCB pfn,
    void * pUser
    )

Parameters:
    pISocket  :  Pointer to the ISocket Interface object.
    a  :  IP Address.
    wPort  :  Port.
    pfn  :  Address of the callback function that is invoked by AEE when the connect
operation either succeeds or fails.
    pUser  :  User-defined data that is passed to the callback function when it
          is invoked.

Return Value:
   AEE_NET_SUCCESS: if successful. This indicates that the callback will be called
after the caller relinquishes control.

   AEE_NET_ERROR: when a NULL callback pointer (pfn) was passed.
    This is the only case in which the callback will not be called.

Comments:
    The registered callback can be cancelled (un-registered) with ISOCKET_Cancel().

    The INAddr and INPort arguments are assumed to be in network byte order (for
example, big-endian). This issue is important for portability across simulator environments
and potential device targets. The callback is passed one of the following values
describing the completion of the connect operation:

   AEE_NET_EADDRINUSE~
    Local address requested via a prior call to ISOCKET_Bind() was not available.

   AEE_NET_EBADF~
    Invalid socket descriptor is specfied

   AEE_NET_ECONNREFUSED~
    Connection attempt refused

   AEE_NET_EIPADDRCHANGED~
    IP address changed due to PPP resync

   AEE_NET_EINVAL~
    Invalid IP address and/or port specified

   AEE_NET_EISCONN~
    The socket is already connected (result was already received by a previous
call to Connect())

   AEE_NET_EMFILE~
    Not enough network resources to complete this operations (too many sockets
in use).
   AEE_NET_ENETDOWN~
    Network is not available (e.g. handset outside of coverage area)

   AEE_NET_EOPNOTSUPP~
    Socket type is not STREAM.

   AEE_NET_ETIMEDOUT~
    Connection attempt timed out

   AEE_NET_SUCCESS~
    Connect completed successfully; socket is prepared for reading and/or writing.


Version:
   Introduced BREW Client 1.0

See Also:
    PFNCONNECTCB
    INAddr
    INPort

=============================================================================

ISOCKET_DbgMark()

Description:
    This function marks an ISocket with __FILE__ and __LINE__ for debugging purposes.
It is a wrapper for ISOCKET_IOCtl(), so ISockets may actually be "marked" with any
string/integer pair.

Prototype:
    void ISOCKET_DbgMark(ISocket *pISocket)

Parameters:
    pISocket :  Pointer to the ISocket interface to mark

Return Value:
    None

Comments:
    This is not a formal member of ISocket, instead it is a wrapper around ISOCKET_IOCtl()

Version:
   Introduced BREW Client 2.1

See Also:
   ISOCKET_IOCtl()

=============================================================================

ISOCKET_GetLastError()

Description:
        This function returns the last error that occurred with the given Socket.The
value returned depends on the most recently called function. The documentation for
each function describes what this function can return when called right after that
function.

Prototype:
        int ISOCKET_GetLastError(ISocket * pISocket)

Parameters:
    pISocket  :  Pointer to the ISocket Interface object.

Return Value:
    The most recently occurred error.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

ISOCKET_GetPeerName()

Description:
    This function returns the remote IP address and port of the connected peer. This
function is not supported for UDP.

Prototype:
    int ISOCKET_GetPeerName
    (
    ISocket * pISocket,
    INAddr * pa,
    INPort * pwPort
    )

Parameters:
    pISocket  :  Pointer to the I ISocket Interface object.
    pa  :  Pointer to the IP Address in network byte order
    pwPort  :  Pointer to the port in network byte order.

Return Value:
   AEE_NET_SUCCESS: if successful.
   AEE_NET_EOPNOTSUPP: If UDP.~
    Network error codes pertaining to the network failure.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
    INAddr
    INPort

=============================================================================

ISOCKET_GetSockName()

Description:
   This function returns the local IP address and port of the socket.

Prototype:
    int ISOCKET_GetSockName
    (
    ISocket * pISocket,
    INAddr * pAddr,
    INPort * pPort
    )

Parameters:
    pISocket :  [in] :  Pointer to the ISocket Interface whose name is to be returned.
    pAddr :  [out] :  Pointer to the IP Address t in network byte order.
    pPort :  [out] :  Pointer to the port t in network byte order.

Return Value:
    AEE_NET_SUCCESS: The local IP address and port have been returned.

    Specific error code: If the operation failed.

Comments:
    This is not a formal member of ISocket. Instead it is a wrapper around ISOCKET_IOCtl()

Version:
   Introduced BREW Client 2.1

See Also:
    INAddr
    INPort

=============================================================================

ISOCKET_GetOpt()

Description:
   Gets options for a socket.

Prototype:
    int ISOCKET_GetOpt
    (
    ISocket * pISocket,
    int nLevel,
    int nOptName,
    void *pOptVal,
    int* pnOptSize
    )

Parameters:
    pISocket :  [in] : Pointer to the ISocket Interface whose name is to be returned.
    nLevel :  [in] : Option level (see AEESockOpt)
    nOptName :  [in] : Option name (see AEESockOpt)
    pOptVal :  [out] : Pointer to the data type appropriate for the option (see AEESockOpt)
    pnOptSize :  [in/out] : Pointer to size of pOptVal

Return Value:
   AEE_NET_SUCCESS:     The socket option has been successfully retrieved.
   AEE_NET_WOULDBLOCK:  Operation in progress; use ISOCKET_Readable() to
                        be notified when to try again.
   AEE_NET_ERROR:       The operation failed.  The specific error code can
                        be retrieved by calling ISOCKET_GetLastError().

Comments:
   This is not a formal member of ISocket, instead it is a wrapper around ISOCKET_IOCtl()

Version:
   Introduced BREW Client 3.1

See Also:
   ISOCKET_SetOpt()
   ISOCKET_Readable()
   AEESockOpt

=============================================================================

ISOCKET_GetSDBAckInfo()

Description:
   Gets information received by SDB Ack.

Prototype:
   int ISOCKET_GetSDBAckInfo
   (
      ISocket * pISocket,
      AEESDBAckInfo* pSDBAckInfo
   )

Parameters:
   pISocket :  [in] : Pointer to the ISocket Interface whose name is to be returned.
   pSDBAckInfo : [out] : Pointer to a AEESDBAckInfo structure.

Return Value:
   AEE_NET_SUCCESS:     The socket option has been successfully retrieved.
   AEE_NET_ERROR:       The operation failed.  The specific error code can
   be retrieved by calling ISOCKET_GetLastError().

Comments:
   This is not a formal member of ISocket, instead it is a wrapper around ISOCKET_IOCtl()

Version:
   Introduced BREW Client 3.1.5 SP01.

See Also:
   ISOCKET_SetOpt()
   ISOCKET_GetOpt
   ISOCKET_Readable()
   AEESockOpt

===========================================================================

ISOCKET_IOCtl()

Description:
    This function allows some control over the behavior of an ISocket, for example
connect time-outs. Its form is analogous to SYSV's ioctl, with encoded arguments
that should be specified with the _IOCTL_ macros listed here. It is intended use
is for special, out-of-object-signature behavior. For that reason, it is generic.

Prototype:
    int ISOCKET_IOCtl(ISocket * pISocket, int nOption, uint32 dwVal);

Parameters:
    pISocket :  Pointer to the ISocket interface
    nOption :  The option id
    dwVal :  The option value

Return Value:
   SUCCESS: the option id is known and the specified operation succeeds
   EUNSUPPORTED: the option id was unrecognized~
   Other error code, depending on the option id specified

Comments:
  Currently supported option ids are:

    ISOCKET_IOCTL_ACCEPT
     dwVal set to a pointer to pointer to ISocket, which is filled in with the newly
connected socket (see ISOCKET_Accept())

    ISOCKET_IOCTL_CLOSE
     dwVal is unused (see ISOCKET_Close())

    ISOCKET_IOCTL_GCLOSER
     dwVal set to a pointer to an ISocketCloser struct, which will be filled with
the ISocket's current sock closer callback

    ISOCKET_IOCTL_GCONNTIMEOUT
     dwVal set to a pointer to an uint32, which will be filled in with the current
time in milliseconds to wait for connect to succeed

    ISOCKET_IOCTL_GDBGMARK
     dwVal set to a pointer to an ISocketDbgMark, which is filled with the current
ISocket's mark (if any)

    ISOCKET_IOCTL_GETSOCKOPT
     dwVal set to a pointer to an AEESockOpt (see ISOCKET_GetOpt())

    ISOCKET_IOCTL_GETSOCKNAME
     dwVal set to a pointer to an INSockAddr (see ISOCKET_GetSockName())

    ISOCKET_IOCTL_LISTEN
     dwVal set to an int indicating the max backlog (see ISOCKET_Listen())

    ISOCKET_IOCTL_QI
     dwVal set to a pointer to an ISocketQI, to do QueryInterface()

    ISOCKET_IOCTL_REALIZE
     dwVal is unused (see ISOCKET_Realize())

    ISOCKET_IOCTL_SCLOSER
     dwVal set to a pointer to an ISocketCloser struct, which specifies the ISocket's
new closer callback

    ISOCKET_IOCTL_SCONNTIMEOUT
     dwVal set to time in milliseconds (zero for default) to wait for connect to
succeed

    ISOCKET_IOCTL_SDBGMARK
     dwVal set to a pointer to an ISocketDbgMark, specifies how to mark the ISocket
(see ISocketDbgMark)

    ISOCKET_IOCTL_SETSOCKOPT
     dwVal set to a pointer to an AEESockOpt (see ISOCKET_SetOpt())

    ISOCKET_IOCTL_SHUTDOWN
     dwVal set to an int indicating how to shutdown (AEE_SHUTDOWN_*) (see ISOCKET_Shutdown())

Version:
   Introduced BREW Client 1.0

See Also:
   ISOCKET_Accept()
   ISOCKET_Close()
   ISOCKET_GetOpt()
   ISOCKET_GetSockName()
   ISOCKET_Listen()
   ISOCKET_Realize()
   ISOCKET_SetOpt()
   ISOCKET_Shutdown()

=============================================================================

ISOCKET_Listen()

Description:
    This function performs a passive open for connections, such that incoming connections
   may be subsequently accepted.
    The socket must be a TCP socket that has been bound to a local port. The nMaxBacklog
parameter indicates the maximum length for the queue of pending connections. If
nMaxBacklog is less than one, it will be silently increased to one. If nMaxBacklog
is larger than the system maximum, it will be silently reduced to the system maximum.


Prototype:
   int ISOCKET_Listen(ISocket * ps, int nMaxBacklog);

Parameters:
   ps:          Pointer to the ISocket interface to put in listening mode
   nMaxBacklog: The maximum number of pending connections

Return Value:
   AEE_NET_SUCCESS:     The socket has been placed in listening mode.
   AEE_NET_WOULDBLOCK:  Unable to listen now; try again later.
                        (See ISOCKET_Writeable())
   AEE_NET_ERROR:       The operation failed.  The specific error code can
                        be retrieved by calling ISOCKET_GetLastError().

Comments:
   This is not a formal member of ISocket, instead it is a wrapper around ISOCKET_IOCtl()

Version:
   Introduced BREW Client 2.1

See Also:
    ISOCKET_Accept()
    ISOCKET_Writeable()

=============================================================================

ISOCKET_Read()

Description:
    This function reads data from a socket into a single buffer specified as a parameter
to this function. This function always returns immediately. If there is no data
available and the connection is still active, ISOCKET_Read() returns AEE_NET_WOULDBLOCK.
To be notified when to call ISOCKET_Read() again, the caller must call ISOCKET_Readable().

Prototype:
    int32 ISOCKET_Read(ISocket * pISocket, byte * pbDest, uint32 wSize)

Parameters:
    pISocket  :  Pointer to the ISocket Interface object.
    pbDest  :  Pointer to the buffer to hold the data that is to be received.
    wSize  :  Specifies the number of bytes to read.

Return Value:
    bytes_read, any positive number indicates the number of bytes that have been
                        successfully read into the provided buffer.

    0 (zero), if there is no more data to be received; the peer has shut down the
connection.

    AEE_NET_WOULDBLOCK, if no data available now; try again later (See ISOCKET_Readable()).

    AEE_NET_ERROR, if the socket is not in a valid state to receive data.

Comments:
    The specific error code can be retrieved by calling ISOCKET_GetLastError().
Error codes returned:

    AEE_NET_EBADF
     Invalid socket descriptor is specified

    AEE_NET_ENOTCONN
     Socket not connected

    AEE_NET_ECONNRESET
     TCP connection reset by server

    AEE_NET_ECONNABORTED
     TCP connection aborted because of a time-out or other failure

    AEE_NET_EIPADDRCHANGED
     IP address changed, causing TCP connection reset

    AEE_NET_EPIPE
     Broken pipe

    AEE_NET_ENETDOWN
     Network subsystem unavailable

    AEE_NET_EFAULT
     Application buffer not valid part of address space

    AEE_NET_GENERAL_FAILURE
     General failure.

    AEE_NET_ESHUTDOWN
     Connection is shutdown for reading.

Version:
   Introduced BREW Client 1.0

See Also:
    ISOCKET_ReadV()
    ISOCKET_Write()
    ISOCKET_WriteV()

=============================================================================

ISOCKET_Readable()

Description:
   This function allows an application to register a callback function to be invoked
by the AEE when a non-blocking read operation (Accept, Read, ReadV, or RecvFrom)
on the specified socket makes progress. Progress can involve returning data, returning
an error code, or returning 0 (zero) to indicate a closed connection—anything but
AEE_NET_WOULDBLOCK. This function is typically be used after a previous read attempt
returned AEE_NET_WOULDBLOCK, but it can be used even if no read function has been
   called.

    NOTE: There is no absolute guarantee that the read function makes progress after
the callback. The caller must be prepared to call ISOCKET_Readable() again when
the read function return AEE_NET_WOULDBLOCK.

Prototype:
    void ISOCKET_Readable(ISocket * pISocket, PFNNOTIFY pfn, void * pUser)

Parameters:
    pISocket  :  Pointer to the ISocket Interface object.
    pfn  :  Address of the callback function. This function is invoked by AEE when
the socket becomes ready to be read or when it is ready to be closed. If this is
NULL, it cancels the Readable callback function registered by a previous call to
ISOCKET_Readable (if any).
    pUser  :  User-defined data that is passed to the callback function when it
   is invoked.

Return Value:
    None

Comments:
   The registered callback can be cancelled (un-registered) with ISOCKET_Cancel().

Version:
   Introduced BREW Client 1.0

See Also:
    PFNNOTIFY

=============================================================================

ISOCKET_ReadV()

Description:
    This function reads data from a socket into one or more buffers described by
the entries in the iov[] array. This function always returns immediately. If there
is no data available and the connection is still active, ISOCKET_ReadV() returns
AEE_NET_WOULDBLOCK. To be notified when to call ISOCKET_ReadV() again, the caller
must call ISOCKET_Readable().

Prototype:
    int32 ISOCKET_ReadV
    (
    ISocket * pISocket,
    SockIOBlock iov[],
    uint16 iovcount
    )

Parameters:
    pISocket  :  Pointer to the ISocket Interface object.
    iov  :  Array of SockIOBlock structures into which data can be read.
    iovcount  :  Specifies the number of entries in the iov array.

Return Value:
    bytes_read, any positive number indicates the number of bytes that have been
successfully read into the provided buffer.

    0 (zero), if there is no more data to be received; the peer has shut down the
connection.

    AEE_NET_WOULDBLOCK, if no data available now; try again later (See ISOCKET_Readable()).

    AEE_NET_ERROR, if the socket is not in a valid state to receive data.

Comments:
    The specific error code can be retrieved by calling ISOCKET_GetLastError().
The error codes returned include:

    AEE_NET_EBADF
     Invalid socket descriptor is specified.

    AEE_NET_ECONNABORTED
     TCP connection aborted because of a time-out or other failure.

    AEE_NET_ECONNRESET
     TCP connection reset by server.

    AEE_NET_EFAULT
     Application buffer not valid part of address space.

    AEE_NET_EIPADDRCHANGED
     IP address changed, causing TCP connection reset.

    AEE_NET_ENETDOWN
     Network subsystem unavailable.

    AEE_NET_ENOTCONN
     Socket not connected.

    AEE_NET_EPIPE
     Broken pipe.

    AEE_NET_EWOULDBLOCK
     Operation would block.

    AEE_NET_GENERAL_FAILURE
     General failure.

    AEE_NET_ESHUTDOWN
     Connection is shutdown for reading.

Version:
   Introduced BREW Client 1.0

See Also:
    SockIOBlock
    ISOCKET_Read()
    ISOCKET_Write()
    ISOCKET_WriteV()

=============================================================================

ISOCKET_Realize()

Description:
    Acquires an IP address and ensures that the underlying network layer is ready
for communication. This may involve the establishment of an Internet connection
using CDMA Packet Data or QNC if such a connection has not already been established.
Finally, any previously-requested ISOCKET_Bind() operation will be attempted if
necessary. As a result, errors relating to network startup or ISOCKET_Bind() could
be reported here, and AEE_NET_WOULDBLOCK return values are commonly encountered.

Prototype:
   int ISOCKET_Realize(ISocket * ps);

Parameters:
   ps:   Pointer to the ISocket interface to realize

Return Value:
   AEE_NET_SUCCESS:    The socket has been successfully realized.
   AEE_NET_WOULDBLOCK: Realize in progress; use either ISOCKET_Readable() or ISOCKET_Writeable()
                       to be notified when to try again.
   AEE_NET_ERROR:      The operation failed.  The specific error code can
                       be retrieved by calling ISOCKET_GetLastError().
Comments:
   This is not a formal member of ISocket, instead it is a wrapper around ISOCKET_IOCtl()

Version:
   Introduced BREW Client 2.1

See Also:
    ISOCKET_Readable()
    ISOCKET_Writeable()

=============================================================================

ISOCKET_RecvFrom()

Description:
    This functions reads data from UDP socket and records the IP address and port
of the sender if non-NULL. This function always returns immediately with the number
of bytes read. If no packets have arrived and the socket is still in a valid state,
ISOCKET_RecvFrom() returns AEE_NET_WOULDBLOCK. ISOCKET_Readable() may be used to
receive notification of when to try ISOCKET_RecvFrom() again.

    Prior to performing any socket-specific behavior, ISOCKET_RecvFrom() performs
an ISOCKET_Realize() if neccesary.

Prototype:
    int32 ISOCKET_RecvFrom
    (
    ISocket * pISocket,
                          byte *pBuff,
                          uint16 wbytes,
                          uint16 wflags,
                          INAddr * pa,
    INPort * pwPort
    )

Parameters:
    pISocket  :  Pointer to the ISocket Interface object.
    pBuff  :  Buffer to hold the received data.
    wBytes  :  Size of the buffer, in terms of number of bytes.
    wflags  :  Data transport options.
    pa  :  Pointer to the IP Address in network byte order. May be NULL only if
not of interest to the application.
    pwPort  :  Pointer to the port in network byte order. May be NULL only if not
of interest to the application.

Return Value:
   bytes_read: any positive number indicates the number of bytes that have been
                       successfully read into the provided buffer.

   AEE_NET_WOULDBLOCK: if no data available now; try again later (See ISOCKET_Readable()).

   AEE_NET_ERROR: if the socket is not in a valid state to receive data. The specific
                        error code can be retrieved by calling ISOCKET_GetLastError().

Comments:
    The specific error code can be retrieved by calling ISOCKET_GetLastError() and
one of the following error codes is returned:

    AEE_NET_EEOF
     End of file.

    AEE_NET_EBADF
     Invalid socket descriptor is specified.

    AEE_NET_EAFNOSUPPORT
     Address family not supported.

    AEE_NET_ENETDOWN
     Network subsystem unavailable.

    AEE_NET_EFAULT
     Application buffer not valid part of address space.

    AEE_NET_EOPNOTSUPP
     Option not supported.

    AEE_NET_GENERAL_FAILURE
     General failure.

Version:
   Introduced BREW Client 1.0

See Also:
    INAddr
    INPort

=============================================================================

ISOCKET_Release()

Description:
    This function is inherited from IBASE_Release().

Version:
   Introduced BREW Client 1.0

See Also:
    ISOCKET_AddRef()

=============================================================================

ISOCKET_SendTo()

Description:
   This function sends a UDP packet to the specified IP address and port from the
   local port bound to the socket.  This function must be used with sockets of type
AEE_NET_DGRAM (UDP), not with sockets of type AEE_NET_STREAM (TCP). If the socket
has not been bound to a local address and port, this function binds it to a port.
    Sockets always operate in a non-blocking mode. This function returns immediately
and, if successful, returns the number of bytes written. If no bytes are successfully
sent and the connection is still active, the function returns AEE_NET_WOULDBLOCK.
To be notified when to call ISOCKET_SendTo() again, the caller must call ISOCKET_Writeable().
    Prior to performing any socket-specific behavior, ISOCKET_Connect() will perform
an ISOCKET_Realize() if neccesary.

   Two flags are supported for the SendTo operation on UDP sockets:
     -    ISOCKET_FLAG_SENDTO_URGENT flag will attempt to deliver the message even
if the network is asleep (e.g. on a common channel instead of a dedicated channel).
   There will probably be limits on the size of such signaling, even if it is supported.
See the INETMGR_GetOpt() function for details. This feature should be used with discretion,
   and requires a privilege: NET_URGENT must be specified as an external dependency in the application's MIF
   in order to utilize this option. It will be useful for applications with very low bandwidth
   requirements or applications which are sensitive to the additional network delay
   of waiting for the network to be awakened from dormancy.
     -    ISOCKET_FLAG_SENDTO_WAKEUP flag can be used in conjunction with the ISOCKET_FLAG_SENDTO_URGENT
flag to concurrently request that the network should be awakened from dormancy as
soon as possible, subsequent to the message delivery. If only the ISOCKET_FLAG_SENDTO_WAKEUP
is set, UDP data will NOT be sent. This feature requires System privileges.

Prototype:
    int32 ISOCKET_SendTo
    (
    ISocket * pISocket,
                        byte * pBuff,
                        uint16 wBytes,
                        uint16 wflags,
    INAddr a,
    INPort wPort
    )

Parameters:
    pISocket  :  Pointer to the ISocket Interface object.
    pBuff  :  Buffer containing data to be sent.
    wBytes  :  Size of the buffer, in terms of number of bytes.
    wflags  :  Data transport options.
    ISOCKET_FLAG_SENDTO_WAKEUP
    ISOCKET_FLAG_SENDTO_URGENT
    a  :  IP Address in network byte order.
    wPort  :  Port in network byte order.

Return Value:
    bytes_sent ( > 0)
     Any positive number indicates the number of bytes successfully sent.

   AEE_NET_WOULDBLOCK:
    Cannot send data at this time. Use ISOCKET_Writeable() to wait for readiness.
This condition could be encountered if transmit buffers have been filled, or when
a network connection is being established.

   AEE_NET_ERROR:
    Indicates failure. The specific error code can be retrieved by calling ISOCKET_GetLastError().
One of the AEE Error Codes is returned

Comments:
  AEE_NET_EMFILE errors can be reported here if too many sockets are open.

Version:
   Introduced BREW Client 1.0

See Also:
    ISOCKET_RecvFrom()
    INAddr
    INPort

=============================================================================

ISOCKET_SetOpt()

Description:
   Sets options for a socket.

Prototype:
    int ISOCKET_SetOpt
    (
    ISocket * pISocket,
    int nLevel,
    int nOptName,
    const void *pOptVal,
    int nOptSize
    )

Parameters:
    pISocket :  [in] : Pointer to the ISocket Interface whose name is to be returned.
    nLevel :  [in] : Option level (see AEESockOpt)
    nOptName :  [in] : Option name (see AEESockOpt)
    pOptVal :  [out] : Pointer to the data type appropriate for the option (see AEESockOpt)
    nOptSize :  [in] : Size of pOptVal

Return Value:
   AEE_NET_SUCCESS:     The socket option has been successfully set.

   AEE_NET_WOULDBLOCK:  Operation in progress; use ISOCKET_Writeable() to
                        be notified when to try again.

   AEE_NET_ERROR:       The operation failed.  The specific error code can
                        be retrieved by calling ISOCKET_GetLastError().

Comments:
   This is not a formal member of ISocket, instead it is a wrapper around ISOCKET_IOCtl()

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ISOCKET_GetOpt()
   ISOCKET_Writeable()
   AEESockOpt

===========================================================================

ISOCKET_Shutdown()

Description:
    This function causes all or part of a full-duplex connection to be terminated
gracefully.
~
    If nHow is AEE_SHUTDOWN_RD, no more reads are allowed.   ~
    If nHow is AEE_SHUTDOWN_WR, no more writes are allowed (AKA half-close).~
    If nHow is AEE_SHUTDOWN_RDWR, both read and write are disallowed.

    NOTE: This function shuts down the socket for TCP sockets only, but it returns
an error message for UDP sockets.
*

Prototype:
    int ISOCKET_Shutdown(ISocket * ps, int nHow);

Parameters:
   ps:   Pointer to the ISocket interface to shutdown
   nHow: Dictates which portion(s) of the connection to shutdown

Return Value:
   AEE_NET_SUCCESS:     The socket has been successfully shutdown.
    (See ISOCKET_Writeable().)

    AEE_NET_WOULDBLOCK: Shutdown in progress   ~
    Use ISOCKET_Readable() for AEE_SHUTDOWN_RD    ~
    or ISOCKET_Writeable() for AEE_SHUTDOWN_WR ~
	or AEE_SHUTDOWN_RDWR to be notified
when to try again.

    AEE_NET_ERROR: The operation failed.
    The specific error code can be retrieved by calling ISOCKET_GetLastError().

Comments:
   This is not a formal member of ISocket, instead it is a wrapper around ISOCKET_IOCtl()

Version:
   Introduced BREW Client 2.1

See Also:
    ISOCKET_Writeable()

=============================================================================

ISOCKET_Write()

Description:
    This function writes data to a connected socket from a single buffer. This function
always returns immediately with the number of bytes that were successfully written.
If no bytes can be successfully written and the connection is still active, ISOCKET_Write()
returns AEE_NET_WOULDBLOCK. To be notified when to call again, the caller must call
   ISOCKET_Writeable().

Prototype:
    int32 ISOCKET_Write(ISocket * pISocket, byte * pBuffer, uint32 wBytes)

Parameters:
    pISocket  :  Pointer to the I ISocket Interface object.
    pBuffer  :  Pointer to the buffer from which the data is sent.
    wBytes  :  Specifies the size of the buffer in terms of number of bytes in the
buffer.

Return Value:
    bytes_written: ( > 0), Any positive number indicates the number of bytes successfully
written.

    AEE_NET_WOULDBLOCK: No bytes can be written at this time; try again later (See
ISOCKET_Writeable()).

    AEE_NET_ERROR, Failed to write any bytes.

Comments:
    The specific error code can be retrieved by calling ISOCKET_GetLastError().
One of the following possible error codes is returned.

    AEE_NET_EBADF
     Invalid socket descriptor is specified.

    AEE_NET_ENOTCONN
     Socket not connected.

    AEE_NET_ECONNRESET
     TCP connection reset by server.

    AEE_NET_ECONNABORTED
     TCP connection aborted because of a time-out or other failure.

    AEE_NET_EIPADDRCHANGED
     IP address changed, causing TCP connection reset.

    AEE_NET_EPIPE
     Broken pipe.

    AEE_NET_ENETDOWN
     Network subsystem unavailable.

    AEE_NET_EFAULT
     Application buffer not valid part of address space.

    AEE_NET_GENERAL_FAILURE
     General failure.

    AEE_NET_ESHUTDOWN
     Connection is shutdown for writing.

Version:
   Introduced BREW Client 1.0

See Also:
    ISOCKET_Read()
    ISOCKET_ReadV()
    ISOCKET_WriteV()

=============================================================================

ISOCKET_Writeable()

Description:
   This function allows an application to register a callback function to be invoked
by the AEE when a non-blocking write operation on the specified socket can make
progress. Progress involves writing data or returning an error code -- anything
but AEE_NET_WOULDBLOCK. This function is used after a previous write attempt returned
AEE_NET_WOULDBLOCK, but it is used even if no write function has been called.

    For a datagram socket, ISOCKET_SendTo() can be used on multiple
interfaces (e.g. BREW Loopback and PPP) for the same socket.  In
this case, ISOCKET_Writeable() is tied to the previous call to
ISOCKET_SendTo().  If ISOCKET_SendTo() has never been called,
ISOCKET_Writeable() is tied to all available interfaces.

    NOTE: There is no absolute guarantee that the write function makes progress
after the callback, so the caller must always be prepared to call ISOCKET_Writeable()
again when the write function returns AEE_NET_WOULDBLOCK.

Prototype:
    void ISOCKET_Writeable(ISocket * pISocket, PFNNOTIFY pfn, void * pUser)

Parameters:
    pISocket  :  Pointer to the ISocket Interface object for which the callback
function needs to be registered.
    pfn  :  Address of the callback function. This function is invoked by AEE when
the socket becomes ready to receive data or when it is ready to be closed. If this
is NULL, it cancels the Writeable callback function registered by a previous call
to ISOCKET_Writeable().
    pUser  :  User-defined data that is passed to the callback function when it
is invoked.

Return Value:
    None

Comments:
    The registered callback can be cancelled (un-registered) with ISOCKET_Cancel().

    Non-blocking write operation include:
~
    ISOCKET_Write()~
    ISOCKET_WriteV()~
    ISOCKET_SendTo())~
    ISOCKET_Bind()~
    ISOCKET_Listen()~
    ISOCKET_Shutdown()~
    ISOCKET_Close()~
*

Version:
   Introduced BREW Client 1.0

See Also:
    PFNNOTIFY

=============================================================================

ISOCKET_WriteV()

Description:
   This function writes data to a connected socket.  It gathers data from one or
more buffers described by the entries in the iov[ ] array into a single write operation.
Behavior can differ from separate calls to ISOCKET_Write() in that multiple calls
to ISOCKET_Write() can generate unnecessary multiple TCP packets.

    This function always returns immediately with the number of bytes that were
successfully written. If no bytes can be successfully written and the connection
is still active, ISOCKET_WriteV() returns AEE_NET_WOULDBLOCK. To be notified when
to call ISOCKET_WriteV() again, the caller must call ISOCKET_Writeable().

Prototype:
    int32 ISOCKET_WriteV
    (
    ISocket * pISocket,
    SockIOBlock iov[],
    uint16 wiovcount
    )

Parameters:
    pISocket  :  Pointer to the ISocket Interface object.
    iov[ ] :  An array of SockIOBlocks containing the individual buffers to be sent.
    wiovcount: Number of entries inside the SockIOBlock structure.

Return Value:
    bytes_written: ( > 0), Any positive number indicates the total number of bytes
                         successfully written from all of the iov[] buffers.

    AEE_NET_WOULDBLOCK: No bytes can be written at this time; try again later (See
ISOCKET_Writeable().

    AEE_NET_ERROR, Failed to write any bytes.

Comments:
    The Network specific error code can be retrieved by calling ISOCKET_GetLastError().
One of the following error codes is returned:

    AEE_NET_EBADF
     Invalid socket descriptor is specified.

    AEE_NET_ENOTCONN
     Socket not connected.

    AEE_NET_ECONNRESET
     TCP connection reset by server.

    AEE_NET_ECONNABORTED
     TCP connection aborted because of a time-out or other failure.

    AEE_NET_EIPADDRCHANGED
     IP address changed, causing TCP connection reset.

    AEE_NET_EPIPE
     Broken pipe.

    AEE_NET_ENETDOWN
     Network subsystem unavailable.

    AEE_NET_EFAULT
     Application buffer not valid part of address space.

    AEE_NET_GENERAL_FAILURE
     General failure.

    AEE_NET_ESHUTDOWN
     Connection is shutdown for writing

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

===========================================================================*/

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

    NMASK_OPENED

Description:
    This is a notification mask that can be used by applications to
     register to be notified (via IAPPLET_HandleEvent()) when PPP is
     opened.

Definition:
  #define NMASK_OPENED      0x0001

Members:
    None

Comments:
    None

Version:
   Introduced BREW Client 3.1

See Also:
    INotifier, ISHELL_RegisterNotify()

=======================================================================

    NMASK_CLOSED

Description:
    This is a notification mask that can be used by applications to
     register to be notified (via IAPPLET_HandleEvent()) when PPP is
     closed.

Definition:
  #define NMASK_CLOSED      0x0002

Members:
    None

Comments:
    None

Version:
   Introduced BREW Client 3.1

See Also:
    INotifier, ISHELL_RegisterNotify()

=======================================================================

    NMASK_IDLE

Description:
    This is a notification mask that can be used by applications to
     register to be notified (via IAPPLET_HandleEvent()) when PPP is
     in a quiescent state.  This is defined as PPP is up, but no open
     sockets exist.

Definition:
  #define NMASK_IDLE      0x0004

Members:
    None

Comments:
    None

Version:
   Introduced BREW Client 3.1

See Also:
    INotifier, ISHELL_RegisterNotify()

=======================================================================

    NMASK_ALSEEP

Description:
    This is a notification mask that can be used by applications to
     register to be notified (via IAPPLET_HandleEvent()) when PPP is
     in the dormant state.

Definition:
  #define NMASK_ASLEEP      0x0008

Members:
    None

Comments:
    None

Version:
   Introduced BREW Client 3.1

See Also:
    INotifier, ISHELL_RegisterNotify()

=======================================================================

    ISocketCloser

Description:
    This structure contains function pointer and user callback data for a callback.
It is used with ISocket_IOCtl. An ISocketCloser callback is fired (by AEENet) when
some other part of the system needs a socket, but none are currently available.
It is polite to specify this callback if you plan to hold sockets open for an arbitrarily
long time.

Definition:
    typedef struct ISocketCloser {
       PFNSOCKETCLOSE pfn;
       void * pv;
    } ISocketCloser

Members:
    pfn :  The function to call to request that the owner close it.
    pv :  User callback data to be passed to pfn as the first parameter.

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    ISOCKET_IOCtl()
    PFNSOCKETCLOSE

=======================================================================

    ISocketDbgMark

Description:
    Holder for file and line number when marking an ISocket with debug information.

Definition:
    typedef struct ISocketDbgMark {
    const char *cpszFile;
    int nLine;
    } ISocketDbgMark

Members:
    cpszFile :  Set to __FILE__, but could be any static char
    nLine :  Line number

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ISOCKET_IOCtl()
    ISOCKET_DbgMark()

=======================================================================

    ISocketQI

Description:
    To accomplish QueryInterface without modifying ISocket as an abstract class,
we define this structure, which packs up the arguments normally passed to QueryInterface.

Definition:
    typedef struct ISocketQI {
    AEECLSID clsId;
    void *pif;
    } ISocketQI

Members:
   clsId :  [in] : Set by caller to the desired interface ID.
   pif :  [out] :  Set by ISocket to the requested interface, if available, set
                   to 0 if unavailable, or call is unsupported

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    ISOCKET_IOCtl(),
    IQueryInterface

=============================================================================

NetMgrEvent

Description:
   This data type is an enumeration of various network and socket events. These
   events can be received when a change in network or socket status occurs and 
   you have registered for a callback with INETMGR_OnEvent().

Definition:
   enum {
      NE_NONE,
      NE_PPP,
      NE_SO_OPENING,
      NE_SO_CONNECTING,
      NE_SO_CONNECTED,
      NE_SO_CLOSING,
      NE_SO_CLOSED,
      NE_IP
   };
   #ifdef _WIN32
   typedef unsigned NetMgrEvent;
   #else
   typedef int8 NetMgrEvent;
   #endif

Members:
   NE_NONE:                   No network status.
   NE_PPP:                    The status of the network connection is changed.
   NE_SO_OPENING:             A socket has opened.
   NE_SO_CONNECTING:          A socket is connecting.
   NE_SO_CONNECTED:           A socket has been connected.
   NE_SO_CLOSING:             A socket is closing.
   NE_SO_CLOSED:              A socket has closed.
   NE_IP:                     The device has a new IP address.   
Version:
   Introduced BREW Client 2.0

See Also:
   PFNNETMGREVENT
   INETMGR_OnEvent()
   NetState

=======================================================================

    NetSocket

Description:
    NetSocket is an enumeration of the types of sockets that can be created with
INetMgr Interface. A NetSocket value is passed to INETMGR_OpenSocket().

Definition:
    typedef enum {
    AEE_SOCK_STREAM=0,
    AEE_SOCK_DGRAM, 
    AEE_SOCK_UNSPEC
    } NetSocket;

Members:
    AEE_SOCK_STREAM  :  TCP: streaming socket.
    AEE_SOCK_DGRAM  :  UDP: datagram socket .
    AEE_SOCK_UNSPEC :  Socket Type not specified. Introduced BREW Client 3.x

Comments:
   This data type should be used for ISocket only. For ISockPort, use 
   AEESockType.

Version:
   Introduced BREW Client 1.0

See Also:
    INETMGR_OpenSocket()

=======================================================================

    NetState

Description:
    NetState is an enumeration of the states of the device's PPP connection to the
Internet. A NetState value is returned by the INETMGR_NetStatus() call.

Definition:
    typedef enum {
    NET_INVALID_STATE,
    NET_PPP_OPENING,
    NET_PPP_OPEN,
    NET_PPP_CLOSING,
    NET_PPP_CLOSED,
    NET_PPP_SLEEPING,
    NET_PPP_ASLEEP,
    NET_PPP_WAKING,
    } NetState;

Members:
    NET_INVALID_STATE  :  Not an actual state; this value is not returned by INETMGR_NetStatus().
    NET_PPP_OPENING  :  The PPP connection is being established.
    NET_PPP_OPEN  :  The PPP connection is active.
    NET_PPP_CLOSING  :  The PPP connection is closing.
    NET_PPP_CLOSED  :  The PPP connection is inactive.
    NET_PPP_SLEEPING  :  The PPP connection is "up", but non-PPP related network
                         resources (e.g. CDMA traffic channel) are being released
    NET_PPP_ASLEEP  :  The PPP connection is "up", but non-PPP related network resources
                       have been released
    NET_PPP_WAKING  :  The PPP connection is "up", and non-PPP related network resources
                       are being re-acquired

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    INETMGR_NetStatus()

=============================================================================

AEEUMTSInfo

Description:
   This data type is used with INETMGR_GetUMTSInfo().   

Definition:
   #define UMTS_INFO_NAME_SIZE 32

   typedef struct AEEUMTSInfo
   {
      boolean bValid;
      AECHAR  szName[UMTS_INFO_NAME_SIZE+1];
   } AEEUMTSInfo;

Members:
   bValid: TRUE if the profile is valid, FALSE otherwise.
   szName: null terminated name of the profile.

Comments:
   None

Version:
   Introduced BREW Client 3.1.0

See Also:
   INETMGR_GetUMTSInfo()   

=======================================================================

    PFNCONNECTCB

Description:
    PFNCONNECTCB specifies the type of the callback function passed to ISOCKET_Connect().

Definition:
    typedef void (* PFNCONNECTCB)(void * pUser, int nError);

Members:
    pUser :  User data.
    nError :  Error code.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    ISOCKET_Connect()
    ISOCKET_IOCtl()
    IQueryInterface

=============================================================================

PFNNETMGREVENT

Description:
   This data type is the prototype for a Network event callback function. It is
   called when networking events occur on a socket or the network after you 
   register for events with the INETMGR_OnEvent() function.

Definition:
   typedef void (*PFNNETMGREVENT)(void* pUser, NetMgrEvent evt, uint32 dwData)

Members:
    pUser:  User-specific data.
    evt:    The Event that triggered this callback.
    dwData: Data passed by caller depending on the evt that caused the callback.

Comments:
   dwData have a different meaning depending on the value of evt. These are given
   below in the order evt: dwData.
~
   NE_PPP  :  NetState~
   NE_SO_OPENING  :  ISocket *~
   NE_SO_CONNECTING  :  ISocket *~
   NE_SO_CONNECTED  :  ISocket *~
   NE_SO_CLOSING  :  ISocket *~
   NE_SO_CLOSED  :  ISocket *~
   NE_IP  :  INAddr in network-byte order*~
*

   In the case of NE_SO_CLOSING and NE_SO_CLOSED, the ISocket pointer no longer
   refers to a valid ISocket and may not be dereferenced. It may be used as a 
   value for searching a list or other tasks.

Version:
   Introduced BREW Client 2.0

See Also:
   INETMGR_OnEvent()
   NetMgrEvent.

=============================================================================

    PFNSOCKETCLOSE

Description:
    PFNSOCKETCLOSE is the prototype for a ISocketCloser callback. It is called to
suggest that the owner of the ISocket close it, if possible, so that another client
of the ISocket API may proceed.

Definition:
    typedef void (* PFNSOCKETCLOSE)(void * pvCxt, ISocket * pSocket);

Members:
    pvCxt :  User data specified in the ISocketCloser
    pSocket :  Relevant socket (please Release this one, if possible)

Comments:
    None.

Version:
   Introduced BREW Client 1.1

See Also:
    ISOCKET_IOCtl()
    ISocketCloser

=======================================================================

=====================================================================*/

#endif    // AEENET_H


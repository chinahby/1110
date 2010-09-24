#ifndef AEEISOCKPORT_H
#define AEEISOCKPORT_H
/*=============================================================================

FILE:         AEEISockPort.h

SERVICES:     ISockPort (IPort based Socket)

DESCRIPTION:
   ISockPort provides a socket interface that operates on generic
   addresses, such that internet (AEE_AF_INET, AEE_AF_INET6) and
   local domain (AEE_AF_LOCAL) sockets can be accessed through the same interface.

   Currently local domain (AEE_AF_LOCAL) sockets are not supported.

===============================================================================
   Copyright (c) 2006-2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEIPort.h"
#include "AEESocketTypes.h"

//
// for SOCKPORT_IOCTL_SELECT_NETWORK_EX
//
typedef struct AEESockPortSelectNetworkEx
{
   int nNetwork;
   int16 nFamily;
} AEESockPortSelectNetworkEx;

//
// for SOCKPORT_IOCTL_RECVMSG
//
typedef struct AEESockPortRecvMsg
{
   AEESockMsgHdr *pmh;
   uint32 uFlags;
} AEESockPortRecvMsg;

#define SOCKPORT_IOCTL_SDBGMARK          1 // dwVal = pointer to ISockPortDbgMark
#define SOCKPORT_IOCTL_GDBGMARK          2 // dwVal = pointer to ISockPortDbgMark
#define SOCKPORT_IOCTL_SCLOSER           3 // dwVal = pointer to AEECallback
#define SOCKPORT_IOCTL_GCLOSER           4 // dwVal = pointer to pointer to AEECallback
#define SOCKPORT_IOCTL_SCONNTIMEOUT      5 // dwVal = connect timeout in milliseconds
#define SOCKPORT_IOCTL_GCONNTIMEOUT      6 // dwVal = pointer to dword
#define SOCKPORT_IOCTL_NATDESC           7 // dwVal = pointer to dword
#define SOCKPORT_IOCTL_SELECT_NETWORK_EX 8 // dwVal = pointer to AEESockPortSelectNetworkEx
#define SOCKPORT_IOCTL_ISREALIZED        9 // dwVal = pointer to boolean
#define SOCKPORT_IOCTL_REALIZE          10 // dwVal will not be used
// New IOCTLs must use unique IDs to avoid conflicts between several BREW versions under development
#define SOCKPORT_IOCTL_RECVMSG  0x0105f09e // dwVal = pointer to AEESockPortRecvMsg 
#define SOCKPORT_IOCTL_GET_SDB_ACK_INFO  0x0105f09d // dwVal = pointer to AEESDBAckInfo

//*************************************************************
// ISockPort Interface
//*************************************************************

#define AEEIID_ISockPort       0x01039ba8

#if defined(AEEINTERFACE_CPLUSPLUS)

struct ISockPort : public IPort 
{
   virtual int CDECL OpenEx (uint16 wFamily, AEESockType nType, int nProtocol) = 0;
   virtual int CDECL SelectNetwork (int nNetwork) = 0;
   virtual int CDECL Bind (const void* pAddr) = 0;
   virtual int CDECL Listen (int nBacklog) = 0;
   virtual int CDECL Accept (ISockPort** ppISockPort) = 0;
   virtual int CDECL Connect (const void* pAddr) = 0;
   virtual int32 CDECL WriteV (const SockIOBlock iov[], uint16 wIovCount) = 0;
   virtual int32 CDECL ReadV (SockIOBlock iov[], uint16 wIovCount) = 0;
   virtual int32 CDECL SendTo (const char* pcBuf, int32 cbBuf, uint32 dwFlags, const void* pAddr) = 0;
   virtual int32 CDECL RecvFrom (char* pcBuf, int32 cbBuf, uint32 dwFlags, void* pAddr, int* pnSize) = 0;
   virtual int CDECL GetSockName (void* pAddr, int* pnSize) = 0;
   virtual int CDECL GetPeerName (void* pAddr, int* pnSize) = 0;
   virtual int CDECL GetOpt (int nLevel, int nOptName, void *pOptVal, int *pnOptSize) = 0;
   virtual int CDECL SetOpt (int nLevel, int nOptName, const void *pOptVal, int nOptSize) = 0;
   virtual int CDECL Shutdown (int nHow) = 0;
   void CDECL ReadableEx(AEECallback* pcb,PFNNOTIFY pfn, void* pv)
   {
      CALLBACK_Cancel(pcb); 
      CALLBACK_Init(pcb,(pfn),(pv)); 
      Readable(pcb);
   }

   void CDECL WriteableEx(AEECallback*pcb, PFNNOTIFY pfn, void*pv)
   {   
      CALLBACK_Cancel((pcb)); 
      CALLBACK_Init((pcb),(pfn),(pv)); 
      Writeable(pcb);
   }
   void CDECL DbgMark(void)                      
   { 
      ISockPortDbgMark m;
      m.cpszFile = __FILE__;
      m.nLine    = __LINE__;
      IOCtl(SOCKPORT_IOCTL_SDBGMARK,(uint32)&m);   
   }
   int CDECL SelectNetworkEx(int nNetwork, int16 nFamily)
   {
      AEESockPortSelectNetworkEx info;

      info.nNetwork = nNetwork;
      info.nFamily = nFamily;

      return IOCtl(SOCKPORT_IOCTL_SELECT_NETWORK_EX, (uint32)&info);
   }

   int CDECL IsRealized(boolean* pbIsRealized)
   {
      return IOCtl(SOCKPORT_IOCTL_ISREALIZED, (uint32)pbIsRealized);
   }
   int CDECL Realize(void)
   {
      return IOCtl(SOCKPORT_IOCTL_REALIZE, (uint32)0);
   }
   int CDECL RecvMsg(AEEMsgHdr *pmh, uint32 uFlags)
   {
      AEESockPortRecvMsg info;

      info.pmh = pmh;
      info.uFlags = uFlags;

      return IOCtl(SOCKPORT_IOCTL_RECVMSG, (uint32)&info);
   }
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

// Use to inherit from an ISockPort
#define INHERIT_ISockPort(iname) \
   INHERIT_IPort(iname); \
   int   (*OpenEx)(iname* po, uint16 wFamily, AEESockType nType, int nProtocol); \
   int   (*SelectNetwork)(iname* po, int nNetwork); \
   int   (*Bind)(iname* po, const void* pAddr); \
   int   (*Listen)(iname* po, int nBacklog); \
   int   (*Accept)(iname* po, iname** pp##iname); \
   int   (*Connect)(iname* po, const void* pAddr); \
   int32 (*WriteV)(iname* po, const SockIOBlock iov[], uint16 wIovCount); \
   int32 (*ReadV)(iname* po, SockIOBlock iov[], uint16 wIovCount); \
   int32 (*SendTo)(iname* po, const char* pcBuf, int32 cbBuf, uint32 dwFlags, const void* pAddr); \
   int32 (*RecvFrom)(iname* po, char* pcBuf, int32 cbBuf, uint32 dwFlags, void* pAddr, int* pnSize); \
   int   (*GetSockName)(iname* po, void* pAddr, int* pnSize); \
   int   (*GetPeerName)(iname* po, void* pAddr, int* pnSize); \
   int   (*GetOpt)(iname *po, int nLevel, int nOptName, void *pOptVal, int *pnOptSize); \
   int   (*SetOpt)(iname *po, int nLevel, int nOptName, const void *pOptVal, int nOptSize); \
   int   (*Shutdown)(iname* po, int nHow)

// declare the actual ISockPort interface
AEEINTERFACE_DEFINE(ISockPort);

//////////////////////////
// ISockPort methods
/////////////////////////

static __inline uint32 ISockPort_AddRef(ISockPort *me)
{
   return AEEGETPVTBL(me,ISockPort)->AddRef(me);
}

static __inline uint32 ISockPort_Release(ISockPort *me)
{
   return AEEGETPVTBL(me,ISockPort)->Release(me);
}

static __inline int ISockPort_QueryInterface(ISockPort* me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,ISockPort)->QueryInterface(me, cls, ppo);
}

static __inline int32 ISockPort_Read(ISockPort* me, char* pcBuf, int32 cbBuf)
{
   return AEEGETPVTBL(me,ISockPort)->Read(me, pcBuf, cbBuf);
}

static __inline void ISockPort_Readable(ISockPort* me, AEECallback* pcb)
{
   AEEGETPVTBL(me,ISockPort)->Readable(me, pcb);
}

static __inline int ISockPort_GetLastError(ISockPort* me)
{
   return AEEGETPVTBL(me,ISockPort)->GetLastError(me);
}

static __inline int32 ISockPort_Write(ISockPort* me, const char* pcBuf, int32 cbBuf)
{
   return AEEGETPVTBL(me,ISockPort)->Write(me, pcBuf, cbBuf);
}

static __inline void ISockPort_Writeable(ISockPort* me, AEECallback* pcb)
{
   AEEGETPVTBL(me,ISockPort)->Writeable(me, pcb);
}

static __inline int ISockPort_IOCtl(ISockPort* me, int nOption, uint32 dwVal)
{
   return AEEGETPVTBL(me,ISockPort)->IOCtl(me, nOption, dwVal);
}

static __inline int ISockPort_Open(ISockPort* me, const char* sz)
{
   return AEEGETPVTBL(me,ISockPort)->Open(me, sz);
}

static __inline int ISockPort_Close(ISockPort* me)
{
   return AEEGETPVTBL(me,ISockPort)->Close(me);
}

static __inline int ISockPort_OpenEx(ISockPort* me, uint16 wFamily, AEESockType nType, int nProtocol)
{
   return AEEGETPVTBL(me,ISockPort)->OpenEx(me, wFamily, nType, nProtocol);
}

static __inline int ISockPort_SelectNetwork(ISockPort* me, int nNetwork)
{
   return AEEGETPVTBL(me,ISockPort)->SelectNetwork(me, nNetwork);
}

static __inline int ISockPort_Bind(ISockPort* me, const void* pAddr)
{
   return AEEGETPVTBL(me,ISockPort)->Bind(me, pAddr);
}

static __inline int ISockPort_Listen(ISockPort* me, int nMaxBacklog)
{
   return AEEGETPVTBL(me,ISockPort)->Listen(me, nMaxBacklog);
}

static __inline int ISockPort_Accept(ISockPort* me, ISockPort** ppSockPort)
{
   return AEEGETPVTBL(me,ISockPort)->Accept(me, ppSockPort);
}

static __inline int ISockPort_Connect(ISockPort* me, const void* pAddr)
{
   return AEEGETPVTBL(me,ISockPort)->Connect(me, pAddr);
}

static __inline int32 ISockPort_WriteV(ISockPort* me, const SockIOBlock iov[], uint16 wIovCount)
{
   return AEEGETPVTBL(me,ISockPort)->WriteV(me, iov, wIovCount);
}

static __inline int32 ISockPort_ReadV(ISockPort* me, SockIOBlock iov[], uint16 wIovCount)
{
   return AEEGETPVTBL(me,ISockPort)->ReadV(me, iov, wIovCount);
}

static __inline int32 ISockPort_SendTo(ISockPort* me, const char* pcBuf, int32 cbBuf,
                                       uint32 dwFlags, const void* pAddr)
{
   return AEEGETPVTBL(me,ISockPort)->SendTo(me, pcBuf, cbBuf, dwFlags, pAddr);
}

static __inline int32 ISockPort_RecvFrom(ISockPort* me, char* pcBuf, int32 cbBuf,
                                         uint32 dwFlags, void* pAddr, int* pnSize)
{
   return AEEGETPVTBL(me,ISockPort)->RecvFrom(me, pcBuf, cbBuf, dwFlags, pAddr, pnSize);
}

static __inline int ISockPort_GetSockName(ISockPort* me, void* pAddr, int* pnSize)
{
   return AEEGETPVTBL(me,ISockPort)->GetSockName(me, pAddr, pnSize);
}

static __inline int ISockPort_GetPeerName(ISockPort* me, void* pAddr, int* pnSize)
{
   return AEEGETPVTBL(me,ISockPort)->GetPeerName(me, pAddr, pnSize);
}

static __inline int ISockPort_GetOpt(ISockPort* me, int nLevel, int nOptName, void* pOptVal, int* pnOptSize)
{
   return AEEGETPVTBL(me,ISockPort)->GetOpt(me, nLevel, nOptName, pOptVal, pnOptSize);
}

static __inline int ISockPort_SetOpt(ISockPort* me, int nLevel, int nOptName, const void* pOptVal, int nOptSize)
{
   return AEEGETPVTBL(me,ISockPort)->SetOpt(me, nLevel, nOptName, pOptVal, nOptSize);
}

static __inline int ISockPort_Shutdown(ISockPort* me, int nHow)
{
   return AEEGETPVTBL(me,ISockPort)->Shutdown(me, nHow);
}

// Pseudo-methods
#define ISockPort_ReadableEx(p,pcb,pfn,pv)    CALLBACK_Cancel((pcb)); \
                                              CALLBACK_Init((pcb),(pfn),(pv)); \
                                              ISockPort_Readable((p), (pcb))

#define ISockPort_WriteableEx(p,pcb,pfn,pv)   CALLBACK_Cancel((pcb)); \
                                              CALLBACK_Init((pcb),(pfn),(pv)); \
                                              ISockPort_Writeable((p), (pcb))

#define ISockPort_DbgMark(p)                      do {\
   ISockPortDbgMark m;\
   m.cpszFile = __FILE__;\
   m.nLine    = __LINE__;\
   ISockPort_IOCtl((p),SOCKPORT_IOCTL_SDBGMARK,(uint32)&m);\
} while(0);

static __inline int ISockPort_SelectNetworkEx(ISockPort *po, int nNetwork, int16 nFamily)
{
   AEESockPortSelectNetworkEx info;

   info.nNetwork = nNetwork;
   info.nFamily = nFamily;

   return ISockPort_IOCtl(po, SOCKPORT_IOCTL_SELECT_NETWORK_EX, (uint32)&info);
}

static __inline int ISockPort_GetSDBAckInfo(ISockPort *po, AEESDBAckInfo* pSDBAckInfo)
{
   return ISockPort_IOCtl(po, SOCKPORT_IOCTL_GET_SDB_ACK_INFO, (uint32)pSDBAckInfo);
}

static __inline int ISockPort_IsRealized(ISockPort *po, boolean* pbIsRealized)
{
   return ISockPort_IOCtl(po, SOCKPORT_IOCTL_ISREALIZED, (uint32)pbIsRealized);
}

static __inline int ISockPort_Realize(ISockPort *po)
{
   return ISockPort_IOCtl(po, SOCKPORT_IOCTL_REALIZE, (uint32)0);
}

static __inline int ISockPort_RecvMsg(ISockPort *po, AEESockMsgHdr *pmh, uint32 uFlags)
{
   AEESockPortRecvMsg info;

   info.pmh = pmh;
   info.uFlags = uFlags;

   return ISockPort_IOCtl(po, SOCKPORT_IOCTL_RECVMSG, (uint32)&info);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name: ISockPort

This interface provides standard socket networking services, both stream and
datagram. It provides methods to open and close, connect, transmit and receive
data and more, over TCP and UDP sockets.

NOTE: Your application must have a privilege level of Network or All to be able
to invoke the functions in this interface.

ISockPort supports multiple address families, which is specified once during
ISockPort_OpenEx() and later by using the appropriate address structure.
Currently, AEE_AF_INET and AEE_AF_INET6 address families are supported. IPv6
mapped IPv4 addresses are not supported.

ISockPort_OpenEx() is used to open a socket. When calling this function the
following is specified - address family (e.g. AEE_AF_INET), socket type (e.g
AEE_SOCKPORT_STREAM) and protocol (e.g. AEE_IPPROTO_TCP).

ISockPort_Connect() is called after opening a TCP socket.
If the network subsystem (physical layer, RLP and PPP, etc.) of the device is
not active, this function first establishes the necessary lower-layer protocol
connections. For TCP sockets, ISockPort_Connect() then sets up a TCP connection
to the specified IP address and port number.

After the TCP socket has been connected, read and write operations may be used to
exchange data over it. All data transfer operations are non-blocking and asynchronous.
Callback objects are used to notify the caller of the availability of socket
for read or write operations. The caller can use ISockPort_Readable() to provide
a callback object that will be resumed when there is data available to read.
Similarly, ISockPort_Writeable() registers a callback object that gets resumed
when the socket is available for writing. Registered callback objects are also
resumed whenever BREW detects any error conditions on the socket that require
the application using this socket to take action.

To cancel a pending callback operation, use CALLBACK_Cancel() on a callback
object that was previously registered with ISockPort_Readable() or
ISockPort_Writeable(). This will prevent the application from receiving
notification when there is a change in the status of the socket it is waiting
on.

ISockPort_Read() and ISockPort_Write() are used for reading data from and
writing data to TCP sockets. If the read or write do not make progress,
ISockPort_Read() and ISockPort_Write() returns an indication that blocking has
occurred. The caller can then use ISockPort_Readable() or ISockPort_Writeable()
to arrange to be notified when blocking is no longer present. If the number of
bytes actually read or written is less than the number requested, repeat calls
to ISockPort_Read() or ISockPort_Write() to complete the data transfer.

ISockPort_ReadV() and ISockPort_WriteV() are used to receive and send data on
TCP sockets when the application uses multiple, non-contiguous buffers for
reading and writing. In place of the single buffer pointer supplied as a parameter
to ISockPort_Read() and ISockPort_Write(), ISockPort_ReadV() and ISockPort_WriteV()
each take an array of buffer descriptors as input, with each array element specifying
the length in bytes and starting address of a buffer. These functions attempt to
transfer an amount of data equal to the accumulated length of all buffers, starting with
the first buffer in the array.

ISockPort_RecvFrom() and ISockPort_SendTo() are used to exchange data over UDP
sockets. Both these functions allow data to be sent to and received from
multiple IP addresses and port numbers. As with TCP sockets, the application may
call ISockPort_Readable() or ISockPort_Writeable() to designate a callback object
if the read or write operation does not make progress immediately.

ISockPort_Bind() associates a local port number with a socket (if an application
does not call this function before ISockPort_Connect(), or ISockPort_SendTo()
a default value is used for the local port number).

ISockPort_Listen() and ISockPort_Accept() are used to implement a server that
can accept incoming TCP connections.

ISockPort_GetLastError() returns the error code for the last error detected
by a function in the ISockPort Interface. In cases where such a function returns
something other than success, ISockPort_GetLastError() provides more detailed
information about why the function failed to perform its task.

ISockPort_GetPeerName() returns the IP address and port number of the entity
with which data was most recently exchanged on the TCP socket.

ISockPort_GetSockName() returns the IP address and port number of the local
socket.

ISockPort_GetOpt() and ISockPort_SetOpt() retrieve and specify optional
parameters for a socket and/or the underlying network stack. See AEESockOpt for
the supported options.

Receiving multicast datagrams is enabled by setting the AEE_IP_ADD_MEMBERSHIP socket
option.  Note that multicast is only supported on a small subset of devices and
carrier networks; please verify support before making use of this feature.

ISockPort_Shutdown() or ISockPort_Close() are used to gracefully close a TCP
connection.

A call to ISockPort_Release() also closes any socket connection and releases
the ISockPort Interface and frees the associated resources.

===H2>
To use a TCP client socket services
===/H2>

1.   Call IEnv_CreateInstance() to create an instance of the ISockPort
Interface.

2.   Call ISockPort_OpenEx() to specify address family, type and protocol for
the socket.

3.   Call ISockPort_Connect() to establish a TCP socket connection with the network
entity with which the application communicates.

4.   Use the functions in the ISockPort Interface to operate on the socket.

===H2>
To use a TCP server socket services
===/H2>

1.   Call IEnv_CreateInstance() to create an instance of the ISockPort
Interface.

2.   Call ISockPort_OpenEx() to specify address family, type and protocol for
the socket.

3.   Call ISockPort_Bind() to specify the socket's local port number.

4.   Call ISockPort_Listen() to set a queue of pending connections for the socket.

5.   Call ISockPort_Accept() to accept an incoming TCP connections.

6.   Use the functions in the ISockPort Interface to operate on the new ISockPort
returned from ISockPort_Accept().

===H2>
To read or write data over the socket
===/H2>

1.   Call the relevant (TCP/UDP, single/multiple I/O buffers) read or write functions
described above.

2.   If the function returns a blocking indication (AEE_NET_EWOULDBLOCK), call
ISockPort_Readable() or ISockPort_Writeable() to attempt the operation at a later time.
Note also the existence of the helper macros - ISockPort_ReadableEx() and
ISockPort_WriteableEx().

3.   If less than the requested number of bytes were transferred, call the read
or write function again to effect the transfer of the remaining data.

4.   Repeat steps 1-3, until all of your data has been transferred.

===H2>
To receive ICMP error information
===/H2>

ISockPort provides the ability to receive ICMP error information. The client can be
notified of ICMP errors that are not handled at a lower level. The specific ICMP
errors that can be returned are documented in the functions that return them.

There are two modes of operation:
   1. Simple mode - only the latest error code can be received.
      Relevant socket options: AEE_SO_ERROR_ENABLE, AEE_SO_ERROR.
      Relevant functions: ISockPort_Connect(), ISockPort_SendTo(),
      ISockPort_RecvFrom() and ISockPort_RecvMsg().
   2. Extended mode - full error information can be received, errors are
      queued.
      Relevant socket option: AEE_IP_RECVERR.
      Relevant function: ISockPort_RecvMsg().

Simple mode:
   1. To receive notifications about ICMP errors, the AEE_SO_ERROR_ENABLE
      socket option must be set.
   2. When an ICMP error arrives, clients that registered callbacks
      using ISockPort_Readable() or ISockPort_Writeable() are notified.
   3. When there is a pending ICMP error on the socket:
      a. ISockPort_SendTo() will return IPORT_ERROR, and the error code can be
         retrieved using ISockPort_GetLastError().
      b. If there is no data pending on the socket, ISockPort_RecvFrom() and
         ISockPort_RecvMsg() will return IPORT_ERROR, and the error code can be
         retrieved using ISockPort_GetLastError().
      c. The ICMP error can be retrieved by calling ISockPort_GetOpt() with the
         AEE_SO_ERROR socket option.
      d. ISockPort_Connect() can also return IPORT_ERROR that is caused by an
         ICMP connection error. Here too the error code can be retrieved using
         ISockPort_GetLastError().
   4. A pending ICMP error is cleared from the socket once the client was
      notified about it, even if ISockPort_GetLastError() wasn't called.
      For example: if the application called ISockPort_SendTo() and it returned
      IPORT_ERROR that is due to a pending ICMP error, a call to ISockPort_GetOpt()
      with AEE_SO_ERROR will not return the ICMP error and a subsequent call to
      ISockPort_SendTo() will not fail due to the same ICMP error (it might fail
      because of other reasons).

Extended mode:
   1. To enable queuing of extended ICMP error information, the AEE_IP_RECVERR
      socket option must be set. This option is only available for UDP sockets.
   2. When a new ICMP error arrives, applications that registered callbacks
      using ISockPort_Readable() are notified.
   3. When there are pending ICMP errors, calling ISockPort_RecvMsg() with no
      flags will notify the caller about the pending ICMP errors by setting the
      SOCKPORT_FLAG_RECVMSG_ERRQUEUE flag in the output flags field.
      The notification flag doesn't interfere with the regular return value of
      ISockPort_RecvMsg(). For example, if the user has both a pending datagram
      and a pending ICMP error, the datagram will be returned together with the
      SOCKPORT_FLAG_RECVMSG_ERRQUEUE flag.
   4. The application can retrieve a queued ICMP error by calling ISockPort_RecvMsg()
      with the SOCKPORT_FLAG_RECVMSG_ERRQUEUE input flag. Such a call will remove and
      return the oldest error information in the queue.
      The SOCKPORT_FLAG_RECVMSG_ERRQUEUE output flag will be set in case there are
      still more errors in the queue.

It is possible to use both socket options (AEE_SO_ERROR_ENABLE & AEE_IP_RECVERR)
simultaneously. In such a case, both modes described above will be combined: the last
received error code will be returned in the cases described under simple mode, and 
extended error information will be queued as described under extended mode.
When both socket options are used together, every error return should be immediately
followed by retrieving the ICMP information with no intervening operations. Following
this guideline will ensure that the first error information in the queue refers to the
latest error code that was returned to the client.
The use of both socket options is discouraged, as it does make it harder for the client
to keep the queue and the latest error in sync. When using this feature, care should be
taken to implement it correctly to enable reliable ICMP error tracking.


ISockPort provides several additional interfaces; please see the corresponding
header file for complete documentation:

===pre>
IQUERYINTERFACE              AEEIQI.h
   ISockPort_AddRef()
   ISockPort_Release()
   ISockPort_QueryInterface()
ISOURCE                      AEEISource.h
   ISockPort_Read()
   ISockPort_Readable()
IPORT                        AEEIPort.h
   ISockPort_GetLastError()
   ISockPort_Write()
   ISockPort_Writeable()
   ISockPort_IOCtl()
   ISockPort_Close()
   ISockPort_Open()
===/pre>

The following header file is required:~
   BREW 4.0 - AEEISockPort.h
   BREW 3.1 - AEESockPort.h

=======================================================================

ISockPort_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   ISockPort_Release()

=======================================================================

ISockPort_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   ISockPort_AddRef()

=======================================================================

ISockPort_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

=======================================================================

ISockPort_Read()

Description:
   This function is inherited from ISource_Read().

See Also:
   None

=======================================================================

ISockPort_Readable()

Description:
   This function is inherited from ISource_Readable().

See Also:
   None

=======================================================================

ISockPort_GetLastError()

Description:
   This function is inherited from IPORT_GetLastError().

See Also:
   None

=======================================================================

ISockPort_Write()

Description:
   This function is inherited from IPORT_Write().

See Also:
   None

=======================================================================

ISockPort_Writeable()

Description:
   This function is inherited from IPORT_Writeable().

See Also:
   None

=======================================================================

ISockPort_Close()

Description:
   This function is inherited from IPORT_Close().

See Also:
   None

=======================================================================

ISockPort_Open()

Description:
   This function is inherited from IPORT_Open().

See Also:
   None

=======================================================================

ISockPort_IOCtl()
   This function is inherited from IPORT_IOCTL().

Description:
   This function allows some control over the behaviour of an ISockPort,
   e.g. setting/getting configuration.

   It's form is analogous to SYSV's ioctl, with encoded arguments that
   should be specified with the *_IOCTL_* macros listed here. It's
   intended use is for special, out-of-object-signature behaviour. For
   that reason, it's super-generic.

Prototype:
   int ISockPort_IOCtl(ISockPort* po, int nOption, uint32 dwVal);

Parameters:
   po:      pointer to the ISockPort interface
   nOption: the option id
   dwVal:   option value

Return Value:
   AEE_SUCCESS         :  the option id is known and the operation succeeded

   AEE_NET_EBADF   :  socket is not open
   AEE_EUNSUPPORTED    :  the option id was unrecognized

   Other error codes are also possible.

Comments:
   All address families support the following option ids:

      SOCKPORT_IOCTL_SDBGMARK - dwVal set to a pointer to an ISockPortDbgMark,
      specifies how to mark the ISockPort.

      SOCKPORT_IOCTL_GDBGMARK - dwVal set to a pointer to an ISockPortDbgMark,
      which will be set to the current ISockPort's mark (if any)

   Additionally, AEE_AF_INET and AEE_AF_INET6 support the following option ids:

      SOCKPORT_IOCTL_SCLOSER - dwVal set to a pointer to an AEECallback
         struct, which specifies the ISockPort's new closer callback

      SOCKPORT_IOCTL_GCLOSER - dwVal set to a pointer to a pointer to an
         AEECallback struct (AEECallback**), which will be set to
         ISockPort's current closer callback

      SOCKPORT_IOCTL_SCONNTIMEOUT - dwVal set to time in milliseconds (zero
         for default) to wait for connect to succeed

      SOCKPORT_IOCTL_GCONNTIMEOUT - dwVal set to a pointer to an uint32,
         which will be set to the current time in milliseconds to
         wait for connect to succeed, Values larger then 0x80000000 
         indicate unlimited timeout

Version:
   Introduced BREW Client 3.1.3

See Also:
   Socket Protocols

=============================================================================

ISockPort_SelectNetwork()

Description:
   Selects a specific data network.

Prototype:
   int ISockPort_SelectNetwork(ISockPort* po, int nNetwork)

Parameters:
   po:       pointer to the ISockPort interface
   nNetwork: data network (AEE_NETWORK_*)

Return Value:
   AEE_SUCCESS           :  network has been successfully selected

   AEE_NET_EINVAL    :  network is not valid
   AEE_NET_EOPNOTSUPP:  network has already been selected

   Other error codes are also possible.

Comments:
   Most applications will not need to explicitly select the network.

   When an ISockPort instance is created, no network is selected.  If an
   ISockPort method that requires a network is called before
   ISockPort_SelectNetwork(), AEE_NETWORK_DEFAULT and the socket's family (or
   AEE_AF_INET if it's unspec) will be implicitly selected, and the decision of
   which network to actually use is deferred to lower layers. Decision will be
   made upon connection creation, depending on the device and/or service provider.

   Once a network has been selected, either explicitly via
   ISockPort_SelectNetwork(), or implicitly as described above, the network may
   not be changed.  To use a different network, a new ISockPort instance is
   required.

Version:
   Introduced BREW Client 3.1.3

See Also:
   Data networks

=============================================================================

ISockPort_SelectNetworkEx()

Description:
   This function selects a specific data network.

   This function deprecates ISockPort_SelectNetwork().

Prototype:
   int ISockPort_SelectNetworkEx(ISockPort *me, int nNetwork, int16 nFamily)

Parameters:
   me: the interface pointer
   nNetwork: data network type (AEE_NETWORK_*)
   nFamily: network address family (AEE_AF_*). If ISockPort_OpenEx() was
            already called for this socket, nFamily should match the address
            family that was requested for the socket.

Return Value:
   AEE_SUCCESS: the data network was selected
   AEE_NET_EINVAL: the network is not valid
   AEE_NET_EOPNOTSUPP: the network has already been selected.
   AEE_EFAILED: other error.

   Other error codes are also possible.

Comments:
   Most applications will not need to explicitly select the network.

   When an ISockPort instance is created, no network is selected. If an
   ISockPort method that requires a network is called before
   ISockPort_SelectNetworkEx(), AEE_NETWORK_DEFAULT and the socket's family (or
   AEE_AF_INET if it's unspec) will be implicitly selected, and the decision of
   which network to actually use is deferred to lower layers. Decision will be
   made upon connection creation, depending on the device and/or service provider.

   Once a network has been selected, either explicitly via
   ISockPort_SelectNetworkEx(), or implicitly as described above, the network
   may not be changed. To use a different network, a new ISockPort instance is
   required.


Version:
   Introduced BREW Client 3.1.3

See Also:
   ISockPort_OpenEx()
   Data networks
   Socket Address Family

===========================================================================

ISOCKPORT_GetSDBAckInfo()

Description:
   Get the information regarding the last SDB acknowledgment that arrived
   for this socket. The acknowledgment may arrive from the base station, or be
   locally produced by the mobile's lower layers.
   An application may use this function only for an open UDP socket.
   Note that receiving an SDB-Ack is only applicable to CDMA networks.

Prototype:
   int ISOCKPORT_GetSDBAckInfo(ISockPort *po, AEESDBAckInfo* pSDBAckInfo)

Parameters:
   po:          [in]  Pointer to the ISockPort interface.
   pSDBAckInfo: [out] User-allocated struct for the retrieved information.

Return Value:
   SUCCESS            :  the information was successfully retrieved.
   AEE_NET_EBADF      :  socket is not open.
   AEE_NET_EOPNOTSUPP :  not a datagram socket.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5 SP01.

See Also: 
   AEE_SO_SDB_ACK_CB

=============================================================================

ISockPort_IsRealized()

Description:
   checks if the socket is realized (i.e. bound).

Prototype:
   int ISockPort_IsRealized(ISockPort *po, boolean* pbIsRealized)

Parameters:
   po: pointer to the ISockPort interface
   pbIsRealized: [out] pointer to a boolean value, set to TRUE or FALSE
                 according to the socket's status.

Return value:
   AEE_SUCCESS: pbIsRealized was successfully updated.

   Other error codes are also possible.

Comments:
   None

Version:
   Introduced BREW Client 4.0

See Also:
   ISockPort_Realize()

=============================================================================

ISockPort_Realize()

Description:
    Acquires an IP address and ensures that the underlying network layer is ready
for communication. This may involve the establishment of an Internet connection
if such a connection has not already been established.
Finally, any previously-requested ISockPort_Bind() operation will be attempted if
necessary. As a result, errors relating to network startup or ISockPort_Bind() could
be reported here, and AEE_NET_WOULDBLOCK return values are commonly encountered.

Prototype:
   int ISockPort_Realize(ISockPort * po);

Parameters:
   po:   Pointer to the ISockPort interface to realize

Return Value:
   AEE_SUCCESS:    The socket has been successfully realized.
   AEE_NET_WOULDBLOCK: Realize in progress; use either ISockPort_Readable() or ISockPort_Writeable()
                       to be notified when to try again.
   AEE_NET_ENETNONET:  network subsystem unavailable for some unknown reason

   Other error codes are also possible.

Comments:
   None

Version:
   Introduced BREW Client 4.0

See Also:
    ISockPort_Readable()
    ISockPort_Writeable()

=============================================================================

ISockPort_OpenEx()

Description:
   Opens a SockPort for use with a specific address family, type and protocol

Prototype:
   int ISockPort_OpenEx(ISockPort* po, uint16 wFamily, AEESockType nType, int nProtocol)

Parameters:
   po:      pointer to the ISockPort interface
   wFamily: address family (AEE_AF_*). If ISockPort_SelectNetwork() or
            ISockPort_SelectNetworkEx() was already called for this socket,
            wFamily should match the address family that was requested for the
            network.
   nType:   socket type
   nProtocol: socket protocol (AEE_IPPROTO_*) or 0 to select the system's default
              for the given wFamily and nType combination

Return value:
   AEE_SUCCESS:          socket has been successfully opened with the
                             address family, type and protocol requested

   AEE_NET_EBADF          :  socket is already open
   AEE_NET_ESOCKNOSUPPORT :  invalid type
   AEE_NET_EPROTONOSUPPORT:  invalid protocol
   AEE_NET_EAFNOSUPPORT   :  invalid family
   AEE_NET_EPROTOTYPE     :  wrong protocol for socket type
   AEE_NET_ENOMEM         :  out of memory
   AEE_NET_EINVAL         :  invalid family for this socket

   Other error codes are also possible.

Comments:
   None

Version:
   Introduced BREW Client 3.1.3

See Also:
   Socket Protocols
   AEESockType

=======================================================================

ISockPort_Bind()

Description:
   This function associates a local address and port with the socket.  The bind
   function is used on an unconnected socket.

   When a socket is created, it exists in a name space (address family), but it
   has no name assigned to it.  There are three distinct name spaces available:
   AEE_AF_LOCAL, AEE_AF_INET and AEE_AF_INET6.


   - AEE_AF_LOCAL

~     Binding of local domain sockets will never result in a return value of
      IPORT_WAIT, as there is no network connection to be established.

      Currently AEE_AF_LOCAL is not supported.


   - AEE_AF_INET/AEE_AF_INET6

~     Passing the IPv4 or IPv6 ANY (zero) address explicitly requests that
      the socket be assigned to any local address.
      Passing the IPv4 or IPv6 BREW LOOPBACK address will allow local
      connections that do not leave the host.
      Passing a multicast group address can be used for receiving
      multicast traffic destined to that group, if the selected network
      is a multicast network, such as MediaFLO or CDMA BCMCS. On other networks,
      multicast is not supported yet.

      Note that the ability to bind to a specific local IP address is not
      presently supported, and the above mentioned options are the only
      valid values for the address.  In the case of ANY, the
      local IP address is assigned automatically by the sockets library.

      The IPv4 ANY address is defined by AEE_INADDR_ANY.
      The IPv6 ANY address can be generated with AEE_IN6ADDR_SET_ANY()

      Passing zero for the port explicitly requests that the socket be assigned
      any available port.

      Also note the possibility of an IPORT_WAIT result.  This will
      normally occur only in cases where a network connection (e.g. a PPP link)
      must be established.  In that event, Bind() will return IPORT_WAIT,
      and can be called again to obtain the final result (error or success).  For
      notification of when to call Bind() again, the ISockPort_Writeable() call can
      be used.  (Note that just as with Write(), a Writeable() callback does not
      guarantee that a subsequent call to Bind() call will complete, so the caller
      must be prepared to receive IPORT_WAIT again.)

      The user is not required to call Bind() until a non-WOULDBLOCK result is
      returned.  The user can proceed to attempt to send and/or receive data,
      in which case the bind operation (and any network startup) will proceed
      in the background, and any failure in startup phases will be reflected in
      a subsequent error result.
*

Prototype:
   int ISockPort_Bind(ISockPort* po, const void* pAddr)

Parameters:
   po:    pointer to the ISockPort interface
   pAddr: an opaque pointer to the address to bind to (see AEESockAddrStorage)

Return Value:
   AEE_SUCCESS             :  socket has been successfully bound to the local address

   IPORT_WAIT        :  (AEE_AF_INET and AEE_AF_INET6 only) The operation
                          cannot be completed at present.  This can occur when
                          a network connection is being established
                          (see ISockPort_Writeable()).

   AEE_NET_EBADF       :  socket is not open
   AEE_NET_EAFNOSUPPORT:  not an inet address
   AEE_NET_EMFILE      :  Not enough resources to complete this operation
                          (too many sockets in use)
   AEE_NET_ENETDOWN    :  Network layer failed; handset has lost network coverage
   AEE_NET_EOPNOTSUPP  :  specific local IP address was requested (not supported)
   AEE_NET_EADDRINUSE  :  the local address is already in use
   AEE_NET_EINVAL      :  socket is already bound to a local address
   AEE_NET_EFAULT      :  invalid address parameter has been specified

   Other error codes are also possible.

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.

   Binding to a specific local IP address is not currently supported.


Version:
   Introduced BREW Client 3.1.3

See Also:
   None

=======================================================================

ISockPort_Listen()

Description:
   Performs a passive open for connections, such that incoming connections
   may be subsequently accepted.

   The socket must be a TCP socket that has been bound to a local address. The
   nMaxBacklog parameter indicates the maximum length for the queue of pending
   connections.  If nMaxBacklog is larger than the system maximum, it will be
   silently reduced to the system maximum.

Prototype:
   int ISockPort_Listen(ISockPort* po, int nBacklog)

Parameters:
   po:          pointer to the ISockPort interface
   nMaxBacklog: the maximum number of pending connections

Return Value:
   AEE_SUCCESS:  socket has been successfully placed into listening mode
   IPORT_WAIT:  (AEE_AF_INET and AEE_AF_INET6 only) The operation cannot be
                  completed at present.  This can occur when a network
                  connection is being established (see ISockPort_Writeable()).
   AEE_NET_EBADF              :  socket is not open
   AEE_NET_EOPNOTSUPP         :  The socket is not capable of listening (UDP)
   AEE_NET_EFAULT             :  backlog parameter is invalid
   AEE_NET_ENETNONET          :  network subsystem unavailable for some unknown reason
   AEE_NET_ENETINPROGRESS     :  network subsystem establishment currently in progress
   AEE_NET_ENETCLOSEINPROGRESS:  network subsystem close in progress.
   AEE_NET_EINVAL             :  Socket is not bound, or already listening
   AEE_NET_EISCONN            :  socket is connected
   AEE_NET_ENOMEM             :  out of memory

   Other error codes are also possible.

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_Accept()

Description:
   The accept function is used on listening sockets to respond when
   AEE_NET_READ_EVENT is asserted.  The first backlog queued connection is
   removed from the queue, and bound to a new connected socket.  The
   newly created socket is in the connected state.  The listening socket
   is unaffected and the queue size is maintained (i.e. there is no need
   to call listen again.)

Prototype:
   int ISockPort_Accept(ISockPort* po, ISockPort** ppISockPort)

Parameters:
   po:          [in]  Pointer to the listening ISockPort interface
   ppISockPort: [out] Pointer to the newly accepted ISockPort interface

Return Value:
   AEE_SUCCESS:  a newly accepted socket has been returned via pps

   IPORT_WAIT:  (AEE_AF_INET and AEE_AF_INET6 only) No new sockets are
                  available now; try again later (see ISockPort_Readable())

   AEE_NET_EBADF              :  socket is not open
   AEE_NET_EFAULT             :  invalid pps pointer
   AEE_NET_EINVAL             :  socket is not listening
   AEE_NET_EOPNOTSUPP         :  The socket is not of type SOCK_STREAM
   AEE_NET_ENETNONET          :  network subsystem unavailable for some unknown reason
   AEE_NET_ENETINPROGRESS     :  network subsystem establishment currently in progress
   AEE_NET_ENETCLOSEINPROGRESS:  network subsystem close in progress.
   AEE_NET_ENOMEM             :  not enough memory to establish connection.
   AEE_NET_EMFILE             :  Not enough resources to complete this operation
                                 (too many sockets in use)

   Other error codes are also possible.

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_Connect()

Description:
   For sockets of type AEE_SOCKPORT_STREAM, this attempts to initiate
   a TCP connection to the specified remote address.

   AEE_AF_LOCAL:
      Currently AEE_AF_LOCAL is not supported.

   AEE_AF_INET and AEE_AF_INET6:
      Prior to performing any socket-specific behavior, ISockPort_Connect() will
      try to acquire an IP address and ensure the underlying network layer is
      ready for communication.  This may involve the establishment of an
      Internet connection using CDMA Packet Data or QNC if such a connection
      has not already been established.  Finally, any previously-requested
      ISockPort_Bind() operation will be completed if necessary.  As a result,
      errors relating to network startup or ISockPort_Bind() could be reported
      here.

      Also note the possibility of an IPORT_WAIT result.  This will
      normally occur only in cases where a network connection (e.g. a PPP link)
      must be established.  In that event, Connect() will return IPORT_WAIT,
      and can be called again to obtain the final result (error or success).  For
      notification of when to call Connect() again, the ISockPort_Writeable() call can
      be used.  (Note that just as with Write(), a Writeable() callback does not
      guarantee that a subsequent call to Connect() call will complete, so the caller
      must be prepared to receive IPORT_WAIT again.)

Prototype:
   int ISockPort_Connect(ISockPort* po, const void* pAddr)

Parameters:
   po:    pointer to the ISockPort interface
   pAddr: an opaque pointer to the address to connect to (see AEESockAddrStorage)

Return Value:
   AEE_SUCCESS:  socket has been successfully connected to the remote address

   IPORT_WAIT:  (AEE_AF_INET and AEE_AF_INET6 only) The operation cannot be
                  completed at present.  This can occur when a network
                  connection is being established (see ISockPort_Writeable()).

   AEE_NET_EBADF         :  socket is not open
   AEE_NET_EAFNOSUPPORT  :  not an inet address
   AEE_NET_EMFILE        :  Not enough resources to complete this operation
                            (too many sockets in use)
   AEE_NET_ENETDOWN      :  Network layer failed; handset has lost network coverage
   AEE_NET_ECONNREFUSED  :  connection attempt refused
   AEE_NET_ETIMEDOUT     :  connection attempt timed out
   AEE_NET_EFAULT        :  invalid address parameter has been specified
   AEE_NET_EOPNOTSUPP    :  invalid server address specified, or not a stream socket
   AEE_NET_EISCONN       :  socket is already connected
   AEE_NET_ENOMEM        :  not enough memory to establish connection.
   AEE_NET_EIPADDRCHANGED:  IP address changed due to PPP resync
   AEE_NET_EINPROGRESS   :  connection establishment in progress
   AEE_NET_ENETDOWN      :  network subsystem unavailable

   The following error codes can be returned for a stream socket in case
   the AEE_SO_ERROR_ENABLE socket option is enabled and there is a pending
   ICMP error for the socket: 

   AEE_NET_ENETUNREACH   :  network is unreachable
   AEE_NET_EHOSTUNREACH  :  host is unreachable
   AEE_NET_EHOSTDOWN     :  host is down
   AEE_NET_ENONET        :  host is not on the network
   AEE_NET_EPROTO        :  protocol error
   AEE_NET_EACCES        :  access denied
   AEE_NET_ENOPROTOOPT   :  protocol unreachable
   AEE_NET_EMSGSIZE      :  message too large

   Other error codes are also possible.

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.

   ISockPort_Connect() with DGRAM sockets is not supported.

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_WriteV()

Description:
   This function writes data to a connected socket.  It gathers data from one or
   more buffers described by the entries in the iov[] array into a single write
   operation.  Behaviour can differ from separate calls to ISockPort_Write() in
   that multiple calls to ISockPort_Write() might unnecessarily generate multiple
   packets.

   This function always returns immediately with the number of bytes that were
   successfully written.  If no bytes could be successfully written and the
   connection is still active, ISockPort_WriteV() will return IPORT_WAIT.
   To be notified when to call ISockPort_WriteV() again, the caller should call
   ISockPort_Writeable().

Prototype:
   int32 ISockPort_WriteV(ISockPort* po, const SockIOBlock iov[], uint16 wIovCount)

Parameters:
   po:        pointer to the ISockPort interface
   iov:       an array of SockIoBlock structures into which data can be read
   wIovCount: specifies the number of entries in the iov array

Return Value:
   bytes_written (>0):  Any positive number indicates the total number of bytes
                        successfully written from all of the iov[] buffers.

   IPORT_WAIT      :  No bytes could be written at this time; try again later.
                        (See ISockPort_Writeable())

   IPORT_ERROR     :  the specific error code can be retrieved by calling
                        ISockPort_GetLastError():
~
      AEE_NET_EBADF         :  socket is not open
      AEE_NET_ENOTCONN      :  socket not connected
      AEE_NET_ECONNRESET    :  connection reset by server
      AEE_NET_ECONNABORTED  :  connection aborted due to timeout or other failure
      AEE_NET_EIPADDRCHANGED:  address changed, causing connection reset
      AEE_NET_EPIPE         :  broken pipe
      AEE_NET_ENETDOWN      :  network subsystem unavailable
      AEE_NET_EFAULT        :  invalid address parameter has been specified
      AEE_NET_EINVAL        :  not a stream socket
      AEE_NET_ESHUTDOWN     :  socket is shutdown for writing

      Other error codes are also possible.
*

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_ReadV()

Description:
   This function reads data from a socket.  It reads data into one or more
   buffers described by the entries in the iov[] array.

   This function always returns immediately.  If there is no data available and the
   connection is still active, ISockPort_ReadV() will return IPORT_WAIT.  To
   be notified when to call ISockPort_ReadV() again, the caller should call
   ISockPort_Readable().

Prototype:
   int32 ISockPort_ReadV(ISockPort* po, SockIOBlock iov[], uint16 wIovCount)

Parameters:
   po:        pointer to the ISockPort interface
   iov:       an array of SockIoBlock structures into which data can be read
   wIovCount: specifies the number of entries in the iov array


Return Value:
   bytes_read (>0)   :  Any positive number indicates the total of all the bytes
                        read into the provided buffers.

   0 (zero)          :  There is no more data to be received; the peer has shut down
                        the connection.

   IPORT_WAIT      :  No data available now; try again later.  (See
                        ISockPort_Readable())

   IPORT_ERROR     :  the specific error code can be retrieved by calling
                        ISockPort_GetLastError():
~
      AEE_NET_EBADF         :  socket is not open
      AEE_NET_ENOTCONN      :  socket not connected
      AEE_NET_ECONNRESET    :  connection reset by server
      AEE_NET_ECONNABORTED  :  connection aborted due to timeout or other failure
      AEE_NET_EIPADDRCHANGED:  IP address changed, causing connection reset
      AEE_NET_EPIPE         :  broken pipe
      AEE_NET_ENETDOWN      :  network subsystem unavailable
      AEE_NET_EFAULT        :  invalid address parameter has been specified
      AEE_NET_EINVAL        :  not a stream socket
      AEE_NET_ESHUTDOWN     :  socket is shutdown for reading

      Other error codes are also possible.
*

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_SendTo()

Description:
   This function sends a datagram packet to the specified address from the
   local address bound to the socket. This function must be used with sockets
   of type AEE_SOCKPORT_DGRAM -- not with sockets of type AEE_SOCKPORT_STREAM.
   If the socket has not been bound to a local address, this function will
   bind it to a local address.

   Sockets always operate in non-blocking mode. This function returns immediately
   and, if successful, returns the number of bytes written.  If no bytes could be
   successfully sent and the connection is still active, the function will
   return IPORT_WAIT.  To be notified when to call ISockPort_SendTo()
   again, the caller should call ISockPort_Writeable().

   For an AEE_AF_INET or AEE_AF_INET6 datagram socket, ISockPort_SendTo() can 
   be used on multiple interfaces (e.g. BREW Loopback and PPP) for the same
   socket.  In this case, ISockPort_Writeable() is tied to the previous call to
   ISockPort_SendTo().  If ISockPort_SendTo() has never been called,
   ISockPort_Writeable() is tied to all available interfaces.

   AEE_AF_LOCAL:
      Currently AEE_AF_LOCAL is not supported.

   AEE_AF_INET and AEE_AF_INET6:
      Prior to performing any socket-specific behavior, ISockPort_SendTo() will
      try to acquire an IP address and ensure the underlying network layer is
      ready for communication.  This may involve the establishment of an
      Internet connection using CDMA Packet Data or QNC if such a connection
      has not already been established.  Finally, any previously-requested
      ISockPort_Bind() operation will be attempted if necessary.  As a result,
      errors relating to network startup or ISockPort_Bind() could be reported
      here, and IPORT_WAIT return values are commonly encountered.

   Two flags are supported for the SendTo operation on UDP sockets:
   -    ISockPort_FLAG_SENDTO_URGENT flag will attempt to deliver the message even
   if the network is asleep (e.g. on a common channel instead of a dedicated channel).
   There will probably be limits on the size of such signaling, even if it is supported.
   See the INetwork_GetOpt() function for details. This feature should be used with discretion,
   and requires a privilege: NET_URGENT must be specified as an external dependency in the application's MIF
   in order to utilize this option. It will be useful for applications with very low bandwidth
   requirements or applications which are sensitive to the additional network delay of waiting for 
   the network to be awakened from dormancy.
   -    ISockPort_FLAG_SENDTO_WAKEUP flag can be used in conjunction with the ISockPort_FLAG_SENDTO_URGENT
   flag to concurrently request that the network should be awakened from dormancy as
   soon as possible, subsequent to the message delivery. If only the ISockPort_FLAG_SENDTO_WAKEUP
   is set, UDP data will NOT be sent. This feature requires System privileges.

Prototype:
   int32 ISockPort_SendTo(ISockPort* po, const char* pcBuf, int32 cbBuf,
                          uint32 dwFlags, const void* pAddr)

Parameters:
   po:      pointer to the ISockPort interface
   pcBuf:   buffer of data to send
   cbBuf:   size of the buffer, in bytes
   dwFlags  :  Data transport options:
      ISockPort_FLAG_SENDTO_WAKEUP
      ISockPort_FLAG_SENDTO_URGENT
   pAddr:   an opaque pointer to the address to send to (see AEESockAddrStorage)

Return Value:

   bytes_sent (>0)   :  any positive number indicates the number of bytes
                        successfully sent.

   IPORT_WAIT      :  Cannot send data at this time; try again later.
                        (See ISockPort_Writeable())

   IPORT_ERROR     :  the specific error code can be retrieved by calling
                        ISockPort_GetLastError():
~
      AEE_NET_EBADF       :  socket is not open
      AEE_NET_EAFNOSUPPORT:  not an inet address
      AEE_NET_EMFILE      :  not enough resources to complete
                             this operation (too many sockets are in use).
      AEE_NET_ENETDOWN    :  network is not available (e.g. handset is outside
                             of coverage area)
      AEE_NET_EOPNOTSUPP  :  not a datagram socket
      AEE_NET_EFAULT      :  invalid pcBuf pointer
      AEE_NET_ENOMEM      :  not enough memory
      AEE_NET_EINVAL      :  socket not bound
      EPRIVLEVEL          :  no privileges to use URGENT or WAKEUP flags

      The following error codes can be returned in case the AEE_SO_ERROR_ENABLE
      socket option is enabled and there is a pending ICMP error for the socket: 

      AEE_NET_ENETUNREACH :  network is unreachable
      AEE_NET_EHOSTUNREACH:  host is unreachable
      AEE_NET_EHOSTDOWN   :  host is down
      AEE_NET_ENONET      :  host is not on the network
      AEE_NET_EPROTO      :  protocol error
      AEE_NET_EACCES      :  access denied
      AEE_NET_ENOPROTOOPT :  protocol unreachable
      AEE_NET_ECONNREFUSED:  port unreachable
      AEE_NET_EMSGSIZE    :  message too large
      AEE_NET_EOPNOTSUPP  :  operation not supported

      Other error codes are also possible.
*

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_RecvFrom()

Description:
   This function reads data from a datagram socket and records the address
   of the sender.

   This function always returns immediately with the number of bytes read.
   If no packets have arrived and the socket is still in a valid state,
   ISockPort_RecvFrom() will return IPORT_WAIT.  ISockPort_Readable()
   may be used to receive notification of when to try ISockPort_RecvFrom()
   again.

   Prior to performing any socket-specific behavior, ISockPort_RecvFrom() will
   try to acquire an IP address and ensure the underlying network layer is
   ready for communication.  This may involve the establishment of an
   Internet connection using CDMA Packet Data or QNC if such a connection
   has not already been established.  Finally, any previously-requested
   ISockPort_Bind() operation will be attempted if necessary.  As a result,
   errors relating to network startup or ISockPort_Bind() could be reported
   here, and IPORT_WAIT return values are commonly encountered.

Prototype:
   int32 ISockPort_RecvFrom(ISockPort* po, char* pcBuf, int32 cbBuf,
                            uint32 dwFlags, void* pAddr, int* pnSize)

Parameters:
   po:      pointer to the ISockPort interface
   pcBuf:   buffer to hold the received data
   cbBuf:   size of the buffer, in bytes
   dwFlags: not used
   pAddr:   a pointer to the address to be filled in (see AEESockAddrStorage)
   pnSize:  a pointer to the size of the address.  Prior to the call, this size
            should be initialized to the size of the space that pAddr points to.
            Upon return, this size will reflect the number of bytes written into
            pAddr.

Return Value:

   bytes_read (>=0)  :  any non-negative number indicates a number of bytes that have been
                        successfully read into the provided buffer

   IPORT_WAIT      :  No data available now; try again later.  (See
                        ISockPort_Readable())

   IPORT_ERROR     :  the specific error code can be retrieved by calling
                        ISockPort_GetLastError():
~
      AEE_NET_EBADF     :  socket is not open
      AEE_NET_EMFILE    :  not enough resources to complete
                           this operation (too many sockets are in use).
      AEE_NET_ENETDOWN  :  network is not available (e.g. handset is outside
                           of coverage area)
      AEE_NET_EOPNOTSUPP:  not a datagram socket
      AEE_NET_EADDRINUSE:  attempted to bind to local port already in use
      AEE_NET_EFAULT    :  invalid pcBuf pointer
      AEE_NET_EINVAL    :  socket not bound

      The following error codes can be returned in case there is no data to read,
      the AEE_SO_ERROR_ENABLE socket option is enabled and there is a pending
      ICMP error for the socket: 

      AEE_NET_ENETUNREACH :  network is unreachable
      AEE_NET_EHOSTUNREACH:  host is unreachable
      AEE_NET_EHOSTDOWN   :  host is down
      AEE_NET_ENONET      :  host is not on the network
      AEE_NET_EPROTO      :  protocol error
      AEE_NET_EACCES      :  access denied
      AEE_NET_ENOPROTOOPT :  protocol unreachable
      AEE_NET_ECONNREFUSED:  port unreachable
      AEE_NET_EMSGSIZE    :  message too large
      AEE_NET_EOPNOTSUPP  :  operation not supported

      Other error codes are also possible.
*

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_GetSockName()

Description:
   This function returns the local address of the socket.

Prototype:
   int ISockPort_GetSockName(ISockPort* po, void* pAddr, int* pnSize)

Parameters:
   po:     pointer to the ISockPort interface
   pAddr:  an opaque pointer to the address to be filled in (see AEESockAddrStorage)
   pnSize: a pointer to the size of the address.  Prior to the call, this size
           should be initialized to the size of the space that pAddr points to.
           Upon return, this size will reflect the number of bytes written into
           pAddr.

Return Value:
   AEE_SUCCESS:  pAddr has been successfully filled in with the local address

   AEE_NET_EBADF :  socket is not open
   AEE_NET_EINVAL:  socket is not bound to a local address
   AEE_NET_EFAULT:  invalid address parameter has been specified

   Other error codes are also possible.

Comments:
   None

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_GetPeerName()

Description:
   This function returns the remote address of the connected socket.

Prototype:
   int ISockPort_GetPeerName(ISockPort* po, void* pAddr, int* pnSize)

Parameters:
   po:     pointer to the ISockPort interface
   pAddr:  an opaque pointer to the address to be filled in (see AEESockAddrStorage)
   pnSize: a pointer to the size of the address.  Prior to the call, this size
           should be initialized to the size of the space that pAddr points to.
           Upon return, this size will reflect the number of bytes written into
           pAddr.

Return Value:
   AEE_SUCCESS:  pAddr has been successfully filled in with the remote address

   AEE_NET_EBADF     :  socket is not open
   AEE_NET_EOPNOTSUPP:  The socket is not of type SOCK_STREAM
   AEE_NET_EINVAL    :  not a stream socket
   AEE_NET_ENOTCONN  :  socket is not connected
   AEE_NET_ECONNRESET:  remote connection has been closed
   AEE_NET_EFAULT    :  invalid address parameter has been specified

   Other error codes are also possible.

Version:
   Introduced BREW Client 3.1.3

   See Also:
   None

===========================================================================

Function: ISockPort_GetOpt()

Description:
   Gets options for a socket.

Prototype:
   int ISockPort_GetOpt(ISockPort* po, int nLevel, int nOptName, void *pOptVal, int* pnOptSize)

Parameters:
   po:        [in]     pointer to the ISockPort interface
   nLevel:    [in]     option level (see AEESockOpt)
   nOptName:  [in]     option name (see AEESockOpt)
   pOptVal:   [out]    pointer to the data type appropriate for the option (see AEESockOpt)
   pnOptSize: [in/out] on input, pnOptSize specifies the size of pOptVal in bytes.
                       On output, pnOptSize is set to the number of data bytes
                       filled into pOptVal.

Return Value:
   AEE_SUCCESS:  The socket option has been successfully retrieved.

   IPORT_WAIT:  Operation in progress; use ISockPort_Readable() to
                  be notified when to try again.

   AEE_NET_EFAULT     :  invalid parameter has been specified
   AEE_NET_EINVAL     :  invalid socket
   AEE_NET_EBADF      :  socket is not open
   AEE_NET_ENOPROTOOPT:  the option is unknown at the level indicated
   AEE_NET_EOPNOTSUPP :  not supported on this socket type
   AEE_NET_ENETDOWN   :  Network layer failed; handset has lost network coverage
   AEE_NET_EMFILE     :  not enough resources to complete
                         this operation (too many sockets are in use).

   Other error codes are also possible.

Comments:
   None

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_SetOpt()

Description:
   Sets options for a socket.

Prototype:
   int ISockPort_SetOpt(ISockPort* po, int nLevel, int nOptName, const void *pOptVal, int nOptSize)

Parameters:
   po:       pointer to the ISockPort interface
   nLevel:   option level (see AEESockOpt)
   nOptName: option name (see AEESockOpt)
   pOptVal:  pointer to the data type appropriate for the option (see AEESockOpt)
   nOptSize: size of pOptVal

Return Value:
   AEE_SUCCESS:  The socket option has been successfully set.

   IPORT_WAIT:  Operation in progress; use ISockPort_Writeable() to
                  be notified when to try again.

   AEE_NET_EFAULT     :  invalid parameter has been specified
   AEE_NET_EINVAL     :  invalid socket
   AEE_NET_EBADF      :  socket is not open
   AEE_NET_ENOPROTOOPT:  the option is unknown at the level indicated
   AEE_NET_EOPNOTSUPP :  not supported on this socket type
   AEE_NET_ENETDOWN   :  Network layer failed; handset has lost network coverage
   AEE_NET_EMFILE     :  not enough resources to complete
                         this operation (too many sockets are in use).

   Other error codes are also possible.

Comments:
   None

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_Shutdown()

Description:
   Causes all or part of a full-duplex connection to be terminated gracefully.

   If nHow is AEE_SHUTDOWN_RD, no more reads will be allowed.  If nHow is
   AEE_SHUTDOWN_WR, no more writes will be allowed (AKA half-close).  If
   nHow is AEE_SHUTDOWN_RDWR, both read and write will be disallowed.

Prototype:
   int ISockPort_Shutdown(ISockPort* po, int nHow)

Parameters:
   po:   Pointer to the ISockPort interface to shutdown
   nHow: Dictates which portion(s) of the connection to shutdown

Return Value:
   AEE_SUCCESS:  the socket has been successfully shutdown.

   IPORT_WAIT :  (AEE_AF_INET and AEE_AF_INET6 only) unable to shutdown now;
                   try again later (see ISockPort_Writeable()).

   AEE_NET_EBADF      :  socket is not open
   AEE_NET_EOPNOTSUPP :  the socket is not of type SOCK_STREAM
   AEE_NET_EINVAL     :  unknown nHow parameter
   AEE_NET_ENOTCONN   :  socket is not connected
   AEE_NET_ESHUTDOWN  :  socket is already shutdown

   Other error codes are also possible.

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

===========================================================================

ISockPort_RecvMsg()

Description:
   This function reads data from a datagram socket and records the address
   of the sender, similarly to ISockPort_RecvFrom(). In addition, it can be
   used to retrieve extended ICMP error information.

   This function always returns immediately with the number of bytes read.
   If no packets have arrived and the socket is still in a valid state,
   ISockPort_RecvMsg() will return IPORT_WAIT.  ISockPort_Readable()
   may be used to receive notification of when to try ISockPort_RecvMsg()
   again.

   Prior to performing any socket-specific behavior, ISockPort_RecvMsg() will
   try to acquire an IP address and ensure the underlying network layer is
   ready for communication.  This may involve the establishment of an
   Internet connection using CDMA Packet Data or QNC if such a connection
   has not already been established.  Finally, any previously-requested
   ISockPort_Bind() operation will be attempted if necessary.  As a result,
   errors relating to network startup or ISockPort_Bind() could be reported
   here, and IPORT_WAIT return values are commonly encountered.

Prototype:
   int ISockPort_RecvMsg(ISockPort *po, AEESockMsgHdr *pmh, uint32 uFlags)

Parameters:
   po:      pointer to the ISockPort interface
   pmh:     pointer to AEESockMsgHdr, used to pack parameters.
            The meaning of the members depends on the value of
            the uFlags parameter (not pmh->uFlags, which is not
            currently used for input flags).
            If uFlags is 0 (no flags used):
               pName:         [out] a pointer to the address to be filled
                              in (see AEESockAddrStorage)
               wNameLen:      [in/out]  a pointer to the size of the address.
                              Prior to the call, this size should be initialized
                              to the size of the space that pName points to.
                              Upon return, this size will reflect the number of
                              bytes written into pName.
               pIov:          [out] an array of SockIOBlock structures into which
                              data can be read. It is the caller's responsibility
                              to allocate enough space for the received packet.
               wIovLen:       [in] specifies the number of entries in the pIov
                              array.
               pControl:      Not used.
               wControlLen:   Not used.
               uFlags:        [out] if the AEE_IP_RECVERR socket option is set,
                              the SOCKPORT_FLAG_RECVMSG_ERRQUEUE flag will be set
                              on return when there are pending ICMP errors.
            If the uFlags mask contains SOCKPORT_FLAG_RECVMSG_ERRQUEUE
            (request to retrieve extended ICMP error information, the
             AEE_IP_RECVERR socket option must be set): 
               pName:         [out] a pointer to the address to be filled
                              in (see AEESockAddrStorage) - the returned 
                              address is the original destination of the datagram
                              that caused the error.
               wNameLen:      [in/out]  the size of the address.
                              Prior to the call, this size should be initialized
                              to the size of the space that pName points to.
                              Upon return, this size will reflect the number of
                              bytes written into pName.
               pIov:          [out] an array of SockIOBlock structures into which
                              data can be read. It is the caller's responsibility
                              to allocate enough space for the received packet.
                              On return, the array will contain the payload of the
                              original packet that caused the error.
               wIovLen:       [in] specifies the number of entries in the pIov
                              array.
               pControl:      [out] A pointer to AEESockCMsgHdr.
                              Ancillary data - the extended error information.
                              Only one error is returned in the data section of the 
                              AEESockCMsgHdr, the error is of the type AEEICMPErrMsg.
               wControlLen:   [in/out] the size of pControl in bytes.
                              To retrieve all the extended error information, the
                              size should be at least AEESOCKCMSG_SPACE(sizeof(AEEICMPErrMsg)).
                              On return, this field will contain the number of bytes
                              written into pControl.
               uFlags:        [out] the SOCKPORT_FLAG_RECVMSG_ERRQUEUE flag will be
                              set on return when there are more pending ICMP errors.
                              The SOCKPORT_FLAG_RECVMSG_MSGTRUNC flag will be set on
                              return when the ancillary data was truncated due to
                              lack of space.
   uFlags:  currently, only the SOCKPORT_FLAG_RECVMSG_ERRQUEUE flag is supported 
            (see pmh for details).

Return Value:

   bytes_read (>=0)  :  any non-negative number indicates a number of bytes
                        that have been successfully read into the provided buffer

   IPORT_WAIT        :  no data available now; try again later.  (See
                        ISockPort_Readable())

   IPORT_ERROR       :  the specific error code can be retrieved by calling
                        ISockPort_GetLastError():
~
      AEE_NET_EBADF     :  socket is not open
      AEE_NET_EMFILE    :  not enough resources to complete
                           this operation (too many sockets are in use).
      AEE_NET_ENETDOWN  :  network is not available (e.g. handset is outside
                           of coverage area)
      AEE_NET_EOPNOTSUPP:  not a datagram socket
      AEE_NET_EADDRINUSE:  attempted to bind to local port already in use
      AEE_NET_EFAULT    :  invalid pcBuf pointer
      AEE_NET_EINVAL    :  socket not bound

      The following error codes can be returned in case there is no data to read,
      the AEE_SO_ERROR_ENABLE socket option is enabled and there is a pending
      ICMP error for the socket: 

      AEE_NET_ENETUNREACH :  network is unreachable
      AEE_NET_EHOSTUNREACH:  host is unreachable
      AEE_NET_EHOSTDOWN   :  host is down
      AEE_NET_ENONET      :  host is not on the network
      AEE_NET_EPROTO      :  protocol error
      AEE_NET_EACCES      :  access denied
      AEE_NET_ENOPROTOOPT :  protocol unreachable
      AEE_NET_ECONNREFUSED:  port unreachable
      AEE_NET_EMSGSIZE    :  message too large
      AEE_NET_EOPNOTSUPP  :  operation not supported

      Other error codes are also possible.
*

Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an AEE_NET_EWOULDBLOCK error code.
   Specified by Posix.1g.

Version:
   Introduced BREW Client 4.0.2

See Also:
   AEESockAddrStorage, AEESockMsgHdr, AEESockCMsgHdr, AEEICMPErrMsg

=====================================================================
DATA TYPES DOCUMENTATION
=======================================================================

=============================================================================

PFNSDBACKEVENT

Description:
   This data type is the prototype for an SDB-Ack event callback function. It is
   called when an SDB-Ack is being received on a socket.

Definition:
   typedef void (*PFNSDBACKEVENT)(ISockPort* pISockPort, void* pUserData);

Members:
   pISockPort: Pointer to the SockPort instance on which the SDB-Ack arrived.
   pUserData:  Pointer to user-specific data. It is actually the pUserData
   member of the AEESDBAckCBType struct that the user supplied 
   when setting the AEE_SO_SDB_ACK_CB socket option.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01.

See Also:
   AEESDBAckCBType

=======================================================================

AEESDBAckCBType

Description:
   This data type is used for setting and getting the
   AEE_SO_SDB_ACK_CB socket option.

Definition:
   typedef struct AEESDBAckCBType
   {
      PFNSDBACKEVENT pfn;
      void* pUserData;
   } AEESDBAckCBType;

Members:
   pfn:       Pointer to a callback function. The callback
   will be called when an SDB Ack arrives on the socket.
   pUserData: User-provided data to be passed to the callback
   when it is being called.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01.

See Also:   
   PFNSDBACKEVENT

====================================================*/

#endif /* AEEISOCKPORT_H */

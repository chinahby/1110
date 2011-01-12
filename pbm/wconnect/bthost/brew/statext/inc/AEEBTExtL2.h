#ifndef _AEE_BT_EXT_L2_H_
#define _AEE_BT_EXT_L2_H_

/*===========================================================================
FILE:      AEEBTExtL2.h

SERVICES:  BlueTooth L2CAP interface for BREW

GENERAL DESCRIPTION: BREW interface to the L2 layer

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTEXTL2

        Copyright © 2004-2005,2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
=============================================================================*/

#include "AEE.h"
#include "AEENet.h"

// BREW generated ID
#define AEECLSID_BLUETOOTH_L2    	0x010250d7

#define AEE_L2_ERROR                (-1)
#define AEE_L2_WOULDBLOCK           (-2)
#define AEE_L2_BUF_TOO_SMALL        (-3)

// event types 
enum
{
  AEEBT_L2_EVT_CONNECTED,
  AEEBT_L2_EVT_CONNECT_FAILED,
  AEEBT_L2_EVT_DISCONNECTED,
  AEEBT_L2_EVT_RECONFIGURED,
  AEEBT_L2_EVT_PING_RSP,
  AEEBT_L2_EVT_DISCONN_IMMEDIATE
};

// error codes for AEEBT_L2_EVT_*
enum
{
  AEEBT_L2_ERR_NONE,
  AEEBT_L2_ERR_BAD_PARAMS,
  AEEBT_L2_CONFIG_PARAMS_NOT_AGREEABLE,
  AEEBT_L2_LOCAL_REJECT_CONNECTION,
  AEEBT_L2_REMOTE_REJECT_CONNECTION,
  AEEBT_L2_PAGE_FAILED,
  AEEBT_L2_LINK_LOST,
  AEEBT_L2_AUTHENTICATION_FAILED,
  AEEBT_L2_UNSPECIFIED_ERROR,
  AEEBT_L2_PING_FAILED,
  AEEBT_L2_SETPRM_REG_FULL,
  AEEBT_L2_REG_FULL,
  AEEBT_L2_REG_DUPLICATE,
  AEEBT_L2_CONN_NOT_REGISTERED,
  AEEBT_L2_CONN_ALL_USED,
  AEEBT_L2_WRITE_CID_NOT_FOUND,
  AEEBT_L2_WRITE_BIG_PKT_SIZE,
  AEEBT_L2_DISC_CID_NOT_FOUND,
  AEEBT_L2_DISC_ERROR,
  AEEBT_L2_UNREG_NOT_FOUND,
  AEEBT_L2_INVALID_PSM,
  AEEBT_L2_CONN_NOT_UP,
  AEEBT_L2_ALREADY_CONNECTED,
  AEEBT_L2_NOT_CONNECTED
};

typedef struct _IBTExtL2 IBTExtL2;

typedef struct _AEEBTL2ConfigInfo AEEBTL2ConfigInfo;

// Types used in the L2 layer

AEEINTERFACE(IBTExtL2)
{
  // IAStream gives us BASE as well as Readable, Read, and Cancel
  INHERIT_IAStream(IBTExtL2);
  
  int    (*Register)( IBTExtL2* po, uint32 PSM );
  int    (*Deregister)( IBTExtL2* po );
  int    (*SetParams)( IBTExtL2* po, uint16 proto_id, const AEEBTL2ConfigInfo* info );
  int    (*Connect)( IBTExtL2* po, uint16 proto_id, const AEEBTBDAddr* pBDAddr );
  int    (*Disconnect)( IBTExtL2* po );
  int32  (*Write)( IBTExtL2* po, const byte* buffer, uint32 bytes );
  int32  (*WriteV)( IBTExtL2* po, const SockIOBlock iov[], uint16 nIovCount );
  int32  (*ReadV)( IBTExtL2* po, SockIOBlock iov[], uint16 nIovCount );
  void   (*Writable)( IBTExtL2* po, PFNNOTIFY pfn, void* pUser );
  uint16 (*NumConn)( IBTExtL2* po, const AEEBTBDAddr *pBdAddr );
  int    (*DisconnImmediate)( IBTExtL2* po );
  int    (*GetOEMHandle)(IBTExtL2 *pParent, int32 *pnHandle);
  int    (*Clone)(IBTExtL2 *pParent, int32 nHandle);
  int    (*GetStatus)(IBTExtL2 *pParent, AEEBTL2Connected *pStatus);
};

// Standard IAStream Methods
#define IBTEXTL2_AddRef(p) \
        AEEGETPVTBL((p),IBTExtL2)->AddRef((p))

#define IBTEXTL2_Release(p)\
        AEEGETPVTBL((p),IBTExtL2)->Release((p))

#define IBTEXTL2_Readable(p, pfn, pUser)\
        AEEGETPVTBL((p),IBTExtL2)->Readable((p), (pfn), (pUser))

#define IBTEXTL2_Read(p, d, n)\
        AEEGETPVTBL((p),IBTExtL2)->Read((p), (d), (n))

#define IBTEXTL2_Cancel(p,pfn,pUser)\
        AEEGETPVTBL((p),IBTExtL2)->Cancel((p), (pfn), (pUser))

// IBTExtL2 Methods
#define IBTEXTL2_Register(p,psm)\
        AEEGETPVTBL((p),IBTExtL2)->Register((p),(psm))

#define IBTEXTL2_Deregister(p)\
        AEEGETPVTBL((p),IBTExtL2)->Deregister((p))

#define IBTEXTL2_SetParams(p,prot,info)\
        AEEGETPVTBL((p),IBTExtL2)->SetParams((p),(prot),(info))

#define IBTEXTL2_Connect(p,proto,addr)\
        AEEGETPVTBL((p),IBTExtL2)->Connect((p),(proto),(addr))

#define IBTEXTL2_Disconnect(p)\
        AEEGETPVTBL((p),IBTExtL2)->Disconnect((p))

#define IBTEXTL2_Write(p,buf,size)\
        AEEGETPVTBL((p),IBTExtL2)->Write((p),(buf),(size))

#define IBTEXTL2_WriteV(p,iov,cnt)\
        AEEGETPVTBL((p),IBTExtL2)->WriteV((p),(iov),(cnt))

#define IBTEXTL2_ReadV(p,iov,cnt)\
        AEEGETPVTBL((p),IBTExtL2)->ReadV((p),(iov),(cnt))

#define IBTEXTL2_Writable(p,pfn,pUser)\
        AEEGETPVTBL((p),IBTExtL2)->Writable((p),(pfn),(pUser))

#define IBTEXTL2_GetOEMHandle(p, pnH)          \
        AEEGETPVTBL((p),IBTExtL2)->GetOEMHandle((p),(pnH))

#define IBTEXTL2_Clone(p, nH)                  \
        AEEGETPVTBL((p),IBTExtL2)->Clone((p),(nH))

#define IBTEXTL2_GetStatus(p,q)             \
        AEEGETPVTBL((p),IBTExtL2)->GetStatus((p),(q))


// For those trying to change the spelling of the English language:
#define IBTEXTL2_Writeable(p,pfn,pUser) IBTEXTL2_Writable((p),(pfn),(pUser))

#define IBTEXTL2_NumConn(p,addr)\
        AEEGETPVTBL((p),IBTExtL2)->NumConn((p),(addr))

#define IBTEXTL2_DisconnImmediate(p)\
        AEEGETPVTBL((p),IBTExtL2)->DisconnImmediate((p))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================
AEEBTQosType

Description: Various Quality of Service options for L2 connections.

Definition:

  typedef enum _AEEBTQoSType
  {
    AEEBT_L2_QOS_NO_TRAFFIC,
    AEEBT_L2_QOS_BEST_EFFORT,
    AEEBT_L2_QOS_GUANANTEED
  } AEEBTQosType;
 
=======================================================================
AEEBTL2ConfigInfo

Description: Configuration information associated with an L2 connection.
  The parameters are defined in the BlueTooth spec.

Definition:

  typedef struct _AEEBTL2ConfigInfo
  {
    uint16 out_flush_timeout;
    uint16 in_mtu;
    uint16 out_mtu;
    AEEBTQoSType qos_service_type;
    uint32 token_rate;
    uint32 token_bucket_size;
    uint32 peak_bandwidth;
    uint32 latency;
    uint32 delay_variation;
  } AEEBTL2ConfigInfo;

=======================================================================
AEEBTConnected

Description: Information passed along with an AEEBT_L2_EVT_CONNECTED
  notification. Contains information about the successfully created
  ACL connection.

Definition:

  struct _AEEBTConnected
  {
    AEEBTBDAddr       bdAddr;
    AEEBTL2ConfigInfo info;
  };

=======================================================================
AEEBTReconfigured

Description: Information passed along with an AEEBT_L2_EVT_RECONFIGURED
  notification. Contains information about the successfully reconfigured
  ACL connection.

Definition:

  struct _AEEBTReconfigured
  {
    AEEBTL2ConfigInfo info;
    uint8             error_code;
  };

=======================================================================
AEEBTPingRsp

Description: Information passed along with an AEEBT_L2_EVT_PING_RSP
  notification. Contains information about the ping response.

Definition:

  struct _AEEBTPingRsp
  {
    uint8* data;
    uint16 size;
  };

=====================================================================*/

/*=====================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name:  IBTExtL2

Description: BlueTooth L2CAP interface

  The use of IBTExtL2 consists of sending commands and receiving
  events.  These events are delivered to IBTExtL2 clients as
  notifications, hence the clients must register for L2 notification
  via ISHELL_RegisterNotify() and the notification mask to use is
  NMASK_BT_L2.  See AEEBTExt.h for more information on IBTExtNotifier.

  Each instance of an IBTExtL2 interface can control at most one L2
  connection (CID). If more than one connection in required, create
  more than one instance of the interface.
  
  If a command returns SUCCESS, then an event may be generated in
  response at a later time.  However, not all events are associated
  with a command.  Some events are asyncronously generated by the BT
  driver.

  Also, there are Readable and Writable callbacks which are called (if
  registered) in the event that the current connection is readable or
  writable.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an
  AEENotify whose dwMask field is the combination of the notification
  mask and the event which is one of the AEEBT_L2_EVT_* events when
  the notification mask is NMASK_BT_L2.  The pData field of the
  AEENotify is a pointer to NotificationData.
  
  Event                           pData->
  -----                           ----------------------------
  AEEBT_L2_EVT_CONNECTED          pConnected - connection info
  AEEBT_L2_EVT_CONNECT_FAILED     uError - reason for failure
  AEEBT_L2_EVT_DISCONNECTED       uError - reason for disconnection
  AEEBT_L2_EVT_RECONFIGURED       pReconfigured - reconfig info
  AEEBT_L2_EVT_PING_RSP           pPingRsp - the ping response

Interfaces:

  IBTEXTL2 provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTL2_AddRef()
    IBTEXTL2_Release()
  IASTREAM                     AEE.h
    IBTEXTL2_Readable()
    IBTEXTL2_Read()
    IBTEXTL2_Cancel()


=======================================================================

Function:  IBTEXTL2_Register

Description: 
  Registers a particular protocol with L2. This is necessary to
  receive connections. Note that L2 connections will be automatically
  accepted after this is called. Use IBTEXTL2_SetParams to set the
  parameters to be negotiated for incoming connections. An
  AEEBT_L2_EVT_CONNECTED event will be generated when a remote device
  connects.
  Using this command will cause the device to be Connectable. IE,
  the bluetooth device will scan for connections. This may
  decrease phone standby time.

Prototype:

Parameters:
  PSM - The Protocol Service Multiplexer, as defined in the BT spec.
        Note that most protocols which use L2 have well known PSMs.
        Values in the range 0x1001 and beyond are user defined. Also
        note that this value should usually be only 16 bits and odd.
        See the BT spec for more information.

Return value:
  SUCCESS                - Indicates that you may now receive connections
                           associated with this PSM.
  AEEBT_L2_INVALID_PSM   - bad PSM
  AEEBT_L2_REG_DUPLICATE - PSM already registered to another app.
  
Event:
  None.
  However, you will receive a AEEBT_L2_EVT_CONNECTED event when a remote
  device has connected.

=======================================================================

Function:  IBTEXTL2_Deregister

Description: Deregisters with the L2 layer. L2 will no longer accept
  incoming connections from the PSM which was previously registered.
  Calling this may cause the phone to no longer be connectable. That
  is, the bluetooth device may no longer be scanning for connections.

Prototype:

Parameters:

Return value:
  SUCCESS                  - Now unregistered.
  AEEBT_L2_UNREG_NOT_FOUND - Not registered.

Event:
  None.

=======================================================================

Function:  IBTEXTL2_SetParams

Description: This function will perform one of two actions:
  1) If you have no L2 connection, this will set the parameters for
     an outgoing connection request, or an incoming L2 connection.
     This is associated with a particular PSM.
  2) If you have an open L2 connection, this will attempt to 
     renegotiate the L2 connection parameters for that connection.
     In this case, the PSM should match the PSM of the current 
     connection.
  Note that support for non-default Flush Timeouts and Quality of Service
  is optional. For this reason, it's recommended to use the default values
  unless other values are necessary and known to work.

Prototype:

Parameters:
  proto_id - PSM you wish to set parameters for
  info - Parameters to set. Note that since the AEEBTL2ConfigInfo type
         is shared, some of the parameters have no meaning for SetParams:
         - PSM: If setting parameters for incoming connections on a PSM, set
                this value to the PSM on which L2 is listening.
         - CID: Use this to reference an existing connection so that it can
                be reconfigured.
         - In Flush Timeout: Sets the flush timeout for the L2 connection.
                Note that the flush_to is implementation dependent, and may
                change from one baseband to the next, or may not be implemented
                at all.
         - In MTU: The maximum transfer unit size that the application can 
                receive.
         - Out MTU: Unused. The L2 layer will negotiate the largest MTU 
                available. If this MTU is insufficient, the L2 link may be
                disconnected by the application.
         - In QoS: The quality of serivce type to be used for this L2 link.
                Note that QoS support is optional, and only 
                AEE_BT_L2_QOS_BEST_EFFORT is guaranteed to be supported.
         - Token Rate: Used for QoS. See BT spec for definition.
         - Token Bucket Size: Used for QoS. See BT spec for definition.
         - Peak Bandwidth: Used for QoS. See BT spec for definition.
         - Latency: Used for QoS. See BT spec for definition.
         - Delay Variation: Used for QoS. See BT spec for definition.

Return value:
  SUCCESS                              - No error. If in an active L2
                                         connection, an event has been
                                         queued and renegotiation of
                                         the parameters will begin.
  AEEBT_L2_CONFIG_PARAMS_NOT_AGREEABLE - if there is an error in the
                                         selected parameters.
  AEEBT_L2_INVALID_PSM                 - If the PSM is not valid

Event:
  This command will generate an event AEEBT_L2_EVT_RECONFIGURED if in
  an active conneciton. The error_code with this event will be
  AEEBT_L2_ERR_NONE if successfully reconfigured, and info will
  contain the reconfigured parameters.

  No event is generated if a connection is not active.

=======================================================================

Function:  IBTEXTL2_Connect

Description: Connect to a remote device.

Prototype:

Parameters:
  proto_id - The PSM to connect to.
  pBDAddr  - The BlueTooth address of the remote device.

Return value:
  SUCCESS if no error.

Event:
  AEEBT_L2_EVT_CONNECTED      - if the connection was accepted
  AEEBT_L2_EVT_CONNECT_FAILED - If the connect failed for some reason.
                                The reason code is contained in the 
                                event.

=======================================================================

Function:  IBTEXTL2_Disconnect

Description: Disconnect the current connection.

Prototype:

Parameters:

Return value:
  SUCCESS - No error, and the disconnection process will begin.

Event:
  AEEBT_L2_EVT_DISCONNECTED will be generated once the connection has
  been terminated.

=======================================================================
Function:  IBTEXTL2_DisconnImmediate

Description: Disconnect the current connection. If this is the last
  connection to the device, it will disconnect the ACL immediately
  rather than waiting for a timeout. If there are other connections,
  this will behave exactly like IBTEXTL2_Disconnect.

Prototype:

Parameters:

Return value:
  SUCCESS - No error, and the disconnection process will begin.

Event:
  AEEBT_L2_EVT_DISCONNECTED_IMMEDIATE will be generated once the 
  connection has been terminated.

=======================================================================

Function:  IBTEXTL2_Write

Description: Write data to the open connection.

Prototype:

Parameters:
  buffer - The data to send
  bytes - Number of bytes in the buffer

Return value:
  Number of bytes queued to be written, or AEE_L2_WOULDBLOCK if the
  queue is full. In this case, call IBTEXTL2_Writable to register a
  callback for when there is more space in the queue.

Event:
  None.

=======================================================================

Function:  IBTEXTL2_WriteV

Description: Write data to the open connection, using a collection of
  buffers. This differs from calling IBTEXTL2_Write with each buffer
  individuallly in that it will collect the data into fewer L2 packets
  before sending.
  This is currently unsupported.

Prototype:

Parameters:
  iov - Array of pointers to buffers with data
  nIovCount - Number of elements in the array

Return value:
  Number of bytes queued to be written, or AEE_L2_WOULDBLOCK if the
  queue is full. In this case, call IBTEXTL2_Writable to register a
  callback for when there is more space in the queue.

Event:
  None.

=======================================================================

Function:  IBTEXTL2_ReadV

Description: Similar to IBTEXTL2_Read, but will write the data to an
  array of buffers.
  This is currently unsupported.

Prototype:

Parameters:
  iov - Array of pointers to buffers.
  nIovCount - Number of elements in the array

Return value:
  Number of bytes read, or AEE_L2_WOULDBLOCK if there is no data. In
  this case, call IBTEXTL2_Readable to register a callback for when
  there is data available.

Event:
  None.

=======================================================================

Function:  IBTEXTL2_Writable

Description: Register a callback function to be called when data can
  be written.

Prototype:

Parameters:
  pfn - Pointer to the callback
  pUser - User data which will be passed to the callback unmodified.

Return value:
  None.

Event:
  None. However, the callback will be called when data can be written.

=======================================================================

Function:  IBTEXTL2_NumConn

Description: Gets the number of open L2 connections to a specific BD Addr.
  Note that this does not include connections coming up or going down,
  only connections in active use.

Prototype:

Parameters:
  pfn - Pointer to the callback
  pBDAddr  - The BlueTooth address of the remote device.

Return value:
  Number of connections to the BD Addr.

Event:
  None.

=======================================================================

Function:  IBTEXTL2_GetOEMHandle()

Description:  Return the handle that identifies the OEM layer object
              associated with this AEE layer object.  This handle can
              then be passed to the Clone function for another IBTExtL2
              object in a different app context.

Prototype:

  int IBTEXTL2_GetOEMHandle( IBTExtL2 *po, int32 *pnHandle );

Parameters:

  po            - pointer to the IBTExtL2 object
  pnHandle      - pointer to an integer to contain the handle value

Return value:

  SUCCESS - Valid handle returned
  EFAILED - Object doesn't have a valid handle

=======================================================================

Function:  IBTEXTL2_Clone()

Description:  Transfer the OEM layer object that was associated with
              some other IBTExtL2, to this one.  This allows a connection
              to be transferred from one app context to another without
              having to close and re-open it.  The app
              that is giving up the connection calls IBTEXTL2_GetOEMHandle()
              and passes the handle value to another app to be used with this
              function.  The OEM layer object that was originally created
              for this object will be released, so that it can be replaced
              with the one that is being transferred.  After this function
              succeeds, API function calls on the IBTExtL2 associated with
              nHandle will fail, except for release.
              The apps transferring the object are expected to provide
              any synchronization they need on their own, for example, there
              is no callback or notification of the transfer provided directly
              through this mechanism.

Prototype:

  int IBTEXTL2_Clone( IBTExtL2 *po, int32 nHandle )

Parameters:

  po            - pointer to the IBTExtL2 object
  nHandle       - A handle from a previous call to IBTEXTL2_GetOEMHandle()
                  on the object whose connection is to be transferred to this
                  object.

Return Value:

  SUCCESS - OEM layer object was successfully transferred.
  EFAILED - The OEM layer object was not successfully transferred.  In this
            case, po should be released since it may be in a half baked
            state.

=======================================================================

Function:  IBTEXTL2_GetStatus

Description:  Returns the current AEEBTL2Conected status of the object.
              This is the same as the last notification event that was
              sent for the object.

prototype:

  int IBTExtL2_GetStatus(IBTExtL2 *pParent, AEEBTL2Connected *pStatus)

Parameters

  po            - pointer to the IBTExtL2 object
  pStatus       - pointer to return location

Return Value:

  SUCCESS - Status returned
  EFAILED - Failed
              
=======================================================================*/

#endif /* _AEE_BT_EXT_L2_H_ */

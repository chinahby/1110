#ifndef _AEE_BT_EXT_SPP_H_
#define _AEE_BT_EXT_SPP_H_

/*===========================================================================
FILE:      AEEBTExtSPP.h

SERVICES:  BlueTooth Serial Port Protocol Interface

GENERAL DESCRIPTION: BREW interface to the BT Serial Port Protocol

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtSPP

        Copyright © 2003, 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
=============================================================================*/

#include "AEESource.h"

// BREW generated ID
#define AEECLSID_BLUETOOTH_SPP    	0x01014d08


// event types returned via HandleEvent()
enum
{
  AEEBT_SPP_EVT_OPENED,         // an SIO stream is opened
  AEEBT_SPP_EVT_CLOSED,         // an SIO stream is closed
  AEEBT_SPP_EVT_CONNECTED,      // connected to remote BT device
  AEEBT_SPP_EVT_DISCONNECTED,   // disconnected from remote BT device
  AEEBT_SPP_EVT_CONFIG_CHANGED, // BT port reconfigured
  AEEBT_SPP_EVT_MODEM_STATUS,   // modem status change report
  AEEBT_SPP_EVT_LINE_ERROR,     // line error report
  AEEBT_SPP_EVT_DTR_CHANGED,    // data terminal ready indication changed
  AEEBT_SPP_EVT_TX_FLUSHED      // all pending output data transmitted
};

// error codes for AEEBT_SPP_EVT_*
enum
{
  AEEBT_SPP_ERR_NONE,
  AEEBT_SPP_ERR_NO_STREAM,        // no SIO stream
  AEEBT_SPP_ERR_OUT_OF_RESOURCES, // no more server channel avail
  AEEBT_SPP_ERR_REMOTE_TIMEOUT,   // remote SDP query timed out
  AEEBT_SPP_ERR_FAILED            // general failure
};


// SPP state
enum
{
  AEEBT_SPP_ST_CLOSED,
  AEEBT_SPP_ST_OPENING,
  AEEBT_SPP_ST_OPEN_ERROR,
  AEEBT_SPP_ST_OPENED,
  AEEBT_SPP_ST_CONNECTED,
  AEEBT_SPP_ST_DISCONNECTING,
  AEEBT_SPP_ST_DISCONNECTED
};
typedef uint8     AEEBTSppState;


// IO Control Commands
enum
{
  AEEBT_IOCTL_INBOUND_FLOW_ENABLE,  // Enable inbound data flow
  AEEBT_IOCTL_INBOUND_FLOW_DISABLE, // Disable inbound data flow
  AEEBT_IOCTL_FLUSH_TX,             // Register to receive
                                    //   AEEBT_SPP_EVT_TX_FLUSHED.
  AEEBT_IOCTL_CD_ASSERT,            // Assert Carrier Detection Indicator
  AEEBT_IOCTL_CD_DEASSERT,          // Deassert Carrier Detection Indicator
  AEEBT_IOCTL_RI_ASSERT,            // Assert Ring Indication
  AEEBT_IOCTL_RI_DEASSERT,          // Dassert Ring Indication
  AEEBT_IOCTL_DSR_ASSERT,           // Assert Data Set Ready Indication
  AEEBT_IOCTL_DSR_DEASSERT,         // Deassert Data Set Ready Indication
  AEEBT_IOCTL_ENABLE_DTR_EVENT,     // Register to receive
                                    //   AEEBT_SPP_EVT_DTR_CHANGED
  AEEBT_IOCTL_GET_DTR,              // Return a boolean (in paramter) which
                                    //   indicates whether or not DTR is
                                    //   currently asserted
  AEEBT_IOCTL_DISABLE_DTR_EVENT,    // De-register from receiving
                                    //   AEEBT_SPP_EVT_DTR_CHANGED
  AEEBT_IOCTL_GET_RTS,              // Return a boolean (in parameter) which
                                    //   indicates whether or not RTS is
                                    //   currently asserted
  AEEBT_IOCTL_BT_CONFIGURE,         // Configure Bluetooth port.  Expect to
                                    //   receive AEEBT_SPP_EVT_CONFIG_CHANGED.
  AEEBT_IOCTL_BT_DISCONNECT,        // Disconnect Bluetooth port.  Expect to
                                    //   receive AEEBT_SPP_EVT_DISCONNECTED.
  AEEBT_IOCTL_BT_GET_STATUS,        // Return Bluetooth status (in parameter).
  AEEBT_IOCTL_MAX
};
typedef uint16    AEEBTIOCtlCommand;


// SPP Modem Status Bitmap
enum
{
  AEEBT_SPP_MS_RTC_ASSERTED_B = 0x04,
  AEEBT_SPP_MS_RTR_ASSERTED_B = 0x08,
  AEEBT_SPP_MS_IC_ASSERTED_B  = 0x40,
  AEEBT_SPP_MS_DV_ASSERTED_B  = 0x80
};
typedef uint8     AEEBTModemStatusBitmap;


// SPP Line Error
enum
{
  AEEBT_SPP_LE_NONE,
  AEEBT_SPP_LE_OVERRUN_ERROR,
  AEEBT_SPP_LE_PARITY_ERROR,
  AEEBT_SPP_LE_FRAMING_ERROR
};
typedef uint8     AEEBTLineErrorType;


// Open Configuration
typedef struct
{
  boolean         bClientApp;       // client or server
  AEEBTBDAddr*    pBDAddr;          // for client: BD address of server
                                    // for server: NULL
  AECHAR*         pSvcName;         // name of service
  uint16          uSvcId;           // UUID of service
  uint16          uSvcVersion;      // version of service
  uint16          uMaxFrameSize;    // 0 means BT driver decides
  uint8           uChannelNumber;   // server channel number

} AEEBTSppOpenConfig;


// A pointer to an AEEBTSppStatus always accompanies the events related to
// SPP state (opened/closed/connected/disconnected)
struct _AEEBTSppStatus
{
  uint16          uID;
  AEEBTBDAddr*    pBDAddr;          // BD address of remote device
  boolean         bClientApp;       // client or server
  AEEBTSppState   state;            // closed, opened, connected, ...
  uint16          uSvcID;           // UUID of service
  uint16          uSvcVersion;      // version of service
  uint16          uReason;          // AEEBT_SPP_ERR_*
  uint8           uChannelNumber;   // server channel number
};


// The fields in AEEBTSppConfig do not control any aspect of the SPP stream. 
// They are settings transferred across the BT connection but do not affect
// the connection.
struct _AEEBTSppConfig
{
  uint8           uBaudRate;
  uint8           uFormat;
  uint8           uFlowCtrl;
  uint8           uXonChar;
  uint8           uXoffChar;

  uint16          uID;              // Only used for configuration change
  uint16          uMaxFrameSize;    //   event.  Ignored on IOCTL BT     
                                    //   configure command.              
};

struct _AEEBTModemStatus
{
  uint16                  uID;
  AEEBTModemStatusBitmap  changedMask;
  AEEBTModemStatusBitmap  status;
  boolean                 breakPresent;
  uint8                   breakLength;
  uint8                   breaks;
};


struct _AEEBTLineError
{
  uint16              uID;
  AEEBTLineErrorType  lineErrorType;
  uint8               overruns;
  uint8               parityErrors;
  uint8               framingErrors;
};


typedef union
{
  boolean*        pbRTSAsserted;    // AEEBT_IOCTL_GET_CURRENT_RTS
  boolean*        pbDTRAsserted;    // AEEBT_IOCTL_DTR_ASSERTED
  AEEBTSppConfig* pSppConfig;       // AEEBT_IOCTL_BT_CONFIGURE
  AEEBTSppStatus* pSppStatus;       // AEEBT_IOCTL_BT_GET_STATUS

} AEEBTIoCtlParam;

typedef struct _IBTExtSPP IBTExtSPP;

AEEINTERFACE(IBTExtSPP)
{
  INHERIT_ISource(IBTExtSPP);
  int   (*Open)(IBTExtSPP* po, const AEEBTSppOpenConfig* pOpenCfg);
  int   (*IOCtl)(IBTExtSPP* po, AEEBTIOCtlCommand cmd, 
                 AEEBTIoCtlParam* pParam);
  int32 (*Write)(IBTExtSPP *po, char *pSrc, int32 cbBuf);
  void  (*Writeable)(IBTExtSPP *po, AEECallback *pcb);
  int   (*Close)(IBTExtSPP* po);
  int   (*GetOEMHandle)(IBTExtSPP *, int32 *);
  int   (*Clone)(IBTExtSPP *, int32);
  int   (*GetuID)(IBTExtSPP *, uint16 *);
  int   (*GetStatus)(IBTExtSPP *, AEEBTSppStatus *);
};


#define IBTEXTSPP_AddRef(p)                     \
        AEEGETPVTBL((p),IBTExtSPP)->AddRef((p))
        
#define IBTEXTSPP_Release(p)                    \
        AEEGETPVTBL((p),IBTExtSPP)->Release((p))
        
#define IBTEXTSPP_QueryInterface(p,i,ppo)       \
        AEEGETPVTBL((p),IBTExtSPP)->QueryInterface((p),(i),(ppo))
        
#define IBTEXTSPP_Read(p,pd,n)                  \
        AEEGETPVTBL((p),IBTExtSPP)->Read((p),(pd),(n))
        
#define IBTEXTSPP_Readable(p,pcb)               \
        AEEGETPVTBL((p),IBTExtSPP)->Readable((p),(pcb))
        
#define IBTEXTSPP_Open(p,po)                    \
        AEEGETPVTBL((p),IBTExtSPP)->Open((p),(po))
        
#define IBTEXTSPP_IOCtl(p,c,pp)                 \
        AEEGETPVTBL((p),IBTExtSPP)->IOCtl((p),(c),(pp))
        
#define IBTEXTSPP_Write(p,ps,n)                 \
        AEEGETPVTBL((p),IBTExtSPP)->Write((p),(ps),(n))
        
#define IBTEXTSPP_Writeable(p,pcb)              \
        AEEGETPVTBL((p),IBTExtSPP)->Writeable((p),(pcb))
        
#define IBTEXTSPP_Close(p)                      \
        AEEGETPVTBL((p),IBTExtSPP)->Close((p))

#define IBTEXTSPP_GetOEMHandle(p, pnH)          \
	AEEGETPVTBL((p),IBTExtSPP)->GetOEMHandle((p),(pnH))

#define IBTEXTSPP_Clone(p, nH)                  \
	AEEGETPVTBL((p),IBTExtSPP)->Clone((p),(nH))

#define IBTEXTSPP_GetuID(p, pu)                 \
	AEEGETPVTBL((p),IBTExtSPP)->GetuID((p), (pu))

#define IBTEXTSPP_GetStatus(p, pss)             \
	AEEGETPVTBL((p),IBTExtSPP)->GetStatus((p), (pss))
        
/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================


=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtSPP

Description: BlueTooth Serial Port profile

  The use of IBTExtSPP consists of sending commands and receiving events.  
  These events are delivered to IBTExtSPP clients as notifications, hence
  the clients must register for SPP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_SPP.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as a modem
  status change.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_SPP_EVT_* events when the notification mask
  is NMASK_BT_SPP.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                           pData->
  -----                           ----------------------------
  AEEBT_SPP_EVT_OPENED            pSPPStatus - pointer to SPP status info
  AEEBT_SPP_EVT_CLOSED            pSPPStatus - pointer to SPP status info
  AEEBT_SPP_EVT_CONNECTED         pSPPStatus - pointer to SPP status info
  AEEBT_SPP_EVT_DISCONNECTED      pSPPStatus - pointer to SPP status info
  AEEBT_SPP_EVT_CONFIG_CHANGED    pSPPConfig - pointer to new config info
  AEEBT_SPP_EVT_MODEM_STATUS      pModemStatus - pointer to modem status info
  AEEBT_SPP_EVT_LINE_ERROR        pLineError - pointer to line error info

Interfaces:

   IBTExtSPP provides several additional interfaces; please see the
   corresponding header file for complete documentation.

   IBASE                        AEE.h
      IBTEXTSPP_AddRef()
      IBTEXTSPP_Release()
   IQUERYINTERFACE              AEE.h
      IBTEXTSPP_QueryInterface()
   ISOURCE
      IBTEXTSPP_Read()
      IBTEXTSPP_Readable()

=======================================================================

Function:  IBTEXTSPP_Open()

Description: Open an SPP stream.

Prototype:

  int IBTEXTSPP_Open( IBTExtSPP* po, const AEEBTSppOpenConfig* pOpenCfg );

Parameters:

  po             - pointer to the IBTExtSPP object
  pOpenCfg       - pointer to the configuration info dictating how the SPP
                   stream should be open

Return value:

  SUCCESS indicates that the SPP open command was performed and that 
  AEEBT_SPP_EVT_OPENED (server applications only) and/or
  AEEBT_SPP_EVT_CONNECTED events will be generated at a later time.
  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
    EBADPARM  - NULL input pointer(s)
    EMEMPTR   - pOpenCfg is invalid
    EBADSTATE - SPP stream already open

Events:

  AEEBT_SPP_EVT_OPENED will be generated once a server SPP stream is opened.
  
  AEEBT_SPP_EVT_CONNECTED will be generated once a BT connection has been
  established.
  
=======================================================================

Function:  IBTEXTSPP_Close()

Description: Close an opened SPP stream.

Prototype:

  int IBTEXTSPP_Close( IBTExtSPP* po );

Parameters:

  po             - pointer to the IBTExtSPP object

Return value:

  SUCCESS indicates that the SPP close command was performed and that 
  an AEEBT_SPP_EVT_CLOSED event will be generated at a later time.
  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
    EBADPARM  - NULL input pointer(s)
    EBADSTATE - or SPP stream not already open

Events:

  AEEBT_SPP_EVT_CLOSED will be generated once the SPP stream associated with
  this object is closed.
  
=======================================================================

Function:  IBTEXTSPP_IOCtl()

Description: Control the SPP stream.

Prototype:

  int IBTEXTSPP_IOCtl( IBTExtSPP* po, AEEBTIOCtlCommand cmd, 
                       AEEBTIoCtlParam* pParam );

Parameters:

  po        - pointer to the IBTExtSPP object
  cmd       - IO Control command to be performed on the open SPP stream
  pParam    - pointer to the parameter for the command, could be NULL

Return value:

  SUCCESS indicates that the SPP IO control command was performed and that 
  depending on the command, an appropriate event might be generated at a 
  later time.  For some commands no events will be generated, and/or 
  contents of pParam are the results.  All other values indicate failure,
  and will not generate an event.  In this context, the following error 
  codes have special meaning:
  
    EBADPARM  - NULL input pointer(s) or invalid command
    EMEMPTR   - pParam is invalid
    EBADSTATE - SPP stream not open

Events:

  See AEEBTIOCtlCommand definition.
  
=======================================================================

Function:  IBTEXTSPP_Read()

Description: Retrieve data received via the SPP stream.  This function should
             be called after AEEBT_SPP_EVT_CONNECTED is received and it should
             be called repeatedly until it returns AEE_STREAM_WOULDBLOCK.

Prototype:

  int32 IBTEXTSPP_Read( IBTExtSPP* po, char* pDst, int32 uMaxBytes );

Parameters:

  po            - pointer to the IBTExtSPP object
  pDst          - pointer to memory location to which data should be copied
  uMaxBytes     - size of memory pointed to by pDst

Return value:

  AEE_STREAM_ERROR      - invalid params, or no SIO stream
  AEE_STREAM_WOULDBLOCK - no data available
  number of bytes read  - a value greater than 0
  
=======================================================================

Function:  IBTEXTSPP_Readable()

Description: Registers an AEEcallback to be invoked when data becomes
             available for reading.

Prototype:

  void IBTEXTSPP_Readable( IBTExtSPP* po, AEECallback* pCb );

Parameters:

  po            - pointer to the IBTExtSPP object
  pCb           - pointer to the AEECallback to be invoked

Return value:

  None.
  
=======================================================================

Function:  IBTEXTSPP_Write()

Description: Send data via the SPP stream.  This function should be called 
             repeatedly until all data get sent or it returns 
             AEE_STREAM_WOULDBLOCK.

Prototype:

  int32 IBTEXTSPP_Write( IBTExtSPP* po, char* pSrc, int32 uNumBytes );

Parameters:

  po            - pointer to the IBTExtSPP object
  pSrc          - pointer to memory location from which data should be copied
  uMaxBytes     - number of data bytes referenced by pSrc

Return value:

  AEE_STREAM_ERROR        - invalid params, or no SIO stream
  AEE_STREAM_WOULDBLOCK   - transmit queue is full
  number of bytes written - a value greater than 0
  
=======================================================================

Function:  IBTEXTSPP_Writeable()

Description: Registers an AEEcallback to be invoked when transmit queue
             can accept more data to be sent out.

Prototype:

  void IBTEXTSPP_Writeable( IBTExtSPP* po, AEECallback* pCb );

Parameters:

  po            - pointer to the IBTExtSPP object
  pCb           - pointer to the AEECallback to be invoked

Return value:

  None.
  
=======================================================================

Function:  IBTEXTSPP_GetOEMHandle()

Description:  Return the handle that identifies the OEM layer object
	      associated with this AEE layer object.  This handle can
              then be passed to the Clone function for another IBTExtSPP
              object in a different app context.

Prototype:

  int IBTEXTSPP_GetOEMHandle( IBTExtSPP *po, int32 *pnHandle );

Parameters:

  po            - pointer to the IBTExtSPP object
  pnHandle      - pointer to an integer to contain the handle value

Return value:

  SUCCESS - Valid handle returned
  EFAILED - Object doesn't have a valid handle

=======================================================================

Function:  IBTEXTSPP_Clone()

Description:  Transfer the OEM layer object that was associated with
              some other IBTExtSPP, to this one.  This allows a connection
              to be transferred from one app context to another without
              having to close and re-open it.  The app
              that is giving up the connection calls IBTEXTSPP_GetOEMHandle()
              and passes the handle value to another app to be used with this
              function.  The OEM layer object that was originally created
              for this object will be released, so that it can be replaced
              with the one that is being transferred.  After this function
              succeeds, API function calls on the IBTExtSPP associated with
              nHandle will fail, except for release.
              The apps transferring the object are expected to provide
              any synchronization they need on their own, for example, there
              is no callback or notification of the transfer provided directly
              through this mechanism.

Prototype:

  int IBTEXTSPP_Clone( IBTExtSPP *po, int32 nHandle )

Parameters:

  po            - pointer to the IBTExtSPP object
  nHandle       - A handle from a previous call to IBTEXTSPP_GetOEMHandle()
                  on the object whose connection is to be transferred to this
                  object.

Return Value:

  SUCCESS - OEM layer object was successfully transferred.
  EFAILED - The OEM layer object was not successfully transferred.  In this
            case, po should be released since it may be in a half baked
            state.

=======================================================================

Function:  IBTEXTSPP_GetuID

Description:  Returns the uID value that is necessary to associate
              notification messages with a particular IBTExtSPP instance.

Prototype:

  int IBTEXTSPP_GetuID( IBTExtSPP *po, uint16 *puID )

Parameters:

  po           - pointer to the IBTExtSPP object
  puid         - Pointer to an unsigned short to hold the uID

Return value

  SUCCESS - *puID is valid
  EFAILED - the uID could not be obtained.

=======================================================================

Function:  IBTEXTSPP_GetStatus

Description:  Returns the current AEEBTSPPConected status of the object.
              This is the same as the last notification event that was
              sent for the object.

prototype:

  int IBTExtSPP_GetStatus(IBTExtSPP *pParent, AEEBTSppStatus *pStatus)

Parameters

  po            - pointer to the IBTExtSPP object
  pStatus       - pointer to return location

Return Value:

  SUCCESS - Status returned
  EFAILED - Failed
           
=======================================================================*/
#endif /* _AEE_BT_EXT_SPP_H_ */

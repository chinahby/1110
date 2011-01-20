#ifndef _AEE_BT_EXT_AVRCP_H_
#define _AEE_BT_EXT_AVRCP_H_

/*===========================================================================
FILE:      AEEBTExtAVRCP.h

SERVICES:  BlueTooth AVRCP interface for BREW

GENERAL DESCRIPTION: BREW interface to the AVRCP layer

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTEXTAVRCP

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

// BREW generated ID
#define AEECLSID_BLUETOOTH_AVRCP         0x0102f409

// error codes for AEEBT_AVRCP_EVT_*
enum
{
  AEEBT_AVRCP_ERR_NONE
};

/*===========================================================================
 * ErrorCodes for AVRCP App
 ===========================================================================*/
/* This error code is used when, configuration
 * parameters doesn't match */
#define AEEBT_AVRCP_ERR_CONFIG_PARAMS_NOT_AGREEABLE     1
/* This error code is used when, connection is
 *  rejected */
#define AEEBT_AVRCP_ERR_CONNECT_REFUSED_NO_RESOURCES    2
/* This error code is used when, there is a 
 * link lost at the lower layers */
#define AEEBT_AVRCP_ERR_LINK_LOST                       3
/* This error code is used when, Authentication
 * fails */
#define AEEBT_AVRCP_ERR_AUTHENTICATION_FAILED           4
/* This is not an error code, is used when
 * normal disconnect happens */
#define AEEBT_AVRCP_ERR_NORMAL_DISCONNECT               5
/* This error code is used when, page 
 * fails */
#define AEEBT_AVRCP_ERR_PAGE_FAILED                     6
/* This error code is used when, a wrong 
 * is issued to the Lower layer */
#define AEEBT_AVRCP_ERR_WRONG_COMMAND                   7

typedef struct _IBTExtAVRCP IBTExtAVRCP;

AEEINTERFACE(IBTExtAVRCP)
{
   INHERIT_IQueryInterface(IBTExtAVRCP);
   int (*Enable)(IBTExtAVRCP* po, AEEBTAVRCPCatType cat);
   int (*Connect)(IBTExtAVRCP* po, AEEBTBDAddr* pa);
   int (*Disconnect)(IBTExtAVRCP* po);
   int (*Disable)(IBTExtAVRCP* po);
   int (*SendReply)(IBTExtAVRCP* po, uint8 txn, AEEBTAVRCPOpStatusType stat,
                    AEEBTAVRCPEventType evt, AEEBTAVRCPRspType rsp);
   int (*SendGenericReply)(IBTExtAVRCP* po, AEEBTAVRCPGenericOpType* pop,
                           uint8* prsp);
   int (*GetGenericOp)(IBTExtAVRCP* po, uint8* pBuf, int nSize);
};

#define IBTEXTAVRCP_AddRef(p)                              \
        AEEGETPVTBL((p),IBTExtAVRCP)->AddRef((p))

#define IBTEXTAVRCP_Release(p)                             \
        AEEGETPVTBL((p),IBTExtAVRCP)->Release((p))

#define IBTEXTAVRCP_QueryInterface(p,i,ppo)                \
        AEEGETPVTBL((p),IBTExtAVRCP)->QueryInterface((p),(i),(ppo))

#define IBTEXTAVRCP_Enable(p,pa)                           \
        AEEGETPVTBL((p),IBTExtAVRCP)->Enable((p),(pa))

#define IBTEXTAVRCP_Connect(p,pa)                          \
        AEEGETPVTBL((p),IBTExtAVRCP)->Connect((p),(pa))

#define IBTEXTAVRCP_Disconnect(p)                          \
        AEEGETPVTBL((p),IBTExtAVRCP)->Disconnect((p))

#define IBTEXTAVRCP_Disable(p)                             \
        AEEGETPVTBL((p),IBTExtAVRCP)->Disable((p))

#define IBTEXTAVRCP_SendReply(p,txn,stat,evt,rsp)          \
        AEEGETPVTBL((p),IBTExtAVRCP)->SendReply((p),(txn),(stat),(evt),(rsp))

#define IBTEXTAVRCP_SendGenericReply(p,frame,rsp)          \
        AEEGETPVTBL((p),IBTExtAVRCP)->SendGenericReply((p),(frame),(rsp))

#define IBTEXTAVRCP_GetGenericOp(p,buf,sz)                 \
        AEEGETPVTBL((p),IBTExtAVRCP)->GetGenericOp((p),(buf),(sz))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtAVRCP

Description: BlueTooth Advanced Audio Distribution Profile

  This interface is for the AVRCP Target role only.

  Consult the Bluetooth ISOD and spec for a description of AVRCP.

  The use of IBTExtAVRCP consists of sending commands and receiving events.
  These events are delivered to IBTExtAVRCP clients as notifications, hence
  the clients must register for AVRCP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_AVRCP.  See AEEBTExt.h for
  more information on IBTExtNotifier.

  If a command returns SUCCESS, then an event is guaranteed to be generated
  in response at a later time.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_AVRCP_EVT_* events when the notification mask
  is NMASK_BT_AVRCP.  The pData field of the AEENotify is a pointer to
  NotificationData.

  Event                                      pData->
  -----                                      ----------------------------
  AEEBT_AVRCP_EVT_CONNECTED                   bdAddr
  AEEBT_AVRCP_EVT_CON_FAILED                  uError
  AEEBT_AVRCP_EVT_DISCONNECTED                uError

Interfaces:

  IBTEXTAVRCP provides several additional interfaces; please see the
  corresponding header file for complete documentation.

  IBASE                        AEE.h
    IBTEXTAVRCP_AddRef()
  IQUERYINTERFACE              AEE.h
    IBTEXTAVRCP_QueryInterface()

=======================================================================

Function: IBTEXTAVRCP_Release()

Description:

   This function decrements the reference count of an object.  The
   object is freed from memory and is no longer valid once its
   reference count reaches 0 (zero).
   Releasing this object will fail if connections still exist;
   calling IBTEXTAVRCP_Disconnect() first is necessary.

Prototype:

  int IBTEXTAVRCP_Release( IBTExtAVRCP*      po );

Parameters:

  po          - Pointer to the IBTExtAVRCP object

Return value:

  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtAVRCP object
    EFAILED   - Object could not be released, possibly due to existing connections

Event:

  None

=======================================================================

Function: IBTEXTAVRCP_Connect()

Description: Connects to an AVRCP sink device.

Prototype:

  int IBTEXTAVRCP_Connect( IBTExtAVRCP*       po,
                           AEEBTAddr*        pBDAddr );

Parameters:

  po            - Pointer to the IBTExtAVRCP object
  pBDAddr       - Address of the AVRCP controller

Return value:

  SUCCESS indicates that the configuration request was queued and that
  AEEBT_AVRCP_EVT_CONNECTED or _CON_FAILED event will be generated at a
  later time. All other values indicate failure, and will not generate an event.

  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtAVRCP object
    ENOMEMORY - out of memory

Event:

  AEEBT_AVRCP_EVT_CONNECTED indicates a successful connection has been established
  with the specfied bluetooth device.

  AEEBT_AVRCP_EVT_CONN_FALIED with accompanying error code.

=======================================================================

Function: IBTEXTAVRCP_Disconnect()

Description: Disconnects an AVRCP device.

Prototype:

  int IBTEXTAVRCP_Disconnect( IBTExtAVRCP*       po );

Parameters:

  po            - Pointer to the IBTExtAVRCP object

Return value:

  SUCCESS indicates that the configuration request was queued and that an
  AEEBT_AVRCP_EVT_DISCONNECTED event will be generated at a later time.
  Note that remote devices should be disconnected before releaseing this
  interface.
  All other values indicate failure, and will not generate an event.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtAVRCP object
    ENOMEMORY - out of memory

Event:

  AEEBT_AVRCP_EVT_DISCONNECTED indicates a successful connection has been
  disconnected.

=======================================================================

Function: IBTEXTAVRCP_Enable()

Description: Enables the AVRCP profile on the local BT device. It also
             updates the service disovery database with new AVRCP 
             service record

Prototype:

  int IBTEXTAVRCP_Enable( IBTExtAVRCP*              pParent,
                          AEEBTAVRCPCatType         supportedCats );

Parameters:

  pParent            - Pointer to the IBTExtAVRCP object
  supportedCats      - Supported categaries of commands. See AVRCP spec,
                       section 4.7.5 for more information

Return value:

  SUCCESS indicates that the AVRCP profile has been enabled
  succesfully.

  All other values indicate failure.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtAVRCP object
    EFAILED   - internal error

Event:

  None.

=======================================================================

Function: IBTEXTAVRCP_Disable()

Description: Disables the AVRCP profile from the local BT device. It also
             removes the AVRCP service record from the service discovery 
             database.

Prototype:

  int IBTExtAVRCP_Disable( IBTExtAVRCP*        po );

Parameters:

  po            - Pointer to the IBTExtAVRCP object

Return value:

  SUCCESS indicates that the AVRCP profile has been disabled
  successfully.
  
  All other values indicate failure.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtAVRCP object
    EFAILED   - internal error
    EFAILED   - Still connected to a device
    
Event:

  None.

=======================================================================

Function: IBTEXTAVRCP_SendReply()

Description: Sends the reply to the pass through command sent by the AVRCP
             controller. Should be called immediately upon receiving the
             passthrough command.

Prototype:

  int  IBTEXTAVRCP_SendReply ( IBTExtAVRCP*             pParent,
                               uint8                    uTxnID,
                               AEEBTAVRCPOpStatusType   uStatus,
                               AEEBTAVRCPEventType      Evt,
                               AEEBTAVRCPRspType        Rsp);
  

Parameters:

  pParent            - Pointer to the IBTExtAVRCP object
  uTxnID             - Transaction ID given in command
  uStatus            - AVRCP op status; should be copied from the
                       incoming event.
  Evt                - The command event; should be copied from the
                       incoming event.
  Rsp                - Response to the command event; for most
                       implementations should be ACCEPTED or
                       NOT IMPLEMENTED.


Return value:

  SUCCESS indicates that the reply to pass through command
  has been sent successfully.

  All other values indicate failure.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtAVRCP object
    EFAILED   - internal error

Event:

  None.

=======================================================================

Function: IBTEXTAVRCP_SendGenericReply()

Description: 

   Sends the reply to a non pass through or vendor specific
   pass through command send by the AVRCP controller.

   A response should be sent immediately upon receiving the notification
   that the command has arrived.

   For an unknown generic op, the response sent should be the same
   as the command, but the pFrame->cmd value should be set to
   AEEBT_AVRCP_RESPONSE_NOT_IMPLEMENTED.

Prototype:

  int  IBTEXTAVRCP_SendGenericReply ( IBTExtAVRCP*               pParent,
                                      AEEBTAVRCPGenericOpType*   pFrame);
  
Parameters:

  pParent            - Pointer to the IBTExtAVRCP object
  pFrame             - Pointer to the AEEBTAVRCPGenericOpType object
                       Note: Most fields should be copied from the
                       incoming event. Only the operand_count
                       field need to be filled in with app supplied
                       data. operand_count is used as the length for
                       the pRsp data.
  pRsp               - Data to be held in the operand response to the
                       AVRCP controller. Size of this data will be
                       taken from the "operand_count" field in pFrame.

Return value:

  SUCCESS indicates that the reply to non pass through command or vender
  specific pass through command has been sent successfully.

  All other values indicate failure.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtAVRCP object
    EFAILED   - internal error

Event:

  None.


=======================================================================

Function: IBTEXTAVRCP_GetGenericOp()

Description: 

   Gets the data associated with a non pass through or vendor specific
   pass through command send by the AVRCP controller. This function should
   be called exactly once for each generic operation.

Prototype:

  int  IBTEXTAVRCP_GetGenericOp ( IBTExtAVRCP*               pParent,
                                  AEEBTAVRCPGenericOpType*   pFrame);
  
Parameters:

  pParent            - Pointer to the IBTExtAVRCP object
  pBuf               - Pointer to App allocated data to hold command
  nSize              - must be at least the size of the command. The
                       command size is passed in the OP_GENERIC event.

Return value:

  SUCCESS indicates that the data has been copied successfully.

  All other values indicate failure.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtAVRCP object
    EFAILED   - internal error
    EFAILED   - no data to read

Event:

  None.


  =======================================================================*/
#endif /* _AEE_BT_EXT_AVRCP_H_ */

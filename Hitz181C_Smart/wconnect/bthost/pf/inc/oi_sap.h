#ifndef _OI_SAP_H
#define _OI_SAP_H

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/oi_sap.h#1 $

  $DateTime: 2009/01/07 18:14:54 $

  $Log: $
*
*    #3        21 JUL 2006            BK
*  After changes to SAP 1.0.
===========================================================================*/
/**
 * @file
 * common definitions for the SIM Access Profile (SAP)
 */

/** \addtogroup SAP SAP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_common.h"



/** message IDs */
enum {
    OI_SAP_CONNECT_REQ = 0x00,
    OI_SAP_CONNECT_RESP = 0x01,
    OI_SAP_DISCONNECT_REQ = 0x02,
    OI_SAP_DISCONNECT_RESP = 0x03,
    OI_SAP_DISCONNECT_IND = 0x04,
    OI_SAP_TRANSFER_APDU_REQ = 0x05,
    OI_SAP_TRANSFER_APDU_RESP = 0x06,
    OI_SAP_TRANSFER_ATR_REQ = 0x07,
    OI_SAP_TRANSFER_ATR_RESP = 0x08,
    OI_SAP_POWER_SIM_OFF_REQ = 0x09,
    OI_SAP_POWER_SIM_OFF_RESP = 0x0A,
    OI_SAP_POWER_SIM_ON_REQ = 0x0B,
    OI_SAP_POWER_SIM_ON_RESP = 0x0C,
    OI_SAP_RESET_SIM_REQ = 0x0D,
    OI_SAP_RESET_SIM_RESP = 0x0E,
    OI_SAP_CARD_READER_STATUS_REQ = 0x0F,
    OI_SAP_CARD_READER_STATUS_RESP = 0x10,
    OI_SAP_STATUS_IND = 0x11,
    OI_SAP_ERROR_RESP = 0x12,
    OI_SAP_SET_TRANSPORT_PROTOCOL_REQ = 0x13,
    OI_SAP_SET_TRANSPORT_PROTOCOL_RESP = 0x14
};

/** connection status */
enum {
    OI_SAP_STATUS_OK = 0x00,
    OI_SAP_STATUS_CONNECT_FAIL = 0x01,
    OI_SAP_STATUS_MSG_SIZE_TOO_LARGE = 0x02,
    OI_SAP_STATUS_MSG_SIZE_TOO_SMALL = 0x03
};

/** disconnect type */
enum {
    OI_SAP_DISCONNECT_GRACEFUL = 0x00,
    OI_SAP_DISCONNECT_IMMEDIATE = 0x01
};

/** result codes */
enum {
    OI_SAP_RESULT_OK = 0x00,
    OI_SAP_RESULT_ERROR_NO_REASON = 0x01,
    OI_SAP_RESULT_CARD_NOT_ACCESSIBLE = 0x02,
    OI_SAP_RESULT_CARD_ALREADY_OFF = 0x03,
    OI_SAP_RESULT_CARD_REMOVED = 0x04,
    OI_SAP_RESULT_CARD_ALREADY_ON = 0x05,
    OI_SAP_RESULT_DATA_NOT_AVAILABLE = 0x06,
    OI_SAP_RESULT_NOT_SUPPORTED = 0x07
};

/** status change codes */
enum {
    OI_SAP_STATUS_CHANGE_UNKNOWN_ERROR = 0x00,
    OI_SAP_STATUS_CHANGE_CARD_RESET = 0x01,
    OI_SAP_STATUS_CHANGE_CARD_NOT_ACCESSIBLE = 0x02,
    OI_SAP_STATUS_CHANGE_CARD_REMOVED = 0x03,
    OI_SAP_STATUS_CHANGE_CARD_INSERTED = 0x04,
    OI_SAP_STATUS_CHANGE_CARD_RECOVERED = 0x05
};

/** transport protocols */
enum {
    OI_SAP_TRANSPORT_PROTOCOL_T0 = 0x00,
    OI_SAP_TRANSPORT_PROTOCOL_T1 = 0x01
};

/** command apdu types */
enum {
    OI_SAP_COMMAND_APDU = 0x04,
    OI_SAP_COMMAND_APDU_7816 = 0x10
};


typedef struct {
    
    /** the message ID */
    OI_UINT8 id;

    union {
        
        struct {
            OI_UINT16 maxMsgSize;
        } connectReq;
        
        struct {
            OI_UINT8 status;
            OI_UINT16 maxMsgSize;
        } connectResp;

        /* DisconnectReq - no parameters */
        
        /* DisconnectResp - no parameters */
        
        struct {
            OI_UINT8 disconnectType;
        } disconnectInd;
        
        struct {
            OI_UINT8 apduType;
            OI_BYTE *data;
            OI_UINT16 dataLen;
        } APDUReq;

        struct {
            OI_UINT8 result;
            OI_BYTE *data;
            OI_UINT16 dataLen;
        } APDUResp;

        /* TransferATRReq - no parameters */

        struct {
            OI_UINT8 result;
            OI_BYTE *data;
            OI_UINT16 dataLen;
        } ATRResp;

        /* PowerSimOffReq - no parameters */
        
        struct {
            OI_UINT8 result;
        } powerSIMOffResp;

        /* PowerSimOnReq - no parameters */

        struct {
            OI_UINT8 result;
        } powerSIMOnResp;

        /* ResetSimReq - no parameters */

        struct {
            OI_UINT8 result;
        } resetSIMResp;

        struct {
            OI_UINT8 status;
        } statusInd;

        /* TransferCardReaderStatusReq - no parameters */

        struct {
            OI_UINT8 result;
            OI_UINT8 cardReaderStatus;
        } cardReaderStatusResp;

        /* ErrorResp - no parameters */
        
        struct {
            OI_UINT8 transportProtocol;
        } setTransportProtocolReq;

        struct {
            OI_UINT8 result;
        } setTransportProtocolResp;
        
    } data;

} OI_SAP_MSG;

/** Minimum frame size - must be large enough for message header with no parameters */
#define SAP_MIN_FRAME_SIZE  4   


/**@}*/
#endif /* _OI_SAP_H */

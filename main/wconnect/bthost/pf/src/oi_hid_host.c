/**
 * @file
 * @internal  
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_HID

#include "oi_common.h"
#include "oi_argcheck.h"
#include "oi_memmgr.h"
#include "oi_dispatch.h"
#include "oi_bytestream.h"
#include "oi_l2cap.h"
#include "oi_bt_assigned_nos.h"
#include "oi_hid_host.h"
#include "oi_hid_endpoint.h"
#include "oi_hid_consts.h"
#include "oi_init_flags.h"


#ifndef OI_CODE
#define BT_MSG_LAYER  BT_MSG_GN
#endif

/*
 * How long to wait for a transaction to complete.
 */
#define TRANSACTION_TIMEOUT  OI_SECONDS(3)


static OI_HID_HOST_CALLBACKS const *hostCallbacks;


static void HidEndpointConnectInd(OI_BD_ADDR const *addr, HID_CONNECTION_HANDLE handle)
{
    hostCallbacks->connectInd(addr, handle);
}

static void HidEndpointConnectCfm(HID_CONNECTION_HANDLE handle, OI_STATUS status)
{
    hostCallbacks->connectCfm(handle, status);
}

static void HidEndpointDisconnectInd(HID_CONNECTION_HANDLE handle, OI_STATUS reason)
{
    hostCallbacks->disconnectInd(handle, reason);
}


static void HidEndpointRecvControlDataInd(HID_CONNECTION_HANDLE handle,
                                          OI_BYTE transType,
                                          OI_BYTE transParam,
                                          OI_BYTE *data,
                                          OI_UINT16 len,
                                          OI_BOOL final)
{
    HID_CONNECTION *conn;
    OI_BOOL first;
    OI_BYTE previousTransType;

    /*    OI_DBGPRINT(("HidEndpointRecvControlDataInd (%d,%d)", transType, transParam));
          OI_DBGPRINT2(("%@", data, len));*/

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        OI_DBGPRINT(("RecvControlDataInd on unknown cid. Ignoring."));
        return;
    }

    /*
     * Got a response so cancel the timeout.
     */
    OI_HIDEndpoint_CancelTransactionTimeout(handle);

    /*
     * Accept an unplug whatever the previous transaction type was.
     */
    if ((transType == OI_HID_CONTROL) && (transParam == OI_HID_CONTROL_VIRTUAL_CABLE_UNPLUG)) {
        OI_DBGPRINT(("Remote device ungplugged virtual cable"));
        hostCallbacks->unpluggedInd(handle);
        return;
    }

    previousTransType = conn->control.transType;
    conn->control.transType = OI_HID_NO_TRANSACTION;

    switch (previousTransType) {
        case OI_HID_GET_REPORT:
            switch (transType) {
                case OI_HID_DATA:
                case OI_HID_DATC:
                    first = (transType == OI_HID_DATA);
                    transParam &= OI_HID_REPORT_TYPE_MASK;
                    if (hostCallbacks->getReportCfm) {
                    hostCallbacks->getReportCfm(handle, OI_HID_HANDSHAKE_SUCCESSFUL, transParam, data, len, first, final);
                    }
                    if (!final) {
                        conn->control.transType = OI_HID_GET_REPORT;
                    }
                    break;
                case OI_HID_HANDSHAKE:
                    if (transParam == OI_HID_HANDSHAKE_SUCCESSFUL) {
                        OI_DBGPRINT(("Host sent GET_REPORT, but device replied with HANDSHAKE_SUCCESS"));
                        transParam = OI_HID_HANDSHAKE_ERR_UNKNOWN; /* A successful GET_REPORT response should be DATA/DATC,
                                                                      not a handshake */
                    }
                    if (hostCallbacks->getReportCfm) {
                    hostCallbacks->getReportCfm(handle, transParam, 0, NULL, 0, TRUE, TRUE);
                    }
                    break;
                default:
                    if (hostCallbacks->getReportCfm) {
                    hostCallbacks->getReportCfm(handle, OI_HID_HANDSHAKE_ERR_UNKNOWN, 0, NULL, 0, TRUE, TRUE);
                    }
                    break;

            }
            break;

        case OI_HID_SET_REPORT:
            if (transType != OI_HID_HANDSHAKE) {
                transType = OI_HID_HANDSHAKE;
                transParam = OI_HID_HANDSHAKE_ERR_FATAL;
            }
            if (hostCallbacks->setReportCfm) {
            hostCallbacks->setReportCfm(handle, transParam, conn->control.clientDataPtr, conn->control.clientDataLen, OI_OK);
            }
            break;

        case OI_HID_GET_PROTOCOL:
            if (transType == OI_HID_DATA && len == 1) {
                conn->protocol = data[0];

                if (hostCallbacks->getProtocolCfm) {
                hostCallbacks->getProtocolCfm(handle, data[0], OI_HID_HANDSHAKE_SUCCESSFUL, OI_OK);
                }
            } else if (transType == OI_HID_HANDSHAKE) {
                if (hostCallbacks->getProtocolCfm) {
                hostCallbacks->getProtocolCfm(handle, 0, transParam, OI_OK);
                }
            } else {
                if (hostCallbacks->getProtocolCfm) {
                hostCallbacks->getProtocolCfm(handle, 0, OI_HID_HANDSHAKE_ERR_FATAL, OI_STATUS_INVALID_PACKET);
            }
            }
            break;

        case OI_HID_SET_PROTOCOL:
            if (transType != OI_HID_HANDSHAKE) {
                transType = OI_HID_HANDSHAKE;
                transParam = OI_HID_HANDSHAKE_ERR_FATAL;
                conn->protocol = OI_HID_PROTOCOL_REPORT;
            } else {
                conn->protocol = conn->pendingProtocol;
            }
            if (hostCallbacks->setProtocolCfm) {
            hostCallbacks->setProtocolCfm(handle, transParam, OI_OK);
            }
            break;

        case OI_HID_GET_IDLE:
            if (transType == OI_HID_DATA && len == 1) {
                if (hostCallbacks->getIdleCfm) {
                hostCallbacks->getIdleCfm(handle, data[0], OI_HID_HANDSHAKE_SUCCESSFUL, OI_OK);
                }
            } else if (transType == OI_HID_HANDSHAKE) {
                if (hostCallbacks->getIdleCfm) {
                hostCallbacks->getIdleCfm(handle, 0, transParam, OI_OK);
                }
            } else {
                if (hostCallbacks->getIdleCfm) {
                hostCallbacks->getIdleCfm(handle, 0, OI_HID_HANDSHAKE_ERR_FATAL, OI_STATUS_INVALID_PACKET);
            }
            }
            break;

        case OI_HID_SET_IDLE:
            if (transType != OI_HID_HANDSHAKE) {
                transType = OI_HID_HANDSHAKE;
                transParam = OI_HID_HANDSHAKE_ERR_FATAL;
            }
            if (hostCallbacks->setIdleCfm) {
            hostCallbacks->setIdleCfm(handle, transParam, OI_OK);
            }
            break;

        default:
            OI_LOG_ERROR(("No transactions outstanding, ignoring packet from device"));
            break;

    }
}


static void HidEndpointRecvInterruptDataInd(HID_CONNECTION_HANDLE handle,
                                            OI_BYTE transType,
                                            OI_BYTE transParam,
                                            OI_BYTE *data,
                                            OI_UINT16 len,
                                            OI_BOOL final)
{
    OI_BOOL first;
    HID_CONNECTION *conn;

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        OI_LOG_ERROR(("Unknown connection. Ignoring interrupt data."));
        return;
    }

    if (transType != OI_HID_DATA && transType != OI_HID_DATC) {
        OI_LOG_ERROR(("Received non DATA/DATC transaction on interrupt endpoint. Ignoring."));
        return;
    }

    if (transParam != OI_HID_REPORT_INPUT) {
        OI_LOG_ERROR(("Received non-input report type on interrupt endpoint. Ignoring."));
        return;
    }

    first = (transType == OI_HID_DATA);

    if (conn->protocol == OI_HID_PROTOCOL_BOOT) {
        if (len < 1) {
            OI_LOG_ERROR(("Received empty boot report. Ignoring."));
            return;
        }
        if (!first || !final) {
            OI_LOG_ERROR(("Received segmented boot report. Ignoring."));
            return;
        }
        switch (data[0]) {
            case OI_HIDBOOT_KEYBOARD_REPORT_ID:
                if (len < OI_HIDBOOT_KEYBOARD_PACKET_LEN) {
                    OI_LOG_ERROR(("Boot-mode keyboard report too short. Ignoring."));
                    return;
                }
                if (hostCallbacks->bootKeyboardInd) {
                    hostCallbacks->bootKeyboardInd(handle, data[1], &data[3]); 
                } else {
                    OI_DBGPRINT2(("Ignoring unwanted boot-mode keyboard report."));
                }
                break;
            case OI_HIDBOOT_MOUSE_REPORT_ID:
                if (len < OI_HIDBOOT_MOUSE_PACKET_LEN) {
                    OI_LOG_ERROR(("Boot-mode mouse report too short. Ignoring."));
                    return;
                }
                if (hostCallbacks->bootMouseInd) {
                    hostCallbacks->bootMouseInd(handle, data[1], data[2], data[3]);
                } else {
                    OI_DBGPRINT2(("Ignoring unwanted boot-mode mouse report."));
                }
                break;
            default:
                OI_LOG_ERROR(("Received unknown boot protocol report ID %d", data[0]));
                return;
        }
    } else {
        if (hostCallbacks->inputReportInd) {
            hostCallbacks->inputReportInd(handle, data, len, first, final);
        } else {
            OI_LOG_ERROR(("No report-protocol handler present. Ignoring."));
        }
    }
}

static void TransactionTimeoutCB(HID_CONNECTION *conn);

static const HID_ENDPOINT_CALLBACKS endpointCallbacks = {
    HidEndpointConnectInd,
    HidEndpointConnectCfm,
    HidEndpointDisconnectInd,
    HidEndpointRecvControlDataInd,
    HidEndpointRecvInterruptDataInd,
    TransactionTimeoutCB
};

OI_STATUS OI_HIDHost_Register(OI_HID_HOST_CONFIG const *config, OI_HID_HOST_CALLBACKS const *cb)
{
    OI_STATUS status;
    HID_THIS *this;

    ARGCHECK(config != NULL);
    ARGCHECK(cb != NULL);
    ARGCHECK(cb->connectInd && cb->connectCfm && cb->disconnectInd && cb->unpluggedInd);

    status = OI_HIDEndpoint_Register(&endpointCallbacks, config->maxConnections, &this);
    if (OI_SUCCESS(status)) {
        hostCallbacks = cb;
    }

    return status;
}

OI_STATUS OI_HIDHost_Deregister(void)
{
    return OI_HIDEndpoint_Deregister();
}
#ifndef OI_CODE
OI_STATUS OI_HIDHost_Connect(OI_BD_ADDR *addr, OI_HID_CONNECTION_PARAMS const *params, OI_HID_HOST_HANDLE *handle)
#else
#error code not present
#endif
{
    /*
     * Cannot connect to a HID device unless the HID host service has been
     * started.
     */
    if (!OI_INIT_FLAG_VALUE(HID)) {
        return OI_HID_HOST_SERVICE_NOT_STARTED;
    }
    return OI_HIDEndpoint_Connect(addr, params, handle);
}

OI_STATUS OI_HIDHost_Accept(OI_HID_HOST_HANDLE handle, OI_HID_CONNECTION_PARAMS const *params, OI_BOOL allow)
{
    return OI_HIDEndpoint_Accept(handle, params, allow);
}

OI_STATUS OI_HIDHost_Disconnect(OI_HID_HOST_HANDLE handle)
{
    return OI_HIDEndpoint_Disconnect(handle);
}


static void TransactionTimeoutCB(HID_CONNECTION *conn)
{
    HID_CHANNEL *chan;
    OI_HID_HOST_HANDLE handle;
    OI_BYTE previousTransType;

    OI_LOG_ERROR(("Didn't get transaction response within timeout period"));

    if (conn == NULL) {
        OI_LOG_ERROR(("Invalid HID host handle in transaction timeout"));
        return;
    }
    handle = GET_HANDLE(conn);
    conn->timeoutCB = 0;

    previousTransType = conn->control.transType;
    conn->control.transType = OI_HID_NO_TRANSACTION;

    switch (previousTransType) {
        case OI_HID_DATA:
            chan = OI_HIDEndpoint_GetChannel(handle, OI_PSM_HID_INTERRUPT);
            if (chan) {
                if (hostCallbacks->outputReportCfm) {
                hostCallbacks->outputReportCfm(handle, chan->clientDataPtr, chan->clientDataLen, OI_TIMEOUT);
            }
            }
            break;
        case OI_HID_CONTROL:
            OI_DBGPRINT(("No response to control command - disconnecting"));
            OI_HIDEndpoint_Disconnect(handle);
            break;
        case OI_HID_GET_REPORT:
            if (hostCallbacks->getReportCfm) {
            hostCallbacks->getReportCfm(handle, OI_HID_HANDSHAKE_ERR_FATAL, 0, NULL, 0, TRUE, TRUE);
            }
            break;
        case OI_HID_SET_REPORT:
            if (hostCallbacks->setReportCfm) {
            hostCallbacks->setReportCfm(handle, OI_HID_HANDSHAKE_ERR_FATAL, 0, 0, OI_TIMEOUT);
            }
            break;
        case OI_HID_GET_PROTOCOL:
            if (hostCallbacks->getProtocolCfm) {
            hostCallbacks->getProtocolCfm(handle, 0, OI_HID_HANDSHAKE_ERR_FATAL, OI_TIMEOUT);
            }
            break;
        case OI_HID_SET_PROTOCOL:
            if (hostCallbacks->setProtocolCfm) {
            hostCallbacks->setProtocolCfm(handle, OI_HID_HANDSHAKE_ERR_FATAL, OI_TIMEOUT);
            }
            break;
        case OI_HID_GET_IDLE:
            if (hostCallbacks->getIdleCfm) {
            hostCallbacks->getIdleCfm(handle, 0, OI_HID_HANDSHAKE_ERR_FATAL, OI_TIMEOUT);
            }
            break;
        case OI_HID_SET_IDLE:
            if (hostCallbacks->setIdleCfm) {
            hostCallbacks->setIdleCfm(handle, OI_HID_HANDSHAKE_ERR_FATAL, OI_TIMEOUT);
            }
            break;
        default:
            OI_HIDEndpoint_Disconnect(handle);
            OI_LOG_ERROR(("No transactions outstanding, disconnecting"));
            break;
    }

}


OI_STATUS OI_HIDHost_SendHidControl(OI_HID_HOST_HANDLE handle,
                                    OI_BYTE controlOp)
{
    OI_STATUS status;
    HID_CONNECTION *conn;

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }
    if (conn->control.transType != OI_HID_NO_TRANSACTION) {
        return OI_BUSY_FAIL;
    }
    status = OI_HIDEndpoint_SendSimple(handle, OI_PSM_HID_CONTROL, OI_HID_CONTROL, controlOp);
    if (OI_SUCCESS(status)) {
        OI_HIDEndpoint_SetTransactionTimeout(handle);
    } else {
        conn->control.transType = OI_HID_NO_TRANSACTION;
    }
    return status;
}


OI_STATUS OI_HIDHost_GetReport(OI_HID_HOST_HANDLE handle,
                               OI_BYTE reportType,
                               OI_INT reportID,
                               OI_UINT16 maxBytes)
{
    OI_BYTE request[HID_REQUEST_MAX_LEN];
    OI_UINT16 requestLen;
    OI_BYTE_STREAM bs;
    OI_MBUF *buf;
    HID_CONNECTION *conn;
    OI_STATUS status;

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }

    ByteStream_Init(bs, request, OI_ARRAYSIZE(request));
    ByteStream_Open(bs, BYTESTREAM_WRITE);

    if (reportID != OI_HID_REPORT_ID_DEFAULT) {
        ByteStream_PutUINT8(bs, (OI_UINT8)reportID);
    }

    reportType &= OI_HID_REPORT_TYPE_MASK;
    if (maxBytes != OI_HID_REPORT_MAX_LENGTH) {
        ByteStream_PutUINT16(bs, maxBytes, OI_HID_BYTE_ORDER);
        reportType |= OI_HID_REPORT_BUFFERSIZE_PRESENT;
    }

    requestLen = ByteStream_GetPos(bs);

    buf = OI_MBUF_Wrap(request, requestLen, MBUF_COPY);
    if (buf == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    if (conn->control.transType != OI_HID_NO_TRANSACTION) {
        return OI_BUSY_FAIL;
    }
    conn->control.transType = OI_HID_GET_REPORT;

    status = OI_HIDEndpoint_SendMbuf(OI_HIDEndpoint_FreeMbufCb,
                                     handle,
                                     OI_PSM_HID_CONTROL,
                                     OI_HID_GET_REPORT,
                                     reportType,
                                     buf);

    if (OI_SUCCESS(status)) {
        OI_HIDEndpoint_SetTransactionTimeout(handle);
    } else {
        OI_MBUF_Free(buf);
        conn->control.transType = OI_HID_NO_TRANSACTION;
    }

    return status;
}


static void SetReportSendDataCfm(HID_CHANNEL *chan,
                                 OI_STATUS status)
{
    /* Nothing to do? Look into setting a timeout. */
}


OI_STATUS OI_HIDHost_SetReport(OI_HID_HOST_HANDLE handle,
                               OI_BYTE reportType,
                               OI_BYTE const *data,
                               OI_UINT16 len)
{
    HID_CONNECTION *conn;
    OI_STATUS status;

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }
    if (conn->control.transType != OI_HID_NO_TRANSACTION) {
        return OI_BUSY_FAIL;
    }
    conn->control.transType = OI_HID_SET_REPORT;

    status = OI_HIDEndpoint_SendData(SetReportSendDataCfm,
                                     handle,
                                     OI_PSM_HID_CONTROL,
                                     OI_HID_SET_REPORT,
                                     reportType,
                                     data,
                                     len);

    if (OI_SUCCESS(status)) {
        OI_HIDEndpoint_SetTransactionTimeout(handle);
    } else {
        conn->control.transType = OI_HID_NO_TRANSACTION;
    }

    return status;
}


OI_STATUS OI_HIDHost_GetProtocol(OI_HID_HOST_HANDLE handle)
{
    HID_CONNECTION *conn;
    OI_STATUS status;

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }
    if (conn->control.transType != OI_HID_NO_TRANSACTION) {
        return OI_BUSY_FAIL;
    }
    conn->control.transType = OI_HID_GET_PROTOCOL;

    status = OI_HIDEndpoint_SendSimple(handle, OI_PSM_HID_CONTROL, OI_HID_GET_PROTOCOL, 0);

    if (OI_SUCCESS(status)) {
        OI_HIDEndpoint_SetTransactionTimeout(handle);
    } else {
        conn->control.transType = OI_HID_NO_TRANSACTION;
    }

    return status;
}


OI_STATUS OI_HIDHost_SetProtocol(OI_HID_HOST_HANDLE handle,
                                 OI_BYTE protocol)
{
    HID_CONNECTION *conn;
    OI_STATUS status;

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }
    if (conn->control.transType != OI_HID_NO_TRANSACTION) {
        return OI_BUSY_FAIL;
    }
    conn->control.transType = OI_HID_SET_PROTOCOL;
    conn->pendingProtocol = protocol;

    status = OI_HIDEndpoint_SendSimple(handle, OI_PSM_HID_CONTROL, OI_HID_SET_PROTOCOL, protocol);

    if (OI_SUCCESS(status)) {
        OI_HIDEndpoint_SetTransactionTimeout(handle);
    } else {
        conn->control.transType = OI_HID_NO_TRANSACTION;
    }

    return status;
}


OI_STATUS OI_HIDHost_GetIdle(OI_HID_HOST_HANDLE handle)
{
    HID_CONNECTION *conn;
    OI_STATUS status;

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }
    if (conn->control.transType != OI_HID_NO_TRANSACTION) {
        return OI_BUSY_FAIL;
    }
    conn->control.transType = OI_HID_GET_IDLE;

    status = OI_HIDEndpoint_SendSimple(handle, OI_PSM_HID_CONTROL, OI_HID_GET_IDLE, 0);

    if (OI_SUCCESS(status)) {
        OI_HIDEndpoint_SetTransactionTimeout(handle);
    } else {
        conn->control.transType = OI_HID_NO_TRANSACTION;
    }

    return status;
}


OI_STATUS OI_HIDHost_SetIdle(OI_HID_HOST_HANDLE handle,
                             OI_BYTE idleRate)
{
    HID_CONNECTION *conn;
    OI_STATUS status;

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }
    if (conn->control.transType != OI_HID_NO_TRANSACTION) {
        return OI_BUSY_FAIL;
    }
    conn->control.transType = OI_HID_SET_IDLE;

    status = OI_HIDEndpoint_SendByte(handle, OI_PSM_HID_CONTROL, OI_HID_SET_IDLE, 0, idleRate);

    if (OI_SUCCESS(status)) {
        OI_HIDEndpoint_SetTransactionTimeout(handle);
    } else {
        conn->control.transType = OI_HID_NO_TRANSACTION;
    }

    return status;
}


static void OutputReportSendDataCfm(HID_CHANNEL *chan,
                                    OI_STATUS status)
{
    HID_CONNECTION *conn;

    conn = OI_HIDEndpoint_GetConnectionByCid(chan->cid);
    if (conn == NULL) {
        OI_LOG_ERROR(("Couldn't get endpoint handle in OutputReportSendDataCfm"));
        return;
    }

    if (hostCallbacks->outputReportCfm) {
    hostCallbacks->outputReportCfm(GET_HANDLE(conn), chan->clientDataPtr, chan->clientDataLen, status);
    }
}


OI_STATUS OI_HIDHost_SendOutputReport(OI_HID_HOST_HANDLE handle,
                                      OI_BYTE reportType,
                                      OI_BYTE const *data,
                                      OI_UINT16 len)
{
    HID_CONNECTION *conn;
    HID_CHANNEL *chan;
    OI_STATUS status;

    chan = OI_HIDEndpoint_GetChannel(handle, OI_PSM_HID_INTERRUPT);
    if (chan == NULL) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    conn = OI_HIDEndpoint_GetConnectionByCid(chan->cid);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }
    if (conn->control.transType != OI_HID_NO_TRANSACTION) {
        return OI_BUSY_FAIL;
    }

    status =  OI_HIDEndpoint_SendData(OutputReportSendDataCfm,
                                      handle,
                                      OI_PSM_HID_INTERRUPT,
                                      OI_HID_DATA,
                                      reportType,
                                      data,
                                      len);
    if (OI_SUCCESS(status)) {
        chan->clientDataPtr = data;
        chan->clientDataLen = len;
    } else {
        conn->control.transType = OI_HID_NO_TRANSACTION;
        chan->clientDataPtr = NULL;
        chan->clientDataLen = 0;
    }

    return status;
}


OI_STATUS OI_HIDHost_SetKeyboardLEDs(OI_HID_HOST_HANDLE handle,
                                     OI_BYTE leds)
{
    OI_STATUS status;
    HID_CONNECTION *conn;

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }
    if (conn->control.transType != OI_HID_NO_TRANSACTION) {
        return OI_BUSY_FAIL;
    }

    if (conn->protocol != OI_HID_PROTOCOL_BOOT) {
        OI_LOG_ERROR(("Attempt to send boot-mode report without setting device to boot mode."));
        return OI_STATUS_INVALID_STATE;
    }

    status = OI_HIDHost_SendOutputReport(handle,                /* OI_HID_HOST_HANDLE handle, */
                                         OI_HID_REPORT_OUTPUT,  /* OI_BYTE reportType, */
                                         &leds,                 /* OI_BYTE const *data, */
                                         1);                    /* OI_UINT16 len */

    return status;
}


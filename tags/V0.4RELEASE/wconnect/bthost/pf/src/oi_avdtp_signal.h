#ifdef FEATURE_BT_EXTPF_AV
/**
@file
@internal

This file provides the interface for assembly, fragmentation, and queuing of AVDTP 
command and response messages.
*/

/** \addtogroup AVS_Internal */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _AVDTP_SIGNAL_H
#define _AVDTP_SIGNAL_H

#include "oi_avdtp.h"
#include "oi_avdtp_private.h"
#include "oi_core_common.h"
#include "oi_l2cap.h"

#define AVDTP_CMD_HEADER_LEN 2
#define AVDTP_RSP_HEADER_LEN 3

/* capability lengths */
#define AVDTP_CAP_CODEC_LEN  2 /**< This length does not include codec-specific elements. */

/** This function assembles and then sends a generic AVDTP request
    on the specified signalling channel.
    @param cid          channel identifier of signalling channel
    @param signalID     signal identifier for the request to be sent
    @param transaction  transaction label
    @param seid         stream endpoint identifier to which the request is to be sent
 */
OI_STATUS AVDTP_GenericReq(OI_L2CAP_CID cid,
                           OI_UINT8 signalID,
                           OI_UINT8 transaction,
                           OI_UINT8 seid);

/** This function assembles and then sends a generic AVDTP response
    on the specified signalling channel. 
    @param cid          channel identifier of signalling channel
    @param signalID     signal identifier for the request to be sent
    @param transaction  transaction label
    @param error        error code
*/
OI_STATUS AVDTP_GenericRsp(OI_L2CAP_CID cid,
                           OI_UINT8 signalID,
                           OI_UINT8 transaction,
                           OI_UINT8 error);

/** Send a general rejection for an unsupported type */
OI_STATUS AVDTP_GeneralReject(OI_L2CAP_CID cid,
                              OI_UINT8 transaction);

/** This function assembles and then sends a response containing capabilities 
    information, in response to either an AVDTP_GET_CAPS or an AVDTP_GET_CONFIG commands. 
    @param cid          channel identifier of signalling channel
    @param command      either AVDTP_GET_CAPS or AVDTP_GET_CONFIG
    @param transaction  transaction label
    @param caps         pointer to a list of capabilities
    @param numCaps      number of capabilites in the list
    @param error        error code
*/
OI_STATUS AVDTPSignal_GenericCapsRsp(OI_L2CAP_CID cid,
                                     OI_UINT8 command,
                                     OI_UINT8 transaction,
                                     OI_AVDTP_CAP *caps,
                                     OI_UINT8 numCaps,
                                     OI_UINT8 error);
/** 
 * This function assembles and then sends a generic configuration response,
 * in response to either an AVDTP_SET_CONFIG or an AVDTP_RECONFIGURE command.
 * @param cid          channel identifier of signalling channel.
 * @param command      the command type
 * @param transaction  transaction label
 * @param category     service category
 * @param error        error code
 */
OI_STATUS AVDTPSignal_ConfigRsp(OI_L2CAP_CID cid,
                                OI_UINT8 command,
                                OI_UINT8 transaction,
                                OI_UINT8 category,
                                OI_UINT8 error);

/**
 * This function assembles and then sends AVDTP requests to multiple stream endpoints
 * on the specified signalling channel. 
 * @param cid          channel identifier of signalling channel
 * @param signalID     signal identifier
 * @param transaction  transaction label
 * @param ids          pointer to an array of stream endpoint IDs
 * @param num          the number of stream endpoint IDs in the array
 */
OI_STATUS AVDTP_MultipleReq(OI_L2CAP_CID cid,
                            OI_UINT8 signalID,
                            OI_UINT8 transaction,
                            OI_UINT8 *ids,
                            OI_UINT8 num);
   
/**
 * This function assembles and then sends an AVDTP response to a command sent 
 * to multiple stream endpoints on the specified signalling channel. 
 * @param cid          channel identifier of signalling channel
 * @param signalID     signal identifier
 * @param transaction  transaction label
 * @param seid         If an error occurred, this is the stream endpoint identifier 
 *                     of the first endpoint to cause an error.
 * @param error        error code
 */
OI_STATUS AVDTP_MultipleRsp(OI_L2CAP_CID cid,
                            OI_UINT8 signalID,
                            OI_UINT8 transaction,
                            OI_UINT8 seid,
                            OI_UINT8 error);

/** 
 * This function assembles and then sends a response to an AVDTP_DISCOVER 
 * message with a list of stream endpoints associated with a particular signalling
 * channel.
 * @param cid          channel identifier of signalling channel
 * @param transaction  transaction label
 */
OI_STATUS AVDTPSignal_Discover(OI_L2CAP_CID cid,
                               OI_UINT8 transaction);

/** 
 * This function responds to an AVDTP_DISCOVER message with a list of stream endpoints.
 *
 * @param cid          channel identifier of signalling channel
 * @param transaction  transaction label
 * @param array        pointer to an array of endpoint records
 * @param arrayLen     the number of records in the array
 * @param error        error code
 */
OI_STATUS AVDTPSignal_DiscoverRsp(OI_L2CAP_CID cid,
                                  OI_UINT8 transaction,
                                  OI_AVDTP_DISCOVER_RSP_INFO *array, 
                                  OI_UINT16 arrayLen,
                                  OI_UINT8 error);

/** This macro uses the AVDTP_GenericReq() function to provide the functionality to 
    send the AVDTP_GET_CAPABILITIES command. */
#define AVDTPSignal_GetCap(cid, transaction, seid)      AVDTP_GenericReq((cid), AVDTP_GET_CAPS, (transaction), (seid))

/** This macro uses the AVDTPSignal_GenericCapsRsp() function to provide the functionality to 
    send the response to an AVDTP_GET_CAPABILITIES command. */
#define AVDTPSignal_GetCapRsp(cid, transaction, caps, numCaps, error) AVDTPSignal_GenericCapsRsp((cid), AVDTP_GET_CAPS, (transaction), (caps), (numCaps), (error))

/** 
 * This function assembles and then sends an AVDTP_SET_CONFIG command. 
 * @param cid           channel identifier of signalling channel
 * @param transaction   transaction label
 * @param remoteSEID    stream endpoint identifier of remote endpoint
 * @param localSEID     stream endpoint identifier of local endpoint
 * @param caps          pointer to a list of capabilities
 * @param numCaps       number of capabilities in the list
 */
OI_STATUS AVDTPSignal_SetConfig(OI_L2CAP_CID cid,
                          OI_UINT8 transaction,
                          OI_UINT8 remoteSEID,
                          OI_UINT8 localSEID,
                          OI_AVDTP_CAP *caps,
                          OI_UINT8 numCaps);

/** This macro uses the AVDTPSignal_ConfigRsp() function to provide the functionality to 
    send the response to an AVDTP_SET_CONFIG command. */
#define AVDTPSignal_SetConfigRsp(cid, transaction, category, error) \
 AVDTPSignal_ConfigRsp((cid), AVDTP_SET_CONFIG, (transaction), (category), (error))

/** This macro uses the AVDTP_GenericReq() function to provide the functionality to 
    send the AVDTP_GET_CONFIG command. */
#define AVDTPSignal_GetConfig(cid, transaction, seid)  AVDTP_GenericReq((cid), AVDTP_GET_CONFIG, (transaction), (seid))

/** This macro uses the AVDTPSignal_GenericCapsRsp() function to provide the functionality to 
    send the response to an AVDTP_GET_CONFIG command. */
#define AVDTPSignal_GetConfigRsp(cid, transaction, caps, numCaps, error) AVDTPSignal_GenericCapsRsp((cid), AVDTP_GET_CONFIG, (transaction), (caps), (numCaps), (error))

/*
OI_STATUS AVDTPSignal_GetConfigRsp(OI_L2CAP_CID cid,
                                   OI_UINT8 transaction,
                                   OI_AVDTP_CAP *caps,
                                   OI_UINT8 numCaps,
                                   OI_UINT8 error);
*/

/** 
 * This function assembles and then sends an AVDTP_AVDTP_RECONFIGURE command. 
 * @param cid           channel identifier of signalling channel
 * @param transaction   transaction label
 * @param seid          stream endpoint identifier
 * @param caps          pointer to a list of capabilities
 * @param numCaps       number of capabilities in the list
 */
OI_STATUS AVDTPSignal_Reconfigure(OI_L2CAP_CID cid,
                            OI_UINT8 transaction,
                            OI_UINT8 seid,
                            OI_AVDTP_CAP *caps,
                            OI_UINT8 numCaps);

/** This macro uses the AVDTPSignal_ConfigRsp() function to provide the functionality to 
    send the response to an AVDTP_RECONFIGURE command. */
#define AVDTPSignal_ReconfigureRsp(cid, transaction, category, error) \
AVDTPSignal_ConfigRsp((cid), AVDTP_RECONFIGURE, (transaction), (category), (error));

//Open
/** This macro uses the AVDTP_GenericReq() function to provide the functionality to 
    send the AVDTP_OPEN command. */
#define AVDTPSignal_Open(cid, transaction, seid)        AVDTP_GenericReq((cid), AVDTP_OPEN, (transaction), (seid))

/** This macro uses the AVDTP_GenericRsp() function to provide the functionality to 
    send the response to an AVDTP_OPEN command. */
#define AVDTPSignal_OpenRsp(cid, transaction, error)    AVDTP_GenericRsp((cid), AVDTP_OPEN, (transaction), (error))

//Start
/** This macro uses the AVDTP_MultipleReq() function to provide the functionality to 
    send the AVDTP_START command to multiple endpoints. */
#define AVDTPSignal_Start(cid, transaction, ids, num) \
  AVDTP_MultipleReq((cid), AVDTP_START, (transaction), (ids), (num))

/** This macro uses the AVDTP_MultipleRsp() function to provide the functionality to 
    send the response to an AVDTP_START command. */
#define AVDTPSignal_StartRsp(cid, transaction, seid, error) \
  AVDTP_MultipleRsp((cid), AVDTP_START, (transaction), (seid), (error))

//Close
/** This macro uses the AVDTP_GenericReq() function to provide the functionality to 
    send the AVDTP_CLOSE command. */
#define AVDTPSignal_Close(cid, transaction, seid)        AVDTP_GenericReq((cid), AVDTP_CLOSE, (transaction), (seid))

/** This macro uses the AVDTP_GenericRsp() function to provide the functionality to 
    send the response to an AVDTP_CLOSE command. */
#define AVDTPSignal_CloseRsp(cid, transaction, error)    AVDTP_GenericRsp((cid), AVDTP_CLOSE, (transaction), (error))

//Suspend
/** This macro uses the AVDTP_MultipleReq() function to provide the functionality to 
    send the AVDTP_SUSPEND command. */
#define AVDTPSignal_Suspend(cid, transaction, ids, num)  AVDTP_MultipleReq((cid), AVDTP_SUSPEND, (transaction), (ids), (num))

/** This macro uses the AVDTP_MultipleRsp() function to provide the functionality to 
    send the response to an AVDTP_SUSPEND command. */
#define AVDTPSignal_SuspendRsp(cid, transaction, seid, error)  AVDTP_MultipleRsp((cid), AVDTP_SUSPEND, (transaction), (seid), (error))

//Abort
/** This macro uses the AVDTP_GenericReq() function to provide the functionality to 
    send the AVDTP_ABORT command. */
#define AVDTPSignal_Abort(cid, transaction, seid)        AVDTP_GenericReq((cid), AVDTP_ABORT, (transaction), (seid))

/** This macro uses the AVDTP_GenericRsp() function to provide the functionality to 
    send the response to an AVDTP_ABORT command. */
#define AVDTPSignal_AbortRsp(cid, transaction, error)    AVDTP_GenericRsp((cid), AVDTP_ABORT, (transaction), (error))

//Security
/** 
 * This function assembles and then sends an AVDTP_SECURITY_CONTROL command on the
 * specified signalling channel.
 * @param cid           channel identifier of signalling channel
 * @param transaction   transaction label
 * @param seid          stream endpoint identifier
 * @param data          pointer to buffer
 * @param dataLen       length of data in buffer
 */
OI_STATUS AVDTPSignal_SecurityControl(OI_L2CAP_CID cid,
                                      OI_UINT8 transaction,
                                      OI_UINT8 seid,
                                      OI_BYTE *data,
                                      OI_UINT16 dataLen);
/** 
 * This function assembles and then sends a response to an AVDTP_SECURITY_CONTROL command.
 * @param cid           channel identifier of signalling channel
 * @param transaction   transaction label
 * @param data          pointer to buffer
 * @param dataLen       length of data in buffer
 * @param error         error code
 */
OI_STATUS AVDTPSignal_SecurityControlRsp(OI_L2CAP_CID cid,
                                         OI_UINT8 transaction,
                                         OI_BYTE *data,
                                         OI_UINT16 dataLen,
                                         OI_UINT8 error);

#endif

/**@}*/
#endif /* FEATURE_BT_EXTPF_AV */

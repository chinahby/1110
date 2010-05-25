/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_obexcommon.h#1 $ 

$Log: $

 * #5         21 March 2007                   MH
 * Added Realm string type
 *
 * #4         19 Sept 2006                    MH
 * The Realm information is added to authentication structure.
===========================================================================*/
/**
@file  
functions common to OBEX server and client
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _OBEXCOMMON_H
#define _OBEXCOMMON_H

#include "oi_obexspec.h"
#include "oi_rfcomm.h"
#include "oi_mbuf.h"
#include "oi_bytestream.h"

/**
 * size of a small buffer used for composing OBEX packet headers
 */
#define OI_OBEX_HDR_BUFSIZE 8

/**
 * the maximum number of headers passed in to a connect request
 */
#define OI_OBEX_MAX_CONNECT_HDRS  4


typedef struct {
    /**
     * password entered by the user
     */
    OI_BYTE password[OI_OBEX_MAX_PASSWORD_LEN];
    OI_UINT8 passwordLen;
    /**
     * user id of client
     */
    OI_BYTE userId[OI_OBEX_MAX_USERID_LEN];
    OI_UINT8 userIdLen;
    /*
     * indicates if server requires a user id in the authentication response.
     */
    OI_BOOL userIdRequired;
#ifndef OI_CODE
    /**
     * Realm Information
     */
    OI_BYTE realmStr[OI_OBEX_MAX_REALM_LEN];
    OI_UINT8 realmLen;
    /* Realm string character set code
    *  Can have values 0 to 9 and 0xFF
    */
    OI_UINT8 realmStrType; 
#endif
    /**
     * nonce received in a challenge
     */
    OI_BYTE challengeDigest[OI_OBEX_AUTH_DIGEST_LEN];
    /**
     * nonce received in a response
     */
    OI_BYTE responseDigest[OI_OBEX_AUTH_DIGEST_LEN];
    /**
     * This is the buffer for sending an authentication response header. This is the entire
     * header byte sequence, so we need two bytes for the authentication field tag and length
     * and two bytes + the maximum user id length for the user id field.
     */
    OI_BYTE responseHeader[5 + OI_OBEX_AUTH_DIGEST_LEN + OI_OBEX_MAX_USERID_LEN];
    /**
     * This is the buffer for sending an authentication challenge header. This is the entire
     * header byte sequence, so we need two bytes for the authentication field tag and length.
     * and three bytes for the option field.
     */
    #ifndef OI_CODE
    /* Now even the max realm should be considered along with the tag field and the
     * len field for realm
     */
    OI_BYTE challengeHeader[5 + OI_OBEX_AUTH_DIGEST_LEN + (OI_OBEX_MAX_REALM_LEN + 2)];
    #else
#error code not present
    #endif
} OBEX_AUTHENTICATION;


typedef struct {

    OI_RFCOMM_LINK_HANDLE link;

    /**
     * The receive buffer is allocated when a connection is established and
     * freed when the connection is terminated.
     */
    OI_BYTE *rcvBuffer;
    /**
     * Byte stream for decomposing the received packet.
     */
    OI_BYTE_STREAM rcvPacket;
    enum {
        OI_OBEX_RCV_BUF_EMPTY,
        OI_OBEX_RCV_IN_PROGRESS,
        OI_OBEX_RCV_COMPLETE
    } rcvState;
    /**
     * mbuf for sending packets.
     */
    OI_MBUF *mbuf;
    /**
     * Small buffer for composing the packet headers and simple response
     * packets.
     */
    OI_BYTE hdrBuf[OI_OBEX_HDR_BUFSIZE];
    /**
     * Are we authenticating connections?
     */
    OI_BOOL authenticating;
    /**
     * indicates read-only access is being granted by server.
     */
    OI_BOOL readOnly;
    /**
     * maintains state during OBEX authentication
     */
    OBEX_AUTHENTICATION *authentication;
    /**
     * RFCOMM frame size for this connection
     */
    OI_UINT16 frameSize;
    /**
     * maximum packet size we can receive (based on configuration parameter)
     */
    OI_UINT16 maxRecvPktLen;
    /**
     * maximum packet size we can send (negotiated at connect time)
     */
    OI_UINT16 maxSendPktLen;
    /**
     * opcode for the current operation.
     */
    OI_UINT8 currentOpcode;
    /**
     * body header that had to be segmented because it would not fit in one
     * packet.
     */
    OI_OBEX_HEADER bodySegment;
#ifndef OI_CODE
    /**
     * Mutual Authentication 
     */
    OI_BOOL mutualAuth;
#endif

} OBEX_COMMON;

#ifndef OI_CODE
/* This is used to store the information of the realm from the client.
 * The info in this is used to construct the challenge header
 * with realm and initiate the authentication from the client side.
 */
typedef struct {
    OI_BD_ADDR addr;
    OI_UINT8 channel;
    OI_REALM_INFO realmInfo;
    OI_BOOL uidRqd;
    OI_BOOL fullAccess;

}OI_AUTH_INFO;
#endif

/**
 * This function initializes and opens a byte stream for composing a generic OBEX packet.
 * 
 * @param common   pointer to common OBEX connection information
 *
 * @param opcode   the OBEX command or response code
 *
 * @param pkt      pointer to byte stream to be initialized
 */
void OI_OBEXCOMMON_InitPacket(OBEX_COMMON *common,
                              OI_UINT8 opcode,
                              OI_BYTE_STREAM *pkt);


/**
 * This function marshalls OBEX headers into an mbuf ready for sending as series
 * of RFCOMM frames.
 * 
 * @param common     pointer to common OBEX connection information
 *
 * @param pktHdr     byte stream previously initialized by
 *                   OI_OBEXCOMMON_InitPacket()
 *
 * @param hdrs       OBEX headers to be marshalled.
 *
 * @param hdrCount   how many headers.
 *
 * @param hdrList    list of headers provided by application.
 *
 * @return           OI_OK or OI_STATUS_OUT_OF_MEMORY
 */
OI_STATUS OI_OBEXCOMMON_MarshalPacket(OBEX_COMMON *common,
                                      OI_BYTE_STREAM *pktHdr,
                                      OI_OBEX_HEADER *hdrs,
                                      OI_UINT16 hdrCount,
                                      OI_OBEX_HEADER_LIST const *hdrList);

/**
 * This function marshalls segmented body headers.
 */
OI_STATUS OI_OBEXCOMMON_MarshalBodySegment(OBEX_COMMON *common);


/**
 * Send a simple OBEX packet.
 */

OI_STATUS OI_OBEXCOMMON_SendSimple(OBEX_COMMON *common,
                                   OI_UINT8 opcode);


#define OI_OBEXCOMMON_SendOk(common) \
    OI_OBEXCOMMON_SendSimple((common), OI_OBEX_FINAL(OI_OBEX_RSP_OK))


#define OI_OBEXCOMMON_SendContinue(common) \
    OI_OBEXCOMMON_SendSimple((common), OI_OBEX_FINAL(OI_OBEX_RSP_CONTINUE))


/**
 * Data received from an OBEX server. 
 */

OI_STATUS OI_OBEXCOMMON_ReassemblePacket(OBEX_COMMON *common,
                                         OI_BYTE *dataBuf,
                                         OI_UINT16 dataLen);

/**
 * Returns the size in bytes that the headers will occupy in an OBEX packet.
 */

OI_UINT16 OI_OBEXCOMMON_HeaderListSize(OI_OBEX_HEADER_LIST const *headers);

/**
 * Get all headers.
 */

OI_STATUS OI_OBEXCOMMON_ParseHeaderList(OI_OBEX_HEADER_LIST *headers,
                                        OI_BYTE_STREAM *bs);

/**
 * Scan for a specific obex header.
 */
OI_OBEX_HEADER* OI_OBEXCOMMON_FindHeader(OI_OBEX_HEADER_LIST const *headers,
                                         OI_UINT8 headerId);

/**
 * Parses an application parameters header into its individual components.
 * The caller is responsible for calling OI_Free(params->list) when it is
 * finished with this information. The underlying byteseq data is not affected
 * by this operation, but the generated param list's data pointers refer back to
 * the byteseq data, which must therefore remain live for the lifetime of the
 * params array.
 */
OI_STATUS OI_OBEXCOMMON_ParseAppParamsHeader(const OI_OBEX_BYTESEQ *data,
                                             OI_OBEX_APP_PARAM_LIST *params);

/**
 * Builds an application paramers header based on a list of elements. The caller
 * is responsible for calling OI_Free(data->data) when it is finished with this
 * information. The underlying application parameter entry list data is not
 * affected by this operation.
 */
OI_STATUS OI_OBEXCOMMON_BuildAppParamsHeader(OI_OBEX_BYTESEQ *data,
                                             const OI_OBEX_APP_PARAM_LIST *params);
#endif /* _OBEXCOMMON_H */

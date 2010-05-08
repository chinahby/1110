#ifdef FEATURE_BT_EXTPF_AV

/**
 * @file  
 * @internal
 *
 * This file implements Audio/Video Control Transport Protocol (AVCTP). 
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_AVCTP

#include "oi_core_common.h"
#include "oi_dispatch.h"
#include "oi_avctp.h"
#include "oi_bytestream.h"
#include "oi_securitydb.h"
#include "oi_init_flags.h"
#include "oi_testprint.h"
#include "oi_l2cap_mbuf.h"

/*************************************************************

  constants

*************************************************************/

#define AVCTP_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER
#define MAX_HEADER_LEN 4
#define COMPLETE_HEADER_LEN 3
#define START_HEADER_LEN 4
#define CONTINUE_HEADER_LEN 1
#define END_HEADER_LEN 1

// packet types
#define AVCTP_PKT_TYPE_COMPLETE 0
#define AVCTP_PKT_TYPE_START    1
#define AVCTP_PKT_TYPE_CONTINUE 2
#define AVCTP_PKT_TYPE_END      3

/*************************************************************

  macros

*************************************************************/

// The following are macros for extracting fields from the first byte of an AVCTP header.
// Values are converted to be most useful to applications.
#define GET_TRANSACTION(x)  (((x) & (BIT7 | BIT6 | BIT5 | BIT4)) >> 4)
#define GET_CMD_TYPE(x)     (((x) & BIT1) ? OI_AVCTP_TYPE_RSP : OI_AVCTP_TYPE_CMD) //translate to use interface codes
#define GET_PKT_TYPE(x)     (((x) & (BIT3 | BIT2)) >> 2)
#define GET_IPID(x)         ((x) & BIT0)

/*************************************************************

  types

*************************************************************/

/** structure type to keep track of connection information */
typedef struct _AVCTP_CONNECT_INFO{

    /** Next profile link */
    struct _AVCTP_CONNECT_INFO *next;

    /** This is a pointer to the buffer currently being written. This is stored so that the
        pointer can be returned to the application. */
    OI_BYTE *writeBuf;

    /** The transaction number for the packet, if any, currently being
     * reassembled */
    OI_UINT8 recvTransaction;

    /** The reassembly buffer */
    OI_BYTE *recvBuf;

    /** the Bluetooth device address of the remote device associated with this connection */
    OI_BD_ADDR addr;

    /** The number of bytes received so far. */
    OI_UINT16 recvCount;

    /** the number of services actively using this connection */
    OI_UINT16 refCount;

    /** saved MTU */
    OI_UINT16 outMTU;

    /** saved MTU */
    OI_UINT16 inMTU;

    /** The total length of the buffer to be written */
    OI_UINT16 writeBufLen;

    /** The number of bytes written so far. */
    OI_UINT16 writeCount;

    /** The pid for the command currently being sent. */
    OI_UINT16 pid;

    /** This field holds the L2CAP connection ID (CID). 0 indicates not in use. */
    OI_L2CAP_CID cid;

    /** The transaction number for the command currently being sent. */
    OI_UINT8 transaction;

    /** The type for the command currently being sent. */
    OI_UINT8 type;

    /** Expected packets */
    OI_UINT8 expectedPkts;

    /** The number of fragments in the current packet */
    OI_UINT8 numPkts;

    /** True if the connection is fully established, false if it is merely
     * pending */
    OI_BOOL connectionEstablished;
} AVCTP_CONNECT_INFO;

/** structure type to keep track of profile information */
typedef struct _AVCTP_PROFILE_INFO{

    /** Next profile link */
    struct _AVCTP_PROFILE_INFO *next;

    /* callbacks */
    OI_AVCTP_CONNECT_IND connectInd;
    OI_AVCTP_CONNECT_CFM connectCfm;
    OI_AVCTP_DISCONNECT_IND disconnectInd;
    OI_AVCTP_RECV_IND recvInd;
    OI_AVCTP_SEND_CFM sendCfm;

    /** This field holds the profile ID. 0 indicates not in use. */
    OI_UINT16 pid;

    /** L2CAP CID on which this profile is waiting for a connection to complete */
    OI_L2CAP_CID cid;
} AVCTP_PROFILE_INFO;

typedef struct {
    OI_CONNECT_POLICY   connectPolicy;
    AVCTP_PROFILE_INFO *profiles;
    AVCTP_CONNECT_INFO *connections;
} AVCTP_THIS;

/*************************************************************

  static data

*************************************************************/

static AVCTP_THIS *AVCTP_This;

/*************************************************************

  function prototypes

*************************************************************/

static OI_STATUS GetProfile(AVCTP_PROFILE_INFO **p,
                            OI_UINT16 pid);

static OI_STATUS GetConnection(AVCTP_CONNECT_INFO **c,
                               OI_L2CAP_CID cid);

static OI_STATUS GetConnectionByAddr(AVCTP_CONNECT_INFO **c,
                                     OI_BD_ADDR *addr);

static OI_STATUS AllocConnection(AVCTP_CONNECT_INFO **c);

static OI_STATUS FreeConnection(OI_L2CAP_CID cid);

static void L2CAPWriteCB(OI_L2CAP_CID cid,
                         OI_UINT8 *buf,
                         OI_UINT16 bufLen,
                         OI_STATUS result);

static void L2CAPWriteMbufCB(OI_L2CAP_CID cid,
                             OI_MBUF *mbuf,
                             OI_STATUS result);

/*************************************************************

  callbacks

*************************************************************/

static void L2CAPConnectCB(OI_L2CAP_CID cid,
                           OI_UINT16 inMTU,
                           OI_UINT16 outMTU,
                           OI_STATUS result)
{
    OI_STATUS retVal;
    AVCTP_PROFILE_INFO *p1, *p2;
    AVCTP_CONNECT_INFO *c;
    OI_BD_ADDR addr;

    OI_DBGPRINT(("L2CAPConnectCB (cid=%d, inMTU=%d, outMTU=%d, result=%!)", cid, inMTU, outMTU, result));

    retVal = GetConnection(&c, cid);
    if (!OI_SUCCESS(retVal)) {
        OI_DBGPRINT2(("GetConnection failed! - cid=%d\n", cid));
        return;
    }

    addr = c->addr;
    

    if (OI_SUCCESS(result)) {
        c->outMTU = outMTU;
        c->inMTU = inMTU;
        c->connectionEstablished = TRUE;
    } else {
        OI_DBGPRINT2(("L2CAP Connection failed\n"));
        FreeConnection(c->cid);
    }

    /** Notify all profiles of the connection result. */
    p1 = AVCTP_This->profiles;
    while (NULL != p1) {
        p2 = p1->next;
        p1->cid = 0; //reset the cid
        p1->connectCfm(&addr, inMTU, outMTU, result);
        p1 = p2;
    }
}

/**
 * Handle a disconnect of the L2CAP link.
 *
 */
static void L2CAPDisconnectCB(OI_L2CAP_CID cid,
                              OI_STATUS reason)
{
    OI_STATUS result;
    AVCTP_PROFILE_INFO *p1, *p2;
    AVCTP_CONNECT_INFO *c;
    OI_BD_ADDR addr;

    OI_DBGPRINT(("L2CAPDisconnectCB(cid=%d, reason=%!)", cid, reason));

    
    result = GetConnection(&c, cid);
    if (!OI_SUCCESS(result)) return;

    addr = c->addr;
    FreeConnection(cid);

    /** Notify all profiles of the disconnection reason. */
    p1 = AVCTP_This->profiles;
    while (NULL != p1) {
        p2 = p1->next;
        p1->disconnectInd(&addr, reason);
        p1 = p2;
    }
}

static void L2CAPRecvCB(OI_L2CAP_CID cid,
                        OI_UINT8 *buf,
                        OI_UINT16 length)
{
    OI_STATUS result;
    OI_BYTE_STREAM bs;
    OI_UINT8 b = 0;
    OI_UINT16 pid = 0;
    OI_UINT8 transaction;
    OI_UINT8 pktType;
    OI_UINT8 cmdType;
    OI_UINT8 ipid;
    AVCTP_PROFILE_INFO *p;
    AVCTP_CONNECT_INFO *c;


    /* ignore zero length packets */
    if (length == 0) {
        OI_LOG_ERROR(("Zero length packet received"));
        return;
    }

    result = GetConnection(&c, cid);
    if (!OI_SUCCESS(result)) {
        return;
    }

    /* Extract header data */


    ByteStream_Init(bs, buf, length);
    ByteStream_Open(bs, BYTESTREAM_READ);
    ByteStream_GetUINT8_Checked(bs, b);
    transaction = GET_TRANSACTION(b);
    pktType = GET_PKT_TYPE(b);
    cmdType = GET_CMD_TYPE(b);
    ipid = GET_IPID(b);

    if (AVCTP_PKT_TYPE_START == pktType) {
        ByteStream_GetUINT8_Checked(bs, c->expectedPkts);
        OI_DBGPRINT2(("Expecting %d fragments\n", c->expectedPkts));
    }
    if (AVCTP_PKT_TYPE_START == pktType || AVCTP_PKT_TYPE_COMPLETE == pktType) {
        ByteStream_GetUINT16_Checked(bs, pid, AVCTP_BYTE_ORDER);
    }

    if (ByteStream_Error(bs)) {
        OI_LOG_ERROR(("Invalid packet received"));
        return;
    }

    /* Determine which profile the packet is intended for, and send a failure
     * response if it can't be found */

    if (AVCTP_PKT_TYPE_START == pktType || AVCTP_PKT_TYPE_COMPLETE == pktType) {
        result = GetProfile(&p, pid);
        if (!OI_SUCCESS(result)) {
            OI_DBGPRINT2(("Packet received for unregistered PID\n"));
            if ((buf[0] & BIT1) == 0) { /* Only reply to a command with invalid PID. */
                OI_BYTE *rspBuf;

                rspBuf = OI_Malloc(COMPLETE_HEADER_LEN);
                if (NULL == rspBuf) {
                    OI_LOG_ERROR(("Out of memory\n"));
                    OI_L2CAP_Disconnect(cid);
                    return;
                }
                OI_MemCopy(rspBuf, buf, COMPLETE_HEADER_LEN);
                OI_BIT_SET(rspBuf[0], BIT1); //change command bit to response bit
                OI_BIT_SET(rspBuf[0], BIT0); //set invalid PID bit
                OI_L2CAP_Write(L2CAPWriteCB, cid, rspBuf, COMPLETE_HEADER_LEN);
            }
            return;
        }
    }


    /* Reassemble payload and notify application */

    switch (pktType) {
        case AVCTP_PKT_TYPE_COMPLETE:
            OI_DBGPRINT2(("Got COMPLETE fragment"));
            if (c->recvBuf) {
                OI_LOG_ERROR(("Discarding previous incomplete packet."));
                OI_FreeIf(&c->recvBuf);
            }
            p->recvInd(&c->addr, transaction, cmdType, ipid, buf + COMPLETE_HEADER_LEN, (OI_UINT16)(length - COMPLETE_HEADER_LEN));

            OI_ASSERT(c->recvBuf == NULL);
            break;

        case AVCTP_PKT_TYPE_START:
            OI_DBGPRINT2(("Got START fragment"));
            c->pid = pid;
            if (c->recvBuf) {
                OI_LOG_ERROR(("Unexpected start packet. Discarding previous buffer.\n"));
                OI_Free(&c->recvBuf);
            }
            c->recvBuf = OI_Malloc((OI_UINT32)c->inMTU * c->expectedPkts); /* Cast to ensure multiplication can't overflow on 16-bit int platforms */
            if (!c->recvBuf) {
                OI_LOG_ERROR(("Out of memory. Terminating connection."));
                OI_L2CAP_Disconnect(cid);
                return;
            }
            c->recvTransaction = transaction;
            c->numPkts = 1;
            OI_MemCopy(c->recvBuf, buf + START_HEADER_LEN, length - START_HEADER_LEN);
            c->recvCount = (length - START_HEADER_LEN);
            OI_ASSERT(c->recvBuf != NULL);
            break;

        case AVCTP_PKT_TYPE_CONTINUE:
            OI_DBGPRINT2(("Got CONTINUE fragment"));
            if (!c->recvBuf) {
                OI_LOG_ERROR(("Unexpected continue packet. Discarding data."));
            } else if (++c->numPkts == c->expectedPkts) {
                OI_LOG_ERROR(("Too many continue packets. Discarding data."));
                OI_FreeIf(&c->recvBuf);
            } else if (c->recvTransaction != transaction) {
                OI_LOG_ERROR(("Received CONTINUE fragment with mismatched transaction number. Discarding data."));
                OI_FreeIf(&c->recvBuf);
            } else {
                OI_MemCopy(c->recvBuf + c->recvCount, buf + CONTINUE_HEADER_LEN, length - CONTINUE_HEADER_LEN);
                c->recvCount += (length - CONTINUE_HEADER_LEN);
            }
            break;

        case AVCTP_PKT_TYPE_END:
            OI_DBGPRINT2(("Got END fragment\n"));
            if (!c->recvBuf) {
                OI_LOG_ERROR(("Unexpected END fragment. Discarding data."));
            } else if (++c->numPkts != c->expectedPkts) {
                OI_LOG_ERROR(("Wrong number of fragments. Discarding data."));
            } else if (c->recvTransaction != transaction) {
                OI_LOG_ERROR(("Received END fragment with mismatched transaction number. Discarding data."));
            } else {
                result = GetProfile(&p, c->pid);
                if (OI_SUCCESS(result)) {
                    OI_MemCopy(c->recvBuf + c->recvCount, buf + END_HEADER_LEN, length - END_HEADER_LEN);
                    p->recvInd(&c->addr, transaction, cmdType, ipid, c->recvBuf, (OI_UINT16)(c->recvCount + length - END_HEADER_LEN));
                } else {
                    OI_LOG_ERROR(("Profile %d not found\n", c->pid));
                }
            }
            OI_FreeIf(&c->recvBuf); /* Data has been delivered or thrown away at this point. */
            break;

        default:
            OI_ASSERT(FALSE); /* Numerically impossible */
    }

}

static void L2CAPWriteCB(OI_L2CAP_CID cid,
                         OI_BYTE *data,
                         OI_UINT16 dataLen,
                         OI_STATUS result)
{
    // This is the confirmation of an ipid response.
    OI_Free(data);
}


static void L2CAPWriteMbufCB(OI_L2CAP_CID cid,
                             OI_MBUF *mbuf,
                             OI_STATUS status)
{
    AVCTP_PROFILE_INFO *p = NULL;
    AVCTP_CONNECT_INFO *c = NULL;
    OI_UINT16 left;
    OI_BYTE *header;
    OI_MBUF *newMbuf = NULL;
    OI_UINT16 headerLen;
    OI_STATUS result;
    OI_UINT16 writeCount;
    OI_BOOL done;
    OI_BYTE *saveWriteBuf;

    OI_MBUF_Free(mbuf);

    result = GetConnection(&c, cid);
    if (!OI_SUCCESS(result)) {
        OI_LOG_ERROR(("Could not find connection\n"));
        return;
    }

    /* Write failed */
    if (!OI_SUCCESS(status)) {
        result = status;
        goto Cleanup;
    }

    left = (c->writeBufLen - c->writeCount);
    if (left != 0) {
        headerLen = CONTINUE_HEADER_LEN;
        done = (left <= (c->outMTU - END_HEADER_LEN));
        writeCount = OI_MIN(left, (c->outMTU - CONTINUE_HEADER_LEN));

        newMbuf = OI_MBUF_Alloc(2);
        if (!newMbuf) {
            OI_LOG_ERROR(("Could not allocate mbuf\n"));
            result = OI_STATUS_OUT_OF_MEMORY;
            goto Cleanup;
        }
        header = OI_Malloc(1);
        if (!header) {
            result = OI_STATUS_OUT_OF_MEMORY;
            goto Cleanup;
        }

        header[0] = ((c->transaction << 4)
                     | ((done ? AVCTP_PKT_TYPE_END : AVCTP_PKT_TYPE_CONTINUE) << 2)
                     | ((c->type == OI_AVCTP_TYPE_CMD) ? 0 : BIT1));

        result = OI_MBUF_Append(newMbuf, header, headerLen, MBUF_FREE);
        if (!OI_SUCCESS(result)) {
            goto Cleanup;
        }
        result = OI_MBUF_Append(newMbuf, c->writeBuf + c->writeCount, writeCount, MBUF_KEEP);
        if (!OI_SUCCESS(result)) {
            goto Cleanup;
        }

#ifndef OI_CODE
        c->writeCount += writeCount;
#endif /* OI_CODE */
#ifndef OI_CODE
        result = OI_L2CAP_WriteMBUF(L2CAPWriteMbufCB,
                                    c->cid, 
                                    newMbuf , NULL);
#else
#error code not present
#endif
        if (!OI_SUCCESS(result)) {
#ifndef OI_CODE
        c->writeCount += writeCount;
#endif /* OI_CODE */
            goto Cleanup;
        }
        else {
#ifdef OI_CODE
#error code not present
#endif /* OI_CODE */
            newMbuf = NULL; /* Ownership of this buffer now rests L2CAPWriteMbufCB */
        }

        return;
    }

Cleanup:
    if (newMbuf) {
        OI_MBUF_Free(newMbuf);
    }

    saveWriteBuf = c->writeBuf;
    c->writeBuf = NULL;

    (void)GetProfile(&p, c->pid); /* p is NULL if and only if the profile record is not found */
    if (NULL != p) {
        p->sendCfm(&c->addr, c->pid, c->transaction, c->type, saveWriteBuf, c->writeBufLen, result);
    } else {
        OI_LOG_ERROR(("Could not find profile %d\n", c->pid));
    }
}


static void L2CAPConnectInd(OI_L2CAP_PSM psm,
                            OI_BD_ADDR *addr,
                            OI_L2CAP_CID cid)
{
    OI_STATUS result;
    AVCTP_PROFILE_INFO *p1, *p2;
    AVCTP_CONNECT_INFO *c;
    OI_BOOL reject;

    OI_DBGPRINT(("L2CAPConnectInd(psm=%d, addr=%:, cid=%d)", psm, addr, cid));


    result = GetConnectionByAddr(&c, addr);
    if (OI_SUCCESS(result)) {
        /* A connection from this same address is already present, or is
         * pending. */

        if (c->connectionEstablished) {
            OI_LOG_ERROR(("AVCTP connection already in progress with %:", addr));

            /* Fall through to reject the connection */
            reject = TRUE;
        } else {
            /* Previous connection is merely pending. It may no longer be valid.
             * See Bugzilla 2896 for an explanation of this uncertainty. */

            /* Reject the previous pending attempt, assuming it's still valid */
            OI_LOG_ERROR(("Rejecting previous pending attempt for incoming AVCTP connection"));
            OI_L2CAP_Accept(NULL, NULL, NULL, c->cid, NULL, FALSE, NULL);
            FreeConnection(c->cid);

            /* Process the current connection attempt just as though no previous
             * connection record had existed */
            reject = FALSE;
        }
    } else {
        /* No previous connection record exists. Allow this attempt to be
         * propagated upwards */

        reject = FALSE;
    }


    if (AVCTP_This->profiles == FALSE) {
        OI_LOG_ERROR(("No AVCTP profiles registered"));
        reject = TRUE;
    }

    if (!reject) {
        result = AllocConnection(&c);
        if (OI_SUCCESS(result)) {
            OI_DBGPRINT2(("Allowing new AVCTP connection"));
            // Initialize connection.
            c->addr = *addr;
            c->cid = cid;
            c->refCount = 1;
            c->writeBuf = NULL;
            c->writeBufLen = 0;
            c->writeCount = 0;
            c->connectionEstablished = FALSE;

            /** Notify all profiles. */
            p1 = AVCTP_This->profiles;
            while (NULL != p1) {
                p2 = p1->next;
                p1->connectInd(addr);
                p1 = p2;
            }
        } else {
            /* Unable to allocate the connection record; reject the request. */
            reject = TRUE;
        }
    }

    if (reject) {
        OI_LOG_ERROR(("Rejecting incoming AVCTP connection request"));
        // Reject.
        OI_L2CAP_Accept(NULL,
                        NULL,
                        NULL,
                        cid,
                        NULL,
                        FALSE   /* Reject the connection */,
                        NULL);
    }
}

// Dispatcher callback - takes PID as argument
static void DispatchedConnectCB(DISPATCH_ARG *arg)
{
    OI_STATUS result;
    AVCTP_PROFILE_INFO *p;
    AVCTP_CONNECT_INFO *c;
    OI_UINT16 pid = Dispatch_GetArg(arg, OI_UINT16);

    OI_DBGPRINT2(("DispatchedConnectCB\n"));

    result = GetProfile(&p, pid);
    if (!OI_SUCCESS(result)) {
        OI_LOG_ERROR(("%! (%d)",result, pid)) ;
        return;
    }

    result = GetConnection(&c, p->cid);
    if (!OI_SUCCESS(result)) {
        OI_LOG_ERROR(("%! (%d)",result, p->cid)) ;
        return;
    }

    p->connectCfm(&c->addr, c->inMTU, c->outMTU, result);
}

/*************************************************************

  internal functions

*************************************************************/

/* Profile functions */

static OI_STATUS GetProfile(AVCTP_PROFILE_INFO **p,
                            OI_UINT16            pid)
{
    AVCTP_PROFILE_INFO *ptr;

    OI_ASSERT(p);

    ptr = AVCTP_This->profiles;
    while (NULL != ptr){
        if (ptr->pid == pid){
            *p = ptr;
            return OI_OK;
        } else {
            ptr = ptr->next;
        }
    }

    return OI_STATUS_NOT_FOUND;
}

static OI_STATUS AllocProfile(AVCTP_PROFILE_INFO **p)
{
    AVCTP_PROFILE_INFO *ptr;

    OI_ASSERT(p);

    ptr = OI_Calloc(sizeof(AVCTP_PROFILE_INFO));
    if (NULL == ptr){
        OI_LOG_ERROR(("Out of memory space trying to allocate profile."));
        return OI_STATUS_NO_RESOURCES;
    } else {
        ptr->next = AVCTP_This->profiles;
        AVCTP_This->profiles = ptr;
        *p = ptr;
        return OI_OK;
    }
}

static OI_STATUS FreeProfile(OI_UINT16 pid)
{
    AVCTP_PROFILE_INFO **prev = &AVCTP_This->profiles;
    AVCTP_PROFILE_INFO *cur = AVCTP_This->profiles;

    while (NULL != cur){
        if (cur->pid == pid){
            /* Unlink and free profile allocation */
            *prev = cur->next;
            OI_Free(cur);
            return OI_OK;
        } else {
            prev = &cur->next;
            cur = *prev;
        }
    }

    return OI_STATUS_NOT_FOUND;
}

/* Connection Functions */

static OI_STATUS GetConnectionByAddr(AVCTP_CONNECT_INFO **c,
                                     OI_BD_ADDR *addr)
{
    AVCTP_CONNECT_INFO *ptr;

    OI_ASSERT(c);

    ptr = AVCTP_This->connections;
    while (NULL != ptr){
        if (SAME_BD_ADDR(addr, &ptr->addr)) {
            *c = ptr;
            return OI_OK;
        } else {
            ptr = ptr->next;
        }
    }

    return OI_STATUS_NOT_FOUND;
}

static OI_STATUS GetConnection(AVCTP_CONNECT_INFO **c,
                               OI_L2CAP_CID cid)
{
    AVCTP_CONNECT_INFO *ptr;

    OI_ASSERT(c);

    ptr = AVCTP_This->connections;
    while (NULL != ptr){
        if (ptr->cid == cid){
            *c = ptr;
            return OI_OK;
        } else {
            ptr = ptr->next;
        }
    }

    return OI_STATUS_NOT_FOUND;
}

static OI_STATUS AllocConnection(AVCTP_CONNECT_INFO **c)
{
    AVCTP_CONNECT_INFO *ptr;

    OI_ASSERT(c);

    ptr = OI_Calloc(sizeof(AVCTP_CONNECT_INFO));
    if (NULL == ptr){
        OI_LOG_ERROR(("Out of memory trying to allocate new AVCTP connetion record"));
        return OI_STATUS_NO_RESOURCES;
    } else {
        ptr->next = AVCTP_This->connections;
        AVCTP_This->connections = ptr;
        *c = ptr;
        return OI_OK;
    }
}

static OI_STATUS FreeConnection(OI_L2CAP_CID cid)
{
    AVCTP_CONNECT_INFO **prev = &AVCTP_This->connections;
    AVCTP_CONNECT_INFO *cur = AVCTP_This->connections;

    while (NULL != cur){
        if (cur->cid == cid){
            /* Unlink and free connection allocation */
            *prev = cur->next;
            OI_FreeIf(&cur->recvBuf);   /* Free reassembly buffer if a disconnect interrupted reassembly */
            OI_Free(cur);
            return OI_OK;
        } else {
            prev = &cur->next;
            cur = *prev;
        }
    }
    OI_LOG_ERROR(("Failed to free AVCTP connection"));
    return OI_STATUS_NOT_FOUND;
}

/**
 * This function initializes the protocol.
 */
static OI_STATUS internalInit(void)
{
    OI_STATUS result;
    OI_CONNECT_POLICY connectPolicy =  {
        OI_UUID_AV_RemoteControl,   // OI_UINT32           serviceUuid32 ;
        OI_SEC_REQUIRE_NOTHING,     // securityPolicy is a place holder, get real policy at runtime
        FALSE,                      // OI_BOOL             mustBeMaster ;
        NULL,                       // OI_L2CAP_FLOWSPEC   *flowspec;
        0                           // OI_UINT8            powerSavingDisables ;
    } ;

    if ( OI_INIT_FLAG_VALUE(AVCTP) ){
        return OI_OK;
    }

    AVCTP_This = OI_Calloc(sizeof(AVCTP_THIS));
#ifndef OI_CODE
	if ( NULL == AVCTP_This ){
        OI_LOG_ERROR(("Out of memory trying to allocate new AVCTP info record"));
        return OI_STATUS_NO_RESOURCES;
    } 
#endif /* OI_CODE */
    AVCTP_This->connectPolicy = connectPolicy;

    /*
     * get security policy.
     */
    AVCTP_This->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(AVCTP_This->connectPolicy.serviceUuid32);
#ifdef OI_CODE
#error code not present
#else
    result = OI_L2CAP_Register(OI_PSM_AVCTP, L2CAPConnectInd, NULL);
#endif
    if (!OI_SUCCESS(result)){
        OI_FreeIf(&AVCTP_This);
    }else{
        OI_INIT_FLAG_PUT_FLAG(TRUE, AVCTP);
    }

    return result;
}

static OI_STATUS internalTerminate(void)
{
    OI_STATUS result = OI_OK;

    OI_CHECK_INIT(AVCTP);

    result = OI_L2CAP_Deregister(OI_PSM_AVCTP);
    if(OI_SUCCESS(result)){
        while (NULL != AVCTP_This->connections){
            FreeConnection(AVCTP_This->connections->cid);
        }
        while (NULL != AVCTP_This->profiles){
            FreeProfile(AVCTP_This->profiles->pid);
        }
        OI_FreeIf(&AVCTP_This);
        OI_INIT_FLAG_PUT_FLAG(FALSE,AVCTP);
    }
    return result;
}

/*************************************************************

  external functions

*************************************************************/

/**
 * This function registers a given profile with AVCTP.
 *
 * @param pid            profile identifier (PID)
 * @param connectInd     connect indication callback function
 * @param disconnectInd  disconnect indication callback function
 * @param recvInd        receive indication callback function
 * @param connectCfm     connect confirmation callback function
 * @param sendCfm        send confirmation callback function
 */
OI_STATUS OI_AVCTP_Register(OI_UINT16 pid,
                         OI_AVCTP_CONNECT_IND connectInd,
                         OI_AVCTP_DISCONNECT_IND disconnectInd,
                         OI_AVCTP_RECV_IND recvInd,
                         OI_AVCTP_CONNECT_CFM connectCfm,
                         OI_AVCTP_SEND_CFM sendCfm)
{
    OI_STATUS result;
    AVCTP_PROFILE_INFO *p;

    OI_ENSURE_INIT(AVCTP);

    /** Only allow one registration per PID. */
    result = GetProfile(&p, pid);
    if (OI_SUCCESS(result)) return OI_STATUS_ALREADY_REGISTERED;

    result = AllocProfile(&p);
    if (!OI_SUCCESS(result)) return result;

    p->pid = pid;
    p->connectInd = connectInd;
    p->disconnectInd = disconnectInd;
    p->recvInd = recvInd;
    p->connectCfm = connectCfm;
    p->sendCfm = sendCfm;

    OI_TestPrint(("AVCT_EventRegistration\n\tEvent=%d\n\tCallback=%s\n\tPID=%d\n", 0x0001, "AVCT_Connect_Ind", pid));
    OI_TestPrint(("AVCT_EventRegistration\n\tEvent=%d\n\tCallback=%s\n\tPID=%d\n", 0x0002, "AVCT_Connect_Cfm", pid));
    OI_TestPrint(("AVCT_EventRegistration\n\tEvent=%d\n\tCallback=%s\n\tPID=%d\n", 0x0003, "AVCT_Disconnect_Cfm", pid));
    OI_TestPrint(("AVCT_EventRegistration\n\tEvent=%d\n\tCallback=%s\n\tPID=%d\n", 0x0004, "AVCT_Disconnect_Ind", pid));
    OI_TestPrint(("AVCT_EventRegistration\n\tEvent=%d\n\tCallback=%s\n\tPID=%d\n", 0x0005, "AVCT_MessageRec_Ind", pid));
    OI_TestPrint(("AVCT_EventRegistration\n\tResult=%d\n", 0));

    return OI_OK;
}

OI_STATUS OI_AVCTP_Deregister(OI_UINT16 pid)
{
    OI_STATUS result;

    OI_CHECK_INIT(AVCTP);

    result = FreeProfile(pid);

    if (OI_SUCCESS(result)){
        if ( NULL == AVCTP_This->profiles ){
            result = internalTerminate();
        }
    }

    return result;
}

/**
 * This function creates a connection to the specified remote device.
 *
 * @param addr    the Bluetooth device address of the remote device
 * @param pid     the profile identifier for the local profile
 * @param params  the parameters to be used for the underlying L2CAP connection
 */
OI_STATUS OI_AVCTP_Connect(OI_BD_ADDR *addr,
                           OI_UINT16 pid,
                           OI_L2CAP_CONNECT_PARAMS *params)
{
    OI_STATUS result;
    AVCTP_PROFILE_INFO *p;
    AVCTP_CONNECT_INFO *c;
    DISPATCH_ARG arg;

    OI_CHECK_INIT(AVCTP);

    /** Ensure that the profile is registered. */
    result = GetProfile(&p, pid);
    if (!OI_SUCCESS(result)) return result;

    result = GetConnectionByAddr(&c, addr);
    if (OI_SUCCESS(result)) {
        // Connection already exists
        c->refCount++;
        p->cid = c->cid;
        Dispatch_SetArg(arg, p->pid);
        OI_Dispatch_RegisterFunc(DispatchedConnectCB, &arg, NULL);
        return OI_OK;
    }

    result = AllocConnection(&c);
    if (!OI_SUCCESS(result)) return result;

    result = OI_L2CAP_Connect(L2CAPConnectCB,
                              L2CAPDisconnectCB,
                              L2CAPRecvCB,
                              OI_PSM_AVCTP,
                              addr,
                              params,
                              &c->cid,
                              &AVCTP_This->connectPolicy);
    if (!OI_SUCCESS(result)) {
        OI_LOG_ERROR(("L2CAP connection failed %!", result));
        FreeConnection(c->cid);
        return result;
    }

    /** Save the CID with the profile. */
    p->cid = c->cid;

    /** Initialize the connection. */
    c->addr = *addr;
    c->refCount = 1;
    return OI_OK;
}

/**
 * This function accepts or rejects a connection from a remote device.
 *
 * @param addr     Bluetooth device address of remote device
 * @param accept   TRUE to accept connection, FALSE to reject connection
 */
OI_STATUS OI_AVCTP_Accept(OI_BD_ADDR *addr,
                          OI_BOOL accept)
{
    OI_STATUS result;
    AVCTP_CONNECT_INFO *c;


    OI_DBGPRINT(("OI_AVCTP_Accept(%:, %B)", addr, accept));

    OI_CHECK_INIT(AVCTP);

    result = GetConnectionByAddr(&c, addr);

    if (!OI_SUCCESS(result)) {
        OI_LOG_ERROR(("Could not find connection %!", result));
        return result;
    }

    result = OI_L2CAP_Accept(L2CAPConnectCB,
                             L2CAPDisconnectCB,
                             L2CAPRecvCB,
                             c->cid,
                             NULL,
                             accept,
                             &AVCTP_This->connectPolicy);

    if (!OI_SUCCESS(result) || !accept) {
        if (!OI_SUCCESS(result)) {
            OI_LOG_ERROR(("OI_L2CAP_Accept failed %!", result));
        }
        FreeConnection(c->cid);
    }
    return result;
}


/**
 * This function disconnects the AVCTP connection with the specified device.
 *
 * @param addr     Bluetooth device address of remote device
 * @param pid      the profile identifier (PID) for the local profile
 */
OI_STATUS OI_AVCTP_Disconnect(OI_BD_ADDR *addr,
                              OI_UINT16 pid)
{
    AVCTP_CONNECT_INFO *c;
    OI_STATUS result;

    OI_TestPrint(("AVCT_DisconnectReq\n\tBD_ADDR=%:\n\tPID=%d\n", addr, pid));

    OI_CHECK_INIT(AVCTP);

    result = GetConnectionByAddr(&c, addr);
    if (!OI_SUCCESS(result)) {
        return result;
    }

    if (0 == c->refCount) { //A disconnect has been requested
        return OI_STATUS_NOT_FOUND;
    }

    if (1 == c->refCount) {
        result = OI_L2CAP_Disconnect(c->cid);
        // Don't free the channel until we get the confirmation.
    } else {
        result = OI_OK;
    }
    if (OI_SUCCESS(result)) {
        c->refCount--;
        OI_DBGPRINT2(("Reference count = %d\n", c->refCount));
    }
    return result;
}

/**
 * This function sends an AVCTP command or response message.
 *
 * @param addr         Bluetooth device address of remote device
 * @param pid          the profile identifier (PID) for the profile sending the message
 * @param transaction  the transaction label for the message
 * @param type         the type of the message (OI_AVCTP_TYPE_CMD or OI_AVCTP_TYPE_RSP)
 * @param data         pointer to message data
 * @param dataLen      the number of bytes of data to be sent
 */
OI_STATUS OI_AVCTP_Send(OI_BD_ADDR *addr,
                        OI_UINT16 pid,
                        OI_UINT8 transaction,
                        OI_UINT8 type,
                        OI_UINT8 *data,
                        OI_UINT16 dataLen)
{
    OI_STATUS result;
    AVCTP_CONNECT_INFO *c;
    AVCTP_PROFILE_INFO *p;
    OI_BYTE_STREAM bs;
    OI_BYTE *header = NULL;
    OI_UINT16 headerLen;
    OI_MBUF *mbuf = NULL;
    OI_BOOL complete;

    OI_TestPrint(("AVCT_SendMessage\n\tBD_ADDR=%:\n\tTransaction=%d\n\tType=%d\n\tPID=%d\n\tData=%s\n\tLength=%d\n", addr, transaction, type, pid, OI_HexText(data, dataLen), dataLen));

    OI_CHECK_INIT(AVCTP);

    if ((transaction > 15) || ((type != OI_AVCTP_TYPE_CMD) && (type != OI_AVCTP_TYPE_RSP)) ) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    result = GetProfile(&p, pid);
    if (!OI_SUCCESS(result)) {
        return OI_STATUS_NOT_REGISTERED;
    }

    result = GetConnectionByAddr(&c, addr);
    if (!OI_SUCCESS(result)) {
        return result;
    }

    if (c->writeBuf != NULL) {
        return OI_STATUS_WRITE_IN_PROGRESS;
    }

    complete = (dataLen + COMPLETE_HEADER_LEN <= c->outMTU);
    if (complete) {
        headerLen = COMPLETE_HEADER_LEN;
    }
    else {
        headerLen = START_HEADER_LEN;
    }
    c->writeCount = OI_MIN((c->outMTU - headerLen), dataLen);

    mbuf = OI_MBUF_Alloc(2);
    if (!mbuf) {
        return OI_STATUS_OUT_OF_MEMORY;
    }
    header = OI_Malloc(headerLen);
    if (!header) {
        result = OI_STATUS_OUT_OF_MEMORY;
        goto Error;
    }

    ByteStream_Init(bs, header, headerLen);
    ByteStream_Open(bs, BYTESTREAM_WRITE);
    if (complete) {
        ByteStream_PutUINT8(bs, ((transaction << 4) | (AVCTP_PKT_TYPE_COMPLETE << 2) | ( (type == OI_AVCTP_TYPE_CMD) ? 0 : BIT1)) );
        ByteStream_PutUINT16(bs, pid, AVCTP_BYTE_ORDER);
    }
    else {
        /* Calculate number of fragments required */
        OI_UINT8 numPkts = 0;
        OI_UINT16 len = dataLen;
        OI_UINT16 fragSize;

        while (len > 0) {
            if (0 == numPkts) {
                fragSize = c->outMTU - START_HEADER_LEN;
            }
            else {
                fragSize = c->outMTU - CONTINUE_HEADER_LEN;
            }

            len -= OI_MIN(fragSize, len);
            numPkts++;
        }
        OI_DBGPRINT2(("Data will require %d fragments\n", numPkts));

        ByteStream_PutUINT8(bs, ((transaction << 4) | (AVCTP_PKT_TYPE_START << 2) | ( (type == OI_AVCTP_TYPE_CMD) ? 0 : BIT1)) );
        ByteStream_PutUINT8(bs, numPkts);
        ByteStream_PutUINT16(bs, pid, AVCTP_BYTE_ORDER);
    }
    ByteStream_Close(bs);

    result = OI_MBUF_Append(mbuf, header, headerLen, MBUF_FREE);
    if (!OI_SUCCESS(result)) {
        goto Error;
    }
    result = OI_MBUF_Append(mbuf, data, c->writeCount, MBUF_KEEP);
    if (!OI_SUCCESS(result)) {
        goto Error;
    }

#ifndef OI_CODE
    /* OI_L2CAP_WriteMBUF will call the write callback before the function returns,
       so we must fill in the data here before the callback is called */
    c->pid = pid;
    c->transaction = transaction;
    c->type = type;
    c->writeBuf = data;
    c->writeBufLen = dataLen;
#endif /* !OI_CODE */
#ifndef OI_CODE
    result = OI_L2CAP_WriteMBUF(L2CAPWriteMbufCB,
                                c->cid, mbuf , NULL);
#else
#error code not present
#endif
    if (!OI_SUCCESS(result)) {
        goto Error;
    }
#ifdef OI_CODE
#error code not present
#endif /* OI_CODE */
    return result;

Error:
    OI_FreeIf(&header);
    OI_FreeIf(&mbuf);
    return result;
}

#endif /* FEATURE_BT_EXTPF_AV */


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

Copyright (c) 2003-2008 by QUALCOMM Incorporated. 
         All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/obexcommon.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
09/15/08   sp  Compiler warning fix

$Log: $
*
*    #8        18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #7        16 Aug 2006            RP
* Added check in ParseHeader() to ignore a PUT request in case of wrong
* encoding of the filename.
*
*    #6        10 Aug 2006            GS
* Added check in ParseHeader() to protect from malformed unicode strings.
*
*    #5        06 Oct 2005            JH
* Added check in ScanHeader() to ensure byte sequence and unicode strings have 
*  valid length fields.
*
*    #4        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
*    #3        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
===========================================================================*/
/**
@file  
functions shared between OBEX server and OBEX client
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_OBEX_SRV

#include "oi_obexcommon.h"
#include "oi_obexspec.h"
#include "oi_memmgr.h"
#include "oi_mbuf.h"
#include "oi_status.h"
#include "oi_assert.h"
#include "oi_text.h"
#include "oi_debug.h"
#include "oi_argcheck.h"


#ifdef OI_DEBUG
#error code not present
#else
#define HdrText(code) ""
#endif

#ifdef OI_DEBUG
#error code not present
#else
#define PktText(code) ""

#endif

#ifdef OI_DEBUG
#error code not present
#endif // OI_DEBUG




static OI_UINT16 HeaderSize(OI_OBEX_HEADER *header)
{
    OI_UINT16 length = 0;

    switch (OI_OBEX_HDR_KIND(header->id)) {
        case OI_OBEX_HDR_ID_UNICODE:
            length = OI_OBEX_HEADER_PREFIX_LEN;
            if (header->val.unicode.len > 0) {
                /*
                 * Non-empty unicode strings must be NULL terminated.
                 */
                if (header->val.unicode.str[header->val.unicode.len - 1] != 0) {
                    OI_LOG_ERROR(("OBEX unicode must be NUL terminated"));
                }
                length += header->val.unicode.len * sizeof(OI_CHAR16);
            }
            break;
        case OI_OBEX_HDR_ID_BYTESEQ:
            length = OI_OBEX_HEADER_PREFIX_LEN + header->val.byteseq.len;
            break;
        case OI_OBEX_HDR_ID_UINT8:
            length = sizeof(OI_UINT8) + sizeof(OI_UINT8);
            break;
        case OI_OBEX_HDR_ID_UINT32:
            length = sizeof(OI_UINT8) + sizeof(OI_UINT32);
            break;
        default:
            OI_LOG_ERROR(("Bad OBEX header"));
            break;
    }
    return length;
}


OI_UINT16 OI_OBEXCOMMON_HeaderListSize(OI_OBEX_HEADER_LIST const *headers)
{
    OI_UINT i;
    OI_UINT16 size = 0;

    if (headers != NULL) {
        for (i = 0; i < headers->count; ++i) {
            size += HeaderSize(headers->list + i);
        }
    }
    return size;
}


static OI_STATUS AppendHeader(OBEX_COMMON *common,
                              OI_MBUF *mbuf,
                              OI_OBEX_HEADER *header,
                              OI_UINT16 *pktLen)
{
    OI_UINT hdrId = header->id;
    OI_STATUS status;
    OI_BYTE tmp[8];
    OI_BYTE *buf;
    OI_BYTE_STREAM bs;
    OI_UINT i;
    OI_UINT16 pos;
    OI_UINT16 length = HeaderSize(header);


    if (length == 0) 
	{
        return OI_STATUS_INVALID_PARAMETERS;
    }
    /*
     * Check the packet has room for this header. We will segment body and
     * end-of-body headers across multiple packets if necessary.
     */
    if ((*pktLen + length) > common->maxSendPktLen) 
	{
        if (!OI_OBEX_IS_A_BODY_HEADER(header->id)) 
		{
            return OI_OBEX_PACKET_OVERFLOW;
        }
        OI_DBGPRINT(("Segmenting body header total=%d", length + *pktLen));
        /*
         * How much data will fit in this packet? 
         */
        length = common->maxSendPktLen - *pktLen;
        if (length <= OI_OBEX_HEADER_PREFIX_LEN) 
		{
            OI_DBGPRINT(("Segmenting body header - no room to send body this time"));
			
            return OI_OK;
        }
        OI_DBGPRINT(("Segmenting size=%d", length));
        OI_MemCopy(&common->bodySegment, header, sizeof(OI_OBEX_HEADER));
        common->bodySegment.val.byteseq.data += (length - OI_OBEX_HEADER_PREFIX_LEN);
        common->bodySegment.val.byteseq.len -= (length - OI_OBEX_HEADER_PREFIX_LEN);
        /*
         * Always a body not end-of-body
         */
        hdrId = OI_OBEX_HDR_BODY;
    }

    *pktLen += length;


	
    switch (OI_OBEX_HDR_KIND(hdrId)) 
	{
        case OI_OBEX_HDR_ID_UNICODE:
            /*
             * We want to preserve the byte order of the unicode string so we
             * have to allocate a buffer and copy the string over.
             */
            buf = OI_Malloc(length);
            if (buf == NULL) 
			{
                return OI_STATUS_OUT_OF_MEMORY;
            }
            ByteStream_Init(bs, buf, length);
            ByteStream_Open(bs, BYTESTREAM_WRITE);
            ByteStream_PutUINT8(bs, hdrId);
            ByteStream_PutUINT16(bs, length, OI_OBEX_BO);
            for (i = 0; i < header->val.unicode.len; ++i) 
			{
                ByteStream_PutUINT16(bs, header->val.unicode.str[i], OI_OBEX_BO);
            }
            /*
             * Unicode strings are supposed to be NUL terminated if this one
             * isn't HeaderSize() will have allocated space for one.
             */
            pos = ByteStream_GetPos(bs);
            if (pos < length) {
                ByteStream_PutUINT16(bs, 0, OI_OBEX_BO);
            }
            ByteStream_Close(bs);
            /*
             * MBUF will free the buffer when it is no longer needed.
             */
			
            status = OI_MBUF_Append(mbuf,
                                    ByteStream_GetDataPointer(bs),
                                    ByteStream_GetSize(bs),
                                    MBUF_FREE);

            break;
        case OI_OBEX_HDR_ID_BYTESEQ:
            tmp[0] = hdrId;
            SetUINT16_BigEndian(&tmp[1], length);
            status = OI_MBUF_Append(mbuf, tmp, OI_OBEX_HEADER_PREFIX_LEN, MBUF_COPY);

            if (OI_SUCCESS(status)) 
			{
                status = OI_MBUF_Append(mbuf, header->val.byteseq.data, (OI_UINT16)(length - OI_OBEX_HEADER_PREFIX_LEN), MBUF_KEEP);
            }
            break;
        case OI_OBEX_HDR_ID_UINT8:
            tmp[0] = hdrId;
            tmp[1] = header->val.uInt8;
            status = OI_MBUF_Append(mbuf, tmp, 2 * sizeof(OI_UINT8), MBUF_COPY);
            break;
        case OI_OBEX_HDR_ID_UINT32:
            tmp[0] = hdrId;
            SetUINT32_BigEndian(&tmp[1], header->val.uInt32);
            status = OI_MBUF_Append(mbuf, tmp, sizeof(OI_UINT8) + sizeof(OI_UINT32), MBUF_COPY);
            break;
        default:
            status = OI_FAIL;
    }

    return status;
}


/*
 * Marshall packet flags and headers into an mbuf.
 */

OI_STATUS OI_OBEXCOMMON_MarshalPacket(OBEX_COMMON *common,
                                      OI_BYTE_STREAM *pktHdr,
                                      OI_OBEX_HEADER *hdrs,
                                      OI_UINT16 hdrCount,
                                      OI_OBEX_HEADER_LIST const *hdrList)
{
    OI_STATUS status;
    OI_UINT8 cells;
    OI_UINT16 pos;
    OI_UINT16 pktLen;
    OI_UINT16 hdrCount2 = (hdrList == NULL) ? 0 : hdrList->count;
    OI_UINT i;
    OI_MBUF *mbuf = NULL;

    OI_DBGPRINT2(("OBEXCOMMON_MarshalPacket"));

    OI_ASSERT(common->mbuf == NULL);
    OI_ASSERT(common->bodySegment.id == 0);

#ifdef OI_DEBUG
#error code not present
#endif

    /*
     * One cell for the packet header + at least one cell per OBEX header.
     */
    cells = (OI_UINT8) (hdrCount + hdrCount2 + 1);
    /*
     * Byteseq headers use two mbuf cells.
     */
    for (i = 0; i < hdrCount; ++i) {
        if (OI_OBEX_HDR_KIND(hdrs[i].id) == OI_OBEX_HDR_ID_BYTESEQ) {
            ++cells;
        }
    }
    for (i = 0; i < hdrCount2; ++i) {
        if (OI_OBEX_HDR_KIND(hdrList->list[i].id) == OI_OBEX_HDR_ID_BYTESEQ) {
            ++cells;
        }
    }
    /*
     * Get the initial packet length.
     */
    pktLen = ByteStream_GetPos(*pktHdr);
    /*
     * Allocate and initialize the mbufs.
     */
    mbuf = OI_MBUF_Alloc(cells);
    OI_DBGPRINT2(("Allocating MBUF %lx", mbuf));
    if (mbuf == NULL) 
	{
        status = OI_STATUS_OUT_OF_MEMORY;
        goto ErrorExit;
    }
	
	
    for (i = 0; i < hdrCount; ++i) 
	{
        status = AppendHeader(common, mbuf, &hdrs[i], &pktLen);
        if (!OI_SUCCESS(status)) 
		{
            goto ErrorExit;
        }
    }

	
    for (i = 0; i < hdrCount2; ++i) 
	{
        status = AppendHeader(common, mbuf, &hdrList->list[i], &pktLen);
        if (!OI_SUCCESS(status)) 
		{
            goto ErrorExit;
        }
    }


    pos = ByteStream_GetPos(*pktHdr);
    ByteStream_SetPos(*pktHdr, 0);
    /*
     * The packet is not "final" if a body header had to be segmented.
     */
    if (OI_OBEX_IS_A_BODY_HEADER(common->bodySegment.id)) {
        OI_UINT op = common->currentOpcode;
        if (op == OI_OBEX_FINAL(OI_OBEX_RSP_OK) || (op == OI_OBEX_FINAL(OI_OBEX_RSP_CONTINUE))) {
            op = OI_OBEX_FINAL(OI_OBEX_RSP_CONTINUE);
        } else {
            op &= ~OI_OBEX_FINAL_BIT;
        }
        ByteStream_PutUINT8(*pktHdr, op);
    } else {
        ByteStream_PutUINT8(*pktHdr, common->currentOpcode);
    }
    ByteStream_PutUINT16(*pktHdr, pktLen, OI_OBEX_BO);
    ByteStream_SetPos(*pktHdr, pos);
    ByteStream_Close(*pktHdr);


	
    status = OI_MBUF_Prepend(mbuf,
                             ByteStream_GetDataPointer(*pktHdr),
                             ByteStream_GetSize(*pktHdr),
                             MBUF_COPY);


    if (!OI_SUCCESS(status)) 
	{
        goto ErrorExit;
    }

    OI_DBGPRINT2(("OBEXCOMMON_MarshalPacket len = %d", pktLen));

    OI_MBUF_SetWindow(mbuf, common->frameSize);

    common->mbuf = mbuf;

	
    return OI_OK;

ErrorExit:

    OI_DBGPRINT(("OBEXCOMMON_MarshalPacket error exit %!", status));

    if (mbuf) 
	{
        OI_MBUF_Free(mbuf);
    }
	
    return status;
}


OI_STATUS OI_OBEXCOMMON_MarshalBodySegment(OBEX_COMMON *common)
{
    OI_BYTE_STREAM pkt;
    OI_OBEX_HEADER hdr;

    OI_ASSERT(OI_OBEX_IS_A_BODY_HEADER(common->bodySegment.id));

    OI_DBGPRINT(("MarshalBodySegment"));

    OI_MemCopy(&hdr, &common->bodySegment, sizeof(OI_OBEX_HEADER));
    common->bodySegment.id = 0;

    OI_OBEXCOMMON_InitPacket(common, common->currentOpcode, &pkt);
    return OI_OBEXCOMMON_MarshalPacket(common, &pkt, &hdr, 1, NULL);
}


void OI_OBEXCOMMON_InitPacket(OBEX_COMMON *common,
                              OI_UINT8 opcode,
                              OI_BYTE_STREAM *pkt)
{
    OI_DBGPRINT2(("OI_OBEXCOMMON_InitPacket %s%s", PktText(opcode), OI_OBEX_IS_FINAL(opcode) ? "(final)" : ""));

    common->currentOpcode = opcode;
    common->bodySegment.id = 0;

    ByteStream_Init(*pkt, common->hdrBuf, OI_OBEX_HDR_BUFSIZE);
    ByteStream_Open(*pkt, BYTESTREAM_WRITE);
    /*
     * Packet opcode and size will be filled in later.
     */
    ByteStream_Skip(*pkt, sizeof(OI_UINT8) + sizeof(OI_UINT16));
}


static void WriteCfm_NOP(OI_RFCOMM_LINK_HANDLE link,
                         OI_UINT8 *dataBuf,
                         OI_UINT16 dataLen,
                         OI_STATUS result)
{
    /* DO NOTHING */
}


/**
 * Send a simple OBEX packet.
 */

OI_STATUS OI_OBEXCOMMON_SendSimple(OBEX_COMMON *common,
                                   OI_UINT8 opcode)
{
    OI_STATUS status;

    OI_DBGPRINT2(("OBEXCOMMON send %s%s", PktText(opcode), OI_OBEX_IS_FINAL(opcode) ? "(final)" : ""));

    common->hdrBuf[0] = opcode;
    common->hdrBuf[1] = 0;
    common->hdrBuf[2] = 3;
    status = OI_RFCOMM_Write(WriteCfm_NOP, common->link, common->hdrBuf, 3);
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("OBEXCOMMON send simple packet failed (%!)", status));
    }
    return status;
}


/**
 * Accumulates data received from an OBEX server in successive RFCOMM frames
 * until a complete OBEX packet has been assembled.
 */

OI_STATUS OI_OBEXCOMMON_ReassemblePacket(OBEX_COMMON *common,
                                         OI_BYTE *dataBuf,
                                         OI_UINT16 dataLen)
{
    OI_STATUS status;
    OI_UINT16 pktLen;
    OI_BOOL setRealSize = FALSE;

    OI_DBGPRINT2(("OBEXCOMMON ReassemblePacket\n%@", dataBuf, dataLen));

    /*
     * Because OBEX is a request/response protocol we expect to have processed
     * each packet before we get another one. If we get multiple packets
     * something has gone wrong.
     */
    if (common->rcvState == OI_OBEX_RCV_COMPLETE) {
        OI_LOG_ERROR(("OBEXCOMMON ReassemblePacket discarding data"));
        status = OI_OBEX_PACKET_OVERFLOW;
        goto ReceiveError;
    }

    /*
     * If this is a new packet, get the packet length to decide if we
     * need to do reassembly.
     */
    if (common->rcvState == OI_OBEX_RCV_BUF_EMPTY) {
        /* It is possible that the split point for a segmented packet be
         * positioned such that the packet length is not known from the data
         * in the first segment.  Therefore, default the bytestream to the the
         * max packet length supported and set the real size once the true
         * packet size is known.
         */
        ByteStream_Init(common->rcvPacket, common->rcvBuffer, common->maxRecvPktLen);
            ByteStream_Open(common->rcvPacket, BYTESTREAM_WRITE);
        common->rcvState = OI_OBEX_RCV_IN_PROGRESS;
        }

    /* Will there be enough data to determine the OBEX packet size? */
    if ((ByteStream_GetPos(common->rcvPacket) < OI_OBEX_SMALLEST_PKT) &&
        (ByteStream_GetPos(common->rcvPacket) + dataLen >= OI_OBEX_SMALLEST_PKT)) {
        setRealSize = TRUE;
    }

    /*
         * Check for packet overflow.
         */
        if (ByteStream_NumWriteBytesAllowed(common->rcvPacket) < dataLen) {
            status = OI_OBEX_PACKET_OVERFLOW;
            goto ReceiveError;
        }
        ByteStream_PutBytes(common->rcvPacket, dataBuf, dataLen);

    if (setRealSize) {
        /* There is enough information to get the real packet size. */
        pktLen = GetUINT16_BigEndian(ByteStream_GetDataPointer(common->rcvPacket) + sizeof(OI_UINT8));

        if (pktLen > common->maxRecvPktLen) {
            OI_LOG_ERROR(("OI_OBEXCOMMON_ReassemblePacket packet len %d exceeds buffer len %d\n",
                          pktLen, common->maxRecvPktLen));
            status = OI_OBEX_BAD_PACKET;
            goto ReceiveError;
        }

        if (dataLen > pktLen) {
            OI_DBGPRINT(("OI_OBEXCOMMON_ReassemblePacket data len %d >  packet len %d", dataLen, pktLen));
            status = OI_OBEX_BAD_PACKET;
            goto ReceiveError;
        }

        ByteStream_SetSize(common->rcvPacket, pktLen);
    }

    /*
     * If we cannot write any more data then we have a complete packet.
     */
    if (ByteStream_NumWriteBytesAllowed(common->rcvPacket) == 0) {
        pktLen = ByteStream_GetSize(common->rcvPacket);
        ByteStream_Close(common->rcvPacket);

        /* Re-initialize the bytestream variables without touching the buffer
         * contents. */
        ByteStream_Init(common->rcvPacket, common->rcvBuffer, pktLen);
        ByteStream_Open(common->rcvPacket, BYTESTREAM_READ);
        common->rcvState = OI_OBEX_RCV_COMPLETE;

        OI_DBGPRINT2(("Received %s%s len=%d",
                      PktText(*(ByteStream_GetDataPointer(common->rcvPacket))),
                      OI_OBEX_IS_FINAL(*(ByteStream_GetDataPointer(common->rcvPacket))) ? "(final)" : "",
                      ByteStream_GetSize(common->rcvPacket)));
    }

    return OI_OK;

ReceiveError:

    /*
     * Discard incomplete packets.
     */
    if (common->rcvState == OI_OBEX_RCV_IN_PROGRESS) {
        common->rcvState = OI_OBEX_RCV_BUF_EMPTY;
    }
#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OBEXCOMMON Received Packet error"));
#endif

    return status;
}


static OI_STATUS ScanHeader(OI_BYTE_STREAM *bs)
{
    OI_UINT16 id = 0;
    OI_UINT16 len = 0;

    ByteStream_GetUINT8_Checked(*bs, id);

    switch (OI_OBEX_HDR_KIND(id)) {
        case OI_OBEX_HDR_ID_BYTESEQ:
        case OI_OBEX_HDR_ID_UNICODE:
            ByteStream_GetUINT16_Checked(*bs, len, OI_OBEX_BO);
            if ((len < OI_OBEX_HEADER_PREFIX_LEN) ||
                (len > (ByteStream_NumReadBytesAvail(*bs) + OI_OBEX_HEADER_PREFIX_LEN))) {
#ifdef OI_CODE
#error code not present
#else
              OI_LOG_ERROR(("Invalid OBEX Header prefix len"));
#endif
              return OI_OBEX_ERROR;
            }
            ByteStream_Skip_Checked(*bs, len - OI_OBEX_HEADER_PREFIX_LEN);
            break;
        case OI_OBEX_HDR_ID_UINT8:
            ByteStream_Skip_Checked(*bs, sizeof(OI_UINT8));
            break;
        case OI_OBEX_HDR_ID_UINT32:
            ByteStream_Skip_Checked(*bs, sizeof(OI_UINT32));
            break;
        default:
            return OI_OBEX_ERROR;
    }
    if (ByteStream_Error(*bs)) {
        return OI_OBEX_ERROR;
    } else {
        return OI_OK;
    }
}


#ifdef OI_CODE
#error code not present
#else
static OI_STATUS ParseHeader(OI_BYTE_STREAM *bs,
                             OI_OBEX_HEADER *header)
#endif

{
    OI_BYTE *base;
    OI_CHAR16 dchar = 0;
    OI_UINT i;
    OI_UINT16 len;

    ByteStream_GetUINT8_Checked(*bs, header->id);

    switch (OI_OBEX_HDR_KIND(header->id)) {
        case OI_OBEX_HDR_ID_UNICODE:
            ByteStream_GetUINT16(*bs, len, OI_OBEX_BO);
            if ((len < OI_OBEX_HEADER_PREFIX_LEN) ||
                (len > (ByteStream_NumReadBytesAvail(*bs) + OI_OBEX_HEADER_PREFIX_LEN))) {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Invalid OBEX Header prefix len"));
            return OI_OBEX_ERROR;
#endif
            }
            header->val.unicode.len = (len - OI_OBEX_HEADER_PREFIX_LEN) / sizeof(OI_CHAR16);
#ifndef OI_CODE
            if(!(len & 0x0001))
            {
                return OI_OBEX_ERROR;
            }
#endif
            if (header->val.unicode.len) {
                /*
                 * Some architecture require that arrays of 16 bit values are
                 * aligned on a 16 bit boundary. We can safely adjust the start
                 * address of the unicode string to a 16 bit boundary because we
                 * no longer need the preceding length byte.
                 */
                base = ByteStream_GetCurrentBytePointer(*bs);
                if (((OI_UINT32) base) & 1) {
                    --base;
                }
                header->val.unicode.str = (OI_CHAR16*) base;
                /*
                 * Do in-place byte order conversion for double byte characters.
                 */
                for (i = 0; i < header->val.unicode.len; ++i) {
#ifdef OI_CODE
#error code not present
#else
                    ByteStream_GetUINT16_Checked(*bs, dchar, OI_OBEX_BO);
                    if (ByteStream_Error(*bs)) {
                      return OI_OBEX_ERROR;
                    }
#endif
                    header->val.unicode.str[i] = dchar;
                }
            } else {
                header->val.unicode.str = NULL;
            }
            break;
        case OI_OBEX_HDR_ID_BYTESEQ:
            ByteStream_GetUINT16(*bs, len, OI_OBEX_BO);
            if ((len < OI_OBEX_HEADER_PREFIX_LEN) ||
                (len > (ByteStream_NumReadBytesAvail(*bs) + OI_OBEX_HEADER_PREFIX_LEN))) {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Invalid OBEX Header prefix len"));
            return OI_OBEX_ERROR;
#endif
            }
            header->val.byteseq.len = len - OI_OBEX_HEADER_PREFIX_LEN;
            if (header->val.byteseq.len) {
                header->val.byteseq.data = (OI_BYTE*) ByteStream_GetCurrentBytePointer(*bs);
            } else {
                header->val.byteseq.data = NULL;
            }
            ByteStream_Skip(*bs, len - OI_OBEX_HEADER_PREFIX_LEN);
            break;
        case OI_OBEX_HDR_ID_UINT8:
            ByteStream_GetUINT8(*bs, header->val.uInt8);
            break;
        case OI_OBEX_HDR_ID_UINT32:
            ByteStream_GetUINT32(*bs, header->val.uInt32, OI_OBEX_BO);
            break;
        default:
            OI_LOG_ERROR(("OBEX Common Parse Header corrupt header"));
#ifdef OI_CODE
#error code not present
#else
            return OI_OBEX_ERROR;
#endif
    }
#ifndef OI_CODE
    return OI_OK;
#endif
}


/**
 * Look for a specific OBEX header in a header list.
 */

OI_OBEX_HEADER* OI_OBEXCOMMON_FindHeader(OI_OBEX_HEADER_LIST const *headers,
                                         OI_UINT8 headerId)
{
    OI_UINT i;

    for (i = 0; i < headers->count; ++i) {
        if (headers->list[i].id == headerId) {
            return headers->list + i;
        }
    }
    return NULL;
}


/**
 * Get all headers. This is done without copying data from the original
 * bytestream, so headers reference directly into the appropriate locations in
 * the bytestream. As a side-effect the byte stream may be modified.
 */

OI_STATUS OI_OBEXCOMMON_ParseHeaderList(OI_OBEX_HEADER_LIST *headers,
                                        OI_BYTE_STREAM *bs)
{
    OI_STATUS status;
    OI_UINT i;
    OI_UINT16 pos;

    /*
     * First count the headers.
     */
    headers->count = 0;
    headers->list = NULL;
    pos = ByteStream_GetPos(*bs);
    while (ByteStream_NumReadBytesAvail(*bs) > 0) {
        status = ScanHeader(bs);
        if (!OI_SUCCESS(status)) {
            headers->count = 0;
            return OI_OBEX_BAD_PACKET;
        }
        ++headers->count;
    }
    ByteStream_SetPos(*bs, pos);

    if (headers->count > 0) {
        headers->list = OI_Malloc(sizeof(OI_OBEX_HEADER) * headers->count);
        if (headers->list == NULL) {
            return OI_STATUS_OUT_OF_MEMORY;
        }
        /*
         * Now get the header values into the header list. We checked that
         * the header were all valid in the scan pass.
         */
        for (i = 0; i < headers->count; ++i) {
#ifdef OI_CODE
#error code not present
#else
            status = ParseHeader(bs, headers->list + i);
            if (!OI_SUCCESS(status)) {
                return OI_OBEX_BAD_PACKET;
            }
#endif
        }
    }
    ByteStream_Close(*bs);

#ifdef OI_DEBUG
#error code not present
#endif

    return OI_OK;
}

static OI_STATUS ScanAppParamsEntry(OI_BYTE_STREAM *bs)
{
    OI_UINT8 len = 0;

    ByteStream_Skip_Checked(*bs, sizeof(OI_UINT8)); /* The length doesn't depend on the value of the param tag */
    ByteStream_GetUINT8_Checked(*bs, len);
    ByteStream_Skip_Checked(*bs, len);

    if (ByteStream_Error(*bs)) {
        return OI_OBEX_ERROR;
    } else {
        return OI_OK;
    }
}

static void ParseAppParamsEntry(OI_BYTE_STREAM *bs,
                                     OI_OBEX_APP_PARAM *param)
{
   /* Not using checked bytestream operations because ScanAppParamsEntry has already validated the field layout */
    ByteStream_GetUINT8(*bs, param->tag);
    ByteStream_GetUINT8(*bs, param->len);
    param->data = ByteStream_GetCurrentBytePointer(*bs);
    ByteStream_Skip(*bs, param->len);
}

OI_STATUS OI_OBEXCOMMON_ParseAppParamsHeader(const OI_OBEX_BYTESEQ *rawData,
                                             OI_OBEX_APP_PARAM_LIST *params)
{
    OI_BYTE_STREAM bs;
    OI_STATUS status;
    OI_UINT i;
    
    ByteStream_Init(bs, rawData->data, rawData->len);
    ByteStream_Open(bs, BYTESTREAM_READ);

    params->count = 0;
    params->list = NULL;

    while (ByteStream_NumReadBytesAvail(bs) > 0) {
        status = ScanAppParamsEntry(&bs);
        if (!OI_SUCCESS(status)) {
            params->count = 0;
            return OI_OBEX_BAD_PACKET;
        }
        params->count++;
    }
    ByteStream_SetPos(bs, 0);

    if (params->count > 0) {
        params->list = OI_Malloc(sizeof(OI_OBEX_APP_PARAM) * params->count);
        if (params->list == NULL) {
            return OI_STATUS_OUT_OF_MEMORY;
        }
        /*
         * Now get the param values into the header list. We checked that
         * the values were all valid during the scan pass.
         */
        for (i = 0; i < params->count; ++i) {
            ParseAppParamsEntry(&bs, params->list + i);
        }
    }
    ByteStream_Close(bs);

    return OI_OK;
}

OI_STATUS OI_OBEXCOMMON_BuildAppParamsHeader(OI_OBEX_BYTESEQ *data,
                                             const OI_OBEX_APP_PARAM_LIST *params)
{
    OI_UINT i;
    OI_BYTE_STREAM bs;
    
    data->len = 0;
    for (i = 0; i < params->count; i++) {
        data->len += sizeof(params->list[i].tag) + sizeof(params->list[i].len) + params->list[i].len;
    }

    data->data = OI_Malloc(data->len);
    if (data->data == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    ByteStream_Init(bs, data->data, data->len);
    ByteStream_Open(bs, BYTESTREAM_WRITE);

    for (i = 0; i < params->count; i++) {
        ByteStream_PutUINT8(bs, params->list[i].tag);
        ByteStream_PutUINT8(bs, params->list[i].len);
        ByteStream_PutBytes(bs, params->list[i].data, params->list[i].len);
    }

    OI_ASSERT(ByteStream_NumWriteBytesAllowed(bs) == 0);

    return OI_OK;
}

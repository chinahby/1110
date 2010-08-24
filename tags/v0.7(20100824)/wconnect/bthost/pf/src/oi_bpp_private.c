/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bpp_private.c#1 $ 

$Log: $

===========================================================================*/
/**
 * @file  
 * Basic Printing Profile private functions
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_common.h"
#include "oi_bpp_printer.h"
#define __OI_MODULE__ OI_MODULE_BPP_SENDER      // sender and printer both in here

#include "oi_bpp_private.h"
#include "oi_debug.h"
#include "oi_endian.h"

static const OI_BYTE targetDpsBytes[16] = { 0x00, 0x00, 0x11, 0x18, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };
static const OI_BYTE targetPbrBytes[16] = { 0x00, 0x00, 0x11, 0x19, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };
static const OI_BYTE targetRuiBytes[16] = { 0x00, 0x00, 0x11, 0x21, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };
static const OI_BYTE targetStsBytes[16] = { 0x00, 0x00, 0x11, 0x23, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };
static const OI_BYTE targetObjBytes[16] = { 0x00, 0x00, 0x11, 0x20, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };

const OI_OBEX_BYTESEQ OI_BPP_ObexTarget_DPS = { OI_ARRAYSIZE(targetDpsBytes), (OI_BYTE *)targetDpsBytes };
const OI_OBEX_BYTESEQ OI_BPP_ObexTarget_PBR = { OI_ARRAYSIZE(targetPbrBytes), (OI_BYTE *)targetPbrBytes };
const OI_OBEX_BYTESEQ OI_BPP_ObexTarget_RUI = { OI_ARRAYSIZE(targetRuiBytes), (OI_BYTE *)targetRuiBytes };
const OI_OBEX_BYTESEQ OI_BPP_ObexTarget_STS = { OI_ARRAYSIZE(targetStsBytes), (OI_BYTE *)targetStsBytes };
const OI_OBEX_BYTESEQ OI_BPP_ObexTarget_OBJ = { OI_ARRAYSIZE(targetObjBytes), (OI_BYTE *)targetObjBytes };

static const OI_CHAR refSimpleBytes[] = BPP_MIMETYPE_REF_SIMPLE;
static const OI_CHAR refXmlBytes[] = BPP_MIMETYPE_REF_XML;
static const OI_CHAR refListBytes[] = BPP_MIMETYPE_REF_LIST;
static const OI_CHAR soapBytes[] = BPP_MIMETYPE_SOAP;
static const OI_CHAR refObjBytes[] = BPP_MIMETYPE_REFOBJ;
static const OI_CHAR ruiBytes[] = BPP_MIMETYPE_RUI;

const OI_OBEX_BYTESEQ OI_BPP_MimeType_RefSimple = { OI_ARRAYSIZE(refSimpleBytes), (OI_BYTE *)refSimpleBytes };
const OI_OBEX_BYTESEQ OI_BPP_MimeType_RefXML = { OI_ARRAYSIZE(refXmlBytes), (OI_BYTE *)refXmlBytes };
const OI_OBEX_BYTESEQ OI_BPP_MimeType_RefList = { OI_ARRAYSIZE(refListBytes), (OI_BYTE *)refListBytes };
const OI_OBEX_BYTESEQ OI_BPP_MimeType_SOAP = { OI_ARRAYSIZE(soapBytes), (OI_BYTE *)soapBytes };
const OI_OBEX_BYTESEQ OI_BPP_MimeType_ReferencedObject = { OI_ARRAYSIZE(refObjBytes), (OI_BYTE *)refObjBytes };
const OI_OBEX_BYTESEQ OI_BPP_MimeType_RUI = { OI_ARRAYSIZE(ruiBytes), (OI_BYTE *)ruiBytes };


void OI_BPP_AddJobIdHeader(OI_OBEX_HEADER_LIST *hdrList, OI_BPP_JOB_ID jobId, OI_BYTE *storage)
{
    OI_OBEX_BYTESEQ appParam;
    
    storage[0] = BPP_OBEX_APP_PARAM_TAG_JOBID;
    storage[1] = sizeof(jobId);
    SetUINT32_BigEndian(&storage[2], jobId);
    appParam.data = storage;
    appParam.len = 2 + sizeof(jobId);
    hdrList->list[hdrList->count].id = OI_OBEX_HDR_APPLICATION_PARAMS;
    hdrList->list[hdrList->count].val.applicationParams = appParam;
    hdrList->count++;
}

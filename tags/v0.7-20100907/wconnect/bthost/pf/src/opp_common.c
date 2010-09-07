/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/opp_common.c#1 $ 

$Log: $
*
*
*    #6       18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
*    #5        09 Mar 2005            JH
* Modified OPPCommon_ParseHeaders() to continue processing body/end-of-body 
*  header in case a bogus name/type/length header is received. This prevents 
*  data from being dropped.
*
*    #4        05 Oct 2005            JH
* Modified OPPCommon_ParseHeaders() to call IsBaseName() to reject objects with 
*  names containing "/", backslash, "..", or ":".
*
*    #3        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
===========================================================================*/
/**
@file  
@internal
  
Object Push Profile common functions
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_OPP_SRV

#include "oi_opp_common.h"
#include "oi_obexsrv.h"
#include "oi_memmgr.h"
#include "oi_debug.h"
#include "oi_assert.h"

#ifndef OI_CODE
/* This function is defined in ftp_server.c. */
extern OI_BOOL IsBaseName(OI_OBEX_UNICODE *name);
#endif

OI_STATUS OPPCommon_ParseHeaders(OI_OPP_GENERIC_OBJECT *obj,
                                 OI_OBEX_HEADER_LIST *headers)
{
    OI_STATUS status;
    OI_OBEX_UNICODE *uname;
    OI_OBEX_BYTESEQ *type;
    OI_UINT i;

    for (i = 0; i < headers->count; ++i) {
        switch (headers->list[i].id) {
            case OI_OBEX_HDR_NAME:
                if (obj->name.str != NULL) {
                    /*
                     * Ignore the duplicate name header.
                     */
#ifdef OI_CODE
#error code not present
#else
                    /* continue to process the body header to prevent dropping
                       data */
                    break;
#endif /* OI_CODE */
                }
                uname = &(headers->list[i].val.name);
                if (!IsBaseName(uname)) {
#ifdef OI_CODE
#error code not present
#else
                /* Reject names containing "/", backslash, "..", or ":" */
                  status = OI_OBEX_VALUE_NOT_ACCEPTABLE;
#endif
                  goto ObjCleanup;
                }
                obj->name.str = OI_Malloc(uname->len * sizeof(OI_CHAR16));
                if (obj->name.str == NULL) {
                    status = OI_STATUS_OUT_OF_MEMORY;
                    goto ObjCleanup;
                }
                obj->name.len = uname->len;
                OI_MemCopy(obj->name.str, uname->str, uname->len * sizeof(OI_CHAR16));
                break;
            case OI_OBEX_HDR_TYPE:
                if (obj->type.data != NULL) {
                    /*
                     * Ignore the duplicate type header.
                     */
#ifdef OI_CODE
#error code not present
#else
                    /* continue to process the body header to prevent dropping
                       data */
                    break;
#endif /* OI_CODE */
                }
                type = &(headers->list[i].val.type);
                obj->type.data = OI_Malloc(type->len);
                if (obj->type.data == NULL) {
                    status = OI_STATUS_OUT_OF_MEMORY;
                    goto ObjCleanup;
                }
                obj->type.len = type->len;
                OI_MemCopy(obj->type.data, type->data, type->len);
                break;
            case OI_OBEX_HDR_LENGTH:
                if (obj->objSize != 0) {
                    /*
                     * Ignore the bogus length header.
                     */
#ifdef OI_CODE
#error code not present
#else
                    /* continue to process the body header to prevent dropping
                       data */
                    break;
#endif /* OI_CODE */
                }
                type = &(headers->list[i].val.type);
                obj->objSize = headers->list[i].val.length;
                if (obj->objSize == 0) {
                    status = OI_OBEX_VALUE_NOT_ACCEPTABLE;
                    goto ObjCleanup;
                }
                break;
            case OI_OBEX_HDR_BODY:
                obj->objData = headers->list[i].val.body;
                break;
            case OI_OBEX_HDR_END_OF_BODY:
                obj->objData = headers->list[i].val.body;
                /*
                 * The Sony/Ericcsson T68i does not provide the mandatory length
                 * header so we have to figure out the length of the object from
                 * the body header.
                 */
                if (obj->objSize == 0) {
                    obj->objSize = obj->objData.len;
                }
                break;
            default:
                /* Other headers are ignored. */
                break;
        }
    }

    return OI_OK;

ObjCleanup:

    OI_FreeIf(&obj->name.str);
    OI_FreeIf(&obj->type.data);

    return status;
}


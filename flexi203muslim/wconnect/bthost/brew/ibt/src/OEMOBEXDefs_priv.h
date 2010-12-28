#ifndef OEMOBEXDEFS_PRIV_H
#define OEMOBEXDEFS_PRIV_H
/*=============================================================================
FILE:  OEMOBEXDefs_priv.h

SERVICES: Defines APIs datatypes that are invoked by the OEM OBEX layer

GENERAL DESCRIPTION:

===============================================================================
           Copyright (c) 2007-2008 QUALCOMM Incorporated.
                All Rights Reserved.
           Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMOBEXDefs_priv.h#1 $ 
$DateTime: 2009/01/07 18:14:54 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
09/15/08   sp  Compiler warning fix

=============================================================================*/


#include "OEMOBEXDefs.h"
#include "mt_obex.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "AEEStdLib.h"
#include "AEEOBEXComDefs.h"

#define OEMOBEX_DEBUG 1
#if defined MSG_DEBUG
#undef MSG_DEBUG
#endif
#if defined OEMOBEX_DEBUG
#define MSG_DEBUG MSG_HIGH
#else
#define MSG_DEBUG MSG_LOW
#endif
//=============================================================================
//  Type definitions
//============================================================================= 

#ifdef  SMALL_RAM_MEMORY

#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
#define OEMOBEX_MAX_RECV_BUFFER_SIZE    (32768 / 2)
#else
#define OEMOBEX_MAX_RECV_BUFFER_SIZE     32768
#endif /* FEATURE_BT_REDUCE_MEMORY_USAGE */ 
#else
#define OEMOBEX_MAX_RECV_BUFFER_SIZE 4277
#endif /* FEATURE_BT_SOC */
#else
#error code not present
#endif


//=============================================================================
//  OEM private function def
//============================================================================= 
int OEMOBEXHdr_MtToAeeHeaders(uint8                    hdrCnt,
                              mt_obex_hdr_type         *pMtHdr,
                              OEMOBEXHdr               *pOemRecvHdr,
                              boolean                  bFlagDisc,
                              bt_app_id_type           appId);
AEEOBEXCmdStatus OEMOBEX_MtToAeeStatus(mt_obex_status_type status);
int OEMOBEX_CmdStatusToAeeResult(mt_obex_status_type status);
mt_obex_security_type OEMOBEX_OEMToMtSecurity(OEMOBEX_BtSecurity OemSec);
mt_obex_status_type OEMOBEX_AEEToMtStatus(AEEOBEXCmdStatus status);
mt_obex_srv_auth_type OEMOBEX_AeeToMtObexAuth( OEMOBEX_AuthType auth);
int OEMOBEXHdr_GetNameHeader(OEMOBEXHdr *pOemHdr, uint16 *pathName, int nLen);
bt_pf_goep_obex_hdr_list_type* OEMOBEXHdr_GetHeaderList(OEMOBEXHdr *pOemHdr);

#endif /* OEMOBEXDEFS_PRIV_H */


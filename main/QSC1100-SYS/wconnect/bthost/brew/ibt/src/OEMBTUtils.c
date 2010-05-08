/*=============================================================================
FILE:         OEMBTUtils.c

SERVICES:     Utility functions used by all IBT interfaces.

DESCRIPTION:  Utility functions used by all IBT interfaces.

===============================================================================
           Copyright (c) 2007-2008 QUALCOMM Incorporated.
                All Rights Reserved.
           Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTUtils.c#2 $ 
$DateTime: 2009/02/24 19:20:32 $
$Author: c_hshukl $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2009-24-02  HS   Adding the error code to qualify PTS compliance issue. 
2008-11-25  gb   Replaced usage of FEATURE_IBT_DYNAMIC_BMP_1_0 flag with
                 FEATURE_BMP.
2008-10-15  vk   Removing compiler warnings.
2008-10-06  rs   Adding the feature flag to avoid inclucion of features not 
                 yet a part of BREW IBT release.
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "OEMBTUtils.h"
#include "AEEStdLib.h"
#include "AEEStdErr.h"

#ifdef FEATURE_BMP
#include "AEEFSErr.h"
#endif /* FEATURE_BMP */

#include "AEEShell.h"
#include "AEE_OEM.h"

#ifdef BT_SIMULATION
#include "btmsg.h"
#else
#include "msg.h"
#endif

void OEMBT_BDAddrToStr(const BDAddress* pAddr, char* pszStr)
{
  uint8   i, p = sizeof(BDAddress) * 2;
  uint16  j, k;

  for (i = 0; i < sizeof(BDAddress); i++)
  {
    j = (uint16) pAddr->uAddr[i];
    k = j & 0x000F;
    pszStr[--p] = ( k <= 9 ) ? k + '0' : k - 10 + 'A';
    k = ( j >> 4 ) & 0x000F;
    pszStr[--p] = ( k <= 9 ) ? k + '0' : k - 10 + 'A';
  }
}

int OEMBT_StrToBDAddr (const char* pszStr, BDAddress* pAddr)
{
   uint8 i, j = 0, nNumDigits = sizeof (BDAddress) * 2;
   uint8 k = nNumDigits;
   char c;
   boolean bSuccess = TRUE;

   for (i = 0; (i < nNumDigits) && (FALSE != bSuccess); i++)
   {
      c = pszStr[--k];
      if ((c >= '0') && (c <= '9'))
      {
         j = (c - '0');
      }
      else if ((c >= 'a') && (c <= 'f'))
      {
         j = (c - 'a' + 10);
      }
      else if ((c >= 'A') && (c <= 'F'))
      {
         j = (c - 'A' + 10);
      }
      else
      {
         bSuccess = FALSE;
      }
      if (k % 2 == 0) // even number?
      {
         j <<= 4;
         pAddr->uAddr[i>>1] |= j;         
      }
      else
      {   
         pAddr->uAddr[i>>1] = j;
      }
   }
   return (FALSE == bSuccess) ? AEE_EFAILED : AEE_SUCCESS;
}

int OEMBT_CmdStatus2AEEResult (bt_cmd_status_type stat)
{
   switch (stat)
   {
      case BT_CS_GN_SUCCESS:
      case BT_CS_GN_PENDING:
      case BT_CS_PF_PENDING:
         return AEE_SUCCESS;
      case BT_CS_GN_CMD_Q_FULL:
         return AEE_ENOMEMORY;
      default:
         return AEE_EFAILED;
   }
}

BTResult OEMBT_CmdStatus2Result (bt_cmd_status_type stat)
{
   switch (stat)
   {
      case BT_CS_GN_SUCCESS:
      case BT_CS_GN_PENDING:
         return AEEBT_RESULT_SUCCESS;
      case BT_CS_GN_CMD_Q_FULL:
         return AEEBT_RESULT_NO_RESOURCE;
      case BT_CS_RM_UNRECOGNIZED_CMD:
         return AEEBT_RESULT_INVALID_OPERATION;
      case BT_CS_RM_MAX_CONNECTIONS:
      case BT_CS_RM_MAX_DEVICES:
      case BT_CS_RM_MAX_APPLICATIONS:
         return AEEBT_RESULT_NO_RESOURCE;
      case BT_CS_RM_BAD_HANDLE:
         return AEEBT_RESULT_INVALID_HANDLE;
      case BT_CS_RM_BUSY:
         return AEEBT_RESULT_OPERATION_IN_PROGRESS;
      case BT_CS_RM_NO_CONNECTION:
          return AEEBT_RESULT_NOT_CONNECTED;
      case BT_CS_RM_CONN_ACTIVE:
          return AEEBT_RESULT_ALREADY_CONNECTED;
      case BT_CS_RM_CMD_NOT_ALLOWED:
          return AEEBT_RESULT_CMD_NOT_ALLOWED;
      case BT_CS_RM_BAD_PARAM:
         return AEEBT_RESULT_BAD_PARAM;
      case BT_CS_PF_OBEX_PARTIAL_CONTENT:
         return AEEBT_RESULT_OBEX_PARTIAL_CONTENT;
      case BT_CS_PF_OBEX_CONTINUE:
          return AEEBT_RESULT_CONTINUE;
      case BT_CS_PF_OBEX_BAD_REQUEST:
         return AEEBT_RESULT_OBEX_BAD_REQUEST;
      case BT_CS_PF_OBEX_NOT_FOUND:
         return AEEBT_RESULT_OBEX_FOLDER_NOT_FOUND;
      case BT_CS_PF_OBEX_NO_SUCH_FOLDER:
         return AEEBT_RESULT_OBEX_FOLDER_NOT_FOUND;
      case BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR:
         return AEEBT_RESULT_OBEX_INTERNAL_SERVER_ERROR;
      case BT_CS_PF_OBEX_UNAUTHORIZED:
         return AEEBT_RESULT_OBEX_UNAUTHORIZED;
      case BT_CS_PF_OBEX_ACCESS_DENIED:
         return AEEBT_RESULT_OBEX_FORBIDDEN;
      case BT_CS_PF_OBEX_NOT_IMPLEMENTED:
         return AEEBT_RESULT_OBEX_NOT_IMPLEMENTED;
      case BT_CS_PF_OBEX_CLIENT_ABORTED_COMMAND:
          return AEEBT_RESULT_OBEX_CLIENT_ABORTED_COMMAND;
      default:
         return AEEBT_RESULT_UNKNOWN_ERR;
   }
}

bt_service_class_enum_type OEMBT_SvcClsBrew2Core (BTClassOfService dCoS)
{
   switch (dCoS)
   {
      case AEEBT_COS_UNKNOWN:
         return BT_SERVICE_CLASS_UNKNOWN;
      case AEEBT_COS_LIMITED_DISCOVERABLE_MODE:
         return BT_SERVICE_CLASS_LIMITED_DISCOVERABLE_MODE;
      case AEEBT_COS_NETWORKING:
         return BT_SERVICE_CLASS_NETWORKING;
      case AEEBT_COS_RENDERING:
         return BT_SERVICE_CLASS_RENDERING;
      case AEEBT_COS_CAPTURING:
         return BT_SERVICE_CLASS_CAPTURING;
      case AEEBT_COS_OBJECT_TRANSFER:
         return BT_SERVICE_CLASS_OBJECT_TRANSFER;
      case AEEBT_COS_AUDIO:
         return BT_SERVICE_CLASS_AUDIO;
      case AEEBT_COS_TELEPHONY:
         return BT_SERVICE_CLASS_TELEPHONY;
      case AEEBT_COS_INFORMATION:
         return BT_SERVICE_CLASS_INFORMATION;
      case AEEBT_COS_POSITIONING:
         return BT_SERVICE_CLASS_POSITIONING;
      case AEEBT_COS_ALL:
         return BT_SERVICE_CLASS_ALL;
      default:
         return BT_SERVICE_CLASS_UNKNOWN;
   }
}

bt_cmd_status_type OEMBT_AEEResult2ObexStat(AEEResult result)
{
   switch (result)
   {
      case AEE_SUCCESS:
         return BT_CS_GN_SUCCESS;
      case AEE_ENEEDMORE:
         return BT_CS_PF_OBEX_CONTINUE;
      case AEE_ENOTALLOWED:
      case AEE_EPRIVLEVEL:
         return BT_CS_PF_OBEX_ACCESS_DENIED;
      case AEE_ENOMORE:
         return BT_CS_PF_OBEX_SERVICE_UNAVAILABLE;
      case AEE_ENOSUCH:
      case AEE_EBADHANDLE:
         return BT_CS_PF_OBEX_NOT_FOUND;
      case AEE_EBADITEM:
         return BT_CS_PF_OBEX_BAD_REQUEST;

#ifdef FEATURE_BMP
      case AEEFS_ENOMEDIA:
         return BT_CS_PF_OBEX_UNSUPPORTED_MEDIA_TYPE;
      case AEEFS_EDIRNOEXISTS:
         return BT_CS_PF_OBEX_NO_SUCH_FOLDER;
      case AEEFS_EINVALIDOPERATION:
         return BT_CS_PF_OBEX_METHOD_NOT_ALLOWED;
#endif /* FEATURE_BMP */
      default:
         MSG_LOW("AEEResult=0x%x", result, 0, 0);
         return BT_CS_PF_OBEX_FILEOP_ERROR; // what other error to use?
   }
}

AEEResult OEMBT_SetServiceSecurity(
   bt_app_id_type  app_id,
   BTSecurityLevel cSec,
   bt_sd_uuid_type uuid
)
{
   bt_service_id_type service_ident;
   bt_cmd_status_type stat;
   bt_security_type   minSec;    // either this...
   bt_sec_level_type  sm4_sec;   // or this
   boolean            bAuth = FALSE;

   // set default values
   minSec = BT_SEC_AUTHENTICATE_AND_ENCRYPT;
   sm4_sec = BT_RM_SL_3_HIGH;

   switch (cSec)
   {
      case AEEBT_SEC_NONE:
         minSec = BT_SEC_NONE;
         break;
      case AEEBT_SEC_AUTHENTICATE:
         minSec = BT_SEC_AUTHENTICATE;
         break;
      case AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT:
         minSec = BT_SEC_AUTHENTICATE_AND_ENCRYPT;
         break;
#ifdef FEATURE_BMP
      case AEEBT_SEC_SM4_LOW:
         sm4_sec = BT_RM_SL_1_LOW;
         break;
      case AEEBT_SEC_SM4_MEDIUM:
         sm4_sec = BT_RM_SL_2_MEDIUM;
         break;
      case AEEBT_SEC_SM4_HIGH:
         sm4_sec = BT_RM_SL_3_HIGH;
         break;
#endif /* FEATURE_BMP */
      default:
         minSec = BT_SEC_AUTHENTICATE_AND_ENCRYPT;
         sm4_sec = BT_RM_SL_3_HIGH;
         break;
   }
   service_ident.service_id_method = BT_SIM_SDP_UUID;
   service_ident.sdp_uuid          = uuid;

   DBGPRINTF("SetServiceSecurity sm4_sec %d minSec %d", sm4_sec, minSec);
#ifdef FEATURE_BT_2_1
   stat = bt_cmd_rm_set_sm4(app_id, 
                            &service_ident, 
                            sm4_sec,
                            bAuth, 
                            FALSE);
#else
   stat = bt_cmd_rm_set_service_security(app_id, 
                                         &service_ident, 
                                         minSec, 
                                         bAuth, 
                                         FALSE);
#endif
   return (OEMBT_CmdStatus2AEEResult(stat));
}

#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


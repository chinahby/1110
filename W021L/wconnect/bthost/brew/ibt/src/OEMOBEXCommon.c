/*=============================================================================
FILE:  OEMOBEXCommon.c

SERVICES:  Common functions between OBEX Server and client.


GENERAL DESCRIPTION:
        These are OEM specific functions called from OEM obex files
   

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMOBEXCommon.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP)
#include "OEMOBEXDefs.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "mt_obex.h"
#include "AEEOBEXComDefs.h"
#include "AEEStdErr.h"

//=============================================================================
// 
//
//=============================================================================
void OEMOBEX_GetSupTransport(uint32 *bitmap)
{
   *bitmap |= OEMOBEX_TRANS_BT_RFCOMM;
   return;

}

//=============================================================================
// 
//
//=============================================================================
AEEOBEXCmdStatus OEMOBEX_MtToAeeStatus(mt_obex_status_type status)
{
  switch ( status )
  {
      case BT_CS_GN_SUCCESS:
         return AEEOBEX_STAT_SUCCESS;
      case BT_CS_PF_INVALID_PARAMETERS:
         return AEEOBEX_STAT_ERROR;
      case BT_CS_PF_NO_RESOURCES:
      case BT_CS_PF_OBEX_NO_MORE_CONNECTIONS:
         return AEEOBEX_STAT_NO_RESOURCE;
      case BT_CS_PF_OBEX_METHOD_NOT_ALLOWED:
      case BT_CS_PF_OBEX_ACCESS_DENIED:
         return AEEOBEX_STAT_METHOD_NOT_ALLOWED;
      case BT_CS_PF_OBEX_REQUIRED_HEADER_NOT_FOUND:
         return AEEOBEX_STAT_ERROR_REQUIRED_HEADER_NOT_FOUND;
      case BT_CS_PF_OBEX_CONNECT_FAILED:
         return AEEOBEX_STAT_CONNECT_FAILED;
      case BT_CS_PF_OBEX_CONNECTION_TIMEOUT:
         return AEEOBEX_STAT_CONNECTION_TIMEOUT;
      case BT_CS_PF_OBEX_NO_SUCH_FOLDER:
         return AEEOBEX_STAT_NO_SUCH_FOLDER;
      case BT_CS_PF_OBEX_INCOMPLETE_PACKET:
         return AEEOBEX_STAT_INCOMPLETE_PACKET;
      case BT_CS_PF_OBEX_CONTINUE:
         return AEEOBEX_STAT_CONTINUE;
      case BT_CS_PF_OBEX_LENGTH_REQUIRED:
         return AEEOBEX_STAT_LENGTH_REQUIRED;
      case BT_CS_PF_OBEX_DATABASE_LOCKED:
         return AEEOBEX_STAT_DATABASE_LOCKED;
      case BT_CS_PF_OBEX_NOT_MODIFIED:
         return AEEOBEX_STAT_NOT_MODIFIED;
      case BT_CS_PF_OBEX_GONE:
         return AEEOBEX_STAT_GONE;
      case BT_CS_PF_OBEX_REQ_TIMEOUT:
         return AEEOBEX_STAT_REQ_TIMEOUT;
      case BT_CS_PF_OBEX_PRECONDITION_FAILED:
         return AEEOBEX_STAT_PRECONDITION_FAILED;
      case BT_CS_PF_OBEX_SERVICE_UNAVAILABLE :
         return AEEOBEX_STAT_SERVICE_UNAVAILABLE;
      case BT_CS_PF_OBEX_NOT_FOUND:
         return AEEOBEX_STAT_NOT_FOUND;
      case BT_CS_PF_OBEX_VALUE_NOT_ACCEPTABLE:
         return AEEOBEX_STAT_VALUE_NOT_ACCEPTABLE;
      case BT_CS_PF_OBEX_DATABASE_FULL:
         return AEEOBEX_STAT_DATABASE_FULL;
      case BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR:
         return AEEOBEX_STAT_INTERNAL_SERVER_ERROR;
      case BT_CS_PF_OBEX_UNSUPPORTED_MEDIA_TYPE:
         return AEEOBEX_STAT_UNSUPPORTED_MEDIA_TYPE;
      case BT_CS_PF_OBEX_UNAUTHORIZED:
         return AEEOBEX_STAT_UNAUTHORIZED;
      case BT_CS_PF_OBEX_PARTIAL_CONTENT:
         return AEEOBEX_STAT_PARTIAL_CONTENT;
      default:
       return AEEOBEX_STAT_ERROR;
  }
}
//=============================================================================
// 
//
//=============================================================================
mt_obex_status_type OEMOBEX_AEEToMtStatus(AEEOBEXCmdStatus status)
{
   switch (status)
   {

      case AEEOBEX_STAT_SUCCESS: 
        return MT_OBEX_SUCCESS;
      case AEEOBEX_STAT_CONTINUE:
        return MT_OBEX_CONTINUE;
      case AEEOBEX_STAT_COMMAND_ERROR:
        return MT_OBEX_COMMAND_ERROR;
      case AEEOBEX_STAT_CONNECTION_TIMEOUT:
        return MT_OBEX_CONNECTION_TIMEOUT;
      case AEEOBEX_STAT_CONNECT_FAILED:
        return MT_OBEX_CONNECT_FAILED;
      case AEEOBEX_STAT_DISCONNECT_FAILED:
        return MT_OBEX_DISCONNECT_FAILED;
      case AEEOBEX_STAT_ERROR:
        return MT_OBEX_ERROR ;
      case AEEOBEX_STAT_INCOMPLETE_PACKET:
        return MT_OBEX_INCOMPLETE_PACKET ;
      case AEEOBEX_STAT_LENGTH_REQUIRED:
        return MT_OBEX_LENGTH_REQUIRED ;
      case AEEOBEX_STAT_NOT_CONNECTED:
        return MT_OBEX_NOT_CONNECTED ;
      case AEEOBEX_STAT_NO_MORE_CONNECTIONS:
        return MT_OBEX_NO_MORE_CONNECTIONS ;
      case AEEOBEX_STAT_OPERATION_IN_PROGRESS:
        return MT_OBEX_OPERATION_IN_PROGRESS ;
      case AEEOBEX_STAT_PUT_RESPONSE_ERROR:
        return MT_OBEX_PUT_RESPONSE_ERROR ;
      case AEEOBEX_STAT_GET_RESPONSE_ERROR: 
        return MT_OBEX_GET_RESPONSE_ERROR ;
      case AEEOBEX_STAT_ERROR_REQUIRED_HEADER_NOT_FOUND:
        return MT_OBEX_REQUIRED_HEADER_NOT_FOUND ;
      case AEEOBEX_STAT_SERVICE_UNAVAILABLE:
        return MT_OBEX_SERVICE_UNAVAILABLE ;
      case AEEOBEX_STAT_TOO_MANY_HEADER_BYTES:
        return MT_OBEX_TOO_MANY_HEADER_BYTES ;
      case AEEOBEX_STAT_UNKNOWN_COMMAND:
        return MT_OBEX_UNKNOWN_COMMAND ;
      case AEEOBEX_STAT_UNSUPPORTED_VERSION:
        return MT_OBEX_UNSUPPORTED_VERSION ;
      case AEEOBEX_STAT_CLIENT_ABORTED_COMMAND:
        return MT_OBEX_CLIENT_ABORTED_COMMAND;
      case AEEOBEX_STAT_BAD_PACKET:
        return MT_OBEX_BAD_PACKET;
      case AEEOBEX_STAT_BAD_REQUEST:
        return MT_OBEX_BAD_REQUEST;
      case AEEOBEX_STAT_OBJECT_OVERFLOW:
        return MT_OBEX_OBJECT_OVERFLOW;
      case AEEOBEX_STAT_NOT_FOUND:
        return MT_OBEX_NOT_FOUND;
      case AEEOBEX_STAT_ACCESS_DENIED:
        return MT_OBEX_ACCESS_DENIED;
      case AEEOBEX_STAT_VALUE_NOT_ACCEPTABLE:
        return MT_OBEX_VALUE_NOT_ACCEPTABLE;
      case AEEOBEX_STAT_PACKET_OVERFLOW:
        return MT_OBEX_PACKET_OVERFLOW;
      case AEEOBEX_STAT_NO_SUCH_FOLDER:
        return MT_OBEX_NO_SUCH_FOLDER;
      case AEEOBEX_STAT_NAME_REQUIRED:
        return MT_OBEX_NAME_REQUIRED;
      case AEEOBEX_STAT_PASSWORD_TOO_LONG:
        return MT_OBEX_PASSWORD_TOO_LONG;
      case AEEOBEX_STAT_PRECONDITION_FAILED:
        return MT_OBEX_PRECONDITION_FAILED;
      case AEEOBEX_STAT_UNAUTHORIZED:
        return MT_OBEX_UNAUTHORIZED ;
      case AEEOBEX_STAT_NOT_IMPLEMENTED:
        return MT_OBEX_NOT_IMPLEMENTED ;
      case AEEOBEX_STAT_INVALID_AUTH_DIGEST:
        return MT_OBEX_INVALID_AUTH_DIGEST;
      case AEEOBEX_STAT_INVALID_OPERATION:
        return MT_OBEX_INVALID_OPERATION;
      case AEEOBEX_STAT_DATABASE_FULL:
        return MT_OBEX_DATABASE_FULL ;
      case AEEOBEX_STAT_DATABASE_LOCKED:
        return MT_OBEX_DATABASE_LOCKED;
      case AEEOBEX_STAT_INTERNAL_SERVER_ERROR:
        return MT_OBEX_INTERNAL_SERVER_ERROR;
      case AEEOBEX_STAT_UNSUPPORTED_MEDIA_TYPE:
        return MT_OBEX_UNSUPPORTED_MEDIA_TYPE;
      case AEEOBEX_STAT_PARTIAL_CONTENT:
        return MT_OBEX_PARTIAL_CONTENT;
      case AEEOBEX_STAT_METHOD_NOT_ALLOWED:
      case AEEOBEX_STAT_FORBIDDEN:
        return MT_OBEX_METHOD_NOT_ALLOWED ;
      case AEEOBEX_STAT_SRV_INCOMPLETE_GET:
        return MT_OBEX_OBEXSRV_INCOMPLETE_GET ;
      case AEEOBEX_STAT_FOLDER_BROWSING_NOT_ALLOWED:
        return MT_OBEX_FOLDER_BROWSING_NOT_ALLOWED;
      case AEEOBEX_STAT_SERVER_FORCED_DISCONNECT:
        return MT_OBEX_SERVER_FORCED_DISCONNECT; 
      case AEEOBEX_STAT_CONNECTION_NOT_FOUND:
        return  MT_OBEX_CONNECTION_NOT_FOUND;
      case AEEOBEX_STAT_NOT_MODIFIED:
        return MT_OBEX_NOT_MODIFIED;
      case AEEOBEX_STAT_GONE:
        return MT_OBEX_GONE;
      case AEEOBEX_STAT_REQ_TIMEOUT:
        return MT_OBEX_REQ_TIMEOUT;
      default: return MT_OBEX_COMMAND_ERROR;

  }
}
//=============================================================================
// 
//
//=============================================================================
mt_obex_security_type OEMOBEX_OEMToMtSecurity(OEMOBEX_BtSecurity OemSec)
{
   switch (OemSec)
   {
      case OEMOBEX_BT_SEC_NONE:
         return MT_OBEX_SEC_NONE;
      case OEMOBEX_BT_SEC_AUTHENTICATE:
         return MT_OBEX_SEC_AUTHENTICATE;
      case OEMOBEX_BT_SEC_AUTHENTICATE_AND_ENCRYPT:
         return MT_OBEX_AUTHENTICATE_AND_ENCRYPT;
      case OEMOBEX_BT_SEC_MAX:
         return MT_OBEX_SEC_MAX;
      default:
         break;
   }
   return MT_OBEX_SEC_NONE;
}
//=============================================================================
// 
//
//=============================================================================

int OEMOBEX_CmdStatusToAeeResult(mt_obex_status_type status)
{
   switch (status)
   {
      case BT_CS_GN_SUCCESS:
      case BT_CS_GN_PENDING:
         return AEE_SUCCESS;
      case BT_CS_GN_CMD_Q_FULL:
      case BT_CS_PF_OUT_OF_MEMORY:
         return AEE_ENOMEMORY;
      case BT_CS_PF_INVALID_PARAMETERS:
         return AEE_EBADPARM;
      default:
         return AEE_EFAILED;
   }
}
//=============================================================================
// 
//
//=============================================================================
mt_obex_srv_auth_type OEMOBEX_AeeToMtObexAuth( OEMOBEX_AuthType auth)
{
   switch (auth)
   {
      case OEMOBEX_SRV_AUTH_NONE:
         return MT_OBEX_SRV_AUTH_NONE;
      case OEMOBEX_SRV_AUTH_PASSWORD :
         return MT_OBEX_SRV_AUTH_PASSWORD;
      case OEMOBEX_SRV_AUTH_USERID_AND_PASSWORD:
         return MT_OBEX_SRV_AUTH_USERID_AND_PASSWORD;
      default :
         break;
   }
   return MT_OBEX_SRV_AUTH_NONE;
}
#endif /* #if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */


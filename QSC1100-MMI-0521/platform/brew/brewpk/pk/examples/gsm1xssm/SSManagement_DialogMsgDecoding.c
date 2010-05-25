/*=========================================================================
FILE: SSManagement_DialogMsgDecoding.c

SERVICES: GSM1x SS Management Dialog Message Decoding

GENERAL DESCRIPTION:
    SS Management dialog message decoding and handling functionality

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */

#include "SSManagement_App.h"
#include "SSManagement_DialogMsgDecoding.h"

#include "ssmanagement.bid"	  // Applet class ID
#include "gsm1xssm_res.h"	    // Applet Resources

#include "AEEShell.h"         // AEE Shell Services
#include "AEEText.h"          // AEE Text Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "SSMsg.h"
#include "SS_CommonMacros.h" 

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

// Macro used to make sure that a condition is true, when decoding messages.
#define VerifyIsTrue(x)                                                     \
    {                                                                       \
        if (!(x))                                                           \
        {                                                                   \
            GSM1X_LOG_ERROR(pMe->m_pSSUtilCls, "SSM:Verify Failed! line:%d",__LINE__,0,0); \
            return SS_DECODING_MSG_INVALID;                                 \
        }                                                                   \
    }                                                                       \


/*===========================================================================
                               MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

SSmsgDecodedResultStatus HandleReturnResultComponent(
    CGSM1xSSManagementApp               *pMe,
    SSMsg_component_return_result_type *pResult
);

SSmsgDecodedResultStatus HandleActivateOrDeactivateOrRegisterOrEraseResult(
    CGSM1xSSManagementApp               *pMe,
    SSMsg_component_return_result_type *pResult
);

SSmsgDecodedResultStatus HandleInterrogationResult(
    CGSM1xSSManagementApp               *pMe,
    SSMsg_component_return_result_type *pResult
);

SSmsgDecodedResultStatus HandleChangePasswordResult(
    CGSM1xSSManagementApp               *pMe,
    SSMsg_component_return_result_type *pResult
);

SSmsgDecodedResultStatus HandleForwardingFeatureList(
    CGSM1xSSManagementApp                     *pMe,
    SSMsg_param_forwarding_feature_list_type *pFeatureList
);

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: DialogMsgDecoding_HandleResultMsg

DESCRIPTION: process and decode a result message

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   pGeneralMsg [in]: Specifies the message that needs to be handled

RETURN VALUE:
   SSmsgDecodedResultStatus: decoded message status

=============================================================================*/
SSmsgDecodedResultStatus DialogMsgDecoding_HandleResultMsg(
   CGSM1xSSManagementApp *pMe,
   SSMsg_int_msg_type   *pGeneralMsg
)
{
    SSMsg_msg_release_complete_type *pMsg;
    SSmsgDecodedResultStatus         status;
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:E:MsgDecoding_HandleResultMsg",0,0,0);

    VerifyIsTrue( pGeneralMsg != NULL);
    VerifyIsTrue( pGeneralMsg->gen_hdr.msg_type == SSMSG_MSG_TYPE_RELEASE_COMPLETE );


    // start decoding the release complete message
    pMsg = &(pGeneralMsg->release_complete);

    // no valid status found yet
    pMe->m_bResultStatusValid = FALSE;

    if (!pMsg->facility.hdr.present) 
    {
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:X:MsgDecoding_HandleResultMsg EndTR",0,0,0);
        return SS_DECODING_MSG_RESULT_END_TRANSACTION;
    }
    
    VerifyIsTrue(pMsg->facility.component.gen_hdr.present);             

    switch (pMsg->facility.component.gen_hdr.component_type)
    {
        case SSMSG_COMPONENT_TYPE_REJECT:
        {
            VerifyIsTrue( pMsg->facility.component.reject.hdr.present );
            VerifyIsTrue( pMsg->facility.component.reject.problem_code.hdr.present );

            pMe->m_cResultFailureCode = ( (pMsg->facility.component.reject.problem_code.problem_tag << 8) |
                                          (pMsg->facility.component.reject.problem_code.value) );

            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:Reject: tagcode-%d\n",pMe->m_cResultFailureCode,0,0);
            status = SS_DECODING_MSG_RESULT_REJECTED;
            break;
        }

        case SSMSG_COMPONENT_TYPE_RETURN_ERROR:
        {
            VerifyIsTrue( pMsg->facility.component.return_error.hdr.present );
            VerifyIsTrue( pMsg->facility.component.return_error.error_code.hdr.present );

            pMe->m_cResultFailureCode = pMsg->facility.component.return_error.error_code.value;

            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:Return Error: code-%d\n",pMe->m_cResultFailureCode,0,0);
            status = SS_DECODING_MSG_RESULT_ERROR_CODE;
            break;
        }

        case SSMSG_COMPONENT_TYPE_RETURN_RESULT:
            status = HandleReturnResultComponent(pMe, &(pMsg->facility.component.return_result));
            break;

        default:
            // for SSMSG_COMPONENT_TYPE_INVOKE
            status = SS_DECODING_MSG_INVALID;
            break;
    }

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:X:MsgDecoding_HandleResultMsg status %d",status,0,0);
    return status;
}

/*=============================================================================
FUNCTION: HandleReturnResultComponent

DESCRIPTION: handles the processing of a return result component.

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   pResult [in]: Specifies the return result component that needs to be handled

RETURN VALUE:
   SSmsgDecodedResultStatus: decoded message status

=============================================================================*/
SSmsgDecodedResultStatus HandleReturnResultComponent(
    CGSM1xSSManagementApp               *pMe,
    SSMsg_component_return_result_type *pResult
)
{
    switch (pMe->m_cSelectedOperation)
    {
        case MANAGEMENET_OPERATION_REGISTER:
            if (pResult->operation_code.hdr.present) 
            {
                VerifyIsTrue( pResult->operation_code.value == SSMSG_OPERATION_CODE_REGISTERSS );
            }
            return HandleActivateOrDeactivateOrRegisterOrEraseResult(pMe, pResult);

        case MANAGEMENET_OPERATION_ACTIVATE:
            if (pResult->operation_code.hdr.present) 
            {
                VerifyIsTrue( pResult->operation_code.value == SSMSG_OPERATION_CODE_ACTIVATESS );
            }
            return HandleActivateOrDeactivateOrRegisterOrEraseResult(pMe, pResult);

        case MANAGEMENET_OPERATION_DEACTIVATE:
            if (pResult->operation_code.hdr.present) 
            {
                VerifyIsTrue( pResult->operation_code.value == SSMSG_OPERATION_CODE_DEACTIVATESS );
            }
            return HandleActivateOrDeactivateOrRegisterOrEraseResult(pMe, pResult);

        case MANAGEMENET_OPERATION_ERASE:
            if (pResult->operation_code.hdr.present) 
            {
                VerifyIsTrue( pResult->operation_code.value == SSMSG_OPERATION_CODE_ERASESS );
            }
            return HandleActivateOrDeactivateOrRegisterOrEraseResult(pMe, pResult);
   
        case MANAGEMENET_OPERATION_INTERROGATE:
            if (pResult->operation_code.hdr.present)
            {
                VerifyIsTrue( pResult->operation_code.value == SSMSG_OPERATION_CODE_INTERROGATESS );
            }
            return HandleInterrogationResult(pMe, pResult);

        case MANAGEMENET_OPERATION_CHANGE_PASSWORD:
            if (pResult->operation_code.hdr.present)
            {
                VerifyIsTrue( pResult->operation_code.value == SSMSG_OPERATION_CODE_REGISTERPASSWORD );
            }
            return HandleChangePasswordResult(pMe, pResult);

        default:
           assert( !"Unknown Operation Id" );
           return SS_DECODING_APP_ERROR; // suppress warnings ...
    }
}

/*=============================================================================
FUNCTION: HandleActivateOrDeactivateOrRegisterOrEraseResult

DESCRIPTION: handles the processing of a return result component
             for an activation, registration, de-activation or erase.

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   pResult [in]: Specifies the return result component that needs to be handled

RETURN VALUE:
   SSmsgDecodedResultStatus: decoded message status

=============================================================================*/
SSmsgDecodedResultStatus HandleActivateOrDeactivateOrRegisterOrEraseResult(
    CGSM1xSSManagementApp               *pMe,
    SSMsg_component_return_result_type *pResult
)
{
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:E:HandleActiveDeactiveRegisterEraseResult",0,0,0);

    switch (pMe->m_ServiceCategory)
    {
    case ID_CALL_DIVERTING:

        if (pResult->forwarding_info.hdr.present ) 
        {
            SSmsgDecodedResultStatus status;
            VerifyIsTrue( pResult->forwarding_info.forwarding_feature_list.hdr.present );

            status = HandleForwardingFeatureList(pMe, &(pResult->forwarding_info.forwarding_feature_list));
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:X:HandleActiveDeactiveRegisterEraseResult",0,0,0);
            return status;
        }
        break;

    case ID_CALL_BARRING:
        if ( pResult->call_barring_info.hdr.present )
        {
            VerifyIsTrue( pResult->call_barring_info.call_barring_feature_list.hdr.present );
            VerifyIsTrue( pResult->call_barring_info.call_barring_feature_list.call_barring_feature_count > 0);

            if ( pResult->call_barring_info.call_barring_feature_list.call_barring_feature[0].ss_status.hdr.present )
            {
                pMe->m_bResultStatusActive = pResult->call_barring_info.call_barring_feature_list.call_barring_feature[0].ss_status.active;
                pMe->m_bResultStatusQuiescent = pResult->call_barring_info.call_barring_feature_list.call_barring_feature[0].ss_status.quiescent;
                pMe->m_bResultStatusValid = TRUE;
            }
        }
        break;

    case ID_CALL_WAITING:
        if ( pResult->ss_data.hdr.present )
        {
            if (pResult->ss_data.ss_code.hdr.present)
            {
                VerifyIsTrue( pMe->m_cSScode == pResult->ss_data.ss_code.value );
            }

            if (pResult->ss_data.ss_status.hdr.present)
            {
                pMe->m_bResultStatusActive = pResult->ss_data.ss_status.active;
                pMe->m_bResultStatusQuiescent = pResult->ss_data.ss_status.quiescent;
                pMe->m_bResultStatusValid = TRUE;
            }
        }
        break;

    case ID_LINE_IDENTIFICATION:
        if ( pResult->ss_data.hdr.present )
        {
            if (pResult->ss_data.ss_code.hdr.present)
            {
                VerifyIsTrue( pMe->m_cSScode == pResult->ss_data.ss_code.value);
            }

            if (pResult->ss_data.ss_status.hdr.present)
            {
                pMe->m_bResultStatusActive = pResult->ss_data.ss_status.active;
                pMe->m_bResultStatusQuiescent = pResult->ss_data.ss_status.quiescent;
                pMe->m_bResultStatusValid = TRUE;
            }
        }
        break;

    default:
        assert( !"Unknown First Menu Choice" );
        return SS_DECODING_APP_ERROR;
    }

   GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:X:HandleActiveDeactiveRegisterEraseResult",0,0,0);
   return SS_DECODING_MSG_RESULT_SUCCESS;
}

/*=============================================================================
FUNCTION: HandleInterrogationResult

DESCRIPTION: handles the processing of a return result component
             for an interrogation.

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   pResult [in]: Specifies the return result component that needs to be handled

RETURN VALUE:
   SSmsgDecodedResultStatus: decoded message status

=============================================================================*/
SSmsgDecodedResultStatus HandleInterrogationResult(
    CGSM1xSSManagementApp               *pMe,
    SSMsg_component_return_result_type *pResult
)
{
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:E:HandleInterrogationResult",0,0,0);

    switch (pMe->m_ServiceCategory)
    {
    case ID_CALL_DIVERTING:
        if ( pResult->ss_status.hdr.present )
        {
            // this means service is in-active, (or active & quiescent ?)
            pMe->m_bResultStatusActive = pResult->ss_status.active;
            pMe->m_bResultStatusQuiescent = pResult->ss_status.quiescent;
            pMe->m_bResultStatusValid = TRUE;
        }
        else
        {   // service should be active ...
            SSmsgDecodedResultStatus result;
            result = HandleForwardingFeatureList(pMe, &(pResult->forwarding_feature_list));

            // make sure that status was found!
            if (pMe->m_bResultStatusValid) 
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:X:HandleInterrogationResult res:%d",result,0,0);
                return result;
            }
            else 
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:X:HandleInterrogationResult Invalid",0,0,0);
                return SS_DECODING_MSG_INVALID;
            }
        }
        break;

    case ID_CALL_BARRING:
        if ( pResult->ss_status.hdr.present )
        {
            // this means service is in-active, (or active & quiescent ?)
            pMe->m_bResultStatusActive = pResult->ss_status.active;
            pMe->m_bResultStatusQuiescent = pResult->ss_status.quiescent;
            pMe->m_bResultStatusValid = TRUE;
        }
        else
        {
            // this means service is active
            // no other field should exist ... ?
            pMe->m_bResultStatusActive = TRUE;
            pMe->m_bResultStatusValid = TRUE;
        }
        break;

    case ID_CALL_WAITING:
        if ( pResult->ss_status.hdr.present )
        {
            // this means service is in-active, (or active & quiescent ?)
            pMe->m_bResultStatusActive = pResult->ss_status.active;
            pMe->m_bResultStatusQuiescent = pResult->ss_status.quiescent;
            pMe->m_bResultStatusValid = TRUE;
        }
        else
        {
            // this means service is active
            // no other field should exist ... ?
            pMe->m_bResultStatusActive = TRUE;
            pMe->m_bResultStatusValid = TRUE;
        }
        break;

    case ID_LINE_IDENTIFICATION:
        switch (pMe->m_ServiceSpecific)
        {
            case ID_LINE_IDENTIFICATION_CLIP:
                if ( pResult->ss_status.hdr.present )
                {
                    // this means service is in-active, (or active & quiescent ?)
                    pMe->m_bResultStatusActive = pResult->ss_status.active;
                    pMe->m_bResultStatusQuiescent = pResult->ss_status.quiescent;
                    pMe->m_bResultStatusValid = TRUE;
                }
                else
                {
                    // this means service is active
                    // no other field should exist ... ?
                    pMe->m_bResultStatusActive = TRUE;
                    pMe->m_bResultStatusValid = TRUE;
                }
                break;

            case ID_LINE_IDENTIFICATION_CLIR:
                if ( pResult->ss_status.hdr.present )
                {
                    // this means service is in-active, (or active & quiescent ?)
                    pMe->m_bResultStatusActive = pResult->ss_status.active;
                    pMe->m_bResultStatusQuiescent = pResult->ss_status.quiescent;
                    pMe->m_bResultStatusValid = TRUE;
                }
                else
                {
                    // this means service is active
                    VerifyIsTrue( pResult->generic_service_info.hdr.present );
                    pMe->m_bResultStatusActive = pResult->generic_service_info.ss_status.active;
                    pMe->m_bResultStatusQuiescent = pResult->generic_service_info.ss_status.quiescent;
                    pMe->m_bResultStatusValid = TRUE;
                }
                break;

            default:
            {                                                                   
                GSM1X_LOG_ERROR(pMe->m_pSSUtilCls, "SSM:unknown specific service",0,0,0); 
                return SS_DECODING_MSG_INVALID;                                 
            }                                                                   
        }
        break;

    default:
       assert( !"Unknown First Menu Choice" );
       return SS_DECODING_APP_ERROR;
    }

   GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:X:HandleInterrogationResult",0,0,0);
   return SS_DECODING_MSG_RESULT_SUCCESS;
}

/*=============================================================================
FUNCTION: HandleChangePasswordResult

DESCRIPTION: handles the processing of a return result component
             for a change password.
PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   pResult [in]: Specifies the return result component that needs to be handled.

RETURN VALUE:
   SSmsgDecodedResultStatus: decoded message status

=============================================================================*/
SSmsgDecodedResultStatus HandleChangePasswordResult(
    CGSM1xSSManagementApp               *pMe,
    SSMsg_component_return_result_type *pResult
)
{
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:E:HandleChangePasswordResult",0,0,0);

    if (pResult->new_password.hdr.present)
    {
        VerifyIsTrue( pResult->new_password.password_len == STRLEN(pMe->m_cNewPassword));
        VerifyIsTrue( STRCMP((char*)(pResult->new_password.password), pMe->m_cNewPassword) == 0);
    }

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:X:HandleChangePasswordResult",0,0,0); 
    return SS_DECODING_MSG_RESULT_SUCCESS;
}

/*=============================================================================
FUNCTION: HandleForwardingFeatureList

DESCRIPTION: handles the processing of a forwarding feature list.

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   pFeatureList [in]: Specifies the forwarding feature list that needs to be handled.

RETURN VALUE:
   SSmsgDecodedResultStatus: decoded message status

=============================================================================*/
SSmsgDecodedResultStatus HandleForwardingFeatureList(
    CGSM1xSSManagementApp                     *pMe,
    SSMsg_param_forwarding_feature_list_type *pFeatureList
)
{
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:E:HandleForwardingFeatureList",0,0,0);
    VerifyIsTrue( pFeatureList->forwarding_feature_count > 0);

    if ( pFeatureList->forwarding_feature[0].ss_status.hdr.present )
    {
        pMe->m_bResultStatusActive = pFeatureList->forwarding_feature[0].ss_status.active;
        pMe->m_bResultStatusQuiescent = pFeatureList->forwarding_feature[0].ss_status.quiescent;
        pMe->m_bResultStatusValid = TRUE;
    }

    if (pFeatureList->forwarding_feature[0].forwarded_to_number.hdr.present)
    {
        VerifyIsTrue( pFeatureList->forwarding_feature[0].forwarded_to_number.address_len < sizeof(pMe->m_cResultForwardedToNumber) );

        STRNCPY(pMe->m_cResultForwardedToNumber,
                (char*)pFeatureList->forwarding_feature[0].forwarded_to_number.address,
                pFeatureList->forwarding_feature[0].forwarded_to_number.address_len);

        pMe->m_cResultForwardedToNumber[ pFeatureList->forwarding_feature[0].forwarded_to_number.address_len ] = '\0';
        pMe->m_bResultForwardedToNumberValid = TRUE;
    }

    if (pFeatureList->forwarding_feature[0].long_ftn.hdr.present)
    {
        VerifyIsTrue( pFeatureList->forwarding_feature[0].long_ftn.address_len < sizeof(pMe->m_cResultForwardedToNumber) );

        STRNCPY(pMe->m_cResultForwardedToNumber,
                (char*)pFeatureList->forwarding_feature[0].long_ftn.address,
                pFeatureList->forwarding_feature[0].long_ftn.address_len);

        pMe->m_cResultForwardedToNumber[ pFeatureList->forwarding_feature[0].long_ftn.address_len ] = '\0';
        pMe->m_bResultForwardedToNumberValid = TRUE;
    }

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:X:HandleForwardingFeatureList",0,0,0);
    return SS_DECODING_MSG_RESULT_SUCCESS;
}

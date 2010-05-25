/*=========================================================================
FILE: SSFacilityExtToInt.c

SERVICES: GSM1x SS messages translation - Facility IE.

GENERAL DESCRIPTION:
    Translation for Facility IE from external format (raw buffer) into internal
    format (c structs). For translation specifications, see GSM1x 
    Supplementary Services document and GSM 24.080.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSFacility.h"
#include "SSMsg.h"
#include "SS_CommonMacros.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

//********************************************************************************************************************************
//
// Parameters translation
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: ext_skip_invoke_id

DESCRIPTION:
    Translate invoke id from external to internal format.
    The invoke id parameter has no meaning in GSM1x and so translation for it 
    is actually checking for validity and then skipping it without any further
    processing.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the tag octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    if invoke id is not found or is not valid, *poffset is not changed.
=============================================================================*/
SSUtil_error_type ext_skip_invoke_id(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset)
{
    uint8 offset = *poffset;

    // check for invoke id tag
    if (buf[offset] != SSMSG_PARAMETER_TAG_INVOKE_ID)
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    // skip tag
    offset++;

    // invoke id length should be 1
    if (buf[offset] != 1)
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    // skip length + value
    offset+=2;

    *poffset = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_operation_code

DESCRIPTION:
    Translate operation code parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_operation_code [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_operation_code(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_operation_code_type* int_operation_code)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];
    uint8 operation_code_value;

    // ensure length is 1
    if (buf[offset] != 1)
    {
        // unexpected operation code length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // ensure enough room for parameter value
    if (++offset >= skip_parameter_offset)
    {
        // unexpectedly reached the end of parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    
    switch (operation_code_value = buf[offset])
    {
    case SSMSG_OPERATION_CODE_REGISTERSS:
    case SSMSG_OPERATION_CODE_ERASESS:
    case SSMSG_OPERATION_CODE_ACTIVATESS:
    case SSMSG_OPERATION_CODE_DEACTIVATESS:
    case SSMSG_OPERATION_CODE_INTERROGATESS:
    case SSMSG_OPERATION_CODE_NOTIFYSS:
    case SSMSG_OPERATION_CODE_REGISTERPASSWORD:
    case SSMSG_OPERATION_CODE_GETPASSWORD:
    case SSMSG_OPERATION_CODE_PROCESSUNSTRUCTUREDSS_DATA:
    case SSMSG_OPERATION_CODE_FORWARDCHECKSS_INDICATION:
    case SSMSG_OPERATION_CODE_PROCESSUNSTRUCTUREDSS_REQUEST:
    case SSMSG_OPERATION_CODE_UNSTRUCTUREDSS_REQUEST:
    case SSMSG_OPERATION_CODE_UNSTRUCTUREDSS_NOTIFY:
    case SSMSG_OPERATION_CODE_ERASECCENTRY:
    case SSMSG_OPERATION_CODE_CALLDEFLECTION:
    case SSMSG_OPERATION_CODE_USERUSERSERVICE:
    case SSMSG_OPERATION_CODE_ACCESSREGISTERCCENTRY:
    case SSMSG_OPERATION_CODE_FORWARDCUG_INFO:
    case SSMSG_OPERATION_CODE_SPLITMPTY:
    case SSMSG_OPERATION_CODE_RETRIEVEMPTY:
    case SSMSG_OPERATION_CODE_HOLDMPTY:
    case SSMSG_OPERATION_CODE_BUILDMPTY:
    case SSMSG_OPERATION_CODE_FORWARDCHARGEADVICE:
    case SSMSG_OPERATION_CODE_EXPLICITCT:
        int_operation_code->value = operation_code_value;
        offset++;      // skip value octet
        break;
    default:
        // unknown operation code. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    int_operation_code->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_error_code

DESCRIPTION:
    Translate error code parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_error_code [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_error_code(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_error_code_type* int_error_code)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];
    uint8 error_code_value;

    // ensure length is 1
    if (buf[offset] != 1)
    {
        // unexpected error code length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // ensure enough room for parameter value
    if (++offset >= skip_parameter_offset)
    {
        // unexpectedly reached the end of parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    
    switch (error_code_value = buf[offset])
    {
    case SSMSG_ERROR_CODE_UNKNOWNSUBSCRIBER:
    case SSMSG_ERROR_CODE_ILLEGALSUBSCRIBER:
    case SSMSG_ERROR_CODE_BEARERSERVICENOTPROVISIONED:
    case SSMSG_ERROR_CODE_TELESERVICENOTPROVISIONED:
    case SSMSG_ERROR_CODE_ILLEGALEQUIPMENT:
    case SSMSG_ERROR_CODE_CALLBARRED:
    case SSMSG_ERROR_CODE_ILLEGALSS_OPERATION:
    case SSMSG_ERROR_CODE_SS_ERRORSTATUS:
    case SSMSG_ERROR_CODE_SS_NOTAVAILABLE:
    case SSMSG_ERROR_CODE_SS_SUBSCRIPTIONVIOLATION:
    case SSMSG_ERROR_CODE_SS_INCOMPATIBILITY:
    case SSMSG_ERROR_CODE_FACILITYNOTSUPPORTED:
    case SSMSG_ERROR_CODE_ABSENTSUBSCRIBER:
    case SSMSG_ERROR_CODE_SHORTTERMDENIAL:
    case SSMSG_ERROR_CODE_LONGTERMDENIAL:
    case SSMSG_ERROR_CODE_SYSTEMFAILURE:
    case SSMSG_ERROR_CODE_DATAMISSING:
    case SSMSG_ERROR_CODE_UNEXPECTEDDATAVALUE:
    case SSMSG_ERROR_CODE_PW_REGISTRATIONFAILURE:
    case SSMSG_ERROR_CODE_NEGATIVEPW_CHECK:
    case SSMSG_ERROR_CODE_NUMBEROFPW_ATTEMPTSVIOLATION:
    case SSMSG_ERROR_CODE_POSITIONMETHODFAILURE:
    case SSMSG_ERROR_CODE_UNKNOWNALPHABET:
    case SSMSG_ERROR_CODE_USSD_BUSY:
    case SSMSG_ERROR_CODE_REJECTEDBYUSER:
    case SSMSG_ERROR_CODE_REJECTEDBYNETWORK:
    case SSMSG_ERROR_CODE_DEFLECTIONTOSERVEDSUBSCRIBER:
    case SSMSG_ERROR_CODE_SPECIALSERVICECODE:
    case SSMSG_ERROR_CODE_INVALIDDEFLECTEDTONUMBER:
    case SSMSG_ERROR_CODE_MAXNUMBEROFMPTY_PARTICIPANTSEXCEEDED:
    case SSMSG_ERROR_CODE_RESOURCESNOTAVAILABLE:
        int_error_code->value = error_code_value;
        offset++;      // skip value octet
        break;
    default:
        // unknown error code. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    int_error_code->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_problem_code

DESCRIPTION:
    Translate problem code parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_problem_code [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    This function assumes that int_problem_code->problem_tag is already set to
    the correct problem tag.
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_problem_code(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_problem_code_type* int_problem_code)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];
    uint8 problem_code_value;

    // ensure length is 1
    if (buf[offset] != 1)
    {
        // unexpected problem code length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // ensure enough room for parameter value
    if (++offset >= skip_parameter_offset)
    {
        // unexpectedly reached the end of parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    
    switch (int_problem_code->problem_tag)
    {
    case SSMSG_PARAMETER_TAG_GENERAL_PROBLEM:
        switch (problem_code_value = buf[offset])
        {
        case SSMSG_PROBLEM_CODE_GENERAL_UNRECOGNIZED_COMPONENT:
        case SSMSG_PROBLEM_CODE_GENERAL_MISTYPED_COMPONENT:
        case SSMSG_PROBLEM_CODE_GENERAL_BADLY_STRUCTURED_COMPONENT:
            int_problem_code->value = problem_code_value;
            offset++;      // skip value octet
            break;
        default:
            // unknown problem code. skip parameter
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
        break;
    case SSMSG_PARAMETER_TAG_INVOKE_PROBLEM:
        switch (problem_code_value = buf[offset])
        {
        case SSMSG_PROBLEM_CODE_INVOKE_DUPLICATE_INVOKE_ID:
        case SSMSG_PROBLEM_CODE_INVOKE_UNRECOGNIZED_OPERATION:
        case SSMSG_PROBLEM_CODE_INVOKE_MISTYPED_PARAMETER:
        case SSMSG_PROBLEM_CODE_INVOKE_RESOURCE_LIMITATION:
        case SSMSG_PROBLEM_CODE_INVOKE_INITIATING_RELEASE:
        case SSMSG_PROBLEM_CODE_INVOKE_UNRECOGNIZED_LINKED_ID:
        case SSMSG_PROBLEM_CODE_INVOKE_LINKED_RESPONSE_UNEXPECTED:
        case SSMSG_PROBLEM_CODE_INVOKE_UNEXPECTED_LINKED_OPERATION:
            int_problem_code->value = problem_code_value;
            offset++;      // skip value octet
            break;
        default:
            // unknown problem code. skip parameter
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
        break;
    case SSMSG_PARAMETER_TAG_RETURN_RESULT_PROBLEM:
        switch (problem_code_value = buf[offset])
        {
        case SSMSG_PROBLEM_CODE_RETURN_RESULT_UNRECOGNIZED_INVOKE_ID:
        case SSMSG_PROBLEM_CODE_RETURN_RESULT_RETURN_RESULT_UNEXPECTED:
        case SSMSG_PROBLEM_CODE_RETURN_RESULT_MISTYPED_PARAMETER:
            int_problem_code->value = problem_code_value;
            offset++;      // skip value octet
            break;
        default:
            // unknown problem code. skip parameter
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
        break;
    case SSMSG_PARAMETER_TAG_RETURN_ERROR_PROBLEM:
        switch (problem_code_value = buf[offset])
        {
        case SSMSG_PROBLEM_CODE_RETURN_ERROR_UNRECOGNIZED_INVOKE_ID:
        case SSMSG_PROBLEM_CODE_RETURN_ERROR_RETURN_ERROR_UNEXPECTED:
        case SSMSG_PROBLEM_CODE_RETURN_ERROR_UNRECOGNIZED_ERROR:
        case SSMSG_PROBLEM_CODE_RETURN_ERROR_UNEXPECTED_ERROR:
        case SSMSG_PROBLEM_CODE_RETURN_ERROR_MISTYPED_PARAMETER:
            int_problem_code->value = problem_code_value;
            offset++;      // skip value octet
            break;
        default:
            // unknown problem code. skip parameter
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
        break;
    default:
        ASSERT_NOT_REACHABLE;
        break;
    }

    int_problem_code->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_forwarded_to_number

DESCRIPTION:
    Translate forwarded to number parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_forwarded_to_number [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_forwarded_to_number(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_forwarded_to_number_type* int_forwarded_to_number)
{
    uint8 offset = *poffset;
    uint8 parameter_len = buf[offset];
    uint8 skip_parameter_offset = offset+1+parameter_len;
    uint8 noa, npi;

    // check parameter length
    if (parameter_len < 1)
    {
        // parameter too short
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // check extension bit = 1
    if (get_bits(buf[offset],7,1) != 1)
    {
        // unexpected extension bit
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // nature of address
    switch(noa = get_bits(buf[offset],4,3))
    {
    case SSMSG_NOA_UNKNOWN:
    case SSMSG_NOA_INTERNATIONAL_NUMBER:
    case SSMSG_NOA_NATIONAL_SIGNIFICANT_NUMBER:
    case SSMSG_NOA_NETWORK_SPECIFIC_NUMBER:
    case SSMSG_NOA_SUBSCRIBER_NUMBER:
    case SSMSG_NOA_ABBREVIATED_NUMBER:
        int_forwarded_to_number->noa = noa;
        break;
    default:
        // reserved nature of address. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    // numbering plan indicator
    switch(npi = get_bits(buf[offset],0,4))
    {
    case SSMSG_NPI_UNKNOWN:
    case SSMSG_NPI_ISDN_TELEPHONY:
    case SSMSG_NPI_DATA:
    case SSMSG_NPI_TELEX:
    case SSMSG_NPI_LAND_MOBILE:
    case SSMSG_NPI_NATIONAL:
    case SSMSG_NPI_PRIVATE:
        int_forwarded_to_number->npi = npi;
        break;
    default:
        // reserved numbering plan indicator. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }
    // skip first octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // ensure enough room for address
    if (offset+parameter_len-2 >= skip_parameter_offset)
    {
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // check address length
    if (parameter_len-1 > SSMSG_MAX_ADDRESS_LENGTH)
    {
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // translate address from 4-bit to ascii
    int_forwarded_to_number->address_len = 
        translate_ext_digits((uint8)(parameter_len-1),  buf+offset, int_forwarded_to_number->address);
    
    int_forwarded_to_number->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_forwarded_to_subaddress

DESCRIPTION:
    Translate forwarded_to_subaddress parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_forwarded_to_subaddress [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    If subaddress field is too long, it is truncated.
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_forwarded_to_subaddress(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_forwarded_to_subaddress_type* int_forwarded_to_subaddress)
{
    uint8 offset = *poffset;
    uint8 parameter_len = buf[offset];
    uint8 skip_parameter_offset = offset+1+parameter_len;
    uint8 tos;

    // check parameter length
    if (parameter_len < 1)
    {
        // parameter too short
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // check extension bit = 1
    if (get_bits(buf[offset],7,1) != 1)
    {
        // unexpected extension bit
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // type of subaddress
    switch(tos = get_bits(buf[offset],4,3))
    {
    case SSMSG_TOS_NSAP:
    case SSMSG_TOS_USER_SPECIFIED:
        int_forwarded_to_subaddress->tos = tos;
        break;
    default:
        // reserved type of subaddress. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    // odd/even indicator
    int_forwarded_to_subaddress->odd = (get_bits(buf[offset],3,1) == 1);

    // skip first octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // ensure enough room for subaddress info
    if (offset+parameter_len-1 >= skip_parameter_offset)
    {
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // copy subaddress info as is up to the buffer size
    int_forwarded_to_subaddress->subaddress_len = 
        MIN(parameter_len-1, sizeof(int_forwarded_to_subaddress->subaddress));
    MEMCPY(int_forwarded_to_subaddress->subaddress, buf+offset,
        int_forwarded_to_subaddress->subaddress_len);
    
    int_forwarded_to_subaddress->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_forwarding_options

DESCRIPTION:
    Translate forwarding_options parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_forwarding_options [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_forwarding_options(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_forwarding_options_type* int_forwarding_options)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];
    uint8 forwarding_reason;

    // check parameter length = 1
    if (buf[offset] != 1)
    {
        // unexpected parameter length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // notification to forwarding party
    int_forwarding_options->notification_to_forwarding_party = get_bits(buf[offset],7,1);

    // redirecting presentation
    int_forwarding_options->redirecting_presentation = get_bits(buf[offset],6,1);

    // notification to calling party
    int_forwarding_options->notification_to_calling_party = get_bits(buf[offset],5,1);

    // forwarding reason
    int_forwarding_options->notification_to_calling_party = get_bits(buf[offset],5,1);

    switch (forwarding_reason = get_bits(buf[offset], 2,2))
    {
    case SSMSG_FORWARDING_REASON_MS_NOT_REACHABLE:
    case SSMSG_FORWARDING_REASON_MS_BUSY:
    case SSMSG_FORWARDING_REASON_NO_REPLY:
    case SSMSG_FORWARDING_REASON_UNCONDITIONAL:
        int_forwarding_options->forwarding_reason = forwarding_reason;
        break;
    default:
        // reserved forwarding reason. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }
    
    int_forwarding_options->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_no_reply_condition_time

DESCRIPTION:
    Translate no_reply_condition_time parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_no_reply_condition_time [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_no_reply_condition_time(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_no_reply_condition_time_type* int_no_reply_condition_time)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];

    // check parameter length = 1
    if (buf[offset] != 1)
    {
        // unexpected parameter length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // check value
    if ((buf[offset] < 5) || (buf[offset] > 30))
    {
        // unexpected parameter value
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    int_no_reply_condition_time->value = buf[offset];

    int_no_reply_condition_time->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_long_FTN

DESCRIPTION:
    Translate long_FTN parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_long_ftn [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_long_FTN(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_long_ftn_type* int_long_ftn)
{
    uint8 offset = *poffset;
    uint8 parameter_len = buf[offset];
    uint8 skip_parameter_offset = offset+1+parameter_len;
    uint8 noa, npi;

    // check parameter length
    if (parameter_len < 1)
    {
        // parameter too short
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // check extension bit = 1
    if (get_bits(buf[offset],7,1) != 1)
    {
        // unexpected extension bit
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // nature of address
    switch(noa = get_bits(buf[offset],4,3))
    {
    case SSMSG_NOA_UNKNOWN:
    case SSMSG_NOA_INTERNATIONAL_NUMBER:
    case SSMSG_NOA_NATIONAL_SIGNIFICANT_NUMBER:
    case SSMSG_NOA_NETWORK_SPECIFIC_NUMBER:
    case SSMSG_NOA_SUBSCRIBER_NUMBER:
    case SSMSG_NOA_ABBREVIATED_NUMBER:
        int_long_ftn->noa = noa;
        break;
    default:
        // reserved nature of address. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    // numbering plan indicator
    switch(npi = get_bits(buf[offset],0,4))
    {
    case SSMSG_NPI_UNKNOWN:
    case SSMSG_NPI_ISDN_TELEPHONY:
    case SSMSG_NPI_DATA:
    case SSMSG_NPI_TELEX:
    case SSMSG_NPI_LAND_MOBILE:
    case SSMSG_NPI_NATIONAL:
    case SSMSG_NPI_PRIVATE:
        int_long_ftn->npi = npi;
        break;
    default:
        // reserved numbering plan indicator. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }
    // skip first octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // ensure enough room for address
    if (offset+parameter_len-1 >= skip_parameter_offset)
    {
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // check address length
    if (parameter_len-1 > SSMSG_MAX_FTN_ADDRESS_LENGTH)
    {
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // translate address from 4-bit to ascii
    int_long_ftn->address_len = 
        translate_ext_digits((uint8)(parameter_len-1),  buf+offset, int_long_ftn->address);
    
    int_long_ftn->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_ss_status

DESCRIPTION:
    Translate ss_status parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_ss_status [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_ss_status(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_ss_status_type* int_ss_status)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];

    // ensure length is 1
    if (buf[offset] != 1)
    {
        // unexpected ss status length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // ensure enough room for parameter value
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
    
    int_ss_status->hdr.present = TRUE;
    int_ss_status->active = get_bits(buf[offset],0,1);
    int_ss_status->registered = get_bits(buf[offset],1,1);
    int_ss_status->provisioned = get_bits(buf[offset],2,1);
    int_ss_status->quiescent = get_bits(buf[offset],3,1);

    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_ss_code

DESCRIPTION:
    Translate ss_code parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_ss_code [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_ss_code(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_ss_code_type* int_ss_code)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];
    uint8 ss_code_value;

    // ensure length is 1
    if (buf[offset] != 1)
    {
        // unexpected ss code length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // ensure enough room for parameter value
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
    
    switch (ss_code_value = buf[offset])
    {
    case SSMSG_SS_CODE_ALLSS:
    case SSMSG_SS_CODE_ALLLINEIDENTIFICATIONSS:
    case SSMSG_SS_CODE_CLIP:
    case SSMSG_SS_CODE_CLIR:
    case SSMSG_SS_CODE_COLP:
    case SSMSG_SS_CODE_COLR:
    case SSMSG_SS_CODE_MCI:
    case SSMSG_SS_CODE_ALLNAMEIDENTIFICATIONSS:
    case SSMSG_SS_CODE_CNAP:
    case SSMSG_SS_CODE_ALLFORWARDINGSS:
    case SSMSG_SS_CODE_CFU:
    case SSMSG_SS_CODE_ALLCONDFORWARDINGSS:
    case SSMSG_SS_CODE_CFB:
    case SSMSG_SS_CODE_CFNRY:
    case SSMSG_SS_CODE_CFNRC:
    case SSMSG_SS_CODE_CD:
    case SSMSG_SS_CODE_ALLCALLOFFERINGSS:
    case SSMSG_SS_CODE_ECT:
    case SSMSG_SS_CODE_MAH:
    case SSMSG_SS_CODE_ALLCALLCOMPLETIONSS:
    case SSMSG_SS_CODE_CW:
    case SSMSG_SS_CODE_HOLD:
    case SSMSG_SS_CODE_CCBS_A:
    case SSMSG_SS_CODE_CCBS_B:
    case SSMSG_SS_CODE_MC:
    case SSMSG_SS_CODE_ALLMULTIPARTYSS:
    case SSMSG_SS_CODE_MULTIPTY:
    case SSMSG_SS_CODE_ALLCOMMUNITYOFINTEREST_SS:
    case SSMSG_SS_CODE_CUG:
    case SSMSG_SS_CODE_ALLCHARGINGSS:
    case SSMSG_SS_CODE_AOCI:
    case SSMSG_SS_CODE_AOCC:
    case SSMSG_SS_CODE_ALLADDITIONALINFOTRANSFERSS:
    case SSMSG_SS_CODE_UUS1:
    case SSMSG_SS_CODE_UUS2:
    case SSMSG_SS_CODE_UUS3:
    case SSMSG_SS_CODE_ALLBARRINGSS:
    case SSMSG_SS_CODE_BARRINGOFOUTGOINGCALLS:
    case SSMSG_SS_CODE_BAOC:
    case SSMSG_SS_CODE_BOIC:
    case SSMSG_SS_CODE_BOICEXHC:
    case SSMSG_SS_CODE_BARRINGOFINCOMINGCALLS:
    case SSMSG_SS_CODE_BAIC:
    case SSMSG_SS_CODE_BICROAM:
    case SSMSG_SS_CODE_ALLPLMN_SPECIFICSS:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_1:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_2:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_3:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_4:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_5:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_6:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_7:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_8:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_9:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_A:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_B:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_C:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_D:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_E:
    case SSMSG_SS_CODE_PLMN_SPECIFICSS_F:
    case SSMSG_SS_CODE_ALLCALLPRIORITYSS:
    case SSMSG_SS_CODE_EMLPP:
    case SSMSG_SS_CODE_ALLLCSPRIVACYEXCEPTION:
    case SSMSG_SS_CODE_UNIVERSAL:
    case SSMSG_SS_CODE_CALLRELATED:
    case SSMSG_SS_CODE_CALLUNRELATED:
    case SSMSG_SS_CODE_PLMNOPERATOR:
    case SSMSG_SS_CODE_ALLMOLR_SS:
    case SSMSG_SS_CODE_BASICSELFLOCATION:
    case SSMSG_SS_CODE_AUTONOMOUSSELFLOCATION:
    case SSMSG_SS_CODE_TRANSFERTOTHIRDPARTY:
        int_ss_code->value = ss_code_value;
        offset++;      // skip value octet
        break;
    default:
        // unknown error code. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    int_ss_code->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_call_is_waiting_indicator

DESCRIPTION:
    Translate call_is_waiting_indicator parameter from external to internal 
    format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_call_is_waiting_indicator [out] : placeholder for the unpacked 
                                          parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_call_is_waiting_indicator(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_call_is_waiting_indicator_type* int_call_is_waiting_indicator)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];

    // ensure length is 0
    if (buf[offset] != 0)
    {
        // unexpected call is waiting indicator length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    
    int_call_is_waiting_indicator->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_call_on_hold_indicator

DESCRIPTION:
    Translate call_on_hold_indicator parameter from external to internal 
    format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_call_on_hold_indicator [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_call_on_hold_indicator(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_call_on_hold_indicator_type* int_call_on_hold_indicator)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];
    SSMsg_hold_indicator_type hold_indicator;

    // ensure length is 1
    if (buf[offset] != 1)
    {
        // unexpected call on hold indicator length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    switch (hold_indicator = buf[offset])
    {
    case SSMSG_HOLD_INDICATOR_CALL_ON_HOLD:
    case SSMSG_HOLD_INDICATOR_CALL_RETRIEVED:
        int_call_on_hold_indicator->value = hold_indicator;
        break;
    default:
        // unknown hold indicator. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }
    
    int_call_on_hold_indicator->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_clir_suppression_rejected

DESCRIPTION:
    Translate clir_suppression_rejected parameter from external to internal 
    format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_clir_suppression_rejected [out] : placeholder for the unpacked 
                                          parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_clir_suppression_rejected(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_clir_suppression_rejected_type* int_clir_suppression_rejected)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];

    // ensure length is 0
    if (buf[offset] != 0)
    {
        // unexpected clir suppression rejected length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    
    int_clir_suppression_rejected->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_cli_restriction_option

DESCRIPTION:
    Translate cli_restriction_option parameter from external to internal 
    format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_cli_restriction_option [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_cli_restriction_option(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_cli_restriction_option_type* int_cli_restriction_option)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset = offset+1+buf[offset];
    uint8 cli_restriction_option;

    // check parameter length = 1
    if (buf[offset] != 1)
    {
        // unexpected parameter length
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    switch (cli_restriction_option = buf[offset])
    {
    case SSMSG_CLI_RESTRICTION_OPTION_PERMANENT:
    case SSMSG_CLI_RESTRICTION_OPTION_TEMPORARYDEFAULTRESTRICTED:
    case SSMSG_CLI_RESTRICTION_OPTION_TEMPORARYDEFAULTALLOWED:
        int_cli_restriction_option->value = cli_restriction_option;
        break;
    default:
        // reserved cli restriction option. skip parameter
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }
    
    int_cli_restriction_option->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_password

DESCRIPTION:
    Translate password parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_password [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE on failure.

COMMENTS:
    If password field is too long, it is truncated.
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_password(
            uint8 buf_len,
            uint8* buf,
            uint8* poffset,
            SSMsg_param_password_type* int_password)
{
    uint8 offset = *poffset;
    uint8 parameter_len = buf[offset];
    uint8 skip_parameter_offset = offset+1+parameter_len;

    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    // ensure enough room for password
    if (offset+parameter_len-1 >= skip_parameter_offset)
    {
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // check password length
    if (parameter_len > SSMSG_MAX_PASSWORD_LENGTH)
    {
        *poffset = skip_parameter_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    // copy password as is up to the buffer size
    int_password->password_len = 
        MIN(parameter_len, sizeof(int_password->password));
    MEMCPY(int_password->password, buf+offset, int_password->password_len);
    
    int_password->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_length

DESCRIPTION:
    Translate length field for sequences.

PARAMETERS:
    buf [in]        : external buffer holding the packed message.
    poffset [in]    : offset inside buf to the length field.
    max_length [in] : upper limit for length.
    length [out]    : set to the unpacked length value. In the following cases
                      length will be set to max_length:
                      1. length field is in indefinite form.
                      2. length field is in long form (with a return value of
                         SSUTIL_ERROR_INVALID_ELEMENT_LENGTH)
                      3. unpacked length value is greater than max_length (with 
                         a return value of SSUTIL_ERROR_INVALID_ELEMENT_LENGTH).

RETURN VALUE:
    SSUTIL_SUCCESS on success.
    SSUTIL_ERROR_INVALID_ELEMENT_LENGTH on failure.

COMMENTS:
    Currently, only short form and indefinite form of length field are 
    supported. Clients of this function assumes that the length of the length
    field is exactly one octet.
    poffset is not changed by this function.
=============================================================================*/
SSUtil_error_type translate_length(uint8 *buf, uint8* poffset, uint8 max_length, uint8* length)
{
    uint8 offset = *poffset;
    
    if (buf[offset] == 0x80)
    {
        // indefinite form
        *length = max_length;
        return SSUTIL_SUCCESS;
    }
    else if ((buf[offset] & 0x80) != 0)
    {
        // long form - not supported
        *length = max_length;
        return SSUTIL_ERROR_INVALID_ELEMENT_LENGTH;
    }

    *length = buf[offset];
    
    if( *length > max_length)
    {
        *length = max_length;
        return SSUTIL_ERROR_INVALID_ELEMENT_LENGTH;
    }
    
    return SSUTIL_SUCCESS;
}

//*****************************************************************************
//
// ForwardingInfo
//
//*****************************************************************************

/*=============================================================================
FUNCTION: translate_ext_parameter_forwarding_feature_list

DESCRIPTION:
    Translate forwarding_feature_list parameter from external to internal
    format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_forwarding_feature_list [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_forwarding_feature_list(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_param_forwarding_feature_list_type* int_forwarding_feature_list)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset, skip_one_item_offset;
    uint8 parameter_len, one_item_len;
    uint8 forwarding_feature_index = 0;
    SSMsg_param_forwarding_feature_type* cur_forwarding_feature_item = NULL;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &parameter_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_parameter_offset = offset+1+parameter_len;

    int_forwarding_feature_list->forwarding_feature_count = 0;

    // skip forwarding feature list length
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    while (offset < skip_parameter_offset)
    {
        switch (buf[offset])
        {
        case 0x30:  // forwarding feature tag (sequence)
            // skip forwarding feature tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if ((result = translate_length(buf, &offset, (uint8)(skip_parameter_offset-offset-1), &one_item_len)) != SSUTIL_SUCCESS)
                return result;
            else
                skip_one_item_offset = offset+1+one_item_len;
            break;
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // skip end of content length
                offset++;
                int_forwarding_feature_list->hdr.present = TRUE;
                int_forwarding_feature_list->forwarding_feature_count = forwarding_feature_index;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        default:
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }

        // skip forwarding feature length.
        SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

        if (forwarding_feature_index<SSMSG_MAX_NUM_OF_BASIC_SERVICE_GROUPS)
            cur_forwarding_feature_item = &int_forwarding_feature_list->forwarding_feature[forwarding_feature_index];
        cur_forwarding_feature_item->ss_status.hdr.present = FALSE;
        cur_forwarding_feature_item->forwarded_to_number.hdr.present = FALSE;
        cur_forwarding_feature_item->forwarded_to_subaddress.hdr.present = FALSE;
        cur_forwarding_feature_item->forwarding_options.hdr.present = FALSE;
        cur_forwarding_feature_item->no_reply_condition_time.hdr.present = FALSE;
        cur_forwarding_feature_item->long_ftn.hdr.present = FALSE;

        while (offset < skip_one_item_offset)
        {
            switch (buf[offset])
            {
            case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
                // skip end of content tag
                SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                if (buf[offset] != 0x00)
                {
                    *poffset = skip_parameter_offset;
                    return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
                }
                else
                {
                    // skip end of content length
                    offset++;
                    int_forwarding_feature_list->hdr.present = TRUE;
                    forwarding_feature_index++;
                    int_forwarding_feature_list->forwarding_feature_count = forwarding_feature_index;
                    *poffset = offset;
                    return SSUTIL_SUCCESS;
                }
                break;
            case 0x82:  // bearer service
            case 0x83:  // teleservice
                // skip it (tag+length+value)
                offset+=3;
                //SSMSG_SAFE_ADVANCE(&offset, 3, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                break;
            case 0x84:  // ss status
                SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                result = translate_ext_parameter_ss_status(buf_len, buf, &offset, &cur_forwarding_feature_item->ss_status);
                if (result != SSUTIL_SUCCESS)
                    return result;
                break;
            case 0x85:  // forwarded to number
                SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                result = translate_ext_parameter_forwarded_to_number(buf_len, buf, &offset, &cur_forwarding_feature_item->forwarded_to_number);
                if (result != SSUTIL_SUCCESS)
                    return result;
                break;
            case 0x88:  // forwarded to subaddress
                SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                result = translate_ext_parameter_forwarded_to_subaddress(buf_len, buf, &offset, &cur_forwarding_feature_item->forwarded_to_subaddress);
                if (result != SSUTIL_SUCCESS)
                    return result;
                break;
            case 0x86:  // forwarding options
                SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                result = translate_ext_parameter_forwarding_options(buf_len, buf, &offset, &cur_forwarding_feature_item->forwarding_options);
                if (result != SSUTIL_SUCCESS)
                    return result;
                break;
            case 0x87:  // no reply condition time
                SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                result = translate_ext_parameter_no_reply_condition_time(buf_len, buf, &offset, &cur_forwarding_feature_item->no_reply_condition_time);
                if (result != SSUTIL_SUCCESS)
                    return result;
                break;
            case 0x89:  // long FTN
                SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                result = translate_ext_parameter_long_FTN(buf_len, buf, &offset, &cur_forwarding_feature_item->long_ftn);
                if (result != SSUTIL_SUCCESS)
                    return result;
                break;
            default:
                // unexpected parameter tag
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
                break;
            }
        }
        forwarding_feature_index++;
    }

    int_forwarding_feature_list->hdr.present = TRUE;
    int_forwarding_feature_list->forwarding_feature_count = forwarding_feature_index;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_forwarding_info

DESCRIPTION:
    Translate forwarding_info parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_return_result [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_forwarding_info(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_component_return_result_type* int_return_result)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset;
    uint8 parameter_len;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &parameter_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_parameter_offset = offset+1+parameter_len;

    int_return_result->forwarding_info.ss_code.hdr.present = FALSE;
    int_return_result->forwarding_info.forwarding_feature_list.hdr.present = FALSE;

    // skip parameter length
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    while(offset < skip_parameter_offset)
    {
        switch(buf[offset])
        {
        case 0x04:  // ss code
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_ss_code(buf_len, buf, &offset, &int_return_result->forwarding_info.ss_code);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
    
        case 0x30:  // forwarding feature list (sequence of)
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_forwarding_feature_list(buf_len, buf, &offset, &int_return_result->forwarding_info.forwarding_feature_list);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:    // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // skip end of content length
                offset++;
                int_return_result->forwarding_info.hdr.present = TRUE;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        default:
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
    }
    
    int_return_result->forwarding_info.hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

//********************************************************************************************************************************
//
// CallBarringInfo
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: translate_ext_parameter_call_barring_feature_list

DESCRIPTION:
    Translate call_barring_feature_list parameter from external to internal
    format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_call_barring_feature_list [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_call_barring_feature_list(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_param_call_barring_feature_list_type* int_call_barring_feature_list)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset, skip_one_item_offset;
    uint8 parameter_len, one_item_len;
    uint8 call_barring_feature_index = 0;
    SSMsg_param_call_barring_feature_type* cur_call_barring_feature_item = NULL;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &parameter_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_parameter_offset = offset+1+parameter_len;

    int_call_barring_feature_list->call_barring_feature_count = 0;

    // skip call barring feature list length
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    while (offset < skip_parameter_offset)
    {
        switch(buf[offset])
        {
        case 0x30:  // call barring feature tag (sequence)
            // skip call barring feature tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if ((result = translate_length(buf, &offset, (uint8)(skip_parameter_offset-offset-1), &one_item_len)) != SSUTIL_SUCCESS)
                return result;
            else
                skip_one_item_offset = offset+1+one_item_len;
            break;
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                int_call_barring_feature_list->hdr.present = TRUE;
                int_call_barring_feature_list->call_barring_feature_count = call_barring_feature_index;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        default:
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }

        // skip call barring feature length.
        SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

        if (call_barring_feature_index<SSMSG_MAX_NUM_OF_BASIC_SERVICE_GROUPS)
            cur_call_barring_feature_item = &int_call_barring_feature_list->call_barring_feature[call_barring_feature_index];
        cur_call_barring_feature_item->ss_status.hdr.present = FALSE;

        while (offset < skip_one_item_offset)
        {
            switch(buf[offset])
            {
            case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
                // skip end of content tag
                SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                if (buf[offset] != 0x00)
                {
                    *poffset = skip_parameter_offset;
                    return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
                }
                else
                {
                    // this parameter has ended
                    offset++;
                    int_call_barring_feature_list->hdr.present = TRUE;
                    call_barring_feature_index++;
                    int_call_barring_feature_list->call_barring_feature_count = call_barring_feature_index;
                    *poffset = offset;
                    return SSUTIL_SUCCESS;
                }
                break;
            case 0x82:  // bearer service
            case 0x83:  // teleservice
                // skip it (tag+length+value)
                offset+=3;
                //SSMSG_SAFE_ADVANCE(&offset, 3, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                break;
            case 0x84:  // ss status
                // skip tag
                SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
                result = translate_ext_parameter_ss_status(buf_len, buf, &offset, &cur_call_barring_feature_item->ss_status);
                if (result != SSUTIL_SUCCESS)
                    return result;
                break;
            default:
                // unexpected parameter tag
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
                break;
            }
        }
        call_barring_feature_index++;
    }

    int_call_barring_feature_list->hdr.present = TRUE;
    int_call_barring_feature_list->call_barring_feature_count = call_barring_feature_index;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_call_barring_info

DESCRIPTION:
    Translate call_barring_info parameter from external to internal
    format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_return_result [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_call_barring_info(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_component_return_result_type* int_return_result)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset;
    uint8 parameter_len;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &parameter_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_parameter_offset = offset+1+parameter_len;

    int_return_result->call_barring_info.ss_code.hdr.present = FALSE;
    int_return_result->call_barring_info.call_barring_feature_list.hdr.present = FALSE;

    // skip parameter length
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    while (offset < skip_parameter_offset)
    {
        switch(buf[offset])
        {
        case 0x04:  // ss code
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_ss_code(buf_len, buf, &offset, &int_return_result->call_barring_info.ss_code);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
    
        case 0x30:// call barring list (sequence of)
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_call_barring_feature_list(buf_len, buf, &offset, &int_return_result->call_barring_info.call_barring_feature_list);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:    // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                int_return_result->call_barring_info.hdr.present = TRUE;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        default:
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
    }

    int_return_result->call_barring_info.hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

//********************************************************************************************************************************
//
// SS Data
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: translate_ext_parameter_ss_data

DESCRIPTION:
    Translate ss_data parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_return_result [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_ss_data(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_component_return_result_type* int_return_result)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset;
    uint8 parameter_len;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &parameter_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_parameter_offset = offset+1+parameter_len;

    int_return_result->ss_data.ss_code.hdr.present = FALSE;
    int_return_result->ss_data.cli_restriction_option.hdr.present = FALSE;
    int_return_result->ss_data.ss_status.hdr.present = FALSE;

    // skip parameter length
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    while (offset < skip_parameter_offset)
    {
        switch (buf[offset])
        {
        case 0x04:  // ss code
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_ss_code(buf_len, buf, &offset, &int_return_result->ss_data.ss_code);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case 0x84:  // ss status
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_ss_status(buf_len, buf, &offset, &int_return_result->ss_data.ss_status);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case 0x82:  // cli restriction option
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_cli_restriction_option(buf_len, buf, &offset, &int_return_result->ss_data.cli_restriction_option);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:    // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                int_return_result->ss_data.hdr.present = TRUE;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        default:
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
    }

    int_return_result->ss_data.hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_parameter_basic_service_group_list

DESCRIPTION:
    Translate basic_service_group_list parameter from external to internal
    format.
    Currently, basic_service_group_list is not applicable to GSM1x so 
    translation for it is actually checking for validity and then skipping it 
    without any further processing.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_basic_service_group_list(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset;
    uint8 parameter_len;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &parameter_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_parameter_offset = offset+1+parameter_len;

    // skip basic service group list length
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    while (offset < skip_parameter_offset)
    {
        switch(buf[offset])
        {
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        case 0x82:  // bearer service
        case 0x83:  // teleservice        
            // skip it (tag+length+value)
            offset+=3;
            //SSMSG_SAFE_ADVANCE(&offset, 3, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            break;
        default:
            // unexpected parameter tag
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
    }

    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

//********************************************************************************************************************************
//
// Generic service info
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: translate_ext_parameter_generic_service_info

DESCRIPTION:
    Translate generic_service_info parameter from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the parameter.
                       on return, set to the index of the octet following 
                       the parameter.
    int_generic_service_info [out] : placeholder for the unpacked parameter.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this parameter. This allows the translation process to continue 
    processing the message while skipping this incorrect parameter.
=============================================================================*/
SSUtil_error_type translate_ext_parameter_generic_service_info(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_param_generic_service_info_type* int_generic_service_info)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset;
    uint8 parameter_len;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &parameter_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_parameter_offset = offset+1+parameter_len;

    int_generic_service_info->ss_status.hdr.present = FALSE;
    int_generic_service_info->cli_restriction_option.hdr.present = FALSE;

    // skip parameter length
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
    
    while (offset < skip_parameter_offset)
    {
        switch(buf[offset])
        {
        case 0x04:  // ss status
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_ss_status(buf_len, buf, &offset, &int_generic_service_info->ss_status);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case 0x0a:  // cli restriction option
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_cli_restriction_option(buf_len, buf, &offset, &int_generic_service_info->cli_restriction_option);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:    // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                int_generic_service_info->hdr.present = TRUE;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        default:
            *poffset = skip_parameter_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            break;
        }
    }

    int_generic_service_info->hdr.present = TRUE;
    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

//********************************************************************************************************************************
//
// Components translation
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: translate_ext_invoke_notify_parameters

DESCRIPTION:
    Translate notify sequence of an invoke component from external to internal 
    format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the sequence.
                       on return, set to the index of the octet following 
                       the sequence.
    int_invoke [out] : placeholder for the unpacked sequence.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this sequence. This allows the translation process to continue 
    processing the message while skipping this incorrect sequence.
=============================================================================*/
SSUtil_error_type translate_ext_invoke_notify_parameters(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_component_invoke_type* int_invoke)
{
    uint8 offset = *poffset;
    uint8 skip_parameter_offset;
    uint8 parameter_len;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &parameter_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_parameter_offset = offset+1+parameter_len;

    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    while (offset < skip_parameter_offset)
    {
        switch (buf[offset])
        {
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_parameter_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        case 0x81:  // ss code
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            translate_ext_parameter_ss_code(buf_len, buf, &offset, &int_invoke->ss_code);
            break;
        case 0x8e:  // call is waiting indicator
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            translate_ext_parameter_call_is_waiting_indicator(buf_len, buf, &offset, &int_invoke->call_is_waiting_indicator);
            break;
        case 0x8f:  // call on hold indicator
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            translate_ext_parameter_call_on_hold_indicator(buf_len, buf, &offset, &int_invoke->call_on_hold_indicator);
            break;
        case 0x92:  // clir suppression rejected
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            translate_ext_parameter_clir_suppression_rejected(buf_len, buf, &offset, &int_invoke->clir_suppression_rejected);
            break;
        default:
            // unexpected or unknown parameter. for now, skip parameter
            // this needs further study. maybe distinguish between unexpected and unknown
            // parameters. unexpected params can safely be skipped while unknown params will
            // entails skipping the whole component.

            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_parameter_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            offset+=(1+buf[offset]);    // skip parameter
            break;
        }
    }

    *poffset = skip_parameter_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_component_invoke

DESCRIPTION:
    Translate an invoke component from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the component.
                       on return, set to the index of the octet following 
                       the component.
    int_invoke [out] : placeholder for the unpacked component.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this component. This allows the translation process to continue 
    processing the message while skipping this incorrect component.
=============================================================================*/
SSUtil_error_type translate_ext_component_invoke(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_component_invoke_type* int_invoke)
{
    uint8 offset = *poffset;
    uint8 skip_component_offset;
    uint8 component_len;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &component_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_component_offset = offset+1+component_len;

    int_invoke->operation_code.hdr.present = FALSE;
    int_invoke->ss_code.hdr.present = FALSE;
    int_invoke->call_is_waiting_indicator.hdr.present = FALSE;
    int_invoke->call_on_hold_indicator.hdr.present = FALSE;
    int_invoke->clir_suppression_rejected.hdr.present = FALSE;

    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    result = ext_skip_invoke_id(buf_len, buf, &offset);
    if (result != SSUTIL_SUCCESS)
    {
        *poffset = skip_component_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    while (offset < skip_component_offset)
    {
        switch (buf[offset])
        {
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_component_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                int_invoke->hdr.present = TRUE;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        case SSMSG_PARAMETER_TAG_OPERATION_CODE:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            translate_ext_parameter_operation_code(buf_len, buf, &offset, &int_invoke->operation_code);
            break;
        case 0x30:  // notifySS-Arg (sequence)
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_invoke_notify_parameters(buf_len, buf, &offset, int_invoke);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        default:
            // unexpected or unknown parameter. for now, skip parameter
            // this needs further study. maybe distinguish between unexpected and unknown
            // parameters. unexpected params can safely be skipped while unknown params will
            // entails skipping the whole component.

            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            offset+=(1+buf[offset]);    // skip parameter
            break;
        }
    }

    int_invoke->hdr.present = TRUE;
    *poffset = skip_component_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_return_result_generic_parameters

DESCRIPTION:
    Translate forwarding info, call barring info or ss data sequences of a
    return result component from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the first octet of the sequence.
                       on return, set to the index of the octet following 
                       the sequence.
    skip_component_offset [in] : offset inside buf of the octet following 
                                 the component that this sequence belongs to.
    int_return_result [out] : placeholder for the unpacked sequence.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this sequence. This allows the translation process to continue 
    processing the message while skipping this incorrect sequence.
=============================================================================*/
SSUtil_error_type translate_ext_return_result_generic_parameters(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  uint8 skip_component_offset,
                  SSMsg_component_return_result_type* int_return_result)
{
    uint8 offset = *poffset;
    SSUtil_error_type result;

    while (offset < skip_component_offset)
    {
        switch(buf[offset])
        {
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_component_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        case 0xa0:  // forwarding info
            // ensure enough room for forwarding info length 
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

            result = translate_ext_parameter_forwarding_info(buf_len, buf, &offset, int_return_result);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case 0xa1:  // call barring info
            // ensure enough room for call barring info length 
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

            result = translate_ext_parameter_call_barring_info(buf_len, buf, &offset, int_return_result);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case 0xa3:  // ss data
            // ensure enough room for ss data length 
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

            result = translate_ext_parameter_ss_data(buf_len, buf, &offset, int_return_result);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
            break;
        default:
            // unexpected or unknown parameter. for now, skip parameter
            // this needs further study. maybe distinguish between unexpected and unknown
            // parameters. unexpected params can safely be skipped while unknown params will
            // entails skipping the whole component.
            
            // ensure enough room for length octet
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            offset+=(1+buf[offset]);    // skip parameter
            break;
        }
    }

    *poffset = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_return_result_interrogate_parameters

DESCRIPTION:
    Translate interrogate parameters of a return result component from 
    external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the first octet of the parameters.
                       on return, set to the index of the octet following 
                       the parameters.
    skip_component_offset [in] : offset inside buf of the octet following 
                                 the component that these parameters belongs to.
    int_return_result [out] : placeholder for the unpacked parameters.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following these parameters. This allows the translation process to continue 
    processing the message while skipping these incorrect parameters.
=============================================================================*/
SSUtil_error_type translate_ext_return_result_interrogate_parameters(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  uint8 skip_component_offset,
                  SSMsg_component_return_result_type* int_return_result)
{
    uint8 offset = *poffset;
    SSUtil_error_type result;

    while (offset < skip_component_offset)
    {
        switch(buf[offset])
        {
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_component_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        case 0x80:  // ss status
            // ensure enough room for ss status length 
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

            result = translate_ext_parameter_ss_status(buf_len, buf, &offset, &int_return_result->ss_status);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case 0xa2:  // basic service group list
            // ensure enough room for basic service group list length 
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // skip it    
            result = translate_ext_parameter_basic_service_group_list(buf_len, buf, &offset);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case 0xa3:  // forwarding feature list
            // ensure enough room for forwarding feature list length 
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

            result = translate_ext_parameter_forwarding_feature_list(buf_len, buf, &offset, &int_return_result->forwarding_feature_list);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        case 0xa4:  // generic service info
            // ensure enough room for generic service info length 
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

            result = translate_ext_parameter_generic_service_info(buf_len, buf, &offset, &int_return_result->generic_service_info);
            if (result != SSUTIL_SUCCESS)
                return result;
            break;
        default:
            // unexpected or unknown parameter. for now, skip parameter
            // this needs further study. maybe distinguish between unexpected and unknown
            // parameters. unexpected params can safely be skipped while unknown params will
            // entails skipping the whole component.
            
            // ensure enough room for length octet
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            offset+=(1+buf[offset]);    // skip parameter
            break;
        }
    }

    *poffset = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_return_result_registerpassword_parameters

DESCRIPTION:
    Translate register password parameters of a return result component from 
    external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the first octet of the parameters.
                       on return, set to the index of the octet following 
                       the parameters.
    skip_component_offset [in] : offset inside buf of the octet following 
                                 the component that these parameters belongs to.
    int_return_result [out] : placeholder for the unpacked parameters.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following these parameters. This allows the translation process to continue 
    processing the message while skipping these incorrect parameters.
=============================================================================*/
SSUtil_error_type translate_ext_return_result_registerpassword_parameters(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  uint8 skip_component_offset,
                  SSMsg_component_return_result_type* int_return_result)
{
    uint8 offset = *poffset;
    SSUtil_error_type result;

    // new password
    if (buf[offset] != 0x12)
    {
        // unexpected tag. skip component
        *poffset = skip_component_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    // ensure enough room for new password length 
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
        
    result = translate_ext_parameter_password(buf_len, buf, &offset, &int_return_result->new_password);
    if (result != SSUTIL_SUCCESS)
        return result;

    if ((offset < skip_component_offset) && (buf[offset] == SSMSG_PARAMETER_TAG_END_OF_CONTENT))
    {
        // skip end of content tag
        SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
        if (buf[offset] != 0x00)
        {
            *poffset = offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        }
        else
        {
            // this parameter has ended
            offset++;
            *poffset = offset;
            return SSUTIL_SUCCESS;
        }
    }

    *poffset = offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_component_return_result

DESCRIPTION:
    Translate a return result component from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the component.
                       on return, set to the index of the octet following 
                       the component.
    int_return_result [out] : placeholder for the unpacked component.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this component. This allows the translation process to continue 
    processing the message while skipping this incorrect component.
=============================================================================*/
SSUtil_error_type translate_ext_component_return_result(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_component_return_result_type* int_return_result)
{
    uint8 offset = *poffset;
    uint8 skip_component_offset;
    uint8 component_len;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &component_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_component_offset = offset+1+component_len;

    int_return_result->operation_code.hdr.present = FALSE;
    int_return_result->forwarding_info.hdr.present = FALSE;
    int_return_result->call_barring_info.hdr.present = FALSE;
    int_return_result->ss_data.hdr.present = FALSE;
    int_return_result->ss_status.hdr.present = FALSE;
    int_return_result->forwarding_feature_list.hdr.present = FALSE;
    int_return_result->generic_service_info.hdr.present = FALSE;
    int_return_result->new_password.hdr.present = FALSE;

    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    result = ext_skip_invoke_id(buf_len, buf, &offset);
    if (result != SSUTIL_SUCCESS)
    {
        *poffset = skip_component_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    while (offset < skip_component_offset)
    {
        switch (buf[offset])
        {
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_component_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                int_return_result->hdr.present = TRUE;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        case 0x30:  // sequence tag (sequence)
            // skip tag+value
            SSMSG_SAFE_ADVANCE(&offset, 2, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            break;
        case SSMSG_PARAMETER_TAG_OPERATION_CODE:
            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            // unpack
            result = translate_ext_parameter_operation_code(buf_len, buf, &offset, &int_return_result->operation_code);
            if (result != SSUTIL_SUCCESS)
                return result;

            // translate Parameters
            switch (int_return_result->operation_code.value)
            {
            case SSMSG_OPERATION_CODE_REGISTERSS:
            case SSMSG_OPERATION_CODE_ERASESS:
            case SSMSG_OPERATION_CODE_ACTIVATESS:
            case SSMSG_OPERATION_CODE_DEACTIVATESS:
                translate_ext_return_result_generic_parameters(buf_len, buf, &offset, skip_component_offset, int_return_result);
                break;
            case SSMSG_OPERATION_CODE_INTERROGATESS:
                translate_ext_return_result_interrogate_parameters(buf_len, buf, &offset, skip_component_offset, int_return_result);
                break;
            case SSMSG_OPERATION_CODE_REGISTERPASSWORD:
                translate_ext_return_result_registerpassword_parameters(buf_len, buf, &offset, skip_component_offset, int_return_result);
                break;
            default:
                // unexpected or unknown operation code. skip component.
                *poffset = skip_component_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
                break;
            }
            break;
        default:
            // unexpected or unknown parameter. for now, skip parameter
            // this needs further study. maybe distinguish between unexpected and unknown
            // parameters. unexpected params can safely be skipped while unknown params will
            // entails skipping the whole component.

            // skip tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            offset+=(1+buf[offset]);    // skip parameter
            break;
        }
    }

    int_return_result->hdr.present = TRUE;
    *poffset = skip_component_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_component_return_error

DESCRIPTION:
    Translate a return error component from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the component.
                       on return, set to the index of the octet following 
                       the component.
    int_return_error [out] : placeholder for the unpacked component.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this component. This allows the translation process to continue 
    processing the message while skipping this incorrect component.
=============================================================================*/
SSUtil_error_type translate_ext_component_return_error(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_component_return_error_type* int_return_error)
{
    uint8 offset = *poffset;
    uint8 skip_component_offset;
    uint8 component_len;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &component_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_component_offset = offset+1+component_len;

    int_return_error->error_code.hdr.present = FALSE;

    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    result = ext_skip_invoke_id(buf_len, buf, &offset);
    if (result != SSUTIL_SUCCESS)
    {
        *poffset = skip_component_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }

    while (offset < skip_component_offset)
    {
        switch (buf[offset])
        {
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_component_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                int_return_error->hdr.present = TRUE;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        case SSMSG_PARAMETER_TAG_ERROR_CODE:
            // ensure enough room for parameter length 
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            translate_ext_parameter_error_code(buf_len, buf, &offset, &int_return_error->error_code);
            break;
        default:
            // unexpected or unknown parameter. for now, skip parameter
            // this needs further study. maybe distinguish between unexpected and unknown
            // parameters. unexpected params can safely be skipped while unknown params will
            // entails skipping the whole component.

            // ensure enough room for length octet
            if (++offset >= skip_component_offset)
            {
                *poffset = skip_component_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }

            offset+=(1+buf[offset]);    // skip parameter
            break;
        }
    }

    int_return_error->hdr.present = TRUE;
    *poffset = skip_component_offset;
    return SSUTIL_SUCCESS;
}

/*=============================================================================
FUNCTION: translate_ext_component_reject

DESCRIPTION:
    Translate a reject component from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the component.
                       on return, set to the index of the octet following 
                       the component.
    int_invoke [out] : placeholder for the unpacked component.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Even if translation fails, on return, poffset is set to the octet 
    following this component. This allows the translation process to continue 
    processing the message while skipping this incorrect component.
=============================================================================*/
SSUtil_error_type translate_ext_component_reject(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  SSMsg_component_reject_type* int_reject)
{
    uint8 offset = *poffset;
    uint8 skip_component_offset;
    uint8 component_len;
    uint8 problem_tag;
    SSUtil_error_type result;

    if ((result = translate_length(buf, &offset, (uint8)(buf_len-offset-1), &component_len)) != SSUTIL_SUCCESS)
        return result;
    else
        skip_component_offset = offset+1+component_len;

    int_reject->problem_code.hdr.present = FALSE;

    // skip length octet
    SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)

    result = ext_skip_invoke_id(buf_len, buf, &offset);
    if (result != SSUTIL_SUCCESS)
    {
        // in a Reject component, the invoke id can be replaced by a Universal
        // null with length=0.
        if (buf[offset] != SSMSG_PARAMETER_TAG_NULL)
        {
            *poffset = skip_component_offset;
            return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        }
        else
            // skip null parameter
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
    }

    while (offset < skip_component_offset)
    {
        switch (problem_tag = buf[offset])
        {
        case SSMSG_PARAMETER_TAG_END_OF_CONTENT:  // end of content
            // skip end of content tag
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            if (buf[offset] != 0x00)
            {
                *poffset = skip_component_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }
            else
            {
                // this parameter has ended
                offset++;
                int_reject->hdr.present = TRUE;
                *poffset = offset;
                return SSUTIL_SUCCESS;
            }
            break;
        case SSMSG_PARAMETER_TAG_GENERAL_PROBLEM:
        case SSMSG_PARAMETER_TAG_INVOKE_PROBLEM:
        case SSMSG_PARAMETER_TAG_RETURN_RESULT_PROBLEM:
        case SSMSG_PARAMETER_TAG_RETURN_ERROR_PROBLEM:
            // ensure enough room for parameter length 
            SSMSG_SAFE_ADVANCE(&offset, 1, skip_component_offset, SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE)
            int_reject->problem_code.problem_tag = problem_tag;
            translate_ext_parameter_problem_code(buf_len, buf, &offset, &int_reject->problem_code);
            break;
        default:
            // unexpected or unknown parameter. for now, skip parameter
            // this needs further study. maybe distinguish between unexpected and unknown
            // parameters. unexpected params can safely be skipped while unknown params will
            // entails skipping the whole component.

            // ensure enough room for length octet
            if (++offset >= skip_component_offset)
            {
                *poffset = skip_component_offset;
                return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
            }

            offset+=(1+buf[offset]);    // skip parameter
            break;
        }
    }

    int_reject->hdr.present = TRUE;
    *poffset = skip_component_offset;
    return SSUTIL_SUCCESS;
}

//********************************************************************************************************************************
//
// Facility ie translation
//
//********************************************************************************************************************************

/*=============================================================================
FUNCTION: translate_ext_ie_facility

DESCRIPTION:
    Translate facility IE from external to internal format.

PARAMETERS:
    buf_len [in]     : length of the external buffer.
    buf [in]         : external buffer holding the packed message.
    poffset [in,out] : offset inside buf to the length octet of the IE.
                       on return, set to the index of the octet following 
                       the IE.
    int_facility [out] : placeholder for the unpacked IE.

RETURN VALUE:
    SSUTIL_SUCCESS or one of the SSUtil error types upon failure.

COMMENTS:
    Current implementation supports only a single component in a Facility IE.
    If more than one component exist, only the first one will be processed.

    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_facility(uint8 buf_len,
                                            uint8* buf,
                                            uint8* poffset,
                                            struct SSMsg_ie_facility_type* int_facility)
{
    uint8 offset = *poffset;
    uint8 skip_ie_offset;    // offset to the octet following this ie
    SSUtil_error_type result;

    int_facility->component.gen_hdr.present = FALSE;

    skip_ie_offset = offset+1+buf[offset];

    if (offset+buf[offset] >= buf_len)
    {
        // ie exceeds message length. jump to end of message.
        *poffset = buf_len;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
    }
    
    if (++offset >= skip_ie_offset)    // skip length octet
    {
        // no components in the Facility ie
        int_facility->hdr.present = TRUE;
        *poffset = skip_ie_offset;
        return SSUTIL_SUCCESS;
    }

    // only a single component in a Facility ie is supported
    switch(buf[offset++])
    {
    case SSMSG_COMPONENT_TYPE_INVOKE:
        int_facility->component.gen_hdr.component_type = SSMSG_COMPONENT_TYPE_INVOKE;
        result = translate_ext_component_invoke(buf_len, buf, &offset, &int_facility->component.invoke);
        break;
    case SSMSG_COMPONENT_TYPE_RETURN_RESULT:
        int_facility->component.gen_hdr.component_type = SSMSG_COMPONENT_TYPE_RETURN_RESULT;
        result = translate_ext_component_return_result(buf_len, buf, &offset, &int_facility->component.return_result);
        break;
    case SSMSG_COMPONENT_TYPE_RETURN_ERROR:
        int_facility->component.gen_hdr.component_type = SSMSG_COMPONENT_TYPE_RETURN_ERROR;
        result = translate_ext_component_return_error(buf_len, buf, &offset, &int_facility->component.return_error);
        break;
    case SSMSG_COMPONENT_TYPE_REJECT:
        int_facility->component.gen_hdr.component_type = SSMSG_COMPONENT_TYPE_REJECT;
        result = translate_ext_component_reject(buf_len, buf, &offset, &int_facility->component.reject);
        break;
    default:
        // unknown component. skip ie
        *poffset = skip_ie_offset;
        return SSUTIL_ERROR_SYNTACTICALLY_INCORRECT_OPTIONAL_IE;
        break;
    }

    if (result == SSUTIL_SUCCESS)
        int_facility->hdr.present = TRUE;
    *poffset = skip_ie_offset;
    return result;
}

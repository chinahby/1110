#ifndef SSFACILITY_H
#define SSFACILITY_H

/*=========================================================================
FILE: SSFacility.h

SERVICES: FACILITY IE translation

GENERAL DESCRIPTION:
    Translation of FACILITY IE from external format (raw buffer) into
    internal format (c structs).

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSUtil.h"
#include "AEEComdef.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

struct SSMsg_ie_facility_type;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

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
    Even if translation fails, on return, poffset is set to the octet 
    following this IE. This allows the translation process to continue 
    processing the message while skipping this incorrect IE.
=============================================================================*/
SSUtil_error_type translate_ext_ie_facility(
                  uint8 buf_len,
                  uint8* buf,
                  uint8* poffset,
                  struct SSMsg_ie_facility_type* int_facility);

#endif //SSFACILITY_H

/**********************************************************************
* sdcc_debug.c
*
* SDCC(Secure Digital Card Controller) driver debug function
*
* This file implements the logging facility for the SDCC driver
*
* Copyright (c) 2004-2009 Qualcomm Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
**********************************************************************/

/*=======================================================================
Edit History
            
$Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_debug.c#2 $
$DateTime: 2009/01/07 14:51:10 $

when       who     what, where, why
---------- --------------------------------------------------------------
2009-01-05 rcc     Removed copyright encumbered printf code & fix lint errors

**********************************************************************/

#include "msg.h"
#include "AEEstd.h"

/******************************************************************************
* Name: msg_printf
*
* Description:
*    This function is used to log debug messages.
*
******************************************************************************/
void msg_printf(const char *fmt, ...)
{
    char dbg_str[100];
    AEEVaList ap = {0};

    AEEVA_START(ap, fmt);
    (void)std_vstrlprintf(dbg_str, (int)sizeof(dbg_str), fmt, ap);
    AEEVA_END(ap);

    MSG_SPRINTF_1(MSG_SSID_FS, MSG_LEGACY_ERROR, "%s", dbg_str);
}

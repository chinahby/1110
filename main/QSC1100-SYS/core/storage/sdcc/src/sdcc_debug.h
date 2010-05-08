/**********************************************************************
* sdcc_debug.h
*
* SDCC(Secure Digital Card Controller) driver debug header file
*
* Copyright (c) 2004-2009 Qualcomm Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
**********************************************************************/

/*=======================================================================
                        Edit History

$Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_debug.h#2 $
$DateTime: 2009/01/07 14:51:10 $ $Author: rcoleman $

when       who     what, where, why
----------  ---     ----------------------------------------------------------
2009-01-05  rcc     Removed unused MMC_TERMINAL_LOGGING code
2006-06-15  dng     Added SDCC_MSG_LEVEL define.
2006-06-05  dng     Change the TNOISY level from 3 to 1.
2005-08-28  hwu     General cleanup. 
2004-07-14  hwu     Added QXDM logging support.
===========================================================================*/
#ifndef __SDCC_DEBUG_H
#define __SDCC_DEBUG_H

#undef DPRINTF
#undef TPRINTF

#define SDCC_MSG_LOGGING
#define SDCC_MSG_LEVEL  1

#if defined(SDCC_MSG_LOGGING)
extern  void msg_printf(const char *fmt, ...);
#define DPRINTF(x)    msg_printf x
#define TPRINTF(level, x) 
#else
#define DPRINTF(x) 
#define TPRINTF(level, x)
#endif /* SDCC_MSG_LOGGING */
#endif /* __SDCC_DEBUG_H */

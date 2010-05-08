#ifndef _OI_TEXT_H
#define _OI_TEXT_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_text.h#1 $ 

$Log: $

===========================================================================*/
/**
 * @file  
 *
 * This file provides an API to functions for generation of text representations.
 *
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_common.h"

/**
 * Generates a text string for a BDADDR for printing out Bluetooth addresses in
 * the SDK samples and in application test programs.
 * 
 * @param addr  A pointer to a Bluetooth address.
 *
 * @return A pointer to text string in the form nn:nn:nn:nn:nn:nn.
 *
 * @note the buffer for the text string is statically allocated and should not be
 * freed. There is only one buffer so the following will not work:
 @code
   printf("addr1=%s adddr2=%s", OI_BDAddrText(&addr1), OI_BDAddrText(&addr2));
 @endcode
 */

OI_CHAR* OI_BDAddrText(const OI_BD_ADDR *addr);


/** 
 * Generate a text string for a number of bytes.
 * 
 * @param bytes  The start address for the bytes.
 * @param len  The number of bytes to output.
 * 
 * @return A pointer to a text represenation of the bytes.
 */

OI_CHAR* OI_HexText(const OI_BYTE* bytes,
                    OI_UINT16 len);


#endif /* _OI_TEXT_H */

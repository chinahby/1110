#ifndef _OI_STATUSTEXT_H
#define _OI_STATUSTEXT_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_statustext.h#1 $ 

$Log: $

===========================================================================*/
/**
 * @file  
 *
 * This file provides the API for a single debugging function, which provides the 
 * text string for OI_STATUS values.
 * This function is only defined in the debug version of the stack library.
 * 
 *
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_status.h"

/**
 * Map a status code into a text string for printing out error statuses in
 * the SDK samples and in application test programs.
 * 
 * @param value a status code.
 *
 * @return a pointer to text string.
 */

OI_CHAR* OI_StatusText(OI_STATUS value);


/*****************************************************************************/
#endif /* _OI_STATUSTEXT_H */

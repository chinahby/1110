#ifndef _ERR_HAND_H
#define _ERR_HAND_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_errhand.h#1 $ 

$Log: $

===========================================================================*/
/** @file   
 This file defines the error handler API. The underlying implementation is platform-specific.
 oi_assert.h can not include oi_errhand.h, since that would cause circular #include statements.
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_status.h"
#include "oi_assert.h"
#include "oi_osinterface.h"


/** Assert failure: This prototype exists in oi_assert.h and oi_errhand.h.
    It is replicated to ensure that the same definition is used.
    oi_assert.h can not include oi_errhand.h, since that would cause
    circular #include statements.
*/
void OI_AssertFail(char* file, int line, char* reason);

/*****************************************************************************/
#endif /* _ERR_HAND_H */

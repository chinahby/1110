#ifndef _OI_STATUS_H
#define _OI_STATUS_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_status.h#1 $ 

$Log: $
*
*    #7        26 Oct 2006           MH
* Removed status code defined under OI_CODE flag.
*
*    #6        25 Sept 2006           MH
* Added OBEX status code.
*
*    #5        23 Jul 2004            JTL
* Merged in OI 3.0.12 Rel 1 code.
* 
*    #4        23 Apr 2004            JH
* Modified to support removal of the type bt_pf_cmd_status_type
*

===========================================================================*/
/**
 * @file
 * This file contains status codes for BLUEmagic 3.0 software.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/
#ifndef OI_CODE

#include "btpfcs.h"
typedef bt_cmd_status_type OI_STATUS;
#define OI_SUCCESS(x)    ((x) == OI_OK)

#else
#error code not present
#endif /* OI_CODE */

#endif /* _OI_STATUS_H */
                    

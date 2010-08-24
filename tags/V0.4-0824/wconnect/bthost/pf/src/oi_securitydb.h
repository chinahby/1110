#ifndef _OI_SECURITYDB_H
#define _OI_SECURITYDB_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_securitydb.h#1 $ 

$Log: $

===========================================================================*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
/** 
    @file  
    This file provides access to security policies defined for services.
    This allows system designers to define security policies for services without having
    to modify code in the respective services.
        
    The security database initialization function prototype is defined in the 
    file @ref oi_bt_stack_init.h
*/

#include "oi_common.h"
#include "oi_bt_stack_config.h"
                            
/**
    Get security policy for a service. If the database does not contain
    a policy for the indicated service, the default security policy defined 
    by the Policy Manager will be applied.
*/
OI_SECURITY_POLICY  OI_SECURITYDB_GetPolicy(OI_UINT32 serviceUuid) ;

/*****************************************************************************/
#endif /* _OI_SECURITYDB_H */

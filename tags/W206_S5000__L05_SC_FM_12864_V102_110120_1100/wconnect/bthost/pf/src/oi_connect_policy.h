#ifndef _OI_CONNECT_POLICY_H
#define _OI_CONNECT_POLICY_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_connect_policy.h#1 $ 

$Log: $
 #3         28 Feb  2007           MH
* Added disconnect indication policy type.
*  
===========================================================================*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
/** 
    @file  
    
    This file defines the connection policy structure that is used to communicate application
    and service requirements.
      
*/

#include "oi_stddefs.h"
#include "oi_secmgr.h"
#include "oi_l2cap_qos.h"

/* Bit definitions for the powerSavingDisables element. */
#define OI_POLICY_DISABLE_HOLD      BIT0
#define OI_POLICY_DISABLE_SNIFF     BIT1
#define OI_POLICY_DISABLE_PARK      BIT2

typedef struct  {

    /** the UUID of the service */
    OI_UUID32           serviceUuid32 ;
    
    /** security policy for the connection **/
    OI_SECURITY_POLICY  securityPolicy ;
    
    /** indicates that the local device must be master of the ACL link */
    OI_BOOL             mustBeMaster ;
    
    /** quality of service requirements for the link: This element must point to a 
        static, constant structure, which must perist at least as long 
        as the connection exists. The pointer may be NULL, in which case
        no L2CAP quality of service requirements will be enforced. 
        Note that L2CAP guaranteed quality of service is not supported by 
        this release of BLUEmagic software, nor does any profile specification require it. */
    OI_L2CAP_FLOWSPEC   *flowspec;    
    
    /** the power-saving modes that should be disallowed on this link: This is a bit-wise OR of
        the possible power-saving mode disables. A value of 0 defines a policy where all
        all power-saving modes are enabled. */
    OI_UINT8            powerSavingDisables ; 

#ifndef OI_CODE
    /** indicates that the disconnection indication should be sent to higher layers */
    OI_BOOL             discInd ;
#endif

} OI_CONNECT_POLICY ;


/*****************************************************************************/
#endif /* _OI_CONNECT_POLICY_H */

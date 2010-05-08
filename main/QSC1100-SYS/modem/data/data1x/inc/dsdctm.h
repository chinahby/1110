#ifndef DSDCTM_H
#define DSDCTM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               D S D C T M 

GENERAL DESCRIPTION
  This file  contains extern definistions for DCTM ( Data Call Throttle 
  Manager) module. This entity posts data session success/failure events 
  to CM.

EXTERNALIZED FUNCTIONS

  dctm_post_ppp_call_status()
    Posts PPP call status to CM using the API provided by CM for this purpose.

  dctm_post_mip_call_status()
    Posts MIP call status to CM using the API provided by CM for this purpose.

  dctm_post_sip_fail()
    Posts SIP call failure status to CM using the API provided by CM for this 
    purpose.

  FEATURE_CALL_THROTTLE   - Supports Call-throttle based on previous fail 
                            reasons.

  Copyright (c) 2004-2009 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/dsdctm.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Featurized contents of this file under FEATURE_CALL_THROTTLE.
06/28/06    squ    added function dctm_post_sip_fail
07/29/05    kvd    Featurized MIP specific code.
10/08/04    kvd    Created Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_CALL_THROTTLE
#include "dsmip.h"
#include "ps_ppp_defs.h"

/*---------------------------------------------------------------------------
TYPEDEF DS_MIP_PPPP_STATUS

DESCRIPTION
  struct to report MIP , PPP fail/success to dssnet instance.
---------------------------------------------------------------------------*/
typedef struct ds_mip_ppp_status
{
  ppp_protocol_e_type     ppp_protocol;
  ppp_fail_reason_e_type  ppp_fail_reason;
#ifdef FEATURE_DS_MOBILE_IP
  mip_fail_reason_e_type  mip_fail_reason;
#endif /* FEATURE_DS_MOBILE_IP */
} ds_mip_ppp_status;

#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================

FUNCTION DCTM_POST_MIP_CALL_STATUS

DESCRIPTION
  This function  posts call fail tyep to CM based on MIP call fail/success
  event and the reason passed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dctm_post_mip_call_status
(
  mip_event_e_type        mip_event,             /* SUCCESS | FAIL         */
  mip_fail_reason_e_type  mip_fail_reason/*sol,rrq timeouts,PDSN/FA/HA fail*/
);
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================

FUNCTION DCTM_POST_PPP_CALL_STATUS

DESCRIPTION
  This function  posts call fail type to CM based on PPP call fail/success
  event and the reason passed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dctm_post_ppp_call_status
(
  ppp_event_e_type       ppp_event,       /* Down/Up                       */
  ppp_protocol_e_type    protocol,        /* LCP. IPCP                     */
  ppp_fail_reason_e_type ppp_fail_reason  /* LCP, NCP Timeout, auth_fail   */
);

/*===========================================================================

FUNCTION DCTM_POST_SIP_FAIL

DESCRIPTION
  This function  posts call fail type to CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dctm_post_sip_fail(void);

#endif /* FEATURE_CALL_THROTTLE */
#endif /* DSDCTM_H */

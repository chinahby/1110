/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               D S D C T M

GENERAL DESCRIPTION
  This module contains the coding for Data Call Throttle Manager module.
  This module interfaces with CM to report call success/failure.  

EXTERNALIZED FUNCTIONS
  These functions are defined in DSDCTM.H 

  dctm_post_ppp_call_status()
    Posts PPP call status to CM using the API provided by CM for this purpose.

  dctm_post_mip_call_status()
    Posts MIP call status to CM using the API provided by CM for this purpose.

  dctm_post_sip_fail()
    Posts SIP call failure status to CM using the API provided by CM for this 
    purpose.

  FEATURE_CAL_THROTTLE - Supports Call-throttle based on previous call-fail
                         reasons.

 Copyright (c) 2005-2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/dsdctm.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Ported support for call throttle feature (DCTM).
05/10/07    ac     Lint fix
07/21/06    as     Featured under CALL_THROTTLE and moves dctm_post_sip_fail
                   outside the DS_MOBILE_IP feature
06/28/06    squ    added function dctm_post_sip_fail
07/29/05    kvd    Featurized MIP specific code.
05/02/05    kvd    Created Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_CALL_THROTTLE
#include "cm.h"
#include "dsdctm.h"
#include "ds3gmgrint.h"    /* needed to get ds3gi_cm_client_id in cmd toCM */
#include "ds707_pkt_mgr.h" /* needed to get current data session network   */


/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
                           Pre-processor Constants
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           Enum Typedefs
---------------------------------------------------------------------------*/

/*===========================================================================
                        Variable Definitions
===========================================================================*/


/*---------------------------------------------------------------------------
  Function prototypes to allow the desired ordering of functions within this
  module - forward declarations.
---------------------------------------------------------------------------*/

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

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
  ppp_event_e_type       ppp_event,                /* Down / Up            */
  ppp_protocol_e_type    protocol,                 /* LCP, IPCP, Auth      */
  ppp_fail_reason_e_type ppp_fail_reason           /* timeout, auth_fail   */
)
{
  cm_ss_orig_thr_reasons_e_type call_status =
                           CM_SS_ORIG_THR_DS_PPP_UP_SUCCESS;
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("Posting PPP status event: %d protocol: %d reason: %d", 
          ppp_event, protocol, ppp_fail_reason);

  switch(ppp_event)
  {
    /*-----------------------------------------------------------------------
       Post PPP SUCCESS event if call is successful. This will let CM clear 
       the throttle flag if the call failed on the current SID-NID_PZID 
       earlier.
    -----------------------------------------------------------------------*/
    case PPP_UP_EV:
    case PPP_SUCCESS_EV:
      call_status = CM_SS_ORIG_THR_DS_PPP_UP_SUCCESS;
      break;

    /*-----------------------------------------------------------------------
       This event should only be posted  if PPP went down while the call is
       coming UP nto after the call is established.
    -----------------------------------------------------------------------*/

    case PPP_DOWN_EV:
    case PPP_FAILURE_EV:
    {
      switch(protocol)
      {
        case PPP_PROTOCOL_LCP:
        case PPP_PROTOCOL_IPCP:
        {
          switch(ppp_fail_reason)
          {
            case PPP_FAIL_REASON_TIMEOUT:
              if(protocol == PPP_PROTOCOL_LCP)
              {
                call_status = CM_SS_ORIG_THR_DS_PPP_LCP_TIMEOUT;
              }
              else
              {
                call_status = CM_SS_ORIG_THR_DS_PPP_NCP_TIMEOUT;
              }
              break;
            case PPP_FAIL_REASON_NONE:
              MSG_HIGH("PPP failure postd with fail_reason NONE", 0, 0, 0);
              call_status = CM_SS_ORIG_THR_DS_PPP_FAILURE;
              break;
            default:
              MSG_ERROR("Got unsupported PPP fail reason %d for event %d", 
                        ppp_fail_reason, ppp_event, 0);
              ASSERT(0);
          }
        }
        break;
        case PPP_PROTOCOL_AUTH:
          call_status = CM_SS_ORIG_THR_DS_PPP_AUTH_FAILURE;
          break;
        default:
          MSG_ERROR("Got unsupported PPP protocol reason %d for event %d", 
                    protocol, ppp_event, 0);
          ASSERT(0);
      }
      break;
    }
    default:
      MSG_ERROR("Got unsupported PPP event %d", ppp_event, 0, 0);
      ASSERT(0);
  }

  if( cm_ss_cmd_orig_thr_update_tbl(
                   NULL,
                   NULL,
                   ds3gi_cm_client_id,
                   CM_SS_ORIG_THR_UPDATE_CURR_SS,
                   call_status,
                   CM_CALL_TYPE_PS_DATA,
                   SYS_SYS_MODE_CDMA,
                   NULL,
                   (sys_band_class_e_type) 0,
                   0) == FALSE)
  {
    MSG_ERROR("Err posting ppp_call_fail to CM", 0, 0, 0);
  }
} /* dctm_post_ppp_call_status() */


#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================

FUNCTION DCTM_POST_MIP_CALL_STATUS

DESCRIPTION
  This function posts call fail type to CM based on MIP call fail/success
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
  mip_event_e_type        mip_event,              /*  SUCCESS / FAIL       */
  mip_fail_reason_e_type  mip_fail_reason/*sol,rrq timeouts,PDSN/FA/HA fail*/
)
{
  cm_ss_orig_thr_reasons_e_type call_status =
                           CM_SS_ORIG_THR_DS_MIP_UP_SUCCESS;
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED( "Posting MIP status event: %d reason %d", 
           mip_event, mip_fail_reason, 0 );

  switch(mip_event)
  {
    /*-----------------------------------------------------------------------
       Post PPP SUCCESS event if call is successful. This will let CM clear 
       the throttle flag if the call failed on the current SID-NID_PZID 
       earlier.
    -----------------------------------------------------------------------*/
    case MIP_SUCCESS_EV:
    {
      call_status = CM_SS_ORIG_THR_DS_MIP_UP_SUCCESS;
      break;
    }
    /*-----------------------------------------------------------------------
       This event should only be posted  if PPP went down while the call is
       coming UP nto after the call is established.
    -----------------------------------------------------------------------*/
    case MIP_FAILURE_EV:
    {
      switch(mip_fail_reason)
      {
        case MIP_FAIL_REASON_SOL_TIMEOUT:
          call_status = CM_SS_ORIG_THR_DS_MIP_SOL_TIMEOUT;
          break;
        case MIP_FAIL_REASON_RRQ_TIMEOUT:
          call_status = CM_SS_ORIG_THR_DS_MIP_RRQ_TIMEOUT;
          break;
        case MIP_FAIL_REASON_HA_FAILURE:
          call_status = CM_SS_ORIG_THR_DS_MIP_HA_FAILURE;
          break;
        case MIP_FAIL_REASON_FA_FAILURE:
          call_status = CM_SS_ORIG_THR_DS_MIP_FA_FAILURE;
          break;
        case MIP_FAIL_REASON_PDSN_FAILURE:
          call_status = CM_SS_ORIG_THR_DS_MIP_PDSN_FAILURE;
          break;
        case MIP_FAIL_REASON_NONE:
          MSG_HIGH("MIP failure postd with fail_reason NONE", 0, 0, 0);
          call_status = CM_SS_ORIG_THR_DS_MIP_FAILURE;
          return;
        default:
          MSG_ERROR("dssnet4 0x%x got unsupported MIP event %d", 
                    mip_event, 0, 0);
          ASSERT(0);
      }
      break;
    }
    default:
    {
      MSG_ERROR("dssnet4 0x%x got unsupported MIP event %d", mip_event, 0, 0);
      ASSERT(0);
    }
  }

  if( cm_ss_cmd_orig_thr_update_tbl(
                   NULL,
                   NULL,
                   ds3gi_cm_client_id,
                   CM_SS_ORIG_THR_UPDATE_CURR_SS,
                   call_status,
                   CM_CALL_TYPE_PS_DATA,
                   SYS_SYS_MODE_CDMA,
                   NULL,
                   (sys_band_class_e_type)0,
                   0) == FALSE)
  {
    MSG_ERROR("DCTM:Err posting ppp_call_fail to CM", 0, 0, 0);
  }
} /* dctm_post_mip_call_status() */

#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================

FUNCTION DCTM_POST_SIP_FAIL

DESCRIPTION
  This function posts call fail type to CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dctm_post_sip_fail(void)
{
  if( cm_ss_cmd_orig_thr_update_tbl(
                   NULL,
                   NULL,
                   ds3gi_cm_client_id,
                   CM_SS_ORIG_THR_UPDATE_CURR_SS,
                   CM_SS_ORIG_THR_DS_PPP_FAILURE,
                   CM_CALL_TYPE_PS_DATA,
                   SYS_SYS_MODE_CDMA,
                   NULL,
                   (sys_band_class_e_type)0,
                   0) == FALSE)
  {
    MSG_ERROR("Err posting ppp_call_sip_fail to CM", 0, 0, 0);
  }
}

#endif /* FEATURE_CALL_THROTTLE */

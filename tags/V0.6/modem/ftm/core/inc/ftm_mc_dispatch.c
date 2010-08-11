/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM MC Dispatch

GENERAL DESCRIPTION
  This is the FTM file which contains FTM MC specific commands

Copyright (c) 2000, 2001       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_mc_dispatch.c#2 $ 
  $DateTime: 2008/11/19 11:46:56 $ 
  $Author: lesp $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/08   sr      Klocwork fixes
01/10/07   aak     Featurize the FTM Hard Handoff feature (FTM_HAS_SRCH_HHO)
12/01/06   aak     Added support for FTM hard handoff feature
12/07/05   vm      Fixed the function ftm_hdr_dispatch() 
10/30/05   vm      Added new calling functions for supporting new FTM APIs
                   for DO RevA
09/20/05   jac     Added 3 new commands to the HDR dispatch to support Rev A.
10/12/04   jac     Mainlined ASYNC packet code.
05/18/04   rsr     renamed FEATURE_FTM_ASYNC_PKT to FTM_HAS_ASYNC_PKT
11/11/03   bmg     Cougar Release A merges
09/23/03   wd      Added new commands for simultaneous FCH and DCCH (Release A)
09/03/03   bmg     Added correct return packet type to MC commands
09/08/03   wd      Added Bad command response pkt for uknown commands.
09/05/03   bmg     Added FTM HDR commands
08/27/03   wd      Added support for variable length response pkt.
06/06/03   wd      Removed unnecessaries includes.
05-06-03   wd      Created.
===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "comdef.h"
#include "target.h"
#include "test_mc.h"
#include "ftm_mc_dispatch.h"
#include "ftm_cmd.h"
#include "ftm_task.h"

#include "ftm_sys_ctl.h"
#include "ftmicap.h"
/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/


/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
#ifdef FEATURE_FTM_MC

/*===========================================================================

FUNCTION FTM_MC_DISPATCH

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_mc_dispatch(ftm_mc_pkt_type  *ftm)
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_mc_cmd_resp_type ftm_mc_response_data = {0,0,0};
  ftm_mc_response_data.diag_hdr = ftm->diag_hdr;
  ftm_mc_response_data.sub_cmd = ftm->ftm_hdr.cmd_id;

  FTM_MSG_MED("FTM MC dispatching packet id: %d",ftm->ftm_hdr.cmd_id,0,0);

  ftm_rfmode_1x();

  switch (ftm->ftm_hdr.cmd_id)
  {
  case FTM_ACQUIRE_SC:
    ftm_mc_response_data.status = ftm_acquire (&ftm->param);
    break;

  case FTM_SYNC_SC:
    ftm_mc_response_data.status = ftm_sync (&ftm->param);
    break;

  case FTM_FWD_FCH_SC:
    ftm_mc_response_data.status = ftm_assign_fwd_fch (&ftm->param);
    break;

  case FTM_FWD_DCCH_SC:
    ftm_mc_response_data.status = ftm_assign_fwd_dcch (&ftm->param);
    break;

  case FTM_FWD_SCH_SC:
    ftm_mc_response_data.status = ftm_assign_fwd_sch (&ftm->param);
    break;

  case FTM_REV_FCH_SC:
    ftm_mc_response_data.status = ftm_assign_rev_fch (&ftm->param);
    break;

  case FTM_REV_DCCH_SC:
    ftm_mc_response_data.status = ftm_assign_rev_dcch (&ftm->param);
    break;

  case FTM_REV_SCH_SC:
    ftm_mc_response_data.status = ftm_assign_rev_sch (&ftm->param);
    break;

  case FTM_FCH_LOOP_SC:
    ftm_mc_response_data.status = ftm_loopback_fch (&ftm->param);
    break;

  case FTM_DCCH_LOOP_SC:
    ftm_mc_response_data.status = ftm_loopback_dcch (&ftm->param);
    break;

  case FTM_SCH_LOOP_SC:
    ftm_mc_response_data.status = ftm_loopback_sch (&ftm->param);
    break;

  case FTM_RELEASE_SC:
    ftm_mc_response_data.status = ftm_deassign_traffic (&ftm->param);
    break;

#ifdef FTM_HAS_SRCH_HHO
  case FTM_FWD_HHO_SC:
    ftm_mc_response_data.status = ftm_fwd_hho (&ftm->param);
    break;
#endif

#ifdef FEATURE_IS2000_P2  
  case FTM_FWD_FCH_DCCH_SC:
    ftm_assign_fwd_fch_dcch (&ftm->param);
    break;

  case FTM_REV_FCH_DCCH_SC:
    ftm_assign_rev_fch_dcch (&ftm->param);
    break;
#endif /* #ifdef FEATURE_IS2000_P2 */
  
  default:
    ftm_mc_response_data.status = TEST_INVALID_CMD;
    ERR("Unknown FTM MC Command",0,0,0);
    break;
  }

  switch (ftm_mc_response_data.status)
  {
  case TEST_SUCCESS:
  case TEST_INVALID_STATE:
  case TEST_NO_SUCCESS:
    rsp_pkt = ftmdiag_create_new_pkt_with_data((byte*)&ftm_mc_response_data, sizeof(ftm_mc_response_data),
                                               (byte*)NULL, 0);
    break;

  case TEST_INVALID_CMD:
  default:
    rsp_pkt.cmd = FTM_RSP_BAD_CMD;
  }
  
  return rsp_pkt;
}   /* end ftm_mc_dispatch */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
#endif /* FEATURE_FTM_MC */
#endif /* FEATURE_FACTORY_TESTMODE */

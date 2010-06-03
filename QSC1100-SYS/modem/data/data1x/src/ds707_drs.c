/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                           D S 7 0 7 _ D R S

GENERAL DESCRIPTION
  This file contains functions that are used to see if the DRS bit should be
  set on originations out of dormancy.
  
EXTERNALIZED FUNCTIONS
  ds707_drs_get_mip_drs_bit()
    Called on a pkt re-origination to see if Mobile IP requires the DRS bit
    to be set, indicating that the mobile has data to send.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

 Copyright (c) 2002-2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_drs.c_v   1.5   02 Dec 2002 19:40:10   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_drs.c#3 $ $DateTime: 2009/05/27 05:07:18 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/29/09   sn         Ported support for call throttle feature (DCTM).
07/28/08   psng       To support RUIM refresh feature, the caching of
                      NV_DS_MIP_QC_DRS_OPT_I is removed
11/01/06   an         Featurize ATCOP, RMSM, Async Fax
08/22/03   jd         Go to resync meta state when forcing PPP resync so that 
                      Um/Rm options are checked afterwards (for laptop call) 
05/05/03   jd         Replaced ds707_drsi_is_mip_up_on_um() with 
                      mip_reg_sm_session_active()
12/02/02   ak         Fixed up compiler warnings.
11/25/02   usb        Added functionality to initiate PPP resync when drs 
                      bit is set.
09/19/02   jd         Only apply DRS opt. if turned on and in MIP call
09/13/02   jd         Added DRS optimization for mobile IP (always on)
07/29/02   ak         Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
                         
#include "msg.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ps_ppp_netmodel.h"
#include "ds707_rmsm.h"
#endif
#ifdef FEATURE_DS_MOBILE_IP
#include "nv.h"
#include "dstaski.h"
#include "dsmip_pdsn_db.h"
#include "ps_ppp.h"
#include "dsmip_regsm.h"
#endif /* FEATURE_DS_MOBILE_IP */

#include "ps707_dssnet.h"


/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/

#ifdef FEATURE_DS_MOBILE_IP
LOCAL nv_item_type drs_opt_nv_item;
#endif /* FEATURE_DS_MOBILE_IP */

LOCAL boolean dsi_use_drs_algorithm = FALSE;


/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_DRS_GET_MIP_DRS_BIT

DESCRIPTION   This function returns TRUE if the DRS bit should be set.  Else
              returns FALSE if the DRS bit should not be set.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_get_mip_drs_bit(void)
{
  /*-------------------------------------------------------------------------
    If this function is going to be used for something OTHER than MIP it will
    likely be necessagy to modify the ds707_drs_handle_snpzid_change_recon()
    function to handle the change!
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DS_MOBILE_IP
  pdsn_db_kind_enum_type curr_pdsn_kind;  /* Have we seen this PDSN */
  nv_stat_enum_type      nv_status;       /* status from NV call  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Retrieve the user preference for use of the zombie PPP avoidance (DRS)
    algorithm.  Default is FALSE (don't use) if not specified.
  -------------------------------------------------------------------------*/
  nv_status = dsi_get_nv_item( NV_DS_MIP_QC_DRS_OPT_I, &drs_opt_nv_item);
  if( nv_status == NV_DONE_S)
  {
    dsi_use_drs_algorithm = drs_opt_nv_item.ds_mip_qc_drs_opt;
  }
  else
  {
    MSG_ERROR("Failed to read MIP_QC_DRS_OPT, DRS off!", 0, 0, 0);
  }

 /*-------------------------------------------------------------------------
    For Mobile IP calls, if handoff optimization algorithm is enabled,
    see if the MS needs to set DRS to 1 to get around zombie PPP
    instances in the network.
  -------------------------------------------------------------------------*/
    if (dsi_use_drs_algorithm && mip_reg_sm_session_active(NULL))
  {
    /*-----------------------------------------------------------------
      If we have been to this PDSN before then there is a chance that
      there is a zombie PPP instance - so set DRS to 1 for MIP calls.
      Also, if this is a new PCF, the mobile doesn't know which PDSN
      this PCF may be connected to - so set DRS conservatively to 1.
      Note, if the carrier has disabled this optimization or the MS is
      in a prev less than 6, then pdsn_db_get will return DISABLED - so
      regular DRS algorithm will be used.
    -----------------------------------------------------------------*/
    curr_pdsn_kind = pdsn_db_get_kind();
    if ((curr_pdsn_kind == VISITED_PDSN) || (curr_pdsn_kind == UNKNOWN_PDSN))
    {
      MSG_MED ("QCODH indicates zombie PPP possible",0,0,0);
      return (TRUE);
    }
  }
#endif /* FEATURE_DS_MOBILE_IP */

  return(FALSE);
} /* ds707_drs_get_mip_drs_bit() */


/*===========================================================================
FUNCTION      DS707_DRS_HANDLE_SNPZID_CHANGE_RECON

DESCRIPTION   This function handles the reconection due to SID/NID/PZID 
              change. If the drs bit was set during reconnection then it 
              initiates a PPP resync on UM interface.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_drs_handle_snpzid_change_recon(void)
{
#ifdef FEATURE_DS_MOBILE_IP
  /*---------------------------------------------------------------------
    QCODH Algorithm - If we don't know that we are performing an 
    inter-PCF handoff, resync the Um and re-register Mobile IP.
  ---------------------------------------------------------------------*/
  if (ds707_drs_get_mip_drs_bit())
  {
    if(ppp_resync(PPP_UM_SN_DEV, PPP_PROTOCOL_LCP, NULL) < 0)
    {
      MSG_ERROR("Failed to resync PPP on SNPZID change", 0, 0, 0);
    }
    else
    {
      /*---------------------------------------------------------------------
        If this is a laptop call, make sure we check Um/Rm framing options
        match after resync is complete.
      ---------------------------------------------------------------------*/
      #ifndef FEATURE_DATA_STRIP_ATCOP
      if(ds707_rmsm_is_packet_call())
      {
        netmdl_set_meta_state(PPP_RESYNC_META_STATE);
      }
      #endif
    }
  }
#endif /* FEATURE_DS_MOBILE_IP */
} /* ds707_drs_handle_snpzid_change_recon() */

/*===========================================================================
FUNCTION      DS707_DRS_IS_IN_MIP_CALL

DESCRIPTION   Wrapper function that returns TRUE if we are in a mobile IP
              call.  This is provided because all the checks in this file
              address zombie PPP which is only a problem for Mobile IP.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_is_in_mip_call(void)
{
#ifdef FEATURE_DS_MOBILE_IP
  // future - decouple from mip client (MIP status active bit in ps_iface)
  if(mip_reg_sm_session_active(NULL))
  {
    return TRUE;
  }
#endif /* FEATURE_DS_MOBILE_IP */
  return FALSE;
} /* ds707_drs_is_in_mip_call() */

#endif /* FEATURE_DATA_IS707 */

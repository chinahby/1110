/*===========================================================================

                         D S M I P . C

DESCRIPTION

 The Data Services Mobile IP configuration source file.

EXTERNALIZED FUNCTIONS

   mip_config_session()
     Load MIP session defaults from NV to all sessions.
   mip_reg_ses_info_cb()
     Register a callback to fill in MIP session information
   mip_reg_pwd_info_cb()
     Register a callback to fill in passwords for MIP session
   mip_get_rm_nai()
     Retrieves the NAIs for a given MIP profile for the
     tethered device and stores them in the mip session info.
  

Copyright (c) 2000-2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip.c_v   1.5   19 Feb 2003 15:54:24   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip.c#6 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
04/29/09    sn     Merged MIP event registration support and Ported support 
                   for call throttle feature (DCTM).
04/16/09    sn     Merged support for separate NAI for Tethered SIP and MIP 
                   calls. Featurized under FEATURE_DS_MULTIPLE_NAI.
04/08/08    sn     Fixed klockwork error.
09/21/07 sq/mga/yz Merged and made changes to support FEATURE_UIM_SUPPORT_3GPD.
11/10/06    an     Fixed compilation errors for 6020
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
06/19/03    jd     Make sure primary HA holds valid IP address in all cases
06/04/03    jd     Check if both HAs are dynamic even when using shadow NV.
06/03/03    jd     Comment and formatting cleanup.
05/29/03    jd     If profile enable flag can't be read, assume disabled to 
                   allow shadow NV settings.
10/09/02    jay    Made sure both Pri and Sec HA are not Dynamic
09/17/02    jd     Changed hard coded dynamic HA address to DYNAMIC_HA_ADDR
07/19/02    jay    Used a temp variable to copy nai and keep the
                   shadow_nv.nai free.
04/29/02    jd     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "dsmip.h"
#include "dsmip_metasm.h"
#include "dsmip_regsm.h"

#ifdef FEATURE_DATA_MM
#include "dsat707mip.h"
#include "ps_utils.h"
#include "ps_mip_compat.h"
#else
#include "psi.h"
#include "dsatcop.h"
#endif /* FEATURE_DATA_MM */

#include "nv.h"
#include "assert.h"

#ifdef FEATURE_UIM_SUPPORT_3GPD
#include "nvruimi.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */


/*---------------------------------------------------------------------------
  Make sure that the definition of NAI length is reasonable - does this work
  with acc?
---------------------------------------------------------------------------*/
#if MAX_NAI_LEN < NV_MAX_NAI_LENGTH
#error "MAX_NAI_LEN is too small!  Must be at least the same as NV_MAX_NAI_LEN"
#endif


/*===========================================================================

                             EXTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  Mobile IP session info array
---------------------------------------------------------------------------*/
mip_session_info_type  mip_ses_info_array[MIP_MAX_SESSIONS];

mip_event_info_s_type  mip_event_buf[MIP_EVENT_CLIENT_MAX];

/*---------------------------------------------------------------------------
   Foreign Agent currently serving the mobile 
---------------------------------------------------------------------------*/
mip_ma_info_type  ma_info;

/*---------------------------------------------------------------------------
  Holds password info, passed to mip_get_pwd_cb. 
---------------------------------------------------------------------------*/
mip_pwd_info_type  mip_pwd_info = { NULL, NULL, 0, 0 };

/*---------------------------------------------------------------------------
  Callback to retrieve session information - if non-null, this callback
  will be called after mipi_get_nv_info to update the session info block
  for the given session (first parameter).
---------------------------------------------------------------------------*/
void (* mip_get_session_info_cb)(
                                  byte profile_num, 
                                  mip_session_info_type * session
                                ) = NULL;

/*---------------------------------------------------------------------------
  Callback to retrieve shared secret information - if non-null,
  this callback will be called to retrieve the user password info for
  the given session (first parameter).  Callback should populate
  referenced mip_pwd_info_type structure.

  Otherwise, NAI and shared secrets will be retrieved from NV.
---------------------------------------------------------------------------*/
void (* mip_get_pwd_cb)( 
                         byte profile_num, 
                         mip_pwd_info_type * pwd_info
                       ) = NULL;


/*===========================================================================

                        INTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIPI_GET_NV_INFO()

DESCRIPTION
  This function retrieves the active Mobile IP profile configuration and
  stores it in the Mobile IP session info block passed by the caller.  

  Note: The shared secret information is read, but not stored
        in the control block.

PARAMETERS
  session: pointer to the mip session control block where info is stored

RETURN VALUE
  -1 if session information retrieved from NV is insufficient to make a call
  otherwise, the active profile number

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL sint15 mipi_get_nv_info
(
  mip_session_info_type *  session
)
{
  nv_stat_enum_type nv_status;
  int               loop; 
  char *            nai_temp;
  uint16            multiplier;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Default values for missing NV items
  -------------------------------------------------------------------------*/
#define MIP_NV_DEFAULT_RRQ_RETRIES    2
#define MIP_NV_DEFAULT_RETRY_INT      0
#define MIP_NV_DEFAULT_PRE_RRQ_TIME   0
#ifdef FEATURE_DS_MOBILE_IP_DEREG
#define MIP_NV_DEFAULT_DERRQ_RETRIES  0
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check to be sure the a valid session control block was specified
  -------------------------------------------------------------------------*/
  if (session == NULL)
  {
    ASSERT(0);
    return (-1);
  }
  session->valid = FALSE;

  /*-------------------------------------------------------------------------
    Retrieve currently selected, or "active", profile from NV.  
    Return from function if there is no active profile.
  -------------------------------------------------------------------------*/
  nv_status = psi_get_nv_item(NV_DS_MIP_ACTIVE_PROF_I, &ps_nv_item);
  if( nv_status != NV_DONE_S )
  {
    MSG_ERROR("Active MIP prof cannot be retrieved",0,0,0);
    return (-1);
  }

  /*-------------------------------------------------------------------------
    Set default values for session info
  -------------------------------------------------------------------------*/
  session->curr_ha_index = PRIMARY_HA_INDEX;

  /*-------------------------------------------------------------------------
    Store profile index in MIP session control block.
  -------------------------------------------------------------------------*/
  session->profile_index = ps_nv_item.ds_mip_active_prof;

  /*-------------------------------------------------------------------------
    Retrieve Mobile IP max registration retries from NV - set max RRQs
    variable as this plus one (total attempts is retries + 1.)
  -------------------------------------------------------------------------*/
  nv_status = psi_get_nv_item( NV_DS_MIP_RETRIES_I, &ps_nv_item);
  if( nv_status != NV_DONE_S )
  {
    MSG_HIGH("MIP max retry unavail:default %d",
             MIP_NV_DEFAULT_RRQ_RETRIES, 0, 0);
    session->rsm_info.max_rrqs = MIP_NV_DEFAULT_RRQ_RETRIES + 1;
  }
  else
  {
    session->rsm_info.max_rrqs = ps_nv_item.ds_mip_retries + 1;
  }

#ifdef FEATURE_DS_MOBILE_IP_DEREG
  /*-------------------------------------------------------------------------
    Retrieve Mobile IP max deregistration retries from NV - set max deRRQs
    variable as this plus one (total attempts is retries + 1.)
  -------------------------------------------------------------------------*/
  nv_status = psi_get_nv_item( NV_DS_MIP_DEREG_RETRIES_I, &ps_nv_item);
  if( nv_status != NV_DONE_S )
  {
    MSG_HIGH( "MIP max retry unavail:default %d", 
              MIP_NV_DEFAULT_DERRQ_RETRIES,0,0 );
    session->rsm_info.max_derrqs = MIP_NV_DEFAULT_DERRQ_RETRIES + 1;
  }
  else
  {
    session->rsm_info.max_derrqs = ps_nv_item.ds_mip_retries + 1;
  }
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

  /*-------------------------------------------------------------------------
    Retrieve Mobile IP retry interval from NV.
  -------------------------------------------------------------------------*/
  nv_status = psi_get_nv_item( NV_DS_MIP_RETRY_INT_I, &ps_nv_item);
  if( nv_status != NV_DONE_S )
  {
    MSG_HIGH("MIP retry int unavail:default %d",
             MIP_NV_DEFAULT_RETRY_INT, 0, 0);
    ps_nv_item.ds_mip_retry_int = MIP_NV_DEFAULT_RETRY_INT;
  }

  /*-------------------------------------------------------------------------
    The retry interval is a number between 0 and 4 which specified the
    retry interval for RRQs, the corresponding time: N * 250 + 1000ms
  -------------------------------------------------------------------------*/
  session->base_retry_interval = 250 * ps_nv_item.ds_mip_retry_int + 1000;
  session->rsm_info.re_rrq_timeout = session->base_retry_interval;

  /*-------------------------------------------------------------------------
    Calculate the max RRQ time (the time we spend trying to register).

    NOTE: this calculation includes enough time for an extra RRQ 
    (i.e. we start with the first attempt, and we add time for max RRQ
    more attempts).  This allows a cushion for re-registration.
  -------------------------------------------------------------------------*/
  session->max_reg_time = session->base_retry_interval;
  multiplier = MIP_RRQ_RETRY_BACKOFF_MULT;
  for(loop = 0; loop < session->rsm_info.max_rrqs; loop += 1)
  {
    session->max_reg_time += (uint16) (session->base_retry_interval * 
                                       multiplier);
    multiplier *=  MIP_RRQ_RETRY_BACKOFF_MULT;
  }

  /*-------------------------------------------------------------------------
    Retrieve Mobile IP pre-registration retry interval from NV.
  -------------------------------------------------------------------------*/
  nv_status = psi_get_nv_item( NV_DS_MIP_PRE_RE_RRQ_TIME_I, &ps_nv_item);
  if( nv_status != NV_DONE_S )
  {
    MSG_ERROR("MIP prereg ret int unavail:default %d",
              MIP_NV_DEFAULT_PRE_RRQ_TIME, 0, 0);
    ps_nv_item.ds_mip_pre_re_rrq_time = MIP_NV_DEFAULT_PRE_RRQ_TIME;
  }

  /*-------------------------------------------------------------------------
    This value is in minutes: convert to ms - there are 60000 ms/minute
  -------------------------------------------------------------------------*/
  session->rsm_info.pre_re_rrq_time = 
    ps_nv_item.ds_mip_pre_re_rrq_time * 60000;

  /*-------------------------------------------------------------------------
    Retrieve the user preference for sending RRQ only if there is some
    traffic since previous registration. We don't consider it a serious
    error if the NV does not have this value. In such a case, we simply
    default it to false.
  -------------------------------------------------------------------------*/
  nv_status = psi_get_nv_item( NV_DS_MIP_RRQ_IF_TFRK_I, &ps_nv_item);
  if( nv_status != NV_DONE_S )
  {
    MSG_HIGH("MIP RRQ if traffic unavailable:default", 0, 0, 0);
    session->rrq_if_tfrk = FALSE;
  }
  else
  {
    session->rrq_if_tfrk = ps_nv_item.ds_mip_rrq_if_tfrk;
  }

  /*-------------------------------------------------------------------------
    Check NV to see if active profile is enabled or disabled
  -------------------------------------------------------------------------*/
  nv_status = psi_get_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ps_nv_item);
  if( nv_status != NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      If the NV item was never written, assume disabled in case shadow NV
      settings have been entered.
    -----------------------------------------------------------------------*/
    ERR("MIP enable profile flag array couldn't be retrieved!", 0, 0, 0);
    MSG_MED ("Assuming profile %d is disabled",session->profile_index,0,0);
    ps_nv_item.ds_mip_enable_prof[session->profile_index] = 0;
  }
  
#ifdef FEATURE_UIM_SUPPORT_3GPD
  if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
     UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
    {
    /*-----------------------------------------------------------------------
      If not all of the items are available in NV, then all must
      be retrived from the card
    -----------------------------------------------------------------------*/
      if( shadow_nv.nai == NULL         ||
          shadow_nv.mn_aaa_ss == NULL   ||
          shadow_nv.mn_ha_ss == NULL    ||
          shadow_nv.mn_aaa_spi == NULL  ||
          shadow_nv.mn_ha_spi == NULL   ||
          shadow_nv.ha_ip == NULL       ||
          shadow_nv.pri_ha_ip == NULL   ||
          shadow_nv.sec_ha_ip == NULL )
      {
        dsat707_erase_shadow_nv();
      }
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  /*-------------------------------------------------------------------------
    If the profile is enabled, load profile settings to session info block
    Use, in order of preference:
    - shadow NV settings (set via "AT$QCMIPxxx=...,0" commands)
    - NV settings
  -------------------------------------------------------------------------*/
  if (ps_nv_item.ds_mip_enable_prof[session->profile_index])
  {
    /*-----------------------------------------------------------------------
      Retrieve Mobile IP general user profile from NV.
    -----------------------------------------------------------------------*/
    ps_nv_item.ds_mip_gen_user_prof.index = session->profile_index;
    nv_status = psi_get_nv_item( NV_DS_MIP_GEN_USER_PROF_I, &ps_nv_item);
    if( nv_status != NV_DONE_S )
    {
      MSG_ERROR("MIP general user profile cannot be retrieved!", 0, 0, 0);
      return (-1);
    }
  
    /*-----------------------------------------------------------------------
      Store Mobile IP general user profile items into M.IP session info 
      struct. Only copy the secondary HA address if we have memory for it.
    -----------------------------------------------------------------------*/
    if (shadow_nv.ha_ip != NULL)
    {
      MSG_MED("Using shadow NV item for Home Add",0,0,0);
      session->home_addr = ntohl(* shadow_nv.ha_ip);
    }
    else
    {
      session->home_addr = ntohl(ps_nv_item.ds_mip_gen_user_prof.home_addr);
    }
    
    /*-----------------------------------------------------------------------
      Use shadow NV settings if set
    -----------------------------------------------------------------------*/
    if (shadow_nv.pri_ha_ip != NULL)
    {
      MSG_MED("Using shadow NV item for Pri HA",0,0,0);
      session->ha_addrs[PRIMARY_HA_INDEX] = ntohl(* shadow_nv.pri_ha_ip);
    }
    else
    {
      session->ha_addrs[PRIMARY_HA_INDEX] = 
        ntohl(ps_nv_item.ds_mip_gen_user_prof.primary_ha_addr);
    }

#if MAX_HA_ADDRS > 1
    if(shadow_nv.sec_ha_ip != NULL)
    {
      MSG_MED("Using shadow NV item for Sec HA",0,0,0);
      session->ha_addrs[SECONDARY_HA_INDEX] = ntohl(* shadow_nv.sec_ha_ip);
    }
    else
    {
      session->ha_addrs[SECONDARY_HA_INDEX] =
        ntohl(ps_nv_item.ds_mip_gen_user_prof.secondary_ha_addr);
    }
#endif /* MAX_HA_ADDRS > 1 */ 

    /*-----------------------------------------------------------------------
      Check whether to retrieve the NAI  - real or soft NV - If there is no 
      shadow NV, then use the value retrieved (use shadow NV pointer value 
      so only memcpy() is needed)
    -----------------------------------------------------------------------*/
    if(shadow_nv.nai != NULL)
    {
      MSG_MED("Using shadow NV item for NAI",0,0,0);
      session->nai_len = strlen(shadow_nv.nai);
      nai_temp = shadow_nv.nai;
    }
    else
    {
      nai_temp = (char*)ps_nv_item.ds_mip_gen_user_prof.nai;
      session->nai_len = ps_nv_item.ds_mip_gen_user_prof.nai_length;
    }
      
    /*-----------------------------------------------------------------------
      Only copy NAI if its length is valid.  If NAI length is > max allowed
      assume a memory/flash scribble and set length to 0.
    -----------------------------------------------------------------------*/
    if(session->nai_len > NV_MAX_NAI_LENGTH)
    {
      session->nai_len = 0;
    }
    else if(session->nai_len > 0)
    {
      memcpy(session->nai, nai_temp, session->nai_len);
      MSG_LOW(" session->nai_len is %d",session->nai_len,0,0);
    }
 
    /*-----------------------------------------------------------------------
      Set the MN-AAA and MN-HA SPI values - real or soft NV
    -----------------------------------------------------------------------*/
    if (shadow_nv.mn_aaa_spi != NULL) 
    {
      MSG_MED("Using shadow NV item for AAA-SPI",0,0,0);
      session->mn_aaa_spi = *shadow_nv.mn_aaa_spi;

     }
    else
    {
#ifdef FEATURE_UIM_SUPPORT_3GPD
      if(!ps_nv_item.ds_mip_gen_user_prof.mn_aaa_spi_set &&
         (UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
         UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
      {
        MSG_HIGH("MN-AAA SPI is not provisioned on the card.  USING CHAP",0,0,0);
        session->mn_aaa_spi = MIP_CHAP_SPI;
      }
      else
#endif /* FEATURE_UIM_SUPPORT_3GPD */
      {
        session->mn_aaa_spi = ps_nv_item.ds_mip_gen_user_prof.mn_aaa_spi; 
      }
    }
    if (shadow_nv.mn_ha_spi != NULL) 
    {
      MSG_MED("Using shadow NV item for HA-SPI",0,0,0);
      session->mn_ha_spi = *shadow_nv.mn_ha_spi;
    }
    else
    {

#ifdef FEATURE_UIM_SUPPORT_3GPD
      if(!ps_nv_item.ds_mip_gen_user_prof.mn_ha_spi_set &&
         (UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
         UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
      {
        MSG_HIGH("MN-HA SPI is not provisioned on the card.  USING MD5",0,0,0);
        session->mn_ha_spi = MIP_MD5_SPI;
      }
      else
#endif /* FEATURE_UIM_SUPPORT_3GPD */

      {
        session->mn_ha_spi = ps_nv_item.ds_mip_gen_user_prof.mn_ha_spi; 
      }
    }
  
    /*-----------------------------------------------------------------------
      Set the Rev Tunneling preference - real or soft NV
    -----------------------------------------------------------------------*/
    if (shadow_nv.rev_tun != NULL)
    {
      MSG_MED("Using shadow NV item for Rev Tun",0,0,0);
      session->mn_request_rev_tun = *shadow_nv.rev_tun;
    }
    else
    {
      session->mn_request_rev_tun = 
        ps_nv_item.ds_mip_gen_user_prof.rev_tun_pref;
    }
       
    /*-----------------------------------------------------------------------
      If Mobile IP shared secret user profile has not been written to NV, 
      log an error - except when using the RUIM for MIP
    -----------------------------------------------------------------------*/

#ifdef FEATURE_UIM_SUPPORT_3GPD

   /*------------------------------------------------------------------------
      If FALLBACK for 3GPD R-UIM is disabled and the card doesn't support MIP
      uim_3gpd_control will return UIM_3GPD_MIP_RUIM_SIP_RUIM.  However, the 
      NV read for GEN_USER_PROF above will fail, and will cause this function
      to fail above
      ---------------------------------------------------------------------*/
    if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
       UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
#endif /* FEATURE_UIM_SUPPORT_3GPD */

    {
      ps_nv_item.ds_mip_ss_user_prof.index = session->profile_index;
      nv_status = psi_get_nv_item( NV_DS_MIP_SS_USER_PROF_I, &ps_nv_item);
      if( nv_status != NV_DONE_S )
      {
        MSG_ERROR("MIP shared secret profile cannot be retrieved!", 0, 0, 0);
        return (-1);
      }
    }
  }
  else /* the profile is disabled */
  {
    /*-----------------------------------------------------------------------
      If the profile is disabled, use (in order of preference):
      - shadow NV settings (set via "AT$QCMIPxxx=...,0" commands)
      - session info defaults
    -----------------------------------------------------------------------*/
    MSG_MED ("Profile %d is disabled",session->profile_index,0,0);

    /*-----------------------------------------------------------------------
      Load NAI
      Use shadow NV value if defined, else set to default.
    -----------------------------------------------------------------------*/
    if (shadow_nv.nai != NULL)
    {
      MSG_MED("Using shadow NV item for NAI",0,0,0);
      session->nai_len = strlen(shadow_nv.nai);
      if(session->nai_len > NV_MAX_NAI_LENGTH)
      {
       session->nai_len = 0;
      }
      else if(session->nai_len > 0)
      memcpy(session->nai, shadow_nv.nai, session->nai_len);
    }
    else
    {
      session->nai_len = 0;
    }

    /*-----------------------------------------------------------------------
      Load Mobile's Home IP address
      Use shadow NV value if defined, else set to default.
    -----------------------------------------------------------------------*/
    if (shadow_nv.ha_ip != NULL)
    {
      MSG_MED("Using shadow NV item for Home Add",0,0,0);
      session->home_addr = ntohl(* shadow_nv.ha_ip);
    }
    else
    {
      session->home_addr = 0;
    }

    /*-----------------------------------------------------------------------
      Load Primary and Secondary Home Agent IP addresses 
      Use shadow NV value if defined, else set to default.
    -----------------------------------------------------------------------*/
    if(shadow_nv.pri_ha_ip != NULL)
    {
      MSG_MED("Using shadow NV item for Pri HA",0,0,0);
      session->ha_addrs[PRIMARY_HA_INDEX] = ntohl(* shadow_nv.pri_ha_ip);
    }
    else
    {
      session->ha_addrs[PRIMARY_HA_INDEX] = DYNAMIC_HA_ADDR;
    }
   
#if MAX_HA_ADDRS > 1
    if(shadow_nv.sec_ha_ip != NULL)
    {
      MSG_MED("Using shadow NV item for Sec HA",0,0,0);
      session->ha_addrs[SECONDARY_HA_INDEX] = ntohl(* shadow_nv.sec_ha_ip);
    }
    else
    {
      session->ha_addrs[SECONDARY_HA_INDEX] = UNSET_HA_ADDR;
    }
#endif /* MAX_HA_ADDRS > 1 */
 
    /*-----------------------------------------------------------------------
      Load MN_AAA and MN_HA SPI values 
      Use shadow NV value if defined, else set to default.
     ----------------------------------------------------------------------*/
    if (shadow_nv.mn_aaa_spi != NULL) 
    {
      MSG_MED("Using shadow NV item for AAA-SPI",0,0,0);
      session->mn_aaa_spi = * shadow_nv.mn_aaa_spi;
    }
    else
    {
      session->mn_aaa_spi = MIP_CHAP_SPI;
    }

    if (shadow_nv.mn_ha_spi != NULL) 
    {
      MSG_MED("Using shadow NV item for HA-SPI",0,0,0);
      session->mn_ha_spi = * shadow_nv.mn_ha_spi;
    }
    else
    {
      session->mn_ha_spi = MIP_MD5_SPI;
    }
   
    /*-----------------------------------------------------------------------
      Load reverse tunneling preference
      Use shadow NV value if defined, else set to default.
    -----------------------------------------------------------------------*/
    if (shadow_nv.rev_tun != NULL)
    {
      MSG_MED("Using shadow NV item for Rev Tun",0,0,0);
      session->mn_request_rev_tun = *shadow_nv.rev_tun;
    }
    else
    {
      session->mn_request_rev_tun = 0;
    }
  } /* else if profile is disabled */

  /*-------------------------------------------------------------------------
    Optimization Heuristics
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    If all HAs are unset, set the primary HA to dynamic.  Since the 
    mobile is configured to try MIP, this is a best effort heuristic.
  -------------------------------------------------------------------------*/
  if (session->ha_addrs[PRIMARY_HA_INDEX] == UNSET_HA_ADDR)
  {
#if MAX_HA_ADDRS > 1
    if (session->ha_addrs[SECONDARY_HA_INDEX] != UNSET_HA_ADDR)
    {
      session->ha_addrs[PRIMARY_HA_INDEX] = 
        session->ha_addrs[SECONDARY_HA_INDEX];
      session->ha_addrs[SECONDARY_HA_INDEX] = UNSET_HA_ADDR;
    }
    else /* secondary HA is unset as well */
#endif /* MAX_HA_ADDRS > 1 */
    {
      session->ha_addrs[PRIMARY_HA_INDEX] = DYNAMIC_HA_ADDR;
    }
  }

  /*-------------------------------------------------------------------------
    Check for error conditions.  
  -------------------------------------------------------------------------*/
#if MAX_HA_ADDRS > 1
  /*-------------------------------------------------------------------------
    Primary and Secondary HAs cannot both be dynamic.  
    If so, reset secondary to UNSET.
  -------------------------------------------------------------------------*/
  if ( session->ha_addrs[PRIMARY_HA_INDEX] == DYNAMIC_HA_ADDR &&
       session->ha_addrs[SECONDARY_HA_INDEX] == DYNAMIC_HA_ADDR )
  {
    MSG_ERROR("Pri and Sec HA are both Dynamic",0,0,0);
    MSG_MED("Changing sec HA to UNSET",0,0,0);
    session->ha_addrs[SECONDARY_HA_INDEX] = UNSET_HA_ADDR;
  }
#endif /* MAX_HA_ADDRS > 1 */

  /*-------------------------------------------------------------------------
    One of NAI and Home address must be known
    If not, flag an error and fail configuration
  -------------------------------------------------------------------------*/
  if (session->home_addr == DYNAMIC_HOME_ADDR &&
      session->nai_len == 0)
  {
    MSG_ERROR ("Home address and NAI length are both 0", 0, 0, 0);
    return (-1);
  }

  /*-------------------------------------------------------------------------
    Set the session info valid flag and return the active profile number
    to the caller.
  -------------------------------------------------------------------------*/
  session->valid = TRUE;
   
  return (session->profile_index);

} /* mipi_get_nv_info() */



/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIP_CONFIG_SESSION()

DESCRIPTION
  Load MIP session defaults from NV to all sessions. Note: this function
  be invoked before the traffic channel is established.

PARAMETERS
  None

RETURN VALUE
  TRUE: No errors encoutered
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_config_session ( void )
{
  int    loop;
  sint15 profile_num; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Load MIP session defaults from NV

    If multiple MIP sessions are allowed simultaneously, 
    we will loop through to configure each session. 
  -------------------------------------------------------------------------*/
  for (loop=0; loop < MIP_MAX_SESSIONS; loop++)
  {
    /*---------------------------------------------------------------------
      Configure session #'loop'.  
      Config function will return the profile being used for this session.
    ---------------------------------------------------------------------*/
    profile_num = mipi_get_nv_info(INDEX_2_MIP_SES_PTR(loop));
    if (profile_num == -1)
    {
      /*---------------------------------------------------------------------
         NV get failed
      ---------------------------------------------------------------------*/
      ERR("Errors encountered while retrieving NV info!",0,0,0);
      return FALSE;
    }

    /*-----------------------------------------------------------------------
      If upper level software has registered a callback to
      populate/modify the session and/or password info, call it now.

      We retrieve the session info from NV first to allow UI to use
      modifications to NV settings if desired.
    -----------------------------------------------------------------------*/
    if (mip_get_session_info_cb != NULL)
    {
      mip_get_session_info_cb(profile_num, INDEX_2_MIP_SES_PTR(loop));
    }

    if (mip_get_pwd_cb != NULL)
    {
      mip_get_pwd_cb(profile_num, &mip_pwd_info);
    }
  }

  return TRUE;

} /* mip_config_session() */

/*===========================================================================
FUNCTION MIP_REG_SES_INFO_CB()

DESCRIPTION
  Registers a user callback to populate MIP session info control block.

PARAMETERS
  fn_ptr:  Pointer to user routine of the following signature:
           void fn_ptr( byte profile_num, mip_session_info_type * ses_info)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  The session info block settings may affect MIP negotiation
===========================================================================*/
void mip_reg_ses_info_cb
(
  void (* callback)( byte profile_num, mip_session_info_type * ses_info )
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Save the pointer to callback
  -------------------------------------------------------------------------*/
  if (callback != NULL)
  {
    mip_get_session_info_cb = callback;
  }
  else
  {
    mip_get_session_info_cb = NULL;
  }
} /* mip_reg_ses_info_cb() */

/*===========================================================================
FUNCTION MIP_REG_PWD_INFO_CB()

DESCRIPTION
  Registers a user callback to provide MIP password info. 

PARAMETERS
  fn_ptr:  Pointer to user routine of the following signature:
           void fn_ptr( byte profile_num, mip_pwd_info_type * pwd_info )

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  The session info block settings may affect MIP negotiation
===========================================================================*/
void mip_reg_pwd_info_cb
(
  void (* callback)( byte profile_num, mip_pwd_info_type * pwd_info )
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Save the pointer to callback
  -------------------------------------------------------------------------*/
  if (callback != NULL)
  {
    mip_get_pwd_cb = callback;
  }
} /* mip_reg_pwd_info_cb() */

/*===========================================================================
FUNCTION MIP_CALL_EVENT_CALLBACK()

DESCRIPTION
  Traverses the MIP event queue and calls the passed event. 

  NOTE: The callback function should not dealloc the event notification
        structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Generates MIP events
===========================================================================*/
void mip_call_event_callback
(
  mip_event_e_type    event
)
{
  mip_event_info_s_type    *event_ptr;
  mip_session_info_type    *mip_session;
  uint8 event_mask = (1<<event);
  mip_fail_reason_e_type   fail_reason = MIP_FAIL_REASON_NONE;
  byte                     i=0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( MIP_MAX_EV <= event )
  {
    MSG_ERROR("Invalid args passed. Event: %d", event, 0, 0);
    ASSERT( 0 );
    return;
  }

  mip_session = &mip_ses_info_array[0];

  /*-------------------------------------------------------------------------
    Traverse queue of MIP event info structures
  -------------------------------------------------------------------------*/
  for (i=0; i<mip_session->mip_event_client_cnt; i++)
  {
    event_ptr = &mip_event_buf[i];

    if( 0 != (event_mask & event_ptr->event_mask))
    {
      if (MIP_FAILURE_EV == event)
      {
        fail_reason = mip_session->mip_fail_reason;
        mip_session->mip_fail_reason = MIP_FAIL_REASON_NONE;
      }
      else
      {
        fail_reason = MIP_FAIL_REASON_NONE;
      }

      MSG_MED( "Calling callbacks. Event %d Fail Reason %d", 
               event, fail_reason, 0 );
      event_ptr->cback( event,
                        fail_reason,
                        event_ptr->user_data
                      );
    }
  }

  return;
} /* mip_call_event_callback() */

/*===========================================================================
FUNCTION MIP_EVENT_ALLOC()

DESCRIPTION
  This function will allocate a MIP event information structure and 
  register callback functions.

PARAMETERS
  mip_cb_f:         callback function to call
  user_data:        user data to be returned with the callback fn.

RETURN VALUE
  NULL on error
  handle to the MIP event information structure on success

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a MIP event information structure and places it on the event
  queue.  Initializes event masks and registers callbacks for MIP protocol.
===========================================================================*/
void *mip_event_alloc
(
  mip_event_cback_f_ptr_type  mip_cb_f,
  void                        *user_data
)
{
  mip_event_info_s_type *event_info_ptr = NULL;
  mip_session_info_type *mip_session;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check  that the callback is not NULL.
  -------------------------------------------------------------------------*/
  if( NULL == mip_cb_f )
  {
    MSG_ERROR("NULL functions passed", 0, 0, 0);
    ASSERT( 0 );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Get default MIP session
  -------------------------------------------------------------------------*/
  mip_session = &mip_ses_info_array[0];

  if( mip_session->mip_event_client_cnt >= MIP_EVENT_CLIENT_MAX )
  {
    MSG_ERROR("Couldn't allocate event info structure", 0, 0, 0);
    ASSERT( 0 );
  }
  else
  {
    /*-------------------------------------------------------------------------
      Get a new event information structure
    -------------------------------------------------------------------------*/
    event_info_ptr = &mip_event_buf[mip_session->mip_event_client_cnt++];

    /*-----------------------------------------------------------------------
      Initialize the event information structure
    -----------------------------------------------------------------------*/
    memset( event_info_ptr, 0, sizeof(mip_event_info_s_type) );

    /*-----------------------------------------------------------------------
      Fill the structure with the passed callbacks and user_data cookies
    -----------------------------------------------------------------------*/
    event_info_ptr->cback             = mip_cb_f;
    event_info_ptr->user_data         = user_data;
  }

  return event_info_ptr;
} /* mip_event_alloc() */

/*===========================================================================
FUNCTION MIP_EVENT_REGISTER()

DESCRIPTION
  This function will add to an event mask for a  MIP session.
  It will  generate an UP event if the event mask is registering for an UP
  event and the UP event was previously not registerd for this protocol and
  the associated protocol is already up.

PARAMETERS
  handle      PPP event structure to add this event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Adds the passed event mask to the already registered event mask
===========================================================================*/
int32 mip_event_register
(
  void                  *handle,
  mip_event_mask_e_type  event_mask
)
{
  mip_event_info_s_type *event_info_ptr = handle;
  uint8 supported_events = MIP_SUCCESS_EV_MASK | MIP_FAILURE_EV_MASK;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DS_MOBILE_IP_DEREG
  supported_events  = supported_events | MIP_DEREGED_EV_MASK;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
  /*-------------------------------------------------------------------------
    Verify that the passed arguments are valid
  -------------------------------------------------------------------------*/
  if( NULL == handle ||
      0 == (supported_events & event_mask) )
  {
    MSG_ERROR( "invalid args: handle 0x%x, event_mask 0x%x", 
               handle, event_mask, 0 );
    ASSERT( 0 );
    return -1;
  }

  /*-------------------------------------------------------------------------
    If registering a mask for a protocol for which no callback is registerd
    return error.
  -------------------------------------------------------------------------*/
  if( NULL == event_info_ptr->cback )
  {
    MSG_ERROR( "Registering event for a protocol with NULL callback", 0, 0, 0);
    ASSERT( 0 );
    return -1;
  }
  
  /*-------------------------------------------------------------------------
    Set the event_mask for the event information structure
  -------------------------------------------------------------------------*/
  event_info_ptr->event_mask |= event_mask;

  return 0;
} /* mip_event_register() */

#ifdef FEATURE_DS_MULTIPLE_NAI
/*===========================================================================
FUNCTION MIP_GET_RM_NAI()

DESCRIPTION
  This function retrieves the NAIs for a given MIP profile for the
  tethered device and stores them in the mip session info.

PARAMETERS
  session: pointer to the mip session control block where info is stored
  profile: profile number associated with the mip session.

RETURN VALUE
  -1 if session information retrieved from NV is insufficient to make a call
  otherwise, the current profile no. that was passed in as argument.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sint15 mip_get_rm_nai
(
  mip_session_info_type *  session,
  int                      active_mip_profile
)
{
  nv_stat_enum_type nv_status;
  char *            nai_temp;
  nv_item_type      nv_item;
  byte              nai_len=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check to be sure the a valid session control block was specified
  -------------------------------------------------------------------------*/
  if (session == NULL)
  {
    ASSERT(0);
    return (-1);
  }

  /*-------------------------------------------------------------------------
    Retrieve currently selected, or "active", profile from NV.  
    Return from function if there is no active profile.
  -------------------------------------------------------------------------*/
  nv_item.ds_mip_rm_nai.index = active_mip_profile;
  nv_status = dsatutil_get_nv_item( NV_DS_MIP_RM_NAI_I, &nv_item);
  if( nv_status != NV_DONE_S )
  {
    MSG_ERROR("Laptop NAI cannot be retrieved", 0, 0, 0);
    return (-1);
  }

  nai_temp = (char*) nv_item.ds_mip_rm_nai.nai;
  nai_len = nv_item.ds_mip_rm_nai.nai_length;
      
  /*-----------------------------------------------------------------------
    Only copy NAI if its length is valid.  If NAI length is > max allowed
    assume a memory/flash scribble and set length to 0.
  -----------------------------------------------------------------------*/
  if(nai_len > NV_MAX_NAI_LENGTH)
  {
    nai_len = 0;
  }
  else if(nai_len > 0)
  {
    session->nai_len = nai_len;
    memcpy(session->nai, nai_temp, nai_len);
    MSG_LOW("session->nai_len is %d", session->nai_len, 0, 0);
  }

  /*-------------------------------------------------------------------------
    return the active profile number to the caller.
  -------------------------------------------------------------------------*/
  return (session->profile_index);

} /* mip_get_rm_nai() */
#endif /* FEATURE_DS_MULTIPLE_NAI */

#endif /* FEATURE_DS_MOBILE_IP */

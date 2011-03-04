/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                        C M    P R E F

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file is the main implementation for the PREF module. This module will
  control the state transitions from/to hybrid and non hybrid operation and
  decide when to kick hdr.

EXTERNALIZED FUNCTIONS
  Functional Interface:
    cmpref_proc_rpt_srv_ind
    cmpref_proc_cmd_oprt_mode_chgd
    cmpref_proc_cmd_pref_sys_chgd
    cmpref_proc_cmd_subsc_chgd
    cmpref_proc_cmd_call_end
    cmpref_proc_timer
    cmpref_proc_cmd_disable_hybr_hdr
    cmpref_proc_cmd_enable_hybr_hdr
    cmpref_proc_cmd_avoid_hybr_hdr_orig
    cmpref_is_hybr_status_on
    cmpref_is_hybr_hdr_call_allowed
    cmpref_init

REGIONAL FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cmpref_init() should be called before any APIs are called.


Copyright (c) 2003 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmpref.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/09   rk      Handling new pref_reason SD_SS_PREF_REAS_MULTIMODE_SCAN
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
01/19/08   sv      Pass the corresponding SS to term_pending_get_networks_req
09/16/08   pk      Added support for WLAN UOoS
06/13/08   sv      Memory Reduction for ULC
06/12/08   sv      Resolve Klocwork defects
03/25/08   vs      Hybr On, sends phone's mode pref to hybr stack instead
                   of the hardcoded default values.
03/05/08   vk      corrected code for HYBR_GW configuration
08/07/07   ic      Lint clean up
06/11/07   pk      Lint cleanup - 01
02/12/07   pk      Honouring phone's band preference for HDR systems
12/07/06   pk      Added CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV
11/11/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
09/17/06   sk      Passing down band info for WLAN manual system selection.
08/29/06   sk      Added WLAN Adhoc support.
08/11/06   pk      Added support for UOOS
07/14/06   pk      Check the realign_hdr flag in 
                   cmpref_hy_on_ss_main__acq_main before kicking HDR
04/19/06   jqi     Updated the band preferece for hybr ss prefence upon sel
                   system preference command.
                   command2) to process 64-bit band preference. 
03/13/06   sk      Allow call on HDR when idling in non-usable HDR system.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/07/05   jqi     Jump start HDR instance when we are in HICPS on JCDMA 
                   build.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
10/25/05   pk      Fixed RVCT compiler warnings
09/27/05   ic      Fixed Lint issues
09/14/05   pk      Modified cmpref_proc_cmd_pref_sys_chgd, if ss=MAX and 
                   wlan ptr is NULL, WLAN instance is not forced
08/26/05   pk      Reverted the 08/16/05 change in 
                   cmpref_hy_on_ss_main__acq_main()
08/17/05   ic      Removed FEATURE_HDR_MIN_CALL_SETUP_TIME (obsolete)
08/16/05   pk      Added support for priority Queues for origination 
                   parameters
08/16/05   pk      Modified cmpref_hy_on_ss_main__acq_main() to check for 
                   re_align HDR before sending a pref_sys changed to HDR
08/16/05   ic      Removed calls to cm_send_hdr_unlock_rf_cmd()
08/08/05   pk      Corrected cm_pref state diagram
08/02/05   ic      Print value that caused error in default switch case
08/01/05   dk      Fixed sending pref_sys changed to WLAN only if SS is WLAN.
07/15/05   dk      Corrected wlan_pref_reas and orig_mode for WLAN SS while
                   handling pref_sys_chgd command.
06/30/05   pk      Modified cmpref_is_hybr_status_on() to return the
                   hybrid status from the Serving system object.
05/18/05   pk      Serving system objects hdr_hybrid field is changed to
                   reflect the true state of the hybrid status
05/04/05   jqi      Fixed for hybr data switch over.
04/08/05   pk      Modified the handling of the pref_sys_chgd command. Added
                   the check for orig_mode for non-HDR operation
03/14/05   dk      Fixed pref_sys_chgd command handling to not send
                   pref_sys_chgd to MAIN or HDR instance for WLAN instance
                   manual selection.
03/07/05   dk      Fixed pref_sys_chgd command handling to not send
                   pref_sys_chgd to MAIN or HDR instance if only WLAN instance
                   is being effected.
03/03/05   ka      Adding support to let mmoc know of protocol that has a
                   subs change when calling mmoc_cmd_subscription_chg
02/25/05   dk      Fixed issue related to WLAN Jump Start during Subscription
                   change.
02/09/05   ic      Lint cleanup
02/01/05   dk      Added WLAN Support.
01/14/05   sj      Fixed issue with powerup in hdr only mode.
11/16/04   sj      Updated avoid_type mapping to SD types.
11/15/04   ic      Lint cleanup with CM_DEBUG on
10/10/04   sj      Don't allow calls on hybrid hdr if 1x indicates no colloc.
09/14/04   ws      Remove calls to srch_rude_wakeup.
09/10/04   jqi     Any systerm preference change or operation mode change
                   should terminate a pending get network request.
08/20/04   sj      Added support for PRL Preference.
08/05/04   ic      Removed missed Lint 788 enabling/disabling comments
08/04/04   ic      Removed enabling/disabling of Lint error 788 as it is
                   globally suppressed in Lint config files.
                   Enabling it locally in CM files caused inconsistent
                   Lint issues reports when a file was linted separately
                   versus when it was linted as part of *.c batch
07/21/04   sj      Added support for umts to 1x handover.
07/02/04   sj      Fixed issue with non-hybrid to hybrid transition.
06/28/04   sj      Lint cleanup.
06/23/04   ic      Fixed ADS 120 compiler warnings (Saber build)
06/22/04   ic      Fixed obvious RVCT 2.1 compiler warnings
05/14/04   ka      Fixed Lint errors.
04/08/04   ws      Updated function names.
03/24/04   ic      Fixed compiler warning.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/06/04   ws      Initial Jaguar/Mainline merge.
01/13/04   sj      Fixed issuw with OTASP with hybrid PRL, HDR instance wasn't
                   realigned after OTASP.
01/13/04   sj      If there is service on HDR SD instance, then hybrid HDR
                   call is allowed irrespective of cmpref state.
12/16/03   rc/aaj  Added support for FEATURE_HDR_HICPS20
12/03/03   sj      Added srch_rudewakeup when sending pref sys changed to
                   SS-Main.
11/07/03   sj      Fixed issue with kicking HDR anytime after HDR call.
11/06/03   sj      Fixed issue with kicking HDR if SID/NID changes during
                   HDR call.
10/22/03   sj      Send unlock RF command to HDRMC only if orig_mode!= NONE.
10/14/03   sj      Added support for FEATURE_HDR_MIN_CALL_SETUP_TIME.
10/14/03   sj      Added support for HDR reselect feature.
10/07/03   sj      Send unlock RF command to HDRMC before sending
                   pref. sys. chgd command for SD_SS_MAIN for call origs.
09/26/03   sj      Removed compiler warnings.
09/24/03   sj      Fixed issue with mode/hybrid validation in
                   pref_sys_chgd().
09/12/03   vt      Moved hybrid instance parametrs from cdma structure to
                   common for serving system.
09/05/03   sj      Using the sd_misc_is_mode_pref() to check mode_pref.
08/27/03   ic      Made cmpref_is_hybr_allowed() external.
                   Changed calls map_mode_pref_to_sd_mode_pref() to
                   map_cm_mode_pref_to_sd_mode_pref().
06/22/03   SJ      Initial release.

===========================================================================*/
/*

                          CMPREF STATE MACHINE
                          ====================


                              +-------------+
                              |             |
                              |    NULL     |
                              |    (0)      |
                              |             |
                              +-------------+
                                |   /|\    |
                                |    |     |  (online,hy_off)
                        (online,|    |     +--------------------+
                         hy_on) |    |(oprt_mode!=              |
                                |    | ONLINE)                  |
                                |    |                          |
                                |                               |
                                |                               |
                               \|/            (pref_sys_chgd   \|/
   +-----------+              +-------------+  hybr_allowed)+-------------+
   | HY_ON_SS_ | (lost_main   | HY_ON_SS_   |<--------------| HY_OFF_SS_  |
   | MAIN_NO_  |------------->| MAIN        |----------+    | MAIN        |
   | SRV_HDR_  | _timeout)    |             |          |    |             |
   | ACQ_TO    |              |             |<-----+   |    |             |
   |    (4)    |-----+        |     (1)     |      |   |    |     (6)     |
   +-----------+     |        +-------------+      |   |    +-------------+
    /|\      |       |           /|\       |       |   |     |       /|\
     |       |       |            |        |       |   |     |        |
   (lost_    |   (acq_main,     (lost_  (acq_main, |   |     |        |
   main)     |   kick_hdr=       main)  is_hdr_acq |   |   (acq_     (lost_
     |       |   true)            |     = false)   |   |   main)     main)
     |       |       |            |        |       |   |     |        |
     |       |       |            |        |       |   |     |        |
     |       |       |            |        |       |   |     |        |
     |       |       |            |        |       |   |     |        |
     |    (acq_main, |            |        |       |   |     |        |
     |     kick_hdr= |            |        |       |   |     |        |
     |     false)    +---------+  |        |       |   |     |        |
     |       |                 |  |        |       |   |     |        |
     |      \|/               \|/ |       \|/      |   |    \|/       |
   +-----------+ (kick_hdr=   +-------------+      |   |    +--------------+
   | HY_ON_SS_ |------------->| HY_ON_SS_   |      |   |    | HY_OFF_SS_   |
   | MAIN_SRV  |   true)      | MAIN_SRV_   | (pref_sys_chgd| MAIN         |
   | HDR_      |              | HDR         |<--------------|              |
   | ACQ_TO    |              |             |  hybr_allowed)|              |
   |   (5)     |<-------------|    (2)      |      |   |    |     (7)      |
   +-----------+ (acq_hdr_to) +-------------+      |   |    +--------------+
     |                         /|\        |        |   |
     |                          |         |        |   |
     |                          |         |        | (acq_main,
     |                        (lost_    (acq_      |  is_hdr_acq
     |                        hdr)       hdr)      |  = true)
     |                          |         |        |   |
   (acq_hdr)                    |         |     (lost_ |
     |                          |         |      hdr)  |
     |                          |         |        |   |
     |                          |        \|/       |   |
     |                        +-------------+      |   |
     |                        | HY_ON_SS_   |      |   |
     +----------------------->| MAIN_SRV_   |      |   |
                              | HDR_SRV     |      |   |
                              |    (3)      |<---------+
                              +-------------+      |
                                 |      /|\        |
                                 |       |         |
                                 |       |         |
                                 |       |         |
                                 |       |         |
                              (lost_  (acq_     (lost_
                               main)   main)     hdr)
                                 |       |         |
                                 |       |         |
                                 |       |         |
                                 |       |         |
                                \|/      |         |
                              +-------------+      |
                              | HY_ON_SS_   |      |
                              | MAIN_NO_SRV_|      |    
                              | HDR_SRV     |------+
                              |    (8)      |
                              +-------------+                             

                              


Note:
  pref_sys_chgd( hybr_not_allowed )
   From states 1 & 4    => HY_OFF_SS_MAIN
   From states 2,5 & 3  => HY_OFF_SS_MAIN_SRV

----------------------------------------------------------------------------      
paragk                                                           
Note: Need to add CDMA + WCDMA hybrid operation state diagram
*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "customer.h" /* Customer configuration file */

#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "rex.h"      /* Rex related services */

#include "sys.h"      /* Common types. */
#include "sd.h"       /* Interface for System Determination services */
#include "mmoc.h"     /* External interface definitions for MMoC */

#include "cmpref.h"   /* Interface to CM pref extern fns. */
#include "cmprefi.h"  /* Interface to CM pref internal data structures. */
#include "cmph.h"     /* Interface to CM phone object. */
#include "cmss.h"     /* Interface to CM Serving system object. */
#include "cmdbg.h"    /* Interface to CM debug macros.  */

#include "clk.h"
#include "net.h"

#ifdef FEATURE_UMTS_1X_HANDOVER
#error code not present
#endif

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* Definition for PREF's info.
*/
static  cmpref_info_s_type             cmpref_info;



/*===========================================================================
=============================================================================
=============================================================================
==================== PREF IMPLEMENTATION RELATED HELPER FUNCTIONS =======
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmpref_get_info_ptr

DESCRIPTION
  This function will fetch the pointer to the PREF's info.

DEPENDENCIES
  None.

RETURN VALUE
  cmpref_info_s_type* - Pointer to PREF's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN cmpref_info_s_type*           cmpref_get_info_ptr
(

       void

)
{
  /* Return the pointer to the local definition of PREF's info.
  */
  return &cmpref_info;

} /* cmpref_get_info_ptr() */



/*===========================================================================

FUNCTION cmpref_update_state

DESCRIPTION
  This function sets the PREF's state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmpref_update_state
(

       cmpref_state_e_type             new_state
          /* The new state of the PREF.
          */

)
{

  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();
  sd_ss_mode_pref_e_type sd_mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT_ENUM_IS_INRANGE(new_state, CMPREF_STATE_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  CM_MSG_HIGH("update_state(),state %d, new %d",
                   cmpref_get_info_ptr()->state,
                   new_state,
                   0
                  );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the state.
  */
  cmpref_info_ptr->state = new_state;

  sd_mode_pref = SD_GET_COMMON_MODE( cmpref_info_ptr->hybr_1_ss_pref.mode_pref,
          cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ));

  sd_mode_pref = SD_GET_COMMON_MODE( sd_mode_pref,
                                     cmpref_info_ptr->mode_pref_mask );
  
  switch(new_state)
  {

    case CMPREF_STATE_HY_ON_SS_MAIN:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV:

      cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_COLLOC;
      cmpref_info_ptr->hybr_1_ss_pref.mode_pref = sd_mode_pref;
      cmss_ptr()->info.hdr_hybrid = TRUE;
      break;

    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_UMTS_SRV:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_UMTS:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_UMTS_SRV:

      cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_NONE;
      cmpref_info_ptr->hybr_1_ss_pref.mode_pref = sd_mode_pref;
      cmss_ptr()->info.hdr_hybrid = TRUE;
      break;

    default:
      cmss_ptr()->info.hdr_hybrid = FALSE;
  } /* switch(new_state) */

} /* cmpref_update_state() */


/*===========================================================================

FUNCTION cmpref_is_hybr_allowed

DESCRIPTION
  This function returns whether hybrid is allowed or not based on the
  mode_pref and hybrid_pref passed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean                         cmpref_is_hybr_allowed
(

       sd_ss_mode_pref_e_type          mode_pref,
          /* Mode preference to use.
          */

       sd_ss_hybr_pref_e_type          hybr_pref
          /* Hybrid preference to use.
          */
)
/*lint -esym(715,mode_pref,hybr_pref)*/
{

  #if  ( defined(FEATURE_HDR_HYBRID) || defined(FEATURE_HYBR_GW))
#error code not present
  #endif


  /* Hybrid is not allowed.
  */
  return FALSE;

} /* cmpref_is_hybr_allowed() */
/*lint +esym(715,mode_pref, hybr_pref)*/



/*===========================================================================

FUNCTION cmpref_send_pref_sys_chgd_cmd

DESCRIPTION
  This function should be called to send the pref sys. changed command to
  MMOC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmpref_send_pref_sys_chgd_cmd
(

        sd_ss_e_type                   ss,
            /* Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */

        sd_ss_pref_reas_e_type         pref_reas,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /* The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /* The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /* Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /* Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        sys_plmn_id_s_type               plmn_id,
           /* The PLMN id to be acquired, valid when the net_sel_mode_pref
           ** is MANUAL.
           ** Applies only for GSM/WCDMA modes.
           */


        sd_ss_srv_domain_pref_e_type       domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type        acq_order_pref
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  cm_act_orig_s_type *pref_sent_ptr   = cmph_get_last_pref_sent_on_main();

  sys_manual_sys_info_s_type       sys_id;

  CM_ASSERT( pref_sent_ptr != NULL );

  /* Initialize SYS_ID for the SS_MAIN.
  */

  sys_id.sys_id.id_type  = SYS_SYS_ID_TYPE_UMTS;
  sys_id.sys_id.id.plmn  = plmn_id;
  sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Terminate any pending get networks request. */
  cmph_term_pending_get_networks_req(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if pref sys. chgd has to sent to SS-W:AN.
  */
  #ifndef FEATURE_CM_EXTERNAL_WLAN
  /* no need to call pref sys chgd if external wlan 
  */
  if ( ss == SD_SS_HYBR_WLAN )
  {

    #ifdef FEATURE_WLAN_ADHOC
#error code not present
    #else
    mmoc_cmd_pref_sys_chgd( SD_SS_HYBR_WLAN,
                            pref_reas,
                            orig_mode,
                            mode_pref,
                            band_pref,
                            prl_pref,
                            roam_pref,
                            hybr_pref,
                            otasp_band,
                            otasp_blksys,
                            avoid_type,
                            avoid_time,
                            &cmpref_info_ptr->wlan_ss_pref.sys_id,
                            domain_pref,
                            acq_order_pref,
                            cmpref_info_ptr->wlan_ss_pref.wlan_tech_pref,
                            cmpref_info_ptr->wlan_ss_pref.wlan_scan_pref
                          );
    #endif
  } /* if ( ss == SD_SS_MAX || ss == SD_SS_HYBR_WLAN ) */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if pref sys. chgd has to sent to SS-HDR.
  */
  if ( ss == SD_SS_MAX || ss == SD_SS_HYBR_1 )
  {
    sd_ss_mode_pref_e_type sd_mode_pref = 
        SD_GET_COMMON_MODE( mode_pref,
                            cmpref_info_ptr->hybr_1_ss_pref.mode_pref );

    sd_mode_pref = SD_GET_COMMON_MODE( sd_mode_pref,
                                       cmpref_info_ptr->mode_pref_mask );

    #ifdef FEATURE_WLAN_ADHOC
#error code not present
    #else
    mmoc_cmd_pref_sys_chgd( SD_SS_HYBR_1,
                            pref_reas,
                            cmpref_info_ptr->hybr_1_ss_pref.orig_mode,
                            sd_mode_pref,
                            cmpref_info_ptr->hybr_1_ss_pref.band_pref,
                            cmpref_info_ptr->hybr_1_ss_pref.prl_pref,
                            cmpref_info_ptr->hybr_1_ss_pref.roam_pref,
                            cmpref_info_ptr->hybr_1_ss_pref.hybr_pref,
                            cmpref_info_ptr->hybr_1_ss_pref.otasp_band,
                            cmpref_info_ptr->hybr_1_ss_pref.otasp_blksys,
                            cmpref_info_ptr->hybr_1_ss_pref.avoid_type,
                            cmpref_info_ptr->hybr_1_ss_pref.avoid_time,
                            &cmpref_info_ptr->hybr_1_ss_pref.sys_id,
                            cmph_map_cm_srv_domain_pref_to_sd(
                                  cmtask_compute_srv_domain_pref(
                                                              CM_SS_HYBR_1 ) ),

                            cmpref_info_ptr->hybr_1_ss_pref.acq_order_pref,
                            cmpref_info_ptr->hybr_1_ss_pref.wlan_tech_pref,
                            cmpref_info_ptr->hybr_1_ss_pref.wlan_scan_pref
                          );
    #endif

    cmpref_info_ptr->is_kick_hdr_pending  = FALSE;

  } /* if ( ss == SD_SS_MAX || ss == SD_SS_HYBR_HDR ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if pref sys. chgd has to sent to SS-Main.
  */
  if ( ss == SD_SS_MAX || ss == SD_SS_MAIN )
  {

    #ifdef FEATURE_WLAN_ADHOC
#error code not present
    #else
    mmoc_cmd_pref_sys_chgd(SD_SS_MAIN,
                            pref_reas,
                            orig_mode,
                            SD_GET_COMMON_MODE( mode_pref,
                                                cmpref_info_ptr->mode_pref_mask
                                              ),
                            band_pref,
                            prl_pref,
                            roam_pref,
                            hybr_pref,
                            otasp_band,
                            otasp_blksys,
                            avoid_type,
                            avoid_time,
                            &sys_id,
                            cmph_map_cm_srv_domain_pref_to_sd(
                                    cmtask_compute_srv_domain_pref(
                                                              CM_SS_MAIN ) ),
                            acq_order_pref,
                            cmpref_info_ptr->wlan_ss_pref.wlan_tech_pref,
                            cmpref_info_ptr->wlan_ss_pref.wlan_scan_pref
                          );
    #endif
    /* Update the last pref sent
    */

    pref_sent_ptr->act_priority   = CM_ACT_PRIORITY_PH;

    pref_sent_ptr->orig_band_pref = band_pref;
    pref_sent_ptr->orig_hybr_pref = cmph_map_sd_hybr_pref_to_cm_hybr_pref(hybr_pref);
    pref_sent_ptr->orig_mode      = orig_mode;
    pref_sent_ptr->orig_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(
                                      SD_GET_COMMON_MODE( mode_pref,
                                                          cmpref_info_ptr->mode_pref_mask
                                                        ));
    pref_sent_ptr->orig_pref_term = CM_PREF_TERM_NONE;
    pref_sent_ptr->orig_prl_pref  = cmph_map_sd_prl_pref_to_cm_prl_pref(prl_pref);
    pref_sent_ptr->orig_roam_pref = cmph_map_sd_roam_pref_to_cm_roam_pref(roam_pref);
    pref_sent_ptr->orig_srv_domain_pref = cmtask_compute_srv_domain_pref( CM_SS_MAIN );

  } /* if ( ss == SD_SS_MAX || ss == SD_SS_MAIN ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
} /* cmpref_send_pref_sys_chgd_cmd() */




/*===========================================================================
=============================================================================
=============================================================================
==================== SERVICE INDICATION EVENT HANDLER ========================
=============================================================================
=============================================================================
===========================================================================*/

#ifdef FEATURE_HYBR_GW
#error code not present
#endif

/*===========================================================================

FUNCTION cmpref_hy_on_ss_main__acq_main

DESCRIPTION
  This function should be called to process the internal event SS-Main
  acquired when in state CMPREF_STATE_HY_ON_SS_MAIN ( SS-Main is attempting
  acquisitions ).

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmpref_hy_on_ss_main__acq_main
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  CM_ASSERT( cmpref_info_ptr->state == CMPREF_STATE_HY_ON_SS_MAIN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initiate SS-HDR acquisition, if SS-HDR is not in a call.
  */
  if( cmpref_info_ptr->hybr_1_ss_pref.hybr_pref == SD_SS_HYBR_PREF_CDMA__HDR ||
      cmpref_info_ptr->hybr_1_ss_pref.hybr_pref == SD_SS_HYBR_PREF_CDMA__HDR_WCDMA )
  {
  
    cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_COLLOC;
    cmpref_info_ptr->hybr_1_ss_pref.mode_pref = SD_SS_MODE_PREF_HDR_WCDMA;

    if ( !cmcall_is_mobile_in_hdr_call() )
    {
      cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                     SD_SS_PREF_REAS_USER,
                                     SD_SS_ORIG_MODE_COLLOC,
                                     cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ),
                                     cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),
                                     cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                     cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                     cmpref_info_ptr->hybr_1_ss_pref.hybr_pref,
                                     SD_BAND_PCS,
                                     SD_BLKSYS_PCS_A,
                                     SD_SS_AVOID_SYS_IDLE,
                                     0,
                                     cmph_ptr()->plmn,
                                     cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                     cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                   );

    }
    else
    {
      /* Set HDR pref. sys chg pending.
      */
      cmpref_info_ptr->is_kick_hdr_pending = TRUE;
    }

    /* Check if SS-HDR already has service.
    */
    if ( sys_srv_status_is_srv( cmss_ptr()->info.hdr_srv_status ) && 
     rpt_ptr->srv_ind_info.si_info.mode != SYS_SYS_MODE_HDR )
    {
      /* SS-HDR instance has service, Update next state.
      */
      cmpref_update_state( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV );

    }
    #ifdef FEATURE_HYBR_GW
#error code not present
    #endif
    else
    {
      /* SS-HDR instance has no service, Start "acq_hdr_timer".
      */
      cmpref_info_ptr->acq_hybr_1_uptime = clk_uptime() + CMPREF_ACQ_HDR_TIMER;

      /* Update next state.
      */
      cmpref_update_state( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR );

    } /* if () */
  } /* if */
  else if( cmpref_info_ptr->hybr_1_ss_pref.hybr_pref == SD_SS_HYBR_PREF_CDMA__WCDMA )
  {

    /* Update the CM pref object
    */
    cmpref_info_ptr->hybr_1_ss_pref.mode_pref = SD_SS_MODE_PREF_GW;
    cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_NONE;

    /* Change the CM pref state
    */
    cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                   SD_SS_PREF_REAS_USER,
                                   SD_SS_ORIG_MODE_NONE,
                                   SD_SS_MODE_PREF_ANY,
                                   cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),                                     
                                   cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                   cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                   cmph_ptr()->int_hybr_pref,
                                   SD_BAND_PCS,
                                   SD_BLKSYS_PCS_A,
                                   SD_SS_AVOID_SYS_IDLE,
                                   0,
                                   cmph_ptr()->plmn,
                                   cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                   cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                 );
  
    #ifdef FEATURE_HYBR_GW
#error code not present
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main__acq_main() */
/*lint +esym(715,rpt_ptr)*/

/*===========================================================================

FUNCTION cmpref_hy_on_ss_main__acq_hdr

DESCRIPTION
  This function should be called to process the internal event SS-Main
  acquired when in state CMPREF_STATE_HY_ON_SS_MAIN ( SS-Main is attempting
  acquisitions ).

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmpref_hy_on_ss_main__acq_hdr
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  CM_ASSERT( cmpref_info_ptr->state == CMPREF_STATE_HY_ON_SS_MAIN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Stop "acq_hdr_timer".
  */
  cmpref_info_ptr->acq_hybr_1_uptime = CMPREF_UPTIME_NONE;

  /* Check if SS-MAIN already has service.
  */
  if ( sys_srv_status_is_srv( cmss_ptr()->info.srv_status ) )
  {
    /* SS-MAIN instance has service, Update next state.
    */
    cmpref_update_state( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV );

  }
  else
  {
    /* Update next state.
    */
    cmpref_update_state( CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV );

  } /* if () */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main__acq_hdr() */
/*lint +esym(715,rpt_ptr)*/


/*===========================================================================

FUNCTION cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__main_srv_chgd

DESCRIPTION
  This function should be called to process the internal event SS-Main service
  changed( SS-Main service reacquired and hdr needs realigning ) when in state
  CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO ( SS-Main service is lost
  briefly).

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void     cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__main_srv_chgd
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initiate SS-HDR acquisition, if HDR is not in call.
  */
  if ( !cmcall_is_mobile_in_hdr_call() )
  {
    cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                   SD_SS_PREF_REAS_USER,
                                   SD_SS_ORIG_MODE_COLLOC,
                                   cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ),
                                   cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),                                     
                                   cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                   cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                   SD_SS_HYBR_PREF_CDMA_HDR,
                                   SD_BAND_PCS,
                                   SD_BLKSYS_PCS_A,
                                   SD_SS_AVOID_SYS_IDLE,
                                   0,
                                   cmph_ptr()->plmn,
                                   cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                   cmph_map_cm_acq_order_pref_to_sd  (cmph_ptr()->acq_order_pref)
                                 );


  }
  else
  {
    /* Set HDR pref. sys chg pending.
    */
    cmpref_info_ptr->is_kick_hdr_pending = TRUE;

  }

  /* Check if SS-HDR already has service.
  */
  if ( sys_srv_status_is_srv( cmss_ptr()->info.hdr_srv_status ) )
  {
    /* Update next state.
    */
    cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV );

  }
  else
  {
  /* Start "acq_hdr_timer".
  */
    cmpref_info_ptr->acq_hybr_1_uptime = clk_uptime() + CMPREF_ACQ_HDR_TIMER;

    /* Update next state.
  */
    cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR );

  }

  /* Stop "lost_main_timer".
  */
  cmpref_info_ptr->lost_main_uptime  = CMPREF_UPTIME_NONE;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__main_srv_chgd() */
/*lint +esym(715,rpt_ptr)*/



/*===========================================================================

FUNCTION cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__main_srv_same

DESCRIPTION
  This function should be called to process the internal event SS-Main service
  same( SS-Main service reacquired and HDR doesn't need reailgning) when in
  state CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO( SS-Main service lost
  briefly).

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void
                 cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__main_srv_same
(
       const cm_sd_rpt_u_type          *rpt_ptr
          /* Pointer to the CM_SRV_IND_INFO_F report.
          */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Stop "lost_main_timer".
  */
  cmpref_info_ptr->lost_main_uptime  = CMPREF_UPTIME_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if SS-HDR already has service.
  */
  if ( sys_srv_status_is_srv( cmss_ptr()->info.hdr_srv_status ) )
  {
    /* Update next state with HDR srv.
    */
    cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV );

  }
  else
  {
    /* Update next state with HDR acq. timed out.
    */
    cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__main_srv_same() */
/*lint +esym(715,rpt_ptr)*/




/*===========================================================================

FUNCTION cmpref_hy_off_ss_main__acq_main

DESCRIPTION
  This function should be called to process the internal event SS-Main acquired
  service when in state CMPREF_STATE_HY_OFF_SS_MAIN.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmpref_hy_off_ss_main__acq_main
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_OFF_SS_MAIN_SRV );


} /*  cmpref_hy_off_ss_main__acq_main() */
/*lint +esym(715,rpt_ptr)*/



/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_srv_hdr__lost_main

DESCRIPTION
  This function should be called to process the internal event SS-Main lost
  service when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                         cmpref_hy_on_ss_main_srv_hdr__lost_main
(
        const cm_sd_rpt_u_type       *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Stop "acq_hdr_timer".
  */
  cmpref_info_ptr->acq_hybr_1_uptime =  CMPREF_UPTIME_NONE;


  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN );

  /* UOOS kick HDR
  */
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  if ( !cmcall_is_mobile_in_hdr_call() )
  {
     cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                    SD_SS_PREF_REAS_USER,
                                    SD_SS_ORIG_MODE_COLLOC,
                                    cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ),
                                    cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),
                                    cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                    cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                    SD_SS_HYBR_PREF_CDMA_HDR,
                                    SD_BAND_PCS,
                                    SD_BLKSYS_PCS_A,
                                    SD_SS_AVOID_SYS_IDLE,
                                    0,
                                    cmph_ptr()->plmn,
                                    cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                    cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                   );

  }
  else
  {   
    /* Set HDR pref. sys chg pending.
    */
    cmpref_info_ptr->is_kick_hdr_pending = TRUE;
  }
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_srv_hdr__lost_main() */
/*lint +esym(715,rpt_ptr)*/



/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_srv_hdr__main_srv_chgd

DESCRIPTION
  This function should be called to process the internal event SS-Main service
  changed when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR( SS-Main has service
  and non service on SS-HDR ).

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                     cmpref_hy_on_ss_main_srv_hdr__main_srv_chgd
(
        const cm_sd_rpt_u_type         *rpt_ptr
          /* Pointer to the CM_SRV_IND_INFO_F report.
          */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Re-Initiate SS-HDR acquisition, if HDR is not in call.
  */
  if ( !cmcall_is_mobile_in_hdr_call() )
  {
    cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                   SD_SS_PREF_REAS_USER,
                                   SD_SS_ORIG_MODE_COLLOC,
                                   cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ),
                                   cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),
                                   cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                   cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                   SD_SS_HYBR_PREF_CDMA_HDR,
                                   SD_BAND_PCS,
                                   SD_BLKSYS_PCS_A,
                                   SD_SS_AVOID_SYS_IDLE,
                                   0,
                                   cmph_ptr()->plmn,
                                   cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                   cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                 );
  }
  else
  {
    /* Set HDR pref. sys chg pending.
    */
    cmpref_info_ptr->is_kick_hdr_pending = TRUE;

  }

  /* Restart "acq_hdr_timer".
  */
  cmpref_info_ptr->acq_hybr_1_uptime = clk_uptime() + CMPREF_ACQ_HDR_TIMER;


  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_srv_hdr__main_srv_chgd() */
/*lint +esym(715,rpt_ptr)*/



/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_srv_hdr__acq_hdr

DESCRIPTION
  This function should be called to process the internal event SS-HDR acquired
  service when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR( SS-Main has service
  and no service on SS-HDR).

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmpref_hy_on_ss_main_srv_hdr__acq_hdr
(
        const cm_sd_rpt_u_type         *rpt_ptr
          /* Pointer to the CM_SRV_IND_INFO_F report.
          */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Stop "acq_hdr_timer".
  */
  cmpref_info_ptr->acq_hybr_1_uptime = CMPREF_UPTIME_NONE;

  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_srv_hdr__acq_hdr() */
/*lint +esym(715,rpt_ptr)*/


/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_srv_hdr_acq_to__lost_main

DESCRIPTION
  This function should be called to process the internal event SS-Main srv
  is lost when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                  cmpref_hy_on_ss_main_srv_hdr_acq_to__lost_main
(
        const cm_sd_rpt_u_type         *rpt_ptr
          /* Pointer to the CM_SRV_IND_INFO_F report.
          */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start "lost_main_timer".
  */
  cmpref_info_ptr->lost_main_uptime = clk_uptime() + CMPREF_LOST_MAIN_TIMER;


  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO );

  /* UOOS kick HDR
  */
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  if ( !cmcall_is_mobile_in_hdr_call() )
  {
     cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                    SD_SS_PREF_REAS_USER,
                                    SD_SS_ORIG_MODE_COLLOC,
                                    cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ),
                                    cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),
                                    cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                    cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                    SD_SS_HYBR_PREF_CDMA_HDR,
                                    SD_BAND_PCS,
                                    SD_BLKSYS_PCS_A,
                                    SD_SS_AVOID_SYS_IDLE,
                                    0,
                                    cmph_ptr()->plmn,
                                    cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                    cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                   );

  }
  else
  {   
    /* Set HDR pref. sys chg pending.
    */
    cmpref_info_ptr->is_kick_hdr_pending = TRUE;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_srv_hdr_acq_to__lost_main() */
/*lint +esym(715,rpt_ptr)*/


/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_srv_hdr_acq_to__main_srv_chgd

DESCRIPTION
  This function should be called to process the internal event SS-Main srv
  changed when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void              cmpref_hy_on_ss_main_srv_hdr_acq_to__main_srv_chgd
(
        const cm_sd_rpt_u_type         *rpt_ptr
          /* Pointer to the CM_SRV_IND_INFO_F report.
          */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Re-Initiate SS-HDR acquisition, if HDR is not in call.
  */
  if ( !cmcall_is_mobile_in_hdr_call() )
  {
    cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                   SD_SS_PREF_REAS_USER,
                                   SD_SS_ORIG_MODE_COLLOC,
                                   cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ),
                                   cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),
                                   cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                   cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                   SD_SS_HYBR_PREF_CDMA_HDR,
                                   SD_BAND_PCS,
                                   SD_BLKSYS_PCS_A,
                                   SD_SS_AVOID_SYS_IDLE,
                                   0,
                                   cmph_ptr()->plmn,
                                   cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                   cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                 );

  }
  else
  {
    /* Set HDR pref. sys chg pending.
    */
    cmpref_info_ptr->is_kick_hdr_pending = TRUE;

  }

  /* Restart "acq_hdr_timer".
  */
  cmpref_info_ptr->acq_hybr_1_uptime = clk_uptime() + CMPREF_ACQ_HDR_TIMER;


  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_srv_hdr_acq_to__main_srv_chgd() */
/*lint +esym(715,rpt_ptr)*/



/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_srv_hdr_acq_to__acq_hdr

DESCRIPTION
  This function should be called to process the internal event SS-HDR srv
  acquired when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                    cmpref_hy_on_ss_main_srv_hdr_acq_to__acq_hdr
(
        const cm_sd_rpt_u_type         *rpt_ptr
          /* Pointer to the CM_SRV_IND_INFO_F report.
          */
)
/*lint -esym(715,rpt_ptr)*/
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_srv_hdr_acq_to__acq_hdr() */
/*lint +esym(715,rpt_ptr)*/



/*===========================================================================

FUNCTION cmpref_hy_off_ss_main_srv__lost_main

DESCRIPTION
  This function should be called to process the internal event SS-MAIN srv
  is lost when in state CMPREF_STATE_HY_OFF_SS_MAIN_SRV.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmpref_hy_off_ss_main_srv__lost_main
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_OFF_SS_MAIN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_off_ss_main_srv__lost_main() */
/*lint +esym(715,rpt_ptr)*/


/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_srv_hdr_srv__lost_hdr

DESCRIPTION
  This function should be called to process the internal event SS-HDR srv
  is lost when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                      cmpref_hy_on_ss_main_srv_hdr_srv__lost_hdr
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Start "acq_hdr_timer".
  */
  cmpref_info_ptr->acq_hybr_1_uptime = clk_uptime() + CMPREF_ACQ_HDR_TIMER;

  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_srv_hdr_srv__lost_hdr() */
/*lint +esym(715,rpt_ptr)*/


/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_srv_hdr_srv__main_srv_chgd

DESCRIPTION
  This function should be called to process the internal event SS-MAIN srv
  changed when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                 cmpref_hy_on_ss_main_srv_hdr_srv__main_srv_chgd
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Re-Initiate SS-HDR acquisition if not in HDR call.
  */
  if ( !cmcall_is_mobile_in_hdr_call() )
  {
    cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                   SD_SS_PREF_REAS_USER,
                                   SD_SS_ORIG_MODE_COLLOC,
                                   cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ),
                                   cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),
                                   cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                   cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                   SD_SS_HYBR_PREF_CDMA_HDR,
                                   SD_BAND_PCS,
                                   SD_BLKSYS_PCS_A,
                                   SD_SS_AVOID_SYS_IDLE,
                                   0,
                                   cmph_ptr()->plmn,
                                   cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                   cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                 );


  }
  else
  {
    /* Set HDR pref. sys chg pending.
    */
    cmpref_info_ptr->is_kick_hdr_pending = TRUE;

  }

  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_srv_hdr_srv__main_srv_chgd() */
/*lint +esym(715,rpt_ptr)*/



/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_srv_hdr_srv__lost_main

DESCRIPTION
  This function should be called to process the internal event SS-MAIN srv
  is lost when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                     cmpref_hy_on_ss_main_srv_hdr_srv__lost_main
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_srv_hdr_srv__lost_main() */
/*lint +esym(715,rpt_ptr)*/


/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_no_srv_hdr_srv__lost_hdr

DESCRIPTION
  This function should be called to process the internal event SS-HDR srv
  is lost when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                    cmpref_hy_on_ss_main_no_srv_hdr_srv__lost_hdr
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_no_srv_hdr_srv__lost_hdr() */
/*lint +esym(715,rpt_ptr)*/


/*===========================================================================

FUNCTION cmpref_hy_on_ss_main_no_srv_hdr_srv__acq_main

DESCRIPTION
  This function should be called to process the internal event SS-HDR srv
  is lost when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                    cmpref_hy_on_ss_main_no_srv_hdr_srv__acq_main
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
/*lint -esym(715,rpt_ptr)*/
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  CM_ASSERT( cmpref_info_ptr->state == CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV );

  cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_COLLOC;
  cmpref_info_ptr->hybr_1_ss_pref.mode_pref = SD_SS_MODE_PREF_HDR_WCDMA;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( rpt_ptr->srv_ind_info.si_info.realign_hdr )
  {
    /* Initiate SS-HDR acquisition, if SS-HDR is not in a call.
    */
    if ( !cmcall_is_mobile_in_hdr_call() )
    {
      cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_HDR,
                                     SD_SS_PREF_REAS_USER,
                                     SD_SS_ORIG_MODE_COLLOC,
                                     cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ),
                                     cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),
                                     cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                     cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                     SD_SS_HYBR_PREF_CDMA_HDR,
                                     SD_BAND_PCS,
                                     SD_BLKSYS_PCS_A,
                                     SD_SS_AVOID_SYS_IDLE,
                                     0,
                                     cmph_ptr()->plmn,
                                     cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                     cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                   );

    }
    else
    {
      /* Set HDR pref. sys chg pending.
      */
      cmpref_info_ptr->is_kick_hdr_pending = TRUE;

    }
  }
  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_hy_on_ss_main_no_srv_hdr_srv__acq_main() */
/*lint +esym(715,rpt_ptr)*/

#ifdef FEATURE_HYBR_GW
#error code not present
#endif /* FEATURE_HYBR_GW */

/*===========================================================================

FUNCTION cmpref_proc_rpt_srv_ind

DESCRIPTION
  This function should be called to process the CM_SRV_IND_INFO_F report
  from SD20.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_proc_rpt_srv_ind
(
        const cm_sd_rpt_u_type         *rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */

)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( rpt_ptr != NULL); 
  CM_ASSERT( rpt_ptr->hdr.cmd == CM_SRV_IND_INFO_F );

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif
  /* Process event based on the state.
  */
  switch ( cmpref_info_ptr->state )
  {
    case CMPREF_STATE_HY_ON_SS_MAIN:

      /* Check if SS-Main acquired service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status ) &&
           !rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq )
      {
        /* Acquired SS-Main service and not doing preferred service acq.
        */
        cmpref_hy_on_ss_main__acq_main( rpt_ptr );
        break;
      }

      #ifdef FEATURE_HYBR_GW
#error code not present
      #endif

      /* Check if SS-HDR acquired service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_HYBR_1  &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status ) &&
           rpt_ptr->srv_ind_info.si_info.mode == SD_MODE_HDR
         )
      {
        /* SS-HDR acquired service.
        */
        cmpref_hy_on_ss_main__acq_hdr( rpt_ptr );
        break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO:
      /* Check if SS-Main acquired service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN  &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status ) &&
           !rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq )
      {
        /* SS-Main acquired service, check if main service parameters
        ** changed or not.
        */
        if ( rpt_ptr->srv_ind_info.si_info.realign_hdr )
        {
          /* SS-Main acquired service and service parameters have
          ** changed.
          */
          cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__main_srv_chgd( rpt_ptr );
          break;
        }
        else
        {
          /* SS-Main acquired service and service parameters have
          ** changed.
          */
          cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__main_srv_same( rpt_ptr );
          break;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR:
      /* Check if SS-Main lost service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN &&
           !sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status )
         )
      {
        /* SS-Main lost service.
        */
        cmpref_hy_on_ss_main_srv_hdr__lost_main( rpt_ptr );
        break;
      }

      /* Check if relevant service parameters have changed.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN  &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status ) &&
           !rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq &&
           rpt_ptr->srv_ind_info.si_info.realign_hdr
         )

      {
        /* SS-Main service parameters have changed.
        */
        cmpref_hy_on_ss_main_srv_hdr__main_srv_chgd( rpt_ptr );
        break;
      }

      #ifdef FEATURE_HYBR_GW
#error code not present
      #endif

      /* Check if SS-HDR acquired service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_HYBR_1  &&
           rpt_ptr->srv_ind_info.si_info.mode == SD_MODE_HDR &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status )
         )
      {
        /* SS-HDR acquired service.
        */
        cmpref_hy_on_ss_main_srv_hdr__acq_hdr( rpt_ptr );
        break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO:
      /* Check if SS-Main lost service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN &&
           !sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status )
         )
      {
        /* SS-Main lost service.
        */
        cmpref_hy_on_ss_main_srv_hdr_acq_to__lost_main( rpt_ptr );
        break;
      }

      /* Check if relevant service parameters have changed.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN  &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status ) &&
           !rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq &&
           rpt_ptr->srv_ind_info.si_info.realign_hdr
         )
      {
        /* SS-Main service parameters have changed.
        */
        cmpref_hy_on_ss_main_srv_hdr_acq_to__main_srv_chgd( rpt_ptr );
        break;
      }

      /* Check if SS-HDR acquired service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_HYBR_HDR  &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status )
         )
      {
        /* SS-HDR acquired service.
        */
        cmpref_hy_on_ss_main_srv_hdr_acq_to__acq_hdr( rpt_ptr );
        break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV:
      /* Check if SS-HDR service was lost.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_HYBR_1 &&
           !sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status )
         )
      {
        /* SS-HDR service was lost.
        */
        cmpref_hy_on_ss_main_srv_hdr_srv__lost_hdr( rpt_ptr );
        break;
      }

      /* Check if SS-Main service parameters have changed.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN  &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status ) &&
           !rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq &&
           rpt_ptr->srv_ind_info.si_info.realign_hdr
         )
      {
         /* SS-Main service parameters have changed.
         */
         cmpref_hy_on_ss_main_srv_hdr_srv__main_srv_chgd( rpt_ptr );
         break;
      }

      /* Check if SS-Main lost service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN &&
           !sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status )
         )
      {
        /* SS-Main lost service.
        */
        cmpref_hy_on_ss_main_srv_hdr_srv__lost_main( rpt_ptr );
        break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV:
      /* Check if SS-HDR service was lost.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_HYBR_1 &&
           !sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status )
         )
      {
        /* SS-HDR service was lost.
        */
        cmpref_hy_on_ss_main_no_srv_hdr_srv__lost_hdr( rpt_ptr );
        break;
      }

      /* Check if SS-Main acquired service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status ) &&
           !rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq )
      {
        /* Acquired SS-Main service and not doing preferred service acq.
        */
        cmpref_hy_on_ss_main_no_srv_hdr_srv__acq_main( rpt_ptr );
        break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMPREF_STATE_HY_OFF_SS_MAIN_SRV:
      /* Check if SS-Main lost service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN &&
           !sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status )
         )
      {
        /* SS-Main lost service.
        */
        cmpref_hy_off_ss_main_srv__lost_main( rpt_ptr );
        break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMPREF_STATE_HY_OFF_SS_MAIN:
      /* Check if SS-Main acquired service.
      */
      if ( rpt_ptr->srv_ind_info.ss == SD_SS_MAIN &&
           sys_srv_status_is_srv( rpt_ptr->srv_ind_info.si_info.srv_status ) &&
           !rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq )
      {
        /* Acquired SS-Main service and not doing preferred service acq.
        */
        cmpref_hy_off_ss_main__acq_main( rpt_ptr );
        break;
      }
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_HYBR_GW
#error code not present
    #endif /* FEATURE_HYBR_GW */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMPREF_STATE_NULL:
    case CMPREF_STATE_MAX:
    default:
      CM_MSG_HIGH( "Recvd srv_ind_info in incorrect state = %d", 
                    cmpref_get_info_ptr()->state, 0, 0 );
      break;
  }

} /* cmpref_proc_rpt_srv_ind() */



/*===========================================================================
=============================================================================
=============================================================================
==================== OPERATING MODE EVENT HANDLER ===========================
=============================================================================
=============================================================================
===========================================================================*/



/*===========================================================================

FUNCTION cmpref_state_null__online_hy_on

DESCRIPTION
  This function should be called to process the internal event ONLINE with
  allowed when in state CMPREF_STATE_NULL.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmpref_state_null__online_hy_on
(
        void
)
{

  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmpref_info_ptr->is_kick_hdr_pending  = FALSE;
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_state_null__online_hy_on() */



/*===========================================================================

FUNCTION cmpref_state_null__online_hy_off

DESCRIPTION
  This function should be called to process the internal event ONLINE with
  not allowed when in state CMPREF_STATE_NULL.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmpref_state_null__online_hy_off
(
        void
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update next state with no srv on SS-MAIN
  */
  cmpref_update_state ( CMPREF_STATE_HY_OFF_SS_MAIN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_state_null__online_hy_off() */



/*===========================================================================

FUNCTION cmpref_state_any__shutdown

DESCRIPTION
  This function should be called to process the internal event is !ONLINE
  when in state other than CMPREF_STATE_NULL.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmpref_state_any__shutdown
(
        void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Reinit the CMPREF module.
  */
  cmpref_init();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_state_any__shutdown() */




/*===========================================================================

FUNCTION cmpref_proc_cmd_oprt_mode_chgd

DESCRIPTION
  This function should be called when the operating mode of the phone has
  changed.This function should be called just before sending the operating
  mode changed command to MMOC.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_proc_cmd_oprt_mode_chgd
(
        sys_oprt_mode_e_type           oprt_mode
           /* New operating mode of the phone.
           */

)
{

  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  cmph_s_type      *ph_ptr = cmph_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Terminate any pending get network request. */
  cmph_term_pending_get_networks_req(SD_SS_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start WLAN Acquisition*/

  cmpref_set_wlan_pref(&ph_ptr->wlan_pref);

  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #else
  mmoc_cmd_pref_sys_chgd( SD_SS_HYBR_WLAN,
                          SD_SS_PREF_REAS_USER,
                          cmpref_info_ptr->wlan_ss_pref.orig_mode,
                          cmpref_info_ptr->wlan_ss_pref.mode_pref,
                          cmpref_info_ptr->wlan_ss_pref.band_pref,
                          cmpref_info_ptr->wlan_ss_pref.prl_pref,
                          cmpref_info_ptr->wlan_ss_pref.roam_pref,
                          cmpref_info_ptr->wlan_ss_pref.hybr_pref,
                          cmpref_info_ptr->wlan_ss_pref.otasp_band,
                          cmpref_info_ptr->wlan_ss_pref.otasp_blksys,
                          cmpref_info_ptr->wlan_ss_pref.avoid_type,
                          cmpref_info_ptr->wlan_ss_pref.avoid_time,
                          &cmpref_info_ptr->wlan_ss_pref.sys_id,
                          cmpref_info_ptr->wlan_ss_pref.domain_pref,
                          cmpref_info_ptr->wlan_ss_pref.acq_order_pref,
                          cmpref_info_ptr->wlan_ss_pref.wlan_tech_pref,
                          cmpref_info_ptr->wlan_ss_pref.wlan_scan_pref
                        );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( cmpref_info_ptr->state )

  {
    case CMPREF_STATE_NULL:
      /* Check if the operating mode is ONLINE.
      */
      if ( oprt_mode == SYS_OPRT_MODE_ONLINE ||
           oprt_mode == SYS_OPRT_MODE_PSEUDO_ONLINE
         )

      {
        /* Update HDR preferences.
        */
        cmpref_info_ptr->hybr_1_ss_pref.mode_pref =
          cmph_map_cm_mode_pref_to_sd_mode_pref(cmph_ptr()->mode_pref);

        cmpref_info_ptr->hybr_1_ss_pref.hybr_pref =cmph_ptr()->int_hybr_pref;

        #ifdef FEATURE_HYBR_GW
#error code not present
        #endif
        /* Check if hybrid is allowed.
        */
        if ( cmpref_is_hybr_allowed( cmph_map_cm_mode_pref_to_sd_mode_pref(cmph_ptr()->mode_pref),
                                     cmph_ptr()->int_hybr_pref
                                   )
           )
        {
          /* Phone has moved to online and hybrid is allowed.
          */
          cmpref_state_null__online_hy_on();
        }
        else
        {
          /* Phone has moved to online and hybrid is not allowed.
          */
          cmpref_state_null__online_hy_off();
        }

        /* Disable Hybrid HDR if in pseudo-online mode.
        */
        if ( oprt_mode == SYS_OPRT_MODE_PSEUDO_ONLINE )
        {
          cmpref_proc_cmd_disable_hybr_hdr();
        }

      } /* if ( oprt_mode == SYS_OPRT_MODE_ONLINE) */
      break;

    default:
      /* Check if the operating mode is not ONLINE.
      */
      if ( oprt_mode != SYS_OPRT_MODE_ONLINE &&
           oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
         )
      {
        /* Phone is moving non-online operating mode.
        */
        cmpref_state_any__shutdown();

      } /* if ( oprt_mode != SYS_OPRT_MODE_ONLINE) */
      break;

  } /*  switch ( cmpref_info_ptr->state ) */



} /*  cmpref_proc_cmd_oprt_mode_chgd() */



/*===========================================================================
=============================================================================
=============================================================================
==================== PREF. SYS. CHANGED EVENT HANDLER =======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmpref_proc_cmd_pref_sys_chgd

DESCRIPTION
  This function should be called when the phone preferences have changed.
  This module will be responsible for queing pref_sys_chgd command to MMOC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_proc_cmd_pref_sys_chgd
(

        sd_ss_e_type                   ss,
            /* Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */

        sd_ss_pref_reas_e_type         pref_reas,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /* The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /* The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /* Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /* Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        sys_plmn_id_s_type             plmn_id,
           /* The PLMN id to be acquired, valid when the net_sel_mode_pref
           ** is MANUAL.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_srv_domain_pref_e_type   domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        const cm_wlan_pref_s_type     *wlan_pref_ptr
           /* WLAN System Preferences.
           */
)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  /* Initialize WLAN Pref reason.
  */
  sd_ss_pref_reas_e_type   wlan_pref_reas = pref_reas;

  sd_ss_orig_mode_e_type   wlan_orig_mode = orig_mode;



  /* Send command to WLAN SS if required.
  */

  if( (ss == SD_SS_MAX && wlan_pref_ptr  !=  NULL) || (ss == SD_SS_HYBR_WLAN) )
  {

    CM_ASSERT( wlan_pref_ptr  !=  NULL );

    /* Update WLAN prefs.
    */
    cmpref_set_wlan_pref(wlan_pref_ptr);

    if (pref_reas == SD_SS_PREF_REAS_MULTIMODE_SCAN)
    {
      wlan_pref_reas = SD_SS_PREF_REAS_USER;
    }
    cmpref_info_ptr->wlan_ss_pref.mode_pref = mode_pref;
    cmpref_info_ptr->wlan_ss_pref.band_pref = band_pref;

    /* Set wlan_orig_mode appropriately based on pref_reas and network
    ** selection mode.
    */

    if(wlan_pref_reas == SD_SS_PREF_REAS_USER_RESEL ||
       wlan_pref_reas == SD_SS_PREF_REAS_USER ||
       wlan_pref_reas == SD_SS_PREF_REAS_STACK_SYNC_UP )
    {
      if(wlan_pref_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
      {
        if( wlan_pref_reas != SD_SS_PREF_REAS_STACK_SYNC_UP )
        {
          wlan_pref_reas   = SD_SS_PREF_REAS_USER_RESEL;
        }
        wlan_orig_mode   = SD_SS_ORIG_MODE_MANUAL_IDLE;
      }
      else
      {
        wlan_orig_mode = SD_SS_ORIG_MODE_NONE;
      }
    }
    else if(wlan_pref_reas != SD_SS_PREF_REAS_AVOID_SYS   &&
            wlan_pref_reas != SD_SS_PREF_REAS_ORIG_END  )
    {
      if(wlan_pref_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
      {
        wlan_orig_mode = SD_SS_ORIG_MODE_MANUAL_ORIG;
      }
      else
      {
        wlan_orig_mode = SD_SS_ORIG_MODE_NORMAL;
      }
    }
    else if(wlan_pref_reas == SD_SS_PREF_REAS_ORIG_END)
    {
      if(wlan_pref_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
      {
        wlan_orig_mode = SD_SS_ORIG_MODE_MANUAL_IDLE;
      }
    }


    cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_WLAN,
                                   wlan_pref_reas,
                                   wlan_orig_mode,
                                   cmpref_info_ptr->wlan_ss_pref.mode_pref,
                                   cmpref_info_ptr->wlan_ss_pref.band_pref,
                                   cmpref_info_ptr->wlan_ss_pref.prl_pref,
                                   cmpref_info_ptr->wlan_ss_pref.roam_pref,
                                   cmpref_info_ptr->wlan_ss_pref.hybr_pref,
                                   cmpref_info_ptr->wlan_ss_pref.otasp_band,
                                   cmpref_info_ptr->wlan_ss_pref.otasp_blksys,
                                   cmpref_info_ptr->wlan_ss_pref.avoid_type,
                                   cmpref_info_ptr->wlan_ss_pref.avoid_time,
                                   plmn_id,
                                   cmpref_info_ptr->wlan_ss_pref.domain_pref,
                                   cmpref_info_ptr->wlan_ss_pref.acq_order_pref
                                 );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If only WLAN SS is being affected, no need to send indication to
  ** other SS.
  */
  if( ss == SD_SS_HYBR_WLAN )
  {
      return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update preferences for HDR.
  */
  if ( ss == SD_SS_MAX || ss == SD_SS_HYBR_1 )
  {
    cmpref_info_ptr->hybr_1_ss_pref.mode_pref   = mode_pref;
    cmpref_info_ptr->hybr_1_ss_pref.hybr_pref   = hybr_pref;
    cmpref_info_ptr->hybr_1_ss_pref.band_pref   = band_pref;
    cmpref_info_ptr->hybr_1_ss_pref.prl_pref    = prl_pref;
    cmpref_info_ptr->hybr_1_ss_pref.roam_pref   = roam_pref;
    cmpref_info_ptr->hybr_1_ss_pref.domain_pref = domain_pref; 

    cmpref_info_ptr->hybr_1_ss_pref.sys_id.sys_id.id_type  = SYS_SYS_ID_TYPE_UMTS; 
    cmpref_info_ptr->hybr_1_ss_pref.sys_id.sys_id.id.plmn  = plmn_id;
    cmpref_info_ptr->hybr_1_ss_pref.sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;

    #ifdef FEATURE_HYBR_GW
#error code not present
    #else
    cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_COLLOC;
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if mode/hybrid pref. is valid.
  */
  if ( ss == SD_SS_MAIN  &&
       cmpref_is_hybr_status_on() &&
       ( hybr_pref == SD_SS_HYBR_PREF_CDMA_HDR ||
         hybr_pref == SD_SS_HYBR_PREF_CDMA__HDR_WCDMA )&&
       mode_pref == SD_SS_MODE_PREF_HDR
     )
  {
    CM_ASSERT( "Invalid combination" );
    CM_MSG_ERROR("hybr mode pref incorrect %d %d", hybr_pref, mode_pref, 0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Handle pref sys changed based on the state.
  */
  switch ( cmpref_info_ptr->state )
  {
    case CMPREF_STATE_HY_ON_SS_MAIN:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_UMTS_SRV:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV:

      /* Check if hybrid is allowed with the new preferences, only if
      ** the ss is MAX.
      */
      if ( ss == SD_SS_MAX  &&
           !cmpref_is_hybr_allowed( mode_pref, hybr_pref )
         )
      {
        /* Hybrid is not allowed, inform both SD instances, and move
        ** to non-hybrid operation.
        */
        cmpref_info_ptr->hybr_1_ss_pref.hybr_pref = SD_SS_HYBR_PREF_NONE;
        cmpref_send_pref_sys_chgd_cmd( SD_SS_MAX, /* send both instances */
                                       pref_reas,
                                       orig_mode,
                                       mode_pref,
                                       band_pref,
                                       prl_pref,
                                       roam_pref,
                                       SD_SS_HYBR_PREF_NONE,
                                       otasp_band,
                                       otasp_blksys,
                                       avoid_type,
                                       avoid_time,
                                       plmn_id,
                                       domain_pref,
                                       acq_order_pref
                                      );

        cmpref_info_ptr->lost_main_uptime = CMPREF_UPTIME_NONE;
        cmpref_update_state( CMPREF_STATE_HY_OFF_SS_MAIN );

      } /* if ( !cmpref_is_hybr_allowed( mode_pref, hybr_pref ) ) */
      else
      {
        /* Hybrid is allowed, follow ss parameter to inform SD instances.
        */
        cmpref_send_pref_sys_chgd_cmd( ss,
                                       pref_reas,
                                       orig_mode,
                                       mode_pref,
                                       band_pref,
                                       prl_pref,
                                       roam_pref,
                                       hybr_pref,
                                       otasp_band,
                                       otasp_blksys,
                                       avoid_type,
                                       avoid_time,
                                       plmn_id,
                                       domain_pref,
                                       acq_order_pref
                                      );


      } /* else */
      break;


    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_UMTS:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_UMTS_SRV:

      /* Check if hybrid is allowed with the new preferences, only if
      ** the ss is MAX.
      */
      if ( ss == SD_SS_MAX &&
           !cmpref_is_hybr_allowed( mode_pref, hybr_pref )
         )
      {
        /* Hybrid is not allowed, inform both SD instances, and move
        ** to non-hybrid operation.
        */
        cmpref_info_ptr->hybr_1_ss_pref.hybr_pref = SD_SS_HYBR_PREF_NONE;
        cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_NONE;
        cmpref_info_ptr->hybr_1_ss_pref.mode_pref = SD_SS_MODE_PREF_NONE;

        cmpref_send_pref_sys_chgd_cmd( SD_SS_MAX, /* send both instances */
                                       pref_reas,
                                       orig_mode,
                                       mode_pref,
                                       band_pref,
                                       prl_pref,
                                       roam_pref,
                                       SD_SS_HYBR_PREF_NONE,
                                       otasp_band,
                                       otasp_blksys,
                                       avoid_type,
                                       avoid_time,
                                       plmn_id,
                                       domain_pref,
                                       acq_order_pref
                                     );

        cmpref_info_ptr->acq_hybr_1_uptime = CMPREF_UPTIME_NONE;
        cmpref_update_state( CMPREF_STATE_HY_OFF_SS_MAIN_SRV );

      } /* if ( !cmpref_is_hybr_allowed( mode_pref, hybr_pref ) ) */
      else
      {
        /* Hybrid is allowed, follow ss parameter to inform SD instances.
        */
        cmpref_send_pref_sys_chgd_cmd( ss,
                                       pref_reas,
                                       orig_mode,
                                       mode_pref,
                                       band_pref,
                                       prl_pref,
                                       roam_pref,
                                       hybr_pref,
                                       otasp_band,
                                       otasp_blksys,
                                       avoid_type,
                                       avoid_time,
                                       plmn_id,
                                       domain_pref,
                                       acq_order_pref
                                     );


      } /* else */
      break;

    case CMPREF_STATE_HY_OFF_SS_MAIN:
      /* Check if hybrid is allowed with the new preferences and
      ** ss is either MAX.
      */
      if ( ss == SD_SS_MAX &&
           cmpref_is_hybr_allowed( mode_pref, hybr_pref )
         )
      {
        /* Hybrid is allowed, inform SD SS-Main instance, and move
        ** to hybrid operation.
        */
        cmpref_send_pref_sys_chgd_cmd( SD_SS_MAIN,
                                       pref_reas,
                                       orig_mode,
                                       mode_pref,
                                       band_pref,
                                       prl_pref,
                                       roam_pref,
                                       hybr_pref,
                                       otasp_band,
                                       otasp_blksys,
                                       avoid_type,
                                       avoid_time,
                                       plmn_id,
                                       domain_pref,
                                       acq_order_pref
                                     );

        cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_COLLOC;

        cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                       pref_reas,
                                       orig_mode,
                                       mode_pref,
                                       band_pref,
                                       prl_pref,
                                       roam_pref,
                                       hybr_pref,
                                       otasp_band,
                                       otasp_blksys,
                                       avoid_type,
                                       avoid_time,
                                       plmn_id,
                                       domain_pref,
                                       acq_order_pref
                                     );

        cmpref_info_ptr->acq_hybr_1_uptime = CMPREF_UPTIME_NONE;
        cmpref_update_state( CMPREF_STATE_HY_ON_SS_MAIN );

      } /* if ( cmpref_is_hybr_allowed( mode_pref, hybr_pref ) ) */
      else
      {
         /* Hybrid is not allowed, inform SD MAIN instance.
         */
        if( ss != SD_SS_HYBR_1 )
        {
          cmpref_send_pref_sys_chgd_cmd( SD_SS_MAIN,
                                         pref_reas,
                                         orig_mode,
                                         mode_pref,
                                         band_pref,
                                         prl_pref,
                                         roam_pref,
                                         SD_SS_HYBR_PREF_NONE,
                                         otasp_band,
                                         otasp_blksys,
                                         avoid_type,
                                         avoid_time,
                                         plmn_id,
                                         domain_pref,
                                         acq_order_pref
                                      );

        }/* if( ss != SD_SS_HYBR_HDR ) */
        else
        {
          CM_MSG_HIGH( "Wrong ss instance %d", ss, 0,0 );
          break;
        }

      } /* else */
      break;


    case CMPREF_STATE_HY_OFF_SS_MAIN_SRV:
      /* Check if hybrid is allowed with the new preferences and
      ** SS is either MAX.
      */
      if ( ss == SD_SS_MAX &&
           cmpref_is_hybr_allowed( mode_pref, hybr_pref )
         )
      {
        /* Hybrid is allowed, inform SD SS-Main followed by HDR instance, and move
        ** to hybrid operation.
        */
        cmpref_send_pref_sys_chgd_cmd( SD_SS_MAIN,
                                       pref_reas,
                                       orig_mode,
                                       mode_pref,
                                       band_pref,
                                       prl_pref,
                                       roam_pref,
                                       hybr_pref,
                                       otasp_band,
                                       otasp_blksys,
                                       avoid_type,
                                       avoid_time,
                                       plmn_id,
                                       domain_pref,
                                       acq_order_pref
                                     );

        cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_COLLOC;

        cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                       pref_reas,
                                       orig_mode,
                                       mode_pref,
                                       band_pref,
                                       prl_pref,
                                       roam_pref,
                                       hybr_pref,
                                       otasp_band,
                                       otasp_blksys,
                                       avoid_type,
                                       avoid_time,
                                       plmn_id,
                                       domain_pref,
                                       acq_order_pref
                                     );

        cmpref_info_ptr->acq_hybr_1_uptime = CMPREF_UPTIME_NONE;
        cmpref_update_state( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR );

        /* SS-HDR instance has no service, Start "acq_hdr_timer".
        */
        cmpref_info_ptr->acq_hybr_1_uptime = clk_uptime() + CMPREF_ACQ_HDR_TIMER;

      } /* if ( cmpref_is_hybr_allowed( mode_pref, hybr_pref ) ) */
      else
      {
        /* Hybrid is not allowed, inform SD MAIN instance.
        */

        /* Add a check to see if Orig mode as HDR,
        ** if we have it then don't do anytihng
        */
        if( ss != SD_SS_HYBR_1 )
        {

          cmpref_send_pref_sys_chgd_cmd( SD_SS_MAIN,
                                         pref_reas,
                                         orig_mode,
                                         mode_pref,
                                         band_pref,
                                         prl_pref,
                                         roam_pref,
                                         SD_SS_HYBR_PREF_NONE,
                                         otasp_band,
                                         otasp_blksys,
                                         avoid_type,
                                         avoid_time,
                                         plmn_id,
                                         domain_pref,
                                         acq_order_pref
                                     );


        } /* if( ss != SD_SS_HYBR_HDR ) */
        else
        {
          CM_MSG_HIGH( "Wrong ss instance %d", ss, 0,0 );
          break;
        }
      } /* else */
      break;

    case CMPREF_STATE_NULL:
    case CMPREF_STATE_MAX:
    default:
      CM_MSG_HIGH( "Pref_sys in incorrect state %d",
                       cmpref_info_ptr->state,0,0 );
      break;

  }  /* switch ( cmpref_info_ptr->state ) */

} /* cmpref_proc_cmd_pref_sys_chgd() */




/*===========================================================================
=============================================================================
=============================================================================
==================== SUBSCRIPTION CHANGED EVENT HANDLER =====================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmpref_proc_cmd_subsc_chgd

DESCRIPTION
  This function should be called when the subscrption availability has
  changed.This function should be called just before sending the subsc_chgd
  command to MMOC.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_proc_cmd_subsc_chgd
(

        mmoc_subsc_chg_e_type          chg_type,
             /* Subscription change type.
             */

        byte                           nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        sys_plmn_id_s_type            plmn_id,
           /* The PLMN id to be acquired, valid when the net_sel_mode_pref
           ** is MANUAL.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                        is_gw_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_cdma_subsc_avail,
           /* Current CDMA/AMPS/HDR subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_wlan_subsc_avail,
           /* Current WLAN subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        const cm_wlan_pref_s_type          *wlan_pref_ptr,
           /* WLAN System Preferences.
           */

        prot_subsc_chg_e_type               prot_subsc_chg
           /* Enumeration of protocols that have a change in
           ** subscription available status
           */

)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  sys_manual_sys_info_s_type       sys_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(wlan_pref_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize SYS_ID for the SS_MAIN.
  */

  sys_id.sys_id.id_type  = SYS_SYS_ID_TYPE_UMTS;
  sys_id.sys_id.id.plmn  = plmn_id;
  sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;

  /* Restart CM pref module.
  */
  cmpref_init();

  /* Check if the operating mode is ONLINE.
  */

  if ( cmph_ptr()->oprt_mode == SYS_OPRT_MODE_ONLINE ||
       cmph_ptr()->oprt_mode == SYS_OPRT_MODE_PSEUDO_ONLINE
     )
  {

    cmpref_info_ptr->hybr_1_ss_pref.mode_pref   = mode_pref;
    cmpref_info_ptr->hybr_1_ss_pref.hybr_pref   = hybr_pref;
    cmpref_info_ptr->hybr_1_ss_pref.domain_pref = srv_domain_pref;

    /* Check if hybrid is allowed.
    */
    if ( cmpref_is_hybr_allowed( mode_pref, hybr_pref ) )
    {
      /* Phone has moved to online and hybrid is allowed.
      */
      cmpref_state_null__online_hy_on();
    }
    else
    {
      /* Phone has moved to online and hybrid is not allowed.
      */
      cmpref_state_null__online_hy_off();
    }

    /* Disable Hybrid HDR if in pseudo-online mode.
    */
    if ( cmph_ptr()->oprt_mode == SYS_OPRT_MODE_PSEUDO_ONLINE )
    {
      cmpref_proc_cmd_disable_hybr_hdr();
    }
  } /* if ( oprt_mode == SYS_OPRT_MODE_ONLINE) */

  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #else
  mmoc_cmd_subscription_chgd( chg_type,
                              nam,
                              orig_mode,
                              mode_pref,
                              band_pref,
                              prl_pref,
                              roam_pref,
                              cmpref_is_hybr_allowed(mode_pref, hybr_pref) ?
                                            hybr_pref: SD_SS_HYBR_PREF_NONE,
                              &sys_id,
                              srv_domain_pref,
                              acq_order_pref,
                              cmph_map_cm_wlan_tech_pref_to_sd_pref(
                                             wlan_pref_ptr->tech_pref),
                              cmph_map_cm_wlan_scan_pref_to_sys_pref(
                                             wlan_pref_ptr->scan_mode),
                              is_gw_subsc_avail,
                              is_cdma_subsc_avail,
                              is_wlan_subsc_avail,
                              prot_subsc_chg
                            );
  #endif


  if(!is_wlan_subsc_avail)
  {
    return;
  }
  /* Update WLAN preferences.
  */
  cmpref_set_wlan_pref(wlan_pref_ptr);

  cmpref_info_ptr->wlan_ss_pref.mode_pref = mode_pref;
  cmpref_info_ptr->wlan_ss_pref.band_pref = band_pref;

  /* Jump Start WLAN Acquisition one more time now.
  */
  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #else
  mmoc_cmd_pref_sys_chgd(SD_SS_HYBR_WLAN,
                          SD_SS_PREF_REAS_USER,
                          cmpref_info_ptr->wlan_ss_pref.orig_mode,
                          cmpref_info_ptr->wlan_ss_pref.mode_pref,
                          cmpref_info_ptr->wlan_ss_pref.band_pref,
                          cmpref_info_ptr->wlan_ss_pref.prl_pref,
                          cmpref_info_ptr->wlan_ss_pref.roam_pref,
                          cmpref_info_ptr->wlan_ss_pref.hybr_pref,
                          cmpref_info_ptr->wlan_ss_pref.otasp_band,
                          cmpref_info_ptr->wlan_ss_pref.otasp_blksys,
                          cmpref_info_ptr->wlan_ss_pref.avoid_type,
                          cmpref_info_ptr->wlan_ss_pref.avoid_time,
                          &cmpref_info_ptr->wlan_ss_pref.sys_id,
                          cmpref_info_ptr->wlan_ss_pref.domain_pref,
                          cmpref_info_ptr->wlan_ss_pref.acq_order_pref,
                          cmpref_info_ptr->wlan_ss_pref.wlan_tech_pref,
                          cmpref_info_ptr->wlan_ss_pref.wlan_scan_pref
                        );
  #endif

} /*  cmpref_proc_cmd_subsc_chgd() */




/*===========================================================================
=============================================================================
=============================================================================
==================== CALL EVENT HANDLER ====================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmpref_proc_cmd_call_end

DESCRIPTION
  This function should be called to process the call end event from CMCALL.
  This function would reinitiate SS-HDR acquisition if necessary.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void      cmpref_proc_cmd_call_end
(
        void
)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if all HDR calls have ended.
  */
  if ( cmcall_is_mobile_in_hdr_call() )
  {
    /* HDR call still pending.
    */
    CM_MSG_HIGH(" MS still in hdr call", 0, 0, 0 );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Kick HDR if one is pending and we are still in hybrid operation.
  */
  switch ( cmpref_info_ptr->state )
  {
    case CMPREF_STATE_HY_ON_SS_MAIN:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV:
      /* Check if pref_sys_chgd with reason = USER is pending.
      */
      if ( cmpref_info_ptr->is_kick_hdr_pending )
      {
        /* Pending, kick hdr.
        */
        #ifdef FEATURE_WLAN_ADHOC
#error code not present
        #else
        mmoc_cmd_pref_sys_chgd( SD_SS_HYBR_1,
                                SD_SS_PREF_REAS_USER,
                                cmpref_info_ptr->hybr_1_ss_pref.orig_mode,
                                SD_GET_COMMON_MODE( cmpref_info_ptr->hybr_1_ss_pref.mode_pref,
                                                    cmpref_info_ptr->mode_pref_mask
                                                  ),
                                cmpref_info_ptr->hybr_1_ss_pref.band_pref,
                                cmpref_info_ptr->hybr_1_ss_pref.prl_pref,
                                cmpref_info_ptr->hybr_1_ss_pref.roam_pref,
                                cmpref_info_ptr->hybr_1_ss_pref.hybr_pref,
                                cmpref_info_ptr->hybr_1_ss_pref.otasp_band,
                                cmpref_info_ptr->hybr_1_ss_pref.otasp_blksys,
                                cmpref_info_ptr->hybr_1_ss_pref.avoid_type,
                                cmpref_info_ptr->hybr_1_ss_pref.avoid_time,
                                &cmpref_info_ptr->hybr_1_ss_pref.sys_id,
                                cmpref_info_ptr->hybr_1_ss_pref.domain_pref,
                                cmpref_info_ptr->hybr_1_ss_pref.acq_order_pref,
                                cmpref_info_ptr->hybr_1_ss_pref.wlan_tech_pref,
                                cmpref_info_ptr->hybr_1_ss_pref.wlan_scan_pref
                               );
        #endif

        cmpref_info_ptr->is_kick_hdr_pending = FALSE;

      }
      break;

    case CMPREF_STATE_HY_OFF_SS_MAIN:
    case CMPREF_STATE_HY_OFF_SS_MAIN_SRV:
    case CMPREF_STATE_NULL:
    case CMPREF_STATE_MAX:
    default:
      cmpref_info_ptr->is_kick_hdr_pending = FALSE;
      /* Hybrid is off, no need to kick hdr.
      */
      break;

  } /* switch ( cmpref_info_ptr->state ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmpref_proc_cmd_call_end() */



/*===========================================================================
=============================================================================
=============================================================================
==================== TIMER EVENT HANDLER ====================================
=============================================================================
=============================================================================
===========================================================================*/



/*===========================================================================

FUNCTION cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__lost_main_to

DESCRIPTION
  This function should be called to process the internal event "lost_main_timer"
  expires when in state CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void      cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__lost_main_to
(
        void
)
{


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__lost_main_to() */


/*===========================================================================

FUNCTION cmpref_state_hy_on_ss_main_srv_hdr__acq_hdr_to

DESCRIPTION
  This function should be called to process the internal event "acq_hdr_timer"
  expires when in state CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                  cmpref_state_hy_on_ss_main_srv_hdr__acq_hdr_to
(
        void
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update next state.
  */
  cmpref_update_state ( CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /*  cmpref_state_hy_on_ss_main_srv_hdr__acq_hdr_to() */




/*===========================================================================

FUNCTION cmpref_proc_timer

DESCRIPTION
  This function should be called to process the CM's 1 second timer tick.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_proc_timer
(

        void
)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();


  /* Check if "lost_main_timer" expired.
  */
  if( cmpref_info_ptr->lost_main_uptime <= clk_uptime() )
  {
    /* reset the expired timer.
    */
    cmpref_info_ptr->lost_main_uptime = CMPREF_UPTIME_NONE;

    /* "lost_main_timer" expired", check if we are right
    ** state.
    */
    if ( cmpref_info_ptr->state ==
                        CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO )
    {
       /* "lost_main_timer" expired, call handler.
       */
       cmpref_state_hy_on_ss_main_no_srv_hdr_acq_to__lost_main_to();
    }
    else
    {
      /* Not in right state, reset timer.
      */
      CM_MSG_HIGH("lost_main_timer expired wrong state %d",
                      cmpref_info_ptr->state, 0, 0 );
    }

  } /* if( cmpref_info_ptr->lost_main_uptime <= clk_uptime() ) */


  /* Check if "acq_hdr_timer" expired.
  */
  if( cmpref_info_ptr->acq_hybr_1_uptime <= clk_uptime() )
  {
    /* reset the expired timer.
    */
    cmpref_info_ptr->acq_hybr_1_uptime = CMPREF_UPTIME_NONE;

    /* "acq_hdr_timer" expired", check if we are right
    ** state.
    */
    if ( cmpref_info_ptr->state ==
                        CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR )
    {
       /* "acq_hdr_timer" expired, call handler.
       */
       cmpref_state_hy_on_ss_main_srv_hdr__acq_hdr_to();
    }
    else
    {
      /* Not in right state, reset timer.
      */
      CM_MSG_HIGH("acq_hdr_timer expired wrong state %d",
                      cmpref_info_ptr->state, 0, 0 );
    }

  } /* if( cmpref_info_ptr->acq_uptime <= clk_uptime() ) */



  /* Check if "avoid_hybr_hdr_timer" expired.
  */
  if( cmpref_info_ptr->avoid_hybr_hdr_uptime <= clk_uptime() )
  {
    /* reset the expired timer.
    */
    CM_MSG_HIGH("Avoid hybr hdr timer expired", 0, 0, 0 );
    cmpref_info_ptr->avoid_hybr_hdr_uptime = CMPREF_UPTIME_NONE;

  } /* if( cmpref_info_ptr->avoid_hybr_hdr_uptime <= clk_uptime() ) */


} /* cmpref_proc_timer() */




/*===========================================================================
=============================================================================
=============================================================================
==================== MISC EXTERN FUNCTIONS ==================================
=============================================================================
=============================================================================
===========================================================================*/



/*===========================================================================

FUNCTION cmpref_proc_cmd_disable_hybr_hdr

DESCRIPTION
  This function should be called to disable the HDR acquisitions while in
  hybrid operation.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_proc_cmd_disable_hybr_hdr
(
        void
)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we are in hybrid operation, else ignore it.
  */
  if ( !cmpref_is_hybr_status_on() )
  {
    /* Not in hybrid operation.
    */
    CM_MSG_HIGH("Disabling hdr; not in hybrid %d",
                cmpref_info_ptr->state, 0, 0
               );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if HDR is already disabled.
  */
  if ( !sd_misc_is_mode_pref( cmpref_info_ptr->mode_pref_mask,
                              SD_SS_MODE_PREF_HDR))
  {
    /* HDR is already disabled.
    */
    CM_MSG_HIGH( "HDR already disabled %d",
                 cmpref_info_ptr->mode_pref_mask, 0, 0
               );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !sd_misc_is_mode_pref( cmpref_info_ptr->hybr_1_ss_pref.mode_pref,
                              SD_SS_MODE_PREF_HDR))
  {
    /* HDR is already disabled.
    */
    CM_MSG_HIGH( "HDR already disabled %d",
                 cmpref_info_ptr->mode_pref_mask, 0, 0
               );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the mode_pref mask.
  */
  cmpref_info_ptr->mode_pref_mask =
              sd_misc_remove_mode_pref( cmpref_info_ptr->mode_pref_mask,
                                        SD_SS_MODE_PREF_HDR
                                      );

  /* Update the hybr_1 mask.
  */
  cmpref_info_ptr->hybr_1_ss_pref.mode_pref =
              sd_misc_remove_mode_pref( cmpref_info_ptr->hybr_1_ss_pref.mode_pref,
                                        SD_SS_MODE_PREF_HDR
                                      );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform SS-HDR instance if not in HDR call.
  */
  if ( !cmcall_is_mobile_in_hdr_call() )
  {
    cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                   SD_SS_PREF_REAS_USER,
                                   SD_SS_ORIG_MODE_COLLOC,
                                   cmph_map_cm_mode_pref_to_sd_mode_pref( cmph_ptr()->mode_pref ),
                                   cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),
                                   cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                   cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                   SD_SS_HYBR_PREF_CDMA_HDR,
                                   SD_BAND_PCS,
                                   SD_BLKSYS_PCS_A,
                                   SD_SS_AVOID_SYS_IDLE,
                                   0,
                                   cmph_ptr()->plmn,
                                   cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                   cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                 );

  } /* if ( !cmcall_is_mobile_in_hdr_call() ) */
  else
  {
    /* Set HDR pref. sys chg pending.
    */
    cmpref_info_ptr->is_kick_hdr_pending = TRUE;

  }

} /* cmpref_proc_cmd_disable_hybr_hdr() */




/*===========================================================================

FUNCTION cmpref_proc_cmd_enable_hybr_hdr

DESCRIPTION
  This function should be called to enable the HDR acquisitions while in
  hybrid operation.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_proc_cmd_enable_hybr_hdr
(
        boolean                        send_pref_sys_cmd
          /* Flag to indicate if pref. sys changed command has to be
          ** sent.
          */
)
{

  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if HDR is already enabled.
  */
  if ( sd_misc_is_mode_pref( cmpref_info_ptr->mode_pref_mask,
                             SD_SS_MODE_PREF_HDR
                           )
     )
  {
    /* HDR is already enabled.
    */
    CM_MSG_HIGH( "HDR already enabling %d",
                 cmpref_info_ptr->mode_pref_mask, 0, 0
               );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the mode_pref mask.
  */
  cmpref_info_ptr->mode_pref_mask =
              sd_misc_add_mode_pref( cmpref_info_ptr->mode_pref_mask,
                                     SD_SS_MODE_PREF_HDR
                                   );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the hybr_1 mode_pref mask.
  */
  cmpref_info_ptr->hybr_1_ss_pref.mode_pref =
    sd_misc_add_mode_pref( cmpref_info_ptr->hybr_1_ss_pref.mode_pref,
                             SD_SS_MODE_PREF_HDR
                           );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we are in hybrid operation, else ignore it.
  */
  if ( !cmpref_is_hybr_status_on() )
  {
    /* Not in hybrid operation.
    */
    CM_MSG_HIGH("Enabling hdr; not in hybrid %d",
                cmpref_info_ptr->state, 0, 0
               );
    return;
  }



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform SS-HDR instance if requested for.
  */
  if ( !send_pref_sys_cmd )
  {
    CM_MSG_HIGH("Skip sending pref.sys chgd cmd", 0, 0, 0 );
    return;
  }


  if ( !cmcall_is_mobile_in_hdr_call() )
  {
    cmpref_send_pref_sys_chgd_cmd( SD_SS_HYBR_1,
                                   SD_SS_PREF_REAS_USER,
                                   SD_SS_ORIG_MODE_COLLOC,
                                   cmpref_info_ptr->hybr_1_ss_pref.mode_pref,
                                   cmph_map_cm_band_pref_to_sd_band_pref( cmph_ptr()->band_pref ),
                                   cmph_map_cm_prl_pref_to_sd_prl_pref( cmph_ptr()->prl_pref ),
                                   cmph_map_cm_roam_pref_to_sd_roam_pref( cmph_ptr()->roam_pref ),
                                   cmpref_info_ptr->hybr_1_ss_pref.hybr_pref,
                                   SD_BAND_PCS,
                                   SD_BLKSYS_PCS_A,
                                   SD_SS_AVOID_SYS_IDLE,
                                   0,
                                   cmph_ptr()->plmn,
                                   cmph_map_cm_srv_domain_pref_to_sd (cmph_ptr()->srv_domain_pref),
                                   cmph_map_cm_acq_order_pref_to_sd (cmph_ptr()->acq_order_pref)
                                 );
  } /* if ( !cmcall_is_mobile_in_hdr_call() ) */
  else
  {
    /* Set HDR pref. sys chg pending.
    */
    cmpref_info_ptr->is_kick_hdr_pending = TRUE;

  }

} /* cmpref_proc_cmd_enable_hybr_hdr() */


/*===========================================================================

FUNCTION cmpref_proc_cmd_avoid_hybr_hdr_orig

DESCRIPTION
  This function should be called to avoid hybr hdr for sometime.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_proc_cmd_avoid_hybr_hdr_orig
(
        void
)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we are in hybrid operation, else ignore it.
  */
  if ( !cmpref_is_hybr_status_on() )
  {
    /* Not in hybrid operation.
    */
    CM_MSG_HIGH("Avoiding hybr hdr; not in hybrid %d",
                cmpref_info_ptr->state, 0, 0
               );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start "avoid_hybr_hdr_timer" if not running.
  */
  if ( cmpref_info_ptr->avoid_hybr_hdr_uptime == CMPREF_UPTIME_NONE )
  {
     CM_MSG_HIGH("Avoiding hybr hdr", 0, 0, 0 );
     cmpref_info_ptr->avoid_hybr_hdr_uptime = clk_uptime() +
                                                CMPREF_AVOID_HYBR_HDR_TIMER;
  }


} /* cmpref_proc_cmd_avoid_hybr_hdr */




/*===========================================================================

FUNCTION cmpref_is_hybr_status_on

DESCRIPTION
  This function should be called to determine if hybrid is ON.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  boolean                         cmpref_is_hybr_status_on
(
        void
)
{
  cmss_s_type *ss_ptr = cmss_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( BOOLEAN( ss_ptr->info.hdr_hybrid ) );

} /* cmpref_is_hybr_status_on() */

/*===========================================================================

FUNCTION cmpref_is_gw_hybr

DESCRIPTION
  This function should be called to determine if hybrid GW is ON.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  boolean                         cmpref_is_gw_hybr()
{
  cmph_s_type *ph_ptr = cmph_ptr();

  CM_ASSERT( ph_ptr != NULL );

  return ( ph_ptr->int_hybr_pref == SD_SS_HYBR_PREF_CDMA__WCDMA ||
           ph_ptr->int_hybr_pref == SD_SS_HYBR_PREF_CDMA__HDR_WCDMA );

}

/*===========================================================================

FUNCTION cmpref_is_hybr_hdr_call_allowed

DESCRIPTION
  This function should be called to determine if hybrid HDR call is allowed.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  boolean                         cmpref_is_hybr_hdr_call_allowed
(
        void
)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we are in avoid hybrid HDR period
  */
  if ( cmpref_info_ptr->avoid_hybr_hdr_uptime != CMPREF_UPTIME_NONE )
  {
    /* we are in avoid hybrid HDR period.
    */
    return FALSE;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if HDR is masked out.
  */
  if ( !sd_misc_is_mode_pref( cmpref_info_ptr->mode_pref_mask,
                              SD_SS_MODE_PREF_HDR
                            )
     )
  {
    /* HDR is masked out, so hybrid HDR call is not allowed.
    */
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we have HDR service, originate with HDR.
  */
  if ( sys_srv_status_is_srv( cmss_ptr()->info.hdr_srv_status ) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check hybrid state.
  */
  switch( cmpref_info_ptr->state )
  {
    case CMPREF_STATE_HY_ON_SS_MAIN:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV:
      return TRUE;

    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR:
      if ( sys_srv_status_is_srv(cmss_ptr()->main_srv_status) &&
           !cmss_ptr()->is_colloc )
      {
        return FALSE;
      }
      break;

    case CMPREF_STATE_NULL:
    case CMPREF_STATE_HY_OFF_SS_MAIN:
    case CMPREF_STATE_HY_OFF_SS_MAIN_SRV:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_MAX:
    default:
      return FALSE;

  } /* switch( cmpref_info_ptr->state ) */

  return TRUE;

} /* cmpref_is_hybr_hdr_call_allowed() */


/*===========================================================================

FUNCTION cmpref_is_hybr_gw_call_allowed

DESCRIPTION
  This function should be called to determine if hybrid GW call is allowed.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  boolean                         cmpref_is_hybr_gw_call_allowed
(
        void
)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if hybr_gw is allowed
  */
  if( cmpref_info_ptr->hybr_1_ss_pref.hybr_pref == SD_SS_HYBR_PREF_NONE ||
    cmpref_info_ptr->hybr_1_ss_pref.hybr_pref == SD_SS_HYBR_PREF_CDMA__HDR )
  {
    return FALSE;
  }

  /* Check if we are in avoid hybrid GW period
  */
  if ( cmpref_info_ptr->avoid_hybr_hdr_uptime != CMPREF_UPTIME_NONE )
  {
    /* we are in avoid hybrid HDR period.
    */
    return FALSE;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if HDR is masked out.
  */
  if ( !sd_misc_is_mode_pref( cmpref_info_ptr->mode_pref_mask,
                              SD_SS_MODE_PREF_GW
                            )
     )
  {
    /* HDR is masked out, so hybrid HDR call is not allowed.
    */
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we have GW service, originate with GW.
  */
  #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
  if ( sys_srv_status_is_srv( cmss_ptr()->info.gw_srv_status ))
  {
    return TRUE;
  }

  if( sys_srv_status_is_srv( cmss_ptr()->info.srv_status ))
  {
    return FALSE;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check hybrid state.
  */
  switch( cmpref_info_ptr->state )
  {
    case CMPREF_STATE_HY_ON_SS_MAIN:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_UMTS_SRV:
      return TRUE;

    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_UMTS:
      if ( sys_srv_status_is_srv(cmss_ptr()->info.srv_status) &&
           !cmss_ptr()->is_colloc )
      {
        return FALSE;
      }
      break;

    case CMPREF_STATE_NULL:
    case CMPREF_STATE_HY_OFF_SS_MAIN:
    case CMPREF_STATE_HY_OFF_SS_MAIN_SRV:
    case CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO:
    case CMPREF_STATE_MAX:
    default:
      return FALSE;

  } /* switch( cmpref_info_ptr->state ) */

  return TRUE;

} /* cmpref_is_hybr_gw_call_allowed() */


/*===========================================================================
=============================================================================
=============================================================================
==================== INIT FUNCTIONS =========================================
=============================================================================
=============================================================================
===========================================================================*/



/*===========================================================================

FUNCTION cmpref_init

DESCRIPTION
  This function should be called to initialize the PREF module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_init
(
        void

)
{
  cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

  cmph_s_type      *ph_ptr = cmph_ptr();

  boolean          is_curr_nam_valid = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate nam */
  if(!INRANGE(ph_ptr->curr_nam, 0, CM_NAM_AUTO))
  {
    CM_ERR( "Invalid nam:%d", ph_ptr->curr_nam,0,0);
    is_curr_nam_valid = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize CMPREF state variables.
  */
  cmpref_update_state( CMPREF_STATE_NULL );
  cmpref_info_ptr->lost_main_uptime    = CMPREF_UPTIME_NONE;
  cmpref_info_ptr->acq_hybr_1_uptime      = CMPREF_UPTIME_NONE;
  cmpref_info_ptr->avoid_hybr_hdr_uptime = CMPREF_UPTIME_NONE;
  cmpref_info_ptr->mode_pref_mask      = SD_SS_MODE_PREF_ALL_MODES;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize SS preferences towards hybrid operation
  */
  cmpref_info_ptr->hybr_1_ss_pref.orig_mode = SD_SS_ORIG_MODE_NONE;
  cmpref_info_ptr->hybr_1_ss_pref.mode_pref = 
                    cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->mode_pref);

  if(is_curr_nam_valid)
  {
    cmpref_info_ptr->hybr_1_ss_pref.band_pref = 
      cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[ph_ptr->curr_nam]);
    cmpref_info_ptr->hybr_1_ss_pref.prl_pref  =  
      cmph_map_cm_prl_pref_to_sd_prl_pref( ph_ptr->prst_prl_pref[ph_ptr->curr_nam]);
    cmpref_info_ptr->hybr_1_ss_pref.roam_pref =
      cmph_map_cm_roam_pref_to_sd_roam_pref( ph_ptr->prst_roam_pref[ph_ptr->curr_nam]);
  }
  else
  {
      cmpref_info_ptr->hybr_1_ss_pref.band_pref = 
        cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->band_pref);
      cmpref_info_ptr->hybr_1_ss_pref.prl_pref  =  
        cmph_map_cm_prl_pref_to_sd_prl_pref( ph_ptr->prl_pref);
      cmpref_info_ptr->hybr_1_ss_pref.roam_pref =
        cmph_map_cm_roam_pref_to_sd_roam_pref( ph_ptr->roam_pref);
  }

  cmpref_info_ptr->hybr_1_ss_pref.hybr_pref = ph_ptr->int_hybr_pref;
  cmpref_info_ptr->hybr_1_ss_pref.otasp_band = SD_BAND_BC0 ;
  cmpref_info_ptr->hybr_1_ss_pref.otasp_blksys = SD_BLKSYS_CELL_A;
  cmpref_info_ptr->hybr_1_ss_pref.avoid_type = SD_SS_AVOID_SYS_IDLE;
  cmpref_info_ptr->hybr_1_ss_pref.avoid_time = 0;


  /* Manual Selection of IS856 is not defined.
  ** We are initializing the structure with a invalid one to indicate UMTS.
  */
  cmpref_info_ptr->wlan_ss_pref.sys_id.sys_id.id_type =  SYS_SYS_ID_TYPE_UMTS;
  sys_plmn_undefine_plmn_id( &cmpref_info_ptr->hybr_1_ss_pref.sys_id.sys_id.id.plmn );

  cmpref_info_ptr->hybr_1_ss_pref.domain_pref = SD_SS_SRV_DOMAIN_PREF_PS_ONLY;
  cmpref_info_ptr->hybr_1_ss_pref.acq_order_pref = SD_SS_ACQ_ORDER_PREF_AUTO;

  cmpref_info_ptr->hybr_1_ss_pref.wlan_tech_pref = SYS_TECH_MASK_EMPTY;
  cmpref_info_ptr->hybr_1_ss_pref.wlan_scan_pref = SYS_WLAN_SCAN_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize SS preferences towards SS-WLAN.
  */
  cmpref_info_ptr->wlan_ss_pref.orig_mode = SD_SS_ORIG_MODE_NONE;
  cmpref_info_ptr->wlan_ss_pref.mode_pref = SD_SS_MODE_PREF_WLAN;
  if(is_curr_nam_valid)
  {
    cmpref_info_ptr->wlan_ss_pref.band_pref =
            cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[ph_ptr->curr_nam]);

    cmpref_info_ptr->wlan_ss_pref.prl_pref  =  
      cmph_map_cm_prl_pref_to_sd_prl_pref( ph_ptr->prst_prl_pref[ph_ptr->curr_nam]);
    cmpref_info_ptr->wlan_ss_pref.roam_pref =
      cmph_map_cm_roam_pref_to_sd_roam_pref( ph_ptr->prst_roam_pref[ph_ptr->curr_nam]);
  }
  else
  {
      cmpref_info_ptr->wlan_ss_pref.band_pref = 
        cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->band_pref);
      cmpref_info_ptr->wlan_ss_pref.prl_pref  =  
        cmph_map_cm_prl_pref_to_sd_prl_pref( ph_ptr->prl_pref);
      cmpref_info_ptr->wlan_ss_pref.roam_pref =
        cmph_map_cm_roam_pref_to_sd_roam_pref( ph_ptr->roam_pref);
  }
  cmpref_info_ptr->wlan_ss_pref.hybr_pref = SD_SS_HYBR_PREF_NONE;
  cmpref_info_ptr->wlan_ss_pref.otasp_band = SD_BAND_BC0 ;
  cmpref_info_ptr->wlan_ss_pref.otasp_blksys = SD_BLKSYS_CELL_A;
  cmpref_info_ptr->wlan_ss_pref.avoid_type = SD_SS_AVOID_SYS_IDLE;
  cmpref_info_ptr->wlan_ss_pref.avoid_time = 0;


  cmpref_info_ptr->wlan_ss_pref.sys_id.sys_id.id_type     =  SYS_SYS_ID_TYPE_WLAN;
  cmpref_info_ptr->wlan_ss_pref.sys_id.sys_id.id.ssid.len = 0;

  cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.bss_type = SYS_WLAN_BSS_TYPE_ANY;
  cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.bssid = 0;
  cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.channel = 0;
  cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.profile_id = 0;

  cmpref_info_ptr->wlan_ss_pref.domain_pref = SD_SS_SRV_DOMAIN_PREF_PS_ONLY;
  cmpref_info_ptr->wlan_ss_pref.acq_order_pref = SD_SS_ACQ_ORDER_PREF_AUTO;
  cmpref_info_ptr->wlan_ss_pref.wlan_tech_pref = SYS_TECH_PREF_MASK_ANY;
  cmpref_info_ptr->wlan_ss_pref.wlan_scan_pref = SYS_WLAN_SCAN_PREF_AUTO;
  cmpref_info_ptr->wlan_ss_pref.wlan_bss_type_pref = SYS_WLAN_BSS_TYPE_ANY;

} /* cmpref_init() */


/*===========================================================================

FUNCTION cmpref_set_wlan_pref

DESCRIPTION
  This function should sets CMPREF's WLAN preference as per the input
  wlan preferences.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           cmpref_set_wlan_pref
(
        const cm_wlan_pref_s_type            *wlan_pref_ptr
)
{
    cmpref_info_s_type *cmpref_info_ptr = cmpref_get_info_ptr();

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_ASSERT( wlan_pref_ptr   != NULL);
    CM_ASSERT( cmpref_info_ptr != NULL);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmpref_info_ptr->wlan_ss_pref.wlan_tech_pref =
                                  (sd_ss_tech_pref_e_type)wlan_pref_ptr->tech_pref;

    cmpref_info_ptr->wlan_ss_pref.wlan_scan_pref =
                 cmph_map_cm_wlan_scan_pref_to_sys_pref(wlan_pref_ptr->scan_mode);

    if(wlan_pref_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
    {
      cmpref_info_ptr->wlan_ss_pref.orig_mode = SD_SS_ORIG_MODE_MANUAL_IDLE;

      // DEEPAK- Find how Manual Band Pref will be sent to SD?
      // cmpref_info_ptr->wlan_ss_pref.band_pref |= wlan_pref_ptr->band;

      /* Copy WLAN Profile.
      */

      cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.profile.profile_size =
                                                   SYS_WLAN_PROFILE_MAX_SIZE;

      #ifdef FEATURE_WLAN_ADHOC
#error code not present
      #endif

      #ifdef FEATURE_WLAN
#error code not present
      #endif
    }
    else
    {
      cmpref_info_ptr->wlan_ss_pref.orig_mode = SD_SS_ORIG_MODE_NONE;

      #ifdef FEATURE_WLAN_ADHOC
#error code not present
      #endif
    }

    cmpref_info_ptr->wlan_ss_pref.sys_id.sys_id.id_type =  SYS_SYS_ID_TYPE_WLAN;
    cmpref_info_ptr->wlan_ss_pref.sys_id.sys_id.id.ssid =  wlan_pref_ptr->ssid;

    cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.bssid    =  wlan_pref_ptr->bss_id;
    cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.channel  =  wlan_pref_ptr->chan;
    #ifdef FEATURE_WLAN_4_0
#error code not present
    #else
    /* WLAN CP ignores band info. Just to make sure SD doesn't reject the
    ** manual system. Some UI code base didn't set this value.
    */
    cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.band =  SYS_BAND_MASK_WLAN_US_2400;
    #endif
    cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.profile_id = wlan_pref_ptr->profile_id;
    cmpref_info_ptr->wlan_ss_pref.sys_id.prm.wlan.bss_type = wlan_pref_ptr->bss_type;

}

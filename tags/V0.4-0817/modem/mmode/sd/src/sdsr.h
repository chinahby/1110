#ifndef SDSR_H
#define SDSR_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

          S Y S T E M   R E C O R D   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with SDSR.C (i.e. the System Record component of the SD).

Copyright (c) 2000 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sdsr.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
10/02/08   aj      Bandclass support for BC17, 18, 19
09/15/08   pk      Added support for WLAN UOoS
03/17/08   vs      Added sdsr_list_sorted_is_same, to sort and compare lists.
09/17/07   pk      Added support for non-Hybrid UOOS
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
06/06/07   pk      UOOS enhancements...
05/14/07   sk      Ported back UOOS changes for Patch C.
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS 
                   builds
11/15/06   pk      Added support for HYBR_GW
11/04/06   sk      Memory reduction changes
                   - Reordered fields in sdsr_s_type to reduce size.
09/22/06   rp      Added BC 15, BC 16 support 
09/08/06   sk      Added WLAN tech mask to sdsr_s_type.
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Added new system GW_LAST_FULL_SYS.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/17/06   jqi     Added ss information in sdsr table to differenciate the 
                   avoidance over different SD instances.
04/28/06   sk      Added new list PWR_SAVE_LST.
04/17/05   jqi     Added EGSRDM support.
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Do not change sd state if only srv domain is changed.
01/09/06   sk      Added sdsr_comp2().
12/02/05   jqi     Lint 8 fixes.
10/14/05   jqi     Added BC14 support.
09/10/05   pk      Added sdsr_list_del_list_cnt().
08/18/05   jqi     Realign sdsr_s_type data structure.
08/15/05   pk      Added SDSR_PREV_COLLOC_LST to SDSR_LISTS
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/11/05   jqi     Added avoidance timer for access failures and fades.
01/30/05   sj      Added support for WLAN.
01/27/05   ka      Added support for Band class 10, 11, 12
01/10/05   dk      Added sdsr_is_same_gw_lac().
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
10/21/04   RC      Added SDSR_HDR_DFLT_LST.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
07/14/04   jqi     Update GW last user system upon acquired limited system.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/16/04   RC      Fixed an issue where sdprl_parser_sys_is_more_pref()
                   was not taking into account the PRL match level.
06/07/04   jqi     More quad-band enhancement for automatic mode.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
04/19/04   jqi     Lint fix - for error level - error.
03/18/04   jqi     lint fix.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
11/06/03   RC      Optimized the side of sdsr_s_type.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
07/15/03   JQ      Added Extended PRL Support.
                   - D PRL
06/09/03   RC      Added a len parameter to sdsr_is_same_is856_sid().
05/27/03   RC      Added to sdsr_list_store_and_app2().
03/04/03   SJ      Added new list GW_LST for limited automatic mode support.
01/20/03   jqi     Added support for band class 6.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
10/15/02   RC      Added prl_assn filed to sdsr_tbl_storexxx() functions.
05/17/02   RC      Added support for IS-683C, PN and data association fields.
12/12/01   RC      Added support CDMA-HDR hybrid operation.
12/07/01   jqi     Added support for band class 5.
10/30/01   RC      Added support for IS-683C collcated systems.
07/20/01   RC      Added support for IS-683C.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
02/01/01   RC      Changes per 8th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/04/00   RC      Changes per 2nd week of I&T.
11/27/00   RC      Changes per 1st week of I&T.
11/17/00   RC      Initial release.

===========================================================================*/


#include "sd.h"         /* External interface to SD */

#include "comdef.h"     /* Definition for basic types and macros */

#include "sdi.h"        /* Internal interface to SD */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of PRL associations.
*/
typedef enum {

  SDSR_PRL_ASSN_NONE          = 0,
                                /* No association */

  SDSR_PRL_ASSN_TAG           = BM( 0 ),
                                /* Tag association */

  SDSR_PRL_ASSN_PN            = BM( 1 ),
                                /* PN association */

  SDSR_PRL_ASSN_DATA          = BM( 2 ),
                                /* Data association */

  SDSR_PRL_ASSN_ALL           = 0xFF,
                                /* All associations */
  SDSR_PRL_ASSN_MAX

} sd_sr_prl_assn_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macros to set the PRL association bits on/off.
*/
#define SDSR_PRL_ASSN_ON( assn_mask, assn_field ) \
                ( assn_mask = (sd_sr_prl_assn_e_type) \
                              ( (int)assn_mask | (int)(assn_field) ) \
                )

#define SDSR_PRL_ASSN_OFF( assn_mask, assn_field ) \
                ( assn_mask = (sd_sr_prl_assn_e_type) \
                              ( (int)assn_mask & ~(int)(assn_field) ) \
                )

/* Macros to get the PRL association bits.
*/
#define SDSR_PRL_ASSN_GET( assn_mask, assn_field ) \
                BOOLEAN(( (int)assn_mask & (int)assn_field ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type define for sdsr status mask
*/
typedef byte       sdsr_status_mask_e_type;

#define SDSR_STATUS_MASK_NONE       ((sdsr_status_mask_e_type)0)
     /* Internal use only */

#define SDSR_STATUS_MASK_DEL_TMSI   ((sdsr_status_mask_e_type)BM(0))
     /* Delete tmsi bit mask */ 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macros to set the status bit mask on/off.
*/
#define SDSR_STATUS_MASK_ON( bit_mask, bit ) \
                ( bit_mask = (bit_mask | bit) )

#define SDSR_STATUS_MASK_OFF( bit_mask, bit ) \
                ( bit_mask = (bit_mask & ~(bit)) )

#define SDSR_STATUS_MASK_SET( bit_mask, bit, on ) \
  ( (on == TRUE) ? SDSR_STATUS_MASK_ON( bit_mask, bit ) : \
                   SDSR_STATUS_MASK_OFF( bit_mask, bit ) \
  )

/* Macros to get the bit mask.
*/
#define SDSR_STATUS_MASK_GET( value, bit ) BOOLEAN( value & bit )


/*---------------------------------------------------------------------------
                                System Record
---------------------------------------------------------------------------*/

/* Type for a system record.
**
** The system record records all the relevant information that is known
** about a particular system.
**
** Note! The order of fields in this structure is important in order to
** optimize the structure size (i.e. eliminate compiler padding) as this
** structure is used in the System Record Table (sdsr_tbl). Add new fields
** only at the end and try to order fields smartly to minimize compiler
** padding.
** JAI :- OFFSETS INCORRECT.
*/
typedef struct {

  /* The system (i.e., mode, band and channel).
  */
  sd_sys_s_type       sys;

  /* The PRL designation (as per sdss_prl_e_type) that is associated with
  ** this system.
  */
  byte                prl;

  /* The PRL match level (as per sdprl_match_sid_nid_e_type) that is
  ** associated with this system.
  */
  byte                match_lvl;

  /* The value of the new-acquisition counter when this system was last
  ** attempted acquisition.
  */
  byte                new_acq_cnt;

  /* The list position of this system record - being used for stable sorting
  ** of system record lists.
  */
  byte                list_pos;

  /*- - - - - - - - - - - - - - offset 16 - - - - - - - - - - -  - - - - - -*/

  /* The System ID that was obtained for this system during acquisition
  ** and/or operation. If not known, set to wildcard (i.e.,
  ** SID=SD_WILDCARD_SID and NID=SD_WILDCARD_NID for CDMA/AMPS or all 0's for
  ** HDR).
  */
  sd_sid2_u_type      sid;

  /*- - - - - - - - - - - - -  offset 32 - - - - - - - - - - -  - - - - - -*/

  /* The uptime when the last SS-Event was received.
  */
  dword               event_time;

  /* The last SS-Event (as defined by sdss_evt_e_type) to be received for
  ** this system .
  */
  word                event;

  /*- - - - - - - - - - - - -  offset 38 - - - - - - - - - - -  - - - - - -*/

  /* The roaming designation (as per sd_roam_e_type) that is associated with
  ** this system.
  */
  sys_roam_status_e_type   roam;

  /*- - - - - - - - - - - - -  offset 40 - - - - - - - - - - -  - - - - - -*/

  /* The measurement value from the last measurement report in units of -1dB
  ** for Rx-power and -0.5dB for Pilot strength. -255 implies that no
  ** measurement was obtained.
  */
  int2                meas_val;

  /* A schedule (as defined by sdssscr_sched_e_type) indicating when this
  ** system should be revisited for re-acquisition attempts or until what
  ** time this system should be avoided from any acquisition attempts, as
  ** well as, the acquisition type (as defined by sdssscr_acq_e_type) to be
  ** attempted.
  ** When inactive, set to SDSSSCR_SCHED_MAX .
  */
  word                sched;

  /*- - - - - - - - - - - - -  offset 44 - - - - - - - - - - -  - - - - - -*/

  union {

    struct {
      word  time_cnt;
      byte  type;
      byte  mode;
    }             acq;

    dword         avoid_time;

  }                   sched_prm;


  /*- - - - - - - - - - - - -  offset 48 - - - - - - - - - - -  - - - - - -*/

  /* Indicate which instance the avoidance is set to
  */
  sd_ss_e_type          sched_ss;

  /* Consolidate the boolean type status information into a bit mask.
  */
  sdsr_status_mask_e_type  status_mask;    //status mask

  /* Indicate whether the PRL association of this system.
  */
  sd_sr_prl_assn_e_type   prl_assn;

  /*- - - - - - - - - - - - -  offset 52 - - - - - - - - - - -  - - - - - -*/

  dword               uptime_fade;

  /* The counter for the fades within SD_MAX_TIME_DURATION_BETWEEN_FADES
  */
  byte                num_fade;

  /* System's service capability ( as per sys_srv_capability_e_type ).
  */
  sys_srv_capability_e_type  srv_cap;

  /*- - - - - - - - - - - - -  offset 58 - - - - - - - - - - -  - - - - - -*/

  /* The expected PRL's system table index match for this system.
  */
  word                idx_expected;

  /* The expected SID/NID for this system. If not known, SID=SD_WILDCARD_SID
  ** and NID=SD_WILDCARD_NID.
  */
  sd_sid_plmn_wlan_s_type  sid_expected;

  /*- - - - - - - - - - - - -  offset 108 - - - - - - - - - - -  - - - - - -*/

  /* WLAN technology mask */
  sys_tech_mask_type    wlan_tech_mask;

  /*- - - - - - - - - - - - -  offset 112 - - - - - - - - - - -  - - - - - -*/

  /* Bss type - adhoc or infra. Applicable for WLAN only.
  ** type (sys_wlan_bss_e_type )
  */
  sys_wlan_bss_e_type   wlan_bss_type;

  /* Profile to use for acquiring.
  */
  byte                  nw_profile_id;

  /*- - - - - - - - - - - - -  offset 114 - - - - - - - - - - -  - - - - - -*/


} sdsr_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for a system record reference - i.e. indicate the location of
** a system record in the system record table.
*/
typedef   byte    sdsr_ref_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Definition for the NULL System Record reference (i.e., a system record
** reference that is resolved to no system record at all).
*/
#define   SDSR_REF_NULL  ((sdsr_ref_type) (-1))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Definition for the NULL System Record list position (i.e., a system record
** list position that is resolved to no list position at all).
*/
#define   SDSR_POS_NULL   ((int) (-1))


/*---------------------------------------------------------------------------
                             System Record List
---------------------------------------------------------------------------*/

/* System record lists are ordered subsets of the system record table. In
** other words, each such list is a list of references to system record table
** entries. A system record item is a system record list of length one.
**
** Note that a system record list shall not contain duplicate elements; that
** is, no two elements on the same list shall reference the same system
** record table entry.
**
** In addition each system record list is of the same length as the system
** record table; or put differently, each system record list is capable of
** referencing all the systems that are stored in the system record table.
*/

// JAI RENUMBER.
/* Enumeration of system record lists and items.
*/
typedef enum {

  /* --------------------
  ** System record lists.
  ** --------------------
  */
                      /* 0 */
  SDSR_PRL_LST,       /* A list of references to system records that
                      ** represent the PRL's acquisition table
                      **
                      ** Initialized: Per the selected NAM acquisition table
                      ** at power-up or when the NAM selection/information
                      ** is modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never.
                      **
                      ** Order: Same as the PRL's acquisition table order */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_MRU_LST,       /* A list of references to system records that
                      ** represent the MRU table
                      **
                      ** Initialized: Per the selected NAM NV MRU table at
                      ** power-up or when the NAM selection/information is
                      ** modified.
                      **
                      ** Updated: When receiving SD_SS_IND_CDMA_OPR_OH_INFO
                      ** or SD_SS_IND_AMPS_OPR_OH_INFO and the serving
                      ** system is a preferred PRL system.
                      **
                      ** Cleared: Never. The least recently used system is
                      ** automatically deleted from the list when a new
                      ** system is added (and the list reached its
                      ** predetermined max size).
                      **
                      ** Order: Most recently used to least recently used */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AVAIL_LST,     /* A list of references to system records that
                      ** represent the Available List. The available list
                      ** contains available systems that were found since
                      ** the last time the AVAIL_LST was cleared.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SD_SS_IND_CDMA_ACQ_SCHM or
                      ** SD_SS_IND_AMPS_ACQ_CCH_SID and the acquired system
                      ** is found to be an available system.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Least recently acquired to most recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_GEO_LST,   /* A list of references to system records that
                      ** represent the GEO of the acquired system
                      ** (SDSR_ACQ_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_ACQ_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_ACQ_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 4 */
  SDSR_IDL_GEO_LST,   /* A list of references to system records that
                      ** represent the GEO of the idle system
                      ** (SDSR_IDL_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_IDL_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_IDL_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SRV_GEO_LST,   /* A list of references to system records that
                      ** represent the GEO of the serving system
                      ** (SDSR_SRV_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_SRV_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_HO_GEO_LST,    /* A list of references to system records that
                      ** represent the GEO of the handoff system
                      ** (SDSR_HO_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_HO_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_HO_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_PREF_LST,  /* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the acquired system (SDSR_ACQ_SYS). If the
                      ** acquired system is a PRL system, this list is a
                      ** subset of the SDSR_ACQ_GEO_LST list. Otherwise (if
                      ** the acquired system is an available system) this
                      ** list is a subset of the SDSR_PRL_LST list.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: If acquired system is a PRL system, same as
                      ** the order of systems in the GEO of the SDSR_ACQ_SYS
                      ** system. If acquired system is not a PRL system,
                      ** same as the order of systems in SDSR_PRL_LST list */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 8 */
  SDSR_IDL_PREF_LST,  /* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the idle system (SDSR_IDL_SYS). If the
                      ** idle system is a PRL system, this list is a
                      ** subset of the SDSR_IDL_GEO_LST list. Otherwise (if
                      ** the idle system is an available system) this list
                      ** is a subset of the SDSR_PRL_LST list.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_IDL_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: If idle system is a PRL system, same as
                      ** the order of systems in the GEO of the SDSR_IDL_SYS
                      ** system. If idle system is not a PRL system,
                      ** same as the order of systems in SDSR_PRL_LST list */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SRV_PREF_LST,  /* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the serving system (SDSR_SRV_SYS). If the
                      ** serving system is a PRL system, this list is a
                      ** subset of the SDSR_SRV_GEO_LST list. Otherwise (if
                      ** the serving system is an available system) this list
                      ** is a subset of the SDSR_PRL_LST list.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: If serving system is a PRL system, same as
                      ** the order of systems in the GEO of the SDSR_SRV_SYS
                      ** system. If serving system is not a PRL system,
                      ** same as the order of systems in SDSR_PRL_LST list */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_HO_PREF_LST,   /* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the handoff system (SDSR_HO_SYS). If the
                      ** handoff system is a PRL system, this list is a
                      ** subset of the SDSR_HO_GEO_LST list. Otherwise (if
                      ** the handoff system is an available system) this
                      ** list is a subset of the SDSR_PRL_LST list.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_HO_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: If handoff system is a PRL system, same as
                      ** the order of systems in the GEO of the
                      ** SDSR_HO_SYS system. If handoff system is not a
                      ** PRL system, same as the order of systems in
                      ** SDSR_PRL_LST list */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_START_PREF_LST,/* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the SDSR_START_SYS). This list is a subset of
                      ** the GEO of the SDSR_START_SYS.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_START_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_START_SYS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 12 */
  SDSR_REDIR_LST,     /* A list of references to system records that
                      ** correspond to the systems that were included in the
                      ** last Redirection indication to be received from the
                      ** SS-Client. When this list is empty Redirection is
                      ** off. Else, redirection is on.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_REDIR.
                      **
                      ** Cleared: Should be cleared by the SS-Script to turn
                      ** redirection OFF.
                      **
                      ** Order: Same as the order of systems in the last
                      ** redirection indication to be received */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SYS_RESEL_LST, /* A list of references to system records that
                      ** correspond to the systems that were included in the
                      ** last System Re-selection indication to be received
                      ** from SS-Client.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_SYS_RESEL.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the last
                      ** system re-selection indication to be received */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_OTASP_LST,     /* A list of references to system records that
                      ** correspond to the set of preferred systems that is
                      ** associated with the last OTASP activation code to be
                      ** received from SS-Client.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_USER_SS_PREF and
                      ** orig_mode = ORIG_MODE_OTASP.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Increasing CDMA channel number order */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SRCH_LST,      /* A list of references to system records that
                      ** represent the Search List. The search list is a
                      ** kind of scratch list that is used to merge two or
                      ** more lists into a single list, which can then be
                      ** sorted and/or attempted acquisition.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: By SS-Script.
                      **
                      ** Cleared: By the SS-Script.
                      **
                      ** Order: Determined by the SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 16 */
  SDSR_SCHED_LST,     /* A list of references to system records that
                      ** represent all the systems that are on a
                      ** re-acquisition schedule.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: By the SS-Script. Specifically, when the
                      ** SS-Script put a system on a re-acquisition schedule
                      ** the system is inserted into this list.
                      **
                      ** Cleared: By the SS-Script.
                      **
                      ** Order: Most recent system to be put on a schedule
                      ** to least recent */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_MEAS_LST,      /* A list of references to system records that
                      ** represent the systems most recently being submitted
                      ** to Search for a channel strength measurement.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: By the SS-Script. Specifically, when the
                      ** SS-Script is calling for a channel strength
                      ** measurement.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same order as systems most recently being
                      ** submitted to Search for a channel strength
                      ** measurement */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_REDIAL_LST,    /* A list of references to system records that
                      ** represent the systems that are on the silent redial
                      ** acquisition list.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: By the SS-Script. Specifically, when the
                      ** SS-Script is calling for silent redial acquisition.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Determined by the SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_EMERG_LST,     /* A list of references to system records that
                      ** represent the systems that are on the emergency
                      ** acquisition list.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: By the SS-Script. Specifically, when the
                      ** SS-Script is calling for emergency acquisition.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Determined by the SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 20 */
  SDSR_TBL_LST,       /* A list of references to system records that
                      ** represent the entire system record table
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever a new system is added to the
                      ** system record table.
                      **
                      ** Cleared: Never.
                      **
                      ** Order: Same as the system record table */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQED_LST,     /* A list of references to system records that
                      ** correspond to the set of all system that were
                      ** acquired since power-up.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: Whenever a system is acquired.
                      **
                      ** Cleared: Never.
                      **
                      ** Order: Most recently acquired to least recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ALL_PREF_LST,  /* A list of references to system records that
                      ** correspond to the set of all preferred system.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: Whenever a system is acquired.
                      **
                      ** Cleared: Never.
                      **
                      ** Order: Most recently acquired to least recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_COLLOC_LST,/* A list of references to system records that
                      ** represent the collocated systems (see IS-683C) of
                      ** the serving system (SDSR_ACQ_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_ACQ_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_ACQ_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 24 */
  SDSR_SRV_COLLOC_LST,/* A list of references to system records that
                      ** represent the collocated systems (see IS-683C) of
                      ** the serving system (SDSR_SRV_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_SRV_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_GW_LST,        /* A list of references to system records that
                      ** that have GSM and/or WCDMA systems.
                      ** Initialized: To an GSM system + WCDMA plus upon
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: None.
                      **
                      ** Order:
                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_MANUAL_LST,    /* A list of dummy items to store the information of
                      ** the system which the user requested to do manual
                      ** acquisition.
                      **
                      ** Initialized: to GSM record with CHAN 1 on nam_sel.
                      **
                      ** Updated: when user_ss_pref is received with
                      ** orig_mode = manual.
                      **
                      ** Cleared: never.
                      **
                      ** Order: Most recently acquired to least recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_GW_LTD_LST,    /* A list of references to a system record that
                      ** corresponds to the GSM/WCDMA limited service mode.
                      ** Note that if MS is not capable of GSM or WCDMA mode,
                      ** this item is empty.
                      **
                      ** Initialized: To the GSM+WCDMA (limited service)
                      ** system at power-up.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never
                      **
                      ** Order: Most recently acquired to least recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 28 */
  SDSR_HDR_DFLT_LST,  /* A list of references to default HDR system records
                      ** to be used when FEATURE_ADD_HDR_IF_IS683A_PRL is
                      ** defined and the loaded PRL is IS-683A/B
                      ** (SSPR_P_REV=1)
                      **
                      ** Initialized: To default HDR systems when
                      ** sdprl_nam_sel() is called (i.e. subscription
                      ** information is loaded.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: None.
                      **
                      ** Order: Desired order of default HDR systems.
                      */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_TEMP_LST,     /* Temporary list used for manipulation.
                      **
                      ** Initialized: By scripts
                      **
                      ** Updated: By scripts
                      **
                      ** Cleared: By scripts.
                      **
                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_WLAN_PRIV_LST, /* A list of references to a system record that
                    ** corresponds to the WLAN systems configured by user.
                    ** TO be used while in automatic selection.
                    ** Note that if MS is not capable of WLAN mode,
                    ** this item is empty.
                    **
                    ** Initialized: Read from NV at power-up.
                    **
                    ** Updated: Never.
                    **
                    ** Cleared: Never
                    **
                    ** Order: Most recently acquired to least recently
                    ** acquired */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_PREV_COLLOC_LST,/* A list of references to system records that
                      ** represent the collocated systems (see IS-683C) of
                      ** the serving system (SDSR_SRV_SYS).
                      ** 
                      ** Previous collocated list used for manipulation.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified. 
                      **
                      ** Updated: Whenever the collocated list changes
                      **
                      ** Cleared: May be cleared by ss-scripts
                      **
                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 32 */
  SDSR_ACQ_CHAN_LST,  /* A list of reference to system records that represent
                      ** the hashing channels that exist in PRL.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever the HDR system ID(i.e., 
                      ** AccessNetworkID) is obtained for the first time 
                      ** since the system is acquired.
                      **
                      ** Cleared: May be cleared by ss-scripts.*/ 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_PWR_SAVE_LST,  /* A list of references to system records that
                      ** represent the systems that are on the power save
                      ** acquisition list.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: By power save scripts.
                      **
                      ** Cleared: Never
                      **
                      ** Order: By MRU_LST */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_OH_CHAN_LST,   /* A list of reference to system records that represent
                      ** the channels that exist in PRL and channel list in 
                      ** HDR sector parameter message.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: whenever the HDR overhead information 
                      ** changes from not current to current.
                      **
                      ** Cleared: Clear before the update.*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_TEMP2_LST,     /* Temporary list used for manipulation.
                      **
                      ** Initialized: By scripts and when comparing systems
                      **
                      ** Updated: Randomly
                      **
                      ** Cleared: Randomly
                      **
                      */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                      /* 36 */

  SDSR_PWR_SCAN_LST,  /* A list of references to system records that
                      ** represent the systems that are on the power scan
                      ** acquisition list.
                      **
                      ** Initialized: To all systems at power-up.
                      **
                      ** Updated: In the scripts after GW ACQ
                      **
                      ** Cleared: Never
                      **
                      ** Order: */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_FORBID_GEO_LST,/* A list of reference to system records that represent
                      ** the channels that correspond to the GEO of a 
                      ** forbidden system
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: Whenever a forbidden system is encountered
                      **
                      ** Cleared: May be cleared by SS scripts. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SCRATCH_LST,   /* Temporary list used as scratch pad all over SD.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: Randomly
                      **
                      ** Cleared: Randomly. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_BACKUP_LST,    /* Temporary list 
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: In scripts that don't have a PREV_SCR 
                      ** construct
                      **
                      ** Cleared: In scripts */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH

  SDSR_HYBR_1_LST        = 120, /* FOR RANGE CHECKING */

  SDSR_HYBR_1_PRL_LST,
  SDSR_HYBR_1_AVAIL_LST,    /* Like SDSR_AVAIL_LST for HDR hybrid operation */
  SDSR_HYBR_1_ACQ_GEO_LST,  /* Like SDSR_ACQ_GEO_LST for HDR hybrid operation */

                             /* 124 */
  SDSR_HYBR_1_IDL_GEO_LST,  /* Like SDSR_IDL_GEO_LST for HDR hybrid operation */
  SDSR_HYBR_1_SRV_GEO_LST,  /* Like SDSR_SRV_GEO_LST for HDR hybrid operation */
  SDSR_HYBR_1_ACQ_PREF_LST, /* Like SDSR_ACQ_PREF_LST for HDR hybrid operation */
  SDSR_HYBR_1_IDL_PREF_LST, /* Like SDSR_IDL_PREF_LST for HDR hybrid operation */

                             /* 128 */
  SDSR_HYBR_1_SRV_PREF_LST, /* Like SDSR_SRV_PREF_LST for HDR hybrid operation */
  SDSR_HYBR_1_START_PREF_LST,/* Like SDSR_START_PREF_LST for HDR hybrid operation*/
  SDSR_HYBR_1_REDIR_LST,    /* Like SDSR_REDIR_LST for HDR hybrid operation */
  SDSR_HYBR_1_SRCH_LST,     /* Like SDSR_SRCH_LST for HDR hybrid operation */

                             /* 132 */
  SDSR_HYBR_1_SCHED_LST,    /* Like SDSR_SCHED_LST for HDR hybrid operation */
  SDSR_HYBR_1_MEAS_LST,     /* Like SDSR_MEAS_LST for HDR hybrid operation */
  SDSR_HYBR_1_ACQ_COLLOC_LST,/*Like SDSR_ACQ_COLLOC_LST_ for HDR hybrid operation*/
  SDSR_HYBR_1_SRV_COLLOC_LST,/*Like SDSR_SRV_COLLOC_LST_ for HDR hybrid operation*/

                              /* 136 */
  SDSR_HYBR_1_MRU_LST,       /* Like SDSR_MRU_LST for hybrid operation */
  SDSR_HYBR_1_ACQ_CHAN_LST,  /* Like SDSR_ACQ_CHAN_LST for HDR hybrid operation */                     
  SDSR_HYBR_1_PWR_SAVE_LST,  /* Like SDSR_PWR_SAVE_LST for HDR hybrid operation */
  SDSR_HYBR_1_OH_CHAN_LST,   /* Like SDSR_OH_CHAN_LST for HDR hybrid operation */

                              /* 140 */
  SDSR_HYBR_1_ACQED_LST,
  SDSR_HYBR_1_GW_LST,       
  SDSR_HYBR_1_MANUAL_LST,  
  SDSR_HYBR_1_GW_LTD_LST,

                              /* 144 */
  SDSR_HYBR_1_SCRATCH_LST,    /* Like SDSR_SCRATCH_LST for HDR hybrid operation */
  SDSR_HYBR_1_BACKUP_LST,     /* Like SDSR_BACKUP_LST for HDR hybrid operation */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_MAX_LST,       /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* --------------------
  ** System record items.
  ** --------------------
  */
  SDSR_1ST_SYS        = 160,  /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_SYS,       /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt acquiring a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_FULL_SYS,  /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted FULL acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt FULL acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_MINI_SYS,  /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted MINI acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt MINI acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 164 */
  SDSR_ACQ_MICRO_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted MICRO acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt MICRO acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_FULL2_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted FULL2 acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt FULL2 acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_MINI2_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted MINI2 acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt MINI2 acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SDSR_ACQ_MICRO2_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted MICRO acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt MICRO acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                      /* 168 */
  SDSR_ACQ_DEEP_SYS,
  SDSR_ACQ_DEEP2_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted DEEP acquisition (for HDR only).
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt DEEP acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_SHALLOW_SYS,
  SDSR_ACQ_SHALLOW2_SYS,/* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted SHALLOW acquisition (for HDR only).
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt SHALLOW acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 172 */
  SDSR_EMERG_SYS,     /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted acquisition during an emergency
                      ** origination mode.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt acquiring a system (not due to reacquisition
                      ** schedule) and origination mode is emergency.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //SDSR_REDIAL_SYS,    /* A reference to a system record that corresponds to
  //                    ** the silent redial system.
  //                    **
  //                    ** Initialized: To an empty item at power-up or when
  //                    ** NAM selection/information is modified.
  //                    **
  //                    ** Updated: By the SS-Script.
  //                    **
  //                    ** Cleared: By the SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_START_SYS,     /* A reference to the system record that corresponds to
                      ** the system most recently acquired during the
                      ** acquisition-start state (see sdsr.c).
                      **
                      ** Initialized: To an empty item at power-up.
                      **
                      ** Updated: Whenever a system is being acquired while
                      ** in the acquisition-start state.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_IDL_SYS,       /* A reference to a system record that corresponds to
                      ** the system on which service was most recently
                      ** provided during idle/access operation.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving:
                      ** sd_ss_ind_cdma_acq_schm
                      ** sd_ss_ind_cdma_opr_oh_info
                      ** sd_ss_ind_amps_acq_cch_sid
                      ** sd_ss_ind_amps_opr_pch_sid
                      ** sd_ss_ind_hdr_acq_sid
                      ** sd_ss_ind_hdr_opr_oh_info
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SRV_SYS,       /* A reference to a system record that corresponds to
                      ** the system on which service was most recently
                      ** provided.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving:
                      ** sd_ss_ind_cdma_acq_schm
                      ** sd_ss_ind_cdma_opr_oh_info
                      ** sd_ss_ind_cdma_opr_cam_cdma
                      ** sd_ss_ind_cdma_opr_cam_amps
                      ** sd_ss_ind_cdma_opr_hdm_cdma
                      ** sd_ss_ind_cdma_opr_hdm_amps
                      ** sd_ss_ind_cdma_opr_itspm
                      ** sd_ss_ind_amps_acq_cch_sid
                      ** sd_ss_ind_amps_opr_pch_sid
                      ** sd_ss_ind_hdr_acq_sid
                      ** sd_ss_ind_hdr_opr_oh_info
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 176 */
  SDSR_SI_SYS,        /* A reference to a system record that corresponds to
                      ** the system per which service is currently indicated.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When updating the sistem indicators.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_REDIR_SYS,     /* A reference to a system record that corresponds to
                      ** the system on which the last redirection (that is
                      ** represented by REDIR_LST) was received.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_REDIR.
                      **
                      ** Cleared: Should be cleared by the SS-Script to turn
                      ** redirection OFF */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_HO_SYS,        /* A reference to a system record that corresponds to
                      ** the system that was included in the last channel
                      ** assignment/handoff direction indication to be
                      ** received from SS-Client
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SD_SS_IND_CDMA_OPR_CAM_CDMA,
                      ** SS_IND_CDMA_OPR_CAM_AMPS, SSS_IND_CDMA_OPR_HDM_CDMA
                      ** or SS_IND_CDMA_OPR_HDM_AMPS.
                      **
                      ** Cleared: Should be cleared by the SS-Script to turn
                      ** AMPS handoff OFF */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_NEW_SYS,       /* A reference to a system record that corresponds to
                      ** the system that was included in the last New
                      ** System indication to be received from SS-Client.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_NEW_SYS.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 180 */
  SDSR_CDMA_AVAIL_SYS,/* A reference to a system record that corresponds to
                      ** the system that was included in the last CDMA
                      ** Available (from AMPS) indication to be received
                      ** from SS-Client.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving
                      ** SD_SS_IND_AMPS_OPR_CDMA_AVAIL.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_LOST_SYS,      /* A reference to a system record that corresponds to
                      ** the system which was most recently lost.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_SYS_LOST or
                      ** SS_IND_AMPS_OPR_SYS_LOST.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_LOCK_SYS,      /* A reference to a system record that corresponds to
                      ** the system on which a lock order was most recently
                      ** received.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_LOCK.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_CAND_SYS,      /* A reference to a system record that corresponds to
                      ** the candidate system. Before a system is fully
                      ** acquired, the candidate system is the most preferred
                      ** system we found so far during acquisition.
                      **
                      ** Initialized: To an empty item at power-up, when NAM
                      ** selection/information is modified, or whenever a
                      ** system gets fully acquired.
                      **
                      ** Updated: When receiving SS_IND_CDMA_ACQ_SCHM or
                      ** SS_IND_AMPS_ACQ_CCH_SID.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 184 */
  SDSR_AMPS_A_SYS,    /* A reference to a system record that corresponds to
                      ** the AMPS A system.
                      **
                      ** Initialized: To reference the AMPS A system at
                      ** power-up or when NAM selection/information is
                      ** modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_B_SYS,    /* A reference to a system record that corresponds to
                      ** the AMPS B system.
                      **
                      ** Initialized: To reference the AMPS B system at
                      ** power-up or when NAM selection/information is
                      ** modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_1ST_SYS,  /* A reference to a system record that corresponds to
                      ** the 1st AMPS system in the PRL acquisition table.
                      ** Note that if PRL contains no AMPS systems, this item
                      ** is empty.
                      **
                      ** Initialized: To reference the 1st AMPS system in the
                      ** PRL at power-up or when NAM selection/information is
                      ** modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_2ND_SYS,  /* A reference to a system record that corresponds to
                      ** the 2nd AMPS system in the PRL acquisition table.
                      ** Note that if PRL contains no AMPS systems, this item
                      ** is empty.
                      **
                      ** Initialized: To reference the 2nd AMPS system in the
                      ** PRL at power-up or when NAM selection/information is
                      ** modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 188 */
  SDSR_AMPS_BS1_SYS,  /* A reference to a system record that corresponds to
                      ** the 1st AMPS backstop system in the PRL acquisition
                      ** table. Note that if AMPS backstop feature is
                      ** disabled, this item is empty.
                      **
                      ** Initialized: To reference the 1st AMPS backstop
                      ** system in the PRL at power-up or when NAM
                      ** selection/information is modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_BS2_SYS,  /* A reference to a system record that corresponds to
                      ** the 2nd AMPS backstop system in the PRL acquisition
                      ** table. Note that if AMPS backstop feature is
                      ** disabled, this item is empty.
                      **
                      ** Initialized: To reference the 2nd AMPS backstop
                      ** system in the PRL at power-up or when NAM
                      ** selection/information is modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_BSIDL_SYS,/* A reference to a system record that corresponds to
                      ** the AMPS backstop system that is associated with the
                      ** SDSR_IDL_SYS. Note that if no AMPS backstop system
                      ** is associated with the SDSR_IDL_SYS, this item is
                      ** empty.
                      **
                      ** Initialized: To an empty item at power-up.
                      **
                      ** Updated: Whenever the SDSR_IDL_SYS is updated.
                      **
                      ** Cleared: Whenever SDSR_IDL_SYS is not associated
                      ** with an AMPS backstop system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_TEMP_SYS,      /* A temporary item to help with list manipulation,
                      ** such as list ordering.
                      **
                      ** Initialized: As needed.
                      **
                      ** Updated: As needed.
                      **
                      ** Cleared: As needed.
                      **
                      ** Order: As needed */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /* 192 */
  SDSR_GW_LAST_LTD_SYS,/* A reference to a system record that corresponds to
                       ** the last attempted GW limited system.
                       **
                       ** Initialized: It is empty during the power up.
                       **
                       ** Updated: When there is a attempt to acquire a full
                       **          service, store the equivalent sr with
                       **          limited service to sdsr table and update
                       **          the system with the
                       **
                       ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_GW_LAST_USER_SYS,
                       /* A reference to a system record that corresponds to
                       ** the last attempted GW User system, which coule be
                       ** any system but not limited system in PRL
                       ** acquisition Table or User list.
                       **
                       ** Initialized: It is populated from MRU list if there
                       **              is user system available.
                       **              OW it is empty during the power up.
                       **
                       ** Updated: Whenever there is a attemp to acq system
                       **          in manual mode or get network request.
                       **
                       ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_GW_LAST_FULL_SYS,
                       /* A reference to a system record that corresponds to
                       ** the last attempted GW system, which is not a
                       ** limited system.
                       **
                       ** Initialized: It is empty during the power up.
                       **
                       ** Updated: Whenever MS decides to stay on a system.
                       **
                       ** Cleared: Never */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_CAND_IDL_SYS,  /* A reference to a system record as a idle candidate
                       ** 
                       ** Initialized: It is empty during the power up.
                       **
                       ** Updated: When HDR overhead is received, it is 
                       **          populated in the following order
                       **
                       **          1.The first channel in the channel list
                       **            received in sector parameter
                       **            message ordered ACQ_SYS followed
                       **            by the band class of SRV_SYS if it has
                       **            a match in the PRL.
                       **
                       **          2.The acquisition system.
                       **          
                       ** 
                       ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_UNIFORM_FULL_SYS,  /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted FULL acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt FULL acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH

  SDSR_HYBR_1_SYS       = 280, /* FOR RANGE CHECKING */

  SDSR_HYBR_1_ACQ_SYS,      /* Like SDSR_ACQ_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_FULL_SYS, /* Like SDSR_ACQ_FULL_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_MINI_SYS, /* Like SDSR_ACQ_MINI_SYS for hybrid operation */

                      /* 284 */
  SDSR_HYBR_1_ACQ_MICRO_SYS,/* Like SDSR_ACQ_MICRO_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_FULL2_SYS,/* Like SDSR_ACQ_FULL2_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_MINI2_SYS,/* Like SDSR_ACQ_MINI2_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_MICRO2_SYS,/* Like SDSR_ACQ_MICRO2_SYS for hybrid operation */
  

                      /* 288 */
  SDSR_HYBR_1_ACQ_DEEP_SYS, /* Like SDSR_ACQ_DEEP_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_DEEP2_SYS,/* Like SDSR_ACQ_DEEP2_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_SHALLOW_SYS,/* Like SDSR_ACQ_SHALLOW_SYS for hybrid oper */
  SDSR_HYBR_1_ACQ_SHALLOW2_SYS,/* Like SDSR_ACQ_SHALLOW2_SYS for hybrid oper */
  

                      /* 292 */
  SDSR_HYBR_1_START_SYS,    /* Like SDSR_START_SYS for hybrid operation */
  SDSR_HYBR_1_IDL_SYS,      /* Like SDSR_IDL_SYS for hybrid operation */
  SDSR_HYBR_1_SRV_SYS,      /* Like SDSR_SRV_SYS for hybrid operation */
  SDSR_HYBR_1_SI_SYS,       /* Like SDSR_SI_SYS for hybrid operation */
  

                      /* 296 */
  SDSR_HYBR_1_REDIR_SYS,    /* Like SDSR_REDIR_SYS for hybrid operation */
  SDSR_HYBR_1_LOST_SYS,     /* Like SDSR_LOST_SYS for hybrid operation */
  SDSR_HYBR_1_CAND_SYS,     /* Like SDSR_CAND_SYS for hybrid operation */
  SDSR_HYBR_1_TEMP_SYS,     /* Like SDSR_TEMP_SYS for hybrid operation */
  

                      /* 300 */
  SDSR_HYBR_1_CAND_IDL_SYS, /* Like SDSR_CAND_IDL_SYS for hybrid operation */
  SDSR_HYBR_1_EMERG_SYS,
  SDSR_HYBR_1_GW_LAST_LTD_SYS,
  SDSR_HYBR_1_GW_LAST_USER_SYS,
                      
                      /* 304 */
  SDSR_HYBR_1_GW_LAST_FULL_SYS,

  #endif

  SDSR_MAX
} sdsr_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of ordering categories, according to which part of system
** records can be compared and system record lists can be ordered.
*/
typedef enum {

  SDSR_ORDER_BAND,      /* According to bands in the system record */

  SDSR_ORDER_MAX
} sdsr_order_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of comparison categories, according to which system records
** can be compared and system record lists can be sorted.
*/
typedef enum {

  SDSR_COMP_RXPWR,      /* According to Rx power measurements */
  SDSR_COMP_PILOT,      /* According to Pilot Strength measurements */
  SDSR_COMP_AMPS,       /* AMPS systems before other systems */
  SDSR_COMP_CDMA,       /* CDMA systems before other systems */
  SDSR_COMP_HDR,        /* HDR systems before other systems */
  SDSR_COMP_DIGITAL,    /* Digital systems before other systems */
  SDSR_COMP_BC0,        /* Band Class 0 systems before other systems */
  SDSR_COMP_BC1,        /* Band Class 1 systems before other systems */
  SDSR_COMP_BC4,        /* Band Class 4 systems before other systems */
  SDSR_COMP_BC5,        /* Band Class 5 systems before other systems */
  SDSR_COMP_BC6,        /* Band Class 6 systems before other systems */
  SDSR_COMP_BC10,       /* Band Class 10 systems before other systems */
  SDSR_COMP_BC11,       /* Band Class 11 systems before other systems */
  SDSR_COMP_BC12,       /* Band Class 12 systems before other systems */
  SDSR_COMP_BC14,       /* Band Class 14 systems before other systems */
  SDSR_COMP_BC15,       /* Band Class 15 systems before other systems */
  SDSR_COMP_BC16,       /* Band Class 16 systems before other systems */
  SDSR_COMP_BC17,       /* Band Class 17 systems before other systems */
  SDSR_COMP_BC18,       /* Band Class 18 systems before other systems */
  SDSR_COMP_BC19,       /* Band Class 19 systems before other systems */
  SDSR_COMP_ROAM,       /* Favorable roaming indication sys before others */
  SDSR_COMP_PRL_ACQ,    /* According to PRL acquisition table order */
  SDSR_COMP_CELL_A,     /* Cellular A systems before other systems */
  SDSR_COMP_CELL_B,     /* Cellular B systems before other systems */
  SDSR_COMP_CELL_HOME,  /* Cellular home-side systems before other systems */
  SDSR_COMP_GW,         /* GW systems before other systems */

  SDSR_COMP_MAX
} sdsr_comp_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of equality categories according to which lists can be checked
** for equality.
*/
typedef enum {

  SDSR_EQUAL_MODE,      /* Mode equality */
  SDSR_EQUAL_BAND,      /* Band equality */
  SDSR_EQUAL_CHAN,      /* Channel equality */
  SDSR_EQUAL_SID_NID,   /* SID/NID equality */

  SDSR_EQUAL_MAX
} sdsr_equal_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of contain conditions according to which lists can be
** compared.
*/
typedef enum {

  SDSR_CONTAIN_NONE,    /* List1 contains no elements of list2 */
  SDSR_CONTAIN_SOME,    /* List1 contains some elements of list2 */
  SDSR_CONTAIN_ALL,     /* List1 contains all elements of list2 */
  SDSR_CONTAIN_SAME,    /* List1 contains the same elements as list2 */

  SDSR_CONTAIN_MAX
} sdsr_contain_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration system record catergories.
*/
typedef enum {

  SDSR_CAT_NON_AMPS,    /* Non AMPS systems */
  SDSR_CAT_NON_CDMA,    /* Non CDMA systems */
  SDSR_CAT_NON_HDR,     /* Non HDR systems */
  SDSR_CAT_NON_GW,      /* Non GW systems */
  SDSR_CAT_GW_SYS,      /* GW system */
  SDSR_CAT_NON_CELL,    /* Non cellular systems */
  SDSR_CAT_NON_COMPLY,  /* NONE COMPLY systems*/
  SDSR_CAT_HDR,         /* HDR systems*/
  SDSR_CAT_NON_WLAN,
  SDSR_CAT_ALL,          /* ALL systems */

  SDSR_CAT_MAX
} sdsr_cat_e_type;


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=============================== SYSTEM RECORD ===============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_print

DESCRIPTION
  Print the content of a system record.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_print(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to print its content.
            */

        boolean                   is_print_full
            /* Indicate whether the complete system record information
            ** should be printed.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_sid_encode_mcc_imsi_11_12

DESCRIPTION
  Incode the MCC/IMSI_11_12 into the SID/NID fields.

DEPENDENCIES
  The MCC and IMSI_11_12 should be in their OTA format - see IS-2000.5.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_sid_encode_mcc_imsi_11_12(

        sdsr_s_type               *sr_ptr
            /* Pointer to a system record into which to encide the
            ** MCC/IMSI_11_12.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_sid_restore_true

DESCRIPTION
  Restore the true SID/NID values into the SID/NID fields. This function undo
  what sdsr_sid_encode_mcc_imsi_11_12() does.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_sid_restore_true(

        sdsr_s_type               *sr_ptr
            /* Pointer to a system record for which to restore the true
            ** SID/NID.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_hdr_sid_print

DESCRIPTION
  Print HDR Sector ID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_hdr_sid_print(

        const byte                sid_ptr[16]
            /* Pointer to array of 16 byte Sector ID.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_is_same_is856_sid

DESCRIPTION
  Check whether a system record as an ID that is matching the specified
  IS-856 Sector ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system ID match the specified IS-856 Sector ID. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_is_same_is856_sid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record to be checked for SID matching.
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        int                       len
            /* Length to match.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_is_same_sid

DESCRIPTION
  Check whether the system IDs of two system records are equal.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system IDs are equal. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_is_same_sid(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
);


/*===========================================================================

FUNCTION sdsr_is_same_gw_lac

DESCRIPTION
  Check whether the LAC of two GW system IDs of two system records are equal.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the LACs are equal. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_is_same_gw_lac(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_comp

DESCRIPTION
  Compare two system records according to a specified category.

DEPENDENCIES
  None.

RETURN VALUE
  Positive integer if the 1st system record is of higher priority than the
  2nd system record according to the specified comparison category.

  Negative integer if the 1st system record is of lower priority than the
  2nd system record according to the specified comparison category.

  0 if 1st system record is of equal priority to the 2nd system record
  according to the specified comparison category.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_comp(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr1_ptr.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_comp2

DESCRIPTION
  Compare two system records according to a specified category.

DEPENDENCIES
  None.

RETURN VALUE
  Positive integer if the 1st system record is of higher priority than the
  2nd system record according to the specified comparison category.

  Negative integer if the 1st system record is of lower priority than the
  2nd system record according to the specified comparison category.

  If the system records are the same according to the specified comparison
  category, the system records are compared by their list positions.
  
  0 ONLY if 1st system record and the 2nd system record have the same list
  positions (which technically can happen only if both the pointers are
  pointing to the same system record).

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  int                       sdsr_comp2(

        const sdsr_s_type               *sr1_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type               *sr2_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr1_ptr.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
);



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SYSTEM RECORD TABLE ============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_tbl_size

DESCRIPTION
  Return the size of the system record table.

DEPENDENCIES
  None.

RETURN VALUE
  The size of the system record table.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                   sdsr_tbl_size( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store

DESCRIPTION
  Store the system record that is pointed to by the sr_ptr parameter in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.

  If a matching table entry is not found and the system record table is not
  full to capacity, a new entry is added to the end of the system record
  table.

  If a matching table entry is not found and the system record table is full
  to capacity, the new system record replaces an existing table entry as per
  the following criteria:

    1. Systems from the PRL's acquisition tables are never deleted from the
       system record table.

    2. Additional systems retained on a most recently used/attempted
       acquisition basis; that is, the system that was least recently
       used/attempted acquisition is deleted from the table first.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record buffer containing the system to be
            ** added to the system record table.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store2

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.

  If a matching table entry is not found and the system record table is not
  full to capacity, a new entry is added to the end of the system record
  table.

  If a matching table entry is not found and the system record table is full
  to capacity, the new system record replaces an existing table entry as per
  the following criteria:

    1. Systems from the PRL's acquisition tables are never deleted from the
       system record table.

    2. Additional systems retained on a most recently used/attempted
       acquisition basis; that is, the system that was least recently
       used/attempted acquisition is deleted from the table first.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store2(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan
            /* The system's channel.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store3

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID and NID per the input SID and NID.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store3(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid
            /* The system's NID.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store4

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's expected SID and NID per the input expected SID
  and NID.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store4(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid_expected,
            /* The system's expected SID.
            */

        word                      nid_expected,
            /* The system's expected NID.
            */
        boolean                   is_delete_tmsi
            /* Indicate if MS is required to delete the assigned TMSI.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store5

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID, NID, PRL-designation and roaming indication
  fields per the corresponding input parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store5(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid,
            /* The system's NID.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

       sys_roam_status_e_type     roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store5_is856

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's IS-856 SID, PRL-designation and roaming
  indication fields per the corresponding input parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store5_is856(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

        sys_roam_status_e_type    roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store6

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID, NID, expected-SID, expected-NID,
  PRL-designation and roaming indication fields per the corresponding input
  parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store6(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid,
            /* The system's NID.
            */

        word                      sid_expected,
            /* The system's expected SID.
            */

        word                      nid_expected,
            /* The system's expected NID.
            */

        word                      mcc,
            /* The system's MCC.
            */
        byte                      imsi_11_12,
            /* The system's IMISI_11_12.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

        sys_roam_status_e_type    roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store7

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's sys_index (i.e., the system table index
  referencing this system) per the input system table index.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store7(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_plmn_s_type      *sd_plmn_ptr,
            /* pointer to sid union type
            */

        word                      sys_idx
            /* The system table index referencing this system.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store9

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and
  expected sid in the system record table.

  Update the stored record's sys_index (i.e., the system table index
  referencing this system) per the input system table index.

  Update the stored record's with expected plmn information.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type                   sdsr_tbl_store9(

        sd_mode_e_type                  mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type          band_pref,
            /* The system's band.
            */

        sd_chan_type                    chan,
            /* The system's channel.
            */

        const sd_sid_plmn_wlan_s_type   *sid_exp_ptr,
             /* pointer to Expected SID.
             */

        word                            sys_idx,
            /* The system table index referencing this system.
            */

        sys_profile_id_type             profile_id,
            /* profile id to be used.
            */

        sys_wlan_bss_e_type             wlan_bss_type,
            /* Applicable only for WLAN.
            */

        sys_tech_mask_type              wlan_tech_mask
            /* WLAN tech mask
            */

);



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store10

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and
  sid in the system record table.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store10(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_sid2_u_type      *sid_ptr
             /* pointer to ID.
             */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_get_ptr

DESCRIPTION
  Return a pointer to the system record that is referenced by the sr_ref
  parameter.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Pointer to the requested system record, NULL if no such record exists.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_s_type               *sdsr_tbl_get_ptr(

        sdsr_ref_type             sr_ref
            /* Reference to a system record for which to get a pointer.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_print

DESCRIPTION
  Print the content of a system record table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_tbl_print( void );




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SYSTEM RECORD LIST =============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_list_clr

DESCRIPTION
  Clear a system record list/item.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_clr(

        sdsr_e_type               list
            /* List/item to clear.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_cnt

DESCRIPTION
  Return the number of items in a system record list

DEPENDENCIES
  None.

RETURN VALUE
  The number of items in the input system record list.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_list_cnt(

        sdsr_e_type               list
            /* List/item for which to return the count.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_cpy

DESCRIPTION
  Copy the elements from one system record list to another system record
  list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_cpy(

        sdsr_e_type               list_to,
            /* List to which to copy the elements.
            */

        sdsr_e_type               list_from
            /* List from which to copy the elements.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_intersect

DESCRIPTION
  Copy the intersect elements to target list from the reference list.
  
  DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void                       sdsr_list_intersect( 

       sdsr_e_type                list_to,
       /* The target list to store the intersect elements from the reference 
       ** list.
       */

       sdsr_e_type                list_ref
       /* The reference List.
       */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_get_ref

DESCRIPTION
  Given a list and a list position return the system record reference of
  the system record that is associated with this list and list position.

DEPENDENCIES
  None.

RETURN VALUE
  System record reference of the system record that is associated with the
  input list and list position if list and pos resolved to an existing system
  record. SDSR_REF_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_list_get_ref(

        sdsr_e_type               list,
            /* List from which to get the system record reference.
            */

        int                       pos
            /* List position from which to get the system record reference.
            */

);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_get_ptr

DESCRIPTION
  Given a list and a list position return a pointer to system record that is
  referenced by this list and list position.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the system record that is referenced by the input list and list
  position if list and pos resolved to an existing system record. NULL
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_s_type               *sdsr_list_get_ptr(

        sdsr_e_type               list,
            /* List from which to get the system record pointer.
            */

        int                       pos
            /* List position from which to get the system record pointer.
            */

);



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_get_cpy

DESCRIPTION
  Get a copy of the system record list that is referenced by the list and pos
  parameters into the system record buffer that is pointed to by the sr_ptr
  parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if list and pos resolved to an existing system record. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_list_get_cpy(

        sdsr_e_type               list,
            /* List from which to get the system record information.
            */

        int                       pos,
            /* List position from which to get the system record information.
            */

        sdsr_s_type               *sr_ptr
            /* Pointer to a buffer where to copy the system record
            ** information.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_find

DESCRIPTION
  Given a list and a system record, this function find and return the list
  position of that system record.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the matching system record if such a system record is found.
  SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_list_find(

        sdsr_e_type               list,
            /* List over which to find the system record reference.
            */

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to search.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_find_ref

DESCRIPTION
  Given a list and a system record reference, this function find and return
  the list position of that system record reference.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the matching system record reference if such a system record is
  found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_list_find_ref(

        sdsr_e_type               list,
            /* List over which to find the system record reference.
            */

        sdsr_ref_type             sr_ref
            /* System record reference to for which to search.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_find_sys

DESCRIPTION
  Find a system that matches specific mode band and channel on a system
  record list.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the 1st matching system record if such a system record is
  found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_list_find_sys(

        sdsr_e_type               list,
            /* List over which to find the matching system.
            */

        sd_mode_e_type            mode,
            /* Mode to match.
            */

        sd_band_e_type            band,
            /* Band to match.
            */

        sd_chan_type              chan
            /* Channel to match.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_put_ref

DESCRIPTION
  Put a system record reference into a system record item.

  Note! The put operation is only valid for system record items (as apposed
  to system record lists).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_put_ref(

        sdsr_e_type               list,
            /* Item where to put the system record information.
            */

        sdsr_ref_type             sr_ref
            /* Pointer to a system record reference which to put into the
            ** system record item that is referenced by list.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_pos

DESCRIPTION
  Delete a position  from a system record list.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_del_pos(

        sdsr_e_type               list,
            /* List for which to add the system record.
            */

        int                       del_pos
            /* Deletion position. SDSR_POS_NULL implies deletion from the
            ** end of the list.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_ref

DESCRIPTION
  Delete a system record reference from a system record list.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  Position of system record reference that was deleted from the list.
  SDSR_POS_NULL, if system record reference to be deleted was not found
  on the list.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_list_del_ref(

        sdsr_e_type               list,
            /* List for which to add the system record.
            */

        sdsr_ref_type             sr_ref
            /* System record reference to be deleted.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_list

DESCRIPTION
  Delete the system records that are referenced by list_ref from list.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_del_list(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_e_type               list_ref
            /* Reference list.
            */
);



/*===========================================================================

FUNCTION sdsr_list_del_list_cnt

DESCRIPTION
  Deletes 'count' entries from the list starting from start_pos.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_del_list_cnt(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */
        int                      start_pos,
            /* The starting position in the list
            */
        int                      cnt
            /* Number of elements to delete from the list
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_sys

DESCRIPTION
  Delete system records from list according to the del_cat parameter.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_del_sys(

        sd_ss_e_type              ss,
            /* The SS for which to check the preferences
            */

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_cat_e_type           sr_cat
            /* System record category.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_band

DESCRIPTION
  Delete system records from list according to the band information in
  the reference system.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_del_band(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_e_type               sys_ref
            /* reference sys which contains band information
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_insert_ref

DESCRIPTION
  Insert a system record reference to a system record list.

  If the reference is already on the list, it is deleted and then inserted
  in the beginning or the end (as specified).

  Note! The add operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_insert_ref(

        sdsr_e_type               list,
            /* List for which to add the system record.
            */

        int                       insert_pos,
            /* Insertion position. SDSR_POS_NULL implies insertion to the
            ** end of the list.
            */

        sdsr_ref_type             sr_ref,
            /* System record reference which to insert to the list.
            */

        boolean                   is_delete_dup
            /* Indicate whether a duplicate item should be deleted from
            ** the list.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_store_and_app

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table along is a specified SID and append a reference to the
  stored system record to the end of the specified input list.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_store_and_app(

        sdsr_e_type               list,
            /* List to be inserted.
            */

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band pref.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_plmn_s_type      *sd_plmn_ptr,
            /* Pointer to sid union type
            */

        word                      sys_idx
            /* The system table index referencing this system.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_store_and_app2

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table along is a specified system index and append a
  reference to the stored system record to the end of the specified input
  lists.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdsr_list_store_and_app2(

        sdsr_e_type                    list1,
        sdsr_e_type                    list2,
        sdsr_e_type                    list3,
            /* Lists to be inserted.
            */

        sd_mode_e_type                 mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type         band_pref,
            /* The system's band pref.
            */

        sd_chan_type                   chan,
            /* The system's channel.
            */

        const sd_sid_plmn_wlan_s_type  *sid_exp_ptr,
            /* Pointer to sid exp type
            */

        word                           sys_idx
            /* The system table index referencing this system.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_app_ref

DESCRIPTION
  Append a system record reference to the end of a list. Duplicate reference
  is not appended.

  This operation is only valid to system record lists (as apposed to system
  record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_app_ref(

        sdsr_e_type               list,
            /* List to which to append system records.
            */

        sdsr_ref_type             sr_ref
            /* System record reference which to appended.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_app

DESCRIPTION
  Append the elements from one system record list to the end of another
  system record list. Duplicate items are not appended.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_app(

        sdsr_e_type               list_to,
            /* List to which to append system records.
            */

        sdsr_e_type               list_from
            /* List from which to append system records.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_order

DESCRIPTION
  Order the elements of a system record list according to their order in a
  reference system record list.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_order(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        sdsr_e_type               list_ref
            /* Reference list according to which to order the elements of
            ** list
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_order2

DESCRIPTION
  Order the elements of a system record list according to a reference system
  record list based on the order cat.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_order2(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        sdsr_e_type               list_ref,
            /* Reference list according to which to order the elements of
            ** list
            */

        sdsr_order_e_type         order_cat
            /* Ordering categories, according to which part of system records
            ** can be compared and system record lists can be ordered.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_order

DESCRIPTION
  Order the elements of a system record list according to the reference
  system record based on the order category.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_order(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        const sdsr_s_type         *sr_ptr,
            /* Pointer to Reference system to be ordered by.
            */

        sdsr_order_e_type         order_cat
            /* Ordering categories, according to which part of system records
            ** can be compared and system record lists can be ordered.
            */
);




/*===========================================================================

FUNCTION sdsr_list_sort

DESCRIPTION
  Sort the elements of a system record list according to an indicated sorting
  order. Note that the sorting algorithm is stable; that is, items that are
  indifference according to the sorting criteria retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_sort(

        sdsr_e_type               list,
            /* List to be sorted.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_print

DESCRIPTION
  Print the content of a system record list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_print(

        sdsr_e_type               list,
            /* List to be printed.
            */

        boolean                   is_print_full
            /* Indicate whether the complete system record information
            ** should be printed.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_is_same

DESCRIPTION
  Compare two system record lists with respect to a specified equality
  category.

  Specifically check whether list1[i] is equal to list2[i] with respect to a
  specified equality category for all i in list1 and list2.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if the two system record lists are the same with respect to the
  specified equality category. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_list_is_same(

        sdsr_e_type               list1,
            /* 1st list to be compared.
            */

        sdsr_e_type               list2,
            /* 2nd list to be compared.
            */

        sdsr_equal_e_type         equal_cat
            /* Category according to which to check the lists for equality.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_sorted_is_same

DESCRIPTION
  Compare two system record lists with respect to a specified equality
  category.

  Specifically check whether list1[i] is equal to list2[i] with respect to a
  specified equality category for all i in list1 and list2.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if the two system record lists are the same with respect to the
  specified equality category. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_list_sorted_is_same(

        sdsr_e_type               list1,
            /* 1st list to be compared.
            */

        sdsr_e_type               list2,
            /* 2nd list to be compared.
            */

        sdsr_comp_e_type          comp_cat,
            /* Comparison category according to which system records should
            ** be compared.
            */

        sdsr_equal_e_type         equal_cat
            /* Category according to which to check the lists for equality.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_is_contain

DESCRIPTION
  Check whether list1 contains contain_cond elements of list2,
  where contain_cond can take on one of the following values:

  SDSR_CONTAIN_NONE   - list1 contains no elements of list2
  SDSR_CONTAIN_SOME   - list1 contains some elements of list2
  SDSR_CONTAIN_ALL    - list1 contains all elements of list2
  SDSR_CONTAIN_SAME   - list1 contains the same elements as list2

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if list1 contain contain_cond elements of list2, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_list_is_contain(

        sdsr_e_type               list1,
            /* 1st list to be checked.
            */

        sdsr_contain_e_type       contain_cond,
            /* Contain condition.
            */


        sdsr_e_type               list2
            /* 2nd list to be checked.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_fade_update

DESCRIPTION
  Update fade related parameter of the reference system

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_fade_update(

        sdsr_e_type               list ,
           /* List to the reference system.
           */
        sd_ss_sys_lost_e_type     sys_lost
           /* The system lost reason.
           */
); /* sdsr_list_fade_update */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_is_fade

DESCRIPTION
  Check if there are at least CNT number of fades for the reference system.
  Return true if so otherwise return false.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_list_is_fade(

        sdsr_e_type               list,
            /* List to the reference system.
            */
        byte                      cnt
            /* Number of consecutive fades.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_lists_update_gw_sys

DESCRIPTION
  If the reference system is GW system, populate GW last full/LTD system
  based on the reference system.

  If reference system is FULL system, update GW last full system with the
  the reference system. Store a equivelent system with limited mode and
  update GW last LTD with the stored system.

  If reference system is LIMITED system, update GW last LTD systme with the
  reference system.

  Create an automatic mode counter part of the reference system and store it
  in SDSR_GW_LAST_FULL_SYS and if update is requested.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_update_gw_sys(

        sd_ss_e_type              ss,
            /* The ss for which the idle sys is to be updated
            */

        sdsr_ref_type             sr_ref,
            /* A system reference to create the couterpart of limited/user
            ** systems.
            */
        boolean                   update_ltd_sys,
            /* If ture, update the limited list and system with the system
            ** reference.
            */
        boolean                   update_user_sys,
            /* If ture, update the user list and system with the system
            ** reference.
            */
        boolean                   update_full_sys
            /* If ture, update the full system with the system reference.
            */
);




/*===========================================================================

FUNCTION sdsr_list_undefine_plmn_id

DESCRIPTION
  Undefine the plmn id of a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_undefine_plmn_id(

        sdsr_e_type               list
            /* List to undefine plmn id.
            */
);




/*===========================================================================

FUNCTION sdsr_list_reset_del_tmsi

DESCRIPTION
  Reset the status bit mask of systems in the reference list.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void                       sdsr_list_reset_status_mask(

        sdsr_e_type               list,
            /* List to the reference system.
            */
        sdsr_status_mask_e_type   status_mask
            /* Status bit mask to be reset
            */
);




/*===========================================================================

FUNCTION sdsr_is_delete_tmsi

DESCRIPTION
  This function indicates that whether tmsi should be delete or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  boolean                   sdsr_is_del_tmsi(

        sdsr_e_type               list,
            /* List to the reference system.
            */

        int                       pos
            /* List position from which to get the system record reference.
            */
);

/*===========================================================================

FUNCTION sdsr_is_sys

DESCRIPTION
  Indicate if the specified system is GW system.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  True  the system is mode.
  FALSE Otherwise.

  Note: mode GW will return TRUE if the system is either GSM, WCDMA or GW

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                    sdsr_is_sys(

       sd_mode_e_type             mode,
           /* The mode to check for the system
           */

       sdsr_e_type                list,
           /* List from which to get the specified system record.
           */

       int                        pos
           /* List position from which to get the specified system record.
           */

);

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
========================== SYSTEM RECORD COMPONENT ==========================
=============================================================================
=============================================================================
===========================================================================*/


#if ( defined (FEATURE_HDR) || defined(FEATURE_TECH_IND_SD_STACK_ARCH))
/*===========================================================================

FUNCTION sdsr_lists_hybr_pref_switch

DESCRIPTION
  Switches the SDSR_HDR_XXX list to reference the appropriate lists according
  to the specified hybrid preference.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                    sdsr_lists_hybr_pref_switch(

        sd_ss_hybr_pref_e_type  hybr_pref
            /* Hybrid preference.
            */
);
#endif /* FEATURE_HDR */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_component_init

DESCRIPTION
  Initialize the System Record Component.

  NOTE! this function must be called before any other sdss_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_component_init( void );




#endif /* SDSR_H */




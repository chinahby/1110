#ifndef SDPRL_H
#define SDPRL_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

    P R E F E R R E D   R O A M I N G    L I S T   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with SDPRL.C (i.e. the PRL interface components of the SD).

Copyright (c) 2000 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sdprl.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
02/13/09   sn/rk      Constant GW UOOS scan time across band groups.
02/06/09   aj      WLAN subscription decoupling 
09/04/08   sv      Expose sdprl_update_wlan_mru_list
09/02/08   sv      Include Block-channel definition from prl.h
03/17/08   vs      Added function sdprl_get_addn_sys_record_info to get the
                   association tag and geo info for the system record
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
01/04/07   jqi     Provide the utility APIs for user to change the acquisition 
                   table of PRL
05/22/06   jqi     Made use of the chan list for HDR oh info.
03/20/06   sk      Added support for PRL extraction level SDPRL_EXTRACT_HOME.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
02/24/06   sk      Added a new enum to sdprl_comp_depth_e_type to support the
                   change of order of checks in sdprl_is_more_pref_upto().
                   Added comments to sdprl_comp_depth_e_type.
01/09/06   sk      Added support for custom homes bitmap in NV.
                   Made changes to treat custom home system as home system.
                   Added sdprl_is_more_pref_upto().
                   Added sdprl_roam_ind_map_custom_home_to_home().
                   Added sdprl_comp_depth_e_type.
12/02/05   jqi     LInt 8 fixes.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
11/16/04   sj      Added prototype for adding sid/nid lock.
10/04/04   jqi     Valid band and mode againt both target and PRL for task
                   initialization and system pref change command.
08/09/04   sj      Added hybr_pref param to sdprl_nam_sel().
06/21/04   RC      Improve association (collocation) determination algorithm
                   for HDR systems while in hybrid operation - CR 44638.
01/15/04   RC      Added support for FEATURE_SILENT_REDIAL_20
10/31/03   JQ      Added support for latest IS-683D format changes.
07/15/03   JQ      Added IS683 Support
                   - D PRL support.
                   - Propagate the LAC, band and chan from stack.
12/12/01   RC      Added support CDMA-HDR hybrid operation.
10/30/01   RC      Added support for IS-683C collcated systems.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
01/03/01   RC      Changes per 4th week of I&T.
11/17/00   RC      Initial release.

===========================================================================*/


#include "sd.h"        /* External interface to SD */
#include "sdsr.h"      /* External interface to SDSR.C (System Record)*/
#include "prl.h"       /* Interface to PRL */
#include "comdef.h"    /* Definition for basic types and macros */




/* <EJECT> */
/*---------------------------------------------------------------------------
                           PRL Matching Levels
---------------------------------------------------------------------------*/

/* Enumeration of PRL matching levels.
*/
typedef enum {

  SDPRL_MATCH_NONE,               /* No match */

  SDPRL_MATCH_SID,                /* SID/NID only match */
  SDPRL_MATCH_SID_MODE,           /* SID/NID and mode only match */
  SDPRL_MATCH_SID_MODE_BAND,      /* SID/NID, mode and band only match */
  SDPRL_MATCH_SID_MODE_BAND_CHAN, /* SID/NID, mode, band and channel match */

  SDPRL_MATCH_MAX
} sdprl_match_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                           PRL Matching Levels
---------------------------------------------------------------------------*/

/* Enumeration of PRL sid/nid matching levels.
*/
typedef enum {

  SDPRL_MATCH_SID_NID_NONE,               /* No match */

  SDPRL_MATCH_WILDCARD_SID,       /* WILDCARD SID(PLMN) only match */
  SDPRL_MATCH_WILDCARD_NID,       /* SID(PLMN), WILDCARD NID(LAC) match */
  SDPRL_MATCH_SID_NID,            /* SID(PLMN) NID(LAC) match */

  SDPRL_MATCH_SID_NID_MAX
} sdprl_match_sid_nid_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                           PRL Extraction Levels
---------------------------------------------------------------------------*/

/* Enumeration of PRL information extraction levels.
*/
typedef enum {

  SDPRL_EXTRACT_ALL,            /* Indicate that all information should be
                                ** extracted */

  SDPRL_EXTRACT_MATCH_ONLY,     /* Indicate that only the information of a
                                ** system that has a PRL match (as apposed
                                ** to a system that does not have a PRL
                                ** match) should be extracted */

  SDPRL_EXTRACT_ROAM_COMPLY,    /* Indicate that only systems that comply
                                ** with the current roaming preferences
                                ** should be extracted */

  SDPRL_EXTRACT_SAME_BETTER,    /* Indicate that only systems that are of
                                ** same or better preference of the serving
                                ** system should be extracted */

  SDPRL_EXTRACT_HOME,           /* Indicate that only systems that are home
                                ** systems should be extracted */

  SDPRL_EXTRACT_MAX
} sdprl_extract_e_type;


/*---------------------------------------------------------------------------
                           PRL Matching Depth
---------------------------------------------------------------------------*/

/* Enumeration of more pref comparison depth.
**
** NOTE: Maintain the order of the enum to match the comparison order in
**       sdprl_is_more_pref_upto.
*/

typedef enum {

  SDPRL_COMP_UPTO_GEO_POS,
    /* Check if one system is more preferred than the other system in the
    ** first GEO that lists both systems.
    */

  SDPRL_COMP_UPTO_PRL_DESIG,
    /* Check if only one of the systems is listed as the preferred or most
    ** preference system in the PRL.
    */

  SDPRL_COMP_UPTO_ROAM,
    /* Check if the roaming indicator of one system is more favorable than
    ** the other.
    */

  SDPRL_COMP_UPTO_ONLY_ONE_MOST_PREF,
    /* Check if only one system is listed as most preferred system in its
    ** GEO.
    */

  SDPRL_COMP_UPTO_CELL_HOME,
    /* Check if both systems are available and only one of them is a cellular
    ** home side system.
    */

  SDPRL_COMP_UPTO_ACQ_TBL_ORDER,
    /* Check if one system is higher than the other in the PRL acquisition
    ** table. 
    */

  SDPRL_COMP_UPTO_DIGITAL,
    /* Check if only one of the systems is DIGITAL.
    */

  SDPRL_COMP_UPTO_LAST
    /* Check all the above conditions.
    */

} sdprl_comp_depth_e_type;


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_amps_bc0_sys_to_blksys

DESCRIPTION
  Given an AMPS band-class 0 system, this function returns the cellular
  system (in blksys representation) that is associated with this AMPS system.

DEPENDENCIES
  None.

RETURN VALUE
  The cellular system (in blksys representation) that is associated with the
  input AMPS band-class 0 system.

  SD_BLKSYS_MAX if the input system is bad/unsupported.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_blksys_e_type           sdprl_amps_bc0_sys_to_blksys(

        sd_cell_sys_e_type        cell_sys
            /* The band-class 0 CDMA channel for which to get the sub-band.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_map_chan_to_blksys

DESCRIPTION
  Given a system's mode, band and a channel, this function returns the
  PCS frequency block/cellular system that is associated with the input
  mode, band and channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block/cellular system that is associated with the input
  mode, band and channel.

  SD_BLKSYS_MAX if the input mode, band and channel are bad or unsupported.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_blksys_e_type          sdprl_map_chan_to_blksys(

        sd_mode_e_type            mode,
            /* The mode of system for which to get the block/system.
            */

        sd_band_e_type            band,
            /* The band of system for which to get the block/system.
            */

        sd_chan_type              chan
            /* The Channel of system for which to get the block/system.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_map_amps_vc_to_sys

DESCRIPTION
  Map AMPS (band-class 0) voice channel to cellular system (A/B).

DEPENDENCIES
  None.

RETURN VALUE
  SD_CELL_SYS_A if AMPS voice channel belongs to A-Side.
  SD_CELL_SYS_B if AMPS voice channel belongs to B-Side.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_cell_sys_e_type        sdprl_map_amps_vc_to_sys(

        word                      voice_chan
            /* AMPS voice channel number to convert.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_supp_mode_band_pref

DESCRIPTION
  Inform the caller to whether a specified combination of mode and band
  preference is supported by the PRL that is associated with the currently
  selected NAM.

  For example, analog-only mode preference is only supported by a PRL that
  contains analog acquisition records.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the PRL that is associated with the currently selected NAM. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference to be validated.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_amps_backstop

DESCRIPTION
  Inform the caller to whether AMPS backstop is turned ON for the current
  NAM selection.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if AMPS backstop is turned ON for the current NAM selection. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_amps_backstop( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_exp_sys_idx

DESCRIPTION
  Check whether a system record matches its expected PRL's system table
  index.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID fields of the system record that is being checked is set to
  appropriate value.

RETURN VALUE
  TRUE if system record matchs its expected PRL's system table index. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_exp_sys_idx(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record to be checked for a match.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_associated

DESCRIPTION
  Check whether two system records are associated (collocated). Two systems
  are said to be associated if they are in the same GEO and their association
  tag is equal.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID fields of the system record that is being checked is set to
  appropriate value.

RETURN VALUE
  TRUE if system record 1 and 2 are associated. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_associated(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to system record 2.
            */
);

/*===========================================================================

FUNCTION sdprl_get_addn_sys_record_info

DESCRIPTION
  Get the associated tag of the system record.
  - find the system ercord in the PRL.
  - If found, return the association tag.
  - return null is not found

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID fields of the system record that is being checked is set to
  appropriate value.

RETURN VALUE
  association_tag if system record is found. SDSS_ASSN_TAG_NONE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                   sdprl_get_addn_sys_record_info(

        const sdsr_s_type      *sr_ptr,
            /* Pointer to system record.
            */
        byte                   *assn_tag,
            /* Pointer to association tag.
            */
        int                    *geo_offset,
            /* Pointer to system record's geo.
            */
        int                    *multi_geo_count
            /* Pointer to the counter, for counting geo's
            ** in a multi-geo scenario.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_more_pref_upto

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2 upto
  the requested depth.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

RETURN VALUE
  TRUE if system record 1 is more preferred than system record 2. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_more_pref_upto(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type               *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type               *sr2_ptr,
            /* Pointer to system record 2.
            */

        sdprl_comp_depth_e_type   depth
            /* Depth of the comparison.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_more_pref

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_more_pref(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to system record 2.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_autonam_proc

DESCRIPTION
  Does auto-NAM change related processing.

  Specifically, check whether a specified system record has a SID/NID that is
  appropriate for auto-NAM change and if so notify the registered client by
  calling on call on sd_si_autonam_change.

  And auto-NAM change is appropriate whenever the following conditions
  are met:

    1. The SID/NID of the specified system does not match any of the home
       SID/NIDs of the current NAM.
    2. The SID/NID of the specified system does match one of the home
       SID/NIDs of other than the current NAM.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_autonam_proc(

        const sdsr_s_type         *sr_ptr
            /* Pointer to system record to be checked against the home
            ** SID/NID list.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_blksys_app

DESCRIPTION
  Append the CDMA systems that are associated with a specified band class and
  a PCS frequency block/Cellular system to a specified system record list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if band-class and/or PCS frequency block/Cellular system are
  supported. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_blksys_app(

        sdsr_e_type               list,
            /* System record list to which to append the system records.
            */

        sd_band_e_type            band,
            /* The band-class from which to append the system records.
            */

        sd_blksys_e_type          blksys
            /* The PCS frequency block/Cellular system from which to append
            ** the system records.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_all_pref_sys_app

DESCRIPTION
  Append all the CDMA preferred systems that are supported by the MS to the
  specified list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_all_pref_sys_app(

        sdsr_e_type               list
            /* System record list to which to append the system records.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_home_side

DESCRIPTION
  Return the home side cellular system (i.e., the cellular system that is
  associated with the first home SID/NID) of the specified NAM and mode.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  The home side cellular system of the specified NAM and mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_cell_sys_e_type        sdprl_get_home_side(

        sd_nam_e_type             nam,
            /* NAM for which to get the cellular side.
            */

        sd_mode_e_type            mode
            /* Mode for which to get the cellular side.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info_base

DESCRIPTION
  Given a system record, geo_list and a pref_list, this function does
  the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_get_info_base(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               srv_list,
            /* The system record list for which to build the serving list.
            */

        int                       srv_list_pos,
            /* The system record list position for which to build the serving
            ** list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info2

DESCRIPTION
  Given a system record, srv_sys, geo_list and a pref_list, this function 
  does the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.
    4. Update the srv_sys wiht PRL designation.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_get_info2(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               srv_list,
            /* The system record list for which to subtract the GEO list and
            ** PREF list.
            */

        int                       srv_list_pos,
            /* The system record list position for which to subtract the GEO
            ** list and PREF list. 
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        boolean                   is_update_srv_sys
            /* Indicate if the serving system need to be updated.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info

DESCRIPTION
  Given a system record, geo_list and a pref_list, this function does the 
  following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_get_info(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_gw_get_info

DESCRIPTION
  Given a GSM/WCDMA system record, geo_list and a pref_list, this function
  does the following:

    1. Set the PRL designation of the input system record as per the
       plmn_forbidden flag
    2. Clears the GEO_LST/PREF_LST/COLLOC_LST.
    3. Updates the system record as per the values passed.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE always.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_gw_get_info(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        sys_plmn_id_s_type        plmn,
            /* PLMN.
            */

        sys_lac_type              lac,
            /* LAC.
            */

        sys_srv_capability_e_type plmn_srv_cap,
           /* PLMN service capability.
           */

        boolean                   plmn_forbidden,
          /* Is PLMN forbidden?
           */


        sys_roam_status_e_type    roaming_ind
           /* Roaming status.
           */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_init_before_task_start

DESCRIPTION
  Initialize the SD PRL services before task start. Now indicate that PRL
  is inactive at this time.


DEPENDENCIES
  This function must be called before any other sdprl_xxx() function is
  ever called.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_init_before_task_start( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_init

DESCRIPTION
  Initialize the SD PRL services.

  NOTE! This function only returns after the PRL related items (but the PRL
  itself that is being loaded when sdprl_nam_sel() is called) are read from
  NV.

DEPENDENCIES
  This function must be called before any other sdprl_xxx() function is
  ever called.

RETURN VALUE
  TRUE if SD PRL initialization is successful. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_init( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_save

DESCRIPTION
  Save PRL-Component related information to NV.

  NOTE! This function only returns after the SD related items (i.e., the
  MRU list) are written to NV.

DEPENDENCIES
  This function must be called when powering off the phone or going offline.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_save( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_wlan_nam_sel

DESCRIPTION
  Inform the SD-PRL component of a new NAM selection.


DEPENDENCIES
  sdprl_init() must have already been called to initialize SDPRL.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                   sdprl_wlan_nam_sel(

        sd_nam_e_type             curr_nam
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */
);


/*===========================================================================

FUNCTION sdprl_nam_sel

DESCRIPTION
  Inform the SD-PRL component of a new NAM selection.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize SDPRL.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully read from NV). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_nam_sel(

        sd_nam_e_type             curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_hybr_pref_e_type    hybr_pref
            /* Current hybrid preference.
            */
);

/*===========================================================================

FUNCTION sdprl_read_wlan_priv_lst

DESCRIPTION
  Read the WLAN user list from NV for WLAN acquisitions during automatic
  mode.


DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdprl_read_wlan_priv_lst
(
        sdsr_e_type               lst
          /* Append the WLAN user systems in this lst.
          */
);

/*===========================================================================

FUNCTION sdprl_update_wlan_mru_list

DESCRIPTION
  Delete records from SDSR_WLAN_MRU_LST that are absent from
  SDSR_WLAN_PRIV_LST

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdprl_update_wlan_mru_list (void);


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_plmn_set_plmn_sys

DESCRIPTION
  Set plmn information: plmn lac include, plmn select mode, plmn id and lac.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                       sdprl_plmn_set_plmn_sys(

        sd_plmn_s_type             *plmn_ptr,
        /* Pointer to sid union type (PLMN)
        */

        const sys_plmn_id_s_type   *plmn_id_ptr,
        /* plmn id
        */

        word                       lac,
        /* lac id
        */

        sd_net_select_mode_e_type  net_select_mode
        /* network selection mode
        */
);




/*===========================================================================

FUNCTION sdprl_is_prl_active

DESCRIPTION
  Indicate if PRL is active or not.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  Reutn TRUE if PRL is active otherwise return FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_prl_active( void );


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_add_lock_sid_nid

DESCRIPTION
  Adds the given SID/NID as locked.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the specified SID/NID was added.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_add_lock_sid_nid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to system record to be added to the locked
            ** SID/NID list.
            */

        boolean                   ignore_nid
            /* Set nid as WILDCARD
            */
);

/*===========================================================================

FUNCTION sdprl_roam_ind_map_custom_home_to_home

DESCRIPTION
  Maps the custom homes to home and leaves the rest untouched.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  SYS_ROAM_STATUS_OFF if input is a custom home. Input value otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_roam_status_e_type    sdprl_roam_ind_map_custom_home_to_home(
    
        sys_roam_status_e_type    roam_ind
            /* Roaming indicator from system record.
            */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref

DESCRIPTION
  Given a band_pref in sd band_pref format, maps it to MRU format

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_map_sd_band_pref_to_mru_band_pref(

        sd_ss_band_pref_e_type    sd_band_pref,
            /* system selection band and sub-band preferences
            */
        sd_band_pref_s_type       *band_pref
           /* MRU band and sub-band preference output
           */
);




/*===========================================================================

FUNCTION sdprl_is_sys_match

DESCRIPTION
  indicate if there is a match of input system in the PRL
  
DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  True if there is a match.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_sys_match(

       sd_ss_e_type               ss,
           /* System selection enum type
           */

       sdsr_ref_type              sr_ref
            /* System reference
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_mru_clr

DESCRIPTION
  Clear the MRU of the current NAM.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void                      sdprl_clr_mru( void );

/*===========================================================================

FUNCTION sdprl_is_prl_gw_mode_gw

DESCRIPTION
  Parses through GW systems in PRL / GW_LST and finds whether both GSM and 
  WCDMA systems are present or not

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  Boolean - Whether both GSM and WCDMA records present or not.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                   sdprl_is_prl_gw_mode_gw(
       sd_ss_e_type               ss
);

#endif /* SDPRL_H */





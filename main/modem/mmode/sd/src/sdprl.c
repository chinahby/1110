/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

            P R E F E R R E D    R O A M I N G     L I S T

GENERAL DESCRIPTION
  This file provides interface to Preferred Roaming List (PRL) services.
  Note that the PRL itself is implemented by prl.c.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2009 by Qualcomm Incorporated. All Rights Reserved.
Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sdprl.c#12 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
05/19/09   sv      Update roam indicator based on valid system matches.
03/16/09   mj      Add default HDR channel from NV to scan list
03/12/09   jq      Add HICPS support for FEATURE_ADD_HDR_IF_IS683A_PRL.
02/17/09   aj      Fix compilation warning
02/13/09   sn      Constant GW UOOS scan time across band groups.
02/06/09   aj      WLAN subscription decoupling 
01/26/09   aj      Fix bug in sdprl_parser_append_geo() - CR 165812
12/18/08   sg      Include "sdprlt.c" only if SD_DEBUG is defined
10/14/08   rm      Printing debug message for Single band group
10/02/08   aj      Bandclass support for BC17, 18, 19
09/25/08   sv      Memory reduction changes, featurizing GW code
09/04/08   sv      Exposing sdprl_update_wlan_mru_list and
                            sdprl_read_wlan_priv_list
09/02/08   sv      Remove Block-channel definition, moved to prl.h
08/12/08   fj      Lint error fix.
06/12/08   aj      Klocwork fixes
05/28/08   ak      Added Debug messages for MRU write.
03/17/08   vs      Added function sdprl_get_addn_sys_record_info to get the
                   association tag from the system record
02/21/08   cl      Added SD API for sd_misc_is_fts_mode to determine if the 
                   phone is in FULL_TIME_SHDR Mode or not.
02/14/08   vk      Corrected code to overwrite roam indicator only when in sid/nid and prl
                   roam indicator is 0,1 or 2
01/28/08   sg      Changes to make Single Band Group PRL the default PRL
12/19/07   ka      Changes to support 16k PRL
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
08/25/07   jqi/cl  Added OTASP band class BC4,5,6,7,8,9,10,11,12,14 & 15
                   support.
07/24/07   cl      Mainline FEATURE_INT_ROAMING and FEATURE_2ND_PRL
06/11/07   sk      Adding support for passing netlist system record index
                   for WLAN acquisition and measurement.
02/02/07   jqi     Removed the band class subblock validation from SD except
                   for CELL and PCS band.
01/04/07   jqi     Provide the utility APIs for user to change the acquisition
                   table of PRL
12/06/06   sk      Fixed improper acquisition of negative collocated HDR
                   system when it is preferred in another GEO.
                   Fixed check "are two systems in same GEO" to consider
                   match level.
11/15/06   pk      Added support for HYBR_GW
11/07/06   sk      Memory reduction effort.
10/31/06   rp      Fixing compiler warnings.
10/11/06   jqi     Cleared the Emergency list upon NAM selection.
10/10/06   jqi     Added CDMA450 BLOCK L support.
09/26/06   sk      Fixed misaligned dword access by WLAN MRU code by
                   rewriting it.
09/22/06   rp      Added BC15, BC16 support.
09/11/06   rp      Removed filtering of WLAN acquisiton based on WLAN
                   technology.
09/08/06   sk      Added WLAN tech mask to WLAN MRU.
                   Fixed ugly memory corruption in WLAN MRU read/write.
08/07/06   pk      Added support for UOOS
07/07/06   sk      Updated calls to sdsr_list_update_gw_sys() to make sure
                   GW_LAST_FULL_SYS is not updated during NAM change.
06/06/06   jqi     fixed compilation warnings.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/04/06   jqi     Lint fixes.
04/09/06   jqi     Removed the specific code to construct IDL_PREF_LIST when
                   PRL_PREF is set to CELL_A or CELL_B only.
03/20/06   sk      Added support for PRL extraction level SDPRL_EXTRACT_HOME.
03/06/06   jqi     Featurized rfm_is_band_chan_supported() API.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
02/24/06   sk      Changed order of checks in sdprl_is_more_pref_upto().
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/12/06   pk      Modified the PRL validation routine. Skip over invalid
                   acq records and invalid channels and bands
01/09/06   sk      When a SID/NID is in the NAM home list, DO NOT change the
                   roaming indicator to OFF if the original value is a custom
                   home.
01/09/06   sk      Added support for custom homes bitmap in NV.
                   Made changes to treat custom home system as home system.
                   Added sdprl_is_more_pref_upto().
                   Added sdprl_roam_ind_map_custom_home_to_home().
12/02/05   jqi     LInt 8 fixes and compilation warning.
11/23/05   jqi     Mainline FEATURE_HICPS20 and FEATURE_HICPS30 under
                   FEATURE_HDR.
11/18/05   pk      Added support for sub-blocks I,J,K for BC 5 and BC 11
11/07/05   jqi     Populated the colloc pref list based on the main colloc
                   list.
                   When there is a nam change, populated the colloc list
                   regardless the hybr preference.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/14/05   jqi     Added BC14 support.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/18/05   ka      After NAM change WLAN MRU list is cleared of systems that
                   are not in WLAN PRIVATE list.
06/26/05   ic      Lint and compiler warnings cleanup
04/13/05   jqi     Do not clear colloc list to fixe the hybrid data switch
04/06/05   dk      Clear WLAN Private list at subscription change.
02/07/05   jqi     Lint fixes.
01/27/05   ka      Added support for Band class 10, 11, 12
01/06/04   jqi     Lint fixes.
12/04/04   jqi     Clear the GW list upon nam selection.
11/19/04   jqi     Added support for FEATURE_ADD_GW_IF_IS683C_PRL
11/16/04   sj      Added support for additional sid/nid lock entries.
11/10/04   jqi     Fixed ADS compilation warnings.
10/21/04   RC      Added support for FEATURE_ADD_HDR_IF_IS683A_PRL.
10/04/04   jqi     Valid band and mode againt both target and PRL for task
                   initialization and system pref change command.
08/31/04   sj      Include current sys in the colloc lst to fix hdronly to
                   hybrid transition.
08/26/04   jqi     Lint fix.
08/20/04   sj      Added support for Auto A/B.
08/09/04   sj      Added hybr_pref param to sdprl_nam_sel() and populate the
                   colloc_lst only if the hybr_pref is ON for HICPS20.
07/30/04   jqi     Qualband optimization - eliminate scheduling.
07/07/04   sj      Fixed with MRU for hybrid hdr by reserving spots for HDR
                   & user.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/21/04   RC      Improve association (collocation) determination algorithm
                   for HDR systems while in hybrid operation - CR 44638.
06/16/04   RC      Fixed an issue where sdprl_parser_sys_is_more_pref()
                   was not taking into account the PRL match level.
06/08/04   RC      Commented out incorrect prl and roam indicators
                   manipulation in sdprl_get_info_base().
06/07/04   jqi     More quad-band enhancement for automatic mode.
06/04/04   RC      Added support for FFEATURE_2ND_PRL
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/24/04   RC      Changed incorrect SD_BLKSYS_CELL_A to SD_BLKSYS_CELL_B
04/19/04   jqi     Lint fix - for error level : error.
03/17/03   RC      In function sdprl_sys_is_match() there is no need to match
                   the band for MCC/IMSI_11_12 systems.
01/22/04   JQ      Changed SDSR_GW_SYS with appropriate grouping of GW bands.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
01/16/04   JQ      Temporary fix of band pref for limited services.
01/15/04   RC      Added support for FEATURE_SILENT_REDIAL_20
12/16/03   rc/aaj  Support for FEATURE_HDR_HICPS20
12/09/03   jqi     CR 38023- MRU3 misaligned mem access due to pointer
                   casting.
12/08/03   jqi     Fixed issues with IS-683D implementation.
12/08/03   SJ      Fixed switch error and and added error message
12/04/03   jqi     CR37489-Fixed a null pointer issue issue when populate MRU table
                   from NV.
11/14/03   JQ      Fixes for IS683D support.
11/12/03   JQ      Use manual sel mode specified with undef plmn for power up
                   manual orig mode.
11/05/03   JQ      Do not error fatal while validating PRL.
                   More MRU3 updates.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
10/31/03   JQ      Added support for latest IS-683D format changes.
                   MRU3 table updates.
10/24/03   RC      Changed SDSR_GW_LST to contain a single system of mode
                   SD_MODE_GW.
10/09/03   jqi     Adjusted band preference enum.
09/22/03   SJ      Fixed issues with MRU save/read backup.
09/10/03   RC      Relaxed BC3 channel checking to include invalid channels.
08/29/03   RC      Calling on sdprl_parser_lists_init() when we found a
                   better match level in sdprl_get_info_base().
08/18/03   RC      Fixed issues with IS-683D implementation.
08/15/03   RC      Fixed out of place "break" that caused PRL mismatch.
07/31/03   SJ      Changed NV_MRU3_TABLE_SIZE to NV_MRU_TABLE_SIZE.
07/15/03   JQ      Added Extended PRL support.
06/16/03   RC      In function sdprl_sys_is_match() there is no need to match
                   the band for HDR systems.
05/27/03   RC      Speed up PRL searching. Put #ifdef around PRL development.
05/06/03   JQ      Added IS683B PRL support.
03/11/03   SJ      Clear MRU lst in sdprl_init.
03/04/03   SJ      Added GSM/WCDMA systems in separate list GW_LST instead of
                   PRL_LST to prevent SD doing reselection on GSM/WCDMA for
                   limited automatic mode support.
02/26/03   SJ      Added support for Band Class 8 & 9.
01/21/03   RC      Relaxed channel checking to include invalid channels.
01/20/03   jqi     Added Band Class 6 support.
01/16/03   SJ      Removed inclusion of DB header file.
06/18/02   RC      Consult the home_sid_nid list in sdprl_parser_app_geo().
05/17/02   RC      Added support for IS-683C, PN and data association fields.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
03/07/02   RC      Fixed out of place #endif that caused compile time
                   error for FEATURE_KPCS.
03/05/02   RC      PRL format changes to support IS-683C/SSPR_P_REV=3.
02/06/02   RC      Eliminated compiler warnings.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
01/16/02   jqi     Read primary/secondary CDMA chan. from NV upon NAM change.
01/10/02   jqi     Introduced FEATURE_BAND_CLASS_5_BLOCK_X_PREF as preferred
                   block to solve channel-overlapping problem in function
                   prl_cdma_bc5_chan_to_blksys.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
12/07/01   jqi     Added support for band class 5.
10/30/01   RC      Added support for IS-683C collcated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
07/20/01   RC      Added support for IS-683C.
07/03/01   RC      Now using PRL_IS_CHAN_IN_CELLULAR_SYS_X macros.
04/24/01   RC      More changes to support tri-mode targets.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
02/01/01   RC      Changes per 8th week of I&T.
01/12/01   RC      Changes per 7th week of I&T.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
11/27/00   RC      Now only adding preferred systems to more preferred list.
11/17/00   RC      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "sdprl.h"    /* External interface to sdprl.c */
#include "sd.h"       /* External interface to sd.c */
#include "sdi.h"      /* Internal interface to sd.c */
#include "sdsr.h"     /* External interface to SDSR.C (System Record)*/
#include "sdss.h"     /* External interface to sdss.c */
#include "sdnv.h"     /* External interface to sdnv.c */
#include "sddbg.h"    /* SD debug macros */

#include "prl.h"      /* Interface to PRL services */
#include "nv.h"       /* Interface to NV services */
#include "bit.h"      /* Bit packing/unpacking services */

#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */

#ifdef FEATURE_USERZONE
#include "puzl.h"     /* Interface to PUZL services */
#endif /* FEATURE_USERZONE */

#include <string.h>   /* Interface to string library */
#include "sys.h"

#ifdef FEATURE_WLAN
#error code not present
#endif

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* To enable TAZ-2 target to use a standard IS-683-A PRL, we define local
** CDMA and HDR mode values (i.e., SD_MODE_CDMA_ and SD_MODE_HDR_). For
** TAZ-2 SD_MODE_HDR_ is defined as SD_MODE_CDMA so that the CDMA acquisition
** records that are in the PRL are perceived as HDR acquisition records.
*/
#ifdef FEATURE_SD_TAZ2_PRL
#error code not present
#else
  #define SD_MODE_CDMA_   SD_MODE_CDMA
  #define SD_MODE_HDR_    SD_MODE_HDR
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Number of entries/offset for ss-main in the MRU.
*/
#define SDPRL_MRU_NUM_MAIN_ENTRIES      (10)
#define SDPRL_MRU_MAIN_ENTRIES_OFFSET   (0)

/* Number of entries/offset for HDR hybrid in the MRU.
*/
#define SDPRL_MRU_NUM_HYBR_ENTRIES       (1)
#define SDPRL_MRU_HYBR_ENTRIES_OFFSET   (SDPRL_MRU_NUM_MAIN_ENTRIES)

/* Number of entries/offset for USER list in the MRU.
*/
#define SDPRL_MRU_NUM_USER_ENTRIES       (1)
#define SDPRL_MRU_USER_ENTRIES_OFFSET   (SDPRL_MRU_HYBR_ENTRIES_OFFSET + SDPRL_MRU_NUM_HYBR_ENTRIES)


/* Number of additional entries for SID/NID lock.
*/
#define SDPRL_NUM_ADDL_SID_NID_LOCK      (10)


#if defined(FEATURE_WLAN)
#error code not present
#endif


/* Type for holding PRL related information of a single NAM instance.
*/
typedef struct {

  /* MRU table.
  */
  //nv_mru3_table_entry_type mru_tbl[NV_MRU_TABLE_SIZE];
  sd_mru_table_entry_u_type mru_tbl[NV_MRU_TABLE_SIZE];


  /* Primary/Secondary A/B channels.
  */
  word                      pch_a;   /* Primary channel A */
  word                      pch_b;   /* Primary channel B */
  word                      sch_a;   /* Secondary channel A */
  word                      sch_b;   /* Secondary channel B */

  /* JTACS Primary/Secondary A/B channels.
  */
  word                      jtacs_pch_a;   /* Primary channel A */
  word                      jtacs_pch_b;   /* Primary channel B */
  word                      jtacs_sch_a;   /* Secondary channel A */
  word                      jtacs_sch_b;   /* Secondary channel B */

  /* Home SID/NID list.
  */
  struct {
    word  sid;
    word  nid;
  }                         home_sid_nid[NV_MAX_HOME_SID_NID];


  /* AMPS home SID.
  */
  word                      amps_home_sid;


  /* Locked SID/NID list.
  */
  struct {
    word  sid;
    word  nid;
  }                         lock_sid_nid[NV_MAX_SID_NID_LOCK+SDPRL_NUM_ADDL_SID_NID_LOCK];


  /* AMPS backstop enabled.
  */
  boolean                   is_amps_backstop;

} sdprl_nam_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for holding the information that is associated with the SD-PRL
** component.
*/
typedef struct {

  /* The currently selected NAM.
  */
  sd_nam_e_type                   curr_nam;

  /* Pointer to buffer that holds a copy of the PRL that is associated is the
  ** currently selected NAM.
  */
  const byte                      *prl_ptr;

  /* Pointer to buffer that holds information about the PRL that is pointed
  ** to by prl_ptr.
  */
  const prl_hdr_s_type            *prl_hdr_ptr;

  /* Item for holding SD information that is associated with the currently
  ** selected NAM.
  */
  sdprl_nam_s_type                nam_info[NV_MAX_NAMS];


  /* Indicate whether the silent redial with AMPS feature is enabled.
  */
  boolean                         is_srda;

  /* Indicate if prl is active or not.
  */
  boolean                         is_prl_active;

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

} sdprl_s_type;


/* SD-PRL item.
*/
static sdprl_s_type  sdprl;


typedef enum {

  SDPRL_APPEND_ALL,       /* Refer to Celluar acq rec for CDMA preferred rec
                          ** OR non-preferred GSM/WCDMA acq rec
                          ** for GW preferred record.
                          */
  SDPRL_APPEND_PREF_ONLY, /* Append only preferred acq record.
                          */
  SDPRL_APPEND_MAX

} sdprl_append_e_type;


#define NO_CHAN ((word) (-1))

/*===========================================================================
=============================================================================
=============================================================================
=============================== PRL PARSER ==================================
=============================================================================
=============================================================================
===========================================================================*/
/* Type for PRL parser.
*/
typedef struct {

  /* Associated dystem selection - MAIN or HDR.
  */
  sd_ss_e_type                    ss;

  /* Pointer to a RAM image of the PRL.
  */
  const byte                      *prl_ptr;

  /* Bit offsets to the beginning of the system table, as well as, the
  ** number of system table entries.
  */
  dword                           sys_tbl_offset;
  word                            sys_tbl_num;

  /* Index and offset (in bits) of the last system record table entry to be
  ** parsed, as well as, the record itself and its packed length in bits.
  */
  word                            sys_idx;
  dword                           sys_offset;
  prli_sys_rec_type               sys_rec;
  dword                           sys_rec_len;


  /* Index and offset (in bits) of the first system in the GEO of the last
  ** system table entry to be parsed.
  */
  word                            geo_idx;
  dword                           geo_offset;

  /* Index and offset of the first more preferred system relative to the last
  ** system table entry to be parsed. Note that the index and offset only
  ** have meaning when pri_is_higher = TRUE;
  */
  word                            pri_idx;
  dword                           pri_offset;
  boolean                         pri_is_higher;
  prl_sspr_p_rev_e_type           sspr_p_rev;

} sdprl_parser_s_type;


static  void sdprl_parser_init(
        sdprl_parser_s_type *parser_ptr,
        sd_ss_e_type        ss,
        const byte          *prl_ptr,
        const prl_hdr_s_type   *prl_hdr_ptr
);


static  boolean                   sdprl_parser_next(
        sdprl_parser_s_type       *parser_ptr,
        prli_sys_rec_type         *rtrn_sys_rec_ptr
);

#define ROAM_IND_CUST_HOME_BYTE_LEN  32

static byte roam_ind_custom_home[ROAM_IND_CUST_HOME_BYTE_LEN];

/* Bit positions 64, 65 and 76 through 83 (inclusive) have been turned on in
** the default case to accomodate the custom home values used by Verizon.
*/
static byte default_roam_ind_custom_home[ROAM_IND_CUST_HOME_BYTE_LEN] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=========================== CONVERSION FUNCTIONS ============================
=============================================================================
=============================================================================
===========================================================================*/

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref

DESCRIPTION
  Given a band_pref in MRU format, maps it to sd band_pref.

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_band_pref_e_type    sdprl_map_mru_band_pref_to_sd_band_pref(

        sd_band_pref_s_type       band_pref
            /* MRU band_pref
            */
)
{
   sd_ss_band_pref_e_type   sd_band_pref = SD_SS_BAND_PREF_NONE;

   sd_mru_band_pref_e_type  mru_band_pref;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert sd_band_pref_s_type to sd_mru_band_pref_e_type
  */
   mru_band_pref  = (sd_mru_band_pref_e_type) band_pref.bit_31_16;
   mru_band_pref  = (sd_mru_band_pref_e_type)((uint32)mru_band_pref << 16);
   mru_band_pref  = SD_BAND_ADD2( mru_band_pref, band_pref.bit_15_0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_450) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_450;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_480) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_480;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_750) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_750;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_850) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_850;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_EGSM_900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_EGSM_900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_PGSM_900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_PGSM_900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_RGSM_900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_RGSM_900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_DCS_1800) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_DCS_1800;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_PCS_1900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_PCS_1900;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_I_IMT_2000) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_I_IMT_2000;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_II_PCS_1900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_II_PCS_1900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_III_1700) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_III_1700;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_IV_1700) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_IV_1700;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_V_850) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_V_850;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VI_800) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_VI_800;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VII_2600) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_VII_2600;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VIII_900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_VIII_900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_IX_1700) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_IX_1700;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sd_band_pref;
}
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */


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
MMEXTN  void                      sdprl_map_sd_band_pref_to_mru_band_pref(

        sd_ss_band_pref_e_type    sd_band_pref,
            /* system selection band and sub-band preferences
            */
        sd_band_pref_s_type       *band_pref
           /* MRU band and sub-band preference output
           */
)
/*lint -esym(818,band_pref) */
{

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
  sd_mru_band_pref_e_type  mru_band_pref = SD_MRU_BAND_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_450) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_450 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_480) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_480 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_750) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_750 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_850) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_850 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_EGSM_900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_EGSM_900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_PGSM_900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_PGSM_900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_RGSM_900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_RGSM_900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_DCS_1800) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_DCS_1800 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_PCS_1900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref,  SD_MRU_BAND_PREF_GSM_PCS_1900 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_I_IMT_2000) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_I_IMT_2000 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_II_PCS_1900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_II_PCS_1900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_III_1700) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_III_1700 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_IV_1700) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_IV_1700 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_V_850) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_V_850 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_VI_800) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref,  SD_MRU_BAND_PREF_WCDMA_VI_800 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_VII_2600) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VII_2600 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_VIII_900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VIII_900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_IX_1700) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_IX_1700 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert sd_mru_band_pref_e_type to sd_band_pref_s_type
  */

  band_pref->bit_15_0 = (word) ((word) mru_band_pref & 0xffff );
  band_pref->bit_31_16 =(word) ( (uint32)mru_band_pref >> 16 );

  #else
  SYS_ARG_NOT_USED(sd_band_pref);
  SYS_ARG_NOT_USED(band_pref);
  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */

  return;
}
/*lint +esym(818,band_pref) */


/*===========================================================================

FUNCTION sdprl_pcs_chan_to_block

DESCRIPTION
  Given a PCS CDMA channel this function returns the PCS frequency block that
  is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input PCS channel.

SIDE EFFECTS
  None.

===========================================================================*/
static  word                      sdprl_pcs_chan_to_block(

        sd_chan_type              chan
            /* The Channel of system for which to get the sub-band.
            */
)
{
  if( chan <= 299 ) {
    return PRL_BLOCK_A;
  }

  if( INRANGE(chan, 300, 399) ) {
    return PRL_BLOCK_D;
  }

  if( INRANGE(chan, 400, 699) ) {
    return PRL_BLOCK_B;
  }

  if( INRANGE(chan, 700, 799) ) {
    return PRL_BLOCK_E;
  }

  if( INRANGE(chan, 800, 899) ) {
    return PRL_BLOCK_F;
  }

  if( INRANGE(chan, 900, 1199) ) {
    return PRL_BLOCK_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the CDMA channel is out of range.
  */
  SD_ASSERT( FALSE );
  return PRL_BLOCK_RESERVED;

} /* sdprl_pcs_chan_to_block */




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
MMEXTN  sd_blksys_e_type           sdprl_amps_bc0_sys_to_blksys(

        sd_cell_sys_e_type        cell_sys
            /* The band-class 0 CDMA channel for which to get the sub-band.
            */
)
{
  if( cell_sys == SD_CELL_SYS_A ) {
    return SD_BLKSYS_CELL_A;
  }

  if( cell_sys == SD_CELL_SYS_B ) {
    return SD_BLKSYS_CELL_B;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the cellular system is out of range.
  */
  SD_MSG_LOW( "Bad amps_bc0_sys=%d",cell_sys,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_amps_bc0_sys_to_blksys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc0_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 0 channel, this function returns the cellular
  system that us associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The cellular system that is associated with the input CDMA band-class 0
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc0_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 0 CDMA channel for which to get the sub-band.
            */
)
{
  if( PRL_IS_CHAN_IN_CELLULAR_SYS_A(chan) )
  {
    return SD_BLKSYS_CELL_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( PRL_IS_CHAN_IN_CELLULAR_SYS_B(chan) )
  {
    return SD_BLKSYS_CELL_B;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the CDMA cellular channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc0_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc0_chan_to_blksys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc1_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 1 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 1
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc1_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 1 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 299 ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 300, 399) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 400, 699) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 700, 799) ) {
    return SD_BLKSYS_PCS_E;
  }

  if( INRANGE(chan, 800, 899) ) {
    return SD_BLKSYS_PCS_F;
  }

  if( INRANGE(chan, 900, 1199) ) {
    return SD_BLKSYS_PCS_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 1 CDMA channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc1_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc1_chan_to_blksys */

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc3_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 3 channel, this function returns the cellular
  system that us associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The cellular system that is associated with the input CDMA band-class 3
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc3_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 0 CDMA channel for which to get the sub-band.
            */
)
{
  /* Refer to ARIB T-53.
  */
  if( ((chan) & 0x0001) == 0 )
  {
    if( (chan) >= 1 && (chan) <=  1600 )
    {
      return SD_BLKSYS_CELL_A;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the CDMA cellular channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc3_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc3_chan_to_blksys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc4_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 4 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 4
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc4_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 4 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 199 ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 200, 399) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 400, 599) ) {
    return SD_BLKSYS_PCS_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 4 CDMA channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc4_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc4_chan_to_blksys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc5_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 5 channel this function returns the PCS frequency
  block (subclass) that is associated with this channel.
  There are channel numbers overlapping between blocks. To solve this
  problem, FEATURE_BAND_CLASS_5_BLOCK_X_PREF is introduced as preferred block.
  (X stands for B, C, D, G, I, J and K).The default preferred blocks are block A, E and H.
  Channel will be checked only in preferred blocks and F Block. If channel is in
  these blocks, the block will be returned. Otherwise SD_BLKSYS_MAX will be returned.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 5
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc5_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 5 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  sd_blksys_e_type ret_val = SD_BLKSYS_MAX;
  //************************************************
  if( INRANGE(chan, 472, 671) ) {
    ret_val =  SD_BLKSYS_PCS_L;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_L_PREF
  if (ret_val == SD_BLKSYS_PCS_L) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan,1536,1715) ) {
    ret_val = SD_BLKSYS_PCS_K;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_K_PREF
  if (ret_val == SD_BLKSYS_PCS_K) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 211, 400) ) {
    ret_val = SD_BLKSYS_PCS_J;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_J_PREF
  if (ret_val == SD_BLKSYS_PCS_J) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 54, 230) ) {
    ret_val = SD_BLKSYS_PCS_I;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_I_PREF
  if (ret_val == SD_BLKSYS_PCS_I) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 1039, 1260) ) {
    ret_val = SD_BLKSYS_PCS_H;
  }
  #ifdef FEATURE_BAND_CLASS_5_BLOCK_H_PREF
  if (ret_val == SD_BLKSYS_PCS_H) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 1235, 1473) ) {
    ret_val = SD_BLKSYS_PCS_G;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_G_PREF
  if (ret_val == SD_BLKSYS_PCS_G) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 1792, 2016) ) {
    ret_val = SD_BLKSYS_PCS_F;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_F_PREF
  if (ret_val == SD_BLKSYS_PCS_F) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 692, 871) ) {
    ret_val = SD_BLKSYS_PCS_E;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_E_PREF
  if (ret_val == SD_BLKSYS_PCS_E) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 539, 706) ) {
    ret_val = SD_BLKSYS_PCS_D;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_D_PREF
  if (ret_val == SD_BLKSYS_PCS_D) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 1, 193) ) {
    ret_val = SD_BLKSYS_PCS_C;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_C_PREF
  if (ret_val == SD_BLKSYS_PCS_C) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 81, 260) ) {
    ret_val = SD_BLKSYS_PCS_B;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_B_PREF
  if (ret_val == SD_BLKSYS_PCS_B) 
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 101, 300) ) {
    ret_val = SD_BLKSYS_PCS_A;
  }

  //************************************************

  if ( ret_val == SD_BLKSYS_MAX) 
  {
    SD_MSG_LOW( "Bad cdma_bc5_chan=%d",chan,0,0 );
  }

  return ret_val;

} /* sdprl_cdma_bc5_chan_to_blksys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc6_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 6 channel this function returns the frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The frequency block that is associated with the input CDMA band-class 6
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc6_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 4 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 1199 ) {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 6 CDMA channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc6_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc6_chan_to_blksys */


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc10_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 10 channel this function returns the frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The frequency block that is associated with the input CDMA band-class 10
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc10_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 10 CDMA channel for which to get the Cellular
            ** frequency block.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //INRANGE(chan, 0, 199) )   {
  if( chan <= 199 )
  {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 200, 399) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 400, 599) ) {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 600, 719) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 720, 919) ) {
    return SD_BLKSYS_PCS_E;
  }

  /* If we got here, the band-class 10 CDMA channel is out of range.
  */
  SD_MSG_ERROR( "Bad cdma_bc10_chan=%d",chan,0,0 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc10_chan_to_blksys () */


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc11_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 11 channel this function returns the frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The frequency block that is associated with the input CDMA band-class 11
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc11_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 11 CDMA channel for which to get the Cellular
            ** frequency block.
            */
)
{
  sd_blksys_e_type ret_val = SD_BLKSYS_MAX;
  //*********************************************
  if( INRANGE(chan, 472, 671) ) {
      ret_val = SD_BLKSYS_PCS_L;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_L_PREF
  if (ret_val == SD_BLKSYS_PCS_L) 
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 1536, 1715) ) {
      ret_val = SD_BLKSYS_PCS_K;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_K_PREF
  if (ret_val == SD_BLKSYS_PCS_K) 
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 211, 400) ) {
      ret_val = SD_BLKSYS_PCS_J;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_J_PREF
  if (ret_val == SD_BLKSYS_PCS_J) 
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 54, 230) ) {
      ret_val = SD_BLKSYS_PCS_I;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_I_PREF
  if (ret_val == SD_BLKSYS_PCS_I) 
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 692, 871) ) {
      ret_val = SD_BLKSYS_PCS_E;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_E_PREF
  if (ret_val == SD_BLKSYS_PCS_E) 
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 539, 706) ) {
    ret_val = SD_BLKSYS_PCS_D;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_D_PREF
  if (ret_val == SD_BLKSYS_PCS_D) 
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 1, 193) ) {
      ret_val = SD_BLKSYS_PCS_C;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_C_PREF
  if (ret_val == SD_BLKSYS_PCS_C) 
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 81, 260) ) {
      ret_val = SD_BLKSYS_PCS_B;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_B_PREF
  if (ret_val == SD_BLKSYS_PCS_B) 
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 101, 300) ) {
    ret_val = SD_BLKSYS_PCS_A;
  }

  //*********************************************

  if ( ret_val == SD_BLKSYS_MAX) 
  {
    SD_MSG_ERROR( "Bad cdma_bc11_chan=%d",chan,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ret_val;

} /* sdprl_cdma_bc11_chan_to_blksys () */


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc12_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 12 channel this function returns the frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The frequency block that is associated with the input CDMA band-class 12
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc12_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 12 CDMA channel for which to get the Cellular
            ** frequency block.
            */
)
{

  sd_blksys_e_type ret_val = SD_BLKSYS_MAX;

  if( chan<= 239 )
  {
    ret_val = SD_BLKSYS_PCS_C;
  }

  #ifdef FEATURE_BAND_CLASS_12_BLOCK_C_PREF
  if (ret_val == SD_BLKSYS_PCS_C) 
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 60, 179) ) {
    ret_val = SD_BLKSYS_PCS_B;
  }

  #ifdef FEATURE_BAND_CLASS_12_BLOCK_B_PREF
  if (ret_val == SD_BLKSYS_PCS_B) 
  {
    return ret_val;
  }
  #endif
  //*********************************************

  if( chan<= 239 )
  {
    ret_val = SD_BLKSYS_PCS_A;
  }

  /* If we got here, the band-class 12 CDMA channel is out of range.
  */
  if (ret_val == SD_BLKSYS_MAX) 
  {
    SD_MSG_ERROR( "Bad cdma_bc12_chan=%d",chan,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ret_val;

} /* sdprl_cdma_bc12_chan_to_blksys () */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_cdma_bc14_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 14 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 
  14 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc14_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 14 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 299 ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 300, 399) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 400, 699) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 700, 799) ) {
    return SD_BLKSYS_PCS_E;
  }

  if( INRANGE(chan, 800, 899) ) {
    return SD_BLKSYS_PCS_F;
  }

  if( INRANGE(chan, 900, 1199) ) {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 1200, 1299) ) {
    return SD_BLKSYS_PCS_G;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 1 CDMA channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc14_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc14_chan_to_blksys */


/*===========================================================================

FUNCTION sdprl_cdma_bc15_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 15 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 
  15 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc15_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 15 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 199 ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 200, 399) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 400, 499) ) {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 500, 599) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 600, 699) ) {
    return SD_BLKSYS_PCS_E;
  }

  if( INRANGE(chan, 700, 899) ) {
    return SD_BLKSYS_PCS_F;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 1 CDMA channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc15_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc15_chan_to_blksys */

/*===========================================================================

FUNCTION sdprl_cdma_bc16_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 16 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 
  15 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc16_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 16 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{

  if( INRANGE(chan, 140, 459) ) 
  {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 470, 799) ) 
  {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 800, 1129) ) 
  {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 1130, 2719) ) 
  {
    return SD_BLKSYS_PCS_D;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 1 CDMA channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc16_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc16_chan_to_blksys */

/*===========================================================================
FUNCTION sdprl_cdma_bc17_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 17 channel this function returns the PCS frequency
  block that is associated with this channel.
  This band class is only valid for HDR.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 
  17 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc17_chan_to_blksys(

        sd_chan_type              chan,  
            /* The band-class 17 CDMA channel for which to get the PCS
            ** frequency block.
            */
        sd_mode_e_type            mode
            /* To check if that the mode is HDR only, since
            ** this bandclass is valid only for HDR systems
            */
)
{
  if( mode !=SYS_SYS_MODE_HDR) {
    SD_MSG_LOW( "Bad bc17_chan=%d,%d",chan,mode,0 );
    return SD_BLKSYS_MAX;
  }

  if(INRANGE(chan, 140, 469) ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 470, 799) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 800, 1129) ) {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 1130, 1459) ) {
    return SD_BLKSYS_PCS_D;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 17 hdr channel is out of range.
  */
  SD_MSG_LOW( "Bad bc17_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc17_chan_to_blksys */

/*===========================================================================
FUNCTION sdprl_cdma_bc18_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 18 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 
  18 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc18_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 18 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if(chan <= 19) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 20, 119) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 120, 219) ) {
    return SD_BLKSYS_PSB;
  }

  if( INRANGE(chan, 220, 240) ) {
    return SD_BLKSYS_PSG;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 18 CDMA channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc18_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc18_chan_to_blksys */

/*===========================================================================
FUNCTION sdprl_cdma_bc19_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 19 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 
  19 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc19_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 19 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if(chan <= 119) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 120, 239) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 240, 360) ) {
    return SD_BLKSYS_PCS_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 19 CDMA channel is out of range.
  */
  SD_MSG_LOW( "Bad cdma_bc19_chan=%d",chan,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc19_chan_to_blksys */


#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

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
MMEXTN  sd_blksys_e_type          sdprl_map_chan_to_blksys(

        sd_mode_e_type            mode,
            /* The mode of system for which to get the block/system.
            */

        sd_band_e_type            band,
            /* The band of system for which to get the block/system.
            */

        sd_chan_type              chan
            /* The Channel of system for which to get the block/system.
            */
)
{
  switch( mode )
  {
    case SD_MODE_AMPS:
      if( band == SD_BAND_BC0 )
      {
        return sdprl_amps_bc0_sys_to_blksys( (sd_cell_sys_e_type) chan );
      }
      else if( band == SD_BAND_BC3 )
      {
        SD_CASE_NOT_IMP( SD_BAND_BC3 );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_CDMA_:
    case SD_MODE_HDR_:
      /*lint -e787 -e788*/
      switch( band )
        {
          case SD_BAND_BC0:   
#ifdef FEATURE_IS683A_450M_SUPPORT 
            if (sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_IS683A_450M_SUPPORT)) 
            { 
              /* Map PCS bands to 450M (BC5)*/ 
              return sdprl_cdma_bc5_chan_to_blksys( chan ); 
            } 
            else 
#endif 
              return sdprl_cdma_bc0_chan_to_blksys( chan );

          case SD_BAND_BC1:   
#ifdef FEATURE_IS683A_450M_SUPPORT 
            if (sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_IS683A_450M_SUPPORT)) 
            { 
              /* Map PCS bands to 450M (BC5)*/ 
              return sdprl_cdma_bc5_chan_to_blksys( chan ); 
            } 
            else 
#endif 
              return sdprl_cdma_bc1_chan_to_blksys( chan );

          case SD_BAND_BC3:   return sdprl_cdma_bc3_chan_to_blksys( chan );
          case SD_BAND_BC4:   return sdprl_cdma_bc4_chan_to_blksys( chan );
          case SD_BAND_BC5:   return sdprl_cdma_bc5_chan_to_blksys( chan );
          case SD_BAND_BC6:   return sdprl_cdma_bc6_chan_to_blksys( chan );
          case SD_BAND_BC10:  return sdprl_cdma_bc10_chan_to_blksys( chan );
          case SD_BAND_BC11:  return sdprl_cdma_bc11_chan_to_blksys( chan );
          case SD_BAND_BC12:  return sdprl_cdma_bc12_chan_to_blksys( chan );
          case SD_BAND_BC14:  return sdprl_cdma_bc14_chan_to_blksys( chan );
          case SD_BAND_BC15:  return sdprl_cdma_bc15_chan_to_blksys( chan );
          case SD_BAND_BC16:  return sdprl_cdma_bc16_chan_to_blksys( chan );
          case SD_BAND_BC17:  return sdprl_cdma_bc17_chan_to_blksys( chan,mode );
          case SD_BAND_BC18:  return sdprl_cdma_bc18_chan_to_blksys( chan );
          case SD_BAND_BC19:  return sdprl_cdma_bc19_chan_to_blksys( chan );
          default:
                  SD_MSG_LOW( "Bad band %d chan %d",band,chan,0 );
                  break;
        }  /*lint +e787 +e788 */ /* enum not used within switch */

       /* If supported band channel */
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GSM:
    case SD_MODE_GW:
      if( band < SD_BAND_MAX )
      {
        return SD_BLKSYS_PCS_A;
      }
      break;
    case SD_MODE_WCDMA:
      #if defined(FEATURE_WCDMA) || defined(FEATURE_GW)
      /*lint -e787 -e788 */
      switch ( band ) {

        case SD_BAND_WCDMA_I_IMT_2000:
          return sdprl_wcdma_i_imt2000_chan_to_blksys( chan );
        case SD_BAND_WCDMA_II_PCS_1900:
          return sdprl_wcdma_ii_pcs_1900_chan_to_blksys( chan );
        case SD_BAND_WCDMA_III_1700:
          return sdprl_wcdma_iii_1700_chan_to_blksys( chan );
        case SD_BAND_WCDMA_IV_1700:
          return sdprl_wcdma_iv_1700_chan_to_blksys( chan );
        case SD_BAND_WCDMA_V_850:
          return sdprl_wcdma_v_850_chan_to_blksys( chan );
        case SD_BAND_WCDMA_VI_800:
          return sdprl_wcdma_vi_800_chan_to_blksys( chan );
        default:
          break;
      } /*lint +e787 +e788 */ /* enum not used within switch */
      #endif /* (FEATURE_WCDMA) || (FEATURE_GW) */
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_MAX:
    case SD_MODE_WLAN:
    default:
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the mode/band combination is not supported.
  */
  SD_MSG_LOW( "Unsupported mode/band",0,0,0 );
  return SD_BLKSYS_MAX;

} /* sdprl_map_chan_to_blksys */




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
MMEXTN  sd_cell_sys_e_type        sdprl_map_amps_vc_to_sys(

        word                      voice_chan
            /* AMPS voice channel number to convert.
            */
)
{
  /* AMPS band-class 0 that belongs to system A.
  */
  if( INRANGE(voice_chan,    1,  333) ||   /* A */
      INRANGE(voice_chan,  667,  716) ||   /* A' */
      INRANGE(voice_chan,  991, 1023) )    /* A''*/
  {
    return SD_CELL_SYS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* AMPS band-class 0 that belongs to system B.
  */
  if( INRANGE(voice_chan,  334,  666) ||   /* B */
      INRANGE(voice_chan,  717,  799) )    /* B'*/
  {
    return SD_CELL_SYS_B;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the AMPS cellular channel is out of range.
  */
  SD_ASSERT( FALSE );
  return SD_CELL_SYS_MAX;

} /* sdprl_map_amps_vc_to_sys */




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== MISCELLANEOUS ================================
=============================================================================
=============================================================================
===========================================================================*/
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
MMEXTN  boolean                   sdprl_is_prl_active( void )
{

  sdprl_s_type    *sdprl_ptr = &sdprl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (sdprl_ptr->is_prl_active);

} /* sdprl_is_prl_active */




/*===========================================================================

FUNCTION sdprl_set_prl_active_status

DESCRIPTION
  Update PRL active status.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  Reutn TRUE if PRL is active otherwise return FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_set_prl_active_status(

        boolean                   status
        /* PRL activate status.
        */
)
{
  sdprl_s_type    *sdprl_ptr = &sdprl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_ptr->is_prl_active   = status;

} /* sdprl_set_prl_active_status */




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
MMEXTN  boolean                   sdprl_is_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference to be validated.
            */
)
{
  int           i;
  sdsr_s_type   *sr_ptr;

  #if defined(FEATURE_HYBRID_HDR) && defined(FEATURE_ADD_HDR_IF_IS683A_PRL)
#error code not present
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If PRL is inactive, skip the rest - return TRUE. */
  if( ! sdprl_is_prl_active() )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the PRL's acquisition list and look for a system that
  ** complies with the specified mode and band preference. If such system is
  ** found, return TRUE. Else, return FALSE.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(SDSR_PRL_LST, i)) != NULL; i++ )
  {
    if( sdss_sr_is_mode_pref(sr_ptr, mode_pref) &&
        sdss_sr_is_band_pref(sr_ptr, band_pref) )
    {
      return TRUE;
    }
  }

  #if defined(FEATURE_HYBRID_HDR) && defined(FEATURE_ADD_HDR_IF_IS683A_PRL)
#error code not present
  #endif

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_WLAN)
#error code not present
  #endif


  return FALSE;

} /* sdprl_is_supp_mode_band_pref */




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
MMEXTN  boolean                   sdprl_is_amps_backstop( void )
{
  sdprl_s_type      *sdprl_ptr = &sdprl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sdprl_ptr->nam_info[sdprl_ptr->curr_nam].is_amps_backstop;

} /* sdprl_is_amps_backstop */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_acq_rec_else_err_fatal

DESCRIPTION
  Get a specified acquisition record from the PRL's acquisition record table.
  If get operation fails, error fatal.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_get_acq_rec_else_err_fatal(

        prli_acq_rec_type         *prl_acq_rec_ptr,
            /* Pointer to a buffer where to build acquisition record.
            */

        const prli_sys_rec_type   *prl_sys_rec_ptr
            /* Pointer to a buffer where to build system record.
            */
)
{
  SD_ASSERT( prl_acq_rec_ptr != NULL );
  SD_ASSERT( prl_sys_rec_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the specified acquisition record from the PRL.
  */
  if( ! prl_extract_acq_rec(prl_acq_rec_ptr, prl_sys_rec_ptr->acq_index ) )
  {
    SD_ERR_FATAL("Error extracting acq_rec %d", prl_sys_rec_ptr->acq_index, 0, 0);
  }

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
  /* Populate plmn field.
  */
  if( prl_acq_rec_ptr->acq_type == PRL_ACQ_REC_GSM_GENERIC ||
      prl_acq_rec_ptr->acq_type == PRL_ACQ_REC_WCDMA_GENERIC ||
      prl_acq_rec_ptr->acq_type == PRL_ACQ_REC_GW_PREFERRED )
  {
    prl_acq_rec_ptr->plmn = prl_sys_rec_ptr->rec.plmn;
  }
  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || FEATURE_GW) */
} /* sdprl_get_acq_rec_else_err_fatal */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_home_sid_nid_read

DESCRIPTION
  Read the home SID/NID list of a specified NAM from NV into its local RAM
  buffer.

  NOTE! This function only returns after the home SID/NID list of the
  specified NAM is read from NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if home SID/NID list is read successfully. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_home_sid_nid_read(

        sd_nam_e_type             nam
            /* NAM for which to read the home SID/NID list.
            */
)
{
  sdprl_s_type      *sdprl_ptr = &sdprl;

  sdnv_data_s_type  buf;
  /*lint -save -e826  buf Suspicious pointer-to-pointer conversion */
  nv_item_type      *item_ptr = (nv_item_type*) &buf;

  unsigned int      i;
  sdprl_nam_s_type  *nam_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[nam];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_SSPR_ENHANCEMENTS

  /* Read from NV the home SID/NID list. If read fails, return FALSE.
  */
  item_ptr->home_sid_nid.nam = (byte) nam;
  if( ! sdnv_read(NV_HOME_SID_NID_I, item_ptr) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the SID/NIDs from the NV item to the local NAM information
  ** structure.
  */
  for( i=0;
       i < ARR_SIZE(nam_ptr->home_sid_nid) &&
       i < ARR_SIZE(item_ptr->home_sid_nid.pair);
       i++ )
  {
    nam_ptr->home_sid_nid[i].sid = item_ptr->home_sid_nid.pair[i].sid;
    nam_ptr->home_sid_nid[i].nid = item_ptr->home_sid_nid.pair[i].nid;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #else /* FEATURE_SSPR_ENHANCEMENTS */

  /* Read from NV the home SID/NID list. If read fails, return FALSE.
  */
  item_ptr->sid_nid.nam = (byte) nam;
  if( ! sdnv_read(NV_SID_NID_I, item_ptr) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the SID/NIDs from the NV item to the local NAM information
  ** structure.
  */
  for( i=0;
       i < ARR_SIZE(nam_ptr->home_sid_nid) &&
       i < ARR_SIZE(item_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX]);
       i++ )
  {
    nam_ptr->home_sid_nid[i].sid =
                            item_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][i].sid;
    nam_ptr->home_sid_nid[i].nid =
                            item_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][i].nid;
  }

  #endif /* FEATURE_SSPR_ENHANCEMENTS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the SID/NID list is not completely full, mark the end of the list
  ** with SD_WILDCARD_SID and SD_WILDCARD_NID.
  */
  if( i < ARR_SIZE(nam_ptr->home_sid_nid) )
  {
    nam_ptr->home_sid_nid[i].sid = SD_WILDCARD_SID;
    nam_ptr->home_sid_nid[i].nid = SD_WILDCARD_NID;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read AMPS home SID.
  */
  item_ptr->analog_home_sid.nam = (byte) nam;
  if( ! sdnv_read(NV_ANALOG_HOME_SID_I, item_ptr) )
  {
    return FALSE;
  }

  /* Copy AMPS home SID to the local NAM info structure.
  */
  nam_ptr->amps_home_sid = item_ptr->analog_home_sid.sid;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the home SID/NID list is successfully read from NV -
  ** return TRUE.
  */
  return TRUE;

} /* sdprl_home_sid_nid_read */
  /*lint -restore */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_lock_sid_nid_read

DESCRIPTION
  Read the locked SID/NID list of a specified NAM from NV into its local RAM
  buffer.

  NOTE! This function only returns after the locked SID/NID list of the
  specified NAM is read from NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if locked SID/NID list is read successfully. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_lock_sid_nid_read(
        /*lint -esym(715, nam) */ /* nam not referenced*/
        sd_nam_e_type             nam
            /* NAM for which to read the locked SID/NID list.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_s_type      *sdprl_ptr = &sdprl;

  #ifdef FEATURE_CDMA_800
  /*lint -save -e826  buf Suspicious pointer-to-pointer conversion */
  sdnv_data_s_type  buf;
  nv_item_type      *item_ptr = (nv_item_type*) &buf;
  /*lint -restore */
  #endif

  unsigned int      i=0;
  sdprl_nam_s_type  *nam_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[nam];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CDMA_800
  /* Read from NV the locked SID/NID list. If read fails, return FALSE.
  */
  item_ptr->sid_nid_lock.nam = (byte) nam;

  if( ! sdnv_read(NV_SID_NID_LOCK_I, item_ptr) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the locked SID/NIDs from the NV item to the local NAM information
  ** structure.
  */
  for( i=0;
       i < ARR_SIZE(nam_ptr->lock_sid_nid) &&
       i < ARR_SIZE(item_ptr->sid_nid_lock.pair);
       i++ )
  {
    nam_ptr->lock_sid_nid[i].sid = item_ptr->sid_nid_lock.pair[i].sid;
    nam_ptr->lock_sid_nid[i].nid = item_ptr->sid_nid_lock.pair[i].nid;
  }
  #endif /* FEATURE_CDMA_800 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mark unused entries with SD_WILDCARD_SID and SD_WILDCARD_NID.
  */
  for( ; i < ARR_SIZE(nam_ptr->lock_sid_nid); i++ )
  {
    nam_ptr->lock_sid_nid[i].sid = SD_WILDCARD_SID;
    nam_ptr->lock_sid_nid[i].nid = SD_WILDCARD_NID;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the locked SID/NID list is successfully read from NV -
  ** return TRUE.
  */
  return TRUE;

} /* sdprl_lock_sid_nid_read */ /*lint +esym(715, nam) */



/* <EJECT> */
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
/*lint -esym(715, lst) */ /* para not accessed */
(
        sdsr_e_type               lst
          /* Append the WLAN user systems in this lst.
          */
)
{
  /*lint -esym(550, net_status) */ /* Symbol not accessed */
  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

} /* sdprl_read_wlan_priv_lst */
/*lint +esym(715, lst) */ /* para not accessed */
/*lint +esym(550, net_status) */ /* Symbol not accessed */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_plmn_is_net_select_mode_valid

DESCRIPTION
  Check if network selection mode is valid.
  system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the network selection mode is valid.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_is_net_select_mode_valid(

        sd_net_select_mode_e_type net_select_mode,
        /* The network selection mode to be checked
        */

        boolean                   is_plmn_id_undefined
        /* Indicate if plmn id is undefined.
        */
)
{

  if( is_plmn_id_undefined )
  {
    if( (net_select_mode == SD_NET_SELECT_MODE_LIMITED)
                            ||
           (net_select_mode == SD_NET_SELECT_MODE_AUTO)
                            ||
           (net_select_mode == SD_NET_SELECT_MODE_HPLMN)
                            ||
           (net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS)
      )
    {
      return TRUE;
    }
  }
  else
  {
    if( (net_select_mode == SD_NET_SELECT_MODE_AUTO)
                         ||
        (net_select_mode == SD_NET_SELECT_MODE_PRL_SYS)
      )
    {
      return TRUE;
    }
  }
  return FALSE;
}/*sdprl_plmn_is_net_select_mode_valid*/
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_mru_read

DESCRIPTION
  Read the MRU table of a specified NAM from NV into its local RAM buffer.

  NOTE! This function only returns after the MRU table of the specified NAM
  is read from NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if MRU is read successfully. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_mru_read(

        sd_nam_e_type             nam
            /* NAM for which to read the MRU.
            */
)
{
  sdprl_s_type                    *sdprl_ptr = &sdprl;

  /*lint -save -e826 Suspicious pointer-to-pointer conversion */
  sdnv_data_s_type                buf;
  nv_item_type                    *item_ptr = (nv_item_type*) &buf;
  /*lint -restore */

  unsigned int                    i;
  sd_blksys_e_type                blksys;
  sd_sys_s_type                   sys;
  sdprl_nam_s_type                *nam_ptr;
  sd_mru_table_entry_u_type       sd_mru_entry;
  sd_band_e_type                  band;
  boolean                         is_valid;
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );
  /* Generic assert for num entries
  */
  SD_ASSERT( (SDPRL_MRU_NUM_MAIN_ENTRIES +
              SDPRL_MRU_NUM_HYBR_ENTRIES  +
              SDPRL_MRU_NUM_USER_ENTRIES) ==  NV_MRU_TABLE_SIZE );
  SD_ASSERT( ARR_SIZE(sdprl_ptr->nam_info[nam].mru_tbl) == NV_MRU_TABLE_SIZE );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the MRU of the specified NAM. If read operation fails,
  ** return FALSE.
  */
  item_ptr->mru3_table.nam = (byte) nam;
  if( ! sdnv_read(NV_MRU3_TABLE_I, item_ptr) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[nam];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the MRU table that was read from NV and copy valid entries
  ** into the MRU table that SD holds in RAM. Note that SD_MODE_INACT
  ** designate the last valid entry in the MRU.
  */

  for( i=0;

       i < ARR_SIZE(nam_ptr->mru_tbl) &&
       i < ARR_SIZE(item_ptr->mru3_table.entry);

       i++ )
  {
    is_valid = TRUE;

    memcpy( (byte *)&sd_mru_entry,
            (byte *)&item_ptr->mru3_table.entry[i],
            sizeof(sd_mru_entry) );

    /* Mode is the first byte in the union data structure.
    */
    sys.mode = (sd_mode_e_type) sd_mru_entry.mode;

    switch ( sys.mode )
    {
      case SD_MODE_CDMA:
      case SD_MODE_HDR:

        band = (sd_band_e_type) sd_mru_entry.cdma.band;

        sys.chan = (sd_chan_type) sd_mru_entry.cdma.channel;

        blksys = sdprl_map_chan_to_blksys( sys.mode,
                                           band,
                                           sys.chan );
        if( blksys == SD_BLKSYS_MAX )
        {
          is_valid = FALSE;
        }

        if( !sdss_is_supp_mode_band( sys.mode,
                                     sdss_map_band_to_band_pref(band)))
        {
          is_valid = FALSE;
        }
        break;

      case SD_MODE_AMPS:

        band = (sd_band_e_type) sd_mru_entry.cellular.band;

        sys.chan = (sd_chan_type) sd_mru_entry.cellular.sys;

        blksys = sdprl_map_chan_to_blksys( sys.mode,
                                           band,
                                           sys.chan );
        if( blksys == SD_BLKSYS_MAX )
        {
          is_valid = FALSE;
        }

        if( !sdss_is_supp_mode_band( sys.mode,
                                     sdss_map_band_to_band_pref(band)))
        {
          is_valid = FALSE;
        }
        break;

    case SD_MODE_GSM:

        #ifdef FEATURE_GSM
#error code not present
      #endif /* FEATURE_GSM */
        break;

      case SD_MODE_WCDMA:

        #ifdef FEATURE_WCDMA
#error code not present
        #endif /* FEATURE_WCDMA */
        break;

      case SD_MODE_GW:

        #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
        #endif /* FEATURE_GSM || FEATURE_WCDMA */
        break;

      case SD_MODE_NONE:
      case SD_MODE_INACT:
      case SD_MODE_GPS:
      case SD_MODE_MAX:
      case SD_MODE_WLAN:
      default:

        is_valid = FALSE;

        break;
    }

    /* Store only valid MRU entry into local RAM.
    */
    if( is_valid )
    {
      nam_ptr->mru_tbl[i] = sd_mru_entry;
    }
    else
    {
      SD_MSG_LOW( "Bad MRU NAM=%d, entry=%d", nam, i, 0 );
      nam_ptr->mru_tbl[i].mode = (byte) SD_MODE_INACT;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, MRU table is successfully read from NV - return TRUE.
  */
  return TRUE;

} /* sdprl_mru_read */



/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_wlan_mru_read

DESCRIPTION
  Read the WLAN MRU table from EFS.


DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_wlan_mru_read(

        void
)
{
  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

} /* sdprl_wlan_mru_read */


/*===========================================================================

FUNCTION sdprl_custom_home_read

DESCRIPTION
  Read from NV the bitmap of values to be considered as custom homes in the
  roaming indicator field of the PRL.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the bitmap is read successfully. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_custom_home_read( void )
{
  byte bit0;
  byte bit1;

  /*lint -save -e826 Suspicious 'roam_ind_custom_home' pointer-to-pointer
  ** conversion.
  */
  if( !sdnv_read(NV_ROAM_IND_CUSTOM_HOME_I,
                 (nv_item_type*) roam_ind_custom_home) )
  {
    /* If the read from NV fails, use the default custom home bitmap and
    ** write it back to NV.
    */
    memcpy( roam_ind_custom_home, default_roam_ind_custom_home,
            sizeof(roam_ind_custom_home) );

    (void)sdnv_write(NV_ROAM_IND_CUSTOM_HOME_I,
               (nv_item_type*) roam_ind_custom_home);

  }
  /*lint -restore */

  /* Swap the value at bit positions 0 and 1 since SD swaps the roaming
  ** indicator values 0 and 1 after reading the PRL.
  */
  bit0 = roam_ind_custom_home[0] & BM(0);
  bit1 = (roam_ind_custom_home[0] & BM(1)) >> 1;
  roam_ind_custom_home[0] &= 0xFC;
  roam_ind_custom_home[0] |= (byte) (bit0 << 1) | bit1;
}

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_mru_write

DESCRIPTION
  Write the MRU table of a specified NAM from its RAM buffer into NV.

  NOTE! This function only returns after the MRU table of the specified NAM
  is written to NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_mru_write(

        sd_nam_e_type             nam
            /* NAM for which to write the MRU.
            */
)
{
  sdprl_s_type                    *sdprl_ptr = &sdprl;

  /*lint -save -e826 Suspicious 'buf' pointer-to-pointer conversion */
  sdnv_data_s_type                buf;
  nv_item_type                    *item_ptr = (nv_item_type*) &buf;
  /*lint -restore */

  unsigned int                    i;
  sdprl_nam_s_type                *nam_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  SD_MSG_LOW( "MRU write, NAM=%d", nam, 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[nam];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the MRU table that SD holds in RAM and copy valid entries
  ** into the NV item buffer to be written to NV.
  */
  for( i=0;
       i < ARR_SIZE(item_ptr->mru3_table.entry) &&
       i < ARR_SIZE(nam_ptr->mru_tbl) ;
       i++ )
  {

    memcpy( (byte *)&item_ptr->mru3_table.entry[i],
            (byte *) &nam_ptr->mru_tbl[i],
            sizeof(item_ptr->mru3_table.entry[i])
          );

    switch(nam_ptr->mru_tbl[i].mode)
    {
      case SYS_SYS_MODE_CDMA:
        SD_MSG_LOW("MRU Entry[%d] : Mode %d Channel %d",i,
                                                        nam_ptr->mru_tbl[i].mode,
                                                        nam_ptr->mru_tbl[i].cdma.channel);
        break;
      case SYS_SYS_MODE_HDR:
        SD_MSG_LOW("MRU Entry[%d] : Mode %d Channel %d",i,
                                                        nam_ptr->mru_tbl[i].mode,
                                                        nam_ptr->mru_tbl[i].hdr.channel);
        break;
      default:
        SD_MSG_LOW("MRU Entry[%d] : Mode %d ",i,
                                              nam_ptr->mru_tbl[i].mode,
                                              0);
    }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write to NV the MRU of the specified NAM.
  */
  item_ptr->mru3_table.nam = (byte) nam;

  (void)sdnv_write( NV_MRU3_TABLE_I, item_ptr );

} /* sdprl_mru_write */


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_wlan_mru_write

DESCRIPTION
  Write the WLAN MRU table into EFS.


DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_wlan_mru_write(

        void
)
{
  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

} /* sdprl_wlan_mru_write */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_mru_cpy

DESCRIPTION
  Copy the MRU table that corresponds to a specified NAM selection from its
  local RAM buffer into a specified system record list.

  Add HDR entries to the hdr_list provided.

  Add USER entries to the user_list provided.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_mru_cpy(

        sdsr_e_type               list,
            /* System record list into which to copy the MRU entries.
            */

        sdsr_e_type               hybr_list,
            /* System record list into which to copy the MRU entries.
            */

        sdsr_e_type               user_list,
            /* System record list from which to get the user MRU entries.
            */

        sd_nam_e_type             nam
            /* NAM selection for which to copy the MRU.
            */
)
{
  sdprl_s_type              *sdprl_ptr = &sdprl;

  unsigned int              i;
  sdsr_ref_type             sr_ref;
  sdprl_nam_s_type          *nam_ptr;
  sdsr_s_type               *sdsr_ptr;

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) */
  sd_ss_band_pref_e_type    band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[nam];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the systems from the MRU table in the system record table and set
  ** the list to reference the stored entries.
  ** Make sure MRU is not bigger than system record table??
  */
  sdsr_list_clr( list );

  for( i= SDPRL_MRU_MAIN_ENTRIES_OFFSET;
       i < SDPRL_MRU_MAIN_ENTRIES_OFFSET + SDPRL_MRU_NUM_MAIN_ENTRIES  &&
       i < ARR_SIZE(nam_ptr->mru_tbl);
       i++
     )
  {
    if( nam_ptr->mru_tbl[i].mode != (byte) SD_MODE_INACT )
    {

      switch ( nam_ptr->mru_tbl[i].mode )
      {

        case SD_MODE_CDMA:

          band_pref = sdss_map_band_to_band_pref(
                              (sd_band_e_type)nam_ptr->mru_tbl[i].cdma.band);
          sr_ref = sdsr_tbl_store2( SD_MODE_CDMA,
                                    band_pref,
                                    nam_ptr->mru_tbl[i].cdma.channel );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);

          if( sdsr_ptr == NULL)
          {
            SD_MSG_LOW("sdsr_ptr NULL",0,0,0);
            break;
          }

          sdsr_ptr->sid.is95.sid = nam_ptr->mru_tbl[i].cdma.sid;

          sdsr_ptr->sid.is95.nid = nam_ptr->mru_tbl[i].cdma.nid;

          sdsr_ptr->sid_expected.prm.is95.sid = sdsr_ptr->sid.is95.sid;

          sdsr_ptr->sid_expected.prm.is95.nid = sdsr_ptr->sid.is95.nid;

          sdsr_list_app_ref( list, sr_ref );

          break;

        case SD_MODE_AMPS:

          band_pref = sdss_map_band_to_band_pref(
                          (sd_band_e_type)nam_ptr->mru_tbl[i].cellular.band);
          sr_ref = sdsr_tbl_store2( SD_MODE_AMPS,
                                    band_pref,
                                    nam_ptr->mru_tbl[i].cellular.sys );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);
          if( sdsr_ptr == NULL)
          {
            SD_MSG_LOW("sdsr_ptr NULL",0,0,0);
            break;
          }

          sdsr_ptr->sid.is95.sid = nam_ptr->mru_tbl[i].cellular.sid;

          sdsr_ptr->sid_expected.prm.is95.sid = sdsr_ptr->sid.is95.sid;

          sdsr_ptr->sid_expected.prm.is95.nid = sdsr_ptr->sid.is95.nid;

          sdsr_list_app_ref( list, sr_ref );

          break;

        case SD_MODE_HDR:

          #ifdef FEATURE_HDR
#error code not present
          #endif

          break;

        case SD_MODE_GSM:

        #if defined(FEATURE_GSM)
#error code not present
        #endif /* defined(FEATURE_GSM) */
          break;

        case SD_MODE_WCDMA:

        #if defined(FEATURE_WCDMA)
#error code not present
      #endif /* defined(FEATURE_WCDMA) */
        break;

      case SD_MODE_GW:

         #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
        #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
          break;

        case SD_MODE_GPS:
        default:
          break;
       }

      } /* if */
    } /* for () */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the Hybrid systems from the MRU table in the system record table
  ** and set the list to reference the stored entries.
  ** Make sure MRU is not bigger than system record table??
  */
  sdsr_list_clr( hybr_list );

  for( i = SDPRL_MRU_HYBR_ENTRIES_OFFSET;
       i < (SDPRL_MRU_HYBR_ENTRIES_OFFSET + SDPRL_MRU_NUM_HYBR_ENTRIES) &&
       i < ARR_SIZE(nam_ptr->mru_tbl) &&
       hybr_list != SDSR_MAX_LST ;
       i++
     )
  {
    if( nam_ptr->mru_tbl[i].mode != (byte) SD_MODE_INACT )
    {
      switch ( nam_ptr->mru_tbl[i].mode )
      {

        case SD_MODE_CDMA:

          band_pref = sdss_map_band_to_band_pref(
                              (sd_band_e_type)nam_ptr->mru_tbl[i].cdma.band);
          sr_ref = sdsr_tbl_store2( SD_MODE_CDMA,
                                    band_pref,
                                    nam_ptr->mru_tbl[i].cdma.channel );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);

          if( sdsr_ptr == NULL )
          {
            SD_MSG_ERROR("sdsr_ptr null",0,0,0);
            break;
          }

          sdsr_ptr->sid.is95.sid = nam_ptr->mru_tbl[i].cdma.sid;

          sdsr_ptr->sid.is95.nid = nam_ptr->mru_tbl[i].cdma.nid;

          sdsr_ptr->sid_expected.prm.is95.sid = sdsr_ptr->sid.is95.sid;

          sdsr_ptr->sid_expected.prm.is95.nid = sdsr_ptr->sid.is95.nid;

          sdsr_list_app_ref( hybr_list, sr_ref );

          break;

        case SD_MODE_AMPS:

          band_pref = sdss_map_band_to_band_pref(
                          (sd_band_e_type)nam_ptr->mru_tbl[i].cellular.band);
          sr_ref = sdsr_tbl_store2( SD_MODE_AMPS,
                                    band_pref,
                                    nam_ptr->mru_tbl[i].cellular.sys );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);

          if( sdsr_ptr == NULL)
          {
            SD_MSG_LOW("sdsr_ptr NULL",0,0,0);
            break;
          }

          sdsr_ptr->sid.is95.sid = nam_ptr->mru_tbl[i].cellular.sid;

          sdsr_ptr->sid_expected.prm.is95.sid = sdsr_ptr->sid.is95.sid;

          sdsr_ptr->sid_expected.prm.is95.nid = sdsr_ptr->sid.is95.nid;

          sdsr_list_app_ref( hybr_list, sr_ref );

          break;

        case SD_MODE_HDR:

          #ifdef FEATURE_HDR
#error code not present
          #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
          break;

        case SD_MODE_GPS:
        default:
          break;

      }

    } /* if */
  } /* for () */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the user systems from the MRU table in the system record table and set
  ** the list to reference the stored entries.
  ** Make sure MRU is not bigger than system record table??
  */
  sdsr_list_clr( user_list );


  for( i = SDPRL_MRU_USER_ENTRIES_OFFSET;
       i < (SDPRL_MRU_USER_ENTRIES_OFFSET + SDPRL_MRU_NUM_USER_ENTRIES) &&
       i < ARR_SIZE(nam_ptr->mru_tbl) &&
       user_list != SDSR_MAX_LST ;
       i++
     )
  {
    if( nam_ptr->mru_tbl[i].mode != (byte) SD_MODE_INACT )
    {

      switch ( nam_ptr->mru_tbl[i].mode )
      {

        case SD_MODE_GSM:

          #if defined(FEATURE_GSM)
#error code not present
          #endif /* defined(FEATURE_GSM) */
          break;

        case SD_MODE_WCDMA:

          #if defined(FEATURE_WCDMA)
#error code not present
        #endif /* defined(FEATURE_WCDMA) */
        break;

      case SD_MODE_GW:

          #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
          #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
          break;

        case SD_MODE_GPS:
        default:
          break;
       }

      } /* if */
    } /* for () */


} /* sdprl_mru_cpy */



/* <EJECT> */

/*lint -e662 */
#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */
/* lint +e662 */

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_mru_save

DESCRIPTION
  Save the systems that are referenced by a specified system record list into
  the local MRU RAM buffer of a specified NAM selection.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_mru_save(

        sdsr_e_type               list,
            /* System record list from which to get the MRU entries.
            */

        sdsr_e_type               hybr_list,
            /* System record list from which to get the HDR MRU entries.
            */

        sdsr_e_type               user_list,
            /* System record list from which to get the user MRU entries.
            */

        sd_nam_e_type             nam
            /* NAM selection for which to save the MRU.
            */
)
{
  sdprl_s_type      *sdprl_ptr = &sdprl;
  unsigned int      i = 0;
  int               j = 0;
  sdsr_s_type       *sr_ptr;
  sdprl_nam_s_type  *nam_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );
  SD_ASSERT_ENUM_IS_INRANGE( hybr_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( user_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[nam];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if there something to update the MRU table
  */
  if ( sdsr_list_cnt(list) == 0 &&
       sdsr_list_cnt(hybr_list) == 0 &&
       sdsr_list_cnt(user_list) == 0
     )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the entries SD_MODE_INACT for all.
  */
  for( i = 0;
       i < ARR_SIZE(nam_ptr->mru_tbl);
       i++
     )
  {
    nam_ptr->mru_tbl[i].mode = (byte) SD_MODE_INACT;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Step through the specified system record list and store systems into the
  ** MRU RAM buffer of the specified NAM.
  */
  for( i=SDPRL_MRU_MAIN_ENTRIES_OFFSET, j =0;
       (sr_ptr=sdsr_list_get_ptr(list, j)) != NULL &&
       i < ARR_SIZE(nam_ptr->mru_tbl) &&
       i < SDPRL_MRU_MAIN_ENTRIES_OFFSET + SDPRL_MRU_NUM_MAIN_ENTRIES;
       i++, j++ )
  {
    nam_ptr->mru_tbl[i].mode = (byte) sr_ptr->sys.mode;

    switch ( sr_ptr->sys.mode )
    {
      case SD_MODE_CDMA:

        nam_ptr->mru_tbl[i].cdma.mode = (byte) sr_ptr->sys.mode;
        nam_ptr->mru_tbl[i].cdma.band =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

        nam_ptr->mru_tbl[i].cdma.channel = (word) sr_ptr->sys.chan;

        nam_ptr->mru_tbl[i].cdma.sid = sr_ptr->sid.is95.sid;

        nam_ptr->mru_tbl[i].cdma.nid = sr_ptr->sid.is95.nid;

        break;

      case SD_MODE_AMPS:

        nam_ptr->mru_tbl[i].cellular.mode = (byte) sr_ptr->sys.mode;
        nam_ptr->mru_tbl[i].cellular.band =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

        nam_ptr->mru_tbl[i].cellular.sys = sr_ptr->sys.chan;

        nam_ptr->mru_tbl[i].cellular.sid = sr_ptr->sid.is95.sid;

        break;

      case SD_MODE_HDR:

        #ifdef FEATURE_HDR
#error code not present
        #endif

        break;

      case SD_MODE_GSM:

        #if defined(FEATURE_GSM)
#error code not present
        #endif /* defined(FEATURE_GSM) */
        break;

      case SD_MODE_WCDMA:

        #if defined(FEATURE_WCDMA)
#error code not present
        #endif /* defined(FEATURE_WCDMA) */
        break;

      case SD_MODE_GW:

        #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
        #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
        break;

      case SD_MODE_NONE:
      case SD_MODE_INACT:
      case SD_MODE_WLAN:
      case SD_MODE_GPS:
      case SD_MODE_MAX:
      default:
        break;

    } /* switch() */

  } /* for () */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the specified system record HDR list and store systems into the
  ** MRU RAM buffer of the specified NAM.
  ** At least one entry in MRU is reserved for HDR hybrid.
  */
  if ( sdsr_list_cnt(hybr_list) != 0 )
  {
    SD_ASSERT( SDPRL_MRU_NUM_HYBR_ENTRIES < ARR_SIZE(nam_ptr->mru_tbl));
    for( i = SDPRL_MRU_HYBR_ENTRIES_OFFSET, j=0;
         (sr_ptr=sdsr_list_get_ptr(hybr_list, j)) != NULL &&
          i < ARR_SIZE(nam_ptr->mru_tbl) &&
          i < SDPRL_MRU_HYBR_ENTRIES_OFFSET + SDPRL_MRU_NUM_HYBR_ENTRIES;
         j++
       )
    {
      nam_ptr->mru_tbl[i].mode = (byte) sr_ptr->sys.mode;
      /*lint -e788 */ /* enum not used within switch */
      switch ( sr_ptr->sys.mode )
      {
        case SD_MODE_CDMA:

          nam_ptr->mru_tbl[i].cdma.mode = (byte) sr_ptr->sys.mode;
          nam_ptr->mru_tbl[i].cdma.band =
                         (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

          nam_ptr->mru_tbl[i].cdma.channel = (word) sr_ptr->sys.chan;

          nam_ptr->mru_tbl[i].cdma.sid = sr_ptr->sid.is95.sid;

          nam_ptr->mru_tbl[i].cdma.nid = sr_ptr->sid.is95.nid;
          i++;
          break;

        case SD_MODE_AMPS:

          nam_ptr->mru_tbl[i].cellular.mode = (byte) sr_ptr->sys.mode;
          nam_ptr->mru_tbl[i].cellular.band =
                         (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

          nam_ptr->mru_tbl[i].cellular.sys = sr_ptr->sys.chan;

          nam_ptr->mru_tbl[i].cellular.sid = sr_ptr->sid.is95.sid;
          i++;
          break;

        case SD_MODE_HDR:

          #ifdef FEATURE_HDR
#error code not present
          #endif
          break;

        case SD_MODE_GSM:

          #if defined(FEATURE_GSM)
#error code not present
          #endif /* defined(FEATURE_GSM) */
          break;

        case SD_MODE_WCDMA:

          #if defined(FEATURE_WCDMA)
#error code not present
          #endif /* defined(FEATURE_WCDMA) */
          break;

        case SD_MODE_GW:

          #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
          nam_ptr->mru_tbl[i].gw.mode = (byte) sr_ptr->sys.mode;

          sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                           &nam_ptr->mru_tbl[i].gw.band_pref );

          nam_ptr->mru_tbl[i].gw.net_select_mode =
                              (byte) sr_ptr->sid_expected.net_select_mode;
          /* Store plmn id populated from stack. */
          nam_ptr->mru_tbl[i].gw.plmn_id =
                                             sr_ptr->sid_expected.prm.plmn.plmn_id;

          /* Store acquired mode.*/
          nam_ptr->mru_tbl[i].gw.acq_mode = (byte) sr_ptr->sid.gw.mode;
          i++;
          #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
          break;

        case SD_MODE_NONE:
        case SD_MODE_INACT:
        case SD_MODE_WLAN:
        case SD_MODE_GPS:
        case SD_MODE_MAX:
        default:
          break;

      } /* switch() */

    } /* for () */


  } /* if() */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the specified system record USER list and store systems into the
  ** MRU RAM buffer of the specified NAM.
  ** At least one entry in MRU is reserved for USER list.
  */
  if ( sdsr_list_cnt(user_list) != 0 )
  {
    SD_ASSERT( SDPRL_MRU_NUM_USER_ENTRIES < ARR_SIZE(nam_ptr->mru_tbl));
    for( i = SDPRL_MRU_USER_ENTRIES_OFFSET, j=0;
         (sr_ptr=sdsr_list_get_ptr(user_list, j)) != NULL &&
          i < ARR_SIZE(nam_ptr->mru_tbl) &&
          i < SDPRL_MRU_USER_ENTRIES_OFFSET + SDPRL_MRU_NUM_USER_ENTRIES;
         j++
       )
    {
      nam_ptr->mru_tbl[i].mode = (byte)sr_ptr->sys.mode;
      /*lint -e788 */   /* enum not used within switch */
      switch ( sr_ptr->sys.mode )
      {

        case SD_MODE_GSM:

          #if defined(FEATURE_GSM)
#error code not present
          #endif /* defined(FEATURE_GSM) */
          break;

        case SD_MODE_WCDMA:

          #if defined(FEATURE_WCDMA)
#error code not present
          #endif /* defined(FEATURE_WCDMA) */
          break;

        case SD_MODE_GW:

          #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
          #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
          break;

        default:
          break;

      } /* switch() */      /*lint +e788 */

    } /* for () */


  } /* if() */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdprl_mru_save */



/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_wlan_mru_save

DESCRIPTION
  Save the systems that are referenced by a specified system record list into
  the local WLAN MRU RAM buffer.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_wlan_mru_save(
/*lint -esym(715, list) */ /* Symbol not used*/

        sdsr_e_type               list
            /* System record list from which to get the MRU entries.
            */

)
{
  #ifdef FEATURE_WLAN
#error code not present
  #endif

} /* sdprl_mru_save */
/*lint +esym(715, list) */ /* Symbol not used*/


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_acq_rec_app

DESCRIPTION
  Append the acquisition record that is pointer to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        sdprl_append_e_type       append_type
            /* Indicates whether the AMPS portion of an acquisition record of
            ** type PRL_ACQ_REC_CELLULAR_GENERIC should be appended.
            */
)
{
  word                pch_a = sdprl.nam_info[sdprl.curr_nam].pch_a;
  word                pch_b = sdprl.nam_info[sdprl.curr_nam].pch_b;
  word                sch_a = sdprl.nam_info[sdprl.curr_nam].sch_a;
  word                sch_b = sdprl.nam_info[sdprl.curr_nam].sch_b;

  word                jtacs_pch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_a;
  word                jtacs_pch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_b;
  word                jtacs_sch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_a;
  word                jtacs_sch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_b;

  int                 i;
  int                 j;

  word                sys[2]    = { NO_CHAN, NO_CHAN };
  word                chans[4]  = { NO_CHAN, NO_CHAN, NO_CHAN, NO_CHAN };


  #if defined(FEATURE_GSM) || defined(FEATURE_GW)
  sd_band_e_type      band;
  sd_ss_band_pref_e_type band_pref;
  sd_sid_plmn_wlan_s_type sid_exp;
  #endif

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GW)
  sd_ss_band_pref_e_type multi_band_pref;
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list1, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list2, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list3, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT( acq_rec_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !acq_rec_ptr->is_valid )
  {
    return;
  }

  switch( acq_rec_ptr->acq_type )
  {
    case PRL_ACQ_REC_CELLULAR_ANALOG:

      switch (acq_rec_ptr->type.cellular_analog.a_b)
      {
        case PRL_AB_SYSTEM_A:
          sys[0] = (word)SD_CELL_SYS_A;
          break;

        case PRL_AB_SYSTEM_B:
          sys[0] = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          sys[0] = (word)SD_CELL_SYS_A;
          sys[1] = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_RESERVED:
        default:
          SD_CASE_ERR( "a_b",0,0,0 );
          break;
      }

      /* Add AMPS systems to list.
      */
      for( i=0; i < (int) ARR_SIZE(sys) && sys[i] != NO_CHAN; i++ )
      {
        sdsr_list_store_and_app2( list1,
                                  list2,
                                  list3,
                                  SD_MODE_AMPS,
                                  sdss_map_band_to_band_pref(SD_BAND_CELL),
                                  sys[i],
                                  NULL,
                                  sys_idx );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_STD:

      /* Add the channels for the specified Cellular CDMA system(s) to the
      ** list
      */
      switch( acq_rec_ptr->type.cellular_cdma.a_b )
      {
        case PRL_AB_SYSTEM_A:

          switch( acq_rec_ptr->type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR("pri_sec",0,0,0);
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:

          switch (acq_rec_ptr->type.cellular_cdma.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_b;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR("pri_sec",0,0,0);
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:

          switch (acq_rec_ptr->type.cellular_cdma.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              chans[1] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              chans[2] = pch_b;
              chans[3] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR("pri_sec",0,0,0);
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_CASE_ERR("a_b",0,0,0);
          break;

      } /* end switch (acq_rec_ptr->type.cellular_cdma.a_b) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        sdsr_list_store_and_app2( list1,
                                  list2,
                                  list3,
                                  SD_MODE_CDMA_,
                                  sdss_map_band_to_band_pref(SD_BAND_CELL),
                                  chans[i],
                                 NULL,
                                  sys_idx );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_CUST:

      for( i=0; i < acq_rec_ptr->type.cellular_cdma_cust.num_chans; i++ )
      {
        sdsr_list_store_and_app2(
                              list1,
                              list2,
                              list3,
                              SD_MODE_CDMA_,
                              sdss_map_band_to_band_pref(SD_BAND_CELL),
                              acq_rec_ptr->type.cellular_cdma_cust.chan[i],
                              NULL,
                              sys_idx );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_GENERIC:

      switch (acq_rec_ptr->type.cellular_generic.a_b)
      {
        case PRL_AB_SYSTEM_A:
          chans[0]  = (word)pch_a;
          chans[1]  = (word)sch_a;
          sys[0]    = (word)SD_CELL_SYS_A;
          break;

        case PRL_AB_SYSTEM_B:
          chans[0]  = (word)pch_b;
          chans[1]  = (word)sch_b;
          sys[0]    = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          chans[0]  = (word)pch_a;
          chans[1]  = (word)sch_a;
          chans[2]  = (word)pch_b;
          chans[3]  = (word)sch_b;
          sys[0]    = (word)SD_CELL_SYS_A;
          sys[1]    = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_RESERVED:
        default:
          SD_CASE_ERR("a_b",0,0,0);
          break;
      }

      /* Add CDMA channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        sdsr_list_store_and_app2( list1,
                                  list2,
                                  list3,
                                  SD_MODE_CDMA_,
                                  sdss_map_band_to_band_pref(SD_BAND_CELL),
                                  chans[i],
                                 NULL,
                                  sys_idx );
      }

      /* If instructed to add the AMPS systems as well, do so.
      */
      switch(append_type)
      {
        case SDPRL_APPEND_ALL:

          for( i=0; i < (int) ARR_SIZE(sys) && sys[i] != NO_CHAN; i++ )
          {
            sdsr_list_store_and_app2( list1,
                                      list2,
                                      list3,
                                      SD_MODE_AMPS,
                                      sdss_map_band_to_band_pref(SD_BAND_CELL),
                                      sys[i],
                                     NULL,
                                      sys_idx );
          }
          break;
        case SDPRL_APPEND_PREF_ONLY:
        case SDPRL_APPEND_MAX:
        default:
          break;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_BLOCKS:

      for( i=0; i < acq_rec_ptr->type.pcs_cdma_block.num_blocks; i++ )
      {
        /* If wildcard block, add all 42 channels, else add only specific
        ** block channels.
        */
        if( acq_rec_ptr->type.pcs_cdma_block.block[i] == PRL_BLOCK_WILDCARD )
        {
          for( j=0; j < (int) ARR_SIZE(prl_pcs_block_chan_tbl); j++ )
          {
            sdsr_list_store_and_app2(
                             list1,
                             list2,
                             list3,
                             SD_MODE_CDMA_,
                             sdss_map_band_to_band_pref(SD_BAND_PCS),
                             prl_pcs_block_chan_tbl[j].chan,
                             NULL,
                             sys_idx );
          }
        }
        else /* Not a wildcard block */
        {
          for( j=0; j < (int) ARR_SIZE(prl_pcs_block_chan_tbl); j++ )
          {
            if (acq_rec_ptr->type.pcs_cdma_block.block[i] ==
                prl_pcs_block_chan_tbl[j].block)
            {
              sdsr_list_store_and_app2(
                              list1,
                              list2,
                              list3,
                              SD_MODE_CDMA_,
                              sdss_map_band_to_band_pref(SD_BAND_PCS),
                              prl_pcs_block_chan_tbl[j].chan,
                              NULL,
                              sys_idx );
            }
          }
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_CHANNELS:

      for( i=0; i < acq_rec_ptr->type.pcs_cdma_chan.num_chans; i++ )
      {
        sdsr_list_store_and_app2( list1,
                                  list2,
                                  list3,
                                  SD_MODE_CDMA_,
                                  sdss_map_band_to_band_pref(SD_BAND_PCS),
                                  acq_rec_ptr->type.pcs_cdma_chan.chan[i],
                                 NULL,
                                  sys_idx );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following three ACQ REC types apply for IS683B PRL or above */

    case PRL_ACQ_REC_JTACS_CDMA_STD:
      /* Add the channels for the specified Cellular CDMA system(s) to the
      ** list
      */
      switch( acq_rec_ptr->type.jtacs_cdma_std.a_b )
      {
        case PRL_AB_SYSTEM_A:

          switch( acq_rec_ptr->type.jtacs_cdma_std.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR("pri_sec",0,0,0);
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:

          switch (acq_rec_ptr->type.jtacs_cdma_std.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_b;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR("pri_sec",0,0,0);
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:

          switch (acq_rec_ptr->type.jtacs_cdma_std.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              chans[2] = jtacs_pch_b;
              chans[3] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR("pri_sec",0,0,0);
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_CASE_ERR("a_b",0,0,0);
          break;

      } /* end switch (acq_rec_ptr->type.cellular_cdma.a_b) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        sdsr_list_store_and_app2( list1,
                                  list2,
                                  list3,
                                  SD_MODE_CDMA_,
                                  SD_SS_BAND_PREF_BC3,
                                  chans[i],
                                  NULL,
                                  sys_idx );
      }
      break;

    case PRL_ACQ_REC_JTACS_CDMA_CUST:

      for( i=0; i < acq_rec_ptr->type.jtacs_cdma_cust.num_chans; i++ )
      {
        sdsr_list_store_and_app2(
                              list1,
                              list2,
                              list3,
                              SD_MODE_CDMA_,
                              SD_SS_BAND_PREF_BC3,
                              acq_rec_ptr->type.jtacs_cdma_cust.chan[i],
                              NULL,
                              sys_idx );
      }
      break;

    case PRL_ACQ_REC_CDMA_2GHZ_CHANS:

      for( i=0; i < acq_rec_ptr->type.cdma_2ghz_chans.num_chans; i++ )
      {
        sdsr_list_store_and_app2(
                              list1,
                              list2,
                              list3,
                              SD_MODE_CDMA_,
                              SD_SS_BAND_PREF_BC6,
                              acq_rec_ptr->type.cdma_2ghz_chans.chan[i],
                              NULL,
                              sys_idx );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following ACQ REC types are only apply for IS683C PRL or above. */

    case PRL_ACQ_REC_CDMA_GENERIC:

      for( i=0; i < acq_rec_ptr->type.cdma_generic.num_recs; i++ )
      {
        sdsr_list_store_and_app2(
            list1,
            list2,
            list3,
            SD_MODE_CDMA_,
            sdss_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec_ptr->type.cdma_generic.rec[i])),
            PRLI_CDMA_GENERIC_CHAN(acq_rec_ptr->type.cdma_generic.rec[i]),
            NULL,
            sys_idx );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_HDR_GENERIC:

      for( i=0; i < acq_rec_ptr->type.hdr_generic.num_recs; i++ )
      {
        sdsr_list_store_and_app2(
            list1,
            list2,
            list3,
            SD_MODE_HDR_,
            sdss_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec_ptr->type.hdr_generic.rec[i])),
            PRLI_CDMA_GENERIC_CHAN(acq_rec_ptr->type.hdr_generic.rec[i]),
            NULL,
            sys_idx );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GSM_GENERIC:

      #if defined(FEATURE_GSM)
#error code not present
      #endif /* (FEATURE_GSM) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_WCDMA_GENERIC:

      #if defined(FEATURE_WCDMA)
#error code not present
      #endif /* (FEATURE_WCDMA) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GW_PREFERRED:

      #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
      /* Extract the PLMN information from the internal PRL format.
      */
      sid_exp.net_select_mode = acq_rec_ptr->plmn.net_select_mode;
      sid_exp.prm.plmn.plmn_id = acq_rec_ptr->plmn.plmn_id;
      sid_exp.prm.plmn.lac = acq_rec_ptr->plmn.lac;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      band_pref = SD_SS_BAND_PREF_NONE;

      /* Get all the GSM bands.
      */
      for( i=0; i < acq_rec_ptr->type.gw_pref.num_gsm_bands; i++ )
      {
        band = prl_map_gsm_band_to_band(
                (prl_gsm_band_e_type)acq_rec_ptr->type.gw_pref.gsm_band[i] );
        band_pref = SD_BAND_ADD( band_pref, sdss_map_band_to_band_pref(band));
      }

      /* Get all the WCDMA bands.
      */
      for( i=0; i < acq_rec_ptr->type.gw_pref.num_wcdma_bands; i++ )
      {
        band = prl_map_wcdma_band_to_band(
         (prl_wcdma_band_e_type)acq_rec_ptr->type.gw_pref.wcdma_rec[i].band);
        band_pref = SD_BAND_ADD( band_pref,
                                 sdss_map_band_to_band_pref(band) );
      }

      /* Store the system in the data base.
      */
      sdsr_list_store_and_app2( list1,
                                list2,
                                list3,
                                SD_MODE_GW,
                                band_pref,
                                0,
                                &sid_exp,
                                sys_idx );
      #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_RESERVED:
    case PRL_ACQ_REC_NONE:
    default:

      SD_MSG_LOW( "Unknown acq_rec_type %d",acq_rec_ptr->acq_type,0,0 );
      break;

  } /* switch( acq_rec_ptr->acq_type ) */

} /* sdprl_acq_rec_app */




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
MMEXTN  boolean                   sdprl_blksys_app(

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
)
{
  prli_acq_rec_type         acq_rec;

  int                       j;             /* Index to the table */

  boolean                   is_match_found = FALSE;
                                           /* Indicate if match is found */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Table associating PCS frequency blocks with their set of
  ** preferred CDMA channels.
  ** 3GPP2 C.S0057-B "Band Class Specification for cdma2000 Spread Spectrum
  ** Systems" - August 3rd 2006

  ** Table 2.1.3-5. CDMA Preferred Set of Frequency Assignments for Band Class 3
  ** Table 2.1.4-5. CDMA Preferred Set of Frequency Assignments for Band Class 4
  ** Table 2.1.5-5. CDMA Preferred Set of Frequency Assignments for Band Class 5
  ** Block L are not specified in the standard.
  ** Table 2.1.6-5. CDMA Preferred Set of Frequency Assignments for Band Class 6
  ** Table 2.1.3-5. CDMA Preferred Set of Frequency Assignments for Band Class 7
  ** Block A and B are not specified in the standard.
  ** Table 2.1.4-5. CDMA Preferred Set of Frequency Assignments for Band Class 8
  ** Table 2.1.5-5. CDMA Preferred Set of Frequency Assignments for Band Class 9
  ** Table 2.1.6-5. CDMA Preferred Set of Frequency Assignments for Band Class 10
  ** Table 2.1.7-5. CDMA Preferred Set of Frequency Assignments for Band Class 11
  ** Block F, G, H, and L are not specified in the standard.
  ** Table 2.1.8-5. CDMA Preferred Set of Frequency Assignments for Band Class 12
  ** Table 2.1.10-5. CDMA Preferred Set of Frequency Assignments for Band Class 14
  ** Table 2.1.11-5. CDMA Preferred Set of Frequency Assignments for Band Class 15
  ** Table 2.1.12-5. CDMA Preferred Set of Frequency Assignments for Band Class 16
  ** 3GPP2 C.S0057-C - July 16th 2008
  ** Table 2.1.19-5. CDMA Preferred Set of Frequency Assignments for Band Class 18
  ** Table 2.1.20-5. CDMA Preferred Set of Frequency Assignments for Band Class 19
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  static const struct {

      sd_band_e_type    band;

      sd_blksys_e_type  block;
          /* frequency block */

      word              chan;
          /* Associated CDMA channel number */

  } sdprl_bc_block_chan_tbl[] = {

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC4_SUPPORT))

    /* Korean PCS block A preferred channels
    */
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    25 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    50 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    75 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    100 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    125 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    150 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    175 },

    /* Korean PCS block B preferred channels.
    */
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    225 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    250 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    275 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    300 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    325 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    350 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    375 },

    /* Korean PCS block C preferred channels.
    */
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    425 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    450 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    475 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    500 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    525 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    550 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    575 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC4_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC5_SUPPORT))

    /* Band Class 5/11 block A preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_A, 160 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_A, 210 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_A, 260 },

    /* Band Class 5/11 block B preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_B, 120 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_B, 170 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_B, 220 },

    /* Band Class 5/11 block C preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_C, 47  },
    { SD_BAND_BC5, SD_BLKSYS_PCS_C, 97  },
    { SD_BAND_BC5, SD_BLKSYS_PCS_C, 147 },

    /* Band Class 5/11 block D preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_D, 573 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_D, 623 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_D, 673 },

    /* Band Class 5/11 block E preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_E, 731 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_E, 781 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_E, 831 },

    /* Band Class 5/11 block F preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_F, 1841 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_F, 1903 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_F, 1965 },

    /* Band Class 5/11 block G preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_G, 1291 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_G, 1353 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_G, 1415 },

    /* Band Class 5/11 block H preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_H, 1089 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_H, 1151 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_H, 1213 },

    /* Band Class 5/11 block I preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_I, 92  },
    { SD_BAND_BC5, SD_BLKSYS_PCS_I, 142 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_I, 192 },

    /* Band Class 5/11 block J preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_J, 255 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_J, 305 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_J, 355 },

    /* Band Class 5/11 block K preferred channels
     */
    { SD_BAND_BC5, SD_BLKSYS_PCS_K, 1575 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_K, 1625 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_K, 1675 },

    /* Band Class 5/11 block L preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_L, 507 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_L, 557 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_L, 607 },

     #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC5_SUPPORT)) */

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC6_SUPPORT))

    /* Band Class 6 block A preferred channels
     */
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 25 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 75 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 125 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 150 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 175 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 200 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 225 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 250 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 275 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 300 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 325 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 350 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 375 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 400 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 425 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 450 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 475 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 500 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 525 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 550 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 575 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 600 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 625 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 650 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 675 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 700 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 725 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 750 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 775 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 800 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 825 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 850 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 875 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 900 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 925 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 950 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 975 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1000 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1025 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1050 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1075 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1100 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1125 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1150 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1175 },

     #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC6_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC7_SUPPORT))

    /* Band Class 7 block C preferred channels
    */
    { SD_BAND_BC7, SD_BLKSYS_PCS_C, 45  },
    { SD_BAND_BC7, SD_BLKSYS_PCS_C, 70  },
    { SD_BAND_BC7, SD_BLKSYS_PCS_C, 95 },

    /* Band Class 7 block D preferred channels
    */
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 145 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 170 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 195 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 220 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 245 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 270 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 295 },

     #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC7_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC8_SUPPORT))

    /* Band Class 8 block A preferred channels
    */
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 25 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 75 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 125 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 150 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 175 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 200 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 225 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 250 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 275 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 300 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 325 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 350 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 375 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 400 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 425 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 450 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 475 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 500 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 525 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 550 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 575 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 600 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 625 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 650 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 675 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 700 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 725 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 750 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 775 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 800 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 825 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 850 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 875 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 900 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 925 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 950 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 975 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1000 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1025 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1050 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1075 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1100 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1125 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1150 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1175 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1100 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1225 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1250 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1275 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1300 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1325 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1350 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1375 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1400 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1425 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1450 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1475 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC8_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC9_SUPPORT))

    /* Band Class 9 block A preferred channels
    */
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 25 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 75 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 125 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 150 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 175 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 200 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 225 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 250 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 275 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 300 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 325 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 350 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 375 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 400 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 425 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 450 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 475 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 500 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 525 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 550 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 575 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 600 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 625 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 650 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 675 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC9_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC10_SUPPORT))

    /* Band Class 10 block A preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_A, 150 },

    /* Band Class 10 block B preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_B, 250 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_B, 300 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_B, 350 },

    /* Band Class 10 block C preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_C, 450  },
    { SD_BAND_BC10, SD_BLKSYS_PCS_C, 500 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_C, 550 },

    /* Band Class 10 block D preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_D, 650 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_D, 670 },

    /* Band Class 10 block E preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_E, 770 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_E, 820 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_E, 870 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC10_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC11_SUPPORT))

    /* Band Class 5/11 block A preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_A, 160 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_A, 210 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_A, 260 },

    /* Band Class 5/11 block B preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_B, 120 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_B, 170 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_B, 220 },

    /* Band Class 5/11 block C preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_C, 47  },
    { SD_BAND_BC11, SD_BLKSYS_PCS_C, 97  },
    { SD_BAND_BC11, SD_BLKSYS_PCS_C, 147 },

    /* Band Class 5/11 block D preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_D, 573 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_D, 623 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_D, 673 },

    /* Band Class 5/11 block E preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_E, 731 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_E, 781 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_E, 831 },


    /* Band Class 5/11 block F preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_F, 1841 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_F, 1903 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_F, 1965 },

    /* Band Class 5/11 block G preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_G, 1291 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_G, 1353 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_G, 1415 },

    /* Band Class 5/11 block H preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_H, 1089 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_H, 1151 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_H, 1213 },

    /* Band Class 5/11 block I preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_I, 92  },
    { SD_BAND_BC11, SD_BLKSYS_PCS_I, 142 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_I, 192 },

    /* Band Class 5/11 block J preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_J, 255 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_J, 305 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_J, 355 },

    /* Band Class 5/11 block K preferred channels
     */
    { SD_BAND_BC11, SD_BLKSYS_PCS_K, 1575 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_K, 1625 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_K, 1675 },

    /* Band Class 5/11 block L preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_L, 507 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_L, 557 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_L, 607 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC11_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC12_SUPPORT))

    /* Band Class 12 block A preferred channels
    */
    { SD_BAND_BC12, SD_BLKSYS_PCS_A, 89 },
    { SD_BAND_BC12, SD_BLKSYS_PCS_A, 139 },
    { SD_BAND_BC12, SD_BLKSYS_PCS_A, 189 },

    /* Band Class 12 block B preferred channels
    */
    { SD_BAND_BC12, SD_BLKSYS_PCS_B, 94 },
    { SD_BAND_BC12, SD_BLKSYS_PCS_B, 144 },

    /* Band Class 12 block C preferred channels
    */
    { SD_BAND_BC12, SD_BLKSYS_PCS_C, 106  },
    { SD_BAND_BC12, SD_BLKSYS_PCS_C, 156 },
    { SD_BAND_BC12, SD_BLKSYS_PCS_C, 206 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC12_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Band Class 13 not supported
    */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC14_SUPPORT))

    /* Band Class 14 block G preferred channels
    */
    { SD_BAND_BC14, SD_BLKSYS_PCS_G, 1225 },
    { SD_BAND_BC14, SD_BLKSYS_PCS_G, 1250 },
    { SD_BAND_BC14, SD_BLKSYS_PCS_G, 1275 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC14_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC15_SUPPORT))

    /* Band Class 15 block A preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 25 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 75 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 125 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 150 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 175 },

    /* Band Class 15 block B preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 225 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 250 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 275 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 300 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 325 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 350 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 375 },

    /* Band Class 15 block C preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_C, 425 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_C, 450 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_C, 475 },

    /* Band Class 15 block D preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_D, 525 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_D, 550 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_D, 575 },

    /* Band Class 15 block E preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_E, 625 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_E, 650 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_E, 675 },

    /* Band Class 15 block F preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 725 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 750 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 775 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 800 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 825 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 850 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 875 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC15_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC16_SUPPORT))

    /* Band Class 16 block A preferred channels
    */
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 165 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 190 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 215 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 240 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 265 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 290 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 315 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 340 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 365 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 390 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 415 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 440 },

    /* Band Class 16 block B preferred channels
    */
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 495 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 520 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 545 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 570 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 595 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 620 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 645 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 670 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 695 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 720 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 745 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 770 },

    /* Band Class 16 block C preferred channels
    */
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 825 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 850 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 875 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 900 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 925 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 950 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 975 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1000 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1025 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1050 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1075 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1100 },

    /* Band Class 16 block D preferred channels
    */
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1155 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1180 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1205 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1230 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1255 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1280 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1305 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1330 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1355 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1380 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1405 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1430 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC16_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC18_SUPPORT))
    
    /* Band Class 18 block D preferred channels
    */
    { SD_BAND_BC18, SD_BLKSYS_PCS_D, 45 },
    { SD_BAND_BC18, SD_BLKSYS_PCS_D, 70 },
    { SD_BAND_BC18, SD_BLKSYS_PCS_D, 95 },

    /* Band Class 18 public safety broadband preferred channels
    */
    { SD_BAND_BC18, SD_BLKSYS_PSB, 145 },
    { SD_BAND_BC18, SD_BLKSYS_PSB, 170 },
    { SD_BAND_BC18, SD_BLKSYS_PSB, 195 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) ||     defined(FEATURE_SD_OTASP_BC18_SUPPORT)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC19_SUPPORT))
    
    /* Band Class 19 block A preferred channels
    */
    { SD_BAND_BC19, SD_BLKSYS_PCS_A, 23 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_A, 48 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_A, 73 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_A, 98 },
    
    /* Band Class 19 block B preferred channels
    */
    { SD_BAND_BC19, SD_BLKSYS_PCS_B, 143 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_B, 168 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_B, 193 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_B, 218 },

    /* Band Class 19 block C preferred channels
    */
    { SD_BAND_BC19, SD_BLKSYS_PCS_C, 263 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_C, 288 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_C, 313 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_C, 338 },

    #endif /* (!defined(FEATURE_SD_LOW_MEM_TARGET) || defined(FEATURE_SD_OTASP_BC19_SUPPORT)) */

    { SD_BAND_BC0, SD_BLKSYS_CELL_A, 1405 },
    { SD_BAND_BC0, SD_BLKSYS_CELL_B, 1430 }

  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( blksys, SD_BLKSYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  acq_rec.is_valid = TRUE;

  /* Based on specified band-class and PCS frequency block/ Cellular system
  ** append systems.
  */
  switch( band ) /*lint -e788 */ /*enum not used within switch */
  {
    case SD_BAND_BC0:

      acq_rec.acq_type                    = PRL_ACQ_REC_CELLULAR_CDMA_STD;
      acq_rec.type.cellular_cdma.pri_sec  = PRL_PRISEC_PRI_OR_SEC;

      if( blksys == SD_BLKSYS_CELL_A )
      {
        acq_rec.type.cellular_cdma.a_b    = PRL_AB_SYSTEM_A;
      }
      else if( blksys == SD_BLKSYS_CELL_B )
      {
        acq_rec.type.cellular_cdma.a_b    = PRL_AB_SYSTEM_B;
      }
      else
      {
        break;
      }
      sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                         &acq_rec,
                         SD_NO_CHANGE_SYS_IDX,
                         SDPRL_APPEND_PREF_ONLY );

      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_BAND_BC1:

      if( INRANGE(blksys, SD_BLKSYS_PCS_A, SD_BLKSYS_PCS_F) )
      {
        acq_rec.acq_type                       = PRL_ACQ_REC_PCS_CDMA_BLOCKS;
        acq_rec.type.pcs_cdma_block.block[0]   = (word) blksys;
        acq_rec.type.pcs_cdma_block.num_blocks = 1;
        sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                           &acq_rec,
                           SD_NO_CHANGE_SYS_IDX,
                           SDPRL_APPEND_PREF_ONLY);

        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_BAND_BC4:
    case SD_BAND_BC5:

    case SD_BAND_BC6:
    case SD_BAND_BC7:
    case SD_BAND_BC8:

    case SD_BAND_BC9:
    case SD_BAND_BC10:
    case SD_BAND_BC11:
    case SD_BAND_BC12:

    case SD_BAND_BC14:
    case SD_BAND_BC15:
    case SD_BAND_BC16:
    case SD_BAND_BC18:
    case SD_BAND_BC19:

      acq_rec.acq_type  = PRL_ACQ_REC_CDMA_GENERIC;

      for( j=0; j < (int) ARR_SIZE(sdprl_bc_block_chan_tbl); j++ )
      {

        if ( band == sdprl_bc_block_chan_tbl[j].band  &&
             blksys == sdprl_bc_block_chan_tbl[j].block )
        {
             sdsr_list_store_and_app2(
                list, SDSR_MAX, SDSR_MAX,
                SD_MODE_CDMA_,
                sdss_map_band_to_band_pref( band ),
                sdprl_bc_block_chan_tbl[j].chan,
                NULL,
                SD_NO_CHANGE_SYS_IDX );

             is_match_found = TRUE;
        }
      }

    break;

    case SD_BAND_BC3:
    case SD_BAND_MAX:
    default:
      break;
  }  /*lint +e788 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this operation is not supported for the specified
  ** band-class and and/or blksys - return FALSE.
  */
  if(!is_match_found )
  {
    SD_MSG_ERROR( "Bad bc=%d, blksys=%d!",band,blksys,0);
  }

  return is_match_found;

} /* sdprl_blksys_app */




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
MMEXTN  void                      sdprl_all_pref_sys_app(

        sdsr_e_type               list
            /* System record list to which to append the system records.
            */
)
{
  prli_acq_rec_type         acq_rec;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  acq_rec.is_valid = TRUE;

  /* If MS supports CDMA band-class 0 systems, append preferred CDMA channels
  ** that are associated with this mode/band combination.
  */
  if( sdss_is_supp_mode_band(SD_MODE_CDMA, sdss_map_band_to_band_pref(SD_BAND_BC0)) )
  {
    acq_rec.acq_type                    = PRL_ACQ_REC_CELLULAR_CDMA_STD;
    acq_rec.type.cellular_cdma.pri_sec  = PRL_PRISEC_PRI_OR_SEC;
    acq_rec.type.cellular_cdma.a_b      = PRL_AB_SYSTEM_A_OR_B;
    sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                       &acq_rec,
                       SD_NO_CHANGE_SYS_IDX,
                       SDPRL_APPEND_PREF_ONLY);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MS supports CDMA band-class 1 systems, append preferred CDMA channels
  ** that are associated with this mode/band combination.
  */
  if( sdss_is_supp_mode_band(SD_MODE_CDMA, sdss_map_band_to_band_pref(SD_BAND_BC1)) )
  {
    acq_rec.acq_type                       = PRL_ACQ_REC_PCS_CDMA_BLOCKS;
    acq_rec.type.pcs_cdma_block.block[0]   = PRL_BLOCK_WILDCARD;
    acq_rec.type.pcs_cdma_block.num_blocks = 1;
    sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                       &acq_rec,
                       SD_NO_CHANGE_SYS_IDX,
                       SDPRL_APPEND_PREF_ONLY );
  }

} /* sdprl_all_pref_sys_app */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_acq_tbl_cpy

DESCRIPTION
  Copy the PRL's acquisition table from its NV storage format into a
  specified system record list.

DEPENDENCIES
  The PRL must have already been read from NV into RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_tbl_cpy(

        sdsr_e_type               list
            /* System record list into which to copy the MRU entries.
            */
)
{
  int                 i;
  prli_acq_rec_type   prl_acq_rec;
      /* PRL-acquisition record
      */

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
  sdprl_parser_s_type parser, *parser_ptr = &parser;
      /* Pointer to a PRL-parser item to be initialized with the
      ** PRL that is pointed to by the prl_ptr parameter.
      */

  prli_sys_rec_type  sys_rec;

  sdprl_s_type       *sdprl_ptr = &sdprl;

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the acquisition table entries from the PRL and append them to
  ** the specified list.
  ** Make sure acquisition table is not bigger than system record table??
  */
  sdsr_list_clr( list );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; prl_extract_acq_rec(&prl_acq_rec, i); i++ )
  {
    /* If the record is not valid, skip
    */
    if( !prl_acq_rec.is_valid )
    {
      continue;
    }

    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
    if( prl_acq_rec.acq_type == PRL_ACQ_REC_GSM_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_WCDMA_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_GW_PREFERRED )
    {
      /* Go through all the system records
      ** if there is matching of acq index
      ** update acq record with associated PLMN informaiton.
      ** store the record to system table.
      */
      sdprl_parser_init( parser_ptr,
                         SD_SS_MAIN,
                         (byte*) sdprl_ptr->prl_ptr,
                         prl_hdr_get_ptr() );

      sys_rec = parser_ptr->sys_rec;

      do{

        if( sys_rec.acq_index == i ){

          prl_acq_rec.plmn = sys_rec.rec.plmn;

          sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                             &prl_acq_rec,
                             SD_NO_CHANGE_SYS_IDX,
                             SDPRL_APPEND_ALL );
        }

      } while( sdprl_parser_next( parser_ptr, &sys_rec ) );

    } /* if */
    else
    #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
    {
      sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                         &prl_acq_rec,
                         SD_NO_CHANGE_SYS_IDX,
                         SDPRL_APPEND_ALL );
    }

  } /* for */

} /* sdprl_acq_tbl_cpy */


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_plmn_id_is_undefined

DESCRIPTION
  Check if plmn is undefined in prl.

DEPENDENCIES
  None.

RETURN VALUE
  True if plmn is undefined
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_id_is_undefined (

        const prli_plmn_sys_type  *prli_ptr
        /* Pointer to prl internal plmn system type.
        */
)
{
  SD_ASSERT( prli_ptr != NULL );

  switch( prli_ptr->net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:
      return TRUE;

    case SD_NET_SELECT_MODE_AUTO:
      return ( sys_plmn_id_is_undefined( prli_ptr->plmn_id ) );

    case SD_NET_SELECT_MODE_PRL_SYS:
      return FALSE;

    case SD_NET_SELECT_MODE_MANUAL_SYS:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    case SD_NET_SELECT_MODE_MAX:
    default:
      SD_MSG_LOW( "network select mode error %d",
                  prli_ptr->net_select_mode,
                  0,
                  0 );
      return FALSE;

  }/* switch */

} /* sdprl_plmn_id_is_undefined */




/* <EJECT> */
/*===========================================================================

FUNCTION SDPRL_PLMN_IS_LAC_INCLUDED

DESCRIPTION
  Check if lac is included in system record field of prl.

DEPENDENCIES
  None.

RETURN VALUE
  True if lac is included
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_is_lac_included(

        const prli_plmn_sys_type  *prli_ptr
        /* Pointer to plmn system data type.
        */
)
{
  sd_net_select_mode_e_type       net_select_mode;

  SD_ASSERT( prli_ptr != NULL );

  net_select_mode = prli_ptr->net_select_mode;

  switch( net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:
      return FALSE;
    case SD_NET_SELECT_MODE_AUTO:
    case SD_NET_SELECT_MODE_PRL_SYS:
    case SD_NET_SELECT_MODE_MANUAL_SYS:
      return ( (prli_ptr->lac != PRL_WILDCARD_LAC) || \
               (prli_ptr->num_lac > 0) );
    case SD_NET_SELECT_MODE_MAX:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    default:
      SD_MSG_LOW( "network select mode error %d", net_select_mode, 0, 0);
      return FALSE;
  }
}/* sdprl_plmn_is_lac_included */



/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_plmn_id_get

DESCRIPTION
  Get plmn id from PRL or over the air.

  If the network selection mode is SD_NET_SELECT_MODE_PRL_SYS or
  SD_NET_SELECT_MODE_MANUAL_SYS, set plmn id from expected sid field (PRL).
  Otherwise set plmn id from sid field (over the air).


DEPENDENCIES
  None.

RETURN VALUE
  boolean return true if plmn id is populated sucessfully.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_id_get(

        sys_plmn_id_s_type        *plmn_ptr,
        /* Pointer to the plmn to be populated
        */

        const sdsr_s_type         *sd_sr_ptr
        /* Pointer to the SD system record to be compared.
        */
)
{
  sd_net_select_mode_e_type       net_select_mode;

  SD_ASSERT( plmn_ptr != NULL );
  SD_ASSERT( sd_sr_ptr != NULL );

  net_select_mode = sd_sr_ptr->sid_expected.net_select_mode;

  switch( net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:
    case SD_NET_SELECT_MODE_AUTO:
         *plmn_ptr = sd_sr_ptr->sid.plmn.plmn_id;
         return TRUE;
    case SD_NET_SELECT_MODE_PRL_SYS:
    case SD_NET_SELECT_MODE_MANUAL_SYS:
         *plmn_ptr = sd_sr_ptr->sid_expected.prm.plmn.plmn_id;
         return TRUE;
    case SD_NET_SELECT_MODE_MAX:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    default:
      SD_MSG_LOW( "network select mode error %d", net_select_mode, 0, 0);
      return FALSE;
  }

} /* sdprl_plmn_id_get */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_plmn_get_lac

DESCRIPTION
  Get lac from PRL or over the air

  If lac is included and the network selection mode is either
  SD_NET_SELECT_MODE_PRL_SYS or SD_NET_SELECT_MODE_MANUAL_SYS, set lac
  from expected sid field (PRL).
  Otherwise set lac from sid field (over the air).

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_get_lac(

        sys_lac_type              *lac_ptr,
        /* Pointer to the lac to be populated
        */

        const sdsr_s_type         *sd_sr_ptr
        /* Pointer to the SD system record to be compared.
        */
)
{
  sd_net_select_mode_e_type       net_select_mode;

  SD_ASSERT( sd_sr_ptr != NULL );
  SD_ASSERT( lac_ptr != NULL );

  net_select_mode = sd_sr_ptr->sid_expected.net_select_mode;

  switch( net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:
    case SD_NET_SELECT_MODE_AUTO:
         *lac_ptr = sd_sr_ptr->sid.plmn.lac;
         return TRUE;
    case SD_NET_SELECT_MODE_PRL_SYS:
    case SD_NET_SELECT_MODE_MANUAL_SYS:
         if( sd_sr_ptr->sid_expected.prm.plmn.lac != PRL_WILDCARD_LAC )
         {
           *lac_ptr = sd_sr_ptr->sid_expected.prm.plmn.lac;
         }
         else
         {
           *lac_ptr = sd_sr_ptr->sid.plmn.lac;
         }
         return TRUE;
    case SD_NET_SELECT_MODE_MAX:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    default:
      SD_MSG_LOW( "network select mode error %d", net_select_mode, 0, 0);
      return FALSE;
  }

} /* sdprl_plmn_get_lac */
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_sid_match

DESCRIPTION
  Check whether the system ID of an SD system record matches to the system ID
  of a PRL system record.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system ID's match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_is_sid_match(

        const sdsr_s_type         *sd_sr_ptr,
            /* Pointer to the SD system record to be compared.
            */

        const prli_sys_rec_type   *prl_sr_ptr
            /* Pointer to the PRL system record against which to compare.
            */
)
{
  /* Mask table to isolate MSB bits within a byte.
  */
  static const  byte mask_tbl[] = { 0x00, 0x80, 0xC0, 0xE0,
                                    0xF0, 0xF8, 0xFC, 0xFE };
  byte          mask;
  unsigned int  i;
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
  sys_plmn_id_s_type plmn;
  sys_lac_type       lac;
  boolean       is_lac_match = FALSE;
  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sd_sr_ptr != 0 );
  SD_ASSERT( prl_sr_ptr != 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two systems have incompatible ID types, return FALSE.
  */
  if( prl_sr_ptr->rec_type == PRL_SYS_REC_IS2000 &&
      sd_sr_ptr->sys.mode != SD_MODE_CDMA_ &&
      sd_sr_ptr->sys.mode != SD_MODE_AMPS )
  {
    return FALSE;
  }

  if( prl_sr_ptr->rec_type == PRL_SYS_REC_IS856 &&
      sd_sr_ptr->sys.mode != SD_MODE_HDR_ )
  {
    return FALSE;
  }

  if( prl_sr_ptr->rec_type == PRL_SYS_REC_PLMN &&
      sd_sr_ptr->sys.mode != SD_MODE_GSM &&
      sd_sr_ptr->sys.mode != SD_MODE_WCDMA &&
      sd_sr_ptr->sys.mode != SD_MODE_GW )
  {
    return FALSE;
  }

  if( prl_sr_ptr->rec_type == PRL_SYS_REC_MAX )
  {
    SD_MSG_LOW( "Unknown sys_rec_type",0,0,0 );
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compare the two SIDs base on the ID type.
  */
  switch( sd_sr_ptr->sys.mode )
  {
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:

      /* If SID or NID does not match, return FALSE.
      */
      if( prl_sr_ptr->rec.is2000.sid != sd_sr_ptr->sid.is95.sid &&
          prl_sr_ptr->rec.is2000.sid != PRL_WILDCARD_SID )
      {
        return FALSE;
      }

      if( prl_sr_ptr->rec.is2000.nid != sd_sr_ptr->sid.is95.nid &&
          prl_sr_ptr->rec.is2000.nid != PRL_WILDCARD_NID )
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_HDR:

      for( i=0;
           i < ARR_SIZE(sd_sr_ptr->sid.is856) &&
           i < (unsigned int) prl_sr_ptr->rec.is856.subnet_length / 8;
           i++ )
      {
        if( prl_sr_ptr->rec.is856.subnet[i] != sd_sr_ptr->sid.is856[i] )
        {
          return FALSE;
        }
      }

      /* Check the reminder, as needed.
      */
      if( i < ARR_SIZE(sd_sr_ptr->sid.is856) )
      {
        mask = mask_tbl[ prl_sr_ptr->rec.is856.subnet_length & 0x07 ];

        if( (prl_sr_ptr->rec.is856.subnet[i] & mask) !=
            (sd_sr_ptr->sid.is856[i] & mask) )
        {
          return FALSE;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:

      #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
      /* If PLMN in PRL system record is undefined, PLMN always match except
      ** for HPLMN.
      */
      if( sdprl_plmn_id_is_undefined( &prl_sr_ptr->rec.plmn ) )
      {
        /* If the PRL system is a HPLMN and the acquired system is not HPLMN
        ** (i.e. roaming indication is not OFF), this is not a match - return
        ** FALSE.
        */
        if( prl_sr_ptr->rec.plmn.net_select_mode ==
                                                 SD_NET_SELECT_MODE_HPLMN &&
            sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) !=
                                                 SYS_ROAM_STATUS_OFF
          )
        {
          return FALSE;
        }

        /* Else, this is a match.
        */
        return TRUE;
      }

      /* Get plmn id from system record.
      */
      if( ! sdprl_plmn_id_get(&plmn, sd_sr_ptr) )
      {
        return FALSE;
      }


      /* Check if plmn in system record matches that in prl.
      */
      if( !sys_plmn_match(prl_sr_ptr->rec.plmn.plmn_id,plmn) )
      {
          return FALSE;
      }

      /* Check if lac is included in prl.
      */
      if( sdprl_plmn_is_lac_included(&prl_sr_ptr->rec.plmn) )
      {
        /* Get lac from system record.
        */
        if( !sdprl_plmn_get_lac(&lac, sd_sr_ptr) )
        {
          return FALSE;
        }

        /* Check if there are multiple lacs in prl.
        */
        if( prl_sr_ptr->rec.plmn.num_lac == 0 )
        {
          /* Check if lac in system record matches that in prl.
          */
          if( prl_sr_ptr->rec.plmn.lac != lac )
          {
              return FALSE;
          }
        }
        else
        {
          for( i = 0; i < prl_sr_ptr->rec.plmn.num_lac; i++ )
          {
            /* Check if lac in system record matches that in prl.
            */
            if( prl_sr_ptr->rec.plmn.lac_a[i] == lac )
            {
              is_lac_match = TRUE;
              break;
            }
          }

          if( !is_lac_match )
          {
            return FALSE;
          }
        }

      }
      #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_WLAN:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_MAX:
    default:
      return FALSE;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two SIDs match - return TRUE.
  */
  return TRUE;

} /* sdprl_is_sid_match */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_sys_match

DESCRIPTION
  Compare a SD system record against a PRL system record.

DEPENDENCIES
  None.

RETURN VALUE
  sdprl_match_e_type enum value that indicates the level of matching.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdprl_match_e_type        sdprl_sys_match(

        const sdsr_s_type         *sd_sr_ptr,
            /* Pointer to the SD system record to be compared.
            */

        const prli_sys_rec_type   *prl_sr_ptr
            /* Pointer to the PRL system record against which to compare.
            */

)
{
  word        pch_a = sdprl.nam_info[sdprl.curr_nam].pch_a;
  word        pch_b = sdprl.nam_info[sdprl.curr_nam].pch_b;
  word        sch_a = sdprl.nam_info[sdprl.curr_nam].sch_a;
  word        sch_b = sdprl.nam_info[sdprl.curr_nam].sch_b;

  word        jtacs_pch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_a;
  word        jtacs_pch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_b;
  word        jtacs_sch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_a;
  word        jtacs_sch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_b;

  prli_acq_rec_type       acq_rec;
  word                    block;
  int                     i;
  sd_blksys_e_type        cell_sys;
  sd_band_e_type          band;
  sd_ss_band_pref_e_type  band_pref;
  sd_mode_e_type          mode;

  word                chans[4]  = { NO_CHAN, NO_CHAN, NO_CHAN, NO_CHAN };

  boolean band_is_match;
  boolean chan_is_match;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sd_sr_ptr != NULL );
  SD_ASSERT( prl_sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If System ID does not match, return SDPRL_MATCH_NONE.
  */
  if( ! sdprl_is_sid_match(sd_sr_ptr, prl_sr_ptr) )
  {
    return SDPRL_MATCH_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the SID/NID match - extract the acquisition record and
  ** check for mode, band-class, and CDMA channel/AMPS system match.
  */
  sdprl_get_acq_rec_else_err_fatal( &acq_rec, prl_sr_ptr );

  switch( acq_rec.acq_type )
  {
    case PRL_ACQ_REC_CELLULAR_ANALOG:

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_AMPS ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_CELL ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the SID/NID mode and band-class match - check for
      ** system match.
      */
      switch( acq_rec.type.cellular_analog.a_b )
      {
        case PRL_AB_SYSTEM_A:
          if( sd_sr_ptr->sys.chan == (sd_chan_type) SD_CELL_SYS_A ) {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_SYSTEM_B:
          if( sd_sr_ptr->sys.chan == (sd_chan_type) SD_CELL_SYS_B ) {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;

        case PRL_AB_RESERVED:
        default:
          SD_CASE_ERR( "a_b",0,0,0 );
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, only the SID/NID mode and band-class match.
      */
      return SDPRL_MATCH_SID_MODE_BAND;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_STD:

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_CELL ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the SID/NID mode and band-class match - check for
      ** system match.
      */
      switch( acq_rec.type.cellular_cdma.a_b )
      {
        case PRL_AB_SYSTEM_A:
          switch( acq_rec.type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR( "pri_sec",0,0,0 );
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:
          switch( acq_rec.type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_b;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR( "pri_sec",0,0,0 );
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:
          switch( acq_rec.type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              chans[1] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              chans[2] = pch_b;
              chans[3] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR( "pri_sec",0,0,0 );
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_CASE_ERR( "a_b",0,0,0 );
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If the system's channel match any of the channels in the chans
      ** array, return SDPRL_MATCH_SID_MODE_BAND_CHAN.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        if( chans[i] == (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

      /* Channel does not match - return SDPRL_MATCH_SID_MODE_BAND.
      */
      return SDPRL_MATCH_SID_MODE_BAND;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_CUST:

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
#ifdef FEATURE_IS683A_450M_SUPPORT 
      if (sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_IS683A_450M_SUPPORT)) 
      { 
        SD_MSG_HIGH("sdprl_sys_match - Feature IS683A_450M_SUPPORT is ON. Mapping PCS -> BC5",0,0,0); 
        if(sd_sr_ptr->sys.band != SD_SS_BAND_PREF_BC5 ) 
        {
            return SDPRL_MATCH_SID_MODE; 
        } 
      } 
      else 
      {
        if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_CELL )  
        { 
          return SDPRL_MATCH_SID_MODE; 
        }
      }
#else
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_CELL ) {
        return SDPRL_MATCH_SID_MODE;
      }
#endif
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the mode and band-class match - check for channel
      ** match. If such match is found, return SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
      for( i=0; i < acq_rec.type.cellular_cdma_cust.num_chans; i++ )
      {
        if( acq_rec.type.cellular_cdma_cust.chan[i] ==
            (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

      /* Channel does not match - return SDPRL_MATCH_SID_MODE_BAND.
      */
      return SDPRL_MATCH_SID_MODE_BAND;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_GENERIC:

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_CELL ) {
        return SDPRL_MATCH_SID_MODE;
      }
      
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the band-class match - check for system match.
      */
      cell_sys = sdprl_map_chan_to_blksys( sd_sr_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sd_sr_ptr->sys.band),
                                           sd_sr_ptr->sys.chan );

      switch( acq_rec.type.cellular_generic.a_b )
      {
        case PRL_AB_SYSTEM_A:
          if( cell_sys == SD_BLKSYS_CELL_A ) {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_SYSTEM_B:
          if( cell_sys == SD_BLKSYS_CELL_B ) {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          if( cell_sys == SD_BLKSYS_CELL_A ||
              cell_sys == SD_BLKSYS_CELL_B )
          {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_RESERVED:
        default:
          SD_CASE_ERR( "a_b",0,0,0 );
          break;
      }

      /* System does not match - return SDPRL_MATCH_SID_MODE_BAND.
      */
      return SDPRL_MATCH_SID_MODE_BAND;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_BLOCKS:

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_PCS ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the mode and band-class match - check for block
      ** match. If such a match is found, return
      ** SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
      block = sdprl_pcs_chan_to_block( sd_sr_ptr->sys.chan );

      for( i=0; i < acq_rec.type.pcs_cdma_block.num_blocks; i++ )
      {
        if( acq_rec.type.pcs_cdma_block.block[i] == block ||
            acq_rec.type.pcs_cdma_block.block[i] == PRL_BLOCK_WILDCARD )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

      /* Block does not match - return SDPRL_MATCH_SID_MODE_BAND.
      */
      return SDPRL_MATCH_SID_MODE_BAND;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_CHANNELS:

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }
      
#ifdef FEATURE_IS683A_450M_SUPPORT 
      if (sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_IS683A_450M_SUPPORT)) 
      { 
        SD_MSG_HIGH("sdprl_sys_match - Feature IS683A_450M_SUPPORT is ON. Mapping PCS -> BC5",0,0,0); 
        if(sd_sr_ptr->sys.band != SD_SS_BAND_PREF_BC5 ) 
        {
            return SDPRL_MATCH_SID_MODE; 
        } 
      } 
      else 
      {
        if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_PCS )  
        { 
          return SDPRL_MATCH_SID_MODE; 
        }
      }
#else
      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_PCS ) {
        return SDPRL_MATCH_SID_MODE;
      }
#endif
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
      /* If we got here, the mode and band-class match - check for channel
      ** match. If such a match is found, return
      ** SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
      for( i=0; i < acq_rec.type.pcs_cdma_chan.num_chans; i++ )
      {
        if( acq_rec.type.pcs_cdma_chan.chan[i] ==
            (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

      /* Channel does not match - return SDPRL_MATCH_SID_MODE_BAND.
      */
      return SDPRL_MATCH_SID_MODE_BAND;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following three acq record types apply for IS683B PRL or above.*/

    case PRL_ACQ_REC_JTACS_CDMA_STD:
      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_BC3 ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the SID/NID mode and band-class match - check for
      ** system match.
      */
      switch( acq_rec.type.jtacs_cdma_std.a_b )
      {
        case PRL_AB_SYSTEM_A:
          switch( acq_rec.type.jtacs_cdma_std.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] =  jtacs_sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] =  jtacs_pch_a;
              chans[1] =  jtacs_sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR( "pri_sec",0,0,0 );
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:
          switch( acq_rec.type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] =  jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] =  jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] =  jtacs_pch_b;
              chans[1] =  jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR( "pri_sec",0,0,0 );
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:
          switch( acq_rec.type.jtacs_cdma_std.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              chans[2] = jtacs_pch_b;
              chans[3] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_CASE_ERR( "pri_sec",0,0,0 );
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_CASE_ERR( "a_b",0,0,0 );
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If the system's channel match any of the channels in the chans
      ** array, return SDPRL_MATCH_SID_MODE_BAND_CHAN.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        if( chans[i] == (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

      /* Channel does not match - return SDPRL_MATCH_SID_MODE_BAND.
      */
      return SDPRL_MATCH_SID_MODE_BAND;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_JTACS_CDMA_CUST:
      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_BC3 ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the mode and band-class match - check for channel
      ** match. If such match is found, return SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
      for( i=0; i < acq_rec.type.jtacs_cdma_cust.num_chans; i++ )
      {
        if( acq_rec.type.jtacs_cdma_cust.chan[i] ==
            (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

      /* Channel does not match - return SDPRL_MATCH_SID_MODE_BAND.
      */
      return SDPRL_MATCH_SID_MODE_BAND;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CDMA_2GHZ_CHANS:
      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_BC6 ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the mode and band-class match - check for channel
      ** match. If such match is found, return SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
      for( i=0; i < acq_rec.type.cdma_2ghz_chans.num_chans; i++ )
      {
        if( acq_rec.type.cdma_2ghz_chans.chan[i] ==
            (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

      /* Channel does not match - return SDPRL_MATCH_SID_MODE_BAND.
      */
      return SDPRL_MATCH_SID_MODE_BAND;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CDMA_GENERIC:
    case PRL_ACQ_REC_HDR_GENERIC:

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( acq_rec.acq_type == PRL_ACQ_REC_CDMA_GENERIC &&
          sd_sr_ptr->sys.mode != SD_MODE_CDMA_ )
      {
        return SDPRL_MATCH_SID;
      }

      if( acq_rec.acq_type == PRL_ACQ_REC_HDR_GENERIC &&
          sd_sr_ptr->sys.mode != SD_MODE_HDR_ )
      {
        return SDPRL_MATCH_SID;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Check for band and channel match.
      */
      band_is_match = FALSE;
      chan_is_match = FALSE;
      for( i=0; i < acq_rec.type.cdma_generic.num_recs; i++ )
      {
        /* If band matches, indicate so.
        */
        if( sd_sr_ptr->sys.band ==
            sdss_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec.type.cdma_generic.rec[i])) )
        {
          band_is_match = TRUE;
        }

        /* If channel matches, indicate so.
        */
        if( sd_sr_ptr->sys.chan ==
            PRLI_CDMA_GENERIC_CHAN(acq_rec.type.cdma_generic.rec[i]) )
        {
          chan_is_match = TRUE;
        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( ! band_is_match ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /* If channel does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( ! chan_is_match ) {
        return SDPRL_MATCH_SID_MODE_BAND;
      }

      /* If we got here, band and channel match -
      ** return SDPRL_MATCH_SID_MODE_BAND_CHAN.
      */
      return SDPRL_MATCH_SID_MODE_BAND_CHAN;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GSM_GENERIC:

      /* If mode does not match, return SDPRL_MATCH_SID
      ** For GW system mode, also check  acquired system mode.
      */
      if( (sd_sr_ptr->sys.mode != SD_MODE_GSM) &&
          !((sd_sr_ptr->sys.mode == SD_MODE_GW) &&
          (sd_sr_ptr->sid.gw.mode == SD_MODE_GSM) ) )
      {
        return SDPRL_MATCH_SID;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Check for band match.
      */
      band_pref = SD_SS_BAND_PREF_NONE;

      for( i=0; i < acq_rec.type.gsm_generic.num_bands; i++ )
      {
        band = prl_map_gsm_band_to_band( (prl_gsm_band_e_type)acq_rec.type.gsm_generic.band[i] );
        band_pref = SD_BAND_ADD( band_pref ,
                                 sdss_map_band_to_band_pref( band )
                               );
      }

      if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.band, band_pref ) )
      {
        return SDPRL_MATCH_SID_MODE_BAND_CHAN;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If goes here, band does not match
      */
      return SDPRL_MATCH_SID_MODE;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_WCDMA_GENERIC:

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( (sd_sr_ptr->sys.mode != SD_MODE_WCDMA) &&
          !((sd_sr_ptr->sys.mode == SD_MODE_GW) &&
          (sd_sr_ptr->sid.gw.mode == SD_MODE_WCDMA) ) )
      {
        return SDPRL_MATCH_SID;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Check for band and channel match.
      */
      //band_is_match = FALSE;
      //multi_band_pref = SD_SS_BAND_PREF_NONE;
      //
      //for( i=0; i < acq_rec.type.wcdma_generic.num_bands; i++)
      //{
      //  band = prl_map_wcdma_band_to_band(
      //                               acq_rec.type.wcdma_generic.rec[i].band);
      //  band_pref = sdss_map_band_to_band_pref(band);
      //
      //  if( acq_rec.type.wcdma_generic.rec[i].uarfcn_incl == PRL_UARFCN_INCL)
      //  {
      //    if( sd_sr_ptr->sys.band == band_pref )
      //    {
      //      if( sd_sr_ptr->sys.chan ==
      //          acq_rec.type.wcdma_generic.rec[i].uarfcn )
      //      {
      //        return SDPRL_MATCH_SID_MODE_BAND_CHAN;
      //      }
      //      else
      //      {
      //        band_is_match = TRUE;
      //      }
      //    }
      //  }
      //  else
      //  {
      //    multi_band_pref |= band_pref;
      //  }
      //}  /* for */
      //
      ///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      //
      ///* If band match but not channel.
      //*/
      //if( band_is_match ||
      //    sd_sr_ptr->sys.band & multi_band_pref )
      //{
      //  return SDPRL_MATCH_SID_MODE_BAND;
      //}

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Check for band match.
      */
      band_pref = SD_SS_BAND_PREF_NONE;

      for( i=0; i < acq_rec.type.wcdma_generic.num_bands; i++)
      {
        band = prl_map_wcdma_band_to_band(
             (prl_wcdma_band_e_type)acq_rec.type.wcdma_generic.rec[i].band );
        band_pref = SD_BAND_ADD( band_pref,
                                 sdss_map_band_to_band_pref( band ) );
      }

      if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.band, band_pref ) )
      {
        return SDPRL_MATCH_SID_MODE_BAND_CHAN;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here,no band match -
      ** return SDPRL_MATCH_SID_MODE.
      */
      return SDPRL_MATCH_SID_MODE;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GW_PREFERRED:

      band_pref = SD_SS_BAND_PREF_NONE;

      /* Use acquired system mode from sid if system record is GW mode.
       */
      if( sd_sr_ptr->sys.mode == SD_MODE_GW )
      {
        mode = sd_sr_ptr->sid.gw.mode;
      }
      else
      {
        mode = sd_sr_ptr->sys.mode;
      }

      switch( mode )/*lint -e788 */ /* enum constand not used */
      {
        case SD_MODE_GSM:

          #if defined(FEATURE_GSM)
          /* Check if the system's band matches one of the GSM bands.
          */
          for( i=0; i < acq_rec.type.gw_pref.num_gsm_bands; i++ )
          {
            band = prl_map_gsm_band_to_band(
                      (prl_gsm_band_e_type)acq_rec.type.gw_pref.gsm_band[i]);
            band_pref = SD_BAND_ADD( band_pref,
                                     sdss_map_band_to_band_pref(band));
          }

          if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.band, band_pref ))
          {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }

          /* If we got here, band does not match.
          */
          return SDPRL_MATCH_SID_MODE;
          #else
          /* Mode does not match.
          */
          return SDPRL_MATCH_SID;
          #endif /* defined(FEATURE_GSM) */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_WCDMA:

          #if defined(FEATURE_WCDMA)
#error code not present
          #else
          /* Mode does not match.
          */
          return SDPRL_MATCH_SID;
          #endif /* defined(FEATURE_WCDMA) */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:

          /* Mode does not match.
          */
          return SDPRL_MATCH_SID;

      } /* switch */      /*lint +e788 */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_RESERVED:
    case PRL_ACQ_REC_NONE:
    default:
      SD_MSG_LOW( "Unknown acq_rec_type %d", acq_rec.acq_type,0,0 );
      break;

  } /* switch( acq_rec.acq_type ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the mode, band and/or CDMA channel/AMPS system do not
  ** match - return SDPRL_MATCH_SID.
  */
  return SDPRL_MATCH_SID;

} /* sdprl_sys_match */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_sys_is_match

DESCRIPTION
  Compare a SD system record against a PRL system record. Return true if they
  match.

  For SD system records with an explicit SID, a match is declared if the SID,
  NID, mode and band-class of the SD system record match the PRL.

  For SD system records with a wildcard SID, a match is declared if the SID,
  NID, mode, band-class, and CDMA channel or analog system of the SD system
  record match the PRL.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the SD system record matches the PRL system record. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_sys_is_match(

        const sdsr_s_type         *sd_sr_ptr,
            /* Pointer to the SD system record to be compared.
            */

        const prli_sys_rec_type   *prl_sr_ptr,
            /* Pointer to the PRL system record against which to compare.
            */

        sdprl_match_sid_nid_e_type *match_lvl_ptr
            /* Pointer to PRL sid nid match enum type.
            */

)
{
  sdprl_match_e_type  match;
  boolean             is_match = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( prl_sr_ptr != NULL );
  SD_ASSERT( sd_sr_ptr  != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially assume no match.
  */
  if( match_lvl_ptr != NULL )
  {
    *match_lvl_ptr = SDPRL_MATCH_SID_NID_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the level of matching.
  */
  match = sdprl_sys_match( sd_sr_ptr, prl_sr_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SID/NID, mode, band and channel match, return TRUE.
  */
  if( match == SDPRL_MATCH_SID_MODE_BAND_CHAN )
  {
    is_match = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is not a wildcard SID, do the following checks.
  */
  else if( prl_sys_wildcard(prl_sr_ptr) != PRL_WCARD_SID )
  {
    /* If SID, mode and band match, this is sufficient.
    */
    if( match == SDPRL_MATCH_SID_MODE_BAND )
    {
      is_match = TRUE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, SID and mode match is sufficient for the following records.
    */
    else if( match == SDPRL_MATCH_SID_MODE )
    {
      /* IS856 (HDR) record.
      */
      if( prl_sr_ptr->rec_type == PRL_SYS_REC_IS856 )
      {
        is_match = TRUE;
      }

      /* PLMN (GSM/UMTS) record.
      */
      else if( prl_sr_ptr->rec_type == PRL_SYS_REC_PLMN )
      {
        is_match = TRUE;
      }
      /* MCC/IMSI_11_12 based CDMA record (i.e. SID=PRL_MCC_SID_0 or
      ** or PRL_MCC_SID_0).
      */
      else if( prl_sr_ptr->rec_type == PRL_SYS_REC_IS2000
                                  &&
               sd_sr_ptr->sys.mode == SD_MODE_CDMA
                                  &&
               PRL_IS_MCC_SID(prl_sr_ptr->rec.is2000.sid) )
      {
        is_match = TRUE;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we have a match, decide on the match level.
  */
  if( is_match )
  {
    if( match_lvl_ptr != NULL)
    {
      /* Checking wildcard SID and NID.
      */
      switch( prl_sys_wildcard(prl_sr_ptr) )
      {
        case PRL_WCARD_NOT:
          *match_lvl_ptr = SDPRL_MATCH_SID_NID;
          break;
        case PRL_WCARD_SID:
          *match_lvl_ptr = SDPRL_MATCH_WILDCARD_SID;
          break;
        case PRL_WCARD_NID:
          *match_lvl_ptr = SDPRL_MATCH_WILDCARD_NID;
          break;
        case PRL_WCARD_UNKNOWN:
        case PRL_WCARD_MAX:
        default:
          SD_CASE_ERR( "Unexpected PRL wildcard enum type ",
                       prl_sys_wildcard(prl_sr_ptr),0,0 );
          break;
      }
    }

    /* Return TRUE to indicate a match.
    */
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, there is no match - return FALSE.
  */
  return FALSE;

} /* sdprl_sys_is_match */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_home_sid_nid

DESCRIPTION
  Check whether a specified system record has a SID/NID that matches one
  of the home SID/NIDs.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the specified system record has a SID/NID that matches one of the
  home SID/NIDs.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_is_home_sid_nid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to system record to be checked against the home
            ** SID/NID list.
            */

        sd_nam_e_type             nam
            /* NAM for which to check for a home SID/NID match.
            */
)
{
  sdprl_s_type              *sdprl_ptr = &sdprl;
  const sdprl_nam_s_type    *nam_ptr;
  int                       i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Home SID/NID list is not applicable for HDR and PLMN systems.
  */
  if( sr_ptr->sys.mode == SD_MODE_HDR   || \
      sr_ptr->sys.mode == SD_MODE_GSM   || \
      sr_ptr->sys.mode == SD_MODE_WCDMA || \
      sr_ptr->sys.mode == SD_MODE_GW    || \
      sr_ptr->sys.mode == SD_MODE_WLAN  || \
      sr_ptr->sys.mode == SD_MODE_MAX )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the structure that contains the information of the specified
  ** NAM.
  */
  nam_ptr = &(sdprl_ptr->nam_info[nam]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether the SID/NID of the system record that is pointed to by
  ** sr_ptr matches one of the SID/NIDs that are stored in the home_sid_nid
  ** list. Return TRUE if so. Note that if FEATURE_SSPR_ENHANCEMENTS is not
  ** defined, this check should only be done for digital systems.
  */
  #ifndef FEATURE_SSPR_ENHANCEMENTS
  if( sr_ptr->sys.mode != SD_MODE_AMPS )
  #endif
  {
    for( i=0;
         i < (int) ARR_SIZE(nam_ptr->home_sid_nid) &&
         nam_ptr->home_sid_nid[i].sid != SD_WILDCARD_SID;
         i++ )
    {
      if( (sr_ptr->sid.is95.sid == nam_ptr->home_sid_nid[i].sid)
                            &&
          (sr_ptr->sys.mode == SD_MODE_AMPS ||
           sr_ptr->sid.is95.nid == nam_ptr->home_sid_nid[i].nid ||
           SD_WILDCARD_NID      == nam_ptr->home_sid_nid[i].nid) )
      {
        return TRUE;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For an AMPS system also check whether the SID matches the amps_home_sid.
  */
  if( sr_ptr->sys.mode == SD_MODE_AMPS &&
      sr_ptr->sid.is95.sid == nam_ptr->amps_home_sid )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the system's SID/NID does not matches any of the home
  ** SID/NIDs - return FALSE.
  */
  return FALSE;

} /* sdprl_is_home_sid_nid */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_lock_sid_nid

DESCRIPTION
  Check whether a specified system record has a SID/NID that matches one
  of the locked SID/NIDs.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the specified system record has a SID/NID that matches one of the
  locked SID/NIDs.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_is_lock_sid_nid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to system record to be checked against the locked
            ** SID/NID list.
            */

        sd_nam_e_type             nam
            /* NAM for which to check for a locked SID/NID match.
            *//*lint -esym(715, sr_ptr, nam ) */ /* Symbol not used*/
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_s_type              *sdprl_ptr = &sdprl;
  const sdprl_nam_s_type    *nam_ptr;
  int                       i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Locked SID/NID list is not applicable for HDR and PLMN systems.
  */
  if( sr_ptr->sys.mode != SD_MODE_CDMA  && \
      sr_ptr->sys.mode != SD_MODE_AMPS
    )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the structure that contains the information of the specified
  ** NAM.
  */
  nam_ptr = &(sdprl_ptr->nam_info[nam]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether the SID/NID of the system record that is pointed to by
  ** sr_ptr matches one of the SID/NIDs that are stored in the lock_sid_nid
  ** list. Return TRUE if so. Else return FALSE.
  */
  for( i=0; i < (int) ARR_SIZE(nam_ptr->lock_sid_nid); i++ )
  {
    if( sr_ptr->sid.is95.sid == nam_ptr->lock_sid_nid[i].sid
                             &&
        (SD_WILDCARD_NID      == nam_ptr->lock_sid_nid[i].nid ||
         sr_ptr->sid.is95.nid == nam_ptr->lock_sid_nid[i].nid) )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the system's SID/NID does not matches any of the locked
  ** SID/NIDs - return FALSE.
  */
  return FALSE;

} /* sdprl_is_lock_sid_nid *//*lint +esym(715, sr_ptr, nam) */


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
MMEXTN  boolean                   sdprl_add_lock_sid_nid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to system record to be added to the locked
            ** SID/NID list.
            */

        boolean                   ignore_nid
            /* Set nid as WILDCARD
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_s_type              *sdprl_ptr = &sdprl;
  sdprl_nam_s_type          *nam_ptr;
  unsigned int              i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sr_ptr == NULL)
  {
    SD_MSG_ERROR("sr_ptr NULL",0,0,0);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Locked SID/NID list is not applicable for HDR and PLMN systems.
  */
  if( sr_ptr->sys.mode != SD_MODE_CDMA && \
      sr_ptr->sys.mode != SD_MODE_AMPS
    )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the structure that contains the information of the specified
  ** NAM.
  */
  nam_ptr = &(sdprl_ptr->nam_info[sdprl_ptr->curr_nam]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether the SID/NID of the system record that is pointed to by
  ** sr_ptr matches one of the SID/NIDs that are stored in the lock_sid_nid
  ** list. Return TRUE if so.
  */
  for( i=0; i < (int) ARR_SIZE(nam_ptr->lock_sid_nid); i++ )
  {
    if( sr_ptr->sid.is95.sid == nam_ptr->lock_sid_nid[i].sid
                             &&
        (SD_WILDCARD_NID      == nam_ptr->lock_sid_nid[i].nid ||
          (!ignore_nid &&
           sr_ptr->sid.is95.nid == nam_ptr->lock_sid_nid[i].nid)) )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find free entry
  */
  for( i=0; i < (int) ARR_SIZE(nam_ptr->lock_sid_nid); i++ )
  {
    if( nam_ptr->lock_sid_nid[i].sid == SD_WILDCARD_SID &&
        nam_ptr->lock_sid_nid[i].nid == SD_WILDCARD_NID
      )
    {
      break;
    }
  }

  /* Check if we found the entry.
  */
  if ( i < ARR_SIZE(nam_ptr->lock_sid_nid))
  {
    /* Found entry.
    */
    nam_ptr->lock_sid_nid[i].sid = sr_ptr->sid.is95.sid;
    nam_ptr->lock_sid_nid[i].nid = sr_ptr->sid.is95.nid;
    if ( ignore_nid )
    {
      nam_ptr->lock_sid_nid[i].nid = SD_WILDCARD_NID;
    }
    return TRUE;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no free entry exists.
  */
  return FALSE;

} /* sdprl_add_lock_sid_nid */




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
MMEXTN  sd_cell_sys_e_type        sdprl_get_home_side(

        sd_nam_e_type             nam,
            /* NAM for which to get the cellular side.
            */

        sd_mode_e_type            mode
            /* Mode for which to get the cellular side.
            */
)
{

  sdprl_s_type      *sdprl_ptr = &sdprl;
  sdprl_nam_s_type  *nam_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(nam, 0, SD_NAM_MAX) );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no is specified, use current NAM.
  */
  if( nam == SD_NAM_MAX )
  {
    nam = sdprl_ptr->curr_nam;
  }

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nam_ptr = &(sdprl_ptr->nam_info[nam]);

  switch( mode )
  {
    case SD_MODE_AMPS:

      #ifdef FEATURE_JCDMA_NV
      if( nam_ptr->amps_home_sid == SD_WILDCARD_SID )
      {
        return SD_CELL_SYS_A;
      }
      #endif /* FEATURE_JCDMA_NV */

      if( nam_ptr->amps_home_sid & 0x0001 )
      {
        return SD_CELL_SYS_A;
      }

      return SD_CELL_SYS_B;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_CDMA_:
    case SD_MODE_HDR_:

      #ifdef FEATURE_JCDMA_NV
      if( nam_ptr->home_sid_nid[0].sid == SD_WILDCARD_SID )
      {
        return SD_CELL_SYS_A;
      }
      #endif /* FEATURE_JCDMA_NV */

      if( nam_ptr->home_sid_nid[0].sid & 0x0001 )
      {
        return SD_CELL_SYS_A;
      }

      return SD_CELL_SYS_B;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_WLAN:
    case SD_MODE_GW:
    case SD_MODE_MAX:
    default:
      SD_CASE_ERR( "mode",0,0,0 );
      return SD_CELL_SYS_MAX;
  }

} /* sdprl_get_home_side */




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=============================== PRL PARSER ==================================
=============================================================================
=============================================================================
===========================================================================*/

/* Type for holding references to a GEO, more-preferred and collocated lists
** along with corresponding bit masks that indicated the PRL's acquisition
** table entries that were appended to these lists.
*/
typedef struct {

  /* The system record list into which to build the GEO list.
  */
  sdsr_e_type       geo_lst;

  /* The system record list into which to build the more preferred list.
  */
  sdsr_e_type       pref_lst;

  /* The system record list into which to build the collocated list.
  */
  sdsr_e_type       colloc_lst;

  /* Bit masks that indicates the PRL's acquisition table entries that were
  ** appended to geo_lst, pref_lst and colloc_lst, respectively.
  */
  byte              geo_mask[64];
  byte              pref_mask[64];
  byte              colloc_mask[64];

} sdprl_parser_lists_s_type;


/* Byte masks array to isolate a particular bit in a byte.
*/
static byte sdprl_byte_mask_arr[] =
                          { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };


/* Macro to test whether a particular bit in a bit-mask is on.
*/
#define SDPRL_BIT_IS_ON( mask_arr, bit_pos ) \
    ( mask_arr[ bit_pos/8 ] & sdprl_byte_mask_arr[bit_pos & 0x07] )


/* Macro to set a particular bit in a bit-mask to on.
*/
#define SDPRL_BIT_SET( mask_arr, bit_pos ) \
    ( mask_arr[ bit_pos/8 ] |= sdprl_byte_mask_arr[bit_pos & 0x07] )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*===========================================================================

FUNCTION sdprl_parser_get

DESCRIPTION
  Extract the system table entry that is currently pointed to by the parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  Length of system table entry if entry is successfully extracted.
  0 otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  dword                      sdprl_parser_get(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

        prli_sys_rec_type         *sys_rec
            /* Pointer to a buffer where to store the system record that
            ** is extracted from the PRL.
            */
)
{
  dword                offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sys_rec != NULL );
  SD_ASSERT( parser_ptr->prl_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we reached the end of the system table, return 0.
  */
  if( parser_ptr->sys_idx >= parser_ptr->sys_tbl_num )
  {
    return 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset = prl_extract_sys_rec( (byte*) parser_ptr->prl_ptr,
                                parser_ptr->sys_offset,
                                sys_rec,
                                parser_ptr->sspr_p_rev );

  SD_ASSERT( offset > 0 &&
             offset > parser_ptr->sys_offset );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( offset - parser_ptr->sys_offset );

} /* sdprl_parser_get */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_init

DESCRIPTION
  Initialize a PRL-parser with a new PRL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                            sdprl_parser_init(

        sdprl_parser_s_type             *parser_ptr,
            /* Pointer to a PRL-parser item to be initialized with the
            ** PRL that is pointed to by the prl_ptr parameter.
            */

        sd_ss_e_type                    ss,
            /* System selection - MAIN or HDR.
            */

        const byte                      *prl_ptr,
            /* Pointer to a PRL RAM image with which to initialize the
            ** PRL-parser that is pointed to by the parser_ptr parameter.
            */

        const prl_hdr_s_type            *prl_hdr_ptr
            /* Pointer to buffer that holds information about the PRL that is
            ** pointed to by prl_ptr.
            */
)
{
  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( prl_ptr != NULL );
  SD_ASSERT( prl_hdr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //SD_FUNC_START( "sdprl_parser_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  parser_ptr->ss              = ss;

  parser_ptr->prl_ptr         = prl_ptr;

  parser_ptr->sys_tbl_offset  = prl_hdr_ptr->sys_tbl_bit_offset;

  parser_ptr->sys_tbl_num     = prl_hdr_ptr->num_sys_recs;

  parser_ptr->sspr_p_rev      = prl_hdr_ptr->sspr_p_rev;

  parser_ptr->sys_idx         = 0;
  parser_ptr->sys_offset      = parser_ptr->sys_tbl_offset;

  parser_ptr->geo_idx         = 0;
  parser_ptr->geo_offset      = parser_ptr->sys_tbl_offset;

  parser_ptr->pri_idx         = 0;
  parser_ptr->pri_offset      = parser_ptr->sys_tbl_offset;
  parser_ptr->pri_is_higher   = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract and store the content of the first system table entry in the
  ** sys_rec field.
  */
  parser_ptr->sys_rec_len = sdprl_parser_get( parser_ptr,
                                              &parser_ptr->sys_rec );
  SD_ASSERT( parser_ptr->sys_rec_len > 0 );

} /* sdprl_parser_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_lists_init

DESCRIPTION
  Initialize an item of type sdprl_parser_lists_s_type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                            sdprl_parser_lists_init(

        sdprl_parser_lists_s_type       *parser_lists_ptr,
            /* Pointer to a an item of type sdprl_parser_lists_s_type
            ** to be initialized.
            */

        sdsr_e_type                     geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type                     pref_lst,
            /* The system record list into which to build the more-preferred
            ** list.
            */

        sdsr_e_type                     colloc_lst
            /* The system record list into which to build the collocated
            ** list.
            */
)
{
  SD_ASSERT( parser_lists_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE_OR( geo_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( pref_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_lst, SDSR_1ST_SYS, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize geo_mask, pref_mask and colloc_mask to all 0's.
  */
  memset( parser_lists_ptr, 0, sizeof(*parser_lists_ptr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize geo_lst, pref_lst and colloc_lst per the input lists.
  */
  parser_lists_ptr->geo_lst = geo_lst;
  parser_lists_ptr->pref_lst = pref_lst;
  parser_lists_ptr->colloc_lst = colloc_lst;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the geo_lst, pref_lst and colloc_lst.
  */
  sdsr_list_clr( geo_lst );
  sdsr_list_clr( pref_lst );
  sdsr_list_clr( colloc_lst );

} /* sdprl_parser_lists_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_next

DESCRIPTION
  Increment the PRL parser to point at the next system record (of the PRL
  with which the parser is currently associated). Return the content of
  newly pointed record via the rtrn_sys_rec_ptr parameter.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if next system record is successfully extracted. FALSE otherwise.

SIDE EFFECTS
  Increment the system table index and offset to point at the next system
  table entry.

  Increment the GEO index and offset as necessary.

  Increment the PREF index and offset as necessary.

===========================================================================*/
static  boolean                   sdprl_parser_next(

        sdprl_parser_s_type       *parser_ptr,
            /* Pointer to a PRL-parser item to be initialized with the
            ** PRL that is pointed to by the prl_ptr parameter.
            */

        prli_sys_rec_type         *rtrn_sys_rec_ptr
            /* Pointer to a buffer where to store the system record that
            ** is extracted from the PRL.
            */
)
{
  word                curr_sys_idx;
  dword               curr_sys_offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( rtrn_sys_rec_ptr != NULL );
  SD_ASSERT( parser_ptr->prl_ptr != NULL );

  curr_sys_idx    = parser_ptr->sys_idx;
  curr_sys_offset = parser_ptr->sys_offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we reached the end of the system table, return FALSE.
  */
  if( parser_ptr->sys_idx+1 >= parser_ptr->sys_tbl_num )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the system offset per the offset of the next system table entry
  ** and increment the system table index.
  */
  parser_ptr->sys_offset += parser_ptr->sys_rec_len;
  parser_ptr->sys_idx++;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the newly pointed system entry. If fails to extract, return
  ** FALSE.
  */
  parser_ptr->sys_rec_len = sdprl_parser_get(parser_ptr, rtrn_sys_rec_ptr);

  if( parser_ptr->sys_rec_len == 0 )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the GEO field of the newly pointed system entry, indicates that this
  ** entry is in a new GEO (compare to the previous entry), set the GEO and
  ** PRI offset and index per the newly pointed entry.
  */
  if( rtrn_sys_rec_ptr->geo == PRL_NEW_GEO_REGION )
  {
    parser_ptr->geo_idx       = parser_ptr->sys_idx;
    parser_ptr->geo_offset    = parser_ptr->sys_offset;

    parser_ptr->pri_is_higher = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, if the previous entry is a preferred system and either the
  ** previous entry PRI field indicates that the previous entry is more
  ** desirable than the newly pointed entry or the newly pointed entry is
  ** a negative system, set the PRI offset and index per the previous entry.
  */
  else if( (parser_ptr->sys_rec.neg_pref == PRL_PREFERRED)
                           &&
           (parser_ptr->sys_rec.pri == PRL_PRI_HIGHER ||
            rtrn_sys_rec_ptr->neg_pref == PRL_NEGATIVE) )
  {
    parser_ptr->pri_idx       = curr_sys_idx;
    parser_ptr->pri_offset    = curr_sys_offset;
    parser_ptr->pri_is_higher = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the content of newly pointed record in the sys_rec field.
  */
  parser_ptr->sys_rec = *rtrn_sys_rec_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sdprl_parser_next */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_next_match

DESCRIPTION
  Increment the PRL parser to point at the next system record entry (of the
  PRL with which the parser is currently associated) that is matching the
  information of the system record that is pointed to by sr_ptr. Return the
  content of newly pointed system record entry via the rtrn_sys_rec_ptr
  parameter.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if a matching system record entry is found. FALSE otherwise.

SIDE EFFECTS
  Increment the system table index and offset to point at the matching system
  table entry.

  Increment the GEO index and offset as necessary.

  Increment the PREF index and offset as necessary.

===========================================================================*/
static  boolean                   sdprl_parser_next_match(

        sdprl_parser_s_type       *parser_ptr,
            /* Pointer to a PRL-parser item to be initialized with the
            ** PRL that is pointed to by the prl_ptr parameter.
            */

        const sdsr_s_type         *sr_ptr,
            /* System record to match.
            */

        prli_sys_rec_type         *rtrn_sys_rec_ptr,
            /* Pointer to a buffer where to store the matching system record
            ** that is extracted from the PRL.
            */

        sdprl_match_sid_nid_e_type *match_lvl_ptr
            /*  Pointer to SID/NID matching level of system record and PRL.
            */
)
{
  //int                 i;
  dword               sys_offset;
  //byte                sys_rec_len;
  //byte                subnet_len;
  byte                *buf_ptr;
  byte                *entry_ptr;
  prli_sys_rec_type   *sys_rec_ptr;

  boolean             is_prl_match    = FALSE;

  dword               dword_buf;
  dword               word_buf;

  dword               prev_offset;
  prl_neg_pref_type   prev_neg_pref;
  prl_pri_type        prev_pri;


  //byte                mask;
  //static const byte   mask_tbl[]      = { 0x00, 0x80, 0xC0, 0xE0,
  //                                        0xF0, 0xF8, 0xFC, 0xFE };

  /* Mask to mask 3 LSB's.
  */
  #define LSB3_MASK  (0x07)

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( rtrn_sys_rec_ptr != NULL );

  SD_ASSERT( parser_ptr->prl_ptr != NULL );

  if( match_lvl_ptr !=NULL )
  {
    *match_lvl_ptr = SDPRL_MATCH_SID_NID_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (parser_ptr->sspr_p_rev) {

    case PRL_SSPR_P_REV_1:

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we reached the end of the system table, return FALSE.
      */
      if( parser_ptr->sys_idx+1 >= parser_ptr->sys_tbl_num )
      {
        return FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Set sys_rec_ptr to point at the buffer containing the most recent
      ** extracted system record .
      */
      sys_rec_ptr   = &parser_ptr->sys_rec;

      /* Remember the offset, PREF_NEG and PRI field of current system record.
      */
      prev_offset   = parser_ptr->sys_offset;
      prev_neg_pref = sys_rec_ptr->neg_pref;
      prev_pri      = sys_rec_ptr->pri;

      /* Set buf_ptr to point at the beginning of the PRL RAM image.
      */
      buf_ptr       = (byte*) parser_ptr->prl_ptr;

      /* Set sys_offset per the bit offset of the next system table entry
      ** (relative to the beginning of the PRL RAM image).
      */
      sys_offset    = parser_ptr->sys_offset + parser_ptr->sys_rec_len;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Traverse through the system record table and look for a matching
      ** system record entry.
      */
      for( parser_ptr->sys_idx++;
           parser_ptr->sys_idx < parser_ptr->sys_tbl_num;
           parser_ptr->sys_idx++ )
      {
        /* Set the system sys_offset per the offset of the system table entry
        ** that is being extracted.
        */
        parser_ptr->sys_offset = sys_offset;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Copy the first 4 bytes.
        */
        entry_ptr = &buf_ptr[ sys_offset >> 3 ];
        dword_buf = BYTES_TO_DWORD( entry_ptr[0],
                                    entry_ptr[1],
                                    entry_ptr[2],
                                    entry_ptr[3] );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Align entry with beginning of buffer by shifting to the left as
        ** necessary.
        */
        dword_buf <<= (sys_offset & LSB3_MASK);

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Extract the SID field.
        */
        sys_rec_ptr->rec.is2000.sid = (word) ( (dword_buf>>17) & 0x7FFF );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If NID is included, skip the NID field and copy the next two bytes
        ** (i.e., PREF_NEG, GEO and PRI fields) into the word buffer.
        */
        if( dword_buf & 0x00008000 )
        {
          sys_offset += 16;

          word_buf = BYTES_TO_WORD( entry_ptr[4], entry_ptr[5] );
          /*lint -save -e734 word_buf loss of precision */
          word_buf <<= (sys_offset & LSB3_MASK);
          /*lint -restore */

          sys_rec_ptr->neg_pref = (prl_neg_pref_type)  ((word_buf>>14) & 0x0001);
          sys_rec_ptr->geo      = (prl_geo_region_type)((word_buf>>13) & 0x0001);
          sys_rec_ptr->pri      = (prl_pri_type)       ((word_buf>>12) & 0x0001);

        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else (NID is not included), the PREF_NEG, GEO and PRI fields are
        ** already in the dword buffer.
        */
        else
        {
          sys_rec_ptr->neg_pref = (prl_neg_pref_type)
                                  ((dword_buf >> 14) & 0x00000001);

          sys_rec_ptr->geo      = (prl_geo_region_type)
                                  ((dword_buf >> 13) & 0x00000001);

          sys_rec_ptr->pri      = (prl_pri_type)
                                  ((dword_buf >> 12) & 0x00000001);
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Increment sys_offset as necessary.
        */
        sys_offset += 28;

        /* If preferred entry add 9.
        */
        if( sys_rec_ptr->neg_pref == PRL_PREFERRED ) {
          sys_offset += 9;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If the GEO field is set to 1, indicates that this entry is in a new
        ** GEO (compare to the previous entry) by setting the GEO offset and
        ** index per the newly pointed entry.
        */
        if( sys_rec_ptr->geo == PRL_NEW_GEO_REGION )
        {
          parser_ptr->geo_idx       = parser_ptr->sys_idx;
          parser_ptr->geo_offset    = parser_ptr->sys_offset;

          /* Indicate that no system is more preferred.
          */
          parser_ptr->pri_is_higher = FALSE;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else, if the previous entry is a preferred system and either the
        ** previous entry PRI field indicates that the previous entry is more
        ** desirable than the current entry or the current entry is a negative
        ** system, set the PRI offset and index per the previous entry.
        */
        else if( (prev_neg_pref == PRL_PREFERRED)
                             &&
                 (prev_pri == PRL_PRI_HIGHER ||
                  sys_rec_ptr->neg_pref == PRL_NEGATIVE) )
        {
          parser_ptr->pri_is_higher = TRUE;
          parser_ptr->pri_idx       = parser_ptr->sys_idx - 1;
          parser_ptr->pri_offset    = prev_offset;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Remember the offset, PREF_NEG and PRI fields of current system record.
        */
        prev_offset   = sys_offset;
        prev_neg_pref = sys_rec_ptr->neg_pref;
        prev_pri      = sys_rec_ptr->pri;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If we find a matching SID or a wildcard SID, check for a full
        ** match.
        */
        if( sys_rec_ptr->rec.is2000.sid == sr_ptr->sid.is95.sid ||
            sys_rec_ptr->rec.is2000.sid == SD_WILDCARD_SID )
        {
          /* Extract the complete system record entry.
          */
          parser_ptr->sys_rec_len = sdprl_parser_get(parser_ptr,sys_rec_ptr);
          SD_ASSERT( parser_ptr->sys_rec_len > 0 );

          if( sdprl_sys_is_match(sr_ptr, sys_rec_ptr, match_lvl_ptr) )
          {
            is_prl_match = TRUE;
            *rtrn_sys_rec_ptr = *sys_rec_ptr;
            break;
          }

        } /* if SID match */

      } /* while */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SSPR_P_REV_3:

      /* If we reached the end of the system table, return FALSE.
      */
      if( parser_ptr->sys_idx+1 >= parser_ptr->sys_tbl_num )
      {
        return FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Set sys_rec_ptr to point at the buffer containing the most recent
      ** extracted system record .
      */
      sys_rec_ptr   = &parser_ptr->sys_rec;

      /* Remember the offset, PREF_NEG and PRI field of current system record.
      */
      prev_offset   = parser_ptr->sys_offset;
      prev_neg_pref = sys_rec_ptr->neg_pref;
      prev_pri      = sys_rec_ptr->pri;

      /* Set buf_ptr to point at the beginning of the PRL RAM image.
      */
      buf_ptr       = (byte*) parser_ptr->prl_ptr;

      /* Set sys_offset per the bit offset of the next system table entry
      ** (relative to the beginning of the PRL RAM image).
      */
      sys_offset    = parser_ptr->sys_offset + parser_ptr->sys_rec_len;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Traverse through the system record table and look for a matching system
      ** record entry.
      */
      for( parser_ptr->sys_idx++;
           parser_ptr->sys_idx < parser_ptr->sys_tbl_num;
           parser_ptr->sys_idx++ )
      {
        /* Set the system sys_offset per the offset of the system table entry
        ** that is being extracted.
        */
        parser_ptr->sys_offset = sys_offset;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    //rotemc - optimize this
    //    /* Point at the system entry.
    //    */
    //    entry_ptr = &buf_ptr[ sys_offset >> 3 ];
    //    //word_buf = BYTES_TO_WORD( entry_ptr[0],
    //    //                          entry_ptr[1] );
    //
    //    //                            entry_ptr[2],
    //    //                            entry_ptr[3] );
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    ///* Align entry with beginning of buffer by shifting to the left as
    //    //** necessary.
    //    //*/
    //    //dword_buf <<= (sys_offset & LSB3_MASK);
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    /* Extract the length and type fields.
    //    */
    //    sys_rec_len           =                       ((entry_ptr[0]>>3) & 0x1F);
    //    sys_rec_ptr->rec_type = (prl_sys_rec_type)    ((entry_ptr[1]>>7) & 0x01);
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    /* Extract the PREF_NEG, GEO and PRI fields.
    //    */
    //    sys_rec_ptr->neg_pref = (prl_neg_pref_type)   ((entry_ptr[1]>>6) & 0x01);
    //    sys_rec_ptr->geo      = (prl_geo_region_type) ((entry_ptr[1]>>5) & 0x01);
    //    sys_rec_ptr->pri      = (prl_pri_type)        ((entry_ptr[1]>>4) & 0x01);
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    /* Extract the system ID field.
    //    */
    //    switch( sys_rec_ptr->rec_type )
    //    {
    //      case PRL_SYS_REC_IS2000:
    //        sys_rec_ptr->rec.is2000.sid = BYTES_TO_WORD( entry_ptr[3],
    //                                                     entry_ptr[4] );
    //        sys_rec_ptr->rec.is2000.sid = (sys_rec_ptr->rec.is2000.sid>>1)&7F;
    //        break;
    //
    //      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //      case PRL_SYS_REC_IS856:
    //
    //        subnet_len = (entry_ptr[3] & 0x7F);
    //
    //        for( i=0; i<subnet_len/8; i++ )
    //        {
    //          sys_rec_ptr->rec.is856.subnet[i] = entry_ptr[4+i];
    //        }
    //
    //        /* Copy the reminder, as needed.
    //        */
    //        if( i < ARR_SIZE(sys_rec_ptr->rec.is856.subnet) )
    //        {
    //          mask = mask_tbl[ sys_rec_ptr->rec.is856.subnet_length & 0x07 ];
    //          sys_rec_ptr->rec.is856.subnet[i] = entry_ptr[4+i] & mask;
    //        }
    //        break;
    //
    //      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //      case PRL_SYS_REC_MAX:
    //      default:
    //        SD_CASE_ERR( "sys_rec_type",0,0,0 );
    //        break;
    //
    //     } /* switch */
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    /* Increment sys_offset as necessary.
    //    */
    //    sys_offset += sys_rec_len*8;

        sys_offset = prl_extract_sys_rec( buf_ptr,
                                          sys_offset,
                                          sys_rec_ptr,
                                          parser_ptr->sspr_p_rev );

        SD_ASSERT( sys_offset > 0 );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If the GEO field is set to 1, indicates that this entry is in a new
        ** GEO (compare to the previous entry) by setting the GEO offset and
        ** index per the newly pointed entry.
        */
        if( sys_rec_ptr->geo == PRL_NEW_GEO_REGION )
        {
          parser_ptr->geo_idx       = parser_ptr->sys_idx;
          parser_ptr->geo_offset    = parser_ptr->sys_offset;

          /* Indicate that no system is more preferred.
          */
          parser_ptr->pri_is_higher = FALSE;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else, if the previous entry is a preferred system and either the
        ** previous entry PRI field indicates that the previous entry is more
        ** desirable than the current entry or the current entry is a
        ** negative system, set the PRI offset and index per the previous
        ** entry.
        */
        else if( (prev_neg_pref == PRL_PREFERRED)
                             &&
                 (prev_pri == PRL_PRI_HIGHER ||
                  sys_rec_ptr->neg_pref == PRL_NEGATIVE) )
        {
          parser_ptr->pri_is_higher = TRUE;
          parser_ptr->pri_idx       = parser_ptr->sys_idx - 1;
          parser_ptr->pri_offset    = prev_offset;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Remember the offset, PREF_NEG and PRI fields of current system
        ** record.
        */
        prev_offset   = sys_offset;
        prev_neg_pref = sys_rec_ptr->neg_pref;
        prev_pri      = sys_rec_ptr->pri;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If we find a matching SID or a wildcard SID, check for a full
        ** match.
        */
        if( sdprl_is_sid_match(sr_ptr, sys_rec_ptr) ||
            prl_sys_wildcard(sys_rec_ptr) == PRL_WCARD_SID )
        {
          /* Extract the complete system record entry.
          */
          parser_ptr->sys_rec_len = sdprl_parser_get( parser_ptr, sys_rec_ptr );
          SD_ASSERT( parser_ptr->sys_rec_len > 0 );

          if( sdprl_sys_is_match(sr_ptr, sys_rec_ptr, match_lvl_ptr) )
          {
            is_prl_match = TRUE;
            *rtrn_sys_rec_ptr = *sys_rec_ptr;
            break;
          }

        } /* if SID match */

      } /* for */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SSPR_P_REV_RESERVED:
    case PRL_SSPR_P_REV_INVALID:
    default:
      SD_MSG_ERROR("Invalid SSPR_P_REV %d", parser_ptr->sspr_p_rev, 0, 0);
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #undef LSB3_MASK

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate whether a matching system table entry was found.
  */
  return is_prl_match;

} /* sdprl_parser_next_match */





/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_app_geo

DESCRIPTION
  Append the GEO of the system table entry that is currently pointed to by
  the parser to the specified system record list.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_parser_app_geo(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

        sdprl_parser_lists_s_type *parser_lists_ptr,
            /* Pointer to a parser lists structure.
            */

        sdprl_extract_e_type      extract
            /* Indicate whether only GEO systems that comply with the current
            ** roaming preferences should be extracted.
            */
)
{
  sdprl_parser_s_type   trav_parser;
  prli_sys_rec_type     prl_sr;
  prli_acq_rec_type     prl_ar;
  boolean               is_sys_comply;
  boolean               is_same_idx_reached  = FALSE;
  boolean               is_app_next_sys      = TRUE;

  sdsr_s_type           sr;
  sdprl_s_type          *sdprl_ptr        = &sdprl;
  sdsr_e_type           geo_lst;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( parser_lists_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( extract, SDPRL_EXTRACT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  geo_lst = parser_lists_ptr->geo_lst;

  SD_ASSERT_ENUM_IS_INRANGE_OR( geo_lst, SDSR_1ST_SYS, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Append the first system in the current GEO to the specified list if it
  ** was not appended previously.
  ** Skip over the unknown record.
  */
  if( trav_parser.sys_rec_len > 0 &&
      ! SDPRL_BIT_IS_ON(parser_lists_ptr->geo_mask, prl_sr.acq_index) &&
      ( prl_sr.rec_type == PRL_SYS_REC_IS2000 ||
        prl_sr.rec_type == PRL_SYS_REC_IS856  ||
        prl_sr.rec_type == PRL_SYS_REC_PLMN))
  {
    is_sys_comply = TRUE;

    /* If only systems that comply with the current roaming preference are to
    ** be extracted, determine whether the system that the parser is pointing
    ** to complies with the current roaming preference. If only home systems
    ** are to be extracted, determine whether the system that the parser is
    ** pointing to is a home system.
    */
    if( extract == SDPRL_EXTRACT_ROAM_COMPLY ||
        extract == SDPRL_EXTRACT_HOME )
    {
      /* Set the roaming indicator value based on the PRL and home SID/NID
      ** list.
      */
      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );

      sr.sys.mode = prl_map_acq_rec_to_sd_mode( prl_ar.acq_type );
      prl_sys_rec_get_sid( &prl_sr, &sr.sid );
      sr.roam = prl_sr.roam_ind;
      MSG_FATAL("sdprl_parser_app_geo roam=%d %d",sr.roam,
                sdprl_is_home_sid_nid(&sr, sdprl_ptr->curr_nam),
                sdprl_roam_ind_map_custom_home_to_home(sr.roam));
      if( sdprl_is_home_sid_nid(&sr, sdprl_ptr->curr_nam) &&
          sdprl_roam_ind_map_custom_home_to_home(sr.roam) !=
                                                  SYS_ROAM_STATUS_OFF && 
          (sr.roam == SYS_ROAM_STATUS_ON || 
           sr.roam == SYS_ROAM_STATUS_BLINK)
        ) 
      {
        sr.roam = SYS_ROAM_STATUS_OFF;
      }

      /* If the system is negative or does not comply with roaming
      ** preference, indicate so.
      */
      if( prl_sr.neg_pref == PRL_NEGATIVE ||
          sdss_sr_is_pref_conf(parser_ptr->ss, &sr, SDSS_PREF_CONF_ROAM) )
      {
        is_sys_comply = FALSE;
      }

      /* If looking for home systems (SDPRL_EXTRACT_HOME) and the system is
      ** not a home system, then indicate that the system is not compliant.
      */
      if( extract == SDPRL_EXTRACT_HOME &&
          sdprl_roam_ind_map_custom_home_to_home(sr.roam) !=
                                                  SYS_ROAM_STATUS_OFF )
      {
        is_sys_comply = FALSE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If only systems that are same or better preference are to be
    ** extracted, determine whether the system to which the parser is
    ** pointing is same or better preference.
    */
    else if( extract == SDPRL_EXTRACT_SAME_BETTER )
    {
      /* If the system is negative or is_app_next_sys is FALSE, indicate so.
      */
      if( prl_sr.neg_pref == PRL_NEGATIVE )
      {
        is_sys_comply = FALSE;
      }

      /* If we reached the system that is pointed to by parser_ptr, indicate
      ** so.
      */
      if( trav_parser.sys_idx == parser_ptr->sys_idx ) {
        is_same_idx_reached = TRUE;
      }

      /* If we reached the systems that are of the same priority as the
      ** system that is pointed to by parser_ptr and the current system is
      ** more preferred than next system (i.e. pri==PRL_PRI_HIGHER), set
      ** is_app_next_sys=FALSE.
      */
      if( is_same_idx_reached && prl_sr.pri == PRL_PRI_HIGHER ) {
        is_app_next_sys = FALSE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Append preferred (i.e., non-negative) systems that comply with the
    ** roaming indication.
    */
    if( is_sys_comply )
    {
      /* Set the geo_mask to indicates that this acquisition record is
      ** appended to the geo_lst.
      */
      SDPRL_BIT_SET( parser_lists_ptr->geo_mask, prl_sr.acq_index );

      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );
      sdprl_acq_rec_app( geo_lst, SDSR_MAX, SDSR_MAX,
                         &prl_ar,
                         SD_NO_CHANGE_SYS_IDX,
                         SDPRL_APPEND_ALL );
    }

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Append the rest of the systems that are part of the current GEO to the
  ** specified list.
  */
  while( sdprl_parser_next(&trav_parser, &prl_sr) &&
         prl_sr.geo != PRL_NEW_GEO_REGION )
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    is_sys_comply = TRUE;

    /* If only systems that comply with the current roaming preference are to
    ** be extracted, determine whether the system that the parser is pointing
    ** to complies with the current roaming preference. If only home systems
    ** are to be extracted, determine whether the system that the parser is
    ** pointing to is a home system.
    */
    if( extract == SDPRL_EXTRACT_ROAM_COMPLY ||
        extract == SDPRL_EXTRACT_HOME )
    {
      /* Set the roaming indicator value based on the PRL and home SID/NID
      ** list.
      */
      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );

      sr.sys.mode = prl_map_acq_rec_to_sd_mode( prl_ar.acq_type );
      prl_sys_rec_get_sid( &prl_sr, &sr.sid );
      sr.roam = prl_sr.roam_ind;

      if( sdprl_is_home_sid_nid(&sr, sdprl_ptr->curr_nam) &&
          sdprl_roam_ind_map_custom_home_to_home(sr.roam) !=
                                                  SYS_ROAM_STATUS_OFF &&
          (sr.roam == SYS_ROAM_STATUS_ON || 
           sr.roam == SYS_ROAM_STATUS_BLINK)
        )
      {
        sr.roam = SYS_ROAM_STATUS_OFF;
      }

      /* If the system is negative or does not comply with roaming
      ** preference, indicate so.
      */
      if( prl_sr.neg_pref == PRL_NEGATIVE ||
          sdss_sr_is_pref_conf(parser_ptr->ss, &sr, SDSS_PREF_CONF_ROAM) )
      {
        is_sys_comply = FALSE;
      }

      /* If looking for home systems (SDPRL_EXTRACT_HOME) and the system is
      ** not a home system, then indicate that the system is not compliant.
      */
      if( extract == SDPRL_EXTRACT_HOME &&
          sdprl_roam_ind_map_custom_home_to_home(sr.roam) !=
                                                  SYS_ROAM_STATUS_OFF )
      {
        is_sys_comply = FALSE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If only systems that are same or better preference are to be
    ** extracted, determine whether the system to which the parser is
    ** pointing is same or better preference.
    */
    else if( extract == SDPRL_EXTRACT_SAME_BETTER )
    {
      /* If the system is negative or is_app_next_sys is FALSE, indicate so.
      */
      if( prl_sr.neg_pref == PRL_NEGATIVE ||
          !is_app_next_sys )
      {
        is_sys_comply = FALSE;
      }

      /* If we reached the system that is pointed to by parser_ptr, indicate
      ** so.
      */
      if( trav_parser.sys_idx == parser_ptr->sys_idx ) {
        is_same_idx_reached = TRUE;
      }

      /* If we reached the systems that are of the same priority as the
      ** system that is pointed to by parser_ptr and the current system is
      ** more preferred than then next system (i.e. pri==PRL_PRI_HIGHER), set
      ** is_app_next_sys=FALSE.
      */
      if( is_same_idx_reached && prl_sr.pri == PRL_PRI_HIGHER ) {
        is_app_next_sys = FALSE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Append preferred (i.e., non-negative) systems that comply with the
    ** roaming indication.
    */
    if( is_sys_comply )
    {
      /* No need to append system record if it was already appended previously.
      */
      if( SDPRL_BIT_IS_ON(parser_lists_ptr->geo_mask, prl_sr.acq_index) ||
          !( prl_sr.rec_type == PRL_SYS_REC_IS2000 ||
             prl_sr.rec_type == PRL_SYS_REC_IS856 ||
             prl_sr.rec_type == PRL_SYS_REC_PLMN ) )
      {
        continue;
      }
      /* Set the geo_mask to indicates that this acquisition record is
      ** appended to the geo_lst.
      */
      SDPRL_BIT_SET( parser_lists_ptr->geo_mask, prl_sr.acq_index );

      sdprl_get_acq_rec_else_err_fatal( &prl_ar,&prl_sr );
      sdprl_acq_rec_app( geo_lst, SDSR_MAX, SDSR_MAX,
                         &prl_ar,
                         SD_NO_CHANGE_SYS_IDX,
                         SDPRL_APPEND_ALL );
    }

  } /* while */

} /* sdprl_parser_app_geo */




#if 0
#ifdef FEATURE_IS683C_PRL
/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_app_associated

DESCRIPTION
  Append the systems that are associated with the system table entry that is
  currently pointed to by the parser to the specified system record list.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_parser_app_associated(

        sdprl_parser_s_type       *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

        sdprl_parser_lists_s_type *parser_lists_ptr
            /* Pointer to a parser lists structure.
            */
)
{
  sdprl_parser_s_type   trav_parser;
  prli_sys_rec_type     prl_sr;
  prli_acq_rec_type     prl_ar;
  sdsr_e_type           colloc_lst      = parser_lists_ptr->colloc_lst;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( parser_lists_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_lst, SDSR_1ST_SYS, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is specified or no systems are associated with the current
  ** system pointed to by the parser, return now.
  */
  if( colloc_lst == SDSR_MAX ||
      ! parser_ptr->sys_rec.association_inc )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we reached the end of the system table, return now.
  */
  if( trav_parser.sys_rec_len <= 0 )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Append acquisition records that are not already appended and that have
  ** the same association tag as the system that is currently pointed to by
  ** the parser.
  */
  do
  {
    if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->colloc_mask, prl_sr.acq_index)
                        &&
        prl_sr.association_inc
                        &&
        prl_sr.association_tag == parser_ptr->sys_rec.association_tag
                        &&
        trav_parser.sys_idx != parser_ptr->sys_idx )
    {
      /* Set the colloc_mask to indicates that this acquisition record is
      ** appended to the colloc_lst.
      */
      SDPRL_BIT_SET( parser_lists_ptr->colloc_mask, prl_sr.acq_index );

      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );
      sdprl_acq_rec_app( colloc_lst, SDSR_MAX, SDSR_MAX,
                         &prl_ar,
                         trav_parser.sys_idx,
                         FALSE );
    }

  } while( sdprl_parser_next(&trav_parser, &prl_sr) &&
           prl_sr.geo != PRL_NEW_GEO_REGION );

} /* sdprl_parser_app_associated */
#endif /* FEATURE_IS683C_PRL */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_app_pref

DESCRIPTION
  Extract the systems that are more preferred than the system table entry
  that is currently pointed to by the parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if there are non-wildcard SID systems that are more preferred than the
  system table entry that is currently pointed to by the parser. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_parser_app_pref(

        sdprl_parser_s_type       *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

        sdprl_parser_lists_s_type *parser_lists_ptr,
            /* Pointer to a parser lists structure.
            */

        sd_mode_e_type            sys_mode
            /* Indicate whether the system whose more preferred list is being
            ** extracted is an AMPS system. This is necessary for the case
            ** where the system table entry that is currently pointed to by
            ** the parser is referencing an acquisition record of type
            ** PRL_ACQ_REC_CELLULAR_GENERIC. The CDMA channels that are
            ** associated with the cellular generic acquisition record are
            ** considered more preferred than the AMPS systems and therefore
            ** are appended to the end of the more preferred list.
            */
)
{
  sdprl_parser_s_type   trav_parser;
  prli_sys_rec_type     prl_sr;
  prli_acq_rec_type     prl_ar;
  boolean               is_more_pref  = FALSE;
  sdsr_e_type           pref_lst      = parser_lists_ptr->pref_lst;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( parser_lists_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE_OR( pref_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sys_mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there are more preferred PRL systems than the system table entry that
  ** is currently pointed to by the parser, append such systems to the more
  ** preferred list.
  **
  ** More preferred systems are preferred (as apposed to negative) systems
  ** between the beginning of the GEO to the last more preferred system
  ** (inclusive).
  */
  if( parser_ptr->pri_is_higher )
  {
    if( trav_parser.sys_rec_len > 0 &&
        prl_sr.neg_pref == PRL_PREFERRED )
    {
      /* Only need to append system record if it was not appended previously.
      */
      if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->pref_mask, prl_sr.acq_index) )
      {
        /* Set the pref_mask to indicates that this acquisition record is
        ** appended to the pref_lst.
        */
        SDPRL_BIT_SET( parser_lists_ptr->pref_mask, prl_sr.acq_index );

        sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );
        sdprl_acq_rec_app( pref_lst, SDSR_MAX, SDSR_MAX,
                           &prl_ar,
                           SD_NO_CHANGE_SYS_IDX,
                           TRUE );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If this is a non-wildcard SID entry, indicate that there are more
      ** preferred non-wildcard SID systems than the system table entry
      ** that is currently pointed to by the parser.
      */
      #ifdef FEATURE_IS683C_PRL
      if( ! prl_sys_is_wildcard(&prl_sr) )
      #else
      if( prl_sr.sid != SD_WILDCARD_SID )
      #endif /* ! FEATURE_IS683C_PRL */
      {
        is_more_pref = TRUE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    while( sdprl_parser_next(&trav_parser, &prl_sr) &&
           trav_parser.sys_idx <= parser_ptr->pri_idx )
    {
      if( prl_sr.neg_pref == PRL_PREFERRED )
      {
        /* Only need to append system record if it was not appended
        ** previously.
        */
        if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->pref_mask, prl_sr.acq_index))
        {
          /* Set the pref_mask to indicates that this acquisition record is
          ** appended to the pref_lst.
          */
          SDPRL_BIT_SET( parser_lists_ptr->pref_mask, prl_sr.acq_index );

          sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );
          sdprl_acq_rec_app( pref_lst, SDSR_MAX, SDSR_MAX,
                             &prl_ar,
                             SD_NO_CHANGE_SYS_IDX,
                             TRUE );
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If this is a non-wildcard SID entry, indicate that there are more
        ** preferred non-wildcard SID systems than the system table entry
        ** that is currently pointed to by the parser.
        */
        #ifdef FEATURE_IS683C_PRL
        if( ! prl_sys_is_wildcard(&prl_sr) )
        #else
        if( prl_sr.sid != SD_WILDCARD_SID )
        #endif /* ! FEATURE_IS683C_PRL */
        {
          is_more_pref = TRUE;
        }
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system table entry that is currently pointed to by the parser is
  ** referencing an acquisition record of type PRL_ACQ_REC_CELLULAR_GENERIC
  ** and the system in question is an analog system, append the CDMA channels
  ** that are associated with this record to the more preferred
  ** list.
  */
  if( sdprl_parser_get(parser_ptr, &prl_sr) > 0 )
  {
    sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );

    if( prl_ar.acq_type == PRL_ACQ_REC_CELLULAR_GENERIC &&
        sys_mode == SD_MODE_AMPS )
    {
      sdprl_acq_rec_app( pref_lst, SDSR_MAX, SDSR_MAX,
                         &prl_ar,
                         SD_NO_CHANGE_SYS_IDX,
                         FALSE );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate whether there are more preferred non-wildcard SID systems than
  ** the system table entry that is currently pointed to by the parser.
  */
  return is_more_pref;

} /* sdprl_parser_app_pref */
#endif /* if 0 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_app_geo_pref_assn

DESCRIPTION
  Given a parser that is pointing at a system table entry, append the
  following information to the specified lists:
  1. Append the systems that are part of the GEO to geo_lst.
  2. Append the GEO systems that are more preferred to pref_lst.
  3. Append the GEO systems that are associated to colloc_lst.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

  The parser lists item that is pointed to by parser_lists_ptr parameter must
  have already been associated with a PRL via a call to
  sdprl_parser_lists_init().

RETURN VALUE
  TRUE if there are non-wildcard SID systems that are more preferred than the
  system table entry that is currently pointed to by the parser. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                     sdprl_parser_app_geo_pref_assn(

        const sdprl_parser_s_type   *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

        sdprl_parser_lists_s_type   *parser_lists_ptr,
            /* Pointer to a parser lists structure.
            */

        sd_mode_e_type              sys_mode
            /* Indicate whether the system whose more preferred list is being
            ** extracted is an AMPS system. This is necessary for the case
            ** where the system table entry that is currently pointed to by
            ** the parser is referencing an acquisition record of type
            ** PRL_ACQ_REC_CELLULAR_GENERIC. The CDMA channels that are
            ** associated with the cellular generic acquisition record are
            ** considered more preferred than the AMPS systems and therefore
            ** are appended to the end of the more preferred list.
            */
)
{
  sdprl_parser_s_type   trav_parser;
  prli_sys_rec_type     prl_sr;
  prli_acq_rec_type     prl_ar;
  boolean               is_more_pref  = FALSE;
  sdsr_e_type           geo_lst;
  sdsr_e_type           pref_lst;
  sdsr_e_type           colloc_lst;
  word                  sys_idx;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( parser_lists_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sys_mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Append the first system in the current GEO to the specified lists as
  ** necessary.
  ** skip over unknow system record.
  */
  if( (trav_parser.sys_rec_len > 0)  &&
      ( prl_sr.rec_type == PRL_SYS_REC_IS2000 ||
        prl_sr.rec_type == PRL_SYS_REC_IS856  ||
        prl_sr.rec_type == PRL_SYS_REC_PLMN))
  {
    /* Initially assume no need to append the acquisition record to the
    ** lists.
    */
    geo_lst     = SDSR_MAX;
    pref_lst    = SDSR_MAX;
    colloc_lst  = SDSR_MAX;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to geo_lst.
    */
    if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->geo_mask, prl_sr.acq_index) )
    {
      geo_lst = parser_lists_ptr->geo_lst;
      SDPRL_BIT_SET( parser_lists_ptr->geo_mask, prl_sr.acq_index );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to pref_lst.
    */
    if( parser_ptr->pri_is_higher &&
        prl_sr.neg_pref == PRL_PREFERRED )
    {
      if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->pref_mask, prl_sr.acq_index) )
      {
        pref_lst = parser_lists_ptr->pref_lst;
        SDPRL_BIT_SET( parser_lists_ptr->pref_mask, prl_sr.acq_index );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If this is a non-wildcard SID entry, indicate that there are more
      ** preferred non-wildcard SID systems than the system table entry
      ** that is currently pointed to by the parser.
      */
      if( prl_sys_wildcard(&prl_sr) != PRL_WCARD_SID )
      {
        is_more_pref = TRUE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to colloc_lst.
    **
    ** Append systems that have the same association tag as the system that
    ** is pointed to by the parser.
    **
    ** Specifically append to assn_lst if:
    ** 1. Association_inc flag of the system pointed to by the parser is set.
    ** 2. An assn_lst is specified.
    ** 3. Association_inc flag of the traversed system is set.
    ** 4. The association tags are the same.
    */
    if( parser_ptr->sys_rec.association_inc
                        &&
        parser_lists_ptr->colloc_lst != SDSR_MAX
                        &&
        prl_sr.association_inc
                        &&
        prl_sr.association_tag == parser_ptr->sys_rec.association_tag
      )
    {
      if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->colloc_mask, prl_sr.acq_index))
      {
        colloc_lst = parser_lists_ptr->colloc_lst;
        SDPRL_BIT_SET( parser_lists_ptr->colloc_mask, prl_sr.acq_index );
      }

      sys_idx     = trav_parser.sys_idx;
    }
    else
    {
      sys_idx     = SD_NO_CHANGE_SYS_IDX;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If need to append to one or more of the lists, do so.
    */
    if( geo_lst     != SDSR_MAX ||
        pref_lst    != SDSR_MAX ||
        colloc_lst  != SDSR_MAX )
    {
      /* Extract acquisition record.
      */
      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Append to lists.
      */
      sdprl_acq_rec_app( geo_lst,
                         pref_lst,
                         colloc_lst,
                         &prl_ar,
                         sys_idx,
                         SDPRL_APPEND_ALL );
    }

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Append the rest of the systems that are part of the current GEO to the
  ** specified lists as necessary.
  */
  while( sdprl_parser_next(&trav_parser, &prl_sr) &&
         prl_sr.geo != PRL_NEW_GEO_REGION )
  {
    /* Skip unknown system records.
    */
    if ( prl_sr.rec_type != PRL_SYS_REC_IS2000 &&
         prl_sr.rec_type != PRL_SYS_REC_IS856  &&
         prl_sr.rec_type != PRL_SYS_REC_PLMN )
    {
      continue;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initially assume no need to append the acquisition record to the
    ** lists.
    */
    geo_lst     = SDSR_MAX;
    pref_lst    = SDSR_MAX;
    colloc_lst  = SDSR_MAX;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to geo_lst.
    */
    if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->geo_mask, prl_sr.acq_index) ||
        prl_sr.rec_type == PRL_SYS_REC_PLMN )
    {
      geo_lst = parser_lists_ptr->geo_lst;
      SDPRL_BIT_SET( parser_lists_ptr->geo_mask, prl_sr.acq_index );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to pref_lst.
    */
    if( parser_ptr->pri_is_higher &&
        trav_parser.sys_idx <= parser_ptr->pri_idx &&
        prl_sr.neg_pref == PRL_PREFERRED )
    {
      if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->pref_mask, prl_sr.acq_index) ||
          prl_sr.rec_type == PRL_SYS_REC_PLMN)
      {
        pref_lst = parser_lists_ptr->pref_lst;
        SDPRL_BIT_SET( parser_lists_ptr->pref_mask, prl_sr.acq_index );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If this is a non-wildcard SID entry, indicate that there are more
      ** preferred non-wildcard SID systems than the system table entry
      ** that is currently pointed to by the parser.
      */
      if( prl_sys_wildcard(&prl_sr) != PRL_WCARD_SID )
      {
        is_more_pref = TRUE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to colloc_lst.
    **
    ** Append systems that have the same association tag as the system that
    ** is pointed to by the parser.
    **
    ** Specifically append to assn_lst if:
    ** 1. Association_inc flag of the system pointed to by the parser is set.
    ** 2. An assn_lst is specified.
    ** 3. Association_inc flag of the traversed system is set.
    ** 4. The association tags are the same.
    */
    if( parser_ptr->sys_rec.association_inc
                        &&
        parser_lists_ptr->colloc_lst != SDSR_MAX
                        &&
        prl_sr.association_inc
                        &&
        prl_sr.association_tag == parser_ptr->sys_rec.association_tag
      )
    {
      if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->colloc_mask, prl_sr.acq_index))
      {
        colloc_lst = parser_lists_ptr->colloc_lst;
        SDPRL_BIT_SET( parser_lists_ptr->colloc_mask, prl_sr.acq_index );
      }

      sys_idx     = trav_parser.sys_idx;
    }

    else
    {
      sys_idx     = SD_NO_CHANGE_SYS_IDX;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If need to append to one or more of the lists, do so.
    */
    if( geo_lst     != SDSR_MAX ||
        pref_lst    != SDSR_MAX ||
        colloc_lst  != SDSR_MAX )
    {
      /* Extract acquisition record.
      */
      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Append to lists.
      */
      sdprl_acq_rec_app( geo_lst,
                         pref_lst,
                         colloc_lst,
                         &prl_ar,
                         sys_idx,
                         SDPRL_APPEND_ALL );
    }

  } /* while */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system table entry that is currently pointed to by the parser is
  ** referencing an acquisition record of type PRL_ACQ_REC_CELLULAR_GENERIC
  ** and the system in question is an analog system, append the CDMA channels
  ** that are associated with this record to the more preferred
  ** list.
  **
  ** Same as PRL_ACQ_REC_GW_PREFERRED with GSM/WCDMA indicator and the system
  ** in question is an WCDMA/GSM system, append the GSM/WCDMA acq list.
  */
  if( sdprl_parser_get(parser_ptr, &prl_sr) > 0 )
  {
    sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );

    if( prl_ar.acq_type == PRL_ACQ_REC_CELLULAR_GENERIC &&
        sys_mode == SD_MODE_AMPS )
    {
      sdprl_acq_rec_app( parser_lists_ptr->pref_lst, SDSR_MAX, SDSR_MAX,
                         &prl_ar,
                         SD_NO_CHANGE_SYS_IDX,
                         SDPRL_APPEND_PREF_ONLY );
    }

    //if( prl_ar.acq_type == PRL_ACQ_REC_GW_PREFERRED &&
    //    ( (sys_mode == SD_MODE_WCDMA &&
    //       prl_ar.type.gw_pref.pref_sys_ind == PRL_GW_PREF_SYS_IND_GSM) ||
    //      (sys_mode == SD_MODE_GSM &&
    //       prl_ar.type.gw_pref.pref_sys_ind == PRL_GW_PREF_SYS_IND_WCDMA) )
    //    )
    //{
    //  sdprl_acq_rec_app( pref_lst, SDSR_MAX, SDSR_MAX,
    //                     &prl_ar,
    //                     SD_NO_CHANGE_SYS_IDX, SDPRL_APPEND_PREF_ONLY );
    //}
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate whether there are more preferred non-wildcard SID systems than
  ** the system table entry that is currently pointed to by the parser.
  */
  return is_more_pref;

} /* sdprl_parser_app_geo_pref_assn */


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#ifdef FEATURE_HYBR_GW
#error code not present
#endif /* FEATURE_HYBR_GW */


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_sys_is_in_geo

DESCRIPTION
  Check whether a specified system is in the GEO of the system table entry
  that is currently pointed to by the parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if the specified system record is in the GEO of the system table entry
  that is currently pointed to by the parser. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_parser_sys_is_in_geo(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL over
            ** which to check for a GEO match.
            */

        const sdsr_s_type         *sr_ptr
            /* The system record to be matched.
            */
)
{
  sdprl_parser_s_type   trav_parser;
  prli_sys_rec_type     prl_sr;
  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If any of the systems that are part of the current GEO matches the
  ** system record that is pointed to by sr_ptr, return TRUE.
  */
  if( trav_parser.sys_rec_len > 0 )
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl )
    {
      return TRUE;
    }
  }

  while( sdprl_parser_next(&trav_parser, &prl_sr) &&
         prl_sr.geo != PRL_NEW_GEO_REGION )
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here none of the systems that are part of the current GEO
  ** matches the system record that is pointed to by sr_ptr - return FALSE.
  */
  return FALSE;

} /* sdprl_parser_sys_is_in_geo */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_sys_is_more_pref

DESCRIPTION
  Check whether a specified system is one of the systems that are more
  preferred than the system table entry that is currently pointed to by the
  parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if the specified system record is one of the systems that are more
  preferred than the system table entry that is currently pointed to by the
  parser. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_parser_sys_is_more_pref(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL over
            ** which to check for a more preferred match.
            */

        const sdsr_s_type         *sr_ptr,
            /* The system record to be matched.
            */

        sd_mode_e_type            sys_mode
            /* Indicate whether the system whose more preferred list is being
            ** checked is an AMPS system. This is necessary for the case
            ** where the system table entry that is currently pointed to by
            ** the parser is referencing an acquisition record of type
            ** PRL_ACQ_REC_CELLULAR_GENERIC. The CDMA channels that are
            ** associated with the cellular generic acquisition record are
            ** considered more preferred than the AMPS systems.
            */
)
{
  sdprl_parser_s_type         trav_parser;
  prli_sys_rec_type           prl_sr;
  prli_acq_rec_type           prl_ar;
  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sys_mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Because the CDMA channels that are associated with the cellular generic
  ** acquisition record are considered more preferred than the AMPS systems,
  ** do the following check.
  */

  /* If the following conditions applies return TRUE:
  **
  ** 1. The system that is being matched is a CDMA system that matches the
  **    system table entry that is currently pointed to by the parser
  **
  ** 2. The system table entry that is currently pointed to by the parser is
  **    an AMPS system that references an acquisition record of type
  **    PRL_ACQ_REC_CELLULAR_GENERIC.
  */
  if( sdprl_parser_get(parser_ptr, &prl_sr) > 0 )
  {
    if( sr_ptr->sys.mode == SD_MODE_CDMA_ &&
        sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl )
    {
      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );

      if( sys_mode == SD_MODE_AMPS &&
          prl_ar.acq_type == PRL_ACQ_REC_CELLULAR_GENERIC )
      {
        return TRUE;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system record that is pointed to by sr_ptr matches any of the
  ** systems that are more preferred than the system table entry that is
  ** currently pointed to by the parser, return TRUE.
  **
  ** More preferred systems are systems between the beginning of the GEO
  ** to the last more preferred system (inclusive).
  */
  if( parser_ptr->pri_is_higher )
  {
    if( trav_parser.sys_rec_len > 0 )
    {
      if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
          (byte)match_lvl >= sr_ptr->match_lvl )
      {
        return TRUE;
      }
    }

    while( sdprl_parser_next(&trav_parser, &prl_sr) &&
           trav_parser.sys_idx <= parser_ptr->pri_idx )
    {
      if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
          (byte)match_lvl >= sr_ptr->match_lvl )
      {
        return TRUE;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here the specified system is not one of the systems that are
  ** more preferred than the system table entry that is currently pointed to
  ** by the parser - return FALSE.
  */
  return FALSE;

} /* sdprl_parser_sys_is_more_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_sys_is_associated

DESCRIPTION
  Check whether a specified system is associated (collocated) with the system
  table entry that is currently pointed to by the parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if the specified system record is associated with the system table
  entry that is currently pointed to by the parser parser. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_parser_sys_is_associated(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL over
            ** which to check for a more preferred match.
            */

        const sdsr_s_type         *sr_ptr
            /* The system record to be checked for association (collocation).
            */
)
{
  sdprl_parser_s_type         trav_parser;
  prli_sys_rec_type           prl_sr;
  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system table entry that is currently pointed to by the parser has
  ** no association field (i.e. not associated with any other system record),
  ** return FALSE.
  */
  if( ! parser_ptr->sys_rec.association_inc )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Go through the current GEO. If any of the systems that are part of the
  ** current GEO matches the system record that is pointed to by sr_ptr and
  ** is associated with the system record that is pointed to by the parser,
  ** return TRUE.
  */

  /* Check first system in the GEO.
  */
  if( trav_parser.sys_rec_len > 0 )
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl &&
        prl_sr.association_inc &&
        prl_sr.association_tag == parser_ptr->sys_rec.association_tag &&
        prl_sr.neg_pref != PRL_NEGATIVE )
    {
      return TRUE;
    }
  }

  /* Check all other systems in the GEO.
  */
  while( sdprl_parser_next(&trav_parser, &prl_sr) &&
         prl_sr.geo != PRL_NEW_GEO_REGION )
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl &&
        prl_sr.association_inc &&
        prl_sr.association_tag == parser_ptr->sys_rec.association_tag &&
        prl_sr.neg_pref != PRL_NEGATIVE )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, none of the systems that are part of the current GEO is
  ** associated with the system record that is pointed to by sr_ptr - return
  ** FALSE.
  */
  return FALSE;

} /* sdprl_parser_sys_is_associated */




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= MAJOR ROUTINES ================================
=============================================================================
=============================================================================
===========================================================================*/


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
  TRUE if system record matches its expected PRL's system table index. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdprl_is_exp_sys_idx(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record to be checked for a match.
            */
)
{
  sdprl_s_type          *sdprl_ptr = &sdprl;

  prli_sys_rec_type     prl_sr;
  sdprl_parser_s_type   prl_parser;
  dword                 num_bits   = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If expected system table index is wildcard, return TRUE.
  */
  if( sr_ptr->idx_expected == SD_WILDCARD_SYS_IDX )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system record is not a listed PRL system, return FALSE.
  */
  if( ! sdss_sr_is_prl(sr_ptr, SDSS_PRL_VAL_LISTED) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get( &prl_parser, &prl_sr );
  SD_ASSERT( num_bits > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If needed, advance the parser to point at the expected system record.
  */
  if( sr_ptr->idx_expected > 0 )
  {
    while( sdprl_parser_next(&prl_parser, &prl_sr) &&
           prl_parser.sys_idx != sr_ptr->idx_expected )
    {}
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we found the index and the SID match, return TRUE. Else, return
  ** FALSE.
  */
  if( prl_parser.sys_idx == sr_ptr->idx_expected &&
      sdprl_sys_is_match(sr_ptr, &prl_sr, NULL) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdprl_is_exp_sys_idx */




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
  TRUE if the two system records are associated. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdprl_is_associated(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to system record 2.
            */
)
{
  sdprl_s_type                *sdprl_ptr    = &sdprl;

  prli_sys_rec_type           prl_sr;
  sdprl_parser_s_type         prl_parser;
  dword                       num_bits      = 0;
  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr1_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If system 1 or system 2 is not listed in the PRL, they can not be
  ** associated - return FALSE now.
  */
  if( !sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_LISTED) ||
      !sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_LISTED) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ADD_HDR_IF_IS683A_PRL
#error code not present
  #endif /* FEATURE_ADD_HDR_IF_IS683A_PRL */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system is from GW list and the system1 is an HDR system, return
  ** TRUE. We do this because GW list is not populated from PRL and does not
  ** have associated information.
  */
  if( ( sr1_ptr->sys.mode == SD_MODE_HDR &&
        sdsr_list_find( SDSR_GW_LST, sr2_ptr) != SDSR_POS_NULL )
                           ||
      ( sr2_ptr->sys.mode == SD_MODE_HDR &&
        sdsr_list_find( SDSR_GW_LST, sr1_ptr) != SDSR_POS_NULL ))

  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get( &prl_parser, &prl_sr );
  SD_ASSERT( num_bits > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Look for a GEO that associated both systems. If such GEO and association
  ** is found, return TRUE.
  */
  do
  {
    if( sdprl_sys_is_match(sr2_ptr, &prl_sr, &match_lvl) &&
        (byte) match_lvl >= sr2_ptr->match_lvl &&
        sdprl_parser_sys_is_associated(&prl_parser, sr1_ptr) )
    {
      return TRUE;
    }

  } while( sdprl_parser_next_match(&prl_parser, sr2_ptr, &prl_sr, NULL) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no association was found, return FALSE.
  */
  return FALSE;

} /* sdprl_is_associated */


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
  association_tag if system record is found. SDSS_ASS_TAG_NONE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                   sdprl_get_addn_sys_record_info(

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
)
{
  sdprl_s_type                *sdprl_ptr    = &sdprl;

  prli_sys_rec_type           prl_sr;
  sdprl_parser_s_type         prl_parser;
  dword                       num_bits      = 0;
  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( assn_tag != NULL );
  SD_ASSERT( geo_offset != NULL );
  SD_ASSERT( multi_geo_count != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the Variables.
  */
  *assn_tag = SDSS_ASSN_TAG_NONE;
  *geo_offset = SDSS_GEO_OFFSET_NONE;
  *multi_geo_count = SDSS_GEO_COUNT_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If system is not listed in the PRL, they can not be
  ** associated - return FALSE now.
  */
  if( !sdss_sr_is_prl(sr_ptr, SDSS_PRL_VAL_LISTED))
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get( &prl_parser, &prl_sr );
  SD_ASSERT( num_bits > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the system record in the prl
  ** fs found, return the corresponding association tag.
  */
  do
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte) match_lvl >= sr_ptr->match_lvl )
    {
      *assn_tag = prl_sr.association_tag;
      *geo_offset = (int) prl_parser.geo_offset;
      (*multi_geo_count)++;
      if((byte) match_lvl > sr_ptr->match_lvl)
      {
        *multi_geo_count = 0;
      }
    }

  } while( sdprl_parser_next_match(&prl_parser, sr_ptr, &prl_sr, NULL) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdprl_get_addn_sys_record_info */


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
MMEXTN  boolean                   sdprl_is_more_pref_upto(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to system record 2.
            */

        sdprl_comp_depth_e_type   depth
            /* Depth of the comparison.
            */
)
{
  sdprl_s_type                *sdprl_ptr = &sdprl;

  prli_sys_rec_type           prl_sr;
  sdprl_parser_s_type         prl_parser;
  dword                       num_bits   = 0;
  int                         comp;
  int                         sr1_pos, sr2_pos;

  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr1_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If system 1 and 2 are preferred PRL systems, look for the first GEO that
  ** lists both systems. If such a GEO is found and within this GEO system
  ** 1 is more preferred than system 2, return TRUE. If within this GEO
  ** system 1 is not more preferred than system 2, return FALSE.
  */
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_ANY_PREF) &&
      sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_ANY_PREF) )
  {
    /* Initialize a parser with the currently loaded PRL.
    */
    sdprl_parser_init( &prl_parser,
                       SD_SS_MAIN,
                       (byte*) sdprl_ptr->prl_ptr,
                       sdprl_ptr->prl_hdr_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Extract the first system record entry.
    */
    num_bits = sdprl_parser_get( &prl_parser, &prl_sr );
    SD_ASSERT( num_bits > 0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Look for a GEO that contains both systems, 1 and 2.
    */
    do
    {
      if( sdprl_sys_is_match(sr2_ptr, &prl_sr, &match_lvl) &&
          (byte) match_lvl >= sr2_ptr->match_lvl &&
          sdprl_parser_sys_is_in_geo(&prl_parser, sr1_ptr) )
      {
        /* We found a GEO that contains both systems. If system 1 is more
        ** preferred return TRUE. Else, return FALSE.
        */
        if( sdprl_parser_sys_is_more_pref(&prl_parser,
                                          sr1_ptr,
                                          sr2_ptr->sys.mode) )
        {
          return TRUE;
        }

        return FALSE;
      }

    } while( sdprl_parser_next_match(&prl_parser, sr2_ptr, &prl_sr, NULL) );

  } /* if */

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_GEO_POS)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  */
  sr1_pos = sdsr_list_find( SDSR_TBL_LST, sr1_ptr );
  sr2_pos = sdsr_list_find( SDSR_TBL_LST, sr2_ptr );

  /* Update sr1 designation.
  */
  (void) sdprl_get_info(
                  ss,                  /* MAIN system selection */
                  SDSR_TBL_LST,        /* Get PRL info for this list */
                  sr1_pos,             /* and list position */
                  SDSR_TEMP2_LST,      /* Put GEO systems into this list */
                  sdss_sr_list_map2(ss, SDSR_TEMP_LST),
                                       /* Put PREF systems into this list */
                  SDSR_TEMP2_LST,      /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info if diff SID or PRL match*/

  /* Update sr2 designation.
  */
  (void) sdprl_get_info(
                  ss,                  /* MAIN system selection */
                  SDSR_TBL_LST,        /* Get PRL info for this list */
                  sr2_pos,             /* and list position */
                  SDSR_TEMP2_LST,      /* Put GEO systems into this list */
                  sdss_sr_list_map2(ss, SDSR_TEMP_LST),
                                       /* Put PREF systems into this list */
                  SDSR_TEMP2_LST,      /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info if diff SID or PRL match*/


  /* If system 1 has higher PRL designation order than system 2, return TRUE.
  ** Else if system 2 has a higher PRL designation order than system 1,
  ** return FALSE.
  */
  if( sr1_ptr->prl > sr2_ptr->prl &&
      sr2_ptr->prl != (byte) SDSS_PRL_PREF )
  {
    return TRUE;
  }
  if( sr2_ptr->prl > sr1_ptr->prl &&
      sr1_ptr->prl != (byte) SDSS_PRL_PREF )
  {
    return FALSE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_PRL_DESIG)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  */

  /* If system 1 has a roaming indicator that is more favorable than the
  ** roaming indicator of system 2, return TRUE. Else, if system 1 has a
  ** roaming indicator that is less favorable than the roaming indicator of
  ** system 2, return FALSE.
  */
  comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_ROAM );

  if( comp > 0 ) {
    return TRUE;
  }

  if( comp < 0 ) {
    return FALSE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_ROAM)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  ** 3. The roaming indicators of system 1 and 2 are indifference.
  */

  /* If system 1 is most preferred and system 2 is not, return TRUE. Else
  ** return FALSE.
  */
  if( sr1_ptr->prl == (byte) SDSS_PRL_MOST_PREF &&
      sr2_ptr->prl != (byte) SDSS_PRL_MOST_PREF )
  {
    return TRUE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_ONLY_ONE_MOST_PREF)
  {
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF

  /* If both system 1 and 2 are available systems and only system 1 is of
  ** cellular home side, return TRUE. If only system 2 if of cellular home
  ** side, return FALSE.
  */
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_AVAIL) &&
      sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_CELL_HOME );

    if( comp > 0 ) {
      return TRUE;
    }

    if( comp < 0 ) {
      return FALSE;
    }
  }

  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  /* IMPORTANT NOTE: All depth checks should be outside any #ifdef.
  */
  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_CELL_HOME)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  ** 3. The roaming indicators of system 1 and 2 are indifference.
  ** 4. The PRL designation of system 1 is NOT MOST_PREF or, system 2 is
  **    MOST_PREF.
  */

  /* If both system 1 and 2 are available systems and system 1 has a PRL
  ** acquisition table position that is lower than system 2, return TRUE. If
  ** system 2 has a PRL acquisition table position that is lower than system
  ** 1, return FALSE.
  */
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_AVAIL) &&
      sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_PRL_ACQ );

    if( comp > 0 ) {
      return TRUE;
    }

    if( comp < 0 ) {
      return FALSE;
    }
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_ACQ_TBL_ORDER)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  ** 3. The roaming indicators of system 1 and 2 are indifference.
  ** 4. The PRL designation of system 1 is NOT MOST_PREF or, system 2 is
  **    MOST_PREF.
  ** 5. The PRL acquisition table orders of system a and 2 are indifference.
  */

  /* If system 1 is digital and system 2 is not, return TRUE. Else, if system
  ** 2 is digital and system 1 is not, return FALSE.
  */
  comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_DIGITAL );

  if( comp > 0 ) {
    return TRUE;
  }

  if( comp < 0 ) {
    return FALSE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_DIGITAL)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the systems are indifference - return FALSE.
  */
  return FALSE;

} /* sdprl_is_more_pref_upto */


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

RETURN VALUE
  TRUE if system record 1 is more preferred than system record 2. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdprl_is_more_pref(

        sd_ss_e_type                     ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type               *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type               *sr2_ptr
            /* Pointer to system record 2.
            */
)
{
  return sdprl_is_more_pref_upto(ss, sr1_ptr, sr2_ptr, SDPRL_COMP_UPTO_LAST);
} /* sdprl_is_more_pref */


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_autonam_proc

DESCRIPTION
  Does auto-NAM change related processing.

  Specifically, check whether a specified system record has a SID/NID that is
  appropriate for auto-NAM change and if so notify the registered client by
  calling on call on sd_si_autonam_change.

  Auto-NAM change is appropriate whenever the following conditions are
  met:

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
MMEXTN  void                      sdprl_autonam_proc(

        const sdsr_s_type         *sr_ptr
            /* Pointer to system record to be checked against the home
            ** SID/NID list.
            */
)
{
  sdprl_s_type              *sdprl_ptr = &sdprl;
  unsigned int              i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this target does not support more than one NAM selections, return
  ** now.
  */ /*lint -e506 -e774 */ /* Constant value Boolean, always false */
  if( ARR_SIZE(sdprl_ptr->nam_info) <= 1 ) /*lint +e506 +e774 */
  {
    return;
  }
  else
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the SID/NID of the system record that is pointed to by sr_ptr
    ** matches one of the home SID/NIDs of the current NAM, return now.
    */
    if( sdprl_is_home_sid_nid(sr_ptr, sdprl_ptr->curr_nam) )
    {
      return;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*  Check whether the SID/NID of the system record that is pointed to by
    ** sr_ptr matches one of the home SID/NIDs of other than the current NAM.
    ** If so, notify the registered client that auto-NAM change is appropriate.
    */
    for( i=0; i < ARR_SIZE(sdprl_ptr->nam_info); i++ )
    {

      if( ((sd_nam_e_type) i) !=  sdprl_ptr->curr_nam &&
          sdprl_is_home_sid_nid(sr_ptr, (sd_nam_e_type) i) )
      {
        sd_si_autonam_change( (sd_nam_e_type) i );
        return;
      }
    }

  }


} /* sdprl_autonam_proc */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info_base

DESCRIPTION
  Given a system record, srv_sys, geo_list and a pref_list, this function
  does the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.
    4. Update the srv_sys wiht PRL designation if required.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdprl_get_info_base(

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
)
{
  sdprl_s_type                *sdprl_ptr          = &sdprl;
  const prl_hdr_s_type        *prl_hdr_ptr        = sdprl_ptr->prl_hdr_ptr;

  sdsr_s_type                 sd_sr;
  sdsr_s_type                 *sd_sr_ptr          = &sd_sr;

  sdsr_ref_type               sd_sr_ref;
  sdsr_s_type                 *list_ptr;

  prli_sys_rec_type           prl_sr;
  sdprl_parser_s_type         prl_parser;
  sdprl_parser_lists_s_type   parser_lists;

  boolean                     is_prl_match              = FALSE;
  boolean                     is_more_pref              = TRUE;
  boolean                     is_non_wildcard_more_pref = FALSE;
  dword                       num_bits                  = 0;
  sdprl_match_sid_nid_e_type  match_lvl = SDPRL_MATCH_SID_NID_NONE;
  sdss_pref_s_type            ss_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( srv_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( geo_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( pref_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( extract, SDPRL_EXTRACT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);
  SYS_ARG_NOT_USED(is_more_pref);
  SYS_ARG_NOT_USED(is_non_wildcard_more_pref);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the Preferences of the current SS
  */
  sdss_get_pref( ss, &ss_pref );

  /* Point at the system for which to get the PRL designation.
  */
  list_ptr = sdsr_list_get_ptr( list, list_pos );
  if(  list_ptr == NULL)
  {
    SD_MSG_LOW("list_ptr NULL",0,0,0);
    return FALSE;
  }


  /* Copy the system record in question into a local buffer.
  */
  *sd_sr_ptr = *list_ptr;

  sd_sr_ref = sdsr_list_get_ref( srv_list, srv_list_pos );
  SD_ASSERT( sd_sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ADD_HDR_IF_IS683A_PRL
#error code not present
  #endif /* FEATURE_ADD_HDR_IF_IS683A_PRL */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find whether the system in question matches any of the PRL system
  ** records.
  */

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     ss,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get(&prl_parser, &prl_sr);
  SD_ASSERT( num_bits > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the first system table entry or any other system table entry matches
  ** the system record in question, indicate so.
  */
  if( sdprl_sys_is_match(sd_sr_ptr, &prl_sr, &match_lvl) ||
      sdprl_parser_next_match(&prl_parser, sd_sr_ptr, &prl_sr, &match_lvl) )
  {
    SD_MSG_HIGH( "PRL match sys_idx=%d, level=%d",
                  prl_parser.sys_idx, match_lvl, 0 );

    is_prl_match = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no PRL match was found and only the information of a system that has
  ** a PRL match should be extracted, return now.
  */
  if( ! is_prl_match &&
      extract == SDPRL_EXTRACT_MATCH_ONLY )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially assume no systems are associated with the system in question.
  */
  SDSR_PRL_ASSN_OFF( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_ALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no PRL match was found for the system in question, decide on the PRL
  ** designation and roaming indicator according to the PRL's PREF_ONLY and
  ** DEF_ROAM_IND fields and the locked SID/NID list.
  */
  if( ! is_prl_match )
  {

    /* Set the PRL match level.
    */
    sd_sr_ptr->match_lvl = (byte) match_lvl;

    /* If the PRL's PREF_ONLY field is set to 1 or the system's SID/NID
    ** matches one of the locked SID/NID or if the prl_pref does not
    ** allow the current band, set the PRL designation to UNAVAIL.
    ** Else, set the PRL designation to AVAIL.
    */
    if(  !(sd_sr_ptr->sid_expected.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS ||
           sd_sr_ptr->sid_expected.net_select_mode == SD_NET_SELECT_MODE_MANUAL_BSSID ||
           sd_sr_ptr->sid_expected.net_select_mode == SD_NET_SELECT_MODE_PRIV_SYS ) &&
         prl_hdr_ptr->pref_only ||
         sdprl_is_lock_sid_nid(sd_sr_ptr, sdprl_ptr->curr_nam) ||
         sdss_sr_is_pref_conf(ss, sd_sr_ptr, SDSS_PREF_CONF_PRL )
      )
    {
      sd_sr_ptr->prl = (byte) SDSS_PRL_UNAVAIL;
    }
    else
    {
      sd_sr_ptr->prl = (byte) SDSS_PRL_AVAIL;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the system's SID/NID matches one of the home SID/NIDs, set the
    ** roaming indicator to OFF. Else, set the roaming indicator to the PRL's
    ** default roaming indication.
    */
    if( sdprl_is_home_sid_nid(sd_sr_ptr, sdprl_ptr->curr_nam) )
    {
      sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
    }
    else
    {
      sd_sr_ptr->roam = prl_hdr_ptr->def_roam_ind;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Set the GEO of a non-PRL system to the empty list and set the more-
    ** preferred list to reference the entire acquisition table.
    */
    sdsr_list_clr( geo_lst );
    sdsr_list_cpy( pref_lst, SDSR_PRL_LST );

    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) 
#error code not present
    #endif

    /* Set the collocated list of a non-PRL system to the empty list.
    */
    sdsr_list_clr( colloc_lst );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Make sure the system in question is not on its more-preferred list.
    */
    (void) sdsr_list_del_ref( pref_lst, sd_sr_ref );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If PRL information should be extracted, set the PRL-designation
    ** and roaming indication of the system record in question per the
    ** extracted PRL-designation and roaming indication.
    */
    if( extract == SDPRL_EXTRACT_ALL )
    {
      list_ptr->match_lvl = sd_sr_ptr->match_lvl;
      list_ptr->prl       = sd_sr_ptr->prl;
      list_ptr->roam      = sd_sr_ptr->roam;

      list_ptr->prl_assn  = sd_sr_ptr->prl_assn;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Indicate that the input system has no PRL match.
    */
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the system in question has a PRL match.
  */

  /* Initialize a parser lists structure with geo_lst, pref_lst and
  ** colloc_lst.
  */
  sdprl_parser_lists_init( &parser_lists,
                           geo_lst,
                           pref_lst,
                           colloc_lst );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If only need to extract specific systems out of the GEO, append such
  ** systems to the geo_lst. Else, append the GEO, more-preferred and
  ** collocated systems to geo_lst, pref_lst and colloc_lst, respectively.
  */
  if( extract == SDPRL_EXTRACT_ROAM_COMPLY ||
      extract == SDPRL_EXTRACT_HOME ||
      extract == SDPRL_EXTRACT_SAME_BETTER )
  {
    sdprl_parser_app_geo( &prl_parser, &parser_lists, extract );
  }
  else
  {
    is_non_wildcard_more_pref =
                      sdprl_parser_app_geo_pref_assn( &prl_parser,
                                                      &parser_lists,
                                                      sd_sr_ptr->sys.mode );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set ON the PN and data association fields as needed.
  */
  if( prl_sr.association_inc )
  {
    if( prl_sr.pn_association ) {
      SDSR_PRL_ASSN_ON( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_PN );
    }

    if( prl_sr.data_association ) {
      SDSR_PRL_ASSN_ON( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_DATA );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the PRL match level.
  */
  sd_sr_ptr->match_lvl = (byte) match_lvl;

  /* Decide on the PRL designation and roaming indicator according to the
  ** matching PRL system record.
  */
  if( prl_sr.neg_pref == PRL_NEGATIVE )
  {
    sd_sr_ptr->prl    = (byte) SDSS_PRL_NEG;
    sd_sr_ptr->roam   = prl_hdr_ptr->def_roam_ind;
  }
  else
  {
    sd_sr_ptr->prl    = (byte) SDSS_PRL_PREF;
    sd_sr_ptr->roam   = prl_sr.roam_ind;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* To take care of a multi-GEO SID (i.e., a SID that is being repeated in
  ** more than one GEO), find all system table entries that match the system
  ** in question and append their GEO and more-preferred lists to the
  ** specified geo_lst and pref_lst lists.
  **
  ** If current match level > previous one, clear GEO/PREF list and append
  ** new records.
  ** If current match level == previous one, append new records at the end.
  ** OW do nothing.
  */
  while( sdprl_parser_next_match(&prl_parser, sd_sr_ptr, &prl_sr,
                                 &match_lvl) )
  {
    if( (byte) match_lvl >= sd_sr_ptr->match_lvl )
    {
      SD_MSG_HIGH( "PRL match sys_idx=%d, level=%d",
                    prl_parser.sys_idx, match_lvl, 0 );

      /* If we found a better match level, start populating the GEO,
      ** PREF and COLLOC from scratch.
      */
      if( (byte) match_lvl > sd_sr_ptr->match_lvl )
      {
        SD_MSG_HIGH( "Better PRL match - ignore previous matches", 0,0,0 );

        /* Initialize a parser lists structure with geo_lst, pref_lst and
        ** colloc_lst.
        */
        sdprl_parser_lists_init( &parser_lists,
                                 geo_lst,
                                 pref_lst,
                                 colloc_lst );

        /* Set the PRL match level.
        */
        sd_sr_ptr->match_lvl = (byte) match_lvl;

        /* Decide on the PRL designation and roaming indicator according to
        ** the matching PRL system record.
        */
        if( prl_sr.neg_pref == PRL_NEGATIVE )
        {
          sd_sr_ptr->prl    = (byte) SDSS_PRL_NEG;
          sd_sr_ptr->roam   = prl_hdr_ptr->def_roam_ind;
        }
        else
        {
          sd_sr_ptr->prl    = (byte) SDSS_PRL_PREF;
          sd_sr_ptr->roam   = prl_sr.roam_ind;
        }

      } /* match_lvl > match_lvl_pre */

      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #endif /* FEATURE_HDR_HYBRID */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If only need to extract specific systems out of the GEO, append such
      ** systems to the geo_lst. Else, append the GEO, more-preferred and
      ** collocated systems to geo_lst, pref_lst and colloc_lst,
      ** respectively.
      */
      if( extract == SDPRL_EXTRACT_ROAM_COMPLY ||
          extract == SDPRL_EXTRACT_HOME ||
          extract == SDPRL_EXTRACT_SAME_BETTER )
      {
        sdprl_parser_app_geo( &prl_parser, &parser_lists, extract );
      }
      else
      {
        is_more_pref = sdprl_parser_app_geo_pref_assn( &prl_parser,
                                                       &parser_lists,
                                                       sd_sr_ptr->sys.mode );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we found a match with a preferred PRL system, do the following:
      */
      if( prl_sr.neg_pref == PRL_PREFERRED )
      {
        /* Set the PRL designation to SDSS_PRL_PREF.
        */
        sd_sr_ptr->prl = (byte) SDSS_PRL_PREF;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        #ifdef FEATURE_WCARD_SID_MGMT

        /* If so far we only found a wildcard SID match.
        */
        if( match_lvl == SDPRL_MATCH_WILDCARD_SID )
        {
          /* If the roaming indication of this match is better than previous
          ** matches and this GEO contains non-wildcard SID systems that are
          ** more preferred than this match, indicate so.
          */
          if( sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) >
              sdprl_roam_ind_map_custom_home_to_home(prl_sr.roam_ind) &&
              is_more_pref )
          {
            is_non_wildcard_more_pref = TRUE;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* If the roaming indication of this match is better or equal than
          ** previous matches and this GEO does not contain non-wildcard SID
          ** systems that are more preferred than this match, indicate so.
          */
          else if( sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) >
                   sdprl_roam_ind_map_custom_home_to_home(prl_sr.roam_ind) &&
                   ! is_more_pref )
          {
            is_non_wildcard_more_pref = FALSE;
          }
        }

        #endif /* FEATURE_WCARD_SID_MGMT */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        #ifdef FEATURE_HDR_HYBRID
#error code not present
        #endif /* FEATURE_HDR_HYBRID */

        /* If the roaming indication of this match is better than previous
        ** matches, update the roaming indication per this match.
        */
        if( sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) >
                    sdprl_roam_ind_map_custom_home_to_home(prl_sr.roam_ind) )
        {
          sd_sr_ptr->roam = prl_sr.roam_ind;
        }

      } /* if( match with preferred system ) */

    } /* if match level is higher or equal to previous one */

  } /* while( more systems ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system in question matches one of the locked SID/NIDs, set the
  ** PRL designation to SDSS_PRL_NEG.
  */
  if( sdprl_is_lock_sid_nid(sd_sr_ptr, sdprl_ptr->curr_nam) )
  {
    sd_sr_ptr->prl = (byte) SDSS_PRL_NEG;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WCARD_SID_MGMT

  /* If we only found a wildcard SID match and no non-wildcard SID system
  ** is more preferred than the wildcard SID match with the best roaming
  ** indication and the PRL contains more than one GEO, do the following:
  **
  ** 1. Set the PRL designation of a negative system to UNAVAIL.
  ** 2. Append the entire acquisition table to the GEO and more preferred
  **    lists.
  */
  if( extract != SDPRL_EXTRACT_ROAM_COMPLY &&
      extract != SDPRL_EXTRACT_HOME &&
      extract != SDPRL_EXTRACT_SAME_BETTER &&
      match_lvl == SDPRL_MATCH_WILDCARD_SID &&
      ! is_non_wildcard_more_pref &&
      prl_parser.geo_idx != 0 )
  {
    if( sd_sr_ptr->prl == (byte) SDSS_PRL_NEG ) {
      sd_sr_ptr->prl = (byte) SDSS_PRL_UNAVAIL;
    }

    sdsr_list_app( geo_lst, SDSR_PRL_LST );
    sdsr_list_app( pref_lst, SDSR_PRL_LST );
  }

  #endif /* FEATURE_WCARD_SID_MGMT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ///* If the system in question is a preferred PRL system and no GEO
  //** contains systems that are more preferred than this system (i.e., the
  //** preferred list is empty), set the PRL designation to SDSS_PRL_MOST_PREF.
  //*/
  //if( sd_sr_ptr->prl == SDSS_PRL_PREF &&
  //    sdsr_list_cnt(pref_lst) == 0 )
  //{
  //  sd_sr_ptr->prl = SDSS_PRL_MOST_PREF;
  //}

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system's SID/NID matches one of the home SID/NIDs overwrite the
  ** roaming indicator to OFF.
  */
  if( sdprl_is_home_sid_nid(sd_sr_ptr, sdprl_ptr->curr_nam) &&
      sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) !=
                                                  SYS_ROAM_STATUS_OFF &&
      (sd_sr_ptr->roam == SYS_ROAM_STATUS_ON || 
       sd_sr_ptr->roam == SYS_ROAM_STATUS_BLINK)
    )
  {
    sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the system in question is not on its more-preferred list.
  */
  (void) sdsr_list_del_ref( pref_lst, sd_sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Delete all GW systems from pref lst if SIM invalid for all selected 
  ** domains
  */  
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) 
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ADD_HDR_IF_IS683A_PRL
#error code not present
  #endif /* FEATURE_ADD_HDR_IF_IS683A_PRL */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the collocated list is not empty, set the tag association of the
  ** system in question to TRUE.
  */
  if( sdsr_list_cnt(colloc_lst) > 0 ) {
    SDSR_PRL_ASSN_ON( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_TAG );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If all PRL information should be extracted, set the PRL-designation
  ** and roaming indication of the system record in question per the
  ** extracted PRL-designation and roaming indication.
  */
  if( extract == SDPRL_EXTRACT_ALL ||
      extract == SDPRL_EXTRACT_MATCH_ONLY )
  {
    list_ptr->match_lvl = sd_sr_ptr->match_lvl;
    list_ptr->prl       = sd_sr_ptr->prl;
    list_ptr->roam      = sd_sr_ptr->roam;

    list_ptr->prl_assn  = sd_sr_ptr->prl_assn;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate that the input system has a PRL match.
  */
  return TRUE;

} /* sdprl_get_info_base */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info

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
MMEXTN  boolean                   sdprl_get_info(

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
)
{
  return( sdprl_get_info2( ss,
                           list,
                           list_pos,
                           list,
                           list_pos,
                           geo_lst,
                           pref_lst,
                           colloc_lst,
                           extract,
                           FALSE)
        );

} /* sdprl_get_info */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info2

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
MMEXTN  boolean                   sdprl_get_info2(

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

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        boolean                   is_update_srv_sys
            /* Indicate if the serving system need to be updated.
            */
)
{
  sdsr_s_type   *sr_ptr, *sys_ptr;
  boolean       return_val;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( srv_list, SDSR_MAX );

  //SD_ASSERT_ENUM_IS_INRANGE_OR( geo_lst,  SDSR_1ST_SYS, SDSR_MAX );
  //SD_ASSERT_ENUM_IS_INRANGE_OR( pref_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( geo_lst, SDSR_1ST_SYS );
  SD_ASSERT_ENUM_IS_INRANGE( pref_lst, SDSR_1ST_SYS );
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_lst, SDSR_1ST_SYS, SDSR_MAX );

  SD_ASSERT_ENUM_IS_INRANGE( extract, SDPRL_EXTRACT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return_val = sdprl_get_info_base( ss,
                                    list,
                                    list_pos,
                                    srv_list,
                                    srv_list_pos,
                                    geo_lst,
                                    pref_lst,
                                    colloc_lst,
                                    extract );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system in question is a preferred PRL system and no GEO contains
  ** systems that are more preferred than this system (i.e., the preferred
  ** has no systems compliant with the SS preference), set the PRL
  ** designation to SDSS_PRL_MOST_PREF.
  */
  sr_ptr = sdsr_list_get_ptr( list, list_pos );
  if (sr_ptr == NULL)
  {
    SD_MSG_LOW("sr_ptr NULL",0,0,0);
    return return_val;
  }

  if( sr_ptr->prl == (byte) SDSS_PRL_PREF &&
      !sdss_sr_list_is_comply( ss, pref_lst ) )
  {
    sr_ptr->prl = (byte) SDSS_PRL_MOST_PREF;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the PRL designation, GEO list and more preferred list of the
  ** serving system if required
  */
  if( is_update_srv_sys )
  {
    sys_ptr  = sdsr_list_get_ptr( srv_list, srv_list_pos );
    if(sys_ptr == NULL )
    {
      SD_MSG_LOW("sys_ptr NULL",0,0,0);
      return return_val;
    }
    sys_ptr->prl = sr_ptr->prl;
    sys_ptr->roam = sr_ptr->roam;
    sys_ptr->prl_assn = sr_ptr->prl_assn;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return return_val;

} /* sdprl_get_info */




#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */




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
MMEXTN  void                      sdprl_init_before_task_start( void )
{
  sdprl_set_prl_active_status ( FALSE);
} /* sdprl_init_before_task_start */




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
MMEXTN  boolean                   sdprl_init( void )
{
  sdprl_s_type      *sdprl_ptr = &sdprl;
  byte              i;

  /*lint -save -e826  Suspicious 'buf' pointer-to-pointer conversion */
  sdnv_data_s_type  buf;
  nv_item_type      *item_ptr = (nv_item_type*) &buf;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For each NAM read from NV:
  **
  ** 1. Clear MRU lst.
  ** 2. MRU table.
  ** 3. Primary/Secondary A/B channels.
  ** 4. Home SID/NID list.
  ** 5. AMPS home SID.
  **
  ** If read operation fails return FALSE:
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear MRU_LST.
  */
  sdsr_list_clr( SDSR_MRU_LST );
  #ifdef FEATURE_HDR
#error code not present
  #endif
  #ifdef FEATURE_WLAN
#error code not present
  #endif

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the WLAN MRU table.
  */
  sdprl_wlan_mru_read();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the custom home bitmap.
  */
  sdprl_custom_home_read();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; i < ARR_SIZE(sdprl_ptr->nam_info); i++ )
  {
    if( ! sdprl_mru_read((sd_nam_e_type) i) )
    {
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read primary A/B channels.
    */
    //#ifdef FEATURE_CDMA_800

    item_ptr->pcdmach.nam = i;
    if( ! sdnv_read(NV_PCDMACH_I, item_ptr) )
    {
      return FALSE;
    }
    sdprl_ptr->nam_info[i].pch_a = item_ptr->pcdmach.channel_a;
    sdprl_ptr->nam_info[i].pch_b = item_ptr->pcdmach.channel_b;

    //#endif /* FEATURE_CDMA_800 */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read secondary A/B channels.
    */
    item_ptr->scdmach.nam = i;
    if( ! sdnv_read(NV_SCDMACH_I, item_ptr) )
    {
      return FALSE;
    }

    sdprl_ptr->nam_info[i].sch_a = item_ptr->scdmach.channel_a;
    sdprl_ptr->nam_info[i].sch_b = item_ptr->scdmach.channel_b;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_PRL_EXTENDED

    /* Read JTACS cmda primary A/B channels.
    */
    item_ptr->pcdmach.nam = i;
    if( ! sdnv_read(NV_JTACS_PCDMACH_I, item_ptr) )
    {
      return FALSE;
    }
    sdprl_ptr->nam_info[i].jtacs_pch_a = item_ptr->jtacs_pcdmach.channel_a;
    sdprl_ptr->nam_info[i].jtacs_pch_b = item_ptr->jtacs_pcdmach.channel_b;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read JTACS cdma secondary A/B channels.
    */
    item_ptr->scdmach.nam = i;
    if( ! sdnv_read(NV_JTACS_SCDMACH_I, item_ptr) )
    {
      return FALSE;
    }

    sdprl_ptr->nam_info[i].jtacs_sch_a = item_ptr->jtacs_scdmach.channel_a;
    sdprl_ptr->nam_info[i].jtacs_sch_b = item_ptr->jtacs_scdmach.channel_b;

    #endif /* FEATURE_PRL_EXTENDED */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read the home SID/NID list.
    */
    if( ! sdprl_home_sid_nid_read((sd_nam_e_type) i) )
    {
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read the locked SID/NID list.
    */
    if( ! sdprl_lock_sid_nid_read((sd_nam_e_type) i) )
    {
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read AMPS backstop enabled.
    */
    item_ptr->amps_backstop_enabled.nam = i;
    if( ! sdnv_read(NV_AMPS_BACKSTOP_ENABLED_I, item_ptr) )
    {
      return FALSE;
    }

    sdprl_ptr->nam_info[i].is_amps_backstop =
                                     item_ptr->amps_backstop_enabled.enabled;

  } /* for */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the silent redial with AMPS indication.
  */
  sdprl_ptr->is_srda = FALSE;

  if( sdnv_read(NV_SRDA_ENABLED_I, item_ptr) ) {
    sdprl_ptr->is_srda = item_ptr->srda_enabled;
    sdss_srda_enabled_update( item_ptr->srda_enabled );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, SD items were successfully read from NV - return TRUE.
  */
  return TRUE;

} /* sdprl_init */
  /*lint -restore */




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

==========================================================================*/
MMEXTN  void                      sdprl_save( void )
{
  sdprl_s_type      *sdprl_ptr = &sdprl;
  byte              i;
  sdsr_e_type       hdr_mru_lst =  SDSR_MAX_LST;
  sdsr_e_type       user_mru_lst = SDSR_MAX_LST;
  sdsr_e_type       wlan_mru_lst = SDSR_MAX_LST;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the systems that are referenced by the SDSR_MRU_LST into the local
  ** MRU RAM buffer that corresponds to the existing NAM selection.
  */
  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  hdr_mru_lst = SDSR_HYBR_1_MRU_LST;
  #elif defined FEATURE_HDR
#error code not present
  #endif
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif
  #ifdef FEATURE_WLAN
#error code not present
  #endif

  sdprl_mru_save( SDSR_MRU_LST,
                  hdr_mru_lst,
                  user_mru_lst,
                  sdprl_ptr->curr_nam
                );


  sdprl_wlan_mru_save( wlan_mru_lst );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For each NAM write its MRU to NV.
  */
  for( i=0; i < ARR_SIZE(sdprl_ptr->nam_info); i++ )
  {
    sdprl_mru_write( (sd_nam_e_type) i );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write WLAN mru to EFS.
  */
  sdprl_wlan_mru_write();




} /* sdprl_save */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_add_gw_sys

DESCRIPTION
  Add GSM/WCDMA systems to the specified list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize SDPRL.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_add_gw_sys(

        sdsr_e_type               list
            /* Specified list to add GW systems */

)
{
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #else
  SYS_ARG_NOT_USED(list);
  #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
} /* sdprl_add_gw_sys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_wlan_nam_sel

DESCRIPTION
  Inform the SD-PRL component of a new NAM selection and update
  WLAN preferences as per the NAM specified.

DEPENDENCIES
  sdprl_init() must have already been called to initialize SDPRL.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                   sdprl_wlan_nam_sel(

        sd_nam_e_type             curr_nam
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */
)
{
   #ifdef FEATURE_WLAN
#error code not present
  #else

  SYS_ARG_NOT_USED(curr_nam);

  #endif /* #ifdef FEATURE_WLAN */
 
}//sdprl_wlan_nam_sel()

/* <EJECT> */
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
MMEXTN  boolean                   sdprl_nam_sel(

        sd_nam_e_type             curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_hybr_pref_e_type    hybr_pref
            /* Current hybrid preference.
            */
        /*lint -esym(715, hybr_pref) */
)
{
  int                   i;
  sdsr_s_type           *sr_ptr;
  sdsr_ref_type         sr_ref;

  sdprl_s_type          *sdprl_ptr  = &sdprl;

  /*lint -save -e826  Suspicious 'buf' pointer-to-pointer conversion */
  sdnv_data_s_type      buf;
  nv_item_type          *item_ptr = (nv_item_type*) &buf;
  sdsr_e_type           hybr_mru_lst = SDSR_MAX_LST;
  sdsr_e_type           user_mru_lst = SDSR_MAX_LST;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(curr_nam, 0, SD_NAM_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* When starting NAM select, indicate prl is inactive.
  */
  sdprl_set_prl_active_status( FALSE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no NAM change, use current NAM.
  */
  if( curr_nam == SD_NAM_MAX )
  {
    curr_nam = sdprl_ptr->curr_nam;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the systems that are referenced by the SDSR_MRU_LST into the local
  ** MRU RAM buffer that corresponds to the existing NAM selection.
  */
  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  hybr_mru_lst = SDSR_HYBR_1_MRU_LST;
  #elif defined FEATURE_HDR
#error code not present
  #endif
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif

  sdprl_mru_save( SDSR_MRU_LST,
                  hybr_mru_lst,
                  user_mru_lst,
                  sdprl_ptr->curr_nam );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Change the NAM selection.
  */
  sdprl_ptr->curr_nam = curr_nam;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the MRU table that corresponds to the new NAM selection from its
  ** local RAM buffer into the SDSR_MRU_LST & SDSR_HDR_MRU_LST.
  */
  sdprl_mru_cpy( SDSR_MRU_LST, hybr_mru_lst, user_mru_lst, curr_nam );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the home SID/NID list of the current NAM selection.
  */
  if( ! sdprl_home_sid_nid_read(curr_nam) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read primary A/B channels.
  */

  item_ptr->pcdmach.nam = (byte) curr_nam;
  if( ! sdnv_read(NV_PCDMACH_I, item_ptr) )
  {
    return FALSE;
  }
  sdprl_ptr->nam_info[curr_nam].pch_a = item_ptr->pcdmach.channel_a;
  sdprl_ptr->nam_info[curr_nam].pch_b = item_ptr->pcdmach.channel_b;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read secondary A/B channels.
  */
  item_ptr->scdmach.nam = (byte) curr_nam;
  if( ! sdnv_read(NV_SCDMACH_I, item_ptr) )
  {
    return FALSE;
  }

  sdprl_ptr->nam_info[curr_nam].sch_a = item_ptr->scdmach.channel_a;
  sdprl_ptr->nam_info[curr_nam].sch_b = item_ptr->scdmach.channel_b;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_PRL_EXTENDED

  /* Read JTACS CDMA primary A/B channels.
  */
  item_ptr->jtacs_pcdmach.nam = (byte) curr_nam;
  if( ! sdnv_read(NV_JTACS_PCDMACH_I, item_ptr) )
  {
    return FALSE;
  }
  sdprl_ptr->nam_info[curr_nam].jtacs_pch_a = item_ptr->jtacs_pcdmach.channel_a;
  sdprl_ptr->nam_info[curr_nam].jtacs_pch_b = item_ptr->jtacs_pcdmach.channel_b;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read JTACS CDMA secondary A/B channels.
  */
  item_ptr->jtacs_scdmach.nam = (byte) curr_nam;
  if( ! sdnv_read(NV_JTACS_SCDMACH_I, item_ptr) )
  {
    return FALSE;
  }

  sdprl_ptr->nam_info[curr_nam].jtacs_sch_a = item_ptr->jtacs_scdmach.channel_a;
  sdprl_ptr->nam_info[curr_nam].jtacs_sch_b = item_ptr->jtacs_scdmach.channel_b;

  #endif /* FEATURE_PRL_EXTENDED */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the PRL component. If initialization fails, return FALSE.
  */
  if( ! prl_init(
             (byte) curr_nam,
             sdprl_ptr->nam_info[sdprl_ptr->curr_nam].home_sid_nid[0].sid,
             sdprl_ptr->nam_info[sdprl_ptr->curr_nam].home_sid_nid[0].nid,
             sdprl_ptr->nam_info[sdprl_ptr->curr_nam].amps_home_sid) )
  {
    SD_MSG_HIGH( "PRL read failed!, NAM=%d",curr_nam,0,0 );
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_USERZONE
  /* Initialize the PUZL component. If initialization fails, just continue.
  **
  */
  if( puzl_init((byte) curr_nam,
                 sdnv_read,
                 sdnv_puzl_read) != PUZL_ERROR_NO_ERR )
  {
    SD_MSG_HIGH( "PUZL read failed!, NAM=%d",curr_nam,0,0 );
  }
  #endif /* FEATURE_USERZONE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the loaded PRL.
  */
  sdprl_ptr->prl_ptr      = prl_buf_get_ptr();

  /* Point at structure containing information about the loaded PRL.
  */
  sdprl_ptr->prl_hdr_ptr  = prl_hdr_get_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the PRL's acquisition table from its NV storage format into the
  ** SDSR_PRL_LST list.
  */
  sdprl_acq_tbl_cpy( SDSR_PRL_LST );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ADD_HDR_IF_IS683A_PRL
#error code not present
  #endif /* FEATURE_ADD_HDR_IF_IS683A_PRL */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined (FEATURE_HDR) || defined (FEATURE_HYBR_GW) || \
       defined( FEATURE_ADD_HDR_IF_IS683A_PRL) ) 
      
  sdsr_list_clr( SDSR_SRV_COLLOC_LST );
  sdsr_list_clr( SDSR_PREV_COLLOC_LST );

  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif /* FEATURE_HDR */


  #ifdef FEATURE_ADD_HDR_IF_IS683A_PRL 
#error code not present
  #endif

  sdsr_list_cpy( SDSR_ACQ_COLLOC_LST, SDSR_SRV_COLLOC_LST );

  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_GPSONE_MSBASED
#error code not present
  #endif /* FEATURE_GPSONE_MSBASED */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  /* Clear GW ,user and ltd list
  */
  sdsr_list_clr( SDSR_GW_LST );
  sdsr_list_clr( SDSR_MANUAL_LST);
  sdsr_list_clr( SDSR_GW_LTD_LST);
  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  sdsr_list_clr( SDSR_HYBR_1_GW_LST );
  sdsr_list_clr( SDSR_HYBR_1_MANUAL_LST);
  sdsr_list_clr( SDSR_HYBR_1_GW_LTD_LST);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Add GSM/WCDMA systems to GW_LST for the following conditions.
  ** 1. If PRLA/B is used (i.e. GSM/WCDMA cannot be listed in the
  ** PRL),
  ** 2. If feature FEATURE_ADD_GW_IF_IS683C_PRL is defined and there is no gw
  ** records.
      */
  if( ( sdprl_ptr->prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_1 )
    #ifdef FEATURE_ADD_GW_IF_IS683C_PRL
                              ||
      !( sdss_sr_list_is_mode(SDSR_PRL_LST, SD_MODE_GSM)  ||
         sdss_sr_list_is_mode(SDSR_PRL_LST, SD_MODE_WCDMA)||
         sdss_sr_list_is_mode(SDSR_PRL_LST, SD_MODE_GW)
       )
    #endif
    )
  {
    sdprl_add_gw_sys( SDSR_GW_LST );
    #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
    sdprl_add_gw_sys( SDSR_HYBR_1_GW_LST );
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we have GSM/WCDMA systems, populate the SDSR_GW_LTD_SYS and
  ** SDSR_USER_LIST from PRL LST and PRL LST whose network selection type
  ** is auto.
  */
  for( i=0; i < sdsr_list_cnt( SDSR_PRL_LST ); i++ )
  {
    sdsr_list_update_gw_sys( SD_SS_MAIN,
                             sdsr_list_get_ref( SDSR_PRL_LST, i),
                             FALSE,
                             FALSE,
                             FALSE);

    #if (defined(FEATURE_TECH_IND_SD_STACK_ARCH) && defined(FEATURE_HYBR_GW))
#error code not present
    #endif
  }/* for */

  /* Populate the limited and user systems from SDSR_GW_LST.
  */
  for( i=0; i < sdsr_list_cnt( SDSR_GW_LST ); i++ )
  {
    sdsr_list_update_gw_sys(SD_SS_MAIN,
                            sdsr_list_get_ref( SDSR_GW_LST, i),
                            FALSE,
                            FALSE,
                            FALSE);

    #if (defined(FEATURE_TECH_IND_SD_STACK_ARCH) && defined(FEATURE_HYBR_GW))
#error code not present
    #endif

  }/* for */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this target support AMPS, make sure the system record table contains
  ** AMPS A and B system records (this is necessary for emergency
  ** originations). Set SDSR_AMPS_A_SYS and SDSR_AMPS_B_SYS to reference
  ** the stored AMPS A and B system records respectively.
  */
  sdsr_list_clr( SDSR_AMPS_A_SYS );
  sdsr_list_clr( SDSR_AMPS_B_SYS );

  if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_AMPS, SD_SS_BAND_PREF_ANY))
  {
    sr_ref = sdsr_tbl_store2( SD_MODE_AMPS,
                              sdss_map_band_to_band_pref(SD_BAND_CELL),
                              (sd_chan_type) SD_CELL_SYS_A );
    sdsr_list_put_ref( SDSR_AMPS_A_SYS, sr_ref );

    sr_ref = sdsr_tbl_store2( SD_MODE_AMPS,
                              sdss_map_band_to_band_pref(SD_BAND_CELL),
                              (sd_chan_type) SD_CELL_SYS_B );

    sdsr_list_put_ref( SDSR_AMPS_B_SYS, sr_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set PRL acquisition table AMPS system references as follows:
  **
  ** 1. SDSR_AMPS_1ST_SYS to reference 1st AMPS system on PRL_LST (if any).
  ** 2. SDSR_AMPS_2ND_SYS to reference 2nd AMPS system on PRL_LST (if any).
  **
  ** 3. SDSR_AMPS_BS1_SYS to reference 1st AMPS backstop system on PRL_LST.
  ** 4. SDSR_AMPS_BS2_SYS to reference 2nd AMPS backstop system on PRL_LST.
  */
  sdsr_list_clr( SDSR_AMPS_1ST_SYS );
  sdsr_list_clr( SDSR_AMPS_2ND_SYS );
  sdsr_list_clr( SDSR_AMPS_BS1_SYS );
  sdsr_list_clr( SDSR_AMPS_BS2_SYS );

  /* Find 1st AMPS system on SDSR_PRL_LST.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(SDSR_PRL_LST, i)) != NULL; i++ )
  {
    if( sr_ptr->sys.mode == SD_MODE_AMPS )
    {
      sr_ref = sdsr_list_get_ref( SDSR_PRL_LST, i );
      sdsr_list_put_ref( SDSR_AMPS_1ST_SYS, sr_ref );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( sdprl_ptr->is_srda )
      {
        sdsr_list_put_ref( SDSR_AMPS_BS1_SYS, sr_ref );
      }

      break;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find 2nd AMPS system on SDSR_PRL_LST.
  */
  for( i++; (sr_ptr=sdsr_list_get_ptr(SDSR_PRL_LST, i)) != NULL; i++ )
  {
    if( sr_ptr->sys.mode == SD_MODE_AMPS )
    {
      sr_ref = sdsr_list_get_ref( SDSR_PRL_LST, i );
      sdsr_list_put_ref( SDSR_AMPS_2ND_SYS, sr_ref );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( sdprl_ptr->is_srda )
      {
        sdsr_list_put_ref( SDSR_AMPS_BS2_SYS, sr_ref );
      }

      break;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the emergency list.
  */
  sdsr_list_clr( SDSR_EMERG_LST );


  /* If we got here, PRL is successfully read from NV , indicate so and
  ** return TRUE.
  */

  sdprl_set_prl_active_status( TRUE );

  return TRUE;

} /* sdprl_nam_sel */
/*lint +esym(715, hybr_pref) */
/*lint -restore */

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
MMEXTN  sys_roam_status_e_type    sdprl_roam_ind_map_custom_home_to_home(

        sys_roam_status_e_type    roam_ind
            /* Roaming indicator from system record.
            */

)
{

  uint8 byte_off = ((uint8) roam_ind) >> 3;
  uint8 bit_off  = ((uint8) roam_ind) & 0x07;

  /* If the roaming indication is marked as the custom home in the bitmap.
  */
  if( roam_ind_custom_home[byte_off] & BM(bit_off) )
  {
    return SYS_ROAM_STATUS_OFF;
  }

  return roam_ind;

}



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
MMEXTN  boolean                   sdprl_is_sys_match(

       sd_ss_e_type               ss,
           /* System selection enum type
           */

      sdsr_ref_type               sr_ref
           /* System reference
           */
)
{

  sdprl_s_type                    *sdprl_ptr  = &sdprl;

  prli_sys_rec_type               prl_sr;
  sdprl_parser_s_type             prl_parser;
  sdprl_match_sid_nid_e_type      match_lvl;
  dword                           num_bits = 0;
  sdsr_s_type                     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  sr_ptr = sdsr_tbl_get_ptr( sr_ref );

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     ss,
                     sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get(&prl_parser, &prl_sr);
  SD_ASSERT( num_bits > 0 );

  /*-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If the first system table entry or any other system table entry matches
  ** the system record in question, indicate so.
  */
  if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) ||
      sdprl_parser_next_match(&prl_parser, sr_ptr, &prl_sr, &match_lvl) )
  {
    SD_MSG_HIGH( "PRL match sys_idx=%d, level=%d",
                  prl_parser.sys_idx, match_lvl, 0 );

    return TRUE;
  }

  /*-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Return False if there is no match.
  */

  return FALSE;
}



/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_clr_mru

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
MMEXTN  void                      sdprl_clr_mru( void )
{

  sdprl_s_type      *sdprl_ptr = &sdprl;
  /* The currently selected NAM.
  */
  sd_nam_e_type     curr_nam=sdprl_ptr->curr_nam;

  sdprl_nam_s_type  *nam_ptr;

  byte              i;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[curr_nam];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the entries SD_MODE_INACT for all.
  */
  for( i = 0;
       i < ARR_SIZE(nam_ptr->mru_tbl);
       i++
     )
  {
    nam_ptr->mru_tbl[i].mode = (byte) SD_MODE_INACT;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write NAM to NV
  */
  sdprl_mru_write( (sd_nam_e_type) curr_nam );

} /* sdprl_clr_mru */


#if( defined(SD_DEBUG) && TG==T_PC && !defined(_lint) )
#error code not present
#endif

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
MMEXTN boolean sdprl_is_prl_gw_mode_gw(
       sd_ss_e_type               ss
)
{
  sdsr_s_type *list_ptr;
  sdsr_e_type list;
  sd_mode_e_type mode = SYS_SYS_MODE_NONE;
  int i;
  

  list = SDSR_PRL_LST;
  if( sdsr_list_cnt(sdss_sr_list_map2(ss, SDSR_GW_LST )) > 0 )
  {
    list = sdss_sr_list_map2(ss, SDSR_GW_LST );
  }
  
  for( i=0; (list_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( list_ptr->sys.mode == SYS_SYS_MODE_GW ||
        (list_ptr->sys.mode == SYS_SYS_MODE_GSM && mode == SYS_SYS_MODE_WCDMA) ||
        (list_ptr->sys.mode == SYS_SYS_MODE_WCDMA && mode == SYS_SYS_MODE_GSM))
    {
      return TRUE;            
    }
    
    if(SD_MODE_CONTAIN((sdss_map_mode_to_mode_pref(list_ptr->sys.mode)), SD_SS_MODE_PREF_GW))
    {
      mode = list_ptr->sys.mode;
    }
  } /*End FOR */

  return FALSE;
}




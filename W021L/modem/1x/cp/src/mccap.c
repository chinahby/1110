/*===========================================================================

          MAIN CONTROL SYSTEM CAPACITY MANAGER

GENERAL DESCRIPTION
   This module is keeping track of the underlying hardware and system capability.
   All other call processing modules look to this module to figure out if a
   particular parameter or configuration is do-able by the mobile.

DESCRIPTION
   This module has data structures to keep track of the capacity info and
   the interfaces to allow for the other modules to query  or validate any
   parameter

Copyright (c) 1999-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccap.c_v   1.30   01 Oct 2002 15:57:24   azafer  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccap.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/08   bb      Fixing the Klcowork errors
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
                   Featurized FEATURE_IS2000_DYNAMIC_FEATURE.
12/14/07   an      Added Passport Mode
04/27/07   an      Report the correct geo_loc_cap to the network
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
03/19/07   pg      Lint Cleanup.
01/18/06   pg      Disable common channel TD mode STS under
                   FEATURE_JCDMA_2.
12/01/06   pg      Permanently disable common channel TD under 
                   FEATURE_JCDMA_2.
09/02/06   fh      AKA/AES: Miscellaneous code review changes
08/09/06   fh      Changed the encryption capability report function: 
                      if AES related DF item is turning off the feature, it
                      returns non-support of AES.
08/04/06   fh      AKA/AES: cleaned up the usage of AES capability functions
07/07/06   fh      AKA/AES: Cleaned up AES capability helper functions.
06/27/06   fc      Corrected feature name.
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
05/18/06   fc      Set msg_integrity_supported to TRUE if AKA is defined.
04/26/06   an      Fixed Lint Errors
03/02/06   fh      Added AES support
07/11/05   fh      Cleaned up header file including from MUX:
                     added including of txcmc.h
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
06/13/05   an      Fixed an error with rev_sch_supported.
05/11/05   fc      Fixed NULL pointer reference issue in macro SYS_CAP_DF.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Fixed compilation error.
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
12/22/04   fc      Code review comment changes.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
09/28/04   fc      Added P_REV check for rev_pdch_supported.
09/27/04   va      Merged from REL B/C archive
07/15/04   fc        Removed dependency between Sync ID and Reconnect Msg.
09/24/04   va      Added REL D fields in system capability
09/21/04   az      Changed feature name from FEATURE_REL_D_NEG_SCI to 
                   FEATURE_IS2000_REL_D_NEG_SCI
09/16/04   fc      Added support for reverse DV.
08/31/04   az      Fixed a compiler issue when Rel D Neg SCI is not defined
08/23/04   az      Dynamic feature for Rel D neg SCI
08/12/04   bkm     Set max enc pkt size in system capability structure.
08/09/04   fc      Service negotiation changes for Release C and D.
08/03/04   pg      Added support for Rel D timer based registration.
06/16/04   fc      Added DF support for Sync ID and Reconnect message.
06/08/04   bkm     Fixed typos in feature names within comments.
06/04/04   bkm     Lint cleanup.
04/08/04   bkm     Channel mask updates, use 1 3-bit unit for ch_cfg_sup_map
                   and only show config 3 for now.  Added check if
                   simultaneous pdch/sch supported.  Fixed func that checks
                   ext_chind supported to handle values not reflected by the map.
03/11/04   bkm     Fixed some Rel C featurization.
02/05/04   bkm     Merged the following from IS2000_REL_B_C
  01/05/04   yll     Added checking whether for/rev sch is supported when
                     setting the SCH max rate.
01/05/04   bkm     Renamed mccap_check_chind_and_rc to mccap_rc_for_chan()
                   and added DV - PDCH support.
12/02/03   bkm     Added capability check on DV EXT_CH_IND.
12/02/03   bkm     Filled in F-PDCH capability info.
11/03/03   az      Fixed the sdb capability reporting field.
10/23/03   va      Added fns for RELC capability reporting
09/18/03   yll     Changes so that simultaneous_fch_dcch doesn't depend on 
                   cs_supported, although it requires the static SVD feature.
09/08/03   yll     Added checking of RX diversity when reporting TD cap.
08/13/03   yll     Fixed compile errors.
08/08/03   fc      Added support for IS2000 Release C.
08/01/03   yll     Added Dynamic Feature support.
06/24/03   va      Added snm.h as an include file.
06/02/03   az      Added changes for reporting MABO and SDB capability info 
                   to the base station.
06/04/03   va      The RLP CAP BLOB is now populated in SNM.
04/23/03   bkm     Corrected setting of ext_cap_included in
                   get_advanced_capability_fields.
03/28/03   va      Added support for RELB capability.
02/14/03   yll     Added FEATURE_AUTH for MS sig encryption capability.
01/02/03   bkm     Change calls INTLOCK/INTFREE to INTLOCK_SAV/INTFREE_SAV.
12/30/02   va      Checked mob_p_rev for td_mode capability check.
11/27/02   lh      Added FEATURE_IS2000_REL_A_MUXPDU5_32X_TURBO to control
                   Turbo Block Size definition.
11/22/02   fc      Renamed FEATURE_IS2000_REL_A_CC_TD to
                   FEATURE_IS2000_REL_A_TD.
11/07/02   az      Removed FEATURE_IS2000_REV0_PREADDENDUM
11/05/02   bkm     Added support for JCDMA Max Rate feature.
10/31/02   va      Added support for dynamic p_rev changing.
10/01/02   az      Mainlined FEATURE_IS2000 and removed dead code under #if 0
09/25/02   bkm     Featurize support for optional IS-2000 reverse FCH gating.
09/25/02   bkm     Added FEATURE_IS2000_SLOTTED_TIMER featurization around
                   MS capability reporting of this optional feature.
09/06/02   va      Cleaned up lint errors.
08/06/02   jrp     Added mccap_reach_rate_supported function.  This was
                   previously a macro in mccap.h.  It now supports 5ms
                   and 10ms frame sizes.
07/11/02   ph      Changed mccap_get_preferred_sch_coding() to return the
                   coding type which supports the max rate.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
06/28/02   lh      Reverse SCH MuxPDU 5 support.
06/14/02   fc      Renamed FEATURE_IS2000_REL_A_FCCCH_5MS_FRAME to
                   FEATURE_IS2000_REL_A_CC_FCCCH_5MS_FRAME.
                   Renamed FEATURE_IS2000_REL_A_FCCCH_10MS_FRAME to
                   FEATURE_IS2000_REL_A_CC_FCCCH_10MS_FRAME.
05/31/02   va      Added support for mccap_is_simult_fch_dcch_supported
03/11/02   ssw     Fixed problem with #ifdef around include of msm.h
03/05/02   fc      Corrected comment.
02/22/02   fc      Added support for Release A common channels.
02/13/02   va      Merged from MSM_CP.05.00.116
                   02/04/02 ssw     Include msm.h for MSM6050
12/11/01   lh      Merged following from SVD branch.
                   08/28/01   lh      Added support for Concurrent Service.
10/15/01   kk      Fixed compiler errors when Release A is not defined.
08/08/01   fc      Merged the following from common archive:
                   08/02/01   lcc     1. Corrected SCH MUX options reported to
                                         the BS to include only the maximum.
                                      2. Corrected SCH RCs reported to the BS
                                         when RS2 SCH is not supported.
                   07/31/01   ht      Enclosed #include dsrlp.h with FEATURE_DS.
                   07/18/01   lcc     Corrected limitation of R-SCH in MSM5105
                                      to 8x double size.
                   07/12/01   lcc     Added limitation of R-SCH to only 8x
                                      single size for MSM5105.
06/26/01   sb      Merged the following from common archive:
                   06/14/01   lcc     Removed references to non_octet_aligned_data and
                                      octet_aligned_data in caii_chan_cfg_capa_info_type.
                                      They are removed in Addendum 2 of IS2000 Rel 0.
                   05/29/01   ht      Added a #ifdef FEATURE_DS declaration.
                   05/15/01   lcc     Added a missing "break" (inside unused code).
05/16/01   fc      Merged the following from common archive.
                   04/24/01   lcc     Rename FEATURE_IS2000_CONTROL_HOLD to
                                      FEATURE_IS2000_CHS to be consistent.
                   04/10/01   lcc     Added support for P2 mode.
                   01/26/01   bkm     Included msmhw.h under T_MSM_5100
                                      featurization.
05/10/01   lh      Added 307K support.
05/04/01   va      Added mccap_is_cs_supported() function. Changed CS supported flag
                   to report FALSE.
04/18/01   kk      added td capability setting depending on the compile flags.
02/28/01   ph      merged in from MSM_CP.05.00.55
                   01/02/01   lcc     1. In mccap_get_ch_specific_fields, removed cases for SCH
                                         which are not used.
                                      2. In mccap_get_chan_cfg_info, added reporting of SCH and DCCH
                                         info.
                   11/29/00   lcc/hrk Added function mccap_get_preferred_sch_coding().
                   01/26/01   bkm     Included msmhw.h under T_MSM_5100 featurization
02/27/01   kk      Added function to retrieve 3x capabilty.
                   Also modified GPS capability initialization.
02/27/01   va      Added functions to retrieve the flex rates and SCH
                   var rates capability of the MS. mccap_is_flex_rates_supported
                   and mccap_is_sch_var_rates_supported.
01/29/01   ph      The definition of mccap_is_td_mode_supported() now uses the new
                   enums in cai.h (REL A).
01/10/01   va      Added mccap_is_td_supported function, This takes td_mode as
                   an argument.
01/10/01   ph      1. Added fields to system_capability for support of
                       STS, SR3, Ext. Encryption, Flex and Var rates, Concurrent Services,
                       40ms, 80ms frames, Max Data Rates for F-SCH and R-SCH.
                       Added functions mccap_get_encryption_cap_info() and updated
                       mccap_get_advanced_capability_fields() to retrieve above new fields.
                   2. mccap_get_ch_specific_fields() - added code for retrieval of
                       DCCH, F-SCH and R-SCH specific fields and mccap_get_chan_cfg_info()
                       now uses this function.
08/18/00   jq      Fixed a bug related to gating in MSM5000A based chip.
08/23/00   lcc     1. Corrected reporting of extended Mux Options in
                      mccap_get_ext_mux_option_info.
                   2. Corrected SCH mux lists in system_capability.
07/17/00   va      Set the R-FCH gating supported to TRUE.
07/14/00   ks      Set the slotter_timer to TRUE in
                   mccap_get_advanced_capability_fields() function.
07/10/00   va      Added mccap_get_gep_loc_cap to support GEO LOC IR.
06/12/00   va      Removed FEATURE_IS2000_SCH around the function that returns
                   the number of SCHs.
06/02/00   lcc     Changes to support IS2000 Rev 0 published addendum: r-fch
                   gating and rlp_cap_blob.
05/30/00   lcc     Added support for RLP_CAP_BLOB
04/07/00   va      Added suport for min_pwr_ctrl_step_size
02/17/00   lcc     Added functions mccap_is_dcch_supported, mccap_num_f_sch_supported,
                   mccap_num_r_sch_supported.
02/13/00   va      Addendum Changes.
12/20/99   va      Added mccap_get_ext_mux_option_info function to support
                   Ext Mux option record.
11/15/99   va      Initial revision of the file.
                   Created for the IS2000 Changes. This file keeps the capabaility
                   database and initializes it correctly based on s/w, h/w
                   capability.
===========================================================================*/
#include "mccap.h"
#include "mccdma.h"
#include "txcmc.h"
#include "caii.h"
#include "msg.h"
#ifdef FEATURE_DS
#include "dsrlp.h"
#endif
#include "snm.h"
#include "srchmc.h"
#include "mccapdf.h"

/*lint -e655 */


/* The table contains whether the DF item is supported. */
/* Since the values are also referenced in system_capability even when 
** dynamic_feature is not defined, it is defined outside of 
** FEATURE_IS2000_DYNAMIC_FEATURE. */

extern uint8 mccap_df_values[MCCAP_DF_MAX];

#define SYS_CAP(df_type) system_capability.cap_value[df_type]


/* This struct contains all the system capability info needed for
   service negotiation, SNM has to be congnizant of this in order
   to come up with an acceptable configuration */
typedef struct
{
  // Even though the RC map is in the cc_cap_info
  // would have this information, we need this list to tell us
  // our relative preference among these RCs
  byte num_fwd_sch_mux;
  word fwd_sch_mux[CAI_MAX_MOS];
  byte num_rev_sch_mux;
  word rev_sch_mux[CAI_MAX_MOS];
  byte min_pwr_ctrl_step;

  #ifdef FEATURE_IS2000_REL_A

  #ifdef FEATURE_IS2000_REL_A_CC
  /* Common Channel TD mode fields */
  boolean csch_otd_supported;
  boolean csch_sts_supported;
  /* FCCCH 5ms frame */
  boolean f_ccch_5ms_frame_supported;
  /* FCCCH 10ms frame */
  boolean f_ccch_10ms_frame_supported;
  #endif /* FEATURE_IS2000_REL_A_CC */

  /* Encryption Fields */
  byte signaling_encryption_support;
  byte user_encryption_support;

  /* Flex Rates */
  boolean flex_supported;
  boolean f_fch_flex_supported;
  boolean r_fch_flex_supported;
  boolean f_dcch_flex_supported;
  boolean r_dcch_flex_supported;
  boolean f_sch_flex_supported;
  boolean r_sch_flex_supported;

  /* Var Rates */
  boolean var_supported;
  boolean f_sch_var_supported;
  boolean r_sch_var_supported;
  word    max_sum_num_bits_c; // sum of max number of bits with Convolutional codes
  word    max_sum_num_bits_t; // sum of max number of bits with Turbo codes
  boolean ltu_table_supported;  //LTU table download supported
  boolean x3_cch_supported; // SR3 supported
  #endif /* FEATURE_IS2000_REL_A */

  boolean fch_supported;
  caii_fch_spec_fields_type fch_fields;      // FCH Type specific fields
  #ifdef FEATURE_IS2000_P2
  caii_dcch_spec_fields_type dcch_fields;    // DCCH Type specific fields
  #endif /* FEATURE_IS2000_P2 */
  #ifdef FEATURE_IS2000_F_SCH
  caii_for_sch_spec_fields_type f_sch_fields;  // F_SCH type specific files
  #endif /* FEATURE_IS2000_F_SCH */
  #ifdef FEATURE_IS2000_R_SCH
  caii_rev_sch_spec_fields_type r_sch_fields;  //R_SCH type specific fields
  #endif /* FEATURE_IS2000_R_SCH */
  byte  geo_loc_cap; //GEO Location Capability
  boolean simultaneous_fch_dcch;  // Indicates if FCH and DCCH are simultaneousely
  uint8   gating_rate_set;        // 0 = gating rate 1, 1 = 1 & 1/2, 2 = 1, 1/2, & 1/4
  boolean mabo_supported;         // Is MABO supported

  #ifdef FEATURE_IS2000_REL_B
  boolean eram_supported; /* Indicates if Enhanced Rate Adaptation Mode is supported*/
  boolean ccsh_supported; /* Indicates Code Combining Soft Handoff is supported*/
  #endif /* FEATURE_IS2000_REL_B */

  #ifdef FEATURE_IS2000_REL_C
  boolean for_pdch_supported;     /* Indicates if F-PDCH is supported */
  caii_for_pdch_spec_fields_type for_pdch_fields; /* F-PDCH Type specific fields */
  boolean for_pdch_sch_supported; /* Indicates if simulateous support of F-PDCH
                                     and F-SCH */
  boolean pdch_chm_supported;     /* PDCH control mode supported indicator */
  boolean msg_integrity_supported;/* Message integrity supported indicator */
  byte sig_integrity_sup;         /* Signaling message integrity support */
  #endif /* FEATURE_IS2000_REL_C */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  /* Capability values associated with Dynamic Features */
  uint8 *cap_value;
 } mccap_system_capability_info;

static mccap_system_capability_info system_capability =
{
  //
  // Forward SCH mux options.  We only need to report the maximum:  This is target dependent
  //
  // Note: MSM5000 and MSM5105 do not have FEATURE_DS_IS2000_RS2 defined and MSM5100
  // may have it defined.
  #ifdef FEATURE_IS2000_REL_A_MUXPDU5
  5, // num_fwd_sch_mux
  {
    CAI_MUX_VARIABLE,             /* MuxPDU 5, 1x-32x variable sch    */
    CAI_MUX_16X_RS1_DOUBLE,       /* MuxPDU 3, 16x, double sch     */
    CAI_MUX_8X_RS1_SINGLE,        /* MuxPDU 3,  8x, single sch     */

    #ifdef FEATURE_DS_IS2000_RS2_16X_F_SCH
    CAI_MUX_16X_RS2_DOUBLE,       /* If 32X RS1 is supported, RS2 16X is supported too */
    #else
    CAI_MUX_8X_RS2_DOUBLE,        /* RS2 MuxPDU 3,  8x, double sch */
    #endif /* FEATURE_DS_IS2000_RS2_16X_F_SCH */

    CAI_MUX_8X_RS2_SINGLE,        /* RS2 MuxPDU 3,  8x, single sch */
    0,0,0,0
  },
  #else /* FEATURE_IS2000_REL_A_MUXPDU5 */
  4, // num_fwd_sch_mux
  {
    CAI_MUX_16X_RS1_DOUBLE,       /* MuxPDU 3, 16x, double sch     */
    CAI_MUX_8X_RS1_SINGLE,        /* MuxPDU 3,  8x, single sch     */

    #ifdef FEATURE_DS_IS2000_RS2_16X_F_SCH
    CAI_MUX_16X_RS2_DOUBLE,       /* If 32X RS1 is supported, RS2 16X is supported too */
    #else
    CAI_MUX_8X_RS2_DOUBLE,        /* RS2 MuxPDU 3,  8x, double sch */
    #endif /* FEATURE_DS_IS2000_RS2_16X_F_SCH */

    CAI_MUX_8X_RS2_SINGLE,        /* RS2 MuxPDU 3,  8x, single sch */
    0,0,0,0,0
  },
  #endif /* FEATURE_IS2000_REL_A_MUXPDU5 */

  //
  // Reverse SCH mux options.
  //
  #ifdef FEATURE_IS2000_REL_A_MUXPDU5
  5, // num_rev_sch_mux
  {
    CAI_MUX_VARIABLE,             /* MuxPDU 5, 1-32x               */
    CAI_MUX_16X_RS1_DOUBLE,       /* MuxPDU 3, 16x, double sch     */
    CAI_MUX_8X_RS1_SINGLE,        /* MuxPDU 3,  8x, single sch     */

    #ifdef FEATURE_DS_IS2000_RS2_16X_R_SCH
    CAI_MUX_16X_RS2_DOUBLE,       /* If 32X RS1 is supported, RS2 16X is supported too */
    #else
    CAI_MUX_8X_RS2_DOUBLE,        /* RS2 MuxPDU 3,  8x, double sch */
    #endif /* FEATURE_DS_IS2000_RS2_16X_R_SCH */

    CAI_MUX_8X_RS2_SINGLE,        /* RS2 MuxPDU 3,  8x, single sch */
    0,0,0,0
  },
  #else
  4, // num_rev_sch_mux
  {
    CAI_MUX_16X_RS1_DOUBLE,       /* MuxPDU 3, 16x, double sch     */
    CAI_MUX_8X_RS1_SINGLE,        /* MuxPDU 3,  8x, single sch     */

    #ifdef FEATURE_DS_IS2000_RS2_16X_R_SCH
    CAI_MUX_16X_RS2_DOUBLE,       /* If 32X RS1 is supported, RS2 16X is supported too */
    #else
    CAI_MUX_8X_RS2_DOUBLE,        /* RS2 MuxPDU 3,  8x, double sch */
    #endif /* FEATURE_DS_IS2000_RS2_16X_R_SCH */

    CAI_MUX_8X_RS2_SINGLE,        /* RS2 MuxPDU 3,  8x, single sch */
    0,0,0,0,0
  },
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef T_MSM5000
  0x2,  //Min pwr ctrl step size of 0.25 db
  #else
#error code not present
  #endif /* T_MSM5000 */

  #ifdef FEATURE_IS2000_REL_A

  #ifdef FEATURE_IS2000_REL_A_CC
  // Common Control Channel TD mode fields */
  /* These values are updated in mccap_pm_init depending in what mode the 
     mobile is operated */
  #if ((!defined FEATURE_PASSPORT_MODE) && (defined FEATURE_IS2000_REL_A_TD))
  TRUE,  // csch_otd_supported
  TRUE,  // csch_sts_supported
  #else
  FALSE, // csch_otd_supported
  FALSE, // csch_sts_supported
  #endif /* FEATURE_IS2000_REL_A_TD */

  #ifdef FEATURE_IS2000_REL_A_CC_FCCCH_10MS_FRAME
  TRUE,  // f_ccch_10ms_frame_supported
  #else
  FALSE, // f_ccch_10ms_frame_supported
  #endif /* FEATURE_IS2000_REL_A_CC_FCCCH_10MS_FRAME */

  #ifdef FEATURE_IS2000_REL_A_CC_FCCCH_5MS_FRAME
  TRUE,  // f_ccch_5ms_frame_supported
  #else
  FALSE, // f_ccch_5ms_frame_supported
  #endif /* FEATURE_IS2000_REL_A_CC_FCCCH_5MS_FRAME */

  #endif /* FEATURE_IS2000_REL_A_CC */

  #ifdef FEATURE_AUTH

  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #else
  CAI_ENC_SUP_CMEA,  // Signaling Encryption Support - only CMEA for now.
  #endif /* FEATURE_IS2000_REL_A_AES */

  // User Encryption Support
  #ifdef FEATURE_IS2000_REL_A_AES  
#error code not present
  #else
  0,
  #endif /* FEATURE_IS2000_REL_A_AES */

  #else /* FEATURE_AUTH */
  
  0,  // no Signaling Encryption support without AUTH feature
  0,  // no UI encryption support withou AUTH feature
  #endif /* FEATURE_AUTH */               
  FALSE,  // Flex rates supported
  FALSE,  // f_fch_flex supported
  FALSE,  // r_fch_flex supported
  FALSE,  // f_dcch_flex
  FALSE,  // r_dcch_flex
  FALSE,  // f_sch_flex
  FALSE,  // r_sch_flex
  FALSE,  // Variable rates supported
  FALSE,  // f_sch_var
  FALSE,  // r_sch_var
  0,      // sum of max num bits with convolutional coding
  0,      // sum of max num bits with turbo coding
  FALSE,  // LTU table download supported
  FALSE,  // SR3 support
  #endif /* FEATURE_IS2000_REL_A */

  TRUE,       // fch_support
  {           // FCH specific fields starts here
    FALSE,    // fch_frame_size, 5ms frame not supported
    2,        // 6 bits in for_fch_rc_map
    0x3e,     // for_fch_rc_map
    2,        // 6 bits in rev_fch_rc_map
    0x3c      // rev_fch_rc_map
  },

  #ifdef FEATURE_IS2000_P2
  {           // DCCH specific fields starts here
    1,        // Only 20ms frame size supported
    2,        // 6 bits in FOR_DCCH_RC_MAP
    0xe,      // RC 3, 4, 5 supports DCCH
    2,        // 6 bits in REV_DCCH_RC_MAP
    0xc       // RC 3, 4 supports DCCH
  },
  #endif /* FEATURE_IS2000_P2 */

  #ifdef FEATURE_IS2000_F_SCH

  {           // Forword SCH specific fields starts here
    2,        // 6 bits in FOR_SCH_RC_MAP
    0xe,      // RC 3, 4, 5 supported on SCH
    1,        // FOR_SCH_NUM
    {
      { // SCH 0

        #ifdef MSMHW_TURBO_CODING
        TRUE,   // FOR_TURBO_SUPPORTED

        #ifdef FEATURE_IS2000_REL_A_MUXPDU5_32X_TURBO
        5,      // FOR_MAX_TURBO_BLOCK_SIZE = 6120 RS1 (32x)
        #else
        4,      // FOR_MAX_TURBO_BLOCK_SIZE = 3048 RS1 (16x)
        #endif  // FEATURE_IS2000_REL_A_MUXPDU5_32X_TURBO

        #else
        FALSE,  // Turbo coding not supported
        0,      // Filler
        #endif /* MSMHW_TURBO_CODING */

        TRUE,   // FOR_CONV_SUPPORTED

        #ifdef FEATURE_IS2000_REL_A_MUXPDU5_32X_F_SCH //32X
        5,     // FOR_MAX_CONV_BLOCK_SIZE = 6120 RS1 (32x)
        #else
        4,     // FOR_MAX_CONV_BLOCK_SIZE = 3048 RS1 (16x)
        #endif //  FEATURE_IS2000_REL_A_MUXPDU5_32X_F_SCH //32X

        #ifdef FEATURE_IS2000_REL_A
        FALSE,   // FOR 40 ms frame supported
        FALSE,    // FOR 80 ms frame supported

        #ifdef FEATURE_IS2000_REL_A_MUXPDU5_32X_F_SCH //32X
        5      //Max Rate 32X
        #else
        4      //Max Rate 16X
        #endif //  FEATURE_IS2000_REL_A_MUXPDU5_32X_F_SCH //32X

        #endif /* FEATURE_IS2000_REL_A */
      },
      { // SCH 1, not supported.  Just filler here
        FALSE, 0, FALSE, 0,
        #ifdef FEATURE_IS2000_REL_A
        FALSE, FALSE, 0
        #endif
      }
    }
  },
  #endif

  #ifdef FEATURE_IS2000_R_SCH

  {           // Reverse SCH specific fields starts here
    2,        // 6 bits in REV_SCH_RC_MAP
    0xc,      // RC 3, 4 supported on SCH
    1,        // REV_SCH_NUM
    {
      { // SCH 0

        #ifdef MSMHW_TURBO_CODING
        TRUE,   // REV_TURBO_SUPPORTED

        #ifdef FEATURE_IS2000_REL_A_MUXPDU5_32X_TURBO
        5,      // REV_MAX_TURBO_BLOCK_SIZE = 6120 RS1 (32x)
        #else
        4,      // REV_MAX_TURBO_BLOCK_SIZE = 3048 RS1 (16x)
        #endif  // FEATURE_IS2000_REL_A_MUXPDU5_32X_TURBO

        #else
        FALSE,  // Turbo coding not supported
        0,      // Filler
        #endif /* MSMHW_TURBO_CODING */

        TRUE,   // REV_CONV_SUPPORTED

        #ifdef FEATURE_IS2000_REL_A_MUXPDU5_32X_R_SCH //32X
        5,     // REV_MAX_CONV_BLOCK_SIZE = 3120 RS1 (32x)
        #else
        4,     // REV_MAX_CONV_BLOCK_SIZE = 3048 RS1 (16x)
        #endif // FEATURE_IS2000_REL_A_MUXPDU5_32X_R_SCH //32X

        #ifdef FEATURE_IS2000_REL_A
        FALSE,   // REV 40 ms frame supported
        FALSE,    // REV 80 ms frame supported

        #ifdef FEATURE_IS2000_REL_A_MUXPDU5_32X_R_SCH //32X
        CAI_SCH_RATE_RS1_32X_RS2_18X      // Max Rate (32x)
        #else
        CAI_SCH_RATE_RS1_16X_RS2_16X      // Max Rate (16x)
        #endif // FEATURE_IS2000_REL_A_MUXPDU5_32X_R_SCH //32X

        #endif /* FEATURE_IS2000_REL_A */
      },
      { // SCH 1, not supported.  Just filler here
        FALSE, 0, FALSE, 0,

        #ifdef FEATURE_IS2000_REL_A
        FALSE, FALSE, 0
        #endif /* FEATURE_IS2000_REL_A */
      }
    }
  },
  #endif /* FEATURE_IS2000_R_SCH */

  #ifndef FEATURE_CGPS
  #if defined FEATURE_GPSONE
#error code not present
  #else
  CAI_NO_MS_ASSIST_GEO_CAP
    // IS2000 is defined but GPS is not supported.
    // No MS assisted geo location capabilities
  #endif /* FEATURE_GPSONE */
  #else
#error code not present
  #endif /* FEATURE_CGPS */


  #ifdef FEATURE_IS2000_REL_A_SVD
  ,TRUE                      // FCH and DCCH supported at the same time
  #else
  ,FALSE
  #endif /* FEATURE_IS2000_REL_A_SVD */

  ,2                          // All gating rate supported (when above is TRUE)
  ,FALSE  // MABO is not supported

  #ifdef FEATURE_IS2000_REL_B
  ,FALSE /* eram not supported */
  ,FALSE /* ccsh not supported */
  #endif /* FEATURE_IS2000_REL_B */

  #ifdef FEATURE_IS2000_REL_C

  #if defined(FEATURE_IS2000_REL_C_DV) || defined(FEATURE_IS2000_DV_SIG_TEST)
#error code not present
  #else
  ,FALSE, /* F-PDCH not supported */
  { /* F-PDCH specific fields start here */
    0, 0, 0, 0, 0, 0
  }
  #endif /* FEATURE_IS2000_REL_C_DV || FEATURE_IS2000_DV_SIG_TEST */

  ,FALSE /* Simulateous F-PDCH and F-SCH not supported */
  ,FALSE /* PDCH control hold mode not supported */
  ,FALSE /* Message Integrity not supported */

  #ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
  #else
  ,FALSE /* Signaling Message Integrity not supported */
  #endif /* FEATURE_IS2000_REL_C_AKA */

  #endif /* FEATURE_IS2000_REL_C */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */
 
  , (uint8*)mccap_df_values /* cap_values */
};

/* The Max Rate feature causes an override of the capabilities in the
system_capability structure. The information which get overriden is pulled out
in this "subset" struct type */

/* We support the rates of 1x, 2x, 4x, 8x, 16x and 32x */
#define MCCAP_NUM_SUPPORTED_RATES   (CAI_SCH_RATE_RS1_32X_RS2_18X + 1)   /*lint !e641 */

typedef struct {
   byte num_sch_mux;
   word sch_mux[CAI_MAX_MOS];
   byte max_turbo_block_size;
   byte max_conv_block_size;
} system_capability_subset;

#if (defined FEATURE_IS2000_SCH && defined FEATURE_IS2000_DYNAMIC_FEATURE)
 /* Please see the function mccap_pm_init where these values are modified. If the array
  * position of this entry changes this function also needs to change as the function 
  * now uses constant numbers to refer to the position in the table */

static system_capability_subset sys_cap_subset[MCCAP_NUM_SUPPORTED_RATES] = { /*lint !e641 */
  {
    /* 1x SCH */
    2,                      // num_sch_mux
    {CAI_MUX_1X_RS1,        // RS1 1x SCH
    CAI_MUX_1X_RS2,         // RS2 1x SCH
    0,0,0,0,0,0,0},         // filler
    0,                      // max turbo block - 1x SCH 172/267 bits
    0                       // max conv block - 1x SCH 172/267 bits
  },
  {
    /* 2x SCH */
    4,                      // num_sch_mux
    {CAI_MUX_2X_RS1_DOUBLE,
    CAI_MUX_2X_RS1_SINGLE,
    CAI_MUX_2X_RS2_DOUBLE,
    CAI_MUX_2X_RS2_SINGLE,
    0,0,0,0,0},             // filler
    1,                      // max turbo block - 2x SCH 360/552 bits
    1                       // max conv block - 2x SCH 360/552 bits
  },
  {
    /* 4x SCH */
    4,                      // num_sch_mux
    {CAI_MUX_4X_RS1_DOUBLE,
    CAI_MUX_4X_RS1_SINGLE,
    CAI_MUX_4X_RS2_DOUBLE,
    CAI_MUX_4X_RS2_SINGLE,
    0,0,0,0,0},             // filler
    2,                      // max turbo block - 4x SCH 744/1128 bits
    2                       // max conv block - 4x SCH 744/1128 bits
  },
  {
    /* 8x SCH */
    4,                      // num_sch_mux
    {CAI_MUX_8X_RS1_DOUBLE,
    CAI_MUX_8X_RS1_SINGLE,
    CAI_MUX_8X_RS2_DOUBLE,
    CAI_MUX_8X_RS2_SINGLE,
    0,0,0,0,0},             // filler
    3,                      // max turbo block - 8x SCH 1512/2280 bits
    3                       // max conv block - 8x SCH 1512/2280 bits
  },
  {
    /* 16x SCH */
    4,                      // num_sch_mux
    {
     CAI_MUX_16X_RS1_DOUBLE,
     CAI_MUX_8X_RS1_SINGLE,
     /* Please see the function mccap_pm_init where these values are modified. If the array
      * position of this entry changes this function also needs to change as the function 
      * now uses constant numbers to refer to this position in the table */
     #if (defined (FEATURE_DS_IS2000_RS2_16X_R_SCH) && !defined(FEATURE_PASSPORT_MODE))
     /* JCDMA not support IS2000 Rel A, support RS2 packet data, but not RS2 16x */
     CAI_MUX_16X_RS2_DOUBLE,/* RS2 16X, not supported in JCDMA. */
     #else
     CAI_MUX_8X_RS2_DOUBLE, /* RS2 MuxPDU 3,  8x, double sch */
     #endif /* FEATURE_DS_IS2000_RS2_16X_R_SCH && !FEATURE_PASSPORT_MODE */

     CAI_MUX_8X_RS2_SINGLE,
     0,0,0,0,0  //filler
    },            
    4,                      // max turbo block - 16x SCH 3048/4584 bits
    4                       // max conv block - 16x SCH 3049/4584 bits
  },
  {
    /* 32x SCH */
    4,                      // num_sch_mux
    {
     /* CAI_MUX_VARIABLE, will be added in mccap_sch_set_cap() */
     CAI_MUX_16X_RS1_DOUBLE,/* MuxPDU 3, 16x, double sch     */
     CAI_MUX_8X_RS1_SINGLE, /* MuxPDU 3,  8x, single sch     */

     #ifdef FEATURE_DS_IS2000_RS2_16X_R_SCH
     CAI_MUX_16X_RS2_DOUBLE,/* If 32X RS1 is supported, RS2 16X is supported too */
     #else
     CAI_MUX_8X_RS2_DOUBLE, /* RS2 MuxPDU 3,  8x, double sch */
     #endif /* FEATURE_DS_IS2000_RS2_16X_R_SCH */

     CAI_MUX_8X_RS2_SINGLE, /* RS2 MuxPDU 3,  8x, single sch */
     0,0,0,0,0              // filler
    },
    5,                      // max turbo block - 32x SCH 6120/9192 bits
    5                       // max conv block - 32x SCH 6120/9192 bits
  }
};
#endif /* FEATURE_IS2000_SCH && FEATURE_IS2000_DYNAMIC_FEATURE */

/*===========================================================================

FUNCTION mccap_check_rc_for_chan

DESCRIPTION
  Given the channel masks and the fwd and rev RCs, this function checks if
  these RCs are doable and compatible with each other for the given channels.

  The RC parameters must be valid for all channels in the mask.  Separate
  calls to validate PDCH and RC and FCH/DCCH and RC are required.

DEPENDENCIES
  None.

RETURN VALUE
  status: true if this is good and false if it is not doable.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccap_check_rc_for_chan
(
  mcc_fwd_chan_mask_type fwd_chan_mask,
  mcc_rev_chan_mask_type rev_chan_mask,
  cai_radio_config_type fwd_rc,
  cai_radio_config_type rev_rc
)
{
  if (fwd_chan_mask & MCC_F_FCH)  
  {
    if (system_capability.fch_supported)
    {
      /* Standard wants the units of len to be 3 bits hence we store it in thoese units */
      //lint -e734  No actual loss of precision here
      byte for_fch_rcmap_len = system_capability.fch_fields.for_fch_len * 3; /* In bits */
      //lint +e734
      if ( ! (system_capability.fch_fields.for_fch_rc_map >>(for_fch_rcmap_len-fwd_rc) & 1))  /*lint !e641 */
      {
        MSG_HIGH("F-FCH RC not supported", 0,0,0);
        return FALSE;
      }
    }
  }

  if (rev_chan_mask & MCC_R_FCH)  /*lint !e641 */
  {
    if (system_capability.fch_supported)
    {
      /* Standard wants the units of len to be 3 bits hence we store it in thoese units */
      //lint -e734  No actual loss of precision here
      byte rev_fch_rcmap_len = system_capability.fch_fields.rev_fch_len * 3; /* In bits */
      //lint +e734
      if ( ! (system_capability.fch_fields.rev_fch_rc_map >>(rev_fch_rcmap_len-rev_rc) & 1))  /*lint !e641 */
      {
        MSG_HIGH("R-FCH RC not supported", 0,0,0);
        return FALSE;
      }
    }
  }

  if (fwd_chan_mask & MCC_F_DCCH) 
  {
    #ifdef FEATURE_IS2000_P2
    if (SYS_CAP(MCCAP_DF_DCCH))
    {
      //lint -e734  No actual loss of precision here
      byte for_rcmap_len = system_capability.dcch_fields.for_dcch_len * 3; /* In bits */
      //lint +e734
      if ( ! (system_capability.dcch_fields.for_dcch_rc_map >>(for_rcmap_len-fwd_rc) & 1))  /*lint !e641 */
      {
        MSG_HIGH("F-DCCH RC not supported", 0,0,0);
        return FALSE;
      }
    }
    #else
    MSG_HIGH("F-DCCH RC not supported", 0,0,0);
    return FALSE;
    #endif /* FEATURE_IS2000_P2 */
  }

  if (rev_chan_mask & MCC_R_DCCH)  
  {
    #ifdef FEATURE_IS2000_P2
    if (SYS_CAP(MCCAP_DF_DCCH))
    {
      //lint -e734  No actual loss of precision here
      byte rev_rcmap_len = system_capability.dcch_fields.rev_dcch_len * 3; /* In bits */
      //lint +e734
      if ( ! (system_capability.dcch_fields.rev_dcch_rc_map >>(rev_rcmap_len-rev_rc) & 1))  /*lint !e641 */
      {
        MSG_HIGH("R-DCCH RC not supported", 0,0,0);
        return FALSE;
      }
    }
    #else
    MSG_HIGH("R-DCCH RC not supported", 0,0,0);
    return FALSE;
    #endif /* FEATURE_IS2000_P2 */
  }
  
  /* If both FCH and DCCH are requested, validate that MS supports them simultaneously */
  if (((fwd_chan_mask & MCC_F_FCH)  || (rev_chan_mask & MCC_R_FCH)) &&     
      ((fwd_chan_mask & MCC_F_DCCH) || (rev_chan_mask & MCC_R_DCCH)))      
  {
    if ( ! system_capability.simultaneous_fch_dcch )
    {
      MSG_HIGH("Simultaneous FCH and DCCH not supported", 0,0,0);
      return FALSE;
    }
  }

#ifdef FEATURE_IS2000_REL_C
  /* Check F-PDCH RC */
  if (fwd_chan_mask & MCC_F_PDCH) 
  {
    /*lint -e734  No actual loss of precision here */
    uint8 for_rcmap_len = (system_capability.for_pdch_fields.for_pdch_len + 1) * 3; /* In bits */
    /*lint +e734 */
    if (!system_capability.for_pdch_supported || 
        !((system_capability.for_pdch_fields.for_pdch_rc_map >> (for_rcmap_len - (fwd_rc - CAI_RC_10 + 1))) & 1)) /*lint !e641 !e834 !e656*/
    {
      MSG_HIGH("F-PDCH or indicated F-PDCH RC %d not supported", fwd_rc,0,0);
      return FALSE;
    }
  }
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

  /* If we get here, channels and RCs are OK */
  return TRUE;

} /* mccap_check_rc_for_chan */

/*===========================================================================

FUNCTION mccap_is_enhanced_operation_supported

DESCRIPTION
   This function returns true if QPCH or any RC class 2 RCs are supported, if
   niether of these 2 is supported, false is returned

DEPENDENCIES
  None.

RETURN VALUE
  status: true or false as described above.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_enhanced_operation_supported( void )
{
  if (SYS_CAP(MCCAP_DF_QPCH))
  {
    return TRUE;
  }
  if (SYS_CAP(MCCAP_DF_DCCH))
  {
    /* If we support DCCH obviously we support enhanced RCs */
    return TRUE;
  }
  if (system_capability.fch_supported)
  {
    byte bit_map_for_rc_1, bit_map_for_rc_2, bit_map_for_rc_1_2;
    /* Check if we support RCs greater than 2*/
    bit_map_for_rc_1 = (byte) (0x1 << (system_capability.fch_fields.for_fch_len*3-CAI_RC_1));  /*lint !e641 */
    bit_map_for_rc_2 = (byte) (0x1 << (system_capability.fch_fields.for_fch_len*3-CAI_RC_2));  /*lint !e641 */
    bit_map_for_rc_1_2 = bit_map_for_rc_1 | bit_map_for_rc_2;
    /* After Ex-Or if the resulting bit-map is non-zero it means we support
     more than just RC1 and 2 */
    if (system_capability.fch_fields.for_fch_rc_map ^ bit_map_for_rc_1_2)
    {
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef FEATURE_IS2000_F_SCH
/*===========================================================================

FUNCTION mccap_num_f_sch_supported

DESCRIPTION
  This function returns number of F-SCH supported.

DEPENDENCIES
  None.

RETURN VALUE
  See description above.

SIDE EFFECTS
  None.

===========================================================================*/
byte mccap_num_f_sch_supported( void )
{
  if ( !SYS_CAP(MCCAP_DF_FOR_SCH) )
  {
    return 0;
  }

  return system_capability.f_sch_fields.for_sch_num;
}
#endif /* FEATURE_IS2000_F_SCH */ 

#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================

FUNCTION mccap_num_r_sch_supported

DESCRIPTION
  This function returns number of R-SCH supported.

DEPENDENCIES
  None.

RETURN VALUE
  See description above.

SIDE EFFECTS
  None.

===========================================================================*/
byte mccap_num_r_sch_supported( void )
{
  if ( !SYS_CAP(MCCAP_DF_REV_SCH) )
  {
    return 0;
  }

  return system_capability.r_sch_fields.rev_sch_num;
}
#endif /* FEATURE_IS2000_R_SCH */

/*===========================================================================

FUNCTION mccap_get_ch_specific_fields

DESCRIPTION
  This function returns the channel specific fields for FCH or DCCH.

DEPENDENCIES
  None

RETURN VALUE
  status: true if the specified channel is supported else false

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_get_ch_specific_fields
(
  mccap_ch_type  chan_type,
  void * data_ptr
)
{
  boolean status = FALSE;

  switch (chan_type)
  {
    case MCCAP_FCH:
    {
      if (system_capability.fch_supported)
      {
        // Copy FCH type specific fields
        caii_fch_spec_fields_type *fch_ptr = (caii_fch_spec_fields_type *)data_ptr;
        *fch_ptr = system_capability.fch_fields;
        status = TRUE;
      }
      break;
    }

    case MCCAP_DCCH:
    {
#ifdef FEATURE_IS2000_P2
      if (SYS_CAP(MCCAP_DF_DCCH))
      {
        // Copy DCCH type specific fields
        caii_dcch_spec_fields_type *dcch_ptr = (caii_dcch_spec_fields_type *)data_ptr;
        *dcch_ptr = system_capability.dcch_fields;
        status = TRUE;
      }
#endif /* FEATURE_IS2000_P2 */
      break;
    }

    case MCCAP_FWD_SCH:
    {
#ifdef FEATURE_IS2000_F_SCH
      if (SYS_CAP(MCCAP_DF_FOR_SCH))
      {
         // Copy For SCH type specific fields
         caii_for_sch_spec_fields_type *f_sch_ptr = (caii_for_sch_spec_fields_type *)data_ptr;
         *f_sch_ptr = system_capability.f_sch_fields;
         status = TRUE;
      }
#endif /* FEATURE_IS2000_F_SCH */
      break;
     }

    case MCCAP_REV_SCH:
    {
#ifdef FEATURE_IS2000_R_SCH
      if (SYS_CAP(MCCAP_DF_REV_SCH))
      {
         // Copy Rev SCH type specific fields
         caii_rev_sch_spec_fields_type *r_sch_ptr = (caii_rev_sch_spec_fields_type *)data_ptr;
         *r_sch_ptr = system_capability.r_sch_fields;
         status=TRUE;
      }
#endif /* FEATURE_IS2000_R_SCH */
      break;
    }

#ifdef FEATURE_IS2000_REL_C
    case MCCAP_FOR_PDCH:
    {
      if (system_capability.for_pdch_supported)
      {
         /* Copy Forward PDCH type specific fields */
         caii_for_pdch_spec_fields_type *f_pdch_ptr = (caii_for_pdch_spec_fields_type *)data_ptr;
         *f_pdch_ptr = system_capability.for_pdch_fields;
         status=TRUE;
      }
      break;
    }
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

    default:
      break;
  }
  return status;

}

/*===========================================================================

FUNCTION mccap_get_chan_cfg_info

DESCRIPTION
   This function populates the passed in channel config IR

DEPENDENCIES
  None.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
void mccap_get_chan_cfg_info
( 
  caii_chan_cfg_capa_info_type *ccc_ptr
)
{
  ccc_ptr->otd_supported = mccap_is_td_mode_supported(CAI_TD_MODE_OTD);
  if ( (ccc_ptr->fch_supported = system_capability.fch_supported) != FALSE )
  {
    ccc_ptr->fch_fields = system_capability.fch_fields;
  }

  ccc_ptr->dcch_supported = (boolean) SYS_CAP(MCCAP_DF_DCCH);
#ifdef FEATURE_IS2000_P2
  if ( ccc_ptr->dcch_supported == TRUE )
  {
    ccc_ptr->dcch_fields = system_capability.dcch_fields;
  }
#endif /* FEATURE_IS2000_P2 */
  ccc_ptr->for_sch_supported = (boolean) SYS_CAP(MCCAP_DF_FOR_SCH);
  ccc_ptr->rev_sch_supported = (boolean) SYS_CAP(MCCAP_DF_REV_SCH);
#ifdef FEATURE_IS2000_F_SCH
  if ( ccc_ptr->for_sch_supported == TRUE )
  {
    ccc_ptr->for_sch_fields = system_capability.f_sch_fields;
  }
#endif
#ifdef FEATURE_IS2000_R_SCH
  if ( ccc_ptr->rev_sch_supported == TRUE )
  {
    ccc_ptr->rev_sch_fields = system_capability.r_sch_fields;
  }
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_REL_A
  ccc_ptr->sts_supported = mccap_is_td_mode_supported(CAI_TD_MODE_STS);
  ccc_ptr->threex_cch_supported = system_capability.x3_cch_supported;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_B
  ccc_ptr->ccsh_supported = system_capability.ccsh_supported;
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C
  ccc_ptr->for_pdch_supported = system_capability.for_pdch_supported;

  if (ccc_ptr->for_pdch_supported)
  {
    ccc_ptr->for_pdch_fields = system_capability.for_pdch_fields;
    ccc_ptr->for_pdch_sch_supported = system_capability.for_pdch_sch_supported;
  }
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

} /* mccap_get_chan_cfg_info */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION mccap_get_thx_cch_supported

DESCRIPTION
   This function populates the passed 3x supported boolean

DEPENDENCIES
  initialize function should be called before this.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
boolean  mccap_get_thx_cch_supported ( void )
{
  return system_capability.x3_cch_supported;
}

#endif

/*===========================================================================

FUNCTION mccap_get_advanced_capability_fields

DESCRIPTION
  This function populates IS2000 specific fields for
  the capability info record passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccap_get_advanced_capability_fields
( 
  caii_capa_rec_type *cap_ptr
)
{
  cap_ptr->qpch = (boolean) SYS_CAP(MCCAP_DF_QPCH);

  // Determines if MABO and SDB fields are included.
  cap_ptr->ext_cap_included = TRUE;

  /* Reporting SDB capability */
  cap_ptr->sdb = (boolean) SYS_CAP(MCCAP_DF_SDB);

  /* Reporting MABO capability */
  cap_ptr->mabo = system_capability.mabo_supported;
  cap_ptr->slotted_timer = (boolean) SYS_CAP(MCCAP_DF_SLOTTED_TIMER);

  if ( (cap_ptr->chs_supported = (boolean) SYS_CAP(MCCAP_DF_CHS)) != FALSE )
  {
    cap_ptr->gating_rate_set = system_capability.gating_rate_set;
  }

  cap_ptr->rlp_cap_blob_len = snm_get_rlp_cap_blob( cap_ptr->rlp_cap_blob);

#ifdef FEATURE_IS2000_REL_A
  cap_ptr->flex_supported = system_capability.flex_supported;
  cap_ptr->f_fch_flex_supported = system_capability.f_fch_flex_supported;
  cap_ptr->r_fch_flex_supported = system_capability.r_fch_flex_supported;
  cap_ptr->f_dcch_flex_supported = system_capability.f_dcch_flex_supported;
  cap_ptr->r_dcch_flex_supported = system_capability.r_dcch_flex_supported;
  cap_ptr->f_sch_flex_supported = system_capability.f_sch_flex_supported;
  cap_ptr->r_sch_flex_supported = system_capability.r_sch_flex_supported;
  cap_ptr->var_supported = system_capability.var_supported;
  cap_ptr->f_sch_var_supported = system_capability.f_sch_var_supported;
  cap_ptr->r_sch_var_supported = system_capability.r_sch_var_supported;
  cap_ptr->max_num_bits_c = system_capability.max_sum_num_bits_c;
  cap_ptr->max_num_bits_t = system_capability.max_sum_num_bits_t;
  cap_ptr->cs_supported = (boolean) SYS_CAP(MCCAP_DF_CS);
  cap_ptr->f_sch_ltu_tab_supported = system_capability.ltu_table_supported;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_B
  cap_ptr->eram_supported = system_capability.eram_supported;
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C
  cap_ptr->pdch_chm_supported = system_capability.pdch_chm_supported;
  cap_ptr->rev_fch_gating_supported = (boolean) SYS_CAP(MCCAP_DF_REV_FCH_GATING);
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

} /* mccap_get_advanced_capability_fields */

/*===========================================================================

FUNCTION mccap_get_ext_mux_option_info

DESCRIPTION
   This function populates the ext mux option record pointer
   from the system capability info

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
void mccap_get_ext_mux_option_info ( caii_ext_mult_opt_info_type  *rec_ptr)
{
  #ifdef FEATURE_IS2000_SCH
  uint8 i,j;
  #endif /* FEATURE_IS2000_SCH */

  /* Populate FCH mux info */
  if (system_capability.fch_supported)
  {
    rec_ptr->num_mo_for_fch = 2;
    rec_ptr->mo_for_fch_recs[0].mo_for_fch = CAI_MUX1;
    rec_ptr->mo_for_fch_recs[0].for_rates_fch = CAI_RS1_ALL;
    rec_ptr->mo_for_fch_recs[1].mo_for_fch = CAI_MUX2;
    rec_ptr->mo_for_fch_recs[1].for_rates_fch = CAI_RS2_ALL;

    rec_ptr->num_mo_rev_fch = 2;
    rec_ptr->mo_rev_fch_recs[0].mo_rev_fch = CAI_MUX1;
    rec_ptr->mo_rev_fch_recs[0].rev_rates_fch = CAI_RS1_ALL;
    rec_ptr->mo_rev_fch_recs[1].mo_rev_fch = CAI_MUX2;
    rec_ptr->mo_rev_fch_recs[1].rev_rates_fch = CAI_RS2_ALL;
  }
  else
  {
    rec_ptr->num_mo_for_fch = 0;
    rec_ptr->num_mo_rev_fch = 0;
  }

#ifdef FEATURE_IS2000_P2
  /* Populate DCCH mux info */
  if (SYS_CAP(MCCAP_DF_DCCH))
  {
    rec_ptr->num_mo_for_dcch = 2;
    rec_ptr->mo_for_dcch[0] = CAI_MUX1;
    rec_ptr->mo_for_dcch[1] = CAI_MUX2;

    rec_ptr->num_mo_rev_dcch = 2;
    rec_ptr->mo_rev_dcch[0] = CAI_MUX1;
    rec_ptr->mo_rev_dcch[1] = CAI_MUX2;
  }
  else
#endif
  {
    rec_ptr->num_mo_for_dcch = 0;
    rec_ptr->num_mo_rev_dcch = 0;
  }

#ifdef FEATURE_IS2000_F_SCH
  /* Populate F-SCH mux info */
  if (SYS_CAP(MCCAP_DF_FOR_SCH))
  {
    uint8 rec = 0;

    rec_ptr->num_mo_for_sch = system_capability.f_sch_fields.for_sch_num *
                              system_capability.num_fwd_sch_mux;  //lint !e734
    // Assumes SCH 0 and SCH1 (though not supported) has same number of
    // mux. options supported.
    for (i=0; i<system_capability.f_sch_fields.for_sch_num; i++)
    {
      for (j=0; j<system_capability.num_fwd_sch_mux; j++)
      {
        rec_ptr->mo_for_sch_recs[rec].for_sch_id = i;
        rec_ptr->mo_for_sch_recs[rec++].mo_for_sch = system_capability.fwd_sch_mux[j];
      }
    }
  }
  else
#endif
  {
    rec_ptr->num_mo_for_sch = 0;
  }

#ifdef FEATURE_IS2000_R_SCH
  /* Populate R-SCH mux info */
  if (SYS_CAP(MCCAP_DF_REV_SCH))
  {
    uint8 rec = 0;

    rec_ptr->num_mo_rev_sch = system_capability.r_sch_fields.rev_sch_num *
                              system_capability.num_rev_sch_mux;  //lint !e734
    // Assumes SCH 0 and SCH1 (though not supported) has same number of
    // mux. options supported.
    for (i=0; i<system_capability.r_sch_fields.rev_sch_num; i++)
    {
      for (j=0; j<system_capability.num_rev_sch_mux; j++)
      {
        rec_ptr->mo_rev_sch_recs[rec].rev_sch_id = i;
        rec_ptr->mo_rev_sch_recs[rec++].mo_rev_sch = system_capability.rev_sch_mux[j];
      }
    }
  }
  else
#endif
  {
    rec_ptr->num_mo_rev_sch = 0;
  }

#ifdef FEATURE_IS2000_REL_C
  if (system_capability.for_pdch_supported)
  {
    /* Only 1 mux option for F-PDCH */
    rec_ptr->num_mo_for_pdch = 1;
    rec_ptr->mo_for_pdch[0] = CAI_MUX_0XF00;
  }
  else
  {
    rec_ptr->num_mo_for_pdch = 0;
  }
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

} /* mccap_get_ext_mux_option_info */

/*===========================================================================

FUNCTION mccap_get_min_pwr_ctrl_step

DESCRIPTION
   This function returns the minimum pwr ctrl step size that can
   be supported by the underlying MSM

DEPENDENCIES
   None

RETURN VALUE
   Min pwr control step size

SIDE EFFECTS
  None.

===========================================================================*/
byte mccap_get_min_pwr_ctrl_step ( void )
{
  return system_capability.min_pwr_ctrl_step;
}

/*===========================================================================

FUNCTION mccap_get_geo_loc_cap

DESCRIPTION
   This function returns the GEO location Capability of the MS

DEPENDENCIES
   None

RETURN VALUE
  geo location capability value

SIDE EFFECTS
  None.

===========================================================================*/
byte mccap_get_geo_loc_cap ( void )
{
  return system_capability.geo_loc_cap;
}

#ifdef FEATURE_IS2000_SCH
/*===========================================================================

FUNCTION mccap_get_preferred_sch_coding

DESCRIPTION
  This function returns turbo coding if turbo code is supported and the max
  turbo rate is better or equal to conv; otherwise, convoluational coding is returned.
  (It is assumed that conv coding is always supported.)  For simplicity, we are
  making the assumption that coding support is uniform across all SCH physical
  channels (0, 1, F, R).

DEPENDENCIES
  The caller has to make sure that SCH is supported.  If this function is called
  when SCH is not supported, the result is unpredictable..

RETURN VALUE
  Coding type that supports the maximum rate.

SIDE EFFECTS
  None.

===========================================================================*/
cai_sch_coding_type mccap_get_preferred_sch_coding( void )
{
  // Since we are making the assumption that coding support is uniform on all
  // SCHs, just check F-SCH 0 here.
  // Assuming CONV coding is always supported.
  if ((!system_capability.f_sch_fields.for_sch_recs[0].for_turbo_supported) ||
      (system_capability.f_sch_fields.for_sch_recs[0].for_max_conv_block_size >
        system_capability.f_sch_fields.for_sch_recs[0].for_max_turbo_block_size)
     )
  {
    /* Either Turbo not supported or Conv supports higher rate */
    return CAI_CONVOLUTIONAL_CODING;
  }
  else
  {
    /* Turbo supported and its rate is higher than or equal to Conv */
    return CAI_TURBO_CODING;
  }

}
#endif /* FEATURE_IS2000_SCH */

/*===========================================================================

FUNCTION mccap_is_td_mode_supported

DESCRIPTION
   This function determines if the specified td_mode is supported
   by the MS.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if the specified TD mode is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccap_is_td_mode_supported( byte td_mode)
{
  boolean td_mode_supported;

  switch (td_mode)
  {
    case CAI_TD_MODE_OTD:
#ifdef FEATURE_DYNAMIC_P_REV
      if (cdma.mob_cai_rev < P_REV_IS2000)
      {
        td_mode_supported = FALSE;
      } else
#endif /*FEATURE_DYNAMIC_P_REV*/
#ifdef FEATURE_CDMA_RX_DIVERSITY
      /* TD is not supported when Receive Diversity is enabled */
      if (srch_get_rx_div_cap()) 
      {
        td_mode_supported = FALSE;
      } else
#endif
      {
        td_mode_supported = (boolean) SYS_CAP(MCCAP_DF_OTD);
      }
      break;  

#ifdef FEATURE_IS2000_REL_A
    case CAI_TD_MODE_STS:
#ifdef FEATURE_DYNAMIC_P_REV
      if (cdma.mob_cai_rev < P_REV_IS2000_REL_A)
      {
        td_mode_supported = FALSE;
      } else
#endif /*FEATURE_DYNAMIC_P_REV*/
#ifdef FEATURE_CDMA_RX_DIVERSITY
      /* TD is not supported when Receive Diversity is enabled */
      if (srch_get_rx_div_cap()) 
      {
        td_mode_supported = FALSE;
      } else
#endif
      {
        td_mode_supported = (boolean) SYS_CAP(MCCAP_DF_STS);
      }
      break; 
#endif /* FEATURE_IS2000_REL_A */

  default:
      td_mode_supported = FALSE;
      break;
  }
  return td_mode_supported;
}

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION mccap_get_encryption_cap_info

DESCRIPTION
   This function returns the Encryption Capability of the MS in the pointer passed.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccap_get_encryption_cap_info ( caii_enc_cap_type *cec_ptr )
{

  /* get the capability information from the system_capability records */
  cec_ptr->sig_enc_sup = system_capability.signaling_encryption_support;
  cec_ptr->ui_enc_sup = system_capability.user_encryption_support;

  /* But if the extended signaling encryption is turned off through DF, 
   * accordingly the AES signaling encryption bit needs to be cleared.
   */
  if ( ! SYS_CAP( MCCAP_DF_AES_SIG ) )
  {
    cec_ptr->sig_enc_sup &= ( ~ ( (byte) CAI_ENC_SUP_REA ) );
  }

  /* Similarily, if the user information encryption (for now we call it
   * voice encryption because we are not implementing user information
   * AES for other services) is turned off through DF, the AES 
   * capability bit will also be cleared.
   */
  if ( ! SYS_CAP( MCCAP_DF_AES_UI_VOICE ) )
  {
    cec_ptr->ui_enc_sup &= ( ~ ( (byte) CAI_UI_ENC_SUP_REA ) );
  }
} /* mccap_get_encryption_cap_info */

/*===========================================================================

FUNCTION mccap_is_flex_rates_supported

DESCRIPTION
   This function determines if flex rates is supported by the MS.
   Once we start supporting flex rates on only certain channels, this
   function will have to take channel as a input.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if flex rates is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_flex_rates_supported( void )
{
  return system_capability.flex_supported;
}

/*===========================================================================

FUNCTION mccap_is_sch_var_rates_supported

DESCRIPTION
   This function determines if var rates is supported on the SCH.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if var rates is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_sch_var_rates_supported( void )
{
  return system_capability.var_supported;
}

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION mccap_is_csch_td_mode_supported

DESCRIPTION
   This function determines if the specified td_mode is supported on common
   channel by the MS.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if the specified TD mode is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccap_is_csch_td_mode_supported(byte td_mode)
{
  boolean csch_td_mode_supported;
  
  switch (td_mode)
  {
    case CAI_TD_MODE_OTD:
#ifdef FEATURE_CDMA_RX_DIVERSITY
      /* TD is not supported when Receive Diversity is enabled */
      if (srch_get_rx_div_cap()) 
      {
        csch_td_mode_supported = FALSE;
      } else
#endif
      {
        csch_td_mode_supported = system_capability.csch_otd_supported;
      }
      break; 

    case CAI_TD_MODE_STS:
#ifdef FEATURE_CDMA_RX_DIVERSITY
     /* TD is not supported when Receive Diversity is enabled */
      if (srch_get_rx_div_cap()) 
      {
        csch_td_mode_supported = FALSE;
      } else
#endif
      {
        csch_td_mode_supported = system_capability.csch_sts_supported;
      }
      break; 

    default:
      csch_td_mode_supported = FALSE;
      break; 
  }

  return csch_td_mode_supported;

} /* mccap_is_csch_td_mode_supported */

/*===========================================================================

FUNCTION mccap_is_csch_f_ccch_frame_size_supported

DESCRIPTION
   This function determines if the specified FCCCH frame size is supported
   by the MS.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if the specified TD mode is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccap_is_csch_f_ccch_frame_size_supported
(
  cai_fccch_frame_size_type frame_size
)
{
  boolean csch_f_ccch_frame_size_supported;

  switch (frame_size)
  {
    case CAI_FCCCH_20MS_FRAME:
      csch_f_ccch_frame_size_supported = TRUE;
      break; 

    case CAI_FCCCH_10MS_FRAME:
      csch_f_ccch_frame_size_supported = system_capability.f_ccch_10ms_frame_supported;
      break; 

    case CAI_FCCCH_5MS_FRAME:
      csch_f_ccch_frame_size_supported = system_capability.f_ccch_5ms_frame_supported;
      break; 

    default:
      csch_f_ccch_frame_size_supported = FALSE;
      break; 
  }
  return csch_f_ccch_frame_size_supported;

} /* mccap_is_f_ccch_frame_size_supported */
#endif /* FEATURE_IS2000_REL_A_CC */

#endif //FEATURE_IS2000_REL_A

#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION mccap_is_msg_integrity_supported

DESCRIPTION
   This function determines if message integrity is supported by the MS.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if message integrity  is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccap_is_msg_integrity_supported()
{
  return system_capability.msg_integrity_supported;
} /* mccap_is_msg_integrity_supported */

/*===========================================================================

FUNCTION mccap_get_sig_integrity_sup_info

DESCRIPTION
   This function returns the Signaling Message Integrity support information
   of the MS in the pointer passed.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccap_get_sig_integrity_sup_info 
(
  caii_sig_msg_int_info_type *cec_ptr
)
{
  cec_ptr->sig_integrity_sup = system_capability.sig_integrity_sup;

} /* mccap_get_sig_integrity_cap_info */
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

FUNCTION mccap_is_simult_fch_dcch_supported

DESCRIPTION
   This function determines if FCH & DCCH are supported concurrently.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if concurrent FCH & DCCH is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_simult_fch_dcch_supported( void )
{
  return system_capability.simultaneous_fch_dcch;
}


#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION mccap_reach_rate_supported

DESCRIPTION
   Determines whether the MS supports the REACH rate_word.

DEPENDENCIES
  None.

RETURN VALUE
   True if supported.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_reach_rate_supported
(
  txc_each_rate_type rate
)
{
  if (
#ifdef FEATURE_IS2000_REL_A_REACH_10MS
    (rate == TXC_EACH_RATE_19200_10_V) ||
    (rate == TXC_EACH_RATE_38400_10_V) ||
#endif
#ifdef FEATURE_IS2000_REL_A_REACH_5MS
    (rate == TXC_EACH_RATE_38400_5_V) ||
#endif
    (rate == TXC_EACH_RATE_9600_20_V)
    || (rate == TXC_EACH_RATE_19200_20_V)
    || (rate == TXC_EACH_RATE_38400_20_V)
  )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif // FEATURE_IS2000_REL_A_CC

/*===========================================================================

FUNCTION mccap_is_f_sch_32x_supported

DESCRIPTION
   This function determines whether the mobile supports 32X Fwd SCH data rate

DEPENDENCIES
  None.

RETURN VALUE
   True if it supports

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_f_sch_32x_supported(void)
{
#ifdef FEATURE_IS2000_REL_A
  if (!SYS_CAP(MCCAP_DF_FOR_SCH))
  {
    return FALSE;
  }
  return (system_capability.f_sch_fields.for_sch_recs[0].for_max_rate == 
          CAI_SCH_RATE_RS1_32X_RS2_18X);  /*lint !e641 */
#else
  return FALSE;
#endif
} /* mccap_is_f_sch_32x_supported */

/*===========================================================================

FUNCTION mccap_is_r_sch_32x_supported

DESCRIPTION
   This function determines whether the mobile supports 32X Rev SCH data rate

DEPENDENCIES
  None.

RETURN VALUE
   True if it supports

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_r_sch_32x_supported(void)
{
#ifdef FEATURE_IS2000_REL_A
  if (!SYS_CAP(MCCAP_DF_REV_SCH))
  {
    return FALSE;
  }
  return (system_capability.r_sch_fields.rev_sch_recs[0].rev_max_rate == 
          CAI_SCH_RATE_RS1_32X_RS2_18X);  /*lint !e641 */
#else
  return FALSE;
#endif
} /* mccap_is_r_sch_32x_supported */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION mccap_f_sch_max_rate

DESCRIPTION
  This function returns the forward SCH max rate.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns the forward SCH max rate. 

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccap_f_sch_max_rate(void)
{
  if (!SYS_CAP(MCCAP_DF_FOR_SCH))
  {
    return 0;
  }
  return system_capability.f_sch_fields.for_sch_recs[0].for_max_rate;
} /* mccap_f_sch_max_rate */

/*===========================================================================

FUNCTION mccap_r_sch_max_rate

DESCRIPTION
   This function returns the reverse SCH max rate.

DEPENDENCIES
  None.

RETURN VALUE
   This function returns the reverse SCH max rate.
   
SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccap_r_sch_max_rate(void)
{
  if (!SYS_CAP(MCCAP_DF_REV_SCH))
  {
    return 0;
  }
  return system_capability.r_sch_fields.rev_sch_recs[0].rev_max_rate;
} /* mccap_r_sch_max_rate */
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_SCH
#if defined (FEATURE_PASSPORT_MODE) || defined (FEATURE_IS2000_DYNAMIC_FEATURE)
/*===========================================================================

FUNCTION mccap_set_sch_sys_cap

DESCRIPTION
  This function was called by DS module for JCDMA Max Rate feature, and the
  Dynamic Feature module where the fwd and rev max rate are configurable.
  
  The JCDMA Max Rate feature requires the system capabilities reported by
  the MS to be adjusted to reflect limitations imposed by system level
  requirements rather than the actual capability of the MS software and
  hardware.

  For JCDMA Max Rate, the DS module calls this function to indicate if 
  F-SCH/R-SCH are supported and if so, the maximum rate (1x,2x,4x,8x,16x)
  on each channel and these limits are captured in the system_capability 
  structure. INTLOCK & INTFREE ensure that this update completes (in DS 
  task context) before MC accesses the data structure.

  For JCDMA Max Rate, this function assumes that IS-2000 Release A is 
  not supported and that RS2 SCH is supported, but RS2 16x is not supported.
  
  For Dynamic Feature, this function is called by DF module during powerup,
  when it reads the fwd and rev SCH max rate from NV. The maximum rate can
  be up to 16x or 32x, and RS2 16x can be supported.

DEPENDENCIES
  For JCDMA Max Rate, DS Module must update configuration in SNM to service
  negotiation and capability reporting of service options.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccap_set_sch_sys_cap
(
  boolean fwd_sch_supported,
  boolean rev_sch_supported,
  cai_sch_rate_type max_fwd_sch_rate,
  cai_sch_rate_type max_rev_sch_rate
)
{
  int i, j;
  system_capability_subset *ptr;
  word i_save;

  #ifdef FEATURE_PASSPORT_MODE
  if(mcc_passport_mode_is_jcdma())
  {  
    if (fwd_sch_supported && (max_fwd_sch_rate > CAI_SCH_RATE_RS1_16X_RS2_16X))
    {
       MSG_HIGH("JCDMA Max Rate Feature not supporting > fwd 16x",0,0,0);
       max_fwd_sch_rate = CAI_SCH_RATE_RS1_16X_RS2_16X;
    }
    if (rev_sch_supported && (max_rev_sch_rate > CAI_SCH_RATE_RS1_16X_RS2_16X))
    {
       MSG_HIGH("JCDMA Max Rate Feature not supporting > rev 16x",0,0,0);
       max_rev_sch_rate = CAI_SCH_RATE_RS1_16X_RS2_16X;
    }
  }
  else
  #endif
  {
    if (fwd_sch_supported && (max_fwd_sch_rate > CAI_SCH_RATE_RS1_32X_RS2_18X))
    {
       MSG_HIGH("Max Rate not supporting > fwd 32x",0,0,0);
       max_fwd_sch_rate = CAI_SCH_RATE_RS1_32X_RS2_18X;
    }
    if (rev_sch_supported && (max_rev_sch_rate > CAI_SCH_RATE_RS1_32X_RS2_18X))
    {
       MSG_HIGH("Max Rate not supporting > rev 32x",0,0,0);
       max_rev_sch_rate = CAI_SCH_RATE_RS1_32X_RS2_18X;
    }  
  }
  INTLOCK_SAV(i_save);

  /* Adjust F-SCH capabilities */
  system_capability.cap_value[MCCAP_DF_FOR_SCH] = fwd_sch_supported;

  if (!SYS_CAP(MCCAP_DF_FOR_SCH))
  {
    /* No F-SCH, so no fwd sch mux options supported */
    system_capability.num_fwd_sch_mux = 0;
  }
  else
  {
	/* Check the max_fwd_sch_rate range cheeck to fix Klocwork error */
	if (max_fwd_sch_rate > CAI_SCH_RATE_RS1_32X_RS2_18X)
	{
		MSG_HIGH("Max Rate not supporting > fwd 32x",0,0,0);
         max_fwd_sch_rate = CAI_SCH_RATE_RS1_32X_RS2_18X;
    }

    ptr = &(sys_cap_subset[max_fwd_sch_rate]);

    #ifdef FEATURE_IS2000_REL_A
    /* Adjust the for_max_rate */
    system_capability.f_sch_fields.for_sch_recs[0].for_max_rate
      = max_fwd_sch_rate;   //lint !e641 !e734 : no loss of precision  
    #endif
    
    /* Adjust max mux options supported */
    if (SYS_CAP(MCCAP_DF_MUXPDU5))
    {
      system_capability.num_fwd_sch_mux = ptr->num_sch_mux + 1;
      system_capability.fwd_sch_mux[0]  = CAI_MUX_VARIABLE;
      j = 1;
    }
    else
    {
      system_capability.num_fwd_sch_mux = ptr->num_sch_mux;
      j = 0;
    }

    for (i=0; i<ptr->num_sch_mux; i++, j++)
    {
      system_capability.fwd_sch_mux[j] = ptr->sch_mux[i];
    }

    /* Adjust Turbo and Conv block sizes for SCH 0 */
    if (system_capability.f_sch_fields.for_sch_recs[0].for_turbo_supported)
    {
      system_capability.f_sch_fields.for_sch_recs[0].for_max_turbo_block_size
        = ptr->max_turbo_block_size;
    }
    if (system_capability.f_sch_fields.for_sch_recs[0].for_conv_supported)
    {
      system_capability.f_sch_fields.for_sch_recs[0].for_max_conv_block_size
        = ptr->max_conv_block_size;
    }
  }

  /* Adjust R-SCH capabilities */
  system_capability.cap_value[MCCAP_DF_REV_SCH] = rev_sch_supported;

  if (!SYS_CAP(MCCAP_DF_REV_SCH))
  {
    /* No R-SCH, so no rev sch mux options supported */
    system_capability.num_rev_sch_mux = 0;
  }
  else
  {
	/* check the max_rev_sch_rate to fix the Klocwork error */
    if (max_rev_sch_rate > CAI_SCH_RATE_RS1_32X_RS2_18X)
    {
       MSG_HIGH("Max Rate not supporting > rev 32x",0,0,0);
       max_rev_sch_rate = CAI_SCH_RATE_RS1_32X_RS2_18X;
    } 
	
    ptr = &(sys_cap_subset[max_rev_sch_rate]);

    #ifdef FEATURE_IS2000_REL_A
    /* Adjust the rev_max_rate */
    system_capability.r_sch_fields.rev_sch_recs[0].rev_max_rate
      = max_rev_sch_rate;   //lint !e641 !e734 : no loss of precision
    #endif

    /* Adjust max mux options supported */
    if (SYS_CAP(MCCAP_DF_MUXPDU5))
    {
      system_capability.num_rev_sch_mux = ptr->num_sch_mux + 1;
      system_capability.rev_sch_mux[0]  = CAI_MUX_VARIABLE;
      j = 1;
    }
    else
    {
      system_capability.num_rev_sch_mux = ptr->num_sch_mux;
      j = 0;
    }

    for (i=0; i<ptr->num_sch_mux; i++, j++)
    {
      system_capability.rev_sch_mux[j] = ptr->sch_mux[i];
    }

    /* Adjust Turbo and Conv block sizes for SCH 0 */
    if (system_capability.r_sch_fields.rev_sch_recs[0].rev_turbo_supported)
    {
      system_capability.r_sch_fields.rev_sch_recs[0].rev_max_turbo_block_size =
        ptr->max_turbo_block_size;
    }
    if (system_capability.r_sch_fields.rev_sch_recs[0].rev_conv_supported)
    {
      system_capability.r_sch_fields.rev_sch_recs[0].rev_max_conv_block_size =
        ptr->max_conv_block_size;
    }
  }
  INTFREE_SAV(i_save);
}
#endif // FEATURE_PASSPORT_MODE || FEATURE_IS2000_DYNAMIC_FEATURE
#endif /* FEATURE_IS2000_SCH */

#ifdef FEATURE_IS2000_DYNAMIC_FEATURE
/*===========================================================================

FUNCTION MCCAP_DF_SET_SYS_CAP

  This function updates the system capability with the dynamic feature item.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  system_capability variable will be updated.
  
===========================================================================*/
void mccap_df_set_sys_cap
(
  void
)
{
#ifdef FEATURE_IS2000_REL_A_SVD
  /* Simultaneous FCH and DCCH requires the static SVD feature enabled. */
  system_capability.simultaneous_fch_dcch = (boolean) SYS_CAP(MCCAP_DF_DCCH);
#else
  system_capability.simultaneous_fch_dcch = FALSE;
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_SCH
  /* Set the for_max_rate and rev_max_rate, Update the MUX     */
  /* option, max TURBO and CONV block size.                    */
  mccap_set_sch_sys_cap((boolean) SYS_CAP(MCCAP_DF_FOR_SCH),
                        (boolean) SYS_CAP(MCCAP_DF_REV_SCH),
                        (cai_sch_rate_type) SYS_CAP(MCCAP_DF_FOR_MAX_RATE),
                        (cai_sch_rate_type) SYS_CAP(MCCAP_DF_REV_MAX_RATE));
#endif /* FEATURE_IS2000_SCH */

} /* mccap_df_set_sys_cap */
#endif /* FEATURE_IS2000_DYNAMIC_FEATURE */

/*===========================================================================
FUNCTION MCCAP_ITEM_IS_SUPPORTED

DESCRIPTION
   Returns TRUE if the dynamic feature specified is supported

DEPENDENCIES
  None.

RETURN VALUE
   Returns TRUE if the dynamic feature specified is supported.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccap_item_is_supported(mccap_df_type df_type)
{
  return (boolean) SYS_CAP(df_type);
}

#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION mccap_is_for_pdch_supported

DESCRIPTION
   Returns TRUE if FOR_PDCH is supported else FALSE

DEPENDENCIES
  None.

RETURN VALUE
   Returns TRUE if FOR_PDCH is supported else FALSE

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccap_is_for_pdch_supported(void)
{

  return ( (P_REV_IS_9_OR_GREATER) ? system_capability.for_pdch_supported : FALSE );

} /* mccap_is_for_pdch_supported() */

/*===========================================================================

FUNCTION mccap_is_ext_ch_ind_supported

DESCRIPTION
   Determine if the input OTA EXT_CH_IND value is supported by the mobile.

DEPENDENCIES
  None.

RETURN VALUE
   Returns TRUE if the input EXT_CH_IND value is supported, else FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_ext_ch_ind_supported(byte ota_ext_ch_ind)
{

  if (ota_ext_ch_ind > CAI_EXT_CHIND_PFD_FD)  /*lint !e641 */
  {
#ifdef FEATURE_IS2000_REL_D 
#error code not present
#else
    return FALSE;
#endif /* FEATURE_IS2000_REL_D */
  }
  else
  {
    uint16 for_pdch_map_bit_len;

    /* ch_config_map_len is in 3 bit units where value 0 means 1 3-bit unit */
    for_pdch_map_bit_len = 3 * (system_capability.for_pdch_fields.ch_config_sup_map_len + 1);
  
    if (ota_ext_ch_ind > for_pdch_map_bit_len)
    {
      /* If we are not even representing whether we support this ota_ext_ch_ind 
         in our ch_config_sup_map, then we clearly don't support that value. 
         For example, if the map_bit_len is 3, which is all that is required to
         say if we support config F-PDCH 1 and F-PDCH 3, and we get an 
         ext_ch_ind of 5, then return FALSE. */
      return FALSE;
    } 

    /* If we get to here then, check in our ch_config_sup_map if that ota_ext_chind 
      is supported. */
    return ((system_capability.for_pdch_fields.ch_config_sup_map & 
            (word) (1 << (for_pdch_map_bit_len - ota_ext_ch_ind))) != 0);

    /* 
    ** Example - assume MS supports config F-PDCH 1, F-PDCH 2, F-PDCH 3 and 
    ** F-PDCH 4 (0x3C) and input is 5. 6 is the map_bit_len required to store
    ** the map.
    ** 0x3C & (1 << (6 - 5))
    ** 0x3C & (1 << 1)
    ** 0x3C & (2) = 0 ==> false
    */
  }

} /* mccap_is_ext_ch_ind_supported */

/*===========================================================================

FUNCTION mccap_is_simult_for_pdch_sch_supported

DESCRIPTION
   Determines if simultaneous F-PDCH and F-SCH are supported.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if simultaneous F-PDCH and F-SCH are supported else FALSE

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccap_is_simult_for_pdch_sch_supported(void)
{
  return system_capability.for_pdch_sch_supported;

} /* mccap_is_simult_for_pdch_sch_supported() */

#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#ifdef FEATURE_MEID_SUPPORT
/*===========================================================================

FUNCTION mccap_is_meid_supported

DESCRIPTION
   This function returns true if MEID is supported else false.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns true if MEID is supported else false

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_meid_supported( void )
{
  return((cdma.scm & CAI_SCM_MEID_CAPABLE_MASK) == CAI_SCM_MEID_CAPABLE_MASK);
}
#endif /* FEATURE_MEID_SUPPORT */


#ifdef FEATURE_IS2000_REL_A_AES
#error code not present
#endif /* FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_PASSPORT_MODE
/*===========================================================================

FUNCTION MCCAP_PM_INIT

  This function initializes the compile time options for JCDMA at runtime
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void mccap_pm_init( void )
{
  /* Initialize the variables based on the MS region. If REL_A_TD is turned 
     off, then the cssh_otd_supported and csch_sts_supported retain the 
     default value FALSE as initialized in the static table above. */
  #ifdef FEATURE_IS2000_REL_A_TD
  if(mcc_passport_mode_is_jcdma())
  {
    system_capability.csch_otd_supported = FALSE;
    system_capability.csch_sts_supported = FALSE;
  }
  else
  {
    system_capability.csch_otd_supported = TRUE;
    system_capability.csch_sts_supported = TRUE;
  }
  #endif

  /* 4th entry of the system capability subset array holds an array of SCH MUX entries. 
     The 2nd entry is dependent on the JCDMA feature being enabled or disabled. So 
     setting this accordingly for the passport mode. If this entry value is changed
     it needs corresponding change here in the array indicess. If feature R_SCH is
     not defined, then the value defined in the static table is retained. */
  #ifdef FEATURE_IS2000_SCH
  #ifdef FEATURE_DS_IS2000_RS2_16X_R_SCH
  if(mcc_passport_mode_is_jcdma())
  {
    sys_cap_subset[4].sch_mux[2] = CAI_MUX_8X_RS2_DOUBLE;
  }
  else
  {
     sys_cap_subset[4].sch_mux[2] = CAI_MUX_16X_RS2_DOUBLE;
  }

  #endif /* FEATURE_DS_IS2000_RS2_16X_R_SCH */
  #endif /* FEATURE_IS2000_SCH */

}

#endif /* FEATURE_PASSPORT_MODE */
/*lint +e655 */

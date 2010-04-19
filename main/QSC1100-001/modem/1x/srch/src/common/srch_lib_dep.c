/*====*====*====*====*====*====*====*========*====*====*====*====*====*====*==*

        S E A R C H   T A S K  --
        S E A R C H   L I B R A R Y   D E P E N D E N C I E S

GENERAL DESCRIPTION
  This file contains the feature dependencies for the search library.  
  This file must be updated to reflect the features used when building the
  searcher library.  A compiler error will occur if the currently defined 
  feature set differs from that used to build the searcher library.
  In addition this file must always be shipped together with the searcher 
  library.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

      Copyright (c) 2004-2009 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/src/common/srch_lib_dep.c#2 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/19/09   adw     Removed FEATURE_TCXOMGR_DIAG dependency.
10/31/08   adw     Removed srchzz_sm.c and srchzz_qpch_offtl dependency on 
                   FEATURE_SRCH_ROT_ONLY_1X_FULL_CTRL.
10/20/08   adw     Added FEATURE_SRCH_ROT_ONLY_1X_FULL_CTRL dependency checks.
08/28/08   adw     Cleaned up featurization and seperated feature dependencies.
08/18/08   adw     Added srch_rx featurization dependency checks for ULC.
08/13/08   vks     Added FEATURE_SRCH_HAS_NO_RTL and EATURE_SRCH_HAS_NO_IRM
                   for ULC
07/13/08   vks     Added FEATURE_SRCH_SINGLE_ANTENNA for ULC
06/20/08   mca     Merge from //depot
03/31/08   mca     Changed FEATURE_FULL_TIME_SHDR to FEATURE_FULL_TIME_SHDR_3
03/18/08   mca     Properly featurized FTS
03/18/08   mca     Added clock regime resource manager support
03/13/08   sst     Modify FTS version to be determined at runtime
10/08/07   aps     Featurized out IS95B_MDR requirements for QSC60X5
02/21/07   aps     Featurized out SCH requirements for QSC60X5
02/07/07   aps     Featurized out REL A requirements for QSC60X5
06/29/06   tjc     Mainline FEATURE_HHO_ENHANCEMENTS
08/03/05   ejv     Remove checks for FEATURE_GPS_MODE, FEATURE_GPSONE_PPM, and
                   FEATURE_PLCM_ENHANCEMENTS.
07/14/05   sfm     Mainlined FEATURE_1X_SRCH_USES_ROTATOR_AFC and
                   FEATURE_TCXOMGR
05/31/05   rng     Merged in from SHDR Sandbox.
05/24/05   ejv     Check for FEATURE_TCXOMGR and FEATURE_RELATIVE_DMOD_POS.
05/20/05   ejv     Make sure FEATURE_ACQ2 is on.
03/14/05   ejv     Add deps for new library members.
03/01/05   sst     Commented out check for FEATURE_ACQ2
11/12/04   ejv     Removed check for FEATURE_IS2000_REL_A_CC_BCSMS.
10/27/04   dd      Removed gpsOne feature checks.
10/15/04   ejv     Remove feature checks for QUASAR.
09/30/04   sfm     Removed FEATURE_FINGER_DRIVER
09/23/04   ejv     Remove FEATURE_BROADCAST_SMS
09/13/04   sst     Added !FEATURE_ACQ2
09/08/04   jcm     FEATURE_FINGER_DRIVER check now reports correct requirement
09/03/04   sfm     added FEATURE_FINGER_DRIVER
08/03/04   ejv     Removed IS-95B feature checks.
07/19/04   ejv     Added additional IS-95B checks.  Check for
                   FEATURE_BROADCAST_SMS.
07/11/04   ajn     Added FEATURE_SRM_CHIPX8
06/25/04   ejv     Reverse tense on AFC feature check.
06/25/04   jcm     Mainlined FEATURE_FACTORY_TESTMODE in search library
06/25/04   jcm     Mainlined FEATURE_HDR, SRCH4_AFLT_ENABLED
06/25/04   jcm     Removed Dependency on SRCH_microHpq1HPQResults,
                   FEATURE_SEARCH1 conditional on T_MSM6700
06/23/04   jcm     implementation, first cut

=============================================================================*/


/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "srchi.h"


/*===========================================================================

               SEARCHER LIBRARY FEATURE REQUIREMENTS

===========================================================================*/

#ifdef T_QUASAR
#error code not present
#else

/* Required by:
     all Files
*/
#if defined (T_QSC1100) || defined (T_QSC1110)
  #ifndef FEATURE_SRCH_SINGLE_ANTENNA
    #error Required by search library: FEATURE_SRCH_SINGLE_ANTENNA
  #endif
#endif

/* Required by:
     srchtc_triage.c
*/
#ifndef T_QSC60X5
#error code not present
#else /* T_QSC60X5 */
#ifdef FEATURE_IS95B_MDR
  /* uncomment before code release
  #error Unsupported by search library: FEATURE_IS95B_MDR
  */
#endif /* FEATURE_IS95B_MDR */
#endif /* !T_QSC60X5 */

/* Required by:
     srch_fing_driver.c
     srchdv.c
     srchtc.c
     srchtc_triage.c
*/
#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */

/* Required by:
     srch_fing_driver.c
     srchdv.c
     srchtc_triage.c
*/
#ifdef FEATURE_IS2000_REL_D_DV
#error code not present
#endif /* FEATURE_IS2000_REL_D_DV */

/* Required by:
     srchacq_sm.c
     srchtc.c
*/
#ifndef FEATURE_DH
  #error Required by search library: FEATURE_DH
#endif /* !FEATURE_DH */

/* Required by:
     srchacq_sm.c
     srchrdiv.c
     srchtc.c
*/
#ifndef FEATURE_FACTORY_TESTMODE
  #error Required by search library: FEATURE_FACTORY_TESTMODE
#endif /* !FEATURE_FACTORY_TESTMODE */

/* Required by:
     srch_sect.c
*/
#ifndef T_QSC60X5
#error code not present
#endif /* T_QSC60X5 */

/* Required by:
     l1m.c
     srch_sched_sm.c
     srch_sect.c
     srchzz_sm.c
     srchzz_tl_common.c
     srchzz_is2000_sm.c
     srchzz_qpch_sm.c
     srch_chan.c
     srch_idle_util.c
     srchidle_sm.c
     srchpc.c
     srchpcg.c
*/
#ifndef T_QSC60X5
#error code not present
#endif /* T_QSC60X5 */

/* Required by:
     srch_sect.c
     srch4drv.c
     srch4trans.c
*/
#ifndef T_QSC60X5
#error code not present
#endif /* T_QSC60X5 */

/* Required by:
     srchtc.c
*/
#ifndef T_QSC60X5
#error code not present
#endif /* T_QSC60X5 */

/* Required by:
     srchtc.c
*/
#ifndef T_QSC60X5
#error code not present
#endif /* T_QSC60X5 */

/* Required by:
     srchzz_ctl.c
     srch_sched_sm.c
*/
#ifdef T_MSM6700
  #ifdef FEATURE_SEARCH1
    #error Unsupported by search library: FEATURE_SEARCH1
  #endif /* FEATURE_SEARCH1 */
#else
  #ifndef FEATURE_SEARCH1
    #error Required by search library: FEATURE_SEARCH1
  #endif /* !FEATURE_SEARCH1 */
#endif /* T_MSM6700 */

/* Required by:
     srch4trans.c
     srch_rx_div.c
*/
#ifndef FEATURE_SRCH_SINGLE_ANTENNA
  #ifndef FEATURE_CDMA_RX_DIVERSITY
    #error Required by search library: FEATURE_CDMA_RX_DIVERSITY
  #endif /* !FEATURE_CDMA_RX_DIVERSITY */
#else
  #ifdef FEATURE_CDMA_RX_DIVERSITY
    #error Unsupported by search library: FEATURE_CDMA_RX_DIVERSITY
  #endif /* !FEATURE_CDMA_RX_DIVERSITY */
#endif /* FEATURE_SRCH_SINGLE_ANTENNA */

/* Required by:
     srchzz_ctl.c
     srchzz_tl_common.c
     srchzz_is2000_sm.c
     srchzz_qpch_ontl_sm.c
*/
#if !(defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
      defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)) && \
      defined(FEATURE_WAKEUP_TCXO_CLK)
  #error Required by search library:\
 !(defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) ||\
 defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)) &&\
 defined(FEATURE_WAKEUP_TCXO_CLK)
#endif /* !(defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
      defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)) && \
      defined(FEATURE_WAKEUP_TCXO_CLK) */

/* Required by:
     srchtri.c
     srch_fing_driver.c
     srch_triage.c
     srchzz_tl_common.c
     srchtc.c
     srchtc_triage.c
*/
#ifndef SRCH_ENHANCED_TRIAGE
  #error Required by search library: SRCH_ENHANCED_TRIAGE
#endif /* !SRCH_ENHANCED_TRIAGE */

/* Required by:
     srchzz_qpch_offtl_sm.c
     srchzz_tl_common.c
*/
#ifndef FEATURE_SRM_CHIPX8
  // REVISIT: #error Required by search library: FEATURE_SRM_CHIPX8
#endif /* FEATURE_SRM_CHIPX8 */

/* Required by:
     srch_genlog.c
     srchacq_sm.c
     srchaq_sm.c
     srchcd_sm.c
     srch_afc.c
     srch_sect.c
     srch_triage.c
 */
#ifndef FEATURE_ACQ2
 #error Required by search library:  FEATURE_ACQ2
#endif

/* Required by:
     srch_util.c
*/
#ifndef FEATURE_RELATIVE_DMOD_POS
 #error Required by search library:  FEATURE_RELATIVE_DMOD_POS
#endif

/* Required by:
     srch_rx.c
     srchcom_sm.c
     srchidle_sm.c
     srchzz_is2000_sm.c
     srchzz_is95a_sm.c
     srchzz_rtl_sm.c
     srchzz_tl_common.c
*/
#if defined(FEATURE_FULL_TIME_SHDR_3) && !defined(FEATURE_SRCH_IS_FTS_CAPABLE)
 #error Required by search library:  FEATURE_SRCH_IS_FTS_CAPABLE
#endif

/* Required by:
    srch_hw.h
    srch_rx_t.h
    srch_stm.h
    srch_rx.h
    srch_fing_driver.c
    srch_hw.c
    srch_mux.c
    srch_nv.c
    srch_rx.c
    srchcom_sm.c
    srchcom_sm.smf
    srchidle_sm.c
    srchidle_sm.smf
    srchzz_is2000_sm.c
    srchzz_is95a_sm.c
    srchzz_tl_common.c
    srchcd_sm.c
    srchna_sm.c
    srchsl_sm.c
    srchtt_sm.c
    srchtc_sm.c
*/
#ifdef FEATURE_SRCH_SINGLE_ANTENNA
  #ifndef FEATURE_SRCH_HAS_NO_TRM
    #error Unsupported by search library w/ single ant: FEATURE_SRCH_HAS_NO_TRM
  #endif /* FEATURE_SRCH_HAS_NO_TRM */
#endif /* FEATURE_SRCH_SINGLE_ANTENNA */

#ifdef FEATURE_SRCH_HAS_NO_TRM
  #ifdef FEATURE_SOOS
    #error Unsupported by search library w/o TRM: FEATURE_SOOS
  #endif /* FEATURE_SOOS */
  #ifdef FEATURE_SRCH_IS_FTS_CAPABLE /* FTS 3 */
    #error Unsupported by search library w/o TRM: FEATURE_SRCH_IS_FTS_CAPABLE
  #endif /* FEATURE_SRCH_IS_FTS_CAPABLE */
  #ifdef FEATURE_SRCH_RTL
    #error Unsupported by search library w/o TRM: FEATURE_SRCH_HAS_NO_RTL
  #endif /* !FEATURE_SRCH_RTL */
#endif /* FEATURE_SRCH_HAS_NO_TRM */

/* Required by:
     srch_cgps.c
     srch_util.c
     srch_diag.c
     srch_sched_sm.c
     srch_idle_ctl.c
     srch_idle_sm.c
     srchzz_sm.c
     srchzz_rtl_sm.c
     srchzz_tl_common.c
     srchtc_sm.c
*/
#if defined(FEATURE_CGPS_IF)
  #if ( defined (FEATURE_SRCH_HAS_NO_IRM) 
    #error Unsupported by search library:  FEATURE_SRCH_HAS_NO_IRM
  #endif

  #if ( defined (FEATURE_SRCH_HAS_NO_RTL) 
    #error Unsupported by search library:  FEATURE_SRCH_HAS_NO_RTL
  #endif
#endif

/* Required by:
     srch_diag.c
     srchidle_ctl.c
     srchzz_sm.c
     srchzz_rtl_sm.c
     srchzz_tl_common.c
*/
#if ( ( !defined (FEATURE_SRCH_HAS_NO_RTL)) && defined (FEATURE_SRCH_HAS_NO_IRM) )
  #error Unsupported by search library:  FEATURE_SRCH_HAS_NO_IRM
#endif

/* Required by:
     srch_afc_sm.c
*/
#if ( defined( FEATURE_SRCH_ROT_ONLY_1X_FULL_CTRL ) &&               \
     !defined( FEATURE_XO_CTL_1X_ONLY ) )
  /* Warning: This could fail if the target is 1x + HDR and no GPS */
  #error Unsupported by search library w/ rot only: !FEATURE_XO_CTL_1X_ONLY
#endif

#if ( !defined( FEATURE_SRCH_ROT_ONLY_1X_FULL_CTRL ) &&              \
       defined( FEATURE_XO_CTL_1X_ONLY ) )
  /* Warning: This could fail if the target is 1x + HDR and no GPS */
  #error Unsupported by search library w/o rot only: FEATURE_XO_CTL_1X_ONLY
#endif

#endif /* T_QUASAR */

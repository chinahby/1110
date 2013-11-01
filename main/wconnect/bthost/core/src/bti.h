#ifndef _BTI_H
#define _BTI_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B L U E T O O T H    I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth module.

Copyright (c) 2000-2009 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/bti.h#3 $
  $DateTime: 2009/07/31 11:25:37 $
  $Author: c_bhatg $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-07-31   gb  Added support for FEATURE_IBT_DYNAMIC.
  2009-06-23   ns  Removed unnecessary featurization for bt_event_mask_type
  2008-11-05   pg  Generic handling for HCI Vendor Specific cmd group.
  2008-10-17   co  Removed #define BT_SSR_LPC_DEBUG.
  2008-09-15   pg  Fixed Compiler warning.
  2008-08-30   pg  Added function prototype for bt_cmd_hc_rd_clk_offset()
  2008-08-25   gs  Fixed definition of BT_SYNC_DEF_ALL_PKT_TYPES
  2008-08-14   rb  Added handling to keep the background timer tick during
                   power collapse
  2008-06-09   co  Defined BT_SSR_LPC_DEBUG under FEATURE_BT_TEST_API if
                   FEATURE_BT_2_1 is defined.
  2008-05-16   vk  Moved BT_RC_MAX_TX_BYTES_PER_TICK from RC module.
  2008-05-08   rs  Added API for dsm data memory alignment
  2008-04-30   rb  Fix warning - Superfluous ',' at the end of enum
  2008-04-16   jn  Change min link supervision timeout to 5.12 secs.
  2008-04-16   jn  Fix EOL issue with file formatting.
  2008-03-13   jn  Created APIs for allowing/not allowing MSM VDD minimization.

 *
 *    #155      28 Feb 2008             VK
 * Added macro for Pause Encryption Feature.
 *
 *    #154      21 Feb 2008             VK
 * Added macro for Min Link Supervision Timeout.
 *
 *    #153      23 Jan 2008             HM
 * Added definition BT_CMD_MAX_RETRIES. Created macro to check if retry count has expired 
 *
 *    #152      14 Dec 2007             VK
 * Added proto type for bt_cmd_hc_wr_link_supervision_timeout.
 *
 *    #151      20 Nov 2007             PC
 * Fixed macro declaration BT_IN_HCI_MODE, if !FEATURE_FACTORY_TESTMODE
 *
 *    #150       07 Nov 2007            GS
 * Added HID profile host role support.
 *
 *    #149       01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *    #148       19 OCT 2007            DSN\SSK
 * Add HCI commands for Controller to Host flow control.
 * 
 *    #147       11 Sep 2007            BK
 * Added BT_DC_UPDATE_DRIVER_STATE macro to support QSOC Driver State state machine.  
 *
 *    #146       23 Aug 2007            PG
 * Added HCI read rssi cmd.
 *
 *    #145       24 AUG 2007            VK
 * Added Macro Enabling all packet types in "Accept Synchronous Connection
 * Request Command".
 *
 *    #144       24 AUG 2007            VK
 * Added Macro for Sniff LMP feature Bit.
 *
 *    #143       17 AUG 2007            BK
 * Use FEATURE_BT_SOC to undefine T_AMS_DISABLED for all SOC builds.    
 *
 *    #142       08 AUG 2007            BH
 * Removed BT_AMS_DISABLED define for 6800 builds.
 *
 *    #141       11 JUL 2007            VK
 * Added macros for feature mask bits and syncronous packet types.
 *
 *   #140        09 JUL 2007            PG
 * HCI Remote name req cancel and connection cancel support.
 *
 *    #139       04 JUN 2007            VK
 * Added proto type for bt_cmd_hc_rd_remote_version function.
 *
 *    #138       05 May 2007            DSN
 * Include a new header file to allow remoting of apis for SAP.
 *
 *    #137       30 MAY 2007            PG
 * Added featurization check for SOC based targets. 
 *
 *    #136       25 MAY 2007            PG
 * Added logic to avoid defining BT_AMS_DISABLED for 7200 so that scans are
 * not disabled during a connection. 
 *
 *    #135       25 MAY 2007            PG
 * Corrected HCI default for Inquiry scan interval. 
 *
 *    #134       11 MAY 2007            VK
 * Added proto type for bt_cmd_hc_rd_remote_feature function and enums for 
 * SCO packet type masks.
 *
 *    #133       10 May 2007            SSK
 * Added prototype definitions for bt_dc_power_off/on_soc APIs.
 *
 *    #132       04 May 2007            PG
 * Changed scan interval definitions. HCI default used for SOC targets.
 * For embedded controller, changed both inquiry and page interval to 1.28s.
 *
 *    #131       23 Apr 2007            SSK
 * Change parameter type in bt_cmd_rm_tx_data_acl() prototype from type
 * 'boolean'to type 'bt_hc_pkt_boundary_flag_type' for stronger type checking.
 *
 *    #130       20 Apr 2007            JN
 * Added bt_dc_sio_change_baud(). Declare bt_dc_sio_ioctl only for SoCs
 * (to be consistent with btdc.c).
 *
 *    #129       18 Apr 2007            JH
 * Added featurization check for 1.2 features.
 *
 *    #128       13 Apr 2007            DM
 * Added define for Default Sync Connection Packet type.
 *
 *    #127       12 Apr 2007            GS
 * Enable BT_WAKEUP_EDGE_TRIGGERED for 6800.
 *
 *    #126       12 Apr 2007            SSK
 * Added prototype definitions for newly added HCI commands.
 *
 *    #125       09 Apr 2007            PG
 * Revised defines for scan intervals used by SOC based targets.
 *
 *    #124       09 Apr 2007            BH
 * Comment changes for bt_soc_enable_flow and bt_soc_disable_flow.
 *
 *    #123       06 Apr 2007            PG
 * Added new HCI commands for writing and reading scan type.
 *
 *    #122       02 Apr 2007            DSN
 * Fix to handle back-to-back HCI and SOC rdm port mappping commands.
 *
 *    #121       14 Mar 2007            JN
 * Changed name of bt_soc_shutdown to bt_soc_shutdown_cleanup due to conflict
 * in function name in btsoc.c
 *
 *    #120       06 Mar 2007            JN
 * Externed functions bt_soc_enable_flow and bt_soc_disable_flow. 
 *
 *    #119       01 Mar 2007            DSN
 * Add support for HCI Read Buffer Size command.
 *
 *    #118       08 Feb 2007            DM
 * Enable BT_SWDEV_ESCO for SoC
 *
 *    #117       01 Feb 2007            DM
 * Added inbound flow status flag to bt_dc.
 *
 *    #116       31 Jan 2007            ACG
 * Added a delay between receipt of yms and voting to sleep.
 *
 *    #113       10 Jan 2007            ACG
 * Keep BT_SLEEP_DEBUG for 7500 only for now. Suppress unnecessary messages on other
 * other targets.
 *
 *    #112        8 Jan 2007            ACG
 * Following features were added for the SOC solution:
 * - vote for SIO sleep/wakeup based on BT activity
 * - enable/disable driver
 * - radio on/off
 * 
 *    #111        22 Dec 2006           DSN
 * Throughput optimization for BT2.0+ EDR based SOC solutions.
 *
 *    #110        03 Dec 2006           JTL
 * Added bt_cmd_rc_cancel_connect().
 *
 *    #109        16 Nov 2006           DM
 * Changed bt_cmd_hc_tx_acl_data() to add local_cid and frame_handle,
 * Required for AV-Sync.
 *
 *    #108        10 Nov 2006           JH
 * Disable eSCO.
 *
 *    #107        08 Nov 2006           DM
 * Added interface definition for bt_cmd_l2_get_acl_tx_flow_status
 *
 *    #106        05 Nov 2006           DM
 * Enabling BT_TEST_PYLD_FAST_TX for SOC.
 *
 *    #105        31 Oct  2006          VK
 * Fixing compilation conflict of bt_rm_atzrq_type enum.
 *
 *    #104       26 Oct 2006            VK
 *   Added one parameter to function bt_cmd_rm_enforce_security_l2cap_psm declaration.
 *
 *    #103       24 Oct 2006            PG
 * Fixed comments.
 *
 *    #102       24 Oct 2006            PG
 * Changed bt_dc_sio_ioctl API to be generic.
 *
 *    #101       20 Oct 2006            SSK
 * External SOC suppport: H4DS and BCCMD modules.

 *    #100       19 Oct 2006            PN
 * Changed BREW SPP events dispatching mechanism.
 *
 *    #99        17 Oct 2006            PG
 * Added scan constants for use with SOC.
 *
 *    #98        04 Oct 2006            JTL
 * Disable AMS for 6500 GSM enabled builds.
 *
 *    #97        19 Sept 2006           JH
 * Removed FEATURE_BT_AFH_PER and FEATURE_BT_AFH_RSSI featurizations. 
 * RSSI-based AFH algorithm is now enabled on all 1.2 targets.
 * Removed code for PER-based AFH algorithm.
 *
 *    #96        12 Sep 2006            JH
 * Enable RSSI-based AFH algorithm on 6550.
 *
 *    #95        31 Aug 2006            JH
 * Enable RSSI-based AFH algorithm on 6280.
 *
 *    #94        23 Aug 2006            DSN
 * Fixed compilation errors on SOC solution.
 *
 *    #93        14 Aug 2006            DM
 * Remove AVSYNC feature (moved to custqbt.h)
 *
 *    #92        13 Aug 2006            JH
 * Implemented RSSI-based AFH algorithm.
 * 
 *    #91        26 Jul 2006            DM
 * Enable AV-Sync on 1.2 Targets
 * Added frame_handle to bt_cmd_l2_write
 *
 *    #90        19 Jul 2006            JH
 * Added missing edit history for a previous check-in.
 *
 *    #89        19 Jul 2006            JH
 * Enable eSCO on all BT 1.2 targets (regardless of APIT setting).
 *
 *    #88        11 Jul 2006            JH
 * Enable eSCO on 6280 APIT builds.
 *
 *    #87        27 Jun 2006            DSN
 * Added support for HostController SoC solution.
 *
 *    #86        25 Jun 2006            DSN
 * Removed RC_2_SIO related references that are no longer used.
 * 
 *    #85        02 Jun 2006            JH
 * Enable eSCO on 6800 APIT builds.
 *
 *    #84        02 Jun 2006            JH
 * Enable eSCO only on 6550 APIT builds.
 *
 *    #83        01 Jun 2006            JTL
 * Add CID to L2Cap Mode 2 security.
 *
 *    #82        18 May 2006            JH
 * Added definition of FEATURE_BT_INTERLACED_SCAN.
 *
 *    #81        26 Apr 2006            JH
 * Define BT_SWDEV_ESCO only when FEATURE_BT_TEST_API is defined.
 *
 *    #80        21 Apr 2006            GS
 * Exclude 6800 target in previous check-in for BT_WAKEUP_EDGE_TRIGGERED feature.
 *
 *    #79        10 Apr 2006            JH
 * Defines BT_WAKEUP_EDGE_TRIGGERED for 6550 L4 builds.
 * 
 *    #78        5  Apr 2006            DP
 * Added Extended Service Security.
 *
 *    #77        14 Mar 2006            JH
 * Modified to support updated bt_cmd_rm_connect_sco() command.
 *
 *    #76        21 Feb 2006            JH
 * Added definition of BT_SWDEV_ESCO.
 *
 *    #75        24 Jan 2006            GS
 * Re-enabling AMS in Jaguar GSM builds.
 *
 *    #74        20 Jan 2006            JTL
 * Added new L2 command to disconnect ACL immediately without waiting
 * for a timeout.
 *
 *    #73        20 Dec 2005            
 * Added image swap support for 6550 multi-mode builds.
 *
 *    #72        02 Dec 2005            DSN
 * Added featurization for MSM7500 bringup.
 *
 *    #71        30 Nov 2005            JTL
 * Added L2 command to retreive number of L2 connections per
 * BD Addr.
 *
 *    #70        9  Nov 2005            DP
 * Reverted default page and inquiry scan windows to 11.25ms when
 * SCO is active.
 *
 *    #69        28 Oct 2005            PG
 * Added BT_AMS_DEBUG
 *
 *    #68        27 Oct 2005            JH
 * Enable AMS for 6500 1x builds.
 *
 *    #67        17 Oct 2005            PN
 * Disabled AMS for 6500.
 *
 *    #66        14 Oct 2005            JH
 * Added definition of BT_1_2_PT_SCO_M and BT_1_2_PT_ESCO_M.
 *
 *    #65        06 Oct 2005            JH
 * Define alternate DEFAULT_BT_NAME for ODM.
 *
 *    #64        04 Oct 2005            RY
 * Enabled AMS for 6500.
 *
 *    #63        27 Sep 2005            DP
 * Define page/inquiry scan windows when a SCO link is present.
 *
 *    #62        15 Sep 2005            RY
 * Enabled AMS for 6550 (and 6800).
 *
 *    #61        12 Sep 2005            JTL
 * Improving DSM allocation.
 *
 *    #60        30 Aug 2005            JTL
 * Enabling AMS for 6275
 *
 *    #59        22 Aug 2005            JH
 * Disabled AMS for Wolf 5.
 * Added indentation for #defines.
 *
 *    #58        03 Aug 2005            GS
 * Mainlined mDSP reset handling functionality for UMTS targets.
 *
 *    #57        25 Jul 2005            GS
 * Expanded BT_IN_HCI_MODE macro to include HCI over FTM/DIAG
 *
 *    #56        22 Jul 2005            JH
 * Define FEATURE_BT_AFH_DEBUG and FEATURE_BT_ESCO_DEBUG if FEATURE_BT_TEST_API 
 *  is defined
 *
 *    #55        29 Jun 2005            RY
 * Defined BT_NEW_SRCHR_CONFIG
 *
 *    #54        27 Jun 2005            JH
 * Added bt_cmd_hc_setup_sync_conn() and the conditional compilation flags:
 *  FEATURE_BT_AFH_DEBUG and FEATURE_BT_ESCO_DEBUG.
 *
 *    #53        24 Jun 2005            PN
 * Added bt_ec_deregister_brew_spp().
 *
 *    #52        16 Jun 2005            DP
 * Updated packet typedefs.  Added BT_DEBUG_RSSI compilation controls.
 *
 *    #51        01 June 2005           GS
 * Enable Radio On/Off feature (on all targets).  Added BT_IN_HCI_MODE() macro.
 *
 *    #50        12 May 2005            RY
 * Enabled AMS for MSM6250 only.
 *
 *    #49        04 May 2005            RY
 * Enabled AMS.
 *
 *    #48        02 May 2005           JTL
 * Added command to disable the L2 initial timeout.
 *
 *    #47        22 Apr 2005            RY
 * Added checkin comment.
 *
 *    #46        22 Apr 2005            RY
 * Updated BT_MY_SCAN_PERIOD to 0x0002 to comply with BT 1.2 spec.
 *
 *    #45        07 Apr 2005            JH
 * Featurize definition of BT_SWDEV_RADIO_OFF using SIRIUS_PLATFORM.
 *
 *    #44        06 Apr 2005            JH
 * Adding support for disabling BT radio, featurized using BT_SWDEV_RADIO_OFF.
 *
 *    #43        23 Mar 2005           JTL
 * Adding A2DP top layer for DSMs.
 *
 *    #42        16 Mar 2005            DP
 * Fix Mode 2 security for client connections.  Remove comment regarding
 * obsolete SWDEV feature.
 *
 *    #41        5  Mar 2005            JH
 * Added definition of BT_MY_SCAN_PERIOD and BT 1.2 HCI command prototypes.
 *
 *    #40       04  Mar 2005            JTL
 * Added more profile queues.
 *
 *    #39        8  Feb 2005            GS
 * Added BT_QDSP_SWAPPING_OUT macro.
 *
 *    #38        5  Nov 2004            PN
 * Added support for audio debug.
 *
 *    #37        25 Oct 2004            GS
 * Added EFS param toggle_seqn_bit.
 *
 *    #36        24 Sep 2004            DP
 * Reworked AMS compilation controls.
 *
 *    #34        02 Sep 2004            JTL
 * Adding event queues for BREW SPP event handling.
 *
 *    #33        25 Aug 2004            DP
 * Removed featurization of BT HF.
 *
 *    #32        12 Aug 2004            JH
 * Added TX continuous frequency HCI command
 *
 *    #31        6  Aug 2004            JTL
 * Adding support for event queues for L2 and RM glue code.
 *
 *    #29        7  Jul 2004            JH
 * Added BT 1.2 eSCO support
 *
 *    #28        1  Jun 2004            DP
 * Implemented Mode 2 security.  Improved sleep debug information.
 *
 *    #27        30 Mar 2004            DP
 * EMS support added for 6100C, 6250, 6300, and 6500.
 *
 *    #26        26 Mar 2004            DP
 * Added sleep data declarations.
 *
 *    #25        25 Feb 2004            JH
 * Defeaturized BT_VSAI_PF_MAX_EV_Q_DEPTH to avoid unnecessary dependency
 *
 *    #24        23 Feb 2004            JH
 * Added support for external profiles
 *
 *    #23        29 Jan 2004            DP
 * Fixed EFS params initializations.
 *
 *    #22        14 Jan 2004            DP
 * Added support for limited discoverable scanning.
 *
 *    #21        16 Dec 2003            RY
 * Added comments.
 *
 *    #20        16 Dec 2003            RY
 * Simplified target-based BT feature definitions.
 *
 *    #19        14 Nov 2003            DP
 * Page/Inquiry scan intervals adjusted when only doing one type
 * of scan to reduce power consumption.
 *
 *    #17        4  Nov 2003            DP
 * General cleanup.
 *
 *    #16        26 Sep 2003            DP
 * Made RM API less exposed by moving some of it into here.
 *
 *    #15        06 Aug 2003            PN
 * Added bt_cmd_hc_wr_class_of_device().
 * 
 *    #14        22 Jul 2003            RY
 * 1. Relocated bt_pair_type here (from btevi.h).
 * 2. Updated bt_efs_params_type with additional LMP parameters and updated
 *    EFS version number.
 *
 *    #13        03 Jul 2003            RY
 * Added BT_TEST_SDP and leave it undefined.
 *
 *    #12        12 Jun 2003            PN
 * Added declaration of bt_ag_current_supported_features() which was moved
 * from bt.h.
 *
 *    Rev 1.57   25 Sep 2002 11:29:06   waynelee
 * externed QDSP poll timer
 * 
 *    Rev 1.56   04 Sep 2002 11:35:56   waynelee
 * Removed temp no mdsp svc code.
 * 
 *    Rev 1.55   09 Aug 2002 18:37:48   waynelee
 * Added MSM6100 support.
 * 

===========================================================================*/

/*===========================================================================

  Development symbols for Bluetooth module:

  BT_SWDEV_QXDM_LOG_PER:
    Enables periodic BT logs to QXDM.
    
  BT_SWDEV_QXDM_LOG_MSG:
    Enables BT log messages to QXDM.
    
  BT_QDSP_DEBUG:
    Places the Bluetooth driver into QDSP debug mode for firmware (QDSP)

  BT_SWDEV_QDSP_POLLING
    Define to use polling mechanism for checking if QDSP TX data has been
    sent and when QDSP RX'd data.

  BT_DSM_MEM_CHK
    Enables DSM memory checking.  Sets tracer value at each layer in
    the Bluetooth driver.

  BT_FLOW_DEBUG
    Enables flow control debug messages to be sent to QXDM.

  BT_SLEEP_DEBUG
    Prints messages related to BT sleep.

  BT_SCAN_DEBUG
    Prints message when we enter/exit page/inquiry scan.

  BT_DEBUG_DOFFSET
    Prints out debug information about DOFFSET.

  BT_TBD_SDP:
    Marks where SDP should be plugged into the AG/HS code.  TBD    
    
  BT_TBD:
    Marks where TBD code still exists (should also search for TBD)

  BT_AG_HF_EXTENSION
    Must be defined at top of bt.h.
    Enables AG to handle some Phone Access Profile features supported by
    some hands-free car-kits.
                     
  BT_AUDIO_DEBUG
    Enables logging of voice samples at various points on the PCM path.
    Must also define SND_TEST_CODE and FEATURE_VOC_PCM_INTERFACE_TEST in
    sndtest.h for this to work.

  BT_DEBUG_RSSI
    Output debug messages related to Receive Signal Strength Indicator.
  
===========================================================================*/

#if defined( FEATURE_BT_1_2_BASIC ) && !defined( FEATURE_BT_1_2 )
  #error Bad 1.2 definitions
#endif

#ifdef FEATURE_BT_1_2

  #ifdef FEATURE_BT_TEST_API      
#error code not present
  #endif

  #ifdef FEATURE_BT_SOC
    #define  BT_SWDEV_ESCO
  #else
    #undef   BT_SWDEV_ESCO
  #endif

  #if defined( BT_SWDEV_ESCO ) && defined( FEATURE_BT_TEST_API )
#error code not present
  #endif 

  #undef  FEATURE_BT_INTERLACED_SCAN
  #define BT_MY_LMP_VERSION  BT_LMP_VERSION_1_2
  #define BT_MY_HCI_VERSION  BT_HCI_VERSION_1_2
  #define BT_MY_SCAN_PERIOD  0x0002 /* The 2-bit Reserved field in the FHS
                                       shall be set to '10' */

  #if defined( FEATURE_BT_1_2_BASIC ) && \
      ( defined( BT_SWDEV_ESCO ) || defined( FEATURE_BT_INTERLACED_SCAN ) )
    #error Bad 1.2 definitions
  #endif    
                                         
#else /* not BT 1.2 */

  #define BT_MY_LMP_VERSION  BT_LMP_VERSION_1_1
  #define BT_MY_HCI_VERSION  BT_HCI_VERSION_1_1
  #define BT_MY_SCAN_PERIOD  0x0000

#endif /* FEATURE_BT_1_2 */

/* Definitions for BT2.1 - Lisbon */
#ifdef FEATURE_BT_2_1
#define BT_SWDEV_2_1_PB  /* Packet Boundary Flag */
#define BT_SWDEV_2_1_EIR /* Extended Inquiry Response */
#define BT_SWDEV_2_1_SSP /* Secure Simple Pairing */
#define BT_SWDEV_2_1_SSR /* Sniff Sub-Rating */
#define BT_SWDEV_2_1_EPR /* Encryption Pause & Resume */
#endif /* FEATURE_BT_2_1 */

#define BT_SWDEV_QXDM_LOG_PER  /*  Enable periodic BT logs to QXDM.  */
#define BT_SWDEV_QXDM_LOG_MSG  /*  Enable BT log messages to QXDM.   */

#define BT_NEW_SRCHR_CONFIG

#ifdef FEATURE_BT_SOC
#undef BT_SWDEV_BT_DISABLED_NV
#endif /* FEATURE_BT_SOC */

#undef  BT_FLOW_DEBUG
#undef  BT_DSM_MEM_CHK

#define BT_DEBUG_DOFFSET

#define BT_TEST_PYLD_FAST_TX

#undef BT_TEST_PYLD_FAST_RX

#undef BT_QDSP_DEBUG

#ifdef FEATURE_BT_SOC
#define BT_SLEEP_DEBUG
#else
#undef BT_SLEEP_DEBUG
#endif

#undef BT_SCAN_DEBUG

#undef BT_AMS_DEBUG

//#define BT_SWDEV_RC2SIO_L2CAP

#undef BT_TEST_SDP

#undef BT_AUDIO_DEBUG

#undef BT_DEBUG_RSSI

/*  Define target specific BT features.  */

#define BT_PARK_SUPPORTED

/*  Advanced Multi Slave (AMS) definitions.  */
#if (defined( T_MSM6100C ) || defined( T_MSM6300 ) || \
     defined( T_MSM6500 )  || defined( T_MSM6250 ) || \
     defined( T_MSM6275 ) )
  #define BT_AMS_SUPPORTED_IN_MDSP
#else
#error code not present
#endif

#ifdef WOLF_5
#error code not present
#else
  #if ( defined( T_MSM6250 ) || defined( T_MSM6275 ) || \
        defined( T_MSM6500 ) || defined( T_MSM6550 ) )
    #undef BT_AMS_DISABLED
  #else
#error code not present
  #endif
#endif /* WOLF_5 */

#if (!defined( BT_AMS_DISABLED ) && !defined( BT_AMS_SUPPORTED_IN_MDSP ))
  #error Bad AMS definitions
#endif

#define BT_SWDEV_RADIO_OFF

#if (defined( T_MSM6550 ) && defined( FEATURE_L4 ))
#error code not present
#endif

#if (defined( FEATURE_BT_SOC ))
  /* BT_AMS_DISABLED is meant only for Qualcomm embedded controller */
  /* This should not get defined for any of the SOC based targets   */
  #undef BT_AMS_DISABLED
#else
#if (defined( T_MSM6500 ) && defined( FEATURE_GSM ))
#error code not present
#endif /* T_MSM6500 && FEATURE_GSM */

#endif /* FEATURE_BT_SOC */

#if (defined( FEATURE_BT_SOC ) && defined( BT_AMS_DISABLED ))
  /* BT_AMS_DISABLED is meant only for Qualcomm embedded controller */
  /* This should not get defined for any of the SOC based targets   */
  #error Bad AMS definitions
#endif /* FEATURE_BT_SOC && BT_AMS_DISABLED */

#ifdef FEATURE_BT_SOC
#if (defined( FEATURE_BT_SOC1 ) && defined( FEATURE_BT_QSOC ))
#error code not present
#endif /* FEATURE_BT_SOC1 && FEATURE_BT_QSOC */
#endif /* FEATURE_BT_SOC */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"
#include "dsm.h"
#include "sio.h"
#include "btrcmsgi.h"
#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#endif
#include "btcomdef.h"


/*-------------------------------------------------------------------------*/
/*                        Constant Definitions                             */
/*-------------------------------------------------------------------------*/

#define BT_NAME_LEN             248
#define BT_EVENT_MASK_LEN       8
#define BT_DM1_PAYLOAD_MAX_LEN  17
#define BT_DH1_PAYLOAD_MAX_LEN  27
#define BT_DM3_PAYLOAD_MAX_LEN  121
#define BT_DH3_PAYLOAD_MAX_LEN  183
#define BT_DM5_PAYLOAD_MAX_LEN  224
#define BT_DH5_PAYLOAD_MAX_LEN  339

#define BT_2DH1_PAYLOAD_MAX_LEN  54
#define BT_3DH1_PAYLOAD_MAX_LEN  83
#define BT_2DH3_PAYLOAD_MAX_LEN  367
#define BT_3DH3_PAYLOAD_MAX_LEN  552
#define BT_2DH5_PAYLOAD_MAX_LEN 679
#define BT_3DH5_PAYLOAD_MAX_LEN 1021

#define BT_AUX1_PAYLOAD_MAX_LEN 29
#define BT_HV1_PAYLOAD_LEN      10
#define BT_HV2_PAYLOAD_LEN      20
#define BT_HV3_PAYLOAD_LEN      30
#ifdef CUST_EDITION
#define BT_CMD_MAX_RETRIES         50
#else
#define BT_CMD_MAX_RETRIES         20
#endif
#ifdef FEATURE_BT_SOC
  #define BT_L2_MAX_MTU              2036
#else
#define BT_L2_MAX_MTU                BT_L2_DEFAULT_MTU
#endif /* FEATURE_BT_SOC */

/* Data Packet types */

typedef enum {

    BT_LM_FEATURE_BYTE0_SNIFF_B              = 0x80,
    BT_LM_FEATURE_BYTE1_HV1_B                = 0x08,
    BT_LM_FEATURE_BYTE1_HV2_B                = 0x10,
    BT_LM_FEATURE_BYTE1_HV3_B                = 0x20,
    BT_LM_FEATURE_BYTE3_EV3_B                = 0x80,
    BT_LM_FEATURE_BYTE4_EV4_B                = 0x01,
    BT_LM_FEATURE_BYTE4_EV5_B                = 0x02,
    BT_LM_FEATURE_BYTE5_EDR_2MBPS_ESCO_B     = 0x20,
    BT_LM_FEATURE_BYTE5_EDR_3MBPS_ESCO_B     = 0x40,
    BT_LM_FEATURE_BYTE5_EDR_3SLOT_ESCO_B     = 0x80,
    BT_LM_FEATURE_BYTE5_PAUSE_ENCRYPTION     = 0x04
} bt_lmp_feat_mask_bit_type;

typedef enum {
  BT_PT_DM1_B  = 0x0008,
  BT_PT_DH1_B  = 0x0010,
  BT_PT_HV1_B  = 0x0020,
  BT_PT_HV2_B  = 0x0040,
  BT_PT_HV3_B  = 0x0080,
  BT_PT_AUX1_B = 0x0200,
  BT_PT_DM3_B  = 0x0400,
  BT_PT_DH3_B  = 0x0800,
  BT_PT_DM5_B  = 0x4000,
  BT_PT_DH5_B  = 0x8000
} bt_bb_pkt_type;


#define BT_SYNC_DEF_PKT_TYPES 0xFFC0
#define BT_SYNC_DEF_ALL_PKT_TYPES 0xFC3F

typedef enum 
{
  BT_SYNC_PT_HV1_B  = 0x0001,
  BT_SYNC_PT_HV2_B  = 0x0002,
  BT_SYNC_PT_HV3_B  = 0x0004,
  BT_SYNC_PT_EV3_B  = 0x0008,
  BT_SYNC_PT_EV4_B  = 0x0010,
  BT_SYNC_PT_EV5_B  = 0x0020,
  BT_SYNC_PT_2EV3_B = 0x0040,
  BT_SYNC_PT_3EV3_B = 0x0080,
  BT_SYNC_PT_2EV5_B = 0x0100,
  BT_SYNC_PT_3EV5_B = 0x0200
}bt_bb_sync_pkt_type;



typedef enum
{
  BT_PAIR_UNIT,
  BT_PAIR_COMB
} bt_pair_type;


#ifdef FEATURE_BT_SOC
/* Needs to be removed during further clean-up for SOC solution */
typedef enum
{
  BT_LM_CONN_DOWN = 0,
  BT_LM_CONN_QDSP_DISC,
  BT_LM_CONN_LMP_TX_DISC,
  BT_LM_CONN_LMP_RX_DISC,
  BT_LM_CONN_GOING_UP,
  BT_LM_CONN_BB_UP,
  BT_LM_CONN_VERSION,
  BT_LM_CONN_NAME,
  BT_LM_CONN_FEATURES,
  BT_LM_CONN_W4_HOST_ACCEPT,
  BT_LM_CONN_W4_REMOTE_ACCEPT,
  BT_LM_CONN_SWITCH,
  BT_LM_CONN_SECURITY,
  BT_LM_CONN_SETUP_CMPLT,
  BT_LM_CONN_UP,
  BT_LM_CONN_UP_SCO_DISABLED,
  BT_LM_CONN_UP_SCO_RECONFIG
} bt_lm_conn_state_type;
#endif /* FEATURE_BT_SOC */


#define BT_PT_ACL_M    (BT_PT_DM1_B | BT_PT_DH1_B |  \
                        BT_PT_DM3_B | BT_PT_DH3_B |  \
                        BT_PT_DM5_B | BT_PT_DH5_B )

#define BT_PT_SCO_M    (BT_PT_HV1_B | BT_PT_HV2_B | BT_PT_HV3_B)

#define BT_PT_1_SLOT_M (BT_PT_DM1_B | BT_PT_DH1_B |               \
                        BT_PT_HV1_B | BT_PT_HV2_B | BT_PT_HV3_B)

#define BT_PT_3_SLOT_M (BT_PT_DM3_B | BT_PT_DH3_B)

#define BT_PT_5_SLOT_M (BT_PT_DM5_B | BT_PT_DH5_B)

#define BT_PT_ACL_H_M  (BT_PT_DH1_B | BT_PT_DH3_B | BT_PT_DH5_B )


#define BT_SYNC_PT_SCO_M        (BT_SYNC_PT_HV1_B | BT_SYNC_PT_HV2_B | BT_SYNC_PT_HV3_B)
#define BT_SYNC_PT_ESCO_M       (BT_SYNC_PT_EV3_B | BT_SYNC_PT_EV4_B | BT_SYNC_PT_EV5_B)
#define BT_SYNC_PT_ESCO_EDR_M   (BT_SYNC_PT_2EV3_B | BT_SYNC_PT_3EV3_B |                \
                                 BT_SYNC_PT_2EV5_B | BT_SYNC_PT_3EV5_B)


typedef uint8 bt_dac_type[ 10 ];

typedef uint8 bt_lap_type[ 3 ];

#define BT_MAX_LMP_PKT  17

#define BT_SCAN_INQ_B   0x01
#define BT_SCAN_PAGE_B  0x02


/*-------------------------------------------------------------------------*/
/*  Vital statistics array definitions.                                    */
/*-------------------------------------------------------------------------*/

typedef enum
{
  BT_VSAI_MAX_CMD_Q_DEPTH,
  BT_VSAI_MAX_CMDS_REQUEUED,
  BT_VSAI_MAX_CMD_RETRIES,
  BT_VSAI_SD_MAX_EV_Q_DEPTH,
  BT_VSAI_BA_MAX_EV_Q_DEPTH,
  BT_VSAI_SIO_MAX_EV_Q_DEPTH,
  BT_VSAI_RC_MAX_EV_Q_DEPTH,
  BT_VSAI_L2_MAX_EV_Q_DEPTH,
  BT_VSAI_RM_MAX_EV_Q_DEPTH,
  BT_VSAI_HC_MAX_EV_Q_DEPTH,
  BT_VSAI_RC_RX_BAD_FCS_COUNT,
  BT_VSAI_RC_L2_DISC_DLCS_ACTIVE,
#ifdef FEATURE_BT_AG
  BT_VSAI_AG_MAX_EV_Q_DEPTH,
#endif
#ifdef FEATURE_BT_EXTPF
  BT_VSAI_PF_L2IF_MAX_EV_Q_DEPTH,
  BT_VSAI_PF_RMIF_MAX_EV_Q_DEPTH,
#ifdef FEATURE_BT_EXTPF_AV
  BT_VSAI_PF_A2DP_MAX_EV_Q_DEPTH,
#endif /* FEATURE_BT_EXTPF_AV */
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
#endif /* FEATURE_BT_EXTPF */
#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
  BT_VSAI_OEMBTEXTSPP_MAX_EV_Q_DEPTH,
#endif
  BT_VSAI_PF_MAX_CMD_Q_DEPTH,
  BT_VSAI_LAST
} bt_vsai_type;

#define BT_VITAL_STAT_COUNT (BT_VSAI_LAST + 1)

#define BT_VSTAT( idx_suffix ) (bt_vital_stats[ BT_VSAI_##idx_suffix## ])

#define BT_VSTAT_INC( idx_suffix ) (                   \
          bt_vital_stats[ BT_VSAI_##idx_suffix## ]++)

#define BT_VSTAT_DEC( idx_suffix ) (                   \
          bt_vital_stats[ BT_VSAI_##idx_suffix## ]--;)

#define BT_VSTAT_MAX( val, idx_suffix ) {            \
          if ( val > BT_VSTAT(##idx_suffix##) ) {    \
            BT_VSTAT(##idx_suffix##) = val;     } }

#define BT_VSTAT_MIN( val, idx_suffix ) {            \
          if ( val < BT_VSTAT(##idx_suffix##) ) {    \
            BT_VSTAT(##idx_suffix##) = val;     } }



/*-------------------------------------------------------------------------*/
/*             Bleutooth Driver DSM tracer values  (100 - 150)             */
/*-------------------------------------------------------------------------*/

/*  General values.  */
#define DSM_BT_TL_L2           100  /*  Top Level - L2CAP                  */
#define DSM_BT_TL_RC           101  /*  Top Level - RFCOMM                 */
#define DSM_BT_TL_BB           102  /*  Top Level - HCI/BB                 */
#define DSM_BT_TL_OB           103  /*  Top Level - OBEX                   */
#define DSM_BT_TL_A2DP         104  /*  Top Level - A2DP                   */
#define DSM_BT_TL_AVDTP        105  /*  Top Level - AVDTP                  */
                                      
/*  BT RX to CDMA TX values.  */      
#define DSM_BT_RX_Q            110  /*  BT BB RX in q ctrl get             */
#define DSM_BT_RX_BB_L2_DATA   111  /*  BT BB RX l2cap data                */
#define DSM_BT_RX_BB_LOOP_DATA 112
#define DSM_BT_HCI_NUM_PKTS    113  /*  BT BB num_cmplt_pkts_ev            */
#define DSM_BT_RX_HCI_DATA_EV  114  /*  BT HC RX data event                */
#define DSM_BT_RX_REACH_L2     115  /*  BT L2 RX data obtained             */
#define DSM_BT_RX_L2           116  /*  BT L2 RX data                      */
#define DSM_BT_RX_L2_HOLD      117  /*  BT L2 RX hold data                 */
#define DSM_BT_RX_L2_APPEND    118  /*  BT L2 RX append data               */
#define DSM_BT_RX_RC_DATA_EV   119  /*  BT RC RX data event                */
#define DSM_BT_RX_RC_UNPACKED  120  /*  BT RC RX data unpacking hdr/foot   */
#define DSM_BT_RX_RC           121  /*  BT RC RX data                      */
#define DSM_BT_RX_RC_NA        122  /*  BT RC RX NA processing             */
#define DSM_BT_RX_R2S_BT_EV    123  /*  RC2SIO                             */
#define DSM_BT_RX_R2S_PROC_EV  124  /*  RC2SIO                             */
#define DSM_BT_RX_R2S_B4_ENQ   125  /*  RC2SIO                             */
#define DSM_BT_RX_R2S_NO_EVB   126  /*  RC2SIO                             */
                                      
/*  CDMA RX to BT TX values.  */      
#define DSM_BT_TX_L2_SIG       130  /*  BT L2 TX signaling pkts            */
#define DSM_BT_TX_RC_DEQUEUED  131  /*  BT RC TX dequeued DSMs             */
#define DSM_BT_TX_RC_APPEND    132  /*  BT RC TX appending DSMs            */
#define DSM_BT_TX_RC_DUP       133  /*  BT RC TX duplicated DSMs           */
#define DSM_BT_TX_RC_FULL      134  /*  BT RC TX full DSMs                 */
#define DSM_BT_TX_RC_LAST      135  /*  BT RC TX last DSMs                 */
#define DSM_BT_TX_RC           136  /*  BT RC TX DSMs                      */
#define DSM_BT_TX_L2           137  /*  BT L2 TX DSMs                      */
#define DSM_BT_TX_HC           138  /*  BT HC TX DSMs                      */
#define DSM_BT_TX_HC_HOLD      139  /*  BT HC TX Hold DSMs                 */
#define DSM_BT_TX_HC_HOLD_OUT  140  /*  BT HC TX Hold DSMs done            */
                                      
/*  RC2SIO to RFCOMM  */                
#define DSM_BT_ST_TO_RC2SIO    141  /*  Speed test to RC2SIO.              */
#define DSM_BT_RC2SIO_TO_RC    142  /*  RC2SIO RX to RC TX.                */

/* RFCOMM to RC2SIO */
#define DSM_BT_RC2SIO_TO_SIO   143  /*  RC2SIO RC RX to SIO TX.            */


/*-------------------------------------------------------------------------*/
/*  RFCOMM specific definitions.                                           */
/*-------------------------------------------------------------------------*/

#define BT_RC_NULL_PID 0xFFFF

#define BT_RC_MAX_HEADER_BYTES   (sizeof( bt_rc_frame_hdre_type ) +      \
                                    sizeof( bt_rc_cbfc_credits_type ))
#define BT_RC_MAX_FOOTER_BYTES   (sizeof( bt_rc_fcs_type ))
#define BT_RC_MAX_OVERHEAD_BYTES (BT_RC_MAX_HEADER_BYTES +    \
                                     BT_RC_MAX_FOOTER_BYTES)
                                     

/*-------------------------------------------------------------------------*/
/*  L2CAP specific definitions.                                            */
/*-------------------------------------------------------------------------*/

#define BT_L2_NULL_CID 0x0000

#define BT_L2_DEFAULT_MTU 672

#define BT_L2_DEFAULT_FLUSH_TIMEOUT     0xFFFF
#define BT_L2_DEFAULT_SERVICE_TYPE      BT_QOS_BEST_EFFORT
#define BT_L2_DEFAULT_TOKEN_RATE        0x00000000
#define BT_L2_DEFAULT_TOKEN_BUCKET_SIZE 0x00000000
#define BT_L2_DEFAULT_PEAK_BANDWIDTH    0x00000000
#define BT_L2_DEFAULT_LATENCY           0xFFFFFFFF
#define BT_L2_DEFAULT_DELAY_VARIATION   0xFFFFFFFF


/*  The maximum number of distinct protocols  */
/*  that can register with L2CAP.             */
#define BT_L2_MAX_PSM_COUNT 9

#define BT_L2_PSM_INVALID 0
#define BT_L2_PSM_SDP     1
#define BT_L2_PSM_RFCOMM  3
#ifdef FEATURE_BT_LPOS
#error code not present
#endif /* FEATURE_BT_LPOS */

typedef struct  /*  L2CAP packet header. */
{
  uint16 length;
  uint16 channel_id;
} bt_l2_hdr_type;

#define BT_L2_MAX_HEADER_BYTES   (sizeof( bt_l2_hdr_type ))
#define BT_L2_MAX_FOOTER_BYTES   (0)
#define BT_L2_MAX_OVERHEAD_BYTES (BT_L2_MAX_HEADER_BYTES +    \
                                     BT_L2_MAX_FOOTER_BYTES)


/*-------------------------------------------------------------------------*/
/*  RM specific definitions.                                               */
/*-------------------------------------------------------------------------*/

/*  Default encryption key length range in bytes.  */
#define BT_RM_DEF_MAX_ENC_KEY_LEN_BYTES 16
#define BT_RM_DEF_MIN_ENC_KEY_LEN_BYTES 1


/*-------------------------------------------------------------------------*/
/*  HCI specific definitions.                                              */
/*-------------------------------------------------------------------------*/

#define BT_HC_NULL_CONN_HANDLE 0xFFFF


/*-------------------------------------------------------------------------*/
/*  Other definitions...                                                   */
/*-------------------------------------------------------------------------*/

/*  Maximum connection definitions.  */
#define BT_MAX_DEVICES_CONNECTED 3
#define BT_MAX_CONNS_PER_DEVICE  2
#define BT_MAX_ACL_CONNECTIONS   BT_MAX_DEVICES_CONNECTED
#define BT_MAX_SCO_CONNECTIONS   1
#define BT_MAX_CONNECTIONS       (BT_MAX_ACL_CONNECTIONS +   \
                                    BT_MAX_SCO_CONNECTIONS)

/*  1 extra device area for broadcast.  */
#define BT_MAX_QDSP_DEVICE_AREAS (BT_MAX_DEVICES_CONNECTED + 1)

/*  For backward compatibility.  */
#define BT_MAX_BT_DEV            BT_MAX_DEVICES_CONNECTED
#define BT_MAX_BT_DEV_AREA       BT_MAX_QDSP_DEVICE_AREAS
#define BT_MAX_CONN_TO_EACH_UNIT BT_MAX_CONNS_PER_DEVICE
#define BT_MAX_SCO_CONN          BT_MAX_SCO_CONNECTIONS


/*-------------------------------------------------------------------------*/
/*  Baseband definitions                                                   */
/*-------------------------------------------------------------------------*/
#define BT_CLK_TICK_MASK   0x0FFFFFFF
#define BT_CLK_NUM_BITS    28
#define BT_CLK_TICK_TOTAL  0x10000000

//
// page scan constants
//
#define BT_MIN_PAGESCAN_INTERVAL             0x0012
#define BT_MAX_PAGESCAN_INTERVAL             0x1000
#define BT_DEFAULT_PAGESCAN_INTERVAL         0x0400 /* Effective interval is 0x800 */
#define BT_DEFAULT_PAGESCAN_ONLY_INTERVAL    0x0800 /* HCI default is 0x800 */

#define BT_MIN_PAGESCAN_WINDOW               0x0012 /*  11.25ms  */
#define BT_MAX_PAGESCAN_WINDOW               0x1000
#define BT_DEFAULT_PAGESCAN_WINDOW           BT_MIN_PAGESCAN_WINDOW
#define BT_DEFAULT_PAGESCAN_WINDOW_SCO       0x0012 /*  11.25ms  */

//
// inquiry scan constants
//
#define BT_MIN_INQUIRYSCAN_INTERVAL          0x0012
#define BT_MAX_INQUIRYSCAN_INTERVAL          0x1000
#define BT_DEFAULT_INQUIRYSCAN_INTERVAL      0x0400 /* Effective interval is 0x800 */
#define BT_DEFAULT_INQUIRYSCAN_ONLY_INTERVAL 0x0800 /* HCI default is 0x1000 */

#define BT_MIN_INQUIRYSCAN_WINDOW            0x0012 /*  11.25ms  */
#define BT_MAX_INQUIRYSCAN_WINDOW            0x1000
#define BT_DEFAULT_INQUIRYSCAN_WINDOW        BT_MIN_INQUIRYSCAN_WINDOW
#define BT_DEFAULT_INQUIRYSCAN_WINDOW_SCO    0x0012 /*  11.25ms  */

#ifdef FEATURE_BT_SOC
//
// page and inquiry scan constants for SOC 
//

#define BT_PAGESCAN_INTERVAL_HCI_DEF         0x0800
#define BT_INQUIRYSCAN_INTERVAL_HCI_DEF      0x1000

#define BT_PAGESCAN_WINDOW_SOC               0x0012 /*  11.25ms  */
#define BT_PAGESCAN_WINDOW_SOC_SCO           0x0024

#define BT_INQUIRYSCAN_WINDOW_SOC            0x0012 /*  11.25ms  */
#define BT_INQUIRYSCAN_WINDOW_SOC_SCO        0x0024

#endif // FEATURE_BT_SOC

//
// supported Inquiry Access Code (IAC) constants
//
#define BT_MIN_NUM_SUPPORTED_IAC             0x01
#define BT_MAX_NUM_SUPPORTED_IAC             0x40
#define BT_DEFAULT_NUM_SUPPORTED_IAC         3

//
// TX power constants
//
#define BT_MIN_TX_POWER_LEVEL                -30
#define BT_MAX_TX_POWER_LEVEL                20
#define BT_DEFAULT_TX_POWER_LEVEL            0

//
// timeout constants
//
#define BT_DEFAULT_CONN_ACCEPT_TIMEOUT      0x1FA0
#define BT_DEFAULT_PAGE_TIMEOUT             0x2000
#define BT_MAX_FLUSH_TIMEOUT                0x07FF
#define BT_DEFAULT_LINK_SUPERVISION_TIMEOUT 0x7D00
#define BT_MIN_LINK_SUPERVISION_TIMEOUT       8192

//
// data length constants
//
#define BT_DEFAULT_ACL_DATA_PKT_LEN    341
#define BT_DEFAULT_SCO_DATA_PKT_LEN    100
#define BT_DEFAULT_TOTAL_SCO_DATA_PKTS 20
#define BT_DEFAULT_TOTAL_ACL_DATA_PKTS 20

//
// LAPs for inquiry access codes
//
#define BT_GENERAL_INQUIRY_LAP   0x9E8B33
#define BT_DEDICATED_INQUIRY_LAP 0x9E8B00


typedef enum
{
  BT_LCIC_ERICSSON,
  BT_LCIC_NOKIA,
  BT_LCIC_INTEL,
  BT_LCIC_IBM,
  BT_LCIC_TOSHIBA,
  BT_NUM_COMP_ID_CODES

} bt_lmp_comp_id_code_type;

typedef enum
{
  BT_SIO_IDLE,
  BT_SIO_AWAKE,
  BT_SIO_SLEEPING
} bt_sio_state_enum_type;

typedef enum
{
#ifdef FEATURE_BT_SOC
  BT_DS_SOC_IDLE,
  BT_DS_SOC_ACTIVE,
  BT_DS_SOC_DISABLED,
  BT_DS_SOC_DISABLING,
  BT_DS_SOC_ENABLING
#else
  BT_DS_IDLE,
  BT_DS_PENDING_QDSP_DOWNLOAD,
  BT_DS_PENDING_HW_INIT,
  BT_DS_QDSP_SWAPPING_IN,
  BT_DS_READY,
  BT_DS_QDSP_SWAPPING_OUT,
  BT_DS_DSP_SLEEPING,
  BT_DS_QDSP_OUT
#endif
} bt_driver_state_enum_type;

#define BT_IS_IN_QDSP()                                 \
  ( ( bt_dc.driver_state >= BT_DS_PENDING_HW_INIT ) &&  \
    ( bt_dc.driver_state <= BT_DS_QDSP_SWAPPING_OUT ) )
    
#define BT_QDSP_SWAPPING_OUT()                          \
  ( bt_dc.driver_state == BT_DS_QDSP_SWAPPING_OUT )

#ifdef FEATURE_FACTORY_TESTMODE
#define BT_IN_HCI_MODE()                                     \
  ( ( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_HCI_ON ) ||     \
    ( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_HCI_VOC_ON )    \
    || ( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_HCI_FTM ) )
#else
#define BT_IN_HCI_MODE()  \
  ( ( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_HCI_ON ) ||     \
    ( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_HCI_VOC_ON ) )
#endif /* FEATURE_FACTORY_TESTMODE */

typedef enum
{
  BT_DSS_NONE,
  BT_DSS_RESET_AFTER_READY, /* Set qdsp_reset sig after ready  */
  BT_DSS_SWAP_OUT_TO_IDLE,  /* Swap out to go to idle (disable)*/
  BT_DSS_SWAP_OUT_TO_STANDBY,/* Swap out to go to standby      */
  BT_DSS_SWAP_OUT_TO_STANDBY_PREPARING, /* Prepare for swap    */
  BT_DSS_SWAP_OUT_TO_RESET, /* Swap out to reset (reset)       */
  BT_DSS_SWAP_OUT_TO_READY, /* Swap out to swap back in.       */
  BT_DSS_SWAP_OUT_TO_READY_PREPARING /* Prepare to swap out.   */
} bt_driver_substate_enum_type;

typedef enum
{
  BT_BB_STANDBY = 0,  // must match with dsp
  BT_BB_INQUIRY,
  BT_BB_INQUIRY_SCAN,
  BT_BB_PAGE,
  BT_BB_PAGE_SCAN,
  BT_BB_CONNECTION
} bt_bb_state_type;

#ifdef BT_SWDEV_RADIO_OFF
typedef enum
{
  BT_RADIO_DISABLING,
  BT_RADIO_DISABLED,
  BT_RADIO_ENABLING,
  BT_RADIO_ENABLED
} bt_radio_state_type;
#endif /* BT_SWDEV_RADIO_OFF */

typedef enum
{
  BT_RDM_INACTIVE,
  BT_RDM_OPEN,
  BT_RDM_ACTIVE,
  BT_RDM_CLOSE
} bt_rdm_mode_type;

typedef struct
{
  uint32  wakeup_isr_cnt;
  uint32  wakeup_cmd_cnt;
  uint32  return_from_sleep_ev_cnt;
  uint32  sleep_enabled_ev_cnt;
} bt_sleep_data_type;

/* BT Services registered with RDM */
typedef enum
{
  BT_RDM_NULL_SRVC,
  BT_RDM_SOC_SRVC,
  BT_RDM_HCI_SRVC,
  BT_RDM_MAX_SRVC
} bt_rdm_srvc_enum_type;

typedef struct
{
  bt_rdm_srvc_enum_type bt_rdm_service;
  bt_rdm_mode_type      bt_rdm_mode;
} bt_rdm_cfg_type;


typedef struct
{
  bt_driver_state_enum_type     driver_state;
  boolean                       inq_active;
  boolean                       page_active;
  boolean                       scan_active;
  boolean                       disable_scan_pending;
  boolean                       persistent_bg_tick_active;
#ifdef FEATURE_BT_SOC
  bt_sio_state_enum_type        sio_state;
#else
  bt_driver_substate_enum_type  driver_substate;
  boolean                       dsp_dont_unload;
  bt_image_enum_type            dsp_bt_image_to_request;
  bt_dsp_image_enum_type        dsp_image_current;
  bt_dsp_image_enum_type        dsp_image_requested;
  bt_dsp_image_enum_type        dsp_image_desired;
#endif /* FEATRUE_BT_SOC */
  bt_hci_mode_type              cur_hci_mode;
  sio_port_id_type              sio_port_id;
  sio_stream_id_type            sio_stream_id;
#ifdef FEATURE_BT_SOC
  sio_port_id_type              sio_port_id_ext_soc;
  sio_stream_id_type            sio_stream_id_ext_soc;
  boolean                       sio_in_flow_on_ext_soc;
#endif /* FEATRUE_BT_SOC */
#ifdef BT_SWDEV_RADIO_OFF
  bt_radio_state_type           radio_state;
#endif
#ifdef T_MSM6250
#error code not present
#endif /* T_MSM6250 */
  bt_rdm_cfg_type               bt_rdm_cfg[BT_RDM_MAX_SRVC];
#ifdef BT_SWDEV_BT_DISABLED_NV
  boolean                       is_bt_disabled;
#endif /* BT_SWDEV_BT_DISABLED_NV */
#ifdef BT_SWDEV_2_1_SSP
  bt_rm_io_cap_type             io_cap;
  bt_sec_level_type             default_sec_level;
#endif /* BT_SWDEV_2_1_SSP */
} bt_driver_control_type;

typedef struct
{
  uint8     bytes[ BT_EVENT_MASK_LEN ];
} bt_event_mask_type;


/*  Bit definitions for bt_event_q_mask.  */

#ifdef FEATURE_BT_AG
#define BT_EVQ_AG_B       0x0001
#endif
#define BT_EVQ_SD_B       0x0004
#define BT_EVQ_SIO_B      0x0008
#define BT_EVQ_RC_B       0x0010
#define BT_EVQ_L2_B       0x0020
#define BT_EVQ_RM_B       0x0040
#define BT_EVQ_HC_B       0x0080
#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
#define BT_EVQ_BREW_SPP_B 0x0100
#endif

typedef struct bt_event_q_info_struct
{
  q_type*  event_q_ptr;
  uint16   max_q_depth;
  uint16   event_q_bit_mask;
  uint16   max_event_bytes;
} bt_event_q_info_type;

/*-------------------------------------------------------------------------*/
/*  Ideal values for throughput optimization.                              */
/*-------------------------------------------------------------------------*/

#define BT_MAX_SLOTS_DEF 1

#define BT_PT_ACL_DEF_M ( BT_PT_DM1_B | BT_PT_DH1_B )

#define BT_PT_ACL_MAX_SLOT_1_M ( BT_PT_DM1_B | BT_PT_DH1_B )

#define BT_PT_ACL_MAX_SLOT_5_M ( BT_PT_DM1_B | BT_PT_DH1_B |  \
                                 BT_PT_DM3_B | BT_PT_DH3_B |  \
                                 BT_PT_DM5_B | BT_PT_DH5_B )

#define BT_PT_SCO_DEF_M ( BT_PT_HV1_B | BT_PT_HV2_B | BT_PT_HV3_B )

#ifdef FEATURE_BT_SOC
#define BT_MTP_BB_INFO_LEN BT_3DH5_PAYLOAD_MAX_LEN
#else
#define BT_MTP_BB_INFO_LEN BT_DH5_PAYLOAD_MAX_LEN
#endif /* FEATURE_BT_SOC */


#define BT_MTP_L2_INFO_LEN (BT_MTP_BB_INFO_LEN -         \
                              BT_L2_MAX_OVERHEAD_BYTES)

#ifdef FEATURE_BT_SOC
#define BT_MTP_RC_INFO_LEN   2*(BT_MTP_L2_INFO_LEN - BT_RC_MAX_OVERHEAD_BYTES)
#else
#define BT_MTP_RC_INFO_LEN (BT_MTP_L2_INFO_LEN -         \
                              BT_RC_MAX_OVERHEAD_BYTES)
#endif /* FEATURE_BT_SOC */

#ifdef FEATURE_BT_SOC
#define BT_MTP_MAX_HC_LEN  341
#else
#define BT_MTP_MAX_HC_LEN  BT_DH5_PAYLOAD_MAX_LEN
#endif /* FEATURE_BT_SOC */

#ifdef FEATURE_BT_SOC
#define BT_BYTES_PER_10_MS (3 * BT_3DH5_PAYLOAD_MAX_LEN)
#else
#define BT_BYTES_PER_10_MS (3 * BT_DH5_PAYLOAD_MAX_LEN)
#endif /* FEATURE_BT_SOC */

#define BT_RC_MAX_TX_BYTES_PER_TICK ( BT_BYTES_PER_10_MS )

#define BT_MAX_PROT_CHUNK_BYTES   ( BT_RC_MAX_TX_BYTES_PER_TICK + BT_L2_MAX_MTU )
/*-------------------------------------------------------------------------*/
/*  BT EFS Definitions                                                     */
/*-------------------------------------------------------------------------*/

/* Change the File Ver if the bt_efs_params_type struct changes. */
#define BT_EFS_PARAMS_FILE_NAME "BT Params"
#define BT_EFS_PARAMS_FILE_VER  6

#ifdef FEATURE_MANUFACTURER_INFO /* defined for ODM builds */
  #define DEFAULT_BT_NAME         CUST_MODEL_ID
#else
#ifdef CUST_EDITION
#if defined (FEATURE_VERSION_S1000T)
  #define DEFAULT_BT_NAME         "V3 Dew"
#elif defined(FEATURE_VERSION_C316)
  #define DEFAULT_BT_NAME         "IN 50 PLUS"  	
#elif defined(FEATURE_VERSION_W516)
  #define DEFAULT_BT_NAME         "Q7"
#elif defined(FEATURE_VERSION_W0271)
  #define DEFAULT_BT_NAME         "CD035"
#elif defined(FEATURE_VERSION_W317A)
  #define DEFAULT_BT_NAME         "ARC8c"  
#elif defined(FEATURE_VERSION_C337)
#if defined(FEATURE_VERSION_C260_IC18)
  #define DEFAULT_BT_NAME         "Micromax C210"    
#else
  #define DEFAULT_BT_NAME         "Micromax C260"   
#endif
#elif defined(FEATURE_VERSION_IC241A_MMX)
#if defined(FEATURE_VERSION_IN50_MMX)
  #define DEFAULT_BT_NAME         "Micromax IN50"  
#else
  #define DEFAULT_BT_NAME         "Micromax IC241A"
#endif  
#else
  #define DEFAULT_BT_NAME         "Hitz 233C"
#endif
#else
  #define DEFAULT_BT_NAME         "BlueQ"
#endif
#endif

typedef struct
{
  /* LM Info */
  uint8                version;
  char                 bt_name[ BT_MAX_NAME_LEN + 1 ];
  bt_link_key_type     unit_key;
  bt_hc_link_key_type  unit_key_type;  
  boolean              cert_mode;
  boolean              park_supported;
  boolean              toggle_seqn_bit;
  bt_pair_type         pair_type;
  bt_encrypt_mode_type encrypt_mode;
  bt_hci_mode_type     mode_to_start_in;

#ifdef FEATURE_BT_AG       
  /* AG Info */
  bt_ag_idle_mode_type    ag_idle_mode;
  uint16                  ag_idle_timeout;
  bt_audio_device_type    ag_pref_dev_type;
  uint8                   ag_ad_mic_gain;
  uint8                   ag_ad_spkr_gain;

  bt_low_power_mode_type  low_power_mode;
#endif /* FEATURE_BT_AG */

#ifdef BT_SWDEV_2_1_EIR
  char                 bt_short_name[ BT_MAX_EIR_NAME_LEN + 1 ];
  uint8                eir_manuf_data[ BT_MAX_EIR_MANUF_DATA_SIZE ];
  uint8                eir_manuf_data_size;
#endif /* BT_SWDEV_2_1_EIR*/

} bt_efs_params_type;

extern bt_efs_params_type bt_efs_params;

#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
typedef void (bt_brew_spp_cb)( void );
#endif /* FEATURE_IBT  || FEATURE_IBT_DYNAMIC*/

/*-------------------------------------------------------------------------*/
/*                BT Driver Internal MACROs                                */
/*-------------------------------------------------------------------------*/

#define BT_CHECK_RETRY_EXPIRED(num_retries) \
                              ( (num_retries) >= (BT_CMD_MAX_RETRIES - 1) ) 


/*-------------------------------------------------------------------------*/
/*                BT Driver Internal Functions and Data                    */
/*-------------------------------------------------------------------------*/
extern timer_group_type  bt_always_on_timer_group;

extern bt_driver_control_type bt_dc;

extern bt_app_id_type bt_dc_last_hci_ev_reg_app_id;

struct bt_ev_msg_struct;
struct bt_cmd_msg_struct;

extern void bt_store_bt_event( bt_ev_msg_type*        bt_ev_ptr,
                               bt_event_q_info_type*  evqi_ptr );

#ifdef BT_SWDEV_RADIO_OFF
extern void bt_remove_queued_hc_commands( void );
#endif /* BT_SWDEV_RADIO_OFF */

extern q_type bt_event_free_q;

extern void bt_dc_initialize( void );

extern void bt_ec_initialize( void );

#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
extern void bt_ec_register_brew_spp( bt_brew_spp_cb* cb_ptr );
extern void bt_ec_deregister_brew_spp( bt_brew_spp_cb* cb_ptr );
extern void bt_ec_dispatch_brew_spp_ev( void );
#endif /* FEATURE_IBT  || FEATURE_IBT_DYNAMIC */

extern void bt_dc_reg_with_external_services( void );

extern void bt_dc_enable_driver( void );

extern void bt_dc_disable_driver( void );

extern void bt_dc_reset_driver( void );

extern void bt_dc_enable_dsp ( bt_dsp_image_enum_type  dsp_image );

extern boolean bt_cmd_ec_app_id_is_registered(
                 bt_app_id_type bt_app_id );

extern bt_cmd_status_type bt_cmd_ec_reregister_app_id(
                            bt_app_id_type      bt_app_id,
                            bt_ev_cb_func_type* ev_cb_ptr );

extern void bt_ec_send_event( struct bt_ev_msg_struct* bt_ev_ptr );

extern bt_cmd_status_type bt_cmd( struct bt_cmd_msg_struct* bt_cmd_ptr );

extern void bt_record_fault( const char*   filename_str,
                             uint16        line_num,
                             uint32        value1,
                             uint32        value2,
                             uint32        value3 );

#ifdef BT_SWDEV_QXDM_LOG_PER
extern void bt_ag_snapshot_logging_function( void );
extern void bt_l2_snapshot_logging_function( void );
extern void bt_lm_snapshot_logging_function( void );
extern void bt_rc_snapshot_logging_function( void );
#endif /* BT_SWDEV_QXDM_LOG_PER */

extern void bt_enable_clocks( void );
extern void bt_disable_clocks( void );
extern void bt_enable_clocks_after_wakeup( void );
extern void bt_disable_clocks_for_sleep( void );
extern boolean bt_dsm_pushdown_tail( dsm_item_type** dsm_ptr_ptr,
                                     const void *buf,
                                     uint16 size,
                                     dsm_mempool_id_enum_type pool_id );
#ifdef FEATURE_BT_ALIGN_SIO_DATA
extern boolean bt_dsm_align_buffer(dsm_item_type **pkt_ptr);
#endif /*FEATURE_BT_ALIGN_SIO_DATA*/
extern boolean bt_dsm_pushdown_packed( dsm_item_type** dsm_ptr_ptr,
                                       void *buf,
                                       uint16 size,
                                       dsm_mempool_id_enum_type pool_id );
extern void bt_set_vdd_min_allowed( void );
extern void bt_set_vdd_min_not_allowed( void );

extern uint16 bt_vital_stats[];

extern rex_timer_type bt_wait_timer;

#define BT_QDSP_POLL_INTERVAL_MS 2
extern rex_timer_type bt_qdsp_poll_timer;

extern bt_bd_addr_type bt_my_bd_addr;


/*  Data for BT Task / SIO communication.  */
extern dsm_watermark_type  bt_from_sio_wm;  /*  Data for BT Task / SIO  */
extern dsm_watermark_type  bt_to_sio_wm;    /*    communication.        */
extern q_type              bt_to_sio_q;
extern q_type              bt_from_sio_q;
#ifdef FEATURE_BT_SOC
extern dsm_watermark_type  bt_soc_to_msm_sio_wm;  /*  Data for BT Task-SOC */
extern dsm_watermark_type  bt_msm_to_soc_sio_wm;    /*    communication.   */
extern q_type              bt_soc_to_msm_sio_q;
extern q_type              bt_msm_to_soc_sio_q;
#endif /*FEATURE_BT_SOC */


extern bt_sleep_data_type bt_sleep_data;

/*===========================================================================

FUNCTION
  bt_qdsp_ready_init

DESCRIPTION
  This function is called when the Bluetooth HW is initialized.
  Performs the third and final part of enabling the Bluetooth driver.

===========================================================================*/
void bt_qdsp_ready_init( void );

#ifdef FEATURE_BT_SOC

/*===========================================================================

FUNCTION
  bt_dc_soc_startup

DESCRIPTION
  This routine powers of the BT Controller SoC.

===========================================================================*/
extern void bt_dc_soc_startup( void );


/*===========================================================================

FUNCTION
  bt_soc_ready_init

DESCRIPTION
  This function is called by the  Bluetooth host when it wants to start
  the external SOC
===========================================================================*/
void bt_soc_ready_init( void );

/*===========================================================================

FUNCTION
  bt_soc_shutdown_cleanup

DESCRIPTION
  This function is called by the  Bluetooth host when it wants to shut
  down the external SOC
===========================================================================*/
void bt_soc_shutdown_cleanup( void );

/*===========================================================================

FUNCTION
  bt_soc_enable_flow

DESCRIPTION
  This sets the UART1DM inbound flow to enabled, enabling the SOC to send more
  data to the BT host driver.

===========================================================================*/
void bt_soc_enable_flow(void);

/*===========================================================================

FUNCTION
  bt_soc_disable_flow

DESCRIPTION
  This sets the UART1DM inbound flow to diabled, disabling the SOC from sendng
  more data to the BT host driver.

===========================================================================*/
void bt_soc_disable_flow(void);

#endif /* FEATURE_BT_SOC */

/*===========================================================================

FUNCTION
  bt_write_to_efs

DESCRIPTION
  Saves the BT Params to EFS.

============================================================================*/
boolean bt_write_to_efs
( 
  boolean create_new_file
);

/*===========================================================================

FUNCTION
  bt_efs_init

DESCRIPTION
  Initializes BT Params from EFS.

============================================================================*/
boolean bt_efs_init( void );

/*===========================================================================

FUNCTION
  bt_free_any_dsms_in_event

DESCRIPTION
  Frees any DSMs included within a specific BT event.

===========================================================================*/
void bt_free_any_dsms_in_event( bt_ev_msg_type*  bt_ev_ptr );

/*-------------------------------------------------------------------------*/
/*                            RFCOMM Commands                              */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_rc_register(
                            bt_app_id_type      bt_app_id,
                            uint8               server_channel,
                            dsm_watermark_type* tx_wm_ptr,
                            dsm_watermark_type* rx_wm_ptr );

extern bt_cmd_status_type bt_cmd_rc_connect(
                            bt_app_id_type      bt_app_id,
                            uint8               server_channel,
                            bt_bd_addr_type*    bd_addr_ptr,
                            dsm_watermark_type* tx_wm_ptr,
                            dsm_watermark_type* rx_wm_ptr );

extern bt_cmd_status_type bt_cmd_rc_connect_custom_cfg(
                            bt_app_id_type      bt_app_id,
                            uint8               server_channel,
                            bt_bd_addr_type*    bd_addr_ptr,
                            dsm_watermark_type* tx_wm_ptr,
                            dsm_watermark_type* rx_wm_ptr,
                            boolean             pn_negot_done,
                            boolean             rpn_negot_done,
                            uint16              max_frame_size,
                            uint8               priority,
                            boolean             cb_flow_ctrl,
                            uint8               baudrate,
                            uint8               format,
                            uint8               flow_ctrl,
                            uint8               xon_char,
                            uint8               xoff_char );

extern bt_cmd_status_type bt_cmd_rc_tx_data(
                            bt_app_id_type bt_app_id,
                            uint16         port_id );

extern bt_cmd_status_type bt_cmd_rc_set_port_rx_fc(
                            bt_app_id_type bt_app_id,
                            uint16         port_id,
                            boolean        enable_flow );

extern bt_cmd_status_type bt_cmd_rc_set_session_rx_fc(
                            bt_app_id_type bt_app_id,
                            uint16         port_id,
                            boolean        enable_flow );

/*---------------------------------------------------------------------*/
/*  Definitions used with modem status read/write routines.            */
/*                                                                     */
/*  Glossary:                                                          */
/*                                                                     */
/*    DTE = Data Terminal Equipment                                    */
/*    DCE = Data Communication Equipment                               */
/*                                                                     */
/*  RS-232 signal names:                                               */
/*                                                                     */
/*    DSR = Data Set Ready                                             */
/*    DTR = Data Terminal Ready                                        */
/*    RI  = Ring Indicator                                             */
/*    CD  = Carrier Detect                                             */
/*    RTS = Ready To Send                                              */
/*    CTS = Clear To Send                                              */
/*                                                                     */
/*  Generic (DTE/DCE independent) signal names:                        */
/*                                                                     */
/*    RTR = Ready To Receive                                           */
/*    RTC = Ready To Communicate                                       */
/*    IC  = Incoming Call                                              */
/*    DV  = Data Valid                                                 */
/*                                                                     */
/*  Below is a summary of signal name mappings.  The arrows indicate   */
/*  the intended direction of the signal.  All caps is used at the     */
/*  source (the side that writes the signal).  Small letters are used  */
/*  at the destination (the side that reads the signal).  The table    */
/*  summarizes both DTE to DCE and DTE to DTE (a.k.a. null modem)      */
/*  connections.  Note that the signals change name for DTE/DCE        */
/*  implementations as they go from source to destination.             */
/*                                                                     */
/*  DTE     DTE     DCE   Gen                                          */
/*  ===     ===     ===   ===                                          */
/*  dtr <-- DSR --> dtr   RTC                                          */
/*  DSR --> dtr <-- DSR                                                */
/*                                                                     */
/*  rts <-- CTS --> rts   RTR                                          */
/*  CTS --> rts <-- CTS                                                */
/*                                                                     */
/*  OFF --> ri  <-- RI    IC                                           */
/*  ON  --> cd  <-- CD    DV                                           */
/*---------------------------------------------------------------------*/

/*  Bit masks used for ms_valid_mask and modem_status fields  */
/*  below and for the ms_changed_mask in the modem status     */
/*  changed RC event.                                         */

#define BT_RC_MS_SIGS_M         RCMCCM_MSC_V24_MS_SIGS_M
#define BT_RC_MS_LOC_DEFAULT_V  RCMCCM_MSC_V24_LOC_DEFAULT_V
#define BT_RC_MS_FAR_DEFAULT_V  RCMCCM_MSC_V24_FAR_DEFAULT_V
#define BT_RC_MS_RTR_ASSERTED_B RCMCCM_MSC_V24_RTR_ASSERTED_B
#define BT_RC_MS_RTC_ASSERTED_B RCMCCM_MSC_V24_RTC_ASSERTED_B
#define BT_RC_MS_IC_ASSERTED_B  RCMCCM_MSC_V24_IC_ASSERTED_B
#define BT_RC_MS_DV_ASSERTED_B  RCMCCM_MSC_V24_DV_ASSERTED_B

extern bt_cmd_status_type bt_cmd_rc_read_modem_status(
                            bt_app_id_type bt_app_id,
                            uint16         port_id,
                            uint8*         modem_status_ptr,
                            uint8*         breaks_ptr,
                            uint8*         last_break_length_ptr );

extern bt_cmd_status_type bt_cmd_rc_write_modem_status(
                            bt_app_id_type bt_app_id,
                            uint16         port_id,
                            uint8          ms_valid_mask,
                            uint8          modem_status,
                            boolean        break_present,
                            uint8          break_length );

extern bt_cmd_status_type bt_cmd_rc_write_sp_config(
                            bt_app_id_type  bt_app_id,
                            uint16          port_id,
                            uint8           baudrate,
                            uint8           format,
                            uint8           flow_ctrl,
                            uint8           xon_char,
                            uint8           xoff_char );

extern bt_cmd_status_type bt_cmd_rc_read_line_status(
                            bt_app_id_type bt_app_id,
                            uint16         port_id,
                            uint8*         overruns_ptr,
                            uint8*         parity_errors_ptr,
                            uint8*         framing_errors_ptr );

extern bt_cmd_status_type bt_cmd_rc_line_error(
                            bt_app_id_type bt_app_id,
                            uint16         port_id,
                            boolean        overrun_error,
                            boolean        parity_error,
                            boolean        framing_error );

extern bt_cmd_status_type bt_cmd_rc_ping(
                            bt_app_id_type bt_app_id,
                            uint16         port_id,
                            uint16         byte_count,
                            uint8          start_value,
                            uint8          increment );

extern bt_cmd_status_type bt_cmd_rc_disconnect(
                            bt_app_id_type bt_app_id,
                            uint16         port_id );

extern bt_cmd_status_type bt_cmd_rc_unregister(
                            bt_app_id_type bt_app_id,
                            uint8          server_channel );

extern bt_cmd_status_type bt_cmd_rc_cancel_connect(
                            bt_app_id_type   bt_app_id,
                            uint8            server_channel,
                            bt_bd_addr_type* bd_addr_ptr );

/*-------------------------------------------------------------------------*/
/*                             L2CAP Commands                              */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_l2_set_params(
                            bt_app_id_type      bt_app_id,
                            uint16              cid,
                            uint16              protocol_id,
                            uint16              out_flush_to,
                            uint16              in_mtu,
                            bt_qos_service_type service_type,
                            dword               token_rate,
                            dword               token_bucket_size,
                            dword               peak_bandwidth,
                            dword               latency,
                            dword               delay_variation );

extern bt_cmd_status_type bt_cmd_l2_register(
                            bt_app_id_type bt_app_id,
                            uint16         protocol_id );

extern bt_cmd_status_type bt_cmd_l2_connect(
                            bt_app_id_type   bt_app_id,
                            uint16           protocol_id,
                            bt_bd_addr_type* bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_l2_write(
                            bt_app_id_type bt_app_id,
                            uint16         channel_id,
                            dsm_item_type* dsm_ptr,
                            void*          frame_handle);

extern bt_cmd_status_type bt_cmd_l2_ping(
                            bt_app_id_type    bt_app_id,
                            bt_bd_addr_type*  bd_addr_ptr,
                            uint16            length,
                            uint8*            data );

extern bt_cmd_status_type bt_cmd_l2_disconnect(
                            bt_app_id_type bt_app_id,
                            uint16         channel_id );

extern bt_cmd_status_type bt_cmd_l2_disconn_immediate(
                            bt_app_id_type bt_app_id,
                            uint16         channel_id );

extern bt_cmd_status_type bt_cmd_l2_unregister(
                            bt_app_id_type bt_app_id,
                            uint16         protocol_id );

extern bt_cmd_status_type bt_cmd_l2_connect_immediate_cid(
                            bt_app_id_type   bt_app_id,
                            uint16           protocol_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            uint16*          cid);

extern bt_cmd_status_type bt_cmd_l2_disable_timeout(
                            bt_app_id_type   bt_app_id,
                            uint16           cid);

extern bt_cmd_status_type bt_cmd_l2_get_num_conn(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            uint16*          num_conn);

extern boolean bt_cmd_l2_get_acl_tx_flow_status(
                            uint16    cid);

/* Note: these aren't commands: */
extern uint16 bt_l2_get_free_cid( void );
extern uint16 bt_l2_get_num_conn( bt_bd_addr_type *bd_addr_ptr );

/*-------------------------------------------------------------------------*/
/*                       Radio Manager (RM) Commands                       */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_rm_set_connectable(
                            bt_app_id_type        bt_app_id,
                            boolean               connectable,
                            bt_rm_act_vs_pwr_type act_vs_pwr );

extern bt_cmd_status_type bt_cmd_rm_set_discoverable(
                            bt_app_id_type          bt_app_id,
                            bt_rm_discoverable_type discoverable,
                            bt_rm_act_vs_pwr_type   act_vs_pwr );

typedef BT_PACKED struct
{
  uint8  iac_bytes[ 3 ];
} bt_iac_type;

#define BT_IAC_2  0x9E
#define BT_IAC_1  0x8B
#define BT_GIAC_0 0x33
#define BT_LIAC_0 0x00

extern bt_cmd_status_type bt_cmd_rm_inquire(
                            bt_app_id_type bt_app_id,
                            bt_iac_type    iac,
                            uint8          duration_secs,
                            bt_cod_type*   class_of_device_filter_ptr,
                            uint8          max_responses );

extern bt_cmd_status_type bt_cmd_rm_stop_inquiry(
                            bt_app_id_type bt_app_id );

extern bt_cmd_status_type bt_cmd_rm_remote_name_req(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_rm_remote_name_req_cancel(
                            bt_app_id_type    bt_app_id,
                            bt_bd_addr_type*  bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_rm_connect_acl(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_rm_connect_cancel(
                            bt_app_id_type    bt_app_id,
                            bt_bd_addr_type*  bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_rm_register_acl(
                            bt_app_id_type bt_app_id );

extern bt_cmd_status_type bt_cmd_rm_accept_acl(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_rm_reject_acl(
                            bt_app_id_type        bt_app_id,
                            bt_bd_addr_type*      bd_addr_ptr,
                            bt_rm_rej_reason_type reason );

extern bt_cmd_status_type bt_cmd_rm_unregister_acl(
                            bt_app_id_type bt_app_id );

extern bt_cmd_status_type bt_cmd_rm_tx_data_acl(
                            bt_app_id_type               bt_app_id,
                            bt_rm_handle_type            handle,
                            bt_hc_pkt_boundary_flag_type l2cap_pkt_start,
                            word                         out_flush_to,
                            dsm_item_type*               dsm_ptr );

extern bt_cmd_status_type bt_cmd_rm_disconnect_acl(
                            bt_app_id_type         bt_app_id,
                            bt_rm_handle_type      handle,
                            bt_rm_disc_reason_type disc_reason );

extern bt_cmd_status_type bt_cmd_rm_connect_sco(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            boolean          attempt_esco );

extern bt_cmd_status_type bt_cmd_rm_register_sco(
                            bt_app_id_type bt_app_id );

extern bt_cmd_status_type bt_cmd_rm_accept_sco(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_rm_reject_sco(
                            bt_app_id_type        bt_app_id,
                            bt_bd_addr_type*      bd_addr_ptr,
                            bt_rm_rej_reason_type reason );

extern bt_cmd_status_type bt_cmd_rm_unregister_sco(
                            bt_app_id_type bt_app_id );

extern bt_cmd_status_type bt_cmd_rm_tx_data_sco(
                            bt_app_id_type    bt_app_id,
                            bt_rm_handle_type handle,
                            dsm_item_type*    dsm_ptr );

extern bt_cmd_status_type bt_cmd_rm_disconnect_sco(
                            bt_app_id_type         bt_app_id,
                            bt_rm_handle_type      handle,
                            bt_rm_disc_reason_type reason );

extern bt_cmd_status_type bt_cmd_rm_enforce_security_rfcomm_scn(
                            bt_app_id_type   bt_app_id,
                            uint8            rfcomm_scn,
                            bt_bd_addr_type* bd_addr_ptr,
                            boolean          local_client );
      
extern bt_cmd_status_type bt_cmd_rm_enforce_security_l2cap_psm(
                            bt_app_id_type   bt_app_id,
                            uint16           l2cap_psm,
                            bt_bd_addr_type* bd_addr_ptr,
                            uint16           l2cap_cid,
                            bt_rm_atzrq_type conn_dir );
      
extern bt_cmd_status_type bt_cmd_rm_uuid_scn_paired(
                            bt_sd_uuid_type  sdp_uuid,
                            uint8            rfcomm_scn,
                            bt_bd_addr_type* bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_rm_uuid_scn_unpaired(
                            bt_sd_uuid_type  sdp_uuid,
                            uint8            rfcomm_scn,
                            bt_bd_addr_type* bd_addr_ptr );

#ifdef BT_SWDEV_2_1_PB
extern bt_cmd_status_type bt_cmd_rm_set_auto_flush_to(
                             uint32             flush_to,
                             bt_rm_handle_type  handle );
#endif /* BT_SWDEV_2_1_PB */

/*-------------------------------------------------------------------------*/
/*                Host Controller Interface (HCI) Commands                 */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_hc_inquiry(
                            bt_lap_type lap,
                            uint8       inquiry_len,
                            uint8       num_responses );

extern bt_cmd_status_type bt_cmd_hc_inquiry_cancel( void );

extern bt_cmd_status_type bt_cmd_hc_create_connection(
              bt_bd_addr_type*           bd_addr_ptr,
              bt_bb_pkt_type             pkt_type,
              bt_page_scan_rep_mode_type ps_rep_mode,
              bt_page_scan_mode_type     ps_mode,
              uint16                     clk_offset,
              bt_role_switch_type        role_switch );

extern bt_cmd_status_type bt_cmd_hc_create_connection_cancel(
                            bt_bd_addr_type* bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_hc_rd_remote_feature (
                                     uint16  conn_hndl );

#ifdef FEATURE_BT_2_1
extern bt_cmd_status_type bt_cmd_hc_rd_remote_feature_ext (
                                     uint16  conn_hndl,
                                     uint8   page_number );
#endif /* FEATURE_BT_2_1 */

extern bt_cmd_status_type bt_cmd_hc_rd_remote_version (
                                    uint16  conn_hndl  );

extern bt_cmd_status_type bt_cmd_hc_rd_clk_offset (
                                    uint16  conn_hndl  );

extern bt_cmd_status_type bt_cmd_hc_accept_connection(
                            bt_bd_addr_type* bd_addr_ptr,
                            bt_role_type     role );

extern bt_cmd_status_type bt_cmd_hc_reject_connection(
                            bt_bd_addr_type* bd_addr_ptr,
                            uint8            reason );

extern bt_cmd_status_type bt_cmd_hc_authentication_requested(
                            uint16 conn_hndl );

extern bt_cmd_status_type bt_cmd_hc_tx_acl_data(
              uint16                       conn_hndl,
              bt_hc_pkt_boundary_flag_type pkt_boundary_flag,
              bt_hc_broadcast_flag_type    broadcast_flag,
              dsm_item_type*               dsm_ptr,
              uint16                       local_cid,
              void*                        frame_handle );

extern bt_cmd_status_type bt_cmd_hc_tx_sco_data(
                            uint16         conn_hndl,
                            dsm_item_type* dsm_ptr );

extern bt_cmd_status_type bt_cmd_hc_add_sco_connection(
                            uint16         conn_hndl,
                            bt_bb_pkt_type pkt_type );

extern bt_cmd_status_type bt_cmd_hc_disconnect(
                            uint16 conn_hndl,
                            uint8  reason );

extern bt_cmd_status_type bt_cmd_hc_sniff_mode(
                            uint16 conn_hndl,
                            uint16 min_interval,
                            uint16 max_interval,
                            uint16 sniff_attempt,
                            uint16 sniff_timeout );

extern bt_cmd_status_type bt_cmd_hc_exit_sniff_mode(
                            uint16 conn_hndl );

extern bt_cmd_status_type bt_cmd_hc_exit_park_mode(
                            uint16 conn_hndl );

extern bt_cmd_status_type bt_cmd_hc_link_key_req_reply(
                            bt_bd_addr_type* bd_addr,
                            bt_link_key_type link_key );

extern bt_cmd_status_type bt_cmd_hc_link_key_req_reply_neg(
                            bt_bd_addr_type* bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_hc_pin_code_req_reply(
                            bt_bd_addr_type* bd_addr_ptr,
                            uint8            pin_code_len,
                            uint8            pin_code[] );

extern bt_cmd_status_type bt_cmd_hc_pin_code_req_reply_neg( 
                            bt_bd_addr_type* bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_hc_remote_name_req(
                            bt_bd_addr_type            bd_addr,
                            bt_page_scan_rep_mode_type ps_rep_mode,
                            bt_page_scan_mode_type     ps_mode,
                            uint16                     clk_offset );

extern bt_cmd_status_type bt_cmd_hc_wr_link_supervision_timeout(
                             uint16                      conn_hndl,
                             uint16                      link_supervision_to );

extern bt_cmd_status_type bt_cmd_hc_remote_name_req_cancel(
                            bt_bd_addr_type*  bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_hc_wr_scan_enable(
                            bt_hc_scan_enable_type scan_enable );

extern bt_cmd_status_type bt_cmd_hc_wr_auth_enable(
                            bt_hc_auth_enable_type auth_enable );

extern bt_cmd_status_type bt_cmd_hc_wr_encrypt_mode(
                            bt_encrypt_mode_type encrypt_mode );

extern bt_cmd_status_type bt_cmd_hc_wr_class_of_device( bt_cod_type cod );

extern bt_cmd_status_type bt_cmd_hc_wr_stored_link_key(
                            uint8          num_keys_to_write,
                            dsm_item_type* dsm_ptr );
              /*  dsm_ptr the equivalent of the following data:       */
              /*    bt_bd_addr_type   bd_addr[ num_keys_to_write ];   */
              /*    bt_link_key_type  link_key[ num_keys_to_write ];  */

extern bt_cmd_status_type bt_cmd_hc_change_local_name(
                            dsm_item_type* dsm_ptr );
              /*  dsm_ptr contains up to 248 bytes of UTF-8  */
              /*  encoded characters; null terminated.       */

extern bt_cmd_status_type bt_cmd_hc_host_num_cmplt_pkt(
                            uint8          number_of_handles,
                            dsm_item_type* dsm_ptr );
              /*  dsm_ptr the equivalent of the following data:        */
              /*    uint16  conn_hndl[ number_of_handles ];            */
              /*    uint16  host_num_cmplt_pkts[ number_of_handles ];  */

extern bt_cmd_status_type bt_cmd_hc_wr_cur_iac_lap(
                            uint8          num_current_iac,
                            dsm_item_type* dsm_ptr );
              /*  dsm_ptr the equivalent of the following data:   */
              /*    bt_lap_type      iac_lap[ num_current_iac ];  */

extern bt_cmd_status_type bt_cmd_hc_wr_page_to( uint16 page_timeout );

extern bt_cmd_status_type bt_cmd_hc_wr_page_scan_act(
              uint16 page_scan_interval,   /* in .625 ms units */
              uint16 page_scan_window );   /* in .625 ms units */

extern bt_cmd_status_type bt_cmd_hc_wr_inq_scan_act(
              uint16 inq_scan_interval,   /* in .625 ms units */
              uint16 inq_scan_window);    /* in .625 ms units */

extern bt_cmd_status_type bt_cmd_hc_enable_dut_mode( void );

extern bt_cmd_status_type bt_cmd_hc_send_raw_mode_pkt(
              uint16         conn_hndl,
              uint8          start,          /*  Start of data to send  */
              uint8          end,            /*  Last of data to send   */
              uint8          num_slots,
              uint16         data_byte_len,  /*  Not to exceed 360--    */
                                             /*    size of a HV5        */
              dsm_item_type* dsm_ptr );      /*  The data to transmit   */

#ifdef FEATURE_BT_TEST_MODE_TESTER
extern bt_cmd_status_type bt_cmd_hc_tester_control(
              uint16         conn_hndl,
              uint8          test_senario,
              uint8          hopping_mode,
              uint8          dut_tx_freq_k,
              uint8          dut_rx_freq_k,
              uint8          power_control_mode,
              uint8          poll_period,  /*  In units of 1.25 ms  */
              uint8          packet_type,
              uint16         payload_len,
              uint8          start_end,    /*  start = 0; end = 1  */
              dsm_item_type* dsm_ptr );
#endif /* FEATURE_BT_TEST_MODE_TESTER */

extern bt_cmd_status_type bt_cmd_hc_wr_link_policy(
                            uint16 conn_hndl,
                            uint16 link_policy_settings );

extern bt_cmd_status_type bt_cmd_hc_set_event_filter_conn_all_dev(
              bt_auto_accept_type auto_accept_flag );

extern bt_cmd_status_type bt_cmd_hc_set_event_filter_clear_all( void );

extern bt_cmd_status_type bt_cmd_hc_vs_any(
                            dsm_item_type*  dsm_ptr,
                            uint16          opcode );

extern bt_cmd_status_type bt_cmd_hc_send_raw_hc_cmd(
                            bt_cmd_type cmd_type,
                            uint8*      cmd_ptr,
                            uint16      byte_count );

extern bt_cmd_status_type bt_cmd_hc_change_pkt_type(
                            uint16 conn_hndl,
                            uint16 pkt_type );

extern bt_cmd_status_type bt_cmd_hc_set_conn_encrypt(
                            uint16                 conn_hndl,
                            bt_encrypt_enable_type encrypt_en );

extern bt_cmd_status_type bt_cmd_hc_change_link_key(
                            uint16 conn_hndl );

extern bt_cmd_status_type bt_cmd_hc_wr_enc_key_len(
                            uint8 min_enc_key_size,
                            uint8 max_enc_key_size );

                                      /*  use BT_HC_VS_**_M #defines  */
extern bt_cmd_status_type bt_cmd_hc_wr_voice_setting(
                            uint16 voice_encoding );

extern bt_cmd_status_type bt_cmd_hc_set_incoming_hold_params(
                            uint16 conn_hndl,
                            uint16 min_interval,
                            uint16 max_interval );

extern bt_cmd_status_type bt_cmd_hc_set_incoming_sniff_params(
                            uint16 conn_hndl,
                            uint16 min_interval,
                            uint16 max_interval );

extern bt_cmd_status_type bt_cmd_hc_set_incoming_park_params(
                            uint16 conn_hndl,
                            uint16 min_interval,
                            uint16 max_interval );

extern bt_cmd_status_type bt_cmd_hc_wr_auto_flush_timeout(
                            uint16 conn_hndl,
                            uint16 flush_timeout );

extern bt_cmd_status_type bt_cmd_hc_tx_continuous(
                            uint8 hopping );

extern bt_cmd_status_type bt_cmd_hc_tx_continuous_freq(
                            uint8 channel_number );

extern bt_cmd_status_type bt_cmd_hc_switch_role(
                            bt_bd_addr_type* bd_addr_ptr,
                            bt_role_type     role_type );

extern bt_cmd_status_type bt_cmd_hc_rd_local_feat( void );

extern bt_cmd_status_type bt_cmd_hc_rd_local_ver( void );

extern bt_cmd_status_type bt_cmd_hc_rd_rssi( uint16 conn_hndl );

#ifdef FEATURE_BT_1_2

extern bt_cmd_status_type bt_cmd_hc_setup_sync_conn(
  uint16            conn_hndl,
  uint32            tx_bandwidth,
  uint32            rx_bandwidth,
  uint16            max_latency,
  uint16            voice_setting,
  uint8             retransmission_effort,
  uint16            pkt_type
);

extern bt_cmd_status_type bt_cmd_hc_accept_sync_conn_req(
  bt_bd_addr_type*  bd_addr_ptr,
  uint32            tx_bandwidth,
  uint32            rx_bandwidth,
  uint16            max_latency,
  uint16            content_format,
  uint8             retransmission_effort,
  uint16            pkt_type
);

extern bt_cmd_status_type bt_cmd_hc_reject_sync_conn_req(
  bt_bd_addr_type*  bd_addr_ptr,
  uint8             reason
);

extern bt_cmd_status_type bt_cmd_hc_afh_host_chan_class(
  bt_afh_ch_map_type ch_class
);

extern bt_cmd_status_type bt_cmd_hc_read_afh_chan_assess_mode(
  void
);

extern bt_cmd_status_type bt_cmd_hc_set_afh_chan_assess_mode(
  uint8 afh_chan_asses_mode
);

extern bt_cmd_status_type bt_cmd_hc_read_afh_chan_map(
  uint16 conn_hndl
);

extern bt_cmd_status_type bt_cmd_hc_rd_buf_size( void );

extern bt_cmd_status_type bt_cmd_hc_host_buf_size( void );

extern bt_cmd_status_type bt_cmd_hc_set_hc_to_host_fc( void );

extern bt_cmd_status_type bt_cmd_hc_set_afh_chan_good(
  bt_afh_ch_map_type ch_map
);

extern bt_cmd_status_type bt_cmd_hc_control_afh(
  uint8 enable
);

extern bt_cmd_status_type bt_cmd_hc_wr_inq_scan_type(
  bt_inq_scan_mode_type scan_type 
);

extern bt_cmd_status_type bt_cmd_hc_wr_page_scan_type(
  bt_page_scan_mode_type scan_type 
);

extern bt_cmd_status_type bt_cmd_hc_rd_inq_scan_type(
  void 
);

extern bt_cmd_status_type bt_cmd_hc_rd_page_scan_type(
  void 
);

#endif /* FEATURE_BT_1_2 */

#ifdef BT_SWDEV_2_1_SSP
extern bt_cmd_status_type bt_cmd_hc_io_capability_response(
                            bt_bd_addr_type*  bd_addr_ptr,
                            uint8             io_capability,
                            uint8             oob_data_present,
                            uint8             auth_req );

extern bt_cmd_status_type bt_cmd_hc_user_cfmtn_rqst_reply(
                            bt_bd_addr_type*  bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_hc_user_cfmtn_rqst_neg_reply(
                            bt_bd_addr_type*  bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_hc_user_passkey_rqst_reply(
                            bt_bd_addr_type*  bd_addr_ptr,
                            uint32            num_val );

extern bt_cmd_status_type bt_cmd_hc_user_passkey_rqst_neg_reply(
                            bt_bd_addr_type*  bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_hc_remote_oob_data_rqst_reply(
                            bt_bd_addr_type*  bd_addr_ptr,
                            bt_sp_hash_type*  sp_hash_ptr,
                            bt_sp_rand_type*  sp_rand_ptr );

extern bt_cmd_status_type bt_cmd_hc_remote_oob_data_rqst_neg_reply(
                            bt_bd_addr_type*  bd_addr_ptr );

extern bt_cmd_status_type bt_cmd_hc_io_capability_rqst_neg_reply(
                            bt_bd_addr_type*  bd_addr_ptr,
                            uint8             reason );

extern bt_cmd_status_type bt_cmd_hc_refresh_encryption_key(
                            uint16 conn_hndl );

extern bt_cmd_status_type bt_cmd_hc_rd_simple_pairing_mode(
                            void );

extern bt_cmd_status_type bt_cmd_hc_wr_simple_pairing_mode(
                            uint8 sp_mode );

extern bt_cmd_status_type bt_cmd_hc_rd_local_oob_data(
                            void );

extern bt_cmd_status_type bt_cmd_hc_send_keypress_notification(
                            bt_bd_addr_type*  bd_addr_ptr,
                            uint8             notification_type );

extern bt_cmd_status_type bt_cmd_hc_wr_simple_pairing_debug_mode(
                            uint8 sp_debug_mode );

#endif /* BT_SWDEV_2_1_SSP */

#ifdef BT_SWDEV_2_1_SSR
extern bt_cmd_status_type bt_cmd_hc_sniff_subrating(
                            uint16            conn_hndl,
                            uint16            max_latency,
                            uint16            min_remote_to,
                            uint16            min_local_to );
#endif /* BT_SWDEV_2_1_SSR */

#ifdef BT_SWDEV_2_1_EIR
extern bt_cmd_status_type bt_cmd_hc_set_event_mask(
  bt_event_mask_type    event_mask
);

extern bt_cmd_status_type bt_cmd_hc_wr_inq_mode(
  bt_inq_mode_type      inquiry_mode
);

extern bt_cmd_status_type bt_cmd_hc_wr_ext_inq_rsp(
  dsm_item_type*        dsm_ptr, 
  uint8                 fec_required
);

extern bt_cmd_status_type bt_cmd_hc_rd_ext_inq_rsp(
  void
);

extern bt_cmd_status_type bt_cmd_hc_wr_inq_tx_pwr(
  uint8                 tx_power_level
);

extern bt_cmd_status_type bt_cmd_hc_rd_inq_rsp_tx_pwr(
  void
);
#endif /* BT_SWDEV_2_1_EIR */

#if defined (FEATURE_BT_AG) && defined (FEATURE_BT_ICON)
#error code not present
#endif /* (FEATURE_BT_AG && FEATURE_BT_ICON) */

extern uint32 bt_ag_current_supported_features( void );


/*===========================================================================

FUNCTION
  bt_dc_process_rdm_change

DESCRIPTION
  Processes change in RDM mode and opens/closes the suitable sio port

===========================================================================*/
extern void bt_dc_process_rdm_change ( void );


#ifdef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_dc_sio_ioctl

DESCRIPTION
  BT Wrapper function for sio_ioctl

===========================================================================*/
extern void bt_dc_sio_ioctl
(
  sio_stream_id_type    stream_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type  *param
);


/*===========================================================================

FUNCTION
  bt_dc_sio_change_baud

DESCRIPTION
  Changes the the sio port baud rate.

===========================================================================*/
void bt_dc_sio_change_baud
(
  sio_bitrate_type    bit_rate,
  sio_ioctl_cmd_type  cmd_type
);


/*===========================================================================

FUNCTION
  bt_dc_update_sio_state

DESCRIPTION
  The SIO state is changed based on UART activity and directives from SOC.
  When the state is updated, SIO needs to be powered up and the SIO vote needs
  to change if SIO is not already in the right state.

===========================================================================*/
extern void bt_dc_update_sio_state
(
  bt_sio_state_enum_type new_sio_state,
  boolean                change_sio_vote
);

/*===========================================================================

FUNCTION
  bt_dc_vote_sio_sleep

DESCRIPTION
  When this function is called the following has already happened:
  - SOC sent YMS
  - uart TX has been flushed
  Now vote for SIO to sleep

===========================================================================*/
extern void bt_dc_vote_sio_sleep
(
  void
);

/*===========================================================================

FUNCTION
  bt_dc_power_on_soc

DESCRIPTION
  This routine powers on the BT Controller SoC.

===========================================================================*/
extern void bt_dc_power_on_soc( void );

/*===========================================================================

FUNCTION
  bt_dc_power_off_soc

DESCRIPTION
  This routine powers of the BT Controller SoC.

===========================================================================*/
extern void bt_dc_power_off_soc( void );


/*===========================================================================

MACRO   
  BT_DC_UPDATE_DRIVER_STATE

DESCRIPTION
  This macro sets the driver_state variable.

===========================================================================*/
#define BT_DC_UPDATE_DRIVER_STATE(new_state) { \
    BT_MSG_HIGH( "BT DC: Driver State Update - Old:%d New:%d",bt_dc.driver_state,new_state,0);\
    bt_dc.driver_state = new_state;}

#endif /* FEATURE_BT_SOC */

#endif /* FEATURE_BT */

#endif /* _BTI_H */


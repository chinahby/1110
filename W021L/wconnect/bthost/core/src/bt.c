/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     B L U E T O O T H     T A S K

GENERAL DESCRIPTION
  This module contains the Bluetooth task which controls the BT driver.

Copyright (c) 2000-2009 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/bt.c#4 $
  $DateTime: 2009/07/31 11:25:37 $
  $Author: c_bhatg $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-07-31   gb  Added support for FEATURE_IBT_DYNAMIC
  2009-05-12   pg  Changed to call RM API to process BT_DRIVER_IDLE_TIMER_SIG    
  2009-03-03   pg  Added call to bt_rm_soc_reset_shutdown().
  2008-10-27   sa  Added support to read BT 2.1 support from a nv item in 
                   NV browser at run-time.
  2008-09-03   gs  Modified command re-queue processing logic to send Cmd Done
                   event if MAX_CMD_RETRIES is reached or if error occurs.
  2008-08-14   rb  Added handling to keep the background timer tick during
                   power collapse.
  2008-08-14   sa  Added support for setting target specific default values for 
           nv items like ref clk, clk sharing and soc logging.
  2008-07-30   rs  BT task to wait for BT_SE_AUTH_TIMER_SIG only in non SOC 
                   builds
  2008-07-28   sa  Added support to read Clk type,Clk sharing,in-band sleep 
                   mode, soc logging enabled, BT Wlan co-ext, BTS power mgmt 
                   mode  from NV browser at run-time.
  2008-05-08   rs  Added API for dsm data memory alignment
  2008-04-16   jn  Modify VDD min API to be used only with out-of-band sleep.
                   Fix EOL issue with file formatting.
  2008-03-13   jn  Created APIs for allowing/not allowing MSM VDD minimization.
  2008-03-11  dsn  Add new edit history format and P4 version control string.
  2008-03-10   jn  Fix end of line issues in previous revision.
 
 *
 *    #137       07 Mar 2008            JN
 * BT modifications to enable MSM VDD minimization for 6246 and 6290. 
 *
 *    #136       28 Jan 2008            SSK
 * Change bt_qsoc_sleep_sm_eval_event() call so that we don't update BTQSOC
 * SlEEP state from a NON-BT task context
 *
 *    #135       22 Jan 2008            DM
 * Supress Enhanced Flushing by not running the packet tracking timer while 
 * in HCI mode
 *
 *    #134       23 Jan 2008            HM
 * Moved definition BT_CMD_MAX_RETRIES to bti.h
 * 
 *    #133       18 Jan 2008            PG
 * Resolved Lint errors in some target builds.
 * 
 *    #132       10 Jan 2008            SSK
 * Removing Inband's Sleep usage of SOC_SIG6(0x00800000)\BT_SOC_SIG5 0x80000000
 * since these are global signals used by RPC\rex.h 
 *
 *    #131       05 Dec 2007            AG
 * BT2.1 - changed enum type bt_encrypt_mode_type to take into account
 * new definition in 2.1 spec.
 *
 *    #130       29 Nov 2007            SSK
 * Implemented Inband Sleep mechanism under FEATURE_BT_QSOC_INBAND_SLEEP
 *
 *    #129       01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *    #128       19 Oct 2007            SSK
 * Add support for FCTL CMD type for handling DSM item level callbacks
 * during Controller to Host HCI Flow control.
 *
 *    #127       12 Sep 2007            BK
 * Added support for QSOC Driver State state machine.
 *
 *    #126       11 Sep 2007            VK
 * Added function to free command buffer.
 *
 *    #125       03 Aug 2007            VK
 * Increasing re-try count to 20 from 10.
 *
 *    #124       27 Jun 2007            DM
 * Changed featurization for AV-Sync in SOC based targets
 *
 *    #123       20 Jun 2007            SR
 * Use VREG_GP1 and VREG_WLAN for H6 FFA
 *
 *    #122       14 Jun 2007            PG
 * Fixed "Data Service WM full error" which causes a L2CAP packet to be
 * dropped resulting in the remote side disconnecting L2CAP.
 *
 *    #121       13 Apr 2007            PN
 * Fixed compile error for SC2X.
 *
 *    #120       13 Apr 2007            JN
 * For BTS (QSOC), re-queue HCI cmds if received before BTS is initialized.
 *
 *    #119       12 Apr 2007            GS
 * Updated to configure BT core wakeup interrupt to be edge triggered 
 * for MSM6800.
 *
 *    #118       04 Apr 2007            BH
 * Added event tracking for QSoC sleep support.
 *
 *    #117       29 Mar 2007            WFD
 * Add addition featurization for T_MSM7200 to:
 * - exclude calling bio_enable_bt()
 * - to call task_start() rather than mc_task_start()
 * - to call task_ack() rather than rex_set_sigs()
 *
 *    #116       14 Mar 2007            JN
 * Changed name of bt_soc_shutdown to bt_soc_shutdown_cleanup due to conflict
 * in function name in btsoc.c
 *
 *    #115       12 Mar 2007            GS
 * Updated bt_record_fault() to use safe string copy macro.
 *
 *    #114       28 Feb 2007            JN
 * Following mods were done:
 * - Added support for BTS4020.
 * - Changed SOC1 specific signal names to generic SOC signal name. Moved 
 *   SOC1 specific code to btsoc1.c
 * - Added support for run-time SOC switch, set as a NV item. 
 *
 *    #113       23 Feb 2007            DSN
 * Modifications to re-enable HCI mode for SoC solutions.
 *
 *    #112       20 Feb 2007            DSN
 * Modified event processing order for SOC solutions.
 *
 *    #111       20 Feb 2007            ACG
 * Once yms is received, either a flush command can be issued to sio or a 
 * timer is used to flush any data in the SIO tx buffer. The switch can 
 * be made by changing just one variable (bt_enable_flush_tx) in btsoc1.c.
 *
 *    #110       16 Feb 2007            PN
 * Made use of safe string functions. Fixed critical lint errors.
 *
 *    #109       07 Feb 2007            ACG
 * Enable Tx Flush IOCTL command and remove flush timer for SOC based solutions.
 *
 *    #108        5 Feb 2007            GS
 * Workaround to prevent phone from going into power save mode during
 * HCI_RESET on 6500/6550 targets.
 *
 *    #107       31 Jan 2007            ACG
 * Added a delay between receipt of yms and voting to sleep.
 *
 *    #104       19 Jan 2007            DSN
 * Removing a header file that is no longer used.
 *
 *    #103        9 Jan 2007            PG
 * Moved bt_soc_etc_powerup_init() from bt_dc_enable_driver()
 *
 *    #102        8 Jan 2007            ACG
 * Following features were added for the SOC solution:
 * - vote for SIO sleep/wakeup based on BT activity
 * - enable/disable driver
 * - radio on/off
 * 
 *    #101       22 Dec 2006            SSK
 * H4DS optimisation: Rename Signal handling functions
 * 
 * Fix bug in which acl_data send requests were not aware of the present
 * deep sleep state of the H4DS Engine.
 *
 *    #100       29 Nov 2006            ACG
 * Enable Vreg in BT task context to power the BT SOC.
 *
 *    #99        22 Nov 2006            DM
 * Added starting and stopping of Av-Sync Flush timer in soc init and shutdown
 *
 *    #98        22 Nov 2006            SSK
 * Setting BT Class of device to a default value.
 *
 *    #97        22 Nov 2006            ACG
 * Print err message during sleep wakeup outside intlock.
 *
 *    #96        16 Nov 2006            SSK
 * Rolling back temporary change for powering on External SOC on 7500.
 *
 *    #95        08 Nov 2006            SSK
 * Temporary change for powering on External SOC on 7500. 
 *
 *    #94        20 Oct 2006            SSK
 * External SOC suppport.
 *
 *    #93        19 Oct 2006            PN
 * Changed BREW SPP events dispatching mechanism.
 *
 *    #92        16 Oct 2006            DSN
 * Fixing compile errors for targets where FEATURE_BT_SOC or FEATURE_NEW_SLEEP_API
 * are defined.
 *
 *    #91        06 Oct 2006            DSN
 * Support new sleep voting apis using a registered sleep handle.
 *
 *    #90        20 Sep 2006            JH
 * Modified bt_enable_hardware and bt_disable_hardware to support 6550 MediaFLO 
 *  FFA.
 *
 *    #89        22 Aug 2006            DSN
 * HCI test mode support on SOC solution, new signal for SOC.
 *
 *    #88        14 Aug 2006            DSN
 * Including the correct header file.
 *
 *    #87        26 Jul 2006            DM
 * Added new timer for flushing l2cap packets which is required for AV-Sync
 *
 *    #86        27 Jun 2006            DSN
 * Added featurization for HostController SoC solution.
 *
 *    #85        25 Jun 2006            DSN
 * Removed Bonding App(BA) related references that are no longer used.
 *
 *    #84        25 May 2006            RY
 * Removed temporary clk_regime_enable fix for 6250 in
 * bt_enable_clocks_after_wakeup.  The real fix is in wsleep.c
 *
 *    #83        10 Apr 2006            JH
 * Added code for configuring BT wakeup interrupt to be edge triggered when
 *  BT_WAKEUP_EDGE_TRIGGERED is defined.
 *
 *    #82        16 Jan 2006            JH
 * Moved the code added in #80 to the else part of the #ifdef structure to
 *  avoid possible effect on other platforms where T_MSM_6100 is defined.
 *
 *    #81        26 Jan 2006            GS
 * Clear LM data during HCI reset.
 *
 *    #80        24 Jan 2006            RY
 * Added PMIC voting support for 6100 ZRF6155 FFA.
 *
 *    #79        21 Dec 2005            GS
 * Disable DSP sleep after forced BT core wakeup during image swap. Fixes
 * image swap handling issue in Sniff mode.
 *
 *    #78        05 Dec 2005            JH
 * Added call to bt_lm_background_tick(() when background timer expires.
 *
 *    #77        11 Oct 2005            PN
 * Fixed problem of Audio debug not re-starting and Audio debug data getting 
 * dropped.
 *
 *    #76        23 Sep 2005            PG
 * Enabled calls to bio_enable_bt() for 6275 to resolve higher
 * rock bottom current issue in 6275.
 *
 *    #75        12 Sep 2005            JTL
 * Improving DSM allocation, and adding BT versions of some DSM functions.
 *
 *    #74        31 Aug 2005            PN
 * Improved audio debug.
 *
 *    #73        30 Aug 2005            GS
 * Suppressing safe LMP timer sig processing debug messages if BT 1.2 feature 
 * is enabled.
 *
 *    #72        03 Aug 2005            GS
 * Mainlined mDSP reset handling functionality for UMTS targets.
 *
 *    #71        03 Aug 2005            GS
 * Clear all security settings during HCI reset.
 *
 *    #70        28 Jul 2005            GS
 * Now really fixing incorrect usage of HWIO_OUTM macro.
 *
 *    #69        28 Jul 2005            GS
 * Fixing incorrect usage of HWIO_OUTM macro.
 *
 *    #68        25 Jul 2005            GS
 * Added support for HCI over DIAG (ASIA).  Send HCI reset cmd done event
 * even in HCI over FTM/DIAG mode.
 *
 *    #67        16 Jun 2005            DP
 * Added LMP 2 timer for control of ACL packet type changes.
 *
 *    #66        03  Jun 2005           GS
 * Backing out force BT core wakeup support for 6275 due to compile issues.
 *
 *    #65        02  Jun 2005           GS
 * Added support for 6800 and 6275 force BT core wakeup through register write.
 *
 *    #64        01  Jun 2005           GS
 * Added support for improved HCI reset functionality.  Don't reset the phone.
 * Instead, re-init driver lower layers, unload and redownload BT DSP image.
 *
 *    #63        31  May 2005           RY
 * Merged in Sirius changes.
 *
 *    #62        16  May 2005           PG
 * LMP and SE_AUTH timers are now processed only when DSP clocks are running.
 *
 *    #61        2  May 2005            JH
 * Modified bt_remove_hc_commands_from_q() to use q_linear_search() and
 *  q_delete() since q_linear_delete() does not work.
 *
 *    #60        29 Apr 2005            GS
 * Modifed bt_wakeup_isr to send wakeup command to DSP if the interrupt occurs
 * during image reset handling.
 *
 *    #59        09 Apr 2005            GS
 * Added support for MSM6800.
 *
 *    #58        7  Apr 2005            DP
 * Commands are now retried for the full max-retry time interval.
 *
 *    #57        06 Apr 2005            JH
 * Adding support for disabling BT radio, featurized using BT_SWDEV_RADIO_OFF.
 *
 *    #56        24 Mar 2005            JTL
 * Adding A2DP top layer for DSMs.
 *
 *    #55        24 Mar 2005            RY
 * Added support for MSM6275.
 *
 *    #54        04 Mar 2005            JTL
 * Increasing size of cmd/evt buffers if FEATURE_BT_EXTPF is defined.
 *
 *    #51        27 Jan 2005            PN
 * Fixed audio debug code compile problem when SND_TEST_CODE is not defined.
 *
 *    #50        21 Jan 2005            PN
 * Improved audio debug code.
 *
 *    #49        7  Dec 2004            JTL
 * Adding SIO command queue.
 *
 *    #48        1  Dec 2004            GS
 * Featurized 'force bt core wakeup' during image swap.
 *
 *    #47        30  Nov 2004           GS
 * Fixed mdsp reset handling during image swap.
 *
 *    #46        5  Nov 2004            PN
 * Added support for audio debug.
 *
 *    #45        27 Nov 2004            JL
 * Fixing DSP clock problems.
 *
 *    #44        25 Oct 2004            GS
 * Added EFS param toggle_seqn_bit.
 *
 *    #43        5  Oct 2004            DP
 * Added PMIC voting code to 6100 Tsunami FFA.
 *
 *    #42        24 Sep 2004            DP
 * Improved sleep debug messages.
 *
 *    #39        14 Sep 2004            PN
 * Replaced OEMBTExtSPP.h with OEMBTExtSPP_priv.h.
 *
 *    #38        02 Sep 2004            JTL
 * Adding event handler processing for BREW SPP events if FEATURE_IBT is
 * defined.
 *
 *    #37        06 Aug 2004            JTL
 * Increasing cmd/evt queue if FEATURE_BT_EXTPF is defined.
 *
 *    #36        22 Jul 2004            RY
 * Updated bt_wakeup_isr to enable code that waits for IRQ to deassert.
 * This is not applicable to UMTS targets.
 *
 *    #35        15 Jul 2004            RY
 * Enabled/disabled the sleep clock regime to enable BT sleep on the 6500.
 *
 *    #34        21 Jun 2004            RY
 * Enabled/disabled BT GPIOs appropriately for 6250 power optimization.
 *
 *    #33        9  Jun PDT             RY
 * Removed CPLL power on for 6250.
 *
 *    #32        1  Jun 2004            DP
 * Implemented Mode 2 security and AG External I/O features.
 *
 *    #31        8  Apr 2004            PN
 * Added call to bt_ag_background_tick() in bt_task().
 *
 *    #30        2  Apr 2004            RY
 * Extended PMIC changes in #28 to 6500.
 *
 *    #29        26 Mar 2004            DP
 * Fixed code to keep track of whether DSP is asleep.
 * Relocated HW disable/enable/init routines to remove predeclaration.
 *
 *    #28        23 Mar 2004            RY
 * Updated bt_enable_hardware and bt_disable_hardware to enable/disable the
 * voltage regulator for 6250 FFA.  This is needed to remove the dependency
 * on MMC.
 *
 *    #27        23 Feb 2004            RY
 * 1. Updated bt_enable_clocks to call clk_regime_enable instead of writing
 *    directly to the CLK_EN registers.
 * 2. Updated bt_enable_hardware to clear the clock gating bit in the clock
 *    block in order to force the DMA clock on for 6250.
 * 3. Updated bt_task to fix a problem in sending data while the BT DSP
 *    could be in sleep.
 *
 *    #25        19 Feb 2004            DP
 * Added APIT SIO RX data event.
 *
 *    #24        29 Jan 2004            DP
 * Fixed EFS params initializations.
 *
 *    #23        16 Dec 2003            RY
 * Added comments.
 *
 *    #22        16 Dec 2003            RY
 * Updated bt_enable_clocks to enable the clocks necessary to support BT on
 * MSM6250.
 *
 *    #21        12 Dec 2003            DP
 * Process re-queued commands prior to regular commands every time.
 * Added background tick for BT SIO.
 *
 *    #20        7  Nov 2003            RY
 * Updated bt_disable_clocks to NOT disable the SBI clock.
 *
 *    #19        5  Nov 2003            DP
 * Fixed compilation error when test API not enabled and added this comment
 * omitted on previous revision #18.
 *
 *    #17        4  Nov 2003            DP
 * General cleanup.
 *
 *    #16        4  Nov 2003            RY
 * Disabled the RFU when the driver is going to idle.
 *
 *    #15        3  Oct 2003            DP
 * Converted RM timer to background timer for entire driver.  Added
 * RFCOMM peer to peer communications monitoring.
 *
 *    #14        12 Sep 2003            RY
 * Added MSM6500 support.
 *
 *    #13        26 Aug 2003            RY
 * Updated to handle the new RDM_CLOSE signal and set HCI mode to OFF.
 *
 *    #12        22 Jul 2003            RY
 * 1. Updated to generate the HCI Command Complete Status event upon boot
 *    if FEATURE_BT_TEST_API is defined and BT has a valid SIO port.
 * 2. Updated bt_efs_init to initialize additional LMP parameters.
 *
 *    Rev 1.55   13 Nov 2002 13:56:26   waynelee
 * Use sleep_bt_clock_rgm for 5200 targets.  Enable CPLL when turning
 * on clocks for 5200 targets.  Always OKTS -- previous fix for 6100A
 * was not correct.
 *
 *    Rev 1.54   28 Sep 2002 15:10:28   waynelee
 * Enable SBI and MDSP clocks when BT is enabled (for 6100)
 *
 *    Rev 1.53   25 Sep 2002 11:26:10   waynelee
 * externed QDSP poll timer.  Check to report RX'd bytes.
 *
 *    Rev 1.52   10 Sep 2002 17:38:56   waynelee
 * Added support for new task init/start mechanism.
 *
 *    Rev 1.51   04 Sep 2002 11:33:12   waynelee
 * Removed temp no mdsp svc code.  Removed never sleep feature.
 *
 *    Rev 1.50   09 Aug 2002 18:53:38   waynelee
 * Added changes for 5100-BTRF build
 *
 *    Rev 1.49   09 Aug 2002 18:36:22   waynelee
 * Added MSM6100 support.  Not using MDSP services.
 *
 *    Rev 1.48   07 Jun 2002 17:02:16   waynelee
 * Added code to compile for MSM6200 targets.
 *
 *    Rev 1.47   12 Apr 2002 11:08:42   propach
 * Made use of RFCOMM and L2CAP overhead #defines.
 *
 *    Rev 1.46   08 Apr 2002 15:50:18   waynelee
 * Ifdef'd out QDSP polling code with BT_SWDEV_QDSP_POLLING.
 *

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#include <string.h>
#include "clkregim.h"
#include "dog.h"
#include "fs.h"
#if (!defined(T_MSM6100) && !defined(T_MSM7200)) 
#error code not present
#endif
#include "nv.h"
#include "queue.h"
#include "rex.h"
#include "sio.h"
#include "sleep.h"
#include "task.h"

#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "bthc.h"
#include "bthcpe.h"
#include "btl2.h"
#include "btmsg.h"
#include "btrc.h"
#include "btrm.h"
#include "btsd.h"
#include "btsio.h"
#include "btutils.h"

#ifndef FEATURE_BT_SOC 
#include "btbb.h"
#include "bthcetc.h"
#include "bthcev.h"
#include "btqddata.h"
#include "btqdspq.h"
#include "btse.h"
#else
#include "bthcdrv.h"
#include "clk.h"
#include "btsocetc.h"
#include "btsoc.h"
#include "btqsocnvm.h"
#endif /* FEATURE_BT_SOC */

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

#ifdef FEATURE_BT_QSOC
#include "btqsoc.h"
#ifdef FEATURE_BT_QSOC_SLEEP
#include "btqsocsleep.h"
#endif /* FEATURE_BT_QSOC_SLEEP */
#endif /* FEATURE_BT_QSOC */

#ifdef FEATURE_FACTORY_TESTMODE
#include "btdiag.h"
#endif

#ifdef FEATURE_BT_AG
  #include "btag.h"
#ifdef FEATURE_BT_ICON
#error code not present
#endif
#endif

#ifdef BT_SWDEV_QXDM_LOG_PER
#include "log.h"
#include "btlogi.h"
#endif

#if defined( T_MSM33 ) && !defined( T_MSM6100 )
#error code not present
#endif

#ifdef T_MSM6200
#error code not present
#endif

#ifdef FEATURE_BT_TEST_MODE_ONLY
#include "hs.h"
#include "lcd.h"
#endif

#ifdef BT_QDSP_DEBUG
#error code not present
#endif

#ifdef FEATURE_PMIC
#include "pm.h"
#endif

#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#endif /* FEATURE_RUNTIME_DEVMAP */

#ifdef BT_AUDIO_DEBUG
#error code not present
#endif /* BT_AUDIO_DEBUG */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_MSG_LAYER BT_MSG_GN

#define BT_REQUEUED_CMD_Q_TIMER_MS 500


#ifdef BT_QDSP_DEBUG
#error code not present
#endif /*  BT_QDSP_DEBUG */


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_BT_SOC
  bt_cod_type           bt_class_of_device = 
  {
    0x04, 0x02, 0x40
  };

  /* Revisit - this is only for MSM7500 bringup. */
  bt_bd_addr_type  bt_my_bd_addr =
  {
    0x34, 0x12, 0x78, 0x56, 0xBC, 0x9A
  };
  bt_bd_addr_type*        bt_qd_my_bd_addr_ptr;
  
#endif /* FEATURE_BT_SOC */

#ifdef FEATURE_BT_EXTPF
#  define BT_NUM_CMDS     150
#else
#  define BT_NUM_CMDS     100
#endif
LOCAL   bt_cmd_msg_type bt_cmds[ BT_NUM_CMDS ];
LOCAL   q_type          bt_cmd_q;
LOCAL   q_type          bt_cmd_re_q;
LOCAL   q_type          bt_cmd_free_q;

#ifdef FEATURE_BT_EXTPF
#  define BT_NUM_EVENTS   150
#else
#  define BT_NUM_EVENTS   100
#endif
LOCAL   bt_ev_msg_type  bt_events[ BT_NUM_EVENTS ];
        q_type          bt_event_free_q;

LOCAL uint16  bt_event_q_mask;

dsm_watermark_type  bt_from_sio_wm;  /*  Data for BT Task / SIO  */
dsm_watermark_type  bt_to_sio_wm;    /*    communication.        */
q_type              bt_to_sio_q;
q_type              bt_from_sio_q;
#ifdef FEATURE_BT_SOC 
/* Sio watermarks and Queues for MSM to HC SOC communication */
dsm_watermark_type  bt_soc_to_msm_sio_wm;  /*  HC SOC to MSM sio watermark */
dsm_watermark_type  bt_msm_to_soc_sio_wm;  /*  MSM to HC SOC sio watermark */
q_type              bt_soc_to_msm_sio_q;   /*  HC SOC to MSM sio queue */
q_type              bt_msm_to_soc_sio_q;   /*  MSM to HC SOC sio queue */
#endif /* FEATURE_BT_SOC */

#ifdef FEATURE_NEW_SLEEP_API 
/* BT okay to sleep handle */
LOCAL sleep_okts_handle bt_okts_handle;
#endif /* FEATURE_NEW_SLEEP_API  */

bt_sleep_data_type  bt_sleep_data;

/* Watchdog report timeout */
LOCAL rex_timer_type  bt_rpt_timer;

/* QDSP Poll timer */
rex_timer_type  bt_qdsp_poll_timer;

/* Requeued command q timeout */
LOCAL rex_timer_type  bt_requeued_cmd_q_timer;

/* Timer to turn off Bluetooth clocks. */
LOCAL rex_timer_type  bt_clocks_monitor_timer;

LOCAL rex_timer_type  bt_background_timer;

rex_timer_type  bt_wait_timer;

/* Timers in this group can run during power collapse */
timer_group_type  bt_always_on_timer_group;

/* This timer will be defined in bt_always_on_timer_group */
/* and hence will run during processor power collapse     */
LOCAL rex_timer_type  bt_persistent_background_timer;

#ifdef FEATURE_BT_L2FLUSH
/* Timer to check if l2cap packets needs to be flushed */
rex_timer_type  bt_l2flush_tick_timer;
#endif

#ifdef BT_SWDEV_QXDM_LOG_PER
/* DM timer */
LOCAL rex_timer_type      bt_dm_timer;
#endif

#ifdef BT_QDSP_DEBUG
#error code not present
#endif

#ifdef BT_AUDIO_DEBUG
#error code not present
#endif /* BT_AUDIO_DEBUG */

LOCAL const rex_sigs_type bt_wait_sig_mask = (
  BT_TASK_START_SIG             |
  BT_TASK_STOP_SIG              |
  BT_TASK_OFFLINE_SIG           |
#ifdef FEATURE_BT_AG
  BT_AG_RX_DATA_SIG             |
  BT_AG_TIMER_SIG               |
#endif /* FEATURE_BT_AG */
  BT_CMD_Q_SIG                  |
#ifdef FEATURE_BT_SOC
  BT_SOC_SIG1                   |
  BT_SOC_SIG2                   |
  BT_SOC_SIG3                   |  
  BT_SOC_SIG4                   |  
#else
  BT_QDSP_EVENT_Q_SIG           |
  BT_QDSP_READY_SIG             |
  BT_QDSP_RESET_SIG             |  
  BT_QDSP_STANDBY_SIG           |  
#endif /* FEATURE_BT_SOC */
#ifdef FEATURE_BT_SOC
  BT_SIO_SOC_RX_SIG             |
#else
  #ifdef BT_SWDEV_QDSP_POLLING
    BT_QDSP_POLL_SIG            |
  #endif /* BT_SWDEV_QDSP_POLLING */
#endif /* FEATURE_BT_SOC */
  BT_EVENT_Q_SIG                |
  BT_SD_TIMER_SIG               |
  BT_RC_TX_TIMER_SIG            |
  BT_RPT_TIMER_SIG              |
  BT_SIO_RX_SIG                 |
#ifdef BT_SWDEV_QXDM_LOG_PER
  BT_DM_TIMER_SIG               |
#endif
#ifndef FEATURE_BT_SOC
  BT_SE_AUTH_TIMER_SIG          |
#endif /* FEATURE_BT_SOC */
  BT_BACKGROUND_TIMER_SIG       |
  BT_L2_SIG_MSG_RESP_TIMER_SIG  |
  BT_REQUEUED_CMD_Q_SIG         |
  BT_CLOCKS_SHUTDOWN_SIG        |
  BT_RDM_SIG                    |
#ifdef BT_AUDIO_DEBUG
#error code not present
#endif /* BT_AUDIO_DEBUG */
#ifdef FEATURE_BT_SOC
  BT_DRIVER_IDLE_TIMER_SIG      |
  BT_SIO_TX_FLUSH_WAIT_SIG
#else
  BT_LMP_TIMER_SIG              |
  BT_LMP_TIMER_2_SIG
#endif
 );

/*  To save baseband timer sigs if they must be  */
/*  put off until it is safe to handle them.     */
LOCAL rex_sigs_type bt_pending_bb_timer_sigs = 0; 

bt_efs_params_type bt_efs_params;

/*-------------------------------------------------------------------------*/
/*  BT Driver vital statistics and internal fault table definitions.       */
/*-------------------------------------------------------------------------*/

#define BT_FAULTS_MAX 10

#define BT_MAX_FILENAME_LEN 13
typedef struct
{
  char    file_name[ BT_MAX_FILENAME_LEN ];
  uint16  line_num;
  uint32  value1;
  uint32  value2;
  uint32  value3;
} bt_fault_table_type;

uint16               bt_fault_count;
bt_fault_table_type  bt_fault_table[ (BT_FAULTS_MAX + 1) ];

uint16  bt_vital_stats[ BT_VITAL_STAT_COUNT ];

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

extern void bt_l2_soc_reset_shutdown( void );
extern void bt_rc_soc_reset_shutdown( void );
extern void bt_sd_soc_reset_shutdown( void );
extern void bt_ag_soc_reset_shutdown( void );
extern void bt_rm_soc_reset_shutdown( void );
extern boolean bt_rm_persistent_tick_required( void );
extern void bt_rm_proc_driver_idle_to( void );

/*==========================================================================

FUNCTION       
  BT_ASSERT_OKTS

DESCRIPTION    
  This function informs the sleep module that Bluetooth is okay to sleep.

DEPENDENCIES   
  If using the registry mechanism, the Bluetooth okay to sleep handle 
  must be valid. The okay to sleep handle is obtained by registering with
  sleep module using a call to sleep_register() at BT task start-up

INPUTS         
  None.

RETURN VALUE   
  None.

SIDE EFFECTS   
  Bluetooth sleep vote is set to TRUE.

==========================================================================*/
void bt_assert_okts( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_NEW_SLEEP_API 
  /* Use registry mechanism. Vote okay to sleep using the registered handle */
  sleep_assert_okts( bt_okts_handle );
#else
  /* Use sleep task signal mechanism. Set the BT okay to sleep signal */
  SLEEP_ASSERT_OKTS( SLEEP_BT_OKTS_SIG );
#endif /* FEATURE_NEW_SLEEP_API  */

} /* bt_assert_okts */


/*==========================================================================

FUNCTION       
  BT_NEGATE_OKTS

DESCRIPTION    
  This function informs the sleep module that Bluetooth is NOT okay to sleep.

DEPENDENCIES   
  If using the registry mechanism, the Bluetooth okay to sleep handle 
  must be valid. The okay to sleep handle (bt_okts_handle) is obtained by 
  registering with sleep module using a call to sleep_register() at BT task 
  start-up.

INPUTS         
  None.

RETURN VALUE   
  None.

SIDE EFFECTS   
  Bluetooth sleep vote is set to FALSE.

==========================================================================*/
void bt_negate_okts( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_NEW_SLEEP_API 
  /* Use registry mechanism. Vote NOT okay to sleep using the registered 
     handle */
  sleep_negate_okts( bt_okts_handle );
#else
  /* Use sleep task signal mechanism. Clear the BT okay to sleep signal */
  SLEEP_NEGATE_OKTS( SLEEP_BT_OKTS_SIG );
#endif /* FEATURE_NEW_SLEEP_API  */

} /* bt_negate_okts */


/*==========================================================================

FUNCTION       
  BT_SET_VDD_MIN_ALLOWED

DESCRIPTION    
  This function informs the sleep module that Bluetooth is okay for MSM VDD 
  minimization.

DEPENDENCIES   
  Sleep handle must be valid. 

INPUTS         
  None.

RETURN VALUE   
  None.

SIDE EFFECTS   
  Bluetooth sleep VDD minimization vote is set to TRUE.

==========================================================================*/
void bt_set_vdd_min_allowed
(
  void
)
{
  /* Current this is avaailable for 6290 and 6246. This must be replaced
     by sleep feature
   */
#if (defined(T_MSM6246) || defined(T_MSM6290))  && \
    !(defined(FEATURE_BT_QSOC_INBAND_SLEEP))
  /* Vote for VDD minimization */
  sleep_set_vdd_min_restriction(bt_okts_handle, VDD_MIN_ALLOWED);
#endif /* (T_MSM6246 || T_MSM6290) && !FEATURE_BT_QSOC_INBAND_SLEEP */
} /* bt_set_vdd_min_allowed */


/*==========================================================================

FUNCTION       
  BT_SET_VDD_MIN_NOT_ALLOWED

DESCRIPTION    
  This function informs the sleep module that Bluetooth is not okay for MSM 
  VDD minimization.

DEPENDENCIES   
  Sleep handle must be valid. 

INPUTS         
  None.

RETURN VALUE   
  None.

SIDE EFFECTS   
  Bluetooth sleep VDD minimization vote is set to FALSE.

==========================================================================*/
void bt_set_vdd_min_not_allowed
(
  void
)
{
  /* Current this is avaailable for 6290 and 6246. This must be replaced
     by sleep feature 
   */
#if (defined(T_MSM6246) || defined(T_MSM6290))  && \
    !(defined(FEATURE_BT_QSOC_INBAND_SLEEP))
  /* Vote against VDD minimization */
  sleep_set_vdd_min_restriction(bt_okts_handle, VDD_MIN_NOT_ALLOWED);
#endif /* (T_MSM6246 || T_MSM6290) && !FEATURE_BT_QSOC_INBAND_SLEEP */
} /* bt_set_vdd_min_not_allowed */


#ifndef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_wakeup_isr

DESCRIPTION
  ISR to execute when BT wakeup interrupt is triggered.

===========================================================================*/
LOCAL void bt_wakeup_isr
(
  void
)
{
  boolean bt_sleep_err = FALSE;

  /*  The BT task also changes the clock enables and sleep     */
  /*  state.  Must lock interrupts to keep this data in sync.  */
  INTLOCK();

  /*  Increment count indicating a BT wakeup ISR has been processed.  */
  bt_sleep_data.wakeup_isr_cnt++;

  /*  Enable clocks now that we are awake.  */
  bt_enable_clocks_after_wakeup();

  /*  Place the driver in the ready state  */
  /*  now that the clocks are enabled.     */
  if ( bt_dc.driver_state == BT_DS_DSP_SLEEPING )
  {
    bt_dc.driver_state = BT_DS_READY;
  }
  else
  {
    bt_sleep_err = TRUE;
  }

  INTFREE();

  if ( bt_sleep_err ) 
  {
    BT_ERR( "BT SLEEP: WakeupISR Unexp ST %x WUI %x RFS %x",
      bt_dc.driver_state, bt_sleep_data.wakeup_isr_cnt,
      bt_sleep_data.return_from_sleep_ev_cnt );
  }

  if ( bt_dc.driver_state == BT_DS_READY || BT_QDSP_SWAPPING_OUT()) 
  {
    if ( bt_pending_bb_timer_sigs != 0 )
    {
      BT_MSG_DEBUG ( "BT: Disabled DSP sleep. Pend BB Tmr sigs %x",
                     bt_pending_bb_timer_sigs, 0, 0 );

      /*  Prevent DSP from sleeping until the  */
      /*  saved bt timer sigs are processed.   */
      *bt_qd_bt_enable_sleep_mode_ptr = BT_QD_DISABLE_SLEEP_MODE;
      (void) rex_set_sigs( &bt_tcb, bt_pending_bb_timer_sigs ) ;
    }

#if defined(T_MSM6300) && defined(FEATURE_GSM)
#error code not present
#endif
    
    bt_bb_send_qdsp_cmd( BT_QDSP_GOTO_WAKEUP_CMD, BT_QDSP_GEN_IDX );
    bt_sleep_data.wakeup_cmd_cnt++;
  
    /*  Now that we have woken up, check both the   */
    /*  QDSP event queue and BT command queues for  */
    /*  events/commands that have been pending.     */
    (void) rex_set_sigs( &bt_tcb, BT_QDSP_EVENT_Q_SIG |
                                  BT_CMD_Q_SIG );
  }

#ifdef FEATURE_REX_PREEMPT_INTS
  /*  If pre-emptive interrupts are installed, then    */
  /*  need to re-enable the unexpected clk interrupt.  */
  clk_unexpected_enable();
#endif

  if ( bt_sleep_data.wakeup_isr_cnt != bt_sleep_data.wakeup_cmd_cnt )
  {
    BT_ERR( "BT SLEEP: WakeupISR Bad Cnts WUI %x WCC %x",
            bt_sleep_data.wakeup_isr_cnt,
            bt_sleep_data.wakeup_cmd_cnt, 0 );
  }

#ifdef BT_SLEEP_DEBUG
  BT_MSG_DEBUG( "BT SLEEP: WakeupISR WUI %x RFS %x ST %x",
                bt_sleep_data.wakeup_isr_cnt,
                bt_sleep_data.return_from_sleep_ev_cnt,
                bt_dc.driver_state );
#endif

  /*  Wait until interrupt is deasserted before exiting  */
  /*  to ensure we do not call this ISR more than once   */
  /*  per interrupt when the interrupt is driven by a    */
  /*  slow clock.                                        */
  /*  This is not needed if the interrupt is configured  */
  /*  to be edge-sensitive as opposed to level-sensitive.*/
#if defined( T_MSM6100 ) && !defined( BT_WAKEUP_EDGE_TRIGGERED )
  tramp_block_till_deasserted( TRAMP_BT_WAKEUP_ISR, 30 );
#endif

}


/*===========================================================================

FUNCTION
  bt_enable_clocks

DESCRIPTION
  Enable all clocks used for BT.

===========================================================================*/
void bt_enable_clocks( void )
{

#if defined( T_MSM6100 )

  clk_regime_enable( CLK_RGM_BT_M );
  clk_regime_enable( CLK_RGM_SBI_M );

#elif defined( T_MSM_5100 )
#error code not present
#elif defined( T_MSM6250 )
#error code not present
#elif defined( T_MSM6200 )
#error code not present
#endif

#if defined( T_MSM6500 )
  clk_regime_enable( CLK_RGM_BT_SLP_M );
#endif

  /* Prevent TCXO from shutting down. */
  sleep_bt_clock_rgm = TRUE;

  /* GSM does not use sleep_bt_clock_rgm, vote that BT is NOT okay to sleep */
  bt_negate_okts();

}

/*===========================================================================

FUNCTION
  bt_disable_clocks

DESCRIPTION
  Disable all clocks used for BT.

===========================================================================*/
void bt_disable_clocks( void )
{

#if defined( T_MSM6100 ) || defined( T_MSM_5100 ) || defined( T_MSM6250 )

    clk_regime_disable( CLK_RGM_BT_M );

#elif defined( T_MSM6200 )
#error code not present
#endif

#if defined( T_MSM6500 )
  clk_regime_disable( CLK_RGM_BT_SLP_M );
#endif

  /* Allow TCXO to shutdown. */
  sleep_bt_clock_rgm = FALSE;

  /* GSM does not use sleep_bt_clock_rgm, vote that BT is okay to sleep */
  bt_assert_okts();

}

/*===========================================================================

FUNCTION
  bt_enable_clocks_after_wakeup

DESCRIPTION
  Enable clocks now BT has returned from sleep.

===========================================================================*/
void bt_enable_clocks_after_wakeup( void )
{

  /* Prevent TCXO from being disabled. */
  sleep_bt_clock_rgm = TRUE;

  /* GSM does not use sleep_bt_clock_rgm, vote that BT is NOT okay to sleep */
  bt_negate_okts();

#ifdef T_MSM6250
#error code not present
#elif defined( T_MSM6200 )
#error code not present
#endif

#if defined( T_MSM6200 ) || defined( T_MSM6100 )
  /* Turn on the MDSP clock. */
  mdsp_dsp_wakeup( MDSP_APP_BT );
#endif

}

/*===========================================================================

FUNCTION
  bt_disable_clocks_for_sleep

DESCRIPTION
  Disable clocks for when BT is sleeping.

===========================================================================*/
void bt_disable_clocks_for_sleep( void )
{

  /* Allow TCXO to be disabled. */
  sleep_bt_clock_rgm = FALSE;

  /* GSM does not use sleep_bt_clock_rgm, so set BT_OKTS_SIG */
  bt_assert_okts();

#if defined( T_MSM6200 ) || defined( T_MSM6100 )
  /* Allow the MDSP clock to be disabled. */
  mdsp_dsp_sleep( MDSP_APP_BT );
#endif

}
#endif /* FEATURE_BT_SOC */


/*===========================================================================

FUNCTION
  bt_initialize_cmd_buffer

DESCRIPTION
  Initializes a specified BT command buffer.

===========================================================================*/
LOCAL void bt_initialize_cmd_buffer
(
  bt_cmd_msg_type*  bt_cmd_ptr
)
{

  bt_cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_NULL;
  bt_cmd_ptr->cmd_hdr.bt_app_id   = BT_APP_ID_NULL;
  bt_cmd_ptr->cmd_hdr.cmd_state   = BT_CST_DONE;
  bt_cmd_ptr->cmd_hdr.cmd_status  = BT_CS_GN_UNRECOGNIZED_CMD;
  bt_cmd_ptr->cmd_hdr.cmd_retries = 0;

}


/*===========================================================================

FUNCTION
  bt_free_any_dsms_in_event

DESCRIPTION
  Frees any DSMs included within a specific BT event.

===========================================================================*/
void bt_free_any_dsms_in_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{

  dsm_item_type*  dsm_ptr = NULL;

  switch ( bt_ev_ptr->ev_hdr.ev_type )
  {
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
    case BT_EV_L2_RX_DATA:
      {
        dsm_ptr = bt_ev_ptr->ev_msg.ev_l2_rxd.dsm_ptr;
        break;
      }
    case BT_EV_RM_RX_DATA_ACL:
      {
        dsm_ptr = bt_ev_ptr->ev_msg.ev_rm_rxda.dsm_ptr;
        break;
      }
    case BT_EV_RM_RX_DATA_SCO:
      {
        dsm_ptr = bt_ev_ptr->ev_msg.ev_rm_rxds.dsm_ptr;
        break;
      }
    case BT_EV_HC_COMMAND_COMPLETE:
      {
        switch ( bt_ev_ptr->ev_msg.ev_hc_cmd_cmplt.cmd_opcode )
        {
          case BT_HCI_RD_LOCAL_NAME:
            {
              dsm_ptr = bt_ev_ptr->ev_msg.ev_hc_cmd_cmplt.
                          ret_params.rd_local_name.dsm_ptr;
              break;
            }
          case BT_HCI_RD_CURRENT_IAC_LAP:
            {
              dsm_ptr = bt_ev_ptr->ev_msg.ev_hc_cmd_cmplt.
                          ret_params.rd_curr_iac_lap.dsm_ptr;
              break;
            }
        }
        break;
      }
    case BT_EV_HC_RX_ACL_DATA:
      {
        dsm_ptr = bt_ev_ptr->ev_msg.ev_hc_rxda.dsm_ptr;
        break;
      }
    case BT_EV_HC_RX_SCO_DATA:
      {
        dsm_ptr = bt_ev_ptr->ev_msg.ev_hc_rxds.dsm_ptr;
        break;
      }
    case BT_EV_HC_INQUIRY_RESULT:
      {
        dsm_ptr = bt_ev_ptr->ev_msg.ev_hc_inqrs.dsm_ptr;
        break;
      }
    case BT_EV_HC_REMOTE_NAME_REQ_COMPLETE:
      {
        dsm_ptr = bt_ev_ptr->ev_msg.ev_hc_rname.dsm_ptr;
        break;
      }
    case BT_EV_HC_NUM_CMPLT_PKTS:
      {
        dsm_ptr = bt_ev_ptr->ev_msg.ev_hc_numpk.dsm_ptr;
        break;
      }
    case BT_EV_HC_RETURN_LINK_KEYS:
      {
        dsm_ptr = bt_ev_ptr->ev_msg.ev_hc_retky.dsm_ptr;
        break;
      }
  }

  if ( dsm_ptr != NULL )
  {
    dsm_free_packet( &dsm_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_task_bt_event_handler

DESCRIPTION
  Handles a specified BT event sent to the BT task.

===========================================================================*/
LOCAL void bt_task_bt_event_handler
(
  bt_ev_msg_type*  bt_ev_ptr
)
{

  BT_ERR( "BT Task: Unexp Event %x", bt_ev_ptr->ev_hdr.ev_type, 0, 0 );

  bt_free_any_dsms_in_event( bt_ev_ptr );

}


/*===========================================================================

FUNCTION
  bt_read_lisbon_disabled_from_nv

DESCRIPTION
  Read NV_BT_LISBON_DISABLED_I from non-volatile memory.

===========================================================================*/
LOCAL void bt_read_lisbon_disabled_from_nv
(
  void
)
{
  /* 1= BT 2.1 is disabled, 0= BT 2.1 not disabled*/    
  uint8  bt_2_1_lisbon_disabled = 0;  
  nv_cmd_type   rd_lisbon_disabled_nv_cmd;
  nv_item_type  nv_item;

  /* Read LISBON_DISABLED from NV */
  rd_lisbon_disabled_nv_cmd.item       = NV_BT_LISBON_DISABLED_I;
  rd_lisbon_disabled_nv_cmd.cmd        = NV_READ_F;
  rd_lisbon_disabled_nv_cmd.data_ptr   = &nv_item;
  rd_lisbon_disabled_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_lisbon_disabled_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_lisbon_disabled_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_lisbon_disabled_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( rd_lisbon_disabled_nv_cmd.status == NV_DONE_S )
  {
    BT_MSG_DEBUG( "BT: NV_BT_DISABLED_I: %u", nv_item.bt_lisbon_disabled, 0, 0 );
    bt_2_1_lisbon_disabled = nv_item.bt_lisbon_disabled;
  }
  else
  {   
    BT_MSG_DEBUG( "BT: Unable to read NV_BT_DISABLED_I", 0, 0, 0 );
#ifdef FEATURE_BT_2_1
    bt_2_1_lisbon_disabled = 0;
#else    
    bt_2_1_lisbon_disabled = 1;
#endif    
  }
  bt_cmd_dc_set_bt_2_1_lisbon_status( &bt_2_1_lisbon_disabled ); 
} 


#ifdef FEATURE_BT_SOC 
#ifdef BT_SWDEV_BT_DISABLED_NV
/*===========================================================================

FUNCTION
  bt_read_is_bt_disabled_from_nv

DESCRIPTION
  Read NV_BT_DISABLED_I from non-volatile memory.

===========================================================================*/
LOCAL void bt_read_is_bt_disabled_from_nv
(
  void
)
{

  nv_cmd_type   rd_bt_disabled_nv_cmd;
  nv_item_type  nv_item;

  bt_dc.is_bt_disabled = FALSE;

  /* Read BT_DISABLED from NV */
  rd_bt_disabled_nv_cmd.item       = NV_BT_DISABLED_I;
  rd_bt_disabled_nv_cmd.cmd        = NV_READ_F;
  rd_bt_disabled_nv_cmd.data_ptr   = &nv_item;
  rd_bt_disabled_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_bt_disabled_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_bt_disabled_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_bt_disabled_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( rd_bt_disabled_nv_cmd.status == NV_DONE_S )
  {
    BT_MSG_DEBUG( "BT: NV_BT_DISABLED_I: %u", nv_item.bt_disabled, 0, 0 );
    if ( nv_item.bt_disabled == 1 ) 
    {
      bt_dc.is_bt_disabled = TRUE;
    }
  }
  else
  {
    BT_MSG_DEBUG( "BT: Unable to read NV_BT_DISABLED_I", 0, 0, 0 );
  }
} /* bt_read_is_bt_disabled_from_nv */
#endif /* BT_SWDEV_BT_DISABLED_NV */
#endif /* FEATURE_BT_SOC */


/*===========================================================================

FUNCTION
  bt_read_my_bd_addr_from_nv

DESCRIPTION
  Read MY_BD_ADDR from non-volatile memory.

===========================================================================*/
LOCAL void bt_read_my_bd_addr_from_nv
(
  void
)
{

  nv_cmd_type   rd_bd_addr_nv_cmd;
  nv_item_type  nv_item;

  /* Read MY_BD_ADDR from NV */
  rd_bd_addr_nv_cmd.item       = NV_BD_ADDR_I;
  rd_bd_addr_nv_cmd.cmd        = NV_READ_F;
  rd_bd_addr_nv_cmd.data_ptr   = &nv_item;
  rd_bd_addr_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_bd_addr_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_bd_addr_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_bd_addr_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( rd_bd_addr_nv_cmd.status == NV_DONE_S )
  {
    bt_cmd_dc_set_bd_addr( ( bt_bd_addr_type* ) &nv_item );
  }
  else
  {
    const bt_bd_addr_type bd_addr = { 0x34, 0x12, 0x78, 0x56, 0xbc, 0x9a };

    BT_ERR( "BT: Unable to read MY_BD_ADDR from NV", 0, 0, 0 );
    bt_cmd_dc_set_bd_addr( ( bt_bd_addr_type* ) &bd_addr );
  }
}


/*===========================================================================

FUNCTION
  bt_read_refclock_type_from_nv

DESCRIPTION
  Read BT_REFCLOCK_TYPE from non-volatile memory.

===========================================================================*/
LOCAL void bt_read_refclock_type_from_nv
(
  void
)
{

  nv_cmd_type   rd_bt_refclock_type_nv_cmd;
  nv_item_type  nv_item;

  /* Read BT_REFCLOCK_TYPE from NV */
  rd_bt_refclock_type_nv_cmd.item       = NV_BT_SOC_REFCLOCK_TYPE_I;
  rd_bt_refclock_type_nv_cmd.cmd        = NV_READ_F;
  rd_bt_refclock_type_nv_cmd.data_ptr   = &nv_item;
  rd_bt_refclock_type_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_bt_refclock_type_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_bt_refclock_type_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_bt_refclock_type_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( rd_bt_refclock_type_nv_cmd.status == NV_DONE_S )
  {
    bt_cmd_dc_set_refclock_type( ( bt_soc_refclock_type* )&nv_item );
  }
  else
  {
    
#ifdef FEATURE_BT_19P2_SYSTEM_CLOCK
    bt_soc_refclock_type bt_refclock_type = BT_SOC_REFCLOCK_19P2MHZ;
#else
    bt_soc_refclock_type bt_refclock_type = BT_SOC_REFCLOCK_32MHZ;
#endif /* FEATURE_BT_19P2_SYSTEM_CLOCK */

    BT_ERR( "BT: Unable to read BT_REFCLOCK_TYPE from NV", 0, 0, 0 );
    bt_cmd_dc_set_refclock_type( &bt_refclock_type );
  }
}


/*===========================================================================

FUNCTION
  bt_read_clock_sharing_type_from_nv

DESCRIPTION
  Read BT_CLOCK_SHARE_TYPE from non-volatile memory.

===========================================================================*/
LOCAL void bt_read_clock_sharing_type_from_nv
(
  void
)
{

  nv_cmd_type   rd_bt_clock_sharing_type_nv_cmd;
  nv_item_type  nv_item;

  /* Read BT_CLOCK_SHARE_TYPE from NV */
  rd_bt_clock_sharing_type_nv_cmd.item       = NV_BT_SOC_CLK_SHARING_TYPE_I;
  rd_bt_clock_sharing_type_nv_cmd.cmd        = NV_READ_F;
  rd_bt_clock_sharing_type_nv_cmd.data_ptr   = &nv_item;
  rd_bt_clock_sharing_type_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_bt_clock_sharing_type_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_bt_clock_sharing_type_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_bt_clock_sharing_type_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( rd_bt_clock_sharing_type_nv_cmd.status == NV_DONE_S )
  {
    bt_cmd_dc_set_clock_sharing_type( (bt_soc_clock_sharing_type *)&nv_item );
  }
  else
  {
#ifdef FEATURE_BT_SYSTEM_CLOCK_SHARING
    bt_soc_clock_sharing_type bt_clock_sharing = BT_SOC_CLOCK_SHARING_ENABLED;
#else
    bt_soc_clock_sharing_type bt_clock_sharing = BT_SOC_CLOCK_SHARING_DISABLED;
#endif /* FEATURE_BT_SYSTEM_CLOCK_SHARING */

    BT_ERR( "BT: Unable to read BT_CLOCK_SHARE_TYPE from NV", 0, 0, 0 );
    bt_cmd_dc_set_clock_sharing_type( &bt_clock_sharing );
  }
}


/*===========================================================================

FUNCTION
  bt_read_inband_sleep_type_from_nv

DESCRIPTION
  Read BT_INBAND_SLEEP_TYPE from non-volatile memory.

===========================================================================*/
LOCAL void bt_read_inband_sleep_type_from_nv
(
  void
)
{

  nv_cmd_type   rd_bt_inband_sleep_type_nv_cmd;
  nv_item_type  nv_item;

  /* Read BT_INBAND_SLEEP_TYPE from NV */
  rd_bt_inband_sleep_type_nv_cmd.item       = NV_BT_SOC_INBAND_SLEEP_I;
  rd_bt_inband_sleep_type_nv_cmd.cmd        = NV_READ_F;
  rd_bt_inband_sleep_type_nv_cmd.data_ptr   = &nv_item;
  rd_bt_inband_sleep_type_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_bt_inband_sleep_type_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_bt_inband_sleep_type_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_bt_inband_sleep_type_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( rd_bt_inband_sleep_type_nv_cmd.status == NV_DONE_S )
  {
    bt_cmd_dc_set_inband_sleep_type( (bt_soc_inband_sleep_type *)&nv_item );
  }
  else
  {
    bt_soc_inband_sleep_type bt_inband_sleep_type = BT_SOC_HW_INBAND_SLEEP;
    BT_ERR( "BT: Unable to read BT_INBAND_SLEEP_TYPE from NV", 0, 0, 0 );
    bt_cmd_dc_set_inband_sleep_type( &bt_inband_sleep_type );
  }
}


/*===========================================================================

FUNCTION
  bt_read_soc_logging_enabled_from_nv

DESCRIPTION
  Read BT_SOC_LOGGING_ENABLED from non-volatile memory.

===========================================================================*/
LOCAL void bt_read_soc_logging_enabled_from_nv
(
  void
)
{

  nv_cmd_type   rd_bt_soc_logging_nv_cmd;
  nv_item_type  nv_item;

  /* Read BT_SOC_LOGGING_ENABLED from NV */
  rd_bt_soc_logging_nv_cmd.item       = NV_BT_SOC_LOGGING_ENABLED_I;
  rd_bt_soc_logging_nv_cmd.cmd        = NV_READ_F;
  rd_bt_soc_logging_nv_cmd.data_ptr   = &nv_item;
  rd_bt_soc_logging_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_bt_soc_logging_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_bt_soc_logging_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_bt_soc_logging_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( rd_bt_soc_logging_nv_cmd.status == NV_DONE_S )
  {
    bt_cmd_dc_set_soc_logging( (uint8 *)&nv_item );
  }
  else
  {
    uint8 bt_soc_logging = 0;

    BT_ERR( "BT: Unable to read BT_SOC_LOGGING_ENABLED from NV", 0, 0, 0 );
    bt_cmd_dc_set_soc_logging( &bt_soc_logging );
  }
}


/*===========================================================================

FUNCTION
  bt_read_soc_bt_wlan_coex_from_nv

DESCRIPTION
  Read WLAN_ENABLE_BT_COEX from non-volatile memory.

===========================================================================*/
LOCAL void bt_read_soc_bt_wlan_coex_from_nv
(
  void
)
{

  nv_cmd_type   rd_bt_soc_wlan_coex_nv_cmd;
  nv_item_type  nv_item;

  /* Read WLAN_ENABLE_BT_COEX from NV */
  rd_bt_soc_wlan_coex_nv_cmd.item       = NV_WLAN_ENABLE_BT_COEX_I;
  rd_bt_soc_wlan_coex_nv_cmd.cmd        = NV_READ_F;
  rd_bt_soc_wlan_coex_nv_cmd.data_ptr   = &nv_item;
  rd_bt_soc_wlan_coex_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_bt_soc_wlan_coex_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_bt_soc_wlan_coex_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_bt_soc_wlan_coex_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
}


/*===========================================================================

FUNCTION
  bt_read_soc_powermgmt_mode_from_nv

DESCRIPTION
  Read BT_SOC_PM_MODE from non-volatile memory.

===========================================================================*/
LOCAL void bt_read_soc_powermgmt_mode_from_nv
(
  void
)
{

  nv_cmd_type   rd_bt_soc_powermgmt_mode_nv_cmd;
  nv_item_type  nv_item;

  /* Read BT_SOC_PM_MODE from NV */
  rd_bt_soc_powermgmt_mode_nv_cmd.item       = NV_BT_SOC_PM_MODE_I;
  rd_bt_soc_powermgmt_mode_nv_cmd.cmd        = NV_READ_F;
  rd_bt_soc_powermgmt_mode_nv_cmd.data_ptr   = &nv_item;
  rd_bt_soc_powermgmt_mode_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_bt_soc_powermgmt_mode_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_bt_soc_powermgmt_mode_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_bt_soc_powermgmt_mode_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
}


#ifdef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_read_soc_type_from_nv

DESCRIPTION
  Read BT_SOC_TYPE from non-volatile memory.

NOTES
  Default type is BT_SOC_TYPE_NON

===========================================================================*/
LOCAL void bt_read_soc_type_from_nv
(
  void
)
{
#if ((defined(FEATURE_BT_SOC1)) && (defined (FEATURE_BT_QSOC)))
#error code not present
#elif (defined (FEATURE_BT_SOC1))
#error code not present
#elif (defined (FEATURE_BT_QSOC))

  bt_soc_type = BT_SOC_TYPE_QSOC;

#else

  bt_soc_type = BT_SOC_TYPE_NONE;

#endif

  BT_MSG_HIGH( "BT: BT_SOC_TYPE set to %x", bt_soc_type, 0, 0 );
}
#endif /* FEATURE_BT_SOC */

/*===========================================================================

FUNCTION
  bt_init_sleep_data_counters

DESCRIPTION
  Initializes BT Sleep related data counters.

============================================================================*/
LOCAL void bt_init_sleep_data_counters
(
  void
)
{
  bt_sleep_data.wakeup_isr_cnt           = 0;
  bt_sleep_data.wakeup_cmd_cnt           = 0;
  bt_sleep_data.return_from_sleep_ev_cnt = 0;
  bt_sleep_data.sleep_enabled_ev_cnt     = 0;
}


/*===========================================================================

FUNCTION
  bt_init_efs_params

DESCRIPTION
  Initializes BT EFS paramaters structure in RAM.

============================================================================*/
LOCAL void bt_init_efs_params
(
  void
)
{

  bt_efs_params.version          = BT_EFS_PARAMS_FILE_VER;
  bt_efs_params.ag_ad_mic_gain   = BT_AG_DEFAULT_HS_VOLUME_LEVEL;
  bt_efs_params.ag_ad_spkr_gain  = BT_AG_DEFAULT_HS_VOLUME_LEVEL;
  bt_efs_params.ag_idle_mode     = BT_AGIM_DEFAULT_IDLE_MODE;
  bt_efs_params.ag_idle_timeout  = BT_AG_DEFAULT_IDLE_TIMEOUT;
  bt_efs_params.ag_pref_dev_type = BT_AD_HEADSET;

  bt_efs_params.low_power_mode   = BT_LPM_SNIFF;

  bt_efs_params.cert_mode        = FALSE;
  bt_efs_params.park_supported   = TRUE;
  bt_efs_params.pair_type        = BT_PAIR_COMB;
#ifdef BT_SWDEV_2_1_SSP
  bt_efs_params.encrypt_mode     = BT_EM_ENABLED;
#else
  bt_efs_params.encrypt_mode     = BT_EM_POINT_TO_POINT;
#endif /* BT_SWDEV_2_1_SSP */
  bt_efs_params.toggle_seqn_bit  = FALSE;
  bt_efs_params.mode_to_start_in = BT_HCIM_OFF;

  BT_STRCPY( bt_efs_params.bt_name, DEFAULT_BT_NAME, 
             sizeof(bt_efs_params.bt_name) );

#ifdef BT_SWDEV_2_1_EIR
  /* Use the first 'n' characeters of the default name as short name */
  BT_STRCPY( (char *)bt_efs_params.bt_short_name,
             bt_efs_params.bt_name, 
             (BT_MAX_EIR_NAME_LEN + 1) );

  bt_efs_params.eir_manuf_data_size = 0;
#endif /* BT_SWDEV_2_1_EIR */
    
}


#ifdef FEATURE_EFS
/*===========================================================================

FUNCTION
  bt_remove_efs_params_file

DESCRIPTION
  Remove the BT EFS parameters file from the flash file system.

============================================================================*/
LOCAL void bt_remove_efs_params_file
(
  void
)
{

  fs_rsp_msg_type  fs_rsp_msg;

  BT_MSG_HIGH( "BT: Removing EFSparams file", 0, 0, 0 );

  fs_remove( BT_EFS_PARAMS_FILE_NAME, NULL, &fs_rsp_msg );

  if ( (fs_rsp_msg.any.status != FS_OKAY_S) &&
       (fs_rsp_msg.any.status != FS_NONEXISTENT_FILE_S) )
  {
    BT_ERR( "BT: Remove EFS params file S %x ",
            fs_rsp_msg.any.status, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_read_efs_params_file

DESCRIPTION
  Reads the BT EFS parameters file in the flash file system.  If
  present, the data read is used to update the EFS parameters
  information in RAM.
  
OUTPUT
  success  TRUE if the file was found and read without error,
             FALSE otherwise.

============================================================================*/
LOCAL boolean bt_read_efs_params_file
(
  void
)
{
  
  boolean          success = FALSE;
  boolean          read_fault = FALSE;
  fs_rsp_msg_type  fs_rsp_msg;
  fs_handle_type   fs_handle = FS_NULL_HANDLE;

  /*  Note that all fs_* functions below will block with rex_wait().  */

  fs_open( BT_EFS_PARAMS_FILE_NAME, FS_OA_READONLY,
           NULL, NULL, &fs_rsp_msg );
  
  if ( fs_rsp_msg.open.status == FS_OKAY_S )
  {
    fs_handle = fs_rsp_msg.open.handle;

    fs_read( fs_handle, &bt_efs_params,
             sizeof( bt_efs_params_type ), NULL, &fs_rsp_msg );
    
    if ( (fs_rsp_msg.any.status != FS_OKAY_S) ||
         (fs_rsp_msg.read.count != sizeof( bt_efs_params_type )) )
    {
      read_fault = TRUE;

      BT_ERR( "BT: Rd EFSparams S %x C %x %x", 
              fs_rsp_msg.any.status, fs_rsp_msg.read.count,
              sizeof( bt_efs_params_type ) );
    }

    fs_close( fs_handle, 0, &fs_rsp_msg );

    if ( fs_rsp_msg.any.status == FS_OKAY_S )
    {
      if ((read_fault == FALSE) &&
          (bt_efs_params.version == BT_EFS_PARAMS_FILE_VER) )
      {
        success = TRUE;
      }
    }
    else
    {
      BT_ERR( "BT: Rd EFSparams close S %x",
              fs_rsp_msg.any.status, 0, 0 );
    }
  }
  else if ( fs_rsp_msg.any.status != FS_NONEXISTENT_FILE_S )
  {
    BT_ERR( "BT: Rd EFSparams open S %x",
            fs_rsp_msg.any.status, 0, 0 );
  }

  return ( success );

}
#endif /* FEATURE_EFS */


/*===========================================================================

FUNCTION
  bt_write_efs_params_file

DESCRIPTION
  Write the BT EFS parameters structure to the flash file system.
  
OUTPUT
  success  TRUE if all is OK, otherwise FALSE

============================================================================*/
LOCAL boolean bt_write_efs_params_file
( 
  void
)
{

  boolean              success = FALSE;

#ifdef FEATURE_EFS

  boolean              write_fault = FALSE;
  fs_open_xparms_type  fs_open_xparms;
  fs_rsp_msg_type      fs_rsp_msg;
  fs_handle_type       fs_handle;
  
  fs_open( BT_EFS_PARAMS_FILE_NAME, FS_OA_READWRITE,
           NULL, NULL, &fs_rsp_msg );

  if ( fs_rsp_msg.any.status == FS_NONEXISTENT_FILE_S )
  {
    BT_MSG_HIGH( "BT: Creating EFSparams file", 0, 0, 0 );

    fs_open_xparms.create.cleanup_option   = FS_OC_CLOSE;
    fs_open_xparms.create.buffering_option = FS_OB_ALLOW;
    fs_open_xparms.create.attribute_mask   = FS_FA_UNRESTRICTED;

    fs_open( BT_EFS_PARAMS_FILE_NAME, FS_OA_CREATE, 
             &fs_open_xparms, NULL, &fs_rsp_msg );
  }

  if ( fs_rsp_msg.any.status == FS_OKAY_S )
  {
    fs_handle = fs_rsp_msg.open.handle;

    fs_write( fs_handle, &bt_efs_params, 
              sizeof( bt_efs_params_type ), NULL, &fs_rsp_msg );

    if ( (fs_rsp_msg.any.status != FS_OKAY_S) ||
         (fs_rsp_msg.write.count != sizeof( bt_efs_params_type )) )
    {
      write_fault = TRUE;

      BT_ERR( "BT: EFSparams Wr S %x C %x %x",
              fs_rsp_msg.any.status, fs_rsp_msg.write.count,
              sizeof( bt_efs_params_type ) );
    }
    
    fs_close( fs_handle, NULL, &fs_rsp_msg );

    if ( fs_rsp_msg.any.status == FS_OKAY_S )
    {
      if ( write_fault == FALSE )
      {
        success = TRUE;
      }
    }
    else
    {
      BT_ERR( "BT RM: Wr EFSparams close S %x",
              fs_rsp_msg.any.status, 0, 0 );
    }
  }
  else
  {
    BT_ERR( "BT: EFSparams open S %x",
            fs_rsp_msg.any.status, 0, 0);
  }

#else /*  No EFS  */
  
  BT_ERR( "BT: Wr EFSparams NO EFS!", 0, 0, 0 );

#endif /* FEATURE_EFS */

  return ( success );

}


/*===========================================================================

FUNCTION
  bt_efs_params_init

DESCRIPTION
  Initialize BT EFS parameter structure in RAM using data stored
  in flash file system, if present, or it will initialize the
  file in EFS as necessary.

============================================================================*/
LOCAL void bt_efs_params_init
(
  void
)
{
  
#ifdef FEATURE_EFS
  /*  Read the BT parameter information from EFS.  */
  if ( bt_read_efs_params_file() == FALSE )
  {
    /*  Either the file doesn't exist, it is the wrong version,  */
    /*  or it has been corrupted.  (Re)initialize the file now.  */

    bt_remove_efs_params_file();

    bt_init_efs_params();
    
#ifndef FEATURE_BT_SOC
    bt_se_create_new_unit_key();
#endif /* FEATURE_BT_SOC */
    
    if ( bt_write_efs_params_file() == FALSE )
    {
      BT_ERR( "BT: Bad Init EFSparams file!", 0, 0, 0 );
    }
  }
#endif /* FEATURE_EFS */

}


/*===========================================================================

FUNCTION
  bt_init_always_on_timers

DESCRIPTION
  This function defines timers that can be active during power collapse.

===========================================================================*/
LOCAL void bt_init_always_on_timers
(
  void
)
{
  /* These timers should be cleared in bt_soc_shutdown() */
  timer_def( &bt_soc_driver_idle_timer, &bt_always_on_timer_group, &bt_tcb, 
             BT_DRIVER_IDLE_TIMER_SIG, NULL, 0 );
  timer_def( &bt_requeued_cmd_q_timer, &bt_always_on_timer_group, &bt_tcb, 
             BT_REQUEUED_CMD_Q_SIG, NULL, 0 );
  timer_def( &bt_persistent_background_timer, &bt_always_on_timer_group,
             &bt_tcb, BT_BACKGROUND_TIMER_SIG, NULL, 0 );
  timer_group_enable( &bt_always_on_timer_group );

}

/*===========================================================================

FUNCTION
  bt_task_init

DESCRIPTION
  This function initializes the Bluetooth Task.

===========================================================================*/
LOCAL void bt_task_init
(
  void
)
{

  uint16 i;

  rex_def_timer( &bt_wait_timer, &bt_tcb, BT_WAIT_SIG );
  rex_def_timer( &bt_rpt_timer, &bt_tcb, BT_RPT_TIMER_SIG );
#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
#else
  rex_def_timer( &bt_qdsp_poll_timer, &bt_tcb, BT_QDSP_POLL_SIG );
#endif /* !FEATURE_BT_SOC */
  rex_def_timer( &bt_clocks_monitor_timer, &bt_tcb, BT_CLOCKS_SHUTDOWN_SIG );
  rex_def_timer( &bt_background_timer, &bt_tcb, BT_BACKGROUND_TIMER_SIG );
#ifdef FEATURE_BT_L2FLUSH
  rex_def_timer( &bt_l2flush_tick_timer, &bt_tcb, BT_L2FLUSH_TIMER_SIG );
#endif
#ifdef BT_SWDEV_QXDM_LOG_PER
  rex_def_timer( &bt_dm_timer, &bt_tcb, BT_DM_TIMER_SIG );
#endif

#ifdef BT_AUDIO_DEBUG_MDSP
  rex_def_timer( &bt_debug_timer, &bt_tcb, BT_AUDIO_DEBUG_SIG );
#endif /* BT_AUDIO_DEBUG */

  bt_event_q_mask = 0x0;

  /*  Initialize task queues.  */
  (void) q_init( &bt_cmd_q );
  (void) q_init( &bt_cmd_re_q );

  (void) q_init( &bt_cmd_free_q );
  for ( i = 0; i < BT_NUM_CMDS; i++ )
  {
    bt_initialize_cmd_buffer( &bt_cmds[ i ] );
    q_put( &bt_cmd_free_q, q_link( &bt_cmds[ i ],
                                   &bt_cmds[ i ].cmd_hdr.link ) );
  }

  (void) q_init( &bt_event_free_q );
  for ( i = 0; i < BT_NUM_EVENTS; i++ )
  {
    q_put( &bt_event_free_q, q_link( &bt_events[ i ],
                                     &bt_events[ i ].ev_hdr.link ) );
  }

  /*  Initialize vital statistics table.  */
  for ( i = 0; i < BT_VITAL_STAT_COUNT; i++ )
  {
    bt_vital_stats[ i ] = 0;
  }

  /*  Initialize fault table.  */
  for ( i = 0; i < BT_FAULTS_MAX; i++ )
  {
    memset( bt_fault_table[ i ].file_name, 0, BT_MAX_FILENAME_LEN );
    bt_fault_table[ i ].line_num = 0;
    bt_fault_table[ i ].value1   = 0;
    bt_fault_table[ i ].value2   = 0;
    bt_fault_table[ i ].value3   = 0;
  }

  /*  Initialize the BT Sleep data counts.  */
  bt_init_sleep_data_counters();

  bt_init_efs_params();

  bt_dc_initialize();
  bt_ec_initialize();
  bt_hc_powerup_init();
#ifndef FEATURE_BT_SOC

  bt_qq_powerup_init();
  bt_bb_powerup_init();
  bt_lm_powerup_init();
  bt_se_powerup_init();
#else

  bt_soc_etc_powerup_init();

#endif /* FEATURE_BT_SOC */
  bt_rm_powerup_init();
  bt_l2_powerup_init();
  bt_rc_powerup_init();
  bt_sd_powerup_init();
  bt_sio_powerup_init();

#ifdef FEATURE_BT_AG
  bt_ag_powerup_init();
#endif /* FEATURE_BT_AG */

  bt_init_always_on_timers();

#if (defined( T_MSM6250 ) || defined( T_MSM6275 )) && !defined( T_MSM7200 )
#error code not present
#endif

#ifdef BT_WAKEUP_EDGE_TRIGGERED
  /* Set to up the BT wakeup interrupt to be edge-sensitive */
  #ifdef T_MSM6800
    #ifndef T_QSC60X5
#error code not present
    #endif
  #else /* MSM6550 */
    HWIO_INT_DETECT_CTL_1_OUTM(
      HWIO_INT_DETECT_CTL_1_BT_WAKEUP_BMSK,
      HWIO_INT_DETECT_CTL_1_BT_WAKEUP_BMSK );
  #endif /* T_MSM6800 */
#endif

}

#ifdef BT_AUDIO_DEBUG
#error code not present
#endif /* BT_AUDIO_DEBUG */


/*===========================================================================

FUNCTION
  bt_task_start

DESCRIPTION
  This function starts the BT task -- performs functions that interact with
  other subsystems.

===========================================================================*/
LOCAL void bt_task_start
(
  void
)
{

#ifdef FEATURE_NEW_SLEEP_API 
  /* Register with the sleep module, obtain a handle to vote for sleep and
     vote that BT is okay to sleep at start-up. */
  bt_okts_handle = sleep_register( "BT", TRUE );

  /* Vote against VDD min */
  bt_set_vdd_min_not_allowed();

#else
  /* Always vote that BT is okay to sleep at start-up. */
  bt_assert_okts();   
#endif /* FEATURE_NEW_SLEEP_API  */

#ifndef FEATURE_BT_SOC
  /* Disable BT clocks until the BT driver is enabled. */
  bt_disable_clocks();

  /* Enable the BT Wakeup ISR */
#if defined( T_MSM33 ) || defined( T_MSM6250 )
  tramp_set_isr( TRAMP_BT_WAKEUP_ISR, bt_wakeup_isr );
#elif defined( T_MSM6200 )
#error code not present
#else
  #error "MSM not supported."
#endif

#endif /* FEATURE_BT_SOC */

#ifdef FEATURE_BT_SOC 
#ifdef BT_SWDEV_BT_DISABLED_NV
  bt_read_is_bt_disabled_from_nv();
#endif /* BT_SWDEV_BT_DISABLED_NV */
#endif /* FEATURE_BT_SOC */

  bt_dc_reg_with_external_services();

  bt_cmd_dc_set_hci_mode( BT_HCIM_OFF );

#if defined( BT_QDSP_DEBUG )
#error code not present
#endif

#ifdef FEATURE_BT_TEST_MODE_ONLY
  bt_cmd_rm_test_mode_enable( bt_l2_app_id, TRUE );
#endif

  /* Start the dog kicking timer */
  rex_set_timer( &bt_rpt_timer, DOG_BT_RPT_TIME );

#ifdef BT_SWDEV_QXDM_LOG_PER
  /* Start sending logs to the DM */
  rex_set_timer( &bt_dm_timer, BT_DM_TIMER_MS );
#endif

  /*  Read from NV/EFS.  */
  bt_read_my_bd_addr_from_nv();
  bt_read_refclock_type_from_nv();
  bt_read_clock_sharing_type_from_nv();
  bt_read_inband_sleep_type_from_nv();
  bt_read_soc_logging_enabled_from_nv();
  bt_read_soc_bt_wlan_coex_from_nv();
  bt_read_soc_powermgmt_mode_from_nv();


#ifdef FEATURE_BT_SOC
  bt_read_soc_type_from_nv();
#endif /* FEATURE_BT_SOC */

  bt_read_lisbon_disabled_from_nv();

  bt_efs_params_init();

  bt_rm_efs_init();

#ifdef FEATURE_BT_2_1
#ifdef BT_SWDEV_2_1_NV
  bt_rm_nv_init();
#endif /* BT_SWDEV_2_1_NV */
#endif /* FEATURE_BT_2_1 */

#ifdef FEATURE_BT_TEST_MODE_ONLY
  bt_output_rf_test_mode_display();
#endif

#ifdef FEATURE_FACTORY_TESTMODE
  /* Initialize BT Diag interface */
  bt_diag_init( );
#endif

}


/*===========================================================================

FUNCTION
  bt_process_command

DESCRIPTION
  Processes a specified Bluetooth command.

===========================================================================*/
LOCAL void bt_process_command
(
  bt_cmd_msg_type*  bt_cmd_ptr
)
{
#ifdef FEATURE_BT_AG
  if ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, AG ) )
  {
    bt_ag_process_command( bt_cmd_ptr );
  }
#endif /* FEATURE_BT_AG */
  else if ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, NA ) )
  {
    bt_na_process_command( bt_cmd_ptr );
  }
  else if ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, RC ) )
  {
    bt_rc_process_command( bt_cmd_ptr );
  }
  else if ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, SD ) )
  {
    bt_sd_process_command( bt_cmd_ptr );
  }
  else if ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, L2 ) )
  {
    bt_l2_process_command( bt_cmd_ptr );
  }
  else if ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, RM ) )
  {
    bt_rm_process_command( bt_cmd_ptr );
  }
  else if ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, HC ) )
  {
#ifdef BT_SWDEV_RADIO_OFF
    if ( bt_dc.radio_state == BT_RADIO_DISABLED )
    {
      bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_RM_RADIO_OFF;
    }
    else
#endif
#ifndef FEATURE_BT_SOC
    {
      if ( bt_dc.driver_state == BT_DS_IDLE )
      {
        bt_dc_enable_driver();
      }
      if ( bt_dc.driver_state == BT_DS_READY )
      {
        bt_hc_process_cmd( bt_cmd_ptr );
      }
      else
      {
        bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }
    }
#else
    { 
#ifdef FEATURE_BT_SOC1
#error code not present
#elif (defined FEATURE_BT_QSOC)

      if ( bt_dc.driver_state == BT_DS_SOC_ENABLING )
      { 
        if ( BT_QSOC_IS_READY() ) /* BK: TBD if this is correct. */
        {
          BT_MSG_HIGH( "BT DC: Driver Enabled ", 0,0,0 );
          BT_DC_UPDATE_DRIVER_STATE(BT_DS_SOC_ACTIVE);
        }
      }
      else if ( bt_dc.driver_state != BT_DS_SOC_ACTIVE ) 
      {
        bt_hc_drv_process_cmd_preamble();
      }

      if (bt_dc.driver_state == BT_DS_SOC_ACTIVE )
      {
        /* send the message through the H4DS engine of soc1 */
        BT_MSG_HIGH( "BT: Sending HC command ", 0,0,0 );
        bt_hc_drv_process_cmd( bt_cmd_ptr );
      }
      else
      {
        BT_MSG_HIGH( "BT: Re-Queuing HC commands until BTS init ", 0, 0, 0 );
        bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }

#else
      /* HCI mode - driver, UART  always enabled. Process commands now. */
      bt_hc_drv_process_cmd( bt_cmd_ptr );

#endif /* FEATURE_BT_SOC1 */
      }
#endif /* FEATURE_BT_SOC */
  }
  else if ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, SIO ) )
  {
    bt_sio_process_command( bt_cmd_ptr );
  }
#ifdef FEATURE_BT_HCI_HOST_FCTL
  else if ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, FCTL ) )
  {
    bt_hc_drv_fctl_process_cmd( bt_cmd_ptr );
  }
#endif /* FEATURE_BT_HCI_HOST_FCTL */
  else
  {
    bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
    BT_ERR("BT: Unable to process unrecognized BT Command type.", 0,0,0 );
  }

}


/*===========================================================================

FUNCTION
  bt_process_requeued_commands

DESCRIPTION
  Dequeues commands from the BT command re-queue and processes them.

===========================================================================*/
LOCAL void bt_process_requeued_commands
(
  boolean  rqd_cmd_q_timer_expired
)
{

  int               cmd_count;
  uint16            i;
  bt_cmd_msg_type*  cmd_ptr;

  cmd_count = q_cnt( &bt_cmd_re_q );

  BT_VSTAT_MAX( cmd_count, MAX_CMDS_REQUEUED );

  for ( i = 0; i < cmd_count; i++ )
  {
    if ( (cmd_ptr = (bt_cmd_msg_type*)(q_get( &bt_cmd_re_q ))) != NULL )
    {
      if ( cmd_ptr->cmd_hdr.cmd_state == BT_CST_QUEUED )
      {
        cmd_ptr->cmd_hdr.cmd_state = BT_CST_PROCESSING;
        bt_process_command( cmd_ptr );

        if ( cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_RETRY_CMD_LATER )
        {
          cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

          if ( rqd_cmd_q_timer_expired != FALSE )
          {
            cmd_ptr->cmd_hdr.cmd_retries++;
          }
          
          if ( cmd_ptr->cmd_hdr.cmd_retries < BT_CMD_MAX_RETRIES )
          {
            cmd_ptr->cmd_hdr.cmd_state = BT_CST_QUEUED;
            q_put( &bt_cmd_re_q, &cmd_ptr->cmd_hdr.link );

            BT_MSG_DEBUG( "BT: Cmd ReQd Cnt %x Typ %x AID %x",
                          cmd_ptr->cmd_hdr.cmd_retries,
                          cmd_ptr->cmd_hdr.cmd_type,
                          cmd_ptr->cmd_hdr.bt_app_id );
          }
          else
          {
            cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_MAX_CMD_RETRIES;
          }
        }
      }
      else
      {
        cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_CMD_STATE;
      }

      if ( cmd_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS )
      {
        BT_ERR( "BT: Bad Cmd Done Stat- %x Typ %x AID %x",
                cmd_ptr->cmd_hdr.cmd_status,
                cmd_ptr->cmd_hdr.cmd_type,
                cmd_ptr->cmd_hdr.bt_app_id );
      }

      if ( cmd_ptr->cmd_hdr.cmd_state != BT_CST_QUEUED )
      {
        bt_ev_msg_type ev_cmd_done;
        
        ev_cmd_done.ev_hdr.ev_type                   = BT_EV_GN_CMD_DONE;
        ev_cmd_done.ev_hdr.bt_app_id                 = cmd_ptr->cmd_hdr.bt_app_id;
        ev_cmd_done.ev_msg.ev_gn_cmd_done.cmd_type   = cmd_ptr->cmd_hdr.cmd_type;
        ev_cmd_done.ev_msg.ev_gn_cmd_done.bt_app_id  = cmd_ptr->cmd_hdr.bt_app_id;
        ev_cmd_done.ev_msg.ev_gn_cmd_done.cmd_status = cmd_ptr->cmd_hdr.cmd_status;
        bt_ec_send_event( &ev_cmd_done );
      }

      if ( cmd_ptr->cmd_hdr.cmd_state == BT_CST_PROCESSING )
      {
        BT_VSTAT_MAX( cmd_ptr->cmd_hdr.cmd_retries, MAX_CMD_RETRIES );

        bt_initialize_cmd_buffer( cmd_ptr );
        q_put( &bt_cmd_free_q, &cmd_ptr->cmd_hdr.link );
      }
    }
    else
    {
      BT_ERR( "BT: Bad Cmd ReQ Cnt %x %x", cmd_count, i, 0 );
    }
  }
  if ( (rqd_cmd_q_timer_expired != FALSE) && (q_cnt( &bt_cmd_re_q ) != 0) )
  {
    rex_set_timer( &bt_requeued_cmd_q_timer,
                   BT_REQUEUED_CMD_Q_TIMER_MS );
  }

}


/*===========================================================================

FUNCTION
  bt_process_command_queue

DESCRIPTION
  Dequeues commands from the BT driver command queue and processes them.
  When necessary a command may be re-queued (placed onto bt_cmd_re_q).

===========================================================================*/
LOCAL void bt_process_command_queue
(
  void
)
{

  bt_cmd_msg_type*  cmd_ptr;
  bt_ev_msg_type    ev_cmd_done;
  uint16            cmds_processed = 0;
  int               q_count = q_cnt( &bt_cmd_q );

  ev_cmd_done.ev_hdr.ev_type = BT_EV_GN_CMD_DONE;

  BT_VSTAT_MAX( q_count, MAX_CMD_Q_DEPTH );

  while ( (cmds_processed < 3) &&
          ((cmd_ptr = ((bt_cmd_msg_type*)q_get( &bt_cmd_q ))) != NULL) )
  {
    if ( cmd_ptr->cmd_hdr.cmd_state == BT_CST_QUEUED )
    {
      cmd_ptr->cmd_hdr.cmd_state = BT_CST_PROCESSING;
      bt_process_command( cmd_ptr );

      if ( cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_RETRY_CMD_LATER )
      {
        cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

        cmd_ptr->cmd_hdr.cmd_state = BT_CST_QUEUED;
        if ( q_cnt( &bt_cmd_re_q ) == 0 )
        {
          rex_set_timer( &bt_requeued_cmd_q_timer,
                         BT_REQUEUED_CMD_Q_TIMER_MS );
        }
        q_put( &bt_cmd_re_q, &cmd_ptr->cmd_hdr.link );

        BT_MSG_DEBUG( "BT: Cmd ReQd AID %x Typ %x Cnt %x",
                      cmd_ptr->cmd_hdr.bt_app_id,
                      cmd_ptr->cmd_hdr.cmd_type,
                      cmd_ptr->cmd_hdr.cmd_retries );
      }
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_CMD_STATE;
    }

    if ( cmd_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS )
    {
      BT_ERR( "BT: Bad Cmd Done Stat %x Typ %x AID %x",
              cmd_ptr->cmd_hdr.cmd_status,
              cmd_ptr->cmd_hdr.cmd_type,
              cmd_ptr->cmd_hdr.bt_app_id );
    }

    /*  Send Command Done event.  */
      ev_cmd_done.ev_hdr.bt_app_id = cmd_ptr->cmd_hdr.bt_app_id;
      ev_cmd_done.ev_msg.ev_gn_cmd_done.cmd_type =
        cmd_ptr->cmd_hdr.cmd_type;
      ev_cmd_done.ev_msg.ev_gn_cmd_done.bt_app_id =
        cmd_ptr->cmd_hdr.bt_app_id;
      ev_cmd_done.ev_msg.ev_gn_cmd_done.cmd_status =
        cmd_ptr->cmd_hdr.cmd_status;
      bt_ec_send_event( &ev_cmd_done );

    if ( cmd_ptr->cmd_hdr.cmd_state == BT_CST_PROCESSING )
    {
      bt_initialize_cmd_buffer( cmd_ptr );
      q_put( &bt_cmd_free_q, &cmd_ptr->cmd_hdr.link );
    }
  }

}

#ifdef BT_SWDEV_RADIO_OFF

/*===========================================================================

FUNCTION
  bt_hc_command_remove_compare_func

DESCRIPTION
  Compare callback function passed into q_linear_delete when removing
  HC commands from the command queue. This function should return a non-zero
  value for the command to be removed.

===========================================================================*/
LOCAL int bt_hc_command_remove_compare_func
( 
  void* item_ptr, 
  void* compare_val
)
{
  bt_cmd_msg_type*  cmd_ptr = (bt_cmd_msg_type*)item_ptr;

  return ( BT_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, HC ) );
} 


/*===========================================================================

FUNCTION
  bt_hc_command_remove_action_func

DESCRIPTION
  Action callback function passed into q_linear_delete when removing
  HC commands from the command queue. This function performs operations
  necessary after the command has been removed from the queue.

===========================================================================*/
LOCAL void bt_hc_command_remove_action_func
(
  void*  item_ptr,
  void*  param_ptr
)
{
  bt_cmd_msg_type*  cmd_ptr = (bt_cmd_msg_type*)item_ptr;
  
  BT_MSG_DEBUG( "BT: HC Cmd %x Rmv'd AID %x State %x", 
                cmd_ptr->cmd_hdr.cmd_type, 
                cmd_ptr->cmd_hdr.bt_app_id,
                cmd_ptr->cmd_hdr.cmd_state );

  bt_initialize_cmd_buffer( cmd_ptr );
  q_put( &bt_cmd_free_q, &cmd_ptr->cmd_hdr.link );

}


/*===========================================================================

FUNCTION
  bt_remove_hc_commands_from_q

DESCRIPTION
  Removes all HC commands from the specified command queue.

===========================================================================*/
LOCAL void bt_remove_hc_commands_from_q
(
  q_type* bt_cmd_q_ptr
)
{
  uint16   i; 
  uint16   q_len = q_cnt( bt_cmd_q_ptr );
  void*    item_ptr;

  for (i = 0; i < q_len; i++)
  {
    item_ptr = q_linear_search( bt_cmd_q_ptr, 
                                bt_hc_command_remove_compare_func,
                                (void*)NULL );
    
    if ( item_ptr == NULL )
    {
      break;
    }
    else
    {
      q_delete( bt_cmd_q_ptr, (q_link_type*)item_ptr );

      /* put back in free queue */
      bt_hc_command_remove_action_func( item_ptr, (void*)NULL );
    }
  }

  BT_MSG_DEBUG( "BT: Rmv %x HC Cmds from Q of len %x ", i, q_len, 0 );
}


/*===========================================================================

FUNCTION
  bt_remove_queued_hc_commands

DESCRIPTION
  Removes all HC commands from both command queues.

===========================================================================*/
void bt_remove_queued_hc_commands
(
  void
)
{
  bt_remove_hc_commands_from_q( &bt_cmd_q );
  bt_remove_hc_commands_from_q( &bt_cmd_re_q );

#ifdef FEATURE_BT_QSOC_SLEEP
  bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_BT_Q_EMPTY, TRUE);
#endif /* FEATURE_BT_QSOC_SLEEP*/
}

#endif /* BT_SWDEV_RADIO_OFF */


/*===========================================================================

FUNCTION
  bt_store_bt_event

DESCRIPTION
  Stores a specified BT event on a specified BT driver internal
  event queue.

===========================================================================*/
void bt_store_bt_event
(
  bt_ev_msg_type*        bt_ev_ptr,
  bt_event_q_info_type*  evqi_ptr
)
{

  bt_ev_msg_type*  ev_ptr;
  int              ev_q_depth;

  if ( (ev_q_depth = q_cnt( evqi_ptr->event_q_ptr )) <
          evqi_ptr->max_q_depth )
  {
    if ( (ev_ptr =
            (bt_ev_msg_type*)q_get( &bt_event_free_q )) != NULL )
    {
      memcpy( (byte*)ev_ptr, (byte*)bt_ev_ptr,
              evqi_ptr->max_event_bytes );

      q_put( evqi_ptr->event_q_ptr, &ev_ptr->ev_hdr.link );

      INTLOCK();
      bt_event_q_mask |= evqi_ptr->event_q_bit_mask;
      INTFREE();

      (void) rex_set_sigs( &bt_tcb, BT_EVENT_Q_SIG );
    }
    else
    {
      bt_free_any_dsms_in_event( bt_ev_ptr );
      BT_ERR( "BT EVQ: Overflow D %x M %x B %x",
              ev_q_depth, evqi_ptr->max_q_depth,
              evqi_ptr->event_q_bit_mask );
    }
  }
  else
  {
    bt_free_any_dsms_in_event( bt_ev_ptr );
    BT_ERR( "BT EVQ: Overflow D %x M %x B %x",
              ev_q_depth, evqi_ptr->max_q_depth,
              evqi_ptr->event_q_bit_mask );
  }

}


/*===========================================================================

FUNCTION
  bt_record_fault

DESCRIPTION
  Records a specified BT driver internal fault.

===========================================================================*/
void bt_record_fault
(
  const char*  filename_str,
  uint16       line_num,
  uint32       value1,
  uint32       value2,
  uint32       value3
)
{

  if ( bt_fault_count < BT_FAULTS_MAX )
  {
    BT_STRCPY( bt_fault_table[ bt_fault_count ].file_name,
               filename_str, 
               sizeof(bt_fault_table[ bt_fault_count ].file_name) );
    bt_fault_table[ bt_fault_count ].line_num = line_num;
    bt_fault_table[ bt_fault_count ].value1 = value1;
    bt_fault_table[ bt_fault_count ].value2 = value2;
    bt_fault_table[ bt_fault_count ].value3 = value3;

    bt_fault_count++;
  }
  else
  {
    bt_fault_table[ BT_FAULTS_MAX ].value1 = bt_fault_count++;
    if ( bt_fault_count == 0 )
    {
      bt_fault_count--;
    }
  }

}


#ifndef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_enable_hardware

DESCRIPTION
  This function is called when the Bluetooth image is being swapped in.
  It clears off the clocks monitor timer. Re-enables all the clocks and
  calls ready_init() functions of the qq, qd and bb sub-sections. It should
  be followed by a call to bt_qdsp_ready_init(). The two of these together
  comprise the third and final part of enabling the bluetooth driver ( after
  it was swapped out) and do the reverse of bt_qdsp_reset_shutdown() routine.

===========================================================================*/
LOCAL void bt_enable_hardware
(
  void
)
{

  rex_clr_timer( &bt_clocks_monitor_timer );

#if (defined( T_MSM6250 ) || defined( T_MSM6275 )) && !defined( T_MSM7200 )
#error code not present
#endif

#ifdef FEATURE_PMIC
  #if defined( T_MSM6250 ) || defined( T_MSM6500 ) || defined( T_FFA_TSUNAMI )
    /* Enable the VREG for 6250, 6275, 6280, 6500, 6550, 6800 */
    #ifdef T_FFA6550_PANDORA
      /* BT device hangs off VREG_WLAN */
      pm_vote_vreg_switch( PM_ON_CMD,
                           PM_VREG_WLAN_ID,
                           PM_VOTE_VREG_WLAN_APP__BT );
    #elif defined( SIRIUS_PLATFORM )
#error code not present
    #else   
      #if (defined(FEATURE_H6_FFA) && defined(T_MSM6260))
#error code not present
      #else
        pm_vote_vreg_switch( PM_ON_CMD,
                             PM_VREG_MMC_ID,
                             PM_VOTE_VREG_MMC_APP__BT );
      #endif
    #endif    
  #elif defined( T_MSM6100 ) && defined( FEATURE_FFA) && defined( T_PMIC_PM6640 )
#error code not present
  #endif

#ifdef T_MSM6500
  /* Vote to not sleep before calling rex_sleep().  Workaround to 
     prevent phone from going into power save during HCI_RESET */
  if ( BT_IN_HCI_MODE() )
  {
    /* Prevent TCXO from being disabled. */
    sleep_bt_clock_rgm = TRUE;

    /* GSM does not use sleep_bt_clock_rgm, vote that BT is NOT okay to sleep */
    bt_negate_okts();
  }
#endif

  /****************************************************/
  /* According to specs we should wait 5ms            */
  /* after turning on vreg, rex_sleep(10) will return */
  /* back between 5-10ms                              */
  /****************************************************/
  rex_sleep( 10 );

#endif /* FEATURE_PMIC */

  /* Workaround for QDSP4u5 DMA clock gating issue*/
  /* Clear the MDSP_DMA_CLK_OVR bit */
#if defined(T_MSM6500) && !defined(T_MSM6550)&& !defined(T_MSM7500)
#error code not present
#elif defined(T_MSM6250) && !defined(T_MSM6275) && !defined(T_MSM6280)
#error code not present
#endif

  /* Enable BT clocks now that the BT driver is enabled. */
  bt_enable_clocks();

  /* Direct GPIO output to BT */
  #if defined( T_MSM33 ) &&  !defined( T_MSM6100 )
#error code not present
  #elif ( defined( T_MSM6200 ) && !defined(T_MSM6250) && !defined(T_MSM6275) )
#error code not present
  #endif

  bt_qq_qdsp_ready_init(); /* Placeholder - nothing to do for now */
  bt_qd_qdsp_ready_init();
  bt_bb_qdsp_ready_init(); /* Placeholder - Nothing to do for now */

}

/*===========================================================================

FUNCTION
  bt_disable_hardware

DESCRIPTION
  Bluetooth is shutting down.  Disable the Bluetooth hardware.

===========================================================================*/
LOCAL void bt_disable_hardware
(
  void
)
{
  /* Configure GPIOs not for BT. */
  #if defined( T_MSM33 ) && !defined( T_MSM6100 )
#error code not present
  #elif ( defined( T_MSM6200 ) || defined( T_MSM6250 ) || defined( T_MSM6275 ) )
#error code not present
  #endif

  /* Disable BT clocks now that we are disable. */
  bt_disable_clocks();

  #ifdef FEATURE_PMIC
    #if defined( T_MSM6250 ) || defined( T_MSM6500 ) || defined( T_FFA_TSUNAMI )
      /* Disable the VREG for 6250, 6275, 6280, 6500, 6550 and 6800 */
      #ifdef T_FFA6550_PANDORA
        /* BT device hangs off VREG_WLAN */
        pm_vote_vreg_switch( PM_OFF_CMD,
                             PM_VREG_WLAN_ID,
                             PM_VOTE_VREG_WLAN_APP__BT );
      #elif defined( SIRIUS_PLATFORM )
#error code not present
      #else
        #if (defined(FEATURE_H6_FFA) && defined(T_MSM6260))
#error code not present
        #else
          pm_vote_vreg_switch( PM_OFF_CMD,
                               PM_VREG_MMC_ID,
                               PM_VOTE_VREG_MMC_APP__BT );
        #endif
      #endif
    #elif defined( T_MSM6100 ) && defined( FEATURE_FFA) && defined( T_PMIC_PM6640 )
#error code not present
    #endif
  #endif /* FEATURE_PMIC */
}

/*===========================================================================

FUNCTION
  bt_initialize_hardware

DESCRIPTION
  This function is called when the Bluetooth image is loaded.
  It performs the second part of enabling the Bluetooth driver.

===========================================================================*/
LOCAL void bt_initialize_hardware
(
  void
)
{

  bt_enable_hardware();

  /* Initialize the BT RFU and BT HW Core */
  bt_dc.driver_state = BT_DS_PENDING_HW_INIT;
  bt_bb_hw_init();

  /* The rest the BT task initializations will occur when the HW is
  ** fully initialized.
  */

}


/*===========================================================================

FUNCTION
  bt_qdsp_ready_init

DESCRIPTION
  This function is called when the Bluetooth HW is initialized.
  Performs the third and final part of enabling the Bluetooth driver.

===========================================================================*/
void bt_qdsp_ready_init
(
  void
)
{
  /*  Initialize the BT Sleep data counts.  */
  bt_init_sleep_data_counters();

#ifdef BT_QDSP_DEBUG
#error code not present
#endif /* BT_QDSP_DEBUG */

#ifdef BT_SWDEV_QDSP_POLLING
  /* The QDSP is in.  Start polling QDSP poll timer. */
  (void) rex_set_timer( &bt_qdsp_poll_timer, BT_QDSP_POLL_INTERVAL_MS );
#endif

  rex_set_timer( &bt_background_timer, BT_BACKGROUND_TIMER_MS );

  bt_se_qdsp_ready_init(); /* Placeholder - nothing to do for now */
  bt_hc_qdsp_ready_init(); /* Placeholder - nothing to do for now */
  bt_lm_qdsp_ready_init();

  /* init the upper layers of the stack if not in HCI mode */
  if ( !BT_IN_HCI_MODE() )
  {
    bt_rm_qdsp_ready_init();
    bt_l2_qdsp_ready_init(); /* Placeholder - nothing to do for now */
    bt_rc_qdsp_ready_init();
    bt_sd_qdsp_ready_init();

#ifdef FEATURE_BT_AG
    bt_ag_qdsp_ready_init();
#endif

#if defined (FEATURE_BT_AG) && defined (FEATURE_BT_ICON)
#error code not present
#endif
  }

  /* We are ready.  Start processing all commands. */
  rex_set_sigs( rex_self(), BT_CMD_Q_SIG );
  bt_dc.driver_state = BT_DS_READY;

  /* If qdsp wants out during hw init, do so now */
  if ( bt_dc.driver_substate == BT_DSS_RESET_AFTER_READY )
  {
    bt_dc.driver_substate = BT_DSS_NONE;
    ( void ) rex_set_sigs( rex_self(), BT_QDSP_RESET_SIG );
  }

#ifdef BT_SWDEV_RADIO_OFF
  if ( bt_dc.radio_state == BT_RADIO_ENABLING )
  {
    bt_dc.radio_state = BT_RADIO_ENABLED;

    if ( BT_IN_HCI_MODE() )
    {
      /* In HCI mode. Generate Command Done event */
      bt_hc_send_cmd_cmplt_status_ev( BT_HCI_RESET, BT_BE_SUCCESS );
    }
    else
    {
      /* Not in HCI mode.  Re-enable scanning and generate Radio On event */
      bt_rm_radio_disabled();
    }
  }
#endif

#ifdef FEATURE_BT_L2FLUSH
  rex_set_timer( &bt_l2flush_tick_timer, BT_L2FLUSH_TIMER_MS );
#endif

}


/*===========================================================================

FUNCTION
  bt_qdsp_reset_shutdown

DESCRIPTION
  This function is called when the Bluetooth image is going
  to be unloaded.

===========================================================================*/
LOCAL void bt_qdsp_reset_shutdown
(
  void
)
{

#ifdef BT_SWDEV_RADIO_OFF
  if ( bt_dc.radio_state != BT_RADIO_DISABLING )
    /* leave the upper layers as is when turning radio off */
#endif
  {
#ifdef FEATURE_BT_AG
    bt_ag_qdsp_reset_shutdown();
#endif
    bt_sd_qdsp_reset_shutdown();
    bt_rc_qdsp_reset_shutdown();
    bt_l2_qdsp_reset_shutdown();
    bt_rm_qdsp_reset_shutdown();
  }

  bt_hc_qdsp_reset_shutdown();
  bt_se_qdsp_reset_shutdown();
  bt_lm_qdsp_reset_shutdown();
  bt_bb_qdsp_reset_shutdown();
  bt_qd_qdsp_reset_shutdown();
  bt_qq_qdsp_reset_shutdown();

#ifdef BT_SWDEV_RADIO_OFF
  if ( bt_dc.radio_state == BT_RADIO_DISABLING )
  {
    /* fully reset lower layers when turning off radio */
    bt_hc_reset();
    bt_se_reset();

    if ( BT_IN_HCI_MODE()) 
    {
      /* Re-init encryption/authentication related data structures */
      bt_se_reinit();

      /* Re-init LM data */
      bt_lm_data_init();
    }
    bt_lm_reset();
    bt_bb_reset();
    bt_qd_reset();
    bt_qq_reset();
  }
#endif

  /* Stop polling QDSP events */
#ifdef BT_SWDEV_QDSP_POLLING
  rex_clr_timer( &bt_qdsp_poll_timer );
  rex_clr_sigs( &bt_tcb, BT_QDSP_POLL_SIG );
#endif

  rex_clr_timer( &bt_background_timer );
  rex_clr_sigs( &bt_tcb, BT_BACKGROUND_TIMER_SIG );

#ifdef FEATURE_BT_L2FLUSH
  rex_clr_timer( &bt_l2flush_tick_timer);
  rex_clr_sigs( &bt_tcb, BT_L2FLUSH_TIMER_SIG );
#endif

  switch ( bt_dc.driver_substate )
  {
    /* The QDSP should be in a good state now.  Proceed w/ shutdown */
    case BT_DSS_SWAP_OUT_TO_IDLE:
      bt_dc.driver_state    = BT_DS_IDLE;
      bt_dc.driver_substate = BT_DSS_NONE;

      bt_disable_hardware();

      /* Driver is shutdown.  Tell QDSP services we no longer need it. */
      bt_dc.dsp_image_requested = BT_DSP_IMAGE_MAX;
      BT_DSP_DISABLE();
      break;

    case BT_DSS_SWAP_OUT_TO_RESET:
      /* Keep state to reinit after we allow QDSP to be swapped out. */

      bt_disable_hardware();

      /* Driver is shutdown.  Tell QDSP services we no longer need it. */
      bt_dc.dsp_image_requested = BT_DSP_IMAGE_MAX;
      BT_DSP_DISABLE();
      break;

    case BT_DSS_SWAP_OUT_TO_STANDBY:
    case BT_DSS_SWAP_OUT_TO_READY:
      bt_dc.driver_state    = BT_DS_QDSP_OUT;

      /* Now that the image is swapped out, request the most desired image,
      ** if one is desired, and if it is not already requested.
      */
      if ( ( bt_dc.dsp_image_desired != BT_DSP_IMAGE_MAX ) &&
           ( bt_dc.dsp_image_desired !=
             bt_dc.dsp_image_requested ) )
      {
        /* This must be called before allow_reset because that function
        ** will pre-empt this flow.  That function will cause our qdsp
        ** services cb to be called, which may enable other qdsp image.
        */
        bt_dc.dsp_image_requested = bt_dc.dsp_image_desired;
        BT_DSP_ENABLE( bt_dc.dsp_image_requested );
      }

      /* Driver is shutdown.  Allow QDSP to reset */
      BT_DSP_ALLOW_RESET();

      /* Keep clocks running for awhile in case Bluetooth comes right
      ** back in when we are surviving a swap.
      */
      rex_set_timer( &bt_clocks_monitor_timer, BT_CLOCKS_MONITOR_TIMEOUT_MS );
      break;

    default:
      BT_ERR( "Unexpected driver substate %d",
              bt_dc.driver_substate, 0, 0 );
  }

#ifdef BT_SWDEV_RADIO_OFF
  if ( bt_dc.radio_state == BT_RADIO_DISABLING )
  {
    bt_dc.radio_state = BT_RADIO_DISABLED;
    
    /* Generate Radio Disabled event only if not in HCI/APIT mode */
    if ( bt_cmd_dc_get_hci_mode() == BT_HCIM_OFF )
    {
      bt_rm_radio_disabled();               
    }
    /* In HCI/APIT mode */
    else
    {
      /* Need to bring DSP right back in, i.e, do the second half of HCI reset */
      bt_dc.radio_state = BT_RADIO_ENABLING;
      bt_dc_enable_driver();
    }
  }
#endif

  if ( bt_dc.driver_substate == BT_DSS_SWAP_OUT_TO_RESET )
  {
    /* We are resetting, so now that we are down, come back up. */
    bt_dc.driver_state    = BT_DS_IDLE;
    bt_dc.driver_substate = BT_DSS_NONE;

    /* About to reinit driver.  Need to do some extra house keeping.
    */
    rex_clr_timer( &bt_rpt_timer );
    rex_clr_timer( &bt_requeued_cmd_q_timer );
    rex_clr_timer( &bt_wait_timer );

#ifdef BT_SWDEV_QXDM_LOG_PER
    rex_clr_timer( &bt_dm_timer );
#endif

    bt_cmd_dc_set_hci_mode( BT_HCIM_OFF );

    bt_task_init();
    bt_task_start();
    bt_hc_finish_reset();   /* do something special for HCI reset */
  }
}


/*===========================================================================

FUNCTION
  bt_process_qdsp_ready_sig

DESCRIPTION
  Processes the QDSP ready signal.  This signal is set when the requested
  Bluetooth QDSP image is loaded.

===========================================================================*/
LOCAL void bt_process_qdsp_ready_sig
(
  void
)
{
#ifdef T_MSM6250
#error code not present
#endif /* T_MSM6250 */

  switch ( bt_dc.driver_state )
  {
    case BT_DS_PENDING_QDSP_DOWNLOAD:
      bt_initialize_hardware();
      break;

    case BT_DS_QDSP_OUT:
      switch ( bt_dc.driver_substate )
      {
        case BT_DSS_SWAP_OUT_TO_READY:
          bt_dc.driver_state = BT_DS_QDSP_SWAPPING_IN;

          /* Init HW and try to reconnect to prior active links. */
          bt_enable_hardware();
          bt_bb_qdsp_swapped_in();
          break;

        case BT_DSS_SWAP_OUT_TO_STANDBY:
          bt_dc.driver_state = BT_DS_PENDING_QDSP_DOWNLOAD;

          /* Initialize Bluetooth hardware. */
          bt_initialize_hardware();
          break;
      }
      break;

    case BT_DS_IDLE:
    case BT_DS_PENDING_HW_INIT:
    case BT_DS_READY:
    case BT_DS_QDSP_SWAPPING_OUT:
    case BT_DS_DSP_SLEEPING:
    default:
      BT_ERR( "Unexpected driver state %d", bt_dc.driver_state, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_process_qdsp_reset_sig

DESCRIPTION
  Processes the QDSP reset signal.  This signal is set when the current
  Bluetooth image is going to be unloaded.

===========================================================================*/
LOCAL void bt_process_qdsp_reset_sig
(
  void
)
{
#ifdef T_MSM6250
#error code not present
#endif /* T_MSM6250 */

  BT_MSG_DEBUG( "BT: MDSP Proc RESET DS %x SS %x QS %x",
                bt_dc.driver_state, bt_dc.driver_substate, 
                bt_bb_data.qdsp_state );

  switch ( bt_dc.driver_state )
  {
    case BT_DS_QDSP_SWAPPING_OUT:
      switch ( bt_dc.driver_substate )
      {
        /* The QDSP should be in a good state now.  Proceed w/ shutdown */
        case BT_DSS_SWAP_OUT_TO_IDLE:
        case BT_DSS_SWAP_OUT_TO_RESET:
        case BT_DSS_SWAP_OUT_TO_READY:
        case BT_DSS_SWAP_OUT_TO_STANDBY:
          bt_qdsp_reset_shutdown();

#ifdef T_MSM6250
#error code not present
#endif /* T_MSM6250 */
          break;
        default:
          BT_ERR( "Unexpected driver substate %d",
                  bt_dc.driver_substate, 0, 0 );
      }
      break;

    case BT_DS_DSP_SLEEPING:
      BT_MSG_DEBUG( "BT: Force BT core Wakeup via HW Wr!", 0, 0, 0 );

      /* Bring BT core out of sleep immediately */
#if (defined(T_MSM6100) || defined(T_MSM6300) ||  \
     defined(T_MSM6500) || defined(T_MSM6550)) && !defined(T_MSM6800)
      /* For 6100, 6300, 6500 and 6550 */
      HWIO_OUTM( WEB_MISC_WR, 
                 HWIO_WEB_MISC_WR_BT_SLEEP_STOP_CMD_BMSK, 
                 0x1 << HWIO_WEB_MISC_WR_BT_SLEEP_STOP_CMD_SHFT );
#elif defined(T_MSM6250) && !defined(T_MSM6275) && !defined(T_MSM6280)
#error code not present
#elif defined(T_MSM6800)
      /* For 6800 */
      HWIO_OUTM( BT_SLP_CTL_CMD, 
                 HWIO_BT_SLP_CTL_CMD_STOP_SLEEP_BMSK, 
                 0x1 << HWIO_BT_SLP_CTL_CMD_STOP_SLEEP_SHFT );
#endif

      bt_dc.driver_substate = BT_DSS_SWAP_OUT_TO_READY;

      break;

    case BT_DS_READY:
      bt_dc.driver_state = BT_DS_QDSP_SWAPPING_OUT;

      /* Swap requested while we are using Bluetooth.
      ** Try to maintain the link.
      */
      bt_dc.driver_substate = BT_DSS_SWAP_OUT_TO_READY;
      bt_bb_qdsp_swapping_out();

      break;

    case BT_DS_PENDING_HW_INIT:
      /* The driver is bringing up the hardware when the image is trying
      ** to be swapped out.  Allow reset after the hardware is up to
      ** prevent corner cases.
      */
      bt_dc.driver_substate = BT_DSS_RESET_AFTER_READY;
      break;

    case BT_DS_IDLE:
    case BT_DS_PENDING_QDSP_DOWNLOAD:
    default:
      BT_ERR( "Unexpected driver state %d", bt_dc.driver_state, 0, 0 );
  }
}
#endif /* FEATURE_BT_SOC */

#ifdef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_soc_ready_init

DESCRIPTION
  This function is called by the  Bluetooth host when it wants to start
  the external SOC

===========================================================================*/
void bt_soc_ready_init
(
  void
)
{
  /*Start the persistent background timer for monitoring any required 
   * events. This timer is restarted only if "current BT state"
   * requires it. Otherwise the previously existing bt_background_timer 
   * ( defined in rex default group and hence does not run during power
   * collapse ) is started.
   */
  (void) rex_clr_timer( &bt_background_timer );
  bt_dc.persistent_bg_tick_active = TRUE;
  (void) rex_set_timer( &bt_persistent_background_timer, BT_BACKGROUND_TIMER_MS );

#ifdef FEATURE_BT_L2FLUSH
  rex_set_timer( &bt_l2flush_tick_timer, BT_L2FLUSH_TIMER_MS );
#endif

}

/*===========================================================================
FUNCTION
  bt_remove_requeued_commands

DESCRIPTION
  This function discards any remaining cmds in the bt_cmd_re_q.

===========================================================================*/
LOCAL void bt_remove_requeued_commands
(
  void
)
{

  int               cmd_count;
  uint16            i;
  bt_cmd_msg_type*  cmd_ptr;

  cmd_count = q_cnt( &bt_cmd_re_q );

  for ( i = 0; i < cmd_count; i++ )
  {
    if ( (cmd_ptr = (bt_cmd_msg_type*)(q_get( &bt_cmd_re_q ))) != NULL )
    {
      BT_ERR( "BT: Discarding reqd cmd CSt %x Typ %x AID %x",
                     cmd_ptr->cmd_hdr.cmd_state,
                     cmd_ptr->cmd_hdr.cmd_type,
                     cmd_ptr->cmd_hdr.bt_app_id );
      bt_initialize_cmd_buffer( cmd_ptr );
      q_put( &bt_cmd_free_q, &cmd_ptr->cmd_hdr.link );
    }
  }
}

/*===========================================================================
FUNCTION
  bt_soc_shutdown

DESCRIPTION
  This function is called by the  Bluetooth host when it wants to shut
  down the external SOC

===========================================================================*/
void bt_soc_shutdown_cleanup
(
  void
)
{
  /* Clear all the timers defined in bt_init_always_on_timers() */
  rex_clr_timer( &bt_soc_driver_idle_timer );
  rex_clr_sigs( &bt_tcb, BT_DRIVER_IDLE_TIMER_SIG );

  bt_remove_requeued_commands();
  rex_clr_timer( &bt_requeued_cmd_q_timer );
  rex_clr_sigs( &bt_tcb, BT_REQUEUED_CMD_Q_SIG );

  rex_clr_timer( &bt_persistent_background_timer );
   /*Stop the background timer for monitoring any required events */
  rex_clr_timer( &bt_background_timer );
  rex_clr_sigs( &bt_tcb, BT_BACKGROUND_TIMER_SIG );
  bt_dc.persistent_bg_tick_active = FALSE;

#ifdef FEATURE_BT_L2FLUSH
  rex_clr_timer( &bt_l2flush_tick_timer);
  rex_clr_sigs( &bt_tcb, BT_L2FLUSH_TIMER_SIG );
#endif

  bt_rm_soc_reset_shutdown();
  bt_l2_soc_reset_shutdown();
  bt_rc_soc_reset_shutdown();
  bt_sd_soc_reset_shutdown();
  bt_ag_soc_reset_shutdown();
}
#endif /* FEATURE_BT_SOC */

#ifdef BT_SWDEV_QXDM_LOG_PER
/*===========================================================================

FUNCTION
  bt_send_bt_log

DESCRIPTION
  Sends the general Bluetooth log packet.

===========================================================================*/
void bt_send_bt_log
(
  void
)
{

  log_bt_gen_type*  log_ptr;

  log_ptr = (log_bt_gen_type*)
            log_alloc( log_bt_gen, sizeof( log_bt_gen_type ) );

  if ( log_ptr != NULL )
  {
#ifndef FEATURE_BT_SOC
    log_ptr->bt_driver_state = (uint8) bt_dc.driver_state;
#endif /* FEATURE_BT_SOC */
    log_ptr->bt_hci_mode     = (uint8) bt_dc.cur_hci_mode;
    log_commit( log_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_send_logs

DESCRIPTION
  Sends all Bluetooth log packets.  This will be called periodically
  when the Bluetooth driver is enabled.

===========================================================================*/
void bt_send_logs()
{

#ifdef FEATURE_BT_AG
  bt_ag_snapshot_logging_function();
#endif
  bt_rc_send_log();
  bt_sd_snapshot_logging_function();
  bt_l2_send_log();

#ifndef FEATURE_BT_SOC
  if ( BT_IS_IN_QDSP() )
  {
    bt_lm_snapshot_logging_function();
    bt_bb_send_log();
  }
#endif /* FEATURE_BT_SOC */

  bt_send_bt_log();

}
#endif /* BT_SWDEV_QXDM_LOG_PER */


#ifdef FEATURE_BT_TEST_MODE_ONLY
/*===========================================================================

FUNCTION
    bt_output_rf_test_mode_display

DESCRIPTION
   For BT RF Test Mode Only release, this function will put a message
   on UI display.

===========================================================================*/
LOCAL void bt_output_rf_test_mode_display
(
  void
)
{

  char message[ 1 + HS_SCRN_WIDE * 2 ] = "Bluetooth   Test Mode   ";

#ifdef T_MSM_5100
  lcd_debug_message( message );
#else
#error code not present
#endif

}
#endif /* FEATURE_BT_TEST_MODE_ONLY */


#ifndef FEATURE_BT_SOC
#ifdef BT_QDSP_DEBUG
#error code not present
#endif /* BT_QDSP_DEBUG */


/*===========================================================================

FUNCTION
  bt_clear_pending_bb_timer_sig

DESCRIPTION
  - Clears the pending baseband sig 
  - Re-enables dsp sleep if all pending sigs are processed.
  This function is entered only when BT_IS_IN_QDSP.

===========================================================================*/
LOCAL void bt_clear_pending_bb_timer_sig
(
  rex_sigs_type  sig
)
{
  
  /*  If we are here because we could not process the      */
  /*  timer signal before, we need to re-enable DSP sleep  */
  /*  if all the pending signals have been processed.      */
  if ( bt_pending_bb_timer_sigs & sig )
  {
    BT_MSG_DEBUG ( "BT: Process skipped BB Timer Sig %x", sig, 0, 0 );
    bt_pending_bb_timer_sigs &= ~sig;

    if ( bt_pending_bb_timer_sigs == 0 )
    {
      BT_MSG_DEBUG ( "BT: Enabled DSP sleep", 0, 0, 0 );
      *bt_qd_bt_enable_sleep_mode_ptr = BT_QD_ENABLE_SLEEP_MODE;          
    }
  }

}


/*===========================================================================

FUNCTION
  bt_process_bb_timer_sig_safely

DESCRIPTION
  This function ensures that timer handler code which may access DSP memory
  is invoked only when safe (i.e., when BT_IS_IN_QDSP).

===========================================================================*/
LOCAL void bt_process_bb_timer_sig_safely
(
  rex_sigs_type  sig
)
{
  
  uint32 isave;
  
  INTLOCK_SAV( isave );
  if ( BT_IS_IN_QDSP() )
  {
    INTFREE_SAV( isave );  
    bt_clear_pending_bb_timer_sig( sig );

    /*  Now it is safe to enter BT driver code.  */
    /*  Call the timer handler for the signal.   */
    if ( sig == BT_SE_AUTH_TIMER_SIG )
    {
      bt_se_auth_timer_expired( sig );
    }
    else if ( sig == BT_LMP_TIMER_SIG )
    {
      bt_lmp_process_timer_signal();
    }
    else if ( sig == BT_LMP_TIMER_2_SIG )
    {
      bt_lmp_process_timer_2_signal();
    }
  }
  else 
  {
    bt_pending_bb_timer_sigs |= sig;
    INTFREE_SAV( isave );
#ifndef FEATURE_BT_1_2
    BT_MSG_DEBUG( "BT: DSP sleeping. Skip BB Tmr Sig %x", sig, 0, 0 );
#endif
  }  

}


/*===========================================================================

FUNCTION
  bt_process_event_queues

DESCRIPTION
  This processes all of the events on all of the BT driver event queues.

===========================================================================*/
LOCAL void bt_process_event_queues
(
  void
)
{
  uint16 event_q_mask;

  INTLOCK();
  event_q_mask = bt_event_q_mask;
  bt_event_q_mask = 0x0000;
  INTFREE();
  
  if ( event_q_mask & BT_EVQ_HC_B )
  {
    bt_hc_process_ev_queue();
  }

  if ( event_q_mask & BT_EVQ_RM_B )
  {
    bt_rm_process_ev_queue();
  }

  if ( event_q_mask & BT_EVQ_L2_B )
  {
    bt_l2_process_ev_queue();
  }

  if ( event_q_mask & BT_EVQ_RC_B )
  {
    bt_rc_process_ev_queue();
  }

  if ( event_q_mask & BT_EVQ_SIO_B )
  {
    bt_sio_process_ev_queue();
  }

  if ( event_q_mask & BT_EVQ_SD_B )
  {
    bt_sd_process_ev_queue();
  }

#ifdef FEATURE_BT_AG
  if ( event_q_mask & BT_EVQ_AG_B )
  {
    bt_ag_process_ev_queue();
  }
#endif /* FEATURE_BT_AG */

#if defined ( FEATURE_IBT ) || defined (FEATURE_IBT_DYNAMIC)
  if ( event_q_mask & BT_EVQ_BREW_SPP_B )
  {
    bt_ec_dispatch_brew_spp_ev();
  }
#endif /* FEATURE_IBT  || FEATURE_IBT_DYNAMIC*/

}

#else /* FEATURE_BT_SOC */
/*===========================================================================

FUNCTION
  bt_process_event_queues

DESCRIPTION
  This processes all of the events on all of the BT driver event queues.

===========================================================================*/
LOCAL void bt_process_event_queues
(
  void
)
{
  uint16 event_q_mask;

  INTLOCK();
  event_q_mask = bt_event_q_mask;
  bt_event_q_mask &= (~BT_EVQ_HC_B);
  INTFREE();
  
  if ( event_q_mask & BT_EVQ_HC_B )
  {
    bt_hc_process_ev_queue();
  }

  INTLOCK();
  event_q_mask = bt_event_q_mask;
  bt_event_q_mask &= (~BT_EVQ_RM_B);
  INTFREE();

  if ( event_q_mask & BT_EVQ_RM_B )
  {
    bt_rm_process_ev_queue();
  }

  INTLOCK();
  event_q_mask = bt_event_q_mask;
  bt_event_q_mask &= (~BT_EVQ_L2_B);
  INTFREE();
  if ( event_q_mask & BT_EVQ_L2_B )
  {
    bt_l2_process_ev_queue();
  }

  INTLOCK();
  event_q_mask = bt_event_q_mask;
  bt_event_q_mask &= (~BT_EVQ_RC_B);
  INTFREE();

  if ( event_q_mask & BT_EVQ_RC_B )
  {
    bt_rc_process_ev_queue();
  }

  INTLOCK();
  event_q_mask = bt_event_q_mask;
  bt_event_q_mask &= (~BT_EVQ_SIO_B);
  INTFREE();
  if ( event_q_mask & BT_EVQ_SIO_B )
  {
    bt_sio_process_ev_queue();
  }

  INTLOCK();
  event_q_mask = bt_event_q_mask;
  bt_event_q_mask &= (~BT_EVQ_SD_B);
  INTFREE();
  if ( event_q_mask & BT_EVQ_SD_B )
  {
    bt_sd_process_ev_queue();
  }

#ifdef FEATURE_BT_AG
  INTLOCK();
  event_q_mask = bt_event_q_mask;
  bt_event_q_mask &= (~BT_EVQ_AG_B);
  INTFREE();
  if ( event_q_mask & BT_EVQ_AG_B )
  {
    bt_ag_process_ev_queue();
  }
#endif /* FEATURE_BT_AG */

#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
  INTLOCK();
  event_q_mask = bt_event_q_mask;
  bt_event_q_mask &= (~BT_EVQ_BREW_SPP_B);
  INTFREE();
  if ( event_q_mask & BT_EVQ_BREW_SPP_B )
  {
    bt_ec_dispatch_brew_spp_ev();
  }
#endif /* FEATURE_IBT  || FEATURE_IBT_DYNAMIC*/

  bt_hc_drv_process_bytes_from_sio();


}
#endif /* FEATURE_BT_SOC */


/*===========================================================================

FUNCTION
  bt_task

DESCRIPTION
  This is the Bluetooth driver task.

===========================================================================*/
void bt_task
(
  dword ignored
  /* Parameter received from Rex - ignored */
  /*lint -esym(715,ignored)
  ** Have lint not complain about the ignored parameter 'ignored' which is
  ** specified to make this routine match the template for rex_init().
  */
)
{

  rex_sigs_type  rex_signals;
  uint32         isave;

  /*  Initialize the BT task and prepare to receive commands.    */
  /*  This MUST NOT include interactions with other subsystems.  */
  bt_task_init();

  /*  Tell MC we are created and wait for start signal.  */
#if (defined(T_MSM6100) || defined(T_MSM7200))
  task_start( BT_RPT_TIMER_SIG, DOG_BT_RPT, &bt_rpt_timer );
#else
#error code not present
#endif

  /*  Start the BT task including interactions with other subsystems.  */
  bt_task_start();

  rex_clr_sigs( rex_self(), BT_TASK_START_SIG );

  while ( 1 )  /*  Never exit this loop.  */
  {
    rex_signals = rex_wait( bt_wait_sig_mask );

#ifdef BT_BG_TIMER_DEBUG
        BT_MSG_DEBUG ( "BT: BT Task loop %x", 
                       rex_signals, 0, 0);
#endif

#ifndef FEATURE_BT_SOC
    if ( rex_signals & BT_CLOCKS_SHUTDOWN_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_CLOCKS_SHUTDOWN_SIG );
      bt_disable_hardware();
    }
#endif /* FEATURE_BT_SOC */

    if ( rex_signals & BT_REQUEUED_CMD_Q_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_REQUEUED_CMD_Q_SIG );
      bt_process_requeued_commands( TRUE );
    }

    if ( rex_signals & BT_RPT_TIMER_SIG )
    {
      dog_report( DOG_BT_RPT );

#ifndef FEATURE_BT_SOC
#ifdef BT_QDSP_DEBUG
#error code not present
#endif
#endif /* FEATURE_BT_SOC */

      (void) rex_set_timer( &bt_rpt_timer, DOG_BT_RPT_TIME );
    }

    if ( rex_signals & (BT_TASK_STOP_SIG | BT_TASK_OFFLINE_SIG) )
    {
      rex_clr_sigs( &bt_tcb, (BT_TASK_STOP_SIG | BT_TASK_OFFLINE_SIG) );
      if ( rex_signals & BT_TASK_STOP_SIG )
      {
        MSG_MED("BT got stop sig",0,0,0);
      }
      else
      {
        MSG_MED("BT got offline sig",0,0,0);
      }
#ifndef FEATURE_BT_SOC
      //bt_qdsp_reset_shutdown();
#endif /* FEATURE_BT_SOC */
#if (defined( T_MSM6100 ) || defined( T_MSM7200 )) 
      task_ack();
#else
#error code not present
#endif
    }
#ifndef FEATURE_BT_SOC
    if ( rex_signals & BT_QDSP_READY_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_QDSP_READY_SIG );
      bt_process_qdsp_ready_sig();
    }

    if ( rex_signals & BT_QDSP_RESET_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_QDSP_RESET_SIG );
      bt_process_qdsp_reset_sig();
    }
#endif /* FEATURE_BT_SOC */

#ifdef FEATURE_BT_SOC
    /* Call SoC interface to handle SoC specific signals */
    if (rex_signals & BT_SOC_SIG_ALL)
    {
      if (NULL != bt_soc_ifc.bt_soc_proc_soc_sigs_func_ptr)
      {
        bt_soc_ifc.bt_soc_proc_soc_sigs_func_ptr(rex_signals);
      }
    }
#endif /* FEATURE_BT_SOC */


    if ( rex_signals & BT_SD_TIMER_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_SD_TIMER_SIG );
      bt_sd_process_timer_expiration();
    }

    if ( rex_signals & BT_SIO_RX_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_SIO_RX_SIG );
      bt_hc_pe_parse_host_msg();
    }

#ifdef FEATURE_BT_SOC      
    if ( rex_signals & BT_SIO_SOC_RX_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_SIO_SOC_RX_SIG );
      /* Process HCI byte stream received from Host Controller SoC via Sio */
      bt_hc_drv_process_hci_rx();
    }
#endif /* FEATURE_BT_SOC */

    if ( rex_signals & BT_CMD_Q_SIG )
    {
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
      /* Check if we are ready to send out cmds from the ACL\HCI WM */
      bt_hc_drv_process_cmd_from_wm();
      bt_hc_drv_process_acl_from_wm();
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */
        
      bt_process_requeued_commands( FALSE );
      
      bt_process_command_queue();

      /*  Clear the BT command Q signal as appropriate.  This   */
      /*  is done below because BT commands can generate other  */
      /*  BT commands which sets the signal again.  However,    */
      /*  bt_process_command_queue() processes all commands on  */
      /*  the queue, effectively processing these internally    */
      /*  generated commands immediately.  The check below      */
      /*  prevents unnecessary task wake-ups and calls to       */
      /*  bt_process_command_queue().                           */
      INTLOCK_SAV( isave );
      if ( q_cnt( &bt_cmd_q ) == 0 )
      {
        rex_clr_sigs( &bt_tcb, BT_CMD_Q_SIG );

#ifdef FEATURE_BT_QSOC_SLEEP
        bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_BT_Q_EMPTY, TRUE);
#endif /* FEATURE_BT_QSOC_SLEEP */

      }
      INTFREE_SAV( isave );
    }

    if ( rex_signals & BT_RC_TX_TIMER_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_RC_TX_TIMER_SIG );
      bt_rc_tx_timer_running = FALSE;
      bt_rc_tx_timer_expired();
    }

#ifdef BT_SWDEV_QDSP_POLLING
  #define QDSP_SIGS  ( BT_QDSP_EVENT_Q_SIG | BT_QDSP_POLL_SIG )
#else
  #define QDSP_SIGS  ( BT_QDSP_EVENT_Q_SIG )
#endif

#ifndef FEATURE_BT_SOC
    if ( rex_signals & QDSP_SIGS )
    {
      rex_clr_sigs( &bt_tcb, QDSP_SIGS );
#ifdef BT_SWDEV_QDSP_POLLING
      rex_set_timer( &bt_qdsp_poll_timer, BT_QDSP_POLL_INTERVAL_MS );
#endif

      if ( BT_IS_IN_QDSP() )
      {
        bt_bb_process_qdsp_events();
      }

      if ( BT_IS_IN_QDSP() )
      {
        bt_qdsp_tx_buf_try_send_data();
        bt_qdsp_buf_ctrl_report_bytes_for_all_dev();
      }
    }
#endif /* !FEATURE_BT_SOC */


    if ( rex_signals & BT_EVENT_Q_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_EVENT_Q_SIG );
      bt_process_event_queues();
    }

#ifdef FEATURE_BT_AG
    if ( rex_signals & BT_AG_RX_DATA_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
      bt_ag_check_rx_wm_data();
    }

    if ( rex_signals & BT_AG_TIMER_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_AG_TIMER_SIG );
      bt_ag_timer_expired();
    }
#endif /* FEATURE_BT_AG */

#ifndef FEATURE_BT_SOC
    if ( rex_signals & BT_SE_AUTH_TIMER_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_SE_AUTH_TIMER_SIG );
      bt_process_bb_timer_sig_safely( BT_SE_AUTH_TIMER_SIG );
    }

    if ( rex_signals & BT_LMP_TIMER_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_LMP_TIMER_SIG );
      bt_process_bb_timer_sig_safely( BT_LMP_TIMER_SIG );
    }

    if ( rex_signals & BT_LMP_TIMER_2_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_LMP_TIMER_2_SIG );
      bt_process_bb_timer_sig_safely( BT_LMP_TIMER_2_SIG );
    }
#endif /* FEATURE_BT_SOC */

    /* 
     * bt_persistent_background_timer defined in a BT timer group and
     * bt_background_timer is defined in rex default group. If 
     * bt_persistent_background_timer is running it will  always result 
     * in exit from power collapse at timer expiry. This can have adverse 
     * current consumption impact. So persistent timer is restarted only
     * during BT states that require "reliable background tick timing". 
     * Otherwise, the previously existing bt_background_timer will provide
     * the BT tick timing as before.
    */
    if ( rex_signals & BT_BACKGROUND_TIMER_SIG )
    {
      /* Clear both background timers and restart which ever is required */
      (void) rex_clr_timer( &bt_background_timer );
      (void) rex_clr_timer( &bt_persistent_background_timer );
      (void) rex_clr_sigs( &bt_tcb, BT_BACKGROUND_TIMER_SIG );

      bt_ag_background_tick();
      bt_sio_background_tick();
      bt_rc_background_tick();
      bt_l2_background_tick();
      bt_rm_background_tick();

#ifdef FEATURE_BT_SOC
      bt_hc_drv_background_tick();
#else
      bt_lm_background_tick();
#endif /* FEATURE_BT_SOC */

      /*
       * TBD : If any other module utilizing bg tick  
       * (AG/RC/L2/SIO) requires reliable background   
       * tick, additional checks need to be added here 
      */
      if ( bt_rm_persistent_tick_required() == TRUE )
      {
        bt_dc.persistent_bg_tick_active = TRUE;
        (void) rex_set_timer( &bt_persistent_background_timer,
                              BT_BACKGROUND_TIMER_MS );
#ifdef BT_BG_TIMER_DEBUG
        BT_MSG_DEBUG ( "BT: Restarting persistent bg timer", 
                       0, 0, 0);
#endif
      }
      else
      {
        bt_dc.persistent_bg_tick_active = FALSE;
        (void) rex_set_timer( &bt_background_timer,
                              BT_BACKGROUND_TIMER_MS );
#ifdef BT_BG_TIMER_DEBUG
         BT_MSG_DEBUG ( "BT: Restarting bg timer", 
                        0, 0, 0);
#endif
      }
    }
    else if ( bt_dc.persistent_bg_tick_active != TRUE ) 
    { 
#ifdef BT_BG_TIMER_DEBUG
        BT_MSG_DEBUG ( "BT: persistent tick Not running.", 
                       0, 0, 0);
#endif
      /* SIG is not set and persistent bg tick is not active. 
       * Start  bt_persistent_background_timer with remaining interval in
       * bt_background_timer if persistent tick is required.   
       * TBD : If any other module utilizing bg tick  
       * (AG/RC/L2/SIO) requires reliable background   
       * tick, additional checks need to be added here 
      */
      if ( bt_rm_persistent_tick_required() == TRUE )
      {
        rex_timer_cnt_type remaining_interval;
        /* Save bt_background_timer current count and clear it. */
        remaining_interval = rex_get_timer ( &bt_background_timer );
        (void) rex_clr_timer ( &bt_background_timer );

        if ( remaining_interval == 0 )
        {
#ifdef BT_BG_TIMER_DEBUG
          /* bt_background_timer must have expired after local copy of 
           * rex_signals was updated. Ensure BT_BACKGROUND_TIMER_SIG is set.  
           * Processing in next task loop will restart the required timer.
          */
          BT_MSG_HIGH ( "BT: Setting BT_BACKGROUND_TIMER_SIG ", 
                       0, 0, 0);
#endif
          (void) rex_set_sigs( &bt_tcb, BT_BACKGROUND_TIMER_SIG ) ;
        }
        else
        {
          bt_dc.persistent_bg_tick_active = TRUE;
          (void) rex_clr_sigs( &bt_tcb, BT_BACKGROUND_TIMER_SIG );
          (void) rex_set_timer( &bt_persistent_background_timer,
                                remaining_interval );
#ifdef BT_BG_TIMER_DEBUG
        BT_MSG_DEBUG ( "BT: persistent tick started interval %x", 
                       remaining_interval, 0, 0);
#endif
        }
      }
    }

#ifdef FEATURE_BT_L2FLUSH
    if ( rex_signals & BT_L2FLUSH_TIMER_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_L2FLUSH_TIMER_SIG );
      (void) rex_set_timer( &bt_l2flush_tick_timer,
                            BT_L2FLUSH_TIMER_MS );
      /* Dont Run the tick timer if in HCI Mode */
      if ( !BT_IN_HCI_MODE() )
      {
#ifdef FEATURE_BT_AVSYNC_SOC
        bt_hc_drv_l2flush_timer_tick();
#else
        bt_l2flush_timer_tick();
#endif
      }
     }
#endif
    if ( rex_signals & BT_L2_SIG_MSG_RESP_TIMER_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_L2_SIG_MSG_RESP_TIMER_SIG );
      bt_l2_expected_resp_timer_expired();
    }

#ifndef FEATURE_BT_SOC
    if ( rex_signals & BT_QDSP_STANDBY_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_QDSP_STANDBY_SIG );
      if ( bt_dc.dsp_dont_unload == FALSE || 
           bt_dc.radio_state     == BT_RADIO_DISABLING)
      {
        if ( BT_IS_IN_QDSP() )
        {
          bt_bb_send_qdsp_cmd( BT_QDSP_DISABLE_RFU_CMD, BT_QDSP_GEN_IDX );
        }
        else
        {
          bt_dc_disable_driver();
        }
      }
    }
#endif /* !FEATURE_BT_SOC */

#ifdef BT_SWDEV_QXDM_LOG_PER
    if ( rex_signals & BT_DM_TIMER_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_DM_TIMER_SIG );
      bt_send_logs();
      (void) rex_set_timer( &bt_dm_timer, BT_DM_TIMER_MS );
    }
#endif /* BT_SWDEV_QXDM_LOG_PER */

    if ( rex_signals & BT_RDM_SIG )
    {

        rex_clr_sigs( &bt_tcb, BT_RDM_SIG );
        /* Process the rdm state change*/
        bt_dc_process_rdm_change();
      
    }

#ifdef BT_AUDIO_DEBUG
#error code not present
#endif /* BT_AUDIO_DEBUG */

#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

    if ( rex_signals & BT_DRIVER_IDLE_TIMER_SIG )
    {
      rex_clr_sigs( &bt_tcb, BT_DRIVER_IDLE_TIMER_SIG );
      bt_rm_proc_driver_idle_to();
    }
#endif /* FEATURE_BT_SOC */
  }

}


/*===========================================================================

FUNCTION
  bt_free_cmd_buffer

DESCRIPTION
  Puts Back command buffer

RETURN VALUE
  NONE

===========================================================================*/
void bt_free_cmd_buffer
(
  bt_cmd_msg_type*   bt_cmd_ptr
)
{

  if ( bt_cmd_ptr != NULL )
  {
      q_put( &bt_cmd_free_q, &bt_cmd_ptr->cmd_hdr.link );
  }
  else
  {
    BT_ERR( "BT: Bad cmd buffers", 0, 0, 0 );
  }

  return ;

}



/*===========================================================================

FUNCTION
  bt_get_free_cmd

DESCRIPTION
  Gets an available BT driver command buffer.

RETURN VALUE
  Pointer to free BT command buffer if available; otherwise NULL.

===========================================================================*/
bt_cmd_msg_type* bt_get_free_cmd
(
  void
)
{

  bt_cmd_msg_type*  free_bt_cmd_ptr = NULL;

  if ( q_cnt( &bt_cmd_free_q ) > 0 )
  {
    free_bt_cmd_ptr = (bt_cmd_msg_type*) q_get( &bt_cmd_free_q );
  }
  else
  {
    BT_ERR( "BT: Out of cmd buffers", 0, 0, 0 );
  }

  return( free_bt_cmd_ptr );

}


/*===========================================================================

FUNCTION
  bt_cmd

DESCRIPTION
  This function takes an already filled out bt_cmd_msg_type and places it
  on the BT command queue.  It is used by all of the routines in btcmd.c

===========================================================================*/
bt_cmd_status_type bt_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr
)
{

  boolean             free_cmd = TRUE;
  int                 bt_cmd_q_depth;
  bt_cmd_status_type  cmd_status;

  if ( ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, HC ) ) ||
       ( BT_CE_GROUP( bt_cmd_ptr->cmd_hdr.cmd_type, FCTL ) ) ||
       ( bt_cmd_ec_app_id_is_registered(
           bt_cmd_ptr->cmd_hdr.bt_app_id ) != FALSE ) )
  {
    if ( bt_cmd_ptr->cmd_hdr.cmd_state == BT_CST_DONE )
    {
      if ( (bt_cmd_q_depth = q_cnt( &bt_cmd_q )) < BT_NUM_CMDS )
      {
        bt_cmd_ptr->cmd_hdr.cmd_state  = BT_CST_QUEUED;
        cmd_status = BT_CS_GN_PENDING;

        /*  Queue command and signal BT task.  */
        q_put( &bt_cmd_q, &bt_cmd_ptr->cmd_hdr.link );
        (void) rex_set_sigs( &bt_tcb, BT_CMD_Q_SIG );

        free_cmd = FALSE;

#ifdef FEATURE_BT_QSOC_SLEEP
        if (0 != q_cnt( &bt_cmd_q ))
        {
            /* This Q_NON_EMPTY evt alone is not sufficient to trigger any Sleep state */
            /* change, hence we don't need to  update Sleep signalling/state change.  */
            /*  Also bt_cmd() could be called from Non-BT task & should be re-entrant */
            bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_BT_Q_NONEMPTY, FALSE);
        }
#endif /* FEATURE_BT_QSOC_SLEEP */

      }
      else
      {
        cmd_status = BT_CS_GN_CMD_Q_FULL;
        BT_ERR( "BT: Cmd Q Full- Depth %x", bt_cmd_q_depth, 0, 0 );
      }
    }
    else
    {
      cmd_status = BT_CS_GN_BAD_CMD_STATE;
      BT_ERR( "BT: Bad Cmd St %x AID %x Typ %x",
              bt_cmd_ptr->cmd_hdr.cmd_state,
              bt_cmd_ptr->cmd_hdr.bt_app_id,
              bt_cmd_ptr->cmd_hdr.cmd_type );
    }
  }
  else
  {
    cmd_status = BT_CS_GN_BAD_APP_ID;
    BT_ERR( "BT cmd: AID %x not regd cmd %x",
            bt_cmd_ptr->cmd_hdr.bt_app_id,
            bt_cmd_ptr->cmd_hdr.cmd_type, 0 );
  }

  if ( free_cmd != FALSE )
  {
    bt_initialize_cmd_buffer( bt_cmd_ptr );
    q_put( &bt_cmd_free_q, &bt_cmd_ptr->cmd_hdr.link );
  }

  return cmd_status;
}

/*===========================================================================

FUNCTION
  bt_dsm_pushdown_tail

DESCRIPTION
  Calls dsm_pushdown_tail. Does additional error checking to insure bytes
  were pushed down, and tries other DSM pools if the first one fails.

RETURN VALUE
  None.

===========================================================================*/
boolean bt_dsm_pushdown_tail
(
  dsm_item_type** dsm_ptr_ptr,
  const void *buf,
  uint16 size,
  dsm_mempool_id_enum_type pool_id
)
{
  uint16 bytes_pushed;
  boolean success = TRUE;

  bytes_pushed = dsm_pushdown_tail( dsm_ptr_ptr, buf, size, pool_id );

  if ( bytes_pushed != size )
  {
    BT_MSG_HIGH("BT: Error with pushdown. Size=%x Sm=%x Lrg=%x",
                size,
                DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL));


    bytes_pushed += dsm_pushdown_tail( dsm_ptr_ptr, 
                                       (uint8*)buf  + bytes_pushed,
                                       size - bytes_pushed,
                                       DSM_DS_OTHER_POOL_ID( pool_id ) );
    if ( bytes_pushed != size )
    {
      BT_ERR_FATAL("BT: No room left in DSMs", 0, 0, 0);
      success = FALSE;
    }
  }
  return success;
}

#ifdef FEATURE_BT_ALIGN_SIO_DATA
/*==========================================================================
FUNCTION
BT_DSM_ALIGN_BUFFER

DESCRIPTION
Before sending data to SIO ensure that data is aligned correctly on
memory boundary. This function takes advantage of the fact that the data
in new DSM buffers is correctly aligned. The alignment is taken care of by
pushing the data from an unaligned DSM buffer to a newly aligned buffer.

DEPENDENCIES
None.

INPUTS
The unaligned DSM packet.

RETURN VALUE
Number of bytes copied (aligned).

SIDE EFFECTS
If the copy from the old, unaligned buffer to the new, aligned buffer is
successful, the pointer to the input packet is modified to point to the
new packet and the old packet is freed.

==========================================================================*/
boolean bt_dsm_align_buffer
(
   dsm_item_type **pkt_ptr
)
{
   boolean aligned = FALSE;
   boolean needs_aligned = FALSE;
   dsm_item_type *new_pkt_ptr = NULL;
   dsm_item_type *temp_pkt_ptr = *pkt_ptr;

   /* Check for NULL DSM pointer */
   if (pkt_ptr != NULL)
   {
      /* Run through the linked list and check to see if any of the data */
      /* is not aligned on a 32 bit boundary */
      while (temp_pkt_ptr != NULL && needs_aligned == FALSE)
      {
         /* If the any of the data is misaligned */
         if (((uint32) (temp_pkt_ptr->data_ptr)) & 0x03)
         {
            /* Mark the entire buffer as needing aligned */
            needs_aligned = TRUE;
         }

         /* Move DSM header pointer to the next header */
         temp_pkt_ptr = temp_pkt_ptr->pkt_ptr;
      }

      /* Check to see if the packet needs to be aligned */
      if (needs_aligned == TRUE)
      {
         /* Set the temporary DSM header pointer to the head of the DSM packet */
         temp_pkt_ptr = *pkt_ptr;

         /* Run the linked list, get a new buffer, and copy the data into the */
         /* new buffer */
         do
         {
            /* Here's the trick - bt_dsm_pushdown_tail will allocate a new buffer */
            /* if the pointer to the input DSM buffer is NULL, the data in the new */
            /* buffer is automatically aligned by DSM, and the bt_dsm_pushdown_tail */
            /* will copy the data from our buffer (temp_pkt_ptr->data_ptr) into */
            /* the newly allocated and aligned buffer. In addition, if the */
            /* pointer to the input DSM buffer is not NULL, pushdown_tail attempts */
            /* to copy the data into the allocated buffer. If it's not large enough */
            /* more space is allocated, that space is aligned as well, and then it */
            /* is added to the packet */
            aligned = bt_dsm_pushdown_tail(&new_pkt_ptr,
                            temp_pkt_ptr->data_ptr, temp_pkt_ptr->used,
                            DSM_DS_SMALL_ITEM_POOL);

            /* If alignment succeeded */
            if (aligned == TRUE)
            {
               /* Increment the total number of bytes copied and move to the next */
               /* DSM header in the linked list */
               temp_pkt_ptr = temp_pkt_ptr->pkt_ptr;
            }
         } while (temp_pkt_ptr != NULL && aligned == TRUE);

         if (aligned == FALSE)
         {
            /* Must be out of memory, deallocate the new DSM packet, */
            /* if necessary */
            if (new_pkt_ptr != NULL)
            {
               dsm_free_packet(&new_pkt_ptr);
            }
            
            BT_ERR_FATAL("BT: No room left in DSMs", 0, 0, 0);
         }
         else
         {
            /* Otherwise the data has been aligned in a new DSM packet */
            /* Free the input packet */
            dsm_free_packet(pkt_ptr);

            /* Set the input packet pointer to point the new, aligned DSM */
            /* packet */
            *pkt_ptr = new_pkt_ptr;
         }
      }
      else
      {
         /* The input packet did not need any alignment, so return the success */
         aligned = TRUE;
      }
   }
   else
   {
      /* Pointer to the input packet is NULL */
      BT_ERR_FATAL("BT: NULL DSM Packet Pointer", 0, 0, 0);
   }
      
   return aligned;
}
#endif /* FEATURE_BT_ALIGN_SIO_DATA */

/*===========================================================================

FUNCTION
  bt_dsm_pushdown_packed

DESCRIPTION
  Calls dsm_pushdown_tail. Does additional error checking to insure bytes
  were pushed down, and tries other DSM pools if the first one fails.

RETURN VALUE
  None.

===========================================================================*/
boolean bt_dsm_pushdown_packed
(
  dsm_item_type** dsm_ptr_ptr,
  void *buf,
  uint16 size,
  dsm_mempool_id_enum_type pool_id
)
{
  uint16 bytes_pushed;
  boolean success = TRUE;

  bytes_pushed = dsm_pushdown_packed( dsm_ptr_ptr, buf, size, pool_id );

  if ( bytes_pushed != size )
  {
    BT_MSG_HIGH("BT: Error with pushdown. Size=%x Sm=%x Lrg=%x",
                size,
                DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL));


    bytes_pushed += dsm_pushdown_packed( dsm_ptr_ptr, 
                                         (uint8*)buf  + bytes_pushed,
                                         size - bytes_pushed,
                                         DSM_DS_OTHER_POOL_ID( pool_id ) );
    if ( bytes_pushed != size )
    {
      BT_ERR_FATAL("BT: No room left in DSMs", 0, 0, 0);
      success = FALSE;
    }
  }
  return success;
}


/*===========================================================================

FUNCTION
  bt_get_free_dsm_ptr

DESCRIPTION
  Retrieves a dsm pointer and sets the data_ptr to correct starting place
  for specified top layer of BT stack to be used.

RETURN VALUE
  Pointer to free DSM buffer.

===========================================================================*/
dsm_item_type* bt_get_free_dsm_ptr
(
  bt_top_layer_type  top_layer,
  uint16             max_payload_bytes
)
{

  dsm_item_type*            dsm_ptr = NULL;

#ifdef FEATURE_BT_NEW_DSMS
  dsm_mempool_id_enum_type  dsm_pool_id;
  uint16                    hdr_size = 0;
  uint16                    ftr_size = 0;

  switch ( top_layer )
  {
    case BT_TL_RFCOMM:
      {
        hdr_size = BT_RC_MAX_HEADER_BYTES + BT_L2_MAX_HEADER_BYTES;
        ftr_size = BT_RC_MAX_FOOTER_BYTES + BT_L2_MAX_FOOTER_BYTES;
        BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TL_RC );
        break;
      }
    case BT_TL_L2CAP:
      {
        hdr_size = BT_L2_MAX_HEADER_BYTES;
        ftr_size = BT_L2_MAX_FOOTER_BYTES;
        BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TL_L2 );
        break;
      }
    case BT_TL_HCI_BB:
      {
        BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TL_BB );
        break;
      }
#ifdef FEATURE_BT_EXTPF_AV
    case BT_TL_AVDTP:
      {
        hdr_size = BT_AVDTP_MAX_HEADER_BYTES + BT_RC_MAX_HEADER_BYTES 
          + BT_L2_MAX_HEADER_BYTES;
        ftr_size = BT_AVDTP_MAX_FOOTER_BYTES + BT_RC_MAX_FOOTER_BYTES 
          + BT_L2_MAX_FOOTER_BYTES;
        BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TL_AVDTP );
        break;
      }
    case BT_TL_A2DP:
      {
        hdr_size = BT_A2DP_MAX_HEADER_BYTES + BT_AVDTP_MAX_HEADER_BYTES 
          + BT_RC_MAX_HEADER_BYTES + BT_L2_MAX_HEADER_BYTES;
        ftr_size = BT_A2DP_MAX_HEADER_BYTES + BT_AVDTP_MAX_FOOTER_BYTES 
          + BT_RC_MAX_FOOTER_BYTES + BT_L2_MAX_FOOTER_BYTES;
        BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TL_A2DP );
        break;
      }
#endif /* FEATURE_BT_EXTPF_AV */
    case BT_TL_OBEX:
      {
        hdr_size = BT_RC_MAX_HEADER_BYTES + BT_L2_MAX_HEADER_BYTES;
        ftr_size = BT_RC_MAX_FOOTER_BYTES + BT_L2_MAX_FOOTER_BYTES;
        BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TL_OB );
        break;
      }

    default:
      {
        BT_ERR( "BT get free DSM: Bad TL %x", top_layer, 0, 0 );
        break;
      }
  }

  dsm_pool_id = DSM_DS_POOL_SIZE( max_payload_bytes +
                                  hdr_size + ftr_size );

  if ( hdr_size != 0 )
  {
    dsm_ptr = dsm_offset_new_buffer( dsm_pool_id, hdr_size );
  }
  else
  {
    dsm_ptr = dsm_new_buffer( dsm_pool_id );
  }

  if ( dsm_ptr == NULL )
  {
    /* No more DSMs in this memory pool. Try the other pool */
    BT_MSG_HIGH("BT: Low DSMs. Sm=%x Lrg=%x cnt=%x",
                DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
                max_payload_bytes + hdr_size + ftr_size);

    dsm_pool_id = DSM_DS_OTHER_POOL_ID( dsm_pool_id );
    if ( hdr_size != 0 )
    {
      dsm_ptr = dsm_offset_new_buffer( dsm_pool_id, hdr_size );
    }
    else
    {
      dsm_ptr = dsm_new_buffer( dsm_pool_id );
    }
  }

#else /* Old DSMs below */

  dsm_ptr = dsm_new_buffer( DSM_DS_POOL_SIZE( max_payload_bytes ) );

#ifdef BT_DSM_MEM_CHK
  switch ( top_layer )
  {
    case BT_TL_L2CAP:
      BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TL_L2 );
      break;
    case BT_TL_RFCOMM:
      BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TL_RC );
      break;
    case BT_TL_HCI_BB:
      BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TL_BB );
      break;
    default:
      BT_ERR( "BT get free DSM: Bad TL %x", top_layer, 0, 0 );
      break;
  }
#endif /* BT_DSM_MEM_CHK */

#endif /* FEATURE_BT_NEW_DSMS */

  if ( dsm_ptr == NULL )
  {
    BT_ERR_FATAL( "BT get free DSM: Out of DSMs", 0, 0, 0 );
  }

  return( dsm_ptr );

}


/*===========================================================================

FUNCTION
  bt_write_to_efs

DESCRIPTION
  Saves the BT EFS parameters structure to EFS.
  
  NOTE:  This routine is only temporary to minimize changes
         for now.  It exists for backward compatibility.
  
============================================================================*/
boolean bt_write_to_efs
( 
  boolean  create_new_file  /*  No longer used.  */
)
{

  return ( bt_write_efs_params_file() );

}


#if defined (FEATURE_BT_AG) && defined (FEATURE_BT_ICON)
#error code not present
#endif /* (FEATURE_BT_AG && FEATURE_BT_ICON) */

#endif /* FEATURE_BT */


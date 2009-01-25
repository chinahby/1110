/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


        B L U E T O O T H    D R I V E R    C O N F I G U R A T I O N

                     C O M M A N D    I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the Driver Configuration (DC) command routines
  of the Bluetooth driver.

Copyright (c) 2000 - 2010 by QUALCOMM Incorporated. All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btdc.c#6 $
  $DateTime: 2010/05/12 22:07:33 $
  $Author: uppalas $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-05-06   us  Disbale Scans when intiating Remote Name request or
                   Create Connection for some SOC versions.
  2010-04-05   rk  Fix to udpate the sio_state after powerdown  
  2009-06-03   jn  Fix BTS powerdown on 7K.
  2009-04-27   vk  Configuring GPIOs for 76xx.
  2009-04-14   pn  Watermark queues get initialized by dsm_queue_init().
  2008-10-31   PG  Added implementation for bt_cmd_dc_is_bt_on()for SOC builds.
  2008-10-27   sa  Added support to read BT 2.1 support from a nv item in 
                   NV browser at run-time.
  2008-07-28   sa  Added support to read Clk type,Clk sharing,in-band sleep mode, 
                   soc logging enabled, BT Wlan co-ext, BTS power mgmt mode  
                   from NV browser at run-time.
  2008-07-28   jn  Fix EOL issue.
  2008-07-03   jn  Add calls to configure GPIOs for different BT state. This 
                   is required to enable QSOC_POWERDOWN on 6290/46. These 
                   configs are removed from TLMM driver.
  2008-04-30   sa  Modified SIO stream mode parameter to use GENERIC_MODE
                   instead of PKT_MODE
                   (Reason is in PKT_MODE the RX ISR buffers queues rx data
                   and only forwards to upper layers when certain threshold is
                   met.This may result in excessive delay & memory on ULC) 
  2008-04-02   jn  Initialize SoC when using inband sleep and QSOC POWERDOWN is
                   not defined.
  2008-03-13   jn  Created APIs for allowing/not allowing MSM VDD minimization.

 *
 *   #93        10 Mar 2008            JN
 * Fix end of line issues due to last checkin (#92).
 *
 *   #92        07 Mar 2008            JN
 * BT modifications to enable VDD minimization for 6246 and 6290.
 *
 *   #91        26 Feb 2008            SSK
 * -Reenable H/W flow control on MSM TX path to handle unexpected flow off by SOC.
 * -Fix MSM-SOC baudrate mismatch when bt_dc_disable_driver()\sio_close()\sio_open()
 *  is done with FEATURE_BT_QSOC_POWERDOWN not enabled.
 *
 *   #90        01 Feb 2008            JN
 * Fix compile errors caused due to certain combination of features.
 *
 *   #89        10 Jan 2008            SSK
 * Fix SIO wakeup issue due to "glitch" on UARTDM Rx line. UARTDM sio port is
 * opened or closed depending on whether BT SOC is powered on/off.
 * 
 *   #88        29 Nov 2007            SSK
 * Implemented Inband Sleep mechanism under FEATURE_BT_QSOC_INBAND_SLEEP
 *  
 *   #87        27 Nov 2007       BK
 * Moved update of driver state to ENABLING in bt_dc_enable_driver() until after 
 * bt_dc_power_on_soc() is called.  Also, baud rate init is not necessary when 
 * FEATURE_BT_QSOC_POWERDOWN is not defined since it is done when the port is opened. 
 * 
 *   #86        20 Nov 2007            PC 
 * Added ONCRPC meta comments for BT commands. 
 *
 *   #85        02 Nov 2007            BK
 * Clear driver idle timer when entering HCI mode to avoid turning off QSOC when
 * in HCI mode.  Set Driver state to ACTIVE when exiting HCI mode to allow 
 * driver idle timeout logic to turn off QSOC if possible.
 *
 *    #84       01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *   #83        11 Oct 2007            BH
 * QSoC Driver State state machine now powers on QSoC at startup
 * rather than after the first BT Command.  This puts the QSoC into
 * a low power mode rather than an idle mode.   
 *
 *   #82        11 Sep 2007            BK
 * Implement QSOC Driver State state machine.  
 *
 *   #81        06 AUG 2007            PG
 * Validated stream id and logged BT error before calling sio APIs to  
 * prevent ASSERT in SIO code.
 *
 *   #80        03 Jul 2007            SSK
 * Fix HCI_RESET bug in HCI APIT Mode on BTS targets :- Change to exclude
 * Power-down or Power-on of the SOC during Radio-On/Off done due to HCI RST.
 *
 *   #79        30 May  2007           BH
 * Removed featurization so that both SOC1 and QSOC use SIO_CTSRFR_AUTO_FCTL instead of
 * SIO_CTSRFR_FCTL for their SIO flow control method.
 *
 *   #78        25 May  2007           DSN
 * Allow FEATURE_WLAN_COEXISTENCE to be defined even without EFATURE_WLAN
 * being defined.
 *
 *   #77        21 May  2007           SSK
 *  Fixed compiler warnings on RVCT2.2 compiler.
 *
 *   #76        25 Apr  2007           BH
 * Added FEATURE_BT_WLAN_COEXISTENCE to allow FEATURE_WLAN to be enabled, but
 * BT/WLAN coexistence to be disabled.
 *
 *   #75        19 Apr 2007            PG
 * Fixed missing semi colon in #73.
 *
 *   #74        16 Apr 2007            SSK
 * Add support to enable core dump capture on ROM based BC4 Socs through
 * SPI interface. The feature FEATURE_BT_SOC1_BC4_DEBUG has to be enabled
 * on a SURF build to enable core dump capture on  BC4 SOC. 
 *
 *   #73        13 Apr  2007           JN/DSN
 * Added bt_dc_sio_change_baud(). Fixed radio on/off support for BTS4020.
 *
 *   #72        13 Apr  2007           DSN
 * Fix enums used in handling RDM port mapping close command.
 *
 *   #71        12 Apr  2007           BH
 * Updated revision history.
 *
 *   #70        12 Apr  2007           BH
 * Moved SoC RX hi/lo WM callbacks so that they are featurized
 * under FEATURE_BT_SOC.
 *
 *   #69        04 Apr  2007           BH
 * Following mods:
 * - Added two new SoC RX hi/lo WM callbacks.
 * - Added events processing for QSoC sleep state machine. 
 *
 *   #68        02 Apr 2007            DSN
 * Fix to handle back-to-back HCI and SOC rdm port mappping commands.
 *
 *   #67        14 Mar  2007           JN
 * Following mods:
 * - Soc feature based header file inclusion.
 * - For HCI mode, start at BT_SOC_SIO_BITRATE. Changed sio baud rate macros. 
 * - Moved feature based power-on sequence from bt_dc_sio_open to
 *   bt_dc_soc_startup.
 * - Moved bt_soc_ifc initialization to bt_soc_ifc_init.
 *
 *   #66        06 Mar  2007           JN
 * Added support for SoC abstraction layer along with support for BTS.
 * Made functions bt_soc_enable_flow and bt_soc_disable_flow global.
 * 
 *   #65         23 Feb 2007           DSN
 * Modifications to re-enable HCI mode for SoC solutions.
 *
 *   #64         21 Feb 2007           AP
 *   Updated enable/disable vreg code for SC2X 
 *   Featurized sio ioctl sleep/wakeup calls under FEATURE_SIO_CLIENT_SLEEP_CONTROL
 *
 *   #63         20 Feb 2007           DSN
 * Sizing the Don'tExceedCount for Sio->MSM watermark to prevent bytes 
 * lost immediately after flow-off
 *
 *   #62         09 Feb  2007          HM
 *   Moved all definitions to change bit rate of UART in SOC based solution 
 *   to a central place.
 * 
 *   #61         08 Feb 2007           PG
 * Changed sio_ioctl cmd names to be compatible with SIO VU 
 * MSMSHARED_SIO.00.01.02 
 *
 *   #60         01 Feb 2007           DM
 * Prevent enabling/disabling inbound flow if it is already enabled / disabled.
 *
 *   #59         31 Jan 2007           PG
 * Enabled automatic HW flow control for UART1DM to prevent SIO overrun when
 * interrupts are locked for more 500 us. Now the UART1DM HW automatically
 * asserts UART HW flow control before the FIFO fills up.
 *
 *   #58         31 Jan 2007           ACG
 * Added a delay between receipt of yms and voting to sleep.
 *
 *   #56         17 Jan 2007           ACG
 * 7500 related changes:
 * - Shutoff Vreg by default on FFAs
 * - Change featurization - replace T_FFA by FEATURE_BT_SOC1_ROM
 *
 *   #55         10 Jan 2007           GS
 * Added FTM-BT support for multi-processor targets.
 *
 *   #54         11 Jan 2007           HM
 * Added feature so that BT can be disabled using an NV item for 
 * SOC based targets.
 *
 *    #53       10 Jan 2007            ACG
 * Fix compile issues on 6k targets caused by 7500 related code check-ins.
 *
 *    #52        9 Jan 2007            ACG
 * Re-enable SIO sleep vote for 7500 SURFs.
 *
 *    #51        9 Jan 2007            PG
 * Moved bt_soc_etc_powerup_init() to bt.c.
 * Cleanup and temporary workaround to get 7500 SURF build working for bt.
 *
 *    #50        8 Jan 2007            ACG
 * Following features were added for the SOC solution:
 * - vote for SIO sleep/wakeup based on BT activity
 * - enable/disable driver
 * - radio on/off
 * 
 *   #49         15 Dec 2006           PG
 * bt_wlan_coex_soc1_cb cleanup. Changed to block 20 BT channels on either
 * side of the centre ferquency corresponding to WLAN channel ( 11 in
 * previous implementation )
 *
 *   #48         16 Nov 2006           SSK
 * Changes to restrict the initial UART1DM baudrate of 115.2k  to  FFA only.
 *
 *   #47         24 Oct 2006           PG
 * Implemented call back function and cb registration with WLAN for 
 * Host Channel Classification update.
 *
 *   #46         24 Oct 2006           PG
 * Fixed compile error and fixed more issues with SOC featurization.
 *
 *   #45         24 Oct 2006           PG
 * Fixed issues with SOC featurization in previous checkin.
 *
 *   #44         20 Oct 2006           SSK
 * External SOC  changes in SIO Open & merge HCI Comand Flow control of DM
 *
 *   #43         09 Oct 2006           GS
 * Fixing compilation issues in enabling APIT and BT 1.2 feature 
 * on 7K target.
 *
 *   #42         29 Sep 2006           DSN
 * Fix to update HCI mode after sio port mapping
 *
 *   #41         27 Sep 2006           DSN
 * Change UART baud rate for SOC interface to 3.2Mbps 
 *
 *   #40         24 Aug 2006           DSN
 * Support HCI test mode, RF test mode and HCI logging on SOC solution
 *
 *   #39         11 Aug 2006           GB
 * Modified to support APIT mode for ASIA 
 *
 *   #38         25 July 2006           PG
 * Changed "image reset" logic so that DSP shared memory varriables are
 * accessed during image reset handling only on 1x targets where dsp
 * image can be swapped out/in.
 *
 *   #37         27 Jun 2006            DSN
 * Added featurization to support HostController SoC solution.
 * Support sio port to UART1DM for SoC communication.
 *
 *    #36        25 Jun 2006            DSN
 * Removed RC_2_SIO related references that are no longer used.
 *
 *    #35        20 Jun 2006            JH
 * Added calls to support GPIO configuration for sleep on 6550.
 *
 *    #34        11 May 2006            JH
 * Added calls to support Dynamic Clock and Voltage Scaling (DCVS) on 6550.
 *
 *    #33        12 Dec 2005            DSN
 * Further Changes for MSM7500 BT bringup. RVCT compiler error fixes.
 *
 *    #32        02 Dec 2005            DSN
 * Changes for MSM7500 BT bringup.
 *
 *    #31        03 Aug 2005            GS
 * Mainlined mDSP reset handling functionality for UMTS targets.
 *
 *    #30        25 Jul 2005            GS
 * Updated bt_cmd_dc_set_hci_mode() to use BT_IN_HCI_MODE macro.
 *
 *    #29        04 May 2005            RY
 * Removed temporary bringup code for MSM6275.
 *
 *    #28        06 Apr 2005            JH
 * Adding support for disabling BT radio, featurized using BT_SWDEV_RADIO_OFF.
 *
 *    #27        26 Mar 2005            JTL
 * Better parameter checking.
 *
 *    #26        24 Mar 2005            RY
 * Added support for MSM6275.
 *
 *    #25        28 Feb 2005            JH
 * Resize bt_mdsp_isr_table for 6800 using the T_MSM6800 featurization.
 *
 *    #24        24 Feb 2005            JH
 * Rolled back change #23. Saving HCI mode setting may disable the driver
 *  permanently in production builds. This should only be allowed in test mode.
 *
 *    #23        24 Feb 2005            JH
 * Unfeaturized code to save HCI mode setting in EFS.
 *
 *    #22        05 Jan 2005            JH
 *  Added command bt_cmd_dc_is_bt_on().
 *
 *    #21        16 Dec 2004            JTL
 * Put critical sections rather than TASKLOCKs around code in bt_dc_rdm_open
 * and _close. This prevents calling rex_wait() while tasks are locked.
 *
 *    #20        30 Nov 2004            GS
 * Fixed mdsp reset handling during image swap.
 *
 *    #19        05 Oct 2004            JTL
 * Allowing HCI mode to use USB.
 *
 *    #18        02 Sep 2004            JTL
 * Making calls to bt_cmd_ec_* not occur within TASKLOCKs.
 *
 *    #17        16 Jul 2004            RY
 * Update bt_dc_rdm_open to support UART3.
 *
 *    #16        6  Apr 2004            PN
 * Added bt_cmd_dc_get_hci_mode().
 *
 *    #15        26 Sep 2003            DP
 * Added call to sio_close within rdm_close().
 *
 *    #14        12 Sep 2003            RY
 * Added bt_cmd_dc_ftm_get_bt_wm.
 *
 *    #13        26 Aug 2003            RY
 * Updated bt_dc_rdm_open and bt_dc_rdm_close to generate the RDM_CLOSE
 * signal instead of calling bt_cmd_dc_set_hci_mode directly.  This is
 * to fix the problem of writing to EFS while in a TASKLOCK.
 * 
 *    #12        22 Jul 2003            RY
 * Updated bt_cmd_dc_set_hci_mode to save the current HCI mode to EFS
 * 
 *    #11        02 Jul 2003            RY
 * Corrected MDSP_USES_MODULES featurization for MSM6500.
 *
 *    Rev 1.31   04 Sep 2002 11:35:50   waynelee
 * Removed temp no mdsp svc code.  Fixed MDSP svc code.
 * 
 *    Rev 1.30   09 Aug 2002 18:53:50   waynelee
 * minor change to compile for 5xxx targets
 * 
 *    Rev 1.29   09 Aug 2002 18:37:32   waynelee
 * Added MSM6100 support.  Not using MDSP services.
 * Temporarily not unloading driver for APIT mode
 * for 5200 and 6100, until MDSP services in place.
 * 
 *    Rev 1.28   07 Jun 2002 17:03:28   waynelee
 * Added temp code to support MSM5200 targets.
 * 
 *    Rev 1.27   08 Apr 2002 15:51:04   waynelee
 * Set QDSP_EVENT signal when the QDSP interrupt fires only if
 * the event was generated by the BT DSP.
 * 
 *    Rev 1.26   03 Apr 2002 13:23:56   propach
 * Adjusted RC2SIO watermarks.
 * 
 *    Rev 1.25   12 Feb 2002 14:16:18   waynelee
 * Modified swapping code to support multi-slaves.
 * 
 *    Rev 1.24   22 Jan 2002 15:46:38   amitj
 * Made some corrections and enhancements to image swapping code.
 * Removed the function bt_dc_new_image_requested().
 * 
 *    Rev 1.23   18 Jan 2002 14:14:22   amitj
 * Made a fix in bt_dc_process_qdsp_services_event() to call qdsp_enable() only
 * if necessary.
 * 
 *    Rev 1.22   04 Jan 2002 17:26:00   amitj
 * Defined new functions: bt_cmd_dc_new_image_requested(), bt_cmd_dc_allow_image
 * _swap(), bt_dc_send_image_swapping_out_ev() and bt_dc_send_image_restored_ev
 * 
 *    Rev 1.21   10 Dec 2001 15:32:30   amitj
 * Modified bt_dc_enable_driver() to not enable the clock regime, turn on GPIO
 * output or set sleep_bt_clock_rgm. This will be done in bt_enable_hardware()
 * as the second part of initializing the bluetooth driver.
 * 
 *    Rev 1.20   30 Nov 2001 10:03:18   propach
 * Fixed QDSP event queue check.
 * 
 *    Rev 1.19   29 Nov 2001 11:12:36   waynelee
 * Fixed FEATURE_BT_NEVER_SLEEP to not prevent keeping TCXO on.
 * 
 *    Rev 1.18   13 Nov 2001 13:35:34   ryuen
 * Updated bt_cmd_dc_set_hci_mode to request the vocoder image for RC2SIO mode
 * 
 *    Rev 1.17   02 Oct 2001 14:09:24   amitj
 * Modified the dony_exceed_cnt value for APIT operation to 5000. 
 * Modified the tx and rx flow values for APIT operation to SIO_CTSRFR_FCTL.
 * 
 *    Rev 1.16   14 Sep 2001 09:31:20   ryuen
 * Updated bt_cmd_dc_set_hci_mode to select the correct DSP image when
 * setting HCI APIT mode
 * 
 *    Rev 1.15   29 Aug 2001 14:06:34   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.14   15 Jul 2001 18:13:34   ryuen
 * Updated checkin comment
 * 
 *    Rev 1.13.1.2   14 Aug 2001 19:20:40   waynelee
 * Don't unload QDSP image if in HCI mode.
 * 
 *    Rev 1.13.1.1   18 Jul 2001 16:51:14   propach
 * Added APIT HCI mode loading the stand alone DSP image.
 * 
 *    Rev 1.13.1.0   29 Jun 2001 15:22:32   waynelee
 * No change.
 * 
 *    Rev 1.13   29 Jun 2001 11:31:20   waynelee
 * Added HCI mechanism to not automatically unload QDSP image.
 * 
 *    Rev 1.12   08 Jun 2001 17:07:52   waynelee
 * Added #include to compile for MSM5100
 * 
 *    Rev 1.11   08 Jun 2001 16:12:56   waynelee
 * Revamped loading and unloading of QDSP images to handle HCI
 * reset and QDSP swap outs cleanly (CR 17159).  Turn on all clks
 * before downloading the QDSP instead of after.
 * 
 *    Rev 1.10   30 May 2001 22:37:48   ryuen
 * Updated bt_cmd_dc_set_hci_mode to use 115200 as the baud rate for RC2SIO
 * 
 *    Rev 1.9   03 May 2001 12:43:56   waynelee
 * Added support for new HCI mode to load vocoder image.
 * Use BT only image for normal HCI mode.  (CR 16872)
 * 
 *    Rev 1.8   29 Mar 2001 08:34:42   waynelee
 * Fixed the BT  QDSP to ARM ISR to properly check if the QDSP is in
 * prior to reading from the QDSP memory.
 * 
 *    Rev 1.7   21 Mar 2001 16:47:06   waynelee
 * Created mechanism to change QDSP image.
 * 
 *    Rev 1.6   16 Mar 2001 20:32:08   ryuen
 * Removed bt_cmd_dc_set_auth_params
 * 
 *    Rev 1.5   03 Mar 2001 11:58:30   sramacha
 * Increased dont_exceed_cnt on SIO watermarks to make NA performance better.
 * 
 *    Rev 1.4   02 Mar 2001 00:50:54   ryuen
 * 1. Removed the call to enable the driver for the various HCI modes
 * 2. Added support for the automatic QDSP load and unload
 * 
 *    Rev 1.3   21 Feb 2001 13:29:26   waynelee
 * Use the TLMM module again for USB/BT selection.
 * 
 *    Rev 1.2   16 Feb 2001 16:10:38   propach
 * Completed EC routing of events by BT application ID.
 * 
 *    Rev 1.1   07 Feb 2001 10:16:52   propach
 * Reworked internal BT application IDs and made event control
 * route based on these.
 * 
 *    Rev 1.0   29 Jan 2001 17:06:06   propach
 * Initial revision.
 * 
 *    Rev 1.38   29 Jan 2001 15:34:22   propach
 * Updated low/high watermark levels for RC to SIO.
 * 
 *    Rev 1.37   18 Jan 2001 11:33:14   waynelee
 * Added msm.h include (needed until tlmm is fixed)
 * 
 *    Rev 1.36   10 Jan 2001 12:31:44   propach
 * Default SIO port for internal BT build options.
 * 
 *    Rev 1.35   10 Jan 2001 11:27:20   propach
 * Made set_hci_mode return error if no SIO port ID is present.
 * 
 *    Rev 1.34   21 Dec 2000 17:38:04   waynelee
 * Change default DSP image for HCI mode to the BT standalone image.  Update
 * wm callbacks for the RC2SIO speed test.  Removed usage of bt_record_fault().
 * 
 *    Rev 1.33   18 Dec 2000 10:03:00   propach
 * Always default SIO port with RDM defined.
 * 
 *    Rev 1.32   11 Dec 2000 18:02:10   waynelee
 * Changed tlmm calls to MSM_OUTM() until tlmm shadow is fixed
 * 
 *    Rev 1.31   11 Dec 2000 15:51:10   propach
 * Made port default with RDM work for RC2SIO as well.
 * 
 *    Rev 1.30   11 Dec 2000 15:36:56   propach
 * Fix previous RDM port defaulting.
 * 
 *    Rev 1.29   11 Dec 2000 12:22:36   waynelee
 * Moved the configuration of the SBI GPIOs from USB to BT from the
 * bt_go_oneline func to bt_dc_cmd_enable_driver().  It must be done
 * prior to the QDSP image from being downloaded, otherwise the RFU
 * will not be initialized properly.
 * 
 *    Rev 1.28   04 Dec 2000 11:28:38   propach
 * Made SIO port default to AUX or MAIN if need be with the RDM.
 * 
 *    Rev 1.27   29 Nov 2000 13:53:08   propach
 * Added RDM support and added TASKLOCK/FREE for DC commands.
 * 
 *    Rev 1.26   22 Nov 2000 12:16:44   waynelee
 * Added support for reset (not yet working).
 * 
 *    Rev 1.25   13 Nov 2000 15:53:44   waynelee
 * Featurized which DSP image is used in HCI mode
 * 
 *    Rev 1.24   06 Nov 2000 17:03:32   ryuen
 * Added bt_cmd_dc_get_bd_addr
 * 
 *    Rev 1.23   25 Oct 2000 09:50:42   waynelee
 * changes for new init procedure.  also save hci mode.
 * 
 *    Rev 1.22   09 Oct 2000 11:31:18   propach
 * Optimized loops in/out of DSP.  Made RC TX timer driven.  Made QDSP
 * events be interrupt driven.
 * 
 *    Rev 1.21   25 Sep 2000 21:24:12   propach
 * NA flow control fixes.  Add MC offline & stop signal handling.
 *
 *    Rev 1.20   22 Sep 2000 17:54:22   waynelee
 * 1. Updated bt_cmd_dc_disable_driver() to take the driver offline, instead
 *    of waiting for the QDSP image reset event, because QDSP services will
 *    not send the event if the image being swapped in and out are identical
 * 2. Added flow control for BT_RC_2_SIO
 *
 *    Rev 1.19   20 Sep 2000 17:33:16   ryuen
 * 1. Updated bt_cmd_dc_enable_driver() and bt_cmd_dc_disable_driver() to
 *    save the caller's app ID
 * 2. Updated bt_cmd_dc_set_bd_addr() to save the BD_ADDR in memory.  Also, it
 *    writes the BD_ADDR to the DSP only if the driver is online
 *
 *    Rev 1.18   19 Sep 2000 18:00:22   ryuen
 * 1. Fixed a problem in which the Bluetooth driver can go online automatically
 *    when the vocoder image is loaded
 * 2. Use the right priorities when loading the vocoder and standalone image
 *
 *    Rev 1.17   16 Sep 2000 16:00:16   propach
 * Fixed SIO flow control change.
 *
 *    Rev 1.16   16 Sep 2000 15:11:48   propach
 * Add flow control between top of RFCOMM and BB for NA.  Also avoid
 * queueing of payload data.
 *
 *    Rev 1.15   14 Sep 2000 19:25:30   ryuen
 * Updated bt_cmd_dc_enable_driver() and bt_cmd_dc_disable_driver() to
 * take an APP ID parameter
 *
 *    Rev 1.14   07 Sep 2000 22:12:28   ryuen
 * Integrated with QDSP Services
 *
 *    Rev 1.13   07 Sep 2000 16:55:04   propach
 * Fixed NA stuff.
 *
 *    Rev 1.12   07 Sep 2000 15:31:48   propach
 * Fixed multiple calls to set HCI mode and fixed unregistering in EC.
 *
 *    Rev 1.11   06 Sep 2000 11:14:38   ryuen
 * Avoid SIO open (temporary)
 *
 *    Rev 1.10   06 Sep 2000 10:35:20   propach
 * Moved SIO setup to DC.  Merged AG/HS.
 *
 *    Rev 1.9   31 Aug 2000 14:18:14   waynelee
 * Changes to support new BT debug MSG macros
 *
 *    Rev 1.8   31 Aug 2000 13:26:22   waynelee
 * Adding intermin auth params support.  Adding RM inquiry support.
 *
 *    Rev 1.7   30 Aug 2000 09:41:22   waynelee
 * Made changes for new BT_MSG macros
 *
 *    Rev 1.6   29 Aug 2000 08:24:44   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.5   28 Aug 2000 16:12:04   propach
 * Cleanup RFCOMM.  Add use of msg/err macros.  Add driver fault
 * recording and statistics gathering.
 *
 *    Rev 1.4   28 Aug 2000 10:33:30   ryuen
 * Added BT message
 *
 *    Rev 1.3   17 Aug 2000 16:28:14   propach
 * Made default FHS whitening ON and starting with standard HCI enabled.
 *
 *    Rev 1.2   17 Aug 2000 13:42:38   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.1   03 Aug 2000 08:41:10   waynelee
 * Adding Low Power Mode Support.  Code for UPF-3
 *
 *    Rev 1.0   02 Aug 2000 09:34:12   propach
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "task.h"
#include "rex.h"
#include "sdevmap.h"
#include "sio.h"
#include "bt.h"
#include "btcmdi.h"
#include "bthc.h"
#include "bti.h"
#include "btmsg.h"
#ifndef FEATURE_BT_SOC 
#include "btqddata.h"
#else
#include "pm.h"
#include "bthcdrv.h"
#include "btsoc.h"
#include "btqsocnvm.h"
#ifdef FEATURE_BT_SOC1
#error code not present
#elif (defined FEATURE_BT_QSOC)
#include "btqsoc.h"
#endif /* FEATURE_BT_SOC1 */
#include "btsocetc.h"
#ifdef FEATURE_BT_QSOC_SLEEP
#include "btqsocsleep.h"
#endif /* FEATURE_BT_QSOC_SLEEP */
#endif /* FEATURE_BT_SOC */
#include "btutils.h"
#include "bthcpe.h"
#include "btrm.h"
#include "assert.h"
#include "btdebug.h"
#include "sleep.h"
#include GPIO_H
#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm_bt.h"
#endif /* FEATURE_FACTORY_TESTMODE */
#include "clkregim.h"

#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#endif

#if defined( T_MSM6100 ) || defined( T_MSM6200 )
  #include "mdsp.h"
#elif defined( T_MSM33 )
#error code not present
#endif

#ifdef T_MSM_33
#include "tlmm.h"
#endif

#if defined( FEATURE_BT_SOC ) && defined( FEATURE_BT_WLAN_COEXISTENCE ) && \
    defined( FEATURE_WLAN )
#error code not present
#endif /* FEATURE_BT_SOC && FEATURE_BT_WLAN_COEXISTENCE &&  FEATURE_WLAN */

#define BT_MSG_LAYER BT_MSG_GN   /* necessary for btmsg.h */


#ifdef FEATURE_BT_DYNAMIC_TLMM
#error code not present
#endif /* FEATURE_BT_DYNAMIC_TLMM */


static sio_port_id_type  bt_sio_port_id_from_rdm[BT_RDM_MAX_SRVC];

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/
bt_driver_control_type  bt_dc;

bt_app_id_type  bt_dc_last_hci_ev_reg_app_id;

// protect against multiple people calling bt_cmd_dc functions:
LOCAL rex_crit_sect_type bt_dc_crit_sect;

// Temporary variable to get and set bd_addr
LOCAL bt_bd_addr_type bt_dc_local_bd_addr;

// Temporary variable to get and set reference clk type (ie 32 MHz or 19.2 MHz)
LOCAL bt_soc_refclock_type bt_dc_local_refclock_type;

// Temporary variable to get and set clk sharing type
LOCAL bt_soc_clock_sharing_type bt_dc_local_clock_sharing_type;

// Temporary variable to get and set inband_sleep_type
LOCAL bt_soc_inband_sleep_type bt_dc_local_inband_sleep_type;

// Temporary variable to get and set bt_soc_logging 
LOCAL uint8 bt_dc_local_soc_logging;

// Temporay variable to get and set bt_2_1_lisbon
LOCAL uint8 bt_soc_bt_2_1_lisbon_disabled;

//Temporary veriable to get and set bt_soc_version
LOCAL bt_qsoc_enum_type bt_dc_local_soc_version;

//debug variables
LOCAL boolean bt_debug_enable_sio_sleep_vote = TRUE;
LOCAL boolean bt_debug_enable_soc_shutdown   = TRUE;

LOCAL boolean bt_dc_sio_uart1dm_opened_once  = FALSE;

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#endif

#ifdef FEATURE_BT_SOC
  /* Maximum number of bytes that can be received in one UART Rx is 1024 bytes.
     1024 is based on current UART1DM FIFO architecture.*/
  #define BT_DC_MAX_UART_RX_BYTES  1024

  /* Worst case sio->BT watermark Don'tExceedCount calculation:
     Note - UART Sio drv issues Rx cmd before processing current Rx bytes 
     from its FIFO.
     HighWM-1 bytes already in sio->BT Watermark; BT_DC_MAX_UART_RX_BYTES
     put on watermark from current UART Rx; HighWM is exceeded, so 
     a flow-off ioctl command is sent to sio driver; Sio deasserts RTS 
     but upto BT_DC_MAX_UART_RX_BYTES - 1 bytes may be received from
     already issued Rx command before flow is stopped by SoC

     Diff. between HighWM and Don’tExceedCount >= BT_DC_MAX_UART_RX_BYTES + 
                                                  (BT_DC_MAX_UART_RX_BYTES -1)
*/
  /* Difference between HighWM and Don’tExceedCount for SIO to MSM watermark */
  #define BT_DC_SIO_TO_MSM_DEC_HW_DIFF ((2*BT_DC_MAX_UART_RX_BYTES -1) + 2)

  #define BT_DC_RX_WM_HI   4500
  #define BT_DC_RX_WM_LO   3000
  #define BT_DC_RX_WM_MAX  (BT_DC_RX_WM_HI + BT_DC_SIO_TO_MSM_DEC_HW_DIFF)

#endif /* FEATURE_BT_SOC */
                          
/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

#ifndef FEATURE_BT_SOC 
/*===========================================================================

ISR
  bt_q2a_isr

DESCRIPTION
  The QDSP to ARM Interrupt Service Routine for Bluetooth interrupts.

===========================================================================*/
void bt_q2a_isr( void )
{
  
  /*  Only set the signal if the QSDP is loaded and  */
  /*  if this interrupt was sent from the BT DSP.    */

  if ( BT_IS_IN_QDSP() && 
       ( *bt_qd_dsp_to_arm_bt_event_int_ptr == BT_QD_BT_INTERRUPT_SET ) )
  {
    *bt_qd_dsp_to_arm_bt_event_int_ptr = BT_QD_BT_INTERRUPT_CLEAR;
    rex_set_sigs( &bt_tcb, BT_QDSP_EVENT_Q_SIG );
  }

}

/*===========================================================================

FUNCTION
  bt_dc_get_dsp_image_type

DESCRIPTION
  Get QDSP image type corresponding to the BT image. 

===========================================================================*/
LOCAL bt_dsp_image_enum_type bt_dc_get_dsp_image_type
(
  bt_image_enum_type  bt_image
)
{

  bt_dsp_image_enum_type  dsp_image = BT_DSP_IMAGE_MAX;

  switch ( bt_image )
  {
    case BT_IMAGE_VOC:

      #if defined( T_MSM6100 )
        dsp_image = BT_DSP_IMAGE_MDSP_1X;
      #elif defined( T_MSM6200 )
#error code not present
      #else
#error code not present
      #endif

      break;

    case BT_IMAGE_BT_ONLY:

      #if defined( T_MSM6100 )
        dsp_image = BT_DSP_IMAGE_MDSP_1X;
      #elif defined( T_MSM6200 )
#error code not present
      #else
#error code not present
      #endif

      break;

#ifdef FEATURE_PHONE_VR
    case BT_IMAGE_BT_AND_VR:

      #if defined( T_MSM6100 )
        dsp_image = BT_DSP_IMAGE_MDSP_1X;
      #elif defined( T_MSM6200 )
#error code not present
      #else
#error code not present
      #endif

      break;
#endif

  }

  return ( dsp_image );

}


/*===========================================================================

FUNCTION
  bt_dc_is_bt_image

DESCRIPTION
  Returns TRUE if the image passed as param contains a Bluetooth image.

===========================================================================*/
LOCAL boolean bt_dc_is_bt_image
(
  bt_dsp_image_enum_type  dsp_image
)
{
  
  boolean  image_has_bt;

  if ( (dsp_image == BT_DSP_IMAGE_OTHER) ||
       (dsp_image == BT_DSP_IMAGE_MAX) )
  {
    image_has_bt = FALSE;
  }
  else
  {
    image_has_bt = TRUE;
  }

  return ( image_has_bt );

}


/*===========================================================================

FUNCTION
  bt_dc_process_dsp_image_ready

DESCRIPTION
  This function processes the QDSP/MDSP Services Image Ready event.

===========================================================================*/
void bt_dc_process_dsp_image_ready
(
  bt_dsp_image_enum_type  dsp_image
)
{
  
  BT_MSG_HIGH( "BT GN: RX Image %x READY", dsp_image, 0, 0 );

  /* Vote to enable mDSP clock regime */
  mdsp_dsp_wakeup( MDSP_APP_BT );

  /* Save new current DSP image. */
  bt_dc.dsp_image_current = dsp_image;

  if ( bt_dc.dsp_image_requested == dsp_image )
  {
    /* We requested this image.  Continue with BT init. */
    rex_set_sigs( &bt_tcb, BT_QDSP_READY_SIG );
  }
  else 
  {
    /* Did not request this image. */
    if ( bt_dc_is_bt_image( dsp_image ) != FALSE )
    {
      /* This image contains the Bluetooth image. */

      if ( bt_dc.dsp_image_desired != BT_DSP_IMAGE_MAX )
      {
        /* Another Bluetooth image is in when Bluetooth has requested
        ** an image.  Since we don't care which Bluetooth image is used, 
        ** let's use this one.  Enable it and wait for the QDSP reset event.
        */
        bt_dc.dsp_image_requested = dsp_image;
        BT_DSP_ENABLE( bt_dc.dsp_image_requested );
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_dc_process_dsp_image_reset

DESCRIPTION
  This function processes the QDSP/MDSP Services Image Reset event.

===========================================================================*/
void bt_dc_process_dsp_image_reset
(
  bt_dsp_image_enum_type  dsp_image
)
{
  
  BT_MSG_HIGH( "BT GN: RX Image %x RESET", dsp_image, 0, 0 );

  /* Set new current DSP image. */
  bt_dc.dsp_image_current = BT_DSP_IMAGE_MAX;

  if ( (bt_dc.dsp_image_desired != BT_DSP_IMAGE_MAX) &&
       (bt_dc.dsp_image_requested == dsp_image) )
  {
#ifdef T_MSM6250
#error code not present
#else
    /* Disable sleep during Image swap */
    /* Only required on 1x targets where image swap out/in */
    /* needs to ensure BT FW state is saved/restored */
    *bt_qd_bt_enable_sleep_mode_ptr = BT_QD_DISABLE_SLEEP_MODE;

    /* Our image is going to be swapped out.  Handle it. */
    rex_set_sigs( &bt_tcb, BT_QDSP_RESET_SIG );
#endif /* T_MSM6250 */
  }
  else
  {
    /* We did not request the image being reset out.  Allow it.
    */
    BT_DSP_ALLOW_RESET();
  }

}


#if defined( T_MSM6100 ) || defined( T_MSM6200 )
#ifndef FEATURE_MDSP_USES_MODULES
/*===========================================================================

FUNCTION
  bt_dc_mdsp_to_bt_image

DESCRIPTION
  Converts a mdsp_image_type to a bt_dsp_image_enum_type.

===========================================================================*/
LOCAL bt_dsp_image_enum_type bt_dc_mdsp_to_bt_image
(
  mdsp_image_type  mdsp_image
)
{
  bt_dsp_image_enum_type  dsp_image;

  switch ( mdsp_image )
  {

#ifdef T_MSM6100
    case MDSP_IMAGE_COUGAR:
      dsp_image = BT_DSP_IMAGE_MDSP_1X;
      break;
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_GPRS
#error code not present
#endif

    default:
      dsp_image = BT_DSP_IMAGE_OTHER;
      break;
  }

  return ( dsp_image );

}


/*===========================================================================

FUNCTION
  bt_dc_bt_to_mdsp_image

DESCRIPTION
  Converts a bt_dsp_image_enum_type to a mdsp_image_type.

===========================================================================*/
LOCAL mdsp_image_type bt_dc_bt_to_mdsp_image
(
  bt_dsp_image_enum_type  dsp_image
)
{
  
  mdsp_image_type  mdsp_image;

  switch ( dsp_image )
  {

#ifdef T_MSM6100
    case BT_DSP_IMAGE_MDSP_1X:
      mdsp_image = MDSP_IMAGE_COUGAR;
      break;
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_GPRS
#error code not present
#endif

    default:
      BT_ERR_FATAL( "BT DC: Non BT image enabled %x", dsp_image, 0, 0 );
      mdsp_image = MDSP_IMAGE_MAX;
      break;
  }

  return ( mdsp_image );

}

#else /* ! FEATURE_MDSP_USES_MODULES */

/*===========================================================================

FUNCTION
  bt_dc_process_mdsp_module_event

DESCRIPTION
  This function processes the MDSP Module event.

===========================================================================*/
LOCAL void bt_dc_process_mdsp_module_event
( 
  mdsp_event_data_type*  event_ptr
)
{
  
  /* TBD:
  **
  ** This bit of code should be cleaned up once all targets that
  ** this code supports moves over to MDSP modules instead of [QM]DSP
  ** images.
  ** 
  ** For now, we are mapping module events to images events by treating
  ** the image enabled/disabled as the image requested.
  */

  switch ( event_ptr->mod.mod_status )
  {
    case MDSP_MOD_READY:
      bt_dc_process_dsp_image_ready( bt_dc.dsp_image_requested );
      break;

    case MDSP_MOD_DISABLE:
      bt_dc_process_dsp_image_reset( bt_dc.dsp_image_requested );
      break;

    case MDSP_MOD_ENABLE_FAILED:
    default:
      BT_ERR_FATAL( "Download failed.  Should never happen.", 0, 0, 0 );
      break;
  }

}
#endif /* ! FEATURE_MDSP_USES_MODULES */


/*===========================================================================

FUNCTION
  bt_dc_process_mdsp_services_event

DESCRIPTION
  This function processes the MDSP Services events

===========================================================================*/
void bt_dc_process_mdsp_services_event
(
  mdsp_event_data_type* event_ptr
)
{
  
  switch( event_ptr->event )
  {
    case MDSP_EV_COMMAND:
      /* TBD */
      break;

#ifdef FEATURE_MDSP_USES_MODULES

    case MDSP_EV_MODULE:
      bt_dc_process_mdsp_module_event( event_ptr );
      break;

#else

    case MDSP_EV_IMAGE:
      if ( event_ptr->img.image_status == MDSP_IMAGE_READY )
      {
        bt_dc_process_dsp_image_ready
        ( 
          bt_dc_mdsp_to_bt_image( event_ptr->img.image ) 
        );
      }
      else if ( event_ptr->img.image_status == MDSP_IMAGE_RESET )
      {
        bt_dc_process_dsp_image_reset
        ( 
          bt_dc_mdsp_to_bt_image( event_ptr->img.image )
        );
      }
      break;

#endif /* FEATURE_MDSP_USES_MODULES */

    default:
      BT_ERR_FATAL( "Illegal QDSP Event type", 0, 0, 0 );
      break;
  }

}

#else /* T_MSM6100 || T_MSM6200 */
#error code not present
#endif /* T_MSM6100 || T_MSM6200 */

/*===========================================================================

FUNCTION
  bt_dc_enable_dsp

DESCRIPTION
  This function enables a particular DSP image.

===========================================================================*/
void bt_dc_enable_dsp
(
  bt_dsp_image_enum_type  dsp_image
)
{

#if defined( T_MSM6100 ) || defined( T_MSM6200 )

#ifdef FEATURE_MDSP_USES_MODULES
  mdsp_enable( MDSP_APP_BT, MDSP_MODULE_BT );
#else
  mdsp_enable( MDSP_APP_BT, bt_dc_bt_to_mdsp_image( dsp_image ) );
#endif /* FEATURE_MDSP_USES_MODULES */

#else
#error code not present
#endif /* T_MSM6100 || T_MSM6200 */

}

#else /* !FEATURE_BT_SOC */
/*===========================================================================

FUNCTION
  bt_dc_power_on_soc

DESCRIPTION
  This routine powers on the BT Controller SoC.

===========================================================================*/
void bt_dc_power_on_soc( void )
{

  /* Power-up SoC unless there is an ongoing HCI RST in APIT Test Mode
     in which case the SOC is already powered-on */
  if ( (NULL != bt_soc_ifc.bt_soc_powerup_func_ptr) &&
       (bt_hc_pe_is_hci_rst_started() == FALSE ) )
  {
    bt_soc_ifc.bt_soc_powerup_func_ptr();
  }
} /* bt_dc_power_on_soc */

/*===========================================================================

FUNCTION
  bt_dc_power_off_soc

DESCRIPTION
  This routine powers of the BT Controller SoC.

===========================================================================*/
void bt_dc_power_off_soc( void )
{

#ifdef T_QSC60X5
  /* Turn off power to SoC through GPIO */
  gpio_out(BT_PWR_EN, GPIO_LOW_VALUE);

  BT_MSG_DEBUG( "BT DC: Calling gpio_out - to power off SoC",0,0,0);

#else 
#error code not present
#endif // T_QSC60X5

  /* Wait for SoC to get powered off by Vreg/GPIO control */
    rex_sleep( 10 );

} /* bt_dc_power_off_soc*/


/*===========================================================================

FUNCTION
  bt_dc_soc_startup

DESCRIPTION
  This routine powers of the BT Controller SoC.

===========================================================================*/
void bt_dc_soc_startup( void )
{

#if defined( FEATURE_BT_SOC1_FLASH ) || defined( FEATURE_BT_SOC1_BC4_DEBUG )
#error code not present
#elif defined(FEATURE_BT_QSOC)
  /* The driver is enabled at startup on QSoC builds to avoid an issue
     where the BTS can be in an idle state at startup */
  bt_dc_enable_driver();

#elif defined(FEATURE_BT_SOC1_ROM)
#error code not present
#else
  /* HCI mode for any SOC - SOC will always be kept awake for HCI mode.
    MSM UART will also be kept awake */

  /* Power up SoC */
  bt_dc_power_on_soc();

  /* Bringup SOC */
  bt_hc_drv_soc_init();
#endif /* FEATURE_BT_SOC1_ROM */
} /* bt_dc_soc_startup */


/*===========================================================================

FUNCTION
  bt_task_rx_data_from_soc

DESCRIPTION
  Callback function executed when the SIO driver enqueues data onto the
  receive watermark carrying bytes from SOC for the BT task.  
  This sets a BT signal to indicate the RX SIO watermark has HCI bytes 
  for the BT task.

===========================================================================*/
LOCAL void bt_task_rx_data_from_soc
(
  void
)
{
  rex_set_sigs( &bt_tcb, BT_SIO_SOC_RX_SIG );

  /* Inband sleep SM doesn't depend on RX data to determine sleep state */
#if defined( FEATURE_BT_QSOC_SLEEP ) && !defined( FEATURE_BT_QSOC_INBAND_SLEEP )
  bt_qsoc_sleep_sm_eval_event (BT_QSOC_SLEEP_EVENT_SIO_RX_BUSY, FALSE);
#endif /* FEATURE_BT_QSOC_SLEEP */

} /* bt_task_rx_data_from_soc */

/*===========================================================================

FUNCTION
  bt_soc_enable_flow

DESCRIPTION
  This sets the UART1DM inbound flow to enabled, enabling the SOC to send more
  data to the BT host driver.

===========================================================================*/
void bt_soc_enable_flow
(
  void
)
{
  if( bt_dc.sio_in_flow_on_ext_soc != TRUE )
  {
#ifndef FEATURE_BT_QSOC_SLEEP
    BT_MSG_HIGH("BT DC: Enabling Flow to SoC sid = 0x%x",
                 bt_dc.sio_stream_id_ext_soc, 0, 0);
#endif /* !FEATURE_BT_QSOC_SLEEP */

    bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                     SIO_IOCTL_INBOUND_FLOW_ENABLE, NULL );
    bt_dc.sio_in_flow_on_ext_soc = TRUE;
  }
}

/*===========================================================================

FUNCTION
  bt_soc_disable_flow

DESCRIPTION
  This sets the UART1DM inbound flow to disabled, disabling the SOC from sendng
  more data to the BT host driver.

===========================================================================*/
void bt_soc_disable_flow
(
  void
)
{
  if( bt_dc.sio_in_flow_on_ext_soc != FALSE )
  {
#ifndef FEATURE_BT_QSOC_SLEEP
    BT_MSG_HIGH("BT DC: Disabling Flow to SoC sid = 0x%x",
                 bt_dc.sio_stream_id_ext_soc, 0, 0);
#endif /* !FEATURE_BT_QSOC_SLEEP */

    bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                     SIO_IOCTL_INBOUND_FLOW_DISABLE, NULL );
    bt_dc.sio_in_flow_on_ext_soc = FALSE;
  }
}


/*===========================================================================

FUNCTION
  bt_dc_sio_rx_lowater_cb

DESCRIPTION
  Callback function executed when the receive watermark carrying bytes from
  SOC for the BT task reaches the low count.
  This sets the UART1DM inbound flow to enabled, enabling the SOC to send more
  data to the BT host driver.

===========================================================================*/
LOCAL void bt_dc_sio_rx_lowater_cb
(
  void
)
{
  
  BT_MSG_HIGH("BT DC: SoC to MSM lo_watermark reached HiCnt=%d, LoCnt=%d, CurCnt=%d", 
             bt_soc_to_msm_sio_wm.hi_watermark,
             bt_soc_to_msm_sio_wm.lo_watermark,
             bt_soc_to_msm_sio_wm.current_cnt); 
  
  bt_soc_enable_flow();
}


/*===========================================================================

FUNCTION
  bt_dc_sio_rx_hiwater_cb

DESCRIPTION
  Callback function executed when the receive watermark carrying bytes from
  SOC for the BT task reaches the high count.
  This sets the UART1DM inbound flow to disabled, disabling the SOC from sendng
  more data to the BT host driver.

===========================================================================*/
LOCAL void bt_dc_sio_rx_hiwater_cb
(
  void
)
{

  BT_MSG_HIGH("BT DC: SoC to MSM hi_watermark reached HiCnt=%d, LoCnt=%d, CurCnt=%d", 
             bt_soc_to_msm_sio_wm.hi_watermark,
             bt_soc_to_msm_sio_wm.lo_watermark,
             bt_soc_to_msm_sio_wm.current_cnt); 

  bt_soc_disable_flow();

}
#endif /* FEATURE_BT_SOC */


/*===========================================================================

FUNCTION
  bt_task_sio_rx_data

DESCRIPTION
  Callback function executed when the SIO driver enqueues data onto the
  receive watermark for the BT task.  This sets a BT signal to indicate
  the RX SIO watermark has bytes for the BT task.

===========================================================================*/
LOCAL void bt_task_sio_rx_data
(
  void
)
{

  rex_set_sigs( &bt_tcb, BT_SIO_RX_SIG );

}


/*===========================================================================

FUNCTION
  bt_dc_initialize

DESCRIPTION
  Initializes the Driver Configuration module.

===========================================================================*/
void bt_dc_initialize( void )
{
  uint8 bt_rdm_cnt;  /* Counter for BT RDM services */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsm_queue_init(&bt_from_sio_wm, 5000, &bt_from_sio_q);
  bt_from_sio_wm.non_empty_func_ptr  = bt_task_sio_rx_data;

  dsm_queue_init(&bt_to_sio_wm, 5000, &bt_to_sio_q);

#ifdef FEATURE_BT_SOC
  dsm_queue_init(&bt_soc_to_msm_sio_wm, BT_DC_RX_WM_MAX, &bt_soc_to_msm_sio_q);
  bt_soc_to_msm_sio_wm.lo_watermark        = BT_DC_RX_WM_LO;
  bt_soc_to_msm_sio_wm.hi_watermark        = BT_DC_RX_WM_HI;
  bt_soc_to_msm_sio_wm.lowater_func_ptr    = bt_dc_sio_rx_lowater_cb;
  bt_soc_to_msm_sio_wm.non_empty_func_ptr  = bt_task_rx_data_from_soc;
  bt_soc_to_msm_sio_wm.hiwater_func_ptr    = bt_dc_sio_rx_hiwater_cb;

  dsm_queue_init(&bt_msm_to_soc_sio_wm, 5000, &bt_msm_to_soc_sio_q);
  bt_dc.sio_state                          = BT_SIO_IDLE;
  bt_dc.driver_state                       = BT_DS_SOC_IDLE;
#endif /* FEATURE_BT_SOC */

  bt_dc_last_hci_ev_reg_app_id = BT_APP_ID_NULL;

#ifndef FEATURE_BT_SOC 
  bt_dc.driver_state            = BT_DS_IDLE;
  bt_dc.driver_substate         = BT_DSS_NONE;
  
  bt_dc.dsp_dont_unload         = FALSE;
  bt_dc.dsp_bt_image_to_request = BT_IMAGE_VOC;
  bt_dc.dsp_image_current       = BT_DSP_IMAGE_MAX;
  bt_dc.dsp_image_requested     = BT_DSP_IMAGE_MAX;
  bt_dc.dsp_image_desired       = BT_DSP_IMAGE_MAX;
#endif /* !FEATURE_BT_SOC */

  bt_dc.cur_hci_mode            = BT_HCIM_OFF;
  bt_dc.sio_stream_id           = SIO_NO_STREAM_ID;
  bt_dc.sio_port_id             = SIO_PORT_NULL;
#ifdef FEATURE_BT_SOC
  bt_dc.sio_stream_id_ext_soc   = SIO_NO_STREAM_ID;
  bt_dc.sio_port_id_ext_soc     = SIO_PORT_NULL;
  bt_dc.sio_in_flow_on_ext_soc  = FALSE;
#endif /* FEATURE_BT_SOC */

#ifdef BT_SWDEV_RADIO_OFF
  bt_dc.radio_state             = BT_RADIO_ENABLED;
#endif

#ifdef BT_SWDEV_2_1_SSP
  bt_dc.io_cap                  = BT_RM_IOC_DISPLAY_YES_NO;
  bt_dc.default_sec_level       = BT_RM_SL_1_LOW;
#endif /* BT_SWDEV_2_1_SSP */

  for( bt_rdm_cnt = 0; bt_rdm_cnt < BT_RDM_MAX_SRVC; bt_rdm_cnt++ )
  {
    bt_dc.bt_rdm_cfg[bt_rdm_cnt].bt_rdm_mode    = BT_RDM_INACTIVE;
    bt_dc.bt_rdm_cfg[bt_rdm_cnt].bt_rdm_service = BT_RDM_NULL_SRVC;
    bt_sio_port_id_from_rdm[bt_rdm_cnt]         = SIO_PORT_NULL;
  } /* for bt_rdm_cnt */

  rex_init_crit_sect( &bt_dc_crit_sect );
}


#ifdef FEATURE_RUNTIME_DEVMAP
/*===========================================================================

FUNCTION
  bt_dc_rdm_open_hci_srvc

DESCRIPTION
  Handles callback from rdm to open Sio port that the BT Driver should use 
  when in HCI mode.

===========================================================================*/
LOCAL void bt_dc_rdm_open_hci_srvc
(
  sio_port_id_type      new_sio_port_id
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bt_sio_port_id_from_rdm[BT_RDM_HCI_SRVC]         = new_sio_port_id;
  bt_dc.bt_rdm_cfg[BT_RDM_HCI_SRVC].bt_rdm_mode    = BT_RDM_OPEN;
  bt_dc.bt_rdm_cfg[BT_RDM_HCI_SRVC].bt_rdm_service = BT_RDM_HCI_SRVC;
  rex_set_sigs( &bt_tcb, BT_RDM_SIG );

}


/*===========================================================================

FUNCTION
  bt_dc_rdm_close_hci_srvc

DESCRIPTION
  Requests the BT Driver to stop using the RDM port for HCI test mode.

===========================================================================*/
void bt_dc_rdm_close_hci_srvc( void )
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bt_sio_port_id_from_rdm[BT_RDM_HCI_SRVC]         = SIO_PORT_NULL;
  bt_dc.bt_rdm_cfg[BT_RDM_HCI_SRVC].bt_rdm_mode    = BT_RDM_CLOSE;
  bt_dc.bt_rdm_cfg[BT_RDM_HCI_SRVC].bt_rdm_service = BT_RDM_HCI_SRVC;
  rex_set_sigs( &bt_tcb, BT_RDM_SIG );

} /* bt_dc_rdm_close */



#if defined( FEATURE_BT_SOC ) && defined( FEATURE_BT_WLAN_COEXISTENCE ) && \
    defined( FEATURE_WLAN )
#error code not present
#endif /* FEATURE_BT_SOC  && FEATURE_BT_WLAN_COEXISTENCE  && FEATURE_WLAN */


#if defined( FEATURE_BT_QSOC_INBAND_SLEEP ) 
/*===========================================================================

FUNCTION
  bt_dc_init_on_rdm_open_soc

DESCRIPTION
This function is called as part of processing the SIG set by RDM callback
function when RDM is ready for opening UARTDM SIO port. Initialization that
can not be performed during BT task startup is done here.

===========================================================================*/
LOCAL void bt_dc_init_on_rdm_open_soc ( void )
{

#if defined( FEATURE_BT_SOC ) && defined( FEATURE_BT_WLAN_COEXISTENCE ) && \
    defined( FEATURE_WLAN )
#error code not present
#endif /* FEATURE_BT_SOC  && FEATURE_BT_WLAN_COEXISTENCE  && FEATURE_WLAN */

} /* bt_dc_init_on_rdm_open_soc */
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */


/*===========================================================================

FUNCTION
  bt_dc_sio_open

DESCRIPTION
 Opens the sio port corresponding to the RDM service type.

===========================================================================*/
void bt_dc_sio_open
(
  bt_rdm_srvc_enum_type  bt_rdm_service              
)
{
#if defined( FEATURE_BT_SOC ) && !defined( FEATURE_BT_QSOC_INBAND_SLEEP ) \
    && !defined( FEATURE_BT_QSOC_POWERDOWN )
  sio_open_type     bt_sio_open_uart1dm;
#endif /* FEATURE_BT_SOC */
  sio_open_type     bt_sio_open;
  sio_port_id_type  new_sio_port_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  new_sio_port_id = bt_sio_port_id_from_rdm[bt_rdm_service];

  
  BT_MSG_HIGH( "BT DC: RX RDM Open SPID %x",
               new_sio_port_id, 0, 0 );

  rex_enter_crit_sect( &bt_dc_crit_sect );
  
  switch( bt_rdm_service )
  {
#ifdef FEATURE_BT_SOC
    case BT_RDM_SOC_SRVC :
#ifdef  FEATURE_BT_QSOC_INBAND_SLEEP
    /* Fix SIO wakeup issue due to "glitch" on UARTDM Rx line. UARTDM sio port */
    /* is opened or closed  depending on whether BT SOC is powered on/off */
    bt_dc_init_on_rdm_open_soc();
    BT_MSG_HIGH( "BT DC: Notified RDM. SIO to be opened after SOC power on", 0, 0, 0 );
    bt_dc.bt_rdm_cfg[BT_RDM_SOC_SRVC].bt_rdm_mode = BT_RDM_ACTIVE;
    rdm_notify( RDM_BT_SOC_SRVC, RDM_DONE_S );
    /* Intialize bt_soc_ifc to be used later */
    bt_soc_ifc_init();    

#ifndef FEATURE_BT_QSOC_POWERDOWN
    /* Initialize SoC when QSOC POWERDOWN is not defined. This is because unlike
       the case when this feature is defined, BTS will not be held in reset. So
       its imp to initialize it so the BTS can be in sleep state */
    bt_dc_soc_startup();
#endif /* !FEATURE_BT_QSOC_POWERDOWN */

    break; /* Skip rest of the code for this case statement */
#else

    /* If BTS 402x can be powered down, sio port is opened and closed 
       everytime BT is turned ON and OFF. Since this function is executed 
       when UART1 ownership is assigned to BT, sio port does not need to be 
       opened here in case QSOC_POWERDOWN is enabled, it will be opened as
       part of BT ON procedure. */
#ifndef FEATURE_BT_QSOC_POWERDOWN    
    if ( bt_dc.sio_port_id_ext_soc != new_sio_port_id )
    {
      /*  Clean up previous stream as necessary.  */
      if ( bt_dc.sio_stream_id_ext_soc != SIO_NO_STREAM_ID )
      {
        dsm_empty_queue( &bt_soc_to_msm_sio_wm );
        dsm_empty_queue( &bt_msm_to_soc_sio_wm );
        sio_close( bt_dc.sio_stream_id_ext_soc, NULL );
        bt_dc.sio_stream_id_ext_soc = SIO_NO_STREAM_ID;
      }

      bt_dc.sio_port_id_ext_soc = new_sio_port_id;

      /*  Open the new SIO port.  */
#ifdef FEATURE_BT_USES_SIO_GENERIC_MODE      
      bt_sio_open_uart1dm.stream_mode    = SIO_GENERIC_MODE;
#else
      bt_sio_open_uart1dm.stream_mode    = SIO_DS_PKT_MODE;
#endif /* FEATURE_BT_USES_SIO_GENERIC_MODE */      
      bt_sio_open_uart1dm.rx_queue       = &bt_soc_to_msm_sio_wm;
      bt_sio_open_uart1dm.tx_queue       = &bt_msm_to_soc_sio_wm;
      bt_sio_open_uart1dm.port_id        = bt_dc.sio_port_id_ext_soc;
      bt_sio_open_uart1dm.tail_char_used = FALSE;
      bt_sio_open_uart1dm.tail_char      = 0xFF;
      bt_sio_open_uart1dm.rx_func_ptr    = NULL;
#ifdef FEATURE_BT_SOC1_ROM
#error code not present
#elif defined(FEATURE_BT_SOC1_FLASH)
#error code not present
#elif defined(FEATURE_BT_QSOC)
      /* For BTS, start with BT_SOC_SIO_INIT_BITRATE */
      bt_sio_open_uart1dm.rx_bitrate     = BT_SOC_SIO_INIT_BITRATE;
      bt_sio_open_uart1dm.tx_bitrate     = BT_SOC_SIO_INIT_BITRATE;
#else
      /* HCI mode - use BT_SOC_SIO_BITRATE */
      bt_sio_open_uart1dm.rx_bitrate     = BT_SOC_SIO_BITRATE;
      bt_sio_open_uart1dm.tx_bitrate     = BT_SOC_SIO_BITRATE;
#endif /* FEATURE_BT_SOC1_ROM */

      bt_sio_open_uart1dm.tx_flow        = SIO_CTSRFR_AUTO_FCTL;

      bt_sio_open_uart1dm.rx_flow        = SIO_CTSRFR_AUTO_FCTL;

      bt_dc.sio_stream_id_ext_soc = sio_open( &bt_sio_open_uart1dm );

      if ( (bt_dc.sio_stream_id_ext_soc) !=
           SIO_NO_STREAM_ID )
      {
        if ( bt_sio_open_uart1dm.rx_flow == SIO_CTSRFR_FCTL ) 
        {
          BT_MSG_HIGH(" BT DC: Enabling Inbound Flow to SoC", 0, 0, 0);
          bt_soc_enable_flow();
        }
        bt_dc.sio_in_flow_on_ext_soc = TRUE;
#endif /* #ifndef FEATURE_BT_QSOC_POWERDOWN */

        bt_dc.bt_rdm_cfg[BT_RDM_SOC_SRVC].bt_rdm_mode = BT_RDM_ACTIVE;
        rdm_notify( RDM_BT_SOC_SRVC, RDM_DONE_S );

        /* Intialize bt_soc_ifc to be used later */
        bt_soc_ifc_init();

#ifndef FEATURE_BT_QSOC_POWERDOWN
        /* Please note bt_soc_ifc_init MUST be called before bt_dc_soc_startup */
        bt_dc_soc_startup();
      }
      else
      {
        BT_ERR( "BT DC: SIO Open SPID %x failed.",
                new_sio_port_id, 0, 0 );

        bt_dc.bt_rdm_cfg[BT_RDM_SOC_SRVC].bt_rdm_mode = BT_RDM_INACTIVE;
        /*  No RDM failure status available... use busy.  */
        rdm_notify( RDM_BT_SOC_SRVC, RDM_APP_BUSY_S );
      }
#endif /* #ifndef FEATURE_BT_QSOC_POWERDOWN */

#if defined( FEATURE_BT_SOC ) && defined( FEATURE_BT_WLAN_COEXISTENCE ) && \
    defined( FEATURE_WLAN )
#error code not present
#endif /* FEATURE_BT_SOC && FEATURE_BT_WLAN_COEXISTENCE  && FEATURE_WLAN  */

#ifndef FEATURE_BT_QSOC_POWERDOWN
    }
    else
    {
      /*  SIO port did not change.  */

      rdm_notify( RDM_BT_SOC_SRVC, RDM_DONE_S );
    }
#endif /* #ifndef FEATURE_BT_QSOC_POWERDOWN */
    break; 
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */
#endif /* FEATURE_BT_SOC */

    case BT_RDM_HCI_SRVC :
    if ( bt_dc.sio_port_id != new_sio_port_id )
    {
      /*  Clean up previous stream as necessary.  */
      if ( bt_dc.sio_stream_id != SIO_NO_STREAM_ID )
      {
        dsm_empty_queue( &bt_from_sio_wm );
        dsm_empty_queue( &bt_to_sio_wm );
        sio_close( bt_dc.sio_stream_id, NULL );
        bt_dc.sio_stream_id = SIO_NO_STREAM_ID;
      }

      bt_dc.sio_port_id = new_sio_port_id;

      /*  Open the new SIO port.  */
      bt_sio_open.stream_mode    = SIO_GENERIC_MODE;
      bt_sio_open.rx_queue       = &bt_from_sio_wm;
      bt_sio_open.tx_queue       = &bt_to_sio_wm;
      bt_sio_open.port_id        = bt_dc.sio_port_id;
      bt_sio_open.tail_char_used = FALSE;
      bt_sio_open.tail_char      = 0xFF;
      bt_sio_open.rx_func_ptr    = NULL;
      bt_sio_open.rx_bitrate     = SIO_BITRATE_115200;
      bt_sio_open.tx_bitrate     = SIO_BITRATE_115200;
      bt_sio_open.rx_flow        = SIO_FCTL_OFF;
      bt_sio_open.tx_flow        = SIO_FCTL_OFF;

      bt_dc.sio_stream_id = sio_open( &bt_sio_open );

      if ( (bt_dc.sio_stream_id) !=
              SIO_NO_STREAM_ID )
      {
        if ( bt_sio_open.rx_flow == SIO_CTSRFR_FCTL )
        {
          sio_ioctl( bt_dc.sio_stream_id,
                     SIO_IOCTL_INBOUND_FLOW_ENABLE, NULL );
        }

        bt_dc.bt_rdm_cfg[BT_RDM_HCI_SRVC].bt_rdm_mode = BT_RDM_ACTIVE;
        rdm_notify( RDM_BT_HCI_SRVC, RDM_DONE_S );

        /* HCI port mapping successful, set the driver HCI mode now */
        bt_cmd_dc_set_hci_mode( bt_efs_params.mode_to_start_in );
      }
      else
      {
        BT_ERR( "BT DC: SIO Open SPID %x failed.",
                new_sio_port_id, 0, 0 );
        
        bt_dc.bt_rdm_cfg[BT_RDM_HCI_SRVC].bt_rdm_mode = BT_RDM_INACTIVE;
        /*  No RDM failure status available... use busy.  */
        rdm_notify( RDM_BT_HCI_SRVC, RDM_APP_BUSY_S );
      }
    }
    else
    {
      /*  SIO port did not change.  */

      rdm_notify( RDM_BT_HCI_SRVC, RDM_DONE_S );
    }
    break;

  case BT_RDM_NULL_SRVC :
  case BT_RDM_MAX_SRVC  :
  default: 
    BT_ERR( "BT DC: Bad RDM Open PID %x SVC %x", 
            new_sio_port_id, 
            bt_rdm_service, 0 );
    rdm_notify( RDM_NULL_SRVC, RDM_NOT_ALLOWED_S );
    break;

  } /* switch*/
  
  rex_leave_crit_sect( &bt_dc_crit_sect );

} /* bt_dc_sio_open */

#ifdef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_dc_soc_sio_open

DESCRIPTION
 Opens the UARTDM for SOC service. When this function is called RDM cb
 must have executed and all the necessary BT initialization completed.

===========================================================================*/
LOCAL bt_cmd_status_type bt_dc_soc_sio_open ( void )
{
  sio_open_type      bt_sio_open_uart1dm;
  sio_port_id_type   rdm_sio_port_id;
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rdm_sio_port_id = bt_sio_port_id_from_rdm[BT_RDM_SOC_SRVC];

  
  BT_MSG_HIGH( "BT DC: Opening SOC SIO  SPID %x", rdm_sio_port_id, 0, 0 );

  rex_enter_crit_sect( &bt_dc_crit_sect );
    
  if ( rdm_sio_port_id != SIO_PORT_NULL )
  {
    /* Check SIO has been closed after a previous open */
    if ( ( bt_dc.sio_port_id_ext_soc == SIO_PORT_NULL ) && 
         ( bt_dc.sio_stream_id_ext_soc == SIO_NO_STREAM_ID ) )
    {
      /*  Open the new SIO port.  */
#ifdef FEATURE_BT_USES_SIO_GENERIC_MODE
      bt_sio_open_uart1dm.stream_mode    = SIO_GENERIC_MODE;      
#else
      bt_sio_open_uart1dm.stream_mode    = SIO_DS_PKT_MODE;
#endif /* FEATURE_BT_USES_SIO_GENERIC_MODE */     

      bt_sio_open_uart1dm.rx_queue       = &bt_soc_to_msm_sio_wm;
      bt_sio_open_uart1dm.tx_queue       = &bt_msm_to_soc_sio_wm;
      bt_sio_open_uart1dm.port_id        = rdm_sio_port_id;
      bt_sio_open_uart1dm.tail_char_used = FALSE;
      bt_sio_open_uart1dm.tail_char      = 0xFF;
      bt_sio_open_uart1dm.rx_func_ptr    = NULL;
#ifdef FEATURE_BT_SOC1_ROM
#error code not present
#elif defined(FEATURE_BT_SOC1_FLASH)
#error code not present
#elif defined(FEATURE_BT_QSOC)
      if ( bt_dc_sio_uart1dm_opened_once == FALSE )
      {
        /* For BTS, start with BT_SOC_SIO_INIT_BITRATE */      
        bt_sio_open_uart1dm.rx_bitrate     = BT_SOC_SIO_INIT_BITRATE;
        bt_sio_open_uart1dm.tx_bitrate     = BT_SOC_SIO_INIT_BITRATE;
#ifndef FEATURE_BT_QSOC_POWERDOWN
        /* This is the only time when SOC and MSM will start with the initial
           lower baud rate. Set a Flag to indicate that SOC will be Configured to
           work only at higher baud  rate now onwards */
        bt_dc_sio_uart1dm_opened_once      = TRUE;
#endif /* FEATURE_BT_QSOC_POWERDOWN */        
      }
      else
      {
        bt_sio_open_uart1dm.rx_bitrate     = BT_SOC_SIO_BITRATE;
        bt_sio_open_uart1dm.tx_bitrate     = BT_SOC_SIO_BITRATE;                
      }
#else
      /* HCI mode - use BT_SOC_SIO_BITRATE */
      bt_sio_open_uart1dm.rx_bitrate     = BT_SOC_SIO_BITRATE;
      bt_sio_open_uart1dm.tx_bitrate     = BT_SOC_SIO_BITRATE;      
#endif /* FEATURE_BT_SOC1_ROM */

      bt_sio_open_uart1dm.tx_flow        = SIO_CTSRFR_AUTO_FCTL;

      bt_sio_open_uart1dm.rx_flow        = SIO_CTSRFR_AUTO_FCTL;

      bt_dc.sio_stream_id_ext_soc = sio_open( &bt_sio_open_uart1dm );

      if ( (bt_dc.sio_stream_id_ext_soc) !=
           SIO_NO_STREAM_ID )
      {
        bt_dc.sio_port_id_ext_soc = bt_sio_port_id_from_rdm[BT_RDM_SOC_SRVC];
        if ( bt_sio_open_uart1dm.rx_flow == SIO_CTSRFR_FCTL ) 
        {
          BT_MSG_HIGH(" BT DC: Enabling Inbound Flow to SoC", 0, 0, 0);
          bt_soc_enable_flow();
        }
        bt_dc.sio_in_flow_on_ext_soc = TRUE;


      }
      else
      {
        BT_ERR( "BT DC: SIO Open SPID %x failed.", rdm_sio_port_id, 0, 0 );
        status = BT_CS_DC_SIO_OPEN_FAILED;
      }
    }
    else
    {
        BT_ERR( "BT DC: SIO not closed P %x S %x", bt_dc.sio_port_id_ext_soc, 
                 bt_dc.sio_stream_id_ext_soc, 0 );
        status = BT_CS_DC_SIO_PORT_ALREADY_OPENED;

    }
  }
  else
  {
    /* RDM cb for open has not executed yet */
    status = BT_CS_DC_NO_SIO_PORT_ID;
  }

  rex_leave_crit_sect( &bt_dc_crit_sect );
  return ( status );
} /* bt_dc_soc_sio_open */


/*===========================================================================

FUNCTION
  bt_dc_sio_ioctl

DESCRIPTION
  Changes the the sio port control settings.

===========================================================================*/
void bt_dc_sio_ioctl
(
  sio_stream_id_type    stream_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type  *param
)
{

  if ( stream_id != SIO_NO_STREAM_ID )
  {
    // Ensure SIO is up before sending any IOCTL cmds.
    // The only command that can be sent while SIO is sleeping
    // is the one that can wake it up!

#ifdef FEATURE_SIO_CLIENT_SLEEP_CONTROL
    if ( ( bt_dc.sio_state == BT_SIO_SLEEPING ) &&
         ( cmd != SIO_IOCTL_WAKEUP ))
    { 
#ifdef BT_SLEEP_DEBUG
      BT_MSG_HIGH( "BT HC DRV: Calling update_sio_state - ioctl activity: %d 1%  %d",cmd,param,0);
#endif
      bt_dc_update_sio_state( BT_SIO_AWAKE, TRUE );
    }
    sio_ioctl( stream_id,
               cmd,
               param
             );
#endif /* FEATURE_SIO_CLIENT_SLEEP_CONTROL */
  }
  else
  {
    BT_ERR( "BT DC: sio_ioctl: Bad stream id c %x", cmd, 0, 0 );  
  }

} /* bt_dc_sio_ioctl */


/*===========================================================================

FUNCTION
  bt_dc_sio_baud_change

DESCRIPTION
  Changes the the sio port baud rate.

===========================================================================*/
void bt_dc_sio_change_baud
(
  sio_bitrate_type    bit_rate,
  sio_ioctl_cmd_type  cmd_type
)
{
  sio_ioctl_param_type  ioctl_param;

  ioctl_param.bitrate = bit_rate;

  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc, cmd_type, &ioctl_param );
} /* bt_dc_sio_baud_change */


/*===========================================================================

FUNCTION
  bt_dc_rdm_open_soc_srvc

DESCRIPTION
  Handles callback from rdm to open Sio port that the BT Driver should use 
  to communicate with HostController SoC.

===========================================================================*/
LOCAL void bt_dc_rdm_open_soc_srvc
(
  sio_port_id_type  new_sio_port_id
)
{

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  bt_sio_port_id_from_rdm[BT_RDM_SOC_SRVC]           = new_sio_port_id;
  bt_dc.bt_rdm_cfg[BT_RDM_SOC_SRVC].bt_rdm_mode      = BT_RDM_OPEN;
  bt_dc.bt_rdm_cfg[BT_RDM_SOC_SRVC].bt_rdm_service   = BT_RDM_SOC_SRVC;
  rex_set_sigs( &bt_tcb, BT_RDM_SIG );

} /* bt_dc_rdm_open_soc_srvc */


/*===========================================================================

FUNCTION
  bt_dc_rdm_close_soc_srvc

DESCRIPTION
  Requests the BT Driver to stop using the RDM port for SoC.

===========================================================================*/
void bt_dc_rdm_close_soc_srvc( void )
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bt_sio_port_id_from_rdm[BT_RDM_SOC_SRVC]           = SIO_PORT_NULL;
  bt_dc.bt_rdm_cfg[BT_RDM_SOC_SRVC].bt_rdm_mode      = BT_RDM_CLOSE;
  bt_dc.bt_rdm_cfg[BT_RDM_SOC_SRVC].bt_rdm_service   = BT_RDM_SOC_SRVC;
  rex_set_sigs( &bt_tcb, BT_RDM_SIG );

} /* bt_dc_rdm_close_soc_srvc */

#endif /* FEATURE_BT_SOC */


#if defined( FEATURE_BT_QSOC_INBAND_SLEEP) || defined( FEATURE_BT_QSOC_POWERDOWN )
/*===========================================================================

FUNCTION
  bt_dc_soc_sio_close

DESCRIPTION
 Opens the sio port corresponding to the RDM service type.

===========================================================================*/
LOCAL void bt_dc_soc_sio_close ( void )
{

  BT_MSG_HIGH( "BT DC: Closing SOC SIO SID %x HCIM %x SPID %x",
                bt_dc.sio_stream_id_ext_soc, bt_dc.cur_hci_mode,
                bt_dc.sio_port_id_ext_soc );

  if ( ( bt_dc.sio_port_id_ext_soc == SIO_PORT_NULL ) ||
       ( bt_dc.sio_stream_id_ext_soc == SIO_NO_STREAM_ID ) )
  {
     BT_ERR( "BT DC: Invalid SIO port/stream", 0, 0, 0 );
  }
  else 
  {
    /*  Clean up stream and close SIO. */
    rex_enter_crit_sect( &bt_dc_crit_sect );
    dsm_empty_queue( &bt_soc_to_msm_sio_wm );
    dsm_empty_queue( &bt_msm_to_soc_sio_wm );
    sio_close( bt_dc.sio_stream_id_ext_soc, NULL );
    /* Change sio state back to idle. For current targets, this is a safe
       change. It is however needed for targets using out of band sleep
       and enabling QSOC_POWERDOWN feature to ensure correct initialization
       during power on (that followed a powered off) */
    bt_dc_update_sio_state( BT_SIO_IDLE, FALSE );	
    bt_dc.sio_stream_id_ext_soc = SIO_NO_STREAM_ID;
    bt_dc.sio_port_id_ext_soc = SIO_PORT_NULL;
    rex_leave_crit_sect( &bt_dc_crit_sect );
  }
} /* bt_dc_soc_sio_close */
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP || FEATURE_BT_QSOC_POWERDOWN */


/*===========================================================================

FUNCTION
  bt_dc_sio_close

DESCRIPTION
  Causes the BT Driver to stop using the sio port associated with the
  specified port service.

===========================================================================*/
void bt_dc_sio_close
(
  bt_rdm_srvc_enum_type  bt_rdm_service
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rex_enter_crit_sect( &bt_dc_crit_sect );

 switch( bt_rdm_service )
 {
#ifdef FEATURE_BT_SOC
   case BT_RDM_SOC_SRVC :

     BT_MSG_HIGH( "BT DC: RX RDM Close SID %x HCIM %x SPID %x",
                 bt_dc.sio_stream_id_ext_soc, bt_dc.cur_hci_mode,
                 bt_dc.sio_port_id_ext_soc );

     /*  Clean up stream and close via SIO as necessary.  */
     if ( bt_dc.sio_stream_id_ext_soc != SIO_NO_STREAM_ID )
     {
       dsm_empty_queue( &bt_soc_to_msm_sio_wm );
       dsm_empty_queue( &bt_msm_to_soc_sio_wm );
       sio_close( bt_dc.sio_stream_id_ext_soc, NULL );
       bt_dc.sio_stream_id_ext_soc = SIO_NO_STREAM_ID;
     }
     
     bt_dc.sio_port_id_ext_soc = SIO_PORT_NULL;

     bt_dc.bt_rdm_cfg[BT_RDM_SOC_SRVC].bt_rdm_mode = BT_RDM_INACTIVE;
     rdm_notify( RDM_BT_SOC_SRVC, RDM_DONE_S );

     break;
#endif /* FEATURE_BT_SOC */

   case BT_RDM_HCI_SRVC :

     BT_MSG_HIGH( "BT DC: RX RDM Close SID %x HCIM %x SPID %x",
                  bt_dc.sio_stream_id, bt_dc.cur_hci_mode,
                  bt_dc.sio_port_id );

     /*  Clean up stream and close via SIO as necessary.  */
     if ( bt_dc.sio_stream_id != SIO_NO_STREAM_ID )
     {
       dsm_empty_queue( &bt_from_sio_wm );
       dsm_empty_queue( &bt_to_sio_wm );
       sio_close( bt_dc.sio_stream_id, NULL );
       bt_dc.sio_stream_id = SIO_NO_STREAM_ID;
     }
     
     bt_dc.sio_port_id = SIO_PORT_NULL;

     bt_dc.bt_rdm_cfg[BT_RDM_HCI_SRVC].bt_rdm_mode = BT_RDM_INACTIVE;
     rdm_notify( RDM_BT_HCI_SRVC, RDM_DONE_S );

     break;

   case BT_RDM_NULL_SRVC :
   case BT_RDM_MAX_SRVC  :
   default: 
     BT_ERR( "BT DC: Bad RDM Close Svc %x", bt_rdm_service, 0, 0 );
     rdm_notify( RDM_NULL_SRVC, RDM_NOT_ALLOWED_S );
     break;

 } /* switch(bt_rdm_service) */

 rex_leave_crit_sect( &bt_dc_crit_sect );

} /* bt_dc_sio_close */


/*===========================================================================

FUNCTION
  bt_dc_process_rdm_change

DESCRIPTION
  Processes change in RDM mode and opens/closes the suitable sio port

===========================================================================*/
void bt_dc_process_rdm_change ( void )
{
  /* Counter for BT RDM services */
  uint8 bt_srvc_cnt;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the RDM command for all BT services that have received one */
  for( bt_srvc_cnt = 0; bt_srvc_cnt < BT_RDM_MAX_SRVC; bt_srvc_cnt++ )
  {
    switch ( bt_dc.bt_rdm_cfg[bt_srvc_cnt].bt_rdm_mode )
    {
      case BT_RDM_OPEN :
      {
        bt_dc_sio_open( bt_dc.bt_rdm_cfg[bt_srvc_cnt].bt_rdm_service );
        break;
      }
      case BT_RDM_CLOSE :
      {
        bt_dc_sio_close( bt_dc.bt_rdm_cfg[bt_srvc_cnt].bt_rdm_service );
        break;
      }
      case BT_RDM_ACTIVE :
      {
        MSG_HIGH( "BT DC: BT RDM active mode svc=%d, mode=%d", 
                  bt_dc.bt_rdm_cfg[bt_srvc_cnt].bt_rdm_service,
                  bt_dc.bt_rdm_cfg[bt_srvc_cnt].bt_rdm_mode, 0 );
      }
      break;
      default          :
      case BT_RDM_INACTIVE :
      {
        MSG_HIGH( "BT DC: BT RDM inactive mode svc=%d, mode=%d", 
                  bt_dc.bt_rdm_cfg[bt_srvc_cnt].bt_rdm_service,
                  bt_dc.bt_rdm_cfg[bt_srvc_cnt].bt_rdm_mode, 0 );
      }
      break;

    } /* switch */
  } /* for */

} /* bt_dc_process_sio_change */

#endif /* FEATURE_RUNTIME_DEVMAP */


/*===========================================================================

FUNCTION
  bt_dc_register_with_external_services

DESCRIPTION
  Registers with external services.

===========================================================================*/
void bt_dc_reg_with_external_services( void )
{

#ifdef FEATURE_RUNTIME_DEVMAP
 
#ifdef FEATURE_BT_SOC
#ifdef BT_SWDEV_BT_DISABLED_NV
  if ( bt_dc.is_bt_disabled == FALSE )
  {
#endif /* BT_SWDEV_BT_DISABLED_NV */
  rdm_register_open_func( RDM_BT_SOC_SRVC, bt_dc_rdm_open_soc_srvc );
  rdm_register_close_func( RDM_BT_SOC_SRVC, bt_dc_rdm_close_soc_srvc );
#ifdef BT_SWDEV_BT_DISABLED_NV
  } 
#endif /* BT_SWDEV_BT_DISABLED_NV */
#endif /* FEATURE_BT_SOC */

  rdm_register_open_func( RDM_BT_HCI_SRVC, bt_dc_rdm_open_hci_srvc );
  rdm_register_close_func( RDM_BT_HCI_SRVC, bt_dc_rdm_close_hci_srvc );
 
#else
  bt_dc.sio_port_id = sdm_get_port_id( SDM_APP_BT );
#endif

#ifndef FEATURE_BT_SOC 
#if defined( T_MSM6100 ) || defined( T_MSM6200 )

  {
#ifndef T_MSM6275
    static mdsp_isr_func_type bt_mdsp_isr_table[ MDSP_ISR_MAX ] =
    {
#ifndef T_MSM6800
      NULL,
      NULL,
#ifdef T_MSM6200 
#error code not present
#endif /* T_MSM6200 */
#endif /* not T_MSM6800 */
      bt_q2a_isr
    };
#endif

    mdsp_register_app( MDSP_APP_BT,
#ifdef FEATURE_WCDMA_MDSP_INTERFACE_REV_6275
#error code not present
#else
                       bt_mdsp_isr_table,
#endif /* FEATURE_WCDMA_MDSP_INTERFACE_REV_6275 */
                       bt_dc_process_mdsp_services_event,
                       MDSP_EV_COMMAND | MDSP_EV_MODULE 
#ifndef FEATURE_MDSP_USES_MODULES
                       | MDSP_EV_IMAGE 
#endif /* FEATURE_MDSP_USES_MODULES */
                       ); 

#ifdef FEATURE_WCDMA_MDSP_INTERFACE_REV_6275
#error code not present
#endif /* FEATURE_WCDMA_MDSP_INTERFACE_REV_6275 */
  }

#else
#error code not present
#endif /* T_MSM6100 or T_MSM6200 */
#endif /* FEATURE_BT_SOC */

}

#ifdef FEATURE_BT_DYNAMIC_TLMM
#error code not present
#endif  /* FEATURE_BT_DYNAMIC_TLMM */


/*===========================================================================

FUNCTION
  bt_dc_gpio_bt_active_config

DESCRIPTION
  Configure BT gpios appropriately for BT active mode.

===========================================================================*/

void bt_dc_gpio_bt_active_config
(
  void
)
{
#if (defined(T_MSM6246)) || (defined(T_MSM6290))
  gpio_tlmm_config_group(GPIO_GROUP_BT_ACTIVE);
#endif /* T_MSM6246 || T_MSM6290 */

#ifdef FEATURE_BT_DYNAMIC_TLMM
#error code not present
#endif /* FEATURE_BT_DYNAMIC_TLMM */

}


/*===========================================================================

FUNCTION
  bt_dc_gpio_bt_pwroff_config

DESCRIPTION
  Configure BT gpios appropriately for power-off mode. If may be required to
  configure some gpios in a non-native but known good state to prevent 
  leagake.

===========================================================================*/

void bt_dc_gpio_bt_pwroff_config
(
  void
)
{
#if (defined(T_MSM6246)) || (defined(T_MSM6290))
  /* At this time TX line should already be in the break state */
  bt_soc_enable_flow();

  /* BT_PWROFF group does not contain AUX_PCM GPIOs. */
  gpio_tlmm_config_group(GPIO_GROUP_BT_PWROFF);

  /* After SCO connection terminates, AUX PCM IN is floating. If this is
     high, it may cause leakage thus draining battery. Configure it to
     no leakage state
   */
  gpio_tlmm_config(GPIO_OUTP_14);
  gpio_out(GPIO_OUTP_14, GPIO_LOW_VALUE);

#endif /* T_MSM6246 || T_MSM6290 */

#ifdef FEATURE_BT_DYNAMIC_TLMM
#error code not present
#endif /* FEATURE_BT_DYNAMIC_TLMM */

}


/*===========================================================================

FUNCTION
  bt_dc_gpio_bt_post_pwroff_config

DESCRIPTION
  Configure BT gpios appropriately after powering-off BTS. Reconfigure gpios
  is they were force-configured in some known state, different from native
  state (to prevent leakage).

===========================================================================*/

void bt_dc_gpio_bt_post_pwroff_config
(
  void
)
{
#if (defined(T_MSM6246)) || (defined(T_MSM6290))
  gpio_tlmm_config(AUX_PCM_DIN);
#endif /* T_MSM6246 || T_MSM6290 */

#ifdef FEATURE_BT_DYNAMIC_TLMM
#error code not present
#endif /* FEATURE_BT_DYNAMIC_TLMM */

}


#ifdef FEATURE_BT_SOC 

#if (defined( FEATURE_BT_QSOC_INBAND_SLEEP ) || \
     defined( FEATURE_BT_QSOC_POWERDOWN ))
/*===========================================================================

FUNCTION
  bt_dc_enable_driver

DESCRIPTION
  This routine enables the driver when functionality below HCI is needed.
  It takes care of Turning ON the Vreg, Enablig the SIO Port and Bringing up the
  SOC.

===========================================================================*/
void bt_dc_enable_driver
(
  void
)
{

  /* These will be NULL if bt_soc_ifc_init() has not been called yet,
     (called from bt_dc_sio_open) */
  if (bt_soc_ifc.bt_soc_powerup_func_ptr == NULL ||
      bt_soc_ifc.bt_soc_init_func_ptr == NULL )
  {
    /* Exit function without doing anything (don't change driver state) */
   BT_ERR( "BT DC: SOC  ptrs not yet assigned !!",
            0,0,0);      

    /* Intialize bt_soc_ifc to be used later 
    bt_soc_ifc_init(); */
            
    return;
  }  

  /* Vote against VDD minimization */
  bt_set_vdd_min_not_allowed();
  
  /* BT active state GPIO config, required only when TLMM does not provide 
     these config already 
   */
  bt_dc_gpio_bt_active_config();
  
  BT_MSG_HIGH( "BT DC: Enable Driver, Open SIO DS %x", bt_dc.driver_state, 
                0, 0);
  /* Power cycle the SOC is taken care of of bt_qsoc_powerup() irrespective of current state
  
   * The bit rate need't be taken set to init bit rate as we do sio_open\sio_close each time
   
   * HCI RST for Test mode will result in sio_open\close() taking place each time.
  */
 
   if ( bt_dc_soc_sio_open() == BT_CS_GN_SUCCESS )
   {

    /* Please note bt_soc_ifc_init MUST be called before bt_dc_power_on_soc */

    /* Power on SoC */
    bt_dc_power_on_soc();

    /* bt_dc_power_on_soc()/bt_qsoc_powerup()  takes care of calling 
       bt_dc_update_sio_state( BT_SIO_AWAKE,  ?? */
       
#ifdef FEATURE_BT_QSOC
    /* Now committed to bring QSOC up so change driver state to ENABLING.
       Need to do this after power on since its logic needs to know when
       driver state is IDLE. */
    BT_DC_UPDATE_DRIVER_STATE(BT_DS_SOC_ENABLING);
#endif /* FEATURE_BT_QSOC */


    /* Bringup SOC */
    bt_hc_drv_soc_init();
    }
    else
    {
       BT_ERR( "BT DC: SIO Port open failed !!",
                0,0,0);        
    }
    
} /* bt_dc_enable_driver */

/*===========================================================================

FUNCTION
  bt_dc_disable_driver

DESCRIPTION
  This routine disables the driver once insufficient activity is detected.
  It will have SIO vote to sleep, shutdown the SOC and then power off the
  Vreg to the SOC.

===========================================================================*/
void bt_dc_disable_driver
(
  void
)
{
    BT_MSG_HIGH( "BT DC: Disable Driver, Close SIO SS %x DS %x ", 
                  bt_dc.sio_state, bt_dc.driver_state, 0 );

    //shutdown SIO
    if ( bt_dc.sio_state != BT_SIO_SLEEPING )
    {
      bt_dc_update_sio_state( BT_SIO_SLEEPING, TRUE );
    }

    if ( bt_debug_enable_soc_shutdown )
    {
      /* BT power off state GPIO config, required when BTS reset is not 
         connected to a GPIO on MSM. 
       */
      bt_dc_gpio_bt_pwroff_config();

      /* Shutdown SoC */
      bt_hc_drv_soc_shutdown();

      /* Re-configure any GPIOs after SoC poweroff, if required */
      bt_dc_gpio_bt_post_pwroff_config();
    }

    /* Close SIO port */
    bt_dc_soc_sio_close();

    BT_DC_UPDATE_DRIVER_STATE(BT_DS_SOC_DISABLED);

    BT_MSG_HIGH( "BT DC: Driver Disabled ", 0,0,0 );

    /* Vote for VDD minimization */
    bt_set_vdd_min_allowed();

} /* bt_dc_disable_driver */

#else
/*===========================================================================

FUNCTION
  bt_dc_enable_driver

DESCRIPTION
  This routine enables the driver once functionality below HCI is needed.
  It takes care of turning ON the Vreg, waking up SIO and bringing up the
  SOC. SIO must be open before this routine is called. This routine will
  log a BT error and return if bt_dc.sio_stream_id_ext_soc is invalid.

===========================================================================*/
void bt_dc_enable_driver
(
  void
)
{
  if ( bt_dc.sio_stream_id_ext_soc == SIO_NO_STREAM_ID )
  {
    /* Log error and bail out */
    BT_ERR( "BT DC: Attempt to enable HC drv with invalid sio stream id !!",
             0,0,0);
    return;
  }

  /* Vote against VDD minimization */
  bt_set_vdd_min_not_allowed();
  
  /* BT active state GPIO config, required only when TLMM does not provide 
     these config already 
   */
  bt_dc_gpio_bt_active_config();
  
/* For QSOC, driver state is not set here. */
#ifndef FEATURE_BT_QSOC
  BT_DC_UPDATE_DRIVER_STATE(BT_DS_SOC_ENABLING);
#endif /* FEATURE_BT_QSOC */

  //wakeup SIO
  if ( bt_dc.sio_state == BT_SIO_SLEEPING ) 
  {
    bt_dc_update_sio_state( BT_SIO_AWAKE, TRUE );
  }
  else
  {
    bt_dc_update_sio_state( BT_SIO_AWAKE, FALSE );
  }

  if ( bt_debug_enable_soc_shutdown ) 
  {
#ifdef FEATURE_BT_SOC1_ROM
#error code not present
#elif defined(FEATURE_BT_SOC1_FLASH)
#error code not present
#else
#if defined(FEATURE_BT_QSOC) 

    /* These will be NULL if bt_soc_ifc_init() has not been called yet, 
       (called from bt_dc_sio_open) */
    if (bt_soc_ifc.bt_soc_powerup_func_ptr == NULL ||
        bt_soc_ifc.bt_soc_init_func_ptr == NULL )
    {
      /* Exit function without doing anything (don't change driver state) */
      return;
    }

#ifdef FEATURE_BT_QSOC_POWERDOWN
    /* If QSOC Powerdown is supported, then baud rate must be reset to initial rate (115200). */
    /* We don't do change baud rate during HCI RST for Test mode */
    if (bt_hc_pe_is_hci_rst_started() == FALSE )
    {
      /* change SIO baudrate */
      BT_MSG_HIGH( "BT DC: Change SIO baudrate to 115200",0,0,0);
      /* Powerdown is enabled, so change to initial baud rate. */
      bt_dc_sio_change_baud( BT_SOC_SIO_INIT_BITRATE, 
                             SIO_IOCTL_CHANGE_BAUD_NOW );
    }
#else /* FEATURE_BT_QSOC_POWERDOWN */
    /* If QSOC Powerdown is not supported, then the baud rate should be at 115K only the first time
       which is set when the SIO port is opened.  Therefore, no change is necessary here.
       After that, the QSOC will not be turned off so the baud rate should not be changed. */
#endif /* FEATURE_BT_QSOC_POWERDOWN */


#endif /* defined(FEATURE_BT_QSOC) */

    //Figure out what other SOCs need
#endif /* FEATURE_BT_SOC1_ROM */

    /* Power on SoC */
    bt_dc_power_on_soc();

#ifdef FEATURE_BT_QSOC
    /* Now committed to bring QSOC up so change driver state to ENABLING. 
       Need to do this after power on since its logic needs to know when
       driver state is IDLE. */
    BT_DC_UPDATE_DRIVER_STATE(BT_DS_SOC_ENABLING);
#endif /* FEATURE_BT_QSOC */

    /* Bringup SOC */
    bt_hc_drv_soc_init();

  }

} /* bt_dc_enable_driver */

/*===========================================================================

FUNCTION
  bt_dc_disable_driver

DESCRIPTION
  This routine disables the driver once insufficient activity is detected.
  It will have SIO vote to sleep, shutdown the SOC and then power off the
  Vreg to the SOC.

===========================================================================*/
void bt_dc_disable_driver
(
  void
)
{
    //shutdown SIO
    if ( bt_dc.sio_state != BT_SIO_SLEEPING ) 
    {
      bt_dc_update_sio_state( BT_SIO_SLEEPING, TRUE );
    }


    if ( bt_debug_enable_soc_shutdown ) 
    {
      bt_dc_gpio_bt_pwroff_config();
      /* Shutdown SoC */
      bt_hc_drv_soc_shutdown();

      bt_dc_gpio_bt_post_pwroff_config();
    }

    BT_DC_UPDATE_DRIVER_STATE(BT_DS_SOC_DISABLED);

    BT_MSG_HIGH( "BT DC: Driver Disabled ", 0,0,0 );

    /* Vote for VDD minimization */
    bt_set_vdd_min_allowed();

} /* bt_dc_disable_driver */

#endif /* FEATURE_BT_QSOC_INBAND_SLEEP || FEATURE_BT_QSOC_POWERDOWN */

/*===========================================================================

FUNCTION
  bt_dc_vote_sio_sleep

DESCRIPTION
  When this function is called the following has already happened:
  - SOC sent YMS
  - uart TX has been flushed
  Now vote for SIO to sleep

===========================================================================*/
void bt_dc_vote_sio_sleep
(
  void
)
{
    if ( bt_dc.sio_state != BT_SIO_SLEEPING )
    { 
#ifdef BT_SLEEP_DEBUG
      BT_MSG_HIGH( "BT DC: Calling update_sio_state - yms rx",0,0,0);
#endif
      bt_dc_update_sio_state( BT_SIO_SLEEPING, TRUE );
    }
}

/*===========================================================================

FUNCTION
  bt_dc_update_sio_state

DESCRIPTION
  The SIO state is changed based on UART activity and directives from SOC.
  This function keeps track of SIO state and sends the appropriate ioctl
  command to wakeup or put SIO to sleep.

===========================================================================*/
void bt_dc_update_sio_state
(
  bt_sio_state_enum_type new_sio_state,
  boolean                change_sio_vote
)
{
  if ( bt_dc.sio_stream_id_ext_soc == SIO_NO_STREAM_ID )
  {
    /* Log error and bail out */
    BT_ERR( "BT DC: Attempt to call sio_ioctl with invalid sio stream id !!",
             0,0,0);
    return;
  }

  switch (new_sio_state) 
  {
    case BT_SIO_SLEEPING:
      /* Put SIO to sleep */
      if ((( bt_dc.sio_state == BT_SIO_AWAKE ) ||
           ( bt_dc.sio_state == BT_SIO_IDLE )) &&
          change_sio_vote && 
          bt_debug_enable_sio_sleep_vote )
      { 
#ifdef FEATURE_SIO_CLIENT_SLEEP_CONTROL
          sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                     SIO_IOCTL_POWERDOWN,
                     NULL
                   );
#endif /* FEATURE_SIO_CLIENT_SLEEP_CONTROL */
      }
      bt_dc.sio_state = BT_SIO_SLEEPING;     
#ifdef BT_SLEEP_DEBUG
      BT_MSG_DEBUG( "BT DC: Voting to sleep ", 0, 0, 0 );
#endif
      break;

    case BT_SIO_AWAKE:
      /* Wakeup SIO if it has been shut down */
      if (( bt_dc.sio_state == BT_SIO_SLEEPING ) &&
          change_sio_vote && 
          bt_debug_enable_sio_sleep_vote )
      { 
#ifdef FEATURE_SIO_CLIENT_SLEEP_CONTROL
          sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                     SIO_IOCTL_WAKEUP,
                     NULL
                   );
#endif /* FEATURE_SIO_CLIENT_SLEEP_CONTROL */
      }
      bt_dc.sio_state = BT_SIO_AWAKE; 
#ifdef BT_SLEEP_DEBUG
      BT_MSG_DEBUG( "BT DC: Voting to stay awake ", 0, 0, 0 );
#endif
      break; 

    case BT_SIO_IDLE:
      bt_dc.sio_state = BT_SIO_IDLE;
      BT_MSG_DEBUG( "BT DC: Change SIO state to IDLE", 0, 0, 0 );
      break;

    default:
      BT_ERR( "BT DC: Invalid SIO state requested ", 0, 0, 0 );
      break;
  }

} /* bt_dc_update_sio_state */

#else /* FEATURE_BT_SOC */
/*===========================================================================

FUNCTION
  bt_dc_enable_driver

DESCRIPTION

===========================================================================*/
void bt_dc_enable_driver( void )
{

  bt_dsp_image_enum_type  dsp_image_requested;

  MSG_HIGH( "BT DC: Enable driver with QDSP image %d",
            bt_dc.dsp_bt_image_to_request, 0, 0 );

  /* In the future, im can be a bitmap of different Bluetooth modules,
   * e.g. data, voice, low power mode, test mode, etc., in which
   * multiple modules can be requested by a BT application, and this
   * function will be responsible for mapping them to the appropriate QDSP
   * image
   */

#ifdef FEATURE_DCVS_LPM
  /* This is a 6550 only feature. */
  /* Vote to increase clock speed */
  clk_regime_register_for_cpu_resource( CLKRGM_BT_CLIENT );
#endif /* FEATURE_DCVS_LPM */

#ifdef FEATURE_GPIO_CONFIG_FOR_SLEEP 
  /* This is a 6550 only feature. */
  /* Tell Clock not to configure the GPIOs */
  clk_regime_gpio_client_enable( CLKRGM_GPIO_BT_CLIENT, TRUE );
#endif /* FEATURE_GPIO_CONFIG_FOR_SLEEP  */

  /* Update desired DSP image. */
  bt_dc.dsp_image_desired = 
    bt_dc_get_dsp_image_type( bt_dc.dsp_bt_image_to_request );

  dsp_image_requested = bt_dc.dsp_image_desired;

  if ( dsp_image_requested == BT_DSP_IMAGE_MAX )
  {
    BT_ERR_FATAL( "BT DC: Unknown image type", 0, 0, 0 );
  }

  if ( bt_dc_is_bt_image( bt_dc.dsp_image_current ) )
  {
    /* If the current QDSP image contains Bluetooth, for now, request 
    ** that one instead because that image is good enough.
    */
    dsp_image_requested = bt_dc.dsp_image_current;
  }

  bt_dc.driver_state    = BT_DS_PENDING_QDSP_DOWNLOAD;
  bt_dc.driver_substate = BT_DSS_NONE;

  bt_dc.dsp_image_requested = dsp_image_requested;
  BT_DSP_ENABLE( bt_dc.dsp_image_requested );

}


/*===========================================================================

FUNCTION
  bt_dc_disable_driver

DESCRIPTION

===========================================================================*/
void bt_dc_disable_driver( void )
{

  MSG_HIGH( "BT DC: Disable driver", 0, 0, 0 );

#ifdef FEATURE_DCVS_LPM
  /* This is a 6550 only feature. */
  /* Vote to reduce clock speed   */
  clk_regime_deregister_for_cpu_resource( CLKRGM_BT_CLIENT );
#endif

#ifdef FEATURE_GPIO_CONFIG_FOR_SLEEP 
  /* This is a 6550 only feature. */
  /* Tell Clock GPIO configuration is OK */
  clk_regime_gpio_client_enable( CLKRGM_GPIO_BT_CLIENT, FALSE );
#endif /* FEATURE_GPIO_CONFIG_FOR_SLEEP */

  if ( bt_dc.driver_state == BT_DS_READY )
  {
    bt_dc.driver_state              = BT_DS_QDSP_SWAPPING_OUT;
    bt_dc.driver_substate           = BT_DSS_SWAP_OUT_TO_IDLE;
    bt_dc.dsp_image_desired  = BT_DSP_IMAGE_MAX;

    /* In a good state to swap out.  Do it now. */
    ( void ) rex_set_sigs( rex_self(), BT_QDSP_RESET_SIG );
  }

}


/*===========================================================================

FUNCTION
  bt_dc_reset_driver

DESCRIPTION
  Reset the Bluetooth driver.  Disable the QDSP, go offline, then
  re-initialize the task.

===========================================================================*/
void bt_dc_reset_driver( void )
{

  MSG_HIGH( "BT DC: Reset driver", 0, 0, 0 );

  if ( bt_dc.driver_state == BT_DS_READY )
  {
    bt_dc.driver_state      = BT_DS_QDSP_SWAPPING_OUT;
    bt_dc.driver_substate   = BT_DSS_SWAP_OUT_TO_RESET;
    bt_dc.dsp_image_desired = BT_DSP_IMAGE_MAX;

    /* TBD -- implement later */
  }

}
#endif /* FEATURE_BT_SOC */


/*===========================================================================

FUNCTION
  bt_dc_check_new_hci_mode

DESCRIPTION
  Validates a new HCI mode being requested.

===========================================================================*/
LOCAL bt_cmd_status_type bt_dc_check_new_hci_mode
(
  bt_hci_mode_type  new_hci_mode
)
{
  
  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;
  
  switch ( new_hci_mode )
  {
    case BT_HCIM_OFF:
      {
        break;
      }
    
    case BT_HCIM_STANDARD_HCI_ON:
    case BT_HCIM_STANDARD_HCI_VOC_ON:
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
      {
        if ( bt_dc.sio_port_id == SIO_PORT_NULL )
        {
          cmd_status = BT_CS_DC_NO_SIO_PORT_ID;
        }
        break;
      }

#ifdef FEATURE_FACTORY_TESTMODE
    case BT_HCIM_STANDARD_HCI_FTM:
    case BT_HCIM_STANDARD_APIT_FTM:
      {
        if ( bt_dc.sio_port_id != SIO_PORT_NULL )
        {
          cmd_status = BT_CS_DC_SIO_PORT_ALREADY_OPENED;
        }
        break;
      }
#endif /* FEATURE_FACTORY_TESTMODE */

    default:
      {
        cmd_status = BT_CS_DC_BAD_HCI_MODE;
        break;
      }
  }

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_dc_reg_hci_events_to_rm

DESCRIPTION
  Registers HCI events to go to the Radio Manager or to the HCI parser
  as specified.

===========================================================================*/
LOCAL bt_cmd_status_type bt_dc_reg_hci_events_to_rm
(
  boolean  to_rm
)
{

  bt_app_id_type       new_hci_ev_reg_app_id;
  bt_ev_cb_func_type*  new_hci_ev_reg_cb_ptr;
  bt_cmd_status_type   cmd_status = BT_CS_GN_SUCCESS;

  if ( to_rm != FALSE )
  {
    /*  Register HCI events to go to the Radio Manager.  */
    new_hci_ev_reg_app_id = bt_rm_app_id;
    new_hci_ev_reg_cb_ptr = bt_rm_store_bt_event;
  }
  else
  {
    /*  Register HCI event to go to the HCI layer.  */
    new_hci_ev_reg_app_id = bt_hc_app_id;
    new_hci_ev_reg_cb_ptr = bt_hc_store_bt_event;
  }

  if ( new_hci_ev_reg_app_id != BT_APP_ID_NULL )
  {
    if ( bt_dc_last_hci_ev_reg_app_id != new_hci_ev_reg_app_id )
    {
      if ( bt_dc_last_hci_ev_reg_app_id != BT_APP_ID_NULL )
      {
        BT_MSG_API( "BT DC CMD TX: EC (Un)Rereg AID %x",
                    bt_dc_last_hci_ev_reg_app_id, 0, 0 );
        cmd_status = bt_cmd_ec_reregister_app_id(
                       bt_dc_last_hci_ev_reg_app_id, NULL );
        ASSERT_DEBUG( cmd_status == BT_CS_GN_SUCCESS );
      }
      BT_MSG_API( "BT DC CMD TX: EC Rereg AID %x CB %x",
                  new_hci_ev_reg_app_id,
                  new_hci_ev_reg_cb_ptr, 0 );
      cmd_status = bt_cmd_ec_reregister_app_id(
                     new_hci_ev_reg_app_id,
                     new_hci_ev_reg_cb_ptr );
      ASSERT_DEBUG( cmd_status == BT_CS_GN_SUCCESS );
      bt_dc_last_hci_ev_reg_app_id = new_hci_ev_reg_app_id;
    }
  }
  else
  {
    cmd_status = BT_CS_DC_NO_APP_ID;
  }

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_dc_set_hci_mode

DESCRIPTION
  Sets the HCI mode of the Bluetooth driver to the one specified.

===========================================================================*/
bt_cmd_status_type bt_cmd_dc_set_hci_mode
(
  bt_hci_mode_type  new_hci_mode
)
{
  
  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;
  boolean route_events_to_rm = TRUE;

  BT_MSG_API( "BT DC: Set HCI Mode %x SID %x SPID %x",
              new_hci_mode, bt_dc.sio_stream_id,
              bt_dc.sio_port_id );

  rex_enter_crit_sect( &bt_dc_crit_sect );
  TASKLOCK();
  
  if ( (cmd_status = bt_dc_check_new_hci_mode( new_hci_mode )) ==
          BT_CS_GN_SUCCESS )
  {
    bt_dc.cur_hci_mode = new_hci_mode;

    if ( BT_IN_HCI_MODE() )
    {
      /*  Register HCI event to go to the HCI layer.  */
      route_events_to_rm = FALSE;

#ifndef FEATURE_BT_SOC
      bt_dc.dsp_dont_unload = TRUE;
      if ( new_hci_mode == BT_HCIM_STANDARD_HCI_ON )
      {
        bt_dc.dsp_bt_image_to_request = BT_IMAGE_BT_ONLY;
      }
      else
      {
        bt_dc.dsp_bt_image_to_request = BT_IMAGE_VOC;
      }
#endif /* FEATURE_BT_SOC */

      /* Clear Idle timer, Driver should not disable during HCI mode. */
      BT_MSG_API( "BT DC: Entering HCI Mode, clearing Driver Idle Timer ", 0, 0, 0 );
      rex_clr_timer( &bt_soc_driver_idle_timer );
      rex_clr_sigs( &bt_tcb, BT_DRIVER_IDLE_TIMER_SIG );  

    }
    else
    {
      /*  Register HCI events to go to the Radio Manager.  */
      route_events_to_rm = TRUE;

#ifdef FEATURE_BT_SOC
      if (bt_dc.driver_state == BT_DS_SOC_DISABLING)
      {
        BT_MSG_API( "BT DC: Exiting HCI Mode, change driver state to ACTIVE ", 0, 0, 0 );
        /* The disable driver process was initiated, but not completed.
           Change driver state back to ACTIVE.  It will go back to DISABLING
           if this still makes sense. */
        BT_DC_UPDATE_DRIVER_STATE(BT_DS_SOC_ACTIVE);
      }
#endif

#ifndef FEATURE_BT_SOC
      bt_dc.dsp_dont_unload = FALSE;
    
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_SOC */
    }
  }
  else
  {
    BT_ERR( "BT DC: Bad Set HCI Mode %x P %x S %x",
            new_hci_mode, bt_dc.sio_port_id, cmd_status );
  }

  TASKFREE();

  if ( cmd_status == BT_CS_GN_SUCCESS )
  {
    bt_dc_reg_hci_events_to_rm( route_events_to_rm );
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
  }
  rex_leave_crit_sect( &bt_dc_crit_sect );

  return ( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_dc_get_hci_mode

DESCRIPTION
  Get current HCI mode.

===========================================================================*/
bt_hci_mode_type bt_cmd_dc_get_hci_mode( void )
{
  return bt_dc.cur_hci_mode;
}


/*===========================================================================

FUNCTION
  bt_cmd_dc_get_bd_addr

DESCRIPTION
  Get MY_BD_ADDR

===========================================================================*/

void bt_cmd_dc_get_bd_addr
(
  bt_bd_addr_type *bd_addr_ptr
)
{  
  if( bd_addr_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    TASKLOCK();
    bt_dc_local_bd_addr = bt_my_bd_addr;
    TASKFREE();
    *bd_addr_ptr = bt_dc_local_bd_addr;
    rex_leave_crit_sect( &bt_dc_crit_sect );
  }
}


/*===========================================================================

FUNCTION
  bt_cmd_dc_set_bd_addr

DESCRIPTION
  Sets this unit's BD_ADDR.

===========================================================================*/
bt_cmd_status_type bt_cmd_dc_set_bd_addr
(
  bt_bd_addr_type*  bd_addr_ptr
)
{
  if( bd_addr_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    bt_dc_local_bd_addr = *bd_addr_ptr;

    TASKLOCK();
    /* Update RAM */
    bt_my_bd_addr = bt_dc_local_bd_addr;

#ifndef FEATURE_BT_SOC
    if ( bt_dc.driver_state == BT_DS_READY )
    {
      /* Update QDSP */
      bt_a2q_memcpy( (uint8*) bt_qd_my_bd_addr_ptr,
                     (uint8*) &bt_dc_local_bd_addr,
                     sizeof( bt_bd_addr_type ) );
    }
#endif /* FEATURE_BT_SOC */
    TASKFREE();
    rex_leave_crit_sect( &bt_dc_crit_sect );
    return ( BT_CS_GN_SUCCESS );
  }
  else
  {
    return ( BT_CS_GN_BAD_CMD_STATE );
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_dc_get_refclock_type

DESCRIPTION
  Get REF_CLK_TYPE

===========================================================================*/

void bt_cmd_dc_get_refclock_type
(
  bt_soc_refclock_type *bt_refclock_type_ptr
)
{  
  if( bt_refclock_type_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    *bt_refclock_type_ptr = bt_dc_local_refclock_type;
    rex_leave_crit_sect( &bt_dc_crit_sect );
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_dc_set_refclock_type

DESCRIPTION
  Sets this unit's ref clk.

===========================================================================*/
bt_cmd_status_type bt_cmd_dc_set_refclock_type
(
  bt_soc_refclock_type *bt_refclock_type_ptr
)
{
  if( bt_refclock_type_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    bt_dc_local_refclock_type = *bt_refclock_type_ptr;
    rex_leave_crit_sect( &bt_dc_crit_sect );
    return ( BT_CS_GN_SUCCESS );
  }
  else
  {
    return ( BT_CS_GN_BAD_CMD_STATE );
  }
}
/*===========================================================================

FUNCTION
  bt_cmd_dc_get_clock_sharing_type

DESCRIPTION
  Get CLK_SHARING_TYPE

===========================================================================*/

void bt_cmd_dc_get_clock_sharing_type
(
  bt_soc_clock_sharing_type *bt_clock_sharing_type_ptr
)
{  
  if( bt_clock_sharing_type_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    *bt_clock_sharing_type_ptr = bt_dc_local_clock_sharing_type;
    rex_leave_crit_sect( &bt_dc_crit_sect );
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_dc_set_clock_sharing_type

DESCRIPTION
  Sets this unit's clk sharing type.

===========================================================================*/
bt_cmd_status_type bt_cmd_dc_set_clock_sharing_type
(
  bt_soc_clock_sharing_type *bt_clock_sharing_type_ptr
)
{
  if( bt_clock_sharing_type_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    bt_dc_local_clock_sharing_type = *bt_clock_sharing_type_ptr;
    rex_leave_crit_sect( &bt_dc_crit_sect );
    return ( BT_CS_GN_SUCCESS );
  }
  else
  {
    return ( BT_CS_GN_BAD_CMD_STATE );
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_dc_get_inband_sleep_type

DESCRIPTION
  Get INBAND_SLEEP_TYPE

===========================================================================*/

void bt_cmd_dc_get_inband_sleep_type
(
  bt_soc_inband_sleep_type *bt_inband_sleep_type_ptr
)
{  
  if( bt_inband_sleep_type_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    *bt_inband_sleep_type_ptr = bt_dc_local_inband_sleep_type;
    rex_leave_crit_sect( &bt_dc_crit_sect );
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_dc_set_inband_sleep_type

DESCRIPTION
  Sets this unit's inband sleep type.

===========================================================================*/
bt_cmd_status_type bt_cmd_dc_set_inband_sleep_type
(
  bt_soc_inband_sleep_type *bt_inband_sleep_type_ptr
)
{
  if( bt_inband_sleep_type_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    bt_dc_local_inband_sleep_type = *bt_inband_sleep_type_ptr;
    rex_leave_crit_sect( &bt_dc_crit_sect );
    return ( BT_CS_GN_SUCCESS );
  }
  else
  {
    return ( BT_CS_GN_BAD_CMD_STATE );
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_dc_get_soc_logging

DESCRIPTION
  Get SOC logging information.

===========================================================================*/

void bt_cmd_dc_get_soc_logging
(
  uint8 *bt_soc_logging_ptr
)
{  
  if( bt_soc_logging_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    *bt_soc_logging_ptr = bt_dc_local_soc_logging;
    rex_leave_crit_sect( &bt_dc_crit_sect );
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_dc_set_soc_logging

DESCRIPTION
  Sets the SOC logging information.

===========================================================================*/
bt_cmd_status_type bt_cmd_dc_set_soc_logging
(
  uint8 *bt_soc_logging_ptr
)
{
  if( bt_soc_logging_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    bt_dc_local_soc_logging = *bt_soc_logging_ptr;
    rex_leave_crit_sect( &bt_dc_crit_sect );
    return ( BT_CS_GN_SUCCESS );
  }
  else
  {
    return ( BT_CS_GN_BAD_CMD_STATE );
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_dc_get_bt_2_1_lisbon_status

DESCRIPTION
  Get the BT 2_1 / Lisbon information. 

===========================================================================*/

void bt_cmd_dc_get_bt_2_1_lisbon_status
(
  uint8 *bt_soc_bt_2_1_lisbon_status_ptr
)
{  
  if( bt_soc_bt_2_1_lisbon_status_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    *bt_soc_bt_2_1_lisbon_status_ptr = bt_soc_bt_2_1_lisbon_disabled;
    rex_leave_crit_sect( &bt_dc_crit_sect );
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_dc_set_bt_2_1_lisbon_status

DESCRIPTION
  Sets the BT 2_1 / Lisbon information.

===========================================================================*/
bt_cmd_status_type bt_cmd_dc_set_bt_2_1_lisbon_status
(
  uint8 *bt_soc_bt_2_1_lisbon_status_ptr
)
{
  if( bt_soc_bt_2_1_lisbon_status_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    bt_soc_bt_2_1_lisbon_disabled = *bt_soc_bt_2_1_lisbon_status_ptr;
    rex_leave_crit_sect( &bt_dc_crit_sect );
    return ( BT_CS_GN_SUCCESS );
  }
  else
  {
    return ( BT_CS_GN_BAD_CMD_STATE );
  }
}


/*===========================================================================

FUNCTION
  bt_cmd_dc_is_bt_on

DESCRIPTION
  Checks if BT is "on".

===========================================================================*/
boolean bt_cmd_dc_is_bt_on( void )
{
#ifndef FEATURE_BT_SOC
  bt_driver_state_enum_type driver_state;

  TASKLOCK();
  driver_state = bt_dc.driver_state;
  TASKFREE();
  
  /* BT is "off" when driver state is BT_DS_IDLE or BT_DS_QDSP_OUT */
  return( ( driver_state >= BT_DS_PENDING_QDSP_DOWNLOAD ) && 
          ( driver_state <= BT_DS_DSP_SLEEPING ) );
#else
  boolean bt_on;
  /* Static extension API that uses this API expects the following : 
   * BT is "on" if the device is in a connection, paging, inquiring, scanning
  */

  TASKLOCK();
  bt_on = ( ( bt_dc.driver_state == BT_DS_SOC_ACTIVE ) &&
            ( bt_dc.inq_active || bt_dc.page_active || bt_dc.scan_active || 
              bt_rm_any_conn_active() 
             )
           );
  TASKFREE();

  return (bt_on);
#endif /* FEATURE_BT_SOC */
} 


#ifdef FEATURE_FACTORY_TESTMODE
/*===========================================================================

FUNCTION
  bt_cmd_dc_ftm_get_bt_wm

DESCRIPTION
  This function is used by FTM to get the from_sio and to_sio watermarks.

  The from_sio watermark, which SIO enqueues data onto and BT dequeues
  the data from using the non_empty callback mechanism, can be used to
  feed data into the BT HCI parsing engine.

  The to_sio watermark, which SIO enqueues BT data onto when the transmit
  buffer is full (note that BT does not enqueue data onto the to_sio
  watermark itself, but calls sio_transmit instead), can be used to get
  data out of BT by registering own non_empty callback.

  When FTM uses these watermarks to get data in and out of BT, it is
  short-circuiting the regular HCI interface via serial port, so HCI mode
  needs to be in FTM mode.

===========================================================================*/
void bt_cmd_dc_ftm_get_bt_wm
(
  dsm_watermark_type** ftm_to_bt_wm_ptr_ptr,
  dsm_watermark_type** ftm_from_bt_wm_ptr_ptr
)
{
  *ftm_to_bt_wm_ptr_ptr   = 0;
  *ftm_from_bt_wm_ptr_ptr = 0;

  if ( (( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_HCI_FTM ) ||
       (bt_dc.cur_hci_mode ==  BT_HCIM_STANDARD_APIT_FTM))&&
       ( bt_dc.sio_port_id  == SIO_PORT_NULL ) )
  {
    *ftm_to_bt_wm_ptr_ptr   = &bt_from_sio_wm;
    *ftm_from_bt_wm_ptr_ptr = &bt_to_sio_wm;
  }
}

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#endif /* FEATURE_MULTIPROCESSOR */

/*===========================================================================

FUNCTION 
  bt_ftm_cmd

DESCRIPTION
  This function is used by FTM to send FTM diag packet containing BT HCI
  command to BT.
  
===========================================================================*/
void bt_ftm_cmd
(
  uint8* data,
  uint16 length
)
{
#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#else
  BT_ERR ("bt_ftm_cmd() is not supported for this target", 0, 0, 0);
#endif /* FEATURE_MULTIPROCESSOR */
}
#endif /* FEATURE_FACTORY_TESTMODE */


#endif /* FEATURE_BT */

/*===========================================================================


FUNCTION
  bt_cmd_dc_set_soc_version

DESCRIPTION
  Sets the SOC version

===========================================================================*/
void bt_cmd_dc_set_soc_version
(
  bt_qsoc_enum_type *bt_soc_version
)
{
  if( bt_soc_version != NULL )
  {
    rex_enter_crit_sect( &bt_dc_crit_sect );
    bt_dc_local_soc_version = *bt_soc_version;
    rex_leave_crit_sect( &bt_dc_crit_sect );

  }

}
/*===========================================================================

FUNCTION
  bt_cmd_dc_get_soc_version

DESCRIPTION
  Get SOC version

===========================================================================*/

bt_qsoc_enum_type bt_cmd_dc_get_soc_version
(
  void
)
{  
  return(bt_dc_local_soc_version);
}

/*===========================================================================*/


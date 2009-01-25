#ifndef _BT_H
#define _BT_H

#include "customer.h"

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     B L U E T O O T H    M O D U L E

DESCRIPTION
  This file contains declarations external to the Bluetooth module.

              Copyright (c) 2000-2010 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary                        
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/bt.h_v   1.52   09 Aug 2002 18:36:50   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/inc/bt.h#6 $ 
$DateTime: 2010/05/12 22:07:33 $
$Author: uppalas $

$Log:   O:/src/asw/COMMON/vcs/bt.h_v  $
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-05-06   us  Disbale Scans when intiating Remote Name request or
                   Create Connection for some SOC versions.
  2009-05-04   pg  Added event notifications for radio power on/off and sync
                   connection parameter changes.
  2009-02-18   hm  Created an API for Bond Cancel
  2009-01-14   gs  Added ev_opp_cli_con_prog_ind in event union struct
  2009-01-13   vk  Added AUTH failure reason in bt_rm_disc_reason_type.
  2008-12-03   vk  Added proto type for sleep voting functions.
  2008-11-05   pg  Generic handling for HCI Vendor Specific cmd group.
  2008-10-27   sa  Added support to read BT 2.1 support from a nv item in 
                   NV browser at run-time.
  2008-09-10   ap  Fixed lint compiler warning.
  2008-09-09   NM  Added data structures to handle retrieval of link key for
                   automation
  2008-08-25   gs  Updated bt_lm_esco_latency_type enum
  2008-08-21   pg  Added bt_cmd APIs to read remote feature and synchronous
                   connection parameters.
  2008-08-13   rb  Initial version with DCVS implementation
  2008-07-28   sa  Added support to read Clk type,Clk sharing,in-band sleep mode, 
                   soc logging enabled, BT Wlan co-ext, BTS power mgmt mode  
                   from NV browser at run-time.
  2008-05-28   hs  Added the support for AVRCP 1.3 meta data feature
                   by adding event structure declarations
  2008-04-30   rb  Fix warning - Superfluous ',' at the end of enum
  2008-03-24   vk  Added macros for HCI Read/write Defalut Erroneous Data Reporting mode
                   commands.
  2008-03-13   ag  Added support for MITM_NO_BONDING for GAP by adding
                   a new security level

 *
 *   #169       05 Mar 2008            AG
 * Update service security result event structure for 2.1.
 *
 *   #168       03 Mar 2008            GS  
 * Added Get/SetReport HID event types to bt_ev_params_union.
 *
 *   #167       29 Feb 2008            AG
 * Rename BT_CMD_HC_IO_CAPABILITY_RESPONSE to BT_CMD_HC_IO_CAPABILITY_RQST_REPLY
 * for consistency with spec.
 *
 *   #166       28 Jan 2008            RS  
 * Making the BT_SD_MAX_NUM_OF_SRV_REC value same as 
 * AEEBT_MAX_NUM_OF_SRV_REC in file AEEBTExtRM.h.
 *
 *   #164       16 Jan 2008            DSN 
 * Adding unique defintions for more vendor-specific command groups.
 *
 *   #163       10 Jan 2008            SSK
 * Removing Inband's Sleep usage of SOC_SIG6(0x00800000)\BT_SOC_SIG5 0x80000000
 * since these are global signals used by RPC\rex.h 
 *
 *   #163       19 Dec 2007            AG
 * Moved BT2.1 commands to be under BT_CMD_EV_HC_BASE, rather than 
 * BT_CMD_EV_FCTL_BASE.
 *
 *   #162       12 Dec 2007            HM
 * Added SD extended command to register custom services 
 *
 *   #161       05 Dec 2007            AG
 * BT2.1 - changed enum type bt_encrypt_mode_type to take into account
 * new definition in 2.1 spec.
 *
 *   #160       29 Nov 2007            HM
 * Added SD command to return service search results in XML format
 * Added command to cancel service search. 
 * Added a parameter to specify duration of a device search. 
 *
 *   #159       29 Nov 2007            SSK
 * Implemented Inband Sleep mechanism under FEATURE_BT_QSOC_INBAND_SLEEP
 *
 *   #158       27 Nov 2007            PN
 * Added role_sw_ok and sco_present to link status.
 *
 *   #157       20 Nov 2007            PC
 * Added ONCRPC meta comments for BT commands.
 *
 *   #156       07 Nov 2007            GS
 * Added support for HID profile host role and API for canceling SD
 * idle timer.
 *
 *   #155       01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *   #154       31 Oct 2007            VK
 * Added proto type to support Inquiry result event with RSSI.
 *
 *   #153       19 Oct 2007            SSK
 * Add support for FCTL CMD type for handling DSM item level callbacks
 * during Controller to Host HCI Flow control.
 *
 *   #152       16 Oct 2007            DSN
 * Added support for Rd Loc Supported commands and Rd local extended 
 * features command.
 *
 *   #151       04 Sept 2007            BH
 * Added support for QSoC Vender specific encryption key length command. 
 *
 *   #150       20 Aug 2007            PG
 * Seperated HCI generic vendor specific event handling from Qualcomm 
 * proprietary implementation used with embedded controller.
 *
 *   #149        24 AUG 2007            VK
 * Changed macros for Max Latency.
 *
 *   #148        25 JUL 2007            PR
 * Reordering SD, RM, HC cancel commands to support automation
 *
 *   #147        17 JUL 2007            PR
 * Remote name request cancel support in SD
 *
 *   #146        09 JUL 2007            PG
 * HCI Remote name req cancel and connection cancel support.
 *
 *   #145        29 May 2007            DSN
 * Add metacomments for remoting Apis for SAP, Move some typedefs to a new
 * header file btc.h to resolve circular referencing of btpf.h via bt.h
 * in order to enable RPC tool to generate BT PF remote apis.
 *
 *   #144        22 May 2007            SSK
 * undefine FEATURE_BT_SOC1_CNTLR_EN_SCATTERNET for SOC1 based targets.
 *
 *   #143        15 May 2007            SSK
 * Change error field type in  struct bt_ev_sd_error_resp_type from
 * enum type "bt_event_reason_type" to  "bt_sd_error_code_enum_type"
 *
 *   #142        09 May 2007            SSK
 * Introduce a new member field in bt_spp_ms_enum_type enumeration.
 *
 *   #141        05 MAY 2007            PG
 * Changes to remove compiler warnings with RVCT.
 *
 *   #140        05 MAY 2007            PG
 * Added new enum value for RM to disallow cmds.
 *
 *   #139        23 Apr 2007            SSK
 * Move bt_cmd_dc_ftm_get_bt_wm() outside FEATURE_MULTIPROCESSOR define.
 *
 *   #138        28 Feb 2007            JN
 * Following modifications were done:
 * - Moved SOC1 specific signal definitions to btsoc1 and replaced them with
 *   generic SoC signals.
 * - Added BTS Vendor Specific cmds and events to appropriate structs/unions
 *
 *    #137       08 Feb 2007            DM
 * Added enums for ESCO Max Latency and Retransmission Type Parameters.
 *
 *    #136       31 Jan 2007            ACG
 * Added a delay between receipt of yms and voting to sleep.
 *
 *    #135       29 Jan  2007           MH
 * Added support to update OBEX Profile service record with 128-bit UUID.
 *
 *    #134       25 Jan  2007           VK
 *  RM starts re-pairing according to PIN length requirement in security mode 2.
 *
 *    #133       18 Jan  2007           VK
 * Added enum for Role switch Encryption state.
 *
 *    #132       12 Jan 2007            GS
 * Added FTM-BT support for multi-processor target.
 *
 *    #131        8 Jan 2007            ACG
 * Following features were added for the SOC solution:
 * - vote for SIO sleep/wakeup based on BT activity
 * - enable/disable driver
 * - radio on/off
 * 
 *    #130       22 Nov  2006           JTL
 * Add RFComm command to cancel a connect operation.
 *
 *    #129       20 Nov  2006           PR
 * Added OBEX command status codes while merging OI Code v3.0.14
 *
 *   #128        14 Nov  2006           RY
 * Added SD support for custom 128-bit UUID
 *
 *    #127        08 Nov  2006           VK
 * RM sends more appropriate error codes, when bonding fails.
 *
 *    #126        31 Oct  2006           VK
 * Fixing compilation conflict of bt_rm_atzrq_type enum.
 *
 *    #125        26 Oct  2006           VK
 * Added new enum definition and one new element in 
 * bt_ev_rm_authorize_request_type structure for Authorization Direction.
 *
 *    #124       20 Oct  2006           SSK
 * External SOC suppport changes.
 *
 *    #123       09 Oct  2006           GS
 * Fixing compilation issues in enabling APIT and BT 1.2 feature 
 * on 7K target.
 *
 *    #122       25 Sept 2006           MH
 * Added OBEX command status codes.
 *
 *    #121       19 Sept 2006           MH
 * Added GOEP Client authentication response event.
 *
 *    #120       07 Sep 2006            DM
 * Added new define , to track flushable packets that dosent need ack, 
 * Increased tick time and flush time, to optimize performance
 *
 *    #119       22 Aug 2006            DSN
 * New signal for transport layer communication with sio on SOC solution.
 *
 *    #118       11 Aug 2006            GB        
 * Added BT_HCIM_STANDARD_APIT_FTM to bt_hci_mode_type enum for ASIA support
 *
 *    #117       28 Jul 2006            BH        
 * modified for obex client authentication
 *
 *    #116       28 Jul 2006            DM
 * Defined Flush timer signal
 * Added l2cap_tx_cfm events
 * 
 *    #115       26 Jul 2006            RP
 * Added errorcodes for A2DP   
 *
 *    #114       27 Jun 2006            DSN
 * Combine two RDM signals into one. Clean-up of unused variables.
 *
 *    #113       25 Jun 2006            DSN
 * Removed Bonding App(BA), RC2SIO related references that are no longer used.
 *
 *    #112       30 May 2006            GS
 * Added support for PBAP feature.
 *
 *    #111       5  Apr 2006            DP
 * Added RM commands for Extended Service Security and to Remove All
 * Devices from the device database.
 *
 *    #110       16 Mar 2006           JTL
 * Adding RC Unregister event.
 *
 *    #109       30 Jan 2006            PN
 * Added APIs to register for, unregister for, and get radio activities.
 *
 *    #107-108   20 Jan 2006           JTL
 * Added L2 command to disconnect ACL link without waiting for timeout.
 *
 *    #106       23 Nov 2005            DP
 * Added RM commands for White List and Extended Service Security.
 *
 *    #105       9  Nov 2005            DP
 * Renamed and extended SD service registration command.
 *
 *    #103       19 Oct 2005            JH
 * Added support for the HCI read remote extended features command and read
 *  remote extended features complete event.
 *
 *    #102       26 Aug 2005            PN
 * Mainlined support for Response & Hold feature of HFP1.5.  Moved relevant 
 * AG definitions to btag.c.
 *
 *    #101       22 Aug 2005            JH
 * Increased BT_RM_MAX_DEVICES to 30.
 *
 *    #100       1  Aug 2005            PN
 * Mainlined support for HFP 1.5 indicators.
 *
 *    #99        26 Jul 2005           JTL
 * Adding another AVDTP error value.
 *
 *    #98        13 Jul 2005           JTL
 * Add more A2DP events: start & suspend.
 *
 *    #97        29 Jun 2005           JTL
 * Correcting error codes for AVDTP
 *
 *    #96        28 Jun 2005            JH
 * Fixed typo.
 *
 *    #95        27 Jun 2005            JH
 * Added BT 1.2 error codes and fast connection related HCI commands and events.
 *
 *    #94        24 Jun 2005            PN
 * Device Discovery Complete event now includes a reason code.
 *
 *    #93        16 Jun 2005            DP
 * Added LMP 2 timer for control of ACL packet type changes.
 *
 *    #92        27 May 2005           JTL
 * Adding A2DP command status codes.
 *
 *    #91        02 May 2005           JTL
 * Adding command to disable the L2 initial idle timeout once
 * a connection is established.
 *
 *    #90        29 Apr 2005            PN
 * Added BT_CS_AG_NO_SLC to bt_cmd_status_type.
 *
 *    #89        10 Apr 2005            RY
 * Adding support for SD Get Discoverable Mode.
 *
 *    #88        06 Apr 2005            JH
 * Adding support for disabling BT radio.
 *
 *    #85-87     24 Mar 2005           JTL
 * Adding A2DP top layer for DSMs.
 * Adding SBC status codes.
 *
 *    #82,84     17 Mar 2005           JTL
 * Adding HCI command status.
 * AV profile fixes.
 *
 *    #83        16 Mar 2005            DP
 * Added RM Service Security Result event.  Increased maximum number
 * of BT SIO ports available.
 *
 *    #81        05 Mar 2005           JH
 * Added AFH and eSCO related declarations.
 *
 *    #80        04 Mar 2005           JTL
 * Added support for AV proiles.
 *
 *    #79        28 Jan 2005            JH
 * Increased BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID from 5 to 12.
 *
 *    #78        22 Jan 2005            PN
 * Added support for SIM Access profile.
 *
 *    #77        05 Jan 2005            JH
 * Added command bt_cmd_dc_is_bt_on().
 *
 *    #76        29 Dec 2004            JH
 * Added commands BT_CMD_RM_AUTO_SERVICE_SEARCH_DISABLE and 
 *  BT_CMD_RM_DEVICE_SERVICES_UPDATE.
 *
 *    #75        29 Dec 2004            JH
 * Added the following SD commands and event: 
 *  BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER, 
 *  BT_CMD_SD_UPDATE_SRV_REC_BY_SCN, BT_CMD_SD_UPDATE_SRV_REC_BY_PSM,
 *  BT_CMD_SD_UPDATE_SRV_NAME, BT_CMD_SD_REGISTER_SRV, 
 *  BT_CMD_SD_UNREGISTER_SRV_BY_SCN, BT_CMD_SD_UNREGISTER_SRV_BY_PSM,
 *  BT_EV_SD_SECONDARY_SERVER_CHANNEL_NUMBER_RESP
 *
 *    #73-74     16 Dec 2004            JTL
 * Added SIO command queue for all IOCTLs and transmit.
 *
 *    #72        29 Sep 2004            JH
 * Increased BT_SD_MAX_NUM_OF_SRV_REC from 8 to 12.
 *
 *    #71        02 Sep 2004            JTL
 * Added SPP events. Note: SPP info is still delivered thru callbacks, these
 * events will only be sent within BT driver code.
 *
 *    #70        30 Aug 2004            JH
 * Added data type bt_sd_uint8_list_type and constant BT_SD_MAX_NUM_OF_UINT.
 * Added the following 4 commands:
 *  bt_cmd_sd_search_service_uuid128
 *  bt_cmd_sd_update_obex_profile_service_record
 *  bt_cmd_sd_register_syncml_service
 *  bt_cmd_sd_unregister_syncml_service
 *
 *    #69        25 Aug 2004            DP
 * Added battery charge, receive signal strength, and roam indicators
 * to AG.  Removed featurization of BT HF.
 *
 *    #68        12 Aug 2004            JH
 * Added TX continuous frequency HCI command
 *
 *    #67        10 Aug 2004            JTL
 * Adding another status to the bt_cmd_status_type for profile code.
 *
 *    #66         9 Aug 2004            JTL
 * Adding another status to the bt_cmd_status_type for profile code.
 *
 *    #65         5 Aug 2004            JTL
 * Removed DEVMGR stubs. 
 * Added prototype for bt_pf_map_event_code.
 *
 *    #64        30 Jul 2004            JH
 * Modified bt_link_mode_type and bt_low_power_ctrl_type to add entry for
 *  "SNIFF_LOCK" state which allows sending ACL data during sniff.
 *
 *    #63        28 Jul 2004            JTL
 * Adding bt_cmd_l2_connect_immediate_cid -- a command which immediately 
 * returns the CID L2 will use.
 *
 *    #62        26 Jul 2004            JTL
 * Removed L2 stubs. Put implementation in another file.
 *
 *    #61        21 Jul 2004            JTL
 * Added CTP/ICP profile interface.
 *
 *    #60        7  Jul 2004            JH
 * Added BT 1.2 AFH and eSCO HCI commands
 *
 *    #59        8  Jun 2004            DP
 * Added client side and PSM based Mode 2 security.
 *
 *    #58        6  Jun 2004            JH
 * Added FTP client browse and SyncML APIT support
 * Added BIP APIT events
 *
 *    #53-57     1  Jun 2004            DP
 * Implemented Mode 2 security and AG External I/O features.
 *
 *    #52        30 Apr 2004            RY
 * Added bt_sd_uuid128_type and SyncML events.
 *
 *    #51        25 Apr 2004            JH
 * Added external profile event ID, error codes, and event structures to the 
 *  respective enumerations and union.
 *
 *    #50        14 Apr 2004            RY
 * Added event base for BIP, BPP, and SyncML.
 *
 *    #49        12 Apr 2004            PN
 * Added bt_cmd_ag_send_pb_entries_info().
 *
 *    #48        8  Apr 2004            PN
 * Changed signature of bt_cmd_ag_send_pb_read_result().
 *
 *    #47        6  Apr 2004            PN
 * Added bt_cmd_dc_get_hci_mode().
 *
 *    #46        2  Apr 2004            DP
 * Added EMS support.
 *
 *    #45        25 Mar 2004            PN
 * - added more support for feature BT_AG_HF_EXTENSION
 *
 *    #44        3  Mar 2004            DP
 * Added RM command status for unexpected PIN reply command.
 *
 *    #43        23 Feb 2004            JH
 * Added support for top level OBEX packets for bt_get_dsm_ptr().
 * Added external profile command/event definitions.
 *
 *    #42        19 Feb 2004            DP
 * Added APIT SIO RX data event.
 *
 *    #41        29 Jan 2004            DP
 * Added device read iterator control to allow inclusion of volatile devices.
 *
 *    #40        14 Jan 2004            DP
 * Added support for limited discoverable scanning.
 *
 *    #39        12 Jan 2004            PN
 * Added bt_cmd_ag_update_vr_state().
 *
 *    #38        19 Dec 2003            DP
 * Added RM command to allow role switch requests from remote devices.
 * Made SD set service discoverable command keep track by application ID.
 * Fixed naming convention on LMP policy definitions.
 *
 *    #37        12 Dec 2003            DP
 * Added event reason for RM.
 *
 *    #36        26 Nov 2003            DP
 * Updated BA and SD command status values.
 *
 *    #35        14 Nov 2003            PN
 * Added new command status for AG.
 *
 *    #34        12 Nov 2003            DP
 * Added SDP command status for bad parameter values.
 *
 *    #32        9  Nov 2003            RY
 * Increased BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID from 4 to 5.
 *
 *    #31        4  Nov 2003            DP
 * Added security APIs and link status reports to RM.  General cleanup.
 *
 *    #30        3  Oct 2003            DP
 * Converted RM timer to background timer for entire driver.  Added
 * RFCOMM peer to peer communications monitoring.
 *
 *    #29        02 Oct 2003            PN
 * Added BT_SERVICE_CLASS_POSITIONING to bt_service_class_enum_type.
 *
 *    #28        2  Oct 2003            RY
 * Added BT_CS_AG_NO_BD_ADDR.
 *
 *    #27        26 Sep 2003            DP
 * Updated RM Device Manager API, AG connect/disconnect and VR capable APIs,
 * and made service/device names character strings.
 *
 *    #26        13 Sep 2003            RY
 * Fixed compiler warning.
 *
 *    #25        12 Sep 2003            RY
 * Added BT_HCIM_STANDARD_HCI_FTM.
 *
 *    #24        4  Sep 2003            RY
 * Added BT_SD_EV_ERROR_CONNECTION_TERMINATED.
 *
 *    #23        26 Aug 2003            RY
 * Added BT_RDM_CLOSE_SIG.
 *
 *    #22        06 Aug 2003            PN
 * Added support for RM Device DB.
 *
 *    #21        25 Jul 2003            RY
 * Added BT_RDM_OPEN_SIG.
 *
 *    #20        20 Jun 2003            PN
 * Undefined BT_AG_HF_EXTENSION.
 *
 *    #19        12 Jun 2003            PN
 * - Added support for BT_AG_HF_EXTENSION.
 * - Added BT_EV_AG_SLC_CHANGED to indicate change in Service Level Connection.
 * - Added BT_EV_AG_DEV_MEM_DIAL to indicate memory dialing by car-kit.
 * - Added BT_EV_AG_DEV_SEND_DTMF to indicate sending of DTMF by car-kit.
 * - Added bt_cmd_ag_set_inband_ring().
 * - Minor code clean up.
 *
 *    Rev 1.52   09 Aug 2002 18:36:50   waynelee
 * Added MSM6100 support.
 *
 *    Rev 1.51   05 Aug 2002 17:57:38   ryuen
 * Merged changes from MSM_BT.01.00.11.01.02 (MSM5100 2.1 Patch 6)
 *

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "dsm.h"
#include "queue.h"
#include "sio.h"
#include "task.h"
#include "btcomdef.h"
#include "btqsocnvm.h"

/*===========================================================================

             DATA DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*  Global definitions.                                                    */
/*-------------------------------------------------------------------------*/

#define BT_PACKED __packed

#undef BT_AG_HF_EXTENSION

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1_CNTLR_EN_SCATTERNET */
 
// ---------------------------------------------------------------------------
// These macros belong to comdef.h or some other global file
// ---------------------------------------------------------------------------
#ifdef ISBITSET
#undef ISBITSET
#endif
#define ISBITSET( flag, b ) (((flag) & (b)) == (b))

#ifdef SETBIT
#undef SETBIT
#endif
#define SETBIT( flag, b )    ((flag) |=  (b))

#ifdef CLRBIT
#undef CLRBIT
#endif
#define CLRBIT( flag, b )    ((flag) &= ~(b))


/*-------------------------------------------------------------------------*/
/*  Bluetooth task signal definitions.                                     */
/*-------------------------------------------------------------------------*/

#define BT_CMD_Q_SIG                               0x00000001
#define BT_EVENT_Q_SIG                             0x00000002
#ifdef FEATURE_BT_SOC
#define BT_SOC_SIG1                                0x00000004
#define BT_SOC_SIG2                                0x00000100
#define BT_SOC_SIG3                                0x00000200
#define BT_SOC_SIG4                                0x00040000
#define BT_SOC_SIG_ALL  (BT_SOC_SIG1 | BT_SOC_SIG2 | BT_SOC_SIG3 | BT_SOC_SIG4)
#else
#define BT_QDSP_EVENT_Q_SIG                        0x00000004
#define BT_QDSP_READY_SIG                          0x00000100
#define BT_QDSP_RESET_SIG                          0x00000200
#define BT_QDSP_STANDBY_SIG                        0x00040000
#endif /* FEATURE_BT_SOC */
#define BT_SE_AUTH_TIMER_SIG                       0x00000008
#define BT_SIO_RX_SIG                              0x00000010
#define BT_RC_TX_TIMER_SIG                         0x00000020
#define BT_WAIT_SIG                                0x00000040
#ifdef FEATURE_BT_SOC
#define BT_SIO_SOC_RX_SIG                          0x00000080
#else
#define BT_QDSP_POLL_SIG                           0x00000080
#endif /* FEATURE_BT_SOC */
#define BT_RPT_TIMER_SIG                           0x00000400
#define BT_DM_TIMER_SIG                            0x00000800
#define BT_BACKGROUND_TIMER_SIG                    0x00001000
#define BT_TASK_OFFLINE_SIG TASK_OFFLINE_SIG   /*  0x00002000 MC   */
#define BT_TASK_STOP_SIG    TASK_STOP_SIG      /*  0x00004000 MC   */
#define BT_TASK_START_SIG   TASK_START_SIG     /*  0x00008000 MC   */
#ifdef FEATURE_BT_SOC
#define BT_DRIVER_IDLE_TIMER_SIG                   0x00010000
#define BT_SIO_TX_FLUSH_WAIT_SIG                   0x00020000
#else
#define BT_LMP_TIMER_SIG                           0x00010000
#define BT_LMP_TIMER_2_SIG                         0x00020000
#endif
#define BT_L2_SIG_MSG_RESP_TIMER_SIG               0x00080000
#define BT_SD_TIMER_SIG                            0x00100000
#define BT_REQUEUED_CMD_Q_SIG                      0x00200000
#define BT_CLOCKS_SHUTDOWN_SIG                     0x00400000
#define BT_RDM_SIG                                 0x01000000
#ifdef FEATURE_BT_AG
#define BT_AG_TIMER_SIG                            0x02000000
#define BT_AG_RX_DATA_SIG                          0x04000000
#endif
#ifdef FEATURE_SEC
#define BT_UNUSABLE_SIG_1 SEC_OP_COMPLETE_SIG  /*  0x10000000 SEC  */
#endif
#ifdef FEATURE_VS_TASK
#define BT_UNUSABLE_SIG_2 VS_OP_COMPLETE_SIG   /*  0x20000000 VS   */
#endif
#ifdef FEATURE_EFS
#define BT_UNUSABLE_SIG_3 FS_OP_COMPLETE_SIG   /*  0x40000000 EFS  */
#endif
#define BT_L2FLUSH_TIMER_SIG                       0x08000000

/*-------------------------------------------------------------------------*/
/*  General definitions required by users of BT API.                       */
/*-------------------------------------------------------------------------*/

#define BT_APP_ID_NULL 0xFFFF

/*  BT driver event callback function prototype.  */
struct bt_ev_msg_struct;
typedef void (bt_ev_cb_func_type) ( struct bt_ev_msg_struct* bt_ev_msg_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK bt_ev_cb_func_type POINTER */
/*~ PARAM IN bt_ev_msg_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#define BT_BD_ADDRS_EQUAL( p1, p2 ) (                                 \
          (memcmp( (byte*)(p1), (byte*)(p2),                          \
                    sizeof( bt_bd_addr_type) ) == 0) ? TRUE : FALSE )

#ifndef CUST_EDITION  //[modify for device name length]
#define BT_MAX_NAME_LEN 32  /*  Same as UIBT_MAX_BT_NAME_LEN.  */
#else
#define BT_MAX_NAME_LEN 100  /*  Same as UIBT_MAX_BT_NAME_LEN.  */
#endif
#define BT_MAX_LOCAL_DEVICE_NAME_LEN (BT_MAX_NAME_LEN - 1)
#ifndef CUST_EDITION  //[modify for device name length]
#define BT_MAX_REMOTE_NICK_NAME_LEN 16
#else
#define BT_MAX_REMOTE_NICK_NAME_LEN 50
#endif
#define BT_SD_MAX_SERVICE_NAME_LEN  31
#ifdef CUST_EDITION //[add for Device name display]
#define BT_SD_MAX_DEVICE_NAME_LEN   100
#else
#define BT_SD_MAX_DEVICE_NAME_LEN   19
#endif

#define BT_MAX_PIN_CODE_LEN 16
typedef BT_PACKED struct
{
  uint8  length;
  uint8  pin_code[ BT_MAX_PIN_CODE_LEN ];
} bt_pin_code_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD bt_pin_code_type.pin_code VARRAY BT_MAX_PIN_CODE_LEN 
    LENGTH bt_pin_code_type.length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* TODO: What should this be ? */
#define BT_MAX_OOB_DATA_SIZE    100

/* This currently includes BD_ADDR, hash, rand and cod */
#define BT_LOCAL_OOB_DATA_SIZE  49

/* OOB Data types */
#define BT_OOB_TYPE_COD     0x0D
#define BT_OOB_TYPE_HASH    0x0E
#define BT_OOB_TYPE_RAND    0X0F

typedef BT_PACKED struct
{
  uint8  sp_hash_c[ 16 ];
} bt_sp_hash_type;

typedef BT_PACKED struct
{
  uint8  sp_rand_r[ 16 ];
} bt_sp_rand_type;

typedef BT_PACKED struct
{
  uint16  max_latency_ms;
  word    local_cid;
  uint8   server_channel;
  boolean stick_to_app_param;
  boolean sniff_initiator_oriented;
} bt_sniff_ssr_ctrl_type;

typedef enum  /*  For use with bt_get_free_dsm_ptr()  */
{
  BT_TL_L2CAP,
  BT_TL_RFCOMM,
  BT_TL_HCI_BB,
#ifdef FEATURE_BT_EXTPF_AV
  BT_TL_A2DP,
  BT_TL_AVDTP,
#endif /* FEATURE_BT_EXTPF_AV */
  BT_TL_OBEX
} bt_top_layer_type;

typedef enum
{
  BT_SCO_LINK,
  BT_ACL_LINK,
  BT_ESCO_LINK
} bt_link_type;

typedef enum
{
  BT_AM_MU_LAW_LOG = 0,
  BT_AM_A_LAW_LOG,
  BT_AM_CVSD,
  BT_AM_TRANSPARENT
} bt_sco_air_mode_type;

typedef BT_PACKED struct
{
  bt_link_type          link_type;
  uint8                 tx_interval;
  uint8                 retx_window;
  uint16                rx_pkt_length;
  uint16                tx_pkt_length;
  bt_sco_air_mode_type  air_mode;
  uint16                hc_handle;
} bt_sync_conn_param_type;

/*-------------------------------------------------------------------------*/
/*  General definitions not required by users of BT API.                   */
/*-------------------------------------------------------------------------*/

#define BT_BACKGROUND_TIMER_MS 500
#define BT_INVALID_ADDRESS     ((void *)0x7FFFFFFF)
#ifdef FEATURE_BT_2_1
#define BT_L2FLUSH_TIMER_MS    50
#else
#define BT_L2FLUSH_TIMER_MS    100
#endif
#define BT_L2FLUSH_MAX_TIME    200

/*-------------------------------------------------------------------------*/
/*  AG specific declarations.                                              */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_BT_AG

typedef enum
{
  BT_AD_HEADSET,
  BT_AD_HANDSFREE,
  BT_AD_MAX
} bt_audio_device_type;

typedef enum
{
  BT_AGIM_AUTOMATIC,              /*  Page scan continuously.   */
  BT_AGIM_LOW_POWER               /*  Connect to HS & sniff.    */
} bt_ag_idle_mode_type;

#define BT_AG_DEFAULT_HS_VOLUME_LEVEL          7
#define BT_AGIM_DEFAULT_IDLE_MODE     BT_AGIM_LOW_POWER
#define BT_AG_DEFAULT_IDLE_TIMEOUT    10                  /* in seconds */

typedef enum
{
  BT_LPM_SNIFF,
  BT_LPM_MAX
} bt_low_power_mode_type;

typedef enum
{
  BT_AG_RH_HOLD_INCOMING_CALL = 0, /* must be in this order */
  BT_AG_RH_ACCEPT_HELD_CALL,
  BT_AG_RH_REJECT_HELD_CALL,
  BT_AG_RH_MAX
} bt_ag_rh_state_type;

typedef enum
{
  BT_AG_BATTCHG_LEVEL_0 = 0,
  BT_AG_BATTCHG_LEVEL_1,
  BT_AG_BATTCHG_LEVEL_2,
  BT_AG_BATTCHG_LEVEL_3,
  BT_AG_BATTCHG_LEVEL_4,
  BT_AG_BATTCHG_LEVEL_5
} bt_ag_battchg_level_type;

typedef enum
{
  BT_AG_RSSI_LEVEL_0 = 0,
  BT_AG_RSSI_LEVEL_1,
  BT_AG_RSSI_LEVEL_2,
  BT_AG_RSSI_LEVEL_3,
  BT_AG_RSSI_LEVEL_4,
  BT_AG_RSSI_LEVEL_5
} bt_ag_rssi_level_type;

typedef enum
{
  BT_AG_CMD_TEST,
  BT_AG_CMD_READ,
  BT_AG_CMD_SET
} bt_ag_cmd_sub_type;

#ifdef BT_AG_HF_EXTENSION
typedef enum
{
  BT_PAP_DEV_ME = 0x00, /* ME phonebook list                  */
  BT_PAP_DEV_DC,        /* ME Dialed calls list               */
  BT_PAP_DEV_MC,        /* ME missed calls list               */
  BT_PAP_DEV_RC,        /* ME received calls list             */
  BT_PAP_DEV_SM,        /* SIM phonebook list                 */
  BT_PAP_DEV_FD,        /* SIM fix-dialing phonebook list     */
  BT_PAP_DEV_LD,        /* SIM last-dialing phonebook list    */
  BT_PAP_DEV_MT,        /* combined ME and SIM phonebook list */
  BT_PAP_DEV_MAX
} bt_pap_device_type;

typedef enum
{
  BT_PAP_SUCCESS = 0x0000,
  BT_PAP_ERROR,
  BT_PAP_ERROR_NO_SIM,
  BT_PAP_ERROR_INDEX_INVALID,
  BT_PAP_ERROR_NOT_FOUND,
  BT_PAP_ERROR_NOT_READY,
  BT_PAP_ERROR_NO_MEMORY,
  BT_PAP_ERROR_MAX
} bt_pap_return_type;

typedef enum
{
  BT_PAP_CMD_TEST = BT_AG_CMD_TEST,
  BT_PAP_CMD_READ = BT_AG_CMD_READ,
  BT_PAP_CMD_SET  = BT_AG_CMD_SET
} bt_pap_cmd_sub_type;

#define BT_PAP_MAX_NUM_LENGTH   32
#define BT_PAP_MAX_TEXT_LENGTH  64

typedef BT_PACKED struct
{
  int16   index;
  char    number[BT_PAP_MAX_NUM_LENGTH + 1]; /* null termiated string */
  uint8   type; /* type of address octet in integer format */
  uint16  text[BT_PAP_MAX_TEXT_LENGTH + 1]; /* null terminated string */
} bt_pap_record_type;

typedef BT_PACKED struct
{
  uint8 number[ BT_PAP_MAX_NUM_LENGTH+1 ];
  uint8 type; /* type of address octet in integer format */
  uint8 service;
} bt_pap_subsc_num_info_type;

typedef enum
{
  BT_PAP_RH_HOLD_INCOMING_CALL = BT_AG_RH_HOLD_INCOMING_CALL,
  BT_PAP_RH_ACCEPT_HELD_CALL   = BT_AG_RH_ACCEPT_HELD_CALL,
  BT_PAP_RH_REJECT_HELD_CALL   = BT_AG_RH_REJECT_HELD_CALL,
  BT_PAP_RH_MAX                = BT_AG_RH_MAX
} bt_pap_rh_state_type;

typedef enum
{
  BT_PAP_BATTCHG_LEVEL_0 = BT_AG_BATTCHG_LEVEL_0,
  BT_PAP_BATTCHG_LEVEL_1 = BT_AG_BATTCHG_LEVEL_1,
  BT_PAP_BATTCHG_LEVEL_2 = BT_AG_BATTCHG_LEVEL_2,
  BT_PAP_BATTCHG_LEVEL_3 = BT_AG_BATTCHG_LEVEL_3,
  BT_PAP_BATTCHG_LEVEL_4 = BT_AG_BATTCHG_LEVEL_4,
  BT_PAP_BATTCHG_LEVEL_5 = BT_AG_BATTCHG_LEVEL_5
} bt_pap_battchg_level_type;

typedef enum
{
  BT_PAP_RSSI_LEVEL_0 = BT_AG_RSSI_LEVEL_0,
  BT_PAP_RSSI_LEVEL_1 = BT_AG_RSSI_LEVEL_1,
  BT_PAP_RSSI_LEVEL_2 = BT_AG_RSSI_LEVEL_2,
  BT_PAP_RSSI_LEVEL_3 = BT_AG_RSSI_LEVEL_3,
  BT_PAP_RSSI_LEVEL_4 = BT_AG_RSSI_LEVEL_4,
  BT_PAP_RSSI_LEVEL_5 = BT_AG_RSSI_LEVEL_5
} bt_pap_rssi_level_type;
#endif /* BT_AG_HF_EXTENSION */
#endif /* FEATURE_BT_AG */


/*-------------------------------------------------------------------------*/
/*  BA specific declarations.                                              */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  uint8  link_key_bytes[ 16 ];
} bt_link_key_type;

typedef enum
{
  BT_HC_LINK_KEY_COMB = 0,
  BT_HC_LINK_KEY_LOCAL,
  BT_HC_LINK_KEY_REMOTE,
#ifdef FEATURE_BT_2_1
  BT_HC_LINK_KEY_DEBUG,
  BT_HC_LINK_KEY_UNAUTH,
  BT_HC_LINK_KEY_AUTH,
  BT_HC_LINK_KEY_CHANGED,
#endif /* FEATURE_BT_2_1 */
  BT_HC_LINK_KEY_NONE
} bt_hc_link_key_type;


#define BT_BA_MAX_NUM_OF_BONDED_DEVICE 10


/*-------------------------------------------------------------------------*/
/*  SDP specific declarations.                                             */
/*-------------------------------------------------------------------------*/

#define BT_SD_INVALID_UUID 0xFFFF

#ifdef CUST_EDITION //[added for not enough service record]
#define BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID    20
#define BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID 20
#else
#define BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID    12
#define BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID 12
#endif
#define BT_SD_MAX_NUM_OF_SRV_REC              20
#define BT_SD_MAX_NUM_OF_UINT                 8
#define BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID128 1
#define BT_SD_MAX_NUM_OF_UUID                 4
#define BT_SD_MAX_NUM_OF_UUID128              1

typedef enum /* For use with bt_cmd_sd_start_device_discovery() */
{
  BT_SERVICE_CLASS_UNKNOWN                   = 0x0000,
  BT_SERVICE_CLASS_LIMITED_DISCOVERABLE_MODE = 0x0001,
  BT_SERVICE_CLASS_NETWORKING                = 0x0002,
  BT_SERVICE_CLASS_RENDERING                 = 0x0004,
  BT_SERVICE_CLASS_CAPTURING                 = 0x0008,
  BT_SERVICE_CLASS_OBJECT_TRANSFER           = 0x0010,
  BT_SERVICE_CLASS_AUDIO                     = 0x0020,
  BT_SERVICE_CLASS_TELEPHONY                 = 0x0040,
  BT_SERVICE_CLASS_INFORMATION               = 0x0080,
  BT_SERVICE_CLASS_POSITIONING               = 0x0100,
  BT_SERVICE_CLASS_ALL                       = 0x01FF
} bt_service_class_enum_type;

typedef enum
{
  /* Bluetooth assigned error code for the BT_EV_SD_ERROR_RESP event */
  BT_SD_EV_ERROR_INVALID_SDP_VERSION           = 0x0001,
  BT_SD_EV_ERROR_INVALID_SERVICE_RECORD_HANDLE = 0x0002,
  BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX        = 0x0003,
  BT_SD_EV_ERROR_INVALID_PDU_SIZE              = 0x0004,
  BT_SD_EV_ERROR_INVALID_CONTINUATION_STATE    = 0x0005,
  BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE         = 0x0006,

/* Qualcomm-specific error code for the BT_EV_SD_ERROR_RESP event */
  BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX       = 0x0020,
  BT_SD_EV_ERROR_MAX_SDP_CONN_EXCEEDED,
  BT_SD_EV_ERROR_REQUEST_PENDING,
  BT_SD_EV_ERROR_CONNECTION_FAILED,
  BT_SD_EV_ERROR_SERVICE_NOT_FOUND,
  BT_SD_EV_ERROR_CONNECTION_TERMINATED,
  BT_SD_EV_ERROR_UNKNOWN,
  BT_SD_EV_ERROR_LAST
} bt_sd_error_code_enum_type;



/* Definition for a Service Registration UUID list */
typedef BT_PACKED struct
{
  uint8                          num_uuid;
  bt_sd_uuid_type                uuid[ BT_SD_MAX_NUM_OF_UUID ];
  uint8                          num_uuid128;
  bt_sd_uuid128_type             uuid128[ BT_SD_MAX_NUM_OF_UUID128 ];
} bt_sd_uuid_list_type;

typedef BT_PACKED struct
{ 
  uint8              num_val;
  uint8              val[ BT_SD_MAX_NUM_OF_UINT ];
} bt_sd_uint8_list_type;

/* Definition for a Service Search Pattern UUID list */
typedef struct
{
  uint8               num_uuid;
  bt_sd_uuid_type     uuid[ BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID ];
  
  uint8               num_uuid128;
  bt_sd_uuid128_type  uuid128[ BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID128 ];

} bt_sd_srch_pat_uuid_list_type;
#ifdef FEATURE_HTORPC_METACOMMENTS 
/*~ FIELD bt_sd_srch_pat_uuid_list_type.uuid 
    VARRAY BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID 
    LENGTH bt_sd_srch_pat_uuid_list_type.num_uuid */
/*~ FIELD bt_sd_srch_pat_uuid_list_type.uuid128 
    VARRAY BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID128 
    LENGTH bt_sd_srch_pat_uuid_list_type.num_uuid128 */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Definition for an attribute ID list */
typedef struct
{
  uint16           num_attr_id;
  struct bt_sd_srch_pat_attr_id_list_attr_id
  {
    boolean        is_range;
    uint32         value;
  } attr_id[ BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID ];
} bt_sd_srch_pat_attr_id_list_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD bt_sd_srch_pat_attr_id_list_type.attr_id 
    VARRAY BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID 
    LENGTH bt_sd_srch_pat_attr_id_list_type.num_attr_id */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*-------------------------------------------------------------------------*/
/*  RFCOMM specific declarations.                                          */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct  /*  RFCOMM channel configuration.  */
{
  boolean  pn_negot_done;
  boolean  rpn_negot_done;
  uint16   max_frame_size;
  uint8    priority;
  boolean  cbfc_enabled;
  uint16   cbfc_local_credits;
  uint16   cbfc_far_credits;
  uint8    baudrate;
  uint8    format;     /*  Corresponds to value3 octet in RPN.  */
  uint8    flow_ctrl;
  uint8    xon_char;
  uint8    xoff_char;
} bt_rc_config_type;


/*  All valid server channel numbers defined below  */
/*  must be between RC_SERV_CHAN_MIN_V and          */
/*  RC_SERV_CHAN_MAX_V defined in btmsgi.h          */

typedef enum
{
  BT_RC_SCN_NOT_SPECIFIED     = 0x00,  /*  Makes SPP code automatically  */
                                       /*    generate an RFCOMM server   */
                                       /*    channel number between      */
                                       /*    values defined below.       */
#ifdef FEATURE_BT_AG
  BT_RC_SCN_AG_HS             = 0x03,
  BT_RC_SCN_AG_HF             = 0x04,
#endif
  BT_RC_SCN_NA                = 0x08,
#ifdef FEATURE_BT_TEST_RC_2_SIO
#error code not present
#endif
  BT_RC_SCN_SPP_MIN_AUTO_V    = 0x10,  /*  These values (inclusive) are  */
  BT_RC_SCN_SPP_MAX_AUTO_V    = 0x18,  /*    reserved for SPP ports.     */
  BT_RC_SCN_LAST
} bt_rc_scn_enum_type;

typedef enum
{
  BT_RCLE_NONE,
  BT_RCLE_OVERRUN_ERROR,
  BT_RCLE_PARITY_ERROR,
  BT_RCLE_FRAMING_ERROR
} bt_rc_line_error_type;


/*  Modem Status Command (MSC) message V.24 signals definitions.  */

#define RCMCCM_MSC_V24_RTC_ASSERTED_B 0x04
#define RCMCCM_MSC_V24_RTR_ASSERTED_B 0x08
#define RCMCCM_MSC_V24_IC_ASSERTED_B  0x40
#define RCMCCM_MSC_V24_DV_ASSERTED_B  0x80


/*  Remote Port Negotiation (RPN) message definitions.  */

/*  Baud Rate field definitions.  */

#define RCMCCM_RPN_BR_2400_BPS_V   0x00
#define RCMCCM_RPN_BR_4800_BPS_V   0x01
#define RCMCCM_RPN_BR_7200_BPS_V   0x02
#define RCMCCM_RPN_BR_9600_BPS_V   0x03
#define RCMCCM_RPN_BR_19200_BPS_V  0x04
#define RCMCCM_RPN_BR_38400_BPS_V  0x05
#define RCMCCM_RPN_BR_57600_BPS_V  0x06
#define RCMCCM_RPN_BR_115200_BPS_V 0x07
#define RCMCCM_RPN_BR_230400_BPS_V 0x08

#define RCMCCM_RPN_BR_DEFAULT_V RCMCCM_RPN_BR_9600_BPS_V
#define RCMCCM_RPN_BR_MIN_V     RCMCCM_RPN_BR_2400_BPS_V
#define RCMCCM_RPN_BR_MAX_V     RCMCCM_RPN_BR_230400_BPS_V


/*  Serial data format definitions (value3 octet).  */

#define RCMCCM_RPN_FMT_D_M        0x03  /*  Data bits (default = 8).  */
#define RCMCCM_RPN_FMT_D_5_V      0x00
#define RCMCCM_RPN_FMT_D_6_V      0x02
#define RCMCCM_RPN_FMT_D_7_V      0x01
#define RCMCCM_RPN_FMT_D_8_V      0x03

#define RCMCCM_RPN_FMT_S_1P5_B    0x04  /*  Stop Bits:                */
                                        /*    0 = 1 (default)         */
                                        /*    1 = 1.5                 */

#define RCMCCM_RPN_FMT_P_B        0x08  /*  Parity:                   */
                                        /*    0 = no (default),       */
                                        /*    1 = yes                 */

#define RCMCCM_RPN_FMT_PT_M       0x30  /*  Parity Type.              */
#define RCMCCM_RPN_FMT_PT_ODD_V   0x00
#define RCMCCM_RPN_FMT_PT_EVEN_V  0x20
#define RCMCCM_RPN_FMT_PT_MARK_V  0x10
#define RCMCCM_RPN_FMT_PT_SPACE_V 0x30

#define RCMCCM_RPN_FMT_M (                 \
                 RCMCCM_RPN_FMT_D_M     |  \
                 RCMCCM_RPN_FMT_S_1P5_B |  \
                 RCMCCM_RPN_FMT_P_B     |  \
                 RCMCCM_RPN_FMT_PT_M          )

/*  Default format is 8 data bits, 1 stop bit, no parity.  */
#define RCMCCM_RPN_FMT_DEFAULT_V RCMCCM_RPN_FMT_D_8_V


/*  Flow Control definitions.  */

#define RCMCCM_RPN_FC_M         0x3F
#define RCMCCM_RPN_FC_XO_IN_B   0x01  /*  XON/XOFF on input.   */
#define RCMCCM_RPN_FC_XO_OUT_B  0x02  /*  XON/XOFF on output.  */
#define RCMCCM_RPN_FC_RTR_IN_B  0x04  /*  RTR on input.        */
#define RCMCCM_RPN_FC_RTR_OUT_B 0x08  /*  RTR on output.       */
#define RCMCCM_RPN_FC_RTC_IN_B  0x10  /*  RTC on input.        */
#define RCMCCM_RPN_FC_RTC_OUT_B 0x20  /*  RTC on output.       */

#define RCMCCM_RPN_FC_DEFAULT_V   0x00  /*  None.  */
#define RCMCCM_RPN_XON_DEFAULT_V  0x11  /*  DC1    */
#define RCMCCM_RPN_XOFF_DEFAULT_V 0x13  /*  DC3    */


/*-------------------------------------------------------------------------*/
/*  L2CAP specific definitions.                                            */
/*-------------------------------------------------------------------------*/

typedef enum
{
  BT_QOS_NO_TRAFFIC,
  BT_QOS_BEST_EFFORT,
  BT_QOS_GUARANTEED
} bt_qos_service_type;

typedef BT_PACKED struct
{
  byte                 flags; // reserved; must be 0
  bt_qos_service_type  service_type;
  dword                token_rate;
  dword                token_bucket_size;
  dword                peak_bandwidth;
  dword                latency;
  dword                delay_variation;
} bt_l2_qos_type;




/*-------------------------------------------------------------------------*/
/*  RM specific definitions.                                               */
/*-------------------------------------------------------------------------*/

#define BT_RM_MAX_DEVICES 30
#define BT_RM_SSP_PASSKEY_LEN  7 // 6-digit + 1 NULL terminating char

typedef enum
{
  BT_RMDR_USER_ENDED      = BT_BE_USER_ENDED_CONNECTION,
  BT_RMDR_LOW_RESOURCES   = BT_BE_LOW_RESOURCES,
  BT_RMDR_POWER_GOING_OFF   = BT_BE_POWER_GOING_OFF,
  BT_RMDR_UNSPECIFIED_ERROR = BT_BE_UNSPECIFIED_ERROR,
  BT_RMDR_AUTH_FAILED = BT_BE_AUTHENTICATION_FAILURE
} bt_rm_disc_reason_type;

typedef enum
{
  BT_RMRJ_NO_RESOURCE      = BT_BE_CONN_REJECTED_NO_RESOURCE,
  BT_RMRJ_SECURITY_FAILURE = BT_BE_CONN_REJECTED_SECURITY_FAILURE,
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
  BT_RMRJ_PERSONAL_DEVICE  = BT_BE_CONN_REJECTED_PERSONAL_DEVICE
#else /* BT 1.2 */
  BT_RMRJ_BD_ADDR          = BT_BE_CONN_REJECTED_BD_ADDR
#endif
} bt_rm_rej_reason_type;


typedef enum
{
  BT_RM_ENH_ENC_OK,
  BT_RM_ENH_ENC_LOCAL,
  BT_RM_ENH_ENC_OFF,
  BT_RM_ENH_ENC_STARTED,
  BT_RM_ENH_ENC_DISABLING,
  BT_RM_ENH_ENC_AUTHENTICATING,
  BT_RM_ENH_ENC_AUTHENTICATING_ONLY
} bt_rm_enh_enc_type;


typedef enum
{
  BT_RM_DISCOVERABLE_NOT,      /*  Order is significant.  */
  BT_RM_DISCOVERABLE_GENERAL,
  BT_RM_DISCOVERABLE_LIMITED
} bt_rm_discoverable_type;


/*  The enumerated type below represents the trade-off  */
/*  between Average Connection Time (ACT) versus Power  */
/*  consumption that must be made when the RM is set    */
/*  to connectable and/or discoverable which makes the  */
/*  the BT receiver being scanning.                     */

typedef enum
{
  BT_RM_AVP_AUTOMATIC,
  BT_RM_AVP_SMALLEST_ACT_MOST_POWER,
  BT_RM_AVP_LARGER_ACT_LESS_POWER,
  BT_RM_AVP_LARGEST_ACT_LEAST_POWER
} bt_rm_act_vs_pwr_type;

typedef enum
{
  BT_RM_IOC_DISPLAY_ONLY,
  BT_RM_IOC_DISPLAY_YES_NO,
  BT_RM_IOC_KEYBOARD_ONLY,
  BT_RM_IOC_NO_IO
} bt_rm_io_cap_type;

typedef enum
{
  BT_RM_AB_DONT_CARE,
  BT_RM_AB_BOND,
  BT_RM_AB_MITM,
  BT_RM_AB_MITM_BOND
} bt_rm_authorize_bond_type;

typedef enum
{
  BT_RM_LKT_NONE,
  BT_RM_LKT_GENERAL,
  BT_RM_LKT_AUTH_GENERAL,
  BT_RM_LKT_DEDICATED,
  BT_RM_LKT_AUTH_DEDICATED,
  BT_RM_LKT_DEBUG,
  BT_RM_LKT_COMBINATION,
  BT_RM_LKT_UNBONDED,
  BT_RM_LKT_AUTH_UNBONDED,
  BT_RM_LKT_UNIT,
  BT_RM_LKT_MAX
} bt_rm_lk_status_type;

typedef enum
{
  BT_RM_KPN_STARTED,
  BT_RM_KPN_DIGIT_ENTERED,
  BT_RM_KPN_DIGIT_ERASED,
  BT_RM_KPN_CLEARED,
  BT_RM_KPN_COMPLETED
} bt_rm_keypress_notification_type;

typedef enum
{
  BT_RM_AR_NO_MITM_NO_BONDING,
  BT_RM_AR_MITM_NO_BONDING,
  BT_RM_AR_NO_MITM_DEDICATED_BONDING,
  BT_RM_AR_MITM_DEDICATED_BONDING,
  BT_RM_AR_NO_MITM_GENERAL_BONDING,
  BT_RM_AR_MITM_GENERAL_BONDING
} bt_rm_auth_req_type;

/*  Used to monitor driver inactivity (swap-time)  */
/*  and turn off the clocks                        */
#define BT_CLOCKS_MONITOR_TIMEOUT_MS 5000

/*-------------------------------------------------------------------------*/
/*  Command and Event definitions.                                         */
/*-------------------------------------------------------------------------*/
#define BT_EC_EV_MAX_RANGE_MASK 0x0FFF



#define BT_CE_GROUP( ce_type, base ) ((ce_type & 0xF000) ==  \
                                       BT_CMD_EV_##base##_BASE)

typedef enum
{
  BT_EVR_GN_SUCCESS =                       BT_CMD_EV_GN_BASE,

#ifdef FEATURE_BT_AG
  BT_EVR_AG_ENABLE_FAILED =                 BT_CMD_EV_AG_BASE,
  BT_EVR_AG_DISABLE_RXD,
  BT_EVR_AG_CONN_FAILED,
  BT_EVR_AG_DISC_RXD,
  BT_EVR_AG_HS_SC_CONN_LOST,
  BT_EVR_AG_HS_DISCONN_REQ_RXD,
#endif /* FEATURE_BT_AG */

  BT_EVR_BA_SUCCESS =                       BT_CMD_EV_BA_BASE,
  BT_EVR_BA_PAIRING_FAILED,
  BT_EVR_BA_DATABASE_ERROR,
  BT_EVR_BA_INTERNAL_ERROR,
  BT_EVR_BA_DEVICE_NOT_BONDED,

  BT_EVR_SD_GET_DEVICE_NAME_FAILED =        BT_CMD_EV_SD_BASE,
  BT_EVR_SD_DEVICE_DISCOVERY_FAILED,
  BT_EVR_SDP_START =                        BT_CMD_EV_SDP_BASE,
  BT_EVR_SDP_END =                          (BT_CMD_EV_SDP_BASE +
                                               BT_SD_EV_ERROR_LAST),

  BT_EVR_SPP_BAD_OPEN_PARAMS =              BT_CMD_EV_SP_BASE,
  BT_EVR_SPP_OUT_OF_SERVER_CHANNELS,
  BT_EVR_SPP_SERVER_CHANNEL_IN_USE,
  BT_EVR_SPP_BAD_SERVER_CHANNEL,
  BT_EVR_SPP_SDP_REGISTRATION_FAILED,
  BT_EVR_SPP_RFCOMM_REGISTRATION_FAILED,
  BT_EVR_SPP_SDP_TIMEOUT,
  BT_EVR_SPP_LOCAL_DISCONNECT,

  BT_EVR_RC_SETUP_NEGOTIATIONS_FAILED =     BT_CMD_EV_RC_BASE,
  BT_EVR_RC_REMOTE_REJECTS_CONNECTION,
  BT_EVR_RC_NORMAL_DISCONNECT,
  BT_EVR_RC_REMOTE_SIGNALING_ERROR,

  BT_EVR_L2_CONFIG_PARAMS_NOT_AGREEABLE =   BT_CMD_EV_L2_BASE,
  BT_EVR_L2_LOCAL_REJECT_CONNECTION,
  BT_EVR_L2_REMOTE_REJECT_CONNECTION,
  BT_EVR_L2_PAGE_FAILED,
  BT_EVR_L2_LINK_LOST,
  BT_EVR_L2_AUTHENTICATION_FAILED,
  BT_EVR_L2_UNSPECIFIED_ERROR,
  BT_EVR_L2_NORMAL_DISCONNECT,
  BT_EVR_L2_PING_SUCCESS,
  BT_EVR_L2_PING_FAILED,
  BT_EVR_L2_CONFIG_SUCCESS,

  BT_EVR_RM_BOND_NO_KEY =                   BT_CMD_EV_RM_BASE,
  BT_EVR_RM_NO_CONNECTION,
  BT_EVR_RM_NO_RESOURCES,
  BT_EVR_RM_SECURITY_NOT_MET,
  BT_EVR_RM_RADIO_OFF,
  BT_EVR_RM_BAD_PARAM,
  BT_EVR_RM_UNEXP_COMMAND,

  BT_EVR_HC_PAGE_FAILED =                   BT_CMD_EV_HC_BASE,
  BT_EVR_HC_ACL_LINK_SETUP_FAILED,
  BT_EVR_HC_LINK_LOST,
  BT_EVR_HC_AUTHENTICATION_FAILED,
  BT_EVR_HC_NORMAL_DISCONNECT,

  BT_EVR_HCI_START =                        BT_CMD_EV_HCI_BASE,
  BT_EVR_HCI_END =                          (BT_CMD_EV_HCI_BASE +
                                               BT_BE_LAST)
} bt_event_reason_type;


typedef enum  /*  For use with EC commands.  */
{
  BT_EC_ES_ALL,
  BT_EC_ES_BONDING_APP,
  BT_EC_ES_SERVICE_DISC,
  BT_EC_ES_NETWORK_ACCESS,
  BT_EC_ES_RFCOMM,
  BT_EC_ES_L2CAP,
  BT_EC_ES_SCO_LINK,
  BT_EC_ES_RADIO_MANAGER,
  BT_EC_ES_HCI,
  BT_EC_ES_CUSTOM
#ifdef FEATURE_BT_AG
  , BT_EC_ES_AUDIO_GATEWAY
#endif
} bt_event_set_type;


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_task

DESCRIPTION
  BT Driver task for use by Main Control task only.

===========================================================================*/
extern void bt_task( dword ignored );


/*===========================================================================

FUNCTION
  bt_get_free_dsm_ptr

DESCRIPTION
  Retrieves a DSM pointer and sets the data_ptr to correct starting place
  for specified top layer of BT stack to be used.

RETURN VALUE
  Pointer to free DSM buffer.

===========================================================================*/
extern dsm_item_type* bt_get_free_dsm_ptr
(
  bt_top_layer_type  top_layer,
  uint16             max_payload_bytes
);

/*-------------------------------------------------------------------------*/
/*                      Network Access (NA) Commands                       */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_na_enable( bt_app_id_type bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_na_enable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_na_disable( bt_app_id_type bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_na_disable */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*-------------------------------------------------------------------------*/
/*                  Driver Configuration (DC) Commands                     */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_dc_set_hci_mode( bt_hci_mode_type );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_set_hci_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_hci_mode_type bt_cmd_dc_get_hci_mode( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_get_hci_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_dc_set_bd_addr(
                            bt_bd_addr_type* bd_addr_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_set_bd_addr */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern void bt_cmd_dc_get_bd_addr( bt_bd_addr_type* bd_addr_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_get_bd_addr */
/*~ PARAM OUT bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_dc_set_refclock_type( 
                            bt_soc_refclock_type *bt_refclock_type_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_set_refclock_type */
/*~ PARAM IN bt_refclock_type_ptr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern void bt_cmd_dc_get_refclock_type( bt_soc_refclock_type *bt_refclock_type_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_get_refclock_type */
/*~ PARAM OUT bt_refclock_type_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_dc_set_clock_sharing_type( 
                            bt_soc_clock_sharing_type *bt_clock_sharing_type_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_set_clock_sharing_type */
/*~ PARAM IN bt_clock_sharing_type_ptr */
#endif /* FEATURE_HTORPC_METACOMMENTS */        

extern void bt_cmd_dc_get_clock_sharing_type( 
                            bt_soc_clock_sharing_type *bt_clock_sharing_type_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_get_clock_sharing_type */
/*~ PARAM OUT bt_clock_sharing_type_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_dc_set_inband_sleep_type (
                            bt_soc_inband_sleep_type *bt_inband_sleep_type_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_set_inband_sleep_type */
/*~ PARAM IN bt_inband_sleep_type_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern void bt_cmd_dc_get_inband_sleep_type(
                            bt_soc_inband_sleep_type *bt_inband_sleep_type_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_get_inband_sleep_type */
/*~ PARAM OUT bt_inband_sleep_type_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_dc_set_soc_logging (
                            uint8 *bt_soc_logging_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_set_soc_logging */
/*~ PARAM IN bt_soc_logging_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern void bt_cmd_dc_get_soc_logging(
                            uint8 *bt_soc_logging_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_get_soc_logging */
/*~ PARAM OUT bt_soc_logging_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_dc_set_bt_2_1_lisbon_status (
                            uint8 *bt_soc_bt_2_1_lisbon_status_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_set_bt_2_1_lisbon_status */
/*~ PARAM IN bt_soc_bt_2_1_lisbon_status_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern void bt_cmd_dc_get_bt_2_1_lisbon_status(
                            uint8 *bt_soc_logging_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_get_bt_2_1_lisbon_status */
/*~ PARAM OUT bt_soc_bt_2_1_lisbon_status_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern boolean bt_cmd_dc_is_bt_on( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_dc_is_bt_on */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_FACTORY_TESTMODE
/* FTM command/response buffer */
#define BT_FTM_CMD_RSP_LEN                  1100 /* approx. 3-DH5 */

extern void bt_ftm_cmd( uint8* data, uint16 length );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_ftm_cmd */
/*~ PARAM data VARRAY LENGTH length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern void bt_cmd_dc_ftm_get_bt_wm(
                            dsm_watermark_type** ftm_to_bt_wm_ptr_ptr,
                            dsm_watermark_type** ftm_from_bt_wm_ptr_ptr );

#endif /* FEATURE_FACTORY_TESTMODE */

/*SOC version commands*/
extern void  bt_cmd_dc_set_soc_version(bt_qsoc_enum_type *bt_soc_version);
extern bt_qsoc_enum_type bt_cmd_dc_get_soc_version(void);


/*-------------------------------------------------------------------------*/
/*                      Event Control (EC) Commands                        */
/*-------------------------------------------------------------------------*/

extern bt_app_id_type bt_cmd_ec_get_app_id_and_register(
                        bt_ev_cb_func_type* ev_cb_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ec_get_app_id_and_register */
/*~ PARAM IN ev_cb_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ec_free_application_id(
                            bt_app_id_type bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_ec_free_application_id */
#endif /* FEATURE_HTORPC_METACOMMENTS */


extern bt_app_id_type bt_cmd_ec_get_application_id( void );

extern bt_cmd_status_type bt_cmd_ec_register_event_set(
                            bt_app_id_type      bt_app_id,
                            bt_ev_cb_func_type* ev_cb_ptr,
                            bt_event_set_type   ev_set_id,
                            bt_event_type       min_event,
                            bt_event_type       max_event );

extern bt_cmd_status_type bt_cmd_ec_unregister_event_set(
                            bt_app_id_type    bt_app_id,
                            bt_event_set_type ev_set_id,
                            bt_event_type     min_event,
                            bt_event_type     max_event );

extern bt_cmd_status_type bt_cmd_ec_reg_event_set_any_app_id(
                            bt_app_id_type      bt_app_id,
                            bt_ev_cb_func_type* ev_cb_ptr,
                            bt_event_set_type   ev_set_id,
                            bt_event_type       min_event,
                            bt_event_type       max_event );

extern bt_cmd_status_type bt_cmd_ec_unreg_event_set_any_app_id(
                            bt_app_id_type    bt_app_id,
                            bt_event_set_type ev_set_id,
                            bt_event_type     min_event,
                            bt_event_type     max_event );


#ifdef FEATURE_BT_AG
/*-------------------------------------------------------------------------*/
/*                      Audio Gateway (AG) Commands                        */
/*-------------------------------------------------------------------------*/

/* Temporarily for backwards compatibility */
#define bt_cmd_ag_ring_headset      bt_cmd_ag_ring_audio_device
#define bt_cmd_ag_set_hs_spkr_gain  bt_cmd_ag_set_ad_spkr_gain
#define bt_cmd_ag_set_hs_mic_gain   bt_cmd_ag_set_ad_mic_gain

#define BT_EV_AG_HEADSET_PICKUP      BT_EV_AG_BUTTON_PRESSED
#define BT_EV_AG_DISCONNECT_REQUEST  BT_EV_AG_BUTTON_PRESSED + 11

extern bt_cmd_status_type bt_cmd_ag_enable(
                            bt_app_id_type       bt_app_id,
                            bt_bd_addr_type*     bd_addr_ptr,
                            bt_audio_device_type audio_device );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_enable */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */


extern bt_cmd_status_type bt_cmd_ag_connect(
                            bt_app_id_type       bt_app_id,
                            bt_bd_addr_type*     bd_addr_ptr,
                            bt_audio_device_type audio_device );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_connect */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_config(
                            bt_app_id_type       bt_app_id,
                            bt_ag_idle_mode_type idle_mode,
                            uint16               idle_timeout );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_config */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_set_vr_capable( 
                            bt_app_id_type  bt_app_id,
                            boolean         vr_capable );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_set_vr_capable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_ring_audio_device(
                            bt_app_id_type bt_app_id,
                            uint16         max_rings,
                            uint16         ring_period );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_ring_audio_device */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_audio_connect(
                            bt_app_id_type bt_app_id,
                            uint16         max_time );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_ag_audio_connect */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_set_ad_spkr_gain(
                            bt_app_id_type bt_app_id,
                            uint8          ad_speaker_gain );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_ag_set_ad_spkr_gain */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_set_ad_mic_gain(
                            bt_app_id_type bt_app_id,
                            uint8          ad_microphone_gain );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_ag_set_ad_mic_gain */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_audio_disconnect(
                            bt_app_id_type bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_ag_audio_disconnect */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_disconnect(
                            bt_app_id_type bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_disconnect */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_disable(
                            bt_app_id_type bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_disable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_update_vr_state( 
                            bt_app_id_type bt_app_id,
                            boolean vr_activated );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_update_vr_state */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*  Indicates not to internally process specific  */
/*  AG AT commands from an audio device, but to   */
/*  pass them on via the external I/O instead.    */
typedef enum
{
  BT_AG_IC_NONE_M  = 0x00000000,  /*  No commands.                    */
  BT_AG_IC_CKPD_B  = 0x00000001,  /*  "AT+CKPD="                      */
  BT_AG_IC_VGM_B   = 0x00000002,  /*  "AT+VGM="                       */
  BT_AG_IC_VGS_B   = 0x00000004,  /*  "AT+VGS="                       */
  BT_AG_IC_A_B     = 0x00000008,  /*  "ATA"                           */
  BT_AG_IC_D_B     = 0x00000010,  /*  "ATD"                           */
  BT_AG_IC_BINP1_B = 0x00000020,  /*  "AT+BINP=1"                     */
  BT_AG_IC_BLDN_B  = 0x00000040,  /*  "AT+BLDN"                       */
  BT_AG_IC_BVRA_B  = 0x00000080,  /*  "AT+BVRA="                      */
  BT_AG_IC_CCWA_B  = 0x00000100,  /*  "AT+CCWA="                      */
  BT_AG_IC_CHUP_B  = 0x00000200,  /*  "AT+CHUP"                       */
  BT_AG_IC_CLIP_B  = 0x00000400,  /*  "AT+CLIP="                      */
  BT_AG_IC_NREC_B  = 0x00000800,  /*  "AT+NREC="                      */
  BT_AG_IC_VTS_B   = 0x00001000,  /*  "AT+VTS="                       */
  BT_AG_IC_BTRH_B  = 0x00040000,  /*  "AT+BTRH" read, set, and test.  */
#ifdef BT_AG_HF_EXTENSION
  BT_AG_IC_CPBS_B  = 0x00002000,  /*  "AT+CPBS" read, set, and test.  */
  BT_AG_IC_CPBR_B  = 0x00004000,  /*  "AT+CPBR" set and test.         */
  BT_AG_IC_CPBF_B  = 0x00008000,  /*  "AT+CPBF="                      */
  BT_AG_IC_CPBW_B  = 0x00010000,  /*  "AT+CPBW="                      */
  BT_AG_IC_CNUM_B  = 0x00020000,  /*  "AT+CNUM?"                      */
#endif /* BT_AG_HF_EXTENSION */
  BT_AG_IC_ALL_M   = 0x0007FFFF  /*  All commands.                   */
} bt_ag_int_cmd_type;

extern bt_cmd_status_type bt_cmd_ag_external_io_config(
                            bt_app_id_type      bt_app_id,
                            boolean             enable_external_io,
                            dsm_watermark_type* rx_wm_ptr,
                            dsm_watermark_type* tx_wm_ptr,
                            bt_ag_int_cmd_type  int_cmd_off_mask );

extern bt_cmd_status_type bt_cmd_ag_set_inband_ring(
                            bt_app_id_type bt_app_id,
                            boolean enabled );
#ifdef FEATURE_HTORPC_METACOMMENTS 
/*~ FUNCTION bt_cmd_ag_set_inband_ring */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_set_battchg_ind(
                            bt_app_id_type           bt_app_id,
                            bt_ag_battchg_level_type battchg );
#ifdef FEATURE_HTORPC_METACOMMENTS 
/*~ FUNCTION bt_cmd_ag_set_battchg_ind */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_get_battchg_ind(
                            bt_app_id_type            bt_app_id,
                            bt_ag_battchg_level_type* battchg_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_get_battchg_ind */
/*~ PARAM OUT battchg_ptr POINTER */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_set_rssi(
                            bt_app_id_type        bt_app_id,
                            bt_ag_rssi_level_type rssi );
#ifdef FEATURE_HTORPC_METACOMMENTS 
/*~ FUNCTION bt_cmd_ag_set_rssi */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_get_rssi(
                            bt_app_id_type         bt_app_id,
                            bt_ag_rssi_level_type* rssi_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_get_rssi */
/*~ PARAM OUT rssi_ptr POINTER */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_send_supported_rh(
                            bt_app_id_type  bt_app_id,
                            boolean         supported_rh[ BT_AG_RH_MAX ] );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_send_supported_rh */
/*~ PARAM IN supported_rh ARRAY BT_AG_RH_MAX */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_ag_send_current_rh(
                            bt_app_id_type        bt_app_id,
                            boolean               is_in_rh,
                            bt_ag_rh_state_type   current_rh );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_ag_send_current_rh */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef BT_AG_HF_EXTENSION
extern bt_cmd_status_type bt_cmd_ag_send_pb_list(
                            bt_app_id_type      bt_app_id,
                            uint8               num_supported_phonebooks,
                            bt_pap_device_type  *supported_phonebook );

extern bt_cmd_status_type bt_cmd_ag_send_pb_select_result(
                            bt_app_id_type      bt_app_id,
                            bt_pap_device_type  selected_phonebook,
                            bt_pap_return_type  result );

extern bt_cmd_status_type bt_cmd_ag_send_pb_info(
                            bt_app_id_type      bt_app_id,
                            bt_pap_device_type  selected_phonebook,
                            uint16              used_recs,
                            uint16              total_recs );

extern bt_cmd_status_type bt_cmd_ag_send_pb_entries_info(
                            bt_app_id_type      bt_app_id,
                            bt_pap_device_type  selected_phonebook,
                            bt_pap_return_type  result,
                            uint16              start,
                            uint16              end,
                            uint16              max_num_len,
                            uint16              max_text_len );

extern bt_cmd_status_type bt_cmd_ag_send_pb_read_result(
                            bt_app_id_type      bt_app_id,
                            bt_event_type       event,
                            bt_pap_device_type  pb,
                            bt_pap_return_type  result,
                            bt_pap_record_type  *rec_ptr );

extern bt_cmd_status_type bt_cmd_ag_send_pb_write_result(
                            bt_app_id_type      bt_app_id,
                            bt_pap_device_type  pb,
                            bt_pap_return_type  result );

extern bt_cmd_status_type bt_cmd_ag_send_subscriber_numbers(
                            bt_app_id_type              bt_app_id,
                            uint8                       subs_num_cnt,
                            bt_pap_subsc_num_info_type  *sni );
#endif  /* BT_AG_HF_EXTENSION */

/*-------------------------------------------------------------------------*/
/*                  Audio Gateway Protocol (AG) Events                     */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_ev_ag_enabled_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_audio_device_type  audio_device;
} bt_ev_ag_connected_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  event_reason;
} bt_ev_ag_connection_failed_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  event_reason;
} bt_ev_ag_disconnected_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  event_reason;
} bt_ev_ag_ring_failed_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_ev_ag_button_pressed_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_ev_ag_audio_connected_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  event_reason;
} bt_ev_ag_audio_disconnected_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  event_reason;
} bt_ev_ag_audio_connect_failed_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  uint8            ad_speaker_gain;
} bt_ev_ag_ad_spkr_gain_report_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  uint8            ad_microphone_gain;
} bt_ev_ag_ad_mic_gain_report_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  event_reason;
} bt_ev_ag_disabled_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  boolean           slc_up;
} bt_ev_ag_slc_changed_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
} bt_ev_ag_dev_pickup_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
} bt_ev_ag_dev_hangup_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  boolean           vr_enabled;
} bt_ev_ag_vr_state_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  uint8             phone_num[ 64 ];  /* TBD - use a defined numbers */
} bt_ev_ag_dev_dial_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  uint8             mem_entry[3];  /* 0-99 */
} bt_ev_ag_dev_mem_dial_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
} bt_ev_ag_dev_redial_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  uint8             dtmf_char;
} bt_ev_ag_dev_send_dtmf_type;

typedef BT_PACKED struct
{
  bt_ag_cmd_sub_type   sub_type;
  bt_ag_rh_state_type  rh_state; /* valid if action==SET */
} bt_ev_ag_btrh_type;

#ifdef BT_AG_HF_EXTENSION
typedef BT_PACKED struct
{
  bt_pap_cmd_sub_type sub_type;
  bt_pap_device_type  selected_pb;  /* not used if action==TEST */
} bt_ev_ag_pap_cpbs_type;

typedef BT_PACKED struct
{
  bt_pap_device_type  pb;
  int16               mem_entry; /* -1 indicates entry range inquiry */
} bt_ev_ag_pap_cpbr_type;

typedef BT_PACKED struct
{
  uint16            find_text[ BT_PAP_MAX_TEXT_LENGTH + 1 ];
} bt_ev_ag_pap_cpbf_type;

typedef BT_PACKED struct
{
  boolean           erase_entry;
  int16             mem_entry; /* -1 indicates next free entry */
  uint8             phone_num[ BT_PAP_MAX_NUM_LENGTH + 1 ];
  uint8             type; /* type of address octet in integer format */
  uint16            text[ BT_PAP_MAX_TEXT_LENGTH + 1 ];
} bt_ev_ag_pap_cpbw_type;

typedef bt_ev_ag_btrh_type bt_ev_ag_pap_btrh_type;

#endif /* BT_AG_HF_EXTENSION */
#endif /* FEATURE_BT_AG */


/*-------------------------------------------------------------------------*/
/*               Service Discovery Protocol (SDP) Commands                 */
/*-------------------------------------------------------------------------*/

typedef enum
{
  BT_SD_SERVICE_DISCOVERABLE_MODE_NONE,     /*  Order is significant.  */
  BT_SD_SERVICE_DISCOVERABLE_MODE_GENERAL,
  BT_SD_SERVICE_DISCOVERABLE_MODE_LIMITED
} bt_sd_service_discoverable_mode_type;

extern bt_cmd_status_type bt_cmd_sd_set_service_discoverable_mode(
  bt_app_id_type                        bt_app_id,
  bt_sd_service_discoverable_mode_type  srv_discv_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_set_service_discoverable_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_get_discoverable_mode(
  bt_app_id_type                        bt_app_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_get_discoverable_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_discover_devices(
  bt_app_id_type                     bt_app_id,
  uint16                             service_class_bitmap,
  bt_bd_addr_type*                   bd_addr_ptr,
  uint16                             max_num_response
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_discover_devices */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_discover_devices_ext(
  bt_app_id_type                     bt_app_id,
  uint16                             service_class_bitmap,
  bt_bd_addr_type*                   bd_addr_ptr,
  uint16                             max_num_response,
  uint8                              duration_secs
);

extern bt_cmd_status_type bt_cmd_sd_start_device_discovery(
  bt_app_id_type                     bt_app_id,
  uint16                             service_class_bitmap,
  uint16                             max_num_response
);

extern bt_cmd_status_type bt_cmd_sd_stop_device_discovery(
  bt_app_id_type                     bt_app_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_stop_device_discovery */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_get_device_name(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_get_device_name */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_get_device_name_cancel(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_get_device_name_cancel */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_send_service_search_req(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  bt_sd_srch_pat_uuid_list_type*     uuid_list,
  uint16                             max_srv_rec_count
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_send_service_search_req */
/*~ PARAM IN bd_addr_ptr POINTER */
/*~ PARAM IN uuid_list POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_send_service_attribute_req(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  uint32                             srv_rec_handle,
  bt_sd_srch_pat_attr_id_list_type*  attr_id_list,
  uint16                             max_attr_byte_count
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_send_service_attribute_req */
/*~ PARAM IN  bd_addr_ptr POINTER */
/*~ PARAM IN attr_id_list POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_search_service(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  bt_sd_srch_pat_uuid_list_type*     uuid_list,
  bt_sd_srch_pat_attr_id_list_type*  attr_id_list,
  uint16                             max_attr_byte_count
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_search_service */
/*~ PARAM IN bd_addr_ptr POINTER */
/*~ PARAM IN uuid_list POINTER */
/*~ PARAM IN attr_id_list POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_cancel_idle_timer(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr
);

extern bt_cmd_status_type bt_cmd_sd_stop_service_search(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr
);

extern bt_cmd_status_type bt_cmd_sd_search_service_ext(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  bt_sd_srch_pat_uuid_list_type*     uuid_list,
  bt_sd_srch_pat_attr_id_list_type*  attr_id_list,
  uint16                             max_attr_byte_count,
  boolean                            generate_xml
);

extern bt_cmd_status_type bt_cmd_sd_get_server_channel_number(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  uint16                             service_class
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_get_server_channel_number */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_get_secondary_server_channel_number(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  uint16                             service_class
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_get_secondary_server_channel_number */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_update_service_record(
  bt_app_id_type                     bt_app_id,
  bt_sd_uuid_type                    uuid,
  uint8                              scn,
  char*                              service_name_str_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_update_service_record */
/*~ PARAM IN service_name_str_ptr STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_update_srv_rec_by_scn(
  bt_app_id_type                     bt_app_id,
  bt_sd_uuid_type                    uuid,
  uint8                              scn,
  uint8                              new_scn,
  char*                              service_name_str_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_update_srv_rec_by_scn */
/*~ PARAM IN service_name_str_ptr STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_update_srv_rec_by_psm(
  bt_app_id_type                     bt_app_id,
  bt_sd_uuid_type                    uuid,
  uint16                             psm,
  uint16                             new_psm,
  char*                              service_name_str_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_update_srv_rec_by_psm */
/*~ PARAM IN service_name_str_ptr STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_update_srv_name(
  bt_app_id_type                     bt_app_id,
  bt_sd_uuid_type                    uuid,
  char*                              service_name_str_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_update_srv_name */
/*~ PARAM IN service_name_str_ptr STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_register_service(
  bt_app_id_type                     bt_app_id,
  bt_sd_uuid_type                    uuid,
  uint16                             ver,
  uint8                              scn,
  char*                              service_name_str_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_register_service */
/*~ PARAM IN service_name_str_ptr STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_register_custom_service(
  bt_app_id_type                     bt_app_id,
  const bt_sd_uuid_list_type*        uuid_list_ptr,
  uint16                             ver,
  uint8                              scn,
  uint16                             psm,
  char*                              service_name_str_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_register_custom_service */
/*~ PARAM IN uuid_list_ptr POINTER */
/*~ PARAM IN service_name_str_ptr STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_register_custom_service_ext
(
  bt_app_id_type               bt_app_id,
  const bt_sd_uuid_list_type*  uuid_list_ptr,
  uint16                       ver,
  uint8                        scn,
  uint16                       psm,
  boolean                      connectable,
  char*                        service_name_str_ptr
);

extern bt_cmd_status_type bt_cmd_sd_register_serv_ext(
  bt_app_id_type                     bt_app_id,
  bt_sd_uuid_type                    uuid,
  uint16                             ver,
  uint8                              scn,
  uint16                             psm,
  boolean                            connectable,
  char*                              service_name_str_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_register_serv_ext */
/*~ PARAM IN service_name_str_ptr STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_unregister_srv_by_scn(
  bt_app_id_type                     bt_app_id,
  bt_sd_uuid_type                    uuid,
  uint8                              scn
);
#ifdef FEATURE_HTORPC_METACOMMENTS 
/*~ FUNCTION bt_cmd_sd_unregister_srv_by_scn */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_unregister_srv_by_psm(
  bt_app_id_type                     bt_app_id,
  bt_sd_uuid_type                    uuid,
  uint16                             psm
);
#ifdef FEATURE_HTORPC_METACOMMENTS 
/*~ FUNCTION bt_cmd_sd_unregister_srv_by_psm */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_unregister_service(
  bt_app_id_type                     bt_app_id,
  bt_sd_uuid_type                    uuid
);
#ifdef FEATURE_HTORPC_METACOMMENTS 
/*~ FUNCTION bt_cmd_sd_unregister_service */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_sd_unregister_custom_service(
  bt_app_id_type                     bt_app_id,
  const bt_sd_uuid128_type*          uuid128_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_unregister_custom_service */
/*~ PARAM IN uuid128_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_BT_EXTPF
extern bt_cmd_status_type bt_cmd_sd_update_obex_profile_service_record(
  bt_app_id_type         bt_app_id,
  bt_sd_uuid_type        uuid,
  uint8                  scn,
  char*                  service_name_str_ptr,
  bt_sd_uuid128_type*    service_id_ptr,
  bt_sd_uint8_list_type* supported_formats_list_ptr,
  uint8                  supported_capabilities,
  uint16                 supported_features,
  uint32                 supported_functions,
  uint64                 total_image_capacity
);

extern bt_cmd_status_type bt_cmd_sd_custom_update_obex_profile_service_record(
  bt_app_id_type         bt_app_id,
  bt_sd_uuid128_type*    uuid128_ptr,
  uint8                  scn,
  char*                  service_name_str_ptr,
  bt_sd_uuid128_type*    service_id_ptr,
  bt_sd_uint8_list_type* supported_formats_list_ptr,
  uint8                  supported_capabilities,
  uint16                 supported_features,
  uint32                 supported_functions,
  uint64                 total_image_capacity
);

#endif /* FEATURE_BT_EXTPF */

extern bt_cmd_status_type bt_cmd_sd_search_service_uuid128(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  bt_sd_uuid128_type*                uuid_ptr,
  uint16                             attr_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_sd_search_service_uuid128 */
/*~ PARAM IN bd_addr_ptr POINTER */
/*~ PARAM IN uuid_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_BT_EXTPF_SML
extern bt_cmd_status_type bt_cmd_sd_register_syncml_service(
  bt_app_id_type   bt_app_id,
  boolean          client,  
  uint8            scn, 
  char*            service_name_str_ptr     
);

extern bt_cmd_status_type bt_cmd_sd_unregister_syncml_service(
  bt_app_id_type   bt_app_id,
  boolean          client  
); 
#endif

/*-------------------------------------------------------------------------*/
/*                Service Discovery Protocol (SDP) Events                  */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_bd_addr_type bd_addr;
  uint16          service_class;
  uint16          major_device_class;
  uint16          minor_device_class;
} bt_ev_sd_device_discovery_resp_type;

typedef BT_PACKED struct
{
  uint16               num_discvd_dev;    /* Number of discovered devices */
  bt_event_reason_type reason;
} bt_ev_sd_device_discovery_complete_type;

typedef BT_PACKED struct
{
  bt_event_reason_type reason;
  bt_bd_addr_type      bd_addr;
  char                 device_name_str[ BT_SD_MAX_DEVICE_NAME_LEN + 1 ];
} bt_ev_sd_dev_name_resp_type;

/*  Only for backward compatibility; to become obsolete.  */
typedef BT_PACKED struct
{
  bt_event_reason_type reason;
  bt_bd_addr_type      bd_addr;
  uint8                device_name[ BT_SD_MAX_DEVICE_NAME_LEN + 1 ];
} bt_ev_sd_device_name_resp_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  dsm_item_type*    dsm_ptr;
} bt_ev_sd_service_search_resp_xml_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type bd_addr;
  uint16          srv_rec_count;
  uint32          srv_rec_handle_list[ BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID ];
  uint16          attr_byte_count;
} bt_ev_sd_service_search_resp_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type bd_addr;
} bt_ev_sd_timeout_resp_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type            bd_addr;
  bt_sd_error_code_enum_type error_code;
} bt_ev_sd_error_resp_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type bd_addr;
  uint16          service_class;
  uint16          scn;
} bt_ev_sd_server_channel_number_resp_type;

typedef BT_PACKED struct
{
  bt_sd_service_discoverable_mode_type  discv_mode;
} bt_ev_sd_discoverable_mode_resp_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type bd_addr;
} bt_ev_sd_cancel_idle_timer_resp_type;


/*-------------------------------------------------------------------------*/
/*           Serial Port Profile (SPP) Specific Definitions                */
/*-------------------------------------------------------------------------*/

#define BT_SPP_MAX_PORTS 10


/*  SPP Status Definitions.  */

typedef enum
{
  BT_SPP_ST_CLOSED,
  BT_SPP_ST_OPENING,
  BT_SPP_ST_OPEN_ERROR,
  BT_SPP_ST_OPEN,
  BT_SPP_ST_CONNECTED,
  BT_SPP_ST_DISCONNECTING,
  BT_SPP_ST_DISCONNECTED
} bt_spp_state_type;

typedef BT_PACKED struct bt_spp_status_struct
{
  sio_stream_id_type    stream_id;
  bt_bd_addr_type       bd_addr;
  boolean               client_app;
  bt_sd_uuid_type       service_uuid;
  uint16                service_version;
  uint8                 rc_server_channel;
  bt_spp_state_type     spp_state;
  bt_event_reason_type  spp_reason;
} bt_spp_status_type;

/*  Prototype for BT SPP Status call back function.  */
typedef void (*bt_spp_status_fptr_type)( bt_spp_status_type* );


/*  SPP Configuration Definitions.  */

typedef BT_PACKED struct bt_spp_config_struct
{
  uint16   max_frame_size;
  uint8    priority;
  boolean  cb_flow_ctrl;
  uint8    baudrate;
  uint8    format;
  uint8    flow_ctrl;
  uint8    xon_char;
  uint8    xoff_char;
} bt_spp_config_type;

typedef BT_PACKED struct
{
  sio_stream_id_type  stream_id;
  bt_spp_config_type  spp_config;
} bt_spp_cfg_rpt_type;

/*  Prototype for BT SPP Configuration Report call back function.  */
typedef void (*bt_spp_cfg_rpt_fptr_type)( bt_spp_cfg_rpt_type* );

/*  Serial data format definitions (format field above).  These  */
/*  correspond to the value3 octet definitions in an RFCOMM      */
/*  Remote Port Negotiation message.  Use (format & *_M) to      */
/*  isolate field of interest, then compare to *_V values.       */
/*  Use (format & *_B) to test single bit definitions (i.e.,     */
/*  stop bits & parity).                                         */

#define BT_SPP_FMT_D_M        RCMCCM_RPN_FMT_D_M      /*  Data bits.    */
#define BT_SPP_FMT_D_5_V      RCMCCM_RPN_FMT_D_5_V
#define BT_SPP_FMT_D_6_V      RCMCCM_RPN_FMT_D_6_V
#define BT_SPP_FMT_D_7_V      RCMCCM_RPN_FMT_D_7_V
#define BT_SPP_FMT_D_8_V      RCMCCM_RPN_FMT_D_8_V

#define BT_SPP_FMT_P_B        RCMCCM_RPN_FMT_P_B      /*  Parity.       */

#define BT_SPP_FMT_PT_M       RCMCCM_RPN_FMT_PT_M     /*  Parity Type.  */
#define BT_SPP_FMT_PT_ODD_V   RCMCCM_RPN_FMT_PT_ODD_V
#define BT_SPP_FMT_PT_EVEN_V  RCMCCM_RPN_FMT_PT_EVEN_V
#define BT_SPP_FMT_PT_MARK_V  RCMCCM_RPN_FMT_PT_MARK_V
#define BT_SPP_FMT_PT_SPACE_V RCMCCM_RPN_FMT_PT_SPACE_V

#define BT_SPP_FMT_S_1P5_B    RCMCCM_RPN_FMT_S_1P5_B  /*  1.5 stop bits  */
                                                      /*  (otherwise 1)  */

/*  Flow Control definitions (flow_ctrl field above).  */
/*  Use (flow_ctrl & *_B) to test each possibility.    */
/*  That is, if (flow_ctrl & *_B) != 0, then yes.      */
/*  Note that these are information only as to the     */
/*  settings negotiated at the RFCOMM layer.  They     */
/*  are not actually implemented within RFCOMM.        */

#define BT_SPP_FC_XO_IN_B   RCMCCM_RPN_FC_XO_IN_B  /*  XON/XOFF on input.  */
#define BT_SPP_FC_XO_OUT_B  RCMCCM_RPN_FC_XO_OUT_B /*  XON/XOFF on output. */
#define BT_SPP_FC_RTR_IN_B  RCMCCM_RPN_FC_RTR_IN_B /*  RTR on input.       */
#define BT_SPP_FC_RTR_OUT_B RCMCCM_RPN_FC_RTR_OUT_B/*  RTR on output.      */
#define BT_SPP_FC_RTC_IN_B  RCMCCM_RPN_FC_RTC_IN_B /*  RTC on input.       */
#define BT_SPP_FC_RTC_OUT_B RCMCCM_RPN_FC_RTC_OUT_B/*  RTC on output.      */


/*  SPP Modem Status Definitions.  */

typedef enum
{
  BT_SPP_MS_NONE           = 0x0,
  BT_SPP_MS_RTC_ASSERTED_B = RCMCCM_MSC_V24_RTC_ASSERTED_B,
  BT_SPP_MS_RTR_ASSERTED_B = RCMCCM_MSC_V24_RTR_ASSERTED_B,
  BT_SPP_MS_IC_ASSERTED_B  = RCMCCM_MSC_V24_IC_ASSERTED_B,
  BT_SPP_MS_DV_ASSERTED_B  = RCMCCM_MSC_V24_DV_ASSERTED_B
} bt_spp_ms_enum_type;

typedef BT_PACKED struct
{
  sio_stream_id_type   stream_id;
  bt_spp_ms_enum_type  ms_changed_mask;
  bt_spp_ms_enum_type  modem_status;
  boolean              break_present;
  uint8                break_length;
  uint8                breaks;
} bt_spp_ms_type;

/*  Prototype for BT SPP Modem Status call back function.  */
typedef void (*bt_spp_ms_fptr_type)( bt_spp_ms_type* );


/*  SPP Line Error Definitions.  */

typedef enum
{
  BT_SPP_LE_NONE          = BT_RCLE_NONE,
  BT_SPP_LE_OVERRUN_ERROR = BT_RCLE_OVERRUN_ERROR,
  BT_SPP_LE_PARITY_ERROR  = BT_RCLE_PARITY_ERROR,
  BT_SPP_LE_FRAMING_ERROR = BT_RCLE_FRAMING_ERROR
} bt_spp_le_enum_type;

typedef BT_PACKED struct
{
  sio_stream_id_type   stream_id;
  bt_spp_le_enum_type  cur_line_error;
  uint8                overruns;
  uint8                parity_errors;
  uint8                framing_errors;
} bt_spp_le_type;

/*  Prototype for BT SPP Line Error call back function.  */
typedef void (*bt_spp_le_fptr_type)( bt_spp_le_type* );


/*  Additional data to open a Bluetooth port via sio_open().  */


/*  RFCOMM Server Channel Number (SCN) definitions for SPP.        */
/*                                                                 */
/*  For servers:                                                   */
/*    BT_RC_SCN_UNSPECIFIED makes SPP automatically generate an    */
/*    RFCOMM server channel number between BT_SPP_SCN_MIN_AUTO_V   */
/*    and BT_SPP_SCN_MAX_AUTO_V (inclusive).                       */
/*                                                                 */
/*  For clients:                                                   */
/*    BT_RC_SCN_UNSPECIFIED makes SPP connect to the UUID          */
/*    specified (service_uuid field) using the remote SDP server   */
/*    to determine the RFCOMM server channel number to use.        */
/*                                                                 */
/*  *_UNAVAILABLE_* values must not be used.                       */

typedef enum
{
  BT_SPP_SCN_UNSPECIFIED   = BT_RC_SCN_NOT_SPECIFIED,
#ifdef FEATURE_BT_AG
  BT_SPP_SCN_UNAVAILABLE_1 = BT_RC_SCN_AG_HS,
  BT_SPP_SCN_UNAVAILABLE_2 = BT_RC_SCN_AG_HF,
#endif
  BT_SPP_SCN_UNAVAILABLE_3 = BT_RC_SCN_NA,
  BT_SPP_SCN_MIN_AUTO_V    = BT_RC_SCN_SPP_MIN_AUTO_V,
  BT_SPP_SCN_MAX_AUTO_V    = BT_RC_SCN_SPP_MAX_AUTO_V
} bt_spp_scn_enum_type;

/*  Maximum Frame Size (MFS) is determined by driver with this value.  */
#define BT_SPP_MFS_AUTOMATIC 0

typedef BT_PACKED struct bt_spp_open_struct
{
  boolean                   client_app;
  bt_bd_addr_type*          bd_addr_ptr;
  bt_sd_uuid_type           service_uuid;
  uint16                    service_version;
  char*                     service_name_str_ptr;
  bt_spp_scn_enum_type      rc_server_channel;
  uint16                    max_frame_size;
  bt_spp_status_fptr_type   status_change_fptr;
  bt_spp_cfg_rpt_fptr_type  config_change_fptr;
  bt_spp_ms_fptr_type       modem_status_fptr;
  bt_spp_le_fptr_type       line_error_fptr;
} bt_spp_open_type;

extern sio_status_type bt_sio_cmd_open( sio_open_type* open_ptr );

extern void bt_sio_cmd_transmit( sio_stream_id_type stream_id,
                                 dsm_item_type*     dsm_ptr );

extern void bt_sio_cmd_ioctl( sio_stream_id_type    stream_id,
                              sio_ioctl_cmd_type    cmd,
                              sio_ioctl_param_type* param_ptr );

extern void bt_sio_cmd_close( sio_stream_id_type stream_id,
                              void (*close_func_ptr)(void) );

/* Event structures for SPP. These are not generated by SPP, but may be    */
/* filled in from callback information, and later used as events.          */

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_flush_tx_type;

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_enable_dtr_type;

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_tx_hi_wm_type;

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_tx_lo_wm_type;

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_tx_gne_type;

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_tx_empty_type;

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_rx_hi_wm_type;

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_rx_lo_wm_type;

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_rx_gne_type;

typedef BT_PACKED struct
{
  sio_stream_id_type stream_id;
} bt_ev_spp_rx_empty_type;

typedef BT_PACKED struct
{
  bt_spp_le_type line_error;
} bt_ev_spp_line_error_type;

typedef BT_PACKED struct
{
  bt_spp_ms_type modem_status;
} bt_ev_spp_modem_status_type;

typedef BT_PACKED struct
{
  bt_spp_cfg_rpt_type config_change;
} bt_ev_spp_config_change_type;

typedef BT_PACKED struct
{
  bt_spp_status_type status_change;
} bt_ev_spp_status_change_type;

/*-------------------------------------------------------------------------*/
/*                  Radio Manager (RM) Commands                            */
/*-------------------------------------------------------------------------*/

typedef uint16 bt_rm_handle_type;

typedef BT_PACKED struct
{
  uint8   lmp_version;
  uint16  manufacturer_name;
  uint16  lmp_subversion;
} bt_version_type;

typedef enum
{
  BT_LM_ACTIVE,
  BT_LM_HOLD,
  BT_LM_SNIFF,
  BT_LM_PARK,

  BT_LM_NONE              = 0x10,  /*  These values are for  */
  BT_LM_GOING_TO_HOLD,             /*    internal use only.  */
  BT_LM_GOING_TO_SNIFF,
  BT_LM_GOING_TO_PARK,
  BT_LM_UNSNIFFING,
  BT_LM_MASTER_UNPARKING,
  BT_LM_SLAVE_REQ_UNPARK,
  BT_LM_SNIFF_LOCK
} bt_link_mode_type;

typedef enum
{
  BT_LM_VC_AC_CVSD,
  BT_LM_VC_AC_MU_LAW,
  BT_LM_VC_AC_A_LAW
} bt_lm_voice_air_coding_type;

#ifdef FEATURE_BT_1_2

#define BT_LM_SCO_BANDWIDTH          8000   /* bytes per second */

typedef enum
{
  BT_LM_ESCO_RETX_NONE,
  BT_LM_ESCO_RETX_OPTIMUM_POWER,         /* At least one retx, with optimum power */
  BT_LM_ESCO_RETX_OPTIMUM_QUALITY,       /* At least one retx, with optimum esco link quality */
  BT_LM_ESCO_RETX_DONT_CARE        = 0xFF
} bt_lm_esco_retx_type;

typedef enum
{
  BT_LM_ESCO_LATENCY_5_MS      = 0x05,   /* 64Kbps using HVx or EV3 with 0 retx */
  BT_LM_ESCO_LATENCY_7_MS      = 0x07,   /* 64Kbps using HVx or EV3 with 1 retx */
  BT_LM_ESCO_LATENCY_10_MS     = 0x0A,
  BT_LM_ESCO_LATENCY_DONT_CARE = 0xFFFF
} bt_lm_esco_latency_type;

#endif /* FEATURE_BT_1_2 */

/* BT roles according to BT 1.0B HCI Spec (sect 4.5.8, 4.6.7, 4.6.8) */
typedef enum
{
  BT_ROLE_MASTER,
  BT_ROLE_SLAVE,
  BT_ROLE_NONE     /*  Not part of specification.  */
} bt_role_type;

/* Encryption mode according to BT 1.0A LMP && HCI Spec */
typedef enum
{
  BT_EM_DISABLED,        /*  No encryption.                              */
#ifdef FEATURE_BT_2_1
  BT_EM_ENABLED
#else
  BT_EM_POINT_TO_POINT,  /*  Encryption only on point-to-point packets.  */
  BT_EM_ALL              /*  Encryption on p2p and broadcast packets.    */
#endif /* FEATURE_BT_2_1 */
} bt_encrypt_mode_type;

typedef BT_PACKED struct  /*  Class of Device.  */
{
  uint8  cod_bytes[ 3 ];
} bt_cod_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/* ~ FIELD bt_cod_type.cod_bytes ARRAY 3 */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_BT_SOC
extern bt_cod_type           bt_class_of_device;
#endif /* FEATURE_BT_SOC */

typedef BT_PACKED struct
{
  uint8  lmp_features_bytes[ 8 ];
} bt_lmp_features_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/* ~ FIELD bt_lmp_features_type.lmp_features_bytes ARRAY 8 */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef enum
{
  BT_RM_SL_0_NONE,
  BT_RM_SL_1_LOW,
  BT_RM_SL_2_MEDIUM,
  BT_RM_SL_3_HIGH
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
} bt_sec_level_type;

typedef enum
{
  BT_SIM_NONE,
  BT_SIM_SDP_UUID,
  BT_SIM_RFCOMM_SCN,
  BT_SIM_L2CAP_PSM,
  BT_SIM_DEFAULT_RFCOMM,
  BT_SIM_DEFAULT_L2CAP,
  BT_SIM_MAX                           /*  BT driver internal use only.  */
} bt_serv_id_method_type;

typedef BT_PACKED struct
{
  bt_serv_id_method_type  service_id_method;
  bt_sd_uuid_type         sdp_uuid;
  uint8                   rfcomm_scn;
  uint16                  l2cap_psm;
} bt_service_id_type;


#define BT_TX_PWR_CLASS_1_DEVICE           20
#define BT_TX_PWR_CLASS_2_DEVICE           4
#define BT_MAX_PATHLOSS                    255

/* Outgoing EIR */
#define BT_EIR_DATA_TYPE_HDR_LEN           2

/* Max allowed in an outgoing EIR by the spec 
   Note: Over HCI the length is always 240 bytes */
#define BT_MAX_EIR_HCI_LEN                 240

/* Though 240 bytes are allowed in an EIR we limit this 
   to 121 bytes (size of a DM3 packet) */
#define BT_MAX_EIR_LEN                     121

/* If there are no UUIDs to be sent in an EIR the name 
   and the tx power should fit in a DM1 packet.
   Also the bt name is UTF-8 coded (ie 1-4 bytes/character) 
   The name in an EIR packet should take in an integer 
   number of characters 
*/
#define BT_MAX_EIR_NAME_LEN                12

/* BT_MAX_EIR_HCI_LEN - BT_EIR_DATA_TYPE_HDR_LEN */
#define BT_MAX_EIR_DATA_SIZE               119 
#define BT_MAX_EIR_MANUF_DATA_SIZE         119 
#define BT_MAX_EIR_UUID_DATA_SIZE          119 

/* Derived from BT_MAX_EIR_HCI_LEN */
#define BT_MAX_EIR_UUID16                  51  
#define BT_MAX_EIR_UUID32                  25
#define BT_MAX_EIR_UUID128                 6

/* Generic EIR definitions */
#define BT_NUM_UUID_TYPES                  3

/* EIR Data types */
#define BT_EIR_TYPE_FLAGS                   0x01
#define BT_EIR_TYPE_UUID16_INCOMPLETE       0x02
#define BT_EIR_TYPE_UUID16_COMPLETE         0x03
#define BT_EIR_TYPE_UUID32_INCOMPLETE       0x04
#define BT_EIR_TYPE_UUID32_COMPLETE         0x05
#define BT_EIR_TYPE_UUID128_INCOMPLETE      0x06
#define BT_EIR_TYPE_UUID128_COMPLETE        0x07
#define BT_EIR_TYPE_LOCAL_NAME_SHORTENED    0x08
#define BT_EIR_TYPE_LOCAL_NAME_COMPLETE     0x09
#define BT_EIR_TYPE_TX_POWER_LEVEL          0x0A
#define BT_EIR_TYPE_MANUF_SPECIFIC_DATA     0xFF

typedef enum
{
  BT_INQUIRY_MODE_INQ_RESP_STANDARD = 0x00,
  BT_INQUIRY_MODE_INQ_RESP_RSSI     = 0x01,
  BT_INQUIRY_MODE_INQ_RESP_EXTENDED = 0x02
} bt_inquiry_mode;

typedef enum
{
  BT_SOC_LMP_VERSION_1_0 = 0,
  BT_SOC_LMP_VERSION_1_1,     
  BT_SOC_LMP_VERSION_1_2,   
  BT_SOC_LMP_VERSION_2_0,   
  BT_SOC_LMP_VERSION_2_1  
} bt_soc_lmp_version;

typedef enum
{
  BT_SOC_HCI_VERSION_1_0 = 0,
  BT_SOC_HCI_VERSION_1_1,     
  BT_SOC_HCI_VERSION_1_2,   
  BT_SOC_HCI_VERSION_2_0,   
  BT_SOC_HCI_VERSION_2_1  
} bt_soc_hci_version;

typedef enum
{
  BT_UUID_16BIT_TYPE,
  BT_UUID_32BIT_TYPE,
  BT_UUID_128BIT_TYPE
} bt_uuid_type;

#define BT_EIR_DATA_RCVD_B           0x01
#define BT_EIR_NAME_CMPLT_B          0x02
#define BT_EIR_UUID16_LIST_CMPLT_B   0x04
#define BT_EIR_UUID32_LIST_CMPLT_B   0x08
#define BT_EIR_UUID128_LIST_CMPLT_B  0x10

/* This data gets passed onto the Brew layer as part of device read */
/* Note: This does not contain the full list of UUIDs or the complete 
   manufacturer data but just the sizes. The Brew layer/app can read the data given 
   the sizes */
typedef BT_PACKED struct
{
  uint8        eir_flag;
               /* bit 0: EIR data valid    0x01
                  bit 1: Name complete     0x02
                  bit 2: UUID 16 complete  0x04
                  bit 3: UUID 32 complete  0x08
                  bit 4: UUID 128 complete 0x10 */ 
  //boolean      is_name_complete;
  //uint8        uuid_complete_bitmap;
  uint8        num_uuids[ BT_NUM_UUID_TYPES ];
  uint8        device_ranking;
  uint8        manuf_data_size;
} bt_device_eir_type;

typedef BT_PACKED struct
{
  boolean               valid;
  bt_bd_addr_type       bd_addr;
  bt_cod_type           class_of_device;
  bt_lmp_features_type  lmp_features;
  bt_version_type       version;
  bt_security_type      security;
  boolean               bonded;
  bt_hc_link_key_type   link_key_type;
  uint16                service_class;
  uint16                major_dev_class;
  uint16                minor_dev_class;
  char                  name_str[ BT_SD_MAX_DEVICE_NAME_LEN + 1 ];
  char                  nick_name_str[ BT_MAX_REMOTE_NICK_NAME_LEN + 1 ];
  uint16                value_1;
  uint16                value_2;
  uint16                value_3;
  uint16                value_4;
  bt_lmp_features_type  lmp_features_ext;
  bt_sec_level_type     sec_level;
  bt_rm_lk_status_type  link_key_status;
  boolean               ssp_capable;
  bt_device_eir_type    eir_data;
} bt_device_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD bt_device_type.name_str STRING (BT_SD_MAX_DEVICE_NAME_LEN + 1) */
/*~ FIELD bt_device_type.nick_name_str STRING (BT_MAX_REMOTE_NICK_NAME_LEN + 1)*/
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef BT_PACKED struct
{
  bt_sd_uuid_type  service_class;
  uint16           profile_version;
  uint8            server_channel_number;
} bt_service_type;

typedef enum
{
   BT_RM_ATZRQ_INVALID,
   BT_RM_ATZRQ_INCOMING,
   BT_RM_ATZRQ_OUTGOING
}bt_rm_atzrq_type;

typedef enum
{
  BT_RM_IC_ALL,
  BT_RM_IC_MATCH_SERVICE_CLASS,
  BT_RM_IC_MATCH_BONDED,
  BT_RM_IC_MATCH_BD_ADDR_MASK,
  BT_RM_IC_MATCH_VALUE_1,
  BT_RM_IC_MATCH_VALUE_2,
  BT_RM_IC_MATCH_VALUE_3,
  BT_RM_IC_MATCH_VALUE_4,
  BT_RM_IC_VALUE_M = 0x0F,  /*  BT driver internal use only.  */
  BT_RM_IC_MAX,             /*  BT driver internal use only.  */
  BT_RM_IC_INCLUDE_VOLATILE_B = 0x80
} bt_rm_iter_ctrl_enum_type;

typedef BT_PACKED struct
{
  boolean                     reset;
  bt_rm_iter_ctrl_enum_type   control;
  bt_service_class_enum_type  service_class;
  boolean                     bonded;
  bt_bd_addr_type             bd_addr_mask;
  bt_bd_addr_type             bd_addr_value;
  uint16                      value_1;
  uint16                      value_2;
  uint16                      value_3;
  uint16                      value_4;
  boolean                     device_db_updated;
  uint32                      state;  /*  BT driver internal use only.  */
  uint8                       index;  /*  BT driver internal use only.  */
} bt_rm_dev_iterator_type;

typedef enum
{
  BT_RM_DU_UNBOND_B    = 0x0001,
  BT_RM_DU_SECURITY_B  = 0x0002,
  BT_RM_DU_NICK_NAME_B = 0x0004,
  BT_RM_DU_VALUE_1_B   = 0x0008,
  BT_RM_DU_VALUE_2_B   = 0x0010,
  BT_RM_DU_VALUE_3_B   = 0x0020,
  BT_RM_DU_VALUE_4_B   = 0x0040
} bt_rm_dev_update_ctrl_type;

typedef enum
{
  BT_RM_DUS_NONE           = 0,
  BT_RM_DUS_UNBONDED_B     = BT_RM_DU_UNBOND_B,
  BT_RM_DUS_SECURITY_B     = BT_RM_DU_SECURITY_B,
  BT_RM_DUS_NICK_NAME_B    = BT_RM_DU_NICK_NAME_B,
  BT_RM_DUS_VALUE_1_B      = BT_RM_DU_VALUE_1_B,
  BT_RM_DUS_VALUE_2_B      = BT_RM_DU_VALUE_2_B,
  BT_RM_DUS_VALUE_3_B      = BT_RM_DU_VALUE_3_B,
  BT_RM_DUS_VALUE_4_B      = BT_RM_DU_VALUE_4_B,
  BT_RM_DUS_ADDED_B        = 0x8000,
  BT_RM_DUS_BONDED_B       = 0x4000,
  BT_RM_DUS_LMP_INFO_B     = 0x2000,
  BT_RM_DUS_NAME_B         = 0x1000,
  BT_RM_DUS_SERVICES_B     = 0x0800,
  BT_RM_DUS_REMOVED_B      = 0x0400,
  BT_RM_DUS_INTERNAL_B     = 0x0200   /*  BT driver internal use only.  */
} bt_rm_dev_update_status_type;

typedef enum
{
  BT_RM_LU_NONE             = 0,
  BT_RM_LU_SECURITY_B       = 0x0001,
  BT_RM_LU_LINK_MODE_B      = 0x0002,
  BT_RM_LU_ROLE_B           = 0x0004,
  BT_RM_LU_ACL_PACKETS_B    = 0x0008,
  BT_RM_LU_SCO_PACKETS_B    = 0x0010,
  BT_RM_LU_RSSI_B           = 0x0020,
  BT_RM_LU_ERROR_VALID_B    = 0x0040,
  BT_RM_LU_ROLE_SW_OK_B     = 0x0080,
  BT_RM_LU_SCO_PRESENCE_B   = 0x0100,
  BT_RM_LU_SYNC_PARAM_UPD_B = 0x0200
} bt_rm_link_update_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type         bd_addr;
  bt_rm_link_update_type  updates;
  bt_security_type        security;
  boolean                 authorized;
  boolean                 role_sw_ok;
  boolean                 sco_present;
  bt_link_mode_type       link_mode;
  uint16                  cur_interval_ms;
  bt_role_type            role;
  uint16                  pkt_types_acl;
  uint16                  pkt_types_sco;
  int8                    rssi;
  bt_error_code_type      error;
  bt_sec_level_type       sec_level;
  bt_sync_conn_param_type sync_conn_params;
} bt_rm_link_status_type;

typedef enum
{
  BT_LPC_DONT_CARE,
  BT_LPC_ACTIVE,
  BT_LPC_SNIFF,
  BT_LPC_PARK,
  BT_LPC_HOLD,
  BT_LPC_SNIFF_LOCK,
  BT_LPC_LAST  /*  BT driver internal use only.  */
} bt_low_power_ctrl_type;

#ifdef FEATURE_BT_1_2
typedef BT_PACKED struct
{
  uint8 bytes[10]; 
} bt_afh_ch_map_type;

typedef enum
{
  BT_AFH_DONT_CARE,
  BT_AFH_LOW,
  BT_AFH_MED,
  BT_AFH_HIGH
} bt_rm_afh_threshold_type;
#endif /* FEATURE_BT_1_2 */

typedef enum
{
  BT_RA_RADIO_IDLE       = 0x0000, /* doing nothing                      */
  BT_RA_RADIO_FORCED_OFF = 0x0001, /* if set, other bits'd better not be */
                                   /* Triggered by app call to bt_rm_cmd_disable_radio()
                                    * Same app_id has to call the API again to remove 
                                    * the "FORCED_OFF" condition
                                   */
  BT_RA_INQUIRING        = 0x0002, /* Inquiry and Paging are mutually exclusive */
  BT_RA_PAGING           = 0x0004, /* Inquiry and Paging are mutually exclusive */
  BT_RA_INQ_SCANNING     = 0x0008, /* Indicates whether Inquiry scans have been enabled by host */
  BT_RA_PAGE_SCANNING    = 0x0010, /* Indicates whether page scans have been enabled by host */
  BT_RA_RADIO_POWERED_ON = 0x0020  /* Set when Radio is powered on.  
                                    * Radio can be powered off after "inactivity"
                                    * to conserve power even when BTHOST is in enabled state    
                                   */
} bt_radio_activity_type;

/* For backward compatability */
#define BT_RA_RADIO_OFF BT_RA_RADIO_FORCED_OFF

typedef enum
{
  BT_MODULE_ID_FTP    = 0x00000001, 
  BT_MODULE_ID_OPP    = 0x00000002,
  BT_MODULE_ID_BIP    = 0x00000004, 
  BT_MODULE_ID_BPP    = 0x00000008,
  BT_MODULE_ID_PBAP   = 0x00000010, 
  BT_MODULE_ID_A2DP   = 0x00000020,
  BT_MODULE_ID_AG     = 0x00000040, 
  BT_MODULE_ID_NA     = 0x00000080 /* DUN profile */
 /* Extend this up to 32 unique modules */
} bt_module_id_type;

typedef enum
{
  BT_MODULE_ST_IDLE    = 0, 
  BT_MODULE_ST_ACTIVE  = 1
} bt_module_state_type;

/*===========================================================================

MACRO
  BT_LMP_FEAT_SUPPORTED

DESCRIPTION
  returns TRUE if the FEATURE bit is set in the 8 byte feature field;
  otherwies FALSE

PARAMETERS
  FEATURE     - Bit number defined in spec.
  feature_ptr - ptr to 8 byte feature field

===========================================================================*/
#define BT_LMP_FEATURE_SUPPORTED( FEATURE, feature_ptr )  \
  ( ( ( (uint8*) (feature_ptr) )[ FEATURE / 8 ] ) &       \
    ( 0x01 << ( FEATURE % 8 ) ) )

extern bt_cmd_status_type bt_cmd_rm_get_remote_feat_mask(
                          bt_app_id_type           bt_app_id,
                          bt_bd_addr_type*         bd_addr_ptr,
                          bt_lmp_features_type*    remote_feat_ptr /* out parameter */
                          );

extern bt_cmd_status_type bt_cmd_rm_get_sync_conn_params(
                          bt_app_id_type           bt_app_id,
                          bt_bd_addr_type*         bd_addr_ptr,
                          bt_sync_conn_param_type* sync_param_ptr /* out parameter */
                          );

extern bt_cmd_status_type bt_cmd_rm_update_bt_module_state(
                            bt_app_id_type        bt_app_id,
                            uint32                module,
                            bt_module_state_type  state );

extern bt_cmd_status_type bt_cmd_rm_get_local_info(
                            bt_app_id_type        bt_app_id,
                            bt_bd_addr_type*      bd_addr_ptr,
                            bt_version_type*      version_ptr,
                            bt_lmp_features_type* lmp_features_ptr,
                            bt_cod_type*          class_of_device_ptr,
                            char*                 bt_name_str_ptr,
                            bt_security_type*     security_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_get_local_info */
/*~ PARAM OUT bd_addr_ptr POINTER */
/*~ PARAM OUT version_ptr POINTER */
/*~ PARAM OUT lmp_features_ptr POINTER */
/*~ PARAM OUT class_of_device_ptr POINTER */
/*~ PARAM OUT bt_name_str_ptr STRING BT_MAX_NAME_LEN */
/*~ PARAM OUT security_ptr POINTER */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_set_local_info(
                            bt_app_id_type bt_app_id,
                            bt_cod_type*   class_of_device_ptr,
                            char*          bt_name_str_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_set_local_info */
/*~ PARAM IN class_of_device_ptr POINTER */
/*~ PARAM IN bt_name_str_ptr STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_BT_2_1
extern bt_cmd_status_type bt_cmd_rm_get_local_info_ext(
                            bt_app_id_type        bt_app_id,
                            bt_bd_addr_type*      bd_addr_ptr,
                            bt_version_type*      version_ptr,
                            bt_lmp_features_type* lmp_features_ptr,
                            bt_cod_type*          class_of_device_ptr,
                            char*                 bt_name_str_ptr,
                            bt_security_type*     security_ptr,
                            char*                 bt_short_name_str );

extern bt_cmd_status_type bt_cmd_rm_set_local_info_ext(
                            bt_app_id_type bt_app_id,
                            bt_cod_type*   class_of_device_ptr,
                            char*          bt_name_str_ptr,
                            char*          bt_short_name_str_ptr );
#endif /* FEATURE_BT_2_1 */

extern bt_cmd_status_type bt_cmd_rm_config_application(
                            bt_app_id_type        bt_app_id,
                            boolean               remove_app_config,
                            bt_rm_act_vs_pwr_type page_scan_act_vs_pwr,
                            bt_rm_act_vs_pwr_type inq_scan_act_vs_pwr,
                            boolean               deliver_dev_updates );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_config_application */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_auto_service_search_disable(
                            bt_app_id_type  bt_app_id,
                            boolean         disable );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_auto_service_search_disable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_register_link_status_ext(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            boolean          security_changes,
                            boolean          mode_changes,
                            boolean          role_switches,
                            boolean          new_acl_pkts,
                            boolean          new_sco_pkts,
                            uint16           rssi_delta, 
                            boolean          sync_param_updates );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_register_link_status */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_register_link_status(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            boolean          security_changes,
                            boolean          mode_changes,
                            boolean          role_switches,
                            boolean          new_acl_pkts,
                            boolean          new_sco_pkts,
                            uint16           rssi_delta );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_register_link_status */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_unregister_link_status(
                            bt_app_id_type    bt_app_id,
                            bt_bd_addr_type*  bd_addr_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_unregister_link_status */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_get_link_status(
                            bt_app_id_type          bt_app_id,
                            bt_rm_link_status_type* link_status_ptr );

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_get_link_status */
/*~ PARAM OUT link_status_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_register_radio_activity(
                            bt_app_id_type  bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_register_radio_activity */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_unregister_radio_activity(
                            bt_app_id_type  bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_unregister_radio_activity */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_get_radio_activity(
                            bt_app_id_type          bt_app_id,
                            bt_radio_activity_type* radio_activity_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_get_radio_activity */
/*~ PARAM OUT radio_activity_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_low_power_config(
                            bt_app_id_type         bt_app_id,
                            bt_bd_addr_type*       bd_addr_ptr,
                            bt_low_power_ctrl_type low_power_ctrl,
                            uint16                 max_interval_ms,
                            uint16                 idle_timeout_ms );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_low_power_config */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_low_power_config_ext(
                            bt_app_id_type          bt_app_id,
                            bt_bd_addr_type*        bd_addr_ptr,
                            bt_low_power_ctrl_type  low_power_ctrl,
                            uint16                  max_interval_ms,
                            uint16                  idle_timeout_ms,
                            bt_sniff_ssr_ctrl_type* sniff_ssr_ctrl_ptr );

extern bt_cmd_status_type bt_cmd_rm_set_connection_role(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            bt_role_type     role );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_rm_set_connection_role */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_get_connection_role(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            bt_role_type*    role_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_rm_get_connection_role */
/*~ PARAM IN bd_addr_ptr POINTER */
/*~ PARAM OUT role_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_set_conn_role_sw_req_ok(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            boolean          role_sw_req_ok );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_rm_set_conn_role_sw_req_ok */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_get_conn_role_sw_req_ok(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            boolean*         role_sw_req_ok_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_get_conn_role_sw_req_ok */
/*~ PARAM IN bd_addr_ptr POINTER */
/*~ PARAM OUT role_sw_req_ok_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_set_connection_security(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            bt_security_type security );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_rm_set_connection_security */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_set_device_security(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr,
                            bt_security_type security );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_set_device_security */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_set_service_security(
                            bt_app_id_type      bt_app_id,
                            bt_service_id_type* service_id_ptr,
                            bt_security_type    security,
                            boolean             authorize,
                            boolean             authorize_first );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_set_service_security */
/*~ PARAM IN service_id_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_set_service_sec_ext(
                            bt_app_id_type      bt_app_id,
                            bt_service_id_type* service_id_ptr,
                            uint8               min_pin_len_bytes,
                            boolean             comb_link_key_only,
                            uint8               min_enc_key_len_bytes );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_set_service_sec_ext */
/*~ PARAM IN service_id_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_authorize_reply(
                            bt_app_id_type      bt_app_id,
                            bt_service_id_type* service_id_ptr,
                            bt_bd_addr_type*    bd_addr_ptr,
                            boolean             authorized );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_authorize_reply */
/*~ PARAM IN service_id_ptr POINTER */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_bond(
                            bt_app_id_type    bt_app_id,
                            bt_bd_addr_type*  bd_addr_ptr,
                            bt_pin_code_type* pin_code_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_bond */
/*~ PARAM IN bd_addr_ptr POINTER */
/*~ PARAM IN pin_code_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_bond_cancel(
                            bt_app_id_type    bt_app_id,
                            bt_bd_addr_type*  bd_addr_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_bond_cancel */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_unbond(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_unbond */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_set_bondable(
                            bt_app_id_type bt_app_id,
                            boolean        bondable );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_set_bondable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_pin_reply(
                            bt_app_id_type    bt_app_id,
                            bt_bd_addr_type*  bd_addr_ptr,
                            boolean           pin_valid,
                            bt_pin_code_type* pin_code_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_rm_pin_reply */
/*~ PARAM IN bd_addr_ptr POINTER */
/*~ PARAM IN pin_code_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_device_add(
                            bt_app_id_type        bt_app_id,
                            const bt_device_type* device_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_device_add */
/*~ PARAM IN device_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_device_update(
                            bt_app_id_type             bt_app_id,
                            bt_rm_dev_update_ctrl_type update_control,
                            const bt_device_type*      device_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_device_update */
/*~ PARAM IN device_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_device_services_update(
                            bt_app_id_type          bt_app_id,
                            bt_bd_addr_type*        bd_addr_ptr,
                            uint16                  num_services,
                            const bt_service_type*  services_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_device_services_update */
/*~ PARAM IN bd_addr_ptr POINTER */
/*~ PARAM IN services_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_get_manuf_data(
                            bt_app_id_type    bt_app_id,
                            bt_bd_addr_type*  bd_addr_ptr,
                            uint8*            manuf_data_size,
                            uint8*            manuf_data_dest_ptr );

extern bt_cmd_status_type bt_cmd_rm_wr_eir_data(
                            bt_app_id_type  bt_app_id,
                            uint8           eir_data_type,
                            uint8           eir_data_size,
                            const uint8*    eir_data_ptr
);

extern bt_cmd_status_type bt_cmd_rm_get_uuid_list(
                            bt_app_id_type    bt_app_id,
                            bt_bd_addr_type*  bd_addr_ptr,
                            bt_uuid_type      uuid_type,
                            uint8             uuid_list_start_idx,
                            uint8*            uuid_dest_size,
                            uint16*           uuid_dest_ptr );

extern bt_cmd_status_type bt_cmd_rm_wr_eir_uuid(
                            bt_app_id_type  bt_app_id,
                            bt_uuid_type    uuid_type, 
                            boolean         uuid_list_complete,
                            uint8           bt_uuid_data_size,
                            uint8*          bt_uuid_data_ptr
);

extern bt_cmd_status_type bt_cmd_rm_device_read(
                            bt_app_id_type    bt_app_id,
                            bt_device_type*   device_ptr,
                            uint8*            num_services_ptr,
                            bt_service_type*  services_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_device_read */
/*~ PARAM INOUT device_ptr POINTER */
/*~ PARAM INOUT num_services_ptr POINTER */
/*~ PARAM OUT services_ptr VARRAY *num_services_ptr LENGTH *num_services_ptr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_device_read_iter(
                            bt_app_id_type            bt_app_id,
                            bt_rm_dev_iterator_type*  dev_iter_ptr,
                            bt_device_type*           device_ptr,
                            uint8*                    num_services_ptr,
                            bt_service_type*          services_ptr );

void bt_negate_okts( void ) ;
void bt_assert_okts( void ) ;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_device_read_iter */
/*~ PARAM IN dev_iter_ptr POINTER */
/*~ PARAM INOUT device_ptr POINTER */
/*~ PARAM INOUT num_services_ptr POINTER */
/*~ PARAM OUT services_ptr VARRAY *num_services_ptr LENGTH *num_services_ptr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_device_remove(
                            bt_app_id_type   bt_app_id,
                            bt_bd_addr_type* bd_addr_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_device_remove */
/*~ PARAM IN bd_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_device_remove_all(
                            bt_app_id_type bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_device_remove_all */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_test_mode_enable(
                            bt_app_id_type  bt_app_id,
                            boolean         enable );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_test_mode_enable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_BT_TELEC
extern bt_cmd_status_type bt_cmd_rm_start_telec_test(
                            bt_app_id_type bt_app_id,
                            boolean        transmit,
                            boolean        hopping );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_start_telec_test */
#endif /* FEATURE_HTORPC_METACOMMENTS */
#endif /* FEATURE_BT_TELEC */

extern bt_cmd_status_type bt_cmd_rm_disable_radio(
                            bt_app_id_type bt_app_id,
                            boolean        disable );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_disable_radio */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_disable_discoverability(
                            bt_app_id_type bt_app_id,
                            boolean        disable );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_disable_discoverability */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#define BT_RM_MAX_DEV_BLACK_LIST_COUNT 20

extern bt_cmd_status_type bt_cmd_rm_set_device_black_list(
                            bt_app_id_type    bt_app_id,
                            uint16            dev_black_list_count,
                            bt_bd_addr_type   dev_black_list[] );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_set_device_black_list */
/*~ PARAM IN dev_black_list VARRAY dev_black_list_count LENGTH dev_black_list_count */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#define BT_RM_MAX_DEV_WHITE_LIST_COUNT 20

extern bt_cmd_status_type bt_cmd_rm_set_device_white_list(
                            bt_app_id_type    bt_app_id,
                            uint16            dev_white_list_count,
                            bt_bd_addr_type   dev_white_list[] );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_set_device_white_list */
/*~ PARAM IN dev_white_list VARRAY dev_white_list_count LENGTH dev_white_list_count */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_BT_1_2
extern bt_cmd_status_type bt_cmd_rm_config_afh(
                            bt_app_id_type bt_app_id,
                            uint16         afh_period,
                            uint16         retx_threshold,
                            uint16         crc_err_threshold );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_config_afh */
#endif /* FEATURE_HTORPC_METACOMMENTS */
   
extern bt_cmd_status_type bt_cmd_rm_enable_afh(
                            bt_app_id_type           bt_app_id,
                            boolean                  enable,
                            bt_rm_afh_threshold_type level );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_enable_afh */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_set_afh_ch_map(
                            bt_app_id_type      bt_app_id,
                            bt_afh_ch_map_type* ch_map_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_set_afh_ch_map */
/*~ PARAM IN ch_map_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_rm_get_afh_ch_map(
                            bt_app_id_type      bt_app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_rm_get_afh_ch_map */
#endif /* FEATURE_HTORPC_METACOMMENTS */
#endif /* FEATURE_BT_1_2 */

extern bt_cmd_status_type bt_cmd_rm_set_io_cap(
                            bt_app_id_type    bt_app_id,
                            bt_rm_io_cap_type io_cap );

extern bt_cmd_status_type bt_cmd_rm_set_sm4(
                            bt_app_id_type       bt_app_id,
                            bt_service_id_type*  service_id_ptr,
                            bt_sec_level_type    min_security,
                            boolean              authorize,
                            boolean              authorize_first );

extern bt_cmd_status_type bt_cmd_rm_wr_remote_oob_data(
                            bt_app_id_type     bt_app_id,
                            uint8              size_oob_data,
                            const void*        ptr_oob_data );

extern bt_cmd_status_type bt_cmd_rm_user_confirmation_reply(
                            bt_app_id_type     bt_app_id,
                            bt_bd_addr_type*   bd_addr_ptr,
                            boolean            answer );

extern bt_cmd_status_type bt_cmd_rm_passkey_reply(
                            bt_app_id_type     bt_app_id,
                            bt_bd_addr_type*   bd_addr_ptr,
                            const char         passkey[BT_RM_SSP_PASSKEY_LEN],
                            boolean            valid );

extern bt_cmd_status_type bt_cmd_rm_keypress_notification(
                            bt_app_id_type     bt_app_id,
                            bt_bd_addr_type*   bd_addr_ptr,
                            bt_rm_keypress_notification_type key );

extern bt_cmd_status_type bt_cmd_rm_create_local_oob_data(
                            bt_app_id_type     bt_app_id );

extern bt_cmd_status_type bt_cmd_rm_rd_local_oob_data(
                            bt_app_id_type     bt_app_id,
                            uint8*             oob_data_size,
                            uint8*             oob_data );

extern bt_cmd_status_type bt_cmd_rm_bond_ext(
                            bt_app_id_type     bt_app_id,
                            bt_bd_addr_type*   bd_addr_ptr,
                            bt_pin_code_type*  pin_code_ptr,
                            boolean            mitm_protection_required );

extern bt_cmd_status_type bt_cmd_rm_authorize_rebond(
                            bt_app_id_type     bt_app_id,
                            bt_bd_addr_type*   bd_addr_ptr,
                            boolean            rebond,
                            bt_rm_authorize_bond_type bonding_requirements );

extern bt_cmd_status_type bt_cmd_rm_set_sm4_debug_mode(
                            bt_app_id_type     bt_app_id,
                            boolean            enable_debug_mode );

/*-------------------------------------------------------------------------*/
/*                      Radio Manager (RM) Events                          */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  bt_rm_lk_status_type lk_status_type;
} bt_ev_rm_bonded_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  reason;
} bt_ev_rm_bond_failed_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_ev_rm_pin_request_type;

typedef BT_PACKED struct
{
  bt_service_id_type  service_id;
  bt_bd_addr_type     bd_addr;
  bt_rm_atzrq_type    authorize_dir;
} bt_ev_rm_authorize_request_type;

typedef BT_PACKED struct
{
  bt_service_id_type   service_id;
  bt_bd_addr_type      bd_addr;
  boolean              secure_enough;
  boolean              authorize;
  boolean              authorized;
#ifdef FEATURE_BT_2_1
  bt_sec_level_type    min_sec_level;
  bt_sec_level_type    sec_level;
#else
  bt_security_type     min_security;
  bt_security_type     security;
#endif /* FEATURE_BT_2_1 */
  uint8                min_pin_len_bytes;
  uint8                pin_len_bytes;
  boolean              comb_link_key_only;
  bt_hc_link_key_type  link_key_type;
  uint8                min_enc_key_len_bytes;
  uint8                enc_key_len_bytes;
} bt_ev_rm_serv_sec_result_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type               bd_addr;
  boolean                       success;
  bt_rm_dev_update_status_type  update_status;
} bt_ev_rm_device_updated_type;

typedef BT_PACKED struct
{
  bt_rm_link_status_type  link_status;
} bt_ev_rm_link_status_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

typedef BT_PACKED struct
{
  boolean  disabled;
} bt_ev_rm_radio_disabled_type;

typedef BT_PACKED struct
{
  bt_radio_activity_type  radio_activity;
} bt_ev_rm_radio_activity_type;

#ifdef FEATURE_BT_1_2
typedef BT_PACKED struct
{
  boolean               success;
} bt_ev_rm_afh_ch_map_set_type;

typedef BT_PACKED struct
{
  boolean            success;
  bt_afh_ch_map_type ch_map;
} bt_ev_rm_afh_ch_map_type;
#endif /* FEATURE_BT_1_2 */

typedef BT_PACKED struct
{
  uint8   size_oob_data;
} bt_ev_rm_oob_local_data_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type     bd_addr;
} bt_ev_rm_oob_remote_data_req_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type     bd_addr;
  char                passkey[ BT_RM_SSP_PASSKEY_LEN ];
} bt_ev_rm_user_confirmation_req_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  char             passkey[ BT_RM_SSP_PASSKEY_LEN ];
} bt_ev_rm_passkey_notification_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type     bd_addr;
} bt_ev_rm_passkey_req_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type     bd_addr;
} bt_ev_rm_rebond_req_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type     bd_addr;
  bt_rm_keypress_notification_type key;
} bt_ev_rm_keypress_notification_type;

/*-------------------------------------------------------------------------*/
/*                  Serial IO (SIO) Commands                               */
/*-------------------------------------------------------------------------*/
extern bt_cmd_status_type bt_cmd_sio_in_flow_on
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

extern bt_cmd_status_type bt_cmd_sio_in_flow_off
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

bt_cmd_status_type bt_cmd_sio_change_mode
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_stream_mode_type   stream_mode,
  dsm_watermark_type    *tx_queue,
  dsm_watermark_type    *rx_queue,
  sio_rx_func_ptr_type   rx_func_ptr

);

bt_cmd_status_type bt_cmd_sio_enable_autodetect
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

bt_cmd_status_type bt_cmd_sio_set_flow_ctl
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_flow_ctl_type      tx_flow,
  sio_flow_ctl_type      rx_flow
);

bt_cmd_status_type bt_cmd_sio_enable_dtr_event
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_vv_func_ptr_type   enable_dte_ready_event
);

bt_cmd_status_type bt_cmd_sio_disable_dtr_event
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

bt_cmd_status_type bt_cmd_sio_cd_assert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

bt_cmd_status_type bt_cmd_sio_cd_deassert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

bt_cmd_status_type bt_cmd_sio_ri_assert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

bt_cmd_status_type bt_cmd_sio_ri_deassert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

bt_cmd_status_type bt_cmd_sio_dsr_assert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

bt_cmd_status_type bt_cmd_sio_dsr_deassert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

bt_cmd_status_type bt_cmd_sio_record_pkt_func_ptr
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_vv_func_ptr_type   func_ptr
);

bt_cmd_status_type bt_cmd_sio_record_esc_func_ptr
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_vv_func_ptr_type   func_ptr
);

bt_cmd_status_type bt_cmd_sio_bt_configure
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  uint8                  baudrate,
  uint8                  format,
  uint8                  flow_ctrl,
  uint8                  xon_char,
  uint8                  xoff_char
);

bt_cmd_status_type bt_cmd_sio_bt_flush_tx
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_vv_func_ptr_type   func_ptr
);

bt_cmd_status_type bt_cmd_sio_bt_disconnect
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
);

extern bt_cmd_status_type bt_cmd_sio_transmit
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  dsm_item_type*         dsm_ptr
);


/*-------------------------------------------------------------------------*/
/*  Bluetooth command type enumeration.                                    */
/*-------------------------------------------------------------------------*/

/* Any new command added to this enum should be added to the end of the */   
/* group to which it belongs.This is to support test automation         */
typedef enum
{
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

  BT_CMD_NA_ENABLE =                 BT_CMD_EV_NA_BASE,
  BT_CMD_NA_DISABLE,

#ifdef FEATURE_BT_AG
  BT_CMD_AG_ENABLE =                 BT_CMD_EV_AG_BASE,
  BT_CMD_AG_CONNECT,
  BT_CMD_AG_CONFIG,
  BT_CMD_AG_SET_VR_CAPABLE,
  BT_CMD_AG_RING_AUDIO_DEVICE,
  BT_CMD_AG_SET_AD_SPKR_GAIN,
  BT_CMD_AG_SET_AD_MIC_GAIN,
  BT_CMD_AG_AUDIO_CONNECT,
  BT_CMD_AG_AUDIO_DISCONNECT,
  BT_CMD_AG_DISCONNECT,
  BT_CMD_AG_DISABLE,
  BT_CMD_AG_UPDATE_VR_STATE,
  BT_CMD_AG_EXT_IO_CONFIG,
  BT_CMD_AG_SET_INBAND_RING,
  BT_CMD_AG_SET_BATTCHG_IND,
  BT_CMD_AG_SET_RSSI,
  BT_CMD_AG_SEND_SUPPORTED_RH,
  BT_CMD_AG_SEND_CURRENT_RH,
#ifdef BT_AG_HF_EXTENSION
  BT_CMD_AG_SEND_PB_LIST,
  BT_CMD_AG_SEND_PB_SELECT_RESULT,
  BT_CMD_AG_SEND_PB_INFO,
  BT_CMD_AG_SEND_PB_ENTRIES_INFO,
  BT_CMD_AG_SEND_PB_READ_RESULT,
  BT_CMD_AG_SEND_PB_WRITE_RESULT,
  BT_CMD_AG_SEND_SUBS_NUM_INFO,
#endif /* BT_AG_HF_EXTENSION */
#endif /* FEATURE_BT_AG */

  BT_CMD_BA_SET_BONDABLE_MODE =      BT_CMD_EV_BA_BASE,
  BT_CMD_BA_BOND_DEVICE,
  BT_CMD_BA_UNBOND_DEVICE,
  BT_CMD_BA_SET_PASS_KEY,
  BT_CMD_BA_SET_AUTH_ENABLE,
  BT_CMD_BA_SET_ENCRYPT_ENABLE,

  BT_CMD_SD_DISCOVER_DEVICES =       BT_CMD_EV_SD_BASE,
  BT_CMD_SD_START_DEVICE_DISCOVERY,
  BT_CMD_SD_STOP_DEVICE_DISCOVERY,
  BT_CMD_SD_GET_DEVICE_NAME,
  BT_CMD_SD_SEARCH_SERVICE,
  BT_CMD_SD_SET_SERVICE_DISCOVERABLE_MODE,
  BT_CMD_SD_GET_SERVER_CHANNEL_NUMBER,
  BT_CMD_SD_UPDATE_SERVICE_RECORD,
  BT_CMD_SD_REGISTER_SERVICE,
  BT_CMD_SD_UNREGISTER_SERVICE,
  BT_CMD_SD_SEARCH_SERVICE_UUID128,
  BT_CMD_SD_UPDATE_OBEX_PROFILE_SERVICE_RECORD,
  BT_CMD_SD_CUSTOM_UPDATE_OBEX_PROFILE_SERVICE_RECORD,
  BT_CMD_SD_REGISTER_SYNCML_SERVICE,
  BT_CMD_SD_UNREGISTER_SYNCML_SERVICE,
  BT_CMD_SD_UPDATE_SRV_REC_BY_SCN,
  BT_CMD_SD_UPDATE_SRV_REC_BY_PSM,
  BT_CMD_SD_UPDATE_SRV_NAME,
  BT_CMD_SD_REGISTER_SERV_EXT,
  BT_CMD_SD_UNREGISTER_SRV_BY_SCN,
  BT_CMD_SD_UNREGISTER_SRV_BY_PSM,
  BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER,
  BT_CMD_SD_GET_DISCOVERABLE_MODE,
  BT_CMD_SD_REGISTER_CUSTOM_SERVICE,
  BT_CMD_SD_UNREGISTER_CUSTOM_SERVICE,
  BT_CMD_SD_GET_DEVICE_NAME_CANCEL,
  BT_CMD_SD_CANCEL_IDLE_TIMER,
  BT_CMD_SD_SEARCH_SERVICE_EXT,
  BT_CMD_SD_DISCOVER_DEVICES_EXT,
  BT_CMD_SD_STOP_SERVICE_SEARCH,
  BT_CMD_SD_REGISTER_CUSTOM_SERVICE_EXT,
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
  
  BT_CMD_SIO_IN_FLOW_ON =            BT_CMD_EV_SIO_BASE,
  BT_CMD_SIO_IN_FLOW_OFF,
  BT_CMD_SIO_TRANSMIT,
  BT_CMD_SIO_CHANGE_MODE,
  BT_CMD_SIO_ENABLE_AUTODETECT,
  BT_CMD_SIO_SET_FLOW_CTL,
  BT_CMD_SIO_ENABLE_DTR_EVENT,
  BT_CMD_SIO_DISABLE_DTR_EVENT,
  BT_CMD_SIO_CD_ASSERT,
  BT_CMD_SIO_CD_DEASSERT,
  BT_CMD_SIO_RI_ASSERT,
  BT_CMD_SIO_RI_DEASSERT,
  BT_CMD_SIO_DSR_ASSERT,
  BT_CMD_SIO_DSR_DEASSERT,
  BT_CMD_SIO_RECORD_PKT_FUNC_PTR,
  BT_CMD_SIO_RECORD_ESC_FUNC_PTR,
  BT_CMD_SIO_BT_CONFIGURE,
  BT_CMD_SIO_BT_DISCONNECT,
  BT_CMD_SIO_BT_FLUSH_TX,

  BT_CMD_RC_REGISTER =               BT_CMD_EV_RC_BASE,
  BT_CMD_RC_CONNECT,
  BT_CMD_RC_CONN_CUST_CFG,
  BT_CMD_RC_TX_DATA,
  BT_CMD_RC_SET_PORT_RX_FC,
  BT_CMD_RC_SET_SESSION_RX_FC,
  BT_CMD_RC_WRITE_MS,
  BT_CMD_RC_WRITE_SP_CFG,
  BT_CMD_RC_LINE_ERROR,
  BT_CMD_RC_PING,
  BT_CMD_RC_DISCONNECT,
  BT_CMD_RC_UNREGISTER,
  BT_CMD_RC_CANCEL_CONNECT,
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

  BT_CMD_L2_SET_PARAMS =             BT_CMD_EV_L2_BASE,
  BT_CMD_L2_REGISTER,
  BT_CMD_L2_CONNECT,
  BT_CMD_L2_WRITE,
  BT_CMD_L2_PING,
  BT_CMD_L2_DISCONNECT,
  BT_CMD_L2_UNREGISTER,
  BT_CMD_L2_CONNECT_IMMEDIATE_CID,
  BT_CMD_L2_DISABLE_TIMEOUT,
  BT_CMD_L2_DISCONN_IMMEDIATE,

  BT_CMD_RM_SET_LOCAL_INFO =         BT_CMD_EV_RM_BASE,
  BT_CMD_RM_CONFIG_APPLICATION,
  BT_CMD_RM_REG_LINK_STATUS,
  BT_CMD_RM_UNREG_LINK_STATUS,
  BT_CMD_RM_LOW_POWER_CONFIG,
  BT_CMD_RM_SET_CONN_ROLE,
  BT_CMD_RM_SET_CONN_SECURITY,
  BT_CMD_RM_SET_DEVICE_SECURITY,
  BT_CMD_RM_BOND,
  BT_CMD_RM_UNBOND,
  BT_CMD_RM_SET_BONDABLE,
  BT_CMD_RM_PIN_REPLY,
  BT_CMD_RM_DEVICE_ADD,
  BT_CMD_RM_DEVICE_UPDATE,
  BT_CMD_RM_DEVICE_REMOVE,
  BT_CMD_RM_TEST_MODE_ENABLE,
  BT_CMD_RM_SET_CONNECTABLE,
  BT_CMD_RM_SET_DISCOVERABLE,
  BT_CMD_RM_INQUIRE,
  BT_CMD_RM_STOP_INQUIRY,
  BT_CMD_RM_REMOTE_NAME_REQ,
  BT_CMD_RM_CONNECT_ACL,
  BT_CMD_RM_REGISTER_ACL,
  BT_CMD_RM_ACCEPT_ACL,
  BT_CMD_RM_REJECT_ACL,
  BT_CMD_RM_UNREGISTER_ACL,
  BT_CMD_RM_TX_DATA_ACL,
  BT_CMD_RM_DISCONNECT_ACL,
  BT_CMD_RM_CONNECT_SCO,
  BT_CMD_RM_REGISTER_SCO,
  BT_CMD_RM_ACCEPT_SCO,
  BT_CMD_RM_REJECT_SCO,
  BT_CMD_RM_UNREGISTER_SCO,
  BT_CMD_RM_TX_DATA_SCO,
  BT_CMD_RM_DISCONNECT_SCO,
  BT_CMD_RM_SET_CONN_ROLE_SW_REQ_OK,
  BT_CMD_RM_SET_SERVICE_SECURITY,
  BT_CMD_RM_AUTHORIZE_REPLY,
  BT_CMD_RM_ENFORCE_SEC_RFCOMM_SCN,
  BT_CMD_RM_ENFORCE_SEC_L2CAP_PSM,
  BT_CMD_RM_AUTO_SERVICE_SEARCH_DISABLE,
  BT_CMD_RM_DEVICE_SERVICES_UPDATE,
#ifdef FEATURE_BT_TELEC
  BT_CMD_RM_START_TELEC_TEST,
#endif
  BT_CMD_RM_DISABLE_RADIO,
  BT_CMD_RM_DISABLE_DISCOVERABILITY,
  BT_CMD_RM_SET_DEVICE_BLACK_LIST,
  BT_CMD_RM_SET_DEVICE_WHITE_LIST,
  BT_CMD_RM_CONFIG_AFH,
  BT_CMD_RM_ENABLE_AFH,
  BT_CMD_RM_GET_AFH_CH_MAP,
  BT_CMD_RM_SET_AFH_CH_MAP,
  BT_CMD_RM_SET_SERVICE_SEC_EXT,
  BT_CMD_RM_REG_RADIO_ACTIVITY,
  BT_CMD_RM_UNREG_RADIO_ACTIVITY,
  BT_CMD_RM_DEVICE_REMOVE_ALL,
  BT_CMD_RM_REMOTE_NAME_REQ_CANCEL,
  BT_CMD_RM_CONNECT_CANCEL,
  BT_CMD_RM_UPDATE_BT_MODULE_ST,
#ifdef FEATURE_BT_2_1
  BT_CMD_RM_LOW_POWER_CONFIG_EXT,
  BT_CMD_RM_SET_AUTO_FLUSH_TO,
  BT_CMD_RM_SET_IO_CAP,
  BT_CMD_RM_SET_SM4,
  BT_CMD_RM_WR_REMOTE_OOB_DATA,
  BT_CMD_RM_USER_CONFIRMATION_REPLY,
  BT_CMD_RM_PASSKEY_REPLY,
  BT_CMD_RM_KEYPRESS_NOTIFICATION,
  BT_CMD_RM_CREATE_LOCAL_OOB_DATA,
  BT_CMD_RM_BOND_EXT,
  BT_CMD_RM_AUTHORIZE_REBOND,
  BT_CMD_RM_SET_SM4_DEBUG_MODE,
  BT_CMD_RM_SET_LOCAL_INFO_EXT,
  BT_CMD_RM_WR_EIR_DATA,
  BT_CMD_RM_WR_EIR_UUID,
#endif /* FEATURE_BT_2_1 */
  BT_CMD_RM_BOND_CANCEL,

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
   
  BT_CMD_HC_REGISTER =               BT_CMD_EV_HC_BASE,
  BT_CMD_HC_TX_ACL_DATA,
  BT_CMD_HC_TX_SCO_DATA,
  BT_CMD_HC_INQUIRY,
  BT_CMD_HC_INQUIRY_CANCEL,
  BT_CMD_HC_PERIODIC_INQUIRY,
  BT_CMD_HC_EXIT_PERIODIC_INQUIRY,
  BT_CMD_HC_CREATE_CONNECTION,
  BT_CMD_HC_DISCONNECT,
  BT_CMD_HC_ADD_SCO_CONN,
  BT_CMD_HC_ACCEPT_CONN,
  BT_CMD_HC_REJECT_CONN,
  BT_CMD_HC_LINK_KEY,
  BT_CMD_HC_LINK_KEY_NEG,
  BT_CMD_HC_PIN_CODE,
  BT_CMD_HC_PIN_CODE_NEG,
  BT_CMD_HC_CHANGE_PKT_TYPE,
  BT_CMD_HC_AUTH_REQ,
  BT_CMD_HC_SET_CONN_ENCRYPT,
  BT_CMD_HC_CHANGE_LINK_KEY,
  BT_CMD_HC_MASTER_LINK_KEY,
  BT_CMD_HC_REMOTE_NAME_REQ,
  BT_CMD_HC_RD_REMOTE_FEATURES,
  BT_CMD_HC_RD_REMOTE_VERSION,
  BT_CMD_HC_RD_CLK_OFFSET,
  BT_CMD_HC_HOLD_MODE,
  BT_CMD_HC_SNIFF_MODE,
  BT_CMD_HC_EXIT_SNIFF_MODE,
  BT_CMD_HC_PARK_MODE,
  BT_CMD_HC_EXIT_PARK_MODE,
  BT_CMD_HC_QOS_SETUP,
  BT_CMD_HC_ROLE_DISCOVERY,
  BT_CMD_HC_SWITCH_ROLE,
  BT_CMD_HC_RD_LINK_POLICY,
  BT_CMD_HC_WR_LINK_POLICY,
  BT_CMD_HC_SET_EVENT_MASK,
  BT_CMD_HC_RESET,
  BT_CMD_HC_SET_EVENT_FILTER,
  BT_CMD_HC_FLUSH,
  BT_CMD_HC_RD_PIN_TYPE,
  BT_CMD_HC_WR_PIN_TYPE,
  BT_CMD_HC_CREATE_NEW_UNIT_KEY,
  BT_CMD_HC_RD_STORED_LINK_KEY,
  BT_CMD_HC_WR_STORED_LINK_KEY,
  BT_CMD_HC_DEL_STORED_LINK_KEY,
  BT_CMD_HC_CHANGE_LOCAL_NAME,
  BT_CMD_HC_RD_LOCAL_NAME,
  BT_CMD_HC_RD_CONN_ACCEPT_TO,
  BT_CMD_HC_WR_CONN_ACCEPT_TO,
  BT_CMD_HC_RD_PAGE_TO,
  BT_CMD_HC_WR_PAGE_TO,
  BT_CMD_HC_RD_SCAN_ENABLE,
  BT_CMD_HC_WR_SCAN_ENABLE,
  BT_CMD_HC_RD_PAGE_SCAN_ACTIVITY,
  BT_CMD_HC_WR_PAGE_SCAN_ACTIVITY,
  BT_CMD_HC_RD_INQ_SCAN_ACTIVITY,
  BT_CMD_HC_WR_INQ_SCAN_ACTIVITY,
  BT_CMD_HC_RD_AUTH_ENABLE,
  BT_CMD_HC_WR_AUTH_ENABLE,
  BT_CMD_HC_RD_ENCRYPT_MODE,
  BT_CMD_HC_WR_ENCRYPT_MODE,
  BT_CMD_HC_RD_CLASS_OF_DEVICE,
  BT_CMD_HC_WR_CLASS_OF_DEVICE,
  BT_CMD_HC_RD_VOICE_SETTING,
  BT_CMD_HC_WR_VOICE_SETTING,
  BT_CMD_HC_RD_AUTO_FLUSH_TO,
  BT_CMD_HC_WR_AUTO_FLUSH_TO,
  BT_CMD_HC_RD_BROAD_RETRANS,
  BT_CMD_HC_WR_BROAD_RETRANS,
  BT_CMD_HC_RD_HOLD_MODE_ACTIVITY,
  BT_CMD_HC_WR_HOLD_MODE_ACTIVITY,
  BT_CMD_HC_RD_TX_POWER_LEVEL,
  BT_CMD_HC_RD_SCO_FLOW_CTRL,
  BT_CMD_HC_WR_SCO_FLOW_CTRL,
  BT_CMD_HC_SET_HC_TO_HOST_FC,
  BT_CMD_HC_HOST_BUFFER_SIZE,
  BT_CMD_HC_HOST_NUM_CMPLT_PACKETS,
  BT_CMD_HC_RD_LINK_SUPERVISION_TO,
  BT_CMD_HC_WR_LINK_SUPERVISION_TO,
  BT_CMD_HC_RD_NUM_SUPPORTED_IAC,
  BT_CMD_HC_RD_CURRENT_IAC_LAP,
  BT_CMD_HC_WR_CURRENT_IAC_LAP,
  BT_CMD_HC_RD_PAGE_SCAN_PER_MODE,
  BT_CMD_HC_WR_PAGE_SCAN_PER_MODE,
  BT_CMD_HC_RD_PAGE_SCAN_MODE,
  BT_CMD_HC_WR_PAGE_SCAN_MODE,
  BT_CMD_HC_RD_LOCAL_VERSION_INFO,
  BT_CMD_HC_RD_LOCAL_FEATURES,
  BT_CMD_HC_RD_BUFFER_SIZE,
  BT_CMD_HC_RD_COUNTRY_CODE,
  BT_CMD_HC_RD_BD_ADDR,
  BT_CMD_HC_RD_FAILED_CONTACT_COUNTER,
  BT_CMD_HC_RESET_FAILED_CONTACT_COUNTER,
  BT_CMD_HC_GET_LINK_QUALITY,
  BT_CMD_HC_RD_RSSI,
  BT_CMD_HC_RD_LOOPBACK_MODE,
  BT_CMD_HC_WR_LOOPBACK_MODE,
  BT_CMD_HC_ENABLE_DUT_MODE,
  BT_CMD_HC_FIX_INIT_WHITE,
  BT_CMD_HC_FIX_INIT_ENC,
  BT_CMD_HC_RD_RX_STATUS_EN,
  BT_CMD_HC_CONFIG_SCHR,
  BT_CMD_HC_RD_TX_BUFFER_EN,
  BT_CMD_HC_RD_RX_BUFFER_EN,
  BT_CMD_HC_SEND_RAW_MODE_PKT,
#ifdef FEATURE_BT_TEST_MODE_TESTER
  BT_CMD_HC_TESTER_ACTIVATE,
  BT_CMD_HC_TESTER_CONTROL,
  BT_CMD_HC_TESTER_START,
#endif /* FEATURE_BT_TEST_MODE_TESTER */
  BT_CMD_HC_SET_NEW_BD_ADDR,
  BT_CMD_HC_TIME_ACCURACY_REQ,
  BT_CMD_HC_MODIFY_BEACON,
  BT_CMD_HC_RD_CLK,
  BT_CMD_HC_RD_SBI_REG,
  BT_CMD_HC_WR_SBI_REG,
  BT_CMD_HC_DIAC_LAP_EN,
  BT_CMD_HC_RD_ENC_KEY_LENGTH,
  BT_CMD_HC_WR_ENC_KEY_LENGTH,
  BT_CMD_HC_CHANGE_REMOTE_PKT_TYPE,
  BT_CMD_HC_STATUS_VIEW_EN,
  BT_CMD_HC_ENABLE_LOOPBACK,
  BT_CMD_HC_SET_INCOMING_HOLD_PARAMS,
  BT_CMD_HC_SET_INCOMING_SNIFF_PARAMS,
  BT_CMD_HC_SET_INCOMING_PARK_PARAMS,
  BT_CMD_HC_SET_TX_POWER_LEVEL,
  BT_CMD_HC_CERT_CTRL,
  BT_CMD_HC_TX_CONTINUOUS,

  BT_CMD_HC_RD_RAW_RSSI,
  BT_CMD_HC_TEST_SET_LM_PARAMS,
  BT_CMD_HC_TEST_SEND_LMP_MSG,
  BT_CMD_HC_WR_RAM,
  BT_CMD_HC_RD_RAM,

  BT_CMD_HC_SET_AFH_HOST_CHAN_CLASS,
  BT_CMD_HC_READ_AFH_CHAN_ASSESS_MODE,
  BT_CMD_HC_SET_AFH_CHAN_ASSESS_MODE,
  BT_CMD_HC_READ_AFH_CHAN_MAP,
  BT_CMD_HC_SETUP_SYNC_CONNECTION,
  BT_CMD_HC_ACCEPT_SYNC_CONN_REQ,
  BT_CMD_HC_REJECT_SYNC_CONN_REQ,
  
  BT_CMD_HC_CONTROL_AFH,
  BT_CMD_HC_SET_AFH_CHAN_GOOD,
  BT_CMD_HC_SET_ESCO_PKT_LEN,
  BT_CMD_HC_TX_CONTINUOUS_FREQ,

  BT_CMD_HC_RD_INQUIRY_SCAN_TYPE,
  BT_CMD_HC_WR_INQUIRY_SCAN_TYPE,
  BT_CMD_HC_RD_INQUIRY_MODE,
  BT_CMD_HC_WR_INQUIRY_MODE,
  BT_CMD_HC_RD_PAGE_SCAN_TYPE,
  BT_CMD_HC_WR_PAGE_SCAN_TYPE,
  BT_CMD_HC_RD_REMOTE_EXT_FEATURES,

  BT_CMD_HC_REMOTE_NAME_REQ_CANCEL,
  BT_CMD_HC_CREATE_CONNECTION_CANCEL,
  BT_CMD_HC_RD_LOCAL_CMDS,
  BT_CMD_HC_RD_LOCAL_EXT_FEAT,

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
  
  BT_CMD_HC_VS_ANY,

#ifdef FEATURE_BT_2_1
  BT_CMD_HC_IO_CAPABILITY_RQST_REPLY,
  BT_CMD_HC_USER_CFMTN_RQST_REPLY,
  BT_CMD_HC_USER_CFMTN_RQST_NEG_REPLY,
  BT_CMD_HC_USER_PASSKEY_RQST_REPLY,
  BT_CMD_HC_USER_PASSKEY_RQST_NEG_REPLY,
  BT_CMD_HC_REMOTE_OOB_DATA_RQST_REPLY,
  BT_CMD_HC_REMOTE_OOB_DATA_RQST_NEG_REPLY,
  BT_CMD_HC_IO_CAPABILITY_RQST_NEG_REPLY,
  BT_CMD_HC_REFRESH_ENCRYPTION_KEY,
  BT_CMD_HC_RD_SIMPLE_PAIRING_MODE,
  BT_CMD_HC_WR_SIMPLE_PAIRING_MODE,
  BT_CMD_HC_RD_LOCAL_OOB_DATA,
  BT_CMD_HC_SEND_KEYPRESS_NOTIFICATION,
  BT_CMD_HC_WR_SIMPLE_PAIRING_DEBUG_MODE,
  BT_CMD_HC_SNIFF_SUBRATING,
  BT_CMD_HC_ENH_FLUSH,
  BT_CMD_HC_RD_EXT_INQ_RSP,
  BT_CMD_HC_WR_EXT_INQ_RSP,
  BT_CMD_HC_RD_INQ_RSP_TX_PWR,
  BT_CMD_HC_WR_INQ_TX_PWR,
  BT_CMD_HC_RD_DEFAULT_ERR_DATA_REPORTING_MODE,
  BT_CMD_HC_WR_DEFAULT_ERR_DATA_REPORTING_MODE,
#endif /* FEATURE_BT_2_1 */

#if defined( FEATURE_BT_SOC ) && defined( FEATURE_BT_HCI_HOST_FCTL )
  BT_CMD_FCTL_SMALL_DSM_HIGH_CNT = BT_CMD_EV_FCTL_BASE,
  BT_CMD_FCTL_SMALL_DSM_LOW_CNT,
  BT_CMD_FCTL_LARGE_DSM_HIGH_CNT,
  BT_CMD_FCTL_LARGE_DSM_LOW_CNT, 
#endif /* FEATURE_BT_HCI_HOST_FCTL && .. */ 

  BT_CMD_NULL =                      BT_CMD_EV_NULL
} bt_cmd_type;


/*-------------------------------------------------------------------------*/
/*                       General BT Driver Events                          */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_app_id_type      bt_app_id;
  bt_cmd_type         cmd_type;
  bt_cmd_status_type  cmd_status;
} bt_ev_gn_cmd_done_type;


#ifdef FEATURE_BT_EXTPF
#include "btpf.h"
/*-------------------------------------------------------------------------*/
/*   Map QCom BT Event codes to OI status                                  */
/*-------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION
  bt_pf_map_event_code

DESCRIPTION
  Maps a BT event reason to an OI_STATUS
===========================================================================*/
bt_cmd_status_type bt_pf_map_event_code( bt_event_reason_type reason );

#endif

/*-------------------------------------------------------------------------*/
/*  Union of all BT events.                                                */
/*-------------------------------------------------------------------------*/

typedef struct
{
  q_link_type          link;
  bt_event_type        ev_type;
  bt_app_id_type       bt_app_id;
  bt_ev_cb_func_type*  ev_cb_ptr;
} bt_ev_hdr_type;

#include "btevi.h"  /*  BT driver internal event declarations.  */

typedef BT_PACKED union
{
  bt_ev_gn_cmd_done_type               ev_gn_cmd_done;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

  bt_ev_na_connected_type              ev_na_conn;
  bt_ev_na_disconnected_type           ev_na_disc;

#ifdef FEATURE_BT_AG
  bt_ev_ag_enabled_type                ev_ag_enabled;
  bt_ev_ag_connected_type              ev_ag_connected;
  bt_ev_ag_disconnected_type           ev_ag_disconnected;
  bt_ev_ag_connection_failed_type      ev_ag_connection_failed;
  bt_ev_ag_ring_failed_type            ev_ag_ring_failed;
  bt_ev_ag_button_pressed_type         ev_ag_button_pressed;
  bt_ev_ag_slc_changed_type            ev_ag_slc_changed;
  bt_ev_ag_dev_pickup_type             ev_ag_dev_pickup;
  bt_ev_ag_dev_hangup_type             ev_ag_dev_hangup;
  bt_ev_ag_dev_dial_type               ev_ag_dev_dial;
  bt_ev_ag_dev_redial_type             ev_ag_dev_redial;
  bt_ev_ag_dev_mem_dial_type           ev_ag_dev_mem_dial;
  bt_ev_ag_dev_send_dtmf_type          ev_ag_dev_send_dtmf;
  bt_ev_ag_vr_state_type               ev_ag_vr_state;
  bt_ev_ag_btrh_type                   ev_ag_btrh;
#ifdef BT_AG_HF_EXTENSION
  bt_ev_ag_pap_cpbs_type               ev_ag_pap_cpbs;
  bt_ev_ag_pap_cpbr_type               ev_ag_pap_cpbr;
  bt_ev_ag_pap_cpbf_type               ev_ag_pap_cpbf;
  bt_ev_ag_pap_cpbw_type               ev_ag_pap_cpbw;
  bt_ev_ag_pap_btrh_type               ev_ag_pap_btrh;
#endif /* BT_AG_HF_EXTENSION */
  bt_ev_ag_audio_connected_type        ev_ag_audio_connected;
  bt_ev_ag_audio_disconnected_type     ev_ag_audio_disconnected;
  bt_ev_ag_audio_connect_failed_type   ev_ag_audio_connect_failed;
  bt_ev_ag_ad_spkr_gain_report_type    ev_ag_ad_spkr_gain_report;
  bt_ev_ag_ad_mic_gain_report_type     ev_ag_ad_mic_gain_report;
  bt_ev_ag_disabled_type               ev_ag_disabled;
#endif /* FEATURE_BT_AG */

  bt_ev_sd_device_discovery_resp_type  ev_sd_dev_discv_resp;
  bt_ev_sd_device_discovery_complete_type  ev_sd_dev_discv_cmplt;
  bt_ev_sd_device_name_resp_type       ev_sd_dev_name_resp;
  bt_ev_sd_dev_name_resp_type          ev_sd_dname;
  bt_ev_sd_service_search_resp_type    ev_sd_service_search_resp;
  bt_ev_sd_timeout_resp_type           ev_sd_timeout_resp;
  bt_ev_sd_error_resp_type             ev_sd_error_resp;
  bt_ev_sd_server_channel_number_resp_type  ev_sd_scn_resp;
  bt_ev_sd_discoverable_mode_resp_type ev_sd_discv_mode_resp;
  bt_ev_sd_cancel_idle_timer_resp_type ev_sd_cancel_idle_timer_resp; 
  bt_ev_sd_service_search_resp_xml_type  ev_sd_service_search_xml_resp;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

  bt_ev_spp_flush_tx_type              ev_spp_flush_tx;
  bt_ev_spp_enable_dtr_type            ev_spp_enable_dtr;
  bt_ev_spp_tx_hi_wm_type              ev_spp_tx_hi_wm;
  bt_ev_spp_tx_lo_wm_type              ev_spp_tx_lo_wm;
  bt_ev_spp_tx_gne_type                ev_spp_tx_gne;
  bt_ev_spp_tx_empty_type              ev_spp_tx_empty;
  bt_ev_spp_rx_hi_wm_type              ev_spp_rx_hi_wm;
  bt_ev_spp_rx_lo_wm_type              ev_spp_rx_lo_wm;
  bt_ev_spp_rx_gne_type                ev_spp_rx_gne;
  bt_ev_spp_rx_empty_type              ev_spp_rx_empty;
  bt_ev_spp_line_error_type            ev_spp_line_error;
  bt_ev_spp_modem_status_type          ev_spp_modem_status;
  bt_ev_spp_config_change_type         ev_spp_config_change;
  bt_ev_spp_status_change_type         ev_spp_status_change;
 
  bt_ev_rc_connected_type              ev_rc_conn;
  bt_ev_rc_disconnected_type           ev_rc_disc;
  bt_ev_rc_connection_failed_type      ev_rc_conn_failed;
  bt_ev_rc_ms_changed_type             ev_rc_ms_chgd;
  bt_ev_rc_ping_result_type            ev_rc_ping_result;
  bt_ev_rc_reconfigured_type           ev_rc_reconfig;
  bt_ev_rc_line_error_type             ev_rc_line_error;
  bt_ev_rc_unregistered_type           ev_rc_unregistered;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

  bt_ev_l2_connected_type              ev_l2_conn;
  bt_ev_l2_rx_data_type                ev_l2_rxd;
  bt_ev_l2_disconnected_type           ev_l2_disc;
  bt_ev_l2_connection_failed_type      ev_l2_conn_failed;
  bt_ev_l2_ping_type                   ev_l2_ping;
  bt_ev_l2_reconfigured_type           ev_l2_reconfig;
  bt_ev_l2_tx_data_cfm_type            ev_l2_tx_data_cfm;

  bt_ev_rm_inquiry_result_type         ev_rm_inqrs;
  bt_ev_rm_inquiry_complete_type       ev_rm_inqcp;
  bt_ev_rm_remote_name_type            ev_rm_rname;
  bt_ev_rm_connect_req_acl_type        ev_rm_creqa;
  bt_ev_rm_connected_acl_type          ev_rm_conna;
  bt_ev_rm_connection_failed_acl_type  ev_rm_confa;
  bt_ev_rm_rx_data_acl_type            ev_rm_rxda;
  bt_ev_rm_disconnected_acl_type       ev_rm_disca;
  bt_ev_rm_connect_req_sco_type        ev_rm_creqs;
  bt_ev_rm_connected_sco_type          ev_rm_conns;
  bt_ev_rm_connection_failed_sco_type  ev_rm_confs;
  bt_ev_rm_rx_data_sco_type            ev_rm_rxds;
  bt_ev_rm_disconnected_sco_type       ev_rm_discs;
  bt_ev_rm_link_status_type            ev_rm_lstat;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
  bt_ev_rm_bonded_type                 ev_rm_bond;
  bt_ev_rm_bond_failed_type            ev_rm_bondf;
  bt_ev_rm_pin_request_type            ev_rm_pinrq;
  bt_ev_rm_authorize_request_type      ev_rm_atzrq;
  bt_ev_rm_rfcomm_scn_sec_result_type  ev_rm_srscn;
  bt_ev_rm_l2cap_psm_sec_result_type   ev_rm_srpsm;
  bt_ev_rm_serv_sec_result_type        ev_rm_ssres;
  bt_ev_rm_device_updated_type         ev_rm_devud;
  bt_ev_rm_radio_disabled_type         ev_rm_radio;
  bt_ev_rm_radio_activity_type         ev_rm_ract;
  bt_ev_rm_tx_data_cfm_type            ev_rm_tx_cfm;
#ifdef FEATURE_BT_1_2
  bt_ev_rm_afh_ch_map_set_type         ev_rm_acms;
  bt_ev_rm_afh_ch_map_type             ev_rm_acm;
#endif /* FEATURE_BT_1_2 */
  bt_ev_rm_oob_local_data_type         ev_rm_oob_ld;
  bt_ev_rm_oob_remote_data_req_type    ev_rm_oob_rdreq;
  bt_ev_rm_user_confirmation_req_type  ev_rm_ucr;
  bt_ev_rm_rebond_req_type             ev_rm_rebreq;
  bt_ev_rm_keypress_notification_type  ev_rm_keypress_notif;
  bt_ev_rm_passkey_notification_type   ev_rm_passkey_notif;
  bt_ev_rm_passkey_req_type            ev_rm_passkey_req;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

  bt_ev_hc_auth_cmplt_type                ev_hc_authc;
  bt_ev_hc_chng_conn_key_cmplt_type       ev_hc_chgky;
  bt_ev_hc_cmd_cmplt_type                 ev_hc_cmd_cmplt;
  bt_ev_hc_cmd_status_type                ev_hc_cmdst;
  bt_ev_hc_conn_cmplt_type                ev_hc_concp;
  bt_ev_hc_conn_pkt_type_chng_type        ev_hc_pktch;
  bt_ev_hc_conn_request_type              ev_hc_conrq;
  bt_ev_hc_data_overflow_type             ev_hc_ovflw;
  bt_ev_hc_disc_cmplt_type                ev_hc_disc;
  bt_ev_hc_encrypt_change_type            ev_hc_encch;
  bt_ev_hc_flush_occurred_type            ev_hc_flush;
  bt_ev_hc_hw_error_type                  ev_hc_hwerr;
  bt_ev_hc_inquiry_cmplt_type             ev_hc_inqcp;
  bt_ev_hc_inquiry_result_type            ev_hc_inqrs;
  bt_ev_hc_key_notif_type                 ev_hc_keynt;
  bt_ev_hc_key_request_type               ev_hc_keyrq;
  bt_ev_hc_loopback_cmd_type              ev_hc_loopb;
  bt_ev_hc_master_key_cmplt_type          ev_hc_mstrk;
  bt_ev_hc_max_slots_change_type          ev_hc_maxsl;
  bt_ev_hc_mode_change_type               ev_hc_modec;
  bt_ev_hc_num_cmplt_pkts_type            ev_hc_numpk;
  bt_ev_hc_pin_request_type               ev_hc_pinrq;
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
  bt_ev_hc_ps_mode_chng_type              ev_hc_psmc;
#endif
  bt_ev_hc_ps_rep_mode_chng_type          ev_hc_psrmc;
  bt_ev_hc_qos_setup_cmplt_type           ev_hc_qoscp;
  bt_ev_hc_qos_violation_type             ev_hc_qosvl;
#ifndef FEATURE_BT_SOC
  bt_ev_hc_qualcomm_type                  ev_hc_qc;
#endif /* !FEATURE_BT_SOC */
  bt_ev_hc_rd_clk_offset_cmplt_type       ev_hc_clock;
  bt_ev_hc_rd_remote_feat_cmplt_type      ev_hc_rfeat;
  bt_ev_hc_rd_remote_ver_cmplt_type       ev_hc_rvers;
  bt_ev_hc_remote_name_req_cmplt_type     ev_hc_rname;
  bt_ev_hc_ret_key_type                   ev_hc_retky;
  bt_ev_hc_role_change_type               ev_hc_role;
  bt_ev_hc_rx_acl_data_type               ev_hc_rxda;
  bt_ev_hc_rx_sco_data_type               ev_hc_rxds;
  bt_ev_hc_tx_data_cfm_type               ev_hc_tx_cfm;
  bt_ev_hc_qc_radio_activity_changed      ev_hc_ract_chgd;
  bt_ev_hc_qc_cur_conn_encrypt_type       ev_hc_ccenc;
#ifdef  FEATURE_BT_1_2
  bt_ev_hc_inquiry_result_with_rssi_type  ev_hc_inqrs_wrssi;
  bt_ev_hc_rd_remote_ext_feat_cmplt_type  ev_hc_rextf;
  bt_ev_hc_sync_conn_cmplt_type           ev_hc_sccmp;
  bt_ev_hc_sync_conn_change_type          ev_hc_scchg;
#endif
#ifdef FEATURE_BT_2_1
  bt_ev_hc_inquiry_result_extended_type   ev_hc_inqrs_extended;
  bt_ev_hc_enh_flush_cmplt_type           ev_hc_eflush;
  bt_ev_hc_enc_key_ref_complt_type        ev_hc_enc_key_rc;
  bt_ev_hc_io_cap_rqst_type               ev_hc_io_cap_rqst;
  bt_ev_hc_io_cap_res_type                ev_hc_io_cap_res;
  bt_ev_hc_user_cfmtn_rqst_type           ev_hc_user_cfm_rqst;
  bt_ev_hc_user_passkey_rqst_type         ev_hc_user_pk_rqst;
  bt_ev_hc_remote_oob_data_rqst_type      ev_hc_rem_oob_data_rqst;
  bt_ev_hc_sp_complt_type                 ev_hc_sp_complt;
  bt_ev_hc_user_passkey_ntfctn_type       ev_hc_user_pk_ntfctn;
  bt_ev_hc_keypress_ntfctn_type           ev_hc_kp_ntfctn;
  bt_ev_hc_rem_host_sup_feat_ntfctn_type  ev_hc_rem_host_sf_ntfctn;
  bt_ev_hc_sniff_subrating_type           ev_hc_ssr;
  bt_ev_hc_lk_sup_to_changed_type         ev_hc_lk_sup_to;
#endif /* FEATURE_BT_2_1 */

#ifdef FEATURE_BT_SOC1
#error code not present
#endif

#ifdef FEATURE_BT_QSOC
  bt_ev_hc_vs_var_type                    ev_hc_vs_var_hdr;
  bt_ev_hc_vs_cmd_cmplt_type              ev_hc_vs_cmd_cmplt_hdr;
#endif

#ifdef FEATURE_BT_EXTPF
#ifdef FEATURE_BT_EXTPF_GOEP
  bt_pf_ev_goep_cli_con_cfm_type               ev_goep_cli_con_cfm;
  bt_pf_ev_goep_cli_dcn_ind_type               ev_goep_cli_dcn_ind; 
  bt_pf_ev_goep_cli_put_cfm_type               ev_goep_cli_put_cfm;
  bt_pf_ev_goep_cli_get_rcv_data_type          ev_goep_cli_get_rcv_data;
  bt_pf_ev_goep_cli_set_path_cfm_type          ev_goep_cli_set_path_cfm;
  bt_pf_ev_goep_cli_auth_ind_type              ev_goep_cli_auth_ind;
  bt_pf_ev_goep_cli_auth_res_ind_type          ev_goep_cli_auth_res_ind;
  bt_pf_ev_goep_srv_con_ind_type               ev_goep_srv_con_ind;
  bt_pf_ev_goep_srv_dcn_ind_type               ev_goep_srv_dcn_ind;
  bt_pf_ev_goep_srv_get_ind_type               ev_goep_srv_get_ind;
  bt_pf_ev_goep_srv_put_ind_type               ev_goep_srv_put_ind;
  bt_pf_ev_goep_srv_set_path_ind_type          ev_goep_srv_set_path_ind;
  bt_pf_ev_goep_srv_auth_req_ind_type          ev_goep_srv_auth_req_ind;
#endif /* FEATURE_BT_EXTPF_GOEP */

#ifdef FEATURE_BT_EXTPF_OPP
  bt_pf_ev_opp_cli_con_cfm_type                ev_opp_cli_con_cfm;
  bt_pf_ev_opp_cli_con_prog_ind_type           ev_opp_cli_con_prog_ind;
  bt_pf_ev_opp_cli_dcn_cfm_type                ev_opp_cli_dcn_cfm;
  bt_pf_ev_opp_cli_push_done_type              ev_opp_cli_push_done;
  bt_pf_ev_opp_cli_pull_done_type              ev_opp_cli_pull_done;
  bt_pf_ev_opp_cli_open_read_req_type          ev_opp_cli_open_read_req;
  bt_pf_ev_opp_cli_open_write_req_type         ev_opp_cli_open_write_req;
  bt_pf_ev_opp_cli_close_req_type              ev_opp_cli_close_req;
  bt_pf_ev_opp_cli_read_req_type               ev_opp_cli_read_req;
  bt_pf_ev_opp_cli_write_req_type              ev_opp_cli_write_req;

  bt_pf_ev_opp_srv_con_ind_type                ev_opp_srv_con_ind;
  bt_pf_ev_opp_srv_dcn_ind_type                ev_opp_srv_dcn_ind;
  bt_pf_ev_opp_srv_open_read_req_type          ev_opp_srv_open_read_req;
  bt_pf_ev_opp_srv_open_write_req_type         ev_opp_srv_open_write_req;
  bt_pf_ev_opp_srv_close_req_type              ev_opp_srv_close_req;
  bt_pf_ev_opp_srv_read_req_type               ev_opp_srv_read_req;
  bt_pf_ev_opp_srv_write_req_type              ev_opp_srv_write_req;
#endif /* FEATURE_BT_EXTPF_OPP */

#ifdef FEATURE_BT_EXTPF_FTP
  bt_pf_ev_ftp_cli_con_cfm_type                ev_ftp_cli_con_cfm;
  bt_pf_ev_ftp_cli_con_read_only_cfm_type      ev_ftp_cli_con_read_only_cfm;
  bt_pf_ev_ftp_cli_dcn_cfm_type                ev_ftp_cli_dcn_cfm;
  bt_pf_ev_ftp_cli_auth_req_type               ev_ftp_cli_auth_req;
  bt_pf_ev_ftp_cli_put_dne_type                ev_ftp_cli_put_dne;
  bt_pf_ev_ftp_cli_delete_dne_type             ev_ftp_cli_delete_dne;
  bt_pf_ev_ftp_cli_get_rsp_type                ev_ftp_cli_get_rsp;
  bt_pf_ev_ftp_cli_set_path_dne_type           ev_ftp_cli_set_path_dne;
  bt_pf_ev_ftp_cli_new_subfolder_dne_type      ev_ftp_cli_new_subfolder_dne;
  bt_pf_ev_ftp_cli_get_folder_list_rsp_type    ev_ftp_cli_get_folder_list_rsp;
  bt_pf_ev_ftp_cli_tree_put_dne_type           ev_ftp_cli_tree_put_dne;
  bt_pf_ev_ftp_cli_tree_get_rsp_type           ev_ftp_cli_tree_get_rsp;
  bt_pf_ev_ftp_cli_browse_req_type             ev_ftp_cli_browse_req;
  bt_pf_ev_ftp_cli_open_req_type               ev_ftp_cli_open_req;
  bt_pf_ev_ftp_cli_close_req_type              ev_ftp_cli_close_req;
  bt_pf_ev_ftp_cli_read_req_type               ev_ftp_cli_read_req;
  bt_pf_ev_ftp_cli_write_req_type              ev_ftp_cli_write_req;
  bt_pf_ev_ftp_cli_set_folder_req_type         ev_ftp_cli_set_folder_req;

  bt_pf_ev_ftp_srv_con_ind_type                ev_ftp_srv_con_ind;
  bt_pf_ev_ftp_srv_dcn_ind_type                ev_ftp_srv_dcn_ind;
  bt_pf_ev_ftp_srv_browse_req_type             ev_ftp_srv_browse_req;
  bt_pf_ev_ftp_srv_open_req_type               ev_ftp_srv_open_req;
  bt_pf_ev_ftp_srv_close_req_type              ev_ftp_srv_close_req;
  bt_pf_ev_ftp_srv_read_req_type               ev_ftp_srv_read_req;
  bt_pf_ev_ftp_srv_write_req_type              ev_ftp_srv_write_req;
  bt_pf_ev_ftp_srv_delete_req_type             ev_ftp_srv_delete_req;
  bt_pf_ev_ftp_srv_set_folder_req_type         ev_ftp_srv_set_folder_req;
#endif /* FEATURE_BT_EXTPF_FTP */

#ifdef FEATURE_BT_EXTPF_BIP
  bt_pf_ev_bip_cli_con_cfm_type                ev_bip_cli_con_cfm;
  bt_pf_ev_bip_cli_dcn_cfm_type                ev_bip_cli_dcn_cfm;
  bt_pf_ev_bip_cli_auth_req_type               ev_bip_cli_auth_req;
  bt_pf_ev_bip_cli_get_capabilities_cfm_type   ev_bip_cli_get_capabilities_cfm;
  bt_pf_ev_bip_cli_put_image_cfm_type          ev_bip_cli_put_image_cfm;
  bt_pf_ev_bip_cli_put_thumbnail_cfm_type      ev_bip_cli_put_thumbnail_cfm;
  bt_pf_ev_bip_cli_put_attachment_cfm_type     ev_bip_cli_put_attachment_cfm;
  bt_pf_ev_bip_cli_get_list_cfm_type           ev_bip_cli_get_list_cfm;
  bt_pf_ev_bip_cli_get_properties_cfm_type     ev_bip_cli_get_properties_cfm;
  bt_pf_ev_bip_cli_get_image_cfm_type          ev_bip_cli_get_image_cfm;
  bt_pf_ev_bip_cli_get_thumbnail_cfm_type      ev_bip_cli_get_thumbnail_cfm;
  bt_pf_ev_bip_cli_get_attachment_cfm_type     ev_bip_cli_get_attachment_cfm;
  bt_pf_ev_bip_cli_delete_image_cfm_type       ev_bip_cli_delete_image_cfm;
  bt_pf_ev_bip_cli_get_status_cfm_type         ev_bip_cli_get_status_cfm;
  bt_pf_ev_bip_cli_get_partial_image_cfm_type  ev_bip_cli_get_partial_image_cfm;
  bt_pf_ev_bip_cli_start_print_cfm_type        ev_bip_cli_start_print_cfm;
  bt_pf_ev_bip_cli_start_archive_cfm_type      ev_bip_cli_start_archive_cfm;
  bt_pf_ev_bip_cli_remote_display_cfm_type     ev_bip_cli_remote_display_cfm;
  bt_pf_ev_bip_cli_get_monitor_image_cfm_type  ev_bip_cli_get_monitor_image_cfm;

  bt_pf_ev_bip_srv_con_ind_type                ev_bip_srv_con_ind;
  bt_pf_ev_bip_srv_dcn_ind_type                ev_bip_srv_dcn_ind;
  bt_pf_ev_bip_srv_auth_ind_type               ev_bip_srv_auth_ind;
  bt_pf_ev_bip_srv_get_capabilities_ind_type   ev_bip_srv_get_capabilities_ind;
  bt_pf_ev_bip_srv_put_image_ind_type          ev_bip_srv_put_image_ind;
  bt_pf_ev_bip_srv_put_thumbnail_ind_type      ev_bip_srv_put_thumbnail_ind;
  bt_pf_ev_bip_srv_put_attachment_ind_type     ev_bip_srv_put_attachment_ind;
  bt_pf_ev_bip_srv_get_list_ind_type           ev_bip_srv_get_list_ind;
  bt_pf_ev_bip_srv_get_properties_ind_type     ev_bip_srv_get_properties_ind;
  bt_pf_ev_bip_srv_get_image_ind_type          ev_bip_srv_get_image_ind;
  bt_pf_ev_bip_srv_get_thumbnail_ind_type      ev_bip_srv_get_thumbnail_ind;
  bt_pf_ev_bip_srv_get_attachment_ind_type     ev_bip_srv_get_attachment_ind;
  bt_pf_ev_bip_srv_delete_image_ind_type       ev_bip_srv_delete_image_ind;
  bt_pf_ev_bip_srv_get_status_ind_type         ev_bip_srv_get_status_ind;
  bt_pf_ev_bip_srv_get_partial_image_ind_type  ev_bip_srv_get_partial_image_ind; 
  bt_pf_ev_bip_srv_start_archive_ind_type      ev_bip_srv_start_archive_ind;
  bt_pf_ev_bip_srv_remote_display_ind_type     ev_bip_srv_remote_display_ind;
  bt_pf_ev_bip_srv_get_monitor_image_ind_type  ev_bip_srv_get_monitor_image_ind;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_BIP */

#ifdef FEATURE_BT_EXTPF_BPP
  bt_pf_ev_bpp_sndr_con_cfm_type               ev_bpp_sndr_con_cfm;
  bt_pf_ev_bpp_sndr_con_ind_type               ev_bpp_sndr_con_ind;
  bt_pf_ev_bpp_sndr_dcn_ind_type               ev_bpp_sndr_dcn_ind;
  bt_pf_ev_bpp_sndr_srv_dcn_ind_type           ev_bpp_sndr_srv_dcn_ind;
  bt_pf_ev_bpp_sndr_auth_req_type              ev_bpp_sndr_auth_req;
  bt_pf_ev_bpp_sndr_send_file_cfm_type         ev_bpp_sndr_send_file_cfm;
  bt_pf_ev_bpp_sndr_send_reference_cfm_type    ev_bpp_sndr_send_reference_cfm;
  bt_pf_ev_bpp_sndr_get_cfm_type               ev_bpp_sndr_get_cfm;
  bt_pf_ev_bpp_sndr_get_ref_obj_ind_type       ev_bpp_sndr_get_ref_obj_ind;
#endif /* FEATURE_BT_EXTPF_BPP */

#ifdef FEATURE_BT_EXTPF_SML
  bt_pf_ev_sml_cli_con_ind_type                ev_sml_cli_con_ind;
  bt_pf_ev_sml_cli_dcn_ind_type                ev_sml_cli_dcn_ind;
  bt_pf_ev_sml_cli_get_ind_type                ev_sml_cli_get_ind;
  bt_pf_ev_sml_cli_put_ind_type                ev_sml_cli_put_ind;
  bt_pf_ev_sml_srv_con_ind_type                ev_sml_srv_con_ind;
  bt_pf_ev_sml_srv_dcn_ind_type                ev_sml_srv_dcn_ind;
  bt_pf_ev_sml_srv_get_ind_type                ev_sml_srv_get_ind;
  bt_pf_ev_sml_srv_put_ind_type                ev_sml_srv_put_ind;
  bt_pf_ev_sml_con_cfm_type                    ev_sml_con_cfm;
  bt_pf_ev_sml_dcn_cfm_type                    ev_sml_dcn_cfm;
  bt_pf_ev_sml_auth_ind_type                   ev_sml_auth_ind;
  bt_pf_ev_sml_get_rcv_data_type               ev_sml_get_rcv_data;
  bt_pf_ev_sml_put_cfm_type                    ev_sml_put_cfm;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_SML */

#ifdef FEATURE_BT_EXTPF_CTP
  bt_pf_ev_ctp_connect_ind_type                ev_ctp_connect_ind;
  bt_pf_ev_ctp_connect_cfm_type                ev_ctp_connect_cfm;
  bt_pf_ev_ctp_disconnect_cfm_type             ev_ctp_disconnect_cfm;
  bt_pf_ev_ctp_establish_ind_type              ev_ctp_establish_ind;
  bt_pf_ev_ctp_establish_cfm_type              ev_ctp_establish_cfm;
  bt_pf_ev_ctp_info_ind_type                   ev_ctp_info_ind;
  bt_pf_ev_ctp_dtmf_start_type                 ev_ctp_dtmf_start;
  bt_pf_ev_ctp_dtmf_stop_type                  ev_ctp_dtmf_stop;
  bt_pf_ev_ctp_release_ind_type                ev_ctp_release_ind;
  bt_pf_ev_ctp_outgoing_status_type            ev_ctp_outgoing_status;

  bt_pf_ev_icp_connect_ind_type                ev_icp_connect_ind;
  bt_pf_ev_icp_connect_cfm_type                ev_icp_connect_cfm;
  bt_pf_ev_icp_disconnect_cfm_type             ev_icp_disconnect_cfm;
  bt_pf_ev_icp_establish_ind_type              ev_icp_establish_ind;
  bt_pf_ev_icp_establish_cfm_type              ev_icp_establish_cfm;
  bt_pf_ev_icp_info_ind_type                   ev_icp_info_ind;
  bt_pf_ev_icp_dtmf_start_type                 ev_icp_dtmf_start;
  bt_pf_ev_icp_dtmf_stop_type                  ev_icp_dtmf_stop;
  bt_pf_ev_icp_release_ind_type                ev_icp_release_ind;
  bt_pf_ev_icp_outgoing_status_type            ev_icp_outgoing_status;
#endif /* FEATURE_BT_EXTPF_CTP */

#if (defined( FEATURE_BT_EXTPF_SAP ) || defined( FEATURE_EXPORT_BT ))
  bt_pf_ev_sap_con_req_type                    ev_sap_con_req;
  bt_pf_ev_sap_con_result_type                 ev_sap_con_result;
  bt_pf_ev_sap_dcn_ind_type                    ev_sap_dcn_ind;
  bt_pf_ev_sap_tx_apdu_rsp_type                ev_sap_tx_apdu_rsp;
  bt_pf_ev_sap_tx_apdu_req_type                ev_sap_tx_apdu_req;
  bt_pf_ev_sap_tx_atr_rsp_type                 ev_sap_tx_atr_rsp;
  bt_pf_ev_sap_pwr_sim_off_rsp_type            ev_sap_pwr_sim_off_rsp;
  bt_pf_ev_sap_pwr_sim_on_rsp_type             ev_sap_pwr_sim_on_rsp;
  bt_pf_ev_sap_reset_sim_rsp_type              ev_sap_reset_sim_rsp;
  bt_pf_ev_sap_card_reader_status_rsp_type     ev_sap_card_reader_status_rsp;
  bt_pf_ev_sap_status_ind_type                 ev_sap_status_ind;
#endif /* FEATURE_BT_EXTPF_SAP || FEATURE_EXPORT_BT */

#ifdef FEATURE_BT_EXTPF_AV
  /* A2DP events */
  bt_pf_ev_a2dp_con_type                       ev_a2dp_con;
  bt_pf_ev_a2dp_con_failed_type                ev_a2dp_con_failed;
  bt_pf_ev_a2dp_discon_type                    ev_a2dp_discon;
  bt_pf_ev_a2dp_start_type                     ev_a2dp_start;
  bt_pf_ev_a2dp_suspend_type                   ev_a2dp_suspend;
  bt_pf_ev_a2dp_open_type                      ev_a2dp_open;
  bt_pf_ev_a2dp_close_type                     ev_a2dp_close;
  bt_pf_ev_a2dp_bitrate_type                   ev_a2dp_bitrate;

  /* AVRCP events */
  bt_pf_ev_avrcp_con_type                      ev_avrcp_con;
  bt_pf_ev_avrcp_con_failed_type               ev_avrcp_con_failed;
  bt_pf_ev_avrcp_discon_type                   ev_avrcp_discon;
  bt_pf_ev_avrcp_op_type                       ev_avrcp_op;
  bt_pf_ev_avrcp_generic_op_type               ev_avrcp_generic_op;

#ifdef FEATURE_BT_AVRCP_13
  /* AVRCP meta data event structures */
  bt_pf_ev_avrcp_capability_command_ind_type                          ev_avrcp_capability_command;
  bt_pf_ev_avrcp_list_attributes_command_ind_type                     ev_avrcp_list_attributes_command;  
  bt_pf_ev_avrcp_list_attribute_values_command_ind_type               ev_avrcp_list_attribute_values_command;
  bt_pf_ev_avrcp_get_attribute_values_command_ind_type                ev_avrcp_get_attribute_values_command;
  bt_pf_ev_avrcp_set_attribute_values_command_ind_type                ev_avrcp_set_attribute_values_command;
  bt_pf_ev_avrcp_get_attribute_setting_text_command_ind_type          ev_get_attribute_setting_text_command;
  bt_pf_ev_avrcp_get_attribute_setting_value_text_command_ind_type    ev_avrcp_get_attribute_setting_value_text_command;
  bt_pf_ev_avrcp_inform_character_set_command_ind_type                ev_avrcp_inform_character_set_command;
  bt_pf_ev_avrcp_inform_battery_status_of_ct_command_ind_type         ev_avrcp_inform_battery_status_of_ct_command;
  bt_pf_ev_avrcp_get_element_attribute_command_ind_type               ev_avrcp_get_element_attribute_command;
  bt_pf_ev_avrcp_get_play_status_command_ind_type                     ev_avrcp_get_play_status_command;
  bt_pf_ev_avrcp_event_notification_command_ind_type                  ev_avrcp_event_notification_command;
  bt_pf_ev_avrcp_continue_response_command_ind_type                   ev_avrcp_continue_response_command;
  bt_pf_ev_avrcp_abort_response_command_ind_type                      ev_avrcp_abort_response_command;
  bt_pf_ev_avrcp_group_navigation_command_ind_type                    ev_avrcp_group_navigation_command;
#endif /* FEATURE_BT_AVRCP_13 */
// OI_AVDTP_SET_CONFIG_IND
  /* AVDTP events */
  bt_ev_pf_avdtp_con_ind_type                  ev_avdtp_con_ind;
  bt_ev_pf_avdtp_con_cfm_type                  ev_avdtp_con_cfm;
  bt_ev_pf_avdtp_discon_ind_type               ev_avdtp_discon_ind;
  bt_ev_pf_avdtp_disc_cfm_type                 ev_avdtp_disc_cfm;
  bt_ev_pf_avdtp_get_cap_cfm_type              ev_avdtp_get_cap_cfm;
  bt_ev_pf_avdtp_set_cfg_ind_type              ev_avdtp_set_cfg_ind;
  bt_ev_pf_avdtp_set_cfg_cfm_type              ev_avdtp_set_cfg_cfm;
  bt_ev_pf_avdtp_get_cfg_cfm_type              ev_avdtp_get_cfg_cfm;
  bt_ev_pf_avdtp_open_cfm_type                 ev_avdtp_open_cfm;
  bt_ev_pf_avdtp_open_ind_type                 ev_avdtp_open_ind;
  bt_ev_pf_avdtp_close_ind_type                ev_avdtp_close_ind;
  bt_ev_pf_avdtp_close_cfm_type                ev_avdtp_close_cfm;
  bt_ev_pf_avdtp_start_ind_type                ev_avdtp_start_ind;
  bt_ev_pf_avdtp_start_cfm_type                ev_avdtp_start_cfm;
  bt_ev_pf_avdtp_suspend_ind_type              ev_avdtp_suspend_ind;
  bt_ev_pf_avdtp_suspend_cfm_type              ev_avdtp_suspend_cfm;
  bt_ev_pf_avdtp_recfg_ind_type                ev_avdtp_recfg_ind;
  bt_ev_pf_avdtp_recfg_cfm_type                ev_avdtp_recfg_cfm;
  bt_ev_pf_avdtp_abort_ind_type                ev_avdtp_abort_ind;
  bt_ev_pf_avdtp_abort_cfm_type                ev_avdtp_abort_cfm;
  bt_ev_pf_avdtp_sec_ind_type                  ev_avdtp_sec_ind;
  bt_ev_pf_avdtp_sec_cfm_type                  ev_avdtp_sec_cfm;
  bt_ev_pf_avdtp_recv_data_type                ev_avdtp_recv_data;
  bt_ev_pf_avdtp_send_cfm_type                 ev_avdtp_send_cfm;

#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_PBAP
  bt_pf_ev_pbap_cli_con_cfm_type                ev_pbap_cli_con_cfm;
  bt_pf_ev_pbap_cli_dcn_cfm_type                ev_pbap_cli_dcn_cfm;
  bt_pf_ev_pbap_cli_auth_req_type               ev_pbap_cli_auth_req;
  bt_pf_ev_pbap_cli_pull_phone_book_cfm_type    ev_pbap_cli_pull_phone_book_cfm;
  bt_pf_ev_pbap_cli_set_phone_book_cfm_type     ev_pbap_cli_set_phone_book_cfm;
  bt_pf_ev_pbap_cli_pull_vcard_listing_cfm_type ev_pbap_cli_pull_vcard_listing_cfm;
  bt_pf_ev_pbap_cli_pull_vcard_entry_cfm_type   ev_pbap_cli_pull_vcard_entry_cfm;

  bt_pf_ev_pbap_srv_con_ind_type                ev_pbap_srv_con_ind;
  bt_pf_ev_pbap_srv_dcn_ind_type                ev_pbap_srv_dcn_ind;
  bt_pf_ev_pbap_srv_auth_ind_type               ev_pbap_srv_auth_ind;
  bt_pf_ev_pbap_srv_pull_phone_book_ind_type    ev_pbap_srv_pull_phone_book_ind;
  bt_pf_ev_pbap_srv_set_phone_book_ind_type     ev_pbap_srv_set_phone_book_ind;
  bt_pf_ev_pbap_srv_pull_vcard_listing_ind_type ev_pbap_srv_pull_vcard_listing_ind;
  bt_pf_ev_pbap_srv_pull_vcard_entry_ind_type   ev_pbap_srv_pull_vcard_entry_ind;
#endif /* FEATURE_BT_EXTPF_PBAP */

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#endif /* FEATURE_BT_EXTPF */
} bt_ev_params_union;

#ifdef FEATURE_HTORPC_METACOMMENTS
  /*~ PARTIAL bt_ev_params_union */
  /*~ CASE BT_EV_GN_CMD_DONE bt_ev_params_union.ev_gn_cmd_done */
/* AG EVENTS */
#ifdef FEATURE_BT_AG
/*~ CASE BT_EV_AG_ENABLED bt_ev_params_union.ev_ag_enabled */
/*~ CASE BT_EV_AG_CONNECTED bt_ev_params_union.ev_ag_connected */
/*~ CASE BT_EV_AG_CONNECTION_FAILED bt_ev_params_union.ev_ag_connection_failed */
/*~ CASE BT_EV_AG_DISCONNECTED bt_ev_params_union.ev_ag_disconnected */
/*~ CASE BT_EV_AG_BUTTON_PRESSED bt_ev_params_union.ev_ag_button_pressed */
/*~ CASE BT_EV_AG_SLC_CHANGED bt_ev_params_union.ev_ag_slc_changed */
/*~ CASE BT_EV_AG_DEV_PICKUP bt_ev_params_union.ev_ag_dev_pickup */
/*~ CASE BT_EV_AG_DEV_HANGUP bt_ev_params_union.ev_ag_dev_hangup */
/*~ CASE BT_EV_AG_DEV_DIAL bt_ev_params_union.ev_ag_dev_dial */
/*~ CASE BT_EV_AG_DEV_REDIAL bt_ev_params_union.ev_ag_dev_redial */
/*~ CASE BT_EV_AG_DEV_MEM_DIAL bt_ev_params_union.ev_ag_dev_mem_dial */
/*~ CASE BT_EV_AG_DEV_SEND_DTMF bt_ev_params_union.ev_ag_dev_send_dtmf */
/*~ CASE BT_EV_AG_VR_TOGGLE bt_ev_params_union.ev_ag_vr_state */
/*~ CASE BT_EV_AG_RING_FAILED bt_ev_params_union.ev_ag_ring_failed */
/*~ CASE BT_EV_AG_AUDIO_CONNECTED bt_ev_params_union.ev_ag_audio_connected */
/*~ CASE BT_EV_AG_AUDIO_CONNECT_FAILED bt_ev_params_union.ev_ag_audio_connect_failed */
/*~ CASE BT_EV_AG_AUDIO_DISCONNECTED bt_ev_params_union.ev_ag_audio_disconnected */
/*~ CASE BT_EV_AG_SPKR_GAIN_REPORT bt_ev_params_union.ev_ag_ad_spkr_gain_report */
/*~ CASE BT_EV_AG_MIC_GAIN_REPORT bt_ev_params_union.ev_ag_ad_mic_gain_report */
/*~ CASE BT_EV_AG_BTRH_RCVD bt_ev_params_union.ev_ag_btrh */
/*~ CASE BT_EV_AG_DISABLED bt_ev_params_union.ev_ag_disabled */
#endif /* FEATURE_BT_AG */
/* SD EVENTS */
/*~ CASE BT_EV_SD_DEVICE_DISCOVERY_RESP bt_ev_params_union.ev_sd_dev_discv_resp */
/*~ CASE BT_EV_SD_DEVICE_DISCOVERY_COMPLETE bt_ev_params_union.ev_sd_dev_discv_cmplt */
/*~ CASE BT_EV_SD_DEVICE_NAME_RESP bt_ev_params_union.ev_sd_dev_name_resp */
/*~ CASE BT_EV_SD_SERVICE_SEARCH_RESP bt_ev_params_union.ev_sd_service_search_resp */
/*~ CASE BT_EV_SD_TIMEOUT_RESP bt_ev_params_union.ev_sd_timeout_resp */
/*~ CASE BT_EV_SD_ERROR_RESP bt_ev_params_union.ev_sd_error_resp */
/*~ CASE BT_EV_SD_SERVER_CHANNEL_NUMBER_RESP bt_ev_params_union.ev_sd_scn_resp */
/*~ CASE BT_EV_SD_SECONDARY_SERVER_CHANNEL_NUMBER_RESP bt_ev_params_union.ev_sd_scn_resp */
/*~ CASE BT_EV_SD_DISCOVERABLE_MODE_RESP bt_ev_params_union.ev_sd_discv_mode_resp */
/* RM EVENTS */
/*~ CASE BT_EV_RM_INQUIRY_RESULT bt_ev_params_union.ev_rm_inqrs */
/*~ CASE BT_EV_RM_INQUIRY_COMPLETE bt_ev_params_union.ev_rm_inqcp */
  /*~ CASE BT_EV_RM_BONDED bt_ev_params_union.ev_rm_bond */
  /*~ CASE BT_EV_RM_BOND_FAILED bt_ev_params_union.ev_rm_bondf */
/*~ CASE BT_EV_RM_DEVICE_UPDATED bt_ev_params_union.ev_rm_devud */
  /*~ CASE BT_EV_RM_PIN_REQUEST bt_ev_params_union.ev_rm_pinrq */
/*~ CASE BT_EV_RM_AUTHORIZE_REQUEST bt_ev_params_union.ev_rm_atzrq */
/*~ CASE BT_EV_RM_LINK_STATUS bt_ev_params_union.ev_rm_lstat */
/*~ CASE BT_EV_RM_RADIO_DISABLED bt_ev_params_union.ev_rm_radio */
/*~ CASE BT_EV_RM_SERVICE_SEC_RESULT bt_ev_params_union.ev_rm_ssres */
/*~ CASE BT_EV_RM_RADIO_ACTIVITY bt_ev_params_union.ev_rm_ract */
/*~ CASE BT_EV_RM_RFCOMM_SCN_SEC_RESULT bt_ev_params_union.ev_rm_srscn */
/*~ CASE BT_EV_RM_L2CAP_PSM_SEC_RESULT bt_ev_params_union.ev_rm_srpsm */
#if (defined( FEATURE_BT_EXTPF_SAP ) || defined( FEATURE_EXPORT_BT ))
  /*~ CASE BT_EV_PF_SAP_CON_REQ bt_ev_params_union.ev_sap_con_req */
  /*~ CASE BT_EV_PF_SAP_CON_RSP bt_ev_params_union.ev_sap_con_result */
  /*~ CASE BT_EV_PF_SAP_CON_CFM bt_ev_params_union.ev_sap_con_result */
  /*~ CASE BT_EV_PF_SAP_DCN_IND bt_ev_params_union.ev_sap_dcn_ind */
  /*~ CASE BT_EV_PF_SAP_TX_APDU_RSP bt_ev_params_union.ev_sap_tx_apdu_rsp */
  /*~ CASE BT_EV_PF_SAP_TX_APDU_REQ bt_ev_params_union.ev_sap_tx_apdu_req */
  /*~ CASE BT_EV_PF_SAP_TX_ATR_RSP bt_ev_params_union.ev_sap_tx_atr_rsp */
  /*~ CASE BT_EV_PF_SAP_PWR_SIM_OFF_RSP bt_ev_params_union.ev_sap_pwr_sim_off_rsp */
  /*~ CASE BT_EV_PF_SAP_PWR_SIM_ON_RSP bt_ev_params_union.ev_sap_pwr_sim_on_rsp */
  /*~ CASE BT_EV_PF_SAP_RESET_SIM_RSP bt_ev_params_union.ev_sap_reset_sim_rsp */
  /*~ CASE BT_EV_PF_SAP_CARD_READER_STATUS_RSP bt_ev_params_union.ev_sap_card_reader_status_rsp */
  /*~ CASE BT_EV_PF_SAP_STATUS_IND bt_ev_params_union.ev_sap_status_ind */
  /*~ CASE BT_EV_PF_SAP_TX_ATR_REQ bt_ev_params_union.void */
  /*~ CASE BT_EV_PF_SAP_PWR_SIM_ON_REQ bt_ev_params_union.void */
  /*~ CASE BT_EV_PF_SAP_PWR_SIM_OFF_REQ bt_ev_params_union.void */
  /*~ CASE BT_EV_PF_SAP_RESET_SIM_REQ bt_ev_params_union.void */
  /*~ CASE BT_EV_PF_SAP_CARD_READER_STATUS_REQ bt_ev_params_union.void */
#endif /* FEATURE_BT_EXTPF_SAP || FEATURE_EXPORT_BT */
#ifdef FEATURE_BT_EXTPF_AV
/* A2DP Events */
/*~ CASE BT_EV_PF_A2DP_CON bt_ev_params_union.ev_a2dp_con */
/*~ CASE BT_EV_PF_A2DP_CON_FAILED bt_ev_params_union.ev_a2dp_con_failed */
/*~ CASE BT_EV_PF_A2DP_DISCON bt_ev_params_union.ev_a2dp_discon */
/*~ CASE BT_EV_PF_A2DP_START bt_ev_params_union.ev_a2dp_start */
/*~ CASE BT_EV_PF_A2DP_SUSPEND bt_ev_params_union.ev_a2dp_suspend */
/*~ CASE BT_EV_PF_A2DP_OPEN bt_ev_params_union.ev_a2dp_open */
/*~ CASE BT_EV_PF_A2DP_CLOSE bt_ev_params_union.ev_a2dp_close */
/*~ CASE BT_EV_PF_A2DP_BITRATE bt_ev_params_union.ev_a2dp_bitrate */
/* AVRCP Events */
/*~ CASE BT_EV_PF_AVRCP_CON bt_ev_params_union.ev_avrcp_con */
/*~ CASE BT_EV_PF_AVRCP_CON_FAILED bt_ev_params_union.ev_avrcp_con_failed */
/*~ CASE BT_EV_PF_AVRCP_DISCON bt_ev_params_union.ev_avrcp_discon */
/*~ IF ( BT_EV_PF_AVRCP_OP_SELECT <= _DISC_ && _DISC_ <= BT_EV_PF_AVRCP_OP_F5 ) 
    bt_ev_params_union.ev_avrcp_op */
#endif /* FEATURE_BT_EXTPF_AV */
  /*~ DEFAULT bt_ev_params_union.void */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef struct bt_ev_msg_struct
{
  bt_ev_hdr_type      ev_hdr;
  bt_ev_params_union  ev_msg;
} bt_ev_msg_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD bt_ev_msg_struct.ev_msg DISC _OBJ_.ev_hdr.ev_type */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT */

#endif /* _BT_H */



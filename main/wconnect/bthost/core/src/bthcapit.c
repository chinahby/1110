/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 B L U E T O O T H    A P I    T E S T E R
                           H C I    P A R S E R

GENERAL DESCRIPTION
  This module contains the Bluetooth API Tester HCI Parser.

Copyright (c) 2006-2009 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/bthcapit.c_v   1.31   07 Jun 2002 17:03:36   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/bthcapit.c#3 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/bthcapit.c_v  $
   when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2009-05-22   pn  Fixed compile errors when selective features are undefined.
 2009-04-14   pn  Watermark queues get initialized by dsm_queue_init().
 2008-09-09   NM  Apitize retrieval of link key from target for automation
 *
 *    #51        24 JUL 2007            PG
 * SD Get Device Name Cancel support.
 *
 *    #50        09 JUL 2007            PG
 * HCI Remote name req cancel and connection cancel support.
 *
 *    #49        22 May 2007            SSK
 * Fix compiler warnings. 
 *
 *    #48        22 Dec 2006            VK
 * Added coping of 128 bit uuid info in BT_CMD_SD_SEARCH_SERVICE cmd.
 *
 *    #47        22 Dec 2006            PG
 * Fixed RVCT compile error.
 *
 *    #46        18 Dec 2006            GB
 * Added support for bt_cmd_ag_connect() and bt_cmd_ag_disconnect()
 *
 *    #45        14 Nov 2006            RY
 * Added SD support for custom 128-bit UUID
 *
 *    #44        26 Oct 2006            VK
 * Added one parameter in function bt_cmd_rm_enforce_security_l2cap_psm call.
 *
 *    #43        09 Oct 2006            GS
 * Fixing compilation issues in enabling APIT and BT 1.2 feature 
 * on 7K target.
 *
 *    #42        14 Sept 2006           GB
 * Modified to append data dsm on to event header dsm item before
 * enqueuing into FTM watermark (For ASIA support)
 * 
 *    #41        11 Aug 2006            GB
 * Modified to support APIT mode in ASIA.
 *  
 *    #40        28 Jul 2006            DM
 * Changes for l2cap_cmd_write frame_handle is NULL for apit cmds
 *
 *    #39        25 Jun 2006            DSN
 * Removed Bonding App(BA) related references that are no longer used.
 *
 *    #38        02 Jun 2006            JTL
 * Add CID to L2Cap mode 2 security.
 *
 *    #37        14 Mar 2006            JH
 * Modified to support updated bt_cmd_rm_connect_sco() command.
 *
 *    #36         9 Nov 2005            DP
 * Renamed and extended SD service registration command.
 *
 *    #35        25 Jul 2005            GS
 * Added support for HCI over DIAG (ASIA).
 *
 *    #34        16 Mar 2005            DP
 * Fix Mode 2 security for client connections.
 *
 *    #32         7 Jan 2005            JH
 * Added call to bt_cmd_pf_str_to_ustr() before calling 
 *  bt_cmd_pf_opp_srv_open_read_done().
 *
 *    #31         5 Jan 2005            JH
 * Added support for new SD and RM commands.
 *
 *    #30        30 Aug 2004            JH
 * Added support for the bt_cmd_sd_search_service_uuid128 command.
 *
 *    #29        25 Aug 2004            DP
 * Removed featurization of BT HF.
 *
 *    #28         9 Aug 2004            JH
 * Renamed bt_cmd_pf_sml_auth_srv_response and bt_cmd_pf_sml_auth_cli_response 
 *  to bt_cmd_pf_sml_auth_request and bt_cmd_pf_sml_auth_response respectively
 *
 *    #27        30 Jul 2004            JH
 * Fixes incorrect dsm_length when processing BT_CMD_RM_TX_DATA_ACL
 *
 *    #25        29 Jul 2004            DP
 * Added CTP/ICP commands and events.
 *
 *    #24        8  Jul 2004            PN
 * Added support for additional AG events: SPKR_GAIN_REPORT, DEV_PICKUP,
 * VR_TOGGLE, and DEV_SEND_DTMF.
 *   
 *    #22        15 Jun 2004            JH
 * Fixed job_id = 0 case when calling bt_cmd_pf_bpp_sndr_send_file
 * Modified BT_EV_PF_BIP_CLI_GET_IMAGES_LIST_CFM, BT_EV_PF_BIP_SRV_PUT_IMAGE_IND,
 *  and BT_EV_PF_BIP_SRV_PUT_LINKED_ATTACHMENT_IND to use a secondary APIT event
 *  to return data
 * Added support for BIP server start print response
 *
 *    #21        8  Jun 2004            DP
 * Added Mode 2 security to APIT.
 *
 *    #20        14 May 2004            JH
 * Added FTP, BIP, BPP, and SyncML support
 *
 *    #19        25 Apr 2004            JH
 * Added code for sending OPP event data back to host via SIO.
 * Changed external profile event and command nomenclature.
 *
 *    #18        14 Apr 2004            RY
 * Added support for OPP and FTP.
 *
 *    #17        2  Apr 2004            DP
 * Added EMS support.
 *
 *    #16        19 Feb 2004            DP
 * Fixed APIT SIO operations.
 *
 *    #15        29 Jan 2004            DP
 * Added RM Remote Name Request support.
 *
 *    #14        19 Dec 2003            DP
 * Added RM command to allow role switch requests from remote devices.
 *
 *    #12        4  Nov 2003            DP
 * Added SPP support.  Added security and link status to RM.  General cleanup.
 *
 *    #11        06 Aug 2003            PN
 * Added support for RM Device DB.
 *
 *    #10        6  Aug 2003            RY
 * Fixed featurization in order to compile in MSM5500.
 *
 *    #9         22 Jul 2003            RY
 * Added the support for AG commands AUDIO_CONNECT and SET_AD_MIC_GAIN, and
 * all the AG events.
 *
 *    #8         07 Jul 2003            RY
 * Replaced MSG_STR_LOW with MSG_SPRINTF_3.
 * 
 *    Rev 1.31   07 Jun 2002 17:03:36   waynelee
 * Fixed usage of DSMs to always use DSM accessors.
 * 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT */


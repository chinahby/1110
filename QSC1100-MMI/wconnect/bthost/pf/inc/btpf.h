#ifndef _BTPF_H
#define _BTPF_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H    E X T E R N A L    P R O F I L E    M O D U L E

DESCRIPTION
  This file contains declarations external to the Bluetooth external profile
  module.
              Copyright (c) 2004-2009 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/btpf.h#2 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-01-14   gs   Added bt_pf_ev_opp_cli_con_prog_ind_type struct and
                    obj_size param in bt_pf_ev_ftp_cli_write_req_type 
  2008-06-17   hs   Added the support for AVRCP 1.3 meta data feature
                    by adding AVRCP metadata command declarations, 
                    event structures, constants and data structures. 
  2008-06-16   pr   Added bt_cmd_pf_bpp_sndr_force_disconnect to tear down 
                    connection from printer to BPP server (object channel).   
  2008-04-30   rb   Fix warning - Superfluous ',' at the end of enum

*   #75         03 Mar 2008            GS
* Added Get/SetReport HID event and command interfaces 
*
*   #73         19 Dec 2007            JTL
* Added support for applications to start A2DP streaming rather than leaving
* it in AVS's control.
*
*   #72          4 Dec  2007           MH
* Added BD address support for BIP connect indication.
*
*   #71         21 Nov 2007            PR
* Added BD Address to all A2DP events
*
*   #70         20 Nov  2007           PC
* Added meta-comments for A2DP and AVRCP profiles. Fixed featurization of 
* SAP profile. The implementation of the profile is now feature governed.
*
*   #69         07 Nov  2007           GS
* Added HID profile host role support.
*
*   #68         31 May  2007          DSN
* Metacomments, featurization changes to allow remoting of btpf.h interfaces
* for SAP on 7K targets.
*
*   #67         30 May 2007           DSN
* Add paramters to apdu req event to avoid using the buffer from mmgsdi.
* 
*   #66         24 Apr  2007           VK
* Making BT_PF_BIP_OPS_TMO 5 sec in APIT builds. 
*
*   #65         21 Mar  2007           MH
* Realm string type parameter added to auth request indication.
*
*   #64         01 Mar  2007           MH
* Added srv disconnect response function prototype to remove warnings
*
*   #63         28 Feb  2007           MH
* Modified srv disconnect indication to support optional headers
*
*   #62         13 Feb  2007           MH
* Modified set path indication to support optional headers
*
*   #61         29 Jan  2007           MH
* Added support to update OBEX Profile service record with 128-bit UUID.
*
*   #60         12 Jan 2007           GS
* Updated bt_pf_uuid_128_type to fix HTORPC build issue on multi-processor 
* target
*
*   #59         10 Nov 2006           MH
* Modified bt_cmd_pf_goep_cli_init_data prototype
*   
*   #58         03 Nov 2006           DM
* Added handle (l2cap_cid) to a2dp_send_data required for flow control
* in A2DP.
*
*   #57         26 Oct 2006           MH
* Modified server confirm set path and connect indication to support
* all header types
*
*    #56        10 Oct 2006           GS
* Added obj_size parameter in bt_pf_ev_ftp_srv_open_req_type.
*
*    #55        19 Sept 2006          MH
* Server initiating authentication command added.
*
*    #54        25 AUG 2006           BK
* Compile errors fixed
*
*    #51        28 Jul 2006           BK
* Modified for obex client authentication
*
*    #50        28 Jul 2006           DM
* Added definition of bt_pf_a2dp_frame_handle_type
* And send_data interfaces changes for Av-Sync
*
*    #49        14 Jul 2006           GS
* Modified definition of OPP Supported formats.
*
*    #47-48     10 Jul 2006           JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #46        23 Jun 2006           JH
* Modified GOEP API to support JSR82.
* Added prototype for bt_cmd_pf_goep_free_header_list().
*
*    #45        30 May 2006           GS
* Added support for PBAP feature.
*
*    #44        19 May 2006           JTL
* Changing AVRCP Generic Op events to include DSMs.
*
*    #43        20 Dec 2005           JTL
* Adding new command, bt_cmd_pf_a2dp_flush, for A2DP.
*
*    #42        15 Nov 2005           JTL
* Adding new command, bt_cmd_pf_a2dp_register_avs, for A2DP.
*
*    #40        22 Oct 2005           JH
* Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS.
*
*    #39        15 Aug 2005           JH
* Define BT_PF_MAX_FILENAME_LEN as 80, decoupling it from FS_FILENAME_MAX_LENGTH
*
*    #38        08 Aug 2005           JTL
* Adding status field to AVRCP responses.
*
*    #37        28 Jul 2005           JTL
* Removing unused types.
*
*    #36        26 Jul 2005           JTL
* Increasing max size of OBEX transfers to be equal to the FS max size.
*
*    #35        18 Jul 2005           GS
* Added bt_cmd_pf_bip_srv_force_disconnect() prototype.
*
*    #34        13 Jul 2005           JH
* Added bt_cmd_pf_a2dp_ctl() prototype.
*
*    #33        13 Jul 2005           JTL
* Added data to A2DP suspend and start events. Added another parameter
* to Bitrate command.
* Re-arranged bitrate enum to match AVS's enum.
*
*    #32         8 Jul 2005            JH
* Added status parameter to bt_pf_ev_ftp_cli_close_req_type and
*  bt_pf_ev_ftp_srv_close_req_type.
*
*    #31        29 Jun 2005           JTL
* Increasing number of remote capabilities and endpoints accepted and processed
* by AVDTP.
* Removing type defines no longer needed.
*
*    #30        02 Jun 2005           JTL
* Updates for A2DP. Using type defined by AVS.
*
*    #29        31 May 2005            RY
* Merged in Sirius changes.
*
*    #23-28     23 Mar 2005           JTL
* Adding support for AV profiles, AVDTP, AVRCP and A2DP.
*
*    #22        22 Jan 2005            PN
* Added support for SIM Access profile.
*
*    #20-21     17 Jan 2005            JH
* Removed the long 5 second BT_PF_BIP_OPS_TMO timeout which is used only when
*  FEATURE_BT_TEST_API is defined. Timeout is always 1 second now.
*
*    #19        29 Dec 2004            JH
* Added BT_PF_BD_ADDR_NULL.
*
*    #18        05 Nov 2004            JH
* Added connection ID to the GOEP set path indication event.
* Reduced BIP profile timeout to prevent watchdog timeout to occur before
*  profile timeout expires (for APIT case).
*
*    #17        19 Oct 2004            JH
* Added num_returned_handles to bt_pf_ev_bip_cli_get_list_cfm_type
*
*    #16        09 Aug 2004            JH
* Renamed bt_cmd_pf_sml_auth_srv_response and bt_cmd_pf_sml_auth_cli_response to
*  bt_cmd_pf_sml_auth_request and bt_cmd_pf_sml_auth_response respectively
*
*    #15        03 Aug 2004
* Modified order of elements in event structs bt_pf_ev_bip_srv_get_attachment_ind_type
*  and bt_pf_ev_bip_srv_get_partial_image_ind_type to resolve alignment issue.
*
*    #14        05 Aug 2004
* Redefining bt_pf_ctp_signal_type so it looks like an 8-bit value.
*
*    #13        29 Jul 2004
* Fixing CTP command Register Recall to be it's own command.
*
*    #12        29 Jul 2004
* Removing unneeded PSM from ICP command.
*
*    #11        29 Jul 2004
* Updating CTP/ICP structs to be packed, not include pointers, and
* be more concise.
*
*    #10        21 Jul 2004
* CTP/ICP code review changes. Adding events as well as commands
*
*    #9        21 Jul 2004
* Added CTP/ICP profile interfaces.
*
*    #8        15 Jun 2004
* Changed BIP timeout to 30000ms in APIT mode to facilitate testing
*
*    #7        06 Jun 2004            JH
* Added APIT support for BIP server start print response
* Added bt_pf_bip_target_type and updated bt_pf_ev_bip_srv_auth_ind_type struct.
*
*    #6        14 May 2004            JH
* Further command and event struct changes
* Added SyncML APIT support
*
*    #5        30 Apr 2004            JH
* Increased max length of MIME-type, needed for support BPP.
*
*    #4        23 Apr 2004            JH
* Profile event structs  and event IDs are now part of the BT event struct union
*  and BT event enum respectively.
*
*    #3        14 Apr 2004            JH
* Added BIP and BPP support
* Replaced all bt_pf_unicode_str_type command parameters with uint16* type

===========================================================================*/

#include "customer.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF
#include "comdef.h"
#include "dsm.h"
#include "queue.h"
#include "btcomdef.h"
#include "btpfi.h"
#include "fs.h"
#ifdef FEATURE_BT_EXTPF_AV
#include "audiosbcenc.h"
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_AVSYNC
#include "time_svc.h"
#endif

#ifdef FEATURE_MT_OBEX
/* bit 5,6,7 of the channel remote channel number can be used to tell the
   underlying driver what transport is requested */
#define MT_OBEX_BLUETOOTH_TP_ID     (0x01 << 5) /* LSH 5 = 0x20 */
#define MT_OBEX_IRDA_TP_ID          (0x02 << 5) /* LSH 5 = 0x40 */
#define MT_OBEX_USB_TP_ID           (0x03 << 5) /* LSH 5 = 0x60 */
#define MT_OBEX_RS232_TP_ID         (0x04 << 5) /* LSH 5 = 0x80 */
#define MT_OBEX_ALL_SUPPORTED_TP_ID (0x05 << 5) /* LSH 5 = 0xA0 */

#ifdef FEATURE_PCAA_SIRIUS
#error code not present
#endif /* FEATURE_PCAA_SIRIUS */

#endif /* FEATURE_MT_OBEX */
/*===========================================================================

             DATA DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_PF_APP_ID_NULL  0xFFFF
#define BT_PF_BD_ADDR_NULL { 0, 0, 0, 0, 0, 0 }

extern bt_app_id_type bt_pf_app_id;

typedef struct
{
  uint16  len;
  byte*   data;
} bt_pf_byte_seq_type;

typedef struct
{
  uint16  len;
  uint16* str;
} bt_pf_unicode_str_type;

#define BT_PF_UUID128_SIZE  16 /* bytes */

typedef struct
{
  uint8  data[BT_PF_UUID128_SIZE];
} bt_pf_uuid_128_type;

#define BT_PF_OBEX_MAX_PASSWORD_LEN OI_OBEX_MAX_PASSWORD_LEN /* 16 */
#define BT_PF_OBEX_MAX_USERID_LEN   OI_OBEX_MAX_USERID_LEN   /* 20 */
#define BT_PF_MAX_FILENAME_LEN      80 /* unicode characters excludes NULL */
#define BT_PF_MAX_MIME_TYPE_LEN     51 /* chars = 50 bytes, excludes NULL */
#define BT_PF_NUM_OBEX_OBJ_TYPES    6  /* there are 6 OBEX object types */


/*-------------------------------------------------------------------------*/
/*                    GOEP specific declarations                           */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_BT_EXTPF_GOEP

#include "oi_obexsrv.h"

typedef uint16 bt_pf_goep_cli_conn_id_type;
typedef uint16 bt_pf_goep_srv_conn_id_type;

typedef struct
{
  uint8 id;
  union
  {
    /* generic header types */
    uint8                  u_int_8;
    uint32                 u_int_32;
    bt_pf_byte_seq_type    byteseq;
    bt_pf_unicode_str_type unicode;
    /* specific header types */
    uint32                 count;
    uint32                 length;
    uint32                 connectionId;
    bt_pf_byte_seq_type    type;
    bt_pf_byte_seq_type    time;
    bt_pf_byte_seq_type    target;
    bt_pf_byte_seq_type    http;
    bt_pf_byte_seq_type    body;
    bt_pf_byte_seq_type    end_of_body;
    bt_pf_byte_seq_type    who;
    bt_pf_byte_seq_type    application_params;
    bt_pf_byte_seq_type    auth_challenge;
    bt_pf_byte_seq_type    auth_response;
    bt_pf_byte_seq_type    object_class;
    bt_pf_unicode_str_type name;
    bt_pf_unicode_str_type description;
  } val;
} bt_pf_goep_obex_hdr_type;

typedef struct
{
  bt_pf_goep_obex_hdr_type* list_ptr;
  uint8                     count;
} bt_pf_goep_obex_hdr_list_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif

typedef enum
{
  BT_PF_GOEP_SRV_AUTH_NONE = OI_OBEXSRV_AUTH_NONE,
  BT_PF_GOEP_SRV_AUTH_PASSWORD = OI_OBEXSRV_AUTH_PASSWORD,
  BT_PF_GOEP_SRV_AUTH_USERID_AND_PASSWORD = OI_OBEXSRV_AUTH_USERID_AND_PASSWORD
} bt_pf_goep_srv_auth_type;

#define BT_PF_OBEX_MAX_REALM_LEN    OI_OBEX_MAX_REALM_LEN       

typedef struct
{
    uint8 realm_str[BT_PF_OBEX_MAX_REALM_LEN];
    uint8 realm_len;
    uint8 realm_str_type;
}bt_pf_realm_info_type;


/*-------------------------------------------------------------------------*/
/*                     GOEP command declarations                           */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_pf_goep_free_headers(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_obex_hdr_type*            hdrs_ptr );

extern bt_cmd_status_type bt_cmd_pf_goep_cli_auth_response(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_cli_conn_id_type          connect_id,
  byte*                                user_id_ptr,
  uint8                                user_id_len,
  char*                                password_ptr );

extern void bt_cmd_pf_goep_cli_init_data(void);

extern bt_cmd_status_type bt_cmd_pf_goep_cli_authenticate(
    bt_bd_addr_type                 *bd_addr_ptr,
    uint8                          channel,
    bt_pf_realm_info_type          *realm_info,
    boolean                        uid_reqd,
    boolean                      full_access );

extern bt_cmd_status_type bt_cmd_pf_goep_cli_connect(
  bt_app_id_type                       bt_app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel,
  bt_pf_goep_obex_hdr_list_type        headers_list,
  bt_sd_uuid_type                      uuid,
  bt_security_type                     security );

extern bt_cmd_status_type bt_cmd_pf_goep_cli_disconnect(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_cli_conn_id_type          conn_id,
  bt_pf_goep_obex_hdr_list_type        headers_list );

extern bt_cmd_status_type bt_cmd_pf_goep_cli_put(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_cli_conn_id_type          conn_id,
  bt_pf_goep_obex_hdr_list_type        headers_list,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_goep_cli_get(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_cli_conn_id_type          conn_id,
  bt_pf_goep_obex_hdr_list_type        headers_list,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_goep_cli_set_path(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_cli_conn_id_type          conn_id,
  uint16*                              folder_ptr,
  boolean                              up_level,
  boolean                              dont_create,
  bt_pf_goep_obex_hdr_list_type        headers_list );

extern bt_cmd_status_type bt_cmd_pf_goep_cli_abort_operation(
  bt_app_id_type                       app_id,
  bt_pf_goep_cli_conn_id_type          conn_id );

extern bt_cmd_status_type bt_cmd_pf_goep_cli_reset( void );

extern bt_cmd_status_type bt_cmd_pf_goep_cli_terminate( void );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_init(
  uint16                               max_pkt_len,
  char*                                private_key );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_accept_connect(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_srv_conn_id_type          conn_id,
  boolean                              accept,
  bt_pf_goep_obex_hdr_list_type        headers_list,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_confirm_set_path(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_srv_conn_id_type          conn_id,
  bt_pf_goep_obex_hdr_list_type        headers_list,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_auth_response(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_srv_conn_id_type          conn_id,
  const char*                          password_ptr,
  boolean                              read_only );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_get_response(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_srv_conn_id_type          conn_id,
  bt_pf_goep_obex_hdr_list_type        headers_list,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_put_response(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_srv_conn_id_type          conn_id,
  bt_pf_goep_obex_hdr_list_type        headers_list,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_disconnect_response(
  bt_app_id_type                 app_id,
  bt_pf_goep_srv_conn_id_type    conn_id,
  bt_pf_goep_obex_hdr_list_type  headers_list,
  bt_cmd_status_type             status );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_force_disconnect(
  bt_app_id_type                       bt_app_id,
  bt_pf_goep_srv_conn_id_type          conn_id );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_register(
  bt_app_id_type                       app_id,
  bt_pf_byte_seq_type*                 target_ptr,
  bt_pf_goep_srv_auth_type             auth,
  bt_sd_uuid_type                      uuid,
  bt_security_type                     security,
  uint8*                               ch_num );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_reg_auth_cb(
  bt_app_id_type                       app_id,
  uint8                                ch_num,
  boolean                              authcb_rqd);

extern bt_cmd_status_type bt_cmd_pf_goep_srv_register_secondary_target(
  bt_app_id_type                       app_id,
  bt_pf_byte_seq_type*                 target_ptr,
  bt_pf_goep_srv_auth_type             auth,
  uint8                                ch_num,
  uint8*                               target_id );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_deregister_server(
  bt_app_id_type                       app_id,
  uint8                                ch_num );

extern bt_cmd_status_type bt_pf_goep_srv_get_target(
  bt_app_id_type                       app_id,
  bt_pf_goep_cli_conn_id_type          conn_id,
  bt_pf_byte_seq_type*                 target_ptr );

extern bt_cmd_status_type bt_pf_goep_srv_get_client_addr(
  bt_app_id_type                       app_id,
  bt_pf_goep_cli_conn_id_type          conn_id,
  bt_bd_addr_type*                     bd_addr_ptr );

extern bt_cmd_status_type bt_pf_goep_srv_get_rfcomm_channel(
  bt_app_id_type                       app_id,
  bt_pf_goep_cli_conn_id_type          conn_id,
  uint8*                               rfcomm_ch_ptr );

extern bt_cmd_status_type bt_cmd_pf_goep_srv_authenticate(
    bt_pf_goep_srv_conn_id_type         conn_id,
    bt_bd_addr_type                     *bd_addr_ptr,
    uint8                               channel,
    bt_pf_realm_info_type               *realm_info_ptr,
    boolean                             uid_reqd,
    boolean                             full_access);

extern bt_cmd_status_type bt_pf_goep_sd_register_custom_service(
        bt_app_id_type                  bt_app_id,
        bt_sd_uuid128_type              *uuid128,
        uint8                           scn,
        char*                           service_name_str_ptr);

#endif /* BT_FEATURE_EXTPF_GOEP */


/*-------------------------------------------------------------------------*/
/*                      OPP specific declarations                          */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_OPP

#define BT_PF_OPP_NO_CONN_ID     0xFFFF

#define BT_PF_OPP_OBJSYS_OPS_TMO 1000 /* ms */

#define BT_PF_OPP_SRV_OBJ_FORMAT_VCARD_2_1 0x01
#define BT_PF_OPP_SRV_OBJ_FORMAT_VCARD_3_0 0x02
#define BT_PF_OPP_SRV_OBJ_FORMAT_VCAL_1_0  0x04
#define BT_PF_OPP_SRV_OBJ_FORMAT_ICAL_2_0  0x08
#define BT_PF_OPP_SRV_OBJ_FORMAT_VNOTE     0x10
#define BT_PF_OPP_SRV_OBJ_FORMAT_VMESSAGE  0x20
#define BT_PF_OPP_SRV_OBJ_FORMAT_ANY       0xFFFF0000
#define BT_PF_OPP_SRV_OBJ_FORMAT_MASK      0xFFFF003F
typedef void* bt_pf_opp_handle_type;

typedef uint16 bt_pf_opp_cli_conn_id_type;
typedef uint16 bt_pf_opp_srv_conn_id_type;

/*-------------------------------------------------------------------------*/
/*                      OPP command declarations                           */
/*-------------------------------------------------------------------------*/
extern bt_cmd_status_type bt_cmd_pf_opp_cli_connect(
  bt_app_id_type                app_id,
  bt_bd_addr_type*              bd_addr_ptr,
  uint8                         channel );

extern bt_cmd_status_type bt_cmd_pf_opp_cli_disconnect(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id );

extern bt_cmd_status_type bt_cmd_pf_opp_cli_push(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  uint16*                       name_ptr,
  char*                         type_ptr );

extern bt_cmd_status_type bt_cmd_pf_opp_cli_pull(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id );

extern bt_cmd_status_type bt_cmd_pf_opp_cli_abort(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id );

extern bt_cmd_status_type bt_cmd_pf_opp_cli_open_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  uint16*                       name_ptr,
  char*                         type_ptr,
  uint32                        size,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_opp_cli_open_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_opp_cli_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_opp_cli_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_opp_srv_register(
  bt_app_id_type                app_id,
  uint32                        supported_formats_b,
  char*                         srv_name );

extern bt_cmd_status_type bt_cmd_pf_opp_srv_accept_connect(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  boolean                       allow_push,
  boolean                       allow_pull );

extern bt_cmd_status_type bt_cmd_pf_opp_srv_force_disconnect(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id );

extern bt_cmd_status_type bt_cmd_pf_opp_srv_deregister(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_cmd_pf_opp_srv_open_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  uint16*                       name_ptr,
  char*                         type_ptr,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_opp_srv_open_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_opp_srv_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_opp_srv_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type         status );

#endif /* FEATURE_BT_EXTPF_OPP */

/*-------------------------------------------------------------------------*/
/*                      FTP specific declarations                          */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_FTP

/* the following must be included after bt_cmd_status_type is defined */
#include "oi_ftp_client.h"
#include "oi_ftp_sys.h"

#define BT_PF_FTP_FILESYS_OPS_TMO 1000 /* ms */

typedef enum
{
  BT_PF_FTP_CLI_SET_PATH_TO_FOLDER = OI_FTP_CLIENT_SETPATH_TO_FOLDER,
  BT_PF_FTP_CLI_SET_PATH_TO_PARENT = OI_FTP_CLIENT_SETPATH_TO_PARENT,
  BT_PF_FTP_CLI_SET_PATH_TO_ROOT   = OI_FTP_CLIENT_SETPATH_TO_ROOT
} bt_pf_ftp_cli_set_path_type;

typedef enum
{
  BT_PF_FTP_MODE_READ  = OI_FTP_MODE_READ,  /* open a file for reading */
  BT_PF_FTP_MODE_WRITE = OI_FTP_MODE_WRITE  /* open a file for writing */
} bt_pf_ftp_mode_type;

#define BT_PF_FTP_NO_CONN_ID 0xFFFF

typedef void*  bt_pf_ftp_handle_type;
typedef uint16 bt_pf_ftp_cli_conn_id_type;
typedef uint16 bt_pf_ftp_srv_conn_id_type;

/*-------------------------------------------------------------------------*/
/*                      FTP command declarations                           */
/*-------------------------------------------------------------------------*/
extern bt_cmd_status_type bt_cmd_pf_ftp_cli_connect(
  bt_app_id_type                app_id,
  bt_bd_addr_type*              bd_addr_ptr,
  uint8                         channel );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_disconnect(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_auth_response(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  byte*                         user_id_ptr,
  uint8                         user_id_len,
  char*                         password_ptr );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_put(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       local_name_ptr,
  uint16*                       remote_name_ptr );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_delete(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       name_ptr );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_get(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       local_name_ptr,
  uint16*                       remote_name_ptr );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_set_path(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       folder_ptr,
  bt_pf_ftp_cli_set_path_type   set_path_ctrl );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_new_subfolder(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       folder_ptr );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_get_folder_listing(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       folder_ptr );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_tree_put(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       local_name_ptr,
  uint16*                       remote_name_ptr );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_tree_get(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       local_name_ptr,
  uint16*                       remote_name_ptr );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_abort(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_browse_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_open_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_read_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_write_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_ftp_cli_set_folder_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_cmd_status_type         status );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_register(
  bt_app_id_type                app_id,
  bt_pf_goep_srv_auth_type      auth,
  boolean                       read_only,
  boolean                       allow_browse,
  char*                         srv_name_ptr );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_deregister(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_auth_response(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  char*                         password_ptr,
  boolean                       read_only,
  boolean                       allow_browse );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_accept_connect(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  boolean                       accept,
  uint16*                       root_folder );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_force_disconnect(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_browse_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_open_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_read_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_write_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_delete_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_ftp_srv_set_folder_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_cmd_status_type            status );

#endif /* FEATURE_BT_EXTPF_FTP */

/*-------------------------------------------------------------------------*/
/*                      BIP specific declarations                          */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_BIP

/* the following must be included after bt_cmd_status_type is defined */
#include "oi_bip_spec.h"

#ifdef FEATURE_BT_TEST_API
#error code not present
#else
#define BT_PF_BIP_OPS_TMO 1000 /* ms */
#endif

#define BT_PF_BIP_NO_CONN_ID 0xFFFF

#define BT_PF_BIP_IMAGE_HANDLE_LEN 7 /* unicode characters = 14 bytes */

typedef enum
{
  BT_PF_BIP_TARGET_IMAGING_RESPONDER = 0,
  BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV,
  BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV,
  BT_PF_BIP_NUM_SRV
} bt_pf_bip_target_type;

typedef enum
{
  BT_PF_BIP_SRV_REQ_INITIAL  = OI_BIP_REQUEST_INITIAL,
  BT_PF_BIP_SRV_REQ_CONTINUE = OI_BIP_REQUEST_CONTINUE,
  BT_PF_BIP_SRV_REQ_CLEANUP  = OI_BIP_REQUEST_CLEANUP
} bt_pf_bip_srv_req_state_type;

typedef uint16 bt_pf_bip_cli_conn_id_type;
typedef uint16 bt_pf_bip_srv_conn_id_type;

typedef BT_PACKED struct
{
  uint16 image_handle_str[ BT_PF_BIP_IMAGE_HANDLE_LEN + 1 ];
                          /* 7 UTF-16 characters + NULL */
} bt_pf_bip_image_handle_type;

/*-------------------------------------------------------------------------*/
/*                      BIP command declarations                           */
/*-------------------------------------------------------------------------*/
extern bt_cmd_status_type bt_cmd_pf_bip_cli_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel,
  byte*                                target_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_disconnect(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_auth_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  char*                                pin_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_capabilities(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_put_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  uint16*                              image_name_ptr,
  bt_pf_byte_seq_type*                 image_descriptior_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_put_linked_thumbnail(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_put_linked_attachment(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  bt_pf_byte_seq_type*                 attachment_descriptior_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_images_list(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  uint16                               handle_count,
  uint16                               handle_offset,
  uint8                                latest,
  bt_pf_byte_seq_type*                 image_handles_descriptor_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_image_properties(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  bt_pf_byte_seq_type*                 image_descriptior_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_linked_thumbnail(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_linked_attachment(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  uint16*                              attachment_name_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_delete_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_status(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_partial_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  uint16*                              image_name_ptr,
  uint32                               partial_length,
  uint32                               partial_offset );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_start_print(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_sd_uuid128_type*                  service_id_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_start_archive(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_sd_uuid128_type*                  service_id_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_remote_display(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  uint8                                display_command );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_monitoring_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  boolean                              store_image );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_abort(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id );

extern bt_cmd_status_type bt_cmd_pf_bip_cli_get_rsp_cfm(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_event_type                        get_event );


extern bt_cmd_status_type bt_cmd_pf_bip_srv_register_imaging_responder(
  bt_app_id_type                       app_id,
  char*                                srv_name_ptr,
  uint8                                supported_capabilities,
  uint16                               supported_features,
  uint32                               supported_functions,
  uint64                               imaging_capacity,
  boolean                              auth );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_register_referenced_objects(
  bt_app_id_type                       app_id,
  char*                                srv_name_ptr,
  bt_sd_uuid128_type*                  service_id_ptr,
  uint32                               supported_functions,
  boolean                              auth );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_register_archived_objects(
  bt_app_id_type                       app_id,
  char*                                srv_name_ptr,
  bt_sd_uuid128_type*                  service_id_ptr,
  uint32                               supported_functions,
  boolean                              auth );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_deregister_imaging_responder(
  bt_app_id_type                       app_id );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_deregister_referenced_objects(
  bt_app_id_type                       app_id );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_deregister_archived_objects(
  bt_app_id_type                       app_id );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_auth_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  char*                                pin_ptr );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_force_disconnect(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_get_capabilities_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_put_image_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_put_linked_thumbnail_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_put_linked_attachment_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_get_images_list_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  uint16                               num_handles,
  bt_pf_byte_seq_type*                 image_handles_descriptor_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_get_image_properties_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_get_image_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  uint32                               file_size,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_get_linked_thumbnail_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_get_linked_attachment_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_delete_image_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_get_status_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_get_partial_image_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  uint32                               total_file_size,
  boolean                              end_flag,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_start_archive_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_remote_display_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_get_monitoring_image_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bip_srv_req_cfm(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_event_type                        req_event,
  bt_cmd_status_type                   status );

#endif /* FEATURE_BT_EXTPF_BIP */

/*-------------------------------------------------------------------------*/
/*                      BPP specific declarations                          */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_BPP

/* the following must be included after bt_cmd_status_type is defined */
#include "oi_bpp.h"

#define BT_PF_BPP_NO_CONN_ID 0xFFFF

#define BT_PF_BPP_OPS_TMO 1000 /* ms */

typedef uint16 bt_pf_bpp_sndr_conn_id_type;

typedef uint32 bt_pf_bpp_job_id_type;

typedef enum
{
  BT_PF_BPP_TARGET_DPS = BPP_TARGET_DPS, /* Direct printing */
  BT_PF_BPP_TARGET_PBR = BPP_TARGET_PBR, /* Print by reference */
  BT_PF_BPP_TARGET_STS = BPP_TARGET_STS, /* Status */
  BT_PF_BPP_REF_OBJ    = REF_TARGET_OBJ, /* Referenced objects */
  BT_PF_BPP_NO_TARGET
} bt_pf_bpp_target_type;

typedef enum {
  BT_PF_BPP_REF_SIMPLE = OI_BPP_REF_SIMPLE,
  BT_PF_BPP_REF_XML    = OI_BPP_REF_XML,
  BT_PF_BPP_REF_LIST   = OI_BPP_REF_LIST
} bt_pf_bpp_reference_type;

/*-------------------------------------------------------------------------*/
/*                      BPP command declarations                           */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel,
  bt_pf_bpp_target_type                target );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_disconnect(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_auth_response(
  bt_app_id_type                       app_id,
  bt_pf_bpp_target_type                target,
  byte*                                user_id_ptr,
  uint8                                user_id_len,
  char*                                password_ptr );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_send_file(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id,
  char*                                type_ptr,
  uint16*                              description_ptr,
  uint16*                              name_ptr,
  bt_pf_bpp_job_id_type*               job_id_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_send_reference(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id,
  bt_pf_bpp_reference_type             type,
  bt_pf_byte_seq_type*                 http_headers_ptr,
  bt_pf_bpp_job_id_type*               job_id_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_soap_request(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id,
  bt_pf_byte_seq_type*                 body_ptr,
  boolean                              final );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_abort(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_get_ref_obj_response(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id,
  bt_pf_byte_seq_type*                 data_ptr,
  int32*                               file_size_ptr,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_register(
  bt_app_id_type                       app_id,
  char*                                srv_name_ptr );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_deregister(
  bt_app_id_type                       app_id );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_get_rsp_cfm(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_srv_req_cfm(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id,
  bt_event_type                        event,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_bpp_sndr_force_disconnect(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id);

#endif /* FEATURE_BT_EXTPF_BPP */

/*-------------------------------------------------------------------------*/
/*                      SML specific declarations                          */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_SML

#define BT_PF_SML_NO_CONN_ID 0xFFFF

typedef uint16 bt_pf_sml_conn_id_type;

/*-------------------------------------------------------------------------*/
/*                      SML command declarations                           */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_pf_sml_register_client(
  bt_app_id_type                       app_id,
  char*                                service_uuid_ptr,
  boolean                              auth );

extern bt_cmd_status_type bt_cmd_pf_sml_register_server(
  bt_app_id_type                       app_id,
  char*                                service_uuid_ptr,
  boolean                              auth );

extern bt_cmd_status_type bt_cmd_pf_sml_deregister_client(
  bt_app_id_type                       app_id );

extern bt_cmd_status_type bt_cmd_pf_sml_deregister_server(
  bt_app_id_type                       app_id );

extern bt_cmd_status_type bt_cmd_pf_sml_auth_request(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  char*                                password_ptr,
  boolean                              read_only );

extern bt_cmd_status_type bt_cmd_pf_sml_accept_connect(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  boolean                              accept,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_sml_auth_response(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  byte*                                user_id_ptr,
  uint8                                user_id_len,
  char*                                password_ptr );

extern bt_cmd_status_type bt_cmd_pf_sml_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel,
  char*                                target_uuid_ptr );

extern bt_cmd_status_type bt_cmd_pf_sml_disconnect(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id );

extern bt_cmd_status_type bt_cmd_pf_sml_get(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  char*                                type_ptr );

extern bt_cmd_status_type bt_cmd_pf_sml_put(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  char*                                type_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_sml_get_rsp(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  bt_pf_byte_seq_type*                 data_ptr,
  bt_cmd_status_type                   status );

extern bt_cmd_status_type bt_cmd_pf_sml_put_rsp(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  bt_cmd_status_type                   status );

#endif /* FEATURE_BT_EXTPF_SML */

/*-------------------------------------------------------------------------*/
/*                    CTP/ICP specific declarations                        */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_CTP

#define BT_PF_CTP_CHAR_DIALING_PAUSE   0x05
#define BT_PF_CTP_CHAR_REGISTER_RECALL 0x16

#define BT_PF_CTP_MAX_DIGITS           20


typedef enum {
    BT_PF_CTP_NUMBER_TYPE_UNKNOWN       = 0x0000,
    BT_PF_CTP_NUMBER_TYPE_INTERNATIONAL = 0x0010,
    BT_PF_CTP_NUMBER_TYPE_NATIONAL      = 0x0020,
    BT_PF_CTP_NUMBER_TYPE_NETWORK       = (0x0010 | 0x0020),
    BT_PF_CTP_NUMBER_TYPE_SUBSCRIBER    = 0x0040,
    BT_PF_CTP_NUMBER_TYPE_ABBREVIATED   = (0x0040 | 0x0020),
    BT_PF_CTP_NUMBER_TYPE_RESERVED_FOR_EXTENSION = (0x0040 | 0x0020 | 0x0010)
} bt_pf_ctp_number_type_type;
typedef bt_pf_ctp_number_type_type bt_pf_icp_number_type_type;

typedef enum {
    BT_PF_CTP_NUMBER_PLAN_UNKNOWN = 0x0000,
    BT_PF_CTP_NUMBER_PLAN_ISDN    = 0x0001,
    BT_PF_CTP_NUMBER_PLAN_X121    = (0x0001 | 0x0002),
    BT_PF_CTP_NUMBER_PLAN_RESERVED = 0x0004,
    BT_PF_CTP_NUMBER_PLAN_NATIONAL = 0x0008,
    BT_PF_CTP_NUMBER_PLAN_PRIVATE  = (0x0008 | 0x0001)
} bt_pf_ctp_number_plan_type;
typedef bt_pf_ctp_number_plan_type bt_pf_icp_number_plan_type;

typedef enum {
    BT_PF_CTP_PRESENTATION_ALLOWED     = 0x0000,
    BT_PF_CTP_PRESENTATION_RESTRICTED  = 0x0020,
    BT_PF_CTP_PRESENTATION_NUM_NOT_AVAILABLE = 0x0040,
    BT_PF_CTP_PRESENTATION_RESERVED    = (0x0020 | 0x0040)
} bt_pf_ctp_presentation_type;
typedef bt_pf_ctp_presentation_type bt_pf_icp_presentation_type;

typedef enum {
    BT_PF_CTP_SCREENING_USER_NOT_SCREENED  = 0x0000,
    BT_PF_CTP_SCREENING_USER_PASSED        = 0x0001,
    BT_PF_CTP_SCREENING_USER_FAILED        = 0x0002,
    BT_PF_CTP_SCREENING_NETWORK            = (0x0001 | 0x0002)
} bt_pf_ctp_screening_type;
typedef bt_pf_ctp_screening_type bt_pf_icp_screening_type;

typedef enum
{
  BT_PF_CTP_AUDIO_CONTROL_VOLUME_INCREASE,
  BT_PF_CTP_AUDIO_CONTROL_VOLUME_DECREASE,
  BT_PF_CTP_AUDIO_CONTROL_MIC_GAIN_INCREASE,
  BT_PF_CTP_AUDIO_CONTROL_MIC_GAIN_DECREASE,
  BT_PF_CTP_AUDIO_CONTROL_NONE
} bt_pf_ctp_audio_control_type;
typedef bt_pf_ctp_audio_control_type bt_pf_icp_audio_control_type;

typedef enum {
    BT_PF_CTP_CAUSE_NONE = 0,   /* Note, 0 is only used internally as a place-
                                   holder for unused cause in elements structure */
    BT_PF_CTP_CAUSE_UNALLOCATED_NUMBER                 = 1,
    BT_PF_CTP_CAUSE_NO_ROUTE_TO_DESTINATION            = 3,
    BT_PF_CTP_CAUSE_NORMAL_CALL_CLEARING               = 16,
    BT_PF_CTP_CAUSE_USER_BUSY                          = 17,
    BT_PF_CTP_CAUSE_NO_USER_RESPONDING                 = 18,
    BT_PF_CTP_CAUSE_NO_ANSWER_FROM_USER                = 19,
    BT_PF_CTP_CAUSE_CALL_REJECTED                      = 21,
    BT_PF_CTP_CAUSE_NUMBER_CHANGED                     = 22,
    BT_PF_CTP_CAUSE_NONSELECTED_USER_CLEARING          = 26,
    BT_PF_CTP_CAUSE_INVALID_NUMBER_FORMAT              = 28,
    BT_PF_CTP_CAUSE_FACILITY_REJECTED                  = 29,
    BT_PF_CTP_CAUSE_NO_CIRCUIT_AVAILABLE               = 34,
    BT_PF_CTP_CAUSE_TEMPORARY_FAILURE                  = 41,
    BT_PF_CTP_CAUSE_REQUESTED_CIRCUIT_NOT_AVAILABLE    = 44,
    BT_PF_CTP_CAUSE_BEARER_CAPABILITY_UNAVAILABLE      = 58,
    BT_PF_CTP_CAUSE_BEARER_CAPABILITY_NOT_IMPLEMENTED  = 65,
    BT_PF_CTP_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED = 69,
    BT_PF_CTP_CAUSE_MANDATORY_INFORMATION_ELEM_MISSING = 96,
    BT_PF_CTP_CAUSE_INVALID_INFORMATION_ELEM_CONTENTS  = 100,
    BT_PF_CTP_CAUSE_RECOVERY_ON_TIMER_EXPIRY           = 102
} bt_pf_ctp_cause_type;
typedef bt_pf_ctp_cause_type bt_pf_icp_cause_type;

typedef enum {
  BT_PF_CTP_CALL_STATUS_OVERLAP_RECEIVING,
  BT_PF_CTP_CALL_STATUS_PROCEEDING,
  BT_PF_CTP_CALL_STATUS_ALERTING
} bt_pf_ctp_call_status_type;
typedef bt_pf_ctp_call_status_type bt_pf_icp_call_status_type;

/** signal */
typedef enum {
    BT_PF_CTP_SIGNAL_NONE,
    BT_PF_CTP_SIGNAL_EXTERNAL_CALL = 0x40,
    BT_PF_CTP_SIGNAL_INTERNAL_CALL = (0x40 | 0x01),
    BT_PF_CTP_SIGNAL_CALL_BACK     = (0x40 | 0x02)
} bt_pf_ctp_signal_type;
typedef bt_pf_ctp_signal_type bt_pf_icp_signal_type;

typedef enum {
    BT_PF_CTP_CALL_CLASS_EXTERNAL = 0,
    BT_PF_CTP_CALL_CLASS_INTERCOM = 1,
    BT_PF_CTP_CALL_CLASS_SERVICE  = 2,
    BT_PF_CTP_CALL_CLASS_EMERGENCY = 3,
    BT_PF_CTP_CALL_CLASS_NONE
} bt_pf_ctp_call_class_type;
typedef bt_pf_ctp_call_class_type bt_pf_icp_call_class_type;

typedef BT_PACKED struct
{
  boolean                     calling;
  bt_pf_ctp_number_type_type  type;
  bt_pf_ctp_number_plan_type  plan;
  bt_pf_ctp_presentation_type presentation;
  bt_pf_ctp_screening_type    screening;
  char                        digits[BT_PF_CTP_MAX_DIGITS];
  char                        num_digits;
} bt_pf_ctp_number_type;
typedef bt_pf_ctp_number_type bt_pf_icp_number_type;

typedef BT_PACKED struct
{
  bt_pf_ctp_call_class_type call_class;
  boolean                   sending_complete;
  bt_pf_ctp_signal_type     signal;
  bt_pf_ctp_number_type     calling_number;
  bt_pf_ctp_number_type     called_number;
} bt_pf_ctp_connect_info_type;
typedef bt_pf_ctp_connect_info_type bt_pf_icp_connect_info_type;

/*-------------------------------------------------------------------------*/
/*                     CTP/ICP command declarations                        */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_pf_ctp_connect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr
);

extern bt_cmd_status_type bt_cmd_pf_ctp_accept_connection (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  boolean                              accept
);

extern bt_cmd_status_type bt_cmd_pf_ctp_disconnect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr
);

extern bt_cmd_status_type bt_cmd_pf_ctp_clear_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_ctp_cause_type                 cause
);

extern bt_cmd_status_type bt_cmd_pf_ctp_establish_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_ctp_number_type*               calling_number,
  bt_pf_ctp_number_type*               called_number
);

extern bt_cmd_status_type bt_cmd_pf_ctp_info (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  boolean                              sending_complete,
  char                                 keypad_facility,
  bt_pf_ctp_number_type*               calling_number,
  bt_pf_ctp_number_type*               called_number,
  bt_pf_ctp_audio_control_type         audio_control
);

extern bt_cmd_status_type bt_cmd_pf_ctp_incoming_status (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_ctp_call_status_type           status
);

extern bt_cmd_status_type bt_cmd_pf_ctp_accept_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr
);

extern bt_cmd_status_type bt_cmd_pf_ctp_start_dtmf (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  char                                 tone
);

extern bt_cmd_status_type bt_cmd_pf_ctp_stop_dtmf (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr
);

extern bt_cmd_status_type bt_cmd_pf_ctp_register (
  bt_app_id_type                       app_id
);

extern bt_cmd_status_type bt_cmd_pf_ctp_deregister (
  bt_app_id_type                       app_id
);

extern bt_cmd_status_type bt_cmd_pf_ctp_register_recall (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr
);

/* *** */
/* ICP */
/* *** */

extern bt_cmd_status_type bt_cmd_pf_icp_connect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr
);

extern bt_cmd_status_type bt_cmd_pf_icp_accept_connection (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  boolean                              accept
);

extern bt_cmd_status_type bt_cmd_pf_icp_disconnect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr
);

extern bt_cmd_status_type bt_cmd_pf_icp_clear_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_icp_cause_type                 cause
);

extern bt_cmd_status_type bt_cmd_pf_icp_establish_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_icp_number_type*               calling_number,
  bt_pf_icp_number_type*               called_number
);

extern bt_cmd_status_type bt_cmd_pf_icp_incoming_status (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_icp_call_status_type           status
);

extern bt_cmd_status_type bt_cmd_pf_icp_accept_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr
);

extern bt_cmd_status_type bt_cmd_pf_icp_register (
  bt_app_id_type                       app_id
);

extern bt_cmd_status_type bt_cmd_pf_icp_deregister (
  bt_app_id_type                       app_id
);


#endif /* FEATURE_BT_EXTPF_CTP */


#include "oi_sap.h"

/*-------------------------------------------------------------------------*/
/*                    SAP specific declarations                            */
/*-------------------------------------------------------------------------*/
typedef enum
{
  BT_PF_SAP_CON_STAT_OK                   = OI_SAP_STATUS_OK,
  BT_PF_SAP_CON_STAT_FAILED               = OI_SAP_STATUS_CONNECT_FAIL,
  BT_PF_SAP_CON_STAT_MSG_SIZE_TOO_LARGE   = OI_SAP_STATUS_MSG_SIZE_TOO_LARGE,
  BT_PF_SAP_CON_STAT_MSG_SIZE_TOO_SMALL   = OI_SAP_STATUS_MSG_SIZE_TOO_SMALL,
  BT_PF_SAP_CON_STAT_MAX
} bt_pf_sap_con_status_type;

typedef enum
{
  BT_PF_SAP_DCN_GRACEFUL                  = OI_SAP_DISCONNECT_GRACEFUL,
  BT_PF_SAP_DCN_IMMEDIATE                 = OI_SAP_DISCONNECT_IMMEDIATE
} bt_pf_sap_dcn_type;

typedef enum
{
  BT_PF_SAP_RESULT_ERR_NONE               = OI_SAP_RESULT_OK,
  BT_PF_SAP_RESULT_ERR_NO_REASON          = OI_SAP_RESULT_ERROR_NO_REASON,
  BT_PF_SAP_RESULT_ERR_CARD_INACCESSIBLE  = OI_SAP_RESULT_CARD_NOT_ACCESSIBLE,
  BT_PF_SAP_RESULT_ERR_CARD_ALREADY_OFF   = OI_SAP_RESULT_CARD_ALREADY_OFF,
  BT_PF_SAP_RESULT_ERR_CARD_REMOVED       = OI_SAP_RESULT_CARD_REMOVED,
  BT_PF_SAP_RESULT_ERR_CARD_ALREADY_ON    = OI_SAP_RESULT_CARD_ALREADY_ON,
  BT_PF_SAP_RESULT_ERR_DATA_UNAVAILABLE   = OI_SAP_RESULT_DATA_NOT_AVAILABLE,
  BT_PF_SAP_RESULT_ERR_MAX
} bt_pf_sap_result_code_type;

typedef enum
{
  BT_PF_SAP_STAT_UNKNOWN_ERROR      = OI_SAP_STATUS_CHANGE_UNKNOWN_ERROR,
  BT_PF_SAP_STAT_CARD_RESET         = OI_SAP_STATUS_CHANGE_CARD_RESET,
  BT_PF_SAP_STAT_CARD_INACCESSIBLE  = OI_SAP_STATUS_CHANGE_CARD_NOT_ACCESSIBLE,
  BT_PF_SAP_STAT_CARD_REMOVED       = OI_SAP_STATUS_CHANGE_CARD_REMOVED,
  BT_PF_SAP_STAT_CARD_INSERTED      = OI_SAP_STATUS_CHANGE_CARD_INSERTED,
  BT_PF_SAP_STAT_CARD_RECOVERED     = OI_SAP_STATUS_CHANGE_CARD_RECOVERED,
  BT_PF_SAP_STAT_MAX
} bt_pf_sap_sim_status_type;

/*-------------------------------------------------------------------------*/
/*                      SAP command declarations                           */
/*-------------------------------------------------------------------------*/
extern bt_cmd_status_type bt_cmd_pf_sap_register(
  bt_app_id_type                app_id,
  uint16                        min_frame_size,
  uint16                        max_frame_size,
  uint8*                        req_dst_ptr,
  uint16*                       req_len_ptr,
  const char*                   srv_name_ptr );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_pf_sap_register */
/*~ PARAM req_len_ptr VOID */
/*~ PARAM req_dst_ptr VOID */
/*~ PARAM srv_name_ptr STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_sap_deregister(
  bt_app_id_type                app_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_pf_sap_deregister */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_sap_accept_connection(
  bt_app_id_type                app_id,
  boolean                       accept );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_pf_sap_accept_connection */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_sap_connect(
  bt_app_id_type                app_id,
  const bt_bd_addr_type*        bd_addr_ptr,
  uint8                         channel,
  uint16                        min_frame_size,
  uint16                        max_frame_size,
  uint8*                        rsp_dst_ptr,
  uint16*                       rsp_len_ptr );

extern bt_cmd_status_type bt_cmd_pf_sap_disconnect(
  bt_app_id_type                app_id,
  boolean                       disc_immediately );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_pf_sap_disconnect */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_sap_tx_apdu_req(
  bt_app_id_type                app_id,
  const uint8*                  req_ptr,
  uint16                        req_len );

extern bt_cmd_status_type bt_cmd_pf_sap_tx_atr_req(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_cmd_pf_sap_power_sim_off(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_cmd_pf_sap_power_sim_on(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_cmd_pf_sap_reset_sim(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_cmd_pf_sap_get_card_reader_status(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_cmd_pf_sap_tx_apdu_rsp(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result,
  const uint8*                  rsp_ptr,
  uint16                        rsp_len );

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_pf_sap_tx_apdu_rsp */
/*~ PARAM rsp_ptr VARRAY LENGTH rsp_len */
#endif /* FEATURE_HTORPC_METACOMMENTS */


extern bt_cmd_status_type bt_cmd_pf_sap_tx_atr_rsp(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result,
  const uint8*                  rsp_ptr,
  uint16                        len );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_pf_sap_tx_atr_rsp */
/*~ PARAM rsp_ptr VARRAY LENGTH len */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_sap_power_sim_off_rsp(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_pf_sap_power_sim_off_rsp */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_sap_power_sim_on_rsp(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  bt_cmd_pf_sap_power_sim_on_rsp */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_sap_reset_sim_rsp(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_sap_reset_sim_rsp */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_sap_tx_card_reader_status(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result,
  uint8                         status );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_sap_tx_card_reader_status */
#endif /* FEATURE_HTORPC_METACOMMENTS */


extern bt_cmd_status_type bt_cmd_pf_sap_tx_sim_status(
  bt_app_id_type                app_id,
  bt_pf_sap_sim_status_type     status );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_sap_tx_sim_status */
#endif /* FEATURE_HTORPC_METACOMMENTS */



#ifdef FEATURE_BT_EXTPF_AV
/*-------------------------------------------------------------------------*/
/*                      AVRCP type declarations                            */
/*-------------------------------------------------------------------------*/

typedef enum {
  BT_PF_AVRCP_RESPONSE_NOT_IMPLEMENTED    = 0x8,
  BT_PF_AVRCP_RESPONSE_ACCEPTED           = 0x9,
  BT_PF_AVRCP_RESPONSE_REJECTED           = 0xa,
  BT_PF_AVRCP_RESPONSE_IN_TRANSITION      = 0xb,
  BT_PF_AVRCP_RESPONSE_IMPLEMENTED        = 0xc,
  BT_PF_AVRCP_RESPONSE_STABLE             = 0xc,
  BT_PF_AVRCP_RESPONSE_CHANGED            = 0xd
} bt_pf_avrcp_rsp_type;

/* See AVRCP 1.0, section 4.7.5 to see which commands are
 * required to be supported by each category */
#define BT_PF_AVRCP_CATEGORY1 0x01 /* Player / Recorder */
#define BT_PF_AVRCP_CATEGORY2 0x02 /* Monitor / Amplifier */
#define BT_PF_AVRCP_CATEGORY3 0x04 /* Tuner */
#define BT_PF_AVRCP_CATEGORY4 0x08 /* Menu */

typedef enum {
  BT_PF_AVRCP_OP_PRESSED,
  BT_PF_AVRCP_OP_RELEASED
} bt_pf_avrcp_op_status_type;

typedef BT_PACKED struct {
  uint8                      txn_id;

  uint8                      cmd;
  uint8                      subunit_type;
  uint8                      subunit_id;
  uint8                      opcode;
  dsm_item_type*             operand;
} bt_pf_avrcp_generic_op_type;

#ifdef FEATURE_BT_AVRCP_13

#define BT_PF_AVRCP_SIZE_OF_CURRENT_TRACK_ID   8

#define BT_PF_MAX_NUM_MEDIA_ATTRIBUTES         8 

/* Support for following character set ids */
typedef enum {
 BT_PF_AVRCP_ILLEGAL_CHARSET_ID    =  0,
 BT_PF_AVRCP_LANG_ENCODING_UTF8    = 106,
 BT_PF_AVRCP_LANG_ENCODING_UTF16   = 1015
} bt_pf_avrcp_charset_type;

/* Allowed values of Battery status */
typedef enum {
  BT_PF_AVRCP_BATTERY_STATUS_NORMAL        = 0x00,
  BT_PF_AVRCP_BATTERY_STATUS_WARNING       = 0x01,
  BT_PF_AVRCP_BATTERY_STATUS_CRITICAL      = 0x02,
  BT_PF_AVRCP_BATTERY_STATUS_EXTERNAL      = 0x03,
  BT_PF_AVRCP_BATTERY_STATUS_FULL_CHARGE   = 0x04
} bt_pf_avrcp_battery_status_type;

/* Allowed values of System status */
typedef enum {
  BT_PF_AVRCP_SYSTEM_STATUS_POWER_ON  = 0x00,
  BT_PF_AVRCP_SYSTEM_STATUS_POWER_OFF = 0x01,
  BT_PF_AVRCP_SYSTEM_STATUS_UNPLUGGED = 0x02
} bt_pf_avrcp_system_status_type;

/* Allowed values of Play status */
typedef enum {
  BT_PF_AVRCP_PLAY_STOPPED            = 0x00,
  BT_PF_AVRCP_PLAY_PLAYING            = 0x01,
  BT_PF_AVRCP_PLAY_PAUSED             = 0x02,
  BT_PF_AVRCP_PLAY_FWD_SEEK           = 0x03,
  BT_PF_AVRCP_PLAY_REV_SEEK           = 0x04,
  BT_PF_AVRCP_PLAY_ERROR              = 0xff
} bt_pf_avrcp_playback_status_type;

/* AVRCP Metadata transfer events */
typedef enum {
  BT_PF_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED            = 0x01,
  BT_PF_AVRCP_EVENT_TRACK_CHANGED                      = 0x02,
  BT_PF_AVRCP_EVENT_TRACK_REACHED_END                  = 0x03,
  BT_PF_AVRCP_EVENT_TRACK_REACHED_START                = 0x04,
  BT_PF_AVRCP_EVENT_PLAYBACK_POS_CHANGED               = 0x05,
  BT_PF_AVRCP_EVENT_BATT_STATUS_CHANGED                = 0x06,
  BT_PF_AVRCP_EVENT_SYSTEM_STATUS_CHANGED              = 0x07,
  BT_PF_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = 0x08
} bt_pf_avrcp_metadata_transfer_events_type;

/* AVRCP Metadata Media Player application settings */
typedef enum {
  BT_PF_AVRCP_SETTING_ILLEGAL             = 0x0,
  BT_PF_AVRCP_SETTING_EQUALIZER_STATUS    = 0x1,
  BT_PF_AVRCP_SETTING_REPEAT_MODE_STATUS  = 0x2,
  BT_PF_AVRCP_SETTING_SHUFFLE_STATUS      = 0x3,
  BT_PF_AVRCP_SETTING_SCAN_STATUS         = 0x4
  /* 0x05 - 0x7F Reserved for future use */
  /* 0x80 - 0xFF Provided for TG driven static media player menu extension by CT */
} bt_pf_avrcp_player_app_settings_type;

/* AVRCP Metadata Media Player equalizer settings */
typedef enum {
  BT_PF_AVRCP_EQUALIZER_STATUS_OFF = 0x1,
  BT_PF_AVRCP_EQUALIZER_STATUS_ON  = 0x2
  /* 0x03 - 0xfF Reserved for future use */
} bt_pf_avrcp_player_equalizer_settings_type;

/* AVRCP Metadata Media Player repeat mode settings */
typedef enum {
  BT_PF_AVRCP_REPEAT_MODE_OFF          = 0x1,
  BT_PF_AVRCP_REPEAT_MODE_SINGLE_TRACK = 0x2,
  BT_PF_AVRCP_REPEAT_MODE_ALL_TRACKS   = 0x3,
  BT_PF_AVRCP_REPEAT_MODE_GROUP        = 0x4
  /* 0x05 - 0xFF Reserved for future use */
} bt_pf_avrcp_player_repeat_mode_settings_type;

/* AVRCP Metadata Media Player shuffle mode settings */
typedef enum {
  BT_PF_AVRCP_SHUFFLE_MODE_OFF        = 0x1,
  BT_PF_AVRCP_SHUFFLE_MODE_ALL_TRACKS = 0x2,
  BT_PF_AVRCP_SHUFFLE_MODE_GROUP      = 0x3
  /* 0x04 - 0xFF Reserved for future use */
} bt_pf_avrcp_player_shuffle_mode_settings_type;

/* AVRCP Metadata Media Player scan mode settings */
typedef enum {
  BT_PF_AVRCP_SCAN_MODE_OFF        = 0x1,
  BT_PF_AVRCP_SCAN_MODE_ALL_TRACKS = 0x2,
  BT_PF_AVRCP_SCAN_MODE_GROUP      = 0x3
  /* 0x04 - 0xFF Reserved for future use */
} bt_pf_avrcp_player_scan_mode_settings_type;

/* AVRCP Media element attributes */
typedef enum {
  BT_PF_AVRCP_MEDIA_ATTR_ILLEGAL            = 0x0,
  BT_PF_AVRCP_MEDIA_ATTR_TITLE              = 0x1,
  BT_PF_AVRCP_MEDIA_ATTR_ARTIST             = 0x2,
  BT_PF_AVRCP_MEDIA_ATTR_ALBUM              = 0x3,
  BT_PF_AVRCP_MEDIA_ATTR_TRACK_NUMBER       = 0x4,
  BT_PF_AVRCP_MEDIA_ATTR_TOTAL_TRACK_NUMBER = 0x5,
  BT_PF_AVRCP_MEDIA_ATTR_GENRE              = 0x6,
  BT_PF_AVRCP_MEDIA_ATTR_PLAYING_TIME       = 0x7
  /* 0x8-0xFFFFFFFF Reserved for future use  */
} bt_pf_avrcp_media_attributes_type;

/* AVRCP group navigation */
typedef enum {
  BT_PF_AVRCP_NEXT_GROUP = 0x00,
  BT_PF_AVRCP_PREV_GROUP = 0x01
} bt_pf_avrcp_group_navigation_type;

/* AVRCP capabiltiy id */
typedef enum {
  BT_PF_AVRCP_CAPABILITY_COMPANY_ID       = 0x2,
  BT_PF_AVRCP_CAPABILITY_EVENTS_SUPPORTED = 0x3
} bt_pf_avrcp_cap_id_type;

/* AVRCP PDU id */
typedef enum {
  /* Configuration PDUs */
  BT_PF_AVRCP_GET_CAPABILITIES                              = 0x10,
  /* Device setting PDUs */
  BT_PF_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES    = 0x11,
  BT_PF_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES        = 0x12,
  BT_PF_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE  = 0x13,
  BT_PF_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE          = 0x14,
  BT_PF_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT = 0x15,
  BT_PF_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT     = 0x16,
  BT_PF_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET              = 0x17,
  BT_PF_AVRCP_INFORM_BATTERY_STATUS_OF_CT                   = 0x18,
  /* Media information PDUs */
  BT_PF_AVRCP_GET_ELEMENT_ATTRIBUTES                        = 0x20,
  /* Notification PDUs */
  BT_PF_AVRCP_GET_PLAY_STATUS                               = 0x30,
  BT_PF_AVRCP_REGISTER_NOTIFICATION                         = 0x31,
  /* Continuing PDUs */
  BT_PF_AVRCP_REQUEST_CONTINUING_RESPONSE                   = 0x40,
  BT_PF_AVRCP_ABORT_CONTINUING_RESPONSE                     = 0x41
} bt_pf_avrcp_pdu_id_type;

/* Attribute pair and its value*/
typedef uint8* bt_pf_avrcp_attr_pair_type; 

/* It holds register notif event specific  data*/
typedef BT_PACKED struct {
  uint8 event_id;             /* An bt_pf_avrcp_metadata_transfer_events_type 
                                 constant. */
  BT_PACKED union {
    uint8 play_status;        /* An bt_pf_avrcp_playback_status_type  */
      /* Must be 8 bytes long. 0xFFFFFFFFFF if no track selected. */
    uint8 track_index[BT_PF_AVRCP_SIZE_OF_CURRENT_TRACK_ID];  
    uint32 playback_pos;      /* The current pos of the song in msecs elapsed.
                                     TG shall return 0xFFFFFFFF if not supported. */
    uint8 battery_status;     /* An bt_pf_avrcp_battery_status_type constant. */
    uint8 system_status;      /* An bt_pf_avrcp_system_status_type constant. */

    BT_PACKED struct {
      bt_pf_avrcp_attr_pair_type attributes;
      uint8 num_of_attr;
    } device_setting;
  } u; /* union */

} bt_pf_avrcp_notif_event_type;

typedef BT_PACKED struct {
  uint8 *p;             /* Pointer to an array of bytes containing the string */
  int32 num_of_bytes;   /* The number of bytes and not characters at p */
} bt_pf_avrcp_UTF_val_type;

/* It contains text value of attributes/attr. values */
typedef BT_PACKED struct {
  uint8                          item_id;
  uint16                         item_charset;
  bt_pf_avrcp_UTF_val_type       item_string_val;
} bt_pf_avrcp_attr_text_type;

/* It maintains the play status of the song */
typedef BT_PACKED struct {
  uint32                     song_length;
  uint32                     song_position;
  uint8                      status;
} bt_pf_avrcp_play_status_type;

/* It contains the information about the media element attribute */
typedef BT_PACKED struct {
  uint32                    media_attr_id;
  uint16                    char_set;
  bt_pf_avrcp_UTF_val_type  media_attr_val;
} bt_pf_avrcp_element_attr_type;

#endif /* FEATURE_BT_AVRCP_13 */
/*-------------------------------------------------------------------------*/
/*                      AVRCP command declarations                         */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_pf_avrcp_enable (
  bt_app_id_type                       app_id,
  uint8                                supported_categories
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_avrcp_enable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_avrcp_connect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_avrcp_connect */
/*~ PARAM IN bt_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_avrcp_disconnect (
  bt_app_id_type                       app_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_avrcp_disconnect */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_avrcp_disable (
  bt_app_id_type                       app_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_avrcp_disable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_avrcp_send_reply (
  bt_app_id_type                       app_id,
  uint8                                txn_id,
  uint8                                status,
  bt_event_type                        evt,
  bt_pf_avrcp_rsp_type                 response
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_avrcp_send_reply */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_avrcp_send_generic_reply (
  bt_app_id_type                       app_id,
  uint8                                txn_id,
  bt_pf_avrcp_generic_op_type*         frame
);

#ifdef FEATURE_BT_AVRCP_13

extern bt_cmd_status_type bt_cmd_pf_avrcp_metadata_enable (
  bt_app_id_type                                   app_id
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_eventid_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  uint8                                            event_count,
  bt_pf_avrcp_metadata_transfer_events_type*       event_ids
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_compid_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  uint8                                            comp_count,
  uint32*                                          comp_ids
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_list_attrid_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  uint8                                            attr_count,
  bt_pf_avrcp_player_app_settings_type*            attr_ids
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_list_attrvalue_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  bt_pf_avrcp_player_app_settings_type             attr_id,
  uint8                                            attr_val_count,
  uint8*                                           attr_settings_vals
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_get_attr_value_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  uint8                                            attr_id_count,
  bt_pf_avrcp_attr_pair_type                       attrs
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_get_attr_setting_text_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  uint8                                            attr_id_count,
  bt_pf_avrcp_attr_text_type*                      attrs_text
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_get_attr_setting_value_text_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  uint8                                            attr_val_count,
  bt_pf_avrcp_attr_text_type*                      attrs_text
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_play_status_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  bt_pf_avrcp_play_status_type*                    play_status
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_get_element_attributes_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  uint8                                            media_attr_count,
  bt_pf_avrcp_element_attr_type*                   media_element_attr
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_register_notification_reply (
  bt_app_id_type                                  app_id,
  uint8                                           txn,
  bt_pf_avrcp_notif_event_type*                   ev_metadata,
  boolean                                         is_final
);

extern bt_cmd_status_type bt_cmd_pf_avrcp_send_group_navigation_reply (
  bt_app_id_type                                 app_id,
  uint8                                          txn,
  uint16                                         vendor_unique_id,
  boolean                                        button_press,
  bt_pf_avrcp_rsp_type                           resp_code
);
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_simple_meta_reply (
  bt_app_id_type                                app_id,
  uint8                                         txn,
  uint8                                         pdu_id, 
  bt_cmd_status_type                            status /* This needs to be converted */
);

#endif /* FEATURE_BT_AVRCP_13 */
/*-------------------------------------------------------------------------*/
/*                      AVDTP specific declarations                        */
/*-------------------------------------------------------------------------*/

/* service capability category values */
#define BT_PF_AVDTP_CAP_MEDIA_TRANSPORT    1
#define BT_PF_AVDTP_CAP_REPORTING          2
#define BT_PF_AVDTP_CAP_RECOVERY           3
#define BT_PF_AVDTP_CAP_CONTENT_PROTECTION 4
#define BT_PF_AVDTP_CAP_ROHC               5
#define BT_PF_AVDTP_CAP_MUX                6
#define BT_PF_AVDTP_CAP_CODEC              7
#define BT_PF_AVDTP_CAP_BROADCAST          8

#define BT_PF_AVDTP_CAP_TOTAL              BT_PF_AVDTP_CAP_BROADCAST

/* endpoint type */
#define BT_PF_AVDTP_SRC  0
#define BT_PF_AVDTP_SNK  1

/* media types */
#define BT_PF_AVDTP_MEDIA_TYPE_AUDIO       0
#define BT_PF_AVDTP_MEDIA_TYPE_VIDEO       0x01
#define BT_PF_AVDTP_MEDIA_TYPE_MULTIMEDIA  0x02

/* codec type */
#define BT_PF_AVDTP_CODEC_SBC              0
#define BT_PF_AVDTP_CODEC_MPEG_AUDIO       0x01
#define BT_PF_AVDTP_CODEC_MPEG_AAC         0x02
#define BT_PF_AVDTP_CODEC_ATRAC            0x04
#define BT_PF_AVDTP_CODEC_NON_A2DP         0xFF

/* Reliability codes for BT_PF_AVDTP_RECV_DATA_IND */
#define BT_PF_AVDTP_RECV_NO_ERROR 0x0000
#define BT_PF_AVDTP_RECV_FRAME_LOST 0x0001
#define BT_PF_AVDTP_RECV_PACKET_RECOVERED 0x0002
#define BT_PF_AVDTP_RECV_LENGTH_MISMATCH 0x0003

/* Maximum number of capabilities per stream internal end point */
#define BT_PF_AVDTP_MAX_CAPS 2

/* Maximum number of capabilities per stream remote end point */
#define BT_PF_AVDTP_MAX_REMOTE_CAPS 7

/* Maximum number of remote end points */
#define BT_PF_AVDTP_MAX_REMOTE_RECORDS 4

/* AVDTP headers can have an additional 15 words for optional multicast data */
#define BT_AVDTP_MAX_HEADER_BYTES   (12)
#define BT_AVDTP_MAX_FOOTER_BYTES   (0)
#define BT_AVDTP_MAX_OVERHEAD_BYTES (BT_AVDTP_MAX_HEADER_BYTES +    \
                                     BT_AVDTP_MAX_FOOTER_BYTES)

/** the type for a data structure used in a discovery response */

/** the type for a structure describing the capabilities of a generic stream endpoint */
typedef BT_PACKED struct {
  /* the capability type */
  uint8 type;
  BT_PACKED union {
    /* the data for a codec capability (type == BT_PF_AVDTP_CAP_CODEC) */
    BT_PACKED struct {
      /* the media type */
      uint8 media_type;
      /* the codec type */
      uint8 codec_type;
      BT_PACKED union {
        BT_PACKED struct {
          /* codec-specific data */
          uint8 *data;
          /* the length of the codec-specific data */
          uint16 data_len;
        } generic_codec;
        audiosbcenc_data_type sbc_data;
      } data;
    } codec_data;

    BT_PACKED struct {
      /* Information Element specific data */
      uint8 *data;
      /* the length of the Information Element specific data */
      uint16 data_len;
    } generic_data;
  } data;
} bt_pf_avdtp_cap_type;

/** the type for a data structure used in a discovery response */
typedef BT_PACKED struct {
  /** the type of media supported by this endpoint */
  uint8 media_type;
  /** source or sink */
  uint8 endpoint_type;
  /** the stream endpoint identifier */
  uint8 seid;
  uint8 in_use;
} bt_pf_avdtp_discover_rsp_info;

/** A data structure of this type is used to register a stream endpoint. */
typedef BT_PACKED struct {
  /** the type of media supported by this endpoint */
  uint8 media_type;
  /** source or sink */
  uint8 endpoint_type;
  /** the array of capabilities for this endpoint */
  bt_pf_avdtp_cap_type *caps;
  /** the number of capabilities */
  uint16 numCaps;
} bt_pf_avdtp_endpoint_reg;

/*-------------------------------------------------------------------------*/
/*                      A2DP specific declarations                         */
/*-------------------------------------------------------------------------*/

// Used to inform AVS what bitrate to use.
typedef enum {
  BT_PF_A2DP_BITRATE_LOWEST,
  BT_PF_A2DP_BITRATE_LOW,
  BT_PF_A2DP_BITRATE_MEDIUM,
  BT_PF_A2DP_BITRATE_HIGH,
  BT_PF_A2DP_BITRATE_HIGHEST,
  BT_PF_A2DP_BITRATE_CUSTOM
} bt_pf_a2dp_bitrate_type;

// Used to control internal operation of A2DP
// Note: These values will be reset to the defaults when a2dp is enabled.
// Also, the bitrate will be reset to defaults on each connect.
typedef enum {
  BT_PF_A2DP_CTL_SUSPEND_ON_END, /* Send AVRCP_SUSPEND when streaming ends
                                  * Default: enabled */

  BT_PF_A2DP_CTL_CLOSE_ON_END,   /* Send AVRCP_CLOSE when streaming ends
                                  * Default: disabled */

  BT_PF_A2DP_CTL_START_DELAY,    /* Sets the delay between receiving a start
                                  * cfm and streaming data.
                                  * data1: timeout in ms.
                                  * Default: 500ms (must be bigger than 4) */

  BT_PF_A2DP_CTL_HONOR_IN_USE,   /* Honor the in_use flag of the remote
                                  * endpoint.
                                  * Default: enabled */

  BT_PF_A2DP_CTL_IGNORE_IN_USE,  /* Ignore the in_use flag of the remote
                                  * endpoint.
                                  * Default: disabled */

  BT_PF_A2DP_CTL_RECFG_BY_RECFG, /* Use AVRCP_RECONFIGURE to reconfigure streams
                                  * Default: disabled */

  BT_PF_A2DP_CTL_RECFG_BY_CLOSE, /* Use AVRCP_CLOSE to reconfigure streams
                                  * Default: enabled */

  BT_PF_A2DP_CTL_RECFG_BY_REPAGE,/* Disconnect/reconnect to reconfigure streams
                                  * Default: disabled */

  BT_PF_A2DP_CTL_INCR_BITRATE,   /* Request AVS to increase bitrate. Disables
                                  * auto bitrate selection */

  BT_PF_A2DP_CTL_DECR_BITRATE,   /* Request to AVS to decrease bitrate.
                                  * Disables auto bitrate selection */

  BT_PF_A2DP_CTL_SET_BITRATE,    /* Sets the bitrate to a particular value.
                                  * Disables auto bitrate selection.
                                  * data1: bitrate in kbps */

  BT_PF_A2DP_CTL_AUTO_BITRATE,   /* Return to auto bitrate selection. This is
                                  * the default */

  BT_PF_A2DP_CTL_LOWEST_BITRATE, /* Sets the lowest bitrate that AUTO_BITRATE
                                  * will use. Enables auto bitrate selection.
                                  * data1: bt_pf_a2dp_bitrate_type, not CUSTOM
                                  * Default: BT_PF_A2DP_BITRATE_LOW */

  BT_PF_A2DP_CTL_APP_START,      /* Determines if App (and not AVS) will
                                  * start the stream. */

  BT_PF_A2DP_CTL_LAST            /* Last element in the enum */
} bt_pf_a2dp_ctl_type;

/* Event type used in START and SUSPEND events.
 * LOCAL_INIT is used to indicate that the event is due to a command.
 * REMOTE_INIT is used for all other reasons, ie, due to signalling from
 * remote device, or a baseband timeout.
 */
typedef enum {
  BT_PF_A2DP_EVENT_LOCAL_INIT,
  BT_PF_A2DP_EVENT_REMOTE_INIT
} bt_pf_a2dp_event_type;


#ifdef FEATURE_BT_AVSYNC
/* To Keep track of the frames transmitted and their timestamps */
typedef struct {
  q_link_type                     link;
  uint64                          a2dp_last_seq;
  boolean                         last_frame;
  time_type                       sent_timestamp;
  /* Parameters for Callback */
  uint16                          l2cap_cid;
  void*                           mbuf;
  bt_cmd_status_type              result;
}bt_pf_a2dp_frame_handle_type;
#endif


// For SBC, the header is only 1 byte.
#define BT_A2DP_MAX_HEADER_BYTES   (1)
#define BT_A2DP_MAX_FOOTER_BYTES   (0)
#define BT_A2DP_MAX_OVERHEAD_BYTES (BT_A2DP_MAX_HEADER_BYTES +    \
                                     BT_A2DP_MAX_FOOTER_BYTES)

/*-------------------------------------------------------------------------*/
/*                      A2DP command declarations                          */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_pf_a2dp_enable (
  bt_app_id_type                       app_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_a2dp_enable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_a2dp_set_device (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_a2dp_set_device */
/*~ PARAM IN bt_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_a2dp_connect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_a2dp_connect */
/*~ PARAM IN bt_addr_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_a2dp_disconnect (
  bt_app_id_type                       app_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_a2dp_disconnect */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_a2dp_disable (
  bt_app_id_type                       app_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_a2dp_disable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_a2dp_start (
  bt_app_id_type                       app_id,
  audiosbcenc_data_type*               sbc_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_a2dp_start */
/*~ PARAM IN sbc_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_a2dp_suspend (
  bt_app_id_type                       app_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_a2dp_suspend */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_a2dp_send_data (
  bt_app_id_type                       app_id,
  dsm_item_type*                       data,
  uint32                               seq_num,
  uint32                               timestamps_per_frame,
  uint8                                num_frames,
  uint16                               frame_len,
  boolean                              last_frame,
  uint16                               handle
);

extern bt_cmd_status_type bt_cmd_pf_a2dp_ctl (
  bt_app_id_type                       app_id,
  bt_pf_a2dp_ctl_type                  ctl,
  uint32                               data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_a2dp_ctl */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern bt_cmd_status_type bt_cmd_pf_a2dp_register_avs (
  bt_app_id_type                       app_id
);

extern bt_cmd_status_type bt_cmd_pf_a2dp_flush (
  bt_app_id_type                       app_id
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION bt_cmd_pf_a2dp_flush */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*-------------------------------------------------------------------------*/
/*     AVDTP command declarations  - INTERNAL USE                          */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_cmd_pf_avdtp_register (
  bt_app_id_type                       app_id
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_deregister (
  bt_app_id_type                       app_id
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_register_endpoint (
  bt_app_id_type                       app_id,
  uint8                                media_type,
  uint8                                endpoint_type,
  const bt_pf_avdtp_cap_type*          caps,
  uint8                                num_caps,
  uint8*                               seid
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_deregister_endpoint (
  bt_app_id_type                       app_id,
  uint8                                seid
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_connect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_accept (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  boolean                              accept
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_disconnect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_discover (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr,
  uint8*                               transation
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_get_cap (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr,
  uint8                                remote_seid,
  uint8*                               transaction
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_set_config (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr,
  uint8                                remote_seid,
  uint8                                local_seid,
  bt_pf_avdtp_cap_type                 caps[BT_PF_AVDTP_MAX_CAPS],
  uint8                                num_caps,
  uint8*                               transaction,
  uint16*                              handle
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_get_config (
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transation
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_set_config_rsp (
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8                                transaction,
  uint8                                category,
  bt_cmd_status_type                   error
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_open (
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_start (
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_close (
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_abort (
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_suspend (
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaciton
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_reconfigure (
  bt_app_id_type                       app_id,
  uint16                               handle,
  bt_pf_avdtp_cap_type                 caps[BT_PF_AVDTP_MAX_REMOTE_CAPS],
  uint8                                num_caps,
  uint8*                               transaction
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_reconfigure_rsp (
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8                                transaction,
  uint8                                category,
  bt_cmd_status_type                   error
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_send_data (
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint16                               seq_num,
  uint32                               timestamp,
  uint8                                payload_type,
  uint16                               marker,
  dsm_item_type*                       data,
  void*                                frame_handle
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_security_control (
  bt_app_id_type                       app_id,
  uint16                               handle,
  dsm_item_type*                       data,
  uint8*                               transaction
);

extern bt_cmd_status_type bt_cmd_pf_avdtp_security_control_rsp (
  bt_app_id_type                       app_id,
  uint8                                transaction,
  dsm_item_type*                       data,
  bt_cmd_status_type                   error
);
#endif /* FEATURE_BT_EXTPF_AV */

/*-------------------------------------------------------------------------*/
/*                      PBAP specific declarations                         */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_PBAP

/* the following must be included after bt_cmd_status_type is defined */
#include "pbap_cli.h"

#define BT_PF_PBAP_NO_CONN_ID     0xFFFF

#define BT_PF_PBAP_OBJSYS_OPS_TMO 1000 /* ms */

#define BT_PF_PBAP_OBJ_FORMAT_VCARD_2_1 0x01
#define BT_PF_PBAP_OBJ_FORMAT_VCARD_3_0 0x02

typedef enum
{
  BT_PF_PBAP_CLI_SET_PATH_TO_FOLDER = PBAP_CLIENT_SETPATH_TO_FOLDER,
  BT_PF_PBAP_CLI_SET_PATH_TO_PARENT = PBAP_CLIENT_SETPATH_TO_PARENT,
  BT_PF_PBAP_CLI_SET_PATH_TO_ROOT   = PBAP_CLIENT_SETPATH_TO_ROOT
} bt_pf_pbap_cli_set_path_type;

typedef enum
{
  BT_PF_PBAP_SRV_REQ_INITIAL  = PBAP_REQUEST_INITIAL,
  BT_PF_PBAP_SRV_REQ_CONTINUE = PBAP_REQUEST_CONTINUE,
  BT_PF_PBAP_SRV_REQ_CLEANUP  = PBAP_REQUEST_CLEANUP
} bt_pf_pbap_srv_req_state_type;

typedef void* bt_pf_pbap_handle_type;

typedef uint16 bt_pf_pbap_cli_conn_id_type;
typedef uint16 bt_pf_pbap_srv_conn_id_type;

/*-------------------------------------------------------------------------*/
/*                     PBAP command declarations                           */
/*-------------------------------------------------------------------------*/
extern bt_cmd_status_type bt_cmd_pf_pbap_cli_connect(
  bt_app_id_type                app_id,
  bt_bd_addr_type*              bd_addr_ptr,
  uint8                         channel );

extern bt_cmd_status_type bt_cmd_pf_pbap_cli_disconnect(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id );

extern bt_cmd_status_type bt_cmd_pf_pbap_cli_auth_response(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  byte*                         user_id_ptr,
  uint8                         user_id_len,
  char*                         pin_ptr );

extern bt_cmd_status_type bt_cmd_pf_pbap_cli_register(
  bt_app_id_type                app_id,
  char*                         cli_name_str );

extern bt_cmd_status_type bt_cmd_pf_pbap_cli_deregister(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_cmd_pf_pbap_cli_abort(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id );

extern bt_cmd_status_type bt_cmd_pf_pbap_cli_pull_phone_book(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id,
  uint16*                       name_ptr,
  uint64*                       attribute_mask_ptr,
  uint8*                        format_ptr,
  uint16*                       max_list_count_ptr,
  uint16*                       list_start_offset_ptr );

extern bt_cmd_status_type bt_cmd_pf_pbap_cli_set_phone_book(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id,
  uint16*                       folder_ptr,
  bt_pf_pbap_cli_set_path_type  set_path_ctrl );

extern bt_cmd_status_type bt_cmd_pf_pbap_cli_pull_vcard_listing(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id,
  uint16*                       name_ptr,
  uint8*                        order,
  char*                         value_ptr,
  uint8*                        search_attrib_ptr,
  uint16*                       max_list_count_ptr,
  uint16*                       list_start_offset_ptr );

extern bt_cmd_status_type bt_cmd_pf_pbap_cli_pull_vcard_entry(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id,
  uint16*                       name_ptr,
  uint64*                       attribute_mask_ptr,
  uint8*                        format_ptr );

extern bt_cmd_status_type bt_cmd_pf_pbap_srv_register(
  bt_app_id_type                app_id,
  char*                         srv_name_str,
  uint8                         supported_repositories,
  bt_pf_goep_srv_auth_type      auth );

extern bt_cmd_status_type bt_cmd_pf_pbap_srv_deregister(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_cmd_pf_pbap_srv_auth_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  char*                         pin_ptr );

extern bt_cmd_status_type bt_cmd_pf_pbap_srv_force_disconnect(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id );

extern bt_cmd_status_type bt_cmd_pf_pbap_srv_accept_connect(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  boolean                       accept );

extern bt_cmd_status_type bt_cmd_pf_pbap_srv_pull_phone_book_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  bt_pf_byte_seq_type*          data_ptr,
  uint16*                       phone_book_size_ptr,
  uint8*                        num_missed_calls_ptr,
  boolean                       final,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_pbap_srv_set_phone_book_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_pbap_srv_pull_vcard_listing_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  bt_pf_byte_seq_type*          data_ptr,
  uint16*                       phone_book_size_ptr,
  uint8*                        num_missed_calls_ptr,
  boolean                       final,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_cmd_pf_pbap_srv_pull_vcard_entry_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  bt_pf_byte_seq_type*          data_ptr,
  boolean                       final,
  bt_cmd_status_type            status );

#endif /* FEATURE_BT_EXTPF_PBAP */

#ifdef FEATURE_BT_EXTPF_HID_HOST 
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           BT PROFILE EVENTS                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*        Generic Object Exchange Profile (GOEP) Event Messages            */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_GOEP

typedef BT_PACKED struct
{
  bt_pf_goep_cli_conn_id_type    conn_id;
  boolean                        read_only;
  uint8                          rsp_headers_cnt;
  bt_pf_goep_obex_hdr_type*      rsp_headers_ptr;
  bt_cmd_status_type             status;
} bt_pf_ev_goep_cli_con_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_goep_cli_conn_id_type    conn_id;
  uint8                          rsp_headers_cnt;
  bt_pf_goep_obex_hdr_type*      rsp_headers_ptr;
} bt_pf_ev_goep_cli_dcn_ind_type;

typedef BT_PACKED struct
{
  bt_pf_goep_cli_conn_id_type    conn_id;
  uint8                          rsp_headers_cnt;
  bt_pf_goep_obex_hdr_type*      rsp_headers_ptr;
  bt_cmd_status_type             status;
} bt_pf_ev_goep_cli_put_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_goep_cli_conn_id_type    conn_id;
  uint8                          rsp_headers_cnt;
  bt_pf_goep_obex_hdr_type*      rsp_headers_ptr;
  bt_cmd_status_type             status;
} bt_pf_ev_goep_cli_get_rcv_data_type;

typedef BT_PACKED struct
{
  bt_pf_goep_cli_conn_id_type    conn_id;
  uint8                          rsp_headers_cnt;
  bt_pf_goep_obex_hdr_type*      rsp_headers_ptr;
  bt_cmd_status_type             status;
} bt_pf_ev_goep_cli_set_path_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_goep_cli_conn_id_type    conn_id;
  boolean                        user_id_required;
  boolean                        full_access;
} bt_pf_ev_goep_cli_auth_ind_type;


typedef BT_PACKED struct
{
  bt_pf_goep_cli_conn_id_type    conn_id;
} bt_pf_ev_goep_cli_auth_res_ind_type;


typedef BT_PACKED struct
{
  uint8                          ch_num;
  uint8                          target_id;
  bt_pf_goep_srv_conn_id_type    conn_id;
  boolean                        unauthorized;
  byte                           user_id[BT_PF_OBEX_MAX_USERID_LEN];
  uint8                          user_id_len;
  uint8                          cmd_headers_cnt;
  bt_pf_goep_obex_hdr_type*      cmd_headers_ptr;
} bt_pf_ev_goep_srv_con_ind_type;


typedef BT_PACKED struct
{
    bt_pf_goep_srv_conn_id_type    conn_id;
    
    uint8                          ch_num;
    uint8                          target_id;
    boolean                        user_id_rqd;
    boolean                        full_access;
    uint8                          realm_str[BT_PF_OBEX_MAX_REALM_LEN];
    uint8                          realm_len;
    uint8                          realm_str_type ;
    byte                           user_id[BT_PF_OBEX_MAX_USERID_LEN];
    uint8                          user_id_len;
} bt_pf_ev_goep_srv_auth_req_ind_type;


typedef BT_PACKED struct
{
  uint8                          ch_num;
  uint8                          target_id;
  bt_pf_goep_srv_conn_id_type    conn_id;
  uint8                          cmd_headers_cnt;
  bt_pf_goep_obex_hdr_type*      cmd_headers_ptr;
} bt_pf_ev_goep_srv_dcn_ind_type;

typedef BT_PACKED struct
{
  uint8                          ch_num;
  uint8                          target_id;
  bt_pf_goep_srv_conn_id_type    conn_id;
  uint8                          cmd_headers_cnt;
  bt_pf_goep_obex_hdr_type*      cmd_headers_ptr;
  bt_cmd_status_type             status;
} bt_pf_ev_goep_srv_get_ind_type;

typedef BT_PACKED struct
{
  uint8                          ch_num;
  uint8                          target_id;
  bt_pf_goep_srv_conn_id_type    conn_id;
  uint8                          cmd_headers_cnt;
  bt_pf_goep_obex_hdr_type*      cmd_headers_ptr;
  bt_cmd_status_type             status;
} bt_pf_ev_goep_srv_put_ind_type;

typedef BT_PACKED struct
{
  uint8                          ch_num;
  uint8                          target_id;
  bt_pf_goep_srv_conn_id_type    conn_id;
  uint16*                        folder_ptr;
  boolean                        up_level;
  boolean                        create;
  uint8                          cmd_headers_cnt;
  bt_pf_goep_obex_hdr_type*      cmd_headers_ptr;
} bt_pf_ev_goep_srv_set_path_ind_type;

#endif /* FEATURE_BT_EXTPF_GOEP */

/*-------------------------------------------------------------------------*/
/*               Object Push Profile (OPP) Event Messages                  */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_OPP

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_opp_cli_con_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_opp_cli_con_prog_ind_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_opp_cli_dcn_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_opp_cli_push_done_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_opp_cli_pull_done_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  uint16                         name_str[BT_PF_MAX_FILENAME_LEN + 1];
  char                           type_str[BT_PF_MAX_MIME_TYPE_LEN + 1];
  uint8                          name_len;
} bt_pf_ev_opp_cli_open_read_req_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  uint32                         obj_size;
  uint16                         name_str[BT_PF_MAX_FILENAME_LEN + 1];
  char                           type_str[BT_PF_MAX_MIME_TYPE_LEN + 1];
  uint8                          name_len;
} bt_pf_ev_opp_cli_open_write_req_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  bt_pf_opp_handle_type          handle;
  bt_cmd_status_type             status;
} bt_pf_ev_opp_cli_close_req_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  bt_pf_opp_handle_type          handle;
  uint16                         max_read;
} bt_pf_ev_opp_cli_read_req_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type     conn_id;
  bt_pf_opp_handle_type          handle;
  byte*                          buffer_ptr;
  uint16                         buffer_len;
} bt_pf_ev_opp_cli_write_req_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type     conn_id;
  bt_bd_addr_type                bd_addr;
} bt_pf_ev_opp_srv_con_ind_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type     conn_id;
} bt_pf_ev_opp_srv_dcn_ind_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type     conn_id;
  uint16                         name_str[BT_PF_MAX_FILENAME_LEN + 1];
  char                           type_str[BT_PF_MAX_MIME_TYPE_LEN + 1];
  uint8                          name_len;
} bt_pf_ev_opp_srv_open_read_req_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type     conn_id;
  uint32                         obj_size;
  uint16                         name_str[BT_PF_MAX_FILENAME_LEN + 1];
  char                           type_str[BT_PF_MAX_MIME_TYPE_LEN + 1];
  uint8                          name_len;
} bt_pf_ev_opp_srv_open_write_req_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type     conn_id;
  bt_pf_opp_handle_type          handle;
  bt_cmd_status_type             status;
} bt_pf_ev_opp_srv_close_req_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type     conn_id;
  bt_pf_opp_handle_type          handle;
  uint16                         max_read;
} bt_pf_ev_opp_srv_read_req_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type     conn_id;
  bt_pf_opp_handle_type          handle;
  byte*                          buffer_ptr;
  uint16                         buffer_len;
} bt_pf_ev_opp_srv_write_req_type;

#endif /* FEATURE_BT_EXTPF_OPP */

/*-------------------------------------------------------------------------*/
/*              File Transfer Profile (FTP) Event Messages                 */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_FTP

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_con_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_con_read_only_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_dcn_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  boolean                        user_id_required;
} bt_pf_ev_ftp_cli_auth_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_put_dne_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_delete_dne_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_get_rsp_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_set_path_dne_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_new_subfolder_dne_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  uint16                         folder_list_data_len;
  byte*                          folder_list_data_ptr;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_get_folder_list_rsp_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_tree_put_dne_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_tree_get_rsp_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  uint16                         folder_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          folder_name_len;
} bt_pf_ev_ftp_cli_browse_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_pf_ftp_mode_type            mode;
  uint16                         name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          name_len;
} bt_pf_ev_ftp_cli_open_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_pf_ftp_handle_type          handle;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_cli_close_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_pf_ftp_handle_type          handle;
  uint16                         max_read;
} bt_pf_ev_ftp_cli_read_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  bt_pf_ftp_handle_type          handle;
  byte*                          buffer_ptr;
  uint16                         buffer_len;
  uint32                         obj_size;
} bt_pf_ev_ftp_cli_write_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type     conn_id;
  boolean                        create;
  uint8                          level;
  uint16                         folder_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          folder_name_len;
} bt_pf_ev_ftp_cli_set_folder_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type     conn_id;
  bt_bd_addr_type                bd_addr;
  boolean                        unauthorized;
  byte                           user_id[BT_PF_OBEX_MAX_USERID_LEN];
  uint8                          user_id_len;
} bt_pf_ev_ftp_srv_con_ind_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type     conn_id;
} bt_pf_ev_ftp_srv_dcn_ind_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type     conn_id;
  uint16                         folder_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          folder_name_len;
} bt_pf_ev_ftp_srv_browse_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type     conn_id;
  bt_pf_ftp_mode_type            mode;
  uint16                         name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          name_len;
  uint32                         obj_size;
} bt_pf_ev_ftp_srv_open_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type     conn_id;
  bt_pf_ftp_handle_type          handle;
  bt_cmd_status_type             status;
} bt_pf_ev_ftp_srv_close_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type     conn_id;
  bt_pf_ftp_handle_type          handle;
  uint16                         max_read;
} bt_pf_ev_ftp_srv_read_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type     conn_id;
  bt_pf_ftp_handle_type          handle;
  byte*                          buffer_ptr;
  uint16                         buffer_len;
} bt_pf_ev_ftp_srv_write_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type     conn_id;
  uint16                         name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          name_len;
} bt_pf_ev_ftp_srv_delete_req_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type     conn_id;
  boolean                        create;
  uint8                          level;
  uint16                         folder_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          folder_name_len;
} bt_pf_ev_ftp_srv_set_folder_req_type;

#endif /* FEATURE_BT_EXTPF_FTP */

/*-------------------------------------------------------------------------*/
/*              Basic Imaging Profile (BIP) Event Messages                 */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_BIP

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_cli_con_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
} bt_pf_ev_bip_cli_dcn_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
} bt_pf_ev_bip_cli_auth_req_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_get_capabilities_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_put_image_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_put_thumbnail_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_put_attachment_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  uint16                         num_returned_handles;
  byte*                          image_handles_descriptor_ptr;
  uint16                         image_handles_descriptor_len;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_get_list_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_get_properties_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  uint32                         file_size;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_get_image_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_get_thumbnail_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_get_attachment_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_delete_image_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_get_status_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  uint32                         total_file_size;
  boolean                        end_flag;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_get_partial_image_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_start_print_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_start_archive_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_remote_display_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bip_cli_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_cmd_status_type             result;
} bt_pf_ev_bip_cli_get_monitor_image_cfm_type;


typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_bd_addr_type                bd_addr;
} bt_pf_ev_bip_srv_con_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
} bt_pf_ev_bip_srv_dcn_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_target_type          target;
  bt_pf_bip_srv_conn_id_type     conn_id;
} bt_pf_ev_bip_srv_auth_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_get_capabilities_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  uint16                         image_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          image_name_len;
  byte*                          image_descriptior_ptr;
  uint16                         image_descriptior_len;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_put_image_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_put_thumbnail_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  byte*                          attachment_descriptor_ptr;
  uint16                         attachment_descriptor_len;
  byte*                          data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_put_attachment_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  uint16                         handle_count;
  uint16                         list_offset;
  uint8                          latest;
  byte*                          handles_descriptor_ptr;
  uint16                         handles_descriptor_len;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_get_list_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_get_properties_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  byte*                          image_descriptor_ptr;
  uint16                         image_descriptor_len;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_get_image_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_get_thumbnail_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  uint16                         attachment_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          attachment_name_len;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_get_attachment_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_delete_image_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_get_status_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  uint32                         partial_length;
  uint32                         partial_offset;
  uint16                         image_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          image_name_len;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_get_partial_image_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_sd_uuid128_type             arch_obj_srv_service_id;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_start_archive_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  bt_pf_bip_image_handle_type    image_handle;
  uint8                          display_command;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_remote_display_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bip_srv_conn_id_type     conn_id;
  boolean                        store_image;
  bt_pf_bip_srv_req_state_type   state;
  bt_cmd_status_type             status;
} bt_pf_ev_bip_srv_get_monitor_image_ind_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF_BIP */

/*-------------------------------------------------------------------------*/
/*              Basic Printing Profile (BPP) Event Messages                 */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_BPP

typedef BT_PACKED struct
{
  bt_pf_bpp_target_type          target;
  bt_pf_bpp_sndr_conn_id_type    conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_bpp_sndr_con_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bpp_sndr_conn_id_type    conn_id;
  bt_bd_addr_type                bd_addr;
} bt_pf_ev_bpp_sndr_con_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bpp_sndr_conn_id_type    conn_id;
} bt_pf_ev_bpp_sndr_dcn_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bpp_sndr_conn_id_type    conn_id;
} bt_pf_ev_bpp_sndr_srv_dcn_ind_type;

typedef BT_PACKED struct
{
  bt_pf_bpp_target_type          target;
  boolean                        user_id_required;
} bt_pf_ev_bpp_sndr_auth_req_type;

typedef BT_PACKED struct
{
  bt_pf_bpp_sndr_conn_id_type    conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_bpp_sndr_send_file_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bpp_sndr_conn_id_type    conn_id;
  byte*                          url_ptr;
  uint16                         url_len;
  byte*                          http_challenge_ptr;
  uint16                         http_challenge_len;
  bt_cmd_status_type             status;
} bt_pf_ev_bpp_sndr_send_reference_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bpp_sndr_conn_id_type    conn_id;
  byte*                          reply_ptr;
  uint16                         reply_len;
  bt_pf_bpp_job_id_type          job_id;
  bt_cmd_status_type             status;
  boolean                        final;
} bt_pf_ev_bpp_sndr_get_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_bpp_sndr_conn_id_type    conn_id;
  uint32                         offset;
  int32                          count;
  boolean                        get_file_size;
  bt_cmd_status_type             status;
  uint16                         name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          name_len;
} bt_pf_ev_bpp_sndr_get_ref_obj_ind_type;

#endif /* FEATURE_BT_EXTPF_BPP */

/*-------------------------------------------------------------------------*/
/*                     SML  Event Messages                                 */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_SML

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type         conn_id;
  boolean                        unauthorized;
  byte                           user_id[BT_PF_OBEX_MAX_USERID_LEN];
  uint8                          user_id_len;
} bt_pf_ev_sml_cli_con_ind_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type         conn_id;
} bt_pf_ev_sml_cli_dcn_ind_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type         conn_id;
  bt_pf_goep_obex_hdr_type*      cmd_headers_ptr;
  uint8                          cmd_headers_cnt;
  bt_cmd_status_type             status;
} bt_pf_ev_sml_cli_get_ind_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type         conn_id;
  bt_pf_goep_obex_hdr_type*      cmd_headers_ptr;
  uint8                          cmd_headers_cnt;
  bt_cmd_status_type             status;
} bt_pf_ev_sml_cli_put_ind_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type         conn_id;
  boolean                        unauthorized;
  byte                           user_id[BT_PF_OBEX_MAX_USERID_LEN];
  uint8                          user_id_len;
} bt_pf_ev_sml_srv_con_ind_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type         conn_id;
} bt_pf_ev_sml_srv_dcn_ind_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type         conn_id;
  bt_pf_goep_obex_hdr_type*      cmd_headers_ptr;
  uint8                          cmd_headers_cnt;
  bt_cmd_status_type             status;
} bt_pf_ev_sml_srv_get_ind_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type         conn_id;
  bt_pf_goep_obex_hdr_type*      cmd_headers_ptr;
  uint8                          cmd_headers_cnt;
  bt_cmd_status_type             status;
} bt_pf_ev_sml_srv_put_ind_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type          conn_id;
  boolean                         read_only;
  bt_cmd_status_type              status;
} bt_pf_ev_sml_con_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type          conn_id;
} bt_pf_ev_sml_dcn_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type          conn_id;
  boolean                         user_id_required;
} bt_pf_ev_sml_auth_ind_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type          conn_id;
  bt_pf_goep_obex_hdr_type*       rsp_headers_ptr;
  uint16                          rsp_headers_cnt;
  bt_cmd_status_type              status;
} bt_pf_ev_sml_get_rcv_data_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type          conn_id;
  bt_pf_goep_obex_hdr_type*       rsp_headers_ptr;
  uint16                          rsp_headers_cnt;
  bt_cmd_status_type              status;
} bt_pf_ev_sml_put_cfm_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF_SML */

/*-------------------------------------------------------------------------*/
/*   Cordless Telephony and Intercom Profiles (CTP/ICP) Event Messages     */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_CTP

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_ev_ctp_connect_ind_type;
typedef bt_pf_ev_ctp_connect_ind_type bt_pf_ev_icp_connect_ind_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_cmd_status_type                  status;
} bt_pf_ev_ctp_connect_cfm_type;
typedef bt_pf_ev_ctp_connect_cfm_type bt_pf_ev_icp_connect_cfm_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_ev_ctp_disconnect_cfm_type;
typedef bt_pf_ev_ctp_disconnect_cfm_type bt_pf_ev_icp_disconnect_cfm_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_pf_ctp_connect_info_type         info;
} bt_pf_ev_ctp_establish_ind_type;
typedef bt_pf_ev_ctp_establish_ind_type bt_pf_ev_icp_establish_ind_type;


typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_cmd_status_type                  status;
} bt_pf_ev_ctp_establish_cfm_type;
typedef bt_pf_ev_ctp_establish_cfm_type bt_pf_ev_icp_establish_cfm_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  boolean                             sending_complete;
  char                                keypad_facility;
  bt_pf_ctp_number_type               calling_number;
  bt_pf_ctp_number_type               called_number;
  bt_pf_ctp_audio_control_type        audio_control;
} bt_pf_ev_ctp_info_ind_type;
typedef bt_pf_ev_ctp_info_ind_type bt_pf_ev_icp_info_ind_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  char                                tone;
} bt_pf_ev_ctp_dtmf_start_type;
typedef bt_pf_ev_ctp_dtmf_start_type bt_pf_ev_icp_dtmf_start_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_ev_ctp_dtmf_stop_type;
typedef bt_pf_ev_ctp_dtmf_stop_type bt_pf_ev_icp_dtmf_stop_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_pf_ctp_cause_type                cause;
} bt_pf_ev_ctp_release_ind_type;
typedef bt_pf_ev_ctp_release_ind_type bt_pf_ev_icp_release_ind_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_pf_ctp_call_status_type          status;
} bt_pf_ev_ctp_outgoing_status_type;
typedef bt_pf_ev_ctp_outgoing_status_type bt_pf_ev_icp_outgoing_status_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_CTP */


/*-------------------------------------------------------------------------*/
/*   SIM Access Profile (SAP) Event Messages                               */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_ev_sap_con_req_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_cmd_status_type                  con_stat;
  uint16                              max_msg_size;
} bt_pf_ev_sap_con_result_type;

// bt_pf_ev_sap_dcn_req_type has no body
// bt_pf_ev_sap_dcn_rsp_type has no body

typedef BT_PACKED struct
{
  boolean                             immediately;
} bt_pf_ev_sap_dcn_ind_type;


typedef BT_PACKED struct
{
  uint8*         req_dst_ptr;
  uint16         req_dst_len;
} bt_pf_ev_sap_tx_apdu_req_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD  bt_pf_ev_sap_tx_apdu_req_type.req_dst_ptr VARRAY LENGTH bt_pf_ev_sap_tx_apdu_req_type.req_dst_len */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type          result_code;
} bt_pf_ev_sap_tx_apdu_rsp_type;

// bt_pf_ev_sap_tx_atr_req_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type          result_code;
} bt_pf_ev_sap_tx_atr_rsp_type;

// bt_pf_ev_sap_pwr_sim_off_req_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type          result_code;
} bt_pf_ev_sap_pwr_sim_off_rsp_type;

// bt_pf_ev_sap_pwr_sim_on_req_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type          result_code;
} bt_pf_ev_sap_pwr_sim_on_rsp_type;

// bt_pf_ev_sap_reset_sim_req_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type          result_code;
} bt_pf_ev_sap_reset_sim_rsp_type;

// bt_pf_ev_sap_card_reader_status_req_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type          result_code;
  uint8                               status;
} bt_pf_ev_sap_card_reader_status_rsp_type;

typedef BT_PACKED struct
{
  bt_pf_sap_sim_status_type           status;
} bt_pf_ev_sap_status_ind_type;


#ifdef FEATURE_BT_EXTPF_AV
/*-------------------------------------------------------------------------*/
/*                        A2DP event messages                              */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  audiosbcenc_data_type    sbc_config;
} bt_pf_ev_a2dp_con_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  bt_cmd_status_type       reason;
} bt_pf_ev_a2dp_con_failed_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  bt_cmd_status_type       reason;
} bt_pf_ev_a2dp_discon_type;

typedef BT_PACKED struct {
  bt_pf_a2dp_event_type    origin;
  bt_bd_addr_type          bd_addr;
} bt_pf_ev_a2dp_start_type;

typedef BT_PACKED struct {
  bt_pf_a2dp_event_type    origin;
  bt_bd_addr_type          bd_addr;
} bt_pf_ev_a2dp_suspend_type;

typedef BT_PACKED struct {
  bt_pf_a2dp_event_type    origin;
  bt_bd_addr_type          bd_addr;
} bt_pf_ev_a2dp_open_type;

typedef BT_PACKED struct {
  bt_pf_a2dp_event_type    origin;
  bt_bd_addr_type          bd_addr;
} bt_pf_ev_a2dp_close_type;

typedef BT_PACKED struct {
  bt_pf_a2dp_bitrate_type  bitrate_hint;
  uint32                   custom_bitrate;
  bt_bd_addr_type          bd_addr;
} bt_pf_ev_a2dp_bitrate_type;

/*-------------------------------------------------------------------------*/
/*                        AVRCP event messages                             */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
} bt_pf_ev_avrcp_con_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  bt_cmd_status_type       reason;
} bt_pf_ev_avrcp_con_failed_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  bt_cmd_status_type       reason;
} bt_pf_ev_avrcp_discon_type;

typedef BT_PACKED struct {
  uint8                      txn_id;
  bt_pf_avrcp_op_status_type status;
} bt_pf_ev_avrcp_op_type;

typedef bt_pf_avrcp_generic_op_type bt_pf_ev_avrcp_generic_op_type;

#ifdef FEATURE_BT_AVRCP_13
  /* AVRCP meta data event structures definitions */

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint8                    cap_id;
} bt_pf_ev_avrcp_capability_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
} bt_pf_ev_avrcp_list_attributes_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint8                    attr_id;
} bt_pf_ev_avrcp_list_attribute_values_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  uint8                       attr_count;
  uint8*                      attr_ids;
} bt_pf_ev_avrcp_get_attribute_values_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  uint8                       attr_id_count;
  bt_pf_avrcp_attr_pair_type  attrs;  
} bt_pf_ev_avrcp_set_attribute_values_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint8                    attr_id;
  uint8                    attr_val_count;
  uint8*                   attr_vals;
} bt_pf_ev_avrcp_get_attribute_setting_value_text_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint8                    charset_count;
  uint16*                  charset_ids;
} bt_pf_ev_avrcp_inform_character_set_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint8                    batt_status;
} bt_pf_ev_avrcp_inform_battery_status_of_ct_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint8*                   media_element_id;                    
  uint8                    elements_count;
  uint32*                  media_attrbutes;
} bt_pf_ev_avrcp_get_element_attribute_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
} bt_pf_ev_avrcp_get_play_status_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint8                    notif_event_id;
  uint32                   playback_interval; 
} bt_pf_ev_avrcp_event_notification_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint8                    pdu_id;
  uint16                   curr_attr_index;
  uint16                   curr_data_pos;
} bt_pf_ev_avrcp_continue_response_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint8                    pdu_id;
} bt_pf_ev_avrcp_abort_response_command_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  uint8                    txn;
  uint16                   vendor_unique;
  boolean                  button_press;
} bt_pf_ev_avrcp_group_navigation_command_ind_type;

typedef bt_pf_ev_avrcp_get_attribute_values_command_ind_type bt_pf_ev_avrcp_get_attribute_setting_text_command_ind_type;
            
           
#endif /* FEATURE_BT_AVRCP_13 */

/*-------------------------------------------------------------------------*/
/*                        AVDTP event messages                             */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
} bt_ev_pf_avdtp_con_ind_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  bt_cmd_status_type       result;
} bt_ev_pf_avdtp_con_cfm_type;

typedef BT_PACKED struct {
  bt_bd_addr_type          bd_addr;
  bt_cmd_status_type       reason;
} bt_ev_pf_avdtp_discon_ind_type;

typedef BT_PACKED struct {
  uint8                         txn;
  bt_bd_addr_type               bd_addr;
  bt_pf_avdtp_discover_rsp_info records[BT_PF_AVDTP_MAX_REMOTE_RECORDS];
  uint8                         num_records;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_disc_cfm_type;

typedef BT_PACKED struct {
  uint8                         txn;
  bt_bd_addr_type               bd_addr;
  uint8                         remote_seid;
  bt_pf_avdtp_cap_type          caps[BT_PF_AVDTP_MAX_REMOTE_CAPS];
  uint8                         num_caps;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_get_cap_cfm_type;

typedef BT_PACKED struct {
  uint8                         txn;
  bt_bd_addr_type               bd_addr;
  uint8                         local_seid;
  uint8                         remote_seid;
  uint16                        handle;
  bt_pf_avdtp_cap_type          caps[BT_PF_AVDTP_MAX_CAPS];
  uint8                         num_caps;
} bt_ev_pf_avdtp_set_cfg_ind_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  uint8                         category;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_set_cfg_cfm_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  bt_pf_avdtp_cap_type          caps[BT_PF_AVDTP_MAX_REMOTE_CAPS];
  uint8                         num_caps;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_get_cfg_cfm_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  uint16                        out_mtu;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_open_cfm_type;

typedef BT_PACKED struct {
  uint16                        handle;
  uint16                        out_mtu;
} bt_ev_pf_avdtp_open_ind_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
} bt_ev_pf_avdtp_close_ind_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_close_cfm_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
} bt_ev_pf_avdtp_start_ind_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_start_cfm_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
} bt_ev_pf_avdtp_suspend_ind_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_suspend_cfm_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  bt_pf_avdtp_cap_type          caps[BT_PF_AVDTP_MAX_REMOTE_CAPS];
  uint8                         num_caps;
} bt_ev_pf_avdtp_recfg_ind_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  uint8                         category;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_recfg_cfm_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
} bt_ev_pf_avdtp_abort_ind_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_abort_cfm_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  dsm_item_type*                dsm_ptr;
} bt_ev_pf_avdtp_sec_ind_type;

typedef BT_PACKED struct {
  uint8                         txn;
  uint16                        handle;
  dsm_item_type*                dsm_ptr;
  bt_cmd_status_type            error;
} bt_ev_pf_avdtp_sec_cfm_type;

typedef BT_PACKED struct {
  uint16                        handle;
  uint16                        seq_num;
  uint32                        timestamp;
  dsm_item_type*                dsm_ptr;
  uint8                         payload_type;
  uint16                        marker;
  uint16                        reliability;
} bt_ev_pf_avdtp_recv_data_type;

typedef BT_PACKED struct {
  uint16                        handle;
  bt_cmd_status_type            result;
} bt_ev_pf_avdtp_send_cfm_type;

#endif /* FEATURE_BT_EXTPF_AV */

/*-------------------------------------------------------------------------*/
/*              Phone Book Access Profile (PBAP) Event Messages            */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_EXTPF_PBAP

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type    conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_pbap_cli_con_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type    conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_pbap_cli_dcn_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type    conn_id;
  boolean                        user_id_required;
} bt_pf_ev_pbap_cli_auth_req_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type    conn_id;
  uint8*                         data_ptr;
  uint16                         data_len;
  uint16*                        phone_book_size_ptr;
  uint8*                         new_missed_calls_ptr;
  boolean                        final;
  bt_cmd_status_type             status;
} bt_pf_ev_pbap_cli_pull_phone_book_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type    conn_id;
  bt_cmd_status_type             status;
} bt_pf_ev_pbap_cli_set_phone_book_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type    conn_id;
  uint8*                         data_ptr;
  uint16                         data_len;
  uint16*                        phone_book_size_ptr;
  uint8*                         new_missed_calls_ptr;
  boolean                        final;
  bt_cmd_status_type             status;
} bt_pf_ev_pbap_cli_pull_vcard_listing_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type    conn_id;
  uint8*                         data_ptr;
  uint16                         data_len;
  boolean                        final;
  bt_cmd_status_type             status;
} bt_pf_ev_pbap_cli_pull_vcard_entry_cfm_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type    conn_id;
  bt_bd_addr_type                bd_addr;
  boolean                        unauthorized;
  byte                           user_id[BT_PF_OBEX_MAX_USERID_LEN];
  uint8                          user_id_len;
} bt_pf_ev_pbap_srv_con_ind_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_srv_conn_id_type    conn_id;
} bt_pf_ev_pbap_srv_dcn_ind_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_srv_conn_id_type    conn_id;
} bt_pf_ev_pbap_srv_auth_ind_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_srv_conn_id_type    conn_id;
  uint16                         obj_name[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          obj_name_len;
  uint64                         attribute_mask;
  uint8                          format;
  uint16                         max_list_count;
  uint16                         list_start_offset;
  bt_pf_pbap_srv_req_state_type  state;
  bt_cmd_status_type             status;
} bt_pf_ev_pbap_srv_pull_phone_book_ind_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_srv_conn_id_type    conn_id;
  uint8                          level;
  uint16                         folder_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          folder_len;
} bt_pf_ev_pbap_srv_set_phone_book_ind_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_srv_conn_id_type    conn_id;
  uint16                         obj_name[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          obj_name_len;
  uint8                          order;
  char                           value[BT_PF_MAX_MIME_TYPE_LEN];
  uint8                          value_len;
  uint8                          search_attrib;
  uint16                         max_list_count;
  uint16                         list_start_offset;
  bt_pf_pbap_srv_req_state_type  state;
  bt_cmd_status_type             status;
} bt_pf_ev_pbap_srv_pull_vcard_listing_ind_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_srv_conn_id_type    conn_id;
  uint16                         obj_name[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                          obj_name_len;
  uint64                         attribute_mask;
  uint8                          format;
  bt_pf_pbap_srv_req_state_type  state;
  bt_cmd_status_type             status;
} bt_pf_ev_pbap_srv_pull_vcard_entry_ind_type;

#endif /* FEATURE_BT_EXTPF_PBAP */

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#endif /* FEATURE_BT_EXTPF */
#endif /* FEATURE_BT */
#endif /* _BTPF_H */


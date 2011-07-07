/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            B L U E T O O T H    E X T E R N A L    P R O F I L E    

              Q U E U E D    C O M M A N D    I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the routines to queue up BT external profile commands.

              Copyright (c) 2004-2009 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfcmd.c#2 $ 
$DateTime: 2009/02/11 11:44:18 $

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2009-02-11   gs  Updated bt_pf_process_unicode_str() to truncate file name
                  without removing file extension if name exceeds max length.
 2009-11-17   pn  Defined BT_MSG_LAYER to fix compile error.
 2009-11-07   pn  BIP API changes: missing param validation checks added; 
                  one return per function; one message logged per function.
 2008-06-16   pr  Added bt_cmd_pf_bpp_sndr_force_disconnect to tear down 
                  connection from printer to BPP server (object channel).
 2008-05-06   gs  Fixed compile error in bt_cmd_pf_pbap_cli_auth_response()
                  if FTP profile feature is undef'ed.

  when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2008-05-28   hs   Added the support for AVRCP 1.3 meta data feature

*   #69         03 Mar 2008            GS
* Added support for HID profile Get/SetReport functions
*
*   #68         25 Feb 2008            JN
* Fixed Banned API warning. Replaced BT_STRCPY with std_strlcpy.
*
*   #67         19 Dec 2007            PN
* Fixed NULL pointer access problem in bt_cmd_pf_a2dp_start().
*
*   #66         20 Nov  2007           PC
* Added meta-comments for A2DP and AVRCP profiles. Fixed featurization of 
* SAP profile. The implementation of the profile is now feature governed.
* 
*    #65        07 Nov 2007            GS
* Added HID profile host role support.
*
*    #64        05 Jun 2007           DSN
* Fixing featurization if FEATURE_BT_EXTPF_SAP is not defined.
*
*    #63        04 Jun 2007           DSN
* Fixing featurization in previous revision.
*
*    #62        31 May 2007           DSN
* Featurization to allow remoting of apis for SAP.
*
*    #61        30 May 2007           DSN
* Avoid using the saved mmsgdi buffer pointer for apdu rsp since BT is on
* ARM11 and mmgsdi is in ARM9 on dual-processor targets and buffer pointers
* are not retained when passed via RPC.
*
*    #60        29 May 2007           DSN
* Fix for RVCT21 compile errors when SAP is enabled.
*
*    #59        21 May 2007           SSK
* Fix compiler warnings.
*
*    #58        27 Mar 2007           JTL
* Free command queue items if they're not used.
*
*    #57        19 Mar 2007           MH
* Fixed BIP Client put image ARM11 memory allignment issue  
*
*    #56        28 Feb 2007           MH
* Added disconnect response function.

*    #55        16 Feb 2007           PN
* Made use of safe string functions.
*
*    #54        08 Nov 2006           DM
* Fixing Compile error/warnings from earlier checkin.
*
*    #53        03 Nov 2006           DM
* Added new parameter handle (l2cap cid) to a2dp send data, required for 
* flow control in A2DP
*
*    #52        26 Oct 2006           MH
* Modified srv confirm set path to support all headers
*
*    #51        23 Oct 2006           GS
* Fixed issue in sending empty Name header in PBAP PullvCardListing API.
*
*    #49-50     14 Aug 2006           DSN
* Typecasting for RVCT compiler.
*
*    #48        28 Jul 2006           DM
* Interface changes for av-sync related to send_data
*
*    #47        14 Jul 2006           GS
* Updated bt_cmd_pf_opp_srv_register() to use revised definition of
* OPP Supported Formats.
*
*    #46        10 Jul 2006           GS
* NULL terminate Type string in bt_cmd_pf_bpp_sndr_send_file().
*
*    #45        06 Jul 2006           MP
* Fixed potential buffer overflow problem.
*
*    #44        27 Jun 2006           DSN
* Featurization for SoC solution.
*
*    #43        23 Jun 2006           JH
* Modified GOEP API to support JSR82.
*
*    #42        12 Jun 2006           DSN
* Typecasting fixes for RVCT compiler.
*
*    #41        07 Jun 2006           RP
* Feature to turn on A2DP and AVRCP without turning on other profiles.
*
*    #40        30 May 2006           GS
* Added support for PBAP feature.
* 
*    #39        16 May 2006           JTL
* Added FEATURE_BT_TIME_DEBUG to determine delays for A2DP command 
* processing. Not defined by default.
*
*    #38        13 Apr 2006           JH
* Modified to allow empty type header in OPP client push command.
*
*    #37        10 Apr 2006           GS
* Updated bt_cmd_pf_bip_cli_remote_display() to allow empty image-handle string.
*
*    #36        15 Dec 2005           JTL
* Adding new command to immediately send (flush) buffers stored in A2DP.
*
*    #35        15 Nov 2005           JTL
* Adding new command for A2DP for registering AVS app ID.
*
*    #34        08 Aug 2005           JTL
* Adding status to AVRCP replies.
*
*    #33        18 Jul 2005           GS
* Added support for BIP server force disconnect.
*
*    #32        13 Jul 2005           JH
* Fixed typo.
*
*    #31        13 Jul 2005           JH
* Fixed misused connection ID type.
*
*    #30        06 Jul 2005           JTL
* Fixing various pointer checking errors.
*
*    #29        02 Jun 2005           JTL
* Change data type to use one defined by AVS.
*
*    #28        31 May 2005           RY
* Merged in Sirius changes.
*
*    #26-27     27 May 2005           JTL
* Adding command for fine grained control of A2DP.
*
*    #25        19 May 2005           JH
* Set name_len in bt_cmd_pf_opp_srv_open_read_done() which was accidentally left
*  out.
*
*    #24        12 Apr 2005           JH
* Converted FTP file operation done and FTP client abort functions into queued
*  commands.
*
*    #20-23     31 Mar 2005           JTL
* Added support for AVDTP and A2DP profiles.
*
*    #19        22 Feb 2005            PN
* Removed from bt_cmd_pf_sap_tx_apdu_rsp() and bt_cmd_pf_sap_tx_atr_rsp()
* checks for NULL rsp_ptr as this should not be considered invalid parameter.
*
*    #18        22 Jan 2005            PN
* Added support for SIM Access profile.
*
*    #17        7 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
*
*    #16       30 Dec 2004            JH
* Modified bt_pf_process_unicode_str() to avoid misaligned 16-bit write.
*
*    #15        6 Dec 2004            JTL
* Fixed bt_cmd_pf_ctp_register_recall() so it actually does something.
*
*    #14       19 Oct 2004            JH
* Added number of returned handles parameter to
*  bt_cmd_pf_bip_srv_get_images_list_response() command.
*
*    #13       30 Aug 2004            JH
* Change input parameter from service_uuid_ptr to srv_name_ptr in SyncML
*  registration commands.
*
*    #12        9 Aug 2004            JH
* Renamed bt_cmd_pf_sml_auth_srv_response and bt_cmd_pf_sml_auth_cli_response to
*  bt_cmd_pf_sml_auth_request and bt_cmd_pf_sml_auth_response respectively
*
*    #8        24 May 2004            JH
* Added BIP server start print response to facilitate APIT testing.
*
*    #7        14 May 2004            JH
* Further command and event struct changes
*
*    #6        30 Apr 2004            JH
* Added SML support
*
*    #5        23 Apr 2004            JH
* Server register and deregister commands are now queued.
* Replaced all bt_pf_cmd* with bt_cmd_pf*; replaced all BT_PF_EV* with
*  BT_EV_PF*
* Replaced bt_pf_cmd_status_type with bt_cmd_status_type
*
*    #4        14 Apr 2004            JH
* Added BIP and BPP support
* Replaced all bt_pf_unicode_str_type command parameters with uint16* type
*
*    #2        25 Feb 2004            JH
* Renamed btpfgoep.h btpfgoepi.h
*

===========================================================================*/

#define BT_PF_FILE_EXT_CHAR '.'
#define BT_PF_FILE_MAX_EXT_LEN 7 /* including '.' */ 

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF
#ifdef FEATURE_MT_OBEX
#include "mt_obex_opp.h"
#endif /* FEATURE_MT_OBEX */
#include <string.h>
#include "memheap.h"
#include "task.h"
#include "rex.h"
#include "dsm.h"
#include "bt.h"
#include "btpfi.h"
#include "btpfcmdi.h"
#include "btpfgoepi.h"
#include "bti.h"
#include "btutils.h"
#include "btmsg.h"
#ifndef FEATURE_BT_SOC
#include "btbb.h"
#endif /* FEATURE_BT_SOC */
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif /* FEATURE_BT_EXTPF_SAP */
#ifdef FEATURE_BT_EXTPF_AV
#include "audiosbcenc.h"
#include "oi_status.h"
#endif /* FEATURE_BT_EXTPF_AV */
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#ifdef FEATURE_BT_TIME_DEBUG
#include "time_svc.h"
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif /* FEATURE_BT_EXTPF_SAP */

/*===========================================================================

FUNCTION
  bt_pf_copy_unicode_str

DESCRIPTION
  This function copies a NULL terminated Unicode string from source to
  destination. It also ensures that the length of the string is within
  the specified limit. The actual length of the string is also optionally
  returned.

===========================================================================*/
bt_cmd_status_type bt_pf_copy_unicode_str(
  BT_PACKED uint16* to_ptr,
  BT_PACKED uint16* from_ptr,
  uint8             max_len,
  uint8*            actual_len )
{
  bt_cmd_status_type status = OI_STATUS_INVALID_PARAMETERS;
  uint8  i;

  if ( (from_ptr == NULL) || (to_ptr == NULL) )
  {
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  for (i = 0; i <= max_len; i++)
  {
    *to_ptr = *from_ptr;
    if (*from_ptr == '\0')
    {
      status = OI_OK;
      break;
    }
    to_ptr++;
    from_ptr++;
  }
  if (actual_len != NULL)
  {
    *actual_len = i + 1; /* includes NULL termination */
  }
  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_copy_ascii_str

DESCRIPTION
  This function copies a NULL terminated ASCII string from source to
  destination. It also ensures that the length of the string is within
  the specified limit.

===========================================================================*/
bt_cmd_status_type bt_pf_copy_ascii_str(
  char* to_ptr,
  char* from_ptr,
  uint8 max_len )
{
  bt_cmd_status_type status = OI_STATUS_INVALID_PARAMETERS;
  uint8 i;

  if ( (from_ptr == NULL) || (to_ptr == NULL) )
  {
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  for (i = 0; i <= max_len; i++)
  {
    *to_ptr = *from_ptr;
    if (*from_ptr == '\0')
    {
      status = OI_OK;
      break;
    }
    to_ptr++;
    from_ptr++;
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_wstrrchr

DESCRIPTION
  Returns ptr to last occurrence of a character within a string.  
  NULL, otherwise.

===========================================================================*/
LOCAL uint16* bt_pf_wstrrchr(const uint16 *str, uint16 len, uint16 c)
{
  uint16* str_ptr = (uint16*)str + len - 1;

  while ((*str_ptr != c) && (str_ptr != str)) 
  {
    str_ptr--;
  }

  return (*str_ptr == c) ? str_ptr : 0;
}

/*===========================================================================

FUNCTION
  bt_pf_process_unicode_str

DESCRIPTION
  This function copies the Unicode string from the unicode string structure
  to a uint16 array. Truncation will be performed if length exceeds limit but 
  without removing the file name extension.
  The uint16 array must be at least max_len + 1 in size to accomodate the NULL
  termination.  

  Function returns length of destination string excluding NULL char.

===========================================================================*/
uint8 bt_pf_process_unicode_str(
  BT_PACKED uint16*       to_ptr,
  bt_pf_unicode_str_type* from_ptr,
  uint8                   max_len)
{
  uint8 len;
  uint16* ext_ptr = NULL;
  uint8 ext_len = 0;

  len = (uint8)(from_ptr->len - 1);

  /* truncate if needed */
  if (from_ptr->len > max_len + 1) /* from_ptr->len is assumed to include NULL */
  {
    len = max_len; /* len excludes NULL */

    if ( ( ext_ptr = bt_pf_wstrrchr( from_ptr->str, from_ptr->len, BT_PF_FILE_EXT_CHAR )) != NULL )
    {
      ext_len = (from_ptr->str + from_ptr->len - 1) - ext_ptr;

      /* validate extension length */
      if ( (ext_len > BT_PF_FILE_MAX_EXT_LEN) || (max_len < BT_PF_FILE_MAX_EXT_LEN) ) 
      {
        BT_MSG_HIGH( "BT PF: unexp len in process_unicode_str, max_len=%d, ext_len=%d, max_ext_len=%d", 
                     max_len, ext_len, BT_PF_FILE_MAX_EXT_LEN);
      }
      else
      {
        /* truncate name without removing file extension */
        memcpy( (void*)to_ptr, (void*)from_ptr->str, (len - ext_len) * sizeof( uint16 ) );
        memcpy( (void*)(to_ptr + (len - ext_len)), (void*)ext_ptr, ext_len * sizeof (uint16) ); 

        *(to_ptr + len) = 0; /* NULL termination */
        return (len);
      }
    }
  }

  memcpy( (void*)to_ptr, (void*)from_ptr->str, len * sizeof( uint16 ) );
  *(to_ptr + len) = 0; /* NULL termination */

  return(len);
}

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#ifdef FEATURE_BT_EXTPF_GOEP
/*-------------------------------------------------------------------------*/
/*         Generic Object Exchange Profile (GOEP) Commands                 */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_cli_auth_response

DESCRIPTION
  This command responds to an authentication indication. A NULL password
  indicates that the application is rejecting the authentication challenge.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_cli_auth_response(
  bt_app_id_type                app_id,
  bt_pf_goep_cli_conn_id_type   connect_id,
  byte*                         user_id_ptr,
  uint8                         user_id_len,
  char*                         password_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (user_id_len > OI_OBEX_MAX_USERID_LEN) ||
       ( (user_id_len > 0) && (user_id_ptr == NULL) ) )
  {
    BT_ERR("BT PF CMD GOEP CLI AUTH: invalid UID, App: %x",
           app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD GOEP CLI AUTH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (user_id_len > 0)
  {
    memcpy( (void*)cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.user_id,
            (void*)user_id_ptr,
            user_id_len );
  }

  if (password_ptr == NULL) /* It's ok to have no password. */
  {
    cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.password_str[0] = '\0';
  }
  else
  {
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.password_str),
                    password_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN ) )
    {
      BT_ERR( "BT PF CMD FTP CLI AUTH: invalid pwd, App: %x, Stat: %x",
              app_id, OI_OBEX_PASSWORD_TOO_LONG, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_OBEX_PASSWORD_TOO_LONG);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_CLI_AUTH_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.conn_id = connect_id;
  cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.user_id_len = user_id_len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_cli_connect

DESCRIPTION
  This command establishes a connection to an OBEX server. This includes
  setting up an RFCOMM connection to the OBEX server.

  Headers must not be freed until connect confirmation is received.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_cli_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel,
  bt_pf_goep_obex_hdr_list_type        headers_list,
  bt_sd_uuid_type                      uuid,
  bt_security_type                     security )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD GOEP CLI CON: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( bt_pf_goep_cli_app_conn_assign( app_id )
         == BT_PF_GOEP_CLI_APP_CONN_FULL )
  {
    BT_ERR("BT PF CMD GOEP CLI CON: table full, App: %x", app_id, 0, 0);
    return(OI_STATUS_NO_RESOURCES);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_CLI_CONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_cli_connect.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_goep_cli_connect.channel = channel;
    cmd_ptr->cmd_msg.cmd_goep_cli_connect.headers_ptr = headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_cli_connect.headers_cnt = headers_list.count;
    cmd_ptr->cmd_msg.cmd_goep_cli_connect.uuid = uuid;
    cmd_ptr->cmd_msg.cmd_goep_cli_connect.security = security;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP CLI CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_cli_disconnect

DESCRIPTION
  This command disconnects a client from an OBEX server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_cli_disconnect(
  bt_app_id_type                  app_id,
  bt_pf_goep_cli_conn_id_type     conn_id,
  bt_pf_goep_obex_hdr_list_type   headers_list )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_CLI_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_cli_disconnect.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_cli_disconnect.headers_ptr = 
      headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_cli_disconnect.headers_cnt = headers_list.count;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP CLI DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_cli_put

DESCRIPTION
  Puts data to an OBEX server. The put_cfm_cb() callback indicates that the
  server has received and acknowledged receipt of the data.

  Headers must not be freed until acknowledged.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_cli_put(
  bt_app_id_type                       app_id,
  bt_pf_goep_cli_conn_id_type          conn_id,  
  bt_pf_goep_obex_hdr_list_type        headers_list,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_CLI_PUT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_cli_put.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_cli_put.headers_ptr = headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_cli_put.headers_cnt = headers_list.count;
    cmd_ptr->cmd_msg.cmd_goep_cli_put.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP CLI PUT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_cli_get

DESCRIPTION
  Get data from an OBEX server. This will cause one or more packets to be
  retrieved from an OBEX server.

  Headers must not be freed until acknowledged.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_cli_get(
  bt_app_id_type                       app_id,
  bt_pf_goep_cli_conn_id_type          conn_id,  
  bt_pf_goep_obex_hdr_list_type        headers_list,
  boolean                              final )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_CLI_GET;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_cli_get.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_cli_get.headers_ptr = headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_cli_get.headers_cnt = headers_list.count;
    cmd_ptr->cmd_msg.cmd_goep_cli_get.final = final;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP CLI GET: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_cli_set_path

DESCRIPTION
  Setpath can be used to set the current folder for getting and putting
  objects and can be used to create new folders.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_cli_set_path(
  bt_app_id_type                 app_id,
  bt_pf_goep_cli_conn_id_type    conn_id,
  uint16*                        folder_ptr,
  boolean                        up_level,
  boolean                        dont_create,
  bt_pf_goep_obex_hdr_list_type  headers_list )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint8               len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    if (folder_ptr == NULL)
    {
      len = 0;
    }
    else
    {
      if ( OI_OK != bt_pf_copy_unicode_str(
                      (uint16*)(cmd_ptr->cmd_msg.cmd_goep_cli_set_path.folder_str),
                      folder_ptr, BT_PF_MAX_FILENAME_LEN,
                      &len ) )
      {
        BT_ERR( "BT PF CMD GOEP CLI SET PATH: invalid fldr, App: %x, Stat: %x",
                app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
        bt_pf_free_cmd( cmd_ptr );
        return(OI_STATUS_INVALID_PARAMETERS);
      }
    }

    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_CLI_SET_PATH;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_cli_set_path.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_cli_set_path.up_level = up_level;
    cmd_ptr->cmd_msg.cmd_goep_cli_set_path.dont_create = dont_create;
    cmd_ptr->cmd_msg.cmd_goep_cli_set_path.folder_len = len;
    cmd_ptr->cmd_msg.cmd_goep_cli_set_path.headers_ptr = headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_cli_set_path.headers_cnt = headers_list.count;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP CLI SET PATH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}



/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_accept_connect

DESCRIPTION
  Command called by upper layer to accept or reject a connect request.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_accept_connect(
  bt_app_id_type                  app_id,
  bt_pf_goep_srv_conn_id_type     conn_id,
  boolean                         accept,
  bt_pf_goep_obex_hdr_list_type   headers_list,
  bt_cmd_status_type              status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_SRV_ACCEPT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_accept.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_accept.accept = accept;
    cmd_ptr->cmd_msg.cmd_goep_srv_accept.status = status;
    cmd_ptr->cmd_msg.cmd_goep_srv_accept.headers_ptr = headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_srv_accept.headers_cnt = headers_list.count;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP SRV ACCEPT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_confirm_set_path

DESCRIPTION
  Command called to confirm that the path has been set in response to a
  setpath indication.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_confirm_set_path(
  bt_app_id_type                    app_id,
  bt_pf_goep_srv_conn_id_type       conn_id,
  bt_pf_goep_obex_hdr_list_type     headers_list,
  bt_cmd_status_type                status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_SRV_CFM_SET_PATH;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_cfm_set_path.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_cfm_set_path.status = status;
    cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.headers_ptr = headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.headers_cnt = headers_list.count;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP SRV SET PATH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_auth_response

DESCRIPTION
  Response to an connect indication which reported that the connection was
  unauthorized. If the password is correct and authentication completes
  another connect indication will be issued.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_auth_response(
  bt_app_id_type                app_id,
  bt_pf_goep_srv_conn_id_type   conn_id,
  const char*                   password_ptr,
  boolean                       read_only )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ((password_ptr != NULL) && 
      (strlen( password_ptr ) > OI_OBEX_MAX_PASSWORD_LEN))
  {
    return(OI_OBEX_PASSWORD_TOO_LONG);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    if (password_ptr != NULL)
    {
      std_strlcpy( (char *)cmd_ptr->cmd_msg.cmd_goep_srv_auth_rsp.password, 
                   password_ptr, 
                   sizeof(cmd_ptr->cmd_msg.cmd_goep_srv_auth_rsp.password) );
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_goep_srv_auth_rsp.password[0] = '\0';
    }
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_SRV_AUTH_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_auth_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_auth_rsp.read_only = read_only;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP SRV AUTH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_get_response

DESCRIPTION
  Respond to a GET command by sending data to a connected OBEX client.

  Headers must not be freed until response confirmation callback is invoked.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_get_response(
  bt_app_id_type                 app_id,
  bt_pf_goep_srv_conn_id_type    conn_id,
  bt_pf_goep_obex_hdr_list_type  headers_list,
  bt_cmd_status_type             status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_SRV_GET_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_get_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_get_rsp.headers_ptr = headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_srv_get_rsp.headers_cnt = headers_list.count;
    cmd_ptr->cmd_msg.cmd_goep_srv_get_rsp.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP SRV GET: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_put_response

DESCRIPTION
  Acknowledge a PUT command, optionally sending response headers to a
  connected OBEX client.

  Headers must not be freed until response confirmation callback is invoked.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_put_response(
  bt_app_id_type                 app_id,
  bt_pf_goep_srv_conn_id_type    conn_id,
  bt_pf_goep_obex_hdr_list_type  headers_list,
  bt_cmd_status_type             status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_SRV_PUT_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.headers_ptr = headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.headers_cnt = headers_list.count;
    cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP SRV PUT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_disconnect_response

DESCRIPTION
  Acknowledge a Disconnect command, optionally sending response headers to a
  connected OBEX client.

  Headers must not be freed until response confirmation callback is invoked.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_disconnect_response(
  bt_app_id_type                 app_id,
  bt_pf_goep_srv_conn_id_type    conn_id,
  bt_pf_goep_obex_hdr_list_type  headers_list,
  bt_cmd_status_type             status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_SRV_DCN_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_dcn_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_dcn_rsp.headers_ptr = headers_list.list_ptr;
    cmd_ptr->cmd_msg.cmd_goep_srv_dcn_rsp.headers_cnt = headers_list.count;
    cmd_ptr->cmd_msg.cmd_goep_srv_dcn_rsp.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP SRV DISCONNECT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_force_disconnect

DESCRIPTION
  This command forcibly severs the connection from an OBEX client to the
  OBEX server. The disconnect indication callback will be called when the
  disconnect is complete.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_force_disconnect(
  bt_app_id_type                app_id,
  bt_pf_goep_srv_conn_id_type   conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_GOEP_SRV_FORCE_DCN;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_goep_srv_force_dcn.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD GOEP SRV DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}


#endif /* FEATURE_BT_EXTPF_GOEP */

#ifdef FEATURE_BT_EXTPF_OPP

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_cli_connect

DESCRIPTION
  Connect to remote OBEX object push profile server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_cli_connect(
  bt_app_id_type                app_id,
  bt_bd_addr_type*              bd_addr_ptr,
  uint8                         channel
)
{
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type     cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_bd_addr_type        null_addr = BT_PF_BD_ADDR_NULL;

  if ( bd_addr_ptr == NULL )
  {
    bd_addr_ptr = &null_addr;
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

    {
      BT_ERR("BT PF CMD OPP CLI CON: unexpected NULL ptrs, App: %x",
             app_id, 0, 0);
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_CLI_CONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_connect.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_opp_cli_connect.channel = channel;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD OPP CLI CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_cli_disconnect

DESCRIPTION
  Terminate the current OBEX connection to a remote object push profile server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_cli_disconnect(
  bt_app_id_type             app_id,
  bt_pf_opp_cli_conn_id_type conn_id )
{
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_CLI_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_disconnect.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD OPP CLI DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_cli_push

DESCRIPTION
  Push an object to an OBEX server. The object type must be one of the object
  types supported by the server. The list of supported object types is
  obtained from the service record for the server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_cli_push(
  bt_app_id_type             app_id,
  bt_pf_opp_cli_conn_id_type conn_id,
  uint16*                    name_ptr,
  char*                      type_ptr )
{
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type     cmd_status;
  uint8                  len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD OPP CLI PSH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (name_ptr == NULL)
  {
    len = 0;
  }
  else
  {
    if( OI_OK != bt_pf_copy_unicode_str(
                   (uint16*)cmd_ptr->cmd_msg.cmd_opp_cli_push.name_str,
                   name_ptr, BT_PF_MAX_FILENAME_LEN,
                   &len ) )
    {
      BT_ERR( "BT PF CMD OPP CLI PSH: invalid name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  if ( type_ptr != NULL )
  {
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(cmd_ptr->cmd_msg.cmd_opp_cli_push.type_str),
                    type_ptr, BT_PF_MAX_MIME_TYPE_LEN ) )
    {
      BT_ERR( "BT PF CMD OPP CLI PSH: invalid type, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }
  else
  {
    cmd_ptr->cmd_msg.cmd_opp_cli_push.type_str[0] = 0;
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_CLI_PUSH;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_opp_cli_push.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_opp_cli_push.name_len = len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_cli_pull

DESCRIPTION
  Pull the default object from an OBEX server. The default object is the
  owner's business card which is an OBEX object of type OI_OBEX_VCARD_TYPE.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_cli_pull(
  bt_app_id_type             app_id,
  bt_pf_opp_cli_conn_id_type conn_id )
{
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_CLI_PULL;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_pull.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD OPP CLI PUL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_cli_abort

DESCRIPTION
  Terminates the current push or pull operation.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_cli_abort(
  bt_app_id_type               app_id,
  bt_pf_opp_srv_conn_id_type   conn_id )
{
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_CLI_ABORT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_abort.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD OPP CLI ABORT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_cli_open_read_done

DESCRIPTION
  This command is called by the application when the object open read
  operation is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_cli_open_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  uint16*                       name_ptr,
  char*                         type_ptr,
  uint32                        size,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint8               name_len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    if( OI_OK != bt_pf_copy_unicode_str(
                   cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.name_str,
                   name_ptr, BT_PF_MAX_FILENAME_LEN,
                   &name_len ) )
    {
      BT_ERR( "BT PF CMD OPP CLI OPEN READ DNE: invalid name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }

    if ( type_ptr != NULL )
    {
      if ( OI_OK != bt_pf_copy_ascii_str(
                    (char *)cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.type_str,
                      type_ptr, BT_PF_MAX_MIME_TYPE_LEN ) )
      {
        BT_ERR( "BT PF CMD OPP CLI OPEN READ DNE: invalid type, App: %x, Stat: %x",
                app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
        bt_pf_free_cmd( cmd_ptr );
        return(OI_STATUS_INVALID_PARAMETERS);
      }
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.type_str[0] = 0;
    }

    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_CLI_OPEN_READ_DONE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.handle = handle;
    cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.name_len = name_len;
    cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.size = size;
    cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF OPP CLI OPEN READ DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_cli_open_write_done

DESCRIPTION
  This command is called by the application when the object open write
  operation is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_cli_open_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_CLI_OPEN_WRITE_DONE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_open_write_done.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_open_write_done.handle = handle;
    cmd_ptr->cmd_msg.cmd_opp_cli_open_write_done.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF OPP CLI OPEN WRITE DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_cli_read_done

DESCRIPTION
  This command is called by the application when the object read operation
  is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_cli_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        data_len,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_CLI_READ_DONE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_read_done.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_read_done.handle = handle;
    cmd_ptr->cmd_msg.cmd_opp_cli_read_done.status = status;
    cmd_ptr->cmd_msg.cmd_opp_cli_read_done.data_ptr = data_ptr;
    cmd_ptr->cmd_msg.cmd_opp_cli_read_done.data_len = data_len;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF OPP CLI READ DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_cli_write_done

DESCRIPTION
  This command is called by the application when the object write operation
  is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_cli_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status ){
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_CLI_WRITE_DONE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_write_done.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_opp_cli_write_done.handle = handle;
    cmd_ptr->cmd_msg.cmd_opp_cli_write_done.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF OPP CLI WRITE DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_srv_register

DESCRIPTION
  Initialize the object push server and register it in the service discovery
  database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_srv_register(
  bt_app_id_type             app_id,
  uint32                     supported_formats,
  char*                      srv_name_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if(!(supported_formats & BT_PF_OPP_SRV_OBJ_FORMAT_MASK) )
  {
    BT_ERR( "BT PF CMD OPP SRV REG: invalid format, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD OPP SRV REG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_ascii_str(
                  (char*)(cmd_ptr->cmd_msg.cmd_opp_srv_register.srv_name_str),
                  srv_name_ptr, BT_PF_MAX_FILENAME_LEN ) )
  {
    BT_ERR( "BT PF CMD OPP SRV REG: invalid srv name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_SRV_REGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_opp_srv_register.supported_formats = supported_formats;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_srv_deregister

DESCRIPTION
  Terminates the Object Push server and deregisters it from the SDP database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_srv_deregister(
  bt_app_id_type             app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD OPP SRV DEREG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_SRV_DEREGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_srv_accept_connect

DESCRIPTION
  The OPP application must call this function to accept or reject a connect
  request from a client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_srv_accept_connect(
  bt_app_id_type             app_id,
  bt_pf_opp_srv_conn_id_type conn_id,
  boolean                    allow_push,
  boolean                    allow_pull )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_SRV_ACCEPT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_accept.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_accept.allow_push = allow_push;
    cmd_ptr->cmd_msg.cmd_opp_srv_accept.allow_pull = allow_pull;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD OPP SRV ACCEPT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_srv_force_disconnect

DESCRIPTION
  Forcibly severs the connection from an OPP client to the OPP server.
  This function should be called for all active connections before
  terminating the OPP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_srv_force_disconnect(
  bt_app_id_type             app_id,
  bt_pf_opp_srv_conn_id_type conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_SRV_FORCE_DCN;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_force_dcn.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD OPP SRV DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_srv_open_read_done

DESCRIPTION
  This command is called by the application when the object open read
  operation is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_srv_open_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  uint16*                       name_ptr,
  char*                         type_ptr,
  uint32                        size,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint8               name_len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    if( OI_OK != bt_pf_copy_unicode_str(
                   cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.name_str,
                   name_ptr, BT_PF_MAX_FILENAME_LEN,
                   &name_len ) )
    {
      BT_ERR( "BT PF OPP SRV OPEN READ DNE: invalid name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }

    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char *)cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.type_str,
                    type_ptr, BT_PF_MAX_MIME_TYPE_LEN ) )
    {
      BT_ERR( "BT PF OPP SRV OPEN READ DNE: invalid type, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }

    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_SRV_OPEN_READ_DONE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.handle = handle;
    cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.size = size;
    cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.name_len = name_len;
    cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF OPP SRV OPEN READ DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_srv_open_write_done

DESCRIPTION
  This command is called by the application when the object open write
  operation is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_srv_open_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_SRV_OPEN_WRITE_DONE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_open_write_done.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_open_write_done.handle = handle;
    cmd_ptr->cmd_msg.cmd_opp_srv_open_write_done.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF OPP SRV OPEN WRITE DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_srv_read_done

DESCRIPTION
  This command is called by the application when the object read operation
  is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_srv_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        data_len,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_SRV_READ_DONE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_read_done.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_read_done.handle = handle;
    cmd_ptr->cmd_msg.cmd_opp_srv_read_done.status = status;
    cmd_ptr->cmd_msg.cmd_opp_srv_read_done.data_ptr = data_ptr;
    cmd_ptr->cmd_msg.cmd_opp_srv_read_done.data_len = data_len;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF OPP SRV READ DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_opp_srv_write_done

DESCRIPTION
  This command is called by the application when the object write operation
  is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_opp_srv_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_OPP_SRV_WRITE_DONE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_write_done.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_opp_srv_write_done.handle = handle;
    cmd_ptr->cmd_msg.cmd_opp_srv_write_done.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF OPP SRV WRITE DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

#endif /* FEATURE_BT_EXTPF_OPP */

#ifdef FEATURE_BT_EXTPF_FTP

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_connect

DESCRIPTION
  This function establishes an OBEX connection over RFCOMM to a remote FTP
  server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( bd_addr_ptr == NULL )
  {
    BT_ERR("BT PF CMD FTP CLI CON: unexpected NULL ptrs, App: %x",
           app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_CONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ftp_cli_connect.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_ftp_cli_connect.channel = channel;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD FTP CLI CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_disconnect

DESCRIPTION
  This function terminate the current OBEX connection to a remote FTP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_disconnect(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ftp_cli_disconnect.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD FTP CLI DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_auth_response

DESCRIPTION
  This function is called by the application in response to a password
  indication to provide a password for OBEX authentication.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_auth_response(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  byte*                         user_id_ptr,
  uint8                         user_id_len,
  char*                         password_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (user_id_len > OI_OBEX_MAX_USERID_LEN) ||
       ( (user_id_len > 0) && (user_id_ptr == NULL) ) )
  {
    BT_ERR("BT PF CMD FTP CLI AUTH: invalid UID, App: %x",
           app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI AUTH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (user_id_len > 0)
  {
    memcpy( (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_auth_rsp.user_id,
            (void*)user_id_ptr,
            user_id_len );
  }

  if (password_ptr == NULL) /* It's ok to have no password. */
  {
    cmd_ptr->cmd_msg.cmd_ftp_cli_auth_rsp.password_str[0] = '\0';
  }
  else
  {
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(cmd_ptr->cmd_msg.cmd_ftp_cli_auth_rsp.password_str),
                    password_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN ) )
    {
      BT_ERR( "BT PF CMD FTP CLI AUTH: invalid pwd, App: %x, Stat: %x",
              app_id, OI_OBEX_PASSWORD_TOO_LONG, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_OBEX_PASSWORD_TOO_LONG);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_AUTH_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_auth_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_auth_rsp.user_id_len = user_id_len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return(cmd_status);
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_put

DESCRIPTION
  This function copies a file from the local client to a remote server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_put(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       local_name_ptr,
  uint16*                       remote_name_ptr )
{
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type     cmd_status;
  uint8                  local_len, remote_len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI PUT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_unicode_str(
                  (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_put.local_name_str),
                  local_name_ptr, BT_PF_MAX_FILENAME_LEN,
                  &local_len ) )
  {
    BT_ERR( "BT PF CMD FTP CLI PUT: invalid lcl name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if (remote_name_ptr == NULL) /* It's ok to have no remote filename */
  {
    remote_len = 0;
  }
  else
  {
    if ( OI_OK != bt_pf_copy_unicode_str(
                    (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_put.remote_name_str),
                    remote_name_ptr, BT_PF_MAX_FILENAME_LEN,
                    &remote_len ) )
    {
      BT_ERR( "BT PF CMD FTP CLI PUT: invalid rmt name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_PUT;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_put.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_put.local_name_len = local_len;
  cmd_ptr->cmd_msg.cmd_ftp_cli_put.remote_name_len = remote_len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_delete

DESCRIPTION
  This function deletes a file or folder on a remote server. Only empty
  folders can be deleted.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_delete(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       name_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;
  uint8                 len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI DEL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_unicode_str(
                  (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_delete.name_str),
                  name_ptr, BT_PF_MAX_FILENAME_LEN,
                  &len ) )
  {
    BT_ERR( "BT PF CMD FTP CLI DEL: invalid name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_DELETE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_delete.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_delete.name_len = len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_get

DESCRIPTION
  This function copies a file from a remote server to the local client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_get(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       local_name_ptr,
  uint16*                       remote_name_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;
  uint8                 local_len, remote_len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI GET: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_unicode_str(
                  (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_get.local_name_str),
                  local_name_ptr, BT_PF_MAX_FILENAME_LEN,
                  &local_len ) )
  {
    BT_ERR( "BT PF CMD FTP CLI GET: invalid lcl name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if (remote_name_ptr == NULL) /* It's ok to have no remote filename */
  {
    remote_len = 0;
  }
  else
  {
    if ( OI_OK != bt_pf_copy_unicode_str(
                    (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_get.remote_name_str),
                    remote_name_ptr, BT_PF_MAX_FILENAME_LEN,
                    &remote_len ) )
    {
      BT_ERR( "BT PF CMD FTP CLI PUT: invalid rmt name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_GET;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_get.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_get.local_name_len = local_len;
  cmd_ptr->cmd_msg.cmd_ftp_cli_get.remote_name_len = remote_len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_set_path

DESCRIPTION
  This function sets the current folder on the remote server for subsequent
  put and get operations.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_set_path(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       folder_ptr,
  bt_pf_ftp_cli_set_path_type   set_path_ctrl )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;
  uint8                 len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI SET PATH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (folder_ptr == NULL)
  {
    len = 0;
  }
  else
  {
    if ( OI_OK != bt_pf_copy_unicode_str(
                    (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.folder_str),
                    folder_ptr, BT_PF_MAX_FILENAME_LEN,
                    &len ) )
    {
      BT_ERR( "BT PF CMD FTP CLI SET PATH: invalid fldr, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_SET_PATH;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.set_path_ctrl = set_path_ctrl;
  cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.folder_len = len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_new_subfolder

DESCRIPTION
  This function creates a subfolder beneath the current folder on the remote
  server if the server supports this function.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_new_subfolder(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       folder_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;
  uint8                 len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI NEW SF: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_unicode_str(
                  (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_new_subfolder.folder_str),
                  folder_ptr, BT_PF_MAX_FILENAME_LEN,
                  &len ) )
  {
    BT_ERR( "BT PF CMD FTP CLI NEW SF: invalid fldr, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_NEW_SUBFOLDER;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_new_subfolder.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_new_subfolder.folder_len = len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_get_folder_listing

DESCRIPTION
  This function gets information about files and folders in the current
  folder or in a specific subfolder of the current folder.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_get_folder_listing(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       folder_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;
  uint8                 len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI GET FL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (folder_ptr == NULL)
  {
    len = 0;
  }
  else
  {
    if ( OI_OK != bt_pf_copy_unicode_str(
                    (uint16*)
                      (cmd_ptr->cmd_msg.cmd_ftp_cli_get_folder_listing.folder_str),
                    folder_ptr, BT_PF_MAX_FILENAME_LEN,
                    &len ) )
    {
      BT_ERR( "BT PF CMD FTP CLI GET FL: invalid fldr, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_GET_FOLDER_LISTING;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_get_folder_listing.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_get_folder_listing.folder_len = len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_tree_put

DESCRIPTION
  This function recursively copies a folder and all its subfolders and files
  from the local client to a remote server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_tree_put(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       local_name_ptr,
  uint16*                       remote_name_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;
  uint8                 local_len, remote_len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI TREE PUT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_unicode_str(
                  (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.local_name_str),
                  local_name_ptr, BT_PF_MAX_FILENAME_LEN,
                  &local_len ) )
  {
    BT_ERR( "BT PF CMD FTP CLI TREE PUT: invalid lcl name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if (remote_name_ptr == NULL) /* It's ok to have no remote filename */
  {
    remote_len = 0;
  }
  else
  {
    if ( OI_OK != bt_pf_copy_unicode_str(
                    (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.remote_name_str),
                    remote_name_ptr, BT_PF_MAX_FILENAME_LEN,
                    &remote_len ) )
    {
      BT_ERR( "BT PF CMD FTP CLI TREE PUT: invalid rmt name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_TREE_PUT;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.local_name_len = local_len;
  cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.remote_name_len = remote_len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_tree_get

DESCRIPTION
  This function recursively copies a folder and all its subfolders and files
  from a remote server to the local client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_tree_get(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  uint16*                       local_name_ptr,
  uint16*                       remote_name_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;
  uint8                 local_len, remote_len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI TREE GET: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_unicode_str(
                  (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.local_name_str),
                  local_name_ptr, BT_PF_MAX_FILENAME_LEN,
                  &local_len ) )
  {
    BT_ERR( "BT PF CMD FTP CLI TREE GET: invalid lcl name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if (remote_name_ptr == NULL) /* It's ok to have no remote filename */
  {
    remote_len = 0;
  }
  else
  {
    if ( OI_OK != bt_pf_copy_unicode_str(
                    (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.remote_name_str),
                    remote_name_ptr, BT_PF_MAX_FILENAME_LEN,
                    &remote_len ) )
    {
      BT_ERR( "BT PF CMD FTP CLI TREE GET: invalid rmt name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_TREE_GET;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.local_name_len = local_len;
  cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.remote_name_len = remote_len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_browse_done

DESCRIPTION
  This command is called by the application when the browse folder operation
  is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_browse_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI BRW DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_BROWSE_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.handle = handle;
  cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.size = size;
  cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_open_done

DESCRIPTION
  This command is called by the application when the file open operation is
  completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_open_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI OPN DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_OPEN_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.handle = handle;
  cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.size = size;
  cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_read_done

DESCRIPTION
  This command is called by the application when the file read operation is
  completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_read_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI RD DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_READ_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.handle = handle;
  cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.len = len;
  cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_set_folder_done

DESCRIPTION
  This command is called by the application when the set folder operation is
  completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_set_folder_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI SET FDR DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_SET_FOLDER_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_set_folder_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_set_folder_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_abort

DESCRIPTION
  Terminates the current put or get operation.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_abort(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP CLI ABT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_ABORT;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_cli_abort.conn_id = conn_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_register

DESCRIPTION
  Initialize the file transfer server and register it in the service discovery
  database.

  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_register(
  bt_app_id_type                      app_id,
  bt_pf_goep_srv_auth_type            auth,
  boolean                             read_only,
  boolean                             allow_browse,
  char*                               srv_name_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (auth != BT_PF_GOEP_SRV_AUTH_NONE) &&
       (auth != BT_PF_GOEP_SRV_AUTH_PASSWORD) &&
       (auth != BT_PF_GOEP_SRV_AUTH_USERID_AND_PASSWORD) )
  {
    BT_ERR( "BT PF CMD FTP SRV REG: invalid auth, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV REG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_ascii_str(
                  (char*)(cmd_ptr->cmd_msg.cmd_ftp_srv_register.srv_name_str),
                  srv_name_ptr, BT_PF_MAX_FILENAME_LEN ) )
  {
    BT_ERR( "BT PF CMD FTP SRV REG: invalid srv name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_REGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_register.auth = auth;
  cmd_ptr->cmd_msg.cmd_ftp_srv_register.read_only = read_only;
  cmd_ptr->cmd_msg.cmd_ftp_srv_register.allow_browse = allow_browse;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_deregister

DESCRIPTION
  Terminates the File Transfer server and deregisters it from the SDP database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_deregister(
  bt_app_id_type             app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV DEREG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_DEREGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_auth_response

DESCRIPTION
  This function is called by the application in response to a connnection
  indication that requires authentication.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_auth_response(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  char*                         password_ptr,
  boolean                       read_only,
  boolean                       allow_browse )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV AUTH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (password_ptr == NULL) /* It's ok to have no password. */
  {
    cmd_ptr->cmd_msg.cmd_ftp_srv_auth_rsp.password_str[0] = '\0';
  }
  else
  {
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(cmd_ptr->cmd_msg.cmd_ftp_srv_auth_rsp.password_str),
                    password_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN ) )
    {
      BT_ERR( "BT PF CMD FTP SRV AUTH: invalid pwd, App: %x, Stat: %x",
              app_id, OI_OBEX_PASSWORD_TOO_LONG, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_OBEX_PASSWORD_TOO_LONG);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_AUTH_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_auth_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_auth_rsp.read_only = read_only;
  cmd_ptr->cmd_msg.cmd_ftp_srv_auth_rsp.allow_browse = allow_browse;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return(cmd_status);
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_accept_connect

DESCRIPTION
  A application calls this function to accept or reject an indicated
  connection.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_accept_connect(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  boolean                       accept,
  uint16*                       root_folder_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;
  uint8                 len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV ACCEPT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_unicode_str(
                  (uint16*)(cmd_ptr->cmd_msg.cmd_ftp_srv_accept.root_folder_str),
                  root_folder_ptr, BT_PF_MAX_FILENAME_LEN,
                  &len ) )
  {
    BT_ERR( "BT PF CMD FTP SRV ACCEPT: invalid root, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_ACCEPT;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_accept.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_accept.accept = accept;
  cmd_ptr->cmd_msg.cmd_ftp_srv_accept.root_folder_len = len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_force_disconnect

DESCRIPTION
  Forcibly severs the connection from an FTP client to the FTP server. This
  function should be called for all active connections before terminating
  the FTP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_force_disconnect(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_FORCE_DCN;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ftp_srv_force_dcn.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD FTP SRV FORCE DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_browse_done

DESCRIPTION
  This command is called by the application when the browse folder operation
  is completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_browse_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV BRW DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_BROWSE_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.handle = handle;
  cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.size = size;
  cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_open_done

DESCRIPTION
  This command is called by the application when the file open operation is
  completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_open_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV OPN DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_OPEN_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.handle = handle;
  cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.size = size;
  cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_read_done

DESCRIPTION
  This command is called by the application when the file read operation is
  completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_read_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV RD DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_READ_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.handle = handle;
  cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.len = len;
  cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_write_done

DESCRIPTION
  This command is called by the application when the file write operation is
  completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_write_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV WRT DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_WRITE_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_write_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_write_done.handle = handle;
  cmd_ptr->cmd_msg.cmd_ftp_srv_write_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_delete_done

DESCRIPTION
  This command is called by the application when the delete file operation is
  completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_delete_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV DEL DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_DELETE_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_delete_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_delete_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_srv_set_folder_done

DESCRIPTION
  This command is called by the application when the set folder operation is
  completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_srv_set_folder_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD FTP SRV SET FDR DNE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_SRV_SET_FOLDER_DONE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_set_folder_done.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_ftp_srv_set_folder_done.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}


#endif /* FEATURE_BT_EXTPF_FTP */

#ifdef FEATURE_BT_EXTPF_BIP

#ifndef BT_MSG_LAYER
#define BT_MSG_LAYER BT_MSG_GN
#endif

#define BIP_COPY_IMG_HNDL()                                      \
   ( ((OI_OK == bt_pf_copy_unicode_str(                          \
                  (uint16*)(bip_cmd_ptr->image_handle_str),      \
                  (uint16*)(image_handle_ptr->image_handle_str), \
                  BT_PF_BIP_IMAGE_HANDLE_LEN, &len )) &&         \
      ((BT_PF_BIP_IMAGE_HANDLE_LEN + 1) == len)) ?               \
     OI_OK : OI_STATUS_INVALID_PARAMETERS )

/*===========================================================================
  Helper function to verify and copy image handle
===========================================================================*/
LOCAL bt_cmd_status_type bt_cmd_pf_bip_check_image_handle(
  bt_pf_bip_image_handle_type*  src_handle_ptr, // can be NULL
  uint16*                       dst_handle_ptr,
  bt_cmd_status_type            status,
  uint8*                        len_ptr
)
{
  bt_cmd_status_type cmd_status;
  if ( NULL == src_handle_ptr )
  {
    if ( (OI_OK == status) || (OI_OBEX_CONTINUE == status) ||
         (OI_OBEX_PARTIAL_CONTENT == status) )
    {
       cmd_status = OI_STATUS_INVALID_PARAMETERS;
    }
    else
    {
      *dst_handle_ptr = 0;
      cmd_status = OI_OK;
    }
  }
  else
  {
    cmd_status = 
       ((OI_OK == bt_pf_copy_unicode_str(
                    dst_handle_ptr, 
                    (uint16*)(src_handle_ptr->image_handle_str),
                    BT_PF_BIP_IMAGE_HANDLE_LEN, len_ptr )) &&
        ((BT_PF_BIP_IMAGE_HANDLE_LEN + 1) == *len_ptr)) ?
       OI_OK : OI_STATUS_INVALID_PARAMETERS;
  }
  return cmd_status;
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_connect

DESCRIPTION
  Connects to a remote BIP imaging responder, referenced object server, or
  archived object server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel,
  byte*                                target_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = OI_STATUS_INVALID_PARAMETERS;

  if ( (bd_addr_ptr != NULL) && (target_ptr != NULL) )
  {
    if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
    {
      cmd_status = BT_CS_GN_CMD_Q_FULL;
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_CONNECT;
      cmd_ptr->cmd_hdr.app_id = app_id;
      cmd_ptr->cmd_msg.cmd_bip_cli_connect.bd_addr = *bd_addr_ptr;
      cmd_ptr->cmd_msg.cmd_bip_cli_connect.channel = channel;
      memcpy( (void*)(cmd_ptr->cmd_msg.cmd_bip_cli_connect.target),
              (void*)(target_ptr),
              BT_PF_UUID128_SIZE );

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }
  BT_MSG_DEBUG( "BT PF CMD BIP CLI CON: App=%x target=%x status=%x", 
                app_id, target_ptr, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_disconnect

DESCRIPTION
  Connects to a remote BIP imaging responder, referenced object server, or
  archived object server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_disconnect(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_cli_disconnect.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI DCN: App=%x connID=%x status=%x", 
                app_id, conn_id, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_auth_response

DESCRIPTION
  Responds to a authentication request during connection with a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_auth_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  char*                                pin_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
     if (pin_ptr == NULL) /* It's ok to have no password. */
     {
       cmd_ptr->cmd_msg.cmd_bip_cli_auth_rsp.pin_str[0] = '\0';
       cmd_status = OI_OK;
     }
     else
     {
       cmd_status = bt_pf_copy_ascii_str(
                      (char*)(cmd_ptr->cmd_msg.cmd_bip_cli_auth_rsp.pin_str),
                      pin_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN );
     }

     if (OI_OK != cmd_status)
     {
       bt_pf_free_cmd( cmd_ptr );
     }
     else
     {
       cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_AUTH_RSP;
       cmd_ptr->cmd_hdr.app_id = app_id;
       cmd_ptr->cmd_msg.cmd_bip_cli_auth_rsp.conn_id = conn_id;

       cmd_status = bt_pf_cmd( cmd_ptr );
     }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI AUTH: App=%x connID=%x status=%x", 
                app_id, conn_id, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_put_image

DESCRIPTION
  Sends a GetCapabilities request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_capabilities(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_GET_CAPABILITIES;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_cli_get_capabilities.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI GET CAP: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_put_image

DESCRIPTION
  Sends a PutImage request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_put_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  uint16*                              image_name_ptr,
  bt_pf_byte_seq_type*                 image_descriptior_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type*               cmd_ptr;
  bt_cmd_status_type                cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_put_image_type* bip_cmd_ptr;
  uint8                             len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_put_image;
    if (image_name_ptr == NULL)
    {
      bip_cmd_ptr->name_str[0] = 0;
      cmd_status = OI_OK;
    }
    else
    {
      cmd_status = bt_pf_copy_unicode_str(
                     (uint16*)(bip_cmd_ptr->name_str),
                     image_name_ptr, BT_PF_MAX_FILENAME_LEN,
                     &len );
    }

    if ( OI_OK != cmd_status )
    {
      bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type    = BT_PF_CMD_BIP_CLI_PUT_IMAGE;
      cmd_ptr->cmd_hdr.app_id      = app_id;
      bip_cmd_ptr->conn_id         = conn_id;
      bip_cmd_ptr->descriptior_ptr = image_descriptior_ptr;
      bip_cmd_ptr->data_ptr        = data_ptr;
      bip_cmd_ptr->name_len        = (uint16)len;
      bip_cmd_ptr->final           = final;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI PUT IMG: App=%x status=%x", 
                app_id, cmd_status, 0 );
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_put_linked_thumbnail

DESCRIPTION
  Sends a PutLinkedThumbnail request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_put_linked_thumbnail(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type*                   cmd_ptr;
  bt_cmd_status_type                    cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_put_thumbnail_type* bip_cmd_ptr;
  uint8                                 len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_put_thumbnail;

    if ( OI_OK != (cmd_status = BIP_COPY_IMG_HNDL()) )
    {
      bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_PUT_LINKED_THUMBNAIL;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->conn_id      = conn_id;
      bip_cmd_ptr->data_ptr     = data_ptr;
      bip_cmd_ptr->final        = final;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI PUT TNL: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_put_linked_attachment

DESCRIPTION
  Sends a PutLinkedAttachment request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_put_linked_attachment(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  bt_pf_byte_seq_type*                 attachment_descriptior_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type*                    cmd_ptr;
  bt_cmd_status_type                     cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_put_attachment_type* bip_cmd_ptr;
  uint8                                  len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_put_attachment;

    if ( OI_OK != (cmd_status = BIP_COPY_IMG_HNDL()) )
    {
      bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type    = BT_PF_CMD_BIP_CLI_PUT_LINKED_ATTACHMENT;
      cmd_ptr->cmd_hdr.app_id      = app_id;
      bip_cmd_ptr->conn_id         = conn_id;
      bip_cmd_ptr->descriptior_ptr = attachment_descriptior_ptr;
      bip_cmd_ptr->data_ptr        = data_ptr;
      bip_cmd_ptr->final           = final;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI PUT ATT: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_get_images_list

DESCRIPTION
  Sends a GetImagesList request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_images_list(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  uint16                               handle_count,
  uint16                               handle_offset,
  uint8                                latest,
  bt_pf_byte_seq_type*                 image_handles_descriptor_ptr )
{
  bt_pf_cmd_msg_type*              cmd_ptr;
  bt_cmd_status_type               cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_get_list_type* bip_cmd_ptr;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_get_list;
    cmd_ptr->cmd_hdr.cmd_type  = BT_PF_CMD_BIP_CLI_GET_IMAGES_LIST;
    cmd_ptr->cmd_hdr.app_id    = app_id;
    bip_cmd_ptr->conn_id       = conn_id;
    bip_cmd_ptr->handle_count  = handle_count;
    bip_cmd_ptr->handle_offset = handle_offset;
    bip_cmd_ptr->latest        = latest;
    bip_cmd_ptr->image_handles_descriptor_ptr = image_handles_descriptor_ptr;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI GET LIST: App=%x status=%x", 
                app_id, cmd_status, 0 );
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_get_image_properties

DESCRIPTION
  Sends a GetImageProperties request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_image_properties(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr )
{
  bt_pf_cmd_msg_type*                    cmd_ptr;
  bt_cmd_status_type                     cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_get_properties_type* bip_cmd_ptr;
  uint8                                  len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_get_properties;

    if ( OI_OK != (cmd_status = BIP_COPY_IMG_HNDL()) )
    {
      bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_GET_IMAGE_PROPERTIES;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->conn_id      = conn_id;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI GET PROP: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_get_image

DESCRIPTION
  Sends a GetImage request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  bt_pf_byte_seq_type*                 image_descriptior_ptr )
{
  bt_pf_cmd_msg_type*               cmd_ptr;
  bt_cmd_status_type                cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_get_image_type* bip_cmd_ptr;
  uint8                             len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_get_image;

    if ( OI_OK != (cmd_status = BIP_COPY_IMG_HNDL()) )
    {
      bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type    = BT_PF_CMD_BIP_CLI_GET_IMAGE;
      cmd_ptr->cmd_hdr.app_id      = app_id;
      bip_cmd_ptr->conn_id         = conn_id;
      bip_cmd_ptr->descriptior_ptr = image_descriptior_ptr;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI GET IMG: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_get_linked_thumbnail

DESCRIPTION
  Sends a GetLinkedThumbnail request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_linked_thumbnail(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr )
{
  bt_pf_cmd_msg_type*                   cmd_ptr;
  bt_cmd_status_type                    cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_get_thumbnail_type* bip_cmd_ptr;
  uint8                                 len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_get_thumbnail;

    if ( OI_OK != (cmd_status = BIP_COPY_IMG_HNDL()) )
    {
      bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_GET_LINKED_THUMBNAIL;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->conn_id      = conn_id;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI GET TNL: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_get_linked_attachment

DESCRIPTION
  Sends a GetLinkedAttachment request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_linked_attachment(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr,
  uint16*                              attachment_name_ptr )
{
  bt_pf_cmd_msg_type*                    cmd_ptr;
  bt_cmd_status_type                     cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_get_attachment_type* bip_cmd_ptr = NULL;
  uint8                                  len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_get_attachment;

    if ( (OI_OK != (cmd_status = BIP_COPY_IMG_HNDL())) ||
         (OI_OK != (cmd_status = bt_pf_copy_unicode_str(
                                   (uint16*)(bip_cmd_ptr->name_str),
                                   attachment_name_ptr, 
                                   BT_PF_MAX_FILENAME_LEN, NULL ))) )
    {
      bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_GET_LINKED_ATTACHMENT;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->conn_id      = conn_id;
      bip_cmd_ptr->name_len     = len;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI GET ATT: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_delete_image

DESCRIPTION
  Sends a DeleteImage request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_delete_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr )
{
  bt_pf_cmd_msg_type*                  cmd_ptr;
  bt_cmd_status_type                   cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_delete_image_type* bip_cmd_ptr;
  uint8                                len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_delete_image;

    if ( OI_OK != (cmd_status = BIP_COPY_IMG_HNDL()) )
    {
      bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_DELETE_IMAGE;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->conn_id      = conn_id;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI DEL IMG: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_get_status

DESCRIPTION
  Sends a GetStatus request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_status(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_GET_STATUS;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_cli_get_status.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI GET STAT: App=%x conn_id=%d status=%x", 
                app_id, conn_id, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_get_partial_image

DESCRIPTION
  Sends a GetPartialImage request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_partial_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  uint16*                              image_name_ptr,
  uint32                               partial_length,
  uint32                               partial_offset )
{
  bt_pf_cmd_msg_type*                       cmd_ptr;
  bt_cmd_status_type                        cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_get_partial_image_type* bip_cmd_ptr;
  uint8                                     len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_get_partial_image;

    if ( OI_OK != bt_pf_copy_unicode_str(
                    (uint16*)(bip_cmd_ptr->name_str),
                    image_name_ptr, BT_PF_MAX_FILENAME_LEN,
                    &len ) )
    {
      bt_pf_free_cmd( cmd_ptr );
      cmd_status = OI_STATUS_INVALID_PARAMETERS;
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type   = BT_PF_CMD_BIP_CLI_GET_PARTIAL_IMAGE;
      cmd_ptr->cmd_hdr.app_id     = app_id;
      bip_cmd_ptr->conn_id        = conn_id;
      bip_cmd_ptr->partial_length = partial_length;
      bip_cmd_ptr->partial_offset = partial_offset;
      bip_cmd_ptr->name_len       = len;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI GET PART IMG: App=%x conn_id=%d status=%x", 
                app_id, conn_id, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_start_print

DESCRIPTION
  Sends a StartPrint request to a BIP print.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_start_print(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_sd_uuid128_type*                  service_id_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type*                 cmd_ptr;
  bt_cmd_status_type                  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_start_print_type* bip_cmd_ptr;

  if ( service_id_ptr == NULL )
  {
    cmd_status = OI_STATUS_INVALID_PARAMETERS;
  }
  else if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_start_print;
    cmd_ptr->cmd_hdr.cmd_type           = BT_PF_CMD_BIP_CLI_START_PRINT;
    cmd_ptr->cmd_hdr.app_id             = app_id;
    bip_cmd_ptr->conn_id                = conn_id;
    bip_cmd_ptr->ref_obj_srv_service_id = *service_id_ptr;
    bip_cmd_ptr->data_ptr               = data_ptr;
    bip_cmd_ptr->final                  = final;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI START PRINT: App=%x conn_id=%d status=%x", 
                app_id, conn_id, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_start_archive

DESCRIPTION
  Sends a StartArchive request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_start_archive(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_sd_uuid128_type*                  service_id_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_start_archive_type* bip_cmd_ptr;

  if ( service_id_ptr == NULL )
  {
    cmd_status = OI_STATUS_INVALID_PARAMETERS;
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_start_archive;
    cmd_ptr->cmd_hdr.cmd_type            = BT_PF_CMD_BIP_CLI_START_ARCHIVE;
    cmd_ptr->cmd_hdr.app_id              = app_id;
    bip_cmd_ptr->conn_id                 = conn_id;
    bip_cmd_ptr->arch_obj_srv_service_id = *service_id_ptr;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI START ARCH: App=%x conn_id=%d status=%x", 
                app_id, conn_id, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_remote_display

DESCRIPTION
  Sends a RemoteDisplay request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_remote_display(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr, // can be NULL
  uint8                                display_command )
{
  bt_pf_cmd_msg_type*                    cmd_ptr;
  bt_cmd_status_type                     cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_cli_remote_display_type* bip_cmd_ptr = NULL;
  uint8                                  len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_cli_remote_display;
    if ( image_handle_ptr == NULL )
    {
      bip_cmd_ptr->image_handle_str[ 0 ] = 0;
      cmd_status = OI_OK;
    }
    else if ( OI_OK != (cmd_status = BIP_COPY_IMG_HNDL()) )
    {
      bt_pf_free_cmd( cmd_ptr );
    }

    if ( OI_OK == cmd_status)
    {
      cmd_ptr->cmd_hdr.cmd_type    = BT_PF_CMD_BIP_CLI_REMOTE_DISPLAY;
      cmd_ptr->cmd_hdr.app_id      = app_id;
      bip_cmd_ptr->conn_id         = conn_id;
      bip_cmd_ptr->display_command = display_command;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI REMOTE DISP: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_get_monitoring_image

DESCRIPTION
  Sends a GetMonitoringImage request to a BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_monitoring_image(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id,
  boolean                              store_image )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_CLI_GET_MONITORING_IMAGE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_cli_get_monitor_image.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bip_cli_get_monitor_image.store_image = store_image;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP CLI GET MON IMG: App=%x conn_id=%d status=%x", 
                app_id, conn_id, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_register_imaging_responder

DESCRIPTION
  Initialize the BIP imaging responder and register it in the service
  discovery database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_register_imaging_responder(
  bt_app_id_type          app_id,
  char*                   srv_name_ptr,
  uint8                   supported_capabilities,
  uint16                  supported_features,
  uint32                  supported_functions,
  uint64                  imaging_capacity,
  boolean                 auth )
{
  bt_pf_cmd_msg_type*                       cmd_ptr;
  bt_cmd_status_type                        cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_reg_img_responder_type* bip_cmd_ptr;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_reg_img_responder;
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(bip_cmd_ptr->srv_name_str),
                    srv_name_ptr, BT_PF_MAX_FILENAME_LEN ) )
    {
      bt_pf_free_cmd( cmd_ptr );
      cmd_status = OI_STATUS_INVALID_PARAMETERS;
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_REG_IMAGING_RERSPONDER;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->supported_capabilities = supported_capabilities;
      bip_cmd_ptr->supported_features     = supported_features;
      bip_cmd_ptr->supported_functions    = supported_functions;
      bip_cmd_ptr->imaging_capacity       = imaging_capacity;
      bip_cmd_ptr->auth                   = auth;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV REG IMG RSPDR: App=%x name=%x status=%x", 
                app_id, srv_name_ptr, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_register_referenced_objects

DESCRIPTION
  Initialize the BIP referenced objects server and register it in the service
  discovery database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_register_referenced_objects(
  bt_app_id_type          app_id,
  char*                   srv_name_ptr,
  bt_sd_uuid128_type*     service_id_ptr,
  uint32                  supported_functions,
  boolean                 auth )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_reg_ref_objects_type* bip_cmd_ptr;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_reg_ref_objects;
    if ( (service_id_ptr == NULL) ||
         (OI_OK != bt_pf_copy_ascii_str(
                     (char*)(bip_cmd_ptr->srv_name_str),
                     srv_name_ptr, BT_PF_MAX_FILENAME_LEN )) )
    {
      bt_pf_free_cmd( cmd_ptr );
      cmd_status = OI_STATUS_INVALID_PARAMETERS;
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_REG_REFERENCED_OBJECTS;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->supported_functions = supported_functions;
      bip_cmd_ptr->service_id          = *service_id_ptr;
      bip_cmd_ptr->auth                = auth;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV REG REF OBJ: App=%x name=%x status=%x", 
                app_id, srv_name_ptr, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_register_archived_objects

DESCRIPTION
  Initialize the BIP archived objects server and register it in the service
  discovery database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_register_archived_objects(
  bt_app_id_type          app_id,
  char*                   srv_name_ptr,
  bt_sd_uuid128_type*     service_id_ptr,
  uint32                  supported_functions,
  boolean                 auth )
{
  bt_pf_cmd_msg_type*                      cmd_ptr;
  bt_cmd_status_type                       cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_reg_arch_objects_type* bip_cmd_ptr;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_reg_arch_objects;
    if ( (service_id_ptr == NULL) ||
         (OI_OK != bt_pf_copy_ascii_str(
                     (char*)(bip_cmd_ptr->srv_name_str),
                     srv_name_ptr, BT_PF_MAX_FILENAME_LEN )) )
    {
      bt_pf_free_cmd( cmd_ptr );
      cmd_status = OI_STATUS_INVALID_PARAMETERS;
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_REG_ARCHIVED_OBJECTS;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->supported_functions = supported_functions;
      bip_cmd_ptr->service_id          = *service_id_ptr;
      bip_cmd_ptr->auth                = auth;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV REG ARCH OBJ: App=%x name=%x status=%x", 
                app_id, srv_name_ptr, cmd_status );

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_deregister_imaging_responder

DESCRIPTION
  Deregisters a BIP imaging responder.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_deregister_imaging_responder(
  bt_app_id_type    app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_DEREG_IMAGING_RERSPONDER;
    cmd_ptr->cmd_hdr.app_id   = app_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV DEREG IMG RSPDR: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_deregister_referenced_objects

DESCRIPTION
  Deregisters a BIP referenced object server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_deregister_referenced_objects(
  bt_app_id_type    app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_DEREG_REFERENCED_OBJECTS;
    cmd_ptr->cmd_hdr.app_id   = app_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV DEREG REF OBJ: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_deregister_archived_objects

DESCRIPTION
  Deregisters a BIP archived object server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_deregister_archived_objects(
  bt_app_id_type    app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_DEREG_ARCHIVED_OBJECTS;
    cmd_ptr->cmd_hdr.app_id = app_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV DEREG ARCH OBJ: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_auth_response

DESCRIPTION
  Responds to an authentication request during connection with a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_auth_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  char*                                pin_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_auth_rsp_type* bip_cmd_ptr;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_auth_rsp;
    if (pin_ptr == NULL) /* It's ok to have no pin. */
    {
      bip_cmd_ptr->pin_str[0] = '\0';
      cmd_status = OI_OK;
    }
    else if ( OI_OK != bt_pf_copy_ascii_str(
                       (char*)(bip_cmd_ptr->pin_str),
                       pin_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN ) )
    {
      bt_pf_free_cmd( cmd_ptr );
      cmd_status = OI_OBEX_PASSWORD_TOO_LONG;
    }

    if (cmd_status == OI_OK)
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_AUTH_RSP;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->conn_id      = conn_id;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV AUTH RSP: App=%x pin=%x status=%x", 
                app_id, pin_ptr, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_force_disconnect

DESCRIPTION
  Forcibly severs the connection from an BIP client to the BIP server.
  This function should be called for all active connections before
  terminating the BIP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_force_disconnect(
  bt_app_id_type             app_id,
  bt_pf_bip_srv_conn_id_type conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_FORCE_DCN;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_force_dcn.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV DCN: App=%x conn_id=%d status=%x", 
                app_id, conn_id, cmd_status );
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_get_capabilities_response

DESCRIPTION
  Responds to a GetCapabilities request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_get_capabilities_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type*                          cmd_ptr;
  bt_cmd_status_type                           cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_get_capabilities_rsp_type* bip_cmd_ptr;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_get_capabilities_rsp;
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_GET_CAPABILITIES_RSP;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    bip_cmd_ptr->conn_id      = conn_id;
    bip_cmd_ptr->data_ptr     = data_ptr;
    bip_cmd_ptr->final        = final;
    bip_cmd_ptr->status       = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV GET CAP RSP: App=%x conn_id=%d status=%x", 
                app_id, conn_id, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_put_image_response

DESCRIPTION
  Responds to a PutImage request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_put_image_response(
  bt_app_id_type               app_id,
  bt_pf_bip_srv_conn_id_type   conn_id,
  bt_pf_bip_image_handle_type* image_handle_ptr, // can be null
  bt_cmd_status_type           status )
{
  bt_pf_cmd_msg_type*                   cmd_ptr;
  bt_cmd_status_type                    cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_put_image_rsp_type* bip_cmd_ptr;
  uint8                                 len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_put_image_rsp;

    if ( OI_OK != (cmd_status = bt_cmd_pf_bip_check_image_handle(
                                  image_handle_ptr, 
                                  (uint16*)bip_cmd_ptr->image_handle_str, 
                                  status, &len )) )
    {
       bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_PUT_IMAGE_RSP;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->conn_id      = conn_id;
      bip_cmd_ptr->status       = status;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV PUT IMG RSP: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_put_linked_thumbnail_response

DESCRIPTION
  Responds to a PutLinkedThumbnail request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_put_linked_thumbnail_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_PUT_LINKED_THUMBNAIL_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_put_thumbnail_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_put_thumbnail_rsp.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV PUT TNL RSP: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_put_linked_attachment_response

DESCRIPTION
  Responds to a PutLinkedAttachment request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_put_linked_attachment_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_PUT_LINKED_ATTACHMENT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_put_attachment_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_put_attachment_rsp.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV PUT ATT RSP: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_get_images_list_response

DESCRIPTION
  Responds to a GetImagesList request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_get_images_list_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  uint16                               num_handles,
  bt_pf_byte_seq_type*                 image_handles_descriptor_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type*                  cmd_ptr;
  bt_cmd_status_type                   cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_get_list_rsp_type* bip_cmd_ptr;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_get_list_rsp;

    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_GET_IMAGES_LIST_RSP;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    bip_cmd_ptr->conn_id                      = conn_id;
    bip_cmd_ptr->status                       = status;
    bip_cmd_ptr->num_returned_handles         = num_handles;
    bip_cmd_ptr->image_handles_descriptor_ptr = image_handles_descriptor_ptr;
    bip_cmd_ptr->data_ptr                     = data_ptr;
    bip_cmd_ptr->final                        = final;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV GET LIST RSP: App=%x final=%d status=%x", 
                app_id, final, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_get_image_properties_response

DESCRIPTION
  Responds to a GetImageProperties request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_get_image_properties_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type*                        cmd_ptr;
  bt_cmd_status_type                         cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_get_properties_rsp_type* bip_cmd_ptr;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_get_properties_rsp;

    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_GET_IMAGE_PROPERTIES_RSP;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    bip_cmd_ptr->conn_id      = conn_id;
    bip_cmd_ptr->status       = status;
    bip_cmd_ptr->data_ptr     = data_ptr;
    bip_cmd_ptr->final        = final;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV GET PROP RSP: App=%x final=%d status=%x", 
                app_id, final, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_get_image_response

DESCRIPTION
  Responds to a GetImage request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_get_image_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  uint32                               file_size,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_GET_IMAGE_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_image_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_image_rsp.status = status;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_image_rsp.file_size = file_size;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_image_rsp.data_ptr = data_ptr;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_image_rsp.final = final;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV GET IMG RSP: App=%x final=%d status=%x", 
                app_id, final, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_get_linked_thumbnail_response

DESCRIPTION
  Responds to a GetLinkedThumbnail request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_get_linked_thumbnail_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_GET_LINKED_THUMBNAIL_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_thumbnail_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_thumbnail_rsp.status = status;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_thumbnail_rsp.data_ptr = data_ptr;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_thumbnail_rsp.final = final;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV GET TNL RSP: App=%x final=%d status=%x", 
                app_id, final, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_get_linked_attachment_response

DESCRIPTION
  Responds to a GetLinkedAttachment request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_get_linked_attachment_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_GET_LINKED_ATTACHMENT_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_attachment_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_attachment_rsp.status = status;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_attachment_rsp.data_ptr = data_ptr;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_attachment_rsp.final = final;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV GET ATT RSP: App=%x final=%d status=%x", 
                app_id, final, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_delete_image_response

DESCRIPTION
  Responds to a DeleteImage request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_delete_image_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_DELETE_IMAGE_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_delete_image_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_delete_image_rsp.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV DEL IMG RSP: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_get_status_response

DESCRIPTION
  Responds to a GetStatus request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_get_status_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_GET_STATUS_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_status_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_get_status_rsp.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV GET STAT RSP: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_get_partial_image_response

DESCRIPTION
  Responds to a GetPartialImage request from a BIP printer.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_get_partial_image_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  uint32                               total_file_size,
  boolean                              end_flag,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_get_partial_image_rsp_type* bip_cmd_ptr;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_get_partial_image_rsp;

    cmd_ptr->cmd_hdr.cmd_type    = BT_PF_CMD_BIP_SRV_GET_PARTIAL_IMAGE_RSP;
    cmd_ptr->cmd_hdr.app_id      = app_id;
    bip_cmd_ptr->conn_id         = conn_id;
    bip_cmd_ptr->total_file_size = total_file_size;
    bip_cmd_ptr->end_flag        = end_flag;
    bip_cmd_ptr->data_ptr        = data_ptr;
    bip_cmd_ptr->final           = final;
    bip_cmd_ptr->status          = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV GET PART IMG RSP: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_start_archive_response

DESCRIPTION
  Responds to a StartArchive request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_start_archive_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_START_ARCHIVE_RSP;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_start_archive_rsp.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bip_srv_start_archive_rsp.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV START ARCH RSP: App=%x status=%x", 
                app_id, cmd_status, 0 );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_remote_display_response

DESCRIPTION
  Responds to a RemoteDisplay request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_remote_display_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr, // can be NULL
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_remote_display_rsp_type* bip_cmd_ptr;
  uint8                                      len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_remote_display_rsp;

    if ( OI_OK != (cmd_status = bt_cmd_pf_bip_check_image_handle(
                                  image_handle_ptr, 
                                  (uint16*)bip_cmd_ptr->image_handle_str, 
                                  status, &len )) )
    {
       bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_REMOTE_DISPLAY_RSP;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->conn_id      = conn_id;
      bip_cmd_ptr->status       = status;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV REMOTE DISP RSP: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_get_monitoring_image_response

DESCRIPTION
  Responds to a GetMonitoringImage request from a BIP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_get_monitoring_image_response(
  bt_app_id_type                       app_id,
  bt_pf_bip_srv_conn_id_type           conn_id,
  bt_pf_bip_image_handle_type*         image_handle_ptr, // can be NULL
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_pf_cmd_bip_srv_get_monitor_image_rsp_type* bip_cmd_ptr = NULL;
  uint8                                         len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    bip_cmd_ptr = &cmd_ptr->cmd_msg.cmd_bip_srv_get_monitor_image_rsp;

    if ( OI_OK != (cmd_status = bt_cmd_pf_bip_check_image_handle(
                                  image_handle_ptr, 
                                  (uint16*)bip_cmd_ptr->image_handle_str, 
                                  status, &len )) )
    {
       bt_pf_free_cmd( cmd_ptr );
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BIP_SRV_GET_MONITORING_IMAGE_RSP;
      cmd_ptr->cmd_hdr.app_id   = app_id;
      bip_cmd_ptr->conn_id      = conn_id;
      bip_cmd_ptr->data_ptr     = data_ptr;
      bip_cmd_ptr->final        = final;
      bip_cmd_ptr->status       = status;

      cmd_status = bt_pf_cmd( cmd_ptr );
    }
  }

  BT_MSG_DEBUG( "BT PF CMD BIP SRV MON IMG RSP: App=%x hndlLen=%d status=%x", 
                app_id, len, cmd_status );

  return( cmd_status );
}

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF_BIP */

#ifdef FEATURE_BT_EXTPF_BPP

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_connect

DESCRIPTION
  Connect to a BPP printer.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel,
  bt_pf_bpp_target_type                target )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( bd_addr_ptr == NULL )
  {
    BT_ERR("BT PF CMD BPP SNDR CON: invalid BD addr, App: %x, Stat: %x",
           app_id, OI_STATUS_INVALID_PARAMETERS, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_CONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bpp_sndr_connect.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_bpp_sndr_connect.channel = channel;
    cmd_ptr->cmd_msg.cmd_bpp_sndr_connect.target = target;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD BPP SNDR CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_disconnect

DESCRIPTION
  Disconnect from a BPP printer.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_disconnect(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bpp_sndr_disconnect.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD BPP SNDR DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_auth_response

DESCRIPTION
  Response to a authentication request during a connection to a BPP printer.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_auth_response(
  bt_app_id_type                       app_id,
  bt_pf_bpp_target_type                target,
  byte*                                user_id_ptr,
  uint8                                user_id_len,
  char*                                password_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type cmd_status;

  if ( (user_id_len > OI_OBEX_MAX_USERID_LEN) ||
       ( (user_id_len > 0) && (user_id_ptr == NULL) ) )
  {
    BT_ERR("BT PF CMD BPP SNDR AUTH: invalid UID, App: %x",
           app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD BPP SNDR AUTH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (user_id_len > 0)
  {
    memcpy( (void*)cmd_ptr->cmd_msg.cmd_bpp_sndr_auth_rsp.user_id,
            (void*)user_id_ptr,
            user_id_len );
  }

  if (password_ptr == NULL) /* It's ok to have no password. */
  {
    cmd_ptr->cmd_msg.cmd_bpp_sndr_auth_rsp.password_str[0] = '\0';
  }
  else
  {
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(cmd_ptr->cmd_msg.cmd_bpp_sndr_auth_rsp.password_str),
                    password_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN ) )
    {
      BT_ERR( "BT PF CMD BPP SNDR AUTH: invalid pwd, App: %x, Stat: %x",
              app_id, OI_OBEX_PASSWORD_TOO_LONG, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_OBEX_PASSWORD_TOO_LONG);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_AUTH_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_auth_rsp.target = target;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_auth_rsp.user_id_len = user_id_len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_send_file

DESCRIPTION
  Executes a FilePush or SendDocument command.
  Sends a file to be printed to a BPP printer.
  The printer may then set up a secondary OBEX connection to the referenced
  object server to retrieve referenced contents from BPP sender.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_send_file(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id,
  char*                                type_ptr,
  uint16*                              description_ptr,
  uint16*                              name_ptr,
  bt_pf_bpp_job_id_type*               job_id_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type cmd_status;
  uint8                 len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD BPP SNDR SND FILE: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (type_ptr == NULL)
  {
    cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.type_str[0] = 0;
  }
  else
  {
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.type_str),
                    type_ptr, BT_PF_MAX_MIME_TYPE_LEN ) )
    {
      BT_ERR( "BT PF CMD BPP SNDR SND FILE: invalid type, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  // NULL terminate the string
  len = strlen( (char *)cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.type_str );
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.type_str[ len ] = '\0';

  if (name_ptr == NULL) /* It's ok to have no doc name. */
  {
    len = 0;
  }
  else
  {
    if ( OI_OK != bt_pf_copy_unicode_str(
                    (uint16*)(cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.name_str),
                    name_ptr, BT_PF_MAX_FILENAME_LEN,
                    &len ) )
    {
      BT_ERR( "BT PF CMD BPP SNDR SND FILE: invalid name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  if (job_id_ptr == NULL)
  {
    cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.job_id = 0;
  }
  else if (*job_id_ptr == 0)
  {
    BT_ERR( "BT PF CMD BPP SNDR SND FILE: invalid job id, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  else
  {
    cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.job_id = *job_id_ptr;
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_SEND_FILE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.final = final;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.name_len = len;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.description_ptr = description_ptr;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_send_reference

DESCRIPTION
  Executes a SimpleReferencePush or SendReference command.
  Sends a reference to print contents to a BPP printer.
  The printer will retrieve the print contents from an external print server
  or content provider.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_send_reference(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id,
  bt_pf_bpp_reference_type             type,
  bt_pf_byte_seq_type*                 http_headers_ptr,
  bt_pf_bpp_job_id_type*               job_id_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD BPP SNDR SND REF: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (job_id_ptr == NULL)
  {
    cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.job_id = 0;
  }
  else if (*job_id_ptr == 0)
  {
    BT_ERR( "BT PF CMD BPP SNDR SND REF: invalid job id, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  else
  {
    cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.job_id = *job_id_ptr;
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_SEND_REFERENCE;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_reference.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_reference.type = type;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_reference.http_headers_ptr =
    http_headers_ptr;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_reference.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_send_reference.final = final;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_soap_request

DESCRIPTION
  Sends a SOAP request to a BPP printer.
  Body of SOAP requests are encoded in XML.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_soap_request(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id,
  bt_pf_byte_seq_type*                 body_ptr,
  boolean                              final )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_SOAP_REQ;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bpp_sndr_soap_req.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_bpp_sndr_soap_req.body_ptr = body_ptr;
    cmd_ptr->cmd_msg.cmd_bpp_sndr_soap_req.final = final;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD BPP SNDR SOAP REQ: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_srv_register

DESCRIPTION
  Registers a referenced object server on the local BPP sender.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_register(
  bt_app_id_type    app_id,
  char*             srv_name_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD BPP SNDR REG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_ascii_str(
                  (char*)(cmd_ptr->cmd_msg.cmd_bpp_sndr_register.srv_name_str),
                  srv_name_ptr, BT_PF_MAX_FILENAME_LEN ) )
  {
    BT_ERR( "BT PF CMD BPP SNDR REG: invalid srv name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_REGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_deregister

DESCRIPTION
  Deregisters a referenced object server on the local BPP sender.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_deregister( bt_app_id_type    app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD BPP SNDR DEREG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_DEREGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_get_ref_obj_response

DESCRIPTION
  Responds to a get reference object request from a BPP printer.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_get_ref_obj_response(
  bt_app_id_type                       app_id,
  bt_pf_bpp_sndr_conn_id_type          conn_id,
  bt_pf_byte_seq_type*                 data_ptr,
  int32*                               file_size_ptr,
  bt_cmd_status_type                status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;


  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD BPP SNDR REF OBJ RSP: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (file_size_ptr == NULL)
  {
    cmd_ptr->cmd_msg.cmd_bpp_sndr_get_ref_obj_rsp.file_size = -1;
  }
  else if (*file_size_ptr < 0)
  {
    BT_ERR( "BT PF CMD BPP SNDR REF OBJ RSP: invalid file sz, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  else
  {
    cmd_ptr->cmd_msg.cmd_bpp_sndr_get_ref_obj_rsp.file_size = *file_size_ptr;
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_GET_REF_OBJ_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_get_ref_obj_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_get_ref_obj_rsp.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_bpp_sndr_get_ref_obj_rsp.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_force_disconnect

DESCRIPTION
  Terminates a connection to BPP Object channel.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_force_disconnect(
  bt_app_id_type             app_id,
  bt_pf_bpp_sndr_conn_id_type conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_BPP_SNDR_FORCE_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_bpp_sndr_force_disconnect.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD BPP SRV DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}
#endif /* FEATURE_BT_EXTPF_BPP */

#ifdef FEATURE_BT_EXTPF_SML

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_register_client

DESCRIPTION
  Initialize an OBEX SyncML client and register it in the service
  discovery database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_register_client(
  bt_app_id_type          app_id,
  char*                   srv_name_ptr,
  boolean                 auth )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML REG CLI: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_REGISTER_CLI;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_sml_register_cli.auth = auth;

  if ( OI_OK != bt_pf_copy_ascii_str(
             (char*)(cmd_ptr->cmd_msg.cmd_sml_register_cli.srv_name_str),
             srv_name_ptr, BT_PF_MAX_FILENAME_LEN ) )
  {
    BT_ERR( "BT PF CMD SML REG CLI: invalid srv name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_register_server

DESCRIPTION
  Initialize an OBEX SyncML server and register it in the service
  discovery database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_register_server(
  bt_app_id_type          app_id,
  char*                   srv_name_ptr,
  boolean                 auth )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML REG SRV: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_REGISTER_SRV;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_sml_register_srv.auth = auth;

  if ( OI_OK != bt_pf_copy_ascii_str(
             (char*)(cmd_ptr->cmd_msg.cmd_sml_register_srv.srv_name_str),
             srv_name_ptr, BT_PF_MAX_FILENAME_LEN ) )
  {
    BT_ERR( "BT PF CMD SML REG SRV: invalid srv name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_deregister_client

DESCRIPTION
  Deregisters a OBEX SyncML client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_deregister_client(
  bt_app_id_type    app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML DEREG CLI: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_DEREGISTER_CLI;
  cmd_ptr->cmd_hdr.app_id = app_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_deregister_server

DESCRIPTION
  Deregisters a OBEX SyncML server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_deregister_server(
  bt_app_id_type    app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML DEREG SRV: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_DEREGISTER_SRV;
  cmd_ptr->cmd_hdr.app_id = app_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_auth_request

DESCRIPTION
  Response to a connect indication which reported that the connection was
  unauthorized. If the password supplied by the OBEX client matches the value
  specified here, another connect indication will be issued.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_auth_request(
  bt_app_id_type                app_id,
  bt_pf_sml_conn_id_type        conn_id,
  char*                         password_ptr,
  boolean                       read_only )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    if (password_ptr == NULL) /* It's ok to have no password. */
    {
      cmd_ptr->cmd_msg.cmd_sml_auth_req.password_str[0] = '\0';
    }
    else
    {
      if ( OI_OK != bt_pf_copy_ascii_str(
                      (char*)(cmd_ptr->cmd_msg.cmd_sml_auth_req.password_str),
                      password_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN ) )
      {
        BT_ERR( "BT PF CMD SML AUTH SRV RSP: invalid pwd, App: %x, Stat: %x",
                app_id, OI_OBEX_PASSWORD_TOO_LONG, 0 );
        bt_pf_free_cmd( cmd_ptr );
        return(OI_OBEX_PASSWORD_TOO_LONG);
      }
    }
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_AUTH_REQ;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_sml_auth_req.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_sml_auth_req.read_only = read_only;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD SML AUTH REQ: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_accept_connect

DESCRIPTION
  Command called by upper layer to accept or reject a connect request.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_accept_connect(
  bt_app_id_type                app_id,
  bt_pf_sml_conn_id_type        conn_id,
  boolean                       accept,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_ACCEPT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_sml_accept.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_sml_accept.accept = accept;
    cmd_ptr->cmd_msg.cmd_sml_accept.status = status;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD SML ACCEPT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_auth_response

DESCRIPTION
  This command responds to an authentication indication. A NULL password
  indicates that the application is rejecting the authentication challenge.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_auth_response(
  bt_app_id_type                app_id,
  bt_pf_sml_conn_id_type        conn_id,
  byte*                         user_id_ptr,
  uint8                         user_id_len,
  char*                         password_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (user_id_len > OI_OBEX_MAX_USERID_LEN) ||
       ( (user_id_len > 0) && (user_id_ptr == NULL) ) )
  {
    BT_ERR("BT PF CMD SML AUTH RSP: invalid UID, App: %x",
           app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML AUTH RSP: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (user_id_len > 0)
  {
    memcpy( (void*)cmd_ptr->cmd_msg.cmd_sml_auth_rsp.user_id,
            (void*)user_id_ptr,
            user_id_len );
  }

  if (password_ptr == NULL) /* It's ok to have no password. */
  {
    cmd_ptr->cmd_msg.cmd_sml_auth_rsp.password_str[0] = '\0';
  }
  else
  {
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(cmd_ptr->cmd_msg.cmd_sml_auth_rsp.password_str),
                    password_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN ) )
    {
      BT_ERR( "BT PF CMD SML AUTH RSP: invalid pwd, App: %x, Stat: %x",
              app_id, OI_OBEX_PASSWORD_TOO_LONG, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_OBEX_PASSWORD_TOO_LONG);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_AUTH_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_sml_auth_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_sml_auth_rsp.user_id_len = user_id_len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_connect

DESCRIPTION
  Connects to a OBEX SyncML server on a remote BT device.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel,
  char*                                target_uuid_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_CONNECT;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_sml_connect.channel = channel;
  cmd_ptr->cmd_msg.cmd_sml_connect.bd_addr = *bd_addr_ptr;

  if ( OI_OK != bt_pf_copy_ascii_str(
                  (char*)(cmd_ptr->cmd_msg.cmd_sml_connect.target_uuid_str),
                  target_uuid_ptr, BT_PF_UUID128_SIZE ) )
  {
    BT_ERR( "BT PF CMD SML CON: invalid uuid, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_disconnect

DESCRIPTION
  Disconnects to a OBEX SyncML server on a remote BT device.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_disconnect(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_DISCONNECT;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_sml_disconnect.conn_id = conn_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_get

DESCRIPTION
  Sends a get request to the OBEX SyncML server on a remote BT device.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_get(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  char*                                type_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML GET: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_GET;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_sml_get.conn_id = conn_id;

  if ( OI_OK != bt_pf_copy_ascii_str(
                  (char*)(cmd_ptr->cmd_msg.cmd_sml_get.type_str),
                  type_ptr, BT_PF_MAX_MIME_TYPE_LEN ) )
  {
    BT_ERR( "BT PF CMD SML GET: invalid type, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_put

DESCRIPTION
  Sends a put request to the OBEX SyncML server on a remote BT device.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_put(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  char*                                type_ptr,
  bt_pf_byte_seq_type*                 data_ptr,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML PUT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_PUT;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_sml_put.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_sml_put.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_sml_put.status = status;

  if ( OI_OK != bt_pf_copy_ascii_str(
                  (char*)(cmd_ptr->cmd_msg.cmd_sml_put.type_str),
                  type_ptr, BT_PF_MAX_MIME_TYPE_LEN ) )
  {
    BT_ERR( "BT PF CMD SML PUT: invalid type, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_get_rsp

DESCRIPTION
  Responds to a get request from a remote OBEX SyncML client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_get_rsp(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  bt_pf_byte_seq_type*                 data_ptr,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML GET RSP: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_GET_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_sml_get_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_sml_get_rsp.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_sml_get_rsp.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sml_put_rsp

DESCRIPTION
  Responds to a put request from a remote OBEX SyncML client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sml_put_rsp(
  bt_app_id_type                       app_id,
  bt_pf_sml_conn_id_type               conn_id,
  bt_cmd_status_type                   status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD SML PUT RSP: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_SML_PUT_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_sml_put_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_sml_put_rsp.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

#endif /* FEATURE_BT_EXTPF_SML */

#ifdef FEATURE_BT_EXTPF_CTP
/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_connect

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_connect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD CTP CON: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_CONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_connect.bd_addr = *bd_addr_ptr;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_accept_connection

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_accept_connection (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  boolean                              accept )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD CTP ACPT CON: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_ACCEPT_CONNECTION;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_accept_connection.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_ctp_accept_connection.accept  = accept;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP ACPT CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_disconnect

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_disconnect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD CTP DCN: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_disconnect.bd_addr = *bd_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_clear_call

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_clear_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_ctp_cause_type                 cause )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD CTP CLR CALL: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_CLEAR_CALL;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_clear_call.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_ctp_clear_call.cause   = cause;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP CLR CALL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_establish_call

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_establish_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_ctp_number_type*               calling_number,
  bt_pf_ctp_number_type*               called_number )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL ||
      calling_number == NULL ||
      called_number == NULL )
  {
    BT_ERR("BT PF CMD CTP EST CALL: null argument, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_ESTABLISH_CALL;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_establish_call.bd_addr        = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_ctp_establish_call.calling_number = *calling_number;
    cmd_ptr->cmd_msg.cmd_ctp_establish_call.called_number  = *called_number;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP EST CALL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_info

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_info (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  boolean                              sending_complete,
  char                                 keypad_facility,
  bt_pf_ctp_number_type*               calling_number,
  bt_pf_ctp_number_type*               called_number,
  bt_pf_ctp_audio_control_type         audio_control )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL ||
      calling_number == NULL ||
      called_number == NULL ||
      audio_control == NULL )
  {
    BT_ERR("BT PF CMD CTP INFO: null argument, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_INFO;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_info.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_ctp_info.sending_complete = sending_complete;
    cmd_ptr->cmd_msg.cmd_ctp_info.keypad_facility  = keypad_facility;
    cmd_ptr->cmd_msg.cmd_ctp_info.calling_number   = *calling_number;
    cmd_ptr->cmd_msg.cmd_ctp_info.called_number    = *called_number;
    cmd_ptr->cmd_msg.cmd_ctp_info.audio_control    = audio_control;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP INFO: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_incoming_status

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_incoming_status (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_ctp_call_status_type           status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD CTP INCOMING STAT: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_INCOMING_STATUS;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_incoming_status.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_ctp_incoming_status.status  = status;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP INFO: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_accept_call

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_accept_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD CTP ACPT CALL: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_ACCEPT_CALL;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_accept_call.bd_addr = *bd_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP ACPT CALL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_start_dtmf

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_start_dtmf (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  char                                 tone )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD CTP STRT DTMF: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_START_DTMF;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_start_dtmf.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_ctp_start_dtmf.tone    = tone;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP STRT DTMF: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_stop_dtmf

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_stop_dtmf (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD CTP STP DTMF: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_STOP_DTMF;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_stop_dtmf.bd_addr = *bd_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP STP DTMF: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_register

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_register (
  bt_app_id_type                       app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_REGISTER;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP REG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_deregister

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_deregister (
  bt_app_id_type                       app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_DEREGISTER;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP DEREG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ctp_register_recall

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_ctp_register_recall (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD CTP INFO: null argument, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_CTP_REGISTER_RECALL;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ctp_info.bd_addr = *bd_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD CTP REG RECALL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/* *** */
/* ICP */
/* *** */

/*===========================================================================

FUNCTION
  bt_cmd_pf_icp_connect

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_icp_connect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD ICP: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_ICP_CONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_icp_connect.bd_addr = *bd_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD ICP CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_icp_accept_connection

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_icp_accept_connection (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  boolean                              accept )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD ICP ACPT CON: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_ICP_ACCEPT_CONNECTION;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_icp_accept_connection.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_icp_accept_connection.accept  = accept;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD ICP ACPT CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_icp_disconnect

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_icp_disconnect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD ICP DCN: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_ICP_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_icp_disconnect.bd_addr = *bd_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD ICP: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_icp_clear_call

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_icp_clear_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_icp_cause_type                 cause )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD ICP CLR CALL: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_ICP_CLEAR_CALL;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_icp_clear_call.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_icp_clear_call.cause   = cause;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD ICP CLR CALL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_icp_establish_call

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_icp_establish_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_icp_number_type*               calling_number,
  bt_pf_icp_number_type*               called_number )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD ICP EST CALL: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_ICP_ESTABLISH_CALL;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_icp_establish_call.bd_addr        = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_icp_establish_call.calling_number = *calling_number;
    cmd_ptr->cmd_msg.cmd_icp_establish_call.called_number  = *called_number;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD ICP EST CALL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_icp_incoming_status

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_icp_incoming_status (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  bt_pf_icp_call_status_type           status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD ICP INC STATUS: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_ICP_INCOMING_STATUS;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_icp_incoming_status.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_icp_incoming_status.status  = status;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD ICP INC STATUS: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_icp_accept_call

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_icp_accept_call (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /* check for NULL because we are copying these */
  if (bd_addr_ptr == NULL)
  {
    BT_ERR("BT PF CMD ICP ACPT CALL: null bd_addr, App: %x", app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_ICP_ACCEPT_CALL;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_icp_accept_call.bd_addr = *bd_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD ICP ACPT CALL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_icp_register

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_icp_register (
  bt_app_id_type                       app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_ICP_REGISTER;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD ICP REG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_icp_deregister

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_icp_deregister (
  bt_app_id_type                       app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_ICP_DEREGISTER;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD ICP DEREG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );

}

#endif /* FEATURE_BT_EXTPF_ICP */

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_register

DESCRIPTION
  Registers SIM Access server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_register(
  bt_app_id_type                app_id,
  uint16                        min_frame_size,
  uint16                        max_frame_size,
  uint8*                        req_dst_ptr,
  uint16*                       req_len_ptr,
  const char*                   srv_name_ptr )
{
#ifdef FEATURE_BT_EXTPF_SAP 
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_deregister

DESCRIPTION
  Deregisters SIM Access server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_deregister(
  bt_app_id_type                app_id )
{
#ifdef FEATURE_BT_EXTPF_SAP 
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */ 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_accept_connection

DESCRIPTION
  Accepts/rejects connection request from SIM Access client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_accept_connection(
  bt_app_id_type                app_id,
  boolean                       accept )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */ 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_connect

DESCRIPTION
  Connect to remote SIM Access server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_connect(
  bt_app_id_type                app_id,
  const bt_bd_addr_type*        bd_addr_ptr,
  uint8                         channel,
  uint16                        min_frame_size,
  uint16                        max_frame_size,
  uint8*                        rsp_dst_ptr,
  uint16*                       rsp_len_ptr )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_disconnect

DESCRIPTION
  Brings down connection from to remote device.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_disconnect(
  bt_app_id_type                app_id,
  boolean                       disc_immediately )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_tx_apdu_req

DESCRIPTION
  Sends TRANSFER APDU request to remote SIM Access Server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_tx_apdu_req(
  bt_app_id_type                app_id,
  const uint8*                  req_ptr,
  uint16                        req_len )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_tx_apdu_rsp

DESCRIPTION
  Sends TRANSFER APDU response to remote SIM Access Client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_tx_apdu_rsp(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result,
  const uint8*                  rsp_ptr,
  uint16                        rsp_len )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_tx_atr_req

DESCRIPTION
  Sends TRANSFER ATR request to remote SIM Access Server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_tx_atr_req(
  bt_app_id_type                app_id )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_tx_atr_rsp

DESCRIPTION
  Sends TRANSFER APDU response to remote SIM Access Client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_tx_atr_rsp(
  bt_app_id_type             app_id,
  bt_pf_sap_result_code_type result,
  const uint8*               rsp_ptr,
  uint16                     rsp_len
)
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_power_sim_off

DESCRIPTION
  Requests SIM Access server to power the SIM on.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_power_sim_off(
  bt_app_id_type                app_id )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_power_sim_off_rsp

DESCRIPTION
  Sends result of the client's request to power the SIM off.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_power_sim_off_rsp(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_power_sim_on

DESCRIPTION
  Requests SIM Access server to power the SIM on.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_power_sim_on(
  bt_app_id_type                app_id )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_power_sim_on_rsp

DESCRIPTION
  Sends result of the client's request to power the SIM on.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_power_sim_on_rsp(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_reset_sim

DESCRIPTION
  Requests SIM Access server to reset the SIM.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_reset_sim(
  bt_app_id_type                app_id )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_reset_sim_rsp

DESCRIPTION
  Sends result of the client's request to reset the SIM.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_reset_sim_rsp(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_get_card_reader_status

DESCRIPTION
  Requests status of SIM Access server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_get_card_reader_status(
  bt_app_id_type                app_id )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_tx_card_reader_status

DESCRIPTION
  Sends status of SIM Access server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_tx_card_reader_status(
  bt_app_id_type                app_id,
  bt_pf_sap_result_code_type    result,
  uint8                         status )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_sap_tx_sim_status

DESCRIPTION
  Sends status of the SIM in the SIM Access server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_sap_tx_sim_status(
  bt_app_id_type                app_id,
  bt_pf_sap_sim_status_type     status )
{
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#else
  return BT_CS_GN_UNRECOGNIZED_CMD;
#endif /* FEATURE_BT_EXTPF_SAP */
}

#ifdef FEATURE_BT_EXTPF_AV
/*===========================================================================

FUNCTION
  bt_pf_cmd_avrcp_enable

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avrcp_enable (
  bt_app_id_type                       app_id,
  uint8                                supported_categories
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP ENABLE: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_ENABLE;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avrcp_enable.supported_categories = supported_categories;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_avrcp_connect

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avrcp_connect (
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP CONNECT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_CONNECT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avrcp_connect.bd_addr = *bt_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_avrcp_disconnect

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avrcp_disconnect (
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP DISCONNECT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_avrcp_disable

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avrcp_disable (
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP DISABLE: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_DISABLE;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_avrcp_send_reply

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avrcp_send_reply (
  bt_app_id_type                       app_id,
  uint8                                txn_id,
  uint8                                status,
  bt_event_type                        evt,
  bt_pf_avrcp_rsp_type                 response
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP SEND REPLY: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_SEND_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avrcp_send_reply.txn      = txn_id;
    cmd_ptr->cmd_msg.cmd_avrcp_send_reply.status
      = (bt_pf_avrcp_op_status_type)status;
    cmd_ptr->cmd_msg.cmd_avrcp_send_reply.opcode   = evt;
    cmd_ptr->cmd_msg.cmd_avrcp_send_reply.response = response;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_avrcp_send_generic_reply

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avrcp_send_generic_reply (
  bt_app_id_type                       app_id,
  uint8                                txn_id,
  bt_pf_avrcp_generic_op_type*         frame
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP SEND GENERIC REPLY: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_SEND_GENERIC_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avrcp_send_generic_reply.txn   = txn_id;
    cmd_ptr->cmd_msg.cmd_avrcp_send_generic_reply.frame = *frame;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

#ifdef FEATURE_BT_AVRCP_13

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_metadata_enable

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_metadata_enable (
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP ENABLE: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_METADATA_ENABLE;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_eventid_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_eventid_reply (
  bt_app_id_type                               app_id,
  uint8                                        txn,
  uint8                                        event_count,
  bt_pf_avrcp_metadata_transfer_events_type*   event_ids
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP EVENTID REPLY: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_EVENTID_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_eventid_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_eventid_reply_type.event_count = event_count;
    cmd_ptr->cmd_msg.cmd_eventid_reply_type.event_ids = ( uint8* )event_ids;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_compid_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_compid_reply (
  bt_app_id_type                               app_id,
  uint8                                        txn,
  uint8                                        comp_count,
  uint32*                                      comp_ids
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP COMPID REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_COMPID_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_compid_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_compid_reply_type.comp_count = comp_count;
    cmd_ptr->cmd_msg.cmd_compid_reply_type.comp_ids = comp_ids;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_list_attrid_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_list_attrid_reply (
  bt_app_id_type                               app_id,
  uint8                                        txn,
  uint8                                        attr_count,
  bt_pf_avrcp_player_app_settings_type*        attr_ids
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP LIST ATTRID REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_LIST_ATTRID_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_list_attrid_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_list_attrid_reply_type.attr_count = attr_count;
    cmd_ptr->cmd_msg.cmd_list_attrid_reply_type.attr_ids = ( uint8* )attr_ids;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_list_attrvalue_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_list_attrvalue_reply (
  bt_app_id_type                               app_id,
  uint8                                        txn,
  bt_pf_avrcp_player_app_settings_type         attr_id,
  uint8                                        attr_val_count,
  uint8*                                       attr_settings_vals 
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP LIST ATTRVALUE REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_LIST_ATTRVALUE_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_list_attrvalue_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_list_attrvalue_reply_type.attr_id = ( uint8 )attr_id;
    cmd_ptr->cmd_msg.cmd_list_attrvalue_reply_type.attr_val_count = attr_val_count;
    cmd_ptr->cmd_msg.cmd_list_attrvalue_reply_type.attr_settings_vals = attr_settings_vals;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_get_attr_value_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_get_attr_value_reply (
  bt_app_id_type                               app_id,
  uint8                                        txn,
  uint8                                        attr_id_count,
  bt_pf_avrcp_attr_pair_type                   attrs  
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP GET ATTRVALUE REPLY: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_GET_ATTRVALUE_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_get_attr_value_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_get_attr_value_reply_type.attr_id_count = attr_id_count;
    cmd_ptr->cmd_msg.cmd_get_attr_value_reply_type.attrs = attrs;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_get_attr_setting_text_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_get_attr_setting_text_reply (
  bt_app_id_type                                  app_id,
  uint8                                           txn,
  uint8                                           attr_id_count,
  bt_pf_avrcp_attr_text_type*                     attrs_text
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP GET ATTR SETTING TEXT REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_GET_ATTR_SETTING_TEXT_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_get_attr_setting_text_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_get_attr_setting_text_reply_type.attr_id_count = attr_id_count;
    cmd_ptr->cmd_msg.cmd_get_attr_setting_text_reply_type.attrs_text = attrs_text;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_get_attr_setting_value_text_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_get_attr_setting_value_text_reply (
  bt_app_id_type                              app_id,
  uint8                                       txn,
  uint8                                       attr_val_count,
  bt_pf_avrcp_attr_text_type*                 attrs_val_text
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP GET ATTR SETTING VALUE TEXT REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_GET_ATTR_SETTING_VALUE_TEXT_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_get_attr_setting_value_text_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_get_attr_setting_value_text_reply_type.attr_val_count = attr_val_count;
    cmd_ptr->cmd_msg.cmd_get_attr_setting_value_text_reply_type.attrs_val_text = attrs_val_text;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_play_status_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_play_status_reply (
  bt_app_id_type                              app_id,
  uint8                                       txn,
  bt_pf_avrcp_play_status_type*               play_status
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP PLAY STATUS_REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_PLAY_STATUS_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_play_status_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_play_status_reply_type.play_status = *play_status;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_get_element_attributes_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_get_element_attributes_reply (
  bt_app_id_type                                   app_id,
  uint8                                            txn,
  uint8                                            media_attr_count,
  bt_pf_avrcp_element_attr_type*                   media_element_attr
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP GET ELEMENTS ATTR REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_GET_ELEMENTS_ATTR_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_get_element_attributes_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_get_element_attributes_reply_type.media_attr_count = media_attr_count;
    cmd_ptr->cmd_msg.cmd_get_element_attributes_reply_type.media_element_attr = media_element_attr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_register_notification_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_register_notification_reply (
  bt_app_id_type                              app_id,
  uint8                                       txn,
  bt_pf_avrcp_notif_event_type*               ev_metadata,
  boolean                                     is_final
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP REG NOTIFICATION REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_REG_NOTIFICATION_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_register_notification_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_register_notification_reply_type.ev_metadata = *ev_metadata;
    cmd_ptr->cmd_msg.cmd_register_notification_reply_type.is_final = is_final;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_group_navigation_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_group_navigation_reply (
  bt_app_id_type                              app_id,
  uint8                                       txn,
  uint16                                      vendor_unique_id,
  boolean                                     button_press,
  bt_pf_avrcp_rsp_type                        resp_code  
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP GROUP NAV REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_GROUP_NAV_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_group_navigation_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_group_navigation_reply_type.vendor_unique_id = vendor_unique_id;
    cmd_ptr->cmd_msg.cmd_group_navigation_reply_type.button_press = button_press;
    cmd_ptr->cmd_msg.cmd_group_navigation_reply_type.resp_code = resp_code;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avrcp_send_simple_meta_reply

DESCRIPTION

===========================================================================*/
extern bt_cmd_status_type bt_cmd_pf_avrcp_send_simple_meta_reply (
  bt_app_id_type                              app_id,
  uint8                                       txn,
  uint8                                       pdu_id, 
  bt_cmd_status_type                          status /* This needs to be converted TBD */
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVRCP SIMPLE META REPLY: cmd q full, App=%x", 
            app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVRCP_SIMPLE_META_REPLY;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_send_simple_meta_reply_type.txn   = txn;
    cmd_ptr->cmd_msg.cmd_send_simple_meta_reply_type.pdu_id = pdu_id;
    cmd_ptr->cmd_msg.cmd_send_simple_meta_reply_type.status = status;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  return( cmd_status ); 
}

#endif /* FEATURE_BT_AVRCP_13 */

/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_enable

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_enable(
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP ENABLE: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_ENABLE;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_disable

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_disable(
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP DISABLE: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_DISABLE;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_set_device

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_set_device(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP SET DEVICE: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_SET_DEVICE;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_a2dp_connect.bd_addr = *bt_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_connect

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP CONNECT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_CONNECT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_a2dp_connect.bd_addr = *bt_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_disconnect

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_disconnect(
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP DISCONNECT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_start

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_start(
  bt_app_id_type                       app_id,
  audiosbcenc_data_type*               sbc_data
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP START: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_START;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    if (sbc_data != NULL)
    {
      cmd_ptr->cmd_msg.cmd_a2dp_start.sbc_data = *sbc_data;
    }
    else
    {
      memset ( (void*)&cmd_ptr->cmd_msg.cmd_a2dp_start.sbc_data,
               0, sizeof (cmd_ptr->cmd_msg.cmd_a2dp_start.sbc_data) );
    }
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_suspend

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_suspend(
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP SUSPEND: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_SUSPEND;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_send_data

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_send_data(
  bt_app_id_type                       app_id,
  dsm_item_type*                       data,
  uint32                               seq_num,
  uint32                               timestamps_per_frame,
  uint8                                num_frames,
  uint16                               frame_len,
  boolean                              last_frame,
  uint16                               handle
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if( bt_cmd_l2_get_acl_tx_flow_status( handle ) == FALSE )
  {
    BT_ERR( "BT PF CMD A2DP SEND DATA: TX Q Full, App=%x", app_id, 0, 0);
    cmd_status = BT_CS_PF_A2DP_ERR_TX_Q_FULL;
  }
  else if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP SEND DATA: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_SEND_DATA;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_a2dp_send_data.data       = data;
    cmd_ptr->cmd_msg.cmd_a2dp_send_data.seq_num    = seq_num;
    cmd_ptr->cmd_msg.cmd_a2dp_send_data.num_frames = num_frames;
    cmd_ptr->cmd_msg.cmd_a2dp_send_data.frame_len  = frame_len;
    cmd_ptr->cmd_msg.cmd_a2dp_send_data.timestamps_per_frame = timestamps_per_frame;
    cmd_ptr->cmd_msg.cmd_a2dp_send_data.last_frame = last_frame;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}
/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_ctl

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_ctl(
  bt_app_id_type                       app_id,
  bt_pf_a2dp_ctl_type                  ctl,
  uint32                               data
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP CTL: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_CTL;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_a2dp_ctl.ctl   = ctl;
    cmd_ptr->cmd_msg.cmd_a2dp_ctl.data1 = data;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_a2dp_flush

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_a2dp_flush(
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD A2DP CTL: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
#ifdef FEATURE_BT_TIME_DEBUG
    time_get( cmd_ptr->cmd_hdr.cmd_start_time );
#endif
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_A2DP_FLUSH;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd_avdtp_register

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_register(
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP REGISTER: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_REGISTER;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_deregister

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_deregister(
  bt_app_id_type                       app_id
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP DEREGISTER: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_DEREGISTER;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_register_endpoint

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_register_endpoint(
  bt_app_id_type                       app_id,
  uint8                                media_type,
  uint8                                endpoint_type,
  const bt_pf_avdtp_cap_type*          caps,
  uint8                                num_caps,
  uint8*                               seid
)
{
  int i;
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP REGISTER_ENDPOINT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_REGISTER_ENDPOINT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_register_endpoint.media_type    = media_type;
    cmd_ptr->cmd_msg.cmd_avdtp_register_endpoint.endpoint_type = endpoint_type;
    for( i = 0; i < num_caps && i < BT_PF_AVDTP_MAX_CAPS; i++ )
    {
      cmd_ptr->cmd_msg.cmd_avdtp_register_endpoint.caps[i]       = caps[i];
    }
    cmd_ptr->cmd_msg.cmd_avdtp_register_endpoint.num_caps      = num_caps;
    cmd_ptr->cmd_msg.cmd_avdtp_register_endpoint.seid          = seid;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_deregister_endpoint

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_deregister_endpoint(
  bt_app_id_type                       app_id,
  uint8                                seid
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP DEREGISTER_ENDPOINT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_DEREGISTER_ENDPOINT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_deregister_endpoint.seid = seid;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_connect

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP CONNECT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_CONNECT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_connect.bd_addr = *bt_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_accept

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_accept(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  boolean                              accept
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP ACCEPT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_ACCEPT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_accept.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_avdtp_accept.accept = accept;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_disconnect

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_disconnect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP DISCONNECT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_disconnect.bd_addr = *bt_addr_ptr;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_discover

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_discover(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr,
  uint8*                               transaction
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP DISCOVER: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_DISCOVER;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_discover.bd_addr = *bt_addr_ptr;
    cmd_ptr->cmd_msg.cmd_avdtp_discover.txn = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_get_cap

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_get_cap(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr,
  uint8                                remote_seid,
  uint8*                               transaction
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP GET_CAP: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_GET_CAP;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_get_cap.bd_addr = *bt_addr_ptr;
    cmd_ptr->cmd_msg.cmd_avdtp_get_cap.remote_seid = remote_seid;
    cmd_ptr->cmd_msg.cmd_avdtp_get_cap.txn = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_set_config

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_set_config(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bt_addr_ptr,
  uint8                                remote_seid,
  uint8                                local_seid,
  bt_pf_avdtp_cap_type                 caps[BT_PF_AVDTP_MAX_CAPS],
  uint8                                num_caps,
  uint8*                               transaction,
  uint16*                              handle
)
{
  int i;
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP SET CONFIG: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_SET_CONFIG;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_set_config.bd_addr     = *bt_addr_ptr;
    cmd_ptr->cmd_msg.cmd_avdtp_set_config.remote_seid = remote_seid;
    cmd_ptr->cmd_msg.cmd_avdtp_set_config.local_seid  = local_seid;
    for( i = 0; i < num_caps && i < BT_PF_AVDTP_MAX_CAPS; i++ )
    {
      cmd_ptr->cmd_msg.cmd_avdtp_set_config.caps[i]        = caps[i];
    }
    cmd_ptr->cmd_msg.cmd_avdtp_set_config.num_caps    = num_caps;
    cmd_ptr->cmd_msg.cmd_avdtp_set_config.txn         = transaction;
    cmd_ptr->cmd_msg.cmd_avdtp_set_config.handle      = handle;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_get_config

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_get_config(
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP GET_CONFIG: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_GET_CONFIG;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_get_config.handle = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_get_config.txn    = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_set_config_rsp

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_set_config_rsp(
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8                                transaction,
  uint8                                category,
  bt_cmd_status_type                   error
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP SET_CONFIG_RSP: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_SET_CONFIG_RSP;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_set_config_rsp.handle   = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_set_config_rsp.txn      = transaction;
    cmd_ptr->cmd_msg.cmd_avdtp_set_config_rsp.category = category;
    cmd_ptr->cmd_msg.cmd_avdtp_set_config_rsp.error    = error;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_open

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_open(
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP OPEN: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_OPEN;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_open.handle = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_open.txn    = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_start

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_start(
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP START: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_START;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_start.handle = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_start.txn    = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_close

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_close(
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP CLOSE: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_CLOSE;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_close.handle = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_close.txn    = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_abort

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_abort(
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP ABORT: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_ABORT;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_abort.handle = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_abort.txn    = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_suspend

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_suspend(
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8*                               transaction )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP SUSPEND: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_SUSPEND;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_suspend.handle = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_suspend.txn    = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_reconfigure

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_reconfigure(
  bt_app_id_type                       app_id,
  uint16                               handle,
  bt_pf_avdtp_cap_type                 caps[BT_PF_AVDTP_MAX_CAPS],
  uint8                                num_caps,
  uint8*                               transaction
)
{
  int i;
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP RECONFIGURE: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_RECONFIGURE;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_reconfigure.handle   = handle;
    for( i = 0; i < num_caps && i < BT_PF_AVDTP_MAX_CAPS; i++ )
    {
      cmd_ptr->cmd_msg.cmd_avdtp_reconfigure.caps[i]     = caps[i];
    }
    cmd_ptr->cmd_msg.cmd_avdtp_reconfigure.num_caps = num_caps;
    cmd_ptr->cmd_msg.cmd_avdtp_reconfigure.txn      = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_reconfigure_rsp

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_reconfigure_rsp(
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint8                                transaction,
  uint8                                category,
  bt_cmd_status_type                   error
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP RECONFIGURE_RSP: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_RECONFIGURE_RSP;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_reconfigure_rsp.handle   = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_reconfigure_rsp.txn      = transaction;
    cmd_ptr->cmd_msg.cmd_avdtp_reconfigure_rsp.category = category;
    cmd_ptr->cmd_msg.cmd_avdtp_reconfigure_rsp.error    = error;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_send_data

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_send_data(
  bt_app_id_type                       app_id,
  uint16                               handle,
  uint16                               seq_num,
  uint32                               timestamp,
  uint8                                payload_type,
  uint16                               marker,
  dsm_item_type*                       data,
  void*                                frame_handle
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP SEND_DATA: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_SEND_DATA;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_send_data.handle       = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_send_data.seq_num      = seq_num;
    cmd_ptr->cmd_msg.cmd_avdtp_send_data.timestamp    = timestamp;
    cmd_ptr->cmd_msg.cmd_avdtp_send_data.payload_type = payload_type;
    cmd_ptr->cmd_msg.cmd_avdtp_send_data.marker       = marker;
    cmd_ptr->cmd_msg.cmd_avdtp_send_data.data         = data;
    cmd_ptr->cmd_msg.cmd_avdtp_send_data.frame_handle = frame_handle;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_security_control

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_security_control(
  bt_app_id_type                       app_id,
  uint16                               handle,
  dsm_item_type*                       data,
  uint8*                               transaction
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP SECURITY_CONTROL: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_SECURITY_CONTROL;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_security_control.handle  = handle;
    cmd_ptr->cmd_msg.cmd_avdtp_security_control.data    = data;
    cmd_ptr->cmd_msg.cmd_avdtp_security_control.txn     = transaction;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_avdtp_security_control_rsp

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_avdtp_security_control_rsp(
  bt_app_id_type                       app_id,
  uint8                                transaction,
  dsm_item_type*                       data,
  bt_cmd_status_type                   error
)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD AVDTP SECURITY_CONTROL_RSP: cmd q full, App=%x", app_id, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_SECURITY_CONTROL_RSP;
    cmd_ptr->cmd_hdr.app_id   = app_id;
    cmd_ptr->cmd_msg.cmd_avdtp_security_control_rsp.txn     = transaction;
    cmd_ptr->cmd_msg.cmd_avdtp_security_control_rsp.data    = data;
    cmd_ptr->cmd_msg.cmd_avdtp_security_control_rsp.error   = error;
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_PBAP
/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_connect

DESCRIPTION
  Connects to a remote PBAP Server

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if (bd_addr_ptr == NULL)
  {
    BT_ERR( "BT PF CMD PBAP CLI CON: NULL ptr, App: %x, Stat: &x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_CONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_pbap_cli_connect.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_pbap_cli_connect.channel = channel;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD PBAP CLI CON: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_disconnect

DESCRIPTION
  Disconnect from a remote PBAP Server

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_disconnect(
  bt_app_id_type                       app_id,
  bt_pf_pbap_cli_conn_id_type          conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_DISCONNECT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_pbap_cli_disconnect.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD PBAP CLI DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_register

DESCRIPTION
  Initialize the PBAP Client and register it in the service
  discovery database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_register(
  bt_app_id_type                app_id,
  char*                         cli_name_str)
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP CLI REG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_ascii_str(
             (char*)(cmd_ptr->cmd_msg.cmd_pbap_cli_register.cli_name_str),
             cli_name_str, BT_PF_MAX_FILENAME_LEN ) )
  {
    BT_ERR( "BT PF CMD PBAP CLI REG: invalid srv name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_REGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_deregister

DESCRIPTION
  Deregisters a PBAP Client

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_deregister(
  bt_app_id_type    app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP DEREG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_DEREGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_abort

DESCRIPTION
  Terminates the current operation.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_abort(
  bt_app_id_type               app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id )
{
  bt_pf_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_ABORT;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_pbap_cli_abort.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD PBAP CLI ABORT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_auth_response

DESCRIPTION
  This function is called by the application in response to a password
  indication to provide a password for OBEX authentication.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_auth_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id,
  byte*                         user_id_ptr,
  uint8                         user_id_len,
  char*                         pin_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (user_id_len > OI_OBEX_MAX_USERID_LEN) ||
       ( (user_id_len > 0) && (user_id_ptr == NULL) ) )
  {
    BT_ERR("BT PF CMD PBAP CLI AUTH: invalid UID, App: %x",
           app_id, 0, 0);
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP CLI AUTH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (user_id_len > 0)
  {
    memcpy( (void*)cmd_ptr->cmd_msg.cmd_pbap_cli_auth_rsp.user_id,
            (void*)user_id_ptr,
            user_id_len );
  }

  if (pin_ptr == NULL) /* It's ok to have no password. */
  {
    cmd_ptr->cmd_msg.cmd_pbap_cli_auth_rsp.pin_str[0] = '\0';
  }
  else
  {
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(cmd_ptr->cmd_msg.cmd_pbap_cli_auth_rsp.pin_str),
                    pin_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN ) )
    {
      BT_ERR( "BT PF CMD PBAP CLI AUTH: invalid pwd, App: %x, Stat: %x",
              app_id, OI_OBEX_PASSWORD_TOO_LONG, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_OBEX_PASSWORD_TOO_LONG);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_AUTH_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_auth_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_auth_rsp.user_id_len = user_id_len;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return(cmd_status);
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_pull_phone_book

DESCRIPTION
  Sends a PullPhoneBook request to a PBAP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_pull_phone_book(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id,
  uint16*                       name_ptr,
  uint64*                       attribute_mask_ptr,
  uint8*                        format_ptr,
  uint16*                       max_list_count_ptr,
  uint16*                       list_start_offset_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint8               name_len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF PBAP CLI PL PN BK:: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if( OI_OK != bt_pf_copy_unicode_str(
                 cmd_ptr->cmd_msg.cmd_pbap_cli_pull_phone_book.name_str,
                 name_ptr, BT_PF_MAX_FILENAME_LEN,
                 &name_len ) )
  {
    BT_ERR( "BT PF PBAP CLI PL PN BK: invalid name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_PULL_PHONE_BOOK;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_phone_book.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_phone_book.attribute_mask_ptr =  
    attribute_mask_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_phone_book.format_ptr = format_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_phone_book.name_len = name_len;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_phone_book.max_list_count_ptr = 
    max_list_count_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_phone_book.list_start_offset_ptr = 
    list_start_offset_ptr;

  cmd_status = bt_pf_cmd( cmd_ptr );
 
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_set_phone_book

DESCRIPTION
  Sends a SetPhoneBook request to a PBAP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_set_phone_book(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id,
  uint16*                       folder_ptr,
  bt_pf_pbap_cli_set_path_type  set_path_ctrl )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint8               folder_len = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF PBAP CLI S PB BK:: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( folder_ptr != NULL ) 
  {
    if( OI_OK != bt_pf_copy_unicode_str(
                   cmd_ptr->cmd_msg.cmd_pbap_cli_set_phone_book.folder_str,
                   folder_ptr, BT_PF_MAX_FILENAME_LEN,
                   &folder_len ) )
    {
      BT_ERR( "BT PF PBAP CLI S PB BK: invalid name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_SET_PHONE_BOOK;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_set_phone_book.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_set_phone_book.folder_len = folder_len;  
  cmd_ptr->cmd_msg.cmd_pbap_cli_set_phone_book.set_path_ctrl = set_path_ctrl;

  cmd_status = bt_pf_cmd( cmd_ptr );
 
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_pull_vcard_listing

DESCRIPTION
  Sends a PullPhoneBook request to a PBAP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_pull_vcard_listing(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id,
  uint16*                       name_ptr,
  uint8*                        order_ptr,
  char*                         value_ptr,
  uint8*                        search_attrib_ptr,
  uint16*                       max_list_count_ptr,
  uint16*                       list_start_offset_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint8               name_len   = 0;
  uint8               value_len  = 0;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF PBAP CLI PL VL: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( name_ptr == NULL ) 
  {
    cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.name_str[0] = '\0';
  }
  else
  {
    if( OI_OK != bt_pf_copy_unicode_str(
                   cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.name_str,
                   name_ptr, BT_PF_MAX_FILENAME_LEN,
                   &name_len ) )
    {
      BT_ERR( "BT PF PBAP CLI PL VL: invalid name, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
  }

  if ( value_ptr == NULL ) 
  {
    cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.value_str[0] = '\0';
  }
  else
  {
    if( OI_OK != bt_pf_copy_ascii_str(
                   (char*)cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.value_str,
                   value_ptr, 
                   BT_PF_MAX_MIME_TYPE_LEN ) )
    {
      BT_ERR( "BT PF PBAP CLI PL VL: invalid value, App: %x, Stat: %x",
              app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_STATUS_INVALID_PARAMETERS);
    }
    
    value_len = 
      strlen((char*)cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.value_str);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_PULL_VCARD_LISTING;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.name_len = name_len;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.value_len = value_len;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.order_ptr = order_ptr; 
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.search_attrib_ptr = 
    search_attrib_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.max_list_count_ptr = 
    max_list_count_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list.list_start_offset_ptr = 
    list_start_offset_ptr;

  cmd_status = bt_pf_cmd( cmd_ptr );
 
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_cli_pull_vcard_entry

DESCRIPTION
  Sends a PullPhoneBook request to a PBAP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_cli_pull_vcard_entry(
  bt_app_id_type                app_id,
  bt_pf_pbap_cli_conn_id_type   conn_id,
  uint16*                       name_ptr,
  uint64*                       attribute_mask_ptr,
  uint8*                        format_ptr )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint8               name_len;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF PBAP CLI PL VE:: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if( OI_OK != bt_pf_copy_unicode_str(
                 cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_entry.name_str,
                 name_ptr, BT_PF_MAX_FILENAME_LEN,
                 &name_len ) )
  {
    BT_ERR( "BT PF PBAP CLI PL VE: invalid name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_CLI_PULL_VCARD_ENTRY;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_entry.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_entry.attribute_mask_ptr =  
    attribute_mask_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_entry.format_ptr = format_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_entry.name_len = name_len;

  cmd_status = bt_pf_cmd( cmd_ptr );
 
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_srv_register

DESCRIPTION
  Initialize the PBAP Server and register it in the service
  discovery database.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_srv_register(
  bt_app_id_type                app_id,
  char*                         srv_name_str,
  uint8                         supported_repositories,
  bt_pf_goep_srv_auth_type      auth )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP SRV REG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if ( OI_OK != bt_pf_copy_ascii_str(
             (char*)(cmd_ptr->cmd_msg.cmd_pbap_srv_register.srv_name_str),
             srv_name_str, BT_PF_MAX_FILENAME_LEN ) )
  {
    BT_ERR( "BT PF CMD PBAP SRV REG: invalid srv name, App: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    bt_pf_free_cmd( cmd_ptr );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_SRV_REGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_register.supported_repositories =
    supported_repositories;
  cmd_ptr->cmd_msg.cmd_pbap_srv_register.auth = auth;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_srv_deregister

DESCRIPTION
  Deregisters a PBAP Server

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_srv_deregister(
  bt_app_id_type    app_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP DEREG: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_SRV_DEREGISTER;
  cmd_ptr->cmd_hdr.app_id = app_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_srv_auth_response

DESCRIPTION
  This function is called by the application in response to a connnection
  indication that requires authentication.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_srv_auth_response(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  char*                         pin_ptr )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP SRV AUTH: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  if (pin_ptr == NULL) /* It's ok to have no password. */
  {
    cmd_ptr->cmd_msg.cmd_pbap_srv_auth_rsp.pin_str[0] = '\0';
  }
  else
  {
    if ( OI_OK != bt_pf_copy_ascii_str(
                    (char*)(cmd_ptr->cmd_msg.cmd_pbap_srv_auth_rsp.pin_str),
                    pin_ptr, BT_PF_OBEX_MAX_PASSWORD_LEN ) )
    {
      BT_ERR( "BT PF CMD PBAP SRV AUTH: invalid pwd, App: %x, Stat: %x",
              app_id, OI_OBEX_PASSWORD_TOO_LONG, 0 );
      bt_pf_free_cmd( cmd_ptr );
      return(OI_OBEX_PASSWORD_TOO_LONG);
    }
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_SRV_AUTH_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_auth_rsp.conn_id = conn_id;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return(cmd_status);
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_srv_accept_connect

DESCRIPTION
  PBAP server must call this function to accept or reject a connect
  request from a client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_srv_accept_connect(
  bt_app_id_type              app_id,
  bt_pf_pbap_srv_conn_id_type conn_id,
  boolean                     accept )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP SRV ACCEPT: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_SRV_ACCEPT;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_accept.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_accept.accept = accept;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_srv_force_disconnect

DESCRIPTION
  Forcibly severs the connection from an PBAP client to the PBAP server.
  This function should be called for all active connections before
  terminating the PBAP server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_srv_force_disconnect(
  bt_app_id_type              app_id,
  bt_pf_pbap_srv_conn_id_type conn_id )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_SRV_FORCE_DCN;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_pbap_srv_force_dcn.conn_id = conn_id;

    cmd_status = bt_pf_cmd( cmd_ptr );
  }
  else
  {
    BT_ERR( "BT PF CMD PBAP SRV DCN: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
  }
  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_srv_pull_phone_book_response

DESCRIPTION
  Responds to a PullPhoneBook request from a PBAP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_srv_pull_phone_book_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  bt_pf_byte_seq_type*          data_ptr,
  uint16*                       phone_book_size_ptr,
  uint8*                        num_missed_calls_ptr,
  boolean                       final,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP SRV PL PB RSP: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_SRV_PULL_PHONE_BOOK_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_phone_book_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_phone_book_rsp.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_phone_book_rsp.phone_book_size_ptr = 
    phone_book_size_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_phone_book_rsp.num_missed_calls_ptr = 
    num_missed_calls_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_phone_book_rsp.final = final;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_phone_book_rsp.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_srv_set_phone_book_response

DESCRIPTION
  Responds to a SetPhoneBook request from a PBAP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_srv_set_phone_book_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP SRV S PB RSP: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_SRV_SET_PHONE_BOOK_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_set_phone_book_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_set_phone_book_rsp.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_srv_pull_vcard_listing_response

DESCRIPTION
  Responds to a PullvCardListing request from a PBAP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_srv_pull_vcard_listing_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  bt_pf_byte_seq_type*          data_ptr,
  uint16*                       phone_book_size_ptr,
  uint8*                        num_missed_calls_ptr,
  boolean                       final,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP SRV PL VL RSP: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_SRV_PULL_VCARD_LISTING_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_list_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_list_rsp.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_list_rsp.phone_book_size_ptr = 
    phone_book_size_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_list_rsp.num_missed_calls_ptr = 
    num_missed_calls_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_list_rsp.final = final;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_list_rsp.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_pbap_srv_pull_vcard_entry_response

DESCRIPTION
  Responds to a PullvCardEntry request from a PBAP client.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_pbap_srv_pull_vcard_entry_response(
  bt_app_id_type                app_id,
  bt_pf_pbap_srv_conn_id_type   conn_id,
  bt_pf_byte_seq_type*          data_ptr,
  boolean                       final,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
  {
    BT_ERR( "BT PF CMD PBAP SRV PL VE RSP: cmd q full, cmd dropped, App: %x",
            app_id, 0, 0 );
    return(BT_CS_GN_CMD_Q_FULL);
  }

  cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_PBAP_SRV_PULL_VCARD_ENTRY_RSP;
  cmd_ptr->cmd_hdr.app_id = app_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_entry_rsp.conn_id = conn_id;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_entry_rsp.data_ptr = data_ptr;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_entry_rsp.final = final;
  cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_entry_rsp.status = status;

  cmd_status = bt_pf_cmd( cmd_ptr );

  return( cmd_status );
}

#endif /* FEATURE_BT_EXTPF_PBAP */

#ifdef FEATURE_BT_EXTPF_HID_HOST 
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#endif /* FEATURE_BT_EXTPF */
#endif /* FEATURE_BT */



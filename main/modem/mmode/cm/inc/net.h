#ifndef NET_H
#define NET_H
/*===========================================================================

              NET  H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions that are required for defining NETWORK
  SYSTEM and PROFILES in a NET FILE.

EXTERNALIZED FUNCTIONS

  net_get_sw_ver

  net_file_read
  net_file_write
  net_file_read2
  net_file_write2

  net_validate

  net_parser_make
  net_parser_init
  net_set_file_id

  net_get_max_allowed_file_size
  net_get_file_len

  net_extract_buf_ptr

  net_get_max_allowed_sys
  net_get_max_allowed_profile
  net_get_num_sys
  net_get_num_profile

  net_get_sys_rec
  net_get_profile
  net_get_wlan_sys_rec

  net_insert_sys_rec
  net_insert_profile
  net_insert_sys_rec_profile
  net_insert_wlan_sys_rec

  net_insert_wlan_sys_rec_profile

  net_del_sys_rec
  net_del_profile

  net_modify_sys_rec
  net_modify_profile
  net_modify_wlan_sys_rec

  net_pack_profile
  net_unpack_profile


Copyright (c) 2004 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/net.h#1 $


when       who    what, where, why
--------   ---    ----------------------------------------------------------
06/04/09   mh     Remoted NET APIs
03/09/09   rm     Reverting the fix -
                  Adding FEATURE_MM_SUPERSET for some features
01/23/09   rm     Adding FEATURE_MM_SUPERSET for some features
07/03/07   dm/ic  Corrected meta comments for wep_104_bit_info
06/12/07   tbh    Fixed ADS1.2 compiler warning: unsigned comparison with 0
06/11/07   tbh    Moved the new fields for WLAN handoff & WLAN security after 
                  SSID field in the system record structure.
06/08/07   tbh    Added new server id field for PEAP, TLS authentication types. 
                  Also modified existing system record structure with new fields
                  for WLAN handoff & WLAN security
03/09/07   sk     RAM usage reduction.
02/23/07   ic     Corrected meta comments for net_wlan_auth_info_u_type and 
                  net_wlan_profile_s_type
02/06/06   dk     Added support for PAP,CHAP,MsCHAPv2,PEAP,TTLS.
12/05/05   ic     Fixed ADS1.2 compiler warnings unsigned comparison with 0
                  is always true: '>='
10/08/05   ic     Added Header: field
06/03/05   dk     Increased the password size to 63.
05/16/05   ic     Added meta comments for net_wlan_auth_info_u_type
05/13/05   ic     Added meta comments for net_profile_u_type
05/12/05   ic     Removed FEATURE_HTORPC_METACOMMENTS flag from around
                  fields in net_wlan_auth_info_u_type
05/11/05   ic     Added metacomments under FEATURE_HTORPC_METACOMMENTS
04/27/05   dk     Added TLS Certificate support.
02/17/05   ka     Removing duplicate declaration of net_get_wlan_sys_rec()
10/29/04   dk     Initial version.
===========================================================================*/
#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif
#include "sys.h"

#include "sys_wlan.h" /* wlan specific declarations */

/*===========================================================================

                     NET FILE DEFINES

===========================================================================*/

/* MAX File size supported
*/
#define NET_LIST_FILE_MAX_SIZE         4096

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* MAX File size for manual system
*/
#ifdef FEATURE_WLAN
#error code not present
#else
#define NET_MANUAL_SYS_FILE_SIZE       1
#endif


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* MAX SSID Size
*/

#define NET_WLAN_SSID_MAX_SIZE         SYS_WLAN_SSID_MAX_SIZE

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* WEP Key Sizes
*/

#define NET_40_BIT_WEP_KEY_SIZE        5

#define NET_104_BIT_WEP_KEY_SIZE       13

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define NET_MANUAL_SYS_ID              0

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* USER ID MAX Size
*/

#define NET_USER_ID_MAX_SIZE           72

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* User password MAX Size
*/

#define NET_USER_PASSWD_MAX_SIZE       63

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Server name MAX Size
*/

#define NET_SERVER_NAME_MAX_SIZE       72


/*===========================================================================

                     NET FILE ENUMS

===========================================================================*/

/* Type of NET FILE
*/
typedef enum{

  NET_TYPE_PRIVATE_LIST  = 0,
  NET_TYPE_MANUAL_SYS    = 1,

  /* For Internal Use Only! */
  NET_TYPE_MAX

} net_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Authenticaton Type associated with a particular profile.
*/

typedef enum{

  NET_AUTH_TYPE_NONE                   = -1,
    /* For Internal Use Only! */

  NET_AUTH_TYPE_NO_AUTH                = 0,
  NET_AUTH_TYPE_WEP                    = 1,
  NET_AUTH_TYPE_8021X_PASSWD           = 2,
  NET_AUTH_TYPE_8021X_TLS_RADIUS       = 3,
  NET_AUTH_TYPE_8021X_MD5_RADIUS       = 4,
  NET_AUTH_TYPE_8021X_TLS_RADIUS_WKEY  = 5,
  NET_AUTH_TYPE_8021X_TLS_CERT         = 6,
  NET_AUTH_TYPE_PAP                    = 7,
  NET_AUTH_TYPE_CHAP                   = 8,
  NET_AUTH_TYPE_MsCHAPv2               = 9,
  NET_AUTH_TYPE_PEAP                   = 10,
  NET_AUTH_TYPE_TTLS                   = 11,
  NET_AUTH_TYPE_SIM                    = 12,
  NET_AUTH_TYPE_AKA                    = 13,

  NET_AUTH_TYPE_MAX
    /* For Internal Use Only! */

} net_auth_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Encryption type associated with a particular profile
*/
typedef enum{

  NET_ENCRY_TYPE_NONE            = -1,
   /* For Internal Use Only! */

  NET_ENCRY_TYPE_NO_ENCRY        = 0,
  NET_ENCRY_TYPE_WEP_40_BIT      = 1,
  NET_ENCRY_TYPE_WEP_104_BIT     = 2,
  NET_ENCRY_TYPE_TKIP            = 3,
  NET_ENCRY_TYPE_AES             = 4,

  NET_ENCRY_TYPE_MAX
   /* For Internal Use Only! */

} net_encry_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* WLAN Technology
*/

typedef enum{

  NET_WLAN_TECH_NONE             = 0,

  NET_WLAN_TECH_A                = 1,
  NET_WLAN_TECH_B                = 2,
  NET_WLAN_TECH_G_ONLY           = 4,
  NET_WLAN_TECH_GB               = 6,

  NET_WLAN_TECH_ANY              = 0x07

} net_wlan_tech_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* WLAN Bands
*/

typedef enum{

  NET_WLAN_BAND_NONE             = -1,
    /* For internal use of net.c only */

  NET_WLAN_BAND_US_2400          = 0,
  NET_WLAN_BAND_EUROPE_2400      = 1,
  NET_WLAN_BAND_FRANCE_2400      = 2,
  NET_WLAN_BAND_SPAIN_2400       = 3,
  NET_WLAN_BAND_JAPAN_2400       = 4,
  NET_WLAN_BAND_US_5000          = 5,
  NET_WLAN_BAND_EUROPE_5000      = 6,
  NET_WLAN_BAND_FRANCE_5000      = 7,
  NET_WLAN_BAND_SPAIN_5000       = 8,
  NET_WLAN_BAND_JAPAN_5000       = 9,

  NET_WLAN_BAND_MAX,
    /* For internal use of net.c only */

  NET_WLAN_BAND_ANY              = 0xFF

} net_wlan_band_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Type fo WLAN System: AD-HOC or Infrastructure or Any
*/

typedef enum{

  NET_WLAN_MODE_ADHOC            = 1,
  NET_WLAN_MODE_INFRA            = 2,
  NET_WLAN_MODE_ANY              = 3

} net_wlan_mode_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* System Record Type
*/

typedef enum{

  /* For internal Use only! */
  NET_REC_TYPE_NONE              = 0,

  NET_REC_TYPE_WLAN              = 1,

  /* For internal Use only! */
  NET_REC_TYPE_MAX

} net_rec_e_type;


typedef enum{
  /* For Internal Use Only! */
  NET_WLAN_SEC_TYPE_NONE               = 0,
    
  NET_WLAN_SEC_TYPE_LEGACY             = 1,

  NET_WLAN_SEC_TYPE_WPA_OR_WPA2        = 2,

  NET_WLAN_SEC_TYPE_WPA2_ONLY          = 3,

  NET_WLAN_SEC_TYPE_MAX

}net_wlan_sec_mode_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Profile Type
*/

typedef enum{

  /* For internal Use only! */
  NET_PROFILE_TYPE_NONE          = 0,

  NET_PROFILE_TYPE_WLAN          = 1,

  /* For internal Use only! */
  NET_PROFILE_TYPE_MAX

} net_profile_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* WLAN Channels
*/

typedef enum{
  NET_CHAN_WLAN_2400_1             = SYS_BM(0),
  NET_CHAN_WLAN_2400_2             = SYS_BM(1),
  NET_CHAN_WLAN_2400_3             = SYS_BM(2),
  NET_CHAN_WLAN_2400_4             = SYS_BM(3),
  NET_CHAN_WLAN_2400_5             = SYS_BM(4),
  NET_CHAN_WLAN_2400_6             = SYS_BM(5),
  NET_CHAN_WLAN_2400_7             = SYS_BM(6),
  NET_CHAN_WLAN_2400_8             = SYS_BM(7),
  NET_CHAN_WLAN_2400_9             = SYS_BM(8),
  NET_CHAN_WLAN_2400_10            = SYS_BM(9),
  NET_CHAN_WLAN_2400_11            = SYS_BM(10),
  NET_CHAN_WLAN_2400_12            = SYS_BM(11),
  NET_CHAN_WLAN_2400_13            = SYS_BM(12),
  NET_CHAN_WLAN_2400_14            = SYS_BM(13),

  NET_CHAN_WLAN_2400_ANY           = 0xFFFF
} net_chan_wlan_2400_e_type;

typedef enum{
  NET_CHAN_WLAN_5000_36            = SYS_BM(0),
  NET_CHAN_WLAN_5000_40            = SYS_BM(1),
  NET_CHAN_WLAN_5000_44            = SYS_BM(2),
  NET_CHAN_WLAN_5000_48            = SYS_BM(3),
  NET_CHAN_WLAN_5000_52            = SYS_BM(4),
  NET_CHAN_WLAN_5000_56            = SYS_BM(5),
  NET_CHAN_WLAN_5000_60            = SYS_BM(6),
  NET_CHAN_WLAN_5000_64            = SYS_BM(7),
  NET_CHAN_WLAN_5000_149           = SYS_BM(8),
  NET_CHAN_WLAN_5000_153           = SYS_BM(9),
  NET_CHAN_WLAN_5000_157           = SYS_BM(10),
  NET_CHAN_WLAN_5000_161           = SYS_BM(11),

  NET_CHAN_WLAN_5000_ANY                = 0xFFFF

} net_chan_wlan_5000_e_type;



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* NET VALIDATION Enumerations
*/

typedef enum{

  NET_STATUS_SUCCESS                 = 0,

  NET_STATUS_GENERAL_FAIL            = 1,

  /* Parser Validation Enums
  */
  NET_STATUS_PARSER_VER_MISMATCH     = 100,
  NET_STATUS_PARSER_BUF_SIZE         = 101,
  NET_STATUS_PARSER_NUM_SYS          = 102,
  NET_STATUS_PARSER_NUM_PROFILE      = 103,
  NET_STATUS_PARSER_MAKE             = 104,
  NET_STATUS_PARSER_INVALID_SYS_REC  = 105,
  NET_STATUS_PARSER_INVALID_PROFILE  = 106,

  /* Sys Rec Status Enums
  */
  NET_STATUS_SYS_INVALID_ID          = 200,
  NET_STATUS_SYS_NO_SPACE            = 201,
  NET_STATUS_SYS_UNKNOWN_TYPE        = 202,
  NET_STATUS_MAX_SYS                 = 203,
  NET_STATUS_SYS_WLAN_TECH           = 204,
  NET_STATUS_SYS_WLAN_BAND           = 205,
  NET_STATUS_SYS_WLAN_CHAN           = 206,
  NET_STATUS_SYS_WLAN_MODE           = 207,
  NET_STATUS_SYS_WLAN_SSID           = 208,
  NET_STATUS_SYS_HO_ALLOWED          = 209,
  NET_STATUS_SYS_HO_HIDDEN           = 210,
  NET_STATUS_SYS_WLAN_SEC_TYPE       = 211,

  /* Profile Status Enums
  */
  NET_STATUS_PROFILE_UNKNOWN_TYPE    = 300,
  NET_STATUS_PROFILE_NO_SPACE        = 301,
  NET_STATUS_MAX_PROFILES            = 302,
  NET_STATUS_PROFILE_UNKNOWN         = 303,
  NET_STATUS_PROFILE_INVALID_ID      = 304,
  NET_STATUS_PROFILE_ENCRYP          = 305,
  NET_STATUS_PROFILE_AUTH            = 305,
  NET_STATUS_PROFILE_ATUH_ENCRYP_MIS = 306,
  NET_STATUS_PROFILE_AUTH_INFO       = 307,

  /* File Status Enums
  */
  NET_STATUS_FILE_PARSE_FAIL         = 400,

  /* For Internal Use Only! */
  NET_STATUS_MAX

} net_status_e_type;


/*===========================================================================

                     SYSTEM RECORD DEFINITION

===========================================================================*/

/* WLAN SSSID.
*/

typedef struct {

  byte       size;

  byte       ssid[NET_WLAN_SSID_MAX_SIZE];

} net_wlan_ssid_s_type;
/*~ FIELD net_wlan_ssid_s_type.ssid VARRAY NET_WLAN_SSID_MAX_SIZE
    LENGTH net_wlan_ssid_s_type.size */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* WLAN Record Type as stored in NET file.
*/

typedef struct {

  /* The WLAN Technology */
  byte                                             tech;

  /* The WLAN Band */
  byte                                             band;

  /* The WLAN channel to acquire */
  /* Channel 0 mean any channel  */
  word                                             chan;

  /* Groups this record belongs to. */
  word                                             groups;

  /* WLAN mode such as AD-HOC,INFRA */
  byte                                             mode;

  /* Profile Id */
  byte                                             profile_id;

  /* The WLAN SSID */
  net_wlan_ssid_s_type                             ssid;

  /* Whether Handoff is permitted on the current SSID */
  byte                                             is_handoff_permitted;

  /* Whether the current SSID is hidden */
  byte                                             is_hidden_ssid;

  /* WLAN security mode such as LEGACY, WPA, WPA2 */
  byte                                             wlan_sec_mode;

} net_wlan_record_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* System Record in NET .
*/

typedef union {

  net_wlan_record_s_type                           wlan;

} net_sys_rec_u_type;
/*~ CASE NET_REC_TYPE_NONE net_sys_rec_u_type.void */
/*~ CASE NET_REC_TYPE_WLAN net_sys_rec_u_type.wlan */

typedef struct {

  /* System Record Length*/
  byte                                             len;

  /* System Record Type */
  byte                                             type;

  /* System Record */
  net_sys_rec_u_type                               sys_rec;

} net_sys_rec_s_type;
/*~ FIELD net_sys_rec_s_type.sys_rec DISC net_sys_rec_s_type.type */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* WLAN Record using SYS.H types.
*/

typedef struct {

  /* The WLAN Technology */
  sys_tech_mask_type                               tech;

  /* The WLAN Band */
  sys_band_mask_type                               band;

  /* The WLAN channel to acquire */
  /* Channel 0 mean any channel  */
  sys_chan_type                                    chan;

  /* Groups this record belongs to. */
  word                                             groups;

  /* WLAN mode such as AD-HOC,INFRA */
  sys_wlan_bss_e_type                              mode;

  /* Profile Id */
  sys_profile_id_type                              profile_id;

  /* The WLAN SSID */
  sys_wlan_ssid_s_type                             ssid;

  /* WLAN misc fields */
  sys_wlan_misc_params_s_type                      wlan_misc_params;

} net_sys_wlan_record_s_type;

/*===========================================================================

                     PROFILE DEFINITION

===========================================================================*/

/* 40 Bit (a.k.a. 64 bit) WEP Key Storage.
*/

typedef struct {

  /* Default WEP Key Id */
  byte                                        default_key;

  /* WEP Key 1 */
  byte                                        key1[NET_40_BIT_WEP_KEY_SIZE];

  /* WEP Key 2 */
  byte                                        key2[NET_40_BIT_WEP_KEY_SIZE];

  /* WEP Key 3 */
  byte                                        key3[NET_40_BIT_WEP_KEY_SIZE];

  /* WEP Key 4 */
  byte                                        key4[NET_40_BIT_WEP_KEY_SIZE];

} net_wlan_40_bit_wep_info_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* 104 Bit (a.k.a. 128 bit) WEP Key Storage.
*/

typedef struct {

  /* Default WEP Key Id */
  byte                                        default_key;

  /* WEP Key 1 */
  byte                                        key1[NET_104_BIT_WEP_KEY_SIZE];

  /* WEP Key 2 */
  byte                                        key2[NET_104_BIT_WEP_KEY_SIZE];

  /* WEP Key 3 */
  byte                                        key3[NET_104_BIT_WEP_KEY_SIZE];

  /* WEP Key 4 */
  byte                                        key4[NET_104_BIT_WEP_KEY_SIZE];

} net_wlan_104_bit_wep_info_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

typedef struct {

  /* ID Length */
  byte                                        id_len;

  /* ID */
  byte                                        id[NET_USER_ID_MAX_SIZE];

} net_profile_id_s_type;
/*~ FIELD net_profile_id_s_type.id VARRAY NET_USER_ID_MAX_SIZE 
    LENGTH net_profile_id_s_type.id_len */

typedef struct {

  /* Password Length */
  byte                                        pass_len;

  /* Password */
  byte                                        pass[NET_USER_PASSWD_MAX_SIZE];

} net_profile_pass_s_type;
/*~ FIELD net_profile_pass_s_type.pass VARRAY NET_USER_PASSWD_MAX_SIZE
    LENGTH net_profile_pass_s_type.pass_len */

typedef struct {

  /* ID Length */
  byte                                        id_len;

  /* ID */
  byte                                        id[NET_USER_ID_MAX_SIZE];

  /* Password/Shared Secret Length */
  byte                                        pass_len;

  /* Password/Shared Secret */
  byte                                        pass[NET_USER_PASSWD_MAX_SIZE];

} net_profile_id_pass_s_type;
/*~ FIELD net_profile_id_pass_s_type.id VARRAY NET_USER_ID_MAX_SIZE
    LENGTH net_profile_id_pass_s_type.id_len */
/*~ FIELD net_profile_id_pass_s_type.pass VARRAY NET_USER_PASSWD_MAX_SIZE
    LENGTH net_profile_id_pass_s_type.pass_len */

typedef struct {

  /* Server name length */
  byte                                        server_name_len;

  /* Server name */
  byte                                        server_name[NET_SERVER_NAME_MAX_SIZE];

} net_profile_server_id_s_type;
/*~ FIELD net_profile_server_id_s_type.server_name VARRAY NET_SERVER_NAME_MAX_SIZE
    LENGTH net_profile_server_id_s_type.server_name_len */

typedef struct {

  /* ID Length */
  byte                                        id_len;

  /* ID */
  byte                                        id[NET_USER_ID_MAX_SIZE];

  /* TLS Certificate Id */
  byte                                        cert_id;

  /* server ID */
  net_profile_server_id_s_type                server_id;

} net_profile_id_cert_s_type;
/*~ FIELD net_profile_id_cert_s_type.id VARRAY NET_USER_ID_MAX_SIZE
    LENGTH net_profile_id_cert_s_type.id_len */

typedef struct {

  /* Nested Auth Type */
  byte                                        auth;

  /* Nested Profile Id */
  byte                                        profile_id;

} net_profile_nested_s_type;

typedef struct {

  /* Nested profile info */
  net_profile_nested_s_type                   nested;

  /* Certificate Id */
  byte                                        cert_id;

  /* ID for PEAP, TTLS */
  net_profile_id_s_type                       user_id;

  /* server ID */
  net_profile_server_id_s_type                server_id;

} net_profile_peap_ttls_s_type;



/* 802.1X based profile info.
*/

typedef union{

  /* For 802 1X Password Based. Home Environment. */
  net_profile_pass_s_type                     passwd;

  /* For 802 1X TLS or TLS_WKEY Based. */
  net_profile_id_s_type                       tls;

  /* For 802 1X MD5 Based. */
  net_profile_id_pass_s_type                  md5;

  /* For 802 1X TLS with Certificate Based. */
  net_profile_id_cert_s_type                  tls_cert;

} net_wlan_8021x_info_u_type;
/*~ CASE NET_AUTH_TYPE_8021X_PASSWD net_wlan_8021x_info_u_type.passwd */
/*~ CASE NET_AUTH_TYPE_8021X_TLS_RADIUS net_wlan_8021x_info_u_type.tls */
/*~ CASE NET_AUTH_TYPE_8021X_TLS_RADIUS_WKEY net_wlan_8021x_info_u_type.tls */
/*~ CASE NET_AUTH_TYPE_8021X_MD5_RADIUS net_wlan_8021x_info_u_type.md5 */
/*~ CASE NET_AUTH_TYPE_8021X_TLS_CERT net_wlan_8021x_info_u_type.tls_cert */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* WLAN Authentication profiles.
*/

typedef union {

  /* WLAN WEP Info */
  net_wlan_40_bit_wep_info_s_type                  wep_40_bit_info;

  net_wlan_104_bit_wep_info_s_type                 wep_104_bit_info;

  /* WLAN 802.1X Info */
  net_wlan_8021x_info_u_type                       x_info;

  /* ID for PAP,CHAP,MsCHAPv2, PEAP, TTLS */
  net_profile_id_s_type                            user_id;

  /* Password for PAP,CHAP,MsCHAPv2 */
  net_profile_id_pass_s_type                       passwd;

  /* PEAP,TTLS */
  net_profile_peap_ttls_s_type                     peap_ttls;

  /*
  ** The following is done to support RPC:
  ** The fields of the x_info union have to be unpacked here because the
  ** discriminator for them is out of the scope of the
  ** net_wlan_8021x_info_u_type definition. The fields can be added safely
  ** here because this is a union.
  */

  /* For 802 1X Password Based. Home Environment. */
  net_profile_pass_s_type                          x_info_passwd;

  /* For 802 1X TLS or TLS_WKEY Based. */
  net_profile_id_s_type                            x_info_tls;

  /* For 802 1X MD5 Based. */
  net_profile_id_pass_s_type                       x_info_md5;

  /* For 802 1X TLS with Certificate Based. */
  net_profile_id_cert_s_type                       x_info_tls_cert;

} net_wlan_auth_info_u_type;

/*~ PARTIAL net_wlan_auth_info_u_type */

/*~ IF ( _DISC_ == ( ((uint32) NET_AUTH_TYPE_NO_AUTH << 4) | 
                     ((uint32) NET_ENCRY_TYPE_NO_ENCRY )) ) 
    net_wlan_auth_info_u_type.void */
/*~ IF ( _DISC_ == ( ((uint32) NET_AUTH_TYPE_WEP << 4) | 
                     ((uint32) NET_ENCRY_TYPE_WEP_40_BIT)) ) 
    net_wlan_auth_info_u_type.wep_40_bit_info */
/*~ IF ( _DISC_ == ( ((uint32) NET_AUTH_TYPE_WEP << 4) | 
                     ((uint32) NET_ENCRY_TYPE_WEP_104_BIT)) ) 
    net_wlan_auth_info_u_type.wep_104_bit_info */ 

/*~ IF ( (_DISC_ & 0xf0) == ((uint32) NET_AUTH_TYPE_8021X_PASSWD << 4) )
    net_wlan_auth_info_u_type.x_info_passwd */

/*~ IF ( (_DISC_ & 0xf0) == ((uint32) NET_AUTH_TYPE_8021X_MD5_RADIUS << 4) )
    net_wlan_auth_info_u_type.x_info_md5 */

/*~ IF ( (_DISC_ & 0xf0) == ((uint32) NET_AUTH_TYPE_8021X_TLS_RADIUS << 4) ||
         (_DISC_ & 0xf0) == ((uint32) NET_AUTH_TYPE_8021X_TLS_RADIUS_WKEY << 4) )
    net_wlan_auth_info_u_type.x_info_tls */

/*~ IF ( (_DISC_ & 0xf0) == ((uint32) NET_AUTH_TYPE_8021X_TLS_CERT << 4) )
    net_wlan_auth_info_u_type.x_info_tls_cert */


typedef struct {

  /* The WLAN Encryption Type */
  byte                                             encryp_type;

  /* The WLAN Authentication Type */
  byte                                             auth_type;

  /* Mobile IP Profile Id */
  byte                                             mob_ip_profile_id;

  /* The WLAN Authentication Info */
  net_wlan_auth_info_u_type                        auth_info;

} net_wlan_profile_s_type;
/*~ FIELD net_wlan_profile_s_type.auth_info
    DISC (((uint32)_OBJ_.auth_type << 4) | _OBJ_.encryp_type) */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Profile structure as required by NET.
*/

typedef union {

  net_wlan_profile_s_type                          wlan;

} net_profile_u_type;
/*~ CASE NET_PROFILE_TYPE_NONE net_profile_u_type.void */
/*~ CASE NET_PROFILE_TYPE_WLAN net_profile_u_type.wlan */


typedef struct {

  /* Profile Length*/
  byte                                             len;

  /* Profile Id */
  byte                                             profile_id;

  /* Profile Type */
  byte                                             type;

  /* Profile */
  net_profile_u_type                               profile;

} net_profile_s_type;
/*~ FIELD net_profile_s_type.profile DISC net_profile_s_type.type */

/*===========================================================================

                     NET LIST PARSER DEFINITION

===========================================================================*/
#define  NET_SYS_LIST_SIZE                   20
#define  NET_PROFILE_LIST_SIZE               20

/* File Header information.
*/
typedef struct {

  word           len;

  word           id;

  byte           rev_id;

  byte           min_sw_ver;

} net_file_header_s_type;

/* NET Parser structure. This objects holds information that help in
** parsing a buffer.
*/
typedef struct {

  word                     profile_tbl_offset;

  word                     sys_rec_tbl_offset;

  word                     profile_tbl_end;

  word                     sys_rec_tbl_end;

  word                     num_profiles_offset;

  word                     num_sys;

  word                     num_profiles;


  /* Pointer to buffer to hold raw file data.
  */
  byte                     *buf_ptr;

  /* Buffer Size.
  */
  uint32                   size;

  /* File hedaer.
  */
  net_file_header_s_type   hdr;

  /* A bitmap to indicate if parser object is created or not.
  */
  byte                     make_mask;

} net_file_parser_s_type;
/*~ FIELD net_file_parser_s_type.buf_ptr VARRAY NET_LIST_FILE_MAX_SIZE
    LENGTH net_file_parser_s_type.size */

/*===========================================================================

                     FILE ACCESSOR FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION net_get_sys_rec

DESCRIPTION
  Read the Specified record.
  Id specifies index of the record. Id=0 mean Ist record.

DEPENDENCIES

  NET LIST is already initialized. net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type    net_get_sys_rec
(

  word                   pos,
    /* Position of the System Record to be read. */

  net_sys_rec_s_type     *rec_ptr,
   /* Record Structure where desired record is returned. */

  const net_file_parser_s_type *parser_ptr
);
/*~ FUNCTION net_get_sys_rec */
/*~ PARAM pos */
/*~ PARAM OUT rec_ptr POINTER */
/*~ PARAM IN parser_ptr POINTER */

/*===========================================================================

FUNCTION net_get_profile

DESCRIPTION
  Read the Specified profile.

DEPENDENCIES

  NET LIST is already initialized. net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type    net_get_profile
(
  byte                   id,
   /* Id of the Profile to be read. */

  net_profile_s_type     *profile_ptr,
   /* Structure where profile will be returend */

  const net_file_parser_s_type *parser_ptr
);
/*~ FUNCTION net_get_profile */
/*~ PARAM id */
/*~ PARAM OUT profile_ptr POINTER */
/*~ PARAM IN parser_ptr POINTER */

/*===========================================================================

FUNCTION net_insert_sys_rec

DESCRIPTION
  Put the Specified record.
  location Id specifies index of the record. locaiotn Id=0 mean Ist record.

DEPENDENCIES

  NET LIST is already initialized.net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type       net_insert_sys_rec
(
  /* Position at which this system record needs to be stored.
  */
  word                   pos,

  net_sys_rec_s_type     *rec_ptr,

  net_file_parser_s_type *parser_ptr
);
/*~ FUNCTION net_insert_sys_rec */
/*~ PARAM pos */
/*~ PARAM rec_ptr POINTER */
/*~ PARAM OUT parser_ptr POINTER */

/*===========================================================================

FUNCTION net_insert_profile

DESCRIPTION
  Put the Specified profile.

DEPENDENCIES

  NET LIST is already initialized.net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type       net_insert_profile
(
  net_profile_s_type     *profile_ptr,

  net_file_parser_s_type *parser_ptr
);
/*~ FUNCTION net_insert_profile */
/*~ PARAM profile_ptr POINTER */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_del_sys_rec

DESCRIPTION
  Delete the Specified record.
  location Id specifies index of the record. locaiotn Id=0 mean Ist record.

DEPENDENCIES

  NET LIST is already initialized. net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type     net_del_sys_rec
(
  word                    pos,
  /* Position of the system record to be deleted.
  */

  net_file_parser_s_type  *parser_ptr
);
/*~ FUNCTION net_del_sys_rec */
/*~ PARAM pos */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_modify_sys_rec

DESCRIPTION
  Modify the Specified record.
  location Id specifies index of the record. locaiotn Id=0 mean Ist record.

DEPENDENCIES

  NET LIST is already initialized. net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type    net_modify_sys_rec
(
  word                    pos,
  /* Position of the system record to be modified.
  */

  net_sys_rec_s_type      *rec_ptr,
  net_file_parser_s_type  *parser_ptr
);
/*~ FUNCTION net_modify_sys_rec */
/*~ PARAM pos */
/*~ PARAM rec_ptr POINTER */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_del_profile

DESCRIPTION
  Delete the Specified profile.

DEPENDENCIES

  NET LIST is already initialized. net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type    net_del_profile
(
  byte                    profile_id,
  net_file_parser_s_type  *parser_ptr
);
/*~ FUNCTION net_del_profile */
/*~ PARAM profile_id */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_modify_profile

DESCRIPTION
  Modify the Specified profile.

DEPENDENCIES

  NET LIST is already initialized. net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type   net_modify_profile
(
  net_profile_s_type      *profile_ptr,
  net_file_parser_s_type  *parser_ptr
);
/*~ FUNCTION net_modify_profile */
/*~ PARAM profile_ptr POINTER */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_insert_sys_rec_profile

DESCRIPTION
  Put the Specified record and Profile.
  location Id specifies index of the record. locaiotn Id=0 mean Ist record.

DEPENDENCIES

  NET LIST is already initialized. net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type    net_insert_sys_rec_profile
(

  word                    pos,
  /* Position of the system record where system record needs to ber
  ** inserted.
  */

  net_sys_rec_s_type      *rec_ptr,

  net_profile_s_type      *profile_ptr,

  net_file_parser_s_type  *parser_ptr

);
/*~ FUNCTION net_insert_sys_rec_profile */
/*~ PARAM pos */
/*~ PARAM rec_ptr POINTER */
/*~ PARAM profile_ptr POINTER */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_get_max_allowed_sys

DESCRIPTION
  Get the Maximum Systems allowed.

DEPENDENCIES

  NONE

RETURN VALUE
  Maximum Number of systems allowed.

SIDE EFFECTS
  None.

===========================================================================*/
extern word       net_get_max_allowed_sys
(
  void
);
/*~ FUNCTION net_get_max_allowed_sys */

/*===========================================================================

FUNCTION net_get_max_allowed_profile

DESCRIPTION
  Get the Maximum Profiles allowed.

DEPENDENCIES

  NONE

RETURN VALUE
  Maximum Number of Profiles allowed.

SIDE EFFECTS
  None.

===========================================================================*/
extern word       net_get_max_allowed_profile
(
  void
);
/*~ FUNCTION net_get_max_allowed_profile */

/*===========================================================================

FUNCTION net_get_num_sys

DESCRIPTION
  Get the Number of systems present.

DEPENDENCIES

  net_parser_init() is already called.

RETURN VALUE
  Number of systems.

SIDE EFFECTS
  None.

===========================================================================*/
extern word       net_get_num_sys
(
  const net_file_parser_s_type *parser_ptr
);
/*~ FUNCTION net_get_num_sys */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_get_num_profile

DESCRIPTION
  Get the Number of profiles present.

DEPENDENCIES

  net_parser_init() is already called.

RETURN VALUE
  Number of profiles.

SIDE EFFECTS
  None.

===========================================================================*/
extern word       net_get_num_profile
(
  const net_file_parser_s_type *parser_ptr
);
/*~ FUNCTION net_get_num_profile */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_extract_buf_ptr

DESCRIPTION
  Get the Pointer to the buffer to be written.

DEPENDENCIES

  net_parser_init() is already called.

RETURN VALUE
  Pointer to buffer

SIDE EFFECTS
  None.

===========================================================================*/
extern byte*       net_extract_buf_ptr
(
  const net_file_parser_s_type *parser_ptr
);
/*~ FUNCTION net_extract_buf_ptr VARRAY NET_LIST_FILE_MAX_SIZE */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_get_max_allowed_file_size

DESCRIPTION
  Get the Maximum File Size allowed.

DEPENDENCIES

  NONE

RETURN VALUE
  Maximum allowed file size.

SIDE EFFECTS
  None.

===========================================================================*/
extern word       net_get_max_allowed_file_size
(
  void
);
/*~ FUNCTION net_get_max_allowed_file_size */

/*===========================================================================

FUNCTION net_get_file_len

DESCRIPTION
  Calculate the File Length.

DEPENDENCIES
  NONE

RETURN VALUE
  File length in bytes

SIDE EFFECTS
  None.

===========================================================================*/
extern word net_get_file_len
(
  const net_file_parser_s_type   *parser_ptr
);
/*~ FUNCTION net_get_file_len */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_parser_init

DESCRIPTION
  Initialize the parser

DEPENDENCIES
  NONE

RETURN VALUE
  net_status_e_type


SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type  net_parser_init
(
  net_file_parser_s_type *parser_ptr,
  byte                   *buf_ptr,
  word                   buf_size
);
/*~ FUNCTION net_parser_init */
/*~ PARAM OUT parser_ptr POINTER */
/*~ PARAM buf_ptr VARRAY LENGTH buf_size */

/*===========================================================================

FUNCTION net_file_read

DESCRIPTION
  Read the NET LIST from EFS file system.

DEPENDENCIES

  NONE

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type                  net_file_read
(

        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** read
            */
        int32                    *size_ptr
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be NULL.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            ** If this function returns FALSE, then *size_ptr is not updated.
            */
);
/*~ FUNCTION net_file_read */
/*~ PARAM OUT data_ptr VARRAY NET_LIST_FILE_MAX_SIZE LENGTH *size_ptr */
/*~ PARAM INOUT size_ptr POINTER */

/*===========================================================================

FUNCTION net_file_write

DESCRIPTION
  Write the NET LIST into EFS file system.

DEPENDENCIES

  NONE

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type                  net_file_write
(

        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** read
            */
        int32                    *size_ptr
            /* On input this parameter is the  maximum number
            ** of bytes to be copied from the buffer pointed by data_ptr.
            ** This parameter must not be NULL.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were written into the file.
            ** If this function returns FALSE, then *size_ptr is not updated.
            */
);
/*~ FUNCTION net_file_write */
/*~ PARAM data_ptr VARRAY NET_LIST_FILE_MAX_SIZE LENGTH *size_ptr */
/*~ PARAM INOUT size_ptr POINTER */

/*===========================================================================

FUNCTION net_file_read2

DESCRIPTION
  Read the net file from EFS file system.

DEPENDENCIES

  NONE

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type                  net_file_read2
(

        net_e_type              file_type,

        byte                   *data_ptr,
        /* Pointer to a data buffer where to store the data that is
        ** read
        */

        const uint32           *size_ptr
        /* On input this parameter is the  maximum number
        ** of bytes to be copied from the buffer pointed by data_ptr.
        ** This parameter must not be NULL.
        ** On output it is set *size_ptr to the actual number of bytes
        ** that were written into the file.
        ** If this function returns FALSE, then *size_ptr is not updated.
        */
);
/*~ FUNCTION net_file_read2 */
/*~ PARAM file_type */
/*~ PARAM OUT data_ptr VARRAY NET_LIST_FILE_MAX_SIZE LENGTH *size_ptr */
/*~ PARAM size_ptr POINTER */

/*===========================================================================

FUNCTION net_file_write2

DESCRIPTION
  Write the net file into EFS file system.

DEPENDENCIES

  NONE

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type                  net_file_write2
(
        net_e_type              file_type,

        byte                   *data_ptr,
        /* Pointer to a data buffer where to store the data that is
        ** read
        */

        uint32                  *size_ptr
        /* On input this parameter is the  maximum number
        ** of bytes to be copied from the buffer pointed by data_ptr.
        ** This parameter must not be NULL.
        ** On output it is set *size_ptr to the actual number of bytes
        ** that were written into the file.
        ** If this function returns FALSE, then *size_ptr is not updated.
        */
);
/*~ FUNCTION net_file_write */
/*~ PARAM file_type */
/*~ PARAM data_ptr VARRAY NET_LIST_FILE_MAX_SIZE LENGTH *size_ptr */
/*~ PARAM INOUT size_ptr POINTER */

/*===========================================================================

FUNCTION net_get_wlan_sys_rec

DESCRIPTION
  Read the Specified record.
  Id specifies index of the record. Id=0 mean Ist record.

DEPENDENCIES

  NET LIST is already initialized. net_parser_init() is already called.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type    net_get_wlan_sys_rec
(

  word                           pos,
    /* Position of the System Record to be read. */

  net_sys_wlan_record_s_type     *rec_ptr,
   /* Record Structure where desired record is returned. */

  const net_file_parser_s_type         *parser_ptr
);
/*~ FUNCTION net_get_wlan_sys_rec */
/*~ PARAM pos */
/*~ PARAM OUT rec_ptr POINTER */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_insert_wlan_sys_rec

DESCRIPTION
  Put the Specified record.

DEPENDENCIES

  NET LIST is already initialized.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type    net_insert_wlan_sys_rec
(

  word                            pos,
  /* Position of the system record to be inserted.
  */

  const net_sys_wlan_record_s_type      *rec_ptr,

  net_file_parser_s_type          *parser_ptr
);
/*~ FUNCTION net_insert_wlan_sys_rec */
/*~ PARAM pos */
/*~ PARAM rec_ptr POINTER */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_modify_wlan_sys_rec

DESCRIPTION
  Modify the Specified record.

DEPENDENCIES

  NET LIST is already initialized.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type    net_modify_wlan_sys_rec
(

  word                            pos,
  /* Position of the system record to be modified.
  */

  const net_sys_wlan_record_s_type      *rec_ptr,

  net_file_parser_s_type          *parser_ptr
);
/*~ FUNCTION net_modify_wlan_sys_rec */
/*~ PARAM pos */
/*~ PARAM rec_ptr POINTER */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_insert_wlan_sys_rec_profile

DESCRIPTION
  Put the Specified record and Profile.

DEPENDENCIES

  NET LIST is already initialized.

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type    net_insert_wlan_sys_rec_profile
(
  word                            pos,
  /* Position at which the system record needs to be inserted.
  */

  const net_sys_wlan_record_s_type      *rec_ptr,
  net_profile_s_type              *profile_ptr,
  net_file_parser_s_type          *parser_ptr
);
/*~ FUNCTION net_insert_wlan_sys_rec_profile */
/*~ PARAM pos */
/*~ PARAM rec_ptr POINTER */
/*~ PARAM profile_ptr POINTER */
/*~ PARAM parser_ptr POINTER */

/*===========================================================================

FUNCTION net_pack_profile

DESCRIPTION
  Pack the given profile in the given buffer.

DEPENDENCIES

  NONE

RETURN VALUE
  NONE


SIDE EFFECTS
  None.

===========================================================================*/
extern void net_pack_profile
(
  byte                      *buf_ptr,
  word                      buf_size,
  net_profile_s_type        *profile_ptr
);
/*~ FUNCTION net_pack_profile */
/*~ PARAM OUT buf_ptr VARRAY NET_LIST_FILE_MAX_SIZE LENGTH buf_size */
/*~ PARAM profile_ptr POINTER */

/*===========================================================================

FUNCTION net_unpack_profile

DESCRIPTION
  UnPack the given profile in the given buffer.

DEPENDENCIES

  NONE

RETURN VALUE
  NONE


SIDE EFFECTS
  None.

===========================================================================*/
extern void net_unpack_profile
(
  net_profile_s_type  *profile_ptr,
  byte                *buf_ptr,
  word                buf_size
);
/*~ FUNCTION net_unpack_profile */
/*~ PARAM OUT profile_ptr POINTER */
/*~ PARAM buf_ptr VARRAY LENGTH buf_size */

/*===========================================================================

FUNCTION net_get_sw_ver

DESCRIPTION
  Get the version of the sw that operate on the  NET file.

DEPENDENCIES

  NONE

RETURN VALUE
  Current Software Version.


SIDE EFFECTS
  None.

===========================================================================*/
extern byte net_get_sw_ver
(
  void
);
/*~ FUNCTION net_get_sw_ver */

/*===========================================================================

FUNCTION net_validate

DESCRIPTION
  Validate the NET buffer.

DEPENDENCIES

  NONE

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type net_validate
(
  byte   *buf_ptr,
  word   buf_size
);
/*~ FUNCTION net_validate */
/*~ PARAM buf_ptr VARRAY LENGTH buf_size */

/*===========================================================================

FUNCTION net_parser_make

DESCRIPTION
  Make the parser structure.

DEPENDENCIES

  NONE

RETURN VALUE
  net_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern net_status_e_type net_parser_make
(
  net_file_parser_s_type   *parser_ptr,
  byte                     *buf_ptr,
  word                     buf_size
);
/*~ FUNCTION net_parser_make */
/*~ PARAM OUT parser_ptr POINTER */
/*~ PARAM buf_ptr VARRAY LENGTH buf_size */

/*===========================================================================

FUNCTION net_set_file_id

DESCRIPTION
  Sets the File Id.

DEPENDENCIES

  NONE

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void net_set_file_id
(
  net_file_parser_s_type   *parser_ptr,
  byte                     file_id
);
/*~ FUNCTION net_set_file_id */
/*~ PARAM OUT parser_ptr POINTER */
/*~ PARAM file_id */

#endif  /* NET_H */


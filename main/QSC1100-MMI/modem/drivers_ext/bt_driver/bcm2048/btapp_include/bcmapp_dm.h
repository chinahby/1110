/*****************************************************************************
**                                                                            
**  Name:             bcmapp_dm.h                                              
**                                                                            
**  Description:     This file contains bcmui internal interface               
**				     definition                                         
**                                                                            
**  Copyright (c) 2000-2004, Widcomm Inc., All Rights Reserved.               
**  Widcomm Bluetooth Core. Proprietary and confidential.                     
******************************************************************************/

#include "gki.h"
#ifndef AEE_SIMULATOR
#include "MediaGallery.h"
#endif
#ifndef BCMAPP_DM_H
#define BCMAPP_DM_H


#define BTUI_AUTH_REQ_NO            0   /* always not required */
#define BTUI_AUTH_REQ_YES           1   /* always required */
#define BTUI_AUTH_REQ_GEN_BOND      2   /* security database + general bonding, dedicated bonding as not required */
#define BTUI_AUTH_REQ_DEFAULT       3   /* security database, dedicated bonding as not required */
#define BTUI_AUTH_REQ_GEN_BOND_DD   4   /* security database + general bonding, dedicated bonding as required */
#define BTUI_AUTH_REQ_DEFAULT_DD    5   /* security database, dedicated bonding as required */

#define BTUI_AUTH_REQ_DD_BIT        4   /* dedicated bonding bit */

#define BTUI_PATH_LEN AEE_MAX_FILE_NAME
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
  BT_SD_MAJOR_DEVICE_CLASS_MISC                 = 0x0000,
  BT_SD_MAJOR_DEVICE_CLASS_COMPUTER             = 0x0100,
  BT_SD_MAJOR_DEVICE_CLASS_PHONE                = 0x0200,
  BT_SD_MAJOR_DEVICE_CLASS_LAN_ACCESS_POINT     = 0x0300,
  BT_SD_MAJOR_DEVICE_CLASS_AUDIO                = 0x0400,
  BT_SD_MAJOR_DEVICE_CLASS_PERIPHERAL           = 0x0500,
  BT_SD_MAJOR_DEVICE_CLASS_IMAGING              = 0x0600,
  BT_SD_MAJOR_DEVICE_CLASS_UNCLASSIFIED         = 0x1F00
} bt_sd_major_device_class_enum_type;
typedef enum
{
  BT_SD_MINOR_DEVICE_CLASS_UNCLASSIFIED         = 0x0000,

  BT_SD_MINOR_DEVICE_CLASS_DESKTOP_WORKSTATION  = BT_SD_MAJOR_DEVICE_CLASS_COMPUTER + 1,
  BT_SD_MINOR_DEVICE_CLASS_SERVER_CLASS_COMPUTER,
  BT_SD_MINOR_DEVICE_CLASS_LAPTOP,
  BT_SD_MINOR_DEVICE_CLASS_HANDHELD_PC_OR_PDA,
  BT_SD_MINOR_DEVICE_CLASS_PALM_SIZED_PC_OR_PDA,
  BT_SD_MINOR_DEVICE_CLASS_WEARABLE_COMPUTER,

  BT_SD_MINOR_DEVICE_CLASS_CELLULAR             = BT_SD_MAJOR_DEVICE_CLASS_PHONE + 1,
  BT_SD_MINOR_DEVICE_CLASS_CORDLESS,
  BT_SD_MINOR_DEVICE_CLASS_SMART_PHONE,
  BT_SD_MINOR_DEVICE_CLASS_WIRED_MODEM_OR_VOICE_GATEWAY,
  BT_SD_MINOR_DEVICE_CLASS_COMMON_ISDN_ACCESS,

  BT_SD_MINOR_DEVICE_CLASS_FULLY_AVAILABLE      = BT_SD_MAJOR_DEVICE_CLASS_LAN_ACCESS_POINT,
  BT_SD_MINOR_DEVICE_CLASS_1_TO_17_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_17_TO_33_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_33_TO_50_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_50_to_67_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_67_TO_83_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_83_TO_99_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_NO_SERVICE_AVAILABLE,

  BT_SD_MINOR_DEVICE_CLASS_HEADSET_PROFILE      = BT_SD_MAJOR_DEVICE_CLASS_AUDIO + 1,
  BT_SD_MINOR_DEVICE_CLASS_HANDSFREE,

  BT_SD_MINOR_DEVICE_CLASS_JOYSTICK             = BT_SD_MAJOR_DEVICE_CLASS_PERIPHERAL + 1,
  BT_SD_MINOR_DEVICE_CLASS_GAMEPAD,
  BT_SD_MINOR_DEVICE_CLASS_REMOTE_CONTROL,
  BT_SD_MINOR_DEVICE_CLASS_SENSING_DEVICE,
  BT_SD_MINOR_DEVICE_CLASS_DIGITIZER_TABLET,
  BT_SD_MINOR_DEVICE_CLASS_SIM_CARD_READER

} bt_sd_minor_device_class_enum_type;
typedef enum
{
  /* 
   * these bit masks should ONLY be used when working on byte 2 of the class of 
   * device field, namely ...class_of_device.cod_bytes[2]
   */
  BT_SD_MAJOR_SERVICE_CLASS_POSITIONING     = 0x01,
  BT_SD_MAJOR_SERVICE_CLASS_NETWORKING      = 0x02,
  BT_SD_MAJOR_SERVICE_CLASS_RENDERING       = 0x04,
  BT_SD_MAJOR_SERVICE_CLASS_CAPTURING       = 0x08,
  BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER = 0x10,
  BT_SD_MAJOR_SERVICE_CLASS_AUDIO           = 0x20,
  BT_SD_MAJOR_SERVICE_CLASS_TELEPHONY       = 0x40,
  BT_SD_MAJOR_SERVICE_CLASS_INFORMATION     = 0x80 
} bt_sd_major_service_class_enum_type;

enum
{
    BT_SAVE_TO_NONE = 0x00,
    BT_SAVE_TO_PHONE,
    BT_SAVE_TO_CARD
};
void bcmapp_dm_startup(void);
void bcmapp_dm_init(void);
void bcmapp_dm_pin_code_reply(BOOLEAN accept, UINT8 pin_len, UINT8 *p_pin);
void bcmapp_dm_confirm_reply(BOOLEAN accept);
void bcmapp_dm_passkey_cancel(void);
void bcmapp_dm_rmt_oob_reply(BOOLEAN accept, BT_OCTET16 c, BT_OCTET16 r);
void  bcmapp_dm_loc_oob(void);
void bcmapp_dm_authorize_resp(tBTA_AUTH_RESP response);
void bcmapp_dm_disable_bt(void);
void bcmapp_dm_set_visibility( BOOLEAN is_visible, BOOLEAN is_temp);
void bcmapp_dm_set_find_me( void);
void bcmapp_dm_make_non_discoverable(void);
void bcmapp_dm_set_local_name(char *p_name);
BOOLEAN bcmapp_dm_set_trusted(tBTA_SERVICE_MASK trusted_mask, tBTUI_REM_DEVICE * p_device_rec);
void bcmapp_dm_set_not_trusted(tBTUI_REM_DEVICE * p_device_rec);
BOOLEAN bcmapp_dm_delete_device(BD_ADDR bd_addr);
void bcmapp_dm_discover_device(BD_ADDR bd_addr, BOOLEAN is_new);
BOOLEAN bcmapp_dm_stored_device_unbond (void);
void bcmapp_dm_cancel_search(void);
void bcmapp_dm_search(tBTA_SERVICE_MASK services,tBTA_DM_INQ *p_data);
BOOLEAN bcmapp_dm_add_device(void);
void bcmapp_dm_sec_add_device(tBTUI_REM_DEVICE * p_device_rec);
void bcmapp_dm_bond(tBTUI_REM_DEVICE * p_device_rec);
void bcmapp_dm_bond_cancel(tBTUI_REM_DEVICE * p_device_rec);
void bcmapp_dm_rename_device(tBTUI_REM_DEVICE * p_device_rec, char * p_text);
//extern void bcmapp_dm_add_devices(void);
tBTUI_REM_DEVICE * bcmapp_dm_db_get_device_info(BD_ADDR bd_addr);
BOOLEAN bcmapp_dm_db_get_device_list(	tBTA_SERVICE_MASK services,	tBTUI_REM_DEVICE * p_device,UINT8*	number_of_devices, BOOLEAN new_only);
void bcmapp_dm_switch_bb2mm(void);
void bcmapp_dm_switch_mm2bb(void);

void bcmapp_dm_proc_io_req(BD_ADDR bd_addr, tBTA_IO_CAP *p_io_cap, tBTA_OOB_DATA *p_oob_data,
                                 tBTA_AUTH_REQ *p_auth_req, BOOLEAN is_orig);
void bcmapp_dm_proc_io_rsp(BD_ADDR bd_addr, tBTA_IO_CAP io_cap, tBTA_AUTH_REQ auth_req);
void bcmapp_dm_proc_lk_upgrade(BD_ADDR bd_addr, BOOLEAN *p_upgrade);
void bcmapp_set_bt_test_mode(void);
#if BTA_DI_INCLUDED == TRUE
void bcmapp_dm_di_discover(BD_ADDR bd_addr);
UINT16 bcmapp_dm_add_di_record(void);
tBTA_STATUS bcmapp_dm_get_di_remote_record(tBTA_DI_GET_RECORD *p_record, UINT8 index);
tBTA_STATUS bcmapp_dm_get_di_local_record(tBTA_DI_GET_RECORD *p_di_record, UINT32 handle);
#endif
void bcmapp_dm_process_cod(DEV_CLASS cod_bytes, uint16* svc_cls_ptr, uint16*  mjr_dev_cls_ptr,uint16* mnr_dev_cls_ptr,  tBTA_SERVICE_MASK *service);
tBTA_SERVICE_MASK bcmapp_dm_change_id_to_server(tBTA_SERVICE_ID id);
boolean bcmapp_dm_get_incall_state(boolean b_use_sd);
#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
byte bcmapp_dm_get_stroe_path(uint32 size, char *path,char * name);
boolean bcmapp_dm_change_brewaddr_to_fs(char * brewaddr,char * fsaddr );
#endif
#endif

/*****************************************************************************
**
**  Name:             bt_ui_int.h
**
**  Description:     This file contains bt_ui internal interface
**				     definition
**
**  Copyright (c) 2000-2006, Broadcom Corp., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
******************************************************************************/

//#include "bte_ht_api.h"

#include "bt_ui_platform.h"
#include "gki.h"
#include "bt_ui.h"
#ifdef AEE_SIMULATOR
#include "..\Appscommon\Appscommon.h"
#else
#include "Appscommon.h"
#endif
#ifdef __CYGWIN__
#include <stdio.h>
#endif

#ifndef BTUI_INT_H
#define BTUI_INT_H


#if( defined BTA_SS_INCLUDED ) && (BTA_SS_INCLUDED == TRUE)
#include "bta_ss_api.h"
#endif
#if( defined BTA_AC_INCLUDED ) && (BTA_AC_INCLUDED == TRUE)
#include "bta_ac_api.h"
#endif
#if (defined BTA_FT_INCLUDED) && (BTA_FT_INCLUDED == TRUE)
#include "bta_ft_api.h"
#include "xml_flp_api.h"
#include "xml_vlist_api.h"
#endif

#if (defined BTA_PBS_INCLUDED) && (BTA_PBS_INCLUDED == TRUE)
#include "bta_pbs_api.h"
#endif

#if (defined BTA_PBC_INCLUDED) && (BTA_PBC_INCLUDED == TRUE)
#include "bta_pbc_api.h"
#include "xml_vlist_api.h"
#endif

#if (defined BTA_SC_INCLUDED) && (BTA_SC_INCLUDED == TRUE)
#include "bta_sc_api.h"
#endif

#if (defined BTA_AA_INCLUDED) && (BTA_AA_INCLUDED == TRUE)
#include "bta_aa_api.h"
#endif
#if (defined BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)
#include "bta_av_api.h"
#endif
#if (defined BTA_PAN_INCLUDED) && (BTA_PAN_INCLUDED == TRUE)
#include "bta_pan_api.h"
#endif

#if( defined BTA_HD_INCLUDED ) && (BTA_HD_INCLUDED == TRUE)
#include "bta_hd_api.h"
#endif

#if( defined BTA_HH_INCLUDED ) && (BTA_HH_INCLUDED == TRUE)
#include "bta_hh_api.h"
#endif

/* UI IDs */
#define UI_DM_ID     1
#define UI_DG_ID     2
#define UI_AG_ID     3
#define UI_FTS_ID    4 
#define UI_OPC_ID    5
#define UI_CT_ID     6
#define UI_TEST_ID   7
#define UI_FTC_ID    8
#define UI_SS_ID     9
#define UI_ACC_ID    10
#define UI_ACS_ID    11 
#define UI_AA_ID     12
#define UI_HD_ID     13
#define UI_PAN_ID    14
#define UI_SC_ID     15
#define UI_AV_ID     16
#define UI_PR_ID     17
#define UI_HH_ID     18
#define UI_PBS_ID    19
#define UI_PBC_ID    20
#define UI_PRM_ID    21
#define UI_FM_ID     22


/* macro for getting the
root state for a ID */
#define UI_STATE_START(id)         ((id) << 8)

typedef UINT32 tBTUI_STATE;

/* main states */
enum
{
	UI_INIT, 
	UI_IDLE ,
	UI_CURRENT_CONNECTIONS
};


/* UI DM states */
enum
{
	UI_BT_SETTING  =	UI_STATE_START(UI_DM_ID),                   
	UI_VIEW_DEVICES,     
	UI_VISIBILITY,             
	UI_CHANGE_NAME,  
	UI_DEVICE_NAME,
	UI_DISPLAY_DEVICES,
	UI_SEARCH_NEW_DEVICE,
	UI_VIEW_DISPLAYED_DEVICE,           
	UI_VIEW_DISPLAY_DI,
	UI_LOCAL_DI,
	UI_SELECT_DEVICE ,    
	UI_NEW_DEVICE_DISCOVER,
	UI_NEW_DEVICE_DISCOVER_CMPL,
	UI_PIN_REPLY,
	UI_PIN_REPLY_MANUAL,    
	UI_CONFIRM_REPLY,
	UI_PASSKEY_REPLY,
	UI_OOB_C_REPLY,
	UI_OOB_R_REPLY,
	UI_AUTH_REPLY,
	UI_DEVICE_BOND,
	UI_DEVICE_BOND_MANUAL,    
	UI_DISPLAY_SERVICES,
	UI_ENTER_BDADDR,
	UI_DEVICE_BOND_CANCEL
};



/* UIAG states */
enum
{
	
	UI_AUDIO_DEVICE	=	UI_STATE_START(UI_AG_ID),  
	UI_VIEW_AUDIO_DEVICES,
	UI_AUDIO_DEVICE_CONNECTED,
	UI_AUDIO_DEVICE_SELECTED,
	
	UI_HF_DEVICES,
	UI_SELECT_AUDIO_DEVICE,
	UI_NEW_AUDIO_DEVICE,
	UI_NEW_HF_DEVICE,
	UI_SELECT_HF_BOND,
	UI_SELECT_AUDIO_DEVICE_BOND,
	UI_SELECT_NEW_AUDIO_DEVICE,
	UI_HS_CONNECTED,
	UI_SELECT_NEW_AUDIO_DEVICE_BOND,
	UI_SELECT_NEW_HF_DEVICE,
	UI_HF_CONNECTED,
	UI_SELECT_NEW_HF_BOND,
	UI_ACTIVE_HS_CONNECTION,
	UI_ACTIVE_HF_CONNECTION
};

/* UIDG states */
enum
{
	UI_DG_CURRENT_CONNECTIONS  = UI_STATE_START(UI_DG_ID),
	UI_DG_VIEW_DEVICES,
	UI_DG_MAIN,
	UI_DG_DEVICE_SELECTED,
	UI_DG_LOOP_BACK,
	UI_SELECT_DG_DEVICE,
	UI_NEW_DG_DEVICE,
	UI_SELECT_NEW_DG_DEVICE,		
};


/* UIOP states */
enum
{
	UI_OP_DEVICE  = UI_STATE_START(UI_OPC_ID), 
	UI_VIEW_OP_DEVICES,
	UI_NEW_OP_DEVICE,
	UI_SELECT_OP_DEVICE,
	UI_SELECT_NEW_OP_DEVICE,
	UI_VIEW_OP_FILES,
	UI_SENDING_OP_FILE,
	UI_SENDED_OP_FILE,
	
	UI_OPC_MAIN_MENU,
	UI_OPC_VIEW_OP_DEVICES,
	UI_OPC_SELECT_DEVICE,
	UI_OPC_VIEW_FILES
};

#if (defined BTA_CT_INCLUDED) && (BTA_CT_INCLUDED == TRUE)
/* Cordless UI states */
enum
{
	UI_CT_MAIN_MENU  = UI_STATE_START(UI_CT_ID),		
	UI_CT_VIEW_CT_DEVICES,
	UI_CT_VIEW_IC_DEVICES,
	UI_CT_ACTIVE_CONNECTION,
	UI_CT_ENTER_NUMBER,
	UI_CT_SELECT_CT_DEVICES,
	UI_CT_NAME_OR_EXTN,
	UI_CT_VIEW_IC_DEVICES_BY_NAME,
	UI_CT_VIEW_IC_DEVICES_BY_EXTN,
	UI_CT_SELECT_IC_DEVICE_BY_NAME
};
#endif

#if (defined BTA_FS_INCLUDED) && (BTA_FS_INCLUDED == TRUE)
/* FT server UI states */
enum
{
	UI_FT_SERVER  = UI_STATE_START(UI_FTS_ID), 
};
#endif

#if (defined BTA_SS_INCLUDED) && (BTA_SS_INCLUDED == TRUE)
/* Sync server UI states */
enum
{
	UI_SS_SERVER  = UI_STATE_START(UI_SS_ID), 
};
#endif

#if (defined BTA_FT_INCLUDED) && (BTA_FT_INCLUDED == TRUE)
/* FT client UI states */
enum
{
	UI_FTC_MAIN_MENU  = UI_STATE_START(UI_FTC_ID), 		
	UI_FTC_VIEW_DEVICES,
	UI_FTC_VIEW_FILES,
	UI_FTC_VIEW_PEER_FOLDER,
	UI_FTC_PEER_FOLDER_EMPTY,
	UI_FTC_VIEW_SELECTED_FILE,
	UI_FTC_ENTER_FOLDER_NAME,
	UI_FTC_DEVICE, 
	UI_VIEW_FTC_DEVICES,
	UI_NEW_FTC_DEVICE,
	UI_SELECT_FTC_DEVICE,
	UI_SELECT_NEW_FTC_DEVICE,
	UI_VIEW_FTC_FILES,
	UI_SENDING_FTC_FILE,
	UI_RECEIVING_FTC_FILE,
	UI_REMOVING_FTC_FILE
};
#endif

#if (defined BTA_PBC_INCLUDED) && (BTA_PBC_INCLUDED == TRUE)
/* PBC client UI states */
enum
{
	UI_PBC_MAIN_MENU  = UI_STATE_START(UI_PBC_ID), 		
	UI_PBC_VIEW_DEVICES,
	UI_PBC_VIEW_PEER_FOLDER,
	UI_PBC_PEER_FOLDER_EMPTY,
	UI_PBC_VIEW_SELECTED_FILE,
	UI_PBC_DEVICE, 
	UI_VIEW_PBC_DEVICES,
	UI_NEW_PBC_DEVICE,
	UI_SELECT_PBC_DEVICE,
	UI_SELECT_NEW_PBC_DEVICE,
	UI_PBC_VIEW_PULL_PB,
	UI_PBC_ENTER_PB_NAME,
	UI_PBC_ENTER_LIST_SEARCH_VALUE,
	UI_PBC_VIEW_PB_OPTION
};
#endif

#if (defined BTA_AC_INCLUDED) && (BTA_AC_INCLUDED == TRUE)
/* AC server UI states */
enum
{
	UI_AC_SERVER  = UI_STATE_START(UI_ACS_ID), 
};

/* AC client UI states */
enum
{
	UI_ACC_DEVICE  = UI_STATE_START(UI_ACC_ID), 
	UI_VIEW_ACC_DEVICES,
	UI_NEW_ACC_DEVICE,
	UI_SELECT_ACC_DEVICE,
	UI_SELECT_NEW_ACC_DEVICE,
	UI_ACC_CONNECTING,
	UI_VIEW_ACC_FILES,
	UI_SENDING_ACC_FILE,
	UI_RECEIVING_ACC_FILE
};
#endif

#if (defined BTA_AA_INCLUDED) && (BTA_AA_INCLUDED == TRUE)
/* AA states */
enum
{
	
	UI_AA_DEVICE        =      UI_STATE_START(UI_AA_ID),  
	UI_AA_MAIN,
	UI_AA_CURRENT_CONNECTION,
	UI_AA_VIEW_DEVICES,
	UI_AA_SELECT_DEVICE
};
#endif

#if (defined BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)
/* AV states */
enum
{
	
	UI_AV_DEVICE        =      UI_STATE_START(UI_AV_ID),  
	UI_AV_CURRENT_CONNECTION,
	UI_AV_VIEW_DEVICES,
	UI_AV_SELECT_DEVICE,
	UI_AV_OPEN,
	UI_AV_MAIN,
	UI_AV_ACTIVE_AUDIO,
	UI_NEW_AV_DEVICE,
	UI_SELECT_AV_DEVICE,
	UI_SELECT_NEW_AV_DEVICE
};
#endif

#if (defined BTA_HD_INCLUDED) && (BTA_HD_INCLUDED == TRUE)
/* HD states */
enum
{
	
	UI_HD_DEVICE      =     UI_STATE_START(UI_HD_ID),  
	UI_VIEW_HD_DEVICES,
	UI_NEW_HD_DEVICES,
	UI_SELECT_HD_DEVICE,
	UI_SELECT_NEW_HD_DEVICE,
	UI_NEW_HD_DEVICE
};
#endif

#if (defined BTA_HH_INCLUDED) && (BTA_HH_INCLUDED == TRUE)
/* HH states */
enum
{
	
	UI_HH_DEVICE      =     UI_STATE_START(UI_HH_ID),  
	UI_VIEW_HH_DEVICES,
	UI_NEW_HH_DEVICES,
	UI_SELECT_HH_DEVICE,
	UI_SELECT_NEW_HH_DEVICE,
	UI_NEW_HH_DEVICE
};
#endif

#if (defined BTA_PAN_INCLUDED) && (BTA_PAN_INCLUDED == TRUE)
/* PAN states */
enum
{
	
	UI_PAN_MAIN      =     UI_STATE_START(UI_PAN_ID),  
	UI_PAN_VIEW_DEVICES,
	UI_PAN_DEVICE_SELECTED,
	UI_PAN_SELECT_SERVICE_TO_CONNECT
		
};
#endif


#if (defined BTA_SC_INCLUDED) && (BTA_SC_INCLUDED == TRUE)
/* SC states */
enum
{	
	UI_SC_MAIN      =     UI_STATE_START(UI_SC_ID),  
	UI_SC_STATUS,
	UI_SC_SERVER,
	UI_SC_SIM_CARD

};
#endif

#if (defined BTA_PR_INCLUDED) && (BTA_PR_INCLUDED == TRUE)
/* PR states */
enum
{
	
	UI_PR_MAIN      =     UI_STATE_START(UI_PR_ID),  
	UI_PR_VIEW_DEVICES,
	UI_PR_GET_CAPS,
	UI_PR_VIEW_FILES
		
};
#endif


#if (defined BTA_FM_INCLUDED) && (BTA_FM_INCLUDED == TRUE)
/* FM states */
enum
{
	
	UI_FM_MAIN      =     UI_STATE_START(UI_FM_ID),  
	UI_FM_TUNE_FREQ,
	UI_FM_AUDIO_MODE,
	UI_FM_ENABLE_OPT,
	UI_FM_RDS_SCAN,
	UI_FM_RDS_MODE,
	UI_FM_SCAN_MODE,
	UI_FM_FAVO_LIST,
	UI_FM_FAVO_ENTRY
};
#endif

/* UI Test states */
enum
{
	
	UI_TEST_MAIN = UI_STATE_START(UI_TEST_ID),
	UI_TEST_AG_MAIN,
	UI_TEST_AG_CALL,
	UI_TEST_AG_AUDIO,
	UI_TEST_AG_MISC,
	UI_TEST_AG_API,
	UI_TEST_DM_MAIN,
	UI_TEST_DM1,
	UI_TEST_DM1_LP_ADDR,
	UI_TEST_DM1_LP_CS,
	UI_TEST_DM2,
	UI_TEST_DM3,
	UI_TEST_DM4,
	UI_TEST_DM4_SET_ADDR,
	UI_TEST_DM4_AUTH_REQ,
	UI_TEST_DG,
	UI_TEST_OPS,
	UI_TEST_OPC,
	UI_TEST_CT_MAIN,
	UI_TEST_CT1,
	UI_TEST_CT2,
	UI_TEST_CT3,
	UI_TEST_CT4,
	UI_TEST_CT5,
	UI_TEST_FT,
	UI_TEST_PR,
	UI_TEST_AC,
	UI_TEST_AA,
	UI_TEST_PAN,
	UI_TEST_HD,
	UI_TEST_HH,
	UI_TEST_HH_DATA,
	UI_TEST_CG,
	UI_TEST_AV_MAIN,
	UI_TEST_AV_C,
	UI_TEST_AV_A,
	UI_TEST_AV_V,
	UI_TEST_PBC,
	UI_TEST_PBC_ENTER_VALUE,
	UI_TEST_PBS,
	UI_TEST_AV_META
};

#define  BTUI_DEFAULT_INQ_DURATION     10 /* in 1.28 secs */

#define BTUI_NUM_REM_DEVICE 15


#define MENU_ITEM_0 '0'
#define MENU_ITEM_1 '1'
#define MENU_ITEM_2 '2'
#define MENU_ITEM_3 '3'
#define MENU_ITEM_4 '4'
#define MENU_ITEM_5 '5'
#define MENU_ITEM_6 '6'
#define MENU_ITEM_7 '7'
#define MENU_ITEM_8 '8'
#define MENU_ITEM_9 '9'
#define MENU_ITEM_A 'A'
#define MENU_ITEM_B 'B'
#define MENU_ITEM_C 'C'
#define MENU_ITEM_D 'D'
#define MENU_ITEM_E 'E'
#define MENU_ITEM_F 'F'
#define MENU_ITEM_H 'H'
#define MENU_ITEM_I 'I'

typedef void (*tBTUI_ACT_NO_PARAM)(void);
typedef void (*tBTUI_ACT_ONE_PARAM)(UINT32);

typedef struct _bt_ag_config
{
    BOOLEAN		ag_connect;
    BOOLEAN		ag_proceing;
    UINT8 ag_id;
}bt_ag_config;

typedef struct _bt_cod_type
{
    uint16            svc_cls_ptr;
    uint16            mjr_dev_cls_ptr;
    uint16            mnr_dev_cls_ptr;
}bt_cod_type;
/* remote device */
typedef struct
{
	BOOLEAN		in_use;
	BD_ADDR		bd_addr;
	char			name[BTUI_DEV_NAME_LENGTH+1];
	UINT8			pin_code[PIN_CODE_LEN]; 
	DEV_CLASS	dev_class;
	char        short_name[BTUI_DEV_NAME_LENGTH+1];      /* short name which user can assign to a device */
	LINK_KEY		link_key;
	UINT8			key_type;
	tBTA_IO_CAP peer_io_cap;
	BOOLEAN		link_key_present;
	BOOLEAN		is_trusted;
	tBTA_SERVICE_MASK trusted_mask;
	BOOLEAN		is_default;
	tBTA_SERVICE_MASK services;
	INT8			rssi;
	INT8			rssi_offset;
	UINT32          bond_time;
	bt_ag_config		ag_config;
	bt_cod_type cod_type;
} tBTUI_REM_DEVICE;

/* connected device */
typedef struct
{
	BOOLEAN    in_use;
	BD_ADDR    bd_addr;
	
} tBTUI_CONN_DEVICE;
#define BTUI_MAX_CONN_DEVICE    7

/* typedef for all data that application needs to store in nvram */
typedef struct
{
	BOOLEAN                  bcm_enabled;  /* bluetooth enabled or not */
	//UINT8                     count;
	char                      local_device_name[BTUI_DEV_NAME_LENGTH+1];     /* local bluetooth name */
	BOOLEAN                  visibility;
	tBTUI_REM_DEVICE      device[BTUI_NUM_REM_DEVICE];
	//TIMER_LIST_ENT        dev_tle;
}tBTUI_DEV_DB;

extern tBTUI_DEV_DB bt_ui_device_db;

#if( defined BTA_FM_INCLUDED ) && (BTA_FM_INCLUDED == TRUE)

#define BTUI_FM_MAX_FAVORITE        BTUI_NUM_REM_DEVICE
typedef struct
{
	UINT16              freq[BTUI_FM_MAX_FAVORITE];       /* frequency */
	UINT8               name[BTUI_FM_MAX_FAVORITE][10];   /* station name */
}
tBTUI_FM_DB;
extern tBTUI_FM_DB bt_ui_fm_db;
#endif

/* Inquiry results database */
typedef struct
{
	tBTUI_REM_DEVICE     remote_device[BTUI_NUM_REM_DEVICE];
	UINT8                    rem_index;
} tBTUI_INQ_DB;

extern tBTUI_INQ_DB bt_ui_inq_db;

#define BTUI_MAX_MENU_CHARACTERS_STORED 600
#define BTUI_MAX_FMT_TYPES    10

#define BTUI_SCREEN_LENGTH    16

typedef void (tBTUI_MENU)(void);
typedef void (tBTUI_INIT_FN)(void);
typedef void (tBTUI_EVENT_HDLR)(tBTUI_MMI_MSG *p_msg);
typedef void (tHT_KEY_PRESSED_CBACK)(UINT32 handle, UINT32 key);

typedef void (bt_ui_start_fun)(tBTUI_BTA_MSG *p_msg);
typedef struct  _bt_ui_start
{
    tBTUI_BTA_MSG msg;
    bt_ui_start_fun *fun;
}bt_ui_start;


/* BTUI main control block */
typedef struct
{
	tBTUI_STATE ui_state;                              /* current UI state */
	tBTUI_STATE ui_prev_state;                         /* previous UI state. Used to store the state when same menu 
																			is reached through different paths*/
	tBTUI_STATE ui_next_state;									/* next state for the UI This is used to store the
																		state when a event from BTA prempts the current state */

	//UINT8   ui_input_remaining;
	//tBTUI_MENU*  pin_cback_menu;
	//tBTUI_MENU*  search_cback_menu;
																			  
	BD_ADDR    local_bd_addr;                          /* local bdaddr */
	//char    bd_addr_str[BD_ADDR_LEN*3];                /* local bdaddr */
	
	//tBTA_SERVICE_MASK ui_current_active_connection;    /* active connection mask */
	tBTUI_REM_DEVICE * p_selected_rem_device;          /* pointer to device selected by UI */
	//char   *pr_doc_fmt_types;

#if (BTM_LISBON_INCLUDED == TRUE)
	BT_OCTET16 sp_c;
	BT_OCTET16 sp_r;
	BD_ADDR  oob_bdaddr;                               /* peer bdaddr stored for SP OOB process etc*/
	UINT32     pass_key;
	UINT8      notif_index;
	BOOLEAN    is_dd_bond;
	UINT8      sp_bond_bits;
	tBTA_IO_CAP    sp_io_cap;
	BD_ADDR    sp_bond_bdaddr;                         /* peer bdaddr stored for SP IO process etc*/
#endif
	BD_ADDR peer_bdaddr;                               /* peer bdaddr stored for pin_reply etc*/
	char     peer_name[BTUI_DEV_NAME_LENGTH+1];        /* bluetooth name of peer device for pin reply etc */
	DEV_CLASS peer_cod;
	BOOLEAN auth_pin_active;                      /* flag to indicate whether authorization
																		or pin request menu is active */	
	UINT32  auth_handle;                          /* handle of the por auth req/ pin req */
	
	tBTA_SERVICE_ID peer_service;                      /* service for authorization */
	//UINT8 num_devices;                                 /* num_devices in db */
	//UINT8 num_audio_devices;                           /* num audio devices in db */
	//UINT8 num_op_devices;                              /* num op devices in db */
	//UINT8 num_pr_devices;                              /* num printer devices in db */
	//UINT8 num_ct_devices;                              /* num CT devices in db */
	//UINT8 num_icom_devices;                            /* num ICOM devices in db */
	//UINT8 num_pan_devices;                             /* num PAN devices in db */
	//UINT8 num_hh_devices;                              /* num HID devices in db */
	
	//tBTUI_CONN_DEVICE     conn_dev[BTUI_MAX_CONN_DEVICE];
	//UINT8 num_conn_devices;                            /* num connected devices in db */
	//UINT8 sel_conn_devices;                            /* selected connected devices in db */
	
	tBTA_SERVICE_MASK search_services;                 /* services to search for */
	//char menu_store[BTUI_MAX_MENU_CHARACTERS_STORED];	/* current menu stored for redisplay */
	//UINT16 menu_indx;                                  /* index to the menu store to current position */
	//BOOLEAN	record_screen;										/* store the strings displayed to the menu store */
	//BOOLEAN	get_string;                               /* for next user input wait for a string input as opposed to a character input */
	//UINT32	idle_menu_handle;
	UINT32	search_msgbox_handle;
	UINT32	search_menu_handle;
	UINT32	current_menu_handle;
	UINT32	current_bonding_handle;
	UINT32	select_bonded_handle;
	UINT32	select_discover_handle;
	BOOLEAN	is_bonding;
	tBTUI_MENU *after_resume_menu;

	BOOLEAN  is_show_bonded_devices;
	tBTA_SERVICE_MASK show_bonded_service;
	
	//TIMER_LIST_Q  timer_queue;									/* timer queue */
	tHT_KEY_PRESSED_CBACK  * p_search_cback;
#if( defined BTA_DG_INCLUDED ) && (BTA_DG_INCLUDED == TRUE)
	BOOLEAN       loop_back_test_status;
#endif
	UINT8  menu_action;                                /* track the currently selected menu option */
#if BTA_DI_INCLUDED == TRUE
	tBTA_DISCOVERY_DB  *p_di_db;
	UINT32             di_handle;
#endif
#if (BTU_DUAL_STACK_INCLUDED == TRUE )
	BOOLEAN is_switched;
	BOOLEAN is_starting_stream;
	BOOLEAN manual_switch;
#endif
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)	
	char opp_send_file_addr[AEE_MAX_FILE_NAME];
	void * opp_send_file_p;
	BOOLEAN   recv_vcard_not_ack; 
#endif
	boolean m_bsend_opc_file;
	boolean b_opp_show_proc;
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
	boolean b_stop_op_file;
#endif
#ifdef FEATURE_KEYGUARD
	boolean b_auto_keyguard;
#endif

} tBTUI_CB;

extern tBTUI_CB bt_ui_cb;

//char * bt_ui_addr_str(UINT8 *p_addr);
//void bt_ui_fclose(FILE **pp_f);
//char * bt_ui_get_selected_dev_name(void);

//extern UINT8 bt_ui_device_db_to_menu_option_lkup_tbl[BTUI_NUM_REM_DEVICE];

//extern void bt_ui_app_start_timer (TIMER_LIST_ENT *p_tle, UINT16 type, UINT32 timeout);
//void bt_ui_app_stop_timer (TIMER_LIST_ENT *p_tle);

//void bt_ui_initial_msg (void);
void bt_ui_menu_idle (void);
void bt_ui_menu_setting (void);
#ifdef BT_BROADCOM_SAMPLE_CODE
void bt_ui_menu_view_devices (void);
#endif
//void bt_ui_act_show_new_devices (void);
void bt_ui_show_search_results (void);
//void bt_ui_display_stored_devices(void);
void bt_ui_search(tBTA_DM_INQ_MODE mode, tBTA_SERVICE_MASK services, tHT_KEY_PRESSED_CBACK  p_cback,tBTA_DM_COD_COND * inq_filt_cond);

//void BTUI_SetInputMode(BOOLEAN on);

/* functions for displaying the menus */
//void BTUI_putc(UINT8 ch);
//void BTUI_puts(const char *str);

//void BTUI_cls(void) ;



#define BTUI_NAME_LENGTH 32
#define BTUI_MAX_AUTH_KEY_LENGTH    16  /* must be <= BTA_FTS_MAX_AUTH_KEY_SIZE */
#define BTUI_MAX_AUTH_REALM_LENGTH 16  /* must be <= OBX_MAX_AUTH_REALM_SIZE */

#ifndef BTUI_MAX_PATH_LENGTH
#define BTUI_MAX_PATH_LENGTH         AEE_MAX_FILE_NAME	// 128
#endif

#define BTUI_MAX_LUID_LENGTH         100
#define BTUI_MIN_LUID_LENGTH         8
#define BTUI_MAX_DID_LENGTH          30
#define BTUI_SS_ROOT_FOLDER          "test_files/ss"


/* type for all configuartion parameters. */
typedef struct
{
	/* Mask for services supported by application */
	tBTA_SERVICE_MASK    supported_services;
	boolean              b_sdcard_state;
	/* event handler for BTUI device manager */
// 	tBTUI_EVENT_HDLR		*p_dm_event_hdlr;
	tBTA_DM_INQ_FILT     dm_inq_filt_type;
	tBTA_DM_INQ_COND     dm_inq_filt_cond;     /* Filter condition data. */
	
#if (BTM_LISBON_INCLUDED == TRUE)
	tBTA_AUTH_REQ    sp_auth_req;
#endif
#if( defined BTA_AG_INCLUDED ) && (BTA_AG_INCLUDED == TRUE)	
 	tBTUI_INIT_FN        * p_ag_init;
// 	tBTUI_MENU				* p_ag_menu;     
// 	tBTUI_EVENT_HDLR     * p_ag_event_hdlr;/* event handler for BTUI Audio Gateway */

	/* AG configuartion parameters */
	tBTA_SEC					ag_security;
	char                 hsag_service_name[BTUI_NAME_LENGTH + 1];
	char                 hfag_service_name[BTUI_NAME_LENGTH + 1];
	int                  ag_features;
	int                  ag_instances;
	BOOLEAN              ag_bldn_enable;	// place call to last dialed number
#endif
#if( defined BTA_DG_INCLUDED ) && (BTA_DG_INCLUDED == TRUE)
	/* DG configuartion parameters */
	char						dg_port_name[BTUI_NAME_LENGTH + 1];
	char                 dg_port_baud[BTUI_NAME_LENGTH + 1];
	tBTA_SEC             sppdg_security;
	char                 sppdg_service_name[BTUI_NAME_LENGTH + 1];
	tBTA_SEC             dundg_security;
	char                 dundg_service_name[BTUI_NAME_LENGTH + 1];
	tBTA_SEC             faxdg_security;
	char                 faxdg_service_name[BTUI_NAME_LENGTH + 1];
	
	BOOLEAN              spp_loopback_mode;
	BOOLEAN              spp_senddata_mode;
	
	/* DG client configuartion parameters */
	char                 dg_client_port_name[BTUI_NAME_LENGTH + 1];
	char                 dg_client_port_baud[BTUI_NAME_LENGTH + 1];
#endif
	char                 dg_client_service_id[2];
#if( defined BTA_DG_INCLUDED ) && (BTA_DG_INCLUDED == TRUE)
	tBTA_SEC             dg_client_security;
	char                 dg_client_peer_name[BTUI_NAME_LENGTH + 1];

 	tBTUI_INIT_FN        * p_dg_init;
 	//tBTUI_MENU				* p_dg_menu;
 	//tBTUI_EVENT_HDLR     * p_dg_event_hdlr;
#endif	
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
	/* OPS configuration parameters */
	BOOLEAN              ops_included;
 	tBTUI_INIT_FN        * p_ops_init;
	tBTA_SEC             ops_security;
	char                 ops_service_name[BTUI_NAME_LENGTH + 1];
	
	/* OPC configuration parameters */
	BOOLEAN              opc_included;
 	tBTUI_INIT_FN        * p_opc_init;
// 	tBTUI_MENU				* p_opc_menu;
// 	tBTUI_EVENT_HDLR		* p_opc_event_hdlr;
	tBTA_SEC             opc_security;
	BOOLEAN              opc_single_op;
	char                 opc_service_name[BTUI_NAME_LENGTH + 1];
#endif
	
#if( defined BIP_PRINTING_INCLUDED ) && (BIP_PRINTING_INCLUDED == TRUE)
	/* Print configuration parameters */
	BOOLEAN              pr_included;
// 	tBTUI_INIT_FN        * p_pr_init;
// 	tBTUI_MENU				* p_pr_menu;
// 	tBTUI_EVENT_HDLR     * p_pr_event_hdlr;
	tBTA_SEC             pr_security;
	char                 pr_service_name[BTUI_NAME_LENGTH + 1];
	char                 pr_password[BTUI_MAX_AUTH_KEY_LENGTH + 1];
	char                 pr_userid[BTUI_MAX_AUTH_KEY_LENGTH + 1];
#endif 
	
#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
	/* FTS configuration parameters */
	BOOLEAN              fts_included;
 	//tBTUI_EVENT_HDLR     * p_fts_event_hdlr;
 	tBTUI_INIT_FN        * p_fts_init;
 	//tBTUI_MENU				* p_fts_menu;
	char                 fts_service_name[BTUI_NAME_LENGTH + 1];
	char                 fts_key [BTUI_MAX_AUTH_KEY_LENGTH + 1];
	char                 fts_realm [BTUI_MAX_AUTH_REALM_LENGTH + 1];
	BOOLEAN              fts_obex_auth;
	tBTA_SEC             fts_security;
#endif
	
#if( defined BTA_PBS_INCLUDED ) && (BTA_PBS_INCLUDED == TRUE)
	/* PBS configuration parameters */
	BOOLEAN              pbs_included;
// 	tBTUI_EVENT_HDLR     * p_pbs_event_hdlr;
// 	tBTUI_INIT_FN        * p_pbs_init;
// 	tBTUI_MENU				* p_pbs_menu;
	char                 pbs_service_name[BTUI_NAME_LENGTH + 1];
	char                 pbs_key [BTUI_MAX_AUTH_KEY_LENGTH + 1];
	char                 pbs_realm [BTUI_MAX_AUTH_REALM_LENGTH + 1];
	BOOLEAN              pbs_obex_auth;
	tBTA_SEC             pbs_security;
#endif 
	
#if( defined BTA_FT_INCLUDED ) && (BTA_FT_INCLUDED == TRUE)
	/* FTC configuration parameters */
	BOOLEAN              ftc_included;
 	tBTUI_EVENT_HDLR     * p_ftc_event_hdlr;
 	tBTUI_INIT_FN        * p_ftc_init;
 	tBTUI_MENU				* p_ftc_menu;
	char                 ftc_password[BTUI_MAX_AUTH_KEY_LENGTH + 1];
	char                 ftc_userid[BTUI_MAX_AUTH_KEY_LENGTH + 1];
	tBTA_SEC             ftc_security;
	tBTA_SERVICE_MASK    ftc_services;
#endif
	
#if( defined BTA_PBC_INCLUDED ) && (BTA_PBC_INCLUDED == TRUE)
	/* PBC configuration parameters */
	BOOLEAN              pbc_included;
// 	tBTUI_EVENT_HDLR     * p_pbc_event_hdlr;
// 	tBTUI_INIT_FN        * p_pbc_init;
// 	tBTUI_MENU				* p_pbc_menu;
	char                 pbc_password[BTUI_MAX_AUTH_KEY_LENGTH + 1];
	char                 pbc_userid[BTUI_MAX_AUTH_KEY_LENGTH + 1];
	tBTA_SEC             pbc_security;
	UINT32               pbc_filter;
	UINT8                pbc_format;
	UINT8                pbc_order;
	UINT8                pbc_attr;
	UINT16               pbc_max_list_count;
	UINT16               pbc_offset;
#endif
	
#if( defined BTA_AC_INCLUDED ) && (BTA_AC_INCLUDED == TRUE)
	/* ACS configuration parameters */
	BOOLEAN              acs_included;
// 	tBTUI_EVENT_HDLR     * p_acs_event_hdlr;
// 	tBTUI_INIT_FN        * p_acs_init;
// 	tBTUI_MENU	         * p_acs_menu;
	char                 acs_service_name[BTUI_NAME_LENGTH + 1];
	tBTA_SEC             acs_security;
	BOOLEAN              acs_req_thumb; /* request for the thumbnail version on PutImage */
//#if( defined BTA_AC_INCLUDED ) && (BTA_AC_INCLUDED == TRUE)
	tBIP_FEATURE_FLAGS	acs_features;  /* supported features */
	tBIP_UINT64          acs_capacity;  /* total image capacity(in bytes) */
	char						acs_path[BTUI_MAX_PATH_LENGTH + 1];
	tBIP_IMG_HDL_STR		acs_del_img_hdl;    /* the image handle */
//#endif    
	UINT8						acs_del_bi_hdl;
	
	/* ACC configuration parameters */
	BOOLEAN					acc_included;
// 	tBTUI_EVENT_HDLR		* p_acc_event_hdlr;
// 	tBTUI_INIT_FN			* p_acc_init;
// 	tBTUI_MENU				* p_acc_menu;
	tBTA_SEC					acc_security;
	BOOLEAN					acc_cam_get_image;
	char						acc_path[BTUI_MAX_PATH_LENGTH + 1];
	char						acc_password[BTUI_MAX_AUTH_KEY_LENGTH + 1];
	char						acc_userid[BTUI_MAX_AUTH_KEY_LENGTH + 1];
#endif
	
#if( defined BTA_SS_INCLUDED ) && (BTA_SS_INCLUDED == TRUE)
	/* SS configuration parameters */
	BOOLEAN					ss_included;
// 	tBTUI_EVENT_HDLR		* p_ss_event_hdlr;
// 	tBTUI_INIT_FN			* p_ss_init;
// 	tBTUI_MENU				* p_ss_menu;
	char						ss_service_name[BTUI_NAME_LENGTH + 1];
	char						ss_key [BTUI_MAX_AUTH_KEY_LENGTH + 1];
	char						ss_realm [BTUI_MAX_AUTH_REALM_LENGTH + 1];
	BOOLEAN					ss_obex_auth;
	tBTA_SEC					ss_security;
	tBTA_SS_DATA_TYPE_MASK	ss_data_types;
	int						ss_luid_size;
	int						ss_cl_size;
	int						ss_did_size;
#endif
	
#if( defined BTA_SC_INCLUDED ) && (BTA_SC_INCLUDED == TRUE)
	/* SC configuration parameters */
	BOOLEAN					sc_included;
// 	tBTUI_EVENT_HDLR		* p_sc_event_hdlr;
// 	tBTUI_INIT_FN			* p_sc_init;
// 	tBTUI_MENU				* p_sc_menu;
	char						sc_service_name[BTUI_NAME_LENGTH + 1];
	tBTA_SEC					sc_security;
#endif
		
#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
   /* CT configuration parameters */
 	tBTUI_INIT_FN			* p_ct_init;
 	//tBTUI_MENU				* p_ct_menu;
 	//tBTUI_EVENT_HDLR		* p_ct_event_hdlr;
	tBTA_SEC					ct_security;
#endif
	
#if( defined PAN_INCLUDED ) && (PAN_INCLUDED == TRUE)	
	/* PAN configuration parameters */
	BOOLEAN					pan_included;
	BOOLEAN					panu_supported;
	BOOLEAN					pangn_supported;
	BOOLEAN					pannap_supported;
// 	tBTUI_INIT_FN			* p_pan_init;
// 	tBTUI_MENU				* p_pan_menu;
// 	tBTUI_EVENT_HDLR		* p_pan_event_hdlr;
	tBTA_SEC					pan_security;
	char						panu_service_name[BTUI_NAME_LENGTH + 1];
	char						pangn_service_name[BTUI_NAME_LENGTH + 1];
	char						pannap_service_name[BTUI_NAME_LENGTH + 1];
	char						pan_port_name[BTUI_NAME_LENGTH + 1];
	char						pan_port_baud[BTUI_NAME_LENGTH + 1];
#endif
	
#if( defined BTA_AA_INCLUDED ) && (BTA_AA_INCLUDED == TRUE)
	/* AA configuration parameters */
	BOOLEAN					aa_included;
// 	tBTUI_EVENT_HDLR		* p_aa_event_hdlr;
// 	tBTUI_INIT_FN			* p_aa_init;
// 	tBTUI_MENU				* p_aa_menu;
	char						aa_service_name[BTUI_NAME_LENGTH + 1];
	tBTA_SEC					aa_security;
//#if( defined BTA_AA_INCLUDED ) && (BTA_AA_INCLUDED == TRUE)
	tBTA_AA_FEAT			aa_features;
//#endif
	UINT16					aa_line_speed_kbps;
	UINT16					aa_line_speed_busy;
	UINT16					aa_line_speed_swampd;
	UINT8						aa_channel_mode;
#endif
	
#if( defined BTA_AV_INCLUDED ) && (BTA_AV_INCLUDED == TRUE)
   /* AV configuration parameters */
	BOOLEAN					av_included;
// 	tBTUI_EVENT_HDLR		* p_av_event_hdlr;
// 	tBTUI_INIT_FN			* p_av_init;
// 	tBTUI_MENU				* p_av_menu;
	tBTA_SEC					av_security;
	BOOLEAN					use_avrc;
	tBTA_AV_FEAT			av_features;
	UINT16					av_line_speed_kbps;
	UINT16					av_line_speed_busy;
	UINT16					av_line_speed_swampd;
	UINT8						av_channel_mode;
	BOOLEAN					av_repeat_list;
	char						av_audio_service_name[BTUI_NAME_LENGTH + 1];
	char						av_video_service_name[BTUI_NAME_LENGTH + 1];
	tBTA_SERVICE_MASK		av_services_used;                      /* Mask of services to use when printing */
	BOOLEAN					av_vdp_support;
	UINT8						av_num_audio;
	BOOLEAN					av_use_player;
#if (BTU_DUAL_STACK_INCLUDED == TRUE )
	BOOLEAN					av_auto_switch;
#endif
	UINT8						av_min_trace;
	BOOLEAN					av_suspd_rcfg;
	BOOLEAN					av_group_supported;
	UINT8						av_max_group;
	char						av_group_1[BTUI_NAME_LENGTH + 1];
	char						av_group_2[BTUI_NAME_LENGTH + 1];
	char						av_group_3[BTUI_NAME_LENGTH + 1];
#endif
	
#if( defined BTA_HD_INCLUDED ) && ( BTA_HD_INCLUDED == TRUE )
	/* HD configuration parameters */
	BOOLEAN					hd_included;
// 	tBTUI_INIT_FN			* p_hd_init;
// 	tBTUI_MENU				* p_hd_menu;
// 	tBTUI_EVENT_HDLR		* p_hd_event_hdlr;
	char						hd_service_name[BTUI_NAME_LENGTH + 1];
	tBTA_SEC					hd_security;
#endif
	
#if( defined BTA_HH_INCLUDED ) && ( BTA_HH_INCLUDED == TRUE )
	/* HH configuration parameters */
	BOOLEAN					hh_included;
// 	tBTUI_INIT_FN			* p_hh_init;
// 	tBTUI_MENU				* p_hh_menu;
// 	tBTUI_EVENT_HDLR		* p_hh_event_hdlr;
	char						hh_service_name[BTUI_NAME_LENGTH + 1];
	tBTA_SEC					hh_security;
#endif
	
	/* number of devices to be found during inquiry */
	UINT32					num_inq_devices;
		
	/* TEST configuration parameters */
	BOOLEAN					test_included;
// 	tBTUI_EVENT_HDLR		* p_test_event_hdlr;
// 	tBTUI_INIT_FN			* p_test_init;
// 	tBTUI_MENU				* p_test_menu;
	//char						test_bd_addr1[BD_ADDR_LEN + 1];
	//char						test_bd_addr2[BD_ADDR_LEN + 1];
	
	char						root_path[BTUI_MAX_PATH_LENGTH];/* "/brew/" */
	
	/* type of UI, txt based or hset gui */
	BOOLEAN					hset_ui_mode;
	
#if( defined BTA_FM_INCLUDED ) && (BTA_FM_INCLUDED == TRUE)
	/* FM run time configuration */
	BOOLEAN					fm_included;
// 	tBTUI_EVENT_HDLR		* p_fm_event_hdlr;
// 	tBTUI_INIT_FN			* p_fm_init;
// 	tBTUI_MENU				* p_fm_menu;
	UINT8						fm_func_mask;
	UINT8						fm_rssi_mono;
	UINT8						fm_rssi_stereo;
	BOOLEAN					fm_live_update;
#endif
#if( defined BTA_RECONFIG_RATE_INCLUDED ) && (BTA_RECONFIG_RATE_INCLUDED == TRUE)
	/* reconfig baud rate */
	UINT8						reconfig_baudrate;
	
	/* uart clock rate */
	UINT32					uart_clock_rate;
#endif
#if (BTU_DUAL_STACK_INCLUDED == TRUE )
	/* mm baud rate */
	UINT8						mm_baudrate;
#endif
#if( defined BTA_LPM_INCLUDED ) && (BTA_LPM_INCLUDED == TRUE)
	/* patch ram */
	BOOLEAN					patchram_enable;
	char						patchram_path[BTUI_MAX_PATH_LENGTH];
	
	/* sleep mode */
	UINT8						lpm_sleep_mode; /* set it by HCISU config */
	UINT8						lpm_host_stack_idle_threshold;
	UINT8						lpm_host_controller_idle_threshold;
	UINT8						lpm_bt_wake_polarity;
	UINT8						lpm_host_wake_polarity;
	UINT8						lpm_allow_host_sleep_during_sco;
	UINT8						lpm_combine_sleep_mode_and_lpm;
	UINT8						lpm_enable_uart_txd_tri_state;
	UINT8						lpm_active_connection_handling;
	UINT8						lpm_resume_timeout;
#endif
} tBTUI_CFG;

extern tBTUI_CFG bt_ui_cfg;

#if( defined BTA_HH_INCLUDED ) && (BTA_HH_INCLUDED == TRUE)
/* test data for HH SetReport */
#define BTUI_MAX_HH_TEST_DATA_LEN   200
#define BTUI_MAX_HH_TEST_DATA_NUM   30

typedef struct
{
	char    data_payload[BTUI_MAX_HH_TEST_DATA_LEN];
	tBTA_HH_RPT_TYPE   rpt_type;
	UINT8               rpt_id;
} tBTUI_HH_TEST_DATA;

typedef struct
{
	UINT8   test_option_num;
	tBTUI_HH_TEST_DATA  test_data[BTUI_MAX_HH_TEST_DATA_NUM];    
} tBTUI_HH_CFG;

extern tBTUI_HH_CFG bt_ui_hh_cfg;
#endif	// BTA_HH_INCLUDED

#if( defined BTA_DG_INCLUDED ) && (BTA_DG_INCLUDED == TRUE)
extern void bt_ui_dg_init(void);
extern void bt_ui_dg_platform_init(void);
extern void bt_ui_menu_dg_connections(void);
#endif
#if( defined BTA_AG_INCLUDED ) && ( BTA_AG_INCLUDED == TRUE )
#ifdef BT_BROADCOM_SAMPLE_CODE
void bt_ui_menu_audio_devices (void);
#endif
void bt_ui_ag_init(void);
void bt_ui_ag_disable(void);
void bt_ui_act_audio_device_connect(void);
void bt_ui_act_audio_device_disconnect(void);
void bt_ui_ag_make_quicktest_call(void);
//void bt_ui_ag_handle_call_event(UINT16 handle, UINT8 event, UINT8 app_id);
#endif
#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
//extern void bt_ui_fts_init(void);
#ifdef BT_BROADCOM_SAMPLE_CODE
void btui_menu_fts_main(void);
#endif
void bt_ui_fts_init(void);
void bt_ui_fts_disable(void);
#if( defined BTA_PBS_INCLUDED ) && (BTA_PBS_INCLUDED == TRUE)
extern void btapp_pbs_init(void);
#endif	// BTA_PBS_INCLUDED
#endif	// BTA_FS_INCLUDED

#if( defined BTA_FT_INCLUDED ) && (BTA_FT_INCLUDED == TRUE)
extern void bt_ui_ftc_init(void);
#if( defined BTA_PBC_INCLUDED ) && (BTA_PBC_INCLUDED == TRUE)
extern void bt_ui_pbc_init(void);
#endif	// BTA_PBC_INCLUDED
extern UINT32 bt_ui_ftc_get_file_size(char *p_name);
#endif	// BTA_FT_INCLUDED

#if( defined BTA_AC_INCLUDED ) && (BTA_AC_INCLUDED == TRUE)
extern void bt_ui_acs_init(void);
extern void bt_ui_acc_init(void);
extern void bt_ui_acc_disable(void);
extern void bt_ui_act_init(tBTUI_MMI_MSG *p_msg);
#endif

#if( defined BTA_SS_INCLUDED ) && (BTA_SS_INCLUDED == TRUE)
extern void bt_ui_ss_init(void);
#endif

void bt_ui_cfg_init(void);

void bt_ui_startup(void);

void bt_ui_dm_init(void);
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
void bt_ui_ops_init(void);
void bt_ui_ops_disable(void);
void bt_ui_opc_init(void);
void bt_ui_opc_disable(void);
void bt_ui_menu_opc_devices (void);
boolean bt_ui_opp_test_file_addr(char * file_addr);
void bt_ui_act_send_file_ex (void);
void bt_ui_act_find_op_devices(void);
#endif
#if( defined BTA_PR_INCLUDED ) && (BTA_PR_INCLUDED == TRUE)
extern void bt_ui_pr_init(void);
#endif

#if( defined BTA_SC_INCLUDED ) && (BTA_SC_INCLUDED == TRUE)
extern void bt_ui_sc_init(void);            /* SIM card server */
#endif

#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
void bt_ui_ct_init(void);
void bt_ui_menu_ct_devices (void);
void bt_ui_ct_disable(void);
#endif

#if( defined BTA_PAN_INCLUDED ) && (BTA_PAN_INCLUDED == TRUE)
extern void bt_ui_pan_init(void);
#endif

#if( defined BTA_AA_INCLUDED ) && (BTA_AA_INCLUDED == TRUE)
extern void bt_ui_aa_init(void);
#endif

#if( defined BTA_AV_INCLUDED ) && (BTA_AV_INCLUDED == TRUE)
extern void bt_ui_av_init(void);
#endif

#if( defined BTA_HD_INCLUDED ) && (BTA_HD_INCLUDED == TRUE)
extern void bt_ui_hd_init(void);
#endif

#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
extern void bt_ui_hh_init(void);
#endif

#if( defined BTA_FM_INCLUDED ) && (BTA_FM_INCLUDED == TRUE)
extern void bt_ui_fm_init(void);
#endif

//void bt_ui_test_init(void);

//char *bt_ui_opc_platform_get_path(UINT8 id, char **p_name);
//int bt_ui_platform_opc_list_files(void);

//int bt_ui_platform_filemenu(char menu_name[], const char filepath[], tHT_KEY_PRESSED_CBACK menu_cback);
//char *bt_ui_platform_pr_filemenu_get_sel(UINT32 h_menu);

//boolean bt_ui_process_cmcall_notify(void *pp,uint32 event,void  *pCallInfo);
boolean bt_ui_process_cmss_notify  (void  *pssInfo);
//extern const char BTUI_PR_FILEPATH[];

#if 0
enum
{
	BTUI_IMG_ERR_NONE,    /* OK */
	BTUI_IMG_ERR_EOF,     /* EOF */
	BTUI_IMG_ERR_BAD      /* bad format */
};
extern UINT8 bt_ui_jpg_scan_header (char *p_name, UINT16 *p_height, UINT16 *p_width);
extern UINT8 bt_ui_bmp_scan_header (char *p_name, UINT16 *p_height, UINT16 *p_width);
extern UINT8 bt_ui_gif_scan_header (char * p_name, UINT16 *p_height, UINT16 *p_width);
#endif

#if (defined BTA_FT_INCLUDED) && (BTA_FT_INCLUDED == TRUE)
extern void bt_ui_ftc_platform_get_thumb(char *path, char *p_name);	
extern BOOLEAN bt_ui_ftc_platform_get_path(UINT8 id, char *p_path, tBTA_FTC_PARAM *p_param);
extern void bt_ui_ftc_platform_get_root(char *p_path);
extern int  bt_ui_platform_ftc_list_files(UINT8 type);
#endif

#if (defined BTA_FS_INCLUDED) && (BTA_FS_INCLUDED == TRUE)
//extern void bt_ui_ftc_platform_get_root(char *p_path);
#endif


#if (defined BTA_PBC_INCLUDED) && (BTA_PBC_INCLUDED == TRUE)
extern void bt_ui_pbc_platform_get_root(char *p_path);
#endif

enum
{
	BTUI_FILE_ANY,  /* any kind of file */
	BTUI_FILE_IMAGE /* image files */
};

UINT8 bt_ui_display_devices(tBTA_SERVICE_MASK services, tHT_KEY_PRESSED_CBACK  p_menu_callback, TCHAR* sft_key2_msg);
//UINT8 bt_ui_display_bonded_devices(tHT_KEY_PRESSED_CBACK p_menu_callback, TCHAR* sft_key2_msg);
//void bt_ui_show_new_devices (tHT_KEY_PRESSED_CBACK  p_menu_callback);
//extern void bt_ui_act_delete_all_stored_device(void);
void bt_ui_print_btaddr(BD_ADDR    bd_addr);
void bt_ui_enable_backlight(void);
/* functions related to device data base */
void bt_ui_init_device_db (void);
void bt_ui_store_visibility_setting(BOOLEAN visibility);
void bt_ui_store_bt_enable_setting(BOOLEAN enabled);
void bt_ui_store_local_name(char * p_name);
void bt_ui_nv_init_device_db(void);
void bt_ui_nv_store_device_db(void);

#if 0
extern void bt_ui_nv_init_wug_db(void);
extern void bt_ui_nv_store_wug_db(void);
#endif

#if( defined BTA_FM_INCLUDED ) && (BTA_FM_INCLUDED == TRUE)
extern void bt_ui_nv_store_fm_db(void);
extern void bt_ui_nv_init_fm_db(void);
#endif

tBTUI_REM_DEVICE * bt_ui_get_inquiry_record(BD_ADDR bd_addr);
tBTUI_REM_DEVICE * bt_ui_get_device_record(BD_ADDR bd_addr);
tBTUI_REM_DEVICE * bt_ui_alloc_device_record(BD_ADDR bd_addr);
//char * bt_ui_get_dev_name(BD_ADDR bd_addr);
BOOLEAN bt_ui_store_device( tBTUI_REM_DEVICE * p_rem_device);
//BOOLEAN bt_ui_store_device_ex( tBTUI_REM_DEVICE * p_rem_device);
BOOLEAN bt_ui_delete_device(BD_ADDR bd_addr);
//void bt_ui_delete_all_stored_device(void);

/* Other functions. */


//void ascii_2_bdaddr (char *p_ascii, UINT8 *p_bd);
//void bt_ui_proc_link_change(BD_ADDR bd_addr, BOOLEAN is_up);

//extern void bt_ui_platform_startup(void);
//void bt_ui_platform_load_configuration(void);

//void bt_ui_platform_set_root_path(char *root_path, UINT16 path_len);

//void bt_ui_platform_load_hh_test_data(void);


/* BTUI functions for displaying menus and message boxes */

/* BTUI key map. */
#ifndef AEE_SIMULATOR
#define HT_KEY_SOFT1		AVK_SELECT
#define HT_KEY_SOFT2		AVK_CLR
#define HT_KEY_SELECT	AVK_INFO
#else
#define HT_KEY_SOFT1		AVK_SOFT1
#define HT_KEY_SOFT2		AVK_SOFT2
#define HT_KEY_SELECT	AVK_SELECT
#endif

/* bt_ui edit box input mode */
#define HT_EB_LETTERS	AEE_TM_LETTERS
#define HT_EB_NUMBERS	AEE_TM_NUMBERS
#define HT_EB_PINYIN		AEE_TM_PINYIN
#define HT_EB_STROKE		AEE_TM_STROKE
#define HT_EB_SYMBOLS	AEE_TM_SYMBOLS

/* bt_ui NULL handle macro */
#define BT_UI_NULL_HANDLE	0

// Messgae box time out period
#define BTUI_TIMEOUT_MSG_PERIOD      2000

/*menu item*/
typedef struct tHT_MENU_ITEM
{
	const char *p_item_name;
	const TCHAR *p_w_item_name;
	UINT32 item_data;
	UINT16 item_index;
	UINT16 item_string_id;
#ifdef FEATURE_BLUETOOTH_DEVICE_IMAGE	
	UINT16 item_img_id;
#endif	
}tHT_MENU_ITEM;

// The default menu callback funtion, the item data is the handle function's ptr.
void bt_ui_menu_cback(UINT32 handle, UINT32 key);
void bt_ui_menu_cback_for_discovery_mode(UINT32 handle, UINT32 key);
void bt_ui_set_draw_hold_down(BOOLEAN f_hold_down);
#ifdef AEE_SIMULATOR
/* Set the softkeys of a window. */
void HT_SetSoftKeyMessages(UINT32 handle, BottomBar_e_Type win_sk);
#endif
UINT32 bt_ui_edit_box(UINT32 flags, tHT_KEY_PRESSED_CBACK *p_cback, TCHAR *p_prompt_text, UINT16 maz_size,BottomBar_e_Type soft_key,char* old_name,boolean is_edit_name);
#ifdef UI_DM_EDIT_NAME_EX
UINT32 bt_ui_edit_box_edit_name(UINT32 flags, tHT_KEY_PRESSED_CBACK *p_cback, TCHAR *pw_prompt_text, UINT16 max_size,BottomBar_e_Type soft_key,char* old_name);
#endif
UINT32 bt_ui_message_box(TCHAR *p_text, tHT_KEY_PRESSED_CBACK *p_cback,BottomBar_e_Type soft_key);
UINT32 bt_ui_message_box_ex(UINT16 string_id, tHT_KEY_PRESSED_CBACK *p_cback,BottomBar_e_Type soft_key);
UINT32 bt_ui_message_box_timeout(TCHAR *p_text, tHT_KEY_PRESSED_CBACK *p_cback, INT32 dwMSecs,BottomBar_e_Type soft_key);

UINT32 bt_ui_create_menu(tHT_MENU_ITEM *p_menu_items, UINT16 menu_item_count,
										 tHT_KEY_PRESSED_CBACK *p_cback,  TCHAR *p_prompt_text   ,BottomBar_e_Type BottomBar,boolean is_option,uint32 dwProps);
UINT32 bt_ui_create_static(TCHAR *string ,  tHT_KEY_PRESSED_CBACK *p_cback,  UINT16 title  ,BottomBar_e_Type BottomBar );
//UINT32 bt_ui_create_top_menu(tHT_MENU_ITEM *p_menu_items, UINT16 menu_item_count,
//											  tHT_KEY_PRESSED_CBACK *p_cback,  TCHAR *p_prompt_text);
void bt_ui_set_menu_sel(uint16 sel);
//uint16 bt_ui_get_menu_sel(void);
void bt_ui_set_pop_msg_type(PromptMsg_e_Type sel);
void bt_ui_close_object(UINT32 handle);

//void HT_SetEditBoxMaxChars(UINT32 handle, uint16 nMaxChars);
TCHAR* HT_GetEditBoxText(UINT32 handle);
AECHAR * bt_ui_get_brx_string(uint16 string_id);
UINT32 HT_GetCurrMenuHandle(void);
int HT_GetMenuSelectedItem(UINT32 handle, tHT_MENU_ITEM *p_menu_item);
void HT_AddMenuItems(UINT32 handle, tHT_MENU_ITEM *p_menu_items, UINT16 menu_item_count);
//void HT_DeleteMenuItem(UINT32 handle, UINT16 index);
void HT_ClearMenuItems(UINT32 handle);
void HT_UpdateHandle(UINT32 handle);
void HT_UpdateDisplay(void);

//UINT32 HT_MessageBox(TCHAR *p_text);
UINT32 HT_MessageBoxTimeout(TCHAR *p_text, INT32 dwMSecs);
UINT32 HT_MessageBoxTimeout_ex(UINT16 string_id, INT32 dwMSecs);
UINT32 HT_MessageBoxProcess(TCHAR *p_text, UINT32 bytes_transferred,UINT32 obj_size,tHT_KEY_PRESSED_CBACK *p_cback,BottomBar_e_Type BottomBar);
//UINT32 HT_GetCurrHandle(void);
BOOLEAN HT_IsActiveHandle(UINT32 handle);
//void Ht_RemovePromptWindow(void);
void bt_ui_restore_set(void);

/* Call Table related changes */
//enum
//{
//	BTUI_CALL_STATE_NONE,       /* Call doesn't exist */
//	BTUI_CALL_STATE_INCOMING,   /* Call is in incoming state */
//	BTUI_CALL_STATE_OUTGOING,   /* Call is in outgoing state */
//	BTUI_CALL_STATE_ACTIVE,     /* Call is active */
//	BTUI_CALL_STATE_ACTIVE_HELD /* Call is active and held */
//};

//enum
//{
//	BTUI_CALL_DIR_MO,           /* Mobile Originated Call */
//	BTUI_CALL_DIR_MT            /* Mobile Terminated Call */
//};

//#define BTUI_MAX_DIAL_DIGITS            15
//#define BTUI_MAX_CALLS                  3

//typedef struct
//{
//    BOOLEAN         is_in_use;
//    char            dial_num[BTUI_MAX_DIAL_DIGITS+1]; /* For dial cmd, copy the number HF sends,
//    * for other cases use dial_num_to_use */
//    UINT8           call_index;
//    UINT8           call_state;
//    UINT8           call_dir;
//    char             dial_num_to_use[BTUI_MAX_DIAL_DIGITS+1];/*default number to dial*/
//    UINT8           call_id;/*this item is get from oemcm*/
    /* This is used to generate an outgoing call or an incoming call */
//} tBTUI_CALL_DATA;
#if 0
//extern tBTUI_CALL_DATA	bt_ui_call_table[];
extern tBTUI_REM_DEVICE stored_devices_list[BTUI_NUM_REM_DEVICE];

extern void bt_ui_ag_get_call_stats(UINT8 *num_active, UINT8 *num_held);
extern void bt_ui_ag_release_calls_in_state(UINT8 call_state);
extern tBTUI_CALL_DATA* bt_ui_ag_get_call_table_entry(void);
extern tBTUI_CALL_DATA* bt_ui_ag_get_call_data_by_index(UINT8 index);
extern tBTUI_CALL_DATA* bt_ui_ag_get_call_data_in_call_setup(void);
extern BOOLEAN bt_ui_ag_is_in_call_setup(void);
//extern void bt_ui_ag_reset_call_table(void);
extern void bt_ui_ag_calc_held_ind(void);
extern void bt_ui_ag_release_call_by_index(UINT8 index);
extern void siors_uart_enable_warmup(boolean val);
#endif
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
#define BT_OPS_PULL_ADDR  "address/"
#define BT_OPS_PUSH_ADDR "address/"
#define BT_OPC_PULL_ADDR  "address/"
#define BT_OPC_PUSH_ADDR "address/"
#endif

#ifdef AEE_SIMULATOR
#define BT_DEV_DB_FILE	"fs:/shared/bt_dev_db.dat"
#else
#define BT_DEV_DB_FILE	"fs:/cfg/bt_dev_db.dat"
#endif
#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
#define BT_WUB_DB_FILE	"fs:/cfg/bt_wub_db.dat"
#endif
#endif /*BTUI_INT_H*/
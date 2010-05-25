/****************************************************************************
**                                                                           
**  Name:          bt_ui_cfg.c                                           
**                 
**  Description:   Functions which sets the configuration for the application
**                 
**                                                                           
**  Copyright (c) 2002-2004, Widcomm Inc., All Rights Reserved.              
**  Widcomm Bluetooth Core. Proprietary and confidential.                    
******************************************************************************/

#include "bt_target.h"
#include "bta_api.h"

#include "bt_ui.h"
#include "bt_ui_int.h"
//#include "btsim_api.h"
#include "hcisu.h"
#include "hcis_h4.h"
#include <stdio.h>
#include <string.h>
#include "bcmapp_dm.h"
 
#if( defined BTA_AG_INCLUDED ) && ( BTA_AG_INCLUDED == TRUE )
#include "bta_ag_api.h"
#endif
#if( defined BTA_PR_INCLUDED ) && (BTA_PR_INCLUDED == TRUE)
#include "bcmapp_pr.h"
#endif
#if( defined BTA_AV_INCLUDED ) && (BTA_AV_INCLUDED == TRUE)
#include "bcmapp_av.h"
#endif
#if( defined BTA_PBC_INCLUDED ) && (BTA_PBC_INCLUDED == TRUE)
#include "bta_pbc_api.h"
#endif

/* Local Constants */
#ifndef BTUI_AG_SECURITY
#define BTUI_AG_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif
#ifndef BTUI_HSAG_SERVICE_NAME
#define BTUI_HSAG_SERVICE_NAME "Headset Gateway"
#endif
#ifndef BTUI_HFAG_SERVICE_NAME
#define BTUI_HFAG_SERVICE_NAME "Handsfree Gateway"
#endif
#ifndef BTUI_AG_FEATURES
#define BTUI_AG_FEATURES (BTA_AG_FEAT_3WAY | BTA_AG_FEAT_ECNR | BTA_AG_FEAT_VREC | \
                          BTA_AG_FEAT_INBAND | BTA_AG_FEAT_VTAG | BTA_AG_FEAT_VTAG | \
                          BTA_AG_FEAT_REJECT | BTA_AG_FEAT_ECS  | BTA_AG_FEAT_ECC  | \
                          BTA_AG_FEAT_EXTERR | BTA_AG_FEAT_BTRH)
#endif
#ifndef BTUI_AG_INSTANCES
#define BTUI_AG_INSTANCES 2
#endif

#ifndef BTUI_SPPDG_SECURITY
#define BTUI_SPPDG_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif
#ifndef BTUI_SPPDG_SERVICE_NAME
#define BTUI_SPPDG_SERVICE_NAME "Serial Port"
#endif

#ifndef BTUI_DUNDG_SECURITY
#define BTUI_DUNDG_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_DUNDG_SERVICE_NAME
#define BTUI_DUNDG_SERVICE_NAME "Dial-up Networking"
#endif

#ifndef BTUI_FAXDG_SECURITY
#define BTUI_FAXDG_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_FAXDG_SERVICE_NAME
#define BTUI_FAXDG_SERVICE_NAME "Fax"
#endif

#ifndef BTUI_OPS_SECURITY
#define BTUI_OPS_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_OPS_SERVICE_NAME
#define BTUI_OPS_SERVICE_NAME "Object Push"
#endif

#ifndef BTUI_OPC_SECURITY
#define BTUI_OPC_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_OPC_SERVICE_NAME
#define BTUI_OPC_SERVICE_NAME "Object Push"
#endif

#ifndef BTUI_PBC_SERVICE_NAME
#define BTUI_PBC_SERVICE_NAME "Phone Book Access Client"
#endif

#ifndef BTUI_FTS_SECURITY
#define BTUI_FTS_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_FTS_SERVICE_NAME
#define BTUI_FTS_SERVICE_NAME "OBEX File Transfer"
#endif

#ifndef BTUI_FTS_KEY
#define BTUI_FTS_KEY "1234"
#endif

#ifndef BTUI_FTS_REALM
#define BTUI_FTS_REALM "guest"
#endif

#ifndef BTUI_CT_SECURITY
#define BTUI_CT_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_DG_CLIENT_SECURITY
#define BTUI_DG_CLIENT_SECURITY (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_FTC_SECURITY
#define BTUI_FTC_SECURITY (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_FTC_SERVICES
#define BTUI_FTC_SERVICES (BTA_FTP_SERVICE_MASK | BTA_OPP_SERVICE_MASK | BTA_BIP_SERVICE_MASK)
#endif

#if( defined BTA_PBC_INCLUDED ) && (BTA_PBC_INCLUDED == TRUE)
#ifndef BTUI_PBC_FORMAT
#define BTUI_PBC_FORMAT     (BTA_PBC_FORMAT_CARD_21)
#endif

#ifndef BTUI_PBC_FILTER
#define BTUI_PBC_FILTER     (BTA_PBC_FILTER_ALL) /* 0 */
#endif

#ifndef BTUI_PBC_ORDER
#define BTUI_PBC_ORDER      (BTA_PBC_ORDER_INDEXED)
#endif

#ifndef BTUI_PBC_ATTR
#define BTUI_PBC_ATTR       (BTA_PBC_ATTR_NAME)
#endif

#ifndef BTUI_PBC_MAX_LCOUNT
#define BTUI_PBC_MAX_LCOUNT       (50)
#endif

#ifndef BTUI_PBC_PASSWORD
#define BTUI_PBC_PASSWORD "1234"
#endif

#ifndef BTUI_PBC_USERID
#define BTUI_PBC_USERID "guest"
#endif

#ifndef BTUI_PBC_SECURITY
#define BTUI_PBC_SECURITY (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#endif /* BTA_PBC_INCLUDED*/

#ifndef BTUI_FTC_PASSWORD
#define BTUI_FTC_PASSWORD "1234"
#endif

#ifndef BTUI_FTC_USERID
#define BTUI_FTC_USERID "guest"
#endif

#ifndef BTUI_PR_SECURITY
#define BTUI_PR_SECURITY (BTA_SEC_NONE)
#endif

#ifndef BTUI_PR_PASSWORD
#define BTUI_PR_PASSWORD "1234"
#endif

#ifndef BTUI_PR_USERID
#define BTUI_PR_USERID "guest"
#endif

#ifndef BTUI_ACC_PASSWORD
#define BTUI_ACC_PASSWORD "1234"
#endif

#ifndef BTUI_ACC_USERID
#define BTUI_ACC_USERID "guest"
#endif

#ifndef BTUI_PAN_SECURITY
#define BTUI_PAN_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_PANU_SERVICE_NAME
#define BTUI_PANU_SERVICE_NAME "Mobile PAN User"
#endif

#ifndef BTUI_PANGN_SERVICE_NAME
#define BTUI_PANGN_SERVICE_NAME "Mobile PAN GN"
#endif

#ifndef BTUI_PANNAP_SERVICE_NAME
#define BTUI_PANNAP_SERVICE_NAME "Mobile PAN NAP"
#endif

#ifndef BTUI_AA_SECURITY
#define BTUI_AA_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_AA_SERVICE_NAME
#define BTUI_AA_SERVICE_NAME "Advanced audio source"
#endif

#ifndef BTUI_AA_FEATURES
#define BTUI_AA_FEATURES 0
#endif

#ifndef BTUI_AA_LINE_SPEED_KBPS
#define BTUI_AA_LINE_SPEED_KBPS 229
#endif

#ifndef BTUI_AA_LINE_SPEED_BUSY
#define BTUI_AA_LINE_SPEED_BUSY    166
#endif

#ifndef BTUI_AA_LINE_SPEED_SWAMPD
#define BTUI_AA_LINE_SPEED_SWAMPD  128
#endif


#ifndef BTUI_AA_CHANNEL_MODE
#define BTUI_AA_CHANNEL_MODE 0x01
#endif

#ifndef BTUI_AV_SECURITY
#define BTUI_AV_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_AV_USE_AVRC
#define BTUI_AV_USE_AVRC    TRUE
#endif

#ifndef BTUI_AV_AUDIO_SERVICE_NAME
#define BTUI_AV_AUDIO_SERVICE_NAME "Advanced audio source"
#endif

#ifndef BTUI_AV_VIDEO_SERVICE_NAME
#define BTUI_AV_VIDEO_SERVICE_NAME "Advanced video source"
#endif

#ifndef BTUI_AV_SERVICES
#define BTUI_AV_SERVICES (BTA_VDP_SERVICE_MASK | BTA_A2DP_SERVICE_MASK | BTA_AVRCP_SERVICE_MASK)
#endif

#ifndef BTUI_AV_FEATURES
#define BTUI_AV_FEATURES 0
#endif

#ifndef BTUI_AV_LINE_SPEED_KBPS
#define BTUI_AV_LINE_SPEED_KBPS 229
#endif

#ifndef BTUI_AV_LINE_SPEED_BUSY
#define BTUI_AV_LINE_SPEED_BUSY    166
#endif

#ifndef BTUI_AV_LINE_SPEED_SWAMPD
#define BTUI_AV_LINE_SPEED_SWAMPD  128
#endif


#ifndef BTUI_AV_CHANNEL_MODE
#define BTUI_AV_CHANNEL_MODE 0x01
#endif

#ifndef BTUI_HD_SERVICE_NAME
#define BTUI_HD_SERVICE_NAME "BRCM HID Device"
#endif

#ifndef BTUI_HD_SECURITY
#define BTUI_HD_SECURITY (BTA_SEC_ENCRYPT|BTA_SEC_AUTHENTICATE)
#endif

#ifndef BTUI_HH_SERVICE_NAME
#define BTUI_HH_SERVICE_NAME "BRCM HID Host"
#endif

#ifndef BTUI_HH_SECURITY
#define BTUI_HH_SECURITY (BTA_SEC_ENCRYPT|BTA_SEC_AUTHENTICATE)
#endif

/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
static void bt_ui_load_app_config (void);

/* parameters which can be
configured for the application */
tBTUI_CFG bt_ui_cfg; 

#if( defined BTA_HH_INCLUDED ) && ( BTA_HH_INCLUDED == TRUE )
tBTUI_HH_CFG bt_ui_hh_cfg;
#endif

/*******************************************************************************
**
** Function         bt_ui_cfg_init
**
** Description      loads the configuation for the applicatin
**                  
** Returns          void
*******************************************************************************/
void bt_ui_cfg_init(void)
{
    /*tBTSIM_CONFIG_HCISU hcisu_info;*/
//     tHCIS_H4_CFG hcisu_h4_cfg;

    memset(&bt_ui_cfg, 0, sizeof(bt_ui_cfg));

    /* Get HCISU settings from Insight */
//     hcisu_info.p_port_cfg = &hcisu_h4_cfg;
//     if (BTSIM_GetConfig(BTSIM_CONFIG_ID_HCISU, &hcisu_info))
//     {
//         if (hcisu_info.port_type == BTSIM_HCIS_UART)
//         {
//             if ((((tHCIS_H4_CFG *)(hcisu_info.p_port_cfg))->cfg_flags) & HCISU_H4_CFG_FL_LPM0)
//                 bt_ui_cfg.lpm_sleep_mode = 0x00;
//             else if ((((tHCIS_H4_CFG *)(hcisu_info.p_port_cfg))->cfg_flags) & HCISU_H4_CFG_FL_LPM1)
//                 bt_ui_cfg.lpm_sleep_mode = 0x01;
//             else if ((((tHCIS_H4_CFG *)(hcisu_info.p_port_cfg))->cfg_flags) & HCISU_H4_CFG_FL_LPM4)
//                 bt_ui_cfg.lpm_sleep_mode = 0x04;
//             else if ((((tHCIS_H4_CFG *)(hcisu_info.p_port_cfg))->cfg_flags) & HCISU_H4_CFG_FL_LPM9)
//                 bt_ui_cfg.lpm_sleep_mode = 0x09;
//             else
//                 bt_ui_cfg.lpm_sleep_mode = 0xFF; /* no reconfiguration */
//         }
//     }

    /* load the configurable parameters */
    bt_ui_load_app_config();

#if( defined BTA_AG_INCLUDED ) && ( BTA_AG_INCLUDED == TRUE )
    if((bt_ui_cfg.supported_services &  BTA_HSP_SERVICE_MASK)
        || (bt_ui_cfg.supported_services &  BTA_HFP_SERVICE_MASK))
    {
        bt_ui_cfg.p_ag_init = &bt_ui_ag_init; 
    }
#endif 

#if( defined BTA_DG_INCLUDED ) && (BTA_DG_INCLUDED == TRUE)
    if((bt_ui_cfg.supported_services &  BTA_SPP_SERVICE_MASK)
        || (bt_ui_cfg.supported_services &  BTA_DUN_SERVICE_MASK)
        || (bt_ui_cfg.supported_services &  BTA_FAX_SERVICE_MASK)
        || (bt_ui_cfg.supported_services &  BTA_LAP_SERVICE_MASK))
    {
        bt_ui_cfg.p_dg_init = &bt_ui_dg_init; 
    }
#endif 

#if( defined BTA_FT_INCLUDED ) && (BTA_FT_INCLUDED == TRUE)
    if(bt_ui_cfg.ftc_included)
    {
        bt_ui_cfg.p_ftc_init = &bt_ui_ftc_init; 
    }
#endif 


#if( defined BTA_PBC_INCLUDED ) && (BTA_PBC_INCLUDED == TRUE)
    if(bt_ui_cfg.pbc_included)
    {
        bt_ui_cfg.p_pbc_init = &bt_ui_pbc_init; 
    }
#endif 

#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
    if(bt_ui_cfg.fts_included)
    {
        bt_ui_cfg.p_fts_init = &bt_ui_fts_init; 
    }
#endif 

#if( defined BTA_PBS_INCLUDED ) && (BTA_PBS_INCLUDED == TRUE)
    if(bt_ui_cfg.pbs_included)
    {
        bt_ui_cfg.p_pbs_init = &bcmapp_pbs_init; 
    }
#endif 

#if( defined BTA_AC_INCLUDED ) && (BTA_AC_INCLUDED == TRUE)
    if(bt_ui_cfg.acs_included)
    {
        bt_ui_cfg.p_acs_init = &bt_ui_acs_init; 
    }

    if(bt_ui_cfg.acc_included)
    {
        bt_ui_cfg.p_acc_init = &bt_ui_acc_init; 
        strcpy(bt_ui_cfg.acc_password, BTUI_ACC_PASSWORD);
        strcpy(bt_ui_cfg.acc_userid, BTUI_ACC_USERID);
    }
#endif 

#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    if(bt_ui_cfg.ops_included)
    {
        bt_ui_cfg.p_ops_init = &bt_ui_ops_init; 
    }
    if(bt_ui_cfg.opc_included)
    {
        bt_ui_cfg.p_opc_init = &bt_ui_opc_init; 
    }
#endif 

#if( defined BTA_PR_INCLUDED ) && (BTA_PR_INCLUDED == TRUE)
    if(bt_ui_cfg.pr_included)
    {
        bt_ui_cfg.p_pr_init = &bt_ui_pr_init; 
    }
#endif 


#if( defined BTA_SS_INCLUDED ) && (BTA_SS_INCLUDED == TRUE)
    if(bt_ui_cfg.ss_included)
    {
        bt_ui_cfg.p_ss_init = &bt_ui_ss_init; 
    }
#endif 

#if( defined BTA_SC_INCLUDED ) && (BTA_SC_INCLUDED == TRUE)
    if(bt_ui_cfg.sc_included)
    {
        bt_ui_cfg.p_sc_init = &bt_ui_sc_init; 
    }
#endif 

#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
    if(bt_ui_cfg.supported_services &  BTA_CTP_SERVICE_MASK)
    {
        bt_ui_cfg.p_ct_init = &bt_ui_ct_init; 
    }
#endif 

#if( defined BTA_PAN_INCLUDED ) && (BTA_PAN_INCLUDED == TRUE)
    if(bt_ui_cfg.pan_included)
    {
        bt_ui_cfg.p_pan_init = &bt_ui_pan_init; 
    }
#endif 

#if( defined BTA_AA_INCLUDED ) && (BTA_AA_INCLUDED == TRUE)
    if(bt_ui_cfg.aa_included)
    {
        bt_ui_cfg.p_aa_init = &bt_ui_aa_init; 
    }
#endif 

#if( defined BTA_AV_INCLUDED ) && (BTA_AV_INCLUDED == TRUE)
    if(bt_ui_cfg.av_included)
    {
        bt_ui_cfg.p_av_init = &bt_ui_av_init; 
    }
#endif 

#if( defined BTA_HD_INCLUDED ) && (BTA_HD_INCLUDED == TRUE)
    if(bt_ui_cfg.hd_included)
    {
        bt_ui_cfg.p_hd_init = &bt_ui_hd_init; 
    }
#endif 

    
#if( defined BTA_HH_INCLUDED ) && (BTA_HH_INCLUDED == TRUE)
    if(bt_ui_cfg.hh_included)
    {
        bt_ui_cfg.p_hh_init = &bt_ui_hh_init; 
    }

#endif 
    
#if( defined BTA_FM_INCLUDED ) && (BTA_FM_INCLUDED == TRUE)
    if(bt_ui_cfg.fm_included)
    {
        bt_ui_cfg.p_fm_init = &bt_ui_fm_init; 
    }

#endif

    //if(bt_ui_cfg.test_included)
    //{
    //    bt_ui_cfg.p_test_init = &bt_ui_test_init; 
    //}

}

/*******************************************************************************
**
** Function         bt_ui_load_app_config
**
** Description      loads the configuration for the application
**                  Set the parameters to default values. Then
**                  override it by some platform specific way
**                  
** Returns          void
*******************************************************************************/
static void bt_ui_load_app_config (void)
{

    /* set configurable parameters to
     default values */
    bt_ui_cfg.num_inq_devices = BTUI_NUM_REM_DEVICE;

#if (BTM_LISBON_INCLUDED == TRUE)
    /* set to use no MITM by default */
    bt_ui_cfg.sp_auth_req = BTUI_AUTH_REQ_NO;
#endif

    /* hsag configuration */
    bt_ui_cfg.ag_security = BTUI_AG_SECURITY;

#if( defined BTA_AG_INCLUDED ) && ( BTA_AG_INCLUDED == TRUE )
    bt_ui_cfg.ag_features = BTUI_AG_FEATURES;
#endif

    strcpy(bt_ui_cfg.hsag_service_name, BTUI_HSAG_SERVICE_NAME);

    /* hfag configuration */

    strcpy(bt_ui_cfg.hfag_service_name, BTUI_HFAG_SERVICE_NAME);

    bt_ui_cfg.ag_instances = BTUI_AG_INSTANCES;
    bt_ui_cfg.ag_bldn_enable = TRUE;	

    /*bt_ui_cfg.dg_port_name; needed for insight*/
    /* bt_ui_cfg.dg_port_baud; needed for insight*/
#if (defined BTA_DG_INCLUDED) && (BTA_DG_INCLUDED == TRUE)
    /* spp configuration */
    bt_ui_cfg.sppdg_security = BTUI_SPPDG_SECURITY;
    strcpy(bt_ui_cfg.sppdg_service_name, BTUI_SPPDG_SERVICE_NAME);


    /* dun configuration */
    bt_ui_cfg.dundg_security = BTUI_DUNDG_SECURITY;
    strcpy(bt_ui_cfg.dundg_service_name, BTUI_DUNDG_SERVICE_NAME);

    /* fax configuration */
    bt_ui_cfg.faxdg_security = BTUI_FAXDG_SECURITY;
    strcpy(bt_ui_cfg.faxdg_service_name, BTUI_FAXDG_SERVICE_NAME);
#endif
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    /* ops configuration */
    bt_ui_cfg.ops_security = BTUI_OPS_SECURITY;
    bt_ui_cfg.ops_included = TRUE;
    strcpy(bt_ui_cfg.ops_service_name, BTUI_OPS_SERVICE_NAME);

    /* opc configuration */
    bt_ui_cfg.opc_security = BTUI_OPC_SECURITY;
    bt_ui_cfg.opc_included = TRUE;
    bt_ui_cfg.opc_single_op = TRUE;
    strcpy(bt_ui_cfg.opc_service_name, BTUI_OPC_SERVICE_NAME);
#endif
#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
    /* fts configuration */
    bt_ui_cfg.fts_security = BTUI_FTS_SECURITY;
    strcpy(bt_ui_cfg.fts_service_name, BTUI_FTS_SERVICE_NAME);
    strcpy(bt_ui_cfg.fts_key, BTUI_FTS_KEY);
    strcpy(bt_ui_cfg.fts_realm, BTUI_FTS_REALM);
    bt_ui_cfg.fts_obex_auth = FALSE;
    bt_ui_cfg.fts_included = TRUE;
#endif
 #if( defined BTA_PBS_INCLUDED ) && (BTA_PBS_INCLUDED == TRUE)
   /* pbs configuration */
    bt_ui_cfg.pbs_security = BTUI_PBS_SECURITY;
    strcpy(bt_ui_cfg.pbs_service_name, BTUI_PBS_SERVICE_NAME);
    strcpy(bt_ui_cfg.pbs_key, BTUI_PBS_KEY);
    strcpy(bt_ui_cfg.pbs_realm, BTUI_PBS_REALM);
    bt_ui_cfg.pbs_obex_auth = FALSE;
#endif

#if( defined BTA_AC_INCLUDED ) && (BTA_AC_INCLUDED == TRUE)
   /* acs configuration */
    bt_ui_cfg.acc_security   = BTUI_ACC_SECURITY;
    bt_ui_cfg.acc_cam_get_image   = BTUI_ACC_CAM_GET_IMAGE;

    /* acs configuration */
    bt_ui_cfg.acs_security   = BTUI_ACS_SECURITY;
    strcpy(bt_ui_cfg.acs_service_name, BTUI_ACS_SERVICE_NAME);
    bt_ui_cfg.acs_req_thumb   = BTUI_ACS_REQUEST_THUMB;
    bt_ui_cfg.acs_features   = BTUI_ACS_FEATURES;
    strncpy(bt_ui_cfg.acs_capacity, BTUI_ACS_CAPACITY, BIP_UINT64_SIZE);
#endif

#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
    /* ct configuration */
    bt_ui_cfg.ct_security = BTUI_CT_SECURITY;
#endif
#if (defined BTA_DG_INCLUDED) && (BTA_DG_INCLUDED == TRUE)
    /* dg client */
    bt_ui_cfg.dg_client_security = BTUI_DG_CLIENT_SECURITY;
    bt_ui_cfg.dg_client_peer_name[0] = '\0';
    bt_ui_cfg.dg_client_service_id[0] = '1';
#endif
#if( defined BTA_FT_INCLUDED ) && (BTA_FT_INCLUDED == TRUE)
    /* ftc configuration */
    bt_ui_cfg.ftc_security = BTUI_FTC_SECURITY;
    bt_ui_cfg.ftc_services = BTUI_FTC_SERVICES;
    strcpy(bt_ui_cfg.ftc_password, BTUI_FTC_PASSWORD);
    strcpy(bt_ui_cfg.ftc_userid, BTUI_FTC_USERID);
#endif
#if( defined BTA_PBC_INCLUDED ) && (BTA_PBC_INCLUDED == TRUE)
    bt_ui_cfg.pbc_security = BTUI_PBC_SECURITY;
    strcpy(bt_ui_cfg.pbc_password, BTUI_PBC_PASSWORD);
    strcpy(bt_ui_cfg.pbc_userid, BTUI_PBC_USERID);
    bt_ui_cfg.pbc_filter = BTUI_PBC_FILTER;
    bt_ui_cfg.pbc_format = BTUI_PBC_FORMAT;
    bt_ui_cfg.pbc_order = BTUI_PBC_ORDER;
    bt_ui_cfg.pbc_attr = BTUI_PBC_ATTR;
    bt_ui_cfg.pbc_max_list_count = BTUI_PBC_MAX_LCOUNT;
    bt_ui_cfg.pbc_offset = 0;
#endif

#if( defined BTA_SS_INCLUDED ) && (BTA_SS_INCLUDED == TRUE)
    /* ss configuration */
    bt_ui_cfg.ss_security = BTUI_SS_SECURITY;
    strcpy(bt_ui_cfg.ss_service_name, BTUI_SS_SERVICE_NAME);
    strcpy(bt_ui_cfg.ss_key, BTUI_SS_KEY);
    strcpy(bt_ui_cfg.ss_realm, BTUI_SS_REALM);

    bt_ui_cfg.ss_obex_auth   = FALSE;
    bt_ui_cfg.ss_data_types  = BTUI_SS_DATA_TYPES;
    bt_ui_cfg.ss_cl_size     = SS_MAX_CL_ENTRIES;
    bt_ui_cfg.ss_did_size    = SS_MAX_DID_SIZE;
    bt_ui_cfg.ss_luid_size   = SS_MAX_LUID_SIZE;
#endif

#if( defined BTA_PR_INCLUDED ) && (BTA_PR_INCLUDED == TRUE)
    /* Default Print Client settings */
    /* By default, PR client will use BIP, BPP, and BIP services (over-ride using bt_ui_cfg.txt) */
    bt_ui_pr_cb.services_used = (BTA_OPP_SERVICE_MASK | BTA_BPP_SERVICE_MASK | BTA_BIP_SERVICE_MASK);
    bt_ui_cfg.pr_security = BTUI_PR_SECURITY;
    strcpy(bt_ui_cfg.pr_password, BTUI_PR_PASSWORD);
    strcpy(bt_ui_cfg.pr_userid, BTUI_PR_USERID);
#endif
#if( defined BTA_SC_INCLUDED ) && (BTA_SC_INCLUDED == TRUE)
    /* SIM Card server (sc) configuration */
    bt_ui_cfg.sc_security = BTUI_SC_SECURITY;
    strcpy(bt_ui_cfg.sc_service_name, BTUI_SC_SERVICE_NAME);
#endif
#if( defined PAN_INCLUDED ) && (PAN_INCLUDED == TRUE)	
    /* pan configuration */
    bt_ui_cfg.pan_security = BTUI_PAN_SECURITY;
    strcpy(bt_ui_cfg.panu_service_name, BTUI_PANU_SERVICE_NAME);
    strcpy(bt_ui_cfg.pangn_service_name, BTUI_PANGN_SERVICE_NAME);
    strcpy(bt_ui_cfg.pannap_service_name, BTUI_PANNAP_SERVICE_NAME);
#endif
#if( defined BTA_AA_INCLUDED ) && (BTA_AA_INCLUDED == TRUE)
    /* AA configuration */
    bt_ui_cfg.aa_security = BTUI_AA_SECURITY;
    strcpy(bt_ui_cfg.aa_service_name, BTUI_AA_SERVICE_NAME);
//#if( defined BTA_AA_INCLUDED ) && (BTA_AA_INCLUDED == TRUE)
    bt_ui_cfg.aa_features = BTUI_AA_FEATURES;
//#endif
    bt_ui_cfg.aa_line_speed_kbps = BTUI_AA_LINE_SPEED_KBPS;
    bt_ui_cfg.aa_line_speed_busy = BTUI_AA_LINE_SPEED_BUSY;
    bt_ui_cfg.aa_line_speed_swampd = BTUI_AA_LINE_SPEED_SWAMPD;
    bt_ui_cfg.aa_channel_mode = BTUI_AA_CHANNEL_MODE;
#endif
#if( defined BTA_AV_INCLUDED ) && (BTA_AV_INCLUDED == TRUE)
    /* AV configuration */
    bt_ui_cfg.av_security = BTUI_AV_SECURITY;
    bt_ui_cfg.use_avrc = BTUI_AV_USE_AVRC;
    bt_ui_cfg.av_features = BTUI_AV_FEATURES;
    bt_ui_cfg.av_line_speed_kbps = BTUI_AV_LINE_SPEED_KBPS;
    bt_ui_cfg.av_line_speed_busy = BTUI_AV_LINE_SPEED_BUSY;
    bt_ui_cfg.av_line_speed_swampd = BTUI_AV_LINE_SPEED_SWAMPD;
    bt_ui_cfg.av_channel_mode = BTUI_AV_CHANNEL_MODE;
    bt_ui_cfg.av_repeat_list = TRUE;

    strcpy(bt_ui_cfg.av_audio_service_name, BTUI_AV_AUDIO_SERVICE_NAME);
    strcpy(bt_ui_cfg.av_video_service_name, BTUI_AV_VIDEO_SERVICE_NAME);
    bt_ui_cfg.av_services_used = BTUI_AV_SERVICES;
    bt_ui_cfg.av_vdp_support= TRUE;
    bt_ui_cfg.av_num_audio  = BTA_AV_NUM_STRS - 1;
    bt_ui_cfg.av_min_trace  = 0;
    bt_ui_cfg.av_use_player = TRUE;
#if (BTU_DUAL_STACK_INCLUDED == TRUE )
    bt_ui_cfg.av_auto_switch = FALSE;
#endif
    bt_ui_cfg.av_suspd_rcfg = FALSE;
#endif

    /* HD configuration
#if( defined BTA_HD_INCLUDED ) && (BTA_HD_INCLUDED == TRUE)
#endif */
#if( defined BTA_HD_INCLUDED ) && ( BTA_HD_INCLUDED == TRUE )
    bt_ui_cfg.hd_security = BTUI_HD_SECURITY;
    strcpy(bt_ui_cfg.hd_service_name, BTUI_HD_SERVICE_NAME);
#endif
#if( defined BTA_HH_INCLUDED ) && (BTA_HH_INCLUDED == TRUE)
    /* HH configuration */
    bt_ui_cfg.hh_security = BTUI_HH_SECURITY;
    strcpy(bt_ui_cfg.hh_service_name, BTUI_HH_SERVICE_NAME);
#endif 


    /* test menu */
    bt_ui_cfg.test_included = FALSE;

    /* default ui hset gui */
    bt_ui_cfg.hset_ui_mode = FALSE;

    /* set the path for accessing files */
    //bt_ui_platform_set_root_path(bt_ui_cfg.root_path, BTUI_MAX_PATH_LENGTH);
#ifdef FEATURE_USE_OEM_FILEMGR
    STRCPY(bt_ui_cfg.root_path, AEEFS_ROOT_DIR);
#else
    STRCPY(bt_ui_cfg.root_path, "/brew/");
#endif
    /* if there is any mechanism to change the
    app configuration, specific to the platform,
    over write the default configuration */
    
    //bt_ui_platform_load_configuration();
    bt_ui_cfg.supported_services = (bt_ui_cfg.supported_services |BTA_HSP_SERVICE_MASK|BTA_HFP_SERVICE_MASK
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
                                               |BTA_OPP_SERVICE_MASK
#endif    
#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
                                               |BTA_FTP_SERVICE_MASK
#endif
#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
                                               |BTA_CTP_SERVICE_MASK|BTA_ICP_SERVICE_MASK
#endif
                                                 );
#if( defined BTA_HH_INCLUDED ) && ( BTA_HH_INCLUDED == TRUE )
    memset(&bt_ui_hh_cfg, 0, sizeof(tBTUI_HH_CFG));
    bt_ui_platform_load_hh_test_data();
#endif

}


/*******************************************************************************
**
** Function         bt_ui_get_selected_dev_name
**
** Description      
**                  
** Returns          UINT8 *
*******************************************************************************/
//char * bt_ui_get_selected_dev_name(void)
//{
//    char * p_name;
//
//    if(strlen(bt_ui_cb.p_selected_rem_device->short_name))
//    {
//        p_name =  bt_ui_cb.p_selected_rem_device->short_name;
//    }
//    else if(strlen(bt_ui_cb.p_selected_rem_device->name))
//    {
//        p_name =  bt_ui_cb.p_selected_rem_device->name;
//    }
//    else
//    {
//        p_name = "<NO NAME>";
//    }
//    return p_name;
//}


/*******************************************************************************
**
** Function         bt_ui_addr_str
**
** Description      given BD_ADDR, convert to ascii str in bt_ui_cb.bd_addr_str
**                  
** Returns          char *
*******************************************************************************/
//char * bt_ui_addr_str(UINT8 *p_addr)
//{
//    if(p_addr)
//    {
//        sprintf(bt_ui_cb.bd_addr_str, "<%02x:%02x:%02x:%02x:%02x:%02x>",
//                             p_addr[0], p_addr[1],
//                             p_addr[2], p_addr[3],
//                             p_addr[4], p_addr[5]);
//    }
//    else
//    {
//        sprintf(bt_ui_cb.bd_addr_str, "<>");
//    }
//
//    return bt_ui_cb.bd_addr_str;
//}

/*******************************************************************************
**
** Function         bt_ui_fclose
**
** Description      close the given file, if open
**                  
** Returns          void
*******************************************************************************/
//void bt_ui_fclose(FILE **pp_f)
//{
//    if(*pp_f)
//    {
//        fclose(*pp_f);
//        *pp_f = 0;
//    }
//}

//void bt_ui_platform_load_configuration(void)
//{
//    bt_ui_cfg.supported_services = bt_ui_cfg.supported_services |BTA_HSP_SERVICE_MASK|BTA_HFP_SERVICE_MASK|BTA_OPP_SERVICE_MASK|BTA_FTP_SERVICE_MASK;
//}
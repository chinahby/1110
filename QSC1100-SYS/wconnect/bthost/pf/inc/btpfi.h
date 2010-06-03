#ifndef _BTEXTPF_H
#define _BTEXTPF_H

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT                  
#ifdef FEATURE_BT_EXTPF        

#include "queue.h"

#ifdef FEATURE_BT_EXTPF_AV
#include "audiosbcenc.h"
#endif /* FEATURE_BT_EXTPF_AV */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    E X T E R N A L    P R O F I L E
                
                        I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth External Profile
  module.
  
Copyright (c) 2005-2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/btpfi.h#1 $ 

$Log: $
*    #24        07 Nov 2007           GS
* Added HID profile host role support.
*
*    #23        30 May 2006           GS
* Added support for PBAP feature
*
*    #21        22 Oct 2005           JH
* Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS
*
*    #20        14 Jul 2005           JH
* Moved bt_pf_avdtp_cmd_send_data() declaration to avoid compiler warning.
*
*    #19        13 Jul 2005           JTL
* Added external variable for denote NULL SBC data.
*
*    #17-18     23 Jun 2005           JTL
* Added external function declarations for AVRCP service registration, and
* a fast TX option for A2DP streaming data to AVDTP.
*
*    #16        10 Jun 2005           PN
* Added bt_pf_sap_process_command().
*
*    #14       05 Mar 2005            ABu
* Increased and renamed BT_PF_SIO_MAX_PORTS as it was doubly defined in btsio.c 
* renamed to BT_PF_SIO_MAX_PORTS and enlarged for Sirius and MT_OBEX requirements.
* 
*    #13       04 Mar 2005            JTL
* Adding AVDTP and A2DP profile support.
*
*    #12       30 Aug 2004            JH
* Removed prototypes of bt_pf_set_cod_obj_xfer_bit() and 
*  bt_pf_clear_cod_obj_xfer_bit(); these operations are now handled by SD.
*
*    #11       06 Aug 2004            JTL
* Adding code for initilization and event queue processing.
*
*    #9        26 Jul 2004            JH
* Revamped profile signal and timer scheme.
*
*    #6        30 Apr 2004            JH
* Added SML support.
*
*    #5        24 Feb 2004            JH
* Moved command ID enum to btpfcmdi.h.
*
*    #2        24 Feb 2004            JH
* Added BT_PF_RPT_TIMER_SIG signal.
*

===========================================================================*/
           
#undef  OI_CODE        
#define OI_CPU_TYPE OI_CPU_ARM7_LEND
#define IMPLEMENT_OI_DBGPRINT2
#define SMALL_RAM_MEMORY /* specify small memory model for OI code */

#define BT_PF_SIO_MAX_PORTS  8 /* maximum number of SIO connections at one time */

/*-------------------------------------------------------------------------*/
/*  Bluetooth profile task signal definitions.                             */
/*-------------------------------------------------------------------------*/
#define BT_PF_CMD_Q_SIG                             0x00000001
#define BT_PF_TIMER_SIG                             0x00000002
#define BT_PF_WAIT_SIG                              0x00000004
#define BT_PF_RPT_TIMER_SIG                         0x00000008
#define BT_PF_CON_CFM_SIG                           0x00000010
#define BT_PF_CON_IND_SIG                           0x00000020
#define BT_PF_DCN_IND_SIG                           0x00000040
#define BT_PF_RCV_IND_SIG                           0x00000080
#define BT_PF_WRT_CFM_SIG                           0x00000100
#define BT_PF_RLS_IND_SIG                           0x00000200
#define BT_PF_MSC_IND_SIG                           0x00000400
#define BT_PF_RPN_IND_SIG                           0x00000800         
#define BT_PF_TMO_SIG                               0x00001000 
#define BT_PF_TASK_OFFLINE_SIG TASK_OFFLINE_SIG  /* 0x00002000 MC */
#define BT_PF_TASK_STOP_SIG    TASK_STOP_SIG     /* 0x00004000 MC */
#define BT_PF_TASK_START_SIG   TASK_START_SIG    /* 0x00008000 MC */
#define BT_PF_OPP_SIG                               0x00010000
#define BT_PF_FTP_SIG                               0x00020000
#define BT_PF_BIP_CLI_SIG                           0x00040000
#define BT_PF_BIP_SRV_SIG                           0x00080000
#define BT_PF_BPP_CLI_SIG                           0x00100000
#define BT_PF_BPP_SRV_SIG                           0x00200000
#define BT_PF_L2IF_EV_SIG                           0x00400000
#define BT_PF_RMIF_EV_SIG                           0x00800000
#define BT_PF_A2DP_EV_SIG                           0x01000000
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */
/* Re-using PCAA sig */
#define BT_PF_REQUEUED_CMD_Q_SIG                    0x02000000
#define BT_PF_HID_HOST_EV_SIG                       0x08000000
#define BT_PF_HID_HOST_TMO_SIG                      0x10000000

#define BT_PF_CE_GROUP( ce_type, group )    ( (ce_type & 0xFF00) == \
                                              BT_CMD_EV_PF_##group##_BASE )

extern void bt_pf_task( dword ignored );

#ifdef FEATURE_BT_EXTPF_TEST
#error code not present
#endif

struct bt_pf_cmd_msg_struct; 

#ifdef FEATURE_BT_EXTPF_GOEP                 
/* Process GOEP client and server commands, in btpfgoep1.c */
extern void bt_pf_goep_process_command( struct bt_pf_cmd_msg_struct* );
#endif

#ifdef FEATURE_BT_EXTPF_OPP                 
/* Process OPP client and server commands, in btpfopp1.c */
extern void bt_pf_opp_process_command( struct bt_pf_cmd_msg_struct* );
#endif

#ifdef FEATURE_BT_EXTPF_FTP                 
/* Process FTP client and server commands, in btpfftp1.c */
extern void bt_pf_ftp_process_command( struct bt_pf_cmd_msg_struct* );
#endif

#ifdef FEATURE_BT_EXTPF_BIP                 
/* Process BIP client and server commands, in btpfbip1.c */
extern void bt_pf_bip_process_command( struct bt_pf_cmd_msg_struct* );
#endif

#ifdef FEATURE_BT_EXTPF_BPP                 
/* Process BPP client and server commands, in btpfbpp1.c */
extern void bt_pf_bpp_process_command( struct bt_pf_cmd_msg_struct* );
#endif

#ifdef FEATURE_BT_EXTPF_SML
/* Process SML client and server commands, in btpfsml1.c */
extern void bt_pf_sml_process_command( struct bt_pf_cmd_msg_struct* );
#endif

#ifdef FEATURE_BT_EXTPF_CTP
/* Process CTP client and server commands, in btpfctp.c */
extern void bt_pf_ctp_process_command( struct bt_pf_cmd_msg_struct* );
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_AV
/* Process AVDTP client and server commands, in btpfavdtp.c */
extern void bt_pf_avdtp_process_command( struct bt_pf_cmd_msg_struct* );

/* Process A2DP client and server commands, in btpfa2dp.c */
extern void bt_pf_a2dp_process_command( struct bt_pf_cmd_msg_struct* );

/* Process AVRCP client and server commands, in btpfavrcp.c */
extern void bt_pf_avrcp_process_command( struct bt_pf_cmd_msg_struct* );

/* Get supported capabilities for AVRCP */
extern uint16 bt_pf_avrcp_current_supported_features( void );

#endif

#ifdef FEATURE_BT_EXTPF_PBAP                 
/* Process PBAP client and server commands, in btpfpbap1.c */
extern void bt_pf_pbap_process_command( struct bt_pf_cmd_msg_struct* );
#endif

/* Process Profile L2 interface glue events */
extern void bt_pf_l2if_process_ev_queue( void );

/* Process Profile RM interface glue events */
extern void bt_pf_rmif_process_ev_queue( void );

#ifdef FEATURE_BT_EXTPF_AV
/* Process Profile A2DP interface events */
extern void bt_pf_a2dp_process_ev_queue( void );

/* NULL SBC data */
extern const audiosbcenc_data_type bt_pf_a2dp_null_sbc_data;
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/*-------------------------------------------------------------------------*/
/* Event queue for the PF task                                             */
/*-------------------------------------------------------------------------*/
struct bt_ev_msg_struct;
typedef struct bt_pf_event_q_info_struct
{
  q_type*  event_q_ptr;
  uint16   max_q_depth;
  uint16   max_event_bytes;
  uint32   pf_task_sig;
} bt_pf_event_q_info_type;



extern void bt_pf_store_bt_event( struct bt_ev_msg_struct*           bt_ev_ptr,
                                  struct bt_pf_event_q_info_struct*  evqi_ptr );



/*-------------------------------------------------------------------------*/
/* Initilization functions called on powerup                               */
/*-------------------------------------------------------------------------*/
/* Init L2 glue */
extern void bt_pf_l2if_init( void );

/* Init RM glue */
extern void bt_pf_rmif_init( void );

/* Init A2DP */
extern void bt_pf_a2dp_init( void );

/*-------------------------------------------------------------------------*/
/* Misc helper functions                                                   */
/*-------------------------------------------------------------------------*/
typedef struct _OI_BD_ADDR OI_BD_ADDR_ALIAS;
typedef struct _bt_bd_addr_type bt_bd_addr_type_alias;

/*===========================================================================

FUNCTION
  bt_pf_copy_bd_addr_from_oi

DESCRIPTION
  Helper function used to copy OI data from one structure to another.

===========================================================================*/
extern void bt_pf_copy_bd_addr_from_oi( bt_bd_addr_type_alias *dest, const OI_BD_ADDR_ALIAS *src );

/*===========================================================================

FUNCTION
  bt_pf_copy_bd_addr_to_oi

DESCRIPTION
  Utility function to copy a Bluetooth driver BD addr to an OI BD Addr.

===========================================================================*/
LOCAL void bt_pf_copy_bd_addr_to_oi( OI_BD_ADDR_ALIAS* dest,
                                     bt_bd_addr_type_alias* src );

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

#endif /* FEATURE_BT_EXTPF */

#endif /* FEAURE_BT */

#endif /* _BTEXTPF_H */


/****************************************************************************
**
**  Name:       bt_target.h
**
**  Function    this file contains definitions that will probably
**              change for each Bluetooth target system. This includes
**              such things as buffer pool sizes, number of tasks,
**              little endian/big endian conversions, etc...
**
**  NOTE        This file should always be included first.
**
**
**  Copyright (c) 1999-2007, Broadcom Corp., All Rights Reserved.
**  WIDCOMM Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#ifndef BT_TARGET_H
#define BT_TARGET_H

#include "data_types.h"

#define BUILDCFG
#ifdef BUILDCFG
#include "buildcfg.h"
#endif

#include "bt_types.h"   /* This must be defined AFTER buildcfg.h */
#include "dyn_mem.h"    /* defines static and/or dynamic memory for components */

/* #define BYPASS_AVDATATRACE */

/******************************************************************************
**
** Platform-Specific
**
******************************************************************************/

/* set to FALSE unless using Zeevo */
#ifndef ZEEVO_CTRL_DEFINED
#define ZEEVO_CTRL_DEFINED FALSE
#endif

/* Operating System Selection */
#ifndef BTE_SIM_APP
#define _GKI_ARM
#define _GKI_STANDALONE
#else
#define _BT_WIN32
#endif

/* Definitions of task IDs for inter-task messaging */
#ifndef MMI_TASK
#define MMI_TASK                0
#endif

#ifndef HCISU_TASK
#define HCISU_TASK              1
#endif

#ifndef BTU_TASK
#define BTU_TASK                2
#endif

/* BTE Application, Sample Apps, or Serial port Demo based on JP3 and JP4 setting) */
#ifndef BTE_APPL_TASK
#define BTE_APPL_TASK           3
#endif

#ifndef RPCT_TASK
#define RPCT_TASK               4
#endif

#ifndef UNV_TASK
#define UNV_TASK                5
#endif

#ifndef DEV_MGR_TASK
#define DEV_MGR_TASK            6
#endif

#ifndef BTE_IDLE_TASK
#define BTE_IDLE_TASK           7
#endif


/* API macros for simulator */

#define BTAPI

#ifndef BTE_BSE_WRAPPER
#ifdef  BTE_SIM_APP
#undef  BTAPI
#define BTAPI         __declspec(dllexport)
#endif
#endif

#define BT_API          BTAPI
#define BTU_API         BTAPI
#define A2D_API         BTAPI
#define VDP_API         BTAPI
#define AVDT_API        BTAPI
#define AVCT_API        BTAPI
#define AVRC_API        BTAPI
#define BIP_API         BTAPI
#define BNEP_API        BTAPI
#define BPP_API         BTAPI
#define BTM_API         BTAPI
#define CTP_API         BTAPI
#define DUN_API         BTAPI
#define FTP_API         BTAPI
#define GAP_API         BTAPI
#define GKI_API         BTAPI
#define GOEP_API        BTAPI
#define HCI_API         BTAPI
#define HCRP_API        BTAPI
#define HID_API         BTAPI
#define HFP_API         BTAPI
#define HSP2_API        BTAPI
#define ICP_API         BTAPI
#define L2C_API         BTAPI
#define OBX_API         BTAPI
#define OPP_API         BTAPI
#define PAN_API         BTAPI
#define RFC_API         BTAPI
#define RPC_API         BTAPI
#define SDP_API         BTAPI
#define SPP_API         BTAPI
#define TCS_API         BTAPI
#define UDRV_API        BTAPI
#define XML_API         BTAPI
#define BTA_API         BTAPI
#define SBC_API         BTAPI
#define LPM_API         BTAPI


/******************************************************************************
**
** GKI
**
******************************************************************************/

/* The number of GKI tasks in the software system. */
#ifndef GKI_MAX_TASKS
#define GKI_MAX_TASKS               8
#endif

/* The number of GKI timers in the software system. */
#ifndef GKI_NUM_TIMERS
#define GKI_NUM_TIMERS              3
#endif

/* A conversion value for translating system ticks to time. */
#ifndef TICKS_PER_SEC
#define TICKS_PER_SEC               100
#endif

/* delay in ticks before stopping system tick. */
#ifndef GKI_DELAY_STOP_SYS_TICK
#define GKI_DELAY_STOP_SYS_TICK     10
#endif

/* TRUE if GKI uses dynamic buffers. */
#ifndef GKI_USE_DYNAMIC_BUFFERS
#define GKI_USE_DYNAMIC_BUFFERS     FALSE
#endif

/* The size of the buffers in pool 0. */
#ifndef GKI_BUF0_SIZE
#define GKI_BUF0_SIZE               64
#endif

/* The number of buffers in buffer pool 0. */
#ifndef GKI_BUF0_MAX
#define GKI_BUF0_MAX                48
#endif

/* The ID of buffer pool 0. */
#ifndef GKI_POOL_ID_0
#define GKI_POOL_ID_0               0
#endif

/* The size of the buffers in pool 1. */
#ifndef GKI_BUF1_SIZE
#define GKI_BUF1_SIZE               128
#endif

/* The number of buffers in buffer pool 1. */
#ifndef GKI_BUF1_MAX
#define GKI_BUF1_MAX                26
#endif

/* The ID of buffer pool 1. */
#ifndef GKI_POOL_ID_1
#define GKI_POOL_ID_1               1
#endif

/* The size of the buffers in pool 2. */
#ifndef GKI_BUF2_SIZE
#define GKI_BUF2_SIZE               660
#endif

/* The number of buffers in buffer pool 2. */
#ifndef GKI_BUF2_MAX
#define GKI_BUF2_MAX                45
#endif

/* The ID of buffer pool 2. */
#ifndef GKI_POOL_ID_2
#define GKI_POOL_ID_2               2
#endif

/* The size of the buffers in pool 3. */
#ifndef GKI_BUF3_SIZE
#define GKI_BUF3_SIZE               1800
#endif

/* The number of buffers in buffer pool 3. */
#ifndef GKI_BUF3_MAX
#define GKI_BUF3_MAX                30
#endif

/* The ID of buffer pool 3. */
#ifndef GKI_POOL_ID_3
#define GKI_POOL_ID_3               3
#endif

/* The size of the largest PUBLIC fixed buffer in system. */
#ifndef GKI_MAX_BUF_SIZE
#define GKI_MAX_BUF_SIZE            GKI_BUF3_SIZE
#endif

/* RESERVED buffer pool for OBX */
/* Ideally there should be 1 buffer for each instance for RX data, and some number
of TX buffers based on active instances. OBX will only use these if packet size
requires it. In most cases the large packets are used in only one direction so
the other direction will use smaller buffers.
Devices with small amount of RAM should limit the number of active obex objects.
*/
/* The size of the buffers in pool 4. */
#ifndef GKI_BUF4_SIZE
#define GKI_BUF4_SIZE               0x2000
#endif

/* The number of buffers in buffer pool 4. */
#ifndef GKI_BUF4_MAX
#define GKI_BUF4_MAX                (OBX_NUM_SERVERS + OBX_NUM_CLIENTS)
#endif

/* The ID of buffer pool 4. */
#ifndef GKI_POOL_ID_4
#define GKI_POOL_ID_4               4
#endif

/* The number of fixed GKI buffer pools.
If L2CAP_FCR_INCLUDED is FALSE, Pool ID 5 is unnecessary */
#ifndef GKI_NUM_FIXED_BUF_POOLS
#define GKI_NUM_FIXED_BUF_POOLS     5
#endif

/* The buffer pool usage mask. */
#ifndef GKI_DEF_BUFPOOL_PERM_MASK
#define GKI_DEF_BUFPOOL_PERM_MASK   0xfff0
#endif

/* The number of fixed and dynamic buffer pools.
If L2CAP_FCR_INCLUDED is FALSE, Pool ID 4 is unnecessary */
#ifndef GKI_NUM_TOTAL_BUF_POOLS
#define GKI_NUM_TOTAL_BUF_POOLS     9
#endif

/* The following is intended to be a reserved pool for L2CAP
Flow control and retransmissions and intentionally kept out
of order */

/* The number of buffers in buffer pool 5. */
#ifndef GKI_BUF5_MAX
#define GKI_BUF5_MAX                46
#endif

/* The ID of buffer pool 5. */
#ifndef GKI_POOL_ID_5
#define GKI_POOL_ID_5               5
#endif

/* The size of the buffers in pool 5. */
#ifndef GKI_BUF5_SIZE
#define GKI_BUF5_SIZE               512
#endif

/* The buffer corruption check flag. */
#ifndef GKI_ENABLE_BUF_CORRUPTION_CHECK
#define GKI_ENABLE_BUF_CORRUPTION_CHECK TRUE
#endif

/* The GKI severe error macro. */
#ifndef GKI_SEVERE
#define GKI_SEVERE(code)
#endif

/* TRUE if GKI includes debug functionality. */
#ifndef GKI_DEBUG
#define GKI_DEBUG                   FALSE
#endif

/* Maximum number of exceptions logged. */
#ifndef GKI_MAX_EXCEPTION
#define GKI_MAX_EXCEPTION           8
#endif

/* Maximum number of chars stored for each exception message. */
#ifndef GKI_MAX_EXCEPTION_MSGLEN
#define GKI_MAX_EXCEPTION_MSGLEN    64
#endif

#ifndef GKI_SEND_MSG_FROM_ISR
#define GKI_SEND_MSG_FROM_ISR    FALSE
#endif

/******************************************************************************
**
** GKI Buffer Pools
**
******************************************************************************/

/* Receives HCI events from the lower-layer. */
#ifndef HCI_CMD_POOL_ID
#define HCI_CMD_POOL_ID             GKI_POOL_ID_2
#endif

#ifndef HCI_CMD_POOL_BUF_SIZE
#define HCI_CMD_POOL_BUF_SIZE       GKI_BUF2_SIZE
#endif

/* Receives ACL data packets from thelower-layer. */
#ifndef HCI_ACL_POOL_ID
#define HCI_ACL_POOL_ID             GKI_POOL_ID_3
#endif

#ifndef HCI_ACL_POOL_BUF_SIZE
#define HCI_ACL_POOL_BUF_SIZE       GKI_BUF3_SIZE
#endif

/* Receives SCO data packets from the lower-layer. */
#ifndef HCI_SCO_POOL_ID
#define HCI_SCO_POOL_ID             GKI_POOL_ID_1
#endif

/* Not used. */
#ifndef HCI_DATA_DESCR_POOL_ID
#define HCI_DATA_DESCR_POOL_ID      GKI_POOL_ID_0
#endif

/* Sends SDP data packets. */
#ifndef SDP_POOL_ID
#define SDP_POOL_ID                 GKI_POOL_ID_2
#endif

/* Sends RFCOMM command packets. */
#ifndef RFCOMM_CMD_POOL_ID
#define RFCOMM_CMD_POOL_ID          GKI_POOL_ID_2
#endif

#ifndef RFCOMM_CMD_POOL_BUF_SIZE
#define RFCOMM_CMD_POOL_BUF_SIZE    GKI_BUF2_SIZE
#endif

/* Sends RFCOMM data packets. */
#ifndef RFCOMM_DATA_POOL_ID
#define RFCOMM_DATA_POOL_ID         GKI_POOL_ID_3
#endif

#ifndef RFCOMM_DATA_POOL_BUF_SIZE
#define RFCOMM_DATA_POOL_BUF_SIZE   GKI_BUF3_SIZE
#endif

/* Sends L2CAP packets to the peer and HCI messages to the controller. */
#ifndef L2CAP_CMD_POOL_ID
#define L2CAP_CMD_POOL_ID           GKI_POOL_ID_2
#endif

/* Used by BTM when it sends HCI commands to the controller. */
#ifndef BTM_CMD_POOL_ID
#define BTM_CMD_POOL_ID             GKI_POOL_ID_2
#endif

/* Sends TCS messages. */
#ifndef TCS_MSG_POOL_ID
#define TCS_MSG_POOL_ID             GKI_POOL_ID_2
#endif

#ifndef OBX_CMD_POOL_SIZE
#define OBX_CMD_POOL_SIZE           GKI_BUF2_SIZE
#endif

#ifndef OBX_LRG_DATA_POOL_SIZE
#define OBX_LRG_DATA_POOL_SIZE      GKI_BUF4_SIZE
#endif

#ifndef OBX_LRG_DATA_POOL_ID
#define OBX_LRG_DATA_POOL_ID        GKI_POOL_ID_4
#endif

/* Used for CTP discovery database. */
#ifndef CTP_SDP_DB_POOL_ID
#define CTP_SDP_DB_POOL_ID          GKI_POOL_ID_3
#endif

/* Used for CTP data exchange feature. */
#ifndef CTP_DATA_EXCHG_POOL_ID
#define CTP_DATA_EXCHG_POOL_ID      GKI_POOL_ID_2
#endif

/* Used to send data to L2CAP. */
#ifndef GAP_DATA_POOL_ID
#define GAP_DATA_POOL_ID            GKI_POOL_ID_3
#endif

/* Used for SPP inquiry and discovery databases. */
#ifndef SPP_DB_POOL_ID
#define SPP_DB_POOL_ID              GKI_POOL_ID_3
#endif

#ifndef SPP_DB_SIZE
#define SPP_DB_SIZE                 GKI_BUF3_SIZE
#endif

/* HCRP protocol and internal commands. */
#ifndef HCRP_CMD_POOL_ID
#define HCRP_CMD_POOL_ID            GKI_POOL_ID_2
#endif

#ifndef HCRP_CMD_POOL_SIZE
#define HCRP_CMD_POOL_SIZE          GKI_BUF2_SIZE
#endif

#ifndef BIP_EVT_POOL_SIZE
#define BIP_EVT_POOL_SIZE           GKI_BUF3_SIZE
#endif

#ifndef BIP_DB_SIZE
#define BIP_DB_SIZE                 GKI_BUF3_SIZE
#endif


/* BNEP data and protocol messages. */
#ifndef BNEP_POOL_ID
#define BNEP_POOL_ID                GKI_POOL_ID_3
#endif

/* RPC pool for temporary trace message buffers. */
#ifndef RPC_SCRATCH_POOL_ID
#define RPC_SCRATCH_POOL_ID         GKI_POOL_ID_2
#endif

/* RPC scratch buffer size (not related to RPC_SCRATCH_POOL_ID) */
#ifndef RPC_SCRATCH_BUF_SIZE
#define RPC_SCRATCH_BUF_SIZE        GKI_BUF3_SIZE
#endif

/* RPC pool for protocol messages */
#ifndef RPC_MSG_POOL_ID
#define RPC_MSG_POOL_ID             GKI_POOL_ID_3
#endif

#ifndef RPC_MSG_POOL_SIZE
#define RPC_MSG_POOL_SIZE           GKI_BUF3_SIZE
#endif

/* AVDTP pool for protocol messages */
#ifndef AVDT_CMD_POOL_ID
#define AVDT_CMD_POOL_ID            GKI_POOL_ID_2
#endif

/* AVDTP pool size for media packets in case of fragmentation */
#ifndef AVDT_DATA_POOL_SIZE
#define AVDT_DATA_POOL_SIZE         GKI_BUF3_SIZE
#endif

#ifndef PAN_POOL_ID
#define PAN_POOL_ID                 GKI_POOL_ID_3
#endif

/* UNV pool for read/write serialization */
#ifndef UNV_MSG_POOL_ID
#define UNV_MSG_POOL_ID             GKI_POOL_ID_2
#endif

#ifndef UNV_MSG_POOL_SIZE
#define UNV_MSG_POOL_SIZE           GKI_BUF2_SIZE
#endif

/* AVCTP pool for protocol messages */
#ifndef AVCT_CMD_POOL_ID
#define AVCT_CMD_POOL_ID            GKI_POOL_ID_1
#endif

#ifndef AVCT_META_CMD_POOL_ID
#define AVCT_META_CMD_POOL_ID       GKI_POOL_ID_2
#endif

/* AVRCP pool for protocol messages */
#ifndef AVRC_CMD_POOL_ID
#define AVRC_CMD_POOL_ID            GKI_POOL_ID_1
#endif

/* AVRCP pool size for protocol messages */
#ifndef AVRC_CMD_POOL_SIZE
#define AVRC_CMD_POOL_SIZE          GKI_BUF1_SIZE   
#endif

/* AVRCP Metadata pool for protocol messages */
#ifndef AVRC_META_CMD_POOL_ID
#define AVRC_META_CMD_POOL_ID       GKI_POOL_ID_2
#endif

/* AVRCP Metadata pool size for protocol messages */
#ifndef AVRC_META_CMD_POOL_SIZE
#define AVRC_META_CMD_POOL_SIZE     GKI_BUF2_SIZE   
#endif

/* Compress pool ID */
#ifndef L2CAP_COMPRESS_POOL_ID
#define	L2CAP_COMPRESS_POOL_ID	    GKI_POOL_ID_3	/* Packet data compression	*/
#endif

/* Compress pool size */
#ifndef L2CAP_COMPRESS_SIZE
#define	L2CAP_COMPRESS_POOL_SIZE		    GKI_BUF3_SIZE
#endif


/******************************************************************************
**
** Lower Layer Interface
**
******************************************************************************/

/* Sends ACL data received over HCI to the upper stack. */
#ifndef HCI_ACL_DATA_TO_UPPER
#define HCI_ACL_DATA_TO_UPPER(p)    {((BT_HDR *)p)->event = BT_EVT_TO_BTU_HCI_ACL; GKI_send_msg (BTU_TASK, BTU_HCI_RCV_MBOX, p);}
#endif

/* Sends SCO data received over HCI to the upper stack. */
#ifndef HCI_SCO_DATA_TO_UPPER
#define HCI_SCO_DATA_TO_UPPER(p)    {((BT_HDR *)p)->event = BT_EVT_TO_BTU_HCI_SCO; GKI_send_msg (BTU_TASK, BTU_HCI_RCV_MBOX, p);}
#endif

/* Sends an HCI event received over HCI to theupper stack. */
#ifndef HCI_EVT_TO_UPPER
#define HCI_EVT_TO_UPPER(p)         {((BT_HDR *)p)->event = BT_EVT_TO_BTU_HCI_EVT; GKI_send_msg (BTU_TASK, BTU_HCI_RCV_MBOX, p);}
#endif

/* HCI 4 wire power management protocol. */
#ifndef HCILL_INCLUDED
#define HCILL_INCLUDED              FALSE
#endif

/* Macro for allocating buffer for HCI commands */
#ifndef HCI_GET_CMD_BUF
#if (!defined(HCI_USE_VARIABLE_SIZE_CMD_BUF) || (HCI_USE_VARIABLE_SIZE_CMD_BUF == FALSE))
/* Allocate fixed-size buffer from HCI_CMD_POOL (default case) */
#define HCI_GET_CMD_BUF(paramlen)    ((BT_HDR *)GKI_getpoolbuf (HCI_CMD_POOL_ID))
#else
/* Allocate smallest possible buffer (for platforms with limited RAM) */
#define HCI_GET_CMD_BUF(paramlen)    ((BT_HDR *)GKI_getbuf ((UINT16)(BT_HDR_SIZE + HCIC_PREAMBLE_SIZE + (paramlen))))
#endif
#endif  /* HCI_GET_CMD_BUF */

/******************************************************************************
**
** HCI Services (H4)
**
******************************************************************************/
#ifndef HCISU_H4_INCLUDED
#define HCISU_H4_INCLUDED               FALSE
#endif

#ifdef __cplusplus
extern "C" {
#endif
BT_API extern void bte_hcisu_send (BT_HDR *p_msg, UINT16 event);
#if (HCISU_H4_INCLUDED == TRUE)
BT_API extern void bte_hcisu_update_baud_rate (UINT8 baudrate, UINT32 clockrate, void *p_cback);
BT_API extern void bte_hcisu_lp_enable (void *p1, void *p2);
BT_API extern void bte_hcisu_lp_disable (void *p1);
BT_API extern void bte_hcisu_lp_allow_bt_device_sleep (void);
BT_API extern void bte_hcisu_lp_wakeup_host (void);
BT_API extern void bte_hcisu_lp_h4ibss_evt(UINT8 *p, UINT8 evt_len);
#endif

/* HCILL API for the applications */
typedef void (tHCILL_SLEEP_ACK)(void);
BT_API extern void HCILL_GoToSleep( tHCILL_SLEEP_ACK *sl_ack_fn);
typedef void (tHCILL_STATE_CBACK)(BOOLEAN is_sleep);
BT_API extern void HCILL_RegState( tHCILL_STATE_CBACK *p_cback);
#ifdef __cplusplus
}
#endif

/* Sends ACL data received from the upper stack tothe HCI transport. */
#ifndef HCI_ACL_DATA_TO_LOWER
#define HCI_ACL_DATA_TO_LOWER(p)    bte_hcisu_send((BT_HDR *)(p), BT_EVT_TO_LM_HCI_ACL);
#endif

/* Sends SCO data received from the upper stack to the HCI transport. */
#ifndef HCI_SCO_DATA_TO_LOWER
#define HCI_SCO_DATA_TO_LOWER(p)    bte_hcisu_send((BT_HDR *)(p), BT_EVT_TO_LM_HCI_SCO);
#endif

/* Sends an HCI command received from the upper stack to the HCI transport. */
#ifndef HCI_CMD_TO_LOWER
#define HCI_CMD_TO_LOWER(p)         bte_hcisu_send((BT_HDR *)(p), BT_EVT_TO_LM_HCI_CMD);
#endif

/* Sends an LM Diagnosic command received from the upper stack to the HCI transport. */
#ifndef HCI_LM_DIAG_TO_LOWER
#define HCI_LM_DIAG_TO_LOWER(p)     bte_hcisu_send((BT_HDR *)(p), BT_EVT_TO_LM_DIAG);
#endif

/* Send HCISU a message to update baudrate */
#ifndef HCI_UTIL_UPDATE_BAUD_RATE
#if (HCISU_H4_INCLUDED == TRUE)
#define HCI_UTIL_UPDATE_BAUD_RATE(p1, p2, p3)   bte_hcisu_update_baud_rate ((UINT8)(p1), (UINT32)(p2), (void *)(p3))
#else
#define HCI_UTIL_UPDATE_BAUD_RATE(p1, p2, p3)   HCIUTIL_UpdateBaudRate ((UINT8)(p1), (UINT32)(p2), (void *)(p3))
#endif
#endif

/* Send HCISU a message to enable sleep mode */
#ifndef HCI_LP_ENABLE
#if (HCISU_H4_INCLUDED == TRUE)
#define HCI_LP_ENABLE(p1, p2)       bte_hcisu_lp_enable((void *)(p1), (void *)(p2))
#else
#define HCI_LP_ENABLE(p1, p2)       HCILP_Enable((void *)(p1), (void *)(p2))
#endif
#endif

/* Send HCISU a message to disable sleep mode */
#ifndef HCI_LP_DISABLE
#if (HCISU_H4_INCLUDED == TRUE)
#define HCI_LP_DISABLE(p1)          bte_hcisu_lp_disable((void *)(p1))
#else
#define HCI_LP_DISABLE(p1)          HCILP_Disable((void *)(p1))
#endif
#endif

/* Send HCISU a message to allow BT sleep */
#ifndef HCI_LP_ALLOW_BT_DEVICE_SLEEP
#if (HCISU_H4_INCLUDED == TRUE)
#define HCI_LP_ALLOW_BT_DEVICE_SLEEP()       bte_hcisu_lp_allow_bt_device_sleep()
#else
#define HCI_LP_ALLOW_BT_DEVICE_SLEEP()       HCILP_AllowBTDeviceSleep()
#endif
#endif

/* Send HCISU a message to wakeup host */
#ifndef HCI_LP_WAKEUP_HOST
#if (HCISU_H4_INCLUDED == TRUE)
#define HCI_LP_WAKEUP_HOST()        bte_hcisu_lp_wakeup_host()
#else
#define HCI_LP_WAKEUP_HOST()        HCILP_WakeupHost()
#endif
#endif

/* Send HCISU the received H4IBSS event from controller */
#ifndef HCI_LP_RCV_H4IBSS_EVT
#if (HCISU_H4_INCLUDED == TRUE)
#define HCI_LP_RCV_H4IBSS_EVT(p1, p2)  bte_hcisu_lp_h4ibss_evt((UINT8*)(p1), (UINT8)(p2))
#else
#define HCI_LP_RCV_H4IBSS_EVT(p1, p2)  h4ibss_sleep_mode_evt((UINT8*)(p1), (UINT8)(p2))
#endif
#endif

/* If nonzero, the upper-layer sends at most this number of HCI commands to the lower-layer. */
#ifndef HCI_MAX_SIMUL_CMDS
#define HCI_MAX_SIMUL_CMDS          0
#endif

/* Timeout for receiving response to HCI command */
#ifndef BTU_CMD_CMPL_TIMEOUT
#define BTU_CMD_CMPL_TIMEOUT        8
#endif

/* If TRUE, stack is compiled to include dual stack functionality */
#ifndef BTU_DUAL_STACK_INCLUDED
#define BTU_DUAL_STACK_INCLUDED     FALSE
#endif

/* If TRUE, stack is compiled as Lite Stack in Multimedia chip */
/* If FALSE, stack is compiled as Full Stack in Basebacn chip */
#ifndef BTU_STACK_LITE_ENABLED
#define BTU_STACK_LITE_ENABLED      FALSE
#endif

/* Transport pause time (BT slot(0.625ms) unit) when switching between BB and MM */
/* FW is using a tick which is 20 slot unit so if timeout is between 0 to 20 slot */
/* then actual timeout would be 0 to 12.5ms because it could be beteen ticks. */
/* if timeout is between 20 to 40 slot then actual timeout would be 12.5 to 25ms */
#ifndef BTU_DUAL_TRANSPORT_PAUSE_TIME
#define BTU_DUAL_TRANSPORT_PAUSE_TIME     40
#endif

/* if UART baudrate is different between BB and MM, it will be updated during switching */
#ifndef BTU_DUAL_TRANSPORT_BB_BAUDRATE
#define BTU_DUAL_TRANSPORT_BB_BAUDRATE      115200
#endif

#ifndef BTU_DUAL_TRANSPORT_MM_BAUDRATE
#define BTU_DUAL_TRANSPORT_MM_BAUDRATE      921600
#endif

/******************************************************************************
**
** BTM
**
******************************************************************************/

/* Include BTM Discovery database and code. */
#ifndef BTM_DISCOVERY_INCLUDED
#define BTM_DISCOVERY_INCLUDED      TRUE
#endif

/* Include inquiry code. */
#ifndef BTM_INQUIRY_INCLUDED
#define BTM_INQUIRY_INCLUDED        TRUE
#endif

/* Include periodic inquiry code (used when BTM_INQUIRY_INCLUDED is TRUE). */
#ifndef BTM_PERIODIC_INQ_INCLUDED
#define BTM_PERIODIC_INQ_INCLUDED   TRUE
#endif

/* Include security code */
#ifndef BTM_SECURITY_INCLUDED
#define BTM_SECURITY_INCLUDED       TRUE
#endif

/* Include security authorization code (used when BTM_SECUIRTY_INCLUDED is TRUE). */
#ifndef BTM_AUTHORIZATION_INCLUDED
#define BTM_AUTHORIZATION_INCLUDED  TRUE
#endif

/* Include Lisbon implementation (2.1_EDR) */
#ifndef BTM_LISBON_INCLUDED
#define BTM_LISBON_INCLUDED         FALSE
#endif

/* Include the implemenation needed by Pre-Lisbon controller (2.0_EDR or older) */
#ifndef BTM_PRE_LISBON_INCLUDED
#define BTM_PRE_LISBON_INCLUDED     TRUE
#endif


/* Includes SCO if TRUE */
#ifndef BTM_SCO_INCLUDED
#define BTM_SCO_INCLUDED            TRUE       /* TRUE includes SCO code */
#endif

/* The size in bytes of the BTM inquiry database. */
#ifndef BTM_INQ_DB_SIZE
#define BTM_INQ_DB_SIZE             12
#endif

/* This is set to enable automatic periodic inquiry at startup. */
#ifndef BTM_ENABLE_AUTO_INQUIRY
#define BTM_ENABLE_AUTO_INQUIRY     FALSE
#endif

/* This is set to always try to acquire the remote device name. */
#ifndef BTM_INQ_GET_REMOTE_NAME
#define BTM_INQ_GET_REMOTE_NAME     FALSE
#endif

/* The inquiry duration in 1.28 second units when auto inquiry is enabled. */
#ifndef BTM_DEFAULT_INQ_DUR
#define BTM_DEFAULT_INQ_DUR         5
#endif

/* The inquiry mode when auto inquiry is enabled. */
#ifndef BTM_DEFAULT_INQ_MODE
#define BTM_DEFAULT_INQ_MODE        BTM_GENERAL_INQUIRY
#endif

/* The default periodic inquiry maximum delay when auto inquiry is enabled, in 1.28 second units. */
#ifndef BTM_DEFAULT_INQ_MAX_DELAY
#define BTM_DEFAULT_INQ_MAX_DELAY   30
#endif

/* The default periodic inquiry minimum delay when auto inquiry is enabled, in 1.28 second units. */
#ifndef BTM_DEFAULT_INQ_MIN_DELAY
#define BTM_DEFAULT_INQ_MIN_DELAY   20
#endif

/* The maximum age of entries in inquiry database in seconds ('0' disables feature). */
#ifndef BTM_INQ_MAX_AGE
#define BTM_INQ_MAX_AGE             0
#endif

/* The maximum age of entries in inquiry database based on inquiry response failure ('0' disables feature). */
#ifndef BTM_INQ_AGE_BY_COUNT
#define BTM_INQ_AGE_BY_COUNT        0
#endif

/* TRUE if controller does not support inquiry event filtering. */
#ifndef BTM_BYPASS_EVENT_FILTERING
#define BTM_BYPASS_EVENT_FILTERING  FALSE
#endif

/* TRUE if inquiry filtering is desired from BTM. */
#ifndef BTM_USE_INQ_RESULTS_FILTER
#define BTM_USE_INQ_RESULTS_FILTER  TRUE
#endif

/* The default scan mode */
#ifndef BTM_DEFAULT_SCAN_TYPE
#define BTM_DEFAULT_SCAN_TYPE       BTM_SCAN_TYPE_INTERLACED
#endif

/* When connectable mode is set to TRUE, the device will respond to paging. */
#ifndef BTM_IS_CONNECTABLE
#define BTM_IS_CONNECTABLE          FALSE
#endif

/* Sets the Page_Scan_Window:  the length of time that the device is performing a page scan. */
#ifndef BTM_DEFAULT_CONN_WINDOW
#define BTM_DEFAULT_CONN_WINDOW     0x0012
#endif

/* Sets the Page_Scan_Activity:  the interval between the start of two consecutive page scans. */
#ifndef BTM_DEFAULT_CONN_INTERVAL
#define BTM_DEFAULT_CONN_INTERVAL   0x0800
#endif

/* This is set to automatically perform inquiry scan on startup. */
#ifndef BTM_IS_DISCOVERABLE
#define BTM_IS_DISCOVERABLE         FALSE
#endif

/* When automatic inquiry scan is enabled, this sets the discovery mode. */
#ifndef BTM_DEFAULT_DISC_MODE
#define BTM_DEFAULT_DISC_MODE       BTM_GENERAL_DISCOVERABLE
#endif

/* When automatic inquiry scan is enabled, this sets the inquiry scan window. */
#ifndef BTM_DEFAULT_DISC_WINDOW
#define BTM_DEFAULT_DISC_WINDOW     0x0012
#endif

/* When automatic inquiry scan is enabled, this sets the inquiry scan interval. */
#ifndef BTM_DEFAULT_DISC_INTERVAL
#define BTM_DEFAULT_DISC_INTERVAL   0x0800
#endif

/* Sets the period, in seconds, to automatically perform service discovery. */
#ifndef BTM_AUTO_DISCOVERY_PERIOD
#define BTM_AUTO_DISCOVERY_PERIOD   0
#endif

/* The size in bytes of the BTM discovery database (if discovery is included). */
#ifndef BTM_DISCOVERY_DB_SIZE
#define BTM_DISCOVERY_DB_SIZE       4000
#endif

/* Number of milliseconds to delay BTU task startup upon device initialization. */
#ifndef BTU_STARTUP_DELAY
#define BTU_STARTUP_DELAY           0
#endif

/* Whether BTA is included in BTU task. */
#ifndef BTU_BTA_INCLUDED
#define BTU_BTA_INCLUDED            FALSE
#endif

/* Number of seconds to wait to send an HCI Reset command upon device initialization. */
#ifndef BTM_FIRST_RESET_DELAY
#define BTM_FIRST_RESET_DELAY       0
#endif

/* The number of seconds to wait for controller module to reset after issuing an HCI Reset command. */
#ifndef BTM_AFTER_RESET_TIMEOUT
#define BTM_AFTER_RESET_TIMEOUT     0
#endif

/* The default class of device. */
#ifndef BTM_INIT_CLASS_OF_DEVICE
#define BTM_INIT_CLASS_OF_DEVICE    "\x00\x1F\x00"
#endif

/* The number of SCO links. */
#ifndef BTM_MAX_SCO_LINKS
#define BTM_MAX_SCO_LINKS           3
#endif

/* The preferred type of SCO links (2-eSCO, 0-SCO). */
#ifndef BTM_DEFAULT_SCO_MODE
#define BTM_DEFAULT_SCO_MODE        2
#endif

/* The number of security records for peer devices. */
#ifndef BTM_SEC_MAX_DEVICE_RECORDS
#define BTM_SEC_MAX_DEVICE_RECORDS  8
#endif

/* The number of security records for services. */
#ifndef BTM_SEC_MAX_SERVICE_RECORDS
#define BTM_SEC_MAX_SERVICE_RECORDS 24
#endif

/* Maximum device name length used in btm database. */
#ifndef BTM_MAX_REM_BD_NAME_LEN
#define BTM_MAX_REM_BD_NAME_LEN     20
#endif

/* Maximum local device name length stored btm database.
  '0' disables storage of the local name in BTM */
#ifndef BTM_MAX_LOC_BD_NAME_LEN
#define BTM_MAX_LOC_BD_NAME_LEN     31
#endif

/* TRUE if default string is used, FALSE if device name is set in the application */
#ifndef BTM_USE_DEF_LOCAL_NAME
#define BTM_USE_DEF_LOCAL_NAME      FALSE
#endif

/* Fixed Default String (Ignored if BTM_USE_DEF_LOCAL_NAME is FALSE) */
#ifndef BTM_DEF_LOCAL_NAME
#define BTM_DEF_LOCAL_NAME      ""
#endif

/* Maximum service name stored with security authorization (0 if not needed) */
#ifndef BTM_SEC_SERVICE_NAME_LEN
#define BTM_SEC_SERVICE_NAME_LEN    BT_MAX_SERVICE_NAME_LEN
#endif

/* Maximum length of the service name. */
#ifndef BT_MAX_SERVICE_NAME_LEN
#define BT_MAX_SERVICE_NAME_LEN     21
#endif

/* ACL buffer size in HCI Host Buffer Size command. */
#ifndef BTM_ACL_BUF_SIZE
#define BTM_ACL_BUF_SIZE            0
#endif

/* This is set to use the BTM power manager. */
#ifndef BTM_PWR_MGR_INCLUDED
#define BTM_PWR_MGR_INCLUDED        TRUE
#endif

/* The maximum number of clients that can register with the power manager. */
#ifndef BTM_MAX_PM_RECORDS
#define BTM_MAX_PM_RECORDS          2
#endif

/* This is set to show debug trace messages for the power manager. */
#ifndef BTM_PM_DEBUG
#define BTM_PM_DEBUG                FALSE
#endif

/* This is set to TRUE if link is to be unparked due to BTM_CreateSCO API. */
#ifndef BTM_SCO_WAKE_PARKED_LINK
#define BTM_SCO_WAKE_PARKED_LINK    TRUE
#endif

/* May be set to the the name of a function used for vendor specific chip initialization */
#ifndef BTM_APP_DEV_INIT
/* #define BTM_APP_DEV_INIT         myInitFunction() */
#endif

/* This is set to TRUE if the busy level change event is desired. (replace ACL change event) */
#ifndef BTM_BUSY_LEVEL_CHANGE_INCLUDED
#define BTM_BUSY_LEVEL_CHANGE_INCLUDED  TRUE
#endif

/* If the user does not respond to security process requests within this many seconds,
 * a negative response would be sent automatically.
 * It's recommended to use a value between 30 and OBX_TIMEOUT_VALUE
 * 30 is LMP response timeout value */
#ifndef BTM_SEC_TIMEOUT_VALUE
#define BTM_SEC_TIMEOUT_VALUE           35
#endif

/******************************************
**    Lisbon Features
**  Used when BTM_LISBON_INCLUDED == TRUE
*******************************************/
/* This is set to TRUE if the server Extended Inquiry Response feature is desired. */
/* server sends EIR to client */
#ifndef BTM_EIR_SERVER_INCLUDED
#define BTM_EIR_SERVER_INCLUDED         TRUE
#endif

/* This is set to TRUE if the client Extended Inquiry Response feature is desired. */
/* client inquiry to server */
#ifndef BTM_EIR_CLIENT_INCLUDED
#define BTM_EIR_CLIENT_INCLUDED         TRUE
#endif

/* This is set to TRUE if the FEC is required for EIR packet. */
#ifndef BTM_EIR_DEFAULT_FEC_REQUIRED
#define BTM_EIR_DEFAULT_FEC_REQUIRED    TRUE
#endif

/* User defined UUID look up table */
#ifndef BTM_EIR_UUID_LKUP_TBL
#endif

/* The IO capability of the local device (for Simple Pairing) */
#ifndef BTM_LOCAL_IO_CAPS
#define BTM_LOCAL_IO_CAPS               BTM_IO_CAP_IO
#endif

/* The default MITM Protection Requirement (for Simple Pairing)
 * Possible values are BTM_AUTH_SP_YES or BTM_AUTH_SP_NO */
#ifndef BTM_DEFAULT_AUTH_REQ
#define BTM_DEFAULT_AUTH_REQ            BTM_AUTH_SP_NO
#endif

/* Include Out-of-Band implementation for Simple Pairing */
#ifndef BTM_OOB_INCLUDED
#define BTM_OOB_INCLUDED                FALSE
#endif

/*************************
** End of Lisbon Features
**************************/

/******************************************************************************
**
** L2CAP
**
******************************************************************************/

/* Flow control and retransmission mode */

#ifndef L2CAP_FCR_INCLUDED
#define L2CAP_FCR_INCLUDED FALSE
#endif

/* The maximum number of simultaneous links that L2CAP can support. */
#ifndef MAX_L2CAP_LINKS
#define MAX_L2CAP_LINKS             4
#endif

/* The maximum number of simultaneous channels that L2CAP can support. */
#ifndef MAX_L2CAP_CHANNELS
#define MAX_L2CAP_CHANNELS          10
#endif

/* The maximum number of simultaneous applications that can register with L2CAP. */
#ifndef MAX_L2CAP_CLIENTS
#define MAX_L2CAP_CLIENTS           8
#endif

/* Whether to attempt link reconnect when HCI disconnect is received while waiting for L2CAP connect response */
#ifndef L2CAP_LINK_RECONNECT
#define L2CAP_LINK_RECONNECT        TRUE
#endif

/* The number of seconds of link inactivity before a link is disconnected. */
#ifndef L2CAP_LINK_INACTIVITY_TOUT
#define L2CAP_LINK_INACTIVITY_TOUT  2
#endif

/* The number of seconds of link inactivity after bonding before a link is disconnected. */
#ifndef L2CAP_BONDING_TIMEOUT
#define L2CAP_BONDING_TIMEOUT       3
#endif

/* The time from the HCI connection complete to disconnect if no channel is established. */
#ifndef L2CAP_LINK_STARTUP_TOUT
#define L2CAP_LINK_STARTUP_TOUT     60
#endif

/* The L2CAP MTU; must be in accord with the HCI ACL pool size. */
#ifndef L2CAP_MTU_SIZE
#define L2CAP_MTU_SIZE              1691
#endif

/* This is set to enable host flow control. */
#ifndef L2CAP_HOST_FLOW_CTRL
#define L2CAP_HOST_FLOW_CTRL        FALSE
#endif

/* If host flow control enabled, this is the number of buffers the controller can have unacknowledged. */
#ifndef L2CAP_HOST_FC_ACL_BUFS
#define L2CAP_HOST_FC_ACL_BUFS      20
#endif

/* The percentage of the queue size allowed before a congestion event is sent to the L2CAP client (typically 120%). */
#ifndef L2CAP_FWD_CONG_THRESH
#define L2CAP_FWD_CONG_THRESH       120
#endif

/* This is set to enable L2CAP to  take the ACL link out of park mode when ACL data is to be sent. */
#ifndef L2CAP_WAKE_PARKED_LINK
#define L2CAP_WAKE_PARKED_LINK      TRUE
#endif

/* Whether link wants to be the master or the slave. */
#ifndef L2CAP_DESIRED_LINK_ROLE
#define L2CAP_DESIRED_LINK_ROLE     HCI_ROLE_SLAVE
#endif

/* Include Non-Flushable Packet Boundary Flag feature of Lisbon */
#ifndef L2CAP_NON_FLUSHABLE_PB_INCLUDED
#define L2CAP_NON_FLUSHABLE_PB_INCLUDED     TRUE
#endif

/******************************************************************************
**
** SDP
**
******************************************************************************/

/* This is set to enable SDP server functionality. */
#ifndef SDP_SERVER_ENABLED
#define SDP_SERVER_ENABLED          TRUE
#endif

/* The maximum number of SDP records the server can support. */
#ifndef SDP_MAX_RECORDS
#define SDP_MAX_RECORDS             20
#endif

/* The maximum number of attributes in each record. */
#ifndef SDP_MAX_REC_ATTR
#if defined(HID_DEV_INCLUDED) && (HID_DEV_INCLUDED==TRUE)
#define SDP_MAX_REC_ATTR            25
#else
#define SDP_MAX_REC_ATTR            13
#endif
#endif

#ifndef SDP_MAX_PAD_LEN
#define SDP_MAX_PAD_LEN             350
#endif

/* The maximum length, in bytes, of an attribute. */
#ifndef SDP_MAX_ATTR_LEN
#if defined(HID_DEV_INCLUDED) && (HID_DEV_INCLUDED==TRUE)
#define SDP_MAX_ATTR_LEN            80
#else
#define SDP_MAX_ATTR_LEN            100
#endif
#endif

/* The maximum number of attribute filters supported by SDP databases. */
#ifndef SDP_MAX_ATTR_FILTERS
#define SDP_MAX_ATTR_FILTERS        12
#endif

/* The maximum number of UUID filters supported by SDP databases. */
#ifndef SDP_MAX_UUID_FILTERS
#define SDP_MAX_UUID_FILTERS        3
#endif

/* This is set to enable SDP client functionality. */
#ifndef SDP_CLIENT_ENABLED
#define SDP_CLIENT_ENABLED          TRUE
#endif

/* The maximum number of record handles retrieved in a search. */
#ifndef SDP_MAX_DISC_SERVER_RECS
#define SDP_MAX_DISC_SERVER_RECS    21
#endif

/* The size of a scratchpad buffer, in bytes, for storing the response to an attribute request. */
#ifndef SDP_MAX_LIST_BYTE_COUNT
#define SDP_MAX_LIST_BYTE_COUNT     1000
#endif

/* The maximum number of parameters in an SDP protocol element. */
#ifndef SDP_MAX_PROTOCOL_PARAMS
#define SDP_MAX_PROTOCOL_PARAMS     2
#endif

/* The maximum number of simultaneous client and server connections. */
#ifndef SDP_MAX_CONNECTIONS
#define SDP_MAX_CONNECTIONS         4
#endif

/* The MTU size for the L2CAP configuration. */
#ifndef SDP_MTU_SIZE
#define SDP_MTU_SIZE                256
#endif

/* The flush timeout for the L2CAP configuration. */
#ifndef SDP_FLUSH_TO
#define SDP_FLUSH_TO                0xFFFF
#endif

/* The name for security authorization. */
#ifndef SDP_SERVICE_NAME
#define SDP_SERVICE_NAME            "Service Discovery"
#endif

/* The security level for BTM. */
#ifndef SDP_SECURITY_LEVEL
#define SDP_SECURITY_LEVEL          BTM_SEC_NONE
#endif

/* Device identification feature. */
#ifndef SDP_DI_INCLUDED
#define SDP_DI_INCLUDED             FALSE
#endif

/******************************************************************************
**
** RFCOMM
**
******************************************************************************/

#ifndef RFCOMM_INCLUDED
#define RFCOMM_INCLUDED             FALSE
#endif

/* The maximum number of ports supported. */
#ifndef MAX_PORTS
#define MAX_PORTS                   5
#endif

/* The maximum simultaneous links to different devices. */
#ifndef MAX_BD_CONNECTIONS
#define MAX_BD_CONNECTIONS          1
#endif

/* The port receive queue low watermark level, in bytes. */
#ifndef PORT_RX_LOW_WM
#define PORT_RX_LOW_WM              5000
#endif

/* The port receive queue high watermark level, in bytes. */
#ifndef PORT_RX_HIGH_WM
#define PORT_RX_HIGH_WM             8000
#endif

/* The port receive queue critical watermark level, in bytes. */
#ifndef PORT_RX_CRITICAL_WM
#define PORT_RX_CRITICAL_WM         12000
#endif

/* The port receive queue low watermark level, in number of buffers. */
#ifndef PORT_RX_BUF_LOW_WM
#define PORT_RX_BUF_LOW_WM          8
#endif

/* The port receive queue high watermark level, in number of buffers. */
#ifndef PORT_RX_BUF_HIGH_WM
#define PORT_RX_BUF_HIGH_WM         16
#endif

/* The port receive queue critical watermark level, in number of buffers. */
#ifndef PORT_RX_BUF_CRITICAL_WM
#define PORT_RX_BUF_CRITICAL_WM     22
#endif

/* The port transmit queue high watermark level, in bytes. */
#ifndef PORT_TX_HIGH_WM
#define PORT_TX_HIGH_WM             8000
#endif

/* The port transmit queue critical watermark level, in bytes. */
#ifndef PORT_TX_CRITICAL_WM
#define PORT_TX_CRITICAL_WM         10000
#endif

/* The port transmit queue high watermark level, in number of buffers. */
#ifndef PORT_TX_BUF_HIGH_WM
#define PORT_TX_BUF_HIGH_WM         16
#endif

/* The port transmit queue high watermark level, in number of buffers. */
#ifndef PORT_TX_BUF_CRITICAL_WM
#define PORT_TX_BUF_CRITICAL_WM     22
#endif

/* The RFCOMM multiplexer preferred flow control mechanism. */
#ifndef PORT_FC_DEFAULT
#define PORT_FC_DEFAULT             PORT_FC_CREDIT
#endif

/* The maximum number of credits receiver sends to peer when using credit-based flow control. */
#ifndef PORT_CREDIT_RX_MAX
#define PORT_CREDIT_RX_MAX          16
#endif

/* The credit low watermark level. */
#ifndef PORT_CREDIT_RX_LOW
#define PORT_CREDIT_RX_LOW          8
#endif

/******************************************************************************
**
** TCS
**
******************************************************************************/

#ifndef TCS_INCLUDED
#define TCS_INCLUDED                FALSE
#endif

/* If set to TRUE, gives lean TCS state machine configuration. */
#ifndef TCS_LEAN
#define TCS_LEAN                    FALSE
#endif

/* To include/exclude point-to-multipoint broadcast SETUP configuration. */
#ifndef TCS_BCST_SETUP_INCLUDED
#define TCS_BCST_SETUP_INCLUDED     TRUE
#endif

/* To include/exclude supplementary services. */
#ifndef TCS_SUPP_SVCS_INCLUDED
#define TCS_SUPP_SVCS_INCLUDED      TRUE
#endif

/* To include/exclude WUG master role. */
#ifndef TCS_WUG_MASTER_INCLUDED
#define TCS_WUG_MASTER_INCLUDED     TRUE
#endif

/* To include/exclude WUG member role. */
#ifndef TCS_WUG_MEMBER_INCLUDED
#define TCS_WUG_MEMBER_INCLUDED     TRUE
#endif

/* Maximum number of WUG members. */
#ifndef TCS_MAX_WUG_MEMBERS
#define TCS_MAX_WUG_MEMBERS         7
#endif

/* Widcomm specific acknowledgement message to ensure fast and robust operation of WUG FIMA procedure. */
#ifndef TCS_WUG_LISTEN_ACPT_ACK_INCLUDED
#define TCS_WUG_LISTEN_ACPT_ACK_INCLUDED TRUE
#endif

/* The number of simultaneous calls supported. */
#ifndef TCS_MAX_NUM_SIMUL_CALLS
#define TCS_MAX_NUM_SIMUL_CALLS     3
#endif

/* The number of devices the device can connect to. */
#ifndef TCS_MAX_NUM_ACL_CONNS
#define TCS_MAX_NUM_ACL_CONNS       7
#endif

/* The maximum length, in bytes, of the company specific information element. */
#ifndef TCS_MAX_CO_SPEC_LEN
#define TCS_MAX_CO_SPEC_LEN         40
#endif

/* The maximum length, in bytes, of the audio control information element . */
#ifndef TCS_MAX_AUDIO_CTL_LEN
#define TCS_MAX_AUDIO_CTL_LEN       40
#endif

/* (Dis)allow EDR ESCO */
#ifndef TCS_AUDIO_USE_ESCO_EDR
#define TCS_AUDIO_USE_ESCO_EDR      FALSE
#endif

/******************************************************************************
**
** OBX
**
******************************************************************************/
#ifndef OBX_INCLUDED
#define OBX_INCLUDED               FALSE
#endif

#ifndef OBX_CLIENT_INCLUDED
#define OBX_CLIENT_INCLUDED        TRUE
#endif

#ifndef OBX_SERVER_INCLUDED
#define OBX_SERVER_INCLUDED        TRUE
#endif

/* TRUE to include OBEX authentication/MD5 code */
#ifndef OBX_MD5_INCLUDED
#define OBX_MD5_INCLUDED           TRUE
#endif

/* The idle timeout value. 0 for no timeout event. */
#ifndef OBX_TIMEOUT_VALUE
#define OBX_TIMEOUT_VALUE           60
#endif

/* Timeout value used for disconnect */
#ifndef OBX_DISC_TOUT_VALUE
#define OBX_DISC_TOUT_VALUE         5
#endif

/* The maximum number of registered servers. */
#ifndef OBX_NUM_SERVERS
#define OBX_NUM_SERVERS             6
#endif

/* The maximum number of active clients. */
#ifndef OBX_NUM_CLIENTS
#define OBX_NUM_CLIENTS             3
#endif

/* The maximum length of OBEX target header.*/
#ifndef OBX_MAX_TARGET_LEN
#define OBX_MAX_TARGET_LEN          16
#endif

/* The maximum length of authentication challenge realm.*/
#ifndef OBX_MAX_REALM_LEN
#define OBX_MAX_REALM_LEN           30
#endif

/******************************************************************************
**
** BNEP
**
******************************************************************************/

#ifndef BNEP_INCLUDED
#define BNEP_INCLUDED               FALSE
#endif

/* Protocol filtering is an optional feature. Bydefault it will be turned on */
#ifndef BNEP_SUPPORTS_PROT_FILTERS
#define BNEP_SUPPORTS_PROT_FILTERS          TRUE
#endif

/* Multicast filtering is an optional feature. Bydefault it will be turned on */
#ifndef BNEP_SUPPORTS_MULTI_FILTERS
#define BNEP_SUPPORTS_MULTI_FILTERS         TRUE
#endif

/* BNEP status API call is used mainly to get the L2CAP handle */
#ifndef BNEP_SUPPORTS_STATUS_API
#define BNEP_SUPPORTS_STATUS_API            TRUE
#endif

/* This is just a debug function */
#ifndef BNEP_SUPPORTS_DEBUG_DUMP
#define BNEP_SUPPORTS_DEBUG_DUMP            TRUE
#endif

#ifndef BNEP_SUPPORTS_ALL_UUID_LENGTHS
#define BNEP_SUPPORTS_ALL_UUID_LENGTHS      TRUE    /* Otherwise it will support only 16bit UUIDs */
#endif

/*
** When BNEP connection changes roles after the connection is established
** we will do an authentication check again on the new role
*/
#ifndef BNEP_DO_AUTH_FOR_ROLE_SWITCH
#define BNEP_DO_AUTH_FOR_ROLE_SWITCH        TRUE
#endif


/* Maximum number of protocol filters supported. */
#ifndef BNEP_MAX_PROT_FILTERS
#define BNEP_MAX_PROT_FILTERS       5
#endif

/* Maximum number of multicast filters supported. */
#ifndef BNEP_MAX_MULTI_FILTERS
#define BNEP_MAX_MULTI_FILTERS      5
#endif

/* Minimum MTU size. */
#ifndef BNEP_MIN_MTU_SIZE
#define BNEP_MIN_MTU_SIZE           L2CAP_MTU_SIZE
#endif

/* Preferred MTU size. */
#ifndef BNEP_MTU_SIZE
#define BNEP_MTU_SIZE               BNEP_MIN_MTU_SIZE
#endif

/* Maximum size of user data, in bytes.  */
#ifndef BNEP_MAX_USER_DATA_SIZE
#define BNEP_MAX_USER_DATA_SIZE     1500
#endif

/* Maximum number of buffers allowed in transmit data queue. */
#ifndef BNEP_MAX_XMITQ_DEPTH
#define BNEP_MAX_XMITQ_DEPTH        20
#endif

/* Maximum number BNEP of connections supported. */
#ifndef BNEP_MAX_CONNECTIONS
#define BNEP_MAX_CONNECTIONS        7
#endif


/******************************************************************************
**
** AVDTP
**
******************************************************************************/

#ifndef AVDT_INCLUDED
#define AVDT_INCLUDED               FALSE
#endif

/* Include reporting capability in AVDTP */
#ifndef AVDT_REPORTING
#define AVDT_REPORTING              TRUE
#endif

/* Include multiplexing capability in AVDTP */
#ifndef AVDT_MULTIPLEXING
#define AVDT_MULTIPLEXING           TRUE
#endif

/* Number of simultaneous links to different peer devices. */
#ifndef AVDT_NUM_LINKS
#define AVDT_NUM_LINKS              2
#endif

/* Number of simultaneous stream endpoints. */
#ifndef AVDT_NUM_SEPS
#define AVDT_NUM_SEPS               3
#endif
 
/* Number of transport channels setup per media stream(audio or video) */
#ifndef AVDT_NUM_CHANNELS

#if AVDT_REPORTING == TRUE
/* signaling, media and reporting channels */
#define AVDT_NUM_CHANNELS   3
#else
/* signaling and media channels */
#define AVDT_NUM_CHANNELS   2
#endif

#endif
 
/* Number of transport channels setup by AVDT for all media streams 
 * AVDT_NUM_CHANNELS * Number of simultaneous streams.
 */
#ifndef AVDT_NUM_TC_TBL
#define AVDT_NUM_TC_TBL             6
#endif


/* Maximum size in bytes of the codec capabilities information element. */
#ifndef AVDT_CODEC_SIZE
#define AVDT_CODEC_SIZE             10
#endif

/* Maximum size in bytes of the content protection information element. */
#ifndef AVDT_PROTECT_SIZE
#define AVDT_PROTECT_SIZE           90
#endif

/* Maximum number of GKI buffers in the fragment queue (for video frames).
 * Must be less than the number of buffers in the buffer pool of size AVDT_DATA_POOL_SIZE */
#ifndef AVDT_MAX_FRAG_COUNT
#define AVDT_MAX_FRAG_COUNT         15
#endif

/******************************************************************************
**
** PAN
**
******************************************************************************/

#ifndef PAN_INCLUDED
#define PAN_INCLUDED                     FALSE
#endif

/* This will enable the PANU role */
#ifndef PAN_SUPPORTS_ROLE_PANU
#define PAN_SUPPORTS_ROLE_PANU              TRUE
#endif

/* This will enable the GN role */
#ifndef PAN_SUPPORTS_ROLE_GN
#define PAN_SUPPORTS_ROLE_GN                TRUE
#endif

/* This will enable the NAP role */
#ifndef PAN_SUPPORTS_ROLE_NAP
#define PAN_SUPPORTS_ROLE_NAP               TRUE
#endif

/* This is just for debugging purposes */
#ifndef PAN_SUPPORTS_DEBUG_DUMP
#define PAN_SUPPORTS_DEBUG_DUMP             TRUE
#endif


/* Maximum number of PAN connections allowed */
#ifndef MAX_PAN_CONNS
#define MAX_PAN_CONNS                    7
#endif

/* Default service name for NAP role */
#ifndef PAN_NAP_DEFAULT_SERVICE_NAME
#define PAN_NAP_DEFAULT_SERVICE_NAME    "Network Access Point Service"
#endif

/* Default service name for GN role */
#ifndef PAN_GN_DEFAULT_SERVICE_NAME
#define PAN_GN_DEFAULT_SERVICE_NAME     "Group Network Service"
#endif

/* Default service name for PANU role */
#ifndef PAN_PANU_DEFAULT_SERVICE_NAME
#define PAN_PANU_DEFAULT_SERVICE_NAME   "PAN User Service"
#endif

/* Default description for NAP role service */
#ifndef PAN_NAP_DEFAULT_DESCRIPTION
#define PAN_NAP_DEFAULT_DESCRIPTION     "Public NAP"
#endif

/* Default description for GN role service */
#ifndef PAN_GN_DEFAULT_DESCRIPTION
#define PAN_GN_DEFAULT_DESCRIPTION      "Widcomm GN"
#endif

/* Default description for PANU role service */
#ifndef PAN_PANU_DEFAULT_DESCRIPTION
#define PAN_PANU_DEFAULT_DESCRIPTION    "Laptop's PANU"
#endif

/* Default Security level for PANU role. */
#ifndef PAN_PANU_SECURITY_LEVEL
#define PAN_PANU_SECURITY_LEVEL          0
#endif

/* Default Security level for GN role. */
#ifndef PAN_GN_SECURITY_LEVEL
#define PAN_GN_SECURITY_LEVEL            0
#endif

/* Default Security level for NAP role. */
#ifndef PAN_NAP_SECURITY_LEVEL
#define PAN_NAP_SECURITY_LEVEL           0
#endif




/******************************************************************************
**
** GAP
**
******************************************************************************/

#ifndef GAP_INCLUDED
#define GAP_INCLUDED                FALSE
#endif

/* This is set to enable use of GAP L2CAP connections. */
#ifndef GAP_CONN_INCLUDED
#define GAP_CONN_INCLUDED           TRUE
#endif

/* The maximum number of simultaneous GAP L2CAP connections. */
#ifndef GAP_MAX_CONNECTIONS
#define GAP_MAX_CONNECTIONS         8
#endif

/******************************************************************************
**
** CTP
**
******************************************************************************/

#ifndef CTP_INCLUDED
#define CTP_INCLUDED                FALSE
#endif

/* To include CTP gateway functionality or not. */
#ifndef CTP_GW_INCLUDED
#define CTP_GW_INCLUDED             TRUE
#endif

/* The number of terminals supported. */
#ifndef CTP_MAX_NUM_TLS
#define CTP_MAX_NUM_TLS             7
#endif

/* If the controller can not support sniff mode when the SCO is up, set this to FALSE. */
#ifndef CTP_USE_SNIFF_ON_SCO
#define CTP_USE_SNIFF_ON_SCO        FALSE
#endif

/* When ACL link between TL and GW is idle for more than this amount of seconds, the ACL may be put to low power mode. */
#ifndef CTP_TL_IDLE_TIMEOUT
#define CTP_TL_IDLE_TIMEOUT         90
#endif

/* To include CTP terminal functionality or not. */
#ifndef CTP_TL_INCLUDED
#define CTP_TL_INCLUDED             TRUE
#endif

/* To include CTP device discovery functionality or not. */
#ifndef CTP_DISCOVERY_INCLUDED
#define CTP_DISCOVERY_INCLUDED      TRUE
#endif

/* set to TRUE for controllers that do not support multi-point */
#ifndef CTP_TL_WAIT_DISC
#define CTP_TL_WAIT_DISC            TRUE
#endif

/* The CTP inquiry database size. */
#ifndef CTP_INQ_DB_SIZE
#define CTP_INQ_DB_SIZE             CTP_DISC_REC_SIZE
#endif

/* The CTP discovery record size. */
#ifndef CTP_DISC_REC_SIZE
#define CTP_DISC_REC_SIZE           60
#endif

/* CTP TL would try to re-establish L2CAP channel after channel is disconnected for this amount of seconds. */
#ifndef CTP_GUARD_LINK_LOST
#define CTP_GUARD_LINK_LOST         1
#endif

/* The link policy bitmap. */
#ifndef CTP_DEFAULT_LINK_POLICY
#define CTP_DEFAULT_LINK_POLICY     0x000F
#endif

/* The minimum period interval used for the sniff and park modes. */
#ifndef CTP_DEF_LOWPWR_MIN_PERIOD
#define CTP_DEF_LOWPWR_MIN_PERIOD   0x100
#endif

/* The maximum period interval used for the sniff and park modes. */
#ifndef CTP_DEF_LOWPWR_MAX_PERIOD
#define CTP_DEF_LOWPWR_MAX_PERIOD   0x1E0
#endif

/* The number of baseband receive slot sniff attempts. */
#ifndef CTP_DEF_LOWPWR_ATTEMPT
#define CTP_DEF_LOWPWR_ATTEMPT      0x200
#endif

/* The number of baseband receive slots for sniff timeout. */
#ifndef CTP_DEF_LOWPWR_TIMEOUT
#define CTP_DEF_LOWPWR_TIMEOUT      0x200
#endif

/* This is set if CTP is to use park mode. */
#ifndef CTP_PARK_INCLUDED
#define CTP_PARK_INCLUDED           TRUE
#endif

/* This is set if CTP is to use sniff mode. */
#ifndef CTP_SNIFF_INCLUDED
#define CTP_SNIFF_INCLUDED          TRUE
#endif

/* To include CTP data exchange functionality or not. */
#ifndef CTP_DATA_EXCHG_FEATURE
#define CTP_DATA_EXCHG_FEATURE      FALSE
#endif

/* To include CTP GW intercom functionality or not. */
#ifndef CTP_GW_INTERCOM_FEATURE
#define CTP_GW_INTERCOM_FEATURE     FALSE
#endif

/* The MTU size for L2CAP channel. */
#ifndef CTP_MTU_SIZE
#define CTP_MTU_SIZE                200
#endif

/* The L2CAP PSM for the data exchange feature. */
#ifndef CTP_DATA_EXCHG_PSM
#define CTP_DATA_EXCHG_PSM          13
#endif

/* The flush timeout for L2CAP channels. */
#ifndef CTP_FLUSH_TO
#define CTP_FLUSH_TO                0xFFFF
#endif

/* The default service name for CTP. */
#ifndef CTP_DEFAULT_SERVICE_NAME
#define CTP_DEFAULT_SERVICE_NAME    "Cordless Telephony"
#endif

/* The CTP security level. */
#ifndef CTP_SECURITY_LEVEL
#define CTP_SECURITY_LEVEL          (BTM_SEC_IN_AUTHORIZE | BTM_SEC_IN_AUTHENTICATE | BTM_SEC_IN_ENCRYPT)
#endif

/* The number of lines to the external network. */
#ifndef CTP_MAX_LINES
#define CTP_MAX_LINES               1
#endif

/* Test if the number of resources in TCS is consistent with CTP setting. */
#ifndef CTP_TEST_FULL_TCS
#define CTP_TEST_FULL_TCS           TRUE
#endif

/* The default inquiry mode. */
#ifndef CTP_DEFAULT_INQUIRY_MODE
#define CTP_DEFAULT_INQUIRY_MODE    BTM_GENERAL_INQUIRY
#endif

/* The default inquiry duration. */
#ifndef CTP_DEFAULT_INQ_DURATION
#define CTP_DEFAULT_INQ_DURATION    4
#endif

/* The maximum number of inquiry responses. */
#ifndef CTP_DEFAULT_INQ_MAX_RESP
#define CTP_DEFAULT_INQ_MAX_RESP    3
#endif

/* When TL does not create another L2CAP channel within this period of time GW declares that it's "Connected Limited". */
#ifndef CTP_TL_CONN_TIMEOUT
#define CTP_TL_CONN_TIMEOUT         5
#endif

/* The delay for ACL to completely disconnect (for intercom) before sending the connect request to GW. */
#ifndef CTP_RECONNECT_DELAY
#define CTP_RECONNECT_DELAY         5
#endif

/* How many times to retry connection when it has failed. */
#ifndef CTP_RETRY_ON_CONN_ERR
#define CTP_RETRY_ON_CONN_ERR       5
#endif

/******************************************************************************
**
** ICP
**
******************************************************************************/

#ifndef ICP_INCLUDED
#define ICP_INCLUDED                FALSE
#endif

/* The ICP default MTU. */
#ifndef ICP_MTU_SIZE
#define ICP_MTU_SIZE                100
#endif

/* The ICP security level. */
#ifndef ICP_SECURITY_LEVEL
#define ICP_SECURITY_LEVEL          BTM_SEC_NONE
#endif

/* The default service name for ICP. */
#ifndef ICP_DEFAULT_SERVICE_NAME
#define ICP_DEFAULT_SERVICE_NAME    "Intercom"
#endif

/* The flush timeout for L2CAP channels. */
#ifndef ICP_FLUSH_TO
#define ICP_FLUSH_TO                0xFFFF
#endif

/******************************************************************************
**
** SPP
**
******************************************************************************/

#ifndef SPP_INCLUDED
#define SPP_INCLUDED                FALSE
#endif

/* The SPP default MTU. */
#ifndef SPP_DEFAULT_MTU
#define SPP_DEFAULT_MTU             127
#endif

/* The interval, in seconds, that a client tries to reconnect to a service. */
#ifndef SPP_RETRY_CONN_INTERVAL
#define SPP_RETRY_CONN_INTERVAL     1
#endif

/* The SPP discoverable mode: limited or general. */
#ifndef SPP_DISCOVERABLE_MODE
#define SPP_DISCOVERABLE_MODE       BTM_GENERAL_DISCOVERABLE
#endif

/* The maximum number of inquiry results returned in by inquiry procedure. */
#ifndef SPP_DEF_INQ_MAX_RESP
#define SPP_DEF_INQ_MAX_RESP        10
#endif

/* The SPP discovery record size. */
#ifndef SPP_DISC_REC_SIZE
#define SPP_DISC_REC_SIZE           60
#endif

#ifndef SPP_MAX_RECS_PER_DEVICE
#define SPP_MAX_RECS_PER_DEVICE     (SPP_DB_SIZE / SPP_DISC_REC_SIZE)
#endif

/* Inquiry duration in 1.28 second units. */
#ifndef SPP_DEF_INQ_DURATION
#define SPP_DEF_INQ_DURATION        9
#endif

/* keep the raw data received from SDP server in database. */
#ifndef SDP_RAW_DATA_INCLUDED
#define SDP_RAW_DATA_INCLUDED       FALSE
#endif

/* TRUE, to allow JV to create L2CAP connection on SDP PSM. */
#ifndef SDP_FOR_JV_INCLUDED
#define SDP_FOR_JV_INCLUDED         FALSE
#endif

/* Inquiry duration in 1.28 second units. */
#ifndef SDP_DEBUG
#define SDP_DEBUG                   TRUE
#endif

/******************************************************************************
**
** HSP2, HFP
**
******************************************************************************/

#ifndef HSP2_INCLUDED
#define HSP2_INCLUDED               FALSE
#endif

/* Include the ability to perform inquiry for peer devices. */
#ifndef HSP2_INQUIRY_INCLUDED
#define HSP2_INQUIRY_INCLUDED       TRUE
#endif

/* Include Audio Gateway specific code. */
#ifndef HSP2_AG_INCLUDED
#define HSP2_AG_INCLUDED            TRUE
#endif

/* Include Headset Specific Code. */
#ifndef HSP2_HS_INCLUDED
#define HSP2_HS_INCLUDED            TRUE
#endif

/* Include the ability to open an SCO connection for In-Band Ringing. */
#ifndef HSP2_IB_RING_INCLUDED
#define HSP2_IB_RING_INCLUDED       TRUE
#endif

/* Include the ability to repeat a ring. */
#ifndef HSP2_AG_REPEAT_RING
#define HSP2_AG_REPEAT_RING         TRUE
#endif

#ifndef HSP2_APP_CLOSES_ON_CKPD
#define HSP2_APP_CLOSES_ON_CKPD     FALSE
#endif


/* Include the ability to park a connection. */
#ifndef HSP2_PARK_INCLUDED
#define HSP2_PARK_INCLUDED          TRUE
#endif

/* Include HSP State Machine debug trace messages. */
#ifndef HSP2_FSM_DEBUG
#define HSP2_FSM_DEBUG              TRUE
#endif

/* The Module's Inquiry Scan Window. */
#ifndef HSP2_INQ_SCAN_WINDOW
#define HSP2_INQ_SCAN_WINDOW        0
#endif

/* The Module's Inquiry Scan Interval. */
#ifndef HSP2_INQ_SCAN_INTERVAL
#define HSP2_INQ_SCAN_INTERVAL      0
#endif

/* The Module's Page Scan Interval. */
#ifndef HSP2_PAGE_SCAN_INTERVAL
#define HSP2_PAGE_SCAN_INTERVAL     0
#endif

/* The Module's Page Scan Window. */
#ifndef HSP2_PAGE_SCAN_WINDOW
#define HSP2_PAGE_SCAN_WINDOW       0
#endif

/* The Park Mode's Minimum Beacon Period. */
#ifndef HSP2_BEACON_MIN_PERIOD
#define HSP2_BEACON_MIN_PERIOD      450
#endif

/* The Park Mode's Maximum Beacon Period. */
#ifndef HSP2_BEACON_MAX_PERIOD
#define HSP2_BEACON_MAX_PERIOD      500
#endif

/* The duration of the inquiry in seconds. */
#ifndef HSP2_INQ_DURATION
#define HSP2_INQ_DURATION           4
#endif

/* Maximum number of peer responses during an inquiry. */
#ifndef HSP2_INQ_MAX_NUM_RESPS
#define HSP2_INQ_MAX_NUM_RESPS      3
#endif

/* Maximum number of times to retry an inquiry prior to failure. */
#ifndef HSP2_MAX_INQ_RETRY
#define HSP2_MAX_INQ_RETRY          6
#endif

/* Maximum number of times to retry an RFCOMM connection prior to failure. */
#ifndef HSP2_MAX_CONN_RETRY
#define HSP2_MAX_CONN_RETRY         3
#endif

/* If the connect request failed for authentication reasons, do not retry */
#ifndef HSP2_NO_RETRY_ON_AUTH_FAIL
#define HSP2_NO_RETRY_ON_AUTH_FAIL  TRUE
#endif

/* Maximum number of characters in an HSP2 device name. */
#ifndef HSP2_MAX_NAME_LEN
#define HSP2_MAX_NAME_LEN           32
#endif

/* The minimum speaker and/or microphone gain setting. */
#ifndef HSP2_MIN_GAIN
#define HSP2_MIN_GAIN               0
#endif

/* The maximum speaker and/or microphone setting. */
#ifndef HSP2_MAX_GAIN
#define HSP2_MAX_GAIN               15
#endif

/* The default value to send on an AT+CKPD. */
#ifndef HSP2_KEYPRESS_DEFAULT
#define HSP2_KEYPRESS_DEFAULT       200
#endif

/* Maximum amount a data that can be received per RFCOMM frame. */
#ifndef HSP2_MAX_RFC_READ_LEN
#define HSP2_MAX_RFC_READ_LEN       128
#endif

/* The time in seconds to wait for completion of a partial AT command or response from the peer. */
#ifndef HSP2_AT_TO_INTERVAL
#define HSP2_AT_TO_INTERVAL         30
#endif

/* The time to wait before repeating a ring to a peer Headset. */
#ifndef HSP2_REPEAT_RING_TO
#define HSP2_REPEAT_RING_TO         4
#endif

/* Time to wait for a response for an AT command */
#ifndef HSP2_AT_RSP_TO
#define HSP2_AT_RSP_TO              20
#endif

/* SCO packet type(s) to use (bitmask: see spec), 0 - device default (recommended) */
#ifndef HSP2_SCO_PKT_TYPES
#define HSP2_SCO_PKT_TYPES          ((UINT16)0x0000) 
#endif

/* The default settings of the SCO voice link. */
#ifndef HSP2_DEFAULT_VOICE_SETTINGS
#define HSP2_DEFAULT_VOICE_SETTINGS (HCI_INP_CODING_LINEAR | HCI_INP_DATA_FMT_2S_COMPLEMENT | HCI_INP_SAMPLE_SIZE_16BIT | HCI_AIR_CODING_FORMAT_CVSD)
#endif

#ifndef HSP2_MAX_AT_CMD_LENGTH
#define HSP2_MAX_AT_CMD_LENGTH       16
#endif

#ifndef HSP2_MAX_AT_VAL_LENGTH
#if (defined(HFP_INCLUDED) && HFP_INCLUDED == TRUE) 
#define HSP2_MAX_AT_VAL_LENGTH       310
#else
#define HSP2_MAX_AT_VAL_LENGTH       5
#endif
#endif


#ifndef HSP2_SDP_DB_SIZE
#define HSP2_SDP_DB_SIZE             300
#endif


/******************************************************************************
**
** HFP
**
******************************************************************************/

#ifndef HFP_INCLUDED
#define HFP_INCLUDED                FALSE
#endif

/* Include Audio Gateway specific code. */
#ifndef HFP_AG_INCLUDED
#define HFP_AG_INCLUDED             TRUE
#endif

/* Include Hand Free Specific Code. */
#ifndef HFP_HF_INCLUDED
#define HFP_HF_INCLUDED             TRUE
#endif

/* Use AT interface instead of full blown API */
#ifndef AT_INTERFACE
#define AT_INTERFACE            FALSE
#endif

/* HFP Manages SCO establishement for various procedures */
#ifndef HFP_SCO_MGMT_INCLUDED
#define HFP_SCO_MGMT_INCLUDED             TRUE
#endif

/* CCAP compliant features and behavior desired */
#ifndef CCAP_COMPLIANCE
#define CCAP_COMPLIANCE             TRUE
#endif

/* Caller ID string, part of +CLIP result code */
#ifndef HFP_MAX_CLIP_INFO
#define HFP_MAX_CLIP_INFO             45
#endif

#ifndef HFP_RPT_PEER_INFO_INCLUDED
#define HFP_RPT_PEER_INFO_INCLUDED  TRUE  /* Reporting of peer features enabled */
#endif

/******************************************************************************
**
** HID
**
******************************************************************************/

/* HID Device Role Included */
#ifndef HID_DEV_INCLUDED
#define HID_DEV_INCLUDED             FALSE
#endif

#ifndef HID_DEV_PM_INCLUDED
#define HID_DEV_PM_INCLUDED         TRUE
#endif

/* The HID Device is a virtual cable */
#ifndef HID_DEV_VIRTUAL_CABLE
#define HID_DEV_VIRTUAL_CABLE       TRUE
#endif

/* The HID device initiates the reconnections */
#ifndef HID_DEV_RECONN_INITIATE
#define HID_DEV_RECONN_INITIATE     TRUE
#endif

/* THe HID device is normally connectable */
#ifndef HID_DEV_NORMALLY_CONN
#define HID_DEV_NORMALLY_CONN       FALSE
#endif

/* The device is battery powered */
#ifndef HID_DEV_BATTERY_POW
#define HID_DEV_BATTERY_POW         TRUE
#endif

/* Device is capable of waking up the host */
#ifndef HID_DEV_REMOTE_WAKE
#define HID_DEV_REMOTE_WAKE         TRUE
#endif

/* Device needs host to close SDP channel after SDP is over */
#ifndef HID_DEV_SDP_DISABLE
#define HID_DEV_SDP_DISABLE         TRUE
#endif

#ifndef HID_DEV_MTU_SIZE
#define HID_DEV_MTU_SIZE                 64
#endif

#ifndef HID_DEV_FLUSH_TO
#define HID_DEV_FLUSH_TO                 0xffff
#endif

#ifndef HID_DEV_PAGE_SCAN_WIN
#define HID_DEV_PAGE_SCAN_WIN       (0)
#endif

#ifndef HID_DEV_PAGE_SCAN_INT
#define HID_DEV_PAGE_SCAN_INT       (0)
#endif

#ifndef HID_DEV_MAX_CONN_RETRY
#define HID_DEV_MAX_CONN_RETRY      (15)
#endif

#ifndef HID_DEV_REPAGE_WIN
#define HID_DEV_REPAGE_WIN          (1)
#endif

#ifndef HID_DEV_SVC_NAME
#define HID_DEV_SVC_NAME            "HID"
#endif

#ifndef HID_DEV_SVC_DESCR
#define HID_DEV_SVC_DESCR           "3-button mouse and keyboard"
#endif

#ifndef HID_DEV_PROVIDER_NAME
#define HID_DEV_PROVIDER_NAME       "Widcomm"
#endif

#ifndef HID_DEV_REL_NUM
#define HID_DEV_REL_NUM             0x0100
#endif

#ifndef HID_DEV_PARSER_VER
#define HID_DEV_PARSER_VER          0x0111
#endif

#ifndef HID_DEV_SUBCLASS
#define HID_DEV_SUBCLASS            COD_MINOR_POINTING
#endif

#ifndef HID_DEV_COUNTRY_CODE
#define HID_DEV_COUNTRY_CODE        0x33
#endif

#ifndef HID_DEV_SUP_TOUT
#define HID_DEV_SUP_TOUT            0x8000
#endif

#ifndef HID_DEV_NUM_LANGS
#define HID_DEV_NUM_LANGS           1
#endif

#ifndef HID_DEV_INACT_TIMEOUT
#define HID_DEV_INACT_TIMEOUT       60
#endif

#ifndef HID_DEV_BUSY_MODE_PARAMS
#define HID_DEV_BUSY_MODE_PARAMS    { 320, 160, 10, 20, HCI_MODE_ACTIVE }
#endif

#ifndef HID_DEV_IDLE_MODE_PARAMS
#define HID_DEV_IDLE_MODE_PARAMS    { 320, 160, 10, 20, HCI_MODE_SNIFF }
#endif

#ifndef HID_DEV_SUSP_MODE_PARAMS
#define HID_DEV_SUSP_MODE_PARAMS    { 640, 320,  0,    0, HCI_MODE_PARK }
#endif

#ifndef HID_DEV_MAX_DESCRIPTOR_SIZE
#define HID_DEV_MAX_DESCRIPTOR_SIZE      128     /* Max descriptor size          */
#endif

#ifndef HID_DEV_LANGUAGELIST
#define HID_DEV_LANGUAGELIST             {0x35, 0x06, 0x09, 0x04, 0x09, 0x09, 0x01, 0x00}
#endif

#ifndef HID_DEV_LINK_SUPERVISION_TO
#define HID_DEV_LINK_SUPERVISION_TO      0x8000
#endif

#ifndef HID_CONTROL_POOL_ID
#define HID_CONTROL_POOL_ID             2   
#endif

#ifndef HID_INTERRUPT_POOL_ID
#define HID_INTERRUPT_POOL_ID           2   
#endif


/*************************************************************************
** Definitions for Both HID-Host & Device 
*/
#ifndef HID_MAX_SVC_NAME_LEN
#define HID_MAX_SVC_NAME_LEN  32
#endif

#ifndef HID_MAX_SVC_DESCR_LEN
#define HID_MAX_SVC_DESCR_LEN 32
#endif

#ifndef HID_MAX_PROV_NAME_LEN
#define HID_MAX_PROV_NAME_LEN 32
#endif

/*************************************************************************
** Definitions for HID-Host
*/
#ifndef  HID_HOST_INCLUDED
#define HID_HOST_INCLUDED           FALSE
#endif

#ifndef HID_HOST_MAX_DEVICES
#define HID_HOST_MAX_DEVICES        7
#endif

#ifndef HID_HOST_MTU
#define HID_HOST_MTU                640
#endif

#ifndef HID_HOST_FLUSH_TO
#define HID_HOST_FLUSH_TO                 0xffff
#endif

#ifndef HID_HOST_MAX_CONN_RETRY
#define HID_HOST_MAX_CONN_RETRY     (15)
#endif

#ifndef HID_HOST_REPAGE_WIN
#define HID_HOST_REPAGE_WIN          (2)
#endif


/******************************************************************************
**
** DUN and FAX
**
******************************************************************************/

#ifndef DUN_INCLUDED
#define DUN_INCLUDED                FALSE
#endif


/******************************************************************************
**
** GOEP
**
******************************************************************************/

#ifndef GOEP_INCLUDED
#define GOEP_INCLUDED               FALSE
#endif

/* This is set to enable GOEP non-blocking file system access functions. */
#ifndef GOEP_FS_INCLUDED
#define GOEP_FS_INCLUDED        TRUE
#endif

/* GOEP authentication key size. */
#ifndef GOEP_MAX_AUTH_KEY_SIZE
#define GOEP_MAX_AUTH_KEY_SIZE      16
#endif

/* Maximum size of the realm authentication string. */
#ifndef GOEP_MAX_AUTH_REALM_SIZE
#define GOEP_MAX_AUTH_REALM_SIZE    16
#endif

/* Realm Character Set */
#ifndef GOEP_REALM_CHARSET
#define GOEP_REALM_CHARSET          0       /* ASCII */
#endif

/* This is set to the maximum length of path name allowed in the system (_MAX_PATH). */
#ifndef GOEP_MAX_PATH_SIZE
#define GOEP_MAX_PATH_SIZE          255
#endif

/* Specifies whether or not client's user id is required during obex authentication */
#ifndef GOEP_SERVER_USERID_REQUIRED
#define GOEP_SERVER_USERID_REQUIRED FALSE
#endif

/* This is set to the maximum length of file name allowed in the system (_MAX_FNAME). */
#ifndef GOEP_MAX_FILE_SIZE
#define GOEP_MAX_FILE_SIZE          128
#endif

/* Character used as path separator */
#ifndef GOEP_PATH_SEPARATOR
#define GOEP_PATH_SEPARATOR         ((char) 0x5c)   /* 0x2f ('/'), or 0x5c ('\') */
#endif

/******************************************************************************
**
** OPP
**
******************************************************************************/

#ifndef OPP_INCLUDED
#define OPP_INCLUDED                FALSE
#endif

/* This is set to enable OPP client capabilities. */
#ifndef OPP_CLIENT_INCLUDED
#define OPP_CLIENT_INCLUDED         FALSE
#endif

/* This is set to enable OPP server capabilities. */
#ifndef OPP_SERVER_INCLUDED
#define OPP_SERVER_INCLUDED         FALSE
#endif

/* if the optional formating functions are to be included or not */
#ifndef OPP_FORMAT_INCLUDED
#define OPP_FORMAT_INCLUDED         FALSE
#endif

/* Maximum number of client sessions allowed by server */
#ifndef OPP_MAX_SRVR_SESS
#define OPP_MAX_SRVR_SESS           3
#endif

/******************************************************************************
**
** FTP
**
******************************************************************************/

#ifndef FTP_INCLUDED
#define FTP_INCLUDED                FALSE
#endif

/* This is set to enable FTP client capabilities. */
#ifndef FTP_CLIENT_INCLUDED
#define FTP_CLIENT_INCLUDED         TRUE
#endif

/* This is set to enable FTP server capabilities. */
#ifndef FTP_SERVER_INCLUDED
#define FTP_SERVER_INCLUDED         TRUE
#endif

/******************************************************************************
**
** XML Parser
**
******************************************************************************/

#ifndef XML_STACK_SIZE
#define XML_STACK_SIZE             7
#endif

/******************************************************************************
**
** BPP Printer
**
******************************************************************************/
#ifndef BPP_DEBUG
#define BPP_DEBUG            FALSE
#endif

#ifndef BPP_INCLUDED
#define BPP_INCLUDED                FALSE
#endif

#ifndef BPP_SND_INCLUDED
#define BPP_SND_INCLUDED            FALSE
#endif

/* Maximum number of senders allowed to connect simultaneously
** The maximum is 6 or (OBX_NUM_SERVERS / 2), whichever is smaller
*/
#ifndef BPP_PR_MAX_CON
#define BPP_PR_MAX_CON         3
#endif

/* Service Name. maximum length: 248
#ifndef BPP_SERVICE_NAME
#define BPP_SERVICE_NAME            "Basic Printing"
#endif
 */
/* Document Format Supported. ASCII comma-delimited list of MIME type:version string
#ifndef BPP_DOC_FORMAT_SUPPORTED
#define BPP_DOC_FORMAT_SUPPORTED    "application/vnd.pwg-xhtml-print:1.0,application/vnd.hp-PCL:5E,application/PDF"
#endif

#ifndef BPP_DOC_FORMAT_SUPPORTED_LEN
#define BPP_DOC_FORMAT_SUPPORTED_LEN    77
#endif
 */
/* Character repertoires.
#ifndef BPP_CHARACTER_REPERTOIRES
#define BPP_CHARACTER_REPERTOIRES {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}
#endif
 */
/* XHTML formats.
#ifndef BPP_XHTML_PRINT_FORMATS
#define BPP_XHTML_PRINT_FORMATS     "image/gif:89A,image/jpeg"
#endif

#ifndef BPP_XHTML_PRINT_FORMATS_LEN
#define BPP_XHTML_PRINT_FORMATS_LEN 24
#endif
 */
/* Color supported.
#ifndef BPP_COLOR_SUPORTED
#define BPP_COLOR_SUPORTED          FALSE
#endif
 */
/* 1284 ID string. First 2 bytes are the length.
#ifndef BPP_1284ID
#define BPP_1284ID                  "\x00\x48MANUFACTURER:ACME Manufacturing;COMMAND SET:PCL,MPL;MODEL:LaserBeam \?;"
#endif

#ifndef BPP_1284ID_LEN
#define BPP_1284ID_LEN              72
#endif
 */
/* Printer name.
#ifndef BPP_PRINTER_NAME
#define BPP_PRINTER_NAME            "My Printer"
#endif

#ifndef BPP_PRINTER_NAME_LEN
#define BPP_PRINTER_NAME_LEN        10
#endif
 */

/* Printer location.
#ifndef BPP_PRINTER_LOCATION
#define BPP_PRINTER_LOCATION        "Hotel Lobby"
#endif

#ifndef BPP_PRINTER_LOCATION_LEN
#define BPP_PRINTER_LOCATION_LEN    11
#endif
 */
/* Duplex printing supported.
#ifndef BPP_DUPLEX_SUPPORTED
#define BPP_DUPLEX_SUPPORTED        TRUE
#endif
 */

/* Media types supported.
#ifndef BPP_MEDIA_TYPES_SUPPORTED
#define BPP_MEDIA_TYPES_SUPPORTED   "stationary,continuous-long,photographic-high-gloss,cardstock"
#endif

#ifndef BPP_MEDIA_TYPES_SUPPORTED_LEN
#define BPP_MEDIA_TYPES_SUPPORTED_LEN   60
#endif
 */
/* Maximum media with supported.
#ifndef BPP_MAX_MEDIA_WIDTH
#define BPP_MAX_MEDIA_WIDTH         205
#endif
 */
/* Maximum media length supported.
#ifndef BPP_MAX_MEDIA_LENGTH
#define BPP_MAX_MEDIA_LENGTH        285
#endif
 */
/* the maximum string len for the media size of medium loaded */
#ifndef BPP_MEDIA_SIZE_LEN
#define BPP_MEDIA_SIZE_LEN          33
#endif

/* Debug Trace the SOAP object, if TRUE */
#ifndef BPP_TRACE_XML
#define BPP_TRACE_XML               TRUE
#endif

/* in case that the SOAP object does not all come in one OBEX packet,
 * this size of data may be kept in the BPP control block for continuing parsing.
 * The maximum is the size of the biggest GKI buffer (GKI_MAX_BUF_SIZE) */
#ifndef BPP_SOAP_KEEP_SIZE
#define BPP_SOAP_KEEP_SIZE          200
#endif


/******************************************************************************
**
** BIP
**
******************************************************************************/
#ifndef BIP_INCLUDED
#define BIP_INCLUDED                FALSE
#endif

/* TRUE to include imaging initiator */
#ifndef BIP_INITR_INCLUDED
#define BIP_INITR_INCLUDED          FALSE
#endif

/* TRUE to include imaging responder */
#ifndef BIP_RSPDR_INCLUDED
#define BIP_RSPDR_INCLUDED          FALSE
#endif

/* TRUE to include image push feature */
#ifndef BIP_PUSH_INCLUDED
#define BIP_PUSH_INCLUDED           TRUE
#endif

/* TRUE to include image pull feature */
#ifndef BIP_PULL_INCLUDED
#define BIP_PULL_INCLUDED           TRUE
#endif

/* TRUE to include advanced image printing feature */
#ifndef BIP_PRINTING_INCLUDED
#define BIP_PRINTING_INCLUDED       TRUE
#endif

/* TRUE to include automatic archive feature */
#ifndef BIP_ARCHIVE_INCLUDED
#define BIP_ARCHIVE_INCLUDED        TRUE
#endif

/* TRUE to include remote camera feature */
#ifndef BIP_CAMERA_INCLUDED
#define BIP_CAMERA_INCLUDED         TRUE
#endif

/* TRUE to include remote display feature */
#ifndef BIP_DISPLAY_INCLUDED
#define BIP_DISPLAY_INCLUDED        TRUE
#endif

/* TRUE to include sanity check code for API functions */
#ifndef BIP_SANITY_CHECKS
#define BIP_SANITY_CHECKS           TRUE
#endif

/* TRUE to show the received XML object in trace for conformance tests */
#ifndef BIP_TRACE_XML
#define BIP_TRACE_XML               TRUE    
#endif

/* in case that the received XML object is not complete, the XML parser state machine needs
 * to keep a copy of the data from the last '<'
 * This macro specifies the maximun amount of data for this purpose */ 
#ifndef BIP_XML_CARRY_OVER_LEN
#define BIP_XML_CARRY_OVER_LEN      100     
#endif

/* minimum 4, maximum is 255. The value should be set to the maximum size of encoding string + 1. JPEG2000.
 * If vendor specific format is supported, it might be bigger than 9 */
#ifndef BIP_IMG_ENCODE_SIZE
#define BIP_IMG_ENCODE_SIZE         9  
#endif

/* MIME type: text/plain */
#ifndef BIP_TYPE_SIZE
#define BIP_TYPE_SIZE               20 
#endif

/* example: iso-8895-1 */
#ifndef BIP_CHARSET_SIZE
#define BIP_CHARSET_SIZE            10 
#endif

/* friendly name */
#ifndef BIP_FNAME_SIZE
#define BIP_FNAME_SIZE              20
#endif

/* service name */
#ifndef BIP_SNAME_SIZE
#define BIP_SNAME_SIZE              60
#endif

/* temporary storage file name(for file system access, may include path) */
#ifndef BIP_TEMP_NAME_SIZE
#define BIP_TEMP_NAME_SIZE          200
#endif

/* image file name */
#ifndef BIP_IMG_NAME_SIZE
#define BIP_IMG_NAME_SIZE           200
#endif

/* attachment file name */
#ifndef BIP_ATT_NAME_SIZE
#define BIP_ATT_NAME_SIZE           200
#endif

/* object (image, attachment, thumbnail) file name (may be used for file system) */
#ifndef BIP_OBJ_NAME_SIZE
#define BIP_OBJ_NAME_SIZE           200
#endif



/******************************************************************************
**
** HCRP
**
******************************************************************************/

#ifndef HCRP_INCLUDED
#define HCRP_INCLUDED               FALSE
#endif

/* This is set to enable server. */
#ifndef HCRP_SERVER_INCLUDED
#define HCRP_SERVER_INCLUDED       FALSE
#endif

/* This is set to enable client. */
#ifndef HCRP_CLIENT_INCLUDED
#define HCRP_CLIENT_INCLUDED        FALSE
#endif

/* TRUE enables the notification option of the profile. */
#ifndef HCRP_NOTIFICATION_INCLUDED
#define HCRP_NOTIFICATION_INCLUDED  TRUE
#endif

/* TRUE enables the vendor specific option of the profile. */
#ifndef HCRP_VENDOR_SPEC_INCLUDED
#define HCRP_VENDOR_SPEC_INCLUDED   TRUE
#endif

/* TRUE enables state machine traces. */
#ifndef HCRP_FSM_DEBUG
#define HCRP_FSM_DEBUG              FALSE
#endif

/* TRUE enables protocol message traces. */
#ifndef HCRP_PROTO_DEBUG
#define HCRP_PROTO_DEBUG            FALSE
#endif

/* Maximum length used to store the service name (Minimum 1). */
#ifndef HCRP_MAX_SERVICE_NAME_LEN
#define HCRP_MAX_SERVICE_NAME_LEN   32
#endif

/* Maximum length used to store the device name (Minimum 1). */
#ifndef HCRP_MAX_DEVICE_NAME_LEN
#define HCRP_MAX_DEVICE_NAME_LEN    32
#endif

/* Maximum length of device location (Minimum 1) */
#ifndef HCRP_MAX_DEVICE_LOC_LEN
#define HCRP_MAX_DEVICE_LOC_LEN     32
#endif

/* Maximum length used to store the friendly name (Minimum 1). */
#ifndef HCRP_MAX_FRIENDLY_NAME_LEN
#define HCRP_MAX_FRIENDLY_NAME_LEN  32
#endif

/* Maximum length used to store the 1284 id string (Minimum 2 byte length field). */
#ifndef HCRP_MAX_SDP_1284_ID_LEN
#define HCRP_MAX_SDP_1284_ID_LEN    128
#endif

/* Maximum length for parameters to be processed for vendor specific commands. */
#ifndef HCRP_MAX_VEND_SPEC_LEN
#define HCRP_MAX_VEND_SPEC_LEN      4
#endif

/* Number of seconds to wait for 2nd GAP to open. */
#ifndef HCRP_OPEN_CHAN_TOUT
#define HCRP_OPEN_CHAN_TOUT         5
#endif

/* Number of seconds to wait for 2nd GAP to close. */
#ifndef HCRP_CLOSE_CHAN_TOUT
#define HCRP_CLOSE_CHAN_TOUT        3
#endif

/* Number of seconds to wait for the application to respond to a protocol request. */
#ifndef HCRP_APPL_RSP_TOUT
#define HCRP_APPL_RSP_TOUT          5
#endif

/* Number of seconds to wait for the peer device to respond to a protocol request. */
#ifndef HCRP_CMD_RSP_TOUT
#define HCRP_CMD_RSP_TOUT           7
#endif

/* Number of seconds between subsequent credit requests to the server when the send watermark has been exceeded. */
#ifndef HCRP_CREDIT_REQ_UPDATES
#define HCRP_CREDIT_REQ_UPDATES     1
#endif

/* Maximum number of results to return in a HCRP_FindServices search. */
#ifndef HCRP_MAX_SEARCH_RESULTS
#define HCRP_MAX_SEARCH_RESULTS     1
#endif

/* Maximum number of bytes to be reserved for searching for the client's notification record. */
#ifndef HCRP_MAX_NOTIF_DISC_BUF
#define HCRP_MAX_NOTIF_DISC_BUF     300
#endif

/* Maximum number of clients the server will allow to be registered for notifications. */
#ifndef HCRP_MAX_NOTIF_CLIENTS
#define HCRP_MAX_NOTIF_CLIENTS      3
#endif

/* Spec says minimum of two notification retries. */
#ifndef HCRP_NOTIF_NUM_RETRIES
#define HCRP_NOTIF_NUM_RETRIES      4
#endif

/*************************************************************************
** Definitions for Multi-Client Server HCRP
** Note: Many of the above HCRP definitions are also used
** Maximum number of clients allowed to connect simultaneously
** Must be less than ((GAP_MAX_CONNECTIONS - 1) / 2)
*/
#ifndef HCRPM_MAX_CLIENTS
#define HCRPM_MAX_CLIENTS           3
#endif


/******************************************************************************
**
** PAN
**
******************************************************************************/

#ifndef PAN_INCLUDED
#define PAN_INCLUDED                FALSE
#endif


/******************************************************************************
**
** SAP
**
******************************************************************************/

#ifndef SAP_SERVER_INCLUDED
#define SAP_SERVER_INCLUDED         FALSE
#endif


/*************************************************************************
 * A2DP Definitions
 */
#ifndef A2D_INCLUDED
#define A2D_INCLUDED            FALSE
#endif

/* TRUE to include SBC utility functions */
#ifndef A2D_SBC_INCLUDED
#define A2D_SBC_INCLUDED        A2D_INCLUDED
#endif

/* TRUE to include MPEG-1,2 (mp3) utility functions */
#ifndef A2D_M12_INCLUDED
#define A2D_M12_INCLUDED        A2D_INCLUDED
#endif

/* TRUE to include MPEG-2,4 (aac) utility functions */
#ifndef A2D_M24_INCLUDED
#define A2D_M24_INCLUDED        A2D_INCLUDED
#endif

/*************************************************************************
 * VDP Definitions
 */
#ifndef VDP_INCLUDED
#define VDP_INCLUDED            FALSE
#endif

/******************************************************************************
**
** AVCTP
**
******************************************************************************/

#ifndef AVCT_INCLUDED
#define AVCT_INCLUDED               FALSE
#endif

/* Number of simultaneous ACL links to different peer devices. */
#ifndef AVCT_NUM_LINKS
#define AVCT_NUM_LINKS              2
#endif

/* Number of simultaneous AVCTP connections. */
#ifndef AVCT_NUM_CONN
#define AVCT_NUM_CONN               3
#endif

/******************************************************************************
**
** AVRCP
**
******************************************************************************/

#ifndef AVRC_INCLUDED
#define AVRC_INCLUDED               FALSE
#endif

#ifndef AVRC_METADATA_INCLUDED
#define AVRC_METADATA_INCLUDED      TRUE
#endif

#ifndef AVRC_METADATA_DEBUG
#define AVRC_METADATA_DEBUG         TRUE
#endif

/******************************************************************************
**
** SER
**
******************************************************************************/

#ifndef SER_INCLUDED
#define SER_INCLUDED                FALSE
#endif

/* Task which runs the serial application. */
#ifndef SER_TASK
#define SER_TASK                    BTE_APPL_TASK
#endif

/* Mailbox used by serial application. */
#ifndef SER_MBOX
#define SER_MBOX                    TASK_MBOX_1
#endif

/* Mailbox mask. */
#ifndef SER_MBOX_MASK
#define SER_MBOX_MASK               TASK_MBOX_1_EVT_MASK
#endif

/* TX path application event. */
#ifndef SER_TX_PATH_APPL_EVT
#define SER_TX_PATH_APPL_EVT        EVENT_MASK(APPL_EVT_3)
#endif

/* RX path application event. */
#ifndef SER_RX_PATH_APPL_EVT
#define SER_RX_PATH_APPL_EVT        EVENT_MASK(APPL_EVT_4)
#endif

/******************************************************************************
**
** HCI Services
**
******************************************************************************/
/* Event mask reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_EVT_MASK
#define HCISU_EVT_MASK              EVENT_MASK(APPL_EVT_0)    
#endif

/* MBox reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_MBOX
#define HCISU_MBOX                  TASK_MBOX_2               
#endif

/* MBox event mask reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_MBOX_EVT_MASK
#define HCISU_MBOX_EVT_MASK         TASK_MBOX_2_EVT_MASK               
#endif

/* Timer reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_TIMER
#define HCISU_TIMER                 TIMER_2          
#endif

/* Timer event mask reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_TIMER_EVT_MASK
#define HCISU_TIMER_EVT_MASK        TIMER_2_EVT_MASK          
#endif

/******************************************************************************
**
** HCI Services (H5 3 wired uart), H4 plus SLIP enabled
**
******************************************************************************/
#ifndef SLIP_INCLUDED
#define SLIP_INCLUDED                   TRUE
#endif

#ifndef SLIP_STATIS_INCLUDED
#define SLIP_STATIS_INCLUDED            TRUE
#endif

#ifndef SLIP_SW_FLOW_CTRL
#define SLIP_SW_FLOW_CTRL               TRUE
#endif

#ifndef BT_TRACE_SLIP
#define BT_TRACE_SLIP                   FALSE
#endif

#ifndef SLIP_HOST_SLIDING_WINDOW_SIZE
#define SLIP_HOST_SLIDING_WINDOW_SIZE   7
#endif

/******************************************************************************
**
** Sleep Mode (Low Power Mode)
**
******************************************************************************/
#ifndef HCILP_INCLUDED
#define HCILP_INCLUDED                  TRUE
#endif

/* sleep mode 

    0: disable
    1: UART with Host wake/BT wake out of band signals
    4: H4IBSS, UART with in band signal without Host/BT wake
    9: H5 with in band signal of SLIP without Host/BT wake   
*/
#ifndef HCILP_SLEEP_MODE
#define HCILP_SLEEP_MODE                (0)   
#endif

/* Host Stack Idle Threshold in 300ms 

  In sleep mode 1, this is the number of firmware loops executed with no activity 
    before the Host wake line is deasserted. Activity includes HCI traffic excluding
    certain sleep mode commands and the presence of SCO connections if the 
    "Allow Host Sleep During SCO" flag is not set to 1. Each count of this 
    parameter is roughly equivalent to 300 ms.

  In sleep mode 4, this is used by host to send SLEEP_REQ when no activity.

  In sleep mode 9, this is not used by host.See SLIP_SLEEP_TO.
*/
#ifndef HCILP_IDLE_THRESHOLD
#define HCILP_IDLE_THRESHOLD             (1)   
#endif

/* Host Controller Idle Threshold in 300ms
    This is the number of firmware loops executed with no activity before the HC is 
    considered idle. Depending on the mode, HC may then attempt to sleep. 
    Activity includes HCI traffic excluding certain sleep mode commands and 
    the presence of ACL/SCO connections.
*/
#ifndef HCILP_HC_IDLE_THRESHOLD
#define HCILP_HC_IDLE_THRESHOLD          (1)
#endif

/* GPIO for BT_WAKE signal */
/* Not applicable for sleep mode 4 and 9 */
#ifndef HCILP_BT_WAKE_GPIO
#define HCILP_BT_WAKE_GPIO              UPIO_GENERAL1
#endif

/* GPIO for HOST_WAKE signal */
/* Not applicable for sleep mode 4 and 9 */
#ifndef HCILP_HOST_WAKE_GPIO
#define HCILP_HOST_WAKE_GPIO            UPIO_GENERAL2
#endif

/* BT_WAKE Polarity - 0=Active Low, 1= Active High */
/* Not applicable for sleep mode 4 and 9 */
#ifndef HCILP_BT_WAKE_POLARITY
#define HCILP_BT_WAKE_POLARITY          0
#endif

/* HOST_WAKE Polarity - 0=Active Low, 1= Active High */
/* Not applicable for sleep mode 4 and 9 */
#ifndef HCILP_HOST_WAKE_POLARITY
#define HCILP_HOST_WAKE_POLARITY        0
#endif

/* HCILP_ALLOW_HOST_SLEEP_DURING_SCO

    When this flag is set to 0, the host is not allowed to sleep while
    an SCO is active. In sleep mode 1, the device will keep the host
    wake line asserted while an SCO is active.
    When this flag is set to 1, the host can sleep while an SCO is active.
    This flag should only be set to 1 if SCO traffic is directed to the PCM interface.

    Not applicable for sleep mode 4 and 9 
*/
#ifndef HCILP_ALLOW_HOST_SLEEP_DURING_SCO
#define HCILP_ALLOW_HOST_SLEEP_DURING_SCO       1
#endif

/* HCILP_COMBINE_SLEEP_MODE_AND_LPM

    In Mode 0, always set byte 7 to 0. In sleep mode 1, device always
    requires permission to sleep between scans / periodic inquiries regardless
    of the setting of this byte. In sleep mode 1, if byte is set, device must
    have "permission" to sleep during the low power modes of sniff, hold, and park.
    If byte is not set, device can sleep without permission during these modes.
    Permission to sleep in Mode 1 is obtained if the BT_WAKE signal is not asserted.

    Not applicable for sleep mode 4 and 9 
*/
#ifndef HCILP_COMBINE_SLEEP_MODE_AND_LPM
#define HCILP_COMBINE_SLEEP_MODE_AND_LPM        1
#endif

/* HCILP_ENABLE_UART_TXD_TRI_STATE

    When set to 0, the device will not tristate its UART TX line before going to sleep.
    When set to 1, the device will tristate its UART TX line before going to sleep.

    Not applicable for sleep mode 4 and 9 
*/
#ifndef HCILP_ENABLE_UART_TXD_TRI_STATE
#define HCILP_ENABLE_UART_TXD_TRI_STATE        0
#endif

#ifndef H4IBSS_INCLUDED
#define H4IBSS_INCLUDED                 TRUE
#endif

/* display H4IBSS state and event in text */
#ifndef H4IBSS_DEBUG
#define H4IBSS_DEBUG                    TRUE
#endif

/* time interval before going into sleep after having sent or received SLEEP_REQ_ACK */
/* Valid range is 20 - 50 ms */
#ifndef H4IBSS_SLEEP_GUARD_TIME
#define H4IBSS_SLEEP_GUARD_TIME         (40)
#endif

/* time interval before lowering its TX line after it raised it high */
/* Valid range is 20 - 50 ms */
#ifndef H4IBSS_WAKE_GUARD_TIME
#define H4IBSS_WAKE_GUARD_TIME          (40)
#endif

/* timeout(msec) to wait for response of sleep request */
#ifndef H4IBSS_SLEEP_REQ_RESP_TIME
#define H4IBSS_SLEEP_REQ_RESP_TIME      (50)
#endif

/******************************************************************************
**
** RPC
**
******************************************************************************/

#ifndef RPC_INCLUDED
#define RPC_INCLUDED                FALSE
#endif

/* RPCT task mailbox ID for messages coming from rpcgen code. */
#ifndef RPCT_MBOX
#define RPCT_MBOX                   TASK_MBOX_0
#endif

/* RPCT task event for mailbox. */
#ifndef RPCT_RPC_MBOX_EVT
#define RPCT_RPC_MBOX_EVT           TASK_MBOX_0_EVT_MASK
#endif

/* RPCT task event from driver indicating RX data is ready. */
#ifndef RPCT_RX_READY_EVT
#define RPCT_RX_READY_EVT           APPL_EVT_0
#endif

/* RPCT task event from driver indicating data TX is done. */
#ifndef RPCT_TX_DONE_EVT
#define RPCT_TX_DONE_EVT            APPL_EVT_1
#endif

/* RPCT task event indicating data is in the circular buffer. */
#ifndef RPCT_UCBUF_EVT
#define RPCT_UCBUF_EVT              APPL_EVT_2
#endif

/* Task ID of RPCGEN task. */
#ifndef RPCGEN_TASK
#define RPCGEN_TASK                 BTU_TASK
#endif

/* RPCGEN task event for messages coming from RPCT. */
#ifndef RPCGEN_MSG_EVT
#define RPCGEN_MSG_EVT              TASK_MBOX_1_EVT_MASK
#endif

#ifndef RPCGEN_MSG_MBOX
#define RPCGEN_MSG_MBOX             TASK_MBOX_1
#endif

/* Size of circular buffer used to store diagnostic messages. */
#ifndef RPCT_UCBUF_SIZE
#define RPCT_UCBUF_SIZE             2000
#endif

/******************************************************************************
**
** SAP - Sample ICP and HSP applications
**
******************************************************************************/

#ifndef SAP_INCLUDED
#define SAP_INCLUDED                FALSE
#endif

#ifndef ICA_INCLUDED
#define ICA_INCLUDED                FALSE
#endif

#ifndef HSA_HS_INCLUDED
#define HSA_HS_INCLUDED             FALSE
#endif

#ifndef HSA_AG_INCLUDED
#define HSA_AG_INCLUDED             FALSE
#endif

#ifndef MMI_INCLUDED
#define MMI_INCLUDED                FALSE
#endif

/* MMI task event from driver indicating RX data is ready. */
#ifndef MMI_RX_READY_EVT
#define MMI_RX_READY_EVT           APPL_EVT_0
#endif

/******************************************************************************
**
** APPL - Application Task
**
******************************************************************************/
/* When TRUE indicates that an application task is to be run */
#ifndef APPL_INCLUDED
#define APPL_INCLUDED                FALSE
#endif

/* When TRUE remote terminal code included (RPC MUST be included) */
#ifndef RSI_INCLUDED
#define RSI_INCLUDED                FALSE
#endif

/******************************************************************************
**
** Tracing:  Include trace header file here.
**
******************************************************************************/

#include "bt_trace.h"

#ifndef L2CAP_COMPRESSION_INCLUDED
#define L2CAP_COMPRESSION_INCLUDED FALSE
#endif

/* Define the supported features. This is a bit map.
*/
#define L2CAP_COMPRESSION         0x0001

/* Define values for Widcomm non-standard features. These may be used
** when the other side is also a Widcomm stack. To enable any feature(s),
** there should be a definition for L2CAP_ENHANCED_FEATURES in target.h.
*/
#if (L2CAP_COMPRESSION_INCLUDED == TRUE)
#define L2CAP_ENHANCED_FEATURES   L2CAP_COMPRESSION
#else
#define L2CAP_ENHANCED_FEATURES   0
#endif

#define L2CAP_FEATURE_REQ_ID      73
#define L2CAP_FEATURE_RSP_ID     173

/* Define the Protocol Element that describes compression, and the default
** values. These may be overwritten in target.h
*/
#define L2CAP_PE_TYPE_COMPRESSION		1

#define L2CAP_CPMR_DEFAULT_MEM_LEVEL	2
#define L2CAP_CPMR_MAX_MEM_LEVEL		8
#define L2CAP_CPMR_DEFAULT_WBITS      10
#define L2CAP_CPMR_MAX_WBITS			15

/* Using fix memory pool for compression setup */
#ifndef COMPRESS_FIX_MEMORY
#define COMPRESS_FIX_MEMORY TRUE
#endif

/* DUN script testing using compression */
#ifndef DUN_SCRIPT_TEST_COMPRESSION
#define DUN_SCRIPT_TEST_COMPRESSION FALSE
#endif

/* Use gki_delay for patch ram */
#ifndef BCM2045_USE_DELAY
#define BCM2045_USE_DELAY TRUE
#endif

/******************************************************************************
**
** BTA
**
******************************************************************************/
/* BTA EIR canned UUID list (default is dynamic) */
#ifndef BTA_EIR_CANNED_UUID_LIST
#define BTA_EIR_CANNED_UUID_LIST FALSE
#endif

#endif /* BT_TARGET_H */


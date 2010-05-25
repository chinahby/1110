/*****************************************************************************
**                                                                           
**  Name:          hcisu.h
**                                                                           
**  Description:
**      HCI Services for the upper layer stack.
**
**      This file contains definitions used by the lower-layer HCIS,
**                                                                           
**  Copyright (c) 2002-2004, WIDCOMM Inc., All Rights Reserved.             
**  WIDCOMM Bluetooth Core. Proprietary and confidential.                    
******************************************************************************/
#ifndef HCISU_H
#define HCISU_H

#include "bt_target.h"
#include "bt_types.h"
#include "hcilp.h"
#include "hciutil.h"

/*****************************************************************************
** Definitions for HCISU interface
******************************************************************************/

/* HCISU reconfiguration defintions */

/* tHCISU_CFG_ID definitions */
/* ID 0x0000 to 0x00FF are for transport independent config items */
/* ID 0x0100 to 0x01FF are for H4 config items (see hcis_h4.h for H4 config definitions) */

#define HCISU_CFG_ID_LPM    0x00000000  /* Reconfigure low power mode */
typedef UINT32 tHCISU_CFG_ID;


/*****************************************************************************
** Prototypes for HCI Service interface functions
******************************************************************************/

/* Initialize transport's control block and hardware */
typedef void (*tHCISU_INIT)(UINT8 btu_task_id,
                            UINT8 transport_task_id,
                            UINT16 transport_task_evt);

/* Open port for HCI services */
typedef BOOLEAN (*tHCISU_OPEN)(void *p_cfg);

/* Close port for HCI services */
typedef void (*tHCISU_CLOSE)(void);

/* Send HCI command/data to the transport */
typedef BOOLEAN (*tHCISU_SEND)(BT_HDR *p_msg);

/* Handler for HCIS events */
typedef UINT32 (*tHCISU_EVENT_HDLR)(UINT16 event);

/* Transport reconfiguration callback function (optional) */
typedef void (*tHCISU_RECONFIG_CBACK)(UINT8 status);

/* Transport reconfiguration function (optional) */
typedef void (*tHCISU_RECONFIG)(tHCISU_CFG_ID reconfig_id, void *p_recfg, tHCISU_RECONFIG_CBACK p_cback);


/*****************************************************************************
** Structure of interface functions 
******************************************************************************/
typedef struct {
    tHCISU_INIT init;
    tHCISU_OPEN open;
    tHCISU_CLOSE close;
    tHCISU_SEND send;
    tHCISU_EVENT_HDLR handle_event;
    tHCISU_RECONFIG reconfig;
} tHCISU_IF;

typedef struct
{
    BT_HDR          hdr;
    UINT8           baudrate;
    UINT32          clockrate;
    tHCIUTIL_UPDATEBAUDRATE_CBACK   *p_cback;
} tHCISU_UTIL_IF_MSG;

/* The longest parameters for reconfiguration must be the size of data */
#define MAX_RECONFIG_DATA_LEN (sizeof(tHCILP_Params))
typedef struct
{
    BT_HDR          hdr;
    UINT32          reconfig_flags;
    UINT8           data[MAX_RECONFIG_DATA_LEN];
    tHCISU_RECONFIG_CBACK   p_cback;
} tHCISU_RECONFIG_IF_MSG;

#if (defined(HCILP_INCLUDED) && HCILP_INCLUDED == TRUE)
typedef struct
{
    BT_HDR          hdr;
    BOOLEAN         has_params;
    tHCILP_Params   params;
    tHCILP_ENABLE_CBACK *p_cback;
#if (defined(H4IBSS_INCLUDED) && H4IBSS_INCLUDED == TRUE)
    UINT8           h4ibss_evt;
#endif
} tHCISU_LP_IF_MSG;
#endif

/* Pointer to current HCI Service functions */
extern tHCISU_IF *p_hcisu_if;   /* Pointer to current HCIS interface functions */
extern void *p_hcisu_cfg;       /* Pointer to configuration parameter of current HCIS */
BT_API extern void bte_hcisu_task(UINT32 param);
BT_API extern void bte_hcisu_reconfig(UINT32 reconfig_flags, void *p_recfg, tHCISU_RECONFIG_CBACK p_recfg_cback);

#endif /* ifndef HCISU_H */

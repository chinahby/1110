#ifndef __BT_DIAG_H
#define __BT_DIAG_H

/*==========================================================================

                   Diagnostic Packet Definitions for BT

  Description: Definitions of BT packets. These define the diagnostic
  interface between BT and the external device.

Copyright (c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/05   GS      Initial version

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "diagcmd.h"
#include "diagpkt.h"
#include "bt.h"
#include "bti.h"
#include "btmsg.h"

/* -------------------------------------------------------------------------
 * Extern functions
 * ------------------------------------------------------------------------- */

/*
extern uint16 bt_diag_get_delayed_rsp_id( void );
extern uint16 bt_diag_get_subsys_cmd_code( void );
extern uint16 bt_diag_get_rsp_count( void );
extern void   bt_diag_set_rsp_count( uint16 rsp_count_in );
*/

void   bt_diag_init( void );

/* -------------------------------------------------------------------------
 * Definitions of BT diagnostic packets.
 * ------------------------------------------------------------------------- */

/*
 * BT Request messages
 */
#define BT_DIAG_CMD_START         0x0000
#define BT_DIAG_CMD_END           0xFFFF

/* 
 * BT Diag request and response packets
 */
typedef PACKED struct {
  diagpkt_subsys_header_type    hdr;
  byte                          bt_diag_cmd_msg[1];  
} bt_diag_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_v2_type hdr;
  byte                          bt_diag_evt_msg[1];
} bt_diag_rsp_type;


#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* __BT_DIAG_H */




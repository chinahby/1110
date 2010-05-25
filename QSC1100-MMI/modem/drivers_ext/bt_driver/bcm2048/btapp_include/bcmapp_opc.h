/*****************************************************************************
**                                                                            
**  Name:             btapp_opc.h                                              
**                                                                            
**  Description:     This file contains btapp interface               
**				     definition                                         
**                                                                            
**  Copyright (c) 2000-2005, Widcomm Inc., All Rights Reserved.               
**  Widcomm Bluetooth Core. Proprietary and confidential.                     
******************************************************************************/

#ifndef BTAPP_OPC_H
#define BTAPP_OPC_H




/* BTUI opc control block */
typedef struct
{
     UINT32         bytes_transferred;
#if (defined BTA_OP_INCLUDED) && (BTA_OP_INCLUDED == TRUE)
     tBTA_OP_OPER operation;
#endif

     BOOLEAN        is_connected;
     BOOLEAN        single_op;
     UINT8          num_files;
     BOOLEAN        is_enabled;
     tBTUI_REM_DEVICE * p_connected_rem_device;
     void  * opp_send_file_p_loc;
} tBTUI_OPC_CB;

//extern tBTUI_OPC_CB btui_opc_cb;

tBTUI_OPC_CB * bcmapp_ag_get_opc_cb(void);

BOOLEAN bcmapp_opc_pull_card(void);
BOOLEAN bcmapp_opc_exch_card(void);
BOOLEAN bcmapp_opc_act_send_file ( char *name);
void bcmapp_opc_init(void);
void bcmapp_opc_close(void);
void bcmapp_opc_disable(void);
#endif

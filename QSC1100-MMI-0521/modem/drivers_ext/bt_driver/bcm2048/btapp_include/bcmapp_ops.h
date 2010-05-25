/*****************************************************************************
**                                                                            
**  Name:             bcmapp_ops.h                                              
**                                                                            
**  Description:     This file contains bcmui internal interface               
**				     definition                                         
**                                                                            
**  Copyright (c) 2000-2004, Widcomm Inc., All Rights Reserved.               
**  Widcomm Bluetooth Core. Proprietary and confidential.                     
******************************************************************************/

#ifndef BCMAPP_OPS_H
#define BCMAPP_OPS_H


#include "bta_op_api.h"

typedef struct
{
     UINT32  bytes_transferred;
     UINT32  obj_size;
     tBTA_OP_ACCESS  access_flag;
} tBTUI_OPS_CB;

//extern tBTUI_OPS_CB bcmui_ops_cb;

void bcmapp_ops_init(void);
void bcmapp_ops_disable(void);
tBTUI_OPS_CB * bcmapp_get_ops_cb(void);
#endif

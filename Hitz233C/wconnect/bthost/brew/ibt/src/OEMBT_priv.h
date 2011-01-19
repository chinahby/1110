#ifndef OEMBT_PRIV_H
#define OEMBT_PRIV_H
/*=============================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMBT_priv.h

SERVICES:  AEE OEM BT Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
BT.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBT.h"
#include "bt.h"

#define BT_ECS_FREE         0
#define BT_ECS_PARSING      1
#define BT_ECS_READY        2
#define BT_ECS_DISPATCHING  3

typedef struct 
{
   bt_ev_msg_type bt_event;
   uint32         uTimeStamp;
   uint8          uState;
}  OEMBTEvent;

OEMBTEvent* OEMBT_GetFreeEv( void );
OEMBTEvent* OEMBT_GetReadyEv( bt_app_id_type appId );
void OEMBT_FreeEv( OEMBTEvent* pEvt );
void OEMBT_DecRefCnt(void);
void OEMBT_IncRefCnt(void);

#endif /*OEMBT_PRIV_H*/


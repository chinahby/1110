#ifndef OEMBT_H
#define OEMBT_H
/*=============================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMBT.h

SERVICES:  AEE OEM BT Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
BT.

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBT.h#2 $ 
$DateTime: 2009/04/03 17:37:13 $
=============================================================================*/
#include "AEEBT.h"

typedef struct _OEMBT OEMBT;

#ifdef __cplusplus
extern "C" {
#endif

   AEE_EXPORTS int OEMBT_New( OEMBT** ppme );
   AEE_EXPORTS uint32 OEMBT_Release( OEMBT* pme );
   AEE_EXPORTS int OEMBT_GetNotificationData( BTNotifyInfo* pNotif, 
                                              BTEventData* pED );
   AEE_EXPORTS int OEMBT_GetLocalInfo( BTLocalInfo* pLocalInfo );
   AEE_EXPORTS int OEMBT_GetRadioState( BTRadioState* pdRadioState );
   AEE_EXPORTS int OEMBT_SetBondable( boolean bondable );
   AEE_EXPORTS int OEMBT_GetBondable( boolean* bondable );
   AEE_EXPORTS int OEMBTNotifier_GetBTEv( BTNotifyInfo* pNotif );
   AEE_EXPORTS void OEMBT_OnBTEv( OEMBT* pme, AEECallback *pcb );
   AEE_EXPORTS void OEMBT_NotifReg( OEMBT* pme );
   AEE_EXPORTS void OEMBT_NotifUnReg( OEMBT* pme );

#ifdef _cplusplus
}
#endif
#endif /*OEMBT_H*/


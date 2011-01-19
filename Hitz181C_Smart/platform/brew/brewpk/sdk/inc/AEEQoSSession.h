#ifndef AEEQOSSESSION_H
#define AEEQOSSESSION_H
/*======================================================
FILE:  AEEQoSSession.h

SERVICES:  BREW QoS Session Interface

GENERAL DESCRIPTION:
	Interface definitions, data structures, etc. for BREW Quality Of Service
   Session Interface

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEECLSID_QOSSESSION.BID"
#include "AEEQoSSpec.h"
#include "AEEQoSFlow.h"
#include "AEE.h"
#include "AEEModel.h"
#include "../../inc/AEEIQoSSession.h"

#define IQOSSESSION_AddRef                            IQoSSession_AddRef
#define IQOSSESSION_Release                           IQoSSession_Release
#define IQOSSESSION_QueryInterface                    IQoSSession_QueryInterface
#define IQOSSESSION_AddListener                       IQoSSession_AddListener
#define IQOSSESSION_Notify                            IQoSSession_Notify
#define IQOSSESSION_SelectNetwork                     IQoSSession_SelectNetwork
#define IQOSSESSION_Open                              IQoSSession_Open
#define IQOSSESSION_Modify                            IQoSSession_Modify
#define IQOSSESSION_Close                             IQoSSession_Close
#define IQOSSESSION_AddRetryCB                        IQoSSession_AddRetryCB
#define IQOSSESSION_GetFlowSpec                       IQoSSession_GetFlowSpec
#define IQOSSESSION_GetStatus                         IQoSSession_GetStatus
#define IQOSSESSION_GoActive                          IQoSSession_GoActive
#define IQOSSESSION_SetDormancyTimeout                IQoSSession_SetDormancyTimeout
#define IQOSSESSION_GetError                          IQoSSession_GetError
#define IQOSSESSION_SelectNetworkEx                   IQoSSession_SelectNetworkEx
#define IQOSSESSION_Deactivate                        IQoSSession_Deactivate
#define IQOSSESSION_ModifyEx                          IQoSSession_ModifyEx
#define IQOSSESSION_GetGrantedFlowSpecDuringAnyState  IQoSSession_GetGrantedFlowSpecDuringAnyState
#endif      // AEEQOSSESSION_H

#ifndef OEMSTK_H
#define OEMSTK_H

/*======================================================
FILE:  OEMSTK.h

SERVICES:  OEMSTK member functions

GENERAL DESCRIPTION:

   ISTK, ISTKNotifier and related functionality

        Copyright © 1999-2006, 2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/istk/rel/07H1_2/inc/OEMSTK.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/09   xz      Extend OEMSTK_GetNextCmd() to return command number
11/01/06   sk      Added newline char at EOF to remove warning.
06/21/06   sk      Corrected type error
06/12/06   sk      Added OEMSTK_SendCommandResponseEx() to support new rsp
                   structures and 4 byte extra param length
=============================================================================*/


#include "AEESTK.h"

typedef struct OEMSTK OEMSTK;

int OEMSTK_SendCommandResponse(
              OEMSTK                     *pMe,
              AEESTKProactiveCmdType     term_resp_e,
              uint8                      cmd_num,
              AEESTKResult               result,
              AEESTKAdditionalInfo       *additional_result,
              void                       *extra_param,
	      uint8			 extra_param_length);

/*   Enhanced SendCommandResponse to support new response structures and 4 bytes 
     extra param length 
*/

int OEMSTK_SendCommandResponseEx(
              OEMSTK                     *pMe,
              AEESTKProactiveCmdType     term_resp_e,
              uint8                      cmd_num,
              AEESTKResult               result,
              AEESTKAdditionalInfo       *additional_result,
              void                       *extra_param);

int OEMSTK_SendEnvelopeEvtDownloadCommand(
                            OEMSTK           *po,
                            uint16            *pwTransactionID,
                            uint32            dwSize,
                            AEESTKEventDLInd  *evt_list
			 );

int OEMSTK_SendEnvelopeMenuSelCommand(OEMSTK        *po,
                                      uint16        *pwTransactionID,
                                      byte          ic,
                                      boolean       hr);

void OEMSTK_OnCardCmd(OEMSTK *pMe, AEECallback *ppcb);
void OEMSTK_OnEnvelopeCmd(OEMSTK *pMe, AEECallback *pecb);
void OEMSTK_GetNextCmd(OEMSTK *pMe, uint32 *pCommandType, uint8 *pCmdNum, uint32 *pdwTransactionID);
int OEMSTK_GetBufSize(OEMSTK *pMe, uint32 dwID, uint32 *pBufSize);
int OEMSTK_GetCachedEntry(OEMSTK *pMe, uint32 dwID, void *pBuf, uint32 nBufSize);
int OEMSTK_New(IShell *pIShell, OEMSTK **ppif);
int OEMSTK_UserActivity(OEMSTK *po);
int OEMSTK_IdleScreen(OEMSTK *po);
int OEMSTK_LangSelection(OEMSTK *po, 
                         AEESTKLangSelectionResponse *sLangSelection);
int OEMSTK_BrowserTermination(OEMSTK *po,AEESTKBrowserTermEnumType sBrowserTerm );
int OEMSTK_ChannelStatusChange(OEMSTK *pISTK, AEESTKGetChStatusResponse sChStatus);
int OEMSTK_DataAvailable(OEMSTK *pISTK, AEESTKDataAvailNotify sDataAvail);

uint32 OEMSTK_Release(OEMSTK *pMe);
#endif //OEMSTK_H


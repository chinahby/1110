/*=============================================================================

FILE: DialerDiag.c

SERVICES: Dialer Applet

GENERAL DESCRIPTION: Dialer Diagnostic functions

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2004 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerDiag.c#4 $
$DateTime: 2005/02/03 15:55:24 $
$Author: carls $
$Change: 162355 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what: where, why
--------   ---     ---------------------------------------------------------
02/03/05   cvs     Fix RVCT compiler error
03/03/04   JAS     Created Dialer Diag file; added functions to handle
                   "Get Call Status" and "Get Call Detail" diag requests.

=============================================================================*/
//lint -save -e611 -e740  Suppress lint complaints about suspicious ptr casts

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"
#include "AEEAppGen.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEText.h"
#include "AEE.h"
#include "AppComFunc.h"
#include "AEECM.h"
#include "DialerUtils.h"
#include "DialerApp.h"
#include "dialerapp_res.h"
#include "uiutils.h"
#include "AEEAlert.h"
#include "DialerApp.bid"

#ifdef FEATURE_APP_TEST_AUTOMATION
#include "AEEDiag.h"
#include "DialerDiag.h"


/*===========================================================================

                      LOCAL/STATIC DATA

===========================================================================*/

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_GetStatusData

DESCRIPTION: Handles the "Get Call Status" diagnostic.  It creates and
             populates the diagnostic response and sends it.

PARAMETERS:
  CDialerApp *pMe: Pointer to the Dialer App structure
  AEEEvent eCode:  The event code for the diagnostic request
  uint16 wp:       The "word parameter" received with the event
  uint32 dwp:      The "double word parameter" received with the event

RETURN VALUE:
  None

COMMENTS:
  It is important to always send a diagnostic response whenever
  possible, even in failure scenarios.

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void CDialerApp_GetStatusData(CDialerApp *pMe, AEEEvent eCode, uint16 wp,
                              uint32 dwp)
{

    IDialog *pActiveDlg;
    uint8 i;                 // index for looping through calls
    CCallStatus *pStoredCallInfo;
    CCallStatusDiagPayload *pRspPayload;
    CCallStatusDiagData *pCallRec;
    uint32 ActualPayloadSize;
    uint16 XactId;
    uint32 Status;

    if (pMe == NULL)
    {
        return;
    }
   
    // Get the transaction ID for this request
    XactId = IDIAG_GetTransactionId(pMe->m_pDiag, AEECLSID_DIALER, eCode,
                                    wp, dwp);
    if (XactId == 0)
    {
        DIALER_ERR("Failed to get xact id in call status diag.", 0, 0, 0);
        return;
    }
   
    // allocate space for the response
//lint -save -e611 -e740  Suppress lint complaint about suspicious cast
    pRspPayload = (CCallStatusDiagPayload *)
                                    MALLOC(sizeof(CCallStatusDiagPayload));
//lint -restore
    if (pRspPayload == NULL)
    {
        return;
    }

    // Using do..while (0) to avoid deeply nested if's
    do
    {
        // Default to success
        Status = IDIAG_SUCCESS;

        pActiveDlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if (pActiveDlg == NULL)
        {
            Status = IDIAG_NO_ACTIVE_DLG;
            break;
        }

        if (IDIALOG_GetID(pActiveDlg) != IDD_STATUS)
        {
            Status = IDIAG_WRONG_DLG;
            break;
        }

        if (pMe->m_pDlgData == NULL)
        {
            // If the data is not there, fail
            Status = IDIAG_DATA_UNAVAILABLE;
            break;
        }
    
        pRspPayload->NumOfCalls =
                     ((CAllCallStatus *)(pMe->m_pDlgData))->yNumDisplayedCalls;
    
        if (pRspPayload->NumOfCalls > (DIALERAPP_MAX_NUM_CALLS * 2))
        {
            // If the number is somehow too big, fail.
            Status = (uint32)IDIAG_OTHER_INT_ERROR;
            break;
        }

        // Populate the call status data in the outgoing message
        for (i = 0; i < pRspPayload->NumOfCalls; i++)
        {
            // Get a pointer to the stored data
            pStoredCallInfo = &(((CAllCallStatus *)
                                 (pMe->m_pDlgData))->CallStatus[i]);
            pCallRec = &(pRspPayload->CallStatus[i]);
    
            // Copy stored data into outgoing message
            pCallRec->CallState = (uint32)(pStoredCallInfo->CallState);
            pCallRec->CallType = (uint32)(pStoredCallInfo->CallType);
            pCallRec->CallId = (uint8)(pStoredCallInfo->CallID);
            pCallRec->Privacy = (char)(pStoredCallInfo->Privacy);
            STRLCPY((char*)pCallRec->MinSecTime, pStoredCallInfo->MinSecTime,
                    DIALER_DIAG_COMMON_STR_SZ);
            STRLCPY((char*)pCallRec->OtherParty, pStoredCallInfo->OtherParty,
                    DIALER_DIAG_OTHER_PARTY_STR_SZ);
        }
//lint -save -e717 Suppress "do..while(0)"
    } while (0);
//lint -restore

    /* Actual size is calculated as follows:
     * Get size of full structure
     * Subtract (max) size of call status data
     * Add actual call status data size (record size * number of records)
     */
    ActualPayloadSize = sizeof(*pRspPayload) -
                        sizeof(pRspPayload->CallStatus) +
                        (pRspPayload->NumOfCalls *
                         sizeof(CCallStatusDiagData));

//lint -save -e611 -e740  Suppress lint complaint about suspicious cast
    IDIAG_SendDiagResponse(pMe->m_pDiag, Status, XactId, TRUE,
                           ActualPayloadSize, (void *)pRspPayload);
    FREE((void *)pRspPayload);
//lint -restore

}

/*=============================================================================
FUNCTION: CDialerApp_GetDetailData

DESCRIPTION: Handles the "Get Call Detail" diagnostic.  It creates and
             populates the diagnostic response and sends it.

PARAMETERS:
  CDialerApp *pMe: Pointer to the Dialer App structure
  AEEEvent eCode:  The event code for the diagnostic request
  uint16 wp:       The "word parameter" received with the event
  uint32 dwp:      The "double word parameter" received with the event

RETURN VALUE:
  None

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void CDialerApp_GetDetailData(CDialerApp *pMe, AEEEvent eCode, uint16 wp,
                              uint32 dwp)
{

    IDialog *pActiveDlg;
    CCallDetail *pStoredData;
    CCallDetailDiagPayload *pRspPayload;
    uint16 XactId;
    uint32 Status;

    if (pMe == NULL)
    {
        return;
    }

    XactId = IDIAG_GetTransactionId(pMe->m_pDiag, AEECLSID_DIALER, eCode,
                                    wp, dwp);
    if (XactId == 0)
    {
        DIALER_ERR("Failed to get xact id in call detail diag.", 0, 0, 0);
        return;
    }

//lint -e611 -e740
    pRspPayload =
              (CCallDetailDiagPayload *)MALLOC(sizeof(CCallDetailDiagPayload));
//lint -restore
    if (pRspPayload == NULL)
    {
        return;
    }

    // Default to success
    Status = IDIAG_SUCCESS;

    // Using do..while (0) to avoid deeply nested if..else's
    do
    {

        // Get the current active dialog
        pActiveDlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if (pActiveDlg == NULL)
        {
            Status = IDIAG_NO_ACTIVE_DLG;
            break;
        }

        if (IDIALOG_GetID(pActiveDlg) != IDD_DETAIL)
        {
            Status = IDIAG_WRONG_DLG;
            break;
        }

        pStoredData = (CCallDetail *)(pMe->m_pDlgData);
    
        if (pStoredData == NULL)
        {
            Status = IDIAG_DATA_UNAVAILABLE;
            break;
        }
    
        pRspPayload->CallId = pStoredData->CallID;
//lint -e611 -e740
        STRLCPY(pRspPayload->Privacy, pStoredData->Privacy,
                DIALER_DIAG_COMMON_STR_SZ);
        STRLCPY(pRspPayload->MinSecTime, pStoredData->MinSecTime,
                DIALER_DIAG_COMMON_STR_SZ);
        STRLCPY(pRspPayload->OtherParty1, pStoredData->OtherParty1,
                DIALER_DIAG_OTHER_PARTY_STR_SZ);
        STRLCPY(pRspPayload->OtherParty2, pStoredData->OtherParty2,
                DIALER_DIAG_OTHER_PARTY_STR_SZ);
        STRLCPY(pRspPayload->CUGString, pStoredData->CUGString,
                DIALER_DIAG_COMMON_STR_SZ);
        STRLCPY(pRspPayload->CallStatus, pStoredData->CallStatus,
                DIALER_DIAG_COMMON_STR_SZ);
//lint -restore
        
//lint -save -e717 Suppress "do..while(0)"
    } while (0);
//lint -restore

//lint -e611 -e740
    IDIAG_SendDiagResponse(pMe->m_pDiag, Status, XactId, TRUE,
                           sizeof(*pRspPayload), (void *)pRspPayload);
    FREE(pRspPayload);
//lint -restore

}

//lint -restore  Restore lints about suspicious ptr casts

#endif //FEATURE_APP_TEST_AUTOMATION

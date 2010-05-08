#ifndef DIALERDIAG_H
#define DIALERDIAG_H
/*=============================================================================

FILE: DialerDiag.h

SERVICES: Dialer Applet Diagnostics

GENERAL DESCRIPTION:
  This file contains definitions used by the Dialer App to
  implement handling of certain diagnostic requests.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerDiag.h#2 $
$DateTime: 2004/04/07 15:38:33 $
$Author: johns $
$Change: 100913 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/03/04   JAS     Created Dialer diagnostic header file

=============================================================================*/
/*=============================================================================

                       INCLUDES

=============================================================================*/
#include "AEE.h"
#include "AEEAppGen.h"
#include "AEECM.h"
#include "DialerApp.bid"
#include "AEECM.h"
#include "AEEVector.h"
#include "AEEAddrBook.h"
#include "AppComFunc.h"
#include "AEEAlert.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#include "AEEDiag.h"

/*=============================================================================

                       CONSTANTS

=============================================================================*/

#define DIALER_DIAG_COMMON_STR_SZ 20
#define DIALER_DIAG_OTHER_PARTY_STR_SZ 65

/*===========================================================================

                       TYPE DECLARATIONs

===========================================================================*/

/************************************************************************
 *
 * Diagnostic Response Payload Definitions
 * This section defines the "payload" portion of the diagnostic
 * response messages send by Dialer
 *
 * For further details on the contents of these fields, refer to the
 * Application Subsystem Interface Control Document (80-V1294-20).
 *
 ***********************************************************************/

/* CDiagCallStatusData defines the structure for the call status
 * information associated with a single call.
 */
typedef PACKED struct
{
    uint32  CallState;
    uint32  CallType;
    uint8   CallId;
    char    Privacy;
    char    MinSecTime[DIALER_DIAG_COMMON_STR_SZ];  // Call duration
    char    OtherParty[DIALER_DIAG_OTHER_PARTY_STR_SZ];
} CCallStatusDiagData;

/* CCallStatusDiagPayload defines the structure of the payload
 * portion of the diagnostic response for the "Get Call Status"
 * diagnostic request.  It aggregates several call status
 * structures with other fields.
 *
 * NOTE: CallStatus contains all active calls followed by all
 * ended calls.  All of the call status entries are contiguous
 * with no empty entries between them.
 */
typedef PACKED struct
{
    uint8                   NumOfCalls;
    CCallStatusDiagData     CallStatus[DIALERAPP_MAX_NUM_CALLS * 2];
} CCallStatusDiagPayload;

/* CCallDetailDiagPayload defines the structure of the payload
 * portion of the diagnostic response for the "Get Call Detail"
 * diagnostic request.
 */
typedef struct
{
  uint8               CallId;
  char                Privacy[DIALER_DIAG_COMMON_STR_SZ];
  char                MinSecTime[DIALER_DIAG_COMMON_STR_SZ];  // Call duration
  char                OtherParty1[DIALER_DIAG_OTHER_PARTY_STR_SZ];
  char                OtherParty2[DIALER_DIAG_OTHER_PARTY_STR_SZ];
  char                CUGString[DIALER_DIAG_COMMON_STR_SZ];
  char                CallStatus[DIALER_DIAG_COMMON_STR_SZ];
} CCallDetailDiagPayload;

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_GetStatusData

DESCRIPTION:  Constructs the payload portion of the response to the
              Get Call Status diagnostic, and sends the response.

PARAMETERS:
  CDialerApp *pMe: Pointer to the Dialer App structure
  AEEEvent eCode:  Event code sent for the diagnostic request
  uint16 wp:       The "word parameter" received with the event
  uint32 dwp:      The "double word parameter" received with the event

RETURN VALUE:
  None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_GetStatusData(CDialerApp *pMe, AEEEvent eCode, uint16 wp,
                              uint32 dwp);

/*=============================================================================
FUNCTION: CDialerApp_GetDetailData

DESCRIPTION:  Constructs the payload portion of the response to the
              Get Call Detail diagnostic, and sends the response.

PARAMETERS:
  CDialerApp *pMe: Pointer to the Dialer App structure
  AEEEvent eCode:  Event code sent for the diagnostic request
  uint16 wp:       The "word parameter" received with the event
  uint32 dwp:      The "double word parameter" received with the event

RETURN VALUE:
  None

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void CDialerApp_GetDetailData(CDialerApp *pMe, AEEEvent eCode, uint16 wp,
                              uint32 dwp);


#endif // FEATURE_APP_TEST_AUTOMATION
#endif // DIALERDIAG_H

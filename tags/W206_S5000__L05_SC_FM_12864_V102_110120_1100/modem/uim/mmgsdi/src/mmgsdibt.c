/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I M   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC protocol processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2005 - 2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                         EDIT HISTORY FOR MODULE

 $Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdibt.c#1 $$ $DateTime: 2008/12/15 12:00:08 $

 when      who      what, where, why
--------   ---    ----------------------------------------------------------
09/22/08   js     Fixed lint errors
09/10/08   sun    Clear the timer before restarting it
07/01/08   tml    For WM BT, allow bt app id to be initialized more than
                  once
06/25/08   kk     Fixed compilation error.
06/23/08   tml    Added featurization for BT in WM
01/11/07   sun    Removed functionality for Unblock and Change CHV
12/10/07   sun    Verify PIN Internally when SAP is Disconnected
11/15/07   sun    Added support for NAA refresh
07/20/07   sun    Added support for USIM APDU Response as well
06/01/07   sun    Register with BT only once, Copy the data for TX APDU
05/22/07   sun    Do not free the data until the get rsp data has been read
05/07/07   sp     Replaced calls to mem_malloc/free with mmgsdi_malloc/free
04/17/07   sun    Set up a timer if external module other than UIM does not
                  respond
04/04/07   sun    Fixed Compile Error
04/03/07   sun    Added Null pointer checks
03/19/07   sun    Fixed Lint Error
11/15/06   pv     Remove voting UIM to do the polling since it is not managed
                  within UIM based on the mode and protocol availability.
11/15/06   sun    Inform BT about a connection failure else the underlining
                  connection lingers
11/02/06   sun    Monitor when the card is powered on and reject commands if
                  an already powered on card is powered on again.
10/17/06   sun    When a call is in progress, do not allow SAP Connections
07/26/06   sun    Lint Fixes and Cleanup
06/26/06   sun    Allow the responses to go to BT even if UIM has returned
                  an error.
                  Print all messages from mmgsdi_dump_data. Fixed Aligment
06/19/06   sun    Fixed Slot for Refresh
05/03/06   sp     Update mmgsdi pin status
04/06/06   sun    Send out only the required number of bytes for
                  GET_RESPONSE
03/21/06   tml    lint
03/14/06   sun    Queue Refresh with MMGSDI_REFRESH_AUTO option so that
                  the refresh gets queued again if the conditions are
                  not satisified the first time around.
02/22/06   tml    lint fix
01/30/06   pv     Notify card in-accessible instead of card removed
01/29/06   tml    include mmgsdi.h in place of mmgsdilib.h due to the
                  mmgsdi_sim_refersh move
01/04/06   sp     Changes to sap_pin_verify for diag support
12/20/05   sun    Formatting cleanup, return error in invalid state
12/14/05   tml    MMGSDI Cleanup
12/07/05   sun    Fixed Polling mask
12/05/05   sun    Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/09/05   sun    Fixed Lint Errors
11/03/05   sun    Refresh when the disconnect is received
10/28/05   sun    Fixed Sap Disconnect
10/20/05   sun    Added support for BT Deregisteration
08/26/05   sun    Added support for BT SAP
07/19/05   sst    Added register pin function
07/18/05   sst    Updated to use MMGSDI interfaces
03/22/05   sst    Initial version
===========================================================================*/

#include "comdef.h"
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

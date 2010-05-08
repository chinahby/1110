/*=============================================================================
FILE:  OEMWMSDIAG.c

SERVICES:  OEM functions for supporting BREW WMSAPPDiag Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW IWMSDIAG interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright 2004,2005,2006,2007,2008 QUALCOMM Incorporated.
            All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc1100/modem/wms/wmsapp/main/latest/src/OEMWMSDIAG.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information
01/17/07   HQ      Updated copyright info for 2007.
05/11/06   PMD     Added Support for New Diag Commands and modified 
                   existing Diag Commands for Delayed Response
12/20/05   PMD     Cleaned Lint Warnings and removed Lint Suppressions
11/21/05   PMD     Fixed High Lint Warnings
07/08/05   Rex     Fixed compile error
06/13/05   Rex     Added OEMWMSDIAG_CbDeleteAllRequest
10/11/04   Rex     Featurization fixes
06/24/04   Rex     Print error msg when cb payloa d size exceeds max value
06/22/04   Rex     Extern delay_rsp_id variable for usage
                   Added checks for invalid values in the response packet
05/10/04   Rex     Initial Version

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions

#include "AEEWMSDIAG_priv.h"
#include "OEMWMSDIAG.h"

#include "diagcmd.h"     /* Header file for Diag command */
#include "diagpkt.h"     /* Header file for Diag packets */
#include "event.h"       /* Header file for events */
#include "wmsdiag.h"     /* Header file for WMSDIAG commadns */


/*=========================================================================
FUNCTION:    OEMWMSDIAG_CbSaveRequest

DESCRIPTION  Sends a request to AEE layer issuing to save a SMS CB message
             from RAM to EFS

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Send to AEE layer for posting notification succeed
  FALSE - Send to AEE layer failed

SIDE EFFECTS
  none
=========================================================================*/
#ifdef FEATURE_IWMSDIAG
boolean OEMWMSDIAG_CbSaveRequest (wmsappdiag_cb_save_req_type *req_pkt_ptr)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif
}
#endif

/*=========================================================================
FUNCTION: OEMWMSDIAG_CbSaveResponse

DESCRIPTION
  This function saves a SMS CB message from RAM to EFS and sends a delayed response
  to the DM with the appropriate field values (tid, status_response, efs_index)

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS - Status information is valid
  EBADPARM - Bad parameter
  EFAILED - Failure

SIDE EFFECTS
  none
===========================================================================*/
boolean OEMWMSDIAG_CbSaveResponse (AEEWMSDIAGCbSaveRsp *rsp_pkt_ptr)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif
}


/*=========================================================================
FUNCTION:    OEMWMSDIAG_CbReadRequest

DESCRIPTION  Sends a request to AEE layer to read CB message content from WMSAPP

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Send to AEE layer for posting notification succeed
  FALSE - Send to AEE layer failed

SIDE EFFECTS
  none
=========================================================================*/
#ifdef FEATURE_IWMSDIAG
boolean OEMWMSDIAG_CbReadRequest (wmsappdiag_cb_read_req_type *req_pkt_ptr)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif
}
#endif

/*===========================================================================
FUNCTION: OEMWMSDIAG_CbReadResponse

DESCRIPTION
  This function passes a SMS CB message read by WMSAPP with a given storage
  type (RAM or EFS) and msg_index and sends the data as a delayed response
  packet to the DM.

  payloadSize - number of bytes for the cb data

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Delayed response packet sent successfully
  FALSE - Bad parameter

SIDE EFFECTS
  none
===========================================================================*/
boolean OEMWMSDIAG_CbReadResponse (uint16 payloadSize, AEEWMSDIAGCbReadRsp *rsp_pkt_ptr)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif
}


/*=========================================================================
FUNCTION:    OEMWMSDIAG_CbDeleteMsgRequest

DESCRIPTION  Sends a request to AEE layer to delete the CB message content
             from WMSAPP Memory Store and given Index

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Send to AEE layer for posting notification succeed
  FALSE - Send to AEE layer failed

SIDE EFFECTS
  none
=========================================================================*/
#ifdef FEATURE_IWMSDIAG
boolean OEMWMSDIAG_CbDeleteMsgRequest (wmsappdiag_cb_delete_msg_req_type *req_pkt_ptr)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif
}
#endif

/*===========================================================================
FUNCTION: OEMWMSDIAG_CbDeleteMsgResponse

DESCRIPTION
  This function passes a SMS CB delet all messages by WMSAPP with a given storage type
  type (RAM, EFS, or Both) and sends the data as a delayed response
  packet to the DM.

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Delayed response packet sent successfully
  FALSE - Bad parameter

SIDE EFFECTS
  none
===========================================================================*/
boolean OEMWMSDIAG_CbDeleteMsgResponse (AEEWMSDIAGCbDeleteMsgRsp *rsp_pkt_ptr)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif
}


/*=========================================================================
FUNCTION:    OEMWMSDIAG_CbDeleteAllMsgsRequest

DESCRIPTION  Sends a request to AEE layer to delete all CB message content
             from WMSAPP Memory Store

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Send to AEE layer for posting notification succeed
  FALSE - Send to AEE layer failed

SIDE EFFECTS
  none
=========================================================================*/
#ifdef FEATURE_IWMSDIAG
boolean OEMWMSDIAG_CbDeleteAllMsgsRequest (wmsappdiag_cb_delete_all_msgs_req_type *req_pkt_ptr)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif
}
#endif

/*===========================================================================
FUNCTION: OEMWMSDIAG_CbDeleteAllMsgsResponse

DESCRIPTION
  This function passes a SMS CB delet all messages by WMSAPP with a given storage type
  type (RAM, EFS, or Both) and sends the data as a delayed response
  packet to the DM.

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Delayed response packet sent successfully
  FALSE - Bad parameter

SIDE EFFECTS
  none
===========================================================================*/
boolean OEMWMSDIAG_CbDeleteAllMsgsResponse (AEEWMSDIAGCbDeleteAllMsgsRsp *rsp_pkt_ptr)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif
}


/*===========================================================================
FUNCTION: OEMWMSDIAG_EventCbSmsNotify

DESCRIPTION
  The function is a notification from WMSAPP indicating that a new SMS CB message
  has been received.  This notification will be sent to the DM as EVENT_CB_SMS_NOTIFY

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Event report payload is sent
  FALSE - Bad parameter

SIDE EFFECTS
  none
===========================================================================*/
boolean OEMWMSDIAG_EventCbSmsNotify (AEEWMSDIAGEventCbSmsNotify *payload)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif
}


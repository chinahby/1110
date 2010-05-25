#ifndef OEMWMSDIAG_H
#define OEMWMSDIAG_H
/*=============================================================================
FILE:  OEMWMSDIAG.h

DESCRIPTION: Definitions to support access to OEM WMS Diag interface

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright 2004,2005,2006,2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/OEMWMSDIAG.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/07   HQ      Updated copyright info for 2007.
05/11/06   PMD     Added Support for New Diag Commands and modified 
                   existing Diag Commands for Delayed Response
08/10/05   Rex     Added function prototype OEMWMSDIAG_CbDeleteAllResponse
06/13/04   Rex     Added function prototype OEMWMSDIAG_CbDeleteAllRequest
05/10/04   Rex     Initial Version

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "AEE.h"         // BREW headers.
#include "AEE_OEM.h"     // Brew OEM Headers.

#include "AEEWMSDIAG.h"

#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions

#include "diagpkt.h"
#include "wmsdiag.h"    //WMSDIAG Header file, for inclusion of request packets


/*===========================================================================

                             TYPE DEFINITIONS

===========================================================================*/


/* Event payload to Diag */
typedef PACKED struct
{
  uint16 service_id;
  uint8  update_num;
  uint16 msg_code;
  uint8  geo_scope;
  uint8  page_num;
  uint8  total_page;
  uint16 msg_index;

} OEMWMSDIAGEventCbSmsNotify;


typedef struct IWMSDIAG IWMSDIAG;


/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/


int AEEWMSDIAG_New(IShell *piShell, AEECLSID cls, void **ppif);

int AEEWMSDIAGNotifier_New(IShell *piShell, AEECLSID cls, void **ppif);


/*=========================================================================
PACKET    OEMWMSDIAG_CbSaveRequest

PURPOSE   Sends a request to AEE layer issuing to save a SMS CB message
          from RAM to EFS
=========================================================================*/
boolean OEMWMSDIAG_CbSaveRequest (wmsappdiag_cb_save_req_type *req_pkt_ptr);


/*=========================================================================
PACKET    OEMWMSDIAG_CbSaveResponse

PURPOSE   Sends a response upon issuing a request to save a SMS CB message
          from RAM to EFS

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values (tid, status_response, efs_index)
=========================================================================*/
boolean OEMWMSDIAG_CbSaveResponse (AEEWMSDIAGCbSaveRsp *rsp_pkt_ptr);


/*=========================================================================
PACKET    OEMWMSDIAG_CbReadRequest

PURPOSE   Sends a request to AEE layer to read CB message content from WMSAPP
=========================================================================*/
boolean OEMWMSDIAG_CbReadRequest (wmsappdiag_cb_read_req_type *req_pkt_ptr);


/*=========================================================================
PACKET    OEMWMSDIAG_CbReadResponse

PURPOSE   Sends CB message content from WMSAPP to OEMWMSDIAG

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values (e.g. tid, time, 88 bytes of data)
=========================================================================*/
boolean OEMWMSDIAG_CbReadResponse (uint16 payloadSize, AEEWMSDIAGCbReadRsp *rsp_pkt_ptr);


/*=========================================================================
PACKET    OEMWMSDIAG_CbDeleteAllMsgsRequest

PURPOSE   Sends CB message content from OEMWMSDIAG to WMSAPP

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values
=========================================================================*/
boolean OEMWMSDIAG_CbDeleteMsgRequest (wmsappdiag_cb_delete_msg_req_type *req_pkt_ptr);


/*=========================================================================
PACKET    OEMWMSDIAG_CbDeleteAllMsgsResponse

PURPOSE   Sends CB message content from OEMWMSDIAG to WMSAPP

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values
=========================================================================*/
boolean OEMWMSDIAG_CbDeleteMsgResponse (AEEWMSDIAGCbDeleteMsgRsp *rsp_pkt_ptr);


/*=========================================================================
PACKET    OEMWMSDIAG_CbDeleteAllMsgsRequest

PURPOSE   Sends CB message content from OEMWMSDIAG to WMSAPP

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values
=========================================================================*/
boolean OEMWMSDIAG_CbDeleteAllMsgsRequest (wmsappdiag_cb_delete_all_msgs_req_type *req_pkt_ptr);


/*=========================================================================
PACKET    OEMWMSDIAG_CbDeleteAllMsgsResponse

PURPOSE   Sends CB message content from OEMWMSDIAG to WMSAPP

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values
=========================================================================*/
boolean OEMWMSDIAG_CbDeleteAllMsgsResponse (AEEWMSDIAGCbDeleteAllMsgsRsp *rsp_pkt_ptr);


/*=========================================================================
PACKET    OEMWMSDIAG_EventCbSmsNotify

PURPOSE   SMS CB notification event sent from WMSAPP to OEMWMSDIAG

RESPONSE  Sends the event to Diag.
          The field parameters of the event are parsed in a separate QXDM window
=========================================================================*/
boolean OEMWMSDIAG_EventCbSmsNotify (AEEWMSDIAGEventCbSmsNotify *payload);



#endif /* OEMWMSAPP_H */

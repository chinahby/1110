#ifndef AEEWMSDIAG_H
#define AEEWMSDIAG_H

/*============================================================================
FILE:  AEEWMSDIAG.h

SERVICES:  AEE IWMSDIAG Interface

GENERAL DESCRIPTION:
   This module supports the AEE interface for supporting WMSDIAG

INTERFACE DOCUMENTATION: 80-V5316-1 [QCT Static Extension]

        Copyright 2004,2005,2006,2007,2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/inc_deprecated/AEEWMSDIAG.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/08   PMD     Fixed Compiler Warnings
01/31/08   PMD     Updated Copyright Information
01/17/07   HQ      Updated copyright info for 2007.
11/15/06   PMD     Updated File Comments with Interface Documentation
                   Updated File Copyright Information
05/11/06   PMD     Added Support for New Diag Commands and modified 
                   existing Diag Commands for Delayed Response
06/13/05   Rex     Added support for CB Delete All Response
05/10/04   Rex     Initial Version

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEComdef.h"


/* Class ID files */
#include "WMSDIAG.BID"
#include "WMSDIAG_NOTIFIER.BID"



/*===========================================================================

                          DEFINITIONS AND CONSTANTS

===========================================================================*/
#define NMASK_WMSDIAG_REQ   0x0001


#define CB_DATA_SIZE_MAX  88 /* maximum size of a SMS CB messsage is 88 bytes */


/*===========================================================================

                          DATA TYPE DECLARATIONS

===========================================================================*/


/* Request Commands from OEM Layer */
typedef enum
{
  AEEWMSDIAG_CMD_ID_CB_SAVE_REQ_DATA = 0x0000,
  AEEWMSDIAG_CMD_ID_CB_READ_REQ_DATA,
  AEEWMSDIAG_CMD_ID_CB_DELETE_MSG_REQ_DATA,
  AEEWMSDIAG_CMD_ID_CB_DELETE_ALL_MSGS_REQ_DATA
} AEEWMSDIAGCmdId;


typedef enum
{
  AEEWMSDIAG_STORAGE_TYPE_GW_RAM = 0,
  AEEWMSDIAG_STORAGE_TYPE_GW_EFS = 1,
  AEEWMSDIAG_STORAGE_TYPE_GW_ALL = 2
} AEEWMSDIAGStorageType;


typedef struct
{
  uint8 tid;
  uint16 ram_index;

} AEEWMSDIAGCbSaveReq;


typedef struct
{
  uint32 tid;
  AEEWMSDIAGStorageType aee_storage_type;
  uint16 index;

} AEEWMSDIAGCbReadReq;


typedef struct
{
  uint32 tid;
  AEEWMSDIAGStorageType aee_storage_type;
  uint32 index;

} AEEWMSDIAGCbDeleteMsgReq;


typedef struct
{
  uint32 tid;
  AEEWMSDIAGStorageType aee_storage_type;

} AEEWMSDIAGCbDeleteAllMsgsReq;


typedef union
{
  AEEWMSDIAGCbSaveReq  cb_save_req;
  /* save specific data */

  AEEWMSDIAGCbReadReq  cb_read_req;
  /* read specific data */

  AEEWMSDIAGCbDeleteMsgReq  cb_delete_msg_req;
  /* delete cb msg */

  AEEWMSDIAGCbDeleteAllMsgsReq cb_delete_all_msgs_req;
  /* delete all cb msgs */

} AEEWMSDIAGCmdReq;


typedef struct
{
  AEEWMSDIAGCmdId     cmd_id;
  AEEWMSDIAGCmdReq    cmd_req;

} AEEWMSDIAGNotifyInfo;

/* Response Commands send to the OEM Layer */
typedef struct
{
  uint8 transaction_ID;
  uint8 status_response;
  uint16 efs_index;

} AEEWMSDIAGCbSaveRsp;

typedef struct
{
  uint32 transaction_ID;
  uint8  status_response;
  uint8  time_year;
  uint8  time_month;

  uint8  time_day;
  uint8  time_hour;
  uint8  time_minute;
  uint8  time_second;

  uint8  cb_data[CB_DATA_SIZE_MAX];

} AEEWMSDIAGCbReadRsp;

typedef struct
{
  uint32 transaction_ID;
  uint32 status_response;

} AEEWMSDIAGCbDeleteMsgRsp;

typedef struct
{
  uint32 transaction_ID;
  uint32 status_response;

} AEEWMSDIAGCbDeleteAllMsgsRsp;


/* Event payload to Diag */
typedef struct
{
  uint16 service_id;
  uint8  update_num;
  uint16 msg_code;
  uint8  geo_scope;
  uint8  page_num;
  uint8  total_page;
  uint16 msg_index;

} AEEWMSDIAGEventCbSmsNotify;

/* Static buffer in AEE layer where OEM can fill up information */


typedef struct IWMSDIAG IWMSDIAG;

typedef struct IWMSDIAGNotifier IWMSDIAGNotifier;


/*===========================================================================

                         CLASS DEFINITIONS

===========================================================================*/

AEEINTERFACE(IWMSDIAG)
{
  INHERIT_IQueryInterface(IWMSDIAG);

  int (*CbSaveResponse) (IWMSDIAG *pMe, AEEWMSDIAGCbSaveRsp *rsp_pkt);

  int (*CbReadResponse) (IWMSDIAG *pMe, uint16 payloadSize, AEEWMSDIAGCbReadRsp *rsp_pkt);

  int (*CbDeleteMsgResponse) (IWMSDIAG *pMe, AEEWMSDIAGCbDeleteMsgRsp *rsp_pkt);

  int (*CbDeleteAllMsgsResponse) (IWMSDIAG *pMe, AEEWMSDIAGCbDeleteAllMsgsRsp *rsp_pkt);

  int (*EventCbSmsNotify) (IWMSDIAG *pMe, AEEWMSDIAGEventCbSmsNotify *payload);
};


AEEINTERFACE(IWMSDIAGNotifier)
{
  INHERIT_INotifier(IWMSDIAGNotifier);
};


/* Defines for accessing the IWMSDIAG class */
#define IWMSDIAG_AddRef(p)                            AEEGETPVTBL(p,IWMSDIAG)->AddRef(p)
#define IWMSDIAG_Release(p)                           AEEGETPVTBL(p,IWMSDIAG)->Release(p)
#define IWMSDIAG_QueryInterface(p, clsid, pp)         AEEGETPVTBL(p,IWMSDIAG)->QueryInterface(p, clsid, pp)
#define IWMSDIAG_CbSaveResponse(p, rp)                AEEGETPVTBL(p,IWMSDIAG)->CbSaveResponse(p, rp)
#define IWMSDIAG_CbReadResponse(p, plsz, rp)          AEEGETPVTBL(p,IWMSDIAG)->CbReadResponse(p, plsz, rp)
#define IWMSDIAG_CbDeleteMsgResponse(p, rp)           AEEGETPVTBL(p,IWMSDIAG)->CbDeleteMsgResponse(p, rp)
#define IWMSDIAG_CbDeleteAllMsgsResponse(p, rp)       AEEGETPVTBL(p,IWMSDIAG)->CbDeleteAllMsgsResponse(p, rp)
#define IWMSDIAG_EventCbSmsNotify(p, pl)              AEEGETPVTBL(p,IWMSDIAG)->EventCbSmsNotify(p, pl)


/* Defines for accessing the IWMSDIAGNotifier class */
#define IWMSDIAGNotifier_AddRef(p)                            AEEGETPVTBL(p,IWMSDIAGNotifier)->AddRef(p)
#define IWMSDIAGNotifier_Release(p)                           AEEGETPVTBL(p,IWMSDIAGNotifier)->Release(p)
#define IWMSDIAGNotifier_QueryInterface(p, clsid, pp)         AEEGETPVTBL(p,IWMSDIAGNotifier)->QueryInterface(p, clsid, pp)


#endif    /* AEEWMSDIAG_H */


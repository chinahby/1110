/*=============================================================================
FILE:  OEMWMS.c

SERVICES:  OEM functions for supporting BREW WMSAPP Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW IWMS interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright © 1999,2000,2001,2002,2003,2004,2005,2006,2007,2008
                    by QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/OEMWMS.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/08   PMD     Cleaned up Feature Flags in Code
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
05/19/08   HN      Support Read ACK
01/31/08   PMD     Updated Copyright Information
04/03/07   HQ      Added support for Apps Power Collapse.
01/17/07   HQ      Updated copyright info for 2007.
01/10/07   PMD     Fixed crash with other data parm in OEMWMS_MsgInfoCacheCb 
11/20/06   PMD     Appended 1 to length for Null Character in wms_ts_unpack_ascii
11/17/06   PMD     Initialized other in client_bd structure to 0
02/06/06   PMD     Fixed Compile Warnings
12/20/05   PMD     Added Function Headers.
12/20/05   PMD     Cleaned High Lint Warnings and removed Lint Suppressions
11/21/05   PMD     Fixed High Lint Warnings
10/19/05   VK      EMS Featurization fixes
10/20/05   PMD     Updated Copyright Information
10/20/05   PMD     Added IWMS_GetRetryPeriod API
10/20/05   PMD     Added IWMS_DbgGetRetryInterval and IWMS_DbgSetRetryInterval APIs
10/19/05   PMD     Mainlined FEATURE_SMS_TEST
                   Fixed Compile Warning
08/10/05   Rex     Fixed function return type consistency
06/08/05   Rex     Added checking in IWMS_MsgInfoCacheCb
03/10/05   PMD     Added IWMS_BcMmMsgDeleteAllIndication API
02/01/05   PMD     Added Support for IWMS_BcMmGetReadingPref() and 
                   IWMS_BcMmSetReadingPref() API
12/08/04   Rex     Fixed length checking for copying NonStdEmail user data
11/30/04   PMD     Added GSM Unicode support in MsgInfoCache.
11/03/04   PMD     Added Comments for various functions.
                   Added IWMS_CfgCheckWapPushMsg function.
09/22/04   Rex     Added IWMS_CfgCheckCphsMsg and IWMS_CfgLocateVoicemailLine2
06/19/04   PMD     Added IWMS_BcMmMsgDeleteIndication API
04/22/04   PMD     Added IWMS_MsgGetCacheInfo API
03/24/04   PMD     Modified IWMS_SetPrimaryClient and IWMS_SetMemoryFull APIs
03/16/04   AP      Added text length when assigning "No Info" to the cache.
                   Assigned a unique length to differentiate a broadcast msg.
03/09/04   PMD     Added IWMS_SetPrimaryClient and IWMS_SetMemoryFull APIs
02/03/04   PMD     Cleaned up Lint Warnings in code
01/30/04   PMD     Added IWMS_TsEncodeDCS and IWMS_TsDecodeDCS
01/26/04   PMD     Added ConcatHeader Info in MsgInfoCacheCb
01/26/04   PMD     Added IWMS_TsGetHeaderLength()
01/16/03   PMD	    Added IWMS_MsgSetRetryPeriod()
01/13/04   HQ      Added IWMS_CfgCheckCDMADuplicate().
01/07/04   PMD     Added IWMS_BcMmSetPriorityAllServices()
01/06/04   HQ      Added #include tmc.h.
12/17/03   PMD     Changed OEMWMS_MsgInfoCacheCb to use TMC heap rather 
                   than BREW Heap
12/02/03   PMD     Lint Related Changes 
12/01/03   PMD     Added Function to Register Parsing Callback
12/01/03   PMD     Removed RegisterMsgInfoCacheCb Function
11/06/03   PMD     Changes for Brew 3.0
11/03/03   PMD     Added Support for Automatic MO SMS Functions
10/31/03   PMD     Synced to the Latest MsgInfoCacheCb
10/31/03   PMD     Added Include for OEMWMS.h
10/22/03   PMD     Added New Function in IWMS to Register MsgInfoCache Callback
                   Added oemwms_email_address_s_type, OEMWMS_MsgInfoCacheCb, 
                   OEMWMS_IsNonStdEmail, OEMWMS_GetNonStdEmail and 
                   OEMWMS_ConvertFromUnicode
10/20/03   PMD     Added New Function to Map WMS Status to BREW Error Code
            
10/20/03   PMD     Featurization Cleanup in IWMS
10/20/03   PMD     Added WMS Transport Service (TS) group functions
10/19/03   PMD     Added Multimode Cell Broadcast Functions
                   Added Multimode BC Registration
10/03/03   AP      Added check for voicemail contents.
09/22/03   PMD     Added Functions to Clear SMMA and Reset Reference #
09/19/03   PMD     Added Functions for Get and Set GW Domain Preference
07/29/03   AP      Added DC registration.
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
02/14/03   AP      More support for bc group.
12/16/02   AP      Added support for dc and bc groups.
10/24/02   SUN     Initial Version
=============================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions

#include "AEE.h"         // BREW headers.
#include "AEE_OEM.h"     // Brew OEM Headers.
#include "AEEWMS.h"      // Header file for WMS interface
#include "OEMWMS.h"      // Header file for new WMS interface
#include "OEMHeap.h"
#include "tmc.h"

#define VOICEMAIL_ID        "VoiceMail"
#define VOICEMAIL_ID_LEN    9

/* The following structure is for the object which is allocated. */

struct IWMS
{
   AEEVTBL(IWMS)             *pvt;
   IShell                    *m_pIShell;
   AEECLSID                  m_cls;
   uint32                    m_uRefs;           /* Number of references to this obj */
};

#ifdef FEATURE_CDSMS
typedef struct
{
  boolean isNonStdEmail;
  uint8  address[WMS_MAX_LEN];
  uint8  len;
}oemwms_email_address_s_type;
#endif /* FEATURE_CDSMS */


/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

static uint32 OEMWMS_AddRef(IWMS *pMe);
static uint32 OEMWMS_Release(IWMS *pMe);

static int OEMWMS_QueryInterface(     IWMS     *pMe,
                                      AEECLSID id,
                                      void     **ppNew);

static int  OEMWMS_Init(              IWMS                    *pMe,
                                      wms_client_type_e_type   clientType,
                                      wms_client_id_type      *pClientId);

static int OEMWMS_Activate(
                                      IWMS                 *pMe,
                                      wms_client_id_type    clientId);

static int OEMWMS_Deactivate(         IWMS                  *pMe,
                                      wms_client_id_type    clientId);

static int OEMWMS_RegisterCfgCb(      IWMS                  *pMe,
                                      wms_client_id_type    clientId,
                                      wms_cfg_event_cb_type cfgCb);

static int OEMWMS_RegisterMsgCb(      IWMS                   *pMe,
                                      wms_client_id_type     clientId,
                                      wms_msg_event_cb_type  msgCb);

static int OEMWMS_RegisterBcMmCb(
                                      IWMS                    *pMe,
                                      wms_client_id_type      clientId,
                                      wms_bc_mm_event_cb_type bcMmCb);

static int OEMWMS_RegisterDcCb(       IWMS                   *pMe,
                                      wms_client_id_type     clientId,
                                      wms_dc_event_cb_type   dcCb);

static int OEMWMS_RegisterParsingCb(  IWMS                      *pMe,
                                      wms_client_id_type        clientId,
                                      wms_cl_parse_msg_cb_type  parsingCb);


static int OEMWMS_SetPrimaryClient(   IWMS                     *pMe,
                                      wms_client_id_type        client_id,
                                      AEECallback               *cmd_cb,
                                      const void                *user_data,
                                      boolean                   set_primary,
                                      boolean                   use_client_memory);

static int OEMWMS_SetMemoryFull(      IWMS                     *po,
                                      wms_client_id_type        client_id,
                                      AEECallback               *cmd_cb,
                                      const void                *user_data,
                                      boolean                   memory_full);

static int OEMWMS_CfgSetRoutes(       IWMS                      *pMe,
                                      wms_client_id_type        clientId,
                                      AEECallback               *cmd_cb,
                                      const void                *user_data,
                                      const wms_routes_s_type   *pRoute);

static int OEMWMS_CfgGetRoutes(       IWMS                      *pMe,
                                      wms_client_id_type        clientId,
                                      AEECallback               *cmd_cb,
                                      const void                *user_data);

static int OEMWMS_CfgGetMemoryStatus( IWMS                     *pMe,
                                      wms_client_id_type       clientId,
                                      AEECallback              *cmdCb,
                                      const void               * user_data,
                                      wms_memory_store_e_type  memStore,
                                      wms_message_tag_e_type   tag);

static int OEMWMS_CfgGetMessageList(  IWMS                      *pMe,
                                      wms_client_id_type        clientId,
                                      AEECallback               *cmdCb,
                                      const void                *user_data,
                                      wms_memory_store_e_type   memStore,
                                      wms_message_tag_e_type    tag);

static int OEMWMS_CfgGetStatusReportList( IWMS                      *pMe,
                                          wms_client_id_type        clientId,
                                          AEECallback               *cmdCb,
                                          const void                *user_data,
                                          wms_memory_store_e_type   memStore);

static wms_message_index_type OEMWMS_CfgGetMsgIndex ( IWMS                      *pMe,
                                                      wms_message_number_type   stsRptIndex);

static boolean OEMWMS_CfgCheckVoiceMail( IWMS                         *pMe,
                                         wms_message_mode_e_type      msgMode,
                                         const wms_raw_ts_data_s_type *rawTs,
                                         boolean                      *isActive,
                                         uint8                        *count);

static boolean OEMWMS_CfgCheckCphsMsg(  IWMS                         *pMe,
                                        const wms_address_s_type     *addr);

static boolean OEMWMS_CfgLocateVoicemailLine2(	IWMS                 *pMe,
                                                uint32               *index_line2);
	
static boolean OEMWMS_CfgCheckCDMADuplicate( IWMS                             *pMe,
                                             const wms_client_message_s_type  *pMsg);

static boolean OEMWMS_CfgCheckWapPushMsg( IWMS                             *pMe,
                                          const wms_client_message_s_type  *pMsg);

static int OEMWMS_CfgSetGWDomainPref( IWMS                             * pMe,
                                      wms_client_id_type                 cl,
                                      AEECallback                      * cmd_cb,
                                      const void                       * user_data,
                                      wms_gw_domain_pref_e_type         gw_domain_pref);

static int OEMWMS_CfgGetGWDomainPref( IWMS                             * pMe,
                                      wms_client_id_type                 cl,
                                      AEECallback                      * cmd_cb,
                                      const void                       * user_data);

static int OEMWMS_MsgSend(           IWMS                            *pMe,
                                     wms_client_id_type              clientId,
                                     AEECallback                     *cmd_cb,
                                     const void                      *user_data,
                                     wms_send_mode_e_type            send_mode,
                                     const wms_client_message_s_type *pMsg);

static int OEMWMS_MsgAck(            IWMS                      *pMe,
                                     wms_client_id_type         clientId,
                                     AEECallback               *cmd_cb,
                                     const void                *user_data,
                                     const wms_ack_info_s_type *pAck);

static int OEMWMS_MsgWrite(          IWMS                           *pMe,
                                     wms_client_id_type             clientId,
                                     AEECallback                    *cmd_cb,
                                     const void                     *user_data,
                                     wms_write_mode_e_type          writeMode,
                                     const wms_client_message_s_type *pMsg);

static int OEMWMS_MsgRead(           IWMS                    *pMe,
                                     wms_client_id_type      clientId,
                                     AEECallback             *cmd_cb,
                                     const void              *user_data,
                                     wms_memory_store_e_type memStore,
                                     wms_message_index_type  index);

static int OEMWMS_MsgDelete(         IWMS                    *pMe,
                                     wms_client_id_type      clientId,
                                     AEECallback             *cmd_cb,
                                     const void              *user_data,
                                     wms_memory_store_e_type memStore,
                                     wms_message_index_type  index);

static int OEMWMS_MsgDeleteAll(      IWMS                    *pMe,
                                     wms_client_id_type       clientId,
                                     AEECallback             *cmd_cb,
                                     const void              *user_data,
                                     wms_memory_store_e_type  memStore,
                                     wms_message_tag_e_type   tag);

static int OEMWMS_MsgModifyTag(      IWMS                    *pMe,
                                     wms_client_id_type      clientId,
                                     AEECallback             *cmd_cb,
                                     const void              *user_data,
                                     wms_memory_store_e_type memStore,
                                     wms_message_index_type  index,
                                     wms_message_tag_e_type  tag);

static int OEMWMS_MsgReadTemplate(   IWMS                    *pMe,
                                     wms_client_id_type       clientId,
                                     AEECallback             *cmd_cb,
                                     const void              *user_data,
                                     wms_memory_store_e_type memStore,
                                     wms_message_index_type  index);

static int OEMWMS_MsgWriteTemplate(  IWMS                             *pMe,
                                     wms_client_id_type                clientId,
                                     AEECallback                      *cmd_cb,
                                     const void                       *user_data,
                                     wms_write_mode_e_type             writeMode,
                                     const wms_client_message_s_type   *pMsg);


static int OEMWMS_MsgDeleteTemplate( IWMS                     *pMe,
                                     wms_client_id_type        clientId,
                                     AEECallback              *cmdCb,
                                     const void               *user_data,
                                     wms_memory_store_e_type   memStore,
                                     wms_message_index_type    index);

static int OEMWMS_MsgDeleteAllTemplate(IWMS                    *pMe,
                                       wms_client_id_type      clientId,
                                       AEECallback             *cmd_cb,
                                       const void              *user_data,
                                       wms_memory_store_e_type memStore);

static int OEMWMS_MsgReadStatusReport(IWMS                    *pMe,
                                       wms_client_id_type      clientId,
                                       AEECallback             *cmd_cb,
                                       const void              *user_data,
                                       wms_memory_store_e_type memStore,
                                       wms_message_index_type  index);

static int OEMWMS_MsgWriteStatusReport(IWMS                    *pMe,
                                       wms_client_id_type      clientId,
                                       AEECallback             *cmd_cb,
                                       const void              *user_data,
                                       wms_write_mode_e_type    writeMode,
                                       const wms_client_message_s_type   *pMsg);

static int OEMWMS_MsgDeleteStatusReport( IWMS                     *pMe,
                                         wms_client_id_type        clientId,
                                         AEECallback              *cmdCb,
                                         const void               *user_data,
                                         wms_memory_store_e_type   memStore,
                                         wms_message_index_type    index);

static int OEMWMS_MsgDeleteAllStatusReport(IWMS                    *pMe,
                                           wms_client_id_type      clientId,
                                           AEECallback             *cmd_cb,
                                           const void              *user_data,
                                           wms_memory_store_e_type memStore);

static int OEMWMS_MsgGetRetryPeriod    (IWMS                     *pMe,
                                        wms_client_id_type        clientId,
                                        uint32                   *period);

static int OEMWMS_MsgSetRetryPeriod    (IWMS                     *pMe,
                                        wms_client_id_type        clientId,
                                        uint32                    period);

static int OEMWMS_MsgGetCacheInfo      (IWMS                     *pMe,
                                        wms_memory_store_e_type   mem_store,
                                        wms_message_index_type    index,
                                        wms_message_tag_e_type    tag,
                                        uint8                    *cache_ptr);

static int OEMWMS_DcEnableAutoDisconnect(  IWMS                  *pMe,
                                           wms_client_id_type    clientId,
                                           AEECallback           *cmd_cb,
                                           const void            *user_data,
                                           uint32                timeout);

static int  OEMWMS_DcDisableAutoDisconnect(IWMS                  *pMe,
                                           wms_client_id_type    clientId,
                                           AEECallback           *cmd_cb,
                                           const void            *user_data);

static int OEMWMS_DcConnect(               IWMS                  *pMe,
                                           wms_client_id_type    clientId,
                                           AEECallback           *cmd_cb,
                                           const void            *user_data,
                                           wms_dc_so_e_type      so);

static int OEMWMS_DcDisconnect(            IWMS                  *pMe,
                                           wms_client_id_type    clientId,
                                           AEECallback           *cmd_cb,
                                           const void            *user_data);

static int OEMWMS_DbgResetTLSeqNum(       IWMS                  *pMe,
                                          wms_client_id_type    clientId,
                                          AEECallback           *cmd_cb,
                                          const void            *user_data);

static int OEMWMS_DbgSetMsgRefNum(        IWMS                  *pMe,
                                          wms_client_id_type    clientId,
                                          AEECallback           *cmd_cb,
                                          const void            *user_data);

static int OEMWMS_DbgClearSMMAFlag(       IWMS                  *pMe,
                                          wms_client_id_type    clientId,
                                          AEECallback           *cmd_cb,
                                          const void            *user_data);

static int OEMWMS_DbgGetRetryInterval(    IWMS                  *pMe,
                                          wms_client_id_type     clientId,
                                          AEECallback           *cmd_cb,
                                          const void            *user_data);

static int OEMWMS_DbgSetRetryInterval(    IWMS                  *pMe,
                                          wms_client_id_type     clientId,
                                          AEECallback           *cmd_cb,
                                          const void            *user_data,
                                          uint32                 retry_interval);

static int OEMWMS_TsEncode(         IWMS                      *pMe,
                                    wms_client_ts_data_s_type *ClientTsDataPtr,
                                    wms_raw_ts_data_s_type    *RawTsDataPtr);

static int OEMWMS_TsDecode(         IWMS                      *pMe,
                                    wms_raw_ts_data_s_type    *RawTsDataPtr,
                                    wms_client_ts_data_s_type *ClientTsDataPtr); 

static uint8 OEMWMS_TsGetHeaderLength(   IWMS                 *pMe,
                                         wms_udh_s_type       *udhPtr);

static int OEMWMS_TsBcdToAscii(     IWMS                      *pMe,
                                    uint8                     *InputPtr,
                                    uint8                      length,
                                    uint8                     *OutputPtr);

static int OEMWMS_TsAsciiToBcd(     IWMS                      *pMe,
                                    uint8                     *InputPtr,
                                    uint8                      length,
                                    uint8                     *OutputPtr);

static int OEMWMS_TsAsciiToDefault( IWMS                      *pMe,
                                    uint8                     *InputPtr,
                                    uint32                     length,
                                    uint8                     *OutputPtr);

static int OEMWMS_TsDefaultToAscii( IWMS                      *pMe,
                                    uint8                     *InputPtr,
                                    uint32                     length,
                                    uint8                     *OutputPtr);

static int OEMWMS_TsDecodeRelativeTime(   IWMS                      *pMe,
                                          uint8                      nTime,
                                          wms_timestamp_s_type      *TimeStampPtr);

static uint8 OEMWMS_TsEncodeRelativeTime( IWMS                      *pMe,
                                          wms_timestamp_s_type      *TimeStampPtr);

static uint8 OEMWMS_TsDecodeDCS(    IWMS                      *pMe,
                                    uint8                     *data,
                                    wms_gw_dcs_s_type         *dcs);

static uint8 OEMWMS_TsEncodeDCS(    IWMS                      *pMe,
                                    wms_gw_dcs_s_type         *dcs,
                                    uint8                     *data);

static byte OEMWMS_TsPackAscii(     IWMS                      *pMe,
                                    char                      *AsciiPtr,
                                    byte                      *DataPtr,
                                    byte                      *DataLengthPtr,
                                    byte                      *PaddingBitsPtr);

static byte OEMWMS_TsUnpackAscii(   IWMS                      *pMe,
                                    wms_cdma_user_data_s_type *UserDataPtr,
                                    byte                       length,
                                    byte                      *OutputPtr);

static byte OEMWMS_TsDtmfToAscii(   IWMS                      *pMe,
                                    byte                       length,
                                    byte                      *InputPtr,
                                    byte                      *OutputPtr);

static byte OEMWMS_TsAsciiToDtmf(   IWMS                      *pMe,
                                    char                      *InputPtr,
                                    byte                      *OutputPtr);
                    
static int OEMWMS_BcMmGetConfig(          IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode);


static int OEMWMS_BcMmGetPref(            IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode);


static int OEMWMS_BcMmSetPref(            IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode,
                                          wms_bc_pref_e_type             pref);

static int OEMWMS_BcMmGetTable(           IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode);

static int OEMWMS_BcMmSelectService(      IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode,
                                          wms_bc_mm_srv_id_type         *srv_id_ptr,
                                          boolean                        selected);

static int OEMWMS_BcMmGetAllServiceIds(   IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode);

static int OEMWMS_BcMmGetServiceInfo(     IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode,
                                          wms_bc_mm_srv_id_type         *srv_id_ptr);

static int OEMWMS_BcMmAddServices(        IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode,
                                          uint8                          num_entries,
                                          wms_bc_mm_service_info_s_type  *entries);

static int OEMWMS_BcMmDeleteServices(     IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode,
                                          uint8                          num_entries,
                                          wms_bc_mm_srv_id_type         *srv_ids);

static int OEMWMS_BcMmChangeServiceInfo(  IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode,
                                          wms_bc_mm_service_info_s_type  *srv_info_ptr);

static int OEMWMS_BcMmDeleteAllServices(  IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode);

static int OEMWMS_BcMmSelectAllServices(  IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode,
                                          boolean                        selected);

static int OEMWMS_BcMmSetPriorityAllServices(  IWMS                      *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode,
                                          wms_priority_e_type            priority);

static int OEMWMS_BcMmMsgDeleteIndication(  IWMS                         *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_gw_cb_page_header_s_type   *cb_page_header);

static int OEMWMS_BcMmMsgDeleteAllIndication(  IWMS                         *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data);

static int OEMWMS_BcMmGetReadingPref(     IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode);

static int OEMWMS_BcMmSetReadingPref(     IWMS                           *pMe,
                                          wms_client_id_type             clientId,
                                          AEECallback                    *cmd_cb,
                                          const void                     *user_data,
                                          wms_message_mode_e_type        message_mode,
                                          boolean                        reading_advised,
                                          boolean                        reading_optional);

static const AEEVTBL(IWMS) gOEMWMSFuncs = {
    OEMWMS_AddRef
   ,OEMWMS_Release
   ,OEMWMS_QueryInterface
   ,OEMWMS_Init
   ,OEMWMS_Activate
   ,OEMWMS_Deactivate
   ,OEMWMS_RegisterCfgCb
   ,OEMWMS_RegisterMsgCb
   ,OEMWMS_RegisterBcMmCb
   ,OEMWMS_RegisterDcCb
   ,OEMWMS_RegisterParsingCb
   ,OEMWMS_SetPrimaryClient
   ,OEMWMS_SetMemoryFull
   ,OEMWMS_CfgSetRoutes
   ,OEMWMS_CfgGetRoutes
   ,OEMWMS_CfgGetMemoryStatus
   ,OEMWMS_CfgGetMessageList
   ,OEMWMS_CfgGetStatusReportList
   ,OEMWMS_CfgGetMsgIndex
   ,OEMWMS_CfgCheckVoiceMail
   ,OEMWMS_CfgCheckCphsMsg
   ,OEMWMS_CfgLocateVoicemailLine2
   ,OEMWMS_CfgCheckCDMADuplicate
   ,OEMWMS_CfgCheckWapPushMsg
   ,OEMWMS_CfgSetGWDomainPref
   ,OEMWMS_CfgGetGWDomainPref
   ,OEMWMS_MsgSend
   ,OEMWMS_MsgAck
   ,OEMWMS_MsgRead
   ,OEMWMS_MsgWrite
   ,OEMWMS_MsgDelete
   ,OEMWMS_MsgDeleteAll
   ,OEMWMS_MsgModifyTag
   ,OEMWMS_MsgReadTemplate
   ,OEMWMS_MsgWriteTemplate
   ,OEMWMS_MsgDeleteTemplate
   ,OEMWMS_MsgDeleteAllTemplate
   ,OEMWMS_MsgReadStatusReport
   ,OEMWMS_MsgWriteStatusReport
   ,OEMWMS_MsgDeleteStatusReport
   ,OEMWMS_MsgDeleteAllStatusReport
   ,OEMWMS_MsgGetRetryPeriod
   ,OEMWMS_MsgSetRetryPeriod
   ,OEMWMS_MsgGetCacheInfo
   ,OEMWMS_DcEnableAutoDisconnect
   ,OEMWMS_DcDisableAutoDisconnect
   ,OEMWMS_DcConnect
   ,OEMWMS_DcDisconnect
   ,OEMWMS_DbgResetTLSeqNum
   ,OEMWMS_DbgSetMsgRefNum
   ,OEMWMS_DbgClearSMMAFlag
   ,OEMWMS_DbgGetRetryInterval
   ,OEMWMS_DbgSetRetryInterval
   ,OEMWMS_TsEncode
   ,OEMWMS_TsDecode	
   ,OEMWMS_TsGetHeaderLength
   ,OEMWMS_TsBcdToAscii
   ,OEMWMS_TsAsciiToBcd
   ,OEMWMS_TsAsciiToDefault
   ,OEMWMS_TsDefaultToAscii
   ,OEMWMS_TsDecodeRelativeTime
   ,OEMWMS_TsEncodeRelativeTime
   ,OEMWMS_TsDecodeDCS
   ,OEMWMS_TsEncodeDCS
   ,OEMWMS_TsPackAscii
   ,OEMWMS_TsUnpackAscii
   ,OEMWMS_TsDtmfToAscii
   ,OEMWMS_TsAsciiToDtmf
   ,OEMWMS_BcMmGetConfig
   ,OEMWMS_BcMmGetPref
   ,OEMWMS_BcMmSetPref
   ,OEMWMS_BcMmGetTable
   ,OEMWMS_BcMmSelectService
   ,OEMWMS_BcMmGetAllServiceIds
   ,OEMWMS_BcMmGetServiceInfo
   ,OEMWMS_BcMmAddServices
   ,OEMWMS_BcMmDeleteServices
   ,OEMWMS_BcMmChangeServiceInfo
   ,OEMWMS_BcMmDeleteAllServices
   ,OEMWMS_BcMmSelectAllServices
   ,OEMWMS_BcMmSetPriorityAllServices
   ,OEMWMS_BcMmMsgDeleteIndication
   ,OEMWMS_BcMmMsgDeleteAllIndication
   ,OEMWMS_BcMmGetReadingPref
   ,OEMWMS_BcMmSetReadingPref
};

/***********************************************************************
 * D A T A    O B J E C T S
 ***********************************************************************/
static struct IWMS *IWMSobj = NULL;



/* The following structure is used to notify clients of WMS events */

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

/*===========================================================================

FUNCTION: OEMWMS_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
===========================================================================*/
int OEMWMS_New(IShell *pIShell, AEECLSID cls, void **ppif)
{

#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

   if (cls != AEECLSID_WMS) {
      return EUNSUPPORTED;
   }

   if (IWMSobj == NULL)
   {
     // Allocate the object.
     if ((IWMSobj = (IWMS*)OEM_Malloc(sizeof(IWMS))) == NULL) {
        return ENOMEMORY;
     }

     IWMSobj->pvt = (IWMSVtbl *)&gOEMWMSFuncs;
     IWMSobj->m_pIShell = pIShell;
     IWMSobj->m_cls = cls;
     IWMSobj->m_uRefs = 1;

     *ppif = IWMSobj;

   }
   else
   {
     (void)OEMWMS_AddRef(IWMSobj);
     *ppif = IWMSobj;
   }

   return AEE_SUCCESS;

}
/*===========================================================================

FUNCTION: OEMWMS_AddRef

DESCRIPTION
  Indicates that one more object is now pointing to the OEMWMS object.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static uint32 OEMWMS_AddRef(IWMS *pMe)
{

   if (pMe) {
      return (++pMe->m_uRefs);
   }
   return 0;
}

/*===========================================================================

FUNCTION: OEMWMS_Release

DESCRIPTION
  Decreases the number of references to the IWMS object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static uint32 OEMWMS_Release(IWMS *pMe)
{
   if (!pMe) {
      return (0);
   }

   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }

   // Ref count is zero.  Release memory
   OEM_Free(pMe);
   IWMSobj = NULL;
   return (0);
}

/*===========================================================================

FUNCTION: OEMWMS_QueryInterface

DESCRIPTION
  Allows applications to query if IWMS supports other interfaces and then
  returns a pointer to that new interface supported via IWMS.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
===========================================================================*/

static int OEMWMS_QueryInterface(IWMS *pMe, AEECLSID id, void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!pMe) || (!ppNew)) {
      return EBADPARM;
   }

   ret_val = ECLASSNOTSUPPORT;
   return ret_val;
}

/*===========================================================================

FUNCTION    MapWMSStatusToBREWCode

DESCRIPTION
  This function is used to Map the WMS Status Codes to BREW (AEE) Error Codes

DEPENDENCIES
  None

RETURN VALUE
  AEE Error Code

SIDE EFFECTS
  None

===========================================================================*/
int MapWMSStatusToBREWCode(
  wms_status_e_type WMSStatus
)
{
  /* Initialized Return Value to EFAILED 
     In order to handle all other statuses */
  int retVal = EFAILED;
  
  switch (WMSStatus)
  {
    case WMS_OK_S:
      retVal = SUCCESS;
      break;

    case WMS_OUT_OF_RESOURCES_S:
      retVal = ENOMEMORY;
      break;

    case WMS_UNSUPPORTED_S:
      retVal = EUNSUPPORTED;
      break;

    case WMS_INVALID_TRANSACTION_ID_S:
    case WMS_INVALID_FORMAT_S:
    case WMS_NULL_PTR_S:
    case WMS_INVALID_PARM_SIZE_S:
    case WMS_INVALID_USER_DATA_SIZE_S:
    case WMS_INVALID_PARM_VALUE_S:
    case WMS_MISSING_PARM_S:
    case WMS_INVALID_TPDU_TYPE_S:
    case WMS_INVALID_VALIDITY_FORMAT_S:
    case WMS_INVALID_CB_DATA_S:
      retVal = EBADPARM;
      break;

    default:
      break;
  }
  return retVal;
}

/*===========================================================================

FUNCTION: OEMWMS_Init

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMWMS_Init
(
  IWMS                   *pMe,
  wms_client_type_e_type clientType,
  wms_client_id_type     *pClientId
)
{
  wms_client_err_e_type   client_err;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  client_err = wms_client_init(clientType,  pClientId );
#endif /* FEATURE_APPS_POWER_COLLAPSE */

  if( client_err != WMS_CLIENT_ERR_NONE )
  {
    ERR_FATAL("client_init error", 0,0,0);
  }


  /* done */
  return SUCCESS;

} /* OEMWMS_Init() */

/*===========================================================================

FUNCTION: OEMWMS_Activate

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int  OEMWMS_Activate
(
  IWMS                  *pMe,
  wms_client_id_type    clientId
)
{
  wms_client_err_e_type client_err;

  client_err = wms_client_activate( clientId);

  if( client_err  != WMS_CLIENT_ERR_NONE )
  {
    ERR_FATAL("client_act error", 0,0,0);
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION: OEMWMS_Deactivate

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int  OEMWMS_Deactivate
(
  IWMS                  *pMe,
  wms_client_id_type    clientId
)
{
  if( wms_client_deactivate( clientId ) != WMS_CLIENT_ERR_NONE )
  {
    ERR_FATAL("client_act error", 0,0,0);
  }
  return SUCCESS;
}

/*===========================================================================

FUNCTION: OEMWMS_RegisterCfgCb

DESCRIPTION
  Registers Configuration callback to WMS.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMWMS_RegisterCfgCb
( IWMS                  *pMe,
  wms_client_id_type    clientId,
  wms_cfg_event_cb_type cfgCb
)
{
  wms_client_err_e_type client_err;

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  client_err = wms_client_reg_cfg_cb( clientId, cfgCb );
#endif /* FEATURE_APPS_POWER_COLLAPSE */

  if( client_err != WMS_CLIENT_ERR_NONE )
    return EFAILED;
  else
    return SUCCESS;
}

/*===========================================================================

FUNCTION: OEMWMS_RegisterMsgCb

DESCRIPTION
  Registers Message callback to WMS.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMWMS_RegisterMsgCb
(
  IWMS                  *pMe,
  wms_client_id_type    clientId,
  wms_msg_event_cb_type msgCb
)
{
  wms_client_err_e_type client_err;

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  client_err = wms_client_reg_msg_cb( clientId, msgCb );
#endif /* FEATURE_APPS_POWER_COLLAPSE */

  if( client_err != WMS_CLIENT_ERR_NONE )
    return EFAILED;
  else
    return SUCCESS;
}

/*===========================================================================

FUNCTION: OEMWMS_RegisterBcMmCb

DESCRIPTION
  Registers multimode broadcast sms callback to WMS.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMWMS_RegisterBcMmCb
(
  IWMS                    *pMe,
  wms_client_id_type      clientId,
  wms_bc_mm_event_cb_type bcMmCb
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  wms_client_err_e_type client_err;

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  client_err = wms_client_reg_bc_mm_cb( clientId, bcMmCb );
#endif /* FEATURE_APPS_POWER_COLLAPSE */

  if( client_err != WMS_CLIENT_ERR_NONE )
    return EFAILED;
  else
    return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
}

/*===========================================================================

FUNCTION: OEMWMS_RegisterDcCb

DESCRIPTION
  Registers dedicated channel sms callback to WMS.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMWMS_RegisterDcCb
(
  IWMS                  *pMe,
  wms_client_id_type    clientId,
  wms_dc_event_cb_type  dcCb
)
{
#ifdef FEATURE_CDSMS
  wms_client_err_e_type client_err;

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  client_err = wms_client_reg_dc_cb( clientId, dcCb );
#endif /* FEATURE_APPS_POWER_COLLAPSE */

  if( client_err != WMS_CLIENT_ERR_NONE )
    return EFAILED;
  else
    return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_CDSMS */
}

/*===========================================================================

FUNCTION: OEMWMS_RegisterParsingCb

DESCRIPTION
  Registers Message Parsing callback to WMS.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMWMS_RegisterParsingCb
(
  IWMS                     *pMe,
  wms_client_id_type        clientId,
  wms_cl_parse_msg_cb_type  parsingCb
)
{
  wms_client_err_e_type client_err;

  client_err = wms_client_reg_parse_msg_cb( clientId, parsingCb );

  if( client_err != WMS_CLIENT_ERR_NONE )
    return EFAILED;
  else
    return SUCCESS;
}

/*===========================================================================

FUNCTION: OEMWMS_SetPrimaryClient

DESCRIPTION
  Sets the Primary Client

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Success
  EFAILED   - Failure

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_SetPrimaryClient
(
  IWMS                     *pMe,
  wms_client_id_type        client_id,
  AEECallback              *cmd_cb,
  const void               *user_data,
  boolean                   set_primary,
  boolean                   use_client_memory
)
{
  wms_status_e_type   st;

  st = wms_cfg_set_primary_client(client_id, (wms_cmd_cb_type)cmd_cb->pfnNotify, user_data, set_primary, use_client_memory);

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================

FUNCTION: OEMWMS_SetMemoryFull

DESCRIPTION
  Sets the Primary Client's Memory Status

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Success
  EFAILED   - Failure

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_SetMemoryFull
(
  IWMS                     *pMe,
  wms_client_id_type        client_id,
  AEECallback              *cmd_cb,
  const void               *user_data,
  boolean                   memory_full
)
{
  wms_status_e_type   st;

  st = wms_cfg_set_memory_full(client_id, (wms_cmd_cb_type)cmd_cb->pfnNotify, user_data, memory_full);

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_CfgSetRoutes,

DESCRIPTION
  Allows UI to the change the message routing configuration

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_CfgSetRoutes
(
  IWMS                      *pMe,
  wms_client_id_type         clientId,
  AEECallback               *cmdCb,
  const void                *user_data,
  const wms_routes_s_type   * pRoutes
)
{
  wms_status_e_type   st;
  st = wms_cfg_set_routes( clientId,
                            (wms_cmd_cb_type) cmdCb->pfnNotify,
                            user_data,
                            pRoutes
                          );
  
  return (MapWMSStatusToBREWCode(st));
}


/*===========================================================================
FUNCTION: OEMWMS_CfgGetRoutes,

DESCRIPTION
  Allows UI to retrieve the message routing configuration

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_CfgGetRoutes
(
  IWMS                     *pMe,
  wms_client_id_type        clientId,
  AEECallback              *cmdCb,
  const void               *user_data
)
{
  wms_status_e_type   st;
  st = wms_cfg_get_routes( clientId,
                            (wms_cmd_cb_type) cmdCb->pfnNotify,
                            user_data
                          );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_CfgGetMemoryStatus

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_CfgGetMemoryStatus
(
  IWMS                     *pMe,
  wms_client_id_type        clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type   memStore,
  wms_message_tag_e_type    tag

)
{
  wms_status_e_type   st;
  st = wms_cfg_get_memory_status( clientId,
                                  (wms_cmd_cb_type) cmdCb->pfnNotify,
                                   user_data, // user_data */
                                   memStore,
                                   tag
                                 );

  return (MapWMSStatusToBREWCode(st));
}
/*===========================================================================

FUNCTION: OEMWMS_CfgGETMESSAGELIST

DESCRIPTION
   Sends a command to the API. It gets a list of all messages(MO and MT)
  in the partcular memory store

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
  none
===========================================================================*/

static int OEMWMS_CfgGetMessageList
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type  memStore,
  wms_message_tag_e_type   tag
)
{
  wms_status_e_type   st;

  st = wms_cfg_get_message_list( clientId,
                                  (wms_cmd_cb_type) cmdCb->pfnNotify,
                                  user_data, // user_data */
                                  memStore,
                                  tag /* all messages */
                                );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================

FUNCTION: OEMWMS_CfgGetStatusReportList

DESCRIPTION
   Sends a command to the API. It gets a list of status reports
  in the partcular memory store

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
  none
===========================================================================*/

static int OEMWMS_CfgGetStatusReportList
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type  memStore
)
{
#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#else 
  return EUNSUPPORTED;
#endif /*FEATURE_GWSMS_STATUS_REPORTS*/
}


/*===========================================================================

FUNCTION: OEMWMS_CfgGetMsgIndex

DESCRIPTION
   Sends a command to the API. It gets a list of status reports
  in the partcular memory store

DEPENDENCIES
  none

RETURN VALUE
  The Message Index of the Message for which the Status Report Index was 
  provided

SIDE EFFECTS
  none
===========================================================================*/

static wms_message_index_type OEMWMS_CfgGetMsgIndex
(
  IWMS                     *pMe,
  wms_message_number_type   stsRptIndex
)
{
#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /*FEATURE_GWSMS_STATUS_REPORTS*/
  {
    return WMS_DUMMY_MESSAGE_INDEX;
  }
}	   

/*===========================================================================

FUNCTION: OEMWMS_CfgCheckVoiceMail

DESCRIPTION
   Checks if the given Message is a Voicemail Message or Not. 

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE
  
SIDE EFFECTS
  none
===========================================================================*/

static boolean OEMWMS_CfgCheckVoiceMail
(
  IWMS                         *pMe,
  wms_message_mode_e_type      msgMode,
  const wms_raw_ts_data_s_type *rawTs,
  boolean                      *isActive,
  uint8                        *count
)
{
  return(wms_cfg_check_voicemail_contents(msgMode,rawTs,isActive,count));
}

/*===========================================================================

FUNCTION: OEMWMS_CfgCheckCphsMsg

DESCRIPTION
   Checks if the given address is the address of a CPHS Message.

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE
  
SIDE EFFECTS
  none
===========================================================================*/

static boolean OEMWMS_CfgCheckCphsMsg
(
  IWMS                         *pMe,
  const wms_address_s_type     *addr
)
{
  return(wms_cfg_check_cphs_msg(addr));
}

/*===========================================================================

FUNCTION: OEMWMS_CfgLocateVoicemailLine2

DESCRIPTION
   Locates the Voicemail Line 2 Index. 

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE based on success / failure
  
SIDE EFFECTS
  none
===========================================================================*/

static boolean OEMWMS_CfgLocateVoicemailLine2
(
  IWMS 							*pMe, 
  uint32 						*index_line2
)
{
  return(wms_cfg_locate_voicemail_line2(index_line2));
}

/*===========================================================================

FUNCTION: OEMWMS_CfgCheckCDMADuplicate

DESCRIPTION
   Checks if the given CDMA Message is duplicate or Not.

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE
  
SIDE EFFECTS
  none
===========================================================================*/

static boolean OEMWMS_CfgCheckCDMADuplicate
(
  IWMS                            * pMe,
  const wms_client_message_s_type * pMsg
)
{
  return( wms_cfg_check_cdma_duplicate( pMsg ) );
}

/*===========================================================================

FUNCTION: OEMWMS_CfgCheckWapPushMsg

DESCRIPTION
   Checks if the given Message is a WAP Push Message or Not.

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE
  
SIDE EFFECTS
  none
===========================================================================*/

static boolean OEMWMS_CfgCheckWapPushMsg
(
  IWMS                            * pMe,
  const wms_client_message_s_type * pMsg
)
{
  return( wms_cfg_check_wap_push_message( pMsg ) );
}

/*===========================================================================

FUNCTION: OEMWMS_CfgSetGWDomainPref

DESCRIPTION
   Sends a command to the API. It sets the Domain Preference for 
   GSM / WCMDA (GW).

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EFAILED   - Request Failed
  
SIDE EFFECTS
  none
===========================================================================*/

static int OEMWMS_CfgSetGWDomainPref
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_gw_domain_pref_e_type gw_domain_pref
)
{
  wms_status_e_type   st;

  st = wms_cfg_set_gw_domain_pref(clientId,
                                  (wms_cmd_cb_type) cmdCb->pfnNotify,
                                  user_data, 
                                  gw_domain_pref
                                 );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================

FUNCTION: OEMWMS_CfgGetGWDomainPref

DESCRIPTION
   Sends a command to the API. It gets the Current Domain Preference for 
   GSM / WCMDA (GW).

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EFAILED   - Request Failed
  
SIDE EFFECTS
  none
===========================================================================*/

static int OEMWMS_CfgGetGWDomainPref
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data
)
{
  wms_status_e_type   st;

  st = wms_cfg_get_gw_domain_pref(clientId,
                                  (wms_cmd_cb_type) cmdCb->pfnNotify,
                                  user_data 
                                 );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgSend

DESCRIPTION
  Sends the message that has not been stored in any memory store

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgSend
(
  IWMS                              *pMe,
  wms_client_id_type                clientId,
  AEECallback                       *cmdCb,
  const void                        *user_data,
  wms_send_mode_e_type              send_mode,
  const wms_client_message_s_type   *pMsg
)
{
  wms_status_e_type   st;
  
  if (cmdCb != NULL) 
  {
    st =  wms_msg_send( clientId,
                        (wms_cmd_cb_type) cmdCb->pfnNotify,
                        user_data,
                        send_mode,
                        pMsg
                      );
  }
  else
  {
    st =  wms_msg_send( clientId,
                        NULL,
                        user_data,
                        send_mode,
                        pMsg
                      );
  }

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgAck,

DESCRIPTION
 Acks the MT message

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgAck
(
  IWMS                            *pMe,
  wms_client_id_type               clientId,
  AEECallback                     *cmdCb,
  const void                      *user_data,
  const wms_ack_info_s_type       *pAck
)
{
  wms_status_e_type   st;

  st = wms_msg_ack ( clientId,
                     (wms_cmd_cb_type) cmdCb->pfnNotify,
                     user_data,
                     pAck
                   );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================

FUNCTION: OEMWMSMsgWRITE

DESCRIPTION
 Stores the message in the memory store contained in msg_ptr either in the
 Insert mode or Replace mode

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgWrite
(
  IWMS                              *pMe,
  wms_client_id_type                clientId,
  AEECallback                       *cmdCb,
  const void                        *user_data,
  wms_write_mode_e_type             writeMode,
  const wms_client_message_s_type   *pMsg
)
{
  wms_status_e_type   st;

  st = wms_msg_write( clientId,
                     (wms_cmd_cb_type)cmdCb->pfnNotify,
                      user_data,
                      writeMode,
                      pMsg
                     );

  return (MapWMSStatusToBREWCode(st));
}
/*===========================================================================

FUNCTION: OEMWMSMsgREAD

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/

static int OEMWMS_MsgRead
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type  memStore,
  wms_message_index_type   index
)
{
  wms_status_e_type   st;

  st = wms_msg_read( clientId,
                    (wms_cmd_cb_type) cmdCb->pfnNotify,
                     user_data,
                     memStore,
                     index
                   );

  return (MapWMSStatusToBREWCode(st));
}
/*===========================================================================

FUNCTION: OEMWMSMSGDEL

DESCRIPTION
 Deletes the message from the specified memory slot from the specified
 memory store


DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/

static int OEMWMS_MsgDelete
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type  memStore,
  wms_message_index_type   index
)
{
  wms_status_e_type   st;

   st = wms_msg_delete( clientId,
                     (wms_cmd_cb_type) cmdCb->pfnNotify,
                      user_data,
                      memStore,
                      index
                    );

  return (MapWMSStatusToBREWCode(st));
}
/*===========================================================================

FUNCTION: OEMWMS_MSGDeleteAll

DESCRIPTION
 Deletes all the messages from the specified memory store

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgDeleteAll
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type  memStore,
  wms_message_tag_e_type   tag
)
{
  wms_status_e_type   st;
  st = wms_msg_delete_all( clientId,
                           (wms_cmd_cb_type) cmdCb->pfnNotify,
                           user_data,
                           memStore,
                           tag
                         );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgModifyTag

DESCRIPTION
  Allows UI to change the status of the message

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgModifyTag
(
  IWMS                            *pMe,
  wms_client_id_type               clientId,
  AEECallback                     *cmdCb,
  const void                      *user_data,
  wms_memory_store_e_type         memStore,
  wms_message_index_type          index,
  wms_message_tag_e_type          tag
)
{
  wms_status_e_type   st;

  st = wms_msg_modify_tag ( clientId,
                            (wms_cmd_cb_type) cmdCb->pfnNotify,
                            user_data,
                            memStore,
                            index,
                            tag
                          );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgReadTemplate,

DESCRIPTION
  Reads the memory slot from the specified memory store

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgReadTemplate
(
  IWMS                     *pMe,
  wms_client_id_type        clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type   memStore,
  wms_message_index_type    index
)
{
  wms_status_e_type   st;
  st = wms_msg_read_template( clientId,
                               (wms_cmd_cb_type) cmdCb->pfnNotify,
                               user_data,
                               memStore,
                               index
                             );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgWriteTemplate,

DESCRIPTION
  Stores the template in the Insert/Replace mode in the memory store
  specified by msg_ptr

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgWriteTemplate
(
  IWMS                              *pMe,
  wms_client_id_type                clientId,
  AEECallback                       *cmdCb,
  const void                        *user_data,
  wms_write_mode_e_type             writeMode,
  const wms_client_message_s_type   *pMsg
)
{
  wms_status_e_type   st;
  st = wms_msg_write_template( clientId,
                                (wms_cmd_cb_type)  cmdCb->pfnNotify,
                                user_data,
                                writeMode,
                                pMsg
                              );

  return (MapWMSStatusToBREWCode(st));
}


/*===========================================================================
FUNCTION: OEMWMS_MsgDeleteTemplate,

DESCRIPTION
  Deletes the template from the specifed slot of the memory store

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgDeleteTemplate
(
  IWMS                     *pMe,
  wms_client_id_type        clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type   memStore,
  wms_message_index_type    index
)
{
  wms_status_e_type   st;
  st = wms_msg_delete_template( clientId,
                               (wms_cmd_cb_type)  cmdCb->pfnNotify,
                                 user_data,
                                 memStore,
                                 index
                               );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgDeleteAllTemplate,

DESCRIPTION
  Deletes all the  templates from the specifed slot of the memory store

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgDeleteAllTemplate
(
  IWMS                     *pMe,
  wms_client_id_type        clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type   memStore
)
{
  wms_status_e_type   st;
  st = wms_msg_delete_template_all( clientId,
                                     (wms_cmd_cb_type) cmdCb->pfnNotify,
                                     user_data,
                                     memStore
                                   );

  return (MapWMSStatusToBREWCode(st));
}
/*===========================================================================

FUNCTION: OEMWMS_MsgReadStatusReport

DESCRIPTION
  Reads the status report from the given memory store and index
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/

int OEMWMS_MsgReadStatusReport
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type  memStore,
  wms_message_index_type   index
)
{
  wms_status_e_type   st;

  st = wms_msg_read_sts_report( clientId,
                    (wms_cmd_cb_type) cmdCb->pfnNotify,
                     user_data,
                     memStore,
                     index
                   );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================

FUNCTION: OEMWMSMsgWRITEStatusReport

DESCRIPTION
 Stores the status report in the memory store contained in msg_ptr either in the
 Insert mode or Replace mode

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgWriteStatusReport
(
  IWMS                              *pMe,
  wms_client_id_type                clientId,
  AEECallback                       *cmdCb,
  const void                        *user_data,
  wms_write_mode_e_type             writeMode,
  const wms_client_message_s_type   *pMsg
)
{
  wms_status_e_type   st;

  st = wms_msg_write_sts_report( clientId,
                     (wms_cmd_cb_type)cmdCb->pfnNotify,
                      user_data,
                      writeMode,
                      pMsg
                     );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgDeleteStatusReport,

DESCRIPTION
  Deletes the Status Report from the specifed slot of the memory store

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgDeleteStatusReport
(
  IWMS                     *pMe,
  wms_client_id_type        clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type   memStore,
  wms_message_index_type    index
)
{
  wms_status_e_type   st;

  st = wms_msg_delete_sts_report( clientId,
                                 (wms_cmd_cb_type)  cmdCb->pfnNotify,
                                 user_data,
                                 memStore,
                                 index
                               );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgDeleteAllStatusReport,

DESCRIPTION
  Deletes all the  status reports from the specifed slot of the memory store

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgDeleteAllStatusReport
(
  IWMS                     *pMe,
  wms_client_id_type        clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_memory_store_e_type   memStore
)
{
  wms_status_e_type   st;

  st = wms_msg_delete_sts_report_all( clientId,
                                     (wms_cmd_cb_type) cmdCb->pfnNotify,
                                     user_data,
                                     memStore
                                   );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgGetRetryPeriod

DESCRIPTION
  Gets the SMS Retry Period

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EUNSUPPORTED  - Not Supported

SIDE EFFECTS

===========================================================================*/
int OEMWMS_MsgGetRetryPeriod
(
  IWMS                             * pMe,
  wms_client_id_type                 clientId,
  uint32                           * period  
)
{
  wms_status_e_type   st;
  
  st =  wms_msg_get_retry_period( clientId,
                                  period
                                );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgSetRetryPeriod

DESCRIPTION
  Sets the SMS Retry Period

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EUNSUPPORTED  - Not Supported

SIDE EFFECTS

===========================================================================*/
int OEMWMS_MsgSetRetryPeriod
(
  IWMS                             * pMe,
  wms_client_id_type                 clientId,
  uint32                             period  
)
{
  wms_status_e_type   st;
  
  st =  wms_msg_set_retry_period( clientId,
                                  period
                                );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgGetCacheInfo

DESCRIPTION
  Gets the Cached Message Information.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EUNSUPPORTED  - Not Supported

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgGetCacheInfo
(
  IWMS                     *pMe,
  wms_memory_store_e_type   mem_store,
  wms_message_index_type    index,
  wms_message_tag_e_type    tag,
  uint8                    *cache_ptr
)
{
  wms_status_e_type   st;
  
  st =  wms_msg_get_cache_info( mem_store, index, tag, cache_ptr);

  return (MapWMSStatusToBREWCode(st));
}


/*===========================================================================
FUNCTION: OEMWMS_DcEnableAutoDisconnect,

DESCRIPTION
  Enables auto disconnect of a SO6/14 SMS call.  Disconnects after a specified
  time (timeout) in seconds.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_DcEnableAutoDisconnect
(
 IWMS                * pMe,
 wms_client_id_type    clientId,
 AEECallback         * cmd_cb,
 const void          * user_data,
 uint32                timeout
)
{
  wms_status_e_type st;
  st = wms_dc_enable_auto_disconnect( clientId,
                                      (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                      user_data,
                                      timeout );

  return (MapWMSStatusToBREWCode(st));
}


/*===========================================================================
FUNCTION: OEMWMS_DcDisableAutoDisconnect,

DESCRIPTION
  Disables the auto-disconnect feature.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_DcDisableAutoDisconnect
(
 IWMS                  * pMe,
 wms_client_id_type      clientId,
 AEECallback           * cmd_cb,
 const void            * user_data
)
{
  wms_status_e_type st;
  st = wms_dc_disable_auto_disconnect( clientId,
                                       (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                       user_data );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_DcConnect,

DESCRIPTION
  Connects a SO6/14/Auto SMS call determined by the specified service option.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_DcConnect
(
   IWMS                     *pMe,
   wms_client_id_type        clientId,
   AEECallback              *cmdCb,
   const void               *user_data,
   wms_dc_so_e_type          so
)
{
  wms_status_e_type   st;

  st = wms_dc_connect( clientId,
                       (wms_cmd_cb_type) cmdCb->pfnNotify,
                       user_data,
                       so );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_DcDisconnect,

DESCRIPTION
  Disconnects SMS call.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS

===========================================================================*/
static int OEMWMS_DcDisconnect
(
  IWMS                  *pMe,
  wms_client_id_type     clientId,
  AEECallback           *cmdCb,
  const void            *user_data
)
{
  wms_status_e_type   st;

  st = wms_dc_disconnect( clientId,
                          (wms_cmd_cb_type) cmdCb->pfnNotify,
                          user_data);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_DbgResetTLSeqNum
DESCRIPTION
  Enable/disable an individual broadcast SMS service.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_DbgResetTLSeqNum (
  IWMS                      *pMe,
  wms_client_id_type        clientId,
  AEECallback               *cmd_cb,
  const void                *user_data
)
{
  wms_status_e_type st;
  st = wms_dbg_reset_tl_seq_num( clientId,
                                 (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                 user_data );

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_DbgSetMsgRefNum
DESCRIPTION
  Set the Message Reference Number in SIM to 0. Applicable only to GW.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_DbgSetMsgRefNum (
  IWMS                      *pMe,
  wms_client_id_type        clientId,
  AEECallback               *cmd_cb,
  const void                *user_data
)
{
  wms_status_e_type st;
  st = wms_dbg_set_msg_ref( clientId,
                            (wms_cmd_cb_type) cmd_cb->pfnNotify,
                            user_data );

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_DbgClearSMMAFlag
DESCRIPTION
  Clears the SMMA Flag. Applicable to GW Only

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_DbgClearSMMAFlag (
  IWMS                      *pMe,
  wms_client_id_type        clientId,
  AEECallback               *cmd_cb,
  const void                *user_data
)
{
  wms_status_e_type st;
  st = wms_dbg_clear_smma_flag( clientId,
                                (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                user_data );

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_DbgGetRetryInterval
DESCRIPTION
  Get the Retry Interval. A Debug Event will be sent.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_DbgGetRetryInterval (
  IWMS                      *pMe,
  wms_client_id_type        clientId,
  AEECallback               *cmd_cb,
  const void                *user_data
)
{
  wms_status_e_type st;
  st = wms_dbg_get_retry_interval( clientId,
                                   (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                   user_data );

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_DbgSetRetryInterval
DESCRIPTION
  Set the Retry Interval. A Debug Event will be sent.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_DbgSetRetryInterval (
  IWMS                      *pMe,
  wms_client_id_type        clientId,
  AEECallback               *cmd_cb,
  const void                *user_data,
  uint32                     retry_interval
)
{
  wms_status_e_type st;
  st = wms_dbg_set_retry_interval( clientId,
                                   (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                   user_data,
                                   retry_interval );

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================
FUNCTION    OEMWMS_TsEncode
DESCRIPTION
  Encode Transport Service structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  EFAILED   - General Failure

SIDE EFFECTS
  None
===========================================================================*/
static int OEMWMS_TsEncode (
  IWMS                      *pMe,
  wms_client_ts_data_s_type *ClientTsDataPtr,
  wms_raw_ts_data_s_type    *RawTsDataPtr
)
{
  wms_status_e_type st;
  
  if ((ClientTsDataPtr == NULL)
      ||(RawTsDataPtr == NULL))
  {
    return EBADPARM;
  }

  st = wms_ts_encode(ClientTsDataPtr,
                     RawTsDataPtr);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================
FUNCTION    OEMWMS_TsDecode
DESCRIPTION
  Decode Transport Service structure from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  EFAILED   - General Failure

SIDE EFFECTS
  None
===========================================================================*/
static int OEMWMS_TsDecode (
  IWMS                      *pMe,
  wms_raw_ts_data_s_type    *RawTsDataPtr,
  wms_client_ts_data_s_type *ClientTsDataPtr
)
{
  wms_status_e_type st;
  
  if ((ClientTsDataPtr == NULL)
      ||(RawTsDataPtr == NULL))
  {
    return EBADPARM;
  }

  st = wms_ts_decode(RawTsDataPtr,
                     ClientTsDataPtr);

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION    OEMWMS_TsGetHeaderLength
DESCRIPTION
  Get the User Data Header Length

DEPENDENCIES
  None

RETURN VALUE
  User Data Header Lenght

SIDE EFFECTS
  None
===========================================================================*/
static uint8 OEMWMS_TsGetHeaderLength (
  IWMS                      *pMe,
  wms_udh_s_type            *udhPtr
)
{
#ifdef FEATURE_SMS_UDH
  if (udhPtr == NULL)
  {
    return 0;
  }
  return (wms_ts_get_udh_length(udhPtr));
#else /* FEATURE_SMS_UDH */
  return 0;
#endif /* FEATURE_SMS_UDH */
}

/* <EJECT> */
/*===========================================================================
FUNCTION    OEMWMS_TsBcdToAscii
DESCRIPTION
  Convert BCD digits to an ASCII number string.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  EFAILED   - General Failure

SIDE EFFECTS
  None
===========================================================================*/
static int OEMWMS_TsBcdToAscii (
  IWMS                      *pMe,
  uint8                     *InputPtr,
  uint8                      length,
  uint8                     *OutputPtr
)
{
  if ((InputPtr == NULL)
      ||(OutputPtr == NULL))
  {
    return EBADPARM;
  }

  wms_ts_bcd_to_ascii(InputPtr, length, OutputPtr);
  
  return SUCCESS;
}

/* <EJECT> */
/*===========================================================================
FUNCTION    OEMWMS_TsAsciiToBcd
DESCRIPTION
  Convert an ASCII number string to BCD digits.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  EFAILED   - General Failure

SIDE EFFECTS
  None
===========================================================================*/
static int OEMWMS_TsAsciiToBcd (
  IWMS                      *pMe,
  uint8                     *InputPtr,
  uint8                      length,
  uint8                     *OutputPtr
)
{
  if ((InputPtr == NULL)
      ||(OutputPtr == NULL))
  {
    return EBADPARM;
  }

  wms_ts_ascii_to_bcd(InputPtr, length, OutputPtr);
  
  return SUCCESS;
}

/*===========================================================================
FUNCTION    OEMWMS_TsAsciiToDefault
DESCRIPTION
  Convert an ASCII string to a GSM 7-bit Default alphabet string.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  EFAILED   - General Failure

SIDE EFFECTS
  None
===========================================================================*/
int OEMWMS_TsAsciiToDefault (
  IWMS                      *pMe,
  uint8                     *InputPtr,
  uint32                     length,
  uint8                     *OutputPtr
)
{
  if ((InputPtr == NULL)
      ||(OutputPtr == NULL))
  {
    return EBADPARM;
  }

  wms_ts_ascii_to_default(InputPtr, length, OutputPtr);
  
  return SUCCESS;
}

/*===========================================================================
FUNCTION    OEMWMS_TsDefaultToAscii
DESCRIPTION
  Convert 7-bit GSM Default alphabet string to an ASCII string.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  EFAILED   - General Failure

SIDE EFFECTS
  None
===========================================================================*/
static int OEMWMS_TsDefaultToAscii(
  IWMS                      *pMe,
  uint8                     *InputPtr,
  uint32                     length,
  uint8                     *OutputPtr
)
{
  if ((InputPtr == NULL)
      ||(OutputPtr == NULL))
  {
    return EBADPARM;
  }

  wms_ts_default_to_ascii(InputPtr, length, OutputPtr);
  
  return SUCCESS;
}

/*===========================================================================
FUNCTION    OEMWMS_TsDecodeRelativeTime
DESCRIPTION
  Decode the Relative TimeStamp

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  EFAILED   - General Failure

SIDE EFFECTS
  None
===========================================================================*/
static int OEMWMS_TsDecodeRelativeTime(
  IWMS                      *pMe,
  uint8                      nTime,
  wms_timestamp_s_type      *TimeStampPtr
)
{
  if (TimeStampPtr == NULL)
  {
    return EBADPARM;
  }

  wms_ts_decode_relative_time(nTime, TimeStampPtr);
  
  return SUCCESS;
}

/*===========================================================================
FUNCTION    OEMWMS_TsEncodeRelativeTime
DESCRIPTION
  Encode the Relative TimeStamp

DEPENDENCIES
  None

RETURN VALUE
  The encoded Timestamp

SIDE EFFECTS
  None
===========================================================================*/
static uint8 OEMWMS_TsEncodeRelativeTime(
  IWMS                      *pMe,
  wms_timestamp_s_type      *TimeStampPtr
)
{
  if (TimeStampPtr == NULL)
  {
    return 0;
  }

  return (wms_ts_encode_relative_time(TimeStampPtr));
}

/*===========================================================================
FUNCTION    OEMWMS_TsDecodeDCS
DESCRIPTION
  Decode the Data Coding Scheme (DCS)

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
===========================================================================*/
static uint8 OEMWMS_TsDecodeDCS(
  IWMS                      *pMe,
  uint8                     *data,
  wms_gw_dcs_s_type         *dcs
)
{
  if ((data == NULL)||(dcs == NULL))
  {
    return 0;
  }
  return (wms_ts_decode_dcs(data, dcs));
}

/*===========================================================================
FUNCTION    OEMWMS_TsEncodeDCS
DESCRIPTION
  Encode the Data Coding Scheme (DCS)

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
===========================================================================*/
static uint8 OEMWMS_TsEncodeDCS(
  IWMS                      *pMe,
  wms_gw_dcs_s_type         *dcs,
  uint8                     *data
)
{
  if ((data == NULL)||(dcs == NULL))
  {
    return 0;
  }
  return (wms_ts_encode_dcs(dcs, data));
}

/*===========================================================================
FUNCTION    OEMWMS_TsPackAscii
DESCRIPTION
  Pack the 7 bits in each character of a null-terminated ASCII string into a
  a byte array.

DEPENDENCIES
  None

RETURN VALUE
  Number of Bytes Packed

SIDE EFFECTS
  None
===========================================================================*/
static byte OEMWMS_TsPackAscii(
  IWMS                      *pMe,
  char                      *AsciiPtr,
  byte                      *DataPtr,
  byte                      *DataLengthPtr,
  byte                      *PaddingBitsPtr
)
{
#ifdef FEATURE_CDSMS
  if ((AsciiPtr == NULL)
      ||(DataPtr == NULL)
      ||(DataLengthPtr == NULL)
      ||(PaddingBitsPtr == NULL))
  {
    return 0;
  }
  return (wms_ts_pack_ascii(AsciiPtr, DataPtr, DataLengthPtr, PaddingBitsPtr));
#else
  return 0;
#endif /* FEATURE_CDSMS */
}

/*===========================================================================
FUNCTION    OEMWMS_TsUnpackAscii
DESCRIPTION
  Unpack the 7-bit ASCII string from the packed format in a byte array.

DEPENDENCIES
  None

RETURN VALUE
  Number of Bytes Unpacked

SIDE EFFECTS
  None
===========================================================================*/
static byte OEMWMS_TsUnpackAscii(
  IWMS                      *pMe,
  wms_cdma_user_data_s_type *UserDataPtr,
  byte                       length,
  byte                      *OutputPtr
)
{
#ifdef FEATURE_CDSMS
  if ((UserDataPtr == NULL)
      ||(OutputPtr == NULL))
  {
    return 0;
  }
  return (wms_ts_unpack_ascii(UserDataPtr, length, OutputPtr));
#else
  return 0;
#endif /* FEATURE_CDSMS */
}

/*===========================================================================
FUNCTION    OEMWMS_TsDtmfToAscii
DESCRIPTION
  Convert the DTMF digits to ASCII digits.

DEPENDENCIES
  None

RETURN VALUE
  Number of Digits Converted

SIDE EFFECTS
  None
===========================================================================*/
static byte OEMWMS_TsDtmfToAscii(
  IWMS                      *pMe,
  byte                       length,
  byte                      *InputPtr,
  byte                      *OutputPtr
)
{
#ifdef FEATURE_CDSMS
  if ((InputPtr == NULL)
      ||(OutputPtr == NULL))
  {
    return 0;
  }
  return (wms_ts_dtmf2ascii(length, InputPtr, OutputPtr));
#else
  return 0;
#endif /* FEATURE_CDSMS */
}

/*===========================================================================
FUNCTION    OEMWMS_TsAsciiToDtmf
DESCRIPTION
  Convert the DTMF digits to ASCII digits.

DEPENDENCIES
  None

RETURN VALUE
  Number of Digits Converted

SIDE EFFECTS
  None
===========================================================================*/
static byte OEMWMS_TsAsciiToDtmf(
  IWMS                      *pMe,
  char                      *InputPtr,
  byte                      *OutputPtr
)
{
#ifdef FEATURE_CDSMS
  if ((InputPtr == NULL)
      ||(OutputPtr == NULL))
  {
    return 0;
  }
  return (wms_ts_ascii2dtmf(InputPtr, OutputPtr));
#else
  return 0;
#endif /* FEATURE_CDSMS */
}

/*===========================================================================

FUNCTION    OEMWMS_CmdErrCb

DESCRIPTION
  Initialize the BC SMS configuration, preferences and service table.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
int OEMWMS_CmdErrCb(
  IWMS                     *pMe,
  wms_cmd_id_e_type        cmd,
  void                     *pUser,
  wms_cmd_err_e_type       status
)
{
  return EUNSUPPORTED;
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmGetConfig

DESCRIPTION
  This function is used to retrieve the Broadcast Configuration

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmGetConfig
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode
)
{
  wms_status_e_type st;
  st = wms_bc_mm_get_config (clientId,
                             (wms_cmd_cb_type) cmd_cb->pfnNotify,
                             user_data,
                             message_mode);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmGetPref

DESCRIPTION
 This function is used to get the Broadcast preference

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmGetPref
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode
)
{
  wms_status_e_type st;
  st = wms_bc_mm_get_pref (clientId,
                           (wms_cmd_cb_type) cmd_cb->pfnNotify,
                           user_data,
                           message_mode);

  return (MapWMSStatusToBREWCode(st));
}


/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmSetPref

DESCRIPTION
  This function is used to set the Broadcast preference 

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmSetPref
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode,
  wms_bc_pref_e_type             pref
)
{
  wms_status_e_type st;
  st = wms_bc_mm_set_pref (clientId,
                           (wms_cmd_cb_type) cmd_cb->pfnNotify,
                           user_data,
                           message_mode,
                           pref);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmGetTable

DESCRIPTION
  This function is used to get the Broadcast Service Table which includes the
  Service ID, Service Description, Selected, etc.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmGetTable
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode
)
{
  wms_status_e_type st;
  st = wms_bc_mm_get_table (clientId,
                            (wms_cmd_cb_type) cmd_cb->pfnNotify,
                            user_data,
                            message_mode);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmSelectService

DESCRIPTION
  This function is used to Select a particular Broadcast service

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmSelectService
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode,
  wms_bc_mm_srv_id_type         *srv_id_ptr,
  boolean                        selected
)
{
  wms_status_e_type st;
  st = wms_bc_mm_select_service (clientId,
                                 (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                 user_data,
                                 message_mode,
                                 srv_id_ptr,
                                 selected);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmGetAllServiceIds

DESCRIPTION
  This function returns all the service IDs of the Broadcast Services

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmGetAllServiceIds
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode
)
{
  wms_status_e_type st;
  st = wms_bc_mm_get_all_service_ids (clientId,
                                      (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                      user_data,
                                      message_mode);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmGetServiceInfo 

DESCRIPTION
  This function returns the Service Information for a particular Broadcast
  service given the service ID

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmGetServiceInfo
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode,
  wms_bc_mm_srv_id_type         *srv_id_ptr
)
{
  wms_status_e_type st;
  st = wms_bc_mm_get_service_info (clientId,
                                   (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                   user_data,
                                   message_mode,
                                   srv_id_ptr);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmAddServices

DESCRIPTION
  This function is used to add one or more services to the Broadcast Service
  Table given all the Service Information for each one of them

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmAddServices
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode,
  uint8                          num_entries,
  wms_bc_mm_service_info_s_type  *entries
)
{
  wms_status_e_type st;
  st = wms_bc_mm_add_services (clientId,
                               (wms_cmd_cb_type) cmd_cb->pfnNotify,
                               user_data,
                               message_mode,
                               num_entries,
                               entries);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmDeleteServices

DESCRIPTION
  This function is used to Delete one or more Services from the Broadcast
  Service Table given the service IDs for each one of them

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmDeleteServices
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode,
  uint8                          num_entries,
  wms_bc_mm_srv_id_type         *srv_ids
)
{
  wms_status_e_type st;
  st = wms_bc_mm_delete_services (clientId,
                                  (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                  user_data,
                                  message_mode,
                                  num_entries,
                                  srv_ids);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmChangeServiceInfo 

DESCRIPTION
  This function is used to chnage the service information for a particular 
  service in the Broadcast Service Table

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmChangeServiceInfo
( 
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode,
  wms_bc_mm_service_info_s_type  *srv_info_ptr
)
{
  wms_status_e_type st;
  st = wms_bc_mm_change_service_info (clientId,
                                      (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                      user_data,
                                      message_mode,
                                      srv_info_ptr);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmDeleteAllServices

DESCRIPTION
  This function is used to Delete all the Services from the Broadcast
  Service Table

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmDeleteAllServices
(  
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode
)
{
  wms_status_e_type st;
  st = wms_bc_mm_delete_all_services (clientId,
                                      (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                      user_data,
                                      message_mode);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmSelectAllServices

DESCRIPTION
  This function is used to Select all the Services from the Broadcast
  Service Table 

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmSelectAllServices
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode,
  boolean                        selected
)
{
  wms_status_e_type st;
  st = wms_bc_mm_select_all_services (clientId,
                                      (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                      user_data,
                                      message_mode,
                                      selected);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmSetPriorityAllServices

DESCRIPTION
  This function is used to Set the Priority for all the Services from the Broadcast
  Service Table 

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmSetPriorityAllServices
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode,
  wms_priority_e_type            priority
)
{
  wms_status_e_type st;
  st = wms_bc_mm_set_priority_for_all_services (clientId,
                                      (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                      user_data,
                                      message_mode,
                                      priority);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmMsgDeleteIndication

DESCRIPTION
  This function is used to indicate the Deletion of a Cell Broadcast
  Message from the UI

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmMsgDeleteIndication
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_gw_cb_page_header_s_type   *cb_page_header
)
{
  wms_status_e_type st;
  st = wms_bc_mm_msg_delete_indication( clientId,
                                        (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                        user_data,
                                        cb_page_header);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmMsgDeleteAllIndication

DESCRIPTION
  This function is used to indicate the Deletion of ALL Cell Broadcast
  Message from the UI

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmMsgDeleteAllIndication
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data
)
{
  wms_status_e_type st;
  st = wms_bc_mm_msg_delete_all_indication( clientId,
                                            (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                            user_data);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmGetReadingPref

DESCRIPTION
  This function is used to Get the Broadcast Reading preferences
  - Advised and Optional.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmGetReadingPref
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode
)
{
  wms_status_e_type st;
  st = wms_bc_mm_get_reading_pref( clientId,
                                   (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                   user_data,
                                   message_mode);

  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcMmSetReadingPref

DESCRIPTION
  This function is used to Set the Broadcast Reading preferences
  - Advised and Optional.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcMmSetReadingPref
(
  IWMS                           *pMe,
  wms_client_id_type             clientId,
  AEECallback                    *cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type        message_mode,
  boolean                        reading_advised,
  boolean                        reading_optional
)
{
  wms_status_e_type st;
  st = wms_bc_mm_set_reading_pref( clientId,
                                   (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                   user_data,
                                   message_mode,
                                   reading_advised,
                                   reading_optional);

  return (MapWMSStatusToBREWCode(st));
}

#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION OEMWMS_IsNonStdEmail

DESCRIPTION
  Checks whether the address is using the non-stanard e-mail address format.
  Returns TRUE if so.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean OEMWMS_IsNonStdEmail
(
  boolean             is_dtmf,
  const uint8         *n_buf,
  const uint8         n_length
)
{
  int i;
  /* 4-bit address to detect a non-std email address */
  byte num_to_compare[5] = "6245";

  /* Temporary ascii variable to hold converted dtmf number */
  uint8 ascii_num = '0';

  /* Check the length first for a possible match */
  if(n_length != 4)
  {
    return FALSE;
  }

  for(i=0; i<4; i++)
  {
    if(is_dtmf)
    {
      /* Convert to ascii */
      /* No need to convert other possible dtmf values ie. 0,# because if
      ** they are contained in the address then it will obviously not be
      ** non-std email
      */
      ascii_num = n_buf[i] + 0x30;
    }
    else
    {
      ascii_num = n_buf[i];
    }

    if(ascii_num == num_to_compare[i])
    {
      /* Good so far */
      continue;
    }
    return FALSE;
  }

  /* This is using a non-std email address format */
  return TRUE;
} /* OEMWMS_IsNonStdEmail() */

/*===========================================================================
FUNCTION OEMWMS_GetNonStdEmail

DESCRIPTION
  Gets the value of the non standard e-mail address. Returns length.

DEPENDENCIES
  None

SIDE EFFECTS
  Changes the value of email_addr directly.
===========================================================================*/
static uint8 OEMWMS_GetNonStdEmail
(
  const uint8 *user_data,
  uint8 *email_addr
)
{
  uint8 i;

  for(i=0; i<(uint8)(STRLEN((char*)user_data)); i++)
  {
    /* Check if we've reached the end of the e-mail address */
    if(user_data[i] != ' ')
    {
      /* Copy address */
      email_addr[i] = user_data[i];
      continue;
    }
    break;
  }
  return i;
} /* OEMWMS_GetNonStdEmail() */

/*===========================================================================
FUNCTION OEMWMS_ConvertFromUnicode

DESCRIPTION
  Converts the user data from unicode.

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies user data and data length.
===========================================================================*/
static void OEMWMS_ConvertFromUnicode
(
  wms_client_bd_s_type *bd_data
)
{
  uint8 i;

  if (bd_data != NULL)
  {
    /* Decrease the data length by half */
    bd_data->user_data.data_len = bd_data->user_data.data_len/2;

    /* Extract the high byte from the user data.  This will be equivalent to
    ** 8-bit Ascii */
    for( i=0; i<bd_data->user_data.data_len; i++ )
    {
      bd_data->user_data.data[i] = bd_data->user_data.data[i*2+1];
    }
  }

} /* OEMWMS_ConvertFromUnicode() */

#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION OEMWMS_ITOA

DESCRIPTION
  Replicating the Integer to Ascii functionality

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 OEMWMS_ITOA(int number, char *string, int base)
{
  if (base == 10)
  {
    (void)SPRINTF(string, "%d", number);
  }
  else if (base == 16)
  {
    (void)SPRINTF(string, "%x", number);
  }
  else if (base == 8)
  {
    (void)SPRINTF(string, "%o", number);
  }
  return ((uint8)(STRLEN(string)));
}

#ifdef FEATURE_SMS_UDH
/*===========================================================================
FUNCTION OEMWMS_ConvertHexToAscii

DESCRIPTION
  Conversion Function from Hexadecimal to Ascii

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static byte OEMWMS_ConvertHexToAscii
(
  byte num   /* Hexadecimal Number to convert */
)
{
  if (num <= 9)
    return (num + '0');
  else
    return ((num-10) + 'A');
}
#endif /* FEATURE_SMS_UDH */


/* <EJECT> */
/*===========================================================================
FUNCTION OEMWMS_MsgInfoCacheCb

DESCRIPTION
  Callback function for Message Info Cache. The cached information is used to
  display the message lists

DEPENDENCIES
  None

SIDE EFFECTS
  pCache is updated

===========================================================================*/
void OEMWMS_MsgInfoCacheCb
(
  const wms_client_message_s_type     *pMsg,
  uint8                               *pCache
)
{
  wms_client_ts_data_s_type   *cl_data;
  uint8                       *buf;
  int                         offset = 0;
  int                         concatInfoLength = 0;
  char                        concatInfo[16];   /* Longest Case: 255/255(0xFFFF) */

  if (pMsg == NULL || pCache == NULL)
  {
    MSG_ERROR("Null Pointers in OEMWMS_MsgInfoCacheCb Parms",0,0,0);
    return;
  }

  cl_data = mem_malloc(&tmc_heap, sizeof(wms_client_ts_data_s_type));
  if (cl_data == NULL)
  {
    MSG_ERROR("memory allocation failed for cl_data in OEMWMS_MsgInfoCacheCb",0,0,0);
    return;
  }
  (void)MEMSET( cl_data, 0, sizeof(wms_client_ts_data_s_type) );

  buf = mem_malloc(&tmc_heap,WMS_MSG_INFO_CACHE_SIZE);
  if (buf == NULL)
  {
    MSG_ERROR("memory allocation failed for buf in OEMWMS_MsgInfoCacheCb",0,0,0);
    mem_free(&tmc_heap,cl_data);
    return;
  }

  (void)MEMSET(concatInfo, 0, 16);

  /* Clear the pCache of any garbage */
  (void)MEMSET(pCache, ' ', WMS_MSG_INFO_CACHE_SIZE);
  
#ifdef FEATURE_CDSMS
  /* Updating the cache:
  ** (1) If user data exists, copy the user data into the info cache.
  ** (2) If no user data, copy the address field into the info cache.
  ** Note: the first byte in the pCache will be filled with the size
  ** of the data.
  */


  /* First find out if this is for a CDMA message or a template */
  if((pMsg->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA) &&
     (pMsg->msg_hdr.tag == WMS_TAG_MO_TEMPLATE))
  {
    wms_client_bd_s_type *info_data;
    oemwms_email_address_s_type *emailAddress;

    info_data=mem_malloc(&tmc_heap,sizeof(wms_client_bd_s_type));
    if (info_data == NULL)
    {
      MSG_ERROR("memory allocation failed for info_data in OEMWMS_MsgInfoCacheCb",0,0,0);
      mem_free(&tmc_heap,cl_data);
      mem_free(&tmc_heap,buf);
      return;
    }

    emailAddress= mem_malloc(&tmc_heap,sizeof(oemwms_email_address_s_type));

    if (emailAddress == NULL)
    {
      MSG_ERROR("memory allocation failed for email_address in OEMWMS_MsgInfoCacheCb",0,0,0);
      mem_free(&tmc_heap,cl_data);
      mem_free(&tmc_heap,buf);
      mem_free(&tmc_heap,info_data);
      return;
    }

    if((pMsg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
       (pMsg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA))
    {
      /* copy the user data into the info cache */
      if((pMsg->u.cdma_template.client_bd.user_data.encoding ==
          WMS_ENCODING_ASCII) ||
         (pMsg->u.cdma_template.client_bd.user_data.encoding ==
          WMS_ENCODING_IA5))
      {
        info_data->user_data.data_len =
        wms_ts_unpack_ascii((wms_cdma_user_data_s_type*)&pMsg->u.cdma_template.client_bd.user_data,
                            pMsg->u.cdma_template.client_bd.user_data.number_of_digits+1,
                            info_data->user_data.data);

        /* User data field will contain email address if non-std email format
        ** is being used.  Extract it and get the real chari's
        */
        if((pMsg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR) &&
           (OEMWMS_IsNonStdEmail(TRUE, pMsg->u.cdma_template.address.digits,
                                  pMsg->u.cdma_template.address.number_of_digits)))
        {
          emailAddress->len =
          OEMWMS_GetNonStdEmail(info_data->user_data.data,
                                 emailAddress->address);

          if(pMsg->u.cdma_template.client_bd.user_data.data_len <=
             emailAddress->len+1)
          {
            /* No other user data, copy the email address to the info cache */
            pCache[0] =
            MIN(WMS_MSG_INFO_CACHE_SIZE-1, emailAddress->len);
            offset++;
            (void)MEMCPY(pCache+offset, emailAddress->address,
                         MIN((uint8)(WMS_MSG_INFO_CACHE_SIZE-offset), emailAddress->len));
          }
          else
          {
            /* Copy the text into the cache */
            pCache[0] =
            MIN(WMS_MSG_INFO_CACHE_SIZE-1,
                info_data->user_data.data_len - emailAddress->len - 1);
            offset++;

            /* Check to see if this message is urgent */
            if((pMsg->u.cdma_template.client_bd.mask & WMS_MASK_BD_PRIORITY) &&
               ((pMsg->u.cdma_template.client_bd.priority == WMS_PRIORITY_URGENT) ||
                (pMsg->u.cdma_template.client_bd.priority == WMS_PRIORITY_EMERGENCY)))
            {
              pCache[1] = '*';
              offset++;
            }

            (void)MEMCPY(pCache+offset,
                   info_data->user_data.data+emailAddress->len+1,
                   MIN(WMS_MSG_INFO_CACHE_SIZE-offset,
                       info_data->user_data.data_len-emailAddress->len-1));
          }
        }
        else
        {
          pCache[0] =
          MIN(WMS_MSG_INFO_CACHE_SIZE-1, info_data->user_data.data_len);
          offset++;
          if((pMsg->u.cdma_template.client_bd.mask & WMS_MASK_BD_PRIORITY) &&
             ((pMsg->u.cdma_template.client_bd.priority == WMS_PRIORITY_URGENT) ||
              (pMsg->u.cdma_template.client_bd.priority == WMS_PRIORITY_EMERGENCY)))
          {
            pCache[1] = '*';
            offset++;
          }

          (void)MEMCPY(pCache+offset, info_data->user_data.data,
                 MIN((uint8)(WMS_MSG_INFO_CACHE_SIZE-offset), info_data->user_data.data_len));
        }
      }
      else /* just copy the user data for other encodings */
      {
        pCache[0] =
        MIN(WMS_MSG_INFO_CACHE_SIZE-1,
            pMsg->u.cdma_template.client_bd.user_data.data_len);
        offset++;
        if((pMsg->u.cdma_template.client_bd.mask & WMS_MASK_BD_PRIORITY) &&
           ((pMsg->u.cdma_template.client_bd.priority == WMS_PRIORITY_URGENT)||
            (pMsg->u.cdma_template.client_bd.priority == WMS_PRIORITY_EMERGENCY)))
        {
          pCache[1] = '*';
          offset++;
        }
        (void)MEMCPY(pCache+1, pMsg->u.cdma_template.client_bd.user_data.data,
               MIN((uint8)(WMS_MSG_INFO_CACHE_SIZE-offset),
                   pMsg->u.cdma_template.client_bd.user_data.data_len));
      }
    }
    else if(pMsg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
    {
      if(pMsg->u.cdma_template.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
      {
        (void)wms_ts_dtmf2ascii((byte)pMsg->u.cdma_template.address.number_of_digits,
                                (byte*)pMsg->u.cdma_template.address.digits,
                                buf);
      }
      else
      {
        (void)MEMCPY(buf,
                pMsg->u.cdma_template.address.digits,
                MIN(WMS_MSG_INFO_CACHE_SIZE-1,
                     pMsg->u.cdma_template.address.number_of_digits));
      }
      pCache[0] =
      MIN(WMS_MSG_INFO_CACHE_SIZE-1,
          pMsg->u.cdma_template.address.number_of_digits);
      offset++;

      (void)MEMCPY(pCache+offset, buf,
             MIN((uint8)(WMS_MSG_INFO_CACHE_SIZE-offset),
                 pMsg->u.cdma_template.address.number_of_digits));
    }
    else
    {
      /* Nothing to put into cache */
      pCache[0] = 14;
      (void)MEMCPY(pCache+1, "Empty Template", 14);
    }

    mem_free(&tmc_heap,info_data);
    mem_free(&tmc_heap,emailAddress);
  }
  /* Cache is for a CDMA message */
  else if(pMsg->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_SMS_UDH
    uint8 i=0;
#endif /* FEATURE_SMS_UDH */
    wms_client_bd_s_type *info_data;
    oemwms_email_address_s_type *emailAddress;

    info_data=mem_malloc(&tmc_heap,sizeof(wms_client_bd_s_type));
    if (info_data == NULL)
    {
      MSG_ERROR("memory allocation failed for info_data in OEMWMS_MsgInfoCacheCb",0,0,0);
      mem_free(&tmc_heap,cl_data);
      mem_free(&tmc_heap,buf);
      return;
    }

    emailAddress= mem_malloc(&tmc_heap,sizeof(oemwms_email_address_s_type));
    if (emailAddress == NULL)
    {
      MSG_ERROR("memory allocation failed for email_address in OEMWMS_MsgInfoCacheCb",0,0,0);
      mem_free(&tmc_heap,cl_data);
      mem_free(&tmc_heap,buf);
      mem_free(&tmc_heap,info_data);
      return;
    }

    /* Initialized other to 0 */
    (void)MEMSET( &cl_data->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

    /* decode the raw bearer data */
    if(wms_ts_decode(&pMsg->u.cdma_message.raw_ts, cl_data) != WMS_OK_S)
    {
      MSG_ERROR("Decoding failed for msg list cache!",0,0,0);
      pCache[offset++] = 8;
      (void)MEMCPY(pCache+offset, "No Info", 8);
      mem_free(&tmc_heap,cl_data);
      mem_free(&tmc_heap,buf);
      mem_free(&tmc_heap,info_data);
      mem_free(&tmc_heap,emailAddress);
      return;
    }
    
#ifdef FEATURE_SMS_UDH
    /* Concat Header */
    for (i=0; i<cl_data->u.cdma.user_data.num_headers; i++)
    {
      if (cl_data->u.cdma.user_data.headers[i].header_id == WMS_UDH_CONCAT_8)
      {
        concatInfoLength += OEMWMS_ITOA(cl_data->u.cdma.user_data.headers[i].u.concat_8.seq_num, concatInfo+concatInfoLength, 10);
        concatInfo[concatInfoLength++] = '/';
        concatInfoLength += OEMWMS_ITOA(cl_data->u.cdma.user_data.headers[i].u.concat_8.total_sm, concatInfo+concatInfoLength, 10);
        concatInfo[concatInfoLength++] = '(';
        concatInfo[concatInfoLength++] = '0';
        concatInfo[concatInfoLength++] = 'x';
        concatInfo[concatInfoLength++] = OEMWMS_ConvertHexToAscii((byte)((cl_data->u.cdma.user_data.headers[i].u.concat_8.msg_ref & 0xF0) >> 4));
        concatInfo[concatInfoLength++] = OEMWMS_ConvertHexToAscii((byte)(cl_data->u.cdma.user_data.headers[i].u.concat_8.msg_ref & 0x0F));
        concatInfo[concatInfoLength++] = ')';
        /* Null Terminate */
        concatInfo[concatInfoLength] = 0;
        break;
      }
      else if(cl_data->u.cdma.user_data.headers[i].header_id == WMS_UDH_CONCAT_16)
      {
        concatInfoLength += OEMWMS_ITOA(cl_data->u.cdma.user_data.headers[i].u.concat_16.seq_num, concatInfo+concatInfoLength, 10);
        concatInfo[concatInfoLength++] = '/';
        concatInfoLength += OEMWMS_ITOA(cl_data->u.cdma.user_data.headers[i].u.concat_16.total_sm, concatInfo+concatInfoLength, 10);
        concatInfo[concatInfoLength++] = '(';
        concatInfo[concatInfoLength++] = '0';
        concatInfo[concatInfoLength++] = 'x';
        concatInfo[concatInfoLength++] = OEMWMS_ConvertHexToAscii((byte)((cl_data->u.cdma.user_data.headers[i].u.concat_16.msg_ref & 0xF000) >> 12));
        concatInfo[concatInfoLength++] = OEMWMS_ConvertHexToAscii((byte)((cl_data->u.cdma.user_data.headers[i].u.concat_16.msg_ref & 0x0F00) >> 8));
        concatInfo[concatInfoLength++] = OEMWMS_ConvertHexToAscii((byte)((cl_data->u.cdma.user_data.headers[i].u.concat_16.msg_ref & 0x00F0) >> 4));
        concatInfo[concatInfoLength++] = OEMWMS_ConvertHexToAscii((byte)(cl_data->u.cdma.user_data.headers[i].u.concat_16.msg_ref & 0x000F));
        concatInfo[concatInfoLength++] = ')';
        /* Null Terminate */
        concatInfo[concatInfoLength] = 0;
        break;
      }
    }
#endif /* FEATURE_SMS_UDH */
    /* Check if this is a voicemail message */
    if(cl_data->u.cdma.mask & WMS_MASK_BD_NUM_OF_MSGS)
    {
      (void)MEMCPY(pCache, VOICEMAIL_ID, VOICEMAIL_ID_LEN);
    }
    /* Check for user data */
    else if(cl_data->u.cdma.mask & WMS_MASK_BD_USER_DATA)
    {
      /* copy the user data into the info cache */
      if((cl_data->u.cdma.user_data.encoding == WMS_ENCODING_ASCII) ||
         (cl_data->u.cdma.user_data.encoding == WMS_ENCODING_IA5))
      {
        info_data->user_data.data_len =
        wms_ts_unpack_ascii((wms_cdma_user_data_s_type*)&cl_data->u.cdma.user_data,
                            cl_data->u.cdma.user_data.number_of_digits+1,
                            info_data->user_data.data);

        /* User data field will contain email address if non-std email format
        ** is being used.  Extract it and get the real chari's
        */
        if(OEMWMS_IsNonStdEmail(TRUE, pMsg->u.cdma_message.address.digits,
                                 pMsg->u.cdma_message.address.number_of_digits))
        {
          emailAddress->len =
          OEMWMS_GetNonStdEmail(info_data->user_data.data,
                                 emailAddress->address);

          if(cl_data->u.cdma.user_data.data_len <= emailAddress->len+1)
          {
            /* No other user data, copy the email address to the info cache */
            pCache[0] =
            MIN(WMS_MSG_INFO_CACHE_SIZE-1, emailAddress->len);
            offset++;
            (void)MEMCPY(pCache+offset, emailAddress->address,
                   MIN((uint8)(WMS_MSG_INFO_CACHE_SIZE-offset), emailAddress->len));
          }
          else
          {
            /* Copy the text into the cache */
            pCache[0] =
            MIN(WMS_MSG_INFO_CACHE_SIZE-1,
                info_data->user_data.data_len-emailAddress->len-1);
            offset++;

            if((cl_data->u.cdma.mask & WMS_MASK_BD_PRIORITY) &&
               ((cl_data->u.cdma.priority == WMS_PRIORITY_URGENT) ||
                (cl_data->u.cdma.priority == WMS_PRIORITY_EMERGENCY)))
            {
              pCache[1] = '*';
              offset++;
            }

            (void)MEMCPY(pCache+offset, info_data->user_data.data+emailAddress->len+1,
                   MIN(WMS_MSG_INFO_CACHE_SIZE-offset,
                       info_data->user_data.data_len-emailAddress->len-1));
          }
        }
        else
        {
          pCache[0] =
          MIN(WMS_MSG_INFO_CACHE_SIZE-1, info_data->user_data.data_len);
          offset++;
          if((cl_data->u.cdma.mask & WMS_MASK_BD_PRIORITY) &&
             ((cl_data->u.cdma.priority == WMS_PRIORITY_URGENT) ||
              (cl_data->u.cdma.priority == WMS_PRIORITY_EMERGENCY)))
          {
            pCache[1] = '*';
            offset++;
          }
          if(pMsg->u.cdma_message.teleservice == WMS_TELESERVICE_BROADCAST)
          {
            /* set pCache[0] to max cache size +1 to signify broadcast */
            pCache[0] = WMS_MSG_INFO_CACHE_SIZE + 1;
          }

          (void)MEMCPY(pCache+offset, info_data->user_data.data,
                 MIN((uint8)(WMS_MSG_INFO_CACHE_SIZE-offset), info_data->user_data.data_len));
        }
      }
      else /* just copy the user data for other encoding types */
      {
        if(cl_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE)
        {
          OEMWMS_ConvertFromUnicode(&cl_data->u.cdma);
        }
        pCache[0] =
        MIN(WMS_MSG_INFO_CACHE_SIZE-1, cl_data->u.cdma.user_data.data_len);
        offset++;
        if((cl_data->u.cdma.mask & WMS_MASK_BD_PRIORITY) &&
           ((cl_data->u.cdma.priority == WMS_PRIORITY_URGENT) ||
            (cl_data->u.cdma.priority == WMS_PRIORITY_EMERGENCY)))
        {
          pCache[1] = '*';
          offset++;
        }
        (void)MEMCPY(pCache+offset, cl_data->u.cdma.user_data.data,
               MIN((uint8)(WMS_MSG_INFO_CACHE_SIZE-offset), cl_data->u.cdma.user_data.data_len));

      }
    }
    else
    {
      if(pMsg->u.cdma_message.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
      {
        (void)wms_ts_dtmf2ascii((byte)pMsg->u.cdma_message.address.number_of_digits,
                                (byte*)pMsg->u.cdma_message.address.digits,
                                buf);
      }
      else
      {
        (void)MEMCPY(buf,
               pMsg->u.cdma_message.address.digits,
               MIN(WMS_MSG_INFO_CACHE_SIZE, pMsg->u.cdma_message.address.number_of_digits));
      }
      pCache[0] =
      MIN(WMS_MSG_INFO_CACHE_SIZE-1,
          pMsg->u.cdma_message.address.number_of_digits);
      offset++;
      (void)MEMCPY(pCache+offset, buf,
             MIN((uint8)(WMS_MSG_INFO_CACHE_SIZE-offset),
                 pMsg->u.cdma_message.address.number_of_digits));
    }
    mem_free(&tmc_heap,info_data);
    mem_free(&tmc_heap,emailAddress);
  }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS_CACHE
#error code not present
#endif /*FEATURE_GWSMS_CACHE*/

  /* Appending ConcatInfo if present */
  if (concatInfoLength > 0)
  {
    uint8 i=0;
    /* Shift the existing Cache contents */
    for (i=pCache[0]; i>0; i--)
    {
      if (i+concatInfoLength < WMS_MSG_INFO_CACHE_SIZE)
      {
        pCache[i+concatInfoLength] = pCache[i];
      }
    }
    (void)MEMCPY(pCache+1, concatInfo, (uint32)concatInfoLength);
    pCache[0] = MIN(WMS_MSG_INFO_CACHE_SIZE-1, pCache[0]+(uint8)concatInfoLength);
  }
  mem_free(&tmc_heap,cl_data);
  mem_free(&tmc_heap,buf);
}




/*=============================================================================
FILE:  OEMWMS.c

SERVICES:  OEM functions for supporting BREW WMSAPP Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW IWMS interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright ? 1999,2000,2001,2002,2003,2004,2005,2006,2007,2008
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
#include "OEMFeatures.h"
#include "err.h"        // Error functions

#include "AEE.h"         // BREW headers.
#include "AEE_OEM.h"     // Brew OEM Headers.
#include "AEEWMS.h"      // Header file for WMS interface
#include "OEMWMS.h"      // Header file for new WMS interface
#include "OEMHeap.h"
#include "tmc.h"
#include "Appscommon.h"

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

static int OEMWMS_RegisterBcCb(       IWMS                   *pMe,
                                      wms_client_id_type     clientId,
                                      wms_bc_event_cb_type   bcCb);

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

static int OEMWMS_SetMultiSend(       IWMS                     *po,
                                      wms_client_id_type        client_id,
                                      AEECallback               *cmd_cb,
                                      const void                *user_data,
                                      boolean                   multisend);
                                      
#ifdef FEATURE_AUTOREPLACE
static int OEMWMS_SetAutoreplace(     IWMS                     *po,
                                      wms_client_id_type        client_id,
                                      AEECallback               *cmd_cb,
                                      const void                *user_data,
                                      boolean                   autoreplace);
#endif

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

#if 0
static boolean OEMWMS_CfgCheckVoiceMail( IWMS                         *pMe,
                                         wms_message_mode_e_type      msgMode,
                                         const wms_raw_ts_data_s_type *rawTs,
                                         boolean                      *isActive,
                                         uint8                        *count);
#else
static boolean OEMWMS_CfgCheckVoiceMail( IWMS                         *pMe,
                                         wms_message_mode_e_type      msgMode,
                                         const wms_client_message_s_type *pCltMsg,
                                         boolean                      *isActive,
                                         uint8                        *count);
#endif

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

static int OEMWMS_MsgDeleteBox(IWMS     *pMe,
                               wms_client_id_type       clientId,
                               AEECallback              *cmdCb,
                               const void               *user_data,
                               wms_deletebox_e_type     box_deltype);
                               
static int OEMWMS_MsgCopy(IWMS                     *pMe,
                          wms_client_id_type       clientId,
                          AEECallback              *cmdCb,
                          const void               *user_data,
                          wms_msg_copy_type        *pmsg_copy);
                          
#if defined(FEATURE_UIM_TOOLKIT)
static int OEMWMS_RefreshRUIMSMS(IWMS                     *pMe,
                                 wms_client_id_type       clientId,
                                 AEECallback              *cmdCb,
                                 const void               *user_data);
#endif                          

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

static int OEMWMS_MsgSetAutoCDMAParams (IWMS                     *pMe,
                                        wms_client_id_type        clientId,
                                        wms_cdma_template_s_type *cdma_params);

static int OEMWMS_MsgSetAutoGWParams   (IWMS                     *pMe,
                                        wms_client_id_type        clientId,
                                        wms_gw_template_s_type   *gw_params);

static int OEMWMS_MsgGetAutoCDMAParams (IWMS                     *pMe,
                                        wms_client_id_type        clientId,
                                        wms_cdma_template_s_type *cdma_params);

static int OEMWMS_MsgGetAutoGWParams   (IWMS                     *pMe,
                                        wms_client_id_type        clientId,
                                        wms_gw_template_s_type   *gw_params);

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


static int OEMWMS_TsConvertCdmaGwMessageToAuto ( IWMS                       * pMe,
                                                 wms_client_message_s_type  * InputPtr,
                                                 uint8                        length,
                                                 wms_auto_message_s_type    * OutputPtr);
                                
  
static int OEMWMS_TsConvertAutoToCdmaGwMessage ( IWMS                       * pMe,
                                                 wms_client_id_type           clientId,
                                                 wms_message_mode_e_type      MsgMode,
                                                 wms_auto_message_s_type    * InputPtr,
                                                 wms_client_message_s_type  * OutputPtr);
                      
static int OEMWMS_BcGetConfig(             IWMS                  *pMe,
                                           wms_client_id_type    clientId,
                                           AEECallback           *cmd_cb,
                                           const void            *user_data);


static int OEMWMS_BcGetPref(               IWMS                  *pMe,
                                           wms_client_id_type    clientId,
                                           AEECallback           *cmd_cb,
                                           const void            *user_data);


static int OEMWMS_BcSetPref(               IWMS                  *pMe,
                                           wms_client_id_type    clientId,
                                           AEECallback           *cmd_cb,
                                           const void            *user_data,
                                           wms_bc_pref_e_type    pref);

static int OEMWMS_BcGetTable(              IWMS                  *pMe,
                                           wms_client_id_type    clientId,
                                           AEECallback           *cmd_cb,
                                           const void            *user_data);

static int OEMWMS_BcSelectService(         IWMS                    *pMe,
                                           wms_client_id_type      clientId,
                                           AEECallback              *cmd_cb,
                                           const void               *user_data,
                                           wms_bc_service_id_s_type *srvcId,
                                           boolean                   selected,
                                           wms_priority_e_type       priority);

static int OEMWMS_BcGetAllServiceIds(      IWMS                  *pMe,
                                           wms_client_id_type    clientId,
                                           AEECallback           *cmd_cb,
                                           const void            *user_data);

static int OEMWMS_BcGetServiceInfo(        IWMS                     *pMe,
                                           wms_client_id_type       clientId,
                                           AEECallback              *cmd_cb,
                                           const void               *user_data,
                                           wms_bc_service_id_s_type *srvcID);

static int OEMWMS_BcAddService(            IWMS                        *pMe,
                                           wms_client_id_type          clientId,
                                           AEECallback                 *cmd_cb,
                                           const void                  *user_data,
                                           wms_bc_service_info_s_type  *srvcInfo);

static int OEMWMS_BcDeleteService(         IWMS                      *pMe,
                                           wms_client_id_type        clientId,
                                           AEECallback               *cmd_cb,
                                           const void                *user_data,
                                           wms_bc_service_id_s_type  *srvcID);


static int OEMWMS_BcChangeLabel(           IWMS                      *pMe,
                                           wms_client_id_type        clientId,
                                           AEECallback               *cmd_cb,
                                           const void                *user_data,
                                           wms_bc_service_id_s_type  *srvcID,
                                           char                      *label);

static int OEMWMS_BcDeleteAllServices(     IWMS                      *pMe,
                                          wms_client_id_type    clientId,
                                          AEECallback           *cmd_cb,
                                          const void            *user_data);
static int OEMWMS_BcSetPriorityForAll (
                                           IWMS                     *pMe,
                                           wms_client_id_type       clientId,
                                           AEECallback              *cmd_cb,
                                           const void               *user_data,
                                           wms_priority_e_type      priority);

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
   ,OEMWMS_RegisterBcCb
   ,OEMWMS_RegisterBcMmCb
   ,OEMWMS_RegisterDcCb
   ,OEMWMS_RegisterParsingCb
   ,OEMWMS_SetPrimaryClient
   ,OEMWMS_SetMemoryFull
   ,OEMWMS_SetMultiSend
#ifdef FEATURE_AUTOREPLACE
   ,OEMWMS_SetAutoreplace
#endif   
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
   ,OEMWMS_MsgDeleteBox
   ,OEMWMS_MsgCopy
#if defined(FEATURE_UIM_TOOLKIT)
   ,OEMWMS_RefreshRUIMSMS
#endif
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
   ,OEMWMS_MsgSetAutoCDMAParams
   ,OEMWMS_MsgSetAutoGWParams
   ,OEMWMS_MsgGetAutoCDMAParams
   ,OEMWMS_MsgGetAutoGWParams
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
   ,OEMWMS_TsConvertCdmaGwMessageToAuto
   ,OEMWMS_TsConvertAutoToCdmaGwMessage
   ,OEMWMS_BcGetConfig
   ,OEMWMS_BcGetPref
   ,OEMWMS_BcSetPref
   ,OEMWMS_BcGetTable
   ,OEMWMS_BcSelectService
   ,OEMWMS_BcGetAllServiceIds
   ,OEMWMS_BcGetServiceInfo
   ,OEMWMS_BcAddService
   ,OEMWMS_BcDeleteService
   ,OEMWMS_BcChangeLabel
   ,OEMWMS_BcDeleteAllServices
   ,OEMWMS_BcSetPriorityForAll
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

FUNCTION: OEMWMS_RegisterBcCb

DESCRIPTION
  Registers broadcast sms callback to WMS.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMWMS_RegisterBcCb
(
  IWMS                  *pMe,
  wms_client_id_type    clientId,
  wms_bc_event_cb_type  bcCb
)
{
#ifdef FEATURE_BROADCAST_SMS
  wms_client_err_e_type client_err;

  client_err = wms_client_reg_bc_cb( clientId, bcCb );

  if( client_err != WMS_CLIENT_ERR_NONE )
    return EFAILED;
  else
    return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_BROADCAST_SMS */

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

FUNCTION: OEMWMS_SetMultiSend

DESCRIPTION
  Sets the Primary Client's multisend Status

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Success
  EFAILED   - Failure

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_SetMultiSend
(
  IWMS                     *pMe,
  wms_client_id_type        client_id,
  AEECallback              *cmd_cb,
  const void               *user_data,
  boolean                   multisend
)
{
  wms_status_e_type   st;


  st = wms_cfg_set_multisend(client_id, (wms_cmd_cb_type)cmd_cb->pfnNotify, user_data, multisend);

  return (MapWMSStatusToBREWCode(st));
}

#ifdef FEATURE_AUTOREPLACE
/*===========================================================================

FUNCTION: OEMWMS_SetAutoreplace

DESCRIPTION
  Sets the Primary Client's autoreplace Status

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Success
  EFAILED   - Failure

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_SetAutoreplace
(
  IWMS                     *pMe,
  wms_client_id_type        client_id,
  AEECallback              *cmd_cb,
  const void               *user_data,
  boolean                   autoreplace
)
{
  wms_status_e_type   st;

  st = wms_cfg_set_autoreplace(client_id, (wms_cmd_cb_type)cmd_cb->pfnNotify, user_data, autoreplace);
  
  return (MapWMSStatusToBREWCode(st));
}
#endif /* FEATURE_AUTOREPLACE */

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
#if 0
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
#else
static boolean OEMWMS_CfgCheckVoiceMail
(
  IWMS                         *pMe,
  wms_message_mode_e_type      msgMode,
  const wms_client_message_s_type *pCltMsg,
  boolean                      *isActive,
  uint8                        *count
)
{
#ifndef WIN32
  return(wms_cfg_check_voicemail_contents(msgMode,pCltMsg,isActive,count));
#else
	return TRUE;
#endif
}
#endif

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
FUNCTION: 
    OEMWMS_MsgDeleteBox

DESCRIPTION
    Deletes specified messages from the specified messages box

DEPENDENCIES
    none

RETURN VALUE
    SUCCESS   - Status information is valid
    EBADPARM  - Bad parameter
    ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgDeleteBox
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_deletebox_e_type     box_deltype
)
{
  wms_status_e_type   st;
#ifndef WIN32

  st = wms_msg_delete_box( clientId,
                           (wms_cmd_cb_type) cmdCb->pfnNotify,
                           user_data,
                           box_deltype
                         );
#endif
  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: 
    OEMWMS_MsgCopy

DESCRIPTION
    Allow the client to copy a message from one Storage to another Storage.

DEPENDENCIES
    none

RETURN VALUE
    SUCCESS   - Status information is valid
    EBADPARM  - Bad parameter
    ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgCopy
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data,
  wms_msg_copy_type        *pmsg_copy
)
{
  wms_status_e_type   st;
#ifndef WIN32

  st = wms_msg_copy( clientId,
                     (wms_cmd_cb_type) cmdCb->pfnNotify,
                     user_data,
                     pmsg_copy
                   );
#endif
  return (MapWMSStatusToBREWCode(st));
}

#if defined(FEATURE_UIM_TOOLKIT)
/*===========================================================================
FUNCTION: 
    OEMWMS_RefreshRUIMSMS

DESCRIPTION
    Allow the client to rebuild ruim sms cache.

DEPENDENCIES
    none

RETURN VALUE
    SUCCESS   - Status information is valid
    EBADPARM  - Bad parameter
    ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_RefreshRUIMSMS
(
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmdCb,
  const void               *user_data
)
{
  wms_status_e_type   st;
#ifndef WIN32

  st = wms_msg_refresh_ruimmsg( clientId,
                     (wms_cmd_cb_type) cmdCb->pfnNotify,
                     user_data);
#endif
  return (MapWMSStatusToBREWCode(st));
}
#endif

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
FUNCTION: OEMWMS_MsgSetAutoCDMAParams

DESCRIPTION
  Sets the Automatic mode Parameters for CDMA

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EUNSUPPORTED  - Not Supported

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgSetAutoCDMAParams
(
  IWMS                             * pMe,
  wms_client_id_type                 clientId,
  wms_cdma_template_s_type         * cdma_params  
)
{
  wms_status_e_type   st;

  st =  wms_msg_set_auto_cdma_params( clientId,
                                      cdma_params
                                    );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgSetAutoGWParams

DESCRIPTION
  Sets the Automatic mode Parameters for GSM / WCDMA

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EUNSUPPORTED  - Not Supported

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgSetAutoGWParams
(
  IWMS                             * pMe,
  wms_client_id_type                 clientId,
  wms_gw_template_s_type           * gw_params  
)
{
  wms_status_e_type   st;
 
  st =  wms_msg_set_auto_gw_params( clientId,
                                      gw_params
                                    );

  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgGetAutoCDMAParams

DESCRIPTION
  Gets the Automatic mode Parameters for CDMA

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EUNSUPPORTED  - Not Supported

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgGetAutoCDMAParams
(
  IWMS                             * pMe,
  wms_client_id_type                 clientId,
  wms_cdma_template_s_type         * cdma_params  
)
{
  wms_status_e_type   st;
   
  st =  wms_msg_get_auto_cdma_params( clientId,
                                      cdma_params
                                    );
  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================
FUNCTION: OEMWMS_MsgGetAutoGWParams

DESCRIPTION
  Gets the Automatic mode Parameters for GSM / WCDMA

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EUNSUPPORTED  - Not Supported

SIDE EFFECTS

===========================================================================*/
static int OEMWMS_MsgGetAutoGWParams
(
  IWMS                             * pMe,
  wms_client_id_type                 clientId,
  wms_gw_template_s_type           * gw_params  
)
{
  wms_status_e_type   st;  
  st =  wms_msg_get_auto_gw_params( clientId,
                                      gw_params
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
FUNCTION    OEMWMS_TsConvertCdmaGwMessageToAuto
DESCRIPTION
  Convert the CDMA / GW Message to Auto

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS - Valid Conversion
  EUNSUPPORTED - Not Supported

SIDE EFFECTS
  None
===========================================================================*/
int OEMWMS_TsConvertCdmaGwMessageToAuto (
  IWMS                       * pMe,
  wms_client_message_s_type  * InputPtr,
  uint8                        length,
  wms_auto_message_s_type    * OutputPtr
)
{
  wms_status_e_type   st;
 
  st = wms_ts_convert_cdma_gw_msg_to_auto ( InputPtr,
                                             length,
                                             OutputPtr
                                           );
  return (MapWMSStatusToBREWCode(st));
}                              
  
/*===========================================================================
FUNCTION    OEMWMS_TsConvertAutoToCdmaGwMessage
DESCRIPTION
  Convert the Auto Message to a CDMA / GW Message

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS - Valid Conversion
  EUNSUPPORTED - Not Supported

SIDE EFFECTS
  None
===========================================================================*/
static int OEMWMS_TsConvertAutoToCdmaGwMessage ( 
  IWMS                       * pMe,
  wms_client_id_type           clientId,
  wms_message_mode_e_type      MsgMode,
  wms_auto_message_s_type    * InputPtr,
  wms_client_message_s_type  * OutputPtr
)
{
  wms_status_e_type   st; 
  st = wms_ts_convert_auto_to_cdma_gw_msg( clientId,
                                           MsgMode,
                                           InputPtr,
                                           OutputPtr
                                         );
  return (MapWMSStatusToBREWCode(st));
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

/*===========================================================================

FUNCTION    OEMWMS_BcGetConfig

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
static int OEMWMS_BcGetConfig
(IWMS                  * pMe,
 wms_client_id_type    clientId,
 AEECallback           * cmd_cb,
 const void            * user_data
)
{
  wms_status_e_type st;
  st = wms_bc_get_config( clientId,
                          (wms_cmd_cb_type) cmd_cb->pfnNotify,
                          user_data );
  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================

FUNCTION    OEMWMS_BcGetPref

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

static int OEMWMS_BcGetPref
(IWMS                  * pMe,
 wms_client_id_type    clientId,
 AEECallback           * cmd_cb,
 const void            * user_data
)
{
  wms_status_e_type st;

  st = wms_bc_get_pref( clientId,
                        (wms_cmd_cb_type) cmd_cb->pfnNotify,
                        user_data );
  return (MapWMSStatusToBREWCode(st));
}


/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcSetPref

DESCRIPTION
  Set the broadcast SMS user preference.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcSetPref (
   IWMS               * pMe,
   wms_client_id_type   clientId,
   AEECallback        * cmd_cb,
   const void         * user_data,
   wms_bc_pref_e_type  pref
)
{
  wms_status_e_type st;
  st = wms_bc_set_pref( clientId,
                        (wms_cmd_cb_type) cmd_cb->pfnNotify,
                        user_data,
                        pref );
  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcGetTable
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
static int OEMWMS_BcGetTable
( IWMS                  *pMe,
  wms_client_id_type    clientId,
  AEECallback           *cmd_cb,
  const void            *user_data
)
{
  wms_status_e_type st;
  
  st = wms_bc_get_table( clientId,
                         (wms_cmd_cb_type) cmd_cb->pfnNotify,
                         user_data );
  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcSelectService
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
static int OEMWMS_BcSelectService (
  IWMS                      *pMe,
  wms_client_id_type        clientId,
  AEECallback               *cmd_cb,
  const void                *user_data,
  wms_bc_service_id_s_type  *srvcID,
  boolean                   selected,
  wms_priority_e_type       priority
)
{
  wms_status_e_type st;
  
  st = wms_bc_select_service( clientId,
                              (wms_cmd_cb_type) cmd_cb->pfnNotify,
                              user_data,
                              srvcID,
                              selected,
                              priority );
  return (MapWMSStatusToBREWCode(st));
}

/*===========================================================================

FUNCTION    OEMWMS_BcGetAllServiceIds

DESCRIPTION
 Get all the service IDs.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcGetAllServiceIds
(
  IWMS                  *pMe,
  wms_client_id_type    clientId,
  AEECallback           *cmd_cb,
  const void            *user_data
)
{
  wms_status_e_type st;
  
  st = wms_bc_get_all_service_ids( clientId,
                                   (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                   user_data );
  return (MapWMSStatusToBREWCode(st));
}


/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcGetServiceInfo

DESCRIPTION
 Change the information of an individual broadcast SMS service.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcGetServiceInfo (
  IWMS                      *pMe,
  wms_client_id_type        clientId,
  AEECallback               *cmd_cb,
  const void                *user_data,
  wms_bc_service_id_s_type  *srvcID
)
{
  wms_status_e_type st;
  
  st = wms_bc_get_service_info( clientId,
                                (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                user_data,
                                srvcID );
  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcAddService

DESCRIPTION
 Add an individual broadcast SMS service.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcAddService (
  IWMS                        *pMe,
  wms_client_id_type          clientId,
  AEECallback                 *cmd_cb,
  const void                  *user_data,
  wms_bc_service_info_s_type  *srvcInfo
)
{
  wms_status_e_type st;
  
  st = wms_bc_add_service( clientId,
                           (wms_cmd_cb_type) cmd_cb->pfnNotify,
                           user_data,
                           srvcInfo );
  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcDeleteService

DESCRIPTION
 Delete an individual broadcast SMS service.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcDeleteService (
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmd_cb,
  const void               *user_data,
  wms_bc_service_id_s_type *srvcID
)
{
  wms_status_e_type st;
  st = wms_bc_delete_service( clientId,
                              (wms_cmd_cb_type) cmd_cb->pfnNotify,
                              user_data,
                              srvcID );
  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcChangeLabel

DESCRIPTION
 Change the label of an individual broadcast SMS service.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcChangeLabel (
  IWMS                      *pMe,
  wms_client_id_type        clientId,
  AEECallback               *cmd_cb,
  const void                *user_data,
  wms_bc_service_id_s_type  *srvcID,
  char                      *label
)
{
  wms_status_e_type st;
  
  st = wms_bc_change_label( clientId,
                            (wms_cmd_cb_type) cmd_cb->pfnNotify,
                            user_data,
                            srvcID,
                            label );
  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcDeleteAllServices

DESCRIPTION
 Delete all individual broadcast SMS service.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
static int OEMWMS_BcDeleteAllServices (
  IWMS                      *pMe,
  wms_client_id_type        clientId,
  AEECallback               *cmd_cb,
  const void                *user_data
)
{
  wms_status_e_type st;
  
  st = wms_bc_delete_all_services(clientId,
                                  (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                  user_data);
  return (MapWMSStatusToBREWCode(st));
}

/* <EJECT> */
/*===========================================================================

FUNCTION    OEMWMS_BcSetPriorityForAll

DESCRIPTION
 Delete all individual broadcast SMS service.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
  None

===========================================================================*/
int OEMWMS_BcSetPriorityForAll (
  IWMS                     *pMe,
  wms_client_id_type       clientId,
  AEECallback              *cmd_cb,
  const void               *user_data,
  wms_priority_e_type      priority
)
{
  wms_status_e_type st;
  
  st = wms_bc_set_priority_for_all(clientId,
                                   (wms_cmd_cb_type) cmd_cb->pfnNotify,
                                   user_data,
                                   priority);
  return (MapWMSStatusToBREWCode(st));
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
void OEMWMS_ConvertFromUnicode
(
  wms_client_bd_s_type *bd_data
)
{
  uint8 i;
  uint16  *pTep;

  if (bd_data != NULL)
  {
    /* Decrease the data length by half */
    bd_data->user_data.data_len = bd_data->user_data.data_len/2;
    pTep = (uint16 *)bd_data->user_data.data;

    /* Extract the high byte from the user data.  This will be equivalent to
    ** 8-bit Ascii */
    for( i=0; i<bd_data->user_data.data_len; i++,pTep++ )
    {
      //bd_data->user_data.data[i] = bd_data->user_data.data[i*2+1];
      *pTep = NTOHS(*pTep);
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


#ifdef  FEATURE_CDSMS_CACHE_USELIST
/*==============================================================================
//           W M S   C A C A H E I N F O   M O D U L E
// 文件:
//        OEMWMS_CacheInfo.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        短信息 Cache Info 结构的定义、建立及维护。供 WMSApp 快速访问存储的短信
//        用。         
//
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "OEMWMS_CacheInfo.h"
#include "clk.h"


/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
#define BCD2INT(x)  (((((x) & 0xf0) >> 4) * 10) + ((x) & 0x0f))

// 长短信数据包间的时间差不允许超过下列值,否则视为属于不同批次的短信
// 1天 = 24*60*60 = 86400 秒
#define MAXDIFFER_TIME  86400
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
// 释放节点内部数据的函数指针类型定义。
typedef void (*PFNWMSNODEFREE)(wms_cache_info_node * pnode);

// 用于比较两个节点大小的函数指针类型定义。
typedef int (*PFNWMSNODECOMPARE)(wms_cache_info_node * pnode1,
                                 wms_cache_info_node * pnode2);


/*==============================================================================
                                 
                                 本地全局变量定义
                                 
==============================================================================*/
// Critical Section for WMS cache info
rex_crit_sect_type		         wms_cache_info_crit_sect;		


/*==============================================================================

                              本地函数声明

==============================================================================*/
static void wms_free_listnodedata(wms_cache_info_node * pnode);
void wms_destroy_cacheinfolist(wms_cache_info_list *plist, 
                                      PFNWMSNODEFREE pfn);
static int wms_cacheinfolist_insertsort(wms_cache_info_list *plist, 
                                        wms_cache_info_node *pnode,
                                        PFNWMSNODECOMPARE pfn);
static int wms_cacheinfolist_delete(wms_cache_info_list *plist,
                                    wms_memory_store_e_type   mem_store,  
                                    wms_message_index_type index, 
                                    PFNWMSNODEFREE pfn);
static int  wms_cacheinfolist_nodecompare(wms_cache_info_node * pnode1,
                                          wms_cache_info_node * pnode2);
                                          
static wms_box_e_type wms_getbox_bytag(wms_message_tag_e_type tag);

/*==============================================================================

                                 函数定义
                                 
==============================================================================*/
/*==============================================================================
函数: 
    OEMWMS_GetTimeFromWmsTimestamp
       
说明: 
    函数用来将短信时间戳格式转换为秒。
       
参数: 
    pTime [in]: wms_timestamp_s_type 结构数据指针
    
返回值:
    转换后的时间值。
       
备注:
    
==============================================================================*/
static uint32 OEMWMS_GetTimeFromWmsTimestamp(wms_timestamp_s_type *pTime)
{
    uint8 tmp;
    clk_julian_type jt;
    
    MEMSET(&jt, 0, sizeof(jt));
    
    tmp = BCD2INT(pTime->year);
    
    // If the year is above 95 it we assume it is in 20 th century(1900+ year
    // otherwise we assume it is in 21 century(2000+year)
    jt.year = (tmp <= 95) ? (2000 + tmp) : (1900 + tmp);
    
    jt.month = BCD2INT(pTime->month);
    jt.day = BCD2INT(pTime->day);
    jt.hour = BCD2INT(pTime->hour);
    jt.minute = BCD2INT(pTime->minute);
    jt.second = BCD2INT(pTime->second);
    
    return clk_julian_to_secs(&jt);
}

/*==============================================================================
函数: 
    wms_cacheinfolist_insertsort_ext
       
说明: 
    函数用来在指定短消息 cache 链表中按从大到小的顺序插入节点。
       
参数: 
    plist [in]: 短消息 cache 链表指针。
    pnode [in]: 短消息 cache 链表节点指针。
    pfn [in]: 短消息 cache 链表两节点数据大小比较函数指针。
    
返回值:
    成功: SUCCESS
    失败: EFAILED
       
备注:
    本函数目前仅用于 wms_cacheinfolist_inboxresort 。
    
==============================================================================*/
static int wms_cacheinfolist_insertsort_ext(wms_cache_info_list *plist, 
                                            wms_cache_info_node *pnode,
                                            PFNWMSNODECOMPARE pfn)
{
    int nRet1, nRet2;
    wms_cache_info_node *ptepnode = NULL;   // 用来遍历plist的临时指针
    
    if ((NULL == plist) || (NULL == pnode))
    {
        return EFAILED;
    }
    
    if (NULL == plist->pTail)
    {// 链表节点为空
        pnode->pNext = NULL;
        pnode->pPrev = NULL;
        
        plist->pHead = pnode;
        plist->pTail = pnode;
        
        goto INSERTSORT_EXT_EXIT;
    }
    else if (NULL == pfn)
    {// 若不提供比较函数，则直接将节点加在链表末尾
        pnode->pNext = NULL;
        pnode->pPrev = plist->pTail;
        
        plist->pTail->pNext = pnode;
        plist->pTail = pnode;
        
        goto INSERTSORT_EXT_EXIT;
    }
    
    // 按从大到小的顺序排列
    ptepnode = plist->pHead;
    while (NULL != ptepnode->pNext)
    {
        nRet1 = (*pfn)(ptepnode, pnode);
        nRet2 = (*pfn)(ptepnode->pNext, pnode);
        
        if ((nRet1 >= 0) || (nRet2 >= 0))
        {
            ptepnode = ptepnode->pNext;
            continue;
        }
        else
        {
            if (ptepnode == plist->pHead)
            {// 将插入节点变成头节点
                pnode->pPrev = NULL;
                pnode->pNext = plist->pHead;
                (plist->pHead)->pPrev = pnode;
                plist->pHead = pnode;
            }
            else
            {
                (ptepnode->pPrev)->pNext = pnode;
                pnode->pPrev = ptepnode->pPrev;
                pnode->pNext = ptepnode;
                ptepnode->pPrev = pnode;
            }
            
            goto INSERTSORT_EXT_EXIT;
        }
    }
    
    // 已搜索到链表尾部
    nRet1 = (*pfn)(ptepnode, pnode);
    if (nRet1 >= 0)
    {// pnode 追加在链表尾部
        pnode->pNext = NULL;
        pnode->pPrev = ptepnode;
        ptepnode->pNext = pnode;
        plist->pTail = pnode;
    }
    else
    {
        if (ptepnode == plist->pHead)
        {// 将插入节点变成头节点
            pnode->pPrev = NULL;
            pnode->pNext = plist->pHead;
            (plist->pHead)->pPrev = pnode;
            plist->pHead = pnode;
        }
        else
        {
            (ptepnode->pPrev)->pNext = pnode;
            pnode->pPrev = ptepnode->pPrev;
            pnode->pNext = ptepnode;
            ptepnode->pPrev = pnode;
        }
    }

INSERTSORT_EXT_EXIT:
    return SUCCESS;
}


/*==============================================================================
函数: 
    wms_cacheinfolist_inboxresort
       
说明: 
    函数用来对收件箱消息列表进行排序。
       
参数: 
    plist [in]: 收件箱 Cache 信息列表
    
返回值:

       
备注:

排序规则：
1、整个列表划分为未读、已读部分，未读部分处于前列；
2、各部分按时间顺序由近及远(时间戳由大到小)的顺序排列。
==============================================================================*/
static void wms_cacheinfolist_inboxresort(wms_cache_info_list *plist)
{
    int         nRet;
    int         xuhao;
    wms_cache_info_node *pnode = NULL;
    wms_cache_info_node *ptepnode = NULL;
    wms_cache_info_list *preadlist = NULL;
    wms_cache_info_list *pnotreadlist = NULL;
    
    if ((NULL == plist) || 
        (NULL == plist->pHead))
    {// 空指针或空链表
        return;
    }
    
    // 分配两个临时链表
    preadlist = (wms_cache_info_list *)sys_malloc(sizeof(wms_cache_info_list));
    pnotreadlist = (wms_cache_info_list *)sys_malloc(sizeof(wms_cache_info_list));
    if ((NULL == preadlist) || (NULL == pnotreadlist))
    {
        goto INBOXRESORT_EXIT;
    }
    
    (void)MEMSET(preadlist, 0, sizeof(wms_cache_info_list));
    (void)MEMSET(pnotreadlist, 0, sizeof(wms_cache_info_list));
    
    // 第一步，将整个链表分类按时间排序到 preadlist 及 pnotreadlist
    pnode = plist->pHead;
    while (NULL != pnode)
    {
        ptepnode = pnode->pNext;
        if (pnode->msg_tag==WMS_TAG_MT_NOT_READ)
        {
            nRet = wms_cacheinfolist_insertsort_ext(pnotreadlist, pnode, wms_cacheinfolist_nodecompare);
        }
        else
        {
            nRet = wms_cacheinfolist_insertsort_ext(preadlist, pnode, wms_cacheinfolist_nodecompare);
        }
        
        if (nRet != SUCCESS)
        {
            goto INBOXRESORT_EXIT;
        }
        pnode = ptepnode;
    }
    
    // 第二步，重建链表，未读部分先加入
    plist->pHead = NULL;
    plist->pTail = NULL;
    xuhao = 0;
    
    // 加入未读部分
    pnode = pnotreadlist->pHead;
    while (NULL != pnode)
    {
        ptepnode = pnode->pNext;
        pnode->xuhao = (++xuhao);
        if (NULL == plist->pTail)
        {// 链表节点为空
            pnode->pNext = NULL;
            pnode->pPrev = NULL;
            
            plist->pHead = pnode;
            plist->pTail = pnode;
        }
        else 
        {// 则直接将节点加在链表末尾
            pnode->pNext = NULL;
            pnode->pPrev = plist->pTail;
            
            plist->pTail->pNext = pnode;
            plist->pTail = pnode;
        }
        pnode = ptepnode;
    }
    
    // 加入已读部分
    pnode = preadlist->pHead;
    while (NULL != pnode)
    {
        ptepnode = pnode->pNext;
        pnode->xuhao = (++xuhao);
        if (NULL == plist->pTail)
        {// 链表节点为空
            pnode->pNext = NULL;
            pnode->pPrev = NULL;
            
            plist->pHead = pnode;
            plist->pTail = pnode;
        }
        else 
        {// 则直接将节点加在链表末尾
            pnode->pNext = NULL;
            pnode->pPrev = plist->pTail;
            
            plist->pTail->pNext = pnode;
            plist->pTail = pnode;
        }
        pnode = ptepnode;
    }
    
    plist->nBranches = xuhao;

INBOXRESORT_EXIT:
    if (NULL != preadlist)
    {
        sys_free(preadlist);
    }
    if (NULL != pnotreadlist)
    {
        sys_free(pnotreadlist);
    }
}


/*==============================================================================
函数: 
    wms_cacheinfolist_updatexuhao
       
说明: 
    函数用来更新链表主干线节点序号。
       
参数: 
    ebox [in]: 短消息存储箱
    
返回值:
    成功: 短消息存储箱对应的 cache 链表指针。
    失败: NULL
       
备注:
     
==============================================================================*/
void wms_cacheinfolist_updatexuhao(wms_box_e_type ebox)
{
    int i=0, j=0;
    wms_cache_info_node *pnode = NULL;
    wms_cache_info_list *plist = NULL;
    
    plist = wms_get_cacheinfolist(ebox);
    
    if (NULL == plist)
    {
        return;
    }
    
    if (WMS_MB_INBOX == ebox)
    {// 对收件箱重新排序并同时更新序号，未读短信始终位于前面
        wms_cacheinfolist_inboxresort(plist);
        return;
    }
    
    pnode = plist->pHead;
    
    while (NULL != pnode)
    {
        if ((ebox == WMS_MB_RESERVE) ||
            (ebox == WMS_MB_RSVFAILED))
        {
            pnode->xuhao = (pnode->msg_tag == WMS_TAG_RESERVE ? ++i : ++j);
        }
        else
        {
            pnode->xuhao = (++i);
        }
        pnode = pnode->pNext;
    }
    
    if ((ebox == WMS_MB_RESERVE) ||
        (ebox == WMS_MB_RSVFAILED))
    {
        plist->nOnUIMS = j;// 预约失败的分支数
    }

    plist->nBranches = i;
}

/*==============================================================================
函数: 
    wms_get_cacheinfolist
       
说明: 
    函数用来获取短消息各存储箱对应的 cache 链表指针。
       
参数: 
    ebox [in]: 短消息存储箱
    
返回值:
    成功: 短消息存储箱对应的 cache 链表指针。
    失败: NULL
       
备注:
     
==============================================================================*/
wms_cache_info_list * wms_get_cacheinfolist(wms_box_e_type ebox)
{
    static wms_cache_info_list  *pwms_voice_list = NULL;
    static wms_cache_info_list  *pwms_inbox_list = NULL;
    static wms_cache_info_list  *pwms_outbox_list = NULL;
    static wms_cache_info_list  *pwms_template_list = NULL;
    static wms_cache_info_list  *pwms_draft_list = NULL;
    static wms_cache_info_list  *pwms_reserve_list = NULL;
    
    wms_cache_info_list **pteplist = NULL;
    static  boolean bfirstrun = TRUE;
    
    if (bfirstrun)
    {
        rex_init_crit_sect(&wms_cache_info_crit_sect);
        bfirstrun = FALSE;
    }
    
    switch (ebox)
    {
        case WMS_MB_VOICEMAIL:
            pteplist = &pwms_voice_list;
            break;
        
        case WMS_MB_INBOX:
            pteplist = &pwms_inbox_list;
            break;
             
        case WMS_MB_OUTBOX:
            pteplist = &pwms_outbox_list;
            break;
            
        case WMS_MB_DRAFT:
            pteplist = &pwms_draft_list;
            break;
            
        case WMS_MB_RESERVE:
        case WMS_MB_RSVFAILED:
            pteplist = &pwms_reserve_list;
            break;
            
        case WMS_MB_TEMPLATE:
            pteplist = &pwms_template_list;
            break;
            
        default:
            return NULL;
    }
    
    if (NULL != (*pteplist))
    {
        return (*pteplist);
    }
    
    (*pteplist) = sys_malloc(sizeof(wms_cache_info_list));
    if (NULL != (*pteplist))
    {// 作链表必要的初始化
        (void)MEMSET((*pteplist), 0, sizeof(wms_cache_info_list));
    }
    
    return (*pteplist);
}

/*==============================================================================
函数: 
    wms_free_listnodedata
       
说明: 
    函数用来获取短消息 cache 链表节点数据。
       
参数: 
    pnode [in]: 短消息 cache 链表节点指针。
    
返回值:
    none
       
备注:
    
==============================================================================*/
static void wms_free_listnodedata(wms_cache_info_node * pnode)
{
    if (NULL == pnode)
    {
        return;
    }
    
    if (NULL != pnode->pszNum)
    {
        sys_free(pnode->pszNum);
        pnode->pszNum = NULL;
    }
    
    if (NULL != pnode->pwszTitle)
    {
        sys_free(pnode->pwszTitle);
        pnode->pwszTitle = NULL;
    }

#ifdef FEATURE_SMS_UDH
    if (pnode->pItems != NULL)
    {
        int i;
        wms_cache_info_node *pItemNode = NULL;
        
        for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
        {
            pItemNode = pnode->pItems[i];
            
            if (pItemNode == pnode)
            {// 此节点不能在此释放
                continue;
            }
            
            // 注意子项的 pItems 必须为 NULL
            // 释放节点数据
            wms_free_listnodedata(pItemNode);
            
            // 释放节点
            sys_free(pItemNode);
            pnode->pItems[i] = NULL;
        }
        
        sys_free(pnode->pItems);
        pnode->pItems = NULL;
    }
#endif    
}

/*==============================================================================
函数: 
    wms_destroy_cacheinfolist
       
说明: 
    函数用来销毁指定短消息 cache 链表。
       
参数: 
    plist [in]: 短消息 cache 链表指针。
    pfn [in]: 短消息 cache 链表节点数据释放函数指针。
    
返回值:
    none
       
备注:
    
==============================================================================*/
void wms_destroy_cacheinfolist(wms_cache_info_list *plist, 
                                      PFNWMSNODEFREE pfn)
{
    wms_cache_info_node *pnode = NULL;
    wms_cache_info_node *pdelnode = NULL;
    
    if (NULL == plist)
    {
        return;
    }
    
    // 从头节点开始，一个接一个释放链表节电及节点数据
    pnode = plist->pHead;
    
    while (NULL != pnode)
    {
        pdelnode = pnode;
        pnode = pnode->pNext;
        
        if (NULL != pfn)
        {
            // 释放节点数据
            (*pfn)(pdelnode);
        }
        
        // 释放节点
        sys_free(pdelnode);
    }
    
    // 释放链表结构体
    sys_free(plist);
    
    plist = NULL;
}

/*==============================================================================
函数: 
    wms_cacheinfolist_insertsort
       
说明: 
    函数用来在指定短消息 cache 链表中按从大到小的顺序插入节点。
       
参数: 
    plist [in]: 短消息 cache 链表指针。
    pnode [in]: 短消息 cache 链表节点指针。
    pfn [in]: 短消息 cache 链表两节点数据大小比较函数指针。
    
返回值:
    成功: SUCCESS
    失败: EFAILED
       
备注:
    本函数假定链表已按从大到小的顺序排序好。最好链表节点加入时统一用此函数。
    
==============================================================================*/
static int wms_cacheinfolist_insertsort(wms_cache_info_list *plist, 
                                        wms_cache_info_node *pnode,
                                        PFNWMSNODECOMPARE pfn)
{
    int nRet1, nRet2;
    wms_cache_info_node *ptepnode = NULL;   // 用来遍历plist的临时指针
    
    if ((NULL == plist) || (NULL == pnode))
    {
        return EFAILED;
    }
    
#ifdef FEATURE_SMS_UDH
    // 需检查链表干线上是否存在对应支线(在能够串接的范围内才检查)
    if (pnode->total_sm > 1)
    {
        uint32  dwTime;
        int     j;
        int32   nSize;
        int     nBranches;
        int     nCurBranchNum = 0;
        
        // 计算长短信分支总数(每个分支最大含 LONGSMS_MAX_PACKAGES 个数据包)
        nBranches = pnode->total_sm / (LONGSMS_MAX_PACKAGES);
        if ((pnode->total_sm % (LONGSMS_MAX_PACKAGES)) != 0)
        {
            nBranches++;
        }
        
        // 确定待插入节点归属的分支号(0,...,nBranches-1)
        nCurBranchNum = (pnode->seq_num - 1) / (LONGSMS_MAX_PACKAGES);
        
        ptepnode = plist->pHead;
        while (NULL != ptepnode)
        {
            if (ptepnode->dwTime >= pnode->dwTime)
            {
                dwTime = ptepnode->dwTime - pnode->dwTime;
            }
            else
            {
                dwTime = pnode->dwTime - ptepnode->dwTime;
            }
            if ((ptepnode->msg_ref == pnode->msg_ref) &&
                (ptepnode->total_sm == pnode->total_sm) &&
                ((ptepnode->seq_num - 1) / (LONGSMS_MAX_PACKAGES) == nCurBranchNum) &&
                (dwTime < MAXDIFFER_TIME))
            {// 找到本数据包所在的支线
                if (ptepnode->pItems == NULL)
                {
                    MSG_ERROR("Bad conditions!",0,0,0);
                    return EFAILED;
                }
                
                // 重复?约觳?
                j = (pnode->seq_num - 1) % LONGSMS_MAX_PACKAGES;
                if (ptepnode->pItems[j] != NULL)
                {// 重复! 对此情况继续往后查找。
                    ptepnode = ptepnode->pNext;
                    continue;
                }
                
                ptepnode->pItems[j] = pnode;
                plist->nCount++;
                
                if (pnode->mem_store == WMS_MEMORY_STORE_RUIM)
                {
                    plist->nOnUIMS++;
                }
                
                // 未读消息节点数统计
                if (pnode->msg_tag == WMS_TAG_MT_NOT_READ)
                {
                    if ((pnode->index == 0) &&
                        (pnode->mem_store == WMS_MEMORY_STORE_NV_CDMA))
                    {// 语音通知短信
                        ;
                    }
                    else
                    {
                    	if(pnode->total_sm>0)
						{
							if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
							{
								plist->nNews++;
							}
						}
						else
						{
							plist->nNews++;
						}
                    }
                }
                else if ((pnode->msg_tag == WMS_TAG_PHRASE) &&
                         (pnode->index >= PHRASE_START+MAX_OEMTEMPLATES))
                {// 常用语用 nNews 表示用户新添加的常语数，不含内置修改而得的常语
	                if(pnode->total_sm>0)
					{
						if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
						{
							plist->nNews++;
						}
					}
					else
					{
						plist->nNews++;
					}
                }
                else if (pnode->msg_tag == WMS_TAG_RESERVE)
                {
	                if(pnode->total_sm>0)
					{
						if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
						{
							plist->nNews++;
						}
					}
					else
					{
						plist->nNews++;
					}
                }
                
                return SUCCESS;
            }
            
            ptepnode = ptepnode->pNext;
        }
        
        // 没找到本数据包所在的支线, 需为本节点分配 pItems 空间
        nSize = LONGSMS_MAX_PACKAGES * sizeof(wms_cache_info_node *);
        pnode->pItems = sys_malloc(nSize);
        if (NULL == pnode->pItems)
        {
            MSG_ERROR("MALLOC failed!",0,0,0);
            return EFAILED;
        }
        MEMSET(pnode->pItems, 0, nSize);
        j = (pnode->seq_num - 1) % LONGSMS_MAX_PACKAGES;
        pnode->pItems[j] = pnode;
    }
#endif
    
    if (NULL == plist->pTail)
    {// 链表节点为空
        pnode->pNext = NULL;
        pnode->pPrev = NULL;
        
        plist->pHead = pnode;
        plist->pTail = pnode;
        
        goto INSERTSORT_EXIT;
    }
    else if (NULL == pfn)
    {// 若不提供比较函数，则直接将节点加在链表末尾
        pnode->pNext = NULL;
        pnode->pPrev = plist->pTail;
        
        plist->pTail->pNext = pnode;
        plist->pTail = pnode;
        
        goto INSERTSORT_EXIT;
    }
    
    // 按从大到小的顺序排列
    ptepnode = plist->pHead;
    while (NULL != ptepnode->pNext)
    {
        nRet1 = (*pfn)(ptepnode, pnode);
        nRet2 = (*pfn)(ptepnode->pNext, pnode);
        
        if ((nRet1 >= 0) || (nRet2 >= 0))
        {
            ptepnode = ptepnode->pNext;
            continue;
        }
        else
        {
            if (ptepnode == plist->pHead)
            {// 将插入节点变成头节点
                pnode->pPrev = NULL;
                pnode->pNext = plist->pHead;
                (plist->pHead)->pPrev = pnode;
                plist->pHead = pnode;
            }
            else
            {
                (ptepnode->pPrev)->pNext = pnode;
                pnode->pPrev = ptepnode->pPrev;
                pnode->pNext = ptepnode;
                ptepnode->pPrev = pnode;
            }
            
            goto INSERTSORT_EXIT;
        }
    }
    
    // 已搜索到链表尾部
    nRet1 = (*pfn)(ptepnode, pnode);
    if (nRet1 >= 0)
    {// pnode 追加在链表尾部
        pnode->pNext = NULL;
        pnode->pPrev = ptepnode;
        ptepnode->pNext = pnode;
        plist->pTail = pnode;
    }
    else
    {
        if (ptepnode == plist->pHead)
        {// 将插入节点变成头节点
            pnode->pPrev = NULL;
            pnode->pNext = plist->pHead;
            (plist->pHead)->pPrev = pnode;
            plist->pHead = pnode;
        }
        else
        {
            (ptepnode->pPrev)->pNext = pnode;
            pnode->pPrev = ptepnode->pPrev;
            pnode->pNext = ptepnode;
            ptepnode->pPrev = pnode;
        }
    }
    
INSERTSORT_EXIT:
    if (pnode->msg_tag == WMS_TAG_MT_NOT_READ)
    {
        if ((pnode->index == 0) &&
            (pnode->mem_store == WMS_MEMORY_STORE_NV_CDMA))
        {// 语音通知短信
            ;
        }
        else
        {
            if(pnode->total_sm>0)
			{
				if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
				{
					plist->nNews++;
				}
			}
			else
			{
				plist->nNews++;
			}
        }
    }
    else if ((pnode->msg_tag == WMS_TAG_PHRASE) &&
             (pnode->index >= PHRASE_START+MAX_OEMTEMPLATES))
    {// 常用语用 nNews 表示用户新添加的常语数，不含内置修改而得的常语
		 if(pnode->total_sm>0)
		{
			if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
			{
				plist->nNews++;
			}
		}
		else
		{
			plist->nNews++;
		}
    }
    else if (pnode->msg_tag == WMS_TAG_RESERVE)
    {
         if(pnode->total_sm>0)
		{
			if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
			{
				plist->nNews++;
			}
		}
		else
		{
			plist->nNews++;
		}
    }
    
    if (pnode->mem_store == WMS_MEMORY_STORE_RUIM)
    {
        plist->nOnUIMS++;
    }
        
    plist->nCount++;
    return SUCCESS;
}

/*==============================================================================
函数: 
    wms_cacheinfolist_delete
       
说明: 
    函数用来删除指定短消息 cache 链表中符合条件的节点。
       
参数: 
    plist [in]: 短消息 cache 链表指针。
    mem_store [in]: 消息存储位置
    index [in]: 消息在存储空间上的索引号。
    pfn [in]: 短消息 cache 链表两节点数据大小比较函数指针。
    
返回值:
    成功: SUCCESS
    失败: EFAILED
       
备注:
    
==============================================================================*/
static int wms_cacheinfolist_delete(wms_cache_info_list *plist,
                                    wms_memory_store_e_type   mem_store,  
                                    wms_message_index_type index, 
                                    PFNWMSNODEFREE pfn)
{
    wms_cache_info_node *pnode; // 用来遍历plist的指针
#ifdef FEATURE_SMS_UDH
    wms_cache_info_node *delnode;
    int  i, j,nSubItems;
#endif    
    
    if (NULL == plist)
    {
        return EFAILED;
    }
    
    if (NULL == plist->pTail)
    {// 链表节点为空
        return EFAILED;
    }
    
    pnode = plist->pHead;
    while (NULL != pnode)
    {
#ifdef FEATURE_SMS_UDH
        if (pnode->pItems != NULL) 
        {// 存在子项列表
            delnode = NULL;
            nSubItems = 0;
            j = -1;
            
            // 先统计有效子节点数，同时找出待删除的节点(若存在的话)
            for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
            {
                if (NULL == pnode->pItems[i])
                {
                    continue;
                }
                
                nSubItems++;
                
                if ((pnode->pItems[i]->index == index) && 
                    (pnode->pItems[i]->mem_store == mem_store))
                {// 找到待删除节点
                    delnode = pnode->pItems[i];
                    j = i;
                }
            }
            
            if (NULL == delnode)
            {// 未找到待删除节点
                pnode = pnode->pNext;
                continue;
            }
            
            // 找到待删除节点
            if (nSubItems >= 2)
            {
                if (pnode == delnode)
                {// 节点位于链表干线上
                    // 找出支线上另一节点顶替该节点
                    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                    {
                        if ((NULL != pnode->pItems[i]) &&
                            (pnode->pItems[i] != pnode))
                        {
                            // 进入临界区操作
                            rex_enter_crit_sect(&wms_cache_info_crit_sect);
                            if (plist->pCur == pnode)
                            {
                                plist->pCur = pnode->pNext;
                            }
                            rex_leave_crit_sect(&wms_cache_info_crit_sect);
                            
                            if (pnode->pPrev == NULL)
                            {// 节点为头节点
                                plist->pHead = pnode->pItems[i];
                                if (pnode->pNext == NULL)
                                {// 链表仅有一个节点
                                    plist->pTail = pnode->pItems[i];
                                }
                                else
                                {
                                    pnode->pItems[i]->pNext = pnode->pNext;
                                    pnode->pNext->pPrev = pnode->pItems[i];
                                }
                            }
                            else if (pnode->pNext == NULL)
                            {// 节点为尾节点
                                (pnode->pPrev)->pNext = pnode->pItems[i];
                                pnode->pItems[i]->pNext = NULL;
                                pnode->pItems[i]->pPrev = pnode->pPrev;
                                plist->pTail = pnode->pItems[i];
                            }
                            else
                            {// 节点为中间节点
                                pnode->pItems[i]->pPrev = pnode->pPrev;
                                pnode->pItems[i]->pNext = pnode->pNext;
                                (pnode->pPrev)->pNext = pnode->pItems[i];
                                (pnode->pNext)->pPrev = pnode->pItems[i];
                            }
                            
                            pnode->pItems[j] = NULL;
                            pnode->pItems[i]->pItems = pnode->pItems;
                            pnode->pItems = NULL;
                            break;
                        }
                    }
                }
                else
                {// 节点位于链表支线上
                    pnode->pItems[j] = NULL;
                    pnode = delnode;
                }
                
                // 更新未读消息节点数
                if (pnode->msg_tag == WMS_TAG_MT_NOT_READ)
                {
                    if ((pnode->index == 0) &&
                        (pnode->mem_store == WMS_MEMORY_STORE_NV_CDMA))
                    {// 语音通知短信
                        plist->nNews = 0;
                        plist->nAll = 0;
                    }
                    else
                    {
                    	if(pnode->total_sm>0)
						{
							if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
							{
                        		if(plist->nNews>0)
								{
	                    			plist->nNews--;
								}
							}
                    	}
						else
						{
							if(plist->nNews>0)
							{
                    			plist->nNews--;
							}
						}
                    }
                }
                else if ((pnode->msg_tag == WMS_TAG_PHRASE) &&
                         (pnode->index >= PHRASE_START+MAX_OEMTEMPLATES))
                {// 常用语用 nNews 表示用户新添加的常语数，不含内置修改而得的常语
                    if(pnode->total_sm>0)
					{
						if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
						{
							if(plist->nNews>0)
							{
                    			plist->nNews--;
							}
						}
                	}
					else
					{
						if(plist->nNews>0)
						{
                			plist->nNews--;
						}
					}
                }
                else if (pnode->msg_tag == WMS_TAG_RESERVE)
                {
                    if(pnode->total_sm>0)
					{
						if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
						{
                    		if(plist->nNews>0)
							{
                    			plist->nNews--;
							}
						}
                	}
					else
					{
						if(plist->nNews>0)
						{
                			plist->nNews--;
						}
					}
                }
                
                if (pnode->mem_store == WMS_MEMORY_STORE_RUIM)
                {
                    plist->nOnUIMS--;
                }
                
                // 释放节点数据
                if (pfn != NULL)
                {
                    (*pfn)(pnode);
                }
                
                // 释放节点
                sys_free(pnode);
                
                plist->nCount--;
                return SUCCESS;
            }
            
            // 找到待删除节点,且支线只有一个成员，在后面删除该节点即可
            break;
        }
        else
#endif        
        if ((pnode->index == index) && (pnode->mem_store == mem_store))
        {
            break;
        }

        pnode = pnode->pNext;
    }
    
    if (NULL == pnode)
    {
        return EFAILED;
    }
        
    // 进入临界区操作
    rex_enter_crit_sect(&wms_cache_info_crit_sect);
    if (plist->pCur == pnode)
    {
        plist->pCur = pnode->pNext;
    }
    rex_leave_crit_sect(&wms_cache_info_crit_sect);
        
    // 找到匹配记录,将该节点从链表中断开
    if (pnode == plist->pHead)
    {// 节点为头节点
        if (pnode->pNext == NULL)
        {// 链表仅有一个节点
            plist->pHead = NULL;
            plist->pTail = NULL;
        }
        else
        {
            plist->pHead = (plist->pHead)->pNext;
            plist->pHead->pPrev = NULL;
        }
    }
    else if (pnode == plist->pTail)
    {// 节点为尾节点
        plist->pTail = pnode->pPrev;
        plist->pTail->pNext = NULL;
    }
    else
    {// 节点为中间节点
        (pnode->pPrev)->pNext = pnode->pNext;
        (pnode->pNext)->pPrev = pnode->pPrev;
    }
        
    if ((pnode->index == 0) &&
        (pnode->mem_store == WMS_MEMORY_STORE_NV_CDMA))
    {// 语音通知短信
        plist->nNews = 0;
        plist->nAll = 0;
    }
    else if (pnode->msg_tag == WMS_TAG_MT_NOT_READ)
    {
        if(pnode->total_sm>0)
		{
			if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
			{
        		if(plist->nNews>0)
				{
        			plist->nNews--;
				}
			}
    	}
		else
		{
			if(plist->nNews>0)
			{
    			plist->nNews--;
			}
		}
    }
    else if (pnode->msg_tag == WMS_TAG_RESERVE)
    {
        if(pnode->total_sm>0)
		{
			if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
			{
        		if(plist->nNews>0)
				{
        			plist->nNews--;
				}
			}
    	}
		else
		{
			if(plist->nNews>0)
			{
    			plist->nNews--;
			}
		}
    }
    else if ((pnode->msg_tag == WMS_TAG_PHRASE) &&
             (pnode->index >= PHRASE_START+MAX_OEMTEMPLATES))
    {// 常用语用 nNews 表示用户新添加的常语数，不含内置修改而得的常语
        if(pnode->total_sm>0)
		{
			if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
			{
        		if(plist->nNews>0)
				{
        			plist->nNews--;
				}
			}
    	}
		else
		{
			if(plist->nNews>0)
			{
    			plist->nNews--;
			}
		}
    }
    
    if (pnode->mem_store == WMS_MEMORY_STORE_RUIM)
    {
        plist->nOnUIMS--;
    }
    // 释放节点数据
    if (pfn != NULL)
    {
        (*pfn)(pnode);
    }
    
    // 释放节点
    sys_free(pnode);
    
    plist->nCount--;
	plist->nBranches = 0;
    return SUCCESS;
}

/*==============================================================================
函数: 
    wms_cacheinfolist_nodecompare
       
说明: 
    函数用来删除指定短消息 cache 链表两节点的大小。
       
参数: 
    pnode1 [in]: 短消息 cache 链表节点指针。
    pnode2 [in]: 短消息 cache 链表节点指针。
    
返回值:
    0 : pnode1 等于 pnode2
    >0: pnode1 大于 pnode2
    <0: pnode1 小于 pnode2
       
备注:
    
==============================================================================*/
static int  wms_cacheinfolist_nodecompare(wms_cache_info_node * pnode1,
                                          wms_cache_info_node * pnode2)
{
    if ((pnode1->msg_tag == WMS_TAG_PHRASE) &&
        (pnode2->msg_tag == WMS_TAG_PHRASE))
    {
        if (pnode1->index > pnode2->index)
        {
            return 1;
        }
        else if (pnode1->index == pnode2->index)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (pnode1->dwTime > pnode2->dwTime)
    {
        if ((pnode1->msg_tag == WMS_TAG_RESERVE || pnode1->msg_tag == WMS_TAG_RSVFAILED) &&
            (pnode2->msg_tag == WMS_TAG_RESERVE || pnode2->msg_tag == WMS_TAG_RSVFAILED))
        {// 预约短信按预约时间由近及远的顺序排列
            return -1;
        }
        
        return 1;
    }
    else if(pnode1->dwTime == pnode2->dwTime)
    {
        return 0;
    }
    else
    {
        if ((pnode1->msg_tag == WMS_TAG_RESERVE || pnode1->msg_tag == WMS_TAG_RSVFAILED) &&
            (pnode2->msg_tag == WMS_TAG_RESERVE || pnode2->msg_tag == WMS_TAG_RSVFAILED))
        {// 预约短信按预约时间由近及远的顺序排列
            return 1;
        }
        
        return -1;
    }
}

/*==============================================================================
函数: 
    OEMWMS_MsgInfoCacheCbExt
       
说明: 
    Callback function for Message Info Cache. The cached information is used to
    display the message lists
       
参数: 
    pMsg [in]: 消息 wms_client_message_s_type 结构指针。
    delcachetag [in]: only used while pMsg->msg_hdr.tag == WMS_TAG_NONE
    
返回值:
    none
       
备注:
    函数在插入节点前，先检索 cache info 链表若存在则相应节点则删除之。
    然后作为新节点加入。pMsg->msg_hdr.tag == WMS_TAG_NONE 表示要删除
    Cache 节点，此时要用到 delcachetag
    
==============================================================================*/
void OEMWMS_MsgInfoCacheCbExt(const wms_client_message_s_type  *pMsg,
                              wms_message_tag_e_type    delcachetag)
{
    wms_client_ts_data_s_type   *cl_data = NULL;    // 需分配内存
    wms_client_bd_s_type        *info_data = NULL;  // 需分配内存
    uint8                       *buf = NULL;        // 需分配内存
    int                         nlen = 0;
    wms_cache_info_list         *plist = NULL;
    wms_cache_info_node         *pnode = NULL;      // 需分配内存
    wms_cdma_user_data_s_type   *puserdata = NULL;
    int32                       nSize;
    int32                       i;
    wms_box_e_type              ebox = WMS_MB_NONE;
    int                         nVMNew = 0;
    int                         nVMAll = 0;

    if (pMsg == NULL)
    {
        MSG_ERROR("Null Pointers!",0,0,0);
        return;
    }
    if (pMsg->msg_hdr.message_mode != WMS_MESSAGE_MODE_CDMA)
    {
        MSG_ERROR("The message mode is not supported!",0,0,0);
        return;
    }
    
    // 确定消息箱，再根据消息箱确定使用的 cache 链表
    switch (pMsg->msg_hdr.mem_store)
    {
        case WMS_MEMORY_STORE_RUIM:
        case WMS_MEMORY_STORE_NV_CDMA:
            ebox = wms_getbox_bytag(pMsg->msg_hdr.tag);
            
            if ((ebox == WMS_MB_NONE) &&
                (pMsg->msg_hdr.tag == WMS_TAG_NONE))
            {
                ebox = wms_getbox_bytag(delcachetag);
            }
            
            if (ebox == WMS_MB_NONE)
            {
                MSG_ERROR("Not supported inde %d !",pMsg->msg_hdr.index,0,0);
                return;
            }
            else if ((ebox == WMS_MB_INBOX) &&
                     (pMsg->msg_hdr.index == 0) &&
                     (pMsg->msg_hdr.mem_store == WMS_MEMORY_STORE_NV_CDMA))
            {
                ebox = WMS_MB_VOICEMAIL;
            }
            break;
            
        default:
            MSG_ERROR("Not supported mem_store !",0,0,0);
            return;
    }
    plist = wms_get_cacheinfolist(ebox);
    if (NULL == plist)
    {
        MSG_ERROR("Get cache list failed !",0,0,0);
        return;
    }
    
    if (pMsg->msg_hdr.tag == WMS_TAG_NONE)
    {
        (void) wms_cacheinfolist_delete(plist, 
                                        pMsg->msg_hdr.mem_store, 
                                        pMsg->msg_hdr.index, 
                                        wms_free_listnodedata);
        return;
    }
    
    // 先分配函数要用到的 buffer
    cl_data = sys_malloc(sizeof(wms_client_ts_data_s_type));
    if (cl_data == NULL)
    {
        MSG_ERROR("memory allocation failed !",0,0,0);
        goto OEMWMS_MsgInfoCacheCbErr;
    }
    (void)MEMSET(cl_data, 0, sizeof(wms_client_ts_data_s_type));
    
    buf = sys_malloc(64);
    if (buf == NULL)
    {
        MSG_ERROR("memory allocation failed !",0,0,0);
        goto OEMWMS_MsgInfoCacheCbErr;
    }
    
    pnode = sys_malloc(sizeof(wms_cache_info_node));
    if (pnode == NULL)
    {
        MSG_ERROR("memory allocation failed !",0,0,0);
        goto OEMWMS_MsgInfoCacheCbErr;
    }
    (void)MEMSET(pnode, 0, sizeof(wms_cache_info_node));
    pnode->index = pMsg->msg_hdr.index;        

    pnode->msg_tag = pMsg->msg_hdr.tag;
    
    info_data = sys_malloc(sizeof(wms_client_bd_s_type));
    if (info_data == NULL)
    {
        MSG_ERROR("memory allocation failed !",0,0,0);
        goto OEMWMS_MsgInfoCacheCbErr;
    }
    (void)MEMSET(info_data, 0, sizeof(wms_client_bd_s_type));

#ifdef FEATURE_CDSMS
    if (pMsg->msg_hdr.tag == WMS_TAG_MO_TEMPLATE)
    {// CDMA 模板消息
        if ((pMsg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
            (pMsg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA))
        {
            // copy the user data into the info cache
            switch (pMsg->u.cdma_template.client_bd.user_data.encoding)
            {
                case WMS_ENCODING_ASCII:
                case WMS_ENCODING_IA5:
                    puserdata = (wms_cdma_user_data_s_type*)&(pMsg->u.cdma_template.client_bd.user_data);
#ifndef WIN32
                    nlen = wms_ts_unpack_ascii(puserdata,
                                pMsg->u.cdma_template.client_bd.user_data.number_of_digits+1,
                                info_data->user_data.data);
#endif
                    info_data->user_data.data_len = nlen;
                    
                    nlen = MIN(WMS_MSG_INFO_CACHE_SIZE-1, info_data->user_data.data_len);
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    info_data->user_data.data[nlen] = 0;
                    pnode->pwszTitle = sys_malloc(nSize);
                    if (NULL == pnode->pwszTitle)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto OEMWMS_MsgInfoCacheCbErr;
                    }
                    (void)MEMSET(pnode->pwszTitle, 0, nSize);
                    
#if defined(FEATURE_SMS_GSM7BIT)
                    {
                        int j;
                        
                        for (j=0; j<nlen; j++)
                        {
                            info_data->user_data.data[j] = GsmToUcs2[info_data->user_data.data[j]];
                            
                            if (info_data->user_data.data[j] == 0)
                            {
                                info_data->user_data.data[j] = 0x20;//' '
                            }
                        }
                    }
#endif            
                    (void)STRTOWSTR((char *)(info_data->user_data.data), 
                                    pnode->pwszTitle,
                                    nSize);
                    break;
                    
                case WMS_ENCODING_UNICODE:
                    info_data->user_data.data_len = pMsg->u.cdma_template.client_bd.user_data.data_len;
                    (void)MEMCPY(info_data->user_data.data,
                                 pMsg->u.cdma_template.client_bd.user_data.data,
                                 info_data->user_data.data_len);
                    OEMWMS_ConvertFromUnicode(info_data);
                    
                    nlen = MIN(WMS_MSG_INFO_CACHE_SIZE-1, info_data->user_data.data_len);
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    //info_data->user_data.data[nlen] = 0;
                    pnode->pwszTitle = sys_malloc(nSize);
                    if (NULL == pnode->pwszTitle)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto OEMWMS_MsgInfoCacheCbErr;
                    }
                    (void)MEMSET(pnode->pwszTitle, 0, nSize);
                    (void)WSTRNCOPYN (pnode->pwszTitle,
                                      (nlen+1),
                                      (AECHAR *)(info_data->user_data.data), 
                                      nlen);
                    break;
                    
                // just copy the user data for other encodings
                default:
                    nlen = MIN(WMS_MSG_INFO_CACHE_SIZE-1, pMsg->u.cdma_template.client_bd.user_data.data_len);
                    
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    pnode->pwszTitle = sys_malloc(nSize);
                    if (NULL == pnode->pwszTitle)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto OEMWMS_MsgInfoCacheCbErr;
                    }
                    for (i=0; i<nlen; i++)
                    {
#ifdef FEATURE_LANG_HEBREW
                        pnode->pwszTitle[i] = (AECHAR)HebrewToUcs2[pMsg->u.cdma_template.client_bd.user_data.data[i]];
                        
                        if (pnode->pwszTitle[i] == 0)
                        {
                            pnode->pwszTitle[i] = (AECHAR)0x20;//' '
                        }
#else
                        pnode->pwszTitle[i] = (AECHAR)pMsg->u.cdma_template.client_bd.user_data.data[i];
#endif                        
                    }
                    pnode->pwszTitle[i] = 0; 
                    break;
            }
            pnode->priority = pMsg->u.cdma_template.client_bd.priority;
        }
        else if (pMsg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
        {
            (void)MEMSET(buf, 0, 64);
            
            if (pMsg->u.cdma_template.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
            {
#ifndef WIN32
                (void)wms_ts_dtmf2ascii((byte)pMsg->u.cdma_template.address.number_of_digits,
                                (byte*)pMsg->u.cdma_template.address.digits, buf);
#endif
            }
            else
            {
                nlen = MIN(WMS_MSG_INFO_CACHE_SIZE-1, pMsg->u.cdma_template.address.number_of_digits);
                (void)MEMCPY(buf, pMsg->u.cdma_template.address.digits, nlen);
            }
            nlen = STRLEN((char *)buf);
            
            if (nlen>0)
            {
                pnode->pszNum = sys_malloc(nlen+1);
                if (NULL == pnode->pszNum)
                {
                    MSG_ERROR("memory allocation failed !",0,0,0);
                    goto OEMWMS_MsgInfoCacheCbErr;
                }
                (void)MEMSET(pnode->pszNum, 0, (nlen+1));
                (void)STRCPY(pnode->pszNum, (char *)buf);
            }
        }
    }
    else
    {// CDMA 消息
        /* Initialized other to 0 */
        (void)MEMSET(&cl_data->u.cdma.other, 0, sizeof(wms_other_parm_s_type));

        // decode the raw bearer data
#ifndef WIN32

        if (wms_ts_decode(&pMsg->u.cdma_message.raw_ts, cl_data) != WMS_OK_S)
        {
            MSG_ERROR("Decoding failed for msg list cache!",0,0,0);
            goto OEMWMS_MsgInfoCacheCbErr;
        }
#endif        
        //if (cl_data->u.cdma.mask & WMS_MASK_BD_NUM_OF_MSGS)
        if ((cl_data->u.cdma.mask & WMS_MASK_BD_NUM_OF_MSGS) &&
            ((pMsg->u.cdma_message.teleservice == WMS_TELESERVICE_VMN_95) ||
             (pMsg->u.cdma_message.teleservice == WMS_TELESERVICE_IS91_VOICE_MAIL) ||
             (pMsg->u.cdma_message.teleservice == WMS_TELESERVICE_MWI)))
        {
            // 语音通知短信仅一条记录，用链表的 nAll 表示消息数
            nVMAll = cl_data->u.cdma.num_messages;
            if (pMsg->msg_hdr.tag == WMS_TAG_MT_NOT_READ)
            {
                nVMNew = nVMAll;
            }
            else
            {
                nVMNew = 0;
            }
        }
        
        if (cl_data->u.cdma.mask & WMS_MASK_BD_MC_TIME)
        {
            pnode->dwTime = OEMWMS_GetTimeFromWmsTimestamp(&(cl_data->u.cdma.mc_time));
        }
    
#ifdef FEATURE_SMS_UDH
        /* Concat Header */
        for (i=0; i<cl_data->u.cdma.user_data.num_headers; i++)
        {
            if (cl_data->u.cdma.user_data.headers[i].header_id == WMS_UDH_CONCAT_8)
            {
                pnode->seq_num = cl_data->u.cdma.user_data.headers[i].u.concat_8.seq_num;
                pnode->total_sm = cl_data->u.cdma.user_data.headers[i].u.concat_8.total_sm;
                pnode->msg_ref = cl_data->u.cdma.user_data.headers[i].u.concat_8.msg_ref;
                break;
            }
            else if (cl_data->u.cdma.user_data.headers[i].header_id == WMS_UDH_CONCAT_16)
            {
                pnode->seq_num = cl_data->u.cdma.user_data.headers[i].u.concat_16.seq_num;
                pnode->total_sm = cl_data->u.cdma.user_data.headers[i].u.concat_16.total_sm;
                pnode->msg_ref = cl_data->u.cdma.user_data.headers[i].u.concat_16.msg_ref;
                break;
            }
        }
        if (((ebox == WMS_MB_OUTBOX) || (ebox == WMS_MB_DRAFT) || (ebox == WMS_MB_RESERVE) || (ebox == WMS_MB_RSVFAILED)) &&
            (pMsg->msg_hdr.mem_store == WMS_MEMORY_STORE_NV_CDMA))
        {
            pnode->seq_num = pMsg->u.cdma_message.concat_8.seq_num;
            pnode->total_sm = pMsg->u.cdma_message.concat_8.total_sm;
            pnode->msg_ref = pMsg->u.cdma_message.concat_8.msg_ref;
        }
#endif /* FEATURE_SMS_UDH */
        if (cl_data->u.cdma.mask & WMS_MASK_BD_MSG_ID)
        {
            pnode->message_id = cl_data->u.cdma.message_id.id_number;
            pnode->msg_type = cl_data->u.cdma.message_id.type;
        }
        
        // 先检查消息有无地址
        if ((pMsg->u.cdma_message.address.number_of_digits) > 0)
        {
            (void)MEMSET(buf, 0, 64);
            
            if (pMsg->u.cdma_message.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
            {
#ifndef WIN32

                (void)wms_ts_dtmf2ascii((byte)pMsg->u.cdma_message.address.number_of_digits,
                                        (byte*)pMsg->u.cdma_message.address.digits, buf);
#endif
			}
            else
            {
                nlen = MIN(WMS_MSG_INFO_CACHE_SIZE, pMsg->u.cdma_message.address.number_of_digits);
                (void)MEMCPY(buf, pMsg->u.cdma_message.address.digits, nlen);
            }
            nlen = STRLEN((char *)buf);
            
            if (nlen>0)
            {
                i = 0;
                
                if ((nlen>3) &&
                    (pnode->msg_tag == WMS_TAG_MT_READ ||
                     pnode->msg_tag == WMS_TAG_MT_NOT_READ))
                {
                    // 去掉 "+86"
                    if (STRNCMP((char *)buf, "+86", 3) == 0)
                    {
                        i = 3;
                        nlen -= 3;
                    }
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                    // 将"886"变成"0"
                    else if (STRNCMP((char *)buf, "886", 3) == 0)
                    {
                        i = 2;
                        buf[i] = '0';
                        nlen -= 2;
                    }
#endif
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
                    // 将"158"变成"0416"
                    // 将"199"变成"0419"
                    // 将"199"变成"0426" guoys modified for movilnet change 2008.10.28
                    else if ((STRNCMP((char *)buf, "158", 3) == 0) ||
                             (STRNCMP((char *)buf, "199", 3) == 0))
                    {
                        int j;
                        
                        buf[nlen+1] = 0;
                        for (j = nlen; j>3; j--)
                        {
                            buf[j] = buf[j-1];
                        }
                        
                        if (buf[2] == '8')
                        {
                            //buf[3] = '6';
                            buf[2] = '1';
                        }
                        else
                        {
                            //buf[3] = '9';
                            buf[2] = '2';
                        }
                        buf[0] = '0';
                        buf[1] = '4';
                        //buf[2] = '1';
                        buf[3] = '6';
                        nlen += 1;
                    }
#endif

#ifdef FEATURE_CARRIER_TAIWAN_APBW
                    if (pnode->msg_tag == WMS_TAG_MT_READ ||
                        pnode->msg_tag == WMS_TAG_MT_NOT_READ)
                    {// 00000000777 去掉多余的 0
                        int j;
                        
                        for (j=i; j<nlen; j++)
                        {
                            if (buf[j] != (AECHAR)'0')
                            {
                                if (STRCMP("777", (char *)&buf[j]) == 0)
                                {
                                    i=j;
                                }
                                break;
                            }
                        }
                    }
#endif
                }
                
                pnode->pszNum = sys_malloc(nlen+1);
                if (NULL == pnode->pszNum)
                {
                    MSG_ERROR("memory allocation failed !",0,0,0);
                    goto OEMWMS_MsgInfoCacheCbErr;
                }
                (void)MEMSET(pnode->pszNum, 0, (nlen+1));
                (void)STRCPY(pnode->pszNum, (char *)&buf[i]);
            }
        }
        
        pnode->priority = cl_data->u.cdma.priority;
        
        // Check for user data
        if ((cl_data->u.cdma.mask & WMS_MASK_BD_USER_DATA) && 
            (NULL == pnode->pszNum))
        {
            // copy the user data into the info cache
            switch (cl_data->u.cdma.user_data.encoding)
            {
                case WMS_ENCODING_ASCII:
                case WMS_ENCODING_IA5:
                    puserdata = (wms_cdma_user_data_s_type*)&cl_data->u.cdma.user_data;
#ifndef WIN32
                    nlen = wms_ts_unpack_ascii(puserdata,
                                cl_data->u.cdma.user_data.number_of_digits+1,
                                info_data->user_data.data);
#endif
                    info_data->user_data.data_len = nlen;
                    
                    nlen = MIN(WMS_MSG_INFO_CACHE_SIZE-1, info_data->user_data.data_len);
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    info_data->user_data.data[nlen] = 0;
                    pnode->pwszTitle = sys_malloc(nSize);
                    if (NULL == pnode->pwszTitle)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto OEMWMS_MsgInfoCacheCbErr;
                    }
                    (void)MEMSET(pnode->pwszTitle, 0, nSize);
                    
#if defined(FEATURE_SMS_GSM7BIT)
                    {
                        int j;
                        
                        for (j=0; j<nlen; j++)
                        {
                            info_data->user_data.data[j] = GsmToUcs2[info_data->user_data.data[j]];
                            
                            if (info_data->user_data.data[j] == 0)
                            {
                                info_data->user_data.data[j] = 0x20;//' '
                            }
                        }
                    }
#endif            
                    (void)STRTOWSTR((char *)(info_data->user_data.data), 
                                    pnode->pwszTitle,
                                    nSize);
                    break;
                    
                case WMS_ENCODING_UNICODE:
                    OEMWMS_ConvertFromUnicode(&cl_data->u.cdma);
                    nlen = MIN(WMS_MSG_INFO_CACHE_SIZE-1, cl_data->u.cdma.user_data.data_len);
                    
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    pnode->pwszTitle = sys_malloc(nSize);
                    if (NULL == pnode->pwszTitle)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto OEMWMS_MsgInfoCacheCbErr;
                    }
                    (void)MEMSET(pnode->pwszTitle, 0, nSize);
                    (void)WSTRNCOPYN (pnode->pwszTitle,
                                      (nlen+1),
                                      (AECHAR *)(cl_data->u.cdma.user_data.data), 
                                      nlen);
                    break;
                    
                // just copy the user data for other encodings
                default:
                    nlen = MIN(WMS_MSG_INFO_CACHE_SIZE-1, cl_data->u.cdma.user_data.data_len);
                    
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    pnode->pwszTitle = sys_malloc(nSize);
                    if (NULL == pnode->pwszTitle)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto OEMWMS_MsgInfoCacheCbErr;
                    }
                    for (i=0; i<nlen; i++)
                    {
#ifdef FEATURE_LANG_HEBREW
                        pnode->pwszTitle[i] = (AECHAR)HebrewToUcs2[cl_data->u.cdma.user_data.data[i]];
                        
                        if (pnode->pwszTitle[i] == 0)
                        {
                            pnode->pwszTitle[i] = (AECHAR)0x20;//' '
                        }
#else
                        pnode->pwszTitle[i] = (AECHAR)cl_data->u.cdma.user_data.data[i];
#endif                        
                    }
                    pnode->pwszTitle[i] = 0; 
                    break;
            }
        }
    }
#endif /* FEATURE_CDSMS */

    sys_free(cl_data);
    cl_data = NULL;
    sys_free(buf);
    buf = NULL;
    sys_free(info_data);
    info_data = NULL;
    
    if ((NULL == pnode->pszNum) && 
        (NULL == pnode->pwszTitle) &&
        (pMsg->u.cdma_message.teleservice == WMS_TELESERVICE_MWI))
    {// MWI 既无号码也无内容, 仅一个数字, 是人为构造消息
        char  strTitle[10] = {0};
        
        SPRINTF(strTitle, "%d", nVMAll);
        nSize = (STRLEN(strTitle)+1)*sizeof(AECHAR);
        
        pnode->pwszTitle = (AECHAR *)sys_malloc(nSize);
        if (NULL != pnode->pwszTitle)
        {
            (void)STRTOWSTR(strTitle, pnode->pwszTitle, nSize);
        }
    }
    
    if ((NULL != pnode->pszNum) || (NULL != pnode->pwszTitle))
    {
        int nRet;
        
        // 无论节点是否存在, 先删除之
        (void) wms_cacheinfolist_delete(plist, 
                                        pMsg->msg_hdr.mem_store, 
                                        pMsg->msg_hdr.index, 
                                        wms_free_listnodedata);
                
        pnode->mem_store = pMsg->msg_hdr.mem_store;
        
        if (plist == wms_get_cacheinfolist(WMS_MB_INBOX))
        {// 因会专门排序，故此处就不必了
            nRet = wms_cacheinfolist_insertsort(plist, pnode, NULL);
        }
        else
        {
            nRet = wms_cacheinfolist_insertsort(plist, pnode, wms_cacheinfolist_nodecompare);
        }
        if (nRet != SUCCESS)
        {
            MSG_ERROR("Add node to list failed !",0,0,0);
            wms_free_listnodedata(pnode);
            sys_free(pnode);
            pnode = NULL;
        }
        else
        {
            if (ebox == WMS_MB_VOICEMAIL)
            {
                plist->nAll = nVMAll;
                plist->nNews = nVMNew;
            }
        }
        
        return;
    }

//--------------------------------函数集中错误处理部分--------------------------
OEMWMS_MsgInfoCacheCbErr:
    if (NULL != cl_data)
    {
        sys_free(cl_data);
    }
    
    if (buf != NULL)
    {
        sys_free(buf);
    }
        
    if (NULL != info_data)
    {
        sys_free(info_data);
    }
    
    if (NULL != pnode)
    {
#ifdef FEATURE_SMS_UDH
        if (NULL != pnode->pItems)
        {
            sys_free(pnode->pItems);
            pnode->pItems = NULL;
        }
#endif        
        wms_free_listnodedata(pnode);
        sys_free(pnode);
    }
}

/*==============================================================================
函数: 
    wms_cacheinfolist_enumbegin
       
说明: 
    cache info 链表枚举初始化函数
       
参数: 
    ebox [in]: 短消息存储箱。
    
返回值:
    none
       
备注:
    
==============================================================================*/
void wms_cacheinfolist_enumbegin(wms_box_e_type ebox)
{
    wms_cache_info_list *plist = NULL;
    
    plist = wms_get_cacheinfolist(ebox);
    
    if (NULL == plist)
    {
        return;
    }
    
    plist->pCur = plist->pHead;
    
    if ((ebox == WMS_MB_RESERVE) ||
        (ebox == WMS_MB_RSVFAILED))
    {
        wms_message_tag_e_type   msg_tag = (ebox == WMS_MB_RESERVE ? WMS_TAG_RESERVE: WMS_TAG_RSVFAILED);
        
        for (;;)
        {
            if ((NULL == plist->pCur) ||
                (plist->pCur->msg_tag == msg_tag))
            {
                break;
            }
            
            plist->pCur = plist->pCur->pNext;
        }
    }
}

/*==============================================================================
函数: 
    wms_cacheinfolist_enumtoend
       
说明: 
    cache info 链表枚举到尾节点
       
参数: 
    ebox [in]: 短消息存储箱。
    
返回值:
    none
       
备注:
    
==============================================================================*/
void wms_cacheinfolist_enumtoend(wms_box_e_type ebox)
{
    wms_cache_info_list *plist= NULL;
    
    plist = wms_get_cacheinfolist(ebox);
    
    if (NULL == plist)
    {
        return;
    }
    
    plist->pCur = plist->pTail;
    
    if ((ebox == WMS_MB_RESERVE) ||
        (ebox == WMS_MB_RSVFAILED))
    {
        wms_message_tag_e_type   msg_tag = (ebox == WMS_MB_RESERVE ? WMS_TAG_RESERVE: WMS_TAG_RSVFAILED);
        
        for (;;)
        {
            if ((NULL == plist->pCur) ||
                (plist->pCur->msg_tag == msg_tag))
            {
                break;
            }
            
            plist->pCur = plist->pCur->pPrev;
        }
    }
}


/*==============================================================================
函数: 
    wms_cacheinfolist_enumnext
       
说明: 
    cache info 链表枚举下一个节点的函数
       
参数: 
    ebox [in]: 短消息存储箱。
    
返回值:
    none
       
备注:
    第一次调用此函数前必须先调用 wms_cacheinfolist_enumbegin() 函数。
==============================================================================*/
wms_cache_info_node *wms_cacheinfolist_enumnext(wms_box_e_type ebox)
{
    wms_cache_info_node *pCur = NULL;
    wms_cache_info_list *plist= NULL;
    
    plist = wms_get_cacheinfolist(ebox);
    
    if (NULL == plist)
    {
        return pCur;
    }
    
    pCur = plist->pCur;
    
    if (NULL != pCur)
    {
        if ((ebox == WMS_MB_RESERVE) ||
            (ebox == WMS_MB_RSVFAILED))
        {
            wms_message_tag_e_type   msg_tag = (ebox == WMS_MB_RESERVE ? WMS_TAG_RESERVE: WMS_TAG_RSVFAILED);
            
            for (;;)
            {
                plist->pCur = plist->pCur->pNext;
                if ((NULL == plist->pCur) ||
                    (plist->pCur->msg_tag == msg_tag))
                {
                    break;
                }
            }
        }
        else
        {
            plist->pCur = pCur->pNext;
        }
        
        return pCur;
    }
    
    return NULL;
}

/*==============================================================================
函数: 
    wms_cacheinfolist_enumprev
       
说明: 
    cache info 链表枚举上一个节点的函数
       
参数: 
    ebox [in]: 短消息存储箱。
    
返回值:
    none
       
备注:
    第一次调用此函数前必须先调用 wms_cacheinfolist_enumbegin() 函数。
==============================================================================*/
wms_cache_info_node *wms_cacheinfolist_enumprev(wms_box_e_type ebox)
{
    wms_cache_info_node *pCur = NULL;
    wms_cache_info_list *plist= NULL;
    
    plist = wms_get_cacheinfolist(ebox);
    
    if (NULL == plist)
    {
        return pCur;
    }
    
    pCur = plist->pCur;
    
    if (NULL != pCur)
    {
        if ((ebox == WMS_MB_RESERVE) ||
            (ebox == WMS_MB_RSVFAILED))
        {
            wms_message_tag_e_type   msg_tag = (ebox == WMS_MB_RESERVE ? WMS_TAG_RESERVE: WMS_TAG_RSVFAILED);
            
            for (;;)
            {
                plist->pCur = plist->pCur->pPrev;
                if ((NULL == plist->pCur) ||
                    (plist->pCur->msg_tag == msg_tag))
                {
                    break;
                }
            }
        }
        else
        {
            plist->pCur = pCur->pPrev;
        }
        
        return pCur;
    }
    
    return NULL;
}

/*==============================================================================
函数: 
    wms_cacheinfolist_enumtoxuhao
       
说明: 
    cache info 链表枚举跳至指定序号的节点的函数
       
参数: 
    ebox [in]: 短消息存储箱。
    xuhao [in]: 指定序号。
    
返回值:
    none
       
备注:
   调用此函数前必须先调用 wms_cacheinfolist_updatexuhao() 函数。
==============================================================================*/
void wms_cacheinfolist_enumtoxuhao(wms_box_e_type ebox, int xuhao)
{
    wms_cache_info_list *plist = NULL;
    wms_cache_info_node *pnode = NULL;
    
    plist = wms_get_cacheinfolist(ebox);
    
    if (NULL == plist)
    {
        return;
    }
    
    if ((ebox == WMS_MB_RSVFAILED) &&
        (xuhao>plist->nOnUIMS))
    {
        return;
    }
    else if ((ebox != WMS_MB_RSVFAILED) &&
        (xuhao>plist->nBranches))
    {
        return;
    }
    
    pnode = plist->pHead;
    
    if ((ebox == WMS_MB_RESERVE) ||
        (ebox == WMS_MB_RSVFAILED))
    {
        wms_message_tag_e_type   msg_tag = (ebox == WMS_MB_RESERVE ? WMS_TAG_RESERVE: WMS_TAG_RSVFAILED);
            
        while (NULL != pnode)
        {
            if ((pnode->xuhao == xuhao) &&
                (pnode->msg_tag == msg_tag))
            {
                plist->pCur = pnode;
                pnode = NULL;
            }
            else
            {
                pnode = pnode->pNext;
            }
        }
    }
    else
    {
        while (NULL != pnode)
        {
            if (pnode->xuhao == xuhao)
            {
                plist->pCur = pnode;
                pnode = NULL;
            }
            else
            {
                pnode = pnode->pNext;
            }
        }
    }
}


/*==============================================================================
函数: 
    wms_cacheinfolist_getcounts
       
说明: 
    函数返回链表节点数及其中新信息项数。
       
参数: 
    box [in]: 短消息存储箱。
    pNew[in]: 用于返回新消息数目的 buffer 指针。
    pOnUIM[in]: 用于返回存于卡上消息数目的 buffer 指针。
    pTotal[in]: 用于返回消息总数目的 buffer 指针。
    
返回值:
    链表节点数
       
备注:

==============================================================================*/
void wms_cacheinfolist_getcounts(wms_box_e_type box, 
                                 uint16 *pNew, 
                                 uint16 *pOnUIM,
                                 uint16 *pTotal)
{
    wms_cache_info_list   *plist = NULL;
    
    plist = wms_get_cacheinfolist(box);
    if (NULL == plist)
    {
        return;
    }
    
    if (NULL != pNew)
    {
        *pNew = plist->nNews;
    }
    
    if (NULL != pOnUIM)
    {
        *pOnUIM = plist->nOnUIMS;
    }
    
    if (NULL != pTotal)
    {
        if (box == WMS_MB_VOICEMAIL)
        {
            *pTotal = plist->nAll;
        }
        else
        {
            *pTotal = plist->nCount;
        }
    }
}
    
/*==============================================================================
函数: 
    wms_cacheinfolist_getnode
       
说明: 
    函数返回符合条件的节点指针。
       
参数: 
    eBox [in]: 消息箱类型
    mem_store [in]: 消息存储位置
    index[in]: 消息在存储空间的索引号。
    
返回值:
    NULL 或 符合条件的节点指针。
       
备注:

==============================================================================*/
wms_cache_info_node *wms_cacheinfolist_getnode(wms_box_e_type  eBox,
                                               wms_memory_store_e_type mem_store, 
                                               uint16 index)
{
    wms_cache_info_list   *pList = NULL;
    wms_cache_info_node   *pnode = NULL;
    
    pList = wms_get_cacheinfolist(eBox);
    if (NULL == pList)
    {
        return pnode;
    }
    
    pnode = pList->pHead;
    while (NULL != pnode)
    {
        if ((pnode->index == index) && (pnode->mem_store == mem_store))
        {
            return pnode;
        }
        
        pnode = pnode->pNext;
    }
    
    return pnode;
}

/*==============================================================================
函数: 
    wms_cacheinfolist_updatenodetag
       
说明: 
    函数返回符合条件的节点指针。
       
参数: 
    mem_store [in]: 消息存储位置
    index[in]: 消息在存储空间的索引号。
    newtag[in]: 消息新的标识

返回值:
    TRUE: 更改成功
    FALSE:更改失败
       
备注:

==============================================================================*/
boolean wms_cacheinfolist_updatenodetag(wms_memory_store_e_type mem_store, 
                                     uint16 index,
                                     wms_message_tag_e_type newtag)
{
    wms_box_e_type ebox = WMS_MB_NONE;
    wms_cache_info_list   *pList = NULL;
    wms_cache_info_node   *pnode = NULL;
    
    // 确定消息箱，在根据消息箱确定使用的 cache 链表
    switch (mem_store)
    {
        case WMS_MEMORY_STORE_RUIM:
        case WMS_MEMORY_STORE_NV_CDMA:
            ebox = wms_getbox_bytag(newtag);
            
            if (ebox == WMS_MB_NONE)
            {
                MSG_ERROR("Not supported tag %d !",newtag,0,0);
                return FALSE;
            }
            else if ((ebox == WMS_MB_INBOX) &&
                     (index == 0) &&
                     (mem_store == WMS_MEMORY_STORE_NV_CDMA))
            {
                ebox = WMS_MB_VOICEMAIL;
            }

            break;
            
        default:
            MSG_ERROR("Not supported mem_store !",0,0,0);
            return FALSE;
    }
    
    pList = wms_get_cacheinfolist(ebox);
    if (NULL == pList)
    {
        return FALSE;
    }
    
    pnode = pList->pHead;
    while (NULL != pnode)
    {
#ifdef FEATURE_SMS_UDH
        if (pnode->pItems != NULL)
        {
            int i;
            wms_cache_info_node *pItemNode = NULL;
            
            for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
            {
                pItemNode = pnode->pItems[i];
                
                if (pItemNode == NULL)
                {
                    continue;
                }
                
                if ((pItemNode->index == index) && (pItemNode->mem_store == mem_store))
                {
                    if ((pItemNode->msg_tag == WMS_TAG_MT_NOT_READ) &&
                        (pItemNode->msg_tag != newtag))
                    {
                        if (ebox == WMS_MB_VOICEMAIL)
                        {
                            pList->nNews = 0;
                        }
                        else
                        {
                            if(pItemNode->total_sm>0)
							{
								if(pItemNode->seq_num%LONGSMS_MAX_PACKAGES == 1)
								{
		                    		if(pList->nNews>0)
									{
		                    			pList->nNews--;
									}
								}
		                	}
							else
							{
								if(pList->nNews>0)
								{
		                			pList->nNews--;
								}
							}
                        }
                    }
                    else if ((pItemNode->msg_tag == WMS_TAG_RESERVE) &&
                        (pItemNode->msg_tag != newtag))
                    {
                    
                        if(pItemNode->total_sm>0)
						{
							if(pItemNode->seq_num%LONGSMS_MAX_PACKAGES == 1)
							{
                        		if(pList->nNews>0)
								{
			            			pList->nNews--;
								}
							}
                    	}
						else
						{
							if(pList->nNews>0)
							{
                    			pList->nNews--;
							}
						}
                    }
                    
                    pItemNode->msg_tag = newtag;
                    return TRUE;
                }
            }
        }
        else
#endif
        if ((pnode->index == index) && (pnode->mem_store == mem_store))
        {
            if ((pnode->msg_tag == WMS_TAG_MT_NOT_READ) &&
                (pnode->msg_tag != newtag))
            {
                if (ebox == WMS_MB_VOICEMAIL)
                {
                    pList->nNews = 0;
                }
                else
                {
                    if(pnode->total_sm>0)
					{
						if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
						{
                    		if(pList->nNews>0)
							{
                    			pList->nNews--;
							}
						}
                	}
					else
					{
						if(pList->nNews>0)
						{
                			pList->nNews--;
						}
					}
                }
            }
            else if ((pnode->msg_tag == WMS_TAG_RESERVE) &&
                     (pnode->msg_tag != newtag))
            {
                if(pnode->total_sm>0)
				{
					if(pnode->seq_num%LONGSMS_MAX_PACKAGES == 1)
					{
                		if(pList->nNews>0)
						{
                			pList->nNews--;
						}
					}
            	}
				else
				{
					if(pList->nNews>0)
					{
            			pList->nNews--;
					}
				}
            }
            
            pnode->msg_tag = newtag;
            return TRUE;
        }
        
        pnode = pnode->pNext;
    }
    
    return FALSE;
}

/*==============================================================================
函数: 
    wms_cacheinfo_deleteruimtemplatecache
       
说明: 
    函数用来删除短消息 cache 链表中位于RUIM的节点。
       
参数: 
    index [in]: 消息在存储空间上的索引号。
    
返回值:
    none
       
备注:
    
==============================================================================*/
void wms_cacheinfo_deleteruimtemplatecache(wms_message_index_type index)
{
    wms_cache_info_list         *plist = NULL;
    
    plist = wms_get_cacheinfolist(WMS_MB_TEMPLATE);
    if (NULL == plist)
    {
        return;
    }
    
    wms_cacheinfolist_delete(plist, WMS_MEMORY_STORE_RUIM, index, wms_free_listnodedata);
}

#ifdef FEATURE_AUTOREPLACE
/*==============================================================================
函数: 
    wms_get_oldestMTmsgIdx
       
说明: 
    函数用于获取最旧 MT 消息所在的存储器及对应索引号.
       
参数: 
    mem_store [in]: 用于返回存储器类型的指针。
    index [in]: 用于返回存储器上记录响应索引号的指针。
    
返回值:
    none
       
备注:

==============================================================================*/
void wms_get_oldestMTmsgIdx(wms_memory_store_e_type *mem_store, wms_message_index_type *index)
{
    wms_cache_info_list *pList = NULL;
    wms_cache_info_node *pnode = NULL;
    uint32    dwTime = 0;
    wms_message_index_type idx = 0;
    wms_memory_store_e_type store;
#ifdef FEATURE_SMS_UDH
    int i;
#endif    
    
    pList = wms_get_cacheinfolist(WMS_MB_INBOX);
    
    if ((NULL == pList) || (NULL == mem_store) || (NULL == index))
    {
        return;
    }
    
    pnode = pList->pHead;
    if (NULL != pnode)
    {
        dwTime = pnode->dwTime;
        idx = pnode->index;
        store = pnode->mem_store;
    }
    while (NULL != pnode)
    {
        if (pnode->dwTime < dwTime)
        {
            dwTime = pnode->dwTime;
            idx = pnode->index;
            store = pnode->mem_store;
        }
        
#ifdef FEATURE_SMS_UDH
        if (pnode->pItems != NULL)
        {
            for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
            {
                if ((pnode->pItems[i] != NULL) &&
                    (pnode->pItems[i]->dwTime < dwTime))
                {
                    dwTime = pnode->pItems[i]->dwTime;
                    idx = pnode->pItems[i]->index;
                    store = pnode->pItems[i]->mem_store;
                }
            }
        }
#endif            
        
        pnode = pnode->pNext;
    }
    
    if (dwTime != 0)
    {
        *mem_store = store;
        *index = idx;
    }
}
#endif /*FEATURE_AUTOREPLACE*/


/*==============================================================================
函数: 
    wms_getbox_bytag
       
说明: 
    函数用于依据消息标记来获取其对应使用的邮箱。
       
参数: 
    mem_store [in]: 用于返回存储器类型的指针。
    index [in]: 用于返回存储器上记录响应索引号的指针。
    
返回值:
    none
       
备注:

==============================================================================*/
static wms_box_e_type wms_getbox_bytag(wms_message_tag_e_type tag)
{
    wms_box_e_type box = WMS_MB_NONE;
    
    switch (tag)
    {
        case WMS_TAG_PHRASE:
        case WMS_TAG_MO_TEMPLATE:
            box = WMS_MB_TEMPLATE;
            break;
            
        case WMS_TAG_MT_READ:
        case WMS_TAG_MT_NOT_READ:
            // 注意语音信箱需外部特殊判断
            box = WMS_MB_INBOX;
            break;
            
        case WMS_TAG_MO_SENT:
        case WMS_TAG_MO_NOT_SENT:
            box = WMS_MB_OUTBOX;
            break;
            
        case WMS_TAG_MO_DRAFT:
            box = WMS_MB_DRAFT;
            break;

        case WMS_TAG_RESERVE:
            box = WMS_MB_RESERVE;
            break;
            
        case WMS_TAG_RSVFAILED:
            box = WMS_MB_RSVFAILED;
            break;
            
        default:
            break;
    }
    
    return box;
}

#endif


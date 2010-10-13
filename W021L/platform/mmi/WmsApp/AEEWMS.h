#ifndef AEEWMS_H
#define AEEWMS_H

/*============================================================================
FILE:  AEEWMS.h

SERVICES:  AEE IWMS Interface

GENERAL DESCRIPTION:
  This module supports the AEE interface for supporting WMS (Wireless
  Messaging Services), including SMS and EMS.

INTERFACE DOCUMENTATION: 80-V5316-1 [QCT Static Extension]

        Copyright ? 2003,2004,2005,2006,2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEEWMS.h#30 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/07   HQ      Updated copyright info for 2007.
11/15/06   PMD     Updated File Comments with Interface Documentation
                   Updated File Copyright Information
10/17/05   PMD     Updated Copyright Information.
10/17/05   PMD     Added IWMS_GetRetryPeriod API
10/17/05   PMD     Added IWMS_DbgGetRetryInterval and IWMS_DbgSetRetryInterval APIs
03/10/05   PMD     Added IWMS_BcMmMsgDeleteAllIndication API
02/01/05   PMD     Added IWMS_BcMmGetReadingPref() and IWMS_BcMmSetReadingPref()
11/03/04   PMD     Added IWMS_CfgCheckWapPushMsg function
09/22/04   Rex     Added IWMS_CfgCheckCphsMsg and IWMS_CfgLocateVoicemailLine2
06/19/04   PMD     Added IWMS_BcMmMsgDeleteIndication API
04/22/04   PMD     Added IWMS_MsgGetCacheInfo API
03/24/04   PMD     Modified IWMS_SetPrimaryClient and IWMS_SetMemoryFull APIs
03/15/04   AP      Fixed definition of IWMS_TsConvertAutoToCdmaGwMessage.
03/09/04   PMD     Added IWMS_SetPrimaryClient and IWMS_SetMemoryFull APIs
01/30/04   PMD     Added IWMS_TsEncodeDCS and IWMS_TsDecodeDCS
01/26/04   PMD     Added IWMS_TsGetHeaderLength()		  
01/16/03   PMD	   Added IWMS_MsgSetRetryPeriod()
01/13/04   HQ      Added IWMS_CfgCheckCDMADuplicate().
01/07/04   PMD     Added IWMS_BcMmSetPriorityAllServices()
12/01/03   PMD     Added Function to Register Parsing Callback
12/01/03   PMD     Removed RegisterMsgInfoCacheCb Function
11/03/03   PMD     Added Support for Automatic MO SMS Functions
10/22/03   PMD     Added New Function in IWMS to Register MsgInfoCache Callback
10/20/03   PMD     Featurization Cleanup in IWMS
10/20/03   PMD     Added WMS Transport Service (TS) group functions
10/19/03   PMD     Added Multimode Cell Broadcast Functions
                   Added Multimode BC Registration
10/03/03   AP      Added check for voicemail contents.
09/22/03   PMD     Added Functions to Clear SMMA and Reset Reference #
09/19/03   PMD     Added Functions for Get and Set GW Domain Preference
07/29/03   AP      Added DC registration.
06/06/03   SUN     Fixed compile error
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
02/27/03   SUN     Fixed Typo for DeleteAllTemplate
02/14/03   AP      More support for bc group.
12/16/02   AP      Feature-ized bc, dc and dbg groups.
10/24/02   SUN     Removed Notifier, Code Cleanup
08/22/02   HQ      Initial version.
=============================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "AEE.h"
#include "AEEComdef.h"

/* WMS API definitions
*/
#include "wms.h"


/* Class ID files
*/
#include "WMS.BID"




/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/


/* Event data structures are defined in wms.h:
**  - wms_cfg_event_info_s_type
**  - wms_msg_event_info_s_type
**  - wms_dc_event_info_s_type
**  - wms_bc_event_info_s_type
**  - wms_dbg_event_info_s_type
*/


/* ============== Callback definitions ================ */

typedef struct
{
  wms_cmd_id_e_type       cmd;
  void                    *user_data;
  wms_cmd_err_e_type      cmd_err;
} AEEWMS_CmdCbData;




/*===========================================================================

                         CLASS DEFINITIONS

===========================================================================*/


/* Forward typedef reference of the objects.
*/
typedef struct IWMS IWMS;



/* IWMS interface class definition
*/
AEEINTERFACE(IWMS)
{
  INHERIT_IQueryInterface(IWMS);

  int  (*Init)(IWMS                  *po,
               wms_client_type_e_type cl,
               wms_client_id_type     *pCl
              );

  int    (*Activate)(
                      IWMS                             * po,
                      wms_client_id_type                 cl
                    );

  /* For completeness
  */
  int    (*Deactivate)(
                          IWMS                             * po,
                          wms_client_id_type                 cl
                      );

  int    (*RegisterCfgCb)(
                           IWMS                  *po,
                           wms_client_id_type    cl,
                           wms_cfg_event_cb_type cb
                         );

  int    (*RegisterMsgCb)(
                           IWMS                  *po,
                           wms_client_id_type    cl,
                           wms_msg_event_cb_type cb
                         );

  int    (*RegisterBcCb)(
                          IWMS                   *po,
                          wms_client_id_type     cl,
                          wms_bc_event_cb_type   cb
                        );

  int    (*RegisterBcMmCb)(
                            IWMS                    *po,
                            wms_client_id_type      cl,
                            wms_bc_mm_event_cb_type cb
                          );

  int    (*RegisterDcCb)(
                           IWMS                  *po,
                           wms_client_id_type    cl,
                           wms_dc_event_cb_type  cb
                         );

  int (*RegisterParsingCb)(
                            IWMS                     *po,
                            wms_client_id_type        cl,
                            wms_cl_parse_msg_cb_type  cb
                          );

  int (*SetPrimaryClient)(
                            IWMS                     *po,
                            wms_client_id_type        client_id,
                            AEECallback              *cmd_cb,
                            const void               *user_data,
                            boolean                   set_primary,
                            boolean                   use_client_memory
                          );
  int (*SetMemoryFull)(
                            IWMS                     *po,
                            wms_client_id_type        client_id,
                            AEECallback              *cmd_cb,
                            const void               *user_data,
                            boolean                   memory_full
                          );

  int (*SetMultiSend)(
                            IWMS                     *po,
                            wms_client_id_type        client_id,
                            AEECallback              *cmd_cb,
                            const void               *user_data,
                            boolean                   multisend
                          );
                          
#ifdef FEATURE_AUTOREPLACE
  int (*SetAutoreplace)(
                            IWMS                     *po,
                            wms_client_id_type        client_id,
                            AEECallback              *cmd_cb,
                            const void               *user_data,
                            boolean                   autoreplace
                          );
#endif
  
  /* Configuration group functions
  */
  int    (*CfgSetRoutes)(
                          IWMS                             * po,
                          wms_client_id_type                 cl,
                          AEECallback                      * cmd_cb,
                          const void                       * user_data,
                          const wms_routes_s_type          * routes_ptr
                        );

  int    (*CfgGetRoutes)(
                          IWMS                             * po,
                          wms_client_id_type                 cl,
                          AEECallback                      * cmd_cb,
                          const void                       * user_data
                        );

  int    (*CfgGetMemoryStatus)(
                                IWMS                             * po,
                                wms_client_id_type                 cl,
                                AEECallback                      * cmd_cb,
                                const void                       * user_data,
                                wms_memory_store_e_type          mem_store,
                                wms_message_tag_e_type           tag
                              );

  int    (*CfgGetMessageList)(
                                IWMS                             * po,
                                wms_client_id_type                 cl,
                                AEECallback                      * cmd_cb,
                                const void                       * user_data,
                                wms_memory_store_e_type          mem_store,
                                wms_message_tag_e_type           tag
                              );

  int    (*CfgGetStatusReportList)(
                                   IWMS                             * po,
                                   wms_client_id_type                 cl,
                                   AEECallback                      * cmd_cb,
                                   const void                       * user_data,
                                   wms_memory_store_e_type          mem_store
                                  );

  wms_message_index_type (*CfgGetMsgIndex)(
                                   IWMS                             * po,
                                   wms_message_number_type          stsRptIndex
                                  );
#if 0
  boolean (*CfgCheckVoiceMail)(
                               IWMS                         * po,
                               wms_message_mode_e_type      msgMode,
                               const wms_raw_ts_data_s_type *rawTs,
                               boolean                      *isActive,
                               uint8                        *count
                              );
#else
  boolean (*CfgCheckVoiceMail)(
                               IWMS                         * po,
                               wms_message_mode_e_type      msgMode,
                               const wms_client_message_s_type *pCltMsg,
                               boolean                      *isActive,
                               uint8                        *count
                              );
#endif

  boolean (*CfgCheckCphsMsg)(
                               IWMS                         * po,
                               const wms_address_s_type			* addr
                              );

  boolean (*CfgLocateVoicemailLine2)(
                               IWMS						              * po,
                               uint32			            			* index_line2
                              );
  								

  boolean (*CfgCheckCDMADuplicate)(
                               IWMS                         * po,
                               const wms_client_message_s_type *pMsg
                              );

  boolean (*CfgCheckWapPushMsg)(
                               IWMS                         * po,
                               const wms_client_message_s_type *pMsg
                              );

  int    (*CfgSetGWDomainPref)(
                                   IWMS                             * po,
                                   wms_client_id_type                 cl,
                                   AEECallback                      * cmd_cb,
                                   const void                       * user_data,
                                   wms_gw_domain_pref_e_type         gw_domain_pref
                                  );

  int    (*CfgGetGWDomainPref)(
                                   IWMS                             * po,
                                   wms_client_id_type                 cl,
                                   AEECallback                      * cmd_cb,
                                   const void                       * user_data
                                  );

  /* Message group functions
  */
  int    (*MsgSend)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_send_mode_e_type                  send_mode,
                          const wms_client_message_s_type     * message_ptr
                    );

  int    (*MsgAck)  (
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          const wms_ack_info_s_type           * ack_info_ptr
                    );

  int    (*MsgRead)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type             mem_store,
                          wms_message_index_type              index
                    );

  int    (*MsgWrite)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_write_mode_e_type                 write_mode,
                          const wms_client_message_s_type     * message_ptr
                    );

  int    (*MsgDelete)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type             mem_store,
                          wms_message_index_type              index
                      );
                      
  int    (*MsgDeleteBox)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_deletebox_e_type                box_deltype
                      );
                      
  int    (*MsgCopy)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_msg_copy_type                   * pmsg_copy
                      );
                      
#if defined(FEATURE_UIM_TOOLKIT)
  int    (*RefreshRUIMSMS)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );
#endif
  
  int    (*MsgDeleteAll)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type             mem_store,
                          wms_message_tag_e_type              tag
                      );

  int    (*MsgModifyTag)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type             mem_store,
                          wms_message_index_type              index,
                          wms_message_tag_e_type              tag
                        );

  int    (*MsgReadTemplate)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type             mem_store,
                          wms_message_index_type              index
                    );

  int    (*MsgWriteTemplate)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_write_mode_e_type               write_mode,
                          const wms_client_message_s_type     * message_ptr
                    );

  int    (*MsgDeleteTemplate)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type             mem_store,
                          wms_message_index_type              index
                      );

  int    (*MsgDeleteAllTemplate)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type               mem_store
                      );
  int    (*MsgReadStatusReport)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type             mem_store,
                          wms_message_index_type              index
                    );

  int    (*MsgWriteStatusReport)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_write_mode_e_type                 write_mode,
                          const wms_client_message_s_type     * message_ptr
                    );

  int    (*MsgDeleteStatusReport)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type             mem_store,
                          wms_message_index_type              index
                      );

  int    (*MsgDeleteAllStatusReport)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_memory_store_e_type             mem_store
                      );

  int    (*MsgSetAutoCDMAParams)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          wms_cdma_template_s_type            * cdma_params
                      );

  int    (*MsgSetAutoGWParams)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          wms_gw_template_s_type              * gw_params
                      );

  int    (*MsgGetAutoCDMAParams)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          wms_cdma_template_s_type            * cdma_params
                      );

  int    (*MsgGetAutoGWParams)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          wms_gw_template_s_type              * gw_params
                      );

  int    (*MsgGetRetryPeriod)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          uint32                              * period
                      );

  int    (*MsgSetRetryPeriod)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          uint32                                period
                      );



  int    (*MsgGetCacheInfo)(
                          IWMS                     *pMe,
                          wms_memory_store_e_type   mem_store,
                          wms_message_index_type    index,
                          wms_message_tag_e_type    tag,
                          uint8                    *cache_ptr
                      );



  /* Dedicated Channel group functions
  */
  int    (*DcEnableAutoDisconnect)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          uint32                              timeout
                      );

  int    (*DcDisableAutoDisconnect)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*DcConnect)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_dc_so_e_type                    so
                      );

  int    (*DcDisconnect)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  /* Debug group functions
  */
  int    (*DbgResetTLSeqNum)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*DbgSetMsgRefNum)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*DbgClearSMMAFlag)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*DbgGetRetryInterval)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*DbgSetRetryInterval)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          uint32                                retry_interval
                      );

  /* Transport Service (TS) Group functions 
  */
  int    (*TsEncode)(
                          IWMS                                * po,
                          wms_client_ts_data_s_type           * ClientTsDataPtr,
                          wms_raw_ts_data_s_type              * RawTsDataPtr
                      );

  int    (*TsDecode)(
                          IWMS                                * po,
                          wms_raw_ts_data_s_type              * RawTsDataPtr,
                          wms_client_ts_data_s_type           * ClientTsDataPtr
                      );

  uint8  (*TsGetHeaderLength)(
                          IWMS                                * po,
                          wms_udh_s_type                      * udhPtr
                      );

  int    (*TsBcdToAscii)(
                          IWMS                                * po,
                          uint8                               * InputPtr,
                          uint8                                 length,
                          uint8                               * OutputPtr
                      );

  int    (*TsAsciiToBcd)(
                          IWMS                                * po,
                          uint8                               * InputPtr,
                          uint8                                 length,
                          uint8                               * OutputPtr
                      );

  int    (*TsAsciiToDefault)(
                          IWMS                                * po,
                          uint8                               * InputPtr,
                          uint32                                 length,
                          uint8                               * OutputPtr
                      );

  int    (*TsDefaultToAscii)(
                          IWMS                                * po,
                          uint8                               * InputPtr,
                          uint32                                 length,
                          uint8                               * OutputPtr
                      );

  int    (*TsDecodeRelativeTime)(
                          IWMS                                * po,
                          uint8                                 time,
                          wms_timestamp_s_type                * TimeStampPtr
                      );

  uint8  (*TsEncodeRelativeTime)(
                          IWMS                                * po,
                          wms_timestamp_s_type                * TimeStampPtr
                      );

  uint8  (*TsDecodeDCS)(
                          IWMS                                * po,
                          uint8                               * data,
                          wms_gw_dcs_s_type                   * dcs
                      );

  uint8  (*TsEncodeDCS)(
                          IWMS                                * po,
                          wms_gw_dcs_s_type                   * dcs,
                          uint8                               * data
                      );

  byte    (*TsPackAscii)(
                          IWMS                                * po,
                          char                                * AsciiPtr,
                          byte                                * DataPtr,
                          byte                                * DataLengthPtr,
                          byte                                * PaddingBitsPtr
                      );

  byte    (*TsUnpackAscii)(
                          IWMS                                * po,
                          wms_cdma_user_data_s_type           * UserDataPtr,
                          byte                                  length,
                          byte                                * OutputPtr
                      );

  byte   (*TsDtmfToAscii)(
                          IWMS                                * po,
                          byte                                  length,
                          byte                                * InputPtr,
                          byte                                * OutputPtr
                      );

  byte   (*TsAsciiToDtmf)(
                          IWMS                                * po,
                          char                                * InputPtr,
                          byte                                * OutputPtr
                      );

  int    (*TsConvertCdmaGwMessageToAuto)(
                          IWMS                                * po,
                          wms_client_message_s_type           * InputPtr,
                          uint8                                 length,
                          wms_auto_message_s_type             * OutputPtr
                      );
  
  int    (*TsConvertAutoToCdmaGwMessage)(
                          IWMS                                * po,
                          wms_client_id_type                    clientId,
                          wms_message_mode_e_type               MsgMode,
                          wms_auto_message_s_type             * InputPtr,
                          wms_client_message_s_type           * OutputPtr
                      );
  
  /* Broadcast group functions
  */
  int    (*BcGetConfig)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*BcGetPref)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );


  int    (*BcSetPref)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_bc_pref_e_type                  pref
                      );

  int    (*BcGetTable)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*BcSelectService)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_bc_service_id_s_type            * srv_id,
                          boolean                             selected,
                          wms_priority_e_type                 priority
                      );

  int    (*BcGetAllServiceIds)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*BcGetServiceInfo)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_bc_service_id_s_type            * srv_id
                      );

  int    (*BcAddService)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_bc_service_info_s_type          * srv_info
                      );

  int    (*BcDeleteService)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_bc_service_id_s_type            * srv_id
                      );

  int    (*BcChangeLabel)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_bc_service_id_s_type            * srv_id,
                          char                                * label_ptr
                      );

  int    (*BcDeleteAllServices)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*BcSetPriorityForAll)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_priority_e_type                 priority
                      );

  /* Multimode Broadcast group functions
  */
  int    (*BcMmGetConfig)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode
                      );

  int    (*BcMmGetPref)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode
                      );


  int    (*BcMmSetPref)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode,
                          wms_bc_pref_e_type                    pref
                      );

  int    (*BcMmGetTable)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode
                      );

  int    (*BcMmSelectService)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode,
                          wms_bc_mm_srv_id_type               * srv_id,
                          boolean                               selected
                      );

  int    (*BcMmGetAllServiceIds)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode
                      );

  int    (*BcMmGetServiceInfo)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode,
                          wms_bc_mm_srv_id_type               * srv_id
                      );

  int    (*BcMmAddServices)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode,
                          uint8                                 num_entries,
                          wms_bc_mm_service_info_s_type       * entries
                      );

  int    (*BcMmDeleteServices)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode,
                          uint8                                 num_entries,
                          wms_bc_mm_srv_id_type               * srv_ids
                      );

  int    (*BcMmChangeServiceInfo)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode,
                          wms_bc_mm_service_info_s_type       * srv_info_ptr
                      );

  int    (*BcMmDeleteAllServices)(
                          IWMS                                * po,
                          wms_client_id_type                    cl,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode
                      );

  int    (*BcMmSelectAllServices)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode,
                          boolean                               selected
                      );

  int    (*BcMmSetPriorityAllServices)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode,
                          wms_priority_e_type                   priority
                      );

  int    (*BcMmMsgDeleteIndication)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_gw_cb_page_header_s_type        * cbPageHeader
                      );
  
  int    (*BcMmMsgDeleteAllIndication)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data
                      );

  int    (*BcMmGetReadingPref)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode
                      );


  int    (*BcMmSetReadingPref)(
                          IWMS                                * po,
                          wms_client_id_type                    client_id,
                          AEECallback                         * cmd_cb,
                          const void                          * user_data,
                          wms_message_mode_e_type               message_mode,
                          boolean                               reading_advised,
                          boolean                               reading_optional
                      );

};
/* end of IWMS interface definition */


/* Defines for accessing the IWMS class
*/
#define IWMS_AddRef(p)                 AEEGETPVTBL(p,IWMS)->AddRef(p)
#define IWMS_Release(p)                AEEGETPVTBL(p,IWMS)->Release(p)
#define IWMS_QueryInterface(p, clsid, pp)   \
          AEEGETPVTBL(p,IWMS)->QueryInterface(p,clsid,pp)

#define IWMS_Init(p, clientType,pClientId) \
       AEEGETPVTBL(p,IWMS)->Init(p, clientType,pClientId)

#define IWMS_Activate(p,cl)  \
       AEEGETPVTBL(p,IWMS)->Activate(p,cl)

#define IWMS_Deactivate(p,cl)  AEEGETPVTBL(p,IWMS)->Deactivate(p,cl)

/* Configuration group
*/
#define IWMS_RegisterCfgCb(p,clientId, cfgCb) \
          AEEGETPVTBL(p,IWMS)->RegisterCfgCb(p, clientId,cfgCb)

#define IWMS_RegisterMsgCb(p,clientId,msgCb) \
         AEEGETPVTBL(p,IWMS)->RegisterMsgCb(p, clientId, msgCb)

#define IWMS_RegisterBcCb(p,clientId,bcCb) \
         AEEGETPVTBL(p,IWMS)->RegisterBcCb(p, clientId, bcCb)

#define IWMS_RegisterBcMmCb(p,clientId,bcMmCb) \
         AEEGETPVTBL(p,IWMS)->RegisterBcMmCb(p, clientId, bcMmCb)

#define IWMS_RegisterDcCb(p,clientId,dcCb) \
         AEEGETPVTBL(p,IWMS)->RegisterDcCb(p, clientId, dcCb)

#define IWMS_RegisterParsingCb(p,clientId,parsingCb) \
         AEEGETPVTBL(p,IWMS)->RegisterParsingCb(p, clientId, parsingCb)

#define IWMS_SetPrimaryClient(p, client_id, cmd_cb, user_data, set_primary, use_client_memory) \
         AEEGETPVTBL(p,IWMS)->SetPrimaryClient(p, client_id, cmd_cb, user_data, set_primary, use_client_memory)

#define IWMS_SetMemoryFull(p, client_id, cmd_cb, user_data, memory_full) \
         AEEGETPVTBL(p,IWMS)->SetMemoryFull(p, client_id, cmd_cb, user_data, memory_full)

#define IWMS_SetMultiSend(p, client_id, cmd_cb, user_data, multisend) \
         AEEGETPVTBL(p,IWMS)->SetMultiSend(p, client_id, cmd_cb, user_data, multisend)
         
#ifdef FEATURE_AUTOREPLACE
#define IWMS_SetAutoreplace(p, client_id, cmd_cb, user_data, autoreplace) \
         AEEGETPVTBL(p,IWMS)->SetAutoreplace(p, client_id, cmd_cb, user_data, autoreplace)
#endif

#define IWMS_CfgSetRoutes(p, clientId, cmd_cb, user_data, routes_ptr) \
           AEEGETPVTBL(p,IWMS)->CfgSetRoutes(p, clientId,cmd_cb, user_data, routes_ptr)

#define IWMS_CfgGetRoutes(p, clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->CfgGetRoutes(p, clientId,cmd_cb, user_data)

#define IWMS_CfgGetMemoryStatus(p, clientId, cmd_cb, user_data, mem_store, tag) \
           AEEGETPVTBL(p,IWMS)->CfgGetMemoryStatus(p, clientId,cmd_cb, user_data, \
                                                   mem_store, tag)

#define IWMS_CfgGetMessageList(p, clientId,cmd_cb, user_data, mem_store, tag) \
           AEEGETPVTBL(p,IWMS)->CfgGetMessageList(p, clientId,cmd_cb, user_data, \
                                        mem_store, tag)

#define IWMS_CfgGetStatusReportList(p, clientId,cmd_cb, user_data, mem_store) \
           AEEGETPVTBL(p,IWMS)->CfgGetStatusReportList(p, clientId,cmd_cb, user_data, \
                                        mem_store)

#define IWMS_CfgGetMsgIndex(p, stsRptIndex) \
           AEEGETPVTBL(p,IWMS)->CfgGetMsgIndex(p,stsRptIndex)

#define IWMS_CfgCheckVoiceMail(p, msgMode, rawTs, isActive, count) \
           AEEGETPVTBL(p,IWMS)->CfgCheckVoiceMail(p, msgMode, rawTs, isActive, \
                                                  count)
#define IWMS_CfgCheckCphsMsg(p, addr) \
		   AEEGETPVTBL(p,IWMS)->CfgCheckCphsMsg(p, addr)

#define IWMS_CfgLocateVoicemailLine2(p, index) \
		   AEEGETPVTBL(p,IWMS)->CfgLocateVoicemailLine2(p, index)

#define IWMS_CfgCheckCDMADuplicate(p, pMsg) \
           AEEGETPVTBL(p,IWMS)->CfgCheckCDMADuplicate(p, pMsg)

#define IWMS_CfgCheckWapPushMsg(p, pMsg) \
           AEEGETPVTBL(p,IWMS)->CfgCheckWapPushMsg(p, pMsg)

#define IWMS_CfgSetGWDomainPref(p, clientId, cmd_cb, user_data, gw_domain_pref) \
           AEEGETPVTBL(p,IWMS)->CfgSetGWDomainPref(p, clientId, cmd_cb, user_data, \
                                        gw_domain_pref)

#define IWMS_CfgGetGWDomainPref(p, clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->CfgGetGWDomainPref(p, clientId, cmd_cb, user_data)


/* Message group
*/
#define IWMS_MsgSend(p, clientId,cmd_cb, user_data, send_mode, message_ptr) \
           AEEGETPVTBL(p,IWMS)->MsgSend(p,clientId, cmd_cb, user_data, \
                                        send_mode, message_ptr)

#define IWMS_MsgAck(p, clientId,cmd_cb, user_data, ack_info_ptr ) \
           AEEGETPVTBL(p,IWMS)->MsgAck(p, clientId,cmd_cb, user_data, \
                                        ack_info_ptr)

#define IWMS_MsgRead(p,clientId, cmd_cb, user_data, mem_store, index) \
           AEEGETPVTBL(p,IWMS)->MsgRead(p, clientId,cmd_cb, user_data, \
                                        mem_store, index)

#define IWMS_MsgWrite(p, clientId,cmd_cb, user_data, write_mode, message_ptr) \
           AEEGETPVTBL(p,IWMS)->MsgWrite(p,clientId, cmd_cb, user_data, \
                                        write_mode, message_ptr )

#define IWMS_MsgDelete(p,clientId, cmd_cb, user_data, mem_store, index) \
           AEEGETPVTBL(p,IWMS)->MsgDelete(p,clientId, cmd_cb, user_data, \
                                        mem_store, index)

#define IWMS_MsgDeleteBox(p,clientId, cmd_cb, user_data, box_deltype) \
           AEEGETPVTBL(p,IWMS)->MsgDeleteBox(p,clientId, cmd_cb, user_data, \
                                        box_deltype)
                                        
#define IWMS_MsgCopy(p,clientId, cmd_cb, user_data, pmsg_copy) \
           AEEGETPVTBL(p,IWMS)->MsgCopy(p,clientId, cmd_cb, user_data, \
                                        pmsg_copy)
                                        
#if defined(FEATURE_UIM_TOOLKIT)
#define IWMS_RefreshRUIMSMS(p,clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->RefreshRUIMSMS(p,clientId, cmd_cb, user_data)
#endif                                        

#define IWMS_MsgDeleteAll(p, clientId,cmd_cb, user_data, mem_store, tag) \
           AEEGETPVTBL(p,IWMS)->MsgDeleteAll(p,clientId, cmd_cb, user_data, \
                                        mem_store, tag)

#define IWMS_MsgModifyTag(p, clientId,cmd_cb, user_data, mem_store, index, tag) \
           AEEGETPVTBL(p,IWMS)->MsgModifyTag(p, clientId,cmd_cb, user_data, \
                                        mem_store, index, tag)

#define IWMS_MsgReadTemplate(p, clientId,cmd_cb, user_data, mem_store, index) \
           AEEGETPVTBL(p,IWMS)->MsgReadTemplate(p,clientId, cmd_cb, user_data, \
                                        mem_store, index)

#define IWMS_MsgWriteTemplate(p, clientId,cmd_cb, user_data, write_mode, message_ptr) \
           AEEGETPVTBL(p,IWMS)->MsgWriteTemplate(p,clientId, cmd_cb, user_data, \
                                        write_mode, message_ptr)

#define IWMS_MsgDeleteTemplate(p,clientId, cmd_cb, user_data, mem_store, index) \
           AEEGETPVTBL(p,IWMS)->MsgDeleteTemplate(p,clientId, cmd_cb, user_data, \
                                          mem_store, index)

#define IWMS_MsgDeleteAllTemplate(p, clientId,cmd_cb, user_data, mem_store) \
           AEEGETPVTBL(p,IWMS)->MsgDeleteAllTemplate(p, clientId,cmd_cb, user_data, \
                                          mem_store)

#define IWMS_MsgReadStatusReport(p,clientId, cmd_cb, user_data, mem_store, index) \
           AEEGETPVTBL(p,IWMS)->MsgReadStatusReport(p, clientId,cmd_cb, user_data, \
                                        mem_store, index)

#define IWMS_MsgWriteStatusReport(p, clientId,cmd_cb, user_data, write_mode, message_ptr) \
           AEEGETPVTBL(p,IWMS)->MsgWriteStatusReport(p,clientId, cmd_cb, user_data, \
                                        write_mode, message_ptr )

#define IWMS_MsgDeleteStatusReport(p,clientId, cmd_cb, user_data, mem_store, index) \
           AEEGETPVTBL(p,IWMS)->MsgDeleteStatusReport(p,clientId, cmd_cb, user_data, \
                                        mem_store, index)

#define IWMS_MsgDeleteAllStatusReport(p, clientId,cmd_cb, user_data, mem_store) \
           AEEGETPVTBL(p,IWMS)->MsgDeleteAllStatusReport(p,clientId, cmd_cb, user_data, \
                                          mem_store)

#define IWMS_MsgSetAutoCDMAParams(p, clientId, params) \
           AEEGETPVTBL(p,IWMS)->MsgSetAutoCDMAParams(p, clientId, params)

#define IWMS_MsgSetAutoGWParams(p, clientId, params) \
           AEEGETPVTBL(p,IWMS)->MsgSetAutoGWParams(p, clientId, params)

#define IWMS_MsgGetAutoCDMAParams(p, clientId, params) \
           AEEGETPVTBL(p,IWMS)->MsgGetAutoCDMAParams(p, clientId, params)

#define IWMS_MsgGetAutoGWParams(p, clientId, params) \
           AEEGETPVTBL(p,IWMS)->MsgGetAutoGWParams(p, clientId, params)

#define IWMS_MsgGetRetryPeriod(p, clientId, period) \
           AEEGETPVTBL(p,IWMS)->MsgGetRetryPeriod(p, clientId, period)

#define IWMS_MsgSetRetryPeriod(p, clientId, period) \
           AEEGETPVTBL(p,IWMS)->MsgSetRetryPeriod(p, clientId, period)

#define IWMS_MsgGetCacheInfo(p, mem_store, index, tag, cache_ptr) \
           AEEGETPVTBL(p,IWMS)->MsgGetCacheInfo(p, mem_store, index, tag, cache_ptr)

/* Dedicated Channel group
*/
#define IWMS_DcEnableAutoDisconnect(p, clientId,cmd_cb, user_data, timeout) \
           AEEGETPVTBL(p,IWMS)->DcEnableAutoDisconnect(p,clientId, cmd_cb, user_data, \
                                          timeout)

#define IWMS_DcDisableAutoDisconnect(p,clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->DcDisableAutoDisconnect(p, clientId,cmd_cb, user_data)

#define IWMS_DcConnect(p, clientId,cmd_cb, user_data, so) \
           AEEGETPVTBL(p,IWMS)->DcConnect(p, clientId,cmd_cb, user_data, \
                                          so)

#define IWMS_DcDisconnect(p, clientId,cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->DcDisconnect(p,clientId, cmd_cb, user_data)

/* Debug group
*/
#define IWMS_DbgResetTLSeqNum(p,clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->DbgResetTLSeqNum(p, clientId,cmd_cb, user_data)

#define IWMS_DbgSetMsgRefNum(p, clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->DbgSetMsgRefNum(p, clientId,cmd_cb, user_data)

#define IWMS_DbgClearSMMAFlag(p, clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->DbgClearSMMAFlag(p, clientId,cmd_cb, user_data)

#define IWMS_DbgGetRetryInterval(p, clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->DbgGetRetryInterval(p, clientId,cmd_cb, user_data)

#define IWMS_DbgSetRetryInterval(p, clientId, cmd_cb, user_data, retry_interval) \
           AEEGETPVTBL(p,IWMS)->DbgGetRetryInterval(p, clientId,cmd_cb, user_data, retry_interval)

/* Transport Service (TS) Group functions 
*/
#define IWMS_TsEncode(p, ClientTsDataPtr, RawTsDataPtr) \
           AEEGETPVTBL(p,IWMS)->TsEncode(p, ClientTsDataPtr, RawTsDataPtr)

#define IWMS_TsDecode(p, RawTsDataPtr, ClientTsDataPtr) \
           AEEGETPVTBL(p,IWMS)->TsDecode(p, RawTsDataPtr, ClientTsDataPtr)

#define IWMS_TsGetHeaderLength(p, udhPtr) \
           AEEGETPVTBL(p,IWMS)->TsGetHeaderLength(p, udhPtr)

#define IWMS_TsBcdToAscii(p, InputPtr, length, OutputPtr) \
           AEEGETPVTBL(p,IWMS)->TsBcdToAscii(p, InputPtr, length, OutputPtr)

#define IWMS_TsAsciiToBcd(p, InputPtr, length, OutputPtr) \
           AEEGETPVTBL(p,IWMS)->TsAsciiToBcd(p, InputPtr, length, OutputPtr)

#define IWMS_TsAsciiToDefault(p, InputPtr, length, OutputPtr) \
           AEEGETPVTBL(p,IWMS)->TsAsciiToDefault(p, InputPtr, length, OutputPtr)

#define IWMS_TsDefaultToAscii(p, InputPtr, length, OutputPtr) \
           AEEGETPVTBL(p,IWMS)->TsDefaultToAscii(p, InputPtr, length, OutputPtr)

#define IWMS_TsDecodeRelativeTime(p, time, TimeStampPtr) \
           AEEGETPVTBL(p,IWMS)->TsDecodeRelativeTime(p, time, TimeStampPtr)

#define IWMS_TsEncodeRelativeTime(p, TimeStampPtr) \
           AEEGETPVTBL(p,IWMS)->TsEncodeRelativeTime(p, TimeStampPtr)

#define IWMS_TsDecodeDCS(p, data, dcs) \
           AEEGETPVTBL(p,IWMS)->TsDecodeDCS(p, data, dcs)

#define IWMS_TsEncodeDCS(p, dcs, data) \
           AEEGETPVTBL(p,IWMS)->TsEncodeDCS(p, dcs, data)

#define IWMS_TsPackAscii(p, AsciiPtr, DataPtr, DataLengthPtr, PaddingBitsPtr) \
           AEEGETPVTBL(p,IWMS)->TsPackAscii(p, AsciiPtr, DataPtr, DataLengthPtr, PaddingBitsPtr)

#define IWMS_TsUnpackAscii(p, UserDataPtr, length, OutputPtr) \
           AEEGETPVTBL(p,IWMS)->TsUnpackAscii(p, UserDataPtr, length, OutputPtr)

#define IWMS_TsDtmfToAscii(p, length, InputPtr, OutputPtr) \
           AEEGETPVTBL(p,IWMS)->TsDtmfToAscii(p, length, InputPtr, OutputPtr)

#define IWMS_TsAsciiToDtmf(p, InputPtr, OutputPtr) \
           AEEGETPVTBL(p,IWMS)->TsAsciiToDtmf(p, InputPtr, OutputPtr)

#define IWMS_TsConvertCdmaGwMessageToAuto(p, InputPtr, length, OutputPtr) \
           AEEGETPVTBL(p,IWMS)->TsConvertCdmaGwMessageToAuto(p, InputPtr, length, OutputPtr)

#define IWMS_TsConvertAutoToCdmaGwMessage(p, clientId, MsgMode, InputPtr, OutputPtr) \
           AEEGETPVTBL(p,IWMS)->TsConvertAutoToCdmaGwMessage(p, clientId, MsgMode, InputPtr, OutputPtr)

/* Broadcast group
*/
#define IWMS_BcGetConfig(p,clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->BcGetConfig(p, clientId,cmd_cb, user_data)

#define IWMS_BcGetPref(p, clientId,cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->BcGetPref(p,clientId, cmd_cb, user_data)

#define IWMS_BcSetPref(p,clientId, cmd_cb, user_data, pref) \
           AEEGETPVTBL(p,IWMS)->BcSetPref(p,clientId, cmd_cb, user_data, \
                                          pref)

#define IWMS_BcGetTable(p, clientId,cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->BcGetTable(p,clientId, cmd_cb, user_data)

#define IWMS_BcSelectService(p, clientId,cmd_cb, user_data, srv_id, selected, priority) \
           AEEGETPVTBL(p,IWMS)->BcSelectService(p, clientId,cmd_cb, user_data, \
                                          srv_id, selected, priority)

#define IWMS_BcGetAllServiceIds(p,clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->BcGetAllServiceIds(p, clientId,cmd_cb, user_data)

#define IWMS_BcGetServiceInfo(p, clientId,cmd_cb, user_data, srv_id) \
           AEEGETPVTBL(p,IWMS)->BcGetServiceInfo(p, clientId,cmd_cb, user_data, \
                                          srv_id)

#define IWMS_BcAddService(p, clientId,cmd_cb, user_data, srv_info) \
           AEEGETPVTBL(p,IWMS)->BcAddService(p,clientId, cmd_cb, user_data, \
                                          srv_info)

#define IWMS_BcDeleteService(p,clientId, cmd_cb, user_data, srv_id) \
           AEEGETPVTBL(p,IWMS)->BcDeleteService(p, clientId,cmd_cb, user_data, \
                                          srv_id)

#define IWMS_BcChangeLabel(p, clientId,cmd_cb, user_data, srv_id, label_ptr) \
           AEEGETPVTBL(p,IWMS)->BcChangeLabel(p, clientId,cmd_cb, user_data, \
                                          srv_id, label_ptr)

#define IWMS_BcDeleteAllServices(p,clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->BcDeleteAllServices(p, clientId,cmd_cb, user_data)

#define IWMS_BcSetPriorityForAll(p,clientId, cmd_cb, user_data, priority) \
           AEEGETPVTBL(p,IWMS)->BcSetPriorityForAll(p, clientId,cmd_cb, user_data, priority)

/* Multimode Broadcast group
*/
#define IWMS_BcMmGetConfig(p,clientId, cmd_cb, user_data, mode) \
           AEEGETPVTBL(p,IWMS)->BcMmGetConfig(p, clientId,cmd_cb, user_data, mode)

#define IWMS_BcMmGetPref(p, clientId,cmd_cb, user_data, mode) \
           AEEGETPVTBL(p,IWMS)->BcMmGetPref(p,clientId, cmd_cb, user_data, mode)

#define IWMS_BcMmSetPref(p,clientId, cmd_cb, user_data, mode, pref) \
           AEEGETPVTBL(p,IWMS)->BcMmSetPref(p,clientId, cmd_cb, user_data, \
                                          mode, pref)

#define IWMS_BcMmGetTable(p, clientId,cmd_cb, user_data, mode) \
           AEEGETPVTBL(p,IWMS)->BcMmGetTable(p,clientId, cmd_cb, user_data, mode)

#define IWMS_BcMmSelectService(p, clientId,cmd_cb, user_data, mode, srv_id, selected) \
           AEEGETPVTBL(p,IWMS)->BcMmSelectService(p, clientId,cmd_cb, user_data, \
                                          mode, srv_id, selected)

#define IWMS_BcMmGetAllServiceIds(p,clientId, cmd_cb, user_data, mode) \
           AEEGETPVTBL(p,IWMS)->BcMmGetAllServiceIds(p, clientId,cmd_cb, user_data, mode)

#define IWMS_BcMmGetServiceInfo(p, clientId,cmd_cb, user_data, mode, srv_id) \
           AEEGETPVTBL(p,IWMS)->BcGetServiceInfo(p, clientId, cmd_cb, user_data, \
                                          mode, srv_id)

#define IWMS_BcMmAddServices(p, clientId,cmd_cb, user_data, mode, num, srv_info) \
           AEEGETPVTBL(p,IWMS)->BcMmAddServices(p,clientId, cmd_cb, user_data, \
                                          mode, num, srv_info)

#define IWMS_BcMmDeleteServices(p,clientId, cmd_cb, user_data, mode, num, srv_id) \
           AEEGETPVTBL(p,IWMS)->BcMmDeleteServices(p, clientId,cmd_cb, user_data, \
                                          mode, num, srv_id)

#define IWMS_BcMmChangeServiceInfo(p,clientId, cmd_cb, user_data, mode, srv_info) \
           AEEGETPVTBL(p,IWMS)->BcMmChangeServiceInfo(p, clientId,cmd_cb, user_data, \
                                          mode, srv_info)

#define IWMS_BcMmDeleteAllServices(p,clientId, cmd_cb, user_data, mode) \
           AEEGETPVTBL(p,IWMS)->BcMmDeleteAllServices(p, clientId,cmd_cb, user_data, \
                                          mode)

#define IWMS_BcMmSelectAllServices(p,clientId, cmd_cb, user_data, mode, selected) \
           AEEGETPVTBL(p,IWMS)->BcMmSelectAllServices(p, clientId,cmd_cb, user_data, \
                                          mode, selected)

#define IWMS_BcMmSetPriorityAllServices(p,clientId, cmd_cb, user_data, mode, priority) \
           AEEGETPVTBL(p,IWMS)->BcMmSetPriorityAllServices(p, clientId,cmd_cb, user_data, \
                                          mode, priority)

#define IWMS_BcMmMsgDeleteIndication(p, clientId, cmd_cb, user_data, cbPageHeader) \
           AEEGETPVTBL(p,IWMS)->BcMmMsgDeleteIndication(p, clientId, cmd_cb, user_data, \
                                          cbPageHeader)

#define IWMS_BcMmMsgDeleteAllIndication(p, clientId, cmd_cb, user_data) \
           AEEGETPVTBL(p,IWMS)->BcMmMsgDeleteAllIndication(p, clientId, cmd_cb, user_data)

#define IWMS_BcMmGetReadingPref(p,clientId, cmd_cb, user_data, mode) \
           AEEGETPVTBL(p,IWMS)->BcMmGetReadingPref(p, clientId,cmd_cb, user_data, mode)

#define IWMS_BcMmSetReadingPref(p,clientId, cmd_cb, user_data, mode, reading_advised, reading_optional) \
           AEEGETPVTBL(p,IWMS)->BcMmSetReadingPref(p, clientId,cmd_cb, user_data, \
                                          mode, reading_advised, reading_optional)

/*=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IWMS

Description:
  IWMS is a comprehensive interface to support SME and EMS in CDMA1x, GSM/GPRS
  and UMTS multimode environment.

  It provides the following services:

  ...


  The IWMS interface is obtained via the ISHELL_CreateInstance mechanism.


  The following defines are used to communicate the WMS status from
  IWMS to the Applications:

  ...

=======================================================================*/

#endif    /* AEEWMS_H */

#ifndef CMWMSG_H
#define CMWMSG_H

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002 - 2006 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/cmwmsg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/08   jd      Removed duplicate entry of uint8 ie_ss_location_notification_res_s_type;
06/17/08   ak      Compilation error discovered on target rectified.
09/14/07   dm      Added support for FEATURE_MM_SUPERSET
06/27/07   cl      Add new CM API to expose cmwcall_get_user_data() to RPC
05/02/07   rp      Adding FEATURE_CM_LCS
04/15/07   rp      Removing FEATURE_MMGPS
07/04/06   ka      Changes to support call control on PS calls.
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/19/04   ka      Adding support for user to user signaling
08/26/04   sbs     Added externalAddress and nameString IEs to MOLR_Invoke /
                   LocationNotification SS messages
08/17/04   ic      Undoing P4 change list #121638
02/06/04   ws      Initial jaguar/mainline merge.
07/08/03   sbs/lt  Added support for Location Services related SS messages
04/29/03   sun     Added new typedef for cc_reject
01/21/03   ws      Updated copyright information for 2003
09/30/02   RI      Added ie_ss_notification_s_type declaration.
08/29/02   PRK     Added "dummy" typedefs for non UMTS targets.
08/15/02   RI      Modified the max. forwarded to number size to be
                   same as that in mn_cm_exp.h
08/12/02   RI      Modified forwarding feature list definition.
                   Added definitions for ECT.
07/29/02   RI      Added definitions for supporting cause of no CLI.
05/07/02   PRK     Renamed to cmwmsg.h.
02/04/02   RI      Initial release.
                   This is a temporary file used by UI, DS and CM only.
                   - translates the data structures defined by the lower
                   layers to the standard API formats at the CM interface.
===========================================================================*/
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_MM_SUPERSET))
#include "mn_cm_exp.h"
#include "sm_common.h"

typedef cm_user_data_T                   cm_user_data_s_type;
typedef cm_repeat_indicator_T            cm_repeat_indicator_s_type;
typedef cm_bearer_capability_T           cm_bearer_capability_s_type;
typedef cm_low_layer_compatibility_T     cm_low_layer_compatibility_s_type;
typedef cm_high_layer_compatibility_T    cm_high_layer_compatibility_s_type;
typedef cm_called_party_bcd_no_T         cm_called_party_bcd_no_s_type;
typedef cm_called_party_subaddress_T     cm_called_party_subaddress_s_type;
typedef cm_calling_party_bcd_no_T        cm_calling_party_bcd_no_s_type;
typedef cm_calling_party_subaddress_T    cm_calling_party_subaddress_s_type;
typedef cm_forward_cug_info_T            cm_forward_cug_info_s_type;
typedef ie_cm_cc_cause_T                 ie_cm_cc_cause_s_type;
typedef ie_cm_mo_call_details_T          ie_cm_mo_call_details_s_type;
typedef ie_cm_ss_error_T                 ie_cm_ss_error_s_type;
typedef active_calls_list_T              active_calls_list_s_type;
typedef cm_connected_no_T                cm_connected_no_s_type;
typedef cm_connected_subaddress_T        cm_connected_subaddress_s_type;
typedef ie_forwarding_info_T             ie_forwarding_info_s_type;
typedef ie_call_barring_info_T           ie_call_barring_info_s_type;
typedef ie_cug_info_T                    ie_cug_info_s_type;
typedef ie_ss_status_T                   ie_ss_status_s_type;
typedef basic_service_group_list_T       basic_service_group_list_s_type;
typedef ie_forwarding_feature_list_T     ie_forwarding_feature_list_s_type;
typedef cli_restriction_info_T           cli_restriction_info_s_type;
typedef ie_ss_password_T                 ie_ss_password_s_type;
typedef uss_data_T                       uss_data_s_type;
typedef ie_cm_ss_data_T                  ie_cm_ss_data_s_type;
typedef basic_service_T                  basic_service_s_type;
typedef ie_no_reply_condition_timer_T    ie_no_reply_condition_timer_s_type;
typedef ie_ss_code_T                     ie_ss_code_s_type;
typedef ss_User_Data_T                   ss_User_Data_s_type;
typedef uss_res_data_T                   uss_res_data_s_type;
typedef cm_cause_of_no_CLI_T             cm_cause_of_no_cli_s_type;
typedef routing_number_option_T          routing_number_option_s_type;
typedef ect_call_state_T                 ect_call_state_s_type;
typedef ie_ss_notification_T             ie_ss_notification_s_type;
typedef ie_cm_cc_reject_T                ie_cm_cc_reject_s_type;

#if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS) || defined(FEATURE_MM_SUPERSET))
typedef location_method_T                location_method_s_type;
typedef lcs_qos_T                        lcs_qos_s_type;
typedef mlc_number_T                     mlc_number_s_type;
typedef lcs_client_external_id_T         lcs_client_external_id_s_type;
typedef gps_assistance_data_T            gps_assistance_data_s_type;
typedef supported_gad_shapes_T           supported_gad_shapes_s_type;
typedef location_notification_T          location_notification_s_type;
typedef ie_ss_location_notification_res_T ie_ss_location_notification_res_s_type;
typedef lcs_molr_res_T                   lcs_molr_res_s_type;
typedef molr_type_e_type                 cm_molr_type_e_type;

#else

typedef uint8                            ie_ss_location_notification_res_s_type;
typedef uint8                            cm_molr_type_e_type;
typedef uint8                            location_method_s_type;
typedef uint8                            lcs_qos_s_type;
typedef uint8                            mlc_number_s_type;
typedef uint8                            lcs_client_external_id_s_type;
typedef uint8                            gps_assistance_data_s_type;
typedef uint8                            supported_gad_shapes_s_type;
typedef uint8                            location_notification_s_type;
typedef uint8                            lcs_molr_res_s_type;

#endif

#if defined(FEATURE_CCBS) || defined(FEATURE_MM_SUPERSET)
typedef ccbs_info_T                      ccbs_info_s_type;
#endif

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_MM_SUPERSET)
typedef cm_supported_codecs_list_T       cm_supported_codecs_list_s_type;
#endif
  

typedef  nsapi_T                         nsapi_s_type;  
typedef  llc_sapi_T                      llc_sapi_s_type;  

#else /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

typedef uint8                            ie_ss_location_notification_res_s_type;
typedef uint8                            cm_molr_type_e_type;
typedef uint8                            location_notification_s_type;
typedef uint8                            lcs_molr_res_s_type;
typedef uint8                            location_method_s_type;
typedef uint8                            lcs_qos_s_type;
typedef uint8                            mlc_number_s_type;
typedef uint8                            lcs_client_external_id_s_type;
typedef uint8                            gps_assistance_data_s_type;
typedef uint8                            supported_gad_shapes_s_type;

typedef uint8                            cm_user_data_s_type;
typedef uint8                            cm_repeat_indicator_s_type;
typedef uint8                            cm_bearer_capability_s_type;
typedef uint8                            cm_low_layer_compatibility_s_type;
typedef uint8                            cm_high_layer_compatibility_s_type;
typedef uint8                            cm_called_party_bcd_no_s_type;
typedef uint8                            cm_called_party_subaddress_s_type;
typedef uint8                            cm_calling_party_bcd_no_s_type;
typedef uint8                            cm_calling_party_subaddress_s_type;
typedef uint8                            cm_forward_cug_info_s_type;
typedef uint8                            ie_cm_cc_cause_s_type;
typedef uint8                            ie_cm_mo_call_details_s_type;
typedef uint8                            ie_cm_ss_error_s_type;
typedef uint8                            active_calls_list_s_type;
typedef uint8                            cm_connected_no_s_type;
typedef uint8                            cm_connected_subaddress_s_type;
typedef uint8                            ie_forwarding_info_s_type;
typedef uint8                            ie_call_barring_info_s_type;
typedef uint8                            ie_cug_info_s_type;
typedef uint8                            ie_ss_status_s_type;
typedef uint8                            basic_service_group_list_s_type;
typedef uint8                            ie_forwarding_feature_list_s_type;
typedef uint8                            cli_restriction_info_s_type;
typedef uint8                            ie_ss_password_s_type;
typedef uint8                            uss_data_s_type;
typedef uint8                            ie_cm_ss_data_s_type;
typedef uint8                            basic_service_s_type;
typedef uint8                            ie_no_reply_condition_timer_s_type;
typedef uint8                            ie_ss_code_s_type;
typedef uint8                            ss_User_Data_s_type;
typedef uint8                            uss_res_data_s_type;
typedef uint8                            cm_cause_of_no_cli_s_type;
typedef uint8                            routing_number_option_s_type;
typedef uint8                            ect_call_state_s_type;
typedef uint8                            ie_ss_notification_s_type;
typedef uint8                            ie_cm_cc_reject_s_type;

typedef uint8                            nsapi_T;
typedef uint8                            qos_T;
typedef uint8                            pdp_address_T;
typedef uint8                            protocol_cfg_options_T;
typedef uint8                            apn_T;
typedef uint8                            ti_T;
typedef uint8                            cause_enum_type_T;
typedef uint8                            pdp_cause_T;
typedef uint8                            rabm_rabid_T;
typedef uint8                            rabmcm_cause_T;
typedef uint8                            tft_type_T;
typedef uint8                            sm_status_T;
typedef uint8                            ss_password_value;
typedef uint8                            cm_ss_conf_e_type;
typedef uint8                            ccbs_info_s_type;
typedef uint8                            cm_supported_codecs_list_s_type;

/* Defined in mn_cm_exp.h */

#define MAXNO_FORWARD_TO_NUMBER          (2)

typedef  uint8                           nsapi_s_type;  
typedef  uint8                           llc_sapi_s_type;  

#endif /* #else (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

typedef enum no_cli_cause_e
{

  CLI_UNAVAILABLE,
  CLI_REJECT_BY_USER,
  CLI_INTERACTION_WITH_OTHER_SRVC,
  CLI_COIN_PAY_PHONE

} no_cli_cause_e_type;

#endif /* CMWMSG_H */

/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef __PTP_HOOK_H__
#define __PTP_HOOK_H__

#include "ptp_responder_api.h"

/* ====== PTP completion callbacks ====== */

typedef jresult_t (*ptp_operation_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response);

typedef jresult_t (*ptp_get_dev_info_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_dev_info_t *devinfo);

typedef jresult_t (*ptp_get_storage_ids_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t *stores,
    juint32_t count);

typedef jresult_t (*ptp_get_storage_info_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_info_t *store_info);

typedef jresult_t (*ptp_get_num_objects_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t num_objects);

typedef jresult_t (*ptp_get_obj_handles_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t *handles,
    juint32_t count);

typedef jresult_t (*ptp_get_obj_info_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_info_t *objinfo);

typedef jresult_t (*ptp_get_obj_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t actual_size, juint32_t obj_size);

typedef jresult_t (*ptp_send_obj_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response);

typedef jresult_t (*ptp_get_dev_prop_desc_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_dev_prop_desc_t *dev_prop_desc);

typedef jresult_t (*ptp_get_dev_prop_val_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_prop_value_t *dev_prop_val);

typedef jresult_t (*ptp_set_dev_prop_val_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response);

typedef jresult_t (*ptp_copy_obj_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t new_handle);

typedef jresult_t (*ptp_send_obj_info_complete_cb)(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t actual_store,
    ptp_obj_handle_t actual_folder, ptp_obj_handle_t new_handle);

typedef struct
{
    ptp_operation_complete_cb           operation_comp;
    ptp_get_dev_info_complete_cb        get_dev_info_comp;
    ptp_get_storage_ids_complete_cb     get_storage_ids_comp;
    ptp_get_storage_info_complete_cb    get_storage_info_comp;
    ptp_get_num_objects_complete_cb     get_num_obj_comp;
    ptp_get_obj_handles_complete_cb     get_obj_handles_comp;
    ptp_get_obj_info_complete_cb        get_obj_info_comp;
    ptp_get_obj_complete_cb             get_obj_comp;
    ptp_send_obj_info_complete_cb       send_obj_info_comp;
    ptp_send_obj_complete_cb            send_obj_comp;
    ptp_get_dev_prop_desc_complete_cb   get_dev_prop_desc_comp;
    ptp_get_dev_prop_val_complete_cb    get_dev_prop_val_comp;
    ptp_set_dev_prop_val_complete_cb    set_dev_prop_val_comp;
    ptp_copy_obj_complete_cb            copy_obj_comp;
} ptp_responder_comp_cbs;

jresult_t ptp_hook_responder(ptp_instanceh_t insth,
    ptp_responder_cbs *prev_cbs, ptp_responder_comp_cbs *prev_comp_cbs,
    ptp_responder_cbs *hook_cbs, ptp_responder_comp_cbs *hook_comp_cbs,
    ptp_appctx_t global_hookctx);

#define PTP_GET_HOOKCTX(sessionh)       (*(((ptp_appctx_t*)sessionh)+1))
#define PTP_SET_HOOKCTX(sessionh,ctx)   \
    *(((ptp_appctx_t*)sessionh)+1) = (ptp_appctx_t)ctx

ptp_appctx_t ptp_get_global_hookctx(ptp_sessionh_t sessionh);
ptp_appctx_t ptp_inst_get_global_hookctx(ptp_instanceh_t insth);

#endif/*__PTP_HOOK_H__*/


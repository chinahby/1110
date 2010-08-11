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
#ifndef __PTP_RESPONDER_API_H__
#define __PTP_RESPONDER_API_H__

#include "ptp_types.h"
#include "ptp_utils.h"

struct ptp_instance;
struct ptp_session;

typedef struct ptp_instance * ptp_instanceh_t;
typedef struct ptp_session * ptp_sessionh_t;
typedef void * ptp_appctx_t;    /* application context */

/* General PTP transaction completion callback.
 * It is called when a PTP transaction is completed, or when an error occurs.
 * NOTE: different parameters are relevant for different transactions
 * NOTE: a session handle and/or an application context may or may not exist
 * at this point since some operations may be performed outside of PTP sessions
 */
typedef void (*ptp_transaction_complete_cb)(ptp_sessionh_t sessionh,
    ptp_io_status_t result, ptp_operation_t operation,
    ptp_response_t response, juint32_t obj_size, juint32_t bytes_transferred);

/* General PTP event completion callback */
typedef void (*ptp_event_complete_cb)(ptp_sessionh_t sessionh,
    ptp_io_status_t result, ptp_event_t event);

/* PTP instance unregister handler */
typedef void (*ptp_unregister_cb)(ptp_instanceh_t insth,
    ptp_appctx_t global_ctx);
    
/* Generic PTP operation completion function.
 * This function should be called by an application after it has finished
 * processing an operation that does not have a specific completion function.
 *   NOTE: This does NOT apply to OpenSession and CloseSession.
 */
jresult_t ptp_operation_complete(ptp_sessionh_t sessionh,
    ptp_response_t response);

/* ====== PTP Operations (No Data Phase) ====== */

typedef ptp_response_t (*ptp_open_session_cb)(ptp_sessionh_t sessionh,
    juint32_t session_id, ptp_appctx_t global_ctx, ptp_appctx_t *sess_ctx);

typedef ptp_response_t (*ptp_close_session_cb)(ptp_sessionh_t sessionh,
    juint32_t session_id);

typedef ptp_response_t (*ptp_get_num_objs_cb)(ptp_sessionh_t sessionh,
    ptp_storage_id_t store, ptp_obj_format_t format,
    ptp_obj_handle_t parent_folder);

jresult_t ptp_get_num_objects_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t num_objects);

typedef ptp_response_t (*ptp_delete_obj_cb)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t handle, ptp_obj_format_t format);

typedef ptp_response_t (*ptp_move_obj_cb)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t handle, ptp_storage_id_t target_store,
    ptp_obj_handle_t target_folder);

typedef ptp_response_t (*ptp_copy_obj_cb)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t handle, ptp_storage_id_t target_store,
    ptp_obj_handle_t target_folder);

jresult_t ptp_copy_obj_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t new_handle);

typedef ptp_response_t (*ptp_set_obj_protection_cb)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t handle, ptp_protection_status_t status);

/*
 * for each image captured, ptp_object_added() should be called.
 * when capture is done, ptp_capture_complete() should be called.
 * if store gets full, ptp_store_full() should be called.
 *
 * NOTE: (internal) all related events should contain the transaction id
 * of the capture operation
 */
typedef ptp_response_t (*ptp_capture_cb)(ptp_sessionh_t sessionh,
    ptp_storage_id_t store, ptp_obj_format_t format);

/*
 * for each image captured, ptp_object_added() should be called.
 * when capture is done, ptp_capture_complete() should be called.
 * if store gets full, ptp_store_full() should be called.
 *
 * NOTE: (internal) all related events should contain the transaction id
 * of the capture operation
 */
typedef ptp_response_t (*ptp_start_capture_cb)(ptp_sessionh_t sessionh,
    ptp_storage_id_t store, ptp_obj_format_t format);

/* NOTE: (internal) the transaction id of the currently executing capture
 * is provided */
typedef ptp_response_t (*ptp_stop_capture_cb)(ptp_sessionh_t sessionh);

typedef ptp_response_t (*ptp_format_store_cb)(ptp_sessionh_t sessionh,
    ptp_storage_id_t store, ptp_filesystem_type_t fs_format);

typedef ptp_response_t (*ptp_reset_dev_cb)(ptp_sessionh_t sessionh);

typedef ptp_response_t (*ptp_self_test_cb)(ptp_sessionh_t sessionh);

typedef ptp_response_t (*ptp_power_down_cb)(ptp_sessionh_t sessionh);

typedef ptp_response_t (*ptp_reset_dev_prop_val_cb)(ptp_sessionh_t sessionh,
    ptp_dev_prop_t dev_prop_code);

/* ====== PTP Events (No Data Phase) ====== */
typedef ptp_response_t (*ptp_transact_cancelled_cb)(ptp_sessionh_t sessionh);
    
    
/* ====== PTP Operations (Responder-to-Initiator Data Phase) ====== */

/*
 * The device info provided should depend on the device's state.
 * NOTE: a session handle and/or an application context may or may not exist
 * at this point since this operation may be performed outside a PTP session.
 */
typedef ptp_response_t (*ptp_get_dev_info_cb)(ptp_sessionh_t sessionh);

jresult_t ptp_get_dev_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_dev_info_t *devinfo);
    
typedef ptp_response_t (*ptp_get_storage_ids_cb)(ptp_sessionh_t sessionh);

jresult_t ptp_get_storage_ids_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t *stores,
    juint32_t count);
    
typedef ptp_response_t (*ptp_get_storage_info_cb)(ptp_sessionh_t sessionh,
    ptp_storage_id_t store);

jresult_t ptp_get_storage_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_info_t *store_info);

typedef ptp_response_t (*ptp_get_obj_handles_cb)(ptp_sessionh_t sessionh,
    ptp_storage_id_t store, ptp_obj_format_t format,
    ptp_obj_handle_t folder);

jresult_t ptp_get_obj_handles_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t *handles,
    juint32_t count);

typedef ptp_response_t (*ptp_get_obj_info_cb)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t handle);

jresult_t ptp_get_obj_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_info_t *objinfo);

typedef ptp_response_t (*ptp_get_obj_cb)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t handle, void *data, juint32_t max_size, juint32_t offset);

jresult_t ptp_get_obj_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t actual_size, juint32_t obj_size);

typedef ptp_response_t (*ptp_get_dev_prop_desc_cb)(ptp_sessionh_t sessionh,
    ptp_dev_prop_t dev_prop_code);

jresult_t ptp_get_dev_prop_desc_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_dev_prop_desc_t *dev_prop_desc);

typedef ptp_response_t (*ptp_get_dev_prop_val_cb)(ptp_sessionh_t sessionh,
    ptp_dev_prop_t dev_prop_code);

jresult_t ptp_get_dev_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_prop_value_t *dev_prop_val);

/* ====== PTP Operations (Initiator-to-Responder Data Phase) ====== */

typedef ptp_response_t (*ptp_send_obj_info_cb)(ptp_sessionh_t sessionh,
    ptp_obj_info_t *objinfo, ptp_storage_id_t target_store,
    ptp_obj_handle_t target_folder);

jresult_t ptp_send_obj_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t actual_store,
    ptp_obj_handle_t actual_folder, ptp_obj_handle_t new_handle);

typedef ptp_response_t (*ptp_send_obj_cb)(ptp_sessionh_t sessionh,
    void *data, juint32_t size, juint32_t offset);

jresult_t ptp_send_obj_complete(ptp_sessionh_t sessionh,
    ptp_response_t response);

typedef ptp_response_t (*ptp_set_dev_prop_val_cb)(ptp_sessionh_t sessionh,
    ptp_dev_prop_t dev_prop_code, ptp_prop_value_t *dev_prop_val);

jresult_t ptp_set_dev_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response);


/* responder API */
typedef struct
{
    /* Operations: No Data Phase */
    ptp_open_session_cb         open_session_h;
    ptp_close_session_cb        close_session_h;
    ptp_get_num_objs_cb         get_num_objs_h;
    ptp_delete_obj_cb           delete_obj_h;
    ptp_move_obj_cb             move_obj_h;
    ptp_copy_obj_cb             copy_obj_h;
    ptp_set_obj_protection_cb   set_obj_protection_h;
    ptp_capture_cb              capture_h;
    ptp_start_capture_cb        start_capture_h;
    ptp_stop_capture_cb         stop_capture_h;
    ptp_format_store_cb         format_store_h;
    ptp_reset_dev_cb            reset_dev_h;
    ptp_self_test_cb            self_test_h;
    ptp_power_down_cb           power_down_h;
    ptp_reset_dev_prop_val_cb   reset_dev_prop_val_h;

    /* Events: No Data Phase */
    ptp_transact_cancelled_cb   transaction_cancelled_h;

    /* Operations: Responder-to-Initiator Data Phase */
    ptp_get_dev_info_cb         get_dev_info_h;
    ptp_get_storage_ids_cb      get_storage_ids_h;
    ptp_get_storage_info_cb     get_storage_info_h;
    ptp_get_obj_handles_cb      get_obj_handles_h;
    ptp_get_obj_info_cb         get_obj_info_h;
    ptp_get_obj_cb              get_obj_h;
    ptp_get_obj_cb              get_thumb_h;
    ptp_get_dev_prop_desc_cb    get_dev_prop_desc_h;
    ptp_get_dev_prop_val_cb     get_dev_prop_val_h;

    /* Operations: Initiator-to-Responder Data Phase */
    ptp_send_obj_info_cb        send_obj_info_h;
    ptp_send_obj_cb             send_obj_h;
    ptp_set_dev_prop_val_cb     set_dev_prop_val_h;

    /* General completion callbacks */
    ptp_transaction_complete_cb transaction_complete_h;
    ptp_event_complete_cb       event_complete_h;

    /* PTP instance unregister callback */
    ptp_unregister_cb           unregister_h;
} ptp_responder_cbs;

/* ====== PTP Events ====== */

jresult_t ptp_object_added(ptp_sessionh_t sessionh, ptp_obj_handle_t obj);
jresult_t ptp_object_removed(ptp_sessionh_t sessionh, ptp_obj_handle_t obj);
jresult_t ptp_store_added(ptp_sessionh_t sessionh, ptp_storage_id_t store);
jresult_t ptp_store_removed(ptp_sessionh_t sessionh, ptp_storage_id_t store);
jresult_t ptp_device_prop_changed(ptp_sessionh_t sessionh, ptp_dev_prop_t prop);
jresult_t ptp_object_info_changed(ptp_sessionh_t sessionh, ptp_obj_handle_t obj);
jresult_t ptp_device_info_changed(ptp_sessionh_t sessionh);
jresult_t ptp_storage_info_changed(ptp_sessionh_t sessionh,
    ptp_storage_id_t store);
jresult_t ptp_request_object_transfer(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj);
jresult_t ptp_store_full(ptp_sessionh_t sessionh, ptp_storage_id_t store);
jresult_t ptp_capture_complete(ptp_sessionh_t sessionh);
jresult_t ptp_unreported_status(ptp_sessionh_t sessionh);

/* ====== PTP initialization ====== */

jresult_t ptp_register_responder(ptp_instanceh_t insth,
    ptp_responder_cbs *cbs, ptp_appctx_t global_ctx);

#define PTP_GET_APPCTX(sessionh) (*((ptp_appctx_t*)sessionh))

#endif /*__PTP_RESPONDER_API_H__*/

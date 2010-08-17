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
#ifndef _MTP_API_H_
#define _MTP_API_H_

#include "mtp_types.h"


/*******************************
 *       MTP Operations
 *******************************/

/*///////////////////////////////
         Basic Operation
 /////////////////////////////// */

/**
 * Function type:  mtp_get_obj_props_supp
 * Description:    Handles a GetObjectPropsSupported operation.
 *                 Upon completion of the operation's data processing phase,
 *                 the application must call mtp_get_obj_props_supp_complete().
 * Requirement:    Mandatory
 * Parameters: 
 *     @sessionh:        (IN) PTP session handle
 *     @obj_format_code: (IN) PTP object format code
 *
 * Return value:   PTP/MTP response code
 * Scope:          Static
 **/
typedef ptp_response_t (*mtp_get_obj_props_supp)(ptp_sessionh_t sessionh,
    ptp_obj_format_t obj_format_code);

/**
 * Function name:  mtp_get_obj_props_supp_complete
 * Description:    GetObjectPropsSupported operation completion callback.
 *                 The application's GetObjectPropsSupported handler -
 *                 mtp_get_obj_props_supp - must call this function upon
 *                 completion of the operation's data processing phase.
 * Parameters: 
 *     @sessionh:      (IN) PTP session handle
 *     @response:      (IN) PTP/MTP response code
 *     @array_size:    (IN) The number of elements in the obj_prop_code array
 *     @obj_prop_code: (IN) An array of MTP object property codes identifying
 *                          the supported object properties
 *
 * Return value:        0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t mtp_get_obj_props_supp_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint16_t array_size,
    mtp_obj_prop_t *obj_prop_code);

/**
 * Function type:  mtp_get_obj_prop_desc
 * Description:    Handles a GetObjectPropDesc operation.
 *                 Upon completion of the operation's data processing phase,
 *                 the application must call mtp_get_obj_prop_desc_complete().
 * Requirement:    Mandatory
 * Parameters: 
 *     @sessionh:        (IN) PTP session handle
 *     @obj_prop_code:   (IN) Object property code identifying the property
 *                            queried for its descriptor dataset
 *     @obj_format_code: (IN) PTP object format code
 *
 * Return value:   PTP/MTP response code
 * Scope:          Static
 **/
typedef ptp_response_t (*mtp_get_obj_prop_desc)(ptp_sessionh_t sessionh,
    mtp_obj_prop_t obj_prop_code, ptp_obj_format_t obj_format_code);

/**
 * Function name:  mtp_get_obj_prop_desc_complete
 * Description:    GetObjectPropDesc operation completion callback.
 *                 The application's GetObjectPropDesc handler -
 *                 mtp_get_obj_prop_desc - must call this function upon
 *                 completion of the operation's data processing phase.
 * Parameters: 
 *     @sessionh:         (IN) PTP session handle
 *     @response:         (IN) PTP/MTP response code
 *     @obj_prop_desc_t:  (IN) An array of object property descriptor datasets
 *
 * Return value:        0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t mtp_get_obj_prop_desc_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, mtp_obj_prop_desc_t *obj_prop_desc);

/**
 * Function type:  mtp_get_obj_prop_val
 * Description:    Handles a GetObjectPropValue operation.
 *                 Upon completion of the operation's data processing phase,
 *                 the application must call mtp_get_obj_prop_val_complete().
 *                 processing phase of this operation.
 *
 *                 Multi-phase GetObjectPropValue handling: If the total size of
 *                 the requested property value  exceeds the size of the FD's
 *                 data transfer buffer (PTP_MAX_BLOCK_SIZE), then the FD will
 *                 divide the handling of the operation into multiple calls to
 *                 to the mtp_get_obj_prop_val callback, each time requesting a
 *                 different portion of the value data.
 *                 A sample calling sequence example is provided below, after
 *                 the description of mtp_get_obj_prop_val_complete().
 * Requirement:    Mandatory
 * Parameters: 
 *     @sessionh:      (IN) PTP session handle
 *     @obj_handle:    (IN) PTP object handle
 *     @obj_prop_code: (IN) MTP object property code
 *     @elem_offset:   (IN) If multi-phase handling of the GetObjectPropValue
 *                          is required (see explanation above), this parameter
 *                          indicates the zero-based offset from the beginning
 *                          of the property value data array from which to start
 *                          the current data transfer.
 *                          Otherwise, this parameter is set to zero.
 *     @max_elems:     (IN) If multi-phase handling of the GetObjectPropValue
 *                          is required (see explanation above), this parameter
 *                          indicates the maximum number of elements from the
 *                          property value array, beginning at offset
 *                          elem_offset, which can be returned by the
 *                          Otherwise, this parameter is set to zero.
 * Return value:   PTP/MTP response code
 * Scope:          Static
 **/
typedef ptp_response_t (*mtp_get_obj_prop_val)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle, mtp_obj_prop_t obj_prop_code,
    juint32_t elem_offset, juint32_t max_elems);

/**
 * Function name:  mtp_get_obj_prop_val_complete
 * Description:    GetObjectPropValue operation completion callback.
 *                 The application's GetObjectPropValue handler -
 *                 mtp_get_obj_prop_val - must call this function upon
 *                 completion of the operation's data processing phase.
 *
 *                 Multi-phase GetObjectPropValue handling: see explanation in
 *                 the description of mtp_get_obj_prop_val.
 * Parameters: 
 *     @sessionh: (IN) PTP session handle
 *     @response: (IN) PTP/MTP response code
 *     @value:    (IN) Pointer to a PTP property value structure, containing
 *                     the value information requested by the FD in the
 *                     previous call to the application's GetObjectPropValue
 *                     handler (see mtp_get_obj_prop_val).
 *
 * Return value:   0 on success, otherwise an error code.
 *                 If the value of the num-elems field of the value parameter
 *                 exceeds the value of the max_elems parameter of the previous
 *                 call to the mtp_get_obj_prop_val callback, the function
 *                 returns JENOMEM.
 * Scope:          Global
 **/
jresult_t mtp_get_obj_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_prop_value_t *value);

/* Multi-phase GetObjectPropValue Calling Sequence Example:
 * Assuming the size of the FD's data transfer buffer (PTP_MAX_BLOCK_SIZE)
 * is 4K, the desired property value data is stored in a juint32_t array, and the
 * total size of the value data is 10,000 bytes - i.e. a total of 2,500
 * juint32_t elements must be received:
 * == START ==
 * mtp_get_obj_prop_val             - (..., 0, 1000);
 * mtp_get_obj_prop_val_complete    - (..., value);
 *                                    value.total_elems = 2500;
 *                                    value.num_elems   = 1000;
 *                                    value.elem_offset = 0
 * mtp_get_obj_prop_val             - (..., 1000, 1000);
 * mtp_get_obj_prop_val_complete    - (..., value);
 *                                    value.total_elems = 2500;
 *                                    value.num_elems   = 1000;
 *                                    value.elem_offset = 1000
 * mtp_get_obj_prop_val             - (..., 2000, 1000)
 * mtp_get_obj_prop_val_complete    - (..., value);
 *                                    value.total_elems = 2500;
 *                                    value.num_elems   =  500;
 *                                    value.elem_offset = 2000
 * == END ==
 * */


/**
 * Function type:  mtp_set_obj_prop_val
 * Description:    Handles a SetObjectPropValue operation.
 *                 Upon completion of the operation's data processing phase,
 *                 the application must call mtp_set_obj_prop_val_complete().
 *                 processing phase of this operation.
 *
 *                 Multi-phase SetObjectPropValue handling: If the total size of
 *                 the specified property value exceeds the size of the FD's
 *                 data transfer buffer (PTP_MAX_BLOCK_SIZE), then the FD will
 *                 handle the SetObjectPropValue operation by issuing several
 *                 calls to the mtp_set_obj_prop_val callback, each time passing
 *                 to the application, via the value parameter, a different
 *                 portion of the property value and the required information
 *                 regarding the partial transfer that is being performed.
 *                 The application should copy the value data received from the
 *                 FD and use it as needed.
 *                 A sample calling sequence example is provided below, after
 *                 the description of mtp_set_obj_prop_val_complete().
 * Requirement:    Mandatory
 * Parameters: 
 *     @sessionh:      (IN) PTP session handle
 *     @obj_handle:    (IN) PTP object handle
 *     @obj_prop_code: (IN) MTP object property code
 *     @value:         (IN) Pointer to a PTP property value structure.
 *                          In case of multi-phase handling of the operation
 *                          (see explanation above), the application should copy
 *                          the received value data and use it as needed.
 *
 * Return value:   PTP/MTP response code
 * Scope:          Static
 **/
typedef ptp_response_t (*mtp_set_obj_prop_val)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle, mtp_obj_prop_t obj_prop_code,
    ptp_prop_value_t *value);

/**
 * Function name:  mtp_set_obj_prop_val_complete
 * Description:    SetObjectPropValue operation completion callback.
 *                 The application's SetObjectPropValue handler -
 *                 mtp_set_obj_prop_val - must call this function upon
 *                 completion of the operation's data processing phase.
 * Parameters: 
 *     @sessionh: (IN) PTP session handle
 *     @response: (IN) PTP/MTP response code
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t mtp_set_obj_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response);

/* Multi-phase SetObjectPropValue Calling Sequence Example:
 * Assuming the size of the FD's data transfer buffer (PTP_MAX_BLOCK_SIZE)
 * is 4K, the size of the property value data to set is 10,000 bytes, and it is
 * stored in a juint32_t array - i.e. the total amount of elements to set is
 * 2,500:
 * == START ==
 * mtp_set_obj_prop_val             - (..., value);
 *                                    value.total_elems = 2500;
 *                                    value.num_elems   = 1000;
 *                                    value.elem_offset = 0
 * mtp_set_obj_prop_val_complete    - (...);
 * mtp_set_obj_prop_val             - (..., value);
 *                                    value.total_elems = 2500;
 *                                    value.num_elems   = 1000;
 *                                    value.elem_offset = 1000
 * mtp_set_obj_prop_val_complete    - (...);
 * mtp_set_obj_prop_val             - (..., value)
 *                                    value.total_elems = 2500;
 *                                    value.num_elems   =  500;
 *                                    value.elem_offset = 2000
 * mtp_set_obj_prop_val_complete    - (...);
 * == END ==
 * */

/**
 * Function type:  mtp_get_obj_refs
 * Description:    Handles a GetObjectRefernces operation.
 *                 Upon completion of the operation's data processing phase,
 *                 the application must call mtp_get_obj_refs_complete().
 * Requirement:    Mandatory
 * Parameters: 
 *     @sessionh:   (IN) PTP session handle
 *     @obj_handle: (IN) PTP object handle
 *
 * Return value:   PTP/MTP response code
 * Scope:          Static
 **/
typedef ptp_response_t (*mtp_get_obj_refs)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle);

/**
 * Function name:  mtp_get_obj_refs_complete
 * Description:    GetObjectRefernces operation completion callback.
 *                 The application's GetObjectRefernces handler -
 *                 mtp_get_obj_refs - must call this function upon
 *                 completion of the operation's data processing phase.
 * Parameters: 
 *     @sessionh:    (IN) PTP session handle
 *     @response:    (IN) PTP/MTP response code
 *     @obj_handles: (IN) An array of PTP object handles
 *     @size:        (IN) The number of elements in the obj_handles array
 *
 * Return value:   0 on success, otherwise an error code.
 *                 If the value of the size parameter exceeds the size of the
 *                 FD's data transfer buffer (PTP_MAX_BLOCK_SIZE) the function
 *                 returns JENOMEM.
 * Scope:          Global
 **/
jresult_t mtp_get_obj_refs_complete( ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t *obj_handles, juint32_t size);

/**
 * Function type:  mtp_set_obj_refs
 * Description:    Handles a SetObjectReferences operation.
 *                 Upon completion of the operation's data processing phase,
 *                 the application must call mtp_set_obj_refs_complete().
 * Requirement:    Mandatory
 * Parameters: 
 *     @sessionh:    (IN) PTP session handle
 *     @obj_handle:  (IN) PTP object handle
 *     @obj_handles: (IN) An array of PTP object handles
 *     @size:        (IN) The number of elements in the obj_handles array
 *
 * Return value:   PTP/MTP response code
 * Scope:          Static
 **/
typedef ptp_response_t (*mtp_set_obj_refs)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle, ptp_obj_handle_t *obj_handles, juint32_t size);

/**
 * Function name:  mtp_set_obj_refs_complete
 * Description:    SetObjectReferences operation completion callback.
 *                 The application's SetObjectReferences handler -
 *                 mtp_set_obj_refs - must call this function upon
 *                 completion of the operation's data processing phase.
 * Parameters: 
 *     @sessionh: (IN) PTP session handle
 *     @response: (IN) PTP/MTP response code
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t mtp_set_obj_refs_complete(ptp_sessionh_t sessionh,
    ptp_response_t response);

/**
 * Function type:  mtp_skip
 * Description:    Handles a Skip operation.
 *                 Upon completion of the operation's data processing phase,
 *                 the application must call mtp_skip_complete().
 * Requirement:    Mandatory
 * Parameters: 
 *     @sessionh:   (IN) PTP session handle
 *     @skip_index: (IN) Skip index -- i.e. the depth and direction into the
 *                       objects playback queue in which to skip.
 *                       A value of 1 indicates that the device should skip
 *                       ahead one media object. A value of -1 indicates that
 *                       the previous object in the device playback queue should
 *                       be loaded as the current playback object.
 * Return value:   PTP/MTP response code
 * Scope:          Static
 **/
typedef ptp_response_t (*mtp_skip)(ptp_sessionh_t sessionh,
    juint32_t skip_index);

/**
 * Function name:  mtp_skip_complete
 * Description:    Skip operation completion callback.
 *                 The application's Skip handler - mtp_skip - must call this
 *                 function upon completion of the skip operation.
 * Parameters: 
 *     @sessionh:  (IN) PTP session handle
 *     @response:  (IN) PTP/MTP response code
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t mtp_skip_complete(ptp_sessionh_t sessionh,
    ptp_response_t response);


/*///////////////////////////////
        Enhanced Operation
 /////////////////////////////// */
#ifdef CONFIG_PROTO_PTP_MTP_ENHANCED

/**
 * Function type:  mtp_get_obj_prop_desc:
 * 2 last parameters:
 *     @elem_idx: (IN) Index in the `element` array (external array)
 *     @data_idx: (IN) Index in the `data` array, when needed (internal array)
 **/
typedef ptp_response_t (*mtp_get_obj_prop_list)(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle, ptp_obj_format_t obj_format_code,
    mtp_obj_prop_t obj_prop_code, mtp_obj_prop_group_t obj_prop_group_code,
    juint32_t depth, juint32_t elem_idx, juint32_t data_idx);

/**
 * Function name:  mtp_get_obj_prop_list_complete:
 * Last parameter:
 *     @num_elems: (IN/OUT)
 *                 IN  - Elements to send
 *                 OUT - Elements sent 
 * */
jresult_t mtp_get_obj_prop_list_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t total_num_elems,
    mtp_obj_prop_elem_t *elem, juint32_t *num_elems);
    
/**
 * Function type:  mtp_set_obj_prop_list
 **/
typedef ptp_response_t (*mtp_set_obj_prop_list)(ptp_sessionh_t sessionh,
    juint32_t total_num_elems, mtp_obj_prop_elem_t *elem, juint32_t elem_count,
    juint32_t elem_idx);

/**
 * Function name:  mtp_set_obj_prop_list_complete
 * Last parameter:
 *     @prop_idx_failed: (IN) Index of the first failed property (1-based)
 **/
jresult_t mtp_set_obj_prop_list_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t prop_idx_failed);

/**
 * Function type:  mtp_get_interdep_prop_desc
 * Last parameter:
 *     @arr_idx: (IN) Array index. Corresponds to the last parameter of
 *                    mtp_get_interdep_prop_desc_complete().
 **/
typedef ptp_response_t (*mtp_get_interdep_prop_desc)(ptp_sessionh_t sessionh,
    ptp_obj_format_t obj_format_code, juint16_t arr_idx);

/**
 * Function name:  mtp_get_interdep_prop_desc_complete
 * Last parameter:
 *     @arr_idx: (IN/OUT)
 *               IN  - Extension index
 *               OUT - High word: Extension index reached
 *                     Low word:  Internal index reached
 **/
jresult_t mtp_get_interdep_prop_desc_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint16_t interdeps_num, 
    interdep_obj_prop_t *interdep_obj_prop_list, juint32_t *arr_idx);

/**
 * Function type:  mtp_send_obj_prop_list
 **/
typedef ptp_response_t (*mtp_send_obj_prop_list)(ptp_sessionh_t sessionh,
    ptp_storage_id_t storage, ptp_obj_handle_t parent_obj_handle,
    ptp_obj_format_t obj_format_code, juint32_t obj_size_msdw,
    juint32_t obj_size_lsdw, mtp_obj_prop_elem_t *elem, juint32_t elem_count,
    juint32_t elem_idx);
    
/**
 * Function name:  mtp_send_obj_prop_list_complete
 **/
jresult_t mtp_send_obj_prop_list_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t storage,
    ptp_obj_handle_t parent_obj_handle, ptp_obj_handle_t reserved_obj_handle,
    juint32_t prop_idx_failed); 
#endif


/*******************************
 * MTP Events
 *******************************/
/**
 * Function name:  mtp_obj_prop_changed
 * Description:    Notifies the host of an ObjectPropChanged event, which occurs
 *                 when an object property value on the responder has changed,
 *                 without that change having been performed by the initiator.
 *                 The function notifies the host of the change.
 * Parameters:
 *     @sessionh:      (IN) PTP session handle
 *     @obj_handle:    (IN) PTP object handle
 *     @obj_prop_code: (IN) MTP object property code
 *
 * Return value:        0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t mtp_obj_prop_changed(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle, mtp_obj_prop_t obj_prop_code);

/**
 * Function name:  mtp_obj_prop_desc_changed
 * Description:    Notifies the host of an ObjectPropDescChanged event, which
 *                 occurs when an object property description dataset has been
 *                 updated, indicating some change on the device.
 *                 The function notifies the host of the change.
 * Parameters:
 *     @sessionh:        (IN) PTP session handle
 *     @obj_prop_code:   (IN) MTP object property code
 *     @obj_format_code: (IN) PTP object format code
 *
 * Return value:        0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t mtp_obj_prop_desc_changed(ptp_sessionh_t sessionh,
    mtp_obj_prop_t obj_prop_code, ptp_obj_format_t obj_format_code);

/**
 * Function name:  mtp_obj_references_changed
 * Description:    Notifies the host of an ObjectReferencesChanged event, which
 *                 occurs when the references on an object have been updated.
 *                 The function notifies the host of the change.
 * Parameters:
 *     @sessionh:   (IN) PTP session handle
 *     @obj_handle: (IN) PTP object handle
 *
 * Return value:   
 * Scope:          Global
 **/
jresult_t mtp_obj_references_changed(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle);


/****************************
 * MTP Initialization API
 ****************************/

/* MTP Application Callbacks Structure */
typedef struct 
{
    /* Basic operation */
    mtp_get_obj_props_supp      get_obj_props_supp_h;
    mtp_get_obj_prop_desc       get_obj_prop_desc_h;
    mtp_get_obj_prop_val        get_obj_prop_val_h;
    mtp_set_obj_prop_val        set_obj_prop_val_h;
    mtp_get_obj_refs            get_obj_refs_h;
    mtp_set_obj_refs            set_obj_refs_h;
    mtp_skip                    skip_h;

#ifdef CONFIG_PROTO_PTP_MTP_ENHANCED
    /* Enhanced operation */
    mtp_get_obj_prop_list       get_obj_prop_list_h;
    mtp_set_obj_prop_list       set_obj_prop_list_h;
    mtp_get_interdep_prop_desc  get_interdep_prop_desc_h;
    mtp_send_obj_prop_list      send_obj_prop_list_h;
#endif
} mtp_callbacks_t;

/**
 * Function name:  mtp_init
 * Description:    Enables usage of the MTP API.
 *                 The function receives and stores the MTP application's
 *                 callback functions (cbs) and a PTP instance handle.
 *                 In order to enable usage of the MTP API as part of the SICD
 *                 FD, this function must be called from the SICD application
 *                 initialization function -- sicd_app_init().
 * Requirement:    Mandatory
 * Parameters:
 *     @insth: (IN) PTP instance handle
 *     @cbs:   (IN) Pointer to an MTP application callbacks structure
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t mtp_init(ptp_instanceh_t insth, mtp_callbacks_t *cbs);

#endif /* _MTP_API_H_ */

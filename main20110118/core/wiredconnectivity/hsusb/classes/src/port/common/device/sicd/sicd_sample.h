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

#ifndef __SICD_SAMPLE_H__
#define __SICD_SAMPLE_H__

#include <uw_device_sicd.h>
#include <jos.h>

#define SICD_MAX_STORES 8

#ifdef CONFIG_FD_MTP_APP
typedef struct 
{
    mtp_obj_prop_t           prop_code;
    mtp_obj_prop_desc_t     *desc;
    ptp_prop_value_t        *current_value;
} obj_proper_t;
#endif

typedef struct _sicd_object
{
    ptp_obj_handle_t id;
    ptp_obj_info_t info;
    juint8_t *data;
    juint8_t *sample_data;
#ifdef CONFIG_FD_MTP_APP
    obj_proper_t       *obj_props;
    juint32_t            obj_props_count;
    ptp_obj_handle_t   *ref_handles;
    juint32_t            ref_handles_count;
#endif
    struct _sicd_object *next;
} sicd_object_t;

typedef struct _sicd_store
{
    ptp_storage_id_t id;
    ptp_storage_info_t info;
    sicd_object_t *objects;
    struct _sicd_store *next;
} sicd_store_t;


jresult_t sicd_store_add(ptp_storage_id_t id, ptp_storage_info_t *info);

jresult_t sicd_store_remove(ptp_storage_id_t id);

jresult_t sicd_store_add_object(ptp_storage_id_t store_id, sicd_object_t *obj);

jresult_t sicd_store_remove_object(ptp_storage_id_t store_id,
    ptp_obj_handle_t obj_id);

sicd_store_t* sicd_find_store(ptp_storage_id_t store_id);

sicd_object_t *sicd_store_find_object(ptp_storage_id_t store_id,
    ptp_obj_handle_t obj_id);

ptp_obj_handle_t sicd_get_new_obj_handle(void);

sicd_store_t* sicd_store_get_first(void);

void sicd_free_object(sicd_store_t *store, sicd_object_t *obj);

#endif /*__SICD_SAMPLE_H__*/


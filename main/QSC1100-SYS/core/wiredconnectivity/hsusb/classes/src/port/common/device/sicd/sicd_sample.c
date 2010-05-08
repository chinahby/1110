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

#include "sicd_sample.h"
#include "dev_props.h"
#include "obj_props.h"

#define SICD_SAMPLE_ONE

/*
 * Static objects that simulate storage on a SICD device
 */

/* Our virtual storage */
static sicd_store_t *sicd_stores = NULL;

/* Prototypes */
#ifdef CONFIG_FD_DPS_APP
jresult_t dps_app_init(ptp_instanceh_t insth);
#endif

#ifdef CONFIG_FD_MTP_APP
jresult_t mtp_app_init(ptp_instanceh_t insth);
#endif

/* Our (global) appliaction context */
typedef struct
{
    sicd_object_t *new_send_obj;
} app_data_t;

/* Our session context */
typedef struct
{
    jint_t dummy;
    app_data_t *app_data;    
} sess_data_t;

/* Supported PTP capabilities */
enum ptp_operation supported_ops[] =
{
    PTP_OP_OPEN_SESSION,
    PTP_OP_GET_DEVICE_INFO,
    PTP_OP_CLOSE_SESSION,
    PTP_OP_GET_OBJECT,
    PTP_OP_GET_PARTIAL_OBJECT,
    PTP_OP_GET_OBJECT_INFO,
    PTP_OP_SEND_OBJECT,
    PTP_OP_SEND_OBJECT_INFO,
    PTP_OP_GET_STORAGE_INFO,
    PTP_OP_GET_STORAGE_IDS,
    PTP_OP_GET_NUM_OBJECTS,
    PTP_OP_GET_OBJECT_HANDLES,
    PTP_OP_GET_THUMB,
    PTP_OP_DELETE_OBJECT,
#ifdef CONFIG_FD_SICD_APP_DEVPROPS
    PTP_OP_GET_DEVICE_PROP_DESC,
    PTP_OP_GET_DEVICE_PROP_VALUE,
    PTP_OP_SET_DEVICE_PROP_VALUE,
    PTP_OP_RESET_DEVICE_PROP_VALUE,
#endif
#ifdef CONFIG_FD_MTP_APP
    MTP_OP_GET_OBJECT_PROPS_SUPPORTED,
    MTP_OP_GET_OBJECT_PROP_DESC,
    MTP_OP_GET_OBJECT_PROP_VALUE,
    MTP_OP_SET_OBJECT_PROP_VALUE,
    MTP_OP_GET_OBJECT_REFERENCES,
    MTP_OP_SET_OBJECT_REFERENCES,
#ifdef CONFIG_PROTO_PTP_MTP_ENHANCED
    MTP_OP_GET_OBJECT_PROP_LIST,
    MTP_OP_SET_OBJECT_PROP_LIST,
    MTP_OP_GET_INTERDEP_PROP_DESC,
    MTP_OP_SEND_OBJECT_PROP_LIST,
#endif
#endif
    PTP_OP_LAST
};

enum ptp_event supported_events[] =
{
    PTP_EVENT_OBJECT_ADDED,
    PTP_EVENT_OBJECT_REMOVED,
    PTP_EVENT_REQUEST_OBJECT_TRANSFER,
#ifdef CONFIG_FD_MTP_APP
    MTP_EVENT_OBJECT_PROP_CHANGED,
    MTP_EVENT_OBJECT_PROP_DESC_CHANGED,
    MTP_EVENT_OBJECT_REFERENCES_CHANGED,
#endif
    PTP_EVENT_LAST
};

enum ptp_object_format supported_image_formats[] =
{
    PTP_OBJ_FORMAT_IMG_JPEG,
#ifdef CONFIG_FD_MTP_APP
    PTP_OBJ_FORMAT_UNDEFINED,
    PTP_OBJ_FORMAT_MP3,
    PTP_OBJ_FORMAT_WAV,
    MTP_OBJ_FORMAT_WMA,
    MTP_OBJ_FORMAT_OGG,
#endif
    PTP_OBJ_FORMAT_LAST
};

/*
 * PTP handlers
 */
static ptp_response_t handle_dev_info(ptp_sessionh_t sessionh)
{
    ptp_dev_info_t devinfo;
#ifdef CONFIG_FD_SICD_APP_DEVPROPS
    juint_t i;
    ptp_dev_prop_t supported_props[NUM_DEV_PROPS+1];
#endif
    DECLARE_FNAME("handle_dev_info");
    
    DBG_X(DSLAVE_SICD_API, ("%s: entered\n", fname));

    j_memset(&devinfo, 0, sizeof(devinfo));
    devinfo.func_mode = PTP_FMODE_STANDARD;
    devinfo.supported_ops = supported_ops;
    devinfo.supported_events = supported_events;
#ifdef CONFIG_FD_SICD_APP_DEVPROPS
    for (i=0; i<NUM_DEV_PROPS; i++)
        supported_props[i] = devprop_desc_map[i].prop_code;
    supported_props[i] = PTP_DEV_PROP_LAST;
    devinfo.supported_props = supported_props;
#endif
    devinfo.image_formats = supported_image_formats;
    devinfo.manufacturer = ptp_string_atow("Jungo");
    devinfo.model = ptp_string_atow("JuMP");
    devinfo.dev_ver = ptp_string_atow("1.0");
    devinfo.serial_num = ptp_string_atow("XTZ4U");
    
    ptp_get_dev_info_complete(sessionh, PTP_RESP_OK, &devinfo);

    ptp_string_free(devinfo.manufacturer);
    ptp_string_free(devinfo.dev_ver);
    ptp_string_free(devinfo.model);
    ptp_string_free(devinfo.serial_num);
    return PTP_RESP_OK;
}

static ptp_response_t handle_open_session(ptp_sessionh_t sessionh,
    juint32_t session_id, ptp_appctx_t global_ctx, ptp_appctx_t *sess_ctx)
{
#ifdef SICD_SAMPLE_TWO
    sess_data_t *ctx;
#endif
    DECLARE_FNAME("handle_open_session");

    DBG_X(DSLAVE_SICD_API, ("%s: entered, id=%u\n", fname, session_id));

#ifdef SICD_SAMPLE_ONE
    /* We are a single session device - point to our application's context */
    *sess_ctx = global_ctx;
#endif
    
#ifdef SICD_SAMPLE_TWO
    ctx = jmalloc(sizeof(sess_data_t), M_ZERO);
    if (!ctx)
        return PTP_RESP_GENERAL_ERROR;

    /* Keep a pointer to our global data in each session context */
    ctx->app_data = (app_data_t*)global_ctx;

    /* Hand our session context to PTP */
    *sess_ctx = (ptp_appctx_t)ctx;
#endif

    return PTP_RESP_OK;
}

static ptp_response_t handle_close_session(ptp_sessionh_t sessionh,
    juint32_t session_id)
{
#ifdef SICD_SAMPLE_TWO
    sess_data_t *ctx = PTP_GET_APPCTX(sessionh);
#endif
    DECLARE_FNAME("handle_close_session");

    DBG_X(DSLAVE_SICD_API, ("%s: entered, id=%u\n", fname, session_id));

#ifdef SICD_SAMPLE_TWO
    if (ctx)
        jfree(ctx);
#endif

    return PTP_RESP_OK;
}

static ptp_response_t handle_storage_ids(ptp_sessionh_t sessionh)
{
    ptp_storage_id_t storage_ids[SICD_MAX_STORES];
    sicd_store_t *store;
    jint_t i = 0;
    DECLARE_FNAME("handle_storage_ids");

    DBG_X(DSLAVE_SICD_API, ("%s: entered\n", fname));

    for (store = sicd_store_get_first();
        store && i<SICD_MAX_STORES; store=store->next, i++)
    {
        storage_ids[i] = store->id;
    }

    ptp_get_storage_ids_complete(sessionh, PTP_RESP_OK, storage_ids, i);
    return PTP_RESP_OK;
}

static ptp_response_t handle_storage_info(ptp_sessionh_t sessionh,
    ptp_storage_id_t store)
{
    sicd_store_t *s;
    DECLARE_FNAME("handle_storage_info");

    DBG_X(DSLAVE_SICD_API, ("%s: entered. store=%X\n", fname, store));

    s = sicd_find_store(store);
    if (!s)
        return PTP_RESP_INVALID_STORAGE_ID;

    ptp_get_storage_info_complete(sessionh, PTP_RESP_OK, &s->info);

    return PTP_RESP_OK;
}

static jbool_t is_image_format(ptp_obj_format_t format)
{
    if (format >= PTP_OBJ_FORMAT_IMG_UNKNOWN || 
        format <= PTP_OBJ_FORMAT_IMG_JPX 
#ifdef CONFIG_FD_MTP_APP
        || format == MTP_OBJ_FORMAT_WIN_IMG_FMT
#endif
        )
    {
        return 1;
    }
    return 0;
}

static juint32_t get_num_objs(ptp_storage_id_t store_id,
    ptp_obj_format_t format, ptp_obj_handle_t parent_folder)
{
    sicd_store_t *store;
    sicd_object_t *obj = NULL;
    juint32_t nhandles = 0;
    DECLARE_FNAME("get_num_objs");

    DBG_X(DSLAVE_SICD_API, ("%s: entered. store=%X, format=%X, parent=%X\n",
        fname, store_id, format, parent_folder));
    
    store = sicd_store_get_first();
    while (store)
    {
        if ((store_id != PTP_STORAGE_ID_ALL) &&
            (store_id != PTP_STORAGE_ID_ANY) &&
            (store->id != store_id))
        {
            store = store->next;
            continue;
        }
        
        obj = store->objects;
        while (obj)
        {
            if ((format == PTP_OBJ_FORMAT_ALL) ||
                (format == PTP_OBJ_FORMAT_IMAGES && 
                is_image_format(obj->info.format)) ||
                obj->info.format == format)
        {
            nhandles++;
        }        

            obj = obj->next;
        }
        
        store = store->next;
    }
    
    DBG_X(DSLAVE_SICD_API, ("%s: %u objects found\n", fname, nhandles));
    return nhandles;
}

static ptp_response_t handle_num_objs(ptp_sessionh_t sessionh,
    ptp_storage_id_t store_id, ptp_obj_format_t format,
    ptp_obj_handle_t parent_folder)
{
    juint32_t nhandles = 0;
    DECLARE_FNAME("handle_num_objs");

    DBG_X(DSLAVE_SICD_API, ("%s: entered. store=%X, format=%X, parent=%X\n",
        fname, store_id, format, parent_folder));

    nhandles = get_num_objs(store_id, format, parent_folder);

    DBG_X(DSLAVE_SICD_API, ("%s: %u objects found\n", fname, nhandles));
    ptp_get_num_objects_complete(sessionh, PTP_RESP_OK, nhandles);
    return PTP_RESP_OK;
}

static ptp_response_t handle_obj_handles(ptp_sessionh_t sessionh,
    ptp_storage_id_t store_id, ptp_obj_format_t format,
    ptp_obj_handle_t folder)
{
    ptp_obj_handle_t *handles = NULL;
    sicd_store_t *store;
    sicd_object_t *obj;
    jint_t nhandles = 0;
    DECLARE_FNAME("handle_obj_handles");

    DBG_X(DSLAVE_SICD_API, ("%s: entered. store=%X, format=%X, folder=%X\n",
        fname, store_id, format, folder));

    nhandles = get_num_objs(store_id, format, folder);
    if (nhandles)
    {
        handles = jmalloc(sizeof(ptp_obj_handle_t)*nhandles, M_ZERO);
        if (!handles)
            return PTP_RESP_GENERAL_ERROR;
    }
    
    store = sicd_stores;
    nhandles = 0;
    while (store)
    {
        if ((store_id != PTP_STORAGE_ID_ALL) &&
            (store_id != PTP_STORAGE_ID_ANY) &&
            (store->id != store_id))
        {
            store = store->next;
            continue;
        }
        
        obj = store->objects;
        while (obj)
        {
            if ((format == PTP_OBJ_FORMAT_ALL) ||
                (format == PTP_OBJ_FORMAT_IMAGES && 
                is_image_format(obj->info.format)) ||
                obj->info.format == format)
            {
                handles[nhandles++] = obj->id;

                DBG_X(DSLAVE_SICD_API, ("%s: %u objects found. handle=%X\n",
                    fname, nhandles, handles[nhandles-1]));
            }

            obj = obj->next;
        }
        
        store = store->next;
    }

    ptp_get_obj_handles_complete(sessionh, PTP_RESP_OK, handles,
        nhandles);

    if (handles)
        jfree(handles);

    return PTP_RESP_OK;
}

static ptp_response_t handle_get_obj_info(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_id)
{
    sicd_object_t *obj;
    DECLARE_FNAME("handle_get_obj_info");

    DBG_X(DSLAVE_SICD_API, ("%s: entered. objh=%X\n", fname, obj_id));

    obj = sicd_store_find_object(PTP_STORAGE_ID_ANY, obj_id);
    if (!obj)
        return PTP_RESP_INVALID_OBJECT_HANDLE;

    DBG_X(DSLAVE_SICD_API, ("%s: obj_size=%u, obj_name=%s\n", fname,
        obj->info.compressed_size, obj->info.filename));

    ptp_get_obj_info_complete(sessionh, PTP_RESP_OK, &obj->info);
    return PTP_RESP_OK;
}

static ptp_response_t handle_get_obj(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_id, void *data, juint32_t max_size, juint32_t offset)
{
    sicd_object_t *obj;
    juint32_t act_size;
    DECLARE_FNAME("handle_get_obj");
    
    DBG_X(DSLAVE_SICD_API, ("%s: entered. objh=%X, offset=%d, max_size=%d\n",
        fname, obj_id, offset, max_size));
    
    obj = sicd_store_find_object(PTP_STORAGE_ID_ANY, obj_id);
    if (!obj)
        return PTP_RESP_INVALID_OBJECT_HANDLE;
    
    act_size = MIN(max_size, obj->info.compressed_size-offset);

    DBG_X(DSLAVE_SICD_API, ("%s: obj_size=%u, act_size=%u, obj_id=%u, "
        "obj_name=%s\n", fname, act_size, obj->info.compressed_size,
        obj->id, obj->info.filename));

    j_memcpy(data, obj->data+offset, act_size);
    ptp_get_obj_complete(sessionh, PTP_RESP_OK, act_size,
        obj->info.compressed_size);
    
    return PTP_RESP_OK;
}

static ptp_response_t handle_get_thumb(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_id, void *data, juint32_t max_size, juint32_t offset)
{
    sicd_object_t *obj;
    juint32_t act_size;
    DECLARE_FNAME("handle_get_thumb");
    
    DBG_X(DSLAVE_SICD_API, ("%s: entered. objh=%X, offset=%d, max_size=%d\n",
        fname, obj_id, offset, max_size));

    obj = sicd_store_find_object(PTP_STORAGE_ID_ANY, obj_id);
    if (!obj)
        return PTP_RESP_INVALID_OBJECT_HANDLE;
    
    act_size = MIN(max_size, obj->info.thumb_compressed_size-offset);
    
    DBG_X(DSLAVE_SICD_API, ("%s: thumb_size=%u, act_size=%u, obj_id=%u, "
        "obj_name=%s\n", fname, act_size, obj->info.thumb_compressed_size,
        obj->id, obj->info.filename));

    j_memcpy(data, obj->sample_data+offset, act_size);
    ptp_get_obj_complete(sessionh, PTP_RESP_OK, act_size,
        obj->info.thumb_compressed_size);
    
    return PTP_RESP_OK;
}

static ptp_response_t handle_send_obj_info(ptp_sessionh_t sessionh,
    ptp_obj_info_t *objinfo, ptp_storage_id_t target_store,
    ptp_obj_handle_t target_folder)
{
    app_data_t *ctx = PTP_GET_APPCTX(sessionh);
    sicd_store_t *store;
    sicd_object_t *sicd_obj = NULL;
    ptp_response_t resp = PTP_RESP_GENERAL_ERROR;

    if (target_store == PTP_STORAGE_ID_ANY)
        target_store = PTP_MAKE_STORAGE_ID(1,1);

    store = sicd_find_store(target_store);
    if (!store)
    {
        resp = PTP_RESP_INVALID_STORAGE_ID;
        goto Error;
    }

    /* We are a flat storage */
    if (target_folder != PTP_OBJH_FOLDER_ROOT &&
        target_folder != PTP_OBJH_FOLDER_ANY)
    {
        resp = PTP_RESP_SPEC_OF_DEST_UNSUPPORTED;
        goto Error;
    }
    
    if (store->info.free_space.low < 
        objinfo->compressed_size + objinfo->thumb_compressed_size)
    {
        resp = PTP_RESP_STORE_FULL;
        goto Error;
    }

    /* Ignore a previous send_object_info not followed by send_object */
    if (ctx->new_send_obj)
        sicd_free_object(NULL, ctx->new_send_obj);
    
    ctx->new_send_obj = sicd_obj = jmalloc(sizeof(sicd_object_t), M_ZERO);
    if (!sicd_obj)
    {
        resp = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }

    sicd_obj->id = sicd_get_new_obj_handle();
    sicd_obj->info.storage_id = target_store;
    sicd_obj->info.format = objinfo->format;
    sicd_obj->info.compressed_size = objinfo->compressed_size;
    sicd_obj->info.thumb_format = objinfo->thumb_format;
    sicd_obj->info.thumb_compressed_size = objinfo->thumb_compressed_size;
    sicd_obj->info.filename = ptp_string_duplicate(objinfo->filename);

    /* If this object does not hold data, i.e. we do not expect a SendObj
     * right after this operation, skip the data preparation phase
     * and clear ctx->new_send_obj */

    if (sicd_obj->info.format == PTP_OBJ_FORMAT_ASSOCIATION ||
        sicd_obj->info.compressed_size == 0)
    {
        sicd_store_add_object(sicd_obj->info.storage_id, sicd_obj);
        ctx->new_send_obj = NULL;
        goto Done;
    }
    
    sicd_obj->data = (juint8_t*)jmalloc(sicd_obj->info.compressed_size, M_ZERO);
    if (!sicd_obj->data)
        goto Error;
    
    if (objinfo->thumb_compressed_size)
    {
        sicd_obj->sample_data =
            (juint8_t*)jmalloc(sicd_obj->info.thumb_compressed_size, 0);
    
        if (!sicd_obj->sample_data)
            goto Error;
    }

Done:
    ptp_send_obj_info_complete(sessionh, PTP_RESP_OK, target_store,
        PTP_OBJH_FOLDER_ROOT, sicd_obj->id);

    return PTP_RESP_OK;

Error:
    if (sicd_obj)
    {
        sicd_free_object(NULL, sicd_obj);
        ctx->new_send_obj = NULL;
    }
    
    return resp;
}

static ptp_response_t handle_send_obj(ptp_sessionh_t sessionh,
    void *data, juint32_t size, juint32_t offset)
{
    app_data_t *ctx = PTP_GET_APPCTX(sessionh);
    juint8_t *target;

    if (!ctx->new_send_obj)
    {
        DBG_E(DSLAVE_SICD_API, ("handle_send_obj: invalid new_send_obj\n"));
        return PTP_RESP_GENERAL_ERROR;
    }

    target = (juint8_t*)ctx->new_send_obj->data;
    target += offset;
    
    j_memcpy(target, data, size);
    
    ptp_send_obj_complete(sessionh, PTP_RESP_OK);
    return PTP_RESP_OK;
}

static ptp_response_t handle_delete_obj(ptp_sessionh_t sessionh,
    ptp_obj_handle_t handle, ptp_obj_format_t format)
{
    if (sicd_store_remove_object(PTP_STORAGE_ID_ALL, handle))
        return PTP_RESP_INVALID_OBJECT_HANDLE;

    ptp_operation_complete(sessionh, PTP_RESP_OK);
    return PTP_RESP_OK;
}

#ifdef CONFIG_FD_SICD_APP_DEVPROPS

static ptp_dev_prop_desc_t* get_dev_prop_desc(ptp_dev_prop_t dev_prop_code)
{
    jint_t i;
    ptp_dev_prop_desc_t *desc = NULL;
    
    for (i=0; i<NUM_DEV_PROPS; i++)
    {
        if (devprop_desc_map[i].prop_code == dev_prop_code)
        {
            desc = &devprop_desc_map[i];
            break;
        }
    }

    return desc;
}

static ptp_response_t handle_dev_prop_desc(ptp_sessionh_t sessionh,
    ptp_dev_prop_t dev_prop_code)
{
    ptp_dev_prop_desc_t *desc;
    DECLARE_FNAME("handle_dev_prop_desc");

    DBG_X(DSLAVE_SICD_API, ("%s: prop_code=%X\n", fname, dev_prop_code));

    /* Find property */
    desc = get_dev_prop_desc(dev_prop_code); 
    if (!desc)
        return PTP_RESP_DEVICE_PROP_NOT_SUPPORTED;

    if (ptp_get_dev_prop_desc_complete(sessionh, PTP_RESP_OK, desc))
    {
        DBG_E(DSLAVE_SICD_API, ("%s: error on "
            "ptp_get_dev_desc_complete(), propcode=%X\n", fname,
            dev_prop_code));
    }

    return PTP_RESP_OK;
}

static ptp_response_t handle_reset_dev_prop_val(ptp_sessionh_t sessionh,
    ptp_dev_prop_t dev_prop_code)
{
    ptp_dev_prop_desc_t *desc;
    DECLARE_FNAME("handle_reset_dev_prop_val");

    DBG_X(DSLAVE_SICD_API, ("%s: prop=%X\n", fname, dev_prop_code));
    
    /* Find property */
    desc = get_dev_prop_desc(dev_prop_code); 
    if (!desc)
        return PTP_RESP_DEVICE_PROP_NOT_SUPPORTED;

    /* Set current value to default */
    ptp_prop_value_clear(desc->current_value);
    ptp_prop_value_copy(desc->current_value, desc->default_value);
    
    if (ptp_operation_complete(sessionh, PTP_RESP_OK))
    {
        DBG_E(DSLAVE_SICD_API, ("%s: error on "
            "ptp_operation_complete(), propcode=%X\n", fname,
            dev_prop_code));
    }
    
    return PTP_RESP_OK;
}

static ptp_response_t handle_get_dev_prop_val(ptp_sessionh_t sessionh,
    ptp_dev_prop_t dev_prop_code)
{
    ptp_dev_prop_desc_t *desc;
    DECLARE_FNAME("handle_get_dev_prop_val");

    DBG_X(DSLAVE_SICD_API, ("%s: prop=%X\n", fname, dev_prop_code));

    /* Find property */
    desc = get_dev_prop_desc(dev_prop_code); 
    if (!desc)
        return PTP_RESP_DEVICE_PROP_NOT_SUPPORTED;

    if (ptp_get_dev_prop_val_complete(sessionh, PTP_RESP_OK,
        desc->current_value))
    {
        DBG_E(DSLAVE_SICD_API, ("%s: error on "
            "ptp_get_dev_prop_val_complete(), propcode=%X\n", fname,
            dev_prop_code));
    }
    
    return PTP_RESP_OK;
}

static ptp_response_t handle_set_dev_prop_val(ptp_sessionh_t sessionh,
    ptp_dev_prop_t dev_prop_code, ptp_prop_value_t *dev_prop_val)
{
    ptp_dev_prop_desc_t *desc;
    DECLARE_FNAME("handle_set_dev_prop_val");

    DBG_X(DSLAVE_SICD_API, ("%s: prop=%X\n", fname, dev_prop_code));

    /* Find property */
    desc = get_dev_prop_desc(dev_prop_code); 
    if (!desc)
        return PTP_RESP_DEVICE_PROP_NOT_SUPPORTED;

    /* Set new current value */
    ptp_prop_value_clear(desc->current_value);
    ptp_prop_value_copy(desc->current_value, dev_prop_val);

    if (ptp_set_dev_prop_val_complete(sessionh, PTP_RESP_OK))
    {
        DBG_E(DSLAVE_SICD_API, ("%s: error on "
            "ptp_set_dev_prop_val_complete(), propcode=%X\n", fname,
            dev_prop_code));
    }

    return PTP_RESP_OK;
}
#endif

static void handle_trans_complete(ptp_sessionh_t sessionh,
    ptp_io_status_t result, ptp_operation_t operation,
    ptp_response_t response, juint32_t obj_size, juint32_t bytes_transferred)
{
    app_data_t *ctx;
    DECLARE_FNAME("handle_trans_complete");

    DBG_X(DSLAVE_SICD_API, ("%s: result=%u, operation=%X, response=%X, "
        "obj_size=%u, bytes_transferred=%u\n", fname, result, operation,
        response, obj_size, bytes_transferred));

    if (!sessionh)
        return;

    ctx = PTP_GET_APPCTX(sessionh);

    if (operation == PTP_OP_SEND_OBJECT && ctx->new_send_obj)
    {
        sicd_store_add_object(ctx->new_send_obj->info.storage_id, 
            ctx->new_send_obj);
        ctx->new_send_obj = NULL;
    }
}

/*============================
 *   Storage handling
 *==========================*/

#define PTP_STRING_FREE(str)    \
    if (str)                    \
    {                           \
        ptp_string_free(str);   \
        str = NULL;             \
    }

sicd_store_t* sicd_find_store(ptp_storage_id_t store_id)
{
    sicd_store_t *store;

    /* Find store */
    for (store = sicd_store_get_first();
        store && store->id != store_id;
        store = store->next)
        ;
    
    return store;
}

void sicd_free_object(sicd_store_t *store, sicd_object_t *obj)
{
    if (store)
    {
        store->info.free_space.low += obj->info.compressed_size +
            obj->info.thumb_compressed_size;
        store->info.free_images = store->info.free_space.low / (50 * 1024);
    }

    PTP_STRING_FREE(obj->info.filename);
    PTP_STRING_FREE(obj->info.capture_date);
    PTP_STRING_FREE(obj->info.modification_date);
    PTP_STRING_FREE(obj->info.keywords);

    if (obj->data)
        jfree(obj->data);
    if (obj->sample_data)
        jfree(obj->sample_data);

    /* Free Object Properties */
#ifdef CONFIG_FD_MTP_APP
    objprops_free(obj);
#endif

    /* TODO: Free object references */

    jfree(obj);
}

jresult_t sicd_store_add(ptp_storage_id_t id, ptp_storage_info_t *info)
{
    sicd_store_t *store;

    store = (sicd_store_t*)jmalloc(sizeof(sicd_store_t), M_ZERO);
    if (!store)
        return JENOMEM;

    j_memcpy(&store->info, info, sizeof(ptp_storage_info_t));
    store->id = id;

    store->next = sicd_stores;
    sicd_stores = store;
    return 0;
}

jresult_t sicd_store_remove(ptp_storage_id_t id)
{
    sicd_store_t **store;
    sicd_store_t *r_store = NULL;
    sicd_object_t *obj;

    /* Find & remove store */
    for (store = &sicd_stores; *store; store = &((*store)->next))
    {
        if ((*store)->id == id)
        {
            r_store = *store;
            *store = (*store)->next;
            break;
        }
    }

    if (!r_store)
        return JENOENT;

    /* Free all objects in store */
    obj = r_store->objects;
    while (obj)
    {
        r_store->objects = obj->next;
        sicd_free_object(r_store, obj);
        obj = r_store->objects;
    }

    jfree(r_store);
    return 0;
}

jresult_t sicd_store_add_object(ptp_storage_id_t store_id, sicd_object_t *obj)
{
#ifdef CONFIG_FD_MTP_APP
    jresult_t rc;
#endif
    sicd_store_t *store;
    DECLARE_FNAME("sicd_store_add_object");

    /* Find store */
    store = sicd_find_store(store_id);
    if (!store)
    {
        DBG_W(DSLAVE_SICD_API, ("%s: storage not "
            "found (%X)\n", fname, store_id));
        return JENOENT;
    }

#ifdef CONFIG_FD_MTP_APP
    /* Initialize object properties */
    rc = objprops_setup_obj(obj);
    if (rc == JENOMEM)
    {
        DBG_E(DSLAVE_MTP_API, ("%s: cant set object property", fname));
        return rc;
    }

    obj->ref_handles = NULL;
    obj->ref_handles_count = 0;
#endif

    obj->next = store->objects;
    store->objects = obj;

    store->info.free_space.low -= obj->info.compressed_size +
        obj->info.thumb_compressed_size;
    store->info.free_images = store->info.free_space.low / (50 * 1024);
    return 0;
}

jresult_t sicd_store_remove_object(ptp_storage_id_t store_id,
    ptp_obj_handle_t obj_id)
{
    sicd_object_t **obj = NULL;
    sicd_object_t *r_obj = NULL;
    sicd_store_t *store;

    store = sicd_stores;
    while (store)
    {
        if (store_id != PTP_STORAGE_ID_ANY &&
            store_id != PTP_STORAGE_ID_ALL &&
            store->id != store_id)
        {
            store = store->next;
            continue;
        }

        /* Find & remove object */
        for (obj = &(store->objects); *obj ; obj = &((*obj)->next))
        {
            if ((*obj)->id == obj_id)
            {
                r_obj = *obj;
                *obj = (*obj)->next;
                break;
            }
        }

        if (r_obj)
        {
            sicd_free_object(store, r_obj);
            return 0;
        }
        
        store = store->next;
    }
    
    return JENOENT;
}

sicd_object_t* sicd_store_find_object(ptp_storage_id_t store_id,
    ptp_obj_handle_t obj_id)
{
    sicd_store_t *store;
    sicd_object_t *obj = NULL;
    
    store = sicd_stores;
    while (store)
    {
        if (store_id != PTP_STORAGE_ID_ALL &&
            store_id != PTP_STORAGE_ID_ANY &&
            store->id != store_id)
        {
            store = store->next;
            continue;
        }

        obj = store->objects;
        while (obj)
        {
            if (obj->id == obj_id)
                return obj;
            
            obj = obj->next;
    }
        
        store = store->next;
    }

    return NULL;
}

ptp_obj_handle_t sicd_get_new_obj_handle()
{
    static juint32_t handle = 0x11111110;
    if (++handle > 0xFFFFFF00)
        handle = 0x11111110;
    return handle;
}

sicd_store_t* sicd_store_get_first(void)
{
    return sicd_stores;
}

static void init_storage(void)
{
    ptp_storage_info_t storage_info;
    
    j_memset(&storage_info, 0, sizeof(storage_info));

    storage_info.type = PTP_STOR_TYPE_REMOVABLE_RAM;
    storage_info.fs_type = PTP_FS_GENERIC_FLAT;
    storage_info.access = PTP_ACCESS_READ_WRITE;
    storage_info.max_capacity.high = 0;
    storage_info.max_capacity.low = 2 * 1024 * 1024 * 1000;
    storage_info.free_space.high = 0;
    storage_info.free_space.low = storage_info.max_capacity.low;
    storage_info.free_images = storage_info.free_space.low / (50 * 1024);
    storage_info.desc = NULL;
    storage_info.label = NULL;

    sicd_store_add(PTP_MAKE_STORAGE_ID(1,1), &storage_info);
}

static void delete_storage(void)
{
    while (sicd_stores)
        sicd_store_remove(sicd_stores->id);
}

static void handle_unregister(ptp_instanceh_t insth, ptp_appctx_t global_ctx)
{
    delete_storage();

#ifdef CONFIG_FD_SICD_APP_DEVPROPS
    devprops_free();
#endif

#ifdef CONFIG_FD_MTP_APP
    objprops_free_descs();
#endif

    /* Free our global context */
    if (global_ctx)
        jfree(global_ctx);
}

jresult_t sicd_app_init(ptp_instanceh_t insth, sicd_init_info_t *info)
{
    ptp_responder_cbs ptp_cbs;
    app_data_t *gctx;
    jresult_t rc = 0;
    DECLARE_FNAME("sicd_app_init");

    /* Init our global context */
    gctx = (app_data_t*)jmalloc(sizeof(app_data_t), M_ZERO);
    if (!gctx)
    {
        DBG_E(DSLAVE_SICD_API, ("%s: failed to allocate global context\n",
            fname));
        goto Exit;
    }
   
#ifdef CONFIG_TEST_STATIC_INTERFACE  
    /* TODO: Init interface number */
    info->interface_number = 2;
#endif

    /* Init our sample storage */
    init_storage();

    /* Init our device properties */
#ifdef CONFIG_FD_SICD_APP_DEVPROPS
    devprops_init();
#endif

    /* Register as a PTP application */
    j_memset(&ptp_cbs, 0, sizeof(ptp_cbs));
    ptp_cbs.open_session_h = handle_open_session;
    ptp_cbs.close_session_h = handle_close_session;
    ptp_cbs.get_dev_info_h = handle_dev_info;
    ptp_cbs.get_storage_ids_h = handle_storage_ids;
    ptp_cbs.get_storage_info_h = handle_storage_info;
    ptp_cbs.get_num_objs_h = handle_num_objs;
    ptp_cbs.get_obj_handles_h = handle_obj_handles;
    ptp_cbs.get_obj_info_h = handle_get_obj_info;
    ptp_cbs.get_obj_h = handle_get_obj;
    ptp_cbs.get_thumb_h = handle_get_thumb;
    ptp_cbs.send_obj_info_h = handle_send_obj_info;
    ptp_cbs.send_obj_h = handle_send_obj;
    ptp_cbs.delete_obj_h = handle_delete_obj;
#ifdef CONFIG_FD_SICD_APP_DEVPROPS
    ptp_cbs.get_dev_prop_desc_h = handle_dev_prop_desc;
    ptp_cbs.get_dev_prop_val_h = handle_get_dev_prop_val;
    ptp_cbs.set_dev_prop_val_h = handle_set_dev_prop_val;
    ptp_cbs.reset_dev_prop_val_h = handle_reset_dev_prop_val;
#endif
    ptp_cbs.transaction_complete_h = handle_trans_complete;
    ptp_cbs.unregister_h = handle_unregister;

    rc = ptp_register_responder(insth, &ptp_cbs, gctx);
    if (rc)
    {
        DBG_E(DSLAVE_SICD_API, ("%s: failed to register PTP (%d)\n", fname,
            rc));
        goto Exit;
    }

#ifdef CONFIG_FD_DPS_APP
    /* Initialize the DPS layer */
    rc = dps_app_init(insth);
    if (rc)
    {
        DBG_E(DSLAVE_SICD_API, ("%s: failed to initialize dps application "
            "(%d)\n", fname, rc));
        goto Exit;
    }
#endif

#ifdef CONFIG_FD_MTP_APP
    rc = mtp_app_init(insth);
    if (rc)
    {
        DBG_E(DSLAVE_SICD_API, ("%s: failed to initialize mtp application "
            "(%d)\n", fname, rc));
        goto Exit;
    }
#endif
    
Exit:
    if (rc && gctx)
        jfree(gctx);

    return rc;
}


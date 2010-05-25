

#include "jusb_common.h"
#include "hsu_al_sicd.h"


/*
 * Static objects that simulate storage on a SICD device
 */


/* Our virtual storage */
static sicd_store_t *sicd_stores = NULL;

/* Prototypes */
extern jresult_t dps_app_init(ptp_instanceh_t insth);

/* Our (global) appliaction context */
typedef struct
{
    sicd_object_t *new_send_obj;
} app_data_t;

app_data_t gctx;

/* Supported PTP capabilities */
enum ptp_operation supported_ops[] =
{
    PTP_OP_GET_DEVICE_INFO, /* dk : switched the orders */
    PTP_OP_OPEN_SESSION,
    PTP_OP_CLOSE_SESSION,
    PTP_OP_GET_STORAGE_IDS,
    PTP_OP_GET_STORAGE_INFO,
    PTP_OP_GET_NUM_OBJECTS,
    PTP_OP_GET_OBJECT_HANDLES,
    PTP_OP_GET_OBJECT_INFO,
    PTP_OP_GET_OBJECT,
    PTP_OP_GET_THUMB,
    PTP_OP_SEND_OBJECT_INFO,
    PTP_OP_SEND_OBJECT,
    PTP_OP_GET_PARTIAL_OBJECT,
    PTP_OP_LAST
};

enum ptp_event supported_events[] =
{
    PTP_EVENT_OBJECT_ADDED,
    PTP_EVENT_OBJECT_REMOVED,
    PTP_EVENT_REQUEST_OBJECT_TRANSFER,
    PTP_EVENT_LAST
};

enum ptp_object_format supported_capture_formats[] =
{
    PTP_OBJ_FORMAT_IMG_JPEG,
    PTP_OBJ_FORMAT_LAST
};

enum ptp_object_format supported_image_formats[] =
{
    PTP_OBJ_FORMAT_IMG_JPEG,
    PTP_OBJ_FORMAT_IMG_BMP,
    PTP_OBJ_FORMAT_IMG_GIF,
    PTP_OBJ_FORMAT_IMG_PNG,
    PTP_OBJ_FORMAT_DPOF,   
    PTP_OBJ_FORMAT_SCRIPT,
    PTP_OBJ_FORMAT_LAST
};

/*
 * PTP handlers
 */
static ptp_response_t handle_dev_info(ptp_sessionh_t sessionh)
{
    ptp_dev_info_t devinfo;
    
    HSU_MSG_HIGH("handle_dev_info: entered", 0,0,0);

    (void) j_memset(&devinfo, 0, sizeof(devinfo));
    devinfo.func_mode = PTP_FMODE_STANDARD;
    devinfo.supported_ops = supported_ops;
    devinfo.supported_events = supported_events; 
    devinfo.capture_formats = supported_capture_formats;
    devinfo.image_formats = supported_image_formats;
    devinfo.manufacturer = ptp_string_atow("Qualcomm");
    devinfo.model = ptp_string_atow("MSM7600");
    devinfo.dev_ver = ptp_string_atow("1.0");
    devinfo.serial_num = ptp_string_atow("12345678");
    
    (void) ptp_get_dev_info_complete(sessionh, PTP_RESP_OK, &devinfo);

    ptp_string_free(devinfo.manufacturer);
    ptp_string_free(devinfo.dev_ver);
    ptp_string_free(devinfo.model);
    ptp_string_free(devinfo.serial_num);
    return PTP_RESP_OK;
}

static ptp_response_t handle_open_session(ptp_sessionh_t sessionh,
    juint32_t session_id, ptp_appctx_t global_ctx, ptp_appctx_t *sess_ctx)
{
    HSU_USE_PARAM(sessionh);
    HSU_USE_PARAM(session_id);

    HSU_MSG_HIGH("handle_open_session: entered, id=%u", session_id,0,0);

    /* We are a single session device - point to our application's context */
    *sess_ctx = global_ctx;
    
    return PTP_RESP_OK;
}

static ptp_response_t handle_close_session(ptp_sessionh_t sessionh,
    juint32_t session_id)
{
    HSU_USE_PARAM(sessionh);
    HSU_USE_PARAM(session_id);

    HSU_MSG_HIGH("handle_close_session: entered, id=%u", session_id,0,0);

    return PTP_RESP_OK;
}

static ptp_response_t handle_storage_ids(ptp_sessionh_t sessionh)
{
    ptp_storage_id_t storage_ids[SICD_MAX_STORES];
    sicd_store_t *store;
    juint32_t i = 0;
    ptp_response_t ptp_rsp = PTP_RESP_OK;

    HSU_MSG_HIGH("handle_storage_ids: entered, sessionh=%X", sessionh,0,0);

    for (store = sicd_store_get_first();
        store && i<SICD_MAX_STORES; store=store->next, i++)
    {
        storage_ids[i] = store->id;
    }

    if (0 != ptp_get_storage_ids_complete(sessionh, PTP_RESP_OK, storage_ids, i))
    {
        return PTP_RESP_GENERAL_ERROR;
    }
    
    return ptp_rsp;
}

static ptp_response_t handle_storage_info(ptp_sessionh_t sessionh,
    ptp_storage_id_t store)
 {
    sicd_store_t *s;
    HSU_MSG_HIGH("handle_storage_info: entered, sessionh=%X", store,0,0);

    s = sicd_find_store(store);
    if (!s)
    {
        return PTP_RESP_INVALID_STORAGE_ID;
    }

    if (0 != ptp_get_storage_info_complete(sessionh, PTP_RESP_OK, &s->info))
    {
        return PTP_RESP_GENERAL_ERROR;
    }

    return PTP_RESP_OK;
}

static jbool_t is_image_format(ptp_obj_format_t format)
{
    if (format >= PTP_OBJ_FORMAT_IMG_UNKNOWN || 
        format <= PTP_OBJ_FORMAT_IMG_JPX )
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
    uint32 nhandles = 0;
    uint32 ptp_obj_format = (uint32)format;
    HSU_USE_PARAM(parent_folder);

    HSU_MSG_HIGH("get_num_objs: entered,  store=%X, format=%X, parent=%X", 
        store_id, format, parent_folder);
   
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
            if ((PTP_OBJ_FORMAT_ALL == ptp_obj_format) ||
                (PTP_OBJ_FORMAT_IMAGES == ptp_obj_format && 
                is_image_format(obj->info.format)) ||
                obj->info.format == format)
            {
                nhandles++;
            }        

            obj = obj->next;
        }
        
        store = store->next;
    }
    
    HSU_MSG_HIGH("get_num_objs: %u objects found", nhandles,0,0);
    return nhandles;
}

static ptp_response_t handle_num_objs(ptp_sessionh_t sessionh,
    ptp_storage_id_t store_id, ptp_obj_format_t format,
    ptp_obj_handle_t parent_folder)
{
    juint32_t nhandles = 0;

    HSU_MSG_HIGH("handle_num_objs: entered. store=%X, format=%X, parent=%X",
        store_id, format, parent_folder);

    nhandles = get_num_objs(store_id, format, parent_folder);

    if (0 != ptp_get_num_objects_complete(sessionh, PTP_RESP_OK, nhandles))
    {
        HSU_MSG_ERROR("handle_num_objs: ptp_get_num_objects_complete() failed",
            0,0,0);
        return PTP_RESP_GENERAL_ERROR;
    }
    
    return PTP_RESP_OK;
}

static ptp_response_t handle_obj_handles(ptp_sessionh_t sessionh,
    ptp_storage_id_t store_id, ptp_obj_format_t format,
    ptp_obj_handle_t folder)
{
    ptp_obj_handle_t *handles = NULL;
    sicd_store_t *store;
    sicd_object_t *obj;
    juint32_t nhandles = 0;
    ptp_response_t ptp_rsp = PTP_RESP_OK;
    uint32 ptp_obj_format = (uint32)format;

    HSU_MSG_HIGH("handle_obj_handles: entered. store=%X, format=%X, folder=%X",
        store_id, format, folder);

    nhandles = get_num_objs(store_id, format, folder);
    if (nhandles)
    {
        uint32 size = sizeof(ptp_obj_handle_t) * nhandles;
        handles = (ptp_obj_handle_t*)jmalloc(size, M_ZERO);
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
            if ((PTP_OBJ_FORMAT_ALL == ptp_obj_format) ||
                (PTP_OBJ_FORMAT_IMAGES == ptp_obj_format && 
                is_image_format(obj->info.format)) ||
                obj->info.format == format)
            {
                handles[nhandles++] = obj->id;

                HSU_MSG_HIGH("handle_obj_handles: %u objects found. handle=%X",
                    nhandles, handles[nhandles-1],0);
            }

            obj = obj->next;
        }
        
        store = store->next;
    }

    if (0 != ptp_get_obj_handles_complete(sessionh, PTP_RESP_OK, handles, nhandles))
    {
       ptp_rsp = PTP_RESP_GENERAL_ERROR;
    }

    if (handles)
        jfree(handles);

    return ptp_rsp;
}

static ptp_response_t handle_get_obj_info(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_id)
{
    sicd_object_t *obj;

    HSU_MSG_HIGH("handle_get_obj_info: entered. objh=%X", obj_id,0,0);

    obj = sicd_store_find_object(PTP_STORAGE_ID_ANY, obj_id);
    if (!obj)
    {
        HSU_MSG_ERROR("handle_get_obj_info: obj not found objh=%X", obj_id,0,0);
        return PTP_RESP_INVALID_OBJECT_HANDLE;
    }

    HSU_MSG_HIGH("handle_get_obj_info: obj_size=%u", 
        obj->info.compressed_size, 0, 0);

    if (0 != ptp_get_obj_info_complete(sessionh, PTP_RESP_OK, &obj->info))
    {
        return PTP_RESP_GENERAL_ERROR;
    }
    
    return PTP_RESP_OK;
}

static ptp_response_t handle_get_obj(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_id, void *data, juint32_t max_size, juint32_t offset)
{
    sicd_object_t *obj;
    juint32_t act_size;
    fs_handle_type hFile = 0; /* image file handle */
    fs_status_type fs_status = FS_OKAY_S;
    uint32 bytes_read = 0;
    
    HSU_MSG_HIGH("handle_get_obj: entered. objh=%X, offset=%d, max_size=%d",
                 obj_id, offset, max_size);
   
    obj = sicd_store_find_object(PTP_STORAGE_ID_ANY, obj_id);
    if (!obj)
        return PTP_RESP_INVALID_OBJECT_HANDLE;
    
    act_size = MIN(max_size, obj->info.compressed_size-offset);

    HSU_MSG_HIGH("handle_get_obj: obj_size=%u, act_size=%u, obj_id=%u, ", 
                 obj->info.compressed_size, act_size, obj->id);

    /* open the image file */
    hFile = sicd_fs_file_open(obj->file_path);
    if(hFile)
    {
        /* seek to the offset */
        if (offset)
        {
            fs_status = sicd_fs_file_seek(hFile, offset);
        }

        /* read act_size number of bytes to the data buffer */
        if ((FS_OKAY_S == fs_status) && (act_size > 0))
        {
            bytes_read = sicd_fs_file_read(hFile, data, act_size);
        }        

        /* close the file */
        if (FS_OKAY_S != sicd_fs_file_close(hFile))
        {
            HSU_MSG_ERROR("handle_get_obj: sicd_fs_file_close() FAILED", 0, 0, 0);
        }
        hFile = 0;        
    }
      
    
    if ((bytes_read > 0) && 0 == ptp_get_obj_complete(sessionh, PTP_RESP_OK, 
                                    act_size, obj->info.compressed_size))
    {
        return PTP_RESP_OK;
    }
    else
    {
        (void) ptp_get_obj_complete(sessionh, PTP_RESP_GENERAL_ERROR, 0,
                                     obj->info.compressed_size);        
    }

    return PTP_RESP_GENERAL_ERROR;    
}

static ptp_response_t handle_get_thumb(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_id, void *data, juint32_t max_size, juint32_t offset)
{
    sicd_object_t *obj;
    juint32_t act_size;
    fs_handle_type hFile = 0; /* image file handle */
    fs_status_type fs_status = FS_OKAY_S;
    uint32 bytes_read = 0;
      
    HSU_MSG_HIGH("handle_get_thumb: objh=%X, offset=%d, max_size=%d", 
                 obj_id, offset, max_size);

    obj = sicd_store_find_object(PTP_STORAGE_ID_ANY, obj_id);
    if (!obj)
        return PTP_RESP_INVALID_OBJECT_HANDLE;
    
    act_size = MIN(max_size, obj->info.thumb_compressed_size-offset);
    
    HSU_MSG_HIGH("handle_get_thumb: thumb_size=%u, act_size=%u, obj_id=%u", 
                 obj->info.thumb_compressed_size, act_size, obj->id);

    /* We don't use Jungo's default implemenation. */
    /* j_memcpy(data, obj->sample_data+offset, act_size);  */

    /* TODO : More efficient implementation is to check the thumb image 
       embedded in the image file.  If thumb data exist, should the thumb data.
       Only if the thumb data is not available, use the whole image.  */
       
    /* open the image file */
    hFile = sicd_fs_file_open(obj->file_path);
    if(hFile)
    {
        /* seek to the offset */
        if (offset)
        {
            fs_status = sicd_fs_file_seek(hFile, offset);
        }

        /* read act_size number of bytes to the data buffer */
        if ((FS_OKAY_S == fs_status) && (act_size > 0))
        {
            bytes_read = sicd_fs_file_read(hFile, data, act_size);
        }        

        /* close the file */
        if (FS_OKAY_S != sicd_fs_file_close(hFile))
        {
            HSU_MSG_ERROR("handle_get_obj: sicd_fs_file_close() FAILED", 0, 0, 0);
        }
        hFile = 0;        
    }
      
    
    if ((bytes_read > 0) && 0 == ptp_get_obj_complete(sessionh, PTP_RESP_OK, 
                                    act_size, obj->info.compressed_size))
    {
        return PTP_RESP_OK;
    }
    else
    {
        (void) ptp_get_obj_complete(sessionh, PTP_RESP_GENERAL_ERROR, 0,
                                     obj->info.compressed_size);        
    }

    return PTP_RESP_GENERAL_ERROR;    
}

static void handle_trans_complete(ptp_sessionh_t sessionh,
    ptp_io_status_t result, ptp_operation_t operation,
    ptp_response_t response, juint32_t obj_size, juint32_t bytes_transferred)
{
    HSU_USE_PARAM(sessionh);
    HSU_USE_PARAM(result);
    HSU_USE_PARAM(operation);
    HSU_USE_PARAM(response);
    HSU_USE_PARAM(obj_size);
    HSU_USE_PARAM(bytes_transferred);

    HSU_MSG_HIGH("handle_trans_complete: result=%u, operation=%d, response=%d",
        result, operation, response);
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

    /* TODO: Free object references */

    jfree(obj);
}

jresult_t sicd_store_add(ptp_storage_id_t id, ptp_storage_info_t *info)
{
    sicd_store_t *store = NULL;

    HSU_PARAM_COULD_BE_CONST(info);

    store = (sicd_store_t*)jmalloc(sizeof(sicd_store_t), M_ZERO);
    if (!store)
        return JENOMEM;

    (void) j_memcpy(&store->info, info, sizeof(ptp_storage_info_t));
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
    sicd_store_t *store;

    /* Find store */
    store = sicd_find_store(store_id);
    if (!store)
    {
        HSU_MSG_ERROR("sicd_store_add_object: storage not found %X", store_id,0,0);
        return JENOENT;
    }

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


/* This function removes all sicd objects in a given store */
jresult_t sicd_store_remove_all_objects(ptp_storage_id_t store_id)
{
    sicd_object_t **obj = NULL;
    sicd_object_t *r_obj = NULL;
    sicd_store_t *store = sicd_stores;

    /* Find the store */
    while (store)
    {
        if (store->id != store_id)
        {
            store = store->next;
            continue;
        }

        break; /* found the store with store_id */
    }

    if (NULL == store)
    {
        return JENOENT;  /* could not find the store with store_id */
    }

    /* Remove all objects in current store */
    obj = &(store->objects);
    while ( *obj )
    {
        r_obj = *obj;
        *obj = (*obj)->next; /* remove the obj from the link list */
        sicd_free_object(store, r_obj);
    }

    /* We finished removing all objs in the current sotre */
    store->objects = NULL; /* obj list should be empty */
    
    return 0;
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
            {
                return obj;
            }
            
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
    
    (void) j_memset(&storage_info, 0, sizeof(storage_info));

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

    (void) sicd_store_add(PTP_MAKE_STORAGE_ID(1,1), &storage_info);
}

static void delete_storage(void)
{
    while (sicd_stores)
    {
        (void) sicd_store_remove(sicd_stores->id);
    }
}

static void handle_unregister(ptp_instanceh_t insth, ptp_appctx_t global_ctx)
{
    HSU_USE_PARAM(insth);
    HSU_USE_PARAM(global_ctx);
    
    delete_storage();
}



/*==============================================================================

                         file op functions

==============================================================================*/

uint32 sicd_fs_file_size(const char * filepath)
{
    fs_rsp_msg_type rsp;
    uint32 file_size = 0; /* file size in bytes */

    if (NULL == filepath)
      return 0;

    fs_file_size(filepath, NULL, &rsp);

    if( rsp.file_size.status == FS_OKAY_S) 
    {
        file_size = rsp.file_size.size;
    }

    return file_size;
}


fs_handle_type sicd_fs_file_open(const char * filepath)
{
    fs_rsp_msg_type fs_rsp;

    fs_open(filepath, FS_OA_READONLY, NULL, NULL, &fs_rsp);

    if (fs_rsp.open.status == FS_OKAY_S)
    {
        return  fs_rsp.open.handle;
    }

    return 0; /* fs_open faileed */
}

fs_status_type sicd_fs_file_close(fs_handle_type hFile)
{
    fs_rsp_msg_type resp;

    fs_close(hFile, NULL, &resp);
    
    return resp.close.status;
}

uint32 sicd_fs_file_read(fs_handle_type hFile, void * buffer, uint32 nBytes)
{
    fs_rsp_msg_type resp;

    fs_read(hFile, buffer, nBytes, NULL, &resp);

    if (FS_OKAY_S == resp.read.status)
    {
        return resp.read.count;
    }

    return 0; /* could not read any */
}

fs_status_type sicd_fs_file_seek(fs_handle_type hFile, uint32 nOffset)
{
    fs_rsp_msg_type resp;

    fs_seek(hFile, FS_SEEK_SET, nOffset, NULL, &resp);
    return resp.seek.status;
}


/*==============================================================================

                        sicd client init function

==============================================================================*/
jresult_t sicd_app_init(ptp_instanceh_t insth, sicd_init_info_t *info)
{
    ptp_responder_cbs ptp_cbs;
    jresult_t rc = 0;

    HSU_USE_PARAM(info);

    /* Init our global context */
    (void) j_memset(&gctx, 0, sizeof(app_data_t));
    gctx.new_send_obj = NULL;
            
    /* Init our sample storage */
    init_storage();

    /* Register as a PTP application */
    (void) j_memset(&ptp_cbs, 0, sizeof(ptp_cbs));
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
    ptp_cbs.transaction_complete_h = handle_trans_complete;
    ptp_cbs.unregister_h = handle_unregister;

    rc = ptp_register_responder(insth, &ptp_cbs, &gctx);
    if (rc)
    {
        HSU_MSG_ERROR("sicd_app_init: failed to register PTP %d", rc,0,0);
    }
    else
    {
        /* Initialize the DPS layer */
        rc = dps_app_init(insth);
    }

    return rc;
}


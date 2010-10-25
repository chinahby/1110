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

#include <uw_device_sicd.h>
#include "sicd_sample.h"
#include "obj_props.h"


/* basic operations */
static ptp_response_t handle_get_obj_props_supp(ptp_sessionh_t sessionh,
    ptp_obj_format_t format)
{
    jint_t i, j;
    mtp_obj_prop_t *prop_list = NULL;
    ptp_response_t resp_rc = PTP_RESP_OK;
    
    DECLARE_FNAME("handle_get_obj_props_supp");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    for (i = 0 ; obj_format_prop_desc_map[i].obj_format != format &&
        obj_format_prop_desc_map[i].obj_format != PTP_OBJ_FORMAT_LAST ; i++)
        ;

    if (obj_format_prop_desc_map[i].obj_format == PTP_OBJ_FORMAT_LAST)
        return PTP_RESP_INVALID_OBJECT_FORMAT_CODE;

    prop_list = jmalloc(sizeof(mtp_obj_prop_t) * 
        obj_format_prop_desc_map[i].obj_prop_desc_count, 0);

    if (!prop_list)
    {
        resp_rc = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }

    for (j = 0 ; j < obj_format_prop_desc_map[i].obj_prop_desc_count ; j++)
    {
        prop_list[j] = 
            obj_format_prop_desc_map[i].obj_prop_desc[j].prop_desc->prop_code;
    }

    if (mtp_get_obj_props_supp_complete(sessionh, PTP_RESP_OK, 
        obj_format_prop_desc_map[i].obj_prop_desc_count, prop_list))
    {
        /* do some checkup  ..... */
    }

Error:
    if (prop_list)
    {
        jfree(prop_list);
        prop_list = NULL;
    }
    return resp_rc;
}

/* section B.2.2 in MTP document */
static ptp_response_t handle_get_obj_prop_desc(ptp_sessionh_t sessionh,
    mtp_obj_prop_t obj_prop_code, ptp_obj_format_t format)
{
    jint_t i = 0;
    jint_t j = 0;
    DECLARE_FNAME("handle_get_obj_prop_supp");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    for (i = 0 ; obj_format_prop_desc_map[i].obj_format != format &&
        obj_format_prop_desc_map[i].obj_format != PTP_OBJ_FORMAT_LAST ; i++)
        ;

    if (obj_format_prop_desc_map[i].obj_format == PTP_OBJ_FORMAT_LAST)
        return PTP_RESP_INVALID_OBJECT_FORMAT_CODE;

    for (j = 0 ; j < obj_format_prop_desc_map[i].obj_prop_desc_count ; j++ )
    {
        if (obj_format_prop_desc_map[i].obj_prop_desc[j].prop_desc->prop_code ==
            obj_prop_code)
        {
            break;
        }
    }

    if (j >= obj_format_prop_desc_map[i].obj_prop_desc_count )
    {
        return MTP_RESP_INVALID_OBJECT_PROP_FORMAT;
    }

    if (mtp_get_obj_prop_desc_complete(sessionh, PTP_RESP_OK,
        &obj_format_prop_desc_map[i].obj_prop_desc[j]))
    {
        /* do some checkup  ..... */
    }

    return PTP_RESP_OK;
}

static jbool_t get_from_objinfo(mtp_obj_prop_t prop_code,
    ptp_prop_value_t *val, sicd_object_t *obj)
{
    jbool_t found = 1;

    switch (prop_code)
    {
    case MTP_OBJ_PROP_STORAGE_ID:
        val->data_type = PTP_TYPE_UINT32;
        val->data.uint32 = (juint32_t)obj->info.storage_id;
        break;
    case MTP_OBJ_PROP_OBJECT_FORMAT:
        val->data_type = PTP_TYPE_UINT16;
        val->data.uint16 = (juint16_t)obj->info.format;
        break;
    case MTP_OBJ_PROP_PROTECTION_STATUS:
        val->data_type = PTP_TYPE_UINT16;
        val->data.uint16 = (juint16_t)obj->info.prot_status;
        break;
    case MTP_OBJ_PROP_OBJECT_SIZE:
        val->data_type = PTP_TYPE_UINT64;
        val->data.uint64.low = (juint32_t)obj->info.compressed_size;
        val->data.uint64.high = 0;
        break;
    case MTP_OBJ_PROP_ASSOCIATION_TYPE:
        val->data_type = PTP_TYPE_UINT16;
        val->data.uint16 = (juint16_t)obj->info.association_type;
        break;
    case MTP_OBJ_PROP_ASSOCIATION_DESC:
        val->data_type = PTP_TYPE_UINT32;
        val->data.uint32 = (juint32_t)obj->info.association_desc;
        break;
    case MTP_OBJ_PROP_OBJECT_FILENAME:
        val->data_type = PTP_TYPE_STR;
        val->data.string = obj->info.filename;
        break;
    case MTP_OBJ_PROP_DATE_CREATED:
        val->data_type = PTP_TYPE_STR;
        val->data.string = obj->info.capture_date;
        break;
    case MTP_OBJ_PROP_DATE_MODIFIED:
        val->data_type = PTP_TYPE_STR;
        val->data.string = obj->info.modification_date;
        break;
    case MTP_OBJ_PROP_KEYWORDS:
        val->data_type = PTP_TYPE_STR;
        val->data.string = obj->info.keywords;
        break;
    case MTP_OBJ_PROP_PARENT_OBJECT:
        val->data_type = PTP_TYPE_UINT32;
        val->data.uint32 = (juint32_t)obj->info.parent_object;
        break;
    case MTP_OBJ_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER:
        val->data_type = PTP_TYPE_UINT128;
        val->data.uint128.high.high = 0;
        val->data.uint128.high.low = 0;
        val->data.uint128.low.high = 0;
        val->data.uint128.low.low = (juint32_t)obj->id;
        break;
    default:
        found = 0;
        break;
    }

    return found;
}

static ptp_response_t handle_get_obj_prop_val(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle, mtp_obj_prop_t obj_prop_code,
    juint32_t elem_offset, juint32_t max_elems)
{
    juint_t i;
    jresult_t rc;
    ptp_prop_value_t *curr;
    ptp_prop_value_t val;
    static sicd_object_t *obj = NULL;
    static juint_t obj_array_prop_idx = 0;
    DECLARE_FNAME("handle_get_obj_prop_val");
#ifdef JDEBUG
        static juint_t iter_count = 0;
#endif

    if (!obj)
    {
        DBG_V(DSLAVE_MTP_API, ("%s: first enter, looking for the object[%d]\n",
            fname, obj_handle));

        obj = sicd_store_find_object(PTP_STORAGE_ID_ALL, obj_handle);
        if (!obj)
        {
            DBG_W(DSLAVE_MTP_API, ("%s: object[%d] not found\n", fname, 
                obj_handle));
            return PTP_RESP_INVALID_OBJECT_HANDLE;
        }

        DBG_V(DSLAVE_MTP_API, ("%s: looking for the prop code[0x%x]\n", fname, 
            obj_prop_code));

        /* If MTP wants info we already keep in the PTP object info, get it
         * from there */
        if (get_from_objinfo(obj_prop_code, &val, obj))
        {
            curr = &val;
            goto AtomicDone;
        }

        /* look for the property */
        for (i = 0 ; i < obj->obj_props_count ; i++)
        {
            if (obj->obj_props[i].prop_code == obj_prop_code)
                break;
        }

        if (i >= obj->obj_props_count)
        {
            DBG_W(DSLAVE_MTP_API, ("%s: prop code[%d] not found\n", fname, 
                obj_prop_code));
            return MTP_RESP_INVALID_OBJECT_PROP_CODE;
        }

        obj_array_prop_idx = i;

        curr = obj->obj_props[obj_array_prop_idx].current_value;
        
        if (!curr)
        {
            DBG_I(DSLAVE_MTP_API, ("%s: no current value, revert to default "
                "value \n", fname));
            curr = &obj->obj_props[obj_array_prop_idx].desc->prop_desc->
                default_value;
        }

AtomicDone:
        /* handle the atomic value */
        if (!PTP_IS_TYPE_ARRAY(curr->data_type))
        {
            rc = mtp_get_obj_prop_val_complete(sessionh, PTP_RESP_OK, curr);

            if (rc)
            {
                /* some checks on rc*/
            }

            /* clear some flags */
            obj = NULL;
            obj_array_prop_idx = 0;

            return PTP_RESP_OK;
        }
        /* curr->type == array ; fall through */
    }

    /* 
     * object already found - handling array iteration 
     */

    DBG_I(DSLAVE_MTP_API, ("%s: obj_prop type value is array iteration #[%d]\n",
        fname, iter_count++));

    curr = obj->obj_props[obj_array_prop_idx].current_value;
    if (!curr)
    {
        DBG_I(DSLAVE_MTP_API, ("%s: no current value, revert to default "
            "value\n", fname));
        curr = &obj->obj_props[obj_array_prop_idx].desc->prop_desc->
            default_value;
    }

    val = *curr;
    val.elem_offset = elem_offset;
    val.num_elems = MIN(max_elems, val.total_elems - val.elem_offset);

    val.data.uint8 += elem_offset * PTP_GET_TYPE_SIZE(val.data_type);

    DBG_V(DSLAVE_MTP_API, ("%s: total_elems[%d], num_elems[%d], elem_offset[%d]"
        "\n", fname, val.total_elems, val.num_elems, val.elem_offset));

    if (val.elem_offset + val.num_elems == val.total_elems)
    {
        DBG_I(DSLAVE_MTP_API, ("%s: end of element array reached\n", fname));
        obj = NULL;
        obj_array_prop_idx = 0;
#ifdef JDEBUG
        iter_count = 0;
#endif
    }

    /* send the value */
    rc = mtp_get_obj_prop_val_complete(sessionh, PTP_RESP_OK, &val);

    if (rc)
    {
        /* some checks on rc*/
        /* simulate some kind of error */
        return MTP_RESP_INVALID_OBJECT_PROP_CODE;
    }

    return PTP_RESP_OK;
}


/* section B.2.4 in MTP document */
static ptp_response_t handle_set_obj_prop_val(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle, mtp_obj_prop_t obj_prop_code,
    ptp_prop_value_t *value)
{
    juint_t i ;
    ptp_response_t resp_rc;
    ptp_prop_value_t *curr;
    static sicd_object_t *obj = NULL;
    static juint_t obj_array_prop_idx = 0;
    DECLARE_FNAME("handle_set_obj_prop_val");

    DBG_X(DSLAVE_MTP_API, ("%s: entered\n", fname));

    /* if the object exist we in partial transfer */
    if (!obj)
    {
        DBG_V(DSLAVE_MTP_API, ("%s: first enter, looking for the object[%d]\n",
            fname, obj_handle));
        obj = sicd_store_find_object(PTP_STORAGE_ID_ALL, obj_handle);
        if (!obj)
        {
            DBG_W(DSLAVE_MTP_API, ("%s: object[%d] not found\n", fname, 
                obj_handle));
            resp_rc = PTP_RESP_INVALID_OBJECT_HANDLE;
            goto Error;
        }

        DBG_V(DSLAVE_MTP_API, ("%s: looking for the prop code[0x%x]\n", fname,
            obj_prop_code));

        /* look for the property */
        for (i = 0 ; i < obj->obj_props_count ; i++)
        {
            if (obj->obj_props[i].prop_code == obj_prop_code)
                break;
        }

        if (i >= obj->obj_props_count)
        {
            DBG_W(DSLAVE_MTP_API, ("%s: prop code[%d] not found\n", fname, 
                obj_prop_code));
            return MTP_RESP_INVALID_OBJECT_PROP_CODE;
        }

        obj_array_prop_idx = i;

        /* free exisiting data */
        if (obj->obj_props[obj_array_prop_idx].current_value)
        {
            ptp_prop_value_clear(
                obj->obj_props[obj_array_prop_idx].current_value);
        }
        else
        {
            obj->obj_props[obj_array_prop_idx].current_value = 
                jmalloc(sizeof(ptp_prop_value_t), M_ZERO);

            if (!obj->obj_props[obj_array_prop_idx].current_value)
            {
                resp_rc = PTP_RESP_GENERAL_ERROR;
                goto Error;
            }
        }
        curr = obj->obj_props[obj_array_prop_idx].current_value;

        /* handle the atomic value */
        if (!PTP_IS_TYPE_ARRAY(value->data_type))
        {
            DBG_I(DSLAVE_MTP_API, ("%s: obj_prop type value is atomic\n",
                fname));

            *curr = *value;
            if (curr->data_type == PTP_TYPE_STR)
            {
                curr->data.string = ptp_string_duplicate(
                    value->data.string);
                if (!curr->data.string)
                {
                    resp_rc = PTP_RESP_GENERAL_ERROR;
                    goto Error;
                }
            }
            /* clear some flags */
            obj = NULL;
            obj_array_prop_idx = 0;
        }
        else /* handle the first partial transfer of array value */
        {
            juint_t type_size = PTP_GET_TYPE_SIZE(value->data_type);
            juint_t total_size = type_size * value->total_elems;
            juint_t current_size = type_size * value->num_elems;

            DBG_I(DSLAVE_MTP_API, ("%s: first partial, type[0x%x] "
                "total_elems[%d], num_elems[%d], elem_offset[%d]\n", fname, 
                value->data_type, value->total_elems,
                value->num_elems, value->elem_offset));

            if (!value->total_elems || !value->num_elems || !type_size)
            {
                resp_rc = PTP_RESP_GENERAL_ERROR;
                goto Error;
            }

            /* free the current value before overwriting */
            if (curr->data.a_uint8)
                jfree(curr->data.a_uint8);
            *curr = *value; /* copy all the fields */

            curr->data.a_uint8 = jmalloc(total_size, 1);
            if (!curr->data.a_uint8)
            {
                resp_rc = PTP_RESP_GENERAL_ERROR;
                goto Error;
            }

            /*copy the partial buffer */
            j_memcpy(curr->data.a_uint8, value->data.a_uint8, current_size);

            curr->elem_offset = curr->num_elems;
        }
    }
    else /* in partial transaction iteration */
    {
        juint_t type_size = PTP_GET_TYPE_SIZE(value->data_type);
        juint_t current_size = type_size * value->num_elems;
        juint_t offset_size = 0;
        juint8_t *ptr = NULL;
#ifdef JDEBUG
        static juint_t iter_count = 0;
#endif
        DBG_I(DSLAVE_MTP_API, ("%s: obj_prop type value is array iteration [%d]"
            "\n", fname, iter_count));

        curr = obj->obj_props[obj_array_prop_idx].current_value;

        DBG_I(DSLAVE_MTP_API, ("%s: value:(total_elems[%d], num_elems[%d], "
            "elem_offset[%d]), curr[%p](data_type[%d], total_elems[%d]\n",fname,
            value->total_elems, value->num_elems, value->elem_offset, 
            curr, curr->data_type, curr->total_elems));

        if (!PTP_IS_TYPE_ARRAY(value->data_type) || !value->total_elems || 
            !value->num_elems || !type_size)
        {
            resp_rc = PTP_RESP_GENERAL_ERROR;
            goto Error;
        }

        if ((value->total_elems != curr->total_elems) || 
            (value->num_elems + value->elem_offset > value->total_elems))
        {
            resp_rc = PTP_RESP_GENERAL_ERROR;
            goto Error;
        }

        offset_size = type_size * curr->elem_offset;
        ptr = curr->data.a_uint8 + offset_size ;

        j_memcpy(ptr, value->data.a_uint8, current_size);


        curr->elem_offset += curr->num_elems;

        /* check if finished */
        if (curr->elem_offset == curr->total_elems)
        {
            /* clear some flags */
            obj = NULL;
            obj_array_prop_idx = 0;
#ifdef JDEBUG
            iter_count = 0;
        }
        else
        {
            iter_count ++;
#endif
        }
    }
    mtp_set_obj_prop_val_complete(sessionh, PTP_RESP_OK);

    return PTP_RESP_OK;
Error:
    /* clear some flags */
    obj = NULL;
    obj_array_prop_idx = 0;
    return resp_rc;
}

/* Limitation on maximum references per object */
static ptp_response_t handle_get_obj_refs(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle)
{
    jresult_t rc = 0;
    sicd_object_t *obj = NULL;
    DECLARE_FNAME("handle_get_obj_refs");

    DBG_X(DSLAVE_MTP_API, ("%s: entered\n", fname));

    obj = sicd_store_find_object(PTP_STORAGE_ID_ALL, obj_handle);
    if (!obj)
    {
        DBG_W(DSLAVE_MTP_API, ("%s: object[%d] not found\n", fname, 
            obj_handle));
        return PTP_RESP_INVALID_OBJECT_HANDLE;
    }

    rc = mtp_get_obj_refs_complete(sessionh, PTP_RESP_OK, obj->ref_handles,
        obj->ref_handles_count);

    if (rc)
    {
        /* some checks on rc*/
        /* simulate some kind of error */
        return PTP_RESP_GENERAL_ERROR;
    }

    return PTP_RESP_OK;
}

/* section B.2.6 in MTP document */
static ptp_response_t handle_set_obj_refs(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle, ptp_obj_handle_t *obj_handles, juint32_t size)
{
    jresult_t rc = 0;
    sicd_object_t *obj = NULL;
    juint_t handles_size = size * sizeof(ptp_obj_handle_t);
    DECLARE_FNAME("handle_set_obj_refs");

    DBG_X(DSLAVE_MTP_API, ("%s: entered\n", fname));

    obj = sicd_store_find_object(PTP_STORAGE_ID_ALL, obj_handle);
    if (!obj)
    {
        DBG_W(DSLAVE_MTP_API, ("%s: object[%d] not found\n", fname, 
            obj_handle));
        return PTP_RESP_INVALID_OBJECT_HANDLE;
    }

    if (obj->ref_handles)
    {
        DBG_I(DSLAVE_MTP_API, ("%s: object references handles already exist"
            "overwriting them\n", fname));
        jfree(obj->ref_handles);
        obj->ref_handles_count = 0;
    }
    else
        DBG_I(DSLAVE_MTP_API, ("%s: object references handles not exist\n",
            fname));

    obj->ref_handles = jmalloc(handles_size, 1);
    if (!obj->ref_handles)
        goto Error;

    DBG_I(DSLAVE_MTP_API, ("%s: copying [%d] object handles \n", fname, size));
    j_memcpy(obj->ref_handles, obj_handles, handles_size);
    obj->ref_handles_count = size;


    rc = mtp_set_obj_refs_complete(sessionh, PTP_RESP_OK);
    if (rc)
    {
        DBG_E(DSLAVE_MTP_API, ("%s: completion rc = %d\n", fname, rc));
        /* some checks on rc*/
        /* simulate some kind of error */
        goto Error;
    }

    return PTP_RESP_OK;
Error:
     return PTP_RESP_GENERAL_ERROR;
}

static ptp_response_t handle_skip(ptp_sessionh_t sessionh, juint32_t skip_index)
{
    jresult_t rc = 0;

    if (skip_index) /* simple check for parameter */
    {
        rc = mtp_skip_complete(sessionh, PTP_RESP_OK);
        if (!rc)
            return PTP_RESP_OK;
    }
        
    return PTP_RESP_GENERAL_ERROR;
}


#ifdef CONFIG_PROTO_PTP_MTP_ENHANCED
/* enhanced operation */
/* section 14.7.2.1 in mtp document */

/* 2 last paramters of function below :
 * elem_idx : idx of the element array (external array)
 * data_idx : idx of the data array, when needed (internal array) */
static ptp_response_t handle_get_obj_prop_list(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle, ptp_obj_format_t format,
    mtp_obj_prop_t obj_prop_code, mtp_obj_prop_group_t obj_prop_group_code,
    juint32_t depth, juint32_t elem_idx, juint32_t data_idx)
{
    return PTP_RESP_GENERAL_ERROR;
}

/* section 14.7.2.3 in mtp document */
static ptp_response_t handle_set_obj_prop_list(ptp_sessionh_t sessionh,
    juint32_t total_num_elems, mtp_obj_prop_elem_t *elem, juint32_t elem_count,
    juint32_t elem_idx)
{
    return PTP_RESP_GENERAL_ERROR;
}

/* section 14.7.2.3 in mtp document */

/* last paramter of function below
 * corresponde to last parameter of next function */
ptp_response_t handle_get_interdep_prop_desc(ptp_sessionh_t sessionh,
    ptp_obj_format_t format, juint16_t arr_idx)
{
    return PTP_RESP_GENERAL_ERROR;
}


/* section 14.7.2.4 in mtp document */
ptp_response_t handle_send_obj_prop_list(ptp_sessionh_t sessionh,
    ptp_storage_id_t storage, ptp_obj_handle_t parent_obj_handle,
    ptp_obj_format_t format, juint32_t obj_size_msdw,
    juint32_t obj_size_lsdw, mtp_obj_prop_elem_t *elem, juint32_t elem_count,
    juint32_t elem_idx)
{
    return PTP_RESP_GENERAL_ERROR;
}

#endif

jresult_t mtp_app_init(ptp_instanceh_t insth)
{
    jresult_t rc = 0;
    mtp_callbacks_t mtp_cbs;

    /* basic operations */
    mtp_cbs.get_obj_props_supp_h = handle_get_obj_props_supp;
    mtp_cbs.get_obj_prop_desc_h  = handle_get_obj_prop_desc;
    mtp_cbs.get_obj_prop_val_h   = handle_get_obj_prop_val;
    mtp_cbs.set_obj_prop_val_h   = handle_set_obj_prop_val;
    mtp_cbs.get_obj_refs_h       = handle_get_obj_refs;
    mtp_cbs.set_obj_refs_h       = handle_set_obj_refs;
    mtp_cbs.skip_h               = handle_skip;

#ifdef CONFIG_PROTO_PTP_MTP_ENHANCED
    /* enhanced operation */
    mtp_cbs.get_obj_prop_list_h = handle_get_obj_prop_list;
    mtp_cbs.set_obj_prop_list_h = handle_set_obj_prop_list;
    mtp_cbs.get_interdep_prop_desc_h = handle_get_interdep_prop_desc;
    mtp_cbs.send_obj_prop_list_h = handle_send_obj_prop_list; 
#endif
    rc = mtp_init(insth, &mtp_cbs);
    if (rc)
        return rc;

    /* Initialize object property descriptor maps and allowed values */
    objprops_init_descs();
    return 0;
}


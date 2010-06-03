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


#include <jos.h>
#include "ptp.h"
#include "ptp_responder_api.h"


#define PTP_VERSION     100     /* Expressed in hundreths, i.e. 1.00 */

static jresult_t dispatch(ptp_session_t *session);
static void prepare_transfer(ptp_session_t *session);
static jresult_t schedule_transport(ptp_session_t *session);
static jresult_t schedule_app(ptp_session_t *session);

/******************************************************************
 *      Utility and debug functions
 *****************************************************************/

static void init_session(ptp_session_t *session, ptp_instance_t *inst)
{
    j_memset(session, 0, sizeof(ptp_session_t));

    session->data[0].buff_size = sizeof(session->data[0].buff);
    session->data[1].buff_size = sizeof(session->data[1].buff);

    session->inst = inst;
}

static ptp_session_t* find_session(ptp_instance_t *inst, juint32_t session_id)
{
    /* Currently, we only supprt a single session */
    if (session_id && inst->sessions &&
        inst->sessions->session_id == session_id)
    {
        return inst->sessions;
    }

    return NULL;
}

jbool_t verify_command_supported(ptp_session_t *session,
    void *pfn, jbool_t in_data_phase)
{
    DECLARE_FNAME("verify_command_supported");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    if (!pfn)
    {
        DBG_E(DPROTO_PTP, ("%s: Operation not supported\n", fname));
        session->response.code = PTP_RESP_OPERATION_NOT_SUPPORTED;
        send_response(session, 0, in_data_phase);
        return 0;
    }

    return 1;
}

#ifdef JDEBUG

#define DBG_PTPBUF(session, idx) dbg_ptpbuf(session, idx)
#define DBG_OPERATION(operation) dbg_operation(operation)

static void dbg_ptpbuf(ptp_session_t *session, jint_t idx)
{
    if (DBG_GET_LEVEL(DPROTO_PTP) == DL_EX_VERBOSE)
    {
        DBG_FUNC("\tptpbuf: idx: %d (active: %d)\n", idx, session->app_buf_idx);
        DBG_FUNC("\tptpbuf: len: %u\n", session->data[idx].data_len);
        DBG_FUNC("\tptpbuf: offset: %u\n", session->data[idx].data_offset);
        DBG_FUNC("\tptpbuf: bufsize: %u\n\n", session->data[idx].buff_size);
    }
}

static void dbg_operation(void *operation)
{
    ptp_io_block_t *op = (ptp_io_block_t*)operation;

    if (DBG_GET_LEVEL(DPROTO_PTP) == DL_EX_VERBOSE)
    {
        DBG_FUNC("\tcode:         0x%X\n", op->code);
        DBG_FUNC("\tsession:      0x%X\n", op->session_id);
        DBG_FUNC("\ttransaction:  0x%X\n", op->transaction_id);
        DBG_FUNC("\tparameter 1:  0x%X\n", op->parameter[0]);
        DBG_FUNC("\tparameter 2:  0x%X\n", op->parameter[1]);
        DBG_FUNC("\tparameter 3:  0x%X\n", op->parameter[2]);
    }
}

#else

#define DBG_PTPBUF(session, idx) do{} while(0)
#define DBG_OPERATION(operation) do{} while(0)

#endif

/******************************************************************
 *      PTP Transaction handlers
 *****************************************************************/

static jresult_t operation_complete(ptp_sessionh_t sessionh,
    ptp_response_t response)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    DECLARE_FNAME("ptp_operation_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    session->response.code = response;
    return send_response(session, 0, 0);
}

static void handle_open_session(ptp_session_t *lsession)
{
    ptp_instance_t *inst = lsession->inst; 
    ptp_session_t *session = NULL;
    DECLARE_FNAME("handle_open_session");

    DBG_V(DPROTO_PTP, ("%s: entered (%u)\n", fname,
        lsession->operation.parameter[0]));

    if (lsession->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            lsession->state));
        lsession->response.code = PTP_RESP_GENERAL_ERROR;
        goto Exit;
    }
    
    if (lsession->operation.session_id || !lsession->operation.parameter[0])
    {
        DBG_I(DPROTO_PTP, ("%s: Invalid session id (%u) or parameter (%u).\n",
            fname, lsession->operation.session_id,
            lsession->operation.parameter[0]));
        lsession->response.code = PTP_RESP_INVALID_PARAM;
        goto Exit;
    }

    if (inst->max_sessions > 1 && inst->active_sessions >= inst->max_sessions)
    {
        DBG_I(DPROTO_PTP, ("%s: Cannot open more than %d sessions.\n",
            fname, inst->active_sessions));
        lsession->response.code = PTP_RESP_DEVICE_BUSY;
        goto Exit;
    }

    /* check if this session exists already */
    for(session = inst->sessions;
        session && session->session_id != lsession->operation.parameter[0];
        session = session->next);

    if (session)
    {
        DBG_I(DPROTO_PTP, ("%s: Session already open %s\n", fname,
            (inst->max_sessions==1) ? "(single session mode)" : ""));

        lsession->response.code = PTP_RESP_SESSION_ALREADY_OPEN;
        if (inst->max_sessions == 1)
            lsession->response.parameter[0] = session->session_id;
        
        session = NULL;
        goto Exit;
    }

    if (!verify_command_supported(lsession,
        (void *)lsession->inst->cbs.open_session_h, 0))
    {
        return;
    }

    session = jmalloc(sizeof(ptp_session_t), 0);
    if (!session)
    {
        DBG_E(DPROTO_PTP, ("%s: Failed to allocate memory for new session\n",
            fname));
        lsession->response.code = PTP_RESP_GENERAL_ERROR;
        goto Exit;
    }

    init_session(session, inst);

    /* Vendor Specific session init */
    if (inst->vs_ext_params.open_session_h &&
        inst->vs_ext_params.open_session_h(session) < 0)
    {
        DBG_E(DPROTO_PTP, ("%s: Failed to initialize vendor specific "
            "extension\n", fname));
        lsession->response.code = PTP_RESP_GENERAL_ERROR;
        jfree(session);
        goto Exit;
    }

    session->session_id = lsession->operation.parameter[0];

    lsession->response.code = inst->cbs.open_session_h(session,
        session->session_id, inst->global_appctx, &session->appctx);

    if (lsession->response.code != PTP_RESP_OK)
    {
        jfree(session);
        goto Exit;
    }

    session->next = inst->sessions;
    inst->sessions = session;
    inst->active_sessions++;

Exit:
    send_response(lsession, 0, 0);
}

static void handle_close_session(ptp_session_t *session, jbool_t force)
{
    ptp_instance_t *inst = session->inst;
    DECLARE_FNAME("handle_close_session");

    DBG_X(DPROTO_PTP, ("%s: entered (%p, %d)\n", fname, session,
        session->session_id));

    if (!force && (session->state != PTP_STATE_COMMAND))
    {
        DBG_E(DPROTO_PTP, ("%s: Trying to close session while transaction "
            "is in progress (state=%d)\n", fname, session->state));
        session->response.code = PTP_RESP_DEVICE_BUSY;
        send_response(session, 0, 0);
        return;
    }

    if (!verify_command_supported(session,
        (void *)session->inst->cbs.close_session_h, 0))
    {
        return;
    }

    /* The application cannot undo a close_session operation */
    inst->cbs.close_session_h(session, session->session_id);

    /* Vendor Specific session cleanup */
    if (inst->vs_ext_params.close_session_h)
        inst->vs_ext_params.close_session_h(session);

    /* Save our state so we can respond to the Initiator */
    session->state = PTP_STATE_RESPONSE;
    session->response.code = PTP_RESP_OK;
    j_memcpy(&inst->null_session, session, sizeof(ptp_session_t));
    send_response(&inst->null_session, 0, 0);

    /* Currently, we support single session only */
    inst->active_sessions--;
    inst->sessions = NULL;
    DBG_V(DPROTO_PTP, ("%s: freed session: %p (%ld)\n", fname, session,
        session ? session->session_id : 0xFFFFFFFF));
    jfree(session);
}

static void handle_get_dev_info(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_dev_info");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (!verify_command_supported(session,
            (void *)session->inst->cbs.get_dev_info_h, 1))
        {
            break;
        }
        
        session->response.code = session->inst->cbs.get_dev_info_h(session);
        if (session->response.code != PTP_RESP_OK)
            send_response(session, 0, 1);
        
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 0, 0);
        break;
    }

    return;
}

static jresult_t get_dev_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_dev_info_t *devinfo)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_instance_t *inst = session->inst;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    void *buf;
    void *count_field;
    jint_t i;
    DECLARE_FNAME("ptp_get_dev_info_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    
    session->response.code = response;
    if (response != PTP_RESP_OK)
        goto Exit;

    buf = ptpbuf->buff + session->inst->transport_hdr_size;

    pack_u16(&buf, PTP_VERSION);
    pack_u32(&buf, inst->vs_ext_params.vendor_ext_id);
    pack_u16(&buf, inst->vs_ext_params.vendor_ext_ver);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN),
        inst->vs_ext_params.vendor_ext_desc);
    
    pack_u16(&buf, devinfo->func_mode);

    i = 0;
    count_field = buf;
    buf = ((juint32_t*)buf) + 1;
    if (devinfo->supported_ops)
    {
        while (devinfo->supported_ops[i] != PTP_OP_LAST)
        {
            pack_u16(&buf, (juint16_t)devinfo->supported_ops[i]);
            i++;
        }
    }
    pack_u32(&count_field, i);

    i = 0;
    count_field = buf;
    buf = ((juint32_t*)buf) + 1;
    if (devinfo->supported_events)
    {
        while (devinfo->supported_events[i] != PTP_EVENT_LAST)
        {
            pack_u16(&buf, devinfo->supported_events[i]);
            i++;
        }
    }
    pack_u32(&count_field, i);
        
    i = 0;
    count_field = buf;
    buf = ((juint32_t*)buf) + 1;
    if (devinfo->supported_props)
    {
        while (devinfo->supported_props[i] != PTP_DEV_PROP_LAST)
        {
            pack_u16(&buf, devinfo->supported_props[i]);
            i++;
        }
    }
    pack_u32(&count_field, i);

    i = 0;
    count_field = buf;
    buf = ((juint32_t*)buf) + 1;
    if (devinfo->capture_formats)
    {
        while (devinfo->capture_formats[i] != PTP_OBJ_FORMAT_LAST)
        {
            pack_u16(&buf, devinfo->capture_formats[i]);
            i++;
        }
    }
    pack_u32(&count_field, i);
    
    i = 0;
    count_field = buf;
    buf = ((juint32_t*)buf) + 1;
    if (devinfo->image_formats)
    {
        while (devinfo->image_formats[i] != PTP_OBJ_FORMAT_LAST)
        {
            pack_u16(&buf, devinfo->image_formats[i]);
            i++;
        }
    }
    pack_u32(&count_field, i);
    
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN),
        devinfo->manufacturer);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN), devinfo->model);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN), devinfo->dev_ver);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN),
        devinfo->serial_num);

    ptpbuf->data_len = ((juint8_t*)buf - ptpbuf->buff) -
        session->inst->transport_hdr_size;
    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

Exit:
    return send_response(session, 0, 1);
}

static void handle_get_object_handles(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_object_handles");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (!verify_command_supported(session,
            (void *)session->inst->cbs.get_obj_handles_h, 1))
        {
            break;
        }
        
        session->response.code = session->inst->cbs.get_obj_handles_h(
            session, (ptp_storage_id_t)session->operation.parameter[0],
            (ptp_obj_format_t)session->operation.parameter[1], 
            (ptp_obj_handle_t)session->operation.parameter[2]);
        
        if (session->response.code != PTP_RESP_OK)
            send_response(session, 0, 1);

        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 0, 0);
        break;
    }

    return;
}

static jresult_t get_obj_handles_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t *handles,
    juint32_t count)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    void *buf;
    jint_t i = 0;
    DECLARE_FNAME("ptp_get_obj_handles_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    
    session->response.code = response;
    if (response != PTP_RESP_OK)
        goto Exit;

    buf = ptpbuf->buff + session->inst->transport_hdr_size;

    /* Do we have enough room in our buffer? */
    if ( (count*sizeof(ptp_obj_handle_t)+sizeof(juint32_t)) >
         (ptpbuf->buff_size - session->inst->transport_hdr_size) )
    {
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Exit;
    }

    pack_u32(&buf, count);
    for(i=0; i<count; i++)
        pack_u32(&buf, handles[i]);

    ptpbuf->data_len = ((juint8_t*)buf - ptpbuf->buff) -
        session->inst->transport_hdr_size;
    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

Exit:
    return send_response(session, 0, 1);
}

static void handle_get_storage_ids(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_storage_ids");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (!verify_command_supported(session,
            (void *)session->inst->cbs.get_storage_ids_h, 1))
        {
            break;
        }
        
        session->response.code = session->inst->cbs.get_storage_ids_h(
            session);

        if (session->response.code != PTP_RESP_OK)
            send_response(session, 0, 1);

        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 0, 0);
        break;
    }

    return;
}

static jresult_t get_storage_ids_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t *stores,
    juint32_t count)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    void *buf;
    jint_t i = 0;
    DECLARE_FNAME("ptp_get_storage_ids_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    
    session->response.code = response;
    if (response != PTP_RESP_OK)
        goto Exit;

    buf = ptpbuf->buff + session->inst->transport_hdr_size;

    /* Do we have enough room in our buffer? */
    if ( (count*sizeof(ptp_storage_id_t)+sizeof(juint32_t)) >
         (ptpbuf->buff_size - session->inst->transport_hdr_size) )
    {
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Exit;
    }

    pack_u32(&buf, count);
    for(i=0; i<count; i++)
        pack_u32(&buf, stores[i]);

    ptpbuf->data_len = ((juint8_t*)buf - ptpbuf->buff) -
        session->inst->transport_hdr_size;
    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

Exit:
    return send_response(session, 0, 1);
}

static void handle_get_storage_info(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_storage_info");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (!verify_command_supported(session,
            (void *)session->inst->cbs.get_storage_info_h, 1))
        {
            break;
        }
        
        session->response.code = session->inst->cbs.get_storage_info_h(
            session, session->operation.parameter[0]);
        
        if (session->response.code != PTP_RESP_OK)
            send_response(session, 0, 1);
        
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 0, 0);
        break;
    }

    return;
}

static jresult_t get_storage_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_info_t *store_info)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    void *buf;
    DECLARE_FNAME("ptp_get_storage_info_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    
    session->response.code = response;
    if (response != PTP_RESP_OK)
        goto Exit;

    buf = ptpbuf->buff + session->inst->transport_hdr_size;

    pack_u16(&buf, store_info->type);
    pack_u16(&buf, store_info->fs_type);
    pack_u16(&buf, store_info->access);
    pack_u64(&buf, &store_info->max_capacity);
    pack_u64(&buf, &store_info->free_space);
    pack_u32(&buf, store_info->free_images);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN), store_info->desc);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN), store_info->label);

    ptpbuf->data_len = ((juint8_t*)buf - ptpbuf->buff) -
        session->inst->transport_hdr_size;
    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

Exit:
    return send_response(session, 0, 1);
}

static void handle_get_num_objects(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_num_objects");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (!verify_command_supported(session,
            (void *)session->inst->cbs.get_num_objs_h, 0))
        {
            break;
        }
        
        session->response.code = session->inst->cbs.get_num_objs_h(
            session, (ptp_storage_id_t)session->operation.parameter[0],
            (ptp_obj_format_t)session->operation.parameter[1],
            (ptp_obj_handle_t)session->operation.parameter[2]);
        
        if (session->response.code != PTP_RESP_OK)
            send_response(session, 1, 0);
        
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 1, 0);
        break;
    }

    return;
}

static jresult_t get_num_objects_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t num_objects)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    DECLARE_FNAME("ptp_get_num_objects_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    session->response.code = response;
    session->response.parameter[0] = num_objects;
    return send_response(session, 1, 0);
}

static void handle_get_object_info(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_object_info");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (!verify_command_supported(session,
            (void *)session->inst->cbs.get_obj_info_h, 1))
        {
            break;
        }
        
        session->response.code = session->inst->cbs.get_obj_info_h(
            session, session->operation.parameter[0]);
        
        if (session->response.code != PTP_RESP_OK)
            send_response(session, 0, 1);
        
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 0, 0);
        break;
    }

    return;
}

static jresult_t get_obj_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_info_t *objinfo)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    void *buf;
    DECLARE_FNAME("ptp_get_obj_info_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    
    session->response.code = response;
    if (response != PTP_RESP_OK)
        goto Exit;

    buf = ptpbuf->buff + session->inst->transport_hdr_size;

    pack_u32(&buf, objinfo->storage_id);
    pack_u16(&buf, objinfo->format);
    pack_u16(&buf, objinfo->prot_status);
    pack_u32(&buf, objinfo->compressed_size);
    pack_u16(&buf, objinfo->thumb_format);
    pack_u32(&buf, objinfo->thumb_compressed_size);
    pack_u32(&buf, objinfo->thumb_width);
    pack_u32(&buf, objinfo->thumb_height);
    pack_u32(&buf, objinfo->width);
    pack_u32(&buf, objinfo->height);
    pack_u32(&buf, objinfo->bit_depth);
    pack_u32(&buf, objinfo->parent_object);
    pack_u16(&buf, objinfo->association_type);
    pack_u32(&buf, objinfo->association_desc);
    pack_u32(&buf, objinfo->sequence_number);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN),
        objinfo->filename);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN),
        objinfo->capture_date);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN),
        objinfo->modification_date);
    pack_str(&buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN),
        objinfo->keywords);

    ptpbuf->data_len = ((juint8_t*)buf - ptpbuf->buff) -
        session->inst->transport_hdr_size;
    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

Exit:
    return send_response(session, 0, 1);
}

static jresult_t get_obj_send_response(ptp_session_t *session,
    jbool_t in_data_phase)
{
    if (session->operation.code == PTP_OP_GET_PARTIAL_OBJECT)
    {
        session->response.parameter[0] =
            session->curr_tr_offset - session->start_offset;
    }
    
    return send_response(session,
        (session->operation.code == PTP_OP_GET_PARTIAL_OBJECT) ? 1 : 0,
        in_data_phase);
}

static void handle_get(ptp_session_t *session, ptp_get_obj_cb get_h)
{
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    juint32_t headroom = 0;
    DECLARE_FNAME("handle_get");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    switch(session->state)
    {
    case PTP_STATE_COMMAND:

        if (!verify_command_supported(session, (void*)get_h, 1))
            break;

        headroom = session->inst->transport_hdr_size;
        session->transport_done = 1;

        if (session->operation.code == PTP_OP_GET_PARTIAL_OBJECT)
        {
            session->start_offset = session->operation.parameter[1];
            session->end_offset = session->start_offset +
                session->operation.parameter[2];
            session->curr_app_offset = session->curr_tr_offset =
                session->start_offset;
        }
        else
        {
            /* Since the actual end_offset can only be set in the completion,
             * we set it now to as much as we can get */
            session->end_offset = ptpbuf->buff_size;
        }

        /* Fall through here... */

    case PTP_STATE_SEND_DATA:

        session->response.code = get_h(
            session,
            session->operation.parameter[0],
            ptpbuf->buff + headroom,
            MIN(ptpbuf->buff_size - headroom,
                session->end_offset - session->curr_app_offset),
            session->curr_app_offset);

        if (session->response.code != PTP_RESP_OK)
            get_obj_send_response(session, 1);

        break;

    case PTP_STATE_DATA_DONE:
        get_obj_send_response(session, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        get_obj_send_response(session, 0);
        break;
    }

    return;
}

static void handle_get_object(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_object");
    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    handle_get(session, session->inst->cbs.get_obj_h);
}

static void handle_get_thumb(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_thumb");
    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    handle_get(session, session->inst->cbs.get_thumb_h);
}

static jresult_t get_obj_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t actual_size, juint32_t obj_size)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    DECLARE_FNAME("ptp_get_obj_complete");
    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        return get_obj_send_response(session, 1);

    /* We do not keep track of object sizes. Either use the
     * object_size as given by the application, or in the case of
     * get_partial_obj, where the host determines the end_offset,
     * we check it against the size given by the application */
    if (session->state == PTP_STATE_COMMAND)
    {
        if (session->operation.code != PTP_OP_GET_PARTIAL_OBJECT ||
            session->end_offset > obj_size)
        {
            /* FOR GET_PARTIAL_OBJECT operations, a host can ask for more
             * than an object's size. e.g 0xFFFFFFFF means 'all data' */
            session->end_offset = obj_size;
        }

        session->state = PTP_STATE_SEND_DATA;
    }

    session->data[session->app_buf_idx].data_len = actual_size;
    session->app_done = 1;
    return schedule_advance(session);
}

static void handle_send_object_info(ptp_session_t *session)
{
    DECLARE_FNAME("handle_send_object_info");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    
    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session,
            (void *)session->inst->cbs.send_obj_info_h, 1))
        {
            start_read_data(session);
        }

        break;

    case PTP_STATE_READ_DATA:
        {
            ptp_obj_info_t *objinfo = NULL;
            ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    
            objinfo = ptp_objectinfo_alloc();
            if (!objinfo)
            {
                session->response.code = PTP_RESP_GENERAL_ERROR;
                send_response(session, 3, 0);
                break;
            }
            unpack_objinfo(ptpbuf->buff + ptpbuf->data_offset , objinfo);

            /* Save info for SendObj operation */
            session->obj_size = objinfo->compressed_size;

            session->response.code = session->inst->cbs.send_obj_info_h(
                session, objinfo,
                (ptp_storage_id_t)session->operation.parameter[0],
                (ptp_obj_handle_t)session->operation.parameter[1]);

            ptp_objectinfo_free(objinfo);

            if (session->response.code != PTP_RESP_OK)
            {
                session->app_done = 1;
                schedule_advance(session);
            }
        }
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 3, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 3, 0);
        break;
    }
    
    return;
}
    
static jresult_t send_obj_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t actual_store,
    ptp_obj_handle_t actual_folder, ptp_obj_handle_t new_handle)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    DECLARE_FNAME("ptp_send_obj_info_complete");
    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    session->response.code = response;
    if (response == PTP_RESP_OK)
    {
        session->response.parameter[0] = actual_store;
        session->response.parameter[1] = actual_folder;
        session->response.parameter[2] = new_handle;
    }
    
    session->app_done = 1;
    return schedule_advance(session);
}

static void handle_send_object(ptp_session_t *session)
{
    DECLARE_FNAME("handle_send_object");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    
    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session,
            (void *)session->inst->cbs.send_obj_h, 1))
        {
            start_read_data(session);
        }

        break;
        
    case PTP_STATE_READ_DATA:
        {
            ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
            
            session->response.code = session->inst->cbs.send_obj_h(
                session, ptpbuf->buff + ptpbuf->data_offset,
                ptpbuf->data_len, session->curr_app_offset);
            
            if (session->response.code != PTP_RESP_OK)
                send_response(session, 0, 1);
        }
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 0, 0);
        break;
    }

    return;
}

static jresult_t send_obj_complete(ptp_sessionh_t sessionh,
    ptp_response_t response)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    DECLARE_FNAME("ptp_send_obj_complete");
    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        return send_response(session, 0, 1);
    
    session->app_done = 1;
    return schedule_advance(session);
}

static void handle_device_reset(ptp_session_t *session)
{
    DECLARE_FNAME("handle_device_reset");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }

    if (!verify_command_supported(session,
        (void *)session->inst->cbs.reset_dev_h, 0))
    {
        return;
    }

    /* We must flag this here so that the application can call the completion
     * from within the handler if it chooses to do so. */
    session->device_reset = 1;

    session->response.code = session->inst->cbs.reset_dev_h(session);

    if (session->response.code == PTP_RESP_OK)
        return;

Error:
    send_response(session, 0, 0);
}

static void handle_reset_dev_prop_val(ptp_session_t *session)
{
    DECLARE_FNAME("handle_reset_dev_prop_val");

    DBG_V(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }
    
    if (!verify_command_supported(session,
        (void *)session->inst->cbs.reset_dev_prop_val_h, 0))
    {
        return;
    }

    session->response.code = session->inst->cbs.reset_dev_prop_val_h(session,
        (ptp_dev_prop_t)session->operation.parameter[0]);

    if (session->response.code == PTP_RESP_OK)
        return;

Error:
    send_response(session, 0, 0);
}

static void handle_delete_obj(ptp_session_t *session)
{
    DECLARE_FNAME("handle_delete_obj");

    DBG_V(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }
    
    if (!verify_command_supported(session,
        (void *)session->inst->cbs.delete_obj_h, 0))
    {
        return;
    }

    session->response.code = session->inst->cbs.delete_obj_h(session,
        (ptp_obj_handle_t)session->operation.parameter[0],
        (ptp_obj_format_t)session->operation.parameter[1]);

    if (session->response.code == PTP_RESP_OK)
        return;

Error:
    send_response(session, 0, 0);
}

static void handle_move_obj(ptp_session_t *session)
{
    DECLARE_FNAME("handle_mov_obj");

    DBG_V(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }
    
    if (!verify_command_supported(session,
        (void *)session->inst->cbs.move_obj_h, 0))
    {
        return;
    }

    session->response.code = session->inst->cbs.move_obj_h(session,
        (ptp_obj_handle_t)session->operation.parameter[0],
        (ptp_storage_id_t)session->operation.parameter[1],
        (ptp_obj_handle_t)session->operation.parameter[2]);

    if (session->response.code == PTP_RESP_OK)
        return;

Error:
    send_response(session, 0, 0);
}

static void handle_copy_obj(ptp_session_t *session)
{
    DECLARE_FNAME("handle_copy_obj");

    DBG_V(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }
    
    if (!verify_command_supported(session,
        (void *)session->inst->cbs.copy_obj_h, 0))
    {
        return;
    }

    session->response.code = session->inst->cbs.copy_obj_h(session,
        (ptp_obj_handle_t)session->operation.parameter[0],
        (ptp_storage_id_t)session->operation.parameter[1],
        (ptp_obj_handle_t)session->operation.parameter[2]);

    if (session->response.code == PTP_RESP_OK)
        return;

Error:
    send_response(session, 1, 0);
}

static jresult_t copy_obj_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t new_handle)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    DECLARE_FNAME("ptp_copy_obj_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    session->response.code = response;
    session->response.parameter[0] = new_handle;
    return send_response(session, 1, 0);
}

static void handle_set_obj_protection(ptp_session_t *session)
{
    DECLARE_FNAME("handle_set_obj_protection");

    DBG_V(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }
    
    if (!verify_command_supported(session,
        (void *)session->inst->cbs.set_obj_protection_h, 0))
    {
        return;
    }

    session->response.code = session->inst->cbs.set_obj_protection_h(session,
        (ptp_obj_handle_t)session->operation.parameter[0],
        (ptp_protection_status_t)session->operation.parameter[1]);

    if (session->response.code == PTP_RESP_OK)
        return;

Error:
    send_response(session, 0, 0);
}

static void handle_format_store(ptp_session_t *session)
{
    DECLARE_FNAME("handle_format_store");

    DBG_V(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }
    
    if (!verify_command_supported(session,
        (void *)session->inst->cbs.format_store_h, 0))
    {
        return;
    }

    session->response.code = session->inst->cbs.format_store_h(session,
        (ptp_storage_id_t)session->operation.parameter[0],
        (ptp_filesystem_type_t)session->operation.parameter[1]);

    if (session->response.code == PTP_RESP_OK)
        return;

Error:
    send_response(session, 0, 0);
}

static void handle_self_test(ptp_session_t *session)
{
    DECLARE_FNAME("handle_self_test");

    DBG_V(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }
    
    if (!verify_command_supported(session,
        (void *)session->inst->cbs.self_test_h, 0))
    {
        return;
    }

    session->response.code = session->inst->cbs.self_test_h(session);

    if (session->response.code == PTP_RESP_OK)
        return;

Error:
    send_response(session, 0, 0);
}

static void handle_power_down(ptp_session_t *session)
{
    DECLARE_FNAME("handle_power_down");

    DBG_V(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname,
            session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Error;
    }
    
    if (!verify_command_supported(session,
        (void *)session->inst->cbs.power_down_h, 0))
    {
        return;
    }

    /* We must flag this here so that the application can call the completion
     * from within the handler if it chooses to do so. */
    session->power_down = 1;
    
    session->response.code = session->inst->cbs.power_down_h(session);

    if (session->response.code == PTP_RESP_OK)
        return;

Error:
    send_response(session, 0, 0);
}

static void handle_get_dev_prop_desc(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_dev_prop_desc");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (!verify_command_supported(session,
            (void *)session->inst->cbs.get_dev_prop_desc_h, 1))
        {
            break;
        }
        
        session->response.code = session->inst->cbs.get_dev_prop_desc_h(
            session, (ptp_dev_prop_t)session->operation.parameter[0]);
        
        if (session->response.code != PTP_RESP_OK)
            send_response(session, 0, 1);
        
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 0, 0);
        break;
    }

    return;
}

#define ADD_SIZE_OR_BAIL        \
    if (size)                   \
    {                           \
        total_size += size;     \
        buf_size -= size;       \
        if (buf_size <= 0)      \
            goto Error;         \
    }                           \
    else                        \
        goto Error;

static jresult_t get_dev_prop_desc_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_dev_prop_desc_t *dev_prop_desc)
{
    ptp_session_t *session = (ptp_session_t *)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    void *buf;
    juint32_t i;
    juint32_t size = 0;
    juint32_t total_size = 5;
    jint32_t buf_size;
    DECLARE_FNAME("ptp_get_dev_prop_desc_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n",fname));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        return send_response(session, 0, 1);

    buf = ptpbuf->buff + session->inst->transport_hdr_size;
    buf_size = ptpbuf->buff_size - session->inst->transport_hdr_size
        - total_size;

    pack_u16(&buf, dev_prop_desc->prop_code);
    pack_u16(&buf, dev_prop_desc->data_type);
    pack_u8(&buf, dev_prop_desc->writable ? 1 : 0);
    size = pack_prop_value(dev_prop_desc->data_type, &buf, buf_size,
        dev_prop_desc->default_value);
    ADD_SIZE_OR_BAIL;
    size = pack_prop_value(dev_prop_desc->data_type, &buf, buf_size,
        dev_prop_desc->current_value);
    ADD_SIZE_OR_BAIL;
    size = sizeof(juint8_t);
    ADD_SIZE_OR_BAIL;
    pack_u8(&buf, dev_prop_desc->form_flag);

    switch(dev_prop_desc->form_flag)
    {
    case PTP_ENUM_FORM:
        size = sizeof(juint16_t);
        ADD_SIZE_OR_BAIL;
        pack_u16(&buf, dev_prop_desc->num_allowed_values);

    case PTP_RANGE_FORM:
        for (i=0; i<dev_prop_desc->num_allowed_values; i++)
        {
            size = pack_prop_value(dev_prop_desc->data_type, &buf, buf_size,
                &dev_prop_desc->allowed_values[i]);
            ADD_SIZE_OR_BAIL;
        }
        break;
        
    case PTP_NONE_FORM:
    default:
        break;
    }

    ptpbuf->data_len = total_size;
    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

Error:
    return JENOMEM;
}

#undef ADD_SIZE_OR_BAIL

static void handle_get_dev_prop_val(ptp_session_t *session)
{
    DECLARE_FNAME("handle_get_dev_prop_val");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (!verify_command_supported(session,
            (void *)session->inst->cbs.get_dev_prop_val_h, 1))
        {
            break;
        }
        
        session->response.code = session->inst->cbs.get_dev_prop_val_h(
            session, (ptp_dev_prop_t)session->operation.parameter[0]);
        
        if (session->response.code != PTP_RESP_OK)
            send_response(session, 0, 1);
        
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 0, 0);
        break;
    }

    return;
}

static jresult_t get_dev_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_prop_value_t *dev_prop_val)
{
    ptp_session_t *session = (ptp_session_t *)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    void *buf;
    DECLARE_FNAME("ptp_get_dev_prop_val_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n",fname));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        return send_response(session, 0, 1);

    buf = ptpbuf->buff + session->inst->transport_hdr_size;

    ptpbuf->data_len = pack_prop_value(dev_prop_val->data_type, &buf,
        ptpbuf->buff_size - session->inst->transport_hdr_size, dev_prop_val);

    if (!ptpbuf->data_len)
        goto Error;
    
    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

Error:
    return JENOMEM;
}

static ptp_type_t get_prop_type(ptp_dev_prop_t prop_code)
{
    switch(prop_code)
    {
    case PTP_DEV_PROP_BATTERY_LEVEL:
    case PTP_DEV_PROP_COMPRESSION_SETTING:
    case PTP_DEV_PROP_CONTRAST:
    case PTP_DEV_PROP_SHARPNESS:
    case PTP_DEV_PROP_DIGITAL_ZOOM:
        return PTP_TYPE_UINT8;

    case PTP_DEV_PROP_FUNCTIONAL_MODE:
    case PTP_DEV_PROP_WHITE_BALANCE:
    case PTP_DEV_PROP_F_NUMBER:
    case PTP_DEV_PROP_FOCUS_DISTANCE:
    case PTP_DEV_PROP_FOCUS_MODE:
    case PTP_DEV_PROP_EXPOSURE_METERING_MODE:
    case PTP_DEV_PROP_FLASH_MODE:
    case PTP_DEV_PROP_EXPOSURE_PROGRAM_MODE:
    case PTP_DEV_PROP_EXPOSURE_INDEX:
    case PTP_DEV_PROP_STILL_CAPTURE_MODE:
    case PTP_DEV_PROP_EFFECT_MODE:
    case PTP_DEV_PROP_BURST_NUMBER:
    case PTP_DEV_PROP_BURST_INTERVAL:
    case PTP_DEV_PROP_TIMELAPSE_NUMBER:
    case PTP_DEV_PROP_FOCUS_METERING_MODE:
        return PTP_TYPE_UINT16;

    case PTP_DEV_PROP_EXPOSURE_BIAS_COMPENSATION:
        return PTP_TYPE_INT16;
    
    case PTP_DEV_PROP_FOCAL_LENGTH:
    case PTP_DEV_PROP_EXPOSURE_TIME:
    case PTP_DEV_PROP_CAPTURE_DELAY:
    case PTP_DEV_PROP_TIMELAPSE_INTERVAL:
        return PTP_TYPE_UINT32;
        
    case PTP_DEV_PROP_IMAGE_SIZE:
    case PTP_DEV_PROP_DATE_TIME:
    case PTP_DEV_PROP_RGB_GAIN:
    case PTP_DEV_PROP_UPLOAD_URL:
    case PTP_DEV_PROP_ARTIST:
    case PTP_DEV_PROP_COPYRIGHT_INFO:
        return PTP_TYPE_STR;

    default:
        return PTP_TYPE_UNDEF;
    }
}        

static void handle_set_dev_prop_val(ptp_session_t *session)
{
    DECLARE_FNAME("handle_set_dev_prop_val");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    
    switch(session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session,
            (void *)session->inst->cbs.set_dev_prop_val_h, 1))
        {
            start_read_data(session);
        }

        break;

    case PTP_STATE_READ_DATA:
        {
            ptp_prop_value_t prop_val;
            ptp_type_t prop_type;
            juint32_t ubytes;
            ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    
            prop_type = get_prop_type(
                (ptp_dev_prop_t)session->operation.parameter[0]);

            if (PTP_IS_TYPE_ARRAY(prop_type))
            {
                prop_val.data.a_uint8 = (juint8_t*)jmalloc(ptpbuf->buff_size,
                    0);
            }

            /* If host tries to send more data than we can accept in one
             * block, send an error */
            if (session->end_offset != ptpbuf->data_len)
            {
                session->response.code = PTP_RESP_GENERAL_ERROR;
                send_response(session, 0, 1);
                return;
            }
            
            ubytes = unpack_prop_value(prop_type,
                ptpbuf->buff + ptpbuf->data_offset,
                ptpbuf->data_len, &prop_val,
                prop_val.data.a_uint8, NULL, NULL);

            if (ubytes)
            {
                session->response.code =
                    session->inst->cbs.set_dev_prop_val_h(session,
                        (ptp_dev_prop_t)session->operation.parameter[0],
                        &prop_val);
            }
            else
            {
                session->response.code = PTP_RESP_GENERAL_ERROR;
            }

            ptp_prop_value_clear(&prop_val);
             
            if (session->response.code != PTP_RESP_OK)
            {
                session->app_done = 1;
                schedule_advance(session);
            }
        }
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        send_response(session, 0, 0);
        break;
    }
    
    return;
}
    
static jresult_t set_dev_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    DECLARE_FNAME("ptp_set_dev_prop_val_complete");
    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    session->response.code = response;
    session->app_done = 1;
    return schedule_advance(session);
}

/*
 *   PTP transaction dispatch
 */
static jresult_t dispatch(ptp_session_t *session)
{
    jresult_t rc = 0;
    DECLARE_FNAME("dispatch");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    if (session->state == PTP_STATE_COMMAND)
    {
        prepare_transfer(session);
        session->response.transaction_id = session->operation.transaction_id;
    }

    if (session->transaction_cancelled)
    {
        session->transaction_cancelled = 0;
        session->response.code = PTP_RESP_INCOMPLETE_TRANSFER;
        return send_response(session, 0, 0);
    }
    
    switch(session->operation.code)
    {
    case PTP_OP_GET_DEVICE_INFO:
        handle_get_dev_info(session);
        break;

    case PTP_OP_OPEN_SESSION:
        handle_open_session(session);
        break;

    case PTP_OP_CLOSE_SESSION:
        handle_close_session(session, 0);
        break;

    case PTP_OP_GET_STORAGE_IDS:
        handle_get_storage_ids(session);
        break;

    case PTP_OP_GET_STORAGE_INFO:
        handle_get_storage_info(session);
        break;

    case PTP_OP_GET_NUM_OBJECTS:
        handle_get_num_objects(session);
        break;

    case PTP_OP_GET_OBJECT_HANDLES:
        handle_get_object_handles(session);
        break;

    case PTP_OP_GET_OBJECT_INFO:
        handle_get_object_info(session);
        break;

    case PTP_OP_GET_PARTIAL_OBJECT:
    case PTP_OP_GET_OBJECT:
        handle_get_object(session);
        break;

    case PTP_OP_GET_THUMB:
        handle_get_thumb(session);
        break;

    case PTP_OP_SEND_OBJECT_INFO:
        handle_send_object_info(session);
        break;

    case PTP_OP_SEND_OBJECT:
        handle_send_object(session);
        break;

    case PTP_OP_RESET_DEVICE:
        handle_device_reset(session);
        break;

    case PTP_OP_DELETE_OBJECT:
        handle_delete_obj(session);
        break;

    case PTP_OP_FORMAT_STORE:
        handle_format_store(session);
        break;

    case PTP_OP_SELF_TEST:
        handle_self_test(session);
        break;

    case PTP_OP_SET_OBJECT_PROTECTION:
        handle_set_obj_protection(session);
        break;

    case PTP_OP_POWER_DOWN:
        handle_power_down(session);
        break;

    case PTP_OP_GET_DEVICE_PROP_DESC:
        handle_get_dev_prop_desc(session);
        break;

    case PTP_OP_GET_DEVICE_PROP_VALUE:
        handle_get_dev_prop_val(session);
        break;

    case PTP_OP_SET_DEVICE_PROP_VALUE:
        handle_set_dev_prop_val(session);
        break;

    case PTP_OP_RESET_DEVICE_PROP_VALUE:
        handle_reset_dev_prop_val(session);
        break;

    case PTP_OP_MOVE_OBJECT:
        handle_move_obj(session);
        break;

    case PTP_OP_COPY_OBJECT:
        handle_copy_obj(session);
        break;

    case PTP_OP_INITIATE_OPEN_CAPTURE:
    case PTP_OP_INITIATE_CAPTURE:
    case PTP_OP_TERMINATE_OPEN_CAPTURE:        

    case PTP_OP_UNDEFINED:
    default:
        /* If an extension is registered, let it try to handle the command */
        if (session->inst->vs_ext_params.dispatch_h)
            rc = session->inst->vs_ext_params.dispatch_h(session);
        else
            session->response.code = PTP_RESP_OPERATION_NOT_SUPPORTED;

        if (session->response.code == PTP_RESP_OPERATION_NOT_SUPPORTED)
        {
            DBG_E(DPROTO_PTP, ("%s: Operation code unsupported (%X)\n", fname,
                session->operation.code));
            send_response(session, 0, 0);
        }
        break;
    }

    return rc;
}

/******************************************************************
 *      PTP Event handlers
 *****************************************************************/

jresult_t ptp_cancel_transaction(ptp_sessionh_t sessionh)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_cancel_transaction");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    if (!session->transaction_id)
    {
        DBG_E(DPROTO_PTP, ("%s: invalid transaction id\n", fname));
        return EUNKNOWN;
    }

    ioevent.code = PTP_EVENT_CANCEL_TRANSACTION;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = session->transaction_id;
    session->response.code = PTP_RESP_INCOMPLETE_TRANSFER;
    
    return send_event(session, &ioevent, 0);
}

jresult_t ptp_object_added(ptp_sessionh_t sessionh, ptp_obj_handle_t obj)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_object_added");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_OBJECT_ADDED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)obj;
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_object_removed(ptp_sessionh_t sessionh, ptp_obj_handle_t obj)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_object_removed");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_OBJECT_REMOVED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)obj;
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_store_added(ptp_sessionh_t sessionh, ptp_storage_id_t store)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_store_added");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_STORE_ADDED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)store;
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_store_removed(ptp_sessionh_t sessionh, ptp_storage_id_t store)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_store_removed");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_STORE_REMOVED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)store;
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_device_prop_changed(ptp_sessionh_t sessionh, ptp_dev_prop_t prop)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_device_prop_changed");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_DEVICE_PROP_CHANGED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)prop;
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_object_info_changed(ptp_sessionh_t sessionh, ptp_obj_handle_t obj)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_object_info_changed");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_OBJECT_INFO_CHANGED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)obj;
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_device_info_changed(ptp_sessionh_t sessionh)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_device_info_changed");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_DEVICE_INFO_CHANGED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    return send_event(session, &ioevent, 0);
}

jresult_t ptp_storage_info_changed(ptp_sessionh_t sessionh,
    ptp_storage_id_t store)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_storage_info_changed");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_STORAGE_INFO_CHANGED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)store;
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_request_object_transfer(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_request_object_transfer");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_REQUEST_OBJECT_TRANSFER;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)obj;
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_store_full(ptp_sessionh_t sessionh, ptp_storage_id_t store)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_store_full");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_STORE_FULL;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)store;
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_device_reset(ptp_sessionh_t sessionh)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_device_reset");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_DEVICE_RESET;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    return send_event(session, &ioevent, 0);
}

jresult_t ptp_capture_complete(ptp_sessionh_t sessionh)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_capture_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_CAPTURE_COMPLETE;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    /* TODO: when capture operations are supported the transaction id
     * should be saved in our context for use by this event */
    /*ioevent.parameter[0] = (juint32_t)session->capture_transaction_id;*/
    return send_event(session, &ioevent, 1);
}

jresult_t ptp_unreported_status(ptp_sessionh_t sessionh)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("ptp_unreported_status");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    ioevent.code = PTP_EVENT_UNREPORTED_STATUS;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    return send_event(session, &ioevent, 0);
}

/******************************************************************
 *      Initialization functions
 *****************************************************************/

jresult_t ptp_create_instance(ptp_instanceh_t *insth, ptp_transport_ops *ops,
    ptp_transport_ctx_t tr_ctx, ptp_init_args *args)
{
    ptp_instance_t *inst = NULL;
    jresult_t rc = 0;
    DECLARE_FNAME("ptp_create_instance");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    inst = jmalloc(sizeof(ptp_instance_t), M_ZERO);
    if (!inst)
    {
        DBG_E(DPROTO_PTP, ("%s: failed to allocate instance\n", fname));
        rc = JENOMEM;
        goto Exit;
    }

    j_memcpy(&inst->ops, ops, sizeof(ptp_transport_ops));
    inst->tr_ctx = tr_ctx;

    init_session(&inst->null_session, inst);
    *insth = inst;

    inst->transport_hdr_size = args->transport_datahdr_size;
    inst->transport_block_size = args->transport_block_size;
    inst->max_sessions = args->max_sessions;

Exit:
    return rc;
}

jresult_t ptp_register_responder(ptp_instanceh_t insth, ptp_responder_cbs *cbs,
    ptp_appctx_t global_ctx)
{
    ptp_instance_t *inst = (ptp_instance_t*)insth;
    DECLARE_FNAME("ptp_register_responder");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    j_memcpy(&inst->cbs, cbs, sizeof(ptp_responder_cbs));
    inst->global_appctx = global_ctx;
    inst->comp_cbs.operation_comp = operation_complete;
    inst->comp_cbs.get_num_obj_comp = get_num_objects_complete;
    inst->comp_cbs.get_dev_info_comp = get_dev_info_complete;
    inst->comp_cbs.get_storage_ids_comp = get_storage_ids_complete;
    inst->comp_cbs.get_storage_info_comp = get_storage_info_complete;
    inst->comp_cbs.get_obj_handles_comp = get_obj_handles_complete;
    inst->comp_cbs.get_obj_info_comp = get_obj_info_complete;
    inst->comp_cbs.get_obj_comp = get_obj_complete;
    inst->comp_cbs.send_obj_info_comp = send_obj_info_complete;
    inst->comp_cbs.send_obj_comp = send_obj_complete;
    inst->comp_cbs.copy_obj_comp = copy_obj_complete;
    inst->comp_cbs.get_dev_prop_desc_comp = get_dev_prop_desc_complete;
    inst->comp_cbs.get_dev_prop_val_comp = get_dev_prop_val_complete;
    inst->comp_cbs.set_dev_prop_val_comp = set_dev_prop_val_complete;
    return 0;
}

jresult_t ptp_delete_instance(ptp_instanceh_t insth, jbool_t graceful)
{
    ptp_instance_t *inst = (ptp_instance_t*)insth;
    DECLARE_FNAME("ptp_delete_instance");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    /* Close all sessions */
    ptp_stop_responder(insth, graceful);

    /* Call uninit */
    inst->cbs.unregister_h(inst, inst->global_appctx);
    
    /* Release vendor specific callbacks */
    if (inst->vs_ext_params.del_cbs_h)
        inst->vs_ext_params.del_cbs_h(inst->vs_ext_cbs);

    jfree(insth);
    return 0;
}

jresult_t ptp_start_responder(ptp_instanceh_t insth)
{
    ptp_instance_t *inst = (ptp_instance_t*)insth;
    DECLARE_FNAME("ptp_start_responder");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    /* Setup null-session */
    init_session(&inst->null_session, inst);
    return inst->ops.read_operation_h(inst->tr_ctx, NULL, 0);
}

jresult_t ptp_stop_responder(ptp_instanceh_t insth, jbool_t graceful)
{
    ptp_instance_t *inst = (ptp_instance_t*)insth;
    DECLARE_FNAME("ptp_stop_responder");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    /* Cleanup */
    while(inst->sessions)
    {
        handle_close_session(inst->sessions, 1);
        inst->sessions = NULL;
    }

    return 0;
}

jresult_t ptp_register_vs_ext(ptp_instanceh_t insth,
    ptp_vs_ext_params_t *ext_params, ptp_vs_ext_cbs_t ext_cbs)
{
    ptp_instance_t *inst = (ptp_instance_t*)insth;
    inst->vs_ext_cbs = ext_cbs;
    j_memcpy(&inst->vs_ext_params, ext_params, sizeof(inst->vs_ext_params));
    return 0;
}

jresult_t ptp_hook_responder(ptp_instanceh_t insth,
    ptp_responder_cbs *prev_cbs, ptp_responder_comp_cbs *prev_comp_cbs,
    ptp_responder_cbs *hook_cbs, ptp_responder_comp_cbs *hook_comp_cbs,
    ptp_appctx_t global_hookctx)
{
    ptp_instance_t *inst = (ptp_instance_t*)insth;
    void **pfn;
    void **new_pfn;
    jint_t i;
    DECLARE_FNAME("ptp_hook_responder");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

    /* Provide current responder info to hook */
    if (prev_cbs)
        j_memcpy(prev_cbs, &inst->cbs, sizeof(ptp_responder_cbs));
    
    if (prev_comp_cbs)
    {
        j_memcpy(prev_comp_cbs, &inst->comp_cbs,
            sizeof(ptp_responder_comp_cbs));
    }

    /* Fill in hook callbacks where requested */
    pfn = (void**)&inst->cbs;
    new_pfn = (void**)hook_cbs;
    i = sizeof(ptp_responder_cbs)/sizeof(pfn);
    for (; i ; i--, pfn++, new_pfn++)
    {
        if (*new_pfn)
        {
            DBG_X(DPROTO_PTP, ("%s: pfn=%p, new_pfn=%p\n", fname, *pfn, *new_pfn));
            *pfn = *new_pfn;
        }
        else
            DBG_X(DPROTO_PTP, ("%s: pfn=%p\n", fname, *pfn));
    }

    /* Fill in hook completion callbacks where requested */
    pfn = (void**)&inst->comp_cbs;
    new_pfn = (void**)hook_comp_cbs;
    i = sizeof(ptp_responder_comp_cbs)/sizeof(pfn);
    for (; i ; i--, pfn++, new_pfn++)
    {
        if (*new_pfn)
        {
            DBG_X(DPROTO_PTP, ("%s: comp: pfn=%p, new_pfn=%p\n", fname, *pfn, *new_pfn));
            *pfn = *new_pfn;
        }
        else
            DBG_X(DPROTO_PTP, ("%s: comp: pfn=%p\n", fname, *pfn));
    }

    inst->global_hookctx = global_hookctx;
    return 0;
}

ptp_appctx_t ptp_get_global_hookctx(ptp_sessionh_t sessionh)
{
    return ((ptp_session_t*)sessionh)->inst->global_hookctx;
}

ptp_appctx_t ptp_inst_get_global_hookctx(ptp_instanceh_t inst)
{
    return ((ptp_instance_t*)inst)->global_hookctx;
}

/******************************************************************
 *      Completion functions (routing to actual functions)
 *****************************************************************/

jresult_t ptp_operation_complete(ptp_sessionh_t sessionh,
    ptp_response_t response)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.operation_comp(
        sessionh, response);
}

jresult_t ptp_get_dev_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_dev_info_t *devinfo)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.get_dev_info_comp(
        sessionh, response, devinfo);
}

jresult_t ptp_get_obj_handles_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t *handles,
    juint32_t count)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.get_obj_handles_comp(
        sessionh, response, handles, count);
}

jresult_t ptp_get_storage_ids_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t *stores,
    juint32_t count)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.get_storage_ids_comp(
        sessionh, response, stores, count);
}

jresult_t ptp_get_storage_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_info_t *store_info)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.get_storage_info_comp(
        sessionh, response, store_info);
}

jresult_t ptp_get_num_objects_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t num_objects)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.get_num_obj_comp(
        sessionh, response, num_objects);
}

jresult_t ptp_get_obj_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_info_t *objinfo)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.get_obj_info_comp(
        sessionh, response, objinfo);
}

jresult_t ptp_get_obj_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t actual_size, juint32_t obj_size)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.get_obj_comp(
        sessionh, response, actual_size, obj_size);
}

jresult_t ptp_send_obj_info_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t actual_store,
    ptp_obj_handle_t actual_folder, ptp_obj_handle_t new_handle)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.send_obj_info_comp(
        sessionh, response, actual_store, actual_folder, new_handle);
}

jresult_t ptp_send_obj_complete(ptp_sessionh_t sessionh,
    ptp_response_t response)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.send_obj_comp(
        sessionh, response);
}

jresult_t ptp_copy_obj_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t new_handle)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.copy_obj_comp(
        sessionh, response, new_handle);
}

jresult_t ptp_get_dev_prop_desc_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_dev_prop_desc_t *dev_prop_desc)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.get_dev_prop_desc_comp(
        sessionh, response, dev_prop_desc);
}

jresult_t ptp_get_dev_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_prop_value_t *dev_prop_val)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.get_dev_prop_val_comp(
        sessionh, response, dev_prop_val);
}

jresult_t ptp_set_dev_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response)
{
    return ((ptp_session_t*)sessionh)->inst->comp_cbs.set_dev_prop_val_comp(
        sessionh, response);
}

/******************************************************************
 *      Dataflow functions
 *****************************************************************/

jresult_t send_response(ptp_session_t *session, juint32_t num_params,
    jbool_t in_data_phase)
{
    int i;
    ptp_io_block_t response;
    DECLARE_FNAME("send_response");

    DBG_X(DPROTO_PTP, ("%s: entered (%X) (%d)\n", fname,
        session->response.code, in_data_phase));

    session->state = PTP_STATE_RESPONSE;    
    PTP_H2T16(&response.code, session->response.code);
    PTP_H2T32(&response.session_id, session->session_id);
    PTP_H2T32(&response.transaction_id, session->response.transaction_id);
    for(i=0; i<num_params; i++)
        PTP_H2T32(&response.parameter[i], session->response.parameter[i]);

    return session->inst->ops.send_response_h(session->inst->tr_ctx, session,
        &response, num_params, in_data_phase);
}

jresult_t send_event(ptp_session_t *session, ptp_io_block_t *event,
    juint32_t num_params)
{
    int i;
    ptp_io_block_t evt;
    DECLARE_FNAME("send_event");

    DBG_X(DPROTO_PTP, ("%s: entered (%X)\n", fname, event->code));

    PTP_H2T16(&evt.code, event->code);
    PTP_H2T32(&evt.session_id, event->session_id);
    PTP_H2T32(&evt.transaction_id, event->transaction_id);
    for(i=0; i<PTP_BLOCK_NUM_PARAMS; i++)
        PTP_H2T32(&evt.parameter[i], event->parameter[i]);

    return session->inst->ops.send_event_h(session->inst->tr_ctx, session,
        &evt, num_params);
}

static void prepare_transfer(ptp_session_t *session)
{
    session->end_offset = 0;
    session->start_offset = 0;
    session->curr_app_offset = 0;
    session->curr_tr_offset = 0;
    session->app_done = 0;
    session->transport_done = 0;
    session->app_buf_idx = 0;
    session->data[0].data_len = 0;
    session->data[0].data_offset = 0;
    session->data[1].data_len = 0;
    session->data[1].data_offset = 0;
}

jresult_t start_send_data(ptp_session_t *session, juint32_t total_size,
        juint32_t block_size)
{    
    DECLARE_FNAME("start_send_data");
    DBG_X(DPROTO_PTP, ("%s: entered. (%u, %u)\n", fname, block_size,
        total_size));

    session->state = PTP_STATE_SEND_DATA;
    session->end_offset = total_size;
    session->curr_app_offset = block_size;
    session->app_done = 1;
    session->transport_done = 1;
    return schedule_advance(session);
}

jresult_t start_read_data(ptp_session_t *session)
{
    DECLARE_FNAME("start_read_data");
    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
        
    session->state = PTP_STATE_READ_DATA;
    session->app_done = 1;
    return schedule_transport(session);
}

static jresult_t schedule_transport(ptp_session_t *session)
{
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx^1];
    DECLARE_FNAME("schedule_transport");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    DBG_PTPBUF(session, session->app_buf_idx^1);

    if (session->end_offset && (session->curr_tr_offset == session->end_offset))
    {
        session->transport_done = 1;
        return schedule_advance(session);
    }
    
    session->transport_done = 0;
    
    if (session->state == PTP_STATE_READ_DATA)
    {
        return session->inst->ops.read_data_h(session->inst->tr_ctx, session,
            ptpbuf->buff, ptpbuf->buff_size);
    }
    else if (session->state == PTP_STATE_SEND_DATA)
    {
        return session->inst->ops.send_data_h(session->inst->tr_ctx, session,
            ptpbuf->buff, ptpbuf->data_len,
            session->end_offset - session->start_offset,
            (session->end_offset==session->curr_app_offset));
    }

    /* We should not get here */
    DBG_E(DPROTO_PTP, ("%s: Invalid state (%d)\n", fname, session->state));
    return EUNKNOWN;
}

static jresult_t schedule_app(ptp_session_t *session)
{
    DECLARE_FNAME("schedule_app");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    DBG_PTPBUF(session, session->app_buf_idx);

    if (session->curr_app_offset == session->end_offset)
    {
        session->app_done = 1;
        return schedule_advance(session);
    }
    
    session->app_done = 0;
    
    if (session->state == PTP_STATE_READ_DATA ||
        session->state == PTP_STATE_SEND_DATA)
    {
        return dispatch(session);
    }

    /* We should not get here... */
    DBG_E(DPROTO_PTP, ("%s: invalid state (%d)\n", fname, session->state));
    return EUNKNOWN;
}

jresult_t schedule_advance(ptp_session_t *session)
{
    jresult_t rc = 0;
    DECLARE_FNAME("schedule_advance");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));
    
    /* Are we ready for the next round? */
    if (!session->app_done || !session->transport_done)
    {
        DBG_V(DPROTO_PTP, ("%s: app: %s, transport %s\n", fname,
            session->app_done ? "done" : "...",
            session->transport_done ? "done" : "..."));
        goto Exit;
    }

    /* Check if transaction was cancelled */
    if (session->transaction_cancelled)
    {
        DBG_I(DPROTO_PTP, ("%s: transaction cancelled\n", fname));
        return dispatch(session);
    }

    /* Are we done? */
    if (session->curr_tr_offset == session->end_offset &&
        session->curr_app_offset == session->end_offset)
    {
        DBG_V(DPROTO_PTP, ("%s: transfer done (%d)\n", fname,
            session->end_offset));
        session->state = PTP_STATE_DATA_DONE;
        return dispatch(session);
    }

    /* Update offsets here !!! (otherwise we loose the last block) */
    if (session->curr_tr_offset != session->end_offset)
    {
        session->curr_tr_offset +=
            session->data[session->app_buf_idx^1].data_len;
    }

    if (session->curr_app_offset != session->end_offset)
    {
        session->curr_app_offset +=
            session->data[session->app_buf_idx].data_len;
    }

    DBG_X(DPROTO_PTP, ("%s:\tapp_done: %u\n", fname, session->app_done?1:0));
    DBG_X(DPROTO_PTP, ("%s:\ttransport_done: %u\n", fname,
        session->transport_done?1:0));
    DBG_X(DPROTO_PTP, ("%s:\tend_offset: %u\n", fname, session->end_offset));
    DBG_X(DPROTO_PTP, ("%s:\tstart_offset: %u\n", fname, session->start_offset));
    DBG_X(DPROTO_PTP, ("%s:\tapp_offset: %u\n", fname,
        session->curr_app_offset));
    DBG_X(DPROTO_PTP, ("%s:\ttr_offset: %u\n\n", fname,
        session->curr_tr_offset));

    /* Go for another round */
    session->app_buf_idx ^= 1;
    session->app_done = 0;
    session->transport_done = 0;

    rc = schedule_transport(session);
    if (rc)
        goto Exit;

    rc = schedule_app(session);
    if (rc)
        goto Exit;

Exit:
    return rc;
}

/******************************************************************
 *      Transport ops implementation
 *****************************************************************/

jresult_t ptp_io_read_operation_complete(ptp_instanceh_t ctx,
    ptp_io_block_t *op)
{
    ptp_instance_t *inst = (ptp_instance_t*)ctx;
    ptp_session_t *session;
    juint16_t opcode;
    juint32_t session_id;
    int i;
    DECLARE_FNAME("ptp_io_read_operation_complete");

    opcode = PTP_T2H16(&op->code);
    session_id = PTP_T2H32(&op->session_id);
    DBG_X(DPROTO_PTP, ("%s: entered (%X, %X)\n", fname, session_id, opcode));

    if (opcode == PTP_OP_OPEN_SESSION ||
        opcode == PTP_OP_GET_DEVICE_INFO)
    {
        session = &inst->null_session;
    }
    else
        session = find_session(inst, session_id);
    
    if (!session)
    {
        session = &inst->null_session;
        session->response.code = PTP_RESP_SESSION_NOT_OPEN;
        return send_response(session, 0, 0);
    }

    if (session->state != PTP_STATE_COMMAND)
    {
        DBG_E(DPROTO_PTP, ("Invalid state (%d)\n", session->state));
        return EUNKNOWN;
    }

    j_memset(&session->response, 0, sizeof(session->response));

    session->operation.code = opcode;
    session->operation.session_id = session_id;
    session->operation.transaction_id = PTP_T2H32(&op->transaction_id);
    for(i=0; i<PTP_BLOCK_NUM_PARAMS; i++)
        session->operation.parameter[i] = PTP_T2H32(&op->parameter[i]);
    
    DBG_OPERATION((void *)op);
    return dispatch(session);
}

jresult_t ptp_io_read_data_complete(ptp_ctx_t ctx, juint32_t size,
    juint32_t total, juint32_t data_offset, jbool_t last)
{
    ptp_session_t *session = (ptp_session_t*)ctx;
    DECLARE_FNAME("ptp_io_read_data_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

#ifdef JDEBUG
    if (session->state != PTP_STATE_READ_DATA)
        DBG_E(DPROTO_PTP, ("%s: invalid state\n", fname, session->state));
#endif

    /* Set the transfer size upon receiving the initial data packet */
    if (total)
    {
        /* In SendObject operation we use the size provided by SendObjectInfo */
        if (session->operation.code == PTP_OP_SEND_OBJECT)
        {
            if (total != session->obj_size)
            {
                DBG_E(DPROTO_PTP, ("%s: size verification mismatch"
                    " (%d != %d)\n", fname, session->obj_size, total));
                return JEIO;
            }

            session->end_offset = session->obj_size;
        }
        else /* In other operation use the size provided by the transport */
            session->end_offset = total;

    }

    /* With USB, if a host sends a short packet too soon, we use the 'last'
     * parameter for detecting a datastage the has 'finished' before
     * it is supposed to */
    if (last && session->end_offset != session->curr_tr_offset+size)
    {
        DBG_E(DPROTO_PTP, ("%s: transfer size mismatch (%d != %d)\n", fname,
            session->end_offset, session->curr_tr_offset+size));
        return JEIO;
    }
    
    session->transport_done = 1;
    session->data[session->app_buf_idx^1].data_len = size;
    session->data[session->app_buf_idx^1].data_offset = data_offset;
    DBG_PTPBUF(session, session->app_buf_idx^1);
    return schedule_advance(session);
}

jresult_t ptp_io_send_data_complete(ptp_ctx_t ctx, juint32_t size)
{
    ptp_session_t *session = (ptp_session_t*)ctx;
    DECLARE_FNAME("ptp_io_send_data_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

#ifdef JDEBUG
    if (session->state != PTP_STATE_SEND_DATA)
        DBG_E(DPROTO_PTP, ("%s: invalid state (%d)\n", fname, session->state));

    if (session->data[session->app_buf_idx^1].data_len != size)
    {
        DBG_E(DPROTO_PTP, ("%s: transfer error (%d/%d)\n", fname,
            session->data[session->app_buf_idx^1].data_len, size));
    }
#endif
    
    DBG_PTPBUF(session, session->app_buf_idx^1);
    session->transport_done = 1;
    return schedule_advance(session);
}

jresult_t ptp_io_send_response_complete(ptp_ctx_t ctx)
{
    ptp_session_t *session = (ptp_session_t*)ctx;
    DECLARE_FNAME("ptp_io_send_response_complete");

    DBG_X(DPROTO_PTP, ("%s: entered\n", fname));

#ifdef JDEBUG
    if (session->state != PTP_STATE_RESPONSE)
        DBG_E(DPROTO_PTP, ("%s: invalid state (%d)\n", fname, session->state));
#endif

    /* For successful transactions, this is the spot where we should have all
     * of the information required for the transaction completion callback.
     * Additionally, there are no pending IO operations at this point */
    if (session->inst->cbs.transaction_complete_h)
    {
        session->inst->cbs.transaction_complete_h(session,
            PTP_IO_STATUS_OK, (ptp_operation_t)session->operation.code,
            (ptp_response_t)session->response.code, session->end_offset,
            MIN(session->curr_app_offset, session->curr_tr_offset));
    }

    /* This is the right place to perform operations that cannot be done
     * in the context of handlers or the application (i.e. completion ctx) */
    if (session->power_down || session->device_reset)
    {
        if (session->response.code == PTP_RESP_OK)
        {
            jbool_t device_reset = session->device_reset;
            ptp_instance_t *inst = session->inst;

            /* Close sessions */
            ptp_stop_responder(inst, 1);

            /* On reset - start up again */
            if (device_reset)
                ptp_start_responder(inst);

            return 0;
        }

        /* We need to reset these flags no matter what the response... */
        session->device_reset = 0;
        session->power_down = 0;
    }

    session->state = PTP_STATE_COMMAND;
    j_memset(&session->operation, 0, sizeof(session->operation));
    return session->inst->ops.read_operation_h(session->inst->tr_ctx, NULL,
        session->session_id);
}

jresult_t ptp_io_initiator_event(ptp_instanceh_t insth,
    ptp_io_block_t *io_event)
{
    ptp_instance_t *inst = (ptp_instance_t*)insth;
    ptp_session_t *session = NULL;
    ptp_block_t evt;
    DECLARE_FNAME("ptp_io_initiator_event");

    evt.code = PTP_T2H16(&io_event->code);
    evt.session_id = PTP_T2H32(&io_event->session_id);

    DBG_X(DPROTO_PTP, ("%s: entered (%X, %X)\n", fname, evt.session_id,
        evt.code));

    session = find_session(inst, evt.session_id);
    if (!session)
        return EUNKNOWN;

    switch(evt.code)
    {
    case PTP_EVENT_CANCEL_TRANSACTION:
        session->transaction_cancelled = 1;
        if (session->inst->cbs.transaction_cancelled_h)
            session->inst->cbs.transaction_cancelled_h(session);
        break;
        
    case PTP_EVENT_DEVICE_RESET:
    default:
        DBG_W(DPROTO_PTP, ("%s: can not handle event %X\n", fname,
            evt.code));
        return EUNKNOWN;
    }
    
    return 0;
}



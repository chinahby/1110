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

#include "mtp_types.h"
#include <ptp_responder_api.h>
#include <ptp.h>
#include "mtp_responder_api.h"
#include "mtp.h"

/*************
 * MTP Events
 *************/

/**
 * Function name:  mtp_obj_prop_changed
 * Description:    function being called in case there is a change in property 
 *                 for a specific object. It will notify the host about the
 *                 change according to the information provided by the
 *                 application.
 *                 
 * Parameters:
 *     @sessionh:       session handler provided to the application earlier.
 *     @obj_handle:     the object handle of the relevant object 
 *     @obj_prop_code:  the property code for the changed property
 *
 * Return value:        0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t mtp_obj_prop_changed(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle,mtp_obj_prop_t obj_prop_code)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("mtp_obj_prop_changed");

    DBG_X(DPROTO_MTP, ("%s: entered\n", fname));

    ioevent.code = MTP_EVENT_OBJECT_PROP_CHANGED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)obj_handle;
    ioevent.parameter[1] = (juint32_t)obj_prop_code;
    return send_event(session, &ioevent, 2);
}

/**
 * Function name:  mtp_obj_prop_desc_changed
 * Description:    function being called in case there is a change in the 
 *                 property descriptor of a specific object property. it 
 *                 will notify the host about the change according to the
 *                 information provided by the application.
 * Parameters:
 *     @sessionh:         session handler provided to the application earlier
 *     @obj_prop_code:    the object property code which it descriptor has
 *                        changed
 *     @format:           the object format code which has its property
 *                        descriptor changed.
 *
 * Return value:        0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t mtp_obj_prop_desc_changed(ptp_sessionh_t sessionh,
    mtp_obj_prop_t obj_prop_code, ptp_obj_format_t format)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("mtp_obj_prop_desc_changed");

    DBG_X(DPROTO_MTP, ("%s: entered\n", fname));

    ioevent.code = MTP_EVENT_OBJECT_PROP_DESC_CHANGED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)obj_prop_code;
    ioevent.parameter[1] = (juint32_t)format;
    return send_event(session, &ioevent, 2);
}

/**
 * Function name:  mtp_obj_references_changed
 * Description:    function being called in case there is a change in the
 *                 references associated with the object noted by its object 
 *                 handle
 * Parameters:
 *     @sessionh:       session handler provided to the application earlier
 *     @obj_handle:     the object handle of the relevant object 
 *
 * Return value:   
 * Scope:          Global
 **/
jresult_t mtp_obj_references_changed(ptp_sessionh_t sessionh,
    ptp_obj_handle_t obj_handle)
{
    ptp_session_t *session = (ptp_session_t*)sessionh;
    ptp_io_block_t ioevent;
    DECLARE_FNAME("mtp_obj_references_changed");

    DBG_X(DPROTO_MTP, ("%s: entered\n", fname));

    ioevent.code = MTP_EVENT_OBJECT_REFERENCES_CHANGED;
    ioevent.session_id = session->session_id;
    ioevent.transaction_id = 0xFFFFFFFF;
    ioevent.parameter[0] = (juint32_t)obj_handle;
    return send_event(session, &ioevent, 1);
}

/**************************************
 * MTP Internal Handlers & Completions 
 **************************************/
static void handle_get_obj_props_supp(ptp_session_t *session)
{
    mtp_get_obj_props_supp cb = 
        ((mtp_callbacks_t *)PTP_VS_EXT_CBS(session))->get_obj_props_supp_h;

    DECLARE_FNAME("handle_get_obj_props_supp");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    switch (session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session, (void *)cb, 1))
        {
            session->response.code = cb(session,
                    (ptp_obj_format_t)session->operation.parameter[0]);

            if (session->response.code != PTP_RESP_OK)
                send_response(session, 0, 1);
        }
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_MTP, ("%s: Invalid state (%d)\n", fname, session->state));
        break;
    }
}

/* section D.2.29 in the MTP document rev 0.96 (section B.2.1 in rev 0.83) */
jresult_t mtp_get_obj_props_supp_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint16_t array_size,
    mtp_obj_prop_t *obj_prop_code)
{
    ptp_session_t *session = (ptp_session_t *)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    juint32_t handles_size = sizeof(juint32_t) + array_size * sizeof(juint16_t);
    void *buf;
    int i = 0;
    DECLARE_FNAME("mtp_get_obj_props_supp_complete");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        goto Exit;

    if (!ptpbuf->buff)
    {
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Exit;
    }

    if ( handles_size > (ptpbuf->buff_size - session->inst->transport_hdr_size))
    {
        DBG_W(DPROTO_MTP, ("%s: data to send exceeds buffer size\n", fname));
        return JENOMEM;
    }

    buf = ptpbuf->buff + session->inst->transport_hdr_size;

    pack_u32(&buf, array_size);
    for (i = 0 ; i != array_size; i++)
        pack_u16(&buf, (juint16_t)obj_prop_code[i]);

    ptpbuf->data_len = sizeof(juint32_t) + sizeof(juint16_t) * array_size;
    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

Exit:
    return send_response(session, 0, 1);
}

static void handle_get_obj_prop_desc(ptp_session_t *session)
{ 
    mtp_get_obj_prop_desc cb = 
        ((mtp_callbacks_t *)PTP_VS_EXT_CBS(session))->get_obj_prop_desc_h;

    DECLARE_FNAME("handle_get_obj_prop_desc");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    switch (session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session, (void *)cb, 1))
        {
            session->response.code = cb(session,
                (mtp_obj_prop_t)session->operation.parameter[0],
                (ptp_obj_format_t)session->operation.parameter[1]);

            if (session->response.code != PTP_RESP_OK)
                send_response(session, 0, 1);
        }
        break;

    case PTP_STATE_DATA_DONE:
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_MTP, ("%s: Invalid state (%d)\n", fname, session->state));
        break;
    }
}

/* section D.2.30 in the MTP document rev 0.96 (section B.2.2 in rev 0.83) */
/**
 * Function name:  mtp_get_obj_prop_desc_complete
 * Description: this function will be called when the application 
 *              is ready to send the property descriptor dataset requested 
 *              by the host. 
 *              the dataset will be packed and send via the ptp transport layer 
 *
 *              NOTE : there is a limitation on the object property descriptor 
 *              dataset size to be less then PTP_MAX_BLOCK_SIZE
 *
 * Parameters: 
 *     @sessionh: 
 *     @response: 
 *     @obj_prop_desc: 
 *
 * Return value: 
 * Scope: global
 **/
jresult_t mtp_get_obj_prop_desc_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, mtp_obj_prop_desc_t *obj_prop_desc)
{
    ptp_session_t *session = (ptp_session_t *)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    void *buf;
    DECLARE_FNAME("mtp_get_obj_prop_desc_complete");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        goto Exit;

    if (!ptpbuf->buff)
    {
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Exit;
    }

    buf = ptpbuf->buff + session->inst->transport_hdr_size;

    ptpbuf->data_len = pack_obj_prop_desc(&buf, obj_prop_desc);

    if (!ptpbuf->data_len)
    {
        DBG_E(DPROTO_MTP, ("%s: a packing error occurred\n",fname));
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Exit;
    }
#ifdef JDEBUG
    else if (ptpbuf->data_len + session->inst->transport_hdr_size > 
        ptpbuf->buff_size)
    {
        j_panic("buffer data length [%d] bigger then buffer size %d",
            ptpbuf->data_len, ptpbuf->buff_size);
    }
#endif

    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);
Exit:
    return send_response(session, 0, 1);
}


/* =================================
 *  Partial data transfer functions 
 * ================================= */

/**
 * Function name:  handle_get_obj_prop_val
 * Description: 
 *
 * this handle called when a GetObjectPropValue operation received from the host
 * in this function the iteration process initiated in the completion function
 * is on going until there is no data left to transfer (according to the
 * information from the application) 
 * the application callback called with the approximate amount of data elements
 * can be packed into the ptp buffer. (residues will be treated by the mtp layer
 * - I.E. Keep it so that data will be continuous).
 * in case the last chunk of data had exceeded the buffer size another empty
 * iteration is needed - in it there is no call to the application callback
 * and the residue is being treated.
 *
 *
 * Parameters: 
 *     @session: 
 *     Scope: local
 **/
static void handle_get_obj_prop_val(ptp_session_t *session)
{ 
    mtp_get_obj_prop_val cb = 
        ((mtp_callbacks_t *)PTP_VS_EXT_CBS(session))->get_obj_prop_val_h;
    mtp_ctx_t *ctx = PTP_VS_EXT_CTX(session);
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    DECLARE_FNAME("handle_get_obj_prop_val");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    switch (session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session, (void *)cb, 1))
        {
            ctx->prop_num_elems = calc_max_type(
                (mtp_obj_prop_t)session->operation.parameter[1],
                ptpbuf->buff_size - session->inst->transport_hdr_size);
            ctx->prop_elem_offset = 0;
            ctx->residue_count = 0;
            ctx->is_array = 0;

            session->response.code = cb(session,
                (ptp_obj_handle_t)session->operation.parameter[0],
                (mtp_obj_prop_t)session->operation.parameter[1],
                ctx->prop_elem_offset, ctx->prop_num_elems); 

            if (session->response.code != PTP_RESP_OK)
                send_response(session, 0, 1);
        }
        break;
        
    case PTP_STATE_SEND_DATA:
        /* if value is of an array type that requires the partial transfer
         * scheme, continue with the iterations */
        if (ctx->is_array)
        {
            DBG_V(DPROTO_MTP, ("%s: array transfer in progress\n",fname));
            ctx->prop_num_elems = calc_max_type(
                (mtp_obj_prop_t)session->operation.parameter[1],
                ptpbuf->buff_size - ctx->residue_count);

            session->response.code = cb(session,
                (ptp_obj_handle_t)session->operation.parameter[0],
                (mtp_obj_prop_t)session->operation.parameter[1],
                ctx->prop_elem_offset, ctx->prop_num_elems);

            if (session->response.code != PTP_RESP_OK)
                send_response(session, 0, 1);
        }
        else if (ctx->residue_count)/* array done but there are left overs */
        {
            ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
            mtp_ctx_t *ctx = PTP_VS_EXT_CTX(session);

            DBG_I(DPROTO_MTP, ("%s: array transfer residue\n",fname));
            j_memcpy(ptpbuf->buff, ctx->res_buf, ctx->residue_count);
            session->state = PTP_STATE_DATA_DONE;
            session->curr_app_offset = ctx->residue_count;
            session->app_done = 1;
            
            /* should it be here */
            session->transport_done = 1;

            ctx->residue_count = 0;
            schedule_advance(session);
        }
        else
            DBG_E(DPROTO_MTP, ("%s: wrong state for array transfer\n",fname));
        break;
    case PTP_STATE_DATA_DONE:
        if (ctx->is_array)
            DBG_E(DPROTO_MTP, ("%s: wrong state for array transfer\n",fname));
        else
            send_response(session, 0, 0);
        break;
    default:
        DBG_E(DPROTO_MTP, ("%s: Invalid state (%d)\n", fname, session->state));
        break;
    }

    return;
}

#define PTP_CALC_ARRAY_SIZE(elem_size, elem_count)      \
    (sizeof(juint32_t) + (elem_count) * (elem_size))

/* section D.2.31 in the MTP document rev 0.96 (section B.2.3 in rev 0.83) */
/**
 * Function name:  mtp_get_obj_prop_val_complete
 * Description: 
 *  
 * this function handle the completion of the GetObjectPropValue operation 
 * received from the host. 
 * if the value is larger then a ptp buffer size the data will be needed to 
 * chopped into slices (the handler function calculate the amount of data 
 * elements can be packed into a ptp buffer)
 * residues will be keep in the mtp layer for continuous flow of the data
 * over the USB layer.
 * property value data not exceeding the ptp buffer size will not initiate the
 * iteration sequence mechanism 
 * 
 *
 * Parameters: 
 *     @sessionh: 
 *     @response: 
 *     @value: 
 *
 * Return value: 
 * Scope: global
 **/
jresult_t mtp_get_obj_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_prop_value_t *value)
{
    ptp_session_t *session = (ptp_session_t *)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    mtp_ctx_t *ctx = PTP_VS_EXT_CTX(session);
    void *buf;
    juint32_t buf_size = 0;
    DECLARE_FNAME("mtp_get_obj_prop_val_complete");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    DBG_I(DPROTO_MTP, ("%s: value info : data_type[0x%x], type[%s], "
        "[max:%d, count:%d, offset:%d]\n",fname, value->data_type,
        (PTP_IS_TYPE_ARRAY(value->data_type) ? "atomic": "array"),
        value->total_elems, value->num_elems, value->elem_offset));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        goto Exit;

    if (!ptpbuf->buff)
    {
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Exit;
    }

    /* not in array iteration yet */
    if (!ctx->is_array)
    {
        buf_size = ptpbuf->buff_size - session->inst->transport_hdr_size;
        buf = ptpbuf->buff + session->inst->transport_hdr_size;
         
        /* We know we have the extra space after the normal buffer to take
         * the extra residual bytes (if any) */
        ptpbuf->data_len = pack_prop_value(value->data_type, &buf,
            buf_size+PTP_MAX_RESIDUE, value);
        if (!ptpbuf->data_len)
        {
            DBG_E(DPROTO_MTP, ("%s: a packing error occurred\n",fname));
            session->response.code = PTP_RESP_GENERAL_ERROR;
            goto Exit;
        }

        if (!PTP_IS_TYPE_ARRAY(value->data_type))
            return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

        ctx->is_array = 1;
        if (!value->data_type || !value->total_elems)
        {
            DBG_E(DPROTO_MTP, ("%s: invalid values: data_type[%d], "
                "total_elems[%d]\n",fname, value->data_type, 
                value->total_elems));
            session->response.code = PTP_RESP_GENERAL_ERROR;
            goto Exit;
        }

        /* If this is the first chunk of a multi-chunk transfer,
         * continue below */
        session->end_offset = PTP_CALC_ARRAY_SIZE(PTP_GET_TYPE_SIZE(
            value->data_type), value->total_elems);
    }
    else
    {
        /* else - we are in array iteration;
         * from this point an array value is compelled */
        if (!PTP_IS_TYPE_ARRAY(value->data_type))
        {
            DBG_E(DPROTO_MTP, ("%s: in array iteration and value is not "
                "array\n", fname));
            session->response.code = PTP_RESP_GENERAL_ERROR;
            goto Exit;
        }

        buf = ptpbuf->buff;
        buf_size = ptpbuf->buff_size;

        /* handle residue issues */
        if (ctx->residue_count)
        {
            DBG_I(DPROTO_MTP, ("%s: handling residue existence [%d]\n",
                fname, ctx->residue_count));
            j_memcpy(buf, ctx->res_buf, ctx->residue_count);
            buf = (juint8_t *)buf + ctx->residue_count;
        }
        else
            DBG_I(DPROTO_MTP, ("%s: no residue exists\n", fname));

        if (value->num_elems > ctx->prop_num_elems)
        {
            DBG_W(DPROTO_MTP, ("%s: the value of num_elems [%d] exceeds "
                "max_elems [%d]\n",
                fname, value->num_elems, ctx->prop_num_elems));
            return JENOMEM;
        }

        /* We know we have the extra space after the normal buffer to take
         * the extra residual bytes (if any) */
        ptpbuf->data_len = pack_prop_value(value->data_type, &buf,
            buf_size - ctx->residue_count + PTP_MAX_RESIDUE, value);

        if (!ptpbuf->data_len)
        {
            DBG_E(DPROTO_MTP, ("%s: a packing error occurred",fname));
            session->response.code = PTP_RESP_GENERAL_ERROR;
            goto Exit;
        }

#ifdef JDEBUG
        else if (ptpbuf->data_len >
            buf_size - ctx->residue_count + PTP_MAX_RESIDUE)
        {
            j_panic("buffer data length [%d] is bigger than the buffer size "
                "[%d] + residue", ptpbuf->data_len, ptpbuf->buff_size);
        }
#endif

        ctx->is_array = (value->num_elems +
            value->elem_offset != value->total_elems);
    }

    /* update data_len with size of residue (if any) */
    ptpbuf->data_len += ctx->residue_count;

    /* update offset for partial transfer */
    ctx->prop_elem_offset += value->num_elems;

    /* calculate new residue (if any) */
    ctx->res_buf = ptpbuf->residue_buf;
    ctx->residue_count = (ptpbuf->data_len > buf_size ? 
                          ptpbuf->data_len - buf_size : 0);
    ptpbuf->data_len -= ctx->residue_count;

    DBG_V(DPROTO_MTP, ("%s: data_len [%d], buf_size [%d], residue [%d]\n",
        fname,ptpbuf->data_len, buf_size, ctx->residue_count));

    /* prepare to send */
    if (session->state == PTP_STATE_COMMAND)
    {
        session->state = PTP_STATE_SEND_DATA;
        session->transport_done = 1;
    }
    session->app_done = 1;
    return schedule_advance(session);

Exit:
    return send_response(session, 0, 1);
}

/* array */
/* need to keep to ourselves the count that we send to the application and the
 * residue as it was received from the bus */
static void handle_set_obj_prop_val(ptp_session_t *session)
{ 
    mtp_ctx_t *ctx = PTP_VS_EXT_CTX(session);
    ptp_prop_value_t val;
    mtp_set_obj_prop_val cb = 
        ((mtp_callbacks_t *)PTP_VS_EXT_CBS(session))->set_obj_prop_val_h;
    DECLARE_FNAME("handle_set_obj_prop_val");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    switch (session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session, (void *)cb, 1))
        {
            ctx->prop_total_elems = 0;
            ctx->prop_num_elems = 0;
            ctx->prop_elem_offset = 0;
            start_read_data(session);
        }
        break;

    case PTP_STATE_READ_DATA:
        {
            ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
            juint32_t hdr_size = 
                (ctx->is_array ? 0 : session->inst->transport_hdr_size);
            void *buf = ptpbuf->buff + hdr_size;
            juint32_t count = 0;
#ifdef JDEBUG
            static juint_t iter_count = 0;
#endif
            ctx->res_buf = (juint8_t*)ctx->unpack_buf + ptpbuf->buff_size;
            val.total_elems = ctx->prop_total_elems;
            val.num_elems = ctx->prop_num_elems;
            val.elem_offset = ctx->prop_elem_offset;

            /* unpack the buffer before handing it over to the application;
             * get ptp_type according to prop code parameter*/
            count = unpack_prop_value(get_obj_prop_type(
                (mtp_obj_prop_t)session->operation.parameter[1]), buf,
                ptpbuf->data_len, &val, ctx->unpack_buf,
                ctx->res_buf, &ctx->residue_count);

            if (!count)
            {
                DBG_E(DPROTO_MTP, ("%s: an unpacking error occurred",fname));
                session->response.code = PTP_RESP_GENERAL_ERROR;
                break;
            }

            if (PTP_IS_TYPE_ARRAY(val.data_type))
            {
                DBG_I(DPROTO_MTP, ("%s: in array iteration[%d]\n", fname,
                    iter_count++));

                if (!ctx->prop_total_elems) /* first entrance */
                {
                    ctx->prop_total_elems = val.total_elems;
                }
                else /* iterations */
                {
                    ctx->prop_elem_offset += ctx->prop_num_elems;
                }

                ctx->prop_num_elems = val.num_elems = count;
                val.elem_offset = ctx->prop_elem_offset;

                ctx->is_array = 1;
                                   
                /* last chunk of partial transaction */
                if (ctx->prop_num_elems + ctx->prop_elem_offset == 
                    ctx->prop_total_elems)
                {
                    ctx->is_array = 0;
#ifdef JDEBUG
                    iter_count = 0;
#endif
                }
                else if (ctx->prop_num_elems + ctx->prop_elem_offset >
                    ctx->prop_total_elems)
                {
                    DBG_E(DPROTO_MTP, ("%s: wrong values - prop_total_elems "
                        "[%d], prop_num_elems [%d], prop_elem_offset [%d]\n"
                        , fname, ctx->prop_total_elems, ctx->prop_num_elems,
                        ctx->prop_elem_offset));
                    session->response.code = PTP_RESP_GENERAL_ERROR;
                    break;
                }
            }
            else if (!PTP_IS_TYPE_ARRAY(val.data_type))
                DBG_I(DPROTO_MTP, ("%s: set atomic value\n", fname));

            /* call the application */
            session->response.code = cb(session,
                    (ptp_obj_handle_t)session->operation.parameter[0],
                    (mtp_obj_prop_t)session->operation.parameter[1], &val);

            if (session->response.code != PTP_RESP_OK)
                send_response(session, 0, 1);
        }
        break;
    case PTP_STATE_DATA_DONE:
        DBG_I(DPROTO_MTP, ("%s: -- reached set data done -- \n", fname));

        ctx->prop_total_elems = 0;
        ctx->prop_num_elems = 0;
        ctx->prop_elem_offset = 0;
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_MTP, ("%s: Invalid state (%d)\n", fname, session->state));
        break;
    }
}

/* section D.2.32 in the MTP document rev 0.96 (section B.2.4 in rev 0.83) */
jresult_t mtp_set_obj_prop_val_complete(ptp_sessionh_t sessionh,
    ptp_response_t response)
{
    ptp_session_t *session = (ptp_session_t *)sessionh;
    mtp_ctx_t *ctx = PTP_VS_EXT_CTX(session);

    DECLARE_FNAME("mtp_set_obj_prop_val_complete");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    DBG_I(DPROTO_MTP, ("%s: value info(from ctx) : "
        "max: %d, count: %d, offset: %d\n",fname, ctx->prop_total_elems, 
        ctx->prop_num_elems, ctx->prop_elem_offset));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        return send_response(session, 0, 1);

    /* not in array value */
    if (!ctx->is_array)
    {
        DBG_I(DPROTO_MTP, ("%s: set property value ended \n",fname));
        session->app_done = 1;
        return send_response(session, 0, 0);
    }
    else
    {
        DBG_V(DPROTO_MTP, ("%s: array - continue to next iteration\n", fname));
        session->app_done = 1;
        return schedule_advance(session);
    }
}

static void handle_get_obj_ref(ptp_session_t *session)
{ 
    mtp_get_obj_refs cb =
        ((mtp_callbacks_t *)PTP_VS_EXT_CBS(session))->get_obj_refs_h;

    DECLARE_FNAME("handle_get_obj_ref");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));
    DBG_X(DPROTO_MTP, ("%s: state [0x%x]\n",fname, session->state));

    switch (session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session, (void *)cb, 1))
        {
            session->response.code = cb(session,
                (ptp_obj_format_t)session->operation.parameter[0]);

            if (session->response.code != PTP_RESP_OK)
                send_response(session, 0, 0);
        }
        break;
    case PTP_STATE_DATA_DONE:
        DBG_I(DPROTO_MTP, ("%s: -- get object references ended -- \n",fname));
        send_response(session, 0, 0);
        break;
    default:
        DBG_E(DPROTO_MTP, ("%s: Invalid state (%d)\n", fname, session->state));
        break;
    }
}

/* section D.2.33 in the MTP document rev 0.96 (section B.2.5 in rev 0.83) */
jresult_t mtp_get_obj_refs_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t *obj_handles, juint32_t size)
{
    ptp_session_t *session = (ptp_session_t *)sessionh;
    ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
    juint32_t handles_size = sizeof(juint32_t) + size * sizeof(ptp_obj_handle_t);
    void *buf;
    int i = 0;
    DECLARE_FNAME("mtp_get_obj_refs_complete");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        goto Exit;

    if (!ptpbuf->buff)
    {
        session->response.code = PTP_RESP_GENERAL_ERROR;
        goto Exit;
    }

    if ( handles_size > (ptpbuf->buff_size - session->inst->transport_hdr_size))
    {
        DBG_W(DPROTO_MTP, ("%s: data to send exceeds buffer size\n", fname));
        return JENOMEM;
    }

    buf = ptpbuf->buff + session->inst->transport_hdr_size;

    pack_u32(&buf, size);
    for (i = 0 ; i != size; i++)
        pack_u32(&buf, (juint32_t)obj_handles[i]);

    ptpbuf->data_len = handles_size;
    return start_send_data(session, ptpbuf->data_len, ptpbuf->data_len);

Exit:
    return send_response(session, 0, 0);
}

       
static void handle_set_obj_ref(ptp_session_t *session)
{
    mtp_ctx_t *ctx = PTP_VS_EXT_CTX(session);
    ptp_obj_handle_t *handles = (ptp_obj_handle_t *)ctx->unpack_buf;
    mtp_set_obj_refs cb = 
        ((mtp_callbacks_t *)PTP_VS_EXT_CBS(session))->set_obj_refs_h;
    DECLARE_FNAME("handle_set_obj_ref");

    DBG_X(DPROTO_MTP, ("%s: entered \n",fname));
    DBG_X(DPROTO_MTP, ("%s: state [0x%x]\n",fname, session->state));

    switch (session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session, (void *)cb, 1))
        {
            start_read_data(session);
        }
        break;
    case PTP_STATE_READ_DATA:
        {
            ptp_buffer_t *ptpbuf = &session->data[session->app_buf_idx];
            void *buf = ptpbuf->buff + session->inst->transport_hdr_size;
            juint32_t count, size;

            size = unpack_u32(&buf);
            DBG_V(DPROTO_MTP, ("%s: array size[%d] to unpack\n",fname, size));

            for (count = 0 ; count < size ; count ++)
                handles[count] = unpack_u32(&buf);
         
            session->response.code = cb(session, 
                    (ptp_obj_handle_t)session->operation.parameter[0],
                    handles, size);

            if (session->response.code != PTP_RESP_OK)
            {
                DBG_W(DPROTO_MTP, ("%s: response [0x%x] \n", fname, 
                    session->response));
                send_response(session, 0, 1);
            }
        }
        break;
    case PTP_STATE_DATA_DONE:
        DBG_I(DPROTO_MTP, ("%s: -- reached set obj refs done -- \n", fname));
        send_response(session, 0, 0);
        break;

    default:
        DBG_E(DPROTO_MTP, ("%s: Invalid state (%d)\n", fname, session->state));
        break;
    }
}

jresult_t mtp_set_obj_refs_complete(ptp_sessionh_t sessionh,
    ptp_response_t response)
{
    ptp_session_t *session = (ptp_session_t *)sessionh;

    DECLARE_FNAME("mtp_set_obj_refs_complete");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        DBG_E(DPROTO_MTP, ("%s: response error : [0x%x]\n", fname, response));

    session->app_done = 1;
    return schedule_advance(session);
}

 
static void handle_skip(ptp_session_t *session)
{
    mtp_skip cb = ((mtp_callbacks_t *)PTP_VS_EXT_CBS(session))->skip_h;
    DECLARE_FNAME("handle_skip");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    switch (session->state)
    {
    case PTP_STATE_COMMAND:
        if (verify_command_supported(session, (void *)cb, 1))
        {
            session->response.code = cb(session,
                    (ptp_obj_format_t)session->operation.parameter[0]);

            if (session->response.code != PTP_RESP_OK)
                send_response(session, 0, 0);
        }
        break;

    default:
        DBG_E(DPROTO_MTP, ("%s: Invalid state (%d)\n", fname, session->state));
        break;
    }

}

jresult_t mtp_skip_complete(ptp_sessionh_t sessionh,
    ptp_response_t response)
{
    ptp_session_t *session = (ptp_session_t *)sessionh;

    DECLARE_FNAME("mtp_skip_complete");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    session->response.code = response;
    if (response != PTP_RESP_OK)
        DBG_W(DPROTO_MTP, ("%s: skip returned with [0x%x]\n", fname, response));

    return send_response(session, 0, 0);
}

#ifdef CONFIG_PROTO_PTP_MTP_ENHANCED
static void handle_get_obj_prop_list(ptp_session_t *session)
{ 
    DECLARE_FNAME("handle_get_obj_prop_list");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));
}

/* section E.2.1 in the MTP document rev 0.96 (section 14.7.2.1 in rev 0.83) */
jresult_t mtp_get_obj_prop_list_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t total_num_elems,
    mtp_obj_prop_elem_t *elem, juint32_t *num_elems /* in elements to send 
                                                      out elements sent */)
{
    return -1;
}
 

static void handle_set_obj_prop_list(ptp_session_t *session)
{ 
    DECLARE_FNAME("handle_set_obj_prop_list");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));
}

/* section E.2.2 in the MTP document rev 0.96 (section 14.7.2.2 in rev 0.83) */
jresult_t mtp_set_obj_prop_list_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t prop_idx_failed/* index of first 
                                                        failed property */)
{
    return -1;
}
 

static void handle_get_interdep_prop_desc(ptp_session_t *session)
{ 
    DECLARE_FNAME("handle_get_interdep_prop_desc");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));
}
   
/* section E.2.3 in the MTP document rev 0.96 (section 14.7.2.3 in rev 0.83) */
jresult_t mtp_get_interdep_prop_desc_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint16_t interdeps_num, 
    interdep_obj_prop_t *interdep_obj_prop_list, juint32_t *arr_idx
                                                    /* IN:  ext idx 
                                                     * OUT: hw ext idx reached
                                                     *      lw int idx reached
                                                     **/)
{
    return -1;
}


static void handle_send_obj_prop_list(ptp_session_t *session)
{ 
    DECLARE_FNAME("handle_send_obj_prop_list");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));
}
   
/* section E.2.4 in the MTP document rev 0.96 (section 14.7.2.4 in rev 0.83) */
jresult_t mtp_send_obj_prop_list_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t storage,
    ptp_obj_handle_t parent_obj_handle, ptp_obj_handle_t reserved_obj_handle,
    juint32_t prop_idx_failed)
{
    return -1;
}

#endif

/* MTP dispatch function - will be called from the PTP dispatch function */
jresult_t mtp_dispatch(ptp_session_t *session)
{
    DECLARE_FNAME("mtp_dispatch");

    DBG_X(DPROTO_MTP, ("%s: entered\n", fname));

    switch(session->operation.code)
    {
    case MTP_OP_GET_OBJECT_PROPS_SUPPORTED:
        handle_get_obj_props_supp(session);
        break;

    case MTP_OP_GET_OBJECT_PROP_DESC:
        handle_get_obj_prop_desc(session);
        break;

    case MTP_OP_GET_OBJECT_PROP_VALUE:
        handle_get_obj_prop_val(session);
        break;

    case MTP_OP_SET_OBJECT_PROP_VALUE:
        handle_set_obj_prop_val(session);
        break;

    case MTP_OP_GET_OBJECT_REFERENCES:
        handle_get_obj_ref(session);
        break;

    case MTP_OP_SET_OBJECT_REFERENCES:
        handle_set_obj_ref(session);
        break;
    case MTP_OP_SKIP:
        handle_skip(session);
        break;
#ifdef CONFIG_PROTO_PTP_MTP_ENHANCED
    case MTP_OP_GET_OBJECT_PROP_LIST:
        handle_get_obj_prop_list(session);
        break;

    case MTP_OP_SET_OBJECT_PROP_LIST:
        handle_set_obj_prop_list(session);
        break;

    case MTP_OP_GET_INTERDEP_PROP_DESC:
        handle_get_interdep_prop_desc(session);
        break;

    case MTP_OP_SEND_OBJECT_PROP_LIST:
        handle_send_obj_prop_list(session);
        break;
#endif
    case MTP_OP_UNDEFINED:
    default:
        DBG_E(DPROTO_MTP, ("%s: Operation code unsupported (%X)\n", fname,
            session->operation.code));
        session->response.code = PTP_RESP_OPERATION_NOT_SUPPORTED;
        send_response(session, 0, 0);
        break;
    }

    return 0;
}

jresult_t mtp_close_session(ptp_session_t *session)
{
    mtp_ctx_t *ctx = (mtp_ctx_t *)PTP_VS_EXT_CTX(session);
    DECLARE_FNAME("mtp_close_session");

    DBG_X(DPROTO_MTP, ("%s: entered\n", fname));

    if (ctx->unpack_buf)
        jfree(ctx->unpack_buf);
    if (ctx)
        jfree(ctx);
    return 0;
}

jresult_t mtp_open_session(ptp_session_t *session)
{
    mtp_ctx_t *ctx  = NULL;
    DECLARE_FNAME("mtp_open_session");

    DBG_X(DPROTO_MTP, ("%s: entered\n", fname));

    ctx = jmalloc(sizeof(mtp_ctx_t), M_ZERO);
    if (!ctx)
        goto Error;
    ctx->unpack_buf_size = PTP_MAX_BLOCK_SIZE + PTP_MAX_RESIDUE;
    ctx->unpack_buf = jmalloc(ctx->unpack_buf_size, 0);
    if (!ctx->unpack_buf)
        goto Error;
    PTP_VS_EXT_CTX(session) = ctx;
    return 0;
Error:
    mtp_close_session(session);
    return JENOMEM;
}

jresult_t mtp_delete_cbs(ptp_vs_ext_cbs_t *cbs)
{
    DECLARE_FNAME("mtp_delete_cbs");

    DBG_X(DPROTO_MTP, ("%s: entered\n", fname));
            
    if (cbs)
        jfree(cbs);
    else
        DBG_W(DPROTO_MTP, ("%s: no mtp callbakcs found\n", fname));
    return 0;
}

wchar_t vs_ext_desc[] = { 'J','u','n','g','o',' ','M','T','P',0 };
jresult_t mtp_init(ptp_instanceh_t insth, mtp_callbacks_t *cbs)
{
    jresult_t rc = 0;
    ptp_vs_ext_params_t mtp_params;
    mtp_callbacks_t *mtp_cbs;
    DECLARE_FNAME("mtp_init");

    DBG_X(DPROTO_MTP, ("%s: entered\n",fname));

    mtp_cbs = jmalloc(sizeof(mtp_callbacks_t), M_ZERO);
    if (!mtp_cbs)
    {
        rc = JEINVAL;
        goto Error;
    }

    j_memcpy(mtp_cbs, cbs, sizeof(mtp_callbacks_t));
    mtp_params.vendor_ext_id = 6;
    mtp_params.vendor_ext_ver = 100;
    mtp_params.vendor_ext_desc = vs_ext_desc;
    mtp_params.dispatch_h = mtp_dispatch;
    mtp_params.open_session_h = mtp_open_session;
    mtp_params.close_session_h = mtp_close_session;
    mtp_params.del_cbs_h = mtp_delete_cbs;

    rc = ptp_register_vs_ext(insth, &mtp_params, mtp_cbs);

    return 0;
Error:
    return rc;
}



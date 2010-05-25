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
#include <jos.h>
#include <xml.h>
#include <uw_device_sicd.h>
#include <ptp_hook.h>

#define MAX_XML_BODY_SIZE 3800
#define MAX_XML_REQ_SIZE 4000
#define MAX_XML_PARAM_SIZE 200

/******************************************************
 *       Data types
 ******************************************************/

typedef enum dps_msg_type
{
    DPS_REQUEST,
    DPS_RESPONSE
} dps_msg_type_t;

typedef struct dps_msg
{
    dps_msg_type_t type;
    ptp_obj_info_t info;
    ptp_obj_handle_t id;
    char *xml;
    juint32_t xml_size;
} dps_msg_t;

typedef enum dps_transaction_state
{
    DPS_TSTATE_IDLE,

    /* DSC to Printer */
    DPS_TSTATE_DREQUEST,
    DPS_TSTATE_DREQUEST_DATA,
    DPS_TSTATE_HRESPONSE,
    DPS_TSTATE_HRESPONSE_DATA,

    /* Printer to DSC */
    DPS_TSTATE_HREQUEST,
    DPS_TSTATE_HREQUEST_DATA,
    DPS_TSTATE_DRESPONSE,
    DPS_TSTATE_DRESPONSE_DATA
} dps_transaction_state_t;

typedef enum dps_state
{
    DPS_STATE_DISCONN,
    DPS_STATE_DISCOVERY,
    DPS_STATE_IDLE,         /* i.e. printer idle and ready for jobs */
    DPS_STATE_CONFIGURE,
    DPS_STATE_GET_CAPABILITY,
    DPS_STATE_GET_JOB_STAT,
    DPS_STATE_GET_DEV_STAT,
    DPS_STATE_START_JOB,
    DPS_STATE_ABORT_JOB,
    DPS_STATE_CONTINUE_JOB,
    DPS_STATE_NOTIFY_JOB_STAT,
    DPS_STATE_NOTIFY_DEV_STAT,
    DPS_STATE_GET_FILE_ID
} dps_state_t;

typedef struct dps_ctx
{
    ptp_sessionh_t ptph;
    ptp_responder_cbs ptp_app_cbs;
    ptp_responder_comp_cbs ptp_comp_cbs;

    dps_transaction_state_t trans_state;
    dps_state_t state;
        
    /* When state is GET_CAPABILITY, this holds the capability requested */
    dps_capability_t cap_requested;

    /* Save device and job status so we can hand it to the application
     * when the transaction is finished */
    dps_param_t hreq_result;
    dps_device_status_t dev_status;
    dps_job_status_t job_status;

    /* DPS application context and callbacks */
    dps_appctx_t appctx;
    dps_callbacks_t cbs;

    /* The only way of telling that an object is for us, is by flagging this
     * during SendObjInfo... */
    jbool_t is_send_obj_for_dps;

    /* When a DPS transaction collision occurs, we need to do some special
     * handling */
    jbool_t collision;

    /* We raise this flag when a hook handler needs the corresponding
     * completion hook to work (since it can be called asynchronously) */
    jbool_t do_dps_discovery;
    
    /* DPS requests and responses */
    ptp_storage_id_t store_id;
    dps_msg_t dsc_msg;
    dps_msg_t printer_msg;

    wchar_t *hdiscvry;
    wchar_t *hrequest;
    wchar_t *hresponse;

    char xml_body[MAX_XML_BODY_SIZE];
    char xml_req[MAX_XML_REQ_SIZE];
} dps_ctx_t;

/******************************************************
 *       Prototypes
 ******************************************************/
jresult_t dps_send_request(dps_handle_t handle, char *xml_request,
    juint32_t size);

static void handle_hrequest(dps_ctx_t *ctx);

jresult_t dps_send_response(dps_handle_t handle, char *xml_response,
    juint32_t size);

static void dps_transaction_complete(dps_ctx_t *ctx);

static void ptp_transaction_complete(ptp_sessionh_t sessionh,
    ptp_io_status_t result, ptp_operation_t operation,
    ptp_response_t response, juint32_t obj_size, juint32_t bytes_transferred);

void dps_uninit(ptp_instanceh_t insth, ptp_appctx_t global_ctx);

/******************************************************
 *       Utility functions
 ******************************************************/

/* DPS engine reserves handles ranging from 0xFFFFFFF0 - 0xFFFFFFFA
 * for virtual file representation */
ptp_obj_handle_t get_virtual_handle(void)
{
    static juint32_t h = 0xFFFFFFF0;
    
    if (h > 0xFFFFFFFA)
        h = 0xFFFFFFF0;

    return h++;
}

#define FREE_STRING(str)        \
    if (str)    {               \
        ptp_string_free(str);   \
        str = NULL; }
static void clear_object_info(ptp_obj_info_t *objinfo)
{
    FREE_STRING(objinfo->filename);
    FREE_STRING(objinfo->capture_date);
    FREE_STRING(objinfo->modification_date);
    FREE_STRING(objinfo->keywords);

    j_memset(objinfo, 0, sizeof(ptp_obj_info_t));
}
#undef FREE_STRING

static char* alloc_xml_buffer(dps_msg_t *msg, juint32_t size)
{
    if ((size > msg->xml_size) && msg->xml)
    {
        jfree(msg->xml);
        msg->xml = NULL;
    }
        
    if (!msg->xml)
    {
        msg->xml = jmalloc(size, M_ZERO);
        msg->xml_size = size;
    }

    return msg->xml;
}

/******************************************************
 *      XML generation functions
 ******************************************************/

char *xml_input_template =
"<?xml version=\"1.0\"?>"
"<dps xmlns=\"http://www.cipa.jp/dps/schema/\">"
"<input>"
"%s"            /* Request body */
"</input>"
"</dps>";

char *xml_output_template =
"<?xml version=\"1.0\"?>"
"<dps xmlns=\"http://www.cipa.jp/dps/schema/\">"
"<output>"
"<result>%.8X</result>"
"%s"            /* Request body */
"</output>"
"</dps>";

static char* xml_build_request(char *xml_buff, juint32_t size, char *body)
{
    j_snprintf(xml_buff, size, xml_input_template, body);
    xml_buff[size-1] = 0;
    return xml_buff;
}

static char* xml_build_response(char *xml_buff, juint32_t size,
    dps_param_t result, char *body)
{
    j_snprintf(xml_buff, size, xml_output_template, result, body);
    xml_buff[size-1] = 0;
    return xml_buff;
}

static char* xml_build_param_with_attrib(char *buff, juint32_t size,
    char *param_name, char *param_value, char *attrib_name,
    char *attrib_value)
{
    char attrib_buff[30];

    *attrib_buff = 0;
    if (attrib_name && *attrib_name && attrib_value)
    {
        j_snprintf(attrib_buff, sizeof(attrib_buff), " %s=\"%s\"",
            attrib_name, (attrib_value?attrib_value:""));
    }

    if (param_value && *param_value)
    {
        j_snprintf(buff, size, "<%s%s>%s</%s>", param_name, attrib_buff,
            param_value, param_name);
    }
    else
    {
        j_snprintf(buff, size, "<%s%s/>", param_name, attrib_buff);
    }
    
    buff[size-1] = 0;
    return buff;
}

static char* xml_body_add_param_with_attrib(char *body_buff, juint32_t size,
    char *param_name, char *param_value, char *attrib_name,
    char *attrib_value)
{
    char param_buff[MAX_XML_PARAM_SIZE];
    j_snprintf(body_buff, size, "%s%s", body_buff,
        xml_build_param_with_attrib(param_buff, sizeof(param_buff),
        param_name, param_value, attrib_name, attrib_value));
    body_buff[size-1] = 0;
    return body_buff;
}

static char* xml_body_add_param(char *body_buff, juint32_t size,
    char *param_name, char *param_value)
{
    return xml_body_add_param_with_attrib(body_buff, size, param_name,
        param_value, NULL, NULL);
}

enum xml_section_type
{
    XML_SECT_OPEN, XML_SECT_CLOSE, XML_SECT_EMPTY
};

static char* xml_body_section(char *body_buff, juint32_t size,
    char *section_name, enum xml_section_type type)
{
    
    j_snprintf(body_buff, size, "%s<%s%s>", body_buff,
        (type==XML_SECT_CLOSE) ? "/" : 
        (type==XML_SECT_EMPTY) ? section_name : "",
        (type==XML_SECT_EMPTY) ? "/" : section_name);
    body_buff[size-1] = 0;
    return body_buff;
}

/******************************************************
 *      XML parsing functions
 ******************************************************/

typedef juint32_t (*xml_convert_cb)(char *xml_data, void *data_out,
    juint32_t max_count);

typedef struct
{
    char *name;
    juint8_t level;
    juint8_t mandatory;
    xml_convert_cb convert_h;
    void *data;
    juint32_t max_count; /* size of buffer pointed to by *data, in elements! */
    juint32_t actual_count; /* number of elements actually read */
} xml_form_t;

static void init_form(xml_form_t *form)
{
    jint_t i;
    for(i=0; form[i].name; i++)
        form[i].actual_count = 0;
}

static juint32_t convert_str(char *xml_data, void *data_out, juint32_t max_count)
{
    char *str = (char*)data_out;
    *str = 0;
    j_strlcpy(str, xml_data, max_count);
    str[max_count-1] = 0;
    return j_strlen(str);
}

static juint32_t convert_hex(char *xml_data, void *data_out,
    juint32_t max_count)
{
    juint32_t *hex = (juint32_t*)data_out;
    char *in_data = xml_data;
    juint32_t count = 0;
    juint32_t value;

    while (*in_data && count < max_count)
    {
        /* eat up any white space */
        while (j_is_space(*in_data))
            in_data++;
        
        value = j_strtonum(in_data, 16, &in_data);
        if (value == NUMERIC_VALUE_INVALID)
            break;
        
        *hex = value;
        hex++;
        count++;
    }

    return count;
}

static juint32_t convert_dec(char *xml_data, void *data_out,
    juint32_t max_count)
{
    juint16_t *dec = (juint16_t*)data_out;
    char *in_data = xml_data;
    juint32_t count = 0;
    juint32_t value;

    while (*in_data && count < max_count)
    {
        /* eat up any white space */
        while (j_is_space(*in_data))
            in_data++;
        
        value = j_strtonum(in_data, 10, &in_data);
        if (value == NUMERIC_VALUE_INVALID)
            break;
        
        *dec = (juint16_t)value;
        dec++;
        count++;
    }

    return count;
}

static juint32_t convert_ver(char *xml_data, void *data_out,
    juint32_t max_count)
{
    dps_version_t *ver = (dps_version_t*)data_out;
    juint32_t minor, major;
    char *in_data = xml_data;
    juint32_t count = 0;

    while (*in_data && count < max_count)
    {
        /* eat up any white space */
        while (j_is_space(*in_data))
            in_data++;
        
        major = j_strtonum(in_data, 10, &in_data);
        if (major == NUMERIC_VALUE_INVALID)
            break;
        
        if (*in_data == '.')
            in_data++;
        else
            break;

        minor = j_strtonum(in_data, 10, &in_data);
        if (minor == NUMERIC_VALUE_INVALID)
            break;
        
        ver->major = (juint16_t)major;
        ver->minor = (juint16_t)minor;
        count++;
        ver++;
    }

    return count;
}

static juint32_t convert_progress(char *xml_data, void *data_out,
    juint32_t max_count)
{
    juint16_t *progress = (juint16_t*)data_out;
    char *in_data = xml_data;
    juint32_t done, total;

    done = j_strtonum(in_data, 10, &in_data);
    if (done == NUMERIC_VALUE_INVALID)
        return 0;

    if (*in_data == '/')
        in_data++;
    else
        return 0;

    total = j_strtonum(in_data, 10, &in_data);
    if (total == NUMERIC_VALUE_INVALID)
        return 0;

    progress[0] = (juint16_t)done;
    progress[1] = (juint16_t)total;
    return 1;
}

static xml_elm_h xml_parse_level(xml_elm_h first, xml_form_t *form,
    juint32_t elm_idx, juint32_t instance, juint32_t max_elems)
{
    juint32_t i = 0;
    juint32_t inst = 0;
    xml_elm_h curr_elm = first;
    char *name;
    char *data;
    DECLARE_FNAME("xml_parse_level");

    if (!first)
    {
        DBG_I(DPROTO_DPS, ("%s: nothing to do. parent=NULL\n", fname));
        goto Exit;
    }

    /* Search for the element */
    for (i=0, curr_elm = first;
        curr_elm && i<max_elems;
        i++, curr_elm = xml_get_next(curr_elm))
    {
        name = xml_get_name(curr_elm);
        if (!name)
        {
            DBG_E(DPROTO_DPS, ("%s: empty name\n", fname));
            goto Exit;
        }

        /* Is this the element we're looking for? */
        if (!j_strcmp(form[elm_idx].name, name))
        {
            if (++inst == instance)
                break;
        }
    }

    /* If it does not exist, leave */
    if (!curr_elm || i>=max_elems)
        goto Exit;

    /* Is there a conversion to be done? */
    if (!form[elm_idx].convert_h || !form[elm_idx].data)
        return curr_elm;

    /* Get XML data for conversion */
    data = xml_get_cdata(curr_elm);
    if (!data)
    {
        DBG_E(DPROTO_DPS, ("%s: no data\n", fname));
        goto Exit;
    }

    /* Convert it */
    form[elm_idx].actual_count = form[elm_idx].convert_h(data,
        form[elm_idx].data, form[elm_idx].max_count);

    if (form[elm_idx].actual_count)
        return curr_elm;

Exit:
    return NULL;
}

static jbool_t xml_parse_hierarchy(char *xml_msg, xml_form_t *form)
{
    xml_elm_h root_elm;
    xml_elm_h curr_elm;
    xml_elm_h first_elm;
    juint32_t curr_idx = 0;
    juint32_t curr_level = 0;
    jbool_t rc = 0;
    DECLARE_FNAME("xml_parse_hierarchy");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    first_elm = curr_elm = root_elm = xml_parse(xml_msg, j_strlen(xml_msg));
    if (!curr_elm)
    {
        DBG_E(DPROTO_DPS, ("%s: failed to parse XML\n", fname));
        goto Exit;
    }

    while (first_elm && form[curr_idx].name)
    {
        /* Should we move down a level? */
        if (form[curr_idx].level > curr_level)
        {
            if (!curr_elm)
            {
                DBG_E(DPROTO_DPS, ("%s: critical element missing '%s'\n",
                    fname, form[curr_idx?curr_idx-1:0].name));
                goto Exit;
            }
            
            first_elm = xml_get_child_first(curr_elm);
            if (!first_elm)
            {
                #ifdef JDEBUG
                char *name = xml_get_name(curr_elm);
                if (!name)
                {
                    DBG_E(DPROTO_DPS, ("%s: xml_get_name() returned NULL\n",
                        fname));
                    goto Exit;
                }
                else
                {
                    DBG_E(DPROTO_DPS, ("%s: could not find child under"
                        " '%s'\n", fname, name));
                }
                #endif

                goto Exit;
            }

            curr_level = form[curr_idx].level;
        }

        /* Scan the current level for our current element */
        curr_elm = xml_parse_level(first_elm, form, curr_idx, 1, 50);
        if (!curr_elm)
        {
            if (form[curr_idx].mandatory)
            {
                DBG_E(DPROTO_DPS, ("%s: mandatory element not found (%s)\n",
                    fname, form[curr_idx].name));
                goto Exit;
            }
        }

        curr_idx++;
    }

    rc = 1;
    
Exit:
    if (root_elm)
        xml_free(root_elm);

    return rc;
}

/******************************************************
 *       PTP handler hooks
 ******************************************************/

static ptp_response_t hook_open_session(ptp_sessionh_t sessionh,
    juint32_t session_id, ptp_appctx_t global_ctx, ptp_appctx_t *sess_ctx)
{
    dps_ctx_t *ctx = (dps_ctx_t*)ptp_get_global_hookctx(sessionh);
    DECLARE_FNAME("hook_open_session");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    /* Initialize our state */
    /* Since we are a single session device, we store our global context
       on the session handle */
    PTP_SET_HOOKCTX(sessionh, ctx);
    
    ctx->state = DPS_STATE_DISCOVERY;
    ctx->ptph = sessionh;

    ctx->hdiscvry = ptp_string_atow("HDISCVRY.DPS");
    ctx->hrequest = ptp_string_atow("HREQUEST.DPS");
    ctx->hresponse = ptp_string_atow("HRSPONSE.DPS");

    /* Prepare a handle for DDISCVRY.DPS */
    ctx->dsc_msg.id = get_virtual_handle();

    if (ctx->ptp_app_cbs.open_session_h)
    {
        return ctx->ptp_app_cbs.open_session_h(sessionh, session_id,
            global_ctx, sess_ctx);
    }

    return PTP_RESP_OK;
}

static ptp_response_t hook_close_session(ptp_sessionh_t sessionh,
    juint32_t session_id)
{
    dps_ctx_t *ctx = NULL;
    DECLARE_FNAME("hook_close_session");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));
    
    ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);

    if (ctx->cbs.printer_disconnect_h)
        ctx->cbs.printer_disconnect_h(ctx->appctx);

    ctx->state = DPS_STATE_DISCONN;
    ctx->trans_state = DPS_TSTATE_IDLE;
    ctx->ptph = NULL;
    ptp_string_free(ctx->hdiscvry);
    ptp_string_free(ctx->hrequest);
    ptp_string_free(ctx->hresponse);
    
    clear_object_info(&ctx->dsc_msg.info);
    clear_object_info(&ctx->printer_msg.info);
    
    if (ctx->dsc_msg.xml)
    {
        jfree(ctx->dsc_msg.xml);
        ctx->dsc_msg.xml = NULL;
    }
    
    if (ctx->printer_msg.xml)
    {
        jfree(ctx->printer_msg.xml);
        ctx->printer_msg.xml = NULL;
    }
    
    if (ctx->ptp_app_cbs.close_session_h)
    {
        return ctx->ptp_app_cbs.close_session_h(sessionh, session_id);
    }

    return PTP_RESP_OK;
}    

static ptp_response_t hook_get_storage_ids(ptp_sessionh_t sessionh)
{
    ptp_storage_id_t store_ids[1];
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    DECLARE_FNAME("hook_get__storage_ids");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    /* If a PTP application has a handler, call it.
     * We'll grab the reply in the completion hook */
    if (ctx->ptp_app_cbs.get_storage_ids_h)
        return ctx->ptp_app_cbs.get_storage_ids_h(sessionh);

    /* Application does not handle this request - do our thing */
    store_ids[0] = PTP_MAKE_STORAGE_ID(0,0);
    ctx->ptp_comp_cbs.get_storage_ids_comp(sessionh, PTP_RESP_OK, store_ids,
        1);

    return PTP_RESP_OK;
}

static ptp_response_t hook_get_num_objects(ptp_sessionh_t sessionh,
    ptp_storage_id_t store, ptp_obj_format_t format,
    ptp_obj_handle_t parent_folder)
{
    juint32_t num_objs = 0;
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    DECLARE_FNAME("hook_get_num_objects");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    if ((ctx->state == DPS_STATE_DISCOVERY) && (store == PTP_STORAGE_ID_ALL)
        && (format == PTP_OBJ_FORMAT_SCRIPT))
    {
        ctx->do_dps_discovery = 1;
    }
    
    /* If a PTP application has a handler, call it.
     * We'll grab the reply in the completion hook */
    if (ctx->ptp_app_cbs.get_num_objs_h)
    {
        return ctx->ptp_app_cbs.get_num_objs_h(sessionh, store, format,
            parent_folder);
    }

    /* PTP App doesn't have one - use our own */
    if (ctx->do_dps_discovery)
    {
        num_objs = 1;
        ctx->do_dps_discovery = 0;
    }

    ctx->ptp_comp_cbs.get_num_obj_comp(sessionh, PTP_RESP_OK, num_objs);
    return PTP_RESP_OK;
}

static ptp_response_t hook_get_obj_handles(ptp_sessionh_t sessionh,
    ptp_storage_id_t store, ptp_obj_format_t format,
    ptp_obj_handle_t folder)
{
    juint32_t count = 0;
    ptp_obj_handle_t handles[1];
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    DECLARE_FNAME("hook_get_obj_handles");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    if ((ctx->state == DPS_STATE_DISCOVERY) && (store == PTP_STORAGE_ID_ALL)
        && (format == PTP_OBJ_FORMAT_SCRIPT))
    {
        ctx->do_dps_discovery = 1;
    }

    /* If PTP APP has a get_obj_handles handler, make it work.
     * (we grab it later at the completion) */
    if (ctx->ptp_app_cbs.get_obj_handles_h)
    {
        return ctx->ptp_app_cbs.get_obj_handles_h(sessionh, store, format,
            folder);
    }

    /* PTP App doesn't have one - use our own */
    if (ctx->do_dps_discovery)
    {
        handles[0] = ctx->dsc_msg.id;
        ctx->trans_state = DPS_TSTATE_DREQUEST;
        count = 1;
    }

    ctx->ptp_comp_cbs.get_obj_handles_comp(sessionh, PTP_RESP_OK,
        handles, count);

    return PTP_RESP_OK;
}

static ptp_response_t hook_get_obj_info(ptp_sessionh_t sessionh,
    ptp_obj_handle_t handle)
{
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    DECLARE_FNAME("hook_get_obj_info");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));
    
    /* If its not for us, pass it on */
    if (handle != ctx->dsc_msg.id)
    {
#ifdef JDEBUG
        if (!ctx->ptp_app_cbs.get_obj_info_h)
        {
            DBG_E(DPROTO_DPS, ("%s: missing get_obj_info handler\n", fname));
            return PTP_RESP_OPERATION_NOT_SUPPORTED;
        }
#endif
        return ctx->ptp_app_cbs.get_obj_info_h(sessionh, handle);
    }

    /* Printer is here to collect our request or reponse */

    /* If we are in the discovery process, provide the DDISCVRY.DPS info */
    if (ctx->state == DPS_STATE_DISCOVERY)
    {
        /* Ask application if it wants DPS */
        if (ctx->cbs.printer_connect_h)
        {
            if (!ctx->cbs.printer_connect_h(ctx->appctx))
            {
                DBG_I(DPROTO_DPS, ("%s: Application does not want DPS\n",
                    fname));
                ptp_object_removed(sessionh, ctx->dsc_msg.id);
                return PTP_RESP_DEVICE_BUSY;
            }
        }

        clear_object_info(&ctx->dsc_msg.info);
        ctx->dsc_msg.type = DPS_REQUEST;
        ctx->dsc_msg.info.format = PTP_OBJ_FORMAT_SCRIPT;
        ctx->dsc_msg.info.storage_id = ctx->store_id;
        ctx->dsc_msg.info.prot_status = PTP_PROTECT_NONE;
        ctx->dsc_msg.info.filename = ptp_string_atow("DDISCVRY.DPS");
    }

    ctx->ptp_comp_cbs.get_obj_info_comp(sessionh, PTP_RESP_OK,
        &ctx->dsc_msg.info);
    return PTP_RESP_OK;
}

static ptp_response_t hook_send_obj_info(ptp_sessionh_t sessionh,
    ptp_obj_info_t *objinfo, ptp_storage_id_t target_store,
    ptp_obj_handle_t target_folder)
{
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    jbool_t is_hrequest, is_hresponse;
    DECLARE_FNAME("hook_send_obj_info");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    /* Is this for us? */
    if ((ctx->state == DPS_STATE_DISCONN) ||
        (objinfo->format != PTP_OBJ_FORMAT_SCRIPT) || !objinfo->filename ||
        target_store || target_folder)
    {
        goto ForwardToApp;
    }

    if (ctx->state == DPS_STATE_DISCOVERY)
    {
        /* DPS discovery process - Printer is PictBridge capable */
        if (!ptp_string_compare(objinfo->filename, ctx->hdiscvry) &&
            objinfo->compressed_size == 0)
        {
            ctx->printer_msg.id = get_virtual_handle();
            ctx->printer_msg.type = DPS_RESPONSE;
            goto HandleIt;
        }

        /* This is not an HDISCVRY. Pass on to PTP application */
        goto ForwardToApp;
    }

    is_hrequest = !ptp_string_compare(objinfo->filename, ctx->hrequest);
    is_hresponse = !ptp_string_compare(objinfo->filename, ctx->hresponse);

    if (is_hrequest || is_hresponse)
    {
        ctx->printer_msg.type = is_hrequest ? DPS_REQUEST : DPS_RESPONSE;

        /* Generate a handle only once */
        if (ctx->trans_state == DPS_TSTATE_IDLE ||
            ctx->trans_state == DPS_TSTATE_HRESPONSE)
        {
            ctx->printer_msg.id = get_virtual_handle();
        }

        goto HandleIt;
    }

    /* Not for us... fall through */

ForwardToApp:
    /* Dont keep bothering a PTP application if the first send object info
     * isn't an hdiscvry sent for us */
    if (ctx->state == DPS_STATE_DISCOVERY)
        ctx->state = DPS_STATE_DISCONN;

#ifdef JDEBUG
    if (!ctx->ptp_app_cbs.send_obj_info_h)
    {
        DBG_E(DPROTO_DPS, ("%s: missing send_obj_info handler\n", fname));
        return PTP_RESP_OPERATION_NOT_SUPPORTED;
    }
#endif
        
    ctx->is_send_obj_for_dps = 0;
    return ctx->ptp_app_cbs.send_obj_info_h(sessionh, objinfo,
        target_store, target_folder);

HandleIt:
    if (!alloc_xml_buffer(&ctx->printer_msg, objinfo->compressed_size+1))
    {
        DBG_E(DPROTO_DPS, ("%s: failed to allocate memory (%d)\n",
            fname, objinfo->compressed_size));
        return PTP_RESP_GENERAL_ERROR;
    }

    ctx->is_send_obj_for_dps = 1;
    clear_object_info(&ctx->printer_msg.info);
    ctx->printer_msg.info.compressed_size = objinfo->compressed_size;
    ctx->printer_msg.info.format = objinfo->format;

    ctx->ptp_comp_cbs.send_obj_info_comp(sessionh, PTP_RESP_OK, ctx->store_id,
        0, ctx->printer_msg.id);
    return PTP_RESP_OK;
}

ptp_response_t hook_get_obj(ptp_sessionh_t sessionh, ptp_obj_handle_t handle,
    void *data, juint32_t max_size, juint32_t offset)
{
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    DECLARE_FNAME("hook_get_obj");

    DBG_X(DPROTO_DPS, ("%s: entered (%d, %d)\n", fname, max_size, offset));

    /* If its not for us, pass it on */
    if (handle != ctx->dsc_msg.id)
    {
#ifdef JDEBUG
        if (!ctx->ptp_app_cbs.get_obj_h)
        {
            DBG_E(DPROTO_DPS, ("%s: missing get_obj handler\n", fname));
            return PTP_RESP_OPERATION_NOT_SUPPORTED;
        }
#endif
        return ctx->ptp_app_cbs.get_obj_h(sessionh, handle, data, max_size,
            offset);
    }

    /* Printer wants our request or response */
    if ((ctx->trans_state == DPS_TSTATE_DRESPONSE_DATA) ||
        (ctx->trans_state == DPS_TSTATE_DREQUEST_DATA))
    {
        juint32_t act_size =
            MIN(max_size, ctx->dsc_msg.info.compressed_size-offset);

        DBG_X(DPROTO_DPS, ("%s: max_size=%u, offset=%u, obj_size=%u, "
            "act_size=%u state=%u\n", fname, max_size, offset,
            ctx->dsc_msg.info.compressed_size, act_size,
            ctx->trans_state));

        /* TODO: check size for overflow */
        j_memcpy(data, ctx->dsc_msg.xml+offset, act_size);
        ctx->ptp_comp_cbs.get_obj_comp(sessionh, PTP_RESP_OK, act_size,
            ctx->dsc_msg.info.compressed_size);
        return PTP_RESP_OK;
    }

    /* If its our object, but our state is not right, we are
     * forced to return a PTP error */
    DBG_E(DPROTO_DPS, ("%s: invalid state (%d, %d)\n", fname, ctx->state,
        ctx->trans_state));
    return PTP_RESP_GENERAL_ERROR;
}

ptp_response_t hook_send_obj(ptp_sessionh_t sessionh, void *data,
    juint32_t size, juint32_t offset)
{
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    DECLARE_FNAME("hook_send_obj");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    /* If its not for us, pass it on */
    if (!ctx->is_send_obj_for_dps)
    {
#ifdef JDEBUG
        if (!ctx->ptp_app_cbs.send_obj_h)
        {
            DBG_E(DPROTO_DPS, ("%s: missing send_obj handler\n", fname));
            return PTP_RESP_OPERATION_NOT_SUPPORTED;
        }
#endif
        return ctx->ptp_app_cbs.send_obj_h(sessionh, data, size, offset);
    }

    if ((ctx->trans_state == DPS_TSTATE_HRESPONSE_DATA) ||
        (ctx->trans_state == DPS_TSTATE_HREQUEST_DATA))
    {
        if (!ctx->collision && offset+size < ctx->printer_msg.xml_size)
            j_memcpy(ctx->printer_msg.xml+offset, data, size);

        DBG_X(DPROTO_DPS, ("%s: size=%u, offset=%u, obj_size=%u, state=%u\n",
            fname, size, offset, ctx->printer_msg.info.compressed_size,
            ctx->trans_state));

        ctx->ptp_comp_cbs.send_obj_comp(sessionh, PTP_RESP_OK); /* TODO: result */
        ctx->is_send_obj_for_dps = 0;
        return PTP_RESP_OK;
    }

    /* If its an object for us, but our state doesn't allow it, we are
     * forced to return a PTP error */
    DBG_E(DPROTO_DPS, ("%s: invalid state (%d, %d)\n", fname, ctx->state,
        ctx->trans_state));
    return PTP_RESP_GENERAL_ERROR;
}

/******************************************************
 *      PTP completion hooks
 ******************************************************/

static jresult_t hook_storage_ids_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_storage_id_t *stores,
    juint32_t count)
{
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    DECLARE_FNAME("hook_storage_ids_complete");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    /* Are we in the discovery process? */
    if ((ctx->state == DPS_STATE_DISCOVERY) && (response == PTP_RESP_OK))
    {
        /* If the PTP application has storage units, save the first one's id.
         * We will use it for our virtual objects */
        if (count)
            ctx->store_id = stores[0];
    }

    /* Call PTP's completion handler */
    return ctx->ptp_comp_cbs.get_storage_ids_comp(sessionh, response, stores,
        count);
}

static jresult_t hook_num_objects_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, juint32_t num_objects)
{
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    DECLARE_FNAME("hook_num_objects_complete");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    /* Are we in the discovery process? */
    if (ctx->do_dps_discovery && (response == PTP_RESP_OK))
    {
        /* Include our DDISCVRY*/
        num_objects++;
        ctx->do_dps_discovery = 0;
    }

    /* Call PTP's completion handler */
    return ctx->ptp_comp_cbs.get_num_obj_comp(sessionh, response,
        num_objects);
}

static jresult_t hook_obj_handles_complete(ptp_sessionh_t sessionh,
    ptp_response_t response, ptp_obj_handle_t *handles,
    juint32_t count)
{
    dps_ctx_t *ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    ptp_obj_handle_t *aug_handles = NULL;
    jresult_t rc = 0;
    DECLARE_FNAME("hook_obj_handles_complete");
    
    DBG_X(DPROTO_DPS, ("%s: entered. resp=%X, handles=%p, count=%u\n",
        fname, response, handles, count));

    if (ctx->do_dps_discovery && (response == PTP_RESP_OK))
    {
        DBG_X(DPROTO_DPS, ("%s: discovery.\n", fname));

        ctx->do_dps_discovery = 0;

        aug_handles = (ptp_obj_handle_t*)
            jmalloc(sizeof(ptp_obj_handle_t)*(count+1), M_ZERO);

        if (!aug_handles)
        {
            DBG_E(DPROTO_DPS, ("%s: failed to allocate handles array.\n",
                fname));
            rc = JENOMEM;
            response = PTP_RESP_GENERAL_ERROR;
            goto Exit;
        }

        if (handles && count)
        {
            DBG_X(DPROTO_DPS, ("%s: copying %u handles.\n", fname, count));
            j_memcpy(aug_handles, handles, count*sizeof(ptp_obj_handle_t));
        }

        /* Prepare the DDISCVRY object */
        aug_handles[count] = ctx->dsc_msg.id;
        DBG_X(DPROTO_DPS, ("%s: new handle %X at idx %u.\n", fname,
            ctx->dsc_msg.id, count));
        count++;
        handles = aug_handles;
        ctx->trans_state = DPS_TSTATE_DREQUEST;
    }

Exit:
    /* Call PTP's handler */
    DBG_X(DPROTO_DPS, ("%s: calling PTP completion: handles=%p, handle[0]=%X, "
        "count=%u.\n", fname, handles, handles[0] , count));

    rc = ctx->ptp_comp_cbs.get_obj_handles_comp(sessionh, response, handles,
        count);

    if (aug_handles)
        jfree(aug_handles);

    return rc;
}

/******************************************************
 *      DPS initialization
 ******************************************************/

jresult_t dps_init(ptp_instanceh_t insth, dps_callbacks_t *app_cbs,
    dps_appctx_t appctx, dps_handle_t *dpsh)
{
    ptp_responder_cbs hook_cbs;
    ptp_responder_comp_cbs hook_comp_cbs;
    dps_ctx_t *dpsc = NULL;
    DECLARE_FNAME("dps_init");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));
    
    /* Initialize our state */
    dpsc = (dps_ctx_t*)jmalloc(sizeof(dps_ctx_t), M_ZERO);
    if (!dpsc)
    {
        DBG_E(DPROTO_DPS, ("%s: failed to allocate DPS context\n",
            fname));
        return JENOMEM;
    }

    dpsc->state = DPS_STATE_DISCOVERY;
    dpsc->ptph = NULL;
    dpsc->appctx = appctx;
    j_memcpy(&dpsc->cbs, app_cbs, sizeof(dps_callbacks_t));

    /* Register PTP callbacks */
    j_memset(&hook_cbs, 0, sizeof(hook_cbs));
    hook_cbs.open_session_h = hook_open_session;
    hook_cbs.close_session_h = hook_close_session;
    hook_cbs.get_num_objs_h = hook_get_num_objects;
    hook_cbs.get_obj_handles_h = hook_get_obj_handles;
    hook_cbs.get_obj_info_h = hook_get_obj_info;
    hook_cbs.send_obj_info_h = hook_send_obj_info;
    hook_cbs.get_obj_h = hook_get_obj;
    hook_cbs.send_obj_h = hook_send_obj;
    hook_cbs.get_storage_ids_h = hook_get_storage_ids;
    hook_cbs.transaction_complete_h = ptp_transaction_complete;
    hook_cbs.unregister_h = dps_uninit;

    j_memset(&hook_comp_cbs, 0, sizeof(hook_comp_cbs));
    hook_comp_cbs.get_num_obj_comp = hook_num_objects_complete;
    hook_comp_cbs.get_obj_handles_comp = hook_obj_handles_complete;
    hook_comp_cbs.get_storage_ids_comp = hook_storage_ids_complete;

    *dpsh = (dps_handle_t)dpsc;

    return ptp_hook_responder(insth, &dpsc->ptp_app_cbs,
        &dpsc->ptp_comp_cbs, &hook_cbs, &hook_comp_cbs, dpsc);
}

void dps_uninit(ptp_instanceh_t insth, ptp_appctx_t global_ctx)
{
    dps_ctx_t *ctx = (dps_ctx_t*)ptp_inst_get_global_hookctx(insth);
    DECLARE_FNAME("dps_uninit");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    if (ctx)
    {
        ctx->ptp_app_cbs.unregister_h(insth, global_ctx);
        jfree(ctx);
    }
}

/******************************************************
 *      Helper functions
 ******************************************************/

/* DSC to Printer requests */
jresult_t dps_send_request(dps_handle_t handle, char *xml_request,
    juint32_t size)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    jresult_t rc = 0;
    DECLARE_FNAME("dps_send_request");

    DBG_X(DPROTO_DPS, ("%s: entered (%d)\n", fname, size));

    if (ctx->state < DPS_STATE_IDLE)
    {
        DBG_E(DPROTO_DPS, ("%s: cannot send a request when DPS is inactive "
            "(%d)\n", fname, ctx->state));
        return JEPERM;
    }
    
    if (ctx->trans_state != DPS_TSTATE_IDLE)
    {
        DBG_E(DPROTO_DPS, ("%s: trying to send a request while device is"
            " busy. (%d)\n", fname, ctx->trans_state));
        return JEBUSY;
    }

    if (!alloc_xml_buffer(&ctx->dsc_msg, size+1))
    {
        DBG_E(DPROTO_DPS, ("%s: failed to allocate xml buffer\n", fname));
        return JENOMEM;
    }

    j_strlcpy(ctx->dsc_msg.xml, xml_request, size + 1);
    ctx->dsc_msg.type = DPS_REQUEST;
    clear_object_info(&ctx->dsc_msg.info);
    ctx->dsc_msg.info.compressed_size = size;
    ctx->dsc_msg.info.format = PTP_OBJ_FORMAT_SCRIPT;
    ctx->dsc_msg.info.storage_id = ctx->store_id;
    ctx->dsc_msg.info.prot_status = PTP_PROTECT_NONE;
    ctx->dsc_msg.info.filename = ptp_string_atow("DREQUEST.DPS");

    ctx->dsc_msg.id = get_virtual_handle();
    ptp_object_added(ctx->ptph, ctx->dsc_msg.id);
    rc = ptp_request_object_transfer(ctx->ptph, ctx->dsc_msg.id);
    if (!rc)
        ctx->trans_state = DPS_TSTATE_DREQUEST;

    return rc;
}

static jresult_t send_simple_request(dps_handle_t handle, char *name)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    char *xml_body = ctx->xml_body;
    char *xml_req = ctx->xml_req;
    DECLARE_FNAME("send_simple_request");

    DBG_X(DPROTO_DPS, ("%s: entered (%s)\n", fname, name));

    if (ctx->state != DPS_STATE_IDLE)
    {
        DBG_E(DPROTO_DPS, ("%s: Invalid state (%d)\n", fname, ctx->state));
        return EUNKNOWN;
    }

    *xml_body = 0;
    *xml_req = 0;
    xml_body_section(xml_body, MAX_XML_BODY_SIZE, name, XML_SECT_EMPTY);
    xml_build_request(xml_req, MAX_XML_REQ_SIZE, xml_body);

    return dps_send_request(handle, xml_req, j_strlen(xml_req));
}

/* Printer to DSC requests */
jresult_t dps_send_response(dps_handle_t handle, char *xml_response,
    juint32_t size)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    jresult_t rc = 0;
    DECLARE_FNAME("dps_send_response");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    if ((ctx->trans_state != DPS_TSTATE_DRESPONSE) ||
        (ctx->state < DPS_STATE_IDLE))
    {
        DBG_E(DPROTO_DPS, ("%s: invalid state (%d, %d)\n", fname,
            ctx->state, ctx->trans_state));
        return EUNKNOWN;
    }

    if (!alloc_xml_buffer(&ctx->dsc_msg, size+1))
    {
        DBG_E(DPROTO_DPS, ("%s: failed to allocate xml buffer\n", fname));
        return JENOMEM;
    }

    j_strlcpy(ctx->dsc_msg.xml, xml_response, size + 1);
    ctx->dsc_msg.type = DPS_RESPONSE;
    clear_object_info(&ctx->dsc_msg.info);
    ctx->dsc_msg.info.compressed_size = size;
    ctx->dsc_msg.info.format = PTP_OBJ_FORMAT_SCRIPT;
    ctx->dsc_msg.info.storage_id = ctx->store_id;
    ctx->dsc_msg.info.prot_status = PTP_PROTECT_NONE;
    ctx->dsc_msg.info.filename = ptp_string_atow("DRSPONSE.DPS");

    ctx->dsc_msg.id = get_virtual_handle();
    ptp_object_added(ctx->ptph, ctx->dsc_msg.id);

    rc = ptp_request_object_transfer(ctx->ptph, ctx->dsc_msg.id);
    if (!rc)
        ctx->trans_state = DPS_TSTATE_DRESPONSE;

    return rc;
}

static void send_simple_response(dps_handle_t handle, dps_param_t result,
    char *req_name)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    char *xml_body = ctx->xml_body;
    char *xml_req = ctx->xml_req;

    *xml_body = 0;
    *xml_req = 0;

    if (req_name)
    {
        xml_body_section(xml_body, MAX_XML_BODY_SIZE, req_name,
            XML_SECT_EMPTY);
    }

    xml_build_response(xml_req, MAX_XML_REQ_SIZE, result, xml_body);
    dps_send_response(handle, xml_req, j_strlen(xml_req));
    return;
}

/***********************************************************
 *  DPS API implementation - DREQUEST & DRESPONSE handling
 ***********************************************************/

jresult_t dps_configure_print_service(dps_handle_t handle,
    dps_req_conf_t *dsc_config)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    char *xml_body = ctx->xml_body;
    char *xml_req = ctx->xml_req;
    char ver[20];
    jresult_t rc = 0;
    DECLARE_FNAME("dps_configure_print_service");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    if (ctx->state != DPS_STATE_IDLE)
    {
        DBG_E(DPROTO_DPS, ("%s: Invalid state (%d)\n", fname, ctx->state));
        return EUNKNOWN;
    }
    
    *xml_body = 0;
    *xml_req = 0;
    
    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "configurePrintService",
        XML_SECT_OPEN);
    
    j_snprintf(ver, sizeof(ver), "%d.%d", dsc_config->dps_version[0].major,
        dsc_config->dps_version[0].minor);
        
    xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "dpsVersions", ver);
    
    xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "vendorName",
        dsc_config->vendor_name);

    if (dsc_config->vendor_specific_version.major &&
        dsc_config->vendor_specific_version.minor)
    {
        j_snprintf(ver, sizeof(ver), "%d.%d",
            dsc_config->vendor_specific_version.major,
            dsc_config->vendor_specific_version.minor);

        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE,
            "vendorSpecificVersion", ver);
    }
        
    xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "productName",
        dsc_config->product_name);

    if (*dsc_config->serial_no)
    {
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "serialNo",
            dsc_config->serial_no);
    }

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "configurePrintService",
        XML_SECT_CLOSE);
    
    xml_build_request(xml_req, MAX_XML_REQ_SIZE, xml_body);

    rc = dps_send_request(handle, xml_req, j_strlen(xml_req));
    if (!rc)
        ctx->state = DPS_STATE_CONFIGURE;

    return rc;
}

jresult_t dps_start_job(dps_handle_t handle, dps_job_config_t *config,
    dps_job_info_t *info, jint_t count)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    char *xml_body = ctx->xml_body;
    char *xml_req = ctx->xml_req;
    char pad[30];
    jresult_t rc = 0;
    jint_t i;
    DECLARE_FNAME("dps_start_job");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    if (ctx->state != DPS_STATE_IDLE)
    {
        DBG_E(DPROTO_DPS, ("%s: Invalid state (%d)\n", fname, ctx->state));
        return EUNKNOWN;
    }
    
    *xml_body = 0;
    *xml_req = 0;

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "startJob", XML_SECT_OPEN);
    
    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "jobConfig", XML_SECT_OPEN);

    if (config->quality)
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "quality",
            j_numtostr(pad, 16, 8, 8, config->quality));

    if (config->paper_size)
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "paperSize",
            j_numtostr(pad, 16, 8, 8, config->paper_size));

    if (config->paper_type)
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "paperType",
            j_numtostr(pad, 16, 8, 8, config->paper_type));

    if (config->file_type)
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "fileType",
            j_numtostr(pad, 16, 8, 8, config->file_type));

    if (config->date_print)
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "datePrint",
            j_numtostr(pad, 16, 8, 8, config->date_print));

    if (config->filename_print)
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "fileNamePrint",
            j_numtostr(pad, 16, 8, 8, config->filename_print));

    if (config->image_optimize)
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "imageOptimize",
            j_numtostr(pad, 16, 8, 8, config->image_optimize));

    if (config->layout)
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "layout",
            j_numtostr(pad, 16, 8, 8, config->layout));
    else
    {
        if (config->fixed_size)
            xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "fixedSize",
                j_numtostr(pad, 16, 8, 8, config->fixed_size));
    }

    if (config->cropping)
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "cropping",
            j_numtostr(pad, 16, 8, 8, config->cropping));

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "jobConfig", XML_SECT_CLOSE);

    /* Print Info section */
    for(i=0; i<count; i++)
    {
        xml_body_section(xml_body, MAX_XML_BODY_SIZE, "printInfo",
            XML_SECT_OPEN);
    
        if (config->cropping && config->cropping != DPS_CROPPING_OFF)
        {
            j_snprintf(pad, sizeof(pad), "%.4X %.4X %.4X %.4X",
                info[i].cropping_area.left, info[i].cropping_area.top,
                info[i].cropping_area.width, info[i].cropping_area.height);
            xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "croppingArea",
                pad);
        }

        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "fileID",
            j_numtostr(pad, 16, 8, 8, info[i].file_id));
        
        if (info[i].file_name[0] &&
            config->filename_print != DPS_FILENAME_PRINT_OFF)
        {
            xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "fileName",
                info[i].file_name);
        }
        
        if (info[i].date[0] && config->date_print != DPS_DATE_PRINT_OFF)
        {
            xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "date",
                info[i].date);
        }

        if (info[i].copies)
        {
            xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "copies",
                j_numtostr(pad, 10, 3, 3, info[i].copies));
        }

        /* DPOF restart */
        if (config->file_type == DPS_FILE_TYPE_DPOF)
        {
            if (count > 1)
            {
                DBG_E(DPROTO_DPS, ("%s: DPOF printing supports a single"
                    " job_info only\n", fname));
                return JEINVAL;
            }

            if (info[i].dpof_restart.prt_pid &&
                info[i].dpof_restart.file_path[0] &&
                info[i].dpof_restart.copy_id)
            {
                xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "prtPID",
                    j_numtostr(pad, 10, 3, 3, info[i].dpof_restart.prt_pid));
                xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "filePath",
                    info[i].dpof_restart.file_path);
                xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "copyID",
                    j_numtostr(pad, 10, 3, 3, info[i].dpof_restart.copy_id));
            }
        }

        xml_body_section(xml_body, MAX_XML_BODY_SIZE, "printInfo",
            XML_SECT_CLOSE);
    }

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "startJob", XML_SECT_CLOSE);

    xml_build_request(xml_req, MAX_XML_REQ_SIZE, xml_body);

    rc = dps_send_request(handle, xml_req, j_strlen(xml_req));
    if (!rc)
        ctx->state = DPS_STATE_START_JOB;

    return rc;
}

static struct
{
    dps_capability_t capability;
    char *cap_str;
} cap_map[] =
{
    { DPS_CAP_QUALITIES,            "qualities"             },
    { DPS_CAP_PAPER_SIZES,          "paperSizes"            },
    { DPS_CAP_PAPER_TYPES,          "paperTypes"            },
    { DPS_CAP_FILE_TYPES,           "fileTypes"             },
    { DPS_CAP_DATE_PRINTS,          "datePrints"            },
    { DPS_CAP_FILENAME_PRINTS,      "fileNamePrints"        },
    { DPS_CAP_IMAGE_OPTIMIZE,       "imageOptimizes"        },
    { DPS_CAP_LAYOUTS,              "layouts"               },
    { DPS_CAP_FIXED_SIZES,          "fixedSizes"            },
    { DPS_CAP_CROPPINGS,            "croppings"             }
};

jresult_t dps_get_capability(dps_handle_t handle,
    dps_capability_t cap_type, dps_param_t paper_size)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    char *xml_body = ctx->xml_body;
    char *xml_req = ctx->xml_req;
    char pad[20];
    jresult_t rc = 0;

    DECLARE_FNAME("dps_get_capability");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    if (ctx->state != DPS_STATE_IDLE)
    {
        DBG_E(DPROTO_DPS, ("%s: Invalid state (%d)\n", fname, ctx->state));
        return EUNKNOWN;
    }

    if (cap_type > DPS_CAP_CROPPINGS)
    {
        DBG_E(DPROTO_DPS, ("%s: Invalid capability (%d)\n", cap_type));
        return JEINVAL;
    }
    
    *xml_body = 0;
    *xml_req = 0;

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "getCapability",
        XML_SECT_OPEN);

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "capability", XML_SECT_OPEN);

    if ((cap_type == DPS_CAP_PAPER_TYPES || cap_type == DPS_CAP_LAYOUTS) &&
        paper_size)
    {
        xml_body_add_param_with_attrib(xml_body, MAX_XML_BODY_SIZE,
            cap_map[cap_type].cap_str, NULL,
            "paperSize", j_numtostr(pad, 16, 8, 8, paper_size));
    }
    else
    {
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE,
            cap_map[cap_type].cap_str, NULL);
    }

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "capability", XML_SECT_CLOSE);

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "getCapability",
        XML_SECT_CLOSE);

    xml_build_request(xml_req, MAX_XML_REQ_SIZE, xml_body);

    rc = dps_send_request(handle, xml_req, j_strlen(xml_req));
    if (!rc)
    {
        ctx->state = DPS_STATE_GET_CAPABILITY;
        ctx->cap_requested = cap_type;
    }
        
    return rc;
}

jresult_t dps_abort_job(dps_handle_t handle, dps_param_t abort_style)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    char *xml_body = ctx->xml_body;
    char *xml_req = ctx->xml_req;
    char pad[20];
    jresult_t rc = 0;

    DECLARE_FNAME("dps_abort_job");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    if (ctx->state != DPS_STATE_IDLE)
    {
        DBG_E(DPROTO_DPS, ("%s: Invalid state (%d)\n", fname, ctx->state));
        return EUNKNOWN;
    }

    *xml_body = 0;
    *xml_req = 0;

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "abortJob", XML_SECT_OPEN);

    if (abort_style)
    {
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "abortStyle",
            j_numtostr(pad, 16, 8, 8, abort_style));
    }

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "abortJob", XML_SECT_CLOSE);
    
    xml_build_request(xml_req, MAX_XML_REQ_SIZE,  xml_body);

    rc = dps_send_request(handle, xml_req, j_strlen(xml_req));
    if (!rc)
        ctx->state = DPS_STATE_ABORT_JOB;
        
    return rc;
}

jresult_t dps_continue_job(dps_handle_t handle)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    jresult_t rc;

    rc = send_simple_request(handle, "continueJob");
    if (!rc)
        ctx->state = DPS_STATE_CONTINUE_JOB;

    return rc;
}

jresult_t dps_get_job_status(dps_handle_t handle)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    jresult_t rc;

    rc = send_simple_request(handle, "getJobStatus");
    if (!rc)
        ctx->state = DPS_STATE_GET_JOB_STAT;
        
    return rc;
}

jresult_t dps_get_device_status(dps_handle_t handle)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    jresult_t rc;

    rc = send_simple_request(handle, "getDeviceStatus");
    if (!rc)
        ctx->state = DPS_STATE_GET_DEV_STAT;

    return rc;
}

static void send_notify_job_response(dps_handle_t handle,
    dps_param_t result)
{
    send_simple_response(handle, result, "notifyJobStatus");
}

static void send_notify_device_response(dps_handle_t handle,
    dps_param_t result)
{
    send_simple_response(handle, result, "notifyDeviceStatus");
}

static void send_get_file_id_response(dps_handle_t handle,
    dps_param_t result, ptp_obj_handle_t id)
{
    dps_ctx_t *ctx = (dps_ctx_t*)handle;
    char *xml_body = ctx->xml_body;
    char *xml_req = ctx->xml_req;
    char pad[20];

    *xml_body = 0;
    *xml_req = 0;
    
    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "getFileID",
        XML_SECT_OPEN);

    if (result == DPS_RESULT_OK)
    {
        xml_body_add_param(xml_body, MAX_XML_BODY_SIZE, "fileID",
            j_numtostr(pad, 16, 8, 8, id));
    }

    xml_body_section(xml_body, MAX_XML_BODY_SIZE, "getFileID",
        XML_SECT_CLOSE);

    xml_build_response(xml_req, MAX_XML_REQ_SIZE, 
        result, xml_body);

    dps_send_response(handle, xml_req, j_strlen(xml_req));
    return;
}

/***********************************************************
 *  DPS API implementation - HRESPONSE & HREQUEST handling
 ***********************************************************/

static void handle_config_response(dps_ctx_t *ctx)
{
    static xml_form_t form[] =
    {
        { "output",                     1, 1, NULL, NULL, 0, 0 },
        { "result",                     2, 1, convert_hex, NULL, 1, 0 },
        { "configurePrintService",      2, 1, NULL, NULL, 0, 0 },
        { "printServiceAvailable",      3, 1, convert_hex, NULL, 1, 0 },
        { "dpsVersions",                3, 1, convert_ver, NULL,
            DPS_MAX_VERSIONS, 0 },
        { "vendorName",                 3, 1, convert_str, NULL,
            DPS_MAX_STRING_SIZE, 0 },
        { "vendorSpecificVersion",      3, 0, convert_ver, NULL, 1, 0 },
        { "productName",                3, 1, convert_str, NULL,
            DPS_MAX_STRING_SIZE, 0 },
        { "serialNo",                   3, 0, convert_str, NULL,
            DPS_MAX_STRING_SIZE, 0 },
        { NULL,                         0, 0, NULL, NULL, 0, 0 }
    };
    dps_param_t result = 0;
    dps_res_conf_t printer_cfg;
    DECLARE_FNAME("handle_config_response");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    form[1].data = &result;
    form[3].data = &printer_cfg.service_available;
    form[4].data = printer_cfg.conf.dps_version;
    form[5].data = printer_cfg.conf.vendor_name;
    form[6].data = &printer_cfg.conf.vendor_specific_version;
    form[7].data = printer_cfg.conf.product_name;
    form[8].data = printer_cfg.conf.serial_no;

    init_form(form);
    j_memset(&printer_cfg, 0, sizeof(printer_cfg));

    if (!xml_parse_hierarchy(ctx->printer_msg.xml, form))
    {
        ctx->cbs.config_print_service_complete(ctx->appctx,
            0, NULL);
        return;
    }
    
    printer_cfg.conf.versions_count = form[4].actual_count;
    ctx->cbs.config_print_service_complete(ctx->appctx,
        result, &printer_cfg);
}

static void handle_get_cap_response(dps_ctx_t *ctx)
{
    static xml_form_t form[] =
    {
        { "output",             1, 1, NULL, NULL, 0, 0 },
        { "result",             2, 1, convert_hex, NULL, 1, 0 },
        { "getCapability",      2, 1, NULL, NULL, 0, 0 },
        { "capability",         3, 1, NULL, NULL, 0, 0 },
        { NULL,                 4, 1, convert_hex, NULL, MAX_CAPABILITIES, 0 },
        { NULL,                 0, 0, NULL, NULL, 0, 0 }
    };
    dps_param_t result = 0;
    dps_cap_array_t cap_array;
    DECLARE_FNAME("handle_get_cap_response");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    form[1].data = &result;
    form[4].data = cap_array.capability;
    form[4].name = cap_map[ctx->cap_requested].cap_str;
 
    init_form(form);
    j_memset(&cap_array, 0, sizeof(cap_array));

    cap_array.cap_type = ctx->cap_requested; /* DK : cap_type needed to index into cap list array */
    ctx->cap_requested = DPS_CAP_QUALITIES;  /* DK : reset cap_requested after updating cap_array */
    
    if (!xml_parse_hierarchy(ctx->printer_msg.xml, form))
    {
        ctx->cbs.get_capability_complete(ctx->appctx, 0, NULL);
        return;
    }
    
    cap_array.count = form[4].actual_count;
    ctx->cbs.get_capability_complete(ctx->appctx, result, &cap_array);
}

static void handle_job_status(dps_ctx_t *ctx, jbool_t is_ntf)
{
    static xml_form_t form[] =
    {
        { "output",             1, 1, NULL, NULL, 0, 0 },
        { "result",             2, 1, convert_hex, NULL, 1, 0 },
        { "getJobStatus",       2, 1, NULL, NULL, 0, 0 },
        { "prtPID",             3, 0, convert_dec, NULL, 1, 0 },
        { "filePath",           3, 0, convert_str, NULL, MAX_DCF_PATH_SIZE, 0 },
        { "copyID",             3, 0, convert_dec, NULL, 1, 0 },
        { "progress",           3, 1, convert_progress, NULL, 1, 0 },
        { "imagesPrinted",      3, 0, convert_dec, NULL, 1, 0 },
        { NULL,                 0, 0, NULL, NULL, 0, 0 }
    };
    dps_param_t result = 0;
    dps_job_status_t *job_stat = &ctx->job_status;
    DECLARE_FNAME("handle_job_status");

    DBG_X(DPROTO_DPS, ("%s: entered %s\n", fname,
        is_ntf?"(notification)":"(response)"));

    form[1].data = (void*)&result;
    form[3].data = (void*)&job_stat->dpof_params.prt_pid;
    form[4].data = (void*)job_stat->dpof_params.file_path;
    form[5].data = (void*)&job_stat->dpof_params.copy_id;
    form[6].data = (void*)&job_stat->pages_done;
    form[7].data = (void*)&job_stat->images_printed;
    if (is_ntf)
    {
        form[2].name = "notifyJobStatus";
        form[1].mandatory = 0;
        form[0].name = "input";
    }

    init_form(form);
    j_memset(&ctx->job_status, 0, sizeof(ctx->job_status));
    ctx->hreq_result = DPS_RESULT_UNEXECUTED_OP;

    if (!xml_parse_hierarchy(ctx->printer_msg.xml, form))
    {
        result = 0;
        job_stat = NULL;
        goto Exit;
    }

    /* Is this is a dpof_job? */
    ctx->job_status.job_is_dpof = (form[3].actual_count ||
        form[4].actual_count || form[5].actual_count);

    /* If so, do the parameter combination make sense? */
    if ((form[7].actual_count!=0) && ctx->job_status.job_is_dpof)
    {
        DBG_E(DPROTO_DPS, ("%s: invalid job status parameter combination\n",
            fname));
        result = 0;
        job_stat = NULL;
    }
    else
    {
        if (is_ntf)
            ctx->hreq_result = DPS_RESULT_OK;
    }

Exit:    
    if (is_ntf)
        send_notify_job_response(ctx, ctx->hreq_result);
    else
        ctx->cbs.get_job_status_complete(ctx->appctx, result, job_stat);
}

#define NUM_PRINT_STATUS 3
#define NUM_END_REASON 5
#define NUM_ERR_STATUS 3
#define NUM_ERR_REASON 26
#define NUM_DISCONN 2
#define NUM_CAP_CHANGE 2
#define NUM_NEW_JOB_OK 2

static struct
{
    dps_param_t print_service_status[NUM_PRINT_STATUS];
    dps_param_t job_end_reason[NUM_END_REASON];
    dps_param_t error_status[NUM_ERR_STATUS];
    dps_reason_t error_reason[NUM_ERR_REASON];
    dps_param_t disconnect_enable[NUM_DISCONN];
    dps_param_t capability_changed[NUM_CAP_CHANGE];
    dps_param_t new_job_ok[NUM_NEW_JOB_OK];
} valid_dev_stat_params = 
{
    /* Possible print_service_status values */
    {
        DPS_DEV_STAT_PRINTING,
        DPS_DEV_STAT_IDLE,
        DPS_DEV_STAT_PAUSED
    },

    /* Possible job_end_reason values */
    {
        DPS_DEV_STAT_JOB_NOT_ENDED,
        DPS_DEV_STAT_JOB_ENDED,
        DPS_DEV_STAT_JOB_TERMINATED,
        DPS_DEV_STAT_JOB_ABORTED,
        DPS_DEV_STAT_JOB_ENDED_OTHER
    },

    /* Possible error_status values */
    {
        DPS_DEV_STAT_ERR_NONE,
        DPS_DEV_STAT_ERR_WARNING,
        DPS_DEV_STAT_ERR_FATAL
    },

    /* Possible error_reason values (dps_reason_t / use REASON MACROS) */
    {
        DPS_DEV_STAT_ERR_REASON_NONE, 
        
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_PAPER, DPS_DEV_STAT_ERR_REASON_PAPER_DEFAULT, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_PAPER, DPS_DEV_STAT_ERR_REASON_PAPER_EMPTY, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_PAPER, DPS_DEV_STAT_ERR_REASON_PAPER_LOAD, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_PAPER, DPS_DEV_STAT_ERR_REASON_PAPER_EJECT, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_PAPER, DPS_DEV_STAT_ERR_REASON_PAPER_MEDIA, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_PAPER, DPS_DEV_STAT_ERR_REASON_PAPER_JAMMED, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_PAPER, DPS_DEV_STAT_ERR_REASON_PAPER_LOW, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_PAPER, DPS_DEV_STAT_ERR_REASON_PAPER_MISMATCH, 0),
            
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_INK, DPS_DEV_STAT_ERR_REASON_INK_DEFAULT, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_INK, DPS_DEV_STAT_ERR_REASON_INK_EMPTY, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_INK, DPS_DEV_STAT_ERR_REASON_INK_LOW, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_INK, DPS_DEV_STAT_ERR_REASON_INK_WASTE_FULL, 0),
            
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_DEFAULT, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_FATAL, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_SRVCALL, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_UNAVAIL, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_BUSY, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_LEVER, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_COVER_OPEN, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_NO_MRK_AGENT, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_INK_COVER, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_HARDWARE, DPS_DEV_STAT_ERR_REASON_HW_NO_CARTRIDGE, 0),
        
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_FILE, DPS_DEV_STAT_ERR_REASON_FILE_DEFAULT, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_FILE, DPS_DEV_STAT_ERR_REASON_FILE_INFO, 0),
        DPS_REASON_MAKE(DPS_DEV_STAT_ERR_REASON_FILE, DPS_DEV_STAT_ERR_REASON_FILE_FORMAT, 0)
    },

    /* Possible disconnect_enable values */
    {
        DPS_DEV_STAT_DISCONN_ENABLED,
        DPS_DEV_STAT_DISCONN_DISABLED
    },

    /* Possible capability_changed values */
    {
        DPS_DEV_STAT_CAP_CHNG_FALSE,
        DPS_DEV_STAT_CAP_CHNG_TRUE
    },

    /* Possible new_job_ok values */
    {
        DPS_DEV_STAT_NEW_JOB_NOT_OK,
        DPS_DEV_STAT_NEW_JOB_OK
    }
};

static dps_param_t validate_param(dps_param_t value,
    dps_param_t *valid_values, jint_t count)
{
    int i;

    /* A value of 0 means there is no actual DPS value to validate */
    if (!value)
        return DPS_RESULT_OK;

    for (i=0; i<count; i++)
    {
        if (value == valid_values[i])
            return DPS_RESULT_OK;
    }

    return DPS_PARAM_MAKE(DPS_RESULT_UNSUPPORTED_OP,
        DPS_RESULT_MIN_INVALID_VALUE);
}

static dps_param_t validate_dev_stat(dps_device_status_t *dev_stat)
{
    dps_param_t result;
    DECLARE_FNAME("validate_dev_stat");
    
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    result = validate_param(dev_stat->print_service_status,
        valid_dev_stat_params.print_service_status, NUM_PRINT_STATUS);
    if (result != DPS_RESULT_OK)
    {
        DBG_E(DPROTO_DPS, ("%s: invalid 'print_service_status' parameter "
            "(0x%X)\n", fname, dev_stat->print_service_status));
        return result;
    }

    result = validate_param(dev_stat->job_end_reason,
        valid_dev_stat_params.job_end_reason, NUM_END_REASON);
    if (result != DPS_RESULT_OK)
    {
        DBG_E(DPROTO_DPS, ("%s: invalid 'job_end_reason' parameter "
            "(0x%X)\n", fname, dev_stat->job_end_reason));
        return result;
    }

    result = validate_param(dev_stat->error_status,
        valid_dev_stat_params.error_status, NUM_ERR_STATUS);
    if (result != DPS_RESULT_OK)
    {
        DBG_E(DPROTO_DPS, ("%s: invalid 'error_status' parameter "
            "(0x%X)\n", fname, dev_stat->error_status));
        return result;
    }

    result = validate_param(dev_stat->error_reason,
        valid_dev_stat_params.error_reason, NUM_ERR_REASON);
    if (result != DPS_RESULT_OK)
    {
        DBG_E(DPROTO_DPS, ("%s: invalid 'error_reason' parameter "
            "(0x%X)\n", fname, dev_stat->error_reason));
        return result;
    }

    result = validate_param(dev_stat->disconnect_enable,
      valid_dev_stat_params.disconnect_enable, NUM_DISCONN);
    if (result != DPS_RESULT_OK)
    {
      DBG_E(DPROTO_DPS, ("%s: invalid 'disconnect_enable' parameter "
        "(0x%X)\n", fname, dev_stat->disconnect_enable));
      return result;
    }
    result = validate_param(dev_stat->capability_changed,
      valid_dev_stat_params.capability_changed, NUM_CAP_CHANGE);
    if (result != DPS_RESULT_OK)
    {
      DBG_E(DPROTO_DPS, ("%s: invalid 'capability_changed' parameter "
        "(0x%X)\n", fname, dev_stat->capability_changed));
      return result;
    }
    
    result = validate_param(dev_stat->new_job_ok,
        valid_dev_stat_params.new_job_ok, NUM_NEW_JOB_OK);
    if (result != DPS_RESULT_OK)
    {
        DBG_E(DPROTO_DPS, ("%s: invalid 'new_job_ok' parameter "
            "(0x%X)\n", fname, dev_stat->new_job_ok));
        return result;
    }

    DBG_X(DPROTO_DPS, ("%s: validation succeeded\n", fname));
    return DPS_RESULT_OK;
}

static void handle_device_status(dps_ctx_t *ctx, jbool_t is_ntf)
{
    static xml_form_t form[] =
    {
        { "output",                     1, 1, NULL, NULL, 0, 0 },
        { "result",                     2, 1, convert_hex, NULL, 1, 0 },
        { "getDeviceStatus",            2, 1, NULL, NULL, 0, 0 },
        { "dpsPrintServiceStatus",      3, 1, convert_hex, NULL, 1, 0 },
        { "jobEndReason",               3, 0, convert_hex, NULL, 1, 0 },
        { "errorStatus",                3, 1, convert_hex, NULL, 1, 0 },
        { "errorReason",                3, 1, convert_hex, NULL, 1, 0 },
        { "disconnectEnable",           3, 0, convert_hex, NULL, 1, 0 },
        { "capabilityChanged",          3, 0, convert_hex, NULL, 1, 0 },
        { "newJobOK",                   3, 1, convert_hex, NULL, 1, 0 },
        { NULL,                         0, 0, NULL, NULL, 0, 0 }
    };
    dps_param_t result = 0;
    dps_device_status_t *dev_stat = &ctx->dev_status;
    DECLARE_FNAME("handle_device_status");
    
    DBG_X(DPROTO_DPS, ("%s: entered %s\n", fname,
        is_ntf?"(notification)":"(response)"));

    form[1].data = (void*)&result;
    form[3].data = (void*)&dev_stat->print_service_status;
    form[4].data = (void*)&dev_stat->job_end_reason;
    form[5].data = (void*)&dev_stat->error_status;
    form[6].data = (void*)&dev_stat->error_reason;
    form[7].data = (void*)&dev_stat->disconnect_enable;
    form[8].data = (void*)&dev_stat->capability_changed;
    form[9].data = (void*)&dev_stat->new_job_ok;
    if (is_ntf)
    {
        form[2].name = "notifyDeviceStatus";
        form[1].mandatory = 0;
        form[0].name = "input";
    }

    init_form(form);
    j_memset(dev_stat, 0, sizeof(dps_device_status_t));

    if (!xml_parse_hierarchy(ctx->printer_msg.xml, form))
    {
        if (is_ntf)
        {
            ctx->hreq_result = DPS_PARAM_MAKE(DPS_RESULT_UNSUPPORTED_OP, 0);

            /* TODO: make xml_parse_hierarchy return an error when a mandatory
             * parameter is missing so we can use the minor code
             * DPS_RESULT_MIN_MISSING_PARAM */
        }
        else
        {
            result = 0;
            dev_stat = NULL;
        }

        goto Exit;
    }

    if (is_ntf)
        ctx->hreq_result = validate_dev_stat(dev_stat);

Exit:
    if (is_ntf)
        send_notify_device_response(ctx, ctx->hreq_result);
    else
        ctx->cbs.get_device_status_complete(ctx->appctx, result, dev_stat);
}

static void handle_simple_response(dps_ctx_t *ctx, char *name,
    dps_complete_cb cb)
{
    dps_param_t result = 0;

    static xml_form_t form[] =
    {
        { "output",     1, 1, NULL, NULL, 0, 0 },
        { "result",     2, 1, convert_hex, NULL, 1, 0 },
        { NULL,         2, 1, NULL, NULL, 0, 0 },
        { NULL,         0, 0, NULL, NULL, 0, 0 }
    };

    DECLARE_FNAME("handle_simple_response");
    DBG_X(DPROTO_DPS, ("%s: entered (%s)\n", fname, name));

    form[1].data = (void*)&result;
    form[2].name = name;
    init_form(form);

    if (!xml_parse_hierarchy(ctx->printer_msg.xml, form))
    {
        cb(ctx->appctx, 0);
        return;
    }

    cb(ctx->appctx, result);
}

static void handle_abort_job_complete(dps_ctx_t *ctx)
{
    handle_simple_response(ctx, "abortJob", ctx->cbs.abort_job_complete);
}

static void handle_start_job_complete(dps_ctx_t *ctx)
{
    handle_simple_response(ctx, "startJob", ctx->cbs.start_job_complete);
}

static void handle_continue_job_complete(dps_ctx_t *ctx)
{
    handle_simple_response(ctx, "continueJob", ctx->cbs.continue_job_complete);
}

static void handle_get_file_id_request(dps_ctx_t *ctx) 
{
    dps_param_t result = DPS_RESULT_UNEXECUTED_OP;
    ptp_obj_handle_t eq_file_id = 0;
    static ptp_obj_handle_t autoprt_file_id;
    static char dcf_path[MAX_DCF_PATH_SIZE+1];
    static xml_form_t form[] =
    {
        { "input",      1, 1, NULL, NULL, 0, 0 },
        { "getFileID",  2, 1, NULL, NULL, 0, 0 },
        { "basePathID", 3, 1, convert_hex, (juint32_t*)&autoprt_file_id, 1, 0 },
        { "filePath",   3, 1, convert_str, dcf_path, MAX_DCF_PATH_SIZE, 0 },
        { NULL,         0, 0, NULL, NULL, 0, 0 }
    };

    DECLARE_FNAME("handle_get_file_id_request");
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    init_form(form);

    if (!ctx->cbs.get_file_id_h)
    {
        DBG_E(DPROTO_DPS, ("%s: get_file_id handler not defined!\n", fname));
        goto Exit;
    }

    if (!xml_parse_hierarchy(ctx->printer_msg.xml, form))
        goto Exit;

    result = ctx->cbs.get_file_id_h(ctx->appctx, autoprt_file_id,
        dcf_path, &eq_file_id);

    if (result == DPS_RESULT_OK)
        ptp_object_added(ctx->ptph, eq_file_id);

Exit:
    send_get_file_id_response(ctx, result, eq_file_id);
}

/******************************************************
 *      DPS state machine
 ******************************************************/

static void handle_hrequest(dps_ctx_t *ctx)
{
    DECLARE_FNAME("handle_hrequest");

    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    if (j_strstr(ctx->printer_msg.xml, "notifyJobStatus"))
    {
        handle_job_status(ctx, 1);
        ctx->state = DPS_STATE_NOTIFY_JOB_STAT;
        return;
    }

    if (j_strstr(ctx->printer_msg.xml, "notifyDeviceStatus"))
    {
        handle_device_status(ctx, 1);
        ctx->state = DPS_STATE_NOTIFY_DEV_STAT;
        return;
    }

    if (j_strstr(ctx->printer_msg.xml, "getFileID"))
    {
        handle_get_file_id_request(ctx);
        ctx->state = DPS_STATE_GET_FILE_ID;
        return;
    }

    DBG_W(DPROTO_DPS, ("%s: unknown hrequest (%d)\n", fname));
    send_simple_response(ctx, DPS_RESULT_UNKNOWN_OP, NULL);
}

static void dps_transaction_complete(dps_ctx_t *ctx)
{
    dps_state_t state;
    DECLARE_FNAME("dps_transaction_complete");
    DBG_X(DPROTO_DPS, ("%s: entered\n", fname));

    state = ctx->state;
    ctx->state = DPS_STATE_IDLE;

    /* If it is a running job, we may want to do something other than reset our
     * state to IDLE */

    switch(state)
    {
    case DPS_STATE_DISCOVERY:
        ctx->state = DPS_STATE_IDLE;
        /* Notify application that printer is ready for DPS operations */
        if (ctx->cbs.printer_estab_h)
            ctx->cbs.printer_estab_h(ctx->appctx);
        break;

    case DPS_STATE_CONFIGURE:
        if (ctx->cbs.config_print_service_complete)
            handle_config_response(ctx);
        break;

    case DPS_STATE_GET_CAPABILITY:
        if (ctx->cbs.get_capability_complete)
            handle_get_cap_response(ctx);
        break;

    case DPS_STATE_GET_JOB_STAT:
        if (ctx->cbs.get_job_status_complete)
            handle_job_status(ctx, 0);
        break;

    case DPS_STATE_GET_DEV_STAT:
        if (ctx->cbs.get_device_status_complete)
            handle_device_status(ctx, 0);
        break;

    case DPS_STATE_START_JOB:
        if (ctx->cbs.start_job_complete)
            handle_start_job_complete(ctx);
        break;
            
    case DPS_STATE_ABORT_JOB:
        if (ctx->cbs.abort_job_complete)
            handle_abort_job_complete(ctx);
        break;

    case DPS_STATE_CONTINUE_JOB:
        if (ctx->cbs.continue_job_complete)
            handle_continue_job_complete(ctx);
        break;
        
    case DPS_STATE_NOTIFY_JOB_STAT:
        if (ctx->cbs.notify_job_status_h)
        {
            if (ctx->hreq_result == DPS_RESULT_OK)
                ctx->cbs.notify_job_status_h(ctx->appctx, &ctx->job_status);
            else
                ctx->cbs.notify_job_status_h(ctx->appctx, NULL);
        }
        break;

    case DPS_STATE_NOTIFY_DEV_STAT:
        if (ctx->cbs.notify_device_status_h)
        {
            if (ctx->hreq_result == DPS_RESULT_OK)
                ctx->cbs.notify_device_status_h(ctx->appctx, &ctx->dev_status);
            else
                ctx->cbs.notify_device_status_h(ctx->appctx, NULL);
        }
        break;

    case DPS_STATE_GET_FILE_ID:
        break;

    case DPS_STATE_IDLE:
        /* we probably reached this spot due to an unknown/illegal HREQUEST */
        break;

    default:
        DBG_E(DPROTO_DPS, ("%s: invalid state (%d)\n", fname, ctx->state));
        break;
    }
}

#define IS_TRANS_DSC_TO_PRINTER(tstate)         \
    (tstate >= DPS_TSTATE_DREQUEST &&           \
     tstate <= DPS_TSTATE_HRESPONSE_DATA)

static void ptp_transaction_complete(ptp_sessionh_t sessionh,
    ptp_io_status_t result, ptp_operation_t operation,
    ptp_response_t response, juint32_t obj_size, juint32_t bytes_transferred)
{
    dps_ctx_t *ctx = NULL;
    jbool_t dps_trans_done = 0;
    DECLARE_FNAME("ptp_transaction_complete");

    DBG_V(DPROTO_DPS, ("%s: entered. op=%X, response=%X, objsize=%d,"
        " transferred=%d\n", fname, operation, response, obj_size,
        bytes_transferred));

    if (!sessionh)
    {
        DBG_X(DPROTO_DPS, ("No PTP session yet...\n"));
        return;
    }
    
    ctx = (dps_ctx_t*)PTP_GET_HOOKCTX(sessionh);
    if (!ctx)
    {
        DBG_X(DPROTO_DPS, ("No DPS session context yet...\n"));
        return;
    }
    
    DBG_V(DPROTO_DPS, ("%s: trans_state=%d, dps_state=%d\n", fname,
        ctx->trans_state, ctx->state));

    /* Don't interfere in non DPS sessions */
    if (ctx->state == DPS_STATE_DISCONN)
        goto Exit;

    /* If we failed somewhere, reset our state, nullify the XML buffer to
       induce an error to the application, and complete the DPS transaction */
    if (response != PTP_RESP_OK)
    {
        if (ctx->dsc_msg.id)
            ptp_object_removed(ctx->ptph, ctx->dsc_msg.id);
        if (ctx->printer_msg.id)
            ptp_object_removed(ctx->ptph, ctx->printer_msg.id);
        
        if (ctx->dsc_msg.xml)
            ctx->dsc_msg.xml[0] = 0;
        if (ctx->printer_msg.xml)
            ctx->printer_msg.xml[0] = 0;

        dps_trans_done = 1;
        goto Exit;
    }

    switch(operation)
    {
    case PTP_OP_GET_OBJECT_INFO:
        switch(ctx->trans_state)
        {
        case DPS_TSTATE_DREQUEST:
            if (ctx->dsc_msg.info.compressed_size == 0)
                ctx->trans_state = DPS_TSTATE_HRESPONSE;
            else
                ctx->trans_state = DPS_TSTATE_DREQUEST_DATA;
            break;

        case DPS_TSTATE_DRESPONSE:
            ctx->trans_state = DPS_TSTATE_DRESPONSE_DATA;
            break;

        default:
            break;
        }
        break;

    case PTP_OP_GET_OBJECT:
        switch(ctx->trans_state)
        {
        case DPS_TSTATE_DREQUEST_DATA:
            ctx->trans_state = DPS_TSTATE_HRESPONSE;
            ptp_object_removed(ctx->ptph, ctx->dsc_msg.id);
            break;

        case DPS_TSTATE_DRESPONSE_DATA:
            dps_trans_done = 1;
            ptp_object_removed(ctx->ptph, ctx->dsc_msg.id);
            break;

        default:
            break;
        }
        break;

    case PTP_OP_SEND_OBJECT_INFO:
        if (IS_TRANS_DSC_TO_PRINTER(ctx->trans_state) &&
            ctx->printer_msg.type == DPS_REQUEST)
        {
            /* Collision */
            ctx->collision = 1;
            DBG_V(DPROTO_DPS, ("%s: COLLISION detected. Ignoring data\n", fname));
        }

        switch(ctx->trans_state)
        {
        case DPS_TSTATE_IDLE:
            ctx->trans_state = DPS_TSTATE_HREQUEST_DATA;
            break;

        case DPS_TSTATE_HRESPONSE:
            if (ctx->printer_msg.info.compressed_size == 0)
            {
                dps_trans_done = 1;
                ptp_object_removed(ctx->ptph, ctx->printer_msg.id);
            }
            else
                ctx->trans_state = DPS_TSTATE_HRESPONSE_DATA;
            break;

        default:
            DBG_E(DPROTO_DPS, ("%s: invalid state\n", fname));
            break;
        }
        break;

    case PTP_OP_SEND_OBJECT:
        /* NULL terminate our newly arrived XML message */
        ctx->printer_msg.xml[ctx->printer_msg.info.compressed_size] = 0;
        
        switch(ctx->trans_state)
        {
        case DPS_TSTATE_HRESPONSE_DATA:
            if (ctx->collision)
            {
                /* Reaching this point means we've just ignored an HREQUEST
                 * which caused a collision - reset our state to receive the
                 * expected HRESPONSE */
                    DBG_V(DPROTO_DPS, ("%s: Collision ended. HREQUEST discarded."
                    " Waiting for HRESPONSE\n", fname));
                ctx->trans_state = DPS_TSTATE_HRESPONSE;
                ctx->collision = 0;
            }
            else
                dps_trans_done = 1;
            ptp_object_removed(ctx->ptph, ctx->printer_msg.id);
            break;

        case DPS_TSTATE_HREQUEST_DATA:
            ctx->trans_state = DPS_TSTATE_DRESPONSE;
            ptp_object_removed(ctx->ptph, ctx->printer_msg.id);
            handle_hrequest(ctx);
            break;

        default:
            DBG_E(DPROTO_DPS, ("%s: invalid state\n", fname));
            break;
        }
        break;

    default:
        break;
    }

Exit:
    if (ctx->ptp_app_cbs.transaction_complete_h)
    {
        ctx->ptp_app_cbs.transaction_complete_h(sessionh, result, operation,
            response, obj_size, bytes_transferred);
    }

    if (dps_trans_done)
    {
        ctx->trans_state = DPS_TSTATE_IDLE;
        ctx->dsc_msg.id = 0;
        ctx->printer_msg.id = 0;
        ctx->collision = 0;
        dps_transaction_complete(ctx);
    }
}


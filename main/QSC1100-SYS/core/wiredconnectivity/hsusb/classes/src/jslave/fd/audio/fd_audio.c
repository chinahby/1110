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

#include <jusb_common.h>
#include <jusb_core_fd.h>
#include <jusb_chp9.h>
#include "fd_audio.h"

#define NOT_IMPLEMENTED DBG_E(DSLAVE_AUDIO,("Feature not implemented yet:(\n"))

static jresult_t exec_app_req_cb(fd_audio_h fd);
static void ep0_read_cb(request_t *core_req);
static void ep0_send_cb(request_t *core_req);

static jresult_t check_config(uwd_audio_config_t *config);

static jresult_t fd_desc_init(fd_audio_h fd);
static void fd_desc_uninit(fd_audio_h fd);

static jresult_t ctrl_iface_init(fd_audio_h fd, interface_desc_t *iface);
static void ctrl_iface_uninit(fd_audio_h fd, interface_desc_t *iface);

static void intr_pipe_init(fd_audio_h fd, pipe_desc_t *pipe);
static jresult_t audio_send_intr(fd_audio_h fd, audio_intr_data_t *data);
static void intr_send_cb(request_t *core_req);

static juint32_t header_desc_size(fd_audio_h fd);
static void header_desc_init(fd_audio_h fd, juint16_t total_len, void *buf);

static jresult_t entity_check_req(fd_audio_h fd);

static void input_trm_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf);

static void output_trm_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf);

static juint32_t feature_unit_get_desc_size(uwd_audio_feature_unit_t *fu);
static void feature_unit_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf);

static juint32_t selector_unit_get_desc_size(uwd_audio_selector_unit_t *su);
static void selector_unit_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf);

static juint32_t extension_unit_get_desc_size(uwd_audio_extension_unit_t *eu);
static void extension_unit_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf);

static jresult_t entity_index_init(fd_audio_h fd);
static jresult_t get_entity(fd_audio_h fd, juint8_t id, uwd_audio_entity_h *ent,
    entity_priv_h *ent_p);
static juint32_t entity_get_desc_size(uwd_audio_entity_h ent);

static jresult_t entity_init(fd_audio_h fd, juint8_t index, void *buf);
static void entities_uninit(fd_audio_h fd);

static jresult_t get_ep_stream(fd_audio_h fd, juint8_t ep_addr,
    uwd_audio_stream_h *stream);
static jresult_t get_stream(fd_audio_h fd, juint8_t iface_index,
    uwd_audio_stream_h *stream);
static stream_priv_h get_stream_priv(fd_audio_h fd, uwd_audio_stream_h stream);

static jresult_t stream_check_req(fd_audio_h fd);

static jresult_t stream_iface_init(fd_audio_h fd, juint_t num);
static void stream_iface_uninit(fd_audio_h fd, juint_t if_num, juint_t alts);

static jresult_t stream_alt_init(fd_audio_h fd, alt_interface_desc_t *desc,
    uwd_audio_stream_alt_t *stream_alt, stream_priv_h stream_p);
static void stream_alt_uninit(fd_audio_h fd,  alt_interface_desc_t *desc,
    uwd_audio_stream_alt_t *stream_alt);

static void stream_iface_desc_init(uwd_audio_stream_alt_h stream_alt,
    void *buf);
static void format_get_samp_freq(uwd_audio_stream_alt_h stream_alt,
    uwd_audio_freq_discrete_t **discrete, uwd_audio_freq_continuous_t **cont);

static juint16_t format_get_audio_frame_size(uwd_audio_stream_alt_h stream_alt);
static juint32_t format_desc_size(uwd_audio_stream_alt_h stream_alt);
static void format_desc_init(uwd_audio_stream_alt_h stream_alt, void *buf);

static juint32_t format_I_desc_size(uwd_audio_stream_alt_h stream_alt);
static void format_I_desc_init(uwd_audio_stream_alt_h stream_alt, void *buf);

static juint32_t samp_freq_desc_size(uwd_audio_freq_discrete_t *discrete,
    uwd_audio_freq_continuous_t *cont);
static void samp_freq_desc_init(uwd_audio_freq_discrete_t *discrete,
    uwd_audio_freq_continuous_t *cont, void *buf);

static jresult_t handle_samp_freq_req(fd_audio_h fd);

static jbool_t get_samp_freq_mult(uwd_audio_stream_alt_h stream_alt);
static void get_samp_freq_range(uwd_audio_stream_alt_h stream_alt,
    juint32_t *min, juint32_t *max);

static juint32_t audio_samp_freq_to_uint32(audio_samp_freq_t *freq);
static void audio_uint32_to_samp_freq(juint32_t freq_hz,
    audio_samp_freq_t *freq);

static jresult_t ep_check_req(fd_audio_h fd);
static jresult_t isoc_pipe_init(fd_audio_h fd, pipe_desc_t *desc,
    uwd_audio_stream_alt_t *stream_alt, stream_priv_h stream_p);
static void audio_isoc_ep_desc_init(jbool_t set_freq_support, void *buf);

static jresult_t stream_restart(fd_audio_h fd, uwd_audio_stream_h stream,
    stream_priv_h stream_p);

static jresult_t stream_restart_freq(fd_audio_h fd, uwd_audio_stream_h stream,
    stream_priv_h stream_p, juint32_t samp_freq);

static jresult_t stream_reschedule(fd_audio_h fd, stream_priv_h stream_p);

static void stream_complete_cb(request_t *req);

static void complete_bufs(fd_audio_h fd, stream_priv_h stream_p,
    juint32_t req_len, juint32_t done_len, jresult_t stat, jbool_t need_copy,
    juint8_t *req_vaddr);

static void stream_close(fd_audio_h fd, uwd_audio_stream_h stream,
    stream_priv_h stream_p);

static void stream_clean(stream_priv_h stream_p);

static jresult_t request_status_to_result(request_status_t stat);

static juint16_t get_next_frame_len(stream_priv_h stream_p);

#ifdef JDEBUG

#define UNDEFINED_VAL 0xffff
#define UNDEFINED_MASK 0

typedef struct {
    jint_t code;
    char *string;
} code2string_t;

static char *code2str(jint_t code, code2string_t codes_list[])
{
    code2string_t *cs;

    for (cs = &codes_list[0]; cs->code != UNDEFINED_VAL && cs->code != code;
        cs++)
        ;

    return cs->string;
}

typedef struct {
    juint32_t mask;
    char *string;
} bitmap2string_t;

static void print_bitmap(juint32_t bitmap, bitmap2string_t bits_list[],
    char  *format)
{
    /* HSU temp fix  -  jprintf doesn't compile, C2432E: non-constant initialiser */
    /*bitmap2string_t *bs;

    for (bs = &bits_list[0]; bs->mask; bs++)
    {
        if (bs->mask & bitmap)
            jprintf(fmt, string_to_print);
    }*/

    /* HSU temp fix */

    jprintf("\n");
}

#define ESTR(err) code2str(err, error_str)
static code2string_t error_str[] = {
    {0,             "OK"},
    {JEPERM,         "Operation not permitted"},
    {JENOENT,        "No such file or directory"},
    {JEINTR,         "Interrupted system call"},
    {JEIO,           "Input/output error"},
    {JENXIO,         "Device not configured"},
    {JE2BIG,         "Argument list too long"},
    {JEAGAIN,        "Try Again"},
    {JENOMEM,        "Cannot allocate memory"},
    {JEACCES,        "Permission denied"},
    {JEBUSY,         "Device busy"},
    {JENODEV,        "Operation not supported by device"},
    {JEINVAL,        "Invalid argument"},
    {JENOTTY,        "Inappropriate ioctl for device"},
    {JEFBIG,         "File too large"},
    {JENOSPC,        "No space left on device"},
    {JEWOULDBLOCK,   "Operation would block"},
    {JERESTART,      "restart syscall"},
    {JEOPNOTSUPP,    "Operation not supported"},
    {JENOTSUP,       "Operation not supported"},
    {JETIMEDOUT,     "Operation timed out"},
    {JENOMEDIUM,     "No medium found"},
    {ESUSPENDED,    "Device is suspended"},
    {EUNKNOWN,      "General error"},
    {JETEST_FAILED,  "Test failed"},
    {UNDEFINED_VAL, "Unknown error"}
};

#define ENTITY_TYPE(ent_type) code2str(ent_type, entity_type_str)
static code2string_t entity_type_str[] = {
    {UWD_AUDIO_ET_INPUT_TERMINAL, "INPUT_TERMINAL"},
    {UWD_AUDIO_ET_OUTPUT_TERMINAL,"OUTPUT_TERMINAL"},
    {UWD_AUDIO_ET_MIXER_UNIT,     "MIXER_UNIT"},
    {UWD_AUDIO_ET_SELECTOR_UNIT,  "SELECTOR_UNIT"},
    {UWD_AUDIO_ET_FEATURE_UNIT,   "FEATURE_UNIT"},
    {UWD_AUDIO_ET_PROCESSING_UNIT,"PROCESSING_UNIT"},
    {UWD_AUDIO_ET_EXTENSION_UNIT, "EXTENSION_UNIT"},
    {UNDEFINED_VAL, "UNDEFINED ENTITY"}
};

#define TRM_TYPE(trm_type) code2str(trm_type, trm_type_str)
static code2string_t trm_type_str[] = {
    {UWD_AUDIO_TT_USB_UNDEFINED,                "USB_UNDEFINED"},
    {UWD_AUDIO_TT_USB_STREAMING,                "USB_STREAMING"},
    {UWD_AUDIO_TT_USB_VENDOR_SPECIFIC,          "USB_VENDOR_SPECIFIC"},
    {UWD_AUDIO_TT_INPUT_UNDEFINED,              "INPUT_UNDEFINED"},
    {UWD_AUDIO_TT_MICROPHONE,                   "MICROPHONE"},
    {UWD_AUDIO_TT_DESKTOP_MICROPHONE,           "DESKTOP_MICROPHONE"},
    {UWD_AUDIO_TT_PERSONAL_MICROPHONE,          "PERSONAL_MICROPHONE"},
    {UWD_AUDIO_TT_OMNI_DIRECTIONAL_MICROPHONE,  "OMNI_DIRECTIONAL_MICROPHONE"},
    {UWD_AUDIO_TT_MICROPHONE_ARRAY,             "MICROPHONE_ARRAY"},
    {UWD_AUDIO_TT_PROCESSING_MICROPHONE_ARRAY,  "PROCESSING_MICROPHONE_ARRAY"},
    {UWD_AUDIO_TT_OUTPUT_UNDEFINED,             "OUTPUT_UNDEFINED"},
    {UWD_AUDIO_TT_SPEAKER,                      "SPEAKER"},
    {UWD_AUDIO_TT_HEADPHONES,                   "HEADPHONES"},
    {UWD_AUDIO_TT_HEAD_MOUNTED_DISPLAY_AUDIO,   "HEAD_MOUNTED_DISPLAY_AUDIO"},
    {UWD_AUDIO_TT_DESKTOP_SPEAKER,              "DESKTOP_SPEAKER"},
    {UWD_AUDIO_TT_ROOM_SPEAKER,                 "ROOM_SPEAKER"},
    {UWD_AUDIO_TT_COMMUNICATION_SPEAKER,        "COMMUNICATION_SPEAKER"},
    {UWD_AUDIO_TT_LOW_FREQUENCY_EFFECTS_SPEAKER,
        "LOW_FREQUENCY_EFFECTS_SPEAKER"},
    {UNDEFINED_VAL, "UNDEFINED"}
};

#define REQ_CODE(code) code2str(code, req_code_str)
static code2string_t req_code_str[] = {
    {UWD_AUDIO_RC_SET_CUR, "SET_CUR"},
    {UWD_AUDIO_RC_GET_CUR, "GET_CUR"},
    {UWD_AUDIO_RC_SET_MIN, "SET_MIN"},
    {UWD_AUDIO_RC_GET_MIN, "GET_MIN"},
    {UWD_AUDIO_RC_SET_MAX, "SET_MAX"},
    {UWD_AUDIO_RC_GET_MAX, "GET_MAX"},
    {UWD_AUDIO_RC_SET_RES, "SET_RES"},
    {UWD_AUDIO_RC_GET_RES, "GET_RES"},
    {UWD_AUDIO_RC_SET_MEM, "SET_MEM"},
    {UWD_AUDIO_RC_GET_MEM, "GET_MEM"},
    {UWD_AUDIO_RC_GET_STAT,"GET_STAT"},
    {UNDEFINED_VAL, "UNDEFINED"}
};

#define FMT_TYPE(type) code2str(type, format_type_str)
static code2string_t format_type_str[] = {
    {UWD_AUDIO_DF_I_UNDEFINED,              "FORMAT_I_UNDEFINED"},
    {UWD_AUDIO_DF_I_PCM,                    "PCM"},
    {UWD_AUDIO_DF_I_PCM8,                   "PCM8"},
    {UWD_AUDIO_DF_I_IEEE_FLOAT,             "IEEE_FLOAT"},
    {UWD_AUDIO_DF_I_ALAW,                   "ALAW"},
    {UWD_AUDIO_DF_I_MULAW,                  "MULAW"},
    {UWD_AUDIO_DF_II_UNDEFINED,             "FORMAT_II_UNDEFINED"},
    {UWD_AUDIO_DF_II_MPEG,                  "MPEG"},
    {UWD_AUDIO_DF_II_AC_3,                  "AC_3"},
    {UWD_AUDIO_DF_III_UNDEFINED,            "FORMAT_III_UNDEFINED"},
    {UWD_AUDIO_DF_III_IEC1937_AC_3,         "IEC1937_AC_3"},
    {UWD_AUDIO_DF_III_IEC1937_MPEG_1_L1,    "IEC1937_MPEG_1_L1"},
    {UWD_AUDIO_DF_III_IEC1937_MPEG_1_L2_L3, "IEC1937_MPEG_1_L2_L3"},
    {UWD_AUDIO_DF_III_IEC1937_MPEG_2_NOEXT, "IEC1937_MPEG_2_NOEXT"},
    {UWD_AUDIO_DF_III_IEC1937_MPEG_2_EXT,   "IEC1937_MPEG_2_EXT"},
    {UWD_AUDIO_DF_III_IEC1937_MPEG_2_L1,    "IEC1937_MPEG_2_L1"},
    {UWD_AUDIO_DF_III_IEC1937_MPEG_2_L2_L3, "IEC1937_MPEG_2_L2_L3"},
    {UNDEFINED_VAL, "UNDEFINED"}
};

static bitmap2string_t channel_config_str[] = {
    {UWD_AUDIO_CC_LEFT_FRONT,           "LEFT_FRONT"},
    {UWD_AUDIO_CC_RIGHT_FRONT,          "RIGHT_FRONT"},
    {UWD_AUDIO_CC_CENTER_FRONT,         "CENTER_FRONT"},
    {UWD_AUDIO_CC_LOW_FREQ_ENHANCEMENT, "LOW_FREQ_ENHANCEMENT"},
    {UWD_AUDIO_CC_LEFT_SURROUND,        "LEFT_SURROUND"},
    {UWD_AUDIO_CC_RIGHT_SURROUND,       "RIGHT_SURROUND"},
    {UWD_AUDIO_CC_LEFT_OF_CENTER,       "LEFT_OF_CENTER"},
    {UWD_AUDIO_CC_RIGHT_OF_CENTER,      "RIGHT_OF_CENTER"},
    {UWD_AUDIO_CC_SURROUND,             "SURROUND"},
    {UWD_AUDIO_CC_SIDE_LEFT,            "SIDE_LEFT"},
    {UWD_AUDIO_CC_SIDE_RIGHT,           "SIDE_RIGHT"},
    {UWD_AUDIO_CC_TOP,                  "TOP"},
    {UNDEFINED_MASK, ""}
};

static bitmap2string_t fu_controls_str[] = {
    {UWD_AUDIO_FC_MUTE,             "MUTE"},
    {UWD_AUDIO_FC_VOLUME,           "VOLUME"},
    {UWD_AUDIO_FC_BASS,             "BASS"},
    {UWD_AUDIO_FC_MID,              "MID"},
    {UWD_AUDIO_FC_TREBLE,           "TREBLE"},
    {UWD_AUDIO_FC_GRAPHIC_EQUALIZER,"GRAPHIC_EQUALIZER"},
    {UWD_AUDIO_FC_AUTOMATIC_GAIN,   "AUTOMATIC_GAIN"},
    {UWD_AUDIO_FC_DELAY,            "DELAY"},
    {UWD_AUDIO_FC_BASS_BOOST,       "BASS_BOOST"},
    {UWD_AUDIO_FC_LOUDNESS,         "LOUDNESS"},
    {UNDEFINED_MASK, ""}
};

static code2string_t req_type_str[] = {
    {AR_ENDPOINT,         "EP"},
    {AR_CTRL_INTERFACE,   "CTRL"},
    {AR_STREAM_INTERFACE, "STREAM"},
    {UNDEFINED_VAL, "UNDEFINED"}
};

static void dump_req(audio_req_t *req)
{
    jprintf("Request %s %s value=%04x len=%d\n",
        code2str(req->type, req_type_str), REQ_CODE(req->code), req->value,
        req->len);
}

#endif

/**
 * Function name:  control_msg
 * Description:    Audio function specific requests callback, called by core.
 * Parameters:
 *     @context: (IN) Audio FD context.
 *     @buffer:  (IN) Control request data.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t control_msg(void *context, void *buffer)
{
    jresult_t rc = 0;
    fd_audio_h fd = (fd_audio_h)context;
    struct usb_ctrlrequest *req = (struct usb_ctrlrequest *)buffer;
    juint8_t recip;
    juint16_t index;

    DBG_I(DSLAVE_AUDIO_CTRL, ("AUDIO: control_msg\n"));

    recip = req->bRequestType & USB_RECIP_MASK;
    index = ua_le16toh(&req->wIndex);

    fd->req.code = (uwd_audio_req_code_t)req->bRequest;
    fd->req.value = ua_le16toh(&req->wValue);
    fd->req.len = ua_le16toh(&req->wLength);

    switch (recip)
    {
    case USB_RECIP_INTERFACE:
        if ((juint8_t)index == fd->ctrl_iface_index)
        {
            fd->req.type = AR_CTRL_INTERFACE;

            rc = get_entity(fd, index >> 8, &fd->req.recip.entity, NULL);
            if (rc)
                goto Error;

            rc = entity_check_req(fd);
        }
        else
        {
            fd->req.type = AR_STREAM_INTERFACE;

            rc = get_stream(fd, (juint8_t)index, &fd->req.recip.stream);
            if (rc)
                goto Error;

            rc = stream_check_req(fd);
        }
        break;

    case USB_RECIP_ENDPOINT:
        fd->req.type = AR_ENDPOINT;

        rc = get_ep_stream(fd, (juint8_t)index, &fd->req.recip.stream);
        if (rc)
            goto Error;

        rc = ep_check_req(fd);
        break;

    default:
        rc = JENOTSUP;
        break;
    }

    if (rc)
        goto Error;

    fd->req.core_req = core_request_alloc(fd->req.len, NULL);
    if (!fd->req.core_req)
    {
        rc = JENOMEM;
        goto Error;
    }

    fd->req.core_req->transfer_size = fd->req.len;
    fd->req.core_req->caller_private = fd;

    DBG_RUN(DSLAVE_AUDIO_CTRL, DL_EX_VERBOSE, dump_req(&fd->req));

    if (req->bRequestType & USB_DIR_IN)
    {
        rc = exec_app_req_cb(fd);
        if (rc)
            goto Error;

        fd->req.core_req->complete = ep0_send_cb;
        rc = core_send_data(fd->core_ctx, NULL, fd->req.core_req);
    }
    else
    {
        fd->req.core_req->complete = ep0_read_cb;
        rc = core_read_data(fd->core_ctx, NULL, fd->req.core_req);
    }

    if (rc)
        goto Error;

    return 0;

Error:
    if (fd->req.core_req)
    {
        core_request_free(fd->req.core_req);
        fd->req.core_req = NULL;
    }

    DBG_E(DSLAVE_AUDIO_CTRL, ("AUDIO: EP0 request error - %s\n", ESTR(rc)));
    return rc;
}

/**
 * Function name:  exec_app_req_cb
 * Description:    Executes application requests callbacks or
 *                 SAMPLING_FREQ_CONTROL handler.
 * Parameters:
 *     @fd: (IN) Audio FD context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t exec_app_req_cb(fd_audio_h fd)
{
    jresult_t rc = 0;

    switch (fd->req.type)
    {
    case AR_CTRL_INTERFACE:

        rc = fd->req.recip.entity->req_cb(fd->app_ctx, fd->req.recip.entity, 
            fd->req.code, fd->req.value, fd->req.len, 
            fd->req.core_req->buffer.vaddr);
        break;
    case AR_STREAM_INTERFACE:

        rc = fd->req.recip.stream->req_cb(fd->app_ctx, fd->req.recip.stream, 
            fd->req.code, fd->req.value, fd->req.len,
            fd->req.core_req->buffer.vaddr);
        break;
    case AR_ENDPOINT:
        {
            uwd_audio_endpoint_cs_t cs =
                (uwd_audio_endpoint_cs_t)(fd->req.value >> 8);

            if (cs == UWD_AUDIO_EP_SAMPLING_FREQ_CONTROL)
            {
                rc = handle_samp_freq_req(fd);
            }
            else
            {
                rc = fd->req.recip.stream->ep_req_cb(fd->app_ctx,
                    fd->req.recip.stream, fd->req.code, cs, fd->req.len,
                    fd->req.core_req->buffer.vaddr);
            }
        }
        break;
    }

    if (rc)
    {
        DBG_E(DSLAVE_AUDIO_CTRL, ("AUDIO: request failed, %s\n", ESTR(rc)));
        DBG_RUN(DSLAVE_AUDIO_CTRL, DL_ERROR, dump_req(&fd->req));
    }

    return rc;
}

/**
 * Function name:  ep0_read_cb
 * Description:    Control pipe read completion callback.
 *                 (Used for host OUT requests).
 * Parameters:
 *     @core_req: (IN) Pointer to a core request structure.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void ep0_read_cb(request_t *core_req)
{
    fd_audio_h fd = (fd_audio_h)core_req->caller_private;
    jresult_t rc;

    if (core_req->status != REQUEST_COMPLETED)
    {
        DBG_E(DSLAVE_AUDIO_CTRL, ("AUDIO: Read request failed, request status "
            "%d\n", core_req->status));
        goto Error;
    }

    rc = exec_app_req_cb(fd);
    if (rc)
    {
        core_stall_pipe(fd->core_ctx, NULL, 1);
        DBG_E(DSLAVE_AUDIO_CTRL, ("AUDIO: Read request application error - "
            "%s\n", ESTR(rc)));
        goto Error;
    }

    /* Sending status stage */
    core_req->complete      = ep0_send_cb;
    core_req->transfer_size = 0;
    core_req->status        = REQUEST_READY;

    rc = core_send_data(fd->core_ctx, 0, core_req);
    if (rc)
    {
        DBG_E(DSLAVE_AUDIO_CTRL, ("AUDIO: Send request status failed - %s\n", 
            ESTR(rc)));
        goto Error;
    }

    return;

Error:
    core_request_free(core_req);
    fd->req.core_req = NULL;
}

/**
 * Function name:  ep0_send_cb
 * Description:    Control pipe send completion callback.
 *                 (Used for the status stage of control requests).
 * Parameters:
 *     @core_req: (IN) Pointer to a core request structure.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void ep0_send_cb(request_t *core_req)
{
    fd_audio_h fd = (fd_audio_h)core_req->caller_private;

    if (core_req->status != REQUEST_COMPLETED)
    {
        DBG_E(DSLAVE_AUDIO_CTRL, ("AUDIO: Request failed, request status %d\n",
            core_req->status));
    }

    core_request_free(core_req);
    fd->req.core_req = NULL;
}

/**
 * Function name:  get_pipe_config
 * Description:    Returns the audio function driver's USB descriptors.
 * Parameters:
 *     @context: (IN) Audio FD context.
 *
 * Return value: Pointer to an FD descriptor.
 * Scope:        Local
 **/
static fd_desc_t *get_pipe_config(context_t context)
{
    fd_audio_h fd = (fd_audio_h)context;

    return fd->desc;
}

/**
 * Function name:  handle_iface_change
 * Description:    Handles interface alternate setting changes.
 * Parameters:
 *     @fd:      (IN) Audio FD context.
 *     @disable: (IN) Disable streaming.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void handle_iface_change(fd_audio_h fd, jbool_t disable)
{
    juint_t i;

    for (i = 0; i < fd->config->streams_num; i++)
    {
        juint8_t alt = disable ? 0 : fd->desc->interfaces[i+1].alt_if_curr;
        stream_priv_h stream_p = &fd->stream_priv[i];

        if (stream_p->alt != alt)
        {
            uwd_audio_stream_h stream;

            stream_p->alt = alt;

            stream = &fd->config->streams[i];

            DBG_I(DSLAVE_AUDIO_STREAM, ("AUDIO: SetInterface %d, AltSet=%d\n",
                fd->desc->interfaces[i+1].number, alt));

            if (alt)
                stream_restart(fd, stream, stream_p);
            else
                stream_close(fd, stream, stream_p);
        }
    }
}

/**
 * Function name:  enable
 * Description:    Enables the audio function driver.
 * Parameters:
 *     @context: (IN) Audio FD context.
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t enable(context_t context)
{
    fd_audio_h fd = (fd_audio_h)context;

    DBG_I(DSLAVE_AUDIO, ("AUDIO: enable\n"));

    fd->state = STATE_ENABLED;

    handle_iface_change(fd, 0);

    return 0;
}

/**
 * Function name:  disable
 * Description:    Disables the audio function driver.
 * Parameters:
 *     @context: (IN) Audio FD context.
 *
 * Return value: None
 * Scope:        Local
 **/
static void disable(void *context)
{
    fd_audio_h fd = (fd_audio_h)context;

    DBG_I(DSLAVE_AUDIO, ("AUDIO: disable\n"));

    handle_iface_change(fd, 1);

    fd->state = STATE_DISABLED;
}

/**
 * Function name:  suspend
 * Description:    Called when bus SUSPEND is received from the host.
 *                 Disables the function driver.
 * Parameters:
 *     @context: (IN) Audio FD context.
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t suspend(void *context)
{
    fd_audio_h fd = (fd_audio_h)context;

    DBG_I(DSLAVE_AUDIO, ("AUDIO: suspend\n"));

    fd->prev_state = fd->state;

    if (fd->state == STATE_ENABLED)
        disable(context);

    return 0;
}

/**
 * Function name:  resume
 * Description:    Called when bus RESUME or RESET is received from the host.
 *                 Enables the function driver.
 * Parameters:
 *     @context: (IN) Audio FD context.
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t resume(void *context)
{
    fd_audio_h fd = (fd_audio_h)context;
    jresult_t rc = 0;

    DBG_I(DSLAVE_AUDIO, ("AUDIO: resume\n"));

    if (fd->prev_state == STATE_ENABLED)
    {
        rc = enable(context);
        if (rc)
            DBG_E(DSLAVE_AUDIO, ("AUDIO: Failed to resume FD, error %d\n", rc));
    }
    return rc;
}

/**
 * Function name:  uninit
 * Description:    Un-initializes the audio function driver.
 * Parameters:
 *     @context: (IN) Audio FD context.
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static jresult_t uninit(context_t context)
{
    fd_audio_h fd = (fd_audio_h)context;

    DBG_I(DSLAVE_AUDIO, ("AUDIO: uninit\n"));

    if (fd->state == STATE_ENABLED)
        disable(fd);

    if (fd->flags & AUDIO_DESC_INIT)
        fd_desc_uninit(fd);

    if (fd->flags & AUDIO_APP_INIT)
        fd->config->uninit(fd->app_ctx);

    jfree(fd);

    return 0;
}

static fd_ops_t fd_ops = {
    get_pipe_config,
    enable,
    disable,
    suspend,
    resume,
    uninit
};

/**
 * Function name:  fd_audio_init
 * Description:    Initializes the audio function driver.
 * Parameters:
 *     @ctx: (IN) Core context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t fd_audio_init(void *ctx)
{
    jresult_t rc;
    fd_audio_h fd;

    DBG_I(DSLAVE_AUDIO, ("AUDIO: fd_audio_init\n"));

    fd = (fd_audio_h)jmalloc(sizeof(fd_audio_t), M_ZERO);
    if (!fd)
        return JENOMEM;

    fd->core_ctx = ctx;
    fd->state = STATE_DISABLED;

    rc = uwd_audio_app_init(fd, &fd->app_ctx, &fd->config);
    if (rc)
        goto Error;

    fd->flags |= AUDIO_APP_INIT;

    rc = check_config(fd->config);
    if (rc)
        goto Error;

    fd->ctrl_iface_index = core_get_available_iface(ctx);

    rc = fd_desc_init(fd);
    if (rc)
        goto Error;

    fd->flags |= AUDIO_DESC_INIT;

    rc = core_register_fd(fd->core_ctx, &fd_ops, (context_t)fd);
    if (rc)
        goto Error;

    return 0;

Error:
    DBG_E(DSLAVE_AUDIO, ("AUDIO: fd_audio_init failed, error %d\n", rc));

    uninit(fd);

    return rc;
}

#define RETURN_ERR(msg) \
    do {\
        DBG_E(DSLAVE_AUDIO_DESC, msg);\
        return JEINVAL;\
    } while (0)

/**
 * Function name:  check_config
 * Description:    Checks audio device configuration data provided by
 *                 the application.
 * Parameters:
 *     @config: (IN) Audio device configuration data.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t check_config(uwd_audio_config_t *config)
{
    juint_t i, j;
    uwd_audio_entity_h ent;
    uwd_audio_stream_h stream;
    uwd_audio_stream_alt_h alt;

    DBG_I(DSLAVE_AUDIO_DESC, ("AUDIO: check_config\n"));

    if (!config->entities_num)
        RETURN_ERR(("AUDIO: Config error - entities number is 0\n"));

    if (!config->entities)
        RETURN_ERR(("AUDIO: Config error - 'entities' is NULL\n"));

    if (config->streams_num && !config->streams)
        RETURN_ERR(("AUDIO: Config error - streams is NULL\n"));

    for (i = 0, ent = config->entities; i < config->entities_num; i++, ent++)
    {
        if (!ent->id)
            RETURN_ERR(("AUDIO: Config error - entity(%d) 'id' is 0\n", i));

        if (!ent->data)
            RETURN_ERR(("AUDIO: Config error - entity(%d) 'data' is NULL\n",i));
    }

    for (i = 0, stream = config->streams; i < config->streams_num; i++,stream++)
    {
        if (!stream->select_cb)
        {
            RETURN_ERR(("AUDIO: Config error - stream(%d) 'select_cb' is NULL"
                "\n", i));
        }

        if (stream->alts_num && !stream->alts)
            RETURN_ERR(("AUDIO: Config error - stream(%d) 'alts' is NULL\n",i));

        for (j = 0, alt = stream->alts; j < stream->alts_num; j++, alt++)
        {
            if (!alt->format)
            {
                RETURN_ERR(("AUDIO: Config error - stream(%d) alt(%d) format "
                    "is NULL\n", i, j));
            }
        }
    }

    return 0;
}

/**
 * Function name:  fd_desc_init
 * Description:    Initializes audio descriptors and FD internal data.
 * Parameters:
 *     @fd:   (IN) Audio FD context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t fd_desc_init(fd_audio_h fd)
{
    jresult_t rc = 0;
    juint_t i;

    DBG_I(DSLAVE_AUDIO_DESC, ("AUDIO: fd_desc_init\n"));

    fd->desc = (fd_desc_t *)jmalloc(sizeof(fd_desc_t), M_ZERO);

    fd->desc->speed = SPEED_HIGH;
    fd->desc->interface_count = fd->config->streams_num + 1;

    fd->desc->interfaces = (interface_desc_t *)
        jmalloc(sizeof(interface_desc_t) * fd->desc->interface_count, M_ZERO);

    fd->entity_priv = (entity_priv_t *)
        jmalloc(sizeof(entity_priv_t) * fd->config->entities_num, M_ZERO);

    fd->stream_priv = (stream_priv_t *)
        jmalloc(sizeof(stream_priv_t) * fd->config->streams_num, M_ZERO);

    if (!fd->desc || !fd->desc->interfaces || !fd->entity_priv ||
        !fd->stream_priv)
    {
        rc = JENOMEM;
        goto Error;
    }

    rc = entity_index_init(fd);
    if (rc)
        goto Error;

    rc = ctrl_iface_init(fd, fd->desc->interfaces);

    for (i = 0; !rc && i < fd->config->streams_num; i++)
        rc = stream_iface_init(fd, i);

    if (rc)
        goto Error;

    return 0;

Error:
    DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: fd_desc_init failed, error - %d\n", rc));

    fd_desc_uninit(fd);

    return rc;
}

/**
 * Function name:  fd_desc_uninit
 * Description:    Frees audio descriptors memory.
 * Parameters:
 *     @fd:      (IN) Audio FD context.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void fd_desc_uninit(fd_audio_h fd)
{
    DBG_I(DSLAVE_AUDIO_DESC, ("AUDIO: fd_desc_uninit\n"));

    if (fd->desc)
    {
        if (fd->desc->interfaces)
        {
            juint_t i;

            if (fd->desc->interfaces[0].if_class == AUDIO_CLASS)
            {
                ctrl_iface_uninit(fd, &fd->desc->interfaces[0]);

                for (i = 0; i < fd->config->streams_num; i++)
                {
                    if (fd->desc->interfaces[i+1].if_class != AUDIO_CLASS)
                        break;

                    stream_iface_uninit(fd, i, fd->config->streams[i].alts_num);
                }
            }

            jfree(fd->desc->interfaces);
        }

        jfree(fd->desc);
    }

    if (fd->entity_priv)
        jfree(fd->entity_priv);

    if (fd->stream_priv)
        jfree(fd->stream_priv);

    if (fd->entity_index)
        jfree(fd->entity_index);
}

/**
 * Function name:  ctrl_iface_init
 * Description:    Audio control interface init.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *     @iface: (OUT) Audio control interface descriptor.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t ctrl_iface_init(fd_audio_h fd, interface_desc_t *iface)
{
    juint_t i;
    juint32_t size;
    jresult_t rc = 0;
    juint8_t *buf;

    iface->control_msg = control_msg;
    iface->alt_if_num  = 1;
    iface->if_subclass = AUDIO_SUBCLASS_CONTROL;
    iface->number = UWD_UNDEFINED_INTERFACE;

    iface->alt_ifs = (alt_interface_desc_t *)
        jmalloc(sizeof(alt_interface_desc_t), M_ZERO);

    if (!iface->alt_ifs)
        goto NoMem;

    /* Create and init interrupt pipe descriptor */
    if (fd->config->intr_ep_ival != UWD_AUDIO_NO_DATA)
    {
        iface->alt_ifs->pipe_count = 1;
        iface->alt_ifs->pipes = (pipe_desc_t *)
            jmalloc(sizeof(pipe_desc_t), M_ZERO);

        if (!iface->alt_ifs->pipes)
            goto NoMem;

        intr_pipe_init(fd, iface->alt_ifs->pipes);
    }

    /* Calculate extra descriptors size */
    size = header_desc_size(fd);

    for (i = 0; i < fd->config->entities_num; i++)
        size += entity_get_desc_size(fd->config->entities + i);

    if (size > JUINT16_MAX)
    {
        DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: ctrl_iface_init failed, descriptor "
            "size is %d (should be less than %d)\n", size, JUINT16_MAX));
        rc = JEINVAL;
        goto Error;
    }

    /* Create and init entities descriptors */
    iface->extra_descriptor_size = size;
    iface->extra_descriptor = jmalloc(size, M_ZERO);

    if (!iface->extra_descriptor)
        goto NoMem;

    buf = (juint8_t *)iface->extra_descriptor;

    header_desc_init(fd, size, buf);
    buf += buf[0];

    for (i = 0; !rc && i < fd->config->entities_num; i++, buf += buf[0])
        rc = entity_init(fd, i, buf);

    if (rc)
        goto Error;

    iface->if_class = AUDIO_CLASS;

    return 0;

NoMem:
    rc = JENOMEM;
Error:
    DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: ctrl_iface_init failed, error %d\n", rc));

    ctrl_iface_uninit(fd, iface);

    return rc;
}

/**
 * Function name:  ctrl_iface_uninit
 * Description:    Frees audio control interface descriptors memory.
 * Parameters:
 *     @fd:    (IN) Audio FD context.
 *     @iface: (IN) Audio control interface descriptor.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void ctrl_iface_uninit(fd_audio_h fd, interface_desc_t *iface)
{
    iface->if_class = 0;

    entities_uninit(fd);

    if (iface->extra_descriptor)
        jfree(iface->extra_descriptor);

    if (iface->alt_ifs)
    {
        if (iface->alt_ifs->pipes)
            jfree(iface->alt_ifs->pipes);

        jfree(iface->alt_ifs);
    }
}

/**
 * Function name:  intr_pipe_init
 * Description:    Initializes an interrupt pipe descriptor.
 * Parameters:
 *     @fd:   (IN)  Audio FD context.
 *     @pipe: (OUT) Pointer to an interrupt pipe descriptor.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void intr_pipe_init(fd_audio_h fd, pipe_desc_t *pipe)
{
    pipe->max_pkt_size_high    = sizeof(audio_intr_data_t);
    pipe->max_pkt_size_full    = sizeof(audio_intr_data_t);
    pipe->type                 = PIPE_INTR;
    pipe->direction            = DIRECTION_IN;
    pipe->rate_u.poll_interval = 1 << ((juint8_t)fd->config->intr_ep_ival - 1);
    pipe->poll_interval_high   = (juint8_t)fd->config->intr_ep_ival + 3;
}

/**
 * Function name:  audio_send_intr
 * Description:    Sends interrupt data to the host.
 * Parameters:
 *     @fd:   (IN) Audio FD context.
 *     @data: (IN) Data to send.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t audio_send_intr(fd_audio_h fd, audio_intr_data_t *data)
{
    pipe_desc_t *pipe;
    request_t *req;
    jresult_t rc;

    pipe = fd->desc->interfaces->alt_ifs->pipes;
    if (!pipe)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Interrupt pipe not defined\n"));
        return JENOENT;
    }

    req = core_request_alloc(sizeof(audio_intr_data_t), NULL);
    if (!req)
    {
        rc = JENOMEM;
        goto Error;
    }

    req->caller_private = fd;
    req->complete  = intr_send_cb;
    req->transfer_size = sizeof(audio_intr_data_t);
    j_memcpy(req->buffer.vaddr, /* HSU addition */(void*)/* HSU addition */data, sizeof(audio_intr_data_t));

    rc = core_send_data(fd->core_ctx, pipe, req);
    if (rc)
        goto Error;

    return 0;

Error:
    DBG_E(DSLAVE_AUDIO, ("AUDIO: audio_send_intr error, %s\n", ESTR(rc)));

    return rc;
}

/**
 * Function name:  intr_send_cb
 * Description:    Interrupt transfer completion callback.
 * Parameters:
 *     @core_req: (IN) Pointer to a core request structure.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void intr_send_cb(request_t *core_req)
{
    if (core_req->status != REQUEST_COMPLETED)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Interrupt transfer failed, %s\n",
            ESTR(request_status_to_result(core_req->status))));
    }

    core_request_free(core_req);
}

/**
 * Function name:  header_desc_size
 * Description:    Returns audio control interface HEADER descriptor size.
 * Parameters:
 *     @fd:   (IN) Audio FD context.
 *
 * Return value:   Descriptor size.
 * Scope:          Local
 **/
static juint32_t header_desc_size(fd_audio_h fd)
{
    return sizeof(ctrl_iface_desc_t) + fd->config->streams_num - 1;
}

/**
 * Function name:  header_desc_init
 * Description:    Initializes an audio control interface HEADER descriptor.
 * Parameters:
 *     @fd:        (IN)  Audio FD context.
 *     @total_len: (IN)  Audio control interface class-specific descriptors
 *                       total length.
 *     @buf:       (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void header_desc_init(fd_audio_h fd, juint16_t total_len, void *buf)
{
    ctrl_iface_desc_t *desc = (ctrl_iface_desc_t *)buf;
    juint_t i;

    desc->bLength               = header_desc_size(fd);
    desc->bDescriptorType       = CS_INTERFACE;
    desc->bDescriptorSubtype    = AC_SUBTYPE_HEADER;
    desc->bInCollection         = fd->config->streams_num;

    ua_htole16(desc->bcdADC, AUDIO_DEVICE_CLASS_SPEC_RELEASE);
    ua_htole16(desc->wTotalLength, total_len);

    for (i = 0; i < fd->config->streams_num; i++)
        desc->baInterfaceNr[i] = fd->ctrl_iface_index + i + 1;
}

/**
 * Function name:  input_trm_init
 * Description:    Initializes an INPUT TERMINAL descriptor.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *     @ent:   (IN)  Pointer to an entity structure.
 *     @ent_p: (IN)  Pointer to a private entity data structure.
 *     @buf:   (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void input_trm_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf)
{
    input_trm_desc_t *desc = (input_trm_desc_t *)buf;
    uwd_audio_in_trm_t *itrm = (uwd_audio_in_trm_t *)ent->data;

    DBG_X(DSLAVE_AUDIO_DESC, ("AUDIO: input_trm_init(%d), trm_type=%d (%s)\n",
        ent->id, itrm->trm_type, TRM_TYPE(itrm->trm_type)));
    DBG_X(DSLAVE_AUDIO_DESC, ("   assoc_trm=%d, nr_channels=%d, channel_config:"
        "\n", itrm->assoc_terminal, itrm->nr_channels));
    DBG_RUN(DSLAVE_AUDIO_DESC, DL_EX_VERBOSE, print_bitmap(itrm->channel_config,
        channel_config_str, " %s"));

    desc->bLength               = sizeof(input_trm_desc_t);
    desc->bDescriptorType       = CS_INTERFACE;
    desc->bDescriptorSubtype    = AC_SUBTYPE_INPUT_TERMINAL;
    desc->bTerminalID           = ent->id;
    desc->bAssocTerminal        = itrm->assoc_terminal;
    desc->bNrChannels           = itrm->nr_channels;
    desc->iChannelNames         = 0;
    desc->iTerminal             = ent_p->str_id;

    ua_htole16(desc->wTerminalType, itrm->trm_type);
    ua_htole16(desc->wChannelConfig, itrm->channel_config);
}

/**
 * Function name:  output_trm_init
 * Description:    Initializes an OUTPUT TERMINAL descriptor.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *     @ent:   (IN)  Pointer to an entity structure.
 *     @ent_p: (IN)  Pointer to a private entity data structure.
 *     @buf:   (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void output_trm_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf)
{
    output_trm_desc_t *desc = (output_trm_desc_t *)buf;
    uwd_audio_out_trm_t *otrm = (uwd_audio_out_trm_t *)ent->data;

    DBG_X(DSLAVE_AUDIO_DESC, ("AUDIO: output_trm_init(%d), trm_type=%d (%s)\n",
        ent->id, otrm->trm_type, TRM_TYPE(otrm->trm_type)));
    DBG_X(DSLAVE_AUDIO_DESC, ("   assoc_trm=%d, source_id=%d\n",
        otrm->assoc_terminal, otrm->source_id));

    desc->bLength            = sizeof(output_trm_desc_t);
    desc->bDescriptorType    = CS_INTERFACE;
    desc->bDescriptorSubtype = AC_SUBTYPE_OUTPUT_TERMINAL;
    desc->bTerminalID        = ent->id;
    desc->bAssocTerminal     = otrm->assoc_terminal;
    desc->bSourceID          = otrm->source_id;
    desc->iTerminal          = ent_p->str_id;

    ua_htole16(desc->wTerminalType, otrm->trm_type);
}

/**
 * Function name:  feature_unit_get_desc_size
 * Description:    Returns FEATURE UNIT descriptor size.
 * Parameters:
 *     @fu: (IN) Pointer to a feature unit structure.
 *
 * Return value:   Descriptor size.
 * Scope:          Local
 **/
static juint32_t feature_unit_get_desc_size(uwd_audio_feature_unit_t *fu)
{
    return sizeof(feature_unit_desc_t) +
        fu->nr_channels * AUDIO_FU_CONTROL_SIZE;
}

#define PTR_ADD(ptr, val) (*(juint8_t **)&(ptr) += (val))

/**
 * Function name:  feature_unit_init
 * Description:    Initializes a FEATURE UNIT descriptor.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *     @ent:   (IN)  Pointer to an entity structure.
 *     @ent_p: (IN)  Pointer to a private entity data structure.
 *     @buf:   (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void feature_unit_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf)
{
    feature_unit_desc_t *desc = (feature_unit_desc_t *)buf;
    uwd_audio_feature_unit_t *fu = (uwd_audio_feature_unit_t *)ent->data;
    juint_t i;

    DBG_X(DSLAVE_AUDIO_DESC, ("AUDIO: feature_unit_init(%d), source_id=%d, "
        "nr_channels=%d, supported controls:\n", ent->id, fu->source_id,
        fu->nr_channels));

    desc->bLength            = feature_unit_get_desc_size(fu);
    desc->bDescriptorType    = CS_INTERFACE;
    desc->bDescriptorSubtype = AC_SUBTYPE_FEATURE_UNIT;
    desc->bUnitID            = ent->id;
    desc->bSourceID          = fu->source_id;
    desc->bControlSize       = AUDIO_FU_CONTROL_SIZE;

    for (i = 0; i < (fu->nr_channels + 1); i++)
    {
        ua_htole16(desc->bmaControls[i], fu->controls[i]);
        DBG_RUN(DSLAVE_AUDIO_DESC, DL_EX_VERBOSE, print_bitmap(fu->controls[i],
            fu_controls_str, " %s"));
    }

    PTR_ADD(desc, fu->nr_channels * AUDIO_FU_CONTROL_SIZE);
    desc->d_iFeature = ent_p->str_id;
}

/**
 * Function name:  selector_unit_get_desc_size
 * Description:    Returns SELECTOR UNIT descriptor size.
 * Parameters:
 *     @su: (IN) Pointer to selector unit struct.
 *
 * Return value:   Descriptor size.
 * Scope:          Local
 **/
static juint32_t selector_unit_get_desc_size(uwd_audio_selector_unit_t *su)
{
    return sizeof(selector_unit_desc_t) + su->nr_in_pins - 1;
}

/**
 * Function name:  selector_unit_init
 * Description:    Initializes SELECTOR UNIT descriptor.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *     @ent:   (IN)  Pointer to an entity structure.
 *     @ent_p: (IN)  Pointer to a private entity data structure.
 *     @buf:   (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void selector_unit_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf)
{
    selector_unit_desc_t *desc = (selector_unit_desc_t *)buf;
    uwd_audio_selector_unit_t *su = (uwd_audio_selector_unit_t *)ent->data;
    juint_t i;

    DBG_X(DSLAVE_AUDIO_DESC, ("AUDIO: selector_unit_init(%d), nr_in_pins=%d\n",
        ent->id, su->nr_in_pins));

    desc->bLength            = selector_unit_get_desc_size(su);
    desc->bDescriptorType    = CS_INTERFACE;
    desc->bDescriptorSubtype = AC_SUBTYPE_SELECTOR_UNIT;
    desc->bUnitID            = ent->id;
    desc->bNrInPins          = su->nr_in_pins;

    for (i = 0; i < su->nr_in_pins; i++)
        desc->baSourceID[i] = su->source_id[i];

    PTR_ADD(desc, su->nr_in_pins - 1);
    desc->d_iTerminal = ent_p->str_id;
}

/**
 * Function name:  extension_unit_get_desc_size
 * Description:    Returns EXTENSION UNIT descriptor size.
 * Parameters:
 *     @eu:  (IN) Pointer to an extension unit structure.
 *
 * Return value:   Descriptor size.
 * Scope:          Local
 **/
static juint32_t extension_unit_get_desc_size(uwd_audio_extension_unit_t *eu)
{
    return sizeof(extension_unit_desc_t) + eu->nr_in_pins - 1;
}

/**
 * Function name:  extension_unit_init
 * Description:    Initializes an EXTENSION UNIT descriptor.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *     @ent:   (IN)  Pointer to an entity structure.
 *     @ent_p: (IN)  Pointer to a private entity data structure.
 *     @buf:   (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void extension_unit_init(fd_audio_h fd, uwd_audio_entity_h ent,
    entity_priv_h ent_p, void *buf)
{
    extension_unit_desc_t *desc = (extension_unit_desc_t *)buf;
    uwd_audio_extension_unit_t *eu = (uwd_audio_extension_unit_t *)ent->data;
    juint_t i;

    DBG_X(DSLAVE_AUDIO_DESC, ("AUDIO: extension_unit_init(%d), extension_code"
        "=0x%04x nr_in_pins=%d, nr_channels=%d\n", ent->id, eu->extension_code,
        eu->nr_in_pins, eu->nr_channels));

    desc->bLength            = extension_unit_get_desc_size(eu);
    desc->bDescriptorType    = CS_INTERFACE;
    desc->bDescriptorSubtype = AC_SUBTYPE_EXTENSION_UNIT;
    desc->bUnitID            = ent->id;
    ua_htole16(desc->wExtensionCode, eu->extension_code);
    desc->bNrInPins          = eu->nr_in_pins;

    for (i = 0; i < eu->nr_in_pins; i++)
        desc->baSourceID[i] = eu->source_id[i];

    PTR_ADD(desc, eu->nr_in_pins - 1);

    desc->d_bNrChannels      = eu->nr_channels;
    ua_htole16(desc->d_wChannelConfig, eu->channel_config);
    desc->d_iChannelNames    = 0;
    desc->d_bControlSize     = 1;
    desc->d_bmControls[0]    = eu->controls;
    desc->d_iExtension       = ent_p->str_id;
}

/**
 * Function name:  entity_index_init
 * Description:    Initializes entities indexing data.
 *                 If entities numbers are continuous (1,2,3,...) - indexing
 *                 array is not created.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t entity_index_init(fd_audio_h fd)
{
    juint8_t max = 0, i, prev = 0;
    jbool_t continuous = 1;

    for (i = 0; i < fd->config->entities_num; i++)
    {
        juint8_t id = fd->config->entities[i].id;

        if (continuous)
        {
            if (id == (prev + 1))
                prev = id;
            else
                continuous = 0;
        }

        max = MAX(id, max);
    }

    fd->max_entity_id = max;

    if (continuous)
    {
        DBG_I(DSLAVE_AUDIO_DESC, ("AUDIO: Entities ids are consequtive\n"));
        return 0;
    }

    DBG_I(DSLAVE_AUDIO_DESC, ("AUDIO: Entities ids are not consequtive, "
        "creating indexing table, max id=%d\n", max));

    fd->entity_index = (juint8_t *)jmalloc(max, M_ZERO);
    if (!fd->entity_index)
        return JENOMEM;

    for (i = 0; i < fd->config->entities_num; i++)
    {
        juint8_t id = fd->config->entities[i].id;

        fd->entity_index[id - 1] = i;
    }

    return 0;
}

/**
 * Function name:  get_entity
 * Description:    Returns a pointer to an entity and private entity structures.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *     @id:    (IN)  Entity ID.
 *     @ent:   (OUT) Pointer to an entity structure.
 *     @ent_p: (OUT) Pointer to a private entity structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t get_entity(fd_audio_h fd, juint8_t id, uwd_audio_entity_h *ent,
    entity_priv_h *ent_p)
{
    juint_t index;

    if (!id || id > fd->max_entity_id)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Wrong entity id %d\n", id));
        return JENOENT;
    }

    if (fd->entity_index)
        index = fd->entity_index[id - 1];
    else
        index = id - 1;

    if (ent_p)
        *ent_p = &fd->entity_priv[index];

    if (ent)
        *ent = &fd->config->entities[index];

    return 0;
}

/**
 * Function name:  trm_check_req
 * Description:    Checks if an audio terminal supports a received control
 *                 request. (Request data located in fd->req).
 * Parameters:
 *     @fd:  (IN) Audio FD context.
 *     @ent: (IN) Pointer to an entity structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t trm_check_req(fd_audio_h fd, uwd_audio_entity_h ent)
{
    uwd_audio_terminal_cs_t cs = (uwd_audio_terminal_cs_t)(fd->req.value >> 8);

    if (cs != UWD_AUDIO_TR_COPY_PROTECT_CONTROL)
    {
        DBG_W(DSLAVE_AUDIO_CTRL, ("AUDIO: Terminal (%d) - unknown control, "
            "cs=0x%02x\n", cs));
    }

    return 0;
}

/**
 * Function name:  feature_unit_check_req
 * Description:    Checks if a FEATURE UNIT supports a received control request.
 *                 (Request data located in fd->req).
 * Parameters:
 *     @fd:  (IN) Audio FD context.
 *     @ent: (IN) Pointer to an entity structure.
 *     @fu:  (IN) Pointer to a feature unit structure.
 *
 * Return value:   0 if the request is supported, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t feature_unit_check_req(fd_audio_h fd, uwd_audio_entity_h ent,
    uwd_audio_feature_unit_t *fu)
{
    juint8_t ch = (juint8_t)fd->req.value;
    uwd_audio_feature_unit_cs_t cs =
        (uwd_audio_feature_unit_cs_t)(fd->req.value >> 8);

    /* Control addressed to all unit controls */
    if (ch == UWD_AUDIO_CTRL_ALL_CH)
        return 0;

    if (ch > fu->nr_channels)
        return JEINVAL;

    if (cs == UWD_AUDIO_FU_CONTROL_UNDEFINED ||
        cs > UWD_AUDIO_FU_LOUDNESS_CONTROL)
    {
        DBG_W(DSLAVE_AUDIO_CTRL, ("AUDIO: Feature unit (%d) - unknown control, "
            "cs=0x%02x\n", cs));
        return 0;
    }

    if (!(fu->controls[ch] & (1 << (cs - 1))))
        return JENOTSUP;

    return 0;
}

/**
 * Function name:  extension_unit_check_req
 * Description:    Checks if an EXTENSION UNIT supports a received control
 *                 request. (Request data located in fd->req).
 * Parameters:
 *     @fd:  (IN) Audio FD context.
 *     @ent: (IN) Pointer to an entity structure.
 *     @eu:  (IN) Pointer to an extension unit structure.
 *
 * Return value:   0 if the request is supported, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t extension_unit_check_req(fd_audio_h fd, uwd_audio_entity_h ent,
    uwd_audio_extension_unit_t *eu)
{
    uwd_audio_extension_unit_cs_t cs =
        (uwd_audio_extension_unit_cs_t)(fd->req.value >> 8);

    if (!(eu->controls & (1 << (cs - 1))))
        return JENOTSUP;

    return 0;
}

/**
 * Function name:  entity_check_req
 * Description:    Checks if an audio entity supports a received control
 *                 request. (Request data located in fd->req).
 * Parameters:
 *     @fd:  (IN) Audio FD context.
 *
 * Return value:   0 if the request is supported, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t entity_check_req(fd_audio_h fd)
{
    jresult_t rc;
    uwd_audio_entity_h ent = fd->req.recip.entity;

    if (!ent->req_cb)
    {
        rc = JENOTSUP;
        goto Exit;
    }

    switch (ent->type)
    {
    case UWD_AUDIO_ET_INPUT_TERMINAL:
    case UWD_AUDIO_ET_OUTPUT_TERMINAL:
        rc = trm_check_req(fd, ent);
        break;

    case UWD_AUDIO_ET_FEATURE_UNIT:
        rc = feature_unit_check_req(fd, ent,
            (uwd_audio_feature_unit_t *)ent->data);
        break;

    case UWD_AUDIO_ET_SELECTOR_UNIT:
        rc = 0;
        break;

    case UWD_AUDIO_ET_EXTENSION_UNIT:
        rc = extension_unit_check_req(fd, ent,
            (uwd_audio_extension_unit_t *)ent->data);
        break;

    case UWD_AUDIO_ET_MIXER_UNIT:
    case UWD_AUDIO_ET_PROCESSING_UNIT:
    default:
        rc = JEINVAL;
        break;
    }

Exit:
    if (rc)
    {
        DBG_E(DSLAVE_AUDIO_CTRL, ("AUDIO: entity_check_req failed, %s\n",
            ESTR(rc)));
    }

    return rc;
}

/**
 * Function name:  entity_get_desc_size
 * Description:    Returns audio entity descriptor size.
 * Parameters:
 *     @ent: (IN)  Pointer to an entity structure.
 *
 * Return value:   Descriptor size.
 * Scope:          Local
 **/
static juint32_t entity_get_desc_size(uwd_audio_entity_h ent)
{
    juint32_t size;

    switch (ent->type)
    {
    case UWD_AUDIO_ET_INPUT_TERMINAL:
        size = sizeof(input_trm_desc_t);
        break;

    case UWD_AUDIO_ET_OUTPUT_TERMINAL:
        size = sizeof(output_trm_desc_t);
        break;

    case UWD_AUDIO_ET_FEATURE_UNIT:
        size = feature_unit_get_desc_size((uwd_audio_feature_unit_t*)ent->data);
        break;

    case UWD_AUDIO_ET_SELECTOR_UNIT:
        size = selector_unit_get_desc_size(
            (uwd_audio_selector_unit_t *)ent->data);
        break;

    case UWD_AUDIO_ET_EXTENSION_UNIT:
        size = extension_unit_get_desc_size(
            (uwd_audio_extension_unit_t *)ent->data);
        break;

    default:
        size = 0;
        break;
    }
    DBG_X(DSLAVE_AUDIO_DESC, ("AUDIO: Entity (type=%d (%s)) descriptor size %d "
        "bytes\n", ent->type, ENTITY_TYPE(ent->type), size));

    return size;
}

/**
 * Function name:  entity_init
 * Description:    Initializes an entity descriptor and creates a string
 *                 descriptor.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *     @index: (IN)  Entity index in the configuration array.
 *     @buf:   (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t entity_init(fd_audio_h fd, juint8_t index, void *buf)
{
    uwd_audio_entity_h ent = &fd->config->entities[index];
    entity_priv_h ent_p = &fd->entity_priv[index];

    DBG_I(DSLAVE_AUDIO_DESC, ("AUDIO: Entity init id=%d, type=%d (%s), buf=%p"
        "\n", ent->id, ent->type, ENTITY_TYPE(ent->type), buf));

    switch (ent->type)
    {
    case UWD_AUDIO_ET_INPUT_TERMINAL:
        input_trm_init(fd, ent, ent_p, buf);
        break;

    case UWD_AUDIO_ET_OUTPUT_TERMINAL:
        output_trm_init(fd, ent, ent_p, buf);
        break;

    case UWD_AUDIO_ET_FEATURE_UNIT:
        feature_unit_init(fd, ent, ent_p, buf);
        break;

    case UWD_AUDIO_ET_SELECTOR_UNIT:
        selector_unit_init(fd, ent, ent_p, buf);
        break;

    case UWD_AUDIO_ET_EXTENSION_UNIT:
        extension_unit_init(fd, ent, ent_p, buf);
        break;

    case UWD_AUDIO_ET_PROCESSING_UNIT:
    case UWD_AUDIO_ET_MIXER_UNIT:
        DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: Unsupported entity type (%d, %s)\n",
            ent->type, ENTITY_TYPE(ent->type)));
        return JENOTSUP;

    default:
        DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: Unknown entity type (%d)\n",
            ent->type));
        return JEINVAL;
    }

    if (!ent->string)
        return 0;

    ent_p->str_id = core_add_string_descriptor(fd->core_ctx, ent->string, 
              HSU_STR_IDX_NOT_SET);
    if (!ent_p->str_id)
    {
        DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: entity_init (%d) failed, can't "
            "create string descriptor\n", ent->id));
        return JENOSPC;
    }

    return 0;
}

/**
 * Function name:  entities_uninit
 * Description:    Frees entities descriptors memory and string descriptors.
 * Parameters:
 *     @fd:    (IN)  Audio FD context.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void entities_uninit(fd_audio_h fd)
{
    jresult_t i;

    for (i = 0; i < fd->config->entities_num; i++)
    {
        entity_priv_h ent_p = &fd->entity_priv[i];

        if (ent_p->str_id)
            core_release_string_descriptor(fd->core_ctx, ent_p->str_id);
    }
}

/* Stream */

/**
 * Function name:  get_pipe
 * Description:    Returns the isochronous pipe descriptor of a given
 *                 AudioStreaming interface.
 * Parameters:
 *     @fd:       (IN) Audio FD context.
 *     @stream:   (IN) Pointer to an AudioStreaming interface structure.
 *     @stream_p: (IN) Pointer to a private audio stream data structure.
 *
 * Return value:   Pointer to a pipe descriptor, or NULL.
 * Scope:          Local
 **/
static pipe_desc_t *get_pipe(fd_audio_h fd, uwd_audio_stream_h stream,
    stream_priv_h stream_p)
{
    juint_t num = stream - &fd->config->streams[0];

    return fd->desc->interfaces[num + 1].alt_ifs[stream_p->alt].pipes;
}

/**
 * Function name:  get_ep_stream
 * Description:    Returns a pointer to an AudioStreaming interface structure
 *                 by isochronous endpoint address.
 * Parameters:
 *     @fd:       (IN)  Audio FD context.
 *     @ep_addr:  (IN)  Streaming endpoint address.
 *     @stream:   (OUT) Pointer to an AudioStreaming interface structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t get_ep_stream(fd_audio_h fd, juint8_t ep_addr,
    uwd_audio_stream_h *stream)
{
    juint_t i;

    for (i = 1; i < fd->desc->interface_count; i++)
    {
        interface_desc_t *iface = &fd->desc->interfaces[i];
        pipe_desc_t *pipe = iface->alt_ifs[iface->alt_if_curr].pipes;

        if (pipe && ep_addr == pipe->address)
        {
            *stream = &fd->config->streams[i-1];
            return 0;
        }
    }

    DBG_E(DSLAVE_AUDIO, ("AUDIO: Wrong isoc ep address %d\n", ep_addr));
    return JENOENT;
}

/**
 * Function name:  get_stream
 * Description:    Returns a pointer to an AudioStreaming interfaces structure
 *                 by interface index.
 * Parameters:
 *     @fd:          (IN)  Audio FD context.
 *     @iface_index: (IN)  Streaming interface index.
 *     @stream:      (OUT) Pointer to an AudioStreaming interface structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t get_stream(fd_audio_h fd, juint8_t iface_index,
    uwd_audio_stream_h *stream)
{
    jint16_t i = (jint16_t)fd->ctrl_iface_index - (jint16_t)iface_index - 1;

    if (i < 0 || i >= fd->config->streams_num)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Wrong stream iface index %d\n",
            iface_index));
        return JENOENT;
    }

    *stream = &fd->config->streams[i];
    return 0;
}

/**
 * Function name:  get_stream_priv
 * Description:    Returns a pointer to a private stream structure.
 * Parameters:
 *     @fd:     (IN) Audio FD context.
 *     @stream: (IN) Pointer to an AudioStreaming interface structure.
 *
 * Return value:   Pointer to a private audio stream data structure.
 * Scope:          Local
 **/
static stream_priv_h get_stream_priv(fd_audio_h fd, uwd_audio_stream_h stream)
{
    return &fd->stream_priv[stream - &fd->config->streams[0]];
}

/**
 * Function name:  get_stream_index
 * Description:    Returns the index of an AudioStreaming interface in the
 *                 config structure's 'streams' array.
 * Parameters:
 *     @fd:     (IN) Audio FD context.
 *     @stream: (IN) Pointer to an AudioStreaming interface structure.
 *
 * Return value:   AudioStreaming interface index.
 * Scope:          Local
 **/
static juint8_t get_stream_index(fd_audio_h fd, uwd_audio_stream_h stream)
{
    return fd->ctrl_iface_index + 1 + (stream - &fd->config->streams[0]);
}

/**
 * Function name:  stream_check_req
 * Description:    Checks if an AudioStreaming interface supports a received
 *                 control request.
 * Parameters:
 *     @fd: (IN) Audio FD context.
 *
 * Return value:   0 if the request is supported, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t stream_check_req(fd_audio_h fd)
{
    uwd_audio_stream_h stream = fd->req.recip.stream;

    if (!stream->req_cb)
    {
        DBG_E(DSLAVE_AUDIO_CTRL, ("AUDIO: stream_check_req failed, %s\n",
            ESTR(JENOTSUP)));
        return JENOTSUP;
    }

    return 0;
}

/**
 * Function name:  stream_iface_init
 * Description:    Initializes AudioStreaming interface descriptors and private
 *                 data.
 * Parameters:
 *     @fd:  (IN) Audio FD context.
 *     @num: (IN) Stream number.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t stream_iface_init(fd_audio_h fd, juint_t num)
{
    interface_desc_t   *iface = &fd->desc->interfaces[num+1];
    uwd_audio_stream_h stream = &fd->config->streams[num];
    stream_priv_h stream_p = &fd->stream_priv[num];
    jresult_t rc = 0;
    juint_t i;

    iface->control_msg = control_msg;
    iface->alt_if_num  = stream->alts_num + 1;
    iface->if_subclass = AUDIO_SUBCLASS_STREAMING;
    iface->if_protocol = 0;
    iface->number = UWD_UNDEFINED_INTERFACE;

    iface->alt_ifs = (alt_interface_desc_t *)
        jmalloc(sizeof(alt_interface_desc_t) * iface->alt_if_num, M_ZERO);

    if (!iface->alt_ifs)
        return JENOMEM;

    STAILQ_INIT(&stream_p->bufs);
    stream_p->fd = fd;
    stream_p->stream = stream;

    /* Alternate setting 0 must be empty */
    for (i = 0; !rc && i < stream->alts_num; i++)
    {
        rc = stream_alt_init(fd, &iface->alt_ifs[i + 1], &stream->alts[i],
            stream_p);
    }

    if (rc)
        goto Error;

    /* Used for freeing request buffers */
    stream_p->flags[STREAM_FD_MEM_DESC] = STREAM_FD_MEM_DESC;

    iface->if_class = AUDIO_CLASS;

    return 0;

Error:
    stream_iface_uninit(fd, num, i);
    return rc;
}

/**
 * Function name:  stream_iface_uninit
 * Description:    Un-initializes (frees) stream descriptors.
 * Parameters:
 *     @fd:     (IN) Audio FD context.
 *     @if_num: (IN) Stream number.
 *     @alts:   (IN) Number of alternate settings to un-initialize.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void stream_iface_uninit(fd_audio_h fd, juint_t if_num, juint_t alts)
{
    interface_desc_t   *iface = &fd->desc->interfaces[if_num+1];
    uwd_audio_stream_h stream = &fd->config->streams[if_num];
    juint_t i;

    iface->if_class = 0;

    for (i = 0; i < alts; i++)
        stream_alt_uninit(fd, &iface->alt_ifs[i + 1], &stream->alts[i]);

    jfree(iface->alt_ifs);
}

/**
 * Function name:  stream_alt_init
 * Description:    Initializes an AudioStreaming interface alternate setting
 *                 descriptor.
 * Parameters:
 *     @fd:         (IN)  Audio FD context.
 *     @desc:       (OUT) Pointer to an AudioStreaming interface  alternate
 *                        setting descriptor.
 *     @stream_alt: (IN)  Pointer to an AudioStreaming interface alternate
 *                        setting structure.
 *     @stream_p:   (OUT) Pointer to a private audio stream data structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t stream_alt_init(fd_audio_h fd, alt_interface_desc_t *desc,
    uwd_audio_stream_alt_t *stream_alt, stream_priv_h stream_p)
{
    jresult_t rc;
    juint8_t *buf;

    DBG_V(DSLAVE_AUDIO_DESC, ("AUDIO: stream_alt_init, format - %s\n",
        FMT_TYPE(stream_alt->format_type)));

    desc->extra_descriptor_size = sizeof(sif_desc_t) +
        format_desc_size(stream_alt);

    desc->extra_descriptor = jmalloc(desc->extra_descriptor_size, M_ZERO);
    if (!desc->extra_descriptor)
        goto NoMem;

    buf = (juint8_t *)desc->extra_descriptor;

    stream_iface_desc_init(stream_alt, buf);
    buf += buf[0];

    format_desc_init(stream_alt, buf);

    /* Allocate pipe */
    if (stream_alt->ep_ival != UWD_AUDIO_NO_DATA)
    {
        desc->pipe_count = 1;
        desc->pipes = (pipe_desc_t *)jmalloc(sizeof(pipe_desc_t), M_ZERO);
        if (!desc->pipes)
            goto NoMem;

        rc = isoc_pipe_init(fd, desc->pipes, stream_alt, stream_p);
        if (rc)
            goto Error;
    }

    return 0;

NoMem:
    rc = JENOMEM;
Error:
    stream_alt_uninit(fd, desc, stream_alt);
    return rc;
}

/**
 * Function name:  stream_alt_uninit
 * Description:    Un-initializes (frees) an AudioStreaming interface alternate
 *                 setting descriptor.
 * Parameters:
 *     @fd:         (IN) Audio FD context.
 *     @desc:       (IN) Pointer to an AudioStreaming interface  alternate
 *                       setting descriptor.
 *     @stream_alt: (IN) Pointer to an AudioStreaming interface alternate
 *                       setting structure.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void stream_alt_uninit(fd_audio_h fd,  alt_interface_desc_t *desc,
    uwd_audio_stream_alt_t *stream_alt)
{
    if (desc->pipes)
    {
        if (desc->pipes->extra_descriptor)
            jfree(desc->pipes->extra_descriptor);

        jfree(desc->pipes);
    }

    if (desc->extra_descriptor)
        jfree(desc->extra_descriptor);
}

/**
 * Function name:  stream_iface_desc_init
 * Description:    Initializes a class-specific streaming interface descriptor.
 * Parameters:
 *     @stream_alt: (IN)  Pointer to an AudioStreaming interface alternate
 *                        setting structure.
 *     @buf:        (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void stream_iface_desc_init(uwd_audio_stream_alt_h stream_alt, void *buf)
{
    sif_desc_t *desc = (sif_desc_t *)buf;

    desc->bLength            = sizeof(sif_desc_t);
    desc->bDescriptorType    = CS_INTERFACE;
    desc->bDescriptorSubtype = AS_GENERAL;
    desc->bTerminalLink      = stream_alt->trm_link;
    desc->bDelay             = stream_alt->delay;

    ua_htole16(&desc->wFormatTag, stream_alt->format_type);
}

/**
 * Function name:  format_desc_init
 * Description:    Initializes an audio format descriptor.
 * Parameters:
 *     @stream_alt: (IN)  Pointer to an AudioStreaming interface alternate
 *                        setting structure.
 *     @buf:        (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void format_desc_init(uwd_audio_stream_alt_h stream_alt, void *buf)
{
    if (stream_alt->format_type < UWD_AUDIO_DF_II_UNDEFINED ||
        stream_alt->format_type >= UWD_AUDIO_DF_III_UNDEFINED)
    {
        /* Type I and III Formats */
        format_I_desc_init(stream_alt, buf);
    }
    else
    {
        /* Type II Formats */
        NOT_IMPLEMENTED;
    }
}

/**
 * Function name:  format_get_samp_freq
 * Description:    Returns a pointer to a sampling frequency data structure.
 *                 If sampling frequency is continuous - returns discrete NULL.
 *                 If sampling frequency is discrete - returns cont NULL.
 * Parameters:
 *     @stream_alt: (IN)  Pointer to an AudioStreaming interface alternate
 *                        setting structure.
 *     @discrete:   (OUT) Pointer to a discrete sampling frequency data
 *                        structure.
 *     @cont:       (OUT) Pointer to a continuous sampling frequency data
 *                        structure.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void format_get_samp_freq(uwd_audio_stream_alt_h stream_alt,
    uwd_audio_freq_discrete_t **discrete, uwd_audio_freq_continuous_t **cont)
{
    if (stream_alt->format_type < UWD_AUDIO_DF_II_UNDEFINED ||
        stream_alt->format_type >= UWD_AUDIO_DF_III_UNDEFINED)
    {
        /* Type I and III Formats */
        uwd_audio_format_I_t *fmt = (uwd_audio_format_I_t *)stream_alt->format;

        *discrete = fmt->freq_discrete;
        *cont = fmt->freq_continuous;
    }
    else
    {
        /* Type II Formats */
        *discrete = NULL;
        *cont = NULL;
        NOT_IMPLEMENTED;
    }
}

/**
 * Function name:  format_get_audio_frame_size
 * Description:    Returns the audio frame size for given AudioStreaming
 *                 interface alternate setting.
 *                 (Can be used for formats type I and III).
 * Parameters:
 *     @stream_alt: (IN) Pointer to an AudioStreaming interface alternate
 *                       setting structure.
 *
 * Return value:   Frame size, in bytes.
 * Scope:          Local
 **/
static juint16_t format_get_audio_frame_size(uwd_audio_stream_alt_h stream_alt)
{
    uwd_audio_format_I_t *fmt = (uwd_audio_format_I_t *)stream_alt->format;

    KASSERT(stream_alt->format_type < UWD_AUDIO_DF_II_UNDEFINED ||
        stream_alt->format_type >= UWD_AUDIO_DF_III_UNDEFINED,
        ("format_get_audio_frame_size used for format type II\n"));
        /* Type I and III Formats */

    return fmt->nr_channels * UWD_AUDIO_SUBFRAME_SIZE(fmt);
}

/**
 * Function name:  format_desc_size
 * Description:    Returns audio format descriptor size.
 * Parameters:
 *     @stream_alt: (IN) Pointer to an AudioStreaming interface alternate
 *                       setting structure.
 *
 * Return value:   Descriptor size.
 * Scope:          Local
 **/
static juint32_t format_desc_size(uwd_audio_stream_alt_h stream_alt)
{
    juint32_t size;

    if (stream_alt->format_type < UWD_AUDIO_DF_II_UNDEFINED ||
        stream_alt->format_type >= UWD_AUDIO_DF_III_UNDEFINED)
    {
        /* Type I and III Formats */
        size = format_I_desc_size(stream_alt);
    }
    else
    {
        /* Type II Formats */
        size = 0;
    }

    DBG_X(DSLAVE_AUDIO_DESC, ("AUDIO: Format %04x descriptor size=%d\n",
        stream_alt->format_type, size));

    return size;
}

/**
 * Function name:  format_I_desc_size
 * Description:    Returns audio format type I descriptor size.
 * Parameters:
 *     @stream_alt: (IN) Pointer to an AudioStreaming interface alternate
 *                       setting structure.
 *
 * Return value:   Descriptor size.
 * Scope:          Local
 **/
static juint32_t format_I_desc_size(uwd_audio_stream_alt_h stream_alt)
{
    uwd_audio_format_I_t *fmt = (uwd_audio_format_I_t *)stream_alt->format;

    return sizeof(format_I_desc_t) + samp_freq_desc_size(fmt->freq_discrete,
        fmt->freq_continuous);
}

/**
 * Function name:  format_I_desc_init
 * Description:    Initializes an audio format type I descriptor.
 * Parameters:
 *     @stream_alt: (IN)  Pointer to an AudioStreaming interface alternate
 *                        setting structure.
 *     @buf:        (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void format_I_desc_init(uwd_audio_stream_alt_h stream_alt, void *buf)
{
    format_I_desc_t *desc = (format_I_desc_t *)buf;
    uwd_audio_format_I_t *fmt = (uwd_audio_format_I_t *)stream_alt->format;

    desc->bLength            = format_I_desc_size(stream_alt);
    desc->bDescriptorType    = CS_INTERFACE;
    desc->bDescriptorSubtype = AS_FORMAT_TYPE;
    desc->bFormatType        = AF_TYPE_I;
    desc->bNrChannels        = fmt->nr_channels;
    desc->bSubframeSize      = UWD_AUDIO_SUBFRAME_SIZE(fmt);
    desc->bBitResolution     = fmt->bit_resolution;

    samp_freq_desc_init(fmt->freq_discrete, fmt->freq_continuous, /* HSU addition */(void*)/* HSU addition */(desc + 1));
}

/**
 * Function name:  audio_samp_freq_to_uint32
 * Description:    Converts sampling frequency from a 3-bytes format to Hz.
 * Parameters:
 *     @freq: (IN) Sampling frequency expressed in a 3-bytes format.
 *
 * Return value:   Frequency in units of Hz.
 * Scope:          Local
 **/
static juint32_t audio_samp_freq_to_uint32(audio_samp_freq_t *freq)
{
    juint32_t f = 0;

    j_memcpy(&f, freq, 3);

    return le32toh(f);
}

/**
 * Function name:  audio_uint32_to_samp_freq
 * Description:    Converts sampling frequency from Hz to a 3-bytes format.
 * Parameters:
 *     @freq_hz:  (IN)  Sampling frequency in Hz.
 *     @freq:     (OUT) Pointer to a 3-bytes format presentation of the given
 *                      sampling frequency (@freq_hz).
 *
 * Return value:   None.
 * Scope:          Local
 **/
static void audio_uint32_to_samp_freq(juint32_t freq_hz, audio_samp_freq_t *freq)
{
    freq_hz = htole32(freq_hz);
    j_memcpy(freq, &freq_hz, 3);
}

/**
 * Function name:  samp_freq_desc_size
 * Description:    Returns sampling frequency descriptor size.
 * Parameters:
 *     @discrete: (IN) Pointer to a discrete sampling frequency data structure.
 *     @cont:     (IN) Pointer to a continuous sampling frequency data structure.
 *
 * Return value:   Descriptor size.
 * Scope:          Local
 **/
static juint32_t samp_freq_desc_size(uwd_audio_freq_discrete_t *discrete,
    uwd_audio_freq_continuous_t *cont)
{
    if (discrete)
    {
        return sizeof(freq_discrete_desc_t) +
            (discrete->freqs_num - 1) * UWD_AUDIO_SAMP_FREQ_SIZE;
    }
    else /* cont */
    {
        return sizeof(freq_cont_desc_t);
    }
}

/**
 * Function name:  samp_freq_desc_init
 * Description:    Initializes a sampling frequency descriptor.
 * Parameters:
 *     @discrete: (IN) Pointer to a discrete sampling frequency data structure.
 *     @cont:     (IN) Pointer to a continuous sampling frequency data structure.
 *     @buf:      (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void samp_freq_desc_init(uwd_audio_freq_discrete_t *discrete,
    uwd_audio_freq_continuous_t *cont, void *buf)
{
    if (discrete)
    {
        freq_discrete_desc_t *desc = (freq_discrete_desc_t *)buf;
        juint_t i;

        desc->bSamFreqType = discrete->freqs_num;

        for (i = 0; i < discrete->freqs_num; i++)
        {
            audio_uint32_to_samp_freq(discrete->freqs[i],
                (audio_samp_freq_t *)&desc->tSamFreq[i]);
        }
    }
    else /* cont */
    {
        freq_cont_desc_t *desc = (freq_cont_desc_t *)buf;

        desc->bSamFreqType = 0;
        audio_uint32_to_samp_freq(cont->min, 
            (audio_samp_freq_t *)&desc->tLowerSamFreq);
        audio_uint32_to_samp_freq(cont->max, 
            (audio_samp_freq_t *)&desc->tUpperSamFreq);
    }
}

/**
 * Function name:  get_samp_freq_mult
 * Description:    Checks if more than one sampling frequency setting is
 *                 possible for given AudioStreaming interface alternate setting.
 * Parameters:
 *     @stream_alt: (IN) Pointer to an AudioStreaming interface alternate
 *                       setting structure.
 *
 * Return value:   TRUE if more than one sampling frequency setting is possible;
 *                 FALSE otherwise.
 * Scope:          Local
 **/
static jbool_t get_samp_freq_mult(uwd_audio_stream_alt_h stream_alt)
{
    uwd_audio_freq_discrete_t *discrete;
    uwd_audio_freq_continuous_t *cont;

    format_get_samp_freq(stream_alt, &discrete, &cont);

    return cont || (discrete->freqs_num > 1);
}

/**
 * Function name:  get_samp_freq_range
 * Description:    Returns min and max sampling frequency for given
 *                 AudioStreaming interface alternate setting.
 * Parameters:
 *     @stream_alt: (IN)  Pointer to an AudioStreaming interface alternate
 *                        setting structure.
 *     @min:        (OUT) Minimum sampling frequency.
 *     @max:        (OUT) Maximum sampling frequency.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void get_samp_freq_range(uwd_audio_stream_alt_h stream_alt,
    juint32_t *min, juint32_t *max)
{
    uwd_audio_freq_discrete_t *discrete;
    uwd_audio_freq_continuous_t *cont;
    juint_t i;

    format_get_samp_freq(stream_alt, &discrete, &cont);

    if (cont)
    {
        *min = cont->min;
        *max = cont->max;
        return;
    }

    *max = 0;
    *min = JUINT32_MAX;
    for (i = 0; i < discrete->freqs_num; i++)
    {
        *max = MAX(*max, discrete->freqs[i]);
        *min = MIN(*min, discrete->freqs[i]);
    }
}

#define DIFF(x, y) ABS((jint32_t)(x) - (jint32_t)(y))

/**
 * Function name:  get_closest_samp_freq
 * Description:    Returns the closest possible value for given sampling
 *                 frequency.
 * Parameters:
 *     @stream_alt: (IN) Pointer to an AudioStreaming interface alternate
 *                       setting structure.
 *     @freq:       (IN) Sampling frequency, in Hz.
 *
 * Return value:   Sampling frequency, in Hz.
 * Scope:          Local
 **/
static juint32_t get_closest_samp_freq(uwd_audio_stream_alt_h stream_alt,
    juint32_t freq)
{
    uwd_audio_freq_discrete_t *discrete;
    uwd_audio_freq_continuous_t *cont;
    juint_t i;
    juint32_t best_freq, best_diff;

    format_get_samp_freq(stream_alt, &discrete, &cont);

    if (cont)
    {
        if (freq < cont->min)
            freq = cont->min;

        if (freq > cont->max)
            freq = cont->max;

        return freq;
    }

    i = 0;
    best_diff = JUINT32_MAX;
    best_freq = 0;

    do {
        juint32_t cur_diff = DIFF(discrete->freqs[i], freq);

        if (best_diff > cur_diff)
        {
            best_freq = discrete->freqs[i];
            best_diff = cur_diff;
        }
    } while (++i < discrete->freqs_num);

    return best_freq;
}

/**
 * Function name:  handle_samp_freq_req
 * Description:    Handles a SAMPLING_FREQ_CONTROL request.
 * Parameters:
 *     @fd: (IN) Audio FD context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t handle_samp_freq_req(fd_audio_h fd)
{
    uwd_audio_stream_h stream = fd->req.recip.stream;
    stream_priv_h stream_p = get_stream_priv(fd, stream);
    uwd_audio_stream_alt_h stream_alt = &stream->alts[stream_p->alt-1];
    juint32_t min, max, cur;

    if (fd->req.len < UWD_AUDIO_SAMP_FREQ_SIZE)
        return JEINVAL;

    get_samp_freq_range(stream_alt, &min, &max);

    switch (fd->req.code)
    {
    case UWD_AUDIO_RC_SET_CUR:

        cur = audio_samp_freq_to_uint32((audio_samp_freq_t *)
            fd->req.core_req->buffer.vaddr);

        cur = get_closest_samp_freq(stream_alt, cur);

        if (cur == stream_p->samp_freq)
            return 0;

        DBG_I(DSLAVE_AUDIO_STREAM, ("AUDIO: Set Samp freq %d\n", cur));

        return stream_restart_freq(fd, stream, stream_p, cur);

    case UWD_AUDIO_RC_GET_CUR:
        cur = stream_p->samp_freq;
        break;
    case UWD_AUDIO_RC_GET_RES:
    case UWD_AUDIO_RC_GET_MIN:
        cur = min;
        break;
    case UWD_AUDIO_RC_GET_MAX:
        cur = max;
        break;
    default:
        return JENOTSUP;
    }

    DBG_I(DSLAVE_AUDIO_CTRL, ("AUDIO: Samp freq %s %d\n",
        REQ_CODE(fd->req.code), cur));

    audio_uint32_to_samp_freq(cur, (audio_samp_freq_t *)
        fd->req.core_req->buffer.vaddr);

    return 0;
}

/* Isoc Pipe */

/**
 * Function name:  calc_max_packet
 * Description:    Calculates an isochronous pipe's maximum packet size.
 * Parameters:
 *     @max_freq:         (IN) Maximum sampling frequency.
 *     @ival_ms:          (IN) Pipe interval, in milliseconds.
 *     @audio_frame_size: (IN) Audio frame size, in bytes.
 *
 * Return value:   Maximum packet size.
 * Scope:          Local
 **/
static juint16_t calc_max_packet(juint32_t max_freq, juint16_t ival_ms,
    juint16_t audio_frame_size)
{
    juint32_t max_pkt;

    if (ival_ms < 256)
    {
        max_pkt = (max_freq * ival_ms + 999) / 1000;
        max_pkt *= audio_frame_size;
    }
    else
    {
        max_pkt = (max_freq * (ival_ms >> 8) + 999) / 1000;
        max_pkt *= audio_frame_size << 8;
    }

    return max_pkt;
}

/**
 * Function name:  ep_check_req
 * Description:    Checks if the AudioStreaming endpoint supports a received
 *                 control.
 * Parameters:
 *     @fd: (IN) Audio FD context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t ep_check_req(fd_audio_h fd)
{
    uwd_audio_stream_h stream = fd->req.recip.stream;
    stream_priv_h stream_p = get_stream_priv(fd, stream);
    uwd_audio_endpoint_cs_t cs = (uwd_audio_endpoint_cs_t)(fd->req.value >> 8);

    if (!stream->ep_req_cb && cs != UWD_AUDIO_EP_SAMPLING_FREQ_CONTROL)
        return JENOTSUP;

    if (cs != UWD_AUDIO_EP_SAMPLING_FREQ_CONTROL)
    {
        DBG_W(DSLAVE_AUDIO_CTRL, ("AUDIO: Unknown endpoint control selector,"
            " cs=0x%02x\n", cs));
        return 0;
    }

    if (!stream_p->alt)
    {
        DBG_E(DSLAVE_AUDIO_CTRL, ("AUDIO: ep_check_req failed, stream alternate"
            "setting 0 is active\n"));
        return JEINVAL;
    }

    if (!get_samp_freq_mult(&stream->alts[stream_p->alt-1]))
    {
        DBG_W(DSLAVE_AUDIO_CTRL, ("AUDIO: ep_check_req failed, "
            "SAMPLING_FREQ_CONTROL not supported\n"));
    }

    return 0;
}

/**
 * Function name:  isoc_pipe_init
 * Description:    Initializes an isochronous pipe descriptor.
 * Parameters:
 *     @fd:         (IN)  Audio FD context.
 *     @desc:       (OUT) Pointer to a pipe descriptor.
 *     @stream_alt: (IN)  Pointer to an AudioStreaming interface alternate
 *                        setting structure.
 *     @stream_p:   (OUT) Pointer to a private audio stream data structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t isoc_pipe_init(fd_audio_h fd, pipe_desc_t *desc,
    uwd_audio_stream_alt_t *stream_alt, stream_priv_h stream_p)
{
    uwd_audio_entity_h ent;
    juint16_t max_packet;
    jresult_t rc;

    rc = get_entity(fd, stream_alt->trm_link, &ent, NULL);
    if (rc)
    {
        DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: isoc_pipe_init failed, invalid "
            "trm_link=%d\n", stream_alt->trm_link));
        return rc;
    }

    switch (ent->type)
    {
    case UWD_AUDIO_ET_INPUT_TERMINAL:
        desc->direction = DIRECTION_OUT;
        stream_p->stream->read = 1;
        break;
    case UWD_AUDIO_ET_OUTPUT_TERMINAL:
        desc->direction = DIRECTION_IN;
        stream_p->stream->read = 0;
        break;
    default:
        DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: isoc_pipe_init failed, trm_link=%d "
            "entity is not terminal\n", stream_alt->trm_link));
        return JEINVAL;
    }

    if (stream_alt->ep_max_packet == UWD_AUDIO_FD_CALC_MAX_PACKET ||
        stream_alt->ep_max_packet == UWD_AUDIO_FD_CALC_MAX_PACKET_SYNC)
    {
        juint32_t max_freq, min_freq;
        juint16_t audio_frame_size = format_get_audio_frame_size(stream_alt);
        get_samp_freq_range(stream_alt, &min_freq, &max_freq);

        max_packet = calc_max_packet(max_freq, 1 << (stream_alt->ep_ival - 1),
            audio_frame_size);

        if (stream_alt->ep_max_packet == UWD_AUDIO_FD_CALC_MAX_PACKET_SYNC)
            max_packet += audio_frame_size;
    }
    else
    {
        max_packet = stream_alt->ep_max_packet;
    }

    if (max_packet > ISOC_MAX_PACKET_FULL || !max_packet)
    {
        DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: isoc_pipe_init failed, invalid "
            "endpoint max packet size %d (%d is max for full speed)\n",
            max_packet, ISOC_MAX_PACKET_FULL));

        return JEINVAL;
    }

    desc->max_pkt_size_high    = (juint16_t)max_packet;
    desc->max_pkt_size_full    = (juint16_t)max_packet;
    desc->type                 = PIPE_ISOC;
    desc->sync_type            = 0;  /* No Synchronization */
    desc->usage_type           = 0;  /* Data Endpoint */
    desc->rate_u.poll_interval = (juint8_t)stream_alt->ep_ival;
    desc->poll_interval_high   = (juint8_t)stream_alt->ep_ival + 3;

    desc->extra_descriptor_size = sizeof(audio_isoc_ep_desc_t);
    desc->extra_descriptor = jmalloc(desc->extra_descriptor_size, M_ZERO);

    if (!desc->extra_descriptor)
        return JENOMEM;

    audio_isoc_ep_desc_init(get_samp_freq_mult(stream_alt),
        desc->extra_descriptor);

    return 0;
}

/**
 * Function name:  audio_isoc_ep_desc_init
 * Description:    Initializes an audio class-specific isochronous pipe
 *                 descriptor.
 * Parameters:
 *     @set_freq_support: (IN)  TRUE if SAMPLING_FREQ_CONTROL is supported.
 *     @buf:              (OUT) Pointer to a descriptor buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void audio_isoc_ep_desc_init(jbool_t set_freq_support, void *buf)
{
    audio_isoc_ep_desc_t *desc = (audio_isoc_ep_desc_t *)buf;

    desc->bLength            = sizeof(audio_isoc_ep_desc_t);
    desc->bDescriptorType    = CS_ENDPOINT;
    desc->bDescriptorSubtype = EP_GENERAL;
    desc->bmAttributes       = set_freq_support ? AI_ATTR_SAMPLING_FREQUENCY :0;
}

/* //////////////
 *    Streaming
 * ////////////// */

/**
 * Function name:  stream_add_buf
 * Description:    Inserts an application buffer into a stream queue.
 * Parameters:
 *     @stream_p: (IN) Pointer to a private audio stream data structure.
 *     @mem:  (IN/OUT) Pointer to a memory data buffer to be used for the
 *                     transfer.
 *     @len:      (IN) Data length, in bytes.
 *     @cb:       (IN) Read/Write completion application callback.
 *     @arg:      (IN) Pointer to arguments for the callback (@cb);
 *                     NULL if no arguments.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t stream_add_buf(stream_priv_h stream_p, void *mem, 
    juint32_t len, uwd_audio_stream_cb_t cb, void *arg)
{
    stream_buf_t *buf = jmalloc(sizeof(stream_buf_t), M_ZERO);

    if (!buf)
        return JENOMEM;

    buf->mem = mem;
    buf->len = len;
    buf->cb  = cb;
    buf->arg = arg;

    STAILQ_INSERT_TAIL(&stream_p->bufs, buf, next);

    if (!stream_p->cur.buf)
    {
        stream_p->cur.buf = buf;
        stream_p->cur.buf_ofs = 0;
    }

    return 0;
}

/**
 * Function name:  stream_restart
 * Description:    Restarts a data stream with default (min) sampling frequency.
 * Parameters:
 *     @fd:       (IN) Audio FD context.
 *     @stream:   (IN) Pointer to an AudioStreaming interface structure.
 *     @stream_p: (IN) Pointer to a private audio stream data structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t stream_restart(fd_audio_h fd, uwd_audio_stream_h stream,
    stream_priv_h stream_p)
{
    uwd_audio_stream_alt_t *stream_alt = &stream->alts[stream_p->alt-1];
    juint32_t min, max;

    DBG_I(DSLAVE_AUDIO_STREAM, ("AUDIO: stream_restart\n"));

    get_samp_freq_range(stream_alt, &min, &max);

    return stream_restart_freq(fd, stream, stream_p, min);
}

/**
 * Function name:  stream_restart_freq
 * Description:    Restarts a data stream with the specified sampling frequency.
 * Parameters:
 *     @fd:        (IN) Audio FD context.
 *     @stream:    (IN) Pointer to an AudioStreaming interface structure.
 *     @stream_p:  (IN) Pointer to a private audio stream data structure.
 *     @samp_freq: (IN) New sampling frequency, in Hz.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t stream_restart_freq(fd_audio_h fd, uwd_audio_stream_h stream,
    stream_priv_h stream_p, juint32_t samp_freq)
{
    uwd_audio_stream_alt_t *stream_alt = &stream->alts[stream_p->alt-1];

    KASSERT(!stream_p->aborting, ("AUDIO: stream_restart_freq while aborting\n"));

    stream_clean(stream_p);

    stream_p->samp_freq = samp_freq;
    stream_p->pipe = get_pipe(fd, stream, stream_p);

    stream_p->isoc_freq = USB_FULL_SPEED_FREQ >> (stream_alt->ep_ival - 1);

    stream_p->average.div = stream_p->samp_freq / stream_p->isoc_freq;
    stream_p->average.rem = stream_p->samp_freq % stream_p->isoc_freq;
    stream_p->average.sum = 0;
    stream_p->audio_frame_size = format_get_audio_frame_size(stream_alt);
    stream_p->sync_frames = 0;
    stream_p->max_pkt = stream_p->pipe->max_pkt_size;

    DBG_I(DSLAVE_AUDIO_STREAM, ("AUDIO: stream_restart_freq alt=%d reqs=%d "
        "samp_freq=%d isoc_freq=%d\n", stream_p->alt, stream_p->reqs, 
        stream_p->samp_freq, stream_p->isoc_freq));

    stream->select_cb(fd->app_ctx, stream, stream_alt, stream_p->samp_freq);

    return 0;
}

/**
 * Function name:  stream_reschedule
 * Description:    Starts new data transfers, provided the number of requests
 *                 does not exceed STREAM_MAX_REQ_NUM and there is more data to
 *                 send.
 * Parameters:
 *     @fd:       (IN) Audio FD context.
 *     @stream_p: (IN) Pointer to a private audio stream data structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t stream_reschedule(fd_audio_h fd, stream_priv_h stream_p)
{
    jresult_t rc;

    DBG_X(DSLAVE_AUDIO_STREAM, ("AUDIO: stream_reschedule\n"));

    while (stream_p->reqs != STREAM_MAX_REQ_NUM && stream_p->cur.buf)
    {
        juint32_t buf_to_send = stream_p->cur.buf->len - stream_p->cur.buf_ofs;
        juint_t req_to_send, to_send;
        jbool_t need_copy = 1;

        /* Create request */
        if (!stream_p->cur.req)
        {
            void *mem = NULL;

            stream_p->cur.req_len = get_next_frame_len(stream_p);
            stream_p->cur.req_ofs = 0;

            /* Verify that the data length matches the frame size */
            if (buf_to_send >= stream_p->cur.req_len)
            {
                need_copy = 0;

                mem = stream_p->cur.buf->mem;
                PTR_ADD(mem, stream_p->cur.buf_ofs);
            }

            DBG_X(DSLAVE_AUDIO_STREAM, ("AUDIO: stream_reschedule - create "
                "request req_len=%d need_copy=%d\n", stream_p->cur.req_len, 
                need_copy));

            stream_p->cur.req = core_request_alloc(stream_p->cur.req_len, mem);
            if (!stream_p->cur.req)
                goto NoMem;

            stream_p->cur.req->complete = stream_complete_cb;
            stream_p->cur.req->caller_private =
                &stream_p->flags[mem ? STREAM_FD_MEM_DESC : 0];
        }

        req_to_send = stream_p->cur.req_len - stream_p->cur.req_ofs;
        to_send = MIN(req_to_send, buf_to_send);

        if (need_copy && !stream_p->stream->read)
        {
            j_memcpy((juint8_t *)stream_p->cur.req->buffer.vaddr +
                stream_p->cur.req_ofs,
                (juint8_t *)stream_p->cur.buf->mem +
                stream_p->cur.buf_ofs, to_send);
        }

        stream_p->cur.req_ofs += to_send;
        stream_p->cur.buf_ofs += to_send;

        /* All data from the application buffer has been scheduled */
        if (stream_p->cur.buf_ofs == stream_p->cur.buf->len)
        {
            stream_p->cur.buf = STAILQ_NEXT(stream_p->cur.buf, next);
            stream_p->cur.buf_ofs = 0;
            DBG_X(DSLAVE_AUDIO_STREAM, ("AUDIO: NEXT=%p\n", stream_p->cur.buf));
        }

        /* Request buffer full */
        if (stream_p->cur.req_ofs == stream_p->cur.req_len)
        {
            request_t *req = stream_p->cur.req;
            stream_p->cur.req = NULL;

            req->transfer_size = stream_p->cur.req_len;
            stream_p->reqs++;

            DBG_V(DSLAVE_AUDIO_STREAM, ("AUDIO: stream_reschedule, sending "
                "len=%d (alt=%d reqs=%d aborting=%d)\n", req->transfer_size,
                stream_p->alt, stream_p->reqs, stream_p->aborting));

            if (stream_p->stream->read)
                rc = core_read_data(fd->core_ctx, stream_p->pipe, req);
            else
                rc = core_send_data(fd->core_ctx, stream_p->pipe, req);

            if (rc)
            {
                DBG_E(DSLAVE_AUDIO_STREAM, ("AUDIO: Send data failed, %s\n",
                    ESTR(rc)));
                req->status = REQUEST_ERROR;
                stream_complete_cb(req);
                goto Error;
            }
        }
    }
    return 0;

NoMem:
    rc = JENOMEM;
Error:
    DBG_E(DSLAVE_AUDIO_DESC, ("AUDIO: stream_reschedule failed, %s\n", 
        ESTR(rc)));

    return rc;
}

/**
 * Function name:  stream_complete_cb
 * Description:    Stream data transfer completion callback.
 * Parameters:
 *     @req: (IN) Pointer to a core request.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void stream_complete_cb(request_t *req)
{
    stream_priv_h stream_p;
    fd_audio_h fd;
    jresult_t rc;
    juint32_t req_len, done_len;
    jbool_t need_copy = 1;
    juint8_t *arg = (juint8_t *)req->caller_private;

    KASSERT(arg, ("AUDIO: stream_complete_cb, arg is NULL\n"));

    if (*arg == STREAM_FD_MEM_DESC)
    {
        need_copy = 0;
        arg -= STREAM_FD_MEM_DESC;
    }

    stream_p = (stream_priv_h)(void *)(arg - STRUCT_OFFSET(stream_p, flags));

    fd = stream_p->fd;

    stream_p->reqs--;

    rc = request_status_to_result(req->status);

    done_len = req->bytes_transferred;
    req_len = req->transfer_size;

    DBG_X(DSLAVE_AUDIO_STREAM, ("AUDIO: stream_complete_cb, len=%d(%d) (%s)"
        "(alt=%d reqs=%d aborting=%d)\n", req_len, done_len,
        need_copy ? "FD buf": "APP buf", stream_p->alt, stream_p->reqs,
        stream_p->aborting));

    if (rc)
    {
        PRINT_DBG(DSLAVE_AUDIO_CTRL, rc == JENXIO ? DL_WARNING : DL_ERROR,
            ("AUDIO: Stream xfer failed, %s\n", ESTR(rc)));
    }

    complete_bufs(fd, stream_p, req_len, done_len, rc, need_copy,
        (juint8_t *)req->buffer.vaddr);

    core_request_free(req);

    if (!stream_p->alt || stream_p->aborting)
        return;

    /* If the stream is still active - start more requests */
    stream_reschedule(fd, stream_p);
}

/**
 * Function name:  memmove_down
 * Description:    Moves data from an higher address to a lower address.
 *                 (Source and destination areas can be overlapped).
 * Parameters:
 *     @dest: (OUT) Destination memory address.
 *     @src:  (IN)  Source memory address.
 *     @len:  (IN)  Data length, in bytes.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void memmove_down(void *dest, const void *src, juint32_t len)
{
    juint32_t i;

    for (i = 0; i < len; i++)
        ((juint8_t *)dest)[i] = ((juint8_t *)src)[i];
}

/**
 * Function name:  complete_bufs
 * Description:    Notifies the application about transfer completion.
 * Parameters:
 *     @fd:        (IN) Audio FD context.
 *     @stream_p:  (IN) Pointer to a private audio stream data structure.
 *     @req_len:   (IN) Scheduled data length.
 *     @done_len:  (IN) Completed data length.
 *     @stat:      (IN) Request completion status.
 *     @need_copy: (IN) TRUE = transfer data should be copied from a temporary
 *                      buffer to an application buffer.
 *     @req_vaddr: (IN) Pointer to a temporary data buffer.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void complete_bufs(fd_audio_h fd, stream_priv_h stream_p,
    juint32_t req_len, juint32_t done_len, jresult_t stat, jbool_t need_copy,
    juint8_t *req_vaddr)
{
    stream_buf_t *buf;

    DBG_X(DSLAVE_AUDIO_STREAM, ("AUDIO: complete_bufs - len=%d(%d) "
        "(alt=%d reqs=%d aborting=%d)\n", req_len, done_len, stream_p->alt,
        stream_p->reqs, stream_p->aborting));

    while ((buf = STAILQ_FIRST(&stream_p->bufs)) != NULL)
    {
        juint32_t buf_to_send = buf->len - stream_p->done_buf_ofs;
        juint32_t buf_done = MIN(buf_to_send, done_len);

        buf->rc = stat;

        if (!stat && stream_p->stream->read && (need_copy ||
            stream_p->done_buf_actlen != stream_p->done_buf_ofs))
        {
            DBG_X(DSLAVE_AUDIO_STREAM, ("AUDIO: complete_bufs - moving data "
                "%d bytes from %p to %p\n", buf_done, req_vaddr,
                (juint8_t *)buf->mem + stream_p->done_buf_actlen));

            memmove_down((juint8_t *)buf->mem + stream_p->done_buf_actlen,
                req_vaddr, buf_done);

            req_vaddr += buf_done;
        }

        stream_p->done_buf_actlen += buf_done;
        stream_p->done_buf_ofs += req_len;

        if (req_len < buf_to_send)
            return;

        buf->len = stream_p->done_buf_actlen;

        req_len -= buf_to_send;
        done_len -= buf_done;

        stream_p->done_buf_ofs = 0;
        stream_p->done_buf_actlen = 0;

        STAILQ_REMOVE_HEAD(&stream_p->bufs, next);

        DBG_X(DSLAVE_AUDIO_STREAM, ("AUDIO: complete_bufs - notify app, len=%d "
            "status - %s\n", buf->len, ESTR(buf->rc)));

        buf->cb(fd->app_ctx, stream_p->stream, buf->arg, buf->rc, buf->len);

        jfree(buf);
    }
}

/**
 * Function name:  stream_close
 * Description:    Cleans stream data and notifies the application.
 * Parameters:
 *     @fd:       (IN) Audio FD context.
 *     @stream:   (IN) Pointer to an AudioStreaming interface structure.
 *     @stream_p: (IN) Pointer to a private audio stream data structure.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void stream_close(fd_audio_h fd, uwd_audio_stream_h stream,
    stream_priv_h stream_p)
{
    DBG_V(DSLAVE_AUDIO_STREAM, ("AUDIO: stream_close (alt=%d reqs=%d "
        "aborting=%d)\n", stream_p->alt, stream_p->reqs, stream_p->aborting));

    stream->select_cb(fd->app_ctx, stream, NULL, 0);

    stream_clean(stream_p);
}

/**
 * Function name:  stream_clean
 * Description:    Aborts stream transfers and cleans temporary data.
 * Parameters:
 *     @stream_p: (IN) Pointer to a private audio stream data structure.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void stream_clean(stream_priv_h stream_p)
{
    request_t *req = stream_p->cur.req;

    DBG_I(DSLAVE_AUDIO_STREAM, ("AUDIO: stream_clean (alt=%d reqs=%d "
        "aborting=%d)\n", stream_p->alt, stream_p->reqs, stream_p->aborting));

    stream_p->aborting = 1;

    core_abort_pipe(stream_p->fd->core_ctx, stream_p->pipe);

    if (req)
    {
        stream_p->cur.req = NULL;

        req->transfer_size = stream_p->cur.req_ofs;
        stream_p->reqs++;
        req->status = REQUEST_CANCELLED;

        stream_complete_cb(req);
    }

    stream_p->cur.buf = NULL;
    stream_p->done_buf_ofs = 0;
    stream_p->done_buf_actlen = 0;

    complete_bufs(stream_p->fd, stream_p, JUINT32_MAX, 0, JENXIO, 0, NULL);

    stream_p->aborting = 0;
}

/**
 * Function name:  request_status_to_result
 * Description:    Converts a core request status to a jresult_t status.
 * Parameters:
 *     @stat: (IN) Core request status.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t request_status_to_result(request_status_t stat)
{
    switch (stat)
    {
    case REQUEST_READY:
    case REQUEST_INPROGRESS:
        return JEBUSY;
    case REQUEST_CANCELLED:
        return JENXIO;
    case REQUEST_ERROR:
        return JEIO;
    case REQUEST_COMPLETED:
        return 0;
    default:
        return EUNKNOWN;
    }
}

/**
 * Function name:  get_next_frame_len
 * Description:    Calculates a stream's next USB frame size.
 * Parameters:
 *     @stream_p: (IN) Pointer to a private audio stream data structure.
 *
 * Return value:   USB frame size in bytes.
 * Scope:          Local
 **/
static juint16_t get_next_frame_len(stream_priv_h stream_p)
{
    juint16_t audio_frames;

    if (stream_p->stream->read)
        return stream_p->max_pkt;

    audio_frames = stream_p->average.div;
    stream_p->average.sum += stream_p->average.rem;

    if (stream_p->average.sum >= stream_p->isoc_freq)
    {
        stream_p->average.sum -= stream_p->isoc_freq;
        audio_frames++;
    }

    if (stream_p->sync_frames)
    {
        if (stream_p->sync_frames > 0)
        {
            juint16_t empty_slots =
                stream_p->max_pkt/stream_p->audio_frame_size - audio_frames;

            if (empty_slots > stream_p->sync_frames)
            {
                audio_frames += stream_p->sync_frames;
                stream_p->sync_frames = 0;
            }
            else
            {
                audio_frames += empty_slots;
                stream_p->sync_frames -= empty_slots;
            }
        }
        else
        {
            if ((jint32_t)audio_frames > -stream_p->sync_frames)
            {
                audio_frames += stream_p->sync_frames;
                stream_p->sync_frames = 0;
            }
            else
            {
                audio_frames = 0;
                stream_p->sync_frames += audio_frames;
            }
        }
    }

    return audio_frames * stream_p->audio_frame_size;
}

/**
 * Function name:  audio_stream_io
 * Description:    Schedules data read/write using the given USB audio stream.
 * Parameters:
 *     @fd_ctx:  (IN) Audio function driver context.
 *     @stream:  (IN) Pointer to an AudioStreaming interface structure.
 *     @buf: (IN/OUT) Pointer to a memory data buffer to be used for the
 *                    transfer.
 *     @len:     (IN) Data length, in bytes.
 *     @cb:      (IN) Read/Write completion application callback.
 *     @arg:     (IN) Pointer to arguments for the callback (@cb);
 *                    NULL if no arguments.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t audio_stream_io(uwd_audio_drv_h fd_ctx,
    uwd_audio_stream_h stream, void *buf, juint32_t len,
    uwd_audio_stream_cb_t cb, void *arg)
{
    fd_audio_h fd = (fd_audio_h)fd_ctx;
    stream_priv_h stream_p;
    jresult_t rc;

    KASSERT(fd_ctx, ("AUDIO: FD context is NULL\n"));

    if (!stream)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Error - Stream is NULL\n"));
        return JEINVAL;
    }

    if (!len)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Error - data len is 0\n"));
        return JEINVAL;
    }

    if (!buf)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Error - buf is NULL\n"));
        return JEINVAL;
    }

    if (!cb)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Error - cb is NULL\n"));
        return JEINVAL;
    }

    stream_p = get_stream_priv(fd, stream);
    KASSERT(stream_p, ("AUDIO: stream_p is NULL\n"));

    if (!stream_p->alt || stream_p->aborting)
        return JENXIO;

    if (len % stream_p->audio_frame_size)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Error - buffer len (%d) is not divisible "
            "with the audio frame size (%d)\n", len, stream_p->audio_frame_size));
        return JEINVAL;
    }

    if (len < stream_p->max_pkt)
    {
        DBG_W(DSLAVE_AUDIO, ("AUDIO: Warning - buffer len (%d) is smaller than "
            "usb packet size (%d), transfer will be scheduled only when "
            "scheduled data size is not less than usb packet size\n", len,
            stream_p->max_pkt));
    }

    rc = stream_add_buf(stream_p, buf, len, cb, arg);
    if (rc)
        return rc;

    stream_reschedule(fd, stream_p);

    return 0;
}


/* Audio FD --> Application Functions */

/* Audio Stream Manipulation Functions */

/**
 * Function name:  uwd_audio_stream_write
 * Description:    Sends data to the host using the given USB audio stream.
 * Parameters:
 *     @fd_ctx: (IN) Audio function driver context.
 *     @stream: (IN) Pointer to an AudioStreaming interface structure.
 *     @buf:    (IN) Pointer to a memory buffer containing the data to write.
 *     @len:    (IN) Data length, in bytes.
 *     @cb:     (IN) Write completion application callback.
 *     @arg:    (IN) Pointer to arguments for the application's write
 *                   completion callback (@cb); NULL if no arguments.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwd_audio_stream_write(uwd_audio_drv_h fd_ctx,
    uwd_audio_stream_h stream, void *buf, juint32_t len,
    uwd_audio_stream_cb_t cb, void *arg)
{
    DBG_V(DSLAVE_AUDIO_STREAM, ("AUDIO: uwd_audio_stream_write buf=%p len=%d\n",
        buf, len));

    KASSERT(!stream->read, ("AUDIO: uwd_audio_stream_write called for READ "
        "stream\n"));

    return audio_stream_io(fd_ctx, stream, buf, len, cb, arg);
}

/**
 * Function name:  uwd_audio_stream_read
 * Description:    Reads data from host using the given USB audio stream.
 * Parameters:
 *     @fd_ctx: (IN)  Audio function driver context.
 *     @stream: (IN)  Pointer to an AudioStreaming interface structure.
 *     @buf:    (OUT) Pointer to a memory buffer to be filled with the read
 *                    data.
 *     @len:    (IN)  Data length, in bytes.
 *     @cb:     (IN)  Read completion application callback.
 *     @arg:    (IN)  Pointer to arguments for the application's read
 *                    completion callback (cb); NULL if no arguments.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwd_audio_stream_read(uwd_audio_drv_h fd_ctx,
    uwd_audio_stream_h stream, void *buf, juint32_t len,
    uwd_audio_stream_cb_t cb, void *arg)
{
    DBG_V(DSLAVE_AUDIO_STREAM, ("AUDIO: uwd_audio_stream_read buf=%p len=%d\n",
        buf, len));

    KASSERT(stream->read, ("AUDIO: uwd_audio_stream_read called for WRITE "
        "stream\n"));

    return audio_stream_io(fd_ctx, stream, buf, len, cb, arg);
}

/**
 * Function name:  uwd_audio_stream_abort
 * Description:    Aborts all transfers for the given USB audio stream.
 * Parameters:
 *     @fd_ctx: (IN) Audio function driver context.
 *     @stream: (IN) Pointer to an AudioStreaming interface structure.
 *
 * Return value:   None
 * Scope:          Global
 **/
void uwd_audio_stream_abort(uwd_audio_drv_h fd_ctx, uwd_audio_stream_h stream)
{
    fd_audio_h fd = (fd_audio_h)fd_ctx;
    stream_priv_h stream_p;

    KASSERT(fd_ctx, ("AUDIO: FD context is NULL\n"));

    if (!stream)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Stream is NULL\n"));
        return;
    }

    stream_p = get_stream_priv(fd, stream);
    KASSERT(stream_p, ("AUDIO: stream_p is NULL\n"));

    DBG_I(DSLAVE_AUDIO_STREAM, ("AUDIO: uwd_audio_stream_abort\n"));

    stream_clean(stream_p);
}

/**
 * Function name:  uwd_audio_stream_sync
 * Description:    Synchronizes the USB stream clock with the audio hardware
 *                 clock.
 *
 *                 By default, the size of the USB packets used for the audio
 *                 stream are calculated by the Audio FD according to the Audio
 *                 Data Formats Specification. The uwd_audio_stream_sync()
 *                 function is used to temporarily change the size of the
 *                 stream's USB packets in order to synchronize between the USB
 *                 and audio data transfer rates.
 *
 *                 NOTE:
 *                 In order to use this function, the maximum USB endpoint size
 *                 of the audio stream's alternate setting(s) - as set in the
 *                 stream->alts[i].ep_max_packet field(s) - must be large
 *                 enough to contain both the standard audio data and the
 *                 additional synchronization data.
 * Parameters:
 *     @fd_ctx:       (IN) Audio function driver context.
 *     @stream:       (IN) Pointer to an AudioStreaming interface structure.
 *     @audio_frames: (IN) The current delta, in audio frames, between the USB
 *                         stream and audio hardware clocks.
 *                         If the audio hardware clock is faster than the USB
 *                         stream clock, @audio_frames should be positive;
 *                         otherwise - negative.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwd_audio_stream_sync(uwd_audio_drv_h fd_ctx,
    uwd_audio_stream_h stream, jint32_t audio_frames)
{
    fd_audio_h fd = (fd_audio_h)fd_ctx;
    stream_priv_h stream_p;

    KASSERT(fd_ctx, ("AUDIO: FD context is NULL\n"));

    if (!audio_frames)
        return 0;

    if (!stream)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Error - Stream is NULL\n"));
        return JEINVAL;
    }

    if (stream->read)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: uwd_audio_stream_sync called for READ "
            "stream\n"));
        return JEINVAL;
    }

    stream_p = get_stream_priv(fd, stream);
    KASSERT(stream_p, ("AUDIO: stream_p is NULL\n"));

    DBG_I(DSLAVE_AUDIO, ("AUDIO: uwd_audio_stream_sync %d (USB max packet=%d, "
        "audio frames in USB packet min=%d max=%d)\n", audio_frames,
        stream_p->max_pkt, stream_p->average.div,
        stream_p->average.div + (stream_p->average.rem ? 1: 0)));

    if (audio_frames > 0 &&
        (stream_p->average.div*stream_p->audio_frame_size) == stream_p->max_pkt)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: uwd_audio_stream_sync failed, no space in "
            "USB frame for syncronization audio data\n"));
        return JEINVAL;
    }

    stream_p->sync_frames += audio_frames;

    return 0;
}


/** Status Interrupt Endpoint Functions
 *  Applicable only if the device is implemented to include a status interrupt
 *  endpoint (optional).
 **/

/**
 * Function name:  uwd_audio_entity_intr
 * Description:    Notifies the host of an AudioControl interface interrupt
 *                 (entity status change).
 *                 Status interrupt endpoint bStatusType status word field:
 *                 Bit D7=1    Bit D6=0    Bits D3..0=0 (originator=ACI)
 *                  
 * Parameters:
 *     @fd_ctx: (IN) Audio function driver context.
 *     @entity: (IN) Pointer to an entity structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwd_audio_entity_intr(uwd_audio_drv_h fd_ctx,
    uwd_audio_entity_h entity)
{
    fd_audio_h fd = (fd_audio_h)fd_ctx;
    audio_intr_data_t data;

    KASSERT(fd_ctx, ("AUDIO: FD context is NULL\n"));

    data.bOriginator = entity ? entity->id : 0;
    data.bStatusType = AUDIO_INTR_ORIG_CTRL_IFACE | AUDIO_INTR_PENDING;

    return audio_send_intr(fd, &data);
}

/**
 * Function name:  uwd_audio_memory_intr
 * Description:    Notifies the host of an AudioControl interface interrupt
 *                 for an entity memory contents change.
 *                 Status interrupt endpoint bStatusType status word field:
 *                 Bit D7=0    Bit D6=1    Bits D3..0=0 (originator=ACI)
 * Parameters:
 *     @fd_ctx: (IN) Audio function driver context.
 *     @entity: (IN) Pointer to an entity structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwd_audio_memory_intr(uwd_audio_drv_h fd_ctx,
    uwd_audio_entity_h entity)
{
    fd_audio_h fd = (fd_audio_h)fd_ctx;
    audio_intr_data_t data;

    KASSERT(fd_ctx, ("AUDIO: FD context is NULL\n"));

    if (!entity)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Entity is NULL\n"));
        return JEINVAL;
    }

    data.bOriginator = entity->id;
    data.bStatusType = AUDIO_INTR_ORIG_CTRL_IFACE | AUDIO_INTR_MEM_CHANGED;

    return audio_send_intr(fd, &data);
}

/**
 * Function name:  uwd_audio_stream_intr
 * Description:    Notifies the host of an AudioStreaming interface interrupt.
 *                 Status interrupt endpoint bStatusType status word field:
 *                 Bit D7=1    Bit D6=0    Bits D3..0=1 (originator=ASI)
 * Parameters:
 *     @fd_ctx: (IN) Audio function driver context.
 *     @stream: (IN) Pointer to an AudioStreaming interface structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwd_audio_stream_intr(uwd_audio_drv_h fd_ctx,
    uwd_audio_stream_h stream)
{
    fd_audio_h fd = (fd_audio_h)fd_ctx;
    audio_intr_data_t data;

    KASSERT(fd_ctx, ("AUDIO: FD context is NULL\n"));

    if (!stream)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Stream is NULL\n"));
        return JEINVAL;
    }

    data.bOriginator = get_stream_index(fd, stream);
    data.bStatusType = AUDIO_INTR_ORIG_STREAM_IFACE | AUDIO_INTR_PENDING;

    return audio_send_intr(fd, &data);
}

/**
 * Function name:  uwd_audio_endpoint_intr
 * Description:    Notifies the host of an AudioStreaming endpoint interrupt.
 *                 Status interrupt endpoint bStatusType status word field:
 *                 Bit D7=1    Bit D6=0    Bits D3..0=2 (originator=AS endpoint)
 * Parameters:
 *     @fd_ctx: (IN) Audio function driver context.
 *     @stream: (IN) Pointer to an AudioStreaming interface structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwd_audio_endpoint_intr(uwd_audio_drv_h fd_ctx,
    uwd_audio_stream_h stream)
{
    fd_audio_h fd = (fd_audio_h)fd_ctx;
    audio_intr_data_t data;
    stream_priv_h stream_p;
    pipe_desc_t *pipe;

    KASSERT(fd_ctx, ("AUDIO: FD context is NULL\n"));

    if (!stream)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: Stream is NULL\n"));
        return JEINVAL;
    }

    stream_p = get_stream_priv(fd, stream);
    KASSERT(stream_p, ("AUDIO: stream_p is NULL\n"));

    pipe = get_pipe(fd, stream, stream_p);
    if (!pipe)
    {
        DBG_E(DSLAVE_AUDIO, ("AUDIO: pipe is NULL\n"));
        return JEINVAL;
    }

    data.bOriginator = pipe->address;
    data.bStatusType = AUDIO_INTR_ORIG_STREAM_EP | AUDIO_INTR_PENDING;

    return audio_send_intr(fd, &data);
}


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
/* Audio Sample Application */

#include <jusb_common.h>
#include <uw_device_audio.h>

#define ELEMENTS_IN_ARRAY(arr) (sizeof(arr) / sizeof(arr[0]))

extern unsigned char audio_data[];
extern unsigned long audio_data_size;

typedef struct {
    juint32_t xfered;
    juint32_t buf_len;
} stream_info_t;

typedef struct {
    uwd_audio_drv_h fd_ctx;

    stream_info_t mic;
    stream_info_t speaker;

} app_ctx_t, *app_ctx_h;

typedef struct {
    struct {
        juint8_t cur;
    } mute;
    struct {
        juint16_t cur;
        juint16_t res;
        juint16_t min;
        juint16_t max;
    } volume;
} app_feature_unit_t;

static jresult_t feature_unit_cb(uwd_audio_app_h app, uwd_audio_entity_h entity,
    uwd_audio_req_code_t code, juint16_t value, juint16_t len, void *data);

static jresult_t selector_unit_cb(uwd_audio_app_h app, uwd_audio_entity_h entity,
    uwd_audio_req_code_t code, juint16_t value, juint16_t len, void *data);

static void mic_stream_select_cb(uwd_audio_app_h app, uwd_audio_stream_h stream,
    uwd_audio_stream_alt_h stream_alt, juint32_t samp_freq);

static jresult_t extension_unit_cb(uwd_audio_app_h app,
    uwd_audio_entity_h entity, uwd_audio_req_code_t code, juint16_t value,
    juint16_t len, void *data);

static void speaker_stream_select_cb(uwd_audio_app_h app,
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt,
    juint32_t samp_freq);

/*        1                2                      3               4
 *       MIC   -------- SELECTOR ------------ EXTENSION ------   USB
 *      IN TRM       --   UNIT             --   UNIT           OUT TRM
 *                  |                     |
 *        3         |            5        |                       6
 *       USB        |         FEATURE     |                    SPEAKER
 *      IN TRM --------------  UNIT   ------------------------ OUT TRM
 */

#define MIC_TRM_ID         1
#define SELECTOR_UNIT_ID   2
#define EXTENSION_UNIT_ID  3
#define USB_OT_ID          4
#define USB_IT_ID          5
#define FEATURE_UNIT_ID    6
#define SPEAKER_TRM_ID     7

#define MONO     1
#define STEREO   2

#define CHANNELS 2
#define CHANNEL_CONFIG (UWD_AUDIO_CC_LEFT_FRONT | UWD_AUDIO_CC_RIGHT_FRONT)

/* Microphone Input Terminal */
static uwd_audio_in_trm_t mic_trm = {
    UWD_AUDIO_TT_MICROPHONE,                            /* trm_type       */
    0,                                                  /* assoc_terminal */
    CHANNELS,                                           /* nr_channels    */
    CHANNEL_CONFIG                                      /* channel_config */
};

/* Selector Unit */
static juint8_t selector_sources[] = {
    MIC_TRM_ID,                                         /* source_id      */
    USB_IT_ID                                           /* source_id      */
};
static uwd_audio_selector_unit_t selector_unit = {
    ELEMENTS_IN_ARRAY(selector_sources),                /* nr_in_pins     */
    selector_sources                                    /* *sources       */
};
static juint8_t selector_state = 1;

/* Extension Unit */
static juint8_t extension_unit_sources[] = {
    SELECTOR_UNIT_ID,
    FEATURE_UNIT_ID
};
static uwd_audio_extension_unit_t extension_unit = {
    0x1234,                                             /* extension_code */
    ELEMENTS_IN_ARRAY(extension_unit_sources),          /* nr_in_pins     */
    extension_unit_sources,                             /* *source_id     */
    CHANNELS,                                           /* nr_channels    */
    CHANNEL_CONFIG,                                     /* channel_config */
    UWD_AUDIO_EC_ENABLE_PROCESSING                      /* controls       */
};
static juint8_t ext_unit_enabled = 0;

/* USB Output Terminal */
static uwd_audio_out_trm_t usb_ot = {
    UWD_AUDIO_TT_USB_STREAMING,                         /* trm_type       */
    0,                                                  /* assoc_terminal */
    EXTENSION_UNIT_ID                                   /* source_id      */
};

/* USB Input Terminal */
static uwd_audio_in_trm_t usb_it = {
    UWD_AUDIO_TT_USB_STREAMING,                         /* trm_type       */
    0,                                                  /* assoc_terminal */
    CHANNELS,                                           /* nr_channels    */
    CHANNEL_CONFIG                                      /* channel_config */
};

/* Feature Unit */
static juint16_t feature_controls[CHANNELS + 1] = {
    UWD_AUDIO_FC_MUTE | UWD_AUDIO_FC_VOLUME,            /* master channel */
    UWD_AUDIO_FC_MUTE | UWD_AUDIO_FC_VOLUME,            /* front left     */
    UWD_AUDIO_FC_MUTE | UWD_AUDIO_FC_VOLUME             /* front right    */
};
static uwd_audio_feature_unit_t feature_unit = {
    USB_IT_ID,                                          /* source_id      */
    CHANNELS,                                           /* nr_channels    */
    feature_controls                                    /* *controls      */
};
static app_feature_unit_t app_feature_ctx[] = {
    { /* master channel */
        {0},        /* cur mute */
        {
            0,      /* cur volume */
            0x0010, /* res volume */
            0xd700, /* min volume */
            0xf400  /* max volume */
        }
    },
    { /* left channel */
        {0},        /* cur mute */
        {
            0,      /* cur volume */
            0x0010, /* res volume */
            0xd700, /* min volume */
            0xf400  /* max volume */
        }
    },
    { /* right channel */
        {0},        /* cur mute */
        {
            0,      /* cur volume */
            0x0010, /* res volume */
            0xd700, /* min volume */
            0xf400  /* max volume */
        }
    }
};

/* Speaker Output Terminal */
static uwd_audio_out_trm_t speaker_trm = {
    UWD_AUDIO_TT_SPEAKER,                   /* trm_type       */
    0,                                      /* assoc_terminal */
    FEATURE_UNIT_ID                         /* source_id      */
};

static uwd_audio_entity_t entities[] = {
    {
        MIC_TRM_ID,                         /* id             */
        UWD_AUDIO_ET_INPUT_TERMINAL,        /* type           */
        &mic_trm,                           /* *data          */
        "Microphone",                       /* *string        */
        NULL,                               /* req_cb         */
        NULL                                /* *app_priv      */
    },
    {
        SELECTOR_UNIT_ID,                   /* id             */
        UWD_AUDIO_ET_SELECTOR_UNIT,         /* type           */
        &selector_unit,                     /* *data          */
        "Selector Unit",                    /* *string        */
        selector_unit_cb,                   /* req_cb         */
        &selector_state                     /* *app_priv      */
    },
    {
        EXTENSION_UNIT_ID,                  /* id             */
        UWD_AUDIO_ET_EXTENSION_UNIT,        /* type           */
        &extension_unit,                    /* *data          */
        "Extension Unit",                   /* *string        */
        extension_unit_cb,                  /* req_cb         */
        &ext_unit_enabled                   /* *app_priv      */
    },
    {
        USB_OT_ID,                          /* id             */
        UWD_AUDIO_ET_OUTPUT_TERMINAL,       /* type           */
        &usb_ot,                            /* *data          */
        "USB IN Stream",                    /* *string        */
        NULL,                               /* req_cb         */
        NULL                                /* *app_priv      */
    },
    {
        USB_IT_ID,                          /* id             */
        UWD_AUDIO_ET_INPUT_TERMINAL,        /* type           */
        &usb_it,                            /* *data          */
        "USB OUT Stream",                   /* *string        */
        NULL,                               /* req_cb         */
        NULL                                /* *app_priv      */
    },
    {
        FEATURE_UNIT_ID,                    /* id             */
        UWD_AUDIO_ET_FEATURE_UNIT,          /* type           */
        &feature_unit,                      /* *data          */
        "Feature Unit",                     /* *string        */
        feature_unit_cb,                    /* req_cb         */
        app_feature_ctx                     /* *app_priv      */
    },
    {
        SPEAKER_TRM_ID,                     /* id             */
        UWD_AUDIO_ET_OUTPUT_TERMINAL,       /* type           */
        &speaker_trm,                       /* *data          */
        "Speaker",                          /* *string        */
        NULL,                               /* req_cb         */
        NULL                                /* *app_priv      */
    }
};

/* Sampling Frequencies */

static juint32_t freq1[] = {16000};
static uwd_audio_freq_discrete_t samp_freq_16k  = {
    ELEMENTS_IN_ARRAY(freq1),
    freq1
};

static juint32_t freq2[] = {16000, 22050};
static uwd_audio_freq_discrete_t samp_freqs_16k_22k = {
    ELEMENTS_IN_ARRAY(freq2),
    freq2
};

/* Microphone Formats */
static uwd_audio_format_I_t mic_formats[] = {
    { /* 16 kHz 16 bits Mono */
        MONO,                               /* nr_channels */
        16,                                 /* bit_resolution */
        &samp_freq_16k,                     /* *samp_freqs */
        NULL                                /* *freq_continuous */
    },
    { /* 16/22.1 kHz 16 bits Stereo */
        STEREO,                             /* nr_channels */
        16,                                 /* bit_resolution */
        &samp_freqs_16k_22k,                /* *samp_freqs */
        NULL                                /* *freq_continuous */
    }
};

/* Microphone Stream Alternate Settings */
static uwd_audio_stream_alt_t mic_stream_alts[] = {
    {
        UWD_AUDIO_IVAL_1_MS,                /* ep_ival */
        UWD_AUDIO_FD_CALC_MAX_PACKET,       /* ep_max_packet */
        USB_OT_ID,                          /* trm_link */
        1,                                  /* delay */
        UWD_AUDIO_DF_I_PCM,                 /* format_type */
        &mic_formats[0]                     /* format_data */
    },
    {
        UWD_AUDIO_IVAL_1_MS,                /* ep_ival */
        UWD_AUDIO_FD_CALC_MAX_PACKET,       /* ep_max_packet */
        USB_OT_ID,                          /* trm_link */
        1,                                  /* delay */
        UWD_AUDIO_DF_I_PCM,                 /* format_type */
        &mic_formats[1]                     /* format_data */
    }
};

/* Speaker Formats */
static uwd_audio_format_I_t speaker_formats[] = {
    { /* 16 kHz 16 bits Mono */
        MONO,                               /* nr_channels */
        16,                                 /* bit_resolution */
        &samp_freq_16k,                     /* *samp_freqs */
        NULL                                /* *freq_continuous */
    },
    { /* 16/22.1 kHz 16 bits Stereo */
        STEREO,                             /* nr_channels */
        16,                                 /* bit_resolution */
        &samp_freqs_16k_22k,                /* *samp_freqs */
        NULL                                /* *freq_continuous */
    }
};

/* Speaker Stream Alternate Settings */
static uwd_audio_stream_alt_t speaker_stream_alts[] = {
    {
        UWD_AUDIO_IVAL_1_MS,                /* ep_ival */
        UWD_AUDIO_FD_CALC_MAX_PACKET,       /* ep_max_packet */
        USB_IT_ID,                          /* trm_link */
        1,                                  /* delay */
        UWD_AUDIO_DF_I_PCM,                 /* format_type */
        &speaker_formats[0]                 /* format_data */
    },
    {
        UWD_AUDIO_IVAL_1_MS,                /* ep_ival */
        UWD_AUDIO_FD_CALC_MAX_PACKET,       /* ep_max_packet */
        USB_IT_ID,                          /* trm_link */
        1,                                  /* delay */
        UWD_AUDIO_DF_I_PCM,                 /* format_type */
        &speaker_formats[1]                 /* format_data */
    }
};

static uwd_audio_stream_t streams[] = {
    {
        ELEMENTS_IN_ARRAY(mic_stream_alts),     /* alts_num */
        mic_stream_alts,                        /* *alts */
        mic_stream_select_cb,                   /* select_cb */
        NULL,                                   /* req_cb */
        NULL,                                   /* ep_req_cb */
        NULL                                    /* *app_priv */
    },
    {
        ELEMENTS_IN_ARRAY(speaker_stream_alts), /* alts_num */
        speaker_stream_alts,                    /* *alts */
        speaker_stream_select_cb,               /* select_cb */
        NULL,                                   /* req_cb */
        NULL,                                   /* ep_req_cb */
        NULL                                    /* *app_priv */
    }
};

static void uninit(uwd_audio_app_h app_ctx)
{
    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: uninit\n"));
    jfree(app_ctx);
}

/* Audio Application Information
 * (Should be available and unchanged until uninit callback called) */
static uwd_audio_config_t app_config = {
    entities,                    /* *entities */
    ELEMENTS_IN_ARRAY(entities), /* entities_num */
    streams,                     /* *streams */
    ELEMENTS_IN_ARRAY(streams),  /* streams_num */
    UWD_AUDIO_IVAL_16_MS,        /* intr_ep_ival */
    uninit                       /* uninit */
};

jresult_t uwd_audio_app_init(uwd_audio_drv_h fd_ctx, uwd_audio_app_h *app_ctx,
    uwd_audio_config_t **config)
{
    app_ctx_h app;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: uwd_audio_app_init\n"));

    app = (app_ctx_h)jmalloc(sizeof(app_ctx_t), M_ZERO);
    if(!app)
    {
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: No memory\n"));
        return JENOMEM;
    }

    app->fd_ctx = fd_ctx;

    *config = &app_config;
    *app_ctx = app;

    return 0;
}

static void mic_complete(uwd_audio_app_h app_ctx, uwd_audio_stream_h stream,
    void *arg, jresult_t stat, juint32_t len)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    void *buf = arg;

    if (stat)
        goto Error;

    app->mic.xfered += len;

    DBG_V(DSLAVE_AUDIO_APP, ("AUDIO_APP: mic_complete done=%d(%d)\n", len,
        app->mic.xfered));

    stat = uwd_audio_stream_write(app->fd_ctx, stream, buf, app->mic.buf_len,
        mic_complete, buf);

    if (!stat)
        return;
Error:
    PRINT_DBG(DSLAVE_AUDIO_APP, stat == JENXIO ? DL_VERBOSE : DL_ERROR,
        ("AUDIO_APP: mic_complete error, stat=%d (xfered=%d)\n", stat,
        app->mic.xfered));
}

static void mic_stream_select_cb(uwd_audio_app_h app_ctx,
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt,
    juint32_t samp_freq)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    uwd_audio_format_I_t *fmt;
    jresult_t rc;
    void *buf1, *buf2;

    if (!stream_alt)
    {
        DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: Microphone stream disabled, "
            "xfered=%d\n", app->mic.xfered));
        return;
    }

    fmt = (uwd_audio_format_I_t *)stream_alt->format;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: Microphone stream enabled, "
        "freq %d Hz %dbits %s\n", samp_freq, fmt->bit_resolution,
        fmt->nr_channels == MONO ? "MONO":"STEREO"));

    /* TODO audio data for all settings */
    if (fmt->bit_resolution != 16 || fmt->nr_channels != STEREO ||
        samp_freq != 22050)
    {
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: Audio settings not supported\n"));
        return;
    }

    /* TODO configure hardware for selected audio settings */

    app->mic.buf_len = audio_data_size/2;
    app->mic.xfered = 0;

    /* Use double buffering for stable streaming */
    buf1 = &audio_data[0];
    buf2 = &audio_data[app->mic.buf_len];

    rc = uwd_audio_stream_write(app->fd_ctx, stream, buf1, 
        app->mic.buf_len, mic_complete, buf1);
    if (rc)
        goto Error;

    rc = uwd_audio_stream_write(app->fd_ctx, stream, buf2, 
        app->mic.buf_len, mic_complete, buf2);
    if (rc)
        goto Error;

    return;

Error:
    DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: Microphone stream start failed, rc=%d"
        "\n", rc));
}

#define SPEAKER_BUFFER_SIZE    400
static juint8_t speaker_bufs[2][SPEAKER_BUFFER_SIZE];

static void speaker_complete(uwd_audio_app_h app_ctx, uwd_audio_stream_h stream,
    void *arg, jresult_t stat, juint32_t len)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    void *buf = arg;

    if (stat)
        goto Error;

    app->speaker.xfered += len;

    DBG_V(DSLAVE_AUDIO_APP, ("AUDIO_APP: speaker_complete done=%d(%d)\n", len,
        app->speaker.xfered));

    /* TODO send data to hardware */

    /* Restart transfer using the same buffer */
    stat = uwd_audio_stream_read(app->fd_ctx, stream, buf, 
        app->speaker.buf_len, speaker_complete, buf);

    if (!stat)
        return;
Error:
    PRINT_DBG(DSLAVE_AUDIO_APP, stat == JENXIO ? DL_VERBOSE : DL_ERROR,
        ("AUDIO_APP: speaker_complete error, stat=%d (xfered=%d)\n", stat,
        app->speaker.xfered));
}

static void speaker_stream_select_cb(uwd_audio_app_h app_ctx,
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt,
    juint32_t samp_freq)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    uwd_audio_format_I_t *fmt;
    jresult_t rc;
    void *buf1, *buf2;

    if (!stream_alt)
    {
        DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: Speaker stream disabled, "
            "xfered=%d\n", app->speaker.xfered));
        return;
    }

    fmt = (uwd_audio_format_I_t *)stream_alt->format;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: Speaker stream enabled, freq %d Hz "
        "%dbits %s\n", samp_freq, fmt->bit_resolution,
        fmt->nr_channels == MONO ? "MONO":"STEREO"));

    /* TODO configure hardware for selected audio settings */

    app->speaker.buf_len = SPEAKER_BUFFER_SIZE;
    app->speaker.xfered = 0;

    /* Use double buffering for stable streaming */

    buf1 = &speaker_bufs[0][0];
    buf2 = &speaker_bufs[1][0];

    rc = uwd_audio_stream_read(app->fd_ctx, stream, buf1,
        app->speaker.buf_len, speaker_complete, buf1);
    if (rc)
        goto Error;

    rc = uwd_audio_stream_read(app->fd_ctx, stream, buf2,
        app->speaker.buf_len, speaker_complete, buf2);
    if (rc)
        goto Error;

    return;

Error:
    DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: Speaker stream start failed, rc=%d\n",
        rc));
}

/* Feature Unit Requests Handling */
static jresult_t feature_unit_cb(uwd_audio_app_h app_ctx,
    uwd_audio_entity_h entity, uwd_audio_req_code_t code, juint16_t value,
    juint16_t len, void *data)
{
    juint8_t ch = (juint8_t)value;
    uwd_audio_feature_unit_cs_t cs = (uwd_audio_feature_unit_cs_t)(value >> 8);
    app_feature_unit_t *fu = (app_feature_unit_t *)entity->app_priv;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: Feature Unit (%d) request, code=0x%02x"
        " channel=%d control selector=0x%02x\n", entity->id, code, ch, cs));

    if (ch == UWD_AUDIO_CTRL_ALL_CH)
        return JENOTSUP;

    switch (cs)
    {
    case UWD_AUDIO_FU_MUTE_CONTROL:
        {
            juint8_t *mute = (juint8_t *)data;
            if (len < sizeof(fu[ch].mute.cur))
                return JEINVAL;

            switch (code)
            {
            case UWD_AUDIO_RC_SET_CUR:
                fu[ch].mute.cur = *mute ? 1 : 0;
                /* TODO change HW mute */
                return 0;
            case UWD_AUDIO_RC_GET_CUR:
                *mute = fu[ch].mute.cur;
                return 0;
            case UWD_AUDIO_RC_GET_RES:
                *mute = 0;
                return 0;
            default:
                return JENOTSUP;
            }
        }

    case UWD_AUDIO_FU_VOLUME_CONTROL:
        {
            juint16_t volume;
            if (len < sizeof(volume))
                return JEINVAL;

            switch (code)
            {
            case UWD_AUDIO_RC_SET_CUR:

                volume = ua_le16toh(data);
                if (volume > fu[ch].volume.max || volume < fu[ch].volume.min)
                    return JEINVAL;

                fu[ch].volume.cur = ua_le16toh(data);
                /* TODO change HW volume */
                return 0;

            case UWD_AUDIO_RC_GET_CUR:
                ua_htole16(data, fu[ch].volume.cur);
                return 0;
            case UWD_AUDIO_RC_GET_MIN:
                ua_htole16(data, fu[ch].volume.min);
                return 0;
            case UWD_AUDIO_RC_GET_MAX:
                ua_htole16(data, fu[ch].volume.max);
                return 0;
            case UWD_AUDIO_RC_GET_RES:
                ua_htole16(data, fu[ch].volume.res);
                return 0;

            default:
                return JENOTSUP;
            }
        }

    default:
        return JENOTSUP;
    }
}

/* Selector Unit Requests Handling */
static jresult_t selector_unit_cb(uwd_audio_app_h app, uwd_audio_entity_h entity,
    uwd_audio_req_code_t code, juint16_t value, juint16_t len, void *data)
{
    uwd_audio_selector_unit_t *su = (uwd_audio_selector_unit_t *)entity->data;
    juint8_t *selector_cur = (juint8_t *)entity->app_priv;
    juint8_t *selector_req = (juint8_t *)data;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: Selector Unit (%d) request, "
        "code=0x%02x\n", entity->id, code));

    if (len < sizeof(juint8_t))
        return JEINVAL;

    switch (code)
    {
    case UWD_AUDIO_RC_SET_CUR:

        if (*selector_req == 0)
            *selector_req = 1;
        else if (*selector_req > su->nr_in_pins)
            *selector_req = su->nr_in_pins;

        *selector_cur = *selector_req;

        /* TODO change selector */
        return 0;

    case UWD_AUDIO_RC_GET_CUR:
        *selector_req = *selector_cur;
        return 0;
    case UWD_AUDIO_RC_GET_MIN:
    case UWD_AUDIO_RC_GET_RES:
        *selector_req = 1;
        return 0;
    case UWD_AUDIO_RC_GET_MAX:
        *selector_req = su->nr_in_pins;
        return 0;

    default:
        return JENOTSUP;
    }
}

/* Extension Unit Requests Handling */
static jresult_t extension_unit_cb(uwd_audio_app_h app,
    uwd_audio_entity_h entity, uwd_audio_req_code_t code, juint16_t value,
    juint16_t len, void *data)
{
    uwd_audio_extension_unit_cs_t cs = (uwd_audio_extension_unit_cs_t)
        (value >> 8);

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: Extension Unit (%d) request, "
        "code=0x%02x control selector=0x%02x\n", entity->id, code, cs));

    switch (cs)
    {
    case UWD_AUDIO_EU_ENABLE_CONTROL:
        {
            juint8_t *ext_req = (juint8_t *)data;
            juint8_t *enabled = (juint8_t *)entity->app_priv;

            if (len < sizeof(juint8_t))
                return JEINVAL;

            switch (code)
            {
            case UWD_AUDIO_RC_SET_CUR:

                *enabled = *ext_req ? 1 : 0;

                /* TODO enable/disable data processing */
                return 0;

            case UWD_AUDIO_RC_GET_CUR:
                *ext_req = *enabled;
                return 0;
            case UWD_AUDIO_RC_GET_RES:
                *ext_req = 0;
                return 0;

            default:
                return JENOTSUP;
            }
        }
    default:
        /* Vendor-specific controls can be processed here */

        return JENOTSUP;
    }
}


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
/* Audio Loopback Application */

#include <jusb_common.h>
#include <uw_device_audio.h>
#ifdef CONFIG_FD_AUDIO_LOOPBACK_H2D
#include <lists.h>

#define BUFFERING_LIMIT_MS 10000

typedef struct buf_desc {
    jdma_handle dma;
    juint32_t len;
    STAILQ_ENTRY(buf_desc) next;
} buf_desc_t, *buf_desc_h;

#endif

#define BUFFERING_TIME_MS  20

typedef struct {
    jbool_t started;
    juint32_t xfered;
} stream_info_t;

typedef struct {
    uwd_audio_drv_h fd;

    stream_info_t read;
    stream_info_t write;

    juint32_t buf_len;
    juint32_t samp_freq;
    juint8_t  samp_len;

#ifdef CONFIG_FD_AUDIO_LOOPBACK_H2D
    STAILQ_HEAD(done_list, buf_desc) done;

    juint32_t buffered;
    uwd_audio_stream_h wstream;
#endif
} app_ctx_t, *app_ctx_h;

/*          1                        2
 *        MIC IT  ---------------  USB OT 
 *                             (IN streaming)
 *     
 *          3                        4
 *        USB IT  --------------  SPEAKER OT
 *    (OUT streaming)
 */

#define ELEMENTS_IN_ARRAY(arr) (sizeof(arr) / sizeof(arr[0]))

#define MIC_TRM_ID 1
#define USB_OT_ID 2
#define USB_IT_ID 3
#define SPEAKER_TRM_ID 4

#define MONO 1
#define STEREO 2
#define CHANNELS 2

/* Microphone Input Terminal */
static uwd_audio_in_trm_t mic_trm = {
    UWD_AUDIO_TT_MICROPHONE,                            /* trm_type       */
    USB_OT_ID,                                          /* assoc_terminal */
    CHANNELS,                                           /* nr_channels    */
    UWD_AUDIO_CC_LEFT_FRONT | UWD_AUDIO_CC_RIGHT_FRONT  /* channel_config */
};

/* USB Output Terminal */
static uwd_audio_out_trm_t usb_ot = {
    UWD_AUDIO_TT_USB_STREAMING,             /* trm_type       */
    MIC_TRM_ID,                             /* assoc_terminal */
    MIC_TRM_ID                              /* source_id      */
};

/* USB Input Terminal */
static uwd_audio_in_trm_t usb_it = {
    UWD_AUDIO_TT_USB_STREAMING,                         /* trm_type       */
    SPEAKER_TRM_ID,                                     /* assoc_terminal */
    CHANNELS,                                           /* nr_channels    */
    UWD_AUDIO_CC_LEFT_FRONT | UWD_AUDIO_CC_RIGHT_FRONT  /* channel_config */
};

/* Speaker Output Terminal */
static uwd_audio_out_trm_t speaker_trm = {
    UWD_AUDIO_TT_SPEAKER,                   /* trm_type       */
    USB_IT_ID,                              /* assoc_terminal */
    USB_IT_ID                               /* source_id      */
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

/* Data Formats */
static uwd_audio_format_I_t format_16k_16bit_mono = {
    MONO,                               /* nr_channels */
    16,                                 /* bit_resolution */
    &samp_freq_16k,                     /* *samp_freqs */
    NULL                                /* *freq_continuous */
};

/* Microphone Stream Alternate Settings */
static uwd_audio_stream_alt_t mic_stream_alts[] = {
    {
        UWD_AUDIO_IVAL_1_MS,                /* ep_ival */
        UWD_AUDIO_FD_CALC_MAX_PACKET,       /* ep_max_packet */
        USB_OT_ID,                          /* trm_link */
        1,                                  /* delay */
        UWD_AUDIO_DF_I_PCM,                 /* format_type */
        &format_16k_16bit_mono              /* format_data */
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
        &format_16k_16bit_mono              /* format_data */
    }
};

static void stream_select_cb(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt, 
    juint32_t samp_freq);

static uwd_audio_stream_t streams[] = {
    {
        ELEMENTS_IN_ARRAY(mic_stream_alts),     /* alts_num */
        mic_stream_alts,                        /* *alts */
        stream_select_cb,                       /* select_cb */
        NULL,                                   /* req_cb */
        NULL,                                   /* ep_req_cb */
        NULL                                    /* *app_priv */
    },
    {
        ELEMENTS_IN_ARRAY(speaker_stream_alts), /* alts_num */
        speaker_stream_alts,                    /* *alts */
        stream_select_cb,                       /* select_cb */
        NULL,                                   /* req_cb */
        NULL,                                   /* ep_req_cb */
        NULL                                    /* *app_priv */
    }
};

#ifdef CONFIG_FD_AUDIO_LOOPBACK_H2D
static void done_free_all(app_ctx_h app);
#endif

static void uninit(uwd_audio_app_h app_ctx)
{
    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: uninit\n"));
#ifdef CONFIG_FD_AUDIO_LOOPBACK_H2D
    done_free_all((app_ctx_h)app_ctx);
#endif
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
    
    app->fd = fd_ctx;

    *config = &app_config;
    *app_ctx = app;

#ifdef CONFIG_FD_AUDIO_LOOPBACK_H2D
    /* Microphone stream */
    app->wstream = &streams[0];
    STAILQ_INIT(&app->done);
#endif
    return 0;
}

static jbool_t audio_stream_init(app_ctx_h app, 
    uwd_audio_stream_alt_h stream_alt, juint32_t samp_freq)
{
    uwd_audio_format_I_t *fmt = (uwd_audio_format_I_t *)stream_alt->format;
    juint32_t samp_len = UWD_AUDIO_SUBFRAME_SIZE(fmt) * fmt->nr_channels;

    app->buf_len = samp_freq * samp_len * BUFFERING_TIME_MS / 1000;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: audio_stream_init, freq %d Hz %d "
        "bits %s (buf_len=%d)\n", samp_freq, fmt->bit_resolution, 
        fmt->nr_channels == MONO ? "MONO":"STEREO", app->buf_len));

    if (!app->read.started && !app->write.started)
    {
        app->samp_freq = samp_freq;
        app->samp_len = samp_len;
        return 1;
    }

    if (app->samp_freq != samp_freq || app->samp_len != samp_len)
    {
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: Write and read settings are "
            "different\n"));
        return 0;
    }

    return 1;
}

#ifndef CONFIG_FD_AUDIO_LOOPBACK_H2D
static jbool_t fill_check_buf(stream_info_t *stream_info, jbool_t fill, 
    jdma_handle dma, juint32_t len)
{
    juint32_t i;

    for (i = 0; i < len; i++, stream_info->xfered++)
    {
        juint32_t count = stream_info->xfered;
        juint8_t x = count >> ((count & 3) * 8);

        if (fill)
        {
            *(juint8_t *)jdma_virt_addr(dma, i) = x;
        }
        else if (*(juint8_t *)jdma_virt_addr(dma, i) != x)
        {
            DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: ERROR DATA, xfered=%d, "
                "expected=%d received=%d\n", stream_info->xfered, x,
                *(juint8_t *)jdma_virt_addr(dma, i)));
            return 0;
        }
    }
    return 1;
}

static void start_io(app_ctx_h app, uwd_audio_stream_h stream, 
    stream_info_t *stream_info, jdma_handle dma);

static void complete_cb(uwd_audio_app_h app_ctx, uwd_audio_stream_h stream,
    void *arg, jresult_t stat, juint32_t len)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    jdma_handle dma = (jdma_handle)arg;
    stream_info_t *stream_info = stream->read ? &app->read : &app->write;

    if (!stream_info->started)
        goto Error;

    if (stat)
    {
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: read_cb error, stat=%d\n", stat));
        goto Error;
    }

    if (stream->read && !fill_check_buf(stream_info, 0, dma, len))
        goto Error;

    DBG_V(DSLAVE_AUDIO_APP, ("AUDIO_APP: complete_cb, %s len=%d(%d)\n", 
        stream->read ? "READ" : "WRITE", len, stream_info->xfered));

    start_io(app, stream, stream_info, dma);
    return;
    
Error:
    jdma_free(dma);
}

static void start_io(app_ctx_h app, uwd_audio_stream_h stream, 
    stream_info_t *stream_info, jdma_handle dma)
{
    jresult_t rc;
    juint_t num = dma ? 1 : 2;

    for (rc = 0; num && !rc; num--, dma = NULL)
    {
        if (!dma)
        {
            rc = jdma_alloc(app->buf_len, 0, NULL, NULL, 0, &dma);
            if (rc)
                goto Exit;
        }

        if (stream->read)
        {
            rc = uwd_audio_stream_read(app->fd, stream,
                jdma_virt_addr(dma, 0), app->buf_len, complete_cb, dma);
        }
        else
        {
            fill_check_buf(stream_info, 1, dma, app->buf_len);

            rc = uwd_audio_stream_write(app->fd, stream,
                jdma_virt_addr(dma, 0), app->buf_len, complete_cb, dma);
        }

        if (rc)
            jdma_free(dma);
    }

Exit:
    stream_info->started = !rc;

    if (rc)
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: start_io failed, rc=%d\n", rc));
}

static void stream_select_cb(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt, 
    juint32_t samp_freq)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    stream_info_t *stream_info = stream->read ? &app->read : &app->write;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: %s stream %s\n", 
        stream->read ? "READ" : "WRITE",
        stream_alt ? "ENABLED" : "DISABLED"));

    if (!stream_alt)
    {
        stream_info->started = 0;

        return;
    }

    if (!audio_stream_init(app, stream_alt, samp_freq))
        return;

    stream_info->xfered = 0;
    start_io(app, stream, stream_info, NULL);
}

#else
/* Host to Device */

static buf_desc_h alloc_buf(app_ctx_h app)
{
    jresult_t rc;
    buf_desc_h buf = (buf_desc_h)jmalloc(sizeof(buf_desc_t), M_ZERO);

    if (!buf)
        return NULL;

    rc = jdma_alloc(app->buf_len, 0, NULL, NULL, 0, &buf->dma);
    if (rc)
    {
        jfree(buf);
        return NULL;
    }
    
    return buf;
}

static void free_buf(buf_desc_h buf)
{
    jdma_free(buf->dma);
    jfree(buf);
}

static buf_desc_h done_remove_head(app_ctx_h app)
{
    buf_desc_h buf = STAILQ_FIRST(&app->done);

    if (buf)
    {
        app->buffered -= BUFFERING_TIME_MS;
        STAILQ_REMOVE_HEAD(&app->done, next);
    }

    return buf;
}

static void done_add_tail(app_ctx_h app, buf_desc_h buf)
{
    if (app->buffered + BUFFERING_TIME_MS > BUFFERING_LIMIT_MS)
    {
        buf_desc_h old_buf = done_remove_head(app);
        KASSERT(old_buf, ("No buffer, I can't believe!\n"));

        free_buf(old_buf);
    }

    app->buffered += BUFFERING_TIME_MS;
    STAILQ_INSERT_TAIL(&app->done, buf, next);
}

static void done_free_all(app_ctx_h app)
{
    buf_desc_h buf;

    DBG_V(DSLAVE_AUDIO_APP, ("AUDIO_APP: done_free_all (%d)\n", app->buffered));

    while ((buf = STAILQ_FIRST(&app->done)) != NULL)
    {
        STAILQ_REMOVE_HEAD(&app->done, next);
        free_buf(buf);
    }
    app->buffered = 0;
}

static void read_start(app_ctx_h app, uwd_audio_stream_h stream, juint_t num);
static void write_reschedule(app_ctx_h app);

static void write_cb(uwd_audio_app_h app_ctx, uwd_audio_stream_h stream,
    void *arg, jresult_t stat, juint32_t len)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    buf_desc_h buf = (buf_desc_h)arg;

    if (stat)
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: write_cb error, stat=%d\n", stat));
    else
        DBG_X(DSLAVE_AUDIO_APP, ("AUDIO_APP: write_cb complete, len=%d\n",len));

    free_buf(buf);

    if (app->write.started)
        write_reschedule(app);
}

static void write_reschedule(app_ctx_h app)
{
    buf_desc_h buf;
    jresult_t rc = 0;

    while ((buf = done_remove_head(app)) != NULL)
    {
        rc = uwd_audio_stream_write(app->fd, app->wstream, 
            jdma_virt_addr(buf->dma, 0), buf->len, write_cb, buf);
        if (rc)
        {
            DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: Write error, rc=%d\n", rc));
            free_buf(buf);
            return;
        }
    }
}

static void read_cb(uwd_audio_app_h app_ctx, uwd_audio_stream_h stream,
    void *arg, jresult_t stat, juint32_t len)
{
    app_ctx_h app = (app_ctx_h)app_ctx;
    buf_desc_h buf = (buf_desc_h)arg;

    if (!app->read.started)
        stat = JENXIO;

    if (stat)
        goto Error;

    DBG_V(DSLAVE_AUDIO_APP, ("AUDIO_APP: read_cb, len=%d\n", len));

    buf->len = len;
    done_add_tail(app, buf);

    if (app->write.started)
        write_reschedule(app);
    
    read_start(app, stream, 1);
    
    return;
Error:
    DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: read_cb error, stat=%d\n", stat));
    free_buf(buf);
}

static void read_start(app_ctx_h app, uwd_audio_stream_h stream, juint_t num)
{
    jresult_t rc = 0;
    juint_t i;

    for (i = 0; i < num; i++)
    {
        buf_desc_h buf = alloc_buf(app);
        if (!buf)
        {
            rc = JENOMEM;
            goto Exit;
        }

        rc = uwd_audio_stream_read(app->fd, stream, jdma_virt_addr(buf->dma, 0),
            app->buf_len, read_cb, buf);
        if (rc)
        {
            free_buf(buf);
            goto Exit;
        }
    }

Exit:
    if (rc)
        DBG_E(DSLAVE_AUDIO_APP, ("AUDIO_APP: read_start failed, rc=%d\n", rc));
}

static void stream_select_cb(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt, 
    juint32_t samp_freq)
{
    app_ctx_h app = (app_ctx_h)app_ctx;

    DBG_I(DSLAVE_AUDIO_APP, ("AUDIO_APP: %s stream %s\n", 
        stream->read ? "READ" : "WRITE", stream_alt ? "ENABLED" : "DISABLED"));

    if (!stream_alt)
    {
        if (stream->read)
        {
            app->read.started = 0;
        }
        else
        {
            app->write.started = 0;
            done_free_all(app);
        }

        return;
    }

    if (!audio_stream_init(app, stream_alt, samp_freq))
        return;

    if (stream->read)
    {
        app->read.started = 1;
        read_start(app, stream, 2);
    }
    else
    {
        app->write.started = 1;
        write_reschedule(app);
    }
}
#endif

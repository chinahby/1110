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

/**
 * File: audio_loopback_sample.c
 * Sample loopback application, which reads data from an input audio device and
 * writes it to an output audio device, using the Audio Class Driver.
 *
 * Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com
 **/

#include <uw_host_audio.h>
#include <jos.h> 

#define LOOPBACK_DEVS_NUM 2
#define LOOPBACK_START_TIMEOUT 3000 /* msec */

typedef struct audio_loopback *audio_loopback_h;

typedef struct {
    audio_loopback_h lb;
    void *buf;
} loopback_req_t;

typedef struct {
    uwh_audio_app_h       dev;
    uwh_audio_entity_t        *entity;
    uwh_audio_stream_iface_t  *sif;
    juint32_t              freq;
} stream_info_t;

typedef struct audio_loopback {
    stream_info_t read;
    stream_info_t write;

    jbool_t          buffered;

    loopback_req_t *first;
    juint32_t        first_len;

    struct {
        juint32_t  received;
        juint32_t  sent;
        juint32_t  active_reqs;
    } info;
} audio_loopback_t;

static jresult_t loopback_start(stream_info_t *read, stream_info_t *write,
    juint32_t buf_time, audio_loopback_h *lb_handle);
static void loopback_stop(audio_loopback_h lb);

typedef struct app_priv {
    uwh_audio_drv_h dev[LOOPBACK_DEVS_NUM];
    juint_t dev_num;

    jtask_h task;
    
    audio_loopback_h lb;

    juint8_t flags;
#define LOOPBACK_TASK_SCHEDULED  0x01
#define LOOPBACK_DYING           0x04
} app_priv_t;

static app_priv_t sample_app;
static uwh_app_h app_h = NULL;

static void write_cb(void *arg, jresult_t status)
{
    loopback_req_t *req = (loopback_req_t *)arg;
    audio_loopback_h lb = req->lb;

    DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: write_cb\n"));

    DBG_IF(status && status != JEIO)
    {
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: Writing error=%d\n", status));
    }

    lb->info.active_reqs--;
    
    if (!lb->info.active_reqs)
        lb->buffered = FALSE;

    DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: Completed req %p buf %p\n", req,
        req->buf));

    jfree(req->buf);
    jfree(req);
}

static void read_cb(void *arg, jresult_t status)
{
    audio_loopback_h lb = (audio_loopback_h)arg;
    juint32_t buf_len;
    jresult_t rc;
    loopback_req_t *req;

    DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: read_cb\n"));

    if (lb->info.active_reqs > 10)
    {
        /* TODO clear data from read stream */
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: WARNING too many active xfers (%d),"
            " skipping buffer\n", lb->info.active_reqs));
    }

    if (status)
    {
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: Reading error=%d\n", status));
        return;
    }

    buf_len = uwh_audio_read_get_data_size(lb->read.entity);
    if (!buf_len)
        return;

    req = (loopback_req_t *)jmalloc(sizeof(loopback_req_t), M_ZERO);
    if (!req)
    {
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: No memory\n"));
        return;
    }
    req->lb = lb;

    req->buf = jmalloc(buf_len, 0);
    if (!req->buf)
    {
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: No memory\n"));
        goto Error;
    }

    DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: Created req %p buf %p\n", req,
        req->buf));

    rc = uwh_audio_read_get_data(lb->read.entity, req->buf, buf_len);
    if (rc)
    {
        DBG_E(DHOST_AUDIO_APP,("AUDIO_APP: uwh_audio_read_get_data failed, "
            "rc=%d\n", rc));
        goto Error;
    }

    lb->info.received += buf_len;

    if (!lb->buffered)
    {
        if (!lb->first)
        {
            /* bufferring */
            DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: Buffering req %p buf %p\n", req,
                req->buf));
            lb->first = req;
            lb->first_len = buf_len;
            return;            
        }
        else
        {
            rc = uwh_audio_write_data(lb->write.entity, lb->first->buf,
                lb->first_len, write_cb, lb->first);
            
            if (rc)
            {
                DBG_E(DHOST_AUDIO_APP,("AUDIO_APP: uwh_audio_write_data failed,"
                    " rc=%d\n", rc));

                jfree(lb->first->buf);
                jfree(lb->first);
                
                lb->first = NULL;
                
                goto Error;
            }

            lb->buffered = TRUE;
            lb->first = NULL;
            
            lb->info.sent += lb->first_len;
            lb->info.active_reqs++;
        }
    }

    rc = uwh_audio_write_data(lb->write.entity, req->buf, buf_len, write_cb, 
        req);
    if (rc)
    {
        DBG_E(DHOST_AUDIO_APP,("AUDIO_APP: uwh_audio_write_data failed, "
            "rc=%d\n", rc));
        goto Error;
    }

    lb->info.sent += buf_len;
    lb->info.active_reqs++;

    return;

Error:
    if (req)
    {
        if (req->buf)
            jfree(req->buf);

        jfree(req);
    }
}

#ifdef JDEBUG
void print_stream_info(char *name, stream_info_t *si)
{
    /* TODO Format II */
    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: %s trm=%d freq=%d Hz "
        "format=%x channnels=%d sub_frame=%d bits=%d\n",
        name,
        si->entity->entity_id,
        si->freq,
        si->sif->format.type_I->bFormatType,
        si->sif->format.type_I->bNrChannels,
        si->sif->format.type_I->bSubframeSize,
        si->sif->format.type_I->bBitResolution));
}
#endif

static jresult_t stream_setup(stream_info_t *si, jbool_t read)
{
    jresult_t rc;

    DBG_RUN(DHOST_AUDIO_APP, DL_VERBOSE, print_stream_info(
        read ? "Setup read" : "Setup write", si));

    rc = uwh_audio_set_stream(si->dev, si->entity, si->sif);
    if (rc)
    {
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: uwh_audio_set_stream for %s failed,"
            " error=%d\n", read ? "read" : "write", rc));
        return rc;
    }

    rc = uwh_audio_samp_freq_req(si->dev, si->entity, UWH_ARC_SET_CUR,
        &si->freq);
    if (rc == JENOTSUP)
        rc = 0;

    DBG_IF(rc)
    {
        DBG_W(DHOST_AUDIO_APP, ("AUDIO_APP: uwh_audio_samp_freq_req (%d Hz) "
            "for %s failed, error=%d\n", si->freq, read ? "read" : "write", 
            rc));
    }

    return rc;
}

static jresult_t loopback_start(stream_info_t *read, stream_info_t *write,
    juint32_t buf_time, audio_loopback_h *lb_handle)
{
    audio_loopback_h lb;
    jresult_t rc;

    KASSERT(lb_handle, ("NULL argument in loopback_start\n"));

    lb = (audio_loopback_h)jmalloc(sizeof(audio_loopback_t), M_ZERO);
    if (!lb)
        return JENOMEM;

    j_memcpy(&lb->read, read, sizeof(stream_info_t));
    j_memcpy(&lb->write, write, sizeof(stream_info_t));

    rc = stream_setup(read, TRUE);
    if (rc)
        goto Error;

    rc = stream_setup(write, FALSE);
    if (rc)
        goto Error;

    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Start streaming\n"));

    rc = uwh_audio_read_start(read->entity, buf_time, read_cb, lb);
    if (rc)
    {
        DBG_E(DHOST_AUDIO_APP,("AUDIO_APP: uwh_audio_read_start failed, "
            "rc=%d\n", rc));
        goto Error;
    }

    *lb_handle = lb;
    
    return 0;

Error:
    jfree(lb);
    return rc;
}

static void loopback_stop(audio_loopback_h lb)
{
    uwh_audio_stream_abort(lb->read.dev, lb->read.entity);
    uwh_audio_stream_abort(lb->write.dev, lb->write.entity);
    jfree(lb);
}

static jbool_t set_samp_supported(uwh_audio_stream_iface_t *sif)
{
    return !!(sif->ep_audio_desc->bmAttributes & 
        (1 << (UWH_AEP_SAMPLING_FREQ_CONTROL - 1)));
}

static jbool_t find_sif(stream_info_t *target, stream_info_t *result, 
    juint_t sif_num)
{
    for (; sif_num; sif_num--, result->sif++)
    {
        uwh_audio_format_I_desc_t *result_desc;
        uwh_audio_format_I_desc_t *target_desc;
        juint32_t t_low = 0, t_high = 0;

        if (target->sif->format_type != result->sif->format_type)
            continue;

        /* FIXME Only format type I currently supported */
        result_desc = result->sif->format.type_I;
        target_desc = target->sif->format.type_I;

        if (target_desc->bNrChannels != result_desc->bNrChannels)
            continue;

        if (target_desc->bSubframeSize != result_desc->bSubframeSize)
            continue;

        if (target_desc->bBitResolution != result_desc->bBitResolution)
            continue;

        /* TODO continious freq range */
        if (!target_desc->bSamFreqType) /* continious */
        {
            t_low = uwh_audio_samp_freq_to_uint32(
                (uwh_audio_samp_freq_t*)&target_desc->tSamFreq[0]);
            t_high = uwh_audio_samp_freq_to_uint32(
                (uwh_audio_samp_freq_t*)&target_desc->tSamFreq[1]);
        }

        if (!result_desc->bSamFreqType) /* continious result */
        {
            juint32_t r_low, r_high;
            
            r_low  = uwh_audio_samp_freq_to_uint32(
                (uwh_audio_samp_freq_t*)&result_desc->tSamFreq[0]);
            r_high = uwh_audio_samp_freq_to_uint32(
                (uwh_audio_samp_freq_t*)&result_desc->tSamFreq[1]);

            if (!target_desc->bSamFreqType) /* continious target & result */
            {
                if (r_high >= t_low && t_high >= r_low)
                {
                    result->freq = MAX(t_low, r_low);
                    return TRUE;
                }
            }
            else /* continious result & discrete target */
            {
                if (r_low <= target->freq && target->freq <= r_high)
                {
                    result->freq = target->freq;
                    return TRUE;
                }
            }
        }
        else /* discrete result */
        {
            juint_t f, max;
            max = set_samp_supported(result->sif) ? 
                result_desc->bSamFreqType : 1;

            for (f = 0; f < max; f++)
            {
                juint32_t low, high;

                result->freq = 
                    uwh_audio_samp_freq_to_uint32(
                        (uwh_audio_samp_freq_t*)&result_desc->tSamFreq[f]);

                DBG_RUN(DHOST_AUDIO_APP, DL_VERBOSE, print_stream_info(
                    "Trying - ", result));

                if (target_desc->bSamFreqType) /* discrete target & result */
                {
                    if (result->freq == target->freq)
                        return TRUE;
                    
                    continue;
                }
                
                /* continious target & discrete result */
                
                low = uwh_audio_samp_freq_to_uint32(
                    (uwh_audio_samp_freq_t*)&target_desc->tSamFreq[0]);
                high = uwh_audio_samp_freq_to_uint32(
                    (uwh_audio_samp_freq_t*)&target_desc->tSamFreq[1]);

                DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: freq=%d..%d Hz\n", low, 
                    high));

                if (low <= result->freq && result->freq <= high)
                    return TRUE;
            }
        }
    }

    return FALSE;
}


static jbool_t find_terminal(app_priv_t *app, jint_t *start_dev_i, 
    jint_t skip_dev_i, uwh_audio_entity_t *start_entity, stream_info_t *target, 
    uwh_audio_entity_type_t type, stream_info_t *result)
{
    juint_t i;

    DBG_X(DHOST_AUDIO_APP,("AUDIO_APP: find_terminal (start_dev_i=%d, "
        "skip_dev_i=%d, start_entity=%p, target=%d, result=%d)\n\n",
        start_dev_i ? *start_dev_i : -1, skip_dev_i, start_entity, target,
        result));

    for (i = start_dev_i ? *start_dev_i : 0; i < app->dev_num; i++)
    {
        uwh_audio_entity_t *e;

        if (i == skip_dev_i)
            continue;

        if (start_dev_i)
            (*start_dev_i) = i;

        result->dev = app->dev[i];

        /* uwh_audio_get_next_entity returns first entity if second argument 
         * is NULL */
        for (e = uwh_audio_get_next_entity(result->dev, start_entity);
            e; 
            e = uwh_audio_get_next_entity(result->dev, e))
        {
            juint_t s_num;
            enum uwh_audio_terminal_type trm_type;

            result->entity = e;

            if (e->type != type)
                continue;

            if (type == UWH_ACD_INPUT_TERMINAL)
            {
                trm_type = (enum uwh_audio_terminal_type)
                    UGETW(e->u.in_trm->terminal_desc->wTerminalType);
                result->sif = e->u.in_trm->sif;
                s_num = e->u.in_trm->sif_num;
            }
            else
            {
                trm_type = (enum uwh_audio_terminal_type)
                    UGETW(e->u.out_trm->terminal_desc->wTerminalType);
                result->sif = e->u.out_trm->sif;
                s_num = e->u.out_trm->sif_num;
            }

            if (trm_type != UWH_ATT_USB_STREAMING)
                continue;

            if (!target)
                return TRUE;

            if (find_sif(target, result, s_num))
                return TRUE;
        }
        result->entity = NULL;
        start_entity = NULL;
    }

    if (start_dev_i)
        (*start_dev_i) = i;

    return FALSE;
}

static jbool_t find_io_trm(app_priv_t *app, stream_info_t *read, 
    stream_info_t *write)
{
    uwh_audio_entity_t *start_entity;
    jint_t start_dev_i;

    for (start_dev_i = 0, start_entity = NULL;
        find_terminal(app, &start_dev_i, -1, start_entity, NULL, 
        UWH_ACD_OUTPUT_TERMINAL, read);
        start_entity = read->entity)
    {
        juint_t sif_num;

        for (sif_num = 0; sif_num < read->entity->u.out_trm->sif_num; sif_num++)
        {
            juint_t freq_i, max;

            read->sif = &read->entity->u.out_trm->sif[sif_num];

            /* TODO check if format type I */
            if (!read->sif->format.type_I->bSamFreqType)
            {
                if (find_terminal(app, NULL, (app->dev_num > 1) ? start_dev_i : 
                    -1, NULL, read, UWH_ACD_INPUT_TERMINAL, write))
                {
                    return TRUE;
                }
                continue;
            }

            max = set_samp_supported(read->sif) ?
                read->sif->format.type_I->bSamFreqType : 1;

            for (freq_i = 0; freq_i < max; freq_i++)
            {
                read->freq = uwh_audio_samp_freq_to_uint32(
                    (uwh_audio_samp_freq_t*)
                    &read->sif->format.type_I->tSamFreq[freq_i]);

                DBG_RUN(DHOST_AUDIO_APP, DL_VERBOSE, print_stream_info(
                    "Trying to find OUT terminal matching to", read));

                if (find_terminal(app, NULL, (app->dev_num > 1) ? start_dev_i : 
                    -1, NULL, read, UWH_ACD_INPUT_TERMINAL, write))
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

static void loopback_task(void *app_ctx)
{
    app_priv_t *app = (app_priv_t *)app_ctx;
    stream_info_t read, write;

    DBG_V(DHOST_AUDIO_APP, ("AUDIO_APP: %d devices attached, find i/o "
        "terminals\n", app->dev_num));

    app->flags &= ~LOOPBACK_TASK_SCHEDULED;

    if (app->flags & LOOPBACK_DYING)
        return;

    if (!find_io_trm(app, &read, &write))
    {
        DBG_E(DHOST_AUDIO_APP,("AUDIO_APP: Can't find matching terminals\n"));
        return;
    }

    /* Start streaming with 50 ms buffering */
    loopback_start(&read, &write, 50, &app->lb);
}

static void audio_loopback_detach(uwh_audio_app_h app_ctx);

static jresult_t audio_loopback_attach(uwh_audio_drv_h drv_ctx, 
    uwh_audio_app_h *app_ctx)
{
    jresult_t rc = JEINVAL;
    app_priv_t *app;

    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Attach drv_ctx=%p\n", drv_ctx));
    KASSERT(app_ctx, ("AUDIO_APP: app_ctx is NULL\n"));

    app = &sample_app;

    if (!app->task)
    {
        rc = jtask_init(&app->task, TASK_CLASS_DRIVER);
        if (rc)
            goto Error;
    }

    if (app->dev_num == LOOPBACK_DEVS_NUM)
    {
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: Attach failed - too many "
            "audio devices\n"));
        return JEBUSY;
    }

    *app_ctx = app;

    app->dev[app->dev_num++] = drv_ctx;

    DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: new audio device (%d), drv_ctx=%p\n", 
        app->dev_num, drv_ctx));

    if (app->lb)
    {
        DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: stop streaming\n"));
        loopback_stop(app->lb);
        app->lb = NULL;
    }

    if (app->flags & LOOPBACK_TASK_SCHEDULED)
        return 0;

    app->flags |= LOOPBACK_TASK_SCHEDULED;

    jtask_schedule(app->task, LOOPBACK_START_TIMEOUT, loopback_task, app);

    return 0;

Error:
    audio_loopback_detach(app);

    return rc;
}

static void audio_loopback_detach(uwh_audio_app_h app_ctx)
{
    app_priv_t *app = (app_priv_t *)app_ctx;
    /* All transfers already aborted by audio class driver,
     * application need only free it's context*/

    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Detach app_ctx=%p\n", app_ctx));
    
    app->flags |= LOOPBACK_DYING;
    
    if (app->lb)
        loopback_stop(app->lb);

    if (app->task)
        jtask_uninit(app->task);

    jos_memset(app, 0, sizeof(app_priv_t));
}

static void audio_notify(uwh_audio_app_h app_ctx, 
    uwh_audio_notify_info_t *info)
{
}

static juint_t audio_loopback_probe(devmon_info_t *desc)
{
    return 1;
}

static uwh_audio_callbacks_t audio_cbs = {
    audio_loopback_probe,
    audio_loopback_attach,
    audio_loopback_detach,
    audio_notify
};

jresult_t audio_loopback_init(void)
{
    app_h = uwh_audio_register(&audio_cbs);
    return app_h ? 0 : EUNKNOWN;
}

void audio_loopback_uninit(void)
{
    if (app_h)
        uwh_audio_unregister(app_h);
}


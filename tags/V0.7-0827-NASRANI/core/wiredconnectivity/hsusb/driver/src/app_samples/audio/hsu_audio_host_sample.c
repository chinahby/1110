/*==============================================================================
Digital Audio over High Speed USB - host sample application

GENERAL DESCRIPTION
Sample loopback application, which reads data from an input audio device and
writes it to an output audio device, using the Audio Class Driver.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when      who     what, where, why
--------  ---     ------------------------------------------------------
05/27/08  merez     First revision of file

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include <uw_host_audio.h>
#include <jos.h> 

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#define HSU_AUDIO_LOOPBACK_DEVS_NUM 2
#define HSU_AUDIO_LOOPBACK_START_TIMEOUT 3000 /* msec */

/*==============================================================================
Typedefs
==============================================================================*/
typedef struct hsu_audio_loopback *audio_loopback_h;

typedef struct {
    audio_loopback_h lb;
    void *buf;
} hsu_loopback_req_t;

typedef struct {
    uwh_audio_app_h       dev;
    uwh_audio_entity_t        *entity;
    uwh_audio_stream_iface_t  *sif;
    juint32_t              freq;
} hsu_stream_info_t;

typedef struct hsu_audio_loopback {
    hsu_stream_info_t read;
    hsu_stream_info_t write;

    jbool_t           buffered;

    hsu_loopback_req_t *first;
    juint32_t          first_len;

    struct {
        juint32_t  received;
        juint32_t  sent;
        juint32_t  active_reqs;
    } info;
} hsu_audio_loopback_t;

/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
static jresult_t hsu_audio_host_sample_loopback_start(hsu_stream_info_t *read, hsu_stream_info_t *write,
    juint32_t buf_time, audio_loopback_h *lb_handle);
static void hsu_audio_host_sample_loopback_stop(audio_loopback_h lb);

typedef struct app_priv {
    uwh_audio_drv_h dev[HSU_AUDIO_LOOPBACK_DEVS_NUM];
    juint_t dev_num;

    jtask_h task;
    
    audio_loopback_h lb;

    juint8_t flags;
#define LOOPBACK_TASK_SCHEDULED  0x01
#define LOOPBACK_DYING           0x04
} hsu_audio_host_sample_app_priv_t;

static hsu_audio_host_sample_app_priv_t sample_app;
static uwh_app_h app_h = NULL;

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================

FUNCTION hsu_audio_host_sample_write_cb

DESCRIPTION
Callback function to be called when the write operation is completed.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/

static void hsu_audio_host_sample_write_cb(void *arg, jresult_t status)
{
    hsu_loopback_req_t *req = (hsu_loopback_req_t *)arg;
    audio_loopback_h lb = req->lb;

    DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: hsu_audio_host_sample_write_cb\n"));

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

/*===========================================================================

FUNCTION hsu_audio_host_sample_read_cb

DESCRIPTION
Callback function to be called when the read operation is completed.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_host_sample_read_cb(void *arg, jresult_t status)
{
    audio_loopback_h lb = (audio_loopback_h)arg;
    juint32_t buf_len;
    jresult_t rc;
    hsu_loopback_req_t *req;

    DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: hsu_audio_host_sample_read_cb\n"));

    /* In case we have too many active transfers we don't read the buffer. */
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

    req = (hsu_loopback_req_t *)jmalloc(sizeof(hsu_loopback_req_t), M_ZERO);
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

    /* Read the incoming buffer and write it to the loopback write buffer */
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
                lb->first_len, hsu_audio_host_sample_write_cb, lb->first);
            
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

    rc = uwh_audio_write_data(lb->write.entity, req->buf, buf_len, hsu_audio_host_sample_write_cb, 
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
void print_stream_info(char *name, hsu_stream_info_t *si)
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

/*===========================================================================

FUNCTION hsu_audio_host_sample_stream_setup

DESCRIPTION
The function configure the stream (sample frequency etc.)

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static jresult_t hsu_audio_host_sample_stream_setup(hsu_stream_info_t *si, jbool_t read)
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

/*===========================================================================

FUNCTION hsu_audio_host_sample_loopback_start

DESCRIPTION
The function initialize the read and write streams and starts the frames loopback 

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static jresult_t hsu_audio_host_sample_loopback_start(hsu_stream_info_t *read, hsu_stream_info_t *write,
    juint32_t buf_time, audio_loopback_h *lb_handle)
{
    audio_loopback_h lb;
    jresult_t rc;

    KASSERT(lb_handle, ("NULL argument in loopback_start\n"));

    lb = (audio_loopback_h)jmalloc(sizeof(hsu_audio_loopback_t), M_ZERO);
    if (!lb)
        return JENOMEM;

    j_memcpy(&lb->read, read, sizeof(hsu_stream_info_t));
    j_memcpy(&lb->write, write, sizeof(hsu_stream_info_t));

    rc = hsu_audio_host_sample_stream_setup(read, TRUE);
    if (rc)
        goto Error;

    rc = hsu_audio_host_sample_stream_setup(write, FALSE);
    if (rc)
        goto Error;

    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Start streaming\n"));

    rc = uwh_audio_read_start(read->entity, buf_time, hsu_audio_host_sample_read_cb, lb);
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

/*===========================================================================

FUNCTION hsu_audio_host_sample_loopback_stop

DESCRIPTION
The function stops the read and write streams

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_host_sample_loopback_stop(audio_loopback_h lb)
{
    uwh_audio_stream_abort(lb->read.dev, lb->read.entity);
    uwh_audio_stream_abort(lb->write.dev, lb->write.entity);
    jfree(lb);
}

/*===========================================================================

FUNCTION hsu_audio_host_sample_set_samp_supported

DESCRIPTION
The function sets the sample frequency

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static jbool_t hsu_audio_host_sample_set_samp_supported(uwh_audio_stream_iface_t *sif)
{
    return !!(sif->ep_audio_desc->bmAttributes & 
        (1 << (UWH_AEP_SAMPLING_FREQ_CONTROL - 1)));
}

/*===========================================================================

FUNCTION hsu_audio_host_sample_find_sif

DESCRIPTION
The function searches for the stream_iface according to the stream info

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static jbool_t hsu_audio_host_sample_find_sif(hsu_stream_info_t *target, hsu_stream_info_t *result, 
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
            max = hsu_audio_host_sample_set_samp_supported(result->sif) ? 
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

/*===========================================================================

FUNCTION hsu_audio_host_sample_find_terminal

DESCRIPTION
The function searches for the terminal according to the stream info

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static jbool_t hsu_audio_host_sample_find_terminal(hsu_audio_host_sample_app_priv_t *app, jint_t *start_dev_i, 
    jint_t skip_dev_i, uwh_audio_entity_t *start_entity, hsu_stream_info_t *target, 
    uwh_audio_entity_type_t type, hsu_stream_info_t *result)
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

            if (hsu_audio_host_sample_find_sif(target, result, s_num))
                return TRUE;
        }
        result->entity = NULL;
        start_entity = NULL;
    }

    if (start_dev_i)
        (*start_dev_i) = i;

    return FALSE;
}

/*===========================================================================

FUNCTION hsu_audio_host_sample_find_io_trm

DESCRIPTION
The function searches for the terminal according to the stream info

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static jbool_t hsu_audio_host_sample_find_io_trm(hsu_audio_host_sample_app_priv_t *app, hsu_stream_info_t *read, 
    hsu_stream_info_t *write)
{
    uwh_audio_entity_t *start_entity;
    jint_t start_dev_i;

    for (start_dev_i = 0, start_entity = NULL;
        hsu_audio_host_sample_find_terminal(app, &start_dev_i, -1, start_entity, NULL, 
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
                if (hsu_audio_host_sample_find_terminal(app, NULL, (app->dev_num > 1) ? start_dev_i : 
                    -1, NULL, read, UWH_ACD_INPUT_TERMINAL, write))
                {
                    return TRUE;
                }
                continue;
            }

            max = hsu_audio_host_sample_set_samp_supported(read->sif) ?
                read->sif->format.type_I->bSamFreqType : 1;

            for (freq_i = 0; freq_i < max; freq_i++)
            {
                read->freq = uwh_audio_samp_freq_to_uint32(
                    (uwh_audio_samp_freq_t*)
                    &read->sif->format.type_I->tSamFreq[freq_i]);

                DBG_RUN(DHOST_AUDIO_APP, DL_VERBOSE, print_stream_info(
                    "Trying to find OUT terminal matching to", read));

                if (hsu_audio_host_sample_find_terminal(app, NULL, (app->dev_num > 1) ? start_dev_i : 
                    -1, NULL, read, UWH_ACD_INPUT_TERMINAL, write))
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

/*===========================================================================

FUNCTION hsu_audio_host_sample_loopback_task

DESCRIPTION
The function initializes the loopback task

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_host_sample_loopback_task(void *app_ctx)
{
    hsu_audio_host_sample_app_priv_t *app = (hsu_audio_host_sample_app_priv_t *)app_ctx;
    hsu_stream_info_t read, write;

    DBG_V(DHOST_AUDIO_APP, ("AUDIO_APP: %d devices attached, find i/o "
        "terminals\n", app->dev_num));

    app->flags &= ~LOOPBACK_TASK_SCHEDULED;

    if (app->flags & LOOPBACK_DYING)
        return;

    if (!hsu_audio_host_sample_find_io_trm(app, &read, &write))
    {
        DBG_E(DHOST_AUDIO_APP,("AUDIO_APP: Can't find matching terminals\n"));
        return;
    }

    /* Start streaming with 50 ms buffering */
    hsu_audio_host_sample_loopback_start(&read, &write, 50, &app->lb);
}



static void hsu_audio_host_sample_detach(uwh_audio_app_h app_ctx);


/*===========================================================================

FUNCTION hsu_audio_host_sample_attach

DESCRIPTION
Device attach driver method callback.
This callback is called by the USB Core after an attached
device has been successfully matched to the class driver.

DEPENDENCIES
None

RETURN VALUE
0 on success, otherwise an error code.

SIDE EFFECTS
None
===========================================================================*/
static jresult_t hsu_audio_host_sample_attach(uwh_audio_drv_h drv_ctx, 
    uwh_audio_app_h *app_ctx)
{
    jresult_t rc = JEINVAL;
    hsu_audio_host_sample_app_priv_t *app;

    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Attach drv_ctx=%p\n", drv_ctx));
    KASSERT(app_ctx, ("AUDIO_APP: app_ctx is NULL\n"));

    app = &sample_app;

    if (!app->task)
    {
        rc = jtask_init(&app->task, TASK_CLASS_DRIVER);
        if (rc)
            goto Error;
    }

    if (app->dev_num == HSU_AUDIO_LOOPBACK_DEVS_NUM)
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
        hsu_audio_host_sample_loopback_stop(app->lb);
        app->lb = NULL;
    }

    if (app->flags & LOOPBACK_TASK_SCHEDULED)
        return 0;

    app->flags |= LOOPBACK_TASK_SCHEDULED;

    jtask_schedule(app->task, HSU_AUDIO_LOOPBACK_START_TIMEOUT, hsu_audio_host_sample_loopback_task, app);

    return 0;

Error:
    hsu_audio_host_sample_detach(app);

    return rc;
}


/*===========================================================================
FUNCTION hsu_audio_host_sample_detach

DESCRIPTION
Device detach driver method callback.
This callback is called by the USB Core when a device detach
is identified.
The callback must clear all allocated device resources.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_host_sample_detach(uwh_audio_app_h app_ctx)
{
    hsu_audio_host_sample_app_priv_t *app = (hsu_audio_host_sample_app_priv_t *)app_ctx;
    /* All transfers already aborted by audio class driver,
     * application need only free it's context*/

    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Detach app_ctx=%p\n", app_ctx));
    
    app->flags |= LOOPBACK_DYING;
    
    if (app->lb)
        hsu_audio_host_sample_loopback_stop(app->lb);

    if (app->task)
        jtask_uninit(app->task);

    jos_memset(app, 0, sizeof(hsu_audio_host_sample_app_priv_t));
}

/*===========================================================================
FUNCTION hsu_audio_host_sample_notify

DESCRIPTION
Device notify driver method callback.
This callback is called by the USB Core for audio notifications.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_audio_host_sample_notify(uwh_audio_app_h app_ctx, 
    uwh_audio_notify_info_t *info)
{
}


/*===========================================================================
FUNCTION hsu_audio_host_sample_probe

DESCRIPTION
Driver probe driver method callback.
This callback is called by the USB Core in order to match new
attached device with this class driver.

DEPENDENCIES
None

RETURN VALUE
Match level

SIDE EFFECTS
None
===========================================================================*/
static juint_t hsu_audio_host_sample_probe(devmon_info_t *desc)
{
    return 1;
}

static uwh_audio_callbacks_t audio_cbs = {
    hsu_audio_host_sample_probe,
    hsu_audio_host_sample_attach,
    hsu_audio_host_sample_detach,
    hsu_audio_host_sample_notify
};

/*===========================================================================
FUNCTION hsu_audio_host_sample_init

DESCRIPTION
This function initializes the audio host sample application 

DEPENDENCIES
This function should be called after hsu_conf_sel_stack_utils_setup_stack 
has been called.

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
jresult_t hsu_audio_host_sample_init(void)
{
    app_h = uwh_audio_register(&audio_cbs);
    return app_h ? 0 : EUNKNOWN;
}

/*===========================================================================
FUNCTION hsu_audio_host_sample_uninit

DESCRIPTION
This function un-initializes the audio host sample application 

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_audio_host_sample_uninit(void)
{
    if (app_h)
        uwh_audio_unregister(app_h);
}

/* Register the driver with the core:
* Defines hsu_audio_host_sample_init function, and initializes driver_method structure.
*/
/*DRIVER_USB_DECLARE_CUSTOM_INIT(hsu_audio_host_sample, 
                               hsu_audio_host_sample_custom_init, 
                               hsu_audio_host_sample_custom_uninit)*/

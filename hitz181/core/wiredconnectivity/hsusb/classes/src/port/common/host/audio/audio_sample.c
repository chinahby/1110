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
 * File: audio_sample.c
 * Sample application, which demonstrates how to use the Audio Class Driver
 * to read data from an input audio device.
 * The data format used by the sample is PCM (fixed frame).
 *
 * Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com
 **/

#include <uw_host_audio.h>
#include <jos.h> 

typedef struct {
    uwh_audio_drv_h dev;
    uwh_audio_entity_t *trm;
    juint8_t channels;
    juint8_t sub_frame_size;
} app_priv_t;

static uwh_app_h app_h = NULL;

#ifdef JDEBUG
static void print_stream_data(juint8_t sif_num, 
    uwh_audio_stream_iface_t *stream_if)
{
    juint8_t i, i_freq;

    if (!sif_num)
        return;

    jprintf("\tStreaming Interface Alternate Settings - %d\n", sif_num);
    
    for (i = 0; i < sif_num; i++)
    {
        uwh_audio_stream_iface_t *sif = &stream_if[i];

        jprintf("\n\tAlternate Setting - %d\n",  i+1);
        jprintf("\tEndpoint address - %02x\n", sif->ep_desc->bEndpointAddress);
        jprintf("\tDelay %d frame(s)\n",       sif->sif_desc->bDelay);
        jprintf("\tFormat Type %04x (%s)\n",   UGETW(sif->sif_desc->wFormatTag),
            UGETW(sif->sif_desc->wFormatTag) == UWH_ADF_I_PCM ? "PCM" :"");

        if (sif->format_type == UWH_AF_TYPE_I)
        {
            juint8_t sam_freq_type  = sif->format.type_I->bSamFreqType;
            uwh_audio_samp_freq_t *freq = (uwh_audio_samp_freq_t *)
                sif->format.type_I->tSamFreq;

            jprintf("\tAudio Format Type I\n");
            jprintf("\tChannels %d\n",      sif->format.type_I->bNrChannels);
            jprintf("\tSubframe size %d\n", sif->format.type_I->bSubframeSize);
            jprintf("\tBit resolution %d\n",sif->format.type_I->bBitResolution);

            if (!sam_freq_type)
            {
                jprintf("\tContinuous sampling frequency %d Hz - %d Hz\n", 
                    uwh_audio_samp_freq_to_uint32(&freq[0]), 
                    uwh_audio_samp_freq_to_uint32(&freq[1]));
            }
            else
            {
                jprintf("\t%d discrete sampling frequencies supported\n", 
                    sam_freq_type);

                for (i_freq = 0; i_freq < sam_freq_type; i_freq++)
                {
                    jprintf("\t  frequency %d - %d Hz\n", i_freq+1, 
                        uwh_audio_samp_freq_to_uint32(&freq[i_freq]));
                }
            }
        }
    }
}

static char *audio_ctrl_desc_subtype_str[] = {
    "Undefined",                /* 0x00 */
    "Header",                   /* 0x01 */
    "Input terminal",           /* 0x02 */
    "Output terminal",          /* 0x03 */
    "Mixer unit",               /* 0x04 */
    "Selector unit",            /* 0x05 */
    "Feature unit",             /* 0x06 */
    "Processing unit",          /* 0x07 */
    "Extension unit"            /* 0x08 */
};

static char *audio_get_ctrl_desc_subtype_str(
    enum uwh_audio_ctrl_desc_subtype type)
{
    if (type >= sizeof(audio_ctrl_desc_subtype_str)/sizeof(char *))
        return audio_ctrl_desc_subtype_str[0];

    return audio_ctrl_desc_subtype_str[type];
}

static struct {
    char *str;
    juint16_t type;
} audio_terminal_type_str[] = {
    {"USB undefined",                     0x0100},
    {"USB streaming",                     0x0101},
    {"USB vendor specific",               0x01FF},
    {"Input undefined",                   0X0200},
    {"Microphone",                        0X0201},
    {"Desktop microphone",                0X0202},
    {"Personal microphone",               0X0203},
    {"Omni directional microphone",       0X0204},
    {"Microphone array",                  0X0205},
    {"Processing microphone array",       0X0206},
    {"Output undefined",                  0X0300},
    {"Speaker",                           0X0301},
    {"Headphones",                        0X0302},
    {"Head mounted display audio",        0X0303},
    {"Desktop speaker",                   0X0304},
    {"Room speaker",                      0X0305},
    {"Communication speaker",             0X0306},
    {"Low frequency effects speaker",     0X0307},
    {"Undefined", 0}
};

static char *audio_get_terminal_type_str(enum uwh_audio_terminal_type type)
{
    juint_t i;

    for (i = 0; audio_terminal_type_str[i].type && 
        audio_terminal_type_str[i].type != type; i++)   
        ;
    return audio_terminal_type_str[i].str;
}

static void print_entity_data(uwh_audio_entity_t *entity)
{
    jprintf ("AUDIO_APP: ENTITY %d - %s (%p)\n", entity->entity_id,
        audio_get_ctrl_desc_subtype_str(entity->type), entity);

    switch (entity->type)
    {
    case UWH_ACD_INPUT_TERMINAL:

        jprintf("\tTerminal Type - %s (%04x)\n", 
            audio_get_terminal_type_str((enum uwh_audio_terminal_type)
            UGETW(entity->u.in_trm->terminal_desc->wTerminalType)),
            UGETW(entity->u.in_trm->terminal_desc->wTerminalType));

        jprintf("\tbAssocTerminal=%d\n", 
            entity->u.in_trm->terminal_desc->bAssocTerminal);

        print_stream_data(entity->u.in_trm->sif_num, entity->u.in_trm->sif);
        break;

    case UWH_ACD_OUTPUT_TERMINAL:

        jprintf("\tTerminal Type - %s (%04x)\n", 
            audio_get_terminal_type_str((enum uwh_audio_terminal_type)
            UGETW(entity->u.out_trm->terminal_desc->wTerminalType)),
            UGETW(entity->u.out_trm->terminal_desc->wTerminalType));

        jprintf("\tbAssocTerminal=%d\n", 
            entity->u.out_trm->terminal_desc->bAssocTerminal);

        print_stream_data(entity->u.out_trm->sif_num, entity->u.out_trm->sif);
        break;

    default:
        break;
    }
}

#endif

jresult_t feature_control(app_priv_t *app, uwh_audio_entity_t *feature_unit,
    uwh_audio_req_code_t code, juint8_t selector, juint8_t channel, void *buf, 
    juint16_t buf_len)
{
    uwh_audio_ctrl_req_t req;
    jresult_t rc;

    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: feature_control, unit=%d\n", 
        feature_unit->entity_id));
    
    /* Init audio control request */
    req.entity = feature_unit;

    req.type = UWH_AR_CTRL_INTERFACE;
    req.code = code;
    req.ctrl_sel = selector;
    req.value = channel;

    req.buffer  = buf;
    req.buf_len = buf_len;

    /* Send request */
    rc = uwh_audio_ctrl_req(app->dev, &req);

    DBG_IF(rc)
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: feature_control, error=%d\n", rc));
   
    return rc;
}

static jbool_t feature_unit_control_enabled(uwh_audio_entity_t *feature_unit, 
    juint8_t channel, enum uwh_audio_feature_unit_cs control)
{
    uwh_audio_feature_unit_desc_t *desc;
    juint8_t *c;
    
    desc = (uwh_audio_feature_unit_desc_t *)feature_unit->u.unit_desc;

    control--;

    c = (juint8_t *)(desc+1) + desc->bControlSize * channel;
    c += (juint8_t)control >> 3; 

    return 1 && (*c & (1 << (control & 0x7)));
}

static jresult_t init_feature_unit(app_priv_t *app, 
    uwh_audio_entity_t *feature_unit)
{
    uByte bMute;
    uWord wVolume;
    jresult_t rc;
    
    if (feature_unit_control_enabled(feature_unit, 0, UWH_AFU_MUTE_CONTROL))
    {
        rc = feature_control(app, feature_unit, UWH_ARC_GET_CUR, 
            UWH_AFU_MUTE_CONTROL, 0, &bMute, sizeof(bMute));
        if (!rc)
        {
            DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Feature Unit (%d), GET_CUR "
                "MUTE_CONTROL %d\n", feature_unit->entity_id, bMute));
        }
    }


    if (feature_unit_control_enabled(feature_unit, 0, UWH_AFU_VOLUME_CONTROL))
    {
        rc = feature_control(app, feature_unit, UWH_ARC_GET_CUR, 
            UWH_AFU_VOLUME_CONTROL, 0, wVolume, sizeof(wVolume));
        if (!rc)
        {
            DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Feature Unit (%d), GET_CUR "
                "VOLUME_CONTROL %d.%04u db\n", feature_unit->entity_id, 
                (jint16_t)(UGETW(wVolume))>>8, 
                (UGETW(wVolume) & 0xff)*10000/256));
        }
    }

    return 0;
}

static void read_cb(void *arg, jresult_t status)
{
    app_priv_t *app = (app_priv_t *)arg;
    juint32_t buf_size;
    jresult_t rc;
    void *buf = NULL;

    DBG_X(DHOST_AUDIO_APP, ("AUDIO_APP: read_cb\n"));

    if (status)
    {
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: Reading error=%d\n", status));
        return;
    }

    buf_size = uwh_audio_read_get_data_size(app->trm);
    if (!buf_size)
        return;

    buf = jmalloc(buf_size, 0);
    if (!buf)
    {
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: No memory\n"));
        return;
    }

    rc = uwh_audio_read_get_data(app->trm, buf, buf_size);
    if (rc)
    {
        DBG_E(DHOST_AUDIO_APP,("AUDIO_APP: uwh_audio_read_get_data failed, "
            "rc=%d\n", rc));
        goto Exit;
    }

    /* TODO Application data processing */

Exit:
    if (buf)
        jfree(buf);
}

static void audio_sample_detach(uwh_audio_app_h app_ctx)
{
    /* All transfers already aborted by audio class driver,
     * application need only free it's context*/

    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Detach app_ctx=%p\n", app_ctx));

    jfree(app_ctx);
}

static jresult_t audio_sample_attach(uwh_audio_drv_h drv_ctx, 
    uwh_audio_app_h *app_ctx)
{
    app_priv_t *app;
    jresult_t rc = JEINVAL;
    uwh_audio_entity_t *entity;

    DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Attach drv_ctx=%p\n", drv_ctx));
    KASSERT(app_ctx, ("AUDIO_APP: app_ctx is NULL\n"));

    app = (app_priv_t *)jmalloc(sizeof(app_priv_t), M_ZERO);
    if (!app)
        return JENOMEM;

    app->dev = drv_ctx;

    *app_ctx = app;

    for (entity = uwh_audio_get_first_entity(app->dev); entity; 
        entity = uwh_audio_get_next_entity(app->dev, entity))
    {
        DBG_RUN(DHOST_AUDIO_APP, DL_EX_VERBOSE,  print_entity_data(entity));

        if (entity->type == UWH_ACD_FEATURE_UNIT)
            init_feature_unit(app, entity);
        
        /* We are going to read audio stream data, so we need Audio Device
         * Output Terminal with terminal type - USB Streaming */
        if (entity->type == UWH_ACD_OUTPUT_TERMINAL && 
            UGETW(entity->u.out_trm->terminal_desc->wTerminalType) == 
            UWH_ATT_USB_STREAMING)
        {
            uwh_audio_output_terminal_t *otrm = entity->u.out_trm;
            juint32_t freq;
            app->trm = entity;

            /* Read audio streaming info */
            app->channels = otrm->sif[0].format.type_I->bNrChannels;

            app->sub_frame_size = otrm->sif[0].format.type_I->bSubframeSize;

            uwh_audio_set_stream(app->dev, app->trm, &otrm->sif[0]);
            
            freq = uwh_audio_samp_freq_to_uint32((uwh_audio_samp_freq_t *)
                &otrm->sif[0].format.type_I->tSamFreq[0]);

            /* Set sampling frequency to first */
            uwh_audio_samp_freq_req(app->dev, app->trm, UWH_ARC_SET_CUR, &freq);

            /* Start reading with 20 ms buffer */
            rc = uwh_audio_read_start(app->trm, 20, read_cb, app);
            goto Exit;
        }
    }

    DBG_E(DHOST_AUDIO_APP,("AUDIO_APP: Attach failed, can't find USB output "
        "terminal\n"));

Exit:
    if (rc)
    {
        DBG_E(DHOST_AUDIO_APP, ("AUDIO_APP: Attach failed, error=%d\n", rc));
        audio_sample_detach(app);
    }
    DBG_IF(!rc)
        DBG_I(DHOST_AUDIO_APP, ("AUDIO_APP: Attach success\n"));

    return rc;
}

static void audio_sample_notify(uwh_audio_app_h app_ctx, 
    uwh_audio_notify_info_t *notify_info)
{
}

static juint_t audio_sample_probe(devmon_info_t *desc)
{
    return 1;
}

static uwh_audio_callbacks_t audio_cbs = {
    audio_sample_probe,
    audio_sample_attach,
    audio_sample_detach,
    audio_sample_notify
};

jresult_t audio_sample_init(void)
{
    app_h = uwh_audio_register(&audio_cbs);
    return app_h ? 0 : EUNKNOWN;
}

void audio_sample_uninit(void)
{
    if (app_h)
        uwh_audio_unregister(app_h);
}


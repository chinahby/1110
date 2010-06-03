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
#ifndef __JAUDIO_H__
#define __JAUDIO_H__

typedef __PACKED_PRE__ struct {
    uByte bLength;
    uByte bDescriptorType;
    uByte bDescriptorSubtype;
    uByte bEntityID;
} __PACKED_POST__ generic_entity_desc_t;

struct audio_unit;
struct jaudio_softc;
struct audio_stream;

#define AUDIO_STREAM_MIN_FRAMES         20
#define AUDIO_STREAM_MIN_BUFS_NUM       2
#define AUDIO_STREAM_DELIM_MIN_BUFS_NUM 3

typedef void (*audio_stream_cb_t)(void *arg, jresult_t status);

typedef struct audio_xfer {
    usbd_xfer_handle         xfer;
    juint16_t                 *fr_len;

    struct audio_stream      *stream;
    STAILQ_ENTRY(audio_xfer) next; 
} audio_xfer_t, *audio_xfer_h;

typedef struct audio_out_xfer {
    audio_xfer_t      ioxfer;

    audio_stream_cb_t        cb;
    void                     *arg;
    void                     *buffer;
} audio_out_xfer_t, *audio_out_xfer_h;

#define GET_OUT_AXFER(axfer) ((audio_out_xfer_h)(axfer))

typedef struct audio_stream {
    usbd_interface_handle iface;
    usbd_device_handle    dev;
    usbd_pipe_handle      pipe;
    juint8_t               flags;
#define AUDIO_STREAM_USE_DELIMITER   0x01
#define AUDIO_STREAM_STARTED         0x02
#define AUDIO_STREAM_CLASS_TASK      0x04
#define AUDIO_STREAM_CORE_TASK       0x08
#define AUDIO_STREAM_IGNORE_EIO      0x10
#define AUDIO_STREAM_TRY_START       0x20
#define AUDIO_STREAM_IN              0x40

    jtask_h              task;

} audio_stream_t, *audio_stream_h;

#define STREAM_IS_IN(stream) \
    (((audio_stream_h)(stream))->flags & AUDIO_STREAM_IN)

typedef struct audio_in_stream {
    audio_stream_t      iostream;

    audio_stream_cb_t   cb;
    void                *arg;

    juint_t              bufs_num;
    juint_t              frames_in_buf;

    struct {
        STAILQ_HEAD(free_in, audio_xfer) free;
        STAILQ_HEAD(done_in, audio_xfer) done;
        juint_t                          active;
    } axfers;

    juint32_t            done_frame;
    juint16_t            done_in_frame;

    jresult_t            error;

} audio_in_stream_t, *audio_in_stream_h;

#define GET_IN_STREAM(s) ((audio_in_stream_h)(s))

typedef struct audio_out_stream {
    audio_stream_t      iostream;

    juint16_t            audio_frame_size;
    juint32_t            sam_freq;
    juint16_t            isoc_freq;
    juint8_t             frame_count;

    audio_xfer_h        rem_axfer;
    juint16_t            rem_axfer_len;

    juint_t              active_axfers;
    STAILQ_HEAD(done_out, audio_xfer) done_axfers;

    struct {
        juint32_t sum;
        juint32_t div;
        juint32_t rem;
    } average;

} audio_out_stream_t, *audio_out_stream_h;

#define GET_OUT_STREAM(s) ((audio_out_stream_h)(s))

typedef void (*unit_uninit_t)(struct jaudio_softc *sc, struct audio_unit *unit);

typedef struct audio_unit {
    uwh_audio_entity_t    entity;

    struct audio_unit     *next;
    generic_entity_desc_t *desc;
    unit_uninit_t         uninit_func;
    
    /* Terminal specific */
    struct jaudio_softc   *sc;
    juint8_t               stream_iface; 
    juint8_t               act_sif; 
    audio_stream_h        stream;    
    juint32_t              sam_freq;
} audio_unit_t;

#define GET_UNIT(entity) ((audio_unit_t *)(entity))
#define GET_ENTITY(unit) ((uwh_audio_entity_t *)(unit))

typedef struct {
    void *terminal_desc;
    juint8_t sif_num;
    uwh_audio_stream_iface_t *sif;
} generic_terminal_t;

#define AUDIO_INTR_BUF_LEN  2

/* JAUDIO software context */
typedef struct jaudio_softc {
    jdevice_t             sc_dev;  /* base device */
    usbd_device_handle    dev;
    usbd_interface_handle iface;   /* control interface */

    struct {
        usbd_pipe_handle    pipe;   /* interrupt pipe */
        juint8_t             buf[AUDIO_INTR_BUF_LEN];
    } intr;

    uwh_audio_app_h       app_ctx;

    audio_control_iface_desc_t *header_desc;
    
    audio_unit_t          *units;
    audio_unit_t          **units_map;

    juint8_t               unit_max_id;

    /* Flags */
    juint8_t               flags;
#define AUDIO_APP_ATTACHED   0x01
#define AUDIO_DETACH         0x02

    uwh_audio_callbacks_t *cbs;

} jaudio_softc_t;

#endif
